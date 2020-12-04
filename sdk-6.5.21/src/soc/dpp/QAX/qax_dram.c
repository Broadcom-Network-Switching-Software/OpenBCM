/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_DRAM

 

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/QAX/qax_dram.h>




int soc_qax_dram_recovery_init(int unit)
{
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_MMU_GENERAL_CONFIGURATION_REGISTERr(unit, &reg64_val));
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, DRAM_BLOCK_TOTAL_RAF_SIZE_THf, 0x190);
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, DRAM_BLOCK_LEAKY_BUCKET_THf, 100);
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_GENERAL_CONFIGURATION_REGISTERr(unit, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_DRAM_BLOCK_BMP_MASKr(unit, SOC_CORE_ALL, reg_above_64_val));
    
    soc_reg_above_64_field32_set(unit, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, reg_above_64_val, VOQ_DRAM_BLOCK_BMP_MASK_DP_0f, 0x138);
    soc_reg_above_64_field32_set(unit, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, reg_above_64_val, VOQ_DRAM_BLOCK_BMP_MASK_DP_1f, 0x138);
    soc_reg_above_64_field32_set(unit, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, reg_above_64_val, VOQ_DRAM_BLOCK_BMP_MASK_DP_2f, 0x138);
    soc_reg_above_64_field32_set(unit, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, reg_above_64_val, VOQ_DRAM_BLOCK_BMP_MASK_DP_3f, 0x138);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_DRAM_BLOCK_BMP_MASKr(unit, SOC_CORE_ALL, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

