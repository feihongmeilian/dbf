#ifndef DBF_D_B_F_HEAD_H
#define DBF_D_B_F_HEAD_H

#include "DBFRecord.h"
#include <nlohmann/json.hpp>

namespace dbf {
class DBFHead : public DBFRecord {
public:
  DBFHead();
  ~DBFHead();

public:
  virtual void parseFrom(dbf::DBFBuffer &) override;
  virtual void serializeTo(dbf::DBFBuffer &) const override;
  virtual void parseFromJson(const nlohmann::json &) override;
  virtual void serializeToJson(nlohmann::json &) const override;
  virtual std::string toString() const override;

public:
  void setVersion(int8_t value) { version_ = value; }
  int8_t version() const { return version_; }

  void setYear(int8_t value) { year_ = value; }
  int8_t year() const { return year_; }

  void setMonth(int8_t value) { month_ = value; }
  int8_t month() const { return month_; }

  void setDay(int8_t value) { day_ = value; }
  int8_t day() const { return day_; }

  void setRecordNumber(int32_t value) { recordNumber_ = value; }
  int32_t recordNumber() const { return recordNumber_; }

  void setHeaderBytes(int16_t value) { headerBytes_ = value; }
  int16_t headerBytes() const { return headerBytes_; }

  void setRecordBytes(int16_t value) { recordBytes_ = value; }
  int16_t recordBytes() const { return recordBytes_; }

  void setReservedBytes1(int16_t value) { reservedBytes1_ = value; }
  int16_t reservedBytes1() const { return reservedBytes1_; }

  void setIncompleteOperations(int8_t value) { incompleteOperations_ = value; }
  int8_t incompleteOperations() const { return incompleteOperations_; }

  void setDbaseIvPasswordMarking(int8_t value) {
    dbaseIvPasswordMarking_ = value;
  }
  int8_t dbaseIvPasswordMarking() const { return dbaseIvPasswordMarking_; }

  void setMultiUserProcessing(const std::string &value) {
    multiUserProcessing_ = value;
  }
  const std::string &multiUserProcessing() const {
    return multiUserProcessing_;
  }

  void setMdxTag(int8_t value) { mdxTag_ = value; }
  int8_t mdxTag() const { return mdxTag_; }

  void setDriverID(int8_t value) { driverID_ = value; }
  int8_t driverID() const { return driverID_; }

  void setReservedBytes2(int16_t value) { reservedBytes2_ = value; }
  int16_t reservedBytes2() const { return reservedBytes2_; }

private:
  int8_t version_;
  int8_t year_;
  int8_t month_;
  int8_t day_;
  int32_t recordNumber_;
  int16_t headerBytes_;
  int16_t recordBytes_;
  int16_t reservedBytes1_;
  int8_t incompleteOperations_;
  int8_t dbaseIvPasswordMarking_;
  std::string multiUserProcessing_;
  int8_t mdxTag_;
  int8_t driverID_;
  int16_t reservedBytes2_;
};
} // namespace dbf
#endif
