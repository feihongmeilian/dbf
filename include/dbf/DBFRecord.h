#ifndef DBF_RECORD_H
#define DBF_RECORD_H

#include <string>
#include <fstream>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "DBFBuffer.hpp"

namespace dbf {
class DBFRecord {
public:
  DBFRecord() : recordDelete_(false), readPos_(0) {}
  virtual ~DBFRecord() {}

public:
  virtual void parseFrom(DBFBuffer &buf) {
    setRecordDelete(buf.readChar());
  }

  virtual void serializeTo(DBFBuffer &buf) const {
    buf.appendChar(recordDelete_ == false ? 0x20 : 0x2A);
  }

  virtual void parseFromJson(const nlohmann::json &) {}
  virtual void serializeToJson(nlohmann::json &) const {}
  virtual std::string toString() const { return ""; }

  bool recordDelete() const { return recordDelete_; }
  void setRecordDelete(char recordDelete) {
    if (recordDelete == 0x20) {
      recordDelete_ = false;
    } else if (recordDelete == 0x2A) {
      recordDelete_ = true;
    } else {
      SPDLOG_WARN("read record delete tag failed");
    }
  }

  void setReadPos(size_t readPos) { readPos_ = readPos; }
  size_t readPos() const { return readPos_; }

private:
  bool recordDelete_;
  size_t readPos_;
};
} // namespace dbf

#endif
