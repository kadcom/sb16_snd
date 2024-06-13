#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <stdint.h>
#include <stdbool.h>

#if !defined(INLINE)
#define INLINE __inline 
#endif

#define KB    1024
#define KB_16 (16 * KB)
#define KB_32 (32 * KB)
#define KB_64 (64 * KB)

#define HZ_8K  8000
#define HZ_11K 11025
#define HZ_22K 22050

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

#define _MAKE16(hi, lo) ((u16)(((u8)(hi) << 8) | (u8)(lo))
#define _LO(x) ((u8)((x) & 0xFF))
#define _HI(x) ((u8)((x) >> 8))

#define _MAKE32(hi, lo) ((u32)(((u16)(hi) << 16) | (u16)(lo))
#define _LO16(x) ((u16)((x) & 0xFFFF))
#define _HI16(x) ((u16)((x) >> 16))

#define SB_SUCCESS          0
#define SB_E_FAIL           1
#define SB_E_NO_BLASTER_ENV 2
#define SB_E_VERSION        3
#define SB_E_NO_MEMORY      4
#define SB_E_INVALID_PARAM  5

#define SB_OK(x) ((x) == SB_SUCCESS)
#define SB_FAIL(x) ((x) != SB_SUCCESS)

#ifdef __386__
#define FAR 
#define _INT(x, regsin, regsout) int386(x, regsin, regsout)
#else
#define FAR far
#define _INT(x, regsin, regsout) int86(x, regsin, regsout)
#endif

/* Parse a number until a whitespace is found */

INLINE static u16 parse_until_wspace(char const *ptr, char const **end_ptr) {
  u16 val = 0;
  while (*ptr != ' ' && *ptr != '\0' && *ptr != '\n' && *ptr != '\r' && *ptr != '\t') {
    val = (val * 10) + (*ptr - '0');
    ++ptr;
  }

  *end_ptr = ptr;
  return val;
}


#endif /* PLATFORM_H */
