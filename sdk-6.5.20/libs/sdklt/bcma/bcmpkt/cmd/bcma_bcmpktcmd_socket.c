/*! \file bcma_bcmpktcmd_socket.c
 *
 * BCMPKT SOCKET commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_socket.h>
#include "bcma_bcmpktcmd_internal.h"
#include <bcma/bcmpkt/bcma_bcmpkt_rx.h>

static bcma_cli_parse_enum_t socket_types[] = {
    {"AUTO",       BCMPKT_SOCKET_DRV_T_AUTO},
    {"TPacKeT",    BCMPKT_SOCKET_DRV_T_TPKT},
    {"RawSocket",  BCMPKT_SOCKET_DRV_T_RAWSOCK},
    {NULL,   0}
};

static int
cmd_socket_attach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    uint32_t type = BCMPKT_SOCKET_DRV_T_AUTO;
    bcma_cli_parse_table_t pt;
    bcmpkt_socket_drv_types_t drv_type;

    rv = bcmpkt_socket_drv_type_get(unit, &drv_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get SOCKET driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (drv_type != BCMPKT_SOCKET_DRV_T_NONE) {
        cli_out("%sDriver exists on unit: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, drv_type);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Type", "enum", &type, socket_types);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    rv = bcmpkt_socket_drv_attach(unit, type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit, "Attach SOCKET driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_socket_detach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;

    /* Destroy all packet watchers on the unit. */
    bcma_bcmpktcmd_watcher_destroy(unit, -1);

    rv = bcmpkt_socket_drv_detach(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit, "Detach SOCKET driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_socket_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int unit = cli->cmd_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);
    bcmpkt_socket_cfg_t socket_cfg = {
        .rx_poll_timeout = 1000
    };
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

    cmd = BCMA_CLI_ARG_GET(args);
    /* If not configure netif_id, use default netif. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (netif_id <= 0) {
        cli_out("Invalid network interface ID %d\n", netif_id);
        return BCMA_CLI_CMD_USAGE;
    }
    rv = bcmpkt_socket_create(unit, netif_id, &socket_cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit,
                              "Create SOCKET on network interface %d failed (%d)\n"),
                   netif_id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_rx_dispatcher_create(unit, netif_id, 0,
                                          bcma_bcmpkt_packet_process);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit,
                              "Create Rx dispatcher on network interface %d failed (%d)\n"),
                   netif_id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_socket_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int unit = cli->cmd_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);
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

    cmd = BCMA_CLI_ARG_GET(args);
    /* If not configure netif_id, use default netif. */
    if (cmd) {
        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (netif_id <= 0) {
        cli_out("Invalid network interface ID %d\n", netif_id);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Destroy packet watcher from the netif. */
    bcma_bcmpktcmd_watcher_destroy(unit, netif_id);

    /* Destroy Rx dispatcher from the netif. */
    rv = bcma_bcmpkt_rx_dispatcher_destroy(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit,
                              "Destroy Rx dispatcher on network interface %d failed (%d)\n"),
                   netif_id, rv));
    }

    rv = bcmpkt_socket_destroy(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit,
                              "Destroy SOCKET on network interface %d failed (%d)\n"),
                   netif_id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static void
show_socket_exist_info(int netif_id, bool created)
{
    if (created) {
        cli_out("  Network interface %d: SOCKET supported\n", netif_id);
    } else {
        cli_out("  Network interface %d: SOCKET not supported\n", netif_id);
    }
}

static int
socket_exist_info(int unit, const bcmpkt_netif_t *netif, void *cb_data)
{
    if (netif) {
        int rv;
        bool created;
        rv = bcmpkt_socket_created(unit, netif->id, &created);
        if (SHR_SUCCESS(rv)) {
            show_socket_exist_info(netif->id, created);
        } else {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get SOCKET created status failed (%d)\n"),
                       rv));
            return rv;
        }
    }
    return SHR_E_NONE;
}

static int
cmd_socket_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int unit = cli->cmd_unit;
    int netif_id = bcma_bcmpkt_netif_defid_get(unit);
    bool enabled;
    bool created;
    bcmpkt_socket_drv_types_t drv_type;

    rv = bcmpkt_socket_drv_type_get(unit, &drv_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get SOCKET type failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("SOCKET information:\n");
    cli_out("  Driver: ");
    if (drv_type == BCMPKT_SOCKET_DRV_T_NONE) {
        cli_out("Not attached\n");
    } else if (drv_type == BCMPKT_SOCKET_DRV_T_TPKT) {
        cli_out(" Packet_mmap\n");
    } else if (drv_type == BCMPKT_SOCKET_DRV_T_RAWSOCK) {
        cli_out("Raw socket\n");
    } else {
        cli_out("Unknown - %d\n", drv_type);
    }

    rv = bcmpkt_dev_enabled(unit, &enabled);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device running status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (enabled) {
        cmd = BCMA_CLI_ARG_GET(args);
        /* If not configure netif_id dump all. */
        if (!cmd) {
            rv = bcmpkt_netif_traverse(unit, socket_exist_info, NULL);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_KNET,
                          (BSL_META_U(unit,
                                      "Traverse network interfaces failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }
            return BCMA_CLI_CMD_OK;
        }

        netif_id = sal_strtol(cmd, &cmd, 0);
        if (*cmd != 0 || netif_id < 0) {
            cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
            return BCMA_CLI_CMD_USAGE;
        }

        cli_out("SOCKET information:\n");
        rv = bcmpkt_socket_created(unit, netif_id, &created);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get SOCKET created status failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        show_socket_exist_info(netif_id, created);
    }

    return BCMA_CLI_CMD_OK;
}


static int
socket_stats_dump_cb(int unit, const bcmpkt_netif_t *netif, void *data)
{
    if (netif != NULL) {
        int rv;
        bool created;
        rv = bcmpkt_socket_created(unit, netif->id, &created);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get SOCKET created status failed (%d)\n"),
                       rv));
            return rv;
        }
        if (created) {
            shr_pb_t *pb = (shr_pb_t *)data;
            if (!pb) {
                return SHR_E_PARAM;
            }

            shr_pb_printf(pb, "[Network interface %d]\n", netif->id);
            return bcmpkt_socket_stats_dump(unit, netif->id, pb);
        }
    }

    return SHR_E_NONE;
}

static int
cmd_socket_stats_show(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int netif_id;
    int unit = cli->cmd_unit;
    shr_pb_t *pb;
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

    cmd = BCMA_CLI_ARG_GET(args);
    /* Show all if no netif ID input. */
    if (!(cmd)) {
        pb = shr_pb_create();
        rv = bcmpkt_netif_traverse(unit, socket_stats_dump_cb, pb);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SOCKET,
                      (BSL_META_U(unit, "Traverse network interface failed (%d)\n"),
                       rv));
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        cli_out("SOCKET statistics:\n");
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    netif_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif_id < 0) {
        cli_out("%sInvalid option: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_USAGE;
    }
    pb = shr_pb_create();
    rv = bcmpkt_socket_stats_dump(unit, netif_id, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit, "Dump SOCKET stats failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("SOCKET statistics:\n[network interface %d]\n", netif_id);
    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_socket_stats_clear(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *cmd;
    int netif_id;
    int unit = cli->cmd_unit;
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

    netif_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || netif_id < 0) {
        cli_out("Invalid option: %s\n",  cmd);
        return BCMA_CLI_CMD_USAGE;
    }
    rv = bcmpkt_socket_stats_clear(unit, netif_id);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SOCKET,
                  (BSL_META_U(unit, "Clear SOCKET stats failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_socket(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("ATtach", cmd,  ' ')) {
        return cmd_socket_attach(cli, args);
    }
    if (bcma_cli_parse_cmp("DeTach", cmd,  ' ')) {
        return cmd_socket_detach(cli, args);
    }

    if (bcma_cli_parse_cmp("Create", cmd,  ' ')) {
        return cmd_socket_create(cli, args);
    }
    if (bcma_cli_parse_cmp("Destroy", cmd,  ' ')) {
        return cmd_socket_destroy(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd,  ' ')) {
        return cmd_socket_info(cli, args);
    }
    if (bcma_cli_parse_cmp("Stats", cmd,  ' ')) {
        if (!(cmd = BCMA_CLI_ARG_GET(args))) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (bcma_cli_parse_cmp("Show", cmd,  ' ')) {
            return cmd_socket_stats_show(cli, args);
        }
        if (bcma_cli_parse_cmp("Clear", cmd,  ' ')) {
            return cmd_socket_stats_clear(cli, args);
        }
    }

    cli_out("%sUnknown SOCKET command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_USAGE;
}

