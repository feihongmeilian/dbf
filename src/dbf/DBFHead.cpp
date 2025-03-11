#include <algorithm>
#include <cstring>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <dbf/DBFBuffer.hpp>
#include <util/StringUtil.hpp>

#include "dbf/DBFHead.h"
#include "dbf/DBFHeadFormatter.h"
#include "dbf/DBFHeadJsonSerializer.hpp"

namespace dbf {
DBFHead::DBFHead()
    : version_(0), year_(0), month_(0), day_(0), recordNumber_(0),
      headerBytes_(0), recordBytes_(0), reservedBytes1_(0),
      incompleteOperations_(0), dbaseIvPasswordMarking_(0), mdxTag_(0),
      driverID_(0), reservedBytes2_(0) {}

DBFHead::~DBFHead() = default;

void DBFHead::parseFrom(dbf::DBFBuffer &buff) {
  {
    auto value = buff.readBinaryInt8();
    setVersion(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setYear(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setMonth(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setDay(value);
  }
  {
    auto value = buff.readBinaryInt32();
    setRecordNumber(value);
  }
  {
    auto value = buff.readBinaryInt16();
    setHeaderBytes(value);
  }
  {
    auto value = buff.readBinaryInt16();
    setRecordBytes(value);
  }
  {
    auto value = buff.readBinaryInt16();
    setReservedBytes1(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setIncompleteOperations(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setDbaseIvPasswordMarking(value);
  }
  {
    auto value = buff.readBinaryString<12>();
    setMultiUserProcessing(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setMdxTag(value);
  }
  {
    auto value = buff.readBinaryInt8();
    setDriverID(value);
  }
  {
    auto value = buff.readBinaryInt16();
    setReservedBytes2(value);
  }
}

void DBFHead::serializeTo(dbf::DBFBuffer &buff) const {
  buff.appendBinaryInt8(version_);
  buff.appendBinaryInt8(year_);
  buff.appendBinaryInt8(month_);
  buff.appendBinaryInt8(day_);
  buff.appendBinaryInt32(recordNumber_);
  buff.appendBinaryInt16(headerBytes_);
  buff.appendBinaryInt16(recordBytes_);
  buff.appendBinaryInt16(reservedBytes1_);
  buff.appendBinaryInt8(incompleteOperations_);
  buff.appendBinaryInt8(dbaseIvPasswordMarking_);
  buff.appendBinaryString<12>(multiUserProcessing_);
  buff.appendBinaryInt8(mdxTag_);
  buff.appendBinaryInt8(driverID_);
  buff.appendBinaryInt16(reservedBytes2_);
}

void DBFHead::parseFromJson(const nlohmann::json &j) { *this = j; }

void DBFHead::serializeToJson(nlohmann::json &j) const { j = *this; }

std::string DBFHead::toString() const { return fmt::format("{}", *this); }

} // namespace dbf
