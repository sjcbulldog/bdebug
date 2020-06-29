#pragma once

#ifdef _MSC_VER
#define PLATFORM_SWAPU16(x) _byteswap_ushort(x)
#define PLATFORM_SWAPU32(x) _byteswap_ulong(x)
#define PLATFORM_SWAPU64(x) _byteswap_uint64(x)
#endif

#ifdef _GCC
#define PLATFORM_SWAPU16(x) __builtin_bswap16(x)
#define PLATFORM_SWAPU32(x) __builtin_bswap32(x)
#define PLATFORM_SWAPU64(x) __builtin_bswap64(x)
#endif
