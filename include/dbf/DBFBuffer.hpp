#ifndef DBF_BUFFER_H
#define DBF_BUFFER_H

#include <algorithm>
#include <cassert>

#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <exception>

#include <boost/utility/string_view.hpp>
#include <boost/endian/conversion.hpp>

#include "StringUtil.hpp"

namespace dbf {
class DBFBuffer {
public:
  explicit DBFBuffer(size_t initialSize = 1024, size_t cheapPrepend = 8)
      : buf_(cheapPrepend + initialSize), kCheapPrepend(cheapPrepend),
        readerIndex_(cheapPrepend), writerIndex_(cheapPrepend) {
    assert(readableBytes() == 0);
    assert(writableBytes() == initialSize);
    assert(prependableBytes() == kCheapPrepend);
  }

  ~DBFBuffer() {}

  void retrieveAll() {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }

  void retrieve(size_t len) {
    assert(len <= readableBytes());
    if (len < readableBytes()) {
      readerIndex_ += len;
    } else {
      retrieveAll();
    }
  }

  size_t readableBytes() const { return writerIndex_ - readerIndex_; }

  size_t writableBytes() const { return buf_.size() - writerIndex_; }

  size_t prependableBytes() const { return readerIndex_; }

  const char *peek() const { return begin() + readerIndex_; }

  char *peek() { return begin() + readerIndex_; }

  template <size_t FieldLen> int8_t readInt8() {
    return static_cast<int8_t>(readInt16<FieldLen>());
  }

  template <size_t FieldLen> uint8_t readUint8() {
    return static_cast<uint8_t>(readUint16<FieldLen>());
  }

  template <size_t FieldLen> int16_t readInt16() {
    return readInt<FieldLen, int16_t>();
  }

  template <size_t FieldLen> uint16_t readUint16() {
    return readInt<FieldLen, uint16_t>();
  }

  template <size_t FieldLen> int32_t readInt32() {
    return readInt<FieldLen, int32_t>();
  }

  template <size_t FieldLen> uint32_t readUint32() {
    return readInt<FieldLen, uint32_t>();
  }

  template <size_t FieldLen> int64_t readInt64() {
    return readInt<FieldLen, int64_t>();
  }

  template <size_t FieldLen> uint64_t readUint64() {
    return readInt<FieldLen, uint64_t>();
  }

  template <size_t FieldLen, size_t PrecisionSize> int8_t readInt8() {
    return static_cast<int8_t>(readInt16<FieldLen, PrecisionSize>());
  }

  template <size_t FieldLen, size_t PrecisionSize> uint8_t readUint8() {
    return static_cast<uint8_t>(readUint16<FieldLen, PrecisionSize>());
  }

  template <size_t FieldLen, size_t PrecisionSize> int16_t readInt16() {
    return readInt<FieldLen, PrecisionSize, int16_t>();
  }

  template <size_t FieldLen, size_t PrecisionSize> uint16_t readUint16() {
    return readInt<FieldLen, PrecisionSize, uint16_t>();
  }

  template <size_t FieldLen, size_t PrecisionSize> int32_t readInt32() {
    return readInt<FieldLen, PrecisionSize, int32_t>();
  }

  template <size_t FieldLen, size_t PrecisionSize> uint32_t readUint32() {
    return readInt<FieldLen, PrecisionSize, uint32_t>();
  }

  template <size_t FieldLen, size_t PrecisionSize> int64_t readInt64() {
    return readInt<FieldLen, PrecisionSize, int64_t>();
  }

  template <size_t FieldLen, size_t PrecisionSize> uint64_t readUint64() {
    return readInt<FieldLen, PrecisionSize, uint64_t>();
  }

  template <size_t FieldLen, typename T> T readInt() {
    auto view = readStringView<FieldLen>();
    if (view.empty()) {
      return 0;
    }
    return stringViewToInt<T>(view);
  }

  template <size_t FieldLen, size_t PrecisionSize, typename T> T readInt() {
    auto view = readStringView<FieldLen>();
    return floatStringViewToInt<FieldLen, PrecisionSize, T>(view);
  }

  template <size_t FieldLen> inline std::string readString() {
    auto view = readStringView<FieldLen>();
    return std::string(view.data(), view.size());
  }

  template <size_t FieldLen> inline boost::string_view readStringView() {
    if (readableBytes() < FieldLen) {
      std::stringstream ss;
      ss << "Insufficient readable data";
      throw std::range_error(ss.str().c_str());
    }
    boost::string_view view(peek(), FieldLen);
    trim(view);
    retrieve(FieldLen);
    return view;
  }

  inline char readChar() {
    if (readableBytes() < sizeof(char)) {
      std::stringstream ss;
      ss << "Insufficient readable data";
      throw std::range_error(ss.str().c_str());
    }
    char ch = *peek();
    retrieve(sizeof(char));
    return ch;
  }

  template <size_t FieldLen> inline std::array<char, FieldLen> readArray() {
    std::array<char, FieldLen> arr;
    arr.fill(' ');
    auto view = readStringView<FieldLen>();
    std::memcpy(arr.data(), view.data(), FieldLen);
    return arr;
  }

  template <size_t FieldLen> void appendInt8(int8_t val) {
    appendInt16<FieldLen>(static_cast<int16_t>(val));
  }

  template <size_t FieldLen> void appendUint8(uint8_t val) {
    appendUint16<FieldLen>(static_cast<uint16_t>(val));
  }

  template <size_t FieldLen> void appendInt16(int16_t val) {
    appendInt<FieldLen>(val);
  }

  template <size_t FieldLen> void appendUint16(uint16_t val) {
    appendInt<FieldLen>(val);
  }

  template <size_t FieldLen> void appendInt32(int32_t val) {
    appendInt<FieldLen>(val);
  }

  template <size_t FieldLen> void appendUint32(uint32_t val) {
    appendInt<FieldLen>(val);
  }

  template <size_t FieldLen> void appendInt64(int64_t val) {
    appendInt<FieldLen>(val);
  }

  template <size_t FieldLen> void appendUint64(uint64_t val) {
    appendInt<FieldLen>(val);
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendInt8(int8_t val) {
    appendInt16<FieldLen, PrecisionSize>(static_cast<int16_t>(val));
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendUint8(uint8_t val) {
    appendInt16<FieldLen, PrecisionSize>(static_cast<uint16_t>(val));
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendInt16(int16_t val) {
    appendInt<FieldLen, PrecisionSize>(val);
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendUint16(uint16_t val) {
    appendInt<FieldLen, PrecisionSize>(val);
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendInt32(int32_t val) {
    appendInt<FieldLen, PrecisionSize>(val);
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendUint32(uint32_t val) {
    appendInt<FieldLen, PrecisionSize>(val);
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendInt64(int64_t val) {
    appendInt<FieldLen, PrecisionSize>(val);
  }

  template <size_t FieldLen, size_t PrecisionSize>
  void appendUint64(uint64_t val) {
    appendInt<FieldLen, PrecisionSize>(val);
  }

  template <size_t FieldLen, typename T> inline void appendInt(T val) {
    ensureWritableBytes(FieldLen);
    boost::string_view view(beginWrite(), FieldLen);
    intToStringView<T>(val, view);
    append<FieldLen>(view);
  }

  template <size_t FieldLen, size_t PrecisionSize, typename T>
  inline void appendInt(T val) {
    ensureWritableBytes(FieldLen);
    boost::string_view view(beginWrite(), FieldLen);
    intToFloatStringView<FieldLen, PrecisionSize, T>(val, view);
    append<FieldLen>(view);
  }

  template <size_t FieldLen> inline void appendString(const std::string &val) {
    ensureWritableBytes(FieldLen);
    if (val.size() > FieldLen) {
      std::stringstream ss;
      ss << "Append value too long : " << val;
      throw std::range_error(ss.str().c_str());
    }
    std::memcpy(beginWrite(), val.data(), val.size());
    if (FieldLen > val.size()) {
      std::memset(beginWrite() + val.size(), ' ', FieldLen - val.size());
    }
    hasWritten(FieldLen);
  }

  inline void appendChar(char val) {
    ensureWritableBytes(sizeof val);
    *beginWrite() = val;
    hasWritten(sizeof val);
  }

  template <size_t FieldLen>
  inline void appendArray(const std::array<char, FieldLen> &arr) {
    ensureWritableBytes(FieldLen);
    std::memcpy(beginWrite(), arr.data(), FieldLen);
    hasWritten(FieldLen);
  }

  int8_t readBinaryInt8() { return readBinaryInt<int8_t>(); }

  uint8_t readBinaryUint8() { return readBinaryInt<uint8_t>(); }

  int16_t readBinaryInt16() { return readBinaryInt<int16_t>(); }

  uint16_t readBinaryUint16() { return readBinaryInt<uint16_t>(); }

  int32_t readBinaryInt32() { return readBinaryInt<int32_t>(); }

  uint32_t readBinaryUint32() { return readBinaryInt<uint32_t>(); }

  int64_t readBinaryInt64() { return readBinaryInt<int64_t>(); }

  uint64_t readBinaryUint64() { return readBinaryInt<uint64_t>(); }

  template <typename T> T readBinaryInt() {
    if (readableBytes() < sizeof(T)) {
      std::stringstream ss;
      ss << "Insufficient readable data";
      throw std::range_error(ss.str().c_str());
    }
    T val;
    std::memcpy(&val, peek(), sizeof val);
    retrieve(sizeof val);
    return boost::endian::little_to_native(val);
  }

  template <size_t FieldLen> inline std::string readBinaryString() {
    if (readableBytes() < FieldLen) {
      std::stringstream ss;
      ss << "Insufficient readable data";
      throw std::range_error(ss.str().c_str());
    }
    auto len = FieldLen;
    while (len > 0 && peek()[len - 1] == 0) {
      --len;
    }
    std::string str;
    if (len > 0) {
      str.insert(0, peek(), len);
    }
    retrieve(FieldLen);
    return str;
  }

  void appendBinaryInt8(int8_t val) { appendBinaryInt(val); }

  void appendBinaryUint8(uint8_t val) { appendBinaryInt(val); }

  void appendBinaryInt16(int16_t val) { appendBinaryInt(val); }

  void appendBinaryUint16(uint16_t val) { appendBinaryInt(val); }

  void appendBinaryInt32(int32_t val) { appendBinaryInt(val); }

  void appendBinaryUint32(uint32_t val) { appendBinaryInt(val); }

  void appendBinaryInt64(int64_t val) { appendBinaryInt(val); }

  void appendBinaryUint64(uint64_t val) { appendBinaryInt(val); }

  template <typename T> inline void appendBinaryInt(T val) {
    ensureWritableBytes(sizeof val);
    val = boost::endian::native_to_little(val);
    std::memcpy(beginWrite(), &val, sizeof val);
    hasWritten(sizeof val);
  }

  template <size_t FieldLen>
  inline void appendBinaryString(const std::string &val) {
    ensureWritableBytes(FieldLen);
    if (val.size() > FieldLen) {
      std::stringstream ss;
      ss << "Append value too long : " << val;
      throw std::range_error(ss.str().c_str());
    }
    std::memcpy(beginWrite(), val.data(), val.size());
    if (FieldLen > val.size()) {
      std::memset(beginWrite() + val.size(), 0, FieldLen - val.size());
    }
    hasWritten(FieldLen);
  }

  void ensureWritableBytes(size_t len) {
    if (writableBytes() < len) {
      makeSpace(len);
    }
    assert(writableBytes() >= len);
  }

  char *beginWrite() { return begin() + writerIndex_; }

  const char *beginWrite() const { return begin() + writerIndex_; }

  void hasWritten(size_t len) {
    assert(len <= writableBytes());
    writerIndex_ += len;
  }

  void unwrite(size_t len) {
    assert(len <= readableBytes());
    writerIndex_ -= len;
  }

private:
  char *begin() { return &*buf_.begin(); }

  const char *begin() const { return &*buf_.begin(); }

  void makeSpace(size_t len) {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
      // FIXME: move readable data
      buf_.resize(writerIndex_ + len);
    } else {
      // move readable data to the front, make space inside buffer
      assert(kCheapPrepend < readerIndex_);
      size_t readable = readableBytes();
      std::memcpy(begin() + kCheapPrepend, begin() + readerIndex_,
                  writerIndex_ - readerIndex_);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
      assert(readable == readableBytes());
    }
  }

  template <size_t FieldLen>
  inline void append(const boost::string_view &view) {
    if (FieldLen > view.size()) {
      std::memset(beginWrite(), ' ', FieldLen - view.size());
    }
    std::memmove(beginWrite() + FieldLen - view.size(), view.data(),
                 view.size());
    hasWritten(FieldLen);
  }

  void trim(boost::string_view &view) {
    auto start = view.find_first_not_of(' ');
    auto end = view.find_last_not_of(' ');

    if (start == boost::string_view::npos) {
      view = boost::string_view();
    } else {
      view = view.substr(start, end - start + 1);
    }
  }

private:
  std::vector<char> buf_;
  const size_t kCheapPrepend;
  size_t readerIndex_;
  size_t writerIndex_;
};
} // namespace dbf

#endif
