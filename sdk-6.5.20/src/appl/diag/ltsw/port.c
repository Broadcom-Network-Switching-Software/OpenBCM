/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Functions to support CLI port commands
 */

#if defined(BCM_LTSW_SUPPORT)

#include <shared/pbmp.h>
#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/ltsw/util.h>
#include <appl/diag/ltsw/cmdlist.h>

#include <bcm/types.h>
#include <bcm/init.h>
#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm_int/ltsw/port.h>

static int prev_linkscan_interval[BCM_MAX_NUM_UNITS];

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

/* Note:  See link.h, bcm_linkscan_mode_e */
char *ltsw_linkscan_mode[] = {
    "None", "SW", "HW", "OVR", NULL
};

#define _CHECK_PRINT(unit, flags, mask, str, val) do {                \
        if ((flags) & (mask)) {                                       \
            cli_out(str, val);               \
        } else {                                                      \
            cli_out(str, "");                \
        }                                                             \
    } while (0)

extern int
bcmi_ltsw_port_fec_get(int unit, bcm_port_t port, bcm_port_phy_fec_t * fec);

/*!
 * \brief Setup the parse table for a port command.
 *
 * \param [in] unit Unit number.
 * \param [in] pt The table.
 * \param [in] info Port info structure to hold parse results.
 */
static void
ltsw_port_parse_setup(int unit, parse_table_t *pt, bcm_port_info_t *info)
{
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
                    0, &info->linkscan, ltsw_linkscan_mode);
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
}

/*!
 * \brief Given PT has been parsed, set seen and parsed flags.
 *
 * \param [in] pt The table.
 * \param [out] seen Which parameters occurred w/o =.
 * \param [out] parsed Which parameters occurred w =.
 */
static void
ltsw_port_parse_mask_get(parse_table_t *pt, uint32 *seen, uint32 *parsed)
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

/*!
 * \brief Setup action bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] info Data structure containing action bitmap.
 * \param [in] actions Pre-set actions from caller.
 */
static void
ltsw_port_info_init(int unit, int port, bcm_port_info_t *info, uint32 actions)
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
}

/*
 * Function:
 *    _if_fmt_speed
 * Purpose:
 *    Format a speed as returned from bcm_xxx for display.
 * Parameters:
 *    b     - buffer to format into.
 *    speed - speed as returned from bcm_port_speed_get
 * Returns:
 *    Pointer to buffer (b).
 */
static char *
ltsw_fmt_speed(char *b, int speed)
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

/*!
 * \brief Print header line for ps command.
 *
 * \param [in] unit Unit number.
 */
static void
ltsw_brief_port_info_header(int unit)
{
    char *fc = " ";
    char *ln = "Lns";
    char *asf[2] = {"cut", "thru"};
    char *disp_str =
        "%12s "                 /* port number */
        "%5s  "                 /* enable/link state */
        "%3s"                   /* lanes */
        "%9s "                  /* speed/duplex */
        "%4s "                  /* link scan mode */
        "%4s "                  /* auto negotiate? */
        "%7s   "                /* spantree state */
        "%5s  "                 /* pause tx/rx */
        "%6s "                  /* discard mode */
        "%3s "                  /* learn to CPU, ARL, FWD or discard */
        "%9s "                 /* medium */
        "%5s "                  /* max frame */
        "%6s "                  /* cutthrough */
        "%9s "                  /* FEC */
        "%5s\n";                /* loopback */

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
               " ",        /* medium */
               "max",               /* max frame */
               asf[0],              /* cutthrough */
               "   ",               /* FEC */
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
               "medium ",        /* medium */
               "frame",             /* max frame */
               asf[1],              /* cutthrough */
               "FEC",               /* FEC */
               "back");             /* loopback */
}

/*!
 * \brief Print body lines for ps command.
 *
 * \param [in] unit Unit number.
 * \param [in] Port Port number.
 * \param [in] info Port info data structure.
 * \param [in] flags Print options.
 */
static int
ltsw_brief_port_info(int unit, int port, bcm_port_info_t *info, uint32 flags)
{
    char *spt_str = FORWARD_MODE(info->stp_state);
    char *discrd_str = DISCARD_MODE(info->discard);
    char sbuf[6];
    int lrn_ptr;
    char lrn_str[4];
    int ret = 0;
    char asf_str[4][4] = {"No", "Yes"};
    int asf_mode = 0;
    bcm_port_resource_t speed_res;
    bcm_port_phy_fec_t fec = bcmPortPhyFecInvalid;
    char fec_list[11][10] = {"NONE", "BASE-R", "RS528", "RS544-1xN", "RS272-1xN", "", "", "", "", "RS544-2xN", "RS272-2xN"};

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
    cli_out("%7s(%3d)  %4s ", bcmi_ltsw_port_name(unit, port), port,
            !info->enable ? "!ena" :
            (info->linkstatus == BCM_PORT_LINK_STATUS_FAILED) ? "fail" :
            (info->linkstatus == BCM_PORT_LINK_STATUS_UP ? "up  " : "down"));

    if (flags & BCM_PORT_ATTR_ALL_MASK) {
        ret = bcm_port_resource_speed_get(unit, port, &speed_res);
        if (ret != BCM_E_NONE) {
            cli_out(" %2s ", "");
        } else {
            cli_out(" %2d ", speed_res.lanes);
        }
    } else {
        cli_out(" %2s ", "");
    }

    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_SPEED_MASK,
                 "%5s ", ltsw_fmt_speed(sbuf, info->speed));
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_DUPLEX_MASK,
                 "%3s ", info->speed == 0 ? "" : info->duplex ? "FD" : "HD");
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LINKSCAN_MASK,
                 "%4s ", ltsw_linkscan_mode[info->linkscan]);
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
    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_MEDIUM_MASK,
                 "%9s ", MEDIUM_STATUS(info->medium));

    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        cli_out("%5d ", info->frame_max);
    } else {
        cli_out("%5s ", "");
    }

    ret = bcm_switch_control_port_get(unit, port,
                                      bcmSwitchAlternateStoreForward,
                                      &asf_mode);
    if (BCM_E_UNAVAIL == ret) {
        asf_mode = 0;
    }

    if ((asf_mode >= 0) && (asf_mode <= 1)) {
        cli_out("%5s ", asf_str[asf_mode]);
    }

    if (flags & BCM_PORT_ATTR_ALL_MASK) {
        ret = bcmi_ltsw_port_fec_get(unit, port, &fec);
        if (ret == BCM_E_NONE) {
            cli_out("%10s ", fec_list[fec - 1]);
        } else {
            cli_out("%10s ", "");
        }
    } else {
        cli_out("%10s ", "");
    }

    _CHECK_PRINT(BSL_UNIT_UNKNOWN, flags, BCM_PORT_ATTR_LOOPBACK_MASK,
                 "%5s",
                 info->loopback != BCM_PORT_LOOPBACK_NONE ?
                 LOOPBACK_MODE(info->loopback) : "     ");

    cli_out("\n");
    return ret;
}


/******************************************************************************
 * Public Functions
 */
cmd_result_t
if_ltsw_linkscan(int unit, args_t *a)
{
    parse_table_t pt;
    char *c;
    int us, port, mode, rv, link;
    bcm_port_config_t pcfg;
    bcm_pbmp_t pbm_sw, pbm_hw, pbm_ovr, pbm_none, pbm_force;
    bcm_pbmp_t pbm_temp, pbm_none_pre, pbm_linkup, pbm_linkup_pre;
    char pfmt[_SHR_PBMP_FMT_LEN];
    int itvl_def = BCM_LINKSCAN_INTERVAL_DEFAULT;

    /*
     * Workaround that allows "linkscan off" at the beginning of ltsw.soc
     */
    if (ARG_CNT(a) == 1 && sal_strcasecmp(_ARG_CUR(a), "off") == 0) {
        rv = bcm_init_check(unit);
        if (rv == BCM_E_UNIT) {
            (void)ARG_GET(a);
            return (CMD_OK);
        }
    }

    bcm_port_config_t_init(&pcfg);
    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: Could not get port config\n", ARG_CMD(a));
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
    BCM_PBMP_CLEAR(pbm_ovr);
    BCM_PBMP_CLEAR(pbm_force);
    BCM_PBMP_CLEAR(pbm_none);
    BCM_PBMP_CLEAR(pbm_linkup);
    BCM_PBMP_CLEAR(pbm_linkup_pre);

    BCM_PBMP_ITER(pcfg.port, port) {
        rv = bcm_linkscan_mode_get(unit, port, &mode);
        if (BCM_SUCCESS(rv)) {
            switch (mode) {
            case BCM_LINKSCAN_MODE_SW:
                BCM_PBMP_PORT_ADD(pbm_sw, port);
                break;
            case BCM_LINKSCAN_MODE_HW:
                BCM_PBMP_PORT_ADD(pbm_hw, port);
                break;
            case BCM_LINKSCAN_MODE_OVERRIDE:
                BCM_PBMP_PORT_ADD(pbm_ovr, port);
                break;
            case BCM_LINKSCAN_MODE_NONE:
                BCM_PBMP_PORT_ADD(pbm_none, port);
            default:
                break;
            }
        } else {
            cli_out("%s: Error: Could not get linkscan mode for port %d: %s\n",
                    ARG_CMD(a), port, bcm_errmsg(rv));
        } 

        rv = bcm_linkscan_override_link_state_get(unit, port, &link);
        if (BCM_SUCCESS(rv)) {
            if (link) {
                BCM_PBMP_PORT_ADD(pbm_linkup_pre, port);
            }
        } else {
            cli_out("%s: Error: Could not get override link state for "
                    "port %d: %s\n", ARG_CMD(a), port, bcm_errmsg(rv));
        }
    }

    /*
     * If there are no arguments, just display the status.
     */
    if (ARG_CNT(a) == 0) {
        if (us) {
            cli_out("%s: Linkscan enabled\n", ARG_CMD(a));
            cli_out("%s:   Software polling interval: %d usec\n",
                    ARG_CMD(a), us);
            cli_out("%s:   Software Port BitMap %s\n",
                    ARG_CMD(a), _SHR_PBMP_FMT(pbm_sw, pfmt));
            cli_out("%s:   Hardware Port BitMap %s\n",
                    ARG_CMD(a), _SHR_PBMP_FMT(pbm_hw, pfmt));
            cli_out("%s:   Override Port BitMap %s\n",
                    ARG_CMD(a), _SHR_PBMP_FMT(pbm_ovr, pfmt));
            cli_out("%s:   Disabled Port BitMap %s\n",
                    ARG_CMD(a), _SHR_PBMP_FMT(pbm_none, pfmt));
            cli_out("%s:   Override link up Port BitMap %s\n",
                    ARG_CMD(a), _SHR_PBMP_FMT(pbm_linkup_pre, pfmt));
        } else {
            cli_out("%s: Linkscan disabled\n", ARG_CMD(a));
        }

        return (CMD_OK);
    }

    BCM_PBMP_ASSIGN(pbm_none_pre, pbm_none);

    us = us ? us : itvl_def;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "SwPortBitMap", PQ_PBMP | PQ_DFL | PQ_BCM, 0, &pbm_sw,
                    0);
    parse_table_add(&pt, "HwPortBitMap", PQ_PBMP | PQ_DFL | PQ_BCM, 0, &pbm_hw,
                    0);
    parse_table_add(&pt, "OverridePortBitMap", PQ_PBMP | PQ_DFL | PQ_BCM, 0,
                    &pbm_ovr, 0);
    parse_table_add(&pt, "OverrideLinkUp", PQ_PBMP | PQ_DFL | PQ_BCM, 0,
                    &pbm_linkup, 0);
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
                 prev_linkscan_interval[unit] : itvl_def;
            if (us == 0) {
                cli_out("WARNING: Cannot enable Linkscan if the polling "
                        "interval is zero\n");
            }
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
            prev_interval = itvl_def;
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
    BCM_PBMP_AND(pbm_ovr, pcfg.port);
    BCM_PBMP_AND(pbm_force, pcfg.port);

    BCM_PBMP_ASSIGN(pbm_none, pcfg.port);
    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
    BCM_PBMP_OR(pbm_temp, pbm_hw);
    BCM_PBMP_OR(pbm_temp, pbm_ovr);
    BCM_PBMP_XOR(pbm_none, pbm_temp);

    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
    BCM_PBMP_XOR(pbm_temp, pbm_hw);
    BCM_PBMP_XOR(pbm_temp, pbm_ovr);
    BCM_PBMP_XOR(pbm_temp, pbm_none);
    if (BCM_PBMP_NEQ(pbm_temp, pcfg.port)) {
        cli_out("%s: Error: One port can only be configured with one "
                "linkscan mode\n", ARG_CMD(a));
        return (CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_sw,
        BCM_LINKSCAN_MODE_SW)) < 0) {
        cli_out("%s: Failed to set software link scanning: PBM=%s: %s\n",
                ARG_CMD(a), _SHR_PBMP_FMT(pbm_sw, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_hw,
        BCM_LINKSCAN_MODE_HW)) < 0) {
        cli_out("%s: Failed to set hardware link scanning: PBM=%s: %s\n",
                ARG_CMD(a), _SHR_PBMP_FMT(pbm_hw, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_ovr,
        BCM_LINKSCAN_MODE_OVERRIDE)) < 0) {
        cli_out("%s: Failed to set override link scanning: PBM=%s: %s\n",
                ARG_CMD(a), _SHR_PBMP_FMT(pbm_hw, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    BCM_PBMP_XOR(pbm_none_pre, pbm_none);
    BCM_PBMP_AND(pbm_none, pbm_none_pre);
    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_none,
                                        BCM_LINKSCAN_MODE_NONE)) < 0) {
        cli_out("%s: Failed to disable link scanning: PBM=%s: %s\n",
                ARG_CMD(a), _SHR_PBMP_FMT(pbm_none, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    /* Set override link state. */
    BCM_PBMP_ASSIGN(pbm_temp, pbm_linkup_pre);
    BCM_PBMP_XOR(pbm_temp, pbm_linkup);
    BCM_PBMP_AND(pbm_linkup, pbm_temp);
    BCM_PBMP_REMOVE(pbm_temp, pbm_linkup);
    BCM_PBMP_AND(pbm_temp, pbm_linkup_pre);
    /* Remove Cpu port. */
    BCM_PBMP_PORT_REMOVE(pbm_linkup, 0);
    BCM_PBMP_ITER(pbm_linkup, port) {
        rv = bcm_linkscan_override_link_state_set(unit, port, 1);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error: Failed to set override link state to UP "
                    "for port %d: %s\n",
                    ARG_CMD(a), port, bcm_errmsg(rv));
        }
    }
    BCM_PBMP_ITER(pbm_temp, port) {
        rv = bcm_linkscan_override_link_state_set(unit, port, 0);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error: Failed to set override link state to DOWN "
                    "for port %d: %s\n",
                    ARG_CMD(a), port, bcm_errmsg(rv));
        }
    }

    if ((rv = bcm_linkscan_enable_set(unit, us)) < 0) {
        cli_out("%s: Error: Failed to enable linkscan: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if ((rv = bcm_link_change(unit, pbm_force)) < 0) {
        cli_out("%s: Failed to force link scan: PBM=%s: %s\n",
                ARG_CMD(a), _SHR_PBMP_FMT(pbm_force, pfmt), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return (CMD_OK);
}


/*!
 * \brief Port config cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_port(int unit, args_t *arg)
{
    char *c = NULL;
    int port, dport;
    parse_table_t pt;
    bcm_pbmp_t pbmp, pbmp_op;
    char pfmt[_SHR_PBMP_FMT_LEN];
    bcm_port_config_t cfg;
    bcm_port_info_t info;
    uint32 seen = 0, parsed = 0;
    int rv, cmd_rv = CMD_OK;

    bcm_port_info_t_init(&info);
    bcm_port_config_t_init(&cfg);

    if (bcm_port_config_get(unit, &cfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }
    if (parse_pbmp(unit, c, &pbmp) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(arg), c);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbmp, cfg.port);

    if (BCM_PBMP_IS_NULL(pbmp)) {
        ARG_DISCARD(arg);
        return CMD_OK;
    }

    if (ARG_CNT(arg) == 0) {
        seen = BCM_PORT_ATTR_ALL_MASK;
    } else {
        if (!sal_strcasecmp(_ARG_CUR(arg), "detach")) {
            bcm_port_detach(unit, pbmp, &pbmp_op);
            cli_out("Detached port bitmap %s\n", _SHR_PBMP_FMT(pbmp_op, pfmt));
            ARG_GET(arg);
            return CMD_OK;
        } else if ((!sal_strcasecmp(_ARG_CUR(arg), "probe")) ||
                   (!sal_strcasecmp(_ARG_CUR(arg), "attach"))) {
            bcm_port_probe(unit, pbmp, &pbmp_op);
            cli_out("Probed port bitmap %s\n", _SHR_PBMP_FMT(pbmp_op, pfmt));
            ARG_GET(arg);
            return CMD_OK;
        } else if (!sal_strcasecmp(_ARG_CUR(arg), "asf")) {
            int asf_mode;
            int in_range = 0;

            ARG_GET(arg);
            if ((c = ARG_GET(arg)) == NULL) {
                return CMD_USAGE;
            }

            asf_mode = sal_ctoi(c, 0);
            if (((asf_mode >= 0) && (asf_mode <= 1))) {
                in_range = 1;
            }

            if (!in_range) {
                cli_out("%d: invalid mode\n", asf_mode);
                return CMD_OK;
            }
            BCM_PBMP_ITER(pbmp, port) {
                rv = bcm_switch_control_port_set(unit, port,
                         bcmSwitchAlternateStoreForward, asf_mode);
                if (BCM_FAILURE(rv)) {
                    if (BCM_E_UNAVAIL == rv) {
                        cli_out("%s: unsupported\n", bcmi_ltsw_port_name(unit, port));
                    }
                    if (BCM_E_PARAM == rv) {
                        cli_out("%s: invalid arg(s)\n", bcmi_ltsw_port_name(unit, port));
                    }
                }
                continue;
            }
            return CMD_OK;
        }

        /* parse parameters */
        ltsw_port_parse_setup(unit, &pt, &info);
        if (parse_arg_eq(arg, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        if (ARG_CNT(arg) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(arg), ARG_CUR(arg));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        /* find out what parameters specified */
        ltsw_port_parse_mask_get(&pt, &seen, &parsed);
        parse_arg_eq_done(&pt);
    }

    if (seen && parsed) {
        cli_out("%s: Cannot get and set "
                "port properties in one command\n", ARG_CMD(arg));
        return CMD_FAIL;
    } else if (seen) {
        DPORT_PBMP_ITER(unit, pbmp, dport, port) {
            bcm_port_info_t_init(&info);
            port_info_init(unit, port, &info, seen);
            rv = bcm_port_selective_get(unit, port, &info);
            if (rv < 0) {
                cli_out("%s: Error: Could not get port %d, info: %s\n",
                        ARG_CMD(arg), port, bcm_errmsg(rv));
                cmd_rv = CMD_FAIL;
            }
            disp_port_info(unit, bcmi_ltsw_port_name(unit, port), port, &info,
                           (info.encap_mode != BCM_PORT_ENCAP_IEEE), seen);
        }
        ARG_GET(arg);
        return CMD_OK;
    }

    info.action_mask = parsed;
    BCM_PBMP_ITER(pbmp, port) {
        rv = bcm_port_selective_set(unit, port, &info);
        if (rv < 0) {
            cli_out("%s: Error: Could not set port %d, info: %s\n",
                    ARG_CMD(arg), port, bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
        }
    }

    return cmd_rv;
}


/*!
 * \brief Port status dispaly cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_port_stat(int unit, args_t *arg)
{
    bcm_port_config_t pcfg;
    bcm_port_info_t *info_all, *info;
    bcm_pbmp_t pbmp;
    int port, dport;
    uint32 mask;
    char *c;
    int rv, cmd_rv = CMD_OK;

    bcm_port_config_t_init(&pcfg);
    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: Could not get port config\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    /* parse port bmp */
    BCM_PBMP_CLEAR(pbmp);
    if ((c = ARG_GET(arg)) == NULL) {
        BCM_PBMP_ASSIGN(pbmp, pcfg.port);
    } else if (parse_pbmp(unit, c, &pbmp) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(arg), c);
        return CMD_FAIL;
    }
    BCM_PBMP_AND(pbmp, pcfg.port);
    if (BCM_PBMP_IS_NULL(pbmp)) {
        cli_out("No ports specified.\n");
        return CMD_OK;
    }

    mask = BCM_PORT_ATTR_ALL_MASK;
    info_all = sal_alloc(BCM_PBMP_PORT_MAX * sizeof(bcm_port_info_t),
                         "if_port_stat");
    if (info_all == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }

    /* get port info for all ports in bmp */
    BCM_PBMP_ITER(pbmp, port) {
        info = info_all + port;
        bcm_port_info_t_init(info);
        ltsw_port_info_init(unit, port, info, mask);
        rv = bcm_port_selective_get(unit, port, info);
        if (rv < 0) {
            cli_out("%s: Error: Could not get port %d, info: %s\n",
                    ARG_CMD(arg), port, bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
        }
    }

    /* print */
    ltsw_brief_port_info_header(unit);
    DPORT_PBMP_ITER(unit, pbmp, dport, port) {
        info = info_all + port;
        ltsw_brief_port_info(unit, port, info, mask);
    }

    sal_free(info_all);
    return cmd_rv;
}


cmd_result_t
cmd_ltsw_pbmp(int unit, args_t *arg)
{
    pbmp_t		pbmp;
    char		*c;
    bcm_port_t		port;
    char		pbmp_str[FORMAT_PBMP_MAX];
    char		pfmt[_SHR_PBMP_FMT_LEN];
    bcm_port_config_t cfg;

    bcm_port_config_t_init(&cfg);
    if (bcm_port_config_get(unit, &cfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    c = ARG_GET(arg);

    if (!c) {
        cli_out("Current bitmaps:\n");
        cli_out("     FE   ==> %s\n", _SHR_PBMP_FMT(cfg.fe, pfmt));
        cli_out("     GE   ==> %s\n", _SHR_PBMP_FMT(cfg.ge, pfmt));
        cli_out("     XE   ==> %s\n", _SHR_PBMP_FMT(cfg.xe, pfmt));
        cli_out("     CE   ==> %s\n", _SHR_PBMP_FMT(cfg.ce, pfmt));
        cli_out("     CD   ==> %s\n", _SHR_PBMP_FMT(cfg.cd, pfmt));
        cli_out("     E    ==> %s\n", _SHR_PBMP_FMT(cfg.e, pfmt));
        cli_out("     HG   ==> %s\n", _SHR_PBMP_FMT(cfg.hg, pfmt));
        cli_out("     PORT ==> %s\n", _SHR_PBMP_FMT(cfg.port, pfmt));
        cli_out("     CMIC ==> %s\n", _SHR_PBMP_FMT(cfg.cpu, pfmt));
        cli_out("     ALL  ==> %s\n", _SHR_PBMP_FMT(cfg.all, pfmt));
        return CMD_OK;
    }

    if (sal_strcasecmp(c, "port") == 0) {
	if ((c = ARG_GET(arg)) == NULL) {
	    cli_out("ERROR: missing port string\n");
	    return CMD_FAIL;
	}
	if (parse_port(unit, c, &port) < 0) {
	    cli_out("%s: Invalid port string: %s\n", ARG_CMD(arg), c);
	    return CMD_FAIL;
	}
	cli_out("    port %s ==> %s (%d)\n",
                c, bcmi_ltsw_port_name(unit, port), port);
	return CMD_OK;
    }

    if (parse_pbmp(unit, c, &pbmp) < 0) {
	cli_out("%s: Invalid pbmp string (%s); use 'pbmp ?' for more info.\n",
                ARG_CMD(arg), c);
	return CMD_FAIL;
    }

    format_pbmp(unit, pbmp_str, sizeof (pbmp_str), pbmp);

    cli_out("    %s ==> %s\n", _SHR_PBMP_FMT(pbmp, pfmt), pbmp_str);

    return CMD_OK;
}

cmd_result_t
if_ltsw_asf(int unit, args_t *a)
{
    int p, dp, rv = BCM_E_NONE;
    char *arg;
    pbmp_t pbm;
    bcm_port_config_t cfg;

    bcm_port_config_t_init(&cfg);

    if (bcm_port_config_get(unit, &cfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((arg = ARG_GET(a)) == NULL) {
        return (CMD_USAGE);
    }

    if (arg && !sal_strcasecmp(arg, "show")) {
        if ((arg = ARG_GET(a)) == NULL) {
            return (CMD_USAGE);
        }
        if (parse_pbmp(unit, arg, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), arg);
            return CMD_FAIL;
        }

        BCM_PBMP_AND(pbm, cfg.port);
        if (BCM_PBMP_IS_NULL(pbm)) {
            return CMD_OK;
        }
        cli_out("         cut-through      cut-through speed \n");
        cli_out("port        mode          range (from/to)\n");
        DPORT_PBMP_ITER(unit, pbm, dp, p) {
            rv = bcmi_ltsw_port_asf_show(unit, p);
        }
    } else if (arg && !sal_strcasecmp(arg, "diag")) {
        if ((arg = ARG_GET(a)) == NULL) {
            return (CMD_USAGE);
        }
        if (parse_pbmp(unit, arg, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n", ARG_CMD(a), arg);
            return CMD_FAIL;
        }

        BCM_PBMP_AND(pbm, cfg.port);
        if (BCM_PBMP_IS_NULL(pbm)) {
            return CMD_OK;
        }

        cli_out("%-5s %-3s %-3s %-6s %-6s %-3s %-32s ", "port", "asf",
            "ctc", "min_sp", "max_sp", "thr",
             "        xmit_start_count");
        cli_out("%-3s, %-3s  \n", "emc", "rxp");
        cli_out("%-33s %-3s %-3s %-3s %-3s %-3s %-3s %-3s %-3s\n", " ",
                "sc0", "sc1", "sc2", "sc3", "sc4", "sc5", "sc6", "sc7");

        DPORT_PBMP_ITER(unit, pbm, dp, p) {
            rv = bcmi_ltsw_port_asf_diag(unit, p);
        }

        cli_out("Legends:\n");
        cli_out("asf: CT Mode, ctc: CT Class, min: Min. Src Port Speed, "\
            "max: Max. Src Port Speed, thr: FIFO Threshold\n");
        cli_out("xmit_start_count: Transmit Start Count, "
            "sc$: Source Class \n");
        cli_out("emc: EGR MMU Credits, rxp: RX Pause\n");
    } else {
        cli_out("error: incorrect argument\n");
    }

    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
}




#define MAX_PM_NUM          64
#define MAX_PIPE_NUM        16
#define MAX_PORTS_PER_PM    8

typedef struct ltsw_pm_mode_s {
    char *pm_str;
    int num_port;
    bcm_port_resource_t pr[MAX_PORTS_PER_PM];
} ltsw_pm_mode_t;

static ltsw_pm_mode_t pm8x50_mode[] = {
    {
        "None",
        0,
    },
    {
        "1x400G",
        1,
        {
            [0] = {
                .speed = 400000,
                .lanes = 8,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544_2xN,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "2x200G",
        2,
        {
            [0] = {
                .speed = 200000,
                .lanes = 4,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544_2xN,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "2x100G",
        2,
        {
            [0] = {
                .speed = 100000,
                .lanes = 4,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecNone,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "2x40G",
        2,
        {
            [0] = {
                .speed = 40000,
                .lanes = 4,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecNone,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "4x100G",
        4,
        {
            [0] = {
                .speed = 100000,
                .lanes = 2,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "4x50G",
        4,
        {
            [0] = {
                .speed = 50000,
                .lanes = 2,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "4x40G",
        4,
        {
            [0] = {
                .speed = 40000,
                .lanes = 2,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecNone,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "8x50G",
        8,
        {
            [0] = {
                .speed = 50000,
                .lanes = 1,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "8x25G",
        8,
        {
            [0] = {
                .speed = 25000,
                .lanes = 1,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecNone,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {
        "8x10G",
        8,
        {
            [0] = {
                .speed = 10000,
                .lanes = 1,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecNone,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    /* example of mixed speed */
    {
        "2x100G+4x50G",
        6,
        {
            [0] = {
                .speed = 100000,
                .lanes = 2,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544,
                .phy_lane_config = -1,
                .link_training = 0,
            },
            /* 50G ports start from 2 */
            [2] = {
                .speed = 50000,
                .lanes = 1,
                .encap = BCM_PORT_ENCAP_IEEE,
                .fec_type = bcmPortPhyFecRs544,
                .phy_lane_config = -1,
                .link_training = 0,
            },
        },
    },
    {0},
};

typedef struct ltsw_range_s {
    int start;
    int end;
} ltsw_range_t;

ltsw_range_t td4_port_range[] = {
    {1,     18},
    {20,    37},
    {40,    57},
    {60,    77},
    {80,    97},
    {100,   117},
    {120,   137},
    {140,   157},
};

ltsw_range_t td4x9_port_range[] = {
    {1,     18},
    {20,    37},
    {40,    57},
    {60,    77},
};

ltsw_range_t th4_port_range[] = {
    {1,     16},
    {17,    32},
    {34,    49},
    {51,    66},
    {68,    83},
    {85,    100},
    {102,   117},
    {119,   134},
    {136,   151},
    {153,   168},
    {170,   185},
    {187,   202},
    {204,   219},
    {221,   236},
    {238,   253},
    {255,   270},
};

typedef struct ltsw_dev_info_s {
    int dev_id;
    int num_port;
    int num_pipe;
    int num_pm;
    int num_lane_per_pm;
    ltsw_pm_mode_t *pm_mode;
    ltsw_range_t *port_range_per_pipe;
} ltsw_dev_info_t;

ltsw_dev_info_t ltsw_dev_info[] = {
    {
        BCM56880_DEVICE_ID,
        144,
        8,
        32,
        8,
        pm8x50_mode,
        td4_port_range,
    },
    {
        BCM56780_DEVICE_ID,
        72,
        4,
        20,
        8,
        pm8x50_mode,
        td4x9_port_range,
    },
#if 0
    {
        BCM56990_DEVICE_ID,
        256,
        16,
        64,
        8,
        pm8x50_mode,
        th4_port_range,
    },
#endif
    {0},
};

typedef struct ltsw_flexport_pm_info_s {
    /* Info get from cli input*/
    char *pm_str;
    int ports[MAX_PORTS_PER_PM];
    int port_cnt;

    /* Parsed info */
    bcm_port_resource_t pr[MAX_PORTS_PER_PM];
    uint32 flags;
#define FLXP_F_SEEN                 0x1 /* PM seen without mode/port config */
#define FLXP_F_PARSED               0x2 /* PM with config is parsed */
#define FLXP_F_PORT_ASSIGNED        0x4 /* Logical ports have been assigned */
} ltsw_flexport_pm_info_t;

typedef struct ltsw_flexport_info_s {
    ltsw_flexport_pm_info_t pm[MAX_PM_NUM];
    int add_cnt;
    int del_cnt;
} ltsw_flexport_info_t;


static ltsw_dev_info_t *ltsw_dev_lookup(int unit)
{
    uint16 dev_id = 0, dev_id_driver = 0, rev_id_driver = 0;
    uint8 rev_id = 0;
    ltsw_dev_info_t *dev = ltsw_dev_info;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &dev_id_driver, &rev_id_driver);

    while (dev->dev_id && (dev->dev_id != dev_id_driver)) {
        dev ++;
    }

    return dev->dev_id ? dev : NULL;
}


static cmd_result_t
ltsw_flexport_show(int unit, ltsw_flexport_info_t *f)
{
    int pm, port, phy_port, phy_base, i;
    bcm_port_resource_t pr;
    char buf[6];
    int max_ports = 0;
    ltsw_dev_info_t *dev;

    dev = ltsw_dev_lookup(unit);

    max_ports = dev->num_lane_per_pm;
    for (pm = 0; pm < dev->num_pm; pm++) {
        if (!(f->pm[pm].flags & FLXP_F_SEEN)) {
            continue;
        }
        cli_out("PM%d:\n", pm);
        phy_base = (pm * max_ports) + 1;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = bcmi_ltsw_pport_to_port(unit, phy_port);
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
ltsw_flexport_parse_portlist(int unit, ltsw_flexport_pm_info_t *pi)
{
    int pm, p, pfirst, plast, i;
    char *s, *sn, *pl;
    int max_ports = 0;
    ltsw_dev_info_t *dev;

    dev = ltsw_dev_lookup(unit);

    max_ports = dev->num_lane_per_pm;
    for (pm = 0; pm < dev->num_pm; pm++) {
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

static cmd_result_t
ltsw_flexport_parse_pm_mode(int unit, ltsw_flexport_info_t *f, int *auto_assign)
{
    ltsw_flexport_pm_info_t *pi = f->pm;
    bcm_port_resource_t *pr;
    int pm, phy_base, phy_port, port, i, j;
    char *s;
    int max_ports = 0;
    ltsw_dev_info_t *dev;
    ltsw_pm_mode_t *pm_mode;

    dev = ltsw_dev_lookup(unit);

    max_ports = dev->num_lane_per_pm;
    f->del_cnt = 0;
    for (pm = 0; pm < dev->num_pm; pm++) {
        if (pi[pm].pm_str == NULL) {
            continue;
        }
        s = pi[pm].pm_str;
        pr = pi[pm].pr;
        phy_base = pm * max_ports + 1;

        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = bcmi_ltsw_pport_to_port(unit, phy_port);
            if (port != -1) {
                f->del_cnt ++;
            }
        }

        pm_mode = dev->pm_mode;
        while (pm_mode->pm_str  && sal_strncasecmp(s, pm_mode->pm_str, 16)) {
            pm_mode ++;
        }

        if (!pm_mode->pm_str) {
            cli_out("Flexport: Error: Unsupported mode %s on pm%d.\n", s, pm);
            return CMD_FAIL;
        }

        for (i = 0, j = 0; i < pm_mode->num_port; i++) {
            if (pm_mode->pr[i].speed) {
                j = i;
            }
            sal_memcpy(pr + i, pm_mode->pr + j, sizeof(bcm_port_resource_t));
            if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
                pr[i].port = pi[pm].ports[i];
            }
            pr[i].physical_port = phy_base;
            phy_base += pr[i].lanes;
        }

        if (pi[pm].flags & FLXP_F_PORT_ASSIGNED) {
            if (pi[pm].port_cnt != pm_mode->num_port) {
                cli_out("Flexport: Error: pm%d %s requires %d ports.\n",
                        pm, s, pm_mode->num_port);
                return CMD_FAIL;
            }
        } else {
            pi[pm].port_cnt = pm_mode->num_port;
            if (pm_mode->num_port) {
                *auto_assign |= 1;
            }
        }
        f->add_cnt += pm_mode->num_port;
    }

    return CMD_OK;
}

static cmd_result_t
ltsw_flexport_port_assign(int unit, ltsw_flexport_info_t *f)
{
    bcm_pbmp_t free_pbmp, cur_pbmp;
    int i, pm, port, phy_base, phy_port, pipe, cnt, pm_per_pipe;
    int ffport[MAX_PIPE_NUM]; /* First free port in each pipe */
    int max_ports = 0;
    ltsw_range_t *pbs;
    ltsw_dev_info_t *dev;

    dev = ltsw_dev_lookup(unit);

    pbs = dev->port_range_per_pipe;
    pm_per_pipe = dev->num_pm / dev->num_pipe;
    max_ports = dev->num_lane_per_pm;

    BCM_PBMP_CLEAR(free_pbmp);

    /* All allowable ports */
    for (pipe = 0; pipe < dev->num_pipe; pipe++) {
        SOC_PBMP_PORTS_RANGE_ADD(free_pbmp,
            pbs[pipe].start, (pbs[pipe].end - pbs[pipe].start + 1));
        ffport[pipe] = pbs[pipe].start;
    }

    /* Remove in using ports */
    bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &cur_pbmp);
    BCM_PBMP_REMOVE(free_pbmp, cur_pbmp);

    /* Involved ports need to be removed first during flexport,
     * thus, add these removed ports to free-port pool */
    for (pm = 0; pm < dev->num_pm; pm++) {
        if (f->pm[pm].pm_str == NULL) {
            continue;
        }
        phy_base = pm * max_ports + 1;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = bcmi_ltsw_pport_to_port(unit, phy_port);
            if (port != -1) {
                BCM_PBMP_PORT_ADD(free_pbmp, port);
            }
        }
    }

    /* Remove ports that specified by CLI from free-port pool */
    for (pm = 0; pm < dev->num_pm; pm++) {
        if (f->pm[pm].pm_str == NULL ||
            !(f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        for (i = 0; i < f->pm[pm].port_cnt; i++) {
            BCM_PBMP_PORT_REMOVE(free_pbmp, f->pm[pm].ports[i]);
        }
    }

    /* Keep the same logical port if possible */
    for (pm = 0; pm < dev->num_pm; pm++) {
        if ((f->pm[pm].pm_str == NULL) ||
            (f->pm[pm].port_cnt == 0) || /* mode is "None" */
            (f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        cnt = 0;
        for (i = 0; i < f->pm[pm].port_cnt; i++ ) {
            phy_port = f->pm[pm].pr[i].physical_port;
            port = bcmi_ltsw_pport_to_port(unit, phy_port);
            if ((port != -1) && BCM_PBMP_MEMBER(free_pbmp, port)) {
                f->pm[pm].pr[i].port = port;
                BCM_PBMP_PORT_REMOVE(free_pbmp, port);
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
    for (pm = 0; pm < dev->num_pm; pm++) {
        if ((f->pm[pm].pm_str == NULL) ||
            (f->pm[pm].port_cnt == 0) || /* mode is "None" */
            (f->pm[pm].flags & FLXP_F_PORT_ASSIGNED)) {
            continue;
        }
        pipe = pm /pm_per_pipe;
        for (i = 0; i < f->pm[pm].port_cnt; i++ ) {
            if ( f->pm[pm].pr[i].port != -1) {
                continue;
            }
            for (port = ffport[pipe]; port <= pbs[pipe].end; port++) {
                if (BCM_PBMP_MEMBER(free_pbmp, port)) {
                    f->pm[pm].pr[i].port = port;
                    BCM_PBMP_PORT_REMOVE(free_pbmp, port);
                    break;
                }
            }
            if (port > pbs[pipe].end) {
                cli_out("No free port.\n");
                return CMD_FAIL;
            } else {
                ffport[pipe] = port + 1;
            }
        }
        f->pm[pm].flags |= FLXP_F_PORT_ASSIGNED;
    }

    return CMD_OK;
}

static cmd_result_t
ltsw_flexport_diag(int unit, ltsw_flexport_info_t *f)
{
    bcm_port_resource_t *pr;
    int phy_port, phy_base, pm, port ;
    int nports, pr_idx, len, i;
    cmd_result_t retCode = CMD_OK;
    int rv = BCM_E_NONE;
    int max_ports = 0;
    ltsw_dev_info_t *dev;

    dev = ltsw_dev_lookup(unit);

    nports = f->del_cnt + f->add_cnt;
    if (nports > dev->num_port * 2) {
        cli_out("Flexport: Entry num exceeds the Maxium(%d).\n", dev->num_port * 2);
        return CMD_FAIL;
    }

    pr = sal_alloc(dev->num_port * 2 * sizeof(bcm_port_resource_t),
                   "PortResource");
    if (pr == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }
    sal_memset(pr, 0, dev->num_port * 2 * sizeof(bcm_port_resource_t));

    max_ports = dev->num_lane_per_pm;
    pr_idx = 0;

    /* Del entries */
    for (pm = 0; pm < dev->num_pm; pm++) {
        if (f->pm[pm].pm_str == NULL) {
            continue;
        }
        phy_base = (pm * max_ports) + 1;
        for(i = 0; i < max_ports; i++) {
            phy_port = phy_base + i;
            port = bcmi_ltsw_pport_to_port(unit, phy_port);
            if (port != -1) {
                pr[pr_idx].port = port;
                pr[pr_idx].physical_port = -1;
                pr_idx++;
            }
        }
    }

    /* Add entries */
    for (pm = 0; pm < dev->num_pm; pm++) {
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
        cli_out("Flexport: Operation failed (%d).\n", rv);
        retCode = CMD_FAIL;
    } else {
        retCode = CMD_OK;
    }

    sal_free(pr);

    return retCode;
}

cmd_result_t
cmd_ltsw_flexport(int unit, args_t *a)
{
    ltsw_flexport_info_t *flexinfo;
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
    ltsw_dev_info_t *dev;

    dev = ltsw_dev_lookup(unit);
    if (dev == NULL) {
        cli_out("Unknown device.\n");
        return CMD_FAIL;
    }

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((arg = ARG_CUR(a)) == NULL) {
        return (CMD_USAGE);
    }

    flexinfo = sal_alloc(sizeof(ltsw_flexport_info_t), "if_flexport");
    if (flexinfo == NULL) {
        cli_out("Insufficient memory.\n");
        return CMD_FAIL;
    }
    sal_memset(flexinfo, 0, sizeof(ltsw_flexport_info_t));

    parse_table_init(unit, &pt);
    for (i = 0; i <  dev->num_pm; i++) {
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
        retCode = ltsw_flexport_show(unit, flexinfo);
    } else if (parsed) {
        auto_assign = 0;
        retCode = ltsw_flexport_parse_portlist(unit, flexinfo->pm);
        if (retCode == CMD_OK) {
            retCode = ltsw_flexport_parse_pm_mode(unit, flexinfo, &auto_assign);
        }
        if (retCode == CMD_OK && auto_assign) {
            retCode = ltsw_flexport_port_assign(unit, flexinfo);
        }
        if (retCode == CMD_OK) {
            retCode = ltsw_flexport_diag(unit, flexinfo);
        }
    }
    parse_arg_eq_done(&pt);
    sal_free(flexinfo);
    return retCode;
}


#endif /* BCM_LTSW_SUPPORT */
