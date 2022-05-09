
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/shmoo_hbm16.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_cb.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <shared/utilex/utilex_rand.h>
#include "hbmc_dbal_access.h"
#include <sal/core/boot.h>
#include <sal/core/thread.h>

shr_error_e
dnx_hbmc_modify_mrs_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int mr_index,
    uint32 data,
    uint32 mask)
{
    uint32 mr_val = 0;
    uint32 allowed_data_mask;
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): hbm_ndx=%d, channel=%d, mr_index=%d, data=0x%x, mask=0x%x\n"),
                 FUNCTION_NAME(), hbm_ndx, channel, mr_index, data, mask));

    allowed_data_mask = dnx_data_dram.general_info.mr_mask_get(unit);
    if ((data | mask) & ~(allowed_data_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bad param\\s data=0x%x mask=0x%x exceeding allowed bit mask 0x%x\n",
                     data, mask, allowed_data_mask);
    }

    SHR_IF_ERR_EXIT(dnx_hbmc_mode_register_get(unit, hbm_ndx, channel, mr_index, &mr_val));

    mr_val &= ~mask;

    mr_val |= (mask & data);

    SHR_IF_ERR_EXIT(dnx_hbmc_mode_register_set(unit, hbm_ndx, channel, mr_index, mr_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_pll_set_cb(
    int unit,
    int hbm_ndx,
    const hbmc_shmoo_pll_t * pll_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_configure_plls(unit, hbm_ndx, pll_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_hbmc_index_verify_with_midstack(
    int unit,
    int hbm_ndx,
    int channel)
{
    uint32 nof_hbms;
    uint32 nof_channels_per_hbm;
    SHR_FUNC_INIT_VARS(unit);

    nof_hbms = dnx_data_dram.general_info.max_nof_drams_get(unit);
    nof_channels_per_hbm = dnx_data_dram.general_info.nof_channels_get(unit);
    if ((hbm_ndx >= nof_hbms) || ((channel >= nof_channels_per_hbm) && (channel != SHMOO_HBM16_MIDSTACK_CHANNEL)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bad param hbm_index=%d, channel=%d \n", hbm_ndx, channel);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_phy_reg_read_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 *data)
{
    int is_midstack = 0;
    uint32 addr_corrected = 0;
    uint32 phy_reg_addr_mask;
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): hbm_ndx=%d, channel=%d, phy_reg_address=0x%x\n"),
                                 FUNCTION_NAME(), hbm_ndx, channel, addr));

    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    SHR_INVOKE_VERIFY_DNXC(dnx_hbmc_index_verify_with_midstack(unit, hbm_ndx, channel));

    phy_reg_addr_mask = dnx_data_dram.general_info.phy_address_mask_get(unit);
    addr_corrected = addr & phy_reg_addr_mask;

    is_midstack = (channel == -1);

    if (is_midstack)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_phy_midstack_register_get(unit, hbm_ndx, addr_corrected, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_phy_channel_register_get(unit, hbm_ndx, channel, addr_corrected, data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_phy_reg_write_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data)
{
    int is_midstack = 0;
    uint32 phy_reg_addr_mask;
    uint32 addr_corrected = 0;
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): hbm_ndx=%d, channel=%d, phy_reg_address=0x%x\n"),
                                 FUNCTION_NAME(), hbm_ndx, channel, addr));

    SHR_INVOKE_VERIFY_DNXC(dnx_hbmc_index_verify_with_midstack(unit, hbm_ndx, channel));

    phy_reg_addr_mask = dnx_data_dram.general_info.phy_address_mask_get(unit);
    addr_corrected = addr & phy_reg_addr_mask;

    is_midstack = (channel == -1);

    if (is_midstack)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_phy_midstack_register_set(unit, hbm_ndx, addr_corrected, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_phy_channel_register_set(unit, hbm_ndx, channel, addr_corrected, data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_phy_reg_modify_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data,
    uint32 mask)
{
    uint32 old_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_phy_reg_read_cb(unit, hbm_ndx, channel, addr, &old_data));

    old_data &= ~mask;
    old_data |= (mask & data);

    SHR_IF_ERR_EXIT(dnx_hbmc_phy_reg_write_cb(unit, hbm_ndx, channel, addr, old_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_enable_wr_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(SOC_E_UNAVAIL, "Not implemented for HBM\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_enable_rd_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(SOC_E_UNAVAIL, "Not implemented for HBM\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_enable_addr_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(SOC_E_UNAVAIL, "Not implemented for HBM\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_enable_wr_dbi_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(SOC_E_UNAVAIL, "Not implemented for HBM\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_enable_rd_dbi_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(SOC_E_UNAVAIL, "Not implemented for HBM\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_soft_reset_controller_without_dram_cb(
    int unit,
    int hbm_ndx,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_hbmc_channel_soft_init(unit, hbm_ndx, channel));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_bist_conf_set_cb(
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    const hbmc_shmoo_bist_info_t * info)
{
    int dummy = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_hbmc_bist_run(unit, flags, hbm_ndx, channel, TRUE, info, &dummy));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_bist_err_cnt_cb(
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters)
{
    int soft_init_channel;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(error_counters, _SHR_E_PARAM, "error_counters");

    SHR_INVOKE_VERIFY_DNXC(dnx_hbmc_dram_and_channel_indexes_verify(unit, hbm_ndx, channel));

    if (flags & HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_WRITE_LFSR)
    {

        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_bist_write_lfsr_error_counters_get
                        (unit, hbm_ndx, channel, error_counters));
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_bist_error_counters_get(unit, hbm_ndx, channel, error_counters));

        error_counters->bist_global_err_cnt = error_counters->bist_data_err_cnt +
            error_counters->bist_dbi_err_cnt + error_counters->bist_dm_err_cnt;
    }

    SHR_IF_ERR_EXIT(dnx_dram_db.channel_soft_init_after_bist.get(unit, &soft_init_channel));
    if (soft_init_channel)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_channel_soft_init(unit, hbm_ndx, channel));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_enable_refresh_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_hbmc_dram_and_channel_indexes_verify(unit, hbm_ndx, channel));

    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_enable_refresh_set(unit, hbm_ndx, channel, enable ? 1 : 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_dram_init_cb(
    int unit,
    int hbm_ndx,
    int phase)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_hbm_init(unit, hbm_ndx));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_model_part_get_cb(
    int unit,
    int hbm_index,
    hbmc_shmoo_hbm_model_part_t * model_part)
{
    uint32 model_part_number;
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        *model_part = HBM_TYPE_SAMSUNG_DIE_X;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_dram_db.vendor_info.model_part_number.get(unit, hbm_index, &model_part_number));
        switch (model_part_number)
        {
            case 0x40:
            case 0x41:
            {
                *model_part = HBM_TYPE_SAMSUNG_DIE_X;
                break;
            }
            case 0x21:
            case 0x22:
            {
                *model_part = HBM_TYPE_SAMSUNG_DIE_B;
                break;
            }
            default:
            {
                *model_part = HBM_TYPE_INVALID;
                SHR_ERR_EXIT(_SHR_E_PARAM, "Model part number (0x%x) is unknown\n", model_part_number);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_bist_status_get_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_bist_status_get
                    (unit, hbm_ndx, channel, read_cmd_counter, write_cmd_counter, read_data_counter));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_dram_info_access_cb(
    int unit,
    hbmc_shmoo_dram_info_t * shmoo_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(shmoo_info, _SHR_E_PARAM, "shmoo_info");

    shmoo_info->ctl_type = dnx_data_dram.general_info.dram_info_get(unit)->ctl_type;
    shmoo_info->data_rate_mbps = dnx_data_dram.general_info.dram_info_get(unit)->data_rate;
    shmoo_info->dram_bitmap = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    shmoo_info->dram_type = dnx_data_dram.general_info.dram_info_get(unit)->dram_type;
    shmoo_info->num_banks = dnx_data_dram.general_info.dram_info_get(unit)->nof_banks;
    shmoo_info->num_columns = dnx_data_dram.general_info.dram_info_get(unit)->nof_columns;
    shmoo_info->num_rows = dnx_data_dram.general_info.dram_info_get(unit)->nof_rows;
    shmoo_info->ref_clk_mhz = dnx_data_dram.general_info.dram_info_get(unit)->ref_clock;
    shmoo_info->parity_latency = dnx_data_dram.general_info.actual_parity_latency_get(unit);
    shmoo_info->sim_system_mode = 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_hbmc_phy_channel_dwords_alignment_check_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int *is_aligned)
{

    hbmc_shmoo_bist_info_t biPtr;
    hbmc_shmoo_bist_err_cnt_t be;
    int i;
    uint32 write_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
    uint32 read_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
    uint32 read_data_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
    uint32 seed = 0x65464;
    UTILEX_RAND utilex_rand;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&biPtr, 0, sizeof(biPtr));

    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_dword_alignment_check))
    {

        *is_aligned = 1;
        SHR_EXIT();
    }

    biPtr.write_weight = 0;
    biPtr.read_weight = 100;
    biPtr.bist_num_actions[0] = 100;

    biPtr.bist_timer_us = 0;
    biPtr.bist_num_actions[1] = 0;
    biPtr.row_start_index = 0x00000000;
    biPtr.column_start_index = 0x00000000;
    biPtr.bank_start_index = 0x00000000;
    biPtr.row_end_index = 0x00003FFF;
    biPtr.column_end_index = 0x0000001F;
    biPtr.bank_end_index = 0x0000001F;
    biPtr.bist_refresh_enable = 0;
    biPtr.bist_ignore_address = 0;
    biPtr.same_row_commands = 8;
    biPtr.data_mode = DBAL_ENUM_FVAL_DRAM_BIST_MODE_PRBS;
    utilex_rand_seed_set(&utilex_rand, seed);
    for (i = 0; i < HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS; i++)
    {
        biPtr.prbs_seeds[i] = utilex_rand_int(&utilex_rand) & 0xFFFFF;
    }

    SHR_IF_ERR_EXIT(dnx_hbmc_bist_status_get_cb
                    (unit, hbm_ndx, channel, read_cmd_counter, write_cmd_counter, read_data_counter));

    SHR_IF_ERR_EXIT(dnx_hbmc_bist_conf_set_cb
                    (unit, HBMC_SHMOO_CFG_FLAG_FORCE_BIST_CONFIGURATION, hbm_ndx, channel, &biPtr));

    SHR_IF_ERR_EXIT(dnx_hbmc_bist_err_cnt_cb(unit, 0, hbm_ndx, channel, &be));

    SHR_IF_ERR_EXIT(dnx_hbmc_bist_status_get_cb
                    (unit, hbm_ndx, channel, read_cmd_counter, write_cmd_counter, read_data_counter));

    if ((read_data_counter[0] == 100) && (read_data_counter[1] == 0))
    {
        *is_aligned = 1;
    }
    else
    {
        *is_aligned = 0;
    }

exit:
    SHR_FUNC_EXIT;
}
