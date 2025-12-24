/* types.h - Basic type definitions */
#ifndef TYPES_H
#define TYPES_H

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef int            int32_t;
typedef short          int16_t;
typedef char           int8_t;
typedef uint32_t       uintptr_t;
typedef uint32_t       size_t;

#define NULL  ((void*)0)

_Static_assert(__SIZEOF_POINTER__ == sizeof(uintptr_t), "uintptr_t size does not match pointer size");

#endif
