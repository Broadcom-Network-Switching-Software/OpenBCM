/** \file diag_dnxc_show.c
 * 
 * show diagnostic pack for fabric
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/parse.h>
#include <appl/diag/dnx/diag_dnx_fabric.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/*bcm*/
#include <bcm/fabric.h>
#include <bcm/stat.h>

/*soc*/
#include <soc/drv.h>
#include <soc/property.h>
#include <soc/counter.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#endif /* BCM_DNX_SUPPORT */

/*dcmn*/

/* #include <appl/diag/dcmn/counter.h> */

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_cgm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>

#endif /* BCM_DNXF_SUPPORT */
/* Put your new common defines in this file*/
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxc/drv.h>
#include "diag_dnxc_patch_database.h"
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL DEFs:
 * {
 */
#define DIAG_DNXC_SHOW_MODIFY_ALL_COUNTERS                  INVALIDr

#define DIAG_DNXC_SHOW_FULL_COUNTERS                        0x1
#define DIAG_DNXC_SHOW_COUNTERS_IN_HEX                      0x2
#define DIAG_DNXC_SHOW_ONLY_NON_ZERO_COUNTERS               0x4
#define DIAG_DNXC_SHOW_ONLY_CHANGED_COUNTERS                0x8
#define DIAG_DNXC_CLEAR_COUNTERS_FULL_MAN            "Parameters: [Port=<integer|port_name|'nif'|'fabric'|'all'>]\n" \
                                                     "        If <Port> is omitted, clear all MIB counters statistics. \n" \
                                                     "        If <Port> is specified, clear counters only for specific ports.\n"

#define DIAG_DNXC_SHOW_COUNTERS_FULL_MAN             "Display all counters, or only for a specific port. \n" \
                                                     "If <full> flag is given there will be provided also: \n" \
                                                     " - counter name \n" \
                                                     " - counter delta from last command call \n"
#define DIAG_DNXC_SHOW_COUNTERS_SYNOPSIS             "[full] [f] [hex] [nz] [chg] [port=<integer|port_name|'tm'|'nif'|'fabric'|'all'> ]"
#define DIAG_DNXC_SHOW_COUNTERS_SHARED_EXAMPLES      "full\n" \
                                                     "hex\n"
#define DIAG_DNXC_DNX_SHOW_COUNTERS_SHARED_EXAMPLES  "full port=xe13\n" \
                                                     "full nz port=xe13\n" \
                                                     "full z port=xe13\n" \
                                                     "full chg port=xe\n"
#define DIAG_DNXC_FABRIC_SHOW_COUNTERS_SHARED_EXAMPLES   "f port=fabric0-fabric5\n"

/*
 * LOCAL DEFs:
 * }
 */
/* 
 * Typpedef
 * {
 */

/* 
 * }
 */
/*
* LOCAL Structures:
* {
*/

/*
 * Data structure for saving the previous value of counters so we can
 * tell which counters that have changed since last shown.
 */
static uint64 *dnxc_counter_val[SOC_MAX_NUM_DEVICES];

static char *_stat_names[] = BCM_STAT_NAME_INITIALIZER;

#ifdef BCM_DNX_SUPPORT
#ifdef INCLUDE_CTEST
/** used in diag test  callbacks to store soc properties to be restored */
static rhhandle_t diag_dnxc_show_soc_set_h[SOC_MAX_NUM_DEVICES] = { NULL };
#endif
#endif

typedef enum
{
    diag_assert_type_option,
    diag_zero_type_option,
    diag_all_type_option
} diag_show_intr_value;

typedef enum
{
    diag_default_type_option,
    diag_mask_type_option,
    diag_unmask_type_option
} diag_show_intr_mask;

/**
 * \brief
 *   Structure used as container for output of diag_dnxc_show_counters_controlled_counter_info_get()
 *   It has elements for the name of the counter, the name of the port, the value (64 bits),
 *   the rate (requiring two consequent invocations) and indication on whether counter has changed
 *   (requiring two consequent invocations)
 */
typedef struct diag_counter_info_s
{
    char *cname;
    char *port_name;
    uint64 value;
    uint64 difference;
    uint64 rate;
    int is_changed;
} diag_counter_info_t;

/*
 * LOCAL Structures:
 * }
 */
/*
 * LOCAL FUNCTIONs:
 * {
 */
void
diag_dnxc_format_uint64_to_str(
    char *buf,
    uint64 value,
    int delimiter,
    int flags)
{

    if (flags & DIAG_DNXC_SHOW_COUNTERS_IN_HEX)
    {
        sal_sprintf(buf, "0x%08x%08x", COMPILER_64_HI(value), COMPILER_64_LO(value));
    }
    else
    {
        format_uint64_decimal(buf, value, delimiter);
    }
}

/* Init diag_counter_info_t structure */
void
counter_info_t_init(
    diag_counter_info_t * counter_info)
{
    COMPILER_64_SET(counter_info->rate, -1, -1);
    COMPILER_64_SET(counter_info->value, 0, 0);
    COMPILER_64_SET(counter_info->difference, 0, 0);
    counter_info->is_changed = 0;
    counter_info->cname = NULL;
    counter_info->port_name = NULL;
}

/*Initialize the database for sotring previous counter values*/
void
diag_dnxc_diag_counter_values_database_init(
    int unit)
{
    int number_of_entries;
    /*
     * If this is the first call of the counters diagnostic for this device create the database array for the previous
     * valus of the counters
     */
    if (dnxc_counter_val[unit] == NULL)
    {
        number_of_entries = SOC_CONTROL(unit)->counter_n32 +
            SOC_CONTROL(unit)->counter_n64 + SOC_CONTROL(unit)->counter_n64_non_dma;

        dnxc_counter_val[unit] = sal_alloc(number_of_entries * sizeof(uint64), "save_ctrs");

        sal_memset(dnxc_counter_val[unit], 0, number_of_entries * sizeof(uint64));
    }

}

/*Deinitialize the database for sotring previous counter values*/
shr_error_e
diag_dnxc_diag_counter_values_database_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(dnxc_counter_val[unit]);

    SHR_FUNC_EXIT;
}

/* Write to the previous counter values database the values of the current counter
   in a special for the per counter and per port index/position.
*/
void
diag_dnxc_counter_val_set(
    int unit,
    soc_port_t port,
    soc_reg_t ctr_reg,
    int ar_idx,
    uint64 val)
{
    int index;
    int number_of_port_counters;
    if (ctr_reg == DIAG_DNXC_SHOW_MODIFY_ALL_COUNTERS)
    {
        /*
         * SET ALL PORT COUNTERS
         */
        /*
         * NOTE! For now used only when clearing
         */

        /*
         * get number of counters per port
         */
        number_of_port_counters = SOC_CONTROL(unit)->counter_perport;
        /*
         * set value
         */
        sal_memset(dnxc_counter_val[unit] + port * number_of_port_counters, COMPILER_64_LO(val),
                   sizeof(uint64) * number_of_port_counters);

    }
    else
    {
        index = soc_counter_idx_get(unit, ctr_reg, ar_idx, port);

        if (index > -1)
        {
            dnxc_counter_val[unit][index] = val;
        }
    }
}

/* Get the previous value using the uniquie counter per port index*/
void
diag_dnxc_counter_val_get(
    int unit,
    soc_port_t port,
    soc_reg_t ctr_reg,
    int ar_idx,
    uint64 *val)
{
    int ind;

    ind = soc_counter_idx_get(unit, ctr_reg, ar_idx, port);

    if (ind < 0)
    {
        COMPILER_64_SET(*val, 0, 0);
    }
    else
    {
        *val = dnxc_counter_val[unit][ind];
    }
}

/* Function which is used for the compact "show counters" printouts*/
static shr_error_e
diag_dnxc_print_counter_table(
    int unit,
    char *table_title,
    bcm_pbmp_t pbmp,
    bcm_stat_val_t * counters_array,
    int number_of_counters,
    int flags,
    sh_sand_control_t * sand_control)
{

    uint64 val64;
    int do_print;
    int not_supported, counter;
    bcm_port_t port;
    char buf_val[32];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", table_title);
    PRT_COLUMN_ADD("Port");
    for (counter = 0; counter < number_of_counters; counter++)
    {
        PRT_COLUMN_ADD("%s", _stat_names[counters_array[counter]]);
    }

    PBMP_ITER(pbmp, port)
    {
        do_print = 0;

        if (flags & DIAG_DNXC_SHOW_ONLY_NON_ZERO_COUNTERS)
        {
            for (counter = 0; counter < number_of_counters; counter++)
            {
                COMPILER_64_SET(val64, 0, 0);
                /*
                 * Check if the counter is supported
                 */
                if (!((IS_IL_PORT(unit, port) && counters_array[counter] == snmpIfHCInUcastPkts) ||
                      (IS_IL_PORT(unit, port) && counters_array[counter] == snmpIfHCOutUcastPkts)))
                {
                    SHR_IF_ERR_EXIT(bcm_stat_get(unit, port, counters_array[counter], &val64));
                }
                /*
                 * Check if port counter is zero. If 0 don't print
                 */
                if (!COMPILER_64_IS_ZERO(val64))
                {
                    do_print = 1;
                    break;
                }
            }
        }
        else
        {
            do_print = 1;
        }
        if (do_print)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%d", port);
            for (counter = 0; counter < number_of_counters; counter++)
            {
                not_supported = 0;
                COMPILER_64_SET(val64, 0, 0);
                if ((IS_IL_PORT(unit, port) && counters_array[counter] == snmpIfHCInUcastPkts) ||
                    (IS_IL_PORT(unit, port) && counters_array[counter] == snmpIfHCOutUcastPkts))
                {
                    not_supported = 1;
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_stat_get(unit, port, counters_array[counter], &val64));
                }
                if (not_supported)
                {
                    PRT_CELL_SET("N/A");
                }
                else if (flags & DIAG_DNXC_SHOW_COUNTERS_IN_HEX)
                {
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(val64), COMPILER_64_LO(val64));
                }
                else
                {

                    format_uint64_decimal(buf_val, val64, ',');
                    PRT_CELL_SET("%s", buf_val);
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_show_counters_cmap_counter_info_get(
    int unit,
    bcm_port_t port,
    soc_reg_t ctr_reg,
    int flags,
    diag_counter_info_t * counter_info)
{
    uint64 val, prev_val, diff, rate;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Regular counters
     */
    if (!(SOC_REG_IS_VALID(unit, ctr_reg) || SOC_REG_IS_COUNTER(unit, ctr_reg)))
    {
         /* coverity[copy_paste_error::FALSE]  */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Register is invalid or it is not a Counter Register\n");
    }

    /*
     * Init counter info structure
     */
    counter_info_t_init(counter_info);

    /*
     * Get current counter value
     */
    if (SOC_CONTROL(unit)->counter_interval == 0)
    {
        /*
         * Counter thread is OFF
         */
        SHR_IF_ERR_EXIT(soc_reg_get(unit, ctr_reg, port, 0, &val));
    }
    else
    {
        /*
         * Counter thread is ON
         */
        SHR_IF_ERR_EXIT(soc_counter_get(unit, port, ctr_reg, 0, &val));
    }

    /*
     * Get prev counter value and rate
     */
    if (SOC_CONTROL(unit)->counter_interval == 0)
    {
        COMPILER_64_ZERO(prev_val);
        COMPILER_64_SET(rate, -1, -1);
    }
    else
    {
        diag_dnxc_counter_val_get(unit, port, ctr_reg, 0, &prev_val);
        soc_counter_get_rate(unit, port, ctr_reg, 0, &rate);
    }

    COMPILER_64_COPY(diff, val);
    COMPILER_64_SUB_64(diff, prev_val);

    if (COMPILER_64_IS_ZERO(diff))
    {
        counter_info->is_changed = 0;
    }
    else
    {
        diag_dnxc_counter_val_set(unit, port, ctr_reg, 0, val);
        counter_info->is_changed = 1;
    }
     /* coverity[Out-of-bounds read::FALSE]  */
    counter_info->cname = SOC_REG_NAME(unit, ctr_reg);
    counter_info->port_name = SOC_PORT_NAME(unit, port);
    COMPILER_64_COPY(counter_info->value, val);
    COMPILER_64_COPY(counter_info->difference, diff);
    COMPILER_64_COPY(counter_info->rate, rate);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] port -
 *   Local port which this counter is monitoring
 * \param [in] ctr_counter_idx -
 *   Index of sub-counter within all counters corresponding to specified port.
 * \param [in] flags -
 *   General info. Currently not used.
 * \param [out] counter_info_p -
 *   This procedure loads pointed memory by the information corresponding
 *   to specified counter as per 'port' and 'ctr_counter_idx':
 *   It has elements for the name of the counter, the name of the port, the value
 *   (64 bits), the rate (requiring two consequent invocations) and indication
 *   on whether counter has changed (requiring two consequent invocations)
 */
static shr_error_e
diag_dnxc_show_counters_controlled_counter_info_get(
    int unit,
    bcm_port_t port,
    int ctr_counter_idx,
    int flags,
    diag_counter_info_t * counter_info_p)
{
    uint64 prev_val, diff, rate;
    uint64 val = COMPILER_64_INIT(0, 0);
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init counter info structure
     */
    counter_info_t_init(counter_info_p);
    /*
     * Get current counter value
     */
    if (SOC_CONTROL(unit)->counter_interval != 0
        && (COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[ctr_counter_idx])))
    {
        /*
         * Counter is collected by counter thread
         */
        SHR_IF_ERR_EXIT(soc_counter_get(unit, port, ctr_counter_idx, 0, &val));
    }
    else
    {
        /*
         * Counter isn't collected by counter thread
         */
        if (SOC_CONTROL(unit)->controlled_counters[ctr_counter_idx].controlled_counter_f != NULL)
        {
            SHR_IF_ERR_EXIT(SOC_CONTROL(unit)->controlled_counters[ctr_counter_idx].controlled_counter_f(unit,
                                                                                                         SOC_CONTROL
                                                                                                         (unit)->
                                                                                                         controlled_counters
                                                                                                         [ctr_counter_idx].
                                                                                                         counter_id,
                                                                                                         port, &val,
                                                                                                         NULL));
        }
    }

    /*
     * get prev counter value and rate
     */
    if (SOC_CONTROL(unit)->counter_interval == 0
        || !(COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[ctr_counter_idx])))
    {
        /*
         * Controlled counters are NOT support while counter thread is off
         */
        /*
         * Controlled counters are clear on read therefore prev val and rate are irrelevant
         */
        COMPILER_64_ZERO(prev_val);
        COMPILER_64_SET(rate, -1, -1);
    }
    else
    {
        diag_dnxc_counter_val_get(unit, port, ctr_counter_idx, 0, &prev_val);
        soc_counter_get_rate(unit, port, ctr_counter_idx, 0, &rate);
    }

    COMPILER_64_COPY(diff, val);
    COMPILER_64_SUB_64(diff, prev_val);

    if (COMPILER_64_IS_ZERO(diff))
    {
        counter_info_p->is_changed = 0;
    }
    else
    {
        diag_dnxc_counter_val_set(unit, port, ctr_counter_idx, 0, val);
        counter_info_p->is_changed = 1;
    }

    counter_info_p->cname = SOC_CONTROL(unit)->controlled_counters[ctr_counter_idx].short_cname;
    counter_info_p->port_name = SOC_PORT_NAME(unit, port);
    counter_info_p->value = val;
    counter_info_p->difference = diff;
    counter_info_p->rate = rate;

exit:
    SHR_FUNC_EXIT;
}
/*
 * LOCAL FUNCTIONs:
 * }
 */
/*
 * LOCAL DIAG PACK:
 * {
 */

/*
* {  PVT diag
*/

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_show_pvt_man = {
    .brief    = "Shows device temperature",
    .full     = "Shows all device temperature monitors indications"
};
/* *INDENT-ON* */
/**
 * \brief - display device temperature
 */
static shr_error_e
cmd_dnxc_show_pvt(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#ifdef BCM_DNX_SUPPORT
    bcm_switch_temperature_monitor_t temp_monitor[DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS];
#else
    bcm_switch_temperature_monitor_t temp_monitor[SOC_DNXC_MAX_NOF_PVT_MONITORS];
#endif
    int temp_int, temp_remainder;
    int peak_int, peak_remainder;
    int monitor_idx, temp_monitor_count;
    int nof_pvt_monitors = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_pvt_monitors = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_pvt_monitors = dnxf_data_device.general.nof_pvt_monitors_get(unit);
    }
#endif
    /*
     * Get PVT info
     */
    SHR_IF_ERR_EXIT(bcm_switch_temperature_monitor_get(unit, nof_pvt_monitors, temp_monitor, &temp_monitor_count));

    /*
     * Print PVT diagnostic { 
     */
    PRT_TITLE_SET("PVT");
    PRT_COLUMN_ADD("Monitor");
    PRT_COLUMN_ADD("Current Temperature (C)");
    PRT_COLUMN_ALIGN;
    PRT_COLUMN_ADD("Peak Temperature (C)");
    PRT_COLUMN_ALIGN;

    for (monitor_idx = 0; monitor_idx < temp_monitor_count; monitor_idx++)
    {
        temp_int = temp_monitor[monitor_idx].curr / 10;
        temp_remainder = temp_monitor[monitor_idx].curr % 10;
        temp_remainder *= temp_remainder < 0 ? -1 : 1;  /* fix sign */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", monitor_idx);
        PRT_CELL_SET("%d.%d", temp_int, temp_remainder);

        peak_int = temp_monitor[monitor_idx].peak / 10;
        peak_remainder = temp_monitor[monitor_idx].peak % 10;
        peak_remainder *= peak_remainder < 0 ? -1 : 1;  /* fix sign */
        PRT_CELL_SET("%d.%d", peak_int, peak_remainder);
    }
    PRT_COMMITX;

    /*
     * Print PVT diagnostic * } 
     */

exit:

    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
* }  PVT diag
*/

/*
* {  patches diag
*/

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_show_patches_man = {
    .brief =    "Show device all installed patches",
    .full =     "Show device all installed patches",
    .synopsis = "",
    .examples = "\n",
};
/* *INDENT-ON* */

/**
 * \brief - display patches installed.
 */
static shr_error_e
cmd_dnxc_show_patches(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int num_of_installed, index;
    int table_size;
    diag_dnxc_patch *installed_patches = NULL;
    char *soc_dnxc_version;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnxc_patches_table_size_get(unit, &table_size));
    installed_patches = sal_alloc(table_size * sizeof(diag_dnxc_patch), "patches table size");
    /*
     * Get all installed patches
     */
    SHR_IF_ERR_EXIT(diag_dnxc_patches_installed_get(unit, table_size, installed_patches, &num_of_installed));
    /*
     * Get Get Current SDK Version.
     */
    SHR_IF_ERR_EXIT(diag_dnxc_version_get(unit, &soc_dnxc_version));
    /*
     * Print patches diagnostic {
     */
    cli_out("Current version installed: %s\n", soc_dnxc_version);

    if (num_of_installed == 0)
    {
        cli_out("No patches have been installed\n");
    }
    else
    {
        PRT_TITLE_SET("Patches installed info");
        PRT_COLUMN_ADD("Patch ID");
        PRT_COLUMN_ADD("Patch String");

        for (index = 0; index < num_of_installed; index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", installed_patches[index].id);
            PRT_CELL_SET("%s", installed_patches[index].str);
        }
        PRT_COMMITX;
    }
    /*
     * Print patches diagnostic * }
     */
exit:
    if (installed_patches != NULL)
    {
        sal_free(installed_patches);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
* }  patches diag
*/

#ifdef BCM_DNX_SUPPORT
#ifdef INCLUDE_CTEST
static shr_error_e
cmd_dnxc_show_init_cb(
    int unit)
{
    ctest_soc_property_t ctest_soc_property[] = {
        {"fabric_connect_mode.0", "FE"},
        {NULL}
    };

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &diag_dnxc_show_soc_set_h[unit]));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_show_deinit_cb(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        ctest_dnxc_restore_soc_properties(unit, diag_dnxc_show_soc_set_h[unit]);
    }
    SHR_FUNC_EXIT;
}
#endif
#endif

/*
* {  Show counters diag
*/
/* *INDENT-OFF* */

static sh_sand_option_t dnxc_show_counters_options[] = {
    /*name      type                 desc                                                                  default*/
    {"FULL",    SAL_FIELD_TYPE_BOOL, "display ALL available counters",                                     "false"},
    {"hex",     SAL_FIELD_TYPE_BOOL, "display counter in hex format",                                      "false"},
    {"NonZero", SAL_FIELD_TYPE_BOOL, "display non-zero counter only or not",                               "false"},
    {"Zero",    SAL_FIELD_TYPE_BOOL, "display counters including zero counter",                            "false"},
    {"CHanGed", SAL_FIELD_TYPE_BOOL, "display only counters that changed their value since previous call", "false"},
    {"port",    SAL_FIELD_TYPE_PORT, "filter on Port ID or port name",                                     "all"},
    {NULL}
};
#ifdef BCM_DNX_SUPPORT
static sh_sand_man_t dnx_fabric_avail_show_counters_man = {
    .brief    = "Shows device counters",
    .full     = DIAG_DNXC_SHOW_COUNTERS_FULL_MAN
                " - counter rate (in packets or cells per second)",
#ifdef INCLUDE_CTEST
    .init_cb = cmd_dnxc_show_init_cb,
    .deinit_cb = cmd_dnxc_show_deinit_cb,
#endif
    .synopsis = DIAG_DNXC_SHOW_COUNTERS_SYNOPSIS,
    .examples = DIAG_DNXC_SHOW_COUNTERS_SHARED_EXAMPLES
                DIAG_DNXC_DNX_SHOW_COUNTERS_SHARED_EXAMPLES
                DIAG_DNXC_FABRIC_SHOW_COUNTERS_SHARED_EXAMPLES,
};

static sh_sand_man_t dnx_fabric_unavail_show_counters_man = {
    .brief    = "Shows device counters",
    .full     = DIAG_DNXC_SHOW_COUNTERS_FULL_MAN
                " - counter rate (in packets or cells per second)",
    .synopsis = DIAG_DNXC_SHOW_COUNTERS_SYNOPSIS,
    .examples = DIAG_DNXC_SHOW_COUNTERS_SHARED_EXAMPLES
                DIAG_DNXC_DNX_SHOW_COUNTERS_SHARED_EXAMPLES
};
#endif
#ifdef BCM_DNXF_SUPPORT
static sh_sand_man_t dnxf_show_counters_man = {
    .brief    = "Shows device counters",
    .full     = DIAG_DNXC_SHOW_COUNTERS_FULL_MAN
                " - counter rate (in cells per second)",
    .synopsis = DIAG_DNXC_SHOW_COUNTERS_SYNOPSIS,
    .examples = DIAG_DNXC_SHOW_COUNTERS_SHARED_EXAMPLES
                DIAG_DNXC_FABRIC_SHOW_COUNTERS_SHARED_EXAMPLES
                "full nz port=sfi180\n"
                "full z port=fabric9\n"
                "full chg port=sfi44-sfi144",
};
#endif
/* *INDENT-ON* */
/**
 * \brief - display device counters
 */
static shr_error_e
cmd_dnxc_show_counters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int display_in_hex, only_non_zero_counters, display_zero_counters, only_changed_counters, display_full;
    soc_control_t *soc;
    bcm_pbmp_t pbmp_fabric, pbmp_rest, selected_pbmp, scannned_ports;
    uint32 flags = 0;
    bcm_port_config_t port_config;
    soc_cmap_t *cmap;
    bcm_port_t port;
    int index, numregs, i;
    int is_port_present;
#ifdef BCM_DNX_SUPPORT
    int enable, printable, rv;
#endif
    soc_reg_t reg;
    diag_counter_info_t counter_info;
    char pfmt[SOC_PBMP_FMT_LEN];
    char value_str[50], difference_str[50], rate_str[50];

    bcm_stat_val_t arr_snmp_nif_rx[] =
        { snmpIfHCInUcastPkts, snmpIfInNUcastPkts, snmpIfInErrors, snmpEtherStatsRXNoErrors };
    bcm_stat_val_t arr_snmp_nif_tx[] =
        { snmpIfHCOutUcastPkts, snmpIfOutNUcastPkts, snmpIfOutErrors, snmpEtherStatsTXNoErrors };
    bcm_stat_val_t arr_snmp_fabric_rx[] = { snmpBcmRxControlCells, snmpBcmRxDataCells, snmpBcmRxCrcErrors };
    bcm_stat_val_t arr_snmp_fabric_tx[] = { snmpBcmTxControlCells, snmpBcmTxDataCells };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init counter info structure
     */
    counter_info_t_init(&counter_info);

    /*
     * Get parameters 
     */
    SH_SAND_GET_BOOL("FULL", display_full);
    SH_SAND_GET_BOOL("hex", display_in_hex);
    SH_SAND_GET_BOOL("Zero", display_zero_counters);
    SH_SAND_GET_BOOL("NonZero", only_non_zero_counters);
    SH_SAND_GET_BOOL("CHanGed", only_changed_counters);
    SH_SAND_IS_PRESENT("port", is_port_present);
    SH_SAND_GET_PORT("port", selected_pbmp);

    /*
     * Create Flag bitmap
     */
    if (display_full)
    {
        flags |= DIAG_DNXC_SHOW_FULL_COUNTERS;
    }
    if (display_in_hex)
    {
        flags |= DIAG_DNXC_SHOW_COUNTERS_IN_HEX;
    }
    if (only_changed_counters)
    {
        flags |= DIAG_DNXC_SHOW_ONLY_CHANGED_COUNTERS;
    }
    /*
     * default when counter thread is active: nz
     * default when counter thread is not active: z
     * Option 'Zero' has higher priority than 'NonZero'
     */

    if (display_zero_counters || ((SOC_CONTROL(unit)->counter_interval == 0) && !only_non_zero_counters))
    {
        flags &= ~DIAG_DNXC_SHOW_ONLY_NON_ZERO_COUNTERS;
    }
    else
    {
        flags |= DIAG_DNXC_SHOW_ONLY_NON_ZERO_COUNTERS;
    }

    /*
     * Get a bitmap off all ports on the device
     */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /*
     * Get a pitmap of ports beeng scanned
     */
    soc = SOC_CONTROL(unit);
    BCM_PBMP_ASSIGN(scannned_ports, soc->counter_pbmp);

    /*
     * Filter only scanned ports from provided port bitmap
     */
    BCM_PBMP_AND(selected_pbmp, scannned_ports);

    /*
     * If no specific port bitmap was passed define default bitmap per device type FAP - all NIF ports FE - all SFI
     * ports 
     */
    if (!is_port_present)
    {
        /*
         * If port bitmap is not provided create default one from all available ports
         */
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            bcm_pbmp_t flexe_mac_pbmp;
            /*
             * FAP case
             */
            /*
             * If no pbmp provided and no flags show all NIF ports by default for FAP device
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF,
                             DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_PHY, &selected_pbmp));
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC, 0, &flexe_mac_pbmp));
            BCM_PBMP_OR(selected_pbmp, flexe_mac_pbmp);
        }
        else
#endif /* BCM_DNX_SUPPORT */
        {
            /*
             * FE case
             */
            BCM_PBMP_ASSIGN(selected_pbmp, port_config.sfi);
        }
    }

    /** sync the latest values */
    if (soc->counter_interval != 0)
    {
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
    }

    if (flags & DIAG_DNXC_SHOW_FULL_COUNTERS)
    {

        PRT_TITLE_SET("Full counters");
        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD("Counter Name");
        PRT_COLUMN_ADD("Counter Value");
        PRT_COLUMN_ADD("Diff from last call");
        PRT_COLUMN_ADD("Rate");

        /*
         * Init counters database for previous values
         */
        diag_dnxc_diag_counter_values_database_init(unit);

        /*
         * Show all available counters per port or for a specific one
         */
        PBMP_ITER(selected_pbmp, port)
        {
            /*
             * Get counters map
             */
            if (SOC_IS_DNX(unit) && !IS_SFI_PORT(unit, port))
            {
                bcm_port_t port_base;
                int phy_port;

                /*
                 * get port base for channelized case 
                 */
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];

                cmap = soc_port_cmap_get(unit, port_base);
            }
            else
            {
                cmap = soc_port_cmap_get(unit, port);
            }

            /*
             * If Counter Map mechanism is unavailable Use controlled counter mechanism
             */
            if (cmap == NULL)
            {
                /*
                 * Iterate over all controlled counters for the device
                 */
                for (index = 0; soc->controlled_counters[index].controlled_counter_f != NULL; index++)
                {
#ifdef BCM_DNX_SUPPORT
                    if (SOC_IS_DNX(unit))
                    {
                        enable = 0;
                        rv = dnx_mib_counter_enable_get(unit, port, index, &enable, &printable);

                        if (rv != BCM_E_NONE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NOT a Controlled counter!");

                        }
                        if (!enable)
                        {
                            continue;
                        }
                    }
#endif /* There are several Dummy counter values and some legacy counters marked with this INVALID flag */
                    if (soc->controlled_counters[index].flags == _SOC_CONTROLLED_COUNTER_FLAG_INVALID)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(diag_dnxc_show_counters_controlled_counter_info_get
                                    (unit, port, index, flags, &counter_info));
                    /*
                     * print result 
                     */
                    if ((counter_info.is_changed || (!counter_info.is_changed && !(flags & DIAG_DNXC_SHOW_ONLY_CHANGED_COUNTERS))) &&   /* changed 
                                                                                                                                         * flag */
                        ((COMPILER_64_IS_ZERO(counter_info.value) && !(flags & DIAG_DNXC_SHOW_ONLY_NON_ZERO_COUNTERS)) || !COMPILER_64_IS_ZERO(counter_info.value))     /* zero 
                                                                                                                                                                         * flag */
                        )
                    {
                        uint64 tmp_64;

                        diag_dnxc_format_uint64_to_str(value_str, counter_info.value, ',', flags);
                        /*
                         * Check if counter rate is available
                         */
                        COMPILER_64_SET(tmp_64, -1, -1);
                        if (COMPILER_64_NE(counter_info.rate, tmp_64))
                        {
                            diag_dnxc_format_uint64_to_str(difference_str, counter_info.difference, ',', flags);
                            diag_dnxc_format_uint64_to_str(rate_str, counter_info.rate, ',', flags);
                        }
                        else
                        {
                            sal_sprintf(difference_str, "N/A");
                            sal_sprintf(rate_str, "N/A");
                        }

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", counter_info.port_name);
                        PRT_CELL_SET("%s", counter_info.cname);
                        PRT_CELL_SET("%s", value_str);
                        PRT_CELL_SET("%s", difference_str);
                        PRT_CELL_SET("%s", rate_str);
                    }
                }
            }
            else
            {
                /*
                 * If Counter Map Available
                 */
                numregs = cmap->cmap_size;

                /*
                 * print all counter regs
                 */
                for (i = 0; i < numregs; i++)
                {
                    reg = cmap->cmap_base[i].reg;
                    if (reg != INVALIDr)
                    {
                        SHR_IF_ERR_EXIT(diag_dnxc_show_counters_cmap_counter_info_get
                                        (unit, port, reg, flags, &counter_info));

                        /*
                         * print result 
                         */
                        if ((counter_info.is_changed || (!counter_info.is_changed && !(flags & DIAG_DNXC_SHOW_ONLY_CHANGED_COUNTERS))) &&       /* changed 
                                                                                                                                                 * flag */
                            ((COMPILER_64_IS_ZERO(counter_info.value) && !(flags & DIAG_DNXC_SHOW_ONLY_NON_ZERO_COUNTERS)) || !COMPILER_64_IS_ZERO(counter_info.value)) /* zero 
                                                                                                                                                                         * flag */
                            )
                        {
                            uint64 tmp_64;

                            diag_dnxc_format_uint64_to_str(value_str, counter_info.value, ',', flags);
                            /*
                             * Check if counter rate is available
                             */
                            COMPILER_64_SET(tmp_64, -1, -1);
                            if (COMPILER_64_NE(counter_info.rate, tmp_64))
                            {
                                diag_dnxc_format_uint64_to_str(difference_str, counter_info.difference, ',', flags);
                                diag_dnxc_format_uint64_to_str(rate_str, counter_info.rate, ',', flags);
                            }
                            else
                            {
                                sal_sprintf(difference_str, "N/A");
                                sal_sprintf(rate_str, "N/A");
                            }

                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", counter_info.port_name);
                            PRT_CELL_SET("%s", counter_info.cname);
                            PRT_CELL_SET("%s", value_str);
                            PRT_CELL_SET("%s", difference_str);
                            PRT_CELL_SET("%s", rate_str);
                        }
                    }
                }
            }
        }
        PRT_COMMITX;
    }
    else
    {
        /*
         * Print compact table
         */

        /*
         * Print tabled for NIF ports and Fabric ports
         */
        BCM_PBMP_ASSIGN(pbmp_fabric, selected_pbmp);
        BCM_PBMP_AND(pbmp_fabric, PBMP_SFI_ALL(unit));

        BCM_PBMP_NEGATE(pbmp_rest, PBMP_SFI_ALL(unit));
        BCM_PBMP_AND(pbmp_rest, selected_pbmp);

        if (BCM_PBMP_NOT_NULL(pbmp_rest))
        {

            cli_out("#############\n");
            cli_out("# NIF Ports #\n");
            cli_out("#############\n");

            /*
             * Print NIF RX Counters
             */
            SHR_IF_ERR_EXIT(diag_dnxc_print_counter_table
                            (unit, "RX COUNTERS", pbmp_rest, arr_snmp_nif_rx, PARSE_ENTRIES(arr_snmp_nif_rx), flags,
                             sand_control));

            /*
             * Print NIF TX Counters
             */
            SHR_IF_ERR_EXIT(diag_dnxc_print_counter_table
                            (unit, "TX COUNTERS", pbmp_rest, arr_snmp_nif_tx, PARSE_ENTRIES(arr_snmp_nif_tx), flags,
                             sand_control));

        }

        if (BCM_PBMP_NOT_NULL(pbmp_fabric))
        {

            cli_out("|--------------|\n");
            cli_out("| Fabric Ports |\n");
            cli_out("|--------------|\n");

            /*
             * Print Fabric RX Counters
             */
            SHR_IF_ERR_EXIT(diag_dnxc_print_counter_table
                            (unit, "RX COUNTERS", pbmp_fabric, arr_snmp_fabric_rx, PARSE_ENTRIES(arr_snmp_fabric_rx),
                             flags, sand_control));

            /*
             * Print Fabric TX Counters
             */
            SHR_IF_ERR_EXIT(diag_dnxc_print_counter_table
                            (unit, "TX COUNTERS", pbmp_fabric, arr_snmp_fabric_tx, PARSE_ENTRIES(arr_snmp_fabric_tx),
                             flags, sand_control));

        }
    }

    SOC_PBMP_REMOVE(selected_pbmp, soc->counter_pbmp);

    if (soc->counter_interval == 0)
    {
        LOG_CLI(("NOTE: counter collection is not running\n"));
    }
    else if (SOC_PBMP_NOT_NULL(selected_pbmp))
    {
        LOG_CLI(("NOTE: counter collection is not active for ports %s\n", SOC_PBMP_FMT(selected_pbmp, pfmt)));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
* }  Show counters diag
*/

/*
* {  Clear counters diag - Start
*/
/* *INDENT-OFF* */

static sh_sand_option_t dnxc_clear_counters_options[] = {
    /*name             type                      desc                              default*/
    {"port",           SAL_FIELD_TYPE_PORT,      "filter on Port ID or port name", "all"},
    {NULL}
};

/* *INDENT-ON* */
/**
 * \brief - display device counters
 */
static shr_error_e
cmd_dnxc_clear_counters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    uint64 zero_val64;
    soc_port_t port;
    bcm_pbmp_t selected_pbmp;
    bcm_pbmp_t valid_pbmp;
    bcm_port_config_t device_port_config;

    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ZERO(zero_val64);
    /*
     *  Retrieve the user input for selected pbmp.
     *  If not selected clear counters for all ports.
     */
    SH_SAND_GET_PORT("port", selected_pbmp);

    /*
     * Init counters database for prevoius values
     */
    diag_dnxc_diag_counter_values_database_init(unit);

    /*
     * Get all device ports
     */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &device_port_config));

    /*
     * Create a bit map with all valid ports for the device
     */
    SOC_PBMP_CLEAR(valid_pbmp);
    if (soc_feature(unit, soc_feature_cpuport_stat_dma))
    {
        SOC_PBMP_ASSIGN(valid_pbmp, device_port_config.all);
    }
    else
    {
        SOC_PBMP_ASSIGN(valid_pbmp, device_port_config.port);
        SOC_PBMP_OR(valid_pbmp, PBMP_FLEXE_MAC_ALL(unit));
    }

    SOC_PBMP_REMOVE(valid_pbmp, PBMP_RCY_ALL(unit));

    /*
     * If no port filter is provided or port filter equal to "all" (as the default value) 
     * clear all counters for all valid ports
     */
    if (SOC_PBMP_EQ(selected_pbmp, device_port_config.all))
    {
        SOC_PBMP_ASSIGN(selected_pbmp, valid_pbmp);
    }

    /*
     * Make sure that all selected ports are members of the valid port bitmap 
     * before accessing the database or resetting registers
     */
    SOC_PBMP_ITER(selected_pbmp, port)
    {
        if (!SOC_PBMP_MEMBER(valid_pbmp, port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %s has no counter information or it is invalid!",
                         SOC_PORT_NAME(unit, port));
        }
    }

    COMPILER_64_ZERO(zero_val64);

    SOC_PBMP_ITER(selected_pbmp, port)
    {
        /*
         * clear counters registers
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, port));
        /*
         * clear diag counters data base
         */
        diag_dnxc_counter_val_set(unit, port, DIAG_DNXC_SHOW_MODIFY_ALL_COUNTERS, 0, zero_val64);
    }

exit:

    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_clear_interrupts(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 intr_id;
    char *str_intr_id;
    int32 port = 0;
    bcm_switch_event_control_t event;
    int32 nof_interrupts;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    /*
     * Get filter input if used
     */

    SH_SAND_GET_STR("ID", str_intr_id);
    if (sal_strcasecmp(str_intr_id, "all") == 0)
    {
        intr_id = BCM_SWITCH_EVENT_CONTROL_ALL;
        port = 0;
    }
    else
    {
        SH_SAND_GET_INT32("port", port);
        if (!isint(str_intr_id))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid input interrupt ID: %s\n", str_intr_id);
        }
        intr_id = parse_integer(str_intr_id);
        if (intr_id >= nof_interrupts)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid Interrupt number specified! interrupt should be between 0 and %d \n",
                         nof_interrupts - 1);
        }
    }
    event.event_id = intr_id;
    event.index = port;
    event.action = bcmSwitchEventClear;

    SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1));

exit:
    SHR_FUNC_EXIT;
}
static sh_sand_option_t dnxc_clear_interrupts_options[] = {
    {"ID", SAL_FIELD_TYPE_STR, "Interrupt ID or all", NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {"Port", SAL_FIELD_TYPE_INT32, "port number", "0"},
    {NULL}
};
static sh_sand_man_t dnxc_clear_interrupts_man = {
    .brief = "Clear device interrupt",
    .full = "Diagnostic to clear interrupts by given interrupt number",
    .synopsis = "<intr_id> [port]",
    .examples = "12\n" "122 p=0\n" "all"
};

/* *INDENT-OFF* */
static sh_sand_man_t dnxf_clear_counters_man = {
    .brief = "Clear device counters",
    .full = DIAG_DNXC_CLEAR_COUNTERS_FULL_MAN,
    .synopsis = "[Port=<integer|port_name|'fabric'|'all'>]",
    .examples = "port=sfi13\n"
                "port=sfi13-sfi16 \n"
                "port=fabric\n",
};

static sh_sand_man_t dnx_clear_counters_man = {
    .brief = "Clear device counters",
    .full = DIAG_DNXC_CLEAR_COUNTERS_FULL_MAN,
    .synopsis = "[Port=<integer|port_name|'fabric'|'all'>]",
    .examples = "port=xe13\n"
                "port=xe13-xe16\n"
                "port=fabric\n",
};

/* *INDENT-ON* */

/*
* }  Clear counters diag - END
*/

/*
* {  Interrupt diag
*/

/* *INDENT-OFF* */
static sh_sand_enum_t dnxc_show_intr_value_enum_table[] = {
    {"assert",  diag_assert_type_option,    "assert interrupts"},
    {"zero",    diag_zero_type_option,      "zero interrupts"},
    {"all",     diag_all_type_option,       "all interrupts"},
    {NULL}
};

static sh_sand_option_t dnxc_show_intr_options[] = {
    {"value",  SAL_FIELD_TYPE_ENUM,   "Interrupt value (assert/zero/all)",  "assert", (void *)dnxc_show_intr_value_enum_table, NULL, SH_SAND_ARG_FREE},
    {"MaSK",      SAL_FIELD_TYPE_BOOL,   "show mask interrupt",  "no"},
    {"UNMask",    SAL_FIELD_TYPE_BOOL,   "show unmask interrupt",  "no"},
    {NULL}
};

static sh_sand_man_t dnxc_show_intr_man = {
    .brief    = "Show device interrupt status/statistics",
    .full     = "Display interrupt Status/Statistics\n"
                "\tASSERT - Display asserted interrupts\n"
                "\tZERO   - Display cleared interrupts\n"
                "\tALL    - Display All interrupts\n"
                "\tMaSK   - Displays Masked interrupts\n"
                "\tUNMask - Displays UnMasked interrupts",
    .synopsis = "[ASSERT/ZERO/ALL] [MaSK] [UNMask]",
    .examples = "\n"
                "zero\n"
                "all\n"
                "MAsK\n"
                "UNMask\n"
                "MAsK zero"
};
/* *INDENT-ON* */

/**
 * \brief - display device interrupt
 */
static shr_error_e
cmd_dnxc_show_intr(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 show_value_selected;
    int show_mask_selected, show_unmask_selected;
    int i, blk, rc, val, mask_val, flags, nof_interrupts;
    soc_interrupt_db_t *interrupts;
    soc_block_info_t *bi = NULL;
    int port;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }
    /*
     * Get filter input if used
     */
    SH_SAND_GET_ENUM("value", show_value_selected);
    SH_SAND_GET_BOOL("MaSK", show_mask_selected);
    SH_SAND_GET_BOOL("UNMask", show_unmask_selected);

    /*
     * device printout
     */
    flags = 0;

    switch (show_value_selected)
    {
        case diag_zero_type_option:
            flags |= SHOW_INTR_Z;
            break;
        case diag_all_type_option:
            flags |= SHOW_INTR_ALL;
            break;
        default:
            break;
    }
    if (show_mask_selected)
    {
        flags |= SHOW_INTR_MASK;
    }
    if (show_unmask_selected)
    {
        flags |= SHOW_INTR_UNMASK;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        SHR_CLI_EXIT(_SHR_E_UNIT, "Not supported for unit %d \n", unit);
    }
    PRT_TITLE_SET("                      Interrupt");
    PRT_COLUMN_ADD("Interrupt Name#");
    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Mask");
    PRT_COLUMN_ADD("On");
    PRT_COLUMN_ADD("Count");

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (SOC_FAILURE(rc))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed in soc_nof_interrupts()\n");
    }

    for (i = 0; i < nof_interrupts; i++)
    {
        if (!SOC_REG_IS_VALID(unit, interrupts[i].reg))
        {
            continue;
        }
        SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit, interrupts[i].reg).block)
        {
            if (!SOC_INFO(unit).block_valid[blk])
            {
                continue;
            }

            bi = &(SOC_BLOCK_INFO(unit, blk));
            port = (bi->type == SOC_BLK_CLP || bi->type == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : bi->number;

            rc = soc_interrupt_get(unit, port, &(interrupts[i]), &val);
            if (SOC_FAILURE(rc))
            {
#if !defined(SOC_NO_NAMES)
/* { */
                LOG_CLI((BSL_META_U(unit, "Failed to read counter: %s\n"), interrupts[i].name));
/* } */
#else
/* { */
                LOG_CLI((BSL_META_U(unit, "Failed to read counter\n")));
/* } */
#endif
                continue;
            }

            rc = soc_interrupt_is_enabled(unit, port, &(interrupts[i]), &mask_val);
            if (SOC_FAILURE(rc))
            {
#if !defined(SOC_NO_NAMES)
/* { */
                LOG_CLI((BSL_META_U(unit, "Failed to read interrupt mask: %s\n"), interrupts[i].name));
/* } */
#else
/* { */
                LOG_CLI((BSL_META_U(unit, "Failed to read interrupt mask\n")));
/* } */
#endif
                continue;
            }
            if ((val && !(flags & SHOW_INTR_Z)) || (!val && (flags & SHOW_INTR_Z)) || (flags & SHOW_INTR_ALL))
            {
                if ((mask_val && !(flags & SHOW_INTR_MASK)) || (!mask_val && !(flags & SHOW_INTR_UNMASK)))
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
#if !defined(SOC_NO_NAMES)
/* { */
                    PRT_CELL_SET("%s[%d]", interrupts[i].name, bi->number);
/* } */
#else
/* { */
                    PRT_CELL_SET("%s[%d]", "--", bi->number);
/* } */
#endif
                    PRT_CELL_SET("%d", i);
                    PRT_CELL_SET("%d", mask_val);
                    PRT_CELL_SET("%d", val);
                    PRT_CELL_SET("%d", interrupts[i].statistics_count[bi->number]);
                }
            }
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
* }  Interrupt diag
*/


/* *INDENT-OFF* */
static sh_sand_option_t dnxc_show_units_options[] = {
    {"id", SAL_FIELD_TYPE_INT32, "device unit id", "-1", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_show_units_man = {
    .brief =    "Show device units list or parameters\n",
    .full =     "Show device units list or parameters\n",
    .synopsis = "[<unit>]\n",
    .examples = "0\n",
};

/* *INDENT-ON* */

/**
 * \brief - display device units info
 */
static shr_error_e
cmd_dnxc_show_units(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int unit_id;
    soc_control_t *usoc;
    int is_variable_present;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("id", is_variable_present);

    if (is_variable_present)    /* dump unit's parameters */
    {
        SH_SAND_GET_INT32("id", unit_id);

        if (!SOC_UNIT_VALID(unit_id))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unit %d is not valid\n", unit_id);
        }
        usoc = SOC_CONTROL(unit_id);
        if (!(usoc->soc_flags & SOC_F_ATTACHED))
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Unit %d (detached)\n", unit_id);
        }
        LOG_CLI(("Unit %d chip %s%s\n", unit_id, soc_dev_name(unit_id), unit_id == unit ? " (current)" : ""));

        soc_dnxc_chip_dump(unit_id, SOC_DRIVER(unit_id));
    }
    else        /* list all units */
    {
        for (unit_id = 0; unit_id < soc_ndev; unit_id++)
        {
            if (!SOC_UNIT_VALID(SOC_NDEV_IDX2DEV(unit_id)))
            {
                continue;
            }
            usoc = SOC_CONTROL(SOC_NDEV_IDX2DEV(unit_id));
            LOG_CLI(("Unit %d chip %s%s\n",
                     SOC_NDEV_IDX2DEV(unit_id),
                     soc_dev_name(SOC_NDEV_IDX2DEV(unit_id)), SOC_NDEV_IDX2DEV(unit_id) == unit ? " (current)" : ""));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * } LOCAL DIAG PACK
 */

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_show_cmds[] = {
    /*keyword,    action,                 command, options,                    man                               */
    {"pvt",       cmd_dnxc_show_pvt,      NULL,    NULL,                       &dnxc_show_pvt_man,      NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"patches",   cmd_dnxc_show_patches,  NULL,    NULL,                       &dnxc_show_patches_man},
#ifdef BCM_DNX_SUPPORT
    {"counters",   cmd_dnxc_show_counters, NULL,    dnxc_show_counters_options, &dnx_fabric_avail_show_counters_man,     NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY,  sh_cmd_is_dnx_with_fabric_available},
    {"counters",   cmd_dnxc_show_counters, NULL,    dnxc_show_counters_options, &dnx_fabric_unavail_show_counters_man,   NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY,  sh_cmd_is_dnx_with_fabric_unvailable},
#endif
#ifdef BCM_DNXF_SUPPORT
    {"counters",   cmd_dnxc_show_counters, NULL,    dnxc_show_counters_options, &dnxf_show_counters_man,                 NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY,  sh_cmd_is_dnxf},
#endif
    {"INTeRrupt", cmd_dnxc_show_intr,     NULL,    dnxc_show_intr_options,     &dnxc_show_intr_man,     NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"units",     cmd_dnxc_show_units,    NULL,    dnxc_show_units_options,    &dnxc_show_units_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_show_man = {
    "Miscellaneous facilities to show PVT, Patch, Interrupts and MIB counters diagnostic"
};

sh_sand_cmd_t sh_dnxc_clear_cmds[] = {
    /*keyword,    action,                    command, options,                       man                               */
    {"counter",   cmd_dnxc_clear_counters,   NULL,    dnxc_clear_counters_options,   &dnx_clear_counters_man,   NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnx},
    {"counter",   cmd_dnxc_clear_counters,   NULL,    dnxc_clear_counters_options,   &dnxf_clear_counters_man,  NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnxf},
    {"INTeRrupt", cmd_dnxc_clear_interrupts, NULL,    dnxc_clear_interrupts_options, &dnxc_clear_interrupts_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_clear_man = {
    "Used to clear miscellaneous diagnostic statistic information"
};
/* *INDENT-ON* */
