/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DPP_JER_RESET_H
#define _SOC_DPP_JER_RESET_H





int soc_jer_reset_soft_init(int unit);
int soc_jer_reset_nif_txi_oor(int unit);
int soc_jer_reset_blocks_soft_init(int unit, int reset_action);

#endif 


