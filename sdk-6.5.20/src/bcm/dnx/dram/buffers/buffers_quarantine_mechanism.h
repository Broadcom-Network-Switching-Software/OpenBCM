

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED

#define _BCM_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED


#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>

#define NUM_POLL_TOTAL_COUNT 10

shr_error_e dnx_dram_buffers_quarantine_init(
    int unit);

#endif 
