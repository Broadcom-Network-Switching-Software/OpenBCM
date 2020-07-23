/** \file diag_dnx_crps.c
 *
 * Main diagnostics for crps applications
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
/** bcm */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <src/bcm/dnx/stat/crps/crps_eviction.h>
#include <src/bcm/dnx/stat/crps/crps_engine.h>
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>

/** sal */
#include <sal/appl/sal.h>
#include "diag_dnx_crps.h"

/*************
 * TYPEDEFS  *
 */
typedef struct diag_dnx_crps_engine_source_s
{
    bcm_stat_counter_interface_type_t source_idx;
    char *source_str;
} diag_dnx_crps_engine_source_t;

static const diag_dnx_crps_engine_source_t source_enum_to_char_binding_table[] = {
    {bcmStatCounterInterfaceIngressReceivePp, "bcmStatCounterInterfaceIngressReceivePp"},
    {bcmStatCounterInterfaceIngressOam, "bcmStatCounterInterfaceIngressOam"},
    {bcmStatCounterInterfaceIngressVoq, "bcmStatCounterInterfaceIngressVoq"},
    {bcmStatCounterInterfaceIngressTransmitPp, "bcmStatCounterInterfaceIngressTransmitPp"},
    {bcmStatCounterInterfaceEgressReceivePp, "bcmStatCounterInterfaceEgressReceivePp"},
    {bcmStatCounterInterfaceEgressReceiveQueue, "bcmStatCounterInterfaceEgressReceiveQueue"},
    {bcmStatCounterInterfaceEgressTransmitPp, "bcmStatCounterInterfaceEgressTransmitPp"},
    {bcmStatCounterInterfaceEgressOam, "bcmStatCounterInterfaceEgressOam"},
    {bcmStatCounterInterfaceOamp, "bcmStatCounterInterfaceOamp"}
};

typedef struct diag_dnx_crps_engine_format_s
{
    bcm_stat_counter_format_type_t format_idx;
    char *format_str;
} diag_dnx_crps_engine_format_t;

static const diag_dnx_crps_engine_format_t format_enum_to_char_binding_table[] = {
    {bcmStatCounterFormatPacketsAndBytes, "bcmStatCounterFormatPacketsAndBytes"},
    {bcmStatCounterFormatPackets, "bcmStatCounterFormatPackets"},
    {bcmStatCounterFormatBytes, "bcmStatCounterFormatBytes"},
    {bcmStatCounterFormatDoublePackets, "bcmStatCounterFormatDoublePackets"},
    {bcmStatCounterFormatMaxSize, "bcmStatCounterFormatMaxSize"},
    {bcmStatCounterFormatIngressLatency, "bcmStatCounterFormatIngressLatency"}
};

typedef struct diag_dnx_crps_expansion_type_s
{
    bcm_stat_counter_format_type_t type_idx;
    char *exp_type_str;
} diag_dnx_crps_expansion_type_t;

static const diag_dnx_crps_expansion_type_t exp_type_enum_to_char_binding_table[] = {
    {bcmStatExpansionTypeTmDropReason, "TmDropReason"},
    {bcmStatExpansionTypeDispositionIsDrop, "DispositionIsDrop"},
    {bcmStatExpansionTypeDropPrecedenceMeter0Valid, "DropPrecedenceMeter0Valid"},
    {bcmStatExpansionTypeDropPrecedenceMeter0Value, "DropPrecedenceMeter0Value"},
    {bcmStatExpansionTypeDropPrecedenceMeter1Valid, "DropPrecedenceMeter1Valid"},
    {bcmStatExpansionTypDropPrecedenceMeter1Value, "DropPrecedenceMeter1Value"},
    {bcmStatExpansionTypeDropPrecedenceMeter2Valid, "DropPrecedenceMeter2Valid"},
    {bcmStatExpansionTypeDropPrecedenceMeter2Value, "DropPrecedenceMeter2Value"},
    {bcmStatExpansionTypeTrafficClass, "TrafficClass"},
    {bcmStatExpansionTypeDropPrecedenceMeterResolved, "DropPrecedenceMeterResolved"},
    {bcmStatExpansionTypeDropPrecedenceInput, "DropPrecedenceInput"},
    {bcmStatExpansionTypeSystemMultiCast, "SystemMultiCast"},
    {bcmStatExpansionTypeEcnEligibleAndCni, "EcnEligibleAndCni"},
    {bcmStatExpansionTypePortMetaData, "PortMetaData"},
    {bcmStatExpansionTypePpDropReason, "PpDropReason"},
    {bcmStatExpansionTypeMetaData, "MetaData"},
    {bcmStatExpansionTypeDiscardPp, "DiscardPp"},
    {bcmStatExpansionTypeDiscardTm, "DiscardTm"},
    {bcmStatExpansionTypeEgressTmActionType, "EgressTmActionType"},
    {bcmStatExpansionTypeLatencyBin, "LatencyBin"},
    {bcmStatExpansionTypeLatencyFlowProfile, "LatencyFlowProfile"},
    {bcmStatExpansionTypeTmLastCopy, "TmLastCopy"},
};

/*************
 * DEFINES   *
 */
#define DIAG_DNX_CRPS_NO_OPTION     -1
#define DIAG_DNX_CRPS_DATABASE_ID_CB 2
#define DIAG_DNX_CRPS_COMMAND_ID_CB 4

static shr_error_e
sh_dnx_get_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int engine_id, valid;
    int core_id, core_idx, type_id;
    int counter_set;
    int database_id;
    int flags = 0;
    int nstat, offset_id;
    int stat_arr[DNX_DATA_MAX_CRPS_ENGINE_MAX_COUNTER_SET_SIZE * 2];
    int is_bytes = 0;
    int found, counter_id, sub_count;
    int base_engine_id;
    bcm_stat_engine_t engine;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t config;
    bcm_stat_counter_input_data_t implicit_input_data;
    bcm_stat_counter_explicit_input_data_t explicit_input_data;
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t value_arr[DNX_DATA_MAX_CRPS_ENGINE_MAX_COUNTER_SET_SIZE * 2];

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("engine", engine_id);
    SH_SAND_GET_INT32("database", database_id);
    SH_SAND_GET_INT32("set", counter_set);
    SH_SAND_GET_INT32("type", type_id);
    SH_SAND_GET_INT32("core", core_id);

    /** check chosen options   */
    if ((engine_id != DIAG_DNX_CRPS_NO_OPTION && database_id != DIAG_DNX_CRPS_NO_OPTION)
        || (engine_id == DIAG_DNX_CRPS_NO_OPTION && database_id == DIAG_DNX_CRPS_NO_OPTION))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d for options give engine OR database \n", unit);
    }

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        database.core_id = core_idx;
        if (database_id != DIAG_DNX_CRPS_NO_OPTION)
        {
            database.database_id = database_id;
            if (_SHR_E_NONE != dnx_crps_mgmt_database_structure_verify(unit, &database, FALSE))
            {
                LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"),
                         unit, database.core_id, database.database_id));
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database_id, &engine_id));
            if (engine_id == DNX_CRPS_MGMT_ENGINE_INVALID)
            {
                LOG_CLI((BSL_META("No engines attached to database : unit=%d core_id=%d database_id=%d\n"),
                         unit, database.core_id, database.database_id));
                continue;
            }
        }
        /** engine key - find the database it belong to */
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get
                            (unit, core_idx, engine_id, &database.database_id));
        }
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid);
        if (valid == FALSE)
        {
            LOG_CLI((BSL_META("engine_id=%d / database_id=%d, core_id=%d is not active \n"),
                     engine_id, database_id, core_idx));
            continue;
        }
        engine.engine_id = engine_id;
        engine.core_id = core_idx;

        PRT_TITLE_SET("Counter values for unit=%d  core=%d engine_id=%d / database_id=%d", unit,
                      database.core_id, engine_id, database.database_id);

        SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &config));
        if (!config.type_config[type_id].valid)
        {
            PRT_INFO_ADD("for engine_id=%d type_id=%d is not valid\n", engine_id, type_id);
        }
        else
        {
            PRT_COLUMN_ADD("Offset_ID");
            PRT_COLUMN_ADD("Counter Value");
            PRT_COLUMN_ADD("Format");
            PRT_COLUMN_ADD("Counter ID");
            PRT_COLUMN_ADD("Engine ID");
            if (config.format_type == bcmStatCounterFormatPacketsAndBytes)
            {
                nstat = config.counter_set_size * 2;
            }
            else
            {
                nstat = config.counter_set_size;
            }
            output_data.value_arr = value_arr;
            /** set array */
            for (offset_id = 0; offset_id < nstat; offset_id++)
            {
                if (config.format_type == bcmStatCounterFormatPacketsAndBytes)
                {
                    /** first packets, then bytes */
                    if (offset_id >= config.counter_set_size)
                    {
                        is_bytes = 1;
                    }
                    else
                    {
                        is_bytes = 0;
                    }
                }
                stat_arr[offset_id] = BCM_STAT_COUNTER_STAT_SET(is_bytes, (offset_id % config.counter_set_size));
            }
            if (database_id == DIAG_DNX_CRPS_NO_OPTION)
            {
                explicit_input_data.engine = engine;
                explicit_input_data.object_stat_id = counter_set;
                explicit_input_data.type_id = type_id;
                explicit_input_data.nstat = nstat;
                explicit_input_data.stat_arr = stat_arr;
                SHR_IF_ERR_EXIT(bcm_stat_counter_explicit_get(unit, flags, &explicit_input_data, &output_data));
            }
            else
            {
                implicit_input_data.core_id = core_idx;
                implicit_input_data.database_id = database_id;
                implicit_input_data.command_id = config.command_id;
                implicit_input_data.counter_source_id = counter_set;
                implicit_input_data.type_id = type_id;
                implicit_input_data.nstat = nstat;
                implicit_input_data.stat_arr = stat_arr;
                SHR_IF_ERR_EXIT(bcm_stat_counter_get(unit, flags, &implicit_input_data, &output_data));
            }

            base_engine_id = engine_id; /** save the base engine id*/
            /** print*/
            for (offset_id = 0; offset_id < nstat; offset_id++)
            {
                /** First calculate the counter id and the engine id it
                 *  belongs to */
                 /** In case of  given database_id - need to calculate the
                  *  counter_id and the engine_id - do this in the offset loop
                  *  as the set might be split in two engines so the check
                  *  will be done for each offset */
                if (database_id != DIAG_DNX_CRPS_NO_OPTION)
                {
                    engine_id = base_engine_id; /** per each offset iteration start from the base engine */
                    /** to find to which engine the counters belong to need to go
                    *  over the database */
                    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
                    {
                        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_id_calc
                                        (unit, core_idx, engine_id, type_id, counter_set, stat_arr[offset_id], &found,
                                         &counter_id, &sub_count));
                        if (found)
                        {
                            break;
                        }
                        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                        next_engine_id.get(unit, core_idx, engine_id, &engine_id));
                    }
                }
                /** In case of given engine_id - only need to calculate the counter_id */
                else
                {
                    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_id_calc
                                    (unit, core_idx, engine_id, type_id, counter_set, stat_arr[offset_id], &found,
                                     &counter_id, &sub_count));
                }

                /** Once the calculation is ready - print the table */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                if (config.format_type == bcmStatCounterFormatPacketsAndBytes)
                {
                    if (offset_id >= config.counter_set_size)
                    {
                        PRT_CELL_SET("%d", (offset_id % config.counter_set_size));
                        PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                     COMPILER_64_LO(output_data.value_arr[offset_id].value));
                        PRT_CELL_SET("Bytes");
                    }
                    else
                    {
                        PRT_CELL_SET("%d", (offset_id % config.counter_set_size));
                        PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                     COMPILER_64_LO(output_data.value_arr[offset_id].value));
                        PRT_CELL_SET("Packets");
                    }
                }
                else if (config.format_type == bcmStatCounterFormatBytes)
                {
                    PRT_CELL_SET("%d", offset_id);
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                 COMPILER_64_LO(output_data.value_arr[offset_id].value));
                    PRT_CELL_SET("Bytes");
                }
                else if (config.format_type == bcmStatCounterFormatMaxSize)
                {
                    PRT_CELL_SET("%d", offset_id);
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                 COMPILER_64_LO(output_data.value_arr[offset_id].value));
                    PRT_CELL_SET(" -- ");
                }
                else
                {
                    PRT_CELL_SET("%d", offset_id);
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                 COMPILER_64_LO(output_data.value_arr[offset_id].value));
                    PRT_CELL_SET("Packets");
                }
                /** print counter id and engine id */
                PRT_CELL_SET("%d", counter_id);
                PRT_CELL_SET("%d", engine_id);
            }
        }
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_attach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    /** attach DMA FIFO */
    result = dnx_crps_dma_fifo_full_attach(unit, BCM_CORE_ALL);
    if (_SHR_E_NONE != result)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor DMA FIFO failed to attach: %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

    /** attach BG thread */
    SHR_IF_ERR_EXIT(bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, TRUE));
    LOG_CLI((BSL_META("unit %d counter processor DMA FIFO is attached and background access resumed\n"), unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_detach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** detach BG thread */
    SHR_IF_ERR_EXIT(bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, FALSE));
    result = dnx_crps_eviction_dma_fifo_full_detach(unit, BCM_CORE_ALL);
    if (BCM_E_NONE == result)
    {
        LOG_CLI((BSL_META("unit %d counter processor background access suspended and DMA FIFO is detached \n"), unit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor DMA FIFO detach failed %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_suspend_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    result = bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, FALSE);
    if (BCM_E_NONE == result)
    {
        LOG_CLI((BSL_META("unit %d counter processor background accesses suspended \n"), unit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor background suspend failed: %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_resume_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    result = bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, TRUE);
    if (BCM_E_NONE == result)
    {
        LOG_CLI((BSL_META("unit %d counter processor background accesses resumed \n"), unit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor background accesses resume failed: %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 engine_id, flags = 0;
    int core_id, core_idx;
    int valid_engine = 0, type_id, valid_type;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    bcm_stat_engine_t engine;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t interface;
    bcm_stat_eviction_t eviction;
    dnx_engine_log_obj_id_range_t range;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("engine", engine_id);
    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid_engine);
        if (!valid_engine)
        {
            LOG_CLI((BSL_META("unit %d core=%d - engine_id=%d is not active \n"), unit, core_idx, engine_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get
                            (unit, core_idx, engine_id, &database.database_id));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.get(unit, core_idx, engine_id, &range));

            database.core_id = core_idx;
            PRT_TITLE_SET(" Interface configuration");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Configuration");
            PRT_COLUMN_ADD("Value");
            SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &interface));
            engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
            /** interface*/
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("engine size");
            PRT_CELL_SET("%d", engine_info->nof_counters);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("source");
            PRT_CELL_SET("%s", source_enum_to_char_binding_table[interface.source].source_str);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("command_id");
            PRT_CELL_SET("%d", interface.command_id);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("counter_set_size");
            PRT_CELL_SET("%d", interface.counter_set_size);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("format");
            PRT_CELL_SET("%s", format_enum_to_char_binding_table[interface.format_type].format_str);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("logical_range_start");
            PRT_CELL_SET("%d", range.logical_object_id_first);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("logical_range_end");
            PRT_CELL_SET("%d", range.logical_object_id_last);

            PRT_COMMITX;
            PRT_TITLE_SET(" valid Types for the engine");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Type_ID");
            PRT_COLUMN_ADD("valid_type");
           /**  type config */
            for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", type_id);
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.type_hw_valid.get
                                (unit, core_idx, engine_id, type_id, &valid_type));
                PRT_CELL_SET("%d", valid_type);
            }
            PRT_COMMITX;
            /** data mapping is in diag "info counterset" */
            /** eviction*/
            engine.core_id = core_idx;
            engine.engine_id = engine_id;
            SHR_IF_ERR_EXIT(bcm_stat_counter_eviction_get(unit, flags, &engine, &eviction));
            PRT_TITLE_SET(" Eviction configuration");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Configuration");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("dma_fifo");
            PRT_CELL_SET("%d", eviction.dma_fifo_select);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("destination_type");
            PRT_CELL_SET("%d", eviction.type);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("record_format");
            PRT_CELL_SET("%d", eviction.record_format);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("eviction_event_id");
            PRT_CELL_SET("%d", eviction.eviction_event_id);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("algorithmic_disable");
            PRT_CELL_SET("%d", eviction.eviction_algorithmic_disable);
            PRT_COMMITX;
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_info_ctr_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 database_id, flags = 0;
    int core_id, core_idx, ind, i;
    int type_id;
    bcm_stat_engine_t engine;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t interface;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion;
    int expansion_array[DNX_DATA_MAX_CRPS_EXPANSION_EXPANSION_SIZE][2] = { {0} };
    int array_nof_elements = 0;
    int array_row_i, array_row_j, slap, temp;
    uint32 metadata_bitmap, size, offset;
    int bitmap_set_bits;
    int mem_idx, offset_id, type_index = 0;
    char data_mapping_buf[20] = "";
    char data_mapping_str[DNX_DATA_MAX_CRPS_ENGINE_DATA_MAPPING_TABLE_SIZE * 2] = "";
    bcm_stat_expansion_data_value_t value;
    const dnx_data_crps_expansion_itm_types_t *itm_types;
    const dnx_data_crps_expansion_etm_types_t *etm_types;
    const bcm_stat_expansion_types_t *valid_types;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("database", database_id);
    SH_SAND_GET_INT32("type", type_id);
    SH_SAND_GET_INT32("core", core_id);

    database.database_id = database_id;

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        database.core_id = core_idx;
        /** check that database created and specific core_id */
        if (_SHR_E_NONE != dnx_crps_mgmt_database_structure_verify(unit, &database, FALSE))
        {
            LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"),
                     unit, database.core_id, database.database_id));
            continue;
        }

        SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &interface));
        if (interface.type_config[type_id].valid != TRUE)
        {
            LOG_CLI((BSL_META
                     ("type id verification failed: unit=%d core_id=%d database_id=%d type_id=%d is not valid\n"), unit,
                     database.core_id, database.database_id, type_id));
            continue;
        }
        itm_types = dnx_data_crps.expansion.itm_types_get(unit);
        etm_types = dnx_data_crps.expansion.etm_types_get(unit);
         /** expansion is not valid for source  OAMP, so for it
         *  only the datamapping will be printed, otherwise - first print
         *  the expansion configuration */
        if (interface.source != bcmStatCounterInterfaceOamp)
        {
            PRT_TITLE_SET(" Expansion format");
            PRT_INFO_ADD("unit=%d database_id=%d core_id=%d", unit, database_id, core_idx);

            for (ind = dnx_data_crps.expansion.expansion_size_get(unit) - 1; ind >= 0; ind--)
            {
                PRT_COLUMN_ADD("Bit%d", ind);
            }
            interface_key.core_id = core_idx;
            interface_key.interface_source = interface.source;
            interface_key.command_id = interface.command_id;
            interface_key.type_id = type_id;
            if (interface.source == bcmStatCounterInterfaceIngressVoq
                || interface.source == bcmStatCounterInterfaceEgressReceiveQueue)
            {
                 /** Get the valid expansion types for source ITM/ETM */
                valid_types =
                    (interface.source ==
                     bcmStatCounterInterfaceIngressVoq) ? itm_types->valid_types : etm_types->valid_types;
                /** Go over the valid expansion types and fill the expansion
                 *  structure */
                while (valid_types[type_index] != bcmStatExpansionTypeMax)
                {
                    expansion.expansion_elements[type_index].type = valid_types[type_index];
                    type_index++;
                }
                expansion.nof_elements = type_index;
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_stat_counter_expansion_select_get(unit, flags, &interface_key, &expansion));
            }
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            array_nof_elements = 0;
            /** get total bits */
            for (ind = 0; ind < expansion.nof_elements; ind++)
            {
                /** get field size */
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                                (unit, core_idx, interface.source, interface.command_id,
                                 expansion.expansion_elements[ind].type, &size, &offset));
                /** for metadata metadata_bitmap holds the relevant metadata
                 *  bitmap, for other types it is returned from the function
                 *  together with the offset */
                if (expansion.expansion_elements[ind].type == bcmStatExpansionTypeMetaData
                    || expansion.expansion_elements[ind].type == bcmStatExpansionTypePortMetaData)
                {
                    metadata_bitmap = expansion.expansion_elements[ind].bitmap;
                    SHR_BITCOUNT_RANGE(&metadata_bitmap, bitmap_set_bits, 0, (sizeof(uint32) * 8));
                    size = bitmap_set_bits;
                }
                for (i = 0; i < size; i++)
                {
                    expansion_array[array_nof_elements][0] = offset;
                    expansion_array[array_nof_elements][1] = expansion.expansion_elements[ind].type;
                    array_nof_elements++;
                }
            }
            /** sort the array according to its offset in descending order
             *  lowest offset is in the expansion LSB */
            for (array_row_i = 0; array_row_i < array_nof_elements; array_row_i++)
            {
                for (array_row_j = array_row_i + 1; array_row_j < array_nof_elements; array_row_j++)
                {
                    if (expansion_array[array_row_i][0] < expansion_array[array_row_j][0])
                    {
                        for (slap = 0; slap < 2; slap++)
                        {
                            temp = expansion_array[array_row_i][slap];
                            expansion_array[array_row_i][slap] = expansion_array[array_row_j][slap];
                            expansion_array[array_row_j][slap] = temp;
                        }
                    }
                }
            }
            i = 0;
            for (ind = DNX_DATA_MAX_CRPS_EXPANSION_EXPANSION_SIZE; ind > 0; ind--)
            {
                if (ind > array_nof_elements)
                {
                    PRT_CELL_SET(" -- ");
                }
                else
                {
                    PRT_CELL_SET("%s", exp_type_enum_to_char_binding_table[expansion_array[i][1]].exp_type_str);
                    i++;
                }
            }
            PRT_COMMITX;
        }
        /** data mapping*/
        engine.core_id = core_idx;
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database_id, &engine.engine_id));

        PRT_TITLE_SET(" Data mapping configuration");
        PRT_INFO_ADD(" counter_set_size = %d", interface.counter_set_size);
        PRT_COLUMN_ADD("Counter_set offset");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Expansion Values according the Expansion format table");
        for (offset_id = 0; offset_id < interface.counter_set_size; offset_id++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", offset_id);
            /** make the row string */
            sal_memset(data_mapping_str, '\0', sizeof(data_mapping_str));
            for (mem_idx = 0; mem_idx < DNX_DATA_MAX_CRPS_ENGINE_DATA_MAPPING_TABLE_SIZE; mem_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping_hw_get(unit, &engine, mem_idx, &value));
                if (value.valid == 1 && value.counter_set_offset == offset_id)
                {
                    sal_snprintf(data_mapping_buf, sizeof(data_mapping_buf), "%d ", mem_idx);
                    sal_strcat(data_mapping_str, data_mapping_buf);
                }
            }
            PRT_CELL_SET("%s", data_mapping_str);
        }
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_last_data_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 engine_id;
    int core_id, core_idx;
    int valid_engine = 0;
    uint32 last_address_value;
    uint64 last_data_value;
    bcm_stat_engine_t engine;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("engine", engine_id);
    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid_engine);
        if (!valid_engine)
        {
            LOG_CLI((BSL_META("unit %d core=%d - engine_id=%d is not active \n"), unit, core_idx, engine_id));
        }
        else
        {
            engine.engine_id = engine_id;
            engine.core_id = core_idx;
            SHR_IF_ERR_EXIT(dnx_crps_engine_last_command_hw_get(unit, &engine, &last_address_value, &last_data_value));
            PRT_TITLE_SET(" LAST COMMAND");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Field");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Last address admitted");
            PRT_CELL_SET("%d", last_address_value);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Last data admitted");
            PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(last_data_value), COMPILER_64_LO(last_data_value));
            PRT_COMMITX;
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_last_input_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 database_id, entry_handle_id, interface_id, interface_id_get = 0, is_enable = 0, is_input_command_enabled =
        0, enable_is_present = 0;
    uint32 read_write, incr, type_id, expansion, counter_pointer, pkt_queue_size[2];
    int core_id;
    bcm_stat_counter_database_t database;
    uint8 is_created;
    const dnx_data_crps_engine_source_base_t *src_base;
    const dnx_data_crps_src_interface_command_id_t *command_data;
    bcm_stat_counter_interface_t config;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("database", database_id);
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_BOOL("enable", is_enable);
    SH_SAND_IS_PRESENT("enable", enable_is_present);

    if (FALSE == dnx_data_crps.engine.feature_get(unit, dnx_data_crps_engine_last_input_command_is_supported))
    {
        LOG_CLI((BSL_META("Diagnostic not supported for this device : unit=%d core_id=%d\n"), unit, core_id));
        SHR_EXIT();
    }
    database.database_id = database_id;
    database.core_id = core_id;
    SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated(unit, database.core_id, database.database_id, &is_created));
    if (is_created == FALSE)
    {
        LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"), unit, database.core_id,
                 database.database_id));
        SHR_EXIT();
    }
    /** the source_select value is unique per source and
     *  command_id.  each source has it's base value,
     *  which need to add the command_id in order to define the
     *   unique value */
    /** get the command id set for the given database */
    SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, 0, &database, &config));
    src_base = dnx_data_crps.engine.source_base_get(unit, config.source);
    command_data = dnx_data_crps.src_interface.command_id_get(unit, config.source);
    interface_id = src_base->base_val + config.command_id - command_data->base;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_LAST_INPUT_COMMAND, &entry_handle_id));
    /** If command is called to enable(disable) the given database
     *  need to set the enable(disable) last input command and set
     *  the interface id according to the command id of the
     *  database */
    if (enable_is_present)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, is_enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_ID, INST_SINGLE, interface_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** first check if the command was enabled and is set for
         *  this  */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &is_input_command_enabled);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INTERFACE_ID, INST_SINGLE, &interface_id_get);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (is_input_command_enabled != 1 || interface_id_get != interface_id)
        {
            LOG_CLI((BSL_META
                     ("Last input command is not enabled for the given database, please first call 'crps last command enable database=%d' : unit=%d core_id=%d \n"),
                     database_id, unit, core_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_LAST_INPUT_COMMAND, &entry_handle_id));
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_READ_WRITE, INST_SINGLE, &read_write);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INCR, INST_SINGLE, &incr);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TYPE_ID, INST_SINGLE, &type_id);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXPANSION, INST_SINGLE, &expansion);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER_POINTER, INST_SINGLE,
                                       &counter_pointer);
            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PKT_OR_QUEUE_SIZE, INST_SINGLE,
                                           pkt_queue_size);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            PRT_TITLE_SET(" LAST INPUT COMMAND");
            PRT_INFO_ADD("unit=%d database=%d core_id=%d", unit, database_id, core_id);
            PRT_COLUMN_ADD("Field");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Read/Write");
            PRT_CELL_SET((read_write == 1) ? "Read" : "Write");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Increase");
            PRT_CELL_SET((incr == 1) ? "Yes" : "No");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Type id");
            PRT_CELL_SET("%d", type_id);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Exapnsion Value");
            PRT_CELL_SET("%d", expansion);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Counter Pointer");
            PRT_CELL_SET("%d", counter_pointer);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Packet/Queue Size");
            PRT_CELL_SET("0x%08x%08x", pkt_queue_size[1], pkt_queue_size[0]);
            PRT_COMMITX;
        }
    }
exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int database_id, engine_id = 0;
    int current_engine_id, base_engine_id;
    int core_id, core_idx, type_id;
    uint32 flags = 0;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int total_size = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t interface;
    int first_database, last_database;
    uint8 is_created;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("database", database_id);
    SH_SAND_GET_INT32("core", core_id);

    if (database_id == -1)
    {
        first_database = 0;
           /** nof databases is the same as nof engines */
        last_database = dnx_data_crps.engine.nof_engines_get(unit) - 1;
    }
    else
    {
        first_database = database_id;
        last_database = database_id;
    }

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        for (database_id = first_database; database_id <= last_database; database_id++)
        {
            database.database_id = database_id;
            database.core_id = core_idx;

            SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated
                            (unit, database.core_id, database.database_id, &is_created));
            if (is_created == FALSE)
            {
                LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"),
                         unit, database.core_id, database.database_id));
                continue;
            }

            PRT_TITLE_SET(" Engines in the database");
            PRT_INFO_ADD("unit=%d database_id=%d core_id=%d", unit, database_id, core_idx);
            PRT_COLUMN_ADD("Engine_id");
            PRT_COLUMN_ADD("Engine size");

            SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database_id, &base_engine_id));
            current_engine_id = base_engine_id;

            /**    go over the linked list of the database */
            while (current_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
            {
                engine_id = current_engine_id;
                engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", engine_id);
                PRT_CELL_SET("%d", engine_info->nof_counters);
                total_size += engine_info->nof_counters;
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                next_engine_id.get(unit, core_idx, engine_id, &current_engine_id));
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Total size: ");
            PRT_CELL_SET("%d", total_size);
            PRT_COMMITX;

            SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &interface));
            /** interface*/
            PRT_TITLE_SET(" Type configuration");
            PRT_INFO_ADD("unit=%d database_id=%d core_id=%d", unit, database_id, core_idx);
            PRT_INFO_ADD("counter_set_size=%d", interface.counter_set_size);
            PRT_COLUMN_ADD("Type_ID");
            PRT_COLUMN_ADD("Type_Offset");
            PRT_COLUMN_ADD("Start");
            PRT_COLUMN_ADD("End");
            /**  type config */
            for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", type_id);
                if (interface.type_config[type_id].valid)
                {
                    PRT_CELL_SET("%d", interface.type_config[type_id].object_type_offset);
                    PRT_CELL_SET("%d", interface.type_config[type_id].start);
                    PRT_CELL_SET("%d", interface.type_config[type_id].end);
                }
                else
                {
                    PRT_CELL_SET(" -- ");
                    PRT_CELL_SET(" -- ");
                    PRT_CELL_SET(" -- ");
                }
            }
            PRT_COMMITX;
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Init callback function for "crps get/infol" diag commands
 *    Create crps databases
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_crps_init_cb(
    int unit)
{
    int flags = 0, core = BCM_CORE_ALL;
    int i, type_id, command_id = DIAG_DNX_CRPS_COMMAND_ID_CB, database_id = DIAG_DNX_CRPS_DATABASE_ID_CB, nof_types = 4;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_eviction_t eviction;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    bcm_stat_expansion_data_mapping_t data_mapping_array[8];
    int counter_set_size;
    int full_color[2][4] = { {0, 2, 2, 2}, {1, 3, 4, -1} };     /* [disposition][color]= *
                                                                 * {{fwd_grn,fwd_yel,fwd_red,fwd_blk}, *
                                                                 * {drp_grn,drp_yel,drp_red,drp_blk}} */
    int reject, color;
    int nof_engines = 4, engine_id_arr[4] = { 1, 6, 12, 13 }, type_valid[4] =
    {
    1, 1, 1, 1};
    uint32 type_offset[4] = { 0, 0, 0, 0 }, start_object_stat_id_arr[4] =
    {
    0, 0, 0, 0}, end_object_stat_id_arr[4];
    const dnx_data_crps_engine_engines_info_t *engine_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** expansion selection strucutre. */
    expansion_select.nof_elements = 2;
    expansion_select.expansion_elements[0].type = bcmStatExpansionTypeDispositionIsDrop;
    expansion_select.expansion_elements[1].type = bcmStatExpansionTypeDropPrecedenceMeterResolved;

    /** counter set mappping strucutre. */
    counter_set_size = 5;
    interface_key.core_id = core;
    interface_key.command_id = command_id;
    interface_key.interface_source = bcmStatCounterInterfaceIngressReceivePp;

     /** configure type 0 */
    /** each type range holds 1 engine full entries */
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id_arr[0]);
    end_object_stat_id_arr[0] = (engine_info->nof_counters) / counter_set_size - 1;
    type_offset[0] = 0;

    /** configure type 1 */
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id_arr[1]);
    end_object_stat_id_arr[1] = (engine_info->nof_counters) / counter_set_size - 1;
    /** offset will be the prev offset + the end point of the prev type */
    type_offset[1] = (end_object_stat_id_arr[0] - start_object_stat_id_arr[0] + 1) + type_offset[0];

    /** configure type 2 */
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id_arr[2]);
    end_object_stat_id_arr[2] = (engine_info->nof_counters) / counter_set_size - 1;
    type_offset[2] = (end_object_stat_id_arr[1] - start_object_stat_id_arr[1] + 1) + type_offset[1];

    /** configure type 3 */
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id_arr[3]);
    end_object_stat_id_arr[3] = (engine_info->nof_counters) / counter_set_size - 1;
    type_offset[3] = (end_object_stat_id_arr[2] - start_object_stat_id_arr[2] + 1) + type_offset[2];

    /** each type requires expansion selection. */
    for (i = 0; i < nof_types; i++)
    {
        interface_key.type_id = i;
        SHR_IF_ERR_EXIT(bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select));
    }

    /** step 2: configure and create database - use user selected
     *  database_id */
    database.database_id = database_id;
    database.core_id = core;
    SHR_IF_ERR_EXIT(bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, &database_id));

    /** configure the structure for API bcm_stat_counter_eviction_set */
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
    eviction.type = bcmStatEvictionDestinationLocalHost;
    /** calling the APIs per engine, in the right order - attach
     *  engines to the database and select eviction */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core;
        engine.engine_id = engine_id_arr[i];
        /** step 3: attach the engines to the database */
        SHR_IF_ERR_EXIT(bcm_stat_counter_engine_attach(unit, 0, &database, engine_id_arr[i]));
       /** step 4: configure the counter eviction per engine */
        SHR_IF_ERR_EXIT(bcm_stat_counter_eviction_set(unit, flags, &engine, &eviction));
    }
     /** step 5: configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceIngressReceivePp;
    cnt_interface.command_id = command_id;
    cnt_interface.format_type = bcmStatCounterFormatPacketsAndBytes;
    cnt_interface.counter_set_size = counter_set_size;

    for (type_id = 0; type_id < nof_types; type_id++)
    {
        /** get type validity and offset from prior configuration */
        cnt_interface.type_config[type_id].valid = type_valid[type_id];
        cnt_interface.type_config[type_id].object_type_offset = type_offset[type_id];
        cnt_interface.type_config[type_id].start = start_object_stat_id_arr[type_id];
        cnt_interface.type_config[type_id].end = end_object_stat_id_arr[type_id];
    }
    SHR_IF_ERR_EXIT(bcm_stat_counter_interface_set(unit, flags, &database, &cnt_interface));

     /** configure 8 entries in order to create the following
     * counter_set mapping: {fwd grn, drop grn, fwd not grn, drop 
     * yel, drop red} */
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    ctr_set_map.nof_entries = 0;
    for (color = bcmColorGreen; color <= bcmColorBlack; color++)
    {
        for (reject = 0; reject < 2; reject++)
        {
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].nof_key_conditions = 2;
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].key[0].type =
                bcmStatExpansionTypeDispositionIsDrop;
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].key[0].value = reject;
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].key[1].type =
                bcmStatExpansionTypeDropPrecedenceMeterResolved;
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].key[1].value = color;
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].value.counter_set_offset =
                ((full_color[reject][color] == -1) ? 0 : full_color[reject][color]);
            ctr_set_map.expansion_data_mapping[ctr_set_map.nof_entries].value.valid =
                (full_color[reject][color] == -1) ? FALSE : TRUE;
            ctr_set_map.nof_entries++;
        }
    }
    /** step 6: configure the counter set mapping */
    SHR_IF_ERR_EXIT(bcm_stat_counter_set_mapping_set(unit, flags, &database, &ctr_set_map));

    /** step 7: enable the database */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    SHR_IF_ERR_EXIT(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Deinit callback function for "crps info/get" diag commands
 *    Destroy crps database
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_crps_deinit_cb(
    int unit)
{
    int flags = 0, core = BCM_CORE_ALL, database_id = DIAG_DNX_CRPS_DATABASE_ID_CB;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_enable_t enable;

    SHR_FUNC_INIT_VARS(unit);

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_counter_enable_t_init(&enable);

    database.core_id = core;
    database.database_id = database_id;
    enable.enable_counting = FALSE;
    enable.enable_eviction = FALSE;
    SHR_IF_ERR_EXIT(bcm_stat_counter_database_enable_set(unit, flags, &database, &enable));
    SHR_IF_ERR_EXIT(bcm_stat_counter_database_destroy(unit, flags, &database));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Init callback function for "crps last_input" diag commands
 *    Create crps databases
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_crps_init_send_pkt_cb(
    int unit)
{
    uint32 entry_handle_id, interface_id;
    const dnx_data_crps_engine_source_base_t *src_base;
    const dnx_data_crps_src_interface_command_id_t *command_data;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** config crps database */
    SHR_IF_ERR_EXIT(sh_dnx_crps_init_cb(unit));
    /** enable manually the command before sending the packet and
    checking the last cmd  */
    src_base = dnx_data_crps.engine.source_base_get(unit, bcmStatCounterInterfaceIngressReceivePp);
    command_data = dnx_data_crps.src_interface.command_id_get(unit, bcmStatCounterInterfaceIngressReceivePp);
    interface_id = src_base->base_val + DIAG_DNX_CRPS_COMMAND_ID_CB - command_data->base;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_LAST_INPUT_COMMAND, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_ID, INST_SINGLE, interface_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

     /** send packet */
    SHR_IF_ERR_EXIT(diag_sand_packet_send_simple(unit));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Conditional check for crps last command support
 * \param [in] unit - unit ID 
 * \param [in] test_list - test list 
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_is_crps_last_input_command_supported(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_init_is_init_done_get(unit))
    {
        if (FALSE == dnx_data_crps.engine.feature_get(unit, dnx_data_crps_engine_last_input_command_is_supported))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
    }
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sh_enum_crps_engine[] = {
    {"max", 0, "Maximum engine number", "DNX_DATA.crps.engine.nof_engines-1"},
    {NULL}
};

static sh_sand_enum_t sh_enum_crps_database[] = {
    {"max", 0, "Maximum engine number", "DNX_DATA.crps.engine.nof_databases-1"},
    {NULL}
};

static sh_sand_option_t dnx_crps_get_arguments[] = {
    {"engine", SAL_FIELD_TYPE_INT32, "engine_id", "-1", sh_enum_crps_engine, "0-max"},
    {"database", SAL_FIELD_TYPE_INT32, "database_id", "-1", sh_enum_crps_database, "0-max"},
    {"set", SAL_FIELD_TYPE_INT32, "counter set index", NULL},
    {"type", SAL_FIELD_TYPE_INT32, "type id", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_get_man = {
    .brief = "get values of counters in a specific sw counter set.",
    .full = "'Get' command gets for a specific counter set all uin64 counter values in it. \n",
    .synopsis = NULL,
    .examples = "engine=1 set=2 type=0\n" "database=2 set=2 type=3 core=0\n" "engine=11 set=55 type=3",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_man_t sh_dnx_crps_attach_man = {
    .brief = "attach the DMA FIFO and BG thread \n",
    .full = "first attach all connected dma fifos, then attach background thread"
};

static sh_sand_man_t sh_dnx_crps_detach_man = {
    .brief = "detach the DMA FIFO and BG thread \n",
    .full = "first detach the background thread, then detach the connected dma fifos"
};

static sh_sand_man_t sh_dnx_crps_suspend_man = {
    .brief = "suspend the BG thread operation \n",
    .full = "suspend the background thread operation"
};

static sh_sand_man_t sh_dnx_crps_resume_man = {
    .brief = "resume the BG thread operation \n",
    .full = "resume the background thread operation"
};
static sh_sand_option_t dnx_crps_info_arguments[] = {
    {"engine", SAL_FIELD_TYPE_UINT32, "engine_id", NULL, sh_enum_crps_engine, "0-max"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_info_man = {
    .brief = "info - show engine configuration \n",
    .full = "with option engine - 3 tables - interface/type config/eviction \n"
        "command id, set size, source type, format type, database id, next engine \n"
        "valid types with offset, start, end \n" "dma fifo, event id, record format, destination"
        "with option counter_set - 2 tables - expnasion/ data mapping"
        "with option database - info for the database range",
    .synopsis = NULL,
    .examples = "crps info database database=2 core=0",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_man_t sh_dnx_crps_info_engine_man = {
    .brief = "info - show engine configuration \n",
    .full = "show engine configuration - 3 tables - interface/type config/eviction \n"
        "command id, set size, source type, format type, database id, next engine \n"
        "valid types with offset, start, end \n" "dma fifo, event id, record format, destination",
    .synopsis = NULL,
    .examples = "engine=1 core=0",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_option_t dnx_crps_info_ctr_set_arguments[] = {
    {"database", SAL_FIELD_TYPE_UINT32, "database_id", NULL, sh_enum_crps_database, "0-max"},
    {"type", SAL_FIELD_TYPE_INT32, "type_id", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_info_ctr_set_man = {
    .brief = "show database configuration - expansion and data mapping \n",
    .full = "show data mapping configuration \n",
    .synopsis = NULL,
    .examples = "database=2 type=3 core=0\n" "database=2 type=1 core=0\n" "database=2 type=2 core=0",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_option_t dnx_crps_last_data_options[] = {
    {"engine", SAL_FIELD_TYPE_UINT32, "engine_id", NULL, sh_enum_crps_engine, "0-max"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_last_data_man = {
    .brief = "last - show last address and data admitted per engine \n",
    .full = "",
    .synopsis = NULL,
    .examples = "engine=12 core=0\n" "engine=11 core=0",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_option_t dnx_crps_last_input_options[] = {
    {"database", SAL_FIELD_TYPE_UINT32, "database_id", NULL, sh_enum_crps_database, "0-max"},
    {"enable", SAL_FIELD_TYPE_BOOL, "enable", "0"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_last_input_man = {
    .brief =
        "The command should be called first with enable option with the selected database and core, then action needs to be made(send packet/etc.)\n"
        "After that - call the diagnostic without enable to see the following information: \n"
        "show last input command for given database - r/w command, on which type id,what expansion, counter pointer \n",
    .full = "",
    .synopsis = NULL,
    .examples = "enable database=2 core=0\n" "database=2 core=0\n" "enable=0 database=2 core=0",
    .init_cb = sh_dnx_crps_init_send_pkt_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_man_t sh_dnx_crps_last_man = {
    .brief = "last - show last data/command per engine/database \n",
    .full = "with option data - show last address and data admitted per engine \n"
        "with option command - show last input command for given interface \n"
        "table - r/w command, type id,expansion, counter pointer \n",
    .synopsis = NULL,
    .examples = "crps last data engine=12 core=0\n",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_option_t dnx_crps_show_options[] = {
    {"database", SAL_FIELD_TYPE_INT32, "database_id", "-1", sh_enum_crps_database, "0-max"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_show_man = {
    .brief = "show all engines in database and the types range \n",
    .full =
        "show all engines in database and the types range. \nif no databse_id give, it prsent the information for all configured databased",
    .synopsis = NULL,
    .examples = "database=2 core=0",
    .init_cb = sh_dnx_crps_init_cb,
    .deinit_cb = sh_dnx_crps_deinit_cb
};

static sh_sand_cmd_t sh_dnx_crps_info_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"engine", sh_dnx_info_cmd, NULL, dnx_crps_info_arguments, &sh_dnx_crps_info_engine_man},
    {"counterSet", sh_dnx_info_ctr_set_cmd, NULL, dnx_crps_info_ctr_set_arguments, &sh_dnx_crps_info_ctr_set_man},
    {"database", sh_dnx_show_cmd, NULL, dnx_crps_show_options, &sh_dnx_crps_show_man, NULL, NULL},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_crps_last_cmds[] = {
    {"data", sh_dnx_last_data_cmd, NULL, dnx_crps_last_data_options, &sh_dnx_crps_last_data_man},
    {"command", sh_dnx_last_input_cmd, NULL, dnx_crps_last_input_options, &sh_dnx_crps_last_input_man, NULL, NULL,
     SH_CMD_CONDITIONAL, sh_dnx_is_crps_last_input_command_supported},
    {NULL}
};

/**
 * \brief DNX CRPS diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for CRPS diagnostic commands 
 */
sh_sand_cmd_t sh_dnx_crps_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"Get", sh_dnx_get_counter_cmd, NULL, dnx_crps_get_arguments, &sh_dnx_crps_get_man},
    {"ATTach", sh_dnx_attach_cmd, NULL, NULL, &sh_dnx_crps_attach_man},
    {"DETach", sh_dnx_detach_cmd, NULL, NULL, &sh_dnx_crps_detach_man},
    {"SuSPenD", sh_dnx_suspend_cmd, NULL, NULL, &sh_dnx_crps_suspend_man},
    {"ReSuMe", sh_dnx_resume_cmd, NULL, NULL, &sh_dnx_crps_resume_man},
    {"INFO", NULL, sh_dnx_crps_info_cmds, NULL, &sh_dnx_crps_info_man},
    {"Last", NULL, sh_dnx_crps_last_cmds, NULL, &sh_dnx_crps_last_man},
    {NULL}
};

sh_sand_man_t sh_dnx_crps_man = {
    .brief = "CRPS diagnostics and commands \n"
};
