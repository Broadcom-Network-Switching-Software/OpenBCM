

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_BUFFERS_DRAMBUFFERS_H_INCLUDED

#define _BCMINT_DNX_DRAM_BUFFERS_DRAMBUFFERS_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>






shr_error_e dnx_dram_buffers_nof_bdbs_per_core_get(
    int unit,
    uint32 *bdbs_per_core);


shr_error_e dnx_dram_buffers_init(
    int unit);


shr_error_e dnx_dram_buffers_status_get(
    int unit,
    int core,
    uint8 deleted,
    uint32 *buffers_array,
    int max_size,
    int *size);


#endif 
