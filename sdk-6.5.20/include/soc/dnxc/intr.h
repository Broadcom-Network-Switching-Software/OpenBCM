
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DNXC_INTR_H_INCLUDE

#define SOC_DNXC_INTR_H_INCLUDE

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (JR2) and DNXF (Ramon) family only!"
#endif

#include <sal/types.h>
#include <shared/cyclic_buffer.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/defs.h>
#include <soc/iproc.h>
#include <soc/dnxc/dnxc_defs.h>

typedef enum
{

    dnxc_mem_mask_mode_none = 0,

    dnxc_mem_mask_mode_zero = 1,

    dnxc_mem_mask_mode_one = 2
} dnxc_mem_mask_mode_e;

#define INTR_ERROR_MAX_INTERRUPTS_SIZE    50

int soc_dnxc_intr_init(
    int unit);
int soc_dnxc_intr_deinit(
    int unit);
void soc_dnxc_intr_handler(
    int unit,
    void *data);
void soc_dnxc_ser_intr_handler(
    void *unit_vp,
    void *d1,
    void *d2,
    void *d3,
    void *d4);

#endif
