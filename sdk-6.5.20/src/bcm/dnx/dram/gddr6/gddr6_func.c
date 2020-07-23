

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
#include <sal/core/boot.h>


#include <soc/debug.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/shmoo_combo16.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_func.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_cb.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_dbal_access.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <bcm_int/dnx/dram/dram.h>


static shr_error_e
_dnx_gddr6_check_swap(
    int unit,
    int nof_items,
    uint8 data_array[])
{
    uint32 values_bitmap, cells_bitmap;
    int item;
    SHR_FUNC_INIT_VARS(unit);

    
    values_bitmap = 0;
    cells_bitmap = 0;

    
    for (item = 0; item < nof_items; ++item)
    {
        if (data_array[item] != item)
        {
            values_bitmap |= 1 << data_array[item];
            cells_bitmap |= 1 << item;
        }
    }

    
    if ((values_bitmap ^ cells_bitmap) != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U
                   (unit,
                    "Swap Doesn't make sence. swapped cells(BitMap): 0x%x\t swapped values(BitMap): 0x%x should match\n"),
                   cells_bitmap, values_bitmap));
        SHR_IF_ERR_EXIT(_SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_gddr6_check_board_swap(
    int unit)
{
    int byte;
    int dram_index, nof_drams, channel;
    uint32 dram_bitmap[1];
    uint8 dq_map[UTILEX_NOF_BITS_IN_BYTE];
    uint8 ca_map[DNX_DATA_MAX_DRAM_GDDR6_NOF_CA_BITS + 1];
    SHR_FUNC_INIT_VARS(unit);

    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_index)
    {
        for (byte = 0; byte < 4; ++byte)
        {
            
            sal_memcpy(dq_map, dnx_data_dram.gddr6.dq_map_get(unit, dram_index, byte)->dq_map, UTILEX_NOF_BITS_IN_BYTE);
            SHR_IF_ERR_EXIT(_dnx_gddr6_check_swap(unit, UTILEX_NOF_BITS_IN_BYTE, dq_map));
        }
        
        for (channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
        {
            sal_memcpy(ca_map, dnx_data_dram.gddr6.ca_map_get(unit, dram_index, channel)->ca_map,
                       dnx_data_dram.gddr6.nof_ca_bits_get(unit) + 1);
            SHR_IF_ERR_EXIT(_dnx_gddr6_check_swap(unit, (dnx_data_dram.gddr6.nof_ca_bits_get(unit) + 1), ca_map));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_gddr6_cpu_command(
    int unit,
    uint32 dram_index,
    uint32 rising_edge_ca,
    uint32 falling_edge_ca,
    uint32 cabi_enabled)
{
    uint32 cabi_n = 3;
    int counter, zero_counter, number_of_ca_bits;
    uint32 negative_val[1], val[1];
    uint32 ca;
    uint32 ca_width;
    SHR_FUNC_INIT_VARS(unit);

    ca_width = dnx_data_dram.gddr6.nof_ca_bits_get(unit);
    number_of_ca_bits = dnx_data_dram.gddr6.use_11bits_ca_get(unit) ? ca_width : ca_width - 1;

    if (cabi_enabled)
    {
        shr_bitop_range_count(&rising_edge_ca, 0, number_of_ca_bits, &counter);
        zero_counter = number_of_ca_bits - counter;
        if (zero_counter > 5)
        {
            
            
            cabi_n &= ~0x2;
            negative_val[0] = 0;
            val[0] = rising_edge_ca;
            shr_bitop_range_negate(val, 0, ca_width, negative_val);
            rising_edge_ca = negative_val[0];
        }

        shr_bitop_range_count(&falling_edge_ca, 0, number_of_ca_bits, &counter);
        zero_counter = number_of_ca_bits - counter;
        if (zero_counter > 5)
        {
            
            
            cabi_n &= ~1;
            negative_val[0] = 0;
            val[0] = falling_edge_ca;
            shr_bitop_range_negate(val, 0, ca_width, negative_val);
            falling_edge_ca = negative_val[0];
        }
    }
    ca = (rising_edge_ca << ca_width) | falling_edge_ca;
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_cpu_command_set(unit, dram_index, ca, cabi_n));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_gddr6_configure_gddr6_board_swapping(
    int unit,
    int dram_index)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_gddr6_check_board_swap(unit));

    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): configure dram%d command address bit map\n"), FUNCTION_NAME(), dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_ca_bit_map_set(unit, dram_index));
    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): configure dram%d dq byte bit map\n"), FUNCTION_NAME(), dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dq_bit_map_set(unit, dram_index));
    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): configure dram%d cadt byte map\n"), FUNCTION_NAME(), dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_cadt_byte_map_set(unit, dram_index));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_gddr6_load_refresh_all_command(
    int unit,
    int dram_ndx)
{
    uint32 rising_edge_ca, falling_edge_ca, cabi_enabled;
    SHR_FUNC_INIT_VARS(unit);

    
    rising_edge_ca = 0x200;
    
    falling_edge_ca = 0x110;
    
    cabi_enabled = 0x1;
    
    SHR_IF_ERR_EXIT(dnx_gddr6_cpu_command(unit, dram_ndx, rising_edge_ca, falling_edge_ca, cabi_enabled));
    
    sal_usleep(1);

exit:
    SHR_FUNC_EXIT;

}


static shr_error_e
dnx_gddr6_set_dram_interface_static_values(
    int unit,
    int dram_index)
{
    uint32 ca;
    uint32 cabi_n;
    SHR_FUNC_INIT_VARS(unit);

    
    
    ca = UTILEX_SET_BITS_RANGE(dnx_data_dram.gddr6.cal_termination_get(unit), 1, 0);
    ca |= UTILEX_SET_BITS_RANGE(dnx_data_dram.gddr6.cah_termination_get(unit), 3, 2);
    ca |= UTILEX_SET_BITS_RANGE(dnx_data_dram.gddr6.ck_odt_get(unit), 5, 4);
    
    ca |= UTILEX_SET_BITS_RANGE(dnx_data_dram.gddr6.dram_mode_get(unit), 6, 6);
    ca |= UTILEX_SET_BITS_RANGE(0xf, 10, 7);
    
    cabi_n = 0x3;
    ca = (ca << dnx_data_dram.gddr6.nof_ca_bits_get(unit)) | ca;

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_force_constant_cpu_value_set(unit, dram_index, ca, cabi_n));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_wck_set(unit, dram_index, 0x0000));

    
    
    SHR_IF_ERR_EXIT(dnx_gddr6_phy_reg_modify_cb(unit, dram_index, 0xe01, 0x100, 0x1ff));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_shmoo_dram_info_get(
    int unit,
    g6phy16_shmoo_dram_info_t * shmoo_info)
{
    int dram_index, channel;
    uint32 dram_bitmap[1];
    int nof_drams;
    int channels_in_dram, byte, byte_index;
    int channel_swapped;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(shmoo_info, _SHR_E_PARAM, "shmoo_info");

    
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): no data into phy\n"), FUNCTION_NAME()));

    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;

    shmoo_info->ctl_type = dnx_data_dram.general_info.dram_info_get(unit)->ctl_type;
    shmoo_info->dram_type = dnx_data_dram.general_info.dram_info_get(unit)->dram_type;
    shmoo_info->dram_bitmap = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    shmoo_info->num_columns = dnx_data_dram.general_info.dram_info_get(unit)->nof_columns;
    shmoo_info->num_rows = dnx_data_dram.general_info.dram_info_get(unit)->nof_rows;
    shmoo_info->num_banks = dnx_data_dram.general_info.dram_info_get(unit)->nof_banks;
    shmoo_info->data_rate_mbps = dnx_data_dram.general_info.dram_info_get(unit)->data_rate;
    shmoo_info->ref_clk_mhz = dnx_data_dram.general_info.dram_info_get(unit)->ref_clock;
    shmoo_info->refi = dnx_data_dram.gddr6.refresh_intervals_get(unit)->trefiab;
    shmoo_info->command_parity_latency = dnx_data_dram.general_info.command_parity_latency_get(unit);
    if (SAL_BOOT_PLISIM)
    {
        shmoo_info->sim_system_mode = 1;
    }
    else
    {
        shmoo_info->sim_system_mode = 0;
    }
    shmoo_info->ref_clk_bitmap = 1;

    channels_in_dram = dnx_data_dram.general_info.nof_channels_get(unit);
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_index)
    {
        
        shmoo_info->zq_cal_array[dram_index] = dnx_data_dram.gddr6.master_phy_get(unit, dram_index)->master_dram_index;

        channel_swapped = dnx_data_dram.gddr6.dq_channel_swap_get(unit, dram_index)->dq_channel_swap;
        for (channel = 0; channel < channels_in_dram; channel++)
        {
            for (byte = 0; byte < dnx_data_dram.gddr6.bytes_per_channel_get(unit); byte++)
            {
                if (channel_swapped)
                {
                    byte_index = byte + DNX_DATA_MAX_DRAM_GDDR6_BYTES_PER_CHANNEL * (channels_in_dram - 1 - channel);
                }
                else
                {
                    byte_index = byte + DNX_DATA_MAX_DRAM_GDDR6_BYTES_PER_CHANNEL * channel;
                }
                for (int dq_bit_iter = 0; dq_bit_iter < SHMOO_G6PHY16_BYTE; ++dq_bit_iter)
                {
                    shmoo_info->dq_swap[dram_index][byte_index][dq_bit_iter] =
                        dnx_data_dram.gddr6.dq_map_get(unit, dram_index, byte_index)->dq_map[dq_bit_iter];
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_shmoo_vendor_info_init(
    int unit,
    g6phy16_vendor_info_t * vendor_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vendor_info, _SHR_E_PARAM, "shmoo_info");

    
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): no data into phy\n"), FUNCTION_NAME()));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_controller_init(
    int unit,
    int dram_index,
    int ignore_vendor)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_controller_config(unit, dram_index, ignore_vendor));
    
    SHR_IF_ERR_EXIT(dnx_gddr6_configure_gddr6_board_swapping(unit, dram_index));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_configure_plls(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info)
{
    uint32 pllRefclkSel = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(pll_info, _SHR_E_PARAM, "pll_info");

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U
                 (unit, "%s(): (enter) dram_ndx=%d,"
                  "pll_info->iso_in=%d, pll_info->ldo_ctrl=%d,pll_info->ndiv_frac=%d,"
                  "pll_info->pdiv=%d, pll_info->ndiv_int=%d, pll_info->ssc_limit=%d,"
                  "pll_info->ssc_mode=%d, pll_info->ssc_step=%d, pll_info->pll_ctrl=%d,"
                  "pll_info->vco_sel=%d, pll_info->mdiv=%d, pll_info->fref_eff_info=%d,"
                  "pll_info->en_ctrl=%d\n"),
                 FUNCTION_NAME(), dram_index,
                 pll_info->iso_in, pll_info->ldo_ctrl, pll_info->ndiv_frac,
                 pll_info->pdiv, pll_info->ndiv_int, pll_info->ssc_limit,
                 pll_info->ssc_mode, pll_info->ssc_step, pll_info->pll_ctrl,
                 pll_info->vco_sel, pll_info->mdiv, pll_info->fref_eff_info, pll_info->en_ctrl));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_div_config_set(unit, dram_index, pll_info));
    
    if (1 == dram_index)
    {
        pllRefclkSel = 1;
    }
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_refclk_sel(unit, dram_index, pllRefclkSel));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_post_reset_set(unit, dram_index, 1));

    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): (enter) dram_ndx=%d, make sure pll is reset"), FUNCTION_NAME(), dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_reset(unit, dram_index, 1));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_init(unit, dram_index, pll_info));

    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): (enter) dram_ndx=%d, make sure phy is reset"), FUNCTION_NAME(), dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_phy_out_of_reset_config(unit, dram_index, 0));

    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): (enter) dram_ndx=%d, make sure pll is out of reset"), FUNCTION_NAME(),
                 dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_reset(unit, dram_index, 0));

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_pll_status_locked_poll(unit, dram_index));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_dram_init(
    int unit,
    int dram_index,
    gddr6_dram_init_phase_t phase)
{
    int mr_index;
    SHR_FUNC_INIT_VARS(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "dram init, dram_index=%d, phase=%d\n"), dram_index, phase));

    if ((phase == SHMOO_GDDR6_DRAM_INIT_UNTIL_CADT) || (phase == SHMOO_GDDR6_DRAM_INIT_ALL))
    {
        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_reset_config(unit, dram_index, 0));

        
        sal_usleep(200);

        
        SHR_IF_ERR_EXIT(dnx_gddr6_set_dram_interface_static_values(unit, dram_index));

        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_reset_config(unit, dram_index, 1));

        
        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_release_constant_cpu_set(unit, dram_index));

        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_cke_set(unit, dram_index, 1));
        
        sal_usleep(1000);

        
        SHR_IF_ERR_EXIT(dnx_gddr6_phy_reg_modify_cb(unit, dram_index, 0xe01, 0x1f0, 0x1ff));
        
        sal_usleep(UTILEX_DIV_ROUND_UP(100, dnx_data_dram.general_info.frequency_get(unit)));
    }

    if ((phase == SHMOO_GDDR6_DRAM_INIT_AFTER_CADT) || (phase == SHMOO_GDDR6_DRAM_INIT_ALL))
    {
        
        dnx_dram_vendor_info_t dram_vendor_info;
        SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, dram_index, &dram_vendor_info));

        
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_mr_init_set(unit, dram_index, 10));

        
        if ((dram_vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
            && (dnx_data_dram.general_info.frequency_get(unit) > 875))
        {
            
            SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_wck_set(unit, dram_index, 0xF0F0));
        }
        else
        {
            
            SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_wck_set(unit, dram_index, 0xCCCC));
        }

        
        for (mr_index = 0; mr_index < dnx_data_dram.general_info.nof_mrs_get(unit); mr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_mr_init_set(unit, dram_index, mr_index));
        }

        
        for (int count = 0; count < 2; ++count)
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_load_refresh_all_command(unit, dram_index));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_load_activate_command(
    int unit,
    int dram_index,
    uint32 bank,
    uint32 cabi_enabled)
{
    uint32 rising_edge_ca, falling_edge_ca;
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): load_activate_command to dram ndx=%d\n"), FUNCTION_NAME(), dram_index));
    rising_edge_ca = UTILEX_SET_BITS_RANGE(0, 3, 0);
    rising_edge_ca |= UTILEX_SET_BITS_RANGE((bank & 0xF), 7, 4);
    rising_edge_ca |= UTILEX_SET_BITS_RANGE(0x4, 10, 8);
    falling_edge_ca = 0;

    SHR_IF_ERR_EXIT(dnx_gddr6_cpu_command(unit, dram_index, rising_edge_ca, falling_edge_ca, cabi_enabled));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_load_precharge_all_command(
    int unit,
    int dram_index,
    uint32 cabi_enabled)
{
    uint32 rising_edge_ca, falling_edge_ca;
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(): dnx_gddr6_load_precharge_all_command to dram_index=%d\n"),
                 FUNCTION_NAME(), dram_index));
    rising_edge_ca = UTILEX_SET_BITS_RANGE(0, 7, 0);
    rising_edge_ca |= UTILEX_SET_BITS_RANGE(0x2, 10, 8);

    falling_edge_ca = UTILEX_SET_BITS_RANGE(0, 3, 0);
    falling_edge_ca |= UTILEX_SET_BITS_RANGE(1, 4, 4);
    falling_edge_ca |= UTILEX_SET_BITS_RANGE(0, 7, 5);
    falling_edge_ca |= UTILEX_SET_BITS_RANGE(0, 10, 8);

    SHR_IF_ERR_EXIT(dnx_gddr6_cpu_command(unit, dram_index, rising_edge_ca, falling_edge_ca, cabi_enabled));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_data_bist_configure(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_configuration_t * bist_cfg)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "configure bist: start dram_index=%d\n"), dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_configure_set(unit, dram_index, channel, bist_cfg));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_bist_start(
    int unit,
    int dram_index,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_run(unit, dram_index, channel, 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_gddr6_bist_stop(
    int unit,
    int dram_index,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_run(unit, dram_index, channel, 0));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_gddr6_bist_run_done_poll(
    int unit,
    int dram_index,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_run_done_poll(unit, dram_index, channel));

exit:
    SHR_FUNC_EXIT;
}
