#ifndef DBF_D_B_F_HEAD_FIELD_JSON_SERIALIZER_H
#define DBF_D_B_F_HEAD_FIELD_JSON_SERIALIZER_H

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <util/StringUtil.hpp>

#include "DBFHeadField.h"

namespace nlohmann {
template <> struct adl_serializer<dbf::DBFHeadField> {
  static void from_json(const json &j, dbf::DBFHeadField &msg) {
    if (j.contains("Name")) {
      msg.setName(j["Name"].get<std::string>());
    } else {
      throw std::invalid_argument("JSON does not contain Name field");
    }
    if (j.contains("FiledType")) {
      msg.setFiledType(j["FiledType"].get<std::string>());
    } else {
      throw std::invalid_argument("JSON does not contain FiledType field");
    }
    if (j.contains("ReservedBytes1")) {
      msg.setReservedBytes1(j["ReservedBytes1"].get<int32_t>());
    } else {
      throw std::invalid_argument("JSON does not contain ReservedBytes1 field");
    }
    if (j.contains("TotalLen")) {
      msg.setTotalLen(j["TotalLen"].get<uint8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain TotalLen field");
    }
    if (j.contains("PrecisionLen")) {
      msg.setPrecisionLen(j["PrecisionLen"].get<uint8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain PrecisionLen field");
    }
    if (j.contains("ReservedBytes2")) {
      msg.setReservedBytes2(j["ReservedBytes2"].get<int16_t>());
    } else {
      throw std::invalid_argument("JSON does not contain ReservedBytes2 field");
    }
    if (j.contains("WorkspaceID")) {
      msg.setWorkspaceID(j["WorkspaceID"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain WorkspaceID field");
    }
    if (j.contains("ReservedBytes3")) {
      msg.setReservedBytes3(j["ReservedBytes3"].get<std::string>());
    } else {
      throw std::invalid_argument("JSON does not contain ReservedBytes3 field");
    }
    if (j.contains("MdxTag")) {
      msg.setMdxTag(j["MdxTag"].get<int8_t>());
    } else {
      throw std::invalid_argument("JSON does not contain MdxTag field");
    }
  }

  static void to_json(json &j, const dbf::DBFHeadField &msg) {
    j["Name"] = msg.name();
    j["FiledType"] = msg.filedType();
    j["ReservedBytes1"] = msg.reservedBytes1();
    j["TotalLen"] = msg.totalLen();
    j["PrecisionLen"] = msg.precisionLen();
    j["ReservedBytes2"] = msg.reservedBytes2();
    j["WorkspaceID"] = msg.workspaceID();
    j["ReservedBytes3"] = msg.reservedBytes3();
    j["MdxTag"] = msg.mdxTag();
  }
};
} // namespace nlohmann

#endif
