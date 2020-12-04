/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR



#include <shared/bsl.h>
#include <shared/util.h>

#include <soc/error.h>
#include <soc/drv.h>
#include <soc/dcmn/error.h>

 



#include <soc/dcmn/dcmn_dev_feature_manager.h>



STATIC soc_error_t dcmn_keywords_cb (int unit,void  *value,int nof_ranges,void *data)
{
    char **keywords = (char **)data;
    int i;

    SOCDNX_INIT_FUNC_DEFS;


    if (value==NULL ||  !sal_strlen(value)) {
        SOC_EXIT;
    }
    for (i = 0; keywords[i]; i++) {
        if (!sal_strcmp(value,keywords[i]))
        {
            SOC_EXIT;
        }
    }
    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);

exit:
    SOCDNX_FUNC_RETURN;

}


STATIC soc_error_t dcmn_range_cb (int unit,void  *value,int nof_ranges,void *data)
{
    uint32 v;
    int i;
    dcmn_range_t *ranges = (dcmn_range_t *) data;


    SOCDNX_INIT_FUNC_DEFS;



    v = *(uint32 *)value;
    for (i=0;i<nof_ranges;i++) {
        if (ranges[i].from <=v && v<=ranges[i].to) {
            SOC_EXIT;
        }
    }
    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);

exit:
    SOCDNX_FUNC_RETURN;

}



STATIC soc_error_t dcmn_otn_enable_cb (int unit,void  *value,int nof_ranges,void *data)
{
    uint16 dev_id; 
    uint8 rev_id;

    SOCDNX_INIT_FUNC_DEFS;
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id==BCM88675_DEVICE_ID  || dev_id==BCM88375_DEVICE_ID) {
        uint32 val;
        SOCDNX_IF_ERR_EXIT(READ_ECI_OGER_1004r_REG32(unit,&val));
        
        if (!(val & 1<<8)) {
            SOC_EXIT;
        }
    }


    SOCDNX_IF_ERR_EXIT(dcmn_keywords_cb(unit,value,nof_ranges,data));

exit:
    SOCDNX_FUNC_RETURN;

}



dcmn_range_t range_0_0[] = {{0,0}};
dcmn_range_t range_0_1[] = {{0,1}};
dcmn_range_t range_0_720000[] = {{0,720000}};
dcmn_range_t range_0_325000[] = {{0,325000}};
dcmn_range_t range_0_175000[] = {{0,175000}};
char *none_keywords[]={"0","none",0};
char *fabric_mesh_keywords[]={"SINGLE_FAP",0};
char *only_mesh_keywords[]={"SINGLE_FAP","MESH","BACK2BACK",0};
char *single_stage_repeater_keywords[]={"SINGLE_STAGE_FE2","REPEATER",0};

dcmn_feature_cb_t  dc_feature_range[] = {
    {
        DCMN_NO_DC_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  dc_feature_none[] = {
    {
        DCMN_NO_DC_FEATURE,
        dcmn_keywords_cb,  
        0, 
        none_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  metro_feature_range[] = {
    {
        DCMN_NO_METRO_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  otn_feature_none[] = {
    {
        DCMN_NO_OTN_FEATURE,
        dcmn_otn_enable_cb,  
        0, 
        none_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  fabric_ilkn_feature_range[] = {
    {
        DCMN_NO_FABRIC_ILKN_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  fabric_mesh_feature_kwywords[] = {
    {
        DCMN_NO_FABRIC_MESH_FEATURE,
        dcmn_keywords_cb,  
        0, 
        fabric_mesh_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  only_mesh_feature_kwywords[] = {
    {
        DCMN_ONLY_MESH_FEATURE,
        dcmn_keywords_cb,  
        0, 
        only_mesh_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  single_stage_repeater_feature_kwywords[] = {
    {
        DCMN_SINGLE_STAGE_REPEATER_FEATURE,
        dcmn_keywords_cb,  
        0, 
        single_stage_repeater_keywords
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  no_ext_ram_feature_range[] = {
    {
        DCMN_NO_EXT_RAM_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  one_ext_ram_feature_range[] = {
    {
        DCMN_ONE_EXT_RAM_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_1)/sizeof(dcmn_range_t), 
        range_0_1
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  core_freq_325_feature_range[] = {
    {
        DCMN_CORE_FREQ_325_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_325000)/sizeof(dcmn_range_t), 
        range_0_325000
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  core_freq_175_feature_range[] = {
    {
        DCMN_CORE_FREQ_175_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_175000)/sizeof(dcmn_range_t), 
        range_0_175000
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_feature_cb_t  fabric_18_quads_feature_range[] = {
    {
        DCMN_FABRIC_18_QUADS_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  fabric_24_quads_feature_range[] = {
    {
        DCMN_FABRIC_24_QUADS_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  fabric_12_quads_feature_range[] = {
    {
        DCMN_FABRIC_12_QUADS_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};
dcmn_feature_cb_t  no_stat_feature_range[] = {
    {
        DCMN_QAX_NO_STAT_FEATURE,
        dcmn_range_cb,  
        sizeof(range_0_0)/sizeof(dcmn_range_t), 
        range_0_0
    },
    {
        DCMN_INVALID_FEATURE,
        NULL,
        0,
        NULL
    }
};

dcmn_property_features_t  properies_feature_map[] = {
                                                        {
                                                            spn_TRILL_MODE,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_BCM886XX_VXLAN_ENABLE,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_BCM886XX_L2GRE_ENABLE ,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_BCM886XX_FCOE_SWITCH_MODE  ,
                                                            dc_feature_range
                                                        },
                                                        {
                                                            spn_NUM_OAMP_PORTS ,
                                                            metro_feature_range
                                                        },
                                                        {
                                                            spn_FAP_TDM_BYPASS  ,
                                                            otn_feature_none
                                                        },
                                                        {
                                                            spn_FABRIC_CONNECT_MODE  ,
                                                            fabric_mesh_feature_kwywords
                                                        },
                                                        {
                                                            spn_FABRIC_CONNECT_MODE  ,
                                                            only_mesh_feature_kwywords
                                                        },
                                                        {
                                                            spn_FABRIC_DEVICE_MODE  ,
                                                            single_stage_repeater_feature_kwywords
                                                        },
                                                        {
                                                            spn_USE_FABRIC_LINKS_FOR_ILKN_NIF  ,
                                                            fabric_ilkn_feature_range
                                                        },
                                                        {
                                                             spn_EXT_RAM_PRESENT  ,
                                                             no_ext_ram_feature_range
                                                        },
                                                        {
                                                             spn_EXT_RAM_PRESENT  ,
                                                             one_ext_ram_feature_range
                                                        },
                                                        {
                                                             spn_CORE_CLOCK_SPEED  ,
                                                             core_freq_325_feature_range
                                                        },
                                                        {
                                                             spn_CORE_CLOCK_SPEED  ,
                                                             core_freq_175_feature_range
                                                        },
                                                        {
                                                             spn_SERDES_QRTT_ACTIVE  ,
                                                             fabric_18_quads_feature_range
                                                        },
                                                        {
                                                             spn_SERDES_QRTT_ACTIVE  ,
                                                             fabric_24_quads_feature_range
                                                        },
                                                        {
                                                             spn_SERDES_QRTT_ACTIVE  ,
                                                             fabric_12_quads_feature_range
                                                        },
                                                        {
                                                             spn_STAT_IF_ENABLE  ,
                                                             no_stat_feature_range
                                                        },

                                                    };




int no_metro_devices[] = 
    {
         BCM88674_DEVICE_ID,
         BCM88674_DEVICE_ID,
         BCM88676_DEVICE_ID,
         BCM88676M_DEVICE_ID,
         BCM88376_DEVICE_ID,
         BCM88376M_DEVICE_ID,
         INVALID_DEV_ID
    }; 
int no_dc_devices[] = 
    {
        BCM88671_DEVICE_ID,
        BCM88671M_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88677_DEVICE_ID,
        BCM88678_DEVICE_ID,
        BCM88371_DEVICE_ID,
        BCM88371M_DEVICE_ID,
        BCM88377_DEVICE_ID,
        BCM88378_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int no_otn_devices[] = 
    {
        BCM88671_DEVICE_ID,
        BCM88671M_DEVICE_ID,
        BCM88675_DEVICE_ID,
        BCM88675M_DEVICE_ID,
        BCM88676_DEVICE_ID,
        BCM88676M_DEVICE_ID,
        BCM88370_DEVICE_ID,
        BCM88371_DEVICE_ID,
        BCM88371M_DEVICE_ID,
        BCM88375_DEVICE_ID,
        BCM88376_DEVICE_ID,
        BCM88376M_DEVICE_ID,
        BCM88477_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_nif_24_44_devices[] =
    {
        BCM88670_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88377_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_nif_40_20_devices[] =
    {
        BCM88682_DEVICE_ID,
        BCM88684_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_nif_32_0_devices[] =
    {
        BCM88687_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_nif_48_0_devices[] =
    {
        BCM88683_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_nif_12_32_devices[] =
    {
        BCM88677_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_nif_36_4_devices[] =
    {
        BCM88685_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_nif_10_32_devices[] =
    {
        BCM88471_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_nif_12_24_devices[] =
    {
        BCM88474_DEVICE_ID,
        BCM88474H_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_nif_16_24_devices[] =
    {
        BCM88477_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qux_nif_24_6_devices[] =
    {
        BCM88272_DEVICE_ID,
        BCM88276_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qux_nif_12_4_devices[] =
    {
        BCM88273_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qux_nif_12_2_devices[] =
    {
        BCM88274_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int jer_ilkn_4_ports_devices[] =
    {
        BCM88670_DEVICE_ID,
        BCM88672_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88677_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_caui4_3_ports_devices[] =
    {
        BCM88474H_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_caui4_0_ports_devices[] =
    {
        BCM88471_DEVICE_ID,
        BCM88474_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_no_stat_devices[] =
    {
        BCM88471_DEVICE_ID,
        BCM88474_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int qax_low_falcon_speed_devices[] =
    {
        BCM88471_DEVICE_ID,
        BCM88474_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int fabric_18_quads_devices[] = 
    {
        BCM88777_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int fabric_24_quads_devices[] = 
    {
        BCM88773_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int fabric_12_quads_devices[] = 
    {
        BCM88774_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int no_fabric_ilkn_devices[] = 
    {
        BCM88670_DEVICE_ID,
        BCM88672_DEVICE_ID,
        BCM88673_DEVICE_ID,
        BCM88674_DEVICE_ID,
        BCM88677_DEVICE_ID,
        BCM88370_DEVICE_ID,
        BCM88376_DEVICE_ID,
        BCM88378_DEVICE_ID,
       INVALID_DEV_ID
    }; 

int no_fabric_mesh_devices[] = 
    {
        BCM88671M_DEVICE_ID,
        BCM88676M_DEVICE_ID,
        BCM88677_DEVICE_ID,
        BCM88370_DEVICE_ID,
        BCM88371_DEVICE_ID,
        BCM88376_DEVICE_ID,
        BCM88378_DEVICE_ID,
        BCM88381_DEVICE_ID,
        BCM88471_DEVICE_ID,
        BCM88474_DEVICE_ID,
        BCM88474H_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int only_mesh_devices[] = 
    {
        BCM88380_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int single_stage_repeater_devices[] =
    {
        BCM88773_DEVICE_ID,
        BCM88774_DEVICE_ID,
        BCM88775_DEVICE_ID,
        BCM88776_DEVICE_ID,
        BCM88777_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int no_ext_ram_devices[] =
    {
        BCM88677_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int one_ext_ram_devices[] =
    {
        BCM88474_DEVICE_ID,
        INVALID_DEV_ID
    }; 

int core_freq_325_devices[] =
    {
         BCM88471_DEVICE_ID,
         BCM88474_DEVICE_ID,
         INVALID_DEV_ID
    }; 

int core_freq_175_devices[] =
    {
         BCM88273_DEVICE_ID,
         BCM88274_DEVICE_ID,
         INVALID_DEV_ID
    }; 

int no_extended_lpm_devices[] =
    {
         BCM88681_DEVICE_ID,
         BCM88682_DEVICE_ID,
         BCM88683_DEVICE_ID,
         BCM88685_DEVICE_ID,
         BCM88687_DEVICE_ID,
         INVALID_DEV_ID
    }; 


int   *device_features_map[DCMN_NUM_OF_FEATURES] = {
    no_metro_devices,
    no_dc_devices,
    no_otn_devices,
    jer_nif_24_44_devices,
    jer_nif_40_20_devices,
    jer_nif_48_0_devices,
    jer_nif_32_0_devices,
    jer_nif_12_32_devices,
    jer_nif_36_4_devices,
    qax_nif_10_32_devices,
    qax_nif_12_24_devices,
    qax_nif_16_24_devices,
    qux_nif_24_6_devices,
    qux_nif_12_4_devices,
    qux_nif_12_2_devices,
    jer_ilkn_4_ports_devices,
    qax_caui4_3_ports_devices,
    qax_caui4_0_ports_devices,
    qax_no_stat_devices,
    qax_low_falcon_speed_devices,
    fabric_18_quads_devices,
    fabric_24_quads_devices,
    fabric_12_quads_devices,
    no_fabric_ilkn_devices,
    no_fabric_mesh_devices,
    only_mesh_devices,
    single_stage_repeater_devices,
    no_ext_ram_devices,
    one_ext_ram_devices,
    core_freq_325_devices,
    core_freq_175_devices,
    no_extended_lpm_devices
    };


soc_error_t dcmn_is_property_value_permited(int unit,const char *property,void *value) {
    int i,j,k;
    int len = sizeof(properies_feature_map)/sizeof(dcmn_property_features_t);
    uint16 dev_id; 
    uint8 rev_id;
    dcmn_feature_cb_t  *features_array;
    dcmn_feature_t feature;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    for (i=0;i<len;i++) {

        
        if (sal_strcmp(property,properies_feature_map[i].property)) {
            continue;
        }
        
        features_array = properies_feature_map[i].features_array;
        for (j=0;features_array[j].feature!=DCMN_INVALID_FEATURE;j++) {
            feature = features_array[j].feature;
            
            for (k=0;device_features_map[feature][k]!=INVALID_DEV_ID && device_features_map[feature][k]!=dev_id;k++) {
            }
            if (device_features_map[feature][k]!=dev_id) {
                continue;
            }
            
            rc = features_array[j].cb(unit,value,features_array[j].nof_ranges,features_array[j].data);
            if (rc!= SOC_E_NONE) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property:%s  blocked for device %s\n"), property, soc_dev_name(unit)));  
            }

            SOCDNX_IF_ERR_EXIT(rc);


        }
    }

exit:
    SOCDNX_FUNC_RETURN;




}


soc_error_t dcmn_property_get(int unit, const char *name, uint32 defl,uint32 *value)
{
    SOCDNX_INIT_FUNC_DEFS;
    *value = soc_property_get(unit,name, defl);

    SOCDNX_IF_ERR_EXIT(dcmn_is_property_value_permited(unit,name,value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t dcmn_property_suffix_num_get(int unit, int num, const char *name, const char *suffix, uint32 defl,uint32 *value)
{
    SOCDNX_INIT_FUNC_DEFS;
    *value = soc_property_suffix_num_get(unit, num, name, suffix, defl);

    SOCDNX_IF_ERR_EXIT(dcmn_is_property_value_permited(unit,name,value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t dcmn_property_get_str(int unit, const char *name,char **value)
{
    SOCDNX_INIT_FUNC_DEFS;
    *value = soc_property_get_str(unit,name);
    SOCDNX_IF_ERR_EXIT(dcmn_is_property_value_permited(unit,name,*value));

exit:
    SOCDNX_FUNC_RETURN;

}

uint8       dcmn_device_block_for_feature(int unit,dcmn_feature_t feature)
{
    uint32 i;
    uint16 dev_id;
    uint8 rev_id;
    soc_cm_get_id(unit, &dev_id, &rev_id);
    for (i=0;device_features_map[feature][i]!=INVALID_DEV_ID && device_features_map[feature][i]!=dev_id;i++) {
    }

    return device_features_map[feature][i]==dev_id;

}

