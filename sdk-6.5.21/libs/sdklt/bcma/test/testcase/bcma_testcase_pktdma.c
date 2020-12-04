/*! \file bcma_testcase_pktdma.c
 *
 *  Packet DMA test common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <bcmlt/bcmlt.h>
#include <bcmbd/bcmbd.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_l2.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include "bcma_testcase_pktdma_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define TX_BITMAP_DEFAULT           0x0001
#define RX_BITMAP_DEFAULT           0x0002
#define PKT_SIZE_DEFAULT            128
#define NUM_PKTS_DEFAULT            5
#define TEST_TYPE_DEFAULT           PKTDMA_TEST_TYPE_SG_CHAIN
#define CHECK_PKT_DEFAULT           1
#define VERBOSE_DEFAULT             0
#define INTRMODE_DEFAULT            0
#define CONTINUOUSMODE_DEFAULT      0
#define CMIC_LB_DEFAULT             0

#define PKTDMA_CHAN_MAX             16
#define PKTDMA_CPU_PORT             0
#define PKTDMA_PKT_PATTEN           0xdeadbeef

#define PKTDMA_ENET_MAC_SIZE        6
#define PKTDMA_ENET_HDR_SIZE        PKTDMA_ENET_MAC_SIZE * 2 + 4 + 2

static uint8_t dst_mac[6] = {0x12,0x34,0x56,0x78,0x9a,0xbc};
static uint8_t src_mac[6] = {0xfe,0xdc,0xba,0x98,0x76,0x54};
static const uint16_t tpid = 0x8100;
static const uint16_t vlan = 0x0abc;
static const uint16_t vlan_pid = 1;
static const uint16_t eth_type = 0xffff;

static int
packet_fill_payload(uint8_t *buf, uint32_t pat, uint32_t size)
{
    int pat_off = 24;

    while (size > 3) {
        *buf++ = pat >> 24;
        *buf++ = pat >> 16;
        *buf++ = pat >> 8;
        *buf++ = pat & 0xff;
        size -= 4;
    }

    while (size > 0) {
        *buf++ = pat >> pat_off;
        pat_off -= 8;
        size--;
    }

    return SHR_E_NONE;
}

int
bcma_testcase_pktdma_packet_fill(uint8_t *packet, uint32_t pkt_len, int inc)
{
    if (pkt_len < PKTDMA_ENET_HDR_SIZE) {
        return SHR_E_PARAM;
    }

    sal_memcpy(packet, dst_mac, PKTDMA_ENET_MAC_SIZE);
    packet += PKTDMA_ENET_MAC_SIZE;

    src_mac[5] += inc;
    sal_memcpy(packet, src_mac, PKTDMA_ENET_MAC_SIZE);
    packet += PKTDMA_ENET_MAC_SIZE;

    /* 802.1Q */
    *packet++ = tpid >> 8;
    *packet++ = tpid & 0xff;
    *packet++ = vlan >> 8;
    *packet++ = vlan & 0xff;

    *packet++ = eth_type >> 8;
    *packet++ = eth_type & 0xff;

    pkt_len -= PKTDMA_ENET_HDR_SIZE;
    packet_fill_payload(packet, PKTDMA_PKT_PATTEN + inc, pkt_len);

    return SHR_E_NONE;
}

static int
cpu_port_verify(int unit)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, "PORT_ID", PKTDMA_CPU_PORT));

    rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        cli_out("Please load a config.yml with CPU port.\n");
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testcase_pktdma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                            uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    pktdma_test_param_t *p = NULL;
    uint32_t idx, num_tx_chan = 0, num_rx_chan = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for saving test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCasePktdmaPrm");
    SHR_NULL_CHECK(p, SHR_E_MEMORY);

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->tx_ch_bmp            = TX_BITMAP_DEFAULT;
    p->rx_ch_bmp            = RX_BITMAP_DEFAULT;
    p->pkt_size             = PKT_SIZE_DEFAULT;
    p->num_pkts             = NUM_PKTS_DEFAULT;
    p->test_type            = TEST_TYPE_DEFAULT;
    p->check_pkt            = CHECK_PKT_DEFAULT;
    p->verbose              = VERBOSE_DEFAULT;
    p->intr_mode            = INTRMODE_DEFAULT;
    p->continuous_mode      = CONTINUOUSMODE_DEFAULT;
    p->cmic_lb              = CMIC_LB_DEFAULT;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "TxBitmap", "hex",
                             &p->tx_ch_bmp, NULL);
    bcma_cli_parse_table_add(&pt, "RxBitmap", "hex",
                             &p->rx_ch_bmp, NULL);
    bcma_cli_parse_table_add(&pt, "PktSize", "int",
                             &p->pkt_size, NULL);
    bcma_cli_parse_table_add(&pt, "NumPkts", "int",
                             &p->num_pkts, NULL);
    bcma_cli_parse_table_add(&pt, "TestType", "int",
                             &p->test_type, NULL);
    bcma_cli_parse_table_add(&pt, "CheckPkt", "bool",
                             &p->check_pkt, NULL);
    bcma_cli_parse_table_add(&pt, "Verbose", "bool",
                             &p->verbose, NULL);
    bcma_cli_parse_table_add(&pt, "IntrMode", "bool",
                             &p->intr_mode, NULL);
    bcma_cli_parse_table_add(&pt, "ContinuousMode", "bool",
                             &p->continuous_mode, NULL);

    /* Parse the parse table */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) > 0) {
        cli_out("%s: Unknown argument %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Show input arguments for "Test Help" CMD */
    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        bcma_cli_parse_table_show(&pt, NULL);
    }

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    /* Verify CPU port before test */
    SHR_IF_ERR_EXIT(cpu_port_verify(unit));

    for (idx = 0; idx < PKTDMA_CHAN_MAX; idx++) {
        if (((p->tx_ch_bmp>> idx) % 2) != 0) {
            num_tx_chan++;
        }
        if (((p->rx_ch_bmp>> idx) % 2) != 0) {
            num_rx_chan++;
        }
    }

    if (num_tx_chan != num_rx_chan) {
        cli_out("Tx Rx channels are not in pair Tx %"PRId32" Rx %"PRId32"\n",
                num_tx_chan, num_rx_chan);
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    } else {
        p->num_pairs = num_tx_chan;
    }

    if ((p->pkt_size != 0) &&
        ((p->pkt_size < PKTDMA_MIN_PKT_SIZE) ||
         (p->pkt_size > PKTDMA_MAX_PKT_SIZE))) {
        cli_out("Invalid PktSize parameter %"PRId32" should be 0 or [64...1518]\n",
                p->pkt_size);
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if ((p->num_pkts <= 0) || (p->num_pkts > PKTDMA_MAX_PKT_NUM)) {
        cli_out("Invalid NumPkts parameter %"PRId32" should be [1...16]\n",
                p->num_pkts);
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    switch (p->test_type) {
    case PKTDMA_TEST_TYPE_SIMPLE:
        p->num_pkts = 1;
        p->num_dcbs = 1;
        break;
    case PKTDMA_TEST_TYPE_SG:
        p->num_pkts = 1;
        p->num_dcbs = p->num_pkts * PKTDMA_DCB_FACTOR_SG;
        p->flags |= PKTDMA_F_SG;
        break;
    case PKTDMA_TEST_TYPE_CHAIN:
        p->num_dcbs = p->num_pkts;
        p->flags |= PKTDMA_F_CHAIN;
        break;
    case PKTDMA_TEST_TYPE_SG_CHAIN:
        p->num_dcbs = p->num_pkts * PKTDMA_DCB_FACTOR_SG;
        p->flags |= PKTDMA_F_SG | PKTDMA_F_CHAIN;
        break;
    case PKTDMA_TEST_TYPE_RELOAD:
        p->num_dcbs = p->num_pkts + PKTDMA_DCB_FACTOR_RL;
        p->flags |= PKTDMA_F_RELOAD | PKTDMA_F_CHAIN;
        break;
    default:
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcma_testcase_pktdma_help(int unit, void *bp)
{
    pktdma_test_param_t *p = (pktdma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  TxBitmap     - 16-bit bitmap of Tx channels\n");
    cli_out("  RxBitmap     - 16-bit bitmap of Rx channels\n");
    cli_out("  PktSize      - Packet size in bytes (include one VLAN tag and CRC)\n"
            "                 0 for random packet sizes\n");
    cli_out("  NumPkts      - Number of packets for which descriptors are set up at one time\n");
    cli_out("  TestType     - 0: Simple Packet DMA\n"
            "                 1: Scatter/Gather without packet chaining\n"
            "                 2: Chained DMA\n"
            "                 3: Scatter/Gather with packet chaining\n"
            "                 4: Reload descriptor is inserted in the middle of chain\n");
    cli_out("  CheckPkt     - Enable packet data checks\n");
    cli_out("  Verbose      - Print descriptors and packet pointers\n");
    cli_out("  IntrMode     - 0: Polled mode (default)\n"
            "                 1: Intrrupt mode\n");
    cli_out("  ContinuousMode - 0: Non-continuous mode (default)\n"
            "                   1: Continuous mode\n");
    cli_out("  ====================\n");
    cli_out("  tx_ch_bmp    =0x%"PRIx32"\n", p->tx_ch_bmp);
    cli_out("  rx_ch_bmp    =0x%"PRIx32"\n", p->rx_ch_bmp);
    cli_out("  pkt_size     =%"PRId32"\n", p->pkt_size);
    cli_out("  num_pkts     =%"PRId32"\n", p->num_pkts);
    cli_out("  test_type    =%"PRId32"\n", p->test_type);
    cli_out("  check_pkt    =%"PRId32"\n", p->check_pkt);
    cli_out("  verbose      =%"PRId32"\n", p->verbose);
    cli_out("  intr_mode    =%"PRId32"\n", p->intr_mode);
    cli_out("  continuous_mode=%"PRId32"\n", p->continuous_mode);

    cli_out("  num_pairs    =%"PRId32"\n", p->num_pairs);
    cli_out("  num_dcbs     =%"PRId32"\n", p->num_dcbs);
    cli_out("  flags        =0x%"PRIx32"\n", p->flags);
}

int
bcma_testcase_pktdma_buf_alloc(int unit, pktdma_test_param_t *p, int bufsize)
{
    uint32_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    /* Allocate tx packet buffer from DMA memory pool */
    for (idx = 0; idx < p->num_pkts; idx++) {
        p->txpkt[idx] = bcmdrd_hal_dma_alloc(unit, bufsize,
                                             "bcmaTestCasePktdmaTxPkt",
                                             &p->txpkt_dma_addr[idx]);
        SHR_NULL_CHECK(p->txpkt[idx], SHR_E_MEMORY);
        sal_memset(p->txpkt[idx], 0, bufsize);
    }

    /* Allocate rx packet buffer from DMA memory pool */
    for (idx = 0; idx < p->num_pkts; idx++) {
        p->rxpkt[idx] = bcmdrd_hal_dma_alloc(unit, bufsize,
                                             "bcmaTestCasePktdmaRxPkt",
                                             &p->rxpkt_dma_addr[idx]);
        SHR_NULL_CHECK(p->rxpkt[idx], SHR_E_MEMORY);
        sal_memset(p->rxpkt[idx], 0, bufsize);
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcma_testcase_pktdma_buf_free(int unit, pktdma_test_param_t *p, int bufsize)
{
    uint32_t idx;

    if (p == NULL) {
        return;
    }

    for (idx = 0; idx < p->num_pkts; idx++) {
        if (p->txpkt[idx]) {
            bcmdrd_hal_dma_free(unit, bufsize,
                                p->txpkt[idx], p->txpkt_dma_addr[idx]);
            p->txpkt[idx] = NULL;
        }
    }

    for (idx = 0; idx < p->num_pkts; idx++) {
        if (p->rxpkt[idx]) {
            bcmdrd_hal_dma_free(unit, bufsize,
                                p->rxpkt[idx], p->rxpkt_dma_addr[idx]);
            p->rxpkt[idx] = NULL;
        }
    }
}

void
bcma_testcase_pktdma_recycle(int unit, void *bp)
{
    return;
}

int
bcma_testcase_pktdma_vlan_set_cb(int unit, void *bp, uint32_t option)
{
    bcmdrd_pbmp_t pbmp, ubmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_create(unit, vlan, vlan_pid));

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, PKTDMA_CPU_PORT);
    BCMDRD_PBMP_CLEAR(ubmp);

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_port_add(unit, vlan, pbmp, ubmp, false));
    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_stp_set(unit, vlan, pbmp,
                                    BCMA_TESTUTIL_STP_FORWARD));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_pktdma_l2_forward_add_cb(int unit, void *bp, uint32_t option)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_l2_unicast_add(unit,
                                      PKTDMA_CPU_PORT,
                                      dst_mac,
                                      vlan,
                                      1));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testcase_pktdma_cleanup_cb(int unit, void *bp, uint32_t option)
{
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_CONT
        (bcma_testutil_l2_unicast_delete(unit,
                                         PKTDMA_CPU_PORT,
                                         dst_mac,
                                         vlan,
                                         1));

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, PKTDMA_CPU_PORT);
    SHR_IF_ERR_CONT
        (bcma_testutil_vlan_port_remove(unit, vlan, pbmp));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_destroy(unit, vlan));

exit:
    SHR_FUNC_EXIT();
}
