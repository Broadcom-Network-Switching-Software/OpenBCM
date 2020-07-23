/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Functions to support CLI port commands
 * Requires:    
 */

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/debug.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>

#include <bcm/init.h>
#include <bcm/port.h>
#include <bcm/stg.h>
#include <bcm/error.h>

#ifdef SW_AUTONEG_SUPPORT
#include <bcm_int/common/sw_an.h>
#endif

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/port.h>
#endif
#ifdef BCM_DFE_SUPPORT
#include <bcm_int/dfe/port.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/port.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_m0ssq.h>
#endif
#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod.h>
#endif

static int         prev_linkscan_interval[BCM_MAX_NUM_UNITS];
/*
 * Function:
 *    if_fmt_speed
 * Purpose:
 *    Format a speed as returned from bcm_xxx for display.
 * Parameters:
 *    b     - buffer to format into.
 *    speed - speed as returned from bcm_port_speed_get
 * Returns:
 *    Pointer to buffer (b).
 */

char *
if_fmt_speed(char *b, int speed)
{
    if (speed >= 1000) {        /* Use Gb */
        if (speed % 1000) {     /* Use Decimal */
            sal_sprintf(b, "%d.%dG", speed / 1000, (speed % 1000) / 100);
        } else {
            sal_sprintf(b, "%dG", speed / 1000);
        }
    } else if (speed == 0) {
        sal_sprintf(b, "-");
    } else {                    /* Use Mb */
        sal_sprintf(b, "%dM", speed);
    }
    return (b);
}

/*
 * These are ordered according the corresponding enumerated types.
 * See soc/portmode.h, bcm/port.h and bcm/link.h for more information
 */

/* Note:  See port.h, bcm_port_discard_e */
char *discard_mode[] = {
    "None", "All", "Untag", "Tag", NULL, NULL
};

/* Note:  See link.h, bcm_linkscan_mode_e */
char *linkscan_mode[] = {
    "None", "SW", "HW", "OFF", "ON", NULL
};

/* Note:  See portmode.h, soc_port_if_e */
char *interface_mode[] = SOC_PORT_IF_NAMES_INITIALIZER;

/* Note:  See portmode.h, soc_port_ms_e */
char *phymaster_mode[] = {
    "Slave", "Master", "Auto", "None", NULL
};

/* Note:  See port.h, bcm_port_loopback_e */
char *loopback_mode[] = {
    "NONE", "MAC", "PHY", "RMT", "MAC_RMT", NULL
};

/* Note:  See port.h, bcm_port_stp_e */
char *forward_mode[] = {
    "Disable", "Block", "LIsten", "LEarn", "Forward", NULL
};

/* Note: See port.h, bcm_port_encap_e */
char *encap_mode[] = {
    "IEEE", "HIGIG", "B5632", "HIGIG2", "HIGIG2_LITE", "HIGIG2_L2",
    "HIGIG2_L3", "OBSOLETE", "SOP", "HGOE", "CPRI", "RSVD4", "HIGIG3", NULL
};

/* Note: See port.h, bcm_port_mdix_e */
char *mdix_mode[] = {
    "Auto", "ForcedAuto", "ForcedNormal", "ForcedXover", NULL
};

/* Note: See port.h, bcm_port_mdix_status_e */
char *mdix_status[] = {
    "Normal", "Xover", NULL
};

/* Note: See port.h, bcm_port_medium_t */
char *medium_status[] = {
    "None", "Copper", "Fiber", "Backplane", "All", NULL
};

/* Note: See port.h, bcm_port_mcast_flood_t */
char *mcast_flood[] = {
    "FloodAll", "FloodUnknown", "FloodNone", NULL
};


void
brief_port_info_header(int unit)
{
    char *fc = " ";
#ifdef BCM_TOMAHAWK_SUPPORT
    char *ln = "";
    char *asf[2] = {"", ""};
#endif
    char *disp_str =
        "%15s "                 /* port number */
        "%5s "                  /* enable/link state */
        "%9s "                  /* speed/duplex */
        "%4s "                  /* link scan mode */
        "%4s "                  /* auto negotiate? */
        "%7s   "                /* spantree state */
        "%5s  "                 /* pause tx/rx */
        "%6s "                  /* discard mode */
        "%3s "                  /* learn to CPU, ARL, FWD or discard */
        "%6s "                  /* interface */
        "%5s "                  /* max frame */
        "%5s\n";                /* loopback */

#ifdef BCM_TOMAHAWK_SUPPORT
    /* coverity [Out-of-bounds] */
    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        ln = "Lns";
        asf[0] = "cut "; asf[1] = "thru?";
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)){
        disp_str = 
        "%15s "                 /* port number */
        "%5s  "                 /* enable/link state */
        "%3s"                   /* lanes */
        "%9s "                  /* speed/duplex */
        "%4s "                  /* link scan mode */
        "%4s "                  /* auto negotiate? */
        "%7s   "                /* spantree state */
        "%5s  "                 /* pause tx/rx */
        "%6s "                  /* discard mode */
        "%3s "                  /* learn to CPU, ARL, FWD or discard */
        "%6s "                  /* interface */
        "%5s "                  /* max frame */
        "%6s "                  /* cutthrough */
            "%6s "                  /* FEC */
        "%8s\n";                /* loopback */
        } else
#endif
        {
            disp_str =
            "%15s "                 /* port number */
            "%5s  "                 /* enable/link state */
            "%3s"                   /* lanes */
            "%9s "                  /* speed/duplex */
            "%4s "                  /* link scan mode */
            "%4s "                  /* auto negotiate? */
            "%7s   "                /* spantree state */
            "%5s  "                 /* pause tx/rx */
            "%6s "                  /* discard mode */
            "%3s "                  /* learn to CPU, ARL, FWD or discard */
            "%6s "                  /* interface */
            "%5s "                  /* max frame */
            "%6s "                  /* cutthrough */
            "%5s\n";                /* loopback */
        }
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)){
        cli_out(disp_str, 
                   " ",                 /* port number */
                   "ena/",              /* enable/link state */
                   "  ",                /* number of lanes */
                   "speed/",            /* speed/duplex */
                   "link",              /* link scan mode */
                   "auto",              /* auto negotiate? */
                   " STP ",             /* spantree state */
                   fc,                  /* pause tx/rx or ilkn FC status */
                   " ",                 /* discard mode */
                   "lrn",               /* learn to CPU, ARL, FWD or discard */
                   "inter",             /* interface */
                   "max",               /* max frame */
                   asf[0],              /* cutthrough */
                   " ",                 /* FEC */
                   "loop");             /* loopback */
        cli_out(disp_str, 
                   "port",              /* port number */
                   "link",              /* enable/link state */
                   ln,                  /* number of lanes */
                   "duplex",            /* speed/duplex */
                   "scan",              /* link scan mode */
                   "neg?",              /* auto negotiate? */
                   "state",             /* spantree state */
                   "pause",             /* pause tx/rx */
                   "discrd",            /* discard mode */
                   "ops",               /* learn to CPU, ARL, FWD or discard */
                   "face",              /* interface */
                   "frame",             /* max frame */
                   asf[1],              /* cutthrough */
                   "FEC",               /* FEC */
                   "back");             /* loopback */

        } else
#endif
        {
            cli_out(disp_str,
                   " ",                 /* port number */
                   "ena/",              /* enable/link state */
                   "  ",                /* number of lanes */
                   "speed/",            /* speed/duplex */
                   "link",              /* link scan mode */
                   "auto",              /* auto negotiate? */
                   " STP ",             /* spantree state */
                   fc,                  /* pause tx/rx or ilkn FC status */
                   " ",                 /* discard mode */
                   "lrn",               /* learn to CPU, ARL, FWD or discard */
                   "inter",             /* interface */
                   "max",               /* max frame */
                   asf[0],              /* cutthrough */
                   "loop");             /* loopback */
            cli_out(disp_str,
                   "port",              /* port number */
                   "link",              /* enable/link state */
                   ln,                  /* number of lanes */
                   "duplex",            /* speed/duplex */
                   "scan",              /* link scan mode */
                   "neg?",              /* auto negotiate? */
                   "state",             /* spantree state */
                   "pause",             /* pause tx/rx */
                   "discrd",            /* discard mode */
                   "ops",               /* learn to CPU, ARL, FWD or discard */
                   "face",              /* interface */
                   "frame",             /* max frame */
                   asf[1],              /* cutthrough */
                   "back");             /* loopback */
        }
    } else 
#endif
    {
    cli_out(disp_str, 
            " ",                 /* port number */
            "ena/",              /* enable/link state */
            "speed/",            /* speed/duplex */
            "link",              /* link scan mode */
            "auto",              /* auto negotiate? */
            " STP ",             /* spantree state */
            fc,                  /* pause tx/rx or ilkn FC status */
            " ",                 /* discard mode */
            "lrn",               /* learn to CPU, ARL, FWD or discard */
            "inter",             /* interface */
            "max",               /* max frame */
            "loop");             /* loopback */
    cli_out(disp_str, 
            "port",              /* port number */
            "link",              /* enable/link state */
            "duplex",            /* speed/duplex */
            "scan",              /* link scan mode */
            "neg?",              /* auto negotiate? */
            "state",             /* spantree state */
            "pause",             /* pause tx/rx */
            "discrd",            /* discard mode */
            "ops",               /* learn to CPU, ARL, FWD or discard */
            "face",              /* interface */
            "frame",             /* max frame */
            "back");             /* loopback */
    }
}

#define _CHECK_PRINT(unit, flags, mask, str, val) do {                \
        if ((flags) & (mask)) {                                       \
            cli_out(str, val);               \
        } else {                                                      \
            cli_out(str, "");                \
        }                                                             \
    } while (0)

int
brief_port_info(int unit, int port, bcm_port_info_t *info, uint32 flags)
{
    char *spt_str, *discrd_str;
    char sbuf[6];
    int lrn_ptr;
    char lrn_str[4];
    char remark[40];
    int p = 0;

#if defined(BCM_KATANA2_SUPPORT)

    uint32 wc_xfi_mode_sel_val[2] = { 0 };
    uint32 top_misc_control_1_val = 0;
    soc_field_t wc_xfi_mode_sel_fld[] =
    { WC0_8_XFI_MODE_SELf, WC1_8_XFI_MODE_SELf };

#endif

    remark[0] = '\0';
    p = port;
    spt_str = FORWARD_MODE(info->stp_state);
    discrd_str = DISCARD_MODE(info->discard);

#if defined(BCM_KATANA2_SUPPORT)

        if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r
                (unit, &top_misc_control_1_val));
            wc_xfi_mode_sel_val[0] =
                soc_reg_field_get(unit, TOP_MISC_CONTROL_1r,
                                    top_misc_control_1_val,
                                    wc_xfi_mode_sel_fld[0]);
            wc_xfi_mode_sel_val[1] =
            soc_reg_field_get(unit, TOP_MISC_CONTROL_1r,
                                top_misc_control_1_val,
                                wc_xfi_mode_sel_fld[1]);
            }

            if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
                    switch (port) {
                    case 25:
                    case 36:
                            if (wc_xfi_mode_sel_val[0]) {
                            p = (port == 25) ? 32 : 34;
                            sal_sprintf(remark, "XFI#Int:%d:==>Ext:%d:", port, p);
                        }
                        break;
                    case 26:
                    case 39:
                        if (wc_xfi_mode_sel_val[1]) {
                            p = (port == 26) ? 29 : 31;
                            sal_sprintf(remark, "XFI#Int:%d:==>Ext:%d:", port, p);
                        }
                        break;
                    default:
                        remark[0] = '\0';
                        break;
                }
            }
#endif


  /* port number (7)
     * enable/link state (4)
     * speed/duplex (6)
     * link scan mode (4)
     * auto negotiate? (4)
     * spantree state (7)
     * pause tx/rx (5)
     * discard mode (6)
     * learn to CPU, ARL, FWD or discard (3)
     */
    cli_out("%10s(%3d)  %4s ", BCM_PORT_NAME(unit, port), p,
            !info->enable ? "!ena" :
            (info->linkstatus == BCM_PORT_LINK_STATUS_FAILED) ? "fail" :
            (info->linkstatus == BCM_PORT_LINK_STATUS_UP ? "up  " : "down"));
#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        if (flags & BCM_PORT_ATTR_ALL_MASK) {
            cli_out(" %2d ", info->phy_master);
        } else {
            cli_out(" %2s ", "");
        }
    }
#endif
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_SPEED_MASK,
                 "%5s ", if_fmt_speed(sbuf, info->speed));
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_DUPLEX_MASK,
                 "%3s ", info->speed == 0 ? "" : info->duplex ? "FD" : "HD");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LINKSCAN_MASK,
                 "%4s ", LINKSCAN_MODE(info->linkscan));
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_AUTONEG_MASK,
                 "%4s ", info->autoneg ? " Yes" : " No ");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_STP_STATE_MASK,
                 " %7s  ", spt_str);
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_PAUSE_TX_MASK,
                 "%2s ", info->pause_tx ? "TX" : "");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_PAUSE_RX_MASK,
                 "%2s ", info->pause_rx ? "RX" : "");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_DISCARD_MASK,
                 "%6s  ", discrd_str);

    lrn_ptr = 0;
    sal_memset(lrn_str, 0, sizeof(lrn_str));
    lrn_str[0] = 'D';
    if (info->learn & BCM_PORT_LEARN_FWD) {
        lrn_str[lrn_ptr++] = 'F';
    }
    if (info->learn & BCM_PORT_LEARN_ARL) {
        lrn_str[lrn_ptr++] = 'A';
    }
    if (info->learn & BCM_PORT_LEARN_CPU) {
        lrn_str[lrn_ptr++] = 'C';
    }
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LEARN_MASK,
                 "%3s ", lrn_str);
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_INTERFACE_MASK,
                 "%6s ", INTERFACE_MODE(info->interface));
    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        cli_out("%5d ", info->frame_max);
    } else {
        cli_out("%5s ", "");
    }
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LOOPBACK_MASK,
                 " %s",
                 info->loopback != BCM_PORT_LOOPBACK_NONE ?
                 LOOPBACK_MODE(info->loopback) : "");
    cli_out("   %s\n",remark);
    return 0;
}

#if defined(BCM_TOMAHAWK_SUPPORT)
int
esw_brief_port_info(int unit, int port, bcm_port_info_t *info, uint32 flags)
{
    char *spt_str, *discrd_str;
    char sbuf[6];
    int lrn_ptr;
    char lrn_str[4];
    int ret = 0;
#if 1
    char asf_str[4][4] = {"No", "Yes", "Yes", "Yes"};
#endif
    int asf_mode = 0;
    int p = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    char lane_num[2];
    char itf_list[3][3]= {"KR", "CR", "SR"};
    char if_name[4];
    pm_info_t pm_info;
    char fec_list[11][10] = {"NONE", "BASE-R", "RS528", "RS544-1xN", "RS272-1xN", "", "", "", "", "RS544-2xN", "RS272-2xN"};
    portmod_port_diag_info_t diag_info;
    phymod_autoneg_status_t an_status;
    portmod_speed_config_t speed_config_status;
    int medium_type = 0;
#endif
    p = port;
    spt_str = FORWARD_MODE(info->stp_state);
    discrd_str = DISCARD_MODE(info->discard);

    /* port number (7)
     * enable/link state (4)
     * speed/duplex (6)
     * link scan mode (4)
     * auto negotiate? (4)
     * spantree state (7)
     * pause tx/rx (5)
     * discard mode (6)
     * learn to CPU, ARL, FWD or discard (3)
     */
    cli_out("%10s(%3d)  %4s ", BCM_PORT_NAME(unit, port), p,
            !info->enable ? "!ena" :
            (info->linkstatus == BCM_PORT_LINK_STATUS_FAILED) ? "fail" :
            (info->linkstatus == BCM_PORT_LINK_STATUS_UP ? "up  " : "down"));

    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        if (flags & BCM_PORT_ATTR_ALL_MASK) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* if TH3 and pm8x50, need to check an resolved num lane */
            if (SOC_IS_TOMAHAWK3(unit)) {
                ret = portmod_pm_info_get(unit, port, &pm_info);
                if (ret) return ret;
                /* check is needed PM8x50 only */
                if (pm_info->type == portmodDispatchTypePm8x50) {
                    ret = portmod_port_autoneg_status_get(unit, port, &an_status);
                    if (ret) return ret;
                    if (an_status.enabled && an_status.locked) {
                        cli_out(" %2d ", an_status.resolved_num_lane);
                    } else {
                        ret = portmod_port_speed_config_get(unit, port, &speed_config_status);
                        if (ret) return ret;
                        cli_out(" %2d ", speed_config_status.num_lane);
                    }
                } else {
                    cli_out(" %2d ", info->phy_master);
                }
            /*non TH3 device */
            } else
#endif
            {
                cli_out(" %2d ", info->phy_master);
            }
        } else {
            cli_out(" %2s ", "");
        }
    }

    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_SPEED_MASK,
                 "%5s ", if_fmt_speed(sbuf, info->speed));
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_DUPLEX_MASK,
                 "%3s ", info->speed == 0 ? "" : info->duplex ? "FD" : "HD");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LINKSCAN_MASK,
                 "%4s ", LINKSCAN_MODE(info->linkscan));
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_AUTONEG_MASK,
                 "%4s ", info->autoneg ? " Yes" : " No ");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_STP_STATE_MASK,
                 " %7s  ", spt_str);
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_PAUSE_TX_MASK,
                 "%2s ", info->pause_tx ? "TX" : "");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_PAUSE_RX_MASK,
                 "%2s ", info->pause_rx ? "RX" : "");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_DISCARD_MASK,
                 "%6s  ", discrd_str);

    lrn_ptr = 0;
    sal_memset(lrn_str, 0, sizeof(lrn_str));
    lrn_str[0] = 'D';
    if (info->learn & BCM_PORT_LEARN_FWD) {
        lrn_str[lrn_ptr++] = 'F';
    }
    if (info->learn & BCM_PORT_LEARN_ARL) {
        lrn_str[lrn_ptr++] = 'A';
    }
    if (info->learn & BCM_PORT_LEARN_CPU) {
        lrn_str[lrn_ptr++] = 'C';
    }
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LEARN_MASK,
                 "%3s ", lrn_str);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)){
        ret = portmod_pm_info_get(unit, port, &pm_info);
        if (ret) return ret;
        if (pm_info->type == portmodDispatchTypePm8x50) {
            ret = portmod_port_speed_config_get(unit, port, &speed_config_status);
            if (ret) {
                return ret;
            }
            medium_type = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(speed_config_status.lane_config);
            sal_snprintf(lane_num, 2, "%d", info->phy_master);
            /* Medium type is backplane */
            if (medium_type == 0) {
                sal_strcpy(if_name, itf_list[0]);
            /* Medium type is copper */
            } else if (medium_type == 1) {
                sal_strcpy(if_name, itf_list[1]);
            /* Medium type is optics */
            } else if (medium_type == 2) {
                sal_strcpy(if_name, itf_list[2]);
            } else {
                sal_strcpy(if_name, itf_list[0]);
            }
            sal_strcat(if_name, lane_num);
            cli_out("%6s ", if_name);
        } else {
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_INTERFACE_MASK,
                 "%6s ", INTERFACE_MODE(info->interface));
        }
    } else
#endif
    {
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_INTERFACE_MASK,
                 "%6s ", INTERFACE_MODE(info->interface));
    }
    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        cli_out("%5d ", info->frame_max);
    } else {
        cli_out("%5s ", "");
    }
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        ret = bcm_switch_control_port_get(unit, port,
                                          bcmSwitchAlternateStoreForward,
                                          &asf_mode);
        if (BCM_E_UNAVAIL == ret) {
            asf_mode = 0;
        }

        if ((asf_mode >= 0) && (asf_mode <= 3)) {
            cli_out("%5s ", asf_str[asf_mode]);
        } else {
            cli_out("%5s ", "     ");
        }
    } else {
        cli_out("%5s ", "     ");
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)){
        ret = portmod_port_diag_info_get(unit, port, &diag_info);
        if (ret) return ret;
        /* _SHR_PORT_PHY_FEC_INVALID is 0 in enum _shr_port_phy_fec_e */
        cli_out("%10s ", fec_list[diag_info.fec - 1]);
    }
#endif
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LOOPBACK_MASK,
                 "%5s",
                 info->loopback != BCM_PORT_LOOPBACK_NONE ?
                 LOOPBACK_MODE(info->loopback) : "     ");

    cli_out("\n");
    return ret;
}
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *    if_port_policer
 * Purpose:
 *    Table display of port information
 * Parameters:
 *    unit - SOC unit #
 *    a - pointer to args
 * Returns:
 *    CMD_OK/CMD_FAIL
 */
cmd_result_t 
if_esw_port_policer(int unit, args_t *a)
{
    pbmp_t pbm;
    char *subcmd, *argpbm, *argpid;
    bcm_policer_t pid = 0;
    bcm_port_config_t pcfg;
    soc_port_t port, dport;
    int rv;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        subcmd = "Get";
    }

    if ((argpbm = ARG_GET(a)) == NULL) {
        pbm = pcfg.port;
    } else {
        if (parse_bcm_pbmp(unit, argpbm, &pbm) < 0) {
            cli_out("%s: ERROR: unrecognized port bitmap: %s\n",
                    ARG_CMD(a), argpbm);
            return CMD_FAIL;
        }

        BCM_PBMP_AND(pbm, pcfg.port);
    }

    if (sal_strcasecmp(subcmd, "Get") == 0) {
        rv = BCM_E_NONE;
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
            if ((rv = bcm_port_policer_get(unit, port, &pid)) < 0) {
                cli_out("Error retrieving info for port %s: %s\n",
                        BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                break;
            }

            cli_out("Port %s policer id is %d\n",
                    BCM_PORT_NAME(unit, port), pid);
        }
        return (rv < 0) ? CMD_FAIL : CMD_OK;
    } else if (sal_strcasecmp(subcmd, "Set") == 0) {
        if ((argpid = ARG_GET(a)) == NULL) {
            cli_out("Missing PID for set.\n");
            return CMD_USAGE;
        }
        pid = sal_ctoi(argpid, 0);
    } else {
        return CMD_USAGE;
    }

    /* Set Policer id as indicated */

    rv = BCM_E_NONE;

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
        if ((rv = bcm_port_policer_set(unit, port, pid)) < 0) {
            cli_out("Error setting port %s default PID to %d: %s\n",
                    BCM_PORT_NAME(unit, port), pid, bcm_errmsg(rv));

            if ((rv == BCM_E_NOT_FOUND) || (rv == BCM_E_CONFIG)) {
                cli_out("Error in setting PID %x to port \n", pid);
            }
            break;
        }
    }
    return CMD_OK;
}
#endif                          /* BCM_KATANA_SUPPORT or BCM_TRIUMPH3_SUPPORT */
/*
 * Function:
 *    if_port_stat
 * Purpose:
 *    Table display of port information
 * Parameters:
 *    unit - SOC unit #
 *    a - pointer to args
 * Returns:
 *    CMD_OK/CMD_FAIL
 */
cmd_result_t 
if_esw_port_stat(int unit, args_t *a)
{
    pbmp_t pbm, tmp_pbm;
    bcm_port_info_t *info_all;
    bcm_port_config_t pcfg;
    soc_port_t p, dport;
    int r = CMD_OK;
    int temp_r = CMD_OK;
    char *c;
    uint32 mask;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }
    BCM_PBMP_AND(pbm, pcfg.port);
    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("No ports specified.\n");
        return CMD_OK;
    }

    mask = BCM_PORT_ATTR_ALL_MASK;

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t),
                         "if_port_stat");
    if (info_all == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        port_info_init(unit, p, &info_all[p], mask);
        if ((temp_r = bcm_port_selective_get(unit, p, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not get port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(temp_r));
            r |= temp_r;
            continue;
        }
#ifdef BCM_TOMAHAWK_SUPPORT
        if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
            /* reusing phy_master to display num of lanes */
            /* coverity[overrun-local] */
            info_all[p].phy_master = SOC_INFO(unit).port_num_lanes[p];
        }
#endif
    }

    brief_port_info_header(unit);

#if defined(BCM_TOMAHAWK_SUPPORT)
#define _call_bpi(pbm, pbm_mask) \
    tmp_pbm = pbm_mask; \
    BCM_PBMP_AND(tmp_pbm, pbm); \
    DPORT_BCM_PBMP_ITER(unit, tmp_pbm, dport, p) { \
        esw_brief_port_info(unit, p, &info_all[p], mask); \
    }
#else
#define _call_bpi(pbm, pbm_mask) \
    tmp_pbm = pbm_mask; \
    BCM_PBMP_AND(tmp_pbm, pbm); \
    DPORT_BCM_PBMP_ITER(unit, tmp_pbm, dport, p) { \
        brief_port_info(unit, p, &info_all[p], mask); \
    }
#endif

    if (soc_property_get(unit, spn_DPORT_MAP_ENABLE, TRUE)) {
        /* If port mapping is enabled, then use port order */
        _call_bpi(pbm, pcfg.port);
    } else {
        /* If no port mapping, ensure that ports are grouped by type */
        _call_bpi(pbm, pcfg.fe);
        _call_bpi(pbm, pcfg.ge);
        _call_bpi(pbm, pcfg.xe);
        _call_bpi(pbm, pcfg.ce);
        _call_bpi(pbm, pcfg.hg);
    }

    sal_free(info_all);

    return r;
}

/*
 * Function:
 *    if_port_rate
 * Purpose:
 *    Set/display of port rate metering characteristics
 * Parameters:
 *    unit - SOC unit #
 *    a - pointer to args
 * Returns:
 *    CMD_OK/CMD_FAIL
 */
cmd_result_t 
if_esw_port_rate(int unit, args_t *a)
{
    pbmp_t pbm,tmp_pbm;
    bcm_port_config_t pcfg;
    soc_port_t p, dport;
    int operation = 0;
    int rv;
    int header;
    uint32 rate = 0xFFFFFFFF;
    uint32 burst = 0xFFFFFFFF;
    char *c;

#define SHOW    1
#define INGRESS 2
#define EGRESS  4
#define PAUSE   8

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    /* Check for metering capabilities */
    if (!soc_feature(unit, soc_feature_ingress_metering) &&
        !soc_feature(unit, soc_feature_egress_metering)) {
        cli_out("%s: Error: metering unavailable for this device\n", ARG_CMD(a));
        return CMD_OK;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }


    if ((c = ARG_GET(a)) == NULL) {
        /* Default with only Ethernet ports */
        BCM_PBMP_ASSIGN(pbm, pcfg.e);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }


    /* Check if there is HG ports and show warning message */
    BCM_PBMP_ASSIGN(tmp_pbm, pbm);
    BCM_PBMP_AND(tmp_pbm, pcfg.hg);
    if (BCM_PBMP_NOT_NULL(tmp_pbm)) {
        cli_out("Warning: Not suggested to control rate on HG ports\n");
    }

    /* Apply PortRate only to those ports which support it */
    BCM_PBMP_AND(pbm, pcfg.port);
    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("Warning: No valid ports specified.\n");
        return CMD_OK;
    }


    /* Ingress, egress or show both */
    if ((c = ARG_GET(a)) == NULL) {
        operation = SHOW;

        /* Show Ethernet ports and other ports (i.e. HG ...) */
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
        if (soc_feature(unit, soc_feature_ingress_metering)) {
            operation |= INGRESS;
        }
        if (soc_feature(unit, soc_feature_egress_metering)) {
            operation |= EGRESS;
        }
    } else if (!sal_strncasecmp(c, "ingress", sal_strlen(c))) {
        if (soc_feature(unit, soc_feature_ingress_metering)) {
            operation = INGRESS;
        } else {
            cli_out("%s: Error: ingress metering unavailable for "
                    "this device\n", ARG_CMD(a));
            return CMD_OK;
        }
    } else if (!sal_strncasecmp(c, "egress", sal_strlen(c))) {
        if (soc_feature(unit, soc_feature_egress_metering)) {
            operation = EGRESS;
        } else {
            cli_out("%s: Error: egress metering unavailable for "
                    "this device\n", ARG_CMD(a));
            return CMD_OK;
        }
    } else if (SOC_IS_TUCANA(unit) &&
               !sal_strncasecmp(c, "pause", sal_strlen(c))) {
        operation = PAUSE;
    } else {
        cli_out("%s: Error: unrecognized port rate type: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    /* Set or get */
    if ((c = ARG_GET(a)) != NULL) {
        rate = parse_integer(c);
        if ((c = ARG_GET(a)) != NULL) {
            burst = parse_integer(c);
        } else {
            cli_out("%s: Error: missing port burst size\n", ARG_CMD(a));
            return CMD_FAIL;
        }
    } else {
        operation |= SHOW;
    }

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        if (operation & SHOW) {
            /* Display current setting */
            header = 0;
            if (operation & (INGRESS | PAUSE)) {
                rv = bcm_port_rate_ingress_get(unit, p, &rate, &burst);
                if (rv < 0) {
                    cli_out("%s port %s: ERROR: bcm_port_rate_ingress_get: "
                            "%s\n",
                            ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                if (rate) {
                    cli_out("%4s:", BCM_PORT_NAME(unit, p));
                    header = 1;
                    if (rate < 64) {
                        cli_out("\tIngress meter: ? kbps ? kbits max burst.\n");
                    } else {
                        cli_out("\tIngress meter: "
                                "%8d kbps %8d kbits max burst.\n", rate, burst);
                    }
                    if (SOC_IS_TUCANA(unit)) {
                        rv = bcm_port_rate_pause_get(unit, p, &rate, &burst);
                        if (rv < 0) {
                            cli_out("%s port %s: ERROR: "
                                    "bcm_port_rate_pause_get: %s\n",
                                    ARG_CMD(a), BCM_PORT_NAME(unit, p),
                                    bcm_errmsg(rv));
                            return CMD_FAIL;
                        }
                        if (rate) {
                            cli_out("\tPause frames: Pause = %8d kbits, "
                                    "Resume = %8d kbits.\n", rate, burst);
                        }
                    }
                }
            }
            if (operation & EGRESS) {
                rv = bcm_port_rate_egress_get(unit, p, &rate, &burst);
                if (rv < 0) {
                    cli_out("%s port %s: ERROR: bcm_port_rate_egress_get: %s\n",
                            ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                if (rate) {
                    if (!header)
                        cli_out("%4s:", BCM_PORT_NAME(unit, p));
                    cli_out("\tEgress meter:  %8d kbps %8d kbits max burst.\n",
                            rate, burst);
                }
            }
        } else {
            /* New setting */
            if (!rate || !burst)
                rate = burst = 0;       /* Disable port metering */
            if (operation & INGRESS) {
                rv = bcm_port_rate_ingress_set(unit, p, rate, burst);
                if (rv < 0) {
                    cli_out("%s: ERROR: bcm_port_rate_ingress_set: %s\n",
                            ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            } else if (operation & PAUSE) {
                rv = bcm_port_rate_pause_set(unit, p, rate, burst);
                if (rv < 0) {
                    cli_out("%s: ERROR: bcm_port_rate_pause_set: %s\n",
                            ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            } else if (operation & EGRESS) {
                rv = bcm_port_rate_egress_set(unit, p, rate, burst);
                if (rv < 0) {
                    cli_out("%s: ERROR: bcm_port_rate_egress_set: %s\n",
                            ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }
    }
#undef SHOW
#undef INGRESS
#undef EGRESS
#undef PAUSE

    return CMD_OK;
}

/*
 * Function:
 *      if_port_samp_rate
 * Purpose:
 *      Set/display of sflow port sampling rates.
 * Parameters:
 *      unit - SOC unit #
 *      args - pointer to comand line arguments
 * Returns:
 *      CMD_OK/CMD_FAIL
 */
char if_port_samp_rate_usage[] =
    "Set/Display port sampling rate characteristics.\n"
    "Parameters: <pbm> [ingress_rate] [egress_rate]\n"
    "\tOn average, every 1/ingress_rate packets will be sampled.\n"
    "\tA rate of 0 indicates no sampling.\n"
    "\tA rate of 1 indicates all packets sampled.\n";

cmd_result_t 
if_port_samp_rate(int unit, args_t *args)
{
#define SHOW    0x01
#define SET     0x02
    pbmp_t pbm;
    bcm_port_config_t pcfg;
    char *ch;
    int operation = SET;        /* Set or Show */
    int ingress_rate = -1;
    int egress_rate = -1;
    soc_port_t soc_port, dport;
    int retval;

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(args));
        return CMD_FAIL;
    }

    /* get port bitmap */
    if ((ch = ARG_GET(args)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, ch, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(args), ch);
        return CMD_FAIL;
    }

    /* read in ingress_rate and egress_rate if given */
    if ((ch = ARG_GET(args)) != NULL) {
        ingress_rate = parse_integer(ch);
        if ((ch = ARG_GET(args)) != NULL) {
            egress_rate = parse_integer(ch);
        } else {
            cli_out("%s: Error: missing egress rate \n", ARG_CMD(args));
            return CMD_FAIL;
        }
    } else {
        operation = SHOW;
    }

    /* Iterate through port bitmap and perform 'operation' on them. */
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, soc_port) {
        if (operation == SHOW) {
            /* Show port sflow sample rate(s) */
            retval = bcm_port_sample_rate_get(unit, soc_port, &ingress_rate,
                                              &egress_rate);
            if (retval != BCM_E_NONE) {
                cli_out("%s port %s: ERROR: bcm_port_sample_rate_get: "
                        "%s\n", ARG_CMD(args),
                        BCM_PORT_NAME(unit, soc_port), bcm_errmsg(retval));
                return CMD_FAIL;
            }

            cli_out("%4s:", BCM_PORT_NAME(unit, soc_port));

            if (ingress_rate == 0) {
                cli_out("\tingress: not sampling,");
            } else {
                cli_out("\tingress: 1 out of %d packets,", ingress_rate);
            }
            if (egress_rate == 0) {
                cli_out("\tegress: not sampling,");
            } else {
                cli_out("\tegress: 1 out of %d packets,", egress_rate);
            }
            cli_out("\n");
        } else {
            /* Set port sflow sample rate(s) */
            retval = bcm_port_sample_rate_set(unit, soc_port, ingress_rate,
                                              egress_rate);
            if (retval != BCM_E_NONE) {
                cli_out("%s port %s: ERROR: bcm_port_sample_rate_set: "
                        "%s\n", ARG_CMD(args),
                        BCM_PORT_NAME(unit, soc_port), bcm_errmsg(retval));
                return CMD_FAIL;
            }
        }
    }

#undef SHOW
#undef SET
    return CMD_OK;
}

/*
 * Function:
 *    disp_port_info
 * Purpose:
 *    Display selected port information
 * Parameters:
 *    info        - pointer to structure with info to display
 *    port_ref    - Port reference to print
 *    st_port     - Is the port a hi-gig port?
 * Returns:     
 *    Nothing
 * Notes:
 *    Assumes link status info always valid
 */

void
disp_port_info(int unit, char *port_ref, bcm_port_t port, bcm_port_info_t *info,
               int st_port, uint32 flags)
{
    int r;
    int no_an_props = 0;        /* Do not show AN props */
    bcm_port_ability_t *local = &info->port_ability;
    bcm_port_ability_t *remote = &info->remote_ability;
    bcm_port_ability_t *advert = &info->local_ability;
#if defined(BCM_ESW_SUPPORT)
    _bcm_port_info_t *port_info;
#endif

    /* Assume link status always available. */
    cli_out(" %c%-7s ", info->linkstatus ? '*' : ' ', port_ref);

    if (info->linkstatus == BCM_PORT_LINK_STATUS_FAILED) {
        cli_out("%s", "FAILED ");
    }

    if (flags & BCM_PORT_ATTR_ENABLE_MASK) {
        cli_out("%s", info->enable ? "(Enabled" : "(Disabled ");
    }

    if (flags & BCM_PORT_ATTR_LINKSCAN_MASK) {
        if (info->linkscan) {
            cli_out("LS(%s ", LINKSCAN_MODE(info->linkscan));
        }
    }
    if (st_port) {
        /*
         * Coverity
         * coverity[result_independent_of_operands]
         */
        cli_out("%s(", ENCAP_MODE(info->encap_mode));
    } else if (flags & BCM_PORT_ATTR_AUTONEG_MASK) {
        if (info->autoneg) {
            if (info->linkstatus != BCM_PORT_LINK_STATUS_UP) {
                cli_out("Auto(no link) ");
                no_an_props = 1;
            } else {
                cli_out("Auto(");
            }
        } else {
            cli_out("Forced(");
        }
    }

    /* If AN is enabled, but not complete, don't show port settings */
    if (!no_an_props) {
        if (flags & BCM_PORT_ATTR_SPEED_MASK) {
            char buf[32];
            cli_out("(%s", if_fmt_speed(buf, info->speed));
        }
        if (flags & BCM_PORT_ATTR_DUPLEX_MASK) {
            cli_out("%s", info->speed == 0 ? "" : info->duplex ? "FD" : "HD");
        }
        if (flags & BCM_PORT_ATTR_PAUSE_MASK) {
            if (info->pause_tx && info->pause_rx) {
                cli_out(",pause");
            } else if (info->pause_tx) {
                cli_out(",pause_tx");
            } else if (info->pause_rx) {
                cli_out(",pause_rx");
            }
        }
        cli_out(") ");
    }

    if (flags & BCM_PORT_ATTR_AUTONEG_MASK) {
        if (info->autoneg) {
            char buf[80];

            if (flags & BCM_PORT_ATTR_ABILITY_MASK) {
                format_port_speed_ability(buf, sizeof(buf),
                                          local->speed_full_duplex);
                cli_out("Ability (fd = %s ", buf);
                format_port_speed_ability(buf, sizeof(buf),
                                          local->speed_half_duplex);
                cli_out("hd = %s ", buf);
                format_port_intf_ability(buf, sizeof(buf), local->interface);
                cli_out("intf = %s ", buf);
                format_port_medium_ability(buf, sizeof(buf), local->medium);
                cli_out("medium = %s ", buf);
                format_port_pause_ability(buf, sizeof(buf), local->pause);
                cli_out("pause = %s ", buf);
                format_port_lb_ability(buf, sizeof(buf), local->loopback);
                cli_out("lb = %s ", buf);
                format_port_flags_ability(buf, sizeof(buf), local->flags);
                cli_out("flags = %s )", buf);
            }

            if (flags & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) {
                format_port_speed_ability(buf, sizeof(buf),
                                          advert->speed_full_duplex);
                cli_out("Local (fd = %s ", buf);
                format_port_speed_ability(buf, sizeof(buf),
                                          advert->speed_half_duplex);
                cli_out("hd = %s ", buf);
                format_port_intf_ability(buf, sizeof(buf), advert->interface);
                cli_out("intf = %s", buf);
                format_port_medium_ability(buf, sizeof(buf), advert->medium);
                cli_out("medium = %s ", buf);
                format_port_pause_ability(buf, sizeof(buf), advert->pause);
                cli_out("pause = %s ", buf);
                format_port_lb_ability(buf, sizeof(buf), advert->loopback);
                cli_out("lb = %s ", buf);
                format_port_flags_ability(buf, sizeof(buf), advert->flags);
                cli_out("flags = %s )", buf);
            }

            if ((flags & BCM_PORT_ATTR_REMOTE_ADVERT_MASK) &&
                info->remote_advert_valid &&
                (info->linkstatus == BCM_PORT_LINK_STATUS_UP)) {
                format_port_speed_ability(buf, sizeof(buf),
                                          remote->speed_full_duplex);
                cli_out("Remote (fd = %s ", buf);
                format_port_speed_ability(buf, sizeof(buf),
                                          remote->speed_half_duplex);
                cli_out("hd = %s ", buf);
                format_port_intf_ability(buf, sizeof(buf), remote->interface);
                cli_out("intf = %s ", buf);
                format_port_medium_ability(buf, sizeof(buf), remote->medium);
                cli_out("medium = %s ", buf);
                format_port_pause_ability(buf, sizeof(buf), remote->pause);
                cli_out("pause = %s ", buf);
                format_port_lb_ability(buf, sizeof(buf), remote->loopback);
                cli_out("lb = %s ", buf);
                format_port_flags_ability(buf, sizeof(buf), remote->flags);
                cli_out("flags = %s )", buf);
            }
        }
    }

    if (flags & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        if ((info->pause_mac[0] | info->pause_mac[1] |
             info->pause_mac[2] | info->pause_mac[3] |
             info->pause_mac[4] | info->pause_mac[5]) != 0) {
            cli_out("Stad(%02x:%02x:%02x:%02x:%02x:%02x) ",
                    info->pause_mac[0], info->pause_mac[1],
                    info->pause_mac[2], info->pause_mac[3],
                    info->pause_mac[4], info->pause_mac[5]);
        }
    }

    if (flags & BCM_PORT_ATTR_STP_STATE_MASK) {
        cli_out("STP(%s) ", FORWARD_MODE(info->stp_state));
    }

    if (!st_port) {
        if (flags & BCM_PORT_ATTR_DISCARD_MASK) {
            switch (info->discard) {
                case BCM_PORT_DISCARD_NONE:
                    break;
                case BCM_PORT_DISCARD_ALL:
                    cli_out("Disc(all) ");
                    break;
                case BCM_PORT_DISCARD_UNTAG:
                    cli_out("Disc(untagged) ");
                    break;
                case BCM_PORT_DISCARD_TAG:
                    cli_out("Disc(tagged) ");
                    break;
                default:
                    cli_out("Disc(?) ");
                    break;
            }
        }

        if (flags & BCM_PORT_ATTR_LEARN_MASK) {
            cli_out("Lrn(");

            r = 0;

            if (info->learn & BCM_PORT_LEARN_ARL) {
                cli_out("ARL");
                r = 1;
            }

            if (info->learn & BCM_PORT_LEARN_CPU) {
                cli_out("%sCPU", r ? "," : "");
                r = 1;
            }

            if (info->learn & BCM_PORT_LEARN_FWD) {
                cli_out("%sFWD", r ? "," : "");
                r = 1;
            }

            if (!r) {
                cli_out("disc");
            }
            cli_out(") ");
        }

        if (flags & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
            cli_out("UtPri(");

            if (info->untagged_priority < 0) {
                cli_out("off");
            } else {
                cli_out("%d", info->untagged_priority);
            }
            cli_out(") ");
        }

        if (flags & BCM_PORT_ATTR_PFM_MASK) {
            cli_out("Pfm(%s) ", MCAST_FLOOD(info->pfm));
        }
    } /* !st_port */
    if (flags & BCM_PORT_ATTR_INTERFACE_MASK) {
        cli_out("IF(%s) ", INTERFACE_MODE(info->interface));
    }

    if (flags & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        if (info->phy_master >= 0 &&
            info->phy_master < SOC_PORT_MS_COUNT &&
            info->phy_master != SOC_PORT_MS_NONE) {
            cli_out("PH(%s) ", PHYMASTER_MODE(info->phy_master));
        }
    }

    if (flags & BCM_PORT_ATTR_LOOPBACK_MASK) {
        if (info->loopback == BCM_PORT_LOOPBACK_PHY) {
            cli_out("LB(PHY) ");
        } else if (info->loopback == BCM_PORT_LOOPBACK_MAC) {
            cli_out("LB(MAC) ");
        }
    }

    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        cli_out("Max_frame(%d) ", info->frame_max);
    }

    if ((flags & BCM_PORT_ATTR_MDIX_MASK) &&
        (0 <= info->mdix) && (info->mdix < BCM_PORT_MDIX_COUNT)) {
        cli_out("MDIX(%s", MDIX_MODE(info->mdix));

        if ((flags & BCM_PORT_ATTR_MDIX_STATUS_MASK) &&
            (0 <= info->mdix_status) &&
            (info->mdix_status < BCM_PORT_MDIX_STATUS_COUNT)) {
            cli_out(", %s", MDIX_STATUS(info->mdix_status));
        }

        cli_out(") ");
    }

    if ((flags & BCM_PORT_ATTR_MEDIUM_MASK) &&
        (0 <= info->medium) && (info->medium < BCM_PORT_MEDIUM_COUNT)) {
        cli_out("Medium(%s) ", MEDIUM_STATUS(info->medium));
    }

    if ((flags & BCM_PORT_ATTR_FAULT_MASK) && (info->fault)) {
        cli_out("Fault(%s%s) ",
                (info->fault & BCM_PORT_FAULT_LOCAL) ? "Local" : "",
                (info->fault & BCM_PORT_FAULT_REMOTE) ? "Remote" : "");
    }

    if ((flags & BCM_PORT_ATTR_VLANFILTER_MASK) && (info->vlanfilter > 0)) {
        cli_out("VLANFILTER(%d) ", info->vlanfilter);
    }

#if defined(BCM_ESW_SUPPORT)
    if (!SOC_PORT_USE_PORTCTRL(unit, port)) {
        _bcm_port_info_access(unit, port, &port_info);
        if (port_info != NULL) {
            cli_out("Mac driver(%s)", port_info->p_mac->drv_name);
        } else {
            cli_out("Mac driver(%s)", "NULL");
        }
    }
#endif

    cli_out("\n");
}

/* This maps the above list to the masks for the proper attributes
 * Note that the order of this attribute map should match that of
 * the parse-table entry/creation below.
 */
static int port_attr_map[] = {
    BCM_PORT_ATTR_ENABLE_MASK,  /* Enable */
    BCM_PORT_ATTR_AUTONEG_MASK, /* AutoNeg */
    BCM_PORT_ATTR_LOCAL_ADVERT_MASK,    /* ADVert */
    BCM_PORT_ATTR_SPEED_MASK,   /* SPeed */
    BCM_PORT_ATTR_DUPLEX_MASK,  /* FullDuplex */
    BCM_PORT_ATTR_LINKSCAN_MASK,        /* LinkScan */
    BCM_PORT_ATTR_LEARN_MASK,   /* LeaRN */
    BCM_PORT_ATTR_DISCARD_MASK, /* DISCard */
    BCM_PORT_ATTR_VLANFILTER_MASK,      /* VlanFilter */
    BCM_PORT_ATTR_UNTAG_PRI_MASK,       /* PRIOrity */
    BCM_PORT_ATTR_PFM_MASK,     /* PortFilterMode */
    BCM_PORT_ATTR_PHY_MASTER_MASK,      /* PHymaster */
    BCM_PORT_ATTR_INTERFACE_MASK,       /* InterFace */
    BCM_PORT_ATTR_LOOPBACK_MASK,        /* LoopBack */
    BCM_PORT_ATTR_STP_STATE_MASK,       /* SpanningTreeProtocol */
    BCM_PORT_ATTR_PAUSE_MAC_MASK,       /* STationADdress */
    BCM_PORT_ATTR_PAUSE_TX_MASK,        /* TxPAUse */
    BCM_PORT_ATTR_PAUSE_RX_MASK,        /* RxPAUse */
    BCM_PORT_ATTR_ENCAP_MASK,   /* Port encapsulation mode */
    BCM_PORT_ATTR_FRAME_MAX_MASK,       /* Max receive frame size */
    BCM_PORT_ATTR_MDIX_MASK,    /* MDIX mode */
    BCM_PORT_ATTR_MEDIUM_MASK,  /* port MEDIUM */
};

/*
 * Function:
 *    port_parse_setup
 * Purpose:
 *    Setup the parse table for a port command
 * Parameters:
 *    pt    - the table
 *    info  - port info structure to hold parse results
 * Returns:
 *    Nothing
 */

void
port_parse_setup(int unit, parse_table_t *pt, bcm_port_info_t *info)
{
    int i;

    /*
     * NOTE: ENTRIES IN THIS TABLE ARE POSITION-DEPENDENT!
     * See references to PQ_PARSED below.
     */
    parse_table_init(unit, pt);
    parse_table_add(pt, "Enable", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->enable, 0);
    parse_table_add(pt, "AutoNeg", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->autoneg, 0);
    if (info->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
        parse_table_add(pt, "ADVert", PQ_PORTABIL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &info->local_ability, 0);
    } else {
        parse_table_add(pt, "ADVert", PQ_PORTMODE | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &info->local_advert, 0);
    }
    parse_table_add(pt, "SPeed", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->speed, 0);
    parse_table_add(pt, "FullDuplex", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->duplex, 0);
    parse_table_add(pt, "LinkScan", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->linkscan, linkscan_mode);
    parse_table_add(pt, "LeaRN", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->learn, 0);
    parse_table_add(pt, "DISCard", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->discard, discard_mode);
    parse_table_add(pt, "VlanFilter", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->vlanfilter, 0);
    parse_table_add(pt, "PRIOrity", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->untagged_priority, 0);
    parse_table_add(pt, "PortFilterMode", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pfm, 0);
    parse_table_add(pt, "PHymaster", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->phy_master, phymaster_mode);
    parse_table_add(pt, "InterFace", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->interface, interface_mode);
    parse_table_add(pt, "LoopBack", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->loopback, loopback_mode);
    parse_table_add(pt, "SpanningTreeProtocol",
                    PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->stp_state, forward_mode);
    parse_table_add(pt, "STationADdress", PQ_MAC | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pause_mac, 0);
    parse_table_add(pt, "TxPAUse", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pause_tx, 0);
    parse_table_add(pt, "RxPAUse", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pause_rx, 0);
    parse_table_add(pt, "ENCapsulation", PQ_MULTI | PQ_DFL,
                    0, &info->encap_mode, encap_mode);
    parse_table_add(pt, "FrameMax", PQ_INT | PQ_DFL, 0, &info->frame_max, 0);
    parse_table_add(pt, "MDIX", PQ_MULTI | PQ_DFL, 0, &info->mdix, mdix_mode);
    parse_table_add(pt, "Medium", PQ_MULTI | PQ_DFL,
                    0, &info->medium, medium_status);

    if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
        for (i = 0; i < pt->pt_cnt; i++) {
            if (~_BCM_DPP_PORT_ATTRS & port_attr_map[i]) {
                pt->pt_entries[i].pq_type |= PQ_IGNORE;
            }
        }
#endif
    } else if (SOC_IS_DFE(unit)) {
#ifdef BCM_DFE_SUPPORT
        for (i = 0; i < pt->pt_cnt; i++) {
            if (~_BCM_DFE_PORT_ATTRS & port_attr_map[i]) {
                pt->pt_entries[i].pq_type |= PQ_IGNORE;
            }
        }
#endif
    } else if (SOC_IS_XGS12_FABRIC(unit)) {
        /* For Hercules, ignore some StrataSwitch attributes */
        for (i = 0; i < pt->pt_cnt; i++) {
            if (~BCM_PORT_HERC_ATTRS & port_attr_map[i]) {
                pt->pt_entries[i].pq_type |= PQ_IGNORE;
            }
        }
    } else if (!SOC_IS_XGS(unit)) {
        /* For all non-XGS chips, ignore special XGS attributes */
        for (i = 0; i < pt->pt_cnt; i++) {
            if (BCM_PORT_XGS_ATTRS & port_attr_map[i]) {
                pt->pt_entries[i].pq_type |= PQ_IGNORE;
            }
        }
    }
}

/*
 * Function:
 *     port_parse_mask_get
 * Purpose:
 *     Given PT has been parsed, set seen and parsed flags
 * Parameters:
 *    pt    - the table
 *    seen  - which parameters occurred w/o =
 *    parsed- which parameters occurred w =
 * Returns:
 *    Nothing
 */

void
port_parse_mask_get(parse_table_t *pt, uint32 *seen, uint32 *parsed)
{
    uint32 were_parsed = 0;
    uint32 were_seen = 0;
    int i;

    /* Check that either all parameters are parsed or are seen (no =) */

    for (i = 0; i < pt->pt_cnt; i++) {
        if (pt->pt_entries[i].pq_type & PQ_SEEN) {
            were_seen |= port_attr_map[i];
        }

        if (pt->pt_entries[i].pq_type & PQ_PARSED) {
            were_parsed |= port_attr_map[i];
        }
    }

    *seen = were_seen;
    *parsed = were_parsed;
}

/* Invalid unit number ( < 0) is permitted */
void
port_info_init(int unit, int port, bcm_port_info_t *info, uint32 actions)
{
    bcm_port_info_t_init(info);

    info->action_mask = actions;

    /* We generally need to get link state */
    info->action_mask |= BCM_PORT_ATTR_LINKSTAT_MASK;

    /* Add the autoneg and advert masks if any of actions is possibly 
     * related to the autoneg. 
     */
    if (actions & (BCM_PORT_AN_ATTRS |
                   BCM_PORT_ATTR_AUTONEG_MASK |
                   BCM_PORT_ATTR_LOCAL_ADVERT_MASK)) {
        info->action_mask |= BCM_PORT_ATTR_LOCAL_ADVERT_MASK;
        info->action_mask |= BCM_PORT_ATTR_REMOTE_ADVERT_MASK;
        info->action_mask |= BCM_PORT_ATTR_AUTONEG_MASK;
    }

    /* if forced speed mode, speed_set should be called */
    if ((info->action_mask & BCM_PORT_ATTR_AUTONEG_MASK) &&
        (info->autoneg == 0)) {
        info->action_mask |= BCM_PORT_ATTR_SPEED_MASK;
    }

    if (unit >= 0) {
        
        if (unit >= 0 && SOC_IS_XGS12_FABRIC(unit)) {
            info->action_mask |= BCM_PORT_ATTR_ENCAP_MASK;
        }

        /* Clear rate for HG/HL ports */
        if (IS_ST_PORT(unit, port)) {
            info->action_mask &= ~(BCM_PORT_ATTR_RATE_MCAST_MASK |
                                   BCM_PORT_ATTR_RATE_BCAST_MASK |
                                   BCM_PORT_ATTR_RATE_DLFBC_MASK);
        }

        if (SOC_IS_SC_CQ(unit) || SOC_IS_TRIUMPH2(unit) ||
            SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit) ||
            SOC_IS_TD_TT(unit) || SOC_IS_HAWKEYE(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_TRIUMPH3(unit) ||
            SOC_IS_HURRICANE3(unit) || SOC_IS_JERICHO(unit) ||
            SOC_IS_KATANA2(unit) || SOC_IS_GREYHOUND2(unit)) {
            info->action_mask2 |= BCM_PORT_ATTR2_PORT_ABILITY;
        }
    }
}

/* Given a maximum speed, return the mask of bcm_port_ability_t speeds
 * while are less than or equal to the given speed. */
bcm_port_abil_t
port_speed_max_mask(bcm_port_abil_t max_speed)
{
    bcm_port_abil_t speed_mask = 0;
    /* This is a giant fall through switch */
    switch (max_speed) {
        case 127000:
            speed_mask |= BCM_PORT_ABILITY_127GB;
        case 120000:
            speed_mask |= BCM_PORT_ABILITY_120GB;
        case 106000:
            speed_mask |= BCM_PORT_ABILITY_106GB;
        case 100000:
            speed_mask |= BCM_PORT_ABILITY_100GB;
        case 53000:
            speed_mask |= BCM_PORT_ABILITY_53GB;
        case 50000:
            speed_mask |= BCM_PORT_ABILITY_50GB;
        case 48000:
            speed_mask |= BCM_PORT_ABILITY_48GB;
        case 42000:
            speed_mask |= BCM_PORT_ABILITY_42GB;
        case 40000:
            speed_mask |= BCM_PORT_ABILITY_40GB;
        case 32000:
            speed_mask |= BCM_PORT_ABILITY_32GB;
        case 30000:
            speed_mask |= BCM_PORT_ABILITY_30GB;
        case 25000:
            speed_mask |= BCM_PORT_ABILITY_25GB;
        case 24000:
            speed_mask |= BCM_PORT_ABILITY_24GB;
        case 21000:
            speed_mask |= BCM_PORT_ABILITY_21GB;
        case 20000:
            speed_mask |= BCM_PORT_ABILITY_20GB;
        case 16000:
            speed_mask |= BCM_PORT_ABILITY_16GB;
        case 15000:
            speed_mask |= BCM_PORT_ABILITY_15GB;
        case 13000:
            speed_mask |= BCM_PORT_ABILITY_13GB;
        case 12500:
            speed_mask |= BCM_PORT_ABILITY_12P5GB;
        case 12000:
            speed_mask |= BCM_PORT_ABILITY_12GB;
        case 10000:
            speed_mask |= BCM_PORT_ABILITY_10GB;
        case 6000:
            speed_mask |= BCM_PORT_ABILITY_6000MB;
        case 5000:
            speed_mask |= BCM_PORT_ABILITY_5000MB;
        case 3000:
            speed_mask |= BCM_PORT_ABILITY_3000MB;
        case 2500:
            speed_mask |= BCM_PORT_ABILITY_2500MB;
        case 1000:
            speed_mask |= BCM_PORT_ABILITY_1000MB;
        case 100:
            speed_mask |= BCM_PORT_ABILITY_100MB;
        case 10:
            speed_mask |= BCM_PORT_ABILITY_10MB;
        default:
            break;
    }
    return speed_mask;
}

/*
 * Function:
 *    port_parse_port_info_set
 * Purpose:
 *    Set/change values in a destination according to parsing
 * Parameters:
 *    flags    - What values to change
 *    src      - Where to get info from
 *    dest     - Where to put info
 * Returns:
 *    -1 on error.  0 on success
 * Notes:
 *    The speed_max and abilities values must be
 *      set in the src port info structure before this is called.
 *
 *      Assumes linkstat and autoneg are valid in the dest structure
 *      If autoneg is specified in flags, assumes local advert
 *      is valid in the dest structure.
 */

int
port_parse_port_info_set(uint32 flags,
                         bcm_port_info_t *src, bcm_port_info_t *dest)
{
    int info_speed_adj;

    if (flags & BCM_PORT_ATTR_AUTONEG_MASK) {
        dest->autoneg = src->autoneg;
    }

    if (flags & BCM_PORT_ATTR_ENABLE_MASK) {
        dest->enable = src->enable;
    }

    if (flags & BCM_PORT_ATTR_STP_STATE_MASK) {
        dest->stp_state = src->stp_state;
    }

    /*
     * info_speed_adj is the same as src->speed except a speed of 0
     * is replaced by the maximum speed supported by the port.
     */

    info_speed_adj = src->speed;

    if ((flags & BCM_PORT_ATTR_SPEED_MASK) && (info_speed_adj == 0)) {
        info_speed_adj = src->speed_max;
    }

    /*
     * If local_advert was parsed, use it.  Otherwise, calculate a
     * reasonable local advertisement from the given values and current
     * values of speed/duplex.
     */

    if ((flags & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) != 0) {
        if (dest->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
            /* Copy source advert info to destination, converting the 
             * format in the process */
            BCM_IF_ERROR_RETURN
                (soc_port_mode_to_ability(src->local_advert,
                                          &(dest->local_ability)));

            /* in case the PQ_PORTABIL is used for parsing */
            if (!src->local_advert) {
                dest->local_ability = src->local_ability;
            }
        } else {
            dest->local_advert = src->local_advert;
        }
    } else if (dest->autoneg) {
        int cur_speed, cur_duplex;
        int cur_pause_tx, cur_pause_rx;
        int new_speed, new_duplex;
        int new_pause_tx, new_pause_rx;
        bcm_port_abil_t mode;

        /*
         * Update link advertisements for speed/duplex/pause.  All
         * speeds less than or equal to the requested speed are
         * advertised.
         */

        if (dest->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
            bcm_port_ability_t *dab, *sab;

            dab = &(dest->local_ability);
            sab = &(src->port_ability);

            cur_speed =
                BCM_PORT_ABILITY_SPEED_MAX(dab->speed_full_duplex |
                                           dab->speed_half_duplex);
            cur_duplex = (dab->speed_full_duplex ?
                          SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF);
            cur_pause_tx = (dab->pause & BCM_PORT_ABILITY_PAUSE_TX) != 0;
            cur_pause_rx = (dab->pause & BCM_PORT_ABILITY_PAUSE_RX) != 0;

            new_speed = (flags & BCM_PORT_ATTR_SPEED_MASK ?
                         info_speed_adj : cur_speed);
            new_duplex = (flags & BCM_PORT_ATTR_DUPLEX_MASK ?
                          src->duplex : cur_duplex);
            new_pause_tx = (flags & BCM_PORT_ATTR_PAUSE_TX_MASK ?
                            src->pause_tx : cur_pause_tx);
            new_pause_rx = (flags & BCM_PORT_ATTR_PAUSE_RX_MASK ?
                            src->pause_rx : cur_pause_rx);

            if (new_duplex != SOC_PORT_DUPLEX_HALF) {
                mode = sab->speed_full_duplex;
                mode &= port_speed_max_mask(new_speed);
                dab->speed_full_duplex = mode;
            } else {
                dab->speed_full_duplex = 0;
            }

            mode = sab->speed_half_duplex;
            mode &= port_speed_max_mask(new_speed);
            dab->speed_half_duplex = mode;

            if (!(sab->pause &
                  BCM_PORT_ABILITY_PAUSE_ASYMM) &&
                (new_pause_tx != new_pause_rx)) {
                cli_out("port parse: Error: Asymmetrical pause not available\n");
                return -1;
            }

            if (!new_pause_tx) {
                dab->pause &= ~BCM_PORT_ABILITY_PAUSE_TX;
            } else {
                dab->pause |= BCM_PORT_ABILITY_PAUSE_TX;
            }

            if (!new_pause_rx) {
                dab->pause &= ~BCM_PORT_ABILITY_PAUSE_RX;
            } else {
                dab->pause |= BCM_PORT_ABILITY_PAUSE_RX;
            }
            dab->eee = sab->eee;
        } else {
            mode = dest->local_advert;

            cur_speed = BCM_PORT_ABIL_SPD_MAX(mode);
            cur_duplex = ((mode & BCM_PORT_ABIL_FD) ?
                          SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF);
            cur_pause_tx = (mode & BCM_PORT_ABIL_PAUSE_TX) != 0;
            cur_pause_rx = (mode & BCM_PORT_ABIL_PAUSE_RX) != 0;

            new_speed = (flags & BCM_PORT_ATTR_SPEED_MASK ?
                         info_speed_adj : cur_speed);
            new_duplex = (flags & BCM_PORT_ATTR_DUPLEX_MASK ?
                          src->duplex : cur_duplex);
            new_pause_tx = (flags & BCM_PORT_ATTR_PAUSE_TX_MASK ?
                            src->pause_tx : cur_pause_tx);
            new_pause_rx = (flags & BCM_PORT_ATTR_PAUSE_RX_MASK ?
                            src->pause_rx : cur_pause_rx);

            /* Start with maximum ability and cut down */

            mode = src->ability;

            if (new_duplex == SOC_PORT_DUPLEX_HALF) {
                mode &= ~BCM_PORT_ABIL_FD;
            }

            if (new_speed < 13000) {
                mode &= ~BCM_PORT_ABIL_13GB;
            }

            if (new_speed < 12000) {
                mode &= ~BCM_PORT_ABIL_12GB;
            }

            if (new_speed < 10000) {
                mode &= ~BCM_PORT_ABIL_10GB;
            }

            if (new_speed < 2500) {
                mode &= ~BCM_PORT_ABIL_2500MB;
            }

            if (new_speed < 1000) {
                mode &= ~BCM_PORT_ABIL_1000MB;
            }

            if (new_speed < 100) {
                mode &= ~BCM_PORT_ABIL_100MB;
            }

            if (!(mode & BCM_PORT_ABIL_PAUSE_ASYMM) &&
                (new_pause_tx != new_pause_rx)) {
                cli_out("port parse: Error: Asymmetrical pause not available\n");
                return -1;
            }

            if (!new_pause_tx) {
                mode &= ~BCM_PORT_ABIL_PAUSE_TX;
            }

            if (!new_pause_rx) {
                mode &= ~BCM_PORT_ABIL_PAUSE_RX;
            }

            dest->local_advert = mode;
        }
    } else {
        /* Update forced values for speed/duplex/pause */

        if (flags & BCM_PORT_ATTR_SPEED_MASK) {
            dest->speed = info_speed_adj;
        }

        if (flags & BCM_PORT_ATTR_DUPLEX_MASK) {
            dest->duplex = src->duplex;
        }

        if (flags & BCM_PORT_ATTR_PAUSE_TX_MASK) {
            dest->pause_tx = src->pause_tx;
        }

        if (flags & BCM_PORT_ATTR_PAUSE_RX_MASK) {
            dest->pause_rx = src->pause_rx;
        }
    }

    if (flags & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        sal_memcpy(dest->pause_mac, src->pause_mac, sizeof(sal_mac_addr_t));
    }

    if (flags & BCM_PORT_ATTR_LINKSCAN_MASK) {
        dest->linkscan = src->linkscan;
    }

    if (flags & BCM_PORT_ATTR_LEARN_MASK) {
        dest->learn = src->learn;
    }

    if (flags & BCM_PORT_ATTR_DISCARD_MASK) {
        dest->discard = src->discard;
    }

    if (flags & BCM_PORT_ATTR_VLANFILTER_MASK) {
        dest->vlanfilter = src->vlanfilter;
    }

    if (flags & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
        dest->untagged_priority = src->untagged_priority;
    }

    if (flags & BCM_PORT_ATTR_PFM_MASK) {
        dest->pfm = src->pfm;
    }

    if (flags & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        dest->phy_master = src->phy_master;
    }

    if (flags & BCM_PORT_ATTR_INTERFACE_MASK) {
        dest->interface = src->interface;
    }

    if (flags & BCM_PORT_ATTR_LOOPBACK_MASK) {
        dest->loopback = src->loopback;
    }

    if (flags & BCM_PORT_ATTR_ENCAP_MASK) {
        dest->encap_mode = src->encap_mode;
    }

    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        dest->frame_max = src->frame_max;
    }

    if (flags & BCM_PORT_ATTR_MDIX_MASK) {
        dest->mdix = src->mdix;
    }

    if (flags & BCM_PORT_ATTR_MEDIUM_MASK) {
        dest->medium = src->medium;
    }

    return 0;
}

/* Iterate thru a port bitmap with the given mask; display info */
STATIC int
_port_disp_iter(int unit, pbmp_t pbm, pbmp_t pbm_mask, uint32 seen)
{
    bcm_port_info_t info;
    soc_port_t port, dport;
    int r = CMD_OK;

    BCM_PBMP_AND(pbm, pbm_mask);
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {

        sal_memset(&info, 0, sizeof(bcm_port_info_t));
        port_info_init(unit, port, &info, seen);

        if ((r |= bcm_port_selective_get(unit, port, &info)) < 0) {
            cli_out("Error: Could not get port %s information: %s\n",
                    BCM_PORT_NAME(unit, port), bcm_errmsg(r));
        }

        /* coverity[overrun-local] */
        disp_port_info(unit, BCM_PORT_NAME(unit, port), port, &info,
                       IS_ST_PORT(unit, port), seen);
    }

    return r;
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
static int
_asf_mode_in_range(int unit, int asf_mode)
{
    int in_range = 0;

    if (soc_feature(unit, soc_feature_asf)) {
        if (((asf_mode >= 0) && (asf_mode <= 1))) {
            in_range = 1;
        }
    } else if (soc_feature(unit, soc_feature_asf_multimode)) {
        if (((asf_mode >= 0) && (asf_mode < 4))) {
            in_range = 1;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if ((SOC_IS_TOMAHAWK3(unit)) && (asf_mode >= 2)) {
            in_range = 0;
        }
#endif
    }
    return in_range;
}
#endif

/*
 * Function:
 *    if_port
 * Purpose:
 *    Configure port specific parameters.
 * Parameters:
 *    u    - SOC unit #
 *    a    - pointer to args
 * Returns:
 *    CMD_OK/CMD_FAIL
 */
cmd_result_t
if_esw_port(int u, args_t *a)
{
    pbmp_t pbm;
    bcm_port_config_t pcfg;
    bcm_port_info_t *info_all;
    bcm_port_info_t info_given;
    bcm_port_ability_t *ability_all;    /* Abilities for all ports */
    bcm_port_ability_t ability_port;    /* Ability for current port */
    bcm_port_ability_t ability_given;
    char *c;
    int r, rv = 0, cmd_rv = CMD_OK;
    soc_port_t p, dport;
    parse_table_t pt;
    uint32 seen = 0;
    uint32 parsed = 0, parsed_adj, pa_speed;
    char pfmt[SOC_PBMP_FMT_LEN];
    int eee_tx_idle_time = 0, eee_tx_wake_time = 0, eee_state, i;
    int eee_tx_ev_cnt, eee_tx_dur, eee_rx_ev_cnt, eee_rx_dur;
    char *eee_enable = NULL, *stats = NULL, *str = NULL;
    uint32 flags;
    bcm_phy_config_t    config;

    if (COUNTOF(interface_mode) != BCM_PORT_IF_COUNT) {
        cli_out("Warning: Interface modes out of sync\n");
    }

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(u, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return (CMD_USAGE);
    }

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t),
                         "if_port");

    if (info_all == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    ability_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t),
                            "if_port");

    if (ability_all == NULL) {
        cli_out("Insufficient memory.\n");
        sal_free(info_all);
        return CMD_FAIL;
    }

    sal_memset(&info_given, 0, sizeof(bcm_port_info_t));
    sal_memset(info_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t));
    sal_memset(&ability_given, 0, sizeof(bcm_port_ability_t));
    sal_memset(&ability_port, 0, sizeof(bcm_port_ability_t));
    sal_memset(ability_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t));
    sal_memset(&config, 0x0, sizeof(bcm_phy_config_t));

    if (parse_bcm_pbmp(u, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm)) {
        ARG_DISCARD(a);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_OK;
    }

    if (ARG_CNT(a) == 0) {
        seen = BCM_PORT_ATTR_ALL_MASK;
    } else {
        /*
         * Otherwise, arguments are given.  Use them to determine which
         * properties need to be gotten/set.
         *
         * Probe and detach, hidden commands.
         */
        if (!sal_strcasecmp(_ARG_CUR(a), "detach")) {
            pbmp_t detached;
            bcm_port_detach(u, pbm, &detached);
            cli_out("Detached port bitmap %s\n", SOC_PBMP_FMT(detached, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        } else if ((!sal_strcasecmp(_ARG_CUR(a), "probe")) ||
                   (!sal_strcasecmp(_ARG_CUR(a), "attach"))) {
            pbmp_t probed;
            bcm_port_probe(u, pbm, &probed);
            cli_out("Probed port bitmap %s\n", SOC_PBMP_FMT(probed, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        } else if (!sal_strcasecmp(_ARG_CUR(a), "lanes")) {
            int lanes;
            ARG_GET(a);
            if ((c = ARG_GET(a)) == NULL) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
            lanes = sal_ctoi(c, 0);
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                rv = bcm_port_control_set(u, p, bcmPortControlLanes, lanes);
                if (rv < 0) {
                    cli_out("Unit:%d Port:%d :Port control lanes set failed"
                            " with %d\n",u, p, rv);
                    break;
                }
            }
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            if (rv < 0) {
                return CMD_FAIL;
            } else {
                return CMD_OK;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "Bridge")) {
            int pbridge = 0;
            ARG_GET(a);
            if ((c = ARG_GET(a)) != NULL) {
                pbridge = sal_ctoi(c, 0);
                DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                    rv = bcm_port_control_set(u, p, bcmPortControlBridge, pbridge);
                    if (rv < 0) {
                        cli_out("Unit %d Port %d :Port Bridge set %d failed"
                                " rv %d\n",u, p, pbridge, rv);
                        break;
                    }
                }
            } else {
                /* Port Bridge show */
                DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                    rv = bcm_port_control_get(u, p, bcmPortControlBridge, &pbridge);
                    if (rv >= 0) {
                        cli_out("Unit %d Port %4s :Port Bridge = %d\n",
                                u, SOC_PORT_NAME(u, p), pbridge);
                    } else {
                        cli_out("Unit %d Port %4s :Port Bridge get failed"
                                " rv %d\n",u, SOC_PORT_NAME(u, p), rv);
                    }
                }
            }
            sal_free(info_all);
            sal_free(ability_all);
            if (rv < 0) {
                return CMD_FAIL;
            } else {
                return CMD_OK;
            }
        } else if (!sal_strcasecmp(_ARG_CUR(a), "EEE")) {
            sal_free(info_all);
            sal_free(ability_all);
            c = ARG_GET(a);
            if ((c = ARG_CUR(a)) != NULL) {

                parse_table_init(u, &pt);
                parse_table_add(&pt, "ENable", PQ_DFL | PQ_STRING,
                                0, &eee_enable, 0);
                parse_table_add(&pt, "TxIDleTime", PQ_DFL | PQ_INT,
                                0, &eee_tx_idle_time, 0);
                parse_table_add(&pt, "TxWakeTime", PQ_DFL | PQ_INT,
                                0, &eee_tx_wake_time, 0);
                parse_table_add(&pt, "STats", PQ_DFL | PQ_STRING, 0, &stats, 0);

                if (parse_arg_eq(a, &pt) < 0) {
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }
                if (ARG_CNT(a) > 0) {
                    cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }

                flags = 0;

                for (i = 0; i < pt.pt_cnt; i++) {
                    if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                        flags |= (1 << i);
                    }
                }

                DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                    cli_out("Port = %5s(%3d)\n", SOC_PORT_NAME(u, p), p);
                    if (flags & 0x1) {
                        if (sal_strcasecmp(eee_enable, "enable") == 0) {
                            rv = bcm_port_control_set(u, p,
                                                      bcmPortControlEEEEnable,
                                                      1);
                        }
                        if (sal_strcasecmp(eee_enable, "disable") == 0) {
                            rv = bcm_port_control_set(u, p,
                                                      bcmPortControlEEEEnable,
                                                      0);
                        }

                        if (rv == BCM_E_NONE) {
                            cli_out("EEE state set to %s\n", eee_enable);
                        } else {
                            if (rv == BCM_E_UNAVAIL) {
                                cli_out("EEE %s is not available\n", eee_enable);
                            } else {
                                cli_out("EEE %s is unsuccessful\n", eee_enable);
                            }
                        }
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEEEnable,
                                                  &eee_state);
                        if (rv == BCM_E_NONE) {
                            if (eee_state == 1) {
                                str = "Enable";
                            } else {
                                str = "Disable";
                            }
                        } else {
                            str = "NA";
                        }
                        cli_out("EEE State = %s\n", str);
                    }

                    if (flags & 0x2) {
                        rv = bcm_port_control_set(u, p,
                                                  bcmPortControlEEETransmitIdleTime,
                                                  eee_tx_idle_time);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control set: "
                                    "bcmPortControlEEETransmitIdleTime "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEETransmitIdleTime,
                                                  &eee_tx_idle_time);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control get: "
                                    "bcmPortControlEEETransmitIdleTime "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        cli_out("EEE Transmit Idle Time is = %d(us)\n",
                                eee_tx_idle_time);
                    }

                    if (flags & 0x4) {
                        rv = bcm_port_control_set(u, p,
                                                  bcmPortControlEEETransmitWakeTime,
                                                  eee_tx_wake_time);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control set: "
                                    "bcmPortControlEEETransmitWakeTime "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEETransmitWakeTime,
                                                  &eee_tx_wake_time);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control get: "
                                    "bcmPortControlEEETransmitWakeTime "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        cli_out("EEE Transmit Wake Time is = %d(us)\n",
                                eee_tx_wake_time);
                    }

                    if (flags & 0x8) {
                        if (sal_strcasecmp(stats, "get") == 0) {
                            rv = bcm_port_control_get(u, p,
                                                      bcmPortControlEEETransmitEventCount,
                                                      &eee_tx_ev_cnt);
                            if (BCM_FAILURE(rv)) {
                                cli_out("Port control get: "
                                        "bcmPortControlEEETransmitEventCount "
                                        "failed with %d\n", rv);
                                return CMD_FAIL;
                            }
                            rv = bcm_port_control_get(u, p,
                                                      bcmPortControlEEETransmitDuration,
                                                      &eee_tx_dur);
                            if (BCM_FAILURE(rv)) {
                                cli_out("Port control get: "
                                        "bcmPortControlEEETransmitDuration "
                                        "failed with %d\n", rv);
                                return CMD_FAIL;
                            }
                            rv = bcm_port_control_get(u, p,
                                                      bcmPortControlEEEReceiveEventCount,
                                                      &eee_rx_ev_cnt);
                            if (BCM_FAILURE(rv)) {
                                cli_out("Port control get: "
                                        "bcmPortControlEEEReceiveEventCount "
                                        "failed with %d\n", rv);
                                return CMD_FAIL;
                            }
                            rv = bcm_port_control_get(u, p,
                                                      bcmPortControlEEEReceiveDuration,
                                                      &eee_rx_dur);
                            if (BCM_FAILURE(rv)) {
                                cli_out("Port control get: "
                                        "bcmPortControlEEEReceiveDuration "
                                        "failed with %d\n", rv);
                                return CMD_FAIL;
                            }
                            cli_out("Tx events = %d TX Duration = %d(us) "
                                    "RX events = %d RX Duration = %d(us)\n",
                                    eee_tx_ev_cnt, eee_tx_dur, eee_rx_ev_cnt,
                                    eee_rx_dur);
                        }
                    }
                }
                /* free allocated memory from arg parsing */
                parse_arg_eq_done(&pt);
            } else {
                cli_out("EEE Details:\n");
                cli_out("%10s %9s %14s %14s %12s %14s %12s %14s\n",
                        "port", "EEE-State", "TxIdleTime(us)", "TxWakeTime(us)",
                        "TxEventCount", "TxDuration(us)", "RxEventCount",
                        "RxDuration(us)");
                DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {

                    rv = bcm_port_control_get(u, p, bcmPortControlEEEEnable,
                                              &eee_state);
                    if (BCM_FAILURE(rv)) {
                        cli_out("Port control get:bcmPortControlEEEEnable "
                                "failed with %d\n", rv);
                        return CMD_FAIL;
                    }
                    rv = bcm_port_control_get(u, p,
                                              bcmPortControlEEETransmitIdleTime,
                                              &eee_tx_idle_time);
                    if (BCM_FAILURE(rv)) {
                        cli_out("Port control get: "
                                "bcmPortControlEEETransmitIdleTime "
                                "failed with %d\n", rv);
                        return CMD_FAIL;
                    }
                    rv = bcm_port_control_get(u, p,
                                              bcmPortControlEEETransmitWakeTime,
                                              &eee_tx_wake_time);
                    if (BCM_FAILURE(rv)) {
                        cli_out("Port control get: "
                                "bcmPortControlEEETransmitWakeTime "
                                "failed with %d\n", rv);
                        return CMD_FAIL;
                    }
                    if (eee_state) {
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEETransmitEventCount,
                                                  &eee_tx_ev_cnt);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control get: "
                                    "bcmPortControlEEETransmitEventCount "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEETransmitDuration,
                                                  &eee_tx_dur);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control get: "
                                    "bcmPortControlEEETransmitDuration "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEEReceiveEventCount,
                                                  &eee_rx_ev_cnt);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control get: "
                                    "bcmPortControlEEEReceiveEventCount "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                        rv = bcm_port_control_get(u, p,
                                                  bcmPortControlEEEReceiveDuration,
                                                  &eee_rx_dur);
                        if (BCM_FAILURE(rv)) {
                            cli_out("Port control get: "
                                    "bcmPortControlEEEReceiveDuration "
                                    "failed with %d\n", rv);
                            return CMD_FAIL;
                        }
                    } else {
                        eee_tx_ev_cnt = -1;
                        eee_tx_dur = -1;
                        eee_rx_ev_cnt = -1;
                        eee_rx_dur = -1;
                    }

                    cli_out("%5s(%3d) %9s %14u %14u %12u %14u %12u %14u\n",
                            SOC_PORT_NAME(u, p), p,
                            eee_state ? "Enable" : "Disable", eee_tx_idle_time,
                            eee_tx_wake_time, eee_tx_ev_cnt, eee_tx_dur,
                            eee_rx_ev_cnt, eee_rx_dur);
                }
            }
            return CMD_OK;
        }  
#if defined(BCM_ESW_SUPPORT)

        else if (!sal_strcasecmp(_ARG_CUR(a), "detail")) {
            DPORT_SOC_PBMP_ITER(u, pbm, dport,p) {
            cli_out("******************************************** \n");
            cli_out("\t %s PORT = %d INFORMATION          \n",soc_dev_name(u),p);
            cli_out("********************************************\n\n");
            /* coverity[overrun-call:FALSE] */
            rv = _bcm_esw_port_sw_info_display(u, p);
            if (BCM_FAILURE(rv)) {
                cli_out("Port software information display failed with %d\n", rv);
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_FAIL;
            }

            rv = _bcm_esw_port_hw_info_display(u, p);
            if (BCM_FAILURE(rv)) {
                cli_out("Port hardware infomation display failed with %d\n", rv);
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_FAIL;
            } else {
                 seen = BCM_PORT_ATTR_ALL_MASK;
            }
            }
        }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        else if (!sal_strcasecmp(_ARG_CUR(a), "asf")) {
            int asf_mode;
            int in_range = 0;

            if (!soc_feature(u, soc_feature_asf) &&
                !soc_feature(u, soc_feature_asf_multimode)) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_NOTIMPL;
            }

            ARG_GET(a);
            if ((c = ARG_GET(a)) == NULL) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
            asf_mode = sal_ctoi(c, 0);
            in_range = _asf_mode_in_range(u, asf_mode);
            if (!in_range) {
                cli_out("%d: invalid mode\n", asf_mode);
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_OK;
            }
            DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                rv = bcm_switch_control_port_set(u, p,
                         bcmSwitchAlternateStoreForward, asf_mode);
                if (BCM_FAILURE(rv)) {
                    if (BCM_E_UNAVAIL == rv) {
                        cli_out("%s: unsupported\n", BCM_PORT_NAME(u, p));
                    }
                    if (BCM_E_PARAM == rv) {
                        cli_out("%s: invalid arg(s)\n", BCM_PORT_NAME(u, p));
                    }
                }
                continue;
            }
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        }
#endif
        if (seen == 0) {
         if (!sal_strcmp(_ARG_CUR(a), "=")) {
            /*
             * For "=" where the user is prompted to enter all the parameters,
             * use the parameters from the first selected port as the defaults.
             */
            if (ARG_CNT(a) != 1) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
            DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                break;          /* Find first port in bitmap */
            }
            port_info_init(u, p, &info_given, 0);
            if ((rv = bcm_port_info_get(u, p, &info_given)) < 0) {
                cli_out("%s: Error: Failed to get port info\n", ARG_CMD(a));
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_FAIL;
            }
        } else {
            DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                break;          /* Find first port in bitmap */
            }
            port_info_init(u, p, &info_given, 0);
        }

        /*
         * Parse the arguments.  Determine which ones were actually given.
         */
        port_parse_setup(u, &pt, &info_given);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }

        if (info_given.encap_mode >= 0
              && info_given.encap_mode < (sizeof(encap_mode) / sizeof(encap_mode[0]))
              &&!sal_strcmp(encap_mode[info_given.encap_mode], "HGOE")) {
            info_given.encap_mode = BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET;
        }

        /* Translate port_info into port abilities. */

        if ((seen == 0) && (ARG_CNT(a) > 0)) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }

        /*
         * Find out what parameters specified.  Record values specified.
         */
        port_parse_mask_get(&pt, &seen, &parsed);
        parse_arg_eq_done(&pt);
     }
    }
    if (seen && parsed) {
        cli_out("%s: Cannot get and set "
                "port properties in one command\n", ARG_CMD(a));
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    } else if (seen) {          /* Show selected information */
        cli_out("%s: Status (* indicates PHY link up)\n", ARG_CMD(a));
        /* Display the information by port type */
#define _call_pdi(u, p, mp, s) \
        if (_port_disp_iter(u, p, mp, s) != CMD_OK) { \
             sal_free(info_all); \
             sal_free(ability_all); \
             return CMD_FAIL; \
        }
        _call_pdi(u, pbm, pcfg.fe, seen);
        _call_pdi(u, pbm, pcfg.ge, seen);
        _call_pdi(u, pbm, pcfg.xe, seen);
        _call_pdi(u, pbm, pcfg.ce, seen);
        _call_pdi(u, pbm, pcfg.hg, seen);
        _call_pdi(u, pbm, pcfg.cd, seen);
        sal_free(info_all);
        sal_free(ability_all);
        ARG_GET(a);
        return (CMD_OK);
    }

    /* Some set information was given */

    if (parsed & BCM_PORT_ATTR_LINKSCAN_MASK) {
        /* Map ON --> S/W, OFF--> None */
        if (info_given.linkscan > 2) {
            info_given.linkscan -= 3;
        }
    }

    parsed_adj = parsed;
    if (parsed & (BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK)) {
        parsed_adj |= BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK;
    }

    /*
     * Retrieve all requested port information first, then later write
     * back all port information.  That prevents a problem with loopback
     * cables where setting one port's info throws another into autoneg
     * causing it to return info in flux (e.g. suddenly go half duplex).
     */

    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        port_info_init(u, p, &info_all[p], parsed_adj);
        if ((r = bcm_port_selective_get(u, p, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not get port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
            continue;
        }
    }

    /*
     * Loop through all the specified ports, changing whatever field
     * values were actually given.  This avoids copying unaffected
     * information from one port to another and prevents having to
     * re-parse the arguments once per port.
     */

    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        if (parsed & BCM_PORT_ATTR_MEDIUM_MASK) {
            rv = bcm_port_medium_config_set(u, p, info_given.medium, &config);
            if (rv < 0) {
                cli_out("port parse: Error: Could not set port %s medium: %s\n",
                    BCM_PORT_NAME(u, p), bcm_errmsg(rv));
                continue;
            }
        }

        if ((rv = bcm_port_speed_max(u, p, &info_given.speed_max)) < 0) {
            cli_out("port parse: Error: Could not get port %s max speed: %s\n",
                    BCM_PORT_NAME(u, p), bcm_errmsg(rv));
            continue;
        }

        if (!IS_CD_PORT(u, p)) {
        if ((rv =
             bcm_port_ability_local_get(u, p, &info_given.port_ability)) < 0) {
            cli_out("port parse: Error: Could not get port %s ability: %s\n",
                    BCM_PORT_NAME(u, p), bcm_errmsg(rv));
            continue;
        }
        if ((rv = soc_port_ability_to_mode(&info_given.port_ability,
                                           &info_given.ability)) < 0) {
            cli_out("port parse: Error: Could not "
                    "transform port %s ability to mode: %s\n",
                    BCM_PORT_NAME(u, p), bcm_errmsg(rv));
            continue;
        }
        }

        if ((r = port_parse_port_info_set(parsed,
                                          &info_given, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not parse port %s info: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }

        /* If AN is on, do not set speed, duplex, pause */
        if (info_all[p].autoneg) {
            info_all[p].action_mask &= ~BCM_PORT_AN_ATTRS;
        } else if (parsed &
                   (BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_DUPLEX_MASK)) {
            int full_duplex_speed;
            int half_duplex_speed;

            pa_speed = SOC_PA_SPEED(info_all[p].speed);

            full_duplex_speed = info_given.port_ability.speed_full_duplex
                                & pa_speed;
            half_duplex_speed = info_given.port_ability.speed_half_duplex
                                & pa_speed;

            if (!soc_feature(u, soc_feature_flexport_based_speed_set)
                && (info_all[p].speed != 0)) {

                if (info_all[p].duplex) {
                    if (!full_duplex_speed) {
                        cli_out("%s: port %s does not support "
                                "%d mbps full duplex\n",
                                ARG_CMD(a), BCM_PORT_NAME(u, p),
                                info_all[p].speed);
                        continue;
                    }
                } else {
                    if (!half_duplex_speed) {
                        cli_out("%s: port %s does not support "
                                "%d mbps half duplex\n",
                                ARG_CMD(a), BCM_PORT_NAME(u, p),
                                info_all[p].speed);
                        continue;
                    }
                }
            }
        } else if (IS_CD_PORT(u, p) && (parsed & BCM_PORT_ATTR_AUTONEG_MASK)) {
            /* if pm8x50 and autoneg is being turned off, don't set speed */
            info_all[p].action_mask &= ~BCM_PORT_ATTR_SPEED_MASK;
        }
        if ((r = bcm_port_selective_set(u, p, &info_all[p])) < 0) {
            cli_out("%s: Error: Could not set port %s information: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }
    }

    sal_free(info_all);
    sal_free(ability_all);
    return (cmd_rv);
}

cmd_result_t
if_esw_gport(int u, args_t *a)
{
    char *c;
    soc_port_t arg_port = 0;
    parse_table_t pt;
    cmd_result_t retCode;
    char gpfmt[FORMAT_GPORT_MAX];

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    if ((c = ARG_CUR(a)) == NULL) {
        return (CMD_USAGE);
    }
    parse_table_init(u, &pt);
    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &arg_port, NULL);
    if (!parseEndOk(a, &pt, &retCode)) {
        cli_out(" Unable to Parse GPORT !!! \n");
        return retCode;
    }

    cli_out(" gport is = %s\n", format_gport(gpfmt, arg_port));
    return CMD_OK;
}

char if_egress_usage[] =
    "Usages:\n\t"
    "  egress set [<Port=port#>] [<Modid=modid>] <PBmp=val>\n\t"
    "        - Set allowed egress bitmap for (modid,port) or all\n\t"
    "  egress show [<Port=port#>] [<Module=modid>]\n\t"
    "        - Show allowed egress bitmap for (modid,port)\n";

/*
 * Note:
 *
 * Since these port numbers are likely on different modules, we cannot
 * use PBMP qualifiers by this unit.
 */

cmd_result_t
if_egress(int unit, args_t *a)
{
    char *subcmd, *c;
    soc_info_t *si = &SOC_INFO(unit);
    int port, arg_port = -1, min_port = 0, max_port = si->modport_max;
    int modid, arg_modid = -1, mod_min = 0, mod_max = SOC_MODID_MAX(unit);
    bcm_pbmp_t pbmp;
    int r;
    bcm_pbmp_t arg_pbmp;
    parse_table_t pt;
    cmd_result_t retCode;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    /* Egress show command */
    if (sal_strcasecmp(subcmd, "show") == 0) {

        if ((c = ARG_CUR(a)) != NULL) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &arg_port, NULL);
            parse_table_add(&pt, "Modid", PQ_DFL | PQ_INT, 0, &arg_modid, NULL);
            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }

            if (BCM_GPORT_IS_SET(arg_port)) {
#if defined(BCM_ESW_SUPPORT)
                int tgid, id;

                if (SOC_IS_ESW(unit)) {
                    r = _bcm_esw_gport_resolve(unit, arg_port, &arg_modid,
                                               &arg_port, &tgid, &id);
			if ((tgid != BCM_TRUNK_INVALID) || (id != -1)||(r != BCM_E_NONE)) {
                        return CMD_FAIL;
                    }
                } else {
                    return CMD_FAIL;
                }
#else
                return CMD_FAIL;
#endif
            }
            if (arg_modid >= 0) {
                mod_min = mod_max = arg_modid;
            }
            if (arg_port >= 0) {
                min_port = max_port = arg_port;
            }
        }

        for (modid = mod_min; modid <= mod_max; modid++) {
            for (port = min_port; port <= max_port; port++) {
                r = bcm_port_egress_get(unit, port, modid, &pbmp);
                if (r < 0) {
                    cli_out("%s: egress (modid=%d, port=%d) get failed: %s\n",
                            ARG_CMD(a), modid, port, bcm_errmsg(r));
                    return CMD_FAIL;
                }

                if (BCM_PBMP_NEQ(pbmp, PBMP_ALL(unit))) {
                    char buf[FORMAT_PBMP_MAX];
                    format_bcm_pbmp(unit, buf, sizeof(buf), pbmp);
                    cli_out("Module %d, port %d:  Enabled egress ports %s\n",
                            modid, port, buf);
                }
            }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "set") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &arg_port, NULL);
        parse_table_add(&pt, "Modid", PQ_DFL | PQ_INT, 0, &arg_modid, NULL);
        parse_table_add(&pt, "Pbmp", PQ_DFL | PQ_PBMP | PQ_BCM, 0, &arg_pbmp,
                        NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        SOC_PBMP_ASSIGN(pbmp, arg_pbmp);

        r = bcm_port_egress_set(unit, arg_port, arg_modid, pbmp);

    } else {
        return CMD_USAGE;
    }

    if (r < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_result_t
if_esw_dscp(int unit, args_t *a)
{
    int rv, port, dport, srccp, mapcp, prio, cng, mode, count, i;
    bcm_port_config_t pcfg;
    bcm_pbmp_t pbm;
    bcm_pbmp_t tpbm;
    int use_global = 0;
    char *s;
    parse_table_t pt;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((s = ARG_GET(a)) == NULL) {
        cli_out("%s: ERROR: missing port bitmap\n", ARG_CMD(a));
        return CMD_FAIL;
    }
    if (parse_bcm_pbmp(unit, s, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), s);
        return CMD_FAIL;
    }

    BCM_PBMP_ASSIGN(tpbm, pbm);
    BCM_PBMP_XOR(tpbm, pcfg.e);
    if (BCM_PBMP_IS_NULL(tpbm)) {
        /* global table */
        use_global = 1;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mode", PQ_INT, (void *)-1, &mode, NULL);
    rv = parse_arg_eq(a, &pt);
    parse_arg_eq_done(&pt);
    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    }

    srccp = -1;
    s = ARG_GET(a);
    if (s) {
        srccp = parse_integer(s);
    }

    if ((s = ARG_GET(a)) == NULL) {
        if (mode != -1) {
            cli_out("%s: WARNING: ignore mode argument\n", ARG_CMD(a));
        }
        if (srccp < 0) {
            srccp = 0;
            count = 64;
        } else {
            count = 1;
        }
        if (BCM_PBMP_IS_NULL(pbm) &&
            !soc_feature(unit, soc_feature_dscp_map_per_port)) {
            port = -1;
            rv = bcm_port_dscp_map_get(unit, port, 0, &mapcp, &prio);
            if (rv == BCM_E_PORT) {
                port = -1;
                cli_out("%d: dscp map:\n", unit);
            }
            for (i = 0; i < count; i++) {
                rv = bcm_port_dscp_map_get(unit, port, srccp + i, &mapcp,
                                           &prio);
                if (rv < 0) {
                    cli_out("ERROR: dscp map get %d failed: %s\n",
                            srccp + i, bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                if (srccp + i != mapcp || count == 1) {
                    cli_out(" %d->%d prio=%d cng=%d\n",
                            srccp + i, mapcp,
                            prio & BCM_PRIO_MASK,
                            (prio & BCM_PRIO_DROP_FIRST) ? 1 : 0);

                }
            }
            cli_out("\n");
        } else {
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
                if (use_global) {
                    rv = bcm_port_dscp_map_get(unit, port, 0, &mapcp, &prio);
                    if (rv == BCM_E_PORT) {
                        port = -1;
                        cli_out("%d: dscp map:\n", unit);
                    }
                } else {
                    cli_out("%d:%s dscp map:\n", unit,
                            BCM_PORT_NAME(unit, port));
                }
                for (i = 0; i < count; i++) {
                    rv = bcm_port_dscp_map_get(unit, port, srccp + i, &mapcp,
                                               &prio);
                    if (rv < 0) {
                        cli_out("ERROR: dscp map get %d failed: %s\n",
                                srccp + i, bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                    if (srccp + i != mapcp || count == 1) {
                        cli_out(" %d->%d prio=%d cng=%d\n",
                                srccp + i, mapcp,
                                prio & BCM_PRIO_MASK,
                                (prio & BCM_PRIO_DROP_FIRST) ? 1 : 0);

                    }
                }
                cli_out("\n");
                if (port == -1) {
                    break;
                }
            }
        }
        return CMD_OK;
    }

    mapcp = parse_integer(s);
    prio = -1;
    cng = 0;

    if ((s = ARG_GET(a)) != NULL) {
        prio = parse_integer(s);
        if ((s = ARG_GET(a)) != NULL) {
            cng = parse_integer(s);
        }
    }
    if (cng)
        prio |= BCM_PRIO_DROP_FIRST;

    /* Allow empty pbmp to configure devices that don't support per port */
    /*  dscp mapping */
    if (BCM_PBMP_IS_NULL(pbm) &&
        !soc_feature(unit, soc_feature_dscp_map_per_port)) {
        port = -1;

        if (mode != -1) {
            rv = bcm_port_dscp_map_mode_set(unit, port, mode);
            if (rv < 0) {
                cli_out("%d:%s ERROR: dscp mode set mode=%d: %s\n",
                        unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
                        mode, bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        rv = bcm_port_dscp_map_set(unit, port, srccp, mapcp, prio);
        if (rv < 0) {
            cli_out("%d:%s ERROR: "
                    "dscp map set %d->%d prio=%d cng=%d failed: %s\n",
                    unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
                    srccp, mapcp, prio, cng, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
            if (use_global) {
                port = -1;
            }

            if (mode != -1) {
                rv = bcm_port_dscp_map_mode_set(unit, port, mode);
                if (rv < 0) {
                    cli_out("%d:%s ERROR: dscp mode set mode=%d: %s\n",
                            unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
                            mode, bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }

            rv = bcm_port_dscp_map_set(unit, port, srccp, mapcp, prio);
            if (rv < 0) {
                cli_out("%d:%s ERROR: "
                        "dscp map set %d->%d prio=%d cng=%d failed: %s\n",
                        unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
                        srccp, mapcp, prio, cng, bcm_errmsg(rv));
                return CMD_FAIL;
            }
            if (port == -1) {
                break;
            }
        }
    }
    return CMD_OK;
}

cmd_result_t
if_esw_ipg(int unit, args_t *a)
/*
 * Function:   if_ipg
 * Purpose:    Configure default IPG values.
 * Parameters: unit - SOC unit #
 *    a - arguments
 * Returns:    CMD_OK/CMD_FAIL
 */
{
    parse_table_t pt;
    pbmp_t arg_pbmp;
    int arg_speed, speed;
    bcm_port_duplex_t arg_duplex, duplex;
    int arg_gap;
    int arg_stretch;
    int stretch;
    cmd_result_t retCode;
    int real_ifg;
    int rv;
    int i;
    bcm_port_config_t pcfg;
    bcm_port_t port, dport;

    const char *header =
        "        "
        "  10M_HD"
        "  10M_FD"
        " 100M_HD"
        " 100M_FD"
        "   1G_HD"
        "   1G_FD"
        " 2.5G_HD"
        " 2.5G_FD"
        "  10G_FD"
        "  25G_FD"
        "  40G_FD"
        " 100G_FD"
        " 106G_FD"
        " STRETCH";

    const int speeds[] = { 10, 100, 1000, 2500, 10000, 25000, 40000, 100000, 106000 };
    const int num_speeds = sizeof(speeds) / sizeof(int);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /*
     * Assign the defaults
     */
    BCM_PBMP_ASSIGN(arg_pbmp, pcfg.port);
    arg_speed = 0;
    arg_duplex = BCM_PORT_DUPLEX_COUNT;
    arg_gap = 0;
    arg_stretch = -1;

    /*
     * Parse the arguments
     */
    if (ARG_CNT(a)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL | PQ_PBMP | PQ_BCM,
                        0, &arg_pbmp, NULL);
        parse_table_add(&pt, "SPeed", PQ_DFL | PQ_INT, 0, &arg_speed, NULL);
        parse_table_add(&pt, "FullDuplex", PQ_DFL | PQ_BOOL,
                        0, &arg_duplex, NULL);
        parse_table_add(&pt, "Gap", PQ_DFL | PQ_INT, 0, &arg_gap, NULL);
        parse_table_add(&pt, "STretch", PQ_DFL | PQ_INT, 0, &arg_stretch, NULL);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
    }

    cli_out("%s\n", header);
    /*
     * Display IPG settings for all the specified ports
     */
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
        cli_out("%-8.8s", BCM_PORT_NAME(unit, port));
        for (i = 0; i < num_speeds; i++) {
            speed = speeds[i];

            for (duplex = BCM_PORT_DUPLEX_HALF;
                 duplex < BCM_PORT_DUPLEX_COUNT; duplex++) {
                /*
                 * Skip the illegal 10000HD combination
                 */
                if (speed >= 10000 && duplex == BCM_PORT_DUPLEX_HALF) {
                    continue;
                }

                /*
                 * Skip an entry if the speed has been explicitly specified
                 */
                if (arg_speed != 0 && speed != arg_speed) {
                    cli_out("%8.8s", " ");
                    continue;
                }

                /*
                 * Skip an entry if duplex has been explicitly specified
                 * and the entry doesn't match
                 */
                if (arg_duplex != BCM_PORT_DUPLEX_COUNT && arg_duplex != duplex) {
                    cli_out("%8.8s", " ");
                    continue;
                }

                if (arg_gap != 0) {
                    rv = bcm_port_ifg_set(unit, port, speed, duplex, arg_gap);
		    if (rv != BCM_E_NONE) {
			return CMD_FAIL;
		    }
                }

                rv = bcm_port_ifg_get(unit, port, speed, duplex, &real_ifg);

                if (rv == BCM_E_NONE) {
                    cli_out("%8d", real_ifg);
                } else {
                    cli_out("%8.8s", "n/a");
                }
            }
        }
        if (arg_stretch >= 0) {
            rv = bcm_port_control_set(unit, port,
                                      bcmPortControlFrameSpacingStretch,
                                      arg_stretch);
		    if (rv != BCM_E_NONE) {
			return CMD_FAIL;
		    }
        }
        rv = bcm_port_control_get(unit, port, bcmPortControlFrameSpacingStretch,
                                  &stretch);

        if (rv == BCM_E_NONE) {
            cli_out("%8d", stretch);
        } else {
            cli_out("%8.8s", "n/a");
        }
        cli_out("\n");
    }

    return (CMD_OK);
}

cmd_result_t
if_esw_dtag(int unit, args_t *a)
{
    char *subcmd, *c;
    bcm_port_config_t pcfg;
    bcm_pbmp_t pbmp;
    bcm_port_t port, dport;
    int mode, r;
    uint16 tpid;
    int dt_mode_mask;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    dt_mode_mask = BCM_PORT_DTAG_MODE_INTERNAL | BCM_PORT_DTAG_MODE_EXTERNAL;

    if ((subcmd = ARG_GET(a)) == NULL) {
        subcmd = "show";
    }

    c = ARG_GET(a);
    if (c == NULL) {
        BCM_PBMP_ASSIGN(pbmp, pcfg.e);
    } else {
        if (parse_bcm_pbmp(unit, c, &pbmp) < 0) {
            cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
            return CMD_FAIL;
        }
    }

    r = 0;
    if (sal_strcasecmp(subcmd, "show") == 0) {
        char *mode_flag;

        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_port_dtag_mode_get(unit, port, &mode);
            if (r < 0) {
                goto bcm_err;
            }
            r = bcm_port_tpid_get(unit, port, &tpid);
            if (r < 0) {
                goto bcm_err;
            }
            switch (mode & dt_mode_mask) {
                case BCM_PORT_DTAG_MODE_NONE:
                    c = "none (disabled)";
                    break;
                case BCM_PORT_DTAG_MODE_INTERNAL:
                    c = "internal (service provider)";
                    break;
                case BCM_PORT_DTAG_MODE_EXTERNAL:
                    c = "external (customer)";
                    break;
                default:
                    c = "unknown";
                    break;
            }

            mode_flag = "";
            if (mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
                mode_flag = " remove customer tag";
            } else if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG) {
                mode_flag = " add customer tag";
            }

            cli_out("port %d:%s\tdouble tag mode %s%s, tpid 0x%x\n",
                    unit, BCM_PORT_NAME(unit, port), c, mode_flag, tpid);
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "mode") == 0) {
        c = ARG_GET(a);
        if (c == NULL) {
            char *mode_flag;
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

                r = bcm_port_dtag_mode_get(unit, port, &mode);
                if (r < 0) {
                    goto bcm_err;
                }
                switch (mode & dt_mode_mask) {
                    case BCM_PORT_DTAG_MODE_NONE:
                        c = "none (disabled)";
                        break;
                    case BCM_PORT_DTAG_MODE_INTERNAL:
                        c = "internal (service provider)";
                        break;
                    case BCM_PORT_DTAG_MODE_EXTERNAL:
                        c = "external (customer)";
                        break;
                    default:
                        c = "unknown";
                        break;
                }

                mode_flag = "";
                if (mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
                    mode_flag = " remove customer tag";
                } else if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG) {
                    mode_flag = " add customer tag";
                }

                cli_out("port %d:%s\tdouble tag mode %s%s\n",
                        unit, BCM_PORT_NAME(unit, port), c, mode_flag);
            }
            return CMD_OK;
        }
        if (sal_strcasecmp(c, "none") == 0) {
            mode = BCM_PORT_DTAG_MODE_NONE;
        } else if (sal_strcasecmp(c, "internal") == 0) {
            mode = BCM_PORT_DTAG_MODE_INTERNAL;
        } else if (sal_strcasecmp(c, "external") == 0) {
            mode = BCM_PORT_DTAG_MODE_EXTERNAL;
        } else {
            return CMD_USAGE;
        }
        c = ARG_GET(a);
        if (c != NULL) {
            if (sal_strcasecmp(c, "addInnerTag") == 0) {
                mode |= BCM_PORT_DTAG_ADD_EXTERNAL_TAG;
            } else if (sal_strcasecmp(c, "removeInnerTag") == 0) {
                mode |= BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG;
            } else {
                return CMD_OK;
            }
        }
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_port_dtag_mode_set(unit, port, mode);
            if (r < 0) {
                goto bcm_err;
            }
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "tpid") == 0) {
        c = ARG_GET(a);
        if (c == NULL) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_port_tpid_get(unit, port, &tpid);
                if (r < 0) {
                    goto bcm_err;
                }
                cli_out("port %d:%s\ttpid 0x%x\n",
                        unit, BCM_PORT_NAME(unit, port), tpid);
            }
        } else {
            tpid = parse_integer(c);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_port_tpid_set(unit, port, tpid);
                if (r < 0) {
                    goto bcm_err;
                }
            }
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "addTpid") == 0) {
        c = ARG_GET(a);
        if (c == NULL) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_port_tpid_get(unit, port, &tpid);
                if (r < 0) {
                    goto bcm_err;
                }
                cli_out("port %d:%s\ttpid 0x%x\n",
                        unit, BCM_PORT_NAME(unit, port), tpid);
            }
        } else {
            tpid = parse_integer(c);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_port_tpid_add(unit, port, tpid, BCM_COLOR_PRIORITY);
                if (r < 0) {
                    goto bcm_err;
                }
            }
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "deleteTpid") == 0) {
        c = ARG_GET(a);
        if (c == NULL) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_port_tpid_get(unit, port, &tpid);
                if (r < 0) {
                    goto bcm_err;
                }
                cli_out("port %d:%s\ttpid 0x%x\n",
                        unit, BCM_PORT_NAME(unit, port), tpid);
            }
        } else {
            if (sal_strcasecmp(c, "all") == 0) {
                DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                    r = bcm_port_tpid_delete_all(unit, port);
                    if (r < 0) {
                        goto bcm_err;
                    }
                }
            } else {
                tpid = parse_integer(c);
                DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                    r = bcm_port_tpid_delete(unit, port, tpid);
                    if (r < 0) {
                        goto bcm_err;
                    }
                }
            }
        }
        return CMD_OK;
    }

    return CMD_USAGE;

 bcm_err:
    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
    return CMD_FAIL;
}

#ifdef SW_AUTONEG_SUPPORT
cmd_result_t
if_esw_swAutoneg(int unit, args_t *a) 
{
    int rv = 0;
    char *c;
    int enabled = 0;
    bcm_port_config_t pcfg;


    rv = bcm_init_check(unit);
    if (rv == BCM_E_UNIT) {
        cli_out("unit %d: is not initialized\n", unit);
        return CMD_FAIL;
    }
   

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_sw_autoneg)) {
        cli_out("%s: Error: SW Autoneg is not supported on this platform\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        /* display status of SW AN thread enabled/disabled */
        rv = bcm_sw_an_enable_get(unit, &enabled);
        if (rv != BCM_E_NONE) {
            cli_out("unit %d: %s failed\n", unit, FUNCTION_NAME());
            return CMD_FAIL;    
        }
        if (enabled) {
            cli_out("SW AN is enabled on unit %d\n", unit);
        } else {
            cli_out("SW AN is disabled on unit %d\n", unit);    
        }
        return CMD_OK;
    }

    if (ARG_CUR(a) != NULL) {
        c = ARG_GET(a);

        if (!sal_strcasecmp(c, "off") ||
            !sal_strcasecmp(c, "disable") ||
            !sal_strcasecmp(c, "no")) {
            rv = bcm_sw_an_enable_set(unit, 0);
            if (rv != BCM_E_NONE) {
                cli_out("unit %d: %s failed\n", unit, FUNCTION_NAME());
                return CMD_FAIL;  
            } 
        } else if (!sal_strcasecmp(c, "on") ||
                   !sal_strcasecmp(c, "enable") ||
                   !sal_strcasecmp(c, "yes")) {
            rv = bcm_sw_an_enable_set(unit, 1);
            if (rv != BCM_E_NONE) {
                cli_out("unit %d: %s failed\n", unit, FUNCTION_NAME());
                return CMD_FAIL;  
            } 
        } else {
            cli_out("unit: %d SW_AN Invalid argument\n", unit);
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}
#endif

cmd_result_t
if_esw_linkscan(int unit, args_t *a)
{
    parse_table_t pt;
    char *c;
    int us, rv;
    pbmp_t pbm_sw, pbm_hw, pbm_none, pbm_force;
    pbmp_t pbm_temp;
    pbmp_t pbm_sw_pre, pbm_hw_pre, pbm_none_pre;
    bcm_port_config_t pcfg;
    soc_port_t port, dport;
    char pfmt[SOC_PBMP_FMT_LEN];
    int linkscan_interval_default;

    linkscan_interval_default = soc_property_get(unit,
        spn_BCM_LINKSCAN_INTERVAL, BCM_LINKSCAN_INTERVAL_DEFAULT);

    
    /*
     * Workaround that allows "linkscan off" at the beginning of rc.soc
     */

    if (ARG_CNT(a) == 1 && sal_strcasecmp(_ARG_CUR(a), "off") == 0) {
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT)
            soc_iproc_m0ssq_exit(unit);
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
        }
#endif /* BCM_CMICX_SUPPORT */
        rv = bcm_init_check(unit);
        if (rv == BCM_E_UNIT) {
            (void)ARG_GET(a);
            return (CMD_OK);
        }
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /*
     * First get current linkscan state.  (us == 0 if disabled).
     */

    if ((rv = bcm_linkscan_enable_get(unit, &us)) < 0) {
        cli_out("%s: Error: Failed to recover enable status: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    BCM_PBMP_CLEAR(pbm_sw);
    BCM_PBMP_CLEAR(pbm_hw);
    BCM_PBMP_CLEAR(pbm_force);

    DPORT_BCM_PBMP_ITER(unit, pcfg.port, dport, port) {
        int mode;

        if ((rv = bcm_linkscan_mode_get(unit, port, &mode)) < 0) {
            /* Ignore requeue ports for linkscan */
            if (!IS_REQ_PORT(unit, port)) {
                cli_out("%s: Error: Could not get linkscan state for port %s\n",
                        ARG_CMD(a), BCM_PORT_NAME(unit, port));
            }
        } else {
            switch (mode) {
                case BCM_LINKSCAN_MODE_SW:
                    BCM_PBMP_PORT_ADD(pbm_sw, port);
                    break;
                case BCM_LINKSCAN_MODE_HW:
                    BCM_PBMP_PORT_ADD(pbm_hw, port);
                    break;
                default:
                    break;
            }
        }
    }
    BCM_PBMP_ASSIGN(pbm_sw_pre, pbm_sw);
    BCM_PBMP_ASSIGN(pbm_hw_pre, pbm_hw);
    BCM_PBMP_REMOVE(pbm_sw_pre, PBMP_HG_SUBPORT_ALL(unit));
    BCM_PBMP_REMOVE(pbm_hw_pre, PBMP_HG_SUBPORT_ALL(unit));

    /*
     * If there are no arguments, just display the status.
     */

    if (ARG_CNT(a) == 0) {
        char buf[FORMAT_PBMP_MAX];
        pbmp_t pbm;

        if (us) {
            cli_out("%s: Linkscan enabled\n", ARG_CMD(a));
            cli_out("%s:   Software polling interval: %d usec\n",
                    ARG_CMD(a), us);
            format_bcm_pbmp(unit, buf, sizeof(buf), pbm_sw);
            cli_out("%s:   Software Port BitMap %s: %s\n",
                    ARG_CMD(a), SOC_PBMP_FMT(pbm_sw, pfmt), buf);
            format_bcm_pbmp(unit, buf, sizeof(buf), pbm_hw);
            cli_out("%s:   Hardware Port BitMap %s: %s\n",
                    ARG_CMD(a), SOC_PBMP_FMT(pbm_hw, pfmt), buf);
            BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
            BCM_PBMP_OR(pbm_temp, pbm_hw);
            BCM_PBMP_REMOVE(pbm_temp, PBMP_HG_SUBPORT_ALL(unit));
            BCM_PBMP_ASSIGN(pbm, pcfg.port);
            BCM_PBMP_XOR(pbm, pbm_temp);
            format_bcm_pbmp(unit, buf, sizeof(buf), pbm);
            cli_out("%s:   Disabled Port BitMap %s: %s\n",
                    ARG_CMD(a), SOC_PBMP_FMT(pbm, pfmt), buf);
        } else {
            cli_out("%s: Linkscan disabled\n", ARG_CMD(a));
        }

        return (CMD_OK);
    }

    us = us ? us : linkscan_interval_default;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "SwPortBitMap", PQ_PBMP | PQ_DFL | PQ_BCM, 0, &pbm_sw,
                    0);
    parse_table_add(&pt, "HwPortBitMap", PQ_PBMP | PQ_DFL | PQ_BCM, 0, &pbm_hw,
                    0);
    parse_table_add(&pt, "Force", PQ_PBMP | PQ_DFL | PQ_BCM, 0, &pbm_force, 0);
    parse_table_add(&pt, "Interval", PQ_INT | PQ_DFL, 0, &us, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return (CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    /*
     * Handle backward compatibility, allowing a raw interval to be
     * specified directly on the command line, as well as "on" or "off".
     */

    if (ARG_CUR(a) != NULL) {
        c = ARG_GET(a);

        if (!sal_strcasecmp(c, "off") ||
            !sal_strcasecmp(c, "disable") ||
            !sal_strcasecmp(c, "no")) {
            us = 0;
        } else if (!sal_strcasecmp(c, "on") ||
                   !sal_strcasecmp(c, "enable") ||
                   !sal_strcasecmp(c, "yes")) {
            us = prev_linkscan_interval[unit] ?
                 prev_linkscan_interval[unit] : linkscan_interval_default;
            if (us == 0) {
                cli_out("WARNING: Cannot enable Linkscan if the polling interval is zero\n");
            }
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx) && us && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM || SAL_BOOT_QUICKTURN)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT)
                soc_iproc_m0ssq_init(unit);
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
            }
#endif /* BCM_CMICX_SUPPORT */
        } else if (isint(c)) {
            us = parse_integer(c);
        } else {
            return (CMD_USAGE);
        }
    }

    if (us == 0) {
        int prev_interval;

        rv = bcm_linkscan_enable_get(unit, &prev_interval);
        if (rv < 0 || prev_interval <= 0) {
            prev_interval = linkscan_interval_default;
        }
        prev_linkscan_interval[unit] = prev_interval;
        /* Turn off linkscan */
        if ((rv = bcm_linkscan_enable_set(unit, 0)) < 0) {
            cli_out("%s: Error: Failed to disable linkscan: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }

        return (CMD_OK);
    }

    BCM_PBMP_AND(pbm_sw, pcfg.port);
    BCM_PBMP_AND(pbm_hw, pcfg.port);
    BCM_PBMP_ASSIGN(pbm_none, pcfg.port);
    BCM_PBMP_REMOVE(pbm_sw, PBMP_HG_SUBPORT_ALL(unit));
    BCM_PBMP_REMOVE(pbm_hw, PBMP_HG_SUBPORT_ALL(unit));
    BCM_PBMP_REMOVE(pbm_none, PBMP_HG_SUBPORT_ALL(unit));

    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
    BCM_PBMP_OR(pbm_temp, pbm_hw);
    BCM_PBMP_XOR(pbm_none, pbm_temp);
    BCM_PBMP_AND(pbm_force, pcfg.port);

    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
    BCM_PBMP_AND(pbm_temp, pbm_hw);
    if (BCM_PBMP_NOT_NULL(pbm_temp)) {
        cli_out("%s: Error: Same port can't use both "
                "software and hardware linkscan\n", ARG_CMD(a));
        return (CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_sw,
                                        BCM_LINKSCAN_MODE_SW)) < 0) {
        cli_out("%s: Failed to set software link scanning: PBM=%s: %s\n",
                ARG_CMD(a), SOC_PBMP_FMT(pbm_sw, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_hw,
                                        BCM_LINKSCAN_MODE_HW)) < 0) {
        cli_out("%s: Failed to set hardware link scanning: PBM=%s: %s\n",
                ARG_CMD(a), SOC_PBMP_FMT(pbm_hw, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    /* Only set the port to mode_none state if it is not previously in this mode */
    BCM_PBMP_ASSIGN(pbm_none_pre, pcfg.port);
    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw_pre);
    BCM_PBMP_OR(pbm_temp, pbm_hw_pre);
    BCM_PBMP_XOR(pbm_none_pre, pbm_temp);
    BCM_PBMP_XOR(pbm_none_pre, pbm_none);
    BCM_PBMP_AND(pbm_none, pbm_none_pre);       /* the one changed from 0 to 1 */

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_none,
                                        BCM_LINKSCAN_MODE_NONE)) < 0) {
        cli_out("%s: Failed to disable link scanning: PBM=%s: %s\n",
                ARG_CMD(a), SOC_PBMP_FMT(pbm_none, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if ((rv = bcm_linkscan_enable_set(unit, us)) < 0) {
        cli_out("%s: Error: Failed to enable linkscan: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if ((rv = bcm_link_change(unit, pbm_force)) < 0) {
        cli_out("%s: Failed to force link scan: PBM=%s: %s\n",
                ARG_CMD(a), SOC_PBMP_FMT(pbm_force, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return (CMD_OK);
}

#if defined(BCM_TOMAHAWK_SUPPORT)
cmd_result_t
if_esw_asf(int unit, args_t *a)
{
    int p, rv = BCM_E_NONE;
    char *arg;
    pbmp_t pbm;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
    if (!soc_feature(unit, soc_feature_asf_multimode)) {
        return CMD_NOTIMPL;
    }

    if ((arg = ARG_GET(a)) == NULL) {
        return (CMD_USAGE);
    }

    if (arg && !sal_strcasecmp(arg, "show")) {
        if ((arg = ARG_GET(a)) == NULL) {
            return (CMD_USAGE);
        }
        if (parse_bcm_pbmp(unit, arg, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), arg);
            return CMD_FAIL;
        }
        if (BCM_PBMP_IS_NULL(pbm)) {
            return CMD_OK;
        }
        cli_out("         cut-through      cut-through speed \n");
        cli_out("port        mode          range (from/to)\n");
        PBMP_ITER(pbm, p) {
            rv = bcm_esw_port_asf_show(unit, p);
        }
    } else if (arg && !sal_strcasecmp(arg, "diag")) {
        if ((arg = ARG_GET(a)) == NULL) {
            return (CMD_USAGE);
        }
        if (parse_bcm_pbmp(unit, arg, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), arg);
            return CMD_FAIL;
        }
        if (BCM_PBMP_IS_NULL(pbm)) {
            return CMD_OK;
        }

        rv = bcm_esw_asf_diag(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
        if(SOC_IS_TOMAHAWK3(unit)) {
            cli_out("%-5s %-3s %-3s %-6s %-6s %-3s %-32s ", "port", "asf",
                "ctc", "min_sp", "max_sp", "thr",
                 "        xmit_start_count");
            cli_out("%-3s, %-3s  \n", "emc", "rxp");
            cli_out("%-33s %-3s %-3s %-3s %-3s %-3s %-3s %-3s %-3s\n", " ",
                    "sc0", "sc1", "sc2", "sc3", "sc4", "sc5", "sc6", "sc7");

            PBMP_ITER(pbm, p) {
                rv = bcm_esw_port_asf_diag(unit, p);
            }

            cli_out("Legends:\n");
            cli_out("asf: CT Mode, ctc: CT Class, min: Min. Src Port Speed, "\
                "max: Max. Src Port Speed, thr: FIFO Threshold\n");
            cli_out("xmit_start_count: Transmit Start Count, "
                "sc$: Source Class \n");
            cli_out("emc: EGR MMU Credits, rxp: RX Pause\n");
        } else

#endif
        {
        cli_out("port  asf ctc min_sp max_sp thr dep                      "\
                "xmit_start_count                 mpb mec emc rxp\n");
        cli_out("                                     "\
                "sc0 sc1 sc2 sc3 sc4 sc5 sc6 sc7 sc8 sc9 s10 s11 s12\n");
        PBMP_ITER(pbm, p) {
            rv = bcm_esw_port_asf_diag(unit, p);
        }

        cli_out("Legends:\n");
        cli_out("asf: CT Mode, ctc: CT Class, min: Min. Src Port Speed, "\
                "max: Max. Src Port Speed, thr: FIFO Threshold\n");
        cli_out("dep: FIFO Depth, xmit_start_count: Transmit Start Count, "
                "sc$: Source Class, mpb: MMU Prebuffer\n");
        cli_out("mec: MMU EP Credits, emc: EGR MMU Credits, rxp: RX Pause\n");
        }
    } else {
        cli_out("error: incorrect argument\n");
    }

    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
}
#endif


/* Must stay in sync with bcm_color_t enum (bcm/types.h) */
const char *diag_parse_color[] = {
    "Green",
    "Yellow",
    "Red",
    NULL
};

char cmd_color_usage[] =
    "Usages:\n\t"
    "  color set Port=<port> Prio=<prio> CFI=<cfi>\n\t"
    "        Color=<Green|Yellow|Red>\n\t"
    "  color show Port=<port>\n\t"
    "  color map Port=<port> PktPrio=<prio> CFI=<cfi>\n\t"
    "        IntPrio=<prio> Color=<Green|Yellow|Red>\n\t"
    "  color unmap Port=<port> IntPrio=<prio> Color=<Green|Yellow|Red>\n\t"
    "        PktPrio=<prio> CFI=<cfi>\n";

cmd_result_t
cmd_color(int unit, args_t *a)
{
    int port = 0, prio = -1, cfi = -1, color_parse = bcmColorRed;
    bcm_color_t color;
    char *subcmd;
    int r;
    parse_table_t pt;
    cmd_result_t retCode;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "set") == 0) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "PRio", PQ_INT | PQ_DFL, 0, &prio, NULL);
        parse_table_add(&pt, "CFI", PQ_INT | PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "Color", PQ_MULTI | PQ_DFL, 0,
                        &color_parse, diag_parse_color);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        if (!SOC_PORT_VALID(unit, port)) {
            cli_out("%s: ERROR: Invalid port selection %d\n", ARG_CMD(a), port);
            return CMD_FAIL;
        }

        color = (bcm_color_t) color_parse;

        if (prio < 0) {
            if (cfi < 0) {
                /* No selection to assign color */
                cli_out("%s: ERROR: No parameter to assign color\n", ARG_CMD(a));
                return CMD_FAIL;
            } else {
                if ((r = bcm_port_cfi_color_set(unit, port, cfi, color)) < 0) {
                    goto bcm_err;
                }
            }
        } else {
            if (cfi < 0) {
                if (prio > BCM_PRIO_MAX) {
                    cli_out("%s: ERROR: Priority %d exceeds maximum\n",
                            ARG_CMD(a), prio);
                    return CMD_FAIL;
                } else {
                    if ((r = bcm_port_priority_color_set(unit, port, prio,
                                                         color)) < 0) {
                        goto bcm_err;
                    }
                }
            }

        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &port, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        if (!SOC_PORT_VALID(unit, port)) {
            cli_out("%s: ERROR: Invalid port selection %d\n", ARG_CMD(a), port);
            return CMD_FAIL;
        }

        cli_out("Color settings for port %s\n", BCM_PORT_NAME(unit, port));
        for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
            if ((r = bcm_port_priority_color_get(unit, port, prio, &color)) < 0) {
                goto bcm_err;
            }
            cli_out("Priority %d\t%s\n", prio, diag_parse_color[color]);
        }

        if ((r = bcm_port_cfi_color_get(unit, port, FALSE, &color)) < 0) {
            goto bcm_err;
        }
        cli_out("No CFI     \t%s\n", diag_parse_color[color]);

        if ((r = bcm_port_cfi_color_get(unit, port, TRUE, &color)) < 0) {
            goto bcm_err;
        }
        cli_out("CFI        \t%s\n", diag_parse_color[color]);
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "map") == 0) {
        int pkt_prio, int_prio;

        pkt_prio = int_prio = -1;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "PktPrio", PQ_INT | PQ_DFL, 0, &pkt_prio, NULL);
        parse_table_add(&pt, "CFI", PQ_INT | PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "IntPrio", PQ_INT | PQ_DFL, 0, &int_prio, NULL);
        parse_table_add(&pt, "Color", PQ_MULTI | PQ_DFL, 0,
                        &color_parse, diag_parse_color);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        if (!SOC_PORT_VALID(unit, port)) {
            cli_out("%s: ERROR: Invalid port selection %d\n", ARG_CMD(a), port);
            return CMD_FAIL;
        }

        if (pkt_prio < 0 || cfi < 0 || int_prio < 0) {
            cli_out("Color map settings for port %s\n",
                    BCM_PORT_NAME(unit, port));

            for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
                for (cfi = 0; cfi <= 1; cfi++) {
                    if ((r = bcm_port_vlan_priority_map_get(unit, port,
                                                            prio, cfi,
                                                            &int_prio,
                                                            &color)) < 0) {
                        goto bcm_err;
                    }
                    cli_out("Packet Prio=%d, CFI=%d, Internal Prio=%d, "
                            "Color=%s\n",
                            prio, cfi, int_prio, diag_parse_color[color]);
                }
            }
        } else {
            color = (bcm_color_t) color_parse;
            if ((r = bcm_port_vlan_priority_map_set(unit, port, pkt_prio, cfi,
                                                    int_prio, color)) < 0) {
                goto bcm_err;
            }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "unmap") == 0) {
        int pkt_prio, int_prio;

        pkt_prio = int_prio = -1;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "PktPrio", PQ_INT | PQ_DFL, 0, &pkt_prio, NULL);
        parse_table_add(&pt, "CFI", PQ_INT | PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "IntPrio", PQ_INT | PQ_DFL, 0, &int_prio, NULL);
        parse_table_add(&pt, "Color", PQ_MULTI | PQ_DFL, 0,
                        &color_parse, diag_parse_color);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        if (!SOC_PORT_VALID(unit, port)) {
            cli_out("%s: ERROR: Invalid port selection %d\n", ARG_CMD(a), port);
            return CMD_FAIL;
        }

        if (pkt_prio < 0 || cfi < 0 || int_prio < 0) {
            cli_out("Color unmap settings for port %s\n",
                    BCM_PORT_NAME(unit, port));

            for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
                for (color = bcmColorGreen; color <= bcmColorRed; color++) {
                    if ((r = bcm_port_vlan_priority_unmap_get(unit, port,
                                                              prio, color,
                                                              &pkt_prio,
                                                              &cfi)) < 0) {
                        goto bcm_err;
                    }
                    cli_out("Internal Prio=%d, Color=%s, Packet Prio=%d, "
                            "CFI=%d\n",
                            prio, diag_parse_color[color], pkt_prio, cfi);
                }
            }
        } else {
            color = (bcm_color_t) color_parse;
            if ((r = bcm_port_vlan_priority_unmap_set(unit, port, int_prio,
                                                      color, pkt_prio,
                                                      cfi)) < 0) {
                goto bcm_err;
            }
        }

        return CMD_OK;

    }

    cli_out("%s: ERROR: Unknown color subcommand: %s\n", ARG_CMD(a), subcmd);

    return CMD_USAGE;

 bcm_err:

    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));

    return CMD_FAIL;
}

#ifdef BCM_TRIUMPH3_SUPPORT
char ibod_sync_usage[] =
    "Parameters: [on] [off] [Interval=<interval>] [tmmu]\n\t"
    "Starts the IBOD recovery thread running every <interval>\n\t"
    "microseconds. If <interval> is 0, stops the task.\n\t"
    "If <interval> is omitted, prints current setting.\n\t"
    "<tmmu> is used to trigger mmu recovery.\n";

cmd_result_t
cmd_ibod_sync(int unit, args_t *a)
{
    parse_table_t pt;
    int r=BCM_E_NONE, ibod_running_flag;
    sal_usecs_t usec, port_avg_time, txerr_avg_time, mmu_avg_time;
    uint32 event_count, txerr_count, mmu_war_count;
    uint32 drain_timeout_count, egress_drain_timeout_count, port_recovery_failed_count;
    _port_ibod_ctrl_t *ibod_ctrl;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!SOC_IS_TRIUMPH3(unit)) {
        cli_out("%s: IBOD sync is not needed\n", ARG_CMD(a));
        return CMD_OK;
    }

    ibod_ctrl = sal_alloc(sizeof(_port_ibod_ctrl_t), "_port_ibod_ctrl_t");
    if (ibod_ctrl == NULL) {
        return CMD_FAIL;
    }
    sal_memset(ibod_ctrl, 0, sizeof(_port_ibod_ctrl_t));

    if (_bcm_esw_ibod_sync_recovery_running(unit, ibod_ctrl) < 0) {
        ibod_running_flag = 0;
        usec = 0;
        port_avg_time = 0;
        txerr_avg_time = 0;
        mmu_avg_time = 0;
        event_count = 0;
        txerr_count = 0;
        mmu_war_count = 0;
        drain_timeout_count = 0;
        egress_drain_timeout_count = 0;
        port_recovery_failed_count = 0;
    }
    else {
        ibod_running_flag = 1;
        usec = ibod_ctrl->interval;
        port_avg_time = ibod_ctrl->port_avg_time;
        txerr_avg_time = ibod_ctrl->txerr_avg_time;
        mmu_avg_time = ibod_ctrl->mmu_avg_time;
        event_count = ibod_ctrl->event_count;
        txerr_count = ibod_ctrl->txerr_count;
        mmu_war_count = ibod_ctrl->mmu_war_count;
        drain_timeout_count = ibod_ctrl->drain_timeout_count;
        egress_drain_timeout_count = ibod_ctrl->egress_drain_timeout_count;
        port_recovery_failed_count = ibod_ctrl->port_recovery_failed_count;
    }

    sal_free(ibod_ctrl);
    ibod_ctrl = NULL;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_DFL | PQ_INT, (void *)0, &usec, NULL);

    if (!ARG_CNT(a)) {          /* Display settings */
        cli_out("Current settings (unit %d):\n", unit);
        parse_eq_format(&pt);
        cli_out("    IBOD sync event count: %d\n", event_count);
        cli_out("    IBOD sync average time: %d us\n", port_avg_time);
        cli_out("    Txerr count: %d\n", txerr_count);
        cli_out("    Txerr average time: %d us\n", txerr_avg_time);
        cli_out("    MMU count: %d\n", mmu_war_count);
        cli_out("    MMU average time: %d us\n", mmu_avg_time);
        cli_out("    Drain timeout count: %d\n", drain_timeout_count);
        cli_out("    Egress drain timeout count: %d\n", egress_drain_timeout_count);
        cli_out("    Port recovery failed count: %d\n", port_recovery_failed_count);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "tmmu")) {
        ARG_NEXT(a);
        _bcm_esw_ibod_debug_flag_set(unit, _PORT_IBOD_FLAG_MMU_TRIGGER);
        return CMD_OK;
    }

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "on")) {
        ARG_NEXT(a);
       if (ibod_running_flag) {
            return CMD_OK;
        }
        if (usec == 0) {
            usec = soc_property_get(unit, spn_PORTMON_INTERVAL, IBOD_SYNC_INTERVAL_MIN);
        }
    }
    else if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "off")) {
        usec = 0;
        ARG_NEXT(a);
    }
    else if (ARG_CNT(a) > 0) {
        return CMD_USAGE;
    }

    if (usec > 0) {
        r = _bcm_esw_ibod_sync_recovery_start(unit, usec);
    } else {
        r = _bcm_esw_ibod_sync_recovery_stop(unit);
    }

    if (r < 0) {
        cli_out("%s: Error: Could not set IBOD SYNCe: %s\n",
                ARG_CMD(a), soc_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}
#endif                          /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT

#define MAX_PM_NUM 64

/* Max number of PMs per device */
#define PM_NUM(unit) (SOC_IS_APACHE(unit)    ? 18 : \
                      SOC_IS_TOMAHAWK2(unit) ? 64 : \
                      SOC_IS_TRIDENT3(unit) ? 32 : \
                      SOC_IS_TOMAHAWK3(unit) ? 32 : \
                      SOC_IS_HELIX5(unit) ? 19 : \
                      SOC_IS_FIREBOLT6(unit) ? 16 : \
                      SOC_IS_HURRICANE4(unit) ? 16 : \
                      SOC_IS_MAVERICK2(unit) ? 20 : \
                      0)

#define FOUR_PORTS_PER_PBLK  4
#define EIGHT_PORTS_PER_PBLK 8
#define MAX_PORTS_PER_PBLK   EIGHT_PORTS_PER_PBLK

typedef struct flexport_pm_info_s {
    /* Info get from cli input*/
    char *pm_str;
    int ports[MAX_PORTS_PER_PBLK];
    int port_cnt;

    /* Parsed info */
    bcm_port_resource_t pr[MAX_PORTS_PER_PBLK];
    uint32 flags;
#define FLXP_F_SEEN                 0x1 /* PM seen without mode/port config */
#define FLXP_F_PARSED               0x2 /* PM with config is parsed */
#define FLXP_F_PORT_ASSIGNED        0x4 /* Logical ports have been assigned */
} flexport_pm_info_t;

typedef struct flexport_info_s {
    flexport_pm_info_t pm[MAX_PM_NUM];
    int add_cnt;
    int del_cnt;
} flexport_info_t;

/*
 * Function:
 *    flexport_max_ports_per_pblk_get
 * Purpose:
 *    Get max number of physical ports per port macro
 * Parameters:
 *    unit(IN) -       Strata switch unit number
 *    max_ports(OUT) - Buffer to return value in to
 * Returns:
 *    None
 */
static void
flexport_max_ports_per_pblk_get(int unit, int *max_ports)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        *max_ports = EIGHT_PORTS_PER_PBLK;
    } else
#endif
    {
        *max_ports = FOUR_PORTS_PER_PBLK;
    }
}

static cmd_result_t
flexport_show(int unit, flexport_info_t *f)
{
    int pm, port, phy_port, phy_base, i;
    bcm_port_resource_t pr;
    char buf[6];
    int max_ports = 0;

    flexport_max_ports_per_pblk_get(unit, &max_ports);

    for (pm = 0; pm < PM_NUM(unit) ; pm++) {
        if (!(f->pm[pm].flags & FLXP_F_SEEN)) {
            continue;
        }
        cli_out("PM%d:\n", pm);
        phy_base = (pm * max_ports) + 1;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1) {
                bcm_port_resource_t_init(&pr);
                if (bcm_port_resource_get(unit, port, &pr) < 0) {
                    cli_out("Flexport: Failed to get port resource of %s.\n",
                            BCM_PORT_NAME(unit, port));
                    return CMD_FAIL;
                }
                cli_out("\t%s   Port(%d) PhysicalPort(%d) Speed(%s) "
                        "Lanes(%d) Encap(%s)\n",
                        BCM_PORT_NAME(unit, port), port, pr.physical_port,
                        if_fmt_speed(buf, pr.speed), pr.lanes,
                        encap_mode[pr.encap]);
            }
        }
    }

    return CMD_OK;
}

static cmd_result_t
flexport_parse_portlist(int unit, flexport_pm_info_t *pi)
{
    int pm, p, pfirst, plast, i;
    char *s, *sn, *pl;
    int max_ports = 0;

    /* no logical ports info for new ports  from cmdline, so no parsing */
    if (SOC_IS_TRIDENT3(unit)) {
        return CMD_OK;
    }

    flexport_max_ports_per_pblk_get(unit, &max_ports);

    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (pi[pm].pm_str == NULL) {
            continue;
        }
        pl = sal_strchr(pi[pm].pm_str, ':');
        if (pl) {
            s = pl+1;
            pfirst = -1;
            i = 0;
            while (*s) {
                sn = s;
                if (isdigit((unsigned)*sn)) {
                    p = 0;
                    do {
                        p = p * 10 + (*sn - '0');
                        sn++;
                    } while (isdigit((unsigned)*sn));
                } else {
                    p = -1;
                }
                plast = p;

                if (*sn == '-') {
                    if (plast < 0) {
                        return CMD_FAIL;
                    } else if(pfirst < 0) {
                        pfirst = plast;
                    } else {
                        return CMD_FAIL;
                    }
                } else if (*sn == ',' || *sn == '\0') {
                    if (pfirst < 0) {
                        pfirst = plast;
                    }
                    for (p = pfirst; p <= plast; p++) {  /* a range */
                        if (i >= max_ports) {
                            return CMD_FAIL;
                        }
                        pi[pm].ports[i] = p;
                        i++;
                    }
                    if (*sn == '\0') {
                        break;
                    }
                    pfirst = -1;
                } else {
                    return CMD_FAIL;
                }
                s = sn + 1;
            }
            if (i !=0 ) {
                pi[pm].port_cnt = i;
                pi[pm].flags |= FLXP_F_PORT_ASSIGNED;
            }
            *pl = '\0';
        }
    }
    return CMD_OK;
}

#ifndef MAX
#define	MAX(a, b)		(((a)>(b))?(a):(b))
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
static cmd_result_t
th3_flexport_parse_pm_mode(int unit, flexport_info_t *f, int *auto_assign)
{
    flexport_pm_info_t *pi = f->pm;
    bcm_port_resource_t *pr;
    int pm, phy_base, phy_port, port, i;
    int req_cnt;
    char *s;
    bcm_port_encap_t encap;
    int max_ports = 0;

#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    portmod_dispatch_type_t pm_type;
    int rv = BCM_E_NONE;
#else
    /* For non-8x50 pm's, this command is not supported, return error */
    return CMD_NOTIMPL;
#endif

    flexport_max_ports_per_pblk_get(unit, &max_ports);

    f->del_cnt = 0;
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (pi[pm].pm_str == NULL) {
            continue;
        }
        s = pi[pm].pm_str;
        pr = pi[pm].pr;
        phy_base = pm * max_ports + 1;

#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
        /*
         * Check for pm type. If its not 8x50, return error
         */
        rv = portmod_phy_pm_type_get(unit, phy_base, &pm_type);

        if (rv != BCM_E_NONE) {
            cli_out("Error: portmod_phy_pm_type_get returned error %d"
                    " for pm%d (%d:%d).\n", rv, pm, phy_base,
                    SOC_INFO(unit).port_p2l_mapping[phy_base]);
            return CMD_FAIL;
        }

        if (pm_type != portmodDispatchTypePm8x50) {
            cli_out("Error: Unsupported pm type %d, pm%d.\n", pm_type, pm);
            return CMD_FAIL;
        }
#endif
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1) {
                f->del_cnt ++;
            }
        }

        encap = BCM_PORT_ENCAP_IEEE;

        if (!sal_strncasecmp(s, "1x400G", MAX(strlen(s), 6))) {
            req_cnt = 1;
            if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                pr[0].port = pi[pm].ports[0];
            }
            pr[0].physical_port = phy_base;
            pr[0].speed = 400000;
            pr[0].lanes = 8;
            pr[0].encap = encap;
            pr[0].fec_type = bcmPortPhyFecRs544_2xN;
            pr[0].phy_lane_config = -1;
            pr[0].link_training = 0;
        } else if(!sal_strncasecmp(s, "2x200G", MAX(strlen(s), 6))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 200000;
                pr[i].lanes = 4;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecRs544_2xN;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if (!sal_strncasecmp(s, "2x100G", MAX(strlen(s), 6))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 100000;
                pr[i].lanes = 4;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecNone;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if (!sal_strncasecmp(s, "2x40G", MAX(strlen(s), 5))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 40000;
                pr[i].lanes = 4;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecNone;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x100G", MAX(strlen(s), 6))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 100000;
                pr[i].lanes = 2;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecRs544;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x50G", MAX(strlen(s), 5))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 50000;
                pr[i].lanes = 2;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecRs544;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x40G", MAX(strlen(s), 5))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 40000;
                pr[i].lanes = 2;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecNone;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "8x50G", MAX(strlen(s), 5))) {
            req_cnt = 8;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 50000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecRs544;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "8x25G", MAX(strlen(s), 5))) {
            req_cnt = 8;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 25000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecNone;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "8x10G", MAX(strlen(s), 5))) {
            req_cnt = 8;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 10000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                pr[i].fec_type = bcmPortPhyFecNone;
                pr[i].phy_lane_config = -1;
                pr[i].link_training = 0;
                phy_base += pr[i].lanes;
            }
        } else if (!sal_strncasecmp(s, "None", MAX(strlen(s), 4))) {
            /* Clear ports on a Port Macro, No add entries */
            req_cnt = 0;
        } else {
            cli_out("Flexport: Error: Unsupported mode %s on pm%d.\n",
                    s, pm);
            return CMD_FAIL;
        }

        if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
            if (pi[pm].port_cnt != req_cnt) {
                cli_out("Flexport: Error: pm%d %s requires %d ports.\n",
                        pm, s, req_cnt);
                return CMD_FAIL;
            }
        } else {
            pi[pm].port_cnt = req_cnt;
            if (req_cnt != 0) {
                *auto_assign |= 1;
            }
        }
        f->add_cnt += req_cnt;
    }

    return CMD_OK;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

static cmd_result_t
flexport_parse_pm_mode(int unit, flexport_info_t *f, int *auto_assign)
{
    flexport_pm_info_t *pi = f->pm;
    bcm_port_resource_t *pr;
    int pm, phy_base, phy_port, port, i, hg_speed;
    int req_cnt;
    char *s;
    bcm_port_encap_t encap;
    int max_ports = 0;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return th3_flexport_parse_pm_mode(unit, f, auto_assign);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    flexport_max_ports_per_pblk_get(unit, &max_ports);

    f->del_cnt = 0;
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (pi[pm].pm_str == NULL) {
            continue;
        }
        s = pi[pm].pm_str;
        pr = pi[pm].pr;
        phy_base = pm * max_ports + 1;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1) {
                f->del_cnt ++;
            }
        }

        /* Add entries info */
        if (sal_strstr(s, "HG") || sal_strstr(s, "hg")) {
            encap = BCM_PORT_ENCAP_HIGIG2;
        } else {
            encap = BCM_PORT_ENCAP_IEEE;
        }

        if (sal_strstr(s, "106") || sal_strstr(s, "53") ||
            sal_strstr(s, "42") || sal_strstr(s, "27") ||
            sal_strstr(s, "21") || sal_strstr(s, "11")) {
            hg_speed = 1;
        } else {
            hg_speed = 0;
        }

        if (!sal_strncasecmp(s, "1x100G", MAX(strlen(s), 6)) ||
            !sal_strncasecmp(s, "1xHG[106]", MAX(strlen(s), 9)) ||
            !sal_strncasecmp(s, "1xHG[100]", MAX(strlen(s), 9))) {
            req_cnt = 1;
            if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                pr[0].port = pi[pm].ports[0];
            }
            pr[0].physical_port = phy_base;
            pr[0].speed = hg_speed ? 106000 : 100000;
            pr[0].lanes = 4;
            pr[0].encap = encap;
        } else if(!sal_strncasecmp(s, "1x40G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "1xHG[42]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "1xHG[40]", MAX(strlen(s), 8))) {
            req_cnt = 1;
            if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                pr[0].port = pi[pm].ports[0];
            }
            pr[0].physical_port = phy_base;
            pr[0].speed = hg_speed ? 42000 : 40000;
            pr[0].lanes = 4;
            pr[0].encap = encap;
        } else if(!sal_strncasecmp(s, "2x50G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "2xHG[53]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "2xHG[50]", MAX(strlen(s), 8))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = hg_speed ? 53000 : 50000;
                pr[i].lanes = 2;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "2x40G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "2xHG[42]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "2xHG[40]", MAX(strlen(s), 8))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = hg_speed ? 42000 : 40000;
                pr[i].lanes = 2;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "2x20G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "2xHG[21]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "2xHG[20]", MAX(strlen(s), 8))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = hg_speed ? 21000 : 20000;
                pr[i].lanes = 2;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "1x40G+1x20G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[42]+1xHG[21]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[40]+1xHG[20]", MAX(strlen(s), 17))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 42000 : 40000;
                } else {
                    pr[i].speed = hg_speed ? 21000 : 20000;
                }
                pr[i].lanes = 2;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "1x20G+1x40G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[21]+1xHG[42]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[20]+1xHG[40]", MAX(strlen(s), 17))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 21000 : 20000;
                } else {
                    pr[i].speed = hg_speed ? 42000 : 40000;
                }
                pr[i].lanes = 2;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "1x25G+1x50G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[27]+1xHG[53]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[25]+1xHG[50]", MAX(strlen(s), 17))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 27000 : 25000;
                    pr[i].lanes = 1;
                    pr[i].flags |= BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR;
                    phy_base += 2;
                } else {
                    pr[i].speed = hg_speed ? 53000 : 50000;
                    pr[i].lanes = 2;
                    phy_base += pr[i].lanes;
                }
                pr[i].encap = encap;
            }
        } else if(!sal_strncasecmp(s, "1x50G+1x25G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[53]+1xHG[27]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[50]+1xHG[25]", MAX(strlen(s), 17))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 53000 : 50000;
                    pr[i].lanes = 2;
                    phy_base += pr[i].lanes;
                } else {
                    pr[i].speed = hg_speed ? 27000 : 25000;
                    pr[i].lanes = 1;
                    pr[i].flags |= BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR;
                    phy_base += 2;
                }
                pr[i].encap = encap;
            }
        }else if(!sal_strncasecmp(s, "2x25G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "2xHG[27]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "2xHG[25]", MAX(strlen(s), 8))) {
            req_cnt = 2;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = hg_speed ? 27000 : 25000;
                pr[i].lanes = 1;
                pr[i].flags |= BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR;
                phy_base += 2;
                pr[i].encap = encap;
            }
        }else if(!sal_strncasecmp(s, "1x50G+2x25G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[53]+2xHG[27]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[50]+2xHG[25]", MAX(strlen(s), 17))) {
            req_cnt = 3;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 53000 : 50000;
                    pr[i].lanes = 2;
                } else {
                    pr[i].speed = hg_speed ? 27000 : 25000;
                    pr[i].lanes = 1;
                }
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "2x25G+1x50G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "2xHG[27]+1xHG[53]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "2xHG[25]+1xHG[50]", MAX(strlen(s), 17))) {
            req_cnt = 3;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 2) {
                    pr[i].speed = hg_speed ? 53000 : 50000;
                    pr[i].lanes = 2;
                } else {
                    pr[i].speed = hg_speed ? 27000 : 25000;
                    pr[i].lanes = 1;
                }
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "1x40G+2x10G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[42]+2xHG[11]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[40]+2xHG[10]", MAX(strlen(s), 17))) {
            req_cnt = 3;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 42000 : 40000;
                    pr[i].lanes = 2;
                } else {
                    pr[i].speed = hg_speed ? 11000 : 10000;
                    pr[i].lanes = 1;
                }
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "2x10G+1x40G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "2xHG[11]+1xHG[42]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "2xHG[10]+1xHG[40]", MAX(strlen(s), 17))) {
            req_cnt = 3;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 2) {
                    pr[i].speed = hg_speed ? 42000 : 40000;
                    pr[i].lanes = 2;
                } else {
                    pr[i].speed = hg_speed ? 11000 : 10000;
                    pr[i].lanes = 1;
                }
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "1x20G+2x10G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "1xHG[21]+2xHG[11]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "1xHG[20]+2xHG[10]", MAX(strlen(s), 17))) {
            req_cnt = 3;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 0) {
                    pr[i].speed = hg_speed ? 21000 : 20000;
                    pr[i].lanes = 2;
                } else {
                    pr[i].speed = hg_speed ? 11000 : 10000;
                    pr[i].lanes = 1;
                }
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "2x10G+1x20G", MAX(strlen(s), 11)) ||
                  !sal_strncasecmp(s, "2xHG[11]+1xHG[21]", MAX(strlen(s), 17)) ||
                  !sal_strncasecmp(s, "2xHG[10]+1xHG[20]", MAX(strlen(s), 17))) {
            req_cnt = 3;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                if (i == 2) {
                    pr[i].speed = hg_speed ? 21000 : 20000;
                    pr[i].lanes = 2;
                } else {
                    pr[i].speed = hg_speed ? 11000 : 10000;
                    pr[i].lanes = 1;
                }
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x25G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "4xHG[27]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "4xHG[25]", MAX(strlen(s), 8))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = hg_speed ? 27000 : 25000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x10G", MAX(strlen(s), 5)) ||
                  !sal_strncasecmp(s, "4xHG[11]", MAX(strlen(s), 8)) ||
                  !sal_strncasecmp(s, "4xHG[10]", MAX(strlen(s), 8))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = hg_speed ? 11000 : 10000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x1G", MAX(strlen(s), 4))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 1000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x5G", MAX(strlen(s), 4))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 5000;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "4x2.5G", MAX(strlen(s), 6))) {
            req_cnt = 4;
            for (i = 0; i < req_cnt; i++) {
                if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                    pr[i].port = pi[pm].ports[i];
                }
                pr[i].physical_port = phy_base;
                pr[i].speed = 2500;
                pr[i].lanes = 1;
                pr[i].encap = encap;
                phy_base += pr[i].lanes;
            }
        } else if(!sal_strncasecmp(s, "None", MAX(strlen(s), 4))) {
            /* Clear ports on a Port Macro, No add entries */
            req_cnt = 0;
        } else {
            cli_out("Flexport: Error: Unsupported mode %s on pm%d.\n",
                    s, pm);
            return CMD_FAIL;
        }
       /* TD3 */
        if (SOC_IS_TRIDENT3(unit)) {
            for (i = 0; i < req_cnt; i++) {
                pr[i].port = pr[i].physical_port > 64 ? (2 + pr[i].physical_port) : pr[i].physical_port;
            }
            pi[pm].flags |= FLXP_F_PORT_ASSIGNED;
            pi[pm].port_cnt = req_cnt;
        }

        if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
            if (pi[pm].port_cnt != req_cnt) {
                cli_out("Flexport: Error: pm%d %s requires %d ports.\n",
                        pm, s, req_cnt);
                return CMD_FAIL;
            }
        } else {
            pi[pm].port_cnt = req_cnt;
            if (req_cnt != 0) {
                *auto_assign |= 1;
            }
        }
        f->add_cnt += req_cnt;
    }

    return CMD_OK;
}

static cmd_result_t
flexport_port_assign(int unit, flexport_info_t *f)
{
    bcm_pbmp_t pipe_pbmp[SOC_MAX_NUM_PIPES]; /* Free-port pool for each pipe */
    int i, pm, port, phy_base, phy_port, pipe, cnt, pms_per_pipe;
    int ffport[SOC_MAX_NUM_PIPES]; /* First free port in each pipe */
    int max_ports = 0;
    typedef struct pbs_s {
        int start_port;
        int end_port;
    } pbs_t;

    /* General port boundary for each pipe */
    STATIC pbs_t pbs_tomahawk2[] = {
        { 1, 32 },
        { 34, 65 },
        { 68, 99 },
        { 102, 133 }
    };
    STATIC pbs_t pbs_apache[] = {
        { 1, 72 }
    };
    STATIC pbs_t pbs_helix5[] = {
        { 1, 69 }
    };

    STATIC pbs_t pbs_firebolt6[] = {
        { 1, 68 }
    };
    STATIC pbs_t pbs_hurricane4[] = {
        { 1, 64 }
    };
    STATIC pbs_t pbs_maverick2[] = {
        { 1, 64 }
    };
    STATIC pbs_t pbs_tomahawk3[] = {
        { 1, 18 },
        { 20, 37 },
        { 40, 57 },
        { 60, 77 },
        { 80, 97 },
        { 100, 117 },
        { 120, 137 },
        { 140, 157 }
    };
    STATIC pbs_t *pbs;
    if (SOC_IS_APACHE(unit)) {
        pbs = pbs_apache;
    } else if (SOC_IS_TOMAHAWK2(unit)) {
        pbs = pbs_tomahawk2;
    } else if (SOC_IS_HELIX5(unit)) {
        pbs = pbs_helix5;
    } else if (SOC_IS_FIREBOLT6(unit)) {
        pbs = pbs_firebolt6;
    } else if (SOC_IS_HURRICANE4(unit)) {
        pbs = pbs_hurricane4;
    } else if (SOC_IS_MAVERICK2(unit)) {
        pbs = pbs_maverick2;
    } else if (SOC_IS_TOMAHAWK3(unit)) {
        pbs = pbs_tomahawk3;
    } else {
        return CMD_NOTIMPL;
    }
    pms_per_pipe = PM_NUM(unit)/NUM_PIPE(unit);

    flexport_max_ports_per_pblk_get(unit, &max_ports);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        SOC_PBMP_CLEAR(pipe_pbmp[pipe]);
        if (SOC_IS_APACHE(unit)) {
            SOC_PBMP_PORTS_RANGE_ADD(pipe_pbmp[pipe], pbs[pipe].start_port, 72);
        } else if (SOC_IS_HELIX5(unit)) {
            SOC_PBMP_PORTS_RANGE_ADD(pipe_pbmp[pipe], pbs[pipe].start_port, 69);
        } else if (SOC_IS_FIREBOLT6(unit)) {
            SOC_PBMP_PORTS_RANGE_ADD(pipe_pbmp[pipe], pbs[pipe].start_port, 68);
        } else if (SOC_IS_HURRICANE4(unit)) {
            SOC_PBMP_PORTS_RANGE_ADD(pipe_pbmp[pipe], pbs[pipe].start_port, 64);
        } else if (SOC_IS_MAVERICK2(unit)) {
            SOC_PBMP_PORTS_RANGE_ADD(pipe_pbmp[pipe], pbs[pipe].start_port, 64);
        } else {
            SOC_PBMP_PORTS_RANGE_ADD(pipe_pbmp[pipe], pbs[pipe].start_port, 32);
        }
        ffport[pipe] = pbs[pipe].start_port;
    }

    for (i = 0; i < NUM_PIPE(unit); i++) {
        SOC_PBMP_REMOVE(pipe_pbmp[i], SOC_INFO(unit).pipe_pbm[i]);
    }

    /* Existing ports need to be removed first during flexport,
     * thus, add these removed ports to free-port pool */
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (f->pm[pm].pm_str == NULL) {
            continue;
        }
        phy_base = pm * max_ports + 1;
        pipe = pm/pms_per_pipe;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1) {
                SOC_PBMP_PORT_ADD(pipe_pbmp[pipe], port);
            }
        }
    }

    /* Remove ports that specified by CLI from free-port pool */
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (f->pm[pm].pm_str == NULL ||
            !(f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        pipe = pm/pms_per_pipe;
        for (i = 0; i < f->pm[pm].port_cnt; i++) {
            SOC_PBMP_PORT_REMOVE(pipe_pbmp[pipe], f->pm[pm].ports[i]);
        }
    }

    /* Keep the same logical port if possible */
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if ((f->pm[pm].pm_str == NULL) ||
            (f->pm[pm].port_cnt == 0) || /* mode is "None" */
            (f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        pipe = pm/pms_per_pipe;
        cnt = 0;
        for (i = 0; i < f->pm[pm].port_cnt; i++ ) {
            phy_port = f->pm[pm].pr[i].physical_port;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1 &&
                SOC_PBMP_MEMBER(pipe_pbmp[pipe], port) &&
                SOC_INFO(unit).port_num_lanes[port] == f->pm[pm].pr[i].lanes) {
                f->pm[pm].pr[i].port = port;
                SOC_PBMP_PORT_REMOVE(pipe_pbmp[pipe], port);
                cnt++;
            } else {
                f->pm[pm].pr[i].port = -1;
            }
        }
        /* Set FLXP_F_PORT_ASSIGNED if all ports in the port macro keep using
         * previous port number. */
        if (cnt == f->pm[pm].port_cnt) {
            f->pm[pm].flags |= FLXP_F_PORT_ASSIGNED;
        }
    }

    /* Assign port from free-port pool */
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if ((f->pm[pm].pm_str == NULL) ||
            (f->pm[pm].port_cnt == 0) || /* mode is "None" */
            (f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        pipe = pm/pms_per_pipe;
        for (i = 0; i < f->pm[pm].port_cnt; i++ ) {
            if ( f->pm[pm].pr[i].port != -1) {
                continue;
            }
            for (port = ffport[pipe]; port <= pbs[pipe].end_port;
                 port++) {
                if (SOC_PBMP_MEMBER(pipe_pbmp[pipe], port)) {
                    f->pm[pm].pr[i].port = port;
                    SOC_PBMP_PORT_REMOVE(pipe_pbmp[pm/pms_per_pipe], port);
                    break;
                }
            }
            if (port > pbs[pipe].end_port) {
                cli_out("No free port.");
                return CMD_FAIL;
            } else {
                ffport[pipe] = port + 1;;
            }
            f->pm[pm].flags |= FLXP_F_PORT_ASSIGNED;
        }
    }
    return CMD_OK;
}

static cmd_result_t
flexport_diag(int unit, flexport_info_t *f)
{
    bcm_port_resource_t *pr;
    int phy_port, phy_base, pm, port ;
    int nports, pr_idx, len, i;
    cmd_result_t retCode = CMD_OK;
    int rv = BCM_E_NONE;
    int max_ports = 0;

    nports = f->del_cnt + f->add_cnt;
    if (nports > MAX_NUM_FLEXPORTS) {
        cli_out("Flexport: Entry num exceeds the Maxium(256).\n");
        return CMD_FAIL;
    }

    pr = sal_alloc(MAX_NUM_FLEXPORTS * sizeof(bcm_port_resource_t),
                   "PortResource");
    if (pr == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }
    sal_memset(pr, 0, MAX_NUM_FLEXPORTS * sizeof(bcm_port_resource_t));

    flexport_max_ports_per_pblk_get(unit, &max_ports);

    pr_idx = 0;

    /* Del entries */
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (f->pm[pm].pm_str == NULL) {
            continue;
        }
        phy_base = (pm * max_ports) + 1;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1) {
               if (SOC_IS_TRIDENT3(unit)) {
                   if ((phy_port == 128) && SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), 130)) {
                       nports--;
                       continue;
                   }
                }
                pr[pr_idx].port = port;
                pr[pr_idx].physical_port = -1;
                pr_idx++;
            }
        }
    }

    /* Add entries */
    for (pm = 0; pm < PM_NUM(unit); pm++) {
        if (f->pm[pm].pm_str  == NULL ||
            !(f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        len = sizeof(bcm_port_resource_t) * f->pm[pm].port_cnt;
        sal_memcpy(&pr[pr_idx], f->pm[pm].pr, len);
        pr_idx += f->pm[pm].port_cnt;
    }

    if (pr_idx != nports){
        cli_out("Flexport: Unmatched entry num and nports.\n");
        sal_free(pr);
        return CMD_FAIL;
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "\nCalling bcm_port_resource_multi:")));
    for (i = 0; i < nports; i ++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: physical_port = %0d"),
                             i, pr[i].physical_port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: port  = %0d"),
                             i, pr[i].port));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: speed = %0d"),
                             i, pr[i].speed));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: lanes = %0d"),
                             i, pr[i].lanes));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "\nresource[%0d]: encap = %0d"),
                             i, pr[i].encap));
    }

    rv = bcm_port_resource_multi_set(unit, nports, pr);
    if (BCM_FAILURE(rv)) {
        retCode = CMD_FAIL;
    } else {
        retCode = CMD_OK;
    }

    sal_free(pr);

    return retCode;
}

cmd_result_t
cmd_if_flexport(int unit, args_t *a)
{
    flexport_info_t *flexinfo;
    char *arg;
    parse_table_t pt;
    cmd_result_t retCode = CMD_OK;
    int i, auto_assign;
    uint32 seen, parsed;
    char *pm_str[] = {
        "PM0", "PM1", "PM2", "PM3", "PM4", "PM5", "PM6", "PM7",
        "PM8", "PM9", "PM10", "PM11", "PM12", "PM13", "PM14", "PM15",
        "PM16", "PM17", "PM18", "PM19", "PM20", "PM21", "PM22", "PM23",
        "PM24", "PM25", "PM26", "PM27", "PM28", "PM29", "PM30", "PM31",
        "PM32", "PM33", "PM34", "PM35", "PM36", "PM37", "PM38", "PM39",
        "PM40", "PM41", "PM42", "PM43", "PM44", "PM45", "PM46", "PM47",
        "PM48", "PM49", "PM50", "PM51", "PM52", "PM53", "PM54", "PM55",
        "PM56", "PM57", "PM58", "PM59", "PM60", "PM61", "PM62", "PM63"
    };

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((arg = ARG_CUR(a)) == NULL) {
        return (CMD_USAGE);
    }

    flexinfo = sal_alloc(sizeof(flexport_info_t), "if_flexport");
    if (flexinfo == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }
    sal_memset(flexinfo, 0, sizeof(flexport_info_t));

    parse_table_init(unit, &pt);
    for (i = 0; i <  PM_NUM(unit); i++) {
        parse_table_add(&pt, pm_str[i], PQ_STRING|PQ_DFL|PQ_NO_EQ_OPT, 0,
                        &flexinfo->pm[i].pm_str, NULL);
    }

    if (parse_arg_eq(a, &pt) < 0) {
        parse_arg_eq_done(&pt);
        sal_free(flexinfo);
        return CMD_FAIL;
    }

    seen = 0;
    parsed = 0;
    for (i = 0; i < pt.pt_cnt; i++) {
        if (pt.pt_entries[i].pq_type & PQ_SEEN) {
            flexinfo->pm[i].flags |= FLXP_F_SEEN;
            seen = 1;
        }

        if (pt.pt_entries[i].pq_type & PQ_PARSED) {
            flexinfo->pm[i].flags |= FLXP_F_PARSED;
            parsed = 1;
        }
    }

    if (seen && parsed) {
        cli_out("%s: Cannot get and set port resources in one command\n",
                ARG_CMD(a));
        retCode = CMD_FAIL;
    } else if (seen) {  /* Show the port resource info for specified pm */
        retCode = flexport_show(unit, flexinfo);
    } else if (parsed) {
        auto_assign = 0;
        retCode = flexport_parse_portlist(unit, flexinfo->pm);
        if (retCode == CMD_OK) {
            retCode = flexport_parse_pm_mode(unit, flexinfo, &auto_assign);
        }
        if (retCode == CMD_OK && auto_assign) {
            retCode = flexport_port_assign(unit, flexinfo);
        }
        if (retCode == CMD_OK) {
            retCode = flexport_diag(unit, flexinfo);
        }
    }
    parse_arg_eq_done(&pt);
    sal_free(flexinfo);
    return retCode;
}

#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */
