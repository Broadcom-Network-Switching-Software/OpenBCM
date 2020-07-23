/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include <stdlib.h>
#include <sal/core/libc.h>
#include "dbal_internal.h"
#include "dbal_formula_cb.h"
#include <shared/bsl.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>

#ifdef BCM_DNX_SUPPORT
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_field_types.h>
#endif
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <sal/appl/config.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_arr.h>

static dbal_field_types_basic_info_t *field_types_basic_info[BCM_MAX_NUM_UNITS][DBAL_NOF_FIELD_TYPES_DEF] = { {0} };


extern dbal_field_types_defs_e dbal_field_id_to_type[DBAL_NOF_FIELDS];

#define DBAL_FIELD_ID_VALIDATE( field_id)                                                         \
        if (field_id >= DBAL_NOF_FIELDS + DBAL_NOF_DYNAMIC_FIELDS)                                \
        {                                                                                         \
            SHR_ERR_EXIT(_SHR_E_PARAM,"Illegal field_id %d, max=%d\n", field_id, DBAL_NOF_FIELDS);\
        }

#define DBAL_FIELD_TYPE_ID_VALIDATE( field_type)                                                                \
        if (field_type >= DBAL_NOF_FIELD_TYPES_DEF || field_type == DBAL_FIELD_TYPE_DEF_EMPTY)                  \
        {                                                                                                       \
            SHR_ERR_EXIT(_SHR_E_PARAM,"Illegal field_type %d, max=%d\n", field_type, DBAL_NOF_FIELD_TYPES_DEF); \
        }

#define FIELD_TYPE(_field_id)  ((_field_id < DBAL_NOF_FIELDS)? dbal_field_id_to_type[_field_id] : dbal_dynamic_field_id_to_type[_field_id - DBAL_NOF_FIELDS])

#define FIELD_TYPE_INFO(_field_type) field_types_basic_info[unit][(_field_type)]

#define DBAL_DYNAMIC_FIELD_TYPE_DIR "/dbal_dynamic/field_types/auto_generated"




static shr_error_e
dbal_field_types_update_structure_info(
    int unit,
    dbal_field_types_defs_e field_type)
{
    int ii;
    dbal_field_types_basic_info_t *int_field_type_info;
    dbal_field_types_basic_info_t *struct_info = field_types_basic_info[unit][field_type];

    SHR_FUNC_INIT_VARS(unit);

    
    for (ii = 0; ii < struct_info->nof_struct_fields; ii++)
    {
        if ((struct_info->struct_field_info[ii].length == 0))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                            (unit, struct_info->struct_field_info[ii].struct_field_id, &int_field_type_info));
            struct_info->struct_field_info[ii].length = int_field_type_info->max_size;
        }
    }

    
    for (ii = 1; ii < struct_info->nof_struct_fields; ii++)
    {
        struct_info->struct_field_info[ii].offset =
            struct_info->struct_field_info[ii - 1].length + struct_info->struct_field_info[ii - 1].offset +
            struct_info->struct_field_info[ii].offset;
    }

    
    for (ii = 0; ii < struct_info->nof_struct_fields; ii++)
    {
        if (struct_info->struct_field_info[ii].arr_prefix_table_entry_width)
        {

            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                            (unit, struct_info->struct_field_info[ii].struct_field_id, &int_field_type_info));

            struct_info->struct_field_info[ii].arr_prefix_size =
                DBAL_FIELD_ARR_PREFIX_SIZE_CALCULATE(int_field_type_info->max_size,
                                                     struct_info->struct_field_info[ii].length);
            struct_info->struct_field_info[ii].arr_prefix =
                DBAL_FIELD_ARR_PREFIX_VALUE_CALCULATE(struct_info->struct_field_info[ii].arr_prefix_table_entry_width,
                                                      struct_info->struct_field_info[ii].arr_prefix_table_entry_value,
                                                      struct_info->struct_field_info[ii].arr_prefix_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_field_types_init_max_size_validation(
    int unit,
    dbal_field_types_defs_e field_type)
{
    int max_field_size;

    dbal_field_types_basic_info_t *field_types_info = field_types_basic_info[unit][field_type];

    SHR_FUNC_INIT_VARS(unit);

    switch (field_types_info->print_type)
    {
        case DBAL_FIELD_PRINT_TYPE_ARRAY32:
        case DBAL_FIELD_PRINT_TYPE_ARRAY8:
        case DBAL_FIELD_PRINT_TYPE_BITMAP:
        case DBAL_FIELD_PRINT_TYPE_HEX:
        case DBAL_FIELD_PRINT_TYPE_DBAL_TABLE:
        case DBAL_FIELD_PRINT_TYPE_STR:
            max_field_size = BYTES2BITS(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            break;
        case DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE:
        case DBAL_FIELD_PRINT_TYPE_BOOL:
            max_field_size = 1;
            break;
        case DBAL_FIELD_PRINT_TYPE_UINT32:
        case DBAL_FIELD_PRINT_TYPE_IPV4:
        case DBAL_FIELD_PRINT_TYPE_ENUM:
            max_field_size = 32;
            break;
        case DBAL_FIELD_PRINT_TYPE_STRUCTURE:
            max_field_size = 256;

            break;
        case DBAL_FIELD_PRINT_TYPE_MAC:
            max_field_size = 48;
            break;
        case DBAL_FIELD_PRINT_TYPE_IPV6:
            max_field_size = 128;
            break;
        case DBAL_FIELD_PRINT_TYPE_NONE:
        case DBAL_NOF_FIELD_PRINT_TYPES:
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field print type, field type %s\n",
                         dbal_field_types_def_to_string(unit, field_type));
    }

    if (max_field_size > DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field print type %s max size is larger than allowed.\n",
                     dbal_field_print_type_to_string(unit, field_types_info->print_type));
    }
    if (field_types_info->max_size > max_field_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field size for field %s is too long %d, max size of is %d bits\n",
                     field_types_info->name, field_types_info->max_size, max_field_size);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_field_types_validation(
    int unit)
{
    int field_type;

    SHR_FUNC_INIT_VARS(unit);

    for (field_type = DBAL_FIELD_TYPE_DEF_EMPTY + 1; field_type < DBAL_NOF_FIELD_TYPES_DEF; field_type++)
    {
        dbal_field_types_basic_info_t *field_type_info = field_types_basic_info[unit][field_type];

        
        if (!SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID))
        {
            continue;
        }

        
        SHR_IF_ERR_EXIT(dbal_field_types_init_max_size_validation(unit, field_type));

        
        if (field_types_basic_info[unit][field_type]->print_type == DBAL_FIELD_PRINT_TYPE_STRUCTURE)
        {
            SHR_IF_ERR_EXIT(dbal_field_types_update_structure_info(unit, field_type));
        }

        
        SHR_IF_ERR_EXIT(dbal_field_types_init_illegal_value_cb_assign(unit, field_type, field_type_info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_field_types_db_init(
    int unit)
{
    field_type_db_struct_t *cur_field_type_param = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(cur_field_type_param, sizeof(field_type_db_struct_t),
              "field_types struct", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->childs = NULL;
    SHR_ALLOC(cur_field_type_param->childs,
              DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS * sizeof(dbal_db_child_field_info_struct_t), "field_types,childs",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->enums = NULL;
    SHR_ALLOC(cur_field_type_param->enums, DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES * sizeof(dbal_db_enum_info_struct_t),
              "field_types,enums", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->define = NULL;
    SHR_ALLOC(cur_field_type_param->define, DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES * sizeof(dbal_db_defines_info_struct_t),
              "field_types,define", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->illegal_values = NULL;
    SHR_ALLOC(cur_field_type_param->illegal_values,
              DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES * sizeof(int),
              "field_types,illegal_values", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    

#ifdef BCM_DNX_SUPPORT
    if (DBAL_IS_DNX2(unit))
    {
        SHR_IF_ERR_EXIT(dbal_init_field_types_init(unit, cur_field_type_param, field_types_basic_info[unit]));
    }
#endif
exit:
    SHR_FREE(cur_field_type_param->childs);
    SHR_FREE(cur_field_type_param->enums);
    SHR_FREE(cur_field_type_param->define);
    SHR_FREE(cur_field_type_param->illegal_values);
    SHR_FREE(cur_field_type_param);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_basic_info_t_init(
    int unit,
    dbal_field_types_basic_info_t * field_type_info)
{
    CONST static dbal_field_types_basic_info_t field_type_default = {
        EMPTY,                          
        0,                              
        DBAL_FIELD_PRINT_TYPE_NONE,     
        {0},                            
        0,                              
        0,                              
        0,                              
        0,                              
        0,                              
        {0},                            
        0,                              
        NULL,                           
        0,                              
        NULL,                           
        0,                              
        0,                              
        NULL,                           
        NULL,                           
        0,                              
        NULL,                           
        0,                              
        {0}                             
    };
    SHR_FUNC_INIT_VARS(unit);

    (*field_type_info) = field_type_default;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_field_types_defaults_set(
    int unit)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    
    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_basic_info_t_init(unit, field_types_basic_info[unit][ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dynamic_xml_field_types_update(
    int unit,
    xml_node top_node)
{
    xml_node cur_node = NULL;
    xml_node tmp_node = NULL;
    xml_node field_type_node, enum_values_node, enum_values_mapping_node, cur_enum_node;
    char field_type_name[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    dbal_field_types_defs_e field_types_def_id;
    dbal_field_types_basic_info_t *field_type_info;
    int enum_val;
    uint32 num_new_enums = 0, nof_identical_enum_values = 0;
    char str[DBAL_MAX_STRING_LENGTH];
    char name_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    int next_free_idx;
    uint32 enum_idx;
    dbal_enum_decoding_info_t *tmp_enum_val_info = NULL;
    int rv;
    int new_enum_iter, new_enum_desc_iter;

    
    uint8 is_common = 0;
    uint8 is_default = 0;
    char image[RHNAME_MAX_SIZE];
    uint8 is_specific_image = 0;

    xml_node general_info_node;

    SHR_FUNC_INIT_VARS(unit);

    general_info_node = dbx_xml_child_get_first(top_node, "GeneralInfo");

    if (general_info_node != NULL)
    {
        
        RHDATA_GET_INT_DEF(general_info_node, "isCommon", is_common, 0);
        RHDATA_GET_INT_DEF(general_info_node, "isDefault", is_default, 0);
        RHDATA_GET_STR_DEF_NULL(general_info_node, "image", image) is_specific_image = !ISEMPTY(image);

        if (is_common)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported option in GeneralInfo: isCommon");
        }
        if (is_default)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported option in GeneralInfo: isDefault");
        }

        if (!is_specific_image)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected image in GeneralInfo");
        }
    }

    
    RHDATA_ITERATOR(field_type_node, top_node, "FieldType")
    {
        sal_memset(str, '\0', DBAL_MAX_STRING_LENGTH);
        sal_memset(name_str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH - 1);
        sal_memset(field_type_name, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        RHDATA_GET_XSTR_STOP(field_type_node, "Print", str, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        if (!sal_strcmp(str, "EMUM"))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field type %s, only ENUM is allowed", str);
        }
        sal_memset(str, '\0', DBAL_MAX_STRING_LENGTH);
        
        tmp_node = dbx_xml_child_get_first(field_type_node, "FieldDbInterface");
        if (tmp_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected xml tag FieldDbInterface not found");
        }
        enum_values_node = dbx_xml_child_get_first(tmp_node, "EnumValues");
        if (enum_values_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected xml tag EnumValues not found");
        }

        num_new_enums = 0;
        nof_identical_enum_values = 0;
        RHDATA_GET_XSTR_STOP(field_type_node, "Name", field_type_name, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
        rv = dbal_field_types_def_string_to_id(unit, field_type_name, &field_types_def_id);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "New field types dynamic loading is not supported");
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }

        RHDATA_ITERATOR(cur_enum_node, enum_values_node, "EnumVal")
        {
            RHDATA_GET_XSTR_STOP(cur_enum_node, "Name", str, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
            rv = dbal_enum_type_string_to_id_no_err(unit, field_types_def_id, str, &enum_idx);
            if (rv == _SHR_E_NOT_FOUND)
            {
                num_new_enums++;
            }
            else if (rv == _SHR_E_NONE)
            {
                nof_identical_enum_values++;
            }
            else
            {
                SHR_SET_CURRENT_ERR(rv);
                SHR_EXIT();
            }
        }
        
        field_type_info = field_types_basic_info[unit][field_types_def_id];

        
        if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_COMMON))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Overriden field type shouldn't have common property in GeneralInfo");
        }

        if (!SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected isDefault property in GeneralInfo");
        }

        
        field_type_info->nof_invalid_enum_values = field_type_info->max_value - nof_identical_enum_values;
        SHR_BITSET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DYNAMIC);
        field_type_info->max_value += num_new_enums;

        
        SHR_FREE(tmp_enum_val_info);
        SHR_ALLOC_SET_ZERO(tmp_enum_val_info,
                           field_type_info->nof_enum_values * sizeof(dbal_enum_decoding_info_t),
                           "enum info allocation", "%s%s%s\r\n", field_type_info->name, EMPTY, EMPTY);
        sal_memcpy(tmp_enum_val_info, field_type_info->enum_val_info,
                   field_type_info->nof_enum_values * sizeof(dbal_enum_decoding_info_t));

        
        SHR_FREE(field_type_info->enum_val_info);
        SHR_ALLOC_SET_ZERO(field_type_info->enum_val_info,
                           (field_type_info->nof_enum_values + num_new_enums) * sizeof(dbal_enum_decoding_info_t),
                           "enum info allocation", "%s%s%s\r\n", field_type_info->name, EMPTY, EMPTY);
        sal_memcpy(field_type_info->enum_val_info, tmp_enum_val_info,
                   field_type_info->nof_enum_values * sizeof(dbal_enum_decoding_info_t));
        
        for (enum_idx = 0; enum_idx < field_type_info->nof_enum_values; enum_idx++)
        {
            field_type_info->enum_val_info[enum_idx].is_invalid = TRUE;
        }
        next_free_idx = field_type_info->nof_enum_values;
        field_type_info->nof_enum_values = field_type_info->nof_enum_values + num_new_enums;

        tmp_node = dbx_xml_child_get_first(field_type_node, "FieldToPhyDbMapping");
        if (tmp_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected xml tag FieldToPhyDbMapping not found");
        }
        cur_node = dbx_xml_child_get_first(tmp_node, "EnumValueMapping");
        if (cur_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "expected xml tag EnumValueMapping not found");
        }

        
        new_enum_iter = 0;
        RHDATA_ITERATOR(enum_values_mapping_node, cur_node, "EnumVal")
        {
            
            new_enum_desc_iter = 0;
            sal_memset(name_str, '\0', DBAL_MAX_EXTRA_LONG_STRING_LENGTH - 1);
            RHDATA_ITERATOR(cur_enum_node, enum_values_node, "EnumVal")
            {
                if (new_enum_desc_iter < new_enum_iter)
                {
                    new_enum_desc_iter++;
                    continue;
                }
                else
                {
                    break;
                }
            }
            RHDATA_GET_XSTR_STOP(cur_enum_node, "Name", name_str, DBAL_MAX_EXTRA_LONG_STRING_LENGTH - 1);
            sal_memset(str, '\0', DBAL_MAX_STRING_LENGTH);
            RHDATA_GET_XSTR_STOP(enum_values_mapping_node, "Name", str, DBAL_MAX_STRING_LENGTH);
            if (sal_strcmp(name_str, str))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Found a mismatch between enum values enum %s and enum values mapping enum %s", name_str,
                             str);
            }
            sal_memset(name_str, '\0', DBAL_MAX_STRING_LENGTH);
            sal_snprintf(name_str, DBAL_MAX_STRING_LENGTH, "%s", str);
            rv = dbal_enum_type_string_to_id_no_err(unit, field_types_def_id, str, &enum_idx);
            if (rv == _SHR_E_NOT_FOUND)
            {
                
                enum_idx = next_free_idx++;
                sal_strncpy(field_type_info->enum_val_info[enum_idx].name, name_str, DBAL_MAX_STRING_LENGTH - 1);
            }
            RHDATA_GET_XSTR_STOP(enum_values_mapping_node, "Value", str, DBAL_MAX_STRING_LENGTH);
            if (!sal_strcmp(str, "INVALID"))
            {
                field_type_info->enum_val_info[enum_idx].is_invalid = TRUE;
                field_type_info->nof_invalid_enum_values++;
            }
            else
            {
                RHDATA_GET_INT_STOP(enum_values_mapping_node, "Value", enum_val);
                field_type_info->enum_val_info[enum_idx].value = enum_val;
                field_type_info->enum_val_info[enum_idx].is_invalid = FALSE;
            }
            new_enum_iter++;
        }
    }

exit:
    if (tmp_enum_val_info)
    {
        sal_free(tmp_enum_val_info);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_alloc(
    int unit)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ++ii)
    {
        SHR_ALLOC_SET_ZERO(FIELD_TYPE_INFO(ii), sizeof(dbal_field_types_basic_info_t),
                           "dbal_field_types_basic_info_t", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_dealloc(
    int unit)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ++ii)
    {
        SHR_FREE(FIELD_TYPE_INFO(ii));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_types_defaults_set(unit));

    SHR_IF_ERR_EXIT(dbal_field_types_db_init(unit));

    SHR_IF_ERR_EXIT(dbal_field_types_validation(unit));

    
    if (DBAL_DYNAMIC_XML_TABLES_ENABLED)
    {
        char file_path[RHNAME_MAX_SIZE] = { 0 };
        char *db_dir;
        uint8 dir_override = TRUE;

        
        db_dir = sal_config_get("dpp_db_path");
        if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
        {
            db_dir = getenv("DPP_DB_PATH");
            if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
            {
                dir_override = FALSE;
                sal_strncpy(file_path, "./db", RHNAME_MAX_SIZE - 1);
            }
        }

        if (dir_override)
        {
            sal_strncpy(file_path, db_dir, RHNAME_MAX_SIZE - 1);
        }

        sal_strncat_s(file_path, DBAL_DYNAMIC_FIELD_TYPE_DIR, sizeof(file_path));

        SHR_IF_ERR_EXIT(dbal_dynamic_updates_dispatcher
                        (unit, file_path, dnx_dynamic_xml_field_types_update, "FieldTypesDbCatalog"));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_deinit(
    int unit)
{
    int ii;
    dbal_field_types_basic_info_t *field_type_entry;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        field_type_entry = field_types_basic_info[unit][ii];
        if (field_type_entry->nof_child_fields > 0)
        {
            SHR_FREE(field_type_entry->sub_field_info);
        }
        if (field_type_entry->nof_enum_values > 0)
        {
            SHR_FREE(field_type_entry->enum_val_info);
        }
        if (field_type_entry->nof_defines_values > 0)
        {
            SHR_FREE(field_type_entry->defines_info);
        }
        if (field_type_entry->nof_struct_fields > 0)
        {
            SHR_FREE(field_type_entry->struct_field_info);
        }
    }

    SHR_FUNC_EXIT;
}



shr_error_e
dbal_field_types_info_get(
    int unit,
    dbal_field_types_defs_e field_type,
    CONST dbal_field_types_basic_info_t ** field_type_info)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);

    (*field_type_info) = field_types_basic_info[unit][field_type];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_struct_field_size_get(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_fields_e field_id,
    uint32 *field_size_p)
{
    dbal_field_types_basic_info_t *field_type_info;
    dbal_sub_struct_field_info_t *struct_field_info;
    int i_f;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);
    SHR_NULL_CHECK(field_size_p, _SHR_E_INTERNAL, "field_size_p");

    field_type_info = field_types_basic_info[unit][field_type];

    if ((struct_field_info = field_type_info->struct_field_info) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field Type:%s has no structure\n", field_types_basic_info[unit][field_type]->name);
    }

    for (i_f = 0; i_f < field_type_info->nof_struct_fields; i_f++)
    {
        if (struct_field_info[i_f].struct_field_id == field_id)
        {
            *field_size_p = struct_field_info[i_f].length;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Structured Field Type:%s has no field:%s\n",
                 field_type_info->name, dbal_field_to_string(unit, field_id));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_field_types_enum_name_to_hw_value_get(
    int unit,
    dbal_field_types_defs_e field_type,
    char enum_value_name[DBAL_MAX_STRING_LENGTH],
    uint32 *hw_value)
{
    int ii;
    int nof_enum_vals;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);

    nof_enum_vals = field_types_basic_info[unit][field_type]->nof_enum_values;

    if (!nof_enum_vals)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " field type is not enum %s\n", field_types_basic_info[unit][field_type]->name);
    }

    for (ii = 0; ii < nof_enum_vals; ii++)
    {

        if (!sal_strcmp(enum_value_name, field_types_basic_info[unit][field_type]->enum_val_info[ii].name))
        {
            if (field_types_basic_info[unit][field_type]->enum_val_info[ii].is_invalid)
            {
                dbal_status_e dbal_status;
                SHR_IF_ERR_EXIT(dbal_status_get(unit, &dbal_status));
                if ((dbal_status == DBAL_STATUS_DEVICE_INIT_DONE) || dbal_image_name_is_std_1(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "HW name %s is invalid for enum %s.\n",
                                 enum_value_name, field_types_basic_info[unit][field_type]->name);
                }
                else
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Field name %s is invalid for field type %s.\n"),
                                              enum_value_name, field_types_basic_info[unit][field_type]->name));
                }
            }
            (*hw_value) = field_types_basic_info[unit][field_type]->enum_val_info[ii].value;
            break;
        }
    }
    if (ii == nof_enum_vals)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Decoding failed for type %s HW name %s has no logical value\n",
                     field_types_basic_info[unit][field_type]->name, enum_value_name);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_field_types_encode_enum(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 sw_field_val,
    uint32 *hw_field_val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sw_field_val >= field_types_basic_info[unit][field_type]->nof_enum_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input value %d for enum field. max value %d\n",
                     sw_field_val, field_types_basic_info[unit][field_type]->nof_enum_values - 1);
    }
    *hw_field_val = field_types_basic_info[unit][field_type]->enum_val_info[sw_field_val].value;
    if (field_types_basic_info[unit][field_type]->enum_val_info[sw_field_val].is_invalid)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT;

}


static shr_error_e
dbal_field_types_decode_enum(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 hw_field_val,
    uint32 *sw_field_val)
{
    int ii;
    int nof_enum_vals = field_types_basic_info[unit][field_type]->nof_enum_values;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < nof_enum_vals; ii++)
    {
        if ((hw_field_val == field_types_basic_info[unit][field_type]->enum_val_info[ii].value) &&
            (!field_types_basic_info[unit][field_type]->enum_val_info[ii].is_invalid))
        {
            (*sw_field_val) = ii;
            break;
        }
    }
    if (ii == nof_enum_vals)
    {
        (*sw_field_val) = UTILEX_U32_MAX;
    }
    SHR_FUNC_EXIT;

}



shr_error_e
dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    (*field_type_info) = field_types_basic_info[unit][field_type];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_defs_e * field_type)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, field_type);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_nof_occupied_dynamic_fields_get(
    int unit,
    int *nof_occupied_fields)
{
    dbal_fields_e field_id;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    (*nof_occupied_fields) = 0;

    for (field_id = 0; field_id < DBAL_NOF_DYNAMIC_FIELDS; field_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, field_id, &field_type));
        if (field_type != DBAL_FIELD_TYPE_DEF_EMPTY)
        {
            (*nof_occupied_fields)++;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_available_field_id_get(
    int unit,
    dbal_fields_e * field_id)
{
    dbal_fields_e field_index;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    for (field_index = 0; field_index < DBAL_NOF_DYNAMIC_FIELDS; field_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, field_index, &field_type));
        if (field_type == DBAL_FIELD_TYPE_DEF_EMPTY)
        {
            (*field_id) = field_index;
            break;
        }
    }

    if (field_index == DBAL_NOF_DYNAMIC_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available field IDs\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_is_basic_type(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_basic_type)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    
    if ((field_type == DBAL_FIELD_TYPE_DEF_UINT) || (field_type == DBAL_FIELD_TYPE_DEF_UINT_INV) ||
        (field_type == DBAL_FIELD_TYPE_DEF_HEXA) || (field_type == DBAL_FIELD_TYPE_DEF_BOOL) ||
        (field_type == DBAL_FIELD_TYPE_DEF_BOOL_INV) || (field_type == DBAL_FIELD_TYPE_DEF_ARRAY32) ||
        (field_type == DBAL_FIELD_TYPE_DEF_ARRAY8) || (field_type == DBAL_FIELD_TYPE_DEF_BITMAP))

    {
        *is_basic_type = TRUE;
    }
    else
    {
        *is_basic_type = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_create(
    int unit,
    dbal_field_types_defs_e field_type,
    char field_name[DBAL_MAX_STRING_LENGTH],
    dbal_fields_e * field_id)
{
    int char_index;
    char eos = '\0';
    int field_name_length;
    char field_name_edited[DBAL_MAX_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_TYPE_ID_VALIDATE(field_type);

    SHR_IF_ERR_EXIT(dbal_fields_available_field_id_get(unit, field_id));

    
    if (field_name[0] == '\0')
    {
        sal_snprintf(field_name_edited, DBAL_MAX_STRING_LENGTH, "DYNAMIC_%d", (int) (*field_id));
    }
    else
    {
        dbal_fields_e existing_field_id;
        _shr_error_t rv;

        rv = dbal_field_string_to_id_no_error(unit, field_name, &existing_field_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "A field with the name \"%.*s\" already exists: %d. \n",
                         DBAL_MAX_STRING_LENGTH, field_name, existing_field_id);
        }
        sal_strncpy_s(field_name_edited, field_name, DBAL_MAX_STRING_LENGTH);
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.set(unit, (*field_id), field_type));
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_id.set(unit, (*field_id), (*field_id) + DBAL_NOF_FIELDS));

    field_name_length = sal_strnlen(field_name_edited, DBAL_MAX_STRING_LENGTH);
    for (char_index = 0; char_index < field_name_length; char_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.
                        field_name.set(unit, (*field_id), char_index, &field_name_edited[char_index]));
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_name.set(unit, (*field_id), char_index, &eos));
    (*field_id) += DBAL_NOF_FIELDS;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_destroy(
    int unit,
    dbal_fields_e field_id)
{
    int sw_state_field_index = field_id - DBAL_NOF_FIELDS;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);
    if (field_id < DBAL_NOF_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot destroy static fields\n");
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.set(unit, sw_state_field_index, DBAL_FIELD_TYPE_DEF_EMPTY));

exit:
    SHR_FUNC_EXIT;
}


uint8
dbal_fields_is_field_encoded(
    int unit,
    dbal_fields_e field_id)
{
    uint8 is_encoded = FALSE;
    int rv;
    dbal_field_types_defs_e field_type;

    rv = dbal_fields_field_type_get(unit, field_id, &field_type);

    if (rv)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "illegal field ID %d\n"), field_id));
    }
    switch (field_types_basic_info[unit][field_type]->encode_info.encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCTURE:
            is_encoded = FALSE;
            break;

        default:
            is_encoded = TRUE;
            break;
    }
    return is_encoded;
}


shr_error_e
dbal_fields_encode_prefix(
    int unit,
    uint32 *field_val,
    uint32 prefix_val,
    uint32 value_length,
    uint32 total_field_len,
    uint32 *field_val_out)
{
    int code_word_length;
    uint32 field_val_internal[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 prefix_val_as_arr[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (prefix_val == 0)
    {
        code_word_length = 0;
    }
    else
    {
        code_word_length = utilex_msb_bit_on(prefix_val) + 1;
    }
    if (code_word_length + value_length > total_field_len)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Sub field does not match to the parent field size in table. total_l = %d, sub_field_l = %d, prefix_val = %d\n",
                     total_field_len, value_length, prefix_val);
    }

    prefix_val_as_arr[0] = prefix_val;

    sal_memcpy(field_val_internal, field_val, WORDS2BYTES(BITS2WORDS(total_field_len)));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val_internal, 0, value_length, field_val_out));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(prefix_val_as_arr, value_length, code_word_length, field_val_out));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_encode_suffix(
    int unit,
    uint32 *field_val,
    uint32 suffix_val,
    uint32 field_length,
    uint32 total_field_len,
    uint32 *field_val_out)
{
    int suffix_length;
    SHR_FUNC_INIT_VARS(unit);

    if (suffix_val == 0)
    {
        suffix_length = 0;
    }
    else
    {
        suffix_length = utilex_msb_bit_on(suffix_val) + 1;
    }
    if (suffix_length + field_length > total_field_len)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Sub field does not match to the parent field size in table. total_l = %d, sub_field_l = %d, suffix_val = %d\n",
                     total_field_len, field_length, suffix_val);
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (field_val, total_field_len - field_length, field_length, field_val_out));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(&suffix_val, 0, suffix_length, field_val_out));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_decode_prefix(
    int unit,
    uint32 *field_val,
    uint32 value_length,
    uint32 *field_val_out)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val, 0, value_length, field_val_out));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_fields_decode_suffix(
    int unit,
    uint32 *field_val,
    uint32 total_field_len,
    uint32 field_length,
    uint32 *field_val_out)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (field_val, total_field_len - field_length, field_length, field_val_out));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_encode_decode_bitwise_not(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out)
{
    int ii;
    int nof_words, residual_bits;
    uint32 last_word_mask;

    SHR_FUNC_INIT_VARS(unit);

    nof_words = BITS2WORDS(max_nof_bits);
    residual_bits = max_nof_bits % 32;
    last_word_mask = (residual_bits == 0) ? UTILEX_U32_MAX : UTILEX_BITS_MASK(residual_bits - 1, 0);

    for (ii = 0; ii < nof_words - 1; ii++)
    {
        field_val_out[ii] = ~field_val[ii];
    }

    field_val_out[ii] = ~field_val[ii];
    field_val_out[ii] &= last_word_mask;

    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_encode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 max_nof_bits,
    uint32 *field_val_out)
{
    int msb_index, bits_to_copy, ii;
    uint32 field_val_as_arr[2] = { 0 };
                                      

    SHR_FUNC_INIT_VARS(unit);

    msb_index = max_nof_bits - 1;
    field_val_as_arr[0] = (*field_val);

    
    if (utilex_bitstream_test_bit(field_val_as_arr, msb_index) && max_nof_bits <= 32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MSB cannot be set");
    }

    
    utilex_bitstream_set_bit(field_val_out, 0);

    
    bits_to_copy = max_nof_bits - 1;
    ii = 0;
    while (bits_to_copy > 0)
    {
        int curr_bits_to_copy = bits_to_copy > 32 ? 32 : bits_to_copy;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                        (field_val_out, 1 + 32 * ii, curr_bits_to_copy, field_val_as_arr[ii]));
        bits_to_copy -= curr_bits_to_copy;
        ii++;
    }

exit:
    SHR_FUNC_EXIT;

}


shr_error_e
dbal_fields_decode_valid_ind(
    int unit,
    uint32 *field_val,
    uint32 curr_nof_bits,
    uint32 *field_val_out)
{

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(field_val, 1, curr_nof_bits - 1, field_val_out));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                    (unit, field_id, field_in_struct_id, field_in_struct_val, NULL, full_field_val, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_arr_prefix_decode(
    int unit,
    dbal_table_field_info_t * table_field,
    uint32 field_val,
    uint32 *ret_field_val,
    uint8 verbose)
{
    uint32 field_min_val = 0;
    uint32 field_max_val = 0;
    uint32 tmp_field_value = 0;
    uint32 field_val_as_arr[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    field_val_as_arr[0] = field_val;

    

    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field->arr_prefix,
                                              table_field->field_nof_bits,
                                              table_field->field_nof_bits + table_field->arr_prefix_size,
                                              &field_min_val));

    tmp_field_value = ((1 << table_field->field_nof_bits) - 1);
    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field->arr_prefix,
                                              table_field->field_nof_bits,
                                              table_field->field_nof_bits + table_field->arr_prefix_size,
                                              &field_max_val));

    if (ret_field_val[0] > field_max_val)
    {
        if (verbose)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field [%s] value %d Max value %d\n",
                         dbal_field_to_string(unit, table_field->field_id), ret_field_val[0], field_max_val);
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            SHR_EXIT();
        }
    }

    if (ret_field_val[0] < field_min_val)
    {
        if (verbose)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field [%s] value %d Min value %d\n",
                         dbal_field_to_string(unit, table_field->field_id), ret_field_val[0], field_min_val);
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            SHR_EXIT();
        }
    }

    ret_field_val[0] = 0;
    
    SHR_IF_ERR_EXIT(dbal_fields_decode_prefix(unit, field_val_as_arr, table_field->field_nof_bits, ret_field_val));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_fields_struct_field_arr_prefix_decode(
    int unit,
    dbal_sub_struct_field_info_t * struct_field_info,
    uint32 field_val)
{
    dbal_table_field_info_t table_field;
    uint32 ret_field_val;

    SHR_FUNC_INIT_VARS(unit);

    table_field.field_id = struct_field_info->struct_field_id;
    table_field.arr_prefix = struct_field_info->arr_prefix;
    table_field.arr_prefix_size = struct_field_info->arr_prefix_size;
    table_field.field_nof_bits = struct_field_info->length;

    ret_field_val = field_val;
    SHR_IF_ERR_EXIT(dbal_field_arr_prefix_decode(unit, &table_field, field_val, &ret_field_val, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_encode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask)
{
    int ii;
    int offset = 0;
    int length = 0;
    int nof_struct_fields;
    uint8 is_sub_field_found = 0;
    dbal_field_types_defs_e field_type;
    dbal_sub_struct_field_info_t *struct_field_info = NULL;
    uint32 *field_val_to_add = field_in_struct_val;
    uint32 field_mask_to_add[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    uint32 encoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    nof_struct_fields = field_types_basic_info[unit][field_type]->nof_struct_fields;
    for (ii = 0; ii < nof_struct_fields; ii++)
    {
        struct_field_info = &(field_types_basic_info[unit][field_type]->struct_field_info[ii]);
        if (struct_field_info->struct_field_id == field_in_struct_id)
        {
            break;
        }
        
        SHR_IF_ERR_EXIT(dbal_fields_sub_field_match
                        (unit, struct_field_info->struct_field_id, field_in_struct_id, &is_sub_field_found));
        if (is_sub_field_found)
        {
            parent_field_id = struct_field_info->struct_field_id;
            break;
        }
    }

    if (ii == nof_struct_fields)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_id %s %d is not a field in field_id %s structure\n",
                     dbal_field_to_string(unit, field_id), field_id, dbal_field_to_string(unit, field_in_struct_id));
    }

    offset = field_types_basic_info[unit][field_type]->struct_field_info[ii].offset;
    length = field_types_basic_info[unit][field_type]->struct_field_info[ii].length;

    if (is_sub_field_found || dbal_fields_is_field_encoded(unit, field_in_struct_id))
    {
        int ret_val;
        ret_val = dbal_fields_field_encode(unit, field_in_struct_id, parent_field_id, struct_field_info->length,
                                           field_in_struct_val, encoded_field_value);
        ret_val = dbal_field_encode_handle_rv(unit, NULL, 0, field_in_struct_id, ret_val);
        SHR_IF_ERR_EXIT(ret_val);

        field_val_to_add = encoded_field_value;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "\n***** ENCODED structure %s before = %d after = %d field_in_struct_id = %s ****\n"),
                     dbal_field_to_string(unit, field_id), field_in_struct_val[0], encoded_field_value[0],
                     dbal_field_to_string(unit, field_in_struct_id)));
    }

    if (field_types_basic_info[unit][field_type]->struct_field_info[ii].arr_prefix_size > 0)
    {
        
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_arr_prefix_decode(unit, struct_field_info, *field_in_struct_val));
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val_to_add, offset, length, full_field_val));

    if (field_in_struct_mask)
    {
        dbal_field_types_basic_info_t *field_type_info;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_in_struct_mask, 0, length, field_mask_to_add));
        if (parent_field_id != DBAL_FIELD_EMPTY)
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_in_struct_id, &field_type_info));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(field_mask_to_add, field_type_info->max_size, (length - 1)));
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_mask_to_add, offset, length, full_field_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *full_field_val)
{
    uint32 internal_field_in_struct_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode_masked
                    (unit, field_id, field_in_struct_id, internal_field_in_struct_val, NULL, full_field_val, NULL, 1));

    *field_in_struct_val = internal_field_in_struct_val[0];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_struct_field_decode_masked(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id,
    uint32 *field_in_struct_val,
    uint32 *field_in_struct_mask,
    uint32 *full_field_val,
    uint32 *full_field_mask,
    int is_uint32)
{
    int ii;
    int offset = 0;
    int length = 0;
    int full_length = 0;
    int arr_prefix_size = 0;
    int arr_prefix_value = 0;
    int nof_struct_fields;
    uint8 is_sub_field_found = 0;
    dbal_field_types_defs_e field_type;
    dbal_sub_struct_field_info_t *struct_field_info;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    uint32 decoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if ((field_id < DBAL_NOF_FIELDS) && (field_in_struct_id < DBAL_NOF_FIELDS))
    {
        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));
        field_in_struct_val[0] = 0;
        nof_struct_fields = field_types_basic_info[unit][field_type]->nof_struct_fields;
        for (ii = 0; ii < nof_struct_fields; ii++)
        {
            struct_field_info = &(field_types_basic_info[unit][field_type]->struct_field_info[ii]);
            if ((field_types_basic_info[unit][field_type])->struct_field_info[ii].struct_field_id == field_in_struct_id)
            {
                break;
            }
            
            SHR_IF_ERR_EXIT(dbal_fields_sub_field_match
                            (unit, struct_field_info->struct_field_id, field_in_struct_id, &is_sub_field_found));
            if (is_sub_field_found)
            {
                parent_field_id = struct_field_info->struct_field_id;
                break;
            }
        }
        if (ii == nof_struct_fields)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_id %d is not a struct field of field_id %d\n", field_id,
                         field_in_struct_id);
        }
        offset = field_types_basic_info[unit][field_type]->struct_field_info[ii].offset;
        length = field_types_basic_info[unit][field_type]->struct_field_info[ii].length;
        arr_prefix_size = field_types_basic_info[unit][field_type]->struct_field_info[ii].arr_prefix_size;
        arr_prefix_value = field_types_basic_info[unit][field_type]->struct_field_info[ii].arr_prefix;
        full_length = length + arr_prefix_size;
        if (is_uint32)
        {
            if (full_length > 32)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "field %s size %d is not supported, max size 32 %s\n",
                             dbal_field_to_string(unit, field_in_struct_id), field_in_struct_id,
                             field_types_basic_info[unit][field_type]->name);
            }
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(full_field_val, offset, length, field_in_struct_val));

        if (is_sub_field_found || dbal_fields_is_field_encoded(unit, field_in_struct_id))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, field_in_struct_id, parent_field_id, 0,
                                                     field_in_struct_val, decoded_field_value));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit, "\n***** DECODED structure %s before = %d after = %d field_in_struct_id = %s ****\n"),
                         dbal_field_to_string(unit, field_id), field_in_struct_val[0], decoded_field_value[0],
                         dbal_field_to_string(unit, field_in_struct_id)));
            field_in_struct_val[0] = 0;
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (decoded_field_value, 0, BYTES2BITS(BITS2BYTES(length)), field_in_struct_val));
        }

        if (arr_prefix_size > 0)
        {
            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix
                            (unit, field_in_struct_val, arr_prefix_value, length, full_length, decoded_field_value));
            field_in_struct_val[0] = 0;
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (decoded_field_value, 0, BYTES2BITS(BITS2BYTES(full_length)), field_in_struct_val));
        }

        if (field_in_struct_mask)
        {
            dbal_field_types_basic_info_t *field_type_info;

            if (parent_field_id != DBAL_FIELD_EMPTY)
            {
                SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_in_struct_id, &field_type_info));
                length = field_type_info->max_size;
            }
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(full_field_mask, offset, length, field_in_struct_mask));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_id %d or %d, DBAL_NOF_FIELD_TYPES_DEF=%d\n", field_id,
                     field_in_struct_id, DBAL_NOF_FIELDS);
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
dbal_fields_is_allocator_get(
    int unit,
    dbal_fields_e field_id,
    uint8 *is_allocator)
{
    dbal_field_types_defs_e field_type;
    dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    field_type_info = field_types_basic_info[unit][field_type];

    (*is_allocator) = SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_encode_handle_rv(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int is_key,
    dbal_fields_e field_id,
    shr_error_e ret_val)
{
    dbal_status_e dbal_status;

    SHR_FUNC_INIT_VARS(unit);

    if (ret_val == _SHR_E_UNAVAIL)
    {
     

        SHR_IF_ERR_EXIT(dbal_status_get(unit, &dbal_status));

        if (dbal_image_name_is_std_1(unit))
        {
            
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        }
        else if (dbal_status == DBAL_STATUS_DEVICE_INIT_DONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit, "%s field %s value used 0 since no mapping in image/device for value added\n\n"),
                       is_key ? "key" : "result", dbal_field_to_string(unit, field_id)));
        }
        else
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "%s field %s value used 0 since no mapping in image/device for value added\n\n"),
                      is_key ? "key" : "result", dbal_field_to_string(unit, field_id)));
        }

        if (entry_handle && is_key)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ret_val);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_encode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out)
{
    dbal_field_types_defs_e field_type;
    dbal_value_field_encode_types_e encode_mode;
    uint32 in_param;
    int sub_field_size = field_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    encode_mode = field_types_basic_info[unit][field_type]->encode_info.encode_mode;
    in_param = field_types_basic_info[unit][field_type]->encode_info.input_param;

    field_value_out[0] = 0;
    
    if (parent_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_sub_field_info_t sub_field_info;
        int rv;

        rv = dbal_fields_sub_field_info_get_internal(unit, field_id, parent_field_id, &sub_field_info);
        if (rv)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find child field. child %s, parent %s\n",
                         dbal_field_to_string(unit, field_id), dbal_field_to_string(unit, parent_field_id));
        }
        encode_mode = sub_field_info.encode_info.encode_mode;
        in_param = sub_field_info.encode_info.input_param;
        sub_field_size = field_types_basic_info[unit][field_type]->max_size;
    }

    switch (encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_PREFIX:
            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix
                            (unit, field_val, in_param, sub_field_size, field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_VALID_IND:
            SHR_IF_ERR_EXIT(dbal_fields_encode_valid_ind(unit, field_val, field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
            SHR_IF_ERR_EXIT(dbal_fields_encode_decode_bitwise_not(unit, field_val, field_size, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_ENUM:
            SHR_IF_ERR_EXIT_NO_MSG(dbal_field_types_encode_enum(unit, field_type, *field_val, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCTURE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Code should not get here. encode mode %s, field_id %s\n",
                         dbal_field_encode_type_to_string(unit, encode_mode), dbal_field_to_string(unit, field_id));
            break;

        case DBAL_VALUE_FIELD_ENCODE_SUFFIX:
            SHR_IF_ERR_EXIT(dbal_fields_encode_suffix
                            (unit, field_val, in_param, sub_field_size, field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLY:
        case DBAL_VALUE_FIELD_ENCODE_HARD_VALUE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported yet\n",
                         dbal_field_encode_type_to_string(unit, encode_mode));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encode type (%d) for field %s\n",
                         encode_mode, dbal_field_to_string(unit, field_id));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_parent_encode_custom_val(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    uint32 encode_mask_val,
    uint32 *field_mask_val,
    uint32 *parent_field_full_mask)
{
    dbal_field_types_defs_e parent_field_type;
    dbal_field_types_defs_e field_type;
    dbal_value_field_encode_types_e encode_mode;
    int sub_field_size;
    int field_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, parent_field_id, &parent_field_type));

    encode_mode = field_types_basic_info[unit][field_type]->encode_info.encode_mode;

    field_size = field_types_basic_info[unit][parent_field_type]->max_size;

    parent_field_full_mask[0] = 0;
    
    if (parent_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_sub_field_info_t sub_field_info;
        int rv;

        rv = dbal_fields_sub_field_info_get_internal(unit, field_id, parent_field_id, &sub_field_info);
        if (rv)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find child field. child %s, parent %s\n",
                         dbal_field_to_string(unit, field_id), dbal_field_to_string(unit, parent_field_id));
        }
        encode_mode = sub_field_info.encode_info.encode_mode;
        sub_field_size = field_types_basic_info[unit][field_type]->max_size;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Valid parent field is mandatory for this function\n");
    }

    switch (encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_PREFIX:
            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix
                            (unit, field_mask_val, encode_mask_val, sub_field_size, field_size,
                             parent_field_full_mask));
            break;

        case DBAL_VALUE_FIELD_ENCODE_SUFFIX:
            SHR_IF_ERR_EXIT(dbal_fields_encode_suffix
                            (unit, field_mask_val, encode_mask_val, sub_field_size, field_size,
                             parent_field_full_mask));
            break;
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLY:
        case DBAL_VALUE_FIELD_ENCODE_HARD_VALUE:
        case DBAL_VALUE_FIELD_ENCODE_VALID_IND:
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
        case DBAL_VALUE_FIELD_ENCODE_ENUM:
        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCTURE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported for encode value set\n",
                         dbal_field_encode_type_to_string(unit, encode_mode));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encode type (%d) for field %s\n",
                         encode_mode, dbal_field_to_string(unit, field_id));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_field_decode(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    int field_size,
    uint32 *field_val,
    uint32 *field_value_out)
{
    dbal_field_types_defs_e field_type;
    dbal_value_field_encode_types_e encode_mode;
    int sub_field_size = field_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    encode_mode = field_types_basic_info[unit][field_type]->encode_info.encode_mode;
    field_value_out[0] = 0;

    
    if (parent_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_sub_field_info_t sub_field_info;
        int rv;
        rv = dbal_fields_sub_field_info_get_internal(unit, field_id, parent_field_id, &sub_field_info);
        if (rv)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find child field. child %s, parent %s\n",
                         dbal_field_to_string(unit, field_id), dbal_field_to_string(unit, parent_field_id));
        }
        encode_mode = sub_field_info.encode_info.encode_mode;
        sub_field_size = field_types_basic_info[unit][field_type]->max_size;
    }

    switch (encode_mode)
    {
        case DBAL_VALUE_FIELD_ENCODE_PREFIX:
            SHR_IF_ERR_EXIT(dbal_fields_decode_prefix(unit, field_val, sub_field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_VALID_IND:
            SHR_IF_ERR_EXIT(dbal_fields_decode_valid_ind(unit, field_val, sub_field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT:
            SHR_IF_ERR_EXIT(dbal_fields_encode_decode_bitwise_not(unit, field_val, sub_field_size, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_ENUM:
            SHR_IF_ERR_EXIT(dbal_field_types_decode_enum(unit, field_type, *field_val, field_value_out));
            break;

        case DBAL_VALUE_FIELD_ENCODE_NONE:
        case DBAL_VALUE_FIELD_ENCODE_STRUCTURE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Code should not get here. encode mode %s, field_id %s\n",
                         dbal_field_encode_type_to_string(unit, encode_mode), dbal_field_to_string(unit, field_id));
            break;

        case DBAL_VALUE_FIELD_ENCODE_SUFFIX:
            SHR_IF_ERR_EXIT(dbal_fields_decode_suffix(unit, field_val, field_size, sub_field_size, field_value_out));
            break;
        case DBAL_VALUE_FIELD_ENCODE_ADD:
        case DBAL_VALUE_FIELD_ENCODE_SUBTRACT:
        case DBAL_VALUE_FIELD_ENCODE_MULTIPLY:
        case DBAL_VALUE_FIELD_ENCODE_HARD_VALUE:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Encode type %s is not supported yet\n",
                         dbal_field_encode_type_to_string(unit, encode_mode));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal encode type (%d) for field %s\n",
                         encode_mode, dbal_field_to_string(unit, field_id));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_of_same_instance(
    int unit,
    dbal_fields_e field_id_1,
    dbal_fields_e field_id_2,
    uint8 *is_same_instance)
{
    dbal_field_types_defs_e field_type_1;
    dbal_field_types_defs_e field_type_2;

    SHR_FUNC_INIT_VARS(unit);

    *is_same_instance = FALSE;

    DBAL_FIELD_ID_VALIDATE(field_id_1);
    DBAL_FIELD_ID_VALIDATE(field_id_2);

    GET_FIELD_TYPE(field_id_1, &field_type_1);
    GET_FIELD_TYPE(field_id_2, &field_type_2);

    if (field_id_1 == field_id_2)
    {
        *is_same_instance = TRUE;
    }

    
    if (field_type_1 == field_type_2)
    {
        CONST char *tmp_string;
        int field_ids_diff;
        int field_prefix_diff;
        int field_1_strlen = 0, field_2_strlen = 0;
        int field_1_prefix_num = 0, field_2_prefix_num = 0;
        uint8 field_1_after = field_id_1 > field_id_2 ? TRUE : FALSE;

        tmp_string = dbal_field_to_string(unit, field_id_1);
        field_1_strlen = sal_strlen(tmp_string);
        if (field_1_strlen < 3)
        {
            
            SHR_EXIT();
        }
        field_1_prefix_num = sal_atoi(&tmp_string[field_1_strlen - 3]);

        tmp_string = dbal_field_to_string(unit, field_id_2);
        field_2_strlen = sal_strlen(tmp_string);
        if (field_2_strlen < 3)
        {
            
            SHR_EXIT();
        }
        field_2_prefix_num = sal_atoi(&tmp_string[field_2_strlen - 3]);

        field_ids_diff = (field_1_after) ? (field_id_1 - field_id_2) : (field_id_2 - field_id_1);
        field_prefix_diff = (field_1_after) ? (field_1_prefix_num - field_2_prefix_num) :
            (field_2_prefix_num - field_1_prefix_num);

        if (field_prefix_diff == field_ids_diff)
        {
            *is_same_instance = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_predefine_value_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int is_key,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value)
{
    dbal_table_field_info_t table_field_info;
    dbal_entry_handle_t *default_handle = NULL;
    dbal_field_types_basic_info_t *field_type_info;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_type_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id, field_id, is_key,
                                               entry_handle->cur_res_type, 0, &table_field_info));

    switch (value_type)
    {
        case DBAL_PREDEF_VAL_MIN_VALUE:
            (*predef_value) = table_field_info.min_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));

                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Min value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }
            }
            break;

        case DBAL_PREDEF_VAL_MAX_VALUE:
            if (table_field_info.field_nof_bits <= UTILEX_NOF_BITS_IN_UINT32)
            {
                (*predef_value) = table_field_info.max_value;
                if (table_field_info.arr_prefix_size != 0)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                              table_field_info.field_nof_bits,
                                                              table_field_info.field_nof_bits +
                                                              table_field_info.arr_prefix_size, predef_value));

                    if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Max value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                     dbal_field_to_string(unit, table_field_info.field_id));
                    }
                }
            }
            else
            {
                uint8 word_offset = table_field_info.field_nof_bits / UTILEX_NOF_BITS_IN_UINT32;
                uint8 bits_offset = table_field_info.field_nof_bits % UTILEX_NOF_BITS_IN_UINT32;
                uint8 word_index;

                for (word_index = 0; word_index < word_offset; word_index++)
                {
                    predef_value[word_index] = UTILEX_U32_MAX;
                }
                if ((word_offset < DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS) && (bits_offset != 0))
                {
                    predef_value[word_offset] = (1 << bits_offset) - 1;
                }
            }
            break;

        case DBAL_PREDEF_VAL_DEFAULT_VALUE:
            if (SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID))
            {
                (*predef_value) = field_type_info->default_value;
            }
            else if (!is_key && (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
                     && entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].is_default_non_standard)
            {
                if (entry_handle->nof_key_fields != entry_handle->table->nof_key_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot set the default value of the entry in case not all key fields"
                                 " were set, table %s field %s\n", entry_handle->table->table_name,
                                 dbal_field_to_string(unit, field_id));
                }

                SHR_ALLOC_SET_ZERO(default_handle, sizeof(dbal_entry_handle_t), "Handle Alloc", "%s%s%s\r\n", EMPTY,
                                   EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dbal_hl_entry_default_entry_build(unit, entry_handle, default_handle));

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, field_id,
                                                           default_handle->phy_entry.payload, predef_value));
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "non default value set field %s %d \n"),
                                             dbal_field_to_string(unit, field_id), predef_value[0]));
            }
            else
            {
                if (field_type_info->max_size <= 32)
                {
                    if (SHR_BITGET(table_field_info.field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
                    {
                        *predef_value = table_field_info.const_value;
                    }
                    else
                    {
                        (*predef_value) = 0;
                        if (table_field_info.arr_prefix_size != 0)
                        {
                            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                                      table_field_info.field_nof_bits,
                                                                      table_field_info.field_nof_bits +
                                                                      table_field_info.arr_prefix_size, predef_value));

                            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                            {
                                int ii;
                                for (ii = 0; ii < field_type_info->nof_enum_values; ii++)
                                {
                                    if ((*predef_value) == field_type_info->enum_val_info[ii].value &&
                                        !(field_type_info->enum_val_info[ii].is_invalid))
                                    {
                                        (*predef_value) = ii;
                                        break;
                                    }
                                }

                                if (ii == field_type_info->nof_enum_values)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                                 "Enum with ARR prefix, default value couldn't be resolved. field: %s",
                                                 dbal_field_to_string(unit, table_field_info.field_id));
                                }
                            }
                        }
                    }
                }
                else
                {
                    sal_memcpy(predef_value, zero_buffer_to_compare, BITS2BYTES(field_type_info->max_size));
                }
            }
            break;

        case DBAL_PREDEF_VAL_RESET_VALUE:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, DBAL_PREDEF_VAL_RESET_VALUE is not supported \n");
            break;

        default:
            
            break;
    }

exit:
    SHR_FREE(default_handle);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_sub_field_match(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint8 *is_found)
{
    dbal_sub_field_info_t sub_field_info;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    (*is_found) = 0;

    rv = dbal_fields_sub_field_info_get_internal(unit, sub_field_id, parent_field_id, &sub_field_info);
    if (!rv)
    {
        (*is_found) = 1;
    }
    else
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }

    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_max_size_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *field_size)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*field_size) = FIELD_TYPE_INFO(field_type)->max_size;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_max_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *max_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*max_value) = FIELD_TYPE_INFO(field_type)->max_value;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_min_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *min_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*min_value) = FIELD_TYPE_INFO(field_type)->min_value;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_const_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *const_value,
    uint8 *const_value_valid)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    *const_value_valid = SHR_BITGET(FIELD_TYPE_INFO(field_type)->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID);
    *const_value = FIELD_TYPE_INFO(field_type)->const_value;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_has_illegal_values(
    int unit,
    dbal_fields_e field_id,
    uint8 *has_illegals)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    (*has_illegals) = FALSE;

    DBAL_FIELD_ID_VALIDATE(field_id);
    GET_FIELD_TYPE(field_id, &field_type);

    if (FIELD_TYPE_INFO(field_type)->nof_illegal_values > 0)
    {
        *has_illegals = TRUE;
        SHR_EXIT();
    }
    if (FIELD_TYPE_INFO(field_type)->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        if (FIELD_TYPE_INFO(field_type)->nof_invalid_enum_values)
        {
            *has_illegals = TRUE;
            SHR_EXIT();
        }
    }
    if (FIELD_TYPE_INFO(field_type)->illegal_value_cb)
    {
        *has_illegals = TRUE;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_fields_is_illegal_value(
    int unit,
    dbal_fields_e field_id,
    uint32 value,
    uint8 *is_illegal)
{
    dbal_field_types_defs_e field_type;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    *is_illegal = FALSE;

    GET_FIELD_TYPE(field_id, &field_type);
    for (ii = 0; ii < FIELD_TYPE_INFO(field_type)->nof_illegal_values; ii++)
    {
        if (value == FIELD_TYPE_INFO(field_type)->illegal_values[ii])
        {
            *is_illegal = TRUE;
            SHR_EXIT();
        }
    }

    if (FIELD_TYPE_INFO(field_type)->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        if (value > FIELD_TYPE_INFO(field_type)->nof_enum_values)
        {
            *is_illegal = TRUE;
            SHR_EXIT();
        }
        if (FIELD_TYPE_INFO(field_type)->enum_val_info[value].is_invalid)
        {
            *is_illegal = TRUE;
            SHR_EXIT();
        }
    }

    if (FIELD_TYPE_INFO(field_type)->illegal_value_cb)
    {
        SHR_IF_ERR_EXIT(FIELD_TYPE_INFO(field_type)->illegal_value_cb(unit, value, is_illegal));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_print_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_print_type_e * field_print_type)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*field_print_type) = FIELD_TYPE_INFO(field_type)->print_type;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_encode_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_value_field_encode_types_e * encode_type)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    (*encode_type) = FIELD_TYPE_INFO(field_type)->encode_info.encode_mode;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
dbal_fields_uint32_sub_field_info_get(
    int unit,
    dbal_fields_e field_id,
    uint32 orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val)
{
    dbal_field_types_defs_e field_type;
    uint32 orig_value_arr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 sub_field_val_arr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    if (field_types_basic_info[unit][field_type]->max_size > 32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, this API support only 32 bit parent fields, field %s is %d bits \n",
                     dbal_field_to_string(unit, (field_id)), field_types_basic_info[unit][field_type]->max_size);
    }

    orig_value_arr[0] = orig_val;

    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, orig_value_arr, sub_field_id, sub_field_val_arr));

    (*sub_field_val) = sub_field_val_arr[0];

exit:
    SHR_FUNC_EXIT;
}

int
dbal_fields_is_parent_field(
    int unit,
    dbal_fields_e field_id)
{
    dbal_field_types_defs_e field_type;

    if ((field_id) < DBAL_NOF_FIELDS)
    {
        field_type = dbal_field_id_to_type[(field_id)];
    }
    else
    {
        DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, (field_id - DBAL_NOF_FIELDS), (&field_type));
    }

    if (field_types_basic_info[unit][field_type]->nof_child_fields)
    {
        return 1;
    }

    return 0;
}


shr_error_e
dbal_fields_sub_field_info_get_internal(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    dbal_sub_field_info_t * sub_field_info)
{
    int iter;
    dbal_field_types_defs_e parent_type;

    SHR_FUNC_INIT_VARS(unit);

    GET_FIELD_TYPE(parent_field_id, &parent_type);

    for (iter = 0; iter < field_types_basic_info[unit][parent_type]->nof_child_fields; iter++)
    {
        dbal_fields_e sub_field_id = field_types_basic_info[unit][parent_type]->sub_field_info[iter].sub_field_id;
        if (sub_field_id == field_id)
        {
            (*sub_field_info) = (field_types_basic_info[unit][parent_type]->sub_field_info[iter]);
            break;
        }
    }

    if (iter == field_types_basic_info[unit][parent_type]->nof_child_fields)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_parent_field_decode(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val)
{
    int iter;
    uint32 decoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 re_encoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_field_types_defs_e field_type, sub_field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    (*sub_field_id) = DBAL_FIELD_EMPTY;
    (*sub_field_val) = 0;

    if (field_types_basic_info[unit][field_type]->nof_child_fields == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "field %s has no child fields\n", dbal_field_to_string(unit, field_id));
    }

    
    for (iter = 0; iter < field_types_basic_info[unit][field_type]->nof_child_fields; iter++)
    {
        dbal_fields_e curr_sub_field_id = field_types_basic_info[unit][field_type]->sub_field_info[iter].sub_field_id;
        if (curr_sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_memset(decoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            sal_memset(re_encoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, curr_sub_field_id, field_id,
                                                     field_types_basic_info[unit][field_type]->max_size, orig_val,
                                                     decoded_value));
            SHR_IF_ERR_EXIT(dbal_fields_field_encode
                            (unit, curr_sub_field_id, field_id, field_types_basic_info[unit][field_type]->max_size,
                             decoded_value, re_encoded_value));
            if (sal_memcmp
                (re_encoded_value, orig_val,
                 WORDS2BYTES(BITS2WORDS(field_types_basic_info[unit][field_type]->max_size))) == 0)
            {
                int size_to_copy;
                GET_FIELD_TYPE(curr_sub_field_id, &sub_field_type);
                (*sub_field_id) = curr_sub_field_id;
                size_to_copy = BYTES2BITS(BITS2BYTES(field_types_basic_info[unit][sub_field_type]->max_size));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(decoded_value, 0, size_to_copy, sub_field_val));
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_parent_field_decode_no_err(
    int unit,
    dbal_fields_e field_id,
    uint32 *orig_val,
    dbal_fields_e * sub_field_id,
    uint32 *sub_field_val)
{
    int iter;
    uint32 decoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 re_encoded_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_field_types_defs_e field_type, sub_field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);

    (*sub_field_id) = DBAL_FIELD_EMPTY;

    if (field_types_basic_info[unit][field_type]->nof_child_fields == 0)
    {
        SHR_EXIT();
    }

    
    for (iter = 0; iter < field_types_basic_info[unit][field_type]->nof_child_fields; iter++)
    {
        dbal_fields_e curr_sub_field_id = field_types_basic_info[unit][field_type]->sub_field_info[iter].sub_field_id;
        if (curr_sub_field_id != DBAL_FIELD_EMPTY)
        {
            sal_memset(decoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            sal_memset(re_encoded_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
            SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, curr_sub_field_id, field_id,
                                                     field_types_basic_info[unit][field_type]->max_size, orig_val,
                                                     decoded_value));
            SHR_IF_ERR_EXIT(dbal_fields_field_encode
                            (unit, curr_sub_field_id, field_id, field_types_basic_info[unit][field_type]->max_size,
                             decoded_value, re_encoded_value));
            if (sal_memcmp
                (re_encoded_value, orig_val,
                 WORDS2BYTES(BITS2WORDS(field_types_basic_info[unit][field_type]->max_size))) == 0)
            {
                int size_to_copy;
                GET_FIELD_TYPE(curr_sub_field_id, &sub_field_type);
                (*sub_field_id) = curr_sub_field_id;
                size_to_copy = BYTES2BITS(BITS2BYTES(field_types_basic_info[unit][sub_field_type]->max_size));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(decoded_value, 0, size_to_copy, sub_field_val));
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

uint32
dbal_fields_instance_idx_get(
    int unit,
    dbal_fields_e field_id)
{
    char field_name[DBAL_MAX_STRING_LENGTH];
    int name_len;
    int inst_idx;

    sal_strncpy(field_name, dbal_field_to_string(unit, field_id), DBAL_MAX_STRING_LENGTH - 1);
    field_name[DBAL_MAX_STRING_LENGTH - 1] = '\0';
    name_len = sal_strlen(field_name);

    if ((field_name[name_len - 4] == '_') && (field_name[name_len - 3] >= '0') && (field_name[name_len - 3] <= '9') &&
        (field_name[name_len - 2] >= '0') && (field_name[name_len - 2] <= '9') && (field_name[name_len - 1] >= '0') &&
        (field_name[name_len - 1] <= '9'))
    {
        inst_idx =
            (field_name[name_len - 3] - '0') * 100 + (field_name[name_len - 2] - '0') * 10 + (field_name[name_len - 1] -
                                                                                              '0');
    }
    else
    {
        inst_idx = 0;
    }

    return inst_idx;
}

shr_error_e
dbal_fields_parent_field32_value_set(
    int unit,
    dbal_fields_e parent_field_id,
    dbal_fields_e sub_field_id,
    uint32 *sub_field_val,
    uint32 *parent_field_val)
{
    dbal_field_types_defs_e field_type;
    uint32 parent_field_val_arr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 }; 

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(sub_field_id);

    GET_FIELD_TYPE(parent_field_id, &field_type);

    if (FIELD_TYPE_INFO(field_type)->max_size > 32)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "API supported for fields lower than 32bit\n")));
    }
    SHR_IF_ERR_EXIT(dbal_fields_field_encode(unit, sub_field_id, parent_field_id,
                                             FIELD_TYPE_INFO(field_type)->max_size, sub_field_val,
                                             parent_field_val_arr));

    (*parent_field_val) = parent_field_val_arr[0];
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_enum_hw_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 enum_value,
    uint32 *hw_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    if (enum_value >= FIELD_TYPE_INFO(field_type)->nof_enum_values)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input value for enum field %s. max value %d\n",
                     FIELD_TYPE_INFO(field_type)->name, FIELD_TYPE_INFO(field_type)->nof_enum_values);
    }
    if (FIELD_TYPE_INFO(field_type)->enum_val_info[enum_value].is_invalid)
    {
        dbal_status_e dbal_status;
        SHR_IF_ERR_EXIT(dbal_status_get(unit, &dbal_status));
        if ((dbal_status == DBAL_STATUS_DEVICE_INIT_DONE) || dbal_image_name_is_std_1(unit))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Illegal input value for enum field %s. Value %d not available for device.\n",
                         FIELD_TYPE_INFO(field_type)->name, enum_value);
        }
        else
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Illegal input value for enum field %s. Value %d not available for device.\n"),
                      FIELD_TYPE_INFO(field_type)->name, enum_value));
        }
    }

    *hw_value = FIELD_TYPE_INFO(field_type)->enum_val_info[enum_value].value;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_enum_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 hw_value,
    uint32 *enum_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_FIELD_ID_VALIDATE(field_id);

    GET_FIELD_TYPE(field_id, &field_type);
    SHR_IF_ERR_EXIT(dbal_field_types_decode_enum(unit, field_type, hw_value, enum_value));

exit:
    SHR_FUNC_EXIT;
}
