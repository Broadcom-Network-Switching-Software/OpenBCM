/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       types.h
 */

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char       uint8;           /* 8-bit quantity  */
typedef unsigned short      uint16;          /* 16-bit quantity */
typedef unsigned int        uint32;          /* 32-bit quantity */
typedef unsigned long long  uint64;          /* 64-bit quantity */

typedef signed char         int8;            /* 8-bit quantity  */
typedef signed short        int16;           /* 16-bit quantity */
typedef signed int          int32;           /* 32-bit quantity */
typedef signed long long    int64;           /* 64-bit quantity */

#ifndef NULL
#define NULL                (void*)0
#endif

#ifndef STATIC
#define STATIC static
#endif

/* Byte swap a 32 bit value */
#define BCMSWAP32(val) \
	((uint32)( \
		(((uint32)(val) & (uint32)0x000000ffUL) << 24) | \
		(((uint32)(val) & (uint32)0x0000ff00UL) <<  8) | \
		(((uint32)(val) & (uint32)0x00ff0000UL) >>  8) | \
		(((uint32)(val) & (uint32)0xff000000UL) >> 24) ))

#endif /* _TYPES_H_ */
