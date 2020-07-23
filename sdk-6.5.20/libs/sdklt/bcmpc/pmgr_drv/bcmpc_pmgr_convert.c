/*! \file bcmpc_pmgr_convert.c
 *
 * BCMPC Type Conversion
 *
 * Convert BCMPC data type to/from PMAC and PHYMOD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_types_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR


/*! Highest NRZ per lane speed is 28.125G. */
#define NRZ_LANE_SPEED_MAX 28125

/*! Maximum number of RX DFE taps that SerDes supports. */
#define PHY_NUM_RX_DFE_TAPS_MAX 14

/*! Encap mode map. */
typedef struct pmgr_encap_map_s {
    bcmpc_port_encap_t pmgr_mode;
    bcmpmac_encap_mode_t pmac_mode;
} pmgr_encap_map_t;

/*! Loopback mode map. */
typedef struct pmgr_lpbk_map_s {
    bcmpc_port_lpbk_t pmgr_mode;
    phymod_loopback_mode_t phy_mode;
} pmgr_lpbk_map_t;

/*! FEC mode map. */
typedef struct pmgr_fec_map_s {
    bcmpc_fec_t pmgr_mode;
    phymod_fec_type_t phy_mode;
} pmgr_fec_map_t;

/*! Tap mode map. */
typedef struct pmgr_tap_mode_map_s {
    bcmpc_phy_txfir_tap_mode_t pmgr_mode;
    phymod_tx_tap_mode_t phy_mode;
} pmgr_tap_mode_map_t;

/*! Signalling mode map. */
typedef struct pmgr_sig_mode_map_s {
    bcmpc_phy_sig_mode_t pmgr_mode;
    phymod_phy_signalling_method_t phy_mode;
} pmgr_sig_mode_map_t;

/*! Media type map. */
typedef struct pmgr_media_type_map_s {
    bcmpc_phy_medium_t pmgr_media_type;
    phymod_firmware_media_type_t phy_media_type;
} pmgr_media_type_map_t;

/*! PAM4 tx pattern map. */
typedef struct pmgr_pam4_tx_pattern_type_map_s {
    bcmpc_pam4_tx_pattern_t pmgr_pam4_tx_pattern_type;
    phymod_PAM4_tx_pattern_t phy_pam4_tx_pattern_type;
} pmgr_pam4_tx_pattern_type_map_t;

/*! Pause type map. */
typedef struct pmgr_pause_type_map_s {
    pm_port_pause_t pm_pause_type;
    bcmpc_phy_pause_type_t pmgr_pause_type;
} pmgr_pause_type_map_t;

/*! Phy channel type map */
typedef struct pmgr_phy_channel_type_map_s {
    pm_port_channel_t pm_channel_type;
    bcmpc_phy_channel_t pmgr_channel_type;
} pmgr_phy_channel_map_t;

/*! Phy autoneg mode map */
typedef struct pmgr_phy_autoneg_mode_map_s {
    pm_port_autoneg_mode_t pm_autoneg_mode;
    bcmpc_phy_autoneg_mode_t pmgr_autoneg_mode;
} pmgr_phy_autoneg_mode_map_t;

/*! Phy autoneg mode map */
typedef struct pmgr_pm_vco_rate_map_s {
    pm_vco_t pm_vco_rate;
    bcmpc_vco_type_t bcmpc_pm_vco_rate;
} pmgr_pm_vco_rate_map_t;

/*! Timesync 1588 deskew compensation mode map. */
typedef struct pmgr_timesync_comp_mode_type_map_s {
    bcmpc_port_ts_comp_mode_t pmgr_timesync_comp_mode;
    pm_timesync_comp_mode_t pm_timesync_comp_mode;
} pmgr_timesync_comp_mode_type_map_t;

static const pmgr_encap_map_t pmgr_encap_map[] = {
    /* bcmpc_port_encap_t, bcmpmac_encap_mode_t */
    { BCMPC_PORT_ENCAP_IEEE, BCMPMAC_ENCAP_IEEE },
    { BCMPC_PORT_ENCAP_HIGIG, BCMPMAC_ENCAP_HIGIG },
    { BCMPC_PORT_ENCAP_HG3, BCMPMAC_ENCAP_HG3 }
};

static const pmgr_lpbk_map_t pmgr_lpbk_map[] = {
    /* bcmpc_port_lpbk_t, phymod_loopback_mode_t */
    { BCMPC_PORT_LPBK_PHY_PCS, phymodLoopbackGlobal },
    { BCMPC_PORT_LPBK_PHY_PMD, phymodLoopbackGlobalPMD },
    { BCMPC_PORT_LPBK_REMOTE_PCS, phymodLoopbackRemotePCS },
    { BCMPC_PORT_LPBK_REMOTE_PMD, phymodLoopbackRemotePMD }
};

static const pmgr_fec_map_t pmgr_fec_map[] = {
    /* bcmpc_fec_t, phymod_fec_type_t */
    { BCMPC_FEC_NONE, phymod_fec_None },
    { BCMPC_FEC_BASE_R, phymod_fec_CL74 },
    { BCMPC_FEC_RS_528, phymod_fec_CL91 },
    { BCMPC_FEC_RS_544, phymod_fec_RS544 },
    { BCMPC_FEC_RS_272, phymod_fec_RS272 },
    { BCMPC_FEC_RS_544_2XN, phymod_fec_RS544_2XN },
    { BCMPC_FEC_RS_272_2XN, phymod_fec_RS272_2XN },
};

static const pmgr_tap_mode_map_t pmgr_tap_mode_map[] = {
    /* bcmpc_phy_tap_mode_t, phymod_tx_tap_mode_t */
    { BCMPC_PHY_TXFIR_TAP_DEFAULT, phymodTxTapModeDefault },
    { BCMPC_PHY_TXFIR_NRZ_TAPS_6, phymodTxTapMode6Tap },
    { BCMPC_PHY_TXFIR_PAM4_TAPS_6, phymodTxTapMode6Tap },
    { BCMPC_PHY_TXFIR_NRZ_LP_TAPS_3, phymodTxTapMode3Tap },
    { BCMPC_PHY_TXFIR_PAM4_LP_TAPS_3, phymodTxTapMode3Tap },
};

static const pmgr_sig_mode_map_t pmgr_sig_mode_map[] = {
    /* bcmpc_phy_sig_mode_t, phymod_phy_signalling_method_t */
    { BCMPC_PHY_SIG_MODE_NRZ, phymodSignallingMethodNRZ },
    { BCMPC_PHY_SIG_MODE_PAM4, phymodSignallingMethodPAM4 },
};

static const pmgr_media_type_map_t pmgr_media_type_map[] = {
    /* bcmpc_phy_medium_t, phymod_firmware_media_type_t */
    {BCMPC_PHY_MEDIUM_BACKPLANE, phymodFirmwareMediaTypePcbTraceBackPlane},
    {BCMPC_PHY_MEDIUM_COPPER, phymodFirmwareMediaTypeCopperCable},
    {BCMPC_PHY_MEDIUM_OPTICAL, phymodFirmwareMediaTypeOptics},
};

static const pmgr_pam4_tx_pattern_type_map_t pmgr_pam4_tx_pat_type_map[] = {
    /* bcmpc_pam4_tx_pattern_t, phymod_PAM4_tx_pattern_t */
    {BCMPC_PAM4_PATTERN_JP03B, phymod_PAM4TxPattern_JP03B},
    {BCMPC_PAM4_PATTERN_LINEAR, phymod_PAM4TxPattern_Linear},
};

static const pmgr_pause_type_map_t pmgr_pause_type_map[] = {
    /* pm_port_pause_t, bcmpc_phy_pause_type_t*/
    {PM_PORT_PAUSE_NONE, BCMPC_PHY_PAUSE_NONE},
    {PM_PORT_PAUSE_TX, BCMPC_PHY_PAUSE_TX},
    {PM_PORT_PAUSE_RX, BCMPC_PHY_PAUSE_RX},
    {PM_PORT_PAUSE_SYMM, BCMPC_PHY_PAUSE_SYMM},
    {PM_PORT_PAUSE_ALL, BCMPC_PHY_PAUSE_ALL}
};

static const pmgr_phy_channel_map_t pmgr_phy_channel_type_map[] = {
     /* pm_port_channel_t, bcmpc_phy_channel_t*/
     {PM_PORT_CHANNEL_SHORT, BCMPC_PHY_CHANNEL_SHORT},
     {PM_PORT_CHANNEL_LONG, BCMPC_PHY_CHANNEL_LONG},
     {PM_PORT_CHANNEL_ALL, BCMPC_PHY_CHANNEL_ALL}
};

static const pmgr_phy_autoneg_mode_map_t pmgr_phy_autoneg_mode_map[] = {
    /* pm_port_autoneg_mode_t, bcmpc_phy_autoneg_mode_t*/
    {PM_PORT_AUTONEG_MODE_NONE, BCMPC_PHY_AUTONEG_MODE_NONE},
    {PM_PORT_AUTONEG_MODE_CL37, BCMPC_PHY_AUTONEG_MODE_CL37},
    {PM_PORT_AUTONEG_MODE_CL37_BAM, BCMPC_PHY_AUTONEG_MODE_CL37_BAM},
    {PM_PORT_AUTONEG_MODE_CL73, BCMPC_PHY_AUTONEG_MODE_CL73},
    {PM_PORT_AUTONEG_MODE_CL73_BAM, BCMPC_PHY_AUTONEG_MODE_CL73_BAM},
    {PM_PORT_AUTONEG_MODE_CL73_MSA, BCMPC_PHY_AUTONEG_MODE_MSA},
    {PM_PORT_AUTONEG_MODE_SGMII, BCMPC_PHY_AUTONEG_MODE_SGMII}
};

static const pmgr_pm_vco_rate_map_t pmgr_pm_vco_rate_map[] = {
    /* pm_vco_t, bcmpc_vco_type_t*/
    {PM_VCO_INVALID,  BCMPC_VCO_RATE_NONE},
    {PM_VCO_10P3125G, BCMPC_VCO_RATE_10P3125G},
    {PM_VCO_12P5G,    BCMPC_VCO_RATE_12P5G},
    {PM_VCO_20P625G,  BCMPC_VCO_RATE_20P625G},
    {PM_VCO_25P781G,  BCMPC_VCO_RATE_25P781G},
    {PM_VCO_26P562G,  BCMPC_VCO_RATE_26P562G},
    {PM_VCO_41P25G,   BCMPC_VCO_RATE_41P25G},
    {PM_VCO_51P562G,  BCMPC_VCO_RATE_51P562G},
    {PM_VCO_53P125G,  BCMPC_VCO_RATE_53P125G}

};

static const pmgr_timesync_comp_mode_type_map_t
                                           pmgr_pm_timesync_comp_mode_map[] = {
    /* bcmpc_port_ts_comp_mode_t, phymod_timesync_compensation_mode_t */
    {BCMPC_TIMESYNC_COMP_NONE, PM_TIMESYNC_COMP_MODE_NONE},
    {BCMPC_TIMESYNC_COMP_EARLIEST_LANE, PM_TIMESYNC_COMP_MODE_EARLIEST_LANE},
    {BCMPC_TIMESYNC_COMP_LATEST_LANE, PM_TIMESYNC_COMP_LATEST_LANE},
    {BCMPC_TIMESYNC_COMP_COUNT, PM_TIMESYNC_COMP_COUNT}
};

/*******************************************************************************
 * Private functions
 */

/*
 * \brief Get the PHYMOD interface type for 1-lane port.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_one_lane_get(int speed, int is_hg, int fiber_pref,
                              phymod_interface_t *intf)
{
    if (speed > 12000) {
        if (is_hg) {
            *intf = phymodInterfaceCR;
        } else {
            *intf = fiber_pref ? phymodInterfaceSR : phymodInterfaceCR;
        }
    } else if (speed >= 10000) {
        if (is_hg) {
            *intf = phymodInterfaceXFI;
        } else {
            *intf = fiber_pref ? phymodInterfaceSFI : phymodInterfaceXFI;
        }
    } else if (speed == 5000) {
        *intf = phymodInterface1000X;
    } else {
        *intf = fiber_pref ? phymodInterface1000X : phymodInterfaceSGMII;
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the PHYMOD interface type for 2-lanes port.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_two_lane_get(int speed, int is_hg, int fiber_pref,
                              phymod_interface_t *intf)
{
    if (speed >= 40000) {
        *intf = phymodInterfaceKR2;
    } else if (speed >= 20000) {
        *intf = phymodInterfaceCR2;
    } else {
        *intf = phymodInterfaceRXAUI;
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the PHYMOD interface type for 4-lanes port.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
pmgr_phymod_intf_four_lane_get(int speed, int is_hg, int fiber_pref,
                               phymod_interface_t *intf)
{
    if (speed >= 100000) {
        *intf = is_hg ? phymodInterfaceCR4 : phymodInterfaceCAUI4;
    } else if (speed >= 40000) {
        *intf = is_hg ? phymodInterfaceCR4 : phymodInterfaceXLAUI;
    } else if (speed >= 20000) {
        if (is_hg) {
            *intf = phymodInterfaceXGMII;
        } else {
            *intf = fiber_pref ? phymodInterfaceSR4 : phymodInterfaceCR4;
        }
    } else {
        *intf = is_hg ? phymodInterfaceXGMII : phymodInterfaceXAUI;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Convert PC tap mode to PHYMOD.
 *
 * \param [in] pmgr_mode PC tap mode, bcmpc_phy_tap_mode_t.
 * \param [out] phy_mode PHYMOD tap mode, phymod_tx_tap_mode_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_tap_mode_to_phy(bcmpc_phy_txfir_tap_mode_t pmgr_mode,
                     phymod_tx_tap_mode_t *phy_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_tap_mode_map); idx++) {
        if (pmgr_mode == pmgr_tap_mode_map[idx].pmgr_mode) {
            *phy_mode = pmgr_tap_mode_map[idx].phy_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*!
 * \brief Convert PC tap mode from PHYMOD.
 *
 * \param [in] phy_mode PHYMOD tap mode, phymod_tx_tap_mode_t.
 * \param [out] pmgr_mode PC tap mode, bcmpc_phy_tap_mode_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_tap_mode_from_phy(phymod_tx_tap_mode_t phy_mode,
                       bcmpc_phy_txfir_tap_mode_t *pmgr_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_tap_mode_map); idx++) {
        if (phy_mode == pmgr_tap_mode_map[idx].phy_mode) {
            *pmgr_mode = pmgr_tap_mode_map[idx].pmgr_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*!
 * \brief Convert PC siganlling mode to PHYMOD.
 *
 * \param [in] pmgr_mode PC siganlling mode, bcmpc_phy_sig_mode_t.
 * \param [out] phy_mode PHYMOD siganlling mode, phymod_phy_signalling_method_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_sig_mode_to_phy(bcmpc_phy_sig_mode_t pmgr_mode,
                     phymod_phy_signalling_method_t *phy_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_sig_mode_map); idx++) {
        if (pmgr_mode == pmgr_sig_mode_map[idx].pmgr_mode) {
            *phy_mode = pmgr_sig_mode_map[idx].phy_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*!
 * \brief Convert PC siganlling mode from PHYMOD.
 *
 * \param [in] phy_mode PHYMOD siganlling mode, phymod_phy_signalling_method_t.
 * \param [out] pmgr_mode PC siganlling mode, bcmpc_phy_sig_mode_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_sig_mode_from_phy(phymod_phy_signalling_method_t phy_mode,
                       bcmpc_phy_sig_mode_t *pmgr_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_sig_mode_map); idx++) {
        if (phy_mode == pmgr_sig_mode_map[idx].phy_mode) {
            *pmgr_mode = pmgr_sig_mode_map[idx].pmgr_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*!
 * \brief Convert media type to PHYMOD.
 *
 * \param [in] pmgr_mode PC tap mode, bcmpc_phy_tap_mode_t.
 * \param [out] phy_mode PHYMOD tap mode, phymod_tx_tap_mode_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_media_type_to_phy(bcmpc_phy_medium_t pmgr_media_type,
                       phymod_firmware_media_type_t *phy_media_type)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_media_type_map); idx++) {
        if (pmgr_media_type == pmgr_media_type_map[idx].pmgr_media_type) {
            *phy_media_type = pmgr_media_type_map[idx].phy_media_type;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*!
 * \brief Convert media type from PHYMOD.
 *
 * \param [in] phy_mode PHYMOD tap mode, phymod_tx_tap_mode_t.
 * \param [out] pmgr_mode PC tap mode, bcmpc_phy_tap_mode_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmgr_media_type_from_phy(phymod_firmware_media_type_t phy_media_type,
                         bcmpc_phy_medium_t *pmgr_media_type)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_media_type_map); idx++) {
        if (phy_media_type == pmgr_media_type_map[idx].phy_media_type) {
            *pmgr_media_type = pmgr_media_type_map[idx].pmgr_media_type;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

static int
pmgr_timesync_comp_mode_to_pm(bcmpc_port_ts_comp_mode_t pmgr_mode,
                               pm_timesync_comp_mode_t *pm_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_pm_timesync_comp_mode_map); idx++) {
        if (pmgr_mode ==
            pmgr_pm_timesync_comp_mode_map[idx].pmgr_timesync_comp_mode) {
            *pm_mode =
                    pmgr_pm_timesync_comp_mode_map[idx].pm_timesync_comp_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

/*******************************************************************************
 * Internal public functions: PMAC utilities
 */

int
bcmpc_pmgr_encap_mode_to_pmac(bcmpc_port_encap_t pmgr_mode,
                              bcmpmac_encap_mode_t *pmac_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_encap_map); idx++) {
        if (pmgr_mode == pmgr_encap_map[idx].pmgr_mode) {
            *pmac_mode = pmgr_encap_map[idx].pmac_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

int
bcmpc_pmgr_encap_mode_from_pmac(bcmpmac_encap_mode_t pmac_mode,
                                bcmpc_port_encap_t *pmgr_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_encap_map); idx++) {
        if (pmac_mode == pmgr_encap_map[idx].pmac_mode) {
            *pmgr_mode = pmgr_encap_map[idx].pmgr_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

int
bcmpc_pmgr_pm_mode_to_pmac(bcmpc_pm_mode_t *pm_mode,
                           bcmpmac_core_port_mode_t *pmac_mode)
{
    sal_memcpy(pmac_mode->lane_map, pm_mode->lane_map,
               sizeof(pmac_mode->lane_map));

    return SHR_E_NONE;
}


/*******************************************************************************
 * Internal public functions: PHYMOD utilities
 */

int
bcmpc_pmgr_lpbk_mode_to_phy(bcmpc_port_lpbk_t pmgr_mode,
                            phymod_loopback_mode_t *phy_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_lpbk_map); idx++) {
        if (pmgr_mode == pmgr_lpbk_map[idx].pmgr_mode) {
            *phy_mode = pmgr_lpbk_map[idx].phy_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

int
bcmpc_pmgr_lpbk_mode_from_phy(phymod_loopback_mode_t phy_mode,
                              bcmpc_port_lpbk_t *pmgr_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_lpbk_map); idx++) {
        if (phy_mode == pmgr_lpbk_map[idx].phy_mode) {
            *pmgr_mode = pmgr_lpbk_map[idx].pmgr_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

int
bcmpc_pmgr_default_phymod_intf_get(int speed, int num_lanes, int is_hg,
                                   int fiber_pref, phymod_interface_t *intf)
{
    switch (num_lanes) {
        case 1:
            pmgr_phymod_intf_one_lane_get(speed, is_hg, fiber_pref, intf);
            break;
        case 2:
            pmgr_phymod_intf_two_lane_get(speed, is_hg, fiber_pref, intf);
            break;
        case 4:
            pmgr_phymod_intf_four_lane_get(speed, is_hg, fiber_pref, intf);
            break;
        case 8:
        case 10:
        case 12:
            *intf = phymodInterfaceCAUI;
            break;
        default:
            return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

int
bcmpc_pmgr_ref_clk_to_phy(int ref_clk, phymod_ref_clk_t *phymod_clk)
{
    switch (ref_clk) {
        case 0:
        case 156000:
            *phymod_clk = phymodRefClk156Mhz;
            break;
        case 125000:
            *phymod_clk = phymodRefClk125Mhz;
            break;
        case 106000:
            *phymod_clk = phymodRefClk106Mhz;
            break;
        case 161000:
            *phymod_clk = phymodRefClk161Mhz;
            break;
        case 174000:
            *phymod_clk = phymodRefClk174Mhz;
            break;
        case 312000:
            *phymod_clk = phymodRefClk312Mhz;
            break;
        case 322000:
            *phymod_clk = phymodRefClk322Mhz;
            break;
        case 349000:
            *phymod_clk = phymodRefClk349Mhz;
            break;
        case 644000:
            *phymod_clk = phymodRefClk644Mhz;
            break;
        case 698000:
            *phymod_clk = phymodRefClk698Mhz;
            break;
        case 155000:
            *phymod_clk = phymodRefClk155Mhz;
            break;
        case 156600:
            *phymod_clk = phymodRefClk156P6Mhz;
            break;
        case 157000:
            *phymod_clk = phymodRefClk157Mhz;
            break;
        case 158000:
            *phymod_clk = phymodRefClk158Mhz;
            break;
        case 159000:
            *phymod_clk = phymodRefClk159Mhz;
            break;
        case 168000:
            *phymod_clk = phymodRefClk168Mhz;
            break;
        case 172000:
            *phymod_clk = phymodRefClk172Mhz;
            break;
        case 173000:
            *phymod_clk = phymodRefClk173Mhz;
            break;
        case 169409:
            *phymod_clk = phymodRefClk169P409Mhz;
            break;
        case 348125:
            *phymod_clk = phymodRefClk348P125Mhz;
            break;
        case 162948:
            *phymod_clk = phymodRefClk162P948Mhz;
            break;
        case 336094:
            *phymod_clk = phymodRefClk336P094Mhz;
            break;
        case 168120:
            *phymod_clk = phymodRefClk168P12Mhz;
            break;
        case 346740:
            *phymod_clk = phymodRefClk346P74Mhz;
            break;
        case 167410:
            *phymod_clk = phymodRefClk167P41Mhz;
            break;
        case 345280:
            *phymod_clk = phymodRefClk345P28Mhz;
            break;
        case 162260:
            *phymod_clk = phymodRefClk162P26Mhz;
            break;
        case 334660:
            *phymod_clk = phymodRefClk334P66Mhz;
            break;
        default:
            return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

int
bcmpc_pmgr_fec_to_phy(bcmpc_fec_t pmgr_mode, phymod_fec_type_t *phy_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_fec_map); idx++) {
        if (pmgr_mode == pmgr_fec_map[idx].pmgr_mode) {
            *phy_mode = pmgr_fec_map[idx].phy_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

int
bcmpc_pmgr_fec_from_phy(phymod_fec_type_t phy_mode, bcmpc_fec_t *pmgr_mode)
{
    int idx;

    for (idx = 0; idx < COUNTOF(pmgr_fec_map); idx++) {
        if (phy_mode == pmgr_fec_map[idx].phy_mode) {
            *pmgr_mode = pmgr_fec_map[idx].pmgr_mode;
            return SHR_E_NONE;
        }
    }

    return SHR_E_FAIL;
}

int
bcmpc_pmgr_pm_core_cfg_to_phy(int unit, bcmpc_pm_core_cfg_t *ccfg,
                              int num_lanes,
                              phymod_core_init_config_t *phymod_ccfg)
{
    int i;
    phymod_lane_map_t *lane_map = &phymod_ccfg->lane_map;
    phymod_polarity_t *polarity_map = &phymod_ccfg->polarity_map;

    SHR_FUNC_ENTER(unit);

    phymod_core_init_config_t_init(phymod_ccfg);
    lane_map->num_of_lanes = num_lanes;
    for (i = 0; i < num_lanes; i++) {
        lane_map->lane_map_rx[i] = BCMPC_LANE_MAP_GET(ccfg->rx_lane_map, i);
        lane_map->lane_map_tx[i] = BCMPC_LANE_MAP_GET(ccfg->tx_lane_map, i);
    }
    polarity_map->rx_polarity = ccfg->rx_polarity_flip;
    polarity_map->tx_polarity = ccfg->tx_polarity_flip;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_ref_clk_to_phy(ccfg->pmd_com_clk,
                                   &phymod_ccfg->interface.ref_clock));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_tx_taps_cfg_to_phy(int unit, bcmpc_tx_taps_config_t *tx_cfg,
                              phymod_tx_t *phymod_tx)
{
    SHR_FUNC_ENTER(unit);

    phymod_tx->amp = (tx_cfg->amp_sign) ? tx_cfg->amp * (-1)
                                              : tx_cfg->amp;
    phymod_tx->main = (tx_cfg->main_sign) ? tx_cfg->main * (-1)
                                                : tx_cfg->main;
    phymod_tx->pre = (tx_cfg->pre_sign) ? tx_cfg->pre * (-1)
                                              : tx_cfg->pre;
    phymod_tx->pre2 = (tx_cfg->pre2_sign) ? tx_cfg->pre2 * (-1)
                                                : tx_cfg->pre2;
    phymod_tx->post = (tx_cfg->post_sign) ? tx_cfg->post * (-1)
                                                : tx_cfg->post;
    phymod_tx->post2 = (tx_cfg->post2_sign) ? tx_cfg->post2 * (-1)
                                                  : tx_cfg->post2;
    phymod_tx->post3 = (tx_cfg->post3_sign) ? tx_cfg->post3 * (-1)
                                                  : tx_cfg->post3;
    phymod_tx->rpara = (tx_cfg->rpara_sign) ? tx_cfg->rpara * (-1)
                                               : tx_cfg->rpara;
    phymod_tx->drivermode = (tx_cfg->drv_mode_sign) ?
                                                         tx_cfg->drv_mode * (-1)
                                                         : tx_cfg->drv_mode;
    
    phymod_tx->pre3 = 0;

    SHR_IF_ERR_EXIT
        (pmgr_tap_mode_to_phy(tx_cfg->txfir_tap_mode,
                              &(phymod_tx->tap_mode)));
    SHR_IF_ERR_EXIT
        (pmgr_sig_mode_to_phy(tx_cfg->sig_mode, &(phymod_tx->sig_method)));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pmd_lane_config_to_phy(int unit, bcmpc_pmd_firmware_config_t *fm_cfg,
                                  phymod_firmware_lane_config_t *pmd_lane_cfg)
{
    SHR_FUNC_ENTER(unit);

    phymod_firmware_lane_config_t_init(pmd_lane_cfg);
    pmd_lane_cfg->DfeOn              = fm_cfg->dfe;
    pmd_lane_cfg->LpDfeOn            = fm_cfg->lp_dfe;
    pmd_lane_cfg->Cl72RestTO         = fm_cfg->cl72_res_to;
    pmd_lane_cfg->LpPrecoderEnabled  = fm_cfg->lp_tx_precoder_on;
    pmd_lane_cfg->UnreliableLos      = fm_cfg->unreliable_los;
    pmd_lane_cfg->ScramblingDisable  = (fm_cfg->scrambling_en == 0) ? 1 : 0;
    pmd_lane_cfg->ForceNormalReach   = (fm_cfg->norm_reach_pam4_bmask);
    pmd_lane_cfg->ForceExtenedReach  = (fm_cfg->ext_reach_pam4_bmask);

    SHR_IF_ERR_EXIT
       (pmgr_media_type_to_phy(fm_cfg->medium, &pmd_lane_cfg->MediaType));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_pmd_lane_config_from_phy(int unit,
                                    phymod_firmware_lane_config_t *pmd_lane_cfg,
                                    bcmpc_pmd_firmware_config_t *fm_cfg)
{
    SHR_FUNC_ENTER(unit);

    fm_cfg->dfe                 = pmd_lane_cfg->DfeOn;
    fm_cfg->lp_dfe              = pmd_lane_cfg->LpDfeOn;
    fm_cfg->cl72_res_to         = pmd_lane_cfg->Cl72RestTO;
    fm_cfg->lp_tx_precoder_on   = pmd_lane_cfg->LpPrecoderEnabled;
    fm_cfg->lp_tx_precoder_on   = pmd_lane_cfg->LpPrecoderEnabled;
    fm_cfg->unreliable_los      = pmd_lane_cfg->UnreliableLos;
    fm_cfg->scrambling_en       = (pmd_lane_cfg->ScramblingDisable)? 0: 1;
    fm_cfg->norm_reach_pam4_bmask = (pmd_lane_cfg->ForceNormalReach);
    fm_cfg->ext_reach_pam4_bmask = (pmd_lane_cfg->ForceExtenedReach);

    SHR_IF_ERR_EXIT
       (pmgr_media_type_from_phy(pmd_lane_cfg->MediaType, &fm_cfg->medium));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pmgr_tx_lane_status_from_phy(int unit, phymod_tx_t *phymod_tx,
                                   bcmpc_tx_taps_config_t *st_tx)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(st_tx, 0, sizeof(*st_tx));

    if (phymod_tx->amp < 0) {
        st_tx->amp = phymod_tx->amp * (-1);
        st_tx->amp_sign = TRUE;
    } else {
        st_tx->amp = phymod_tx->amp;
        st_tx->amp_sign = FALSE;
    }
    if ( phymod_tx->main < 0) {
        st_tx->main = phymod_tx->main * (-1);
        st_tx->main_sign = TRUE;
    } else {
        st_tx->main = phymod_tx->main;
        st_tx->main_sign = FALSE;
    }
    if (phymod_tx->pre < 0) {
        st_tx->pre = phymod_tx->pre * (-1);
        st_tx->pre_sign = TRUE;
    } else {
        st_tx->pre = phymod_tx->pre;
        st_tx->pre_sign = FALSE;
    }
    if (phymod_tx->pre2 < 0) {
        st_tx->pre2 = phymod_tx->pre2 * (-1);
        st_tx->pre2_sign = TRUE;
    } else {
        st_tx->pre2 = phymod_tx->pre2;
        st_tx->pre2_sign = FALSE;
    }
    if (phymod_tx->post < 0) {
        st_tx->post = phymod_tx->post * (-1);
        st_tx->post_sign = TRUE;
    } else {
        st_tx->post = phymod_tx->post;
        st_tx->post_sign = FALSE;
    }
    if (phymod_tx->post2 < 0) {
        st_tx->post2 = phymod_tx->post2 * (-1);
        st_tx->post2_sign = TRUE;
    } else {
        st_tx->post2 = phymod_tx->post2;
        st_tx->post2_sign = FALSE;
    }
    if (phymod_tx->post3 < 0) {
        st_tx->post3 = phymod_tx->post3 * (-1);
        st_tx->post3_sign = TRUE;
    } else {
        st_tx->post3 = phymod_tx->post3;
        st_tx->post3_sign = FALSE;
    }
    if (phymod_tx->rpara < 0) {
        st_tx->rpara = phymod_tx->rpara * (-1);
        st_tx->rpara_sign = TRUE;
    } else {
        st_tx->rpara = phymod_tx->rpara;
        st_tx->rpara_sign = FALSE;
    }
    if (phymod_tx->drivermode < 0) {
        st_tx->drv_mode = phymod_tx->drivermode * (-1);
        st_tx->drv_mode_sign = TRUE;
    } else {
        st_tx->drv_mode = phymod_tx->drivermode;
        st_tx->drv_mode_sign = FALSE;
    }
    rv = pmgr_tap_mode_from_phy(phymod_tx->tap_mode, &st_tx->txfir_tap_mode);
    if (SHR_FAILURE(rv)) {
        st_tx->txfir_tap_mode = BCMPC_PHY_TXFIR_TAP_DEFAULT;
    }
    rv = pmgr_sig_mode_from_phy(phymod_tx->sig_method, &st_tx->sig_mode);
    if (SHR_FAILURE(rv)) {
        st_tx->sig_mode = BCMPC_PHY_SIG_MODE_NRZ;
    }

    SHR_FUNC_EXIT();
}

phymod_phy_signalling_method_t
bcmpc_phy_signal_mode_get(uint32_t lane_spd)
{
    if (lane_spd > NRZ_LANE_SPEED_MAX) {
        return phymodSignallingMethodPAM4;
    }

    return phymodSignallingMethodNRZ;
}

int
bcmpc_pmgr_pm_core_cfg_to_pm_cfg(int unit, bcmpc_pm_core_cfg_t *ccfg,
                                 int num_lanes,
                                 pm_port_add_info_t  *add_info)
{
    int i;
    phymod_lane_map_t *lane_map = &add_info->init_config.lane_map;
    phymod_polarity_t *polarity = &add_info->init_config.polarity;

    phymod_lane_map_t_init(lane_map);
    phymod_polarity_t_init(polarity);
    for (i = 0; i < num_lanes; i++) {
        lane_map->lane_map_rx[i] = BCMPC_LANE_MAP_GET(ccfg->rx_lane_map, i);
        lane_map->lane_map_tx[i] = BCMPC_LANE_MAP_GET(ccfg->tx_lane_map, i);
    }
    polarity->rx_polarity = ccfg->rx_polarity_flip;
    polarity->tx_polarity = ccfg->tx_polarity_flip;

    lane_map->num_of_lanes = num_lanes;

    return SHR_E_NONE;
}

int
bcmpc_pmgr_port_cfg_to_pm_cfg(int unit, bcmpc_port_cfg_t *pcfg,
                              pm_port_add_info_t *add_info)
{
    add_info->flags = 0;
    add_info->init_config.ref_clk = phymodRefClk156Mhz;

    add_info->speed_config.speed = pcfg->speed;
    add_info->speed_config.num_lanes = pcfg->num_lanes;
    add_info->speed_config.fec = pcfg->fec_mode;
    add_info->speed_config.link_training = pcfg->link_training;

    return SHR_E_NONE;
}

int
bcmpc_pmgr_port_speed_config_to_pm_cfg(int unit, bcmpc_port_cfg_t *pcfg,
                                       pm_speed_config_t *speed_config)
{
    speed_config->speed = pcfg->speed;
    speed_config->num_lanes = pcfg->num_lanes;
    speed_config->fec = pcfg->fec_mode;
    speed_config->link_training = pcfg->link_training;

    return SHR_E_NONE;
}

int
bcmpc_pmgr_port_pm_cfg_to_speed_config(int unit,
                                       pm_speed_config_t *speed_config,
                                       bcmpc_port_cfg_t *pcfg)
{
    pcfg->speed = speed_config->speed;
    pcfg->num_lanes = speed_config->num_lanes;
    pcfg->fec_mode = speed_config->fec;
    pcfg->link_training = speed_config->link_training;

    return SHR_E_NONE;
}

int
bcmpc_pmgr_port_ability_to_pm_ability(int unit, bcmpc_autoneg_prof_t *an_prof,
                                      pm_port_ability_t *ability)
{

    ability->speed = an_prof->an_advert_speed;
    ability->num_lanes = an_prof->num_lanes;
    ability->an_mode = an_prof->autoneg_mode;
    ability->fec_type = an_prof->fec_mode;
    ability->medium = an_prof->phy_medium;
    switch(an_prof->pause_type) {
        case 0:
            ability->pause = PM_PORT_PAUSE_NONE;
            break;
        case 1:
            ability->pause = PM_PORT_PAUSE_TX;
            break;
        case 2:
            ability->pause = PM_PORT_PAUSE_RX;
            break;
        case 3:
            ability->pause = PM_PORT_PAUSE_SYMM;
            break;
        default:
            return SHR_E_PARAM;
    }
    ability->channel = an_prof->long_ch ? PM_PORT_CHANNEL_LONG :
                                          PM_PORT_CHANNEL_SHORT;

    return SHR_E_NONE;
}

int
bcmpc_pmgr_pm_pfc_cfg_to_pm_cfg(int unit, bcmpc_pfc_control_t *pctrl,
                                bcmpmac_pfc_t *pfc_cfg)
{
    pfc_cfg->tx_enable = pctrl->tx_enable;
    pfc_cfg->rx_enable = pctrl->rx_enable;
    pfc_cfg->stats_enable = pctrl->stats_enable;
    pfc_cfg->refresh_timer = pctrl->refresh_timer;
    sal_memcpy(pfc_cfg->da, pctrl->da, sizeof(pfc_cfg->da));
    pfc_cfg->eth_type = pctrl->eth_type;
    pfc_cfg->opcode = pctrl->opcode;
    pfc_cfg->xoff_timer = pctrl->xoff_timer;
    pfc_cfg->pfc_pass = pctrl->pfc_pass;

    return SHR_E_NONE;
}

int
bcmpc_pmgr_pm_pfc_cfg_from_pm_cfg(int unit, bcmpmac_pfc_t *pfc_cfg,
                                bcmpc_pfc_control_t *pctrl)
{
    pctrl->tx_enable = pfc_cfg->tx_enable;
    pctrl->rx_enable = pfc_cfg->rx_enable;
    pctrl->stats_enable = pfc_cfg->stats_enable;
    pctrl->refresh_timer = pfc_cfg->refresh_timer;
    sal_memcpy(pctrl->da, pfc_cfg->da, sizeof(pfc_cfg->da));
    pctrl->eth_type = pfc_cfg->eth_type;
    pctrl->opcode = pfc_cfg->opcode;
    pctrl->xoff_timer = pfc_cfg->xoff_timer;
    pctrl->pfc_pass = pfc_cfg->pfc_pass;

    return SHR_E_NONE;
}

void
bcmpc_pmgr_phy_status_from_pm_phy_status(int unit, pm_phy_status_t *pm_phy_st,
                                         bcmpc_pmgr_phy_status_t *pmgr_phy_st)
{
    uint8_t i;

    if (pm_phy_st->tx_control == phymodTxSquelchOn) {
        pmgr_phy_st->tx_squelch = 1;
    }
    if (pm_phy_st->tx_control == phymodTxSquelchOff) {
        pmgr_phy_st->tx_squelch = 0;
    }
    if (pm_phy_st->rx_control == phymodRxSquelchOn) {
        pmgr_phy_st->rx_squelch = 1;
    }
    if (pm_phy_st->rx_control == phymodRxSquelchOff) {
        pmgr_phy_st->rx_squelch = 0;
    }
    pmgr_phy_st->rx_afe_vga = pm_phy_st->rx_afe.vga.value;
    pmgr_phy_st->rx_afe_pk_flt = pm_phy_st->rx_afe.peaking_filter.value;
    pmgr_phy_st->rx_afe_low_fre_pk_flt =
                            pm_phy_st->rx_afe.low_freq_peaking_filter.value;
    pmgr_phy_st->rx_afe_hi_fre_pk_flt =
                            pm_phy_st->rx_afe.high_freq_peaking_filter.value;
    for (i = 0 ; i < pm_phy_st->rx_afe.num_of_dfe_taps; i++) {
        if (pm_phy_st->rx_afe.dfe[i].value < 0 ) {
            pmgr_phy_st->rx_afe_dfe_tap[i] =
                                          pm_phy_st->rx_afe.dfe[i].value * (-1);
            pmgr_phy_st->rx_afe_dfe_tap_sign[i] = TRUE;
        } else {
            pmgr_phy_st->rx_afe_dfe_tap[i] = pm_phy_st->rx_afe.dfe[i].value;
            pmgr_phy_st->rx_afe_dfe_tap_sign[i] = FALSE;
        }
    }
    pmgr_phy_st->rx_signal_detect = pm_phy_st->rx_signal_detect;
    pmgr_phy_st->pam4_tx_precoder_enabled = pm_phy_st->pam4_tx_precoder_enabled;
    pmgr_phy_st->phy_ecc_intr_enabled = pm_phy_st->phy_ecc_intr_enabled;
    pmgr_phy_st->pmd_debug_lane_event_log_level = pm_phy_st->pmd_debug_lane_event_log_level;


    for (i = 0; i < COUNTOF(pmgr_pam4_tx_pat_type_map); i++) {
        if (pm_phy_st->pam4_tx_pat ==
                      pmgr_pam4_tx_pat_type_map[i].phy_pam4_tx_pattern_type) {
            pmgr_phy_st->pam4_tx_pattern =
                      pmgr_pam4_tx_pat_type_map[i].pmgr_pam4_tx_pattern_type;
            break;
         }
    }
    if (pm_phy_st->pam4_tx_pat == phymod_Count) {
        pmgr_phy_st->pam4_tx_pattern = BCMPC_PAM4_PATTERN_NONE;
    }

    if (pm_phy_st->tx_pi < 0) {
        pmgr_phy_st->tx_pi = pm_phy_st->tx_pi * (-1);
        pmgr_phy_st->tx_pi_sign = TRUE;
    } else {
        pmgr_phy_st->tx_pi = pm_phy_st->tx_pi;
        pmgr_phy_st->tx_pi_sign = FALSE;
    }

    pmgr_phy_st->phy_fec_bypass_ind = pm_phy_st->phy_fec_bypass_ind;
}

void
bcmpc_pmgr_phy_ctrl_to_pm_phy_ctrl(int unit, bcmpc_phy_control_t *pctrl,
                                   pm_phy_control_t *pm_pctrl)
{
    int i;

    if (pctrl->tx_squelch) {
        pm_pctrl->tx_control = phymodTxSquelchOn;
    }  else {
        pm_pctrl->tx_control = phymodTxSquelchOff;
    }
    if (pctrl->rx_squelch) {
        pm_pctrl->rx_control = phymodRxSquelchOn;
    } else {
        pm_pctrl->rx_control = phymodRxSquelchOff;
    }
    for (i = 0; i < PHY_NUM_RX_DFE_TAPS_MAX; i++) {
        if (pctrl->rx_afe_dfe_tap_sign[i]) {
            pm_pctrl->rx_afe.dfe[i].value = pctrl->rx_afe_dfe_tap[i] * -1;
        } else {
            pm_pctrl->rx_afe.dfe[i].value = pctrl->rx_afe_dfe_tap[i];
        }
    }

    pm_pctrl->rx_afe.num_of_dfe_taps = PHY_NUM_RX_DFE_TAPS_MAX;
    pm_pctrl->rx_afe.vga.value = pctrl->rx_afe_vga;
    pm_pctrl->rx_afe.peaking_filter.value = pctrl->rx_afe_pk_flt;
    pm_pctrl->rx_afe.low_freq_peaking_filter.value =
                                                  pctrl->rx_afe_low_fre_pk_flt;
    pm_pctrl->rx_afe.high_freq_peaking_filter.value =
                                                   pctrl->rx_afe_hi_fre_pk_flt;
    pm_pctrl->pam4_tx_precoder_enable = pctrl->pam4_tx_precoder;
    pm_pctrl->phy_ecc_intr_enable = pctrl->phy_ecc_intr;
    pm_pctrl->pmd_debug_lane_event_log_level = pctrl->pmd_debug_lane_event_log_level;
    pm_pctrl->phy_fec_bypass_ind = pctrl->phy_fec_bypass_ind;

    for (i = 0; i < COUNTOF(pmgr_pam4_tx_pat_type_map); i++) {
        if (pctrl->pam4_tx_pat ==
                      pmgr_pam4_tx_pat_type_map[i].pmgr_pam4_tx_pattern_type) {
            pm_pctrl->pam4_tx_pat =
                      pmgr_pam4_tx_pat_type_map[i].phy_pam4_tx_pattern_type;
            pm_pctrl->pam4_tx_pat_enable = 1;
            break;
         }
    }
    if (pctrl->pam4_tx_pat == BCMPC_PAM4_PATTERN_NONE) {
        pm_pctrl->pam4_tx_pat_enable = 0;
    }

    if (pctrl->tx_pi_ovride_sign) {
        pm_pctrl->tx_pi = pctrl->tx_pi_ovride * (-1);
    } else {
        pm_pctrl->tx_pi = pctrl->tx_pi_ovride;
    }
}

void
pm_port_ability_to_bcmpc_pmgr_ability(int unit, uint32_t num_abilities,
                                      pm_port_ability_t *port_abilities,
                                      bcmpc_pmgr_port_abilities_t *pabilities)
{
    uint8_t idx = 0, idx_2 = 0;

    pabilities->num_ability = num_abilities;

    for (idx = 0; idx < num_abilities; idx++) {
        pabilities->port_abilities[idx].speed = port_abilities[idx].speed;
        pabilities->port_abilities[idx].num_lanes =
                                                  port_abilities[idx].num_lanes;
        pabilities->port_abilities[idx].fec_type = port_abilities[idx].fec_type;
        pabilities->port_abilities[idx].medium = port_abilities[idx].medium;
        pabilities->port_abilities[idx].encap = port_abilities[idx].encap;
        for (idx_2 = 0; idx_2 < COUNTOF(pmgr_phy_autoneg_mode_map); idx_2++) {
            if (port_abilities[idx].an_mode ==
                             pmgr_phy_autoneg_mode_map[idx_2].pm_autoneg_mode) {
                pabilities->port_abilities[idx].an_mode =
                             pmgr_phy_autoneg_mode_map[idx_2].pmgr_autoneg_mode;
                break;
            }
        }
        for (idx_2 = 0; idx_2 < COUNTOF(pmgr_pause_type_map); idx_2++) {
            if (port_abilities[idx].pause ==
                                   pmgr_pause_type_map[idx_2].pm_pause_type) {
                pabilities->port_abilities[idx].pause =
                                    pmgr_pause_type_map[idx_2].pmgr_pause_type;
                break;
            }
        }
        for (idx_2 = 0; idx_2 < COUNTOF(pmgr_phy_channel_type_map); idx_2++) {
            if (port_abilities[idx].channel ==
                             pmgr_phy_channel_type_map[idx_2].pm_channel_type) {
                pabilities->port_abilities[idx].channel =
                             pmgr_phy_channel_type_map[idx_2].pmgr_channel_type;
                break;
            }
        }
    }

}

void
pm_vco_rate_to_bcmpc_vco_rate(int unit, pm_vco_t *pm_vco,
                               bcmpc_vco_type_t *vco_rate)
{
    uint8_t idx;
    uint8_t pll_idx;

    for (pll_idx = 0; pll_idx < 2; pll_idx++) {
        for (idx = 0; idx < COUNTOF(pmgr_pm_vco_rate_map); idx++) {
            if (pm_vco[pll_idx] == pmgr_pm_vco_rate_map[idx].pm_vco_rate) {
                vco_rate[pll_idx] = pmgr_pm_vco_rate_map[idx].bcmpc_pm_vco_rate;
                break;
            }
        }
    }
}

int
bcmpc_pmgr_timesync_to_pm_ts_config(int unit, bcmpc_port_timesync_t *cfg,
                                    pm_phy_timesync_config_t *ts_cfg)
{
    int rv;

    ts_cfg->timesync_en = cfg->ieee_1588;
    ts_cfg->is_one_step = cfg->one_step_timestamp;
    ts_cfg->is_sop = cfg->is_sop;
    rv = pmgr_timesync_comp_mode_to_pm(cfg->ts_comp_mode, &(ts_cfg->comp_mode));

    return rv;
}

int
bcmpc_pmgr_timesync_to_pm_synce(int unit, bcmpc_port_timesync_t *cfg,
                                pm_port_synce_clk_ctrl_t *synce_cfg)
{
    switch (cfg->stg0_mode) {
        case BCMPC_PORT_SYNCE_STG0_MODE_DIV_NONE:
            synce_cfg->stg0_mode = PM_SYNCE_STG0_NO_DIV;
            break;
        case BCMPC_PORT_SYNCE_STG0_MODE_DIV_GAP_CLK_4_OVER_5:
            synce_cfg->stg0_mode = PM_SYNCE_STG0_DIV_GAP_CLK_4_OVER_5;
            break;
        case BCMPC_PORT_SYNCE_STG0_MODE_SDM_FRAC_DIV:
            synce_cfg->stg0_mode = PM_SYNCE_STG0_SDM_FRAC_DIV;
            break;
        default:
            return SHR_E_PARAM;
    }

    switch (cfg->stg1_mode) {
        case BCMPC_PORT_SYNCE_STG1_MODE_DIV_NONE:
            synce_cfg->stg1_mode = PM_SYNCE_STG1_NO_DIV;
            break;
        case BCMPC_PORT_SYNCE_STG1_MODE_DIV_7:
            synce_cfg->stg1_mode = PM_SYNCE_STG1_DIV_7;
            break;
        case BCMPC_PORT_SYNCE_STG1_MODE_DIV_11:
            synce_cfg->stg1_mode = PM_SYNCE_STG1_DIV_11;
            break;
        default:
            return SHR_E_PARAM;
    }

    synce_cfg->sdm_val = cfg->fractional_divisor;

    return SHR_E_NONE;
}

int
pm_port_rlm_status_to_bcmpc_pmgr_port_rlm_status(int unit,
                                          pm_port_rlm_status_t *pm_status,
                                          bcmpc_pmgr_port_rlm_status_t *status)
{

    if (pm_status->rlm_disabled) {
        *status = BCMPC_PMGR_PORT_RLM_STATUS_DISABLED;
    } else if (pm_status->rlm_failed) {
        *status = BCMPC_PMGR_PORT_RLM_STATUS_FAILED;
    } else if (pm_status->rlm_done) {
        *status = BCMPC_PMGR_PORT_RLM_STATUS_DONE;
    } else if (pm_status->rlm_busy) {
        *status = BCMPC_PMGR_PORT_RLM_STATUS_BUSY;
    } else {
        return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}
