/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _JER_DRAM_H
#define _JER_DRAM_H



int soc_jer_dram_info_verify(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_jer_dram_init_drc_soft_init(int unit, soc_dpp_drc_combo28_info_t *drc_info, uint32  init);
int soc_jer_dram_recovery_init(int unit);

#endif 
