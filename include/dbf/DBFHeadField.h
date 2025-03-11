#ifndef DBF_D_B_F_HEAD_FIELD_H
#define DBF_D_B_F_HEAD_FIELD_H

#include "DBFRecord.h"
#include <nlohmann/json.hpp>

namespace dbf {
class DBFHeadField : public DBFRecord {
public:
  DBFHeadField();
  DBFHeadField(const DBFHeadField &);
  ~DBFHeadField();

public:
  virtual void parseFrom(dbf::DBFBuffer &) override;
  virtual void serializeTo(dbf::DBFBuffer &) const override;
  virtual void parseFromJson(const nlohmann::json &) override;
  virtual void serializeToJson(nlohmann::json &) const override;
  virtual std::string toString() const override;

public:
  void setName(const std::string &value) { name_ = value; }
  const std::string &name() const { return name_; }

  void setFiledType(const std::string &value) { filedType_ = value; }
  const std::string &filedType() const { return filedType_; }

  void setReservedBytes1(int32_t value) { reservedBytes1_ = value; }
  int32_t reservedBytes1() const { return reservedBytes1_; }

  void setTotalLen(uint8_t value) { totalLen_ = value; }
  uint8_t totalLen() const { return totalLen_; }

  void setPrecisionLen(uint8_t value) { precisionLen_ = value; }
  uint8_t precisionLen() const { return precisionLen_; }

  void setReservedBytes2(int16_t value) { reservedBytes2_ = value; }
  int16_t reservedBytes2() const { return reservedBytes2_; }

  void setWorkspaceID(int8_t value) { workspaceID_ = value; }
  int8_t workspaceID() const { return workspaceID_; }

  void setReservedBytes3(const std::string &value) { reservedBytes3_ = value; }
  const std::string &reservedBytes3() const { return reservedBytes3_; }

  void setMdxTag(int8_t value) { mdxTag_ = value; }
  int8_t mdxTag() const { return mdxTag_; }

private:
  std::string name_;
  std::string filedType_;
  int32_t reservedBytes1_;
  uint8_t totalLen_;
  uint8_t precisionLen_;
  int16_t reservedBytes2_;
  int8_t workspaceID_;
  std::string reservedBytes3_;
  int8_t mdxTag_;
};
} // namespace dbf
#endif
