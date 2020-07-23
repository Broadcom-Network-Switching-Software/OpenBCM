/*! \file bcma_testcase_lb.c
 *
 *  Loopback test common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_l2.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include "bcma_testcase_lb_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define LB_DEFAULT_NETIF_ID 1
#define LB_DEFAULT_VLAN_ID 1
#define LB_DEFAULT_VLAN_PROFILE_ID 0

#define LB_CPU_PORT 0

#define LB_ENET_MAC_SIZE 6
#define LB_ENET_TPID 0x8100
#define LB_ENET_ETHERTYPE 0xFFFF

#define LB_PACKET_RCV_TIMEOUT 5
#define LB_REPORT_INTERVAL 5

#define LB_TX_RX_NONE 0
#define LB_TX_FAILED  1
#define LB_RX_FAILED  2

#define LB_PARAM_HELP_0 \
    "  PortList        - Destination port list\n"\
    "  VlanID          - 802.1Q VLAN identifier\n"

#define LB_PARAM_HELP_1 \
    "  PortListA       - Port list group A\n"\
    "  PortListB       - Port list group B\n"\
    "  VlanID          - 802.1Q VLAN identifier\n"

#define LB_PARAM_HELP_2 \
    "  ITERation       - Number of iterations to repeat\n"\
    "  LengthStart     - Starting packet length\n"\
    "  LengthEnd       - Ending packet length\n"\
    "  LengthIncrement - Increasing step of packet length\n"\
    "  CosStart        - Starting cosq\n"\
    "  CosEnd          - Ending cosq\n"\
    "  CheckPostLink   - Check link down status after testing\n"

#define MAC_LB_EXAMPLE \
    "\nExamples:\n"\
    "  tr 18 pl=1-8 iter=10 ls=64 le=1500 li=100 cs=0 ce=7 vid=11\n"\
    "  tr 18 pl=1-8\n"\
    "  tr 18\n"

#define PHY_LB_EXAMPLE \
    "\nExamples:\n"\
    "  tr 19 pl=1-8 iter=10 ls=64 le=1500 li=100 cs=0 ce=7 vid=11\n"\
    "  tr 19 pl=1-8\n"\
    "  tr 19\n"

#define CPU_LB_EXAMPLE \
    "\nExamples\n"\
    "  tr 17 iter=10 ls=64 le=1500 li=100 cs=0 ce=7\n"\
    "  tr 17\n"

#define EXT_LB_EXAMPLE \
    "\nExamples:\n"\
    " port pair : (1, 5), (2, 6), (3, 7), (4, 35)\n"\
    "  tr 20 pla=1-4 plb=5-7,35 iter=10 ls=64 le=1500 li=100 cs=0 ce=7 vid=11\n"\
    "  tr 20 pla=1-4 plb=5-7,35\n"

static const shr_mac_t lb_default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t lb_default_dst_mac = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};

static void
lb_stat_init(int unit, lb_param_t *lb_param)
{
    int port, len_num, cos_num, port_num = 0;

    lb_param->tx_pkts = 0;
    lb_param->tx_bytes = 0;
    lb_param->rx_pkts = 0;
    lb_param->rx_bytes = 0;

    lb_param->tx_time = sal_time();
    lb_param->rx_time = lb_param->tx_time;
    lb_param->report_time = lb_param->tx_time + LB_REPORT_INTERVAL;

    BCMDRD_PBMP_ITER(lb_param->pbmp, port) {
        port_num++;
    }
    BCMDRD_PBMP_ITER(lb_param->pbmp2, port) {
        port_num++;
    }

    len_num = (lb_param->pkt_len_end - lb_param->pkt_len_start) / lb_param->pkt_len_inc + 1;
    cos_num = lb_param->cos_end - lb_param->cos_start + 1;
    if (lb_param->lb_type == LB_TYPE_CPU) {
        port_num = LB_CMC_CHAN_NUM * (LB_CMC_CHAN_NUM - 1);
    }
    lb_param->tx_total = len_num * lb_param->iteration * cos_num * port_num;

    cli_out("total %d pkt (%d length * %d iteration * %d cos * %d %s)\n",
        lb_param->tx_total, len_num, lb_param->iteration, cos_num, port_num,
        (lb_param->lb_type != LB_TYPE_CPU) ? "port" : "chan");
}

static void
lb_stat_report(int unit, lb_param_t *lb_param)
{
    int secs;

    secs = lb_param->rx_time - lb_param->tx_time;
    if (secs == 0) {
        secs = 1;
    }

    cli_out("xmit %d pkt (%d%%, %d pkt/s, %d KB/s avg), "
        "recv %d pkt (%d sec)\n",
        lb_param->tx_pkts, (100*lb_param->tx_pkts/lb_param->tx_total),
        (lb_param->tx_pkts/secs), (lb_param->tx_bytes/1024/secs),
        lb_param->rx_pkts, secs);
}

static uint32_t
packet_fill_payload(uint8_t *buf, int size, uint32_t pat, uint32_t inc)
{
    int pat_off = 24;

    while (size > 3) {
        *buf++ = pat >> 24;
        *buf++ = pat >> 16;
        *buf++ = pat >> 8;
        *buf++ = pat & 0xff;
        pat += inc;
        size -= 4;
    }

    while (size > 0) {
        *buf++ = pat >> pat_off;
        pat_off -= 8;
        size--;
    }

    return pat;
}

static int
packet_fill_data(bcmpkt_data_buf_t *buf, shr_mac_t dst_mac, shr_mac_t src_mac,
                 int vid, uint32_t pkt_pattern, int pkt_len)
{
    uint8_t *cur_addr;
    const int tpid = LB_ENET_TPID;
    const int eth_type = LB_ENET_ETHERTYPE;

    bcmpkt_trim(buf, 0);
    bcmpkt_put(buf, pkt_len);

    cur_addr = buf->data;
    sal_memcpy(cur_addr, dst_mac, LB_ENET_MAC_SIZE);
    cur_addr += LB_ENET_MAC_SIZE;
    sal_memcpy(cur_addr, src_mac, LB_ENET_MAC_SIZE);
    cur_addr += LB_ENET_MAC_SIZE;

    /* 802.1Q */
    *cur_addr++ = tpid >> 8;
    *cur_addr++ = tpid & 0xff;
    *cur_addr++ = vid >> 8;
    *cur_addr++ = vid & 0xff;

    *cur_addr++ = eth_type >> 8;
    *cur_addr++ = eth_type & 0xff;

    pkt_len -= (LB_ENET_MAC_SIZE*2 + 4 + 2);
    packet_fill_payload(cur_addr, pkt_len, pkt_pattern, 0);

    return SHR_E_NONE;
}

static void
packet_dump(const bcmpkt_data_buf_t *buf, const char *head)
{
    uint8_t *cur_addr;
    uint32_t idx;

    cli_out("%s", head);

    cur_addr = buf->data;
    for (idx = 0; idx < buf->data_len; idx++) {
        if (idx % 8 == 0) {
            cli_out("\n");
        }
        cli_out("%02x ", *cur_addr++);
    }
    cli_out("\n");
}

static int
packet_compare(const bcmpkt_data_buf_t *buf1, const bcmpkt_data_buf_t *buf2)
{
    uint8_t *pbuf1, *pbuf2;
    uint32_t idx;

    if (buf1->data_len != buf2->data_len) {
        return -1;
    }

    pbuf1 = buf1->data;
    pbuf2 = buf2->data;

    for (idx = 0; idx < buf1->data_len; idx++) {
        if (pbuf1[idx] != pbuf2[idx]) {
            return idx;
        }
    }

    return 0;
}

static int
packet_receive_cb(int unit, int netif_id, bcmpkt_packet_t *packet,
                  void *cookie)
{
    lb_param_t *lb_param = (lb_param_t *)cookie;
    SHR_FUNC_ENTER(unit);

    lb_param->rx_time = sal_time();
    lb_param->rx_pkts++;
    lb_param->rx_bytes += packet->data_buf->data_len;

    if (lb_param->rx_time >= lb_param->report_time) {
        lb_param->report_time = lb_param->rx_time + LB_REPORT_INTERVAL;
        lb_stat_report(unit, lb_param);
    }

    /* check incoming port */
    if (lb_param->lb_type != LB_TYPE_CPU) {
        uint32_t src_port;
        int rv = bcmpkt_rxpmd_field_get(
                    lb_param->pkt_dev_type, packet->pmd.rxpmd,
                    BCMPKT_RXPMD_SRC_PORT_NUM, (uint32_t *)&src_port);

        if (SHR_FAILURE(rv)) {
            cli_out("Cannot get value from RXPMD field\n");
            lb_param->tx_rx_state = LB_RX_FAILED;
            SHR_EXIT();
        } else if ((int)src_port != lb_param->port_rx_expect) {
            cli_out("packet incoming port is %"PRId32"(expect :%"PRId32")\n",
                    src_port, lb_param->port_rx_expect);
            lb_param->tx_rx_state = LB_RX_FAILED;
            SHR_EXIT();
        }
    }

    /* check packet content */
    if (lb_param->packet != NULL && lb_param->packet->data_buf != NULL) {
        if (packet_compare(lb_param->packet->data_buf, packet->data_buf) != 0) {
            lb_param->tx_rx_state = LB_RX_FAILED;

            packet_dump(packet->data_buf, "RX packet: ");
            packet_dump(lb_param->packet->data_buf, "TX packet: ");
            SHR_EXIT();
        }
    }

 exit:
    sal_sem_give(lb_param->sema);
    SHR_FUNC_EXIT();
}

static int
packet_tx_rx(int unit, lb_param_t *lb_param, int port_tx, int port_rx)
{
    bcmpkt_packet_t *pkt = NULL;
    uint32_t *txpmd;
    int pkt_len, cos, iter, rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lb_param, SHR_E_PARAM);

    if (port_tx != LB_CPU_PORT) {
        SHR_IF_ERR_EXIT
            (bcmpkt_fwd_port_set(lb_param->pkt_dev_type,
                                 port_tx, lb_param->packet));
    }

    /* Iterate all packet length */
    for (pkt_len = lb_param->pkt_len_start; pkt_len <= lb_param->pkt_len_end;
         pkt_len += lb_param->pkt_len_inc) {
        packet_fill_data(lb_param->packet->data_buf, lb_param->dst_mac,
            lb_param->src_mac, lb_param->vid, lb_param->pkt_pattern, pkt_len);

        SHR_IF_ERR_EXIT
            (bcmpkt_packet_clone(unit, lb_param->packet, &pkt));

        /* Iterate all cos */
        for (cos = lb_param->cos_start; cos <= lb_param->cos_end; cos++) {
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_get(pkt, &txpmd));
            /* Class of service for MMU queueing for this packet */
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_field_set(lb_param->pkt_dev_type, txpmd,
                                        BCMPKT_TXPMD_COS, cos));

            for (iter = 0; iter < lb_param->iteration; iter++) {
                /* Copy data */
                if (pkt->data_buf != NULL) {
                    bcmpkt_data_buf_free(pkt->unit, pkt->data_buf);
                    pkt->data_buf = NULL;
                }
                SHR_IF_ERR_EXIT
                    (bcmpkt_data_buf_copy(lb_param->packet->unit,
                                          lb_param->packet->data_buf,
                                          &pkt->data_buf));

                lb_param->tx_rx_state = LB_TX_RX_NONE;
                lb_param->port_rx_expect = port_rx;
                rv = bcmpkt_tx(unit, lb_param->netif_id, pkt);
                if (SHR_FAILURE(rv)) {
                    lb_param->tx_rx_state = LB_TX_FAILED;
                    break;
                }

                lb_param->tx_pkts++;
                lb_param->tx_bytes += pkt_len;

                /* Wait packet come in */
                if (sal_sem_take(lb_param->sema, lb_param->timeout)) {
                    lb_param->tx_rx_state = LB_RX_FAILED;
                    break;
                }

                if (lb_param->tx_rx_state != LB_TX_RX_NONE) {
                    break;
                }
            }

            if (lb_param->tx_rx_state != LB_TX_RX_NONE) {
                break;
            }
        }

        bcmpkt_free(pkt->unit, pkt);
        pkt = NULL;

        if (lb_param->tx_rx_state != LB_TX_RX_NONE) {
            SHR_IF_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if (pkt != NULL) {
        bcmpkt_free(pkt->unit, pkt);
    }

    SHR_FUNC_EXIT();
}

static int
lb_param_init(int unit, lb_param_t *lb_param)
{
    int rv = SHR_E_NONE;

    if (lb_param == NULL) {
        return SHR_E_PARAM;
    }

    rv = bcmpkt_dev_type_get(unit, &lb_param->pkt_dev_type);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    BCMDRD_PBMP_CLEAR(lb_param->pbmp);
    BCMDRD_PBMP_CLEAR(lb_param->pbmp2);

    lb_param->iteration = 10;
    lb_param->timeout = LB_PACKET_RCV_TIMEOUT*1000000;
    lb_param->netif_id = LB_DEFAULT_NETIF_ID;

    sal_memcpy(lb_param->src_mac, lb_default_src_mac, sizeof(lb_param->src_mac));
    sal_memcpy(lb_param->dst_mac, lb_default_dst_mac, sizeof(lb_param->dst_mac));
    lb_param->vid = LB_DEFAULT_VLAN_ID;

    lb_param->pkt_pattern = 0xa5a4a3a2;
    lb_param->pkt_len_start = 68;
    lb_param->pkt_len_end = 1522;
    lb_param->pkt_len_inc = 128;
    lb_param->cos_start = 0;
    lb_param->cos_end = 7;
    lb_param->check_post_link = TRUE;

    return rv;
}

static int
lb_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
               lb_param_t *lb_param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *pbmp = NULL, *pbmp2 = NULL;

    SHR_FUNC_ENTER(unit);

    bcma_cli_parse_table_init(cli, &pt);

    SHR_IF_ERR_EXIT(lb_param_init(unit, lb_param));

    if (lb_param->lb_type == LB_TYPE_MAC || lb_param->lb_type == LB_TYPE_PHY) {
        bcma_cli_parse_table_add(&pt, "PortList", "str", &pbmp, NULL);
        bcma_cli_parse_table_add(&pt, "VlanID", "int", &lb_param->vid, NULL);
    } else if (lb_param->lb_type == LB_TYPE_EXT) {
        bcma_cli_parse_table_add(&pt, "PortListA", "str", &pbmp, NULL);
        bcma_cli_parse_table_add(&pt, "PortListB", "str", &pbmp2, NULL);
        bcma_cli_parse_table_add(&pt, "VlanID", "int", &lb_param->vid, NULL);
    }
    bcma_cli_parse_table_add(&pt, "ITERation", "int", &lb_param->iteration, NULL);
    bcma_cli_parse_table_add(&pt, "LengthStart", "int", &lb_param->pkt_len_start, NULL);
    bcma_cli_parse_table_add(&pt, "LengthEnd", "int", &lb_param->pkt_len_end, NULL);
    bcma_cli_parse_table_add(&pt, "LengthIncrement", "int", &lb_param->pkt_len_inc, NULL);
    bcma_cli_parse_table_add(&pt, "CosStart", "int", &lb_param->cos_start, NULL);
    bcma_cli_parse_table_add(&pt, "CosEnd", "int", &lb_param->cos_end, NULL);
    bcma_cli_parse_table_add(&pt, "CheckPostLink", "int", &lb_param->check_post_link, NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) > 0) {
        cli_out("%s: Unknown argument %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (show) {
        /* just show parsing result and exit function */
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }

    switch(lb_param->lb_type) {
        case LB_TYPE_CPU:
            BCMDRD_PBMP_PORT_ADD(lb_param->pbmp, LB_CPU_PORT);
            break;
        case LB_TYPE_PHY:
        case LB_TYPE_MAC:
            if (pbmp != NULL) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_parse_port_bmp(pbmp, &lb_param->pbmp));
            } else {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_default_port_bmp_get
                        (unit, BCMDRD_PORT_TYPE_FPAN | BCMDRD_PORT_TYPE_MGMT,
                         &lb_param->pbmp));
            }
            break;
        case LB_TYPE_EXT:
            if (pbmp != NULL) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_parse_port_bmp(pbmp, &lb_param->pbmp));
            }
            if (pbmp2 != NULL) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_parse_port_bmp(pbmp2, &lb_param->pbmp2));
            }
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (lb_param->pkt_len_start < 64 || lb_param->pkt_len_inc <= 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    bcma_cli_parse_table_done(&pt);

    SHR_FUNC_EXIT();
}

static void
lb_print_help(int unit, lb_param_t *lb_param)
{
    if (lb_param == NULL) {
        return;
    }

    switch(lb_param->lb_type) {
        case LB_TYPE_CPU:
            cli_out("%s", LB_PARAM_HELP_2);
            cli_out("%s", CPU_LB_EXAMPLE);
            break;

        case LB_TYPE_MAC:
            cli_out("%s", LB_PARAM_HELP_0);
            cli_out("%s", LB_PARAM_HELP_2);
            cli_out("%s", MAC_LB_EXAMPLE);
            break;

        case LB_TYPE_PHY:
            cli_out("%s", LB_PARAM_HELP_0);
            cli_out("%s", LB_PARAM_HELP_2);
            cli_out("%s", PHY_LB_EXAMPLE);
            break;

        case LB_TYPE_EXT:
            cli_out("%s", LB_PARAM_HELP_1);
            cli_out("%s", LB_PARAM_HELP_2);
            cli_out("%s", EXT_LB_EXAMPLE);
            break;

        default:
            break;
    }

    return;
}

int
bcma_testcase_lb_txrx_init(int unit, lb_param_t *lb_param)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lb_param, SHR_E_PARAM);

    lb_param->sema = sal_sem_create("lb-sema", SAL_SEM_BINARY, 0);
    if (lb_param->sema == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }



    lb_stat_init(unit, lb_param);

exit:
    if (SHR_FUNC_ERR()) {
        if (lb_param->sema != NULL) {
            sal_sem_destroy(lb_param->sema);
            lb_param->sema = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcma_testcase_lb_txrx_single(int unit, lb_param_t *lb_param,
                             bcmdrd_pbmp_t tx_pbmp, bcmdrd_pbmp_t rx_pbmp)
{
    int port_tx, port_rx;
    uint64_t tx_pkt, rx_pkt;
    uint64_t tx_err, rx_err, rx_err0, rx_err1, rx_err2, rx_err3;
    SHR_FUNC_ENTER(unit);

    port_rx = 0;
    BCMDRD_PBMP_ITER(tx_pbmp, port_tx) {
        for (; port_rx < BCMDRD_CONFIG_MAX_PORTS; port_rx++) {
            if (BCMDRD_PBMP_MEMBER(rx_pbmp, port_rx)) {
                break;
            }
        }
        if (port_rx == BCMDRD_CONFIG_MAX_PORTS) {
            /* cannot find rx port? should not happen */
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* clear stat */
        if (lb_param->lb_type != LB_TYPE_CPU) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_enable(unit, port_tx, 1));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_enable(unit, port_rx, 1));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_clear(unit, port_tx));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_clear(unit, port_rx));
        }

        /* send packet via port_tx */
        SHR_IF_ERR_EXIT
            (packet_tx_rx(unit, lb_param, port_tx, port_rx));

        /* check stat */
        if (lb_param->lb_type != LB_TYPE_CPU) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_enable(unit, port_tx, 0));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_enable(unit, port_rx, 0));

            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_tx,
                    SNMP_IF_OUT_UCAST_PKTS, &tx_pkt));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_rx,
                    SNMP_IF_IN_UCAST_PKTS, &rx_pkt));

            /* Check TX error */
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_tx,
                    SNMP_IF_OUT_ERRORS, &tx_err));

            /* Check RX error */
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_rx,
                    SNMP_ETHER_STATS_UNDERSIZE_PKTS, &rx_err0));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_rx,
                    SNMP_ETHER_STATS_FRAGMENTS, &rx_err1));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_rx,
                    SNMP_ETHER_STATS_JABBERS, &rx_err2));
            SHR_IF_ERR_EXIT
                (bcma_testutil_stat_get(unit, port_rx,
                    SNMP_ETHER_STATS_CRC_ALIGN_ERRORS, &rx_err3));

            rx_err = rx_err0 + rx_err1 + rx_err2 + rx_err3;
            if (tx_pkt != rx_pkt || tx_err || rx_err) {
                SHR_IF_ERR_EXIT(SHR_E_FAIL);
            }
        }
        port_rx++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_lb_txrx(int unit, lb_param_t *lb_param, int tx_chan, int rx_chan)
{
    int init_pktrx_done = 0, init_pktdev_done = 0;
    int pktdev_unit = unit;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lb_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_packet_dev_init
            (unit, tx_chan, rx_chan,
             lb_param->pkt_len_end + 4, /* include CRC */
             BCMPKT_BPOOL_BCOUNT_DEF, &(lb_param->netif_id), &pktdev_unit));

    init_pktdev_done = 1;

    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(pktdev_unit,
                      lb_param->pkt_len_end,
                      BCMPKT_BUF_F_TX,
                      &lb_param->packet));

    SHR_IF_ERR_EXIT
        (bcmpkt_rx_register(unit, lb_param->netif_id, 0, packet_receive_cb,
                            (void*)lb_param));

    init_pktrx_done = 1;

    if (lb_param->lb_type == LB_TYPE_EXT) {
        SHR_IF_ERR_EXIT(
            bcma_testcase_lb_txrx_single(
                unit, lb_param, lb_param->pbmp, lb_param->pbmp2));
        SHR_IF_ERR_EXIT(
            bcma_testcase_lb_txrx_single(
                unit, lb_param, lb_param->pbmp2, lb_param->pbmp));
    } else {
        SHR_IF_ERR_EXIT(
            bcma_testcase_lb_txrx_single(
                unit, lb_param, lb_param->pbmp, lb_param->pbmp));
    }

exit:
    if (init_pktrx_done) {
        (void)bcmpkt_rx_unregister(unit, lb_param->netif_id, packet_receive_cb,
                                   (void*)lb_param);
    }
    if (lb_param->packet != NULL) {
        bcmpkt_free(lb_param->packet->unit, lb_param->packet);
        lb_param->packet = NULL;
    }
    if (init_pktdev_done) {
        (void)bcma_testutil_packet_dev_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

void
bcma_testcase_lb_txrx_cleanup(int unit, lb_param_t *lb_param)
{
    if (lb_param == NULL) {
        return;
    }

    lb_stat_report(unit, lb_param);

    if (lb_param->sema != NULL) {
        sal_sem_destroy(lb_param->sema);
        lb_param->sema = NULL;
    }
}

int
bcma_testcase_lb_select(int unit)
{
    return true;
}

int
bcma_testcase_lb_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                        uint32_t flags, void **bp, testutil_port_lb_type_t type)
{
    lb_param_t *lb_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    lb_param = sal_alloc(sizeof(lb_param_t), "bcmaTestCaseLbPrm");
    SHR_NULL_CHECK(lb_param, SHR_E_MEMORY);

    sal_memset(lb_param, 0, sizeof(lb_param_t));
    *bp = (void*)lb_param;

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    lb_param->lb_type = type;
    SHR_IF_ERR_EXIT
        (lb_parse_param(unit, cli, a, lb_param, show));
    if (show == true) {
        /* would not free lb_param here, still needs it in help() */
        SHR_EXIT();
    }

    if (BCMDRD_PBMP_IS_NULL(lb_param->pbmp)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (type == LB_TYPE_EXT) {
        int count_of_pbmp = 0, count_of_pbmp2 = 0, port;
        bcmdrd_pbmp_t pbmp_overlay;

        /* check rule 1 : port number in pbmp and pbmp2 should be the same */
        BCMDRD_PBMP_ITER(lb_param->pbmp, port) {
            count_of_pbmp++;
        }
        BCMDRD_PBMP_ITER(lb_param->pbmp2, port) {
            count_of_pbmp2++;
        }
        if (count_of_pbmp != count_of_pbmp2) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        /* check rule 2 : pbmp and pbmp2 cannot overlay */
        BCMDRD_PBMP_ASSIGN(pbmp_overlay, lb_param->pbmp);
        BCMDRD_PBMP_AND(pbmp_overlay, lb_param->pbmp2);
        if (BCMDRD_PBMP_NOT_NULL(pbmp_overlay)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (lb_param != NULL) {
            sal_free(lb_param);
        }
        if (bp != NULL) {
            *bp = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

void
bcma_testcase_lb_help(int unit, void *bp)
{
    lb_param_t *lb_param = (lb_param_t *)bp;
    if (lb_param == NULL) {
        return;
    }

    lb_print_help(unit, lb_param);

    return;
}

void
bcma_testcase_lb_recycle(int unit, void *bp)
{
    return;
}

int
bcma_testcase_lb_vlan_set_cb(int unit, void *bp, uint32_t option)
{
    lb_param_t *lb_param = (lb_param_t *)bp;
    bcmdrd_pbmp_t pbmp, ubmp;
    bool src_port_block = true;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_create(unit, lb_param->vid,
                                   LB_DEFAULT_VLAN_PROFILE_ID));

    BCMDRD_PBMP_ASSIGN(pbmp, lb_param->pbmp);
    BCMDRD_PBMP_OR(pbmp, lb_param->pbmp2);
    BCMDRD_PBMP_PORT_ADD(pbmp, LB_CPU_PORT);
    BCMDRD_PBMP_CLEAR(ubmp);

    if (lb_param->lb_type == LB_TYPE_CPU) {
        src_port_block = false;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_port_add(unit, lb_param->vid, pbmp, ubmp,
                                     src_port_block));
    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_stp_set(unit, lb_param->vid, pbmp,
                                    BCMA_TESTUTIL_STP_FORWARD));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_lb_port_set_cb(int unit, void *bp, uint32_t option)
{
    lb_param_t *lb_param = (lb_param_t *)bp;
    bcmdrd_pbmp_t pbmp;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    BCMDRD_PBMP_ASSIGN(pbmp, lb_param->pbmp);
    BCMDRD_PBMP_OR(pbmp, lb_param->pbmp2);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, lb_param->lb_type, pbmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_lb_l2_forward_add_cb(int unit, void *bp, uint32_t option)
{
    lb_param_t *lb_param = (lb_param_t *)bp;
    int copy2cpu = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (lb_param->lb_type == LB_TYPE_CPU) {
        copy2cpu = 1;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_l2_unicast_add(unit,
                                      LB_CPU_PORT,
                                      lb_param->dst_mac,
                                      lb_param->vid,
                                      copy2cpu));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_lb_txrx_cb(int unit, void *bp, uint32_t option)
{
    lb_param_t *lb_param = (lb_param_t *)bp;
    int init_done = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testcase_lb_txrx_init(unit, lb_param));
    init_done = 1;

    SHR_IF_ERR_EXIT
        (bcma_testcase_lb_txrx(unit, lb_param, LB_DEFAULT_TX_CHAN,
                               LB_DEFAULT_RX_CHAN));

exit:
    if (init_done) {
        bcma_testcase_lb_txrx_cleanup(unit, lb_param);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testcase_lb_cleanup_cb(int unit, void *bp, uint32_t option)
{
    lb_param_t *lb_param = (lb_param_t *)bp;
    bcmdrd_pbmp_t pbmp;
    int copy2cpu = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (lb_param->lb_type == LB_TYPE_CPU) {
        copy2cpu = 1;
    }

    SHR_IF_ERR_CONT
        (bcma_testutil_l2_unicast_delete(unit,
                                         LB_CPU_PORT,
                                         lb_param->dst_mac,
                                         lb_param->vid,
                                         copy2cpu));

    BCMDRD_PBMP_ASSIGN(pbmp, lb_param->pbmp);
    BCMDRD_PBMP_OR(pbmp, lb_param->pbmp2);

    if (lb_param->lb_type != LB_TYPE_CPU) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_port_clear(unit, lb_param->lb_type, pbmp,
                                      lb_param->check_post_link));
    }

    BCMDRD_PBMP_PORT_ADD(pbmp, LB_CPU_PORT);
    SHR_IF_ERR_CONT
        (bcma_testutil_vlan_port_remove(unit, lb_param->vid, pbmp));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_destroy(unit, lb_param->vid));

exit:
    SHR_FUNC_EXIT();
}
