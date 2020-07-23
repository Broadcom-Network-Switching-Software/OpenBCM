/*! \file bcma_bcmpktcmd_pkttest.c
 *
 * Packet I/O  performance test commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pkttest.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include <bcmltd/chip/bcmltd_device_constants.h>

static int
pkttest_rx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int unit = cli->cmd_unit;
    int port;
    int chan = 1;
    SHR_BITDCLNAME(queue_bmp, MAX_CPU_COS) = {0};
    bcma_bcmpkt_test_rx_param_t *rx_param = NULL;
    int fail_cnt = 0;

    rx_param = sal_alloc(sizeof(bcma_bcmpkt_test_rx_param_t),
                         "bcmaBcmpktTestRxParam");
    if (rx_param == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(rx_param, 0, sizeof(bcma_bcmpkt_test_rx_param_t));

    rv = bcma_bcmpkt_test_rx_parse_args(unit, cli, args, rx_param, false);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }
    port = rx_param->port;

    rv = bcma_bcmpkt_test_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dev_init(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    SHR_BITSET_RANGE(queue_bmp, 0, MAX_CPU_COS);
    rv = bcma_bcmpkt_test_chan_qmap_set(unit, chan, queue_bmp, MAX_CPU_COS);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_MAC);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_packet_generate(unit, &rx_param->packet,
                                          rx_param->len_start,
                                          rx_param->len_end, port);
    if (SHR_FAILURE(rv)) {
         bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_NONE);
         bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dpath_create(unit, port, port, rx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_dpath_destroy(unit, port, rx_param->ifp_cfg);
        bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_NONE);
        bcma_bcmpkt_test_rx_recycle(unit, rx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_rx_run(unit, rx_param);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_dpath_destroy(unit, port, rx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_NONE);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    bcma_bcmpkt_test_rx_recycle(unit, rx_param);

    if (fail_cnt > 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
pkttest_tx(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int unit = cli->cmd_unit;
    bcma_bcmpkt_test_tx_param_t *tx_param = NULL;
    int port;
    int fail_cnt = 0;

    tx_param = sal_alloc(sizeof(bcma_bcmpkt_test_rx_param_t),
                         "bcmaBcmpktTestTxParam");
    if (tx_param == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(tx_param, 0, sizeof(bcma_bcmpkt_test_tx_param_t));

    rv = bcma_bcmpkt_test_tx_parse_args(unit, cli, args, tx_param, false);
    if (SHR_FAILURE(rv)) {
         bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }
    port = tx_param->port;

    rv = bcma_bcmpkt_test_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dev_init(unit);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_MAC);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_packet_generate(unit, &tx_param->packet,
                                          tx_param->len_start,
                                          tx_param->len_end, port);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_NONE);
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_dpath_create(unit, port, -1, tx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_NONE);
        bcma_bcmpkt_test_dpath_destroy(unit, port, tx_param->ifp_cfg);
        bcma_bcmpkt_test_tx_recycle(unit, tx_param);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_bcmpkt_test_tx_run(unit, tx_param);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_port_lb_set(unit, port, BCMA_TEST_LPBK_NONE);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_dpath_destroy(unit, port, tx_param->ifp_cfg);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    rv = bcma_bcmpkt_test_dev_cleanup(unit);
    if (SHR_FAILURE(rv)) {
        fail_cnt++;
    }

    bcma_bcmpkt_test_tx_recycle(unit, tx_param);

    if (fail_cnt > 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmpktcmd_pkttest(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("rx", cmd,  ' ')) {
        return pkttest_rx(cli, args);
    }

    if (bcma_cli_parse_cmp("tx", cmd,  ' ')) {
        return pkttest_tx(cli, args);
    }

    cli_out("Error: Unknown PKTTEST command: %s.\n", cmd);

    return BCMA_CLI_CMD_FAIL;
}

