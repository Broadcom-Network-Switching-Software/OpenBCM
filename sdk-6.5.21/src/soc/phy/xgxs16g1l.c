/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs16g.c
 * Purpose:     Broadcom 1000 Mbps SerDes 
 *              (XGXS-16G1L)
 *
 *  supported speed modes in independent channel mode for these three devices.
 *
 *                XGXS_16G          XGXS_13G                    Quad SGMII
 *   CL37         lane0 only        lane0 only                  lane0 only
 *   CL37 BAM     no                no                          no
 *   CL73         lane0 only        all four lanes              all four lanes
 *   CL73 BAM     no                no                          no
 *   Forced 2.5G  yes               yes                         yes
 *   Forced 1G    yes               yes                         yes
 *   100FX        no                yes                         yes
 *   SGMII        yes               yes                         yes
 *
 * Each channel has to run the same rate. 
 * Invalid combination: 
 *    lane0 1G and lane1 2.5G, lane2 SGMII and lane1 2.5G
 * Valid combination:
 *    lane0 1G and lane1 SGMII, lane0 autoneg 1G and lane2 forced 1G.
 */

#include "phydefs.h"      /* Must include before other phy related includes */ 
#if defined(INCLUDE_XGXS_16G)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include <sal/types.h>

#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>


#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "xgxs.h"
#include "serdesid.h"
#include "xgxs16g.h"

#define PHY_XGXS16G_LANEPRBS_LANE_SHIFT   4


#define XGXS16G_PLL_WAIT  250000

STATIC int phy_xgxs16g1l_interface_get(int unit, soc_port_t port, 
                                       soc_port_if_t *pif);
STATIC int phy_xgxs16g1l_master_get(int unit, soc_port_t port, int *master);
STATIC int phy_xgxs16g1l_an_set(int unit, soc_port_t port, int an);
STATIC int phy_xgxs16g1l_an_get(int unit, soc_port_t port,
                              int *an, int *an_done);
STATIC int phy_xgxs16g1l_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_xgxs16g1l_ability_advert_set(int unit, soc_port_t port, 
                                         soc_port_ability_t *ability);
STATIC int phy_xgxs16g1l_ability_advert_get(int unit, soc_port_t port,
                                         soc_port_ability_t *ability);
STATIC int phy_xgxs16g1l_ability_local_get(int unit, soc_port_t port,
                                   soc_port_ability_t *ability); 
STATIC int phy_xgxs16g1l_medium_config_set(int unit, soc_port_t port,
                                   soc_port_medium_t medium,  soc_phy_config_t *cfg);
STATIC int phy_xgxs16g1l_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium);

/* Notify event forward declaration */
STATIC int 
_phy_xgxs16g1l_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int 
_phy_xgxs16g1l_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int 
_phy_xgxs16g1l_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int 
_phy_xgxs16g1l_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int 
_phy_xgxs16g1l_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int
_phy_xgxs16g1l_control_vco_disturbed_set(int unit, soc_port_t port);
STATIC int
_phy_xgxs16g1l_control_vco_disturbed_get(int unit, soc_port_t port, uint32 *value);

STATIC int
_phy_xgxs16g1l_pll_lock_wait(int unit, soc_port_t port)
{
    uint16           data16;
    phy_ctrl_t      *pc;
    soc_timeout_t    to;
    int              rv;

    pc = INT_PHY_SW_STATE(unit, port);
    _phy_xgxs16g1l_control_vco_disturbed_set(unit, port);
    soc_timeout_init(&to, XGXS16G_PLL_WAIT, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_XGXS16G_XGXSBLK0_XGXSSTATUSr(unit, pc, &data16);
        if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "XGXS_16G : TXPLL did not lock: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_xgxs16g1l_lane_swap(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int i;
    uint16 lane_map;
    uint16 lane, hw_map, chk_map;

    pc = INT_PHY_SW_STATE(unit, port);

    /* Update RX lane map */

    if (soc_property_port_get_str(unit, port, spn_XGXS_RX_LANE_MAP)) {
        lane_map = soc_property_port_get(unit, port,
                                    spn_XGXS_RX_LANE_MAP, 0x0123) & 0xffff;
        hw_map  = 0;
        chk_map = 0;

        /* contruct lane swap map in register format */
        for (hw_map=0, i = 3; i >= 0; i--) {
            lane  = (lane_map >> (i * 4)) & 0xf;
            hw_map  |= lane << ((3 - i) * 2);
            chk_map |= 1 << lane;
        }
        
        /* check if valid configuration */
        if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
                   (MODIFY_XGXS16G_XGXSBLK8_RXLNSWAP1r(unit, pc,
                       hw_map,
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                       XGXSBLK8_RXLNSWAP1_RX0_LNSWAP_SEL_MASK |
                       XGXSBLK8_RXLNSWAP1_RX1_LNSWAP_SEL_MASK |
                       XGXSBLK8_RXLNSWAP1_RX2_LNSWAP_SEL_MASK |
                       XGXSBLK8_RXLNSWAP1_RX3_LNSWAP_SEL_MASK));
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "unit %d port %s: Invalid RX lane map 0x%04x.\n"),
                      unit, SOC_PORT_NAME(unit, port), lane_map));
        }
    }

    /* Update TX lane map */
    if (soc_property_port_get_str(unit, port, spn_XGXS_TX_LANE_MAP)) {
        lane_map = soc_property_port_get(unit, port,
                                        spn_XGXS_TX_LANE_MAP, 0x0123) & 0xffff;
        hw_map  = 0;
        chk_map = 0;

        /* contruct lane swap map in register format */
        for (hw_map=0, i = 3; i >= 0; i--) {
            lane  = (lane_map >> (i * 4)) & 0xf;
            hw_map  |= lane << ((3 - i) * 2);
            chk_map |= 1 << lane;
        }
        if (chk_map == 0xf) {
            SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
               (MODIFY_XGXS16G_XGXSBLK8_TXLNSWAP1r(unit, pc,
                    hw_map,
                    XGXSBLK8_TXLNSWAP1_TX3_LNSWAP_SEL_MASK |
                    XGXSBLK8_TXLNSWAP1_TX2_LNSWAP_SEL_MASK |
                    XGXSBLK8_TXLNSWAP1_TX1_LNSWAP_SEL_MASK |
                    XGXSBLK8_TXLNSWAP1_TX0_LNSWAP_SEL_MASK));
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "unit %d port %s: Invalid TX lane map 0x%04x.\n"),
                      unit, SOC_PORT_NAME(unit, port), lane_map));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_polarity_flip(int unit, soc_port_t port)
{
    uint16      data16;
    uint16      mask16;
    phy_ctrl_t *pc;
    uint32      phy_xaui_polarity_flip;

    pc = INT_PHY_SW_STATE(unit, port);

    if (soc_property_port_get_str(unit, port, spn_PHY_XAUI_TX_POLARITY_FLIP)) {
        phy_xaui_polarity_flip = soc_property_port_get(unit, port,
                                              spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
        /* Flip TX polarity */
        mask16 = TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
        data16 = phy_xaui_polarity_flip? mask16:0;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "unit %d port %s: TX Polarity swap 0x%04x.\n"),
                  unit, SOC_PORT_NAME(unit, port), data16));
        if (pc->lane_num == 0) {
            SOC_IF_ERROR_RETURN
              (MODIFY_XGXS16G_TX0_TX_ACONTROL0r(unit, pc, data16, mask16));
        } else if (pc->lane_num == 1) {
            SOC_IF_ERROR_RETURN
              (MODIFY_XGXS16G_TX1_TX_ACONTROL0r(unit, pc, data16, mask16));
        } else if (pc->lane_num == 2) {
            SOC_IF_ERROR_RETURN
              (MODIFY_XGXS16G_TX2_TX_ACONTROL0r(unit, pc, data16, mask16));
        } else {
            SOC_IF_ERROR_RETURN
              (MODIFY_XGXS16G_TX3_TX_ACONTROL0r(unit, pc, data16, mask16));
        }
    }

    if (soc_property_port_get_str(unit, port, spn_PHY_XAUI_RX_POLARITY_FLIP)) {
        phy_xaui_polarity_flip = soc_property_port_get(unit, port,
                                              spn_PHY_XAUI_RX_POLARITY_FLIP, 0);
        /* Flip RX polarity. */
        /*XXX hardcoded for now */
        mask16 = 0xc;
        data16 = phy_xaui_polarity_flip? mask16 :0;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "unit %d port %s: RX Polarity swap 0x%04x.\n"),
                  unit, SOC_PORT_NAME(unit, port), data16));

        SOC_IF_ERROR_RETURN
            (XGXS16G_REG_MODIFY(unit,pc,0x0, 0x80ba + (0x10 * pc->lane_num),
                 data16, mask16));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g1l_init
 * Purpose:     
 *      Initialize xgxs6 phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g1l_init(int unit, soc_port_t port)
{
    uint16              data16;
    uint16              mask16;
    uint16             mode_1000x;
    int                 preemph, idriver, pdriver;
    phy_ctrl_t         *pc;
    soc_port_ability_t  ability;
    XGXS16G_DEV_DESC *pDesc;
    soc_phy_info_t *pi;
    int len;
    int serdes_id0;
    int asymmetric_speed_mode = 0;
    uint16              rx_los = 0;
    uint16              rx_los_invert = 0;


    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (XGXS16G_DEV_DESC *)(pc + 1);

    asymmetric_speed_mode = soc_property_port_get(unit, port, spn_SERDES_ASYMMETRIC_SPEED_MODE, 0);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID0r(unit, pc, &pDesc->serdes_id0));
    serdes_id0 = pDesc->serdes_id0;
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID2r(unit, pc, &pDesc->serdes_id2));

    /* compose device name/instance/lane information for display
     * XGXS_16G device uses existing method
     */
    if (XGXS16G_13G_ID(pc) || XGXS16G_2p5G_ID(pc)) {
        if (XGXS16G_13G_ID(pc)) {
        /* 
         * COVERITY
         *
         * Buffer will not happen as destination string is of length 30.
         */
        /* coverity[secure_coding] */

            sal_strcpy(pDesc->name,"XGXS13G-");
        } else {
            sal_strcpy(pDesc->name,"QS-");
        }
        len = sal_strlen(pDesc->name);
        /* add rev letter */
        pDesc->name[len++] = 'A' + ((serdes_id0 >>
                          SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pDesc->name[len++] = '0' + ((serdes_id0 >>
                          SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pDesc->name[len++] = '/';
        pDesc->name[len++] = (pc->chip_num/10)%10 + '0';
        pDesc->name[len++] = pc->chip_num%10 + '0';
        pDesc->name[len++] = '/';
        pDesc->name[len++] = pc->lane_num + '0';
        pDesc->name[len] = 0;  /* string terminator */

        if (len > XGXS16G_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "XGXS13G info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,XGXS16G_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }

        pi = &SOC_PHY_INFO(unit, port);
        if (!PHY_EXTERNAL_MODE(unit, port)) {
            pi->phy_name = pDesc->name;
        }

    }

    /* Initialize software state */
    pc->fiber.autoneg_enable = TRUE;

    if ((!PHY_FLAGS_TST(unit, port, PHY_FLAGS_INIT_DONE)) &&
       (pc->lane_num == 0)) {

        /* Reset the core */

        /* Stop PLL Sequencer and configure the core into correct mode */
        data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane <<
                    XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
                 XGXSBLK0_XGXSCONTROL_HSTL_MASK |
                 XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
                 XGXSBLK0_XGXSCONTROL_EDEN_MASK |
                 XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
                 XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, data16)); 

        /* Disable IEEE block select auto-detect. 
         * The driver will select desired block as necessary.
         * By default, the driver keeps the XAUI block in
         * IEEE address space.
         */
        if (XGXS16G_2p5G_ID(pc)) {
            data16 = 0;
        } else {
            data16 = XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_XGXSBLK0_MISCCONTROL1r(unit, pc, 
                           data16,
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK));

        if (XGXS16G_13G_ID(pc)) {  
            data16 = XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK |
                     XGXSBLK0_MMDSELECT_MULTIMMDS_EN_MASK;
            mask16 = data16;
#ifdef BCM_KATANA_SUPPORT
            if (SOC_IS_KATANA(unit)) {
                if ((25 == port) || (26 == port)) {
                    data16 &= ~XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;
                }
            }
#endif
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_XGXSBLK0_MMDSELECTr(unit, pc,
                           data16, mask16));
        }

        /* disable in-band MDIO. PHY-443 */
        SOC_IF_ERROR_RETURN
            (XGXS16G_REG_MODIFY(unit, pc, 0x00, 0x8111, 1 << 3, 1 << 3));


        if (XGXS16G_13G_REV_B0(pc) && asymmetric_speed_mode) { 
        
            /* FORCE ASYMMETRIC SPEED MODES. Refer sect 4.3.2 product brief. */
            
            /*
               1. Force VCO at 3.125GHz by configuring 
                  Misc1 register (0x8308) so that 
                  Misc1[12] = 1'b1; 
                  Misc[11:8] = 4'h1.
             */
            SOC_IF_ERROR_RETURN
               (MODIFY_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, 
                          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK
                            | (0x1 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT),
                          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK
                            | SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK));
            /*
               2. Enable the asymmetric_enable 
                  bit[BIT8] within the xgxsBlk8txLnSwap1 register (0x8169)
             */

            SOC_IF_ERROR_RETURN
               (MODIFY_XGXS16G_XGXSBLK8_TXLNSWAP1r(unit, pc, 
                          XGXSBLK8_TXLNSWAP1_ASYMM_EN_MASK,
                          XGXSBLK8_TXLNSWAP1_ASYMM_EN_MASK));

           } 

    }

    if (XGXS16G_13G_ID(pc)) {  
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_REMOTEPHY_MISC3r(unit, pc,
                                             0, (1 << 6)));
    }

    /* check if need to do lane swap */
    if (FEATURE_NEW_LANESWAP(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_xgxs16g1l_lane_swap(unit,port)); 
    }

    /* check if need to polarity flip */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g1l_polarity_flip(unit,port)); 

    if (soc_property_port_get(unit, port,
                          spn_PHY_AN_C73, FALSE)) {
         PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
    }

    /* disable CL73 BAM */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN_CL73_REG(unit,pc,0x8372,
               0,
               CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK));

    if (!PHY_CLAUSE73_MODE(unit, port)) {
        /* disable CL73 AN device*/
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_AN_CL73_REG(unit,pc,MII_CTRL_REG,0));
    }

    /* Set Local Advertising Configuration */
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g1l_ability_local_get(unit, port, &ability));
    /* Do not advertise 2.5G. 1Gbps vs 2.5 Gbps AN is not supported */
    ability.speed_full_duplex &= SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB |
                                 SOC_PA_SPEED_1000MB;
    pc->fiber.advert_ability = ability;
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g1l_ability_advert_set(unit, port, &ability));
                                                                                
    /* Disable BAM in Independent Lane mode. Over1G AN not supported  */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr(unit, pc, 0));
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_BAM_NEXTPAGE_UD_FIELDr(unit, pc, 0));
                                                                                
    data16 = SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
             SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, data16));

    mask16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
    }

    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    mode_1000x = 0;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        mode_1000x = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
    /* Allow property to override */
    if (soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, mode_1000x)) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    } else {
        data16 &= ~SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, data16, mask16));   
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        /* Enable 1000X parallel detect */
        data16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
        mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    } else {
        data16 = 0;
        mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK |
                 SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data16, mask16));

    /* Initialialize autoneg and fullduplex */
    data16 = MII_CTRL_FD | MII_CTRL_SS_1000;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 |= MII_CTRL_AE | MII_CTRL_RAN;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, data16));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_AN_CL73_REG(unit, pc,MII_CTRL_REG,
                                          data16,
                                          MII_CTRL_AE | MII_CTRL_RAN));
    }

    /* Disable 10G parallel detect */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_AN73_PDET_PARDET10GCONTROLr(unit, pc, 0));

    /* Disable BAM mode and Teton mode */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr(unit, pc, 0));

    /* Initialize XAUI to reasonable default settings */
    if (XGXS16G_13G_ID(pc) || XGXS16G_2p5G_ID(pc)){
        preemph = 0;
        idriver = 4;
        preemph = soc_property_port_get(unit, port, spn_XGXS_PREEMPHASIS, preemph);
        idriver = soc_property_port_get(unit, port, spn_XGXS_DRIVER_CURRENT, idriver);
        data16  = ((preemph & 0xf) << TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_SHIFT) |
                  ((idriver & 0x7) << TX_ALL_040_TX_DRIVER_IDRIVER_SHIFT);
        mask16  = TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_MASK |
                  TX_ALL_040_TX_DRIVER_IDRIVER_MASK;
        switch(pc->lane_num){
            case 0:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS13G_040_TX0_ACONTROL1r(unit, pc, data16, mask16));
                break;
            case 1:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS13G_040_TX1_ACONTROL1r(unit, pc, data16, mask16));
                break;
            case 2:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS13G_040_TX2_ACONTROL1r(unit, pc, data16, mask16));
                break;
            case 3:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS13G_040_TX3_ACONTROL1r(unit, pc, data16, mask16));
                break;
            default:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS13G_040_TX_ALL_ACONTROL1r(unit, pc, data16, mask16));
                break;
        }
    } else {
        preemph = 0;
        idriver = 9;
        pdriver = 9;
        preemph = soc_property_port_get(unit, port, spn_XGXS_PREEMPHASIS, preemph);
        idriver = soc_property_port_get(unit, port, spn_XGXS_DRIVER_CURRENT, idriver);
        pdriver = soc_property_port_get(unit, port, spn_XGXS_PRE_DRIVER_CURRENT, pdriver);
        data16  = ((preemph & 0xf) << TX_ALL_TX_DRIVER_PREEMPHASIS_SHIFT) |
                  ((idriver & 0xf) << TX_ALL_TX_DRIVER_IDRIVER_SHIFT) |
                  ((pdriver & 0xf) << TX_ALL_TX_DRIVER_IPREDRIVER_SHIFT);
        mask16  = TX_ALL_TX_DRIVER_PREEMPHASIS_MASK |
                  TX_ALL_TX_DRIVER_IDRIVER_MASK |
                  TX_ALL_TX_DRIVER_IPREDRIVER_MASK;
        switch(pc->lane_num){
            case 0:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX0_TX_DRIVERr(unit, pc, data16, mask16));
                break;
            case 1:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX1_TX_DRIVERr(unit, pc, data16, mask16));
                break;
            case 2:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX2_TX_DRIVERr(unit, pc, data16, mask16));
                break;
            case 3:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX3_TX_DRIVERr(unit, pc, data16, mask16));
                break;
            default:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX_ALL_TX_DRIVERr(unit, pc, data16, mask16));
                break;
        }
    }

    /* Enable lanes */
    mask16 = XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
             XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK;
    data16 = mask16;
    SOC_IF_ERROR_RETURN
      (MODIFY_XGXS16G_XGXSBLK1_LANECTRL0r(unit, pc, data16, mask16));

    /* set elasticity fifo size to 13.5k to support 12k jumbo pkt size*/
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X3r(unit,pc,
                   (1 << 2),
                   SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_RX_MASK));

    /* Enabble LPI passthru' for native mode EEE */
    if (pc->dev_name && !(sal_strcmp(pc->dev_name, "XGXS13G1l") && sal_strcmp(pc->dev_name, "QS40"))) {
        /* Enabble LPI passthru' for native mode EEE */
        SOC_IF_ERROR_RETURN
             (MODIFY_XGXS16G_REMOTEPHY_MISC5r(unit, pc, 0xc000, 0xc000));
        SOC_IF_ERROR_RETURN
             (MODIFY_XGXS16G_XGXSBLK7_EEECONTROLr(unit, pc, 0x0007, 0x0007));
    }

    if ((!PHY_FLAGS_TST(unit, port, PHY_FLAGS_INIT_DONE)) && 
        (pc->lane_num == 0)) {
        /* Start PLL Sequencer and wait for PLL to lock */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
        (void) _phy_xgxs16g1l_pll_lock_wait(unit, port);
    }

 /* PRBS Polynomial for all lanes: 0x8019=0x0000 */

    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, 0));

    /* ************************************************************************/
    /* Speical case handling for KT2:OLP port(MXQ10:lane2).Lane0,1,3 not used */
    /* ************************************************************************/

#ifdef BCM_KATANA2_SUPPORT
    if ((SOC_IS_KATANA2(unit)) && 
        (SOC_INFO(unit).olp_port[0]) && 
        (port == 40) && /* OLP PORT surely */
        (!(soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE))) {
        /* Set Broadcast mode */
        SOC_IF_ERROR_RETURN(WRITE_XGXS16G_AERBLK_AERr(unit, pc, 0x1F));
        /* Disable PLL */
        SOC_IF_ERROR_RETURN(
                     WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x062f));
        /* Put serdes in 4 * 2.5G symmetric mode then */
        SOC_IF_ERROR_RETURN(
                     WRITE_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, 0x6010));
        /* Enable PLL */
        SOC_IF_ERROR_RETURN(
                     WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x262f));
    }
#endif
    /* Check if HW rx los is configured */
    data16 = 0;
    mask16 = SERDESDIGITAL_CONTROL1000X1_SIGNAL_DETECT_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_INVERT_SIGNAL_DETECT_MASK;
    if (soc_property_port_get(unit, port,
                              spn_SERDES_RX_LOS, rx_los)) {
        data16 = SERDESDIGITAL_CONTROL1000X1_SIGNAL_DETECT_EN_MASK;
    }
    if (soc_property_port_get(unit, port,
                              spn_SERDES_RX_LOS_INVERT, rx_los_invert)) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_INVERT_SIGNAL_DETECT_MASK;
    } 
    SOC_IF_ERROR_RETURN(
        MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, data16, mask16));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g1l_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;
    uint16  mii_serdes_stat;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
        return (SOC_E_NONE);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_MIISTATr(unit, pc, &mii_serdes_stat));

    *link = ((mii_serdes_stat & MII_STAT_LA) != 0);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g1l_enable_set(int unit, soc_port_t port, int enable)
{
    int rv;

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        rv = _phy_xgxs16g1l_notify_resume(unit, port, PHY_STOP_PHY_DIS);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        rv = _phy_xgxs16g1l_notify_stop(unit, port, PHY_STOP_PHY_DIS);
    }

    return (rv);
}

/*
 * Function:
 *      phy_xgxs16g1l_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g1l_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *local_pc;
    uint16      speed_val, mask;
    uint16      sgmii_status = 0, data16, temp16;
    uint16      speed_mii;
    int asymmetric_speed_mode = 0;
    int blk_num = 0;
    soc_pbmp_t blk_pbmp;
    soc_port_t local_port = 0;

    pc = INT_PHY_SW_STATE(unit, port);
                                                                               
    if (speed > 2500) {
        return (SOC_E_CONFIG);
    }

    speed_val = 0;
    speed_mii = 0;
    mask      = SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
                SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;

    asymmetric_speed_mode = soc_property_port_get(unit, port, spn_SERDES_ASYMMETRIC_SPEED_MODE, 0);
    
    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 10:
        speed_mii = MII_CTRL_SS_10;
        break;
    case 100:
        speed_mii = MII_CTRL_SS_100;
        break;
    case 1000:
        speed_mii = MII_CTRL_SS_1000;
        break;
    case 2500:
        speed_val = 0;
        break;
    default:
        return SOC_E_PARAM;
    }

    /* 2.5G only valid in fiber mode */
    if (speed == 2500) {
        speed_val |= SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK;
    }

    /* Hold rxSeqStart */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_RX0_RX_CONTROLr(unit, pc,
                                  DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK,
                                  DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK));
                                                                               
    /* hold TX FIFO in reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X3r(unit, pc,
                           SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK,
                           SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK));

    if(XGXS16G_13G_REV_B0(pc) && asymmetric_speed_mode) {

        /*  FORCE ASYMMETRIC SPEED MODES. Refer sect 4.3.2 product brief.
            Initial setup is done under init()

            (RESET TX and RX lane) 

           1. Set anaTxAControl1 7th bit 
           (Lane0 - 0x8065[6]; 
            Lane1 - 0x8075[6]; 
            Lane2 - 0x8085[6]; 
            Lane3 - 0x8095[6]; 
            All lanes - 0x80A5[6];) 
           if rates are 1G or less than 1G. 
           Keep this bit at the default value 1'b0, if the rate is 2.5G.
        
           2. Set anaRxAControl1 7th bit 
           (Lane0 - 0x80BC[6];
            Lane1 - 0x80CC[6]; 
            Lane2 - 0x80DC[6]; 
            Lane3 - 0x80EC[6]; 
            All lanes - 0x80FC[6];) 
            if rates are 1G or less than 1G. 
            Keep this bit at the default value 1'b0, if the rate is 2.5G.

           3. Set laneCtrl1 (0x8016), so that each lane is in OS1 mode.
              For OS1, set mode value = 2'b11 = 0x3

      */
        temp16 = 0x0;
        if( speed <= 1000) {
            temp16 = 0x1 << 6;
        }
        
        /* Reset lane - tx & rx */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_XGXSBLK2_LANERESETr(unit, pc,
                                 ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETTX_SHIFT)
                               | ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETRX_SHIFT),
                                 ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETTX_SHIFT)
                               | ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETRX_SHIFT)));

        switch(pc->lane_num) {
            case 0:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX0_TX_BGVCMr(unit, pc,
                                             temp16,
                                             TX0_TX_BGVCM_TICKSEL_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_RX0_RX_ANALOGBIAS0r(unit, pc,
                                             temp16,
                                             RX0_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                            0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT
                                          | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT,
                                            XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK
                                          | XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK));
                break;
            case 1:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX1_TX_BGVCMr(unit, pc,
                                             temp16,
                                             TX1_TX_BGVCM_TICKSEL_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_RX1_RX_ANALOGBIAS0r(unit, pc,
                                             temp16,
                                             RX1_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                            0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT
                                          | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT,
                                            XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK
                                          | XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK));
                break;
            case 2:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX2_TX_BGVCMr(unit, pc,
                                             temp16,
                                             TX2_TX_BGVCM_TICKSEL_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_RX2_RX_ANALOGBIAS0r(unit, pc,
                                             temp16,
                                             RX2_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                            0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT
                                          | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT,
                                            XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK
                                          | XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK));
                break;
            case 3:
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_TX3_TX_BGVCMr(unit, pc,
                                             temp16,
                                             TX3_TX_BGVCM_TICKSEL_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_RX3_RX_ANALOGBIAS0r(unit, pc,
                                             temp16,
                                             RX3_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                            0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT
                                          | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT,
                                            XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK
                                          | XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK));
                break;
            default:
               return SOC_E_FAIL;
        }               

        if (speed <= 1000) {
            speed_val |= SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK;
            mask |= SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK;
        }
    }

    if (FEATURE_SPEED_100FX(pc)
        && IF_XGXS16G_13G_B0_IS_SYMMETRIC_MODE(pc, asymmetric_speed_mode)) {
        /* disable 100FX and 100FX auto-detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_FX100_CONTROL1r(unit,pc,
                              0,
                              FX100_CONTROL1_AUTODET_EN_MASK |
                              FX100_CONTROL1_ENABLE_MASK));

        /* disable 100FX idle detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_FX100_CONTROL3r(unit,pc,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_SERDESDIGITAL_STATUS1000X1r(unit, pc, &sgmii_status));

        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, speed_val, mask));
    
    if ( FEATURE_SPEED_100FX(pc)
         && (!sgmii_status && (speed == 100))
         && IF_XGXS16G_13G_B0_IS_SYMMETRIC_MODE(pc, asymmetric_speed_mode)) {
		/* fiber mode 100fx, enable */
		SOC_IF_ERROR_RETURN
			(MODIFY_XGXS16G_FX100_CONTROL1r(unit,pc,
						  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
						  FX100_CONTROL1_ENABLE_MASK,
						  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
						  FX100_CONTROL1_ENABLE_MASK));

		/* enable 100fx extended packet size */
		SOC_IF_ERROR_RETURN
			(MODIFY_XGXS16G_FX100_CONTROL2r(unit,pc,
						FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
						FX100_CONTROL2_EXTEND_PKT_SIZE_MASK));

    } else {
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &data16));
        data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
        data16 |= speed_mii;
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, data16));
    }


    if(XGXS16G_13G_REV_B0(pc) && asymmetric_speed_mode) {
        /* Release Rx and Tx lane reset  */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_XGXSBLK2_LANERESETr(unit, pc, 0,
                                 ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETTX_SHIFT)
                               | ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETRX_SHIFT)));

    }


    /* release rxSeqStart */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_RX0_RX_CONTROLr(unit, pc,
                              0,
                              DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK));
                                                                               
    /* release TX FIFO reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X3r(unit, pc,
                           0,
                           SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK));
    
    if (SOC_IS_KATANA(unit)) {
        blk_num = SOC_PORT_BLOCK(unit, port);
        SOC_PBMP_ASSIGN(blk_pbmp,SOC_BLOCK_BITMAP(unit,blk_num));
        SOC_PBMP_ITER(blk_pbmp,local_port) {
            local_pc = INT_PHY_SW_STATE(unit, local_port);
            if (local_pc == NULL) {
                continue;
            }
            if ((local_pc->lane_num == 0) && !SOC_PORT_VALID(unit, local_port)) {
                SOC_IF_ERROR_RETURN(WRITE_XGXS16G_AERBLK_AERr(unit, pc, 0x1f));
                break;
            }
        }
    }
    
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_xgxs16g1l_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16 data16;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (NULL == speed) {
        return (SOC_E_PARAM);
    }
 
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESDIGITAL_STATUS1000X1r(unit, pc, &data16));

    data16 &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
    data16 >>= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT;

    if (data16 == 3) {
        *speed= 2500;
    } else if (data16 == 2) {
        *speed= 1000;
    } else if (data16 == 1) {
        *speed= 100;
    } else {   
        *speed= 10;
    }    
    
    return (SOC_E_NONE);
}

STATIC int
_force_asym_speedmod_set(int unit, soc_port_t port, int is_speed_2500)
{
    int asymmetric_speed_mode ;
    phy_ctrl_t  *pc;
    uint16   temp16;


    pc = INT_PHY_SW_STATE(unit, port);
    asymmetric_speed_mode = soc_property_port_get(unit, port, spn_SERDES_ASYMMETRIC_SPEED_MODE, 0);
    if(XGXS16G_13G_REV_B0(pc) && asymmetric_speed_mode) {

            /*  FORCE ASYMMETRIC SPEED MODES. Refer sect 4.3.2 product brief.
                Initial setup is done under init()

                (RESET TX and RX lane)

               1. Set anaTxAControl1 7th bit
               (Lane0 - 0x8065[6];
                Lane1 - 0x8075[6];
                Lane2 - 0x8085[6];
                Lane3 - 0x8095[6];
                All lanes - 0x80A5[6];)
               if rates are 1G or less than 1G.
               Keep this bit at the default value 1'b0, if the rate is 2.5G.

               2. Set anaRxAControl1 7th bit
               (Lane0 - 0x80BC[6];
                Lane1 - 0x80CC[6];
                Lane2 - 0x80DC[6];
                Lane3 - 0x80EC[6];
                All lanes - 0x80FC[6];)
                if rates are 1G or less than 1G.
                Keep this bit at the default value 1'b0, if the rate is 2.5G.

               3. Set laneCtrl1 (0x8016), so that each lane is in OS1 mode.
                  For OS1, set mode value = 2'b11 = 0x3

          */
            temp16 = 0x0;
            if(!is_speed_2500) {
                temp16 = 0x1 << 6;
            }

            /* Reset lane - tx & rx */
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_XGXSBLK2_LANERESETr(unit, pc,
                                     ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETTX_SHIFT)
                                   | ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETRX_SHIFT),
                                     ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETTX_SHIFT)
                                   | ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETRX_SHIFT)));

            switch(pc->lane_num) {
                case 0:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_TX0_TX_BGVCMr(unit, pc,
                                                 temp16,
                                                 TX0_TX_BGVCM_TICKSEL_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_RX0_RX_ANALOGBIAS0r(unit, pc,
                                                 temp16,
                                                 RX0_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                                0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT
                                              | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT,
                                                XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK
                                              | XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK));
                    break;
                case 1:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_TX1_TX_BGVCMr(unit, pc,
                                                 temp16,
                                                 TX1_TX_BGVCM_TICKSEL_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_RX1_RX_ANALOGBIAS0r(unit, pc,
                                                 temp16,
                                                 RX1_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                                0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT
                                              | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT,
                                                XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK
                                              | XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK));
                    break;
                case 2:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_TX2_TX_BGVCMr(unit, pc,
                                                 temp16,
                                                 TX2_TX_BGVCM_TICKSEL_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_RX2_RX_ANALOGBIAS0r(unit, pc,
                                                 temp16,
                                                 RX2_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                                0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT
                                              | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT,
                                                XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK
                                              | XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK));
                    break;
                case 3:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_TX3_TX_BGVCMr(unit, pc,
                                                 temp16,
                                                 TX3_TX_BGVCM_TICKSEL_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_RX3_RX_ANALOGBIAS0r(unit, pc,
                                                 temp16,
                                                 RX3_RX_ANALOGBIAS0_SIG_PWRDN_MASK));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc,
                                                0x3 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT
                                              | 0x3 << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT,
                                                XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK
                                              | XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK));
                    break;
                default:
                   return SOC_E_FAIL;
            }
            /* Release Rx and Tx lane reset  */
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_XGXSBLK2_LANERESETr(unit, pc, 0,
                                  ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETTX_SHIFT)
                                | ((1 << pc->lane_num) <<XGXSBLK2_LANERESET_RESETRX_SHIFT)));

        }

    return SOC_E_NONE ;

}
/*
 * Function:    
 *      phy_xgxs16g1l_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_xgxs16g1l_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    phy_ctrl_t  *local_pc;
    uint16             an_enable;
    uint16             auto_det;
    int blk_num = 0;
    soc_pbmp_t blk_pbmp;
    soc_port_t local_port = 0;
    int speed = 0;
    int is_speed_2500 = 0;

    pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g1l_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));

    an_enable = 0;
    auto_det  = 0;

    if (an) {
        /* disable 100FX and 100FX auto-detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_FX100_CONTROL1r(unit, pc, 0,
                                            FX100_CONTROL1_AUTODET_EN_MASK |
                                            FX100_CONTROL1_ENABLE_MASK));

        an_enable = MII_CTRL_AE | MII_CTRL_RAN;

        /*
         * Should read one during init and cache it in Phy flags
         */
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM,
                                  FALSE)) {
            auto_det = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
        }

        /* If auto-neg is enabled, make sure not forcing any speed */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, 0,
                                   SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK));
        /* Enable/Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));
                                                                                
    } else {
        /* Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));
                                                                                
    }

    /* restart the autoneg if enabled, or disable the autoneg */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, an_enable,
                                      MII_CTRL_AE | MII_CTRL_RAN));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_AN_CL73_REG(unit, pc,MII_CTRL_REG,
                                          an_enable,
                                          MII_CTRL_AE | MII_CTRL_RAN));
    }

    if(an) {
        is_speed_2500 = 0;
    }else{
        SOC_IF_ERROR_RETURN
            (phy_xgxs16g1l_speed_get(unit, port, &speed));
        if(speed == 2500) {
           is_speed_2500 = 1;
        } else {
            is_speed_2500 = 0;
        }
    }

    SOC_IF_ERROR_RETURN
        (_force_asym_speedmod_set(unit,port,is_speed_2500));

    pc->fiber.autoneg_enable = an;

    if (SOC_IS_KATANA(unit)) {
        blk_num = SOC_PORT_BLOCK(unit, port);
        SOC_PBMP_ASSIGN(blk_pbmp,SOC_BLOCK_BITMAP(unit,blk_num));
        SOC_PBMP_ITER(blk_pbmp,local_port) {
            local_pc = INT_PHY_SW_STATE(unit, local_port);
            if (local_pc == NULL) {
                continue;
            } 
            if ((local_pc->lane_num == 0) && !SOC_PORT_VALID(unit, local_port)) {
                SOC_IF_ERROR_RETURN(WRITE_XGXS16G_AERBLK_AERr(unit, pc, 0x1f));
                break;
            }
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:    
 *      phy_xgxs16g1l_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_xgxs16g1l_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      mii_stat;
    phy_ctrl_t *pc;
                                                                                
    if ((NULL == an) || (NULL == an_done)) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_MIISTATr(unit, pc, &mii_stat));
                                                                                
    *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
    *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
    if (!((*an == TRUE) && (*an_done == TRUE))) {
        if (PHY_CLAUSE73_MODE(unit, port)) {
            /* check clause 73 */
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_AN_CL73_REG(unit, pc, MII_CTRL_REG,&mii_ctrl));
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_AN_CL73_REG(unit, pc, MII_STAT_REG, &mii_stat));

            *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
            *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_c73_adv_local_set(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    uint16            pause;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ?
               CL73_AN_ADV_TECH_1G_KX : 0;
    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ?
              CL73_AN_ADV_TECH_10G_KX4 : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT1_REG, an_adv,
                                     CL73_AN_ADV_TECH_SPEEDS_MASK));

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        pause = CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE;
        break;
    default:
        pause = 0;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT0_REG, pause,
                                     (CL73_AN_ADV_PAUSE |
                                      CL73_AN_ADV_ASYM_PAUSE)));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g1l_c73_adv_local_set: u=%d p=%d pause=%08x speeds=%04x,adv=0x%x\n"),
              unit, port, pause, an_adv,ability->speed_full_duplex));
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_c73_adv_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   speeds,pause;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT1_REG, &an_adv));

    speeds = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= speeds;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT0_REG, &an_adv));

    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_RX;
            break;
        default:
            pause = 0;
            break;
    }
    ability->pause = pause;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g1l_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, pause, speeds));
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_c73_adv_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;
     pc = INT_PHY_SW_STATE(unit, port);
     SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc,CL73_AN_LP_ABILITY1_REG, &an_adv));

    mode = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= mode;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_LP_ABILITY0_REG,&an_adv));

    mode = 0;
    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g1l_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_xgxs16g1l_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    uint16           an_adv;
    phy_ctrl_t      *pc;
                                                                                
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
                                                                                
    pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    /* Set advertised duplex (only FD supported).  */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
              MII_ANA_C37_FD : 0;
                                                                                
    /* Set advertised pause bits in link code word.  */
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE;
        break;
    }
    /* Update less than 1G capability */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_COMBO_IEEE0_AUTONEGADVr(unit, pc, an_adv));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_xgxs16g1l_c73_adv_local_set(unit, port, ability));
    }
                                                                                
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g1l_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
              unit, port, ability->pause, an_adv));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_xgxs16g1l_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16           an_adv;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_AUTONEGADVr(unit, pc, &an_adv));
                                                                                
    ability->speed_full_duplex = 
                  (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    /* check for clause73 */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_xgxs16g1l_c73_adv_local_get(unit, port, ability));
    }
                                                                             
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g1l_ability_advert_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));
                                                                                
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_xgxs16g1l_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    phy_ctrl_t       *pc;
    uint16            an_adv;
    soc_port_mode_t   mode;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
 
    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_AUTONEGLPABILr(unit, pc, &an_adv));

    mode = (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
    ability->speed_full_duplex = mode;

    mode = 0;
    switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
        case MII_ANP_C37_PAUSE:
            mode |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANP_C37_ASYM_PAUSE:
            mode |= SOC_PA_PAUSE_TX;
            break;
        case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
            mode |= SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_xgxs16g1l_c73_adv_remote_get(unit, port, ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g1l_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_xgxs16g1l_lb_set
 * Purpose:
 *      Put XGXS6/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g1l_lb_set(int unit, soc_port_t port, int enable)
{
    uint16      misc_ctrl;
    uint16      lb_bit;
    uint16      lb_mask;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    
    /* Configure Loopback in XAUI */
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK0_MISCCONTROL1r(unit, pc, &misc_ctrl));
    if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PCS_DEV_EN_OVERRIDE_MASK) {
          /* PCS */
          lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
          lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
    } else if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PMD_DEV_EN_OVERRIDE_MASK) {
          /* PMA/PMD */
          lb_bit  = (enable) ? 1 : 0;
          lb_mask = 1;
    } else {
          /* PHY XS, DTE XS */
          lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
          lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_IEEE0BLK_IEEECONTROL0r(unit, pc, lb_bit, lb_mask));

    /* Configure Loopback in SerDes */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, 
                                             (enable) ? MII_CTRL_LE : 0,
                                             MII_CTRL_LE));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_lb_get
 * Purpose:
 *      Get XGXS6/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g1l_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      mii_ctrl;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    *enable = ((mii_ctrl & MII_CTRL_LE) == MII_CTRL_LE);

    return (SOC_E_NONE);
}


STATIC int
phy_xgxs16g1l_ability_local_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
    if (PHY_FIBER_MODE(unit, port))   {
        ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
        if (FEATURE_SPEED_100FX(pc)) {
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
        }
    } else {
        ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                      SOC_PA_SPEED_100MB;
        ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                       SOC_PA_SPEED_100MB;
    }

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM; 
    ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    return (SOC_E_NONE);
}

STATIC int
_phy_xgxs16g1l_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    uint16  data;             /* Temporary holder of reg value to be written */
    uint16  mask;             /* Bit mask of reg value to be updated */

    if (value > 15) {
        return SOC_E_PARAM;
    }

    data = mask = 0;
    if (XGXS16G_13G_ID(pc) || XGXS16G_2p5G_ID(pc)){
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             data = value << TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_SHIFT;
             mask = TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_MASK;
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             data = value << TX_ALL_040_TX_DRIVER_IDRIVER_SHIFT;
             mask = TX_ALL_040_TX_DRIVER_IDRIVER_MASK;
             break;
        default:
             /* should never get here */
             return SOC_E_PARAM;
        }
        /* Update preemphasis/driver/pre-driver current */
        switch(pc->lane_num){
        case 0:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS13G_040_TX0_ACONTROL1r(unit, pc, data, mask));
             break;
        case 1:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS13G_040_TX1_ACONTROL1r(unit, pc, data, mask));
             break;
        case 2:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS13G_040_TX2_ACONTROL1r(unit, pc, data, mask));
             break;
        case 3:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS13G_040_TX3_ACONTROL1r(unit, pc, data, mask));
             break;
        default:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS13G_040_TX_ALL_ACONTROL1r(unit, pc, data, mask));
             break;
        }
    } else {
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             data = value << TX_ALL_TX_DRIVER_PREEMPHASIS_SHIFT;
             mask = TX_ALL_TX_DRIVER_PREEMPHASIS_MASK;
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             data = value << TX_ALL_TX_DRIVER_IDRIVER_SHIFT;
             mask = TX_ALL_TX_DRIVER_IDRIVER_MASK;
             break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
             data = value << TX_ALL_TX_DRIVER_IPREDRIVER_SHIFT;
             mask = TX_ALL_TX_DRIVER_IPREDRIVER_MASK;
             break;
        default:
             /* should never get here */
             return SOC_E_PARAM;
        }
        /* Update preemphasis/driver/pre-driver current */
        switch(pc->lane_num){
        case 0:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS16G_TX0_TX_DRIVERr(unit, pc, data, mask));
             break;
        case 1:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS16G_TX1_TX_DRIVERr(unit, pc, data, mask));
             break;
        case 2:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS16G_TX2_TX_DRIVERr(unit, pc, data, mask));
             break;
        case 3:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS16G_TX3_TX_DRIVERr(unit, pc, data, mask));
             break;
        default:
             SOC_IF_ERROR_RETURN
                 (MODIFY_XGXS16G_TX_ALL_TX_DRIVERr(unit, pc, data, mask));
             break;
        }

    }
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_linkdown_transmit_set(int unit, soc_port_t port, 
                                           uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (value) {
        data = (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data));
    } else {
        data = (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_BITS <<
                SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_linkdown_transmit_get(int unit, soc_port_t port, 
                                           uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
            (READ_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, &data));

    *value = (data & 
              (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
               SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT))
             ? 1 : 0;

    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g1l_control_prbs_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;

    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      prbs_lane_mask = 0;

    value &= 7;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    prbs_lane_mask = 0x7 << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT * pc->lane_num);
    data &= ~prbs_lane_mask;
    data |= value << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT * pc->lane_num);

    
    /* 
     * write back updated configuration 
     */

    SOC_IF_ERROR_RETURN
	(WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    /* Extract prbs polynomial setting from register */
    data = ((data >> (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num)) &
            XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK);
    *value = (uint32) data;

    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g1l_control_prbs_tx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);		
    uint32      lane_mask = 1 << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT * pc->lane_num);


    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    
    /* prbs_inv is bit 2 per lane */
    if (value) {
        data |= lane_mask << XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    } else {
        data &= ~(lane_mask << XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT);
    }

    /* write back updated configuration */
    if (value > 0x8000) {
        SOC_IF_ERROR_RETURN
	    (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, value));
    } else {
        SOC_IF_ERROR_RETURN
	    (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));
    } 

    return SOC_E_NONE;

}

STATIC int
_phy_xgxs16g1l_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    uint16       inv_shifter = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    inv_shifter = (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num) +
                 XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
   
    data &= (1 << inv_shifter);
    
    *value = (data) ?  1 : 0;
    
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g1l_control_xgxs_prbs_tx_enable_set(int unit, soc_port_t port, 
                                          uint32 value)
{
    uint16      data = 0;
    uint16      pll_mode = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (value) {

        /* Set double wide div/1: 0x8016=0xffff */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc, 0xffff));
        
        /* clear multiPRT mode: 0x800d=0x0003 */
        /*SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_MMDSELECTr(unit, pc, 0x4003));
	*/

        /* Set in individual lane mode, Disable PLL Sequencer: 0x8000=0x063f */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x063f));
        
        /* Force VCO based on current operating frequency */
        SOC_IF_ERROR_RETURN 
            (READ_XGXS16G_PLL_PLLSTATUSr(unit, pc, &pll_mode)); 
        pll_mode &= 
            (PLL_PLLSTATUS_PLL_MODE_AFE_MASK | PLL_PLLSTATUS_RESERVED0_MASK); 
        SOC_IF_ERROR_RETURN 
            (READ_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, &data)); 
        data &= ~SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK; 
        data |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
            (pll_mode << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT); 
        SOC_IF_ERROR_RETURN 
            (WRITE_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, data)); 

	/* Disable asymmetric mode for 40nm UC_B0 */
	if(XGXS16G_13G_REV_B0(pc)){
	       SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_XGXSBLK8_TXLNSWAP1r(unit, pc, 0,
                        XGXSBLK8_TXLNSWAP1_ASYMM_EN_MASK)); 
	}

        /* Turn AN off: 0x0=0x0 */
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &data));
        data &= ~IEEE0BLK_IEEECONTROL0_AUTONEG_ENABLE_MASK;
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, 0));
	
        /* Turn CL36 PCS off: 0x8015=0x0 */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANECTRL0r(unit, pc, 0));
        
        /* Put port in loopback if requested */
        data = ((value & 0x8000) == 0x8000) ? 
               XGXSBLK1_LANECTRL2_GLOOP1G_MASK : 0;
        /* CDED & EDEN off: 0x8017 */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANECTRL2r(unit, pc, data));

        /* PRBS Polynomial for all lanes: 0x8019=0x8888 */
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));
        data |= 0x8888;
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));
 
        /* Enable PRBS read: 0x80f1=0x1c47 */ 
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_RX_ALL_RX_CONTROLr(unit, pc, 0x1c47));
        
        /* Enable PLL Sequencer: 0x8000=0x263f */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x263f));
        
        /* Delay a while */
        sal_usleep(250000);
    } else {
        /* Restore port */
        return phy_xgxs16g1l_init(unit, port);
    }
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_prbs_tx_enable_set(int unit, soc_port_t port, uint32 value)
{
    SOC_IF_ERROR_RETURN(
			_phy_xgxs16g1l_control_xgxs_prbs_tx_enable_set(unit, port, value));
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g1l_control_prbs_tx_rx_enable_get(int unit, soc_port_t port, 
                                        uint32 *value)
{
    uint16      data;
    uint16       inv_shifter = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    /*
     * Hypercore PRBS - note that in the Hypercore there is only 1 enable 
     * for both TX/RX 
     */
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    inv_shifter = (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num) +
                XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    data &= (1 << inv_shifter);
    *value = (data) ?  1 : 0;
  
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_prbs_tx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    return _phy_xgxs16g1l_control_prbs_tx_rx_enable_get(unit, port, value);
}

STATIC int
_phy_xgxs16g1l_control_prbs_rx_enable_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      lane_mask = 1 << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT * pc->lane_num);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    if (value) {
        data |= lane_mask <<  XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    }  else {
        data &= ~(lane_mask << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
    }
    
    /* write back updated configuration */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));
    
    /* Set up status collection */
    /* uPrbsRxControlReg = 0x80b1;
     * uPrbsRxControlReg += uLaneAddr * 0x10;
     */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), &data));
    
    data &= ~RX0_RX_CONTROL_STATUS_SEL_MASK;
    if (value) {
        data |= RX0_RX_CONTROL_STATUS_SEL_prbsStatus;
    } else {
        data &= ~RX0_RX_CONTROL_STATUS_SEL_prbsStatus;
    }
    
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), data));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_prbs_rx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    return _phy_xgxs16g1l_control_prbs_tx_rx_enable_get(unit, port, value);
}

/*
 * Returns value 
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
STATIC int
_phy_xgxs16g1l_control_xgxs_prbs_rx_status_get(int unit, soc_port_t port, 
                                          uint32 *value)
{
  /*int         lane;*/
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    
    /* Get status for one of the 4 lanes and check for sync
     * 0x80b0, 0x80c0, 0x80d0, 0x80e0 
     */
    *value = 0;

    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_READ(unit, pc, 0x0, 0x80b0 + (0x10 * pc->lane_num), &data));
    if (data == (RX0_RX_STATUS_PRBS_STATUS_PRBS_LOCK_BITS  <<
		 RX0_RX_STATUS_PRBS_STATUS_PRBS_LOCK_SHIFT)) {
        /* PRBS is in sync */
    } else if (data == 0) {
        /* PRBS not in sync */
        *value = -1;            
    } else {
        /* Get errors */
        *value += data & RX0_RX_STATUS_PRBS_STATUS_PTBS_ERRORS_MASK;
    }
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g1l_control_prbs_rx_status_get(int unit, soc_port_t port, uint32 *value)
{
    int         rv;
    uint16      rx_sts, saved_rx_sts;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* Set up prbs rx status collection */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), &rx_sts));
    saved_rx_sts = rx_sts;
    rx_sts = (rx_sts & ~RX0_RX_CONTROL_STATUS_SEL_MASK) |
             RX0_RX_CONTROL_STATUS_SEL_prbsStatus;
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), rx_sts));

    rv = _phy_xgxs16g1l_control_xgxs_prbs_rx_status_get(unit, port, value);


    /* Restore rx status */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), saved_rx_sts));

    return rv;
}

STATIC int
_phy_xgxs16g1l_control_vco_disturbed_get(int unit, soc_port_t port, uint32 *value)
{
    uint16 data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    if(!pc) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(READ_XGXS16G_SERDESDIGITAL_PATGENCTRLr(unit, pc, &data));
    SOC_IF_ERROR_RETURN(MODIFY_XGXS16G_SERDESDIGITAL_PATGENCTRLr(unit, pc, 0,  SERDESDIGITAL_PATGENCTRL_PKT_SIZE_MASK));

    *value = (data & SERDESDIGITAL_PATGENCTRL_PKT_SIZE_MASK) ? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g1l_control_vco_disturbed_set(int unit, soc_port_t port)
{
    uint16 data, mask;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    if(!pc) {
        return SOC_E_NONE;
    }
    data = 1 << SERDESDIGITAL_PATGENCTRL_PKT_SIZE_SHIFT;
    mask = SERDESDIGITAL_PATGENCTRL_PKT_SIZE_MASK;

    SOC_IF_ERROR_RETURN(MODIFY_XGXS16G_SERDESDIGITAL_PATGENCTRLr(unit, pc, data, mask));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_xgxs16g1l_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_xgxs16g1l_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_xgxs16g1l_control_tx_driver_set(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_EQUALIZER_BOOST:
        rv = MODIFY_XGXS16G_RX0_RX_ANALOGBIAS0r(unit, pc, value,
                                  RX_ALL_RX_EQ_BOOST_EQUALIZER_CONTROL_MASK);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_xgxs16g1l_control_linkdown_transmit_set(unit, port, value);
        break;

    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_xgxs16g1l_control_prbs_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_xgxs16g1l_control_prbs_tx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = _phy_xgxs16g1l_control_prbs_tx_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_xgxs16g1l_control_prbs_rx_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc,
            value? SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK:0,              SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        SOC_IF_ERROR_RETURN(
                MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc,
                value ? SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK : 0,
                SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK));
        rv = SOC_E_NONE;
        break; 
    default:
        rv = (SOC_E_UNAVAIL);
        break;
    }
    return rv;
}

STATIC int
_phy_xgxs16g1l_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */

    /* Read preemphasis/driver/pre-driver current */
    if (XGXS16G_13G_ID(pc) || XGXS16G_2p5G_ID(pc)){
        switch(pc->lane_num){
        case 0:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS13G_040_TX0_ACONTROL1r(unit, pc, &data16));
             break;
        case 1:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS13G_040_TX1_ACONTROL1r(unit, pc, &data16));
             break;
        case 2:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS13G_040_TX2_ACONTROL1r(unit, pc, &data16));
             break;
        case 3:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS13G_040_TX3_ACONTROL1r(unit, pc, &data16));
             break;
        default:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS13G_040_TX_ALL_ACONTROL1r(unit, pc, &data16));
             break;
        }
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             *value = (data16 & TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_MASK) >>
                      TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_SHIFT;
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             *value = (data16 & TX_ALL_040_TX_DRIVER_IDRIVER_MASK) >>
                      TX_ALL_040_TX_DRIVER_IDRIVER_SHIFT;
             break;
        default:
             /* should never get here */
             return (SOC_E_PARAM);
        }
    } else {
        switch(pc->lane_num){
        case 0:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS16G_TX0_TX_DRIVERr(unit, pc, &data16));
             break;
        case 1:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS16G_TX1_TX_DRIVERr(unit, pc, &data16));
             break;
        case 2:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS16G_TX2_TX_DRIVERr(unit, pc, &data16));
             break;
        case 3:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS16G_TX3_TX_DRIVERr(unit, pc, &data16));
             break;
        default:
             SOC_IF_ERROR_RETURN
                 (READ_XGXS16G_TX_ALL_TX_DRIVERr(unit, pc, &data16));
             break;
        }
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             *value = (data16 & TX_ALL_TX_DRIVER_PREEMPHASIS_MASK) >>
                      TX_ALL_TX_DRIVER_PREEMPHASIS_SHIFT;
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
              *value = (data16 & TX_ALL_TX_DRIVER_IDRIVER_MASK) >>
                       TX_ALL_TX_DRIVER_IDRIVER_SHIFT;
              break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
              *value = (data16 & TX_ALL_TX_DRIVER_IPREDRIVER_MASK) >>
                       TX_ALL_TX_DRIVER_IPREDRIVER_SHIFT;
              break;
        default:
              /* should never get here */
              return (SOC_E_PARAM);
        }
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_control_get
 * Purpose:
 *      Get current control settign of the PHY. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      type  - Control to update 
 *      value - (OUT)Current setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_xgxs16g1l_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
    uint16 data16;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_xgxs16g1l_control_tx_driver_get(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_EQUALIZER_BOOST:
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_RX0_RX_ANALOGBIAS0r(unit, pc, &data16));
        *value = data16 & RX_ALL_RX_EQ_BOOST_EQUALIZER_CONTROL_MASK;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_xgxs16g1l_control_linkdown_transmit_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_xgxs16g1l_control_prbs_polynomial_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_xgxs16g1l_control_prbs_tx_invert_data_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = _phy_xgxs16g1l_control_prbs_tx_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_xgxs16g1l_control_prbs_rx_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = _phy_xgxs16g1l_control_prbs_rx_status_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc,&data16));
        *value =
           data16 & SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK?
             TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, &data16));
        * value =
            data16 & SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK?
            TRUE : FALSE;
        rv = SOC_E_NONE;
        break; 
    case SOC_PHY_CONTROL_VCO_DISTURBED:
         rv = _phy_xgxs16g1l_control_vco_disturbed_get(unit, port, value);
         break;
    default:
        rv = (SOC_E_UNAVAIL);
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_xgxs16g1l_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_xgxs16g1l_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_read(unit, pc, phy_reg_addr, &data));

   *phy_data = data;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_xgxs16g1l_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_write(unit, pc, phy_reg_addr, data));

    return (SOC_E_NONE);
}  

/*
 * Function:
 *      phy_xgxs16g1l_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_xgxs16g1l_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_modify(unit, pc, phy_reg_addr, data, mask));

    return (SOC_E_NONE);
}

STATIC int
phy_xgxs16g1l_probe(int unit, phy_ctrl_t *pc)
{
    uint16      serdes_id0;
    uint16      serdes_id2;
    soc_info_t *si;
    int phy_port;  /* physical port number */

    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[pc->port];
    } else {
        phy_port = pc->port;
    }

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID0r(unit, pc, &serdes_id0));
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID2r(unit, pc, &serdes_id2));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_probe u=%d p=%d: id0 = 0x%x, id1 = 0x%x\n"),
              unit, pc->port, serdes_id0,serdes_id2));

    if (((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) ==
        SERDES_ID0_MODEL_NUMBER_XGXS_16G) && 
           (serdes_id2 & SERDESID_SERDESID2_DR_16G_4L_MASK)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "_phy_xgxs16g_probe u=%d p=%d: xgxs16g\n"),
                  unit, pc->port));
        /* XGXS16G */
        pc->size = sizeof(XGXS16G_DEV_DESC);
        return SOC_E_NONE;
    }

    if (((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == 
        SERDES_ID0_MODEL_NUMBER_XGXS_16G) && 
        (serdes_id2 & SERDESID_SERDESID2_DR_13G_4L_MASK)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "_phy_xgxs16g_probe u=%d p=%d: xgxs13g\n"),
                  unit, pc->port));
        /* XGXS13G new 40nm serdes */
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));
        if (SOC_IS_TRIUMPH3(unit)) {
            pc->lane_num = pc->lane_num % 4;
        } else if (pc->lane_num >= 4) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_xgxs16g_probe u=%d p=%d: wrong lane# 0x%x, set to 0\n"),
                      unit, pc->port, pc->lane_num));
            pc->lane_num = 0;
        }

        pc->dev_name = "XGXS13G1l";

        pc->size = sizeof(XGXS16G_DEV_DESC);
        return SOC_E_NONE;
    }

    if (((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == 0xf) && 
        (serdes_id2 & SERDESID_SERDESID2_DR_2P5G_SL_MASK)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "_phy_xgxs16g_probe u=%d p=%d: qs40\n"),
                  unit, pc->port));
        /* quad SGMII new 40nm serdes */
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));
        pc->chip_num *= 2;
        pc->chip_num += pc->lane_num/4;
        pc->lane_num %= 4;

        if (SOC_IS_TRIUMPH3(unit)) {
            pc->lane_num = pc->lane_num % 4;
        } else if (pc->lane_num >= 4) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_xgxs16g_probe u=%d p=%d: wrong lane# 0x%x, set to 0\n"),
                      unit, pc->port, pc->lane_num));
            pc->lane_num = 0;
        }
        pc->dev_name = "QS40";

        pc->size = sizeof(XGXS16G_DEV_DESC);
        return SOC_E_NONE;
    }

    return SOC_E_NOT_FOUND;
}

STATIC int
phy_xgxs16g1l_notify(int unit, soc_port_t port,
                 soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_xgxs16g1l_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_xgxs16g1l_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_xgxs16g1l_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_xgxs16g1l_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_xgxs16g1l_notify_resume(unit, port, value));
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_xgxs16g1l_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_xgxs16g1l_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16       data16;
    phy_ctrl_t  *pc;
                                                                                
    pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_GP_STATUS_STATUS1000X1r(unit, pc, &data16));

    if (!(data16 & GP_STATUS_STATUS1000X1_SGMII_MODE_MASK)) {
        /* 1000X fiber mode, always full duplex */
        duplex = TRUE;
    }

    data16 = duplex? MII_CTRL_FD: 0;
 
    /* program the duplex setting */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, data16,MII_CTRL_FD));
                                                                                
    return SOC_E_NONE;
}

STATIC int
phy_xgxs16g1l_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;
                                                                               
    pc = INT_PHY_SW_STATE(unit, port);
                                                                               
    *duplex = TRUE;
                                                                               
    SOC_IF_ERROR_RETURN(READ_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, &reg0_16));

    if (!(reg0_16 & SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK)) {
                                                                               
    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
                                                                               
        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_COMBO_IEEE0_AUTONEGLPABILr(unit,pc,&reg0_16));
                                                                               
            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & MII_ANP_SGMII_MODE) {
                if (reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }
                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }
                                                                               
    return SOC_E_NONE;
}

/*
 * Variable:
 *      phy_xgxs16g1l_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_xgxs16g1l_ge = {
    /* .drv_name                      = */ "XGXS16G/1 Unicore PHY Driver",
    /* .pd_init                       = */ phy_xgxs16g1l_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_xgxs16g1l_link_get,
    /* .pd_enable_set                 = */ phy_xgxs16g1l_enable_set,
    /* .pd_enable_get                 = */ phy_null_enable_get,
    /* .pd_duplex_set                 = */ phy_xgxs16g1l_duplex_set,
    /* .pd_duplex_get                 = */ phy_xgxs16g1l_duplex_get,
    /* .pd_speed_set                  = */ phy_xgxs16g1l_speed_set,
    /* .pd_speed_get                  = */ phy_xgxs16g1l_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_xgxs16g1l_master_get,
    /* .pd_an_set                     = */ phy_xgxs16g1l_an_set,
    /* .pd_an_get                     = */ phy_xgxs16g1l_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_xgxs16g1l_lb_set,
    /* .pd_lb_get                     = */ phy_xgxs16g1l_lb_get,
    /* .pd_interface_set              = */ phy_null_interface_set,
    /* .pd_interface_get              = */ phy_xgxs16g1l_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */ 
    /* .pd_linkup_evt                 = */ NULL,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ phy_xgxs16g1l_medium_config_set,
    /* .pd_medium_config_get          = */ NULL,
    /* .pd_medium_get                 = */ phy_xgxs16g1l_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_xgxs16g1l_control_set,
    /* .pd_control_get                = */ phy_xgxs16g1l_control_get,
    /* .pd_reg_read                   = */ phy_xgxs16g1l_reg_read,
    /* .pd_reg_write                  = */ phy_xgxs16g1l_reg_write,
    /* .pd_reg_modify                 = */ phy_xgxs16g1l_reg_modify,
    /* .pd_notify                     = */ phy_xgxs16g1l_notify,
    /* .pd_probe                      = */ phy_xgxs16g1l_probe,
    /* .pd_ability_advert_set         = */ phy_xgxs16g1l_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_xgxs16g1l_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_xgxs16g1l_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_xgxs16g1l_ability_local_get
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      phy_xgxs16g1l_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) 56xxx is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in SGMII mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */
STATIC int
_phy_xgxs16g1l_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
                                                                               
    fiber = PHY_FIBER_MODE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g1l_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));
                                                                               
    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
                                                                               
    /* Put SERDES PHY in reset */
                                                                               
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g1l_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    SOC_IF_ERROR_RETURN
        (phy_xgxs16g1l_duplex_set(unit,port,duplex));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g1l_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));
                                                                               
    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_xgxs16g1l_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g1l_notify_speed
 * Purpose:
 *      Program duplex if (and only if) 56xxx is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Speed to program.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_xgxs16g1l_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g1l_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g1l_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g1l_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {        SOC_IF_ERROR_RETURN
            (phy_xgxs16g1l_an_set(unit, port, FALSE));
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      _phy_xgxs16g1l_stop
 * Purpose:
 *      Put 56xxx SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_xgxs16g1l_stop(int unit, soc_port_t port)
{
    uint16 mask16,data16;
    int          stop, copper;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    copper = (pc->stop &
              PHY_STOP_COPPER) != 0;
    stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g1l_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mask16 = (1 << pc->lane_num);    /* rx lane */
    mask16 |= (mask16 << 4); /* add tx lane */
                                                                               
    if (stop) {
        mask16 |= 0x800;
        data16 = mask16;
    } else {
        data16 = 0;
    }
                                                                               
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK1_LANECTRL3r(unit,pc,data16,mask16));
                                                                               
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g1l_notify_stop
 * Purpose:
 *      Add a reason to put 56xxx PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_xgxs16g1l_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop |= flags;

    return _phy_xgxs16g1l_stop(unit, port);
}

/*
 * Function:
 *      phy_xgxs16g1l_notify_resume
 * Purpose:
 *      Remove a reason to put 56xxx PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_xgxs16g1l_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop &= ~flags;

    return _phy_xgxs16g1l_stop(unit, port);
}


STATIC int
_phy_xgxs16g1l_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = (intf == SOC_PORT_IF_SGMII) ? 
                 0 : SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;

    /* Disable 1000X/SGMII auto detect and force to 1000X.
     * 1000X mode is not supported in this driver.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, data16,
                      SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g1l_master_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_xgxs16g1l_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_xgxs16g1l_interface_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      Interface mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_xgxs16g1l_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
   int speed;
   SOC_IF_ERROR_RETURN(phy_xgxs16g1l_speed_get(unit, port, &speed));
   switch(speed)
   {
   case 10:
   case 100:
        *pif = SOC_PORT_IF_MII;
        break;
   case 1000:
   case 2500:
   default:
        *pif = SOC_PORT_IF_GMII;
        break;
   }
   return SOC_E_NONE;
}

/*
 * Function:
 * phy_xgxs16g1l_medium_config_set
 * Purpose:
 * set the configured medium the device is operating on.
 * Parameters:
 * unit - StrataSwitch unit #
 * port - Port number
 * medium - SOC_PORT_MEDIUM_COPPER/FIBER
 * cfg - not used
 * Returns:
 * SOC_E_XXX
 */
STATIC int
phy_xgxs16g1l_medium_config_set(int unit, soc_port_t port, soc_port_medium_t medium,  soc_phy_config_t *cfg)
{
    phy_ctrl_t *pc;
    uint16 fiber_mode = 0;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        fiber_mode = 0;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        fiber_mode = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
        break;
    default:
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, fiber_mode,
                            SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));

    if (medium == SOC_PORT_MEDIUM_FIBER) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
    } else  {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * phy_xgxs16g1l_medium_get
 * Purpose:
 * get the configured medium the device is operating on.
 * Parameters:
 * unit - StrataSwitch unit #
 * port - Port number
 * medium - SOC_PORT_MEDIUM_COPPER/FIBER
 * cfg - not used
 * Returns:
 * SOC_E_XXX
 */
STATIC int
phy_xgxs16g1l_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    phy_ctrl_t *pc;
    uint16 data16 = 0;

    if (NULL == medium) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(READ_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, &data16));

    *medium = data16 & SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK ? SOC_PORT_MEDIUM_FIBER : SOC_PORT_MEDIUM_COPPER;

    return SOC_E_NONE;
}

#ifdef BROADCOM_DEBUG
int _phy_xgxs16g1l_cfg_dump(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    soc_phy_info_t *pi;
    XGXS16G_DEV_DESC *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pCfg = (XGXS16G_DEV_DESC *)(pc + 1);
    pi = &SOC_PHY_INFO(unit, port);

    LOG_CLI((BSL_META_U(unit,
                        "serdesid0    0x%04x\n"), pCfg->serdes_id0));
    LOG_CLI((BSL_META_U(unit,
                        "serdesid2      0x%04x\n"),
             pCfg->serdes_id2));
    LOG_CLI((BSL_META_U(unit,
                        "chip_num     0x%04x\n"), pc->chip_num));
    LOG_CLI((BSL_META_U(unit,
                        "lane_num     0x%04x\n"), pc->lane_num));
    LOG_CLI((BSL_META_U(unit,
                        "speedMax     0x%04x\n"), pc->speed_max));
    LOG_CLI((BSL_META_U(unit,
                        "pc->flags    0x%04x\n"), pc->flags));
    LOG_CLI((BSL_META_U(unit,
                        "pc->stop     0x%04x\n"), pc->stop));
    LOG_CLI((BSL_META_U(unit,
                        "pi->phy_flags   0x%04x\n"), pi->phy_flags));
    return 0;
}
#endif

#else
typedef int _phy_xgxs16g1l_not_empty; /* Make ISO compilers happy. */
#endif /*  INCLUDE_XGXS_16G */

