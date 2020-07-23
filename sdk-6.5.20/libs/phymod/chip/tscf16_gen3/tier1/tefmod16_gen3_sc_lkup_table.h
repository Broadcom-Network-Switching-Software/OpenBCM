/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators
 *----------------------------------------------------------------------*/
#ifndef tefmod16_gen3_sc_lkup_table_H_
#define tefmod16_gen3_sc_lkup_table_H_

#define _SDK_TEFMOD16_GEN3_ 1

#include "tefmod16_gen3_enum_defines.h"
#include "tefmod16_gen3.h"
#include <phymod/phymod.h>

/****************************************************************************
 * Enums: sc_x4_control_sc
 */
#define sc_x4_control_sc_S_10G_CR1                         0
#define sc_x4_control_sc_S_10G_KR1                         1
#define sc_x4_control_sc_S_10G_X1                          2
#define sc_x4_control_sc_S_10G_HG2_CR1                     4
#define sc_x4_control_sc_S_10G_HG2_KR1                     5
#define sc_x4_control_sc_S_10G_HG2_X1                      6
#define sc_x4_control_sc_S_20G_CR1                         8
#define sc_x4_control_sc_S_20G_KR1                         9
#define sc_x4_control_sc_S_20G_X1                          10
#define sc_x4_control_sc_S_20G_HG2_CR1                     12
#define sc_x4_control_sc_S_20G_HG2_KR1                     13
#define sc_x4_control_sc_S_20G_HG2_X1                      14
#define sc_x4_control_sc_S_25G_CR1                         16
#define sc_x4_control_sc_S_25G_KR1                         17
#define sc_x4_control_sc_S_25G_X1                          18
#define sc_x4_control_sc_S_25G_HG2_CR1                     20
#define sc_x4_control_sc_S_25G_HG2_KR1                     21
#define sc_x4_control_sc_S_25G_HG2_X1                      22
#define sc_x4_control_sc_S_20G_CR2                         24
#define sc_x4_control_sc_S_20G_KR2                         25
#define sc_x4_control_sc_S_20G_X2                          26
#define sc_x4_control_sc_S_20G_HG2_CR2                     28
#define sc_x4_control_sc_S_20G_HG2_KR2                     29
#define sc_x4_control_sc_S_20G_HG2_X2                      30
#define sc_x4_control_sc_S_40G_CR2                         32
#define sc_x4_control_sc_S_40G_KR2                         33
#define sc_x4_control_sc_S_40G_X2                          34
#define sc_x4_control_sc_S_40G_HG2_CR2                     36
#define sc_x4_control_sc_S_40G_HG2_KR2                     37
#define sc_x4_control_sc_S_40G_HG2_X2                      38
#define sc_x4_control_sc_S_40G_CR4                         40
#define sc_x4_control_sc_S_40G_KR4                         41
#define sc_x4_control_sc_S_40G_X4                          42
#define sc_x4_control_sc_S_40G_HG2_CR4                     44
#define sc_x4_control_sc_S_40G_HG2_KR4                     45
#define sc_x4_control_sc_S_40G_HG2_X4                      46
#define sc_x4_control_sc_S_50G_CR2                         48
#define sc_x4_control_sc_S_50G_KR2                         49
#define sc_x4_control_sc_S_50G_X2                          50
#define sc_x4_control_sc_S_50G_HG2_CR2                     52
#define sc_x4_control_sc_S_50G_HG2_KR2                     53
#define sc_x4_control_sc_S_50G_HG2_X2                      54
#define sc_x4_control_sc_S_50G_CR4                         56
#define sc_x4_control_sc_S_50G_KR4                         57
#define sc_x4_control_sc_S_50G_X4                          58
#define sc_x4_control_sc_S_50G_HG2_CR4                     60
#define sc_x4_control_sc_S_50G_HG2_KR4                     61
#define sc_x4_control_sc_S_50G_HG2_X4                      62
#define sc_x4_control_sc_S_100G_CR4                        64
#define sc_x4_control_sc_S_100G_KR4                        65
#define sc_x4_control_sc_S_100G_X4                         66
#define sc_x4_control_sc_S_100G_HG2_CR4                    68
#define sc_x4_control_sc_S_100G_HG2_KR4                    69
#define sc_x4_control_sc_S_100G_HG2_X4                     70
#define sc_x4_control_sc_S_CL73_20GVCO                     72
#define sc_x4_control_sc_S_CL73_25GVCO                     80
#define sc_x4_control_sc_S_CL36_20GVCO                     88
#define sc_x4_control_sc_S_CL36_25GVCO                     96
#define sc_x4_control_sc_S_2P5G_KX1                        98
#define sc_x4_control_sc_S_5G_KR1                          99
#define sc_x4_control_sc_S_25G_CR_IEEE                     112
#define sc_x4_control_sc_S_25G_CRS_IEEE                    113
#define sc_x4_control_sc_S_25G_KR_IEEE                     114
#define sc_x4_control_sc_S_25G_KRS_IEEE                    115

/****************************************************************************
 * Enums: main0_refClkSelect
 */
#define main0_refClkSelect_clk_25MHz                       0
#define main0_refClkSelect_clk_100MHz                      1
#define main0_refClkSelect_clk_125MHz                      2
#define main0_refClkSelect_clk_156p25MHz                   3
#define main0_refClkSelect_clk_187p5MHz                    4
#define main0_refClkSelect_clk_161p25Mhz                   5
#define main0_refClkSelect_clk_50Mhz                       6
#define main0_refClkSelect_clk_106p25Mhz                   7



/**
\struct sc_pmd_dpll_entry_t

This embodies all parameters passed from PCS to PMD. For more details
please refer to the micro-arch document.
*/
typedef struct sc_pmd_entry_t
{
  int pma_os_mode;
  uint16_t pll_mode;
} sc_pmd_entry_st;

extern const sc_pmd_entry_st sc_pmd16_gen3_entry[], sc_pmd16_gen3_entry_125M_ref[];

extern int print_tefmod16_gen3_sc_lkup_table(PHYMOD_ST* pc);
extern int tefmod16_gen3_vl_bml_get(int speed, uint16_t *vl, uint16_t *bml);
extern int tefmod16_gen3_am_bits_get(int speed, uint32_t *am_bits);
extern int tefmod16_gen3_ui_values_get(int speed, uint16_t *ui_msb, uint16_t *ui_lsb);
extern int tefmod16_gen3_mapped_frac_ns_get(int speed, uint16_t *frac_ns);
extern int tefmod16_gen3_get_mapped_speed(tefmod16_gen3_spd_intfc_type_t spd_intf, int *speed);
extern int tefmod16_gen3_txrx_fixed_latency_get(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac);
extern int tefmod16_gen3_no_pmd_txrx_fixed_latency_get(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac);

#endif
