/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DPP_QAX_INIT_H
#define _SOC_DPP_QAX_INIT_H






typedef enum soc_fabric_dtq_mode_type_e
{
    SOC_FABRIC_DTQ_MODE_SINGLE_QUEUE = 0x0,
    SOC_FABRIC_DTQ_MODE_UC_MC = 0x1,
    SOC_FABRIC_DTQ_MODE_UC_HMC_LMC = 0x2
} soc_fabric_dtq_mode_type_t ;




int soc_qax_init_blocks_init_conf(int unit);
int soc_qax_init_blocks_general(int unit);


int soc_qax_init_overrides(int unit);


int soc_qax_init_dpp_defs_overrides(int unit);


soc_error_t soc_qax_tdq_contexts_init(int unit);

uint32 soc_qax_pdq_dtq_contexts_init(SOC_SAND_IN int unit);

int soc_qax_nbil_phy_led_pmlq_bypass_enable(int unit, int quad, int enable);
#endif 

