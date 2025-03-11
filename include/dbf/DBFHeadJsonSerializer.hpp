#ifndef DBF_D_B_F_HEAD_JSON_SERIALIZER_H
#define DBF_D_B_F_HEAD_JSON_SERIALIZER_H

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <util/StringUtil.hpp>

#include "DBFHead.h"

namespace nlohmann {
template <> struct adl_serializer<dbf::DBFHead> {
  static void from_json(const json &j, dbf::DBFHead &msg) {
    if (j.contains("Version")) {
      msg.setVersion(j["Version"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain Version field");
    }
    if (j.contains("Year")) {
      msg.setYear(j["Year"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain Year field");
    }
    if (j.contains("Month")) {
      msg.setMonth(j["Month"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain Month field");
    }
    if (j.contains("Day")) {
      msg.setDay(j["Day"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain Day field");
    }
    if (j.contains("RecordNumber")) {
      msg.setRecordNumber(j["RecordNumber"].get<int32_t>());
    } else {
      throw std::invalid_argument("JSON does not contain RecordNumber field");
    }
    if (j.contains("HeaderBytes")) {
      msg.setHeaderBytes(j["HeaderBytes"].get<int16_t>());
    } else {
      throw std::invalid_argument("JSON does not contain HeaderBytes field");
    }
    if (j.contains("RecordBytes")) {
      msg.setRecordBytes(j["RecordBytes"].get<int16_t>());
    } else {
      throw std::invalid_argument("JSON does not contain RecordBytes field");
    }
    if (j.contains("ReservedBytes1")) {
      msg.setReservedBytes1(j["ReservedBytes1"].get<int16_t>());
    } else {
      throw std::invalid_argument("JSON does not contain ReservedBytes1 field");
    }
    if (j.contains("IncompleteOperations")) {
      msg.setIncompleteOperations(j["IncompleteOperations"].get<int8_t>());
    } else {
      throw std::invalid_argument(
          "JSON does not contain IncompleteOperations field");
    }
    if (j.contains("DbaseIvPasswordMarking")) {
      msg.setDbaseIvPasswordMarking(j["DbaseIvPasswordMarking"].get<int8_t>());
    } else {
      throw std::invalid_argument(
          "JSON does not contain DbaseIvPasswordMarking field");
    }
    if (j.contains("MultiUserProcessing")) {
      msg.setMultiUserProcessing(j["MultiUserProcessing"].get<std::string>());
    } else {
      throw std::invalid_argument(
          "JSON does not contain MultiUserProcessing field");
    }
    if (j.contains("MdxTag")) {
      msg.setMdxTag(j["MdxTag"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain MdxTag field");
    }
    if (j.contains("DriverID")) {
      msg.setDriverID(j["DriverID"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain DriverID field");
    }
    if (j.contains("ReservedBytes2")) {
      msg.setReservedBytes2(j["ReservedBytes2"].get<int16_t>());
    } else {
      throw std::invalid_argument("JSON does not contain ReservedBytes2 field");
    }
  }

  static void to_json(json &j, const dbf::DBFHead &msg) {
    j["Version"] = msg.version();
    j["Year"] = msg.year();
    j["Month"] = msg.month();
    j["Day"] = msg.day();
    j["RecordNumber"] = msg.recordNumber();
    j["HeaderBytes"] = msg.headerBytes();
    j["RecordBytes"] = msg.recordBytes();
    j["ReservedBytes1"] = msg.reservedBytes1();
    j["IncompleteOperations"] = msg.incompleteOperations();
    j["DbaseIvPasswordMarking"] = msg.dbaseIvPasswordMarking();
    j["MultiUserProcessing"] = msg.multiUserProcessing();
    j["MdxTag"] = msg.mdxTag();
    j["DriverID"] = msg.driverID();
    j["ReservedBytes2"] = msg.reservedBytes2();
  }
};
} // namespace nlohmann

#endif
