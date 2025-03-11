#include <algorithm>
#include <cstring>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <dbf/DBFBuffer.hpp>
#include <util/StringUtil.hpp>

#include "dbf/DBFHeadField.h"
#include "dbf/DBFHeadFieldFormatter.h"
#include "dbf/DBFHeadFieldJsonSerializer.hpp"

namespace dbf {
DBFHeadField::DBFHeadField()
    : reservedBytes1_(0), totalLen_(0), precisionLen_(0), reservedBytes2_(0),
      workspaceID_(0), mdxTag_(0) {}

DBFHeadField::DBFHeadField(const DBFHeadField &msg)
    : name_(msg.name()), filedType_(msg.filedType()),
      reservedBytes1_(msg.reservedBytes1()), totalLen_(msg.totalLen()),
      precisionLen_(msg.precisionLen()), reservedBytes2_(msg.reservedBytes2()),
      workspaceID_(msg.workspaceID()), reservedBytes3_(msg.reservedBytes3()),
      mdxTag_(msg.mdxTag()) {}

DBFHeadField::~DBFHeadField() = default;

void DBFHeadField::parseFrom(dbf::DBFBuffer &buff) {
  {
    auto value = buff.readBinaryString<11>();
    setName(value);
  }
  {
    auto value = buff.readBinaryString<1>();
    setFiledType(value);
  }
  {
    auto value = buff.readBinaryInt32();
    setReservedBytes1(value);
  }
  {
    auto value = buff.readBinaryUint8();
    setTotalLen(value);
  }
  {
    auto value = buff.readBinaryUint8();
    setPrecisionLen(value);
  }
  {
    auto value = buff.readBinaryInt16();
    setReservedBytes2(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setWorkspaceID(value);
  }
  {
    auto value = buff.readBinaryString<10>();
    setReservedBytes3(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setMdxTag(value);
  }
}

void DBFHeadField::serializeTo(dbf::DBFBuffer &buff) const {
  buff.appendBinaryString<11>(name_);
  buff.appendBinaryString<1>(filedType_);
  buff.appendBinaryInt32(reservedBytes1_);
  buff.appendBinaryUint8(totalLen_);
  buff.appendBinaryUint8(precisionLen_);
  buff.appendBinaryInt16(reservedBytes2_);
  buff.appendBinaryInt8(workspaceID_);
  buff.appendBinaryString<10>(reservedBytes3_);
  buff.appendBinaryInt8(mdxTag_);
}

void DBFHeadField::parseFromJson(const nlohmann::json &j) { *this = j; }

void DBFHeadField::serializeToJson(nlohmann::json &j) const { j = *this; }

std::string DBFHeadField::toString() const { return fmt::format("{}", *this); }

} // namespace dbf
