

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA


#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/pbmp.h>

#include <sal/appl/sal.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/dnxc/dnxc_data/dnxc_data_dyn_loader.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
#if defined(BCM_DNX_SUPPORT)
#include <include/soc/dnx/utils/bare_metal_dynamic_loading.h>
#endif
#include <bcm/types.h>





static shr_error_e dnxc_data_dyn_loader_sku_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);


static shr_error_e dnxc_data_dyn_loader_internal_sku_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);


static shr_error_e dnxc_data_dyn_loader_sku_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);


static shr_error_e dnxc_data_dyn_loader_internal_sku_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);

#ifdef ADAPTER_SERVER_MODE

static shr_error_e dnxc_data_dyn_loader_adapter_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE]);
#endif 






#define DNXC_DATA_DYN_XML_DB_PATH "data/"
#define DNXC_DATA_DYN_XML_DB_PATH_INTERNAL "data/internal/"

#ifdef ADAPTER_SERVER_MODE
#define DNXC_DATA_DYN_ADAPTER_XML_DB_PATH "data/adapter/"
#endif 


#if defined(BCM_DNX_SUPPORT)
static dnxc_data_dyn_file_path_get_f dnx_data_dyn_files[] = {
    dnxc_data_dyn_loader_sku_file_path_get,
    dnxc_data_dyn_loader_internal_sku_file_path_get,
    dnxc_data_dyn_loader_sku_revision_file_path_get,
    dnxc_data_dyn_loader_internal_sku_revision_file_path_get,
#ifdef ADAPTER_SERVER_MODE
    dnxc_data_dyn_loader_adapter_revision_file_path_get,
#endif 
    
    dnx_arr_prefix_dynamic_file_get,
    dnx_aod_tables_dynamic_file_get,
    dnx_mdb_app_db_dynamic_file_get,
    dnx_pemla_soc_dynamic_file_get,
    dnx_aod_sizes_dynamic_file_get
};
#endif


#if defined(BCM_DNXF_SUPPORT)
static dnxc_data_dyn_file_path_get_f dnxf_data_dyn_files[] = {
    dnxc_data_dyn_loader_sku_file_path_get,
    dnxc_data_dyn_loader_internal_sku_file_path_get,
    dnxc_data_dyn_loader_sku_revision_file_path_get,
    dnxc_data_dyn_loader_internal_sku_revision_file_path_get
};
#endif


typedef struct dnxc_data_dyn_str_linked_list_s dnxc_data_dyn_str_linked_list_s;
typedef struct dnxc_data_dyn_str_linked_list_s
{
    dnxc_data_dyn_str_linked_list_s *next;
    char str[RHSTRING_MAX_SIZE];
} dnxc_data_dyn_str_linked_list_t;


static dnxc_data_dyn_str_linked_list_t *dnxc_data_dyn_str_handles_head[SOC_MAX_NUM_DEVICES] = { NULL };
static dnxc_data_dyn_str_linked_list_t *dnxc_data_dyn_str_handles_tail[SOC_MAX_NUM_DEVICES] = { NULL };

typedef struct
{
    
    rhentry_t entry;

    
    xml_node file_top;
} dnxc_data_dyn_file_top_info_t;


rhlist_t *dnxc_data_dyn_files_top_list[SOC_MAX_NUM_DEVICES] = { NULL };


typedef enum
{
  
    dnxc_data_dyn_table_data_type_pbmp = 0,

  
    dnxc_data_dyn_table_data_type_int = 1,

  
    dnxc_data_dyn_table_data_type_uint32 = 2,

    
    dnxc_data_dyn_table_data_type_int_array = 3,

    
    dnxc_data_dyn_table_data_type_uint32_array = 4,

    
    dnxc_data_dyn_table_data_type_uint8 = 5,
    
    dnxc_data_dyn_table_data_type_string = 6
} dnxc_data_dyn_table_data_type;






static shr_error_e
dxc_data_dyn_loader_property_parse(
    int unit,
    void *node,
    dnxc_data_property_t * property_info)
{
    int map_id, range_min, range_max, map_value;
    void *sub_node;
    char map_name[RHSTRING_MAX_SIZE];
    char method[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_GET_STR_DEF_NULL(node, "method", method);
    if (!sal_strcasecmp(method, ""))
    {
        if (sal_strncmp(method, property_info->method_str, RHSTRING_MAX_SIZE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - new method=%s is different than old method=%s\n",
                         method, property_info->method_str);
        }
    }

    
    if (property_info->method == dnxc_data_property_method_port_range_signed ||
        property_info->method == dnxc_data_property_method_port_range ||
        property_info->method == dnxc_data_property_method_range ||
        property_info->method == dnxc_data_property_method_suffix_range)
    {
        RHDATA_GET_INT_STOP(node, "range_min", range_min);
        RHDATA_GET_INT_STOP(node, "range_max", range_max);

        property_info->range_min = range_min;
        property_info->range_max = range_max;
    }

    
    if (property_info->method == dnxc_data_property_method_suffix_direct_map ||
        property_info->method == dnxc_data_property_method_port_direct_map ||
        property_info->method == dnxc_data_property_method_direct_map)
    {

        
        for (map_id = 0; map_id < property_info->nof_mapping; map_id++)
        {
            (property_info->mapping[map_id]).is_invalid = TRUE;
        }

        
        RHDATA_ITERATOR(sub_node, node, "map")
        {
            RHDATA_GET_STR_STOP(sub_node, "name", map_name);

            
            for (map_id = 0; map_id < property_info->nof_mapping; map_id++)
            {
                if (!sal_strncmp(map_name, (property_info->mapping[map_id]).name, RHSTRING_MAX_SIZE))
                {
                    
                    (property_info->mapping[map_id]).is_invalid = FALSE;

                    
                    RHDATA_GET_INT_BREAK(sub_node, "value", map_value);

                    (property_info->mapping[map_id]).val = map_value;

                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_table_data_type_name_to_enum_convert(
    int unit,
    char *type_name,
    dnxc_data_dyn_table_data_type * data_type)
{
    SHR_FUNC_INIT_VARS(unit);

    
    
    if (!sal_strcasecmp(type_name, "bcm_pbmp_t"))
    {
        *data_type = dnxc_data_dyn_table_data_type_pbmp;
    }
    
    else if (!sal_strcasecmp(type_name, "int"))
    {
        *data_type = dnxc_data_dyn_table_data_type_int;
    }
    
    else if (!sal_strcasecmp(type_name, "uint32"))
    {
        *data_type = dnxc_data_dyn_table_data_type_uint32;
    }
    
    else if (!sal_strncasecmp(type_name, "int[", sal_strlen("int[")))
    {
        *data_type = dnxc_data_dyn_table_data_type_int_array;
    }
    
    else if (!sal_strncasecmp(type_name, "uint32[", sal_strlen("uint32[")))
    {
        *data_type = dnxc_data_dyn_table_data_type_uint32_array;
    }
    
    else if (!sal_strcasecmp(type_name, "uint8"))
    {
        *data_type = dnxc_data_dyn_table_data_type_uint8;
    }
    
    else if (!sal_strcasecmp(type_name, "char *"))
    {
        *data_type = dnxc_data_dyn_table_data_type_string;
    }
    
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Dynamic DNX DATA - dynamic tables doesn't support the member type (%s)\n",
                     type_name);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dxc_data_dyn_loader_string_to_bitmap_convert(
    int unit,
    char *pbmp_str,
    bcm_pbmp_t * pbmp)
{
    char **datatokens;
    uint32 nof_tokens;
    int word_index, word_int;
    char *str_pointer_end;

    SHR_FUNC_INIT_VARS(unit);

    datatokens = NULL;

    
    utilex_str_white_spaces_remove(pbmp_str);

    
    datatokens = utilex_str_split(pbmp_str, ",", _SHR_PBMP_WORD_MAX, &nof_tokens);
    SHR_NULL_CHECK(datatokens, _SHR_E_FAIL, "Failed to split bitmap string into tokens");

    for (word_index = 0; word_index < nof_tokens; word_index++)
    {
        word_int = sal_ctoi(datatokens[word_index], &str_pointer_end);
        if (datatokens[word_index] == str_pointer_end)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Provided string (%s) is not a valid bitmap format", pbmp_str);
        }

        
        _SHR_PBMP_WORD_SET(*pbmp, word_index, word_int);
    }

exit:

    
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
dxc_data_dyn_loader_string_to_array_convert(
    int unit,
    char *array_str,
    uint32 *array)
{
    char **datatokens;
    uint32 nof_tokens;
    int word_index;
    uint32 word_int;
    char *str_pointer_end;

    SHR_FUNC_INIT_VARS(unit);

    datatokens = NULL;

    
    utilex_str_white_spaces_remove(array_str);

    
    datatokens = utilex_str_split(array_str, ",", _SHR_PBMP_WORD_MAX, &nof_tokens);
    SHR_NULL_CHECK(datatokens, _SHR_E_FAIL, "Failed to split bitmap string into tokens");

    for (word_index = 0; word_index < nof_tokens; word_index++)
    {
        word_int = sal_ctoi(datatokens[word_index], &str_pointer_end);
        if (datatokens[word_index] == str_pointer_end)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Provided string (%s) is not a valid array format", array_str);
        }

        
        array[word_index] = word_int;
    }

exit:

    
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
dxc_data_dyn_loader_string_to_array_length_get(
    int unit,
    char *array_str,
    int *length)
{
    char **datatokens;
    uint32 nof_tokens;

    SHR_FUNC_INIT_VARS(unit);

    datatokens = NULL;

    
    utilex_str_white_spaces_remove(array_str);

    
    datatokens = utilex_str_split(array_str, ",", _SHR_PBMP_WORD_MAX, &nof_tokens);
    SHR_NULL_CHECK(datatokens, _SHR_E_FAIL, "Failed to split bitmap string into tokens");

    *length = nof_tokens;

exit:

    
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_table_properties_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    int value_index;
    void *value_node, *property_node;
    char value_name[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    if (table_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table data wasn't found \n");
    }

    
    RHDATA_ITERATOR(value_node, table_info->dynamic_data_node, "value")
    {
        
        property_node = dbx_xml_child_get_first(value_node, "property");
        if (property_node != NULL)
        {
            
            RHDATA_GET_STR_STOP(value_node, "name", value_name);

            
            for (value_index = 0; value_index < table_info->nof_values; value_index++)
            {
                if (!sal_strncasecmp(table_info->values[value_index].name, value_name, RHSTRING_MAX_SIZE))
                {
                    
                    break;
                }
            }

            
            if (value_index == table_info->nof_values)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) has no member (%s) \n",
                             table_info->name, value_name);
            }

            
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_property_parse(unit, property_node,
                                                               &(table_info->values[value_index].property)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_table_defaults_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    int key1_size, key2_size, key1, key2, value_index, index;
    void *value_node;
    char default_value_str[RHSTRING_MAX_SIZE], value_name[RHSTRING_MAX_SIZE];
    char *str_pointer_end;
    uint32 *data_value_ptr;
    dnxc_data_dyn_table_data_type data_type;
    int val_int = 0;
    bcm_pbmp_t val_pbmp;
    char *val_str = NULL;
    uint32 *val_array = NULL;
    int length = 0;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(val_pbmp);

    
    RHDATA_ITERATOR(value_node, table_info->dynamic_data_node, "value")
    {
        
        RHDATA_GET_STR_STOP(value_node, "name", value_name);

        
        RHDATA_GET_LSTR_STOP(value_node, "default", default_value_str);

        
        for (value_index = 0; value_index < table_info->nof_values; value_index++)
        {
            if (!sal_strncasecmp(table_info->values[value_index].name, value_name, RHSTRING_MAX_SIZE))
            {
                
                break;
            }
        }

        
        if (value_index == table_info->nof_values)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) has no member (%s) \n", table_info->name,
                         value_name);
        }

        
        SHR_IF_ERR_EXIT(dnxc_data_dyn_table_data_type_name_to_enum_convert(unit,
                                                                           (table_info->values[value_index].type),
                                                                           &data_type));

        
        if (data_type == dnxc_data_dyn_table_data_type_pbmp)
        {
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_bitmap_convert(unit, default_value_str, &val_pbmp));
        }
        
        else if ((data_type == dnxc_data_dyn_table_data_type_int) ||
                 (data_type == dnxc_data_dyn_table_data_type_uint32))
        {
            val_int = sal_ctoi(default_value_str, &str_pointer_end);
            if (default_value_str == str_pointer_end)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                             table_info->name, value_name);
            }
        }
        
        else if (data_type == dnxc_data_dyn_table_data_type_uint8)
        {
            val_int = sal_ctoi(default_value_str, &str_pointer_end);
            if (default_value_str == str_pointer_end)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                             table_info->name, value_name);
            }
        }
        
        else if ((data_type == dnxc_data_dyn_table_data_type_int_array) ||
                 (data_type == dnxc_data_dyn_table_data_type_uint32_array))
        {
            
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_length_get(unit, default_value_str, &length));

            
            SHR_ALLOC_SET_ZERO(val_array, length * sizeof(uint32), "data array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_convert(unit, default_value_str, val_array));
        }
        else if (data_type == dnxc_data_dyn_table_data_type_string)
        {
            dnxc_data_dyn_str_linked_list_t *element = NULL;
            
            SHR_ALLOC_SET_ZERO(element, sizeof(dnxc_data_dyn_str_linked_list_t), "dnxc_data_dyn_str",
                               "dnxc_data_dyn_str_handle for unit %d %s%s\r\n", unit, EMPTY, EMPTY);
            sal_snprintf(element->str, sizeof(element->str) - 1, "%s", default_value_str);
            if (dnxc_data_dyn_str_handles_head[unit] == NULL)
            {
                dnxc_data_dyn_str_handles_head[unit] = element;
                dnxc_data_dyn_str_handles_tail[unit] = element;
            }
            else
            {
                dnxc_data_dyn_str_handles_tail[unit]->next = element;
                dnxc_data_dyn_str_handles_tail[unit] = element;
            }

            val_str = element->str;
        }
        
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) member (%s) has an "
                         "unsupported data type (%s). Dynamic data supports only 'int, uint32 and bcm_pbmp_t' \n",
                         table_info->name, value_name, table_info->values[value_index].name);
        }

        
        key1_size = (table_info->keys[0]).size;
        key2_size = (table_info->keys[1]).size;

        
        if (key1_size == 0)
        {
            key1_size = 1;
        }
        if (key2_size == 0)
        {
            key2_size = 1;
        }

        for (key1 = 0; key1 < key1_size; key1++)
        {
            for (key2 = 0; key2 < key2_size; key2++)
            {
                
                data_value_ptr = dnxc_data_utils_table_member_data_pointer_get(unit, table_info, value_name,
                                                                               key1, key2);
                if (data_value_ptr == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find data pointer for member (%s) in table (%s)",
                                 value_name, table_info->name);
                }

                
                if (data_type == dnxc_data_dyn_table_data_type_pbmp)
                {
                    _SHR_PBMP_ASSIGN(*(bcm_pbmp_t *) data_value_ptr, val_pbmp);
                }
                
                else if ((data_type == dnxc_data_dyn_table_data_type_int) ||
                         (data_type == dnxc_data_dyn_table_data_type_uint32))
                {
                    *(int *) data_value_ptr = val_int;
                }
                
                else if (data_type == dnxc_data_dyn_table_data_type_uint8)
                {
                    *(uint8 *) data_value_ptr = (uint8) val_int;
                }
                
                else if ((data_type == dnxc_data_dyn_table_data_type_int_array) ||
                         (data_type == dnxc_data_dyn_table_data_type_uint32_array))
                {
                    
                    if (val_array != NULL)
                    {
                        for (index = 0; index < length; index++)
                        {
                            ((uint32 *) data_value_ptr)[index] = val_array[index];
                        }

                        SHR_FREE(val_array);
                    }
                }
                
                else if (data_type == dnxc_data_dyn_table_data_type_string)
                {
                    *(char **) data_value_ptr = val_str;
                }
            }
        }
    }

exit:
    SHR_FREE(val_array);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_table_entries_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    int key1, key2, value_index, index;
    void *entries_node, *entry_node;
    char value_str[RHSTRING_MAX_SIZE];
    char *str_pointer_end;
    uint32 *data_value_ptr;
    dnxc_data_dyn_table_data_type data_type;
    int val_int = 0;
    bcm_pbmp_t val_pbmp;
    uint32 *val_array = NULL;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(val_pbmp);

    
    if (table_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table data wasn't found \n");
    }

    
    entries_node = dbx_xml_child_get_first(table_info->dynamic_data_node, "entries");
    if (entries_node != NULL)
    {
        
        RHDATA_ITERATOR(entry_node, entries_node, "entry")
        {
            
            {
                key1 = 0;
                key2 = 0;

                
                if (table_info->nof_keys > 0)
                {
                    
                    RHDATA_GET_INT_STOP(entry_node, (table_info->keys[0].name), key1);
                }
                if (table_info->nof_keys > 1)
                {
                    
                    RHDATA_GET_INT_STOP(entry_node, (table_info->keys[1].name), key2);
                }
                if (table_info->nof_keys > 2)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) has more than 2 keys = %d", table_info->name,
                                 table_info->nof_keys);
                }
            }

            
            for (value_index = 0; value_index < table_info->nof_values; value_index++)
            {
                
                RHDATA_GET_LSTR_DEF(entry_node, (table_info->values[value_index].name), value_str, "");

                
                if (sal_strncmp(value_str, "", RHSTRING_MAX_SIZE))
                {
                    
                    SHR_IF_ERR_EXIT(dnxc_data_dyn_table_data_type_name_to_enum_convert(unit,
                                                                                       (table_info->
                                                                                        values[value_index].type),
                                                                                       &data_type));

                    
                    data_value_ptr = dnxc_data_utils_table_member_data_pointer_get(unit, table_info,
                                                                                   (table_info->
                                                                                    values[value_index].name), key1,
                                                                                   key2);
                    if (data_value_ptr == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find data pointer for member (%s) in table (%s)",
                                     (table_info->values[value_index].name), table_info->name);
                    }

                    
                    if (data_type == dnxc_data_dyn_table_data_type_pbmp)
                    {
                        SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_bitmap_convert(unit, value_str, &val_pbmp));

                        
                        _SHR_PBMP_ASSIGN(*(bcm_pbmp_t *) data_value_ptr, val_pbmp);
                    }
                    
                    else if ((data_type == dnxc_data_dyn_table_data_type_int) ||
                             (data_type == dnxc_data_dyn_table_data_type_uint32))
                    {
                        val_int = sal_ctoi(value_str, &str_pointer_end);
                        if (value_str == str_pointer_end)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                                         table_info->name, table_info->values[value_index].name);
                        }

                        
                        *(int *) data_value_ptr = val_int;
                    }
                    
                    else if (data_type == dnxc_data_dyn_table_data_type_uint8)
                    {
                        val_int = sal_ctoi(value_str, &str_pointer_end);
                        if (value_str == str_pointer_end)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "Table (%s) member (%s) default value must be a number",
                                         table_info->name, table_info->values[value_index].name);
                        }

                        
                        *(uint8 *) data_value_ptr = (uint8) val_int;
                    }
                    
                    else if ((data_type == dnxc_data_dyn_table_data_type_int_array) ||
                             (data_type == dnxc_data_dyn_table_data_type_uint32_array))
                    {
                        int length;

                        
                        SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_length_get(unit, value_str, &length));

                        SHR_ALLOC_SET_ZERO(val_array, length * sizeof(uint32),
                                           "data array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

                        SHR_IF_ERR_EXIT(dxc_data_dyn_loader_string_to_array_convert(unit, value_str, val_array));

                        
                        for (index = 0; index < length; index++)
                        {
                            ((uint32 *) data_value_ptr)[index] = val_array[index];
                        }

                        SHR_FREE(val_array);
                    }
                    
                    else if (data_type == dnxc_data_dyn_table_data_type_string)
                    {
                        dnxc_data_dyn_str_linked_list_t *element = NULL;
                        
                        SHR_ALLOC_SET_ZERO(element, sizeof(dnxc_data_dyn_str_linked_list_t), "dnxc_data_dyn_str",
                                           "dnxc_data_dyn_str_handle for unit %d %s%s\r\n", unit, EMPTY, EMPTY);
                        sal_snprintf(element->str, sizeof(element->str) - 1, "%s", value_str);
                        if (dnxc_data_dyn_str_handles_head[unit] == NULL)
                        {
                            dnxc_data_dyn_str_handles_head[unit] = element;
                            dnxc_data_dyn_str_handles_tail[unit] = element;
                        }
                        else
                        {
                            dnxc_data_dyn_str_handles_tail[unit]->next = element;
                            dnxc_data_dyn_str_handles_tail[unit] = element;
                        }

                        *(char **) data_value_ptr = element->str;
                    }
                }
            }
        }
    }

exit:
    SHR_FREE(val_array);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_tables_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char table_name[RHSTRING_MAX_SIZE];
    xml_node table_node;
    dnxc_data_table_t *table_info;

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_ITERATOR(table_node, node, "table")
    {
        
        RHDATA_GET_STR_STOP(table_node, "name", table_name);

        
        table_info = dnxc_data_utils_table_info_get(unit, module_name, submodule_name, table_name);

        if (table_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - table (%s) data wasn't found \n", table_name);
        }

        table_info->dynamic_data_node = table_node;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_table_value_set(
    int unit,
    dnxc_data_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (table_info->dynamic_data_node != NULL)
    {
        
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_defaults_set(unit, table_info));

        
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_entries_set(unit, table_info));

        
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_properties_set(unit, table_info));

        
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, table_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_numerics_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char numeric_name[RHSTRING_MAX_SIZE];
    dnxc_data_define_t *numeric_info;
    xml_node sub_node;

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_ITERATOR(sub_node, node, "numeric")
    {
        RHDATA_GET_STR_STOP(sub_node, "name", numeric_name);

        
        numeric_info = dnxc_data_utils_define_info_get(unit, module_name, submodule_name, numeric_name);
        if (numeric_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - numeric (%s) data wasn't found \n", numeric_name);
        }

        numeric_info->dynamic_data_node = sub_node;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_numeric_value_set(
    int unit,
    dnxc_data_define_t * numeric_info)
{
    int numeric_default_value;
    xml_node property_node;

    SHR_FUNC_INIT_VARS(unit);

    
    if (numeric_info->dynamic_data_node != NULL)
    {
        
        RHDATA_GET_INT_STOP(numeric_info->dynamic_data_node, "value", numeric_default_value);

        
        numeric_info->data = numeric_default_value;

        
        property_node = dbx_xml_child_get_first(numeric_info->dynamic_data_node, "property");
        if (property_node != NULL)
        {
            SHR_IF_ERR_EXIT(dxc_data_dyn_loader_property_parse(unit, property_node, &(numeric_info->property)));
        }

        
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, numeric_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_features_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char feature_name[RHSTRING_MAX_SIZE];
    dnxc_data_feature_t *feature_info;
    xml_node sub_node;

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_ITERATOR(sub_node, node, "feature")
    {
        RHDATA_GET_STR_STOP(sub_node, "name", feature_name);

        
        feature_info = dnxc_data_utils_feature_info_get(unit, module_name, submodule_name, feature_name);
        if (feature_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - feature (%s) data wasn't found \n", feature_name);
        }

        feature_info->dynamic_data_node = sub_node;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_feature_value_set(
    int unit,
    dnxc_data_feature_t * feature_info)
{
    int feature_new_value;

    SHR_FUNC_INIT_VARS(unit);

    
    if (feature_info->dynamic_data_node != NULL)
    {
        
        RHDATA_GET_INT_DEF(feature_info->dynamic_data_node, "value", feature_new_value, 1);

        
        if ((feature_new_value != 0) && (feature_new_value != 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - feature (%s) value (%d) must be either '0' or '1'\n",
                         feature_info->name, feature_new_value);
        }

        
        feature_info->data = feature_new_value;

        
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, feature_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_define_value_set_verify(
    int unit,
    dnxc_data_define_t * define_info,
    int define_new_value)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (_SHR_IS_FLAG_SET(define_info->flags, DNXC_DATA_F_DEFINE) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - failed parsing define."
                     " data (%s) type is not of type 'define'\n", define_info->name);
    }

    
    if (define_new_value > define_info->data)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - define data (%s) new value (%d) is bigger than the original"
                     " value(%d)\n", define_info->name, define_new_value, define_info->data);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_defines_parse(
    int unit,
    xml_node node,
    char *module_name,
    char *submodule_name)
{
    char define_name[RHSTRING_MAX_SIZE];
    dnxc_data_define_t *define_info;
    xml_node sub_node;

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_ITERATOR(sub_node, node, "define")
    {
        
        RHDATA_GET_STR_STOP(sub_node, "name", define_name);

        
        define_info = dnxc_data_utils_define_info_get(unit, module_name, submodule_name, define_name);
        if (define_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - define data wasn't found %s\n", define_name);
        }

        define_info->dynamic_data_node = sub_node;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_define_value_set(
    int unit,
    dnxc_data_define_t * define_info)
{
    int define_new_value;

    SHR_FUNC_INIT_VARS(unit);

    
    if (define_info->flags & DNXC_DATA_F_NUMERIC)
    {
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_numeric_value_set(unit, define_info));
        SHR_EXIT();
    }

    
    if (define_info->dynamic_data_node != NULL)
    {
        
        RHDATA_GET_INT_STOP(define_info->dynamic_data_node, "value", define_new_value);

        
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set_verify(unit, define_info, define_new_value));

        
        define_info->data = define_new_value;

        
        SHR_IF_ERR_EXIT(dnxc_data_utils_label_set(unit, define_info->labels, "dynamic"));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_dev_id_verify(
    int unit)
{

    uint16 dev_id;
    uint8 rev_id;
    uint32 data_dev_id;
    dnxc_data_define_t *numeric_info;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(soc_cm_get_id(unit, &dev_id, &rev_id));

    
    numeric_info = dnxc_data_utils_define_info_get(unit, "device", "general", "device_id");
    SHR_NULL_CHECK(numeric_info, _SHR_E_FAIL, "numeric_info");

    data_dev_id = numeric_info->data;
    
    if (dev_id != data_dev_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic DNX DATA - Incorrect DNX Data XML was provided. Current device ID is %x, "
                     "but the provided data belongs to device ID %x \n", dev_id, data_dev_id);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_module_parse(
    int unit,
    xml_node module,
    char module_name[RHNAME_MAX_SIZE])
{
    xml_node cur_sub_module, node;
    char sub_module_name[RHFILE_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_ITERATOR(cur_sub_module, module, "sub_module")
    {
        
        RHDATA_GET_STR_STOP(cur_sub_module, "name", sub_module_name);

        
        node = dbx_xml_child_get_first(cur_sub_module, "defines");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_defines_parse(unit, node, module_name, sub_module_name));
        }

        
        node = dbx_xml_child_get_first(cur_sub_module, "features");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_features_parse(unit, node, module_name, sub_module_name));
        }

        
        node = dbx_xml_child_get_first(cur_sub_module, "numerics");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_numerics_parse(unit, node, module_name, sub_module_name));
        }

        
        node = dbx_xml_child_get_first(cur_sub_module, "tables");
        if (node != NULL)
        {
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_tables_parse(unit, node, module_name, sub_module_name));
        }

    } 

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_file_parse(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    xml_node cur_top = NULL, cur_module;
    char module_name[RHNAME_MAX_SIZE];
    rhhandle_t temp = NULL;
    dnxc_data_dyn_file_top_info_t *dnxc_data_dyn_file_top_info;

    SHR_FUNC_INIT_VARS(unit);

    
    if (ISEMPTY(file_path))
    {
        SHR_EXIT();
    }

    
    cur_top =
        dbx_pre_compiled_devices_top_get(unit, file_path, "device_data",
                                         CONF_OPEN_NO_ERROR_REPORT | CONF_OPEN_PER_DEVICE);

    
    if (cur_top == NULL)
    {
        SHR_EXIT();
    }

#ifndef ADAPTER_SERVER_MODE
    LOG_INFO_EX(BSL_LOG_MODULE, "Dynamic Data XML found: %s\n%s%s%s", file_path, EMPTY, EMPTY, EMPTY);
#endif 

    
    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(dnxc_data_dyn_files_top_list[unit], "", RHID_TO_BE_GENERATED, &temp));
    dnxc_data_dyn_file_top_info = (dnxc_data_dyn_file_top_info_t *) temp;
    dnxc_data_dyn_file_top_info->file_top = cur_top;

    
    RHDATA_GET_STR_DEF_NULL(cur_top, "module", module_name);
    if (sal_strcasecmp(module_name, ""))
    {
        SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_module_parse(unit, cur_top, module_name));
    }
    else
    {
        
        RHDATA_ITERATOR(cur_module, cur_top, "module")
        {
            
            RHDATA_GET_STR_STOP(cur_module, "name", module_name);
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_module_parse(unit, cur_module, module_name));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_parse_init(
    int unit)
{
    char file_path[RHFILE_MAX_SIZE];
    int file_i, nof_files = 0;
    dnxc_data_dyn_file_path_get_f *file_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if ((dnxc_data_dyn_files_top_list[unit] =
         utilex_rhlist_create("dnxc_data_dyn_files_top_list", sizeof(dnxc_data_dyn_file_top_info_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to create dnxc_data_dyn_files_top_list\n");
    }

    
#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX(unit))
    {
        nof_files = COUNTOF(dnx_data_dyn_files);
        file_list = dnx_data_dyn_files;
    }
#endif

#if defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_DNXF(unit))
    {
        nof_files = COUNTOF(dnxf_data_dyn_files);
        file_list = dnxf_data_dyn_files;
    }
#endif

    
    for (file_i = 0; file_i < nof_files; file_i++)
    {
        sal_memset(file_path, 0, sizeof(file_path));

        if (file_list != NULL)
        {
            
            SHR_IF_ERR_EXIT(file_list[file_i] (unit, file_path));

            
            SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_file_parse(unit, file_path));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_dyn_loader_parse_deinit(
    int unit)
{
    dnxc_data_dyn_str_linked_list_t *cur_element, *next_element;
    rhhandle_t file_top_element;
    dnxc_data_dyn_file_top_info_t *dnxc_data_dyn_file_top_info;

    SHR_FUNC_INIT_VARS(unit);

    
    cur_element = dnxc_data_dyn_str_handles_head[unit];
    while (cur_element != NULL)
    {
        next_element = cur_element->next;
        SHR_FREE(cur_element);
        cur_element = next_element;
    }

    
    dnxc_data_dyn_str_handles_head[unit] = NULL;
    dnxc_data_dyn_str_handles_tail[unit] = NULL;

    
    if (dnxc_data_dyn_files_top_list[unit] != NULL)
    {
        file_top_element = utilex_rhlist_entry_get_first(dnxc_data_dyn_files_top_list[unit]);
        while (file_top_element != NULL)
        {
            dnxc_data_dyn_file_top_info = (dnxc_data_dyn_file_top_info_t *) file_top_element;

            
            if (dnxc_data_dyn_file_top_info->file_top != NULL)
            {
                dbx_xml_top_close(dnxc_data_dyn_file_top_info->file_top);
            }

            
            file_top_element = utilex_rhlist_entry_get_next(file_top_element);
        }
    }

    
    utilex_rhlist_free_all(dnxc_data_dyn_files_top_list[unit]);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_data_dyn_loader_sku_file_path_common_get(
    int unit,
    char *dyn_data_subdir,
    char file_path[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE];
    uint16 dev_id;
    uint8 rev_id;
    char chip_name[RHFILE_MAX_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    

    
    sal_strncpy(file_path, dyn_data_subdir, RHFILE_MAX_SIZE);

    
    
    SHR_IF_ERR_EXIT(soc_cm_get_id(unit, &dev_id, &rev_id));
    
    sal_snprintf(chip_name, RHFILE_MAX_SIZE, "BCM8%hx", dev_id);
    sal_strncpy(file_name, chip_name, RHFILE_MAX_SIZE - 12);

    
    utilex_str_to_lower(file_name);

    
    sal_strncat(file_name, "_data.xml", RHFILE_MAX_SIZE - 1);

    
    sal_strncat(file_path, file_name, RHFILE_MAX_SIZE - 1);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_data_dyn_loader_sku_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_sku_file_path_common_get(unit, DNXC_DATA_DYN_XML_DB_PATH, file_path));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_data_dyn_loader_internal_sku_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_sku_file_path_common_get(unit, DNXC_DATA_DYN_XML_DB_PATH_INTERNAL, file_path));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_sku_revision_file_path_common_get(
    int unit,
    char *dyn_data_subdir,
    char file_path[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE];
    const char *chip_name;

    

    
    sal_strncpy(file_path, dyn_data_subdir, RHFILE_MAX_SIZE);

    
    chip_name = soc_dev_name(unit);
    sal_strncpy(file_name, chip_name, RHFILE_MAX_SIZE - 10);

    
    utilex_str_to_lower(file_name);

    
    sal_strncat(file_name, "_data.xml", RHFILE_MAX_SIZE - 1);

    

    
    sal_strncat(file_path, file_name, RHFILE_MAX_SIZE - 1);

    return _SHR_E_NONE;
}


static shr_error_e
dnxc_data_dyn_loader_sku_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_sku_revision_file_path_common_get(unit, DNXC_DATA_DYN_XML_DB_PATH, file_path));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnxc_data_dyn_loader_internal_sku_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_sku_revision_file_path_common_get
                    (unit, DNXC_DATA_DYN_XML_DB_PATH_INTERNAL, file_path));

exit:
    SHR_FUNC_EXIT;

}

#ifdef ADAPTER_SERVER_MODE

static shr_error_e
dnxc_data_dyn_loader_adapter_revision_file_path_get(
    int unit,
    char file_path[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE];
    const char *chip_name;

    

    
    sal_strncpy(file_path, DNXC_DATA_DYN_ADAPTER_XML_DB_PATH, RHFILE_MAX_SIZE);

    
    chip_name = soc_dev_name(unit);
    sal_strncpy(file_name, chip_name, RHFILE_MAX_SIZE - 10);

    
    utilex_str_to_lower(file_name);

    
    sal_strncat(file_name, "_data.xml", RHFILE_MAX_SIZE - 1);

    
    sal_strncat(file_path, file_name, RHFILE_MAX_SIZE - 1);

    return _SHR_E_NONE;
}
#endif 


