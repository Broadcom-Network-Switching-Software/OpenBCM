/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * mem bulk tests
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
#include <soc/soc_mem_bulk.h>

#include <bcm/error.h>
#include <bcm/link.h>

#include <bcm_int/esw/mbcm.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/progress.h>

#include "testlist.h"
/* Decimal point formatting */
#define INT_FRAC_2PT(x) (x) / 100, (x) % 100

#define BULK_BUFMEM_SIZE 0x100000

typedef struct mem_bulk_op_s {
    /* memory */
    soc_mem_t *mem;

    /* index */
     int *index;

    /* Copy number */
    int *copyno;

    /* Data  ptr*/
    uint32 **entry;

    /* Data */
    uint32 *data;

} mem_bulk_op_t;

typedef struct bulk_test_s {
    uint32 testMask;
    int memTableID;
    int testNum;
    int sizeMem;
    soc_mem_t test_mem;
    mem_bulk_op_t buf_mem;
    char *testName;
    sal_usecs_t stime;
    sal_usecs_t etime;
    uint32 dataSize;
    uint32 dataWidth;
    char *dataUnit;
} bulk_test_t;
#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICX_SUPPORT)
int
mem_bulk_test_cleanup(int u, void *pa)
{
    bulk_test_t *b = (bulk_test_t *) pa;

    if (b) {
        if (b->buf_mem.mem) {
            sal_free(b->buf_mem.mem);
        }
        if (b->buf_mem.index) {
            sal_free(b->buf_mem.index);
        }
        if (b->buf_mem.copyno) {
            sal_free(b->buf_mem.copyno);
        }
        if (b->buf_mem.entry) {
            sal_free(b->buf_mem.entry);
        }
        if (b->buf_mem.data) {
            sal_free(b->buf_mem.data);
        }
        sal_free(b);
    }

    return 0;
}

int
mem_bulk_test_init(int u, args_t *a, void **pa)
{
    bulk_test_t *b = 0;
    int rv = SOC_E_FAIL;
    parse_table_t       pt;
    soc_mem_t mem;
    int count = 0;
    const char mem_bulk_test_usage[] =
    "MemTableID: Memory table selection\n"
    "                   0->L3_DEFIP_ALPM_LEVEL3m.\n"
    "                   1->L3_DEFIP_ALPM_LEVEL2m.\n"
    "                   2->L3_DEFIP_LEVEL1m.\n"
    "                   3->L3_DEFIP_PAIR_LEVEL1m.\n"
    "                   4->L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm.\n"
    "                   5->L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm.\n"
    "                   6->L3_DEFIP_ALPM_LEVEL3m.\n"
    "Count   : Number of operations (200 by default).\n";

    *pa = NULL;

    if ((b = sal_alloc(sizeof (bulk_test_t), "bulk_test")) == NULL) {
        goto done;
    }

    sal_memset(b, 0, sizeof (*b));

    *pa = (void *) b;

    b->testMask = ~0;
    b->testNum = 0;
    b->sizeMem = 200;

    parse_table_init(u, &pt);
    parse_table_add(&pt, "TestMask", PQ_HEX|PQ_DFL, 0,
                    &b->testMask, NULL);

    parse_table_add(&pt, "MemTableID", PQ_HEX|PQ_DFL, 0,
                    &b->memTableID, NULL);

    parse_table_add(&pt, "Count", PQ_INT|PQ_DFL, 0,
                    &b->sizeMem, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(u,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        goto done;
    }

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", mem_bulk_test_usage);
        test_error(u,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        rv = BCM_E_FAIL;
        goto done;
    } else {
        cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
        cli_out("\ntestMask   = %0d", b->testMask);
        cli_out("\nmemTableID   = %0d", b->memTableID);
        cli_out("\nsizeMem = %0d", b->sizeMem);
        cli_out("\n -----------------------------------------------------\n");
    }

    parse_arg_eq_done(&pt);

    switch (b->memTableID) {
        case 0: b->test_mem = L3_DEFIP_ALPM_LEVEL3m;   break;
        case 1: b->test_mem = L3_DEFIP_ALPM_LEVEL2m;   break;
        case 2: b->test_mem = L3_DEFIP_LEVEL1m;        break;
        case 3: b->test_mem = L3_DEFIP_PAIR_LEVEL1m;   break;
        case 4: b->test_mem = L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm; break;
        case 5: b->test_mem = L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm; break;
        case 6: b->test_mem = L3_DEFIP_ALPM_LEVEL3m;   break;
        default:b->test_mem = L3_DEFIP_ALPM_LEVEL3m;   break;
    }

    mem = b->test_mem;
    count = b->sizeMem;
    if (!SOC_MEM_IS_VALID(u, mem)) {
        rv = BCM_E_FAIL;
        goto done;
    }

    if (count == 0) {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(u,
            "Size of Table[%s] is ZERO\n"),
            SOC_MEM_UFNAME(u, mem)));
        goto done;
    }

    /* Allocate memory buffers */
    if ((b->buf_mem.mem =
             sal_alloc(count * sizeof(soc_mem_t), "bulk_op_mem")) == NULL) {
        cli_out("Not enough host memory\n");
        goto done;
    }

    if ((b->buf_mem.index =
             sal_alloc(count * sizeof(int), "bulk_op_id")) == NULL) {
        cli_out("Not enough host memory\n");
        goto done;
    }

    if ((b->buf_mem.copyno =
             sal_alloc(count * sizeof(int), "bulk_op_copy")) == NULL) {
        cli_out("Not enough host memory\n");
        goto done;
    }

    if ((b->buf_mem.entry =
             sal_alloc(count * sizeof(uint32 *), "bulk_op_entry")) == NULL) {
        cli_out("Not enough host memory\n");
        goto done;
    }

    if ((b->buf_mem.data =
             sal_alloc(BULK_BUFMEM_SIZE * sizeof(uint32), "bulk_op_data")) == NULL) {
        cli_out("Not enough host memory\n");
        goto done;
    }
    sal_memset(b->buf_mem.data, 0, BULK_BUFMEM_SIZE * sizeof(uint32));

    rv = 0;

 done:
    if (rv < 0) {
        mem_bulk_test_cleanup(u, *pa);
    }

    return rv;
}

static int
mem_bulk_test_begin(bulk_test_t *b,
                        char *testName,
                        char *dataUnit,
                        uint32 dataSize,
                        uint32 dataWidth)
{
    assert(b->testName == NULL);

    if ((b->testMask & (1U << b->testNum)) == 0) {
        return 0;
    }

    b->testName = testName;
    b->dataSize = dataSize;
    b->dataUnit = dataUnit;
    b->dataWidth = dataWidth;
    b->stime = sal_time_usecs();

    return 1;
}

static void
mem_bulk_test_end(bulk_test_t *b)
{
    if (b->testName) {
        uint32 per_sec, usec_per;
        sal_usecs_t td;
        char plus = ' ', lessthan = ' ';

        b->etime = sal_time_usecs();
        td = SAL_USECS_SUB(b->etime, b->stime);
        if (td == 0) {
            td = 1; /* Non-zero value */
            plus = '+';
            lessthan = '<';
        }
        per_sec = _shr_div_exp10(b->dataSize, td, 8);
        usec_per = b->dataSize ? _shr_div_exp10(td, b->dataSize, 2) : 0;
        cli_out ("\n Entry Size = %0d words\n", b->dataWidth);
        cli_out("%2d) %-28s%7d.%02d%c %5s/sec  ; %c%7d.%02d usec/%s\n",
            b->testNum,
            b->testName,
            INT_FRAC_2PT(per_sec),
            plus,
            b->dataUnit,
            lessthan,
            INT_FRAC_2PT(usec_per),
            b->dataUnit);
        b->testName = 0;
    }
    b->testNum++;
}

static int
mem_bulk_test_validate(int unit, bulk_test_t *b, int count)
{
    int rv = 0;
    int i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int size;

    for (i = 0; i < count; i++) {
        uint32 *data =  b->buf_mem.entry[i];
        size = soc_mem_entry_words(unit, b->buf_mem.mem[i]);
        rv = soc_mem_read(unit, b->buf_mem.mem[i], MEM_BLOCK_ANY,
                          b->buf_mem.index[i], entry);
        if (rv == SOC_E_NONE) {
            if (sal_memcmp(data, entry, size * sizeof(uint32)) != 0) {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "Data mismatch:[%s] index = %d \n"),
                    SOC_MEM_UFNAME(unit, b->buf_mem.mem[i]), b->buf_mem.index[i]));
                rv = SOC_E_FAIL;
                break;
            }
         } else {
             break;
         }
    }

    return rv;
}

int
mem_bulk_test(int unit, args_t *a, void *pa)
{
    bulk_test_t *b = (bulk_test_t *) pa;
    int rv = SOC_E_NONE;
    int i;
    soc_mem_t mem = 0;
    soc_mem_blk_handle_t handle;
    int size = 0;
    uint8 skip_cache;

    skip_cache = SOC_MEM_TEST_SKIP_CACHE(unit);

    SOC_MEM_TEST_SKIP_CACHE_SET(unit, TRUE);

    mem = b->test_mem;
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        goto error;
    }
    cli_out("\nName of test mem: %s\n",
                SOC_MEM_NAME(unit, mem));
    size = soc_mem_entry_words(unit, mem);
    cli_out ("\nTotal num of writes = %0d\n", b->sizeMem);
    for (i = 0; i <  b->sizeMem; i++) {
        uint32 *data;
        b->buf_mem.mem[i] = mem;
        b->buf_mem.index[i] = i;
        b->buf_mem.copyno[i] = MEM_BLOCK_ALL;
        b->buf_mem.entry[i] = &b->buf_mem.data[i * size];
        data = b->buf_mem.entry[i];
        rv = soc_mem_read(unit,mem, MEM_BLOCK_ANY,
                          i, data);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "Memory Read Error = %d\n"), rv));
            goto error;
        }
    }
    /* Get mem bulk handle */
    rv = soc_mem_bulk_op(unit, SOC_MEM_BULK_WRITE_INIT, &handle, 0, 0, 0, NULL);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                    "Error in allocating bulk handle = %d\n"), rv));
        goto error;
    }

    /* Stage the entries to write */
    for (i = 0; i <  b->sizeMem; i++) {
        rv = soc_mem_bulk_op(unit, SOC_MEM_BULK_WRITE_STAGE, &handle,
                             b->buf_mem.mem[i], b->buf_mem.index[i],
                             b->buf_mem.copyno[i], b->buf_mem.entry[i]);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "Error in staging bulk data = %d\n"), rv));
            goto error;
        }
    }

    /* Bulk mem commit */
    if (mem_bulk_test_begin(b, "Mem bulk Commit", "Entry",
                             b->sizeMem, size)) {
        rv = soc_mem_bulk_op(unit, SOC_MEM_BULK_WRITE_COMMIT, &handle, 0, 0, 0, NULL);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "Error in commiting bulk data = %d\n"), rv));
            goto error;
        }
        mem_bulk_test_end(b);
        rv = mem_bulk_test_validate(unit, b, b->sizeMem);
    }

    /* Bulk mem write */
    if (mem_bulk_test_begin(b, "Mem bulk Write", "Entry",
                             b->sizeMem, size)) {
        rv = soc_mem_bulk_write(unit, b->buf_mem.mem, b->buf_mem.index, b->buf_mem.copyno,
                                b->buf_mem.entry, b->sizeMem);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit,
                        "Error in writing bulk data = %d\n"), rv));
            goto error;
        }
        mem_bulk_test_end(b);
        rv = mem_bulk_test_validate(unit, b, b->sizeMem);
    }

error:
    SOC_MEM_TEST_SKIP_CACHE_SET(unit, skip_cache);
    return rv;
}
#endif /* BCM_ESW_SUPPORT && BCM_CMICX_SUPPORT */
