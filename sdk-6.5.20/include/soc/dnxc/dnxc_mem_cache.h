/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DNXC_MEM_CACHE_H_
#define _SOC_DNXC_MEM_CACHE_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#include <soc/mcm/allenum.h>

shr_error_e soc_dnxc_mem_mark_cachable(
    int unit);

shr_error_e soc_dnxc_mem_cache_init(
    int unit);

shr_error_e soc_dnxc_mem_cache_deinit(
    int unit);

#endif
