
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNX_DRV_REGS_H
#define _SOC_DNX_DRV_REGS_H

#if (!defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT))
#error "This file is for use by DNX or DNXF family only!"
#endif

#include <soc/drv.h>

extern const soc_reg_t q2a_dynamic_mem_enable_regs[];
extern const soc_reg_t q2a_dynamic_mem_disable_regs[];
extern const soc_reg_t jr2_dynamic_mem_enable_regs[];
extern const soc_reg_t jr2_dynamic_mem_disable_regs[];
extern const soc_reg_t j2c_dynamic_mem_enable_regs[];
extern const soc_reg_t j2c_dynamic_mem_disable_regs[];
extern const soc_reg_t j2p_dynamic_mem_enable_regs[];
extern const soc_reg_t j2p_dynamic_mem_disable_regs[];
extern const soc_reg_t j2x_dynamic_mem_enable_regs[];
extern const soc_reg_t j2x_dynamic_mem_disable_regs[];

extern const soc_reg_t J2P_ECI_GLOBAL_MEM_OPTIONS_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_CFG_1_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_CFG_2_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_CFG_3_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_SYS_HEADER_CFG_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_IDB_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_1_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_2_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_3_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_4_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_5_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_6_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_7_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_8_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_9_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_10_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_11_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_12_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_GTIMER_CYCLE_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_GTIMER_CONFIGURATION_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_PCMI_0_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_PCMI_1_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_PCMI_2_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_PCMI_3_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_PCMI_4_global_register_block_registers[];
extern const soc_reg_t J2P_ECI_PCMI_5_global_register_block_registers[];
extern const soc_reg_t *j2p_common_regs_addr_2_global_register_block_registers[257];

extern const soc_reg_t *j2x_common_regs_addr_2_global_register_block_registers[257];

#endif
