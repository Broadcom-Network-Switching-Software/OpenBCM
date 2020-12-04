/*! \file pktio_cmd.c
 *
 * ESW Pktio command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/drv.h>
#include <soc/feature.h>
#include <appl/diag/cmdlist.h>

#if defined(INCLUDE_PKTIO)

typedef enum pktio_mode_state_e {
    ModeOff,
    ModeOn
} pktio_mode_t;

STATIC pktio_mode_t pktio_mode[SOC_MAX_NUM_DEVICES] = {0};

cmd_t bcm_esw_pktio_dyn_cmd[] = {
    {"TX",             cmd_pktio_txn,           cmd_pktio_txn_usage,         "Transmit one or more packets"},
    {"PacketWatcher",  cmd_pktio_pcktwatch,     cmd_pktio_pcktwatch_usage,   "Monitor ports for packets"},
};
int bcm_esw_pktio_dyn_cmd_cnt = COUNTOF(bcm_esw_pktio_dyn_cmd);

int
diag_pktio_mode_enable(int unit)
{
    int i;

    if (pktio_mode[unit] == ModeOn) {
        return 0;
    }
    for (i = 0; i < bcm_esw_pktio_dyn_cmd_cnt; i++) {
        cmdlist_add(unit, &bcm_esw_pktio_dyn_cmd[i]);
    }
    pktio_mode[unit] = ModeOn;

    return 0;
}

int
diag_pktio_mode_disable(int unit)
{
    int i;

    if (pktio_mode[unit] == ModeOff) {
        return 0;
    }

    for (i = 0; i < bcm_esw_pktio_dyn_cmd_cnt; i++) {
        cmdlist_remove(unit, &bcm_esw_pktio_dyn_cmd[i]);
    }
    pktio_mode[unit] = ModeOff;
    return 0;
}

/*
 * Function: cmd_esw_pktio
 * Purpose: Enable pktio command
 * Returns: CMD_USAGE/CMD_FAIL/CMD_OK.
 */
cmd_result_t
cmd_esw_pktio(int unit, args_t *args)
{
    cmd_result_t rv = CMD_FAIL;
    char *subcmd;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("TX/PW using PKTIO mode is %s\n",
                   (pktio_mode[unit] != ModeOff) ? "on" : "off");
        rv = CMD_OK;
    } else if (sal_strcasecmp(subcmd, "ON") == 0) {
        if (!soc_feature(unit, soc_feature_sdklt_pktio)) {
            cli_out("PKTIO is not available when spn_PKTIO_DRIVER_TYPE = 0.\n");
            return CMD_OK;
        }
        (void)diag_pktio_mode_enable(unit);
            rv = CMD_OK;

    } else if (sal_strcasecmp(subcmd, "OFF") == 0) {
        if (soc_feature(unit, soc_feature_sdklt_pktio)) {
            cli_out("Disabling PKTIO mode when spn_PKTIO_DRIVER_TYPE = 1 is not allowed.\n");
            return CMD_OK;
        }
        (void) diag_pktio_mode_disable(unit);
        rv = CMD_OK;
    } else {
        cli_out("PKTIO subcommand '%s' not found\n", subcmd);
        rv = CMD_USAGE;
    }
    return rv;
}

#else

cmd_result_t
cmd_esw_pktio(int unit, args_t *args)
{
    char *subcmd;

    if ((subcmd = ARG_GET(args)) == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_PKTIO,
                    (BSL_META_U(unit,
                                "Dummy command.\n"
                                "Applicable when PKTIO feature is enabled.\n")));
    } else {
        LOG_VERBOSE(BSL_LS_APPL_PKTIO,
                    (BSL_META_U(unit, "Dummy command.\n")));
    }
    return CMD_OK;

}
#endif
