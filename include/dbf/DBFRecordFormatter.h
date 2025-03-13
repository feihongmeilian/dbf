#ifndef DBF_D_B_F_RECORD_FORMATTER_H
#define DBF_D_B_F_RECORD_FORMATTER_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "dbf/DBFRecord.h"

namespace fmt {
template <> struct formatter<dbf::DBFRecord> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const dbf::DBFRecord &p, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return format_to(ctx.out(), "RecordDelete : {} ReadPos : {} ",
                     p.recordDelete(), p.readPos());
  }
};
} // namespace fmt

#endif
