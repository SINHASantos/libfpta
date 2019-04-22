﻿/*
 *  Copyright (c) 1994-2019 Leonid Yuriev <leo@yuriev.ru>.
 *  https://github.com/leo-yuriev/erthink
 *  ZLib License
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgement in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include "erthink_arch.h"
#include "erthink_byteorder.h"
#include "erthink_defs.h"
#include "erthink_intrin.h"

#ifdef __cplusplus
namespace erthink {
#endif

static constexpr __always_inline uint64_t
ERTHINK_NAME_PREFIX(bswap64)(uint64_t v) {
#if __GNUC_PREREQ(4, 4) || __CLANG_PREREQ(4, 0) ||                             \
    __has_builtin(__builtin_bswap64)
  return __builtin_bswap64(v);
#elif defined(_MSC_VER)
  return _byteswap_uint64(v);
#elif defined(__bswap_64)
  return __bswap_64(v);
#elif defined(bswap_64)
  return bswap_64(v);
#else
  return v << 56 | v >> 56 | ((v << 40) & UINT64_C(0x00ff000000000000)) |
         ((v << 24) & UINT64_C(0x0000ff0000000000)) |
         ((v << 8) & UINT64_C(0x000000ff00000000)) |
         ((v >> 8) & UINT64_C(0x00000000ff000000)) |
         ((v >> 24) & UINT64_C(0x0000000000ff0000)) |
         ((v >> 40) & UINT64_C(0x000000000000ff00));
#endif
}

static constexpr __always_inline uint32_t
ERTHINK_NAME_PREFIX(bswap32)(uint32_t v) {
#if __GNUC_PREREQ(4, 4) || __CLANG_PREREQ(4, 0) ||                             \
    __has_builtin(__builtin_bswap32)
  return __builtin_bswap32(v);
#elif defined(_MSC_VER)
  return _byteswap_ulong(v);
#elif defined(__bswap_32)
  return __bswap_32(v);
#elif defined(bswap_32)
  return bswap_32(v);
#else
  return v << 24 | v >> 24 | ((v << 8) & UINT32_C(0x00ff0000)) |
         ((v >> 8) & UINT32_C(0x0000ff00));
#endif
}

static constexpr __always_inline uint16_t
ERTHINK_NAME_PREFIX(bswap16)(uint16_t v) {
#if __GNUC_PREREQ(4, 8) || __has_builtin(__builtin_bswap16)
  return __builtin_bswap16(v);
#elif defined(_MSC_VER)
  return _byteswap_ushort(v);
#elif defined(__bswap_16)
  return __bswap_16(v);
#elif defined(bswap_16)
  return bswap_16(v);
#else
  return return v << 8 | v >> 8;
#endif
}

#ifdef __cplusplus
static constexpr __always_inline int64_t bswap64(int64_t v) {
  return bswap64(uint64_t(v));
}

static constexpr __always_inline int32_t bswap32(int32_t v) {
  return bswap32(uint32_t(v));
}

static constexpr __always_inline int16_t bswap16(int16_t v) {
  return bswap16(uint16_t(v));
}

template <typename T> inline constexpr T bswap(T v);

template <> inline constexpr uint8_t bswap<uint8_t>(uint8_t v) { return v; }
template <> inline constexpr int8_t bswap<int8_t>(int8_t v) { return v; }

template <> inline constexpr uint16_t bswap<uint16_t>(uint16_t v) {
  return bswap16(v);
}
template <> inline constexpr int16_t bswap<int16_t>(int16_t v) {
  return bswap16(v);
}

template <> inline constexpr uint32_t bswap<uint32_t>(uint32_t v) {
  return bswap32(v);
}
template <> inline constexpr int32_t bswap<int32_t>(int32_t v) {
  return bswap32(v);
}

template <> inline constexpr uint64_t bswap<uint64_t>(uint64_t v) {
  return bswap64(v);
}
template <> inline constexpr int64_t bswap<int64_t>(int64_t v) {
  return bswap64(v);
}
}
#endif
