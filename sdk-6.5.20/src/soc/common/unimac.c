/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS 10/100/1000/2500 Megabit Media Access Controller Driver (unimac)
 *
 * This module is used for:
 *
 *   -  Gigabit ports on some XGS3 systems.
 */

#include <shared/bsl.h>

#if defined(PHYMOD_SUPPORT)
#include <soc/phyreg.h>
#include <soc/phy/phyctrl.h>
#include <phymod/phymod.h>
#endif

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/error.h>
#include <soc/portmode.h>
#include <soc/macutil.h>
#include <soc/phyctrl.h>
#if defined(BCM_ESW_SUPPORT) && defined(BCM_HURRICANE4_SUPPORT)
#include <bcm_int/esw/port.h>
#endif


#ifdef BCM_UNIMAC_SUPPORT

#define JUMBO_MAXSZ  0x3fe8

/*
 * Forward Declarations
 */
mac_driver_t soc_mac_uni;

#define SOC_UNIMAC_SPEED_10     0x0
#define SOC_UNIMAC_SPEED_100    0x1
#define SOC_UNIMAC_SPEED_1000   0x2
#define SOC_UNIMAC_SPEED_2500   0x3

/* EEE related defination */
#define SOC_UNIMAC_MAX_EEE_SPEED    1000

/* RX DISABLE TIME */
#define SOC_UNIMAC_DISABLE_RX_TIME  20

/* Config value of LCCDRAIN_TIMEOUT_USEC. Default is 250000 usec. */
STATIC int lcc_drain_timeout[SOC_MAX_NUM_DEVICES];

#ifdef BROADCOM_DEBUG
static char *mac_uni_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *mac_uni_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

/*
 * Function:
 *      _mac_uni_drain_cells
 * Purpose:
 *      Wait until MMU cell count reaches zero for a specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number being examined.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Keeps polling as long as LCCCOUNT/GCCCOUNT is decrementing.
 *      If LCCCOUNT/GCCCOUNT stops decrementing without reaching zero,
 *      after a timeout period a warning message is printed
 *      and it gives up.
 *
 *      Before calling this routine, make sure no packets are switching
 *      to the port or it might never finish draining.
 */

STATIC int
_mac_uni_drain_cells(int unit, soc_port_t port)
{
    int                 rv;
    uint32              flush_ctrl;
    int                 pause_tx, pause_rx;
#ifdef BCM_GREYHOUND_SUPPORT
    uint32              mmu_fc = 0;
#endif
    rv  = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return rv;
    }
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
	/* Drop out all packets in TX FIFO without egressing any packets */
	SOC_IF_ERROR_RETURN
	    (READ_FLUSH_CONTROLr(unit, port, &flush_ctrl));
	soc_reg_field_set(unit, FLUSH_CONTROLr, &flush_ctrl, FLUSHf, 1);
	SOC_IF_ERROR_RETURN
	    (WRITE_FLUSH_CONTROLr(unit, port, flush_ctrl));
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d unimac TX fifo FLUSH_CONTROL.FLUSH set to 1\n"), port));
    }
#endif /* BCM_KATANA_SUPPORT */

    /* First put the port in flush state - the packets from the XQ of the 
     * port are purged after dequeue. */
    SOC_IF_ERROR_RETURN
        (soc_mmu_flush_enable(unit, port, TRUE));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d unimac mmu flush enable completed\n"), port));
#ifdef BCM_GREYHOUND_SUPPORT
    /* clear the MMU pause state*/
    if (SOC_REG_IS_VALID(unit, MMU_FC_RX_ENr)) {
        SOC_IF_ERROR_RETURN(
            READ_MMU_FC_RX_ENr(unit, port, &mmu_fc));
        SOC_IF_ERROR_RETURN(
            WRITE_MMU_FC_RX_ENr(unit,port,0));
    }
#endif
    /* Disable pause function */
    SOC_IF_ERROR_RETURN
        (soc_mac_uni.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_uni.md_pause_set(unit, port, 0, 0));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d unimac saved pause and pfc state\n"), port));

    if (!SOC_IS_KATANA(unit)) {
	/* Drop out all packets in TX FIFO without egressing any packets */
	SOC_IF_ERROR_RETURN
	    (READ_FLUSH_CONTROLr(unit, port, &flush_ctrl));
	soc_reg_field_set(unit, FLUSH_CONTROLr, &flush_ctrl, FLUSHf, 1);
	SOC_IF_ERROR_RETURN
	    (WRITE_FLUSH_CONTROLr(unit, port, flush_ctrl));
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d unimac TX fifo FLUSH_CONTROL.FLUSH set to 1\n"), port));
    }
    /* Notify PHY driver */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, 
                            phyEventStop, PHY_STOP_DRAIN));

    /* Disable switch egress metering so that packet draining is not rate
     * limited.
     */
    rv = soc_egress_drain_cells(unit, port, lcc_drain_timeout[unit]);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                           "port %d unimac egress packet draining completed\n"),
                           port));
    }

    /* Notify PHY driver */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, 
                            phyEventResume, PHY_STOP_DRAIN));

    /* Soft-reset is recommended here. 
     * SOC_IF_ERROR_RETURN
     *     (soc_mac_uni.md_control_set(unit, port, SOC_MAC_CONTROL_SW_RESET,
     *                                 TRUE));
     * SOC_IF_ERROR_RETURN
     *     (soc_mac_uni.md_control_set(unit, port, SOC_MAC_CONTROL_SW_RESET,
     *                                 FALSE));
     */
  
    /* Bring the TxFifo out of flush */
    soc_reg_field_set(unit, FLUSH_CONTROLr, &flush_ctrl, FLUSHf, 0);
    SOC_IF_ERROR_RETURN
        (WRITE_FLUSH_CONTROLr(unit, port, flush_ctrl));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d unimac TX fifo FLUSH_CONTROL.FLUSH set to 0\n"), port));


#ifdef BCM_GREYHOUND_SUPPORT
    /* restore the MMU pause state*/
    if (SOC_REG_IS_VALID(unit, MMU_FC_RX_ENr)) {
        SOC_IF_ERROR_RETURN(
            WRITE_MMU_FC_RX_ENr(unit, port, mmu_fc));
    }
#endif

    /* Restore original pause configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_uni.md_pause_set(unit, port, pause_tx, pause_rx));

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "KATANA port %d mmu flush disable will be invoked during port re-enable"),
                      port));
    } else
#endif
    {
        /* Bring the switch MMU out of flush */
        SOC_IF_ERROR_RETURN
           (soc_mmu_flush_enable(unit, port, FALSE));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d unimac mmu flush disabled\n"), port));
    }
    return rv;
}

/*
 * Function:
 *      mac_uni_init
 * Purpose:
 *      Initialize UniMAC into a known good state.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The initialization speed/duplex is arbitrary and must be
 *      updated by linkscan before enabling the MAC.
 */

STATIC  int
mac_uni_init(int unit, soc_port_t port)
{
    uint32              command_config, ocommand_config, speed, rval32;
    int                 frame_max;
    int                 ignore_pause;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    /* Get MAC configurations from config settings. */
    lcc_drain_timeout[unit] = soc_property_get(unit, spn_LCCDRAIN_TIMEOUT_USEC, 
                                         250000);
    SOC_IF_ERROR_RETURN
        (soc_port_blk_init(unit, port));

    frame_max = IS_ST_PORT(unit, port) ? JUMBO_MAXSZ : 1518;

    soc_mac_uni.md_frame_max_set(unit, port, frame_max);

    /* First put the MAC in reset and sleep */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   TRUE));

    /* Do the initialization */
    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
    speed = IS_FE_PORT(unit, port) ? SOC_UNIMAC_SPEED_100 : SOC_UNIMAC_SPEED_1000;
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, ETH_SPEEDf, 
                      speed);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PROMIS_ENf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAD_ENf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CRC_FWDf, 1);
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAD_ENf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CRC_FWDf, 0);
        /* Quickfix - Enable external config for SGMII mode in shadow */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                          ENA_EXT_CONFIGf, 1);
    }
#endif
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_FWDf, 0);

    /* Ignore pause if using as stack port */
    ignore_pause = IS_ST_PORT(unit, port) ? 1 : 0;
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_IGNOREf,
                      ignore_pause);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, IGNORE_TX_PAUSEf,
                      ignore_pause);

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ADDR_INSf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, HD_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LOOP_ENAf, 0);
    
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CNTL_FRM_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, NO_LGTH_CHECKf,
                      1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LINE_LOOPBACKf,
                      0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ERR_DISCf, 0);
    if (soc_reg_field_valid(unit, COMMAND_CONFIGr, PRBL_ENAf)) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PRBL_ENAf, 0);
    }
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CNTL_FRM_ENAf, 1);

    /* For Hawkeye, let the bit ENA_EXT_CONFIG to stay the default value 
     * without changing by S/W.
     * It is same as unmanaged mode and suggested by design team during 
     * bringup.
     */
    if(!SOC_IS_HAWKEYE(unit)) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, 
                          ENA_EXT_CONFIGf,
                          PHY_SGMII_AUTONEG_MODE(unit, port) ? 1 : 0);
    }

    if (PHY_SGMII_AUTONEG_MODE(unit, port) &&
            soc_reg_field_valid(unit,COMMAND_CONFIGr,SW_OVERRIDE_RXf) &&
            soc_reg_field_valid(unit,COMMAND_CONFIGr,SW_OVERRIDE_TXf)) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                           SW_OVERRIDE_RXf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                           SW_OVERRIDE_TXf, 1);
    }

    if (ocommand_config != command_config) {
	SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    }

    /* Initialize mask for purging packet data received from the MAC */
    SOC_IF_ERROR_RETURN(soc_packet_purge_control_init(unit, port));

    /* Bring the UniMAC out of reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   FALSE));

    /* Pulse the Serdes AN if using auto_cfg mode */
    if (PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventAutoneg, 0));
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventAutoneg, 1));        
    }

    rval32 = 0;
    soc_reg_field_set(unit, PAUSE_CONTROLr, &rval32, ENABLEf, 1);

    if (soc_feature(unit, soc_feature_pause_control_update)) {
        soc_reg_field_set(unit, PAUSE_CONTROLr, &rval32, VALUEf, 0xc000);
    } else {
        soc_reg_field_set(unit, PAUSE_CONTROLr, &rval32, VALUEf, 0x1ffff);
    }

    SOC_IF_ERROR_RETURN(WRITE_PAUSE_CONTROLr(unit, port, rval32));

    SOC_IF_ERROR_RETURN(WRITE_PAUSE_QUANTr(unit, port, 0xffff));

    if (SOC_REG_IS_VALID(unit, MAC_PFC_REFRESH_CTRLr)) {
        soc_field_t fields[] = { PFC_REFRESH_ENf, PFC_REFRESH_TIMERf };
        uint32 values[] = { 1, 0xc000 };
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, MAC_PFC_REFRESH_CTRLr, port, 2,
                                     fields, values));
    }

    SOC_IF_ERROR_RETURN(WRITE_TX_IPG_LENGTHr(unit, port, 12));

    /* Set egress enable */
    SOC_IF_ERROR_RETURN
        (soc_egress_enable(unit, port, TRUE));

    /* assigning proper setting for EEE feature :
     * Note :
     *  - GE speed force assigned for timer setting 
     */
    if (soc_feature(unit, soc_feature_eee)) {
        SOC_IF_ERROR_RETURN(soc_port_eee_timers_init(unit, 
                port, SOC_UNIMAC_MAX_EEE_SPEED));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_enable_set(int unit, soc_port_t port, int enable)
{
    uint32              command_config, tx_ena, rx_ena;
    pbmp_t              mask;
    uint32 version, merge_config;
    int unimac_version;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_enable_set: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 enable ? "True" : "False"));

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    tx_ena = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, TX_ENAf);
    rx_ena = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);

    /* We have seen instances when there are a flurry of disable/enable calls 
       where the TX_ENA and RX_ENA are set, but the mask2 bit is clear for the port. 
       This resulted in the MAC not actually being enabled for the port. The logic 
       below ensures that we consider the membership of the mask2 bitmap before 
       returning early. */ 
    soc_link_mask2_get(unit, &mask); 
    if ((tx_ena == rx_ena) && 
        (tx_ena == (enable ? 1 : 0)) && 
        (enable ? SOC_PBMP_MEMBER(mask, port) : !SOC_PBMP_MEMBER(mask, port))) { 
        return SOC_E_NONE; 
    } 

#if defined(BCM_ESW_SUPPORT) && defined(BCM_HURRICANE4_SUPPORT)
    if (soc_feature(unit, soc_feature_egphy_port_leverage) &&
        soc_feature(unit, soc_feature_port_leverage) &&
        (BCM_ESW_PORT_DRV(unit)!= NULL) &&
        (BCM_ESW_PORT_DRV(unit)->port_enable_set != NULL)) {

        uint32 ctrl;
        int reset = 1;

        ctrl = command_config;

        soc_reg_field_set(unit, COMMAND_CONFIGr, &ctrl, TX_ENAf, enable ? 1: 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &ctrl, RX_ENAf, enable ? 1: 0);

        if (ctrl == command_config) {
            if (enable) {
                reset = 0;
            } else {
                if (soc_reg_field_get(unit, COMMAND_CONFIGr, ctrl, SW_RESETf)) {
                    reset = 0;
                }
            }
        }


        if (!enable) {
            soc_link_mask2_get(unit, &mask);
            SOC_PBMP_PORT_REMOVE(mask, port);
            SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        }

        /* RESET MAC only when state is changing */

        if (reset) {
            SOC_IF_ERROR_RETURN
                (BCM_ESW_PORT_DRV(unit)->port_enable_set(unit, port, enable));
        }

        if (enable) {
            soc_link_mask2_get(unit, &mask);
            SOC_PBMP_PORT_ADD(mask, port);
            SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        }

        return SOC_E_NONE;
    }
#endif /* BCM_ESW_SUPPORT && BCM_HURRICANE4_SUPPORT */


    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   TRUE));

    /* de-assert RX_ENA and TX_ENA */
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    sal_udelay(2);

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   FALSE));

    if (!enable) {
        if (soc_feature(unit, soc_feature_unimac_reset_wo_clock)) {
            SOC_IF_ERROR_RETURN
                (soc_pgw_rx_fifo_reset(unit, port, TRUE));
        }

        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        /*
         * Disable MMU port
         * for some devices where EPC_LINK can not block the SOBMH from cpu and
         * no mechanism is avalible to reset EDATABUF.
         */
        SOC_IF_ERROR_RETURN(soc_port_mmu_buffer_enable(unit, port, FALSE));

        SOC_IF_ERROR_RETURN(_mac_uni_drain_cells(unit, port));
        SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
						       SOC_MAC_CONTROL_SW_RESET,
						       TRUE));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventStop,
                                               PHY_STOP_MAC_DIS));
        /* Drop incoming packets from this port in the ingress pipeline */
        if (SOC_REG_IS_VALID(unit, PKT_DROP_ENABLEr) && 
            !(SOC_IS_SC_CQ(unit) && 
              (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_QGPORT)) &&
            !(SOC_IS_ENDURO(unit) && 
              (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_GPORT)) &&
            !(SOC_IS_HURRICANE(unit) && 
              (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_GPORT))) {
            SOC_IF_ERROR_RETURN(WRITE_PKT_DROP_ENABLEr(unit, port, 1));
        }

        /* Forces rx_lost_eop sequence */
        if (SOC_REG_IS_VALID(unit, UMAC_REV_IDr) &&
            SOC_REG_IS_VALID(unit, MAC_MERGE_CONFIGr)) {

            SOC_IF_ERROR_RETURN(READ_UMAC_REV_IDr(unit, port, &version));
            unimac_version =
                soc_reg_field_get(
                    unit, UMAC_REV_IDr, version, REVISION_ID_MAJORf);
            if (unimac_version == 4) {
                /* Set SW force */
                SOC_IF_ERROR_RETURN(
                    READ_MAC_MERGE_CONFIGr(unit, port, &merge_config));
                soc_reg_field_set(
                    unit, MAC_MERGE_CONFIGr, &merge_config,
                    SW_FORCE_RX_LOST_EOPf, 1);
                SOC_IF_ERROR_RETURN(
                    WRITE_MAC_MERGE_CONFIGr(unit, port, merge_config));

                /* Clear SW force */
                SOC_IF_ERROR_RETURN(
                    READ_MAC_MERGE_CONFIGr(unit, port, &merge_config));
                soc_reg_field_set(
                    unit, MAC_MERGE_CONFIGr, &merge_config,
                    SW_FORCE_RX_LOST_EOPf, 0);
                SOC_IF_ERROR_RETURN(
                    WRITE_MAC_MERGE_CONFIGr(unit, port, merge_config));
            }
        }
    } else {
        /* Stop the ingress pipeline from dropping packets from this port */
#if defined(BCM_KATANA_SUPPORT)
        if (SOC_IS_KATANA(unit)) {
            SOC_IF_ERROR_RETURN
              (soc_mmu_flush_enable(unit, port, FALSE));
        }
#endif
        if (SOC_REG_IS_VALID(unit, PKT_DROP_ENABLEr) && 
            !(SOC_IS_SC_CQ(unit) && 
              (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_QGPORT)) &&
            !(SOC_IS_ENDURO(unit) && 
              (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_GPORT)) &&
            !(SOC_IS_HURRICANE(unit) && 
              (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_GPORT))) {
            SOC_IF_ERROR_RETURN(WRITE_PKT_DROP_ENABLEr(unit, port, 0));
        }
#if defined(BCM_SCORPION_SUPPORT)
        if (soc_feature(unit, soc_feature_priority_flow_control)) {
            /* Flush MMU XOFF state with toggle bit */
            if (SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                SOC_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                          FORCE_PFC_XONf, 1));
                SOC_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                          FORCE_PFC_XONf, 0));
            }
        }
#endif /* BCM_SCORPION_SUPPORT */
        /* if it is to enable, assert RX_ENA and TX_ENA */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf,
                          0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 1);
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
        sal_udelay(2);

        /* Enable MMU port */
        SOC_IF_ERROR_RETURN(soc_port_mmu_buffer_enable(unit, port, TRUE));

        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
						       SOC_MAC_CONTROL_SW_RESET,
						       TRUE));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventResume,
                                               PHY_STOP_MAC_DIS));
        SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
						       SOC_MAC_CONTROL_SW_RESET,
						       FALSE));

        if (soc_feature(unit, soc_feature_unimac_reset_wo_clock)) {
            SOC_IF_ERROR_RETURN
                (soc_pgw_rx_fifo_reset(unit, port, FALSE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_enable_get
 * Purpose:
 *      Get UniMAC enable state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_enable_get(int unit, soc_port_t port, int *enable)
{
    uint32		command_config;

    SOC_IF_ERROR_RETURN
        (READ_COMMAND_CONFIGr(unit, port, &command_config));

    *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config,
                                RX_ENAf);

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_enable_get: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *enable ? "True" : "False"));
    return SOC_E_NONE;
}


#if defined(BCM_KATANA_SUPPORT)
/*
 * Function:
 *      _mac_uni_timestamp_delay_set
 * Purpose:
 *      Set Timestamp delay for one-step to account for lane and pipeline delay.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_uni_timestamp_delay_set(int unit, soc_port_t port, int speed)
{
    uint32 ctrl;
    int ts_adjust;
    uint32 field;
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_timestamp_delay_set: unit %d port %s speed %d\n"),
                 unit, SOC_PORT_NAME(unit, port), speed));
    /* 
     * KT-1379 : UniMAC Rx pipeline delay accounting on Tx path 
     * can cause issue for asymmetric paths/ speeds
     * UMAC_TIMESTAMP_ADJUST.AUTO_ADJUST programmed to 0. 
     * Per-ingress port EGR_1588_LINK_DELAY.LINK_DELAY registers.
     *      - MACRxPipelineDelay(1000M ingress speed) = 232ns
     *      - MACRxPipelineDelay(100M ingress speed) = 2320ns
     *      - MACRxPipelineDelay(10M ingress speed) = 23200ns
     */

    if (SOC_REG_IS_VALID(unit, UMAC_TIMESTAMP_ADJUSTr)) {
        ts_adjust = soc_property_port_get(unit, port, spn_TIMESTAMP_ADJUST(speed), 0);

        if (SOC_E_NONE != soc_reg_signed_field_mask(unit, UMAC_TIMESTAMP_ADJUSTr,
                                                    ADJUSTf, ts_adjust, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                                  "%s property out of bounds (is %d)\n"),
                                       spn_TIMESTAMP_ADJUST(speed), ts_adjust));
            field = 0;
        }
        SOC_IF_ERROR_RETURN(READ_UMAC_TIMESTAMP_ADJUSTr(unit, port, &ctrl));
        soc_reg_field_set(unit, UMAC_TIMESTAMP_ADJUSTr, &ctrl, 
                          AUTO_ADJUSTf, 0 );
        soc_reg_field_set(unit, UMAC_TIMESTAMP_ADJUSTr, &ctrl, 
                          ADJUSTf, ts_adjust);
        SOC_IF_ERROR_RETURN(WRITE_UMAC_TIMESTAMP_ADJUSTr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}
#endif /* defined(BCM_KATANA_SUPPORT) */

/*
 * Function:
 *      mac_uni_ifg_to_ipg
 * Description:
 *      Converts the inter-frame gap specified in bit-times into a value
 *      suitable to be programmed into the IPG register
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - Inter-frame gap in bit-times
 *      ipg    - (OUT) the value to be written into IPG
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the calculated IPG value will not cause
 *      hardware to fail. If the requested ifg value cannot be supported in
 *      hardware, the function will choose a value that approximates the
 *      requested value as best as possible.
 *
 *      Specifically:
 *         -- Current chips only support ifg which is divisible by 8. If
 *            the specified ifg is not divisible by 8, it will be rounded
 *            to the next multiplier of 8 (65 will result in 72).
 *         -- ifg < 64 are not supported
 *
 *      This function supports only GE portion of GE MAC (FE portion is
 *      supported by mac_fe_ipg_* functions
 */
STATIC int
mac_uni_ifg_to_ipg(int unit, soc_port_t port, int speed, int duplex,
                  int ifg, int *ipg)
{
    int         real_ifg;

    /* 
     * Silently adjust the specified ifp bits to valid value
     *  - valid value: 8 to 64 bytes (i.e. multiple of 8 bits)
     */
    real_ifg = ifg < 64 ? 64 : (ifg > 512 ? 512 : (ifg + 7) & (0x7f << 3));
    *ipg = real_ifg / 8;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_ipg_to_ifg
 * Description:
 *      Converts the IPG register value into the inter-frame gap expressed in
 *      bit-times
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ipg    - the value in the IPG register
 *      ifg    - Inter-frame gap in bit-times
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      This function supports only GE portion of GE MAC (FE portion is
 *      supported by mac_fe_ipg_* functions
 */
STATIC int
mac_uni_ipg_to_ifg(int unit, soc_port_t port, int speed, int duplex,
                  int ipg, int *ifg)
{
    /*
     * Now we need to convert the value according to various chips'
     * peculiarities (there are none as of now)
     */
    *ifg = ipg * 8;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_ipg_update
 * Purpose:
 *      Set the IPG appropriate for current duplex
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 * Notes:
 *      The current duplex is read from the hardware registers.
 */

STATIC int
mac_uni_ipg_update(int unit, int port)
{
    int                 fd, speed, ipg, ifg;
    uint32              rval;
    soc_ipg_t           *si = &SOC_PERSIST(unit)->ipg[port];

    SOC_IF_ERROR_RETURN(soc_mac_uni.md_duplex_get(unit, port, &fd));
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));

    if (fd) {
        switch (speed) {
	case 10:
            ifg = si->fd_10;
            break;
        case 100:
            ifg = si->fd_100;
            break;
        case 1000:
            ifg = si->fd_1000;
            break;
        case 2500:
            ifg = si->fd_2500;
            break;
        default:
            return SOC_E_INTERNAL;
            break;
        }
    } else {
        switch (speed) {
        case 10:
            ifg = si->hd_10;
            break;
        case 100:
            ifg = si->hd_100;
            break;
        default:
            return SOC_E_INTERNAL;
            break;
        }
    }

    /*
     * Convert the ifg value from bit-times into IPG register-specific value
     */
    SOC_IF_ERROR_RETURN(mac_uni_ifg_to_ipg(unit, port, speed, fd, ifg, &ipg));

    /*
     * Program the appropriate register
     */
    SOC_IF_ERROR_RETURN
        (WRITE_TX_IPG_LENGTHr(unit, port, ipg));

    /* Do not process any packets that have less than 8 bytes IPG */
    SOC_IF_ERROR_RETURN(READ_IPG_HD_BKP_CNTLr(unit, port, &rval));
    soc_reg_field_set(unit, IPG_HD_BKP_CNTLr, &rval, IPG_CONFIG_RXf,
                      speed > 1000 ? 5 : speed == 1000 ? 6 : 12);
    SOC_IF_ERROR_RETURN(WRITE_IPG_HD_BKP_CNTLr(unit, port, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_duplex_set
 * Purpose:
 *      Set UniMAC in the specified duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 */

STATIC int
mac_uni_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint32 	command_config, ocommand_config;
    int     speed;
    int     sw_reset = 1;
    int     rx_enable = 0;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));

    if (PHY_SGMII_AUTONEG_MODE(unit, port)) {
        return SOC_E_NONE;
    }
    
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));

    if (speed >= 1000) {
        /*
         * program the bit full-duplex for speed 1000 or 2500Mbps.
         */
        duplex = 1;
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, HD_ENAf, 
                      duplex ? 0 : 1);
    if (command_config == ocommand_config) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);
        sw_reset = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);

        /* Enable Flush control to drop all pkts */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 1));
        }

        /* Disable RX before reset MAC */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           FALSE));
            /* Update command_config */
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
            sal_udelay(SOC_UNIMAC_DISABLE_RX_TIME);
        }
    }

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   TRUE));

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
 
    /* Set IPG to match new duplex */
    SOC_IF_ERROR_RETURN(mac_uni_ipg_update(unit, port));

    /*
     * Notify internal PHY driver of duplex change in case it is being
     * used as pass-through to an external PHY.
     */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventDuplex, duplex));

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Disable Flush control */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 0));
        }
    }

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   FALSE));

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Restore RX_ENA */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           TRUE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_duplex_get
 * Purpose:
 *      Get UniMAC duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint32 command_config;
    int    speed;

    SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));

    if ((1000 == speed) || (2500 == speed)) {
        *duplex = TRUE;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_COMMAND_CONFIGr(unit, port, &command_config));

        *duplex = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                    HD_ENAf) ? FALSE : TRUE;
    }

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_speed_set
 * Purpose:
 *      Set UniMAC in the specified speed.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      speed - 10,100,1000, 2500 for speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 */

STATIC int
mac_uni_speed_set(int unit, soc_port_t port, int speed)
{
    uint32  command_config;
    int     speed_select;
    uint32  cur_speed;
    int     sw_reset = 1;
    int     rx_enable = 0;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_speed_set: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed));

    if (PHY_SGMII_AUTONEG_MODE(unit, port)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    cur_speed = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                  ETH_SPEEDf);

    switch (speed) {
    case 10:
        speed_select = SOC_UNIMAC_SPEED_10;
	break;
    /* support non-standard speed in Broadreach mode */
    case 20:
    case 25:
    case 33:
    case 50:
	/* fall through to case 100 */
    case 100:
        speed_select = SOC_UNIMAC_SPEED_100;
	break;
    case 1000:
        speed_select = SOC_UNIMAC_SPEED_1000;
        break;
    case 2500:
#ifdef BCM_HURRICANE4_SUPPORT
        /* Only upto 1G is supported */
        if (SOC_IS_HURRICANE4(unit)) {
            return (SOC_E_CONFIG);
        }
#endif
        speed_select = SOC_UNIMAC_SPEED_2500;
        break;
    case 0:
        return (SOC_E_NONE);              /* Support NULL PHY */            
    default:
        return (SOC_E_CONFIG);
    }

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, 
                      ETH_SPEEDf, speed_select);

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);
        sw_reset = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);

        /* Enable Flush control to drop all pkts */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 1));
        }

        /* Disable RX before reset MAC */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           FALSE));
            /* Update command_config */
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
            sal_udelay(SOC_UNIMAC_DISABLE_RX_TIME);
        }
    }

    /* First reset the MAC */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   TRUE));

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

    /*
     * Notify internal PHY driver of speed change in case it is being
     * used as pass-through to an external PHY.
     */
	if (! PHY_REPEATER(unit, port)) {
		SOC_IF_ERROR_RETURN
			(soc_phyctrl_notify(unit, port, phyEventSpeed, speed));
	}

     /* Set IPG to match new speed */
    SOC_IF_ERROR_RETURN(mac_uni_ipg_update(unit, port));

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Disable Flush control */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 0));
        }
    }

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   FALSE));

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Restore RX_ENA */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           TRUE));
        }
    }

    /* MAC speed switch results in a tx clock glitch to the serdes in 100fx mode.
     * reset serdes txfifo clears this condition. However this reset triggers
     * a link transition. Do not apply this reset if speed is already in 100M
     */
    if ((speed == 100) && (cur_speed != SOC_UNIMAC_SPEED_100)) {
        (void)soc_phyctrl_notify(unit, port, phyEventTxFifoReset, 100);
    }

#if defined(BCM_KATANA_SUPPORT)
    /* Set unimac timestamp delay */
    if (SOC_IS_KATANAX(unit)) {
        SOC_IF_ERROR_RETURN(_mac_uni_timestamp_delay_set(unit,port,speed));
    }
#endif

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit, port)) {
        /*
         * Update port_init_speed varaible
         * which is used during port enable
         */
        SOC_INFO(unit).port_init_speed[port] = speed;
    }
#endif
    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_uni_speed_get
 * Purpose:
 *      Get GE MAC speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      speed - (OUT) speed in Mb (10/100/1000)
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
mac_uni_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32 		command_config, uni_speed;
    int                 rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    uni_speed = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                  ETH_SPEEDf);

    switch(uni_speed) {
    case SOC_UNIMAC_SPEED_10:
	*speed = 10;
	break;
    case SOC_UNIMAC_SPEED_100:
	*speed = 100;
	break;
    case SOC_UNIMAC_SPEED_1000:
	*speed = 1000;
	break;
    case SOC_UNIMAC_SPEED_2500:
	*speed = 2500;
	break;
    default:
        rv = SOC_E_INTERNAL;
    }

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return(rv);
}

/*
 * Function:
 *      mac_uni_pause_set
 * Purpose:
 *      Configure UniMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_tx - Boolean: transmit pause, or -1 (don't change)
 *      pause_rx - Boolean: receive pause, or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_uni_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    uint32 		command_config, ocommand_config;
    int         sw_reset = 1;
    int         rx_enable = 0;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_pause_set: unit %d port %s RX=%s TX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_rx ? "on" : "off",
                 pause_tx ? "on" : "off"));

    if (pause_tx < 0 && pause_rx < 0) {
        return SOC_E_NONE;
    }

    if (IS_ST_PORT(unit, port)) {
        if ((pause_tx == 0) && (pause_rx == 0)) {
            return SOC_E_NONE;
        } else {
            /* Flow control cannot be enabled in HiGL mode */
            return SOC_E_CONFIG;
        }
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    if (pause_rx >= 0) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                            PAUSE_IGNOREf, pause_rx ? 0 : 1);
    }
    if (pause_tx >= 0) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                            IGNORE_TX_PAUSEf, pause_tx ? 0 : 1);
    }

    if (command_config == ocommand_config) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);
        sw_reset = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);

        /* Enable Flush control to drop all pkts */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 1));
        }

        /* Disable RX before reset MAC */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           FALSE));
            /* Update command_config */
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
            sal_udelay(SOC_UNIMAC_DISABLE_RX_TIME);
        }
    }

    /* First put the MAC in reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   TRUE));

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

    /* Add 2usec delay before deasserting SW_RESET */
    sal_udelay(2);

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Disable Flush control */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 0));
        }
    }

    /* Bring the MAC out of reset */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   FALSE));

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Restore RX_ENA */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           TRUE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_pause_get
 * Purpose:
 *      Return current GE MAC transmit/receive pause frame configuration.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_tx - (OUT) TRUE is TX pause enabled.
 *      pause_rx - (OUT) TRUE if obey RX pause enabled.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint32 		command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    *pause_rx = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                  PAUSE_IGNOREf) ?  FALSE : TRUE;
    *pause_tx = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                  IGNORE_TX_PAUSEf) ?  FALSE : TRUE;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_pause_get: unit %d port %s RX=%s TX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *pause_rx ? "on" : "off",
                 *pause_tx ? "on" : "off"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_pause_addr_set
 * Purpose:
 *      Set GE MAC source address for transmitted PAUSE frame
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_uni_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t pause_mac)
{
    uint32 mac_0, mac_1;

    mac_0 = pause_mac[0] << 24 |
            pause_mac[1] << 16 |
            pause_mac[2] << 8 |
            pause_mac[3] << 0;
    mac_1 = pause_mac[4] << 8 |
            pause_mac[5];

    SOC_IF_ERROR_RETURN(WRITE_MAC_0r(unit, port, mac_0));
    SOC_IF_ERROR_RETURN(WRITE_MAC_1r(unit, port, mac_1));
    
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_pause_addr_set: unit %d port %s "
                             "MAC=<%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_mac[0], pause_mac[1], pause_mac[2],
                 pause_mac[3], pause_mac[4], pause_mac[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_pause_addr_get
 * Purpose:
 *      Return current GE MAC source address for transmitted PAUSE frames
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_uni_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t pause_mac)
{
    uint32 mac_0, mac_1;

    SOC_IF_ERROR_RETURN(READ_MAC_0r(unit, port, &mac_0));
    SOC_IF_ERROR_RETURN(READ_MAC_1r(unit, port, &mac_1));

    pause_mac[0] = (uint8)(mac_0 >> 24);
    pause_mac[1] = (uint8)(mac_0 >> 16);
    pause_mac[2] = (uint8)(mac_0 >> 8);
    pause_mac[3] = (uint8)(mac_0 >> 0);
    pause_mac[4] = (uint8)(mac_1 >> 8);
    pause_mac[5] = (uint8)(mac_1 >> 0);

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_pause_addr_get: unit %d port %s "
                             "MAC=<%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_mac[0], pause_mac[1], pause_mac[2],
                 pause_mac[3], pause_mac[4], pause_mac[5]));
    return SOC_E_NONE;
}

#define QTC_PCS_LOOPBACK_REG_ADDR         0x9005
#define GH2_QSGMII_PHYSICAL_PORT_BASE_NUM 26
#define HR3_QSGMII_PHYSICAL_PORT_BASE_NUM 2

/*
 * Function:
 *      mac_uni_loopback_set
 * Purpose:
 *      Set GE MAC into/out-of loopback mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_loopback_set(int unit, soc_port_t port, int loopback)
{
    uint32      command_config, ocommand_config;
    int         phy_lb = 0;
    int         sw_reset = 1;
    int         rx_enable = 0;
#if defined(PHYMOD_SUPPORT) && defined(PHYMOD_QTCE_SUPPORT)
    int phy_port = 0, lane_num = 0;
    uint32 phy_data = 0;
    phy_ctrl_t *pc = NULL;
    phymod_phy_access_t *pm_phy = NULL;
    phymod_access_t *access = NULL;
    int is_qtc_Q_mode = 0;
#endif

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_loopback_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 loopback ? "True" : "False"));

#if defined(PHYMOD_SUPPORT) && defined(PHYMOD_QTCE_SUPPORT)
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc) {
        if (pc->phymod_ctrl.phy[0]) {
            pm_phy = &(pc->phymod_ctrl.phy[0]->pm_phy);
            if (pm_phy->type == phymodDispatchTypeQtce) {
                access = &(pc->phymod_ctrl.phy[0]->pm_phy.access);
                is_qtc_Q_mode = PHYMOD_ACC_F_QMODE_GET(access);
                if (is_qtc_Q_mode) {
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    if (SOC_IS_GREYHOUND2(unit)) {
                        lane_num = ((phy_port - GH2_QSGMII_PHYSICAL_PORT_BASE_NUM) % 16) >> 2;
                    } else if (SOC_IS_HURRICANE3(unit)) {
                        lane_num = ((phy_port - HR3_QSGMII_PHYSICAL_PORT_BASE_NUM) % 16) >> 2;
                    }
                }
            }
        }
    }
#endif

    /* The MAC loopback will not work when ENA_EXT_CONFIGf=1 because hardware puts MAC
     * in reset state if link is down.
     * A simple workaround is to put PHY in loopback mode to get the link. This brings
     * MAC out of reset state, and then clears ENA_EXT_CONFIGf.  SDK-28705
     */
    if (PHY_SGMII_AUTONEG_MODE(unit, port) && (!SOC_IS_HAWKEYE(unit))) {
#if defined(PHYMOD_SUPPORT) && defined(PHYMOD_QTCE_SUPPORT)
        if (is_qtc_Q_mode) {
            /* Enable PHY loopback */
            SOC_IF_ERROR_RETURN(soc_phyctrl_reg_read(unit, port, SOC_PHY_INTERNAL, QTC_PCS_LOOPBACK_REG_ADDR, &phy_data));
            SOC_IF_ERROR_RETURN(soc_phyctrl_reg_write(unit, port, SOC_PHY_INTERNAL, QTC_PCS_LOOPBACK_REG_ADDR, (phy_data | (1 << lane_num))));

            sal_usleep(40000);
            SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                              ENA_EXT_CONFIGf, loopback? 0 : 1);
            SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

            /* Retore PHY loopback */
            SOC_IF_ERROR_RETURN(soc_phyctrl_reg_write(unit, port, SOC_PHY_INTERNAL, QTC_PCS_LOOPBACK_REG_ADDR, phy_data));

        } else
#endif
        {
            SOC_IF_ERROR_RETURN(soc_phyctrl_loopback_get(unit, port, &phy_lb));

            SOC_IF_ERROR_RETURN(soc_phyctrl_loopback_set(unit,port,TRUE, TRUE));
            sal_usleep(40000);
            SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config,
                              ENA_EXT_CONFIGf, loopback? 0 : 1);
            SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
            SOC_IF_ERROR_RETURN(soc_phyctrl_loopback_set(unit,port,phy_lb, TRUE));
        }
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    ocommand_config = command_config;

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);
        sw_reset = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);

        /* Enable Flush control to drop all pkts */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 1));
        }

        /* Disable RX before reset MAC */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           FALSE));
            /* Update command_config */
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
            sal_udelay(SOC_UNIMAC_DISABLE_RX_TIME);
        }
    }

    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   TRUE));

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LOOP_ENAf, 
                      loopback ? 1 : 0);

    if (command_config != ocommand_config) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    }

#ifdef  BCM_GREYHOUND2_SUPPORT
#if defined(PHYMOD_SUPPORT) && defined(PHYMOD_QTCE_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit) && is_qtc_Q_mode && loopback) {
        /* GH2's QTCE ports need phy linkup signal to active MAC loopback */
        /* enable PHY loopback */
        SOC_IF_ERROR_RETURN(soc_phyctrl_reg_read(unit, port, SOC_PHY_INTERNAL, QTC_PCS_LOOPBACK_REG_ADDR, &phy_data));
        SOC_IF_ERROR_RETURN(soc_phyctrl_reg_write(unit, port, SOC_PHY_INTERNAL, QTC_PCS_LOOPBACK_REG_ADDR, (phy_data | (1 << lane_num))));

        sal_usleep(4000);

        /* Restore PHY loopback */
        SOC_IF_ERROR_RETURN(soc_phyctrl_reg_write(unit, port, SOC_PHY_INTERNAL, QTC_PCS_LOOPBACK_REG_ADDR, phy_data));
    }
#endif
#endif  /* BCM_GREYHOUND2_SUPPORT */

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Disable Flush control */
        if (!sw_reset) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 0));
        }
    }

    SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                   SOC_MAC_CONTROL_SW_RESET,
                                                   FALSE));

    if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
        /* Restore RX_ENA */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                           SOC_MAC_CONTROL_RX_SET,
                                                           TRUE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_loopback_get
 * Purpose:
 *      Get current GE MAC loopback mode setting.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_loopback_get(int unit, soc_port_t port, int *loopback)
{
    uint32 		command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    *loopback = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                  LOOP_ENAf);

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_loopback_get: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *loopback ? "True" : "False"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_ability_get
 * Purpose:
 *      Return the UniMAC abilities
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port # on unit.
 *      mode - (OUT) Mask of MAC abilities returned.
 * Returns:
 *      SOC_E_NONE
 */

STATIC  int
mac_uni_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (SOC_IS_SC_CQ(unit) && 
        (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_QGPORT)) {
        /* 5682x/72x GE-only ports are fixed at 1G speed */
        *mode = (SOC_PM_10MB_FD | SOC_PM_100MB_FD | SOC_PM_1000MB_FD |
                 SOC_PM_SGMII | SOC_PM_LB_MAC | SOC_PM_PAUSE);
    } else if (IS_ST_PORT(unit, port)) {
        *mode = (SOC_PM_1000MB_FD | SOC_PM_2500MB_FD | SOC_PM_LB_MAC |
                 SOC_PM_MII | SOC_PM_GMII);
                
    } else {
        *mode = (SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD |
                 SOC_PM_2500MB_FD |  SOC_PM_MII | SOC_PM_GMII |
                 SOC_PM_LB_MAC | SOC_PM_PAUSE);
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            *mode = *mode & ~(SOC_PM_2500MB_FD);
        }
#endif
    }

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_ability_get: unit %d port %s mode=0x%x\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *mode));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */

STATIC int
mac_uni_frame_max_set(int unit, soc_port_t port, int size)
{
    uint32 command_config, rx_ena;
    int speed = 0;
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_frame_max_set: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 size));

    if (IS_ST_PORT(unit, port)) {
        size += 16; /* Account for 16 bytes of Higig2 header */
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    rx_ena = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);

    /* If Rx is enabled then disable RX */
    if (rx_ena) {
        /* Disable RX */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_COMMAND_CONFIGr(unit, port, command_config));

        /* Wait for maximum frame receiption time(for 16K) based on speed */
        SOC_IF_ERROR_RETURN(soc_mac_uni.md_speed_get(unit, port, &speed));
        switch (speed) {
        case 2500:
            sal_usleep(55);
            break;
        case 1000:
            sal_usleep(131);
            break;
        case 100:
            sal_usleep(1310);
            break;
        case 10:
            sal_usleep(13100);
            break;
        default:
            break;
        }
    }

    SOC_IF_ERROR_RETURN(WRITE_FRM_LENGTHr(unit, port, size));

    /* if Rx was enabled before, restore it */
    if (rx_ena) {
        /* Enable RX */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_COMMAND_CONFIGr(unit, port, command_config));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size
 *      might be slightly higher.
 */
STATIC int
mac_uni_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint32  frame_length;
 
    SOC_IF_ERROR_RETURN(READ_FRM_LENGTHr(unit, port, &frame_length));

    *size = (int) frame_length;
    if (IS_ST_PORT(unit, port)) {
        *size -= 16;  /* Account for 16 bytes of Higig2 header */
    }

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_frame_max_get: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *size));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - Inter-frame gap in bit-times
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_uni_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t  duplex, int ifg)
{
    int         sw_reset = 1;
    int         rx_enable = 0;
    int         cur_speed = 0;
    int         cur_duplex;
    int         ipg, real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    uint32 	    command_config;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_ifg_set: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex == SOC_PORT_DUPLEX_FULL ? "Full" : "Half",
                 ifg));

    /* Get IPG, check range of IFG */
    SOC_IF_ERROR_RETURN(mac_uni_ifg_to_ipg(unit, port, speed, duplex,
                                          ifg, &ipg));
    SOC_IF_ERROR_RETURN(mac_uni_ipg_to_ifg(unit, port, speed, duplex,
                                          ipg, &real_ifg));

    if (duplex == SOC_PORT_DUPLEX_FULL) {
        if((speed != 10) && (speed != 100) &&
           (speed != 1000) && (speed != 2500)) {
            return SOC_E_PARAM;
        }
    } else {
        if((speed != 10) && (speed != 100)) {
            return SOC_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(mac_uni_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_uni_speed_get(unit, port, &cur_speed));

    if (cur_speed == speed &&
        cur_duplex == (duplex == SOC_PORT_DUPLEX_FULL ? TRUE : FALSE)) {

        if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
            SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
            rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);
            sw_reset = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);

            /* Enable Flush control to drop all pkts */
            if (!sw_reset) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 1));
            }

            /* Disable RX before reset MAC */
            if (rx_enable) {
                SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                               SOC_MAC_CONTROL_RX_SET,
                                                               FALSE));
                sal_udelay(SOC_UNIMAC_DISABLE_RX_TIME);
            }
        }

        /* First put the MAC in reset */
        SOC_IF_ERROR_RETURN
            (soc_mac_uni.md_control_set(unit, port, SOC_MAC_CONTROL_SW_RESET,
                                        TRUE));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TX_IPG_LENGTHr, port,
                                                   TX_IPG_LENGTHf, ipg));

        if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
            /* Disable Flush control */
            if (!sw_reset) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 0));
            }
        }

        /* Bring the MAC out of reset */
        SOC_IF_ERROR_RETURN
            (soc_mac_uni.md_control_set(unit, port, SOC_MAC_CONTROL_SW_RESET,
                                        FALSE));

        if (soc_feature(unit, soc_feature_unimac_no_rx_fifo_reset)) {
            /* Restore RX_ENA */
            if (rx_enable) {
                SOC_IF_ERROR_RETURN(soc_mac_uni.md_control_set(unit, port,
                                                               SOC_MAC_CONTROL_RX_SET,
                                                               TRUE));
            }
        }

        /* HW update success. Update the SI */
        if (duplex == SOC_PORT_DUPLEX_FULL) {
            switch (speed) {
                case 10:
                    si->fd_10 = real_ifg;
                    break;
                case 100:
                    si->fd_100 = real_ifg;
                    break;
                case 1000:
                    si->fd_1000 = real_ifg;
                    break;
                case 2500:
                    si->fd_2500 = real_ifg;
                    break;
                /* coverity[dead_error_begin] */
                default:
                    /* Should not reach */
                    break;
            }
        } else {
            switch (speed) {
                case 10:
                    si->hd_10 = real_ifg;
                    break;
                case 100:
                    si->hd_100 = real_ifg;
                    break;
                /* coverity[dead_error_begin] */
                default:
                    /* Should not reach */
                    break;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_ifg_get
 * Description:
 *      Gets the ifg (Inter-frame gap) value for a specific speed/duplex
 *      combination
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - Inter-frame gap in bit-times
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function returns the REAL ifg value that will be (or is currently)
 *      used by the chip, which might be different from the value initially
 *      set by the ifg_set() call. The reason for that is that only certain
 *      values are allowed.
 */
STATIC int
mac_uni_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t  duplex, int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];

    if (duplex == SOC_PORT_DUPLEX_FULL) {
        switch (speed) {
        case 10:
            *ifg = si->fd_10;
            break;
        case 100:
            *ifg = si->fd_100;
            break;
        case 1000:
            *ifg = si->fd_1000;
            break;
        case 2500:
            *ifg = si->fd_2500;
            break;
        default:
            return SOC_E_PARAM;
            break;
        }
    } else {
        switch (speed) {
        case 10:
            *ifg = si->hd_10;
            break;
        case 100:
            *ifg = si->hd_100;
            break;
       default:
            return SOC_E_PARAM;
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_ifg_get: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex == SOC_PORT_DUPLEX_FULL ? "Full" : "Half",
                 *ifg));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_interface_set
 * Purpose:
 *      Set GE MAC interface type
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_interface_set: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_uni_port_if_names[pif]));
#endif

    switch (pif) {
    case SOC_PORT_IF_MII:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_SGMII:
        return SOC_E_NONE;
    default:
        return SOC_E_UNAVAIL;
    }
}

/*
 * Function:
 *      mac_uni_interface_get
 * Purpose:
 *      Retrieve GE MAC interface type
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_uni_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_GMII;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_interface_get: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_uni_port_if_names[*pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_encap_set
 * Purpose:
 *      Set the port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - encapsulation mode
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_uni_encap_set(int unit, soc_port_t port, int mode)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_encap_set: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_uni_encap_mode[mode]));
#endif

    if (mode == SOC_ENCAP_IEEE) {
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      mac_uni_encap_get
 * Purpose:
 *      Get the port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encapsulation mode
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_uni_encap_get(int unit, soc_port_t port, int *mode)
{
    *mode = SOC_ENCAP_IEEE;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_encap_get: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_uni_encap_mode[*mode]));
#endif
    return SOC_E_NONE;
}

STATIC int
_mac_uni_sw_reset(int unit, soc_port_t port, int reset_assert)
{
    uint32 command_config;
    int reset_sleep_usec;

    if (SAL_BOOT_QUICKTURN) {
        reset_sleep_usec = 50000;
    } else {
        /* Minimum of 5 clocks period with the slowest clock is required
         * between each reset step.
         *   10Mbps (2.5MHz) = 2000ns
         *  100Mbps  (25MHz) = 200ns
         * 1000Mbps (125MHz) = 40ns
         * 2500Mbps (133MHz) = 37.5ns
         */
        reset_sleep_usec = 2;  /* choose max delay */
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            reset_sleep_usec = 20; /* choose max delay, change to 20 for ECC error issue */
        }
#endif /* BCM_HURRICANE4_SUPPORT */
    }
 
    SOC_IF_ERROR_RETURN
        (READ_COMMAND_CONFIGr(unit, port, &command_config));
    if (reset_assert) {
        /* SIDE EFFECT: TX and RX are enabled when SW_RESET is set. */
        /* Assert SW_RESET */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 1);
    } else {
        /* Deassert SW_RESET */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 0);
    }

    if (soc_feature(unit, soc_feature_unimac_reset_wo_clock)) {
        if (reset_assert)
            SOC_IF_ERROR_RETURN
                (soc_pgw_rx_fifo_reset(unit, port, TRUE));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_COMMAND_CONFIGr(unit, port, command_config));

    sal_udelay(reset_sleep_usec);

    if (soc_feature(unit, soc_feature_unimac_reset_wo_clock)) {
        if (!reset_assert)
            SOC_IF_ERROR_RETURN
                (soc_pgw_rx_fifo_reset(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_expected_rx_latency_get
 * Purpose:
 *      Get the UniMAC port expected Rx latency, part of determining LINK_DELAY
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      latency - (OUT) Latency in NS
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_uni_expected_rx_latency_get(int unit, soc_port_t port, int *latency)
{
    int speed = 0;
    SOC_IF_ERROR_RETURN(mac_uni_speed_get(unit, port, &speed));

    switch (speed) {
    case 10:
        /* 10M value from KT-1370 */
        *latency = 23200;
        break;
    case 100:
        /* 100M value from KT-1370 */
        *latency = 2320;
        break;
    case 1000:  /* GigE */
        /* 1G value from KT-1370: 232ns */
        /* 1G value from SDK-84155: 275ns */
        *latency = 275;
        break;

    default:
        *latency = 0;
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_control_set
 * Purpose:
 *      To configure MAC control properties.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_uni_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                    int value)
{
    uint32 rval, fval, copy;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_control_set: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    switch(type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &rval));
        copy = rval;
        soc_reg_field_set(unit, COMMAND_CONFIGr, &rval, RX_ENAf, value ? 1 : 0);
        if (copy != rval) {
            SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, rval));
        }
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &rval));
        copy = rval;
        soc_reg_field_set(unit, COMMAND_CONFIGr, &rval, TX_ENAf, value ? 1 : 0);
        if (copy != rval) {
            SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, rval));
        }
        break;
    case SOC_MAC_CONTROL_SW_RESET:
    case SOC_MAC_CONTROL_DISABLE_PHY:
        return _mac_uni_sw_reset(unit, port, value);

    case SOC_MAC_CONTROL_PFC_TYPE:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_TYPEr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, MAC_PFC_TYPEr, port,
                                                   PFC_ETH_TYPEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_OPCODEr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, MAC_PFC_OPCODEr,
                                                   port, PFC_OPCODEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
            return SOC_E_UNAVAIL;
        }
        if (soc_reg_field_valid(unit, UNIMAC_PFC_CTRLr, PFC_EIGHT_CLASSf)) {
            if (value == 16) {
                fval = 0;
            } else if (value == 8) {
                fval = 1;
            } else {
                return SOC_E_PARAM;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                        PFC_EIGHT_CLASSf, fval));
        } else if (value != 8) {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_DA_0r)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &rval));
        rval &= 0x00ffffff;
        rval |= (value & 0x0000ff) << 24;
        SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DA_0r(unit, port, rval));

        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, MAC_PFC_DA_1r, port,
                                                   PFC_MACDA_1f, value >> 8));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_DA_0r)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &rval));
        rval &= 0xff000000;
        rval |= value & 0x00ffffff;
        SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DA_0r(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                        RX_PASS_PFC_FRMf, value ? 1 : 0));
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                        RX_PASS_PFC_FRMf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port, PFC_RX_ENBLf,
                                        value ? 1 : 0));
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port, PFC_RX_ENBLf,
                                        value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port, PFC_TX_ENBLf,
                                        value ? 1 : 0));
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port, PFC_TX_ENBLf,
                                        value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                        FORCE_PFC_XONf, value ? 1 : 0));
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                        FORCE_PFC_XONf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                        PFC_STATS_ENf, value ? 1 : 0));
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                        PFC_STATS_ENf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_REFRESH_CTRLr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_PFC_REFRESH_CTRLr,
                                    port, PFC_REFRESH_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        if (!SOC_REG_IS_VALID(unit, PFC_XOFF_TIMERr)) {
            return SOC_E_UNAVAIL;
        }
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PFC_XOFF_TIMERr,
                                    port, PFC_XOFF_TIMER_FLDf, value));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PFC_XOFF_TIMERr,
                                    port, PFC_XOFF_TIMERf, value));
        }
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, UMAC_EEE_CTRLr, EEE_ENf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, UMAC_EEE_CTRLr, 
                                                   port, EEE_ENf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
            SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit) ||
            SOC_IS_HURRICANE4(unit)) {
            if (SOC_REG_PORT_VALID(unit, MII_EEE_DELAY_ENTRY_TIMERr, port)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, MII_EEE_DELAY_ENTRY_TIMERr,
                    port, MII_EEE_LPI_TIMERf, value));
            }

            if (SOC_REG_PORT_VALID(unit, GMII_EEE_DELAY_ENTRY_TIMERr, port)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, GMII_EEE_DELAY_ENTRY_TIMERr,
                    port, GMII_EEE_LPI_TIMERf, value));
            }
        } else
#endif
        {
            if (!soc_reg_field_valid(unit,
                EEE_DELAY_ENTRY_TIMERr, EEE_LPI_TIMERf)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EEE_DELAY_ENTRY_TIMERr,
                port, EEE_LPI_TIMERf, value));
        }
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) || 
            SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit) ||
            SOC_IS_HURRICANE4(unit)) {
            if (SOC_REG_PORT_VALID(unit, MII_EEE_WAKE_TIMERr, port)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, MII_EEE_WAKE_TIMERr,
                    port, MII_EEE_WAKE_TIMERf, value));
            }

            if (SOC_REG_PORT_VALID(unit, GMII_EEE_WAKE_TIMERr, port)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, GMII_EEE_WAKE_TIMERr,
                    port, GMII_EEE_WAKE_TIMERf, value));
            }
        } else
#endif
        {
            if (!soc_reg_field_valid(unit, EEE_WAKE_TIMERr, EEE_WAKE_TIMERf)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EEE_WAKE_TIMERr,
                port, EEE_WAKE_TIMERf, value));
        }
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TAG_0r, port,
                                    FRM_TAG_0f, value));
        if (soc_reg_field_valid(unit, TAG_0r, CONFIG_OUTER_TPID_ENABLEf)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TAG_0r, port,
                     CONFIG_OUTER_TPID_ENABLEf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TAG_1r, port,
                                    FRM_TAG_1f, value));
        if (soc_reg_field_valid(unit, TAG_1r, CONFIG_INNER_TPID_ENABLEf)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TAG_1r, port,
                     CONFIG_INNER_TPID_ENABLEf, value ? 1 : 0));
        }
        break;

#if defined(BCM_FIRELIGHT_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
    case SOC_MAC_PASS_CONTROL_FRAME:
        if (SOC_IS_FIRELIGHT(unit) || SOC_IS_HURRICANE4(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, COMMAND_CONFIGr, port,
                                    PAUSE_FWDf, value ? 1 : 0));
        }
        break;
#endif

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_control_get
 * Purpose:
 *      To get current MAC control setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int 
mac_uni_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                    int *value)
{
    int rv;
    uint32  rval, fval0, fval1;

    rv = SOC_E_NONE;

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &rval));
        *value = soc_reg_field_get(unit, COMMAND_CONFIGr, rval, RX_ENAf);
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &rval));
        *value = soc_reg_field_get(unit, COMMAND_CONFIGr, rval, TX_ENAf);
        break;
    case SOC_MAC_CONTROL_TIMESTAMP_TRANSMIT:
            {
                uint32 timestamp = 0;
                if (soc_feature(unit, soc_feature_timestamp_counter) &&
                    soc_property_get(unit, spn_SW_TIMESTAMP_FIFO_ENABLE, 1)) {
                    rv = soc_counter_timestamp_get(unit, port, &timestamp);
                    if (rv != SOC_E_NOT_FOUND) {
                        *value = (int)timestamp;
                        return rv;
                    }
                }

                if (!SOC_REG_IS_VALID(unit, TS_STATUS_CNTRLr)) {
                    return SOC_E_UNAVAIL;
                }
                SOC_IF_ERROR_RETURN(READ_TS_STATUS_CNTRLr(unit, port, &rval));
                if (soc_reg_field_get(unit, TS_STATUS_CNTRLr, rval, TX_TS_FIFO_EMPTYf)) {
                    return SOC_E_EMPTY;
                }
                if (SOC_REG_IS_VALID(unit,TX_TS_SEQ_IDr)) {
                    SOC_IF_ERROR_RETURN(READ_TX_TS_SEQ_IDr(unit, port, &rval));
                    if (!soc_reg_field_get(unit, TX_TS_SEQ_IDr, rval, TSTS_VALIDf)) {
                        return SOC_E_EMPTY;
                    }
                }
                SOC_IF_ERROR_RETURN(READ_TX_TS_DATAr(unit, port, &rval));
                *value = (int) soc_reg_field_get(unit, TX_TS_DATAr, rval, TX_TS_DATAf);
                rv = SOC_E_NONE;
            }
            break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_TYPEr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_TYPEr(unit, port, &rval));
        *value = soc_reg_field_get(unit, MAC_PFC_TYPEr, rval, PFC_ETH_TYPEf);
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_OPCODEr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_OPCODEr(unit, port, &rval));
        *value = soc_reg_field_get(unit, MAC_PFC_OPCODEr, rval, PFC_OPCODEf);
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
            return SOC_E_UNAVAIL;
        }
        if (soc_reg_field_valid(unit, UNIMAC_PFC_CTRLr, PFC_EIGHT_CLASSf)) {
            SOC_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, rval,
                                       PFC_EIGHT_CLASSf) ? 8 : 16;
        } else {
            *value = 8;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_DA_0r)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &rval));
        fval0 = soc_reg_field_get(unit, MAC_PFC_DA_0r, rval, PFC_MACDA_0f);
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_1r(unit, port, &rval));
        fval1 = soc_reg_field_get(unit, MAC_PFC_DA_1r, rval, PFC_MACDA_1f);
        *value = (fval0 >> 24) | (fval1 << 8);
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_DA_0r)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &rval));
        *value = soc_reg_field_get(unit, MAC_PFC_DA_0r, rval, PFC_MACDA_0f) &
            0x00ffffff;
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &rval));
            *value = soc_reg_field_get(unit, MAC_PFC_CTRLr, rval,
                                       RX_PASS_PFC_FRMf);
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, rval,
                                       RX_PASS_PFC_FRMf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &rval));
            *value = soc_reg_field_get(unit, MAC_PFC_CTRLr, rval, PFC_RX_ENBLf);
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, rval, PFC_RX_ENBLf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &rval));
            *value = soc_reg_field_get(unit, MAC_PFC_CTRLr, rval, PFC_TX_ENBLf);
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, rval, PFC_TX_ENBLf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &rval));
            *value = soc_reg_field_get(unit, MAC_PFC_CTRLr, rval,
                                       FORCE_PFC_XONf);
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, rval,
                                       FORCE_PFC_XONf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)
             || SOC_IS_HURRICANE4(unit))
            && SOC_REG_IS_VALID(unit, MAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &rval));
            *value = soc_reg_field_get(unit, MAC_PFC_CTRLr, rval,
                                       PFC_STATS_ENf);
        } else {
            if (!SOC_REG_IS_VALID(unit, UNIMAC_PFC_CTRLr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_UNIMAC_PFC_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UNIMAC_PFC_CTRLr, rval,
                                       PFC_STATS_ENf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        if (!SOC_REG_IS_VALID(unit, MAC_PFC_REFRESH_CTRLr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_PFC_REFRESH_CTRLr(unit, port, &rval));
        *value = soc_reg_field_get(unit, MAC_PFC_REFRESH_CTRLr, rval,
                                   PFC_REFRESH_TIMERf);
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        if (!SOC_REG_IS_VALID(unit, PFC_XOFF_TIMERr)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_PFC_XOFF_TIMERr(unit, port, &rval));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            *value = soc_reg_field_get(unit, PFC_XOFF_TIMERr, rval,
                                       PFC_XOFF_TIMER_FLDf);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            *value = soc_reg_field_get(unit, PFC_XOFF_TIMERr, rval,
                                       PFC_XOFF_TIMERf);
        }
        break;

        case SOC_MAC_CONTROL_EEE_ENABLE:
            if (!soc_feature(unit, soc_feature_eee)) {
                return SOC_E_UNAVAIL;
            }
            if (!soc_reg_field_valid(unit, UMAC_EEE_CTRLr, EEE_ENf)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(READ_UMAC_EEE_CTRLr(unit, port, &rval));
            *value = soc_reg_field_get(unit, UMAC_EEE_CTRLr, rval, EEE_ENf);
            break;

        case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
            if (!soc_feature(unit, soc_feature_eee)) {
                return SOC_E_UNAVAIL;
            }
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) || 
                SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit) ||
                SOC_IS_HURRICANE4(unit)) {
                if (SOC_REG_PORT_VALID(
                    unit, MII_EEE_DELAY_ENTRY_TIMERr, port)) {
                    SOC_IF_ERROR_RETURN(
                        READ_MII_EEE_DELAY_ENTRY_TIMERr(unit, port, &rval));
                    *value = soc_reg_field_get(unit, MII_EEE_DELAY_ENTRY_TIMERr,
                        rval, MII_EEE_LPI_TIMERf);
                } else if (SOC_REG_PORT_VALID(unit,
                    GMII_EEE_DELAY_ENTRY_TIMERr, port)) {
                    SOC_IF_ERROR_RETURN(
                        READ_GMII_EEE_DELAY_ENTRY_TIMERr(unit, port, &rval));
                    *value = soc_reg_field_get(unit,GMII_EEE_DELAY_ENTRY_TIMERr,
                        rval, GMII_EEE_LPI_TIMERf);
                } else {
                    return SOC_E_UNAVAIL;
                }
            } else
#endif
            {
                if (!soc_reg_field_valid(unit,
                    EEE_DELAY_ENTRY_TIMERr, EEE_LPI_TIMERf)) {
                    return SOC_E_UNAVAIL;
                }
                SOC_IF_ERROR_RETURN(
                    READ_EEE_DELAY_ENTRY_TIMERr(unit, port, &rval));
                *value = soc_reg_field_get(unit, EEE_DELAY_ENTRY_TIMERr, rval,
                        EEE_LPI_TIMERf);
            }
            break;

        case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
            if (!soc_feature(unit, soc_feature_eee)) {
                return SOC_E_UNAVAIL;
            }
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
                SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit) ||
                SOC_IS_HURRICANE4(unit)) {
                if (SOC_REG_PORT_VALID(unit, MII_EEE_WAKE_TIMERr, port)) {
                    SOC_IF_ERROR_RETURN(
                        READ_MII_EEE_WAKE_TIMERr(unit, port, &rval));
                    *value = soc_reg_field_get(unit, MII_EEE_WAKE_TIMERr,
                        rval, MII_EEE_WAKE_TIMERf);
                } else if (
                    SOC_REG_PORT_VALID(unit, GMII_EEE_WAKE_TIMERr, port)) {
                    SOC_IF_ERROR_RETURN(
                        READ_GMII_EEE_WAKE_TIMERr(unit, port, &rval));
                    *value = soc_reg_field_get(unit, GMII_EEE_WAKE_TIMERr,
                        rval, GMII_EEE_WAKE_TIMERf);
                } else {
                    return SOC_E_UNAVAIL;
                }
            } else
#endif
            {
                if (!soc_reg_field_valid(unit,
                    EEE_WAKE_TIMERr, EEE_WAKE_TIMERf)) {
                    return SOC_E_UNAVAIL;
                }
                SOC_IF_ERROR_RETURN(READ_EEE_WAKE_TIMERr(unit, port, &rval));
                *value = soc_reg_field_get(unit, EEE_WAKE_TIMERr, rval,
                        EEE_WAKE_TIMERf);
            }
            break;
        
        case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS: 
        case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS: 
            *value = 0;
            break;

        case SOC_MAC_CONTROL_EXPECTED_RX_LATENCY:
            SOC_IF_ERROR_RETURN(mac_uni_expected_rx_latency_get(unit, port, value));
            break;

        case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
            SOC_IF_ERROR_RETURN(READ_TAG_0r(unit, port, &rval));
            *value = soc_reg_field_get(unit, TAG_0r, rval, FRM_TAG_0f);
            break;

        case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
            SOC_IF_ERROR_RETURN(READ_TAG_1r(unit, port, &rval));
            *value = soc_reg_field_get(unit, TAG_1r, rval, FRM_TAG_1f);
            break;

#if defined(BCM_FIRELIGHT_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
    case SOC_MAC_PASS_CONTROL_FRAME:
        if (SOC_IS_FIRELIGHT(unit) || SOC_IS_HURRICANE4(unit)) {
            SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &rval));
            *value = soc_reg_field_get(unit, COMMAND_CONFIGr, rval, PAUSE_FWDf);
        }
        break;
#endif

        default:
            return SOC_E_UNAVAIL;
    }
    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_control_get: unit %d port %s type=%d value=%d "
                             "rv=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, *value, rv));
    return rv;
}

/*
 * Function:
 *      mac_uni_ability_local_get
 * Purpose:
 *      Return the UniMAC abilities
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port # on unit.
 *      mode - (OUT) Mask of MAC abilities returned.
 * Returns:
 *      SOC_E_NONE
 */

STATIC  int
mac_uni_ability_local_get(int unit, soc_port_t port,
                         soc_port_ability_t *ability)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit) && 
        (SOC_PORT_BLOCK_TYPE(unit, port) == SOC_BLK_QGPORT)) {
        /* 5682x/72x GE-only ports are restricted */
        ability->speed_half_duplex = 0; 
        ability->speed_full_duplex =
            SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB;
    } else
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
        if (SOC_IS_TD_TT(unit) || SOC_IS_HURRICANE3(unit)) {
        ability->speed_half_duplex =
            SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB; 
        ability->speed_full_duplex =
            SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB;
        if (SOC_CONTROL(unit)->info.port_speed_max[port] > 1000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        ability->speed_half_duplex =
            SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB; 
        ability->speed_full_duplex =
            SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB |
            SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB;
    }
    ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_GMII; 
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->medium    = SOC_PA_ABILITY_NONE;
    ability->loopback  = SOC_PA_LB_MAC;
    ability->flags     = SOC_PA_ABILITY_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_uni_ability_local_get: unit %d port %s "
                             "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                             "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 ability->speed_half_duplex, ability->speed_full_duplex,
                 ability->encap, ability->pause, ability->interface,
                 ability->medium, ability->loopback, ability->flags));

    return (SOC_E_NONE);
}


/* Exported UniMAC driver structure */
mac_driver_t soc_mac_uni = {
    "UniMAC Driver",                 /* drv_name */
    mac_uni_init,                    /* md_init  */
    mac_uni_enable_set,              /* md_enable_set */
    mac_uni_enable_get,              /* md_enable_get */
    mac_uni_duplex_set,              /* md_duplex_set */
    mac_uni_duplex_get,              /* md_duplex_get */
    mac_uni_speed_set,               /* md_speed_set */
    mac_uni_speed_get,               /* md_speed_get */
    mac_uni_pause_set,               /* md_pause_set */
    mac_uni_pause_get,               /* md_pause_get */
    mac_uni_pause_addr_set,          /* md_pause_addr_set */
    mac_uni_pause_addr_get,          /* md_pause_addr_get */
    mac_uni_loopback_set,            /* md_lb_set */
    mac_uni_loopback_get,            /* md_lb_get */
    mac_uni_interface_set,           /* md_interface_set */
    mac_uni_interface_get,           /* md_interface_get */
    mac_uni_ability_get,             /* md_ability_get */
    mac_uni_frame_max_set,           /* md_frame_max_set */
    mac_uni_frame_max_get,           /* md_frame_max_get */
    mac_uni_ifg_set,                 /* md_ifg_set */
    mac_uni_ifg_get,                 /* md_ifg_get */
    mac_uni_encap_set,               /* md_encap_set */
    mac_uni_encap_get,               /* md_encap_get */
    mac_uni_control_set,             /* md_control_set */
    mac_uni_control_get,             /* md_control_get */
    mac_uni_ability_local_get,       /* md_ability_local_get */
    NULL                             /* md_timesync_tx_info_get */
};
#endif /* BCM_UNIMAC_SUPPORT */
