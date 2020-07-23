/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 STACK
 */
 
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/dfe/cmn/dfe_stack.h>
#include <soc/dfe/fe3200/fe3200_stack.h>
#include <soc/drv.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STK


#if defined(BCM_88950_A0)

#define LINKS_ARRAY_SIZE (SOC_DFE_MAX_NOF_LINKS/32 + (SOC_DFE_MAX_NOF_LINKS%32 ? 1:0) )



soc_error_t 
soc_fe3200_stk_modid_set(int unit, uint32 fe_id)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;
        
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg_val32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg_val32, DEV_ID_0f, fe_id);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg_val32));
   
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_stk_modid_get(int unit, uint32* fe_id)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg_val32));
    *fe_id = soc_reg_field_get(unit, ECI_GLOBAL_GENERAL_CFG_2r, reg_val32, DEV_ID_0f);
    
exit:
    SOCDNX_FUNC_RETURN;
  
}

soc_error_t
soc_fe3200_stk_valid_module_id_verify(int unit, bcm_module_t module, int verify_divisble_by_32)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (module > SOC_FE3200_STK_MAX_MODULE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , above max\n")));
    }

    if (module < 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , below zero \n")));
    }

    if (verify_divisble_by_32 && (module % SOC_FE3200_STK_FAP_GROUP_SIZE != 0) && (module != SOC_FE3200_STK_MAX_MODULE))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , must be divisible by 32 \n")));
    }

    if (verify_divisble_by_32 && (module == 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , can't be 0 \n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_all_reachable_verify(int unit, bcm_module_t module)
{
    uint32 field_val;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_valid_module_id_verify(unit, module, 1));

    

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    field_val = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, MAX_BASE_INDEXf);
    if ((module/SOC_FE3200_STK_FAP_GROUP_SIZE) > field_val) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Max Fap Id for all reachable vector can't be higher than Max number of Faps in the system configured \n")));
    }
       
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_all_reachable_set(int unit, bcm_module_t module)
{
    uint32 fap_group_num = 0, reg_val32;
    SOCDNX_INIT_FUNC_DEFS;

    if (module == SOC_FE3200_STK_MAX_MODULE) {
        fap_group_num = module / SOC_FE3200_STK_FAP_GROUP_SIZE;
    } else {
        fap_group_num = module / SOC_FE3200_STK_FAP_GROUP_SIZE - 1;
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_ALL_REACHABLE_CFGr, &reg_val32, ALRC_MAX_BASE_INDEXf, fap_group_num);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_CFGr(unit, reg_val32));
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_all_reachable_get(int unit, bcm_module_t *module)
{
    uint32 reg_val32, field_val;
    bcm_module_t mod;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
    field_val = soc_reg_field_get(unit, RTP_ALL_REACHABLE_CFGr, reg_val32, ALRC_MAX_BASE_INDEXf);

    mod = SOC_FE3200_STK_FAP_GROUP_SIZE * (field_val + 1);
    if (mod != SOC_FE3200_STK_MAX_MODULE + 1) {
        *module = mod;
    } else {        
        *module = SOC_FE3200_STK_MAX_MODULE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_fap_verify(int unit, bcm_module_t module)
{
    uint32 reg_val32, field_val;

    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_valid_module_id_verify(unit, module, 1));

    

    SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
    field_val = soc_reg_field_get(unit, RTP_ALL_REACHABLE_CFGr, reg_val32, ALRC_MAX_BASE_INDEXf);
    if (field_val > (module/SOC_FE3200_STK_FAP_GROUP_SIZE)) 
    {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_module_max_all_reachable_set(unit, module));
        LOG_WARN(BSL_LS_SOC_STK, (BSL_META_U(unit, "Max Fap Id for all reachable vector is now equal to Max Fap Id\n")));
    }
          
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_fap_set(int unit, bcm_module_t module)
{
    int i;
    uint64 reg_val64;
    uint32 fap_group_num = 0;
    uint32 rtp_bm_raw[LINKS_ARRAY_SIZE];
    uint32 rtp_reg_val[LINKS_ARRAY_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    if (module == SOC_FE3200_STK_MAX_MODULE) {
        fap_group_num = module / SOC_FE3200_STK_FAP_GROUP_SIZE;
    } else {
        fap_group_num = module / SOC_FE3200_STK_FAP_GROUP_SIZE - 1;
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, UPDATE_BASE_INDEXf, fap_group_num);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

    
    rtp_bm_raw[0] = rtp_bm_raw[1] = rtp_bm_raw[2] = rtp_bm_raw[3] = rtp_bm_raw[4] = 0;
    for (i = module + 1; i < SOC_DFE_MODID_NOF; ++i) {
        SOCDNX_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, i, rtp_reg_val));
        soc_mem_field_set(unit, RTP_RMHMTm, rtp_reg_val, LINK_BIT_MAPf, rtp_bm_raw);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit, MEM_BLOCK_ALL, i, rtp_reg_val));
    }

    sal_sleep(1);

    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, MAX_BASE_INDEXf, fap_group_num);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_fap_get(int unit, bcm_module_t *module)
{
    uint64 reg_val64;
    uint32 field_val;
    bcm_module_t mod;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    field_val = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, MAX_BASE_INDEXf);

    mod = SOC_FE3200_STK_FAP_GROUP_SIZE * (field_val + 1);
    if (mod != SOC_FE3200_STK_MAX_MODULE + 1) {
        *module = mod;
    } else {        
        *module = SOC_FE3200_STK_MAX_MODULE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_all_reachable_ignore_id_set(int unit, bcm_module_t module, int arg)
{
    int i, first_empty_cell = -1; 
    uint32 reg_val, valid_field, id_field;
    SOCDNX_INIT_FUNC_DEFS;

    if (module > SOC_FE3200_STK_MAX_MODULE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , above max\n")));
    }

    for (i=0 ; i < SOC_FE3200_STK_MAX_IGNORED_FAP_IDS ; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, i, &reg_val));
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
                    SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALRC_EXCLUDEr(unit, i, reg_val));
                    SOC_EXIT;  
                }
                else 
                {
                    SOC_EXIT; 
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
        SOC_EXIT;
    }
    
    if (first_empty_cell == -1) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't ignore this FAP ID - 16 FAP IDs are already ignored \n")));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, first_empty_cell, &reg_val));
        soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_DEVICEf, module);
        soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_VALIDf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALRC_EXCLUDEr(unit, first_empty_cell, reg_val));
    }
exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t
soc_fe3200_stk_module_all_reachable_ignore_id_get(int unit, bcm_module_t module, int *arg)
{
    int i, found = 0;
    uint32 reg_val, id_field, valid_field;
    SOCDNX_INIT_FUNC_DEFS;

    for (i=0 ; i < SOC_FE3200_STK_MAX_IGNORED_FAP_IDS ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, i, &reg_val));
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
    SOCDNX_FUNC_RETURN;
}



#endif

#undef _ERR_MSG_MODULE_NAME

