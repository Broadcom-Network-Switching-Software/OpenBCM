/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON STACK
 */
 
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/dnxf/cmn/dnxf_stack.h>
#include <soc/dnxf/ramon/ramon_stack.h>
#include <soc/drv.h>
#include <soc/dnxf/cmn/dnxf_drv.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_STK


shr_error_e 
soc_ramon_stk_modid_set(int unit, int fe_id)
{
    uint32 reg_val32;
    SHR_FUNC_INIT_VARS(unit);
        
    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_GENERAL_CFG_2r(unit, &reg_val32));
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_2r, &reg_val32, DEV_IDf, fe_id);
    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_GENERAL_CFG_2r(unit, reg_val32));
   
exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_stk_modid_get(int unit, int* fe_id)
{
    uint32 reg_val32;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_GENERAL_CFG_2r(unit, &reg_val32));
    *fe_id = soc_reg_field_get(unit, ECI_FE_GLOBAL_GENERAL_CFG_2r, reg_val32, DEV_IDf);
    
exit:
    SHR_FUNC_EXIT;
  
}

shr_error_e
soc_ramon_stk_valid_module_id_verify(int unit, bcm_module_t module, int verify_divisble_by_32)
{
    SHR_FUNC_INIT_VARS(unit);

    if (module > SOC_RAMON_STK_MAX_MODULE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , above max\n");
    }

    if (module < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , below zero \n");
    }

    if (verify_divisble_by_32 && (module % SOC_RAMON_STK_FAP_GROUP_SIZE != 0) && (module != SOC_RAMON_STK_MAX_MODULE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , must be divisible by 32 \n");
    }

    if (verify_divisble_by_32 && (module == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , can't be 0 \n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_max_all_reachable_verify(int unit, bcm_module_t module)
{
    uint32 field_val;
    soc_reg_above_64_val_t reg_val_above_64;
    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(soc_ramon_stk_valid_module_id_verify(unit, module, 1));

    

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));
    field_val = soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, MAX_BASE_INDEXf);
    if ((module/SOC_RAMON_STK_FAP_GROUP_SIZE) > field_val) 
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max Fap Id for all reachable vector can't be higher than Max number of Faps in the system configured \n");
    }
       
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_max_all_reachable_set(int unit, bcm_module_t module)
{
    uint32 fap_group_num = 0, reg_val32;
    SHR_FUNC_INIT_VARS(unit);

    if (module == SOC_RAMON_STK_MAX_MODULE) {
        fap_group_num = module / SOC_RAMON_STK_FAP_GROUP_SIZE;
    } else {
        fap_group_num = module / SOC_RAMON_STK_FAP_GROUP_SIZE - 1;
    }

    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, &reg_val32, ALRC_MAX_BASE_INDEXf, fap_group_num);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, reg_val32));
    
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_max_all_reachable_get(int unit, bcm_module_t *module)
{
    uint32 reg_val32, field_val;
    bcm_module_t mod;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &reg_val32));
    field_val = soc_reg_field_get(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, reg_val32, ALRC_MAX_BASE_INDEXf);

    mod = SOC_RAMON_STK_FAP_GROUP_SIZE * (field_val + 1);
    if (mod != SOC_RAMON_STK_MAX_MODULE + 1) {
        *module = mod;
    } else {        
        *module = SOC_RAMON_STK_MAX_MODULE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_max_fap_verify(int unit, bcm_module_t module)
{
    uint32 reg_val32, field_val;

    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(soc_ramon_stk_valid_module_id_verify(unit, module, 1));

    

    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &reg_val32));
    field_val = soc_reg_field_get(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, reg_val32, ALRC_MAX_BASE_INDEXf);
    if (field_val > (module/SOC_RAMON_STK_FAP_GROUP_SIZE)) 
    {
        SHR_IF_ERR_EXIT(soc_ramon_stk_module_max_all_reachable_set(unit, module));
        LOG_WARN(BSL_LS_SOC_STK, (BSL_META_U(unit, "Max Fap Id for all reachable vector is now equal to Max Fap Id\n")));
    }
          
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_max_fap_set(int unit, bcm_module_t module)
{
    int i, nof_qrh;
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 reg_val32;
    uint32 fap_group_num = 0;
    uint32 rtp_bm_raw[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 rtp_reg_val[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    SHR_FUNC_INIT_VARS(unit);

    if (module == SOC_RAMON_STK_MAX_MODULE) {
        fap_group_num = module / SOC_RAMON_STK_FAP_GROUP_SIZE;
    } else {
        fap_group_num = module / SOC_RAMON_STK_FAP_GROUP_SIZE - 1;
    }

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, UPDATE_BASE_INDEXf, fap_group_num);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));

    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);

    for (i = 0; i < nof_qrh; ++i) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, i, 0, &reg_val32));
        soc_reg_field_set(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val32, UPDATE_BASE_INDEXf, fap_group_num);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, i, 0, reg_val32));
    }

    
    sal_memset(rtp_bm_raw, 0, sizeof(rtp_bm_raw));
    for (i = module + 1; i < SOC_DNXF_MODID_NOF; ++i) {
        SHR_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, i, rtp_reg_val));
        soc_mem_field_set(unit, RTP_RMHMTm, rtp_reg_val, LINK_BIT_MAPf, rtp_bm_raw);
        SHR_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit, MEM_BLOCK_ALL, i, rtp_reg_val));
    }

    sal_sleep(1);

    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, MAX_BASE_INDEXf, fap_group_num);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));

    for (i = 0; i < nof_qrh; ++i) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, i, 0, &reg_val32));
        soc_reg_field_set(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val32, MAX_BASE_INDEXf, fap_group_num);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, i, 0, reg_val32));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_max_fap_get(int unit, bcm_module_t *module)
{
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 field_val;
    bcm_module_t mod;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));
    field_val = soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, MAX_BASE_INDEXf);

    mod = SOC_RAMON_STK_FAP_GROUP_SIZE * (field_val + 1);
    if (mod != SOC_RAMON_STK_MAX_MODULE + 1) {
        *module = mod;
    } else {        
        *module = SOC_RAMON_STK_MAX_MODULE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stk_module_all_reachable_ignore_id_set(int unit, bcm_module_t module, int arg)
{
    int i, first_empty_cell = -1; 
    uint32 reg_val, valid_field, id_field;
    SHR_FUNC_INIT_VARS(unit);

    if (module > SOC_RAMON_STK_MAX_MODULE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid module id , above max\n");
    }

    for (i=0 ; i < SOC_RAMON_STK_MAX_IGNORED_FAP_IDS ; i++) 
    {
        SHR_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, i, &reg_val));
        id_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_DEVICEf);
        valid_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_VALIDf);

        if (valid_field == 1) 
        {
            if (id_field == module) 
            {
                if (arg == 0)  
                {
                    soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_DEVICEf, 0);
                    soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_VALIDf, 0);
                    SHR_IF_ERR_EXIT(WRITE_RTP_ALRC_EXCLUDEr(unit, i, reg_val));
                    SHR_EXIT();  
                }
                else 
                {
                    SHR_EXIT(); 
                }
            }
        }
        else if (first_empty_cell == -1 && arg == 1) 
        {
            first_empty_cell = i;
        }
    }

    if (arg == 0) 
    {
        SHR_EXIT();
    }
    
    if (first_empty_cell == -1) 
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't ignore this FAP ID - 16 FAP IDs are already ignored \n");
    }
    else 
    {
        SHR_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, first_empty_cell, &reg_val));
        soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_DEVICEf, module);
        soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_VALIDf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_ALRC_EXCLUDEr(unit, first_empty_cell, reg_val));
    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
soc_ramon_stk_module_all_reachable_ignore_id_get(int unit, bcm_module_t module, int *arg)
{
    int i, found = 0;
    uint32 reg_val, id_field, valid_field;
    SHR_FUNC_INIT_VARS(unit);

    for (i=0 ; i < SOC_RAMON_STK_MAX_IGNORED_FAP_IDS ; i++)
    {
        SHR_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, i, &reg_val));
        id_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_DEVICEf);
        valid_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_VALIDf);

        if (valid_field == 1 && id_field == module)
        {
            found = 1;
            break;
        }
    }

    *arg = found;

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

