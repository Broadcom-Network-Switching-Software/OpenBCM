
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNX_ACCESS_H

#define _SOC_DNX_ACCESS_H

#include <bcm_int/dnx/init/init.h>
#include <soc/sand/sand_signals.h>

#define DNX_MAX_NOF_ECC_ERR_MEMS 13

shr_error_e dnx_access_aux_init(
    int unit,
    device_t * device);

#endif
