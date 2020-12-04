/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI IPFIX command
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/format.h>

#include <bcm/error.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_export.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

static int ftmon_report_count[BCM_MAX_NUM_UNITS] = { 0 };
static int ftmon_is_running[BCM_MAX_NUM_UNITS] = { 0 };
static uint32 ftmon_report[BCM_MAX_NUM_UNITS] = { 0 };

extern int bcma_ft_ipfix_print_record(int unit,
                                      int set_c,
                                      int payload_length,
                                      uint8 *payload_data);

typedef struct ft_cache_s {
    int record_count;
    bcm_collector_export_record_t record;
} ft_cache_t;

/* FT record reasons. */
#define FT_RECORD_REASON_ALU16          0x1
#define FT_RECORD_REASON_ALU32          0x2
#define FT_RECORD_REASON_STATE_TRANS    0x4
#define FT_RECORD_REASON_LEARN          0x8
#define FT_RECORD_REASON_AGED           0x10
#define FT_RECORD_REASON_FLUSH          0x20
#define FT_RECORD_REASON_PERIODIC       0x40


typedef struct ft_record_info_s {
    int ft_group;
    int ft_index;
    int key_len;
    int data_len;
    uint32 ft_reason;
    uint8 ft_key[64];
    uint8 ft_data[64];
} ft_record_info_t;

ft_cache_t *ft_record_cache[BCM_MAX_NUM_UNITS];

#define REPORT_COUNT      0x01   /* Report export count */
#define REPORT_DECODE     0x02   /* Report Decoded export data */
#define REPORT_CACHE      0x04   /* Report Start Caching export data */
#define REPORT_DUMP       0x08   /* Report Dump cached record data */

#define REPORT_CACHE_ENABLED  0x10

static const parse_pm_t ftmon_report_table[] = {
    {"Count",     REPORT_COUNT},
    {"Decode",    REPORT_DECODE},
    {"Cache",     REPORT_CACHE},
    {"Dump",      REPORT_DUMP},
    {"@ALL",      ~0},
    {"@*",        ~0},
    {NULL,        0}
};

uint8    record_data_record[] = {
            0xe1,
            0xfe,
            0x00,
            0x80,
            0x01,
            0x94,
            0x00,
            0x19,
            0x00,
            0x08,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x06,
            0x42,
            0x72,
            0x6f,
            0x61,
            0x19,
            0x32,
            0x00,
            0x00,
            0x1a,
            0x32,
            0x00,
            0x00,
            0x02,

            0x00,
            0xc9,
            0xff,
            0xff,
            0xde,
            0x00,
            0x00,
            0x00,
            0x00,
            0x44,
            0x00,
            0x00,
            0x00,
            0x01,
            0x00,
            0x2a,
            0x00,
            0x00,
            0x00,
            0x2a,
            0xff,
            0xff,
            0x00,
            0x00,
            0x00,
            0x00,
            0x06,
            0x00,
            0x01,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
    };




STATIC void
bcm_ftmon_export_entry_parse(int unit,
                             bcm_collector_export_record_t *record,
                             ft_record_info_t *record_info)
{
    int ix, rec_words, fld_len;
    soc_field_t fld;
    soc_format_t fmt;
    uint32 fldbuf[32] = {0}, entbuf[32] = {0}, *record_p;


    rec_words = 128 / 4;

    record_p = (uint32 *)(&(record->data_record[0]));
    for (ix = 0; ix < (rec_words); ix++) {

        entbuf[rec_words - ix - 1] = record_p[ix];
    }

    fmt = BSC_EX_EXPORT_RECORDfmt;

    sal_memset(fldbuf, 0, 128);
    fld = GROUP_IDf;
    record_info->ft_group = soc_format_field32_get(unit, fmt, entbuf, fld);

    sal_memset(fldbuf, 0, 128);
    fld = FLOW_INDEXf;
    record_info->ft_index = soc_format_field32_get(unit, fmt, entbuf, fld);

    sal_memset(fldbuf, 0, 128);
    fld = EXPORT_REASONf;
    record_info->ft_reason = soc_format_field32_get(unit, fmt, entbuf, fld);

    sal_memset(fldbuf, 0, 128);
    fld = FLOW_KEYf;
    fld_len = soc_format_field_length(unit, fmt, fld);
    soc_format_field_get(unit, fmt, entbuf, fld, fldbuf);
    record_info->key_len = (fld_len / 8);
    sal_memcpy((record_info->ft_key), fldbuf, (fld_len / 8));

    sal_memset(fldbuf, 0, 128);
    fld = FLOW_DATAf;
    fld_len = soc_format_field_length(unit, fmt, fld);
    soc_format_field_get(unit, fmt, entbuf, fld, fldbuf);
    record_info->data_len = (fld_len / 8);
    sal_memcpy((record_info->ft_data), fldbuf, (fld_len / 8));

    return;
}

#define LINE_DEMARC cli_out("**************************************************\n")


STATIC void
bcm_ftmon_export_entry_dump(int unit,
                            int count,
                            bcm_collector_export_record_t *record,
                            ft_record_info_t *record_info)
{
    int ix;
    uint8 *d, n;

    LINE_DEMARC;

    cli_out("FT Export Record: Flow Index:%d", record_info->ft_index);
    if (count > 0) {
        cli_out(" Num Exports:%d", count);
    }
    cli_out("\n");

    cli_out("Group Id:%d Flow Reasons:%d", record_info->ft_group, record_info->ft_reason);
    cli_out("\n");

    cli_out("Key:\n");
    n = record_info->key_len;
    d = &(record_info->ft_key[0]);

    for (ix = 0; ix < n; ix++) {

        if (ix == 0)
            cli_out("0x");

        cli_out("%02x", d[ix]);

        if (ix == (n - 1))
            cli_out("\n");
    }


    cli_out("Data:\n");
    n = record_info->data_len;
    d = &(record_info->ft_data[0]);

    for (ix = 0; ix < n; ix++) {

        if (ix == 0)
            cli_out("0x");

        cli_out("%02x", d[ix]);

        if (ix == (n - 1))
            cli_out("\n");
    }


    LINE_DEMARC;

    return;
}

int
bcm_ftmon_record_parse_test(int unit)
{
    bcm_collector_export_record_t record;
    ft_record_info_t record_info;

    sal_memcpy(record.data_record,record_data_record, 128);
    bcm_ftmon_export_entry_parse(unit, &record, &record_info);

    return 0;
}

bcm_collector_export_record_handle_t
ftmon_callback(int unit,
               bcm_collector_t collector_id,
               bcm_collector_export_record_t *record,
               void *userdata)
{
    ft_record_info_t record_info;

    /* Increment the global record count.  */
    ftmon_report_count[unit]++;

    /* If Caching is disabled, print count for each record.*/
    if ((ftmon_report[unit] & REPORT_COUNT) &&
        !(ftmon_report[unit] & REPORT_CACHE_ENABLED)) {
        cli_out("Export entry #%d.\n", ftmon_report_count[unit]);
    }

    /* If Caching is enabled, dump each record.*/
    if ((ftmon_report[unit] & REPORT_DUMP) &&
        !(ftmon_report[unit] & REPORT_CACHE_ENABLED)) {

        sal_memset(&(record_info), 0, sizeof(record_info));
        bcm_ftmon_export_entry_parse(unit, record, &record_info);

        bcm_ftmon_export_entry_dump(unit, 0, record, &record_info);
    }

    /* Decode record.*/
    if ((ftmon_report[unit] & REPORT_DECODE) &&
        !(ftmon_report[unit] & REPORT_CACHE_ENABLED)) {

        if (bcma_ft_ipfix_print_record(unit, 0, record->size, record->data_record)) {
            cli_out("ipfix parsing failed\n");
        }
    }

    /* If cache is enabled, cache the record.*/
    if(ftmon_report[unit] & REPORT_CACHE_ENABLED) {
        sal_memset(&(record_info), 0, sizeof(record_info));
        bcm_ftmon_export_entry_parse(unit, record, &record_info);

        ft_record_cache[unit][record_info.ft_index].record_count += 1;
        sal_memcpy(&(ft_record_cache[unit][record_info.ft_index].record),
                     record, sizeof(bcm_collector_export_record_t));
    }

    return BCM_FLOWTRACKER_EXPORT_RECORD_NOT_HANDLED;
}

bcm_flowtracker_export_record_handle_t
ftmon_ft_export_callback(int unit,
               bcm_flowtracker_collector_t collector_id,
               bcm_flowtracker_export_record_t *record,
               void *userdata)
{
    bcm_collector_export_record_handle_t record_handle;
    bcm_collector_t col_id;
    bcm_collector_export_record_t col_record;

    col_id = (bcm_collector_t) collector_id;
    sal_memcpy(col_record.data_record, record->data_record,
                sizeof (col_record.data_record));
    col_record.size = record->size;

    record_handle = ftmon_callback(unit, col_id, &col_record, userdata);

    return (bcm_flowtracker_export_record_handle_t) record_handle;
}

STATIC int
ftmon_cache_alloc(int unit, int enable)
{
    int max_flows;

    /* Max flows is size of single wide key table. */
    max_flows = soc_mem_index_count(unit, FT_KEY_SINGLEm);

    if (enable) {
        ft_record_cache[unit] = sal_alloc((sizeof(ft_cache_t) * max_flows),
                                          "FT records Cache");
        sal_memset(ft_record_cache[unit], 0, (sizeof(ft_cache_t) * max_flows));
        if (ft_record_cache[unit] == NULL) {
            return -1;
        }
    } else {
        sal_free(ft_record_cache[unit]);
        ft_record_cache[unit] = NULL;
    }

    return 0;
}
#endif /* BCM_FLOWTRACKER_SUPPORT */

char cmd_ftmon_usage[] =
    "Parameters: [report +/-<values>] [set <values>] [stop|start]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\t  start  - Start the FT export fifo deamon\n"
    "\t  stop   - Stop the FT export fifo deamon\n"
    "\t  report - Configure reporting to the console\n"
    "\t      count   - print export entry # only\n"
    "\t      decode  - dump decoded export entry\n"
    "\t      Prepend + to set, - to clear\n"
#endif
;

cmd_result_t
cmd_ftmon(int unit, args_t *arg)
{
#if defined(BCM_FLOWTRACKER_SUPPORT)
    int rv = BCM_E_NONE;
    char *subcmd = NULL;
    bcmi_ft_cb_entry_t entry;
    bcm_collector_t collector_id = BCMI_FT_COLLECTOR_ID_INVALID;
#endif /* BCM_FLOWTRACKER_SUPPORT */

    if (!soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        return (CMD_NFND);
    }

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (!ARG_CNT(arg)) {                  /* Nothing passed */
        cli_out("IPFIX Deamon Status: %s.\n",
                (ftmon_is_running[unit]) ? "Running" : "Not Running");

        cli_out("Reporting is enabled for: ");
        parse_mask_format(80, ftmon_report_table, ftmon_report[unit]);
        cli_out("Reporting is disabled for: ");
        parse_mask_format(80, ftmon_report_table, ftmon_report[unit] ^ ~0);

        cli_out("Number of records received: %d\n", ftmon_report_count[unit]);
        return(CMD_OK);
    }

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "start")) {
        if (ftmon_is_running[unit]) {
            cli_out("%s: FTMON thread already running.\n", ARG_CMD(arg));
            return CMD_OK;
        }
        ftmon_report_count[unit] = 0;
        ftmon_is_running[unit] = 1;

#if 0
        /* commented since collector id is dont care */
        rv = bcma_ft_export_dma_collector_get(unit, &collector_id);
        if (BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }
#endif
        sal_memset(&entry, 0, sizeof(entry));
        entry.callback = ftmon_ft_export_callback;

        rv = bcmi_ft_export_record_cb_install(unit,
                (bcm_flowtracker_collector_t) collector_id, 0, &entry);
        if (rv < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcma_ft_export_fifo_control(unit, 0, TRUE);
        if (rv < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "stop")) {
        if (!ftmon_is_running[unit]) {
            cli_out("%s: ERROR: FTMON thread already stopped.\n", ARG_CMD(arg));
            return CMD_FAIL;
        }

        rv = bcma_ft_export_dma_collector_get(unit, &collector_id);
        if (BCM_FAILURE(rv)) {
            collector_id = -1;
        }

        sal_memset(&entry, 0, sizeof(entry));
        entry.callback = ftmon_ft_export_callback;

        rv = bcmi_ft_export_record_cb_remove(unit,
                (bcm_flowtracker_collector_t) collector_id, 0, &entry);

        if (BCM_SUCCESS(rv)) {
            rv = bcma_ft_export_fifo_control(unit, 0, FALSE);
            if (BCM_FAILURE(rv)) {
                return CMD_FAIL;
            }
        }
        ftmon_is_running[unit] = 0;
    } else if (!sal_strcasecmp(subcmd, "report")) {
        if (ARG_CNT(arg)) {
            while ((subcmd = ARG_CUR(arg)) != NULL &&
                   !parse_mask(subcmd, ftmon_report_table, &ftmon_report[unit])) {
                ARG_NEXT(arg);        /* Bump arg if matched report */
            }

            /* Enable caching. */
            if ((ftmon_report[unit] & REPORT_CACHE) &&
                !(ftmon_report[unit] & REPORT_CACHE_ENABLED)) {
                if (ftmon_cache_alloc(unit, TRUE) != 0) {
                    return CMD_FAIL;
                }
                ftmon_report[unit] |= REPORT_CACHE_ENABLED;
            }

            /* Disable caching. */
            if (!(ftmon_report[unit] & REPORT_CACHE) &&
                (ftmon_report[unit] & REPORT_CACHE_ENABLED)) {
                if (ftmon_cache_alloc(unit, FALSE) != 0) {
                    return CMD_FAIL;
                }
                ftmon_report[unit] &= ~REPORT_CACHE_ENABLED;
            }

            /* Dump cached entries. */
            if ((ftmon_report[unit] & REPORT_DUMP) &&
                (ftmon_report[unit] & REPORT_CACHE_ENABLED)) {
                int ix;
                int max_flows;
                ft_record_info_t record_info;

                /* Max flows is size of single wide key table. */
                max_flows = soc_mem_index_count(unit, FT_KEY_SINGLEm);

                for (ix = 0; ix < max_flows; ix++) {
                    if ((ft_record_cache[unit][ix].record_count) > 0) {
                        bcm_ftmon_export_entry_parse(unit, &(ft_record_cache[unit][ix].record), &record_info);
                        bcm_ftmon_export_entry_dump(unit, ft_record_cache[unit][ix].record_count, &(ft_record_cache[unit][ix].record), &record_info);
                    }
                }
            }

        } else {                    /* Print values */
            cli_out("FTMON Reporting on for: ");
            parse_mask_format(50, ftmon_report_table, ftmon_report[unit]);
            cli_out("FTMON Reporting off for: ");
            parse_mask_format(50, ftmon_report_table, ~ftmon_report[unit]);
        }
    } else if (!sal_strcasecmp(subcmd, "dump")) {
            /* Dump cached entries. */
            if ((ftmon_report[unit] & REPORT_CACHE_ENABLED)) {
                int ix;
                int max_flows;
                ft_record_info_t record_info;

                /* Max flows is size of single wide key table. */
                max_flows = soc_mem_index_count(unit, FT_KEY_SINGLEm);

                for (ix = 0; ix < max_flows; ix++) {
                    if ((ft_record_cache[unit][ix].record_count) > 0) {
                        bcm_ftmon_export_entry_parse(unit, &(ft_record_cache[unit][ix].record), &record_info);
                        bcm_ftmon_export_entry_dump(unit, ft_record_cache[unit][ix].record_count, &(ft_record_cache[unit][ix].record), &record_info);
                    }
                }
            }


    } else {
        return CMD_USAGE;
    }
#endif

    return CMD_OK;
}
