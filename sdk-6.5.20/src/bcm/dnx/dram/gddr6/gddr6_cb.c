

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM



#include <sal/appl/sal.h>
#include <sal/types.h>


#include <soc/debug.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/bitop.h>

#include <soc/shmoo_g6phy16.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/dbal/dbal.h>

#include <bcm_int/dnx/dram/gddr6/gddr6_cb.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_func.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_dbal_access.h>
#include <bcm_int/dnx/dram/dram.h>




shr_error_e
dnx_gddr6_modify_mrs_cb(
    int unit,
    int dram_index,
    int channel_index,
    uint32 mr_index,
    uint32 data,
    uint32 mask)
{
    uint32 mr_val = 0;
    uint32 allowed_data_mask;
    int channel;
    int channel_start;
    int channel_end;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, mr_index=%d, data=0x%x, mask=0x%x\n"),
                 FUNCTION_NAME(), unit, dram_index, mr_index, data, mask));
    
    allowed_data_mask = dnx_data_dram.general_info.mr_mask_get(unit);
    if ((data | mask) & ~(allowed_data_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bad param\\s data=0x%x mask=0x%x exceeding allowed bit mask 0x%x\n",
                     data, mask, allowed_data_mask);
    }

    channel_start = (channel_index == SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS) ? 0 : channel_index;
    channel_end =
        (channel_index ==
         SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS) ? dnx_data_dram.general_info.nof_channels_get(unit) : (channel_start + 1);
    for (channel = channel_start; channel < channel_end; channel++)
    {
        
        SHR_IF_ERR_EXIT(dnx_gddr6_mode_register_get(unit, dram_index, channel, mr_index, &mr_val));
        
        mr_val &= ~mask;
        
        mr_val |= (mask & data);
        
        SHR_IF_ERR_EXIT(dnx_gddr6_mode_register_set(unit, dram_index, channel, mr_index, mr_val));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_pll_set_cb(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pll_info, _SHR_E_PARAM, "pll_info");

    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_configure_plls(unit, dram_index, pll_info));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_phy_reg_read_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 *data)
{
    uint32 addr_corrected = 0;
    uint32 phy_reg_addr_mask;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, phy_reg_address=0x%x\n"),
                                 FUNCTION_NAME(), unit, dram_index, addr));
    
    phy_reg_addr_mask = dnx_data_dram.general_info.phy_address_mask_get(unit);
    addr_corrected = addr & phy_reg_addr_mask;
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_phy_channel_register_get(unit, dram_index, addr_corrected, data));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_phy_reg_write_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 data)
{
    uint32 addr_corrected = 0;
    uint32 phy_reg_addr_mask;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, phy_reg_address=0x%x, data=0x%x\n"),
                                 FUNCTION_NAME(), unit, dram_index, addr, data));
    
    phy_reg_addr_mask = dnx_data_dram.general_info.phy_address_mask_get(unit);
    addr_corrected = addr & phy_reg_addr_mask;
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_phy_channel_register_set(unit, dram_index, addr_corrected, data));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_phy_reg_modify_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 data,
    uint32 mask)
{
    uint32 old_data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, phy_reg_address=0x%x, data=0x%x, mask=0x%x\n"),
                 FUNCTION_NAME(), unit, dram_index, addr, data, mask));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_phy_reg_read_cb(unit, dram_index, addr, &old_data));

    
    old_data &= ~mask;
    old_data |= (mask & data);

    
    SHR_IF_ERR_EXIT(dnx_gddr6_phy_reg_write_cb(unit, dram_index, addr, old_data));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_enable_wr_crc_cb(
    int unit,
    int dram_index,
    int enable)
{
    uint32 mrs_opcode, mrs_mask, mrs_num;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d\n"),
                                 FUNCTION_NAME(), unit, dram_index, enable));
    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_wr_crc_enable_set(unit, dram_index, enable));

    
    if (enable == 0)
    {
        mrs_opcode = UTILEX_SET_BITS_RANGE(1, 10, 10);
    }
    else
    {
        mrs_opcode = 0;
    }
    mrs_mask = UTILEX_BITS_MASK(10, 10);
    mrs_num = 4;
    SHR_IF_ERR_EXIT(dnx_gddr6_modify_mrs_cb
                    (unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, mrs_num, mrs_opcode, mrs_mask));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_enable_rd_crc_cb(
    int unit,
    int dram_index,
    int enable)
{
    uint32 mrs_opcode, mrs_mask, mrs_num;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    if ((enable == 0) && !dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_allow_disable_read_crc))
    {
        
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "Operating on a system where crc disabled is not supported, value was override to enabled.\n")));
        enable = 1;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d\n"), FUNCTION_NAME(), unit, dram_index,
                 enable));
    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_rd_crc_enable_set(unit, dram_index, enable));

    
    if (enable == 0)
    {
        mrs_opcode = UTILEX_SET_BITS_RANGE(1, 9, 9);
    }
    else
    {
        mrs_opcode = 0;
    }

    mrs_mask = UTILEX_BITS_MASK(9, 9);
    mrs_num = 4;
    SHR_IF_ERR_EXIT(dnx_gddr6_modify_mrs_cb
                    (unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, mrs_num, mrs_opcode, mrs_mask));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_enable_wr_dbi_cb(
    int unit,
    int dram_index,
    int enable)
{
    uint32 mrs_opcode, mrs_mask, mrs_num;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d\n"),
                                 FUNCTION_NAME(), unit, dram_index, enable));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_wr_dbi_enable_set(unit, dram_index, enable));

    
    if (enable == 0)
    {
        mrs_opcode = UTILEX_SET_BITS_RANGE(1, 9, 9);
    }
    else
    {
        mrs_opcode = 0;
    }
    mrs_mask = UTILEX_BITS_MASK(9, 9);
    mrs_num = 1;
    SHR_IF_ERR_EXIT(dnx_gddr6_modify_mrs_cb
                    (unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, mrs_num, mrs_opcode, mrs_mask));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_enable_rd_dbi_cb(
    int unit,
    int dram_index,
    int enable)
{
    uint32 mrs_opcode, mrs_mask, mrs_num;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d\n"),
                                 FUNCTION_NAME(), unit, dram_index, enable));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_rd_dbi_enable_set(unit, dram_index, enable));

    
    if (enable == 0)
    {
        mrs_opcode = UTILEX_SET_BITS_RANGE(1, 8, 8);
    }
    else
    {
        mrs_opcode = 0;
    }
    mrs_mask = UTILEX_BITS_MASK(8, 8);
    mrs_num = 1;
    SHR_IF_ERR_EXIT(dnx_gddr6_modify_mrs_cb
                    (unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, mrs_num, mrs_opcode, mrs_mask));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_soft_reset_controller_without_dram_cb(
    int unit,
    int dram_index)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d\n"), FUNCTION_NAME(), unit, dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_channels_soft_init(unit, dram_index));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_rd_fifo_rstn_set(unit, dram_index, 0));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_rd_fifo_rstn_set(unit, dram_index, 1));

    
    for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pipelines_interrupts_clear(unit, dram_index, channel));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_bist_conf_set_cb(
    int unit,
    int dram_index,
    const g6phy16_bist_info_t * info)
{
    dnx_gddr6_bist_configuration_t bist_config = { 0 };
    int nof_channels;
    int override_bist_configuration;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d\n"), FUNCTION_NAME(), unit, dram_index));

    
    SHR_IF_ERR_EXIT(dnx_dram_db.override_bist_configurations_during_tuning.get(unit, &override_bist_configuration));
    if (!override_bist_configuration)
    {
        
        SHR_EXIT();
    }

    
    COMPILER_64_SET(bist_config.nof_commands, 0, info->bist_num_actions);
    bist_config.write_weight = info->write_weight;
    bist_config.read_weight = info->read_weight;
    bist_config.same_row = 0;
    bist_config.bank_mode = DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL;
    bist_config.bank_start = 0;
    bist_config.bank_end = dnx_data_dram.general_info.dram_info_get(unit)->nof_banks - 1;
    
    bist_config.column_mode = DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL;
    bist_config.column_start = 0;
    bist_config.column_end = dnx_data_dram.general_info.dram_info_get(unit)->nof_columns - 1;
    
    bist_config.row_mode = DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL;
    bist_config.row_start = 0;
    bist_config.row_end = dnx_data_dram.general_info.dram_info_get(unit)->nof_rows - 1;;
    bist_config.data_mode = DNX_GDDR6_BIST_DATA_MODE_PRBS;
    for (int seed_iter = 0; seed_iter < sizeof(bist_config.data_seed) / sizeof(bist_config.data_seed[0]); ++seed_iter)
    {
        bist_config.data_seed[seed_iter] = sal_rand();
    }
    for (int pattern_iter = 0; pattern_iter < 8; ++pattern_iter)
    {
        for (int arr_iter = 0; arr_iter < 8; ++arr_iter)
        {
            bist_config.bist_dq_pattern[pattern_iter][arr_iter] = info->data_pattern[pattern_iter][arr_iter];
        }
    }
    bist_config.bist_timer_us = info->bist_timer_us;

    
    nof_channels = dnx_data_dram.general_info.nof_channels_get(unit);
    for (int channel_iter = 0; channel_iter < nof_channels; ++channel_iter)
    {
        
        SHR_IF_ERR_EXIT(dnx_gddr6_data_bist_configure(unit, dram_index, channel_iter, &bist_config));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_bist_run_cb(
    int unit,
    int dram_index,
    const g6phy16_bist_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d\n"), FUNCTION_NAME(), unit, dram_index));

    
    for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_bist_start(unit, dram_index, channel));
    }

    
    if (info->bist_timer_us)
    {
        
        sal_usleep(info->bist_timer_us);

        
        for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_stop(unit, dram_index, channel));
        }
    }
    else
    {
        if (!info->bist_num_actions)
        {
            
            SHR_ERR_EXIT(_SHR_E_PARAM, "attempt to run Infinite BIST from CB context, this is invalid\n");
        }

        
        for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_run_done_poll(unit, dram_index, channel));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_bist_err_cnt_cb(
    int unit,
    int dram_index,
    g6phy16_bist_err_cnt_t * info)
{
    dnx_gddr6_bist_err_cnt_t bist_err_cnt;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    sal_memset(info, 0, sizeof(g6phy16_bist_err_cnt_t));
    for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_err_cnt_get(unit, dram_index, channel, &bist_err_cnt));
        info->bist_err_bmp |= bist_err_cnt.bist_data_err_bitmap[0] << (16 * channel);
        info->bist_err_cnt += bist_err_cnt.bist_data_err_cnt;
        info->bist_dbi_err_bmp |= bist_err_cnt.bist_dbi_err_bitmap[0] << (2 * channel);
        info->bist_dbi_err_cnt += bist_err_cnt.bist_dbi_err_cnt;
        info->bist_edc_err_bmp |= bist_err_cnt.bist_edc_err_bitmap[0] << (2 * channel);
        info->bist_edc_err_cnt += bist_err_cnt.bist_edc_err_cnt;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dram_init_cb(
    int unit,
    int dram_index,
    gddr6_dram_init_phase_t phase)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_init(unit, dram_index, phase));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dram_info_access_cb(
    int unit,
    g6phy16_shmoo_dram_info_t * shmoo_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(shmoo_info, _SHR_E_PARAM, "shmoo_info");

    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_dram_info_get(unit, shmoo_info));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_enable_wck2ck_training_cb(
    int unit,
    int dram_index,
    int enable)
{
    uint32 mrs_opcode, mrs_mask, mrs_num;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d\n"),
                                 FUNCTION_NAME(), unit, dram_index, enable));

    mrs_opcode = mrs_mask = 0;
    
    if (enable != 0)
    {
        UTILEX_SET_BIT(mrs_opcode, 1, 8);
    }
    else
    {
        UTILEX_SET_BIT(mrs_opcode, 0, 8);
    }
    UTILEX_SET_BIT(mrs_mask, 1, 8);
    mrs_num = 10;
    SHR_IF_ERR_EXIT(dnx_gddr6_modify_mrs_cb
                    (unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, mrs_num, mrs_opcode, mrs_mask));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_enable_write_leveling_cb(
    int unit,
    int dram_index,
    uint32 command_parity_lattency,
    int use_continious_dqs,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U
               (unit,
                "%s(): unit=%d, dram_index=%d, enable=%d, command_parity_lattency= 0x%x, use_continious_dqs=%d\n"),
               FUNCTION_NAME(), unit, dram_index, enable, command_parity_lattency, use_continious_dqs));

    SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_mpr_en_cb(
    int unit,
    int dram_index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d\n"),
                               FUNCTION_NAME(), unit, dram_index, enable));

    SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_mpr_load_cb(
    int unit,
    int dram_index,
    uint8 *mpr_pattern)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mpr_pattern, _SHR_E_PARAM, "mpr_pattern");

    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d\n"), FUNCTION_NAME(), unit, dram_index));

    SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_vendor_info_get_cb(
    int unit,
    int dram_index,
    g6phy16_vendor_info_t * vendor_info)
{
    dnx_dram_vendor_info_t dram_vendor_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vendor_info, _SHR_E_PARAM, "vendor_info");

    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d\n"), FUNCTION_NAME(), unit, dram_index));

    
    vendor_info->fifo_depth = 6;

    SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, dram_index, &dram_vendor_info));
    vendor_info->manufacture_id = dram_vendor_info.gddr6_info.manufacturer_vendor_code;
    vendor_info->revision_id = dram_vendor_info.gddr6_info.revision_id;
    vendor_info->dram_density = dram_vendor_info.gddr6_info.density;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dqs_pulse_gen_cb(
    int unit,
    int dram_index,
    int use_continious_dqs)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, use_continious_dqs=%d\n"),
                               FUNCTION_NAME(), unit, dram_index, use_continious_dqs));

    SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_training_bist_conf_set_cb(
    int unit,
    int dram_index,
    g6phy16_training_bist_info_t * info)
{
    uint32 entry_handle_id;
    uint64 nof_commands;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_TRAINING_BIST_COMMAND_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_BIST_MODE, INST_SINGLE,
                                 info->training_bist_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_READ_FIFO_DEPTH, INST_SINGLE, info->fifo_depth);
    COMPILER_64_SET(nof_commands, 0, info->nof_commands);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_NUM_OF_COMMANDS, INST_SINGLE, nof_commands);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CONSECUTIVE_WRITE_COMMANDS, INST_SINGLE,
                                 info->write_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CONSECUTIVE_READ_COMMANDS, INST_SINGLE,
                                 info->read_weight);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_TRAINING_CADT_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAINING_CADT_NUM_OF_COMMANDS, INST_SINGLE,
                                 info->nof_commands);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_MODE, INST_SINGLE, info->cadt_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_TO_CADT_PRD, INST_SINGLE,
                                 info->cadt_to_cadt_prd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_TO_RDEN_PRD, INST_SINGLE,
                                 info->cadt_to_rden_prd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_PRBS_MODE, INST_SINGLE,
                                 !!info->cadt_prbs_enabled);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_SEED, INST_SINGLE, info->cadt_seed);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_INVERT_CA_UI_2, INST_SINGLE,
                                 info->cadt_invert_caui2);
    for (int pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS; ++pattern_index)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_PATTERN_N, pattern_index,
                                     info->cadt_pattern[pattern_index]);
    }
    if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_cadt_exit_mode_when_done))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CADT_EXIT_MODE_WHEN_DONE, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_LDFF_TRAINING_PATTERN, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    for (int pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS; ++pattern_index)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_LDFF_DQ_PATTERN_N, pattern_index,
                                         info->ldff_data_pattern[pattern_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_LDFF_DBI_PATTERN_N, pattern_index,
                                     info->ldff_dbi_pattern[pattern_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_LDFF_EDC_PATTERN_N, pattern_index,
                                     info->ldff_edc_pattern[pattern_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_MODE, INST_SINGLE, info->data_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DBI_MODE, INST_SINGLE, info->dbi_mode);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRBS_DATA_SEED, INST_SINGLE,
                                     info->prbs_data_seed);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRBS_DBI_SEED, INST_SINGLE, info->prbs_dbi_seed);
    for (int pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS; ++pattern_index)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_DBI_PATTERN_N, pattern_index,
                                     info->bist_dbi_pattern[pattern_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG_PATTERN_1, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    for (int pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS / 2; ++pattern_index)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BIST_DQ_PATTERN_N, pattern_index,
                                         info->bist_data_pattern[pattern_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_GDDR6_DRAM_BIST_CONFIG_PATTERN_2, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    for (int pattern_index = 0; pattern_index < SHMOO_G6PHY16_BIST_NOF_PATTERNS / 2; ++pattern_index)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BIST_DQ_PATTERN_N, pattern_index,
                                         info->bist_data_pattern[pattern_index + 4]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if ((info->training_bist_mode == SHMOO_G6PHY16_TRAINING_BIST_LOAD_READ_FIFO) ||
        (info->training_bist_mode == SHMOO_G6PHY16_TRAINING_BIST_WRITE_THEN_READ_FROM_LDFF))
    {
        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_rd_fifo_rstn_set(unit, dram_index, 0));
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_rd_fifo_rstn_set(unit, dram_index, 1));
    }

    if (info->training_bist_mode != SHMOO_G6PHY16_TRAINING_BIST_ADDR_TRAINING)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_load_activate_command(unit, dram_index, 0  , 1  ));
    }
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_training_bist_run(unit, dram_index, 1));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_training_bist_run_done_poll(unit, dram_index));
    if (info->training_bist_mode != SHMOO_G6PHY16_TRAINING_BIST_ADDR_TRAINING)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_load_precharge_all_command(unit, dram_index, 1  ));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_training_bist_err_cnt_cb(
    int unit,
    int dram_index,
    g6phy16_training_bist_err_cnt_t * info)
{
    dnx_gddr6_bist_err_cnt_t bist_err_cnt;
    dnx_gddr6_bist_status_cnt_t bist_status_cnt;
    int nof_channels = dnx_data_dram.general_info.nof_channels_get(unit);
    int bytes_per_channel = dnx_data_dram.gddr6.bytes_per_channel_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    sal_memset(info, 0x0, sizeof(*info));
    
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    for (int channel = 0; channel < nof_channels; channel++)
    {
        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_err_cnt_get(unit, dram_index, channel, &bist_err_cnt));
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_status_cnt_get(unit, dram_index, channel, &bist_status_cnt));
        info->cadt_err_cnt += bist_err_cnt.cadt_err_cnt;
        info->cadt_err_bitmap[0] |= bist_err_cnt.cadt_err_bitmap[0] << (10 * channel);
        info->bist_data_err_cnt += bist_err_cnt.bist_data_err_cnt;
        info->bist_data_err_bitmap[0] |= bist_err_cnt.bist_data_err_bitmap[0] << (16 * channel);
        info->bist_edc_err_cnt += bist_err_cnt.bist_edc_err_cnt;
        info->bist_edc_err_bitmap[0] |= bist_err_cnt.bist_edc_err_bitmap[0] << (2 * channel);
        info->bist_dbi_err_cnt += bist_err_cnt.bist_dbi_err_cnt;
        info->bist_dbi_err_bitmap[0] |= bist_err_cnt.bist_dbi_err_bitmap[0] << (2 * channel);
        info->bist_read_command_counter += bist_status_cnt.rdtr_command_cnt[0];
        info->bist_write_command_counter += bist_status_cnt.wrtr_command_cnt[0];
        info->bist_data_counter += bist_status_cnt.read_training_data_cnt[0];
        info->bist_edc_counter += bist_status_cnt.read_edc_cnt[0];
        if ((bist_status_cnt.rdtr_command_cnt[0] == bist_status_cnt.read_training_data_cnt[0]) &&
            (bist_status_cnt.rdtr_command_cnt[0] == bist_status_cnt.read_edc_cnt[0]))
        {
            
            
            
            SHR_BITCLR_RANGE(info->bist_sanity_err_bitmap, channel * bytes_per_channel, bytes_per_channel);
        }
        else
        {
            
            SHR_BITSET_RANGE(info->bist_sanity_err_bitmap, channel * bytes_per_channel, bytes_per_channel);
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "EDC BMP:0x%08x; DEC err count:%08d; rdtr_cmd:%08d; read_data:%08d; edc_data:%08d;\n"),
                     info->bist_edc_err_bitmap[0], info->bist_edc_err_cnt, bist_status_cnt.rdtr_command_cnt[0],
                     bist_status_cnt.read_training_data_cnt[0], bist_status_cnt.read_edc_cnt[0]));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get_cb(
    int unit,
    int dram_index,
    int *pairs_were_swapped)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pairs_were_swapped, _SHR_E_PARAM, "pairs_were_swapped");

    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d\n"), FUNCTION_NAME(), unit, dram_index));

    
    if (dnx_data_dram.gddr6.dq_channel_swap_get(unit, dram_index)->dq_channel_swap)
    {
        *pairs_were_swapped = 1;
    }
    else
    {
        *pairs_were_swapped = 0;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_force_dqs_cb(
    int unit,
    int dram_index,
    uint32 force_dqs)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GDDR6_DRAM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_INDEX, dram_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WCK, INST_SINGLE, force_dqs ? 0x0 : 0xCCCC);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}


shr_error_e
dnx_gddr6_enable_refresh_cb(
    int unit,
    int dram_index,
    int enable,
    uint32 new_trefi,
    uint32 new_trefi_ab,
    uint32 *curr_refi,
    uint32 *curr_refi_ab)
{
    uint32 refresh_bitmap, refresh_ab_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(curr_refi, _SHR_E_PARAM, "curr_refi");
    
    SHR_INVOKE_VERIFY_DNX(dnx_gddr6_index_verify(unit, dram_index));
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): unit=%d, dram_index=%d, enable=%d, new_trefi=0x%x\n"),
                                 FUNCTION_NAME(), unit, dram_index, enable, new_trefi));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_refresh_enable_bitmap_get(unit, dram_index, &refresh_bitmap, &refresh_ab_bitmap));
    *curr_refi = refresh_bitmap;
    *curr_refi_ab = refresh_ab_bitmap;
    if (enable)
    {
        refresh_bitmap = new_trefi;
        refresh_ab_bitmap = new_trefi_ab;
        SHR_IF_ERR_EXIT(dnx_gddr6_dram_refresh_enable_bitmap_set(unit, dram_index, refresh_bitmap, refresh_ab_bitmap));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_dram_refresh_enable_bitmap_set(unit, dram_index, 0, 0));
    }

exit:
    SHR_FUNC_EXIT;
}
