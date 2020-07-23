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


#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>


#include <soc/dcmn/error.h>

 
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/DRC/drc_combo28_bist.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>

 
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_dram.h> 


#define DRC_COMBO28_CB_MASK_4_LSB 0xf





int soc_dpp_drc_combo28_shmoo_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_dpp_drc_combo28_phy_reg_read(unit, drc_ndx, addr, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}  

int soc_dpp_drc_combo28_shmoo_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_dpp_drc_combo28_phy_reg_write(unit, drc_ndx, addr, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
} 

int soc_dpp_drc_combo28_shmoo_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_dpp_drc_combo28_phy_reg_modify(unit, drc_ndx, addr, data, mask);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
} 

int soc_dpp_drc_combo16_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo16_bist_info_t info)
{
    combo28_bist_info_t info_internal;
    sal_memcpy(&info_internal, &info, sizeof(info_internal));
    return soc_dpp_drc_combo28_shmoo_drc_bist_conf_set(unit, drc_ndx, info_internal);
}

int soc_dpp_drc_combo28_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo28_bist_info_t info)
{
    int 
        rv = SOC_E_NONE,
        i;
    SOC_DPP_DRC_COMBO28_BIST_INFO bist_info;
    SOC_DPP_DRC_COMBO28_BIST_MPR_INFO mpr_info;
    
    uint32 
        bist_timer_us;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&bist_info, 0, sizeof(bist_info));
    sal_memset(&mpr_info, 0, sizeof(mpr_info));

    bist_info.write_weight       = info.write_weight;
    bist_info.read_weight        = info.read_weight;
    bist_info.bist_start_address = info.bist_start_address;
    bist_info.bist_end_address   = info.bist_end_address;

    for (i = 0; i < SHMOO_COMBO28_BIST_MPR_NOF_PATTERNS; i++) {
        bist_info.pattern[i] = info.data_pattern[i];
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, bist_timer_us=0x%x, mpr_mode=0x%x\n"), 
                            FUNCTION_NAME(), drc_ndx, info.bist_timer_us, info.mpr_mode));

    bist_timer_us = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bist_repeat_pattern", 0xffffffff);
    if(bist_timer_us != 0xffffffff) {
        info.bist_timer_us = bist_timer_us;
    } 
    
    if (info.bist_timer_us == 0x0) {
        
        bist_info.bist_num_actions   = info.bist_num_actions;

        if (info.mpr_mode != 0x0) {
            
            mpr_info.mpr_mode                 = SOC_DPP_DRC_COMBO28_BIST_MPR_MODE_STAGARED;
            mpr_info.mpr_readout_mpr_location = 0;
            

            for (i = 0; i < SHMOO_COMBO28_BIST_MPR_NOF_PATTERNS; i++) {
                mpr_info.mpr_pattern[i] = info.mpr_pattern[i];
            }
            
            rv = soc_dpp_drc_combo28_bist_test_start(unit, drc_ndx, &bist_info, &mpr_info);
            SOCDNX_IF_ERR_EXIT(rv);
            
        } else {

            

            if (info.prbs_mode == 0x1) {
                bist_info.pattern_mode |= SOC_DPP_DRC_COMBO28_BIST_DATA_PATTERN_RANDOM_PRBS;
                bist_info.arad_bist_flags    = SOC_DPP_DRC_COMBO28_BIST_FLAGS_USE_RANDOM_SEED | SOC_DPP_DRC_COMBO28_BIST_FLAGS_BG_INTERLEAVE;
            }

            rv = soc_dpp_drc_combo28_bist_test_start(unit, drc_ndx, &bist_info, NULL);
            SOCDNX_IF_ERR_EXIT(rv);
        }


    } else {
        
        bist_info.arad_bist_flags |= SOC_DPP_DRC_COMBO28_BIST_FLAGS_INFINITE;
         rv = soc_dpp_drc_combo28_bist_test_start(unit, drc_ndx, &bist_info, NULL);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(info.bist_timer_us);

        bist_info.arad_bist_flags = SOC_DPP_DRC_COMBO28_BIST_FLAGS_STOP;
        rv = soc_dpp_drc_combo28_bist_test_start(unit, drc_ndx, &bist_info, NULL);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo16_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo16_bist_err_cnt_t *info)
{
    combo28_bist_err_cnt_t *info_internal;
    info_internal = (combo28_bist_err_cnt_t*)info;
    return soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt(unit, drc_ndx, info_internal);
}

int soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo28_bist_err_cnt_t *info)
{
    uint32 reg_val;
    const static soc_reg_t drc_bist_error_occurred[] = {DRCA_BIST_ERROR_OCCURREDr, DRCB_BIST_ERROR_OCCURREDr,
      DRCC_BIST_ERROR_OCCURREDr, DRCD_BIST_ERROR_OCCURREDr, DRCE_BIST_ERROR_OCCURREDr,
      DRCF_BIST_ERROR_OCCURREDr, DRCG_BIST_ERROR_OCCURREDr, DRCH_BIST_ERROR_OCCURREDr};
    const static soc_reg_t drc_bist_full_mask_error_counter[] = {DRCA_BIST_FULL_MASK_ERROR_COUNTERr, DRCB_BIST_FULL_MASK_ERROR_COUNTERr,
      DRCC_BIST_FULL_MASK_ERROR_COUNTERr, DRCD_BIST_FULL_MASK_ERROR_COUNTERr, DRCE_BIST_FULL_MASK_ERROR_COUNTERr,
      DRCF_BIST_FULL_MASK_ERROR_COUNTERr, DRCG_BIST_FULL_MASK_ERROR_COUNTERr, DRCH_BIST_FULL_MASK_ERROR_COUNTERr};
    const static soc_reg_t drc_bist_single_bit_mask_error_counter[] = {DRCA_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCB_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr,
      DRCC_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCD_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCE_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr,
      DRCF_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCG_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCH_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr};
    const static soc_reg_t drc_bist_global_error_counter[] = {DRCA_BIST_GLOBAL_ERROR_COUNTERr, DRCB_BIST_GLOBAL_ERROR_COUNTERr,
      DRCC_BIST_GLOBAL_ERROR_COUNTERr, DRCD_BIST_GLOBAL_ERROR_COUNTERr, DRCE_BIST_GLOBAL_ERROR_COUNTERr,
      DRCF_BIST_GLOBAL_ERROR_COUNTERr, DRCG_BIST_GLOBAL_ERROR_COUNTERr, DRCH_BIST_GLOBAL_ERROR_COUNTERr};
    const static soc_reg_t drc_cnt_bist_dbi_err_global[] = {DRCA_CNT_BIST_DBI_ERR_GLOBALr, DRCB_CNT_BIST_DBI_ERR_GLOBALr, 
      DRCC_CNT_BIST_DBI_ERR_GLOBALr, DRCD_CNT_BIST_DBI_ERR_GLOBALr, DRCE_CNT_BIST_DBI_ERR_GLOBALr, 
      DRCF_CNT_BIST_DBI_ERR_GLOBALr, DRCG_CNT_BIST_DBI_ERR_GLOBALr, DRCH_CNT_BIST_DBI_ERR_GLOBALr};
    const static soc_reg_t drc_bist_dbi_err_occured[] = {DRCA_BIST_DBI_ERR_OCCUREDr, DRCB_BIST_DBI_ERR_OCCUREDr, 
      DRCC_BIST_DBI_ERR_OCCUREDr, DRCD_BIST_DBI_ERR_OCCUREDr, DRCE_BIST_DBI_ERR_OCCUREDr, 
      DRCF_BIST_DBI_ERR_OCCUREDr, DRCG_BIST_DBI_ERR_OCCUREDr, DRCH_BIST_DBI_ERR_OCCUREDr};
    const static soc_reg_t drc_cnt_bist_edc_err_global[] = {DRCA_CNT_BIST_EDC_ERR_GLOBALr, DRCB_CNT_BIST_EDC_ERR_GLOBALr, 
      DRCC_CNT_BIST_EDC_ERR_GLOBALr, DRCD_CNT_BIST_EDC_ERR_GLOBALr, DRCE_CNT_BIST_EDC_ERR_GLOBALr, 
      DRCF_CNT_BIST_EDC_ERR_GLOBALr, DRCG_CNT_BIST_EDC_ERR_GLOBALr, DRCH_CNT_BIST_EDC_ERR_GLOBALr};
    const static soc_reg_t drc_bist_edc_err_occured[] = {DRCA_BIST_EDC_ERR_OCCUREDr, DRCB_BIST_EDC_ERR_OCCUREDr, 
      DRCC_BIST_EDC_ERR_OCCUREDr, DRCD_BIST_EDC_ERR_OCCUREDr, DRCE_BIST_EDC_ERR_OCCUREDr, 
      DRCF_BIST_EDC_ERR_OCCUREDr, DRCG_BIST_EDC_ERR_OCCUREDr, DRCH_BIST_EDC_ERR_OCCUREDr};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_bist_error_occurred[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_err_occur = soc_reg_field_get(unit, DRCA_BIST_ERROR_OCCURREDr, reg_val, ERR_OCCURREDf);
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_bist_full_mask_error_counter[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_full_err_cnt = soc_reg_field_get(unit, DRCA_BIST_FULL_MASK_ERROR_COUNTERr, reg_val, FULL_ERR_CNTf); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_bist_single_bit_mask_error_counter[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_single_err_cnt = soc_reg_field_get(unit, DRCA_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, reg_val, SINGLE_ERR_CNTf);
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_bist_global_error_counter[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_global_err_cnt = soc_reg_field_get(unit, DRCA_BIST_GLOBAL_ERROR_COUNTERr, reg_val, GLOBAL_ERR_CNTf); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_cnt_bist_dbi_err_global[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_dbi_global_err_cnt = soc_reg_field_get(unit, DRCA_CNT_BIST_DBI_ERR_GLOBALr, reg_val, CNT_BIST_DBI_ERR_GLOBALf); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_bist_dbi_err_occured[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_dbi_err_occur = soc_reg_field_get(unit, DRCA_BIST_DBI_ERR_OCCUREDr, reg_val, BIST_DBI_ERR_OCCUREDf); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_cnt_bist_edc_err_global[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_edc_global_err_cnt = soc_reg_field_get(unit, DRCA_CNT_BIST_EDC_ERR_GLOBALr, reg_val, CNT_BIST_EDC_ERR_GLOBALf); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, drc_bist_edc_err_occured[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    info->bist_edc_err_occur = soc_reg_field_get(unit, DRCA_BIST_EDC_ERR_OCCUREDr, reg_val, BIST_EDC_ERR_OCCUREDf); 

 LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): bist_err_occur=0x%x,  bist_full_err_cnt=0x%x,  bist_single_err_cnt=0x%x,  bist_global_err_cnt=0x%x, "
                                                    "bist_dbi_global_err_cnt=0x%x,  bist_dbi_err_occur=0x%x,  bist_edc_global_err_cnt=0x%x,  bist_edc_err_occur=0x%x\n"), 
                                    FUNCTION_NAME(), info->bist_err_occur, info->bist_full_err_cnt, info->bist_single_err_cnt, info->bist_global_err_cnt,     
                                                     info->bist_dbi_global_err_cnt, info->bist_dbi_err_occur, info->bist_edc_global_err_cnt, info->bist_edc_err_occur)); 

exit:
    SOCDNX_FUNC_RETURN;
}




int soc_dpp_drc_combo28_shmoo_dram_init(int unit, int drc_ndx, int phase)
{
    uint32 
        reg_val,
        dram_type;
    uint64 reg64_val;
    int 
        rv = SOC_E_NONE, 
        init_phase, 
        wck_phase;
    static soc_reg_t
        DRC_DRAM_MR_3l[] = {DRCA_DRAM_MR_3r, DRCB_DRAM_MR_3r, DRCC_DRAM_MR_3r,
                            DRCD_DRAM_MR_3r, DRCE_DRAM_MR_3r, DRCF_DRAM_MR_3r,
                            DRCG_DRAM_MR_3r, DRCH_DRAM_MR_3r},
        DRC_INITIALIZATION_CONTROLLl[] = {DRCA_INITIALIZATION_CONTROLLr, DRCB_INITIALIZATION_CONTROLLr, DRCC_INITIALIZATION_CONTROLLr, 
                                          DRCD_INITIALIZATION_CONTROLLr, DRCE_INITIALIZATION_CONTROLLr, DRCF_INITIALIZATION_CONTROLLr, 
                                          DRCG_INITIALIZATION_CONTROLLr, DRCH_INITIALIZATION_CONTROLLr},
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};

    SOCDNX_INIT_FUNC_DEFS;
    
    init_phase = phase % 10;
    wck_phase = phase / 10;

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);
    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ddr4_init_zqcl", 0))
        {
            rv = soc_dpp_drc_combo28_shmoo_init_ddr4(unit, drc_ndx);
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            rv = soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init(unit, drc_ndx);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_dpp_drc_combo28_shmoo_wait_dram_init_done(unit, drc_ndx);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } else { 
    
        if (wck_phase == 0) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): drc_ndx=%d, Configure DRC to retain WCK inversion state\n"), FUNCTION_NAME(), drc_ndx));
            
            
        } else if (wck_phase == 1) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): drc_ndx=%d, Configure DRC to wipeout WCK inversion state\n"), FUNCTION_NAME(), drc_ndx));
    
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DRAM_MR_3l[drc_ndx], REG_PORT_ANY, 0, &reg_val));
            reg_val &= 0x3FFF3;
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DRAM_MR_3l[drc_ndx], REG_PORT_ANY, 0, reg_val));
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("Undefined action for WCK inversion state=%d"), wck_phase));
        }
    
        if (init_phase == 0) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): drc_ndx=%d, Configure DRC for full DRAM initialization\n"), FUNCTION_NAME(), drc_ndx));
    
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DRAM_INIT_START_POINTf, 0x0);
            soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DRAM_INIT_FINISHED_POINTf, 0x0);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg_val));
        } else if (init_phase == 1) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): drc_ndx=%d, Configure DRC for first stage of DRAM initialization\n"), FUNCTION_NAME(), drc_ndx));
    
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DRAM_INIT_START_POINTf, 0x0);
            soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DRAM_INIT_FINISHED_POINTf, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg_val));
        } else if (init_phase == 2) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): drc_ndx=%d, Configure DRC for second stage of DRAM initialization\n"), FUNCTION_NAME(), drc_ndx));
    
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DRAM_INIT_START_POINTf, 0x1);
            soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DRAM_INIT_FINISHED_POINTf, 0x0);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("Undefined DRAM Init Phase=%d"), init_phase));
        }
        
        rv = soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init(unit, drc_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
    
        rv = soc_dpp_drc_combo28_shmoo_wait_dram_init_done(unit, drc_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
        
        
        if (init_phase == 0) {
            rv = soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training(unit, drc_ndx, 0x1);
            SOCDNX_IF_ERR_EXIT(rv);
        
            rv = soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training(unit, drc_ndx, 0x0);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
} 

int soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init(int unit, int drc_ndx)
{
    uint32 reg_val;
    static soc_reg_t 
        DRC_INITIALIZATION_CONTROLLl[] = {DRCA_INITIALIZATION_CONTROLLr, DRCB_INITIALIZATION_CONTROLLr, DRCC_INITIALIZATION_CONTROLLr, 
                                          DRCD_INITIALIZATION_CONTROLLr, DRCE_INITIALIZATION_CONTROLLr, DRCF_INITIALIZATION_CONTROLLr, 
                                          DRCG_INITIALIZATION_CONTROLLr, DRCH_INITIALIZATION_CONTROLLr},
        DRC_TRAIN_INIT_TRIGERSl[] = {DRCA_TRAIN_INIT_TRIGERSr, DRCB_TRAIN_INIT_TRIGERSr, DRCC_TRAIN_INIT_TRIGERSr, 
                                     DRCD_TRAIN_INIT_TRIGERSr, DRCE_TRAIN_INIT_TRIGERSr, DRCF_TRAIN_INIT_TRIGERSr, 
                                     DRCG_TRAIN_INIT_TRIGERSr, DRCH_TRAIN_INIT_TRIGERSr};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Initiate dram initialization.\n"), FUNCTION_NAME(), drc_ndx));

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, AUTO_DRAM_INITf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_TRAIN_INIT_TRIGERSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_TRAIN_INIT_TRIGERSr, &reg_val, FORCE_DRAM_INIT_DONEf, 0x0);
    soc_reg_field_set(unit, DRCA_TRAIN_INIT_TRIGERSr, &reg_val, TRIG_DRAM_INITf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_TRAIN_INIT_TRIGERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_reg_field_set(unit, DRCA_TRAIN_INIT_TRIGERSr, &reg_val, TRIG_DRAM_INITf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_TRAIN_INIT_TRIGERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
} 

int soc_dpp_drc_combo28_shmoo_wait_dram_init_done(int unit, int drc_ndx)
{
    int 
        rv = SOC_E_NONE,
        mrs_mask=0,
        mrs_opcode=0;     
    uint32
        soc_sand_rv,
        dram_type;  
    uint64
        reg64_val;
    static soc_reg_t 
        DRC_DRAM_INIT_FINISHEDl[] = {DRCA_DRAM_INIT_FINISHEDr, DRCB_DRAM_INIT_FINISHEDr, DRCC_DRAM_INIT_FINISHEDr, 
                                                  DRCD_DRAM_INIT_FINISHEDr, DRCE_DRAM_INIT_FINISHEDr, DRCF_DRAM_INIT_FINISHEDr, 
                                                  DRCG_DRAM_INIT_FINISHEDr, DRCH_DRAM_INIT_FINISHEDr},
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};     

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Wait dram init finish ....\n"), FUNCTION_NAME(), drc_ndx));

    soc_sand_rv = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          DRC_DRAM_INIT_FINISHEDl[drc_ndx],
          REG_PORT_ANY,
          0,
          DRAM_INIT_FINISHEDf,
          1
        );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);

    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {    
        
        SOC_SAND_SET_BIT(mrs_opcode, 0x1, 7);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 7);
        
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 6, mrs_opcode, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 6, 0, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, dram init finished !!!\n"), FUNCTION_NAME(), drc_ndx));

exit:
    SOCDNX_FUNC_RETURN;
} 

int soc_dpp_drc_combo16_shmoo_drc_pll_set(int unit, int drc_ndx, CONST combo16_drc_pll_t *pll_info)
{                    
    uint32 reg_val;
    uint32 soc_sand_rv;
    soc_reg_above_64_val_t reg_above_64_val;
    
    SOCDNX_INIT_FUNC_DEFS;    
    
    
    SOCDNX_NULL_CHECK(pll_info);

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Power up and reset\n"), FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DDR_PHY_PLL_RESETr, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, PWR_ONf, 0x1);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, PWR_ON_LDOf, 0x1);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, RESETBf, 0x0);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, POST_RESETBf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DDR_PHY_PLL_RESETr, REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRCA_DDR_PHY_PLL_DEBUG_PROGRAMr, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_DEBUG_PROGRAMr, reg_above_64_val, PLL_CTRLf, pll_info->pll_ctrl);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_DEBUG_PROGRAMr, reg_above_64_val, LDO_CTRLf, pll_info->ldo_ctrl);    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRCA_DDR_PHY_PLL_DEBUG_PROGRAMr, REG_PORT_ANY, 0, reg_above_64_val));
    sal_usleep(1);

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DDR_PHY_PLL_RESETr, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, ISO_INf, pll_info->iso_in);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DDR_PHY_PLL_RESETr, REG_PORT_ANY, 0, reg_val));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, NDIV_FRACf, pll_info->ndiv_frac);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, NDIV_INTf,  pll_info->ndiv_int);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, PDIVf,  pll_info->pdiv);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, MDIVf,  pll_info->mdiv);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, SSC_LIMITf,  pll_info->ssc_limit);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, SSC_MODEf,  pll_info->ssc_mode);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, SSC_STEPf,  pll_info->ssc_step);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above_64_val, FREFEFF_INFOf,  pll_info->fref_eff_info);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, REG_PORT_ANY, 0, reg_above_64_val));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRCA_DDR_PHY_PLL_VCO_CTRLr, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_VCO_CTRLr, reg_above_64_val, VCO_SELf,  pll_info->vco_sel);
    soc_reg_above_64_field32_set(unit, DRCA_DDR_PHY_PLL_VCO_CTRLr, reg_above_64_val, EN_CTRLf,  pll_info->en_ctrl);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRCA_DDR_PHY_PLL_VCO_CTRLr, REG_PORT_ANY, 0, reg_above_64_val));
    sal_usleep(1);
          
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Out of Reset\n"), FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRCA_DDR_PHY_PLL_RESETr, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, RESETBf, 0x1);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, POST_RESETBf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRCA_DDR_PHY_PLL_RESETr, REG_PORT_ANY, 0, reg_val));    

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Wait PLL lock ...\n"), FUNCTION_NAME(), drc_ndx));

     soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRCA_DDR_PHY_PLL_STATUSr, REG_PORT_ANY, 0, PLL_LOCKf, 1);
     SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, PLL lock !!!\n"), FUNCTION_NAME(), drc_ndx));
    
exit:
    SOCDNX_FUNC_RETURN;      
}

int soc_dpp_drc_combo28_shmoo_drc_pll_set(int unit, int drc_ndx, CONST combo28_drc_pll_t *pll_info)
{
    uint32 
        reg_val,
        soc_sand_rv;
    soc_reg_above_64_val_t
        reg_above64_val,
        field_above64_val;
    static soc_reg_t 
        DRC_DDR_PHY_PLL_RESETl[] = {DRCA_DDR_PHY_PLL_RESETr, DRCB_DDR_PHY_PLL_RESETr, DRCC_DDR_PHY_PLL_RESETr, 
                                    DRCD_DDR_PHY_PLL_RESETr, DRCE_DDR_PHY_PLL_RESETr, DRCF_DDR_PHY_PLL_RESETr, 
                                    DRCG_DDR_PHY_PLL_RESETr, DRCH_DDR_PHY_PLL_RESETr},
        DRC_DDR_PHY_PLL_CTRLl[] = {DRCA_DDR_PHY_PLL_CTRLr, DRCB_DDR_PHY_PLL_CTRLr, DRCC_DDR_PHY_PLL_CTRLr, 
                                   DRCD_DDR_PHY_PLL_CTRLr, DRCE_DDR_PHY_PLL_CTRLr, DRCF_DDR_PHY_PLL_CTRLr, 
                                   DRCG_DDR_PHY_PLL_CTRLr, DRCH_DDR_PHY_PLL_CTRLr},
        DRC_DDR_PHY_PLL_RCl[] = {DRCA_DDR_PHY_PLL_RCr, DRCB_DDR_PHY_PLL_RCr, DRCC_DDR_PHY_PLL_RCr, 
                                 DRCD_DDR_PHY_PLL_RCr, DRCE_DDR_PHY_PLL_RCr, DRCF_DDR_PHY_PLL_RCr, 
                                 DRCG_DDR_PHY_PLL_RCr, DRCH_DDR_PHY_PLL_RCr},
        DRC_DDR_PHY_PLL_FREQ_CTRLl[] = {DRCA_DDR_PHY_PLL_FREQ_CTRLr, DRCB_DDR_PHY_PLL_FREQ_CTRLr, DRCC_DDR_PHY_PLL_FREQ_CTRLr, 
                                        DRCD_DDR_PHY_PLL_FREQ_CTRLr, DRCE_DDR_PHY_PLL_FREQ_CTRLr, DRCF_DDR_PHY_PLL_FREQ_CTRLr, 
                                        DRCG_DDR_PHY_PLL_FREQ_CTRLr, DRCH_DDR_PHY_PLL_FREQ_CTRLr},
        DRC_DDR_PHY_PLL_STATUSl[] = {DRCA_DDR_PHY_PLL_STATUSr, DRCB_DDR_PHY_PLL_STATUSr, DRCC_DDR_PHY_PLL_STATUSr, 
                                     DRCD_DDR_PHY_PLL_STATUSr, DRCE_DDR_PHY_PLL_STATUSr, DRCF_DDR_PHY_PLL_STATUSr, 
                                     DRCG_DDR_PHY_PLL_STATUSr, DRCH_DDR_PHY_PLL_STATUSr};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(pll_info);

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Power up and reset\n"), FUNCTION_NAME(), drc_ndx));

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DDR_PHY_PLL_RESETl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, PWR_ONf, 0x1);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, PWR_ON_LDOf, 0x1);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, RESETBf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DDR_PHY_PLL_RESETl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_DDR_PHY_PLL_CTRLl[drc_ndx], REG_PORT_ANY, 0, pll_info->pll_ctrl));

    sal_usleep(1);

    reg_val = 0x0;
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, ISO_INf, pll_info->iso_in);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, CPf,     pll_info->cp);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, CP_1f,   pll_info->cp1);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, CZf,     pll_info->cz);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, ICPf,    pll_info->icp);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, RPf,     pll_info->rp);
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RCr, &reg_val, RZf,     pll_info->rz);
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DDR_PHY_PLL_RCl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    sal_usleep(1);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->ldo_ctrl, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, LDO_CTRLf, field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->msc_ctrl, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, MSC_CTRLf, field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->ndiv_frac, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, NDIV_FRACf,field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->ndiv_int, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, NDIV_INTf, field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->pdiv, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, PDIVf,     field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->ssc_limit, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, SSC_LIMITf,field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->ssc_mode, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, SSC_MODEf, field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->ssc_step, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, SSC_STEPf, field_above64_val);

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &pll_info->vco_gain , 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_DDR_PHY_PLL_FREQ_CTRLr, reg_above64_val, VCO_GAINf, field_above64_val);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_DDR_PHY_PLL_FREQ_CTRLl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Out of Reset\n"), FUNCTION_NAME(), drc_ndx));

     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DDR_PHY_PLL_RESETl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
     soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, RESETBf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DDR_PHY_PLL_RESETl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Wait PLL lock ...\n"), FUNCTION_NAME(), drc_ndx));

     soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_DDR_PHY_PLL_STATUSl[drc_ndx], REG_PORT_ANY, 0, PLL_LOCKf, 1);
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, PLL lock !!!\n"), FUNCTION_NAME(), drc_ndx));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_modify_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 data, uint32 mask)
{
    int rv = SOC_E_NONE;
    uint32 mrs_opcode;
    uint32 curr_refi = 0;
    static soc_reg_t
        DRC_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
        {{DRCA_DRAM_MR_0r, DRCB_DRAM_MR_0r, DRCC_DRAM_MR_0r, DRCD_DRAM_MR_0r, DRCE_DRAM_MR_0r, DRCF_DRAM_MR_0r, DRCG_DRAM_MR_0r, DRCH_DRAM_MR_0r},
         {DRCA_DRAM_MR_1r, DRCB_DRAM_MR_1r, DRCC_DRAM_MR_1r, DRCD_DRAM_MR_1r, DRCE_DRAM_MR_1r, DRCF_DRAM_MR_1r, DRCG_DRAM_MR_1r, DRCH_DRAM_MR_1r},
         {DRCA_DRAM_MR_2r, DRCB_DRAM_MR_2r, DRCC_DRAM_MR_2r, DRCD_DRAM_MR_2r, DRCE_DRAM_MR_2r, DRCF_DRAM_MR_2r, DRCG_DRAM_MR_2r, DRCH_DRAM_MR_2r},
         {DRCA_DRAM_MR_3r, DRCB_DRAM_MR_3r, DRCC_DRAM_MR_3r, DRCD_DRAM_MR_3r, DRCE_DRAM_MR_3r, DRCF_DRAM_MR_3r, DRCG_DRAM_MR_3r, DRCH_DRAM_MR_3r},
         {DRCA_DRAM_MR_4r, DRCB_DRAM_MR_4r, DRCC_DRAM_MR_4r, DRCD_DRAM_MR_4r, DRCE_DRAM_MR_4r, DRCF_DRAM_MR_4r, DRCG_DRAM_MR_4r, DRCH_DRAM_MR_4r},
         {DRCA_DRAM_MR_5r, DRCB_DRAM_MR_5r, DRCC_DRAM_MR_5r, DRCD_DRAM_MR_5r, DRCE_DRAM_MR_5r, DRCF_DRAM_MR_5r, DRCG_DRAM_MR_5r, DRCH_DRAM_MR_5r},
         {DRCA_DRAM_MR_6r, DRCB_DRAM_MR_6r, DRCC_DRAM_MR_6r, DRCD_DRAM_MR_6r, DRCE_DRAM_MR_6r, DRCF_DRAM_MR_6r, DRCG_DRAM_MR_6r, DRCH_DRAM_MR_6r},
         {DRCA_DRAM_MR_7r, DRCB_DRAM_MR_7r, DRCC_DRAM_MR_7r, DRCD_DRAM_MR_7r, DRCE_DRAM_MR_7r, DRCF_DRAM_MR_7r, DRCG_DRAM_MR_7r, DRCH_DRAM_MR_7r},
         {DRCA_DRAM_MR_8r, DRCB_DRAM_MR_8r, DRCC_DRAM_MR_8r, DRCD_DRAM_MR_8r, DRCE_DRAM_MR_8r, DRCF_DRAM_MR_8r, DRCG_DRAM_MR_8r, DRCH_DRAM_MR_8r},
         {DRCA_DRAM_MR_9r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {DRCA_DRAM_MR_10r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {DRCA_DRAM_MR_11r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {DRCA_DRAM_MR_12r, DRCB_DRAM_MR_12r, DRCC_DRAM_MR_12r, DRCD_DRAM_MR_12r, DRCE_DRAM_MR_12r, DRCF_DRAM_MR_12r, DRCG_DRAM_MR_12r, DRCH_DRAM_MR_12r},
         {DRCA_DRAM_MR_13r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {DRCA_DRAM_MR_14r, DRCB_DRAM_MR_14r, DRCC_DRAM_MR_14r, DRCD_DRAM_MR_14r, DRCE_DRAM_MR_14r, DRCF_DRAM_MR_14r, DRCG_DRAM_MR_14r, DRCH_DRAM_MR_14r},
         {DRCA_DRAM_MR_15r, DRCB_DRAM_MR_15r, DRCC_DRAM_MR_15r, DRCD_DRAM_MR_15r, DRCE_DRAM_MR_15r, DRCF_DRAM_MR_15r, DRCG_DRAM_MR_15r, DRCH_DRAM_MR_15r}};

    static soc_field_t DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX] = 
         {DRAM_MR_0f, DRAM_MR_1f, DRAM_MR_2f, DRAM_MR_3f, DRAM_MR_4f, DRAM_MR_5f, DRAM_MR_6f, DRAM_MR_7f,
          DRAM_MR_8f, INVALIDf, INVALIDf, INVALIDf, DRAM_MR_12f, INVALIDf, DRAM_MR_14f, DRAM_MR_15f};
    
    SOCDNX_INIT_FUNC_DEFS;    
    
    if ((mr_ndx == 9) || (mr_ndx == 10) || (mr_ndx == 11) || (mr_ndx == 13)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mr_ndx=%d is not used.\n"), mr_ndx));
    }

    if ((data > 0xffff) || (mask > 0xffff)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Data/Mask are out 16bit size. data=0x%x, mask=0x%x\n"), data, mask));
    }
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, &mrs_opcode));
    mrs_opcode = soc_reg_field_get(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], mrs_opcode, DRAM_MRl[mr_ndx]);

    mrs_opcode &= ~(mask);
    mrs_opcode |= (data & mask);

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Set mr_ndx=%d with with mrs_opcode=0x%x where data=0x%x and mask=0x%x.\n"), 
                            FUNCTION_NAME(), drc_ndx, mr_ndx, mrs_opcode, data, mask));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_enable_refresh(unit, drc_ndx , 0, 0, &curr_refi));
        
    rv = soc_dpp_drc_combo28_shmoo_load_mrs(unit, drc_ndx, mr_ndx, mrs_opcode);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_enable_refresh(unit, drc_ndx , 1, curr_refi, &curr_refi));
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_drc_enable_adt(int unit, int drc_ndx, int enable)
{
    uint32 
        field_val,
        mrs_mask = 0;
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, Disable refresh and enable ADT\n"), FUNCTION_NAME(), drc_ndx));

        sal_usleep(1);

        
        field_val = 0;
        SOC_SAND_SET_BIT(field_val, 0x1, 10);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 10);
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 15, field_val, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);

    } else {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, Disable ADT\n"), FUNCTION_NAME(), drc_ndx));

        
        field_val = 0;
        SOC_SAND_SET_BIT(field_val, 0x0, 10);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 10);
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 15, field_val, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(1);

    }

exit:
    SOCDNX_FUNC_RETURN;
} 

int soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training(int unit, int drc_ndx, int enable)
{
    int rv = SOC_E_NONE;
    uint32 
        field_val,
        mrs_mask = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, Disable refresh and enable Wck2Ck training\n"), FUNCTION_NAME(), drc_ndx));

        sal_usleep(1);

        
        field_val = 0;
        SOC_SAND_SET_BIT(field_val, 0x1, 4);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 4);
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 3, field_val, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, Disable Wck2Ck training.\n"), FUNCTION_NAME(), drc_ndx));

        
        field_val = 0;
        SOC_SAND_SET_BIT(field_val, 0x0, 4);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 4);
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 3, field_val, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);
        sal_usleep(1);

    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo16_gddr5_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo16_gddr5_bist_info_t info)
{
    combo28_gddr5_bist_info_t info_internal;
    sal_memcpy(&info_internal, &info, sizeof(info_internal));
    return soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set(unit, drc_ndx, info_internal);
}

int soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo28_gddr5_bist_info_t info)
{
    int rv= SOC_E_NONE, i;
    SOC_DPP_DRC_COMBO28_GDDR5_BIST_INFO bist_info;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&bist_info, 0, sizeof(bist_info));

    bist_info.bist_mode         = info.bist_mode;
    bist_info.bist_flags        = SOC_DPP_DRC_COMBO28_BIST_FLAGS_USE_RANDOM_SEED;
    bist_info.fifo_depth        = info.fifo_depth;    
    bist_info.num_commands      = info.num_commands;    
    bist_info.data_pattern_mode = SOC_TMC_DRAM_BIST_DATA_PATTERN_CUSTOM; 
    bist_info.dbi_pattern_mode  = SOC_TMC_DRAM_BIST_DATA_PATTERN_CUSTOM; 
    bist_info.edc_pattern_mode  = SOC_TMC_DRAM_BIST_DATA_PATTERN_CUSTOM; 
    
    for (i=0; i < SOC_DPP_DRC_COMBO28_BIST_NOF_PATTERNS; ++i) {
        bist_info.data_pattern[i] = info.data_pattern[i]; 
        bist_info.dbi_pattern[i] = info.dbi_pattern[i];
        bist_info.edc_pattern[i] = info.edc_pattern[i];
    }

    rv = soc_dpp_drc_combo28_gddr5_bist_test_start(unit, drc_ndx, &bist_info);
    SOCDNX_IF_ERR_EXIT(rv);        

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo16_gddr5_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo16_gddr5_bist_err_cnt_t *info)
{
    combo28_gddr5_bist_err_cnt_t *info_internal;
    info_internal = (combo28_gddr5_bist_err_cnt_t*)info;
    return soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt(unit, drc_ndx, info_internal);
}

int soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo28_gddr5_bist_err_cnt_t *info)
{
    int rv= SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_dpp_drc_combo28_gddr5_bist_read_err_cnt(
        unit, 
        drc_ndx, 
        &(info->bist_data_err_occur), 
        &(info->bist_dbi_err_occur),  
        &(info->bist_edc_err_occur),
        &(info->bist_adt_err_occur));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}
 
int soc_dpp_drc_combo28_shmoo_drc_enable_write_leveling(int unit, int drc_ndx, uint32 command_parity_lattency, int use_continious_gddr5_dqs, int enable)
{
    int rv = SOC_E_NONE;
    uint32 
        dram_type,
        mrs_mask = 0,
        mrs_opcode = 0;
    uint64 
        reg64_val,
        field64_val;
    static soc_reg_t
         DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr,
                                         DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                         DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},
         DRC_CPU_COMMANDSl[] = {DRCA_CPU_COMMANDSr, DRCB_CPU_COMMANDSr, DRCC_CPU_COMMANDSr,
                                DRCD_CPU_COMMANDSr, DRCE_CPU_COMMANDSr, DRCF_CPU_COMMANDSr,
                                DRCG_CPU_COMMANDSr, DRCH_CPU_COMMANDSr},
         DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, use_continious_gddr5_dqs=%d, enable=%d.\n"), 
                            FUNCTION_NAME(), drc_ndx,  use_continious_gddr5_dqs, enable));
    if (enable) {

        
        if (command_parity_lattency == 0) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): Set CommandParityLattency = 4 to unsure only deselect.\n"), FUNCTION_NAME()));
            SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
            COMPILER_64_SET(field64_val, 0x0, 0x1);
            soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, COMMAND_PARITY_LATTENCYf, field64_val);
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));       
        }

        
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): Enable write leveling.\n"), FUNCTION_NAME()));
        SOC_SAND_SET_BIT(mrs_opcode, 0x1, 7);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 7); 
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 1, mrs_opcode, mrs_mask);
        
        sal_usleep(1);

        
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drive ODT high.\n"), FUNCTION_NAME()));
        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
        COMPILER_64_SET(field64_val, 0x0, 0x3);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CS_Nf, field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CS_N_ODDf, field64_val);
        COMPILER_64_SET(field64_val, 0x0, 0x1);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, RESf, field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CKEf, field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ODTf, field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, ODT_ODDf, field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CMD_CONST_VALf, field64_val);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
           
        sal_usleep(1);

        if (use_continious_gddr5_dqs == 1) { 

             
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): Change to GDDR5 mode.\n"), FUNCTION_NAME()));
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
            dram_type  = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);
            if (SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_GDDR5 == dram_type) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s(): Unsupport GDDR5.\n"), FUNCTION_NAME()));    
            }
            soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DIS_DRAM_WCKf, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

            soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DRAM_TYPEf, SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_GDDR5);
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
    
         } else {

             
             LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Drive DQS low.\n"), FUNCTION_NAME()));
             SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_force_dqs(unit, drc_ndx , 0, 1));
         }
    } else {
        if (1 == use_continious_gddr5_dqs) {

            
            LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Change back to DDR4 mode.\n"), FUNCTION_NAME()));
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
            soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DRAM_TYPEf, SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4);
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

        } else {

            
            LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Disable DQS force.\n"), FUNCTION_NAME()));
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_force_dqs(unit, drc_ndx , 0, 0));
        }

        
        LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Disabling command force.\n"), FUNCTION_NAME()));
       SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
        COMPILER_64_ZERO(field64_val);
        soc_reg64_field_set(unit, DRCA_CPU_COMMANDSr, &reg64_val, CMD_CONST_VALf, field64_val);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg64_val));                      

        
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, disable write leveling.\n"), FUNCTION_NAME(), drc_ndx)); 
        SOC_SAND_SET_BIT(mrs_opcode, 0x0, 7);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 7); 
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, 1, mrs_opcode, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);
        
        sal_usleep(1);

        
        if (command_parity_lattency == 0) {
            LOG_VERBOSE(BSL_LS_SOC_DRAM,
                        (BSL_META_U(unit,
                                    "%s(): Retriving drc_CommandParityLattency to 0.\n"), FUNCTION_NAME()));
            SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
            COMPILER_64_SET(field64_val, 0x0, command_parity_lattency); 
            soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, COMMAND_PARITY_LATTENCYf, field64_val);
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
        }

      }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_shmoo_write_mpr(int unit, int drc_ndx, uint32 intial_calib_mpr_addr, uint32 mpr_mode, uint32 mpr_page, uint32 mrs_readout, int enable_mpr)
{
    int rv;
    uint32 
        mrs_mask,
        mrs_opcode,
        mrs_num,
        reg_val;
    static soc_reg_t
          DRC_INTIAL_CALIB_USE_MPRl[] = {DRCA_INTIAL_CALIB_USE_MPRr, DRCB_INTIAL_CALIB_USE_MPRr, DRCC_INTIAL_CALIB_USE_MPRr, 
                                         DRCD_INTIAL_CALIB_USE_MPRr, DRCE_INTIAL_CALIB_USE_MPRr, DRCF_INTIAL_CALIB_USE_MPRr, 
                                         DRCG_INTIAL_CALIB_USE_MPRr, DRCH_INTIAL_CALIB_USE_MPRr};
    SOCDNX_INIT_FUNC_DEFS;

    if ((mpr_page != 0) && (mpr_mode != 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("%s(): drc_ndx=%d, Wrong MPR configuration.\n"), FUNCTION_NAME(), drc_ndx));
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, intial_calib_mpr_addr=%d, mpr_mode=%d, mpr_page=0x%x, mrs_readout=0x%x, enable_mpr=%d.\n"), 
                            FUNCTION_NAME(), drc_ndx, intial_calib_mpr_addr, mpr_mode, mpr_page, mrs_readout, enable_mpr));

    if (1 == enable_mpr) {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, Disable auto refresh and active command.\n"), FUNCTION_NAME(), drc_ndx));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, DRCA_INTIAL_CALIB_USE_MPRr, &reg_val, INTIAL_CALIB_READ_MPRf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    }

    mrs_mask  = SOC_SAND_SET_BITS_RANGE(0x7, 2, 0);
    mrs_mask |= SOC_SAND_SET_BITS_RANGE(0x1, 5, 5);
    mrs_mask |= SOC_SAND_SET_BITS_RANGE(0x3, 12, 11);
    mrs_opcode  = SOC_SAND_SET_BITS_RANGE(mpr_page, 1, 0);
    mrs_opcode |= SOC_SAND_SET_BITS_RANGE(enable_mpr, 2, 2);
    mrs_opcode |= SOC_SAND_SET_BITS_RANGE(mrs_readout, 5, 5);
    mrs_opcode |= SOC_SAND_SET_BITS_RANGE(mpr_mode, 12, 11);
    mrs_num = 3;
    rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
    SOCDNX_IF_ERR_EXIT(rv);
  
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    SOC_SAND_SET_BIT(intial_calib_mpr_addr, 0x1, 12);  
    soc_reg_field_set(unit, DRCA_INTIAL_CALIB_USE_MPRr, &reg_val, INTIAL_CALIB_MPR_ADDRf, intial_calib_mpr_addr);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, reg_val));
  
    if (0 == enable_mpr) {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, Enable auto refresh and active.\n"), FUNCTION_NAME(), drc_ndx));
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, DRCA_INTIAL_CALIB_USE_MPRr, &reg_val, INTIAL_CALIB_READ_MPRf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INTIAL_CALIB_USE_MPRl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    }
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Finished write MPR.\n"), FUNCTION_NAME(), drc_ndx));
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_drc_mpr_en(int unit, int drc_ndx, int enable)
{
    int mrs_num,
        rv;
    uint32 
        mpr_mode,
        mrs_mask = 0,
        mrs_opcode = 0;
    uint64 reg64_val,
        field64_val;
    static soc_reg_t
          DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr,
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr};

    SOCDNX_INIT_FUNC_DEFS;
  
    if (1 == enable) {

        mpr_mode = SOC_DPP_DRC_COMBO28_BIST_MPR_MODE_STAGARED;
 
        
        rv = soc_dpp_drc_combo28_shmoo_write_mpr(
            unit, 
            drc_ndx, 
            0x0,                                        
            mpr_mode,
            0x0,                                        
            0x0,                                        
            0x1);
        SOCDNX_IF_ERR_EXIT(rv);

        
        sal_usleep(1);

        SOC_SAND_SET_BIT(mrs_opcode, 0x0, 12);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 12);
        mrs_num = 5;
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);
        
        sal_usleep(1);

    } else {

        
        rv = soc_dpp_drc_combo28_shmoo_write_mpr(
            unit, 
            drc_ndx, 
            0x0,                                        
            0x0,                                        
            0x0,                                        
            0x0,                                        
            0x0);
        SOCDNX_IF_ERR_EXIT(rv);

        
        sal_usleep(1);
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
        field64_val = soc_reg64_field_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx],reg64_val, READ_DBIf);
        SOC_SAND_SET_BIT(mrs_opcode, COMPILER_64_LO(field64_val), 12);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 12);
        mrs_num = 5; 
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(1);

    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_shmoo_mpr_load(int unit, int drc_ndx, uint8 *mpr_pattern)
{
    int 
        rv,
        mpr_location;
    uint32 
        cpu_addr,
        bank;


    SOCDNX_INIT_FUNC_DEFS;

    for (mpr_location = 0; mpr_location < SHMOO_COMBO28_BIST_MPR_NOF_PATTERNS; mpr_location++) {

        bank = 0;
        bank |= SOC_SAND_SET_BITS_RANGE(mpr_location, 2, 0);
        cpu_addr = 0;
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(mpr_pattern[mpr_location], 7, 0);

        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, mpr_location=%d, mpr_pattern=%d \n"), FUNCTION_NAME(), drc_ndx, mpr_location, mpr_pattern[mpr_location]));
        rv = soc_dpp_drc_combo28_dram_cpu_command(unit, drc_ndx, 1 , 0 , 0 , 1 , bank , cpu_addr );
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_shmoo_drc_active_gddr5_cmd(int unit, int drc_ndx)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): Activate command.\n"), FUNCTION_NAME()));    
    rv = soc_dpp_drc_combo28_dram_cpu_command(unit, drc_ndx, 0 , 1 , 1 , 1 , 0 , 0 );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_drc_precharge_all(int unit, int drc_ndx)
{
    uint32 cpu_addr = 0,
        reg_val,
        ap_bit_pos;
    int rv;

    static soc_reg_t
        DRC_GENERAL_CONFIGURATIONSl[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr, DRCC_GENERAL_CONFIGURATIONSr,
                                        DRCD_GENERAL_CONFIGURATIONSr, DRCE_GENERAL_CONFIGURATIONSr, DRCF_GENERAL_CONFIGURATIONSr,
                                        DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_GENERAL_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    ap_bit_pos = soc_reg_field_get(unit, DRCA_GENERAL_CONFIGURATIONSr, reg_val, AP_BIT_POSf); 
    if (ap_bit_pos == SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_10) {
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(0x1,  10, 10); 
    } else if (ap_bit_pos == SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_8) {
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(0x1,  8, 8);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Undefined Ap bit.\n")));
    }
    rv = soc_dpp_drc_combo28_dram_cpu_command(unit, drc_ndx, 0 , 1 , 0 , 1 , 0 , cpu_addr );
    SOCDNX_IF_ERR_EXIT(rv);

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Perform precharge. ap_bit_pos=0x%x, cpu_addr=0x%x\n"), FUNCTION_NAME(), drc_ndx, ap_bit_pos, cpu_addr));
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_drc_enable_gddr5_training_protocol(int unit, int drc_ndx, int enable)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {

        rv = soc_dpp_drc_combo28_shmoo_drc_active_gddr5_cmd(unit, drc_ndx);
        SOCDNX_IF_ERR_EXIT(rv);

    } else {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s() Disable: issue precharge all and retrive refresh.\n"), FUNCTION_NAME()));
        rv = soc_dpp_drc_combo28_shmoo_drc_precharge_all(unit, drc_ndx);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(1);

    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo16_shmoo_vendor_info_get(int unit, int drc_ndx, combo16_vendor_info_t *info)
{
    combo28_vendor_info_t *info_internal;
    info_internal = (combo28_vendor_info_t*)info;
    return soc_dpp_drc_combo28_shmoo_vendor_info_get(unit, drc_ndx, info_internal);
}

int soc_dpp_drc_combo28_shmoo_vendor_info_get(int unit, int drc_ndx, combo28_vendor_info_t *info)
{
    uint32
        reg_val,
        override_fifo_depth,
        field_val,
        transfromed_value_according_to_byte_swaps,
        original_bist_mode = 3;
    soc_reg_above_64_val_t
        reg_above64_val,
        field_above64_val;
    static soc_reg_t
         DRC_GDDR_5_SPECIAL_CMD_TIMINGl[] = {DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, DRCB_GDDR_5_SPECIAL_CMD_TIMINGr, DRCC_GDDR_5_SPECIAL_CMD_TIMINGr,
                                             DRCD_GDDR_5_SPECIAL_CMD_TIMINGr, DRCE_GDDR_5_SPECIAL_CMD_TIMINGr, DRCF_GDDR_5_SPECIAL_CMD_TIMINGr,
                                             DRCG_GDDR_5_SPECIAL_CMD_TIMINGr, DRCH_GDDR_5_SPECIAL_CMD_TIMINGr},
         DRC_GDDR_5_BIST_CONFIGURATIONSl[] = {DRCA_GDDR_5_BIST_CONFIGURATIONSr, DRCB_GDDR_5_BIST_CONFIGURATIONSr, DRCC_GDDR_5_BIST_CONFIGURATIONSr,
                                              DRCD_GDDR_5_BIST_CONFIGURATIONSr, DRCE_GDDR_5_BIST_CONFIGURATIONSr, DRCF_GDDR_5_BIST_CONFIGURATIONSr,
                                              DRCG_GDDR_5_BIST_CONFIGURATIONSr, DRCH_GDDR_5_BIST_CONFIGURATIONSr},
         DRC_GDDR_BIST_LAST_READ_DATA_LSBl[] = {DRCA_GDDR_BIST_LAST_READ_DATA_LSBr, DRCB_GDDR_BIST_LAST_READ_DATA_LSBr, DRCC_GDDR_BIST_LAST_READ_DATA_LSBr, 
                                                DRCD_GDDR_BIST_LAST_READ_DATA_LSBr, DRCE_GDDR_BIST_LAST_READ_DATA_LSBr, DRCF_GDDR_BIST_LAST_READ_DATA_LSBr, 
                                                DRCG_GDDR_BIST_LAST_READ_DATA_LSBr, DRCH_GDDR_BIST_LAST_READ_DATA_LSBr};
    SOCDNX_INIT_FUNC_DEFS;

    sal_usleep(1);

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Read VENDOR ID.\n"), FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    field_val = 50;
    SHR_BITCOPY_RANGE(field_above64_val, 0, &field_val, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, VENDOR_ID_RD_DLY_PRDf, field_above64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    original_bist_mode = soc_reg_field_get(unit, DRCA_GDDR_5_BIST_CONFIGURATIONSr, reg_val, GDDR_5_BIST_MODEf);
    soc_reg_field_set(unit, DRCA_GDDR_5_BIST_CONFIGURATIONSr, &reg_val, GDDR_5_BIST_MODEf, 4 );
    soc_reg_field_set(unit, DRCA_GDDR_5_BIST_CONFIGURATIONSr, &reg_val, GDDR_5_BIST_ENf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_reg_field_set(unit, DRCA_GDDR_5_BIST_CONFIGURATIONSr, &reg_val, GDDR_5_BIST_ENf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    
    sal_usleep(1);

    soc_reg_field_set(unit, DRCA_GDDR_5_BIST_CONFIGURATIONSr, &reg_val, GDDR_5_BIST_ENf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_BIST_LAST_READ_DATA_LSBl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_transform_value_based_on_byte_swaps(unit, drc_ndx, reg_above64_val[0], &transfromed_value_according_to_byte_swaps));

    info->manufacture_id  = SOC_SAND_GET_BITS_RANGE(transfromed_value_according_to_byte_swaps, 3, 0);
    info->revision_id     = SOC_SAND_GET_BITS_RANGE(transfromed_value_according_to_byte_swaps, 7, 4);
    info->dram_density    = SOC_SAND_GET_BITS_RANGE(transfromed_value_according_to_byte_swaps, 17, 16);
    info->fifo_depth      = SOC_SAND_GET_BITS_RANGE(transfromed_value_according_to_byte_swaps, 19, 18);

    info->fifo_depth += 4;


    override_fifo_depth = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "override_fifo_depth", 0xffffffff);
    if (override_fifo_depth != 0xffffffff) {
        info->fifo_depth = override_fifo_depth;
        LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "override fifo_depth with value 0x%x"), info->fifo_depth));
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_GDDR_5_BIST_CONFIGURATIONSr, &reg_val, GDDR_5_BIST_MODEf, original_bist_mode);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_GDDR_5_BIST_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, reg_above64_val[0]=0x%x, VENDOR ID revision_id = 0x%x manufacture_id = 0x%x dram_density=0x%x fifo_depth 0x%x.\n"), 
                            FUNCTION_NAME(), drc_ndx, reg_above64_val[0], info->revision_id, info->manufacture_id, info->dram_density, info->fifo_depth));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_drc_dqs_pulse_gen(int unit, int drc_ndx, int use_continious_gddr5_dqs)
{
    uint64 reg64_val;
    static soc_reg_t DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                                      DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                                      DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, DQS pulse by use_continious_gddr5_dqs=%d\n"), FUNCTION_NAME(), drc_ndx, use_continious_gddr5_dqs));

    if (use_continious_gddr5_dqs) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
        soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DIS_DRAM_WCKf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
        soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DIS_DRAM_WCKf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_force_dqs(unit, drc_ndx , 1, 1));
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_force_dqs(unit, drc_ndx , 0, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
 
int soc_dpp_drc_combo28_gddr5_shmoo_drc_dq_byte_pairs_swap_info_get(int unit, int drc_ndx, int* pairs_were_swapped)
{
    soc_dpp_drc_combo28_info_t *drc_info;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(pairs_were_swapped);

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
    *pairs_were_swapped = ((drc_info->dram_dq_swap_bytes[drc_ndx][0] == 2) || (drc_info->dram_dq_swap_bytes[drc_ndx][0] == 3));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_enable_wr_crc(int unit, int drc_ndx, int enable)
{
    int 
    rv = SOC_E_NONE;
    
    SOCDNX_INIT_FUNC_DEFS;
 
    rv = soc_dpp_drc_combo28_enable_wr_crc(unit, drc_ndx, enable);
    SOCDNX_IF_ERR_EXIT(rv);
        
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_enable_rd_crc(int unit, int drc_ndx, int enable)
{
    int 
        rv = SOC_E_NONE,
        mrs_opcode=0,
        mrs_mask=0,
        mrs_num;
    uint32 dram_type;
    uint64 
        reg64_val,
        field64_val;
   
    static soc_reg_t 
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr, 
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr, 
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},     
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};     
     
    SOCDNX_INIT_FUNC_DEFS;
 
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    COMPILER_64_SET(field64_val, 0x0, ((enable == 0x0) ? FALSE : TRUE));
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, READ_CRCf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);

    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("ddr4_gddr5_enable_rd_crc Read CRC not supported in DDR4")));    
    } else {
    
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0) ? 0x1 : 0x0), 9);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,9);       
        mrs_num = 4;
        
        rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
        SOCDNX_IF_ERR_EXIT(rv);        
    }
        
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_enable_wr_dbi(int unit, int drc_ndx, int enable)
{
    int 
        rv = SOC_E_NONE,
        mrs_opcode=0,
        mrs_mask=0,
        mrs_num;
    uint32 dram_type;
    uint64 
        reg64_val,
        field64_val;
   
    static soc_reg_t 
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr, 
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr, 
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},     
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};     
     
    SOCDNX_INIT_FUNC_DEFS;
 
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    COMPILER_64_SET(field64_val, 0x0, ((enable == 0x0)? FALSE : TRUE));
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, WRITE_DBIf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);

    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0)? FALSE : TRUE), 11);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,11);        
        mrs_num = 5;
    } else {
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0)? TRUE : FALSE), 9);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 9);       
        mrs_num = 1;
    }

    rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
    SOCDNX_IF_ERR_EXIT(rv);
        
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_enable_rd_dbi(int unit, int drc_ndx, int enable)
{
    int 
        rv = SOC_E_NONE,
        mrs_opcode=0,
        mrs_mask=0,
        mrs_num;
    uint32 dram_type;
    uint64 
        reg64_val,
        field64_val;
   
    static soc_reg_t 
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr, 
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr, 
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},     
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                          DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};     
     
    SOCDNX_INIT_FUNC_DEFS;
 
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    
    COMPILER_64_SET(field64_val, 0x0, ((enable == 0x0)? FALSE : TRUE));
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, READ_DBIf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);

    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0)? FALSE : TRUE), 12);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,12);        
         mrs_num = 5;
    } else {
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0)? TRUE : FALSE),8);
        SOC_SAND_SET_BIT(mrs_mask, 0x1, 8);       
        mrs_num = 1;
    }

    rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
    SOCDNX_IF_ERR_EXIT(rv);
        
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_enable_refresh(int unit, int drc_ndx , int enable, uint32 new_trefi, uint32 * curr_refi)
{
    uint32 reg_val;
    static soc_reg_t
        DRC_AC_OPERATING_CONDITIONS_4l[] = {DRCA_AC_OPERATING_CONDITIONS_4r, DRCB_AC_OPERATING_CONDITIONS_4r, DRCC_AC_OPERATING_CONDITIONS_4r,
                                            DRCD_AC_OPERATING_CONDITIONS_4r, DRCE_AC_OPERATING_CONDITIONS_4r, DRCF_AC_OPERATING_CONDITIONS_4r,
                                            DRCG_AC_OPERATING_CONDITIONS_4r, DRCH_AC_OPERATING_CONDITIONS_4r};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "drc_ndx=%d, enable=%d, new_refi=%d\n"), drc_ndx, enable, new_trefi));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    *curr_refi = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg_val, DD_RT_REFIf);
      
    if (1 == enable) {
        soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg_val, DD_RT_REFIf, new_trefi);     
    } else {
        soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg_val, DD_RT_REFIf, 0); 
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, reg_val));  

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_force_dqs(int unit, int drc_ndx , uint32 force_dqs_val, uint32 force_dqs_oeb)
{
    uint64 reg64_val;
    static soc_reg_t DRC_AC_OPERATING_CONDITIONS_4l[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                            DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                            DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, &reg64_val));

    
    
    force_dqs_val = force_dqs_val ? 0xf : 0;
    force_dqs_oeb = force_dqs_oeb ? 0xf : 0;

    
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, FORCE_DQS_VALf, force_dqs_val);
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DQS_OEB_VALf, (~force_dqs_oeb)&DRC_COMBO28_CB_MASK_4_LSB);
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, OVERRIDE_DQS_OEBf, force_dqs_oeb);
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_soft_reset_drc_without_dram(int unit, int drc_ndx)
{

    static soc_reg_t DRC_INITIALIZATION_CONTROLLl[] = { DRCA_INITIALIZATION_CONTROLLr, DRCB_INITIALIZATION_CONTROLLr, DRCC_INITIALIZATION_CONTROLLr, DRCD_INITIALIZATION_CONTROLLr,
                                                        DRCE_INITIALIZATION_CONTROLLr, DRCF_INITIALIZATION_CONTROLLr, DRCG_INITIALIZATION_CONTROLLr, DRCH_INITIALIZATION_CONTROLLr };
    static soc_reg_t DRC_TRAIN_INIT_TRIGERSl[] = {  DRCA_TRAIN_INIT_TRIGERSr, DRCB_TRAIN_INIT_TRIGERSr, DRCC_TRAIN_INIT_TRIGERSr, DRCD_TRAIN_INIT_TRIGERSr,
                                                    DRCE_TRAIN_INIT_TRIGERSr, DRCF_TRAIN_INIT_TRIGERSr, DRCG_TRAIN_INIT_TRIGERSr, DRCH_TRAIN_INIT_TRIGERSr   };
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, AUTO_DRAM_INITf, 0));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, DDR_RSTNf, 0));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, DDR_RSTNf, 1));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_TRAIN_INIT_TRIGERSl[drc_ndx], REG_PORT_ANY, FORCE_DRAM_INIT_DONEf, 1));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo16_shmoo_dram_info_access(int unit, combo16_shmoo_dram_info_t** shmoo_info)
{
    combo28_shmoo_dram_info_t** shmoo_info_internal;
    shmoo_info_internal = (combo28_shmoo_dram_info_t**) shmoo_info;
    return soc_dpp_drc_combo28_shmoo_dram_info_access(unit, shmoo_info_internal);
}

int soc_dpp_drc_combo28_shmoo_dram_info_access(int unit, combo28_shmoo_dram_info_t** shmoo_info)
{
    SOCDNX_INIT_FUNC_DEFS;    
 
    SOCDNX_NULL_CHECK(shmoo_info);
    *shmoo_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info.shmoo_info;   
           
exit:
    SOCDNX_FUNC_RETURN;        
}

int soc_dpp_drc_combo16_shmoo_vendor_info_access(int unit, combo16_vendor_info_t** vendor_info)
{
    combo28_vendor_info_t** vendor_info_internal;
    vendor_info_internal = (combo28_vendor_info_t**)vendor_info;
    return soc_dpp_drc_combo28_shmoo_vendor_info_access(unit, vendor_info_internal);
}

int soc_dpp_drc_combo28_shmoo_vendor_info_access(int unit, combo28_vendor_info_t** vendor_info)
{
    SOCDNX_INIT_FUNC_DEFS;    
 
    SOCDNX_NULL_CHECK(vendor_info);
    *vendor_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info.vendor_info;   
           
exit:
    SOCDNX_FUNC_RETURN;        
}


int soc_dpp_drc_combo28_shmoo_cas_latency_get(int unit, int* cas_latency)
{
    SOCDNX_INIT_FUNC_DEFS;    
 
    SOCDNX_NULL_CHECK(cas_latency);
    *cas_latency = SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_param.c_cas_latency;   
           
exit:
    SOCDNX_FUNC_RETURN;        
}


