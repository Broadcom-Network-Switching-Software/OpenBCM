/*! \file bcma_bcmpktcmd_pktdev.c
 *
 * CLI commands related to packet device configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_bitop.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pktdev.h>
#include "bcma_bcmpktcmd_internal.h"
#include <bcma/bcmpkt/bcma_bcmpkt_rx.h>
#include <bcmmgmt/bcmmgmt.h>

/*******************************************************************************
 * Local data
 */

static bcma_cli_parse_enum_t udev_types[] = {
    {"AUTO",   BCMPKT_DEV_DRV_T_AUTO},
    {"UNET",   BCMPKT_DEV_DRV_T_UNET},
    {"KNET",   BCMPKT_DEV_DRV_T_KNET},
    {NULL,   0}
};

static bcma_cli_parse_enum_t dma_chan_dir[] = {
    {"TX", BCMPKT_DMA_CH_DIR_TX},
    {"RX", BCMPKT_DMA_CH_DIR_RX},
    {NULL, 1}
};

static int netif_defid[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
dev_cfg_dump(shr_pb_t *pb, bcmpkt_dev_init_t *cfg)
{
    shr_pb_printf(pb, "\nPacket Device Configuration:\n");
    shr_pb_printf(pb, "\tName: %s\n", cfg->name);
    shr_pb_printf(pb, "\tcgrp_size: 0x%x\n", cfg->cgrp_size);
    shr_pb_printf(pb, "\tcgrp_bmp: 0x%x\n", cfg->cgrp_bmp);
    shr_pb_printf(pb, "\tdef_netif_mac_addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  cfg->mac_addr[0], cfg->mac_addr[1],
                  cfg->mac_addr[2], cfg->mac_addr[3],
                  cfg->mac_addr[4], cfg->mac_addr[5]);
}

static int
cmd_dev_attach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    uint32_t type = BCMPKT_DEV_DRV_T_AUTO;
    bcma_cli_parse_table_t pt;
    bcmpkt_dev_drv_types_t drv_type;

    rv = bcmpkt_dev_drv_type_get(unit, &drv_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (drv_type != BCMPKT_DEV_DRV_T_NONE) {
        cli_out("%sDriver exists on unit: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, drv_type);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Type", "enum",
                             &type, &udev_types);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    LOG_VERBOSE(BSL_LS_APPL_PKTDEV,
                (BSL_META_U(unit, "Attach driver type %d\n"), type));

    rv = bcmpkt_dev_drv_attach(unit, type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Attach device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_detach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    int cleanup_dev = 1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "CleanUp", "bool", &cleanup_dev, NULL);
    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (cleanup_dev) {
        bool initialized = false;
        rv = bcmpkt_dev_initialized(unit, &initialized);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Get device initialization status failed (%d)\n"),
                       rv));
        }
        if (initialized) {
            rv = bcmpkt_dev_cleanup(unit);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit,
                                      "Packet device cleanup failed (%d)\n"),
                           rv));
            }
        }

    }
    rv = bcmpkt_dev_drv_detach(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Detach device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
dispatcher_delete(int unit, const bcma_bcmpkt_dispatch_info_t *dispatcher,
                  void *cb_data)
{
    if (dispatcher != NULL) {
        bcma_bcmpkt_rx_dispatcher_destroy(unit, dispatcher->netif_id);
    }

    return SHR_E_NONE;
}

static void
clean_all(int unit, void *cb_data)
{
    int rv;

    if (bcmdrd_dev_exists(unit)) {
        /* Destroy all packet watchers on the unit */
        bcma_bcmpktcmd_watcher_destroy(unit, -1);

        /*
         * Only execute the callback function
         * with the unit number of the detached device.
         */
        /* Destroy all Rx dispatchers. */
        rv = bcma_bcmpkt_rx_dispatcher_traverse(unit, dispatcher_delete, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Destroy all Rx dispatchers failed (%d)\n"),
                       rv));
        }

        netif_defid[unit] = 0;
    }
}

static int
cmd_dev_init(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bcma_cli_parse_table_t pt;
    shr_pb_t *pb;
    static int create_def_path = 1;
    static int max_frame_size = MAX_FRAME_SIZE_DEF;
    char *name = NULL;
    bcmpkt_dev_drv_types_t drv_type;
    bool initialized;
    
    static bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x1,
        .cgrp_size = 4,
        .mac_addr = BCMA_BCMPKT_DEF_NETIF_MAC,
    };

    rv = bcmpkt_dev_drv_type_get(unit, &drv_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (drv_type == BCMPKT_DEV_DRV_T_NONE) {
        cli_out("%sDriver not attached: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }
    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (initialized) {
        cli_out("Already initialized\n");
        return BCMA_CLI_CMD_OK;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_data_add_net();
    sal_snprintf(cfg.name, BCMPKT_DEV_NAME_MAX, "bcm%d", unit);
    bcma_cli_parse_table_add(&pt, "Name", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "DefPath", "bool", &create_def_path, NULL);
    bcma_cli_parse_table_add(&pt, "MACaddr", "mac", cfg.mac_addr, NULL);
    bcma_cli_parse_table_add(&pt, "CHGrp", "int", &cfg.cgrp_bmp, NULL);
    bcma_cli_parse_table_add(&pt, "FrameSize", "int", &max_frame_size, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (name != NULL){
        int last = sizeof(cfg.name) - 1;
        sal_strncpy(cfg.name, name, last);
        cfg.name[last] = '\0';
    }
    bcma_cli_parse_table_done(&pt);

    pb = shr_pb_create();
    dev_cfg_dump(pb, &cfg);
    shr_pb_printf(pb, "\tDefPath: %s\n", create_def_path ? "Yes" : "No");
    LOG_VERBOSE(BSL_LS_APPL_PKTDEV,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    rv = bcmpkt_dev_init(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Network device init failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }

    /*
     * For other module's convenience, create default path:
     *   - Setup two DMA channel, one for TX and one for RX;
     *   - Bringup network device;
     *   - Create a netif for TX/RX;
     *   - Create SOCKET on the netif;
     *   - filter all packets to the netif with priority 255;
     */
    if (create_def_path) {
        bcmpkt_dev_drv_types_t dev_drv_type;
        bcmpkt_netif_t netif = {
            .mac_addr = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
            .max_frame_size = max_frame_size,
            .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
        };
        bcmpkt_filter_t filter = {
            .type = BCMPKT_FILTER_T_RX_PKT,
            .priority = 255,
            .dma_chan = 1,
            .dest_type = BCMPKT_DEST_T_NETIF,
            .dest_id = 1,
            .match_flags = 0,
        };
        bcmpkt_dma_chan_t chan = {
            .dir = BCMPKT_DMA_CH_DIR_TX,
            .ring_size = 64,
            .max_frame_size = max_frame_size,
        };
        rv = bcmpkt_dev_drv_type_get(unit, &dev_drv_type);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get device driver type failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        /* Create per device buffer pool for UNET only. */
        if (dev_drv_type == BCMPKT_DEV_DRV_T_UNET) {
            bcmpkt_bpool_create(unit, max_frame_size, BCMPKT_BPOOL_BCOUNT_DEF);
        }

        /* Configure TX channel*/
        chan.id = 0;
        chan.dir = BCMPKT_DMA_CH_DIR_TX;
        rv = bcmpkt_dma_chan_set(unit, &chan);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Configure TX channel failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        /* Configure RX channel*/
        chan.id = 1;
        chan.dir = BCMPKT_DMA_CH_DIR_RX;
        rv = bcmpkt_dma_chan_set(unit, &chan);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Configure RX channel failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        /* Bringup network device. */
        rv = bcmpkt_dev_enable(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Pull up network device failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
            /* Create netif. */
            rv = bcmpkt_netif_create(unit, &netif);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit,
                                      "Create default network interface failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }
            /* Setup SOCKET. */
            rv = bcmpkt_socket_create(unit, netif.id, NULL);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit, "Create default SOCKET failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }
            /* Create filter to forward all packet to the netif. */
            filter.dest_id = netif.id;
            rv = bcmpkt_filter_create(unit, &filter);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PKTDEV,
                          (BSL_META_U(unit, "Create KNET filter failed (%d)\n"),
                           rv));
                return BCMA_CLI_CMD_FAIL;
            }

            netif_defid[unit] = netif.id;
            cli_out("Create SOCKET on network interface %d successfully\n", netif.id);
        } else {
            netif_defid[unit] = 1;
            cli_out("Create UNET successfully\n");
        }
        /* Create Rx dispatcher. */
        rv = bcma_bcmpkt_rx_dispatcher_create(unit, netif_defid[unit], 0,
                                              bcma_bcmpkt_packet_process);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Create Rx dispatcher failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        /* Register a shutdown callback function for required cleanup operations. */
        rv = bcmmgmt_shutdown_cb_register(clean_all, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Failed to create shutdown callback (%s)\n"),
                       shr_errmsg(rv)));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    cfg.mac_addr[4]++;

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_cleanup(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    int rv;
    bool initialized;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (initialized) {
        /* Destroy all packet watchers on the unit */
        bcma_bcmpktcmd_watcher_destroy(unit, -1);

        /* Destroy all Rx dispatchers. */
        rv = bcma_bcmpkt_rx_dispatcher_traverse(unit, dispatcher_delete, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Rx dispatcher cleanup failed (%d)\n"),
                       rv));
        }

        rv = bcmpkt_dev_cleanup(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Packet device cleanup failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        netif_defid[unit] = 0;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_up(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bool initialized;
    bool enabled;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("%sPacket device not initialized: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmpkt_dev_enabled(unit, &enabled);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device running status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!enabled) {
        rv = bcmpkt_dev_enable(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Packet device pull up failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_down(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bool initialized;
    bool enabled;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("%sPacket device not initialized: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmpkt_dev_enabled(unit, &enabled);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device running status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (enabled) {
        rv = bcmpkt_dev_disable(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Packet device pull down failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_dev_info_dump(int unit)
{
    int rv;
    shr_pb_t *pb;
    bcmpkt_dev_drv_types_t drv_type;
    bool initialized;
    bool enabled;

    cli_out("Packet device information:\n");
    rv = bcmpkt_dev_drv_type_get(unit, &drv_type);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device driver failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("  Driver: ");
    if (drv_type == BCMPKT_DEV_DRV_T_NONE) {
        cli_out("not attached\n");
        return BCMA_CLI_CMD_OK;
    } else if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        cli_out("KNET\n");
    } else if (drv_type == BCMPKT_DEV_DRV_T_UNET) {
        cli_out("UNET\n");
    } else {
        cli_out("Unknown - %d\n", drv_type);
        return BCMA_CLI_CMD_FAIL;
    }
    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("  Not initialized\n");
        return BCMA_CLI_CMD_OK;
    }
    rv = bcmpkt_dev_enabled(unit, &enabled);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device running status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!enabled) {
        cli_out("  Not running\n");
    }

    pb = shr_pb_create();
    rv = bcmpkt_dev_info_dump(unit, pb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Packet device info dump failed (%d)\n"),
                   rv));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("%s\n", shr_pb_str(pb));

    shr_pb_destroy(pb);
    return BCMA_CLI_CMD_OK;
}

static void
chan_cfg_dump(shr_pb_t *pb, bcmpkt_dma_chan_t *cfg)
{
    shr_pb_printf(pb, "\nDMA channel info:\n");
    shr_pb_printf(pb, "\tID: %d\n", cfg->id);
    shr_pb_printf(pb, "\tDirection: %d(%s)\n", cfg->dir, cfg->dir ? "TX" : "RX");
    shr_pb_printf(pb, "\tMax Frame Size: %d\n", cfg->max_frame_size);
    shr_pb_printf(pb, "\tRingsize: %d\n", cfg->ring_size);
}

static int
cmd_chan_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    char *cmd;
    shr_pb_t *pb;
    bcma_cli_parse_table_t pt;
    static bcmpkt_dma_chan_t cfg = {
        .dir = BCMPKT_DMA_CH_DIR_RX,
        .ring_size = 64,
        .max_frame_size = MAX_FRAME_SIZE_DEF,
    };
    bool initialized;
    bool enabled;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("%sPacket device not initialized: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }
    rv = bcmpkt_dev_enabled(unit, &enabled);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get device running status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (enabled) {
        cli_out("%sDMA is running, prohibit change DMA channel setting on the fly",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    cfg.id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || cfg.id < 0) {
        cli_out("Error: Invalid option: %s\n",  cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Dir", "enum",
                             &cfg.dir, dma_chan_dir);
    bcma_cli_parse_table_add(&pt, "RingSize", "int",
                             &cfg.ring_size, NULL);
    bcma_cli_parse_table_add(&pt, "FrameSize", "int",
                             &cfg.max_frame_size, NULL);


    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return BCMA_CLI_CMD_USAGE;
    }

    pb = shr_pb_create();
    chan_cfg_dump(pb, &cfg);
    LOG_VERBOSE(BSL_LS_APPL_PKTDEV,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    rv = bcmpkt_dma_chan_set(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Configure DMA channel %d failed (%d)\n"),
                   cfg.id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_chan_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    char *cmd;
    shr_pb_t *pb;
    bcmpkt_dma_chan_t cfg;
    bool initialized;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("%sPacket device not initialized: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }

    /* If no channel ID input, dump all channels */
    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        uint32_t i, count;
        bcmpkt_dma_chan_t chans[12];

        /* Get channel numbers */
        rv = bcmpkt_dma_chan_get_list(unit, 0, chans, &count);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get DMA channel list failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (count > 12) {
            cli_out("Count %d is bigger than 12\n", count);
            count = 12;
        }
        /* Get all channels' configuration */
        rv = bcmpkt_dma_chan_get_list(unit, count, chans, &count);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Get DMA channel list failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }

        pb = shr_pb_create();
        for (i = 0; i < count; i++) {
            chan_cfg_dump(pb, &chans[i]);
        }
        cli_out("%s", shr_pb_str(pb));
        shr_pb_destroy(pb);

        return BCMA_CLI_CMD_OK;
    }
    sal_memset(&cfg, 0, sizeof(cfg));
    cfg.id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || cfg.id < 0) {
        cli_out("Error: Invalid option: %s\n",  cmd);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = bcmpkt_dma_chan_get(unit, cfg.id, &cfg);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get DMA channel %d failed (%d)\n"),
                   cfg.id, rv));
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();
    chan_cfg_dump(pb, &cfg);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_chan_qmap_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int chan_id;
    int unit = cli->cmd_unit;
    char* cmd;
    bcma_cli_parse_table_t pt;
    uint32_t bmp_high[1] = { 0xffffffff };
    uint32_t bmp_low[1] = { 0xffffffff };
    SHR_BITDCLNAME(queue_bmp, MAX_CPU_COS) = { 0 };

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    chan_id = sal_strtol(cmd, &cmd, 0);
    if (*cmd != 0 || chan_id <= 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "HighWord", "hex", bmp_high, NULL);
    bcma_cli_parse_table_add(&pt, "LowWord", "hex", bmp_low, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    SHR_BITCOPY_RANGE(queue_bmp, 0, bmp_low, 0, 32);
    if (MAX_CPU_COS > 32) {
        SHR_BITCOPY_RANGE(queue_bmp, 32, bmp_high, 0, 32);
    }

    if (bcma_bcmpkt_chan_qmap_set(unit, chan_id, queue_bmp, MAX_CPU_COS)) {
       return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_bpool_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    static int shared = 0, size = -1, count = -1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Shared", "bool", &shared, NULL);
    bcma_cli_parse_table_add(&pt, "BufSize", "int", &size, NULL);
    bcma_cli_parse_table_add(&pt, "BufCount", "int", &count, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (shared) {
        unit = -1;
    }

    if ((rv = bcmpkt_bpool_create(unit, size, count))) {
        cli_out("Create buffer pool failed (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Create buffer pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_bpool_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    static int shared = 0;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Shared", "bool", &shared, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (shared) {
        unit = -1;
    }

    if ((rv = bcmpkt_bpool_destroy(unit))) {
        cli_out("Destroy buffer pool failed %d\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Destroy buffer pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_bpool_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;
    int unit = cli->cmd_unit;
    shr_pb_t *pb = NULL;

    cmd = BCMA_CLI_ARG_GET(args);
    if (!cmd) {
        pb = shr_pb_create();
        bcmpkt_bpool_info_dump(unit, pb);
        cli_out("%s", shr_pb_str(pb));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("All", cmd, ' ')) {
        pb = shr_pb_create();
        for (unit = -1; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            shr_pb_reset(pb);
            bcmpkt_bpool_info_dump(unit, pb);
            cli_out("%s", shr_pb_str(pb));
        }
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    return BCMA_CLI_CMD_USAGE;
}

static int
cmd_ppool_create(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int count = -1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "PacketCount", "int", &count, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if ((rv = bcmpkt_ppool_create(count))) {
        cli_out("Create packet pool failed (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Create packet pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_ppool_destroy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;

    if ((rv = bcmpkt_ppool_destroy())) {
        cli_out("Destroy packet pool failed %d\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    cli_out("Destroy packet pool succeed\n");

    return BCMA_CLI_CMD_OK;
}

static int
cmd_ppool_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    shr_pb_t *pb = NULL;

    pb = shr_pb_create();
    bcmpkt_ppool_info_dump(pb);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_rx_rate_limit_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bcma_cli_parse_table_t pt;
    int rate_limit = -1;
    bool initialized;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("%sPacket device is not initialized: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "RxRate", "int", &rate_limit, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    rate_limit = (rate_limit < 0) ? -1 : (rate_limit * 1000);
    rv = bcmpkt_rx_rate_limit_set(unit, rate_limit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_KNET,
                  (BSL_META_U(unit,
                              "Set RX rate limit failed (%d)\n"),
                   rv));

        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_rx_rate_limit_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    int rate_limit;
    bool initialized;

    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit,
                              "Get device initialization status failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    if (!initialized) {
        cli_out("%sPacket device is not initialized: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmpkt_rx_rate_limit_get(unit, &rate_limit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PKTDEV,
                  (BSL_META_U(unit, "Get RX rate limit failed (%d)\n"),
                   rv));
        return BCMA_CLI_CMD_FAIL;
    }
    rate_limit = (rate_limit < 0) ? -1 : (rate_limit / 1000);
    cli_out("Rx rate limit: %d Kpps\n", rate_limit);

    return BCMA_CLI_CMD_OK;
}

static int
chan_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Set", cmd, ' ')) {
        return cmd_chan_set(cli, args);
    }

    if (bcma_cli_parse_cmp("QueueMap", cmd, ' ')) {
        return cmd_chan_qmap_set(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_chan_dump(cli, args);
    }

    cli_out("%sUnknown device channel command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
stats_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char* cmd;
    int unit = cli->cmd_unit;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Show", cmd, ' ')) {
        shr_pb_t *pb;
        bool initialized;

        rv = bcmpkt_dev_initialized(unit, &initialized);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Get device initialization status failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        if (!initialized) {
            cli_out("%sPacket device is not initialized: %d\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit);
            return BCMA_CLI_CMD_FAIL;
        }
        pb = shr_pb_create();
        rv = bcmpkt_dev_stats_dump(unit, pb);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Dump packet device stats failed (%d)\n"),
                       rv));
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        cli_out("Packet device statistics:\n");
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("Clear", cmd, ' ')) {
        bool initialized;
        rv = bcmpkt_dev_initialized(unit, &initialized);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit,
                                  "Get device initialization status failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        if (!initialized) {
            cli_out("%sPacket device is not initialized: %d\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit);
            return BCMA_CLI_CMD_FAIL;
        }
        rv = bcmpkt_dev_stats_clear(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PKTDEV,
                      (BSL_META_U(unit, "Clear pktdev stats failed (%d)\n"),
                       rv));
            return BCMA_CLI_CMD_FAIL;
        }
        return BCMA_CLI_CMD_OK;
    }

    cli_out("%sUnknown device stats command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
bpool_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return cmd_bpool_create(cli, args);
    }

    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return cmd_bpool_destroy(cli, args);
    }

    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_bpool_info(cli, args);
    }

    cli_out("%sUnknown buffer pool command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
ppool_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Create", cmd, ' ')) {
        return cmd_ppool_create(cli, args);
    }

    if (bcma_cli_parse_cmp("Destroy", cmd, ' ')) {
        return cmd_ppool_destroy(cli, args);
    }

    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_ppool_info(cli, args);
    }

    cli_out("%sUnknown packet pool command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
rate_limit_cmds(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("Set", cmd, ' ')) {
        return cmd_rx_rate_limit_set(cli, args);
    }
    if (bcma_cli_parse_cmp("Info", cmd, ' ')) {
        return cmd_rx_rate_limit_info(cli, args);
    }

    cli_out("%sUnknown RateLimit command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpkt_netif_defid_get(int unit)
{
    if ((uint32_t)unit < BCMDRD_CONFIG_MAX_UNITS) {
        return netif_defid[unit];
    }

    return 0;
}

int
bcma_bcmpkt_netif_defid_set(int unit, int netif_id)
{
    if ((uint32_t)unit < BCMDRD_CONFIG_MAX_UNITS) {
        return netif_defid[unit] = netif_id;
    }

    return 0;
}

int
bcma_bcmpktcmd_pktdev(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char* cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("ATtach", cmd, ' ')) {
        return cmd_dev_attach(cli, args);
    }
    if (bcma_cli_parse_cmp("DeTach", cmd, ' ')) {
        return cmd_dev_detach(cli, args);
    }
    if (bcma_cli_parse_cmp("Init", cmd, ' ')) {
        return cmd_dev_init(cli, args);
    }
    if (bcma_cli_parse_cmp("Up", cmd, ' ')) {
        return cmd_dev_up(cli, args);
    }
    if (bcma_cli_parse_cmp("Down", cmd, ' ')) {
        return cmd_dev_down(cli, args);
    }
    if (bcma_cli_parse_cmp("CleanUp", cmd, ' ')) {
        return cmd_dev_cleanup(cli, args);
    }
    if (bcma_cli_parse_cmp("InFo", cmd, ' ')) {
        return cmd_dev_info_dump(cli->cmd_unit);
    }
    if (bcma_cli_parse_cmp("Stats", cmd, ' ')) {
        return stats_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("Chan", cmd,  ' ')) {
        return chan_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("BufPool", cmd,  ' ')) {
        return bpool_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("PacketPool", cmd,  ' ')) {
        return ppool_cmds(cli, args);
    }
    if (bcma_cli_parse_cmp("RateLimit", cmd,  ' ')) {
        return rate_limit_cmds(cli, args);
    }

    cli_out("%sUnknown device command: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}
