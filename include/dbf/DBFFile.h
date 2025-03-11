#ifndef DBF_FILE_H
#define DBF_FILE_H

#include <cstdio>
#include <ctime>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "DBFHeadField.h"

namespace dbf {
class DBFRecord;
class DBFBuffer;
class DBFHead;

class DBFFile {
public:
  explicit DBFFile(const std::string &name);
  virtual ~DBFFile();

public:
  void appendHeadField(const std::string &name, const std::string &type,
                       uint8_t totalLen = 0, uint8_t precisionLen = 0);
  const std::vector<DBFHeadField> &headFields() const { return headFields_; }

  bool readRecordNumber();
  bool writeRecordNumber();
  bool readHead();
  bool writeHead();

  bool read(DBFRecord &record);
  bool read(const std::shared_ptr<DBFRecord> &record);
  bool read(const std::list<std::shared_ptr<DBFRecord>> &record);
  bool overRead(DBFRecord &record);
  bool overRead(const std::shared_ptr<DBFRecord> &record);
  bool overRead(const std::list<std::shared_ptr<DBFRecord>> &records);

  bool overWriten(const DBFRecord &record);
  bool overWriten(const std::shared_ptr<DBFRecord> &record);
  bool overWriten(const std::list<std::shared_ptr<DBFRecord>> &records);
  bool appendWriten(const DBFRecord &record);
  bool appendWriten(const std::shared_ptr<DBFRecord> &record);
  bool appendWriten(const std::list<std::shared_ptr<DBFRecord>> &records);

  const std::unique_ptr<DBFHead> &head() const { return head_; }
  size_t writerPos() const { return writerPos_; }
  void setWriterPos(size_t val) { writerPos_ = val; }
  size_t readerPos() const { return readerPos_; }
  void setReaderPos(size_t val) { readerPos_ = val; }

  bool write(const char *buf, long start, size_t len);
  bool read(char *buf, long start, size_t len);
  const std::string &name() const { return name_; }

private:
  bool open(const std::string &mode);
  bool close();
  bool lock(int whence, long start, long len);
  bool readLock(int whence, long start, long len);
  bool unlock(int whence, long start, long len);
  bool seek(long pos, int whence);
  bool write(const char *buf, size_t len);
  bool read(char *buf, size_t len);
  bool flush();

private:
  bool read(DBFBuffer &buf);
  bool appendWriten(const DBFBuffer &buf);
  bool appendRecordWriten(const DBFBuffer &buf);

private:
  void checkHeadField(const DBFHeadField &, const DBFHeadField &);

private:
  std::string name_;
  FILE *file_;
  std::vector<DBFHeadField> headFields_;

  size_t writerPos_;
  size_t readerPos_;

  std::unique_ptr<DBFHead> head_;
  std::unique_ptr<DBFBuffer> buf_;

private:
  static const long kRecorNumIndex = 4;
  static const long kHeadBytesIndex = 8;
  static const size_t kFieldLen = 32;
  static const char kEndHeadFlag = 0x0D;
  static const char kEndFileFlag = 0x1A;
};
} // namespace dbf

#endif // !DBF_FILE_H
