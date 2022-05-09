
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#include "../pemladrv_logical_access.h"
#include "pemladrv_vw_logical_access.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"

extern ApiInfo api_info[MAX_NOF_UNITS];


VirtualWiresContainer* dnx_pemladrv_get_virtual_wires_info(const int unit) {
    return &api_info[unit].vw_container;
}



