/*----------------------------------------------------------------------
 * $Id: tbhmod_sub10g.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
/*
 * 
 * $Copyright:
 * All Rights Reserved.$
 */

#ifndef _tbhmod_sub10g_H_
#define _tbhmod_sub10g_H_

#ifndef _DV_TB_
 #define _SDK_TBHMOD_SUB10G_ 1
#endif

#ifdef _DV_TB_
#ifdef LINUX
#include <stdint.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

#ifdef _SDK_TBHMOD_SUB10G_
#include <phymod/phymod_debug.h>
#include <tsce_dpll/tier1/temod_dpll_enum_defines.h>
#include <tsce_dpll/tier1/temod_dpll.h>
#include <tsce_dpll/tier1/temod_dpll_device.h>
#include <tsce_dpll/tier1/temod_dpll_sc_lkup_table.h>
#include <tsce_dpll/tier1/tedpll_PCSRegEnums.h>
#endif

#define TSCBH_SUB10G_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = 0xff; \
    }while(0)
#define TSCBH_SUB10G_NOF_LANES_IN_CORE       8
/* So far 4 bits debug mask are used by TBHMOD_SUB10G */
#define TBHMOD_SUB10G_DBG_MEM        (1L << 4) /* allocation/object */
#define TBHMOD_SUB10G_DBG_REGACC     (1L << 3) /* Print all register accesses */
#define TBHMOD_SUB10G_DBG_FUNCVALOUT (1L << 2) /* All values returned by Tier1*/
#define TBHMOD_SUB10G_DBG_FUNCVALIN  (1L << 1) /* All values pumped into Tier1*/
#define TBHMOD_SUB10G_DBG_FUNC       (1L << 0) /* Every time we enter a  Tier1*/

#define TSCBH_SUB10G_LANE_MASK_UPPER(lane_mask) ((lane_mask >> 4) & 0xf)
#define TSCBH_SUB10G_LANE_MASK_LOWER(lane_mask) (lane_mask & 0xf)

#define TSCBH_SUB10G_OS_MODE_2                               1
#define TSCBH_SUB10G_OS_MODE_4                               2
#define TSCBH_SUB10G_OS_MODE_8                               5
#define TSCBH_SUB10G_OS_MODE_16p5                            8

typedef enum {
    TBHMOD_SUB10G_PLL_MODE_DIV_ZERO = 0, /* Divide value to be determined by API. */
    TBHMOD_SUB10G_PLL_MODE_DIV_66         =          (int)0x00000042, /* Divide by 66        */
    TBHMOD_SUB10G_PLL_MODE_DIV_80         =          (int)0x00000050, /* Divide by 80        */
    TBHMOD_SUB10G_PLL_MODE_DIV_82P5       =          (int)0x80000052, /* Divide by 82.5      */
    TBHMOD_SUB10G_PLL_MODE_DIV_85         =          (int)0x00000055, /* Divide by 85        */
    TBHMOD_SUB10G_PLL_MODE_DIV_132        =          (int)0x00000084, /* Divide by 132       */
    TBHMOD_SUB10G_PLL_MODE_DIV_160        =          (int)0x000000A0, /* Divide by 160       */
    TBHMOD_SUB10G_PLL_MODE_DIV_165        =          (int)0x000000A5, /* Divide by 165       */
    TBHMOD_SUB10G_PLL_MODE_DIV_170        =          (int)0x000000AA  /* Divide by 170       */
} tbhmod_sub10g_pll_mode_type;

#define TSCBH_SUB10G_SYNCE_SDM_DIVISOR_10G_PER_LN  5280
#define TSCBH_SUB10G_SYNCE_SDM_DIVISOR_5G_PER_LN  2640
#define TSCBH_SUB10G_SYNCE_SDM_DIVISOR_2P5G_PER_LN  1600
#define TSCBH_SUB10G_SYNCE_SDM_DIVISOR_1G_PER_LN  1280

int tbhmod_sub10g_enable_set(PHYMOD_ST* pc);
int tbhmod_sub10g_update_port_mode(PHYMOD_ST *pc);
int tbhmod_sub10g_plldiv_lkup_get(PHYMOD_ST* pc,
                                  temod2pll_spd_intfc_type spd_intf,
                                  phymod_ref_clk_t ref_clk,
                                  uint32_t *plldiv,
                                  uint16_t *speed_vec);
int tbhmod_sub10g_osmode_lkup_get(PHYMOD_ST* pc,
                                  temod2pll_spd_intfc_type spd_intf,
                                  uint32_t *osmode);
int tbhmod_sub10g_port_start_lane_get(PHYMOD_ST *pc,
                                     int *port_starting_lane,
                                     int *port_num_lane);
int tbhmod_sub10g_osdfe_on_lkup_get(PHYMOD_ST* pc,
                                    temod2pll_spd_intfc_type spd_intf,
                                    uint32_t* osdfe_on);
int tbhmod_sub10g_scrambling_dis_lkup_get(PHYMOD_ST* pc,
                                          temod2pll_spd_intfc_type spd_intf,
                                          uint32_t* scrambling_dis);
int tbhmod_sub10g_set_an_port_mode(PHYMOD_ST* pc,
                                   int enable, int nl_encoded,
                                   int starting_lane, int single_port);
int tbhmod_sub10g_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
int tbhmod_sub10g_pcs_clk_blk_en(PHYMOD_ST* pc, uint32_t en);
int tbhmod_sub10g_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock);
int tbhmod_sub10g_tx_loopback_control(PHYMOD_ST* pc, int enable,
                                      int starting_lane,
                                      int num_lanes);

#endif  /*  _tbhmod_sub10g_H_ */
