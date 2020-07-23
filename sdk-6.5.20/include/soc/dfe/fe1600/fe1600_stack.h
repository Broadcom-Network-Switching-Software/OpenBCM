/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 STACK H
 */
 
#ifndef _SOC_FE1600_STACK_H_
#define _SOC_FE1600_STACK_H_

#include <bcm/stat.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

soc_error_t soc_fe1600_stk_modid_set(int unit, uint32 fe_id);
soc_error_t soc_fe1600_stk_modid_get(int unit, uint32* fe_id);

#endif 
