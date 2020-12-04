/*! \file bcma_testcase_cmicx_schan.c
 *
 * Broadcom test cases for CMICX PCI S-channel access
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

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx2.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_schan.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define CMICX_SCHAN_DEFAULT_VERBOSE     FALSE
#define CMICX_SCHAN_DEFAULT_PATTERN     0xff7f0080
#define CMICX_SCHAN_MAX_WORDS           BCMBD_CMICX2_SCHAN_WORDS
#define CMICX_SCHAN_PARAM_HELP \
    "  Verbose     - Whether to enable verbose testing mode\n"\
    "\nExample:\n"\
    "    Run PCI S-channel access testing\n"\
    "      tr 4\n"\
    "    Run PCI S-channel access testing with verbose mode (take longer time)\n"\
    "      tr 4 v=true\n"

typedef struct cmicx_schan_test_param_s {
    int verbose_mode;
} cmicx_schan_test_param_t;

static int
cmicx_schan_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static int
cmicx_schan_parse_param(bcma_cli_t *cli, bcma_cli_args_t *a,
                        cmicx_schan_test_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Verbose", "bool",
                                  &param->verbose_mode, NULL));
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
cmicx_schan_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                   uint32_t flags, void **bp)
{
    cmicx_schan_test_param_t *schan_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(schan_param, sizeof(cmicx_schan_test_param_t),
              "bcmaTestCasePciSchanTestParam");
    SHR_NULL_CHECK(schan_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    schan_param->verbose_mode = CMICX_SCHAN_DEFAULT_VERBOSE;

    /* parse */
    SHR_IF_ERR_EXIT
        (cmicx_schan_parse_param(cli, a, schan_param, show));
    if (show == true) {
        SHR_FREE(schan_param);
        SHR_EXIT();
    }

    *bp = (void *)schan_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(schan_param);
    }
    SHR_FUNC_EXIT();
}

static void
cmicx_schan_help(int unit, void *bp)
{
    cli_out("%s", CMICX_SCHAN_PARAM_HELP);
}

static void
cmicx_schan_recycle(int unit, void *bp)
{
    /* schan_param would be freed in testdb_run_teardown */
    return;
}

static int
cmicx_schan_test(int unit, void *bp, uint32_t option)
{
    cmicx_schan_test_param_t *p = (cmicx_schan_test_param_t *)bp;
    uint32_t word, pattern[CMICX_SCHAN_MAX_WORDS];
    uint32_t max_words = BCMBD_CMICX_SCHAN_WORDS;
    uint32_t iter;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX2)) {
        max_words = BCMBD_CMICX2_SCHAN_WORDS;
    }

    /* init pattern for each word */
    for (word = 0; word < max_words; word++) {
        pattern[word] = CMICX_SCHAN_DEFAULT_PATTERN ^
                        (word << 24 | word << 16 | word << 8 | word);
    }

    /* start to test */
    for (word = 0; word < max_words; word++) {
        uint32_t bit_num = BCMDRD_BYTES2BITS(sizeof(pattern[word]));

        for (iter = 0; iter < bit_num; iter++) {
            uint32_t read_value;

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "test schan word %"PRId32" "
                                 "iter %"PRId32" (%"PRIx32")\n"),
                      word, iter, pattern[word]));

            BCMBD_CMICX_WRITE(unit,
                              CMIC_SCHAN_MESSAGEr_OFFSET + word * 4,
                              pattern[word]);
            BCMBD_CMICX_READ(unit,
                             CMIC_SCHAN_MESSAGEr_OFFSET + word * 4,
                             &read_value);
            if (read_value != pattern[word]) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "test schan word %"PRId32" : "
                                      "wrote %"PRIx32" but got %"PRIx32"\n"),
                           word, pattern[word], read_value));
                SHR_IF_ERR_EXIT(SHR_E_FAIL);
            }

            /* exit loop earlier if not verbose mode, or rotate left */
            if (p->verbose_mode == FALSE && iter == 0) {
                break;
            }
            pattern[word] = (pattern[word] << 1) |
                            ((pattern[word] >> (bit_num - 1)) & 1);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t cmicx_schan_proc[] = {
    {
        .desc = "run PCI S-Channel access test.\n",
        .cb = cmicx_schan_test,
    },
};

static bcma_test_op_t cmicx_schan_op = {
    .select = cmicx_schan_select,
    .parser = cmicx_schan_parser,
    .help = cmicx_schan_help,
    .recycle = cmicx_schan_recycle,
    .procs = cmicx_schan_proc,
    .proc_count = COUNTOF(cmicx_schan_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_schan_op_get(void)
{
    return &cmicx_schan_op;
}
