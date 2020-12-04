/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wc40_config.c
 * Purpose:     Perform configuration for Warpcore device.
 */

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_XGXS_WC40)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phyctrl.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>


#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "serdesid.h"
#include "wc40.h" 
#include "wc40_extra.h"

/*
 * Function:
 *      phy_wc40_config_init
 * Purpose:
 *      Obtain switch chip and board specific configuration information for the Warpcore device to
 *      allow driver to put the device in the right operation mode. This function should be
 *      called first in the driver's initialization sequence.   
 * Parameters:
 *      pc - device descriptor.
 * Returns:
 *      SOC_E_NONE
 */

int
phy_wc40_config_init(phy_ctrl_t *pc)
{
    WC40_DEV_CFG_t *pCfg;
    WC40_DEV_INFO_t  *pInfo;
    WC40_DEV_DESC_t *pDesc;
    soc_phy_info_t *pi;
    int len;
    uint16 serdes_id0;
    int unit;
    int port;
    int i;
    WC40_TX_DRIVE_t *p_tx;

    pDesc = (WC40_DEV_DESC_t *)(pc + 1);
    pCfg = &pDesc->cfg;
    pInfo = &pDesc->info;
    unit = pc->unit;
    port = pc->port;


    if (SOC_IS_HELIX4(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */

        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

        /* convert to phy_ctrl macros */
        if (si->port_num_lanes[port] == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
        } else if (si->port_num_lanes[port] == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        }

        pc->lane_num = (phy_port-1) % 4;
        pc->chip_num = (phy_port-1) / 4;
    }


#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */

        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

        /* convert to phy_ctrl macros */
        if (si->port_num_lanes[port] == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
        } else if (si->port_num_lanes[port] == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        }
        /* warpcore intstance number and lane number */
        pc->lane_num = (phy_port - 1) % 4;
        pc->chip_num = (phy_port - 1) / 4;
    }
#endif
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */

        si = &SOC_INFO(unit);
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = si->port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

        /* convert to phy_ctrl macros */
        if (si->port_num_lanes[port] == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
        } else if (si->port_num_lanes[port] == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        }

        if (phy_port > 8) {
            if ((soc_property_get(unit, spn_BCM88732_2X40_1X40, 0)) ||
                (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0)) ||
                (soc_property_get(unit, spn_BCM88732_8X10_1X40, 0)) ||
                (soc_property_get(unit, spn_BCM88732_8X10_2X40, 0))) {
                pc->phy_mode = PHYCTRL_LANE_MODE_CUSTOM;
                pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
           } else if (soc_property_get(unit,
                                 spn_BCM88732_2X40_2X40,0)) {
                pc->phy_mode = PHYCTRL_LANE_MODE_CUSTOM;
                pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
           } else if ((soc_property_get(unit,spn_BCM88732_1X40_4X10,0)) ||
                      (soc_property_get(unit, spn_BCM88732_4X10_4X10, 0))) {
                pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
           } else if ((soc_property_get(unit,spn_BCM88732_2X40_8X12,0)) ||
                (soc_property_get(unit, spn_BCM88732_1X40_4X10_8X12, 0)) ||
                (soc_property_get(unit, spn_BCM88732_4X10_1X40_8X12, 0)) ||
                (soc_property_get(unit, spn_BCM88732_8X10_8X12, 0)) ||
                (soc_property_get(unit, spn_BCM88732_8X10_4X12, 0)) ||
                (soc_property_get(unit, spn_BCM88732_6X10_2X12, 0)) ||
                (soc_property_get(unit, spn_BCM88732_8X10_2X12, 0))) {
                pc->phy_mode = PHYCTRL_LANE_MODE_CUSTOM1;
           } else if (soc_property_get(unit, spn_BCM88732_2X40_8X10, 0) || 
                      soc_property_get(unit, spn_BCM88732_8X10_8X10, 0))  {
                pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
                pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
           } else {
             LOG_WARN(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "Board configuration not found: u=%d p=%d\n"), unit,port));
           }
        } else {
            /* convert to phy_ctrl macros */
            if (si->port_num_lanes[port] == 4) {
                pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            } else if (si->port_num_lanes[port] == 2) {
                pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
                pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            } else if (si->port_num_lanes[port] == 1) {
                pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
                pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            }
        }

        if (phy_port > 8) {
            if (soc_property_get(unit, "IL3125", 0)) {
                pc->phy_mode |= PHYCTRL_LANE_MODE_CUSTOM_3p125MHZ;
            }
        }
    }
#endif

    /* retrieve chip level information for warpcore */

    pi       = &SOC_PHY_INFO(unit, port);

    serdes_id0 = pInfo->serdes_id0;
    /* 
     * COVERITY
     * 
     * Buffer overflow will not happen as destination string is of length 30.
     */
    /* coverity[secure_coding] */

    sal_strcpy(pInfo->name,"WC-");
    len = sal_strlen(pInfo->name);
    /* add rev letter */
    pInfo->name[len++] = 'A' + ((serdes_id0 >>
                 SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);

    /* add rev number */
    pInfo->name[len++] = '0' + ((serdes_id0 >>
                          SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
    pInfo->name[len++] = '/';
    pInfo->name[len++] = (pc->chip_num/10)%10 + '0';
    pInfo->name[len++] = pc->chip_num%10 + '0';
    pInfo->name[len++] = '/';

    /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
     *           1 dual port mode, port uses 2 lanes
     *           2 quad port mode, port uses 1 lane
     */
    if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
        if (pc->lane_num < 2) { /* the first dual-xgxs port */
            pInfo->name[len++] = '0';
            pInfo->name[len++] = '-';
            pInfo->name[len++] = '1';
        } else {
            pInfo->name[len++] = '2';
            pInfo->name[len++] = '-';
            pInfo->name[len++] = '3';
        }
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
    } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || CUSTOM_MODE(pc) ||
               pc->phy_mode == PHYCTRL_SINGLE_LANE_IN_DUAL_PORT_MODE) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        pInfo->name[len++] = pc->lane_num + '0';
    } else {
        pInfo->name[len++] = '4';
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
    }
    pInfo->name[len] = 0;  /* string terminator */

    if (len > WC40_LANE_NAME_LEN) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "WC info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                   len,WC40_LANE_NAME_LEN,unit, port));
        return SOC_E_MEMORY;
    }

    if (!PHY_EXTERNAL_MODE(unit, port)) {
        pi->phy_name = pInfo->name;
    }

    if (CUSTOM_MODE(pc)) {
        pCfg->custom = pc->phy_mode;
    }
    if (CUSTOM1_MODE(pc)) {
        pCfg->custom1 = pc->phy_mode;
    }

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        if (PHY_EXTERNAL_MODE(unit, port)) {
            pCfg->line_intf =  WC40_IF_XFI;
        } else {
            pCfg->line_intf =  WC40_IF_SFI;
        }
        pCfg->auto_medium = FALSE;
        if (CUSTOM_MODE(pc) || CUSTOM1_MODE(pc)) {
            pCfg->lane_mode = xgxs_operationModes_IndLane_OS5;
        } else {
            pCfg->lane_mode = xgxs_operationModes_Indlane_OS8;
        }
    } else {
        pCfg->line_intf =  WC40_IF_XLAUI;
        pCfg->auto_medium = TRUE;
        pCfg->lane_mode  = xgxs_operationModes_ComboCoreMode;
        pc->lane_num = 0;  /* make sure lane_num is 0 */
    }

    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        (PHY_PASSTHRU_MODE(unit, port)) ||
        (!PHY_INDEPENDENT_LANE_MODE(unit, port)) ||
         (pc->speed_max >= 10000)) {
        pCfg->fiber_pref = TRUE;
    } else {
        pCfg->fiber_pref = FALSE;
    }
    pCfg->fw_dfe = SOC_PHY_DFE_AUTO;

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        pCfg->cl73an     = WC40_CL73_AND_CL73BAM; /*WC40_CL73_WO_CL73BAM; */
    } else {
       if (IS_HG_PORT(unit, port)) {
           pCfg->cl73an     = FALSE;
       } else {
           pCfg->cl73an     = WC40_CL73_AND_CL73BAM;
       }
    }
   
    /* Default setting for CL37 */ 
    pCfg->cl37an     = WC40_CL37_AND_CL37BAM; 

    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        pCfg->pdetect1000x = TRUE;
    } else {
        pCfg->pdetect1000x = FALSE;
    }

    /* populate the default TX drive value for each speed mode */
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT(unit)) {

        /* XFI  */
        p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
        p_tx->u.tap.post  = 0x08;
        p_tx->u.tap.main  = 0x37;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x03;

        /* SFI SR */ 
        p_tx = &pCfg->tx_drive[TXDRV_SFI_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;
 
        /* SFI DAC */ 
        p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;
 
        /* SR4 */ 
        p_tx = &pCfg->tx_drive[TXDRV_SR4_INX];
        p_tx->u.tap.post  = 0x13;
        p_tx->u.tap.main  = 0x2c;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x05;
        p_tx->ipredrive = 0x05;
 
        /* XLAUI */ 
        p_tx = &pCfg->tx_drive[TXDRV_XLAUI_INX];
        p_tx->u.tap.post  = 0x18;
        p_tx->u.tap.main  = 0x27;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x04;
        p_tx->ipredrive = 0x04;
 
        /* TXDRV_6GOS1_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS1_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
 
        /* TXDRV_6GOS2_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
 
        /* TXDRV_6GOS2_CX4_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_CX4_INX];
        p_tx->u.tap.post  = 0x0e;
        p_tx->u.tap.main  = 0x31;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x08;
        p_tx->ipredrive = 0x08;
 
        /* Autoneg */ 
        p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* default */
        p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
    }
#endif
    /* populate the default TX drive value for each speed mode */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_HELIX4(unit)) {

        /* XFI  */
        p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
        p_tx->u.tap.post  = 0x08;
        p_tx->u.tap.main  = 0x37;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x03;

        /* SFI SR */ 
        p_tx = &pCfg->tx_drive[TXDRV_SFI_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;
 
        /* SFI DAC */ 
        p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;
 
        /* SR4 */ 
        p_tx = &pCfg->tx_drive[TXDRV_SR4_INX];
        p_tx->u.tap.post  = 0x13;
        p_tx->u.tap.main  = 0x2c;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x05;
        p_tx->ipredrive = 0x05;
 
        /* XLAUI */ 
        p_tx = &pCfg->tx_drive[TXDRV_XLAUI_INX];
        p_tx->u.tap.post  = 0x18;
        p_tx->u.tap.main  = 0x27;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x04;
        p_tx->ipredrive = 0x04;
 
        /* TXDRV_6GOS1_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS1_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
 
        /* TXDRV_6GOS2_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
 
        /* TXDRV_6GOS2_CX4_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_CX4_INX];
        p_tx->u.tap.post  = 0x0e;
        p_tx->u.tap.main  = 0x31;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x08;
        p_tx->ipredrive = 0x08;
 
        /* Autoneg */ 
        p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* default */
        p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
    }
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {

        /* XFI  */
        p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
        p_tx->u.tap.post  = 0x08;
        p_tx->u.tap.main  = 0x37;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x03;

        /* SFI SR */ 
        p_tx = &pCfg->tx_drive[TXDRV_SFI_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;
 
        /* SFI DAC */ 
        p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;
 
        /* SR4 */ 
        p_tx = &pCfg->tx_drive[TXDRV_SR4_INX];
        p_tx->u.tap.post  = 0x13;
        p_tx->u.tap.main  = 0x2c;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x05;
        p_tx->ipredrive = 0x05;
 
        /* XLAUI */ 
        p_tx = &pCfg->tx_drive[TXDRV_XLAUI_INX];
        p_tx->u.tap.post  = 0x18;
        p_tx->u.tap.main  = 0x27;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x04;
        p_tx->ipredrive = 0x04;
 
        /* TXDRV_6GOS1_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS1_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
 
        /* TXDRV_6GOS2_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
 
        /* TXDRV_6GOS2_CX4_INX */ 
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_CX4_INX];
        p_tx->u.tap.post  = 0x0e;
        p_tx->u.tap.main  = 0x31;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x08;
        p_tx->ipredrive = 0x08;
 
        /* Autoneg */ 
        p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* default */
        p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
    }
#endif

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {

        /* XFI  */
        p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
        p_tx->u.tap.post  = 0x06;
        p_tx->u.tap.main  = 0x39;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x01;
        p_tx->ipredrive = 0x03;

        /* SFI SR */
        p_tx = &pCfg->tx_drive[TXDRV_SFI_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* SFI DAC */
        p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* SR4 */
        p_tx = &pCfg->tx_drive[TXDRV_SR4_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* XLAUI */
        p_tx = &pCfg->tx_drive[TXDRV_XLAUI_INX];
        p_tx->u.tap.post  = 0x18;
        p_tx->u.tap.main  = 0x27;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x01;
        p_tx->ipredrive = 0x03;

        /* TXDRV_6GOS1_INX */
        p_tx = &pCfg->tx_drive[TXDRV_6GOS1_INX];
        p_tx->u.tap.post  = 0x08;
        p_tx->u.tap.main  = 0x37;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x00;
        p_tx->ipredrive = 0x01;

        /* TXDRV_6GOS2_INX */
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;

        /* TXDRV_6GOS2_CX4_INX */
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_CX4_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* Autoneg */
        p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* default */
        p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
    }
#endif

    /* config property overrides */
    for (i = 0; i < NUM_LANES; i++) {
        pCfg->preemph[i] = soc_property_port_get(unit, port,
                                    spn_SERDES_PREEMPHASIS, pCfg->preemph[i]);
        pCfg->idriver[i] = soc_property_port_get(unit, port,
                                    spn_SERDES_DRIVER_CURRENT, pCfg->idriver[i]);
        pCfg->pdriver[i] = soc_property_port_get(unit, port,
                                    spn_SERDES_PRE_DRIVER_CURRENT, pCfg->pdriver[i]);
        pCfg->post2driver[i] = soc_property_port_get(unit, port,
                                    spn_SERDES_POST2_DRIVER_CURRENT,pCfg->post2driver[i]);
    }
    pCfg->auto_medium = soc_property_port_get(unit, port,
                              spn_SERDES_AUTOMEDIUM, pCfg->auto_medium);
    pCfg->fiber_pref = soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, pCfg->fiber_pref);
    pCfg->sgmii_mstr = soc_property_port_get(unit, port,
                              spn_SERDES_SGMII_MASTER, pCfg->sgmii_mstr);
    pCfg->pdetect10g = soc_property_port_get(unit, port,
                              spn_XGXS_PDETECT_10G, pCfg->pdetect10g);
    pCfg->cx42hg = soc_property_port_get(unit, port,
                              spn_CX4_TO_HIGIG, pCfg->cx42hg);
    pCfg->rxlane_map = soc_property_port_get(unit, port,
                                        spn_XGXS_RX_LANE_MAP, pCfg->rxlane_map);
    pCfg->txlane_map = soc_property_port_get(unit, port,
                                        spn_XGXS_TX_LANE_MAP, pCfg->txlane_map);
    pCfg->cl73an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C73, pCfg->cl73an);
    pCfg->cl37an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C37, pCfg->cl37an);
    pCfg->rxaui = soc_property_port_get(unit, port,
                                    spn_SERDES_2WIRE_XAUI, FALSE);
    pCfg->txpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_TX_POLARITY_FLIP, pCfg->txpol);
    pCfg->rxpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_RX_POLARITY_FLIP, pCfg->rxpol);
    pCfg->rx_los = soc_property_port_get(unit, port,
                            spn_SERDES_RX_LOS, pCfg->rx_los);
    pCfg->rx_los_invert = soc_property_port_get(unit, port,
                            spn_SERDES_RX_LOS_INVERT, pCfg->rx_los_invert);
    if (PHY_EXTERNAL_MODE(unit, port)) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
        pCfg->phy_passthru = soc_property_port_get(unit, port,
                            spn_PHY_PCS_REPEATER, pCfg->phy_passthru);
        if (pCfg->phy_passthru) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_PASSTHRU);
        }
    }
    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        pCfg->lane_mode = soc_property_port_get(unit, port,
                          spn_PHY_HL65_1LANE_MODE, pCfg->lane_mode);
    }
    pCfg->cx4_10g = soc_property_port_get(unit, port,
                          spn_10G_IS_CX4, pCfg->cx4_10g);
    pCfg->lane0_rst = soc_property_get(unit,
                             spn_SERDES_LANE0_RESET, pCfg->lane0_rst);
    /* XXX internal use for now */
    pCfg->refclk = soc_property_port_get(unit,port,
                             "wc_refclk_speed", pCfg->refclk);

    pCfg->hg_mode = IS_HG_PORT(unit,port);
    if (PHY_EXTERNAL_MODE(unit, port)) {
        /* default: always IEEE speed mode if connected with an external PHY */
        pCfg->hg_mode = FALSE;

        /* exception: combo mode && passthru */
       if ((!PHY_INDEPENDENT_LANE_MODE(unit, port)) && pCfg->phy_passthru) {
           pCfg->hg_mode = IS_HG_PORT(unit,port);
       }
    }

    if (!PHY_EXTERNAL_MODE(unit, port)) {

        if (pCfg->fiber_pref) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }

        if (pCfg->cl73an) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
        }
    }

    /* For HG mode or BAM mode, cl37 BAM should be enabled
       WARN if not 
     */
    if((pCfg->cl37an != WC40_CL37_AND_CL37BAM) && 
          ((pCfg->hg_mode) || (pCfg->cl73an == WC40_CL73_AND_CL73BAM))) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC config init, CL37 BAM is not enabled with HG/BAM mode: u=%d p=%d\n"), unit,port));

    }


    return SOC_E_NONE;
}

#else /* INCLUDE_XGXS_WC40 */
typedef int _xgxs_wc40_config_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_WC40 */

