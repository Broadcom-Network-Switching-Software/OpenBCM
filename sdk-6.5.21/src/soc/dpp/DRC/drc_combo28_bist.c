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


#include <soc/dpp/drv.h>


#include <soc/dcmn/error.h>

 
#include <soc/dpp/DRC/drc_combo28_bist.h>



#define SOC_DPP_DRC_COMBO28_BIST_MIN_POLLS                          (10000000)




int soc_dpp_drc_combo28_configure_bist(
    int unit,
    int drc_ndx,
    uint32 write_weight,
    uint32 read_weight,
    uint32 pattern_bit_mode,
    uint32 two_addr_mode,
    uint32 prbs_mode,
    uint32 address_shift_mode,
    uint32 data_shift_mode,
    uint32 data_addr_mode,
    uint32 bist_num_actions,
    uint32 bist_start_address,
    uint32 bist_end_address,
    uint32 bg_interleave,
    uint32 single_bank_test,
    uint32 address_prbs_mode,
    uint32 bist_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS],
    uint32 bist_data_seed[SOC_DPP_DRC_COMBO28_BIST_NOF_SEEDS])
{
    int i;
    uint32 reg_val;
    uint64 
        reg64_val, 
        reg64_field_val;
    const static soc_reg_t 
        DRC_BIST_CONFIGURATIONSl[] = {DRCA_BIST_CONFIGURATIONSr, DRCB_BIST_CONFIGURATIONSr, DRCC_BIST_CONFIGURATIONSr,
                                      DRCD_BIST_CONFIGURATIONSr, DRCE_BIST_CONFIGURATIONSr, DRCF_BIST_CONFIGURATIONSr,
                                      DRCG_BIST_CONFIGURATIONSr, DRCH_BIST_CONFIGURATIONSr},
        DRC_BIST_NUMBER_OF_ACTIONSl[] = {DRCA_BIST_NUMBER_OF_ACTIONSr, DRCB_BIST_NUMBER_OF_ACTIONSr, DRCC_BIST_NUMBER_OF_ACTIONSr,
                                         DRCD_BIST_NUMBER_OF_ACTIONSr, DRCE_BIST_NUMBER_OF_ACTIONSr, DRCF_BIST_NUMBER_OF_ACTIONSr,
                                         DRCG_BIST_NUMBER_OF_ACTIONSr, DRCH_BIST_NUMBER_OF_ACTIONSr},
        DRC_BIST_START_ADDRESSl[] = {DRCA_BIST_START_ADDRESSr, DRCB_BIST_START_ADDRESSr, DRCC_BIST_START_ADDRESSr,
                                     DRCD_BIST_START_ADDRESSr, DRCE_BIST_START_ADDRESSr, DRCF_BIST_START_ADDRESSr,
                                     DRCG_BIST_START_ADDRESSr, DRCH_BIST_START_ADDRESSr},
        DRC_BIST_END_ADDRESSl[] = {DRCA_BIST_END_ADDRESSr, DRCB_BIST_END_ADDRESSr, DRCC_BIST_END_ADDRESSr,
                                   DRCD_BIST_END_ADDRESSr, DRCE_BIST_END_ADDRESSr, DRCF_BIST_END_ADDRESSr,
                                   DRCG_BIST_END_ADDRESSr, DRCH_BIST_END_ADDRESSr},
        DRC_BIST_PATTERN_WORDl[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = {{DRCA_BIST_PATTERN_WORD_0r, DRCB_BIST_PATTERN_WORD_0r, DRCC_BIST_PATTERN_WORD_0r,
                                                                                             DRCD_BIST_PATTERN_WORD_0r, DRCE_BIST_PATTERN_WORD_0r, DRCF_BIST_PATTERN_WORD_0r,
                                                                                             DRCG_BIST_PATTERN_WORD_0r, DRCH_BIST_PATTERN_WORD_0r},
                                                                                            {DRCA_BIST_PATTERN_WORD_1r, DRCB_BIST_PATTERN_WORD_1r, DRCC_BIST_PATTERN_WORD_1r,
                                                                                             DRCD_BIST_PATTERN_WORD_1r, DRCE_BIST_PATTERN_WORD_1r, DRCF_BIST_PATTERN_WORD_1r,
                                                                                             DRCG_BIST_PATTERN_WORD_1r, DRCH_BIST_PATTERN_WORD_1r},
                                                                                            {DRCA_BIST_PATTERN_WORD_2r, DRCB_BIST_PATTERN_WORD_2r, DRCC_BIST_PATTERN_WORD_2r,
                                                                                             DRCD_BIST_PATTERN_WORD_2r, DRCE_BIST_PATTERN_WORD_2r, DRCF_BIST_PATTERN_WORD_2r,
                                                                                             DRCG_BIST_PATTERN_WORD_2r, DRCH_BIST_PATTERN_WORD_2r},
                                                                                            {DRCA_BIST_PATTERN_WORD_3r, DRCB_BIST_PATTERN_WORD_3r, DRCC_BIST_PATTERN_WORD_3r,
                                                                                             DRCD_BIST_PATTERN_WORD_3r, DRCE_BIST_PATTERN_WORD_3r, DRCF_BIST_PATTERN_WORD_3r,
                                                                                             DRCG_BIST_PATTERN_WORD_3r, DRCH_BIST_PATTERN_WORD_3r},
                                                                                            {DRCA_BIST_PATTERN_WORD_4r, DRCB_BIST_PATTERN_WORD_4r, DRCC_BIST_PATTERN_WORD_4r,
                                                                                             DRCD_BIST_PATTERN_WORD_4r, DRCE_BIST_PATTERN_WORD_4r, DRCF_BIST_PATTERN_WORD_4r,
                                                                                             DRCG_BIST_PATTERN_WORD_4r, DRCH_BIST_PATTERN_WORD_4r},
                                                                                            {DRCA_BIST_PATTERN_WORD_5r, DRCB_BIST_PATTERN_WORD_5r, DRCC_BIST_PATTERN_WORD_5r,
                                                                                             DRCD_BIST_PATTERN_WORD_5r, DRCE_BIST_PATTERN_WORD_5r, DRCF_BIST_PATTERN_WORD_5r,
                                                                                             DRCG_BIST_PATTERN_WORD_5r, DRCH_BIST_PATTERN_WORD_5r},
                                                                                            {DRCA_BIST_PATTERN_WORD_6r, DRCB_BIST_PATTERN_WORD_6r, DRCC_BIST_PATTERN_WORD_6r,
                                                                                             DRCD_BIST_PATTERN_WORD_6r, DRCE_BIST_PATTERN_WORD_6r, DRCF_BIST_PATTERN_WORD_6r,
                                                                                             DRCG_BIST_PATTERN_WORD_6r, DRCH_BIST_PATTERN_WORD_6r},
                                                                                            {DRCA_BIST_PATTERN_WORD_7r, DRCB_BIST_PATTERN_WORD_7r, DRCC_BIST_PATTERN_WORD_7r,
                                                                                             DRCD_BIST_PATTERN_WORD_7r, DRCE_BIST_PATTERN_WORD_7r, DRCF_BIST_PATTERN_WORD_7r,
                                                                                             DRCG_BIST_PATTERN_WORD_7r, DRCH_BIST_PATTERN_WORD_7r }},
        DRC_BIST_PRBS_DATA_SEED_LSBl[] = {DRCA_BIST_PRBS_DATA_SEED_LSBr, DRCB_BIST_PRBS_DATA_SEED_LSBr, DRCC_BIST_PRBS_DATA_SEED_LSBr, 
                                          DRCD_BIST_PRBS_DATA_SEED_LSBr, DRCE_BIST_PRBS_DATA_SEED_LSBr, DRCF_BIST_PRBS_DATA_SEED_LSBr, 
                                          DRCG_BIST_PRBS_DATA_SEED_LSBr, DRCH_BIST_PRBS_DATA_SEED_LSBr},
        DRC_BIST_PRBS_DATA_SEED_MSBl[] = {DRCA_BIST_PRBS_DATA_SEED_MSBr, DRCB_BIST_PRBS_DATA_SEED_MSBr, DRCC_BIST_PRBS_DATA_SEED_MSBr, 
                                          DRCD_BIST_PRBS_DATA_SEED_MSBr, DRCE_BIST_PRBS_DATA_SEED_MSBr, DRCF_BIST_PRBS_DATA_SEED_MSBr, 
                                          DRCG_BIST_PRBS_DATA_SEED_MSBr, DRCH_BIST_PRBS_DATA_SEED_MSBr};  
    soc_reg_above_64_val_t seed_val;

    const static soc_field_t BIST_PATTERN_fieldl[] = {BIST_PATTERN_0f, BIST_PATTERN_1f, BIST_PATTERN_2f, BIST_PATTERN_3f,
                                                      BIST_PATTERN_4f, BIST_PATTERN_5f, BIST_PATTERN_6f, BIST_PATTERN_7f};

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 
    COMPILER_64_ZERO(reg64_val);
    COMPILER_64_ZERO(reg64_field_val);
    
    COMPILER_64_SET(reg64_field_val, 0x0, write_weight);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, WRITE_WEIGHTf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, read_weight);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, READ_WEIGHTf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, pattern_bit_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, PATTERN_BIT_MODEf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, two_addr_mode); 
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, TWO_ADDR_MODEf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, prbs_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, PRBS_MODEf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, address_shift_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, ADDRESS_SHIFT_MODEf, reg64_field_val); 
    COMPILER_64_SET(reg64_field_val, 0x0, data_shift_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, DATA_SHIFT_MODEf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, data_addr_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, DATA_ADDR_MODEf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, bg_interleave);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, BG_INTERLEAVEf, reg64_field_val);
    COMPILER_64_SET(reg64_field_val, 0x0, single_bank_test);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, SINGLE_BANK_TESTf, reg64_field_val);       
    COMPILER_64_SET(reg64_field_val, 0x0, address_prbs_mode); 
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, ADDRESS_PRBS_MODEf, reg64_field_val);    

    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
  
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_BIST_NUMBER_OF_ACTIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_BIST_NUMBER_OF_ACTIONSr, &reg_val, BIST_NUM_ACTIONSf, bist_num_actions);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_BIST_NUMBER_OF_ACTIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_BIST_START_ADDRESSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_BIST_START_ADDRESSr, &reg_val, BIST_START_ADDRESSf, bist_start_address);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_BIST_START_ADDRESSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_BIST_END_ADDRESSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_BIST_END_ADDRESSr, &reg_val, BIST_END_ADDRESSf, bist_end_address); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_BIST_END_ADDRESSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
  
    
    if (bist_pattern != NULL) {
        for (i=0; i < SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit,  DRC_BIST_PATTERN_WORDl[i][drc_ndx], REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit,  DRC_BIST_PATTERN_WORDl[i][drc_ndx], &reg_val, BIST_PATTERN_fieldl[i], bist_pattern[i]);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit,  DRC_BIST_PATTERN_WORDl[i][drc_ndx], REG_PORT_ANY, 0, reg_val));
        }
    }

    if (bist_data_seed != NULL) {
        
        sal_memset(seed_val,0,sizeof(soc_reg_above_64_val_t));

        
        sal_memcpy(seed_val, bist_data_seed, sizeof(uint32) * 4);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_BIST_PRBS_DATA_SEED_LSBl[drc_ndx], REG_PORT_ANY, 0, seed_val));
            
         
        sal_memcpy(seed_val, &(bist_data_seed[4]), sizeof(uint32)*4 );
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_BIST_PRBS_DATA_SEED_MSBl[drc_ndx], REG_PORT_ANY, 0, seed_val));
    }
                  
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_mpr_configure_bist(
    int unit,
    int drc_ndx,
    int mpr_stagger_mode,
    int mpr_stagger_increment_mode,
    int mpr_readout_mode,
    int mpr_readout_mpr_location,
    uint8 mpr_pattern[SHMOO_COMBO28_BIST_MPR_NOF_PATTERNS])
{
    uint32
        reg_val,
        fld_val,
        i,
        clam_shell_mode;
    uint64 
        reg64_val, 
        reg64_field_val;    
    const static soc_reg_t 
        DRC_BIST_CONFIGURATIONSl[] = {DRCA_BIST_CONFIGURATIONSr, DRCB_BIST_CONFIGURATIONSr, DRCC_BIST_CONFIGURATIONSr,
                                      DRCD_BIST_CONFIGURATIONSr, DRCE_BIST_CONFIGURATIONSr, DRCF_BIST_CONFIGURATIONSr,
                                      DRCG_BIST_CONFIGURATIONSr, DRCH_BIST_CONFIGURATIONSr},
        DRC_BIST_MPR_STAGGER_WEIGHTSl[] = {DRCA_BIST_MPR_STAGGER_WEIGHTSr, DRCB_BIST_MPR_STAGGER_WEIGHTSr, DRCC_BIST_MPR_STAGGER_WEIGHTSr,
                                           DRCD_BIST_MPR_STAGGER_WEIGHTSr, DRCE_BIST_MPR_STAGGER_WEIGHTSr, DRCF_BIST_MPR_STAGGER_WEIGHTSr,
                                           DRCG_BIST_MPR_STAGGER_WEIGHTSr, DRCH_BIST_MPR_STAGGER_WEIGHTSr},
        DRC_CLAM_SHELLl[] = {DRCA_CLAM_SHELLr, DRCB_CLAM_SHELLr, DRCC_CLAM_SHELLr,
                             DRCD_CLAM_SHELLr, DRCE_CLAM_SHELLr, DRCF_CLAM_SHELLr,
                             DRCG_CLAM_SHELLr, DRCH_CLAM_SHELLr},
        DRC_BIST_MPR_STAGGER_PATTERNSl[] = {DRCA_BIST_MPR_STAGGER_PATTERNSr, DRCB_BIST_MPR_STAGGER_PATTERNSr, DRCC_BIST_MPR_STAGGER_PATTERNSr,
                                            DRCD_BIST_MPR_STAGGER_PATTERNSr, DRCE_BIST_MPR_STAGGER_PATTERNSr, DRCF_BIST_MPR_STAGGER_PATTERNSr,
                                            DRCG_BIST_MPR_STAGGER_PATTERNSr, DRCH_BIST_MPR_STAGGER_PATTERNSr},
        DRC_INTIAL_CALIB_USE_MPRl[] = {DRCA_INTIAL_CALIB_USE_MPRr, DRCB_INTIAL_CALIB_USE_MPRr, DRCC_INTIAL_CALIB_USE_MPRr,
                                       DRCD_INTIAL_CALIB_USE_MPRr, DRCE_INTIAL_CALIB_USE_MPRr, DRCF_INTIAL_CALIB_USE_MPRr,
                                       DRCG_INTIAL_CALIB_USE_MPRr, DRCH_INTIAL_CALIB_USE_MPRr};
    const static soc_field_t DRC_BIST_MPR_STAGGER_PATTERNS_fieldl[] = {BIST_MPR_STAGGER_PATTERN_0f, BIST_MPR_STAGGER_PATTERN_1f, BIST_MPR_STAGGER_PATTERN_2f, BIST_MPR_STAGGER_PATTERN_3f};

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 
    
    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    fld_val = soc_reg_field_get(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], reg_val, INTIAL_CALIB_READ_MPRf);
    if (fld_val == 0x0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("INTIAL_CALIB_READ_MPR field is not set. fld_val=0x%x"), fld_val));
    }
    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    COMPILER_64_SET(reg64_field_val, 0x0, mpr_stagger_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, MPR_STAGGER_MODEf, reg64_field_val);
    if(mpr_stagger_mode == TRUE){        
        
        COMPILER_64_SET(reg64_field_val, 0x0, mpr_stagger_increment_mode);
        soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, MPR_STAGGER_INCREMENT_MODEf, reg64_field_val);
    }
    COMPILER_64_SET(reg64_field_val, 0x0, mpr_readout_mode);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, MPR_READOUT_MODEf, reg64_field_val); 
    COMPILER_64_SET(reg64_field_val, 0x0, mpr_readout_mpr_location); 
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg64_val, MPR_READOUT_MPR_LOCATIONf, reg64_field_val);        
        
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    if (mpr_stagger_mode == TRUE) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_CLAM_SHELLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
        clam_shell_mode = soc_reg_field_get(unit, DRC_CLAM_SHELLl[drc_ndx], reg_val, CLAM_SHELL_MODEf);

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
        if (clam_shell_mode == 0x1) {
             soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_1f, 0);
             soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_2f, 0);
             soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_3f, 0);                   
        } else {
             soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_1f,3);
             soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_2f,6);
             soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_3f,1);
        }
        soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], &reg_val, BIST_MPR_STAGGER_WEIGHT_0f, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_BIST_MPR_STAGGER_WEIGHTSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_BIST_MPR_STAGGER_PATTERNSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));        
        for(i=0; i < SHMOO_COMBO28_BIST_MPR_NOF_PATTERNS; ++i) {
            soc_reg_field_set(unit, DRC_BIST_MPR_STAGGER_PATTERNSl[drc_ndx], &reg_val, DRC_BIST_MPR_STAGGER_PATTERNS_fieldl[i], mpr_pattern[i]);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_BIST_MPR_STAGGER_PATTERNSl[drc_ndx], REG_PORT_ANY, 0, reg_val));      
    }
 
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_bist_atomic_action_start(int unit, int drc_ndx, uint8 start)
{
    const static soc_reg_t 
        DRC_BIST_CONFIGURATIONSl[] = {DRCA_BIST_CONFIGURATIONSr, DRCB_BIST_CONFIGURATIONSr, DRCC_BIST_CONFIGURATIONSr,
                                  DRCD_BIST_CONFIGURATIONSr, DRCE_BIST_CONFIGURATIONSr, DRCF_BIST_CONFIGURATIONSr,
                                  DRCG_BIST_CONFIGURATIONSr, DRCH_BIST_CONFIGURATIONSr};
    uint64 
        reg_val,
        field64_val;
    
    SOCDNX_INIT_FUNC_DEFS;
  
    
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    COMPILER_64_ZERO(field64_val);
    soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, BIST_ENf, field64_val); 
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
  
    if (start == TRUE) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
        COMPILER_64_SET(field64_val, 0x0, 0x1);
        soc_reg64_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, BIST_ENf, field64_val); 
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
   }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_bist_atomic_action_polling(int unit, int drc_ndx)
{
    uint32 soc_sand_rv, reg_val;
    const static soc_reg_t DRC_BIST_STATUSESl[] = {DRCA_BIST_STATUSESr, DRCB_BIST_STATUSESr, DRCC_BIST_STATUSESr,
                                                   DRCD_BIST_STATUSESr, DRCE_BIST_STATUSESr, DRCF_BIST_STATUSESr,
                                                   DRCG_BIST_STATUSESr, DRCH_BIST_STATUSESr};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_BIST_STATUSESl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): reg_val=0x%x\n"), FUNCTION_NAME(), reg_val)); 
 
    soc_sand_rv = arad_polling(
        unit,
        ARAD_TIMEOUT,
        SOC_DPP_DRC_COMBO28_BIST_MIN_POLLS,
        DRC_BIST_STATUSESl[drc_ndx],
        REG_PORT_ANY,
        0,
        BIST_FINISHEDf,
        1);
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    } 
  
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): Bist Atomic acttion polling finished \n"), FUNCTION_NAME())); 

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_bist_atomic_action(int unit, int drc_ndx, uint8 is_infinite, uint8 stop)
{ 
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (stop == FALSE) {
        
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_interrupt_control(unit, drc_ndx, 0, 1));
        
        rv = soc_dpp_drc_combo28_bist_atomic_action_start(unit, drc_ndx, TRUE); 
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (is_infinite == FALSE && stop == FALSE) {  
        
        rv = soc_dpp_drc_combo28_bist_atomic_action_polling(unit, drc_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
        
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_interrupt_control(unit, drc_ndx, 1, 0));
    }  

    
    if (is_infinite == FALSE || stop == TRUE) {
        
        rv = soc_dpp_drc_combo28_bist_atomic_action_start(unit, drc_ndx, FALSE); 
        SOCDNX_IF_ERR_EXIT(rv);
        
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_interrupt_control(unit, drc_ndx, 1, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_bist_test_start(
        int unit,
        int drc_ndx,
        SOC_DPP_DRC_COMBO28_BIST_INFO *info,
        SOC_DPP_DRC_COMBO28_BIST_MPR_INFO *mpr_info)
{
    int rv = SOC_E_NONE;
    uint32
        i,
        pattern_lcl[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS],
        data_seed_lcl[SOC_DPP_DRC_COMBO28_BIST_NOF_SEEDS],
        bist_timer_us;
        
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 

    if ((mpr_info != NULL) && (mpr_info->mpr_mode == SOC_DPP_DRC_COMBO28_BIST_MPR_MODE_SERIAL)) {
        for (i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = info->pattern[i];
        } 
    } else if ((mpr_info != NULL) && (mpr_info->mpr_mode == SOC_DPP_DRC_COMBO28_BIST_MPR_MODE_PARALLEL)) {
        for (i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            SHR_BITCOPY_RANGE(&pattern_lcl[0], i*4, &(info->pattern[i]), 0, 4);  
        }
        for (i=1; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = pattern_lcl[0];  
        }
    } else if (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0x55555555;
        }
    } else if (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0xffffffff;
        }
    } else if (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0x00000000;
        }
    } else {
        for (i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = info->pattern[i];
        }
    }
    
    if ((info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_USE_RANDOM_DATA_SEED ) != 0x0) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            data_seed_lcl[i] = sal_rand();
        }
    } else {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            data_seed_lcl[i] = info->data_seed[i];
        }    
    }  

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, write_weight=0x%x, read_weight=0x%x, bist_num_actions=0x%x, bist_start_address=0x%x, bist_end_address=0x%x, arad_bist_flags=0x%x\n"), 
                            FUNCTION_NAME(), drc_ndx, info->write_weight, info->read_weight, info->bist_num_actions, info->bist_start_address, info->bist_end_address, info->arad_bist_flags));


    bist_timer_us = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bist_repeat_pattern", 0xffffffff);

    if(((info->arad_bist_flags & (SOC_DPP_DRC_COMBO28_BIST_FLAGS_STOP | SOC_DPP_DRC_COMBO28_BIST_FLAGS_GET_DATA)) == 0)
        && bist_timer_us == 0xffffffff) {
        
        rv = soc_dpp_drc_combo28_configure_bist(
                  unit,
                  drc_ndx,
                  info->write_weight, 
                  info->read_weight, 
                  (info->pattern_mode   == SOC_DPP_DRC_COMBO28_BIST_DATA_PATTERN_BIT_MODE) ? TRUE : FALSE, 
                  (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_TWO_ADDRESS_MODE) ? TRUE : FALSE, 
                  (info->pattern_mode   == SOC_DPP_DRC_COMBO28_BIST_DATA_PATTERN_RANDOM_PRBS) ? TRUE : FALSE,
                  (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_ADDRESS_SHIFT_MODE) ? TRUE : FALSE,                  
                  (info->pattern_mode   == SOC_DPP_DRC_COMBO28_BIST_DATA_PATTERN_SHIFT_MODE) ? TRUE : FALSE,
                  (info->pattern_mode   == SOC_DPP_DRC_COMBO28_BIST_DATA_PATTERN_ADDR_MODE) ? TRUE : FALSE, 
                  (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_INFINITE) ? 0 : info->bist_num_actions,
                  info->bist_start_address, 
                  info->bist_end_address,  
                  (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_BG_INTERLEAVE) ? TRUE : FALSE,
                  (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_SINGLE_BANK_TEST) ? TRUE : FALSE, 
                  (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_ADDRESS_PRBS_MODE) ? TRUE : FALSE,                   
                  pattern_lcl,
                  data_seed_lcl);            
        SOCDNX_IF_ERR_EXIT(rv);
                  
        if ((mpr_info != NULL) && (mpr_info->mpr_mode != 0x0)) {
        
            if(info->write_weight != 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("In MPR BIST write weight must be 0x0. write_weight=0x%x \n"), info->write_weight));
            }       
               
            rv = soc_dpp_drc_combo28_mpr_configure_bist(
                unit,
                drc_ndx,
                (mpr_info->mpr_mode == SOC_DPP_DRC_COMBO28_BIST_MPR_MODE_STAGARED) ? TRUE : FALSE,
                ((mpr_info->mpr_mode == SOC_DPP_DRC_COMBO28_BIST_MPR_MODE_STAGARED) && (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_STAGGER_INCREMENT_MODE)) ? TRUE : FALSE,
                (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_MPR_READOUT_MODE) ? TRUE : FALSE,
                mpr_info->mpr_readout_mpr_location,
                mpr_info->mpr_pattern
            );
            SOCDNX_IF_ERR_EXIT(rv);
        }

    }
    
    if((info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_GET_DATA) == 0) {
        rv = soc_dpp_drc_combo28_bist_atomic_action(
            unit,
            drc_ndx,
            (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_INFINITE) ? TRUE : FALSE,
            (info->arad_bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_STOP) ? TRUE : FALSE);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_gddr5_bist_configure(
    int unit,
    int drc_ndx,
    uint32 fifo_depth,
    uint32 num_commands,
    uint32 bist_mode,
    uint32 adt_enable,
    uint64 bist_data_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS],
    uint8 bist_dbi_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS],
    uint8 bist_edc_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS],
    uint32 bist_data_seeds[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS],
    uint32 bist_dbi_seeds)
{
    uint32 
        reg_val, 
        i;        
    soc_reg_above_64_val_t 
        reg_above64_val, 
        field_above64_val;         
        
    const static soc_reg_t 
        DRC_GDDR_5_BIST_CONFIGURATIONSl[] = {DRCA_GDDR_5_BIST_CONFIGURATIONSr, DRCB_GDDR_5_BIST_CONFIGURATIONSr, DRCC_GDDR_5_BIST_CONFIGURATIONSr,
                                             DRCD_GDDR_5_BIST_CONFIGURATIONSr, DRCE_GDDR_5_BIST_CONFIGURATIONSr, DRCF_GDDR_5_BIST_CONFIGURATIONSr,
                                             DRCG_GDDR_5_BIST_CONFIGURATIONSr, DRCH_GDDR_5_BIST_CONFIGURATIONSr},        
        DRC_GDDR_5_BIST_RD_FIFO_PATTERNl
            [SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = {{DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_0r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_0r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_0r, 
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_0r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_0r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_0r, 
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_0r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_0r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_1r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_1r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_1r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_1r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_1r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_1r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_1r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_1r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_2r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_2r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_2r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_2r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_2r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_2r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_2r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_2r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_3r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_3r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_3r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_3r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_3r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_3r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_3r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_3r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_4r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_4r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_4r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_4r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_4r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_4r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_4r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_4r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_5r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_5r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_5r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_5r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_5r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_5r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_5r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_5r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_6r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_6r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_6r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_6r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_6r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_6r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_6r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_6r},
                                                                                    {DRCA_GDDR_5_BIST_RD_FIFO_PATTERN_7r, DRCB_GDDR_5_BIST_RD_FIFO_PATTERN_7r, DRCC_GDDR_5_BIST_RD_FIFO_PATTERN_7r,
                                                                                     DRCD_GDDR_5_BIST_RD_FIFO_PATTERN_7r, DRCE_GDDR_5_BIST_RD_FIFO_PATTERN_7r, DRCF_GDDR_5_BIST_RD_FIFO_PATTERN_7r,
                                                                                     DRCG_GDDR_5_BIST_RD_FIFO_PATTERN_7r, DRCH_GDDR_5_BIST_RD_FIFO_PATTERN_7r}},
 
        DRC_GDDR_5_BIST_PRBS_DATA_SEED_LSBl[] = {DRCA_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, DRCB_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, DRCC_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, 
                                                  DRCD_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, DRCE_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, DRCF_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, 
                                                  DRCG_GDDR_5_BIST_PRBS_DATA_SEED_LSBr, DRCH_GDDR_5_BIST_PRBS_DATA_SEED_LSBr},
        DRC_GDDR_5_BIST_PRBS_DATA_SEED_MSBl[] = {DRCA_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, DRCB_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, DRCC_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, 
                                                 DRCD_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, DRCE_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, DRCF_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, 
                                                 DRCG_GDDR_5_BIST_PRBS_DATA_SEED_MSBr, DRCH_GDDR_5_BIST_PRBS_DATA_SEED_MSBr},    
        DRC_GDDR_5_BIST_PRBS_DBI_SEEDl[] = {DRCA_GDDR_5_BIST_PRBS_DBI_SEEDr, DRCB_GDDR_5_BIST_PRBS_DBI_SEEDr, DRCC_GDDR_5_BIST_PRBS_DBI_SEEDr, 
                                            DRCD_GDDR_5_BIST_PRBS_DBI_SEEDr, DRCE_GDDR_5_BIST_PRBS_DBI_SEEDr, DRCF_GDDR_5_BIST_PRBS_DBI_SEEDr, 
                                            DRCG_GDDR_5_BIST_PRBS_DBI_SEEDr, DRCH_GDDR_5_BIST_PRBS_DBI_SEEDr},
        DRC_GDDR_5_BIST_FINISH_PRDl[] = {DRCA_GDDR_5_BIST_FINISH_PRDr, DRCB_GDDR_5_BIST_FINISH_PRDr, DRCC_GDDR_5_BIST_FINISH_PRDr, 
                                         DRCD_GDDR_5_BIST_FINISH_PRDr, DRCE_GDDR_5_BIST_FINISH_PRDr, DRCF_GDDR_5_BIST_FINISH_PRDr, 
                                         DRCG_GDDR_5_BIST_FINISH_PRDr, DRCH_GDDR_5_BIST_FINISH_PRDr},
        DRC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSl[] = {DRCA_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCB_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr,
                                                       DRCD_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCE_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCF_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr,
                                                       DRCG_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCH_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr},
        DRC_GDDR_5_SPECIAL_CMD_TIMINGl[] = {DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, DRCB_GDDR_5_SPECIAL_CMD_TIMINGr, DRCC_GDDR_5_SPECIAL_CMD_TIMINGr,
                                            DRCD_GDDR_5_SPECIAL_CMD_TIMINGr, DRCE_GDDR_5_SPECIAL_CMD_TIMINGr, DRCF_GDDR_5_SPECIAL_CMD_TIMINGr,
                                            DRCG_GDDR_5_SPECIAL_CMD_TIMINGr, DRCH_GDDR_5_SPECIAL_CMD_TIMINGr},
        DRC_GDDR_5_BIST_ADT_PATTERNS_0l[] = {DRCA_GDDR_5_BIST_ADT_PATTERNS_0r, DRCB_GDDR_5_BIST_ADT_PATTERNS_0r, DRCC_GDDR_5_BIST_ADT_PATTERNS_0r,      
                                             DRCD_GDDR_5_BIST_ADT_PATTERNS_0r, DRCE_GDDR_5_BIST_ADT_PATTERNS_0r, DRCF_GDDR_5_BIST_ADT_PATTERNS_0r,      
                                             DRCG_GDDR_5_BIST_ADT_PATTERNS_0r, DRCH_GDDR_5_BIST_ADT_PATTERNS_0r},
        DRC_GDDR_5_BIST_ADT_PATTERNS_1l[] = {DRCA_GDDR_5_BIST_ADT_PATTERNS_1r, DRCB_GDDR_5_BIST_ADT_PATTERNS_1r, DRCC_GDDR_5_BIST_ADT_PATTERNS_1r,      
                                             DRCD_GDDR_5_BIST_ADT_PATTERNS_1r, DRCE_GDDR_5_BIST_ADT_PATTERNS_1r, DRCF_GDDR_5_BIST_ADT_PATTERNS_1r,      
                                             DRCG_GDDR_5_BIST_ADT_PATTERNS_1r, DRCH_GDDR_5_BIST_ADT_PATTERNS_1r},
        DRC_GDDR_5_BIST_PRBS_ADT_SEEDl[] = {DRCA_GDDR_5_BIST_PRBS_ADT_SEEDr, DRCB_GDDR_5_BIST_PRBS_ADT_SEEDr, DRCC_GDDR_5_BIST_PRBS_ADT_SEEDr,                                                                               
                                            DRCD_GDDR_5_BIST_PRBS_ADT_SEEDr, DRCE_GDDR_5_BIST_PRBS_ADT_SEEDr, DRCF_GDDR_5_BIST_PRBS_ADT_SEEDr,                                                                               
                                            DRCG_GDDR_5_BIST_PRBS_ADT_SEEDr, DRCH_GDDR_5_BIST_PRBS_ADT_SEEDr};

     const static soc_field_t 
         GDDR_5_BIST_RD_FIFO_DATA_PATTERN_fieldl[] = {GDDR_5_BIST_RD_FIFO_DATA_PATTERN_0f, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_1f, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_2f, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_3f,
                                                      GDDR_5_BIST_RD_FIFO_DATA_PATTERN_4f, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_5f, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_6f, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_7f},
         GDDR_5_BIST_RD_FIFO_DBI_PATTERN_fieldl[] = {GDDR_5_BIST_RD_FIFO_DBI_PATTERN_0f, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_1f, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_2f, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_3f,
                                                     GDDR_5_BIST_RD_FIFO_DBI_PATTERN_4f, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_5f, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_6f, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_7f},    
         GDDR_5_BIST_RD_FIFO_EDC_PATTERN_fieldl[] = {GDDR_5_BIST_RD_FIFO_EDC_PATTERN_0f, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_1f, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_2f, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_3f,
                                                     GDDR_5_BIST_RD_FIFO_EDC_PATTERN_4f, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_5f, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_6f, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_7f},        
         DRC_GDDR_5_BIST_ADT_PATTERNS_fieldl[] = {GDDR_5_BIST_ADT_PATTERN_0f, GDDR_5_BIST_ADT_PATTERN_1f, GDDR_5_BIST_ADT_PATTERN_2f, GDDR_5_BIST_ADT_PATTERN_3f,
                                                  GDDR_5_BIST_ADT_PATTERN_4f, GDDR_5_BIST_ADT_PATTERN_5f, GDDR_5_BIST_ADT_PATTERN_6f, GDDR_5_BIST_ADT_PATTERN_7f};   

    SOCDNX_INIT_FUNC_DEFS;   

    if(SOC_IS_DPP_DRC_COMBO28(unit) == FALSE){
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, fifo_depth=%d, adt_enable=%d\n"), FUNCTION_NAME(), drc_ndx, fifo_depth, adt_enable));    

        
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_FINISH_PRDl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_FINISH_PRDl[drc_ndx], &reg_val, GDDR_5_BIST_FINISH_PRDf, 64);
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_GDDR_5_BIST_FINISH_PRDl[drc_ndx], REG_PORT_ANY, 0, reg_val));
  
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, GDDR_5_RD_FIFO_SIZEf, fifo_depth);
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, GDDR_5_WRITE_WEIGHTf, fifo_depth);
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, GDDR_5_READ_WEIGHTf, fifo_depth);
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, WR_RD_PRBS_MODEf, 1);
    if (adt_enable == TRUE) {
        soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, ADT_PRBS_MODEf, 1);  
    } else {
        soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, ADT_PRBS_MODEf, 0);  
    }
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, GDDR_5_BIST_ENf, FALSE);
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
     soc_reg_field_set(unit, DRC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSl[drc_ndx], &reg_val, GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSf, num_commands);    
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    
    for (i = 0; i < SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_5_BIST_RD_FIFO_PATTERNl[i][drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
        
        SOC_REG_ABOVE_64_CLEAR(field_above64_val);
        reg_val = COMPILER_64_LO(bist_data_pattern[i]);
        SHR_BITCOPY_RANGE(field_above64_val, 0, &reg_val, 0, 32);
        reg_val = COMPILER_64_HI(bist_data_pattern[i]);
        SHR_BITCOPY_RANGE(field_above64_val, 32, &reg_val, 0, 32);
        soc_reg_above_64_field_set(unit, DRC_GDDR_5_BIST_RD_FIFO_PATTERNl[i][drc_ndx], reg_above64_val, GDDR_5_BIST_RD_FIFO_DATA_PATTERN_fieldl[i], field_above64_val);

        SOC_REG_ABOVE_64_CLEAR(field_above64_val);
        reg_val = (uint32)(bist_dbi_pattern[i]);
        SHR_BITCOPY_RANGE(field_above64_val, 0, &reg_val, 0, 8);  
        soc_reg_above_64_field_set(unit, DRC_GDDR_5_BIST_RD_FIFO_PATTERNl[i][drc_ndx], reg_above64_val, GDDR_5_BIST_RD_FIFO_DBI_PATTERN_fieldl[i], field_above64_val);  
        reg_val = (uint32)(bist_edc_pattern[i]);
        SHR_BITCOPY_RANGE(field_above64_val, 0, &reg_val, 0, 8);
        soc_reg_above_64_field_set(unit, DRC_GDDR_5_BIST_RD_FIFO_PATTERNl[i][drc_ndx], reg_above64_val, GDDR_5_BIST_RD_FIFO_EDC_PATTERN_fieldl[i], field_above64_val);      
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_BIST_RD_FIFO_PATTERNl[i][drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    }
  
    
    sal_memset(reg_above64_val,0,sizeof(soc_reg_above_64_val_t));
    
    sal_memcpy(reg_above64_val, bist_data_seeds, sizeof(uint32)*4 );
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit,  DRC_GDDR_5_BIST_PRBS_DATA_SEED_LSBl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    
    sal_memcpy(reg_above64_val, &(bist_data_seeds[4]), sizeof(uint32)*4 );
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_BIST_PRBS_DATA_SEED_MSBl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_PRBS_DBI_SEEDl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
     soc_reg_field_set(unit, DRC_GDDR_5_BIST_PRBS_DBI_SEEDl[drc_ndx], &reg_val, GDDR_5_BIST_PRBS_DBI_SEEDf, bist_dbi_seeds);    
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_GDDR_5_BIST_PRBS_DBI_SEEDl[drc_ndx], REG_PORT_ANY, 0, reg_val));
   
       
    if (adt_enable == TRUE) {
    
        SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
        SOC_REG_ABOVE_64_CLEAR(field_above64_val);
        for (i = 0; i < (SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS / 2); ++i) {
            reg_val = COMPILER_64_LO(bist_data_pattern[i]);
            SHR_BITCOPY_RANGE(field_above64_val, 0, &reg_val, 0, 20);  
            soc_reg_above_64_field_set(unit, DRC_GDDR_5_BIST_ADT_PATTERNS_0l[drc_ndx], reg_above64_val, DRC_GDDR_5_BIST_ADT_PATTERNS_fieldl[i], field_above64_val);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_BIST_ADT_PATTERNS_0l[drc_ndx], REG_PORT_ANY, 0, reg_above64_val)); 

        for (i = (SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS / 2); i < SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            reg_val = COMPILER_64_LO(bist_data_pattern[i]);
            SHR_BITCOPY_RANGE(field_above64_val, 0, &reg_val, 0, 20); 
            soc_reg_above_64_field_set(unit, DRC_GDDR_5_BIST_ADT_PATTERNS_1l[drc_ndx], reg_above64_val, DRC_GDDR_5_BIST_ADT_PATTERNS_fieldl[i], field_above64_val);              
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_BIST_ADT_PATTERNS_1l[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
               
        
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_GDDR_5_BIST_PRBS_ADT_SEEDl[drc_ndx], REG_PORT_ANY, 0, bist_data_seeds[0]));
    }
 
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Set timing configurations\n"), FUNCTION_NAME()));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    if (adt_enable == TRUE) {  
        field_above64_val[0]=31;
        soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, ADT_RD_2_RD_PRDf, field_above64_val); 
        field_above64_val[0]=35;
        soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, ADT_RD_DLY_PRDf, field_above64_val);                  
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_gddr5_bist_atomic_action_start(
    int unit,
    int drc_ndx,
    uint8 start)
{
    uint32
        reg_val;
    const static soc_reg_t drc_bist_configurations[] = {DRCA_GDDR_5_BIST_CONFIGURATIONSr, DRCB_GDDR_5_BIST_CONFIGURATIONSr, DRCC_GDDR_5_BIST_CONFIGURATIONSr,
        DRCD_GDDR_5_BIST_CONFIGURATIONSr, DRCE_GDDR_5_BIST_CONFIGURATIONSr, DRCF_GDDR_5_BIST_CONFIGURATIONSr,
        DRCG_GDDR_5_BIST_CONFIGURATIONSr, DRCH_GDDR_5_BIST_CONFIGURATIONSr};

    SOCDNX_INIT_FUNC_DEFS;
  
    if(SOC_IS_DPP_DRC_COMBO28(unit) == FALSE){
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 

    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, drc_bist_configurations[drc_ndx], REG_PORT_ANY, 0, &reg_val));
     soc_reg_field_set(unit,drc_bist_configurations[drc_ndx], &reg_val, GDDR_5_BIST_ENf, FALSE);    
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, drc_bist_configurations[drc_ndx], REG_PORT_ANY, 0, reg_val));     

  
    if (start == TRUE) {
    
        SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, drc_bist_configurations[drc_ndx], REG_PORT_ANY, 0, &reg_val));
         soc_reg_field_set(unit, drc_bist_configurations[drc_ndx], &reg_val, GDDR_5_BIST_ENf, TRUE);    
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, drc_bist_configurations[drc_ndx], REG_PORT_ANY, 0, reg_val));  
    }

exit:
  SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_gddr5_bist_atomic_action_polling(int unit, int drc_ndx)
{
    uint32 soc_sand_rv;
    const static soc_reg_t drc_bist_statuses[] = {DRCA_GDDR_5_BIST_STATUSESr, DRCB_GDDR_5_BIST_STATUSESr, DRCC_GDDR_5_BIST_STATUSESr, DRCD_GDDR_5_BIST_STATUSESr, DRCE_GDDR_5_BIST_STATUSESr, DRCF_GDDR_5_BIST_STATUSESr, DRCG_GDDR_5_BIST_STATUSESr, DRCH_GDDR_5_BIST_STATUSESr};

    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_IS_DPP_DRC_COMBO28(unit) == FALSE){
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 

    soc_sand_rv = arad_polling(
            unit,
            ARAD_TIMEOUT,
            SOC_DPP_DRC_COMBO28_BIST_MIN_POLLS,
            drc_bist_statuses[drc_ndx],
            REG_PORT_ANY,
            0,
            GDDR_5_BIST_FINISHEDf,
            1
          );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    } 

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_gddr5_bist_atomic_action(
        int unit,
        int drc_ndx,
        SHMOO_COMBO28_GDDR5_BIST_MODE bist_mode)
{
    int rv = SOC_E_NONE; 
    uint32 reg_val;    
    const static soc_reg_t 
        DRC_GDDR_5_BIST_CONFIGURATIONSl[] = {DRCA_GDDR_5_BIST_CONFIGURATIONSr, DRCB_GDDR_5_BIST_CONFIGURATIONSr, DRCC_GDDR_5_BIST_CONFIGURATIONSr,
                                             DRCD_GDDR_5_BIST_CONFIGURATIONSr, DRCE_GDDR_5_BIST_CONFIGURATIONSr, DRCF_GDDR_5_BIST_CONFIGURATIONSr,
                                             DRCG_GDDR_5_BIST_CONFIGURATIONSr, DRCH_GDDR_5_BIST_CONFIGURATIONSr};   

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 

    
    
     SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], &reg_val, GDDR_5_BIST_MODEf, bist_mode);
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    
    
    rv = soc_dpp_drc_combo28_gddr5_bist_atomic_action_start(unit, drc_ndx, TRUE); 
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dpp_drc_combo28_gddr5_bist_atomic_action_polling(unit, drc_ndx);
    SOCDNX_IF_ERR_EXIT(rv);
      
    
    rv = soc_dpp_drc_combo28_gddr5_bist_atomic_action_start(unit, drc_ndx, FALSE); 
    SOCDNX_IF_ERR_EXIT(rv);
  
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): Bist Atomic action finished \n"), FUNCTION_NAME()));    

exit:
  SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_gddr5_bist_read_err_cnt(
    int unit,
    int drc_ndx,
    uint32 *bist_data_err_occur,
    uint32 *bist_dbi_err_occur,
    uint32 *bist_edc_err_occur,
    uint32 *bist_adt_err_occur)
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64_val;
    const static soc_reg_t 
        DRC_GDDR_5_BIST_DATA_ERR_OCCUREDl[] = {DRCA_GDDR_5_BIST_DATA_ERR_OCCUREDr, DRCB_GDDR_5_BIST_DATA_ERR_OCCUREDr, DRCC_GDDR_5_BIST_DATA_ERR_OCCUREDr, 
                                               DRCD_GDDR_5_BIST_DATA_ERR_OCCUREDr, DRCE_GDDR_5_BIST_DATA_ERR_OCCUREDr, DRCF_GDDR_5_BIST_DATA_ERR_OCCUREDr, 
                                               DRCG_GDDR_5_BIST_DATA_ERR_OCCUREDr, DRCH_GDDR_5_BIST_DATA_ERR_OCCUREDr},
        DRC_GDDR_5_BIST_DBI_ERR_OCCUREDl[] = {DRCA_GDDR_5_BIST_DBI_ERR_OCCUREDr, DRCB_GDDR_5_BIST_DBI_ERR_OCCUREDr, DRCC_GDDR_5_BIST_DBI_ERR_OCCUREDr, 
                                              DRCD_GDDR_5_BIST_DBI_ERR_OCCUREDr, DRCE_GDDR_5_BIST_DBI_ERR_OCCUREDr, DRCF_GDDR_5_BIST_DBI_ERR_OCCUREDr, 
                                              DRCG_GDDR_5_BIST_DBI_ERR_OCCUREDr, DRCH_GDDR_5_BIST_DBI_ERR_OCCUREDr},   
        DRC_GDDR_5_BIST_EDC_ERR_OCCURED[] = {DRCA_GDDR_5_BIST_EDC_ERR_OCCUREDr, DRCB_GDDR_5_BIST_EDC_ERR_OCCUREDr, DRCC_GDDR_5_BIST_EDC_ERR_OCCUREDr, 
                                             DRCD_GDDR_5_BIST_EDC_ERR_OCCUREDr, DRCE_GDDR_5_BIST_EDC_ERR_OCCUREDr, DRCF_GDDR_5_BIST_EDC_ERR_OCCUREDr, 
                                             DRCG_GDDR_5_BIST_EDC_ERR_OCCUREDr, DRCH_GDDR_5_BIST_EDC_ERR_OCCUREDr},
        DRC_GDDR_5_BIST_ADT_ERR_OCCURED[] = {DRCA_GDDR_5_BIST_ADT_ERR_OCCUREDr, DRCB_GDDR_5_BIST_ADT_ERR_OCCUREDr, DRCC_GDDR_5_BIST_ADT_ERR_OCCUREDr, 
                                             DRCD_GDDR_5_BIST_ADT_ERR_OCCUREDr, DRCE_GDDR_5_BIST_ADT_ERR_OCCUREDr, DRCF_GDDR_5_BIST_ADT_ERR_OCCUREDr, 
                                             DRCG_GDDR_5_BIST_ADT_ERR_OCCUREDr, DRCH_GDDR_5_BIST_ADT_ERR_OCCUREDr},
        DRC_GDDR_BIST_LAST_READ_DATA_LSBl[] = {DRCA_GDDR_BIST_LAST_READ_DATA_LSBr, DRCB_GDDR_BIST_LAST_READ_DATA_LSBr, DRCC_GDDR_BIST_LAST_READ_DATA_LSBr, 
                                               DRCD_GDDR_BIST_LAST_READ_DATA_LSBr, DRCE_GDDR_BIST_LAST_READ_DATA_LSBr, DRCF_GDDR_BIST_LAST_READ_DATA_LSBr, 
                                               DRCG_GDDR_BIST_LAST_READ_DATA_LSBr, DRCH_GDDR_BIST_LAST_READ_DATA_LSBr},
        DRC_GDDR_BIST_LAST_READ_DATA_MSBl[] = {DRCA_GDDR_BIST_LAST_READ_DATA_MSBr, DRCB_GDDR_BIST_LAST_READ_DATA_MSBr, DRCC_GDDR_BIST_LAST_READ_DATA_MSBr, 
                                               DRCD_GDDR_BIST_LAST_READ_DATA_MSBr, DRCE_GDDR_BIST_LAST_READ_DATA_MSBr, DRCF_GDDR_BIST_LAST_READ_DATA_MSBr, 
                                               DRCG_GDDR_BIST_LAST_READ_DATA_MSBr, DRCH_GDDR_BIST_LAST_READ_DATA_MSBr};                                                     
        
    SOCDNX_INIT_FUNC_DEFS; 
 
    if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    }  
 
    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_DATA_ERR_OCCUREDl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    *bist_data_err_occur = soc_reg_field_get(unit, DRC_GDDR_5_BIST_DATA_ERR_OCCUREDl[drc_ndx], reg_val, GDDR_5_BIST_DATA_ERR_OCCUREDf);   
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_DBI_ERR_OCCUREDl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    *bist_dbi_err_occur = soc_reg_field_get(unit, DRC_GDDR_5_BIST_DBI_ERR_OCCUREDl[drc_ndx], reg_val, GDDR_5_BIST_DBI_ERR_OCCUREDf);  
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit,  DRC_GDDR_5_BIST_EDC_ERR_OCCURED[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    *bist_edc_err_occur = soc_reg_field_get(unit,  DRC_GDDR_5_BIST_EDC_ERR_OCCURED[drc_ndx], reg_val, GDDR_5_BIST_EDC_ERR_OCCUREDf);  
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_GDDR_5_BIST_ADT_ERR_OCCURED[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    *bist_adt_err_occur = soc_reg_field_get(unit, DRC_GDDR_5_BIST_ADT_ERR_OCCURED[drc_ndx], reg_val, GDDR_5_BIST_ADT_ERR_OCCUREDf);  

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_BIST_LAST_READ_DATA_LSBl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): LSB1=0x%08x, LSB0=0x%08x\n"), FUNCTION_NAME(), reg_above64_val[1], reg_above64_val[0]));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_BIST_LAST_READ_DATA_MSBl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): MSB1=0x%08x, MSB0=0x%08x\n"), FUNCTION_NAME(), reg_above64_val[1], reg_above64_val[0]));    

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): *bist_data_err_occur=0x%08x, *bist_dbi_err_occur=0x%08x *bist_dbi_err_occur=0x%08x, *bist_adt_err_occur=0x%x\n"), 
                            FUNCTION_NAME(), *bist_data_err_occur, *bist_dbi_err_occur, *bist_dbi_err_occur, *bist_adt_err_occur)); 

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_gddr5_bist_test_start(
        int unit,
        int drc_ndx,
        SOC_DPP_DRC_COMBO28_GDDR5_BIST_INFO *info)
{
    int rv = SOC_E_NONE, i;
    int repeat_last_bist;
    uint64 lcl_data_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS];
    uint32 lcl_data_seed[SOC_DPP_DRC_COMBO28_BIST_NOF_SEEDS], lcl_dbi_seed;  
    uint8 lcl_dbi_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS], lcl_edc_pattern[SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS];  
      
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_IS_DPP_DRC_COMBO28(unit) == FALSE){
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("MPR BIST can't be done, combo28 is unsupported")));
    } 

    
    if(info->data_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
             COMPILER_64_SET(lcl_data_pattern[i] , 0x0, 0x55555555);
        }
    } else if(info->data_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            COMPILER_64_SET(lcl_data_pattern[i] , 0x0, 0xffffffff);
        }
    } else if(info->data_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            COMPILER_64_SET(lcl_data_pattern[i] , 0x0, 0x00000000);
        }
    } else {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_data_pattern[i] = info->data_pattern[i];
        }
    }

      
    if(info->dbi_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_dbi_pattern[i] = 0x55;
        }
    } else if(info->dbi_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_dbi_pattern[i] = 0xff;
        }
    } else if(info->dbi_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_dbi_pattern[i] = 0x00;
        }
    } else {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_dbi_pattern[i] = info->dbi_pattern[i];
        }
    }     

             
    if(info->edc_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_edc_pattern[i] = 0x55;
        }
    } else if(info->edc_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_edc_pattern[i] = 0xff;
        }
    } else if(info->edc_pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_edc_pattern[i] = 0x00;
        }
    } else {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_edc_pattern[i] = info->edc_pattern[i];
        }
    } 

    
    if((info->bist_flags & SOC_DPP_DRC_COMBO28_BIST_FLAGS_USE_RANDOM_DATA_SEED) != 0) {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_data_seed[i] = sal_rand();
        }
    } else {
        for(i=0; i <  SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
            lcl_data_seed[i] = info->prgs_data_seed[i];
        }
    }

    if((info->bist_mode & SOC_DPP_DRC_COMBO28_BIST_FLAGS_USE_RANDOM_DBI_SEED) != 0) {
        lcl_dbi_seed = sal_rand();
    } else {
        lcl_dbi_seed = info->prgs_dbi_seed;
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, info->bist_flags=0x%x, info->bist_mode=0x%x, SHMOO_COMBO28_GDDR5_BIST_ADDR_TRAINING_MODE=0x%x, info->fifo_depth=%d\n"), 
                            FUNCTION_NAME(), drc_ndx, info->bist_flags, info->bist_mode, SHMOO_COMBO28_GDDR5_BIST_ADDR_TRAINING_MODE, info->fifo_depth));

    repeat_last_bist = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "repeat_last_bist", 0);
    if (repeat_last_bist == 0)
    {
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_gddr5_bist_configure(
                              unit,
                              drc_ndx,
                              info->fifo_depth,
                              info->num_commands,
                              info->bist_mode,
                              (info->bist_mode == SHMOO_COMBO28_GDDR5_BIST_ADDR_TRAINING_MODE) ? TRUE : FALSE,
                                      lcl_data_pattern,
                                      lcl_dbi_pattern,
                                      lcl_edc_pattern,
                                      lcl_data_seed,
                                      lcl_dbi_seed));
    }
     
    
    rv = soc_dpp_drc_combo28_gddr5_bist_atomic_action(unit, drc_ndx, info->bist_mode);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

