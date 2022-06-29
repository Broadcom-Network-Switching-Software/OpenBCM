#ifndef _SOC_DNXC_MEM_CACHE_H_
#define _SOC_DNXC_MEM_CACHE_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX and DNXF family only!"
#endif

#include <soc/mcm/allenum.h>

shr_error_e soc_dnxc_mem_mark_cachable(
    int unit);

shr_error_e soc_dnxc_mem_cache_init(
    int unit);

shr_error_e soc_dnxc_mem_cache_deinit(
    int unit);

#endif
