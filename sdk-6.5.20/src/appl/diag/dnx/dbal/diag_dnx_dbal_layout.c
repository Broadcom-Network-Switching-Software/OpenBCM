/** \file diag_dnx_dbal_layout.c
 *
 * Main DBAL layout functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for access*/
#include <appl/diag/diag.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include "diag_dnx_dbal_internal.h"
#include "diag_dnx_dbal.h"
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>

void
diag_dbal_offset_encode_info_to_str(
    int unit,
    CONST dbal_offset_encode_info_t * encode_info,
    char *str)
{
    sal_snprintf(str, CMD_MAX_STRING_LENGTH, "%s", "");

    if (encode_info->formula != NULL)
    {
        /** cFormula filled in the xml by the user are converted to
         *  cb functions by autocoder.
         *  Therefore, formula_offset_cb indicates that we have a
         *  cFormula.  */
        if (encode_info->formula->cb != NULL)
        {
            sal_snprintf(str, CMD_MAX_STRING_LENGTH, "%s", "cFormula");
        }
        else
        {
            sal_snprintf(str, CMD_MAX_STRING_LENGTH, "%d", encode_info->formula->val);
        }
    }
}

STATIC void
diag_dbal_conditions_string_build(
    int unit,
    int nof_conditions,
    CONST dbal_access_condition_info_t * condition,
    char *str)
{
    int ii, is_printed = 0, offset;

    sal_snprintf(str, CMD_MAX_STRING_LENGTH, "%s", "NONE");

    /** support params print for conditions */
    for (ii = 0; ii < nof_conditions; ii++)
    {
        if (condition[ii].type != DBAL_CONDITION_NONE)
        {
            if (is_printed == 1)
            {
                offset = sal_snprintf(str, CMD_MAX_STRING_LENGTH, ", %s %d",
                                      dbal_condition_to_string(unit, condition[ii].type), condition[ii].value[0]);
            }
            else
            {
                offset = sal_snprintf(str, CMD_MAX_STRING_LENGTH, "%s %d",
                                      dbal_condition_to_string(unit, condition[ii].type), condition[ii].value[0]);
                is_printed = 1;
            }
            str += offset;
        }
    }
}

static void
dbal_field_types_override_info_dump(
    int unit,
    CONST dbal_field_types_basic_info_t * field_type_info)
{
    LOG_CLI((BSL_META("\n")));

    LOG_CLI((BSL_META("Override Infos: ")));
    if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DYNAMIC))
    {
        LOG_CLI((BSL_META("Field type was overridden dynamically ")));
    }
    else
    {
        /** common field type   */
        if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_COMMON))
        {
            LOG_CLI((BSL_META("Field type is common. It can't be overridden dynamically")));
        }
        /** default field type   */
        else
        {
            LOG_CLI((BSL_META("Field type is default. It can be overridden dynamically")));
        }

        LOG_CLI((BSL_META("\n")));
        if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_STANDARD_1))
        {
            LOG_CLI((BSL_META("Field type is defined in standard_1 image")));
        }

    }
    LOG_CLI((BSL_META("\n")));

}

static shr_error_e
dbal_fields_type_related_labels_dump(
    int unit,
    dbal_field_types_defs_e field_type)
{
    int ii, is_printed = 0;
    uint8 is_label_related = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\n")));

    LOG_CLI((BSL_META("Related Labels: ")));

    for (ii = DBAL_LABEL_NONE + 1; ii < DBAL_NOF_LABEL_TYPES; ii++)
    {
        is_label_related = FALSE;
        SHR_IF_ERR_EXIT(dbal_field_types_label_is_related_get(unit, field_type, ii, &is_label_related));

        if (is_label_related)
        {
            if (!is_printed)
            {
                LOG_CLI((BSL_META("%s"), dbal_label_to_string(unit, ii)));
                is_printed = 1;
            }
            else
            {
                LOG_CLI((BSL_META(", %s"), dbal_label_to_string(unit, ii)));
            }
        }
    }

    if (!is_printed)
    {
        LOG_CLI((BSL_META("NONE")));
    }

    LOG_CLI((BSL_META("\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_fields_type_related_tables_dump(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_labels_e label)
{
    int ii, jj, kk, is_printed = 0;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\n")));

    LOG_CLI((BSL_META("Field type Usage in tables: ")));

    /** Print labels from tables that use this type */
    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        uint8 is_table_uses_type = FALSE;
        uint8 is_table_uses_label = FALSE;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));

        if (label != DBAL_LABEL_NONE)
        {
            for (jj = 0; jj < table->nof_labels; jj++)
            {
                if (table->table_labels[jj] == label)
                {
                    is_table_uses_label = TRUE;
                }
            }
        }
        else
        {
            is_table_uses_label = TRUE;
        }

        if (is_table_uses_label)
        {
            for (jj = 0; jj < table->nof_key_fields; jj++)
            {
                if (table->keys_info[jj].field_type == field_type)
                {
                    is_table_uses_type = TRUE;
                    break;
                }
            }

            if (!is_table_uses_type)
            {
                for (jj = 0; jj < table->nof_result_types; jj++)
                {
                    multi_res_info_t *res_info;
                    res_info = &table->multi_res_info[jj];
                    for (kk = 0; kk < res_info->nof_result_fields; kk++)
                    {
                        if (res_info->results_info[kk].field_type == field_type)
                        {
                            is_table_uses_type = TRUE;
                            break;
                        }
                    }
                    if (is_table_uses_type)
                    {
                        break;
                    }
                }
            }
        }

        if (is_table_uses_type && is_table_uses_label)
        {
            if (is_printed)
            {
                LOG_CLI((BSL_META(", %s"), table->table_name));
            }
            else
            {
                is_printed = 1;
                LOG_CLI((BSL_META("%s"), table->table_name));
            }
        }
    }

    if (!is_printed)
    {
        LOG_CLI((BSL_META("NONE")));
    }
    LOG_CLI((BSL_META("\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_physical_table_print_related_tables(
    int unit,
    dbal_physical_tables_e phy_table_id,
    sh_sand_control_t * sand_control)
{
    int ii, jj;
    CONST dbal_logical_table_t *table;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Physical table usage");

    PRT_COLUMN_ADD("logical table name");
    PRT_COLUMN_ADD("App ID");
    PRT_COLUMN_ADD("App ID size");
    PRT_COLUMN_ADD("Num of entries");

    /** Print labels from tables that use this type */
    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));

        for (jj = 0; jj < table->nof_physical_tables; jj++)
        {
            if (table->physical_db_id[jj] == phy_table_id)
            {
                int nof_entries;
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, ii, &nof_entries));
                switch (table->table_type)
                {
                    case DBAL_TABLE_TYPE_DIRECT:
                    case DBAL_TABLE_TYPE_TCAM_DIRECT:
                    case DBAL_TABLE_TYPE_TCAM:
                    case DBAL_TABLE_TYPE_TCAM_BY_ID:
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", table->table_name);
                        PRT_CELL_SET("%s", "N/A");
                        PRT_CELL_SET("%s", "N/A");
                        PRT_CELL_SET("%s", "N/A");
                        break;
                    case DBAL_TABLE_TYPE_EM:
                    case DBAL_TABLE_TYPE_LPM:
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", table->table_name);
                        PRT_CELL_SET("%d", table->app_id);
                        PRT_CELL_SET("%d", table->app_id_size);
                        PRT_CELL_SET("%d", nof_entries);
                        break;

                    default:
                        break;
                }
            }
        }
    }
    PRT_COMMITX;

    LOG_CLI((BSL_META("\n")));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

STATIC shr_error_e
diag_dbal_hard_logic_direct_hw_info_dump(
    int unit,
    CONST dbal_hl_access_info_t * access_info,
    sh_sand_control_t * sand_control)
{
    char *memory_name;
    char *alias_name = NULL;
    char *field_name;
    int jj, ii, to_print = 0, is_memory;
    char info_string[CMD_MAX_STRING_LENGTH];
    uint8 has_alias;
    CONST dbal_hl_l2p_info_t *l2p_hl_info = access_info->l2p_hl_info;
    /*
     * soc_reg_t last_reg; int last_offset; soc_mem_t last_mem;
     */

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_HL_ACCESS_TYPES; ii++)
    {
        if (ii == DBAL_HL_ACCESS_SW)
        {
            continue;
        }
        if (l2p_hl_info[ii].num_of_access_fields != 0)
        {
            to_print = 1;
            break;
        }
    }

    if (to_print)
    {
        PRT_TITLE_SET("HL access logical to physical info");
        PRT_INFO_ADD("Default entry: %s",
                     access_info->is_default_non_standard ? "Non-standard" : "Standard (all zero)");

        PRT_COLUMN_ADD("Field Name");
        PRT_COLUMN_ADD("bit size");
        PRT_COLUMN_ADD("offset");
        PRT_COLUMN_ADD("Mapped to Mem/Reg/Group");
        PRT_COLUMN_ADD("Alias info");
        PRT_COLUMN_ADD("HW field (offset - len)");
        PRT_COLUMN_ADD("Array offset");
        PRT_COLUMN_ADD("Block index");
        PRT_COLUMN_ADD("Entry offset");
        PRT_COLUMN_ADD("Field/data offset");
        PRT_COLUMN_ADD("group offset");
        PRT_COLUMN_ADD("Condition");
        PRT_COLUMN_ADD("Encoding");

        for (ii = 0; ii < DBAL_NOF_HL_ACCESS_TYPES; ii++)
        {
            if (ii == DBAL_HL_ACCESS_SW)
            {
                continue;
            }
            for (jj = 0; jj < l2p_hl_info[ii].num_of_access_fields; jj++)
            {
                has_alias = FALSE;
                if (ii == DBAL_HL_ACCESS_MEMORY)
                {
                    is_memory = 1;
                    memory_name = SOC_MEM_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].memory[0]);
                    if (l2p_hl_info[ii].l2p_fields_info[jj].alias_memory != INVALIDm)
                    {
                        alias_name = SOC_MEM_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].alias_memory);
                        diag_dbal_offset_encode_info_to_str(unit,
                                                            &(l2p_hl_info[ii].
                                                              l2p_fields_info[jj].alias_data_offset_info), info_string);
                        has_alias = TRUE;
                    }
                }
                else if (ii == DBAL_HL_ACCESS_REGISTER)
                {
                    is_memory = 0;
                    memory_name = SOC_REG_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].reg[0]);
                    if (l2p_hl_info[ii].l2p_fields_info[jj].alias_reg != INVALIDr)
                    {
                        alias_name = SOC_REG_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].alias_reg);
                        diag_dbal_offset_encode_info_to_str(unit,
                                                            &(l2p_hl_info[ii].
                                                              l2p_fields_info[jj].alias_data_offset_info), info_string);
                        has_alias = TRUE;
                    }
                }
                else
                {
                    LOG_ERROR_EX(BSL_LOG_MODULE, "\n access type not parsed %d %s %s %s\n", ii, EMPTY, EMPTY, EMPTY);
                    SHR_EXIT();
                }

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", dbal_field_to_string(unit, l2p_hl_info[ii].l2p_fields_info[jj].field_id));
                PRT_CELL_SET("%d", l2p_hl_info[ii].l2p_fields_info[jj].nof_bits_in_interface);
                PRT_CELL_SET("%d", l2p_hl_info[ii].l2p_fields_info[jj].offset_in_interface);
                if (l2p_hl_info[ii].l2p_fields_info[jj].hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY)
                {
                    char *group_name =
                        dbal_hw_entity_group_to_string(unit, l2p_hl_info[ii].l2p_fields_info[jj].hw_entity_group_id);
                    PRT_CELL_SET("%s%s", group_name, "g");
                }
                else
                {
                    PRT_CELL_SET("%s%s", memory_name, is_memory ? "m" : "r");
                }

                if (has_alias)
                {
                    if (sal_strcasecmp(info_string, "") == 0)
                    {
                        sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default (NONE)");
                    }
                    PRT_CELL_SET("%s, %s", alias_name, info_string);
                }
                else
                {
                    PRT_CELL_SET("--");
                }
                if ((l2p_hl_info[ii].l2p_fields_info[jj].hw_field != 0) &&
                    (l2p_hl_info[ii].l2p_fields_info[jj].hw_field != INVALIDf))
                {
                    int nof_fields;
                    soc_field_info_t *fieldinfo;
                    soc_field_info_t *fields_array;

                    /** calculating the field size */
                    if (!is_memory)
                    {
                        nof_fields = SOC_REG_INFO(unit, l2p_hl_info[ii].l2p_fields_info[jj].reg[0]).nFields;
                        fields_array = SOC_REG_INFO(unit, l2p_hl_info[ii].l2p_fields_info[jj].reg[0]).fields;
                    }
                    else
                    {
                        nof_fields = SOC_MEM_INFO(unit, l2p_hl_info[ii].l2p_fields_info[jj].memory[0]).nFields;
                        fields_array = SOC_MEM_INFO(unit, l2p_hl_info[ii].l2p_fields_info[jj].memory[0]).fields;

                    }
                    SOC_FIND_FIELD(l2p_hl_info[ii].l2p_fields_info[jj].hw_field, fields_array, nof_fields, fieldinfo);

                    field_name = SOC_FIELD_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].hw_field);
                    PRT_CELL_SET("%s, (%d, %d)", field_name, fieldinfo->bp, fieldinfo->len);/** bp is the offset in HW len is the size*/
                }
                else
                {
                    PRT_CELL_SET(" - ");
                }

                /** Array offset */
                diag_dbal_offset_encode_info_to_str(unit,
                                                    &(l2p_hl_info[ii].l2p_fields_info[jj].array_offset_info),
                                                    info_string);
                if (sal_strcasecmp(info_string, "") == 0)
                {
                    sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default (ALL)");
                }
                PRT_CELL_SET("%s", info_string);

                /** Block index */
                diag_dbal_offset_encode_info_to_str(unit,
                                                    &(l2p_hl_info[ii].l2p_fields_info[jj].block_index_info),
                                                    info_string);
                if (sal_strcasecmp(info_string, "") == 0)
                {
                    sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default");
                }
                PRT_CELL_SET("%s", info_string);

                /** Entry offset */
                diag_dbal_offset_encode_info_to_str(unit, &(l2p_hl_info[ii].l2p_fields_info[jj].entry_offset_info),
                                                    info_string);
                if (sal_strcasecmp(info_string, "") == 0)
                {
                    sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default (KEY)");
                }
                PRT_CELL_SET("%s", info_string);

                /** Data offset */
                diag_dbal_offset_encode_info_to_str(unit, &(l2p_hl_info[ii].l2p_fields_info[jj].data_offset_info),
                                                    info_string);
                if (sal_strcasecmp(info_string, "") == 0)
                {
                    sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default (0)");
                }
                if ((l2p_hl_info[ii].l2p_fields_info[jj].data_offset_info.formula) &&
                    (l2p_hl_info[ii].l2p_fields_info[jj].data_offset_info.formula->cb == NULL))
                {
                    PRT_CELL_SET("%s, %d", info_string,
                                 l2p_hl_info[ii].l2p_fields_info[jj].data_offset_info.formula->val);
                }
                else
                {
                    PRT_CELL_SET("%s", info_string);
                }

                /** Group offset */
                diag_dbal_offset_encode_info_to_str(unit, &(l2p_hl_info[ii].l2p_fields_info[jj].group_offset_info),
                                                    info_string);
                if (sal_strcasecmp(info_string, "") == 0)
                {
                    sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default (all)");
                }
                PRT_CELL_SET("%s", info_string);

                diag_dbal_conditions_string_build(unit, l2p_hl_info[ii].l2p_fields_info[jj].nof_conditions,
                                                  (l2p_hl_info[ii].l2p_fields_info[jj].mapping_condition), info_string);
                PRT_CELL_SET("%s", info_string);

                PRT_CELL_SET("%s, val=%d",
                             dbal_field_encode_type_to_string(unit,
                                                              l2p_hl_info[ii].l2p_fields_info[jj].
                                                              encode_info.encode_mode),
                             l2p_hl_info[ii].l2p_fields_info[jj].encode_info.input_param);
            }
        }

        PRT_COMMITX;
        LOG_CLI((BSL_META("\n")));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_tcam_cs_hard_coded_memory_dump(
    int unit,
    dbal_stage_e stage,
    soc_mem_t memory)
{
    dbal_tcam_cs_stage_info_t stage_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tcam_cs_stage_info_get(unit, stage, &stage_info));

    LOG_CLI((BSL_META("  Hard coded mapping info for stage %s:\n\n"), dbal_stage_to_string(unit, stage)));

    LOG_CLI((BSL_META("\tCommnad Memory %s \n"), SOC_MEM_NAME(unit, stage_info.cmd_memory)));

    LOG_CLI((BSL_META("\t\t Write Field          %s \n"), SOC_FIELD_NAME(unit, stage_info.cmd_write_field)));
    LOG_CLI((BSL_META("\t\t Read field           %s \n"), SOC_FIELD_NAME(unit, stage_info.cmd_read_field)));
    LOG_CLI((BSL_META("\t\t Compare field        %s \n"), SOC_FIELD_NAME(unit, stage_info.cmd_compare_field)));
    LOG_CLI((BSL_META("\t\t Valid _field         %s \n"), SOC_FIELD_NAME(unit, stage_info.cmd_valid_field)));
    LOG_CLI((BSL_META("\t\t Key field;           %s \n"), SOC_FIELD_NAME(unit, stage_info.cmd_key_field)));
    LOG_CLI((BSL_META("\t\t Mask field;          %s \n\n"), SOC_FIELD_NAME(unit, stage_info.cmd_mask_field)));

    LOG_CLI((BSL_META("\tReply Memory %s \n"), SOC_MEM_NAME(unit, stage_info.reply_memory)));

    LOG_CLI((BSL_META("\t\t Valid Field          %s \n"), SOC_FIELD_NAME(unit, stage_info.reply_valid_field)));
    LOG_CLI((BSL_META("\t\t data out             %s \n\n"), SOC_FIELD_NAME(unit, stage_info.reply_data_out_field)));

    if (dnx_data_ingress_cs.
        features.feature_get(unit, dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write))
    {
        LOG_CLI((BSL_META("\tECC fix enable\n")));

        LOG_CLI((BSL_META("\t\t ECC FIX_EN Register %s \n\n"), SOC_REG_NAME(unit, stage_info.ecc_fix_en_reg)));
    }

    LOG_CLI((BSL_META("\tPhysical Memory Properties\n")));

    LOG_CLI((BSL_META("\t\t Full key size        %d \n\n"), stage_info.line_length_key_only));
    if (stage_info.supports_half_entries)
    {
        LOG_CLI((BSL_META("\t Supports half entries \n")));
        LOG_CLI((BSL_META("\t\t full key plus prefix %d \n"), stage_info.line_length_with_key_size_prefix));
    }

    LOG_CLI((BSL_META("\tAction memory Name\t    %s \n\n"), SOC_MEM_NAME(unit, memory)));

    LOG_CLI((BSL_META("\n")));

exit:
    SHR_FUNC_EXIT;
}

STATIC shr_error_e
diag_dbal_hard_logic_tcam_hw_info_dump(
    int unit,
    CONST dbal_hl_access_info_t * access_info,
    sh_sand_control_t * sand_control)
{
    char *memory_name;
    char *field_name;
    int jj, ii, to_print = 0;
    CONST dbal_hl_l2p_info_t *l2p_hl_info = access_info->l2p_hl_info;
    char info_string[CMD_MAX_STRING_LENGTH];

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_HL_ACCESS_TYPES; ii++)
    {
        if (ii == DBAL_HL_ACCESS_SW)
        {
            continue;
        }
        if (l2p_hl_info[ii].num_of_access_fields != 0)
        {
            to_print = 1;
            break;
        }
    }

    if (to_print)
    {
        PRT_TITLE_SET("HL access logical to physical info");
        PRT_INFO_ADD("Default entry: %s",
                     access_info->is_default_non_standard ? "Non-standard" : "Standard (all zero)");
        PRT_COLUMN_ADD("Type Name");
        PRT_COLUMN_ADD("Mapped to Mem/Reg/Group");
        PRT_COLUMN_ADD("HW field");

        for (ii = 0; ii < DBAL_NOF_HL_ACCESS_TYPES; ii++)
        {
            if (ii == DBAL_HL_ACCESS_SW)
            {
                continue;
            }
            for (jj = 0; jj < l2p_hl_info[ii].num_of_access_fields; jj++)
            {
                if (ii == DBAL_HL_ACCESS_MEMORY)
                {
                    memory_name = SOC_MEM_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].memory[0]);
                }
                else
                {
                    LOG_ERROR_EX(BSL_LOG_MODULE, "\n access type not parsed %d %s %s %s\n", ii, EMPTY, EMPTY, EMPTY);
                    SHR_EXIT();
                }

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s",
                             dbal_hl_tcam_access_type_to_string(unit,
                                                                l2p_hl_info[ii].
                                                                l2p_fields_info[jj].hl_tcam_access_type));
                PRT_CELL_SET("%sm", memory_name);

                if ((l2p_hl_info[ii].l2p_fields_info[jj].hw_field != 0) &&
                    (l2p_hl_info[ii].l2p_fields_info[jj].hw_field != INVALIDf))
                {
                    field_name = SOC_FIELD_NAME(unit, l2p_hl_info[ii].l2p_fields_info[jj].hw_field);
                    PRT_CELL_SET("%s", field_name);
                }
                else
                {
                    diag_dbal_offset_encode_info_to_str(unit, &(l2p_hl_info[ii].l2p_fields_info[jj].data_offset_info),
                                                        info_string);
                    if (sal_strcasecmp(info_string, "") == 0)
                    {
                        sal_snprintf(info_string, CMD_MAX_STRING_LENGTH, "%s", "default (KEY)");
                    }
                    PRT_CELL_SET("%s", info_string);
                }

            }
        }

        PRT_COMMITX;
        LOG_CLI((BSL_META("\n")));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

STATIC shr_error_e
diag_dbal_hard_logic_direct_sw_info_dump(
    int unit,
    CONST dbal_logical_table_t * table,
    CONST dbal_hl_l2p_info_t * l2p_hl_info,
    sh_sand_control_t * sand_control)
{
    int jj;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("SW access logical to physical info");

    PRT_COLUMN_ADD("Field Name");
    PRT_COLUMN_ADD("SW buffer size [Bits]");
    PRT_COLUMN_ADD("SW buffer offset [Bits]");
    PRT_COLUMN_ADD("Interface buffer size [Bits]");
    PRT_COLUMN_ADD("Interface buffer offset [Bits]");

    for (jj = 0; jj < l2p_hl_info[DBAL_HL_ACCESS_SW].num_of_access_fields; jj++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_field_to_string(unit, l2p_hl_info[DBAL_HL_ACCESS_SW].l2p_fields_info[jj].field_id));
        PRT_CELL_SET("%d", l2p_hl_info[DBAL_HL_ACCESS_SW].l2p_fields_info[jj].access_nof_bits);

        PRT_CELL_SET("%d", l2p_hl_info[DBAL_HL_ACCESS_SW].l2p_fields_info[jj].access_offset);

        PRT_CELL_SET("%d", l2p_hl_info[DBAL_HL_ACCESS_SW].l2p_fields_info[jj].nof_bits_in_interface);

        PRT_CELL_SET("%d", l2p_hl_info[DBAL_HL_ACCESS_SW].l2p_fields_info[jj].offset_in_interface);
    }

    PRT_COMMITX;
    LOG_CLI((BSL_META("\n")));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_table_indications_dump(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    sh_sand_control_t * sand_control)
{
    dbal_table_status_e table_status;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    if ((table_status == DBAL_TABLE_NOT_INITIALIZED))
    {
        LOG_CLI((BSL_META("Table not initialized\n")));
        SHR_EXIT();
    }

    PRT_TITLE_SET("Table Indications");

    PRT_COLUMN_ADD("Hitbit");
    PRT_COLUMN_ADD("Hook active");
    PRT_COLUMN_ADD("Result type");
    PRT_COLUMN_ADD("SW Rresult type");
    PRT_COLUMN_ADD("Range set");
    PRT_COLUMN_ADD("Priority");
    PRT_COLUMN_ADD("Learning");
    PRT_COLUMN_ADD("Commit mode");
    PRT_COLUMN_ADD("Image specific");
    PRT_COLUMN_ADD("Is Dirty");
    PRT_COLUMN_ADD("Iterator Mode");
    PRT_COLUMN_ADD("Default");
    PRT_COLUMN_ADD("Protection");
    PRT_COLUMN_ADD("Table validations");
    PRT_COLUMN_ADD("Collection Mode");

    /** Print table indications */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HITBIT_EN) ? "Enabled" : "Disabled"));
    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE) ? "YES" : "NO"));
    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE) ? "MULTIPLE" : "SINGLE"));
    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW) ? "YES" : "NO"));
    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED) ? "Supported" :
                        "Not Supported"));

    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED) ? "Supported" :
                        "Not Supported"));

    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN) ? "Enabled" : "Disabled"));

    PRT_CELL_SET("%s",
                 (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED) ? "Opimized" :
                  "Regular"));

    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE) ? "YES" : "NO"));
    PRT_CELL_SET("%s", (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_TABLE_DIRTY) ? "YES" : "NO"));

    PRT_CELL_SET("%s",
                 (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED) ? "Opimized" : "Regular"));
    PRT_CELL_SET("%s",
                 (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD) ? "Not standard" :
                  "Standard"));
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED))
    {
    PRT_CELL_SET("%s %d", "Enabled", table->mutex_id)}
    else
    {
        PRT_CELL_SET("%s", "Disabled");
    }

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS))
    {
        PRT_CELL_SET("Disabled");
    }
    else
    {
        PRT_CELL_SET("Enabled");
    }

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_COLLECT_MODE))
    {
        PRT_CELL_SET("Enabled");
    }
    else
    {
        PRT_CELL_SET("Disabled");
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

STATIC shr_error_e
diag_dbal_table_key_fields_dump(
    int unit,
    CONST dbal_logical_table_t * table,
    sh_sand_control_t * sand_control)
{
    int ii;
    dbal_field_types_basic_info_t *field_type_info;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Key-fields");

    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Offset");
    PRT_COLUMN_ADD("Min Value");
    PRT_COLUMN_ADD("Max Value");
    PRT_COLUMN_ADD("Const Value");
    PRT_COLUMN_ADD("is allocator");

    /** Print Key fields */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, table->keys_info[ii].field_id, &field_type_info));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_field_to_string(unit, table->keys_info[ii].field_id));
        PRT_CELL_SET("%s", field_type_info->name);
        PRT_CELL_SET("%d", table->keys_info[ii].field_nof_bits);
        if (table->keys_info[ii].field_id == DBAL_FIELD_CORE_ID)
        {
            PRT_CELL_SET("%s", "-");
        }
        else
        {
            PRT_CELL_SET("%d", table->keys_info[ii].bits_offset_in_buffer);
        }

        if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
        {
            PRT_CELL_SET("0x%x", table->keys_info[ii].min_value);
            PRT_CELL_SET("0x%x", table->keys_info[ii].max_value);
        }
        else
        {
            PRT_CELL_SET("%d", table->keys_info[ii].min_value);
            PRT_CELL_SET("%d", table->keys_info[ii].max_value);
        }

        if (SHR_BITGET(table->keys_info[ii].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
        {
            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
            {
                PRT_CELL_SET("0x%x", table->keys_info[ii].const_value);
            }
            else
            {
                PRT_CELL_SET("%d", table->keys_info[ii].const_value);
            }
        }
        else
        {
            PRT_CELL_SET("-");
        }

        if (SHR_BITGET(table->keys_info[ii].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR))
        {
            PRT_CELL_SET("Yes");
        }
        else
        {
            PRT_CELL_SET("No");
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

STATIC shr_error_e
diag_dbal_table_result_fields_dump(
    int unit,
    CONST dbal_logical_table_t * table,
    int result_idx,
    sh_sand_control_t * sand_control)
{
    int ii;
    dbal_field_types_basic_info_t *field_type_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        if (result_idx == table->nof_result_types)
        {
            PRT_TITLE_SET("Superset");
        }
        else
        {
            PRT_TITLE_SET("result type=%s (hw %d), payload size=%d bits",
                          table->multi_res_info[result_idx].result_type_name,
                          table->multi_res_info[result_idx].result_type_hw_value[0],
                          table->multi_res_info[result_idx].entry_payload_size);
        }
    }
    else
    {
        /** Title set is mandatory, set to empty srting  */
        PRT_TITLE_SET("payload size=%d bits", table->multi_res_info[result_idx].entry_payload_size);
    }

    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Offset");
    PRT_COLUMN_ADD("Valid Range");

    if (table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
    {
        PRT_COLUMN_ADD("SW buffer offset [Bits]");
    }

    PRT_COLUMN_ADD("Permission");

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        PRT_COLUMN_ADD("HW Mapping Value(s)");
    }

    PRT_COLUMN_ADD("Max Value");
    PRT_COLUMN_ADD("Min Value");
    PRT_COLUMN_ADD("Valid Indication");
    PRT_COLUMN_ADD("Arr Prefix,size");
    PRT_COLUMN_ADD("Const value");

    /** Print Results fields */
    for (ii = 0; ii < table->multi_res_info[result_idx].nof_result_fields; ii++)
    {
        dbal_table_field_info_t *results_info = &table->multi_res_info[result_idx].results_info[ii];
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, results_info->field_id, &field_type_info));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_field_to_string(unit, results_info->field_id));
        PRT_CELL_SET("%s", field_type_info->name);

        PRT_CELL_SET("%d", results_info->field_nof_bits);
        PRT_CELL_SET("%d", results_info->bits_offset_in_buffer);
        if (results_info->field_nof_bits < UTILEX_NOF_BITS_IN_UINT32)
        {
            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
            {
                PRT_CELL_SET("0x%x - 0x%x", results_info->min_value, results_info->max_value);
            }
            else
            {
                PRT_CELL_SET("%d - %d", results_info->min_value, results_info->max_value);
            }
        }
        else
        {
            PRT_CELL_SET("%d - By Size", results_info->min_value);
        }

        if (table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
        {
            PRT_CELL_SET("%d", results_info->bits_offset_in_buffer);
        }
        PRT_CELL_SET("%s", dbal_field_permission_to_string(unit, results_info->permission));

        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            if (results_info->field_id == DBAL_FIELD_RESULT_TYPE)
            {
                int jj;
                char buf[CMD_MAX_STRING_LENGTH];
                char *buf_ptr;
                int char_count;

                buf_ptr = &buf[0];

                char_count = sal_sprintf(buf_ptr, "%d", table->multi_res_info[result_idx].result_type_hw_value[0]);
                buf_ptr += char_count;

                for (jj = 1; jj < table->multi_res_info[result_idx].result_type_nof_hw_values; jj++)
                {
                    char_count =
                        sal_sprintf(buf_ptr, ", %d", table->multi_res_info[result_idx].result_type_hw_value[jj]);
                    buf_ptr += char_count;
                }

                PRT_CELL_SET("%s", &buf[0]);
            }
            else
            {
                PRT_CELL_SET("");
            }
        }
        if (results_info->field_nof_bits < UTILEX_NOF_BITS_IN_UINT32)
        {
            PRT_CELL_SET("%d", results_info->max_value);
        }
        else
        {
            PRT_CELL_SET("By Size");
        }
        PRT_CELL_SET("%d", results_info->min_value);
        PRT_CELL_SET("%s",
                     SHR_BITGET(results_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION) ? "V" : "X");
        if (results_info->arr_prefix_size > 0)
        {
            PRT_CELL_SET("%d, %d", results_info->arr_prefix, results_info->arr_prefix_size);
        }
        else
        {
            PRT_CELL_SET("0");
        }

        if (SHR_BITGET(results_info->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
        {
            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
            {
                PRT_CELL_SET("0x%x", results_info->const_value);
            }
            else
            {
                PRT_CELL_SET("%d", results_info->const_value);
            }
        }
        else
        {
            PRT_CELL_SET("-");
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_logical_table_dump(
    int unit,
    dbal_tables_e table_id,
    int print_access,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logical_table_t *table;
    int ii, jj;
    dbal_table_string_t table_string;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(dbal_full_table_to_string(unit, table_id, &table_string));

    DIAG_DBAL_HEADER_DUMP("Logical table info ", table->table_name);

    LOG_CLI((BSL_META("\tAccess method: %-10s \n"), table_string.access_method));
    LOG_CLI((BSL_META("\tTable type: %-10s \n"), table_string.table_type));
    LOG_CLI((BSL_META("\tTouched status: %-10s \n"), table_string.table_status));

    if (table->max_capacity != 0)
    {
        int nof_entries;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_entries));
        if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type != DBAL_TABLE_TYPE_TCAM)
            && (table->table_type != DBAL_TABLE_TYPE_DIRECT))
        {
            uint8 result_type_index = 0;
            int estimated_nof_entries;

            if (table->nof_result_types == 1)
            {
                SHR_IF_ERR_EXIT(diag_dnx_dbal_table_capacity_estimate
                                (unit, table_id, result_type_index, &estimated_nof_entries));
                LOG_CLI((BSL_META("\tEntries Status: Estimated Capacity %d, Committed %d, \n"), estimated_nof_entries,
                         nof_entries));
            }
            else
            {
                LOG_CLI((BSL_META("\tEntries Status: Committed %d, \n"), nof_entries));

                for (result_type_index = 0; result_type_index < table->nof_result_types; result_type_index++)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_dbal_table_capacity_estimate
                                    (unit, table_id, result_type_index, &estimated_nof_entries));
                    LOG_CLI((BSL_META("\t    resut_type[%s]: Estimated Capacity %d\n"),
                             dbal_result_type_to_string(unit, table_id, result_type_index), estimated_nof_entries));
                }
            }
        }
        else if (DBAL_ACCESS_METHOD_TCAM_CS == table->access_method && DBAL_CORE_MODE_DPC == table->core_mode)
        {
            LOG_CLI((BSL_META("\tEntries Status: Max Capacity(per core): %d, Committed %d, \n"),
                     table->max_capacity, nof_entries));
        }
        else if (table->access_method != DBAL_ACCESS_METHOD_MDB)
        {
            LOG_CLI((BSL_META("\tEntries Status: Max Capacity: %d, Committed %d, \n"),
                     table->max_capacity, nof_entries));
        }
        else
        {
            LOG_CLI((BSL_META("\tEntries Status: Max Capacity: HW dependent (see mapping), Committed %d \n"),
                     nof_entries));
        }
    }
    else if ((table->access_method == DBAL_ACCESS_METHOD_KBP) && !DBAL_TABLE_IS_TCAM(table))
    {
        int estimated_nof_entries;
        dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

        if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            LOG_CLI((BSL_META("\tKBP device is not locked!!!\n")));
        }
        SHR_IF_ERR_EXIT(diag_dnx_dbal_table_capacity_estimate(unit, table_id, 0, &estimated_nof_entries));
        LOG_CLI((BSL_META("\tEntries Status: Estimated Capacity %d\n"), estimated_nof_entries));
    }
    else
    {
        LOG_CLI((BSL_META("\tEntries Status: HW depended (see mapping) \n")));
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    if (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
    {
        LOG_CLI((BSL_META("\tno more information for table, Table is not active in current image!\n")));
        SHR_EXIT();
    }

    if (table_status == DBAL_NOF_TABLE_STATUS || table_status == DBAL_TABLE_HW_ERROR)
    {
        LOG_CLI((BSL_META("\tNo more information for table, only partial for table with status %s\n"),
                 table_string.table_status));
        SHR_EXIT();
    }
    LOG_CLI((BSL_META("\t%s\n"), table_string.range_set_supported));

    LOG_CLI((BSL_META("\tMaturity_level: %s \n"), dbal_maturity_level_to_string(unit, table->maturity_level)));

    LOG_CLI((BSL_META("\tTable Labels: %s\n"), table_string.labels));
    LOG_CLI((BSL_META("\tCore mode: %s\n"), table_string.core_mode));

    if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        uint32 key_buffer;
        SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get(unit, table_id, &key_buffer));
        LOG_CLI((BSL_META("\tMax key value: %d\n"), key_buffer));
    }
    LOG_CLI((BSL_META("\tMax payload size in bits: %d\n"), table->max_payload_size));
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))
    {
        LOG_CLI((BSL_META("\tTable is using special hook to perform access\n")));
    }
    LOG_CLI((BSL_META("\n")));

    LOG_CLI((BSL_META("Table fields: \n")));
    SHR_IF_ERR_EXIT(diag_dbal_table_key_fields_dump(unit, table, sand_control));
    LOG_CLI((BSL_META("\n")));

    LOG_CLI((BSL_META("Result-fields: \n")));
    for (ii = 0; ii < table->nof_result_types; ii++)
    {
        SHR_IF_ERR_EXIT(diag_dbal_table_result_fields_dump(unit, table, ii, sand_control));
    }
    LOG_CLI((BSL_META("\n")));

    /**Show superset info only for static tables*/
    if ((SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        && ((table_id < DBAL_NOF_STATIC_TABLES) || (table_id > DBAL_NOF_TABLES)))
    {
        LOG_CLI((BSL_META("\n\nSuperset of all the fields: \n")));
        SHR_IF_ERR_EXIT(diag_dbal_table_result_fields_dump(unit, table, table->nof_result_types, sand_control));
        LOG_CLI((BSL_META("\n")));
    }

    SHR_IF_ERR_EXIT(diag_dbal_table_indications_dump(unit, table_id, table, sand_control));
    LOG_CLI((BSL_META("\n")));

    if (print_access)
    {
        uint32 sw_nof_entries = 0;
        DIAG_DBAL_SUBHEADER_DUMP("Access information ", "");
        if (table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
        {
            for (ii = 0; ii < table->nof_result_types; ii++)
            {
                LOG_CLI((BSL_META("HL Mapping:\n")));

                if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
                {
                    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
                    {
                        LOG_CLI((BSL_META("Direct mapping of result type %s\n"),
                                 table->multi_res_info[ii].result_type_name));
                    }
                    if ((table->hl_mapping_multi_res[ii].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields > 0) ||
                        (table->hl_mapping_multi_res[ii].l2p_hl_info[DBAL_HL_ACCESS_REGISTER].num_of_access_fields > 0))
                    {
                         /* coverity[stack_use_overflow : FALSE]  */
                        SHR_IF_ERR_EXIT(diag_dbal_hard_logic_direct_hw_info_dump
                                        (unit, &(table->hl_mapping_multi_res[ii]), sand_control));
                        LOG_CLI((BSL_META("\n")));
                    }

                    if (table->hl_mapping_multi_res[ii].l2p_hl_info[DBAL_HL_ACCESS_SW].num_of_access_fields > 0)
                    {
                        SHR_IF_ERR_EXIT(dbal_sw_table_nof_entries_calculate
                                        (unit, (dbal_logical_table_t *) table, &sw_nof_entries));

                        LOG_CLI((BSL_META("SW Mapping:\n")));
                        LOG_CLI((BSL_META("sw state table size = %d, sw state entry length = %d bytes\n"),
                                 sw_nof_entries, table->sw_access_info.sw_payload_length_bytes));
                        SHR_IF_ERR_EXIT(diag_dbal_hard_logic_direct_sw_info_dump
                                        (unit, table, table->hl_mapping_multi_res[ii].l2p_hl_info, sand_control));
                    }

                    /*
                     * no access defined. This case might happen when no mapping is defined for a specific result type.
                     */
                    if ((table->hl_mapping_multi_res[ii].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields == 0)
                        && (table->hl_mapping_multi_res[ii].l2p_hl_info[DBAL_HL_ACCESS_REGISTER].num_of_access_fields ==
                            0)
                        && (table->hl_mapping_multi_res[ii].l2p_hl_info[DBAL_HL_ACCESS_SW].num_of_access_fields == 0))
                    {
                        LOG_CLI((BSL_META("No access defined for this result type \n\n")));
                    }

                }
                else if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                {
                    SHR_IF_ERR_EXIT(diag_dbal_hard_logic_tcam_hw_info_dump
                                    (unit, &(table->hl_mapping_multi_res[0]), sand_control));
                    LOG_CLI((BSL_META("\n")));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                                 dbal_table_type_to_string(unit, table->table_type), table->table_name);
                }
            }
        }
        else if (table->access_method == DBAL_ACCESS_METHOD_PEMLA)
        {
            LOG_CLI((BSL_META("PEMLA Mapping:\n")));
            if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
            {
                LOG_CLI((BSL_META("  Register ID %d\n"), table->app_id));
                LOG_CLI((BSL_META("  Result fields: \n")));
                for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
                {
                    LOG_CLI((BSL_META("   %s mapped to HW ID %02d\n"),
                             dbal_field_to_string(unit, table->multi_res_info[0].results_info[ii].field_id),
                             table->pemla_mapping.result_fields_mapping[ii]));
                }
            }
            else
            {
                LOG_CLI((BSL_META("Not supported\n")));
            }
        }
        else if (table->access_method == DBAL_ACCESS_METHOD_SW_STATE)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_nof_entries_calculate(unit, (dbal_logical_table_t *) table, &sw_nof_entries));
            LOG_CLI((BSL_META("SW Mapping:\n")));
            LOG_CLI((BSL_META("sw state table size = %d, sw state entry length = %d bytes\n"), sw_nof_entries,
                     table->sw_access_info.sw_payload_length_bytes));
        }
        else if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            dbal_physical_table_def_t *physical_table;
            LOG_CLI((BSL_META("MDB Mapping:\n")));
            for (jj = 0; jj < table->nof_physical_tables; jj++)
            {
                SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table->physical_db_id[jj], &physical_table));

                LOG_CLI((BSL_META("\tPhysical table: %-20s\n"),
                         dbal_physical_table_to_string(unit, table->physical_db_id[jj])));

                if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
                {
                    SHR_IF_ERR_EXIT(dbal_sw_table_nof_entries_calculate
                                    (unit, (dbal_logical_table_t *) table, &sw_nof_entries));
                    LOG_CLI((BSL_META("\tResult type is mapped to SW\n")));
                    LOG_CLI((BSL_META("\t\tsw state table size = %d, sw state entry length = %d bytes\n"),
                             sw_nof_entries, table->sw_access_info.sw_payload_length_bytes));
                }

                if (dbal_tables_is_non_direct(unit, table_id))
                {
                    int capacity = 0;
                    int nof_entries;
                    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, table->physical_db_id[jj], &capacity));
                    SHR_IF_ERR_EXIT(DBAL_SW_STATE_MDB_PHY_TBL.
                                    nof_entries.get(unit, table->physical_db_id[jj], &nof_entries));
                    LOG_CLI((BSL_META("\tEntries Status: Address Capacity %d, Committed %d, \n"), capacity,
                             nof_entries));
                }
            }

            if (DBAL_TABLE_IS_TCAM(table))
            {
                uint32 tcam_handler_id;

                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, table_id, &tcam_handler_id));
                LOG_CLI((BSL_META("\tApp DB ID: Size %d bits Value %d\n"), table->app_id_size, table->app_id));
                LOG_CLI((BSL_META("\tTCAM Handler ID: Value %d\n"), tcam_handler_id));
            }
            else if (table->app_id_size != 0)
            {
                LOG_CLI((BSL_META("\tApp DB ID: Size %d bits, Value %d\n"), table->app_id_size, table->app_id));
            }
            else
            {
                LOG_CLI((BSL_META("\tApp DB ID: N/A\n")));
            }

            if (table->table_type == DBAL_TABLE_TYPE_EM)
            {
                LOG_CLI((BSL_META("\tMDB Entry Type (ONE/HALF/QUARTER/EIGHTH) per result type:\n")));
                for (ii = 0; ii < table->nof_result_types; ii++)
                {
                    mdb_em_entry_encoding_e entry_encoding;
                    uint32 physical_result_size = table->multi_res_info[ii].entry_payload_size;
                    uint32 physical_key_size = table->key_size - table->core_id_nof_bits;
                    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
                    {
                        physical_result_size -= table->multi_res_info[ii].results_info[0].field_nof_bits;
                    }
                    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding(unit,
                                                              table->physical_db_id[0],
                                                              physical_key_size,
                                                              physical_result_size,
                                                              table->app_id, table->app_id_size, &entry_encoding));
                    LOG_CLI((BSL_META("\t\tResult type %d, MDB Entry %s\n"), ii,
                             entry_encoding == MDB_EM_ENTRY_ENCODING_ONE ? "ONE" :
                             entry_encoding == MDB_EM_ENTRY_ENCODING_HALF ? "HALF" :
                             entry_encoding == MDB_EM_ENTRY_ENCODING_QUARTER ? "QUARTER" :
                             entry_encoding == MDB_EM_ENTRY_ENCODING_EIGHTH ? "EIGHTH" : "EMPTY"));
                }
            }
        }
        else if (table->access_method == DBAL_ACCESS_METHOD_TCAM_CS)
        {
            SHR_IF_ERR_EXIT(diag_dbal_tcam_cs_hard_coded_memory_dump
                            (unit, table->dbal_stage, table->tcam_cs_mapping->memory));
            LOG_CLI((BSL_META("\n")));
        }
        else if (table->access_method == DBAL_ACCESS_METHOD_KBP)
        {
            uint32 db_id;
            dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

            SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

            if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
            {
                LOG_CLI((BSL_META("KBP device is not locked!!!\n")));
            }

            LOG_CLI((BSL_META("KBP mapping\n")));
            if (dbal_tables_app_db_id_get(unit, table_id, &db_id, NULL) != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("KBP DB ID not initialized\n")));
            }
            else
            {
                int nof_entries = 0;

                LOG_CLI((BSL_META("\tKBP DB ID: %d\n"), db_id));

                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_entries));
                LOG_CLI((BSL_META("\tEntries Status: Committed %d\n"), nof_entries));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_physical_tables_info_dump(
    int unit,
    dbal_physical_tables_e phy_table_id,
    sh_sand_control_t * sand_control)
{
    dbal_physical_table_def_t *physical_table;
    int capacity = 0;
    int nof_entries;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, phy_table_id, &physical_table));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, phy_table_id, &capacity));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_MDB_PHY_TBL.nof_entries.get(unit, phy_table_id, &nof_entries));

    LOG_CLI((BSL_META("\n")));

    PRT_TITLE_SET("Physical table %s Info", dbal_physical_table_to_string(unit, phy_table_id));

    PRT_COLUMN_ADD("Capacity");
    PRT_COLUMN_ADD("Num of Entries");
    PRT_COLUMN_ADD("Core Mode");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", capacity);
    if (nof_entries != 0)
    {
        PRT_CELL_SET("%d", nof_entries);
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }
    PRT_CELL_SET("%s", dbal_core_mode_to_string(unit, physical_table->physical_core_mode));

    PRT_COMMITX;

    LOG_CLI((BSL_META("\n")));
    SHR_IF_ERR_EXIT(dbal_physical_table_print_related_tables(unit, phy_table_id, sand_control));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_field_type_print_info(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_labels_e label,
    sh_sand_control_t * sand_control)
{
    CONST dbal_field_types_basic_info_t *field_type_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));

    LOG_CLI((BSL_META("\n\n")));
    PRT_TITLE_SET("Field Type %s Info", field_type_info->name);

    PRT_COLUMN_ADD("Size in bits");
    PRT_COLUMN_ADD("Min value");
    PRT_COLUMN_ADD("Max value");
    PRT_COLUMN_ADD("Const value");
    PRT_COLUMN_ADD("Illegal values");
    PRT_COLUMN_ADD("Print type");
    PRT_COLUMN_ADD("is allocator");
    PRT_COLUMN_ADD("Default value");
    PRT_COLUMN_ADD("Encode mode");
    PRT_COLUMN_ADD("Encode input param");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    /**Size in bits  */
    PRT_CELL_SET("%d", field_type_info->max_size);

    /**Min */
    if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
    {
        PRT_CELL_SET("0x%x", field_type_info->min_value);
    }
    else
    {
        PRT_CELL_SET("%d", field_type_info->min_value);
    }

    /**Max  */
    if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
    {
        PRT_CELL_SET("0x%x", field_type_info->max_value);
    }
    else
    {
        PRT_CELL_SET("%d", field_type_info->max_value);
    }

    /** const value */
    if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
    {
        if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
        {
            PRT_CELL_SET("0x%x", field_type_info->const_value);
        }
        else
        {
            PRT_CELL_SET("%d", field_type_info->const_value);
        }
    }
    else
    {
        PRT_CELL_SET(" - ");
    }

    /**Illegal values*/
    if (field_type_info->nof_illegal_values != 0)
    {
        char buffer_to_print[CMD_MAX_STRING_LENGTH];
        char *current_loc;

        int char_count = 0;
        int ii;

        current_loc = &buffer_to_print[0];
        for (ii = 0; ii < field_type_info->nof_illegal_values; ii++)
        {
            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
            {
                char_count = sal_sprintf(current_loc, "0x%x", field_type_info->illegal_values[ii]);
                current_loc += char_count;
            }
            else
            {
                char_count = sal_sprintf(current_loc, "%d", field_type_info->illegal_values[ii]);
                current_loc += char_count;
            }
            if (ii != (field_type_info->nof_illegal_values - 1))
            {
                char_count = sal_sprintf(current_loc, ", ");
                current_loc += char_count;
            }
        }
        PRT_CELL_SET("%s", buffer_to_print);
    }
    else
    {
        if (field_type_info->illegal_value_cb)
        {
            PRT_CELL_SET("By callBack");

        }
        else
        {
            PRT_CELL_SET("-");
        }
    }

    PRT_CELL_SET("%s", dbal_field_print_type_to_string(unit, field_type_info->print_type));

    if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR))
    {
        PRT_CELL_SET("Yes");
    }
    else
    {
        PRT_CELL_SET("No");
    }

    if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID))
    {
        if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
        {
            PRT_CELL_SET("0x%x", field_type_info->default_value);
        }
        else
        {
            PRT_CELL_SET("%d", field_type_info->default_value);
        }
    }
    else
    {
        PRT_CELL_SET(" - ");
    }

    if (field_type_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
    {
        PRT_CELL_SET("%s", dbal_field_encode_type_to_string(unit, field_type_info->encode_info.encode_mode));
        PRT_CELL_SET("%d", field_type_info->encode_info.input_param);
    }
    else
    {
        PRT_CELL_SET("NONE");
        PRT_CELL_SET(" - ");
    }

    PRT_COMMITX;

    LOG_CLI((BSL_META("\n\n")));

    if (field_type_info->nof_enum_values > 0)
    {
        int ii;

        PRT_TITLE_SET("Enum values");
        PRT_INFO_ADD("nof invalid enum values %d", field_type_info->nof_invalid_enum_values);

        PRT_COLUMN_ADD("Name");
        PRT_COLUMN_ADD("Value");
        PRT_COLUMN_ADD("Status");

        for (ii = 0; ii < field_type_info->nof_enum_values; ii++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", field_type_info->enum_val_info[ii].name);
            PRT_CELL_SET("%d", field_type_info->enum_val_info[ii].value);
            if (field_type_info->enum_val_info[ii].is_invalid)
            {
                PRT_CELL_SET("INVALID");
            }
            else
            {
                PRT_CELL_SET("Normal");
            }
        }

        PRT_COMMITX;
    }
    else if (field_type_info->nof_struct_fields > 0)
    {
        /** Print child fields */
        int ii;
        PRT_TITLE_SET("Sub-Struct fields");

        PRT_COLUMN_ADD("Field");
        PRT_COLUMN_ADD("Size");
        PRT_COLUMN_ADD("Offset");
        PRT_COLUMN_ADD("ArrPrefixSize");
        PRT_COLUMN_ADD("ArrPrefixVal");

        for (ii = 0; ii < field_type_info->nof_struct_fields; ii++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", dbal_field_to_string(unit, field_type_info->struct_field_info[ii].struct_field_id));
            PRT_CELL_SET("%d", field_type_info->struct_field_info[ii].length);
            PRT_CELL_SET("%d", field_type_info->struct_field_info[ii].offset);
            PRT_CELL_SET("%d", field_type_info->struct_field_info[ii].arr_prefix_size);
            PRT_CELL_SET("%d", field_type_info->struct_field_info[ii].arr_prefix);
        }
        PRT_COMMITX;
    }
    else if (field_type_info->nof_child_fields > 0)
    {
        /** Print child fields */
        int ii;
        PRT_TITLE_SET("Child fields");
        if (field_type_info->reference_field_id != DBAL_FIELD_TYPE_DEF_EMPTY)
        {
            PRT_INFO_ADD("Reference Field name: %s",
                         dbal_field_types_def_to_string(unit, field_type_info->reference_field_id));
        }

        PRT_COLUMN_ADD("Field");
        PRT_COLUMN_ADD("Encoding");
        PRT_COLUMN_ADD("value");

        for (ii = 0; ii < field_type_info->nof_child_fields; ii++)
        {
            uint32 field_size;

            SHR_IF_ERR_EXIT(dbal_fields_max_size_get
                            (unit, field_type_info->sub_field_info[ii].sub_field_id, &field_size));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", dbal_field_to_string(unit, field_type_info->sub_field_info[ii].sub_field_id));
            PRT_CELL_SET("%s",
                         dbal_field_encode_type_to_string(unit,
                                                          field_type_info->sub_field_info[ii].encode_info.encode_mode));
            PRT_CELL_SET("%d", field_type_info->sub_field_info[ii].encode_info.input_param);
        }
        PRT_COMMITX;
    }

    dbal_field_types_override_info_dump(unit, field_type_info);

    if (label == DBAL_LABEL_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_fields_type_related_labels_dump(unit, field_type));
        SHR_IF_ERR_EXIT(dbal_fields_type_related_tables_dump(unit, field_type, DBAL_LABEL_NONE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_type_related_tables_dump(unit, field_type, label));
    }

    if (field_type_info->nof_defines_values > 0)
    {
        int ii;
        int str_len = sal_strlen(field_type_info->name) + 1;
        LOG_CLI((BSL_META("\tDefined values:\n")));
        for (ii = 0; ii < field_type_info->nof_defines_values; ii++)
        {
            LOG_CLI((BSL_META("\tName %s,\tValue %d\n"), &(field_type_info->defines_info[ii].name[str_len]),
                     field_type_info->defines_info[ii].value));
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
