/*! \file bcma_testcase_counter_snmp.c
 *
 * Broadcom test cases for verifying the SNMP statistics for each port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/testcase/bcma_testcase_counter_snmp.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define COUNTER_SNMP_PARAM_HELP \
    "  PortList  - Port list to be tested\n"\
    "\nExample:\n"\
    "    Run counter snmp statistics testing with default port list\n"\
    "      tr 73\n"\
    "    Run counter snmp statistics testing with user-specified port list\n"\
    "      tr 73 pl=2,4,7-10\n"

typedef struct counter_snmp_test_param_s {
    /* port bitmap need to be tested */
    bcmdrd_pbmp_t pbmp;
} counter_snmp_test_param_t;

static int
counter_snmp_select(int unit)
{
    return true;
}

static int
counter_snmp_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                         counter_snmp_test_param_t *snmp_param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_pbmp = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(snmp_param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;

    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PortList", "str", &parse_pbmp, NULL));

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

    /* translate pbmp */
    if (parse_pbmp != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_port_bmp(parse_pbmp, &(snmp_param->pbmp)));
    } else {
        SHR_IF_ERR_EXIT
            (bcma_testutil_default_port_bmp_get
                (unit, BCMDRD_PORT_TYPE_FPAN, &(snmp_param->pbmp)));
    }

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
counter_snmp_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                    uint32_t flags, void **bp)
{
    counter_snmp_test_param_t *snmp_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(snmp_param, sizeof(counter_snmp_test_param_t),
              "bcmaTestCaseCounterSnmpPrm");
    SHR_NULL_CHECK(snmp_param, SHR_E_MEMORY);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    BCMDRD_PBMP_CLEAR(snmp_param->pbmp);

    /* parse */
    SHR_IF_ERR_EXIT
        (counter_snmp_parse_param(unit, cli, a, snmp_param, show));

    if (show == true) {
        SHR_FREE(snmp_param);
        SHR_EXIT();
    }

    *bp = (void *)snmp_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(snmp_param);
    }
    SHR_FUNC_EXIT();
}

static void
counter_snmp_help(int unit, void *bp)
{
    cli_out("%s", COUNTER_SNMP_PARAM_HELP);
}

static void
counter_snmp_recycle(int unit, void *bp)
{
    /* snmp_param would be freed in testdb_run_teardown */
    return;
}

static int
counter_snmp_test(int unit, void *bp, uint32_t option)
{
    counter_snmp_test_param_t *snmp_param = (counter_snmp_test_param_t *)bp;
    int port, i;
    const char *table_str[] = {"CTR_MAC", "CTR_MAC_ERR"};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(snmp_param, SHR_E_PARAM);

    BCMDRD_PBMP_ITER(snmp_param->pbmp, port) {

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Testing port %d\n"), port));

        for (i = 0; i < COUNTOF(table_str); i++) {

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "%s\n"), table_str[i]));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, table_str[i], &entry_hdl));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                    BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL));

            bcmlt_entry_free(entry_hdl);
            entry_hdl = BCMLT_INVALID_HDL;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t counter_snmp_proc[] = {
    {
        .desc = "SNMP statistics retrieving test\n",
        .cb = counter_snmp_test,
    },
};

static bcma_test_op_t counter_snmp_op = {
    .select = counter_snmp_select,
    .parser = counter_snmp_parser,
    .help = counter_snmp_help,
    .recycle = counter_snmp_recycle,
    .procs = counter_snmp_proc,
    .proc_count = COUNTOF(counter_snmp_proc),
};

bcma_test_op_t *
bcma_testcase_counter_snmp_op_get(void)
{
    return &counter_snmp_op;
}
