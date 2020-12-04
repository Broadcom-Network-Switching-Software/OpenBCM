/*! \file bcma_ha_mem_usage.c
 *
 * APIs for display HA memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmmgmt/bcmmgmt_comp.h>
#include <bcmha/bcmha_mem.h>
#include <bcmha/bcmha_mem_diag.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/ha/bcma_ha_mem_usage.h>

/*******************************************************************************
 * Local definitions
 */

#define MAX_HA_SUB_ID        SHR_HA_SUB_IDS
#define MAX_HA_SUB_ID_LEN    64
#define COL_WIDTH_ID         16
#define COL_WIDTH_UMEM       34
#define COL_WIDTH_GMEM       30

/*******************************************************************************
 * Local functions
 */

static bcmmgmt_sysm_comp_id_t
comp_id_parse(const char *name)
{
    return bcmmgmt_comp_id_from_name(name);
}

static const char *
comp_id_name(bcmmgmt_sysm_comp_id_t comp_id)
{
    return bcmmgmt_comp_name_from_id(comp_id);
}

static uint32_t
ha_comp_mem_usage(int unit, uint8_t comp_id,
                  uint32_t report_size, shr_ha_usage_report_t *reports)
{
    int rv;
    uint32_t sub_id;
    uint32_t comp_mem_usage = 0;

    rv = bcmha_mem_usage(unit, comp_id, report_size, reports);
    if (SHR_FAILURE(rv)) {
        return 0;
    }
    /* Calculate total memory usage of the component */
    for (sub_id = 0; sub_id < report_size; sub_id++) {
        comp_mem_usage += reports[sub_id].size;
    }

    return comp_mem_usage;
}

static uint32_t
ha_gen_comp_mem_usage(uint8_t comp_id, uint32_t report_size,
                      shr_ha_usage_report_t *reports)
{
    int rv;
    uint32_t sub_id;
    uint32_t gen_comp_mem_usage = 0;

    rv = bcmha_mem_usage(BCMHA_GEN_UNIT, comp_id, report_size, reports);
    if (SHR_FAILURE(rv)) {
        return 0;
    }
    /* Calculate total generic memory usage of the component */
    for (sub_id = 0; sub_id < report_size; sub_id++) {
        gen_comp_mem_usage += reports[sub_id].size;
    }

    return gen_comp_mem_usage;
}

static void
ha_mem_usage_header_format(shr_pb_t *pb)
{
    shr_pb_printf(pb, "%-*s", COL_WIDTH_ID, "Component");
    shr_pb_printf(pb, "%-*s", COL_WIDTH_UMEM, "Unit memory usage");
    shr_pb_printf(pb, "%-*s", COL_WIDTH_GMEM, "Generic memory usage");
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "------------------------------------------------------"
                      "-------------------------\n");
}

static void
usage_size_format(shr_pb_t *pb, const char *id, uint32_t usage, bool gen_mem)
{
    int len = gen_mem ? COL_WIDTH_GMEM : COL_WIDTH_UMEM;

    /* Add initial print buffer length (see below) */
    len += sal_strlen(shr_pb_str(pb));

    if (usage > 0) {
        shr_pb_printf(pb, " %8"PRIu32, usage);
    } else {
        shr_pb_printf(pb, " %8s", "-");
    }
    if ((usage >> 20) > 0) {
        shr_pb_printf(pb, " (%"PRIu32"M)", usage >> 20);
    } else if ((usage >> 10) > 0) {
        shr_pb_printf(pb, " (%"PRIu32"K)", usage >> 10);
    }
    if (id && id[0]) {
        shr_pb_printf(pb, " [%s]", id);
    }

    /*
     * By subtracting the final print buffer length (after adding the
     * initial length at the top) we have essentially subtracted the
     * length added by this function.
     */
    len -= sal_strlen(shr_pb_str(pb));
    /* Pad to column width */
    if (len > 0) {
        shr_pb_printf(pb, "%-*s", len, "");
    }
}

static void
ha_mem_usage_format(shr_pb_t *pb, const char *prefix, const char *name,
                    uint8_t comp_id,
                    const char *mem_id, uint32_t mem_usage,
                    const char *gen_mem_id, uint32_t gen_mem_usage)
{
    int len = COL_WIDTH_ID;

    if (prefix) {
        shr_pb_printf(pb, "%s", prefix);
        len -= sal_strlen(prefix);
    }

    /* If name is not specified, try to get component name from comp_enum */
    if (name == NULL) {
        name = comp_id_name(comp_id);
    }

    if (name) {
        shr_pb_printf(pb, "%-*s", len, name);
    } else {
        shr_pb_printf(pb, "%-*.03d", len, comp_id);
    }
    usage_size_format(pb, mem_id, mem_usage, false);
    usage_size_format(pb, gen_mem_id, gen_mem_usage, true);
    shr_pb_printf(pb, "\n");
}

static uint32_t
ha_comp_id_mem_usage(int unit, uint8_t comp_id, bool gen_mem_inc,
                     shr_pb_t *pb, const char *prefix, bool long_fmt)
{
    shr_ha_usage_report_t *mem_usage;
    shr_ha_usage_report_t *gen_mem_usage;
    uint32_t comp_mem_usage = 0;
    uint32_t gen_comp_mem_usage = 0;
    uint32_t total_usage;
    int sub_id;
    char sub_id_name[MAX_HA_SUB_ID_LEN];

    mem_usage = sal_alloc(MAX_HA_SUB_ID * 2 * sizeof(shr_ha_usage_report_t),
                          "bcmaHaMemUsage");
    if (mem_usage == NULL) {
        return 0;
    }
    sal_memset(mem_usage, 0, MAX_HA_SUB_ID * 2 * sizeof(shr_ha_usage_report_t));
    gen_mem_usage = mem_usage + MAX_HA_SUB_ID;

    /* Display HA memory allocated for this comp_id */
    comp_mem_usage = ha_comp_mem_usage(unit, comp_id,
                                       MAX_HA_SUB_ID, mem_usage);
    if (gen_mem_inc) {
        gen_comp_mem_usage = ha_gen_comp_mem_usage(comp_id,
                                                   MAX_HA_SUB_ID,
                                                   gen_mem_usage);
    }
    total_usage = comp_mem_usage + gen_comp_mem_usage;

    /* The output information is not required */
    if (pb == NULL) {
        sal_free(mem_usage);
        return total_usage;
    }

    /* Format the component memory usage */
    if (total_usage > 0) {
        ha_mem_usage_format(pb, prefix, NULL, comp_id,
                            NULL, comp_mem_usage, NULL, gen_comp_mem_usage);
        /* Format HA memory allocated per sub-id */
        if (long_fmt) {
            for (sub_id = 0; sub_id < MAX_HA_SUB_ID; sub_id++) {
                if (mem_usage[sub_id].size == 0 &&
                    gen_mem_usage[sub_id].size == 0) {
                    continue;
                }
                sal_snprintf(sub_id_name, MAX_HA_SUB_ID_LEN,
                             "sub-id-%03d", sub_id);
                ha_mem_usage_format(pb, "  ", sub_id_name, -1,
                                    mem_usage[sub_id].blk_id,
                                    mem_usage[sub_id].size,
                                    gen_mem_usage[sub_id].blk_id,
                                    gen_mem_usage[sub_id].size);
            }
        }
    }

    sal_free(mem_usage);

    return total_usage;
}

/*******************************************************************************
 * Public functions
 */

uint32_t
bcma_ha_comp_mem_usage(int unit, const char *comp_name, bool gen_mem_inc,
                       shr_pb_t *pb, bool fmt_hdr)
{
    int comp_id;

    comp_id = comp_id_parse(comp_name);
    /* Check for comp_id if not a pre-defined component name */
    if (comp_id == BCMMGMT_MAX_COMP_ID) {
        if (bcma_cli_parse_int(comp_name, &comp_id) < 0) {
            cli_out("%sInvalid component name: %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, comp_name);
            return 0;
        }
        /* Check the valid comp_id range (uint8_t) */
        if (comp_id >= BCMMGMT_MAX_COMP_ID || comp_id < 0) {
            cli_out("%sInvalid component ID: %d\n",
                    BCMA_CLI_CONFIG_ERROR_STR, comp_id);
            return 0;
        }
    }
    if (fmt_hdr) {
        ha_mem_usage_header_format(pb);
    }
    return ha_comp_id_mem_usage(unit, comp_id, gen_mem_inc, pb, NULL, true);
}

uint32_t
bcma_ha_mem_usage(int unit, bool gen_mem_inc, shr_pb_t *pb,
                  bool fmt_hdr, bool long_fmt)
{
    int comp_id;
    uint32_t comp_mem_usage = 0;
    uint32_t total_mem_usage = 0;

    if (fmt_hdr) {
        ha_mem_usage_header_format(pb);
    }

    /* Display HA memory allocated per-component */
    for (comp_id = 0; comp_id < BCMMGMT_MAX_COMP_ID; comp_id++) {
        comp_mem_usage = ha_comp_id_mem_usage(unit, comp_id, gen_mem_inc,
                                              pb, NULL, long_fmt);
        total_mem_usage += comp_mem_usage;
    }

    /* Format the sum of HA memory allocated */
    if (pb) {
        shr_pb_printf(pb, "Total number of bytes allocated: ");
        usage_size_format(pb, NULL, total_mem_usage, false);
        shr_pb_printf(pb, "\n");
    }

    return total_mem_usage;
}
