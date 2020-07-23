/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains STAT definitions internal to the BCM library.
 */

#ifndef __JER_REG_ACCESS_INCLUDED__
#define __JER_REG_ACCESS_INCLUDED__
#include <soc/sand/sand_signals.h>

int  soc_jer_ilkn_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data);
int  soc_jer_ilkn_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32  data);

int  soc_jer_ilkn_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
int  soc_jer_ilkn_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64  data);

int  soc_jer_ilkn_reg_above_64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);
int  soc_jer_ilkn_reg_above_64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);
void  soc_jer_access_aux_init(int unit, device_t * device);

#endif 
