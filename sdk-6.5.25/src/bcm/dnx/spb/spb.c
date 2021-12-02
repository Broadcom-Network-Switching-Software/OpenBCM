/** \file src/bcm/dnx/spb/spb.c
 * 
 *
 * Sram Packet Buffer configuration implementation
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/spb/spb.h>
#include <soc/mem.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_spb.h>
#include <sal/core/boot.h> /** Included for SAL_BOOT_PLISIM */
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include "spb_otp.h"
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

#define EXPECTED_PTR_COUNT_AFTER_BANK_DISABLE(unit)     ((dnx_data_spb.ocb.nof_banks_per_ring_get(unit) - 1) * dnx_data_spb.ocb.nof_ptrs_per_bank_get(unit))

/**
 * \brief - set spb dynamic configuration
 * 
 * \param [in] unit - unit number
 * \param [in] enable_data_path - enable data path
 * \param [in] max_buffers_threshold - max buffers threshold per 
 *        packet
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_dynamic_configuration_set(
    int unit,
    uint8 enable_data_path,
    uint32 max_buffers_threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DYNAMIC_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DATA_PATH, INST_SINGLE, enable_data_path);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_MAX_BUFFERS_THRESHOLD, INST_SINGLE, max_buffers_threshold);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_spb_context_mru_default_get_f(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MAX_SIZEf, 0x2fff);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MAX_ORG_SIZEf,
                        dnx_data_ingr_congestion.info.max_jumbo_packet_size_get(unit));
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MIN_SIZEf, 0x20);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, entry_data, MIN_ORG_SIZEf, 0x20);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_pdm_fpc_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PDM_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PDM_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PDM_INIT, entry_handle_id));
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_FPC_INIT_STATUS, 0));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PDM_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_ALLOCATE_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_ocbm_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OCBM_GLOBAL_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OCBM_DISABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OCBM_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_drop_convert_priority_to_dbal(
    int unit,
    uint32 priority,
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e * dbal_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (priority)
    {
        case 0:
            *dbal_priority = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH;
            break;
        case 1:
            *dbal_priority = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID;
            break;
        case 2:
            *dbal_priority = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal priority value %d\n", priority);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_spb_drop_convert_priority_from_dbal(
    int unit,
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e dbal_priority,
    uint32 *priority)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_priority)
    {
        case DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH:
            *priority = 0;
            break;
        case DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID:
            *priority = 1;
            break;
        case DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW:
            *priority = 2;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal priority value %d\n", dbal_priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - disable one of the SRAM logical banks
 *
 * \param [in] unit - unit number
 * \param [in] core - core index
 * \param [in] ring - ring in core index
 * \param [in] bank - bank in ring index
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_spb_bank_disable(
    int unit,
    int core,
    int ring,
    int bank)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_PER_BANK_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_RING_ID, ring);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_BANK_ID, bank);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_CFG_START_POINTER, INST_SINGLE, 1);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_CFG_LAST_POINTER, INST_SINGLE, 0);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset FBM
 *
 * \param [in] unit - unit number
 * \param [in] core - core ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_fbm_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_BANKS_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FBM_INIT, INST_SINGLE, 1);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Setting Value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FBM_INIT, INST_SINGLE, 0);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Clearing the handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_BANKS_CFG, entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Polling to confirm reset completed */
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_FBM_INIT_STAT, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - decode bank status info from OTP, mark defective banks in bitmap and count them.
 *
 * \param [in] unit - unit number
 * \param [in] defective_banks - array with a list of defective banks per ring and core
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Function assumes the chip has a legal amount of defective banks (one per ring at most)
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_defective_banks_disable(
    int unit,
    int defective_banks[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_SPB_OCB_NOF_RINGS])
{
    int core;
    int ring;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (ring = 0; ring < dnx_data_spb.ocb.nof_rings_get(unit); ring++)
        {
            SHR_IF_ERR_EXIT(dnx_spb_bank_disable(unit, core, ring, defective_banks[core][ring]));
        }

        SHR_IF_ERR_EXIT(dnx_spb_fbm_reset(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the thresholds determining when minimal free buffers before bank is considered full
 *
 * \param [in]  unit - unit number
 * \param [out] bank_full_thresholds - current configuration of the number of
 *                                        free buffers, under which bank is considered full
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_spb_bank_full_thresholds_get(
    int unit,
    uint32 bank_full_thresholds[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES])
{
    uint32 entry_handle_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_BANKS_CFG, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        /** Setting pointer value to receive the field */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BANKS_FULL_TH, 0, &bank_full_thresholds[core]);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  set the thresholds determining when minimal free buffers before bank is considered full
 *
 * \param [in]  unit - unit number
 * \param [in]  core - core number
 * \param [in]  bank_full_threshold - value to give to bank full threshold
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_spb_bank_full_threshold_set(
    int unit,
    int core,
    uint32 bank_full_threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_BANKS_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANKS_FULL_TH, INST_SINGLE, bank_full_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#ifdef BANK_REDUNDANCY_VERIFICATION

/**
 * \brief - get the number of enabled buffers in a bank
 *
 * \param [in]  unit - unit number
 * \param [in]  core - core number
 * \param [in]  ring - ring in core number
 * \param [in]  bank - bank in ring number
 * \param [out] buffers_in_bank - how many buffers are in the bank
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_nof_buffers_in_bank_get(
    int unit,
    int core,
    int ring,
    int bank,
    uint32 *buffers_in_bank)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_FREE_BUFFERS_BANK_STATUS_REGISTER,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_RING_ID, ring);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_BANK_ID, bank);

    /** Setting pointer value to receive the field */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BANK_DBG_PTR_STAT, INST_SINGLE, buffers_in_bank);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the number of enabled buffers in a ring from status register
 *
 * \param [in]  unit - unit number
 * \param [in]  core - core number
 * \param [in]  ring - ring in core number
 * \param [out] buffers_in_ring - how many buffers are in the ring (sum of buffers in ring's banks)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_spb_nof_buffers_in_ring_get(
    int unit,
    int core,
    int ring,
    uint32 *buffers_in_ring)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      DBAL_TABLE_SRAM_PACKET_BUFFER_OCBM_FREE_BUFFERS_STATUS_REGISTER,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_RING_ID, ring);

    /** Setting pointer value to receive the field */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DBG_PTR_STAT, INST_SINGLE, buffers_in_ring);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify by reading status registers that defective banks were disabled
 *
 * \param [in] unit - unit number
 * \param [in] defective_banks - array with a list of defective banks per ring and core
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_spb_post_disable_nof_free_bufferes_verify(
    int unit,
    int defective_banks[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_SPB_OCB_NOF_RINGS])
{
    int is_unexpected_buffers_per_bank_val = FALSE;
    int is_unexpected_buffers_per_ring_val = FALSE;
    uint32 free_buffers_count;
    int core;
    int ring;

    SHR_FUNC_INIT_VARS(unit);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (ring = 0; ring < dnx_data_spb.ocb.nof_rings_get(unit); ring++)
        {
            SHR_IF_ERR_EXIT(dnx_spb_nof_buffers_in_bank_get(unit,
                                                            core,
                                                            ring, defective_banks[core][ring], &free_buffers_count));

            if (free_buffers_count > 0)
            {
                is_unexpected_buffers_per_bank_val = TRUE;
            }

            SHR_IF_ERR_EXIT(dnx_spb_nof_buffers_in_ring_get(unit, core, ring, &free_buffers_count));

            if (free_buffers_count != EXPECTED_PTR_COUNT_AFTER_BANK_DISABLE(unit))
            {
                is_unexpected_buffers_per_ring_val = TRUE;
            }
        }
    }

    if (is_unexpected_buffers_per_bank_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected number of buffers in a bank that should be disabled \n");
    }

    if (is_unexpected_buffers_per_ring_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected number of buffers in a ring with a bank that should be disabled \n");
    }

exit:
    SHR_FUNC_EXIT;
}

#endif

/**
 * \brief - Check OTP for defective memory indication, and disable logical bank if required
 *
 * \param [in] unit - unit number
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Function assumes the chip has a legal amount of defective banks (one per ring at most)
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_spb_defective_banks_get_and_disable(
    int unit)
{
    int defective_banks[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_SPB_OCB_NOF_RINGS];
    uint32 bank_full_threshold_to_disable_banks = 7;
    uint32 bank_full_thresholds[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    int core;
    int ring;

    SHR_FUNC_INIT_VARS(unit);

    /** initialize with 0's, so if there is no defective banks in ring - bank 0 will be disabled */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (ring = 0; ring < DNX_DATA_MAX_SPB_OCB_NOF_RINGS; ring++)
        {
            defective_banks[core][ring] = 0;
        }
    }

    SHR_IF_ERR_EXIT(dnx_spb_otp_defective_banks_info_read(unit, defective_banks));

    /** get current bank full thresholds */
    SHR_IF_ERR_EXIT(dnx_spb_bank_full_thresholds_get(unit, bank_full_thresholds));

    /** set thresholds to value needed to disable bank */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_spb_bank_full_threshold_set(unit, core, bank_full_threshold_to_disable_banks));
    }

    /** disable banks and verify they were disabled */
    SHR_IF_ERR_EXIT(dnx_spb_defective_banks_disable(unit, defective_banks));

#ifdef BANK_REDUNDANCY_VERIFICATION
    SHR_IF_ERR_EXIT(dnx_spb_post_disable_nof_free_bufferes_verify(unit, defective_banks));
#endif

    /** restore original bank full thresholds */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_spb_bank_full_threshold_set(unit, core, bank_full_thresholds[core]));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_threshold_set(
    int unit,
    int priority,
    uint32 value)
{

    uint32 entry_handle_id;
    dbal_fields_e dbal_priority[] =
        { DBAL_FIELD_THRESHOLD_HIGH_PRIORITY, DBAL_FIELD_THRESHOLD_MID_PRIORITY, DBAL_FIELD_THRESHOLD_LOW_PRIORITY };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_THRESHOLDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_priority[priority], INST_SINGLE, value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_threshold_get(
    int unit,
    int priority,
    uint32 *value)
{

    uint32 entry_handle_id;
    dbal_fields_e dbal_priority[] =
        { DBAL_FIELD_THRESHOLD_HIGH_PRIORITY, DBAL_FIELD_THRESHOLD_MID_PRIORITY, DBAL_FIELD_THRESHOLD_LOW_PRIORITY };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_THRESHOLDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, dbal_priority[priority], INST_SINGLE, value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_reassembly_context_profile_map_set(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_CONTEXT_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_context);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_reassembly_context_profile_map_get(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 *profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_CONTEXT_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_context);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_profile_priority_map_set(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 priority)
{
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e dbal_priority;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_spb_drop_convert_priority_to_dbal(unit, priority, &dbal_priority));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_PRIORITY_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_PACKET_BUFFER_DROP_PRIORITY, INST_SINGLE,
                                 (uint32) dbal_priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_spb_drop_profile_priority_map_get(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 *priority)
{
    dbal_enum_value_field_sram_packet_buffer_drop_priority_e dbal_priority;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_PRIORITY_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_PACKET_BUFFER_DROP_PRIORITY, INST_SINGLE,
                               (uint32 *) &dbal_priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_spb_drop_convert_priority_from_dbal(unit, dbal_priority, priority));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - init spb drop threshold values
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_drop_thresholds_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_DROP_THRESHOLDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_HIGH_PRIORITY, INST_SINGLE,
                                 dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get(unit, 0)->value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_MID_PRIORITY, INST_SINGLE,
                                 dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get(unit, 1)->value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_LOW_PRIORITY, INST_SINGLE,
                                 dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get(unit, 2)->value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - init OCB last in chain
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_ocb_last_in_chain_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 ocb_index;
    uint32 nof_ocb_blocks = dnx_data_spb.ocb.nof_blocks_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRAM_PACKET_BUFFER_OCB_LAST_IN_CHAIN, &entry_handle_id));

    for (ocb_index = 0; ocb_index < nof_ocb_blocks; ++ocb_index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCB_INDEX, ocb_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LAST_IN_CHAIN, INST_SINGLE,
                                     dnx_data_spb.ocb.last_in_chain_get(unit, ocb_index)->is_last_in_chain);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_spb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_spb_dynamic_configuration_set(unit, 1, 48));
    SHR_IF_ERR_EXIT(dnx_spb_pdm_fpc_init(unit));
    SHR_IF_ERR_EXIT(dnx_spb_drop_thresholds_init(unit));

    if (dnx_data_spb.ocb.feature_get(unit, dnx_data_spb_ocb_dbg_disable_enable))
    {
        SHR_IF_ERR_EXIT(dnx_spb_ocbm_init(unit));
    }

    if (dnx_data_spb.ocb.feature_get(unit, dnx_data_spb_ocb_blocks_in_ring))
    {
        SHR_IF_ERR_EXIT(dnx_spb_ocb_last_in_chain_init(unit));
    }

    if (dnx_data_spb.ocb.feature_get(unit, dnx_data_spb_ocb_is_bank_redundancy_supported) && !SAL_BOOT_PLISIM)
    {
        SHR_IF_ERR_EXIT(dnx_spb_defective_banks_get_and_disable(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_spb_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}
