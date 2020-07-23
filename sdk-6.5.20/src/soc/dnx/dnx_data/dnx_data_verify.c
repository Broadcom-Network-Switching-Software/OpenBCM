

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA


#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <sal/appl/sal.h>
#include <soc/drv.h> 



shr_error_e
dnx_data_property_unsupported_verify(
    int unit)
{
    const dnxc_data_table_info_t *table_info;
    const dnx_data_dev_init_properties_unsupported_t *prop_info;
    int property_index;
    char *val = NULL;
    bcm_port_t logical_port;
    int num_index;
    SHR_FUNC_INIT_VARS(unit);

    
    table_info = dnx_data_dev_init.properties.unsupported_info_get(unit);

    
    for (property_index = 0; property_index < table_info->key_size[0]; property_index++)
    {
        prop_info = dnx_data_dev_init.properties.unsupported_get(unit, property_index);

        
        if (prop_info->per_port)
        {
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                val = soc_property_port_get_str(unit, logical_port, prop_info->property);
                if (val != NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "soc property is not supported: name: %s port: %d\n %s\n",
                                 prop_info->property, logical_port,
                                 prop_info->err_msg != NULL ? prop_info->err_msg : "");
                }
            }
        }
        
        else if (prop_info->suffix != NULL || prop_info->num_max != -1)
        {
            
            for (num_index = -1; num_index <= prop_info->num_max; num_index++)
            {
                val =
                    soc_property_suffix_num_str_get(unit, num_index, prop_info->property,
                                                    prop_info->suffix != NULL ? prop_info->suffix : "");
                if (val != NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "soc property is not supported: name: %s suffix: %s\n %s\n",
                                 prop_info->property, prop_info->suffix != NULL ? prop_info->suffix : "",
                                 prop_info->err_msg != NULL ? prop_info->err_msg : "");
                }
            }
        }
        
        else
        {
            val = soc_property_get_str(unit, prop_info->property);
            if (val != NULL)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "soc property is not supported: name: %s \n %s\n",
                             prop_info->property, prop_info->err_msg != NULL ? prop_info->err_msg : "");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

