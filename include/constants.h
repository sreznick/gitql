#pragma once

#include <cstdint>
namespace constants {
inline constexpr const int HashSize = 40;
inline constexpr const int32_t Hash = 1 << 0;
inline constexpr const int32_t AuthorName = 1 << 1;
inline constexpr const int32_t AuthorEmail = 1 << 2;
inline constexpr const int32_t Message = 1 << 3;
inline constexpr const int32_t Files = 1 << 4;
inline constexpr const int32_t Date = 1 << 5;
inline constexpr const char *DBPath = "/tmp";
inline constexpr const int32_t MaxWordLen = 100;
} // namespace constants
