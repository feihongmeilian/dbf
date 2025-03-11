#ifndef DBF_D_B_F_HEAD_FORMATTER_H
#define DBF_D_B_F_HEAD_FORMATTER_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "dbf/DBFHead.h"

namespace fmt {
template <> struct formatter<dbf::DBFHead> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const dbf::DBFHead &p, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return format_to(ctx.out(),
                     "Version : {} Year : {} Month : {} Day : {} RecordNumber "
                     ": {} HeaderBytes : {} RecordBytes : {} ReservedBytes1 : "
                     "{} IncompleteOperations : {} DbaseIvPasswordMarking : {} "
                     "MultiUserProcessing : {} MdxTag : {} DriverID : {} "
                     "ReservedBytes2 : {} ",
                     uint32_t(p.version()), uint32_t(p.year()), uint32_t(p.month()), uint32_t(p.day()),
                     p.recordNumber(), p.headerBytes(), p.recordBytes(),
                     p.reservedBytes1(), uint32_t(p.incompleteOperations()),
                     uint32_t(p.dbaseIvPasswordMarking()), p.multiUserProcessing(),
                     uint32_t(p.mdxTag()), uint32_t(p.driverID()), p.reservedBytes2());
  }
};
} // namespace fmt

#endif
