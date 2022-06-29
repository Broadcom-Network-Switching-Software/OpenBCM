/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tbhmod_sub10g_cfg_seq.c
 * Description: c functions implementing Tier1s for TBHMod Sub10g Serdes Driver
 *---------------------------------------------------------------------*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tscbh_sub10g_xgxs_defs.h>
#include "tbhmod_sub10g.h"

#define TBHMOD_SUB10G_DBG_IN_FUNC_INFO(pc) \
  PHYMOD_VDBG(TBHMOD_SUB10G_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, pc->addr, pc->lane_mask))
#define TBHMOD_SUB10G_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_SUB10G_DBG_FUNCVALIN,pc,_print_)
#define TBHMOD_SUB10G_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_SUB10G_DBG_FUNCVALOUT,pc,_print_)

static eagle2pll_sc_pmd_entry_st tscbh_sub10g_sc_pmd_entry[] = {
/*SPD_10M              0*/ { 1, TSCBH_SUB10G_OS_MODE_16p5,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  1},
/*SPD_10M              1*/ { 1, TSCBH_SUB10G_OS_MODE_16p5,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  1},
/*SPD_100M             2*/ { 1, TSCBH_SUB10G_OS_MODE_16p5,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  1},
/*SPD_1000M            3*/ { 1, TSCBH_SUB10G_OS_MODE_16p5,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  1},
/*SPD_1G_CX1           4*/ { 1, TSCBH_SUB10G_OS_MODE_16p5,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  1},
/*SPD_1G_KX1           5*/ { 1, TSCBH_SUB10G_OS_MODE_16p5, TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  1},
/*SPD_2p5G             6*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_5G_X1            7*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10G_CX4          8*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10G_KX4          9*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10G_X4          10*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_13G_X4          11*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_15G_X4          12*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_16G_X4          13*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_20G_CX4         14*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10G_CX2         15*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10G_X2          16*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_20G_X4          17*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10p5G_X2        18*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_21G_X4          19*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_12p7G_X2        20*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_25p45G_X4       21*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_15p75G_X2       22*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_31p5G_X4        23*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_31p5G_KR4       24*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_20G_CX2         25*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_20G_X2          26*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_40G_X4          27*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10G_KR1         28*/ { 1, TSCBH_SUB10G_OS_MODE_2,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  1, 0,  0},
/*SPD_10p6_X1         29*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_20G_KR2         30*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_20G_CR2         31*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_21G_X2          32*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_40G_KR4         33*/ { 4, TSCBH_SUB10G_OS_MODE_2,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  1, 0,  0},
/*SPD_40G_CR4         34*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_42G_X4          35*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_100G_CR10       36*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_107G_CR10       37*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_120G_X12        38*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_127G_X12        39*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_12P12_XFI       40*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_24P24_MLD_DXGXS 41*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_48P48_MLD       42*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_21P21G_HG_DXGXS 43*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_42P42G_HG2      44*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         45*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         46*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         47*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         48*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_5G_KR1          49*/ { 1, TSCBH_SUB10G_OS_MODE_4,  TBHMOD_SUB10G_PLL_MODE_DIV_132,  0, 0,  0},
/*SPD_10p5G_X4        50*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         51*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         52*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10M_10p3125     53*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_100M_10p3125    54*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_1000M_10p3125   55*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_2p5G_X1_10p3125 56*/ { 1, TSCBH_SUB10G_OS_MODE_8,TBHMOD_SUB10G_PLL_MODE_DIV_160,  0, 0,  0},
/*SPD_10000_KX4_10P3  57*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10000_CX4_10P3  58*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_10000_X4_10P3   59*/ {-1, -1, -1 , -1, -1, -1},
/*SPD_ILLEGAL         60*/ {-1, -1, -1 , -1, -1, -1}
};


int tbhmod_sub10g_enable_set(PHYMOD_ST* pc)
{
    TSCE_SC_X4_CTLr_t reg_sc_ctrl;

    TSCE_SC_X4_CTLr_CLR(reg_sc_ctrl);

    TSCE_SC_X4_CTLr_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_SC_X4_CTLr(pc,reg_sc_ctrl));

    return PHYMOD_E_NONE;
}

/**
@brief   update the port mode
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pll_restart Indicates if required to restart PLL.
@returns The value PHYMOD_E_NONE upon successful completion
*/
int tbhmod_sub10g_update_port_mode(PHYMOD_ST *pc)
{
    TSCE_MAIN0_SETUPr_t mode_reg;
    int port_mode_sel, port_mode_sel_reg;
    uint32_t single_port_mode;
    uint32_t first_couple_mode = 0, second_couple_mode = 0;
    uint32_t lane_mask;

    port_mode_sel = 0 ;
    single_port_mode = 0 ;

    PHYMOD_IF_ERR_RETURN(READ_TSCE_MAIN0_SETUPr(pc, &mode_reg));
    port_mode_sel_reg = TSCE_MAIN0_SETUPr_PORT_MODE_SELf_GET(mode_reg);

    lane_mask = TSCBH_SUB10G_LANE_MASK_UPPER(pc->lane_mask) ?
                TSCBH_SUB10G_LANE_MASK_UPPER(pc->lane_mask) :
                TSCBH_SUB10G_LANE_MASK_LOWER(pc->lane_mask);

    if (lane_mask == 0xf) {
        port_mode_sel = 4;
    } else {
        first_couple_mode  = ((port_mode_sel_reg == 2) || (port_mode_sel_reg == 3) || (port_mode_sel_reg == 4));
        second_couple_mode = ((port_mode_sel_reg == 1) || (port_mode_sel_reg == 3) || (port_mode_sel_reg == 4));
        switch (lane_mask) {
        case 1:
        case 2:
            first_couple_mode = 0;
            break;
        case 4:
        case 8:
            second_couple_mode = 0;
            break;
        case 3:
            first_couple_mode = 1;
            break;
        case 0xc:
            second_couple_mode = 1;
            break;
        default:
            return PHYMOD_E_FAIL;
            break ;
        }

        if (first_couple_mode ) {
            port_mode_sel = (second_couple_mode) ? 3: 2;
        } else if (second_couple_mode) {
            port_mode_sel = 1;
        } else{
            port_mode_sel = 0 ;
        }
    }

    TSCE_MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(mode_reg, single_port_mode);
    TSCE_MAIN0_SETUPr_PORT_MODE_SELf_SET(mode_reg, port_mode_sel);
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_MAIN0_SETUPr(pc, mode_reg));
    return PHYMOD_E_NONE ;
}

/**
@brief   Get the plldiv from lookup table
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf speed as specified by enum #temod2pll_spd_intfc_type
@param   plldiv Receives the pll divider value
@param   speed_vec  Receives the speed id.
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the plldiv from lookup table
*/
int tbhmod_sub10g_plldiv_lkup_get(PHYMOD_ST* pc,
                                  temod2pll_spd_intfc_type spd_intf,
                                  phymod_ref_clk_t ref_clk,
                                  uint32_t *plldiv,
                                  uint16_t *speed_vec)
{
    int speed_id;

    *plldiv = TBHMOD_SUB10G_PLL_MODE_DIV_ZERO;
    PHYMOD_IF_ERR_RETURN(temod2pll_get_mapped_speed(spd_intf, &speed_id));
    if (ref_clk == phymodRefClk156Mhz) {
        *plldiv = tscbh_sub10g_sc_pmd_entry[speed_id].pll_mode;
    } else {
        return PHYMOD_E_PARAM;
    }
    *speed_vec = speed_id ;
    TBHMOD_SUB10G_DBG_IN_FUNC_VOUT_INFO(pc,("plldiv: %d", *plldiv));

    return PHYMOD_E_NONE;
}

/**
@brief   Get the osmode from lookup table for the given speed
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf the speed type as in enum #temod2pll_spd_intfc_type
@param   osmode receives the osmode
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the osmode from software copy of Speed table
*/
int tbhmod_sub10g_osmode_lkup_get(PHYMOD_ST* pc,
                                  temod2pll_spd_intfc_type spd_intf,
                                  uint32_t *osmode)
{
    int speed_id;

    PHYMOD_IF_ERR_RETURN(temod2pll_get_mapped_speed(spd_intf, &speed_id));
    *osmode = tscbh_sub10g_sc_pmd_entry[speed_id].t_pma_os_mode;
    TBHMOD_SUB10G_DBG_IN_FUNC_VOUT_INFO(pc,("osmode: %d", (int)*osmode));

    return PHYMOD_E_NONE;
}

/**
@brief   Get the osdfe from lookup table for the given speed
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf the speed type as in enum #temod2pll_spd_intfc_type
@param   osdfe_on receives the osdfe_on
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the osdfe_on from software copy of Speed table. Dfe_on:1, Dfe_off=0
*/
int tbhmod_sub10g_osdfe_on_lkup_get(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint32_t* osdfe_on)
{
    int speed_id;

    PHYMOD_IF_ERR_RETURN(temod2pll_get_mapped_speed(spd_intf, &speed_id));
    *osdfe_on = tscbh_sub10g_sc_pmd_entry[speed_id].osdfe_on;
    TBHMOD_SUB10G_DBG_IN_FUNC_VOUT_INFO(pc,("osdfe_on: %d", (int)*osdfe_on));

    return PHYMOD_E_NONE;
}

/**
@brief   Get the scrambling_dis from lookup table for the given speed
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   spd_intf the speed type as in enum #temod2pll_spd_intfc_type
@param   scrambling_dis receives the scrambling_dis
@returns The value PHYMOD_E_NONE upon successful completion
@details Get the scrambling_dis from software copy of Speed table. scrambling_dis:0, scrambling_dis : 1
*/
int tbhmod_sub10g_scrambling_dis_lkup_get(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint32_t* scrambling_dis)
{
    int speed_id;

    PHYMOD_IF_ERR_RETURN(temod2pll_get_mapped_speed(spd_intf, &speed_id));
    *scrambling_dis = tscbh_sub10g_sc_pmd_entry[speed_id].scrambling_dis;
    TBHMOD_SUB10G_DBG_IN_FUNC_VOUT_INFO(pc,("scrambling_dis: %d", (int)*scrambling_dis));

    return PHYMOD_E_NONE;
}

/*For per lane control*/
int tbhmod_sub10g_port_start_lane_get(PHYMOD_ST *pc,
                                     int *port_starting_lane,
                                     int *port_num_lane)
{
    phymod_access_t pc_copy;

    PHYMOD_MEMCPY(&pc_copy, pc, sizeof(pc_copy));
    pc_copy.lane_mask = TSCBH_SUB10G_LANE_MASK_UPPER(pc->lane_mask) ?
                        TSCBH_SUB10G_LANE_MASK_UPPER(pc->lane_mask) :
                        TSCBH_SUB10G_LANE_MASK_LOWER(pc->lane_mask);

    PHYMOD_IF_ERR_RETURN
        (temod2pll_port_start_lane_get(&pc_copy, port_starting_lane,
                                       port_num_lane));

    if (TSCBH_SUB10G_LANE_MASK_UPPER(pc->lane_mask)) {
        *port_starting_lane += 4;
    }

    return PHYMOD_E_NONE;
}

/**
@brief    Set the Port mode related fields for AN enabled ports
@param    pc handle to current TSCE context (#PHYMOD_ST)
@param    nl_encoded Number of lanes in this port
@param    starting_lane first lane in the port
@param    single_port Single port or not
@returns  PHYMOD_E_NONE if successful. PHYMOD_E_FAIL else.
@details  This function sets up the port for an be called multiple times. Elements of #PHYMOD_ST
should be initialized to required values prior to calling this function. The
following sub-configurations are performed here.
*/
int tbhmod_sub10g_set_an_port_mode(PHYMOD_ST* pc, int enable, int nl_encoded, int starting_lane, int single_port)
{
    uint16_t rdata;
    TSCE_MAIN0_SETUPr_t  reg_setup;
    int new_portmode = -1 ;

    if (starting_lane >= 4) {
        starting_lane -= 4;
    }
    TSCE_MAIN0_SETUPr_CLR(reg_setup);
    PHYMOD_IF_ERR_RETURN (READ_TSCE_MAIN0_SETUPr(pc, &reg_setup));

    rdata = TSCE_MAIN0_SETUPr_PORT_MODE_SELf_GET(reg_setup);

    /* nl_encoded: encoded num of lanes adv: say 0->KR, 1->KR2, 2->KR4, 3->CR10*/
    if ((nl_encoded == 2) || (nl_encoded == 3)) {
        TSCE_MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_setup, 0);
    } else {
        if (starting_lane == 0 || starting_lane == 1){
            switch(rdata) {
              case 0x2: new_portmode = 0; break ;
              case 0x3: new_portmode = 1; break ;
              case 0x4: new_portmode = 1; break ;
              default: break ;
            }
        }
        if (starting_lane == 2 || starting_lane == 3) {
            switch(rdata) {
              case 0x1: new_portmode = 0; break ;
              case 0x3: new_portmode = 2; break ;
              case 0x4: new_portmode = 2; break ;
              default: break ;
            }
        }
        if ((enable) && (new_portmode>=0)) { /* only AN enable to change this field */
            TSCE_MAIN0_SETUPr_PORT_MODE_SELf_SET(reg_setup, new_portmode);
        }
    }
    /*Setting single port mode*/
    TSCE_MAIN0_SETUPr_SINGLE_PORT_MODEf_SET(reg_setup, single_port);

    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_MAIN0_SETUPr(pc,reg_setup));

    return PHYMOD_E_NONE;
}
/**
@brief   Init the PMD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   pmd_touched If the PMD is already initialized
@returns The value PHYMOD_E_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
int tbhmod_sub10g_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched)
{
    TSCE_PMD_X1_CTLr_t reg_pmd_x1_ctrl;

    TSCE_PMD_X1_CTLr_CLR(reg_pmd_x1_ctrl);

    if (pmd_touched == 0) {
        TSCE_PMD_X1_CTLr_POR_H_RSTBf_SET(reg_pmd_x1_ctrl,0);
        TSCE_PMD_X1_CTLr_CORE_DP_H_RSTB_PLL0f_SET(reg_pmd_x1_ctrl,0);
        TSCE_PMD_X1_CTLr_CORE_DP_H_RSTB_PLL1f_SET(reg_pmd_x1_ctrl,0);
        PHYMOD_IF_ERR_RETURN(WRITE_TSCE_PMD_X1_CTLr(pc,reg_pmd_x1_ctrl));

        TSCE_PMD_X1_CTLr_POR_H_RSTBf_SET(reg_pmd_x1_ctrl,1);
        TSCE_PMD_X1_CTLr_CORE_DP_H_RSTB_PLL0f_SET(reg_pmd_x1_ctrl,1);
        TSCE_PMD_X1_CTLr_CORE_DP_H_RSTB_PLL1f_SET(reg_pmd_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(WRITE_TSCE_PMD_X1_CTLr(pc,reg_pmd_x1_ctrl));
    }

    return PHYMOD_E_NONE;
}

/* Enable PCS clock block */
int tbhmod_sub10g_pcs_clk_blk_en(PHYMOD_ST* pc, uint32_t en)
{
    AMS_PLL_PLL_CTL_12r_t reg_pll_ctrl;

    AMS_PLL_PLL_CTL_12r_CLR(reg_pll_ctrl);
    AMS_PLL_PLL_CTL_12r_AMS_PLL_EN_CLK4PCSf_SET(reg_pll_ctrl, en);

    PHYMOD_IF_ERR_RETURN
       (MODIFY_AMS_PLL_PLL_CTL_12r(pc, reg_pll_ctrl));

    return PHYMOD_E_NONE;
}

int tbhmod_sub10g_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock)
{
    TSCE_MAIN0_SETUPr_t  reg_setup;


    TSCE_MAIN0_SETUPr_CLR(reg_setup);
    PHYMOD_IF_ERR_RETURN (READ_TSCE_MAIN0_SETUPr(pc, &reg_setup));

    if(ref_clock == phymodRefClk156Mhz) {
        TSCE_MAIN0_SETUPr_REFCLK_SELf_SET(reg_setup, 3);
    } else {
        return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_MAIN0_SETUPr(pc,reg_setup));

    return PHYMOD_E_NONE;
}

/**
@brief   Sets loopback mode at PCS/PMD parallel interface.(gloop).
@param   pc handle to current TSCE context (#PHYMOD_ST)
@param   enable controls enabling loopback
@param   starting_lane lane 0 of this port
@param   num_lanes Number of lanes of this port
@returns The value PHYMOD_E_NONE upon successful completion
@details This function sets the TX-to-RX digital loopback mode at the PCS/PMD parallel
interface, which is set based on port_type.

\li 0:1 : Enable  TX->RX loopback
\li 0:0 : Disable TX->RX loopback
*/
int tbhmod_sub10g_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int num_lanes)
{
    TSCE_MAIN0_LPBK_CTLr_t reg_lb_ctrl;
    TSCE_PMD_X4_OVRRr_t    reg_pmd_ovr;
    TSCE_PMD_X4_CTLr_t     reg_pmd_ctrl;
    uint16_t               lane_mask, i, data, tmp_data;

    lane_mask = 0;
    READ_TSCE_MAIN0_LPBK_CTLr(pc, &reg_lb_ctrl);
    tmp_data = TSCE_MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_GET(reg_lb_ctrl);
    TSCE_PMD_X4_OVRRr_CLR(reg_pmd_ovr);
    TSCE_PMD_X4_CTLr_CLR(reg_pmd_ctrl);

    lane_mask = 0;
    data = 0;
    for (i = 0; i < num_lanes; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(pc->lane_mask, starting_lane + i)) {
            continue;
        }
        lane_mask |= 1 << (starting_lane + i);
        data |= enable << (starting_lane + i);
    }

    lane_mask = TSCBH_SUB10G_LANE_MASK_UPPER(lane_mask) ?
                TSCBH_SUB10G_LANE_MASK_UPPER(lane_mask) :
                TSCBH_SUB10G_LANE_MASK_LOWER(lane_mask);

    data = TSCBH_SUB10G_LANE_MASK_UPPER(data) ?
           TSCBH_SUB10G_LANE_MASK_UPPER(data) :
           TSCBH_SUB10G_LANE_MASK_LOWER(data);

    tmp_data &= ~lane_mask;
    tmp_data |= data;
    TSCE_MAIN0_LPBK_CTLr_LOCAL_PCS_LOOPBACK_ENABLEf_SET(reg_lb_ctrl, tmp_data);

    PHYMOD_IF_ERR_RETURN
        (MODIFY_TSCE_MAIN0_LPBK_CTLr(pc, reg_lb_ctrl));

    /* signal_detect and rx_lock */
    if (enable) {
        TSCE_PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(reg_pmd_ovr, 1);
        TSCE_PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(reg_pmd_ovr, 1);
        TSCE_PMD_X4_OVRRr_TX_DISABLE_OENf_SET(reg_pmd_ovr, 1);
    } else {
        TSCE_PMD_X4_OVRRr_RX_LOCK_OVRDf_SET(reg_pmd_ovr, 0);
        TSCE_PMD_X4_OVRRr_SIGNAL_DETECT_OVRDf_SET(reg_pmd_ovr, 0);
        TSCE_PMD_X4_OVRRr_TX_DISABLE_OENf_SET(reg_pmd_ovr, 0);
    }

    PHYMOD_IF_ERR_RETURN
        (MODIFY_TSCE_PMD_X4_OVRRr(pc, reg_pmd_ovr));

    /* set tx_disable */
    if (enable) {
        TSCE_PMD_X4_CTLr_TX_DISABLEf_SET(reg_pmd_ctrl, 1);
    } else {
        TSCE_PMD_X4_CTLr_TX_DISABLEf_SET(reg_pmd_ctrl, 0);
    }
    /* mask = PMD_X4_CONTROL_TX_DISABLE_MASK ; */
    PHYMOD_IF_ERR_RETURN (MODIFY_TSCE_PMD_X4_CTLr(pc, reg_pmd_ctrl));

    /* When the phy is put in gloop, the rx clk changes, so need to reset the rxp to clear the rx pipe */
    PHYMOD_IF_ERR_RETURN
        (temod2pll_rx_lane_control_set(pc, 1));

    /* Tscf uses following code but tsce needs above code */
    /* related JIRA is SDK-80414 */
    /* trigger speed change */
    /* temod2pll_trigger_speed_change(pc); */

    return PHYMOD_E_NONE;
}

/*Set the delay from CMIC to the PCS*/
int tbhmod_sub10g_1588_ts_offset_set(PHYMOD_ST* pc, uint32_t ts_offset)
{
    TSCE_MAIN0_TS_TMR_OFFSr_t TS_TMR_OFFSr_reg;
    TSCE_MAIN0_TS_TMR_OFFSr_CLR(TS_TMR_OFFSr_reg);

    /* set ts_offset */
    PHYMOD_IF_ERR_RETURN(READ_TSCE_MAIN0_TS_TMR_OFFSr(pc, &TS_TMR_OFFSr_reg));
    TSCE_MAIN0_TS_TMR_OFFSr_SET(TS_TMR_OFFSr_reg, ts_offset);
    PHYMOD_IF_ERR_RETURN(WRITE_TSCE_MAIN0_TS_TMR_OFFSr(pc, TS_TMR_OFFSr_reg));
    return PHYMOD_E_NONE;
}

/* Get the delay from CMIC to the PCS */
int tbhmod_sub10g_1588_ts_offset_get(PHYMOD_ST* pc, uint32_t *ts_offset)
{
    TSCE_MAIN0_TS_TMR_OFFSr_t reg_ts_offset_ctrl;
    TSCE_MAIN0_TS_TMR_OFFSr_CLR(reg_ts_offset_ctrl);

    /* set ts_offset */
    PHYMOD_IF_ERR_RETURN(READ_TSCE_MAIN0_TS_TMR_OFFSr(pc, &reg_ts_offset_ctrl));
    *ts_offset = TSCE_MAIN0_TS_TMR_OFFSr_U_TS_OFFSETf_GET(reg_ts_offset_ctrl);

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tdmod_pcs_ts_en per port.
@param unit number for instance lane number for decide which lane
@param en tells enabling/disabling r_test_mode AKA scrmable_idle_en
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Enables ts in rx path */
int tbhmod_sub10g_1588_ts_config_set(PHYMOD_ST* pc, int byte_offset, int rpm_mode)
{

    TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_t  RX_X4_RXP_1588_SFD_TS_CTLr_reg;
    TSCE_TX_X4_TX_TS_POINT_CTLr_t      TX_X4_TX_TS_POINT_CTLr_reg;
    TSCE_MAIN0_TS_TMR_OFFSr_t          TS_TMR_OFFSr_reg;
    int is_hg, ts_clk_period;
    int pcs_1588_osts_crc_calc_mode;

    TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_CLR(RX_X4_RXP_1588_SFD_TS_CTLr_reg);
    TSCE_TX_X4_TX_TS_POINT_CTLr_CLR(TX_X4_TX_TS_POINT_CTLr_reg);
    TSCE_MAIN0_TS_TMR_OFFSr_CLR(TS_TMR_OFFSr_reg);

    PHYMOD_IF_ERR_RETURN(temod2pll_hg_enable_get(pc, &is_hg));
    if (is_hg) {
        pcs_1588_osts_crc_calc_mode = 1;
    } else {
        pcs_1588_osts_crc_calc_mode = 0;
    }

    /* select RXP sop or sfd timestamping */
    PHYMOD_IF_ERR_RETURN(READ_TSCE_RX_X4_RXP_1588_SFD_TS_CTLr(pc, &RX_X4_RXP_1588_SFD_TS_CTLr_reg));
    if (byte_offset == TSCBH_SUB10G_TS_OFFSET_SOP) {
        TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_SFD_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 0);
        TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_DA_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 0);
    } else if (byte_offset == TSCBH_SUB10G_TS_OFFSET_SFD) {
        TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_SFD_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 1);
        TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_DA_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 0);
    } else if (byte_offset == TSCBH_SUB10G_TS_OFFSET_MAC_DA) {
        TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_SFD_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 0);
        TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_DA_TS_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, 1);
    }
    TSCE_RX_X4_RXP_1588_SFD_TS_CTLr_RED_PA_ENf_SET(RX_X4_RXP_1588_SFD_TS_CTLr_reg, rpm_mode);
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_RX_X4_RXP_1588_SFD_TS_CTLr(pc, RX_X4_RXP_1588_SFD_TS_CTLr_reg));

    /* ts_clk is 500M. ts_clk_period=1/500M * 10^9 ns */
    ts_clk_period = 2;
    TSCE_MAIN0_TS_TMR_OFFSr_SET(TS_TMR_OFFSr_reg, ts_clk_period*2);
    PHYMOD_IF_ERR_RETURN(WRITE_TSCE_MAIN0_TS_TMR_OFFSr(pc, TS_TMR_OFFSr_reg));

    /* select TXP sop or sfd timestamping, osts_crc_calc_mode, ts interrupt enable */
    PHYMOD_IF_ERR_RETURN(READ_TSCE_TX_X4_TX_TS_POINT_CTLr(pc, &TX_X4_TX_TS_POINT_CTLr_reg));
    if (byte_offset == TSCBH_SUB10G_TS_OFFSET_SOP) {
        TSCE_TX_X4_TX_TS_POINT_CTLr_TX_SFD_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 0);
        TSCE_TX_X4_TX_TS_POINT_CTLr_TX_DA_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 0);
    } else if (byte_offset == TSCBH_SUB10G_TS_OFFSET_SFD) {
        TSCE_TX_X4_TX_TS_POINT_CTLr_TX_SFD_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 1);
        TSCE_TX_X4_TX_TS_POINT_CTLr_TX_DA_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 0);
    } else if (byte_offset == TSCBH_SUB10G_TS_OFFSET_MAC_DA) {
        TSCE_TX_X4_TX_TS_POINT_CTLr_TX_SFD_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 0);
        TSCE_TX_X4_TX_TS_POINT_CTLr_TX_DA_TIMESTAMP_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, 1);
    }
    TSCE_TX_X4_TX_TS_POINT_CTLr_OSTS_CRC_CALC_MODEf_SET(TX_X4_TX_TS_POINT_CTLr_reg, pcs_1588_osts_crc_calc_mode);
    TSCE_TX_X4_TX_TS_POINT_CTLr_TX_REDUCE_PRE_ENf_SET(TX_X4_TX_TS_POINT_CTLr_reg, rpm_mode);
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_TX_X4_TX_TS_POINT_CTLr(pc, TX_X4_TX_TS_POINT_CTLr_reg));

    return PHYMOD_E_NONE;
}

int tbhmod_sub10g_set_fclk_period(PHYMOD_ST* pc)
{
    int fclk_frac_ns_t;
    TSCE_PMD_X1_FCLK_PERIODr_t TSCE_PMD_X1_FCLK_PERIODr_reg;

    TSCE_PMD_X1_FCLK_PERIODr_CLR(TSCE_PMD_X1_FCLK_PERIODr_reg);

    /* frequency 20.625 G 6000/20625 = 0.290909 0.290909 * (1024 * 64) = 19065.02 */
    fclk_frac_ns_t = 19065;

    TSCE_PMD_X1_FCLK_PERIODr_SET(TSCE_PMD_X1_FCLK_PERIODr_reg, fclk_frac_ns_t);
    PHYMOD_IF_ERR_RETURN
        (MODIFY_TSCE_PMD_X1_FCLK_PERIODr(pc, TSCE_PMD_X1_FCLK_PERIODr_reg));

    return PHYMOD_E_NONE;
}

#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
/* LANE RATE */
static double
_tbhmod_sub10g_pcs_port_speed_in_mbps_get(int speed)
{
    if(speed == digital_operationSpeeds_SPEED_10M_10p3125)     return 12.5;
    if(speed == digital_operationSpeeds_SPEED_100M_10p3125)    return 125.0;
    if(speed == digital_operationSpeeds_SPEED_10M)             return 12.5;
    if(speed == digital_operationSpeeds_SPEED_100M)            return 125.0;
    if(speed == digital_operationSpeeds_SPEED_1000M_10p3125)   return 1250.0;
    if(speed == digital_operationSpeeds_SPEED_1000M)           return 1250.0;
    if(speed == digital_operationSpeeds_SPEED_1G_KX1)          return 1250.0;
    if(speed == digital_operationSpeeds_SPEED_2p5G_X1_10p3125) return 3125.0;
    if(speed == digital_operationSpeeds_SPEED_2p5G_X1)         return 3125.0;
    if(speed == digital_operationSpeeds_SPEED_5G_KR1)          return 5156.0;
    if(speed == digital_operationSpeeds_SPEED_5G_X1 )          return 6250.0;
    if(speed == digital_operationSpeeds_SPEED_10G_KR1)         return 10312.5;
    if(speed == digital_operationSpeeds_SPEED_12p12G_X1)       return 12500.0;
    if(speed == digital_operationSpeeds_SPEED_20G_KR2)         return (10312.5*2.0);
    if(speed == digital_operationSpeeds_SPEED_20G_CR2)         return (10312.5*2.0);
    if(speed == digital_operationSpeeds_SPEED_24p24G_X2)       return (12500.0*2.0);
    if(speed == digital_operationSpeeds_SPEED_40G_KR4)         return (10312.5*4.0);
    if(speed == digital_operationSpeeds_SPEED_40G_CR4)         return (10312.5*4.0);
    if(speed == digital_operationSpeeds_SPEED_48p48G_X4)       return (12500.0*4.0);

    return 0.0;
}

static float
_tbhmod_sub10g_pcs_bit_ui_value_calc(int speed_id, int num_pcs_lanes)
{
    float speed_in_mbps, bit_ui_val;

    speed_in_mbps = _tbhmod_sub10g_pcs_port_speed_in_mbps_get(speed_id);
    bit_ui_val = (float)((1000.0/speed_in_mbps)*num_pcs_lanes);

    return bit_ui_val;
}

static uint32_t
_tbhmod_sub10g_pcs_ui_value_get(int speed, int num_pcs_lanes)
{
    /*(1000.0/speed_in_mbps)*num_pcs_lanes * 2 ^ 32*/
    if(speed == digital_operationSpeeds_SPEED_10M_10p3125) {
        /*1000/12.5 * 1 * 4294967296*/
        return 0x0;
    }
    if(speed == digital_operationSpeeds_SPEED_100M_10p3125) {
        /*1000/125 * 1 * 4294967296*/
        return 0x0;
    }
    if(speed == digital_operationSpeeds_SPEED_10M) {
        /*1000/12.5 * 1 * 4294967296*/
        return 0x0;
    }
    if(speed == digital_operationSpeeds_SPEED_100M) {
        /*1000/125 * 1 * 4294967296*/
        return 0x0;
    }
    if(speed == digital_operationSpeeds_SPEED_1000M_10p3125) {
        /*1000/1250 * 1 * 4294967296*/
        return 0xCCCCCCCC;
    }
    if(speed == digital_operationSpeeds_SPEED_1000M) {
        /*1000/1250 * 1 * 4294967296*/
        return 0xCCCCCCCC;
    }
    if(speed == digital_operationSpeeds_SPEED_1G_KX1) {
        /*1000/1250 * 1 * 4294967296*/
        return 0xCCCCCCCC;
    }
    if(speed == digital_operationSpeeds_SPEED_2p5G_X1_10p3125) {
        /*1000/3125 * 1 * 4294967296*/
        return 0x51EB851E;
    }
    if(speed == digital_operationSpeeds_SPEED_2p5G_X1) {
        /*1000/3125 * 1 * 4294967296*/
        return 0x51EB851E;
    }
    if(speed == digital_operationSpeeds_SPEED_5G_KR1) {
        /*1000/5156 * 1 * 4294967296*/
        return 0x31A6A0DE;
    }
    if(speed == digital_operationSpeeds_SPEED_5G_X1 ) {
        /*1000/6250 * 1 * 4294967296*/
        return 0x28F5C28F;
    }
    if(speed == digital_operationSpeeds_SPEED_10G_KR1) {
        /*1000/10312.5 * 1 * 4294967296*/
        return 0x18D3018D;
    }
    if(speed == digital_operationSpeeds_SPEED_12p12G_X1) {
        /*1000/12500 * 1 * 4294967296*/
        return 0x147AE147;
    }
    if(speed == digital_operationSpeeds_SPEED_20G_KR2) {
        /*1000/(10312.5 * 2) * 2 * 4294967296*/
        return 0x18D3018D;
    }
    if(speed == digital_operationSpeeds_SPEED_20G_CR2) {
        /*1000/(10312.5 * 2) * 2 * 4294967296*/
        return 0x18D3018D;
    }
    if(speed == digital_operationSpeeds_SPEED_24p24G_X2) {
        /*1000/(12500 * 2) * 2 * 4294967296*/
        return 0x147AE147;
    }
    if(speed == digital_operationSpeeds_SPEED_40G_KR4) {
        /*1000/(10312.5 * 4) * 4 * 4294967296*/
        return 0x18D3018D;
    }
    if(speed == digital_operationSpeeds_SPEED_40G_CR4) {
        /*1000/(10312.5 * 4) * 4 * 4294967296*/
        return 0x18D3018D;
    }
    if(speed == digital_operationSpeeds_SPEED_48p48G_X4) {
        /*1000/(12500 * 4) * 4 * 4294967296*/
        return 0x147AE147;
    }

    return 0x0;

}


static uint32_t
_tbhmod_sub10g_pcs_ui_value_calc(int speed_id, int num_pcs_lanes)
{
    uint32_t ui_value;

    ui_value = 0;

    ui_value = _tbhmod_sub10g_pcs_ui_value_get(speed_id, num_pcs_lanes);

    ui_value = ui_value >> 9; /* making 23 bit */

    return ui_value;
}
#endif

/*!
 *  @brief tdmod_set_1588_ui per port.
@param unit number for instance lane number for decide which lane
@param vco tells which vco is used in the lane
@param os_mode tells which os_mode is used in the lane
@param clk4sync_div the fast clk divider 0 => 8 and 1 => 6
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Enables ts in rx path */
int tbhmod_sub10g_bit_ui_set(PHYMOD_ST* pc, int speed_id, int num_pcs_lanes)
{

#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    TSCE_PMD_X4_UI_VALUE_HIr_t PMD_X4_UI_VALUE_HIr_reg;
    TSCE_PMD_X4_UI_VALUE_LOr_t PMD_X4_UI_VALUE_LOr_reg;

    int ui_value_hi, ui_value_lo;


    ui_value_hi = _tbhmod_sub10g_pcs_ui_value_calc(speed_id, num_pcs_lanes)>>7;
    ui_value_lo = _tbhmod_sub10g_pcs_ui_value_calc(speed_id, num_pcs_lanes)&0x7f;
    /* ui_value_lo = tdmod_pcs_calc_ui_value(speed_id, num_pcs_lanes); */

    TSCE_PMD_X4_UI_VALUE_HIr_CLR(PMD_X4_UI_VALUE_HIr_reg);
    TSCE_PMD_X4_UI_VALUE_LOr_CLR(PMD_X4_UI_VALUE_LOr_reg);

    TSCE_PMD_X4_UI_VALUE_HIr_UI_FRAC_M1_TO_M16f_SET( PMD_X4_UI_VALUE_HIr_reg, ui_value_hi);
    TSCE_PMD_X4_UI_VALUE_LOr_UI_FRAC_M17_TO_M23f_SET(PMD_X4_UI_VALUE_LOr_reg, ui_value_lo);

    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_PMD_X4_UI_VALUE_HIr(pc, PMD_X4_UI_VALUE_HIr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_PMD_X4_UI_VALUE_LOr(pc, PMD_X4_UI_VALUE_LOr_reg));
#endif

  return PHYMOD_E_NONE;
}

#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
/*!
 *  @brief tdmod_calc_pmd_latency per port.
@param unit number for instance lane number for decide which lane
@param vco tells which vco is used in the lane
@param os_mode tells which os_mode is used in the lane
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Enables ts in rx path */
static uint32_t
_tbhmod_sub10g_pmd_latency_calc(int speed_id, int num_pcs_lanes,
                                int tx_or_rx, int pmd_present)
{
    uint32_t tx_latency, rx_latency;
    float bit_ui_val, rx_clk;

    switch(speed_id) {
        case digital_operationSpeeds_SPEED_1G_KX1            : { rx_clk = 0.9697; break;}
        case digital_operationSpeeds_SPEED_1000M             : { rx_clk = 0.9697; break;}
        case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   : { rx_clk = 1.6; break;}
        case digital_operationSpeeds_SPEED_5G_X1             : { rx_clk = 3.2; break;}
        case digital_operationSpeeds_SPEED_5G_KR1            : { rx_clk = 3.8788; break;}
        case digital_operationSpeeds_SPEED_10G_KR1           : { rx_clk = 1.9394; break;}
        case digital_operationSpeeds_SPEED_20G_KR2           : { rx_clk = 1.9394; break;}
        case digital_operationSpeeds_SPEED_20G_CR2           : { rx_clk = 1.9394; break;}
        case digital_operationSpeeds_SPEED_40G_KR4           : { rx_clk = 1.9394; break;}
        case digital_operationSpeeds_SPEED_40G_CR4           : { rx_clk = 1.9394; break;}
        default : { rx_clk = 0; break;}
    }

    bit_ui_val = _tbhmod_sub10g_pcs_bit_ui_value_calc(speed_id, num_pcs_lanes);

    /* the TX PMD latency format is 12Bits of nano and 4bits of subnano */
    if (pmd_present) {
        /* tx_latency = ((uint32_t)((bit_ui_val * 144.0) *16.0)); */
        switch(speed_id) {
          case digital_operationSpeeds_SPEED_1G_KX1:
              {
                  tx_latency = ((uint32_t)((bit_ui_val *  10.0) * 16.0) + (53.4 * 16.0) + (0.4 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_1000M:
              {
                  tx_latency = ((uint32_t)((bit_ui_val *  10.0) * 16.0) + (53.4 * 16.0) + (0.4 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_2p5G_X1_10p3125:
              {
                  tx_latency = ((uint32_t)((bit_ui_val *  38.0) * 16.0) + (22.9 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_5G_X1:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 154.0) * 16.0) + (19.5 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_5G_KR1:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 154.0) * 16.0) + (23.75 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_10G_KR1:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 153.0) * 16.0) + (9.697 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_20G_KR2:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 153.0) * 16.0) + (9.697 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_20G_CR2:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 153.0) * 16.0) + (9.697 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_40G_KR4:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 153.0) * 16.0) + (9.697 * 16.0));
                  break;
              }
          case digital_operationSpeeds_SPEED_40G_CR4:
              {
                  tx_latency = ((uint32_t)((bit_ui_val * 153.0) * 16.0) + (9.697 * 16.0));
                  break;
              }
          default :
              {
                  tx_latency = 0;
                  break;
              }
        }
    } else {
        tx_latency = ((uint32_t)((bit_ui_val * 144.0) *16.0));
    }

/*
  if(speed_id == TSC_SPEED_ID_1G_KX1) tx_latency = tx_latency >> 2;
  if(speed_id == TSC_SPEED_ID_2P5G_X1_10P3125) tx_latency = tx_latency >> 1;
*/

  /* tx_latency += (tx_clk + (40.0 * bit_ui_val)) * 16.0; */



    /* The RX PMD measured latency is 70Bits of UI and 40Bits of UI is added
    * for the glas pipeline delay in the DUT just like the cmic timer flop stages.
    */
    rx_latency = 3.0 * 1024.0 * rx_clk;
    if((speed_id == digital_operationSpeeds_SPEED_1G_KX1) ||
       (speed_id == digital_operationSpeeds_SPEED_1000M)) {
        rx_latency += (1024.0 * 15.0 * bit_ui_val);
    }

    if (pmd_present) {
        /* rx_latency = ((uint32_t)((bit_ui_val * (78 + 40)) *1024)); */
        switch(speed_id) {
            case digital_operationSpeeds_SPEED_1G_KX1            :
                {
                    rx_latency += ((uint32_t)((bit_ui_val *  10.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_1000M             :
                {
                    rx_latency += ((uint32_t)((bit_ui_val *  10.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_2p5G_X1_10p3125   :
                {
                    rx_latency += ((uint32_t)((bit_ui_val *  70.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_5G_X1             :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 121.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_5G_KR1            :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 123.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_10G_KR1           :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 141.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_20G_KR2           :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 141.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_20G_CR2           :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 141.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_40G_KR4           :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 141.0) *1024.0));
                    break;
                }
            case digital_operationSpeeds_SPEED_40G_CR4           :
                {
                    rx_latency += ((uint32_t)((bit_ui_val * 141.0) *1024.0));
                    break;
                }
            default :
                {
                    rx_latency = 0;
                    break;
                }
        }
    }

    if (tx_or_rx == 1) {
        return rx_latency;
    } else {
        return tx_latency;
    }
}
#endif

/*!
 *  @brief tdmod_set_tx_pmd_latency per port.
@param unit number for instance lane number for decide which lane
@param vco tells which vco is used in the lane
@param os_mode tells which os_mode is used in the lane
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Enables ts in rx path */
int tbhmod_sub10g_tx_pmd_latency_set(PHYMOD_ST* pc, int speed_id, int num_pcs_lanes)
{
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    uint32_t tx_latency;
    TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_t PMD_X4_PMD_TX_FIXED_LATENCYr_reg;

    tx_latency = _tbhmod_sub10g_pmd_latency_calc(speed_id, num_pcs_lanes, 0, 1);

    TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_CLR(PMD_X4_PMD_TX_FIXED_LATENCYr_reg);

    TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_INT_NSf_SET (PMD_X4_PMD_TX_FIXED_LATENCYr_reg, (tx_latency >> 4));
    TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_FRAC_NSf_SET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg, ((tx_latency & 0xf)<<2)); /* Frectional bit 6 [5:0] */

    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr(pc, PMD_X4_PMD_TX_FIXED_LATENCYr_reg));
#endif

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tdmod_set_rx_pmd_latency per port.
 */
int tbhmod_sub10g_rx_pmd_latency_set(PHYMOD_ST* pc, int speed_id, int num_pcs_lanes)
{
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    uint32_t rx_latency;
    TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_INTr_t PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg;
    TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_t PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg;

    rx_latency = _tbhmod_sub10g_pmd_latency_calc(speed_id, num_pcs_lanes, 1, 1);

    TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_INTr_CLR(PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg);
    TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_CLR(PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg);


    /*program receive fixed latency - int*/
    TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_INTr_TAB_DP_INT_NSf_SET(PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg, (rx_latency >> 10));
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_INTr(pc, PMD_X4_PMD_RX_FIXED_LATENCY_INTr_reg));

    /*program receive fixed latency - frac*/
    TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_TAB_DP_FRAC_NSf_SET(PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg, (rx_latency & 0x3ff));
    PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_PMD_X4_PMD_RX_FIXED_LATENCY_FRACr(pc, PMD_X4_PMD_RX_FIXED_LATENCY_FRACr_reg));
#endif

    return PHYMOD_E_NONE;
}

#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
/*!
 *  @brief tdmod_set_tx_lane_skew_capture per port.
@param unit number for instance lane number for decide which lane
@param vco tells which vco is used in the lane
@param os_mode tells which os_mode is used in the lane
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Enables ts in rx path */
static int
_tbhmod_sub10g_tx_lane_skew_capture_set(PHYMOD_ST* pc, int num_pcs_lanes, int *tx_lane_swap)
{

    int i;
    uint8_t lane_mask;
    phymod_access_t pa_copy;
    int start_lane = 0, num_lane = 0;

    PKTGEN_GLASTEST_CTLr_t   PKTGEN_GLASTEST_CTLr_reg;

    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    lane_mask =0xf;
    for (i=0; i<num_pcs_lanes; i++) {
        lane_mask &= ((1<<tx_lane_swap[(start_lane % 4) + i]) ^ 0xf);
    }

    PKTGEN_GLASTEST_CTLr_CLR(PKTGEN_GLASTEST_CTLr_reg);
    PKTGEN_GLASTEST_CTLr_GLASTEST_ONESHOTf_SET(PKTGEN_GLASTEST_CTLr_reg, 1);
    PKTGEN_GLASTEST_CTLr_GLASTEST_ENf_SET(PKTGEN_GLASTEST_CTLr_reg, 0);
    PKTGEN_GLASTEST_CTLr_GLASTEST_TPMA_SNAPSHOT_MASKf_SET(PKTGEN_GLASTEST_CTLr_reg, lane_mask);

    if (start_lane < 4) {
        pa_copy.lane_mask = 0x1;
    } else {
        pa_copy.lane_mask = 0x10;
    }
    PHYMOD_IF_ERR_RETURN(MODIFY_PKTGEN_GLASTEST_CTLr(&pa_copy, PKTGEN_GLASTEST_CTLr_reg));

    return PHYMOD_E_NONE;
}

static int
_tbhmod_sub10g_is_tx_lane_skew_capture_done(PHYMOD_ST* pc, int *done)
{
    PKTGEN_GLASTEST_CTLr_t   PKTGEN_GLASTEST_CTLr_reg;

    PKTGEN_GLASTEST_CTLr_CLR(PKTGEN_GLASTEST_CTLr_reg);
    PHYMOD_IF_ERR_RETURN(READ_PKTGEN_GLASTEST_CTLr(pc, &PKTGEN_GLASTEST_CTLr_reg));
    *done = PKTGEN_GLASTEST_CTLr_GLASTEST_ONESHOTf_GET(PKTGEN_GLASTEST_CTLr_reg) ^ 1; /* invert of one shot */
    return PHYMOD_E_NONE;
}

static int
_tbhmod_sub10g_tx_lane_skew_measure(PHYMOD_ST* pc, int speed_id, int num_pcs_lanes,
                                    phymod_timesync_compensation_mode_t timesync_am_norm_mode,
                                    int *tx_lane_swap,
                                    uint32_t *tx_max_skew,
                                    uint32_t *tx_per_lane_skew,
                                    int *tx_anchor_lane)
{
    int i, ln;
    uint32_t tx_min_skew_ln, tx_max_skew_ln, tx_lane_skew_min, tx_lane_skew_max;
    float fclk_period, ui_value ;
    uint32_t dat_15_0, dat_adj, fclk_frac_ns;
    uint32_t tx_lane_skew_bits[4] = {0, 0, 0, 0};
    int start_lane = 0, num_lane = 0;
    phymod_access_t pa_copy;

    TSCE_PMD_X1_FCLK_PERIODr_t TSCE_PMD_X1_FCLK_PERIODr_reg;

    PKTGEN_GLASTEST_STS_SELr_t  PKTGEN_GLASTEST_STS_SELr_reg;
    PKTGEN_GLASTEST_ACTDATA0r_t PKTGEN_GLASTEST_ACTDATA0r_reg;
    PKTGEN_GLASTEST_ACTDATA1r_t PKTGEN_GLASTEST_ACTDATA1r_reg;
    PKTGEN_GLASTEST_ACTDATA2r_t PKTGEN_GLASTEST_ACTDATA2r_reg;
    PKTGEN_GLASTEST_ACTADJr_t   PKTGEN_GLASTEST_ACTADJr_reg;

    TSCE_PMD_X1_FCLK_PERIODr_CLR(  TSCE_PMD_X1_FCLK_PERIODr_reg);
    PKTGEN_GLASTEST_ACTDATA0r_CLR( PKTGEN_GLASTEST_ACTDATA0r_reg);
    PKTGEN_GLASTEST_ACTDATA1r_CLR( PKTGEN_GLASTEST_ACTDATA1r_reg);
    PKTGEN_GLASTEST_ACTDATA2r_CLR( PKTGEN_GLASTEST_ACTDATA2r_reg);
    PKTGEN_GLASTEST_ACTADJr_CLR(   PKTGEN_GLASTEST_ACTADJr_reg);

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    PHYMOD_IF_ERR_RETURN(READ_TSCE_PMD_X1_FCLK_PERIODr(pc, &TSCE_PMD_X1_FCLK_PERIODr_reg));
    fclk_frac_ns = TSCE_PMD_X1_FCLK_PERIODr_FRAC_NSf_GET(TSCE_PMD_X1_FCLK_PERIODr_reg);
    fclk_period = fclk_frac_ns/(1024.0 * 64.0); /* all 16 bit fraction dividing by 2^16 */

    ui_value = _tbhmod_sub10g_pcs_bit_ui_value_calc(speed_id, num_pcs_lanes);

    *tx_max_skew = 0;
    *tx_anchor_lane = start_lane;
    for(i=0; i<4; i++) {
        tx_per_lane_skew[i] = 0;
    }

    tx_min_skew_ln = 0;
    tx_max_skew_ln = 0;
    tx_lane_skew_min = tx_lane_skew_bits[0];
    tx_lane_skew_max = tx_lane_skew_bits[0];
    for (i=0; i<num_pcs_lanes; i++) {
        ln = tx_lane_swap[(start_lane % 4) + i];
        PKTGEN_GLASTEST_STS_SELr_CLR(  PKTGEN_GLASTEST_STS_SELr_reg);
        PKTGEN_GLASTEST_STS_SELr_GLASTEST_IDf_SET(PKTGEN_GLASTEST_STS_SELr_reg, 6+(ln%4));
        PHYMOD_IF_ERR_RETURN(MODIFY_PKTGEN_GLASTEST_STS_SELr(pc, PKTGEN_GLASTEST_STS_SELr_reg));
        PHYMOD_IF_ERR_RETURN(READ_PKTGEN_GLASTEST_ACTDATA0r(pc,&PKTGEN_GLASTEST_ACTDATA0r_reg));
        PHYMOD_IF_ERR_RETURN(READ_PKTGEN_GLASTEST_ACTDATA1r(pc,&PKTGEN_GLASTEST_ACTDATA1r_reg));
        PHYMOD_IF_ERR_RETURN(READ_PKTGEN_GLASTEST_ACTDATA2r(pc,&PKTGEN_GLASTEST_ACTDATA2r_reg));
        PHYMOD_IF_ERR_RETURN(READ_PKTGEN_GLASTEST_ACTADJr(pc,&PKTGEN_GLASTEST_ACTADJr_reg));
        dat_15_0  = PKTGEN_GLASTEST_ACTDATA0r_GLASTEST_DAT_15_0f_GET(PKTGEN_GLASTEST_ACTDATA0r_reg);
        /* dat_31_16 = PKTGEN_GLASTEST_ACTDATA1r_GLASTEST_DAT_31_16f_GET(PKTGEN_GLASTEST_ACTDATA1r_reg);
        dat_47_32 = PKTGEN_GLASTEST_ACTDATA2r_GLASTEST_DAT_47_32f_GET(PKTGEN_GLASTEST_ACTDATA2r_reg);
        dat_51_48 = PKTGEN_GLASTEST_ACTADJr_GLASTEST_DAT_51_48f_GET(PKTGEN_GLASTEST_ACTADJr_reg); */
        dat_adj   = PKTGEN_GLASTEST_ACTADJr_GLASTEST_ADJf_GET(PKTGEN_GLASTEST_ACTADJr_reg);
        tx_lane_skew_bits[i] = dat_15_0 + ((dat_adj * (fclk_period/ui_value)) + 0.5); /* 0.5 for rounding */
    }

    tx_min_skew_ln = 0;
    tx_max_skew_ln = 0;
    tx_lane_skew_min = tx_lane_skew_bits[0];
    tx_lane_skew_max = tx_lane_skew_bits[0];
    for(i=1; i<num_pcs_lanes; i++) {
        if(tx_lane_skew_bits[i] > tx_lane_skew_max) {
            tx_lane_skew_max = tx_lane_skew_bits[i];
            tx_max_skew_ln = i;
        }
        if(tx_lane_skew_bits[i] < tx_lane_skew_min) {
            tx_lane_skew_min = tx_lane_skew_bits[i];
            tx_min_skew_ln = i;
        }
    }

    if ((tx_lane_skew_max - tx_lane_skew_min) > 0x800) {
        for (i=0; i<num_pcs_lanes; i++) {
            if((tx_lane_skew_max-tx_lane_skew_bits[i]) > 0x800) tx_lane_skew_bits[i] = 0x1000 | tx_lane_skew_bits[i];
        }

        tx_min_skew_ln = 0;
        tx_max_skew_ln = 0;
        tx_lane_skew_min = tx_lane_skew_bits[0];
        tx_lane_skew_max = tx_lane_skew_bits[0];
        for(i=1; i<num_pcs_lanes; i++) {
            if(tx_lane_skew_bits[i] > tx_lane_skew_max) {
                tx_lane_skew_max = tx_lane_skew_bits[i];
                tx_max_skew_ln = i;
            }
            if(tx_lane_skew_bits[i] < tx_lane_skew_min) {
                tx_lane_skew_min = tx_lane_skew_bits[i];
                tx_min_skew_ln = i;
            }
        }
    }

    *tx_max_skew = ((uint32_t) (tx_lane_skew_max - tx_lane_skew_min) *ui_value*1024);
     if(timesync_am_norm_mode == phymodTimesyncCompensationModeLatestlane) {
         *tx_anchor_lane = start_lane + tx_max_skew_ln;
     } else {
         *tx_anchor_lane = start_lane + tx_min_skew_ln;
     }
    for (i=0; i<num_pcs_lanes; i++) {
        if(timesync_am_norm_mode == phymodTimesyncCompensationModeLatestlane) {
            tx_per_lane_skew[i] = ((uint32_t) (tx_lane_skew_bits[i]-tx_lane_skew_min) *ui_value*16);
        } else { /*Normalize to earliest */
            tx_per_lane_skew[i] = ((uint32_t) (tx_lane_skew_max - tx_lane_skew_bits[i]) *ui_value*16);
        }
    }

  return PHYMOD_E_NONE;
}
#endif

static int
_tbhmod_sub10g_tx_pmd_latency_get(PHYMOD_ST* pc, uint32_t *tx_latency)
{

  TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_t PMD_X4_PMD_TX_FIXED_LATENCYr_reg;

  TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_CLR(PMD_X4_PMD_TX_FIXED_LATENCYr_reg);

  PHYMOD_IF_ERR_RETURN(READ_TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr(pc,&PMD_X4_PMD_TX_FIXED_LATENCYr_reg));

  *tx_latency = TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_INT_NSf_GET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg) << 4;
  *tx_latency = *tx_latency | ((TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_FRAC_NSf_GET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg)>>2) & 0xf);

  return PHYMOD_E_NONE;
}

/*!
 *  @brief tdmod_set_tx_pmd_latency per port.
@param unit number for instance lane number for decide which lane
@param vco tells which vco is used in the lane
@param os_mode tells which os_mode is used in the lane
@returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
@details Enables ts in rx path */
int tbhmod_sub10g_tx_pmd_latency_update(PHYMOD_ST* pc, int speed_id, int num_pcs_lanes,
                                        phymod_timesync_compensation_mode_t timesync_am_norm_mode)
{
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_t PMD_X4_PMD_TX_FIXED_LATENCYr_reg;

    uint32_t tx_latency;
    uint32_t tx_per_lane_skew[4] = {0, 0, 0, 0};
    int tx_lane_swap[4] = {0, 0, 0, 0};
    int i, tx_anchor_lane;
    int start_lane = 0, num_lane = 0;
    phymod_access_t pa_copy;
    uint32_t tx_swap, tx_max_skew;
    int cnt=0 ,is_done = 0;

    TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_CLR(PMD_X4_PMD_TX_FIXED_LATENCYr_reg);

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_pcs_tx_lane_swap_get(pc, &tx_swap));
    for (i = 0; i < 4; i ++) {
        tx_lane_swap[i] = (tx_swap >> (i * 4)) & 0xf;
    }
    PHYMOD_IF_ERR_RETURN
        (_tbhmod_sub10g_tx_lane_skew_capture_set(pc, num_pcs_lanes, tx_lane_swap));
    PHYMOD_IF_ERR_RETURN
        (_tbhmod_sub10g_is_tx_lane_skew_capture_done(pc, &is_done));
    if (!is_done) {
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN
                (_tbhmod_sub10g_is_tx_lane_skew_capture_done(pc, &is_done));
            cnt = cnt + 1;
            if (is_done) {
                break;
            } else {
                if (cnt == 1000) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: tx lane skew capture is not done\n"));
                    break;
                }
            }
            PHYMOD_USLEEP(20);
        }
    }
    PHYMOD_IF_ERR_RETURN
        (_tbhmod_sub10g_tx_lane_skew_measure(pc, speed_id, num_pcs_lanes,
                                             timesync_am_norm_mode,
                                             tx_lane_swap,
                                             &tx_max_skew,
                                             tx_per_lane_skew,
                                             &tx_anchor_lane));

    for (i = 0; i < num_pcs_lanes; i++) {
        pa_copy.lane_mask = (0x1 << (i + start_lane));
        PHYMOD_IF_ERR_RETURN
            (_tbhmod_sub10g_tx_pmd_latency_get(&pa_copy, &tx_latency));

        tx_latency += tx_per_lane_skew[i];

        TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_INT_NSf_SET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg, (tx_latency >> 4));
        TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr_PMD_TX_LATENCY_FRAC_NSf_SET(PMD_X4_PMD_TX_FIXED_LATENCYr_reg, ((tx_latency & 0xf)<<2)); /* Frectional bit 6 [5:0] */
        PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_PMD_X4_PMD_TX_FIXED_LATENCYr(&pa_copy, PMD_X4_PMD_TX_FIXED_LATENCYr_reg));
    }
#endif

    return PHYMOD_E_NONE;
}



/* SET_1588_RX_DESKEW */
int tbhmod_sub10g_1588_set_rx_deskew(PHYMOD_ST* pc, int speed_id, phymod_timesync_compensation_mode_t timesync_am_norm_mode)
{
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    int i;
    uint32_t ui_value;
    /*uint32_t ui_value_sml;*/
    int num_pcs_lanes;
    int total_slip_cnt[4];
    int norm_deskew[4];
    int min_slip_cnt, max_slip_cnt;
    long deskew_val_int,deskew_val_frac;
    int start_lane = 0, num_lane = 0;
    phymod_access_t pa_copy;

    TSCE_RX_X4_R_DESKEW_INFO_ADETr_t reg_info_adet;
    TSCE_RX_X4_RX_DESKEW_INTr_t reg_rx_deskew_int;
    TSCE_RX_X4_RX_DESKEW_FRACr_t reg_rx_deskew_frac;
    /*TSCE_RX_X4_R_DESKEW_LL_TO_VL_MAPr_t reg_deskew_ll_to_vl;*/

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&pa_copy, pc, sizeof(pa_copy));

    if((speed_id==digital_operationSpeeds_SPEED_20G_KR2)||
       (speed_id==digital_operationSpeeds_SPEED_20G_CR2)) {
        num_pcs_lanes = 2;
    } else if((speed_id==digital_operationSpeeds_SPEED_40G_KR4)||
              (speed_id==digital_operationSpeeds_SPEED_40G_CR4)) {
        num_pcs_lanes = 4;
    } else {
        num_pcs_lanes = 1;
    }
    ui_value = _tbhmod_sub10g_pcs_ui_value_calc(speed_id, num_pcs_lanes);
    /*ui_value_sml = ui_value >> 2;*/ /* dropping last 2 frectional bits */

    for (i=0; i<num_pcs_lanes; i++) {
        pa_copy.lane_mask = (0x1 << (i + start_lane));
        TSCE_RX_X4_R_DESKEW_INFO_ADETr_CLR(reg_info_adet);
        PHYMOD_IF_ERR_RETURN(READ_TSCE_RX_X4_R_DESKEW_INFO_ADETr(&pa_copy, &reg_info_adet));
        total_slip_cnt[i] = (TSCE_RX_X4_R_DESKEW_INFO_ADETr_INFO_ADETf_GET(reg_info_adet) * 66); /* only cl82 speed and fec block size 66b */
        total_slip_cnt[i] -= TSCE_RX_X4_R_DESKEW_INFO_ADETr_INFO_SLIP_CNTf_GET(reg_info_adet);
        /* read logical lane to virtual lane mapping */
    /*
        TSCE_RX_X4_R_DESKEW_LL_TO_VL_MAPr_CLR(reg_deskew_ll_to_vl);
        PHYMOD_IF_ERR_RETURN(READ_TSCE_RX_X4_R_DESKEW_LL_TO_VL_MAPr(&pa_copy, &reg_deskew_ll_to_vl));
        ll_to_vl[i] = TSCE_RX_X4_R_DESKEW_LL_TO_VL_MAPr_LL2VLf_GET(reg_deskew_ll_to_vl);
     */
    }

    /* sort min and max */
    min_slip_cnt = total_slip_cnt[0];
    max_slip_cnt = total_slip_cnt[0];
    for (i=0; i<num_pcs_lanes; i++) {
        if(total_slip_cnt[i] < min_slip_cnt) {
            min_slip_cnt = total_slip_cnt[i];
        }
        if(total_slip_cnt[i] > max_slip_cnt) {
            max_slip_cnt = total_slip_cnt[i];
        }
    }

    /* convert to time */
    for (i=0; i<num_pcs_lanes; i++) {
        if (timesync_am_norm_mode == phymodTimesyncCompensationModeLatestlane) {
            norm_deskew[i] = (total_slip_cnt[i] - max_slip_cnt) * ui_value;
        } else {
            norm_deskew[i] = (total_slip_cnt[i] - min_slip_cnt) * ui_value;
        }
    }

    for(i=0; i<num_pcs_lanes; i++) {
        pa_copy.lane_mask = (0x1 << (i + start_lane));
        TSCE_RX_X4_RX_DESKEW_INTr_CLR(reg_rx_deskew_int);
        TSCE_RX_X4_RX_DESKEW_FRACr_CLR(reg_rx_deskew_frac);
        deskew_val_frac = (norm_deskew[i]>> 13) & 0x3ff;
        deskew_val_int  = (norm_deskew[i]>> 23) & 0xfff; /* deskew value (23-bit)={deskew_int(12-bits),deskew_frac(9-bits)} */
        TSCE_RX_X4_RX_DESKEW_FRACr_TAB_DS_FRAC_NSf_SET(reg_rx_deskew_frac,deskew_val_frac);
        PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_RX_X4_RX_DESKEW_FRACr(&pa_copy, reg_rx_deskew_frac));
        TSCE_RX_X4_RX_DESKEW_INTr_TAB_DS_INT_NSf_SET(reg_rx_deskew_int,deskew_val_int);
        PHYMOD_IF_ERR_RETURN(MODIFY_TSCE_RX_X4_RX_DESKEW_INTr(&pa_copy, reg_rx_deskew_int));
    }
#endif

    return PHYMOD_E_NONE;
}
