#pragma once

#include <cstdint>
namespace constants {
inline constexpr const char *ApiHost = "api.github.com";
inline constexpr const char *HelperUrl =
    "/repos/MAXXXIMUS-tropical-milkshake/drop-archiving/commits?sha=feature/"
    "archiving";
inline constexpr const int HashSize = 40;
inline constexpr int32_t Hash = 1 << 0;
inline constexpr int32_t AuthorName = 1 << 1;
inline constexpr int32_t AuthorEmail = 1 << 2;
inline constexpr int32_t Message = 1 << 3;
inline constexpr int32_t Files = 1 << 4;
inline constexpr int32_t Date = 1 << 5;
} // namespace constants
