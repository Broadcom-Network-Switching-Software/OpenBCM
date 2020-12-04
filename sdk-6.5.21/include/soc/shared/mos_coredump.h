/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    MOS_COREDUMP.h
 */

#ifndef _SOC_SHARED_MOS_COREDUMP_H
#define _SOC_SHARED_MOS_COREDUMP_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#endif

typedef struct mos_coredump_region_s {
    uint32    cores;              /* bitfield of core numbers region belongs to */
    uint32    baseaddr;           /* Core-view base addr of region */
    uint32    start;              /* Offset of start of region */
    uint32    end;                /* Offset of end of region */
} mos_coredump_region_t;

#endif


