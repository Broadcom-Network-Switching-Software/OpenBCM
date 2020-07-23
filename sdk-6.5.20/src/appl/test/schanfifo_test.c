/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SCHAN FIFO tests
 *
 * Test Limitations
 * MemTableID: 0->L3_DEFIP (default), 1->L3_DEFIP_ALPM_RAW, 2->L3_TUNNEL.
 * NumRead: Number of read operations (200 by default).
 * NumWrite: Number of write operations (200 by default).
 * NumRun: Number of total run times of the test (1 by default)
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <stdarg.h>

#include <sal/types.h>
#include <soc/mem.h>
#include <soc/register.h>
#include <soc/cm.h>

#include <sal/appl/pci.h>
#include <sal/appl/sal.h>
#include <sal/core/boot.h>
#include <sal/core/time.h>

#include <soc/l2x.h>
#include <soc/l3x.h>
#include <soc/phyctrl.h>
#include <soc/phy.h>
#include <soc/error.h>
#include <soc/cmicx.h>
#include <soc/soc_schan_fifo.h>

#include <bcm/error.h>
#include <bcm/link.h>

#include <bcm_int/esw/mbcm.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/progress.h>

#include "testlist.h"

#define SCHANFIFO_SOC_CMD_BUFF_DEPTH 352

#define MAX_NUM_OF_ASYNC_TRANSACTIONS 20

#define ASYNC_EN_PARAM_DEFAULT 0
#define MEM_TABLE_ID_PARAM_DEFAULT 0
#define NUM_READ_PARAM_DEFAULT 300
#define NUM_WRITE_PARAM_DEFAULT 300
#define NUM_RUN_PARAM_DEFAULT 1

/* Decimal point formatting */
#define INT_FRAC_2PT(x) (x) / 100, (x) % 100

typedef struct benchmark_s {
    uint32                      param_testMask;
    uint32                      param_asyncEn;
    uint32                      param_memTableID;
    uint32                      param_numRead;
    uint32                      param_numWrite;
    uint32                      param_numRun;
    uint32                      param_debug;
    int                         test_fail;
    soc_reg_t                   test_reg;
    soc_mem_t                   test_mem;
    int                         testNum;
    int                         sizeMem;
    char                        *bufMem;
    int                         sizeDMA;
    char                        *bufDMA;
    char                        *testName;
    sal_usecs_t                 stime, etime;
    sal_usecs_t                 time_drv;
    uint32                      dataSize;
    uint32                      dataWidth;
    char                        *dataUnit;
    uint32                      *schanfifoCmdBuff;
    schan_fifo_cmd_t            *schanfifoCmd;
    schan_fifo_resp_t           schanfifoResp;
    soc_schan_fifo_msg_t        schanfifoMsg;
    uint32                      async_en;
    volatile uint32             async_msg_send_cnt;
    volatile uint32             async_msg_done_cnt;
    volatile sal_usecs_t        async_stime[MAX_NUM_OF_ASYNC_TRANSACTIONS];
    volatile sal_usecs_t        async_etime[MAX_NUM_OF_ASYNC_TRANSACTIONS];
} benchmark_t;

#define BENCH_BUFMEM_SIZE       0x100000
#define BENCH_BUFDMA_SIZE       0x200000

void
schanfifo_async_wait_for_done(int unit, benchmark_t *b)
{
    int timeout = 5000;
    while (timeout-- && b->async_msg_send_cnt > b->async_msg_done_cnt) {
        sal_usleep(1000);
    }
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                "[async_wait] async_msg_send_cnt %0d "
                "async_msg_done_cnt %0d (timeout %0d)\n"),
                b->async_msg_send_cnt, b->async_msg_done_cnt, timeout));
}

int
schanfifo_test_cleanup(int unit, void *pa)
{
    int rv = BCM_E_NONE;
    benchmark_t *b = (benchmark_t *) pa;

    if (b) {
        if (b->bufMem)
            sal_free(b->bufMem);
        if (b->bufDMA)
            soc_cm_sfree(unit, b->bufDMA);
        if (b->schanfifoCmdBuff) {
            sal_dma_free(b->schanfifoCmdBuff);
        }

        sal_free(b);
    }

    return rv;
}

int
schanfifo_test_init(int unit, args_t *a, void **pa)
{
    benchmark_t         *b = 0;
    int                 rv = BCM_E_NONE;
    parse_table_t       pt;
    int                 i;

    const char schanfifo_test_usage[] =
    "Configuration parameters passed from CLI:\n"
#ifdef BCM_SOC_ASYNC_SUPPORT
    "AsyncEn   : 0->Synchronous (default), 1->Asynchronous \n"
#endif
    "MemTableID: Memory table selectin\n"
    "                   0->L3_DEFIPm.\n"
    "                   1->L3_DEFIP_ALPM_RAWm.\n"
    "                   2->L3_TUNNELm.\n"
    "                   3->L3_DEFIP_ALPM_IPV6_128m.\n"
    "                   4->L3_DEFIP_ALPM_IPV4m.\n"
    "NumRead   : Number of read operations (200 by default).\n"
    "NumWrite  : Number of write operations (200 by default).\n"
    "NumRun    : Number of total run times of the test(1 by default).\n";

    *pa = NULL;

    /* benchmark_t */
    if ((b = sal_alloc(sizeof(benchmark_t), "benchmark")) == 0) {
        cli_out("Failed to allocate mem for benchmark\n");
        rv = BCM_E_FAIL;
        goto done;
    }
    memset(b, 0, sizeof(*b));
    *pa = (void *) b;
    b->test_fail = 0;
    b->testNum = 0;
    b->sizeMem = BENCH_BUFMEM_SIZE;
    b->sizeDMA = BENCH_BUFDMA_SIZE;
    b->async_msg_send_cnt = 0;
    b->async_msg_done_cnt = 0;
    b->async_en = 0;
    for (i = 0; i < MAX_NUM_OF_ASYNC_TRANSACTIONS; i++) {
        b->async_stime[i] = 0;
        b->async_etime[i] = 0;
    }

    if ((b->bufMem = sal_alloc(b->sizeMem, "bufMem")) == 0) {
        cli_out("Failed to allocate mem for bufMem\n");
        rv = BCM_E_FAIL;
        goto done;
    }

    if ((b->bufDMA = soc_cm_salloc(unit, b->sizeDMA, "bufDMA")) == 0) {
        cli_out("Failed to allocate mem for bufDMA\n");
        rv = BCM_E_FAIL;
        goto done;
    }
    /* schan_fifo_cmd_t */
    b->schanfifoCmdBuff = sal_dma_alloc(SCHANFIFO_SOC_CMD_BUFF_DEPTH *
                                sizeof(uint32), "schanfifoCmdBuff");
    if (b->schanfifoCmdBuff == 0) {
        cli_out("Failed to allocate mem for schanfifoCmdBuff\n");
        rv = BCM_E_FAIL;
        goto done;
    }
    b->schanfifoCmd = (schan_fifo_cmd_t *) b->schanfifoCmdBuff;

    /* soc_schan_fifo_msg_t */
    b->schanfifoMsg.num = 0;
    b->schanfifoMsg.interval = -1;
    b->schanfifoMsg.size = 0;
    b->schanfifoMsg.cmd = b->schanfifoCmd;

    /* CLI parameters */
    b->param_testMask = ~0;
    b->param_asyncEn = ASYNC_EN_PARAM_DEFAULT;
    b->param_memTableID = MEM_TABLE_ID_PARAM_DEFAULT;
    b->param_numRead = NUM_READ_PARAM_DEFAULT;
    b->param_numWrite = NUM_WRITE_PARAM_DEFAULT;
    b->param_numRun = NUM_RUN_PARAM_DEFAULT;
    b->param_debug = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "TestMask", PQ_HEX|PQ_DFL, 0,
                    &b->param_testMask, NULL);
#ifdef BCM_SOC_ASYNC_SUPPORT
    parse_table_add(&pt, "AsyncEn", PQ_BOOL|PQ_DFL, 0,
                    &b->param_asyncEn, NULL);
#endif
    parse_table_add(&pt, "MemTableID", PQ_HEX|PQ_DFL, 0,
                    &b->param_memTableID, NULL);
    parse_table_add(&pt, "NumRead", PQ_HEX|PQ_DFL, 0,
                    &b->param_numRead, NULL);
    parse_table_add(&pt, "NumWrite", PQ_HEX|PQ_DFL, 0,
                    &b->param_numWrite, NULL);
    parse_table_add(&pt, "NumRun", PQ_HEX|PQ_DFL, 0,
                    &b->param_numRun, NULL);
    parse_table_add(&pt, "Debug", PQ_INT|PQ_DFL, 0,
                    &b->param_debug, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", schanfifo_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        rv = BCM_E_FAIL;
        goto done;
    } else {
        cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
        cli_out("\nparam_testMask   = %0d", b->param_testMask);
        cli_out("\nparam_asyncEn    = %0d", b->param_asyncEn);
        cli_out("\nparam_memTableID = %0d", b->param_memTableID);
        cli_out("\nparam_numRead    = %0d", b->param_numRead);
        cli_out("\nparam_numWrite   = %0d", b->param_numWrite);
        cli_out("\nparam_numRun     = %0d", b->param_numRun);
        cli_out("\nparam_debug      = %0d", b->param_debug);
        cli_out("\n -----------------------------------------------------\n");
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
        /* reg */
        b->test_reg = ECMP_CONFIGr; /* EGR_CONFIGr; */
        if(!SOC_REG_IS_VALID(unit, b->test_reg)) {
            cli_out("Invalid reg %s\n", SOC_REG_NAME(unit, b->test_reg));
            rv = BCM_E_FAIL;
        } else {
            cli_out("\nName of test reg: %s\n", SOC_REG_NAME(unit,
                                                             b->test_reg));
        }
        /* mem */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            switch (b->param_memTableID) {
                case 0: b->test_mem = L3_DEFIP_ALPM_LEVEL3m;   break;
                case 1: b->test_mem = L3_DEFIP_ALPM_LEVEL2m;   break;
                case 2: b->test_mem = L3_DEFIP_LEVEL1m;        break;
                case 3: b->test_mem = L3_DEFIP_PAIR_LEVEL1m;   break;
                case 4: b->test_mem = L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm; break;
                case 5: b->test_mem = L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm; break;
                case 6: b->test_mem = L3_DEFIP_ALPM_LEVEL3m;   break;
                default:b->test_mem = L3_DEFIP_ALPM_LEVEL3m;   break;
            }
        } else
#endif
            switch (b->param_memTableID) {
                case 0: b->test_mem = L3_DEFIPm;               break;
                case 1: b->test_mem = L3_DEFIP_ALPM_RAWm;      break;
                case 2: b->test_mem = L3_TUNNELm;              break;
                case 3: b->test_mem = L3_DEFIP_ALPM_IPV6_128m; break;
                case 4: b->test_mem = L3_DEFIP_ALPM_IPV4m;     break;
                case 5: b->test_mem = L2_ENTRY_ECCm;           break;
                case 6: b->test_mem = INITIAL_ING_L3_NEXT_HOPm;break;
                default:b->test_mem = L3_DEFIPm;               break;
        }
        if(!SOC_MEM_IS_VALID(unit, b->test_mem)) {
            cli_out("Invalid mem %s\n", SOC_MEM_NAME(unit, b->test_mem));
            rv = BCM_E_FAIL;
        } else if (!SOC_MEM_IS_ENABLED(unit, b->test_mem)) {
            cli_out("Cannot test mem %s:  No entries.\n",
                    SOC_MEM_NAME(unit, b->test_mem));
            rv = BCM_E_FAIL;
        } else {
            cli_out("\nName of test mem: %s\n",
                    SOC_MEM_NAME(unit, b->test_mem));
        }
#endif
        /* numRun */
        if (b->param_numRun < 1) {
            b->param_numRun = 1;
        }

        parse_arg_eq_done(&pt);
    }

done:
    if (BCM_FAILURE(rv)) {
        /* benchmark_done(unit, *pa); */
        if (b) {
            b->test_fail = 1;
        }
    }

    return rv;
}

static int
benchmark_begin(int unit,
                benchmark_t *b,
                char *testName,
                char *dataUnit,
                uint32 dataSize,
                uint32 dataWidth)
{
    assert(b->testName == 0);

    if ((b->param_testMask & (1U << b->testNum)) == 0) {
        return 0;
    }

    b->testName  = testName;
    b->dataUnit  = dataUnit;
    b->dataWidth = dataWidth;
    b->dataSize  = dataSize;
    b->stime = sal_time_usecs();
    b->time_drv = 0;
    b->async_msg_send_cnt = 0;
    b->async_msg_done_cnt = 0;

    return 1;
}

static void
benchmark_end(int unit, benchmark_t *b)
{
    if (b->testName) {
        uint32          per_sec, usec_per, kb_per_sec;
        sal_usecs_t     td = 0;
        char            plus = ' ', lessthan = ' ';
        int             i;
        sal_usecs_t     async_stime, async_etime;
        uint32          async_cnt = 0;

        if (b->async_en == 1) {
            /* wait for async callback done */
            schanfifo_async_wait_for_done(unit, b);
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "[benchmark_end] async_msg_send_cnt %0d\n"),
                        b->async_msg_send_cnt));
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "[benchmark_end] async_msg_done_cnt %0d\n"),
                        b->async_msg_done_cnt));
            /* print verbose info of api time cost */
            for (i = 0; i < MAX_NUM_OF_ASYNC_TRANSACTIONS; i++) {
                if (b->async_stime[i] > 0 && b->async_etime[i] > 0) {
                    td = SAL_USECS_SUB(b->async_etime[i], b->async_stime[i]);
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                                "[benchmark_end] msg %2d, stime %0d, "
                                "etime %0d, driver_time %8d\n"),
                                i, (int) b->async_stime[i],
                                (int) b->async_etime[i], (int)td));
                }
            }

            /* recalculate driver time */
            async_cnt = (b->async_msg_done_cnt > 0) ? (b->async_msg_done_cnt) : 1;
            async_stime = b->async_stime[0];
            async_etime = b->async_etime[async_cnt - 1];
            td = SAL_USECS_SUB(async_etime, async_stime);
            b->time_drv = td;
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "[benchmark_end] stime[%d] %0d, etime[%d] %0d, "
                        "driver_time %8d, data Size = %u\n"),
                        0, (int)async_stime,
                        async_cnt-1, async_etime, (int)td, b->dataSize * b->dataWidth));

            td = b->time_drv;
        } else {
            b->etime = sal_time_usecs();
            td = SAL_USECS_SUB(b->etime, b->stime);
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "[benchmark_end] stime-%d, etime-%d,"
                        "driver_time %8d, data Size = %u\n"),
                        (int)b->stime, b->etime, (int)td, b->dataSize * b->dataWidth));
        }

        if (td == 0) {
            td = 1; /* Non-zero value */
            plus = '+';
            lessthan = '<';
        }
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "driver_time_ratio %d%s\n"),
                  (int) (b->time_drv*100/td), "%"));
        per_sec = _shr_div_exp10(b->dataSize, td, 8);
        usec_per = b->dataSize ? _shr_div_exp10(td, b->dataSize, 2) : 0;
        kb_per_sec = _shr_div_exp10(b->dataSize * b->dataWidth, td, 8);

        cli_out("%2d) %-28s%7d.%02d%c %5s/sec  ; %c%7d.%02d usec/%-5s  ; %7d.%02d byte/sec\n",
                b->testNum,
                b->testName,
                INT_FRAC_2PT(per_sec),
                plus,
                b->dataUnit,
                lessthan,
                INT_FRAC_2PT(usec_per),
                b->dataUnit,
                INT_FRAC_2PT(kb_per_sec));

        b->testName = 0;
    }

    b->testNum++;
}

int
schan_fifo_flush(int unit, benchmark_t *b)
{
    int rv = BCM_E_NONE;
    sal_usecs_t stime, etime;

    if (b->schanfifoMsg.num > 0) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "schan_fifo_flush: Calling "
                             "soc_schan_fifo_msg_send with %0d commands\n\n"),
                             b->schanfifoMsg.num));
        stime = sal_time_usecs();
        rv = soc_schan_fifo_msg_send(unit, &b->schanfifoMsg, NULL, NULL);
        etime = sal_time_usecs();
        b->time_drv = SAL_USECS_ADD(b->time_drv, SAL_USECS_SUB(etime, stime));
        if (BCM_FAILURE(rv)) {
            sal_printf("soc_schan_fifo_msg_send: %s\n", bcm_errmsg(rv));
        }
        b->schanfifoMsg.num = 0;
        b->schanfifoMsg.size = 0;
    }

    return rv;
}

int
schan_fifo_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data, benchmark_t *b)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    schan_fifo_cmd_t *schan_fifo_cmd;
    uint32 addr;
    int block = 0;
    int numword = 2;
    uint8 acc_type;
    int data_byte_len = 4;

    if (b->schanfifoMsg.size + numword < SCHANFIFO_SOC_CMD_BUFF_DEPTH) {
        schan_fifo_cmd = (schan_fifo_cmd_t *) &b->schanfifoCmdBuff[b->schanfifoMsg.num * numword];
        addr = soc_reg_addr_get(unit, reg, port, index,
                                SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
        soc_schan_header_cmd_set(unit, &schan_fifo_cmd->header, READ_REGISTER_CMD_MSG,
                                 block, 0, acc_type, data_byte_len, 0, 0);
        schan_fifo_cmd->address = addr;
        b->schanfifoMsg.num++;
        b->schanfifoMsg.size += numword;

        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "add cmd fifo_reg32_get, msg_num %d, "
                                   "msg_size %d, num_words %d, "
                                   "cmdBuff_size %d\n"),
                    b->schanfifoMsg.num, (int) b->schanfifoMsg.size,
                    numword, b->schanfifoMsg.num * numword));
    } else {
        rv = schan_fifo_flush(unit, b);
    }
#else
    rv = soc_reg32_get(unit, reg, port, index, data);
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */
    return rv;
}

int
schan_fifo_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data, benchmark_t *b)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    schan_fifo_cmd_t *schan_fifo_cmd;
    uint32 addr;
    int block = 0;
    int numword = 3;
    uint8 acc_type;
    int data_byte_len = 4; /* sizeof(uint32) */

    if (b->schanfifoMsg.size + numword < SCHANFIFO_SOC_CMD_BUFF_DEPTH) {
        schan_fifo_cmd = (schan_fifo_cmd_t *) &b->schanfifoCmdBuff[b->schanfifoMsg.num * numword];
        addr = soc_reg_addr_get(unit, reg, port, index,
                                SOC_REG_ADDR_OPTION_WRITE, &block, &acc_type);
        soc_schan_header_cmd_set(unit, &schan_fifo_cmd->header, WRITE_REGISTER_CMD_MSG,
                                 block, 0, acc_type, data_byte_len, 0, 0);
        schan_fifo_cmd->address = addr;
        schan_fifo_cmd->data[0] = data;
        b->schanfifoMsg.num++;
        b->schanfifoMsg.size += numword;

        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "add cmd fifo_reg32_set, msg_num %d, "
                                   "msg_size %d, num_words %d, "
                                   "cmdBuff_size %d\n"),
                    b->schanfifoMsg.num, (int) b->schanfifoMsg.size,
                    numword, b->schanfifoMsg.num * numword));
    } else {
        rv = schan_fifo_flush(unit, b);
    }
#else
    rv = soc_reg32_set(unit, reg, port, index, data);
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */
    return rv;
}

int
schan_fifo_mem_read(int unit, soc_mem_t mem, int copyno, int index, void *entry_data, benchmark_t *b)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    schan_fifo_cmd_t *schan_fifo_cmd;
    int numword = 2;
    int dst_blk, src_blk, acc_type;
    uint32 addr;
    uint8 at;

    if (b->schanfifoMsg.size + numword < SCHANFIFO_SOC_CMD_BUFF_DEPTH) {
        schan_fifo_cmd = (schan_fifo_cmd_t *) &b->schanfifoCmdBuff[b->schanfifoMsg.num * numword];
        addr = soc_mem_addr_get(unit, mem, 0, copyno, index, &at);
        dst_blk = SOC_BLOCK2SCH(unit, copyno);
        src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        soc_schan_header_cmd_set(unit, &schan_fifo_cmd->header, READ_MEMORY_CMD_MSG,
                                 dst_blk, src_blk, acc_type, 4, 0, 0);
        schan_fifo_cmd->address = addr;
        b->schanfifoMsg.num++;
        b->schanfifoMsg.size += numword;

        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "add cmd fifo_mem_read, msg_num %d, "
                                   "msg_size %d, num_words %d, "
                                   "cmdBuff_size %d\n"),
                    b->schanfifoMsg.num, (int) b->schanfifoMsg.size,
                    numword, b->schanfifoMsg.num * numword));
    } else {
        rv = schan_fifo_flush(unit, b);
    }
#else
    rv = soc_mem_read(unit, mem, copyno, index, entry_data);
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */
    return rv;
}

int
schan_fifo_mem_write(int unit, soc_mem_t mem, int copyno, int index, void *entry_data, benchmark_t *b)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    schan_msg_t schan_msg;
    int dst_blk, src_blk, acc_type;
    int cmd_dw = 2;
    int entry_dw;
    int data_byte_len;
    uint32 maddr;
    uint8 at;

    entry_dw = soc_mem_entry_words(unit, mem);
    data_byte_len = entry_dw * sizeof (uint32);

    if (b->schanfifoMsg.size + cmd_dw + entry_dw < SCHANFIFO_SOC_CMD_BUFF_DEPTH) {
        schan_msg_clear(&schan_msg);
        maddr = soc_mem_addr_get(unit, mem, 0, copyno, index, &at);
        schan_msg.readcmd.address = maddr;
        dst_blk = SOC_BLOCK2SCH(unit, copyno);
        src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_MEMORY_CMD_MSG,
                                 dst_blk, src_blk, acc_type, data_byte_len, 0, 0);

        sal_memcpy(&b->schanfifoCmdBuff[b->schanfifoMsg.num * (cmd_dw + entry_dw)],
                   &schan_msg, sizeof(uint32) * cmd_dw);
        sal_memcpy(schan_msg.writecmd.data, entry_data, entry_dw * sizeof(uint32));
        b->schanfifoMsg.num++;
        b->schanfifoMsg.size += cmd_dw + entry_dw;

        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "add cmd fifo_mem_write, msg_num %d, "
                                   "msg_size %d, num_words %d, "
                                   "cmdBuff_size %d\n"),
                    b->schanfifoMsg.num, (int) b->schanfifoMsg.size,
                    cmd_dw + entry_dw, b->schanfifoMsg.num * (cmd_dw + entry_dw)));
    } else {
        rv = schan_fifo_flush(unit, b);
    }

#else
    rv = soc_mem_write(unit, mem, copyno, index, entry_data);
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWK3_SUPPORT */

    return rv;
}

static void
schanfifo_async_cb(int unit, void *data, void *cookie, int status)
{
    soc_schan_fifo_msg_t *msg = (soc_schan_fifo_msg_t *) data;
    benchmark_t *b = (benchmark_t*) cookie;
    int done_cnt = 0;

    if (b != NULL) {
        done_cnt = b->async_msg_done_cnt;
        if (done_cnt < MAX_NUM_OF_ASYNC_TRANSACTIONS) {
            b->async_etime[done_cnt] = sal_time_usecs();
        }

        b->async_msg_done_cnt++;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "[callback] async_msg_done_cnt %0d\n"),
                    b->async_msg_done_cnt));
    }

    if (SOC_SUCCESS(status)) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                  "SUCCESS: %s:unit= %d, msg_num= %d\n "),
                  __func__, unit, msg->num));
    } else {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "FAILURE: %s:unit= %d, msg_num= %d\n"),
                    __func__, unit, msg->num));
    }

	(void) soc_schan_fifo_control(unit, CTL_FIFO_RESP_FREE, msg->resp);
    sal_dma_free(msg->cmd);
    sal_free(msg);
}

int
schanfifo_reg_send(int unit, soc_reg_t reg, int op_code, uint32 op_num,
                   int port, int async_en, benchmark_t *b)
{
    int rv = BCM_E_NONE;
    uint32 cmd_num_limit = 176; /* CMIC_SCHAN_FIFO_CMD_SIZE_MAX; */
    uint32 cmd_num_max;
    char *str_op;
    int block = 0;
    int index = 0;
    uint8 acc_type;
    int entry_dw = 0;
    int cmd_dw = 2;
    int dw_per_msg;
    int data_bytes = 4;
    uint32 addr;
    uint32 data = 0x123;
    int try;
    uint32 *cmdBuff = NULL;
    soc_schan_fifo_msg_t *msg;
    schan_fifo_alloc_t resp_alloc;
    schan_fifo_cmd_t  *cmd_ptr;
    sal_usecs_t stime, etime;
    int send_cnt;

    /* op code */
    op_code = (op_code == WRITE_REGISTER_CMD_MSG) ? op_code :
              READ_REGISTER_CMD_MSG;
    if (op_code == READ_REGISTER_CMD_MSG) {
        str_op = (async_en == 0) ? "SYNC_READ" : "ASYNC_READ";
    } else {
        str_op = (async_en == 0) ? "SYNC_WRITE" : "ASYNC_WRITE";
    }

    cmd_num_max = (op_num < cmd_num_limit) ? op_num : cmd_num_limit;

    /* malloc: cmd buff */
    cmdBuff = sal_dma_alloc(sizeof(uint32) * SCHANFIFO_SOC_CMD_BUFF_DEPTH,
                            "SCHANFIFO CMD BUFF");
    if (cmdBuff == NULL) {
        return SOC_E_MEMORY;
    }

    /* malloc: msg */
    msg = sal_alloc(sizeof(soc_schan_fifo_msg_t), "SCHANFIFO MSG");
    if (msg == NULL) {
        /* cmdBuff */
        sal_dma_free(cmdBuff);
        return SOC_E_MEMORY;
    }
    msg->num      = 0;
    msg->interval = -1;
    msg->size     = 0;
    msg->cmd      = (schan_fifo_cmd_t *) cmdBuff;
    msg->resp     = NULL;

    /* malloc: resp */
    if (op_code == READ_REGISTER_CMD_MSG) {
        resp_alloc.num = cmd_num_max;
        rv = soc_schan_fifo_control(unit, CTL_FIFO_RESP_ALLOC, &resp_alloc);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        msg->resp = resp_alloc.resp;
    }

    /* get reg info */
    addr = soc_reg_addr_get(unit, reg, port, index,
                            SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);

    if (op_code == WRITE_REGISTER_CMD_MSG) {
        entry_dw = 1;
    }
    dw_per_msg = cmd_dw + entry_dw;

    /* fill cmd buff */
    while ((msg->num < cmd_num_max) &&
           (msg->size + dw_per_msg) <= SCHANFIFO_SOC_CMD_BUFF_DEPTH) {
        cmd_ptr = (schan_fifo_cmd_t *) &cmdBuff[msg->size];
        soc_schan_header_cmd_set(unit, &cmd_ptr->header, op_code,
                                 block, 0, acc_type, data_bytes, 0, 0);
        cmd_ptr->address = addr;
        cmd_ptr->data[0] = data;
        msg->num++;
        msg->size += dw_per_msg;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "add schanfifo cmd, msg_num %0d, "
                    "msg_size %0d, num_words %0d\n"),
                    msg->num, (int) msg->size, dw_per_msg));
    }

    /* kick off msg */
    stime = sal_time_usecs();
    if (async_en) {
        send_cnt = b->async_msg_send_cnt;
        if (send_cnt < MAX_NUM_OF_ASYNC_TRANSACTIONS) {
            b->async_stime[send_cnt] = stime;
        }
    }
    if (async_en == 0) { /* sync mode */
        rv = soc_schan_fifo_msg_send(unit, msg, NULL, NULL);
    } else { /* async mode */
        try = 5;
        while(try--) {
            rv = soc_schan_fifo_msg_send(unit, msg, (void *) b,
                                         schanfifo_async_cb);
            if (SOC_SUCCESS(rv)) {
                break;
            }
            /* queue is full, let wait before try again */
            sal_usleep(200);
        }
    }
    etime = sal_time_usecs();
    b->time_drv = SAL_USECS_ADD(b->time_drv, SAL_USECS_SUB(etime, stime));

    if (SOC_FAILURE(rv)) {
        cli_out("[%s] soc_schan_fifo_msg_send failed = %d\n", str_op, rv);
    }
    if (SOC_SUCCESS(rv) && async_en == 1) {
        b->async_msg_send_cnt++;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "[async] async_msg_send_cnt %0d\n"),
                    b->async_msg_send_cnt));
    }

    /* free allocated memory */
    if (SOC_FAILURE(rv) || async_en == 0) {
        /* cmdBuff */
        sal_dma_free(msg->cmd);
        /* resp_alloc->resp */
        (void) soc_schan_fifo_control(unit, CTL_FIFO_RESP_FREE, msg->resp);
        /* msg */
        sal_free(msg);
    }

    return rv;
}
int
schanfifo_reg_send_wrapper(int unit, soc_reg_t reg, int op_code, uint32 op_num,
                           int port, int async_en, benchmark_t *b)
{
    int rv = BCM_E_NONE;
    int dw_per_msg;
    int entry_dw = 0;
    int cmd_dw = 2;
    int op_num_curr, op_num_unassigned, op_num_assigned,
        cmd_num_increment;

    /* op_num_increment */
    if (op_code == WRITE_MEMORY_CMD_MSG) {
        entry_dw = 1;
    }
    dw_per_msg = cmd_dw + entry_dw;
    /* cmd_num_limit 176 */
    cmd_num_increment =  SCHANFIFO_SOC_CMD_BUFF_DEPTH / dw_per_msg;

    op_num_unassigned = op_num;
    op_num_assigned = 0;
    while (op_num_unassigned > 0) {
        op_num_curr = (op_num_unassigned > cmd_num_increment) ?
                       cmd_num_increment : op_num_unassigned;
        op_num_unassigned -= op_num_curr;
        op_num_assigned += op_num_curr;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "op_num_curr = %0d, op_num_unassigned = %0d, "
                    "op_num_assigned = %0d\n"),
                    op_num_curr, op_num_unassigned, op_num_assigned));
        rv = schanfifo_reg_send(unit, reg, op_code, op_num, port, async_en, b);
        if (BCM_FAILURE(rv))
            break;
    }

    return rv;
}

int
schanfifo_mem_send(int unit, soc_mem_t mem, int op_code, uint32 op_num,
                   int async_en, benchmark_t *b)
{
    int rv = BCM_E_NONE;
    uint32 cmd_num_limit = 176; /* CMIC_SCHAN_FIFO_CMD_SIZE_MAX; */
    uint32 cmd_num_max;
    char *str_op;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_block_t block;
    int index = 0;
    int dst_blk, src_blk, acc_type;
    int entry_dw = 0;
    int cmd_dw = 2;
    int dw_per_msg;
    int data_bytes = 4;
    uint32 addr;
    uint8 at;
    int try;
    uint32 *cmdBuff = NULL;
    soc_schan_fifo_msg_t *msg;
    schan_fifo_alloc_t resp_alloc;
    schan_fifo_cmd_t  *cmd_ptr;
    sal_usecs_t stime, etime;
    int send_cnt = 0;

    /* op code */
    op_code = (op_code == WRITE_MEMORY_CMD_MSG) ? op_code :
              READ_MEMORY_CMD_MSG;
    if (op_code == READ_MEMORY_CMD_MSG) {
        str_op = (async_en == 0) ? "SYNC_READ" : "ASYNC_READ";
    } else {
        str_op = (async_en == 0) ? "SYNC_WRITE" : "ASYNC_WRITE";
    }

    cmd_num_max = (op_num < cmd_num_limit) ? op_num : cmd_num_limit;
    sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    /* malloc: cmd buff */
    cmdBuff = sal_dma_alloc(sizeof(uint32) * SCHANFIFO_SOC_CMD_BUFF_DEPTH,
                            "SCHANFIFO CMD BUFF");
    if (cmdBuff == NULL) {
        return SOC_E_MEMORY;
    }

    /* malloc: msg */
    msg = sal_alloc(sizeof(soc_schan_fifo_msg_t), "SCHANFIFO MSG");
    if (msg == NULL) {
        /* cmdBuff */
        sal_dma_free(cmdBuff);
        return SOC_E_MEMORY;
    }
    msg->num      = 0;
    msg->interval = -1;
    msg->size     = 0;
    msg->cmd      = (schan_fifo_cmd_t *) cmdBuff;
    msg->resp     = NULL;

    /* malloc: resp */
    if (op_code == READ_MEMORY_CMD_MSG) {
        resp_alloc.num = cmd_num_max;
        rv = soc_schan_fifo_control(unit, CTL_FIFO_RESP_ALLOC, &resp_alloc);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        msg->resp = resp_alloc.resp;
    }

    /* get memory info */
    block = SOC_MEM_BLOCK_ANY(unit, mem);
    addr = soc_mem_addr_get(unit, mem, 0, block, index, &at);
    dst_blk = SOC_BLOCK2SCH(unit, block);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    acc_type = SOC_MEM_ACC_TYPE(unit, mem);

    if (op_code == WRITE_MEMORY_CMD_MSG) {
        entry_dw = soc_mem_entry_words(unit, mem);
        data_bytes = entry_dw * sizeof(uint32);
    }
    dw_per_msg = cmd_dw + entry_dw;

    /* fill cmd buff */
    while ((msg->num < cmd_num_max) &&
           (msg->size + dw_per_msg) <= SCHANFIFO_SOC_CMD_BUFF_DEPTH) {
        cmd_ptr = (schan_fifo_cmd_t *) &cmdBuff[msg->size];
        soc_schan_header_cmd_set(unit, &cmd_ptr->header, op_code, dst_blk,
                                 src_blk, acc_type, data_bytes, 0, 0);
        cmd_ptr->address = addr;
        msg->num++;
        msg->size += dw_per_msg;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "add schanfifo cmd, msg_num %0d, "
                    "msg_size %0d, num_words %0d\n"),
                    msg->num, (int) msg->size, dw_per_msg));
    }

    /* kick off msg */
    stime = sal_time_usecs();
    if (async_en) {
        send_cnt = b->async_msg_send_cnt;
        if (send_cnt < MAX_NUM_OF_ASYNC_TRANSACTIONS) {
            b->async_stime[send_cnt] = stime;
            /* cli_out("debug: async_stime[%d] %0d\n", send_cnt, b->async_stime[send_cnt]); */
        }
    }

    if (async_en == 0) { /* sync mode */
        rv = soc_schan_fifo_msg_send(unit, msg, NULL, NULL);
    } else { /* async mode */
        try = 5;
        while(try--) {
            rv = soc_schan_fifo_msg_send(unit, msg, (void *) b,
                                         schanfifo_async_cb);
            if (SOC_SUCCESS(rv)) {
                break;
            }
            /* queue is full, let wait before try again */
            sal_usleep(200);
        }
    }
    etime = sal_time_usecs();
    b->time_drv = SAL_USECS_ADD(b->time_drv, SAL_USECS_SUB(etime, stime));

    if (SOC_FAILURE(rv)) {
        cli_out("[%s] soc_schan_fifo_msg_send failed = %d\n", str_op, rv);
    }
    if (SOC_SUCCESS(rv) && async_en == 1) {
        b->async_msg_send_cnt++;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "[async] b->async_msg_send_cnt %0d\n"),
                    b->async_msg_send_cnt));
        /* cli_out("[async] b->async_msg_send_cnt %0d\n", b->async_msg_send_cnt); */
    }

    /* free allocated memory */
    if (SOC_FAILURE(rv) || async_en == 0) {
        /* cmdBuff */
        sal_dma_free(msg->cmd);
        /* resp_alloc->resp */
        (void) soc_schan_fifo_control(unit, CTL_FIFO_RESP_FREE, msg->resp);
        /* msg */
        sal_free(msg);
    }

    return rv;
}

int
schanfifo_mem_send_wrapper(int unit, soc_mem_t mem, int op_code, 
                           uint32 op_num, int async_en, benchmark_t *b)
{
    int rv = BCM_E_NONE;
    int dw_per_msg;
    int entry_dw = 0;
    int cmd_dw = 2;
    int op_num_curr, op_num_unassigned, op_num_assigned,
        cmd_num_increment;
    uint32 cmd_num_limit = 176; /* CMIC_SCHAN_FIFO_CMD_SIZE_MAX; */
    
    /* op_num_increment */
    if (op_code == WRITE_MEMORY_CMD_MSG) {
        entry_dw = soc_mem_entry_words(unit, mem);
    }
    dw_per_msg = cmd_dw + entry_dw;
    cmd_num_increment =  SCHANFIFO_SOC_CMD_BUFF_DEPTH / dw_per_msg;
    if (cmd_num_increment > cmd_num_limit) {
        cmd_num_increment = cmd_num_limit;
    }

    op_num_unassigned = op_num;
    op_num_assigned = 0;
    while (op_num_unassigned > 0) {
        op_num_curr = (op_num_unassigned > cmd_num_increment) ?
                       cmd_num_increment : op_num_unassigned;
        op_num_unassigned -= op_num_curr;
        op_num_assigned += op_num_curr;
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "op_num_curr = %0d, op_num_unassigned = %0d, "
                    "op_num_assigned = %0d\n"),
                    op_num_curr, op_num_unassigned, op_num_assigned));
        rv = schanfifo_mem_send(unit, mem, op_code, op_num_curr, async_en, b);
        if (BCM_FAILURE(rv))
            break;
    }

    return rv;
}

int
schanfifo_test(int unit, args_t *a, void *pa)
{
    int         rv = BCM_E_NONE;
    soc_mem_t   mem;
    soc_reg_t   reg;
    int         iter_cmic_read;
    int         iter_cmic_write;
    int         repeat_num;
    int         num_byte;
    int         quickturn, i, j;
    uint32      rval = 0;
    uint32      entry[SOC_MAX_MEM_WORDS];
    int         port = 0, phy_port = 0;
    int         blk = 0;
    int         entry_idx = 0;
    benchmark_t *b = (benchmark_t *) pa;

    if (b == NULL || b->test_fail == 1) {
        goto test_done;
    }

    quickturn = (SAL_BOOT_QUICKTURN != 0);

    if (quickturn) {
        iter_cmic_read = b->param_numRead;
        iter_cmic_write = b->param_numWrite;
    } else if (NULL != SOC_CONTROL(unit)->soc_rcpu_schan_op){
        iter_cmic_read = 5000;
        iter_cmic_write = 5000;
    } else {
        iter_cmic_read = 5000;
        iter_cmic_write = 5000;
    }
    repeat_num = b->param_numRun;
    cli_out ("\ntotal_num_of_read  = %0d", iter_cmic_read * repeat_num);
    cli_out ("\ntotal_num_of_write = %0d\n", iter_cmic_write * repeat_num);

    reg = b->test_reg;
    mem = b->test_mem;

    SOC_MEM_TEST_SKIP_CACHE_SET(unit, TRUE);

    if (SOC_IS_ESW(unit)) {
        /* find an appropriate port/block */
        if (SOC_PORT_NUM(unit, fe) > 0) {
            port = SOC_PORT(unit, fe, 0);
        } else if (SOC_PORT_NUM(unit, ge) > 0) {
            port = SOC_PORT(unit, ge, 0);
        } else if (SOC_PORT_NUM(unit, xe) > 0) {
            port = SOC_PORT(unit, xe, 0);
        } else {
            port = SOC_PORT(unit, hg, 0);
        }
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        blk = SOC_PORT_BLOCK(unit, phy_port);
    }

    num_byte = sizeof(uint32);
if (b->param_asyncEn == 0) {
    if (benchmark_begin(unit, b, "read soc reg", "read",
                        iter_cmic_read*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_read; i++) {
                /*    coverity[unchecked_value]    */
                rv = soc_reg32_get(unit, reg, port, 0, &rval);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
        }
    }
    benchmark_end(unit, b);

    if (benchmark_begin(unit, b, "write soc reg", "write",
                        iter_cmic_write*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_write; i++) {
                rv = soc_reg32_set(unit, reg, port, 0, rval);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
        }
    }
    benchmark_end(unit, b);

    if (benchmark_begin(unit, b, "schan fifo read soc reg", "read",
                        iter_cmic_read*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_read; i++) {
                /*    coverity[unchecked_value]    */
                rv = schan_fifo_reg32_get(unit, reg, port, 0, &rval, b);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
            rv = schan_fifo_flush(unit, b);
            if (BCM_FAILURE(rv))
                goto test_done;
        }
    }
    benchmark_end(unit, b);

    if (benchmark_begin(unit, b, "schan fifo write soc reg", "write",
                        iter_cmic_write*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_write; i++) {
                rv = schan_fifo_reg32_set(unit, reg, port, 0, rval, b);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
            rv = schan_fifo_flush(unit, b);
            if (BCM_FAILURE(rv))
                goto test_done;
        }
    }
    benchmark_end(unit, b);

    blk = SOC_MEM_BLOCK_ANY(unit, mem);
    num_byte = soc_mem_entry_words(unit, mem) * sizeof(uint32);
    if (benchmark_begin(unit, b, "read soc memory", "read",
                        iter_cmic_read*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_read; i++) {
                /*    coverity[unchecked_value]    */
                rv = soc_mem_read(unit, mem, blk, entry_idx, entry);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
        }
    }
    benchmark_end(unit, b);

    if (benchmark_begin(unit, b, "write soc memory", "write",
                        iter_cmic_write*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_write; i++) {
                /*    coverity[unchecked_value]    */
                rv = soc_mem_write(unit, mem, blk, entry_idx, entry);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
        }
    }
    benchmark_end(unit, b);

    if (benchmark_begin(unit, b, "schan fifo read soc memory", "read",
                        iter_cmic_read*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_read; i++) {
                /*    coverity[unchecked_value]    */
                rv = schan_fifo_mem_read(unit, mem, blk, entry_idx, entry, b);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
            rv = schan_fifo_flush(unit, b);
            if (BCM_FAILURE(rv))
                goto test_done;
        }
    }
    benchmark_end(unit, b);

    if (benchmark_begin(unit, b, "schan fifo write soc memory", "write",
                        iter_cmic_write*repeat_num, num_byte)) {
        for (j = 0; j < repeat_num; j++) {
            for (i = 0; i < iter_cmic_write; i++) {
                /*    coverity[unchecked_value]    */
                rv = schan_fifo_mem_write(unit, mem, blk, entry_idx, entry, b);
                if (BCM_FAILURE(rv))
                    goto test_done;
            }
            rv = schan_fifo_flush(unit, b);
            if (BCM_FAILURE(rv))
                goto test_done;
        }
    }
    benchmark_end(unit, b);
}
#ifdef BCM_SOC_ASYNC_SUPPORT
if (b->param_asyncEn == 1) {
    uint32 async_en = 1;
    int op_num_total = iter_cmic_read * repeat_num;
    int op_code;

    b->async_en = async_en;
    /* async mode: reg read */
    if (benchmark_begin(unit, b, "[ASYNC] schan fifo read soc reg", "read",
                        op_num_total, num_byte)) {
        op_code = READ_REGISTER_CMD_MSG;
        rv = schanfifo_reg_send_wrapper(unit, reg, op_code, op_num_total,
                                        port, async_en, b);
        if (BCM_FAILURE(rv))
            goto test_done;
    }
    benchmark_end(unit, b);

    /* async mode: reg write */
    if (benchmark_begin(unit, b, "[ASYNC] schan fifo write soc reg", "write",
                        op_num_total, num_byte)) {
        op_code = WRITE_REGISTER_CMD_MSG;
        rv = schanfifo_reg_send_wrapper(unit, reg, op_code, op_num_total,
                                        port, async_en, b);
        if (BCM_FAILURE(rv))
            goto test_done;
    }
    benchmark_end(unit, b);

    /* async mode: mem read */
    num_byte = soc_mem_entry_words(unit, mem) * sizeof(uint32);
    if (benchmark_begin(unit, b, "[ASYNC] schan fifo read soc memory", "read",
                        op_num_total, num_byte)) {
        op_code = READ_MEMORY_CMD_MSG;
        rv = schanfifo_mem_send_wrapper(unit, mem, op_code, op_num_total,
                                        async_en, b);
        if (BCM_FAILURE(rv))
            goto test_done;
    }
    benchmark_end(unit, b);

    /* async mode: mem write */
     num_byte = soc_mem_entry_words(unit, mem) * sizeof(uint32);
    if (benchmark_begin(unit, b, "[ASYNC] schan fifo write soc memory", "write",
                        op_num_total, num_byte)) {
        op_code = WRITE_MEMORY_CMD_MSG;
        rv = schanfifo_mem_send_wrapper(unit, mem, op_code, op_num_total,
                                        async_en, b);
        if (BCM_FAILURE(rv))
            goto test_done;
    }
    benchmark_end(unit, b);
}
#endif
test_done:
    SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);

    if (BCM_FAILURE(rv)) {
        test_error(unit, "SCHAN FIFO Test Failed!\n");
    }

    return rv;
}
