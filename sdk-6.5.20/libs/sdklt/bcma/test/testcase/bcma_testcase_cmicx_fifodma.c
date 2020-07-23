/*! \file bcma_testcase_cmicx_fifodma.c
 *
 * Broadcom test cases for CMICX FIFODMA  access
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_fifodma.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcmlt/bcmlt.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcma/test/util/bcma_testutil_l2.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_ctr.h>
#include <bcma/test/util/bcma_testutil_traffic.h>
#include "bcma_testcase_pkt_internal.h"

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_fifodma.h>
#include <bcma/test/util/bcma_testutil_stat.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)


typedef struct cmicx_fifodma_test_param_s {
    char *sname;            /* Symbol Name */
    int intrmode;           /* 0:Polling, 1:Intr, Default=0 */
    int debug;              /* 0:False, 1:True, Default=0 */
    int perf;               /* 0: Normal test, 1: Performance test */
    int pkt_cnt;

    int unit;
    int ctrl_interval;
    shr_thread_t handler_tc;
    bcmbd_fifodma_data_t data;
    bcmbd_fifodma_work_t work;
    int buf_entries;
    int fifo_chan;
    int test_result;
} cmicx_fifodma_test_param_t;

#define FIFODMA_DIRTY_DATA      0xdeadbeef

#define FIFO_POP_CMD_MSG        0x2a

static const shr_mac_t default_src_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const shr_mac_t default_dst_mac = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};

#define PACKET_NUM          100
#define PACKET_LEN          128
#define PACKET_VID          1
#define PACKET_ETHERTYPE    0xffff
#define PACKET_PATTEN       0xdeadbeef

#define ING_PORT 1
#define EGR_PORT 2

static int
cmicx_fifodma_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static uint32_t
cmicx_schan_opcode_set(int unit, int op_code, int dst_blk, int acc_type,
                       int data_len, int dma, uint32_t ignore_mask)
{
    uint32_t oc = 0;

    SCMH_OPCODE_SET(oc, op_code);
    SCMH_DSTBLK_SET(oc, dst_blk);
    SCMH_ACCTYPE_SET(oc, acc_type);
    SCMH_DATALEN_SET(oc, data_len);
    SCMH_DMA_SET(oc, dma);
    SCMH_BANK_SET(oc, ignore_mask);

    return oc;
}

static void
cmicx_fifodma_data_process(cmicx_fifodma_test_param_t *p, uint32_t *addr, int ptr, int entries)
{
    int i;
    uint32_t j;

    for (i = 0; i < entries; i++) {
        for (j = 0; j < p->data.data_width; j++) {
            if (addr[j] == FIFODMA_DIRTY_DATA) {
                p->test_result = SHR_E_FAIL;
                return;
            }
        }
        ptr = (ptr + 1) % p->buf_entries;
        addr += p->data.data_width;
        if (ptr == 0) {
            addr = p->work.buf_cache;
        }
    }

    p->test_result = SHR_E_NONE;
}

static void
cmicx_fifodma_intr_cb(int unit, void *data)
{
    cmicx_fifodma_test_param_t *p = (cmicx_fifodma_test_param_t *)data;
    shr_thread_wake(p->handler_tc);
}

static int
cmicx_fifodma_ctr_evict_cleanup_cb(int unit, void *bp, uint32_t option)
{
    uint32_t egr_port = EGR_PORT;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_ctr_evict_delete(unit, (void *)(&egr_port)));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fifodma_ctr_evict_setup_cb(int unit, void *bp, uint32_t option)
{
    uint32_t egr_port = EGR_PORT;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_ctr_evict_add(unit, (void *)(&egr_port)));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fifodma_ctr_evict_trigger(int unit, int pkt_cnt)
{
    bcmdrd_dev_type_t pkt_dev_type;
    bcmpkt_dev_drv_types_t drv_type;
    int idx;
    int unit_bcmpkt = unit, port = ING_PORT, netif_id = 1;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(unit);

    /* alloacte packet buffer */
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));

    unit_bcmpkt = (drv_type == BCMPKT_DEV_DRV_T_KNET) ?
                  BCMPKT_BPOOL_SHARED_ID : unit;

    for (idx = 0; idx < pkt_cnt; idx++) {
        SHR_IF_ERR_EXIT
            (bcmpkt_alloc(unit_bcmpkt, PACKET_LEN, BCMPKT_BUF_F_TX, &packet));
        SHR_NULL_CHECK(packet, SHR_E_MEMORY);

        SHR_IF_ERR_EXIT
            (bcma_testutil_packet_fill
                (packet->data_buf, PACKET_LEN,
                 default_src_mac, default_dst_mac, true, PACKET_VID,
                 PACKET_ETHERTYPE, PACKET_PATTEN, 0));

        SHR_IF_ERR_EXIT
            (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));

        SHR_IF_ERR_EXIT
            (bcmpkt_tx(unit, netif_id, packet));

        SHR_IF_ERR_EXIT
            (bcmpkt_free(unit_bcmpkt, packet));
        packet = NULL;
    }

exit:
    if (packet != NULL) {
        (void)bcmpkt_free(unit_bcmpkt, packet);
    }

    SHR_FUNC_EXIT();
}

static void
cmicx_fifodma_handler(shr_thread_t tc, void *vp)
{
    cmicx_fifodma_test_param_t *p = (cmicx_fifodma_test_param_t *)vp;
    int unit = p->unit;
    uint32_t *buf, timeout, ptr, num;
    uint32_t i;
    int rv;

    buf = p->work.buf_cache;
    for (i = 0; i < p->data.num_entries * p->data.data_width; i++) {
        buf[i] = FIFODMA_DIRTY_DATA;
    }
    timeout = p->intrmode ? 0 : p->ctrl_interval;

    while (1) {
        if (p->intrmode) {
            shr_thread_sleep(tc, p->ctrl_interval);
        }
        if (shr_thread_stopping(tc)) {
            break;
        }

        rv = bcmbd_fifodma_pop(unit, p->fifo_chan, timeout, &ptr, &num);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_EMPTY) {
                continue;
            } else {
                break;
            }
        }
        cmicx_fifodma_data_process(p, buf + ptr * p->data.data_width, ptr, num);
        if (p->debug) {
            cli_out("Valid entries %"PRIu32"\n", num);
        }
    }
}

static int
cmicx_fifodma_work_create(int unit, cmicx_fifodma_test_param_t *p)
{
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sym_info_t sinfo = {0};
    int blktype;
    struct mem_info_s {
        uint32_t acctype;
        uint32_t offset;
        int index;
        int blknum;
        int lane;
    } mi = {0};

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get_by_name(unit, p->sname, &sinfo));

    if (!(sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Initialize FIFODMA work */
    blktype = bcmdrd_pt_blktype_get(unit, sinfo.sid, 0);
    mi.acctype = BCMBD_CMICX_BLKACC2ACCTYPE(sinfo.blktypes);
    mi.offset = sinfo.offset;
    mi.index = sinfo.index_min;
    mi.blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);
    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        mi.lane = -1;
    }

    p->data.start_addr = bcmbd_block_port_addr(unit, mi.blknum, mi.lane,
                                               mi.offset, mi.index);
    p->data.data_width = sinfo.entry_wsize;
    p->data.op_code = cmicx_schan_opcode_set(unit, FIFO_POP_CMD_MSG,
                                             mi.blknum, mi.acctype,
                                             sinfo.entry_wsize * sizeof(uint32_t),
                                             0, 0);
    p->data.num_entries = p->buf_entries;
    p->data.thresh_entries = p->data.num_entries / 10;

    p->work.data = &p->data;
    p->work.cb = cmicx_fifodma_intr_cb;
    p->work.cb_data = p;
    p->work.flags = p->intrmode ? FIFODMA_WF_INT_MODE : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fifodma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    cmicx_fifodma_test_param_t *p = NULL;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicxFIFODMAPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->sname = sal_strdup("CENTRAL_CTR_EVICTION_FIFOm");
    p->intrmode = 0;
    p->debug = 0;

    p->unit = unit;
    p->ctrl_interval = 100000;
    p->buf_entries = 8192;
    p->fifo_chan = 0;
    p->test_result = SHR_E_FAIL;
    p->pkt_cnt = PACKET_NUM;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "IntrMode", "bool",
                             &p->intrmode, NULL);
    bcma_cli_parse_table_add(&pt, "Debug", "bool",
                             &p->debug, NULL);
    bcma_cli_parse_table_add(&pt, "PERF", "bool",
                             &p->perf, NULL);
    bcma_cli_parse_table_add(&pt, "PacKeT", "int",
                             &p->pkt_cnt, NULL);

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

    /* Create the FIFODMA work */
    SHR_IF_ERR_EXIT
        (cmicx_fifodma_work_create(unit, p));

exit:
    SHR_FUNC_EXIT();
}

static void
cmicx_fifodma_help(int unit, void *bp)
{
    cmicx_fifodma_test_param_t *p = (cmicx_fifodma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  IntrMode         - Interrupt Mode[0: Polling, 1: Intr]\n");
    cli_out("  Debug            - Debug Mode[0: False, 1: True]\n");
    cli_out("  ======================================================\n");
    cli_out("  sname            =%s\n", p->sname ? p->sname : "(null)");
    cli_out("  intrmode         =%"PRId32"\n", p->intrmode);
    cli_out("  debug            =%"PRId32"\n", p->debug);
    cli_out("  buf_entries      =%"PRId32"\n", p->buf_entries);
    cli_out("  fifo_chan        =%"PRId32"\n", p->fifo_chan);
    cli_out("  start_addr       =0x%"PRIx32"\n", p->data.start_addr);
    cli_out("  data_width       =%"PRId32"\n", p->data.data_width);
    cli_out("  opcode           =0x%"PRIx32"\n", p->data.op_code);
    cli_out("  num_entries      =%"PRId32"\n", p->data.num_entries);
    cli_out("  thresh_entries   =%"PRId32"\n", p->data.thresh_entries);

    return;
}

static void
cmicx_fifodma_recycle(int unit, void *bp)
{
    cmicx_fifodma_test_param_t *p = (cmicx_fifodma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    if (p->sname) {
        SHR_FREE(p->sname);
    }
    return;
}

static int
cmicx_fifodma_forward_path_set_cb(int unit, void *bp, uint32_t opcode)
{
    bcmdrd_pbmp_t pbmp, ubmp;

    SHR_FUNC_ENTER(unit);

    /* Create a vlan */
    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_create(unit, PACKET_VID, 1));

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, ING_PORT);
    BCMDRD_PBMP_PORT_ADD(pbmp, EGR_PORT);

    /* Set port mac loopback */
    SHR_IF_ERR_EXIT
        (bcma_testutil_port_setup(unit, LB_TYPE_MAC ,pbmp));

    /* Add ports into the vlan */
    BCMDRD_PBMP_PORT_ADD(pbmp, 0);
    BCMDRD_PBMP_CLEAR(ubmp);
    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_port_add(unit, PACKET_VID, pbmp, ubmp, TRUE));
    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_stp_set(unit, PACKET_VID, pbmp,
                                    BCMA_TESTUTIL_STP_FORWARD));

    /* Add a l2 unicast address */
    SHR_IF_ERR_EXIT
        (bcma_testutil_l2_unicast_add(unit, EGR_PORT,
                                      default_dst_mac, PACKET_VID, 0));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_fifodma_perf(int unit, cmicx_fifodma_test_param_t *p)
{
    sal_usecs_t start, end;
    uint64_t data_bits = 0, time_diff;
    uint32_t ptr, valid_entries, total_entries = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * send packets to trigger flex counter to update counter eviction FIFO.
     */

    SHR_IF_ERR_EXIT
        (bcmptm_cci_evict_fifo_enable(unit, 1));

    SHR_IF_ERR_EXIT
        (cmicx_fifodma_ctr_evict_trigger(unit, p->pkt_cnt));

    /*
     * prepare host memory and enable FIFO DMA and then pop data until valid
     * entries become empty.
     * record start/end time and calculate the time counsuming.
     */

    start = sal_time_usecs();
    SHR_IF_ERR_EXIT
        (bcmbd_fifodma_start(unit, p->fifo_chan, &p->work));

    while (1) {
        rv = bcmbd_fifodma_pop(unit, p->fifo_chan, 0, &ptr, &valid_entries);
        if (rv == SHR_E_NONE) {
            total_entries += valid_entries;
            if (p->debug) {
                cli_out("POP %d entries.\n", valid_entries);
            }
            continue;
        } else {
            if (rv == SHR_E_EMPTY) {
                if (p->debug) {
                    cli_out("FIFO DMA become empty.\n");
                }
                break;
            } else {
                break;
            }
        }
    }

    (void)bcmbd_fifodma_stop(unit, p->fifo_chan);
    end = sal_time_usecs();

    /* show data. */
    data_bits = BCMDRD_WORDS2BITS(p->data.data_width) * total_entries;
    time_diff = SAL_USECS_SUB(end, start);
    cli_out("FIFO DMA pop: %d entries (%"PRIu64" bits) in %"PRIu64" usec, ",
            total_entries, data_bits, time_diff);
    data_bits *= SECOND_USEC;
    bcma_testutil_stat_show(data_bits, time_diff, true);
    cli_out(" b/sec\n");

exit:
    (void)bcmptm_cci_evict_fifo_enable(unit, 0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief FIFODMA write, read and compare test.
 *
 * This function is intend to test FIFODMA operation
 * for CENTRAL_CTR_EVICTION_FIFO memory.
 *
 * \param [in] unit Device Number.
 * \param [in] bp Buffer pointer for test case parameters.
 * \param [in] option TDB options.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_FAIL Comparing fails.
 * \return SHR_E_xxx Other failures.
 */
static int
cmicx_fifodma_test(int unit, void *bp, uint32_t option)
{
    cmicx_fifodma_test_param_t *p = (cmicx_fifodma_test_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    if (p->perf) {
        SHR_VERBOSE_EXIT
            (cmicx_fifodma_perf(unit, p));
    }

    /* 0. Start FIFO DMA test work */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_evict_fifo_enable(unit, 1));
    SHR_IF_ERR_EXIT
        (bcmbd_fifodma_start(unit, p->fifo_chan, &p->work));

    /* 1. Send 100 packets to trigger Counter Eviction */
    SHR_IF_ERR_EXIT
        (cmicx_fifodma_ctr_evict_trigger(unit, p->pkt_cnt));

    /* 2. Start thread to read data from CENTRAL_CTR_EVICTION_FIFO */
    p->handler_tc = shr_thread_start("bcmaTestCaseCmicxFIFODMAHandler", -1,
                                     cmicx_fifodma_handler, (void *)p);
    if (p->handler_tc == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "FIFODMA handler thread cannot start.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_usleep(p->ctrl_interval * 20);

    /* 3. Check the test result */
    SHR_IF_ERR_EXIT
        (p->test_result);

exit:
    /* 4. Terminate handler thread */
    if (p->handler_tc) {
        if (SHR_FAILURE(shr_thread_stop(p->handler_tc, 500000))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "FIFODMA handler thread cannot exit.\n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* 5. Cleanup FIFO DMA work */
    (void)bcmbd_fifodma_stop(unit, p->fifo_chan);
    (void)bcmptm_cci_evict_fifo_enable(unit, 0);

    SHR_FUNC_EXIT();
}

static int
cmicx_fifodma_forward_path_cleanup_cb(int unit, void *bp, uint32_t option)
{
    bcmdrd_pbmp_t pbmp, ubmp;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_PORT_ADD(pbmp, ING_PORT);
    BCMDRD_PBMP_PORT_ADD(pbmp, EGR_PORT);

    SHR_IF_ERR_EXIT
        (bcma_testutil_port_clear(unit, LB_TYPE_MAC, pbmp, TRUE));

    SHR_IF_ERR_EXIT
        (bcma_testutil_l2_unicast_delete(unit, EGR_PORT,
                                         default_dst_mac, PACKET_VID, 0));

    BCMDRD_PBMP_PORT_ADD(pbmp, 0);
    BCMDRD_PBMP_CLEAR(ubmp);

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_port_remove(unit, PACKET_VID, pbmp));

    SHR_IF_ERR_EXIT
        (bcma_testutil_vlan_destroy(unit, PACKET_VID));
exit:
    SHR_FUNC_EXIT();
}

#define BCMA_TEST_PROC_DESCRIPTION \
    "Execute the FIFO DMA test.\n" \
    "    0). Stop fifo work in cci and start fifodma test work.\n" \
    "    1). Send 100 packets to trigger Counter Eviction.\n" \
    "    2). Start a handler thread to process FIFO DMA data of CENTRAL_CTR_EVICTION_FIFO.\n" \
    "    3). Record the test result.\n" \
    "    4). Terminate handler thread.\n" \
    "    5). Cleanup FIFO DMA work.\n" \
    ""

static bcma_test_proc_t cmicx_fifodma_proc[] = {
    {
        .desc = "cleanup packet I/O device\n",
        .cb = bcma_testcase_pkt_dev_cleanup_cb,
    },
    {
        .desc = "init packet I/O device\n",
        .cb = bcma_testcase_pkt_dev_init_cb,
    },
    {
        .desc = "create a l2 forwarding path\n",
        .cb = cmicx_fifodma_forward_path_set_cb,
    },
    {
        .desc = "configure counter eviction\n",
        .cb = cmicx_fifodma_ctr_evict_setup_cb,
    },
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = cmicx_fifodma_test,
    },
    {
        .desc = "cleanup counter eviction\n",
        .cb = cmicx_fifodma_ctr_evict_cleanup_cb,
    },
    {
        .desc = "cleanup l2 forward and vlan configuration\n",
        .cb = cmicx_fifodma_forward_path_cleanup_cb,
    }
};

static bcma_test_op_t cmicx_fifodma_op = {
    .select = cmicx_fifodma_select,
    .parser = cmicx_fifodma_parser,
    .help = cmicx_fifodma_help,
    .recycle = cmicx_fifodma_recycle,
    .procs = cmicx_fifodma_proc,
    .proc_count = COUNTOF(cmicx_fifodma_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_fifodma_op_get(void)
{
    return &cmicx_fifodma_op;
}
