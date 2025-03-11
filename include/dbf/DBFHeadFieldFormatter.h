#ifndef DBF_D_B_F_HEAD_FIELD_FORMATTER_H
#define DBF_D_B_F_HEAD_FIELD_FORMATTER_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "dbf/DBFHeadField.h"

namespace fmt {
template <> struct formatter<dbf::DBFHeadField> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const dbf::DBFHeadField &p, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return format_to(ctx.out(),
                     "Name : {} FiledType : {} ReservedBytes1 : {} TotalLen : "
                     "{} PrecisionLen : {} ReservedBytes2 : {} WorkspaceID : "
                     "{} ReservedBytes3 : {} MdxTag : {} ",
                     p.name(), p.filedType(), p.reservedBytes1(), p.totalLen(),
                     p.precisionLen(), p.reservedBytes2(), p.workspaceID(),
                     p.reservedBytes3(), p.mdxTag());
  }
};
} // namespace fmt

#endif
