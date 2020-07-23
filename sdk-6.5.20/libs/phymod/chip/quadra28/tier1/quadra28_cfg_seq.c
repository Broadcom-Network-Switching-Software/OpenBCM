/*
*
* 
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* File: quadra28.c
* Purpose: tier1 phymod microcode download support for Broadcom 40G Quadra28 
* note
*/
/*
 * Includes
 */
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include "quadra28_cfg_seq.h"
#include "quadra28_reg_access.h"
#include "bcmi_quadra28_defs.h"
#include "quadra28_serdes/common/srds_api_enum.h"
#include "quadra28_serdes/merlin_quadra28_functions.h"
static 
int _quadra28_merlin_phy_reset_get(phymod_access_t *pa,  phymod_phy_reset_t* reset);
static 
int _quadra28_merlin_phy_reset_set(phymod_access_t *pa, const phymod_phy_reset_t* reset);

static int quadra28_power_set(const phymod_access_t *pa, const phymod_phy_power_t* power);

static int quadra28_power_get(const phymod_access_t *pa, phymod_phy_power_t* power);

int quadra28_channel_select (const phymod_access_t *pa, uint16_t lane)
{
    BCMI_QUADRA28_CONFIG_ACCESSr_t  single_pmd_bcast_sel;
    ADDRESS_EXTENSIONr_t single_pmd_ch_sel;

    PHYMOD_MEMSET(&single_pmd_bcast_sel, 0 , sizeof(BCMI_QUADRA28_CONFIG_ACCESSr_t));
    PHYMOD_MEMSET(&single_pmd_ch_sel, 0 , sizeof(ADDRESS_EXTENSIONr_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_CONFIG_ACCESSr(pa, &single_pmd_bcast_sel));
    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_ADDRESS_EXTENSIONr(pa, &single_pmd_ch_sel));

    if (lane == Q28_ALL_LANE) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_ADDRESS_EXTENSIONr(pa, &single_pmd_ch_sel));
        BCMI_QUADRA28_ADDRESS_EXTENSIONr_AER_CHf_SET(single_pmd_ch_sel, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_ADDRESS_EXTENSIONr(pa, single_pmd_ch_sel));

        /* Enable Broadcast mode*/
        BCMI_QUADRA28_CONFIG_ACCESSr_BROADCAST_ENABLEf_SET(single_pmd_bcast_sel,1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_CONFIG_ACCESSr(pa, single_pmd_bcast_sel));
    } else {
        BCMI_QUADRA28_ADDRESS_EXTENSIONr_AER_CHf_SET(single_pmd_ch_sel, lane);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_ADDRESS_EXTENSIONr(pa, single_pmd_ch_sel));
    }
    return PHYMOD_E_NONE;
}

/**   read chip ID
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
uint32_t  _quadra28_get_chip_id(const phymod_access_t *pa)
{
    CHIP_ID0r_t chipid_lsb;
    CHIP_ID1r_t chipid_msb;
    int32_t chipid = 0;

    PHYMOD_MEMSET(&chipid_lsb, 0, sizeof(CHIP_ID0r_t));
    PHYMOD_MEMSET(&chipid_msb, 0, sizeof(CHIP_ID1r_t));

    PHYMOD_IF_ERR_RETURN(
            READ_CHIP_ID0r(pa, &chipid_lsb));
    PHYMOD_IF_ERR_RETURN(
            READ_CHIP_ID1r(pa, &chipid_msb));
    chipid = CHIP_ID0r_GET(chipid_lsb);
    chipid |= (CHIP_ID1r_GET(chipid_msb)) << 16;
    
    return chipid;
} 
/**   poll intf update 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
                
 *    @timeout                  timeout in us
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_intf_update_wait_check(const phymod_access_t *pa, uint32_t apps_mode0_reg_val,
                   int32_t timeout)          /* max wait time to check */
{
    uint32_t timer_counter = 0;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    timer_counter = (timeout / DELAY_1_MS_FROM_US);
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0, sizeof(GENERAL_PURPOSE_3r_t));
    do {
        /* Read C843 register & compare its value with apps_mod0_reg_val */
        PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(pa,&gen_pur_reg_3r_val));   
        if ((gen_pur_reg_3r_val.v[0] & 0xffff) == (apps_mode0_reg_val & 0xffff)) {
            return PHYMOD_E_NONE;
        }
        PHYMOD_USLEEP(DELAY_1_MS_FROM_US);
        timer_counter--;
    } while(timer_counter > 0);
    PHYMOD_DIAG_OUT(("Error Gp3:%x app:%x--:%x\n", gen_pur_reg_3r_val.v[0], apps_mode0_reg_val,pa->addr));
    return PHYMOD_E_FAIL;
} 

/**   Set config mode 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface specified by user 
 *    @param speed              Speed val as specified by user  
 *    @param ref_clk            Reference clock frequency to set 
 *                              the PHY into specified interface 
 *                              and speed
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_set_config_mode(const phymod_phy_access_t *phy,
                       phymod_interface_t intf,
                       uint32_t speed,
                       phymod_ref_clk_t ref_clk, uint32_t datapath,
                       uint16_t retimer)
{
    uint16_t single_pmd_mode = 0;
    uint32_t pkg_side = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl_reg;
    uint32_t phy_id = 0, lane_index = 0;
    int retry_cnt = 20;
    phymod_phy_power_t power;
    BCMI_QUADRA28_APPS_MODE_1r_t apps_mode1_reg;
    PHYMOD_MEMSET(&apps_mode1_reg, 0, sizeof(BCMI_QUADRA28_APPS_MODE_1r_t));
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&mdio_broadcast_ctrl_reg, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    phy_id = (phy->access.addr & 0x1f);
    QUADRA28_GET_LOC(phy, pkg_side);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_0r(&acc_cpy,&apps_mode0_reg_val));
    PHYMOD_IF_ERR_RETURN(_quadra28_phy_power_get(phy, &power));
    if (speed >= Q28_SPEED_40G) {
        for (lane_index = 0; lane_index < QUADRA28_MAX_LANE; lane_index ++) {
            acc_cpy.addr = phy->access.addr + lane_index;
            BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MCAST_ENf_SET(mdio_broadcast_ctrl_reg, 1);
            MDIO_BROADCAST_CONTROLr_MDIO_MULTICAST_PHYADf_SET(mdio_broadcast_ctrl_reg, phy_id);
            PHYMOD_IF_ERR_RETURN 
                   (WRITE_MDIO_BROADCAST_CONTROLr(&acc_cpy, mdio_broadcast_ctrl_reg));
        }
        acc_cpy.addr = phy->access.addr;
    }
    
    if ((speed <= Q28_SPEED_12P5G) ||
         BCMI_QUADRA28_APPS_MODE_0r_SPEEDf_GET(apps_mode0_reg_val) == 0x4 || 
         BCMI_QUADRA28_APPS_MODE_0r_SPEEDf_GET(apps_mode0_reg_val) == 0x7) {
         PHYMOD_IF_ERR_RETURN (
                quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }
    if (datapath != 0xffff) {
        if (datapath == phymodDatapathNormal) {
            datapath = 0;
        } else {
            datapath = 1;
        }
    }

    
    /* Configure port speed */
    PHYMOD_IF_ERR_RETURN(_quadra28_config_port_speed (speed, \
        (uint32_t*)&apps_mode0_reg_val.v[0], (uint16_t*)&single_pmd_mode));
    if(Q28_INTF_LINE == pkg_side) {
        /* Configure line side interface */
        PHYMOD_IF_ERR_RETURN(_quadra28_config_line_interface(&phy->access, (uint32_t*)&apps_mode0_reg_val.v[0], 
            intf, speed));
    } else {
        /* configure system side interface */
        PHYMOD_IF_ERR_RETURN(_quadra28_config_sys_interface(&phy->access, (uint32_t*)&apps_mode0_reg_val.v[0], 
            intf));  
    } 

    if (speed >= Q28_SPEED_40G) {
         PHYMOD_IF_ERR_RETURN (
           BCMI_QUADRA28_READ_APPS_MODE_1r(&acc_cpy, &apps_mode1_reg));
         BCMI_QUADRA28_APPS_MODE_1r_RETIMER_MODEf_SET(apps_mode1_reg, 0);
         PHYMOD_IF_ERR_RETURN (
             BCMI_QUADRA28_WRITE_APPS_MODE_1r(&acc_cpy, apps_mode1_reg));
    } else {
        if (retimer != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN (
              BCMI_QUADRA28_READ_APPS_MODE_1r(&acc_cpy, &apps_mode1_reg));
            BCMI_QUADRA28_APPS_MODE_1r_RETIMER_MODEf_SET(apps_mode1_reg, 
                    (retimer == phymodOperationModeRetimer) ? 1 : 0);
            PHYMOD_IF_ERR_RETURN (
              BCMI_QUADRA28_WRITE_APPS_MODE_1r(&acc_cpy, apps_mode1_reg));
        }
    }
    /* Resetting Datapath with the value got in core init*/
    if (datapath != 0xFFFF) {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, datapath);
    }
    PHYMOD_IF_ERR_RETURN(
                _quadra28_refclk_set(&acc_cpy, phymodRefClk156Mhz));

    BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
    PHYMOD_USLEEP(10000);
    /* Set bit 7 in C8D8 reg to update changes */
    BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 1);
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));

    PHYMOD_IF_ERR_RETURN(_quadra28_intf_update_wait_check(&acc_cpy,apps_mode0_reg_val.v[0], 50000));

    {
        BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
        BCMI_QUADRA28_CHIP_MODEr_t chip_mode;
        BCMI_QUADRA28_MERLIN_MISC_CTRLr_t reset_done;
        PHYMOD_MEMSET(&chip_mode, 0, sizeof(BCMI_QUADRA28_CHIP_MODEr_t));
        PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
        PHYMOD_MEMSET(&reset_done, 0, sizeof(BCMI_QUADRA28_MERLIN_MISC_CTRLr_t));

        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_CHIP_MODEr(&acc_cpy, &chip_mode));
        if (speed == Q28_SPEED_40G) {
            CHIP_MODEr_CHIP_MODEf_SET(chip_mode, 1);
        } else {
            CHIP_MODEr_CHIP_MODEf_SET(chip_mode, 0);
        }
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_CHIP_MODEr(&acc_cpy, chip_mode));

        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
        BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));
    }
    {
        BCMI_QUADRA28_MERLIN_MISC_CTRLr_t reset_ack;
        BCMI_QUADRA28_XGXSSTATUSr_t xgs_sts;
        BCMI_QUADRA28_GENERAL_PURPOSE_1r_t gpreg_1;
        PHYMOD_MEMSET(&gpreg_1, 0, sizeof(gpreg_1));
        do {
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_MERLIN_MISC_CTRLr(&acc_cpy, &reset_ack));
            PHYMOD_USLEEP(1000);
            retry_cnt --;
        } while (((reset_ack.v[0] & 0x8000) != 0x8000) && retry_cnt);
        if (retry_cnt <= 0) {
            return PHYMOD_E_TIMEOUT;
        }
        retry_cnt = 20;
        PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_GENERAL_PURPOSE_1r(&acc_cpy, &gpreg_1));
        if (BCMI_QUADRA28_GENERAL_PURPOSE_1r_TXON_PAD_STATUSf_GET(gpreg_1) != 0) {
            do {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_XGXSSTATUSr(&acc_cpy, &xgs_sts));
                PHYMOD_USLEEP(800);
                retry_cnt --;
            }  while (((xgs_sts.v[0] & 0x800) != 0x800) && retry_cnt);
            if (retry_cnt <= 0) {
                return PHYMOD_E_TIMEOUT;
            } 
        }
    } 
    apps_mode0_reg_val.v[0] &= 0xFF7F;
    PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));

    PHYMOD_IF_ERR_RETURN(_quadra28_intf_update_wait_check(&acc_cpy,apps_mode0_reg_val.v[0], 50000));
    PHYMOD_IF_ERR_RETURN(_quadra28_phy_power_set(phy, &power));
    return PHYMOD_E_NONE;

}
/**   Get config mode 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param phy                Pointer to phy structure 
 *    @param intf               Interface type 
 *    @param speed              Speed val retrieved from PHY 
 *    @param ref_clk            Reference clock 
 *    @param interface_modes    Supported interface modes
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes)
{
    uint16_t ref_clk_scaler_val = 0; 
    int pkg_side = 0;
    APPS_MODE_2r_t apps_mod2_reg_val;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    uint16_t config_speed  = 0;
    phymod_access_t acc_cpy;
   
    *speed = 0;
    *intf = 0;
    *ref_clk = 0; 

    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0, sizeof(GENERAL_PURPOSE_3r_t));
    PHYMOD_MEMSET(&apps_mod2_reg_val, 0 , sizeof(BCMI_QUADRA28_APPS_MODE_2r_t));

    QUADRA28_GET_IF_SIDE(acc_cpy.flags, pkg_side);
    if (acc_cpy.lane_mask == 0xf) {
        PHYMOD_IF_ERR_RETURN(
            quadra28_channel_select(&acc_cpy, 0));
    } else {
        /* Check the speed of 1st channel of this DIE*/
        PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(&acc_cpy, &gen_pur_reg_3r_val));
        /* Extract bits for speed */
        config_speed = (gen_pur_reg_3r_val.v[0] & 0xF);
        if (0x1 == config_speed){
            *speed = Q28_SPEED_1G;
        } else if (0x2 == config_speed) {
            *speed = Q28_SPEED_10G;
        } else if (0x8 == config_speed) {
            *speed = Q28_SPEED_11P5G;
        } else if (0xC  == config_speed) {
            *speed = Q28_SPEED_20G;
        } else if (0x4  == config_speed) {
            *speed = Q28_SPEED_40G;
        } else if (0x7  == config_speed) {
            *speed = Q28_SPEED_42G;
            *interface_modes = PHYMOD_INTF_MODES_HIGIG; 
        } else if (0x5  == config_speed) {
            *speed = Q28_SPEED_HG11;
            *interface_modes = PHYMOD_INTF_MODES_HIGIG; 
        }
        if (*speed <= Q28_SPEED_HG11) {
            /* If lane mask is nott 0xF, assuming it as 10G*/
            Q28_10G_PHY_ID_WORK_AROUND(10000, acc_cpy);
        }
    }
   
    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_2r(&acc_cpy, &apps_mod2_reg_val));
    ref_clk_scaler_val = APPS_MODE_2r_REFERENCE_CLOCKf_GET(apps_mod2_reg_val);
    
    PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(&acc_cpy, &gen_pur_reg_3r_val));

    /* Extract bits for speed */
    config_speed = (gen_pur_reg_3r_val.v[0] & 0xF);
    if (0x1 == config_speed){
        *speed = Q28_SPEED_1G;
    } else if (0x2 == config_speed) {
        *speed = Q28_SPEED_10G;
    } else if (0x8 == config_speed) {
        *speed = Q28_SPEED_11P5G;
    } else if (0xC  == config_speed) {
        *speed = Q28_SPEED_20G;
    } else if (0x4  == config_speed) {
        *speed = Q28_SPEED_40G;
    } else if (0x7  == config_speed) {
        *speed = Q28_SPEED_42G;
        *interface_modes = PHYMOD_INTF_MODES_HIGIG; 
    } else if (0x5  == config_speed) {
        *speed = Q28_SPEED_HG11;
        *interface_modes = PHYMOD_INTF_MODES_HIGIG; 
    }

    /* should use speed info & line side supported connections for Q28*/
    if (Q28_INTF_LINE == pkg_side) {
        if ((Q28_SPEED_42G == *speed || Q28_SPEED_40G == *speed)) {
            if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(14)) && 
              (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(9)))) {
                *intf = phymodInterfaceLR4;
            } else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(9)) && 
                    (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
                    (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
                *intf = phymodInterfaceCR4;
            } else if ((!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
                      (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
                *intf = phymodInterfaceSR4;
            } else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(8)) &&
                      (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
                *intf = phymodInterfaceKR4;
            } else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(14)) && 
                       (gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) {
                *intf = phymodInterfaceXLAUI;
            }
        } else if((Q28_SPEED_10G == *speed || Q28_SPEED_HG11 == *speed)) {
            if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(9)) &&
                (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
                (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
               *intf = phymodInterfaceCR;
            } else if ((!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
                       (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
               *intf = phymodInterfaceSR;
            }  else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(8)) &&
                       (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(14)))) {
               *intf = phymodInterfaceKR;
            } else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(14)) && 
                       (gen_pur_reg_3r_val.v[0] & Q28_BIT(12)) &&
                       (gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) {
               *intf = phymodInterfaceZR;
            } else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(4)) && 
                       (gen_pur_reg_3r_val.v[0] & Q28_BIT(5)) &&
                       (gen_pur_reg_3r_val.v[0] & Q28_BIT(14))) {
               *intf = phymodInterfaceLRM;
            } else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(14)) && 
                       (gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) {
               *intf = phymodInterfaceXFI;
            } else if ((!(gen_pur_reg_3r_val.v[0] & Q28_BIT(9))) && 
                      (gen_pur_reg_3r_val.v[0] & Q28_BIT(14))) {
               *intf = phymodInterfaceLR;
            }
        } else if (Q28_SPEED_1G == *speed) {
            if ((!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
                (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(10)))) {
#ifdef SA_PLP_SUPPORT
                *intf = phymodInterface1000X;
#else
                /*Since interface 1000X is not supported in phyctrl
                 * returning as SR*/
                *intf = phymodInterfaceSR;
#endif
            } else if (gen_pur_reg_3r_val.v[0] & Q28_BIT(8)) {
                *intf = phymodInterfaceKX;
            }
        }
    } else if (Q28_INTF_SYS == pkg_side) {
       /* If speed is 40G or 42G, system side connections: XLAUI, Forced CL72 */
        if (Q28_SPEED_40G == *speed || Q28_SPEED_42G == *speed) {
            if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(15)) &&
                 (gen_pur_reg_3r_val.v[0] & Q28_BIT(11))) {
                *intf = phymodInterfaceXLAUI;
            } else if ((gen_pur_reg_3r_val.v[0] & (Q28_BIT(11))) && 
                    !(gen_pur_reg_3r_val.v[0] & (Q28_BIT(15)))) {
                *intf = phymodInterfaceCR4;
            } else if ((gen_pur_reg_3r_val.v[0] & (Q28_BIT(10)))) {
                *intf = phymodInterfaceKR4;
            }
        } else if((Q28_SPEED_10G == *speed || Q28_SPEED_HG11 == *speed)) {
            if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(15)) &&
                 (gen_pur_reg_3r_val.v[0] & Q28_BIT(11))) {
                *intf = phymodInterfaceXFI;
            } else if ((gen_pur_reg_3r_val.v[0] & (Q28_BIT(10)))) {
                *intf = phymodInterfaceKR;
            }else if ((gen_pur_reg_3r_val.v[0] & Q28_BIT(11))){
                *intf = phymodInterfaceCR;
            }
        } else if(*speed == Q28_SPEED_1G) {
            if ((!(gen_pur_reg_3r_val.v[0] & Q28_BIT(8))) &&
                (!(gen_pur_reg_3r_val.v[0] & Q28_BIT(10)))) {
                /**intf = phymodInterface1000X;*/
                /*Since interface 1000X is not supported in phyctrl
                 * returning as SR*/
                *intf = phymodInterfaceSR;
            } else if (gen_pur_reg_3r_val.v[0] & Q28_BIT(8)) {
                *intf = phymodInterfaceKX;
            }
        }
        if (*intf == 0) {
            return PHYMOD_E_FAIL;
        }
    }
    
    if (phymodRefClk156Mhz == ref_clk_scaler_val) {
        *ref_clk = phymodRefClk156Mhz;
    } else {
        *ref_clk = phymodRefClk155Mhz; /* check this */
    }
   
    return PHYMOD_E_NONE;
}

/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param prbs_mode          User specified polynomial
 *                              0 - PRBS7
 *                              1 - PRBS9
 *                              2 - PRBS11    
 *                              3 - PRBS15
 *                              4 - PRBS23
 *                              5 - PRBS31
 *                              6 - PRBS58
 * 
 *    @param prbs_inv           User specified invert data config
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quara28_prbs_config_set(const phymod_phy_access_t *phy,
                        uint32_t flags,
                        uint32_t prbs_mode,
                        uint32_t prbs_inv)
{

    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE; /* num of lanes in octal package core */
    int lane_index = 0, pmd_mode = 0;
    unsigned short int lane_map = 0;
    uint32_t speed = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    BCMI_QUADRA28_PRBS_CHK_CONFIGr_t ckr_cfg;
    BCMI_QUADRA28_GENERAL_PURPOSE_3r_t gpreg3;
    phymod_access_t acc_cpy;
    
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&phy->access);
    QUADRA28_GET_LOC(phy,pkg_side);

    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMSET(&ckr_cfg, 0 , sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMSET(&gpreg3, 0 , sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_3r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    QUADRA28_SELECT_SIDE(&acc_cpy, side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                   quadra28_channel_select(&acc_cpy,
                      ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));

                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_config_tx_prbs(&acc_cpy, prbs_mode, (uint8_t)prbs_inv)); 
                }
                /* Following code block for PRBS Checker or RX*/
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_config_rx_prbs(&acc_cpy, prbs_mode, PRBS_INITIAL_SEED_HYSTERESIS, (uint8_t)prbs_inv)); 
                }
                if (Q28_ALL_LANE == lane_map){
                    break;
                }
            }
        }
    } else {
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
            PHYMOD_IF_ERR_RETURN(
                merlin_quadra28_config_tx_prbs(&acc_cpy, prbs_mode, (uint8_t)prbs_inv)); 
        }
        /* Following code block for PRBS Checker or RX*/
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
            PHYMOD_IF_ERR_RETURN(
                merlin_quadra28_config_rx_prbs(&acc_cpy, prbs_mode, PRBS_INITIAL_SEED_HYSTERESIS, (uint8_t)prbs_inv));
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(&acc_cpy, &gpreg3));
            if (gpreg3.v[0] & 0x20) {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_PRBS_CHK_CONFIGr(&acc_cpy, &ckr_cfg));
                BCMI_QUADRA28_PRBS_CHK_CONFIGr_PRBS_CHK_EN_AUTO_MODEf_SET(ckr_cfg,0);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_PRBS_CHK_CONFIGr(&acc_cpy, ckr_cfg));
            }

        }
    }
    
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val,Q28_INTF_LINE);	

    return PHYMOD_E_NONE;
}

/**   Get PRBS generator configuration 
 *    This function is used to retrieve PRBS generator configuration from 
 *    the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param prbs_mode          Configured Polynomial retrieved from chip 
 *    @param prbs_inv           Configured invert data retrieved from chip
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_config_get(const phymod_phy_access_t *phy,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum *prbs_mode,
                        uint32_t *prbs_inv)
{

    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    uint8_t prbs_invert = 0;
    uint32_t prbs_checker_mode = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;

    *prbs_inv = 0;
    *prbs_mode = 0; 
    
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy,pkg_side);
    
    QUADRA28_SELECT_SIDE(&acc_cpy, side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_get_tx_prbs_config(&acc_cpy, prbs_mode, &prbs_invert));
                    *prbs_inv = prbs_invert;
                }
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    /* Read Serdes API */
                    PHYMOD_IF_ERR_RETURN(
                            merlin_quadra28_get_rx_prbs_config(&acc_cpy, prbs_mode, &prbs_checker_mode, &prbs_invert));
                    *prbs_inv = prbs_invert;
                }
                break;
            }
        }
    } else {
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)){
            PHYMOD_IF_ERR_RETURN(
                    merlin_quadra28_get_tx_prbs_config(&acc_cpy, prbs_mode, &prbs_invert));
            *prbs_inv = prbs_invert;
        }
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
           /* Read Serdes API */
            PHYMOD_IF_ERR_RETURN(
                   merlin_quadra28_get_rx_prbs_config(&acc_cpy, prbs_mode, &prbs_checker_mode, &prbs_invert));
            *prbs_inv = prbs_invert;
        }
    }

    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val,Q28_INTF_LINE);	
    return PHYMOD_E_NONE;  
}

/**   Enable PRBS generator and PRBS checker 
 *    This function is used to enable or disable PRBS generator and PRBS checker
 *    as requested by the user  
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param enable              Enable or disable as specified by the user
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_enable_set(const phymod_phy_access_t *phy, uint32_t flags, uint32_t enable_state)
{
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE; /* num of lanes in octal package core */
    int lane_index = 0;
    int lane_map = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    SIDE_SELECTIONr_t side_sel_reg;
    BCMI_QUADRA28_PRBS_GEN_CONFIGr_t prbs_gen_config_reg_val;
    BCMI_QUADRA28_PRBS_CHK_CONFIGr_t prbs_chk_config_reg_val;
 
    PHYMOD_MEMSET(&prbs_gen_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_GEN_CONFIGr_t));
    PHYMOD_MEMSET(&prbs_chk_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMSET(&side_sel_reg, 0, sizeof(SIDE_SELECTIONr_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
   
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy,pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg, pkg_side);
    
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                   quadra28_channel_select(&acc_cpy,
                      ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));

                /* Select respective side */
                if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
                    PHYMOD_IF_ERR_RETURN(
                      merlin_quadra28_tx_prbs_en(&acc_cpy, enable_state));
                }
                /* Following code block for PRBS Checker or RX*/
                if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    PHYMOD_IF_ERR_RETURN(
                       merlin_quadra28_rx_prbs_en(&acc_cpy, enable_state));
                }
                if (lane_map == Q28_ALL_LANE) {
                    break;
                }
            }
        }
    } else {
       /* Select respective side */
        if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
            PHYMOD_IF_ERR_RETURN(
              merlin_quadra28_tx_prbs_en(&acc_cpy, enable_state));
        }
        /* Following code block for PRBS Checker or RX*/
        if((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
            PHYMOD_IF_ERR_RETURN(
               merlin_quadra28_rx_prbs_en(&acc_cpy, enable_state));
        }
    }
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg,Q28_INTF_LINE);		
    return PHYMOD_E_NONE;
}

/**   Get Enable status of PRBS generator and PRBS checker 
 *    This function is used to retrieve the enable status of PRBS generator and
 *    PRBS checker
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param enable              Enable or disable read from chip 
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_enable_get(const phymod_phy_access_t *phy, uint32_t flags, uint32_t *enable)
{
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE; /* num of lanes in octal package core */
    int lane_index = 0;
    int lane_map = 0;
    SIDE_SELECTIONr_t side_sel_reg;
    BCMI_QUADRA28_PRBS_GEN_CONFIGr_t prbs_gen_config_reg_val;
    BCMI_QUADRA28_PRBS_CHK_CONFIGr_t prbs_chk_config_reg_val;
    uint8_t gen_en = 0;
    uint8_t chk_en = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    
    *enable = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&phy->access);
    QUADRA28_GET_LOC(phy,pkg_side);
    
    PHYMOD_MEMSET(&prbs_gen_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_GEN_CONFIGr_t));
    PHYMOD_MEMSET(&prbs_chk_config_reg_val, 0, sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMSET(&side_sel_reg, 0, sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_tx_prbs_en(&acc_cpy, &gen_en));
                    *enable = gen_en;
                }
                if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_rx_prbs_en(&acc_cpy, &chk_en));
                    *enable = chk_en;
                }
                break;				
            }
         }
    } else {
        if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_TX)) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_tx_prbs_en(&acc_cpy, &gen_en));
            *enable = gen_en;
        }
        if ((flags == 0) || (flags == PHYMOD_PRBS_DIRECTION_RX)) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_get_rx_prbs_en(&acc_cpy, &chk_en));
            *enable = chk_en;
        }
    }
    if (flags == 0) {
        if(gen_en && chk_en) {
            *enable = 1;
        } else {
            *enable = 0;
        }
    }
     /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg,Q28_INTF_LINE);	
    return PHYMOD_E_NONE;
}


/**   Get PRBS lock and error status 
 *    This function is used to retrieve PRBS lock, loss of lock and error counts
 *    from the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param lock_status        PRBS lock status denotes PRBS generator and 
 *                              checker are locked to same polynomial data
 *    @param lock_loss          Lo of lock denotes PRBS generator and checker
 *                              are not in sync   
 *    @param error_count        PRBS error count retrieved from chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_status_get(const phymod_phy_access_t *phy,
                                uint32_t *lock_status,
                                uint32_t *lock_loss,
                                uint32_t *error_count)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0; 
    uint8_t loss_of_lock = 0;
    uint32_t per_lane_err_count = 0;
    uint8_t prbs_chk_lock_stat;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;

    *lock_status = 1;
    *lock_loss = 1;
    *error_count = 0;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy,&phy->access, sizeof(phymod_access_t));

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(&phy->access);
    QUADRA28_GET_LOC(phy, pkg_side);
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    
    /* Select respective side */
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                   
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_chk_lock_state(&acc_cpy, (uint8_t*)&prbs_chk_lock_stat));
 
                *lock_status &= prbs_chk_lock_stat;
                
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_err_count_state(&acc_cpy, &per_lane_err_count, &loss_of_lock));
                *error_count |= per_lane_err_count;
                *lock_loss &= loss_of_lock;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_chk_lock_state(&acc_cpy, (uint8_t*)&prbs_chk_lock_stat));
        *lock_status &= prbs_chk_lock_stat;
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_prbs_err_count_state(&acc_cpy, &per_lane_err_count, &loss_of_lock));
        *error_count |= per_lane_err_count;
        *lock_loss &= loss_of_lock;
    }
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val,Q28_INTF_LINE);	
    return PHYMOD_E_NONE;
}								
/**   Get  link status of PHY 
 *    This function is used to retrieve the link status of PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param link_status        link status of the PHY 
 *                              1 - Up 
 *                              0 - Down 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_link_status(const phymod_phy_access_t *phy, uint32_t *link_status)
{
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_map = 0;
    uint32_t retime_enable = 0, pcs_status = 0; 
    int lane_index = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    BCMI_QUADRA28_PMD_STATUSr_t ieee_pmd_status1_reg_val;
    BCMI_QUADRA28_PCS_STATUS1r_t pcs_status1_reg_val;	
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;

 
    PHYMOD_MEMSET(&side_sel_reg_val, 0, sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMSET(&ieee_pmd_status1_reg_val, 0, sizeof(BCMI_QUADRA28_PMD_STATUSr_t));
    PHYMOD_MEMSET(&pcs_status1_reg_val, 0, sizeof(BCMI_QUADRA28_PCS_STATUS1r_t)); 
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    QUADRA28_GET_LOC(phy, pkg_side);
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);

    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,pkg_side);
    *link_status = 1;

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                /* select a lane */
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                
                /* Get PMD status */
                PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PMD_STATUSr(&acc_cpy, &ieee_pmd_status1_reg_val));
                *link_status &= BCMI_QUADRA28_PMD_STATUSr_LN_LKDTCDRf_GET(ieee_pmd_status1_reg_val);
             }
         }
    } else {
       /* Get PCS status if PCS is ON */
       PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_retimer_enable_get(&acc_cpy, &retime_enable));
       if (1 == retime_enable) {
           if (speed == 1000) {
              PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_read(&acc_cpy, 0x7ffe1, &pcs_status));
              *link_status &= ((pcs_status & 0x4) >> 2);
           } else {
               PHYMOD_IF_ERR_RETURN(
                       BCMI_QUADRA28_READ_PCS_STATUS1r(&acc_cpy, &pcs_status1_reg_val));
               *link_status &= BCMI_QUADRA28_PCS_STATUS1r_PCS_RECEIVE_LINK_STATUSf_GET(pcs_status1_reg_val);
           }
       }
       /* Get PMD status */
       PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PMD_STATUSr(&acc_cpy, &ieee_pmd_status1_reg_val));
       *link_status &= BCMI_QUADRA28_PMD_STATUSr_LN_LKDTCDRf_GET(ieee_pmd_status1_reg_val);
    }
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val,Q28_INTF_LINE);
    return PHYMOD_E_NONE;
    
}

int _quadra28_refclk_set(const phymod_access_t *pa, uint32_t ref_clk) 
{
    BCMI_QUADRA28_APPS_MODE_2r_t apps_mod2_reg_val;
       
    PHYMOD_MEMSET(&apps_mod2_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_2r_t));
    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_APPS_MODE_2r(pa, &apps_mod2_reg_val));
    if (phymodRefClk156Mhz == ref_clk) {
        BCMI_QUADRA28_APPS_MODE_2r_REFERENCE_CLOCKf_SET(apps_mod2_reg_val,0);
    } else {
        BCMI_QUADRA28_APPS_MODE_2r_REFERENCE_CLOCKf_SET(apps_mod2_reg_val,1);
    }

    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_APPS_MODE_2r(pa, apps_mod2_reg_val));
   
    return PHYMOD_E_NONE;
}

/**   Set Tx Rx polarity 
 *    This function is used to set Tx Rx polarity of a single lane
 *    or multiple lanes 
 *
 *    @param phy                 Pointer to phymod access structure 
 *    @param tx_polarity        Tx polarity 
 *    @param rx_polarity        Rx polarity 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_tx_rx_polarity_set (const phymod_phy_access_t *phy, uint32_t tx_polarity, uint32_t rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    BCMI_QUADRA28_TX_CTRL_0r_t tx_ctrl0r_config;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_t  tx_misc;
    BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_t   rx_misc;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    uint8_t datapath = 0;
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&tx_ctrl0r_config, 0 , sizeof(BCMI_QUADRA28_TX_CTRL_0r_t));
    PHYMOD_MEMSET(&tx_misc, 0 , sizeof(BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_t));
    PHYMOD_MEMSET(&rx_misc, 0 , sizeof(BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
  
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,pkg_side);
    Q28_GET_DATAPATH(&acc_cpy, datapath);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                   quadra28_channel_select(&acc_cpy, lane_index));
                if (datapath == phymodDatapathUll) {
                    if (tx_polarity == 0xFFFF || tx_polarity == 0xFFFFFFFF) {
                        continue;
                    }
                    /* Read 4bit tx config register for both line & sys side */
                    PHYMOD_IF_ERR_RETURN
                        (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                    /* Program 4-bit Line Side Tx Polarity Flip */
                    BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_SET(tx_ctrl0r_config, 
                            ((tx_polarity >> lane_index) & 1));
                    /* Write to Rx config register - 4-bit */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_WRITE_TX_CTRL_0r(&acc_cpy, tx_ctrl0r_config));
                } else {
                    if (tx_polarity == 0xFFFF || tx_polarity == 0xFFFFFFFF) {
                        continue;
                    }
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_READ_TLB_TX_MISC_CONFIGr(&acc_cpy, &tx_misc));
                    BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_TX_PMD_DP_INVERTf_SET(tx_misc, 
                                  ((tx_polarity >> lane_index) & 1));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_WRITE_TLB_TX_MISC_CONFIGr(&acc_cpy, tx_misc));
                    if (rx_polarity == 0xFFFF || rx_polarity == 0xFFFFFFFF) {
                        continue;
                    }
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_READ_TLB_RX_MISC_CONFIGr(&acc_cpy, &rx_misc));
                    BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_RX_PMD_DP_INVERTf_SET(rx_misc, 
                                   ((rx_polarity >> lane_index) & 1));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_WRITE_TLB_RX_MISC_CONFIGr(&acc_cpy, rx_misc));
                }
            }
        } /* End for */
    } else {
         
        if (datapath == phymodDatapathUll) {
            if ((tx_polarity != 0xFFFF) && (tx_polarity != 0xFFFFFFFF)) {
                /* Read 4bit tx config register for both line & sys side */
                PHYMOD_IF_ERR_RETURN
                    (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                                   
                /* Program 4-bit Line Side Tx Polarity Flip */
                BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_SET(tx_ctrl0r_config, tx_polarity);
                
                /* Write to Rx config register - 4-bit */
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_TX_CTRL_0r(&acc_cpy, tx_ctrl0r_config));
            }
        } else {
            if ((tx_polarity != 0xFFFF) && (tx_polarity != 0xFFFFFFFF)) {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_TLB_TX_MISC_CONFIGr(&acc_cpy, &tx_misc));
                BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_TX_PMD_DP_INVERTf_SET(tx_misc, tx_polarity);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_TLB_TX_MISC_CONFIGr(&acc_cpy, tx_misc));
            }
            if ((rx_polarity != 0xFFFF) && (rx_polarity != 0xFFFFFFFF)) {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_TLB_RX_MISC_CONFIGr(&acc_cpy, &rx_misc));
                BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_RX_PMD_DP_INVERTf_SET(rx_misc, rx_polarity); 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_TLB_RX_MISC_CONFIGr(&acc_cpy, rx_misc));
            }
        }
    }
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val,Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}
/**   Get Tx Rx polarity 
 *    This function is used to get Tx Rx polarity of a specific lane
 *    specified by user 
 *
 *    @param phy                 Pointer to phymod access structure 
 *    @param tx_polarity        Tx polarity 
 *    @param rx_polarity        Rx polarity 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_tx_rx_polarity_get (const phymod_phy_access_t *phy, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = 0;
    int lane_index = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_t  tx_misc;
    BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_t   rx_misc;
    uint8_t datapath = 0;
    BCMI_QUADRA28_TX_CTRL_0r_t tx_ctrl0r_config;

    PHYMOD_MEMSET(&tx_misc, 0 , sizeof(BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_t));
    PHYMOD_MEMSET(&rx_misc, 0 , sizeof(BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_t));
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&tx_ctrl0r_config, 0 , sizeof(BCMI_QUADRA28_TX_CTRL_0r_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);
    
    Q28_GET_DATAPATH(&acc_cpy, datapath);
    num_lanes = QUADRA28_MAX_LANE; /* for octal pkg */

    *tx_polarity = 0;
    *rx_polarity = 0;
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                if (datapath == phymodDatapathUll) {
                    PHYMOD_IF_ERR_RETURN
                      (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
                    *tx_polarity |= (BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_GET(tx_ctrl0r_config)) << lane_index;
                } else {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_READ_TLB_TX_MISC_CONFIGr(&acc_cpy, &tx_misc));
                    *tx_polarity |= BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_TX_PMD_DP_INVERTf_GET(tx_misc) << lane_index;
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_READ_TLB_RX_MISC_CONFIGr(&acc_cpy, &rx_misc));
                    *rx_polarity |= BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_RX_PMD_DP_INVERTf_GET(rx_misc) << lane_index; 
                }
            }
        }
    } else {
        if (datapath == phymodDatapathUll) {
            PHYMOD_IF_ERR_RETURN
              (BCMI_QUADRA28_READ_TX_CTRL_0r(&acc_cpy, &tx_ctrl0r_config));
            *tx_polarity = BCMI_QUADRA28_TX_CTRL_0r_LL_POLARITY_FLIPf_GET(tx_ctrl0r_config);
        } else {
            PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_READ_TLB_TX_MISC_CONFIGr(&acc_cpy, &tx_misc));
            *tx_polarity = BCMI_QUADRA28_TLB_TX_MISC_CONFIGr_TX_PMD_DP_INVERTf_GET(tx_misc);
            PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_READ_TLB_RX_MISC_CONFIGr(&acc_cpy, &rx_misc));
            *rx_polarity = BCMI_QUADRA28_TLB_RX_MISC_CONFIGr_RX_PMD_DP_INVERTf_GET(rx_misc); 
        }

    }
    
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_pmd_lock_get(const phymod_phy_access_t *phy, uint32_t *rx_seq_done) 
{
    uint8_t rx_pmd_lock = 0;
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
 
    /* Initializing with invalid value*/
    if(NULL != rx_seq_done) {
        *rx_seq_done = 0xFFFF;
    } else {
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(&mdio_broadcast_ctrl, 0 , sizeof(MDIO_BROADCAST_CONTROLr_t));	
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
 
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN(
                    merlin_quadra28_pmd_lock_status(&acc_cpy, &rx_pmd_lock));
                *rx_seq_done &= rx_pmd_lock;
            }
        }
    } else {
        if(NULL != rx_seq_done) {
            PHYMOD_IF_ERR_RETURN(
                merlin_quadra28_pmd_lock_status(&acc_cpy, &rx_pmd_lock));
            *rx_seq_done &= rx_pmd_lock;
        }
    }
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

/**   Set line side interface  
 *    This function is used to set the operating mode of the PHY
 *
 *    @param reg_val            Pointer to apps mode reg 
 *    @param intf               Interface specified by user 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_config_line_interface(const phymod_access_t *pa, uint32_t* reg_val, phymod_interface_t intf, uint32_t speed)
{
    int return_val = PHYMOD_E_NONE;
    uint32_t mask = 0, enable = 0;
    if (speed == Q28_SPEED_10G || speed == Q28_SPEED_HG11) {
        if ((intf != phymodInterfaceSR) && (intf != phymodInterfaceCR) &&
                (intf != phymodInterfaceLR) && (intf != phymodInterfaceKR) &&
                (intf != phymodInterfaceXFI) &&(intf != phymodInterfaceZR) &&
                (intf != phymodInterfaceLRM)) {
            PHYMOD_DIAG_OUT(("Interface %d not supported, so setting default interface \n", intf));
            intf = phymodInterfaceSR;
        }
    } else if(speed == Q28_SPEED_40G || speed == Q28_SPEED_42G) {
        if ((intf != phymodInterfaceSR4) && (intf != phymodInterfaceCR4) &&
                (intf != phymodInterfaceLR4) && (intf != phymodInterfaceKR4) &&
                (intf != phymodInterfaceXLAUI)) {
            PHYMOD_DIAG_OUT(("Interface %d not supported, so setting default interface \n", intf));
            intf = phymodInterfaceSR4;
        }
    } else if (speed == Q28_SPEED_1G) {
        if ((intf != phymodInterface1000X) && (intf != phymodInterfaceKX)
                && (intf != phymodInterfaceSR)) {
            PHYMOD_DIAG_OUT(("Interface %d not supported, so setting default interface \n", intf));
            intf = phymodInterface1000X;
        }
    }
    /* Need this, If Configured as ZR previously*/
    if ((*reg_val & 0x5A00 ) == 0x5A00) {
        *reg_val |= (Q28_BIT(15));
    }
    switch (intf)
    {
        case phymodInterfaceSR:  
        case phymodInterfaceSR4:
            mask = ~(Q28_BIT(4) | Q28_BIT(5)| Q28_BIT(8) | 
                    Q28_BIT(14) | Q28_BIT(9) | Q28_BIT(12));
            *reg_val &= mask;
            break;
        case phymodInterfaceKX:
            mask = ~ (Q28_BIT(4) | Q28_BIT(5)|Q28_BIT(8) | 
                    Q28_BIT(14) | Q28_BIT(9) | Q28_BIT(15) | 
                    Q28_BIT(11));
            *reg_val &= mask;
             PHYMOD_IF_ERR_RETURN(
                     _quadra28_phy_retimer_enable_get(pa, &enable));
             /* Setting both line and sys*/
             if (enable) {
                *reg_val |= 0x500;
             } else {
                 /* Chip limitation: AN Will not work properly when its is enabled on both line and 
                  * system side in repeater mode*/
                *reg_val |= 0x100;
             }
            break;
        case phymodInterfaceKR:
        case phymodInterfaceKR4:
            mask = ~(Q28_BIT(4) | Q28_BIT(5)|Q28_BIT(8) | 
                    Q28_BIT(14) | Q28_BIT(9)| Q28_BIT(12));
            *reg_val &= mask;
            *reg_val |= 0x100;            
            break;
        case phymodInterfaceCR:
        case phymodInterfaceCX:
        case phymodInterfaceCR4:
            mask = ~(Q28_BIT(4) | Q28_BIT(5)|Q28_BIT(8) | 
                    Q28_BIT(14) | Q28_BIT(9)| Q28_BIT(12));
            *reg_val &= mask;
            *reg_val|= 0x200;
            break;
       
        case phymodInterfaceLR:       
        case phymodInterfaceLR4:
            mask = ~(Q28_BIT(4) | Q28_BIT(5) | Q28_BIT(8) 
                    | Q28_BIT(14) | Q28_BIT(9)| Q28_BIT(12));
            *reg_val &= mask;
            *reg_val |= 0x4000;
            break;
        case phymodInterface1000X:
            mask = 0xF;
            *reg_val &= mask;
            break;
        case phymodInterfaceXLAUI:
        case phymodInterfaceXFI:
            mask = ~(Q28_BIT(4) | Q28_BIT(5) | Q28_BIT(8) | 
                    Q28_BIT(14) | Q28_BIT(9)| Q28_BIT(12));
            *reg_val &= mask;
            *reg_val |= 0x4200;
            break;
        case phymodInterfaceZR:
            mask = ~(Q28_BIT(4) | Q28_BIT(5) | Q28_BIT(8) | 
                    Q28_BIT(14) | Q28_BIT(9) | Q28_BIT(15));
            *reg_val &= mask;
            *reg_val |= 0x5A00;
            break;
        case phymodInterfaceLRM:
            *reg_val &= 0xF;
            *reg_val |= 0xC830;
            break;

        default:
            return_val = PHYMOD_E_FAIL;
            break;
    } 
    return return_val;
}

/**   Set sys side interface  
 *    This function is used to set the operating mode of the PHY
 *
 *    @param reg_val            Pointer to apps mode reg 
 *    @param intf               Interface specified by user 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_config_sys_interface(const phymod_access_t *pa, uint32_t* reg_val, phymod_interface_t intf)
{
    uint32_t mask = 0, enable = 0;
    /* configure the System side port mode - interface type */
    switch (intf)
    {
        case phymodInterfaceXLAUI:
        case phymodInterfaceXFI:
            mask = ~(Q28_BIT(11) | Q28_BIT(15) | Q28_BIT(10));
            *reg_val &= mask;
            *reg_val |= 0x8800;
            break;
        case phymodInterface1000X:
            mask = ~(Q28_BIT(11) | Q28_BIT(15) | Q28_BIT(10));
            *reg_val &= mask;
            break;
        case phymodInterfaceKX:
            mask = ~(Q28_BIT(11) | Q28_BIT(15) | Q28_BIT(10));
            *reg_val &= mask;
            PHYMOD_IF_ERR_RETURN(
                     _quadra28_phy_retimer_enable_get(pa, &enable));
             /* Setting both line and sys*/
            if (enable) {
                *reg_val |= 0x500;
            } else {
                /* Chip limitation: AN Will not work properly when its is enabled on both line and 
                 * system side in repeater mode*/
                *reg_val |= 0x100;
            }
            break;
        case phymodInterfaceCR4:
        case phymodInterfaceCX4:
        case phymodInterfaceCR:
            mask = ~(Q28_BIT(11) | Q28_BIT(15) | Q28_BIT(10));
            *reg_val &= mask;
            *reg_val |= 0x800;
            break;
        case phymodInterfaceKR4:
        case phymodInterfaceKR:
            mask = ~(Q28_BIT(11) | Q28_BIT(15) | Q28_BIT(10));
            *reg_val &= mask;
            *reg_val |= 0x400;

            break;
        default:
            return PHYMOD_E_FAIL;            
    } 
    return PHYMOD_E_NONE;
}

/**   Set port speed  
 *    This function is used to set the operating mode of the PHY
 *
 *    @param reg_val            Pointer to apps mode reg 
 *    @param speed              Interface specified by user 
 *    @param mode_type          port mode (single vs multiple)
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_config_port_speed(uint32_t speed, uint32_t* reg_val, uint16_t* mode_type)
{
    int return_val = PHYMOD_E_NONE; 
    *reg_val &= ~(0xF);
    *mode_type = 0;
    switch (speed)
    {
        case SPEED_1G_OS8P25:
            *reg_val |= 0x1;
            break;

        case Q28_SPEED_10G:
            *reg_val |= 0x2;
            break;

        case Q28_SPEED_1G:
            *reg_val |= 0x1;
            break;

        case Q28_SPEED_11P5G:
            *reg_val |= 0x8;
            break;

        case Q28_SPEED_20G: /* KR2 2 * 10.3125G */
            *reg_val |= 0xC;
            break;

        case Q28_SPEED_40G:
            *reg_val |= 0x4;
            *mode_type = 1;
            break;

        case Q28_SPEED_42G:
            *reg_val |= 0x7;
            *mode_type = 1;
            break;
        case Q28_SPEED_HG11:
            *reg_val |= 0x5;
            break;
           
        default:
            return_val = PHYMOD_E_PARAM;
            break;

    }/* End: Switch for speed */
    return return_val;
}

int quadra28_tx_lane_control_set(const phymod_phy_access_t *phy,
                              phymod_phy_tx_lane_control_t tx_control)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    uint16_t err_code = 0;
    PMD_TRANSMIT_DISABLEr_t tx_disable;
#ifdef SA_PLP_SUPPORT
	BCMI_QUADRA28_MISC_CONTROL1r_t misc_ctrl;
#endif
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    phymod_access_t *pa=&acc_cpy; 
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&tx_disable, 0 , sizeof(PMD_TRANSMIT_DISABLEr_t));
#ifdef SA_PLP_SUPPORT
	PHYMOD_MEMSET(&misc_ctrl, 0 , sizeof(BCMI_QUADRA28_MISC_CONTROL1r_t));
#endif
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);

    /* Get the lane map from phymod access */
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy, side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                   quadra28_channel_select(&acc_cpy,
                      ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                switch(tx_control) {
                    case phymodTxSquelchOn:
                       PHYMOD_IF_ERR_RETURN(
                           READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
                       PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET
                                                        (tx_disable, 1);
                       PHYMOD_IF_ERR_RETURN(
                           WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_disable));
#ifdef SA_PLP_SUPPORT
                       PHYMOD_IF_ERR_RETURN(
                           BCMI_QUADRA28_READ_MISC_CONTROL1r(&acc_cpy,&misc_ctrl));
                       BCMI_QUADRA28_MISC_CONTROL1r_PMD_TX_DISABLE_PIN_DISf_SET(misc_ctrl,0);
                       PHYMOD_IF_ERR_RETURN(
                           BCMI_QUADRA28_WRITE_MISC_CONTROL1r(&acc_cpy,misc_ctrl));
#endif
                       break;
                    case phymodTxSquelchOff:
                       PHYMOD_IF_ERR_RETURN(
                           READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
                       PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET
                                                        (tx_disable, 0);
                       PHYMOD_IF_ERR_RETURN(
                           WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_disable));
#ifdef SA_PLP_SUPPORT
                       PHYMOD_IF_ERR_RETURN(
                           BCMI_QUADRA28_READ_MISC_CONTROL1r(&acc_cpy,&misc_ctrl));
                       BCMI_QUADRA28_MISC_CONTROL1r_PMD_TX_DISABLE_PIN_DISf_SET(misc_ctrl,1);
                       PHYMOD_IF_ERR_RETURN(
                           BCMI_QUADRA28_WRITE_MISC_CONTROL1r(&acc_cpy,misc_ctrl));
#endif
                       break;
                    case phymodTxReset:
                       err_code = wr_ln_dp_s_rstb(0);
                       if (err_code != ERR_CODE_NONE){
                           return err_code;
                       }
                       break;
                    default:
                       return PHYMOD_E_UNAVAIL;
                }
                if (Q28_ALL_LANE == lane_map) {
                    break;
                }
            }
        }
    } else {
        switch(tx_control) {
            case phymodTxSquelchOn:
                PHYMOD_IF_ERR_RETURN(
                    READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
                PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET
                                                 (tx_disable, 1);
                PHYMOD_IF_ERR_RETURN(
                    WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_disable));
#ifdef SA_PLP_SUPPORT
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_MISC_CONTROL1r(&acc_cpy,&misc_ctrl));
                BCMI_QUADRA28_MISC_CONTROL1r_PMD_TX_DISABLE_PIN_DISf_SET(misc_ctrl,0);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_MISC_CONTROL1r(&acc_cpy,misc_ctrl));
#endif
               break;
            case phymodTxSquelchOff:
                PHYMOD_IF_ERR_RETURN(
                    READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
                PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET
                                                 (tx_disable, 0);
                PHYMOD_IF_ERR_RETURN(
                    WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_disable));
#ifdef SA_PLP_SUPPORT
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_MISC_CONTROL1r(&acc_cpy,&misc_ctrl));
               BCMI_QUADRA28_MISC_CONTROL1r_PMD_TX_DISABLE_PIN_DISf_SET(misc_ctrl,1);
               PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_MISC_CONTROL1r(&acc_cpy,misc_ctrl));
#endif
                break;
            case phymodTxReset:
                err_code = wr_ln_dp_s_rstb(0);
                if (err_code != ERR_CODE_NONE){
                    return err_code;
                }
                break;
            default:
               return PHYMOD_E_UNAVAIL;
        }
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy, side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;    
}

int quadra28_tx_squelch_get(const phymod_phy_access_t *phy,
                               int *tx_squelch)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    PMD_TRANSMIT_DISABLEr_t tx_disable;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&tx_disable, 0 , sizeof(PMD_TRANSMIT_DISABLEr_t));	
    PHYMOD_MEMCPY(&acc_cpy,&phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN(
                    READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
                *tx_squelch = 
                    PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_GET(tx_disable);
                break;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(
            READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
        *tx_squelch = 
            PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_GET(tx_disable);
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}

int quadra28_rx_lane_control_set(const phymod_phy_access_t *phy,
                              phymod_phy_rx_lane_control_t rx_control)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    uint16_t err_code = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    SIGDET_CTRL_1r_t sigdet_ctrl1_reg;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    phymod_access_t *pa=&acc_cpy;   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&sigdet_ctrl1_reg, 0 , sizeof(SIGDET_CTRL_1r_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        switch(rx_control) {
            case phymodRxSquelchOff:
            case phymodRxSquelchOn: 
                for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                    if (((lane_map >> lane_index) & 1) == 0x1) {
                        PHYMOD_IF_ERR_RETURN(
                            quadra28_channel_select(&acc_cpy,
                            ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                        PHYMOD_IF_ERR_RETURN(                
                            READ_SIGDET_CTRL_1r(&acc_cpy, &sigdet_ctrl1_reg));
                        SIGDET_CTRL_1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctrl1_reg, 
                                   ((rx_control == phymodRxSquelchOn) ? 0 : 1));
                        SIGDET_CTRL_1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctrl1_reg, 1);
                        PHYMOD_IF_ERR_RETURN(  
                             WRITE_SIGDET_CTRL_1r(&acc_cpy, sigdet_ctrl1_reg));
                        /* If there is no RX squelch change the force to 0
                         * i.e. get the RX status from Analog*/
                        if (rx_control == phymodRxSquelchOff) {
                            SIGDET_CTRL_1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctrl1_reg, 0);
                            PHYMOD_IF_ERR_RETURN(  
                             WRITE_SIGDET_CTRL_1r(&acc_cpy, sigdet_ctrl1_reg));

                        }
                        if (Q28_ALL_LANE == lane_map) {
                             break;
                        }
                    }
                }
            break;
            case phymodRxReset:
                err_code = wr_ln_dp_s_rstb(0);
                if (err_code != ERR_CODE_NONE){
                    return err_code;
                }
            break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else {
        switch(rx_control) {
            case phymodRxSquelchOn: 
            case phymodRxSquelchOff:
                PHYMOD_IF_ERR_RETURN(                
                    READ_SIGDET_CTRL_1r(&acc_cpy, &sigdet_ctrl1_reg));
                SIGDET_CTRL_1r_SIGNAL_DETECT_FRC_VALf_SET(sigdet_ctrl1_reg, 
                        ((rx_control == phymodRxSquelchOn) ? 0 : 1));
                SIGDET_CTRL_1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctrl1_reg, 1);
                PHYMOD_IF_ERR_RETURN(  
                    WRITE_SIGDET_CTRL_1r(&acc_cpy, sigdet_ctrl1_reg));
                /* If there is no RX squelch change the force to 0
                 * i.e. get the RX status from Analog*/
                 if (rx_control == phymodRxSquelchOff) {
                     SIGDET_CTRL_1r_SIGNAL_DETECT_FRCf_SET(sigdet_ctrl1_reg, 0);
                     PHYMOD_IF_ERR_RETURN(  
                          WRITE_SIGDET_CTRL_1r(&acc_cpy, sigdet_ctrl1_reg));
                  }

            break;
            case phymodRxReset:
                err_code = wr_ln_dp_s_rstb(0);
                if (err_code != ERR_CODE_NONE){
                    return err_code;
                }
            break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);         
    return PHYMOD_E_NONE;
}

int quadra28_rx_squelch_get(const phymod_phy_access_t *phy, int *rx_squelch)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    SIGDET_CTRL_1r_t sigdet_ctrl1_reg;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&sigdet_ctrl1_reg, 0 , sizeof(SIGDET_CTRL_1r_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN(                
                     READ_SIGDET_CTRL_1r(&acc_cpy, &sigdet_ctrl1_reg));
                *rx_squelch = SIGDET_CTRL_1r_SIGNAL_DETECT_FRCf_GET(sigdet_ctrl1_reg) ?
                               1 : 0;
                break;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(                
             READ_SIGDET_CTRL_1r(&acc_cpy, &sigdet_ctrl1_reg));
        *rx_squelch = SIGDET_CTRL_1r_SIGNAL_DETECT_FRCf_GET(sigdet_ctrl1_reg) ?
                       1 : 0;
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}

int quadra28_display_eye_scan(const phymod_phy_access_t *phy)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_DIAG_OUT((" eyescan for lane = %d lanemap:%x\n",     lane_index, lane_map));

                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN
                    (merlin_quadra28_display_lane_state_hdr(&acc_cpy));
                PHYMOD_IF_ERR_RETURN
                    (merlin_quadra28_display_lane_state(&acc_cpy)); 
                PHYMOD_IF_ERR_RETURN
                        (merlin_quadra28_display_eye_scan(&acc_cpy));
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin_quadra28_display_lane_state_hdr(&acc_cpy));
        PHYMOD_IF_ERR_RETURN
            (merlin_quadra28_display_lane_state(&acc_cpy)); 
        PHYMOD_IF_ERR_RETURN
                (merlin_quadra28_display_eye_scan(&acc_cpy));
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int quadra28_ber_proj (const phymod_phy_access_t *phy, const phymod_phy_eyescan_options_t* eyescan_options)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
#ifdef SERDES_API_FLOATING_POINT
    USR_DOUBLE speedD;
    BCMI_QUADRA28_PRBS_CHK_CONFIGr_t chk_config;
#endif
    phymod_access_t acc_cpy;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
#ifdef SERDES_API_FLOATING_POINT
    PHYMOD_MEMSET(&chk_config, 0 , sizeof(BCMI_QUADRA28_PRBS_CHK_CONFIGr_t));	
#endif    
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_DIAG_OUT((" BER proj for lane = %d lanemap:%x\n",     lane_index, lane_map));

                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
#ifdef SERDES_API_FLOATING_POINT
                if ((speed == Q28_SPEED_40G) || (speed == Q28_SPEED_10G) 
                      || (speed == Q28_SPEED_20G) || (speed == Q28_SPEED_1G)) {
                    speedD = 10312 * 1000.0 * 1000.0;
                } else if((speed == Q28_SPEED_42G) || (speed == Q28_SPEED_HG11)) {
                    speedD = 10937 * 1000.0 * 1000.0;
                } else if(speed == Q28_SPEED_11P5G) {
                    speedD = 11500 * 1000.0 * 1000.0;
                } else {
                    speedD = 10312 * 1000.0 * 1000.0;
                }

                PHYMOD_IF_ERR_RETURN
                    (merlin_quadra28_eye_margin_proj((&acc_cpy), speedD, eyescan_options->ber_proj_scan_mode,
                               eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt));
                PHYMOD_IF_ERR_RETURN(
                     BCMI_QUADRA28_READ_PRBS_CHK_CONFIGr(&acc_cpy, &chk_config));
                BCMI_QUADRA28_PRBS_CHK_CONFIGr_TRNSUM_ERROR_COUNT_ENf_SET(chk_config, 0);
                PHYMOD_IF_ERR_RETURN(
                     BCMI_QUADRA28_WRITE_PRBS_CHK_CONFIGr(&acc_cpy, chk_config));
#else 
                PHYMOD_DIAG_OUT(("BER PROJ needs SERDES_API_FLOATING_POINT define to operate\n"));
                return PHYMOD_E_RESOURCE;
#endif

            }
        }
    } else {
#ifdef SERDES_API_FLOATING_POINT
        if ((speed == Q28_SPEED_40G) || (speed == Q28_SPEED_10G) 
                 || (speed == Q28_SPEED_20G) || (speed == Q28_SPEED_1G)) {
            speedD = 10312 * 1000.0 * 1000.0;
        } else if((speed == Q28_SPEED_42G) || (speed == Q28_SPEED_HG11)) {
            speedD = 10937 * 1000.0 * 1000.0;
        } else if(speed == Q28_SPEED_11P5G) {
            speedD = 11500 * 1000.0 * 1000.0;
        } else {
            speedD = 10312 * 1000.0 * 1000.0;
        }

        PHYMOD_IF_ERR_RETURN
            (merlin_quadra28_eye_margin_proj((&acc_cpy), speedD, eyescan_options->ber_proj_scan_mode,
                               eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt));
        PHYMOD_IF_ERR_RETURN(
             BCMI_QUADRA28_READ_PRBS_CHK_CONFIGr(&acc_cpy, &chk_config));
        BCMI_QUADRA28_PRBS_CHK_CONFIGr_TRNSUM_ERROR_COUNT_ENf_SET(chk_config, 0);
        PHYMOD_IF_ERR_RETURN(
             BCMI_QUADRA28_WRITE_PRBS_CHK_CONFIGr(&acc_cpy, chk_config));
#else 
        PHYMOD_DIAG_OUT(("BER PROJ needs SERDES_API_FLOATING_POINT define to operate\n"));
        return PHYMOD_E_RESOURCE;
#endif

    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_status_dump(const phymod_phy_access_t *phy)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    uint8_t trace_mem[768];

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);

    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ******* PHY status dump for Q28 PHY ID:0x%x ********\n", acc_cpy.addr));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ******* PHY status dump for side:%x ********\n", pkg_side));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_display_core_config(&acc_cpy));
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_display_core_state(&acc_cpy));
    
    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN
                    (merlin_quadra28_display_lane_state_hdr(&acc_cpy));
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_display_lane_state(&acc_cpy));
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_display_lane_config(&acc_cpy));
                if ((lane_index & 1) == 0) {
                    PHYMOD_IF_ERR_RETURN(
                         merlin_quadra28_read_event_log(&acc_cpy, trace_mem, 2));
                }
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin_quadra28_display_lane_state_hdr(&acc_cpy));
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_display_lane_state(&acc_cpy));
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_display_lane_config(&acc_cpy));
        PHYMOD_IF_ERR_RETURN(
             merlin_quadra28_read_event_log(&acc_cpy, trace_mem, 2));

    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_diagnostics_get(const phymod_phy_access_t *phy, phymod_phy_diagnostics_t* diag)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    merlin_quadra28_lane_state_st_define state;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
    if (pmd_mode == Q28_SINGLE_PMD) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN
                    (merlin_quadra28_read_lane_state_define(&acc_cpy, &state));
                diag->signal_detect = state.sig_det;
                diag->osr_mode = state.osr_mode.tx_rx;
                diag->rx_lock = state.rx_lock;
                diag->tx_ppm = state.tx_ppm;
                diag->clk90_offset = state.clk90;
                diag->clkp1_offset = state.clkp1;
                diag->p1_lvl = state.p1_lvl;
                diag->dfe1_dcd = state.dfe1_dcd;
                diag->dfe2_dcd = state.dfe2_dcd;
                diag->slicer_offset.offset_pe = state.pe;
                diag->slicer_offset.offset_ze = state.ze;
                diag->slicer_offset.offset_me = state.me;
                diag->slicer_offset.offset_po = state.po;
                diag->slicer_offset.offset_zo = state.zo;
                diag->slicer_offset.offset_mo = state.mo;
                diag->eyescan.heye_left = state.heye_left;
                diag->eyescan.heye_right = state.heye_right;
                diag->eyescan.veye_upper = state.veye_upper;
                diag->eyescan.veye_lower = state.veye_lower;
                diag->link_time = state.link_time;
                diag->pf_main = state.pf_main;
                diag->pf_hiz = state.pf_hiz;
                diag->pf_bst = state.pf_bst;
                diag->pf_low = 0; /* Not Availble in serdes API*/
                diag->pf2_ctrl = state.pf2_ctrl;
                diag->vga = state.vga;
                diag->dc_offset = state.dc_offset;
                diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
                diag->dfe1 = state.dfe1;
                diag->dfe2 = state.dfe2;
                diag->dfe3 = state.dfe3;
                diag->dfe4 = state.dfe4;
                diag->dfe5 = state.dfe5;
                diag->dfe6 = state.dfe6;
                diag->txfir_pre = state.txfir_pre;
                diag->txfir_main = state.txfir_main;
                diag->txfir_post1 = state.txfir_post1;
                diag->txfir_post2 = state.txfir_post2;
                diag->txfir_post3 = state.txfir_post3;
                diag->tx_amp_ctrl = 0; /* Not Availble in serdes API*/
                diag->br_pd_en = state.br_pd_en;
                break;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin_quadra28_read_lane_state_define(&acc_cpy, &state));
        diag->signal_detect = state.sig_det;
        diag->osr_mode = state.osr_mode.tx_rx;
        diag->rx_lock = state.rx_lock;
        diag->tx_ppm = state.tx_ppm;
        diag->clk90_offset = state.clk90;
        diag->clkp1_offset = state.clkp1;
        diag->p1_lvl = state.p1_lvl;
        diag->dfe1_dcd = state.dfe1_dcd;
        diag->dfe2_dcd = state.dfe2_dcd;
        diag->slicer_offset.offset_pe = state.pe;
        diag->slicer_offset.offset_ze = state.ze;
        diag->slicer_offset.offset_me = state.me;
        diag->slicer_offset.offset_po = state.po;
        diag->slicer_offset.offset_zo = state.zo;
        diag->slicer_offset.offset_mo = state.mo;
        diag->eyescan.heye_left = state.heye_left;
        diag->eyescan.heye_right = state.heye_right;
        diag->eyescan.veye_upper = state.veye_upper;
        diag->eyescan.veye_lower = state.veye_lower;
        diag->link_time = state.link_time;
        diag->pf_main = state.pf_main;
        diag->pf_hiz = state.pf_hiz;
        diag->pf_bst = state.pf_bst;
        diag->pf_low = 0; /* Not Availble in serdes API*/
        diag->pf2_ctrl = state.pf2_ctrl;
        diag->vga = state.vga;
        diag->dc_offset = state.dc_offset;
        diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
        diag->dfe1 = state.dfe1;
        diag->dfe2 = state.dfe2;
        diag->dfe3 = state.dfe3;
        diag->dfe4 = state.dfe4;
        diag->dfe5 = state.dfe5;
        diag->dfe6 = state.dfe6;
        diag->txfir_pre = state.txfir_pre;
        diag->txfir_main = state.txfir_main;
        diag->txfir_post1 = state.txfir_post1;
        diag->txfir_post2 = state.txfir_post2;
        diag->txfir_post3 = state.txfir_post3;
        diag->tx_amp_ctrl = 0; /* Not Availble in serdes API*/
        diag->br_pd_en = state.br_pd_en;
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int quadra28_soft_reset(const phymod_access_t* pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    int pkg_side = 0;
    uint32_t acc_flags = 0; 
    SIDE_SELECTIONr_t side_sel_reg_val;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    PMD_CONTROLr_t  pmd_ctrl;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&pmd_ctrl, 0 , sizeof(PMD_CONTROLr_t));	
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);
    if (reset_mode == phymodResetModeSoft) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
        BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_reset_set(const phymod_phy_access_t *phy,  const phymod_phy_reset_t* reset)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                        quadra28_channel_select(&acc_cpy,
                        ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                PHYMOD_IF_ERR_RETURN(
                        _quadra28_merlin_phy_reset_set(&acc_cpy, reset));
                if (lane_map == Q28_ALL_LANE) {
                    break;
                }
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(
             _quadra28_merlin_phy_reset_set(&acc_cpy, reset));
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

static int _quadra28_merlin_phy_reset_set( phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    /* TX AFE Lane Reset */
    switch (reset->tx) {
        /* In Reset */
        case phymodResetDirectionIn:
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc_val(1));
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc(1));
        break;
        /* Out Reset */
        case phymodResetDirectionOut:
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc_val(0));
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc(1));
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc_val(1));
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc(1));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc_val(0));
            PHYMOD_IF_ERR_RETURN(wr_afe_tx_reset_frc(1));

        break;
        default:
        break;
    }

    /* RX AFE Lane Reset */
    switch (reset->rx) {
        /* In Reset */
        case phymodResetDirectionIn:
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc_val(1));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc(1));
        break;
        /* Out Reset */
        case phymodResetDirectionOut:
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc_val(0));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc(1));
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc_val(1));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc(1));

            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc_val(0));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_reset_frc(1));

        break;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int _quadra28_phy_reset_get( const phymod_phy_access_t *phy,  phymod_phy_reset_t* reset)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    SIGDET_CTRL_1r_t sigdet_ctrl1_reg;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&sigdet_ctrl1_reg, 0 , sizeof(SIGDET_CTRL_1r_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN (
                   _quadra28_merlin_phy_reset_get(&acc_cpy, reset));
                break;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
          _quadra28_merlin_phy_reset_get(&acc_cpy, reset));

    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;

}

static int _quadra28_merlin_phy_reset_get( phymod_access_t *pa,  phymod_phy_reset_t* reset)
{
    uint8_t data = 0;
    uint16_t __err = 0;
    data = rd_afe_tx_reset_frc_val();
    if (data == 0) {
        /* Out of Reset */
        reset->tx = phymodResetDirectionOut;
    } else {
        /* In Reset */
        reset->tx = phymodResetDirectionIn;
    }
    data = rd_afe_rx_reset_frc_val();
    if (data == 0) {
        /* Out of Reset */
        reset->rx = phymodResetDirectionOut;
    } else {
        /* In Reset */
        reset->rx = phymodResetDirectionIn;
    }
    return __err;
}

int _quadra28_loopback_set(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    uint8_t datapath = 0;
    PMD_TRANSMIT_DISABLEr_t tx_disable;
    BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;

    PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   
    Q28_GET_DATAPATH(&acc_cpy, datapath);
    if (datapath == phymodDatapathUll && 
             loopback == phymodLoopbackRemotePMD) {
        PHYMOD_DIAG_OUT(("Remote loopback doesnt support 4-bit datapath\n"));
        return PHYMOD_E_PARAM;
    }
    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                        quadra28_channel_select(&acc_cpy,
                        ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                if (loopback == phymodLoopbackGlobalPMD) {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
                    BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_SET(pmd_ctrl, enable);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));
                    /*PHYMOD_IF_ERR_RETURN(
                         merlin_quadra28_dig_lpbk(&acc_cpy, enable));*/
                } else if (loopback == phymodLoopbackRemotePMD) {
                    PHYMOD_IF_ERR_RETURN(
                         merlin_quadra28_rmt_lpbk(&acc_cpy, enable));
                } else if(loopback == phymodLoopbackGlobal) {
                    BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
                    PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
                    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
                    /*PHYMOD_IF_ERR_RETURN(
                         merlin_quadra28_dig_lpbk(&acc_cpy, enable));*/
                    PHYMOD_IF_ERR_RETURN(
                       BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
                    BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_SET(pmd_ctrl, enable);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));
                    /* Squelch TX */
                    PHYMOD_IF_ERR_RETURN(
                        READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
                    BCMI_QUADRA28_PMD_TRANSMIT_DISABLEr_PMD_TRANSMITDISABLE_0f_SET
                                                     (tx_disable, enable);
                    PHYMOD_IF_ERR_RETURN(
                        WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_disable));
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                if (lane_map == Q28_ALL_LANE) {
                    break;
                }
            }
        }
    } else {
        if (loopback == phymodLoopbackGlobalPMD) { 
           PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
           BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_SET(pmd_ctrl, enable);
           PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));
            /*PHYMOD_IF_ERR_RETURN(
                 merlin_quadra28_dig_lpbk(&acc_cpy, enable));*/
        } else if (loopback == phymodLoopbackRemotePMD) {
            PHYMOD_IF_ERR_RETURN(
                 merlin_quadra28_rmt_lpbk(&acc_cpy, enable));
        } else if (loopback == phymodLoopbackGlobal) {
           BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
           PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
           QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
           
           /*PHYMOD_IF_ERR_RETURN(
           merlin_quadra28_dig_lpbk(&acc_cpy, enable));*/
           PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
           BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_SET(pmd_ctrl, enable);
           PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));

           /* Squelch TX */
           PHYMOD_IF_ERR_RETURN(
               READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_disable));
           BCMI_QUADRA28_PMD_TRANSMIT_DISABLEr_PMD_TRANSMITDISABLE_0f_SET
                                            (tx_disable, enable);
           PHYMOD_IF_ERR_RETURN(
               WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_disable));
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_loopback_get(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_DIG_LPBK_CONFIGr_t dig_loopback;
    RMT_LPBK_CONFIGr_t rmt_loopback;
    BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
   
    PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&dig_loopback, 0 , sizeof(BCMI_QUADRA28_DIG_LPBK_CONFIGr_t));	
    PHYMOD_MEMSET(&rmt_loopback, 0 , sizeof(RMT_LPBK_CONFIGr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                if (loopback == phymodLoopbackGlobalPMD) {
                    /*PHYMOD_IF_ERR_RETURN(
                        READ_DIG_LPBK_CONFIGr(&acc_cpy, &dig_loopback));*/
                   PHYMOD_IF_ERR_RETURN(
                       BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
                   *enable = BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_GET(pmd_ctrl);
                } else if (loopback == phymodLoopbackGlobal) {
                   QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
                   PHYMOD_IF_ERR_RETURN(
                       BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
                   *enable = BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_GET(pmd_ctrl);
                } else if (loopback == phymodLoopbackRemotePMD) {
                    PHYMOD_IF_ERR_RETURN(
                        READ_RMT_LPBK_CONFIGr(&acc_cpy, &rmt_loopback));
                    *enable = RMT_LPBK_CONFIGr_RMT_LPBK_ENf_GET(rmt_loopback);
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            }
        }
    } else {
        if (loopback == phymodLoopbackGlobalPMD) {
            PHYMOD_IF_ERR_RETURN(
                 BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
            *enable = BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_GET(pmd_ctrl);
        } else if (loopback == phymodLoopbackRemotePMD) {
            PHYMOD_IF_ERR_RETURN(
                READ_RMT_LPBK_CONFIGr(&acc_cpy, &rmt_loopback));
            *enable = RMT_LPBK_CONFIGr_RMT_LPBK_ENf_GET(rmt_loopback);
        } else if (loopback == phymodLoopbackGlobal) {
            QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
            *enable = BCMI_QUADRA28_PMD_CONTROLr_PMA_LOOPBACKf_GET(pmd_ctrl);

        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}

int _quadra28_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                    quadra28_channel_select(&acc_cpy,
                        ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                PHYMOD_IF_ERR_RETURN(quadra28_power_set(&acc_cpy, power));
                if (Q28_ALL_LANE == lane_map) {
                    break;
                }
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(quadra28_power_set(&acc_cpy, power));
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

static int quadra28_power_set(const phymod_access_t *pa, const phymod_phy_power_t* power)
{
    
    PMD_TRANSMIT_DISABLEr_t tx_disable;
    if (power->tx != phymodPowerNoChange) {
        if ( power->tx == phymodPowerOn) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_s_pwrdn(0));
            PHYMOD_IF_ERR_RETURN(   
              READ_PMD_TRANSMIT_DISABLEr(pa, &tx_disable));
            PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET(tx_disable, 0);
            PHYMOD_IF_ERR_RETURN(
                           WRITE_PMD_TRANSMIT_DISABLEr(pa, tx_disable)); 
        } else if (power->tx == phymodPowerOff) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_s_pwrdn(1));
            PHYMOD_IF_ERR_RETURN(
               READ_PMD_TRANSMIT_DISABLEr(pa, &tx_disable));
            PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET(tx_disable, 1);
            PHYMOD_IF_ERR_RETURN(
                           WRITE_PMD_TRANSMIT_DISABLEr(pa, tx_disable)); 
        } else if (power->tx == phymodPowerOffOn) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_s_pwrdn(1));
            PHYMOD_IF_ERR_RETURN(
               READ_PMD_TRANSMIT_DISABLEr(pa, &tx_disable));
            PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET(tx_disable, 1);
            PHYMOD_IF_ERR_RETURN(
                           WRITE_PMD_TRANSMIT_DISABLEr(pa, tx_disable)); 
            PHYMOD_USLEEP(500);
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_s_pwrdn(0));
            PHYMOD_IF_ERR_RETURN(
               READ_PMD_TRANSMIT_DISABLEr(pa, &tx_disable));
            PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET(tx_disable, 0);
            PHYMOD_IF_ERR_RETURN(
                           WRITE_PMD_TRANSMIT_DISABLEr(pa, tx_disable)); 
        }
    }
    if (power->rx != phymodPowerNoChange) {
        if (power->rx == phymodPowerOn) {
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc_val(0));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc(0));
        } else if (power->rx == phymodPowerOff) {
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc_val(1));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc(1));
        } else if (power->rx == phymodPowerOffOn) {
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc_val(1));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc(1));

            PHYMOD_USLEEP(500);
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc_val(0));
            PHYMOD_IF_ERR_RETURN(wr_afe_rx_pwrdn_frc(0));

        }
    }

    return PHYMOD_E_NONE;
}

int _quadra28_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                PHYMOD_IF_ERR_RETURN (
                    quadra28_power_get(&acc_cpy, power));
                break;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
              quadra28_power_get(&acc_cpy, power));
    }
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;

}
static int quadra28_power_get(const phymod_access_t *pa, phymod_phy_power_t* power)
{
    uint8_t temp = 0;
    uint16_t __err = 0;
    temp = rd_ln_tx_s_pwrdn();
    if (temp == 0) {
        power->tx = phymodPowerOn;
    } else {
        power->tx = phymodPowerOff;
    }
    temp = rd_afe_rx_pwrdn_frc();
    if (temp == 0) {
        power->rx = phymodPowerOn;
    } else {
        power->rx = phymodPowerOff;
    }
    return __err;
}

int _quadra28_pll_seq_restart(const phymod_access_t *pa, uint32_t flag, phymod_sequencer_operation_t operation)
{
    BCMI_QUADRA28_XGXSCONTROLr_t xgs_ctrl;
    int pkg_side = 0;
    uint32_t acc_flags = 0; 
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&xgs_ctrl, 0 , sizeof(BCMI_QUADRA28_XGXSCONTROLr_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        PHYMOD_IF_ERR_RETURN(
            quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_XGXSCONTROLr(&acc_cpy, &xgs_ctrl));
    if (operation == phymodSeqOpStop) {
        BCMI_QUADRA28_XGXSCONTROLr_START_SEQUENCERf_SET(xgs_ctrl, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_XGXSCONTROLr(&acc_cpy, xgs_ctrl));
    } else if(operation == phymodSeqOpStart) {
        BCMI_QUADRA28_XGXSCONTROLr_START_SEQUENCERf_SET(xgs_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_XGXSCONTROLr(&acc_cpy, xgs_ctrl));
    } else if (operation == phymodSeqOpRestart) {
        BCMI_QUADRA28_XGXSCONTROLr_START_SEQUENCERf_SET(xgs_ctrl, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_XGXSCONTROLr(&acc_cpy, xgs_ctrl));
        PHYMOD_USLEEP(1000);
        BCMI_QUADRA28_XGXSCONTROLr_START_SEQUENCERf_SET(xgs_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_XGXSCONTROLr(&acc_cpy, xgs_ctrl));
    } else {
        return PHYMOD_E_PARAM;
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}

int _quadra28_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                    quadra28_channel_select(&acc_cpy,
                        ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                PHYMOD_IF_ERR_RETURN(
                     merlin_quadra28_apply_txfir_cfg(&acc_cpy, (int8_t)tx->pre, (int8_t)tx->main, (int8_t)tx->post, (int8_t)tx->post2));
                if (Q28_ALL_LANE == lane_map) {
                    break;
                }
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(
          merlin_quadra28_apply_txfir_cfg(&acc_cpy, (int8_t)tx->pre, (int8_t)tx->main, (int8_t)tx->post, (int8_t)tx->post2));
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx) 
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    int8_t value;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                    quadra28_channel_select(&acc_cpy, lane_index));
                break;
           }
        }
    }
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_tx_afe(&acc_cpy, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_tx_afe(&acc_cpy, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_tx_afe(&acc_cpy, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_tx_afe(&acc_cpy, TX_AFE_POST2, &value));
    tx->post2 = value;

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0, idx = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                    quadra28_channel_select(&acc_cpy,
                        ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));
                if (rx->vga.enable) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_VGA, rx->vga.value));
                } 

                for (idx = 0 ; idx < rx->num_of_dfe_taps ; idx++) {
                    if (rx->dfe[idx].enable) {
                        PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
                        PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_DFE1 + idx,
                                             rx->dfe[idx].value));
                    }
                }
                if (rx->peaking_filter.enable) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_PF, rx->peaking_filter.value));
                } 
                if (rx->low_freq_peaking_filter.enable) {
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
                    PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
                }

                if (Q28_ALL_LANE == lane_map) {
                    break;
                }
            }
        }
    } else {
        if (rx->vga.enable) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_VGA, rx->vga.value));
        } 

        for (idx = 0 ; idx < rx->num_of_dfe_taps ; idx++) {
            if (rx->dfe[idx].enable) {
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
                PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_DFE1 + idx,
                                     rx->dfe[idx].value));
            }
        }
        if (rx->peaking_filter.enable) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_PF, rx->peaking_filter.value));
        } 
        if (rx->low_freq_peaking_filter.enable) {
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 1));
            PHYMOD_IF_ERR_RETURN(merlin_quadra28_write_rx_afe(&acc_cpy, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
        }
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0, idx = 0;
    uint32_t speed = 0;
    int8_t data = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    if (rx) {
        PHYMOD_MEMSET(rx, 0 , sizeof(phymod_rx_t));	
    }else{
	    return PHYMOD_E_MEMORY;	
    }

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                    quadra28_channel_select(&acc_cpy, lane_index));
                break;
           }
        }
    }

    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_rx_afe(&acc_cpy, RX_AFE_VGA, &data));
    rx->vga.value = data;

    for (idx = 0; idx < 5 ; idx++) {
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_rx_afe(&acc_cpy, RX_AFE_DFE1 + idx, &data));
        rx->dfe[idx].value = data;
        rx->dfe[idx].enable = 1 ;
    }
    rx->num_of_dfe_taps = 5;

    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_rx_afe(&acc_cpy, RX_AFE_PF, &data));
    rx->peaking_filter.value = data;
    PHYMOD_IF_ERR_RETURN(merlin_quadra28_read_rx_afe(&acc_cpy, RX_AFE_PF2, &data));
    rx->low_freq_peaking_filter.value = data;

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_autoneg_ability_set(const phymod_access_t* pa, q28_an_ability_t an_ability)
{
    BCMI_QUADRA28_AN_ADVERTISEMENT_1r_t pause_adv;
    BCMI_QUADRA28_AN_ADVERTISEMENT_3r_t fec_adv;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
   
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_UNUSED_PARAM(pmd_mode);
    PHYMOD_MEMSET(&pause_adv, 0, sizeof(BCMI_QUADRA28_AN_ADVERTISEMENT_1r_t));
    PHYMOD_MEMSET(&fec_adv, 0, sizeof(BCMI_QUADRA28_AN_ADVERTISEMENT_3r_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_ADVERTISEMENT_1r(&acc_cpy, &pause_adv));
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_ADVERTISEMENT_3r(&acc_cpy, &fec_adv));

    BCMI_QUADRA28_AN_ADVERTISEMENT_1r_PAUSEf_SET(pause_adv, an_ability.pause_ability);
    BCMI_QUADRA28_AN_ADVERTISEMENT_3r_FEC_REQUESTEDf_SET(fec_adv, an_ability.fec_ability);
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_AN_ADVERTISEMENT_1r(&acc_cpy, pause_adv));
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_AN_ADVERTISEMENT_3r(&acc_cpy, fec_adv));

    return PHYMOD_E_NONE;
}

int _quadra28_phy_autoneg_ability_get(const phymod_access_t* pa, phymod_autoneg_ability_t* an_ability)
{
    uint16_t ability;
    BCMI_QUADRA28_AN_ADVERTISEMENT_1r_t pause_ability;
    BCMI_QUADRA28_AN_ADVERTISEMENT_2r_t tech_ability;
    BCMI_QUADRA28_AN_ADVERTISEMENT_3r_t fec_ability;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
   
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_UNUSED_PARAM(pmd_mode);

    PHYMOD_MEMSET(&pause_ability, 0, sizeof(BCMI_QUADRA28_AN_ADVERTISEMENT_1r_t));
    PHYMOD_MEMSET(&tech_ability, 0, sizeof(BCMI_QUADRA28_AN_ADVERTISEMENT_2r_t));
    PHYMOD_MEMSET(&fec_ability, 0, sizeof(BCMI_QUADRA28_AN_ADVERTISEMENT_3r_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_ADVERTISEMENT_1r(&acc_cpy, &pause_ability));
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_ADVERTISEMENT_2r(&acc_cpy, &tech_ability));
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_ADVERTISEMENT_3r(&acc_cpy, &fec_ability));

    ability = BCMI_QUADRA28_AN_ADVERTISEMENT_1r_PAUSEf_GET(pause_ability);
    if (ability == Q28_SYMM_PAUSE) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability);
    } else if(ability == Q28_ASYM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability);
    } else {
        PHYMOD_AN_CAP_SYMM_PAUSE_CLR(an_ability);
        PHYMOD_AN_CAP_ASYM_PAUSE_CLR(an_ability);
    }
    ability = BCMI_QUADRA28_AN_ADVERTISEMENT_2r_TECHABILITYf_GET(tech_ability); 
    /*first check cl73 ability*/
    switch (ability) {
        case Q28_CL73_100GBASE_CR10:
            PHYMOD_AN_CAP_100G_CR10_SET(an_ability->an_cap);
        break;
        case Q28_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability->an_cap);
        break;
        case Q28_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability->an_cap);
        break;
        case Q28_CL73_10GBASE_KR:
            PHYMOD_AN_CAP_10G_KR_SET(an_ability->an_cap);
        break;
        case Q28_CL73_1000BASE_KX:
            PHYMOD_AN_CAP_1G_KX_SET(an_ability->an_cap);
        break;
        case Q28_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability->an_cap);
        break;
        case Q28_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability->an_cap);
        break;
        default:
            break;
    }
    an_ability->an_fec = BCMI_QUADRA28_AN_ADVERTISEMENT_3r_FEC_REQUESTEDf_GET(fec_ability);

    return PHYMOD_E_NONE;
}

int _quadra28_phy_autoneg_get(const phymod_access_t* pa, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    BCMI_QUADRA28_AN_STATUSr_t an_status;
    BCMI_QUADRA28_AN_CONTROLr_t an_control;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    phymod_interface_t intf;
    phymod_ref_clk_t ref_clk;
    uint32_t intf_modes = 0, an_enable = 0, an_cl37_sts = 0;
 
    PHYMOD_MEMSET(&an_status, 0, sizeof(BCMI_QUADRA28_AN_STATUSr_t));
    PHYMOD_MEMSET(&an_control, 0, sizeof(BCMI_QUADRA28_AN_CONTROLr));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_IF_ERR_RETURN (
         quadra28_get_config_mode(&acc_cpy, &intf, &speed, &ref_clk, &intf_modes));
    Q28_UNUSED_PARAM(ref_clk);   
    Q28_UNUSED_PARAM(intf_modes);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    if ((speed == 1000) && (intf == phymodInterfaceSR)) {
        PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_read(&acc_cpy, 0x7ffe0, &an_enable));
        PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_read(&acc_cpy, 0x7ffe1, &an_cl37_sts));
        if (an_enable & (1 << 12)) {
            an->enable = 1;
            an->an_mode = phymod_AN_MODE_CL37;
        } else {
            an->enable = 0;
        }
        if (an_cl37_sts & (1 << 5)) {
            *an_done = 1;
        } else {
            *an_done = 0;
        }
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_AN_STATUSr(&acc_cpy, &an_status));
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_AN_CONTROLr(&acc_cpy, &an_control));
        an->an_mode = phymod_AN_MODE_CL73;
        *an_done = BCMI_QUADRA28_AN_STATUSr_AUTO_NEGOTIATIONCOMPLETEf_GET(an_status);
        an->enable = BCMI_QUADRA28_AN_CONTROLr_AUTO_NEGOTIATIONENABLEf_GET(an_control);
    }
   


    return PHYMOD_E_NONE;
}

int _quadra28_phy_autoneg_set(const phymod_access_t* pa, const phymod_autoneg_control_t* an)
{
    BCMI_QUADRA28_AN_CONTROLr_t an_control;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0;
    BCMI_QUADRA28_GENERAL_PURPOSE_3r_t gp_reg3_reg_val;
    BCMI_QUADRA28_APPS_MODE_1r_t apps_mode1;
    BCMI_QUADRA28_PMD_TRANSMIT_DISABLEr_t tx_dis;
    unsigned int an_enable = 0, retry_cnt=20;
    BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
    BCMI_QUADRA28_MERLIN_MISC_CTRLr_t reset_ack;
    BCMI_QUADRA28_XGXSSTATUSr_t xgs_sts;
    phymod_interface_t intf;
    phymod_ref_clk_t ref_clk;
    uint32_t intf_modes = 0;

    PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
    PHYMOD_MEMSET(&apps_mode0, 0, sizeof(apps_mode0));
    PHYMOD_MEMSET(&apps_mode1, 0, sizeof(apps_mode1));
    PHYMOD_MEMSET(&tx_dis, 0, sizeof(tx_dis));
    PHYMOD_MEMSET(&gp_reg3_reg_val, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_3r_t));
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));

    PHYMOD_IF_ERR_RETURN (
         quadra28_get_config_mode(&acc_cpy, &intf, &speed, &ref_clk, &intf_modes));
    Q28_UNUSED_PARAM(ref_clk);   
    Q28_UNUSED_PARAM(intf_modes);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    PHYMOD_MEMSET(&an_control, 0, sizeof(BCMI_QUADRA28_AN_CONTROLr));
    if (an->an_mode == phymod_AN_MODE_CL37 ||
            ((speed == 1000) && (intf == phymodInterfaceSR))) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_APPS_MODE_1r (&acc_cpy, &apps_mode1));
        if (!BCMI_QUADRA28_APPS_MODE_1r_RETIMER_MODEf_GET(apps_mode1)) {
            /*CL37 will be set only if the chip is confiured as re-timer
             * Not enabling if re-time mode is not set*/
            /* Fix for PHY-2821*/
            return PHYMOD_E_NONE;
        }
        /* Configure 1G(1.25G)-retime */
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(&acc_cpy, &gp_reg3_reg_val));
        apps_mode0.v[0] = gp_reg3_reg_val.v[0];
        BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(&acc_cpy, apps_mode0));

        PHYMOD_IF_ERR_RETURN(_quadra28_intf_update_wait_check(&acc_cpy,
                    apps_mode0.v[0], 50000));
        /*PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_APPS_MODE_1r (&acc_cpy, &apps_mode1));
        BCMI_QUADRA28_APPS_MODE_1r_RETIMER_MODEf_SET(apps_mode1,1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_1r (&acc_cpy, apps_mode1));*/
        if (an->enable) {
            BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0, 0x81);
        } else {
            BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0, 0x83);
        }
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r (&acc_cpy, apps_mode0));

        PHYMOD_IF_ERR_RETURN(_quadra28_intf_update_wait_check(&acc_cpy,
                    apps_mode0.v[0], 50000));
        if (an->enable) {
            BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0, 0x1);
        } else {
            BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0, 0x3);
        }
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r (&acc_cpy, apps_mode0));

        PHYMOD_IF_ERR_RETURN(_quadra28_intf_update_wait_check(&acc_cpy,
                    apps_mode0.v[0], 50000));

        /* Perform softreset and wait for reset to clear*/ 
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_PMD_CONTROLr(&acc_cpy, &pmd_ctrl));
        BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_CONTROLr(&acc_cpy, pmd_ctrl));
        do {
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_MERLIN_MISC_CTRLr(&acc_cpy, &reset_ack));
            PHYMOD_USLEEP(1000);
            retry_cnt --;
        } while (((reset_ack.v[0] & 0x8000) != 0x8000) && retry_cnt);
        if (retry_cnt <= 0) {
            return PHYMOD_E_TIMEOUT;
        }
        retry_cnt = 20;
        do {
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_XGXSSTATUSr(&acc_cpy, &xgs_sts));
            PHYMOD_USLEEP(800);
            retry_cnt --;
        }  while (((xgs_sts.v[0] & 0x800) != 0x800) && retry_cnt);
        if (retry_cnt <= 0) {
            return PHYMOD_E_TIMEOUT;
        } 
        /* Enable AN by disabling tx*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_PMD_TRANSMIT_DISABLEr(&acc_cpy, &tx_dis));
        BCMI_QUADRA28_PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET(tx_dis,1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_dis));

        PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_read(&acc_cpy, 0x7ffe0, &an_enable));
        if (an->enable == 1) {
            an_enable |= 1 << 12;
        } else {
            an_enable &= ~(1 << 12);
        }
        PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(&acc_cpy, 0x7ffe0, an_enable));
        BCMI_QUADRA28_PMD_TRANSMIT_DISABLEr_GLOBAL_PMDTRANSMIT_DISABLEf_SET(tx_dis,0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_TRANSMIT_DISABLEr(&acc_cpy, tx_dis));
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_AN_CONTROLr(&acc_cpy, &an_control));
   
        BCMI_QUADRA28_AN_CONTROLr_AUTO_NEGOTIATIONENABLEf_SET(an_control, an->enable);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_AN_CONTROLr(&acc_cpy, an_control));
    }

    return PHYMOD_E_NONE;
}

int _quadra28_phy_autoneg_remote_ability_get(const phymod_access_t* pa, phymod_autoneg_ability_t* an_ability)
{
    BCMI_QUADRA28_AN_LP_BASE_PAGE_ABILITY_1r_t lp_pause_ability;
    BCMI_QUADRA28_AN_LP_BASE_PAGE_ABILITY_2r_t lp_tech_ability;
    BCMI_QUADRA28_AN_LP_BASE_PAGE_ABILITY_3r_t lp_fec_ability;
    uint16_t ability;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
   
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_UNUSED_PARAM(pmd_mode);

    PHYMOD_MEMSET(&lp_pause_ability, 0, sizeof(BCMI_QUADRA28_AN_LP_BASE_PAGE_ABILITY_1r_t));
    PHYMOD_MEMSET(&lp_tech_ability, 0, sizeof(BCMI_QUADRA28_AN_LP_BASE_PAGE_ABILITY_2r_t));
    PHYMOD_MEMSET(&lp_fec_ability, 0, sizeof(BCMI_QUADRA28_AN_LP_BASE_PAGE_ABILITY_3r_t));
 
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_LP_BASE_PAGE_ABILITY_1r(&acc_cpy, &lp_pause_ability));
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_LP_BASE_PAGE_ABILITY_2r(&acc_cpy, &lp_tech_ability));
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_AN_LP_BASE_PAGE_ABILITY_3r(&acc_cpy, &lp_fec_ability));

    ability = (lp_pause_ability.v[0] >> 10) & 0x7;
    if (ability == Q28_SYMM_PAUSE) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability);
    } else if(ability == Q28_ASYM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability);
    } else {
        PHYMOD_AN_CAP_SYMM_PAUSE_CLR(an_ability);
        PHYMOD_AN_CAP_ASYM_PAUSE_CLR(an_ability);
    }
    ability = (lp_tech_ability.v[0] >> 5) & 0x7ff; 
    /*first check cl73 ability*/
    switch (ability) {
        case Q28_CL73_100GBASE_CR10:
            PHYMOD_AN_CAP_100G_CR10_SET(an_ability->an_cap);
        break;
        case Q28_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability->an_cap);
        break;
        case Q28_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability->an_cap);
        break;
        case Q28_CL73_10GBASE_KR:
            PHYMOD_AN_CAP_10G_KR_SET(an_ability->an_cap);
        break;
        case Q28_CL73_1000BASE_KX:
            PHYMOD_AN_CAP_1G_KX_SET(an_ability->an_cap);
        break;
        case Q28_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability->an_cap);
        break;
        case Q28_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability->an_cap);
        break;
        default:
            break;
    }
    an_ability->an_fec = (lp_fec_ability.v[0] >> 14) & 0x3 ;

    return PHYMOD_E_NONE;
}

int _quadra28_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    int pkg_side = 0;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
   
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    QUADRA28_GET_LOC(phy, pkg_side);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_APPS_MODE_0r(&acc_cpy, &apps_mode0_reg_val));
    if (pkg_side == Q28_INTF_LINE) {
        BCMI_QUADRA28_APPS_MODE_0r_LINE_FORCE_CL72f_SET(apps_mode0_reg_val, (cl72_en ? 1 :0));
    } else {
        BCMI_QUADRA28_APPS_MODE_0r_SYSTEM_FORCE_CL72f_SET(apps_mode0_reg_val, (cl72_en ? 1 :0));
    }
    if (pmd_mode == Q28_SINGLE_PMD) {
        PHYMOD_IF_ERR_RETURN (quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }
    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));

    BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 1);

    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));

    PHYMOD_IF_ERR_RETURN(
            _quadra28_intf_update_wait_check(&acc_cpy,apps_mode0_reg_val.v[0], 50000));

    PHYMOD_USLEEP(500);

    BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
    
    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));

    PHYMOD_IF_ERR_RETURN(
            _quadra28_intf_update_wait_check(&acc_cpy,apps_mode0_reg_val.v[0], 50000));

    return PHYMOD_E_NONE;
}

int _quadra28_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    int pkg_side = 0;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t mode_sts_val;
   
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&mode_sts_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_UNUSED_PARAM(pmd_mode);
    /* Get the lane map from phymod access */
    QUADRA28_GET_LOC(phy, pkg_side);

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_APPS_MODE_0r(&acc_cpy, &mode_sts_val));
    if (pkg_side == Q28_INTF_LINE) {
        *cl72_en = BCMI_QUADRA28_APPS_MODE_0r_LINE_FORCE_CL72f_GET(mode_sts_val);
    } else {
        *cl72_en = BCMI_QUADRA28_APPS_MODE_0r_SYSTEM_FORCE_CL72f_GET(mode_sts_val);
    }

    return PHYMOD_E_NONE;
}

int _quadra28_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    int lane_map = 0;
    int pkg_side = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    uint32_t cl72_status = 0xFFFF;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_t cl72_sts;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&cl72_sts, 0, sizeof(BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    PHYMOD_IF_ERR_RETURN (
        BCMI_QUADRA28_READ_BASE_R_PMD_STATUS_151r(&acc_cpy, &cl72_sts));

    if (pmd_mode == Q28_SINGLE_PMD) {
        if (lane_map & 1) {
            cl72_status &= BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_RECEIVER_STATUS_0f_GET(cl72_sts);
        }
        if (lane_map & 2) {
            cl72_status &= BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_RECEIVER_STATUS_1f_GET(cl72_sts);
        }
        if (lane_map & 4) {
            cl72_status &= BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_RECEIVER_STATUS_2f_GET(cl72_sts);
        }
        if (lane_map & 8) {
            cl72_status &= BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_RECEIVER_STATUS_3f_GET(cl72_sts);
        }

    } else {
        cl72_status &= BCMI_QUADRA28_BASE_R_PMD_STATUS_151r_RECEIVER_STATUS_0f_GET(cl72_sts);
    }
    status->locked = cl72_status;
    PHYMOD_IF_ERR_RETURN (
        _quadra28_phy_cl72_get(phy, &status->enabled));
    

    return PHYMOD_E_NONE;
}

int _quadra28_finish_mask_seq(const phymod_access_t *pa) 
{
    int retry_cnt = 20;

    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));

    PHYMOD_IF_ERR_RETURN (
        BCMI_QUADRA28_READ_APPS_MODE_0r(pa, &apps_mode0_reg_val));

    BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 1);

    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa,apps_mode0_reg_val));

    PHYMOD_IF_ERR_RETURN(
            _quadra28_intf_update_wait_check(pa,apps_mode0_reg_val.v[0], 50000));
    {
        BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
        PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));

        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_PMD_CONTROLr(pa, &pmd_ctrl));
        BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_CONTROLr(pa, pmd_ctrl));
    }
    {
        BCMI_QUADRA28_MERLIN_MISC_CTRLr_t reset_ack;
        BCMI_QUADRA28_XGXSSTATUSr_t xgs_sts;
        do {
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_MERLIN_MISC_CTRLr(pa, &reset_ack));
            PHYMOD_USLEEP(1000);
            retry_cnt --;
        } while (((reset_ack.v[0] & 0x8000) != 0x8000) && retry_cnt);
        if (retry_cnt <= 0) {
            return PHYMOD_E_TIMEOUT;
        }
        retry_cnt = 20;
        do {
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_XGXSSTATUSr(pa, &xgs_sts));
            PHYMOD_USLEEP(800);
            retry_cnt --;
        }  while (((xgs_sts.v[0] & 0x800) != 0x800) && retry_cnt);
        if (retry_cnt <= 0) {
            return PHYMOD_E_TIMEOUT;
        } 
    } 

    BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
    
    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));

    PHYMOD_IF_ERR_RETURN(
            _quadra28_intf_update_wait_check(pa, apps_mode0_reg_val.v[0], 50000));

    return PHYMOD_E_NONE;

}

int _quadra28_phy_retimer_enable(const phymod_access_t *pa, uint32_t enable)
{
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_1r_t apps_mode1_reg;
    SIDE_SELECTIONr_t side_sel_reg_val;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&apps_mode1_reg, 0, sizeof(BCMI_QUADRA28_APPS_MODE_1r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    if (pmd_mode == Q28_SINGLE_PMD) {
        if (enable) {
            PHYMOD_DIAG_OUT(("Retimer not supported in 40G/42G. So enabling repeater\n"));
            return PHYMOD_E_NONE;
        }
        PHYMOD_IF_ERR_RETURN (
             quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }
    PHYMOD_IF_ERR_RETURN (
        BCMI_QUADRA28_READ_APPS_MODE_1r(&acc_cpy, &apps_mode1_reg));

    BCMI_QUADRA28_APPS_MODE_1r_RETIMER_MODEf_SET(apps_mode1_reg, enable);

    PHYMOD_IF_ERR_RETURN (
        BCMI_QUADRA28_WRITE_APPS_MODE_1r(&acc_cpy, apps_mode1_reg));

    PHYMOD_IF_ERR_RETURN (
        _quadra28_finish_mask_seq(&acc_cpy));
    
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_phy_retimer_enable_get(const phymod_access_t *pa, uint32_t *enable)
{
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_1r_t apps_mode1_reg;
   
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&apps_mode1_reg, 0, sizeof(BCMI_QUADRA28_APPS_MODE_1r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_UNUSED_PARAM(pmd_mode);

    PHYMOD_IF_ERR_RETURN (
        BCMI_QUADRA28_READ_APPS_MODE_1r(&acc_cpy, &apps_mode1_reg));

    *enable = BCMI_QUADRA28_APPS_MODE_1r_RETIMER_MODEf_GET(apps_mode1_reg);

    return PHYMOD_E_NONE;
}

int _quadra28_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    int pkg_side = 0;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_1r_t apps_mode1_reg_val;
   
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&apps_mode1_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_1r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    QUADRA28_GET_LOC(phy, pkg_side);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_APPS_MODE_1r(&acc_cpy, &apps_mode1_reg_val));
    if (pkg_side == Q28_INTF_LINE) {
        apps_mode1_reg_val.v[0] &= 0xCFFF;
        apps_mode1_reg_val.v[0] |= (enable ? (0x3 << 12) : 0);
    } else {
        apps_mode1_reg_val.v[0] &= 0x3FFF;
        apps_mode1_reg_val.v[0] |= (enable ? (0xC << 12) : 0);
    }
    if (pmd_mode == Q28_SINGLE_PMD) {
        PHYMOD_IF_ERR_RETURN (quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }

    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_1r(&acc_cpy,apps_mode1_reg_val));

    PHYMOD_IF_ERR_RETURN (
        _quadra28_finish_mask_seq(&acc_cpy));

    return PHYMOD_E_NONE;
}

int _quadra28_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    int pkg_side = 0;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_1r_t apps_mode1_reg_val;
   
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&apps_mode1_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_1r_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_UNUSED_PARAM(pmd_mode);

    /* Get the lane map from phymod access */
    QUADRA28_GET_LOC(phy, pkg_side);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_APPS_MODE_1r(&acc_cpy, &apps_mode1_reg_val));

    if (pkg_side == Q28_INTF_LINE) {
        *enable = (apps_mode1_reg_val.v[0] & 0x3000) ? 1 : 0;
    } else {
        *enable = (apps_mode1_reg_val.v[0] & 0xC000) ? 1 : 0;
    }

    return PHYMOD_E_NONE;
}

int _quadra28_set_datapath(const phymod_access_t *pa, int speed, phymod_datapath_t op_datapath)
{
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    SIDE_SELECTIONr_t side_sel_reg_val;
    uint16_t max_lane = 0, lane_idx = 0, retry_cnt =0;
    uint32_t chip_id;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;

    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0 , sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));	
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0 , sizeof(GENERAL_PURPOSE_3r_t));	
    PHYMOD_MEMCPY(&acc_cpy, pa, sizeof(phymod_access_t));
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_0r(&acc_cpy, &apps_mode0_reg_val));
    if (speed >= Q28_SPEED_40G) {
        PHYMOD_IF_ERR_RETURN (
                quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }
    if (op_datapath == phymodDatapathNormal) {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, 0);
    } else if (op_datapath == phymodDatapathUll) {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, 1);
    } else {
        BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val, 0);
    }
    
    if (speed >= Q28_SPEED_40G) {
        PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
        PHYMOD_IF_ERR_RETURN(
                _quadra28_finish_mask_seq(&acc_cpy));
    } else {
        chip_id =  _quadra28_get_chip_id(&acc_cpy);
        max_lane = (chip_id == QUADRA28_82752_CHIP_ID) ? 2 : QUADRA28_MAX_LANE;
        acc_cpy.addr &= ~(0x3);
        for (lane_idx = 0; lane_idx < max_lane; lane_idx ++) {
            retry_cnt = 20;
            acc_cpy.addr |= lane_idx;
            BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
            PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
            do {
                PHYMOD_IF_ERR_RETURN(
                  BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(&acc_cpy,&gen_pur_reg_3r_val));
                PHYMOD_USLEEP(200);
            } while(((gen_pur_reg_3r_val.v[0] & 0x80) != 0) && retry_cnt--);
            if (retry_cnt == 0) {
                return PHYMOD_E_TIMEOUT;
            }
            PHYMOD_IF_ERR_RETURN(
                    _quadra28_finish_mask_seq(&acc_cpy));
            acc_cpy.addr &= ~(0x3);
        }
        acc_cpy.addr = pa->addr;
    }
    
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}


int _quadra28_i2c_write(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr,
                       uint32_t no_of_bytes, const uint8_t *write_data)
{
    BCMI_QUADRA28_I2C_CONTROLr_t   i2c_ctrl;
    uint8_t data[QUADRA28_I2C_MAX_BYTE];
    uint32_t index = 0;
    
    if (no_of_bytes > QUADRA28_I2C_MAX_BYTE) {
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(&i2c_ctrl, 0, sizeof(BCMI_QUADRA28_I2C_CONTROLr_t));
    PHYMOD_MEMSET(data, 0, QUADRA28_I2C_MAX_BYTE);
    PHYMOD_MEMCPY(data, write_data, no_of_bytes);

    /* Clear the latched status */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_I2C_CONTROLr(pa, &i2c_ctrl));
    for (index = 0; index < (no_of_bytes/Q28_MAX_BYTES); index++) { 
        PHYMOD_IF_ERR_RETURN
            (_quadra28_i2c_rw(pa, slv_addr, (start_addr+(index*Q28_MAX_BYTES)), Q28_MAX_BYTES, Q28_I2C_WRITE, (data+(index*Q28_MAX_BYTES))));
    }

    return PHYMOD_E_NONE;
}

int _quadra28_i2c_read(const phymod_access_t *pa, uint32_t i2c_slave_addr, uint32_t slave_start_addr,
                      uint32_t no_of_bytes, uint8_t *read_data)
{
    BCMI_QUADRA28_I2C_CONTROLr_t   i2c_ctrl;

    PHYMOD_MEMSET(&i2c_ctrl, 0, sizeof(BCMI_QUADRA28_I2C_CONTROLr_t));

    /* Clear the latched status */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_I2C_CONTROLr(pa, &i2c_ctrl));
    PHYMOD_IF_ERR_RETURN
            (_quadra28_i2c_rw(pa, i2c_slave_addr, slave_start_addr, no_of_bytes, Q28_I2C_READ , read_data));
    return PHYMOD_E_NONE;
}

int _quadra28_i2c_rw(const phymod_access_t *pa, uint32_t slv_addr, 
                     uint32_t slave_start_addr, uint16_t xfer_cnt, uint8_t acc_type, uint8_t *data) 
{
    BCMI_QUADRA28_I2C_CONTROLr_t    i2c_control;
    BCMI_QUADRA28_I2C_TRANSFER_SIZEr_t  i2c_xfer_size; 
    BCMI_QUADRA28_I2C_EXTERNAL_NVM_ADDRESSr_t eeprom_start_addr; 
    BCMI_QUADRA28_I2C_INTERNAL_ADDRESSr_t  nvram_addr;
    BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_t i2c_config;
    uint16_t cnt_index = 0, temp_cnt = xfer_cnt;
    uint32_t ram_data = 0;
    uint32_t tries = 10;

    PHYMOD_MEMSET(&i2c_control, 0, sizeof(BCMI_QUADRA28_I2C_CONTROLr_t));
    PHYMOD_MEMSET(&i2c_xfer_size, 0, sizeof(BCMI_QUADRA28_I2C_TRANSFER_SIZEr_t));
    PHYMOD_MEMSET(&eeprom_start_addr, 0, sizeof(BCMI_QUADRA28_I2C_EXTERNAL_NVM_ADDRESSr_t));
    PHYMOD_MEMSET(&nvram_addr, 0, sizeof(BCMI_QUADRA28_I2C_INTERNAL_ADDRESSr_t));
    PHYMOD_MEMSET(&i2c_config, 0, sizeof(BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_t));

    if (acc_type == Q28_I2C_WRITE) {

        /* Setup Starting address*/
        BCMI_QUADRA28_I2C_EXTERNAL_NVM_ADDRESSr_SET(eeprom_start_addr, slave_start_addr);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_EXTERNAL_NVM_ADDRESSr(pa, eeprom_start_addr));
        BCMI_QUADRA28_I2C_INTERNAL_ADDRESSr_SET(nvram_addr, QUADRA28_I2C_RAM_START_ADDRESS);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_INTERNAL_ADDRESSr(pa, nvram_addr));

        BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_IMC_SL_DEV_ADDf_SET(i2c_config, slv_addr);
        BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_IMC_I2C_SPEEDf_SET(i2c_config, 0);
        BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_IMC_XFER_ADDR_23_16f_SET(i2c_config, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_SLAVE_ID_ADDRESSr(pa, i2c_config));

        /* Write Data in to RAM location*/
        for (cnt_index = 0; cnt_index < temp_cnt; cnt_index++) {
            PHYMOD_IF_ERR_RETURN(
                phymod_raw_iblk_write(pa, (QUADRA28_I2C_RAM_START_ADDRESS + cnt_index),(0xff << 8) | data[cnt_index]));
        }

        BCMI_QUADRA28_I2C_TRANSFER_SIZEr_IMC_XFER_CNTf_SET(i2c_xfer_size, xfer_cnt);
        
        /* Add Tx count*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_TRANSFER_SIZEr(pa, i2c_xfer_size));
    } else {
        BCMI_QUADRA28_I2C_TRANSFER_SIZEr_IMC_XFER_CNTf_SET(i2c_xfer_size, xfer_cnt);
        /* Add Tx count*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_TRANSFER_SIZEr(pa, i2c_xfer_size));
        
        /* Setup Starting address*/
        BCMI_QUADRA28_I2C_EXTERNAL_NVM_ADDRESSr_SET(eeprom_start_addr, slave_start_addr);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_EXTERNAL_NVM_ADDRESSr(pa, eeprom_start_addr));

        /* Setting up internal RAm address*/
        BCMI_QUADRA28_I2C_INTERNAL_ADDRESSr_SET(nvram_addr, QUADRA28_I2C_RAM_START_ADDRESS);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_INTERNAL_ADDRESSr(pa, nvram_addr));

        /* Setting up slave address*/
        BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_IMC_SL_DEV_ADDf_SET(i2c_config, slv_addr);
        BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_IMC_I2C_SPEEDf_SET(i2c_config, 1);
        BCMI_QUADRA28_I2C_SLAVE_ID_ADDRESSr_IMC_XFER_ADDR_23_16f_SET(i2c_config, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_SLAVE_ID_ADDRESSr(pa, i2c_config));
    }

    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_I2C_CONTROLr(pa, &i2c_control));
    if (BCMI_QUADRA28_I2C_CONTROLr_I2C_STATUSf_GET(i2c_control) != 0) {
        return PHYMOD_E_CONFIG;
    }
    
    BCMI_QUADRA28_I2C_CONTROLr_EXTENDED_R_Wf_SET(i2c_control, 2);
    BCMI_QUADRA28_I2C_CONTROLr_SLAVE_CMDf_SET(i2c_control, ((acc_type == Q28_I2C_WRITE) ? 1 : 0));
    BCMI_QUADRA28_I2C_CONTROLr_I2C_MASTER_ENABLEf_SET(i2c_control, 1);
    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_I2C_CONTROLr(pa, i2c_control));
   
    do {
        PHYMOD_USLEEP(100 * xfer_cnt);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_I2C_CONTROLr(pa, &i2c_control));
    } while((BCMI_QUADRA28_I2C_CONTROLr_I2C_STATUSf_GET(i2c_control) != 1) && (--tries > 0));

    if (BCMI_QUADRA28_I2C_CONTROLr_I2C_STATUSf_GET(i2c_control) != 1) {
        return PHYMOD_E_TIMEOUT;
    }
    if (acc_type == Q28_I2C_READ) {
        /* Write Data in to RAM location*/
        for (cnt_index = 0; cnt_index < temp_cnt; cnt_index ++) {
            PHYMOD_IF_ERR_RETURN(
                phymod_raw_iblk_read(pa, (QUADRA28_I2C_RAM_START_ADDRESS + cnt_index),&ram_data));
            *data = (ram_data & 0xff);
            data ++;
        }
    }
    return PHYMOD_E_NONE;
}

int _quadra28_edc_config_set(const phymod_phy_access_t* phy, const phymod_edc_config_t* edc_config)
{
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    SIDE_SELECTIONr_t side_sel_reg_val;
    uint16_t retry_cnt =0;
    int speed = 0;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    uint8_t datapath = 0;
    phymod_access_t pa = phy->access;
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0 , sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));	
    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0 , sizeof(GENERAL_PURPOSE_3r_t));	
    PHYMOD_MEMCPY(&acc_cpy, &pa, sizeof(phymod_access_t));
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_GET_DATAPATH(&acc_cpy, datapath);
    PHYMOD_IF_ERR_RETURN(READ_APPS_MODE_0r(&acc_cpy, &apps_mode0_reg_val));
    
    if (speed >= Q28_SPEED_40G) {
        PHYMOD_IF_ERR_RETURN (
                quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
    }
    if (edc_config->method != phymodEdcConfigMethodNone) {
        apps_mode0_reg_val.v[0] &= 0xf ;
        apps_mode0_reg_val.v[0] |= 0xC830;
        if (datapath == phymodDatapathUll) {
            BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val,1);
        }
    } else {
        apps_mode0_reg_val.v[0] &= 0xf ;
        BCMI_QUADRA28_APPS_MODE_0r_SYTEM_LRf_SET(apps_mode0_reg_val,1);
        BCMI_QUADRA28_APPS_MODE_0r_SYSTEM_CUf_SET(apps_mode0_reg_val,1);
        if (datapath == phymodDatapathUll) {
            BCMI_QUADRA28_APPS_MODE_0r_ENABLE_ULL_DATAPATHf_SET(apps_mode0_reg_val,1);
        }
    }

    if (speed >= Q28_SPEED_40G) {
        PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
        PHYMOD_IF_ERR_RETURN(
                _quadra28_finish_mask_seq(&acc_cpy));
    } else {
        retry_cnt = 20;
        BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
        PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,apps_mode0_reg_val));
        do {
            PHYMOD_IF_ERR_RETURN(
              BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(&acc_cpy,&gen_pur_reg_3r_val));
            PHYMOD_USLEEP(200);
        } while(((gen_pur_reg_3r_val.v[0] & 0x80) != 0) && retry_cnt--);
        if (retry_cnt == 0) {
            return PHYMOD_E_TIMEOUT;
        }
        PHYMOD_IF_ERR_RETURN(
              _quadra28_finish_mask_seq(&acc_cpy));
    }
    
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}

int _quadra28_edc_config_get(const phymod_phy_access_t* phy, phymod_edc_config_t* edc_config)
{
    phymod_access_t acc_cpy;
    GENERAL_PURPOSE_3r_t gen_pur_reg_3r_val;
    uint16_t temp = 0;
    int speed = 0;
    phymod_access_t pa = phy->access;

    PHYMOD_MEMSET(&gen_pur_reg_3r_val, 0 , sizeof(GENERAL_PURPOSE_3r_t));	
    PHYMOD_MEMCPY(&acc_cpy, &pa, sizeof(phymod_access_t));
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    PHYMOD_IF_ERR_RETURN(
              BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(&acc_cpy,&gen_pur_reg_3r_val));
    temp = (BCMI_QUADRA28_GENERAL_PURPOSE_3r_GET(gen_pur_reg_3r_val) & 0x10) >> 4;

    edc_config->method = (temp)? phymodEdcConfigMethodHardware: phymodEdcConfigMethodNone;

    return PHYMOD_E_NONE;
}

int _quadra28_soft_reset(phymod_access_t *pa) 
{
    BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
    BCMI_QUADRA28_MERLIN_MISC_CTRLr_t reset_ack;
    BCMI_QUADRA28_XGXSSTATUSr_t xgs_sts;
    int retry_cnt = 20;

    PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_PMD_CONTROLr(pa, &pmd_ctrl));
    BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_PMD_CONTROLr(pa, pmd_ctrl));

    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_MERLIN_MISC_CTRLr(pa, &reset_ack));
        PHYMOD_USLEEP(1000);
        retry_cnt --;
    } while (((reset_ack.v[0] & 0x8000) != 0x8000) && retry_cnt);
    if (retry_cnt <= 0) {
        return PHYMOD_E_TIMEOUT;
    }
    retry_cnt = 20;
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_XGXSSTATUSr(pa, &xgs_sts));
        PHYMOD_USLEEP(800);
        retry_cnt --;
    }  while (((xgs_sts.v[0] & 0x800) != 0x800) && retry_cnt);
    if (retry_cnt <= 0) {
        return PHYMOD_E_TIMEOUT;
    }
    return PHYMOD_E_NONE;
}

int _quadra28_failover_mode_set(const phymod_phy_access_t* phy, phymod_failover_mode_t failover_mode)
{
    uint32_t speed = 0;
    uint16_t if_side = 0;
    uint32_t fov_mode = 0, read_mode = 0;
    int pmd_mode = 0;
    uint8_t datapath = 0;
    int retry_cnt = 20;
    uint32_t chip_id = 0;
    uint8_t fov_port_phy = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    phymod_access_t acc_cpy = phy->access;
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_t bcast_ctrl;
    BCMI_QUADRA28_APPS_MODE_2r_t aps_mode2;
    BCMI_QUADRA28_APPS_MODE_0r_t aps_mode0;
    uint16_t lane = 0;
    BCMI_QUADRA28_RING_CONFIGURATIONr_t ring_conf;
    
    PHYMOD_MEMSET(&bcast_ctrl, 0, sizeof(bcast_ctrl));
    PHYMOD_MEMSET(&aps_mode2, 0, sizeof(aps_mode2));
    PHYMOD_MEMSET(&aps_mode0, 0, sizeof(aps_mode0));
    PHYMOD_MEMSET(&ring_conf, 0, sizeof(ring_conf));
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_GET_DATAPATH(&acc_cpy, datapath);
    QUADRA28_GET_LOC(phy, if_side);
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    fov_port_phy = (pmd_mode == Q28_SINGLE_PMD) ? 4 : 1;
    chip_id = _quadra28_get_chip_id(&acc_cpy);
    /*40G FOv can be performed only on 2 DIE(82780/8278f)*/
    if (((chip_id != QUADRA28_82780_CHIP_ID) && (chip_id != QUADRA28_8278F_CHIP_ID))
            && (pmd_mode == Q28_SINGLE_PMD)) {
        return PHYMOD_E_UNAVAIL;
    }
    if (datapath == phymodDatapathUll) {
        return PHYMOD_E_CONFIG;
    }
    if (failover_mode == phymodFailovermodeNone) { 
        fov_mode = 0;
    } else if (if_side == Q28_INTF_LINE) {
        if (acc_cpy.addr & fov_port_phy) {
            fov_mode = Q28_FOV_TYPE_N1S0;
        } else {
            fov_mode = Q28_FOV_TYPE_N0S1;
        }
    } else {
        if (acc_cpy.addr & fov_port_phy) {
            fov_mode = Q28_FOV_TYPE_N0S1;
        } else {
            fov_mode = Q28_FOV_TYPE_N1S0;
        }
    }
    if (pmd_mode == Q28_SINGLE_PMD) {
        if ( fov_mode != 0) {
            /* As per programming seq, FOV can be configured only with 10G mode, so Changing mode for DIE 1*/
            acc_cpy.addr = (acc_cpy.addr & ~0x7);
            PHYMOD_IF_ERR_RETURN(_quadra28_clear_mode_config(&acc_cpy));
            /* As per programming seq, FOV can be configured only with 10G mode, so Changing mode for DIE 1*/
            acc_cpy.addr = (acc_cpy.addr & ~0x7) | 4;
            PHYMOD_IF_ERR_RETURN(_quadra28_clear_mode_config(&acc_cpy));

            acc_cpy.addr = (acc_cpy.addr & ~0x7);
            /* Enable multicast for all the 8 lanes*/
            for (lane = 0; lane <= 7; lane ++) {
                acc_cpy.addr &= ~7;
                acc_cpy.addr |= lane;
                PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(&acc_cpy,0x1c8db,lane));
            }
            /* Setting up multicast for octal channel,
             * No need to enable Multicast for ch1*/
            for (lane = 1; lane <= 7; lane ++) {
                acc_cpy.addr &= ~7;
                acc_cpy.addr |= lane;
                if (lane == 0) {
                    continue;
                }

                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_MDIO_BROADCAST_CONTROLr(&acc_cpy, &bcast_ctrl));
                BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MCAST_ENf_SET(bcast_ctrl, 1);
                BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MULTICAST_PHYADf_SET(bcast_ctrl, 0);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_WRITE_MDIO_BROADCAST_CONTROLr(&acc_cpy, bcast_ctrl));
            }
            PHYMOD_USLEEP(100000); /* Delay as per PGM seq*/
            acc_cpy.addr &= (~7);
            PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(&acc_cpy,Q28_TOP_GP_REG0,0));
            BCMI_QUADRA28_APPS_MODE_2r_SET(aps_mode2, 0x8000 );
            PHYMOD_IF_ERR_RETURN(
               BCMI_QUADRA28_WRITE_APPS_MODE_2r(&acc_cpy, aps_mode2));
            
            /*set Write finish mask*/
            PHYMOD_IF_ERR_RETURN(
              BCMI_QUADRA28_READ_APPS_MODE_0r(&acc_cpy, &aps_mode0));
            BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(aps_mode0, 0);
            PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,aps_mode0));
            PHYMOD_IF_ERR_RETURN(
              _quadra28_intf_update_wait_check(&acc_cpy, aps_mode0.v[0], 50000));

            PHYMOD_USLEEP(100000); /* Delay as per PGM seq*/
            /* Clear Write finish mask*/
            aps_mode0.v[0] = 0x8884;   /* As per PGM seq, it works only for SR4*/
            PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,aps_mode0));
            PHYMOD_IF_ERR_RETURN(
              _quadra28_intf_update_wait_check(&acc_cpy, aps_mode0.v[0], 50000));

            /*set Write finish mask*/
            PHYMOD_IF_ERR_RETURN(
              BCMI_QUADRA28_READ_APPS_MODE_0r(&acc_cpy, &aps_mode0));
            BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(aps_mode0, 0);
            PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,aps_mode0));
            PHYMOD_IF_ERR_RETURN(
              _quadra28_intf_update_wait_check(&acc_cpy, aps_mode0.v[0], 50000));

            acc_cpy.addr |= 4; /* Die 2 soft reset*/
            PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));

            acc_cpy.addr &= ~7;
            /* Soft reset for die 1*/
            PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));

            PHYMOD_USLEEP(200000);

            /* Ring reset*/
            acc_cpy.addr &= ~7;
            acc_cpy.addr |= 4;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 1);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            acc_cpy.addr &= ~7;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 1);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            acc_cpy.addr |= 4;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            acc_cpy.addr &= ~7;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            /* Enabling FOV mode*/
            acc_cpy.addr &= ~7;
            PHYMOD_IF_ERR_RETURN (
                    quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
            PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(&acc_cpy,Q28_TOP_GP_REG0,fov_mode));
            do {
                PHYMOD_IF_ERR_RETURN(
                        phymod_raw_iblk_read(&acc_cpy, Q28_TOP_GP_REG1, &read_mode));
                PHYMOD_USLEEP(200);
            } while((retry_cnt--) && (read_mode != fov_mode));
            
            PHYMOD_IF_ERR_RETURN (
                    quadra28_channel_select(&acc_cpy, 0));
            PHYMOD_USLEEP(100000);

            acc_cpy.addr |= 4; /* Die 2 soft reset*/
            PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));

            acc_cpy.addr &= ~7;
            /* Soft reset for die 1*/
            PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));

            PHYMOD_USLEEP(200000);

            /* Ring reset*/
            acc_cpy.addr &= ~7;
            acc_cpy.addr |= 4;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 1);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            acc_cpy.addr &= ~7;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 1);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            acc_cpy.addr |= 4;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));

            acc_cpy.addr &= ~7;
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_RING_CONFIGURATIONr(&acc_cpy, &ring_conf));
            BCMI_QUADRA28_RING_CONFIGURATIONr_RING_SOFTRESETf_SET(ring_conf, 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_RING_CONFIGURATIONr(&acc_cpy, ring_conf));
        } else {
            phymod_phy_access_t local_phy;
            int16_t die;
            PHYMOD_MEMCPY(&local_phy, phy, sizeof(phymod_phy_access_t));
            for (die = 1;die >=0; die --) {
                acc_cpy.addr &= ~7;
                acc_cpy.addr |= die * 4;
                PHYMOD_IF_ERR_RETURN (
                        quadra28_channel_select(&acc_cpy, Q28_ALL_LANE));
                BCMI_QUADRA28_APPS_MODE_2r_SET(aps_mode2, 0);
                PHYMOD_IF_ERR_RETURN(
                   BCMI_QUADRA28_WRITE_APPS_MODE_2r(&acc_cpy, aps_mode2));
                PHYMOD_IF_ERR_RETURN(
                        phymod_raw_iblk_write(&acc_cpy,Q28_TOP_GP_REG0,0));
                aps_mode0.v[0] = 0x8882;   /* As per PGM seq, it works only for SR*/
                PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,aps_mode0));
                PHYMOD_IF_ERR_RETURN(
                    _quadra28_intf_update_wait_check(&acc_cpy, aps_mode0.v[0], 50000));

                /*set Write finish mask*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_QUADRA28_READ_APPS_MODE_0r(&acc_cpy, &aps_mode0));
                BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(aps_mode0, 0);
                PHYMOD_IF_ERR_RETURN(WRITE_APPS_MODE_0r(&acc_cpy,aps_mode0));
                PHYMOD_IF_ERR_RETURN(
                   _quadra28_intf_update_wait_check(&acc_cpy, aps_mode0.v[0], 50000));

                PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));
            }
            /* Move to 40G default mode*/
            local_phy.access.addr &= ~0x7;
            PHYMOD_IF_ERR_RETURN(
                   quadra28_set_config_mode(&local_phy, phymodInterfaceSR4, 40000,
                       0, 0xFFFF, 0xFFFF));
            local_phy.port_loc = phymodPortLocSys; 
            PHYMOD_IF_ERR_RETURN(
                   quadra28_set_config_mode(&local_phy, phymodInterfaceXLAUI, 40000,
                       0, 0xFFFF, 0xFFFF));

            local_phy.port_loc = phymodPortLocLine; 
            local_phy.access.addr |= 0x4;
            PHYMOD_IF_ERR_RETURN(
                   quadra28_set_config_mode(&local_phy, phymodInterfaceSR4, 40000,
                       0, 0xFFFF, 0xFFFF));
            local_phy.port_loc = phymodPortLocSys; 
            PHYMOD_IF_ERR_RETURN(
                   quadra28_set_config_mode(&local_phy, phymodInterfaceXLAUI, 40000,
                       0,0xFFFF, 0xFFFF));
        }
    } else {
        /* Enable FOV for both the channels on a core*/
        acc_cpy.addr &= (~1);
        PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(&acc_cpy,Q28_TOP_GP_REG0,fov_mode));
        do {
            PHYMOD_IF_ERR_RETURN(
                    phymod_raw_iblk_read(&acc_cpy, Q28_TOP_GP_REG1, &read_mode));
            PHYMOD_USLEEP(200);
        } while((retry_cnt--) && (read_mode != fov_mode));

        PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));
        retry_cnt = 20;
        acc_cpy.addr |= 1;
        PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(&acc_cpy,Q28_TOP_GP_REG0,fov_mode));
        do {
            PHYMOD_IF_ERR_RETURN(
                    phymod_raw_iblk_read(&acc_cpy, Q28_TOP_GP_REG1, &read_mode));
            PHYMOD_USLEEP(200);
        } while((retry_cnt--) && (read_mode != fov_mode));
        PHYMOD_IF_ERR_RETURN(_quadra28_soft_reset(&acc_cpy));
    }
    
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);
    return PHYMOD_E_NONE;
}

int _quadra28_failover_mode_get(const phymod_phy_access_t* phy, phymod_failover_mode_t* failover_mode)
{
    uint32_t speed = 0;
    uint32_t fov_mode = 0;
    uint8_t datapath = 0;
    phymod_access_t acc_cpy = phy->access;

    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);
    Q28_GET_DATAPATH(&acc_cpy, datapath);

    if (datapath == phymodDatapathUll) {
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN(
         phymod_raw_iblk_read(&acc_cpy, Q28_TOP_GP_REG1, &fov_mode));
    if (fov_mode == 0) {
        *failover_mode = phymodFailovermodeNone; 
    } else {
        *failover_mode = phymodFailovermodeEnable;
    }

    return PHYMOD_E_NONE;
}

int _quadra28_phy_rx_adaptation_resume(const phymod_phy_access_t *phy)
{
    int lane_map = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_index = 0;
    SIDE_SELECTIONr_t side_sel_reg_val;
    int pmd_mode = 0;
    uint32_t speed = 0;
    phymod_access_t acc_cpy;
   
    PHYMOD_MEMSET(&side_sel_reg_val, 0 , sizeof(SIDE_SELECTIONr_t));	
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));
    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);
    QUADRA28_GET_LOC(phy, pkg_side);   
    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val, pkg_side);   

    if (pmd_mode == Q28_SINGLE_PMD) {
        for (lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                PHYMOD_IF_ERR_RETURN(
                    quadra28_channel_select(&acc_cpy,
                        ((lane_map == Q28_ALL_LANE)? Q28_ALL_LANE : lane_index)));

                PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 0));
                if (lane_map == Q28_ALL_LANE) {
                    break;
                }
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_quadra28_stop_rx_adaptation(&acc_cpy, 0));
    }

    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val, Q28_INTF_LINE);

    return PHYMOD_E_NONE;
}

int _quadra28_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t gpio_config_ctrl_1;
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t gpio_config_ctrl_2;
    phymod_access_t acc_cpy;
    uint16_t temp = 0;
    PHYMOD_MEMSET(&gpio_config_ctrl_1, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t));
    PHYMOD_MEMSET(&gpio_config_ctrl_2, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t));
    PHYMOD_MEMSET(&acc_cpy, 0, sizeof(phymod_access_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    if (pin_no > 5) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Q28 has only 5 GPIOs (0 - 5)")));
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROLr(&acc_cpy, &gpio_config_ctrl_1));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROL_2r(&acc_cpy, &gpio_config_ctrl_2));

    switch (gpio_mode) {
      case phymodGpioModeDisabled:
          return PHYMOD_E_NONE;
      case phymodGpioModeOutput:
          if (pin_no < 2) {
              temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DISf_GET(gpio_config_ctrl_1);
              temp &= ~(1 << pin_no);
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DISf_SET(gpio_config_ctrl_1,temp);
          } else if (pin_no == 2 || pin_no == 3) {
              temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DISf_GET(gpio_config_ctrl_2);
              temp &= ~(1 << (pin_no - 2));
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DISf_SET(gpio_config_ctrl_2, temp);
          }  else if (pin_no == 4) {
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_OPRXLOS_DISf_SET(gpio_config_ctrl_1, 0);
          } else if (pin_no == 5) {
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_MOD_ABS_DISf_SET(gpio_config_ctrl_1, 0);
          }

      break;     
      case phymodGpioModeInput:
          if (pin_no < 2) {
              temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DISf_GET(gpio_config_ctrl_1);
              temp &= ~(1 << pin_no);
              temp |= (1 << pin_no);
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DISf_SET(gpio_config_ctrl_1,temp);
          } else if (pin_no == 2 || pin_no == 3) {
              temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DISf_GET(gpio_config_ctrl_2);
              temp &= ~(1 << (pin_no - 2));
              temp |= (1 << (pin_no - 2));
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DISf_SET(gpio_config_ctrl_2, temp);
          }  else if (pin_no == 4) {
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_OPRXLOS_DISf_SET(gpio_config_ctrl_1, 1);
          } else if (pin_no == 5) {
              BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_MOD_ABS_DISf_SET(gpio_config_ctrl_1, 1);
          }
      break;    
      default:
          return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_PURPOSE_IO_CONTROLr(&acc_cpy, gpio_config_ctrl_1));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_PURPOSE_IO_CONTROL_2r(&acc_cpy, gpio_config_ctrl_2));


    return PHYMOD_E_NONE;
}

int _quadra28_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    phymod_access_t acc_cpy;
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t gpio_config_ctrl_1;
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t gpio_config_ctrl_2;
    uint16_t temp = 0;
    PHYMOD_MEMSET(&gpio_config_ctrl_1, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t));
    PHYMOD_MEMSET(&gpio_config_ctrl_2, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t));
    PHYMOD_MEMSET(&acc_cpy, 0, sizeof(phymod_access_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    if (pin_no > 5) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Q28 has only 5 GPIOs (0 - 5)")));
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROLr(&acc_cpy, &gpio_config_ctrl_1));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROL_2r(&acc_cpy, &gpio_config_ctrl_2));

    if (pin_no < 2) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DISf_GET(gpio_config_ctrl_1);
        temp = (pin_no == 0) ? (temp & 1) : ((temp & 2) >> 1);
    } else if (pin_no == 2 || pin_no == 3) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DISf_GET(gpio_config_ctrl_2);
        temp = (pin_no == 2) ? (temp & 1) : ((temp & 2) >> 1);
    }  else if (pin_no == 4) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_OPRXLOS_DISf_GET(gpio_config_ctrl_1);
    } else if (pin_no == 5) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_MOD_ABS_DISf_GET(gpio_config_ctrl_1);
    }
    if (temp == 1) {
        *gpio_mode = phymodGpioModeInput;
    } else {
        *gpio_mode = phymodGpioModeOutput;
    }

    return PHYMOD_E_NONE;
}

int _quadra28_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t gpio_config_ctrl_1;
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t gpio_config_ctrl_2;
    phymod_access_t acc_cpy;
    uint16_t temp = 0;
    PHYMOD_MEMSET(&gpio_config_ctrl_1, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t));
    PHYMOD_MEMSET(&gpio_config_ctrl_2, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t));
    PHYMOD_MEMSET(&acc_cpy, 0, sizeof(phymod_access_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    if (pin_no > 5) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Q28 has only 5 GPIOs (0 - 5)")));
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROLr(&acc_cpy, &gpio_config_ctrl_1));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROL_2r(&acc_cpy, &gpio_config_ctrl_2));

    if (pin_no < 2) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DATf_GET(gpio_config_ctrl_1);
        temp &= ~(1 << pin_no);
        temp |= (value << pin_no);
        BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OUT_GP_DATf_SET(gpio_config_ctrl_1,temp);
    } else if (pin_no == 2 || pin_no == 3) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DATf_GET(gpio_config_ctrl_2);
        temp &= ~(1 << (pin_no-2));
        temp |= (value << (pin_no - 2));
        BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_OUT_GP_DATf_SET(gpio_config_ctrl_2, temp);
    }  else if (pin_no == 4) {
        BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_OPT_RX_LOS_OUTf_SET(gpio_config_ctrl_1, value);
    } else if (pin_no == 5) {
        BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_MOD_ABS_OUTf_SET(gpio_config_ctrl_1, value);
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_PURPOSE_IO_CONTROLr(&acc_cpy, gpio_config_ctrl_1));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_PURPOSE_IO_CONTROL_2r(&acc_cpy, gpio_config_ctrl_2));

    return PHYMOD_E_NONE;
}

int _quadra28_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t gpio_config_ctrl_1;
    BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t gpio_config_ctrl_2;
    phymod_access_t acc_cpy;
    uint16_t temp = 0;
    PHYMOD_MEMSET(&gpio_config_ctrl_1, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_t));
    PHYMOD_MEMSET(&gpio_config_ctrl_2, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_t));
    PHYMOD_MEMSET(&acc_cpy, 0, sizeof(phymod_access_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    if (pin_no > 5) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Q28 has only 5 GPIOs (0 - 5)")));
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROLr(&acc_cpy, &gpio_config_ctrl_1));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_IO_CONTROL_2r(&acc_cpy, &gpio_config_ctrl_2));

    if (pin_no < 2) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_IN_GP_DATf_GET(gpio_config_ctrl_1);
        *value = (pin_no == 0)? (temp & 1) : ((temp & 2) >> 1);
    } else if (pin_no == 2 || pin_no == 3) {
        temp = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROL_2r_IN_GP_DATf_GET(gpio_config_ctrl_2);
        *value = (pin_no == 2)? (temp & 1) : ((temp & 2) >> 1);
    }  else if (pin_no == 4) {
        *value = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_IN_OPRXLOS_DATf_GET(gpio_config_ctrl_1);
    } else if (pin_no == 5) {
        *value = BCMI_QUADRA28_GENERAL_PURPOSE_IO_CONTROLr_IN_MOD_ABS_DATf_GET(gpio_config_ctrl_1);
    }

    return PHYMOD_E_NONE;
}

