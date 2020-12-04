/*! \file bcma_testcase_pkt_multi.c
 *
 *  Test case for packet I/O with multiple channels.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_atomic.h>
#include <sal/sal_sleep.h>
#include <bcmlt/bcmlt.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_pkt_multi.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include "bcma_testcase_pkt_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define PKT_MULTI_THREAD_STOP_WAITTIME              (2) /* secs */
#define PKT_MULTI_DEFAULT_MULTI_CMC                 (FALSE)
#define PKT_MULTI_DEFAULT_TX_CHAN_NUM               (2)
#define PKT_MULTI_DEFAULT_RX_CHAN_NUM               (2)
#define PKT_MULTI_DEFAULT_TESTING_TIME              (2) /* secs */
#define PKT_MULTI_DEFAULT_TESTING_LENSTART          (64)
#define PKT_MULTI_DEFAULT_TESTING_LENEND            (1536)
#define PKT_MULTI_DEFAULT_TESTING_LENINC            (64)

#define PKT_MULTI_PARAM_HELP \
    "  TxNum           - The number of TX channels\n"\
    "  RxNum           - The number of RX channels\n"\
    "  MultiCMC        - Whether to use multiple CMC during testing\n"\
    "  Time            - Testing duration for each packet length\n"\
    "  LengthStart     - Starting packet length\n"\
    "  LengthEnd       - Ending packet length\n"\
    "  LengthInc       - Increasing step of packet length\n"\
    "\nExample:\n"\
    "    Run packet I/O performance testing with two TX channels\n"\
    "      tr 42 tn=2 rn=0\n"\
    "    Run packet I/O performance testing with two TX/RX channels across different CMCs\n"\
    "      tr 42 tn=2 rn=2 mcmc=true\n"\
    "    Run packet I/O performance testing with packet length 128, 256, 384, 512\n"\
    "      tr 42 ls=128 le=512 li=128\n"\
    "    Run packet I/O performance testing with 5 secs for each packet length\n"\
    "      tr 42 ls=128 le=512 li=128 t=5\n"

static void
pkt_multi_print_help(int unit, void *bp)
{
    cli_out("%s", PKT_MULTI_PARAM_HELP);
    return;
}

typedef struct pkt_multi_param_s {
    /**@{*/
    /*! The parameters for tx channels */
    int tx_num;
    int tx_dst_port;
    int tx_netif[NUM_QUE_MAX];
    int tx_chan[NUM_QUE_MAX];
    shr_thread_t *tx_thread_pid;
    bcmpkt_packet_t *tx_packet;
    /**@}*/

    /**@{*/
    /*! The parameters for rx channels */
    int rx_num;
    int rx_chan[NUM_QUE_MAX];
    int rx_netif[NUM_QUE_MAX];
    int rx_lb_port[NUM_QUE_MAX];
    int rx_lb_new_pri[NUM_QUE_MAX];
    bcmpkt_packet_t *rx_packet[NUM_QUE_MAX];
    int rx_pktcnt[NUM_QUE_MAX];
    bcma_bcmpkt_test_fp_cfg_t rx_ifp_cfg[NUM_QUE_MAX];
    /**@}*/

    /**@{*/
    /*! reserved default tx/rx channel */
    int tx_netif_default;
    int tx_chan_default;
    int rx_netif_default;
    int rx_chan_default;
    /**@}*/

    /*! Testing with multiple CMC */
    int multi_cmc;

    /*! Unit number */
    int unit;

    /*! The flag to indicate whether we are under testing or not */
    shr_atomic_int_t in_testing;

    /*! Starting packet length. */
    uint32_t len_start;

    /*! Ending packet length. */
    uint32_t len_end;

    /*! Increasing step of packet length. */
    uint32_t len_inc;

    /*! Testing duration. */
    uint32_t time;

    /*! Packet device id */
    int pktdev_id;
}pkt_multi_param_t;

static int
pkt_multi_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      pkt_multi_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "TxNum", "int",
                                  &param->tx_num, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "RxNum", "int",
                                  &param->rx_num, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "MultiCMC", "bool",
                                  &param->multi_cmc, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Time", "int",
                                  &param->time, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthStart", "int",
                                  &param->len_start, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthEnd", "int",
                                  &param->len_end, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "LengthInc", "int",
                                  &param->len_inc, NULL));

    /* parse start */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (show) {
        /* just show parsing result and exit function */
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
pkt_multi_next_chanid(int cur_chan, int cmc_num, int chan_num, int multi_cmc)
{
    int next_chan = cur_chan;

    /*
     * ex. cmc_num = 2, chan_num = 8
     *      ==> chan_id
     *           cmc 0 : 0  1  2  3  4  5  6  7
     *           cmc 1 : 8  9 10 11 12 13 14 15
     *     if multi_cmc = false: returns 0, 1, 2, 3, 4, 5, 6 ...
     *                    true : returns 0, 8, 1, 9, 2, 10, 3 ...
     */
    if (cur_chan == -1) {
        /* start the traverse from chan_id 0 */
        return 0;
    } else {
        while (1) {
            if (multi_cmc == FALSE) {
                next_chan = (next_chan + 1) % (cmc_num * chan_num);
            } else {
                next_chan += chan_num;
                if (next_chan >= cmc_num * chan_num) {
                    next_chan = (next_chan + 1) % chan_num;
                }
            }
            if (next_chan == 0) {
                /* no more any available channels */
                return -1;
            }
            if (next_chan != (cmc_num * chan_num - 2) &&
                next_chan != (cmc_num * chan_num - 1)) {
                /* this chan id is not reserved, okay to return */
                return next_chan;
            }
        }
    }
}

static int
pkt_multi_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags,
                 void **bp)
{
    pkt_multi_param_t *param = NULL;
    bool show = false;
    int cmc_num, chan_num, i, cur_chan_ptr, port;
    bcmdrd_pbmp_t pbmp_default;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(param, sizeof(pkt_multi_param_t),
              "bcmaTestCasePktMultiPrm");
    SHR_NULL_CHECK(param, SHR_E_MEMORY);

    /* init */
    sal_memset(param, 0, sizeof(pkt_multi_param_t));
    param->unit = unit;
    param->multi_cmc = PKT_MULTI_DEFAULT_MULTI_CMC;
    param->tx_num = PKT_MULTI_DEFAULT_TX_CHAN_NUM;
    param->rx_num = PKT_MULTI_DEFAULT_RX_CHAN_NUM;
    param->len_start = PKT_MULTI_DEFAULT_TESTING_LENSTART;
    param->len_end = PKT_MULTI_DEFAULT_TESTING_LENEND;
    param->len_inc = PKT_MULTI_DEFAULT_TESTING_LENINC;
    param->time = PKT_MULTI_DEFAULT_TESTING_TIME;

    /* parse */
    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }
    SHR_IF_ERR_EXIT
        (pkt_multi_parse_param(unit, cli, a, param, show));
    if (show == true) {
        SHR_FREE(param);
        SHR_EXIT();
    }

    /* param sanity check */
    if (param->tx_num < 0 || param->tx_num > NUM_QUE_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (param->rx_num < 0 || param->rx_num > NUM_QUE_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (param->tx_num == 0 && param->rx_num == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * reserve the last two channels for default tx/rx
     *   ex. available chan id = 0 ~ 15
     *     ==> tx_chan_default = 15
     *         rx_chan_default = 14
     */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_cmc_chan_get(unit, &cmc_num, &chan_num));

    param->tx_chan_default = cmc_num * chan_num - 1;
    param->rx_chan_default = cmc_num * chan_num - 2;
    if (param->tx_chan_default < 0 || param->rx_chan_default < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* assign channels for the others */
    cur_chan_ptr = -1;
    cli_out("tx channels : ");
    for (i = 0; i < param->tx_num; i++) {
        param->tx_chan[i] = pkt_multi_next_chanid(cur_chan_ptr, cmc_num,
                                                  chan_num, param->multi_cmc);
        if (param->tx_chan[i] == -1) {
            cli_out("\n the number of channels cannot exceed %"PRIu32"\n",
                    (cmc_num * chan_num) - 2);
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        cur_chan_ptr = param->tx_chan[i];
        cli_out("%2"PRId32" (cmc%"PRId32", chan%"PRId32") ",
            param->tx_chan[i], param->tx_chan[i]/chan_num,
                               param->tx_chan[i]%chan_num);
    }
    cli_out("%s", param->tx_num == 0? "(none)\n": "\n");
    cli_out("rx channels : ");
    for (i = 0; i < param->rx_num; i++) {
        param->rx_chan[i] = pkt_multi_next_chanid(cur_chan_ptr, cmc_num,
                                                  chan_num, param->multi_cmc);
        if (param->rx_chan[i] == -1) {
            cli_out("\n the number of channels cannot exceed %"PRIu32"\n",
                    (cmc_num * chan_num) - 2);
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        cur_chan_ptr = param->rx_chan[i];
        cli_out("%2"PRId32" (cmc%"PRId32", chan%"PRId32") ",
            param->rx_chan[i], param->rx_chan[i]/chan_num,
                               param->rx_chan[i]%chan_num);
    }
    cli_out("%s", param->rx_num == 0? "(none)\n": "\n");

    /* assign single one tx_dst_port first, then each rx_lb_ports */
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get(unit, BCMDRD_PORT_TYPE_FPAN,
                                            &pbmp_default));
    i = 0;
    BCMDRD_PBMP_ITER(pbmp_default, port) {
        if (param->tx_dst_port == 0) {
            param->tx_dst_port = port;
        } else {
            if (i < param->rx_num) {
                param->rx_lb_port[i] = port;
                i++;
            }
        }
    }
    if (param->tx_dst_port == 0 || i < param->rx_num) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    cli_out("tx_dst_port : %2"PRId32"\n", param->tx_dst_port);
    cli_out("rx_dst_port : ");
    for (i = 0; i < param->rx_num; i++) {
        cli_out("%2"PRId32", ", param->rx_lb_port[i]);
    }
    cli_out("%s", param->rx_num == 0? "(none)\n": "\n");

    /* assign new internal priority for each rx, starting from 1 */
    for (i = 0; i < param->rx_num; i++) {
        param->rx_lb_new_pri[i] = (i + 1);
    }

    *bp = (void *)param;
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(param);
    }
    SHR_FUNC_EXIT();
}

static int
pkt_multi_pkt_dev_setup_cb(int unit, void *bp, uint32_t option)
{
    pkt_multi_param_t *param = (pkt_multi_param_t *)bp;
    bcma_testutil_packet_dev_init_param_t pktdev_param;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* assign chan id */
    pktdev_param.tx_chan_num = param->tx_num + 1;
    pktdev_param.rx_chan_num = param->rx_num + 1;
    for (i = 0; i < param->tx_num; i++) {
        pktdev_param.tx_chan[i] = param->tx_chan[i];
    }
    pktdev_param.tx_chan[param->tx_num] = param->tx_chan_default;
    for (i = 0; i < param->rx_num; i++) {
        pktdev_param.rx_chan[i] = param->rx_chan[i];
    }
    pktdev_param.rx_chan[param->rx_num] = param->rx_chan_default;

    /* assign queue mapping for rx */
    for (i = 0; i < param->rx_num; i++) {
        SHR_BITCLR_RANGE(pktdev_param.rx_queue_bmp[i], 0, MAX_CPU_COS);
    }
    SHR_BITSET_RANGE(pktdev_param.rx_queue_bmp[param->rx_num], 0, MAX_CPU_COS);

    for (i = 0; i < param->rx_num; i++) {
        SHR_BITSET(pktdev_param.rx_queue_bmp[i], param->rx_lb_new_pri[i]);
        SHR_BITCLR(pktdev_param.rx_queue_bmp[param->rx_num], param->rx_lb_new_pri[i]);
    }

    /* init packet device */
    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init(unit, &pktdev_param,
                                       param->len_end + 4, &param->pktdev_id));

    /* assign returned netif id */
    for (i = 0; i < param->tx_num; i++) {
        param->tx_netif[i] = pktdev_param.tx_netif[i];
    }
    param->tx_netif_default = pktdev_param.tx_netif[param->tx_num];
    for (i = 0; i < param->rx_num; i++) {
        param->rx_netif[i] = pktdev_param.rx_netif[i];
    }
    param->rx_netif_default = pktdev_param.rx_netif[param->rx_num];

exit:
    SHR_FUNC_EXIT();
}

static int
pkt_multi_port_lb_set_cb(int unit, void *bp, uint32_t option)
{
    pkt_multi_param_t *param = (pkt_multi_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    for (i = 0; i < param->rx_num; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, param->rx_lb_port[i]);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, LB_TYPE_MAC ,pbmp,
                                  BCMA_TEST_DEFAULT_CHECK_LINK_TIME));

exit:
    SHR_FUNC_EXIT();
}

static int
pkt_multi_port_lb_cleanup_cb(int unit, void *bp, uint32_t option)
{
    pkt_multi_param_t *param = (pkt_multi_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    BCMDRD_PBMP_CLEAR(pbmp);
    for (i = 0; i < param->rx_num; i++) {
        BCMDRD_PBMP_PORT_ADD(pbmp, param->rx_lb_port[i]);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, LB_TYPE_MAC, pbmp, TRUE));

exit:
    SHR_FUNC_EXIT();
}

static void
pkt_multi_run_tx_thread(shr_thread_t tc, void *arg)
{
    pkt_multi_param_t *param = (pkt_multi_param_t *)arg;
    bcmpkt_packet_t *packet_clone = NULL;
    int rv, _func_unit, _func_rv;
    int tx_ptr = 0;

    if (param == NULL) {
        return;
    }
    _func_unit = param->unit;

    if (param->tx_packet == NULL) {
        SHR_ERR_MSG_EXIT(SHR_E_PARAM, (BSL_META("null packet")));
    }

    rv = bcmpkt_packet_clone(param->pktdev_id, param->tx_packet, &packet_clone);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_MSG_EXIT(rv, (BSL_META("packet_clone failed")));
    }

    tx_ptr = 0;
    while (1) {
        if (shr_thread_stopping(tc)) {
           break;
        }

        /* free the previous and allocate the new databuf to packet_clone */
        if (packet_clone->data_buf) {
            rv = bcmpkt_data_buf_free(param->pktdev_id, packet_clone->data_buf);
            if (SHR_FAILURE(rv)) {
                SHR_ERR_MSG_EXIT(rv, (BSL_META("buf_free failed")));
            }
            packet_clone->data_buf = NULL;
        }
        rv = bcmpkt_data_buf_copy(param->pktdev_id, param->tx_packet->data_buf,
                                  &packet_clone->data_buf);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_MSG_EXIT(rv, (BSL_META("buf_copy failed")));
        }

        /* tx */
        rv = bcmpkt_tx(param->unit, param->tx_netif[tx_ptr] , packet_clone);
        if (SHR_FAILURE(rv) && rv != SHR_E_RESOURCE) {
            /*
             * Underlying packet DMA driver reports SHR_E_RESOURCE
             * when TX HW temporary unavaialble. Skip the error code
             * and do the next loop.
             */
            SHR_ERR_MSG_EXIT(rv, (BSL_META("tx failed")));
        }
        tx_ptr = (tx_ptr + 1) % param->tx_num;
    }

exit:
    if (packet_clone != NULL) {
        bcmpkt_free(param->pktdev_id, packet_clone);
    }
}

static int
pkt_multi_run_rx_cb(int unit, int netif_id, bcmpkt_packet_t *packet, void *cookie)
{
    pkt_multi_param_t *param = (pkt_multi_param_t *)cookie;
    int i, rx_idx = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    for (i = 0; i < param->rx_num; i++) {
        if (param->rx_netif[i] == netif_id) {
            rx_idx = i;
            break;
        }
    }
    if (rx_idx == -1) {
        cli_out("can not get rx neftif\n");
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (shr_atomic_int_get(param->in_testing)) {
        param->rx_pktcnt[rx_idx]++;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
pkt_multi_free_packet(int unit, pkt_multi_param_t *param)
{
    int i;

    if (param == NULL)
        return;

    if (param->tx_packet != NULL) {
        bcmpkt_free(param->pktdev_id, param->tx_packet);
        param->tx_packet = NULL;
    }

    for (i = 0; i < param->rx_num; i++) {
        if (param->rx_packet[i] != NULL) {
            bcmpkt_free(param->pktdev_id, param->rx_packet[i]);
            param->rx_packet[i] = NULL;
        }
    }
}

static int
pkt_multi_run_cb(int unit, void *bp, uint32_t option)
{
    pkt_multi_param_t *param = (pkt_multi_param_t *)bp;
    uint32_t pkt_len = 0;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    param->in_testing = shr_atomic_int_create(0);
    SHR_NULL_CHECK(param->in_testing, SHR_E_INTERNAL);

    if (param->tx_num != 0 && param->rx_num != 0) {
        cli_out("\n"
                " Packet |       Tx Rate       Total   |       Rx Rate       Total   |        |                     %%CPU                    \n"
                "  Len   |  KPkt/s |   MB/s  | packets |  KPkt/s |   MB/s  | packets |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
                " -------+---------+---------+---------+---------+---------+---------+--------+-----+-----+-----+-----+------+-----+--------+\n");
    } else if (param->tx_num != 0 && param->rx_num == 0) {
        cli_out("\n"
                " Packet |       Tx Rate       Total   |        |                     %%CPU                    \n"
                "  Len   |  KPkt/s |   MB/s  | packets |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
                " -------+---------+---------+---------+--------+-----+-----+-----+-----+------+-----+--------+\n");
    } else if (param->tx_num == 0 && param->rx_num != 0) {
        cli_out("\n"
                " Packet |       Rx Rate       Total   |        |                     %%CPU                    \n"
                "  Len   |  KPkt/s |   MB/s  | packets |  Intr  |Idle |User |Kern |Nice |IOwait|IRQ  |softIRQ  \n"
                " -------+---------+---------+---------+--------+-----+-----+-----+-----+------+-----+--------+\n");
    }

    /* setup rx watcher cb and ifp rules */
    if (param->rx_num != 0) {
        SHR_IF_ERR_EXIT
            (bcmpkt_rx_register(unit, 0, 0, pkt_multi_run_rx_cb, (void*)param));

        for (i = 0; i < param->rx_num; i++) {
            SHR_IF_ERR_EXIT
                (bcma_bcmpkt_test_dpath_create
                    (unit, param->rx_lb_port[i], param->rx_lb_port[i],
                     param->rx_lb_new_pri[i], &param->rx_ifp_cfg[i]));
        }
    }

    /* testing */
    for (pkt_len = param->len_start; pkt_len <= param->len_end;) {
        uint32_t tx_total_pktcnt = 0, rx_total_pktcnt = 0;
        cpu_stats_t cpu_start, cpu_end;
        uint32_t tx_pktcnt_start[NUM_QUE_MAX], tx_pktcnt_end[NUM_QUE_MAX];

        /*
         * start tx traffic
         *   ==> setup a tx thread and send the packets repeatedly
         *       through multiple tx_netifs (dst port = param->tx_dst_port)
         */
        if (param->tx_num != 0) {
            SHR_IF_ERR_EXIT
                (bcma_bcmpkt_test_packet_generate(unit, &param->tx_packet,
                                                  pkt_len, pkt_len,
                                                  param->tx_dst_port));
            SHR_NULL_CHECK(param->tx_packet, SHR_E_INTERNAL);
            param->tx_thread_pid = shr_thread_start("bcmatestcasetxthread",
                                                SAL_THREAD_PRIO_DEFAULT,
                                                pkt_multi_run_tx_thread,
                                                (void *)param);
            SHR_NULL_CHECK(param->tx_thread_pid, SHR_E_INTERNAL);
        }

        /*
         * start rx traffic
         *  ==> For each rx_lb_port
         *       1. create ifp rule:
         *           a) redirect the packet back to the lb_port
         *           b) and clone it to cpu and with a new COS value
         *              mapping to the corresponding cmic rx channel
         *       2. rx watcher will count the number of packets for each channel
         */
        for (i = 0; i < param->rx_num; i++) {
            SHR_IF_ERR_EXIT
                (bcma_bcmpkt_test_dpath_update_policy
                    (unit, param->rx_ifp_cfg[i].entry_id,
                    param->rx_ifp_cfg[i].copy_policy_id));

            param->rx_pktcnt[i] = 0;

            SHR_IF_ERR_EXIT
                (bcma_bcmpkt_test_packet_generate(unit, &param->rx_packet[i],
                                                  pkt_len, pkt_len,
                                                  param->rx_lb_port[i]));
            SHR_NULL_CHECK(param->rx_packet[i], SHR_E_INTERNAL);
            SHR_IF_ERR_EXIT
                (bcmpkt_tx(param->unit, param->tx_netif_default,
                           param->rx_packet[i]));
        }

        /* get statistic during 2 seconds */
        sal_usleep(100000);
        SHR_IF_ERR_EXIT
            (bcma_bcmpkt_test_cpu_stats_get(&cpu_start));
        for (i = 0; i < param->tx_num; i++) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_pktdma_counter_get
                    (unit, param->tx_chan[i], true, &tx_pktcnt_start[i]));
        }

        shr_atomic_int_set(param->in_testing, 1);
        sal_sleep(param->time);
        shr_atomic_int_set(param->in_testing, 0);

        SHR_IF_ERR_EXIT
            (bcma_bcmpkt_test_cpu_stats_get(&cpu_end));
        for (i = 0; i < param->tx_num; i++) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_pktdma_counter_get
                    (unit, param->tx_chan[i], true, &tx_pktcnt_end[i]));
        }

        /*
         * show statistic
         *   TX : from cmic tx counter
         *   RX : counting by rx callback watcher
         */
        for (i = 0; i < param->tx_num; i++) {
            uint32_t tx_pktcnt = tx_pktcnt_end[i] - tx_pktcnt_start[i];
            if (tx_pktcnt == 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Tx channel %"PRIu32" no traffic\n"),
                                        param->tx_chan[i]));
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            tx_total_pktcnt += tx_pktcnt;
        }
        for (i = 0; i < param->rx_num; i++) {
            if (param->rx_pktcnt[i] == 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Rx channel %"PRIu32" no traffic\n"),
                                        param->rx_chan[i]));
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            rx_total_pktcnt += param->rx_pktcnt[i];
        }

        if (param->tx_num != 0 && param->rx_num != 0) {
            bcma_bcmpkt_test_stats_dump
                (param->time * SECOND_USEC, tx_total_pktcnt, rx_total_pktcnt,
                 pkt_len, &cpu_end, &cpu_start, false, true);
        } else if (param->tx_num != 0 && param->rx_num == 0) {
            bcma_bcmpkt_test_stats_dump
                (param->time * SECOND_USEC, tx_total_pktcnt, 0,
                 pkt_len, &cpu_end, &cpu_start, false, false);
        } else if (param->tx_num == 0 && param->rx_num != 0) {
            bcma_bcmpkt_test_stats_dump
                (param->time * SECOND_USEC, rx_total_pktcnt, 0,
                 pkt_len, &cpu_end, &cpu_start, false, false);
        }

        /* stop tx traffic (by stopping thread) */
        if (param->tx_num != 0) {
            SHR_IF_ERR_EXIT
                (shr_thread_stop(param->tx_thread_pid,
                                 PKT_MULTI_THREAD_STOP_WAITTIME * SECOND_USEC));
            param->tx_thread_pid = NULL;
        }

        /* stop rx traffic (by change ifp policy to drop) */
        for (i = 0; i < param->rx_num; i++) {
            SHR_IF_ERR_EXIT
                (bcma_bcmpkt_test_dpath_update_policy
                    (unit, param->rx_ifp_cfg[i].entry_id,
                    param->rx_ifp_cfg[i].drop_policy_id));
        }

        /* free the packet in this round */
        pkt_multi_free_packet(unit, param);
        if (pkt_len == param->len_end) {
            break;
        }
        pkt_len += param->len_inc;
        if (pkt_len > param->len_end) {
            pkt_len = param->len_end;
        }
    }

exit:
    /*
     * all resources would be cleanup in pkt_multi_recycle_cb()
     * to avoid this function would be interrupted by ctrl-c
     */
    SHR_FUNC_EXIT();
}

static void
pkt_multi_recycle_cb(int unit, void *bp)
{
    /* param would be freed in testdb_run_teardown() */
    pkt_multi_param_t *param = (pkt_multi_param_t *)bp;
    int rv, i;

    if (!param) {
        return;
    }

    pkt_multi_free_packet(unit, param);
    if (param->in_testing != NULL) {
        shr_atomic_int_destroy(param->in_testing);
        param->in_testing = NULL;
    }
    if (param->tx_thread_pid != NULL) {
        rv = shr_thread_stop(param->tx_thread_pid,
                             PKT_MULTI_THREAD_STOP_WAITTIME * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "cannot stop thread\n")));
        }
        param->tx_thread_pid = NULL;
    }
    if (param->rx_num != 0) {
        rv = bcmpkt_rx_unregister(unit, 0, pkt_multi_run_rx_cb, (void*)param);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "error to unregister rx cb\n")));
        }
        for (i = 0; i < param->rx_num; i++) {
            rv = bcma_bcmpkt_test_dpath_destroy
                    (unit, param->rx_lb_port[i], &param->rx_ifp_cfg[i]);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "error to destroy dpath\n")));
            }
        }
    }
    (void)bcma_testutil_packet_dev_cleanup(unit);
    return;
}

static bcma_test_proc_t pkt_multi_proc[] = {
    {
        .desc = "init packet I/O device\n",
        .cb = pkt_multi_pkt_dev_setup_cb,
    },
    {
        .desc = "set port loopback mode, wait link up\n",
        .cb = pkt_multi_port_lb_set_cb,
    },
    {
        .desc = "run packet I/O Tx test\n",
        .cb = pkt_multi_run_cb,
    },
    {
        .desc = "clean port loopback mode\n",
        .cb = pkt_multi_port_lb_cleanup_cb,
    },
};

static bcma_test_op_t pkt_multi_op = {
    .select = bcma_testcase_pkt_select,
    .parser = pkt_multi_parser,
    .help = pkt_multi_print_help,
    .recycle = pkt_multi_recycle_cb,
    .procs = pkt_multi_proc,
    .proc_count = COUNTOF(pkt_multi_proc)
};

bcma_test_op_t *
bcma_testcase_pkt_multi_op_get(void)
{
    return &pkt_multi_op;
}
