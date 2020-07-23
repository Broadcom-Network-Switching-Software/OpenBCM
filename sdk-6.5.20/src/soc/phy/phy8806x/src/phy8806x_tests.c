/*******************************************************************************
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <soc/error.h>
#include "phy8806x_defs.h"
#include "phy8806x_funcs.h"

int _mt2_reset_all_blocks(int unit, uint16 phyaddr)
{
    mt2_sym_t *phy8806xsymbs;
    uint32 mt2_data[2];

    /* Reset the core */    
    phy8806xsymbs = mt2_syms_find_name("TOP_SOFT_RESET_REGr");
    mt2_sbus_reg_read(unit, phyaddr, -1, phy8806xsymbs, mt2_data);
    mt2_data[0] = 0;
    mt2_sbus_reg_write(unit, phyaddr, -1, phy8806xsymbs, mt2_data);
    sal_usleep(1000000);
    mt2_data[0] = 0xffffffff;
    mt2_sbus_reg_write(unit, phyaddr, -1, phy8806xsymbs, mt2_data);

    /* Bring CLPORT Blocks out of reset */
    phy8806xsymbs = mt2_syms_find_name("CLPORT_MAC_CONTROLr");
    mt2_sbus_reg_read(unit, phyaddr, -1, phy8806xsymbs, mt2_data);
    mt2_data[0] &= 0xfffffffe;
    mt2_sbus_reg_write(unit, phyaddr, -1, phy8806xsymbs, mt2_data);

    return SOC_E_NONE;
}

int mt2_test_run_1(int unit, uint16 phyaddr, int silent)
{
    int    err, rv = SOC_E_NONE;
    uint32 mt2_data[2];
    int i, skipped=0, passed=0, failed=0;

    _mt2_reset_all_blocks(unit, phyaddr);
    sal_usleep(1000000);

    for (i=0; i<phy8806x_syms_numels; i++)
    {
        if (phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_MEMORY) {
             continue;
        }

        if (phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_NOTEST) {
            skipped++;
            continue;
        }

        err = mt2_sbus_reg_read(unit, phyaddr, -1, (mt2_sym_t *) &phy8806x_syms[i], mt2_data);
        if (err == SOC_E_TIMEOUT) {
            if (silent == 0) {
                LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "%s - Access Timeout\n"),
                          phy8806x_syms[i].name));
            }
            failed++;
        }
        else if ((mt2_data[0] != phy8806x_syms[i].rstval) || 
            (mt2_data[1] != phy8806x_syms[i].rstval_hi)) {
            if (silent == 0) {             
                LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                            "%s - Read  0x%08x%08x  Expected 0x%08x%08x\n"),
                            phy8806x_syms[i].name, mt2_data[1], mt2_data[0],
                            phy8806x_syms[i].rstval_hi, phy8806x_syms[i].rstval));
            }
            failed++;
        } else {
            passed++;
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit, "TR 1 passed %d, failed %d, skipped %d\n"),
                              passed, failed, skipped));
    return rv;
}

int _mt2_reg_rw_test(int unit, uint16 phyaddr, mt2_sym_t *mt2_sym,
            uint32 * pattern, int silent)
{
    int err, rv = SOC_E_NONE;
    uint32 mt2_data[2];

    err = mt2_sbus_reg_write(unit, phyaddr, -1, mt2_sym, pattern);
    if (err == SOC_E_TIMEOUT) {
        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "%s - Write Timed Out\n"),
                      mt2_sym->name));
        }
        return SOC_E_FAIL;
    }

    mt2_data[0] = 0;
    mt2_data[1] = 0;
    err = mt2_sbus_reg_read(unit, phyaddr, -1, mt2_sym , mt2_data);
    if (err == SOC_E_TIMEOUT) {
        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "%s - Read Timed Out\n"),
                      mt2_sym->name));
        }
        return SOC_E_FAIL;
    }

    mt2_data[0] &= mt2_sym->test_mask;
    mt2_data[1] &= mt2_sym->test_mask_hi;

    if (mt2_sym->flags & MT2_SYMBOL_FLAG_R64) {
        if ((mt2_data[0] != pattern[0]) || (mt2_data[1] != pattern[1])) {
            if (silent == 0) {             
                LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                            "%s - Read  0x%08x%08x  Expected 0x%08x%08x\n"),
                            mt2_sym->name, mt2_data[1], mt2_data[0], pattern[1], pattern[0]));
            }
            rv = SOC_E_FAIL;
        }
    } else {
        if (mt2_sym->index == 2) {
            mt2_data[0] &= 0xffff;
            pattern[0] &= 0xffff;
        }
        if (mt2_data[0] != pattern[0]) {
            if (silent == 0) {             
                LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                            "%s - Read  0x%08x  Expected 0x%08x\n"),
                            mt2_sym->name, mt2_data[0], pattern[0]));
            }
            rv = SOC_E_FAIL;
        }
    }
    return rv;
}

int mt2_test_run_3(int unit, uint16 phyaddr, int silent)
{
    int    rv = SOC_E_NONE;
    uint32 saved_data[2];
    uint32 mt2_pattern[2];
    int i, fail, skipped=0, passed=0, failed=0;

    _mt2_reset_all_blocks(unit, phyaddr);
    sal_usleep(1000000);

    for (i=0; i<phy8806x_syms_numels; i++)
    {
        if (phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_MEMORY) {
             continue;
        }

        if (phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_NOTEST ||
            phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_READONLY) {
            skipped++;
            continue;
        }

        mt2_sbus_reg_read(unit, phyaddr, -1, (mt2_sym_t *) &phy8806x_syms[i], saved_data);

        /* All 0's Test */
        mt2_pattern[0] = 0;
        mt2_pattern[1] = 0;
        rv = _mt2_reg_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], mt2_pattern, silent);
        fail = (rv == SOC_E_FAIL);

        /* all 1's Test */
        mt2_pattern[0] = 0xffffffff & phy8806x_syms[i].test_mask;
        mt2_pattern[1] = 0xffffffff & phy8806x_syms[i].test_mask_hi;
        rv = _mt2_reg_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], mt2_pattern, silent);
        fail |= (rv == SOC_E_FAIL);

        /* Alternating 1 & 0 Test */
        mt2_pattern[0] = 0x55555555 & phy8806x_syms[i].test_mask;
        mt2_pattern[1] = 0x55555555 & phy8806x_syms[i].test_mask_hi;
        rv = _mt2_reg_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], mt2_pattern, silent);
        fail |= (rv == SOC_E_FAIL);

        mt2_pattern[0] = 0xAAAAAAAA & phy8806x_syms[i].test_mask;
        mt2_pattern[1] = 0xAAAAAAAA & phy8806x_syms[i].test_mask_hi;
        rv = _mt2_reg_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], mt2_pattern, silent);
        fail |= (rv == SOC_E_FAIL);

        if (fail) {
            failed++;
        } else {
            passed++;
        }

        /* Restore Data */
        mt2_sbus_reg_write(unit, phyaddr, -1, (mt2_sym_t *) &phy8806x_syms[i], saved_data);
    }
    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit, "TR 3 passed %d, failed %d, skipped %d\n"),
                              passed, failed, skipped));

    return (failed == 0) ? SOC_E_NONE : SOC_E_FAIL;
}

int _mt2_mem_rw_test(int unit, uint16 phyaddr, mt2_sym_t *mt2_sym,
            int idx, uint32 pattern, int silent)
{
    int rv = SOC_E_NONE;
    uint32 mt2_data[4];
    uint32 pat_data[4];

    pat_data[0] = pat_data[1] = pat_data[2] = pat_data[3] = pattern;

    mt2_sbus_mem_write(unit, phyaddr, -1, mt2_sym, idx, pat_data);

    mt2_data[0] = mt2_data[1] = mt2_data[2] = mt2_data[3] = 0;
    mt2_sbus_mem_read(unit, phyaddr, -1, mt2_sym , idx, mt2_data);

    if ((mt2_data[0] != pat_data[0]) || (mt2_data[1] != pat_data[1]) ||
        (mt2_data[2] != pat_data[2]) || (mt2_data[3] != pat_data[3])) {
        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "\n%s[%d] - Read  0x%08x  Expected 0x%08x "),
                        mt2_sym->name, idx, mt2_data[0], pat_data[0]));
        }
        rv = SOC_E_FAIL;
    } else {
        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, ".")));
        }
    }

    return rv;
}

int mt2_test_run_50(int unit, uint16 phyaddr, int silent)
{
    int    rv = SOC_E_NONE;
    int i, j, fail=0, skipped=0, passed=0, failed=0;

    for (i=0; i<phy8806x_syms_numels; i++)
    {
        if (!(phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_MEMORY)) {
            /* Not a Memory */
            continue;
        }
        if (phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_NOTEST ||
            phy8806x_syms[i].flags & MT2_SYMBOL_FLAG_READONLY) {
            skipped++;
            continue;
        }

        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "\nMemory %s - Min %d, Max %d\n"),
                    phy8806x_syms[i].name,
                    MT2_SYMBOL_INDEX_MIN_DECODE(phy8806x_syms[i].index),
                    MT2_SYMBOL_INDEX_MAX_DECODE(phy8806x_syms[i].index)));
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Pattern : All 0's\n")));
        }
        for (j = MT2_SYMBOL_INDEX_MIN_DECODE(phy8806x_syms[i].index);
             j < MT2_SYMBOL_INDEX_MAX_DECODE(phy8806x_syms[i].index);
             j++) {
            rv = _mt2_mem_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], j, 0, silent);
            fail = (rv == SOC_E_FAIL);
        }

        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "\nPattern : All 1's\n")));
        }
        for (j = MT2_SYMBOL_INDEX_MIN_DECODE(phy8806x_syms[i].index);
             j < MT2_SYMBOL_INDEX_MAX_DECODE(phy8806x_syms[i].index);
             j++) {
            rv = _mt2_mem_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], j, 0xffffffff, silent);
            fail |= (rv == SOC_E_FAIL);
        }
        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Pattern : 5555's\n")));
        }
        for (j = MT2_SYMBOL_INDEX_MIN_DECODE(phy8806x_syms[i].index);
             j < MT2_SYMBOL_INDEX_MAX_DECODE(phy8806x_syms[i].index);
             j++) {
            rv = _mt2_mem_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], j, 0x55555555, silent);
            fail |= (rv == SOC_E_FAIL);
        }

        if (silent == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Pattern : AAAA's\n")));
        }
        for (j = MT2_SYMBOL_INDEX_MIN_DECODE(phy8806x_syms[i].index);
             j < MT2_SYMBOL_INDEX_MAX_DECODE(phy8806x_syms[i].index);
             j++) {
            rv = _mt2_mem_rw_test(unit, phyaddr, (mt2_sym_t *) &phy8806x_syms[i], j, 0xAAAAAAAA, silent);
            fail |= (rv == SOC_E_FAIL);
        }

        if (fail) {
            failed++;
        } else {
            passed++;
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,"\nTR 50 passed %d, failed %d, skipped %d\n"),
                          passed, failed, skipped));

    return (failed == 0) ? SOC_E_NONE : SOC_E_FAIL;
}

int mt2_test_run(int unit, uint16 phyaddr, uint32 testno, int silent)
{
    int    rv = SOC_E_NONE;

    LOG_WARN(BSL_LS_SOC_PHY, (BSL_META_U(unit, 
                "WARNING : Running TR Tests on MT2 will leave it in Unpredictable state\n")));
    LOG_WARN(BSL_LS_SOC_PHY, (BSL_META_U(unit, 
                "          and needs to be reset/restarted\n")));

    if (testno == 1) {
        return mt2_test_run_1(unit, phyaddr, silent);
    } else if (testno == 3) {
        return mt2_test_run_3(unit, phyaddr, silent);
    } else if (testno == 50) {
        return mt2_test_run_50(unit, phyaddr, silent);
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "TR Test %d at phy addr 0x%02x\n"),
                    testno, phyaddr));
    }

    return rv;
}

int mt2_diag_tempmon(int unit, uint16 phy_addr, int resetmax)
{
    int    rv = SOC_E_NONE;
    mt2_sym_t *phy8806xsymbs;
    uint32 mt2_data[2];
    uint32 regval[2];
    int fval, curval, minval, maxval, avg_cur, avg_min, avg_max;

    phy8806xsymbs = mt2_syms_find_name("TOP_PVTMON_CTRL_1r");
    mt2_sbus_reg_read(unit, phy_addr, -1, phy8806xsymbs, mt2_data);
    mt2_data[0] &= 0xc7ffffff;  /* set PVTMON_SELECTf to 0 */
    mt2_sbus_reg_write(unit, phy_addr, -1, phy8806xsymbs, mt2_data);

    sal_usleep(1000);

    phy8806xsymbs = mt2_syms_find_name("TOP_PVTMON_RESULT_0r");
    mt2_sbus_reg_read(unit, phy_addr, -1, phy8806xsymbs, mt2_data);
    regval[0] = mt2_data[0];

    phy8806xsymbs = mt2_syms_find_name("TOP_PVTMON_RESULT_1r");
    mt2_sbus_reg_read(unit, phy_addr, -1, phy8806xsymbs, mt2_data);
    regval[1] = mt2_data[0];

    if (resetmax)
    {
        cli_out ("Resetting Min & Max Values\n");
        phy8806xsymbs = mt2_syms_find_name("TOP_SOFT_RESET_REG_2r");
        mt2_sbus_reg_read(unit, phy_addr, -1, phy8806xsymbs, mt2_data);
        mt2_data[0] &= 0xffffff3f;  /* set TOP_PVT_MON_MIN/MAX_RST_Lf to 0 */
        mt2_sbus_reg_write(unit, phy_addr, -1, phy8806xsymbs, mt2_data);
        mt2_data[0] |= 0xc0;    /* set TOP_PVT_MON_MIN/MAX_RST_Lf to 1 */
        mt2_sbus_reg_write(unit, phy_addr, -1, phy8806xsymbs, mt2_data);
    }

    /* PvtMon 0 */
    fval = (regval[0] & 0x3ff);
    curval = (4100400-(fval*4870))/1000;
    avg_cur = curval;
    fval = ((regval[0] >> 12) & 0x3ff);
    minval = (4100400-(fval*4870))/1000;
    avg_min = minval;
    fval = ((regval[0] >> 22) & 0x3ff);
    maxval = (4100400-(fval*4870))/1000;
    avg_max = maxval;
    cli_out("temperature monitor 0: current=%d.%d, peak=%d.%d, min=%d.%d\n",
            curval/10,
            curval >=0? curval%10:(-curval)%10, /* remove minus sign */
            minval/10,
            minval >=0? minval%10:(-minval)%10, /*remove minus sign */
            maxval/10,
            maxval >=0? maxval%10:(-maxval)%10); /*remove minus sign */

    /* PvtMon 1 */
    fval = (regval[1] & 0x3ff);
    curval = (4100400-(fval*4870))/1000;
    avg_cur = (avg_cur + curval)/2;
    fval = ((regval[1] >> 12) & 0x3ff);
    minval = (4100400-(fval*4870))/1000;
    avg_min = (avg_min + minval)/2;
    fval = ((regval[1] >> 22) & 0x3ff);
    maxval = (4100400-(fval*4870))/1000;
    avg_max = (avg_max + maxval)/2;
    cli_out("temperature monitor 1: current=%d.%d, peak=%d.%d, min=%d.%d\n",
            curval/10,
            curval >=0? curval%10:(-curval)%10, /* remove minus sign */
            minval/10,
            minval >=0? minval%10:(-minval)%10, /*remove minus sign */
            maxval/10,
            maxval >=0? maxval%10:(-maxval)%10); /*remove minus sign */

    /* Average */
    cli_out("average current temperature is %d.%d\n",
            avg_cur/10,
            avg_cur >=0? avg_cur%10:(-avg_cur)%10); /* remove minus sign */
    cli_out("maximum peak temperature is %d.%d\n",
            avg_min/10,
            avg_min >=0? avg_min%10:(-avg_min)%10); /*remove minus sign */
    cli_out("minimum temperature is %d.%d\n",
            avg_max/10,
            avg_max >=0? avg_max%10:(-avg_max)%10); /*remove minus sign */

    return rv;
}

int mt2_diag_run(int unit, uint16 phy_addr, char *cmd_str, char *para)
{
    int    val,rv = SOC_E_NONE;

    if (sal_strcasecmp(cmd_str, "tempmon") == 0) {
        val = (para && (sal_strcasecmp(para, "resetpeaks") == 0)) ? 1 : 0;
        return mt2_diag_tempmon(unit, phy_addr, val);
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                  "Yet to Implement Diag %s\n"),
                  cmd_str));
    }
    return rv;
}

