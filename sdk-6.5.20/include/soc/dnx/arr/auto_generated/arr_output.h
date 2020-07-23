
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/utilex/utilex_rhlist.h>

#ifndef ARR_OUTPUT_H_INCLUDED
#define ARR_OUTPUT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>

shr_error_e dnx_arr_output_init(
    int unit);

#endif
