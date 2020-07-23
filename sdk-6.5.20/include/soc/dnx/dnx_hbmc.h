
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_HBMC_H

#define _DNX_HBMC_H

#include <sal/types.h>

#define SOC_JR2_HBM_DEVICE_ID_WORDS 3
typedef struct
{
    uint32 device_id_raw[SOC_JR2_HBM_DEVICE_ID_WORDS];
} soc_dnx_hbm_device_id_t;

#endif
