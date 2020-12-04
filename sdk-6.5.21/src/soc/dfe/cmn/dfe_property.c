/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_property.h>

void
soc_dfe_check_soc_property(int unit,char* soc_property_name,int* is_supported,soc_dfe_property_info_t* soc_property_info)
{
    soc_dfe_property_info_t* soc_dfe_property_info_array = NULL;
    int i=0;
    *is_supported=0;

    MBCM_DFE_DRIVER_CALL_VOID(unit,mbcm_dfe_get_soc_properties_array,(unit,&soc_dfe_property_info_array));

    if (soc_dfe_property_info_array != NULL)
    {
        while (soc_dfe_property_info_array[i].str != NULL) 
        {
            if (sal_strcmp(soc_dfe_property_info_array[i].str,soc_property_name)==0)
            {
                *is_supported=1;
                *soc_property_info=soc_dfe_property_info_array[i];
            }
            i++;
        }
    }
}

char*
soc_dfe_property_get_str(int unit,char* soc_property_name, int force_use_default, char* soc_prop_default)
{
    return soc_dfe_property_port_get_str(unit, soc_property_name, -1, force_use_default, soc_prop_default);
}

char*
soc_dfe_property_port_get_str(int unit,char* soc_property_name, soc_port_t port, int force_use_default, char* soc_prop_default)
{
    char* str;
    char* rv;
    int is_soc_property_supported;
    soc_dfe_property_info_t soc_property_info;

    soc_dfe_check_soc_property(unit,soc_property_name ,&is_soc_property_supported,&soc_property_info);
    if (is_soc_property_supported)
    {
        if (port == -1)
        {
            str = soc_property_get_str(unit,soc_property_name);
        } else {
            str = soc_property_port_get_str(unit, port, soc_property_name);
        }
        if (str == NULL)
        {
            if (force_use_default)
            {
                rv = soc_prop_default;
            } else {
                if (soc_property_info.def_type==SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING)
                {
                    rv = soc_property_info.def_str;
                } else {
                    rv = NULL;
                }
            }

        } else {
            rv = str;
        }
    } else {
        rv = NULL;
    }
    if (port == -1)
    {
         LOG_DEBUG(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "unit %d: %s=%s\n"), unit, soc_property_name,(rv!=NULL)? rv:"NULL"));
    } else {
         LOG_DEBUG(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "unit %d: %s_%d=%s\n"), unit, soc_property_name, port, (rv!=NULL)? rv:"NULL"));
    }
    return rv;


}

int
soc_dfe_property_port_get(int unit,  char* soc_property_name, soc_port_t port, int force_use_default, int soc_prop_default)
{
    int is_soc_property_supported;
    soc_dfe_property_info_t soc_property_info;
    int rv;

    soc_dfe_check_soc_property(unit,soc_property_name ,&is_soc_property_supported,&soc_property_info);
    if (is_soc_property_supported)
    {
        if (force_use_default)
        {
            rv = soc_property_port_get(unit, port, soc_property_name,soc_prop_default);
        } else {
            if (soc_property_info.def_type==SOC_DFE_PROPERTY_DEFAULT_TYPE_INT)
            {
                rv = soc_property_port_get(unit, port, soc_property_name, soc_property_info.def_int);
            } else {
                rv = SOC_DFE_PROPERTY_UNAVAIL;
            }
        }
    } else {
        rv = SOC_DFE_PROPERTY_UNAVAIL;
    }
     LOG_DEBUG(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "unit %d, port %d: %s=%d\n"),unit, port, soc_property_name,rv));
    return rv;

}


int
soc_dfe_property_get(int unit,char* soc_property_name,int force_use_default, int soc_prop_default)
{
    int is_soc_property_supported;
    soc_dfe_property_info_t soc_property_info;
    int rv;

    soc_dfe_check_soc_property(unit,soc_property_name ,&is_soc_property_supported,&soc_property_info);
    if (is_soc_property_supported)
    {
        if (force_use_default)
        {
            rv = soc_property_get(unit, soc_property_name,soc_prop_default);
        } else {
            if (soc_property_info.def_type==SOC_DFE_PROPERTY_DEFAULT_TYPE_INT)
            {
                rv = soc_property_get(unit,soc_property_name,soc_property_info.def_int);
            } else {
                rv = SOC_DFE_PROPERTY_UNAVAIL;
            }
        }
    } else {
        rv = SOC_DFE_PROPERTY_UNAVAIL;
    }
     LOG_DEBUG(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "unit %d: %s=%d\n"),unit, soc_property_name,rv));
    return rv;

}

int
soc_dfe_property_suffix_num_get(int unit,int num, char* soc_property_name,const char* suffix, int force_use_default, int soc_prop_default)
{
    int is_soc_property_supported;
    soc_dfe_property_info_t soc_property_info;
    int rv;

    soc_dfe_check_soc_property(unit,soc_property_name,&is_soc_property_supported,&soc_property_info);
    if (is_soc_property_supported)
    {

        if (force_use_default)
        {
            rv= soc_property_suffix_num_get(unit,num,soc_property_name,suffix,soc_prop_default);
        }
        
        else
        {
            rv=soc_property_suffix_num_get(unit,num,soc_property_name,suffix,soc_property_info.def_int);
        }
    }
    else
    {
        rv= SOC_DFE_PROPERTY_UNAVAIL;
    }

     LOG_DEBUG(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "unit %d: %s_%s_%d=%d\n"),unit, soc_property_name,suffix,num,rv));
    return rv;  
}

int
soc_dfe_property_suffix_num_get_only_suffix(int unit, int num, char* soc_property_name, const char* suffix, int force_use_default, int soc_prop_default)
{
    int is_soc_property_supported;
    soc_dfe_property_info_t soc_property_info;
    int rv;

    soc_dfe_check_soc_property(unit,soc_property_name,&is_soc_property_supported,&soc_property_info);
    if (is_soc_property_supported)
    {

        if (force_use_default)
        {
            rv= soc_property_suffix_num_get_only_suffix(unit,num,soc_property_name,suffix,soc_prop_default);
        }
        
        else
        {
            rv=soc_property_suffix_num_get_only_suffix(unit,num,soc_property_name,suffix,soc_property_info.def_int);
        }
    }
    else
    {
        rv= SOC_DFE_PROPERTY_UNAVAIL;
    }

     LOG_DEBUG(BSL_LS_SOC_INIT,
                   (BSL_META_U(unit,
                               "unit %d: %s_%s_%d=%d\n"),unit, soc_property_name,suffix,num,rv));
    return rv;  
}


soc_error_t
soc_dfe_property_str_to_enum(int unit, char *soc_property_name, const soc_dfe_property_str_enum_t *property_info, char *str_val, int *int_val)
{
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    *int_val = 0;

    
    for (i=0; property_info[i].str != NULL; i++)
    {
        if (str_val != NULL)
        {
            if (sal_strcmp(str_val, property_info[i].str) == 0)
            {
                *int_val = property_info[i].enum_val;
                SOC_EXIT;
            }
        }
    }

    
    if (str_val == NULL)
    {
        
        *int_val = property_info[i].enum_val;
    } else {
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s has unrecognized value %s"),soc_property_name, str_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t 
soc_dfe_property_enum_to_str(int unit, char *soc_property_name, const soc_dfe_property_str_enum_t *property_info, int int_val, char **str_val)
{
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    
    for (i=0; property_info[i].str != NULL; i++)
    {
            if (int_val == property_info[i].enum_val)
            {
                *str_val = property_info[i].str;
                SOC_EXIT;
            }
    }

    
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s has unrecognized value enum %d"),soc_property_name, int_val));

exit:
    SOCDNX_FUNC_RETURN;
}







const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_device_mode[] = {
    {soc_dfe_fabric_device_mode_single_stage_fe2,   "SINGLE_STAGE_FE2"},
    {soc_dfe_fabric_device_mode_multi_stage_fe2,    "MULTI_STAGE_FE2"},
    {soc_dfe_fabric_device_mode_multi_stage_fe13,   "MULTI_STAGE_FE13"},
    {soc_dfe_fabric_device_mode_repeater,           "REPEATER"},
    {soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric, "MULTI_STAGE_FE13_ASYMMETRICAL"},
    {DFE_FABRIC_DEVICE_MODE_DEFAULT,   NULL}
};


const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_multicast_mode[] = {
    {soc_dfe_multicast_mode_direct,     "DIRECT"},
    {soc_dfe_multicast_mode_indirect,   "INDIRECT"},
    {DFE_FABRIC_MULTICAST_MODE_DEFAULT, NULL}
};

const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_backplane_serdes_encoding[] = {
    {soc_dcmn_port_pcs_8_9_legacy_fec,       "0"},
    {soc_dcmn_port_pcs_8_10,                 "1"},
    {soc_dcmn_port_pcs_64_66_fec,            "2"},
    {soc_dcmn_port_pcs_64_66_bec,            "3"},
    {soc_dcmn_port_pcs_64_66,                "4"},
    {soc_dcmn_port_pcs_64_66_rs_fec,         "5"},
    {soc_dcmn_port_pcs_64_66_ll_rs_fec,      "6"},
    {soc_dcmn_port_pcs_64_66_fec,            "KR_FEC"},
    {soc_dcmn_port_pcs_64_66,                "64_66"},
    {soc_dcmn_port_pcs_64_66_rs_fec,         "RS_FEC"},
    {soc_dcmn_port_pcs_64_66_ll_rs_fec,      "LL_RS_FEC"},
    {SOC_DFE_PROPERTY_UNAVAIL,              NULL}                 
};


const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_load_balancing_mode[] = {
    {soc_dfe_load_balancing_mode_normal,                    "NORMAL_LOAD_BALANCE"},
    {soc_dfe_load_balancing_mode_destination_unreachable,   "DESTINATION_UNREACHABLE"},
    {soc_dfe_load_balancing_mode_balanced_input,            "BALANCED_INPUT"},
    {DFE_FABRIC_LOAD_BALANCING_MODE_DEFAULT,                NULL}
};


const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_cell_format[] = {
    {soc_dfe_fabric_link_cell_size_VSC128,                "VSC128"},
    {soc_dfe_fabric_link_cell_size_VSC256_V1,             "VSC256"}, 
    {SOC_DFE_PROPERTY_UNAVAIL,                            NULL}
};


const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_tdm_priority_min[] = {
    {0,                                                 "0"},
    {1,                                                 "1"},
    {2,                                                 "2"},
    {3,                                                 "3"}, 
    {SOC_DFE_FABRIC_TDM_PRIORITY_NONE,                  "NONE"},
    {SOC_DFE_FABRIC_TDM_PRIORITY_DEFAULT,               NULL}
};


const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fe_mc_id_range[] = {
    {soc_dfe_multicast_table_mode_64k,      "64K"},
    {soc_dfe_multicast_table_mode_128k,     "128K"},
    {soc_dfe_multicast_table_mode_128k_half,"128K_HALF"}, 
    {soc_dfe_multicast_table_mode_64k,       NULL}
};


#undef _ERR_MSG_MODULE_NAME
