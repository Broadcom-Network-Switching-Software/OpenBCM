/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#ifndef _WCMOD_PLATFORM_DEFINES_H_
#define _WCMOD_PLATFORM_DEFINES_H_

#include "wcmod_defines.h"
#include "wcmod_enum_defines.h"

#ifdef _SDK_WCMOD_
#include <sal/core/sync.h>
#else
#include "sync.h"
#endif /* _SDK_WCMOD_ */

#if defined (_DV_ODYSSEY)

typedef enum {CHIP, TESTBENCH, DEVICE_ILLEGAL } device_type;

typedef struct {
	int              core_num; /* the serdes number on the chip */
	device_type      device;   /* indicates either chip or testbench */
	wcmod_model_type core;     /* indicates the type of core */
} platform_info_type;

#elif defined (_DV_TRIUMPH3)

#define MAX_CORES 18 /* maximum number of cores for a particular core_type */

typedef enum {CHIP,
              TESTBENCH,
              DEVICE_ILLEGAL /* This must be last */
} device_type;

typedef struct {
  int                 core_num; /* the serdes number on the chip */
  device_type         device;   /* indicates either chip or testbench */
  wcmod_model_type    core;     /* indicates the type of core */
} platform_info_type;

#elif defined (_DV_NATIVE)
typedef struct {
  int unit;
  int prt_ad;
  int phy_ad;
} platform_info_type;

#elif defined (_DV_REDSTONE)
typedef struct {
  int unit;
  int prt_ad;
  int phy_ad;
} platform_info_type;

#elif defined (STANDALONE)
/* dummy platform_info_type. */
typedef struct {
  int id; /* dummy  */
} platform_info_type;

#elif defined (_MDK_WCMOD_)

typedef struct {
  int id; /* unique id (core_num, device, core)-tuple that indicates what mutex to use */
} platform_info_type;

#else
  BAD_DEFINES_IN_WCMOD_PLATFORM_DEFINES_H
#endif

int init_platform_info(platform_info_type* pi);
int init_wcmod_st     (wcmod_st* ws);
int init_platform_info_from_wcmod_st(wcmod_st* from, platform_info_type* pi);
int init_platform_info_to_wcmod_st  (wcmod_st* to,   platform_info_type* pi);
int copy_platform_info(platform_info_type* from, platform_info_type* pi);

#endif /* _WCMOD_PLATFORM_DEFINES_H_ */
