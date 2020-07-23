
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA


#include <soc/drv.h>

#include <sal/appl/sal.h>

#include <shared/pbmp.h>

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_dyn_loader.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/init/init_time_analyzer.h>
#endif

#include <bcm/types.h>
#include <soc/dnxc/dnxc_verify.h>




dnxc_data_t _dnxc_data[SOC_MAX_NUM_DEVICES] = { {0}
};




#ifdef DNXC_VERIFICATION

static shr_error_e
dnxc_data_mgmt_access_verify_internal(
    int unit,
    uint32 state_flags,
    uint32 data_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (((data_flags & DNXC_DATA_F_SUPPORTED) == 0) && ((data_flags & DNXC_DATA_F_FEATURE) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "data not supported for device\n");
    }

    
    if ((state_flags & DNXC_DATA_STATE_F_BCM_INIT_DONE) && (data_flags & DNXC_DATA_F_INIT_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "reading init only data after init is not allowed\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
#endif 

#ifdef BCM_DNX_SUPPORT

static int
dnxc_data_mgmt_is_port_active(
    int unit,
    int key,
    uint8 skip_check)
{
    const dnx_data_port_static_add_ucode_port_t *ucode_port;
    const dnxc_data_table_info_t *ucode_port_info;

    if (SOC_IS_RAMON(unit))
    {
        
        return TRUE;
    }

    if (skip_check)
    {
        
        return TRUE;
    }

    ucode_port_info = dnx_data_port.static_add.ucode_port_info_get(unit);
    if (key < ucode_port_info->key_size[0])
    {
        ucode_port = dnx_data_port.static_add.ucode_port_get(unit, key);
        if (ucode_port->interface != BCM_PORT_IF_NULL)
            return TRUE;
    }
    else
    {
        return TRUE;
    }

    return FALSE;
}

#else
static int
dnxc_data_mgmt_is_port_active(
    int unit,
    int key,
    uint8 skip_check)
{
    return TRUE;
}
#endif


shr_error_e
dnxc_data_mgmt_access_verify(
    int unit,
    uint32 state_flags,
    uint32 data_flags,
    dnxc_data_value_set_f set_func)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((state_flags & DNXC_DATA_STATE_F_INIT_DONE) == 0)
    {
        
    }
    else if ((state_flags & DNXC_DATA_STATE_F_DEV_DATA_DONE) == 0)
    {
        
        if (((data_flags & DNXC_DATA_F_SUPPORTED) == 0) && set_func != NULL)
        {
            SHR_IF_ERR_EXIT(set_func(unit));
        }
    }
    else
    {
        DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxc_data_mgmt_access_verify_internal(unit, state_flags, data_flags)));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_state_set(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    _dnxc_data[unit].state |= flags;

    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_state_reset(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    _dnxc_data[unit].state &= ~flags;

    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_deinit(
    int unit,
    dnxc_data_t * unit_data)
{
    int module_index;
    dnxc_data_module_t *module;

    int submodule_index;
    dnxc_data_submodule_t *submodule;

    int table_index, feature_index, define_index;
    dnxc_data_table_t *table;
    dnxc_data_feature_t *feature;
    dnxc_data_define_t *define;

    int value_index;
    SHR_FUNC_INIT_VARS(unit);

    
    if (unit_data->state != 0)
    {
        
        SHR_IF_ERR_CONT(dnxc_data_dyn_loader_parse_deinit(unit));

        
        for (module_index = 0; module_index < unit_data->nof_modules; module_index++)
        {
            module = &unit_data->modules[module_index];

            
            for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
            {
                submodule = &module->submodules[submodule_index];

                
                for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
                {
                    
                    feature = &submodule->features[feature_index];
                    DNXC_DATA_FREE(feature->property.mapping);
                }
                
                DNXC_DATA_FREE(submodule->features);

                
                for (define_index = 0; define_index < submodule->nof_defines; define_index++)
                {
                    
                    define = &submodule->defines[define_index];
                    DNXC_DATA_FREE(define->property.mapping);
                }
                
                DNXC_DATA_FREE(submodule->defines);

                
                for (table_index = 0; table_index < submodule->nof_tables; table_index++)
                {
                    table = &submodule->tables[table_index];

                    
                    for (value_index = 0; value_index < table->nof_values; value_index++)
                    {
                        
                        DNXC_DATA_FREE(table->values[value_index].property.mapping);
                    }
                    
                    DNXC_DATA_FREE(table->data);

                    
                    DNXC_DATA_FREE(table->property.mapping);

                    
                    DNXC_DATA_FREE(table->values);
                }
                
                DNXC_DATA_FREE(submodule->tables);
            }

            
            DNXC_DATA_FREE(module->submodules);
        }
        
        DNXC_DATA_FREE(unit_data->modules);

        
        sal_memset(&_dnxc_data[unit], 0, sizeof(dnxc_data_t));
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_values_set(
    int unit)
{
    int module_index;
    dnxc_data_module_t *module;

    int submodule_index;
    dnxc_data_submodule_t *submodule;

    int data_index;
    SHR_FUNC_INIT_VARS(unit);

    if (_dnxc_data[unit].state != 0)
    {

        
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            module = &_dnxc_data[unit].modules[module_index];

            for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
            {
                submodule = &module->submodules[submodule_index];

                
                for (data_index = 0; data_index < submodule->nof_defines; data_index++)
                {

                    
                    if (submodule->defines[data_index].flags & DNXC_DATA_F_NUMERIC)
                    {
                        continue;
                    }
                    if (submodule->defines[data_index].set == NULL)
                    {
                        continue;
                    }
                    
                    if (submodule->defines[data_index].flags & DNXC_DATA_F_SUPPORTED)
                    {
                        continue;
                    }

                    
                    SHR_IF_ERR_EXIT(submodule->defines[data_index].set(unit));
                }
            }
        }

        
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            module = &_dnxc_data[unit].modules[module_index];

            for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
            {
                submodule = &module->submodules[submodule_index];

                
                for (data_index = 0; data_index < submodule->nof_features; data_index++)
                {
                    if (submodule->features[data_index].set != NULL)
                    {
                        
                        if (submodule->features[data_index].flags & DNXC_DATA_F_SUPPORTED)
                        {
                            continue;
                        }
                        
                        SHR_IF_ERR_EXIT(submodule->features[data_index].set(unit));

                        
                        SHR_IF_ERR_EXIT(dnxc_data_mgmt_feature_changed_set(unit, &submodule->features[data_index]));
                    }
                }
            }
        }

        
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            module = &_dnxc_data[unit].modules[module_index];

            for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
            {
                submodule = &module->submodules[submodule_index];

                
                for (data_index = 0; data_index < submodule->nof_defines; data_index++)
                {

                    
                    if ((submodule->defines[data_index].flags & DNXC_DATA_F_NUMERIC) == 0)
                    {
                        continue;
                    }
                    if (submodule->defines[data_index].set == NULL)
                    {
                        continue;
                    }
                    
                    if (submodule->defines[data_index].flags & DNXC_DATA_F_SUPPORTED)
                    {
                        continue;
                    }

                    
                    SHR_IF_ERR_EXIT(submodule->defines[data_index].set(unit));

                    
                    SHR_IF_ERR_EXIT(dnxc_data_mgmt_define_changed_set(unit, &submodule->defines[data_index]));

                }
            }
        }

        
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            module = &_dnxc_data[unit].modules[module_index];

            for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
            {
                submodule = &module->submodules[submodule_index];

                
                for (data_index = 0; data_index < submodule->nof_tables; data_index++)
                {
                    if (submodule->tables[data_index].set != NULL)
                    {
                        
                        if (submodule->tables[data_index].flags & DNXC_DATA_F_SUPPORTED)
                        {
                            continue;
                        }
                        
                        SHR_IF_ERR_EXIT(submodule->tables[data_index].set(unit));

                        
                        SHR_IF_ERR_EXIT(dnxc_data_mgmt_table_changed_set(unit, &submodule->tables[data_index]));
                    }
                }
            }
        }
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_dev_id_verify(unit));

exit:
    SHR_FUNC_EXIT;
}

uint32
dnxc_data_mgmt_define_data_get(
    int unit,
    int module_index,
    int submodule_index,
    int define_index)
{
    shr_error_e rv = _SHR_E_NONE;
    dnxc_data_define_t *define;
#ifdef BCM_DNX_SUPPORT
    DNX_INIT_TIME_ANALYZER_DNX_DATA_START(unit, DNX_INIT_TIME_ANALYZER_DNX_DATA);
#endif
    
    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];

    
    rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, define->flags, define->set);
    if (rv != _SHR_E_NONE)
    {
        
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "DNX DATA - define %s access denied for device %s\n"), define->name,
                   _dnxc_data[unit].name));
        assert(0);
    }

#ifdef BCM_DNX_SUPPORT
    DNX_INIT_TIME_ANALYZER_DNX_DATA_STOP(unit, DNX_INIT_TIME_ANALYZER_DNX_DATA);
#endif
    
    return define->data;

}

int
dnxc_data_mgmt_feature_data_get(
    int unit,
    int module_index,
    int submodule_index,
    int feature_index)
{
    shr_error_e rv;
    dnxc_data_feature_t *feature;
#ifdef BCM_DNX_SUPPORT
    DNX_INIT_TIME_ANALYZER_DNX_DATA_START(unit, DNX_INIT_TIME_ANALYZER_DNX_DATA);
#endif
    
    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];

    
    rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, feature->flags, feature->set);

#ifdef BCM_DNX_SUPPORT
    DNX_INIT_TIME_ANALYZER_DNX_DATA_STOP(unit, DNX_INIT_TIME_ANALYZER_DNX_DATA);
#endif

    if (rv == _SHR_E_NONE)
    {
        
        return feature->data;
    }

    
    return 0;
}

dnxc_data_table_t *
dnxc_data_mgmt_table_get(
    int unit,
    int module_index,
    int submodule_index,
    int table_index)
{
    
    return &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
}

const dnxc_data_table_info_t *
dnxc_data_mgmt_table_info_get(
    int unit,
    int module_index,
    int submodule_index,
    int table_index)
{
    shr_error_e rv = _SHR_E_NONE;
    dnxc_data_table_t *table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];

    
    rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, table->flags, table->set);
    if (rv != _SHR_E_NONE)
    {
        
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "DNX DATA - table %s access denied for device %s\n"), table->name,
                   _dnxc_data[unit].name));
        assert(0);
    }

    
    return &table->info_get;
}

static char *
dnxc_data_mgmt_table_data_internal_get(
    int unit,
    dnxc_data_table_t * table,
    int key0,
    int key1,
    int verify)
{
    shr_error_e rv = _SHR_E_NONE;
    int key0_size, key1_size, nof_elements;

    
    if (verify)
    {
        rv = dnxc_data_mgmt_access_verify(unit, _dnxc_data[unit].state, table->flags, table->set);
    }
    if (rv == _SHR_E_NONE)
    {
        if (table->key_map != NULL)
        {
            
            rv = table->key_map(unit, key0, key1, &key0, &key1);
            assert(rv == _SHR_E_NONE);
        }
        
        if (key0 == -1 || key1 == -1)
        {
            if ((table->nof_keys > 0 && table->keys[0].size == 0) || (table->nof_keys > 1 && table->keys[1].size == 0))
            {
                nof_elements = 0;
            }
            else
            {
                key0_size = table->keys[0].size != 0 ? table->keys[0].size : 1;
                key1_size = table->keys[1].size != 0 ? table->keys[1].size : 1;
                nof_elements = key0_size * key1_size;
            }
            return table->data + nof_elements * table->size_of_values;
        }
        
        if ((key0 >= 0 && key0 < table->keys[0].size) || (table->nof_keys == 0 && key0 == 0))
        {
            if ((key1 >= 0 && key1 < table->keys[1].size) || (table->nof_keys <= 1 && key1 == 0))
            {
                
                return table->data + key0 * table->size_of_values + key1 * table->keys[0].size * table->size_of_values;
            }
            else
            {
                
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DNX DATA - out of bound key - table %s\n"), table->name));
                assert(0);
            }
        }
        else
        {
            
            if (table->keys[0].size > 0)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - out of bound key - table %s, key=%d, max=%d\n"), table->name,
                           key0, table->keys[0].size - 1));
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "DNX DATA - out of bound key - table %s, key=%d - Zero sized table \n"),
                           table->name, key0));
            }

            assert(0);
        }
    }
    else
    {
        
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "DNX DATA - table %s access denied for device %s\n"), table->name,
                   _dnxc_data[unit].name));
        assert(0);
    }

    
    return NULL;
}
char *
dnxc_data_mgmt_table_data_get(
    int unit,
    dnxc_data_table_t * table,
    int key0,
    int key1)
{
    char *data;
#ifdef BCM_DNX_SUPPORT
    DNX_INIT_TIME_ANALYZER_DNX_DATA_START(unit, DNX_INIT_TIME_ANALYZER_DNX_DATA);
#endif

    data = dnxc_data_mgmt_table_data_internal_get(unit, table, key0, key1, 1);

#ifdef BCM_DNX_SUPPORT
    DNX_INIT_TIME_ANALYZER_DNX_DATA_STOP(unit, DNX_INIT_TIME_ANALYZER_DNX_DATA);
#endif

    return data;
}
char *
dnxc_data_mgmt_table_data_diag_get(
    int unit,
    dnxc_data_table_t * table,
    int key0,
    int key1)
{
    return dnxc_data_mgmt_table_data_internal_get(unit, table, key0, key1, 0);
}


shr_error_e
dnxc_data_mgmt_property_read(
    int unit,
    dnxc_data_property_t * property,
    int key,
    void *value)
{
    int value_index, found = 0;
    uint32 val;
    uint32 *value_uint32;
    int *value_int;
    int val_int, range_min_int, range_max_int;
    bcm_pbmp_t *value_pbmp;
    char *val_str;
    char **value_str;

    int res;
    SHR_FUNC_INIT_VARS(unit);

    
    switch (property->method)
    {
        case dnxc_data_property_method_enable:
        {
            value_uint32 = (uint32 *) value;
            
            
            val = soc_property_get(unit, property->name, *value_uint32);
            
            if (val != 0 && val != 1 && val != *value_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Enable read method] SoC property %s - only 0 or 1 are allowed - got %d \n",
                             property->name, val);
            }
            *value_uint32 = val;
            break;
        }

        case dnxc_data_property_method_port_enable:
        {
            value_uint32 = (uint32 *) value;

            

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val = soc_property_port_get(unit, key, property->name, *value_uint32);
            
            if (val != 0 && val != 1 && val != *value_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Port enable read method] SoC property %s  port %d - only 0 or 1 are allowed - got %d \n",
                             property->name, key, val);
            }
            *value_uint32 = val;
            break;
        }

        case dnxc_data_property_method_suffix_enable:
        {
            value_uint32 = (uint32 *) value;
            

            
            val = soc_property_suffix_num_get(unit, key, property->name, property->suffix, *value_uint32);
            
            if (val != 0 && val != 1 && val != *value_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Suffix enable read method] SoC property %s - suffix %s num %d - only 0 or 1 are allowed - got %d \n",
                             property->name, property->suffix, key, val);
            }
            *value_uint32 = val;
            break;
        }
        case dnxc_data_property_method_suffix_only_enable:
        {
            value_uint32 = (uint32 *) value;
            

            
            val = soc_property_suffix_num_get_only_suffix(unit, key, property->name, property->suffix, *value_uint32);
            
            if (val != 0 && val != 1 && val != *value_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Suffix enable read method] SoC property %s - suffix %s num %d - only 0 or 1 are allowed - got %d \n",
                             property->name, property->suffix, key, val);
            }
            *value_uint32 = val;
            break;
        }
        case dnxc_data_property_method_port_suffix_enable:
        {
            value_uint32 = (uint32 *) value;
            

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val = soc_property_port_suffix_num_get(unit, key, -1, property->name, property->suffix, *value_uint32);
            
            if (val != 0 && val != 1 && val != *value_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Suffix enable read method] SoC property %s - suffix %s num %d - only 0 or 1 are allowed - got %d \n",
                             property->name, property->suffix, key, val);
            }
            *value_uint32 = val;
            break;
        }

        case dnxc_data_property_method_range:
        {
            value_uint32 = (uint32 *) value;
            

            
            val = soc_property_get(unit, property->name, *value_uint32);
            
            if ((val < property->range_min || val > property->range_max) && (val != *value_uint32))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range read method] SoC property %s - [%u - %u] is allowed - got %d \n",
                             property->name, property->range_min, property->range_max, val);
            }
            *value_uint32 = val;
            break;
        }
        case dnxc_data_property_method_range_signed:
        {
            value_int = (int *) value;
            

            
            val_int = soc_property_get(unit, property->name, *value_int);
            range_min_int = (int) property->range_min;
            range_max_int = (int) property->range_max;
            
            if ((val_int < range_min_int || val_int > range_max_int) && (val_int != *value_int))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range read method] SoC property %s - [%d - %d] is allowed - got %d \n",
                             property->name, range_min_int, range_max_int, val_int);
            }
            *value_int = val_int;
            break;
        }

        case dnxc_data_property_method_port_range:
        {
            value_uint32 = (uint32 *) value;
            

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val = soc_property_port_get(unit, key, property->name, *value_uint32);
            
            if ((val < property->range_min || val > property->range_max) && (val != *value_uint32))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range port read method] SoC property %s port %d - [%u - %u] is allowed - got %d \n",
                             property->name, key, property->range_min, property->range_max, val);
            }
            *value_uint32 = val;
            break;
        }

        case dnxc_data_property_method_port_range_signed:
        {
            value_int = (int *) value;
            

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }
            
            val_int = soc_property_port_get(unit, key, property->name, *value_int);
            range_min_int = (int) property->range_min;
            range_max_int = (int) property->range_max;
            
            if ((val_int < range_min_int || val_int > range_max_int) && (val_int != *value_int))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range port read method] SoC property %s port %d - [%d - %d] is allowed - got %d \n",
                             property->name, key, range_min_int, range_max_int, val_int);
            }
            *value_int = val_int;
            break;
        }

        case dnxc_data_property_method_suffix_range:
        {
            
            value_uint32 = (uint32 *) value;

            
            val = soc_property_suffix_num_get(unit, key, property->name, property->suffix, *value_uint32);
            
            if ((val < property->range_min || val > property->range_max) && (val != *value_uint32))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range suffix read method] SoC property %s suffix %s num %d - [%u - %u] is allowed - got %d \n",
                             property->name, property->suffix, key, property->range_min, property->range_max, val);
            }
            *value_uint32 = val;
            break;
        }
        case dnxc_data_property_method_suffix_only_range:
        {
            
            value_uint32 = (uint32 *) value;

            
            val = soc_property_suffix_num_get_only_suffix(unit, key, property->name, property->suffix, *value_uint32);
            
            if ((val < property->range_min || val > property->range_max) && (val != *value_uint32))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range suffix read method] SoC property %s suffix %s num %d - [%u - %u] is allowed - got %d \n",
                             property->name, property->suffix, key, property->range_min, property->range_max, val);
            }
            *value_uint32 = val;
            break;
        }
        case dnxc_data_property_method_suffix_range_signed:
        {
            
            value_int = (int *) value;
            range_min_int = (int) property->range_min;
            range_max_int = (int) property->range_max;

            
            val_int = soc_property_suffix_num_get(unit, key, property->name, property->suffix, *value_int);
            
            if ((val_int < range_min_int || val_int > range_max_int) && (val_int != *value_int))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range suffix read method] SoC property %s suffix %s num %d - [%d - %d] is allowed - got %d \n",
                             property->name, property->suffix, key, range_min_int, range_max_int, val_int);
            }
            *value_int = val_int;
            break;
        }
        case dnxc_data_property_method_suffix_only_range_signed:
        {
            
            value_int = (int *) value;
            range_min_int = (int) property->range_min;
            range_max_int = (int) property->range_max;

            
            val_int = soc_property_suffix_num_get_only_suffix(unit, key, property->name, property->suffix, *value_int);
            
            if ((val_int < range_min_int || val_int > range_max_int) && (val_int != *value_int))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range suffix read method] SoC property %s suffix %s num %d - [%d - %d] is allowed - got %d \n",
                             property->name, property->suffix, key, range_min_int, range_max_int, val_int);
            }
            *value_int = val_int;
            break;
        }
        case dnxc_data_property_method_port_suffix_range:
        {
            
            value_uint32 = (uint32 *) value;

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val = soc_property_port_suffix_num_get(unit, key, -1, property->name, property->suffix, *value_uint32);
            
            if ((val < property->range_min || val > property->range_max) && (val != *value_uint32))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range suffix read method] SoC property %s suffix %s num %d - [%u - %u] is allowed - got %d \n",
                             property->name, property->suffix, key, property->range_min, property->range_max, val);
            }
            *value_uint32 = val;
            break;
        }

        case dnxc_data_property_method_port_suffix_range_signed:
        {
            
            value_int = (int *) value;
            range_min_int = (int) property->range_min;
            range_max_int = (int) property->range_max;

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val_int = soc_property_port_suffix_num_get(unit, key, -1, property->name, property->suffix, *value_int);
            
            if ((val_int < range_min_int || val_int > range_max_int) && (val_int != *value_int))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "[Range suffix read method] SoC property %s suffix %s num %d - [%d - %d] is allowed - got %d \n",
                             property->name, property->suffix, key, range_min_int, range_max_int, val_int);
            }
            *value_int = val_int;
            break;
        }

        case dnxc_data_property_method_direct_map:
        {
            
            value_uint32 = (uint32 *) value;

            
            val_str = soc_property_get_str(unit, property->name);
            
            if (val_str == NULL)
            {
                
            }
            else
            {
                
                for (value_index = 0; value_index < property->nof_mapping; value_index++)
                {
                    
                    if (property->mapping[value_index].is_invalid == TRUE)
                    {
                        continue;
                    }

                    if (!sal_strncasecmp(property->mapping[value_index].name, val_str, DNXC_DATA_UTILS_STRING_MAX))
                    {
                        *value_uint32 = property->mapping[value_index].val;;
                        found = 1;
                    }
                }
                if (!found)
                {
                    char buffer[DNXC_DATA_UTILS_STRING_MAX];
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, property, buffer, DNXC_DATA_UTILS_STRING_MAX, 0));

                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "[direct map method] SoC property %s - %s is not a valid value\n"
                                 "Supported values:\n%s\n", property->name, val_str, buffer);

                }
            }
            break;
        }

        case dnxc_data_property_method_port_direct_map:
        {
            
            value_uint32 = (uint32 *) value;

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val_str = soc_property_port_get_str(unit, key, property->name);
            
            if (val_str == NULL)
            {
                
            }
            else
            {
                
                for (value_index = 0; value_index < property->nof_mapping; value_index++)
                {
                    
                    if (property->mapping[value_index].is_invalid == TRUE)
                    {
                        continue;
                    }

                    if (!sal_strncasecmp(property->mapping[value_index].name, val_str, DNXC_DATA_UTILS_STRING_MAX))
                    {
                        *value_uint32 = property->mapping[value_index].val;
                        found = 1;
                    }
                }
                if (!found)
                {
                    char buffer[DNXC_DATA_UTILS_STRING_MAX];
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, property, buffer, DNXC_DATA_UTILS_STRING_MAX, 0));

                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "[port direct map method] SoC property %s port %d - %s is not a valid value\n"
                                 "Supported values:\n%s\n", property->name, key, val_str, buffer);

                }
            }
            break;
        }

        case dnxc_data_property_method_suffix_direct_map:
        {
            
            value_uint32 = (uint32 *) value;

            
            val_str = soc_property_suffix_num_str_get(unit, key, property->name, property->suffix);
            
            if (val_str == NULL)
            {
                
            }
            else
            {
                
                for (value_index = 0; value_index < property->nof_mapping; value_index++)
                {

                    
                    if (property->mapping[value_index].is_invalid == TRUE)
                    {
                        continue;
                    }

                    if (!sal_strncasecmp(property->mapping[value_index].name, val_str, DNXC_DATA_UTILS_STRING_MAX))
                    {
                        *value_uint32 = property->mapping[value_index].val;
                        found = 1;
                    }
                }
                if (!found)
                {
                    char buffer[DNXC_DATA_UTILS_STRING_MAX];
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, property, buffer, DNXC_DATA_UTILS_STRING_MAX, 0));

                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "[suffix direct map method] SoC property %s suffix %s num %d- %s is not a valid value\n"
                                 "Supported values:\n%s\n", property->name, property->suffix, key, val_str, buffer);

                }
            }
            break;
        }
        case dnxc_data_property_method_suffix_only_direct_map:
        {
            
            value_uint32 = (uint32 *) value;

            
            val_str = soc_property_suffix_num_only_suffix_str_get(unit, key, property->name, property->suffix);
            
            if (val_str == NULL)
            {
                
            }
            else
            {
                
                for (value_index = 0; value_index < property->nof_mapping; value_index++)
                {

                            
                    if (property->mapping[value_index].is_invalid == TRUE)
                    {
                        continue;
                    }

                    if (!sal_strncasecmp(property->mapping[value_index].name, val_str, DNXC_DATA_UTILS_STRING_MAX))
                    {
                        *value_uint32 = property->mapping[value_index].val;
                        found = 1;
                    }
                }
                if (!found)
                {
                    char buffer[DNXC_DATA_UTILS_STRING_MAX];
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, property, buffer, DNXC_DATA_UTILS_STRING_MAX, 0));

                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "[suffix only direct map method] SoC property %s suffix %s num %d- %s is not a valid value\n"
                                 "Supported values:\n%s\n", property->name, property->suffix, key, val_str, buffer);

                }
            }
            break;
        }
        case dnxc_data_property_method_port_suffix_direct_map:
        {
            
            value_uint32 = (uint32 *) value;

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val_str = soc_property_port_suffix_num_get_str(unit, key, -1, property->name, property->suffix);
            
            if (val_str == NULL)
            {
                
            }
            else
            {
                
                for (value_index = 0; value_index < property->nof_mapping; value_index++)
                {

                   
                    if (property->mapping[value_index].is_invalid == TRUE)
                    {
                        continue;
                    }

                    if (!sal_strncasecmp(property->mapping[value_index].name, val_str, DNXC_DATA_UTILS_STRING_MAX))
                    {
                        *value_uint32 = property->mapping[value_index].val;
                        found = 1;
                    }
                }
                if (!found)
                {
                    char buffer[DNXC_DATA_UTILS_STRING_MAX];
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, property, buffer, DNXC_DATA_UTILS_STRING_MAX, 0));

                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "[suffix direct map method] SoC property %s suffix %s num %d- %s is not a valid value\n"
                                 "Supported values:\n%s\n", property->name, property->suffix, key, val_str, buffer);

                }
            }
            break;
        }
            
        case dnxc_data_property_method_pbmp:
        {
            value_pbmp = (bcm_pbmp_t *) value;
            
            val_str = soc_property_get_str(unit, property->name);

            if (val_str == NULL)
            {
                
            }
            else
            {
                
                res = _shr_pbmp_decode(val_str, value_pbmp);
                if (res < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Property %s - Expected PBMP got %s\n", property->name, val_str);
                }
            }
            break;
        }
            
        case dnxc_data_property_method_port_pbmp:
        {
            value_pbmp = (bcm_pbmp_t *) value;
            
            val_str = soc_property_port_get_str(unit, key, property->name);

            if (val_str == NULL)
            {
                
            }
            else
            {
                
                res = _shr_pbmp_decode(val_str, value_pbmp);
                if (res < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Property %s Port %d - Expected PBMP got %s\n", property->name, key, val_str);
                }
            }
            break;
        }
            
        case dnxc_data_property_method_suffix_pbmp:
        {
            value_pbmp = (bcm_pbmp_t *) value;
            
            val_str = soc_property_suffix_num_str_get(unit, key, property->name, property->suffix);

            if (val_str == NULL)
            {
                
            }
            else
            {
                
                res = _shr_pbmp_decode(val_str, value_pbmp);
                if (res < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Property %s Key %d Suffix %s - Expected PBMP got %s\n",
                                 property->name, key, property->suffix, val_str);
                }
            }
            break;
        }
            
        case dnxc_data_property_method_str:
        {
            value_str = (char **) value;
            
            val_str = soc_property_get_str(unit, property->name);

            if (val_str == NULL)
            {
                
            }
            else
            {
                *value_str = val_str;
            }
            break;
        }
            
        case dnxc_data_property_method_port_str:
        {
            value_str = (char **) value;

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val_str = soc_property_port_get_str(unit, key, property->name);

            if (val_str == NULL)
            {
                
            }
            else
            {
                *value_str = val_str;
            }
            break;
        }
            
        case dnxc_data_property_method_suffix_str:
        {
            value_str = (char **) value;
            
            val_str = soc_property_suffix_num_str_get(unit, key, property->name, property->suffix);

            if (val_str == NULL)
            {
                
            }
            else
            {
                *value_str = val_str;
            }
            break;
        }

            
        case dnxc_data_property_method_port_suffix_str:
        {
            value_str = (char **) value;

            
            if (!dnxc_data_mgmt_is_port_active(unit, key, property->skip_is_active_check))
            {
                break;
            }

            
            val_str = soc_property_port_suffix_num_get_str(unit, key, -1, property->name, property->suffix);

            if (val_str == NULL)
            {
                
            }
            else
            {
                *value_str = val_str;
            }
            break;
        }

        default:
        {
            

            SHR_ERR_EXIT(_SHR_E_PARAM, "SoC property read method not supported %d \n", property->method);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_feature_changed_set(
    int unit,
    dnxc_data_feature_t * feature)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (feature->data != feature->default_data)
    {
        feature->flags |= DNXC_DATA_F_CHANGED;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_define_changed_set(
    int unit,
    dnxc_data_define_t * define)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (define->data != define->default_data)
    {
        define->flags |= DNXC_DATA_F_CHANGED;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_table_changed_set(
    int unit,
    dnxc_data_table_t * table)
{
    char *default_val;
    char *val;
    int changed;
    int key0, key1;
    SHR_FUNC_INIT_VARS(unit);

    
    default_val = dnxc_data_mgmt_table_data_get(unit, table, -1, -1);

    
    for (key0 = 0; key0 < table->keys[0].size || (table->nof_keys == 0 && key0 == 0); key0++)
    {
        for (key1 = 0; key1 < table->keys[1].size || (table->nof_keys <= 1 && key1 == 0); key1++)
        {
            val = dnxc_data_mgmt_table_data_get(unit, table, key0, key1);
            changed = sal_memcmp(default_val, val, table->size_of_values);
            if (changed)
            {
                table->flags |= DNXC_DATA_F_CHANGED;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_mgmt_table_entry_changed_get(
    int unit,
    dnxc_data_table_t * table,
    int key0,
    int key1,
    int *changed)
{
    char *default_val;
    char *val;
    SHR_FUNC_INIT_VARS(unit);

    
    default_val = dnxc_data_mgmt_table_data_diag_get(unit, table, -1, -1);

    
    val = dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);

    
    *changed = sal_memcmp(default_val, val, table->size_of_values);

    SHR_FUNC_EXIT;
}

int
dnxc_data_mgmt_is_jr2_a0(
    int unit)
{
    return SOC_IS_JERICHO2_A0(unit);
}

int
dnxc_data_mgmt_is_jr2_b0(
    int unit)
{
    return SOC_IS_JERICHO2_B0_ONLY(unit);
}

int
dnxc_data_mgmt_is_jr2_b1(
    int unit)
{
    return SOC_IS_JERICHO2_B1_ONLY(unit);
}

int
dnxc_data_mgmt_is_j2c_a0(
    int unit)
{
    return SOC_IS_J2C_A0(unit);
}

int
dnxc_data_mgmt_is_j2c_a1(
    int unit)
{
    return SOC_IS_J2C_A1(unit);
}

int
dnxc_data_mgmt_is_q2a_a0(
    int unit)
{
    return SOC_IS_Q2A_A0(unit);
}
int
dnxc_data_mgmt_is_q2a_b0(
    int unit)
{
    return SOC_IS_Q2A_B0(unit);
}

int
dnxc_data_mgmt_is_q2a_b1(
    int unit)
{
    return SOC_IS_Q2A_B1(unit);
}

int
dnxc_data_mgmt_is_j2p_a0(
    int unit)
{
    return SOC_IS_J2P(unit);
}

int
dnxc_data_mgmt_is_ramon(
    int unit)
{
    return SOC_IS_RAMON(unit);
}


