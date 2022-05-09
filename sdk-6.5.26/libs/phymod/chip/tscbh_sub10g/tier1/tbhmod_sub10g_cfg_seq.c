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
