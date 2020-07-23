

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA


#include <stdlib.h>

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <shared/utilex/utilex_str.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>





shr_error_e
dnxc_data_utils_dump_verify(
    int unit,
    uint32 state_flags,
    uint32 data_flags,
    char **data_labels,
    uint32 dump_flags,
    char *dump_label,
    int *dump)
{
    uint32 data_types;
    SHR_FUNC_INIT_VARS(unit);

    
    *dump = 1;

    
    if ((data_flags & DNXC_DATA_F_SUPPORTED) == 0)
    {
        *dump = 0;
    }

    
    if ((dump_flags & DNXC_DATA_F_CHANGED) && ((data_flags & DNXC_DATA_F_CHANGED) == 0))
    {
        *dump = 0;
    }

    
    if ((dump_flags & DNXC_DATA_F_PROPERTY) && ((data_flags & DNXC_DATA_F_PROPERTY) == 0))
    {
        *dump = 0;
    }

    
    data_types = dump_flags & DNXC_DATA_FLAG_DATA_TYPES_MASK;
    if (data_types)
    {
        if ((data_flags & data_types) == 0)
        {
            *dump = 0;
        }
    }

    
    if (dump_label != NULL && (*dump_label) != 0)
    {
        int label_index, label_exist = 0;
        for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
        {
            if (data_labels[label_index] != NULL
                && !sal_strncmp(data_labels[label_index], dump_label, DNXC_DATA_UTILS_STRING_MAX))
            {
                label_exist = 1;
                break;
            }
        }
        if (label_exist == 0)
        {
            *dump = 0;
        }
    }

    
    SHR_FUNC_EXIT;
}

const uint32 *
dnxc_data_utils_generic_feature_data_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name)
{
    int feature_index;
    dnxc_data_feature_t *features;
    int nof_features;

    
    features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].features;
    nof_features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_features;

    
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {
        if (!sal_strncasecmp(features[feature_index].name, data_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            return (uint32 *) &features[feature_index].data;
        }
    }

    
    return NULL;
}

const uint32 *
dnxc_data_utils_generic_define_data_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name)
{
    int define_index;
    dnxc_data_define_t *defines;
    int nof_defines;
    shr_error_e rv;

    
    defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].defines;
    nof_defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_defines;

    
    for (define_index = 0; define_index < nof_defines; define_index++)
    {
        if (!sal_strncasecmp(defines[define_index].name, data_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                              defines[define_index].set);
            if (rv != _SHR_E_NONE)
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"),
                           defines[define_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            
            return &defines[define_index].data;
        }
    }

    return NULL;
}

const uint32 *
dnxc_data_utils_generic_table_data_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data,
    char *member,
    int key1,
    int key2)
{
    int table_index, value_index;
    dnxc_data_table_t *tables;
    int nof_tables;
    char *table_data;

    
    tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].tables;
    nof_tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_tables;

    
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(tables[table_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            table_data = dnxc_data_mgmt_table_data_get(unit, &tables[table_index], key1, key2);
            if (table_data == NULL)
            {
                
                return NULL;
            }

            
            for (value_index = 0; value_index < tables[table_index].nof_values; value_index++)
            {
                if (!sal_strncasecmp(tables[table_index].values[value_index].name, member, DNXC_DATA_UTILS_STRING_MAX))
                {
                    
                    return (const uint32 *) (table_data + tables[table_index].values[value_index].offset);
                }
            }

            return NULL;
        }
    }

    return NULL;
}

const uint32 *
dnxc_data_utils_generic_module_2d_data_get(
    int unit,
    int module_index,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2)
{
    int submodule_index;
    const uint32 *res;

    
    dnxc_data_module_t *module = &_dnxc_data[unit].modules[module_index];
    dnxc_data_submodule_t *submodules = module->submodules;

    
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        if (!sal_strncasecmp(submodules[submodule_index].name, submodule, DNXC_DATA_UTILS_STRING_MAX))
        {

            
            res = dnxc_data_utils_generic_feature_data_get(unit, module_index, submodule_index, data);
            if (res != NULL)
            {
                return res;
            }

            
            res = dnxc_data_utils_generic_define_data_get(unit, module_index, submodule_index, data);
            if (res != NULL)
            {
                return res;
            }

            
            if (!ISEMPTY(member))
            {
                res =
                    dnxc_data_utils_generic_table_data_get(unit, module_index, submodule_index, data, member, key1,
                                                           key2);
                if (res != NULL)
                {
                    return res;
                }
            }
            
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "DNX DATA - data wasn't found  %s, %s\n"), data, member != NULL ? member : ""));
            return NULL;
        }
    }

    
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - submodule wasn't found  %s\n"), submodule));
    return NULL;
}

const uint32 *
dnxc_data_utils_generic_value_get(
    int unit,
    char *dnx_data_str)
{
    char *module = NULL, *submodule = NULL, *data = NULL, *member = NULL;
    int key1 = 0, key2 = 0;
    char **tokens = NULL;
    uint32 realtokens = 0;
    const uint32 *value_ptr = NULL;
    
    if ((tokens = utilex_str_split(dnx_data_str, ".", 4, &realtokens)) == NULL)
    {
        goto exit;
    }

    if ((realtokens == 3) || (realtokens == 4))
    {
        module = tokens[0];
        submodule = tokens[1];
        data = tokens[2];
    }
    else
        goto exit;

    if (realtokens == 4)
    {
        int i_ch, size;
        char *key1_str = NULL;
        char *key2_str = NULL;

        member = tokens[3];
        size = sal_strlen(tokens[3]);
        
        for (i_ch = 0; i_ch < size; i_ch++)
        {
            char *end_ptr;
            if (tokens[3][i_ch] == '(')
            {
                tokens[3][i_ch] = 0;
                key1_str = tokens[3] + i_ch;
            }
            else if (tokens[3][i_ch] == ',')
            {
                tokens[3][i_ch] = 0;
                if (key1_str == NULL)
                    goto exit;

                key1 = sal_strtoul(key1_str, &end_ptr, 0);
                if (end_ptr[0] != 0)
                    goto exit;
                key2_str = tokens[3] + i_ch;
            }
            else if (tokens[3][i_ch] == ')')
            {
                tokens[3][i_ch] = 0;
                if (key2_str != NULL)
                { 
                    key2 = sal_strtoul(key2_str, &end_ptr, 0);
                    if (end_ptr[0] != 0)
                        goto exit;
                }
                else if (key1_str != NULL)
                { 
                    key1 = sal_strtoul(key1_str, &end_ptr, 0);
                    if (end_ptr[0] != 0)
                        goto exit;
                }
                else if ((key1_str + 1) == (tokens[3] + i_ch))
                { 
                }
                else
                    goto exit;
            }
            else
            {
                
                continue;
            }
        }
    }
    value_ptr = dnxc_data_utils_generic_2d_data_get(unit, module, submodule, data, member, key1, key2);

exit:
    if (value_ptr == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - Illegal dnx data string:%s\n"), dnx_data_str));
    }
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    return value_ptr;
}

const uint32 *
dnxc_data_utils_generic_2d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2)
{
    int module_index;

    
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;

    
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        if (!sal_strncasecmp(modules[module_index].name, module, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            return dnxc_data_utils_generic_module_2d_data_get(unit, module_index, submodule, data, member, key1, key2);
        }
    }

    
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - module wasn't found  %s\n"), module));
    return NULL;
}

const uint32 *
dnxc_data_utils_generic_1d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1)
{
    return dnxc_data_utils_generic_2d_data_get(unit, module, submodule, data, member, key1, 0);
}

const uint32 *
dnxc_data_utils_generic_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member)
{
    return dnxc_data_utils_generic_2d_data_get(unit, module, submodule, data, member, 0, 0);
}

char *
dnxc_data_utils_generic_feature_data_str_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name,
    char *buffer)
{
    int feature_index;
    dnxc_data_feature_t *features;
    int nof_features;
    int val;

    
    features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].features;
    nof_features = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_features;

    
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {
        if (!sal_strncasecmp(features[feature_index].name, data_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            val = features[feature_index].data;
            sal_sprintf(buffer, "%d", val);
            return buffer;

        }
    }

    return NULL;
}

char *
dnxc_data_utils_generic_define_data_str_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data_name,
    char *buffer)
{
    int define_index;
    dnxc_data_define_t *defines;
    int nof_defines;
    shr_error_e rv;

    
    defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].defines;
    nof_defines = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_defines;

    
    for (define_index = 0; define_index < nof_defines; define_index++)
    {
        if (!sal_strncasecmp(defines[define_index].name, data_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                              defines[define_index].set);
            if (rv != _SHR_E_NONE)
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"),
                           defines[define_index].name, _dnxc_data[unit].name));
                sal_sprintf(buffer, "%s", "null");
            }

            
            sal_sprintf(buffer, "%d", defines[define_index].data);
            return buffer;
        }
    }

    return NULL;
}

char *
dnxc_data_utils_generic_table_data_str_get(
    int unit,
    int module_index,
    int submodule_index,
    char *data,
    char *member,
    int key1,
    int key2,
    char *buffer)
{
    int table_index, value_index;
    dnxc_data_table_t *tables;
    int nof_tables;
    shr_error_e rv;

    
    tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].tables;
    nof_tables = _dnxc_data[unit].modules[module_index].submodules[submodule_index].nof_tables;

    
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(tables[table_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
        {
            if (tables[table_index].entry_get == NULL)
            {
                
                return NULL;
            }

            
            for (value_index = 0; value_index < tables[table_index].nof_values; value_index++)
            {
                if (!sal_strncasecmp(tables[table_index].values[value_index].name, member, DNXC_DATA_UTILS_STRING_MAX))
                {
                    
                    rv = tables[table_index].entry_get(unit, buffer, key1, key2, value_index);
                    if (rv != _SHR_E_NONE)
                    {
                        return NULL;
                    }
                    return buffer;
                }
            }

            return NULL;
        }
    }

    return NULL;
}

char *
dnxc_data_utils_generic_module_str_get(
    int unit,
    int module_index,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2,
    char *buffer)
{
    int submodule_index;
    char *res;
    dnxc_data_module_t *module = &_dnxc_data[unit].modules[module_index];
    dnxc_data_submodule_t *submodules = module->submodules;

    
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        if (!sal_strncasecmp(submodules[submodule_index].name, submodule, DNXC_DATA_UTILS_STRING_MAX))
        {

            
            res = dnxc_data_utils_generic_feature_data_str_get(unit, module_index, submodule_index, data, buffer);
            if (res != NULL)
            {
                return res;
            }

            
            res = dnxc_data_utils_generic_define_data_str_get(unit, module_index, submodule_index, data, buffer);
            if (res != NULL)
            {
                return res;
            }

            
            res = dnxc_data_utils_generic_table_data_str_get(unit, module_index, submodule_index, data, member, key1,
                                                             key2, buffer);
            if (res != NULL)
            {
                return res;
            }

            
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "DNX DATA - data wasn't found  %s, %s\n"), data, member != NULL ? member : ""));
            return NULL;
        }
    }

    
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - submodule wasn't found  %s\n"), submodule));
    return NULL;
}

char *
dnxc_data_utils_generic_data_str_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2,
    char *buffer,
    int buffer_size)
{
    int module_index;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;

    
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        if (!sal_strncasecmp(modules[module_index].name, module, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            return dnxc_data_utils_generic_module_str_get(unit, module_index, submodule, data, member, key1, key2,
                                                          buffer);
        }
    }

    
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - module wasn't found  %s\n"), module));
    return NULL;
}


static shr_error_e
dnxc_data_utils_indices_get(
    int unit,
    char *module_name,
    char *submodule_name,
    int *module_index,
    int *submodule_index)
{
    dnxc_data_module_t *modules;

    SHR_FUNC_INIT_VARS(unit);

    
    modules = _dnxc_data[unit].modules;

    
    for (*module_index = 0; *module_index < _dnxc_data[unit].nof_modules; (*module_index)++)
    {
        if (!sal_strncasecmp(modules[*module_index].name, module_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            dnxc_data_module_t *module;
            dnxc_data_submodule_t *submodules;

            
            module = &_dnxc_data[unit].modules[*module_index];
            submodules = module->submodules;

            
            for (*submodule_index = 0; *submodule_index < module->nof_submodules; (*submodule_index)++)
            {
                if (!sal_strncasecmp(submodules[*submodule_index].name, submodule_name, DNXC_DATA_UTILS_STRING_MAX))
                {
                    
                    SHR_EXIT();
                }
            }

            SHR_ERR_EXIT(_SHR_E_PARAM, "DNX DATA - submodule wasn't found  %s\n", submodule_name);
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "DNX DATA - module wasn't found  %s\n", module_name);

exit:
    SHR_FUNC_EXIT;
}


dnxc_data_define_t *
dnxc_data_utils_define_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *define_name)
{
    int module_index, submodule_index, define_index;
    dnxc_data_module_t *modules;
    dnxc_data_define_t *defines;
    int nof_defines;
    shr_error_e rv;

    
    modules = _dnxc_data[unit].modules;

    
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    
    defines = modules[module_index].submodules[submodule_index].defines;
    nof_defines = modules[module_index].submodules[submodule_index].nof_defines;

    
    for (define_index = 0; define_index < nof_defines; define_index++)
    {
        if (!sal_strncasecmp(defines[define_index].name, define_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, defines[define_index].flags,
                                              defines[define_index].set);
            if (rv != _SHR_E_NONE)
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"),
                           defines[define_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            
            return &defines[define_index];
        }
    }

    return NULL;
}


dnxc_data_feature_t *
dnxc_data_utils_feature_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *feature_name)
{
    int module_index, submodule_index, feature_index;
    dnxc_data_module_t *modules;
    dnxc_data_feature_t *features;
    int nof_features;
    shr_error_e rv;

    
    modules = _dnxc_data[unit].modules;

    
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    
    features = modules[module_index].submodules[submodule_index].features;
    nof_features = modules[module_index].submodules[submodule_index].nof_features;

    
    for (feature_index = 0; feature_index < nof_features; feature_index++)
    {
        if (!sal_strncasecmp(features[feature_index].name, feature_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, features[feature_index].flags,
                                              features[feature_index].set);
            if (rv != _SHR_E_NONE)
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - feature %s access denied for device %s\n"),
                           features[feature_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            
            return &features[feature_index];
        }
    }

    return NULL;
}


dnxc_data_table_t *
dnxc_data_utils_table_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *table_name)
{
    int module_index, submodule_index, table_index;
    dnxc_data_module_t *modules;
    dnxc_data_table_t *tables;
    int nof_tables;
    shr_error_e rv;

    
    modules = _dnxc_data[unit].modules;

    
    rv = dnxc_data_utils_indices_get(unit, module_name, submodule_name, &module_index, &submodule_index);
    if (rv != _SHR_E_NONE)
    {
        return NULL;
    }

    
    tables = modules[module_index].submodules[submodule_index].tables;
    nof_tables = modules[module_index].submodules[submodule_index].nof_tables;

    
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        if (!sal_strncasecmp(tables[table_index].name, table_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, tables[table_index].flags,
                                              tables[table_index].set);
            if (rv != _SHR_E_NONE)
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - feature %s access denied for device %s\n"),
                           tables[table_index].name, _dnxc_data[unit].name));
                return NULL;
            }

            
            return &tables[table_index];
        }
    }

    return NULL;
}


uint32 *
dnxc_data_utils_table_member_data_pointer_get(
    int unit,
    dnxc_data_table_t * table_info,
    char *member_name,
    int key1,
    int key2)
{
    int value_index;
    char *table_data;

    
    table_data = dnxc_data_mgmt_table_data_get(unit, table_info, key1, key2);
    if (table_data == NULL)
    {
        
        return NULL;
    }

    
    for (value_index = 0; value_index < table_info->nof_values; value_index++)
    {
        if (!sal_strncasecmp(table_info->values[value_index].name, member_name, DNXC_DATA_UTILS_STRING_MAX))
        {
            
            return (uint32 *) (table_data + table_info->values[value_index].offset);
        }
    }

    return NULL;
}


shr_error_e
dnxc_data_utils_label_set(
    int unit,
    char **labels,
    char *label)
{
    int label_index;

    SHR_FUNC_INIT_VARS(unit);

    
    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
    {
        
        if (labels[label_index] == NULL)
        {
            labels[label_index] = label;
            break;
        }
    }


    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_utils_supported_values_get(
    int unit,
    dnxc_data_property_t * property,
    char *buffer_supported_values,
    int buffer_size,
    int show_mapping)
{
    int map_index;
    int offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    buffer_supported_values[0] = 0;
    switch (property->method)
    {
        case dnxc_data_property_method_enable:
        case dnxc_data_property_method_port_enable:
        case dnxc_data_property_method_suffix_enable:
        case dnxc_data_property_method_suffix_only_enable:
        case dnxc_data_property_method_port_suffix_enable:
            sal_snprintf(buffer_supported_values, buffer_size, "<0 | 1>");
            break;
        case dnxc_data_property_method_range:
        case dnxc_data_property_method_port_range:
        case dnxc_data_property_method_suffix_range:
        case dnxc_data_property_method_suffix_only_range:
        case dnxc_data_property_method_port_suffix_range:
            sal_snprintf(buffer_supported_values, buffer_size, "<[%u - %u]>", property->range_min, property->range_max);
            break;
        case dnxc_data_property_method_range_signed:
        case dnxc_data_property_method_port_range_signed:
        case dnxc_data_property_method_suffix_range_signed:
        case dnxc_data_property_method_suffix_only_range_signed:
        case dnxc_data_property_method_port_suffix_range_signed:
            sal_snprintf(buffer_supported_values, buffer_size, "<[%d - %d]>", property->range_min, property->range_max);

            break;
        case dnxc_data_property_method_direct_map:
        case dnxc_data_property_method_port_direct_map:
        case dnxc_data_property_method_suffix_direct_map:
        case dnxc_data_property_method_suffix_only_direct_map:
        case dnxc_data_property_method_port_suffix_direct_map:
            if (!show_mapping)
            {
                offset = sal_snprintf(buffer_supported_values, buffer_size, "<");
            }
            for (map_index = 0; map_index < property->nof_mapping; map_index++)
            {
                if (show_mapping)
                {
                    sal_snprintf(buffer_supported_values, buffer_size, "%s%s -> %d\n",
                                 buffer_supported_values, property->mapping[map_index].name,
                                 property->mapping[map_index].val);
                }
                else
                {
                    if (property->mapping[map_index].is_invalid != TRUE)
                    {
                        offset +=
                            sal_snprintf(buffer_supported_values + offset, buffer_size - offset, "%s | ",
                                         property->mapping[map_index].name);
                    }
                }
            }
            if (!show_mapping)
            {
                sal_snprintf(buffer_supported_values + offset - 3, buffer_size - offset, ">");
            }
            break;
        case dnxc_data_property_method_pbmp:
        case dnxc_data_property_method_port_pbmp:
        case dnxc_data_property_method_suffix_pbmp:
            sal_snprintf(buffer_supported_values, buffer_size, "<port bitmap>");
            break;
        case dnxc_data_property_method_str:
        case dnxc_data_property_method_port_str:
        case dnxc_data_property_method_suffix_str:
        case dnxc_data_property_method_port_suffix_str:
            sal_snprintf(buffer_supported_values, buffer_size, "<string>");
            break;
        case dnxc_data_property_method_custom:
            sal_snprintf(buffer_supported_values, buffer_size, "custom - see description");
            break;
        default:
            
            break;
    }

        
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_utils_property_val_get(
    int unit,
    dnxc_data_property_t * property,
    char *val_str,
    char *buffer_property_val_str,
    int buffer_size)
{
    int map_index;
    SHR_FUNC_INIT_VARS(unit);

    
    buffer_property_val_str[0] = 0;
    switch (property->method)
    {
        case dnxc_data_property_method_direct_map:
        case dnxc_data_property_method_port_direct_map:
        case dnxc_data_property_method_suffix_direct_map:
        case dnxc_data_property_method_suffix_only_direct_map:
        case dnxc_data_property_method_port_suffix_direct_map:
            sal_snprintf(buffer_property_val_str, buffer_size, "%s", "invalid");
            for (map_index = 0; map_index < property->nof_mapping; map_index++)
            {
                if (property->mapping[map_index].is_invalid != TRUE)
                {
                    
                    if (property->mapping[map_index].val == sal_ctoi(val_str, NULL))
                    {
                        sal_snprintf(buffer_property_val_str, buffer_size, "%s", property->mapping[map_index].name);
                    }
                }
            }
            break;
        case dnxc_data_property_method_custom:
            sal_snprintf(buffer_property_val_str, buffer_size, "custom - see description");
            break;
        case dnxc_data_property_method_invalid:
            sal_snprintf(buffer_property_val_str, buffer_size, "%s", "invalid");
            break;
        default:
            sal_snprintf(buffer_property_val_str, buffer_size, "%s", val_str);
            break;
    }

 
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_utils_generic_property_val_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    char *buffer,
    int buffer_size)
{
    dnxc_data_property_t *property = NULL;
    dnxc_data_define_t *define_info;
    dnxc_data_feature_t *feature_info;
    dnxc_data_table_t *table_info;
    char val_str[DNXC_DATA_UTILS_STRING_MAX];
    int value_index;
    SHR_FUNC_INIT_VARS(unit);

        
    val_str[0] = 0;

        
    define_info = dnxc_data_utils_define_info_get(unit, module, submodule, data);
    if (define_info != NULL)
    {
        property = &define_info->property;
        sal_snprintf(val_str, DNXC_DATA_UTILS_STRING_MAX, "%d", define_info->data);
    }

        
    feature_info = dnxc_data_utils_feature_info_get(unit, module, submodule, data);
    if (feature_info != NULL)
    {
        property = &feature_info->property;
        sal_snprintf(val_str, DNXC_DATA_UTILS_STRING_MAX, "%d", feature_info->data);
    }

        
    table_info = dnxc_data_utils_table_info_get(unit, module, submodule, data);
    if (table_info != NULL)
    {
        if (member == NULL)
        {
            property = &table_info->property;
            sal_snprintf(val_str, DNXC_DATA_UTILS_STRING_MAX, "%s", table_info->values[0].default_val);
        }
        else
        {
            
            for (value_index = 0; value_index < table_info->nof_values; value_index++)
            {
                if (!sal_strncasecmp(table_info->values[value_index].name, member, DNXC_DATA_UTILS_STRING_MAX))
                {
                    property = &table_info->property;
                    sal_snprintf(val_str, DNXC_DATA_UTILS_STRING_MAX, "%s",
                                 table_info->values[value_index].default_val);
                }
            }
        }
    }

        
    if ((property != NULL) && (val_str[0] != 0))
    {
        SHR_IF_ERR_EXIT(dnxc_data_utils_property_val_get(unit, property, val_str, buffer, buffer_size));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_utils_generic_property_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    const dnxc_data_property_t ** property)
{
    dnxc_data_define_t *define_info;
    dnxc_data_feature_t *feature_info;
    dnxc_data_table_t *table_info;
    int value_index;
    SHR_FUNC_INIT_VARS(unit);

    *property = NULL;
    define_info = dnxc_data_utils_define_info_get(unit, module, submodule, data);
    feature_info = dnxc_data_utils_feature_info_get(unit, module, submodule, data);
    table_info = dnxc_data_utils_table_info_get(unit, module, submodule, data);

    if (define_info != NULL)
    {
        
        *property = &define_info->property;
    }
    else if (feature_info != NULL)
    {
        
        *property = &feature_info->property;
    }
    else if (table_info != NULL)
    {
        
        if (member == NULL)
        {
            *property = &table_info->property;
        }
        else
        {
            
            for (value_index = 0; value_index < table_info->nof_values; value_index++)
            {
                if (!sal_strncasecmp(table_info->values[value_index].name, member, DNXC_DATA_UTILS_STRING_MAX))
                {
                    *property = &table_info->values[value_index].property;
                    SHR_EXIT();
                }
            }
            SHR_ERR_EXIT(_SHR_E_PARAM, "Property for module:%s, submodule:%s, data:%s, member%s, wasn't found\n",
                         module, submodule, data, member);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Property for module:%s, submodule:%s, data:%s, member%s, wasn't found\n", module,
                     submodule, data, member);
    }

exit:
    SHR_FUNC_EXIT;
}

char *
dnxc_data_utils_generic_property_values_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    char *buffer,
    int buffer_size)
{
    const dnxc_data_property_t *property;
    shr_error_e rv;
    int map_index, buffer_offset = 0;

    buffer[0] = 0;
    rv = dnxc_data_utils_generic_property_get(unit, module, submodule, data, member, &property);
    if (rv == _SHR_E_NONE)
    {
        
        if (property->mapping != NULL)
        {
            buffer_offset += sal_snprintf(buffer + buffer_offset, buffer_size - buffer_offset, "%s", "mapping ");
            for (map_index = 0; map_index < property->nof_mapping; map_index++)
            {
                buffer_offset +=
                    sal_snprintf(buffer + buffer_offset, buffer_size - buffer_offset, "%s ",
                                 property->mapping[map_index].name);
            }
        }
        else if (property->range_min != 0 || property->range_max != 0)
        {
            buffer_offset += sal_snprintf(buffer + buffer_offset, buffer_size - buffer_offset, "%s ", "range");
            buffer_offset +=
                sal_snprintf(buffer + buffer_offset, buffer_size - buffer_offset, "%d ", property->range_min);
            buffer_offset +=
                sal_snprintf(buffer + buffer_offset, buffer_size - buffer_offset, "%d ", property->range_max);
        }
        else
        {
            buffer_offset += sal_snprintf(buffer + buffer_offset, buffer_size - buffer_offset, "%s", "unknown");
        }
    }

    return buffer;
}


int
dnxc_data_utils_table_size_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *table_name)
{
    return dnxc_data_utils_table_info_get(unit, module_name, submodule_name, table_name)->info_get.key_size[0];
}


