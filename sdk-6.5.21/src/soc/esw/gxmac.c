/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS 1/2.5/10/12G Media Access Controller Driver (gxmac)
 *
 * This module is used for:
 *
 *   - All ports on XGS3 HUMV/Bradley/Goldwing family
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/portmode.h>
#include <soc/ll.h>
#include <soc/counter.h>
#include <soc/phy.h>
#include <soc/phyctrl.h>

#ifdef BCM_GXPORT_SUPPORT


#ifndef GXPORT_ISLOATE_IF_LOOPBACK
#define GXPORT_ISLOATE_IF_LOOPBACK 0
#endif

/*
 * GXMAC Register field definitions.
 */

/* Transmit CRC Modes (Receive has bit field definitions in register) */
#define GXMAC_CRC_APPEND          0x00   /* Append CRC (Default) */
#define GXMAC_CRC_KEEP            0x01   /* CRC Assumed correct */
#define GXMAC_CRC_REGEN           0x02   /* Replace CRC with a new one */
#define GXMAC_CRC_RSVP            0x03   /* Reserved (does Replace) */


#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

/*
 * Forward Declarations
 *
 *      gxmac_xxx        Routines that work on XGS ports in 10Gig mode.
 */
STATIC int   gxmac_ipg_update(int unit, soc_port_t port);
STATIC int   gxmac_init(int unit, soc_port_t port);
STATIC int   gxmac_enable_set(int unit, soc_port_t port, int enable);
STATIC int   gxmac_enable_get(int unit, soc_port_t port, int *enable);
STATIC int   gxmac_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int   gxmac_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int   gxmac_pause_set(int unit, soc_port_t, int, int);
STATIC int   gxmac_pause_get(int unit, soc_port_t, int*, int*);
STATIC int   gxmac_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t);
STATIC int   gxmac_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t);
STATIC int   gxmac_interface_set(int unit, soc_port_t port,
                                soc_port_if_t pif);
STATIC int   gxmac_interface_get(int unit, soc_port_t port,
                                soc_port_if_t *pif);
STATIC int   gxmac_speed_set(int unit, soc_port_t port, int speed);
STATIC int   gxmac_speed_get(int unit, soc_port_t port, int *speed);
STATIC int   gxmac_loopback_get(int unit, soc_port_t port, int *loopback);
STATIC int   gxmac_loopback_set(int unit, soc_port_t port, int loopback);
STATIC int   gxmac_frame_max_set(int unit, soc_port_t port, int size);
STATIC int   gxmac_frame_max_get(int unit, soc_port_t port, int *size);
STATIC int   gxmac_control_set(int unit, soc_port_t port,
              soc_mac_control_t type, int value);
STATIC int   gxmac_control_get(int unit, soc_port_t port,
              soc_mac_control_t type, int *value);
STATIC int   gxmac_frame_spacing_stretch_set(int unit,
              soc_port_t port, int value);
STATIC int   soc_unicore_pll_lock_wait(int unit, soc_port_t port);
STATIC int   soc_unicore_speed_set(int unit, soc_port_t port, int speed);

#ifdef BROADCOM_DEBUG
static char *gxmac_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *gxmac_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

/*
 * Returns the CMIC_XGXS_MDIO_CONFIG_N register corresponding to the port.
 */
STATIC int
_port2cmic_xgxs_mdio_config(int unit, soc_port_t port)
{
    if (SOC_IS_HB_GW(unit)) {
        switch(port) {
            case  0: return CMIC_XGXS_MDIO_CONFIG_0r;
            case  1: return CMIC_XGXS_MDIO_CONFIG_1r;
            case  2: return CMIC_XGXS_MDIO_CONFIG_2r;
            case  3: return CMIC_XGXS_MDIO_CONFIG_3r;
            case  4: return CMIC_XGXS_MDIO_CONFIG_4r;
            case  5: return CMIC_XGXS_MDIO_CONFIG_5r;
            case  6: return CMIC_XGXS_MDIO_CONFIG_6r;
            case  7: return CMIC_XGXS_MDIO_CONFIG_7r;
            case  8: return CMIC_XGXS_MDIO_CONFIG_8r;
            case  9: return CMIC_XGXS_MDIO_CONFIG_9r;
            case 10: return CMIC_XGXS_MDIO_CONFIG_10r;
            case 11: return CMIC_XGXS_MDIO_CONFIG_11r;
            case 12: return CMIC_XGXS_MDIO_CONFIG_12r;
            case 13: return CMIC_XGXS_MDIO_CONFIG_13r;
            case 14: return CMIC_XGXS_MDIO_CONFIG_14r;
            case 15: return CMIC_XGXS_MDIO_CONFIG_15r;
            case 16: return CMIC_XGXS_MDIO_CONFIG_16r;
            case 17: return CMIC_XGXS_MDIO_CONFIG_17r;
            case 18: return CMIC_XGXS_MDIO_CONFIG_18r;
            case 19: return CMIC_XGXS_MDIO_CONFIG_19r;
            default: return INVALIDr;
        }
    }

    return INVALIDr;
}

/*
 * Function:
 *     soc_unicore_reset (10(X)-Gig/Xaui/Serdes (XGXS) reset)
 * Purpose:
 *     Reset and initialize the BigMAC and Unicore.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 * Returns:
 *     SOC_E_XXX
 */
int
soc_unicore_reset(int unit, soc_port_t port)
{
    uint8               phy_addr;
    uint64              xgxs_ctrl;
    uint32              reg_val, lcpll;
    uint16              mreg_val, hg_speeds;
    soc_reg_t           reg;
    int                 preemph;
    int                 idriver, pdriver;
    int                 rxequal, offsctl;
    int                 reset_sleep_usec;
#ifdef BCM_HB_GW_SUPPORT
    uint32              gport_config;
#endif

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac: unit %d port %s: unicore reset\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    /* 
     * CX4 compliance related
     */
    reg = _port2cmic_xgxs_mdio_config(unit, port);
    if (reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    reg_val = soc_pci_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0));
    soc_reg_field_set(unit, reg, &reg_val, MD_DEVADf, 0x0);
    soc_reg_field_set(unit, reg, &reg_val, IEEE_DEVICES_IN_PKGf,
                      (IS_HG_PORT(unit, port)) ? 0x03 : 0x15);
    soc_pci_write(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), reg_val);

    /*
     * Unicore reference clock selection between LCPLL and clock pads.
     */
    lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                  (SAL_BOOT_QUICKTURN) ? 0 : 1);
    SOC_IF_ERROR_RETURN(READ_MAC_XGXS_CTRLr(unit, port, &xgxs_ctrl));
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr,
                          &xgxs_ctrl, LCREFENf, lcpll);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

    /*
     * Allow properties to change important variables.
     */
    rxequal = soc_property_port_get(unit, port, spn_XGXS_EQUALIZER,
                                    /* coverity[identical_branches : FALSE] */
                                    IS_HG_PORT(unit,port) ? 0x5 : 0x5);
    offsctl = soc_property_port_get(unit, port, spn_XGXS_OFFSET, 
                                    /* coverity[identical_branches : FALSE] */
                                    IS_HG_PORT(unit,port) ? 0x2 : 0x2);
    /* NB: preemphasis, driver, pre-driver currents are bit-reversed in HW */
    preemph = soc_property_port_get(unit, port, spn_XGXS_PREEMPHASIS, 
                                    /* coverity[identical_branches : FALSE] */
                                    IS_HG_PORT(unit,port) ? 0x4 : 0x4);
    preemph = _shr_bit_rev8(preemph) >> 4;

    idriver = soc_property_port_get(unit, port, spn_XGXS_DRIVER_CURRENT, 
                                    IS_HG_PORT(unit,port) ? 0x9 : 0xf);
    idriver = _shr_bit_rev8(idriver) >> 4;

    pdriver = soc_property_port_get(unit, port, spn_XGXS_PRE_DRIVER_CURRENT, 
                                    IS_HG_PORT(unit,port) ? 0x9 : 0xf);
    pdriver = _shr_bit_rev8(pdriver) >> 4;

    phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    reset_sleep_usec = 100;
    if (SAL_BOOT_QUICKTURN) {
        /* Kick the QT phy model */
        uint16 rv, kv;

        rv = 0x0800; /* Reset */ 
        if (IS_HG_PORT(unit,port)) {
            kv = 0x0000; /* Configure for HG mode */ 
        } else {
            kv = 0x0002; /* Configure for XE mode */ 
        }
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, port + 0x41 , 0x00, rv));
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, port + 0x41 , 0x00, kv));
        reset_sleep_usec = 500000;
    } else {
        if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) {
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, port + 1, 0x00, 0x0000));
        }
    }

    /* 
     * Force BigMAC and TriMAC into reset before initialization
     */
    SOC_IF_ERROR_RETURN(READ_MAC_XGXS_CTRLr(unit, port, &xgxs_ctrl));
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, IDDQf, 1);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, PWRDWNf, 1);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, HW_RSTLf, 0);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, RSTB_MDIOREGSf, 0);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, RSTB_PLLf, 0);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, BIGMACRSTLf, 0);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, TXFIFO_RSTLf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

#ifdef BCM_HB_GW_SUPPORT
    if (soc_reg_field_valid(unit, GPORT_CONFIGr, TRIMAC_RESETf)) {
        SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &gport_config));
        soc_reg_field_set(unit, GPORT_CONFIGr, &gport_config,
                          TRIMAC_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, gport_config));
    }
#endif
    sal_usleep(reset_sleep_usec);

    /*
     * XGXS MAC initialization steps.
     *
     * A minimum delay is required between various initialization steps.
     * There is no maximum delay.  The values given are very conservative
     * including the timeout for PLL lock.
     */

    /* Powerup Unicore interface (digital and analog clocks) */
    SOC_IF_ERROR_RETURN(READ_MAC_XGXS_CTRLr(unit, port, &xgxs_ctrl));
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, IDDQf, 0);
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, PWRDWNf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));
    sal_usleep(reset_sleep_usec);

    /* Bring Unicore out of reset */
    SOC_IF_ERROR_RETURN(READ_MAC_XGXS_CTRLr(unit, port, &xgxs_ctrl));
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, HW_RSTLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

    /* Bring MDIO registers out of reset */
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, RSTB_MDIOREGSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

    /* Activate all clocks */
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, RSTB_PLLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

    /* Bring TriMac out of reset */
#ifdef BCM_HB_GW_SUPPORT
    if (soc_reg_field_valid(unit, GPORT_CONFIGr, TRIMAC_RESETf)) {
        SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &gport_config));
        soc_reg_field_set(unit, GPORT_CONFIGr, &gport_config,
                          TRIMAC_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, gport_config));
    }
#endif

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) {
        /*
         * Isolate external power-down input pins
         */
        if (soc_feature(unit, soc_feature_xgxs_power)) {
            SOC_IF_ERROR_RETURN     /* Select block 0x1 */
                (soc_miim_write(unit, phy_addr, 0x1f, 0x0010));
            SOC_IF_ERROR_RETURN
                (soc_miim_read(unit, phy_addr, 0x1a, &mreg_val));
            mreg_val |= (1 << 10);
            SOC_IF_ERROR_RETURN     /* Power on Core */
                (soc_miim_write(unit, phy_addr, 0x1a, mreg_val));
        }
    
        /*
         * Basic XGXS configuration
         */
        SOC_IF_ERROR_RETURN	        /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN	        /* Disable PLL state machine */
            (soc_miim_write(unit, phy_addr, 0x11, 0x5006));
        SOC_IF_ERROR_RETURN	        /* PLL VCO step time */
            (soc_miim_write(unit, phy_addr, 0x12, 0x04ff));
        SOC_IF_ERROR_RETURN	        /* Turn off slowdn_xor */
            (soc_miim_write(unit, phy_addr, 0x15, 0x0000));
        SOC_IF_ERROR_RETURN	        /* Select block 0xf */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));
        SOC_IF_ERROR_RETURN	        /* CDR bandwidth */
            (soc_miim_write(unit, phy_addr, 0x16, 0x8300));
    
        SOC_IF_ERROR_RETURN		/* Select block 0x0 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
        SOC_IF_ERROR_RETURN
            (soc_miim_read(unit, phy_addr, 0x10, &mreg_val));
        mreg_val = (mreg_val & 0xf0ff) | (0xc << 8);
        SOC_IF_ERROR_RETURN         /* Select ComboCore mode */
            (soc_miim_write(unit, phy_addr, 0x10, mreg_val));
        if (IS_HG_PORT(unit,port)) {
            SOC_IF_ERROR_RETURN     /* Enable DTE mdio reg mapping */
                (soc_miim_write(unit, phy_addr, 0x1e, 0x0001));
            SOC_IF_ERROR_RETURN     /* Select block 0x32 */
                (soc_miim_write(unit, phy_addr, 0x1f, 0x0320));
            /* For Draco and Hercules, use pre-CX4 signalling */
            hg_speeds = 0x0008;
            if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
                /* Also advertise 10G CX4 signalling by default */
                hg_speeds |= 0x0010;
            }
            if (SOC_INFO(unit).port_speed_max[port] >= 13000) {
                /* Also advertise 12G and 13G */
                hg_speeds |= 0x00a0;
            }
            SOC_IF_ERROR_RETURN     
                (soc_miim_write(unit, phy_addr, 0x19, hg_speeds));
        } else {
            SOC_IF_ERROR_RETURN     /* Enable PMA/PMD mdio reg mapping */
                (soc_miim_write(unit, phy_addr, 0x1e, 0x0201));
            SOC_IF_ERROR_RETURN     /* Select block 0x32 */
                (soc_miim_write(unit, phy_addr, 0x1f, 0x0320));
            if (IS_GE_PORT(unit,port)) {
                SOC_IF_ERROR_RETURN     /* Only advertise 2.5G CX4 */
                    (soc_miim_write(unit, phy_addr, 0x19, 0x0001));
            } else {
                /* Advertise only 10G CX4 for XE port */
                SOC_IF_ERROR_RETURN     /* Advertise 10G CX4 only */
                    (soc_miim_write(unit, phy_addr, 0x19, 0x0010));
           }
        }
    
        /*
         * Configure 10G parallel detect
         */
        SOC_IF_ERROR_RETURN     /* Select block 0x34 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0340));
        SOC_IF_ERROR_RETURN     /* Adjust parallel detect link timer to 60ms */
            (soc_miim_write(unit, phy_addr, 0x13, 0x16E2));
        SOC_IF_ERROR_RETURN     /* Read parDet10GControl register */
            (soc_miim_read(unit, phy_addr, 0x11, &mreg_val));
        mreg_val &= ~(1 << 0);
        if (soc_property_port_get(unit, port, spn_XGXS_PDETECT_10G, 1)) {
            mreg_val |= (1 << 0);   /* Enable 10G parallel detect */
        }
        SOC_IF_ERROR_RETURN     /* Update parDet10GControl register */
            (soc_miim_write(unit, phy_addr, 0x11, mreg_val));
        if (!(IS_HG_PORT(unit, port) && 
            SOC_INFO(unit).port_speed_max[port] > 10000)) {
            /*
             * Disable 12 Gbps parallel detect on all ports with max
             * speed 10Gbps and lower.
             */ 
            SOC_IF_ERROR_RETURN     /* Read parDet10GControl register */
                (soc_miim_read(unit, phy_addr, 0x11, &mreg_val));
            mreg_val |= (1 << 5);   /* Disable parallel detect for 12 Gbps */
            mreg_val |= (1 << 6);   /* Disable parallel detect 12 Gbps TXD */
            SOC_IF_ERROR_RETURN     /* Update parDet10GControl register */
                (soc_miim_write(unit, phy_addr, 0x11, mreg_val));
        }
        SOC_IF_ERROR_RETURN     /* Update parDet10GLostLink register */
            (soc_miim_write(unit, phy_addr, 0x14, 0x4c4a));
    
        /*
         * Configure TX pre-emphasis, driver and pre-driver currents
         */
        SOC_IF_ERROR_RETURN         /* Select block 0xa */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
        SOC_IF_ERROR_RETURN         /* TX pre-emphasis */
            (soc_miim_write(unit, phy_addr, 0x17, ((preemph & 0xf) << 12) |
                                                  ((idriver & 0xf) << 8) |
                                                  ((pdriver & 0xf) << 4)));
    
        /*
         * Configure PLL
         */
        SOC_IF_ERROR_RETURN	        /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN	        /* Enable PLL state machine */
            (soc_miim_write(unit, phy_addr, 0x11, 0xf01e));
    
        /*
         * Transform CX4 pin out on the board to HG pinout
         */
        if (soc_property_port_get(unit, port, spn_CX4_TO_HIGIG, FALSE)) {
            SOC_IF_ERROR_RETURN     /* Select block 0x10 */
                (soc_miim_write(unit, phy_addr, 0x1f, 0x0100));
            SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                (soc_miim_write(unit, phy_addr, 0x11, 0x80e4));
    
            SOC_IF_ERROR_RETURN     /* Select block 0x0a */
                (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
            SOC_IF_ERROR_RETURN     /* Flip TX Lane polarity */
                (soc_miim_write(unit, phy_addr, 0x11, 0x1020));
        } else {
            /* If CX4 to HG conversion is enabled, do not allow individual lane 
             * swapping.
             */
            uint16 lane_map, i;
            uint16 lane, hw_map, chk_map;
          
            /* Update RX lane map */
            lane_map = soc_property_port_get(unit, port, 
                                            spn_XGXS_RX_LANE_MAP, 0x0123) & 0xffff; 
    
            if (lane_map != 0x0123) {
                hw_map  = 0;
                chk_map = 0;
                for (i = 0; i < 4; i++) {
                    lane     = (lane_map >> (i * 4)) & 0xf;
                    hw_map  |= lane << (i * 2);
                    chk_map |= 1 << lane;
                }
                if (chk_map == 0xf) {
                    SOC_IF_ERROR_RETURN     /* Select block 0x10 */
                        (soc_miim_write(unit, phy_addr, 0x1f, 0x0100));
                    SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
                       (soc_miim_modify(unit, phy_addr, 0x10, 
                                        0x8000 | hw_map, 0x80ff));
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d port %s: Invalid RX lane map 0x%04x.\n"),
                               unit, SOC_PORT_NAME(unit, port), lane_map));
                }
            }
    
            /* Update TX lane map */
            lane_map = soc_property_port_get(unit, port, 
                                            spn_XGXS_TX_LANE_MAP, 0x0123) & 0xffff;
            
            if (lane_map != 0x0123) {
                hw_map  = 0;
                chk_map = 0;
                for (i = 0; i < 4; i++) {
                    lane     = (lane_map >> (i * 4)) & 0xf;
                    hw_map  |= lane << (i * 2);
                    chk_map |= 1 << lane;
                }
                if (chk_map == 0xf) {
                    SOC_IF_ERROR_RETURN     /* Select block 0x10 */
                        (soc_miim_write(unit, phy_addr, 0x1f, 0x0100));
                    SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                       (soc_miim_modify(unit, phy_addr, 0x11, 
                                        0x8000 | hw_map, 0x80ff));
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d port %s: Invalid TX lane map 0x%04x.\n"),
                               unit, SOC_PORT_NAME(unit, port), lane_map));
                }
            }
        }
    
        /*
         * Configure RX equalizer boost
         */
        SOC_IF_ERROR_RETURN         /* Select block 0xb */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00b0));
        SOC_IF_ERROR_RETURN         /* RX equalizer and offset controls */
            (soc_miim_write(unit, phy_addr, 0x1c, ((offsctl & 0x7) << 3) |
                                                  (rxequal & 0x7)));
    
        SOC_IF_ERROR_RETURN         /* Select block 0x0 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
    
        if (SAL_BOOT_QUICKTURN) {
            LOG_VERBOSE(BSL_LS_SOC_10G,
                        (BSL_META_U(unit,
                        "SIMULATION: xmac: unit %d port %s: "
                         "skipped waiting for pll lock\n"),
                         unit, SOC_PORT_NAME(unit, port)));
        } else {
            /* Wait for Tx PLL lock */
            SOC_IF_ERROR_RETURN
                (soc_unicore_pll_lock_wait(unit, port));
        }
    
        /*
         * Configure signal auto-detection between SGMII and fiber.
         */
        SOC_IF_ERROR_RETURN         /* Select block DIGITAL (0x30) */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0300));
        SOC_IF_ERROR_RETURN
            (soc_miim_read(unit, phy_addr, 0x10, &mreg_val));
        mreg_val &= ~((1 << 4) | (1 << 0)); 
        if (soc_property_port_get(unit, port, spn_SERDES_AUTOMEDIUM, TRUE)) {
            mreg_val |= (1 << 4); 
        }
        if (soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF, TRUE)) {
            mreg_val |= (1 << 0); 
        }
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x10, mreg_val)); 
     
        /* Set the block select to 0 after reset */ 
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x1f, 0));
    } /* end if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) */
 
    /* Bring BigMac out of reset */
    SOC_IF_ERROR_RETURN(READ_MAC_XGXS_CTRLr(unit, port, &xgxs_ctrl));
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, BIGMACRSTLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

    /* Enable Tx FIFO */
    soc_reg64_field32_set(unit, MAC_XGXS_CTRLr, &xgxs_ctrl, TXFIFO_RSTLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MAC_XGXS_CTRLr(unit, port, xgxs_ctrl));

    /* Set default speed */
    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_unicore_speed_set(unit, port, 10000));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_unicore_pll_lock_wait
 * Purpose:
 *     Wait for Unicore Tx PLL lock.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 * Returns:
 *     SOC_E_XXX
 */
int
soc_unicore_pll_lock_wait(int unit, int port)
{
    uint64 xgxs_stat;
    int pll_lock_usec;
    int locked;
    soc_timeout_t to;

    /* 
     * Wait up to 500 msec for TX PLL lock.
     */
    pll_lock_usec = 500000;
    soc_timeout_init(&to, pll_lock_usec, 0);

    locked = 0;
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));
        locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat,
                                       TXPLL_LOCKf);
        if (locked) {
            break;
        }
    }

    if (!locked) {
        uint16  x_pll_stat;
        uint16  block_select;
 
        uint8 phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);

        SOC_IF_ERROR_RETURN   /* Save original block select to restore later */ 
            (soc_miim_read(unit, phy_addr, 0x1f, &block_select));
        SOC_IF_ERROR_RETURN		/* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN		/* Read PLL status */
            (soc_miim_read(unit, phy_addr, 0x10, &x_pll_stat));
        SOC_IF_ERROR_RETURN		/* Restore original block select */
            (soc_miim_write(unit, phy_addr, 0x1f, block_select));

        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d port %s: Unicore PLL did not lock "
                              "PLL_STAT %04x\n"),
                   unit, SOC_PORT_NAME(unit, port), x_pll_stat));

        return SOC_E_TIMEOUT;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "unit %d port %s: Unicore PLL locked in %d usec\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 soc_timeout_elapsed(&to)));

    return SOC_E_NONE;
}

#ifdef GXPORT_ISLOATE_IF_LOOPBACK
/*
 * Function:
 *     soc_unicore_lane_power_set
 * Purpose:
 *     Power XAUI lanes /up/down
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     power_mask - mask of lanes to power up
 * Returns:
 *     SOC_E_XXX
 */
int
soc_unicore_lane_power_set(int unit, soc_port_t port, uint32 power_mask)
{
    uint8 phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    uint16 lane_ctrl, o_lane_ctrl;

    SOC_IF_ERROR_RETURN     /* Select block 0x10 */
        (soc_miim_write(unit, phy_addr, 0x1f, 0x0010));

    SOC_IF_ERROR_RETURN
        (soc_miim_read(unit, phy_addr, 0x18, &lane_ctrl));

    o_lane_ctrl = lane_ctrl;
    lane_ctrl &= ~0x8ff;
    /* Note that register bit value 1 means power down */
    power_mask = (power_mask & 0xf) ^ 0xf;
    /* Set power for both Rx and Tx lanes */
    lane_ctrl |= (power_mask << 4) | power_mask;
    if (power_mask) {
        /* Force power down */
        lane_ctrl |= 0x800;
    }

    if (o_lane_ctrl != lane_ctrl) {
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x18, lane_ctrl));
    }

    SOC_IF_ERROR_RETURN     /* Select block 0 */
        (soc_miim_write(unit, phy_addr, 0x1f, 0));

    return SOC_E_NONE;
}
#endif

/*
 * Function:
 *     soc_unicore_speed_set
 * Purpose:
 *     Get current Unicore speed.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     speed - new XGXS speed.
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *      The Start Sequencer bit in the xgxsControl register should
 *      be deasserted before calling this function to guarantee a
 *      glitch-free clock change.
 */
int
soc_unicore_speed_set(int unit, soc_port_t port, int speed)
{
    int    speed_val;
    uint8  phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    uint16 mii_ctrl, misc1, misc_ctrl;
    uint16 block_select;

    if (IS_HG_PORT(unit, port)) {
        if ((speed != 0) && (speed < 10000)) {
            return SOC_E_PARAM;
        }
        if (SOC_INFO(unit).port_speed_max[port] > 0 &&
            speed > SOC_INFO(unit).port_speed_max[port]) {
            return SOC_E_PARAM;
        }
    } else if (IS_XE_PORT(unit, port)) {
        if ((speed != 0) && ((speed < 1000) || (speed > 10000))) {
            return SOC_E_PARAM;
        }
    } else {
        if ((speed < 0) || (speed > 2500)) {
            return SOC_E_PARAM;
        }
    }

    switch (speed) {
    case 13000:
        speed_val = 7;
        break;
    case 12500:
        speed_val = 6;
        break;
    case 12000:
        speed_val = 5;
        break;
    case 10000:
        speed_val = 4; /* 10G CX4 */
        if (IS_HG_PORT(unit, port) && 
            soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE) == FALSE) {
            speed_val = 3; /* 10G HiG */
        }
        break;
    case 2500:
        speed_val = 0;
        break;
    case 1000:
    case 100:
    case 10:
        speed_val = -1;
        break;
    case 0:
        return SOC_E_NONE;
    case 16000:
    case 15000:
    case 6000:
    case 5000:
    default:
        return SOC_E_PARAM;
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) {
        /* Save original block select mapping */
        SOC_IF_ERROR_RETURN
            (soc_miim_read(unit, phy_addr, 0x1f, &block_select)); 
     
        /* Select block DIGITAL (0x30) */
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0300));
    
        /* Set speed in Misc1 register */
        SOC_IF_ERROR_RETURN
            (soc_miim_read(unit, phy_addr, 0x18, &misc1));
        misc1 &= ~0x001f;
        if (speed_val >= 0) {
            misc1 |= 0x0010 | speed_val;
        }
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x18, misc1));
    
        /* Select block BLOCK0 */
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
    
        /* Map XAUI or SerDes IEEE registers */
        SOC_IF_ERROR_RETURN
            (soc_miim_read(unit, phy_addr, 0x1e, &misc_ctrl));
        /* Disable dynamic IEEE address space mapping and 
         * map Serdes IEEE address space.
         */
        misc_ctrl &= ~0x0003;
        if (speed > 2500) {
            misc_ctrl |= 0x0001; /* Select XAUI registers */
        }
    
        /* Write final IEEE register mapping */
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x1e, misc_ctrl));
    
        /* 
         * Speeds of 2.5 Gbps and below must be set in IEEE registers.
         */
        if (speed <= 2500) {
            SOC_IF_ERROR_RETURN
                (soc_miim_read(unit, phy_addr, MII_CTRL_REG, &mii_ctrl));
            mii_ctrl &= ~(MII_CTRL_SS_MASK | MII_CTRL_FS_2500);
            if (speed == 2500) {
                /* Force 2.5 Gbps */
                mii_ctrl |= MII_CTRL_FS_2500;
            } else if (speed == 1000) {
                mii_ctrl |= MII_CTRL_SS_1000;
            } else if (speed == 100) {
                mii_ctrl |= MII_CTRL_SS_100;
            } else {
                mii_ctrl |= MII_CTRL_SS_10;
            }
            SOC_IF_ERROR_RETURN
                (soc_miim_write(unit, phy_addr, MII_CTRL_REG, mii_ctrl));
        }
    
        /* Restore Original Block select mapping */
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, 0x1f, block_select));
    }
 
    if (speed < 10000) {
        SOC_IF_ERROR_RETURN
            (gxmac_interface_set(unit, port, SOC_PORT_IF_GMII));
    } else {
        SOC_IF_ERROR_RETURN
            (gxmac_interface_set(unit, port, SOC_PORT_IF_XGMII));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_unicore_speed_get
 * Purpose:
 *     Get current Unicore speed.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     speed - (OUT) current XGXS speed (forced or negotiated).
 *     an_done - (OUT) if true, auto-negotiation is complete. This value
 *	     	 is undefined unless auto-negotiation is enabled.
 * Returns:
 *     SOC_E_XXX
 */
int
soc_unicore_speed_get(int unit, soc_port_t port, int *speed, int *an_done)
{
    uint64 xgxs_stat;
    uint32 i;

    static struct {
        soc_field_t field;
        int speed;
    } _sp_sel[] = {
        { SPEED_13000f,     13000 },
        { SPEED_12500f,     12500 },
        { SPEED_12000f,     12000 },
        { SPEED_10000_CX4f, 10000 },
        { SPEED_10000f,     10000 },
        { SPEED_6000f,      6000  },
        { SPEED_5000f,      5000  },
        { SPEED_2500f,      2500  },
        { SPEED_1000f,      1000  },
        { SPEED_100f,       100   },
        { SPEED_10f,        10    }
    };

    /* Provide reasonable default value */
    *speed = IS_GE_PORT(unit, port) ? 1000 : 10000;

    SOC_IF_ERROR_RETURN
        (READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));

    for (i = 0; i < COUNTOF(_sp_sel); i++) {
        if (soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                  _sp_sel[i].field)) {
            *speed = _sp_sel[i].speed;
            break;
        }
    }

    if (IS_HG_PORT(unit, port) && *speed < 10000) {
        *speed = 0;
    }

    /* Ignore if null pointer */
    if (an_done) {
        *an_done = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                         AUTONEG_COMPLETEf);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_unicore_mac_mode_set
 * Purpose:
 *     Set current Unicore MAC mode.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     mode - new XGXS MAC mode (TriMAC or BigMAC).
 *     enable - if zero, disable both MACs
 * Returns:
 *     SOC_E_NONE
 * Note:
 *     Don't need to restore old MAC states to new MAC when switching
 *  mode. All MACs are already in the same state.
 */
int
soc_unicore_mac_mode_set(int unit, soc_port_t port, 
                         soc_mac_mode_t mode, int enable)
{
    int gmac_en, bmac_en, femac_en, gport_en, xport_en;
    uint32 xport, gport;
    uint64 ctrl, octrl;
    uint32 gmacc1, ogmacc1;
    uint32 fe_mac1, ofe_mac1;

    xport_en = (mode == SOC_MAC_MODE_10000) ? 1 : 0;

    /*
     * Note that for correct LED operation, either the XPORT or the
     * GPORT must be enabled, even when both MACs are disabled.
     */
    gport_en = !xport_en;

    femac_en = gmac_en = bmac_en = 0;
    if (enable) {
        switch (mode) {
        case SOC_MAC_MODE_10000:
            bmac_en = 1;
            break;
        case SOC_MAC_MODE_1000_T:
            gmac_en   = 1;
            break;  
        case SOC_MAC_MODE_10_100: /* fall through */
        case SOC_MAC_MODE_10:
            femac_en  = 1;
            break;
        default:
            return SOC_E_INTERNAL;
        }
    }

    SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &gport));
    SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &xport));

    /*
     * When switching between MAC modes, both GPORT and XPORT should be
     * disabled before enabling the appropriate PORT. However, we should
     * only disable the ports if necessary. In particular, when disabling
     * a port the xPORT_EN should remain set to avoid incoming packets
     * getting chopped in the middle. Only by disabling the MAC Rx we
     * can ensure that packets are stopped at a packet boundary.
     */
    if (xport_en) {
        if (soc_reg_field_valid(unit, GPORT_CONFIGr, GPORT_ENf)) {
            soc_reg_field_set(unit, GPORT_CONFIGr, &gport, GPORT_ENf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, gport));
    } else {
        soc_reg_field_set(unit, XPORT_CONFIGr, &xport, XPORT_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, xport));
    }

    /* Enable/disable TriMAC */
    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    ogmacc1 = gmacc1;
    soc_reg_field_set(unit, GMACC1r, &gmacc1, TXEN0f, 1);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, RXEN0f, gmac_en);
    if (gmacc1 != ogmacc1) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
    }

    SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &fe_mac1));
    ofe_mac1 = fe_mac1;
    soc_reg_field_set(unit, FE_MAC1r, &fe_mac1, RX_ENf, femac_en);
    if (fe_mac1 != ofe_mac1) {
        SOC_IF_ERROR_RETURN(WRITE_FE_MAC1r(unit, port, fe_mac1));
    }

    if (gport_en) {
        if (soc_reg_field_valid(unit, GPORT_CONFIGr, GPORT_ENf)) {
            soc_reg_field_set(unit, GPORT_CONFIGr, &gport, GPORT_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, gport));
    }

    /* Enable/disable BigMAC */
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, TXENf, 1);
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, RXENf, bmac_en);
    if (COMPILER_64_NE(ctrl, octrl)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, ctrl));
    }
    if (xport_en) {
        soc_reg_field_set(unit, XPORT_CONFIGr, &xport, XPORT_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, xport));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_unicore_mac_mode_get
 * Purpose:
 *     Get current Unicore MAC mode.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     mode - (OUT) current XGXS MAC mode (TriMAC or BigMAC).
 * Returns:
 *     SOC_E_NONE
 */
int
soc_unicore_mac_mode_get(int unit, soc_port_t port, soc_mac_mode_t *mode)
{
    int cur_speed;
    SOC_IF_ERROR_RETURN
            (soc_unicore_speed_get(unit, port, &cur_speed, NULL));
    switch (cur_speed) {
        case 16000: /* fall through */
        case 15000: /* fall through */
        case 13000: /* fall through */
        case 12500: /* fall through */
        case 12000: /* fall through */
        case 10000: 
        *mode = SOC_MAC_MODE_10000;
            break;
        case 2500: /* fall through */
        case 1000:
        *mode = SOC_MAC_MODE_1000_T;
            break;
        case 100:
            *mode = SOC_MAC_MODE_10_100;
            break;
        case 10:
            *mode = SOC_MAC_MODE_10;
            break;
        default:
            return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_ifg_to_ipg
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
 */
STATIC int
gxmac_ifg_to_ipg(int unit, soc_port_t port, int speed, int duplex,
                int ifg, int *ipg)
{
    /* The inter-frame gap should be a multiple of 8 bit-times */
     ifg = (ifg + 7) & ~7;

     /* The smallest supported ifg is 64 bit-times */
     ifg = (ifg < 64) ? 64 : ifg;
     
    if (duplex) {
        if (speed == 10 || speed == 100) {
            *ipg = ifg / 4 - 3;
        } else {
            *ipg = ifg / 8;
        }
    } else {
        switch (speed) {
        case 10:
            *ipg = ifg / 4 - 12;
            break;
        case 100:
            *ipg = ifg / 4 - 13;
            break;
        default:
            return SOC_E_INTERNAL;
        }
    }

    if (IS_HG_PORT(unit, port) && *ipg >= 4) {
        *ipg -= 4;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_ipg_to_ifg
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
 */
STATIC int
gxmac_ipg_to_ifg(int unit, soc_port_t port, int speed, int duplex,
                int ipg, int *ifg)
{
     /*
      * Now we need to convert the value accoring to various chips' 
      * peculiarities (there are none as of now)
      */
     if (IS_HG_PORT(unit, port)) {
         ipg += 4;
     }

     if (duplex) {
         if (speed == 10 || speed == 100) { 
             *ifg = (ipg + 3) * 4;
         } else {
             *ifg = ipg * 8;
         }
     } else {
         switch (speed) {
         case 10:
             *ifg = (ipg + 12) * 4;
             break;
         case 100:
             *ifg = (ipg + 13) * 4;
             break;
         default:
             return SOC_E_INTERNAL;
         }
     }

     return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_ipg_update
 * Purpose:
 *      Set the IPG appropriate for current speed/duplex
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 * Notes:
 *      The current speed/duplex are read from the hardware registers.
 */
STATIC int
gxmac_ipg_update(int unit, int port)
{
    int                 fd, speed, ifg, ipg;
    soc_ipg_t           *si = &SOC_PERSIST(unit)->ipg[port];
    uint64              tx_ctrl, otx_ctrl;
    uint32              gmacc2, ogmacc2, fe_ipg;

    SOC_IF_ERROR_RETURN(gxmac_duplex_get(unit, port, &fd));
    SOC_IF_ERROR_RETURN(gxmac_speed_get(unit, port, &speed));

    /* Get the ifg value in bit-times */
    if (fd) {
        switch (speed) {
        case 0:
            ifg = IS_GE_PORT(unit, port) ? si->fd_1000 : si->fd_10000;
            break;
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
        case 10000: /* fall through */
        case 12000: /* fall through */
        case 12500: /* fall through */
        case 13000: /* fall through */
        case 15000: /* fall through */
        case 16000: /* fall through */
            ifg = si->fd_10000;
            break;
        default:
            return SOC_E_INTERNAL;
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
        }
    }

    /* Convert the ifg value from bit-times into IPG register-specific value */
    SOC_IF_ERROR_RETURN(gxmac_ifg_to_ipg(unit, port, speed, fd, ifg, &ipg));


    /* Program the appropriate register */
    switch (speed) {
    case 10:
    case 100:
        fe_ipg = 0;
        soc_reg_field_set(unit, FE_IPGTr, &fe_ipg, IPGTf, ipg);
        SOC_IF_ERROR_RETURN(WRITE_FE_IPGTr(unit, port, fe_ipg));
        break;        
    case 2500:
    case 1000:
        SOC_IF_ERROR_RETURN(READ_GMACC2r(unit, port, &gmacc2));
        ogmacc2 = gmacc2;
        soc_reg_field_set(unit, GMACC2r, &gmacc2, IPGTf, ipg);
        if (gmacc2 != ogmacc2) {
            SOC_IF_ERROR_RETURN(WRITE_GMACC2r(unit, port, gmacc2));
        }
        break;
    default:
        SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx_ctrl));
        otx_ctrl = tx_ctrl;

        
        if (soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE)) {
            /* Back to back 64 bytes packet must have IPG = 19 to 
             * prevent data rate higher than acceptable WAN mode line rate.
             */ 
            if (ipg < 19) {
                ipg = 19;
            }

            /* Insert an Idle after every 13 bytes of data to keep
             * the data rate lower than 9.294Gbps.
             */
            soc_reg64_field32_set(unit, MAC_TXCTRLr, &otx_ctrl, THROTDENOMf,
                                  13);              
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
            if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
                soc_reg64_field32_set(unit, MAC_TXCTRLr, &otx_ctrl, THROTNUMERf, 1);
            }
            else
#endif
            {
                soc_reg64_field32_set(unit, MAC_TXCTRLr, &otx_ctrl, THROTNUMf, 1);
            }
        }

        soc_reg64_field32_set(unit, MAC_TXCTRLr, &otx_ctrl, AVGIPGf, 
                              ipg & 0x1f);
        if (COMPILER_64_NE(tx_ctrl, otx_ctrl)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, otx_ctrl));
        }
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_gxmac_trimac_init(int unit, soc_port_t port)
{
    uint32              gmacc0, gmacc1, gmacc2, val32, pctrl;

    /*
     * Perform length check on Ethernet frames and do not allow huge frames
     */
    SOC_IF_ERROR_RETURN(READ_FE_MAC2r(unit, port, &val32));
    soc_reg_field_set(unit, FE_MAC2r, &val32, FULL_DUPf, 1);
    soc_reg_field_set(unit, FE_MAC2r, &val32, EXC_DEFf, 1);
    soc_reg_field_set(unit, FE_MAC2r, &val32, LG_CHKf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, HUGE_FRf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, DEL_CRCf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, CRC_ENf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, PAD_ENf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, VLAN_PADf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, AUTO_PADf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, PURE_PADf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, LONG_PREf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, NO_BOFFf, 0);
    soc_reg_field_set(unit, FE_MAC2r, &val32, BP_NO_BOFFf, 0);
    SOC_IF_ERROR_RETURN(WRITE_FE_MAC2r(unit, port, val32));

    SOC_IF_ERROR_RETURN(READ_FE_CLRTr(unit, port, &val32));
    soc_reg_field_set(unit, FE_CLRTr, &val32, COL_WINf, 0x37);
    soc_reg_field_set(unit, FE_CLRTr, &val32, RETRYf, 0xf);
    SOC_IF_ERROR_RETURN(WRITE_FE_CLRTr(unit, port, val32));

    /* Set receive IPG for FE MAC */
    val32 = 0;
    soc_reg_field_set(unit, FE_IPGRr, &val32, IPGR1f, 0x6);
    soc_reg_field_set(unit, FE_IPGRr, &val32, IPGR2f, 0xf);
    SOC_IF_ERROR_RETURN(WRITE_FE_IPGRr(unit, port, val32));

    /* Update transmit IPG for FE MAC */ 
    val32 = 0;
    soc_reg_field_set(unit, FE_IPGTr, &val32, IPGTf, 0xb);
    SOC_IF_ERROR_RETURN(WRITE_FE_IPGTr(unit, port, val32));

    SOC_IF_ERROR_RETURN(READ_FE_SUPPr(unit, port, &val32));
    soc_reg_field_set(unit, FE_SUPPr, &val32, SPEEDf, 1);
    soc_reg_field_set(unit, FE_SUPPr, &val32, BIT_MODEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_FE_SUPPr(unit, port, val32));

    SOC_IF_ERROR_RETURN(READ_FE_TESTr(unit, port, &val32));
    soc_reg_field_set(unit, FE_TESTr, &val32, SHORT_QNTAf, 0);
    soc_reg_field_set(unit, FE_TESTr, &val32, TEST_PAUSEf, 0);
    soc_reg_field_set(unit, FE_TESTr, &val32, TEST_BACKf, 0);
    SOC_IF_ERROR_RETURN(WRITE_FE_TESTr(unit, port, val32));

    SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &val32));
    soc_reg_field_set(unit, FE_MAC1r, &val32, LBACKf, 0);
    soc_reg_field_set(unit, FE_MAC1r, &val32, RX_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_FE_MAC1r(unit, port, val32));

    /* Clear station address */
    val32 = 0;
    SOC_IF_ERROR_RETURN(WRITE_ESA0r(unit, port, val32));
    SOC_IF_ERROR_RETURN(WRITE_ESA1r(unit, port, val32));
    SOC_IF_ERROR_RETURN(WRITE_ESA2r(unit, port, val32));

    /* NOTE : Allow dribble packets? */

    /* Initialize 1000 Mbps MAC */
    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));
    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    SOC_IF_ERROR_RETURN(READ_GMACC2r(unit, port, &gmacc2));

    soc_reg_field_set(unit, GMACC0r, &gmacc0, SRSTf, 0);
    soc_reg_field_set(unit, GMACC0r, &gmacc0, L10Bf, 0);
    soc_reg_field_set(unit, GMACC0r, &gmacc0, L32Bf, 0);
    soc_reg_field_set(unit, GMACC0r, &gmacc0, TMDSf, 0);

    soc_reg_field_set(unit, GMACC1r, &gmacc1, FULLDf, 1);  /* arbitrary */
    if (IS_ST_PORT(unit, port)) {
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCTXf, 0);  /* Flow ctl */
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCRXf, 0);  /* Flow ctl */
    } else { 	 
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCTXf, 1);  /* Flow ctl */ 	 
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCRXf, 1);  /* Flow ctl */ 	 
    }
    soc_reg_field_set(unit, GMACC1r, &gmacc1, LONGPf, 1);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, MIFGf, 2);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, GLVRf, 1);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, TXEN0f, 1);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, RXEN0f, 0);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, JUMBOf, 1);  /* HUGEN=0 */
    
    soc_reg_field_set(unit, GMACC2r, &gmacc2, IPGTf, 0xc);

    SOC_IF_ERROR_RETURN(WRITE_GMACC2r(unit, port, gmacc2));
    SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
    SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0));
    
    pctrl = 0;
    soc_reg_field_set(unit, PAUSE_CONTROLr, &pctrl, ENABLEf, 1);
    /* Max out the pause duration; "resume" pause frame will end pause */
    soc_reg_field_set(unit, PAUSE_CONTROLr, &pctrl, VALUEf,
                      (1 << soc_reg_field_length(unit,
                                                 PAUSE_CONTROLr,
                                                 VALUEf)) - 1);
    SOC_IF_ERROR_RETURN(WRITE_PAUSE_CONTROLr(unit, port, pctrl));

    val32 = 0;
    SOC_IF_ERROR_RETURN(WRITE_TEST2r(unit, port, val32));


    /* Clear station address */
    val32 = 0;
    SOC_IF_ERROR_RETURN(WRITE_GSA0r(unit, port, val32));
    SOC_IF_ERROR_RETURN(WRITE_GSA1r(unit, port, val32));

    /* Set egress enable */
    SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &val32));
    soc_reg_field_set(unit, EGR_ENABLEr, &val32, PRT_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, val32));

    /* Update IPG for GE MAC */
    SOC_IF_ERROR_RETURN(gxmac_ipg_update(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_init
 * Purpose:
 *      Initialize GMAC / Unicore MAC into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
gxmac_init(int unit, soc_port_t port)
{
    uint64              rx_ctrl, tx_ctrl, mac_ctrl;
    int                 encap;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    SOC_IF_ERROR_RETURN(_gxmac_trimac_init(unit, port));

    /*
     * Initialize BigMAC
     */

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rx_ctrl));
    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx_ctrl));
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &mac_ctrl));

    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RXENf, 0);
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, TXENf, 0);
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RMTLOOPf, 0);
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, LCLLOOPf, 0);
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, PAUSEENf, 0);
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, RXPAUSENf, 0);

    SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, mac_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, tx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, rx_ctrl));

    /* Re-read MAC_TXCTRL, since it gets affected by ipg_update() */
    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx_ctrl));

    /* Set jumbo max size for 10G and Higig by default */
    if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(gxmac_frame_max_set(unit, port, JUMBO_MAXSZ));
    }

    /* Program MAC settings */

    /* Setup header mode */
    encap = (IS_HG_PORT(unit, port)) ? SOC_ENCAP_HIGIG : SOC_ENCAP_IEEE;
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, HDRMODEf, encap);
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, HDRMODEf, encap);

    /*
     * Power-up defaults are CRC_APPEND for TX and STRIPCRC for RX.
     * Change to CRC_KEEP for Hercules, CRC_REGEN for other XGS,
     * and turn off STRIPCRC to be compatible with other BCM56xx.
     */
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, STRIPCRCf, 0);
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, CRC_MODEf, 
                          GXMAC_CRC_REGEN);
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, tx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, rx_ctrl));
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
         int wan_mode = soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE);
         if (wan_mode) {
            /* Max speed for WAN mode is 9.294Gbps.
              * This setting gives 10Gbps * (13/14) or 9.286 Gbps */
            SOC_IF_ERROR_RETURN
                       (gxmac_control_set(unit, port,
                           SOC_MAC_CONTROL_FRAME_SPACING_STRETCH, 13));
         }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    /* Update IPG separately */
    SOC_IF_ERROR_RETURN(gxmac_ipg_update(unit, port));

    /*
     * Note that we leave Rx and Tx in the disabled state.
     * Rx and Tx are enabled for the appropriate MAC (BigMAC/TriMAc)
     * in gxmac_enable_set.
     */

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_enable_set(int unit, soc_port_t port, int enable)
{
    uint32 val, val2, drain_cells;
    pbmp_t mask;
    int cur_speed;
    soc_mac_mode_t mode;
    int drain_usec, loopback = -1;
    soc_timeout_t to;
    int rv = SOC_E_NONE;

    /* Ensure we match the register field size */
    if (enable) {
        enable = 1;
    }

    /* Select appropriate MAC */
    SOC_IF_ERROR_RETURN(soc_unicore_speed_get(unit, port, &cur_speed, NULL));
    if (cur_speed > 2500 || IS_HG_PORT(unit, port)) {
        mode = SOC_MAC_MODE_10000;
    } else if (cur_speed >= 1000) {
        mode = SOC_MAC_MODE_1000_T;
    } else if (cur_speed >= 100) {
        mode = SOC_MAC_MODE_10_100;
    } else {
        mode = SOC_MAC_MODE_10;
    }

    /*
     * Use EPC_LINK to control other ports sending to this port.  This
     * blocking is independent and in addition to what Linkscan does.
     */
    soc_link_mask2_get(unit, &mask);
    if (enable) {
        SOC_PBMP_PORT_ADD(mask, port);
        /* Postpone write until MAC is enabled */
    } else {
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }

    if (!(SOC_IS_RELOADING(unit)))
    {
        /* 
        * Drain all buffers and FIFOs if we are disabled
        */
        if (!enable) {
            /* Start flushing BigMAc and TriMAC */
            SOC_IF_ERROR_RETURN(WRITE_XP_EGR_PKT_DROP_CTLr(unit, port, 1));
            SOC_IF_ERROR_RETURN(WRITE_GE_EGR_PKT_DROP_CTLr(unit, port, 1));

            /* Do not drain forever in case of problems */
            drain_usec = soc_property_get(unit, spn_LCCDRAIN_TIMEOUT_USEC, 250000);
            soc_timeout_init(&to, drain_usec, 0);

            /* Drain MMU and GXPORT in one go */
            drain_cells = 0xffffffff;
            do {
                if (SOC_REG_IS_VALID(unit, OP_PORT_TOTAL_COUNTr)) {
                    SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNTr(unit, port, &val));
                } else {
                    SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &val));
                }
                SOC_IF_ERROR_RETURN(READ_XP_XBODE_CELL_CNTr(unit, port, &val2));
                val += val2;
                SOC_IF_ERROR_RETURN(READ_GE_GBODE_CELL_CNTr(unit, port, &val2));
                val += val2;
                /* Gig MAC in pause state needs loopback mode to clear xoff */
                if (drain_cells == val && loopback < 0 &&
                    mode == SOC_MAC_MODE_1000_T) {
                        if (gxmac_loopback_get(unit, port, &loopback) < 0) {
                            loopback = -1;
                        } else {
                            gxmac_loopback_set(unit, port, 1);
                        }
                }
                /* Check for progress */
                if (val < drain_cells) {
                    soc_timeout_init(&to, drain_usec, 0);
                }
                drain_cells = val;
            }
            while (drain_cells && !soc_timeout_check(&to));

            /* Stop flushing */
            SOC_IF_ERROR_RETURN(WRITE_XP_EGR_PKT_DROP_CTLr(unit, port, 0));
            SOC_IF_ERROR_RETURN(WRITE_GE_EGR_PKT_DROP_CTLr(unit, port, 0));

            /* Restore loopback setting if changed */
            if (loopback >= 0) {
                SOC_IF_ERROR_RETURN
                    (gxmac_loopback_set(unit, port, loopback));
            }

            /* Check for drain error */
            if (drain_cells) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: port %d:%s: "
                                      "timeout draining packets (%d cells remain)\n"),
                           unit, SOC_PORT_NAME(unit, port), drain_cells));
                /* Indicate error, but continue to MAC disable below */
                rv = SOC_E_INTERNAL;
            }
        }
    }

    /* Set new MAC mode (or disable MACs) */
    SOC_IF_ERROR_RETURN(soc_unicore_mac_mode_set(unit, port, mode, enable));

    if (enable) {
        /* Set EPC_LINK */
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }

    return rv;
}

/*
 * Function:
 *      gxmac_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_enable_get(int unit, soc_port_t port, int *enable)
{
    uint32 fe_mac1, gmacc1;
    uint64 ctrl;

    /* Instead of checking the current mode we read from all MACs */

    /* TriMAC */
    SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &fe_mac1));
    *enable = soc_reg_field_get(unit, FE_MAC1r, fe_mac1, RX_ENf);

    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    *enable |= soc_reg_field_get(unit, GMACC1r, gmacc1, RXEN0f);

    /* BigMAC */
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    *enable |= soc_reg64_field32_get(unit, MAC_CTRLr, ctrl, RXENf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_enable_get:u=%d port=%d enable=%c\n"),
                 unit, port, *enable ? 'T' : 'F'));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_duplex_set
 * Purpose:
 *      Set  GXMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
gxmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    /* Always full duplex */
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(duplex);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_duplex_set: unit %d port %s %s duplex\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "FULL" : "HALF"));

    if (IS_GE_PORT(unit, port)) {
        int cur_speed;

        SOC_IF_ERROR_RETURN
            (soc_unicore_speed_get(unit, port, &cur_speed, NULL));

        /* Do not support half duplex for 1Gbps speed */
        if (cur_speed <= 100) { 
            uint32 fe_mac2, ofe_mac2;
            duplex = duplex ? 1 : 0;

            SOC_IF_ERROR_RETURN(READ_FE_MAC2r(unit, port, &fe_mac2));
            ofe_mac2 = fe_mac2;
            soc_reg_field_set(unit, FE_MAC2r, &fe_mac2, FULL_DUPf, duplex);
            if (fe_mac2 != ofe_mac2) {
                /* Set duplex for FE MAC */ 
                SOC_IF_ERROR_RETURN(WRITE_FE_MAC2r(unit, port, fe_mac2));

                /* Set IPG to match new duplex */
                SOC_IF_ERROR_RETURN(gxmac_ipg_update(unit, port));
          
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_duplex_get
 * Purpose:
 *      Get GXMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *duplex = TRUE; /* HG and XE ports are always full duplex */

    if (IS_GE_PORT(unit, port)){
        int cur_speed;

        SOC_IF_ERROR_RETURN
            (soc_unicore_speed_get(unit, port, &cur_speed, NULL));
        if (cur_speed <= 100) {
            uint32 fe_mac2;

            SOC_IF_ERROR_RETURN(READ_FE_MAC2r(unit, port, &fe_mac2));
            *duplex = soc_reg_field_get(unit, FE_MAC2r, fe_mac2, FULL_DUPf);
        }
    } 
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_duplex_get: unit %d port %s %s duplex\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "FULL" : "HALF"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_pause_set
 * Purpose:
 *      Configure GXMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    uint64 ctrl, octrl;
    uint32 gmacc1, ogmacc1;
    uint32 fe_mac1, ofe_mac1;

    pause_tx = pause_tx ? 1 : 0;
    pause_rx = pause_rx ? 1 : 0;

    /* TriMAC */
    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    ogmacc1 = gmacc1;
    SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &fe_mac1));
    ofe_mac1 = fe_mac1;

    if (pause_tx >= 0) {
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCTXf, pause_tx);
        soc_reg_field_set(unit, FE_MAC1r, &fe_mac1,
                          TX_PAUf, pause_tx);
    }
    if (pause_rx >= 0) {
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCRXf, pause_rx);
        soc_reg_field_set(unit, FE_MAC1r, &fe_mac1,
                          RX_PAUf, pause_rx);
    }

    if (gmacc1 != ogmacc1) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
    }
    if (fe_mac1 != ofe_mac1) {
        SOC_IF_ERROR_RETURN(WRITE_FE_MAC1r(unit, port, fe_mac1));
    }

    /* BigMAC */
    if (pause_tx >= 0) {
        SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &ctrl));
        octrl = ctrl;
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &ctrl, PAUSEENf,
                              pause_tx ? 1 : 0);
        if (COMPILER_64_NE(ctrl, octrl)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, ctrl));
        }
    }
    if (pause_rx >= 0) {
        SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &ctrl));
        octrl = ctrl;
        soc_reg64_field32_set(unit, MAC_RXCTRLr, &ctrl, RXPAUSENf,
                              pause_rx ? 1 : 0);
        if (COMPILER_64_NE(ctrl, octrl)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, ctrl));
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_pause_set: unit %d port %s RX=%s TX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_rx ? "on" : "off",
                 pause_tx ? "on" : "off"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_pause_get
 * Purpose:
 *      Return the pause ability of GXMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause
 *      pause_rx - Boolean: receive pause
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint32 gmacc1;

    /* Read TriMAC status only */
    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    *pause_tx = soc_reg_field_get(unit, GMACC1r, gmacc1, FCTXf);
    *pause_rx = soc_reg_field_get(unit, GMACC1r, gmacc1, FCRXf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_pause_get: unit %d port %s RX=%s TX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *pause_rx ? "on" : "off",
                 *pause_tx ? "on" : "off"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_speed_set
 * Purpose:
 *      Set BigMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 1000, 2500, ... 13000.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
gxmac_speed_set(int unit, soc_port_t port, int speed)
{
    soc_mac_mode_t mode;
    int enable;
    uint32 speed_sel, fe_supp;

    /* Also set the Unicore speed here for MAC loopback support */
    SOC_IF_ERROR_RETURN(soc_unicore_speed_set(unit, port, speed));

    /* Disable MACs before doing anything else */
    SOC_IF_ERROR_RETURN(gxmac_enable_get(unit, port, &enable));
    SOC_IF_ERROR_RETURN(gxmac_enable_set(unit, port, 0));

    speed_sel = 0;
    fe_supp   = 0;
    if (speed > 2500) {
        mode      = SOC_MAC_MODE_10000;
    } else if (speed >=1000) {
        speed_sel = 0;
        mode      = SOC_MAC_MODE_1000_T;
    } else if (speed >=100) {
        fe_supp   = 1;
        speed_sel = 1;
        mode      = SOC_MAC_MODE_10_100;
    } else {
        fe_supp   = 0;
        speed_sel = 2;
        mode      = SOC_MAC_MODE_10;
    }

    if (speed <= 1000) { 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, GE_PORT_CONFIGr, port, 
                                    SPEED_SELECTf, speed_sel));

        if (speed <= 100) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FE_SUPPr, port, SPEEDf, fe_supp));
        }
    }

    /* Select appropriate MAC */
    SOC_IF_ERROR_RETURN(soc_unicore_mac_mode_set(unit, port, mode, enable));

    /* Update IPG for new speed */
    SOC_IF_ERROR_RETURN(gxmac_ipg_update(unit, port));

    /* Re-enable MAC */
    SOC_IF_ERROR_RETURN(gxmac_enable_set(unit, port, enable));

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_speed_set: unit %d port %s speed %dMb\n"),
                 unit, SOC_PORT_NAME(unit, port), speed));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_speed_get
 * Purpose:
 *      Get GXMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mbps
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv;

    rv = soc_unicore_speed_get(unit, port, speed, NULL);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_speed_get: unit %d port %s speed %dMb\n"),
                 unit, SOC_PORT_NAME(unit, port), *speed));
    return rv;
}

/*
 * Function:
 *      gxmac_loopback_set
 * Purpose:
 *      Set a GXMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On bigmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_loopback_set(int unit, soc_port_t port, int lb)
{
    uint64 ctrl, octrl;
    uint32 gmacc0, ogmacc0;
    uint32 fe_mac1, ofe_mac1;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_loopback_set: unit %d port %s %s loopback\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "local" : "no"));

    lb = lb ? 1 : 0;

    /* TriMAC */
    SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &fe_mac1));
    ofe_mac1 = fe_mac1;
    soc_reg_field_set(unit, FE_MAC1r, &fe_mac1, LBACKf, lb); 
    if (fe_mac1 != ofe_mac1) {
        SOC_IF_ERROR_RETURN(WRITE_FE_MAC1r(unit, port, fe_mac1));
    }

    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));
    ogmacc0 = gmacc0;
    soc_reg_field_set(unit, GMACC0r, &gmacc0, L32Bf, lb);
    if (gmacc0 != ogmacc0) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0));
    }

    /* BigMAC */
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, LCLLOOPf, lb);
    if (COMPILER_64_NE(ctrl, octrl)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, ctrl));
    }

#ifdef GXPORT_ISLOATE_IF_LOOPBACK
    /* Power lanes up/down */
    soc_unicore_lane_power_set(unit, port, lb ? 0 : 0xf);
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_loopback_get
 * Purpose:
 *      Get current GXMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint64 ctrl;
    int local, remote;

    /* Read BigMAC status only */
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    remote = soc_reg64_field32_get(unit, MAC_CTRLr, ctrl, RMTLOOPf);
    local = soc_reg64_field32_get(unit, MAC_CTRLr, ctrl, LCLLOOPf);
    *lb = local | remote;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_loopback_get: unit %d port %s %s loopback\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *lb ? (remote ? "remote" : "local") : "no"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_pause_addr_set
 * Purpose:
 *      Configure PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint32 w0, w1, w2;
    uint32 gsa0, gsa1;
    uint32 esa0, esa1, esa2;
    uint64 macsa;

    w0 = mac[0] << 8 | mac[1];
    w1 = mac[2] << 8 | mac[3];
    w2 = mac[4] << 8 | mac[5];

    /* TriMAC */
    esa0 = 0;
    esa1 = 0;
    esa2 = 0;

    soc_reg_field_set(unit, ESA0r, &esa0, STAD0f, (mac[4] << 8) | mac[5]);
    soc_reg_field_set(unit, ESA1r, &esa1, STAD0f, (mac[2] << 8) | mac[3]);
    soc_reg_field_set(unit, ESA2r, &esa2, STAD0f, (mac[0] << 8) | mac[1]);

    SOC_IF_ERROR_RETURN(FE_WRITE(ESA0r, GTH_ESA0r, unit, port, esa0));
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA1r, GTH_ESA1r, unit, port, esa1));
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA2r, GTH_ESA2r, unit, port, esa2));

    gsa0 = 0;
    gsa1 = 0;
    soc_reg_field_set(unit, GSA0r, &gsa0, STAD1f, (w0 << 16) | w1);
    soc_reg_field_set(unit, GSA1r, &gsa1, STAD2f, w2);

    SOC_IF_ERROR_RETURN(WRITE_GSA0r(unit, port, gsa0));
    SOC_IF_ERROR_RETURN(WRITE_GSA1r(unit, port, gsa1));

    /* BigMAC */
    COMPILER_64_SET(macsa, w0, (w1 << 16) | w2);

    SOC_IF_ERROR_RETURN(WRITE_MAC_TXMACSAr(unit, port, macsa));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXMACSAr(unit, port, macsa));

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_pause_addr_set: unit %d port %s MAC=<"
                 "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_pause_addr_get
 * Purpose:
 *      Retrieve PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 * Notes;
 *       We always write the same thing to TX & RX SA
 *       so, we just return the contects on RXMACSA.
 */
STATIC int
gxmac_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 macsa;
    uint32 msw, lsw;

    /* Read BigMAC only */
    SOC_IF_ERROR_RETURN(READ_MAC_RXMACSAr(unit, port, &macsa));

    COMPILER_64_TO_32_HI(msw, macsa);
    COMPILER_64_TO_32_LO(lsw, macsa);

    mac[0] = (uint8)(msw >> 8);
    mac[1] = (uint8)(msw >> 0);
    mac[2] = (uint8)(lsw >> 24);
    mac[3] = (uint8)(lsw >> 16);
    mac[4] = (uint8)(lsw >> 8);
    mac[5] = (uint8)(lsw >> 0);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_pause_addr_get: unit %d port %s MAC=<"
                 "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_interface_set
 * Purpose:
 *      Set a GXMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - requested mode not supported.
 * Notes:
 */
STATIC int
gxmac_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    uint32 gmacc0, gpcsc;
    uint32 ogmacc0, ogpcsc;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_interface_set: unit %d port %s %s interface\n"),
                 unit, SOC_PORT_NAME(unit, port), gxmac_port_if_names[pif]));
#endif

    if (pif == SOC_PORT_IF_XGMII) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));
    SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
    ogmacc0 = gmacc0;
    ogpcsc = gpcsc;

    switch (pif) {
    case SOC_PORT_IF_MII:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_SGMII:
        soc_reg_field_set(unit, GMACC0r, &gmacc0, TMDSf, 1); /* GMII */
        soc_reg_field_set(unit, GPCSCr, &gpcsc, RCSELf, 1);  /* 125 MHz */
        break;
    case SOC_PORT_IF_TBI:
        soc_reg_field_set(unit, GMACC0r, &gmacc0, TMDSf, 0); /* TBI */
        soc_reg_field_set(unit, GPCSCr, &gpcsc, RCSELf, 0);  /* 62.5 MHz */
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    if (gpcsc != ogpcsc) {
        SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));
    }
    if (gmacc0 != ogmacc0) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      gxmac_interface_get
 * Purpose:
 *      Retrieve GXMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
gxmac_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    uint32 gmacc0;
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(soc_unicore_mac_mode_get(unit, port, &mode));

    if (mode == SOC_MAC_MODE_1000_T) {
        SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));

        *pif = (soc_reg_field_get(unit, GMACC0r, gmacc0, TMDSf) ?
                SOC_PORT_IF_GMII :
                SOC_PORT_IF_TBI);
    } else {
        *pif = SOC_PORT_IF_MII;
    }

    
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_interface_get: unit %d port %s %s interface\n"),
                 unit, SOC_PORT_NAME(unit, port), gxmac_port_if_names[*pif]));
#endif

    return SOC_E_NONE;
}


/*
 * Function:
 *      gxmac_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
gxmac_frame_max_set(int unit, soc_port_t port, int size)
{
    uint64 val64;
    uint32 val32;

    if (!IS_ST_PORT(unit, port)) {
        /* Account for vlan tagged packets */
        COMPILER_64_SET(val64, 0, size + 4);
    } else {
        COMPILER_64_SET(val64, 0, size);
    }

    /* BigMAC */
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXMAXSZr(unit, port, val64));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXMAXSZr(unit, port, val64));

    /* To be neutral to the above change */
    val32 = size;

    /* TriMAC */
    SOC_IF_ERROR_RETURN(WRITE_MAXFRr(unit, port, val32));

    size += 1; /* XGS3 FE MAC_FR is one greater than MAX len */
    val32 = size;
    return (WRITE_FE_MAXFr(unit, port, val32));
}

/*
 * Function:
 *      gxmac_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
gxmac_frame_max_get(int unit, soc_port_t port, int *size)
{
    int    rv;
    uint64 val64;

    /* Note: MAC_RXMAXSZ returns 0 if the port is in GPORT mode. */ 
    rv = READ_MAC_TXMAXSZr(unit, port, &val64);

    if (rv == SOC_E_NONE) {
        COMPILER_64_TO_32_LO(*size, val64);
        if (!IS_ST_PORT(unit, port)) {
            /* For VLAN tagged packets */
            *size -= 4;
        }
    }

    return rv;
}

/*
 * Function:
 *      gxmac_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
gxmac_ifg_set(int unit, soc_port_t port, int speed, soc_port_duplex_t duplex, 
               int ifg)
{
    int         cur_duplex;
    int         cur_speed;
    int         ipg;
    int         real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    int         rv;

    SOC_IF_ERROR_RETURN(gxmac_ifg_to_ipg(unit, port, speed, duplex, 
                                        ifg, &ipg));
    SOC_IF_ERROR_RETURN(gxmac_ipg_to_ifg(unit, port, speed, duplex, 
                                        ipg, &real_ifg));

    if (duplex) {
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
        case 10000:
        case 12000:
        case 12500:
        case 13000:
            si->fd_10000 = real_ifg;
            break;
        case 5000:
        case 6000:
        case 15000:
        case 16000:
        default:
            return SOC_E_PARAM;
        }
    } else {
        switch (speed) {
        case 10:
            si->hd_10 = real_ifg;
            break;
        case 100:
            si->hd_100 = real_ifg;
            break;
        default: 
        return SOC_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(gxmac_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(gxmac_speed_get(unit, port, &cur_speed));

    if (cur_speed == speed && ((soc_port_duplex_t)cur_duplex == duplex)) {
        rv = gxmac_ipg_update(unit, port);
    } else {
        rv = SOC_E_NONE;
    }
    
    return (rv);
}

/*
 * Function:
 *      gxmac_ifg_get
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
gxmac_ifg_get(int unit, soc_port_t port, int speed, soc_port_duplex_t duplex, 
             int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];

    if (duplex) {
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
        case 10000:
        case 12000:
        case 12500:
        case 13000:
            *ifg = si->fd_10000;
            break;
        case 5000:
        case 6000:
        case 15000:
        case 16000:
        default:
            return SOC_E_PARAM;
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
    }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_gxmac_port_mode_update
 * Purpose:
 *      Set the GXMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      to_hg_port - (TRUE/FALSE) 
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_gxmac_port_mode_update(int unit, soc_port_t port, int to_hg_port)
{
    uint32              rval;
    uint64              val64;
    soc_pbmp_t          ctr_pbmp;
    int                 rv = SOC_E_NONE;

    /* Pause linkscan */
    soc_linkscan_pause(unit);
    
    /* Pause counter collection */
    COUNTER_LOCK(unit);
    
    soc_xport_type_update(unit, port, to_hg_port);

    /* Now that we've patched, reinit the port */
    rv = soc_unicore_reset(unit, port);

    if (rv >= 0) {
        rv = gxmac_init(unit, port);
    }

    if (rv >= 0) {
        rv = gxmac_enable_set( unit, port, 0);
    }

    if (rv >= 0) {
        SOC_PBMP_CLEAR(ctr_pbmp);
        SOC_PBMP_PORT_SET(ctr_pbmp, port);
        COMPILER_64_SET(val64, 0, 0);
        rv = soc_counter_set_by_port(unit, ctr_pbmp, val64);
    }

    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit); 

    SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &rval));
    if (to_hg_port) {
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
    } else {
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));

    return rv;
}

/*
 * Function:
 *      gxmac_encap_set
 * Purpose:
 *      Set the GXMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_encap_set(int unit, soc_port_t port, int mode)
{
    uint64 rx_ctrl, tx_ctrl, orx, otx;
    int encap = 0; /* IEEE */
    int higig2 = 0;
    int enable;

    switch (mode) {
    case SOC_ENCAP_IEEE:
        break;
    case SOC_ENCAP_HIGIG2:
        higig2 = 1;
        /* Fall through */
    case SOC_ENCAP_HIGIG:
        encap = 1;
        break;
    default:
        return SOC_E_PARAM;
    }

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_encap_set: unit %d port %s %s encapsulation\n"),
                 unit, SOC_PORT_NAME(unit, port), gxmac_encap_mode[mode]));
#endif

    SOC_IF_ERROR_RETURN(gxmac_enable_get(unit, port, &enable));

    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(gxmac_enable_set(unit, port, 0));
    }

    if ((mode != SOC_ENCAP_IEEE) && IS_E_PORT(unit, port)) {
        /* XE -> HG */
        SOC_IF_ERROR_RETURN(soc_gxmac_port_mode_update(unit, port, TRUE));
    } else if ((mode == SOC_ENCAP_IEEE) && IS_ST_PORT(unit, port)) {
        /* HG -> XE */
        SOC_IF_ERROR_RETURN(soc_gxmac_port_mode_update(unit, port, FALSE));
    }

    /* Read modify write the encapsulation mode */
    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rx_ctrl));
    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx_ctrl));
    orx = rx_ctrl;
    otx = tx_ctrl;
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, HDRMODEf, encap);
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, HIGIG2MODEf, higig2);
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, HDRMODEf, encap);
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, HIGIG2MODEf, higig2);
    if (COMPILER_64_NE(tx_ctrl, otx)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, tx_ctrl));
    }
    if (COMPILER_64_NE(rx_ctrl, orx)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, rx_ctrl));
    }

    if (enable) {
    /* Re-enable transmitter and receiver */
    SOC_IF_ERROR_RETURN(gxmac_enable_set(unit, port, 1));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_encap_get
 * Purpose:
 *      Get the GXMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
gxmac_encap_get(int unit, soc_port_t port, int *mode)
{
    uint64              rx_ctrl;

    /*
     * TX/RX mode should always be programmed to the same
     * value (or all sorts of weird things will happen).
     * So, for purpose of S/W state, we only need to read
     * one or the other and return the state.
     *
     * WARNING: The following assumes BCM_PORT_ENCAP_xxx values equal
     * the hardware register field values.
     */

    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rx_ctrl));
    *mode = soc_reg64_field32_get(unit, MAC_RXCTRLr, rx_ctrl, HDRMODEf);

    if (*mode == SOC_ENCAP_HIGIG && 
        soc_reg64_field32_get(unit, MAC_RXCTRLr, rx_ctrl, HIGIG2MODEf)) {
        *mode = SOC_ENCAP_HIGIG2;
    }

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_encap_get: unit %d port %s %s encapsulation\n"),
                 unit, SOC_PORT_NAME(unit, port), gxmac_encap_mode[*mode]));
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      gxmac_ability_local_get
 * Purpose:
 *      Return the abilities of GXMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC  int
gxmac_ability_local_get(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    if (NULL == ability) {
       return (SOC_E_PARAM);
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_ABILITY_NONE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_GMII | 
                         SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_ABILITY_NONE;
    ability->loopback  = SOC_PA_LB_MAC;
    ability->flags     = SOC_PA_ABILITY_NONE;
    ability->encap = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG |
        SOC_PA_ENCAP_HIGIG2;

    if (IS_HG_PORT(unit, port)) {
        switch (SOC_INFO(unit).port_speed_max[port]) {
        case 13000:
            ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
            /* fall through */
        case 12000:
            ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
            /* fall through */
        default:
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB; 
            break;
        } 

    } else if (IS_XE_PORT(unit, port)) {
        ability->speed_full_duplex = SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB |
                            SOC_PA_SPEED_10GB;
    } else {
        ability->speed_half_duplex = SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB;
        ability->speed_full_duplex = SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB | 
                            SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB;
    }

    return (SOC_E_NONE);
}

/*
* Function:
*      gxmac_frame_spacing_stretch_set
* Purpose:
*      Set frame spacing stretch parameters.
* Parameters:
*      unit - XGS unit #.
*      port - XGS port # on unit.
* Returns:
*      SOC_E_XXX
*/

STATIC int
gxmac_frame_spacing_stretch_set(int unit, soc_port_t port, int value)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
 if (SOC_IS_XGS3_SWITCH(unit)) {
     int    field_width, max_value;
     uint64 reg_val;
     field_width = soc_reg_field_length(unit, MAC_TXCTRLr, THROTDENOMf);
     max_value = (1 << field_width) - 1;
     if ((value > max_value) || (value < 0)) {
         return SOC_E_PARAM;
     }

     SOC_IF_ERROR_RETURN
         (READ_MAC_TXCTRLr(unit, port, &reg_val));
     soc_reg64_field32_set(unit, MAC_TXCTRLr, &reg_val,
                           THROTDENOMf, value);

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
     if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
         soc_reg64_field32_set(unit, MAC_TXCTRLr, &reg_val,
                           THROTNUMERf, value ? 1 : 0);
     } else 
#endif
	 {
         soc_reg64_field32_set(unit, MAC_TXCTRLr, &reg_val,
                           THROTNUMf, value ? 1 : 0);
     }
     SOC_IF_ERROR_RETURN
         (WRITE_MAC_TXCTRLr(unit, port, reg_val));

     return SOC_E_NONE;
 }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

 return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      gxmac_control_set
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
gxmac_control_set(int unit, soc_port_t port,
                  soc_mac_control_t type, int value)
{
    int rv;
    uint32 reg32, copy32;
    uint64 reg64, copy64;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                "gxmac_control_set: unit %d port %s: %d = %d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    if ((type < 0) || (type >= SOC_MAC_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &reg32));
        copy32 = reg32;
        soc_reg_field_set(unit, GMACC1r, &reg32, RXEN0f, value ? 1 : 0);
        if (reg32 != copy32) {
            SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, reg32));
        }
        SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &reg32));
        copy32 = reg32;
        soc_reg_field_set(unit, FE_MAC1r, &reg32, RX_ENf, value ? 1 : 0);
        if (reg32 != copy32) {
            SOC_IF_ERROR_RETURN(WRITE_FE_MAC1r(unit, port, reg32));
        }
        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &reg64));
        copy64 = reg64;
        soc_reg64_field32_set(unit, MAC_CTRLr, &reg64, RXENf, value ? 1 : 0);
        if (COMPILER_64_NE(reg64, copy64)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, reg64));
        }
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        rv = gxmac_frame_spacing_stretch_set(unit, port, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      gxmac_control_get
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
gxmac_control_get(int unit, soc_port_t port,
                  soc_mac_control_t type, int *value)
{
    uint32 val;
    int rv;
    uint32 reg32;
    uint64 reg64;

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    if ((type < 0) || (type >= SOC_MAC_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &reg32));
        val = soc_reg_field_get(unit, GMACC1r, reg32, RXEN0f);
        SOC_IF_ERROR_RETURN(READ_FE_MAC1r(unit, port, &reg32));
        val |= soc_reg_field_get(unit, FE_MAC1r, reg32, RX_ENf);
        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &reg64));
        val |= soc_reg64_field32_get(unit, MAC_CTRLr, reg64, RXENf);
        *value = val ? 1 : 0;
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if (SOC_REG_IS_VALID(unit, MAC_TXCTRLr)) {
            SOC_IF_ERROR_RETURN
                (READ_MAC_TXCTRLr(unit, port, &reg64));
            *value = soc_reg64_field32_get(unit, MAC_TXCTRLr, reg64,
                                           THROTDENOMf);
            rv = SOC_E_NONE;
        }
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

 LOG_VERBOSE(BSL_LS_SOC_10G,
             (BSL_META_U(unit,
             "gxmac_control_get: unit %d port %s: %d = %d: rv = %d\n"),
              unit, SOC_PORT_NAME(unit, port),
              type, *value, rv));

 return rv;
}
/* Exported XGS MAC driver structure */

mac_driver_t soc_mac_gx = {
    "12G/10G/2.5G/1G MAC Driver",  /* drv_name */
    gxmac_init,                    /* md_init  */
    gxmac_enable_set,              /* md_enable_set */
    gxmac_enable_get,              /* md_enable_get */
    gxmac_duplex_set,              /* md_duplex_set */
    gxmac_duplex_get,              /* md_duplex_get */
    gxmac_speed_set,               /* md_speed_set */
    gxmac_speed_get,               /* md_speed_get */
    gxmac_pause_set,               /* md_pause_set */
    gxmac_pause_get,               /* md_pause_get */
    gxmac_pause_addr_set,          /* md_pause_addr_set */
    gxmac_pause_addr_get,          /* md_pause_addr_get */
    gxmac_loopback_set,            /* md_lb_set */
    gxmac_loopback_get,            /* md_lb_get */
    gxmac_interface_set,           /* md_interface_set */
    gxmac_interface_get,           /* md_interface_get */
    NULL,                          /* md_ability_get - Deprecated */
    gxmac_frame_max_set,           /* md_frame_max_set */
    gxmac_frame_max_get,           /* md_frame_max_get */
    gxmac_ifg_set,                 /* md_ifg_set */
    gxmac_ifg_get,                 /* md_ifg_get */
    gxmac_encap_set,               /* md_encap_set */
    gxmac_encap_get,               /* md_encap_get */
    gxmac_control_set,             /* md_control_set */
    gxmac_control_get,             /* md_control_get */
    gxmac_ability_local_get,       /* md_ability_local_get */
    NULL                           /* md_timesync_tx_info_get */
};
#endif /* BCM_GXPORT_SUPPORT */
