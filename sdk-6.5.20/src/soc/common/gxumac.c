/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS Media Access Controller Driver for port with multiple MACs such as:
 *     Port with BigMAC and UniMAC or
 *     Port with XMAC and UniMAC
 *
 * The general idea is to have this "pseudo" driver act like a wrapper to
 * 10G MAC (BigMAC or XMAC) driver and GE MAC (UniMAC) driver. Depends on the
 * nature of individual driver funciton, some functions will call corresponding
 * function in both MAC drivers, some just call the the function in the active
 * MAC driver.
 *
 * Use _mac_combo_mode_get to determine which driver is currently active.
 * Should not make decision upon the link speed from XGXS, it is only accurate
 * when link is up, however some driver function may be called regardless of
 * the link status.
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#ifdef BCM_TRIUMPH_SUPPORT
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_HURRICANE_SUPPORT
#include <soc/hurricane.h>
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_BIGMAC_SUPPORT) || defined(BCM_XMAC_SUPPORT) || defined(BCM_CMAC_SUPPORT) ||\
    defined(BCM_UNIMAC_SUPPORT)

STATIC mac_driver_t *
_mac_combo_ge_mac(int unit)
{
#ifdef BCM_UNIPORT_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANAX(unit)) {
        return &soc_mac_x;
    } else 
#endif
    {
        return &soc_mac_uni;
    }
}

STATIC mac_driver_t *
_mac_combo_10g_mac(int unit)
{
    mac_driver_t *mac = NULL;

#ifdef BCM_XMAC_SUPPORT
    if (soc_feature(unit, soc_feature_xmac)) {
        return &soc_mac_x;
    }
#endif /* BCM_XMAC_SUPPORT */
#ifdef BCM_BIGMAC_SUPPORT
    mac = &soc_mac_big;
#endif /* BCM_BIGMAC_SUPPORT */

    return mac;
}

#ifdef BCM_CMAC_SUPPORT
STATIC mac_driver_t *
_mac_combo_100g_mac(int unit)
{
    mac_driver_t *mac = NULL;
    if (soc_feature(unit, soc_feature_cmac)) {
        return &soc_mac_c;
    }
    return mac;
}
#endif /* BCM_CMAC_SUPPORT */

/*
 * Function:
 *     _xgmac_select_mode
 * Purpose:
 *     Select current MAC
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     mode - new MAC mode (SOC_MAC_MODE_XXX).
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_mac_combo_select_mac(int unit, soc_port_t port, soc_mac_mode_t mode)
{
    if (LOG_CHECK(BSL_LS_SOC_10G | BSL_VERBOSE) ||
        LOG_CHECK(BSL_LS_SOC_GE | BSL_VERBOSE)) {
        LOG_CLI((BSL_META_U(unit,
                            "_mac_combo_select_mac: unit %d port %s mode %s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mode == SOC_MAC_MODE_10000 ? "10G" : "GE"));
    }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if (!SOC_IS_TRIUMPH3(unit) && IS_XL_PORT(unit, port)) {
        soc_info_t *si;
        int phy_port, bindex;
        soc_field_t field[] = { PORT0_MAC_MODEf, PORT1_MAC_MODEf,
                                PORT2_MAC_MODEf, PORT3_MAC_MODEf };

        si = &SOC_INFO(unit);
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = si->port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        if (phy_port == -1) {
            return SOC_E_INTERNAL;
        }
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLPORT_MODE_REGr, port,
                                    field[bindex],
                                    mode == SOC_MAC_MODE_10000 ? 0 : 1));
        return SOC_E_NONE;
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && IS_CL_PORT(unit, port)) {
        soc_info_t *si;
        int phy_port, bindex;
        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "cl port: %d, phy_port: %d, bindex %d\n"), 
                     port, phy_port, bindex));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MODE_REGr, port,
                                    MAC_MODEf,
                                    mode == SOC_MAC_MODE_100000 ? 1 : 0));
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_KATANA_SUPPORT
    if (IS_MXQ_PORT(unit, port)) {
        int phy_port;
        /* MXQPORT2 = 27,32,33,34 and MXQPORT3 = 28,29,30,31 */
        if ((port >= 29) && (port <= 31)) {
            phy_port = 28;
        } else if ((port >= 32) && (port <= 34)) {
            phy_port = 27;
        } else {
            phy_port = port;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XPORT_MODE_REGr, phy_port,
                                CORE_PORT_MODEf,
                                mode == SOC_MAC_MODE_10000 ? 0 : 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XPORT_MODE_REGr, phy_port,
                                PHY_PORT_MODEf,
                                mode == SOC_MAC_MODE_10000 ? 0 : 2));
        return SOC_E_NONE;
    }
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (IS_XG_PORT(unit, port)) {
        return soc_triumph_xgport_mode_change(unit, port, mode);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XPORT_CONFIGr, port, XPORT_ENf,
                                mode == SOC_MAC_MODE_10000 ? 1 : 0));

#ifdef BCM_TRIUMPH2_SUPPORT
    if (IS_XQ_PORT(unit, port)) {
#ifdef BCM_ENDURO_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, XQPORT_MODE_REGr, port,
                                    XQPORT_MODE_BITSf,
                                    mode == SOC_MAC_MODE_10000 ? 2 : 1));
            return SOC_E_NONE;
        } else 
#endif
#ifdef BCM_HURRICANE1_SUPPORT
        if (SOC_IS_HURRICANE(unit)) {
            return soc_hu_xqport_mode_change(unit, port, mode);
        } else 
#endif
        {
            return soc_tr2_xqport_mode_change(unit, port, mode);
        }
    } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XPORT_MODE_REGr, port,
                                    XPORT_MODE_BITSf,
                                    mode == SOC_MAC_MODE_10000 ? 1 : 2));
    }

    return SOC_E_NONE;
}

STATIC int
_mac_combo_mode_get(int unit, soc_port_t port, soc_mac_mode_t *mode)
{
    uint32 rval;
    int bits;

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if (!SOC_IS_TRIUMPH3(unit) && IS_XL_PORT(unit, port)) {
        soc_info_t *si;
        int phy_port, bindex;
        soc_field_t field[] = { PORT0_MAC_MODEf, PORT1_MAC_MODEf,
                                PORT2_MAC_MODEf, PORT3_MAC_MODEf };

        si = &SOC_INFO(unit);
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = si->port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        if (phy_port == -1) {
            return SOC_E_INTERNAL;
        }
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XLPORT_MODE_REGr, rval, field[bindex]);
        *mode = bits == 0 ? SOC_MAC_MODE_10000 : SOC_MAC_MODE_1000_T;
        return SOC_E_NONE;
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && IS_XL_PORT(unit, port)) {
        *mode = SOC_MAC_MODE_10000;
        return SOC_E_NONE;
    } else if (SOC_IS_TRIUMPH3(unit) && IS_XT_PORT(unit, port)) {
        *mode = SOC_MAC_MODE_10000;
        return SOC_E_NONE;
    } else if (SOC_IS_TRIUMPH3(unit) && IS_CL_PORT(unit, port)) {
        soc_info_t *si;
        int phy_port, bindex;

        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "cl port: %d, phy_port: %d, bindex %d\n"), 
                     port, phy_port, bindex));
        SOC_IF_ERROR_RETURN(READ_PORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, PORT_MODE_REGr, rval, MAC_MODEf);
        *mode = bits == 0 ? SOC_MAC_MODE_10000 : SOC_MAC_MODE_100000;
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_KATANA_SUPPORT
    if (IS_MXQ_PORT(unit, port)) {
        /* MXQPORT2 = 27,32,33,34 and MXQPORT3 = 28,29,30,31 */
        if ((port >= 29) && (port <= 31)) {
            SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, 28, &rval));
        } else if ((port >= 32) && (port <= 34)) {
            SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, 27, &rval));
        } else {
            SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        }
        bits = soc_reg_field_get(unit, XPORT_MODE_REGr, rval,
                                 CORE_PORT_MODEf);
        if (bits == 0) { /* 10G+ Mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 2) { /* 1G/2.5G Mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled or other modes */
            return SOC_E_FAIL;
        }
        return SOC_E_NONE;
    }
#endif /* BCM_KATANA_SUPPORT */

    if (IS_XG_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_XGPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XGPORT_MODE_REGr, rval,
                                 XGPORT_MODE_BITSf);
        if (bits == 2) { /* xport-mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 1) { /* gport-mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled or other modes */
            return SOC_E_FAIL;
        }
    } else if (IS_XQ_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XQPORT_MODE_REGr, rval,
                                 XQPORT_MODE_BITSf);
        if (bits == 2) { /* xport-mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 1) { /* gport-mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled or other modes */
            return SOC_E_FAIL;
        }
    } else {
        SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XPORT_MODE_REGr, rval,
                                 XPORT_MODE_BITSf);
        if (bits == 1) { /* xport-mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 2) { /* gport-mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled */
            return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_init
 * Purpose:
 *      Initialize 10G MAC and GE MAC into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_combo_init(int unit, soc_port_t port)
{
    soc_mac_mode_t mode;

    /*
     * Initialize GE MAC
     */
    if (!SOC_IS_TRIUMPH3(unit) && !SOC_IS_KATANAX(unit)) {
        SOC_IF_ERROR_RETURN(_mac_combo_ge_mac(unit)->md_init(unit, port));
    } 

    /*
     * Initialize 10G MAC
     */
    SOC_IF_ERROR_RETURN(_mac_combo_10g_mac(unit)->md_init(unit, port));

#ifdef BCM_CMAC_SUPPORT
    /*
     * Initialize 100G MAC if applicable
     */
    if (IS_CE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(_mac_combo_100g_mac(unit)->md_init(unit, port));
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANAX(unit)) {
        return SOC_E_NONE;
    }
#endif
    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_100000) {
        SOC_IF_ERROR_RETURN(
            _mac_combo_select_mac(unit, port, SOC_MAC_MODE_100000));
    } else if (mode == SOC_MAC_MODE_10000) {
        SOC_IF_ERROR_RETURN(
            _mac_combo_select_mac(unit, port, SOC_MAC_MODE_10000));
    } else {
        SOC_IF_ERROR_RETURN(
            _mac_combo_select_mac(unit, port, SOC_MAC_MODE_1000_T));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_enable_set
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
mac_combo_enable_set(int unit, soc_port_t port, int enable)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_enable_set(unit, port, enable);
    } else 
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_enable_set(unit, port, enable);
    } else {
        return _mac_combo_ge_mac(unit)->md_enable_set(unit, port, enable);
    }
}

/*
 * Function:
 *      mac_combo_enable_get
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
mac_combo_enable_get(int unit, soc_port_t port, int *enable)
{
    int enable_ge = 0, enable_10g = 0;
#if defined(BCM_CMAC_SUPPORT) && defined(BCM_UNIPORT_SUPPORT)
    int enable_100g = 0;
#endif

    /* Instead of checking the current mode we read from all MACs */

#ifdef BCM_UNIPORT_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        SOC_IF_ERROR_RETURN
        (_mac_combo_10g_mac(unit)->md_enable_get(unit, port, &enable_10g));
#ifdef BCM_CMAC_SUPPORT
        if (IS_CL_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
            (_mac_combo_100g_mac(unit)->md_enable_get(unit, port, 
                                                      &enable_100g));
        }
        *enable = enable_10g | enable_100g;
#else
        *enable = enable_10g;
#endif
        return SOC_E_NONE;
    }
#endif
    /* GE MAC */
    SOC_IF_ERROR_RETURN
        (_mac_combo_ge_mac(unit)->md_enable_get(unit, port, &enable_ge));
    /* 10G MAC */
    SOC_IF_ERROR_RETURN
        (_mac_combo_10g_mac(unit)->md_enable_get(unit, port, &enable_10g));

    *enable = enable_ge | enable_10g;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_duplex_set
 * Purpose:
 *      Set  mac_combo in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_combo_duplex_set(int unit, soc_port_t port, int duplex)
{
    if (IS_GE_PORT(unit, port)) {
        return _mac_combo_ge_mac(unit)->md_duplex_set(unit, port, duplex);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_duplex_get
 * Purpose:
 *      Get mac_combo duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_duplex_get(int unit, soc_port_t port, int *duplex)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
    if ((mode == SOC_MAC_MODE_10000) || (mode == SOC_MAC_MODE_100000)) {
        *duplex = TRUE;
        return SOC_E_NONE;
    } else {
        return _mac_combo_ge_mac(unit)->md_duplex_get(unit, port, duplex);
    }
}

/*
 * Function:
 *      mac_combo_speed_set
 * Purpose:
 *      Set mac_combo speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 1000, 2500, ... 16000.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_combo_speed_set(int unit, soc_port_t port, int speed)
{
    int enable;
    soc_mac_mode_t mode, new_mode;
    int rv = SOC_E_NONE;

    if (IS_HG_PORT(unit, port)) {
        if (speed && speed < 10000) {
            return SOC_E_PARAM;
        }
        if (SOC_INFO(unit).port_speed_max[port] > 0 &&
            speed > SOC_INFO(unit).port_speed_max[port]) {
                /* make exception for 11G & 21G & 42G speeds*/
                if (!((soc_feature(unit, soc_feature_higig_misc_speed_support))
                    && (((SOC_INFO(unit).port_speed_max[port] >= 40000) && speed == 42000)
                    || ((SOC_INFO(unit).port_speed_max[port] == 20000) && speed == 21000)
                    || ((SOC_INFO(unit).port_speed_max[port] == 10000) && speed == 11000)))) {
                    return SOC_E_PARAM;
                }
        }
    } else {
        if (speed && speed < 0) {
            return SOC_E_PARAM;
        }
    }
    if (speed == 0) {
        return SOC_E_NONE; /* Transient state */
    }

    SOC_IF_ERROR_RETURN(mac_combo_enable_get(unit, port, &enable));
    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
    new_mode = speed < 10000 ? SOC_MAC_MODE_1000_T : 
                               speed < 100000 ? SOC_MAC_MODE_10000 : SOC_MAC_MODE_100000;

    if (mode != new_mode) {
      rv = _mac_combo_select_mac(unit, port, new_mode);
    }

    /* Disable MACs before doing anything else */
    if (SOC_SUCCESS(rv)) {
        rv = mac_combo_enable_set(unit, port, 0);
    }

    if (SOC_SUCCESS(rv)) {
        if (speed <= 2500) {
            rv = _mac_combo_ge_mac(unit)->md_speed_set(unit, port, speed);
        } else if (speed <= 42000) {
            rv = _mac_combo_10g_mac(unit)->md_speed_set(unit, port, speed);
        } 
#ifdef BCM_CMAC_SUPPORT
          else {
            rv = _mac_combo_100g_mac(unit)->md_speed_set(unit, port, speed);
        }
#endif
    }

    if (SOC_SUCCESS(rv)) {
        if (mode != new_mode) {
            rv = _mac_combo_select_mac(unit, port, new_mode);
        }
    }

    /* Re-enable MAC */
    if (SOC_SUCCESS(rv)) {
        rv = mac_combo_enable_set(unit, port, enable);
    }

    return rv;
}

/*
 * Function:
 *      mac_combo_speed_get
 * Purpose:
 *      Get mac_combo speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mbps
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_speed_get(unit, port, speed);
    } else 
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_speed_get(unit, port, speed);
    } else {
        return _mac_combo_ge_mac(unit)->md_speed_get(unit, port, speed);
    }
}

/*
 * Function:
 *      mac_combo_pause_set
 * Purpose:
 *      Configure mac_combo to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    if (!IS_HG_PORT(unit, port)) { /* Avoid stacking pause conflict */
        /* GE MAC */
        SOC_IF_ERROR_RETURN
            (_mac_combo_ge_mac(unit)->md_pause_set(unit, port, pause_tx,
                                                   pause_rx));
    }

    /* 10G MAC */
    SOC_IF_ERROR_RETURN
        (_mac_combo_10g_mac(unit)->md_pause_set(unit, port, pause_tx,
                                                pause_rx));

#ifdef BCM_CMAC_SUPPORT
    if (IS_CL_PORT(unit, port)) {
        /* 100G MAC */
        SOC_IF_ERROR_RETURN
        (_mac_combo_100g_mac(unit)->md_pause_set(unit, port, pause_tx,
                                                 pause_rx));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_pause_get
 * Purpose:
 *      Return the pause ability.
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
mac_combo_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_pause_get(unit, port, pause_tx,
                                                       pause_rx);
    } else 
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_pause_get(unit, port, pause_tx,
                                                      pause_rx);
    } else {
        return _mac_combo_ge_mac(unit)->md_pause_get(unit, port, pause_tx,
                                                     pause_rx);
    }
}

/*
 * Function:
 *      mac_combo_pause_addr_set
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
mac_combo_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    /* GE MAC */
    if (!SOC_IS_TRIUMPH3(unit)) {
        SOC_IF_ERROR_RETURN
        (_mac_combo_ge_mac(unit)->md_pause_addr_set(unit, port, mac));
    }
    /* 10G MAC */
    SOC_IF_ERROR_RETURN
        (_mac_combo_10g_mac(unit)->md_pause_addr_set(unit, port, mac));

#ifdef BCM_CMAC_SUPPORT
    if (IS_CL_PORT(unit, port)) {
        /* 100G MAC */
        SOC_IF_ERROR_RETURN
        (_mac_combo_100g_mac(unit)->md_pause_addr_set(unit, port, mac));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_pause_addr_get
 * Purpose:
 *      Retrieve PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_pause_addr_get(unit, port, mac);
    } else
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_pause_addr_get(unit, port, mac);
    } else {
        return _mac_combo_ge_mac(unit)->md_pause_addr_get(unit, port, mac);
    }
}

/*
 * Function:
 *      mac_combo_loopback_set
 * Purpose:
 *      Set MAC loopback mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_loopback_set(int unit, soc_port_t port, int lb)
{
    if (!SOC_IS_TRIUMPH3(unit)) {
        /* GE MAC */
        SOC_IF_ERROR_RETURN(_mac_combo_ge_mac(unit)->md_lb_set(unit, port, lb));
    }

    /* 10G MAC */
    SOC_IF_ERROR_RETURN(_mac_combo_10g_mac(unit)->md_lb_set(unit, port, lb));

#ifdef BCM_CMAC_SUPPORT
    if (IS_CL_PORT(unit, port)) {
        /* 100G MAC */
        SOC_IF_ERROR_RETURN(_mac_combo_100g_mac(unit)->md_lb_set(unit, port, lb));
    }
#endif

#if 0 
    /* Power lanes up/down? */
    soc_unicore16g_lane_power_set(unit, port, lb ? 0 : 0xf);
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_loopback_get
 * Purpose:
 *      Get current loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_loopback_get(int unit, soc_port_t port, int *lb)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_lb_get(unit, port, lb);
    } else 
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_lb_get(unit, port, lb);
    } else {
        return _mac_combo_ge_mac(unit)->md_lb_get(unit, port, lb);
    }
}

/*
 * Function:
 *      mac_combo_interface_set
 * Purpose:
 *      Set MAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */
STATIC int
mac_combo_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_interface_get
 * Purpose:
 *      Retrieve MAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_combo_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_MII;
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_frame_max_set
 * Description:
 *      Set the maximum allowed receive frame size.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_combo_frame_max_set(int unit, soc_port_t port, int size)
{
    /* GE MAC */
    if (!SOC_IS_TRIUMPH3(unit)) {
        SOC_IF_ERROR_RETURN
        (_mac_combo_ge_mac(unit)->md_frame_max_set(unit, port, size));
    }

    /* 10G MAC */
    SOC_IF_ERROR_RETURN
        (_mac_combo_10g_mac(unit)->md_frame_max_set(unit, port, size));

#ifdef BCM_CMAC_SUPPORT
    if (IS_CL_PORT(unit, port)) {
        /* 100G MAC */
        SOC_IF_ERROR_RETURN
        (_mac_combo_100g_mac(unit)->md_frame_max_set(unit, port, size));
    }
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_combo_frame_max_get
 * Description:
 *      Get the maximum allowed receive frame size.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_combo_frame_max_get(int unit, soc_port_t port, int *size)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_frame_max_get(unit, port, size);
    } else 
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_frame_max_get(unit, port, size);
    } else {
        return _mac_combo_ge_mac(unit)->md_frame_max_get(unit, port, size);
    }
}

/*
 * Function:
 *      mac_combo_ifg_set
 * Description:
 *      Set ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - the IFG value
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_combo_ifg_set(int unit, soc_port_t port, int speed,
                  soc_port_duplex_t duplex, int ifg)
{
    if (speed < 10000) {
        /* GE MAC */
        return _mac_combo_ge_mac(unit)->md_ifg_set(unit, port, speed, duplex,
                                                   ifg);
    } else if (speed < 100000) {
        /* 10G MAC */
        return _mac_combo_10g_mac(unit)->md_ifg_set(unit, port, speed, duplex,
                                                    ifg);
    } 
#ifdef BCM_CMAC_SUPPORT
      else {
        /* 100G MAC */
        return _mac_combo_100g_mac(unit)->md_ifg_set(unit, port, speed, duplex,
                                                     ifg);
    }
#endif
    return SOC_E_PARAM;
}

/*
 * Function:
 *      mac_combo_ifg_get
 * Description:
 *      Get ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - the IFG value
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_combo_ifg_get(int unit, soc_port_t port, int speed,
                  soc_port_duplex_t duplex, int *ifg)
{
    if (speed < 10000) {
        /* GE MAC */
        return _mac_combo_ge_mac(unit)->md_ifg_get(unit, port, speed, duplex,
                                                   ifg);
    } else if (speed < 100000) {
        /* 10G MAC */
        return _mac_combo_10g_mac(unit)->md_ifg_get(unit, port, speed, duplex,
                                                    ifg);
    }
#ifdef BCM_CMAC_SUPPORT
      else {
        /* 100G MAC */
        return _mac_combo_100g_mac(unit)->md_ifg_get(unit, port, speed, duplex,
                                                     ifg);
    }
#endif
    return SOC_E_PARAM;
}

/*
 * Function:
 *      mac_combo_encap_set
 * Purpose:
 *      Set MAC encapsulation type.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - (IN) encap type.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_encap_set(int unit, soc_port_t port, int type)
{
    int rv = SOC_E_NONE;
    rv = _mac_combo_10g_mac(unit)->md_encap_set(unit, port, type);
    if (rv != SOC_E_NONE) {
        return rv;
    }
#ifdef BCM_CMAC_SUPPORT
    if (IS_CL_PORT(unit, port)) {
        return _mac_combo_100g_mac(unit)->md_encap_set(unit, port, type);
    }
#endif
    return rv;   
}

/*
 * Function:
 *      mac_combo_encap_get
 * Purpose:
 *      Get MAC encapsulation type.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - encap type.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_combo_encap_get(int unit, soc_port_t port, int *type)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_encap_get(unit, port, type);
    } else 
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_encap_get(unit, port, type);
    } else {
        return _mac_combo_ge_mac(unit)->md_encap_get(unit, port, type);
    }
}

/*
 * Function:
 *      mac_combo_control_set
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
mac_combo_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                      int value)
{
    int rv = SOC_E_NONE;
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
    case SOC_MAC_CONTROL_TX_SET:
    case SOC_MAC_CONTROL_SW_RESET:
    case SOC_MAC_CONTROL_DISABLE_PHY:
    case SOC_MAC_CONTROL_EGRESS_DRAIN:
#ifdef BCM_CMAC_SUPPORT
        if (mode == SOC_MAC_MODE_100000) {
            rv = _mac_combo_100g_mac(unit)->md_control_set(unit, port, type,
                                                             value);
        } else
#endif 
        if (mode == SOC_MAC_MODE_10000) {
            rv = _mac_combo_10g_mac(unit)->md_control_set(unit, port, type,
                                                            value);
        } else {
            rv = _mac_combo_ge_mac(unit)->md_control_set(unit, port, type,
                                                           value);
        }
        break;
    default:
        if (!SOC_IS_TRIUMPH3(unit)) {
            rv = _mac_combo_ge_mac(unit)->md_control_set(unit, port, type, 
                                                           value);
        }
        if (SOC_SUCCESS(rv) || rv == SOC_E_UNAVAIL) {
            rv = _mac_combo_10g_mac(unit)->md_control_set(unit, port, type, 
                                                            value);
        }
#ifdef BCM_CMAC_SUPPORT
        if (IS_CL_PORT(unit, port) &&
            (SOC_SUCCESS(rv) || rv == SOC_E_UNAVAIL)) {
            rv = _mac_combo_100g_mac(unit)->md_control_set(unit, port, type, 
                                                              value);
        }
#endif 
    }

    return rv;
}

/*
 * Function:
 *      mac_combo_control_get
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
mac_combo_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                      int *value)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_mac_combo_mode_get(unit, port, &mode));
#ifdef BCM_CMAC_SUPPORT
    if (mode == SOC_MAC_MODE_100000) {
        return _mac_combo_100g_mac(unit)->md_control_get(unit, port, type,
                                                        value);
    } else
#endif
    if (mode == SOC_MAC_MODE_10000) {
        return _mac_combo_10g_mac(unit)->md_control_get(unit, port, type,
                                                        value);
    } else {
        return _mac_combo_ge_mac(unit)->md_control_get(unit, port, type,
                                                       value);
    }
}

/*
 * Function:
 *      mac_combo_ability_local_get
 * Purpose:
 *      Get MAC abilities.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC  int
mac_combo_ability_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    soc_port_ability_t mac_ability_ge, mac_ability_10g; 
#if defined(BCM_UNIPORT_SUPPORT) || defined(BCM_CMAC_SUPPORT)
    soc_port_ability_t mac_ability_100g = { 0 };
#endif

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(&mac_ability_ge, 0, sizeof(mac_ability_ge));
    sal_memset(&mac_ability_10g, 0, sizeof(mac_ability_10g));

    if ((!IS_HG_PORT(unit, port)) && (!SOC_IS_TRIUMPH3(unit))) {
        /* GE MAC */
        SOC_IF_ERROR_RETURN
            (_mac_combo_ge_mac(unit)->md_ability_local_get(unit, port,
                                                           &mac_ability_ge));
    }

    /*  10G MAC */
    SOC_IF_ERROR_RETURN
        (_mac_combo_10g_mac(unit)->md_ability_local_get(unit, port,
                                                        &mac_ability_10g));

#ifdef BCM_CMAC_SUPPORT
    if (IS_CL_PORT(unit, port)) {
        /*  10G MAC */
        SOC_IF_ERROR_RETURN
        (_mac_combo_100g_mac(unit)->md_ability_local_get(unit, port,
                                                         &mac_ability_100g));
    }
#endif
#if defined(BCM_UNIPORT_SUPPORT) || defined(BCM_CMAC_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        ability->speed_half_duplex =
            mac_ability_10g.speed_half_duplex | mac_ability_100g.speed_half_duplex;
        ability->speed_full_duplex =
            mac_ability_10g.speed_full_duplex | mac_ability_100g.speed_full_duplex;
        ability->pause = mac_ability_10g.pause | mac_ability_100g.pause;
        ability->interface = mac_ability_10g.interface | mac_ability_100g.interface;
        ability->medium = mac_ability_10g.medium | mac_ability_100g.medium;
        ability->loopback = mac_ability_10g.loopback | mac_ability_100g.loopback;
        ability->flags = mac_ability_10g.flags | mac_ability_100g.flags;

        return SOC_E_NONE;
    }    
#endif
    ability->speed_half_duplex =
        mac_ability_ge.speed_half_duplex | mac_ability_10g.speed_half_duplex;
    ability->speed_full_duplex =
        mac_ability_ge.speed_full_duplex | mac_ability_10g.speed_full_duplex;
    ability->pause = mac_ability_ge.pause | mac_ability_10g.pause;
    ability->interface = mac_ability_ge.interface | mac_ability_10g.interface;
    ability->medium = mac_ability_ge.medium | mac_ability_10g.medium;
    ability->loopback = mac_ability_ge.loopback | mac_ability_10g.loopback;
    ability->flags = mac_ability_ge.flags | mac_ability_10g.flags;
    ability->encap = mac_ability_ge.encap | mac_ability_10g.encap;

    return SOC_E_NONE;
}

/* Exported combo MAC driver structure */
mac_driver_t soc_mac_combo = {
    "Combo MAC Driver",            /* drv_name */
    mac_combo_init,                /* md_init  */
    mac_combo_enable_set,          /* md_enable_set */
    mac_combo_enable_get,          /* md_enable_get */
    mac_combo_duplex_set,          /* md_duplex_set */
    mac_combo_duplex_get,          /* md_duplex_get */
    mac_combo_speed_set,           /* md_speed_set */
    mac_combo_speed_get,           /* md_speed_get */
    mac_combo_pause_set,           /* md_pause_set */
    mac_combo_pause_get,           /* md_pause_get */
    mac_combo_pause_addr_set,      /* md_pause_addr_set */
    mac_combo_pause_addr_get,      /* md_pause_addr_get */
    mac_combo_loopback_set,        /* md_lb_set */
    mac_combo_loopback_get,        /* md_lb_get */
    mac_combo_interface_set,       /* md_interface_set */
    mac_combo_interface_get,       /* md_interface_get */
    NULL,                          /* md_ability_get - Deprecated */
    mac_combo_frame_max_set,       /* md_frame_max_set */
    mac_combo_frame_max_get,       /* md_frame_max_get */
    mac_combo_ifg_set,             /* md_ifg_set */
    mac_combo_ifg_get,             /* md_ifg_get */
    mac_combo_encap_set,           /* md_encap_set */
    mac_combo_encap_get,           /* md_encap_get */
    mac_combo_control_set,         /* md_control_set */
    mac_combo_control_get,         /* md_control_get */
    mac_combo_ability_local_get,   /* md_ability_local_get */
    NULL                           /* md_timesync_tx_info_get */
};

#endif /* BCM_BIGMAC_SUPPORT || BCM_XMAC_SUPPORT || BCM_UNIMAC_SUPPORT || BCM_CMAC_SUPPORT */
