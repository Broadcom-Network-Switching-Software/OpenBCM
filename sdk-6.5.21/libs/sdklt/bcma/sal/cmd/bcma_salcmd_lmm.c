/*! \file bcma_salcmd_lmm.c
 *
 * CLI command to display LMM memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_lmem_mgr.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/sal/bcma_salcmd_lmm.h>

static void
lmm_stats_header_format(shr_pb_t *pb)
{
    shr_pb_printf(pb, "Statistics in number of elements:\n");
    shr_pb_printf(pb, "C: elements number per Chunk, A: Allocated, I: In-use"
                      ", H: High-water-mark\nE: Element-size (in byte)\n");
    shr_pb_printf(pb, "%-25s %9s %5s %5s %5s %5s %4s %8s\n",
                  "ID", "Available", "C", "A", "I", "H", "E",
                  "Allocated-size");
    shr_pb_printf(pb, "=================================================="
                      "==============================\n");
}

static void
lmm_stats_format(shr_pb_t *pb, shr_lmm_stat_t *stats, size_t cnt)
{
    size_t idx = 0;

    do {
        uint32_t bytes, hbytes;
        char *hstr;

        bytes = (uint32_t)stats->element_size * stats->allocated;
        hbytes = bytes;
        hbytes /= 1024;
        hstr = "K";
        if (hbytes >= 1024) {
            hbytes /= 1024;
            hstr = "M";
        }

        shr_pb_printf(pb, "%-30s", stats->label);
        if ((stats->avail + stats->in_use) == 0xffffffff) {
            /* The maximum number of elements is undefined */
            shr_pb_printf(pb, " %4s", "n/a");
        } else {
            shr_pb_printf(pb, " %4"PRIu32, stats->avail);
        }
        shr_pb_printf(pb, " %5"PRIu32" %5"PRIu32" %5"PRIu32" %5"PRIu32,
                      (uint32_t)stats->chunk_size, stats->allocated,
                      stats->in_use, stats->hwtr_mark);
        shr_pb_printf(pb, " %4"PRIu32" %8"PRIu32" (%"PRIu32"%s)\n",
                      (uint32_t)stats->element_size, bytes, hbytes, hstr);
        stats++;
    } while (++idx < cnt);
}

static int
cmd_lmm_stats(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    shr_pb_t *pb;
    shr_lmm_stat_t *lmm_stats;
    size_t stats_cnt = 0;
    bool get_all = false;
    int rv = SHR_E_NONE;

    stats_cnt = BCMA_CLI_ARG_CNT(args);
    if (stats_cnt == 0) {
        shr_lmm_stat_get_all(0, NULL, &stats_cnt);
        if (stats_cnt <= 0) {
            cli_out("%sFailed to get statistics count\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        get_all = true;
    }
    lmm_stats = sal_alloc(stats_cnt * sizeof(shr_lmm_stat_t), "bcmaLmmStats");
    if (lmm_stats == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(lmm_stats, 0, stats_cnt * sizeof(shr_lmm_stat_t));

    if (get_all) {
        rv = shr_lmm_stat_get_all(stats_cnt, lmm_stats, NULL);
    } else {
        shr_lmm_stat_t *stats = lmm_stats;

        while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = shr_lmm_stat_by_id_get(arg, stats++);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
    }

    if (SHR_SUCCESS(rv)) {
        pb = shr_pb_create();

        lmm_stats_header_format(pb);
        lmm_stats_format(pb, lmm_stats, stats_cnt);
        cli_out("%s", shr_pb_str(pb));

        shr_pb_destroy(pb);
    } else {
        cli_out("%sFailed to get statistics for %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, get_all ? "all" : arg);
    }

    sal_free(lmm_stats);

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

int
bcma_salcmd_lmm(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "stats") == 0) {
        return cmd_lmm_stats(cli, args);
    }

    return BCMA_CLI_CMD_USAGE;
}
