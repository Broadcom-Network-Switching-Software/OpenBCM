/*! \file bcma_testcase_cmicx_sbusdma.c
 *
 * Broadcom test cases for CMICX SBUSDMA access
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
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_mem.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_symbols.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_sbusdma.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

typedef struct cmicx_sbusdma_test_param_s {
    char *sname;           /* Symbol Name */
    int write;             /* 0:RO, 1:WR, Default=0 */
    int intr_mode;         /* 0:Polling, 1:Intr, Default=0 */
    int blktype;
    int blknum;
    int lane;
    int acctype;
    uint32_t offset;
    uint32_t index_min;
    uint32_t index_max;
    uint32_t laddr;
    uint32_t haddr;
    uint32_t adext;
    uint32_t addr;
    uint32_t shift;
    uint32_t wsize;
    uint32_t count;
    uint32_t flags;
    int msize;
    char *buf0;
    char *buf1;
    uint64_t paddr0;
    uint64_t paddr1;
} cmicx_sbusdma_test_param_t;

#define SBUSDMA_MAGIC_DATA1     0xa5

/*!
 * \brief Get the first available symbol name.
 *
 * This function is for getting the first available symbol name.
 *
 * \param [in] unit Device Number.
 * \param [out] sname Symbol Name.
 *
 * \retval SHR_E_NONE Get the symbol name successfully.
 * \retval SHR_E_INTERNAL Symbol table is emty.
 * \retval SHR_E_NOT_FOUND Not found the available symbol.
 */
static int
cmicx_sbusdma_sname_get(int unit, const char **sname)
{
    const bcmdrd_symbols_t *symbols;
    bcma_testutil_symbol_selector_t ss;
    bcmdrd_symbol_t s;

    SHR_FUNC_ENTER(unit);

    /* Initialize the symbol selector */
    sal_memset(&ss, 0, sizeof(ss));

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ss.pflags |= BCMDRD_SYMBOL_FLAG_MEMORY;
    /*
     * Use this flag to avoid including some unexpected memories,
     * like CLPORT_WC_UCMEM_DATAm, CDPORT_TSC_UCMEM_DATAm, etc...
     */
    ss.aflags |= BCMDRD_SYMBOL_FLAG_SOFT_PORT;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcma_testutil_symbol_get(symbols, &ss, &s));

    *sname = s.name;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_sbusdma_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static int
cmicx_sbusdma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    const char *sname = NULL;
    cmicx_sbusdma_test_param_t *p = NULL;
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sym_info_t sinfo = {0};
    /* Always use table instance 0 */
    int tbl_inst = 0;
    uint32_t pipe_info, pmask, linst, sect_size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicxSBUSDMAPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->sname = NULL;
    p->write = 0;
    p->intr_mode = 0;

    /* Get the first available symbol name. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cmicx_sbusdma_sname_get(unit, &sname));

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str",
                             &sname, NULL);
    bcma_cli_parse_table_add(&pt, "Write", "bool",
                             &p->write, NULL);
    bcma_cli_parse_table_add(&pt, "IntMode", "bool",
                             &p->intr_mode, NULL);

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

    if (!sname) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: Should specify a memory name\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Save parameter parsed from CLI */
    p->sname = sal_strdup(sname);

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (SHR_FAILURE(bcmdrd_pt_info_get_by_name(unit, p->sname, &sinfo))) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "No matching symbols. (%s)\n"),
                   p->sname));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (!(sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    p->blktype = bcmdrd_pt_blktype_get(unit, sinfo.sid, 0);
    p->acctype = BCMBD_CMICX_BLKACC2ACCTYPE(sinfo.blktypes);
    p->offset = sinfo.offset;
    p->blknum = bcmdrd_chip_block_number(cinfo, p->blktype, 0);
    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        p->lane = -1;
    }

    pipe_info = bcmbd_cmicx_pipe_info(unit, p->offset, p->acctype,
                                      p->blktype, -1);
    /* pmask > 0 if (access type == UNIQUE && (block type == IP/EP/MMU_ITM/MMU_EB)) */
    pmask = BCMBD_CMICX_PIPE_PMASK(pipe_info);
    /* linst > 0 if (block type == MMU_ITM/MMU_EB/MMU_GLB), indicate the number of base index */
    linst = BCMBD_CMICX_PIPE_LINST(pipe_info);
    sect_size = BCMBD_CMICX_PIPE_SECT_SIZE(pipe_info);
    if (pmask != 0) {
        /* Select a specific block instance for unique access type */
        p->acctype = tbl_inst;
    }
    if (linst != 0 && sect_size != 0) {
        p->offset += tbl_inst * sect_size;
    }

    p->adext = 0;
    BCMBD_CMICX_ADEXT_BLOCK_SET(p->adext, p->blknum);
    BCMBD_CMICX_ADEXT_ACCTYPE_SET(p->adext, p->acctype);
    p->index_min = sinfo.index_min;
    p->index_max = sinfo.index_max;
    p->laddr = bcmbd_block_port_addr(unit, p->blknum, p->lane,
                                     p->offset, p->index_min);
    p->haddr = bcmbd_block_port_addr(unit, p->blknum, p->lane,
                                     p->offset, p->index_max);
    p->shift = 0;
    p->wsize = sinfo.entry_wsize;
    p->count = sinfo.index_max - sinfo.index_min + 1;

    p->flags = p->intr_mode ? BCMBD_ROF_INTR : 0;
    p->msize = p->count * p->wsize * sizeof(uint32_t);
    p->buf0 = bcmdrd_hal_dma_alloc(unit, p->msize,
                                   "bcmaTestCaseCmicxSBUSDMADataBuf0",
                                   &p->paddr0);
    if (p->buf0 == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Cannot allocate DMA memory!\n"
                              "Please check whether DMA operation is "
                              "available on current system.\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(p->buf0, SBUSDMA_MAGIC_DATA1, p->msize);
    if (p->write) {
        p->buf1 = bcmdrd_hal_dma_alloc(unit, p->msize,
                                       "bcmaTestCaseCmicxSBUSDMADataBuf1",
                                       &p->paddr1);
        if (p->buf1 == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Cannot allocate DMA memory!\n"
                                  "Please check whether DMA operation is "
                                  "available on current system.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(p->buf1, SBUSDMA_MAGIC_DATA1, p->msize);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
cmicx_sbusdma_help(int unit, void *bp)
{
    cmicx_sbusdma_test_param_t *p = (cmicx_sbusdma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  Memory - Memory name\n");
    cli_out("  Write - SbusDma write[0: RO, 1: WR]\n");
    cli_out("  IntMode - Interrupt Mode[0: Polling, 1: Intr]\n");
    cli_out("  ====\n");
    cli_out("  sname=%s\n", p->sname ? p->sname : "(null)");
    cli_out("  write=%"PRId32"\n", p->write);
    cli_out("  intr_mode=%"PRId32"\n", p->intr_mode);
    cli_out("  blktype=%"PRId32"\n", p->blktype);
    cli_out("  blknum=%"PRId32"\n", p->blknum);
    cli_out("  lane=%"PRId32"\n", p->lane);
    cli_out("  acctype=%"PRId32"\n", p->acctype);
    cli_out("  offset=0x%"PRIx32"\n", p->offset);
    cli_out("  index_min=%"PRId32"\n", p->index_min);
    cli_out("  index_max=%"PRId32"\n", p->index_max);
    cli_out("  laddr=0x%"PRIx32"\n", p->laddr);
    cli_out("  haddr=0x%"PRIx32"\n", p->haddr);
    cli_out("  adext=0x%"PRIx32"\n", p->adext);
    cli_out("  addr=0x%"PRIx32"\n", p->addr);
    cli_out("  shift=%"PRId32"\n", p->shift);
    cli_out("  wsize=%"PRId32"\n", p->wsize);
    cli_out("  count=%"PRId32"\n", p->count);
    cli_out("  flags=0x%"PRIx32"\n", p->flags);
    cli_out("  msize=%"PRId32"\n", p->msize);
    return;
}

static void
cmicx_sbusdma_recycle(int unit, void *bp)
{
    cmicx_sbusdma_test_param_t *p = (cmicx_sbusdma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    if (p->sname) {
        SHR_FREE(p->sname);
    }
    if (p->buf0) {
        bcmdrd_hal_dma_free(unit, p->msize, p->buf0, p->paddr0);
        p->buf0 = NULL;
    }
    if (p->buf1) {
        bcmdrd_hal_dma_free(unit, p->msize, p->buf1, p->paddr1);
        p->buf1 = NULL;
    }
    return;
}

/*!
 * \brief Compare with two entries.
 *
 * This function is to compare with two entries.
 *
 * \param [in] dst Dest entries pointer.
 * \param [in] src Source entries pointer.
 * \param [in] wsize Size of entry in 32-bit words.
 * \param [in] count Count of entries to compare.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_FAIL Comparing fails.
 */
static int
cmicx_sbusdma_entries_cmp(char *dst, char *src, uint32_t wsize, int cnt)
{
    int i, off, len;

    len = (wsize - 1) * sizeof(uint32_t);
    for (i = 0; i < cnt; i++) {
        off = i * wsize * sizeof(uint32_t);
        if (sal_memcmp(&dst[off], &src[off], len)) {
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Sbusdma write, read and compare test.
 *
 * This function is intend to test sbusdma write operation
 * for a specific memory.
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
cmicx_sbusdma_test_write(int unit, void *bp, uint32_t option)
{
    cmicx_sbusdma_test_param_t *p = (cmicx_sbusdma_test_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    /* Regular write/read */
    p->addr = p->laddr;
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_write(unit, p->adext, p->addr, p->wsize,
                                     p->shift, p->count, p->paddr0, p->flags));
    sal_memset(p->buf1, 0, p->msize);
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_read(unit, p->adext, p->addr, p->wsize, p->shift,
                                    p->count, p->paddr1, p->flags));
    SHR_IF_ERR_EXIT
        (cmicx_sbusdma_entries_cmp(p->buf0, p->buf1, p->wsize, p->count));
    /* Invers write/read */
    p->addr = p->haddr;
    p->flags |=  BCMBD_ROF_INV_ADDR;
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_write(unit, p->adext, p->addr, p->wsize,
                                     p->shift, p->count, p->paddr0, p->flags));
    sal_memset(p->buf1, 0, p->msize);
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_read(unit, p->adext, p->addr, p->wsize, p->shift,
                                    p->count, p->paddr1, p->flags));
    SHR_IF_ERR_EXIT
        (cmicx_sbusdma_entries_cmp(p->buf0, p->buf1, p->wsize, p->count));
    p->flags &= ~BCMBD_ROF_INV_ADDR;

    /* Single address write/read */
    p->addr = p->laddr;
    p->flags |=  BCMBD_ROF_SGL_ADDR;
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_write(unit, p->adext, p->addr, p->wsize,
                                     p->shift, p->count, p->paddr0, p->flags));
    sal_memset(p->buf1, 0, p->msize);
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_read(unit, p->adext, p->addr, p->wsize, p->shift,
                                    p->count, p->paddr1, p->flags));
    SHR_IF_ERR_EXIT
        (cmicx_sbusdma_entries_cmp(p->buf0, p->buf1, p->wsize, p->count));
    p->flags &= ~BCMBD_ROF_SGL_ADDR;

    /* Memory clean/read */
    p->addr = p->laddr;
    p->flags |=  BCMBD_ROF_SGL_DATA;
    sal_memset(p->buf0, 0, p->msize);
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_write(unit, p->adext, p->addr, p->wsize,
                                     p->shift, p->count, p->paddr0, p->flags));
    sal_memset(p->buf1, 0, p->msize);
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_read(unit, p->adext, p->addr, p->wsize, p->shift,
                                    p->count, p->paddr1, p->flags));
    SHR_IF_ERR_EXIT
        (cmicx_sbusdma_entries_cmp(p->buf0, p->buf1, p->wsize, p->count));
    p->flags &= ~BCMBD_ROF_SGL_DATA;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Sbusdma read test.
 *
 * This function is intend to test sbusdma read operation
 * for a specific memory.
 *
 * \param [in] unit Device Number.
 * \param [in] bp Buffer pointer for test case parameters.
 * \param [in] option TDB options.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_xxx Other failures.
 */
static int
cmicx_sbusdma_test_read(int unit, void *bp, uint32_t option)
{
    cmicx_sbusdma_test_param_t *p = (cmicx_sbusdma_test_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    /* Regular read */
    p->addr = p->laddr;
    sal_memset(p->buf0, 0, p->msize);
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_mem_range_read(unit, p->adext, p->addr, p->wsize, p->shift,
                                    p->count, p->paddr0, p->flags));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_sbusdma_test(int unit, void *bp, uint32_t option)
{
    cmicx_sbusdma_test_param_t *p = (cmicx_sbusdma_test_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    if (p->write) {
        SHR_IF_ERR_EXIT(cmicx_sbusdma_test_write(unit, bp, option));
    } else {
        SHR_IF_ERR_EXIT(cmicx_sbusdma_test_read(unit, bp, option));
    }

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t cmicx_sbusdma_proc[] = {
    {
        .desc = "run SBUS DMA access test.\n",
        .cb = cmicx_sbusdma_test,
    },
};

static bcma_test_op_t cmicx_sbusdma_op = {
    .select = cmicx_sbusdma_select,
    .parser = cmicx_sbusdma_parser,
    .help = cmicx_sbusdma_help,
    .recycle = cmicx_sbusdma_recycle,
    .procs = cmicx_sbusdma_proc,
    .proc_count = COUNTOF(cmicx_sbusdma_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_sbusdma_op_get(void)
{
    return &cmicx_sbusdma_op;
}
