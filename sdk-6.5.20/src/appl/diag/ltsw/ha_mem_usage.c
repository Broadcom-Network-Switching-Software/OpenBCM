/*! \file bcma_ha_mem_usage.c
 *
 * APIs for display HA memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/ltsw/ha.h>
#include <bcm_int/ltsw/ha.h>

#include <bsl/bsl.h>

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>

#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmmgmt/bcmmgmt_comp.h>

/*******************************************************************************
 * Local definitions
 */

#define MAX_HA_SUB_ID        SHR_HA_SUB_IDS
#define MAX_HA_SUB_ID_LEN    32

/*******************************************************************************
 * Local functions
 */

static uint32_t
comp_id_parse(const char *name, bool sdklt)
{
    if (sdklt) {
        return bcmmgmt_comp_id_from_name(name);
    } else {
        return bcmi_ltsw_ha_comp_id_from_name(name);
    }
}

static const char *
comp_id_name(uint32_t comp_id, bool sdklt)
{
    if (sdklt) {
        return bcmmgmt_comp_name_from_id(comp_id);
    } else {
        return bcmi_ltsw_ha_comp_name_from_id(comp_id);
    }
}

static uint32_t
ha_comp_mem_usage(int unit, uint8_t comp_id,
                  uint32_t report_size, shr_ha_usage_report_t *reports)
{
    int rv;
    uint32_t sub_id;
    uint32_t comp_mem_usage = 0;

    rv = shr_ha_comp_mem_usage(unit, comp_id, report_size, reports);
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
ha_gen_comp_mem_usage(uint8_t comp_id, uint32_t report_size, shr_ha_usage_report_t *reports)
{
    int rv;
    uint32_t sub_id;
    uint32_t gen_comp_mem_usage = 0;

    rv = shr_ha_gen_comp_mem_usage(comp_id, report_size, reports);
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
ha_mem_usage_separator(shr_pb_t *pb)
{
    shr_pb_printf(pb, "------------------------------------------------------"
                      "--------\n");
}

static void
ha_mem_usage_header_format(shr_pb_t *pb)
{
    shr_pb_printf(pb, "%-20s", "Component");
    shr_pb_printf(pb, "%-20s", "Unit memory usage");
    shr_pb_printf(pb, "%-20s", "Generic memory usage");
    shr_pb_printf(pb, "\n");
}

static void
usage_size_format(shr_pb_t *pb, uint32_t usage)
{
    char tmp[16];

    if (usage > 0) {
        shr_pb_printf(pb, "%10"PRIu32, usage);
    }else {
        shr_pb_printf(pb, "%10s", "-");
    }

    sal_memset(tmp, 0, sizeof(tmp));

    if ((usage >> 20) > 0) {
        sal_snprintf(tmp, sizeof(tmp), "(%"PRIu32"M)", usage >> 20);
    } else if ((usage >> 10) > 0) {
        sal_snprintf(tmp, sizeof(tmp), "(%"PRIu32"K)", usage >> 10);
    }
    shr_pb_printf(pb, " %-9s", tmp);
}

static void
ha_mem_usage_format(shr_pb_t *pb, const char *prefix, const char *name,
                    uint8_t comp_id, uint32_t mem_usage, uint32_t gen_mem_usage)
{
    int len = 20;

    if (prefix) {
        shr_pb_printf(pb, "%s", prefix);
        len -= sal_strlen(prefix);
    }

    /* If name is not specified, try to get component name from comp_enum */
    if (name == NULL) {
        name = comp_id_name(comp_id, true);
    }

    if (name) {
        shr_pb_printf(pb, "%-*s", len, name);
    } else {
        shr_pb_printf(pb, "%-*.03d", len, comp_id);
    }
    usage_size_format(pb, mem_usage);
    usage_size_format(pb, gen_mem_usage);
    shr_pb_printf(pb, "\n");
}

static uint32_t
ha_comp_id_mem_usage(int unit, uint8_t comp_id, bool gen_mem_inc,
                     shr_pb_t *pb, const char *prefix, bool long_fmt)
{
    shr_ha_usage_report_t *mem_usage = NULL;
    shr_ha_usage_report_t *gen_mem_usage = NULL;
    uint32_t comp_mem_usage = 0;
    uint32_t gen_comp_mem_usage = 0;
    uint32_t total_usage;
    int sub_id;
    char sub_id_name[MAX_HA_SUB_ID_LEN];
    const char *name = NULL;

    mem_usage = sal_alloc(MAX_HA_SUB_ID * 2 * sizeof(shr_ha_usage_report_t),
                          "applDiagHaMemUsage");
    if (mem_usage == NULL) {
        return 0;
    }
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

    if (pb == NULL) {
        sal_free(mem_usage);
        return total_usage;;
    }

    /* Format the component memory usage */
    if (long_fmt || total_usage > 0) {
        if (unit >= 0 && unit < BCMDRD_CONFIG_MAX_UNITS) {
            name = comp_id_name(comp_id, true);
        } else {
            name = comp_id_name(comp_id, false);
        }
        ha_mem_usage_format(pb, prefix, name, comp_id,
                            comp_mem_usage, gen_comp_mem_usage);
    }
    /* Format HA memory allocated per sub-id */
    if (long_fmt && total_usage > 0) {
        for (sub_id = 0; sub_id < MAX_HA_SUB_ID; sub_id++) {
            if (mem_usage[sub_id].size == 0) {
                continue;
            }
            sal_snprintf(sub_id_name, MAX_HA_SUB_ID_LEN,
                         "sub-id-%03d blk-id: %s", sub_id,
                         mem_usage[sub_id].blk_id);
            ha_mem_usage_format(pb, "  ", sub_id_name, -1,
                                mem_usage[sub_id].size,
                                gen_mem_usage[sub_id].size);
        }
    }

    sal_free(mem_usage);

    return total_usage;
}

/*******************************************************************************
 * Public functions
 */

uint32_t
appl_ltsw_ha_comp_mem_usage(int unit, const char *comp_name, bool gen_mem_inc,
                       shr_pb_t *pb, bool fmt_hdr)
{
    int comp_id, found = 0;
    uint32_t total_mem_usage = 0;

    if (fmt_hdr) {
        ha_mem_usage_header_format(pb);
    }

    comp_id = comp_id_parse(comp_name, true);
    /* Check for comp_id if not a pre-defined component name */
    if (comp_id < BCMMGMT_MAX_COMP_ID) {
        ha_mem_usage_separator(pb);
        total_mem_usage += ha_comp_id_mem_usage(unit,
                                                comp_id, gen_mem_inc, pb,
                                                NULL, true);
        found++;
    }

    comp_id = comp_id_parse(comp_name, false);
    /* Check for comp_id if not a pre-defined component name */
    if (comp_id < BCMI_HA_COMP_ID_MAX) {
        ha_mem_usage_separator(pb);
        total_mem_usage += ha_comp_id_mem_usage(unit + BCMDRD_CONFIG_MAX_UNITS,
                                                comp_id, gen_mem_inc, pb,
                                                NULL, true);
        found++;
    }
    if (!found) {
        cli_out("Error: Invalid component name: %s\n", comp_name);
    }

    return total_mem_usage;
}

uint32_t
appl_ltsw_ha_mem_usage(int unit, bool gen_mem_inc, shr_pb_t *pb, bool fmt_hdr)
{
    int comp_id;
    uint32_t comp_mem_usage = 0;
    uint32_t sdklt_mem_usage = 0;
    uint32_t total_mem_usage = 0;

    if (fmt_hdr) {
        ha_mem_usage_header_format(pb);
        ha_mem_usage_separator(pb);
    }

    /* Display HA memory allocated per-component */
    for (comp_id = 0; comp_id < BCMMGMT_MAX_COMP_ID; comp_id++) {
        comp_mem_usage = ha_comp_id_mem_usage(unit, comp_id, gen_mem_inc,
                                              pb, "  ", false);
        sdklt_mem_usage += comp_mem_usage;
    }
    total_mem_usage += sdklt_mem_usage;

    if (fmt_hdr) {
        ha_mem_usage_separator(pb);
    }

    /* Display HA memory allocated per-component */
    for (comp_id = 0; comp_id < BCMI_HA_COMP_ID_MAX; comp_id++) {
        comp_mem_usage = ha_comp_id_mem_usage(unit + BCMDRD_CONFIG_MAX_UNITS,
                                              comp_id, gen_mem_inc,
                                              pb, "  ", false);
        total_mem_usage += comp_mem_usage;
    }

    if (fmt_hdr) {
        ha_mem_usage_separator(pb);
    }
    /* Format the sum of HA memory allocated */
    if (pb) {
        shr_pb_printf(pb, "                sdklt allocated: ");
        usage_size_format(pb, sdklt_mem_usage);
        shr_pb_printf(pb, "\n");
        shr_pb_printf(pb, "Total number of bytes allocated: ");
        usage_size_format(pb, total_mem_usage);
        shr_pb_printf(pb, "\n");
    }

    return total_mem_usage;
}
