/** \file   field_tcam.c
 * 
 *
 * Field init procedures for DNX.
 *
 * The file contains all procedures needed for field initialization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <bcm_int/dnx/field/tcam/field_tcam.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_prefix_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_cache_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.h>

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <shared/utilex/utilex_framework.h>

static shr_error_e
dnx_field_tcam_access_mapper_sw_state_init(
    int unit)
{
    sw_state_htbl_init_info_t hash_init;
    int hash_idx;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.init(unit));

    sal_memset(&hash_init, 0, sizeof(hash_init));
    hash_init.max_nof_elements = hash_init.expected_nof_elements = dnx_data_field.tcam.nof_big_bank_lines_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.key_2_entry_id_hash.alloc(unit));

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        for (hash_idx = 0; hash_idx < dnx_data_field.tcam.nof_banks_get(unit); hash_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                            key_2_entry_id_hash.create(unit, core, hash_idx, &hash_init));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_sw_state_init(
    int unit)
{
    sw_state_htbl_init_info_t hash_init;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.alloc(unit));

    sal_memset(&hash_init, 0, sizeof(hash_init));
    hash_init.max_nof_elements = hash_init.expected_nof_elements =
        dnx_data_field.tcam.nof_big_banks_get(unit) * dnx_data_field.tcam.nof_big_bank_lines_get(unit) +
        dnx_data_field.tcam.nof_small_banks_get(unit) * dnx_data_field.tcam.nof_big_bank_lines_get(unit);

    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.alloc_bitmap(unit, core));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.alloc_bitmap(unit, core));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.create(unit, core, &hash_init));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_manager_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw.tcam_handlers.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_bank_sw_state_init(
    int unit)
{
    int bank_index;
    sw_state_ll_init_info_t init_info;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(init_info));
    init_info.max_nof_elements = dnx_data_field.tcam.nof_tcam_handlers_get(unit);
    init_info.expected_nof_elements = dnx_data_field.tcam.nof_tcam_handlers_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.alloc(unit));

    for (bank_index = 0; bank_index < dnx_data_field.tcam.nof_banks_get(unit); bank_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                        nof_free_entries.alloc(unit, bank_index, dnx_data_device.general.nof_cores_get(unit),
                                               DNX_FIELD_TCAM_ENTRY_SIZE_COUNT));
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            uint32 max_nof_half_entries;
            uint32 max_nof_entries;
            if (bank_index < dnx_data_field.tcam.nof_big_banks_get(unit))
            {
                max_nof_half_entries = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
            }
            else
            {
                max_nof_half_entries = dnx_data_field.tcam.nof_small_bank_lines_get(unit);
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            nof_free_entries.set(unit, bank_index, core, DNX_FIELD_TCAM_ENTRY_SIZE_HALF,
                                                 max_nof_half_entries));

            max_nof_entries = max_nof_half_entries / 2;
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            nof_free_entries.set(unit, bank_index, core, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE,
                                                 max_nof_entries));
            /** For double sized entries, we only set even bank indexes to number of single entries, while setting odd bank indexes number to 0 */
            if (bank_index % 2)
            {
                max_nof_entries = 0;
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw.tcam_banks.
                            nof_free_entries.set(unit, bank_index, core, DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE,
                                                 max_nof_entries));
        }
        dnx_field_tcam_bank_sw.tcam_banks.active_handlers_id.create_empty(unit, bank_index, &init_info);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_sw_state_handler_info_init(
    int unit,
    uint32 handler_id)
{
    sw_state_ll_init_info_t priorities_init_info;
    uint32 key;
    dnx_field_tcam_location_range_t range;
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.priorities_range.alloc(unit, handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.entries_occupation_bitmap.alloc(unit, handler_id));
    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        /*
         * -- Init the priorities list -- 
         */
        sal_memset(&priorities_init_info, 0x0, sizeof(priorities_init_info));
        /*
         * Max piorities is equal to the max 80b entries we can have in TCAM + 2 for boundary nodes
         */
        priorities_init_info.max_nof_elements = dnx_data_field.tcam.nof_entries_80_bits_get(unit) + 1 + 2;

        priorities_init_info.key_cmp_cb_name = "tcam_sorted_list_cmp_priority";

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.create_empty(unit, handler_id, core, &priorities_init_info));

        /*
         * Add two 'limit' nodes to the linked list to prevent limit checking
         * within the linked list handling code.
         */
        /*
         * The smallest priority that exists 
         */
        key = 0;
        /*
         * min/max of -1, insertion range will start from 0 
         */
        range.min = -1;
        range.max = -1;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.add(unit, handler_id, core, &key, &range));

        /*
         * The highest priority valid for TCAM entry.
         */
        key = dnx_data_field.tcam.max_tcam_priority_get(unit);
        /*
         * set min/max to be the max size of the TCAM banks 
         */
        range.min = dnx_data_field.tcam.tcam_banks_last_index_get(unit);
        range.max = dnx_data_field.tcam.tcam_banks_last_index_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.add(unit, handler_id, core, &key, &range));

        /*
         * -- Init the entries occupation bitmap -- 
         */
        SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));
        btmp_init_info.size = dnx_data_field.tcam.tcam_banks_size_get(unit);

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        entries_occupation_bitmap.create(unit, handler_id, core, &btmp_init_info));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_sw_state_init(
    int unit)
{
    uint32 handler_index;
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.alloc(unit));

    for (handler_index = 0; handler_index < dnx_data_field.tcam.nof_tcam_handlers_get(unit); handler_index++)
    {
        /*
         * Init new location_handler_info with empty sorted list 
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_state_handler_info_init(unit, handler_index));
    }
    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));
    btmp_init_info.size = dnx_data_field.tcam.tcam_banks_size_get(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_banks_occupation_bitmap.alloc(unit));
    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_banks_occupation_bitmap.create(unit, core, &btmp_init_info));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.location_priority_arr.alloc(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_profile_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw.access_profiles.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_prefix_sw_state_init(
    int unit)
{
    int bank_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.alloc(unit));

    for (bank_index = 0; bank_index < dnx_data_field.tcam.nof_banks_get(unit); bank_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.prefix_handler_map.alloc(unit, bank_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initiate the parametrs of SW state for the TCAM shadow (including allocation if needed).
 * \param [in] unit  - Device id
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_tcam_shadow_sw_state_init(
    int unit)
{
    int bank_index;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.init(unit));

        /*
         * Allocate by the number of the banks and action tables.
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.alloc(unit, dnx_data_field.tcam.nof_banks_get(unit)));
        SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.
                        payload_table.alloc(unit, dnx_data_field.tcam.nof_payload_tables_get(unit)));

        /*
         * For each bank, allocate the number of entries, depending on whether it is a big bank or a small bank.
         */
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            for (bank_index = 0; bank_index < dnx_data_field.tcam.nof_banks_get(unit); bank_index++)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                                entry_key.alloc(unit, core, bank_index, FIELD_TCAM_BANK_NOF_ENTRIES(bank_index),
                                                DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE));
                /*
                 * Also allocate the two action tables associated with the bank.
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.
                                entry_payload.alloc(unit, core, bank_index * 2,
                                                    FIELD_TCAM_BANK_NOF_ENTRIES(bank_index) / 2,
                                                    DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE));
                SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.
                                entry_payload.alloc(unit, core, bank_index * 2 + 1,
                                                    FIELD_TCAM_BANK_NOF_ENTRIES(bank_index) / 2,
                                                    DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_hit_indication_sw_state_init(
    int unit)
{
    int sram_index;
    int nof_big_banks_sram_indexes;
    int nof_small_banks_sram_indexes;

    SHR_FUNC_INIT_VARS(unit);

    nof_big_banks_sram_indexes =
        dnx_data_field.tcam.nof_big_banks_get(unit) * dnx_data_field.tcam.action_width_selector_size_get(unit);
    nof_small_banks_sram_indexes =
        dnx_data_field.tcam.nof_small_banks_get(unit) * dnx_data_field.tcam.action_width_selector_size_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication.tcam_action_hit_indication.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.alloc(unit));

    for (sram_index = 0; sram_index < nof_big_banks_sram_indexes; sram_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.alloc(unit, sram_index));
    }

    for (sram_index = 0; sram_index < nof_small_banks_sram_indexes; sram_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.
                        bank_line_id.alloc(unit, sram_index));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_manager_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw_state_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_tcam_shadow_sw_state_init(unit));

    if (dnx_data_field.features.hitbit_volatile_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_hit_indication_sw_state_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
