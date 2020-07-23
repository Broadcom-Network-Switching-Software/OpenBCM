/*! \file bcma_bcmpktcmd_rx.c
 *
 * BCMPKT RX commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_bitop.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_rx.h>
#include <bcma/bcmpkt/bcma_bcmpkt_rx.h>
#include "bcma_bcmpktcmd_internal.h"

typedef struct px_watcher_s {
    int show_pkt_data;
    int show_meta_data;
    int show_rx_reason;
    int lb_pkt_data;
    int show_rx_rate;
    int terminate_packet;
} px_watcher_t;

static watcher_data_t *wdata_list[BCMDRD_CONFIG_MAX_UNITS];

static bcma_cli_parse_enum_t reason_nm_enum[] = {
    BCMPKT_REASON_NAME_MAP_INIT
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {#_bd, BCMDRD_DEV_T_##_bd},
static shr_enum_map_t device_types[] = {
#include <bcmdrd/bcmdrd_devlist.h>
};

static void
watcher_cfg_api2px(uint32_t *flags, px_watcher_t *px_cfg)
{
    px_cfg->show_pkt_data =
        bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_SHOW_PACKET_DATA);
    px_cfg->show_meta_data =
        bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_SHOW_META_DATA);
    px_cfg->show_rx_reason =
        bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_SHOW_RX_REASON);
    px_cfg->lb_pkt_data =
        bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_LPBK_PACKET);
    px_cfg->show_rx_rate =
        bcma_bcmpkt_flag_status_get(*flags, WATCHER_DEBUG_SHOW_RX_RATE);
}

static void
watcher_cfg_px2api(const px_watcher_t *px_cfg, uint32_t *flags)
{
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_SHOW_PACKET_DATA,
                         px_cfg->show_pkt_data);
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_SHOW_META_DATA,
                         px_cfg->show_meta_data);
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_SHOW_RX_REASON,
                         px_cfg->show_rx_reason);
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_LPBK_PACKET,
                         px_cfg->lb_pkt_data);
    bcma_bcmpkt_flag_set(flags, WATCHER_DEBUG_SHOW_RX_RATE,
                         px_cfg->show_rx_rate);
}

static void
watcher_cfg_dump(shr_pb_t *pb,  watcher_data_t *wdata)
{
    shr_pb_printf(pb, "\nWatcher Debug Configuration:\n");
    shr_pb_printf(pb, "\tShowPacketData: %s\n",
                  (wdata->debug_mode & WATCHER_DEBUG_SHOW_PACKET_DATA) ? "True" : "False");
    shr_pb_printf(pb, "\tShowMetaData: %s\n",
                  (wdata->debug_mode & WATCHER_DEBUG_SHOW_META_DATA) ? "True" : "False");
    shr_pb_printf(pb, "\tShowRxReason: %s\n",
                  (wdata->debug_mode & WATCHER_DEBUG_SHOW_RX_REASON) ? "True" : "False");
    shr_pb_printf(pb, "\tLoopbackData: %s\n",
                  (wdata->debug_mode & WATCHER_DEBUG_LPBK_PACKET) ? "True" : "False");
    shr_pb_printf(pb, "\tShowRxRate: %s\n",
                  (wdata->debug_mode & WATCHER_DEBUG_SHOW_RX_RATE) ? "True" : "False");
    shr_pb_printf(pb, "\tTerminateNetif: %d\n", wdata->term_netif);
    shr_pb_printf(pb, "\tTerminateVlan: %d\n", wdata->term_vlan);
    shr_pb_printf(pb, "\tTerminateMatchID: %d\n", wdata->term_match_id);
}

static int
watcher_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    int netif_id = BCMA_BCMPKT_NETIF_ALL;
    watcher_data_t *wdata = NULL, *pos;
    char *cmd;
    bcma_cli_parse_table_t pt;
    px_watcher_t px_cfg;
    int rv;
    shr_pb_t *pb;

    cmd = BCMA_CLI_ARG_GET(args);
    /* If no netif_id input, create the global watcher. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || !bcma_bcmpkt_rx_netif_valid(unit, netif_id)) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    wdata = sal_alloc(sizeof(watcher_data_t), "bcmaCliRxWatcherCreate");
    if (wdata == NULL) {
        LOG_ERROR(BSL_LS_APPL_RX,
                  (BSL_META_U(unit, "Allocate wdata failed\n")));
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(wdata, 0, sizeof(*wdata));
    wdata->netif_id = netif_id;
    /* Show Packet raw data, meta data and RX reason by default */
    wdata->debug_mode = WATCHER_DEBUG_SHOW_PACKET_DATA |
                        WATCHER_DEBUG_SHOW_META_DATA |
                        WATCHER_DEBUG_SHOW_RX_REASON;

    /* Not terminate nay packet by default */
    wdata->term_netif = -1;
    wdata->term_vlan = -1;
    wdata->term_match_id = -1;

    /* Create mutex lock for printing debug info */
    wdata->pb_mutex = sal_mutex_create("bcmaCliRxWatcher");
    if (wdata->pb_mutex == NULL) {
        LOG_ERROR(BSL_LS_APPL_RX,
                  (BSL_META_U(unit, "Create Rx watcher mutex failed\n")));
        sal_free(wdata);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_bcmpkt_parse_data_add();
    watcher_cfg_api2px(&wdata->debug_mode, &px_cfg);
    bcma_cli_parse_table_add(&pt, "ShowPacketData", "bool",
                             &px_cfg.show_pkt_data, NULL);
    bcma_cli_parse_table_add(&pt, "ShowMetaData", "bool",
                             &px_cfg.show_meta_data, NULL);
    bcma_cli_parse_table_add(&pt, "ShowRXReason", "bool",
                             &px_cfg.show_rx_reason, NULL);
    bcma_cli_parse_table_add(&pt, "LoopbackData", "bool",
                             &px_cfg.lb_pkt_data, NULL);
    bcma_cli_parse_table_add(&pt, "ShowRxRate", "bool",
                             &px_cfg.show_rx_rate, NULL);
    bcma_cli_parse_table_add(&pt, "TerminateNetif", "int",
                             &wdata->term_netif, NULL);
    bcma_cli_parse_table_add(&pt, "TerminateVlan", "int",
                             &wdata->term_vlan, NULL);
    bcma_cli_parse_table_add(&pt, "TerminateMatchID", "int",
                             &wdata->term_match_id, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        sal_mutex_destroy(wdata->pb_mutex);
        sal_free(wdata);
        return BCMA_CLI_CMD_USAGE;
    }

    watcher_cfg_px2api(&px_cfg, &wdata->debug_mode);
    bcma_cli_parse_table_done(&pt);
    pb = shr_pb_create();
    watcher_cfg_dump(pb, wdata);
    LOG_VERBOSE(BSL_LS_APPL_RX,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    for (pos = wdata_list[unit]; pos != NULL; pos = pos->next){
        if (pos->netif_id == netif_id) {
            LOG_WARN(BSL_LS_APPL_RX,
                     (BSL_META_U(unit,
                                 "RX watcher already exists on network interface %d\n"),
                      netif_id));
            sal_mutex_destroy(wdata->pb_mutex);
            sal_free(wdata);
            return BCMA_CLI_CMD_OK;
        }
    }
    rv = bcma_bcmpkt_rx_register(unit, netif_id, 0, BCMA_BCMPKT_CB_PRIO_HIGHEST,
                                 "bcmaCliRxWatcher", bcma_bcmpkt_watcher, wdata);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LS_APPL_RX,
                 (BSL_META_U(unit, "Create RX watcher failed (%d)\n"),
                  rv));
        sal_mutex_destroy(wdata->pb_mutex);
        wdata->pb_mutex = NULL;
        sal_free(wdata);
        return BCMA_CLI_CMD_FAIL;
    }

    wdata->next = NULL;
    if (wdata_list[unit] == NULL) {
        wdata_list[unit] = wdata;
    }
    else {
        pos = wdata_list[unit];
        while (pos->next != NULL) {
            pos = pos->next;
        }
        pos->next = wdata;
    }
    LOG_VERBOSE(BSL_LS_APPL_RX,
                (BSL_META_U(unit,
                            "Create RX watcher on network interface %d succeeded\n"),
                 netif_id));

    return BCMA_CLI_CMD_OK;
}

static int
watcher_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    int netif_id = BCMA_BCMPKT_NETIF_ALL;
    char *cmd;

    cmd = BCMA_CLI_ARG_GET(args);
    /* If no netif_id input, remove watcher from all netifs. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || !bcma_bcmpkt_rx_netif_valid(unit, netif_id)) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    bcma_bcmpktcmd_watcher_destroy(unit, netif_id);

    return BCMA_CLI_CMD_OK;
}

static int
watcher_output(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_cli_parse_table_t pt;
    char *outputfile = NULL;
    int file_en = 0; /* No file logging by default */
    int console_en = 1; /* Enable console display by default */
    int rv = SHR_E_NONE;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_bcmpkt_parse_data_add();
    bcma_cli_parse_table_add(&pt, "OutputFile", "str",
                             &outputfile, NULL);
    bcma_cli_parse_table_add(&pt, "LogEnable", "bool",
                             &file_en, NULL);
    bcma_cli_parse_table_add(&pt, "ConsoleEnable", "bool",
                             &console_en, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if ((outputfile != NULL) && sal_strlen(outputfile)) {
        rv = bcma_bcmpkt_rx_watch_output_file_set(outputfile);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_EXISTS) {
                cli_out("%sRX watcher log file %s already specified.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, outputfile);
            } else {
                cli_out("%sFailed to create RX watcher log file.\n",
                        BCMA_CLI_CONFIG_ERROR_STR);
            }
            return BCMA_CLI_CMD_FAIL;
        }
    }

    rv = bcma_bcmpkt_rx_watch_output_enable(file_en, console_en);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_CONFIG) {
            cli_out("%sRX watcher log file not specified.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to enable RX watcher log file.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        }
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_done(&pt);

    return BCMA_CLI_CMD_OK;
}

static int
watcher_cmd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    char* cmd;
    bool enabled;

    rv = bcmpkt_dev_enabled(unit, &enabled);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device running status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!enabled) {
        cli_out("%sPacket device is down: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return watcher_create(cli, args);
    }
    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return watcher_destroy(cli, args);
    }
    if (bcma_cli_parse_cmp("Output", cmd, ' ')) {
        return watcher_output(cli, args);
    }

    cli_out("%sUnknown RX Watcher command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
rxpmd_fields_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bcma_cli_parse_table_t pt;
    bcmdrd_dev_type_t dev_type;
    char *view_name = NULL;
    shr_pb_t *pb;

    rv = bcmpkt_dev_type_get(unit, &dev_type);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "DeviceType", "enum",
                             &dev_type, device_types);
    bcma_cli_parse_table_add(&pt, "ViewName", "str",
                             &view_name, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    pb = shr_pb_create();
    rv = bcmpkt_rxpmd_field_list_dump(dev_type, view_name, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_TX,
                  (BSL_META_U(unit, "Dump field list failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("RXPMD field list:\n");
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
reason_fields_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int i;
    for (i=0; i < BCMPKT_RX_REASON_COUNT; i++) {
        cli_out("    %s\n", reason_nm_enum[i].name);
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_rx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Watcher", cmd,  ' ')) {
        return watcher_cmd(cli, args);
    }

    if (bcma_cli_parse_cmp("RxPmdList", cmd,  ' ')) {
        return rxpmd_fields_list(cli, args);
    }

    if (bcma_cli_parse_cmp("ReasonList", cmd,  ' ')) {
        return reason_fields_list(cli, args);
    }

    cli_out("%sUnknown RX command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

void
bcma_bcmpktcmd_watcher_destroy(int unit, int netif_id)
{
    int rv;
    watcher_data_t *pos, *pre;

    for (pre = wdata_list[unit], pos = pre; pos != NULL;) {
        if (netif_id == -1) { /* Delete all watchers. */
            pre = pos;
            rv = bcma_bcmpkt_rx_unregister(unit, pos->netif_id,
                                           BCMA_BCMPKT_CB_PRIO_HIGHEST,
                                           bcma_bcmpkt_watcher);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_RX,
                          (BSL_META_U(unit, "Destroy watcher failed\n")));
            }

            pos = pos->next;
            sal_mutex_destroy(pre->pb_mutex);
            sal_free(pre);
        }
        else if (pos->netif_id == netif_id) {/* Found, delete it. */
            rv = bcma_bcmpkt_rx_unregister(unit, pos->netif_id,
                                           BCMA_BCMPKT_CB_PRIO_HIGHEST,
                                           bcma_bcmpkt_watcher);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_RX,
                          (BSL_META_U(unit, "Destroy watcher failed\n")));
            }

            if (pos == wdata_list[unit]) {
                wdata_list[unit] = pos->next;
                sal_mutex_destroy(pos->pb_mutex);
                sal_free(pos);
            }
            else {
                pre->next = pos->next;
                sal_mutex_destroy(pos->pb_mutex);
                sal_free(pos);
            }
            break;
        }
        else { /* Not found, go to next. */
            pre = pos;
            pos = pos->next;
        }
    }

    if (netif_id == -1 && wdata_list[unit] != NULL) { /* Delete all watchers. */
        wdata_list[unit] = NULL;
    }

    rv = bcma_bcmpkt_rx_watch_output_enable(0, 1);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to disable RX watcher log file.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
    }
    rv = bcma_bcmpkt_rx_watch_output_file_set(NULL);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to destroy RX watcher log file.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
    }
}
