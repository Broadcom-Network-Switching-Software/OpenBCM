

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/dram/dram.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_func.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_dbal_access.h>
#include <sal/core/thread.h>


int
dnx_gddr6_get_sequential_channel(
    int unit,
    int dram_index,
    int channel_in_dram)
{
    return dram_index * dnx_data_dram.general_info.nof_channels_get(unit) + channel_in_dram;
}


shr_error_e
dnx_gddr6_dbal_access_phy_channel_register_set(
    int unit,
    int dram_index,
    uint32 address,
    uint32 data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PHY_REGISTER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_REG_ADDRESS, address);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PHY_REGISTER_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_phy_channel_register_get(
    int unit,
    int dram_index,
    uint32 address,
    uint32 *data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PHY_REGISTER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_REG_ADDRESS, address);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_PHY_REGISTER_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channel_soft_init_set(
    int unit,
    int dram_index,
    int channel,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CHANNEL_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channels_soft_init_set(
    int unit,
    int dram_index,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CHANNEL_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channels_soft_reset_set(
    int unit,
    int dram_index,
    uint32 in_soft_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CHANNEL_SOFT_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_RESET, INST_SINGLE, ! !in_soft_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_tsm_banks_disable_set(
    int unit,
    int dram_index,
    uint32 disable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TSM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_BANK_DISABLE, INST_SINGLE, disable ? 0xffff : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_enable_refresh_set(
    int unit,
    int dram_index,
    int channel,
    uint32 enable_refresh,
    uint32 enable_refresh_ab)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TSM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_REFRESH_ENABLE, INST_SINGLE, enable_refresh);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_AB_REFRESH_ENABLE, INST_SINGLE,
                                 enable_refresh_ab);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_enable_refresh_get(
    int unit,
    int dram_index,
    int channel,
    uint32 *enable_refresh,
    uint32 *enable_refresh_ab)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TSM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TSM_REFRESH_ENABLE, INST_SINGLE, enable_refresh);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TSM_AB_REFRESH_ENABLE, INST_SINGLE, enable_refresh_ab);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_reset(
    int unit,
    int dram_index,
    uint32 pll_in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_ISO_IN, INST_SINGLE, ! !pll_in_reset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_RESETB, INST_SINGLE, !pll_in_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_post_reset_set(
    int unit,
    int dram_index,
    uint32 pll_post_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_POST_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_POST_RESETB, INST_SINGLE,
                                 ! !pll_post_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_div_config_set(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pll_info, _SHR_E_PARAM, "pll_info");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_DIV_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PDIV, INST_SINGLE, pll_info->pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_NDIV_INT, INST_SINGLE, pll_info->ndiv_int);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_NDIV_FRAC, INST_SINGLE,
                                 pll_info->ndiv_frac);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_MDIV, INST_SINGLE, pll_info->mdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_FREFEFF_INFO, INST_SINGLE,
                                 pll_info->fref_eff_info);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_init(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info)
{
    uint32 entry_handle_id;
    uint32 LoopParamCtr[2];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pll_info, _SHR_E_PARAM, "pll_info");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_WDT_ENB, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_WDT_CNT, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_VCO_SEL, INST_SINGLE, pll_info->vco_sel);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_STAYS_LOCKED, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_STAT_UPDATE, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_STAT_SELECT, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_STAT_RESET, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_STAT_MODE, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_STEP, INST_SINGLE, pll_info->ssc_step);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_MODE, INST_SINGLE, pll_info->ssc_mode);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_LIMIT, INST_SINGLE,
                                 pll_info->ssc_limit);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_DOWNSPREAD, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SPARE_DIG, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_RATE_MNGR_MODE, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PWR_ON_LDO, INST_SINGLE, 1);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PWR_ON, INST_SINGLE, 1);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PQ_MODE, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_POST_CTRL, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PLL_CTRL, INST_SINGLE, pll_info->pll_ctrl);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_NOREF_CHK_ENB, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_MASH_11_MODE, INST_SINGLE, 0);
    
    
    LoopParamCtr[0] = LoopParamCtr[1] = 0;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_LOOP_PARAM_CTR, INST_SINGLE,
                                     LoopParamCtr);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_LOCK_LOST_CLR, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_LOCK_CNT, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_LDO_CTRL, INST_SINGLE, pll_info->ldo_ctrl);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_HOLD_DELAY, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_EN_CTRL, INST_SINGLE, pll_info->en_ctrl);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_DITH_ORDER, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_DITH_EN, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_DC_COMP_OPT, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_CP_CTRL, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_CODE_VCOCAL, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_BYPASS_MODE, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_BYPASS_DAC, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_BG_CTRL, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_AUTO_CNFG_DSBL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_config_set(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pll_info, _SHR_E_PARAM, "pll_info");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_ISO_IN, INST_SINGLE, pll_info->iso_in);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_LDO_CTRL, INST_SINGLE, pll_info->ldo_ctrl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_NDIV_FRAC, INST_SINGLE,
                                 pll_info->ndiv_frac);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PDIV, INST_SINGLE, pll_info->pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_NDIV_INT, INST_SINGLE, pll_info->ndiv_int);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_LIMIT, INST_SINGLE,
                                 pll_info->ssc_limit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_MODE, INST_SINGLE, pll_info->ssc_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_SSC_STEP, INST_SINGLE, pll_info->ssc_step);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_PLL_CTRL, INST_SINGLE, pll_info->pll_ctrl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_VCO_SEL, INST_SINGLE, pll_info->vco_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_MDIV, INST_SINGLE, pll_info->mdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_FREFEFF_INFO, INST_SINGLE,
                                 pll_info->fref_eff_info);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_EN_CTRL, INST_SINGLE, pll_info->en_ctrl);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_status_get(
    int unit,
    int dram_index,
    uint32 *pll_locked,
    uint32 *pll_stat_out)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_LOCK, INST_SINGLE, pll_locked);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_STAT_OUT, INST_SINGLE, pll_stat_out);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_phy_iddq_set(
    int unit,
    int dram_index,
    uint32 iddq)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PHY_IDDQ, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PHY_IDDQ, INST_SINGLE, iddq);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_refclk_sel(
    int unit,
    int dram_index,
    uint32 pll_clk_sel)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_REFCLK_SEL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PLL_REFCLK_SEL, INST_SINGLE, pll_clk_sel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_phy_out_of_reset_config(
    int unit,
    int dram_index,
    int out_of_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PHY_CHANNEL_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PHY_CH_RESET, INST_SINGLE, ! !out_of_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_phy_write_fifo_enable_config(
    int unit,
    int dram_index,
    uint32 write_fifo_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PHY_CHANNEL_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PHY_CH_WRITE_FIFO_ENABLE, INST_SINGLE,
                                 ! !write_fifo_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_phy_ilm_mode_config(
    int unit,
    int dram_index,
    uint32 ilm_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PHY_CHANNEL_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_PHY_ILM_MODE, INST_SINGLE, ilm_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_pll_status_locked_poll(
    int unit,
    int dram_index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_PLL_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 20000, 1000000, entry_handle_id, DBAL_FIELD_DRAM_PLL_LOCK, 1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_dynamic_memory_access_set(
    int unit,
    int dram_index,
    int enable)
{
    int channels_in_dram;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DPC_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DYNAMIC_MEMORY_ACCESS, INST_SINGLE,
                                 ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    channels_in_dram = dnx_data_dram.general_info.nof_channels_get(unit);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, 0, channels_in_dram - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DYNAMIC_MEMORY_ACCESS, INST_SINGLE,
                                 ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_dram_reset_config(
    int unit,
    int dram_index,
    uint32 out_of_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_RESET_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_RESET_N, INST_SINGLE, ! !out_of_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dram_controller_config(
    int unit,
    uint32 dram_index,
    int ignore_vendor)
{
    uint32 entry_handle_id;
    uint32 field_val;
    uint32 read_edc_vendor_specific_variation = 0;
    uint32 write_latency_vendor_specific_variation = 0;
    const dnx_data_dram_general_info_timing_params_t *timing_params;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!ignore_vendor)
    {
        dnx_dram_vendor_info_t dram_vendor_info;
        
        SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, dram_index, &dram_vendor_info));
        if (dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
        {
            write_latency_vendor_specific_variation = 1;
            read_edc_vendor_specific_variation = 1;
        }
    }

    timing_params = dnx_data_dram.general_info.timing_params_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_WRITE_LATENCY, INST_SINGLE,
                                 (dnx_data_dram.general_info.write_latency_get(unit) +
                                  write_latency_vendor_specific_variation));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_DBIDC_READ, INST_SINGLE,
                                 ! !dnx_data_dram.general_info.dbi_read_get(unit));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_DBIDC_WRITE, INST_SINGLE,
                                 ! !dnx_data_dram.general_info.dbi_write_get(unit));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_CABIDC, INST_SINGLE, 1);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_WR_CRC, INST_SINGLE,
                                 ! !dnx_data_dram.general_info.crc_write_get(unit));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_RD_CRC, INST_SINGLE,
                                 ! !dnx_data_dram.general_info.crc_read_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_EDC_HALF_RATE, INST_SINGLE, 0);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BANK_GROUPS_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_PIPELINES_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMAND_PIPE_EXTRA_DELAY, INST_SINGLE,
                                 dnx_data_dram.gddr6.command_pipe_extra_delay_get(unit));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTPUT_ENABLE_DELAY, INST_SINGLE,
                                 ((dnx_data_dram.general_info.write_latency_get(unit) +
                                   write_latency_vendor_specific_variation) -
                                  dnx_data_dram.gddr6.command_pipe_extra_delay_get(unit)));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTPUT_ENABLE_LENGTH, INST_SINGLE, 3);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RD_CMD_EXTRA_DELAY, INST_SINGLE, 2);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_ROW_ACCESS_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TWR, INST_SINGLE,
                                 (dnx_data_dram.gddr6.write_recovery_get(unit)
                                  + (dnx_data_dram.general_info.write_latency_get(unit) +
                                     write_latency_vendor_specific_variation) + BURST_LENGTH_DIV_4));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_WR_TO_RDAP, INST_SINGLE,
                                 (dnx_data_dram.gddr6.write_recovery_get(unit)
                                  + (dnx_data_dram.general_info.write_latency_get(unit) +
                                     write_latency_vendor_specific_variation) + BURST_LENGTH_DIV_4));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRRDS, INST_SINGLE, timing_params->trrds);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRCDWR, INST_SINGLE, timing_params->trcdwr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRCDRD, INST_SINGLE, timing_params->trcdrd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TFAW, INST_SINGLE, timing_params->tfaw);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_COLUMN_ACCESS_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TWTRL, INST_SINGLE,
                                 timing_params->twtrl + BURST_LENGTH_DIV_4 +
                                 (dnx_data_dram.general_info.write_latency_get(unit) +
                                  write_latency_vendor_specific_variation));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TWTRS, INST_SINGLE,
                                 timing_params->twtrs + BURST_LENGTH_DIV_4 +
                                 (dnx_data_dram.general_info.write_latency_get(unit) +
                                  write_latency_vendor_specific_variation));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRTW, INST_SINGLE,
                                 timing_params->trtw + BURST_LENGTH_DIV_4 -
                                 (dnx_data_dram.general_info.write_latency_get(unit) +
                                  write_latency_vendor_specific_variation) +
                                 dnx_data_dram.general_info.read_latency_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TCCDL, INST_SINGLE, timing_params->tccdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TCCDS, INST_SINGLE, timing_params->tccds);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_REFRESH_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRFC, INST_SINGLE, timing_params->trfc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRFCSB, INST_SINGLE, timing_params->trfcsb);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRREFD, INST_SINGLE, timing_params->trrefd);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_ROW_ACCESS_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRTPL, INST_SINGLE, timing_params->trtpl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRP, INST_SINGLE, timing_params->trp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRC, INST_SINGLE, timing_params->trc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRTPS, INST_SINGLE, timing_params->trtpl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRAS, INST_SINGLE, timing_params->tras);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTER_MANAGER_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UPDATE_DRAM_ON_MODE_REGISTER_CHANGE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_force_update_per_mode_register_bitmap_set(unit, dram_index, 0xffff));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_mask_update_per_mode_register_bitmap_set(unit, dram_index, 0x0000));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_MRS_RESET, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MRS_RST_N, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    field_val =
        dnx_data_dram.general_info.crc_write_latency_get(unit) +
        (dnx_data_dram.general_info.write_latency_get(unit) + write_latency_vendor_specific_variation) - 5;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_EDC_WRITE_LATENCY, INST_SINGLE, field_val);

    field_val =
        dnx_data_dram.general_info.crc_read_latency_get(unit) + read_edc_vendor_specific_variation +
        dnx_data_dram.general_info.read_latency_get(unit) - 5;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_EDC_READ_LATENCY, INST_SINGLE, field_val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_GENERAL_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TMOD, INST_SINGLE, timing_params->tmod);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_TRAINING_GENERAL_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRCDLTR, INST_SINGLE, timing_params->trcdltr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRCDRTR, INST_SINGLE, timing_params->trcdrtr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TREFTR, INST_SINGLE, timing_params->treftr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TLTLTR, INST_SINGLE, timing_params->tltltr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TLTRTR, INST_SINGLE, timing_params->tltrtr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRDTLT, INST_SINGLE, timing_params->trdtlt);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRCDWTR, INST_SINGLE, timing_params->trcdwtr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TWTRTR, INST_SINGLE, timing_params->twtrtr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TWRWTR, INST_SINGLE, timing_params->twrwtr);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_DATA_SOURCE_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_READ_INFLIGHTS_DS_N, INST_SINGLE, 256);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_WRITE_INFLIGHTS_DS_N, INST_SINGLE, 256);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_ROW_ACCESS_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TRRDL, INST_SINGLE, timing_params->trrdl);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_BANK_REORDER, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_BANK_REORDER_BIT_0, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_BANK_REORDER_BIT_1, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_BANK_REORDER_BIT_2, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_BANK_REORDER_BIT_3, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_TSM_READ_WRITE_SWITCH_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWITCH_BANK_ALMOST_FULL_THRESHOLD, INST_SINGLE, 12);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWITCH_TO_WRITE_MIN_ALMOST_FULL_BANKS_WRITE,
                                 INST_SINGLE, 5);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_TRAINING_CADT_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_BYTE_MAP, INST_SINGLE, dram_index % 2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

     
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_TSM_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
     
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_REFRESH_ENABLE, INST_SINGLE, 0);
     
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_AB_REFRESH_ENABLE, INST_SINGLE, 0);
     
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_EMERGENCY_REFRESH_ENABLE, INST_SINGLE, 0);
    if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_interleaved_refresh_cycles))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_NO_AB_REFRESH_DURING_PB_REFRESH, INST_SINGLE,
                                     0);
    }
    else
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_NO_AB_REFRESH_DURING_PB_REFRESH, INST_SINGLE,
                                     1);
    }
    if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_refresh_pend_switch))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_REFRESH_PEND_SWITCH, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

     
     
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONTROL, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_REFRESH_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

     
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_REFRESH_INTERVALS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    field_val =
        UTILEX_MIN(dnx_data_dram.gddr6.refresh_intervals_get(unit)->trefiab - 100000,
                   dnx_data_dram.gddr6.dynamic_calibration_period_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TREFIAB, INST_SINGLE, field_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_TREFISB, INST_SINGLE,
                                 dnx_data_dram.gddr6.refresh_intervals_get(unit)->trefisb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

   
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_VENDOR_ID_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VENDOR_ID_TO_RDEN_PRD, INST_SINGLE, 40);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_controller_coherency_handling_mode))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_CONTROLLER_CONFIG, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COHERENCY_HANDLE_MODE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_cke_set(
    int unit,
    int dram_index,
    uint32 cke_ui)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CKE_N, INST_SINGLE, !cke_ui);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_mr_set(
    int unit,
    uint32 dram_index,
    uint32 mr_index,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_MR, mr_index, value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channel_mr_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 mr_index,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_MR, mr_index, value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_channel_mr_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 mr_index,
    uint32 *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_MR, mr_index, value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_cpu_command_set(
    int unit,
    int dram_index,
    uint32 ca,
    uint32 cabi_n)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CA, INST_SINGLE, ca);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CABI_N, INST_SINGLE, cabi_n);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRIGGER_CPU_COMMAND, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_force_constant_cpu_value_set(
    int unit,
    int dram_index,
    uint32 ca,
    uint32 cabi_n)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CA, INST_SINGLE, ca);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CABI_N, INST_SINGLE, cabi_n);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_CONSTANT_CPU_VALUES, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_release_constant_cpu_set(
    int unit,
    int dram_index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_CONSTANT_CPU_VALUES, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_wck_set(
    int unit,
    int dram_index,
    uint32 wck)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WCK, INST_SINGLE, wck);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_update_dram_on_mode_register_change(
    int unit,
    int dram_index,
    int update)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTER_MANAGER_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UPDATE_DRAM_ON_MODE_REGISTER_CHANGE, INST_SINGLE,
                                 ! !update);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_force_update_per_mode_register_bitmap_set(
    int unit,
    int dram_index,
    uint32 force)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTER_MANAGER_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_UPDATE_PER_MODE_REGISTER_BITMAP, INST_SINGLE,
                                 force);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_mask_update_per_mode_register_bitmap_set(
    int unit,
    int dram_index,
    uint32 mask)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTER_MANAGER_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_UPDATE_PER_MODE_REGISTER_BITMAP, INST_SINGLE,
                                 mask);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_gddr6_dbal_access_dram_block_write_minus_read_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 decrement_thr_factor = dnx_data_dram.dram_block.wmr_decrement_thr_factor_get(unit);
    uint32 reset_on_deassert = dnx_data_dram.dram_block.wmr_reset_on_deassert_get(unit);
    uint32 full_size = dnx_data_dram.dram_block.wmr_full_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_GDDR6_TDU_DRAM_BLOCK_WRITE_MINUS_READ_LEAKY_BUCKET_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 2000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 4000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 4);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_0, INST_SINGLE,
                                 0 * decrement_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_1, INST_SINGLE,
                                 5 * decrement_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_2, INST_SINGLE,
                                 10 * decrement_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_2, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE,
                                 reset_on_deassert);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE, full_size);
    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_EN, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_gddr6_dbal_access_dram_block_average_read_inflights_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_GDDR6_TDU_DRAM_BLOCK_AVERAGE_READ_INFLIGHTS_LEAKY_BUCKET_CONFIG,
                     &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE,
                                 450);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 500);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 550);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 8);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_0, INST_SINGLE,
                                 450);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_1, INST_SINGLE,
                                 400);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_2, INST_SINGLE,
                                 350);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_1, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_2, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE, 40);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE, 24);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_WINDOW_SIZE_LOG_2, INST_SINGLE, 12);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_gddr6_dbal_access_dram_block_write_plus_read_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 increment_thr_factor = dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_GDDR6_TDU_DRAM_BLOCK_WRITE_PLUS_READ_LEAKY_BUCKET_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE, 16);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE, 50);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 4);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE,
                                 91 * increment_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 93 * increment_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 95 * increment_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE, INST_SINGLE, 1);
    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_EN, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_tdu_configure(
    int unit)
{
    uint32 entry_handle_id;
    uint32 window_size = dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TDU_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_SOURCE_ID, INST_SINGLE, 0);
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0x2)
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_SOURCE_ID, 0, 1);
    }
    else
    {
        for (int data_source = 0; data_source < dnx_data_device.general.nof_cores_get(unit); ++data_source)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_SOURCE_ID, data_source, data_source);
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_PATH_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_RING_0_EXTRA_DELAY, INST_SINGLE, 12);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_RING_1_EXTRA_DELAY, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WRITE_REQUEST_PATH_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_REQUEST_PATH_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_WINDOW_SIZE, INST_SINGLE, window_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_block_write_minus_read_leaky_bucket_config_set(unit));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_block_write_plus_read_leaky_bucket_config_set(unit));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_block_average_read_inflights_leaky_bucket_config_set(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_tdu_atm_configure(
    int unit,
    const uint32 *atm)
{
    uint32 entry_handle_id;
    uint32 atm_column_size;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    atm_column_size = dnx_data_dram.address_translation.matrix_column_size_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TDU_ADDRESS_TRANSLATION_MATRIX, &entry_handle_id));
    for (int line = 0; line < atm_column_size; ++line)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDRESS_TRANSLATION_MATRIX, line, atm[line]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dram_cpu_access_get(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, module);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK, bank);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW, row);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN, column);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_CPU_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dram_cpu_access_set(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, module);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK, bank);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW, row);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN, column);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_CPU_ACCESS, INST_SINGLE, pattern);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_ca_bit_map_set(
    int unit,
    uint32 dram_index)
{
    uint32 entry_handle_id;
    int channel;
    int ca_bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CA_BIT_MAP, &entry_handle_id));
    for (channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
        for (ca_bit = 0; ca_bit <= dnx_data_dram.gddr6.nof_ca_bits_get(unit); ca_bit++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CA_BIT_MAP, ca_bit,
                                         dnx_data_dram.gddr6.ca_map_get(unit, dram_index, channel)->ca_map[ca_bit]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dq_bit_map_set(
    int unit,
    uint32 dram_index)
{
    uint32 entry_handle_id;
    int channel;
    int byte, bit_index;
    int channel_swapped;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    channel_swapped = dnx_data_dram.gddr6.dq_channel_swap_get(unit, dram_index)->dq_channel_swap;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_DQ_BIT_MAP, &entry_handle_id));
    for (channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
        if (channel_swapped)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL,
                                       (dnx_data_dram.general_info.nof_channels_get(unit) - 1 - channel));
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
        }
        for (bit_index = 0; bit_index < 16; bit_index++)
        {
            byte = channel * 2 + bit_index / 8;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DQ_BYTE_BIT_MAP, bit_index,
                                         dnx_data_dram.gddr6.dq_map_get(unit, dram_index, byte)->dq_map[bit_index % 8]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_cadt_byte_map_set(
    int unit,
    uint32 dram_index)
{
    uint32 entry_handle_id;
    int channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_TRAINING_CADT_BYTE_MAP, &entry_handle_id));
    for (channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_BYTE_MAP, INST_SINGLE,
                                     dnx_data_dram.gddr6.cadt_byte_map_get(unit, dram_index, channel)->cadt_byte_map);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_wr_crc_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_WR_CRC, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_rd_crc_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_RD_CRC, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_wr_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_DBIDC_WRITE, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_rd_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_DBIDC_READ, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channel_rd_dbi_enable_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_DBIDC_READ, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channel_rd_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_DBIDC_READ, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dram_err_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_dram_err_cnt_t * dram_err_cnt)
{
    uint32 entry_handle_id;
    int i_byte;
    int bytes_per_channel = dnx_data_dram.gddr6.bytes_per_channel_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_ERR_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    for (i_byte = 0; i_byte < bytes_per_channel; ++i_byte)
    {
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_CRC_ERR_CNT, i_byte,
                                       dram_err_cnt->read_crc_err_cnt[i_byte]);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_WRITE_CRC_ERR_CNT, i_byte,
                                       dram_err_cnt->write_crc_err_cnt[i_byte]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_bist_err_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_ERR_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_CADT_ERR_CNT, INST_SINGLE,
                                   &bist_err_cnt->cadt_err_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_CADT_ERR_BITMAP, INST_SINGLE,
                                   bist_err_cnt->cadt_err_bitmap);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_DATA_ERR_CNT, INST_SINGLE,
                                   &bist_err_cnt->bist_data_err_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_DATA_ERR_BITMAP, INST_SINGLE,
                                   bist_err_cnt->bist_data_err_bitmap);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_EDC_ERR_CNT, INST_SINGLE,
                                   &bist_err_cnt->bist_edc_err_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_EDC_ERR_BITMAP, INST_SINGLE,
                                   bist_err_cnt->bist_edc_err_bitmap);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_DBI_ERR_CNT, INST_SINGLE,
                                   &bist_err_cnt->bist_dbi_err_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_DBI_ERR_BITMAP, INST_SINGLE,
                                   bist_err_cnt->bist_dbi_err_bitmap);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_bist_status_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_WRITE_COMMAND_CNT, INST_SINGLE,
                                   bist_status_cnt->write_command_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_READ_COMMAND_CNT, INST_SINGLE,
                                   bist_status_cnt->read_command_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_READ_DATA_CNT, INST_SINGLE,
                                   bist_status_cnt->read_data_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_READ_TRAINING_DATA_CNT, INST_SINGLE,
                                   bist_status_cnt->read_training_data_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_READ_EDC_CNT, INST_SINGLE,
                                   bist_status_cnt->read_edc_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_WRTR_COMMAND_CNT, INST_SINGLE,
                                   bist_status_cnt->wrtr_command_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RDTR_COMMAND_CNT, INST_SINGLE,
                                   bist_status_cnt->rdtr_command_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_LDFF_COMMAND_CNT, INST_SINGLE,
                                   bist_status_cnt->ldff_command_cnt);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_rd_fifo_rstn_set(
    int unit,
    uint32 dram_index,
    uint32 rstn)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_READ_FIFO_RSTN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_FIFO_RESET, INST_SINGLE, ! !rstn);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_channel_rd_fifo_rstn_set(
    int unit,
    uint32 dram_index,
    int channel,
    uint32 rstn)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_READ_FIFO_RSTN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_FIFO_RESET, INST_SINGLE, ! !rstn);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_pipelines_interrupts_clear(
    int unit,
    uint32 dram_index,
    uint32 channel)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_PIPELINES_INTERRUPTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EDC_RD_IS_TRAINING_FIFO_UNDERFLOW, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EDC_RD_IS_TRAINING_FIFO_OVERFLOW, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_pipelines_interrupts_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *is_underflow,
    uint32 *is_overflow)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_PIPELINES_INTERRUPTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EDC_RD_IS_TRAINING_FIFO_UNDERFLOW, INST_SINGLE,
                               is_underflow);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EDC_RD_IS_TRAINING_FIFO_OVERFLOW, INST_SINGLE,
                               is_overflow);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_bist_run(
    int unit,
    int dram_index,
    int channel,
    uint32 start)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_GO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_GO, INST_SINGLE, ! !start);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_bist_run_done_poll(
    int unit,
    int dram_index,
    int channel)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_GO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_BIST_GO, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_training_bist_run(
    int unit,
    int dram_index,
    uint32 start)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_GO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_BIST_GO, INST_SINGLE, ! !start);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_training_bist_run_done_poll(
    int unit,
    int dram_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_GO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
        SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_TRAINING_BIST_GO, 0));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_ldff_training_pattern_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_ldff_pattern * ldff_pattern)
{
    uint32 entry_handle_id;
    int pattern_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_LDFF_TRAINING_PATTERN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    for (pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS; ++pattern_index)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_LDFF_DQ_PATTERN_N, pattern_index,
                                         &ldff_pattern->dq_pattern[pattern_index][0]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_LDFF_DBI_PATTERN_N, pattern_index,
                                     ldff_pattern->dbi_pattern[pattern_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_LDFF_EDC_PATTERN_N, pattern_index,
                                     ldff_pattern->edc_pattern[pattern_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_cadt_training_pattern_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_cadt_bist_configuration * cadt_bist)
{
    uint32 entry_handle_id;
    int pattern_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CADT_TRAINING_PATTERN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    for (pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS; ++pattern_index)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_PATTERN_N, pattern_index,
                                     cadt_bist->cadt_pattern[pattern_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_cmd_training_bist_configure_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_bist_cmd_configuration * training_bist_conf)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TRAINING_BIST_COMMAND_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_FIFO_DEPTH, INST_SINGLE,
                                 training_bist_conf->fifo_depth);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_BIST_MODE, INST_SINGLE,
                                 training_bist_conf->training_bist_mode);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_NUM_OF_COMMANDS, INST_SINGLE,
                                 training_bist_conf->commands);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CONSECUTIVE_WRITE_COMMANDS, INST_SINGLE,
                                 training_bist_conf->write_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CONSECUTIVE_READ_COMMANDS, INST_SINGLE,
                                 training_bist_conf->read_weight);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_periodic_temp_readout_config(
    int unit,
    uint32 dram_index,
    dnx_gddr6_periodic_temp_readout_config_t * temp_readout_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_PERIODIC_TEMPERATURE_READOUT_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READOUT_TO_READOUT_PRD, INST_SINGLE,
                                 temp_readout_config->readout_to_readout_prd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REFRESH_TO_READOUT_PRD, INST_SINGLE,
                                 temp_readout_config->refresh_to_readout_prd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READOUT_DONE_TO_DONE_PRD, INST_SINGLE,
                                 temp_readout_config->readout_done_to_done_prd);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_periodic_temp_readout_enable(
    int unit,
    uint32 dram_index,
    uint32 channel,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_PERIODIC_TEMPERATURE_READOUT_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TEMPERATURE_READOUT_ENABLE, INST_SINGLE, ! !enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_periodic_temp_readout_status_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_periodic_temp_readout_status_t * temp_readout_status)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_PERIODIC_TEMPERATURE_READOUT_STATUS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TEMPERATURE, INST_SINGLE,
                               &temp_readout_status->temperature);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_VALID, INST_SINGLE, &temp_readout_status->is_valid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_TEMPERATURE, INST_SINGLE,
                               &temp_readout_status->max_temperature);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dynamic_calibration_config(
    int unit,
    uint32 dram_index,
    uint32 channel,
    int set_init_position)
{
    uint32 entry_handle_id;
    uint64 training_num_of_commands;
    g6phy16_step_size_t step_size;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(soc_g6phy16_calculate_step_size(unit, dram_index, &step_size));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TRAINING_BIST_COMMAND_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_FIFO_DEPTH, INST_SINGLE,
                                 dnx_data_dram.gddr6.training_fifo_depth_get(unit));
    COMPILER_64_SET(training_num_of_commands, 0, 12);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_NUM_OF_COMMANDS, INST_SINGLE,
                                 training_num_of_commands);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CONSECUTIVE_WRITE_COMMANDS, INST_SINGLE, 6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CONSECUTIVE_READ_COMMANDS, INST_SINGLE, 6);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_TRAINING_BIST_COMMAND_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_BIST_MODE, INST_SINGLE, WRTR);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_MODE, INST_SINGLE,
                                 DNX_GDDR6_BIST_DATA_MODE_PRBS);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DYNAMIC_CALIBRATION_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHADOW_VDL_TO_TRAINING_PRD, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CDR_WRAPAROUND_UPDATE_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CDR_WRAPAROUND_UPDATE_LENGTH, INST_SINGLE, 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_VDL_UPDATE_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_TO_CALIB_PRD, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_VDL_CENTER_DRIFT_TRESHOLD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_VDL_JUMP, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_UI_SHIFT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_UI_SIZE, INST_SINGLE,
                                 UTILEX_DIV_ROUND(step_size.size1000UI, 1000));

    
    for (int byte = 0; byte < dnx_data_dram.gddr6.bytes_per_channel_get(unit); ++byte)
    {
        uint32 vdl_address = 0xb + 0x100 * byte + 0x200 * channel;
        uint32 static_vdl_address = 0xa + 0x100 * byte + 0x200 * channel;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_ADDRESS_N, byte, vdl_address);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STATIC_VDL_ADDRESS_N, byte, static_vdl_address);

        if (set_init_position)
        {
            int normalized_starting_vdl_position = 0;
            uint32 ui_shift = 0;
            uint32 vdl_position = 0;
            uint32 another_entry_handle_id;
            int vdl_right_border;
            int vdl_left_border;
            const int border_size = 50;

            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_PHY_WRITE_MAX_VDL_DATA, &another_entry_handle_id));
            dbal_entry_key_field32_set(unit, another_entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
            dbal_entry_key_field32_set(unit, another_entry_handle_id, DBAL_FIELD_CHANNEL, channel);
            dbal_entry_key_field32_set(unit, another_entry_handle_id, DBAL_FIELD_DBYTE, byte);
            dbal_value_field32_request(unit, another_entry_handle_id, DBAL_FIELD_MAX_VDL_STEP, INST_SINGLE,
                                       &vdl_position);
            dbal_value_field32_request(unit, another_entry_handle_id, DBAL_FIELD_UI_SHIFT, INST_SINGLE, &ui_shift);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, another_entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, another_entry_handle_id);
            normalized_starting_vdl_position = UTILEX_DIV_ROUND(step_size.size1000UI * ui_shift, 1000) + vdl_position;

            
            vdl_right_border = normalized_starting_vdl_position + border_size;
            vdl_left_border =
                (normalized_starting_vdl_position - border_size) <
                0 ? 0 : (normalized_starting_vdl_position - border_size);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_RIGHT_BORDER_DATA_N, byte,
                                         vdl_right_border);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_LEFT_BORDER_DATA_N, byte,
                                         vdl_left_border);

            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_RIGHT_INITIAL_DATA_N, byte,
                                         normalized_starting_vdl_position + 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VDL_LEFT_INITIAL_DATA_N, byte,
                                         normalized_starting_vdl_position - 1);

        }
    }

    if (set_init_position)
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_LOAD_INITIAL_BORDERS, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_LOAD_INITIAL_BORDERS, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALIB_LOAD_INITIAL_BORDERS, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dynamic_calibration_enable_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DYNAMIC_CALIBRATION_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (enable)
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_db.dynamic_calibration_was_enabled_after_dram_tune.set(unit, dram_index, 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_dynamic_calibration_enable_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DYNAMIC_CALIBRATION_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_training_bist_mode_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_bist_mode mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TRAINING_BIST_COMMAND_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_BIST_MODE, INST_SINGLE, mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_bist_configure_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_configuration_t * bist_conf)
{
    uint32 entry_handle_id;
    int pattern_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "configure bist: set cmd starts dram_index=%d\n"), dram_index));
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_NUM_OF_COMMANDS, INST_SINGLE,
                                 bist_conf->nof_commands);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONSECUTIVE_WRITE_COMMANDS, INST_SINGLE,
                                 bist_conf->write_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONSECUTIVE_READ_COMMANDS, INST_SINGLE,
                                 bist_conf->read_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_ROW_COMMANDS, INST_SINGLE, bist_conf->same_row);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "configure bist: set addr starts dram_index=%d\n"), dram_index));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_MODE, INST_SINGLE, bist_conf->bank_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_START, INST_SINGLE, bist_conf->bank_start);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_END, INST_SINGLE, bist_conf->bank_end);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_SEED, INST_SINGLE, bist_conf->bank_seed);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN_MODE, INST_SINGLE, bist_conf->column_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN_START, INST_SINGLE, bist_conf->column_start);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN_END, INST_SINGLE, bist_conf->column_end);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN_SEED, INST_SINGLE, bist_conf->column_seed);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW_MODE, INST_SINGLE, bist_conf->row_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW_START, INST_SINGLE, bist_conf->row_start);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW_END, INST_SINGLE, bist_conf->row_end);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW_SEED, INST_SINGLE, bist_conf->row_seed);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "configure bist: set data starts dram_index=%d\n"), dram_index));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_MODE, INST_SINGLE, bist_conf->data_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DBI_MODE, INST_SINGLE, bist_conf->dbi_mode);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRBS_DATA_SEED, INST_SINGLE,
                                     bist_conf->data_seed);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRBS_DBI_SEED, INST_SINGLE, bist_conf->dbi_seed);

    for (pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS; ++pattern_index)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_DBI_PATTERN_N, pattern_index,
                                     bist_conf->bist_dbi_pattern[pattern_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "configure bist: set pattern starts dram_index=%d\n"), dram_index));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG_PATTERN_1, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    for (pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS / 2; ++pattern_index)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BIST_DQ_PATTERN_N, pattern_index,
                                         bist_conf->bist_dq_pattern[pattern_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG_PATTERN_2, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    for (pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS / 2; ++pattern_index)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BIST_DQ_PATTERN_N, pattern_index,
                                         bist_conf->bist_dq_pattern[pattern_index + 4]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_training_cadt_mode_set(
    int unit,
    uint32 dram_index,
    uint32 cadt_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_TRAINING_CADT_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_MODE, INST_SINGLE, cadt_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_gddr6_dbal_access_mr_init_value_set(
    int unit,
    int dram_index,
    int mr_index,
    uint32 entry_handle_id)
{
    uint32 field_val = 0;
    uint32 field_in_struct_val;
    dnx_dram_vendor_info_t dram_vendor_info;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, dram_index, &dram_vendor_info));

    switch (mr_index)
    {
        case 0:
        {
            
            field_in_struct_val = dnx_data_dram.general_info.write_latency_get(unit) % 8;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_0, DBAL_FIELD_WRITE_LATENCY, &field_in_struct_val, &field_val));
            field_in_struct_val = (dnx_data_dram.general_info.read_latency_get(unit) - 5) % 0x10;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_0, DBAL_FIELD_READ_LATENCY, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_0, DBAL_FIELD_TM, &field_in_struct_val, &field_val));
            field_in_struct_val = (dnx_data_dram.gddr6.write_recovery_get(unit) - 4) % 0x10;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_0, DBAL_FIELD_WRITE_RECOVERY, &field_in_struct_val, &field_val));
            break;
        }

        case 1:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_DRIVER_STRENGTH, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 1;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_DATA_TERMINATION, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_PLL_RANGE, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_CAL_UPD, &field_in_struct_val, &field_val));
            field_in_struct_val = (dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
                && (dnx_data_dram.general_info.frequency_get(unit) > 875);
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_PLL_DLL, &field_in_struct_val, &field_val));
            field_in_struct_val = !dnx_data_dram.general_info.dbi_read_get(unit);
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_RDBI, &field_in_struct_val, &field_val));
            field_in_struct_val = !dnx_data_dram.general_info.dbi_write_get(unit);
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_WDBI, &field_in_struct_val, &field_val));
            field_in_struct_val = !dnx_data_dram.gddr6.cabi_get(unit);
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_CABI, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_1, DBAL_FIELD_PLL_RESET, &field_in_struct_val, &field_val));
            break;
        }

        case 2:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_OCD_PULLDOWN_DO, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_OCD_PULLUP_DO, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_SELF_REFRESH, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_EDC_MODE, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_RDQS, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_CADT_SRF, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_2, DBAL_FIELD_EDC_HR, &field_in_struct_val, &field_val));
            break;
        }

        case 3:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_3, DBAL_FIELD_DA_WCK_TERMINATION_OFF, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_3, DBAL_FIELD_CA_TERMINATION_OFF, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_3, DBAL_FIELD_DRAM_INFO, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_3, DBAL_FIELD_WR_SCALING, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_3, DBAL_FIELD_BANK_GROUP, &field_in_struct_val, &field_val));
            break;
        }

        case 4:
        {
            
            field_in_struct_val = 0xf;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_4, DBAL_FIELD_EDC_HOLD_PATTERN, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = (dnx_data_dram.general_info.crc_write_latency_get(unit) + 1) % 8;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_4, DBAL_FIELD_CRCWL, &field_in_struct_val, &field_val));
            field_in_struct_val = dnx_data_dram.general_info.crc_read_latency_get(unit) % 4;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_4, DBAL_FIELD_CRCRL, &field_in_struct_val, &field_val));
            field_in_struct_val = !dnx_data_dram.general_info.crc_read_get(unit);
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_4, DBAL_FIELD_RD_CRC, &field_in_struct_val, &field_val));
            field_in_struct_val = !dnx_data_dram.general_info.crc_write_get(unit);
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_4, DBAL_FIELD_WR_CRC, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_4, DBAL_FIELD_EDC_INV, &field_in_struct_val, &field_val));
            break;
        }

        case 5:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_5, DBAL_FIELD_LP1, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_5, DBAL_FIELD_LP2, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_5, DBAL_FIELD_LP3, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_5, DBAL_FIELD_PLL_BAND_WIDTH, &field_in_struct_val, &field_val));
            if (dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
            {
                field_in_struct_val = dnx_data_dram.general_info.timing_params_get(unit)->tras;
            }
            else
            {
                field_in_struct_val = 0;
            }
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_5, DBAL_FIELD_RAS, &field_in_struct_val, &field_val));
            break;
        }

        case 6:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_6, DBAL_FIELD_VREFD_LEVEL, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_6, DBAL_FIELD_PIN_SA, &field_in_struct_val, &field_val));
            break;
        }

        case 7:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_WCK_AP, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_HIBERNATE, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_PLL_DELC, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_LF_MODE, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_AUTO_SYNC, &field_in_struct_val, &field_val));
            
            field_in_struct_val =
                (dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_SAMSUNG) ? 1 : 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_DQ_PREA, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_HALF_VREFC, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_HALF_VREFD, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_VDD_RANGE, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_7, DBAL_FIELD_DCC, &field_in_struct_val, &field_val));
            break;
        }

        case 8:
        {
            
            field_in_struct_val = 1;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_CAL_TERM, &field_in_struct_val, &field_val));
            field_in_struct_val = 1;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_CAH_TERM, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_CA_TO, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_EDC_HIZ, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_CK_AC, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_REFPB, &field_in_struct_val, &field_val));
            field_in_struct_val = (dnx_data_dram.general_info.read_latency_get(unit) - 5) / 0x10;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_RLEHF, &field_in_struct_val, &field_val));
            field_in_struct_val = (dnx_data_dram.gddr6.write_recovery_get(unit) - 4) / 0x10;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_WREHF, &field_in_struct_val, &field_val));
            field_in_struct_val = 1;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_8, DBAL_FIELD_CK_TERM, &field_in_struct_val, &field_val));
            break;
        }

        case 9:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_9, DBAL_FIELD_DFE, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_9, DBAL_FIELD_RFU, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_9, DBAL_FIELD_PIN_SUB_ADDR, &field_in_struct_val, &field_val));
            break;
        }

        case 10:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_10, DBAL_FIELD_VREFC_OFF, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_10, DBAL_FIELD_WCK_INV_BYTE_0, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_10, DBAL_FIELD_WCK_INV_BYTE_1, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_10, DBAL_FIELD_WCK2CK, &field_in_struct_val, &field_val));
            if ((dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
                && (dnx_data_dram.general_info.frequency_get(unit) > 875))
            {
                field_in_struct_val = 1;
            }
            else
            {
                field_in_struct_val = 0;
            }
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_10, DBAL_FIELD_WCK_RATIO, &field_in_struct_val, &field_val));
            field_in_struct_val = 1;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_10, DBAL_FIELD_WCK_TERM, &field_in_struct_val, &field_val));
            break;
        }

        case 11:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_11, DBAL_FIELD_PASR_2_BANK_MASK, &field_in_struct_val,
                             &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_11, DBAL_FIELD_PASR_ROW_SEGMENG_MASK, &field_in_struct_val,
                             &field_val));
            break;
        }

        case 12:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_12, DBAL_FIELD_VDDQ_OFF, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_12, DBAL_FIELD_P2BR_ADDR, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_12, DBAL_FIELD_PRBS, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_12, DBAL_FIELD_RFU, &field_in_struct_val, &field_val));
            break;
        }

        case 13:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_13, DBAL_FIELD_RESERVED, &field_in_struct_val, &field_val));
            break;
        }

        case 14:
        {
            
            if (dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
            {
                field_in_struct_val = 0x802;
            }
            else
            {
                field_in_struct_val = 0;
            }
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_14, DBAL_FIELD_RESERVED, &field_in_struct_val, &field_val));
            break;
        }

        case 15:
        {
            
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_15, DBAL_FIELD_MREA, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_15, DBAL_FIELD_MREB, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_15, DBAL_FIELD_CADT, &field_in_struct_val, &field_val));
            field_in_struct_val = 0;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, DBAL_FIELD_GDDR6_MR_15, DBAL_FIELD_RESERVED, &field_in_struct_val, &field_val));
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid MR index %d\n", mr_index);
            break;
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_MR, mr_index, field_val);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_mr_init_set(
    int unit,
    int dram_index,
    int mr_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_mr_init_value_set(unit, dram_index, mr_index, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dbal_access_vendor_id_mode_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_info_mode_t mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_VENDOR_ID_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VENDOR_ID_MODE, INST_SINGLE, mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_dbal_access_channel_bist_last_returned_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_last_returned_data_t * last_returned_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_BIST_LAST_RETURNED_DATA, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_BIST_LAST_RETURNED_DQ,
                                   INST_SINGLE, last_returned_data->last_returned_dq);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIST_LAST_RETURNED_DBI,
                               INST_SINGLE, &last_returned_data->last_returned_dbi);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIST_LAST_RETURNED_EDC,
                               INST_SINGLE, &last_returned_data->last_returned_edc);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
