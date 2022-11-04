/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_SYMTAB

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/error.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifndef NO_FRAMER_LIB_BUILD
#ifdef CALENDAR_ENf
#undef CALENDAR_ENf
#endif
#ifdef SEG_FSM_STATEf
#undef SEG_FSM_STATEf
#endif
#ifdef CHID_RAMf
#undef CHID_RAMf
#endif
#include "lib_sal.h"
#include "chips.h"
#include "framer_sal_j2x_info.h"
#include "diag_framer_symbols.h"
#include "diag_framer_extra_symops.h"
#include "diag_framer_access.h"

typedef struct test_sym_s
{
    uint32 flags;
    int start_index;
    int count;
    void *cookie;
} test_sym_t;

#define TEST_F_ALL       (0x1 << 0)
#define TEST_F_CMP       (0x1 << 1)

#define TEST_P_RANDOM    (0x1 << 0)

typedef struct test_pattern_s
{
    uint32 flags;
    uint32 pattern;
} test_pattern_t;

static int testrun_debug = 0;

char cmd_testrun_usage[] =
    "Usages:\n\t"
    "With no parameters runs all selected tests with the set\n\t"
    "loop counts and set parameters.  Otherwise, runs a specific\n\t"
    "test with the specified loop count and arguments.\n\t"
    "\n"
    "Options:\n\t"
    "TestRun [Test ID|ALL] [*|arguments]\n\t"
    "\n"
    "Examples:\n\t"
    "TR 1\n\t"
    "TR 1 *\n\t"
    "TR 1 OHIF_OTN_1__INS_PKT_DA_SA\n\t"
    "TR 3\n\t"
    "TR 3 *\n\t"
    "TR 3 *.FICPB\n\t"
    "TR 3 OHIF_OTN_1__INS_PKT_DA_SA.FBSW\n\t"
    "TR 50\n\t"
    "TR 50 *\n\t"
    "TR 50 *.FICPB\n\t"
    "TR 50 * 0 1\n\t" "TR 50 B66SW__MAC2_ROUTE\n\t" "TR 50 B66SW__MAC2_ROUTE.FBSW 0 1\n\t" "TR all\n\t" "\n";

static int
testcase_1_do_filter(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (sym->flags & FRAMER_SYMBOL_F_RO || sym->flags & FRAMER_SYMBOL_F_WO || sym->flags & FRAMER_SYMBOL_F_INIT_ON_READ)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    /*
     * Module: PPS_TOD_GLOBAL_TS 
     */
    if (sym->base_addr == 0xb0600)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

static int
testcase_1_do(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int w, dw, midx, mismatch = 0;
    int rv = _SHR_E_NONE;
    char *regname = NULL;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    uint32 *data = NULL, *mask = NULL, value;
    test_sym_t *ts = (test_sym_t *) user_data;

    SHR_FUNC_INIT_VARS(unit);

    if (ts->flags & TEST_F_ALL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(testcase_1_do_filter(unit, flags, sym, user_data));
    }
    if (sym->alias[0] != '\0')
    {
        regname = &sym->alias[0];
    }
    else
    {
        regname = &sym->sname[0];
    }
    dw = sym->count;
    data = sal_alloc(sizeof(uint32) * dw, "RegData");
    SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(data, 0, sizeof(uint32) * dw);
    mask = sal_alloc(sizeof(uint32) * dw, "RegMask");
    SHR_NULL_CHECK(mask, _SHR_E_MEMORY, "mask is NULL!");
    sal_memset(mask, 0, sizeof(uint32) * dw);
    SHR_IF_ERR_EXIT(framer_symbol_mask(unit, sym, dw, mask));
    rv = framer_reg_raw_read(unit, sym->base_addr, sym->offset, dw, data);
    if (SHR_FAILURE(rv))
    {
        cli_out("RRD: %s[0x%x][0x%x]-> (%d)\n", regname, sym->base_addr, sym->offset, dw);
    }
    SHR_IF_ERR_EXIT(rv);
    for (w = 0; w < dw; w++)
    {
        value = data[w];
        value ^= sym->sinfo[w]->default_value;
        value &= mask[w];
        if (value)
        {
            mismatch++;
        }
    }
    if (testrun_debug || mismatch)
    {
        FRAMER_SYMBOL_M_GET(sym->index, midx);
        cli_out("%s", regname);
        if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
        {
            cli_out("[%d]", midx);
        }
        if (sym->blk_no < 0)
        {
            cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
        }
        else
        {
            cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
        }
        cli_out(" ... %s\n", (mismatch == 0) ? "PASSED" : "FAILED");
        if (mismatch)
        {
            cli_out("   DATA    = 0x");
            for (w = dw - 1; w >= 0; w--)
            {
                cli_out("%08x ", data[w]);
            }
            cli_out("\n");
            cli_out("   MASK    = 0x");
            for (w = dw - 1; w >= 0; w--)
            {
                cli_out("%08x ", mask[w]);
            }
            cli_out("\n");
            cli_out("   DEFAULT = 0x");
            for (w = dw - 1; w >= 0; w--)
            {
                cli_out("%08x ", sym->sinfo[w]->default_value);
            }
            cli_out("\n");
        }
    }

    SHR_SET_CURRENT_ERR(mismatch == 0 ? _SHR_E_NONE : _SHR_E_FAIL);

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    if (mask)
    {
        sal_free(mask);
        mask = NULL;
    }
    SHR_FUNC_EXIT;
}

static int
testcase_1(
    int unit,
    args_t * a)
{
    uint32 flags = 0;
    test_sym_t ts;
    char *arg2 = NULL;
    uint32 len, dflen;
    framer_symbol_filter_t filter;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ts, 0, sizeof(test_sym_t));
    arg2 = ARG_GET(a);
    if (arg2 == NULL)
    {
        arg2 = "*";
        ts.flags |= TEST_F_ALL;
    }
    else
    {
        dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
        len = sal_strnlen(arg2, dflen);
        if (sal_strncmp(arg2, "*", len) == 0)
        {
            ts.flags |= TEST_F_ALL;
        }
        else if (sal_strnchr(arg2, '*', len))
        {
            ts.flags |= TEST_F_ALL;
        }
    }
    framer_symbol_filter_t_init(&filter);
    filter.flags = FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = testcase_1_do;
    filter.name = arg2;
    filter.user_data = &ts;
    filter.summary = "TR 1";
    SHR_IF_ERR_EXIT_NO_MSG(framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter));

exit:
    SHR_FUNC_EXIT;
}

static int
testcase_3_do_filter(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (sym->flags & FRAMER_SYMBOL_F_RO || sym->flags & FRAMER_SYMBOL_F_WO || sym->flags & FRAMER_SYMBOL_F_INIT_ON_READ
        || sym->flags & FRAMER_SYMBOL_F_INTR || sym->flags & FRAMER_SYMBOL_F_RST)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    /*
     * Module: PPS_TOD_GLOBAL_TS 
     */
    if (sym->base_addr == 0xb0600)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

static int
testcase_3_do(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int w, dw, midx, mismatch = 0;
    int rv = _SHR_E_NONE;
    char *regname = NULL;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    uint32 *data = NULL, *mask = NULL, *value = NULL;
    test_sym_t *ts = (test_sym_t *) user_data;
    test_pattern_t *mp = (test_pattern_t *) ts->cookie;
    uint32 pattern;

    SHR_FUNC_INIT_VARS(unit);

    if (ts->flags & TEST_F_ALL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(testcase_3_do_filter(unit, flags, sym, user_data));
    }
    if (sym->alias[0] != '\0')
    {
        regname = &sym->alias[0];
    }
    else
    {
        regname = &sym->sname[0];
    }
    dw = sym->count;
    if (ts->flags & TEST_F_CMP)
    {
        data = sal_alloc(sizeof(uint32) * dw, "RegData");
        SHR_NULL_CHECK(data, _SHR_E_MEMORY, "data is NULL!");
        sal_memset(data, 0, sizeof(uint32) * dw);
    }
    mask = sal_alloc(sizeof(uint32) * dw, "RegMask");
    SHR_NULL_CHECK(mask, _SHR_E_MEMORY, "mask is NULL!");
    sal_memset(mask, 0, sizeof(uint32) * dw);
    SHR_IF_ERR_EXIT(framer_symbol_mask(unit, sym, dw, mask));
    value = sal_alloc(sizeof(uint32) * dw, "RegValue");
    SHR_NULL_CHECK(value, _SHR_E_MEMORY, "value is NULL!");
    sal_memset(value, 0, sizeof(uint32) * dw);
    if (mp->flags & TEST_P_RANDOM)
    {
        sal_srand(mp->pattern);
        pattern = sal_rand();
    }
    else
    {
        pattern = mp->pattern;
    }
    for (w = 0; w < dw; w++)
    {
        value[w] = pattern & mask[w];
    }
    rv = framer_reg_raw_write(unit, sym->base_addr, sym->offset, dw, value);
    if (SHR_FAILURE(rv))
    {
        cli_out("RWR: %s[0x%x][0x%x]-> (%d)\n", regname, sym->base_addr, sym->offset, dw);
    }
    SHR_IF_ERR_EXIT(rv);

    if (ts->flags & TEST_F_CMP)
    {
        rv = framer_reg_raw_read(unit, sym->base_addr, sym->offset, dw, data);
        if (SHR_FAILURE(rv))
        {
            cli_out("RRD: %s[0x%x][0x%x]-> (%d)\n", regname, sym->base_addr, sym->offset, dw);
        }
        SHR_IF_ERR_EXIT(rv);
        for (w = 0; w < dw; w++)
        {
            value[w] = data[w];
            value[w] ^= pattern;
            value[w] &= mask[w];
            if (value[w])
            {
                mismatch++;
            }
        }
        if (testrun_debug || mismatch)
        {
            FRAMER_SYMBOL_M_GET(sym->index, midx);
            cli_out("%s", regname);
            if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
            {
                cli_out("[%d]", midx);
            }
            if (sym->blk_no < 0)
            {
                cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
            }
            else
            {
                cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
            }
            cli_out(" ... %s (pattern=0x%08x)\n", (mismatch == 0) ? "PASSED" : "FAILED", mp->pattern);
            if (mismatch)
            {
                cli_out("   WRITE    = 0x");
                for (w = dw - 1; w >= 0; w--)
                {
                    cli_out("%08x ", pattern & mask[w]);
                }
                cli_out("\n");
                cli_out("   READ     = 0x");
                for (w = dw - 1; w >= 0; w--)
                {
                    cli_out("%08x ", data[w]);
                }
                cli_out("\n");
                cli_out("   MASK     = 0x");
                for (w = dw - 1; w >= 0; w--)
                {
                    cli_out("%08x ", mask[w]);
                }
                cli_out("\n");
            }
        }

        SHR_SET_CURRENT_ERR(mismatch == 0 ? _SHR_E_NONE : _SHR_E_FAIL);
    }

exit:
    if (data)
    {
        sal_free(data);
        data = NULL;
    }
    if (mask)
    {
        sal_free(mask);
        mask = NULL;
    }
    if (value)
    {
        sal_free(value);
        value = NULL;
    }
    SHR_FUNC_EXIT;
}

static int
testcase_3(
    int unit,
    args_t * a)
{
    uint32 flags = 0;
    char *arg2 = NULL;
    uint32 len, dflen;
    framer_symbol_filter_t filter;
    test_pattern_t mp[] = {
        {0, 0xffffffff},
        {0, 0x00000000},
        {0, 0x55555555},
        {0, 0xaaaaaaaa},
        {TEST_P_RANDOM, 0xd246fe4b},
    };
    test_sym_t ts;
    int i, failures = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ts, 0, sizeof(test_sym_t));
    ts.flags |= TEST_F_CMP;
    arg2 = ARG_GET(a);
    if (arg2 == NULL)
    {
        arg2 = "*";
        ts.flags |= TEST_F_ALL;
    }
    else
    {
        dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
        len = sal_strnlen(arg2, dflen);
        if (sal_strncmp(arg2, "*", len) == 0)
        {
            ts.flags |= TEST_F_ALL;
        }
        else if (sal_strnchr(arg2, '*', len))
        {
            ts.flags |= TEST_F_ALL;
        }
    }
    for (i = 0; i < COUNTOF(mp); i++)
    {
        ts.cookie = &mp[i];
        framer_symbol_filter_t_init(&filter);
        filter.name = arg2;
        filter.flags = FRAMER_FILTER_LOOP | FRAMER_FILTER_CONT_ON_ERR;
        filter.cb = testcase_3_do;
        filter.user_data = &ts;
        filter.summary = "TR 3";
        SHR_IF_ERR_EXIT(framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter));
        failures += filter.stat.failure;
    }
    SHR_IF_ERR_EXIT_NO_MSG(failures ? _SHR_E_FAIL : _SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

static int
testcase_50_do_ab_tables(
    int unit,
    uint32 flags,
    uint32 base_addr,
    uint32 offset)
{
    int idx;
    uint32 sel, sel_st, cnt;
    SHR_FUNC_INIT_VARS(unit);

    if (ADAPT_INSTANCE_TX__CALENA_INSTANCE_TXm__addr(base_addr, offset))
    {
        ADAPT_INSTANCE_TX__CALEN_SEL_INSTANCE_TXr__CALEN_SEL_INSTANCE_TXf__modify(unit, 1);
    }
    else if (ADAPT_INSTANCE_TX__CALENB_INSTANCE_TXm__addr(base_addr, offset))
    {
        ADAPT_INSTANCE_TX__CALEN_SEL_INSTANCE_TXr__CALEN_SEL_INSTANCE_TXf__modify(unit, 0);
    }
    else if (ADAPT_FLEXE_DEMUX__CALENA_FLEXE_DEMUXm__addr(0, base_addr, offset))
    {
        ADAPT_FLEXE_DEMUX__CALEN_SEL_FLEXE_DEMUXr__CALEN_SEL_FLEXE_DEMUXf__modify(unit, 0, 1);
    }
    else if (ADAPT_FLEXE_DEMUX__CALENB_FLEXE_DEMUXm__addr(0, base_addr, offset))
    {
        ADAPT_FLEXE_DEMUX__CALEN_SEL_FLEXE_DEMUXr__CALEN_SEL_FLEXE_DEMUXf__modify(unit, 0, 0);
    }
    else if (ADAPT_FLEXE_DEMUX__CALENA_FLEXE_DEMUXm__addr(1, base_addr, offset))
    {
        ADAPT_FLEXE_DEMUX__CALEN_SEL_FLEXE_DEMUXr__CALEN_SEL_FLEXE_DEMUXf__modify(unit, 1, 1);
    }
    else if (ADAPT_FLEXE_DEMUX__CALENB_FLEXE_DEMUXm__addr(1, base_addr, offset))
    {
        ADAPT_FLEXE_DEMUX__CALEN_SEL_FLEXE_DEMUXr__CALEN_SEL_FLEXE_DEMUXf__modify(unit, 1, 0);
    }
    else if (B66SW__CALENA_X9_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0x1 << 0);
    }
    else if (B66SW__CALENB_X9_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0);
    }
    else if (B66SW__CALENA_X5_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0x1 << 1);
    }
    else if (B66SW__CALENB_X5_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0);
    }
    else if (B66SW__CALENA_X4_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0x1 << 2);
    }
    else if (B66SW__CALENB_X4_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0);
    }
    else if (B66SW__CALENA_X8_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_ST_B66SWr__t r;
        sel = 0x1 << 3;
        cnt = 3;
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, sel);
        do
        {
            framer_time_delay_us(5000);
            B66SW__CALENDAR_SEL_ST_B66SWr__read(unit, r);
            B66SW__CALENDAR_SEL_ST_B66SWr__CALENDAR_SEL_ST_B66SWf__get(r, &sel_st);
            if (sel_st == sel)
            {
                break;
            }
        }
        while (--cnt);
        if (cnt <= 0)
        {
            SHR_IF_ERR_EXIT(_SHR_E_TIMEOUT);
        }
    }
    else if (B66SW__CALENB_X8_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0);
    }
    else if (B66SW__CALENA_X2_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0x1 << 4);
    }
    else if (B66SW__CALENB_X2_B66SWm__addr(base_addr, offset))
    {
        B66SW__CALENDAR_SEL_B66SWr__CALENDAR_SEL_B66SWf__modify(unit, 0);
    }
    else if (ODUSW__CALENA_ODUSWm__addr(base_addr, offset))
    {
        idx = ODUSW__CALENA_ODUSWm_idx(base_addr, offset);
        ODUSW__CALENDAR_SEL_ODUSWr__CALENDAR_SEL_ODUSWf__modify(unit, 0x1 << idx);
    }
    else if (ODUSW__CALENB_ODUSWm__addr(base_addr, offset))
    {
        ODUSW__CALENDAR_SEL_ODUSWr__CALENDAR_SEL_ODUSWf__modify(unit, 0);
    }
    else if (P2S_PHYMUX_RX__CALENA_X16m__addr(base_addr, offset))
    {
        idx = P2S_PHYMUX_RX__CALENA_X16m_idx(base_addr, offset);
        P2S_PHYMUX_RX__CALEN_SEL_X16r__CALEN_SEL_X16f__modify(unit, 0x1 << idx);
    }
    else if (P2S_PHYMUX_RX__CALENB_X16m__addr(base_addr, offset))
    {
        P2S_PHYMUX_RX__CALEN_SEL_X16r__CALEN_SEL_X16f__modify(unit, 0);
    }
    else if (P2S_PHYMUX_RX__CALENA_X4m__addr(base_addr, offset))
    {
        P2S_PHYMUX_RX__CALEN_SEL_X4r__CALEN_SEL_X4f__modify(unit, 1);
    }
    else if (P2S_PHYMUX_RX__CALENB_X4m__addr(base_addr, offset))
    {
        P2S_PHYMUX_RX__CALEN_SEL_X4r__CALEN_SEL_X4f__modify(unit, 0);
    }
    else if (ODU4CSW__CALENA_ODU4CSWm__addr(base_addr, offset))
    {
        idx = ODU4CSW__CALENA_ODU4CSWm_idx(base_addr, offset);
        ODU4CSW__CALEN_SEL_ODU4CSWr__CALEN_SEL_ODU4CSWf__modify(unit, 0x1 << idx);
    }
    else if (ODU4CSW__CALENB_ODU4CSWm__addr(base_addr, offset))
    {
        ODU4CSW__CALEN_SEL_ODU4CSWr__CALEN_SEL_ODU4CSWf__modify(unit, 0);
    }
    else if (ADAPT_ODUCDEMUX__CALENA_ODU4Cm__addr(base_addr, offset))
    {
        ADAPT_ODUCDEMUX__CALEN_SEL_B0r__CALEN_SEL_B0f__modify(unit, 1);
    }
    else if (ADAPT_ODUCDEMUX__CALENB_ODU4Cm__addr(base_addr, offset))
    {
        ADAPT_ODUCDEMUX__CALEN_SEL_B0r__CALEN_SEL_B0f__modify(unit, 0);
    }
    else if (OPU2_DEMUX__CALENA_OPU2_SCHm__addr(base_addr, offset))
    {
        OPU2_DEMUX__CALEN_SEL_OPU2_SCHr__CALEN_SEL_OPU2_SCHf__modify(unit, 1);
    }
    else if (OPU2_DEMUX__CALENB_OPU2_SCHm__addr(base_addr, offset))
    {
        OPU2_DEMUX__CALEN_SEL_OPU2_SCHr__CALEN_SEL_OPU2_SCHf__modify(unit, 0);
    }
    else if (ADAPT_ODU2DEMUX__CALENA_6XODUJm__addr(base_addr, offset))
    {
        ADAPT_ODU2DEMUX__CALEN_SEL_B2r__CALEN_SEL_B2f__modify(unit, 1);
    }
    else if (ADAPT_ODU2DEMUX__CALENB_6XODUJm__addr(base_addr, offset))
    {
        ADAPT_ODU2DEMUX__CALEN_SEL_B2r__CALEN_SEL_B2f__modify(unit, 0);
    }
    else if (OPU25_DEMUX__CALENA_OPU25_SCHm__addr(base_addr, offset))
    {
        OPU25_DEMUX__CALEN_SEL_OPU25_SCHr__CALEN_SEL_OPU25_SCHf__modify(unit, 1);
    }
    else if (OPU25_DEMUX__CALENB_OPU25_SCHm__addr(base_addr, offset))
    {
        OPU25_DEMUX__CALEN_SEL_OPU25_SCHr__CALEN_SEL_OPU25_SCHf__modify(unit, 0);
    }
    else if (ADAPT_ODUCMUX__CALENA_3XODUCMUXm__addr(base_addr, offset))
    {
        ADAPT_ODUCMUX__CALEN_SEL_B3r__CALEN_SEL_B3f__modify(unit, 0x1 << 0);
    }
    else if (ADAPT_ODUCMUX__CALENB_3XODUCMUXm__addr(base_addr, offset))
    {
        ADAPT_ODUCMUX__CALEN_SEL_B3r__CALEN_SEL_B3f__modify(unit, 0);
    }
    else if (ADAPT_ODUCMUX__CALENA_3XODU4Cm__addr(base_addr, offset))
    {
        ADAPT_ODUCMUX__CALEN_SEL_B3r__CALEN_SEL_B3f__modify(unit, 0x1 << 1);
    }
    else if (ADAPT_ODUCMUX__CALENB_3XODU4Cm__addr(base_addr, offset))
    {
        ADAPT_ODUCMUX__CALEN_SEL_B3r__CALEN_SEL_B3f__modify(unit, 0);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENA_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0x1 << 0);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENB_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENA_FLEXO_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0x1 << 1);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENB_FLEXO_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENA_ODUC_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0x1 << 2);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENB_ODUC_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENA_ODU4_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0x1 << 3);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENB_ODU4_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENA_ODUX_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0x1 << 4);
    }
    else if (ADAPT_ENCRYPT_LINE__CALENB_ODUX_ENCRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_ENCRYPT_LINE__CALEN_SEL_ENCRYPT_LINEr__CALEN_SEL_ENCRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_DECRYPT_LINE__CALENA_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0x1 << 0);
    }
    else if (ADAPT_DECRYPT_LINE__CALENB_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_DECRYPT_LINE__CALENA_FLEXO_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0x1 << 1);
    }
    else if (ADAPT_DECRYPT_LINE__CALENB_FLEXO_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_DECRYPT_LINE__CALENA_ODUC_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0x1 << 2);
    }
    else if (ADAPT_DECRYPT_LINE__CALENB_ODUC_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_DECRYPT_LINE__CALENA_ODU4_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0x1 << 3);
    }
    else if (ADAPT_DECRYPT_LINE__CALENB_ODU4_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0);
    }
    else if (ADAPT_DECRYPT_LINE__CALENA_ODUX_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0x1 << 4);
    }
    else if (ADAPT_DECRYPT_LINE__CALENB_ODUX_DECRYPT_LINEm__addr(base_addr, offset))
    {
        ADAPT_DECRYPT_LINE__CALEN_SEL_DECRYPT_LINEr__CALEN_SEL_DECRYPT_LINEf__modify(unit, 0);
    }
    else if (ODUK_LINK_FIFO__ODUK_LK_CALENA_REQm__addr(0, base_addr, offset))
    {
        ODUK_LINK_FIFO__ODUK_LK_CALEN_SELr__ODUK_LK_CALEN_SELf__modify(unit, 0, 1);
    }
    else if (ODUK_LINK_FIFO__ODUK_LK_CALENB_REQm__addr(0, base_addr, offset))
    {
        ODUK_LINK_FIFO__ODUK_LK_CALEN_SELr__ODUK_LK_CALEN_SELf__modify(unit, 0, 0);
    }
    else if (ODUK_LINK_FIFO__ODUK_LK_CALENA_REQm__addr(1, base_addr, offset))
    {
        ODUK_LINK_FIFO__ODUK_LK_CALEN_SELr__ODUK_LK_CALEN_SELf__modify(unit, 1, 1);
    }
    else if (ODUK_LINK_FIFO__ODUK_LK_CALENB_REQm__addr(1, base_addr, offset))
    {
        ODUK_LINK_FIFO__ODUK_LK_CALEN_SELr__ODUK_LK_CALEN_SELf__modify(unit, 1, 0);
    }
    else if (ADAPT_ODU2MUX__CALENA_3XODUXMUXm__addr(base_addr, offset))
    {
        ADAPT_ODU2MUX__CALEN_SEL_B5r__CALEN_SEL_B5f__modify(unit, 1);
    }
    else if (ADAPT_ODU2MUX__CALENB_3XODUXMUXm__addr(base_addr, offset))
    {
        ADAPT_ODU2MUX__CALEN_SEL_B5r__CALEN_SEL_B5f__modify(unit, 0);
    }
    else if (ADAPT_ODU4MUX__CALENA_2XODU4MUXm__addr(base_addr, offset))
    {
        idx = ADAPT_ODU4MUX__CALENA_2XODU4MUXm_idx(base_addr, offset);
        ADAPT_ODU4MUX__CALEN_SEL_B4r__CALEN_SEL_B4f__modify(unit, 0x1 << idx);
    }
    else if (ADAPT_ODU4MUX__CALENB_2XODU4MUXm__addr(base_addr, offset))
    {
        ADAPT_ODU4MUX__CALEN_SEL_B4r__CALEN_SEL_B4f__modify(unit, 0);
    }
    else if (OPU4_DEMUXJ__CALENA_OPU4_SCHm__addr(base_addr, offset))
    {
        OPU4_DEMUXJ__CALEN_SEL_OPU4_SCHr__CALEN_SEL_OPU4_SCHf__modify(unit, 1);
    }
    else if (OPU4_DEMUXJ__CALENB_OPU4_SCHm__addr(base_addr, offset))
    {
        OPU4_DEMUXJ__CALEN_SEL_OPU4_SCHr__CALEN_SEL_OPU4_SCHf__modify(unit, 0);
    }
    else if (ADAPT_ODU4DEMUX__CALENA_3XODUXm__addr(base_addr, offset))
    {
        ADAPT_ODU4DEMUX__CALEN_SEL_B1r__CALEN_SEL_B1f__modify(unit, 1);
    }
    else if (ADAPT_ODU4DEMUX__CALENB_3XODUXm__addr(base_addr, offset))
    {
        ADAPT_ODU4DEMUX__CALEN_SEL_B1r__CALEN_SEL_B1f__modify(unit, 0);
    }
    else if (SAR_TX__SARTX_TPULSEAm__addr(base_addr, offset))
    {
        SAR_TX__SARTX_GLOBAL_CFGr__DSM_TPULSE_TBL_SELf__modify(unit, 1);
    }
    else if (SAR_TX__SARTX_TPULSEBm__addr(base_addr, offset))
    {
        SAR_TX__SARTX_GLOBAL_CFGr__DSM_TPULSE_TBL_SELf__modify(unit, 0);
    }
    else if (SAR_TX__SARTX_DSMAm__addr(base_addr, offset))
    {
        SAR_TX__SARTX_GLOBAL_CFGr__DSM_TPULSE_TBL_SELf__modify(unit, 1);
    }
    else if (SAR_TX__SARTX_DSMBm__addr(base_addr, offset))
    {
        SAR_TX__SARTX_GLOBAL_CFGr__DSM_TPULSE_TBL_SELf__modify(unit, 0);
    }
    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

static int
testcase_50_do_changeable_tables(
    int unit,
    uint32 flags,
    uint32 base_addr,
    uint32 offset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (FLEXE_OAM_RX__CHID_RAMm__addr(0, base_addr, offset))
    {
        FLEXE_OAM_RX__OAM_RX_CONFIG1r__RAM_DEPTHf__modify(unit, 0, 1600);
    }
    else if (FLEXE_OAM_RX__CHID_RAMm__addr(1, base_addr, offset))
    {
        FLEXE_OAM_RX__OAM_RX_CONFIG1r__RAM_DEPTHf__modify(unit, 1, 1600);
    }

    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NONE);
exit:
    SHR_FUNC_EXIT;
}

static int
testcase_50_do_filter(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int i;
    struct filter_s
    {
        uint32 base_addr;
        uint16 offset_addr;
    } filter[] =
    {
        /*
         * OTUC_RX_LINE__CR_DFM_LOM_SET_CLR_CFG.FLOTN
         */
        {
        0x41000, 0x21},
            /*
             * OTUC_RX_LINE__CR_DFM_DESCR_EN_CFG.FLOTN
             */
        {
        0x41000, 0x22},
            /*
             * OTUC_RX_LINE__CR_DFM_LOF_SET_CLR_CFG.FLOTN
             */
        {
        0x41000, 0x23},
            /*
             * ODU4_RX_MUX__CR_DFM_LOM_SET_CLR_CFG.FDMXBI
             */
        {
        0x4c800, 0x21},
            /*
             * ODU4_RX_MUX__CR_DFM_DESCR_EN_CFG.FDMXBI
             */
        {
        0x4c800, 0x22},
            /*
             * ODU4_RX_MUX__CR_DFM_LOF_SET_CLR_CFG.FDMXBI
             */
        {
        0x4c800, 0x23},
            /*
             * STAT_RX__STATRX_CRC_MODE_CFG.FMACR
             */
        {
        0x94000, 0x407},
            /*
             * ODUK_RX_OH_MUX__R_DFM_LOM_SET_CLR_CFG.FODUO0
             */
        {
        0xb8000, 0x21},
            /*
             * ODUK_RX_OH_MUX__R_DFM_DESCR_EN_CFG.FODUO0
             */
        {
        0xb8000, 0x22},
            /*
             * ODUK_RX_OH_SAR__R_DFM_LOM_SET_CLR_CFG.FODUO1
             */
        {
        0xbc000, 0x21},
            /*
             * ODUK_RX_OH_SAR__R_DFM_DESCR_EN_CFG.FODUO1
             */
        {
    0xbc000, 0x22},};

    SHR_FUNC_INIT_VARS(unit);
    if (sym->flags & FRAMER_SYMBOL_F_RO || sym->flags & FRAMER_SYMBOL_F_WO)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    for (i = 0; i < COUNTOF(filter); i++)
    {
        if (sym->base_addr == filter[i].base_addr && sym->offset == filter[i].offset_addr)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
testcase_50_do(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int i, k, dw, start, count;
    int rv = _SHR_E_NONE;
    uint32 width = 0, depth = 0;
    uint32 *rdata = NULL, *wdata = NULL, *mask = NULL, pattern, value;
    test_sym_t *ts = (test_sym_t *) user_data;
    test_pattern_t *mp = (test_pattern_t *) ts->cookie;
    const char *blk_name[] = FRAMER_BLK_NAME_STR;
    int midx, mismatch = 0;
    char *memname = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (ts->flags & TEST_F_ALL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(testcase_50_do_filter(unit, flags, sym, user_data));
    }
    start = ts->start_index;
    count = ts->count;

    if (sym->alias[0] != '\0')
    {
        memname = &sym->alias[0];
    }
    else
    {
        memname = &sym->sname[0];
    }
    SHR_IF_ERR_EXIT_NO_MSG(testcase_50_do_ab_tables(unit, flags, sym->base_addr, sym->offset));
    SHR_IF_ERR_EXIT_NO_MSG(testcase_50_do_changeable_tables(unit, flags, sym->base_addr, sym->offset));
    depth = sym->depth;
    width = sym->width;
    dw = (width + 32 - 1) / 32;
    if ((start + count) > depth)
    {
        count = 0;
    }
    if (count == 0)
    {
        count = depth - start;
    }
    if (ts->flags & TEST_F_CMP)
    {
        rdata = sal_alloc(sizeof(uint32) * dw, "ReadMemData");
        SHR_NULL_CHECK(rdata, _SHR_E_MEMORY, "rdata is NULL!");
        sal_memset(rdata, 0, sizeof(uint32) * dw);
    }
    wdata = sal_alloc(sizeof(uint32) * dw, "WriteMemData");
    SHR_NULL_CHECK(wdata, _SHR_E_MEMORY, "wdata is NULL!");
    sal_memset(wdata, 0, sizeof(uint32) * dw);
    mask = sal_alloc(sizeof(uint32) * dw, "MemMask");
    SHR_NULL_CHECK(mask, _SHR_E_MEMORY, "mask is NULL!");
    sal_memset(mask, 0, sizeof(uint32) * dw);
    SHR_IF_ERR_EXIT(framer_symbol_mask(unit, sym, dw, mask));

    for (k = start; k < start + count; k++)
    {
        for (i = 0; i < dw; i++)
        {
            if (mp->flags & TEST_P_RANDOM)
            {
                sal_srand(mp->pattern);
                pattern = sal_rand();
            }
            else
            {
                pattern = mp->pattern;
            }
            wdata[i] = pattern & mask[i];
        }
        rv = framer_mem_raw_write(unit, sym->base_addr, sym->offset, k, width, wdata);
        if (SHR_FAILURE(rv))
        {
            cli_out("MWR: %s[0x%x][0x%x][%d]-> width = %d (%d)\n", memname, sym->base_addr, sym->offset, k, width, dw);
        }
        SHR_IF_ERR_EXIT(rv);
        if (ts->flags & TEST_F_CMP)
        {
            rv = framer_mem_raw_read(unit, sym->base_addr, sym->offset, k, width, rdata);
            if (SHR_FAILURE(rv))
            {
                cli_out("MRD: %s[0x%x][0x%x][%d]-> width = %d (%d)\n", memname, sym->base_addr, sym->offset, k, width,
                        dw);
            }
            SHR_IF_ERR_EXIT(rv);
            for (i = 0; i < dw; i++)
            {
                value = rdata[i];
                value ^= wdata[i];
                value &= mask[i];
                if (value)
                {
                    mismatch++;
                }
            }
            if (testrun_debug || mismatch)
            {
                FRAMER_SYMBOL_M_GET(sym->index, midx);
                cli_out("%s", memname);
                if (sym->flags & FRAMER_SYMBOL_F_ARRAY)
                {
                    cli_out("[%d]", midx);
                }
                if (sym->blk_no < 0)
                {
                    cli_out(".%s", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk]);
                }
                else
                {
                    cli_out(".%s%d", sym->blk < 0 ? FRAMER_BLK_NAME_RAW : blk_name[sym->blk], sym->blk_no);
                }
                cli_out("[0x%x][0x%x][%d]", sym->base_addr, sym->offset, k);
                cli_out(" ... %s (pattern=0x%08x)\n", (mismatch == 0) ? "PASSED" : "FAILED", mp->pattern);
                if (mismatch)
                {
                    cli_out("   WRITE    = 0x");
                    for (i = dw - 1; i >= 0; i--)
                    {
                        cli_out("%08x ", wdata[i]);
                    }
                    cli_out("\n");
                    cli_out("   READ     = 0x");
                    for (i = dw - 1; i >= 0; i--)
                    {
                        cli_out("%08x ", rdata[i]);
                    }
                    cli_out("\n");
                    cli_out("   MASK     = 0x");
                    for (i = dw - 1; i >= 0; i--)
                    {
                        cli_out("%08x ", mask[i]);
                    }
                    cli_out("\n");
                }
            }
        }
    }

    if (ts->flags & TEST_F_CMP)
    {
        SHR_SET_CURRENT_ERR(mismatch == 0 ? _SHR_E_NONE : _SHR_E_FAIL);
    }
exit:
    if (rdata)
    {
        sal_free(rdata);
        rdata = NULL;
    }
    if (wdata)
    {
        sal_free(wdata);
        wdata = NULL;
    }
    if (mask)
    {
        sal_free(mask);
        mask = NULL;
    }
    SHR_FUNC_EXIT;
}

static int
testcase_50(
    int unit,
    args_t * a)
{
    uint32 flags = 0;
    char *arg2 = NULL, *arg3 = NULL, *arg4 = NULL;
    uint32 len, dflen;
    framer_symbol_filter_t filter;
    test_sym_t ts;
    test_pattern_t mp[] = {
        {0, 0xffffffff},
        {0, 0x00000000},
        {0, 0x55555555},
        {0, 0xaaaaaaaa},
        {TEST_P_RANDOM, 0xd246fe4b},
    };
    int i, failures = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ts, 0, sizeof(test_sym_t));
    ts.flags |= TEST_F_CMP;
    arg2 = ARG_GET(a);
    if (arg2 == NULL)
    {
        arg2 = "*";
        ts.flags |= TEST_F_ALL;
    }
    else
    {
        dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
        len = sal_strnlen(arg2, dflen);
        if (sal_strncmp(arg2, "*", len) == 0)
        {
            ts.flags |= TEST_F_ALL;
        }
        else if (sal_strnchr(arg2, '*', len))
        {
            ts.flags |= TEST_F_ALL;
        }
    }
    arg3 = ARG_GET(a);
    arg4 = ARG_GET(a);
    if ((arg3) && (isint(arg3)))
    {
        ts.start_index = parse_integer(arg3);
        if ((arg4) && (isint(arg4)))
        {
            ts.count = parse_integer(arg4);
        }
        else
        {
            ts.count = 1;
        }
    }
    else
    {
        ts.start_index = 0;
        ts.count = 0;
    }
    for (i = 0; i < COUNTOF(mp); i++)
    {
        ts.cookie = &mp[i];
        framer_symbol_filter_t_init(&filter);
        filter.name = arg2;
        filter.flags = FRAMER_FILTER_LOOP | FRAMER_FILTER_CONT_ON_ERR;
        filter.cb = testcase_50_do;
        filter.user_data = &ts;
        filter.summary = "TR 50";
        SHR_IF_ERR_EXIT(framer_symbol_filter(unit, FRAMER_REGSFILE_F_MEM | flags, &filter));
        failures += filter.stat.failure;
    }
    SHR_IF_ERR_EXIT_NO_MSG(failures ? _SHR_E_FAIL : _SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

static int
testcase_141_do_memories(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_NO_MSG(testcase_50_do(unit, flags, sym, user_data));
exit:
    SHR_FUNC_EXIT;
}

static int
testcase_141_do_registers(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    void *user_data)
{
    int w;
    test_sym_t *ts = (test_sym_t *) user_data;
    REG_INFO_S **sym_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (ts->flags & TEST_F_ALL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(testcase_3_do_filter(unit, flags, sym, user_data));
    }
    sym_info = sym->sinfo;
    for (w = 0; w < sym->count; w++)
    {
        SHR_IF_ERR_EXIT_NO_MSG(framer_reg_raw_write(unit, sym->base_addr,
                                                    sym_info[w]->offset_addr, 1, &sym_info[w]->default_value));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
testcase_141(
    int unit,
    args_t * a)
{
    uint32 flags = 0;
    char *arg2 = NULL, *arg3 = NULL;
    RST_TYPE init_type = FRAMER_RST_COLD;
    uint32 ext = 0;
    FRAMER_CHIP_S *sys_info = NULL;
    uint32 *base;
    framer_symbol_filter_t filter;
    test_sym_t ts;
    test_pattern_t mp[] = {
        {0, 0x00000000},
    };
    int failures = 0;

    SHR_FUNC_INIT_VARS(unit);

    arg2 = ARG_GET(a);
    arg3 = ARG_GET(a);
    if ((arg2) && (isint(arg2)))
    {
        init_type = parse_integer(arg2);
    }
    if ((arg3) && (isint(arg3)))
    {
        ext = parse_integer(arg3);
    }

    SHR_IF_ERR_EXIT(framer_sal_j2x_get_sys_info(unit, &sys_info));
    base = sys_info->chip_addr;
    SHR_IF_ERR_EXIT(framer_chip_uninit(unit));
    SHR_IF_ERR_EXIT(framer_chip_destory(unit));

    SHR_IF_ERR_EXIT(framer_chip_setup(unit, base));

    /*
     * Reinitialize memories to zero 
     */
    sal_memset(&ts, 0, sizeof(test_sym_t));
    ts.flags |= TEST_F_ALL;
    ts.start_index = 0;
    ts.count = 0;
    ts.cookie = &mp[0];
    framer_symbol_filter_t_init(&filter);
    filter.name = "*";
    filter.flags = FRAMER_FILTER_LOOP | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = testcase_141_do_memories;
    filter.user_data = &ts;
    filter.summary = "TR 141 Memories";
    SHR_IF_ERR_EXIT(framer_symbol_filter(unit, FRAMER_REGSFILE_F_MEM | flags, &filter));
    failures += filter.stat.failure;
    SHR_IF_ERR_EXIT_NO_MSG(failures ? _SHR_E_FAIL : _SHR_E_NONE);

    /*
     * Reinitialize registers to default value 
     */
    sal_memset(&ts, 0, sizeof(test_sym_t));
    ts.flags |= TEST_F_ALL;
    framer_symbol_filter_t_init(&filter);
    filter.name = "*";
    filter.flags = FRAMER_FILTER_LOOP | FRAMER_FILTER_CONT_ON_ERR;
    filter.cb = testcase_141_do_registers;
    filter.user_data = &ts;
    filter.summary = "TR 141 Registers";
    SHR_IF_ERR_EXIT(framer_symbol_filter(unit, FRAMER_REGSFILE_F_REG | flags, &filter));
    failures += filter.stat.failure;
    SHR_IF_ERR_EXIT_NO_MSG(failures ? _SHR_E_FAIL : _SHR_E_NONE);

    SHR_IF_ERR_EXIT(framer_chip_init(unit, init_type, ext));
exit:
    SHR_FUNC_EXIT;
}

static struct test_s
{
    int test_id;
    int (
    *test_func) (
    int unit,
    args_t * a);
    char *test_desc;
} test_cases[] =
{
    {
    1, testcase_1, "Register reset defaults"},
    {
    3, testcase_3, "Register read/write"},
    {
    50, testcase_50, "Memory Fill/Verify"},
    {
141, testcase_141, "Framer reinit"},};

static int test_cases_cnt = COUNTOF(test_cases);

static int
testrun(
    int unit,
    int test_id,
    args_t * a)
{
    int i, rv = _SHR_E_NOT_FOUND;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < test_cases_cnt; i++)
    {
        if (test_id == -1 || test_id == test_cases[i].test_id)
        {
            cli_out("TR %d [%s] is running ...\n", test_cases[i].test_id, test_cases[i].test_desc);
            rv = test_cases[i].test_func(unit, a);
            cli_out("TR %d [%s] %s\n",
                    test_cases[i].test_id, test_cases[i].test_desc, SHR_SUCCESS(rv) ? "PASSED" : "FAILED");
        }
    }
    SHR_IF_ERR_EXIT_NO_MSG(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:    test_run
 * Purpose:     Perform "run" command for test.
 * Parameters:  none - Run all selected tests with loaded parameters.
 *              "test [options]"
 * Returns:     Test result.
 */

cmd_result_t
cmd_testrun(
    int unit,
    args_t * a)
{
    int rv = CMD_OK;
    char *arg1 = NULL;
    int test_id = 0;
    uint32 dflen;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == sh_check_attached(ARG_CMD(a), unit))
    {
        SHR_IF_ERR_EXIT(CMD_FAIL);
    }
    arg1 = ARG_GET(a);
    if (arg1 && isint(arg1))
    {
        test_id = parse_integer(arg1);
    }
    else
    {
        dflen = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
        if (arg1 && sal_strncasecmp(arg1, "all", sal_strnlen(arg1, dflen)) == 0)
        {
            test_id = -1;
        }
        else
        {
            SHR_IF_ERR_EXIT_NO_MSG(CMD_USAGE);
        }
    }
    rv = testrun(unit, test_id, a);
    if (rv < 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(CMD_FAIL);
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /* NO_FRAMER_LIB_BUILD */
