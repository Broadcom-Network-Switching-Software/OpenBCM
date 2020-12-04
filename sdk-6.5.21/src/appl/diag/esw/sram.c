/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Triumph external SRAM CLI command 
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH_SUPPORT)

#include <appl/diag/system.h>
#include <bcm/error.h>
#include <soc/triumph.h>

static int
parse_string_backwards(char *end, uint32 *value, int *adj)
{
    int i, shift = 0;

    *value = 0;
    *adj = 0;

    for (i = 0; i < 6; i++) {
        char ch = *--end;
        int nibble =
            ((ch >= 'a') && (ch <= 'f'))? (ch - 'a' + 10):
            ((ch >= 'A') && (ch <= 'F'))? (ch - 'A' + 10):
            ((ch >= '0') && (ch <= '9'))? (ch - '0'):
            -1;
        if (nibble == -1) {
            if (i < 4) {
                return -1;
            } else if (i==4) {
              uint32 hi, lo;
              int j, phi=0, plo=0;

              lo = *value & 0xff;
              hi = (*value >> 8) & 0xff;
              for (j = 0; j < 8; j++) {
                   phi ^= ((hi >> j) & 1);
                   plo ^= ((lo >> j) & 1);
              }
              *value = (lo) | (plo << 8) | (hi << 9) | (phi << 17);
              return 1;
            } else if ((i==5) && (ch == '_')) {
              *adj += 1;
              return 0;
            }
        }
        *value |= (nibble << shift);
        shift += 4;
    }
    return 0;
}

static int
parse_72_bits_to_4_words(char *s, uint32 *value)
{
    char * end = s + sal_strlen(s);
    int sum = 0;
    int i, adj, rv;

    if (sal_strlen(s) == 0) {
        return -1;
    }

    for (i = 0; i < 4; i++) {
         if ((rv = parse_string_backwards(end, &value[i], &adj)) < 0) {
             return -1;
         }
         if (rv==1) sum += 1;
         value[i] &= 0x3ffff;
         end = end - 5 - adj;
    }

    if ((sum != 0) && (sum != 4)) return -1;

    return 0;
}

char cmd_sram_usage[] =
    "Usages:\n\t"
    "  sram hse|cse <addr0> <data0>\n\t"
    "               [addr1] [data1] [Testmode] [Adrmode] [Latency] [W2R] [R2W]\n\t"
    "               [wdoebr] [wdoebf] [wdmr] [wdmf] [rdmr] [rdmf]\n\n\t"
    "       - Test external SRAM\n"
;

cmd_result_t
cmd_sram(int unit, args_t *a)
{
    char *subcmd;
    parse_table_t pt;
    char *ds0 = NULL, *ds1 = NULL;
    int rv, mode = 0;
    int addr0 = -1, addr1 = -1;
    int test_mode = 3;
    int adr_mode = 2; 
    int late = 0, w2r = 0, r2w = 0;
    int wdoebr = 0, wdoebf = 0, wdmr = 0, wdmf = 0, rdmr = 0, rdmf = 0;
#ifdef BCM_TRIUMPH_SUPPORT
    tr_ext_sram_bist_t tr_sram_bist;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_esm_support)) {
        cli_out("Command only valid for BCM566xx devices\n");
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (soc_feature(unit, soc_feature_esm_support)) {
        if (!sal_strcasecmp(subcmd, "es0")) {
            mode = 0;
        } else if (!sal_strcasecmp(subcmd, "es1")) {
    /*    coverity[assignment]    */
            mode = 1;
        } else {
            return CMD_USAGE;
        }
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Testmode", PQ_DFL|PQ_HEX, 0, &test_mode, NULL);
    parse_table_add(&pt, "Adrmode", PQ_DFL|PQ_HEX, 0, &adr_mode, NULL);
    parse_table_add(&pt, "Addr0", PQ_DFL|PQ_HEX, 0, &addr0, NULL);
    parse_table_add(&pt, "Addr1", PQ_DFL|PQ_HEX, 0, &addr1, NULL);
    parse_table_add(&pt, "Data0", PQ_DFL|PQ_STRING, 0, &ds0, NULL);
    parse_table_add(&pt, "Data1", PQ_DFL|PQ_STRING, 0, &ds1, NULL);
    parse_table_add(&pt, "Latency", PQ_DFL|PQ_INT, 0, &late, NULL);
    parse_table_add(&pt, "W2R", PQ_DFL|PQ_INT, 0, &w2r, NULL);
    parse_table_add(&pt, "R2W", PQ_DFL|PQ_INT, 0, &r2w, NULL);
    parse_table_add(&pt, "WDOEBR", PQ_DFL|PQ_INT, 0, &wdoebr, NULL);
    parse_table_add(&pt, "WDOEBF", PQ_DFL|PQ_INT, 0, &wdoebf, NULL);
    parse_table_add(&pt, "WDMR", PQ_DFL|PQ_INT, 0, &wdmr, NULL);
    parse_table_add(&pt, "WDMF", PQ_DFL|PQ_INT, 0, &wdmf, NULL);
    parse_table_add(&pt, "RDMR", PQ_DFL|PQ_INT, 0, &rdmr, NULL);
    parse_table_add(&pt, "RDMF", PQ_DFL|PQ_INT, 0, &rdmf, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s ERROR: parsing arguments\n", ARG_CMD(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if ((ds0 == NULL) || (addr0 == -1)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        uint32 data[4];
        sal_memset(&tr_sram_bist, 0, sizeof(tr_ext_sram_bist_t));

        rv = soc_triumph_ext_sram_enable_set(unit, mode, TRUE, TRUE);
        if (rv < 0) {
            cli_out("Fail to enable LTE for test.\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    /*    coverity[uninit_use_in_call : FALSE]    */

    /* coverity[uninit_use_in_call] */
        if ((rv = parse_72_bits_to_4_words(ds0, &data[0])) != 0) {
           cli_out("Wrong data input format, see example below:\n\n");
           cli_out("Use data0=12345_0abcd_12345_0abcd for raw data,\n");
           cli_out("or  data0=1234_abcd_1234_abcd for inserting");
           cli_out(" 1 parity bit/byte.\n");
           parse_arg_eq_done(&pt);
           return CMD_FAIL;
        }
        tr_sram_bist.d0f_0 = data[0];
        tr_sram_bist.d0f_1 = data[1];
        tr_sram_bist.d0r_0 = data[2];
        tr_sram_bist.d0r_1 = data[3];
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Program data register ES0_DTU_LTE_D0:\n")));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "D0R_1 = 0x%x, D0R_0 = 0x%x, D0F_1 = 0x%x, D0F_0 = 0x%x\n"),
                  tr_sram_bist.d0r_1, tr_sram_bist.d0r_0,
                  tr_sram_bist.d0f_1, tr_sram_bist.d0f_1));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Program address register CSE_DTU_LTE_ADR0:\n")));
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "DTU_LTE_ADR0 = 0x%x\n"), (addr0 & 0xfffff)));
        if (ds1 && (addr1 != -1)) {
            if ((rv = parse_72_bits_to_4_words(ds1, data)) != 0) {
                cli_out("Wrong data input format, see example below:\n\n");
                cli_out("Use data1=12345_0abcd_12345_0abcd for raw data,\n");
                cli_out("or  data1=1234_abcd_1234_abcd for inserting");
                cli_out(" 1 parity bit/byte.\n");
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
             }
            tr_sram_bist.d1f_0 = data[0];
            tr_sram_bist.d1f_1 = data[1];
            tr_sram_bist.d1r_0 = data[2];
            tr_sram_bist.d1r_1 = data[3];
             LOG_INFO(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Program data register CSE_DTU_LTE_D1:\n")));
             LOG_INFO(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "D1R_1 = 0x%x, D1R_0 = 0x%x, D1F_1 = 0x%x, D1F_0 = 0x%x\n"),
                       tr_sram_bist.d1r_1, tr_sram_bist.d1r_0,
                       tr_sram_bist.d1f_1, tr_sram_bist.d1f_0));
             LOG_INFO(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Program address register CSE_DTU_LTE_ADR1:\n")));
             LOG_INFO(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "DTU_LTE_ADR1 = 0x%x\n"), (addr1 & 0xfffff)));
        }

        tr_sram_bist.adr0 = addr0;
        tr_sram_bist.adr1 = addr1;
        tr_sram_bist.loop_mode = test_mode;
        tr_sram_bist.adr_mode = adr_mode;
        tr_sram_bist.em_latency = late;
        tr_sram_bist.w2r_nops = w2r;
        tr_sram_bist.r2w_nops = r2w;
        tr_sram_bist.wdoebr = wdoebr;
        tr_sram_bist.wdoebf = wdoebf;
        tr_sram_bist.wdmr = wdmr;
        tr_sram_bist.wdmf = wdmf;
        tr_sram_bist.rdmr = rdmr;
        tr_sram_bist.rdmf = rdmf;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    parse_arg_eq_done(&pt);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Start the test with...\n")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "test_mode = %d, adr_mode = %d, latency7 = %d\n"),
              (test_mode & 3), (adr_mode & 3), late & 7));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "wdoebr = 0x%x, wdoebf = 0x%x wdmr = 0x%x\n"),
              wdoebr, wdoebf, wdmr));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "wdmf = 0x%x, rdmr = 0x%x rdmf = 0x%x\n"),
              wdmf, rdmr, rdmf));

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        soc_triumph_ext_sram_bist_setup(unit, mode, &tr_sram_bist);
        rv = soc_triumph_ext_sram_op(unit, mode, &tr_sram_bist, NULL);
        if (rv < 0) {
            cli_out("Fail to run LTE %s test.\n", subcmd);
            (void) soc_triumph_ext_sram_enable_set(unit, mode, FALSE, FALSE);
            return CMD_FAIL;
        }
        if ((tr_sram_bist.err_cnt != 0) || (tr_sram_bist.err_bitmap != 0)) {
            cli_out("unit %d: %s ext SRAM has %d errors with bitmap 0x%02x\n",
                    unit, subcmd, tr_sram_bist.err_cnt,
                    tr_sram_bist.err_bitmap);
            cli_out("Error Address: 0x%x\n", tr_sram_bist.err_adr);
            cli_out("Error Data:\n");
            cli_out("ERR_DR_1=0x%x ERR_DR_0=0x%x ERR_DF_1=0x%x ERR_DF_0=0x%x\n",
                    tr_sram_bist.err_dr_1, tr_sram_bist.err_dr_0,
                    tr_sram_bist.err_df_1, tr_sram_bist.err_df_0);
        } else {
            cli_out("%s LTE test finished successfully\n", subcmd);
        }

        if ((rv = soc_triumph_ext_sram_enable_set(unit, mode, FALSE, FALSE)) <
            0) {
            cli_out("Fail to disable LTE after test.\n");
            (void) soc_triumph_ext_sram_enable_set(unit, mode, FALSE, FALSE);
            return CMD_FAIL;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return CMD_OK;
}

#endif /* BCM_TRIUMPH_SUPPORT */
