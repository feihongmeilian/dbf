#include <cstdio>
#include <cstring>
#include <exception>

#ifdef _WIN32
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <stdlib.h>
#include <sys/locking.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include <boost/endian/conversion.hpp>
#include <spdlog/spdlog.h>

#include "dbf/DBFBuffer.hpp"
#include "dbf/DBFFile.h"
#include "dbf/DBFHead.h"
#include "dbf/DBFRecord.h"

namespace dbf {
DBFFile::DBFFile(const std::string &name)
    : name_(name), file_(nullptr), writerPos_(0), readerPos_(0),
      head_(new DBFHead), buf_(new DBFBuffer) {}

DBFFile::~DBFFile() {}

void DBFFile::appendHeadField(const std::string &name, const std::string &type,
                              uint8_t totalLen, uint8_t precisionLen) {
  headFields_.emplace_back();
  headFields_.back().setName(name);
  headFields_.back().setFiledType(type);
  if (type == "D" && totalLen != 8) {
    SPDLOG_WARN("Field desc error type : D, total len ： {}", totalLen);
    totalLen = 8;
  }
  headFields_.back().setTotalLen(totalLen);
  headFields_.back().setPrecisionLen(precisionLen);
}

bool DBFFile::readRecordNumber() {
  int32_t recordNumber = 0;
  if (!read(reinterpret_cast<char *>(&recordNumber), kRecorNumIndex,
            sizeof recordNumber)) {
    return false;
  }
  recordNumber = boost::endian::little_to_native(recordNumber);
  head_->setRecordNumber(recordNumber);
  return true;
}

bool DBFFile::writeRecordNumber() {
  int32_t recordNumber = head_->recordNumber();
  recordNumber = boost::endian::native_to_little(recordNumber);
  if (!write(reinterpret_cast<char *>(&recordNumber), kRecorNumIndex,
             sizeof recordNumber)) {
    return false;
  }
  return true;
}

bool DBFFile::readHead() {
  //读消息头的时候，还不知道一条dbf的长度，故需要手动设置
  head_->setRecordBytes(kFieldLen);
  if (!read(*head_)) {
    SPDLOG_WARN("read dbf head failed");
    return false;
  }

  auto recordLen = head_->headerBytes() - readerPos_;
  buf_->retrieveAll();
  buf_->ensureWritableBytes(recordLen);
  buf_->hasWritten(recordLen);

  if (!read(*buf_)) {
    return false;
  }

  size_t recordNum = (recordLen - 1) / kFieldLen;
  int16_t recordBytes = 1;
  for (size_t index = 0; index < recordNum; ++index) {
    DBFHeadField field;
    field.setReadPos(readerPos_);
    try {
      field.parseFrom(*buf_);
    } catch (const std::exception &ex) {
      SPDLOG_WARN("The field descriptor of the DBF file is parsed incorrectly : {}", ex.what());
      return false;
    }
    checkHeadField(headFields_[index], field);
    headFields_.push_back(field);
    recordBytes += field.totalLen();
    readerPos_ += kFieldLen;
  }
  if (recordBytes != head_->recordBytes()) {
    SPDLOG_WARN("The record length in the dbf file header does not match the sum of lengths of each field.");
    return false;
  }

  char endChar = 0;
  try {
    endChar = buf_->readChar();
  } catch (const std::exception &ex) {
    SPDLOG_WARN("Read header termination identifier failed : {}", ex.what());
    return false;
  }

  if (endChar != kEndHeadFlag) {
    SPDLOG_WARN("File header termination identifier error : {}", endChar);
    return false;
  }
  readerPos_ += 1;
  writerPos_ = readerPos_ + static_cast<size_t>(head_->recordNumber()) *
                                static_cast<size_t>(head_->recordBytes());
  return true;
}

bool DBFFile::writeHead() {
  uint16_t headBytes =
      static_cast<uint16_t>(kFieldLen * (headFields_.size() + 1) + 1);
  head_->setHeaderBytes(headBytes);
  head_->setReadPos(0);
  head_->setVersion(3);

  std::time_t currentTime = std::time(nullptr);
  std::tm *localTime = std::localtime(&currentTime);
  head_->setYear(static_cast<int8_t>(localTime->tm_year));
  head_->setMonth(static_cast<char>(localTime->tm_mon + 1));
  head_->setDay(static_cast<char>(localTime->tm_mday));

  int16_t recordBytes = 1; //第一个字节标记记录是否删除
  for (auto &headField : headFields_) {
    recordBytes += headField.totalLen();
  }
  head_->setRecordBytes(recordBytes);

  buf_->retrieveAll();
  head_->serializeTo(*buf_);
  for (auto &headField : headFields_) {
    headField.serializeTo(*buf_);
  }
  buf_->appendChar(kEndHeadFlag); //写入文件头结束标志
  buf_->appendChar(kEndFileFlag); //写入文件结束标志
  readerPos_ = buf_->readableBytes() - 1;
  writerPos_ = readerPos_;
  return appendWriten(*buf_);
}

bool DBFFile::read(DBFRecord &record) {
  auto recordBytes = head_->recordBytes();
  buf_->retrieveAll();
  buf_->ensureWritableBytes(recordBytes);
  buf_->hasWritten(recordBytes);

  if (!read(*buf_)) {
    return false;
  }

  record.setReadPos(readerPos_);
  try {
    record.parseFrom(*buf_);
  } catch (const std::exception &ex) {
    SPDLOG_WARN("Record parse failed : {}", ex.what());
    return false;
  }
  readerPos_ += recordBytes;
  return true;
}

bool DBFFile::read(const std::shared_ptr<DBFRecord> &record) {
  return read(*record);
}

bool DBFFile::read(const std::list<std::shared_ptr<DBFRecord>> &records) {
  size_t allSize = head_->recordBytes() * records.size();
  buf_->retrieveAll();
  buf_->ensureWritableBytes(allSize);
  buf_->hasWritten(allSize);

  if (!read(*buf_)) {
    return false;
  }

  auto pos = readerPos_;
  for (auto &record : records) {
    record->setReadPos(pos);
    try {
      record->parseFrom(*buf_);
    } catch (const std::exception &ex) {
      SPDLOG_WARN("Record parse failed : {}", ex.what());
      return false;
    }
    pos += head_->recordBytes();
  }
  readerPos_ = pos;
  return true;
}

bool DBFFile::overRead(DBFRecord &record) {
  auto recordBytes = head_->recordBytes();
  buf_->retrieveAll();
  buf_->ensureWritableBytes(recordBytes);
  buf_->hasWritten(recordBytes);

  size_t pos = record.readPos() == 0 ? readerPos_ : record.readPos();
  if (!read(buf_->peek(), static_cast<long>(pos), recordBytes)) {
    return false;
  }

  try {
    record.parseFrom(*buf_);
  } catch (const std::exception &ex) {
    SPDLOG_WARN("Record parse failed : {}", ex.what());
    return false;
  }
  record.setReadPos(pos);
  return true;
}

bool DBFFile::overRead(const std::shared_ptr<DBFRecord> &record) {
  return overRead(*record);
}

bool DBFFile::overRead(const std::list<std::shared_ptr<DBFRecord>> &records) {
  if (records.empty()) {
    return true;
  }

  size_t allSize = head_->recordBytes() * records.size();
  buf_->retrieveAll();
  buf_->ensureWritableBytes(allSize);
  buf_->hasWritten(allSize);

  auto pos = (*records.begin())->readPos();
  if (0 == pos) {
    pos = readerPos_;
  }
  if (!read(buf_->peek(), static_cast<long>(pos), allSize)) {
    return false;
  }

  for (auto &record : records) {
    try {
      record->parseFrom(*buf_);
    } catch (const std::exception &ex) {
      SPDLOG_WARN("Record parse failed : {}", ex.what());
      return false;
    }
    record->setReadPos(pos);
    pos += head_->recordBytes();
  }

  if (readerPos_ < pos) {
    readerPos_ = pos;
  }
  return true;
}

bool DBFFile::overWriten(const DBFRecord &record) {
  buf_->retrieveAll();
  record.serializeTo(*buf_);
  size_t pos = record.readPos();
  if (0 == pos) {
    buf_->appendChar(kEndFileFlag);
    pos = writerPos_;
  }

  if (!write(buf_->peek(), static_cast<long>(pos), buf_->readableBytes())) {
    return false;
  }

  if (record.readPos() == 0) {
    head_->setRecordNumber(head_->recordNumber() + 1);
    if (!writeRecordNumber()) {
      head_->setRecordNumber(head_->recordNumber() - 1);
      return false;
    }
    const_cast<DBFRecord &>(record).setReadPos(pos);
    writerPos_ += head_->recordBytes();
  }
  return true;
}

bool DBFFile::overWriten(const std::shared_ptr<DBFRecord> &record) {
  return overWriten(*record);
}

bool DBFFile::overWriten(const std::list<std::shared_ptr<DBFRecord>> &records) {
  if (records.empty()) {
    return true;
  }
  buf_->retrieveAll();
  for (auto &record : records) {
    record->serializeTo(*buf_);
  }
  size_t pos = (*records.begin())->readPos();
  if (0 == pos) {
    buf_->appendChar(kEndFileFlag);
    pos = writerPos_;
  }

  if (!write(buf_->peek(), static_cast<long>(pos), buf_->readableBytes())) {
    return false;
  }

  int32_t newRecordNum = 0;
  if (pos + buf_->readableBytes() - 1 > writerPos_) {
    newRecordNum =
        (pos + buf_->readableBytes() - 1 - writerPos_) / head_->recordBytes();
  }

  if (newRecordNum > 0) {
    head_->setRecordNumber(head_->recordNumber() + newRecordNum);
    if (!writeRecordNumber()) {
      head_->setRecordNumber(head_->recordNumber() - newRecordNum);
      return false;
    }
    for (auto &record : records) {
      record->setReadPos(pos);
      pos += head_->recordBytes();
    }
    writerPos_ = pos;
  }
  return true;
}

bool DBFFile::appendWriten(const DBFRecord &record) {
  buf_->retrieveAll();
  record.serializeTo(*buf_);
  buf_->appendChar(kEndFileFlag);

  if (!appendRecordWriten(*buf_)) {
    return false;
  }

  head_->setRecordNumber(head_->recordNumber() + 1);
  if (!writeRecordNumber()) {
    head_->setRecordNumber(head_->recordNumber() - 1);
    return false;
  }
  writerPos_ += head_->recordBytes();
  return true;
}

bool DBFFile::appendWriten(const std::shared_ptr<DBFRecord> &record) {
  return appendWriten(*record);
}

bool DBFFile::appendWriten(
    const std::list<std::shared_ptr<DBFRecord>> &records) {
  buf_->retrieveAll();
  for (auto &record : records) {
    record->serializeTo(*buf_);
  }
  buf_->appendChar(kEndFileFlag);

  if (!appendRecordWriten(*buf_)) {
    return false;
  }

  head_->setRecordNumber(head_->recordNumber() +
                         static_cast<int32_t>(records.size()));
  if (!writeRecordNumber()) {
    head_->setRecordNumber(head_->recordNumber() -
                           static_cast<int32_t>(records.size()));
    return false;
  }

  writerPos_ += head_->recordBytes() * records.size();
  return true;
}

void DBFFile::checkHeadField(const DBFHeadField &lField,
                             const DBFHeadField &rField) {
  if (lField.name() != rField.name()) {
    SPDLOG_WARN("Field Name error {} != {}", lField.name(), rField.name());
  } else if (lField.filedType() != rField.filedType()) {
    SPDLOG_WARN("Field Type error {} != {}", lField.filedType(),
                rField.filedType());
  } else if (lField.totalLen() != rField.totalLen()) {
    SPDLOG_WARN("Field TotalLen error {} != {}", lField.totalLen(),
                rField.totalLen());
  } else if (lField.precisionLen() != rField.precisionLen()) {
    SPDLOG_WARN("Field PrecisionLen error {} != {}", lField.precisionLen(),
                rField.precisionLen());
  }
}

bool DBFFile::open(const std::string &mode) {
  file_ = fopen(name_.c_str(), mode.c_str());
  if (file_ == nullptr) {
    SPDLOG_WARN("Open failure : {}", strerror(errno));
    return false;
  }
  return true;
}

bool DBFFile::close() {
  if (file_ == nullptr) {
    return true;
  }

  int result = 0;
  do {
    result = fclose(file_);
    if (-1 == result) {
      SPDLOG_WARN("Close failure : {}", strerror(errno));
    }
  } while (-1 == result && errno == EINTR);

  file_ = nullptr;
  return true;
}

#ifdef _WIN32
/**
 * @brief  对文件加锁，windows下只能锁定整个文件
 *
 * @param whence 从何处开始加锁，指定加锁起始位置的计算方式
 * @param _start 加锁起始位置，windows下无效
 * @param _len 加锁长度，windows下无效
 * @return  加锁成功返回true，失败返回false
 */
bool DBFFile::lock(int whence, long _start, long _len) {
  if (0 != fseek(file_, 0, SEEK_SET)) {
    SPDLOG_WARN("Seek failure : {}", strerror(errno));
    return false;
  }

  if (0 != _locking(_fileno(file_), _LK_LOCK, 0)) {
    SPDLOG_WARN(
        "Failed to lock the file : {}, start : {}, len : {}, error : {}", name_,
        _start, _len, strerror(errno));
    return false;
  }
  return true;
}

/**
 * @brief  对文件读加锁，windows下只能锁定整个文件
 *
 * @param whence 从何处开始加锁，指定加锁起始位置的计算方式
 * @param _start 加锁起始位置，windows下无效
 * @param _len 加锁长度，windows下无效
 * @return  加锁成功返回true，失败返回false
 */
bool DBFFile::readLock(int whence, long _start, long _len) {
  if (0 != fseek(file_, 0, SEEK_SET)) {
    SPDLOG_WARN("Seek failure : {}", strerror(errno));
    return false;
  }

  if (0 != _locking(_fileno(file_), _LK_RLCK, 0)) {
    SPDLOG_WARN(
        "Failed to lock the file : {}, start : {}, len : {}, error : {}", name_,
        _start, _len, strerror(errno));
    return false;
  }
  return true;
}

/**
 * @brief  对文件解锁，windows下只能解锁整个文件
 *
 * @param whence 从何处开始解锁，指定解锁起始位置的计算方式
 * @param _start 解锁起始位置，windows下无效
 * @param _len 解锁长度，windows下无效
 * @return  解锁成功返回true，失败返回false
 */
bool DBFFile::unlock(int whence, long _start, long _len) {
  if (0 != fseek(file_, 0, SEEK_SET)) {
    SPDLOG_WARN("Seek failure : {}", strerror(errno));
    return false;
  }

  if (0 != _locking(_fileno(file_), _LK_UNLCK, 0)) {
    SPDLOG_WARN(
        "Failed to unlock the file : {}, start : {}, len : {}, error : {}",
        name_, _start, _len, strerror(errno));
    return false;
  }
  return true;
}
#else
bool DBFFile::lock(int whence, long start, long len) {
  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_whence = whence;
  fl.l_start = start;
  fl.l_len = len;
  if (fcntl(fileno(file_), F_SETLKW, &fl) == -1) {
    SPDLOG_WARN(
        "Failed to lock the file : {}, start : {}, len : {}, error : {}", name_,
        start, len, strerror(errno));
    return false;
  }

  return true;
}

bool DBFFile::readLock(int whence, long start, long len) {
  struct flock fl;
  fl.l_type = F_RDLCK;
  fl.l_whence = whence;
  fl.l_start = start;
  fl.l_len = len;
  if (fcntl(fileno(file_), F_SETLKW, &fl) == -1) {
    SPDLOG_WARN(
        "Failed to lock the file : {}, start : {}, len : {}, error : {}", name_,
        start, len, strerror(errno));
    return false;
  }

  return true;
}

bool DBFFile::unlock(int whence, long start, long len) {
  struct flock fl;
  fl.l_type = F_UNLCK;
  fl.l_whence = whence;
  fl.l_start = start;
  fl.l_len = len;
  if (fcntl(fileno(file_), F_SETLKW, &fl) == -1) {
    SPDLOG_WARN(
        "Failed to unlock the file : {}, start : {}, len : {}, error : {}",
        name_, start, len, strerror(errno));
    return false;
  }

  return true;
}
#endif

bool DBFFile::seek(long pos, int whence) {
  if (0 != fseek(file_, pos, whence)) {
    SPDLOG_WARN("Seek failure : {}", strerror(errno));
    return false;
  }
  return true;
}

bool DBFFile::write(const char *buf, size_t len) {
  auto size = fwrite(buf, sizeof(char), len, file_);
  if (size != len) {
    SPDLOG_WARN("Write failure : {}", strerror(errno));
    return false;
  }
  return true;
}

bool DBFFile::read(char *buf, size_t len) {
  auto size = fread(buf, sizeof(char), len, file_);
  if (size != len) {
    SPDLOG_WARN("Read failure : {}", strerror(errno));
    return false;
  }
  return true;
}

bool DBFFile::flush() {
  if (fflush(file_) != 0) {
    SPDLOG_WARN("Flush failure : {}", strerror(errno));
    return false;
  }

  return true;
}

bool DBFFile::read(DBFBuffer &buf) {
  if (!open("rb")) {
    return false;
  }

  bool ret = true;
  do {
    if (!seek(static_cast<long>(readerPos_), SEEK_SET)) {
      ret = false;
      break;
    }

    if (!read(buf.peek(), buf.readableBytes())) {
      ret = false;
      break;
    }
  } while (false);

  if (!close()) {
    return false;
  }
  return ret;
}

bool DBFFile::appendWriten(const DBFBuffer &buf) {
  if (!open("wb+")) {
    return false;
  }

  bool ret = true;
  do {
    if (!write(buf.peek(), buf.readableBytes())) {
      ret = false;
      break;
    }
  } while (false);

  if (!close()) {
    return false;
  }
  return ret;
}

bool DBFFile::appendRecordWriten(const DBFBuffer &buf) {
  if (!open("r+b")) {
    return false;
  }

  bool ret = true;
  do {
    if (!seek(static_cast<long>(writerPos_), SEEK_SET)) {
      ret = false;
      break;
    }

    if (!write(buf.peek(), buf.readableBytes())) {
      ret = false;
      break;
    }
  } while (false);

  if (!close()) {
    return false;
  }
  return ret;
}

bool DBFFile::write(const char *buf, long start, size_t len) {
  if (!open("rb+")) {
    return false;
  }

  bool ret = true;
  do {
    if (!seek(start, SEEK_SET)) {
      ret = false;
      break;
    }

    if (!write(buf, len)) {
      ret = false;
      break;
    }
  } while (false);

  if (!close()) {
    return false;
  }
  return ret;
}

bool DBFFile::read(char *buf, long start, size_t len) {
  if (!open("rb")) {
    return false;
  }

  bool ret = true;
  do {
    if (!seek(start, SEEK_SET)) {
      ret = false;
      break;
    }

    if (!read(buf, len)) {
      ret = false;
      break;
    }
  } while (false);

  if (!close()) {
    return false;
  }
  return ret;
}
} // namespace dbf