/** \file diag_dnxc_linkscan.c
 *
 * linkscan diagnostic pack for fabric
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/* shared */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
/* appl */
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>

/*bcm*/
#include <bcm/fabric.h>
#include <bcm/port.h>
#include <bcm/link.h>

/*soc*/
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_linkscan.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>
#endif
#include <shared/cmicfw/cmicx_link.h>

static int linkscan_interval_saved[BCM_MAX_NUM_UNITS];


/* *INDENT-OFF* */
sh_sand_option_t dnxc_linkscan_options[] = {
    {"interval", SAL_FIELD_TYPE_INT32, "Configure linkscan polling interval", "-1", NULL, NULL, SH_SAND_ARG_FREE},
    {"on", SAL_FIELD_TYPE_BOOL, "Enable linkscan", "No"},
    {"yes", SAL_FIELD_TYPE_BOOL, "Enable linkscan", "No"},
    {"enable", SAL_FIELD_TYPE_BOOL, "Enable linkscan", "No"},
    {"off", SAL_FIELD_TYPE_BOOL, "Disable linkscan", "No"},
    {"no", SAL_FIELD_TYPE_BOOL, "Disable linkscan", "No"},
    {"disable", SAL_FIELD_TYPE_BOOL, "Disable linkscan", "No"},
    {"SwPortBitMap", SAL_FIELD_TYPE_PORT, "Enables software linkscan on ports specified by SwPortBitMap. Disables linkscan on all other ports.", "all"},
    {"HwPortBitMap", SAL_FIELD_TYPE_PORT, "Enables hardware linkscan on ports specified by HwPortBitMap. Disables linkscan on all other ports.", "all"},
    {"force", SAL_FIELD_TYPE_PORT, "Requests linkscan to update ports once even if link status did not change.", "none"},

    {NULL}
};

/* *INDENT-ON* */

static void
_print_linkscan_status(
    int unit,
    args_t * args,
    int us,
    bcm_pbmp_t * pbm_sw,
    bcm_pbmp_t * pbm_hw,
    bcm_pbmp_t * pbm_all)
{
    char buf[FORMAT_PBMP_MAX];
    char pfmt[SOC_PBMP_FMT_LEN];
    bcm_pbmp_t pbm_dis, pbm_en;

    if (us)
    {
        cli_out("%s: Linkscan enabled\n", ARG_CMD(args));
        cli_out("%s:   Software polling interval: %d usec\n", ARG_CMD(args), us);
        format_bcm_pbmp(unit, buf, sizeof(buf), *pbm_sw);
        cli_out("%s:   Software Port BitMap %s: %s\n", ARG_CMD(args), SOC_PBMP_FMT(*pbm_sw, pfmt), buf);
        format_bcm_pbmp(unit, buf, sizeof(buf), *pbm_hw);
        cli_out("%s:   Hardware Port BitMap %s: %s\n", ARG_CMD(args), SOC_PBMP_FMT(*pbm_hw, pfmt), buf);
        BCM_PBMP_ASSIGN(pbm_en, *pbm_sw);
        BCM_PBMP_OR(pbm_en, *pbm_hw);
        BCM_PBMP_ASSIGN(pbm_dis, *pbm_all);
        BCM_PBMP_REMOVE(pbm_dis, pbm_en);
        format_bcm_pbmp(unit, buf, sizeof(buf), pbm_dis);
        cli_out("%s:   Disabled Port BitMap %s: %s\n", ARG_CMD(args), SOC_PBMP_FMT(pbm_dis, pfmt), buf);
    }
    else
    {
        cli_out("%s: Linkscan disabled\n", ARG_CMD(args));
    }

}


shr_error_e
cmd_dnxc_linkscan(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int on_flag, yes_flag, enable_flag, off_flag, no_flag, disable_flag;
    int linkscan_interval_default, mode, us_var, us, rv;
    bcm_pbmp_t pbm_sw, pbm_hw, pbm_force, pbm_en, pbm_both;
    bcm_pbmp_t pbm_sw_pre, pbm_hw_pre, pbm_need_to_dis;
    int is_pbm_sw_present, is_pbm_hw_present, is_force_present;
    int on_state, off_state;
    bcm_port_config_t pcfg;
    soc_port_t port;
    char pfmt[SOC_PBMP_FMT_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("interval", us_var);
    SH_SAND_GET_BOOL("on", on_flag);
    SH_SAND_GET_BOOL("yes", yes_flag);
    SH_SAND_GET_BOOL("enable", enable_flag);
    SH_SAND_GET_BOOL("off", off_flag);
    SH_SAND_GET_BOOL("no", no_flag);
    SH_SAND_GET_BOOL("disable", disable_flag);
    SH_SAND_IS_PRESENT("SwPortBitMap", is_pbm_sw_present);
    SH_SAND_IS_PRESENT("HwPortBitMap", is_pbm_hw_present);
    SH_SAND_IS_PRESENT("force", is_force_present);
    SH_SAND_GET_PORT("force", pbm_force);


#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        linkscan_interval_default = BCM_LINKSCAN_INTERVAL_DEFAULT;
    }
    else
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        linkscan_interval_default = dnxf_data_linkscan.general.interval_get(unit);
    }
    else
#endif
    {
        SHR_CLI_EXIT(_SHR_E_UNIT, "%s: Error: Device types other than DNX/DNXF are not supported\n", ARG_CMD(args));
    }

    on_state = ((us_var != 0) && (us_var != -1)) || on_flag || yes_flag
        || enable_flag || is_pbm_sw_present || is_pbm_hw_present || is_force_present;
    off_state = (us_var == 0) || off_flag || no_flag || disable_flag;

    if (off_state && on_state)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "%s: Error: Options turning linkscan off cannot be used with any other options\n",
                     ARG_CMD(args));
    }
    else if (off_state) /* turn off linkscan */
    {
        rv = bcm_linkscan_enable_get(unit, &us);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Failed to retrieve linkscan status: %s%s\n",
                                 ARG_CMD(args), bcm_errmsg(rv), EMPTY);

        us = us > 0 ? us : (linkscan_interval_saved[unit] > 0 ?
                            linkscan_interval_saved[unit] : linkscan_interval_default);

        linkscan_interval_saved[unit] = us;

        rv = bcm_linkscan_enable_set(unit, 0);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Failed to disable linkscan: %s%s\n",
                                 ARG_CMD(args), bcm_errmsg(rv), EMPTY);
    }
    else
    {
        rv = bcm_port_config_get(unit, &pcfg);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: bcm ports not initialized %s%s\n", ARG_CMD(args), EMPTY, EMPTY);

        rv = bcm_linkscan_enable_get(unit, &us);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Failed to retrieve linkscan status: %s%s\n", ARG_CMD(args),
                                 bcm_errmsg(rv), EMPTY);

        BCM_PBMP_CLEAR(pbm_sw_pre);
        BCM_PBMP_CLEAR(pbm_hw_pre);

        BCM_PBMP_ITER(pcfg.port, port)
        {
            rv = bcm_linkscan_mode_get(unit, port, &mode);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Failed to retrieve linkscan mode: %s%s\n",
                                     ARG_CMD(args), bcm_errmsg(rv), EMPTY);

            switch (mode)
            {
                case BCM_LINKSCAN_MODE_SW:
                    BCM_PBMP_PORT_ADD(pbm_sw_pre, port);
                    break;
                case BCM_LINKSCAN_MODE_HW:
                    BCM_PBMP_PORT_ADD(pbm_hw_pre, port);
                    break;
                default:
                    break;
            }
        }

        /*
         * if no option presents, just print linkscan status
         */
        if ((us_var == -1) &&
            !on_flag && !yes_flag && !enable_flag &&
            !off_flag && !no_flag && !disable_flag && !is_pbm_sw_present && !is_pbm_hw_present && !is_force_present)
        {
            _print_linkscan_status(unit, args, us, &pbm_sw_pre, &pbm_hw_pre, &pcfg.port);
            SHR_EXIT();
        }

        us = ((us_var != -1) ? us_var : (us ? us : (linkscan_interval_saved[unit] ? linkscan_interval_saved[unit] :
                                                    linkscan_interval_default)));

        /*
         * valid SW/HW port bitmap received
         */
        if (is_pbm_sw_present || is_pbm_hw_present)
        {
            if (is_pbm_sw_present)
            {
                SH_SAND_GET_PORT("SwPortBitMap", pbm_sw);
                BCM_PBMP_AND(pbm_sw, pcfg.port);
            }
            else
            {
                BCM_PBMP_ASSIGN(pbm_sw, pbm_sw_pre);
            }

            if (is_pbm_hw_present)
            {
                SH_SAND_GET_PORT("HwPortBitMap", pbm_hw);
                BCM_PBMP_AND(pbm_hw, pcfg.port);
                BCM_PBMP_REMOVE(pbm_hw, pcfg.il);
            }
            else
            {
                BCM_PBMP_ASSIGN(pbm_hw, pbm_hw_pre);
            }
            /*
             * just not-allow flexe ports to be set to HW linkscan.
             */
            BCM_PBMP_ASSIGN(pbm_both, pbm_hw);
            BCM_PBMP_AND(pbm_both, PBMP_FLEXE_PHY_ALL(unit));
            if (BCM_PBMP_NOT_NULL(pbm_both))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "%s: Error: FlexE phy port can't use hardware linkscan,"
                             "please use software linkscan instead.\n", ARG_CMD(args));
            }
            /*
             * a port is not allowed in both HW and SW bitmap
             */
            BCM_PBMP_ASSIGN(pbm_both, pbm_sw);
            BCM_PBMP_AND(pbm_both, pbm_hw);
            if (BCM_PBMP_NOT_NULL(pbm_both))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "%s: Error: Same port can't use both "
                             "software and hardware linkscan\n", ARG_CMD(args));
            }

            /*
             * calculate the port bitmap that previous enabled but need to disable
             */
            BCM_PBMP_ASSIGN(pbm_en, pbm_sw);
            BCM_PBMP_OR(pbm_en, pbm_hw);
            BCM_PBMP_ASSIGN(pbm_need_to_dis, pbm_sw_pre);
            BCM_PBMP_OR(pbm_need_to_dis, pbm_hw_pre);
            BCM_PBMP_REMOVE(pbm_need_to_dis, pbm_en);

            rv = bcm_linkscan_mode_set_pbm(unit, pbm_sw, BCM_LINKSCAN_MODE_SW);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Failed to set software link scanning: PBM=%s: %s\n",
                                     ARG_CMD(args), SOC_PBMP_FMT(pbm_sw, pfmt), bcm_errmsg(rv));

            rv = bcm_linkscan_mode_set_pbm(unit, pbm_hw, BCM_LINKSCAN_MODE_HW);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Failed to set hardware link scanning: PBM=%s: %s\n",
                                     ARG_CMD(args), SOC_PBMP_FMT(pbm_hw, pfmt), bcm_errmsg(rv));

            rv = bcm_linkscan_mode_set_pbm(unit, pbm_need_to_dis, BCM_LINKSCAN_MODE_NONE);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Failed to disable link scanning: PBM=%s: %s\n",
                                     ARG_CMD(args), SOC_PBMP_FMT(pbm_need_to_dis, pfmt), bcm_errmsg(rv));
        }

        rv = bcm_linkscan_enable_set(unit, us);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Error: Failed to enable linkscan: %s%s\n",
                                 ARG_CMD(args), bcm_errmsg(rv), EMPTY);

        if (!BCM_PBMP_IS_NULL(pbm_force))
        {
            BCM_PBMP_AND(pbm_force, pcfg.port);
            rv = bcm_link_change(unit, pbm_force);
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "%s: Failed to force link scan: PBM=%s: %s\n",
                                     ARG_CMD(args), SOC_PBMP_FMT(pbm_force, pfmt), bcm_errmsg(rv));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * } LOCAL DIAG PACK
 */

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
#ifdef BCM_DNXF_SUPPORT
sh_sand_man_t sh_dnxf_linkscan_man = {
    .brief = "Misc facilities for configuring or displaying linkscan",
    .full = "With no arguments, displays the linkscan status for all ports.\n"
            "Enables software linkscan on ports specified by SwPortBitMap.\n"
            "Enables hardware linkscan on ports specified by HwPortBitMap.\n"
            "Disables linkscan on all other ports.\n"
            "Interval specifies non-default linkscan interval for software.\n"
            "Note: With linkscan disabled, autonegotiated ports will NOT have the MACs updated with speed/duplex\n"
            "Forces a transient link down event on ports specified by FORCE",
    .synopsis = "[SwPortBitMap=<pbmp>] [HwPortBitMap=<pbmp>] [Interval=<usec>] [FORCE=<pbmp>] [on] [off]",
    .examples = "on\n"
                "yes\n"
                "enable\n"
                "off\n"
                "no\n"
                "disable\n"
                "force=sfi\n"
                "interval=100000\n"
                "spbm=sfi100 hpbm=none\n"
                "spbm=none hpbm=101\n"
                "spbm=none hpbm=sfi100-sfi109\n"
                "200000\n",
};
#endif

#ifdef BCM_DNX_SUPPORT
sh_sand_man_t sh_dnx_linkscan_man = {
    .brief = "Misc facilities for configuring or displaying linkscan.",
    .full = "Without arguments command displays the linkscan status for all ports.\n"
            "Enables software linkscan on ports specified by SwPortBitMap.\n"
            "Enables hardware linkscan on ports specified by HwPortBitMap.\n"
            "Disables linkscan on all other ports.\n"
            "Interval specifies non-default linkscan interval for software.\n"
            "Note: With linkscan disabled, autonegotiated ports will NOT have the MACs updated with speed/duplex\n"
            "Forces a transient link down event on ports specified by FORCE",
    .synopsis = "[SwPortBitMap=<pbmp>] [HwPortBitMap=<pbmp>] [Interval=<usec>] [FORCE=<pbmp>] [on] [off]",
    .examples = "on\n"
                "yes\n"
                "enable\n"
                "off\n"
                "no\n"
                "disable\n"
                "force=xe\n"
                "interval=100000\n"
                "hpbm=14-17 spbm=none\n"
                "spbm=13 hpbm=none\n"
                "spbm=sfi hpbm=none\n"
                "200000\n",
};
#endif
/* *INDENT-ON* */
