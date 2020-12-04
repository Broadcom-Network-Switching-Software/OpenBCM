/** \file diag_dnx_meter.c
 *
 * Main diagnostics for meter applications
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
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/meter_access.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <src/bcm/dnx/policer/meter_global.h>
#include <src/bcm/dnx/policer/meter_generic.h>
#include <src/bcm/dnx/policer/meter_utils.h>
#include <bcm_int/dnx/gtimer/gtimer.h>

/** sal */
#include <sal/appl/sal.h>
#include <sal/core/boot.h>
#include "diag_dnx_meter.h"

/*************
 * TYPEDEFS  *
 */
typedef struct diag_dnx_meter_global_type_s
{
    bcm_policer_fwd_types_t type;
    char type_str[10];
} diag_dnx_meter_global_type_t;

static const diag_dnx_meter_global_type_t diag_dnx_meter_global_type_enum_to_char_binding_table[] = {
    {bcmPolicerFwdTypeUc, "    UC   "},
    {bcmPolicerFwdTypeUnkownUc, "UnknownUC"},
    {bcmPolicerFwdTypeMc, "    MC   "},
    {bcmPolicerFwdTypeUnkownMc, "UnknownMC"},
    {bcmPolicerFwdTypeBc, "BC"}
};

typedef struct diag_dnx_meter_policer_mode_s
{
    bcm_policer_mode_t mode;
    char *mode_str;
} diag_dnx_meter_policer_mode_t;

static const diag_dnx_meter_policer_mode_t diag_dnx_meter_policer_mode_enum_to_char_binding_table[] = {
    {bcmPolicerModeSrTcm, "bcmPolicerModeSrTcm"},
    {bcmPolicerModeCommitted, "bcmPolicerModeCommitted"},
    {bcmPolicerModePeak, "bcmPolicerModePeak"},
    {bcmPolicerModeTrTcm, "bcmPolicerModeTrTcm"},
    {bcmPolicerModeTrTcmDs, "bcmPolicerModeTrTcmDs"},
    {bcmPolicerModeGreen, "bcmPolicerModeGreen"},
    {bcmPolicerModePassThrough, "bcmPolicerModePassThrough"},
    {bcmPolicerModeSrTcmModified, "bcmPolicerModeSrTcmModified"},
    {bcmPolicerModeCoupledTrTcmDs, "bcmPolicerModeCoupledTrTcmDs"},
    {bcmPolicerModeCascade, "bcmPolicerModeCascade"},
    {bcmPolicerModeCoupledCascade, "bcmPolicerModeCoupledCascade"},
    {bcmPolicerModeSrTcmTsn, "bcmPolicerModeSrTcmTsn"},
    {bcmPolicerModeCascade, "bcmPolicerModeCascade"}
};

/*************
 * DEFINES   *
 */
#define DIAG_DNX_METER_NO_OPTION     -1
#define DIAG_DNX_METER_STR_LENGTH (20)

#define DIAG_DNX_METER_GET_BIG_ENGINE_ID(unit) dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit)

static shr_error_e
sh_dnx_meter_mem_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int engine_id, used_for_meter, valid_engine;
    int core_id, core_idx;
    char engine_section[DIAG_DNX_METER_STR_LENGTH];
    bcm_policer_engine_t engine;
    int is_ingress, database_id, bank_idx;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int section, engine_found;
    uint32 is_single_bucket;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        PRT_TITLE_SET(" Meter Shared Memory Usage");
        PRT_COLUMN_ADD("Core");
        PRT_COLUMN_ADD("Engine");
        PRT_COLUMN_ADD("Is METER");
        PRT_COLUMN_ADD("Is COUNTER");

        for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
        {
            engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
            if (engine_info->meter_shared == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.get(unit, core_idx, engine_id, &used_for_meter));
                /** if not used for meter - check if CRPS uses it */
                if (used_for_meter == FALSE)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    PRT_CELL_SET("%d", core_idx);
                    PRT_CELL_SET("%d", engine_id);
                    PRT_CELL_SET("%s", "NO");
                    DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid_engine);
                    PRT_CELL_SET("%s", (valid_engine) ? "YES" : "NO");
                }
                /** if engine is used for meter - check if ingress or egress */
                else
                {
                    /** check if it is special engine */
                    if (DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine_id))
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%d", core_idx);
                        PRT_CELL_SET("%d", engine_id);
                        PRT_CELL_SET("%s",
                                     (engine_id ==
                                      dnx_data_meter.
                                      mem_mgmt.ingress_single_bucket_engine_get(unit)) ? "INGRESS" : "EGRESS");
                        PRT_CELL_SET("%s", "NO");
                    }
                    /** for big engines - go over the databases and search the
                     *  bank id that represents this engine */
                    else
                    {
                        for (section = bcmPolicerEngineSectionLow; section <= bcmPolicerEngineSectionHigh; section++)
                        {
                            engine.core_id = core_idx;
                            engine.engine_id = engine_id;
                            engine.section = section;
                            /** find the bank that the engine belong to */
                            SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_find
                                            (unit, core_idx, &engine, &is_ingress, &database_id, &bank_idx,
                                             &engine_found));
                            if (engine_found == 1)
                            {
                                SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                                                is_single_bucket.get(unit, core_idx, is_ingress, database_id,
                                                                     &is_single_bucket));
                                /** if double bucket mode - both banks are used - 1 print */
                                if (is_single_bucket == FALSE)
                                {
                                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                    PRT_CELL_SET("%d", core_idx);
                                    PRT_CELL_SET("%d", engine_id);
                                    PRT_CELL_SET("%s", (is_ingress == TRUE) ? "INGRESS" : "EGRESS");
                                    PRT_CELL_SET("%s", "NO");
                                    break;
                                }
                                /** if single bucket */
                                else
                                {
                                    sal_snprintf(engine_section, DIAG_DNX_METER_STR_LENGTH, "%d", engine_id);
                                    sal_sprintf(engine_section, "%d", engine_id);
                                    sal_strcat(engine_section,
                                               (section == bcmPolicerEngineSectionLow) ? " Low" : " High");
                                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                    PRT_CELL_SET("%d", core_idx);
                                    PRT_CELL_SET("%s", engine_section);
                                    PRT_CELL_SET("%s", (is_ingress == TRUE) ? "INGRESS" : "EGRESS");
                                    PRT_CELL_SET("%s", "NO");
                                }
                            }
                        }
                    }
                }
            }
        }
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_meter_verify_database_id(
    int unit,
    int is_egress,
    int database_id,
    int *print_error)
{
    int max_ingress_database_id = dnx_data_meter.meter_db.nof_ingress_db_get(unit);
    int max_egress_database_id = dnx_data_meter.meter_db.nof_egress_db_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if ((is_egress == 0 && database_id >= max_ingress_database_id)
        || (is_egress == 1 && database_id >= max_egress_database_id))
    {
        *print_error = 1;
    }
    else
    {
        *print_error = 0;
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_meter_generic_database_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int database_id, is_egress;
    int engine_id, used_for_meter;
    int core_id, core_idx;
    bcm_policer_engine_t engine;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int section;
    uint32 is_single_bucket, is_created, is_enabled;
    char engine_final_str[DIAG_DNX_METER_STR_LENGTH];
    bcm_policer_database_attach_config_t config;
    int flags = 0;
    /*
     * int policer_database_handle;
     */
    int first_database, last_database, print_error = 0;
    int max_ingress_database_id = dnx_data_meter.meter_db.nof_ingress_db_get(unit);
    int max_egress_database_id = dnx_data_meter.meter_db.nof_egress_db_get(unit);
    int find_is_ingress, find_database_id, find_bank_idx, section_found;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_INT32("id", database_id);
    SH_SAND_GET_BOOL("egress", is_egress);

    if (database_id == -1)
    {
        first_database = 0;
        last_database = (is_egress == 0) ? (max_ingress_database_id - 1) : (max_egress_database_id - 1);
    }
    else
    {
        /**verify database id   */
        SHR_IF_ERR_EXIT(sh_dnx_meter_verify_database_id(unit, is_egress, database_id, &print_error));
        if (print_error)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d incorrect database id for meter, max possible database value = %d \n",
                         unit, (is_egress ? max_egress_database_id - 1 : max_ingress_database_id - 1));
        }
        first_database = database_id;
        last_database = database_id;
    }

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        for (database_id = first_database; database_id <= last_database; database_id++)
        {
            PRT_TITLE_SET(" Meter Database Info - database ID = %d", database_id);
            PRT_COLUMN_ADD("Property");
            PRT_COLUMN_ADD("Value");

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Core");
            PRT_CELL_SET("%d", core_idx);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Database ID");
            PRT_CELL_SET("%d", database_id);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Ingress/Egress");
            PRT_CELL_SET("%s", (is_egress == 1) ? "Egress" : "Ingress");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Created");
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                            created.get(unit, core_idx, !(is_egress), database_id, &is_created));
            PRT_CELL_SET("%s", (is_created == 1) ? "Yes" : "No");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Enabled");
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                            enable.get(unit, core_idx, !(is_egress), database_id, &is_enabled));
            PRT_CELL_SET("%s", (is_enabled == 1) ? "Yes" : "No");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Bucket Mode");
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                            is_single_bucket.get(unit, core_idx, !(is_egress), database_id, &is_single_bucket));
            PRT_CELL_SET("%s", (is_single_bucket == 1) ? "Single Bucket" : "Dual Bucket");

            /** set policer_database_handle and call database_get API to
             *  compare if certain engine belongs to the given database */
            /** Iterate over the engines and check if they belong to the
             *  given database id */
            for (engine_id = 0; engine_id < dnx_data_crps.engine.nof_engines_get(unit); engine_id++)
            {
                engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);

                if (!engine_info->meter_shared)
                    continue;

                SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.get(unit, core_idx, engine_id, &used_for_meter));
                if (!used_for_meter)
                    continue;

                SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                                is_single_bucket.get(unit, core_idx, !(is_egress), database_id, &is_single_bucket));
                engine.core_id = core_idx;
                engine.engine_id = engine_id;

                /** check if it is special engine */
                if (DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine_id))
                {
                    if (database_id == 0)
                    {
                        engine.section = bcmPolicerEngineSectionLow;
                        SHR_IF_ERR_EXIT(bcm_policer_engine_database_get(unit, flags, &engine, &config));
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%s", "Engine, Section, Base");
                        sal_memset(engine_final_str, '\0', sizeof(engine_final_str));
                        sal_snprintf(engine_final_str, DIAG_DNX_METER_STR_LENGTH, "%d %s", engine_id, " N/A  0");
                        PRT_CELL_SET("%s", engine_final_str);
                    }
                    continue;
                }
                for (section = bcmPolicerEngineSectionLow; section <= bcmPolicerEngineSectionHigh; section++)
                {

                    engine.section = (is_single_bucket == TRUE) ? section : bcmPolicerEngineSectionAll;

                    /** find if the given engine section belongs to some bank id and
                     *  database  */
                    SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_find
                                    (unit, core_idx, &engine, &find_is_ingress, &find_database_id,
                                     &find_bank_idx, &section_found));

                    if (section_found == FALSE)
                        continue;

                    /** check if engine belongs to the database */
                    if (database_id != find_database_id)
                        continue;

                    SHR_IF_ERR_EXIT(bcm_policer_engine_database_get(unit, flags, &engine, &config));
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    PRT_CELL_SET("%s", "Engine, Section, Base");
                    sal_memset(engine_final_str, '\0', sizeof(engine_final_str));

                    /** for big engines - check the section*/
                    if (is_single_bucket == FALSE)
                    {
                        sal_snprintf(engine_final_str, DIAG_DNX_METER_STR_LENGTH,
                                     "%d %s %d", engine_id, " ALL ", config.object_stat_pointer_base);
                        PRT_CELL_SET("%s", engine_final_str);
                        break;
                    }
                    else
                    {
                        sal_snprintf(engine_final_str, DIAG_DNX_METER_STR_LENGTH, "%d %s %d", engine_id,
                                     (section == bcmPolicerEngineSectionLow) ? " Low " : " High",
                                     config.object_stat_pointer_base);
                        PRT_CELL_SET("%s", engine_final_str);
                    }
                }
            }
            PRT_COMMITX;
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_meter_global_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core_idx;
    int group_idx, meter_idx;
    uint32 flags = 0;
    int valid_flag = 0;
    bcm_policer_expansion_group_t expansion_group;
    char type[60];
    int is_enabled, policer_id;
    int policer_database_handle, is_ingress = 1, is_global = 1, database_id = 0;
    bcm_policer_config_t pol_cfg;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        _SHR_POLICER_DATABASE_HANDLE_SET(policer_database_handle, is_ingress, is_global, core_idx, database_id);
        SHR_IF_ERR_EXIT(bcm_policer_database_enable_get(unit, flags, policer_database_handle, &is_enabled));
        if (is_enabled == FALSE)
        {
            LOG_CLI((BSL_META("unit %d Global Meter is not enabled \n"), unit));
        }
        else
        {
            PRT_TITLE_SET(" Global Meter Info");
            PRT_COLUMN_ADD("Core");
            PRT_COLUMN_ADD("Meter index");
            PRT_COLUMN_ADD("Types");
            PRT_COLUMN_ADD("Rate[kbps]");
            PRT_COLUMN_ADD("Burst[bytes]");
            PRT_COLUMN_ADD("Packet Mode");

            SHR_IF_ERR_EXIT(bcm_policer_expansion_groups_get(unit, flags, core_idx, &expansion_group));
            for (meter_idx = bcmPolicerFwdTypeUc; meter_idx < bcmPolicerFwdTypeMax; meter_idx++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", core_idx);
                PRT_CELL_SET("%d", meter_idx);
                BCM_POLICER_ID_SET(policer_id, policer_database_handle, meter_idx);
                bcm_policer_config_t_init(&pol_cfg);
                SHR_IF_ERR_EXIT(bcm_policer_get(unit, policer_id, &pol_cfg));
                valid_flag = 0;
                /** check types offset */
                sal_memset(type, '\0', sizeof(type));
                for (group_idx = bcmPolicerFwdTypeUc; group_idx < bcmPolicerFwdTypeMax; group_idx++)
                {
                    if (expansion_group.config[group_idx].offset == meter_idx
                        && expansion_group.config[group_idx].valid == TRUE)
                    {
                        sal_strcat(type, diag_dnx_meter_global_type_enum_to_char_binding_table[group_idx].type_str);
                        sal_strcat(type, "  ");
                        valid_flag = 1;
                    }
                }
                if (valid_flag == 0)
                {
                    PRT_CELL_SET("%s", "--");
                    PRT_CELL_SET("%s", "--");
                    PRT_CELL_SET("%s", "--");
                    PRT_CELL_SET("%s", "--");
                }
                else
                {
                    PRT_CELL_SET("%s", type);
                    PRT_CELL_SET("%d", pol_cfg.ckbits_sec);
                    PRT_CELL_SET("%d", DNX_POLICER_KBITS_TO_BYTES(pol_cfg.ckbits_burst));
                    PRT_CELL_SET("%s", (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) ? "Yes" : "No");
                }

            }
            PRT_COMMITX;
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** Rate Statistic Settings */
static shr_error_e
sh_dnx_meter_rate_counters_settings(
    int unit,
    int core_id,
    int is_egress,
    int is_global,
    int database_id,
    int meter_id)
{
    uint32 entry_handle_id;
    int ingress_global_engine;
    int ptr_start;
    uint32 data;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ingress_global_engine = dnx_data_meter.profile.global_engine_hw_stat_database_id_get(unit);
    if (is_egress == TRUE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_STAT, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_STAT, &entry_handle_id));
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** extract from ptr_map table the bank id it is being used */
    SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_data_get(unit, core_id, (is_egress ? FALSE : TRUE), database_id,
                                                          (meter_id /
                                                           dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                          &data));
    ptr_start =
        ((data *
          dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get(unit)) | (meter_id & (dnx_data_meter.
                                                                                     mem_mgmt.ptr_map_table_resolution_get
                                                                                     (unit) - 1)));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_POINTER_START, INST_SINGLE, ptr_start);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_POINTER_END, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_PACKETS, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, INST_SINGLE,
                                 (is_global == TRUE) ? ingress_global_engine : database_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_meter_rate_counters_get(
    int unit,
    int core_id,
    int is_egress,
    uint64 *cir_token_counter,
    uint64 *eir_token_counter)
{
    dbal_table_field_info_t field_info;
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_egress == TRUE)
    {
        table_id = DBAL_TABLE_METER_EGR_STAT;
    }
    else
    {
        table_id = DBAL_TABLE_METER_ING_STAT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_CIR_TOKEN_COUNTER, INST_SINGLE, cir_token_counter);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_EIR_TOKEN_COUNTER, INST_SINGLE, eir_token_counter);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, DBAL_FIELD_CIR_TOKEN_COUNTER, FALSE, 0, 0, &field_info));
    COMPILER_64_BITCLR(*cir_token_counter, (field_info.field_nof_bits - 1));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, DBAL_FIELD_EIR_TOKEN_COUNTER, FALSE, 0, 0, &field_info));
    COMPILER_64_BITCLR(*eir_token_counter, (field_info.field_nof_bits - 1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_meter_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core_idx, rate;
    uint32 flags = 0;
    int policer_id, policer_database_handle;
    int is_global, is_egress, database_id, meter_id, interval;
    uint64 interval_64;
    uint64 tmp_64;
    bcm_policer_config_t pol_cfg;
    int is_enabled, print_error = 0;
    uint64 cir_token_counter, eir_token_counter;
    uint32 cir_rate, eir_rate;
    uint32 nof_clock_cycles;
    uint64 field_max_value = COMPILER_64_INIT(0, 0);
    uint64 gtimer_max_period_sec = COMPILER_64_INIT(0, 0);
    uint32 max_size;
    dnxcmn_time_t time_given_milisec, gtimer_max_sec, gtimer_max_milisec;
    int max_ingress_database_id = dnx_data_meter.meter_db.nof_ingress_db_get(unit);
    int max_egress_database_id = dnx_data_meter.meter_db.nof_egress_db_get(unit);
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_INT32("database", database_id);
    SH_SAND_GET_INT32("meter_id", meter_id);
    SH_SAND_GET_INT32("interval", interval);
    SH_SAND_GET_BOOL("egress", is_egress);
    SH_SAND_GET_BOOL("global", is_global);
    SH_SAND_GET_BOOL("rate", rate);

     /**verify database id   */
    SHR_IF_ERR_EXIT(sh_dnx_meter_verify_database_id(unit, is_egress, database_id, &print_error));
    if (print_error)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d incorrect database id for meter, max possible database value = %d \n", unit,
                     (is_egress ? max_egress_database_id - 1 : max_ingress_database_id - 1));
    }

    /** check the max possible gtimer period in seconds*/
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_GTIMER_CYCLE, &max_size));
    COMPILER_64_SET(field_max_value, 0, 1);
    COMPILER_64_SHL(field_max_value, max_size);
    COMPILER_64_SUB_32(field_max_value, (uint32) 1);
    /*
     * Effectively, the lines above are:
     * field_max_value = utilex_power_of_2(max_size) - 1;
     */
    COMPILER_64_ADD_32(field_max_value, (DNXCMN_CORE_CLOCK_HZ_GET(unit) / 2));
    COMPILER_64_UDIV_32(field_max_value, DNXCMN_CORE_CLOCK_HZ_GET(unit));
    /*
     * Effectively, the lines above are:
     * field_max_value = UTILEX_DIV_ROUND(field_max_value, DNXCMN_CORE_CLOCK_HZ_GET(unit))
     */
    COMPILER_64_COPY(gtimer_max_period_sec, field_max_value);
    gtimer_max_sec.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_COPY(gtimer_max_sec.time, gtimer_max_period_sec);
    SHR_IF_ERR_EXIT(dnxcmn_time_units_convert(unit, &gtimer_max_sec, DNXCMN_TIME_UNIT_MILISEC, &gtimer_max_milisec));
    COMPILER_64_SET(interval_64, 0, interval);
    COMPILER_64_SET(tmp_64, 0, 1000);
    if (COMPILER_64_GT(interval_64, gtimer_max_milisec.time) || COMPILER_64_LT(interval_64, tmp_64))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d interval given must be between 1000ms and 0x%08x%08x ms \n", unit,
                     COMPILER_64_HI(gtimer_max_milisec.time), COMPILER_64_LO(gtimer_max_milisec.time));
    }

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        /**1. print table for the configuration */
        _SHR_POLICER_DATABASE_HANDLE_SET(policer_database_handle, !(is_egress), is_global, core_idx, database_id);
        SHR_IF_ERR_EXIT(bcm_policer_database_enable_get(unit, flags, policer_database_handle, &is_enabled));
        if (is_enabled == FALSE)
        {
            LOG_CLI((BSL_META("unit %d core=%d Meter is not enabled \n"), unit, core_idx));
        }
        else
        {
            BCM_POLICER_ID_SET(policer_id, policer_database_handle, meter_id);
            bcm_policer_config_t_init(&pol_cfg);
            SHR_IF_ERR_EXIT(bcm_policer_get(unit, policer_id, &pol_cfg));

            PRT_TITLE_SET("Meter Configuration Info");
            PRT_COLUMN_ADD("Configuration");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Core");
            PRT_CELL_SET("%d", core_idx);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Meter index");
            PRT_CELL_SET("%d", meter_id);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Ingress / Egress");
            PRT_CELL_SET("%s", (is_egress == TRUE) ? "Egress" : "Ingress");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Policer Mode");
            PRT_CELL_SET("%s", diag_dnx_meter_policer_mode_enum_to_char_binding_table[pol_cfg.mode].mode_str);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Committed Input Rate[kbps]");
            PRT_CELL_SET("0x%08x", pol_cfg.ckbits_sec);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Excess Input Rate[kbps]");
            PRT_CELL_SET("0x%08x", pol_cfg.pkbits_sec);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Committed Max Rate[kbps]");
            PRT_CELL_SET("0x%08x", pol_cfg.max_ckbits_sec);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Excess Max Rate[kbps]");
            PRT_CELL_SET("0x%08x", pol_cfg.max_pkbits_sec);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Committed Burst Size[kbits]");
            PRT_CELL_SET("%d", pol_cfg.ckbits_burst);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Excess Burst Size[kbits]");
            PRT_CELL_SET("%d", pol_cfg.pkbits_burst);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Packet Mode");
            PRT_CELL_SET("%s", (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) ? "Yes" : "No");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Color Blind");
            PRT_CELL_SET("%s", (pol_cfg.flags & BCM_POLICER_COLOR_BLIND) ? "Yes" : "No");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Header Truncate Adjustment");
            PRT_CELL_SET("%s", (pol_cfg.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE) ? "Yes" : "No");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Color Resolve Profile");
            PRT_CELL_SET("%d", pol_cfg.color_resolve_profile);
            PRT_COMMITX;
            /** check global config if it is 0 */

            if (rate == TRUE)
            {
                if (!dnx_data_meter.diag.feature_get(unit, dnx_data_meter_diag_rate_measurements_support))
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_NONE,
                                      "This Diagnostic command is not supported on the current Device %s%s%s\n", EMPTY,
                                      EMPTY, EMPTY);
                }
                else
                {
#ifdef PLISIM
                    if (!SAL_BOOT_PLISIM)       /* not pcid */
#endif
                    {
                        if (is_global == TRUE)
                        {
                            SHR_EXIT_WITH_LOG(_SHR_E_NONE,
                                              "Rate command is not supported for is_global=TRUE %s%s%s\n", EMPTY,
                                              EMPTY, EMPTY);
                        }
                        /**2. configure MRPS to count the specific meter index */
                        SHR_IF_ERR_EXIT(sh_dnx_meter_rate_counters_settings
                                        (unit, core_idx, is_egress, is_global, database_id, meter_id));
                        /**3. configure G-TIMER for MRPS block */
                        time_given_milisec.time_units = DNXCMN_TIME_UNIT_MILISEC;
                        COMPILER_64_SET(time_given_milisec.time, 0, interval);
                        SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time_given_milisec, &nof_clock_cycles));
                        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_MRPS, core_idx));
                        /**4. Read the counters*/
                        /** wait gtimer trigger to indicate it finished counting */
                        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_MRPS, core_idx));
                        SHR_IF_ERR_EXIT(sh_dnx_meter_rate_counters_get
                                        (unit, core_idx, is_egress, &cir_token_counter, &eir_token_counter));
                        /**5. calculate rate in kbps - if 1 second - rate equals to
                         * the bytes transmitted */
                        /**    bits/miliseconds  = kbits/s   */
                        COMPILER_64_UDIV_32(cir_token_counter, (DNX_METER_UTILS_KBITS_TO_BYTES * (interval / 1000)));
                        cir_rate = COMPILER_64_LO(cir_token_counter);

                        if (COMPILER_64_HI(cir_token_counter))
                        {
                            SHR_EXIT_WITH_LOG(_SHR_E_NONE,
                                              "cir_rate calculation is wrong. rate [kbps] exceed max possible value %s%s%s\n",
                                              EMPTY, EMPTY, EMPTY);
                        }
                        COMPILER_64_UDIV_32(eir_token_counter, (DNX_METER_UTILS_KBITS_TO_BYTES * (interval / 1000)));
                        eir_rate = COMPILER_64_LO(eir_token_counter);

                        if (COMPILER_64_HI(eir_token_counter))
                        {
                            SHR_EXIT_WITH_LOG(_SHR_E_NONE,
                                              "eir_rate calculation is wrong. rate [kbps] exceed max possible value %s%s%s\n",
                                              EMPTY, EMPTY, EMPTY);
                        }
                        /**6. print table */
                        PRT_TITLE_SET("Rate for interval %d ms", interval);
                        PRT_COLUMN_ADD("Rate");
                        PRT_COLUMN_ADD("Calculated value[kbps]");
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%s", "Committed Rate");
                        PRT_CELL_SET("%d", cir_rate);
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%s", "Excess Rate");
                        PRT_CELL_SET("%d", eir_rate);
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_COMMITX;
                        /**7. Disable G-TIMER */
                        SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_MRPS, core_idx));
                    }
                }
            }
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Init callback function for "meter global" diag commands
 *    Create global policer database
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_meter_global_init_cb(
    int unit)
{
    int flags = 0, core = BCM_CORE_ALL;
    bcm_policer_expansion_group_t expansion_group;
    bcm_policer_config_t pol_cfg;
    int policer_id, database_handle;
    SHR_FUNC_INIT_VARS(unit);

    bcm_policer_expansion_group_t_init(&expansion_group);
    /** step 1: configure expansion groups based on l2 forwarding types */
    /** 3 groups: UC, MC+UMC, BC */
    expansion_group.config[bcmPolicerFwdTypeUc].offset = 0;
    expansion_group.config[bcmPolicerFwdTypeUc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeMc].offset = 1;
    expansion_group.config[bcmPolicerFwdTypeMc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].offset = 1;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeBc].offset = 2;
    expansion_group.config[bcmPolicerFwdTypeBc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeUnkownUc].valid = FALSE;
    SHR_IF_ERR_EXIT(bcm_policer_expansion_groups_set(unit, flags, core, &expansion_group));

    /** step 2: for each group need to configure its global meter. the offset indicates on the meter index */

    /** decode into database_handle, the database parameter that are revevant - core and is_global = TRUE*/
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, TRUE, TRUE, core, 2);

    /** set the meter configuration for offset 0 */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeCommitted; /** global meter is always in committed mode (single bucket, one rate: drop or not) */
    pol_cfg.ckbits_sec = 30000; /** 30MB */
    pol_cfg.ckbits_burst = 1000;

    /** set the policer id, using the macro for offset 0*/
    BCM_POLICER_ID_SET(policer_id, database_handle, 0);
    SHR_IF_ERR_EXIT(bcm_policer_set(unit, policer_id, &pol_cfg));
    pol_cfg.ckbits_sec = 30; /** 30Kbps - min rate for 1Ghz clock rate */
    pol_cfg.ckbits_burst = (32000 - 1); /** max bucket size (4MB) without using large bucket mode. */

    BCM_POLICER_ID_SET(policer_id, database_handle, 1);
    SHR_IF_ERR_EXIT(bcm_policer_set(unit, policer_id, &pol_cfg));

    pol_cfg.ckbits_sec = 1000000; /** 1Gbps */
    pol_cfg.ckbits_burst = (2048000 - 1); /** max bucket size (256MB) when using large bucket mode.
                                                                                   this will influence badly on the output rate accuracy in the HW */
    BCM_POLICER_ID_SET(policer_id, database_handle, 2);
    SHR_IF_ERR_EXIT(bcm_policer_set(unit, policer_id, &pol_cfg));

    /** step 3: enable the global policer */
    SHR_IF_ERR_EXIT(bcm_policer_database_enable_set(unit, flags, database_handle, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Init callback function for "meter info" diag commands
 *    Create egress generic policer database
 *    Create global policers using sh_dnx_meter_global_init_cb
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_meter_info_init_cb(
    int unit)
{
    int flags = 0, core = BCM_CORE_ALL, database_id = 1, engine_id;
    bcm_policer_database_config_t database_config;
    int database_handle, is_ingress = 0;
    bcm_policer_engine_t engine;
    bcm_policer_database_attach_config_t attach_config;
    bcm_policer_config_t pol_cfg;
    int meter_idx, policer_id, nof_meters;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_policer_config_t_init(&pol_cfg);
    bcm_policer_engine_t_init(&engine);
    bcm_policer_database_attach_config_t_init(&attach_config);
    bcm_policer_database_config_t_init(&database_config);

    database_config.expansion_enable = 0;
    database_config.is_single_bucket = 0;

    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, 0, core, database_id);
    SHR_IF_ERR_EXIT(bcm_policer_database_create(unit, flags, database_handle, &database_config));
    attach_config.policer_database_handle = database_handle;
    engine_id = DIAG_DNX_METER_GET_BIG_ENGINE_ID(unit);
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
    nof_meters = engine_info->nof_counters;
    engine.engine_id = engine_id;
    engine.section = bcmPolicerEngineSectionAll;
    engine.core_id = core;
    attach_config.object_stat_pointer_base = 0;
    SHR_IF_ERR_EXIT(bcm_policer_engine_database_attach(unit, flags, &engine, &attach_config));

    pol_cfg.mode = bcmPolicerModeCommitted;
    pol_cfg.ckbits_sec = 80000;
    pol_cfg.ckbits_burst = 1000;

    for (meter_idx = 0; meter_idx < nof_meters; meter_idx++)
    {
        BCM_POLICER_ID_SET(policer_id, database_handle, meter_idx);
        SHR_IF_ERR_EXIT(bcm_policer_create(unit, &pol_cfg, &policer_id));
    }
    SHR_IF_ERR_EXIT(bcm_policer_database_enable_set(unit, flags, database_handle, TRUE));

    /** create global database */
    SHR_IF_ERR_EXIT(sh_dnx_meter_global_init_cb(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Deinit callback function for "meter info" diag commands
 *    Destrou policer database
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_meter_info_deinit_cb(
    int unit)
{
    int database_handle, flags = 0, core = BCM_CORE_ALL, is_ingress = 0, database_id = 1;
    bcm_policer_engine_t engine;
    SHR_FUNC_INIT_VARS(unit);

    bcm_policer_engine_t_init(&engine);
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, 0, core, database_id);

    SHR_IF_ERR_EXIT(bcm_policer_database_enable_set(unit, flags, database_handle, FALSE));
    engine.core_id = core;
    engine.engine_id = DIAG_DNX_METER_GET_BIG_ENGINE_ID(unit);;
    engine.section = bcmPolicerEngineSectionAll;
    SHR_IF_ERR_EXIT(bcm_policer_engine_database_detach(unit, flags, &engine));
    SHR_IF_ERR_EXIT(bcm_policer_database_destroy(unit, flags, database_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Init callback function for "meter database" diag command
 *    Create egress generic policer database with big engine
 *    with High Section
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_meter_generic_database_info_init_cb(
    int unit)
{
    int flags = 0, core = BCM_CORE_ALL, database_id = 0, engine_id;
    bcm_policer_database_config_t database_config;
    int database_handle, is_ingress = 1;
    bcm_policer_engine_t engine;
    bcm_policer_database_attach_config_t attach_config;
    bcm_policer_config_t pol_cfg;
    int policer_id;
    SHR_FUNC_INIT_VARS(unit);

    bcm_policer_config_t_init(&pol_cfg);
    bcm_policer_engine_t_init(&engine);
    bcm_policer_database_attach_config_t_init(&attach_config);
    bcm_policer_database_config_t_init(&database_config);

    database_config.expansion_enable = 0;
    database_config.is_single_bucket = 1;

    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, 0, core, database_id);
    SHR_IF_ERR_EXIT(bcm_policer_database_create(unit, flags, database_handle, &database_config));
    SHR_IF_ERR_EXIT(bcm_policer_database_enable_set(unit, flags, database_handle, TRUE));
    attach_config.policer_database_handle = database_handle;
    engine_id = DIAG_DNX_METER_GET_BIG_ENGINE_ID(unit);
    engine.engine_id = engine_id;
    engine.section = bcmPolicerEngineSectionHigh;
    engine.core_id = core;
    attach_config.object_stat_pointer_base = 0;
    SHR_IF_ERR_EXIT(bcm_policer_engine_database_attach(unit, flags, &engine, &attach_config));

    pol_cfg.mode = bcmPolicerModeCommitted;
    pol_cfg.ckbits_sec = 80000;
    pol_cfg.ckbits_burst = 1000;
    pol_cfg.flags = BCM_POLICER_COLOR_BLIND;
    BCM_POLICER_ID_SET(policer_id, database_handle, 0);
    SHR_IF_ERR_EXIT(bcm_policer_create(unit, &pol_cfg, &policer_id));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sh_dnx_meter_mem_map_man = {
    .brief = "print meter engines usage - if they are used for MRPS/CRPS.",
    .full =
        "Print table with engines configuration \n"
        "if it is used from meter or counter if meter - print ingress or egress\n",
    .synopsis = NULL,
    .examples = "core=0"
};

sh_sand_cmd_t sh_dnx_meter_memory_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"MAPping", sh_dnx_meter_mem_map_cmd, NULL, NULL, &sh_dnx_meter_mem_map_man},
    {NULL}
};

static sh_sand_man_t sh_dnx_meter_memory_man = {
    .brief = "memory - show engine configuration - used for MRPS or CRPS \n",
    .full = "with option mapping - print tables per core for the engine usage \n",
};

static sh_sand_option_t dnx_meter_generic_database_options[] = {
    {"id", SAL_FIELD_TYPE_INT32, "id", "-1"},
    {"egress", SAL_FIELD_TYPE_BOOL, "is_egress", "0"},
    {NULL}
};

static sh_sand_man_t sh_dnx_meter_generic_database_man = {
    .brief = "Show basic information for the given meter database (present all databases if id not set) \n",
    .full = NULL,
    .synopsis = NULL,
    .examples = "id=1 egress=1 core=0\n" "egress=0\n" "id=0",
    .init_cb = sh_dnx_meter_generic_database_info_init_cb
};

static sh_sand_man_t sh_dnx_meter_global_man = {
    .brief = "Show basic information for the global meter \n",
    .full = NULL,
    .synopsis = NULL,
    .examples = "core=0",
    .init_cb = sh_dnx_meter_global_init_cb
};

static sh_sand_option_t dnx_meter_info_options[] = {
    {"global", SAL_FIELD_TYPE_BOOL, "is_global", "0"},
    {"database", SAL_FIELD_TYPE_INT32, "database", NULL},
    {"meter_id", SAL_FIELD_TYPE_INT32, "meter_id", NULL},
    {"egress", SAL_FIELD_TYPE_BOOL, "is_egress", "0"},
    {"InTerVal", SAL_FIELD_TYPE_INT32, "interval", "1000"},
    {"RATE", SAL_FIELD_TYPE_BOOL, "rate", "No"},
    {NULL}
};

static sh_sand_man_t sh_dnx_meter_info_man = {
    .brief =
        "info - show the basic configuration for the meter given and if rate=yes the actual rate for given interval in ms \n",
    .full =
        "show the basic information for the meter given - CIR, EIR, Burst,packet mode, color blind \n"
        "if user provides rate after the configuration table will be printed actual rate information \n"
        "user can provide interval in milliseconds for which he wants to calculate the rate",
    .synopsis = NULL,
    .examples = "global=0 database=1 meter_id=1 egress=1 core=0\n"
        "global=0 database=1 meter_id=1 egress=1 core=0 rate interval=2000\n" "global=1 database=2 meter_id=1 core=0",
    .init_cb = sh_dnx_meter_info_init_cb,
    .deinit_cb = sh_dnx_meter_info_deinit_cb
};

/**
 * \brief DNX Meter diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for CRPS diagnostic commands
 */
sh_sand_cmd_t sh_dnx_meter_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"MEMory", NULL, sh_dnx_meter_memory_cmds, NULL, &sh_dnx_meter_memory_man},
    {"DaTaBaSe", sh_dnx_meter_generic_database_cmd, NULL, dnx_meter_generic_database_options,
     &sh_dnx_meter_generic_database_man},
    {"GLOBal", sh_dnx_meter_global_cmd, NULL, NULL, &sh_dnx_meter_global_man},
    {"info", sh_dnx_meter_info_cmd, NULL, dnx_meter_info_options, &sh_dnx_meter_info_man},
    {NULL}
};

sh_sand_man_t sh_dnx_meter_man = {
    .brief = "Meter diagnostics and commands"
};
