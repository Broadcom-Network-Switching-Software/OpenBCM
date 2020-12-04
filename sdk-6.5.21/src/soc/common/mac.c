/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * StrataSwitch Media Access Controller API
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/portmode.h>
#include <soc/ll.h>
#include <soc/phyctrl.h>
#include <soc/macutil.h>

#include <soc/mem.h>

#if defined(BCM_ESW_SUPPORT)

#define JUMBO_MAXSZ  0x3fe8

/*
 * Forward Declarations
 *
 *      mac_fe_xxx      Routines that work on FE ports and
 *                      GE ports that support 10/100 mode.
 *      mac_ge_xxx      Routines that work on GE ports and
 *                      GE ports that support 10/100 mode.
 */

STATIC int      mac_fe_init(int unit, soc_port_t port);
STATIC int      mac_fe_enable_set(int unit, soc_port_t port, int enable);
STATIC int      mac_fe_enable_get(int unit, soc_port_t port, int *enable);

STATIC int      mac_fe_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int      mac_fe_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int      mac_fe_speed_set(int unit, soc_port_t port, int speed);
STATIC int      mac_fe_speed_get(int unit, soc_port_t port, int *speed);

STATIC int      mac_fe_pause_set(int unit, soc_port_t port,
                                 int pause_tx, int pause_rx);
STATIC int      mac_fe_pause_get(int unit, soc_port_t port,
                                 int *pause_tx, int *pause_rx);

STATIC int      mac_ge_init(int unit, soc_port_t port);
STATIC int      mac_ge_enable_set(int unit, soc_port_t port, int enable);
STATIC int      mac_ge_enable_get(int unit, soc_port_t port, int *enable);

STATIC int      mac_ge_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int      mac_ge_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int      mac_ge_speed_set(int unit, soc_port_t port, int speed);
STATIC int      mac_ge_speed_get(int unit, soc_port_t port, int *speed);
STATIC int      mac_ge_frame_max_set(int unit, soc_port_t port, int size);

STATIC int      mac_ge_pause_set(int unit, soc_port_t port,
                                 int pause_tx, int pause_rx);
STATIC int      mac_ge_pause_get(int unit, soc_port_t port,
                                 int *pause_tx, int *pause_rx);
STATIC int      mac_ge_pause_addr_set(int unit, soc_port_t port,
                                      sal_mac_addr_t);
STATIC int      mac_ge_pause_addr_get(int unit, soc_port_t port,
                                      sal_mac_addr_t);
STATIC int      mac_ge_loopback_set(int unit, soc_port_t port, int);
STATIC int      mac_ge_loopback_get(int unit, soc_port_t port, int *);
STATIC int      mac_ge_interface_set(int unit, soc_port_t port,
                                     soc_port_if_t pif);

/*
 * Helper macros for PAUSE_CONTROL setting.  The PAUSE_CONTROL value
 * determines how often XOFFs must be sent.  We use 75% of the maximum
 * value to give some time for the pause to be transmitted and parsed
 * on the far side.  These are all usually counted in units of 512 bit
 * times independent of port speed, so PAUSE_VAL(512) can be used to
 * set the PAUSE_CONTROL register at initialization time.
 */
#define	PAUSE_MAX	0x10000		/* units of 512 bit times */
#define	PAUSE_VAL(_bt)	(PAUSE_MAX * (512/_bt) * 3 / 4)

/*
 * Function:
 *      soc_mac_probe
 * Purpose:
 *      Probe the specified unit/port for MAC type and return
 *      a mac_driver_t pointer used to drive the MAC.
 * Parameters:
 *      unit - StrataSwitch/XGS Unit #.
 *      port - StrataSwitch/XGS Port # on unit.
 *      macd_ptr - (OUT) Pointer to MAC driver.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_mac_probe(int unit, soc_port_t port, mac_driver_t **macdp)
{
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        *macdp = &soc_mac_xl;
         return SOC_E_NONE;
    }
#endif

#if defined(BCM_HURRICANE4_SUPPORT)
    /* GPHY */
    if (SOC_IS_HURRICANE4(unit) &&
        IS_EGPHY_PORT(unit, port)) {
        *macdp = &soc_mac_uni;
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_CLMAC_SUPPORT
    if (soc_feature(unit, soc_feature_cport_clmac)) {
        if ((IS_CXX_PORT(unit, port) || IS_C_PORT(unit, port)) &&
            SOC_INFO(unit).port_speed_max[port] >= 100000) {
            *macdp = &soc_mac_cl;
        } else if ((SOC_IS_APACHE(unit) || SOC_IS_MONTEREY(unit)) &&
                   (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port))) {
            *macdp = &soc_mac_cl;
        } else 
        {
            *macdp = &soc_mac_xl;
        }
        return SOC_E_NONE;
    }
#endif /* BCM_CLMAC_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit) && IS_CL_PORT(unit, port)) {
        *macdp = &soc_mac_cl;
        return SOC_E_NONE;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    if (IS_FE_PORT(unit, port)) {
#ifdef BCM_UNIMAC_SUPPORT
        if (soc_feature(unit, soc_feature_unimac)) {
            *macdp = &soc_mac_uni;
        } else 
#endif
        {
            *macdp = &soc_mac_fe;
        }
#if defined(BCM_XMAC_SUPPORT) && defined(BCM_ESW_SUPPORT)
    } else if (IS_MXQ_PORT(unit, port)) {
        *macdp = &soc_mac_x;
#endif
#ifdef BCM_GXPORT_SUPPORT
#if defined(BCM_HUMV_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_GOLDWING_SUPPORT)
    } else if (IS_GX_PORT(unit, port) &&
               (SOC_IS_HUMV(unit) || SOC_IS_BRADLEY(unit) ||
                SOC_IS_GOLDWING(unit))) {
        *macdp = &soc_mac_gx;
#endif /* BCM_HUMV_SUPPORT || BCM_BRADLEY_SUPPORT || BCM_GOLDWING_SUPPORT */
#endif /* BCM_GXPORT_SUPPORT */
    } else if (IS_GE_PORT(unit, port) && !SOC_IS_TD2_TT2(unit)) {
#ifdef BCM_HURRICANE2_SUPPORT
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit) && IS_CL_PORT(unit,port)) {
            *macdp = &soc_mac_cl;
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        if ((SOC_IS_HURRICANE2(unit)|| SOC_IS_GREYHOUND(unit) || 
             SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) && 
            IS_XL_PORT(unit,port)) {
            *macdp = &soc_mac_xl;
        } else
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_UNIMAC_SUPPORT
        if (soc_feature(unit, soc_feature_unimac)) {
            *macdp = &soc_mac_uni;
        } else
#endif
        {
            if (SOC_IS_TRIUMPH3(unit)) { /* For all GE's */
                *macdp = &soc_mac_x;
            } else {
                *macdp = &soc_mac_ge;
            }
        }
    } else if (soc_feature(unit, soc_feature_unified_port)) {
#if defined(BCM_CMAC_SUPPORT)
        if (IS_CL_PORT(unit, port) || IS_C_PORT(unit, port)) {
#if defined(BCM_CLMAC_SUPPORT)
            if (soc_feature(unit, soc_feature_clmac)) {
                *macdp = &soc_mac_cl;
                return SOC_E_NONE;
            } else
#endif /* BCM_CLMAC_SUPPORT */
            if (IS_CE_PORT(unit, port) ||
                SOC_INFO(unit).port_speed_max[port] >= 100000) {
                *macdp = &soc_mac_c;
                return SOC_E_NONE;
            } else {
                *macdp = &soc_mac_x;
                return SOC_E_NONE;
            }
        }
#endif /* BCM_CMAC_SUPPORT */
#if defined(BCM_XLMAC_SUPPORT)
        if (soc_feature(unit, soc_feature_xlmac)) {
            *macdp = &soc_mac_xl;
        }
#endif /* BCM_XLPORT_SUPPORT */
#if defined(BCM_XMAC_SUPPORT)
        if (soc_feature(unit, soc_feature_xmac)) {
            *macdp = &soc_mac_x;
        }
#endif /* BCM_XMAC_SUPPORT */
        return SOC_E_NONE;
#if (defined(BCM_BIGMAC_SUPPORT) || defined(BCM_XMAC_SUPPORT)) && defined(BCM_UNIMAC_SUPPORT)
    } else if (IS_GX_PORT(unit, port) ||
               (IS_XG_PORT(unit, port) &&
                soc_feature(unit, soc_feature_flexible_xgport))) {
#ifdef BCM_ESW_SUPPORT
        if (IS_IL_PORT(unit, port)) {
            *macdp = &soc_mac_il;
        } else
#endif /* BCM_ESW_SUPPORT */
        {
            *macdp = &soc_mac_combo;
        }
#endif /* (BCM_BIGMAC_SUPPORT || BCM_XMAC_SUPPORT) && BCM_UNIMAC_SUPPORT */
    } else if (((IS_XE_PORT(unit, port)) || (IS_HG_PORT(unit, port))) && !SOC_IS_TRIUMPH3(unit)) {
#ifdef BCM_BIGMAC_SUPPORT
        {
            *macdp = &soc_mac_big;
        }
#endif /* BCM_BIGMAC_SUPPORT */
    } else {
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mac_speed_get
 * Purpose:
 *      Get the speed of a given mac
 * Parameters:
 *      unit   - Strata Switch unit #
 *      port   - Strata Switch port #
 *      speed - (OUT) Speed in Mb/sec
 * Notes:
 *      Do not add a soc_mac_speed_set function.  Use the
 *      higher level MAC driver (eg in bcm/port.c) for this.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_mac_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32           fereg;
    soc_mac_mode_t   mode;
    int              turbo = 0;

    if (speed == NULL) {
        return SOC_E_PARAM;
    }

    if (IS_FE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_FE_SUPPr(unit, port, &fereg));
        if (soc_reg_field_get(unit, FE_SUPPr, fereg, SPEEDf)) {
            *speed = 100;
        } else {
            *speed = 10;
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));
        switch (mode) {
        case SOC_MAC_MODE_10:
        case SOC_MAC_MODE_10_100:       /* Get the speed from the mac */
            SOC_IF_ERROR_RETURN(FE_READ(FE_SUPPr, GTH_FE_SUPPr,
                                        unit, port, &fereg));
            if (soc_reg_field_get(unit, FE_SUPPr, fereg, SPEEDf)) {
                *speed = 100;
            } else {
                *speed = 10;
            }
            break;
        case SOC_MAC_MODE_1000_T:
#if defined(BCM_RAPTOR1_SUPPORT)
            if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port))  {
                soc_pbmp_t pbmp_s0;
                uint32     gport_config;

                SOC_IF_ERROR_RETURN(
                    READ_GPORT_CONFIGr(unit, port, &gport_config));
                SOC_PBMP_WORD_SET(pbmp_s0, 0, 0x00000002);
                if (SOC_PBMP_MEMBER(pbmp_s0, port)) {
                    turbo = soc_reg_field_get(unit, GPORT_CONFIGr, 
                                              gport_config, PLL_MODE_DEF_S0f); 
                } else {
                    turbo = soc_reg_field_get(unit, GPORT_CONFIGr, 
                                              gport_config, PLL_MODE_DEF_S1f); 
                }
            }
#endif
            /* coverity[dead_error_line] */
            *speed = turbo ? 2500 : 1000;
            break;
        case SOC_MAC_MODE_10000:
            *speed = 10000;
#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT)
            if (soc_feature(unit, soc_feature_lmd)) {
                if (IS_LMD_ENABLED_PORT(unit, port)) {
                    *speed >>= 2;   /* Div by 4 */
                }
            }
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */
            break;
        default:
            return(SOC_E_INTERNAL);
        }
    }
    return(SOC_E_NONE);
}

/*
 * Function:
 *      _port_sp_sel_get/set
 * Purpose:
 *      Get/set speed select for a port based on chip type and port
 * Parameters:
 *      unit, port
 * Returns:
 *      Returns < 0 on error.  Otherwise, speed select value for port.
 * Notes:
 *      These should only be called for gig ports supporting fe mode.
 */

STATIC int
_port_sp_sel_get(int unit, int port)
{
#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_trimac)) {
        /* FB/ER puts speed into GE_PORT_CONFIGr */
        uint32 val;

        SOC_IF_ERROR_RETURN(READ_GE_PORT_CONFIGr(unit, port, &val));
        return soc_reg_field_get(unit, GE_PORT_CONFIGr, val, SPEED_SELECTf);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return SOC_E_INTERNAL;
}

STATIC int
_port_sp_sel_set(int unit, int port, uint32 sp_sel)
{
#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_trimac)) {
        /* FB/ER puts speed into GE_PORT_CONFIGr */
        uint32 val;

        SOC_IF_ERROR_RETURN(READ_GE_PORT_CONFIGr(unit, port, &val));
        soc_reg_field_set(unit, GE_PORT_CONFIGr, &val, SPEED_SELECTf, sp_sel);
        SOC_IF_ERROR_RETURN(WRITE_GE_PORT_CONFIGr(unit, port, val));
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return sp_sel;
}


/*
 * Function:
 *      soc_mac_mode_get
 * Purpose:
 *      Certain gig ports have two MACs.  This function indicates which
 *      mode a gig port is in.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      mode - (OUT) current operating mode.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      This routine is used outside of this module.
 */

int
soc_mac_mode_get(int unit, soc_port_t port, soc_mac_mode_t *mode)
{
    int speed_sel;
    uint32 rval;
    int bits;

    if(IS_GE_PORT(unit, port) && soc_feature(unit, soc_feature_unimac)) {
        *mode = SOC_MAC_MODE_1000_T; /* Default */
    } else if (IS_GE_PORT(unit, port) ||
        (IS_FE_PORT(unit, port) && soc_feature(unit, soc_feature_trimac))) {
        *mode = SOC_MAC_MODE_1000_T; /* Default */

        if (soc_feature(unit, soc_feature_fe_gig_macs)) {
            if ((speed_sel = _port_sp_sel_get(unit, port)) < 0) {
                return SOC_E_INTERNAL;
            }
            if (soc_feature(unit, soc_feature_trimac)) {
                if (speed_sel == 1) {
                    *mode = SOC_MAC_MODE_10_100;
                } else if (speed_sel == 2) {
                    *mode = SOC_MAC_MODE_10;
                }
            } else if (speed_sel != 0) {
                *mode = SOC_MAC_MODE_10_100;
            }
        }
    } else if (IS_FE_PORT(unit, port)) {
        *mode = SOC_MAC_MODE_10_100;
    } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        if (IS_XQ_PORT(unit, port)) {
            if (SOC_REG_IS_VALID(unit, XQPORT_MODE_REGr)) {
                SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
                bits = soc_reg_field_get(unit, XQPORT_MODE_REGr, rval,
                                 XQPORT_MODE_BITSf);
                if (bits == 2) { /* xport-mode */
                    *mode = SOC_MAC_MODE_10000;
                } else if (bits == 1) { /* gport-mode */
                    *mode = SOC_MAC_MODE_1000_T;
                    return SOC_E_NONE;
                }
            }
        }
        *mode = SOC_MAC_MODE_10000;
    } else {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_drain_cells
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

int
soc_drain_cells(int unit, soc_port_t port)
{
    soc_timeout_t       to;
    int                 rv, timeout_usec, loopback = -1;
    uint32              cur_cells, new_cells;
    soc_mac_mode_t      mode;
    uint32              egr_meter_config;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32  pkt_drop_ctrl, pkt_drop_set;
    int                 pause_tx = 0, pause_rx = 0;
#endif

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    timeout_usec = soc_property_get(unit, spn_LCCDRAIN_TIMEOUT_USEC, 250000);
    soc_timeout_init(&to, timeout_usec, 0); 

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        SOC_IF_ERROR_RETURN
            (mac_ge_pause_get(unit, port, &pause_tx, &pause_rx));
        SOC_IF_ERROR_RETURN
            (mac_ge_pause_set(unit, port, 0, 0));
        SOC_IF_ERROR_RETURN
            (READ_GE_EGR_PKT_DROP_CTLr(unit, port, &pkt_drop_ctrl));
        pkt_drop_set = 0;
        soc_reg_field_set(unit, GE_EGR_PKT_DROP_CTLr, &pkt_drop_set, FLUSHf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_GE_EGR_PKT_DROP_CTLr(unit, port, pkt_drop_set));
        if (IS_GE_PORT(unit, port) && !IS_GMII_PORT(unit, port) &&
            soc_feature(unit, soc_feature_dodeca_serdes) &&
            !PHY_SGMII_AUTONEG_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, 
                                    phyEventStop, PHY_STOP_DRAIN));
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* Disable egress metering. */
    if (SOC_REG_IS_VALID(unit, EGRMETERINGCONFIGr)) {
        SOC_EGRESS_METERING_LOCK(unit);
        rv = READ_EGRMETERINGCONFIGr(unit, port, &egr_meter_config);
        if (rv < 0) {
            SOC_EGRESS_METERING_UNLOCK(unit);
            return (rv);
        }
        rv = WRITE_EGRMETERINGCONFIGr(unit, port, 0);
        if (rv < 0) {
            SOC_EGRESS_METERING_UNLOCK(unit);
            return (rv);
        }
    }

    if (soc_mac_mode_get(unit, port, &mode) < 0) {
        mode = SOC_MAC_MODE_10_100;
    }

    cur_cells = 0xffffffff;

    for (;;) {
        if ((rv = soc_egress_cell_count(unit, port, &new_cells)) < 0) {
            break;
        }

        if (new_cells == 0) {
            rv = SOC_E_NONE;
            break;
        }

        /* gig mac in pause state needs loopback mode to clear xoff */
        if (new_cells == cur_cells && loopback < 0 &&
            mode == SOC_MAC_MODE_1000_T) {
            if (mac_ge_loopback_get(unit, port, &loopback) < 0) {
                loopback = -1;
            } else {
                mac_ge_loopback_set(unit, port, 1);
            }
        }

        if (new_cells < cur_cells) {                    /* Progress made */
            soc_timeout_init(&to, timeout_usec, 0);     /* Restart timeout */
            cur_cells = new_cells;
        }

        if (soc_timeout_check(&to)) {
            if ((rv = soc_egress_cell_count(unit, port, &new_cells)) < 0) {
                break;
            }

            LOG_ERROR(BSL_LS_SOC_GE,
                      (BSL_META_U(unit,
                                  "MacDrainTimeOut:port %d,%s, "
                                  "timeout draining packets (%d cells remain)\n"),
                       unit, SOC_PORT_NAME(unit, port), new_cells));
            rv = SOC_E_INTERNAL;
            break;
        }
    }

    /* Reenable egres metering. */
    if (SOC_REG_IS_VALID(unit, EGRMETERINGCONFIGr)) {
        rv = WRITE_EGRMETERINGCONFIGr(unit, port, egr_meter_config);
        SOC_EGRESS_METERING_UNLOCK(unit);
        if (rv < 0) {
            return (rv);
        }
    }

    if (loopback >= 0) {
        SOC_IF_ERROR_RETURN
            (mac_ge_loopback_set(unit, port, loopback));
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (IS_GE_PORT(unit, port) && !IS_GMII_PORT(unit, port) &&
            soc_feature(unit, soc_feature_dodeca_serdes) &&
            !PHY_SGMII_AUTONEG_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, 
                                    phyEventResume, PHY_STOP_DRAIN));
        }
        SOC_IF_ERROR_RETURN
            (WRITE_GE_EGR_PKT_DROP_CTLr(unit, port, pkt_drop_ctrl));
        SOC_IF_ERROR_RETURN
            (mac_ge_pause_set(unit, port, pause_tx, pause_rx));
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}

/*
 * Function:
 *      soc_mac_mode_set
 * Purpose:
 *      Force GE MAC into 10/100 or 1000 mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      mode - one of SOC_MAC_MODE_*
 * Returns:
 *      SOC_E_CONFIG - Speed not available on port
 *      SOC_E_XXX
 * Notes:
 *      Can be used on any port; verifies parameters.
 */

int
soc_mac_mode_set(int unit, soc_port_t port, soc_mac_mode_t new_mode)
{
    soc_mac_mode_t      old_mode;
    int                 old_mac_enable;
    int                 save_duplex;
    sal_mac_addr_t      save_pause_mac;         /* Pause MAC address */
    int                 save_pause_tx;          /* Pause Tx enable status */
    int                 save_pause_rx;          /* Pause Rx enable status */
    int                 save_lb;                /* Mac LB enable status */

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "soc_mac_mode_set: reconfiguring port=%d mode=%d\n"),
              port, new_mode));

    switch (new_mode) {                 /* Check mode for validity */
    case SOC_MAC_MODE_10:
        if (!soc_feature(unit, soc_feature_trimac)) {
            return SOC_E_CONFIG;
        }
        break;
    case SOC_MAC_MODE_10_100:
        if (IS_FE_PORT(unit, port) && !soc_feature(unit, soc_feature_trimac)) {
            return SOC_E_NONE;
        } else if (IS_GE_PORT(unit, port)) {
            if (!soc_feature(unit, soc_feature_fe_gig_macs)) {
                return SOC_E_CONFIG;
            }
        }
        break;
    case SOC_MAC_MODE_1000_T:
        if (!IS_GE_PORT(unit, port)) {
            return SOC_E_CONFIG;
        }
        break;
    case SOC_MAC_MODE_10000:
        if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
            return SOC_E_CONFIG;
        } else {
            return SOC_E_NONE;
        }
        break;
    default:
        return SOC_E_INTERNAL;
    }

    /*
     * At this point, we know the request is valid, and that we *MAY* need
     * to take action. Cases such as setting mac on 10/100 port only OR
     * 10G on HG port are already handled.
     */

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &old_mode));

    if (new_mode == old_mode) {         /* Already in requested mode */
        return SOC_E_NONE;
    }

    /*
     * Save some state from the current MAC that must be restored in the
     * new MAC after the mode is switched.
     */

    SOC_IF_ERROR_RETURN
        (mac_ge_duplex_get(unit, port, &save_duplex));

    SOC_IF_ERROR_RETURN
        (mac_ge_pause_get(unit, port, &save_pause_tx, &save_pause_rx));

    SOC_IF_ERROR_RETURN
        (mac_ge_pause_addr_get(unit, port, save_pause_mac));

    SOC_IF_ERROR_RETURN
        (mac_ge_loopback_get(unit, port, &save_lb));

    /*
     * The currently selected MAC is quiesced before switching it out.
     * It must not be talking to the MMU when the switch occurs.  For
     * receive, RX is disabled.  For transmit, Egress is disabled.
     */

    SOC_IF_ERROR_RETURN
        (mac_ge_enable_get(unit, port, &old_mac_enable));
    SOC_IF_ERROR_RETURN
        (mac_ge_enable_set(unit, port, FALSE));

    switch (new_mode) {
    case SOC_MAC_MODE_10:
    case SOC_MAC_MODE_10_100:
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "soc_mac_mode_set: port=%d, 10/100 mode\n"),
                  port));
        if (new_mode == SOC_MAC_MODE_10) {
            _port_sp_sel_set(unit, port, 2);
        } else {
            _port_sp_sel_set(unit, port, 1);
        }
        break;
    case SOC_MAC_MODE_1000_T:
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "soc_mac_mode_set: port=%d, Gig mode\n"),
                  port));
        _port_sp_sel_set(unit, port, 0);
        break;
        /*
         * COVERITY
         *
         * Defensive default
         */
        /* coverity[dead_error_begin] */
    default:
        return SOC_E_INTERNAL;
    }

    /*
     * Restore the saved state in the new MAC.
     */

    SOC_IF_ERROR_RETURN
        (mac_ge_duplex_set(unit, port, save_duplex));

    SOC_IF_ERROR_RETURN
        (mac_ge_pause_set(unit, port, save_pause_tx, save_pause_rx));

    SOC_IF_ERROR_RETURN
        (mac_ge_pause_addr_set(unit, port, save_pause_mac));

    SOC_IF_ERROR_RETURN
        (mac_ge_loopback_set(unit, port, save_lb));

    /*
     * Enable the egress and MAC, if previously enabled.
     */

    if (old_mac_enable) {
        if (new_mode == SOC_MAC_MODE_1000_T) {
            SOC_IF_ERROR_RETURN(mac_ge_enable_set(unit, port, TRUE));
        } else {
            SOC_IF_ERROR_RETURN(mac_fe_enable_set(unit, port, TRUE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The EGR_ENABLE and GEGR_ENABLE registers disable the egress,
 *      chopping any packet that is in progress and leaving part of it
 *      in the MMU.
 *
 *      On FE ports, we can avoid using EGR_ENABLE by putting the egress
 *      single step debug mode.  This will stop traffic to the egress
 *      between packets.
 *
 *      On GE ports, egress single step is not implemented.  Before
 *      setting GEGR_ENABLE to 0, we necessary to clear the port from
 *      EPC LINK to stop traffic from flowing to it, then wait for all
 *      packets to drain from the MMU to the egress.
 *
 *      The draining approach is not used for FE because the draining
 *      takes too long.  It is likely for a link to come back up before
 *      the draining is complete.
 */

STATIC int
mac_fe_enable_set(int unit, soc_port_t port, int enable)
{
    uint32              regval;
    pbmp_t              mask;

#ifdef BCM_RAPTOR1_SUPPORT
    /* Need to reset the FE MAC for Raptor before enabling RX */
    if (SOC_IS_RAPTOR(unit) && enable) {    
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 0));
    }
#endif
    /*
     * Turn on/off receive enable.
     *
     * We do not bother then TXEN because on some chips this signal does
     * not take effect (get latched) until traffic stops flowing.
     */

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &regval));
    FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, RX_ENf, regval, enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC1r, GTH_FE_MAC1r, unit, port, regval));
    
    sal_usleep(50000); /* sleep 50ms */

    /*
     * Use EPC_LINK to control other ports sending to this port.  This
     * blocking is independent and in addition to what Linkscan does.
     *
     * Single-step mode is used to stop traffic from going from the
     * egress to the MAC.
     */

    if (!enable) {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));

        SOC_IF_ERROR_RETURN(soc_drain_cells(unit, port));

#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (SOC_IS_FIREBOLT2(unit)) {
            int fd, speed;

            SOC_IF_ERROR_RETURN(mac_fe_duplex_get(unit, port, &fd));
            SOC_IF_ERROR_RETURN(mac_fe_speed_get(unit, port, &speed));
            
            /* Only for 100 FX via the SerDes */
            if ((speed == 100) && fd && PHY_FIBER_MODE(unit, port) &&
                !PHY_EXTERNAL_MODE(unit, port)) {
                /* Reset the MAC so that the error link state which may
                 * affect receive false carrier counter is cleared when
                 * the link is up again. */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 0));
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
    } else {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }

    /*
     * The SERDES PHY is kept in reset whenever the MAC is
     * disabled.  When the link comes up, the SERDES PHY must come out
     * of reset last, *after* the speed and duplex are programmed.  If
     * the SERDES PHY is reset after the external PHY has link, a false
     * carrier is generated which can cause certain remote hardware to
     * drop link.  SEE ALSO: mac_ge_enable_set.
     */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_dodeca_serdes)) {
        if (enable) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, 
                                    phyEventResume, PHY_STOP_MAC_DIS));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, 
                                    phyEventStop, PHY_STOP_MAC_DIS));
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_RAPTOR_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_fe_enable_get(int unit, soc_port_t port, int *enable)
{
    uint32      regval;

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &regval));

    *enable = FE_GET(unit, port, FE_MAC1r, GTH_FE_MAC1r, RX_ENf, regval);

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_ifg_to_ipg
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
 *      This function support both FE and GTH_FE MACs
 */
STATIC int
mac_fe_ifg_to_ipg(int unit, soc_port_t port, int speed, int duplex,
                  int ifg, int *ipg)
{
    /*
     * The inter-frame gap should be a multiple of 8 bit-times.
     */
     ifg = (ifg + 7) & ~7;

     /*
      * The smallest supported ifg is 64 bit-times
      */
     ifg = (ifg < 64) ? 64 : ifg;

     /*
      * Now we need to convert the value according to various chips'
      * peculiarities
      */
     if (duplex) {
         *ipg = ifg / 4 - 3;
     } else {
        switch (SOC_CHIP_GROUP(unit)) {
        case SOC_CHIP_BCM56504:
        case SOC_CHIP_BCM56102:
        case SOC_CHIP_BCM56304:
        case SOC_CHIP_BCM56800:
        case SOC_CHIP_BCM56218:
        case SOC_CHIP_BCM56112:
        case SOC_CHIP_BCM56314:
        case SOC_CHIP_BCM56514:
        case SOC_CHIP_BCM56624:
        case SOC_CHIP_BCM56820:
        case SOC_CHIP_BCM56680:
        case SOC_CHIP_BCM56634:
        case SOC_CHIP_BCM56524:
        case SOC_CHIP_BCM56685:
        case SOC_CHIP_BCM88732:
            switch (speed) {
            case 10:
                if (soc_feature(unit, soc_feature_ext_gth_hd_ipg)) {
                    *ipg = ifg / 4 - 4;
                } else {
                    *ipg = ifg / 4 - 12;
                }
                break;
            case 100:
                if (soc_feature(unit, soc_feature_ext_gth_hd_ipg)) {
                    *ipg = ifg / 4 - 5;
                } else {
                    *ipg = ifg / 4 - 13;
                }
                break;
            default:
                return SOC_E_INTERNAL;
            }
            break;
        default:
            *ipg = ifg / 4 - 6;
            break;
        }
    }

     /*
      * Notice, that since we force the minimal ifg to be 64, then there
      * no way ipg will be less than 0 or something like that.
      */

     return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_ipg_to_ifg
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
 *      This function works for both FE and GTH_FE MACs
 */
STATIC int
mac_fe_ipg_to_ifg(int unit, soc_port_t port, int speed, int duplex,
                  int ipg, int *ifg)
{
     /*
      * Now we need to convert the value according to various chips'
      * peculiarities
      */
     if (duplex) {
         *ifg = (ipg + 3) * 4;
     } else {
        switch (SOC_CHIP_GROUP(unit)) {
        case SOC_CHIP_BCM56504:
        case SOC_CHIP_BCM56102:
        case SOC_CHIP_BCM56304:
        case SOC_CHIP_BCM56800:
        case SOC_CHIP_BCM56218:
        case SOC_CHIP_BCM56112:
        case SOC_CHIP_BCM56314:
        case SOC_CHIP_BCM56514:
        case SOC_CHIP_BCM56624:
        case SOC_CHIP_BCM56820:
        case SOC_CHIP_BCM56680:
        case SOC_CHIP_BCM56634:
        case SOC_CHIP_BCM56524:
        case SOC_CHIP_BCM56685:
        case SOC_CHIP_BCM88732:
            if (soc_feature(unit, soc_feature_ext_gth_hd_ipg)) {
                if (speed == 10) {
                    *ifg = (ipg + 4) * 4;
                } else {
                    *ifg = (ipg + 5) * 4;
                }
            } else {
                if (speed == 10) {
                    *ifg = (ipg + 12) * 4;
                } else {
                    *ifg = (ipg + 13) * 4;
                }
            }
            break;
        default:
            *ifg = (ipg + 6) * 4;
            break;
        }
    }

     return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_ipg_update
 * Purpose:
 *      Set the IPG appropriate for current speed/duplex
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 * Notes:
 *      The current speed/duplex are read from the hardware registers.
 */

STATIC int
mac_fe_ipg_update(int unit, int port)
{
    int                 fd, speed, ifg, ipg = 0;
    soc_ipg_t           *si = &SOC_PERSIST(unit)->ipg[port];
    uint32              ipg32;

    SOC_IF_ERROR_RETURN(mac_fe_duplex_get(unit, port, &fd));
    SOC_IF_ERROR_RETURN(mac_fe_speed_get(unit, port, &speed));

    /*
     * Get the ifg value in bit-times
     */
    if (fd) {
        switch (speed) {
        case 10:
            ifg = si->fd_10;
            break;
        case 100:
            ifg = si->fd_100;
            break;
        default:
            return SOC_E_INTERNAL;
        }
        if (SOC_IS_XGS3_SWITCH(unit)) {
            SOC_IF_ERROR_RETURN(mac_fe_ifg_to_ipg(unit, port, speed, fd, ifg, &ipg));
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
        /* Deal with XGS 3 switches */
        if (SOC_IS_XGS3_SWITCH(unit)) {
            switch (SOC_CHIP_GROUP(unit)) {
            case SOC_CHIP_BCM56102:
            case SOC_CHIP_BCM56112:
                ipg = 0x11;
                break;
            case SOC_CHIP_BCM56601:
            case SOC_CHIP_BCM56602:
            case SOC_CHIP_BCM56504:
            case SOC_CHIP_BCM56304:
            case SOC_CHIP_BCM56800:
            case SOC_CHIP_BCM56218:
            case SOC_CHIP_BCM56314:
            case SOC_CHIP_BCM56514:
                ipg = 0x12;
                break;
            default:
                return SOC_E_INTERNAL;
            }
        }
    }

    /*
     * Convert the ifg value from bit-times into IPG register-specific value
     * Do only for older switches - XGS3 dealt with separately
     */
    if (!SOC_IS_XGS3_SWITCH(unit)) {
        SOC_IF_ERROR_RETURN(mac_fe_ifg_to_ipg(unit, port, speed, fd, ifg, &ipg));
    }

    /*
     * Program the appropriate register
     */
    ipg32 = ipg;
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_IPGTr, GTH_FE_IPGTr, unit, port, ipg32));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_init
 * Purpose:
 *      Initialize FE MAC into a known good state.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The initialization speed/duplex is arbitrary and must be
 *      updated by linkscan before enabling the MAC.
 */

STATIC int
mac_fe_init(int unit, soc_port_t port)
{
    uint32      reg32;

#if defined(BCM_FELIX_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
        if (IS_FE_PORT(unit, port) && soc_feature(unit, soc_feature_trimac)) {
            SOC_IF_ERROR_RETURN(soc_mac_mode_set(unit, port,
                                                 SOC_MAC_MODE_10_100));
#ifdef BCM_RAPTOR1_SUPPORT
            /*
             * Do a MAC soft reset
             */
            if (SOC_IS_RAPTOR(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 0));
            }
#endif
        }
#endif /* BCM_FELIX_SUPPORT || BCM_RAPTOR_SUPPORT */
    /*
     * Perform length check on Ethernet frames and do not allow huge frames
     */
    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC2r, GTH_FE_MAC2r, unit, port, &reg32));
    /* FULL_DUP=0 for workaround */
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, FULL_DUPf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, EXC_DEFf, reg32, 1);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, LG_CHKf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, HUGE_FRf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, DEL_CRCf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, CRC_ENf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, PAD_ENf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, VLAN_PADf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, AUTO_PADf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, PURE_PADf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, LONG_PREf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, NO_BOFFf, reg32, 0);
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r, BP_NO_BOFFf, reg32, 0);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC2r, GTH_FE_MAC2r, unit, port, reg32));

    SOC_IF_ERROR_RETURN(FE_READ(FE_CLRTr, GTH_FE_CLRTr, unit, port, &reg32));
    FE_SET(unit, port, FE_CLRTr, GTH_FE_CLRTr, COL_WINf, reg32, 0x37);
    FE_SET(unit, port, FE_CLRTr, GTH_FE_CLRTr, RETRYf, reg32, 0xf);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_CLRTr, GTH_FE_CLRTr, unit, port, reg32));

    if (soc_feature(unit, soc_feature_fe_maxframe)) {
        reg32 = 1518 + 1;
        SOC_IF_ERROR_RETURN(WRITE_FE_MAXFr(unit, port,  reg32));
    }

    SOC_IF_ERROR_RETURN(FE_READ(FE_IPGRr, GTH_FE_IPGRr, unit, port, &reg32));
    FE_SET(unit, port, FE_IPGRr, GTH_FE_IPGRr, IPGR2f, reg32, 0x12);
    FE_SET(unit, port, FE_IPGRr, GTH_FE_IPGRr, IPGR1f, reg32, 0xc);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_IPGRr, GTH_FE_IPGRr, unit, port, reg32));

    SOC_IF_ERROR_RETURN(FE_READ(FE_SUPPr, GTH_FE_SUPPr, unit, port, &reg32));
    FE_SET(unit, port, FE_SUPPr, GTH_FE_SUPPr, SPEEDf, reg32, 1);
    FE_SET(unit, port, FE_SUPPr, GTH_FE_SUPPr, BIT_MODEf, reg32, 0);

    SOC_IF_ERROR_RETURN(FE_WRITE(FE_SUPPr, GTH_FE_SUPPr, unit, port, reg32));
    SOC_IF_ERROR_RETURN(FE_READ(FE_TESTr, GTH_FE_TESTr, unit, port, &reg32));
    FE_SET(unit, port, FE_TESTr, GTH_FE_TESTr, SHORT_QNTAf, reg32, 0);
    FE_SET(unit, port, FE_TESTr, GTH_FE_TESTr, TEST_PAUSEf, reg32, 0);
    FE_SET(unit, port, FE_TESTr, GTH_FE_TESTr, TEST_BACKf, reg32, 0);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_TESTr, GTH_FE_TESTr, unit, port, reg32));

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &reg32));
    if (!SOC_IS_XGS_SWITCH(unit)) {
        FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, SOFT_RSTf, reg32, 0);
    }

    /* Enable pause on FE ports */
    FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, TX_PAUf, reg32, 1);
    FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, RX_PAUf, reg32, 1);

    FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, LBACKf, reg32, 0);
    FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, RX_ENf, reg32, 0);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC1r, GTH_FE_MAC1r, unit, port, reg32));

    /* Clear station address */
    reg32 = 0;
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA0r, GTH_ESA0r, unit, port, reg32));
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA1r, GTH_ESA1r, unit, port, reg32));
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA2r, GTH_ESA2r, unit, port, reg32));

    /* Set IPG to match initial operating mode */
    SOC_IF_ERROR_RETURN(mac_fe_ipg_update(unit, port));

    if (!IS_GE_PORT(unit, port)) {
        uint32 port_bit;

        port_bit = (1 << (port % 8));

#if defined(BCM_FELIX_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
        if (IS_FE_PORT(unit, port) && soc_feature(unit, soc_feature_trimac)) {
            SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &reg32));
            soc_reg_field_set(unit, EGR_ENABLEr, &reg32, PRT_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, reg32));
        } else
#endif /* BCM_FELIX_SUPPORT || BCM_RAPTOR_SUPPORT */
        {
            /* Set egress enable */
            SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &reg32));
            reg32 |= port_bit;
            SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, reg32));
        }
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_duplex_set
 * Purpose:
 *      Set FE MAC in the specified duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 *      Also handles GTH ports (gig ports in 10/100 MAC mode).
 */

STATIC int
mac_fe_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint32      fe_mac2;
    int         old_mac_en;

    SOC_IF_ERROR_RETURN(mac_fe_enable_get(unit, port, &old_mac_en));
    if (old_mac_en) {
        SOC_IF_ERROR_RETURN(mac_fe_enable_set(unit, port, FALSE));
    }

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC2r, GTH_FE_MAC2r, unit, port, &fe_mac2));
    FE_SET(unit, port, FE_MAC2r, GTH_FE_MAC2r,
           FULL_DUPf, fe_mac2, duplex ? 1 : 0);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC2r, GTH_FE_MAC2r, unit, port, fe_mac2));

    /* Set IPG to match new duplex */
    SOC_IF_ERROR_RETURN(mac_fe_ipg_update(unit, port));

    if (old_mac_en) {
        SOC_IF_ERROR_RETURN(mac_fe_enable_set(unit, port, TRUE));
    }

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    /*
     * Notify internal PHY driver of duplex change in case it is being
     * used as pass-through to an external PHY.
     */
    if (soc_feature(unit, soc_feature_dodeca_serdes)) {
        if (IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventDuplex, duplex));
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_RAPTOR_SUPPORT */

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_duplex_get
 * Purpose:
 *      Get FE MAC duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_fe_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint32      fe_mac2;

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC2r, GTH_FE_MAC2r, unit, port, &fe_mac2));

    *duplex = FE_GET(unit, port, FE_MAC2r, GTH_FE_MAC2r,
                     FULL_DUPf, fe_mac2) ? 1 : 0;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_speed_set
 * Purpose:
 *      Set FE MAC in the specified speed.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      speed - 10,100 for speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      On chips with a PHYMOD field:
 *              If PHYMOD=0, the speed is hardwired to 100Mb/s.
 *              If PHYMOD=1, the speed is obtained directly from the PHY.
 *      In either case, FE_SUPP.SPEED is completely ignored.
 *      Programs an IFG time appropriate to speed and duplex.
 */

STATIC int
mac_fe_speed_set(int unit, soc_port_t port, int speed)
{
    uint32      fe_supp;

    SOC_IF_ERROR_RETURN(FE_READ(FE_SUPPr, GTH_FE_SUPPr, unit, port, &fe_supp));

    switch (speed) {
    case 0:
        /* Leave speed unchanged; supports NULL PHY */
        break;
    case 10:
        if (IS_FE_PORT(unit, port) && soc_feature(unit, soc_feature_trimac)) {
            SOC_IF_ERROR_RETURN(soc_mac_mode_set(unit, port,
                                                 SOC_MAC_MODE_10));
        }
        FE_SET(unit, port, FE_SUPPr, GTH_FE_SUPPr, SPEEDf, fe_supp, 0);
        break;
    case 100:
        if (IS_FE_PORT(unit, port) && soc_feature(unit, soc_feature_trimac)) {
            SOC_IF_ERROR_RETURN(soc_mac_mode_set(unit, port,
                                                 SOC_MAC_MODE_10_100));
        }
        FE_SET(unit, port, FE_SUPPr, GTH_FE_SUPPr, SPEEDf, fe_supp, 1);
        break;
    default:
        return (SOC_E_CONFIG);
    }

    SOC_IF_ERROR_RETURN(FE_WRITE(FE_SUPPr, GTH_FE_SUPPr, unit, port, fe_supp));

    /* Set IPG to match new speed */
    SOC_IF_ERROR_RETURN(mac_fe_ipg_update(unit, port));

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    /*
     * Notify internal PHY driver of speed change in case it is being
     * used as pass-through to an external PHY.
     */
    if (soc_feature(unit, soc_feature_dodeca_serdes)) {
        if (IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventSpeed, speed));
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_RAPTOR_SUPPORT */

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_speed_get
 * Purpose:
 *      Get FE MAC speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      speed - (OUT) speed in Mb (10/100)
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      On chips with a PHYMOD field:
 *              If PHYMOD=0, the speed is hardwired to 100Mb/s.
 *              If PHYMOD=1, the speed is obtained directly from the PHY.
 *      In either case, FE_SUPP.SPEED is completely ignored.
 */

STATIC int
mac_fe_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32      fe_supp;

    SOC_IF_ERROR_RETURN(FE_READ(FE_SUPPr, GTH_FE_SUPPr, unit, port, &fe_supp));

    if (FE_GET(unit, port, FE_SUPPr, GTH_FE_SUPPr, SPEEDf, fe_supp) == 1) {
        *speed = 100;
    } else {
        *speed = 10;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_pause_set
 * Purpose:
 *      Configure MAC to transmit/receive pause frames.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_tx - Boolean: transmit pause, or -1 (don't change)
 *      pause_rx - Boolean: receive pause, or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_fe_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    uint32      fe_mac1;

    if (pause_tx < 0 && pause_rx < 0) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &fe_mac1));
    if (pause_rx >= 0) {
        FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r,
               RX_PAUf, fe_mac1, pause_rx ? 1 : 0);
    }
    if (pause_tx >= 0) {
        FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r,
               TX_PAUf, fe_mac1, pause_tx ? 1 : 0);
    }
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC1r, GTH_FE_MAC1r, unit, port, fe_mac1));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_pause_addr_set
 * Purpose:
 *      Configure FE MAC PAUSE frame source address.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_fe_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t pause_mac)
{
    uint32      esa0, esa1, esa2;

    esa0 = 0;
    esa1 = 0;
    esa2 = 0;

    FE_SET(unit, port, ESA0r, GTH_ESA0r, STAD0f, esa0,
           (pause_mac[4] << 8) | pause_mac[5]);
    FE_SET(unit, port, ESA1r, GTH_ESA1r, STAD0f, esa1,
           (pause_mac[2] << 8) | pause_mac[3]);
    FE_SET(unit, port, ESA2r, GTH_ESA2r, STAD0f, esa2,
           (pause_mac[0] << 8) | pause_mac[1]);

    SOC_IF_ERROR_RETURN(FE_WRITE(ESA0r, GTH_ESA0r, unit, port, esa0));
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA1r, GTH_ESA1r, unit, port, esa1));
    SOC_IF_ERROR_RETURN(FE_WRITE(ESA2r, GTH_ESA2r, unit, port, esa2));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_pause_get
 * Purpose:
 *      Return the ability of FE MAC
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_tx - Boolean: transmit pause
 *      pause_rx - Boolean: receive pause
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_fe_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint32      fe_mac1;

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &fe_mac1));

    *pause_rx = FE_GET(unit, port, FE_MAC1r, GTH_FE_MAC1r,
                       RX_PAUf, fe_mac1) ? 1 : 0;
    *pause_tx = FE_GET(unit, port, FE_MAC1r, GTH_FE_MAC1r,
                       TX_PAUf, fe_mac1) ? 1 : 0;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_pause_addr_set
 * Purpose:
 *      Configure PAUSE frame source address.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_fe_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t pause_mac)
{
    uint32      esa0, esa1, esa2;
    uint32      b0_15, b16_31, b32_47;

    SOC_IF_ERROR_RETURN(FE_READ(ESA0r, GTH_ESA0r, unit, port, &esa0));
    SOC_IF_ERROR_RETURN(FE_READ(ESA1r, GTH_ESA1r, unit, port, &esa1));
    SOC_IF_ERROR_RETURN(FE_READ(ESA2r, GTH_ESA2r, unit, port, &esa2));

    b0_15  = FE_GET(unit, port, ESA0r, GTH_ESA0r, STAD0f, esa0);
    b16_31 = FE_GET(unit, port, ESA1r, GTH_ESA1r, STAD0f, esa1);
    b32_47 = FE_GET(unit, port, ESA2r, GTH_ESA2r, STAD0f, esa2);

    pause_mac[0] = (uint8)(b32_47 >> 8);
    pause_mac[1] = (uint8)(b32_47 >> 0);
    pause_mac[2] = (uint8)(b16_31 >> 8);
    pause_mac[3] = (uint8)(b16_31 >> 0);
    pause_mac[4] = (uint8)(b0_15  >> 8);
    pause_mac[5] = (uint8)(b0_15  >> 0);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_loopback_set
 * Purpose:
 *      Set a MAC into/out-of loopback mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_fe_loopback_set(int unit, soc_port_t port, int loopback)
{
    uint32      fe_mac1;

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &fe_mac1));
    FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r,
           LBACKf, fe_mac1, loopback ? 1 : 0);
    SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC1r, GTH_FE_MAC1r, unit, port, fe_mac1));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_loopback_get
 * Purpose:
 *      Get current FE MAC loopback mode setting.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_fe_loopback_get(int unit, soc_port_t port, int *loopback)
{
    int         rv;
    uint32      fe_mac1;

    if ((rv = FE_READ(FE_MAC1r, GTH_FE_MAC1r,
                      unit, port, &fe_mac1)) == SOC_E_NONE) {
        *loopback = FE_GET(unit, port, FE_MAC1r, GTH_FE_MAC1r,
                           LBACKf, fe_mac1);
    }

    return(rv);
}

/*
 * Function:
 *      mac_fe_interface_set
 * Purpose:
 *      Set a FE MAC interface type
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - requested mode not supported.
 * Notes:
 *      Only MII supported on FE ports.
 */

STATIC int
mac_fe_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    switch (pif) {
    case SOC_PORT_IF_MII:
        return SOC_E_NONE;
    default:
        return SOC_E_UNAVAIL;
    }
}

/*
 * Function:
 *      mac_fe_interface_get
 * Purpose:
 *      Retrieve FE MAC interface type
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
mac_fe_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_MII;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_fe_ability_get
 * Purpose:
 *      Return the abilities of FE MAC
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_fe_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);

    if (IS_FE_PORT(unit, port)) {
        /* Asymmetric pause not available (SOC_PM_PAUSE_ASYMM) */
        *mode = (SOC_PM_10MB | SOC_PM_100MB | SOC_PM_PAUSE |
                 SOC_PM_MII | SOC_PM_LB_MAC);
        return (SOC_E_NONE);
    } else {
        *mode = 0;
        return (SOC_E_UNAVAIL);
    }
}

/*
 * Function:
 *      mac_fe_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      1) Depending on chip or port type the actual maximum receive frame
 *         size might be slightly higher.
 */

STATIC int
mac_fe_frame_max_set(int unit, soc_port_t port, int size)
{
    uint32 val32;

    if (soc_feature(unit, soc_feature_fe_maxframe)) {
        size += 1; /* XGS3 FE MAC MAX_FR is one greater than MAX len */
    }

    val32 = size;

    return FE_WRITE(FE_MAXFr, GTH_FE_MAXFr, unit, port, val32);
}

/*
 * Function:
 *      mac_fe_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      1) Depending on chip or port type the actual maximum receive frame
 *         size might be slightly higher.
 */
STATIC int
mac_fe_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint32         val32;

    SOC_IF_ERROR_RETURN(FE_READ(FE_MAXFr, GTH_FE_MAXFr, unit, port, &val32));
    *size = val32;

    if (soc_feature(unit, soc_feature_fe_maxframe)) {
        *size -= 1; /* XGS3 FE MAC MAX_FR is one greater than MAX len */
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_ifg_set
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
mac_fe_ifg_set(int unit, soc_port_t port, int speed, soc_port_duplex_t duplex,
               int ifg)
{
    int         cur_speed;
    int         cur_duplex;
    int         ipg;
    int         real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    int         rv;

    SOC_IF_ERROR_RETURN(mac_fe_ifg_to_ipg(unit, port, speed, duplex,
                                          ifg, &ipg));
    SOC_IF_ERROR_RETURN(mac_fe_ipg_to_ifg(unit, port, speed, duplex,
                                          ipg, &real_ifg));

    if (duplex) {
        switch (speed) {
        case 10:
            si->fd_10 = real_ifg;
            break;
        case 100:
            si->fd_100 = real_ifg;
            break;
        default:
            return SOC_E_PARAM;
            break;
        }
    } else {
        switch (speed) {
        case 10:
            si->hd_10 = real_ifg;
    /*    coverity[equality_cond]    */
            break;
        case 100:
            si->hd_100 = real_ifg;
            break;
        default:
            return SOC_E_PARAM;
            break;
        }
    }

    SOC_IF_ERROR_RETURN(mac_fe_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_fe_speed_get(unit, port, &cur_speed));

    if (cur_speed == speed && ((soc_port_duplex_t)cur_duplex == duplex)) {
        switch (speed) {
        case 10:
        case 100:
            rv = mac_fe_ipg_update(unit, port);
            break;
        /* coverity[dead_error_begin] */
        default:
            rv =  SOC_E_PARAM;
        }
    } else {
        rv = SOC_E_NONE;
    }

    return (rv);
}

/*
 * Function:
 *      mac_fe_ifg_get
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
mac_fe_ifg_get(int unit, soc_port_t port, int speed, soc_port_duplex_t  duplex,
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

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_encap_set
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
mac_fe_encap_set(int unit, soc_port_t port, int mode)
{
    if (mode == SOC_ENCAP_IEEE) {
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      mac_fe_encap_get
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
mac_fe_encap_get(int unit, soc_port_t port, int *mode)
{
    *mode = SOC_ENCAP_IEEE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_fe_ability_local_get
 * Purpose:
 *      Return the abilities of FE MAC
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_fe_ability_local_get(int unit, soc_port_t port,
                        soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (IS_FE_PORT(unit, port)) {
        ability->speed_half_duplex  = SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB; 
        ability->speed_full_duplex  = SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB;
        ability->pause     = SOC_PA_PAUSE;
        ability->interface = SOC_PA_INTF_MII;
        ability->medium    = SOC_PA_ABILITY_NONE;
        ability->loopback  = SOC_PA_LB_MAC;
        ability->flags     = SOC_PA_ABILITY_NONE;
        ability->encap     = SOC_PA_ENCAP_IEEE;
        return (SOC_E_NONE);
    } else {
        return (SOC_E_UNAVAIL);
    }
}

/* Exported FE MAC driver structure */
mac_driver_t soc_mac_fe = {
    "10Mb/100Mb MAC Driver",        /* drv_name */
    mac_fe_init,                    /* md_init  */
    mac_fe_enable_set,              /* md_enable_set */
    mac_fe_enable_get,              /* md_enable_get */
    mac_fe_duplex_set,              /* md_duplex_set */
    mac_fe_duplex_get,              /* md_duplex_get */
    mac_fe_speed_set,               /* md_speed_set */
    mac_fe_speed_get,               /* md_speed_get */
    mac_fe_pause_set,               /* md_pause_set */
    mac_fe_pause_get,               /* md_pause_get */
    mac_fe_pause_addr_set,          /* md_pause_addr_set */
    mac_fe_pause_addr_get,          /* md_pause_addr_get */
    mac_fe_loopback_set,            /* md_lb_set */
    mac_fe_loopback_get,            /* md_lb_get */
    mac_fe_interface_set,           /* md_interface_set */
    mac_fe_interface_get,           /* md_interface_get */
    mac_fe_ability_get,             /* md_ability_get */
    mac_fe_frame_max_set,           /* md_frame_max_set */
    mac_fe_frame_max_get,           /* md_frame_max_get */
    mac_fe_ifg_set,                 /* md_ifg_set */
    mac_fe_ifg_get,                 /* md_ifg_get */
    mac_fe_encap_set,               /* md_encap_set */
    mac_fe_encap_get,               /* md_encap_get */
    NULL,                           /* md_control_set */
    NULL,                           /* md_control_get */
    mac_fe_ability_local_get,       /* md_ability_local_get */
    NULL,                           /* md_timesync_tx_info_get */
};

/*
 * Function:
 *      mac_ge_enable_set
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
mac_ge_enable_set(int unit, soc_port_t port, int enable)
{
    uint32              gmacc1, ogmacc1;
    soc_mac_mode_t      mode;
    pbmp_t              mask;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_enable_set(unit, port, enable));
    }

    /*
     * Turn on/off receive enable.
     *
     * We do not bother then TXEN because on some chips this signal does
     * not take effect (get latched) until traffic stops flowing.
     */

    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    ogmacc1 = gmacc1;
    if (enable) {
        soc_reg_field_set(unit, GMACC1r, &gmacc1, TXEN0f, 1);
        soc_reg_field_set(unit, GMACC1r, &gmacc1, RXEN0f, 1);
    } else {            /* Turn off Receive Enable */
        soc_reg_field_set(unit, GMACC1r, &gmacc1, RXEN0f, 0);
    }
    if (gmacc1 != ogmacc1) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
    }

    /*
     * Use EPC_LINK to control other ports sending to this port.  This
     * blocking is independent and in addition to what Linkscan does.
     *
     * Single-step mode is used to stop traffic from going from the
     * egress to the MAC.
     */

    if (!enable) {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));

        SOC_IF_ERROR_RETURN(soc_drain_cells(unit, port));

    } else {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
   }

    /*
     * The SERDES PHY is kept in reset whenever the MAC is
     * disabled.  When the link comes up, the SERDES PHY must come out
     * of reset last, *after* the speed and duplex are programmed.  If
     * the SERDES PHY is reset after the external PHY has link, a false
     * carrier is generated which can cause certain remote hardware to
     * drop link.  SEE ALSO: mac_fe_enable_set.
     */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_dodeca_serdes)) {
        if (IS_GE_PORT(unit, port)) {
            if (enable) {
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_notify(unit, port,
                                        phyEventResume, PHY_STOP_MAC_DIS));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_notify(unit, port,
                                        phyEventStop, PHY_STOP_MAC_DIS));
            }
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_RAPTOR_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_ge_enable_get(int unit, soc_port_t port, int *enable)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_enable_get(unit, port, enable));
    } else {
        uint32          gmacc1;

        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));

        *enable = soc_reg_field_get(unit, GMACC1r, gmacc1, RXEN0f);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_ifg_to_ipg
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
mac_ge_ifg_to_ipg(int unit, soc_port_t port, int speed, int duplex,
                  int ifg, int *ipg)
{
    /*
     * The inter-frame gap should be a multiple of 8 bit-times.
     */
     ifg = (ifg + 7) & ~7;

     /*
      * The smallest supported ifg is 64 bit-times
      */
     ifg = (ifg < 64) ? 64 : ifg;

     /*
      * Now we need to convert the value according to various chips'
      * peculiarities (there are none as of now)
      */
     *ipg = ifg / 8;

     return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_ipg_to_ifg
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
mac_ge_ipg_to_ifg(int unit, soc_port_t port, int speed, int duplex,
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
 *      mac_ge_ipg_update
 * Purpose:
 *      Set the IPG appropriate for current duplex
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 * Notes:
 *      The current duplex is read from the hardware registers.
 */

STATIC int
mac_ge_ipg_update(int unit, int port)
{
    uint32              gmacc2, ogmacc2;
    int                 fd, speed, ipg, ifg;
    soc_ipg_t           *si = &SOC_PERSIST(unit)->ipg[port];

    SOC_IF_ERROR_RETURN(mac_ge_duplex_get(unit, port, &fd));
    SOC_IF_ERROR_RETURN(mac_ge_speed_get(unit, port, &speed));

    if (fd) {
        switch (speed) {
        case 10:
        case 100:
            return (mac_fe_ipg_update(unit, port));
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
        case 100:
            return (mac_fe_ipg_update(unit, port));
            break;
        case 1000:
            ifg = si->hd_1000;
            break;
        case 2500:
            ifg = si->hd_2500;
            break;
        default:
            return SOC_E_INTERNAL;
            break;
        }
    }

    /*
     * Convert the ifg value from bit-times into IPG register-specific value
     */
    SOC_IF_ERROR_RETURN(mac_ge_ifg_to_ipg(unit, port, speed, fd, ifg, &ipg));

    /*
     * Program the appropriate register (if necessary)
     */
    SOC_IF_ERROR_RETURN(READ_GMACC2r(unit, port, &gmacc2));
    ogmacc2 = gmacc2;
    soc_reg_field_set(unit, GMACC2r, &gmacc2, IPGTf, ipg);
    if (gmacc2 != ogmacc2) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC2r(unit, port, gmacc2));
    }

   return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_init
 * Purpose:
 *      Initialize the gigabit MAC into a known good state.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Both the FE and GE MACs are initialized.
 *      The initialization speed/duplex is arbitrary and must be
 *      updated by linkscan before enabling the MAC.
 */

STATIC  int
mac_ge_init(int unit, soc_port_t port)
{
    uint32              gmacc0, gmacc1, gmacc2, val32;
    uint32              ogmacc0, ogmacc1, ogmacc2;
    uint32 pctrl;

    /*
     * Initialize 10/100MB/s MAC if present
     */
    if (soc_feature(unit, soc_feature_fe_gig_macs)) {
        SOC_IF_ERROR_RETURN(soc_mac_mode_set(unit, port,
                                             SOC_MAC_MODE_10_100));
        SOC_IF_ERROR_RETURN(mac_fe_init(unit, port));
    }

    /*
     * Initialize 1000Mb/s MAC
     */
    SOC_IF_ERROR_RETURN(soc_mac_mode_set(unit, port, SOC_MAC_MODE_1000_T));

    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));
    SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
    SOC_IF_ERROR_RETURN(READ_GMACC2r(unit, port, &gmacc2));
    ogmacc0 = gmacc0;
    ogmacc1 = gmacc1;
    ogmacc2 = gmacc2;
    soc_reg_field_set(unit, GMACC0r, &gmacc0, SRSTf, 0);
    soc_reg_field_set(unit, GMACC0r, &gmacc0, L10Bf, 0);
    soc_reg_field_set(unit, GMACC0r, &gmacc0, L32Bf, 0);
    soc_reg_field_set(unit, GMACC0r, &gmacc0, TMDSf, 0);

    soc_reg_field_set(unit, GMACC1r, &gmacc1, FULLDf, 1);  /* arbitrary */
    if (IS_ST_PORT(unit, port)) {
        /* Flow control should not be enabled in stacking mode. */
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCTXf, 0);  /* Flow ctl */
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCRXf, 0);  /* Flow ctl */
    } else {
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCTXf, 1);  /* Flow ctl */
        soc_reg_field_set(unit, GMACC1r, &gmacc1, FCRXf, 1);  /* Flow ctl */
    }
    soc_reg_field_set(unit, GMACC1r, &gmacc1, LONGPf, 1);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, MIFGf, 2);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, GLVRf, 1);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, TXEN0f, 0);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, RXEN0f, 0);
    soc_reg_field_set(unit, GMACC1r, &gmacc1, JUMBOf, 1);  /* HUGEN=0 */

    pctrl = 0;
    soc_reg_field_set(unit, PAUSE_CONTROLr, &pctrl, ENABLEf, 1);
    soc_reg_field_set(unit, PAUSE_CONTROLr, &pctrl, VALUEf,
                          PAUSE_VAL(512));

    SOC_IF_ERROR_RETURN(WRITE_PAUSE_CONTROLr(unit, port, pctrl));

    val32 = 0;
    SOC_IF_ERROR_RETURN(WRITE_TEST2r(unit, port, val32));

    if (gmacc2 != ogmacc2) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC2r(unit, port, gmacc2));
    }
    if (gmacc1 != ogmacc1) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
    }
    if (gmacc0 != ogmacc0) {
        SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0));
    }

#if defined(BCM_RAPTOR_SUPPORT) 
    if (IS_S_PORT(unit, port)) {
        soc_pbmp_t pbmp_s0;
#if defined(BCM_RAPTOR1_SUPPORT)
        int        support_2_5g = 0;
#endif

        /* Select between 2.5 and 1G mode */
        if (!PHY_EXTERNAL_MODE(unit, port)) {
            mac_ge_frame_max_set(unit, port, JUMBO_MAXSZ);
#if defined(BCM_RAPTOR1_SUPPORT)
            support_2_5g = 1;
#endif
        } else {
            mac_ge_frame_max_set(unit, port, 1518);
        }

        SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &val32));
        soc_reg_field_set(unit, GPORT_CONFIGr, &val32, CLR_CNTf, 1);
        soc_reg_field_set(unit, GPORT_CONFIGr, &val32, GPORT_ENf, 1);
        SOC_PBMP_WORD_SET(pbmp_s0, 0, 0x00000002);
        if (SOC_PBMP_MEMBER(pbmp_s0, port)) {
            /* The "SOP check enables" are not gated with "HiGig2 enables"
             * and "bond_disable_stacking". 
             * Thus software need to enable/disable the SOP drop check 
             * in HiGig2 mode as desired.
             */
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              DROP_ON_WRONG_SOP_EN_S0f, 0);
#if defined(BCM_RAPTOR1_SUPPORT)
            /* Select 2.5 Gbps or 1 Gbps mode. */
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              PLL_MODE_DEF_S0f, support_2_5g);
#endif
            /* Enable HiGig 2 */
            /* Assuming that always use stacking port in HiGig mode */
            /* Actually, stacking port can also be used in ethernet mode */
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              HGIG2_EN_S0f, IS_ST_PORT(unit, port) ? 1 : 0);
#if defined(BCM_RAPTOR1_SUPPORT)
            /* Select LCPLL as reference clock */
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              SEL_LCPLL_S0f, 1);
#endif
        } else {
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              DROP_ON_WRONG_SOP_EN_S1f, 0);
#if defined(BCM_RAPTOR1_SUPPORT)
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              PLL_MODE_DEF_S1f, support_2_5g);
#endif
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              HGIG2_EN_S1f, IS_ST_PORT(unit, port) ? 1 : 0);
#if defined(BCM_RAPTOR1_SUPPORT)
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              SEL_LCPLL_S1f, 1);
#endif
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, val32));

        /* Reset the clear-count bit after 64 clocks */
        SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &val32));
        soc_reg_field_set(unit, GPORT_CONFIGr, &val32, CLR_CNTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, val32));
    }
#endif /* BCM_RAPTOR_SUPPORT */

    /* Clear station address */
    val32 = 0;
    SOC_IF_ERROR_RETURN(WRITE_GSA0r(unit, port, val32));
    SOC_IF_ERROR_RETURN(WRITE_GSA1r(unit, port, val32));

    /* Set IPG to match initial operating mode */
    SOC_IF_ERROR_RETURN(mac_ge_ipg_update(unit, port));

    /* Set receive IPGs */
    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56504:
    case SOC_CHIP_BCM56102:
    case SOC_CHIP_BCM56304:
    case SOC_CHIP_BCM56800:
    case SOC_CHIP_BCM56218:
    case SOC_CHIP_BCM56112:
    case SOC_CHIP_BCM56314:
    case SOC_CHIP_BCM56514:
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM88732:
        val32 = 0;
        soc_reg_field_set(unit, FE_IPGRr, &val32, IPGR1f, 0x6);
        soc_reg_field_set(unit, FE_IPGRr, &val32, IPGR2f, 0xf);
        SOC_IF_ERROR_RETURN(WRITE_FE_IPGRr(unit, port, val32));
        break;
    default:
        break;
    }

    /* Set egress enable */
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_FBX(unit) || SOC_IS_RAPTOR(unit)) {
        SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &val32));
        soc_reg_field_set(unit, EGR_ENABLEr, &val32, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, val32));
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    /*
     * Firebolt ports are initialized in bcm_port_init() so as to
     * block control frames (PORT_TABm.PASS_CONTROL_FRAMESf = 0).
     */

#if defined(BCM_RAPTOR1_SUPPORT)
    if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
        /* Must initialize the ComboSerDes after MAC is initialized. */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_init(unit, port));
    }
#endif /* BCM_RAPTOR1_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_interface_set
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
mac_ge_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    uint32              gmacc0, gpcsc;
    uint32              ogmacc0, ogpcsc;

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
 *      mac_ge_interface_get
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
mac_ge_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    uint32      gmacc0;

    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));

    *pif = (soc_reg_field_get(unit, GMACC0r, gmacc0, TMDSf) ?
            SOC_PORT_IF_GMII :
            SOC_PORT_IF_TBI);

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_duplex_set
 * Purpose:
 *      Set GE MAC in the specified duplex mode.
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
mac_ge_duplex_set(int unit, soc_port_t port, int duplex)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_duplex_set(unit, port, duplex));
    } else {
        uint32 gmacc1, ogmacc1;
        if (IS_S_PORT(unit, port)) {
            /* Full-Duplex is tied to 1 on Raptor combo SerDes */
            duplex = 1;
        }
        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
        ogmacc1 = gmacc1;
        soc_reg_field_set(unit, GMACC1r, &gmacc1, 
                              FULLDf, duplex ? 1 : 0);
        if (gmacc1 != ogmacc1) {
            SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));

            /* Set IPG to match new duplex */
            SOC_IF_ERROR_RETURN(mac_ge_ipg_update(unit, port));
        }

    }

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    /*
     * Notify internal PHY driver of duplex change in case it is being
     * used as pass-through to an external PHY.
     */
    if (soc_feature(unit, soc_feature_dodeca_serdes)) {
        if (IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_notify(unit, port, phyEventDuplex, duplex));
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_duplex_get
 * Purpose:
 *      Get GE MAC duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
mac_ge_duplex_get(int unit, soc_port_t port, int *duplex)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return mac_fe_duplex_get(unit, port, duplex);
    } else {
        uint32          gmacc1;

        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));

        *duplex = soc_reg_field_get(unit, GMACC1r, gmacc1, FULLDf);

        return SOC_E_NONE;
    }
}

#if defined(BCM_RAPTOR1_SUPPORT)
STATIC int
mac_se_speed_set(int unit, soc_port_t port, int speed)
{
    int        cur_speed;
    uint32     val32;
    uint32     val64;
    soc_pbmp_t pbmp_s0;

    SOC_IF_ERROR_RETURN(mac_ge_speed_get(unit, port, &cur_speed));
    if (cur_speed == speed) {
        return SOC_E_NONE;
    }

    /* Make sure all packets in flight have reached destination */

    /* Keep Trimac in soft reset */
    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &val64));
    soc_reg_field_set(unit, GMACC0r, &val64, SRSTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, val64));

    /* Change the config bits to desired mode */
    SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &val32));
    SOC_PBMP_WORD_SET(pbmp_s0, 0, 0x00000002);
    if (SOC_PBMP_MEMBER(pbmp_s0, port)) {
        /* Select 2.5 Gbps or 1 Gbps mode. */
        soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              PLL_MODE_DEF_S0f, (speed == 2500) ? 1 : 0);
    } else {
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                              PLL_MODE_DEF_S1f, (speed == 2500) ? 1 : 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, val32));

    /* Bring Trimac out of reset */
    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &val64));
    soc_reg_field_set(unit, GMACC0r, &val64, SRSTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, val64));

    return SOC_E_NONE;
}
#endif /* BCM_RAPTOR1_SUPPORT */

/*
 * Function:
 *      mac_ge_speed_set
 * Purpose:
 *      Set GE MAC in the specified speed.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      speed - 10,100,1000 for speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 */

STATIC int
mac_ge_speed_set(int unit, soc_port_t port, int speed)
{
    int cur_speed;

    if (!IS_GE_PORT(unit, port)) {
        return SOC_E_CONFIG;
    }

    SOC_IF_ERROR_RETURN(mac_ge_speed_get(unit, port, &cur_speed));

#if defined(BCM_RAPTOR1_SUPPORT)
    if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
        if (!PHY_EXTERNAL_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (mac_se_speed_set(unit, port, speed));
        }
    }  
#endif  /* BCM_RAPTOR1_SUPPORT */

    switch (speed) {
    case 0:
        break;                          /* Support NULL PHY */

    case 10:
    case 100:
        if (soc_feature(unit, soc_feature_trimac)) {
            SOC_IF_ERROR_RETURN
                (soc_mac_mode_set(unit, port,
                                  (speed == 10) ?
                                  SOC_MAC_MODE_10:
                                  SOC_MAC_MODE_10_100));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_mac_mode_set(unit, port, SOC_MAC_MODE_10_100));
        }
        SOC_IF_ERROR_RETURN
            (mac_fe_speed_set(unit, port, speed));

        break;

    case 1000:
        SOC_IF_ERROR_RETURN
            (soc_mac_mode_set(unit, port, SOC_MAC_MODE_1000_T));

#if defined(BCM_FIREBOLT_SUPPORT)
        /*
         * Notify internal PHY driver of speed change in case it is being
         * used as pass-through to an external PHY.
         */

        if (soc_feature(unit, soc_feature_dodeca_serdes)) {
            /*
             * Notify internal PHY driver of speed change in case it is being
             * used as pass-through to an external PHY.
             */
            if (IS_GE_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port,
                                                   phyEventSpeed, speed));
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */

        break;

    case 2500:
        SOC_IF_ERROR_RETURN
            (soc_mac_mode_set(unit, port, SOC_MAC_MODE_1000_T));
        break;

    default:
        return (SOC_E_CONFIG);
    }

    /* Set IPG to match new speed */
    SOC_IF_ERROR_RETURN(mac_ge_ipg_update(unit, port));

    /* MAC speed switch results in a tx clock glitch to the serdes in 100fx mode.
     * reset serdes txfifo clears this condition. However this reset triggers
     * a link transition. Do not apply this reset if speed is already in 100M
     */
    if ((speed == 100) && (cur_speed != 100)) {
        (void)soc_phyctrl_notify(unit, port, phyEventTxFifoReset, 100);
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_ge_speed_get
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
mac_ge_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_mac_mode_t      mode;
    int                 turbo = 0;
    int                 rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    switch(mode) {
    case SOC_MAC_MODE_10:
    case SOC_MAC_MODE_10_100:
        rv = mac_fe_speed_get(unit, port, speed);
        break;
    case SOC_MAC_MODE_1000_T:
#if defined(BCM_RAPTOR1_SUPPORT)
        if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
            uint32        gport_config;
            soc_pbmp_t    pbmp_s0;

            SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &gport_config));

            SOC_PBMP_WORD_SET(pbmp_s0, 0, 0x00000002);
            if (SOC_PBMP_MEMBER(pbmp_s0, port)) {
                 turbo = soc_reg_field_get(unit, GPORT_CONFIGr, 
                                           gport_config, PLL_MODE_DEF_S0f);
            } else {
                 turbo = soc_reg_field_get(unit, GPORT_CONFIGr,
                                           gport_config, PLL_MODE_DEF_S1f);
            }
        }
#endif
        /* coverity[dead_error_line] */
        *speed = turbo ? 2500 : 1000;
        break;
    default:
        rv = SOC_E_INTERNAL;
    }
    return(rv);
}

/*
 * Function:
 *      mac_ge_pause_set
 * Purpose:
 *      Configure GE MAC to transmit/receive pause frames.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      pause_tx - Boolean: transmit pause, or -1 (don't change)
 *      pause_rx - Boolean: receive pause, or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */

STATIC  int
mac_ge_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    soc_mac_mode_t      mode;

    if (pause_tx < 0 && pause_rx < 0) {
        return SOC_E_NONE;
    }
#if defined(BCM_RAPTOR1_SUPPORT)
    if (SOC_IS_RAPTOR(unit) && IS_ST_PORT(unit, port)) {
        /* Flow control cannot be enabled in HiGL mode */
        pause_tx = 0;
        pause_rx = 0;
    }
#endif
    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_pause_set(unit, port, pause_tx, pause_rx));
    } else {
        uint32          gmacc1, ogmacc1;

        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));
        ogmacc1 = gmacc1;
        if (pause_tx >= 0) {
            soc_reg_field_set(unit, GMACC1r, &gmacc1, FCTXf,
                                  pause_tx ? 1 : 0);
        }
        if (pause_rx >= 0) {
            soc_reg_field_set(unit, GMACC1r, &gmacc1, FCRXf,
                                  pause_rx ? 1 : 0);
        }
        if (gmacc1 != ogmacc1) {
            SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
        }

        return SOC_E_NONE;
    }
}

/*
 * Function:
 *      mac_ge_pause_addr_set
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
mac_ge_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t pause_mac)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_pause_addr_set(unit, port, pause_mac));
    } else {                    /* mac in gig mode */
        uint32  gsa0, gsa1;

        gsa0 = 0;
        gsa1 = 0;

        soc_reg_field_set(unit, GSA0r, &gsa0, STAD1f,
                          pause_mac[0] << 24 |
                          pause_mac[1] << 16 |
                          pause_mac[2] << 8 |
                          pause_mac[3] << 0);

        soc_reg_field_set(unit, GSA1r, &gsa1, STAD2f,
                          pause_mac[4] << 8 |
                          pause_mac[5]);

        SOC_IF_ERROR_RETURN(WRITE_GSA0r(unit, port, gsa0));
        SOC_IF_ERROR_RETURN(WRITE_GSA1r(unit, port, gsa1));
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_ge_pause_get
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

STATIC  int
mac_ge_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_pause_get(unit, port, pause_tx, pause_rx));
    } else {                    /* mac in gig mode */
        uint32          gmacc1;

        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));

        *pause_tx = soc_reg_field_get(unit, GMACC1r, gmacc1, FCTXf) ?
            TRUE : FALSE;
        *pause_rx = soc_reg_field_get(unit, GMACC1r, gmacc1, FCRXf) ?
            TRUE : FALSE;

        return (SOC_E_NONE);
    }
}

/*
 * Function:
 *      mac_ge_pause_addr_get
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
mac_ge_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t pause_mac)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return (mac_fe_pause_addr_get(unit, port, pause_mac));
    } else {                    /* mac in gig mode */
        uint32 gsa0, gsa1;
        uint32 stad1, stad2;

        SOC_IF_ERROR_RETURN(READ_GSA0r(unit, port, &gsa0));
        SOC_IF_ERROR_RETURN(READ_GSA1r(unit, port, &gsa1));

        stad1 = soc_reg_field_get(unit, GSA0r, gsa0, STAD1f);
        stad2 = soc_reg_field_get(unit, GSA1r, gsa1, STAD2f);

        pause_mac[0] = (uint8)(stad1 >> 24);
        pause_mac[1] = (uint8)(stad1 >> 16);
        pause_mac[2] = (uint8)(stad1 >> 8);
        pause_mac[3] = (uint8)(stad1 >> 0);
        pause_mac[4] = (uint8)(stad2 >> 8);
        pause_mac[5] = (uint8)(stad2 >> 0);

        return (SOC_E_NONE);
    }
}

/*
 * Function:
 *      mac_ge_loopback_set
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
mac_ge_loopback_set(int unit, soc_port_t port, int loopback)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return mac_fe_loopback_set(unit, port, loopback);
    } else {
        uint32 gmacc0, ogmacc0;

        /* L10B also works, but not on Raptor so we'll use L32B */
        SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));
        ogmacc0 = gmacc0;
        soc_reg_field_set(unit, GMACC0r, &gmacc0, L32Bf, loopback ? 1 : 0);
        if (gmacc0 != ogmacc0) {
            SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0));
        }

        return (SOC_E_NONE);
    }
}

/*
 * Function:
 *      mac_ge_loopback_get
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
mac_ge_loopback_get(int unit, soc_port_t port, int *loopback)
{
    soc_mac_mode_t      mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return mac_fe_loopback_get(unit, port, loopback);
    } else {                    /* mac in gig mode */
        uint32 gmacc0;

        SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0));

        *loopback = soc_reg_field_get(unit, GMACC0r, gmacc0, L32Bf);

        return (SOC_E_NONE);
    }
}

/*
 * Function:
 *      mac_ge_ability_get
 * Purpose:
 *      Return the GE MAC abilities
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port # on unit.
 *      mode - (OUT) Mask of MAC abilities returned.
 * Returns:
 *      SOC_E_NONE
 */

STATIC  int
mac_ge_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *mode = (SOC_PM_1000MB_FD | SOC_PM_MII | SOC_PM_GMII | SOC_PM_TBI |
             SOC_PM_LB_MAC | SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM);

    if (soc_feature(unit, soc_feature_fe_gig_macs)) {
        *mode |= SOC_PM_100MB | SOC_PM_10MB;
    }

#if defined(BCM_RAPTOR1_SUPPORT)
    if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
        *mode |= SOC_PM_2500MB_FD;
    }
#endif /* BCM_RAPTOR_SUPPORT */

    return (SOC_E_NONE);
}

/*
 * Function:
 *      mac_ge_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The maximum frame size on GE port is set the same for both GE and
 *      10/100 mode
 */

STATIC int
mac_ge_frame_max_set(int unit, soc_port_t port, int size)
{
    uint32 val32;

    /* 10/100 mode first */
    if (soc_feature(unit, soc_feature_fe_gig_macs)) {
        SOC_IF_ERROR_RETURN(mac_fe_frame_max_set(unit, port, size));
    }

    /* now 1000 mode */

    if (IS_ST_PORT(unit, port)) {
        size += 16; /* Account for 16 bytes of Higig2 header */
    }

    val32 = size;
    return WRITE_MAXFRr(unit, port, val32);
}

/*
 * Function:
 *      mac_ge_frame_max_get
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
 *
 *      For GE ports that use 2 separate MACs (one for GE and another one for
 *      10/100 modes) the function returns the maximum rx frame size set for
 *      the current mode.
 */
STATIC int
mac_ge_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint32         val32;
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));

    if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
        return mac_fe_frame_max_get(unit, port, size);
    } else {
        SOC_IF_ERROR_RETURN(READ_MAXFRr(unit, port, &val32));
        *size = val32;

        if (IS_ST_PORT(unit, port)) {
            *size -= 16; /* Account for 16 bytes of Higig2 header */
        }
        return SOC_E_NONE;
    }
}

/*
 * Function:
 *      mac_ge_ifg_set
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
mac_ge_ifg_set(int unit, soc_port_t port, int speed, soc_port_duplex_t  duplex,
               int ifg)
{
    int         cur_speed;
    int         cur_duplex;
    int         ipg;
    int         real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    int         rv;

    SOC_IF_ERROR_RETURN(mac_ge_ifg_to_ipg(unit, port, speed, duplex,
                                          ifg, &ipg));
    SOC_IF_ERROR_RETURN(mac_ge_ipg_to_ifg(unit, port, speed, duplex,
                                          ipg, &real_ifg));

    if (duplex) {
        switch (speed) {
        case 10:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                si->fd_10 = real_ifg;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 100:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                si->fd_100 = real_ifg;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 1000:
    /** coverity[equality_cond]    **/
            si->fd_1000 = real_ifg;
    /** coverity[equality_cond]    **/
            break;
        case 2500:
#ifdef BCM_RAPTOR1_SUPPORT
            if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
                si->fd_2500 = real_ifg;
            } else
#endif
            {
                return SOC_E_PARAM;
            }
            break;

        default:
            return SOC_E_PARAM;
            break;
        }
    } else {
        switch (speed) {
        case 10:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                si->hd_10 = real_ifg;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 100:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                si->hd_100 = real_ifg;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 1000:
            si->hd_1000 = real_ifg;
            break;

        default:
            return SOC_E_PARAM;
            break;
        }
    }

    SOC_IF_ERROR_RETURN(mac_ge_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_ge_speed_get(unit, port, &cur_speed));

    if (cur_speed == speed && ((soc_port_duplex_t)cur_duplex == duplex)) {
        switch (speed) {
        case 10:
        case 100:
            rv = mac_fe_ipg_update(unit, port);
            break;
        case 1000:
        case 2500:
            rv = mac_ge_ipg_update(unit, port);
            break;
        /* coverity[dead_error_begin] */
        default:
            rv =  SOC_E_PARAM;
        }
    } else {
        rv = SOC_E_NONE;
    }

    return (rv);
}

/*
 * Function:
 *      mac_ge_ifg_get
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
mac_ge_ifg_get(int unit, soc_port_t port, int speed, soc_port_duplex_t  duplex,
               int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];

    if (duplex) {
        switch (speed) {
        case 10:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                *ifg = si->fd_10;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 100:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                *ifg = si->fd_100;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 1000:
            *ifg = si->fd_1000;
            break;
        case 2500:
#ifdef BCM_RAPTOR1_SUPPORT
            if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
                *ifg = si->fd_2500;
            } else
#endif
            {
                return SOC_E_PARAM;
            }
            break;

        default:
            return SOC_E_PARAM;
            break;
        }
    } else {
        switch (speed) {
        case 10:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                *ifg = si->hd_10;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 100:
            if (soc_feature(unit, soc_feature_fe_gig_macs)) {
                *ifg = si->hd_100;
            }  else {
                return SOC_E_PARAM;
            }
            break;
        case 1000:
            *ifg = si->hd_1000;
            break;

        default:
            return SOC_E_PARAM;
            break;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_encap_set
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
mac_ge_encap_set(int unit, soc_port_t port, int mode)
{
    if (mode == SOC_ENCAP_IEEE) {
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      mac_ge_encap_get
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
mac_ge_encap_get(int unit, soc_port_t port, int *mode)
{
    *mode = SOC_ENCAP_IEEE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_control_set
 * Purpose:
 *      To configure GE MAC control properties.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_ge_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                   int value)
{
    soc_mac_mode_t mode;
    uint32 regval, copy;

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_ge_control_set: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));
        if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
#ifdef BCM_RAPTOR1_SUPPORT
            /* Need to reset the FE MAC for Raptor before enabling RX */
            if (SOC_IS_RAPTOR(unit) && value) {    
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, GMACC0r, port, SRSTf, 0));
            }
#endif
            SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &regval));
            FE_SET(unit, port, FE_MAC1r, GTH_FE_MAC1r, RX_ENf, regval, value ? 1 : 0);
            SOC_IF_ERROR_RETURN(FE_WRITE(FE_MAC1r, GTH_FE_MAC1r, unit, port, regval));
            break;
        }
    
        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &regval));
        copy = regval;
        if (value) {
            soc_reg_field_set(unit, GMACC1r, &regval, RXEN0f, 1);
        } else {
            soc_reg_field_set(unit, GMACC1r, &regval, RXEN0f, 0);
        }
        if (regval != copy) {
            SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, regval));
        }
        break;
     
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_ge_control_get
 * Purpose:
 *      To get current GE MAC control setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_ge_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                   int *value)
{
    soc_mac_mode_t mode;
    uint32 regval;

    if (value == NULL) {
        return SOC_E_PARAM;
    }

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(soc_mac_mode_get(unit, port, &mode));
        if ((mode == SOC_MAC_MODE_10_100) || (mode == SOC_MAC_MODE_10)) {
            SOC_IF_ERROR_RETURN(FE_READ(FE_MAC1r, GTH_FE_MAC1r, unit, port, &regval));
            *value = FE_GET(unit, port, FE_MAC1r, GTH_FE_MAC1r, RX_ENf, regval);
            break;
        }
        SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &regval));
        *value = soc_reg_field_get(unit, GMACC1r, regval, RXEN0f);
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_GE,
                (BSL_META_U(unit,
                            "mac_ge_control_get: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, *value));
    return SOC_E_NONE;
}            

/*
 * Function:
 *      mac_ge_ability_local_get
 * Purpose:
 *      Return the GE MAC abilities
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port # on unit.
 *      mode - (OUT) Mask of MAC abilities returned.
 * Returns:
 *      SOC_E_NONE
 */

STATIC  int
mac_ge_ability_local_get(int unit, soc_port_t port,
                        soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB; 
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_TBI; 
    ability->medium    = SOC_PA_ABILITY_NONE;
    ability->loopback  = SOC_PA_LB_MAC;
    ability->flags     = SOC_PA_ABILITY_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE;

    if (soc_feature(unit, soc_feature_fe_gig_macs)) {
        ability->speed_half_duplex |= SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB;
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB; 
    }

#if defined(BCM_RAPTOR1_SUPPORT)
    if (SOC_IS_RAPTOR(unit) && IS_S_PORT(unit, port)) {
        ability->speed_full_duplex |= SOC_PA_SPEED_2500MB; 
    }
#endif /* BCM_RAPTOR_SUPPORT */

    return (SOC_E_NONE);
}

/* Exported GE MAC driver structure */
mac_driver_t soc_mac_ge = {
    "1000Mb MAC Driver",            /* drv_name */
    mac_ge_init,                    /* md_init  */
    mac_ge_enable_set,              /* md_enable_set */
    mac_ge_enable_get,              /* md_enable_get */
    mac_ge_duplex_set,              /* md_duplex_set */
    mac_ge_duplex_get,              /* md_duplex_get */
    mac_ge_speed_set,               /* md_speed_set */
    mac_ge_speed_get,               /* md_speed_get */
    mac_ge_pause_set,               /* md_pause_set */
    mac_ge_pause_get,               /* md_pause_get */
    mac_ge_pause_addr_set,          /* md_pause_addr_set */
    mac_ge_pause_addr_get,          /* md_pause_addr_get */
    mac_ge_loopback_set,            /* md_lb_set */
    mac_ge_loopback_get,            /* md_lb_get */
    mac_ge_interface_set,           /* md_interface_set */
    mac_ge_interface_get,           /* md_interface_get */
    mac_ge_ability_get,             /* md_ability_get */
    mac_ge_frame_max_set,           /* md_frame_max_set */
    mac_ge_frame_max_get,           /* md_frame_max_get */
    mac_ge_ifg_set,                 /* md_ifg_set */
    mac_ge_ifg_get,                 /* md_ifg_get */
    mac_ge_encap_set,               /* md_encap_set */
    mac_ge_encap_get,               /* md_encap_get */
    mac_ge_control_set,             /* md_control_set */
    mac_ge_control_get,             /* md_control_get */
    mac_ge_ability_local_get,       /* md_ability_local_get */
    NULL                            /* md_timesync_tx_info_get */
};

#endif /* defined(BCM_ESW_SUPPORT) */

