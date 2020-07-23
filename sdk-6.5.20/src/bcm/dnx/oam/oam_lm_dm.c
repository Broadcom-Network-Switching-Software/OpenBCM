
/** \file oam_lm_dm.c
 * 
 *
 * OAM LM DM procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

#include <bcm/oam.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>

#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <src/bcm/dnx/oam/oamp_pe_infra.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_bfd_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * Check if endpoint is unaccelerated
 */
#define DNX_OAM_MEP_IS_UNACCELERATED(mep_id) (mep_id & ((7 << DNX_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT) | \
                                                        (1 << DNX_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT) | \
                                                        (1 << DNX_OAM_DOWN_MEP_LEVEL_0_NON_ACC_BIT)))

/*
 * Check if endpoint between 0-3th bank, return 0 if not
 */
#define DNX_OAM_MEP_IS_BETWEEN_4_7_BANK(mep_id) (mep_id & 0x8000)

/*
 * Check if OAMP_MEP_DB.MEPTYPE is of type MPLS TP or PWE.
 */
#define DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(mep_type) (mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP ||\
                                                      mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE || \
                                                      mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)

/*
 * Given the DM MEP_DB entry info of type dnx_oam_oamp_endpoint_dm_params_t
 * and info whether it is an offloaded or self contained endpoint,
 * get the DM_STAT/OFFLOADED entry part 2 index.
 */
#define DNX_OAM_GET_DELAY_ENTRY_CONFIGURED(offloaded,dm_mep_db_entry,delay_entry_ptr)\
do\
{\
    if (offloaded) {\
        /* For offloaded, flex_lm_dm_ptr gives the part2_ptr.\
         */\
        delay_entry_ptr = dm_mep_db_entry->flex_dm_entry;\
    } else {\
        /* For selfcontained, it is given by dm_stat_entry_idx */\
        delay_entry_ptr = dm_mep_db_entry->dm_stat_entry_idx;\
    }\
}while(0)

/*
 * Given the LM MEP_DB entry info of type dnx_oam_oamp_endpoint_lm_params_t
 * and info whether it is an offloaded or self contained endpoint,
 * get the LM_DB/OFFLOADED entry part2 index.
 */
#define DNX_OAM_GET_LOSS_ENTRY_CONFIGURED(offloaded,lm_mep_db_entry,lm_db_entry_ptr)\
do\
{\
    if (offloaded) {\
        /* For offloaded, flex_lm_dm_ptr gives the part2_ptr.\
         */\
        lm_db_entry_ptr = lm_mep_db_entry->flex_lm_entry;\
    } else {\
        /* For selfcontained, it is given by lm_db_entry_idx */\
        lm_db_entry_ptr = lm_mep_db_entry->lm_db_entry_idx;\
    }\
}while(0)

/*
 * Given the LM MEP_DB entry info of type dnx_oam_oamp_endpoint_lm_params_t
 * for a self contained endpoint, get the LM_STAT entry part2 index.
 */
#define DNX_OAM_GET_LOSS_STAT_ENTRY_CONFIGURED(lm_mep_db_entry,lm_stat_entry_ptr)\
do\
{\
    /* For selfcontained, it is given by lm_stat_entry_idx */\
    lm_stat_entry_ptr = lm_mep_db_entry->lm_stat_entry_idx;\
}while(0)

/* Get priority value from bitmap */
#define DNX_OAM_GET_PRIORITY_FROM_BITMAP(bitmap,priority) \
    switch (bitmap) \
    { \
        case 0: \
        case 255: \
        case 1: \
            priority = 0; \
            break; \
        case 2: \
            priority = 1; \
            break; \
        case 4: \
            priority = 2; \
            break; \
        case 8: \
            priority = 3; \
            break; \
        case 16: \
            priority = 4; \
            break; \
        case 32: \
            priority = 5; \
            break; \
        case 64: \
            priority = 6; \
            break; \
        case 128: \
            priority = 7; \
            break; \
        default:\
            SHR_ERR_EXIT(_SHR_E_PARAM,\
                         "Priority may be defined for one priority at a time. Please consider multiple bcm_oam_delay_add");\
    }

/** Flags used to indicate if oam oamp profiles should be updated */
#define DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE             (1 << 0)
#define DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE             (1 << 1)
#define DNX_OAM_LM_DM_OAMP_MEP_PROFILE               (1 << 2)
/** Indicates if LM_STAT entry exists */
#define DNX_OAM_LM_DM_LM_STAT_EXISTS                 (1 << 3)
/** Indicates if DM_STAT entry exists */
#define DNX_OAM_LM_DM_DM_STAT_EXISTS                 (1 << 4)

/* HW write data for loss/delay configuration */
typedef struct _dnx_oam_loss_delay_oamp_hw_write_data_s
{
    /*
     * Flags indicating which profiles should be updated in HW
     */
    uint32 flags;

    /*
     * The OUI profile
     */
    uint8 oamp_da_msb_prof;
    /*
     * The OUI (MSB) value
     */
    bcm_mac_t oamp_mac_da_msb;

    /*
     * The NIC profile
     */
    uint8 oamp_da_lsb_prof;
    /*
     * The NIC (LSB) value
     */
    bcm_mac_t oamp_mac_da_lsb;

    /*
     * The MEP profile
     */
    uint8 oamp_mep_profile;
    /*
     * The MEP profile data
     */
    dnx_oam_mep_profile_t mep_profile_data;

    /*
     * CCM MEP DB index
     */
    uint16 ccm_oam_id;
    /*
     * Additional statistics enabled ?
     */
    uint8 stat_enable;
    /*
     * Is it offloaded endpoint ?
     */
    uint8 offloaded;
    /*
     * Part 2 entry in case of offloaded lm_db/dm_stat entry in case of self_contained
     */
    uint16 flex_lm_dm_entry;
    /*
     * Update mep db
     */
    uint8 update_mep_db;
    /*
     * Measurement period indication and configuration
     */
    uint8 measurement_period;
    uint8 measurement_period_write_index;
    int measurement_period_write_value;
    uint32 slm_counter_base_id;
    uint32 slm_counter_core_id;
    /*
     * 1DM indication
     */
    uint8 is_1dm;
    /*
     * jumbo tlv dm indication
     */
    uint8 is_jumbo_dm;
    uint8 is_48b_group;
    /*
     * OAMP MEP PE profile
     */
    uint8 mep_pe_profile;
} dnx_oam_loss_delay_oamp_hw_write_data_t;

/**
 * \brief - Calculae the LM/DM stamping offset.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_db_entry     - information about the mep db entry
 * \param [in] is_piggy_back    - Indicate for ccm pg-back entry
 * \param [in] is_dm            - Indicate for LM = 0, DM = 1
 * \param [in] is_reply         - Indicate for response
 * \param [in] is_rfc_6374_lsp  - Indicates for LSP RFC6374 entry
 * \param [in] is_egress_inj    - Indicates for egress injection
 *
 * \return
 *   offset value
 *
 * \see
 *   * None
 */
int
dnx_oam_lm_dm_set_offset(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 is_piggy_back,
    uint8 is_dm,
    uint8 is_reply,
    uint8 is_rfc_6374_lsp,
    uint8 is_egress_inj)
{
    int value = 0;
    uint8 is_rfc_6374 = mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374 ? 1 : 0;
    int is_jr2_mode = 0;

    is_jr2_mode =
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ? 1 : 0;

    /**
    * Only one of these could be active at a time
    *   For piggy backed CCMs an extra 54 bytes are added.
    *       * 4B (sequence #) + 2B (MEP-ID) + 48B (MEG-ID)
    *   RFC-6374 LM an extra 24 bytes are added for OAM PDU.
    *   RFC-6374 DM an extra 20 bytes are added for OAM PDU.
    *
    * In addition extra 4 bytes are added for LSP RFC 6374 MEP
    */
    value += (is_piggy_back * 54) + (is_rfc_6374 * (24 - (is_dm * 4))) + (is_rfc_6374_lsp * 4);

    /** Offsets are calculated in the following way*/
    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        /**
         * ETH: DA, SA addresss (12B) + num-tags (taken from mep db) * 4B
         * Ethertype (2B) + offset in OAM PDU (4B)
         * 
         * For down egress injection with PPH in jr2 mode, ptch+itmh+itmh_base+ASE+PPH+PPH_FHEI_5B will be removed, bytes to removed is 0x21
         */
        value += mep_db_entry->nof_vlan_tags * 4 + 18 + 0x21 * is_egress_inj * is_jr2_mode;
    }
    else if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP)
    {
        /**
         * MPLS: GAL, GACH, MPLS/PWE (4B each), offset in OAM PDU * (4B) 
         * 
         * For oam over mpls in jr2 mode, ptch+itmh+itmh_base+ASE+PPH will be removed, bytes to removed is 0x1c
         * offset field equal to the actual offset + the projected bytes_to_remove
         */
        value += (16 + 0x1c * is_jr2_mode);
    }
    else if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)
    {
        value += (12 + 0x1c * is_jr2_mode);
        /** RFC6374 PWE/Section: GAL/PWE, GACH (4B each) + value (calculated with the RFC OAM PDU) */
    }
    else if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)
    {
        value += 0x8;
    }

    if (is_rfc_6374)
    {
        /** The offsets for the Query and Response are the same*/
        value += 0x1c * is_jr2_mode;
        return value;
    }

    if (is_reply & is_dm)
    {
        /** DMR case */
        value += 0x10;
    }
    else if (is_reply)
    {
        /*
         * For SLR the stamping is done 4 bytes after the LM stamping, for LMR 8.
         */
        value += 8;
    }

    return value;
}

/**
 * \brief - Verify the MEP DB index (Loss_ID/Delay_ID)
 *          in the loss/delay information structure.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_delay_ptr - Pointer to the structure,
 *                 containing information about the loss/delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_0_dm_1 - Indicate caller LM = 0, DM = 1
 * \param [in] other_exists - Indicates other entry exists.
 * \param [in] entry_idx - Other entry's ID.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_lm_dm_add_mep_db_entry_verify(
    int unit,
    const void *loss_delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_0_dm_1,
    uint8 other_exists,
    uint32 entry_idx)
{
    uint32 flags = 0, with_id_flag = 0;
    uint32 mep_db_idx = 0;
    bcm_oam_loss_t *loss_ptr = NULL;
    bcm_oam_delay_t *delay_ptr = NULL;
    char entry_type_str[6] = { 0 };
    char other_entry_type_str[6] = { 0 };
    uint8 update = 0, offloaded = 0;
    uint32 mep_db_threshold = 0;
    dnx_oam_oamp_mep_db_args_t mep_db_entry;
    dbal_enum_value_field_oamp_mep_type_e entry_type, exp_entry_type;
    uint32 pm_id = 0;
    uint32 oam_id = 0;
    uint8 is_with_priority = 0;
    uint8 is_rfc_6374 = 0;
    SHR_FUNC_INIT_VARS(unit);

    is_rfc_6374 = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_MEP_IS_RF6374) ? 1 : 0;
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    is_with_priority = 0;
    if (!lm_0_dm_1)
    {
        /*
         * Loss case
         */
        loss_ptr = (bcm_oam_loss_t *) loss_delay_ptr;
        flags = loss_ptr->flags;
        with_id_flag = BCM_OAM_LOSS_WITH_ID;
        mep_db_idx = loss_ptr->loss_id;
        oam_id = loss_ptr->id;
        sal_strncpy_s(entry_type_str, "loss", sizeof(entry_type_str));
        sal_strncpy_s(other_entry_type_str, "delay", sizeof(other_entry_type_str));
        update = (loss_ptr->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;
        pm_id = loss_ptr->pm_id;
        /*
         * Expected entry type to check in case of update
         */
        if (offloaded)
        {
            exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;
        }
        else
        {
            exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
        }
        if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
        {
            is_with_priority = 1;
        }
    }
    else
    {
        /*
         * Delay case
         */
        delay_ptr = (bcm_oam_delay_t *) loss_delay_ptr;
        flags = delay_ptr->flags;
        with_id_flag = BCM_OAM_DELAY_WITH_ID;
        mep_db_idx = delay_ptr->delay_id;
        oam_id = delay_ptr->id;
        sal_strncpy_s(entry_type_str, "delay", sizeof(entry_type_str));
        sal_strncpy_s(other_entry_type_str, "loss", sizeof(other_entry_type_str));
        update = (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) ? 1 : 0;
        pm_id = delay_ptr->pm_id;
        /*
         * Expected entry type to check in case of update
         */
        if (offloaded)
        {
            exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;
        }
        else
        {
            if (flags & BCM_OAM_DELAY_ONE_WAY)
            {
                exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            }
            else
            {
                exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            }
        }
        if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
        {
            is_with_priority = 1;
        }
    }

    /*
     * Verify if the loss_id or delay_id given is above threshold.
     * If other entry exists, the ID should be 0
     */
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (!other_exists && mep_db_idx <= mep_db_threshold)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %s ID needs to be above mep_db_threshold.\n", entry_type_str);
    }

    /*
     * Verify if loss_id or delay_id is required or not
     */
    if (!other_exists || update)
    {
        /*
         * Self contained: User needs to provide LM/DM ID if there is no other LM/DM entry.
         */
        /*
         * Offloaded LM/DM: User needs to provide Part 2 LM/DM ID if there is no other LM/DM entry.
         */
        if (!(flags & with_id_flag) || !(mep_db_idx))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %s with valid ID is mandatory.\n", entry_type_str);
        }
    }
    else
    {
        /*
         * Self contained: If there is other entry, we will figure out the rest of the entries from there. User
         * may not provide the ID.
         */
        /*
         * Offloaded: If there is other entry, we dont need to alloc any entry. User may not provide the ID.
         */
        if (((flags & with_id_flag) || (mep_db_idx)) && !is_with_priority)
        {
            if (mep_db_idx != entry_idx)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %s already exists with different ID.\n", other_entry_type_str);
            }
        }
    }

    if (update)
    {
        /*
         * Read the mep db entry to see if the correct LM/DM type is configured. If not throw an error
         */
        sal_memset(&mep_db_entry, 0, sizeof(mep_db_entry));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, mep_db_idx, &mep_db_entry));
        entry_type = mep_db_entry.mep_type;
        if (entry_type != exp_entry_type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Entry type mismatch.\n");
        }
    }

    /*
     * If ID for addition ccm entry for loss/delay with priority is given by pm_id
     * check that it doesn't conflict with existing ccm entry's ID
     */
    if (pm_id != 0)
    {
        if (is_with_priority)
        {
            if (other_exists && !is_rfc_6374)
            {
                if (pm_id != oam_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error: %s entry with different ID already exists on this endpoint and priority.\n",
                                 other_entry_type_str);
                }
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: pm_id field may be used for %s with priority only.\n",
                         other_entry_type_str);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the conflict between the new flexible DMAC
 *          and the existing flexible DMAC.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flexible_dmac - new flexible DMAC.
 * \param [in] extra_data_index - Pointer to extra data where the
 *                                existing flexible DMAC is stored
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_lmdm_flexible_dmac_conflict_verify(
    int unit,
    bcm_mac_t flexible_dmac,
    uint16 extra_data_index)
{
    dnx_oamp_ccm_endpoint_extra_data_t extra_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
    extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(extra_data_index);
    SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_get(unit, &extra_data));

    /*
     * Compare with the existing LM/DM flexible DMAC
     */
    if (sal_memcmp(flexible_dmac, extra_data.dmac, sizeof(bcm_mac_t)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LM/DM flexible DMAC conflict is not allowed.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear the TX rate profile for MplsLmDm for the HW
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] tx_rate_profile - profile index
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_lm_dm_tx_rate_clear(
    int unit,
    int tx_rate_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_TX_RATES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, tx_rate_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_VALUE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the loss information structure
 *          for correct parameters.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] dm_exists - Indicates whether DM entry exists.
 * \param [in] dm_entry_idx - Other DM entry's ID.
 * \param [out] priority - Priority in case of LM with priority
 *        is being used
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_loss_add_verify(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 dm_exists,
    uint32 dm_entry_idx,
    int *priority)
{
    uint32 correct_flags = 0;
    uint32 is_mesure_next_slr_enabled = 0;
    uint8 mpls_pwe_mep = 0;
    uint8 nof_cores = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    /*
     * Check if loss add is supported
     */
    if (dnx_data_oam.oamp.lmm_dmm_supported_get(unit) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Loss add is not supported on this device.");
    }

    if (((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0) && ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == 0)
        && ((ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_IS_UPMEP) == 0) && (ccm_mep_db_entry->nof_vlan_tags == 2)
        && ((ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: Down-MEP injection of double-tagged piggy-back CCMs is not "
                     "supported on this device. DOWN_MEP_EGRESS_INJECTION flag must be set.");
    }

    /*
     * Loss add is not supported on quarter entry endpoints
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Invalid memory type.\n");

    }

    /*
     * Validate that the correct number of cores are provided for slm_counter_core_id
     */
    if (loss_ptr->slm_counter_core_id > nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: slm_counter_core_id should be below or equal to device number of cores.\n");
    }

    /*
     * Loss entries are not supported on quarter entry and selfcontained entry endpoints when measure next SLR is enabled
     */
    SHR_IF_ERR_EXIT(dnx_oam_enable_next_received_slr_get(unit, &is_mesure_next_slr_enabled));
    if (!(ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) && is_mesure_next_slr_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Loss entries are not supported on selfcontained entry endpoints when measure next SLR is enabled\n");

    }

    /*
     * Flags verification
     */
    correct_flags |= BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED |
        BCM_OAM_LOSS_WITH_ID | BCM_OAM_LOSS_UPDATE | BCM_OAM_LOSS_REPORT_MODE | BCM_OAM_LOSS_SLM;

    if (loss_ptr->flags & ~correct_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal flags set\n");
    }

    if ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) && (loss_ptr->flags & BCM_OAM_LOSS_SLM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: Invalid flags combination. When BCM_OAM_LOSS_SLM is set, BCM_OAM_LOSS_SINGLE_ENDED should not be set\n");
    }

    if (loss_ptr->loss_farend || loss_ptr->loss_nearend || loss_ptr->gport || loss_ptr->int_pri
        || loss_ptr->loss_threshold || loss_ptr->pkt_dp_bitmap || loss_ptr->pkt_pri || loss_ptr->rx_farend
        || loss_ptr->rx_nearend || loss_ptr->rx_oam_packets || loss_ptr->tx_farend || loss_ptr->tx_nearend
        || loss_ptr->tx_oam_packets)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal field set.\n");
    }

    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * Piggy back cases and MPLS/PWE endpoints - DA needs to be zero
     */
    if ((((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0) && ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == 0))
        || (mpls_pwe_mep))
    {
        if (!BCM_MAC_IS_ZERO(loss_ptr->peer_da_mac_address))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: DA MAC needs to be zero.\n");
        }
    }

    DNX_OAM_GET_PRIORITY_FROM_BITMAP(loss_ptr->pkt_pri_bitmap, *priority)
        if (*priority && !(loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED))
    {
        if (!(loss_ptr->flags & BCM_OAM_LOSS_SLM))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Loss with priority may not be defined CCM piggy-back entries");
        }
    }

    /*
     * Call mep db index verify function
     */
    SHR_IF_ERR_EXIT(dnx_oam_lm_dm_add_mep_db_entry_verify
                    (unit, loss_ptr, ccm_mep_db_entry, 0 /* LM */ , dm_exists, dm_entry_idx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Alloc/exchange a profile for the LMM/DMM DA NIC.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_mac_da_lsb - LSB part of MAC DA
 * \param [in] old_oamp_mac_da_lsb - LSB part of existing MAC DA
 * \param [in] old_oamp_da_lsb_prof - The existing MAC DA LSB profile
 * \param [in] exchange - Whether profile needs to be exchanged/allocated
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LMM DMM HW data
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mac_da_lsb_profile_alloc(
    int unit,
    bcm_mac_t oamp_mac_da_lsb,
    bcm_mac_t old_oamp_mac_da_lsb,
    uint8 old_oamp_da_lsb_prof,
    uint8 exchange,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int oamp_da_lsb_prof = 0;
    uint8 write_hw = 0, delete_old_prof = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (exchange)
    {
        /*
         * If the LSB is different, update the profile
         */
        if (sal_memcmp(old_oamp_mac_da_lsb, oamp_mac_da_lsb, sizeof(bcm_mac_t)))
        {
            /*
             * Exchange the old profile with new profile.
             */
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_da_mac_lsb_profile_template.exchange(unit, 0 /* No flags */ , oamp_mac_da_lsb,
                                                                     old_oamp_da_lsb_prof, NULL /* No extra args */ ,
                                                                     &(oamp_da_lsb_prof), &write_hw, &delete_old_prof));
        }
        else
        {
            oamp_da_lsb_prof = old_oamp_da_lsb_prof;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_oam_db.
                        oam_da_mac_lsb_profile_template.allocate_single(unit, 0 /* No flags */ , oamp_mac_da_lsb,
                                                                        NULL /* No extra args */ ,
                                                                        &(oamp_da_lsb_prof), &write_hw));
        delete_old_prof = 0;
    }

    hw_write_data->oamp_da_lsb_prof = oamp_da_lsb_prof;
    sal_memcpy(hw_write_data->oamp_mac_da_lsb, oamp_mac_da_lsb, sizeof(bcm_mac_t));
    if (write_hw)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Alloc/exchange a profile for the LMM/DMM DA OUI.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_mac_da_msb - MSB part of MAC DA
 * \param [in] old_oamp_mac_da_msb - MSB part of existing MAC DA
 * \param [in] old_oamp_da_msb_prof - The existing MAC DA MSB profile
 * \param [in] exchange - Whether profile needs to be exchanged/allocated
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LMM DMM HW data
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mac_da_msb_profile_alloc(
    int unit,
    bcm_mac_t oamp_mac_da_msb,
    bcm_mac_t old_oamp_mac_da_msb,
    uint8 old_oamp_da_msb_prof,
    uint8 exchange,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int oamp_da_msb_prof = 0;
    uint8 write_hw = 0, delete_old_prof = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (exchange)
    {
        /*
         * If the MSB is different, update the profile
         */
        if (sal_memcmp(old_oamp_mac_da_msb, oamp_mac_da_msb, sizeof(bcm_mac_t)))
        {
            /*
             * Exchange the old profile with new profile.
             */
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_da_mac_msb_profile_template.exchange(unit, 0 /* No flags */ , oamp_mac_da_msb,
                                                                     old_oamp_da_msb_prof, NULL /* No extra args */ ,
                                                                     &(oamp_da_msb_prof), &write_hw, &delete_old_prof));
        }
        else
        {
            oamp_da_msb_prof = old_oamp_da_msb_prof;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_oam_db.
                        oam_da_mac_msb_profile_template.allocate_single(unit, 0 /* No flags */ , oamp_mac_da_msb,
                                                                        NULL /* No extra args */ ,
                                                                        &(oamp_da_msb_prof), &write_hw));
        delete_old_prof = 0;
    }

    hw_write_data->oamp_da_msb_prof = oamp_da_msb_prof;
    sal_memcpy(hw_write_data->oamp_mac_da_msb, oamp_mac_da_msb, sizeof(bcm_mac_t));
    if (write_hw)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function allocate slm_measurement_period profile in SW and prepare the
 *          needed information for HW data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - see definition of struct for
 *          description of fields
 * \param [in] mep_profile_data - Pointer to the structure,
 *          containing information about the
 *          OAMP MEP PROFILE content.
 * \param [out] hw_write_data - Pointer to struct where data
 *          for writing to HW tables is placed.  This data is
 *          relevant to fields in the OAMP_TXM_SLM_MEASUREMENT_PERIOD_TIMER register and OAMP_MEP_PROFILE.
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
dnx_oam_oamp_slm_measurement_profile_sw_update(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    dnx_oam_mep_profile_t * mep_profile_data,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    shr_error_e rv;
    uint16 slm_measurement_hw_number_of_scans;
    int measurement_profile_index;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    if (loss_ptr->measurement_period == 0)
    {
        slm_measurement_hw_number_of_scans = 0;
    }
    else
    {
        /*
         * Convert the period from miliseconds to number of HW scans
         */
        slm_measurement_hw_number_of_scans =
            BCM_OAMP_SW_ENTRY_INFO_PERIOD_TO_MEP_DB_SCAN_RATE(loss_ptr->measurement_period);
    }

    /*
     * Store the calculated value in the SW.
     */
    rv = algo_oam_db.slm_oamp_measurement_period.allocate_single
        (unit, 0, (uint32 *) &slm_measurement_hw_number_of_scans, NULL, &measurement_profile_index, &write_hw);
    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free measurement period entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }
    /** Update the SW profile data */
    mep_profile_data->slm_measurement_period_index = measurement_profile_index;
    hw_write_data->measurement_period = write_hw;
    /** This will definitely be written to the OAMP_MEP_PROFILE */
    hw_write_data->mep_profile_data.slm_measurement_period_index = measurement_profile_index;
    /** This will definitely be written to the OAMP_MEP_DB */
    hw_write_data->slm_counter_base_id = loss_ptr->slm_counter_base_id;
    hw_write_data->slm_counter_core_id = loss_ptr->slm_counter_core_id;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end in OAMP_TXM_SLM_MEASUREMENT_PERIOD_TIMER register */
        hw_write_data->measurement_period_write_index = measurement_profile_index;
        hw_write_data->measurement_period_write_value = slm_measurement_hw_number_of_scans;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function calculate and allocate TX profile based on LM period.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] period - Lm/Dm period in miliseconds
 * \param [in] is_update - if set update the entry
 * \param [in] is_dm - used to indicate if update is for dm or lm entry
 * \param [in] mep_profile_data - Pointer to the structure,
 *          containing information about the
 *          OAMP MEP PROFILE content.
 * \param [out] tx_rate_profile - allocated tx profile
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
dnx_oam_oamp_lmm_dmm_rate_to_rfc6374_rate(
    int unit,
    int period,
    uint8 is_update,
    uint8 is_dm,
    const dnx_oam_mep_profile_t * mep_profile_data,
    uint32 *tx_rate_profile)
{
    shr_error_e rv;
    int dummy, tx_rate_id;
    dnx_bfd_tx_period_t tx_rate_data, tx_rate_data_old;
    bfd_temp_oamp_mep_db_data_t *mep_hw_write_data = NULL;
    uint8 write_hw = FALSE, tx_rate_write_delete = FALSE;
    uint32 old_tx_profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(mep_hw_write_data, sizeof(bfd_temp_oamp_mep_db_data_t),
                       "Data saved to write to hard-logic tables", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    tx_rate_data.tx_period = 0;
    tx_rate_data.cluster_profile = 0xff;

    if (period == 0)
    {
        /**
         *  If the user sets period==0, the endpoint should not transmit packets.
         *  However, if BFD_TX_RATE_PROFILE points to a profile that has a 0 Tx interval,
         *  Packets from the endpoint are sent at the maximum rate.  To overcome this, scan
         *  profile is set to maximum value, and since count==MAX never happens at maximum rate, packets
         *  will not be transmitted.
         */
        tx_rate_data.tx_period = 0;
    }
    else
    {
        tx_rate_data.tx_period = BCM_OAMP_SW_ENTRY_INFO_PERIOD_TO_MEP_DB_SCAN_RATE(period);
    }

    if (is_update)
    {
        old_tx_profile = is_dm ? mep_profile_data->dmm_rate : mep_profile_data->lmm_rate;
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, old_tx_profile, &dummy, &tx_rate_data_old));

        if (tx_rate_data.tx_period != tx_rate_data_old.tx_period)
        {
            /** A new rate should be added */
            rv = algo_bfd_db.bfd_oamp_tx_rate.allocate_single(unit, NO_FLAGS, &tx_rate_data, NULL, &tx_rate_id,
                                                              &write_hw);
            if (rv == _SHR_E_RESOURCE)
            {
                /** Resource error means no match - display error message */
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free Tx rate entries available.\n");
            }
            else
            {
                /** All other errors handled generically */
                SHR_IF_ERR_EXIT(rv);
            }

            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.free_single(unit, old_tx_profile, &tx_rate_write_delete));

            if (tx_rate_write_delete)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_tx_rate_clear(unit, old_tx_profile));
            }
        }
        else
        {
            tx_rate_id = old_tx_profile;
        }

    }
    else
    {
        /** A new rate should be added */
        rv = algo_bfd_db.bfd_oamp_tx_rate.allocate_single(unit, NO_FLAGS, &tx_rate_data, NULL, &tx_rate_id, &write_hw);
        if (rv == _SHR_E_RESOURCE)
        {
            /** Resource error means no match - display error message */
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free Tx rate entries available.\n");
        }
        else
        {
            /** All other errors handled generically */
            SHR_IF_ERR_EXIT(rv);
        }
    }

    /** This will definitely be written to the MEP DB   */
    mep_hw_write_data->tx_rate_write_index = tx_rate_id;
    mep_hw_write_data->tx_rate_write_value = tx_rate_data.tx_period;
    *tx_rate_profile = tx_rate_id;
    if (write_hw == TRUE)
    {
        /** Write data to BFD Tx rate table   */
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(unit,
                                                                     mep_hw_write_data->tx_rate_write_index,
                                                                     mep_hw_write_data->tx_rate_write_value));
    }
exit:
    SHR_FREE(mep_hw_write_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill OAMP MEP profile data based on Loss information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 * \param [in] is_jumbo_tlv_config -  indicate jumbo tlv is config.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP PROFILE content.
 * \param [in] is_rfc_6374_lsp_type - Indication if the entry is RFC6374 over MPLS LSP.
 * \param [in] is_egress_inj    - Indicates for egress injection

 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_loss_info_to_mep_profile_data(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    uint8 is_jumbo_tlv_config,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data,
    uint8 is_rfc_6374_lsp_type,
    uint8 is_egress_inj)
{
    uint32 new_tx_rate = 0;
    uint8 is_mpls_lm_dm_without_bfd_endpoint;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Fill up with LM params
     */
    mep_profile_data->piggy_back_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0);
    if ((loss_ptr->flags & BCM_OAM_LOSS_SLM) != 0)
    {
        mep_profile_data->slm_lm = 1;
        mep_profile_data->piggy_back_lm = 0;
        mep_profile_data->slm_measurement_period_index = loss_ptr->measurement_period;
    }
    mep_profile_data->report_mode_lm = ((loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE) != 0);

    /*
     * Setting LMM/LMR offsets here since there could be change in piggyback and SLM
     */
    mep_profile_data->lmm_offset =
        dnx_oam_lm_dm_set_offset(unit, ccm_mep_db_entry, mep_profile_data->piggy_back_lm, 0 /** LM type */ ,
                                 0 /** LMM */ ,
                                 is_rfc_6374_lsp_type, is_egress_inj);
    mep_profile_data->lmr_offset =
        dnx_oam_lm_dm_set_offset(unit, ccm_mep_db_entry, mep_profile_data->piggy_back_lm, 0 /** LM type */ ,
                                 1 /** LMR */ ,
                                 is_rfc_6374_lsp_type, is_egress_inj);

    /*
     * Setting phase count for LMM
     */
    if (ccm_mep_db_entry->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        mep_profile_data->lmm_rate = loss_ptr->period;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, loss_ptr->id, is_jumbo_tlv_config, (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY), ccm_mep_db_entry->ccm_interval, 0  /** don't
                                                                                                                                                                                             * care
                                                                                                                                                                                             * for
                                                                                                                                                                                             * opcodes
                                                                                                                                                                                             * other
                                                                                                                                                                                             * than
                                                                                                                                                                                             * CCM */
                                                     , OAMP_MEP_TX_OPCODE_LMM, mep_profile_data));

    }
    else
    {
        /** Calculate the LM Tx rate for RFC6374 Endpoint. It's based on BFD TX rate mechanism */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_lmm_dmm_rate_to_rfc6374_rate
                        (unit, loss_ptr->period, _SHR_IS_FLAG_SET(loss_ptr->flags, BCM_OAM_LOSS_UPDATE),
                         0 /** Loss entry */ ,
                         mep_profile_data, &new_tx_rate));
        /** RFC6374 use profiles for store different TX Rates. The lmm_rate in oamp_mep_profile stores the pointer to the rate profile */
        mep_profile_data->lmm_rate = new_tx_rate;
        /** Check if there is no BFD endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, loss_ptr->id, &is_mpls_lm_dm_without_bfd_endpoint));
        /** In case of period==0, set the phase cnt to maximum value, in order to disable the rate. */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_rfc_6374_lm_dm
                        (unit, loss_ptr->period, OAMP_MEP_TX_OPCODE_LMM, mep_profile_data,
                         is_mpls_lm_dm_without_bfd_endpoint));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove loss information from OAMP MEP profile data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id  - Mep id
 * \param [in] is_jumbo_tlv_dm - indicate jumbo tlv is config.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP PROFILE content.
 * \param [in] is_rfc_6374_lsp_type - Indication if the entry is RFC6374 over MPLS LSP.
 * \param [in] is_egress_inj    - Indicates for egress injection
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_remove_loss_info_from_mep_profile_data(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data,
    uint8 is_rfc_6374_lsp_type,
    uint8 is_egress_inj)
{
    int dummy = 0;
    dnx_bfd_tx_period_t tx_rate_data;
    uint8 is_mpls_lm_dm_without_bfd_endpoint;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Remove the LM params
     */
    mep_profile_data->piggy_back_lm = 0;
    mep_profile_data->slm_lm = 0;
    mep_profile_data->report_mode_lm = 0;
    mep_profile_data->lmm_rate = 0;
    mep_profile_data->slm_measurement_period_index = 0;
    /*
     * Setting LMM/LMR offsets here since there is change in piggyback and SLM
     */
    mep_profile_data->lmm_offset =
        dnx_oam_lm_dm_set_offset(unit, ccm_mep_db_entry, mep_profile_data->piggy_back_lm, 0 /** LM type */ ,
                                 0 /** LMM */ ,
                                 is_rfc_6374_lsp_type, is_egress_inj);
    mep_profile_data->lmr_offset =
        dnx_oam_lm_dm_set_offset(unit, ccm_mep_db_entry, mep_profile_data->piggy_back_lm, 0 /** LM type */ ,
                                 1 /** LMR */ ,
                                 is_rfc_6374_lsp_type, is_egress_inj);
    /*
     * Setting phase count for LMM
     */
    if (ccm_mep_db_entry->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, is_jumbo_tlv_dm ? endpoint_id : 0    /* dont care for *
                                                                                                 * opcodes other than *
                                                                                                 * * CCM */ ,
                                                     is_jumbo_tlv_dm, (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY), ccm_mep_db_entry->ccm_interval, 0       /* don't
                                                                                                                                                                         * care
                                                                                                                                                                         * for
                                                                                                                                                                         * opcodes
                                                                                                                                                                         * other
                                                                                                                                                                         * than CCM 
                                                                                                                                                                         */ ,
                                                     OAMP_MEP_TX_OPCODE_LMM, mep_profile_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, mep_profile_data->lmm_rate, &dummy,
                                                                      &tx_rate_data));
        /** Check if there is no BFD endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, endpoint_id, &is_mpls_lm_dm_without_bfd_endpoint));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_rfc_6374_lm_dm
                        (unit, BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(tx_rate_data.tx_period),
                         OAMP_MEP_TX_OPCODE_LMM, mep_profile_data, is_mpls_lm_dm_without_bfd_endpoint));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Exchange the profile with currently allocated profile
 *           (during endpoint create) and update the HW write data
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] old_mep_profile_id - The existing MEP Profile.
 * \param [in] mep_profile_data  - The MEP profile data to be written.
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LMM DMM HW data
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_profile_exchange(
    int unit,
    uint8 old_mep_profile_id,
    const dnx_oam_mep_profile_t * mep_profile_data,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int new_mep_profile = 0;
    uint8 write_hw = 0, delete_old_prof = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update the MEP profile by exchanging
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.exchange(unit, 0 /* No flags */ , mep_profile_data, old_mep_profile_id,
                                                         NULL /* No extra args */ ,
                                                         &new_mep_profile, &write_hw, &delete_old_prof));

    sal_memcpy(&(hw_write_data->mep_profile_data), mep_profile_data, sizeof(dnx_oam_mep_profile_t));
    hw_write_data->oamp_mep_profile = new_mep_profile;
    if (write_hw)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_OAMP_MEP_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Add the mep_profile
 *           (during loss add) and update the HW write data
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile_data  - The MEP profile data to be written.
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LMM DMM HW data
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_profile_add(
    int unit,
    dnx_oam_mep_profile_t * mep_profile_data,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int new_mep_profile = 0;
    uint8 first_reference = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add the MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                    (unit, 0, mep_profile_data, NULL, &new_mep_profile, &first_reference));

    sal_memcpy(&(hw_write_data->mep_profile_data), mep_profile_data, sizeof(dnx_oam_mep_profile_t));
    hw_write_data->oamp_mep_profile = new_mep_profile;
    if (first_reference)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_OAMP_MEP_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free up the MEP DB indexes for LM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_id - MEP DB index of the LM DB entry
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] dm_exists - Whether DM already exists or not.
 * \param [in] dm_entry_idx - Index of DM entry if exists.
 * \param [in] lm_stat_exists - Whether LM_STAT exists or not.
 * \param [in] lm_stat_entry_idx- If statistics is enabled on self
 *                                contained endpoint, the entry ID.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_loss_mep_id_free(
    int unit,
    uint32 loss_id,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 dm_exists,
    uint32 dm_entry_idx,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_idx)
{
    uint32 entry[2];            /* Max 2 entries only. */
    int num_entries = 0, entry_id;
    uint8 offloaded = 0;
    uint32 entry_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!dm_exists)
        {
            /*
             * part2 ptr
             */
            entry[0] = loss_id;
            num_entries++;
        }       /* Else If other entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        if (ccm_mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
        {
            /** Since the RFC6374 is always first entry, move the pointer to next in chain. */
            entry_index = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(ccm_mep_db_entry->extra_data_header);
        }
        else
        {
            entry_index = MEP_DB_ENTRY_TO_OAM_ID(ccm_mep_db_entry->flex_lm_dm_entry);
        }
        if (dm_exists && (entry_index != dm_entry_idx))
        {
            /*
             * LM entries are first.
             * DM entry will be moved to LM_DB entry index.
             * Deallocate DM entry
             */
            entry[0] = dm_entry_idx;
            num_entries++;
        }
        else
        {
            /*
             * DM entry is first.
             * Deallocate LM_DB entry
             */
            entry[0] = loss_id;
            num_entries++;
        }

        if (lm_stat_entry_idx)
        {
            entry[1] = lm_stat_entry_idx;
            num_entries++;
        }
    }

    for (entry_id = 0; entry_id < num_entries; entry_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry[entry_id]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate the MEP DB index for LM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to loss information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] dm_exists - Whether DM already exists or not.
 * \param [in] dm_entry_idx - If exists, what is the entry ID.
 * \param [in] lm_db_entry_idx - LM_DB entry ID.
 * \param [in] lm_stat_exists - Whether LM_STAT already exists or not.
 * \param [in] lm_stat_entry_idx - If exists, what is the entry ID.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_loss_mep_id_alloc(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 dm_exists,
    uint32 dm_entry_idx,
    uint32 lm_db_entry_idx,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_idx)
{
    uint32 entry[2];            /* Max 2 entries only. */
    int num_entries = 0, entry_id;
    uint8 extended_statistics = 0;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;
    uint8 offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);

    extended_statistics = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) ? 1 : 0;
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!(loss_ptr->flags & BCM_OAM_LOSS_UPDATE))
        {
            if (!dm_exists)
            {
                /*
                 * part2 ptr given by user
                 */
                entry[0] = loss_ptr->loss_id;
                num_entries++;
            }
            /*
             * Else If other entry exists, we have nothing to do
             */
        }
    }
    else        /* Self contained */
    {
        if (!(loss_ptr->flags & BCM_OAM_LOSS_UPDATE))
        {
            if (dm_exists)
            {
                /*
                 * calculate the ID(s) from the other entry.
                 */
                entry[0] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(dm_entry_idx);
                num_entries++;
            }
            else
            {
                if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_MEP_IS_RF6374)
                {
                    /*
                     * In case of RFC6374, the first entry in chain is always RFC entry
                     */
                    entry[0] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(ccm_mep_db_entry->extra_data_header);
                }
                else
                {
                    /*
                     * given by user
                     */
                    entry[0] = loss_ptr->loss_id;
                }
                num_entries++;
            }

            if (extended_statistics)
            {
                /*
                 * Extended entry is added next in chain to the first entry
                 */
                entry[1] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry[0]);
                num_entries++;
            }
        }
        else
        {
            if ((loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) && !lm_stat_exists)
            {
                if (dm_exists && (dm_entry_idx != MEP_DB_ENTRY_TO_OAM_ID(ccm_mep_db_entry->flex_lm_dm_entry)))
                {
                    /*
                     * Allocate DM entry ID in next bank.
                     */
                    entry[0] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(dm_entry_idx);
                    num_entries++;
                }
                else
                {
                    /*
                     * Allocate LM stat entry ID
                     */
                    entry[0] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(lm_db_entry_idx);
                    num_entries++;
                }
            }

            if (!(loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) && lm_stat_exists)
            {
                if (dm_exists && (dm_entry_idx != MEP_DB_ENTRY_TO_OAM_ID(ccm_mep_db_entry->flex_lm_dm_entry)))
                {
                    entry[0] = dm_entry_idx;
                }
                else
                {
                    entry[0] = lm_stat_entry_idx;
                }

                SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry[0]));
            }
        }
    }

    for (entry_id = 0; entry_id < num_entries; entry_id++)
    {
        /*
         * Since we pass the ID, mep_id_memory_type info need not be filled.
         */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit,
                                                   DNX_ALGO_RES_ALLOCATE_WITH_ID, &mep_id_memory_type,
                                                   &(entry[entry_id])));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert LOSS structure information to OAMP LM MEP DB
 *          HW contents.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to loss information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LM MEP DB content
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static void
dnx_oam_loss_info_to_lm_mep_db_hw_write_info(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    /*
     * Offloaded endpoint is true if CCM MEP db entry get says offloaded
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED)
    {
        hw_write_data->offloaded = 1;
    }

    /*
     * CCM OAM id = loss -> id
     */
    hw_write_data->ccm_oam_id = loss_ptr->id;
    /*
     * stat enable true if extended statistics is set
     */
    hw_write_data->stat_enable = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) ? 1 : 0;
    hw_write_data->update_mep_db = (loss_ptr->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;

    /*
     * part_2_ptr is filled if offloaded is true. lm_db entry is filled if selfcontained is true
     */
    hw_write_data->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(loss_ptr->loss_id);
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_MEP_IS_RF6374)
    {
        hw_write_data->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(ccm_mep_db_entry->extra_data_header);
    }

}

/**
 * \brief - Write OAMP LM MEP DB info
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LM MEP DB content
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_act_on_oamp_lm_mep_db_hw_write_info(
    int unit,
    const dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    dnx_oam_oamp_endpoint_lm_params_t lm_mep_db_write_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&lm_mep_db_write_data, 0, sizeof(lm_mep_db_write_data));
    lm_mep_db_write_data.flags |= hw_write_data->stat_enable ? DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY : 0;
    lm_mep_db_write_data.flags |= hw_write_data->update_mep_db ? DNX_OAMP_OAM_LM_MEP_UPDATE : 0;
    lm_mep_db_write_data.flags |=
        (hw_write_data->flags & DNX_OAM_LM_DM_LM_STAT_EXISTS) ? DNX_OAMP_OAM_LM_MEP_LM_STAT_EXISTS : 0;
    lm_mep_db_write_data.flags |=
        (hw_write_data->flags & DNX_OAM_LM_DM_DM_STAT_EXISTS) ? DNX_OAMP_OAM_LM_MEP_DM_STAT_EXISTS : 0;
    if (hw_write_data->offloaded)
    {
        /*
         * For offloaded, setting stat entry is must, since part 2 entry is required.
         */
        lm_mep_db_write_data.flags |= DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY;
    }
    lm_mep_db_write_data.lmm_dmm_da_profile = hw_write_data->oamp_da_lsb_prof;
    lm_mep_db_write_data.mep_profile = hw_write_data->oamp_mep_profile;
    lm_mep_db_write_data.flex_lm_entry = MEP_DB_ENTRY_TO_OAM_ID(hw_write_data->flex_lm_dm_entry);
    if (hw_write_data->mep_profile_data.slm_lm)
    {
        lm_mep_db_write_data.flags |= DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR;
        lm_mep_db_write_data.slm_counter_pointer = hw_write_data->slm_counter_base_id;
        lm_mep_db_write_data.slm_core_id = hw_write_data->slm_counter_core_id;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_set(unit, hw_write_data->ccm_oam_id, &lm_mep_db_write_data));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Write OAMP LM SESSION DB info
 *          This table maps oam_id, priority to updated loss_id
 *          thus enables lm with priority.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id - Endpoint ID
 * \param [in] new_mep_id - id of the new entry
 * \param [in] priority - priority related to the session
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_lm_dm_session_set(
    int unit,
    int mep_id,
    int new_mep_id,
    int priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, mep_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, new_mep_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get TLV length from gen memory of PE
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id -mep_id
 * \param [out] data_lsb - data pattern lsb
 * \param [out] data_msb - data pattern msb
 * \param [out] tlv_length - tlv length
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_tlv_data_get(
    int unit,
    int mep_id,
    uint16 *data_msb,
    uint16 *data_lsb,
    int *tlv_length)
{
    uint32 entry_handle_id;
    uint32 index;
    uint32 nof_sub_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    uint32 nof_jumbo_tlv_entries = dnx_data_oam.oamp.nof_jumbo_dm_tlv_entries_get(unit);
    uint32 tlv_lng;
    uint32 tlv_msb;
    uint32 tlv_lsb;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_GEN_MEM_DATA, &entry_handle_id));
    index = (mep_id / nof_sub_entries) / JUMBO_TLV_ENTRIES_STEPS;

    /** get tlv  Length*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_MEM_INDEX, index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PE_GEN_DATA, INST_SINGLE, &tlv_lng);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** get  data lsb*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_MEM_INDEX, index + nof_jumbo_tlv_entries);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PE_GEN_DATA, INST_SINGLE, &tlv_lsb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** get  data msb*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_MEM_INDEX, index + (2 * nof_jumbo_tlv_entries));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PE_GEN_DATA, INST_SINGLE, &tlv_msb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *tlv_length = tlv_lng;
    *data_msb = tlv_msb;
    *data_lsb = tlv_lsb;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write TLV data pattern and length to gen memory of PE
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id -mep_id
 * \param [in] data_lsb - data pattern lsb
 * \param [in] data_msb - data pattern msb
 * \param [in] tlv_length - tlv length
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_tlv_data_set(
    int unit,
    int mep_id,
    uint16 data_msb,
    uint16 data_lsb,
    int tlv_length)
{
    uint32 entry_handle_id;
    uint32 index;
    uint32 nof_sub_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    uint32 nof_jumbo_tlv_entries = dnx_data_oam.oamp.nof_jumbo_dm_tlv_entries_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_GEN_MEM_DATA, &entry_handle_id));
    index = (mep_id / nof_sub_entries) / JUMBO_TLV_ENTRIES_STEPS;

    /*
     *  For jumbo tlv entry, mep_id should be 64(4*16), so the mem index=mep_id/64,
     *  TLV DATA stored as : tlv_length stored in N, LSB of data stored in 256+N, MSB of data stored in 512 +N.
     */
    /** set tlv  Length*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_MEM_INDEX, index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PE_GEN_DATA, INST_SINGLE, tlv_length);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set  data lsb*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_MEM_INDEX, index + nof_jumbo_tlv_entries);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PE_GEN_DATA, INST_SINGLE, data_lsb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set  data msb*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GEN_MEM_INDEX, index + (2 * nof_jumbo_tlv_entries));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PE_GEN_DATA, INST_SINGLE, data_msb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Read OAMP LM SESSION DB info
 *          This table maps oam_id, priority to updated loss_id
 *          thus enables lm with priority.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id - Endpoint ID
 * \param [in] priority - priority related to the session
 * \param [out] new_mep_id - id of the new entry
 * \param [out] is_exist - indication whether entry was found
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_lm_dm_session_get(
    int unit,
    int mep_id,
    int priority,
    uint32 *new_mep_id,
    uint8 *is_exist)
{
    uint32 entry_handle_id;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_exist = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, mep_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, new_mep_id);
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NOT_FOUND)
    {
        *is_exist = 0;
    }
    else
    {
        *is_exist = 1;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write OAMP SLM MEASUREMENT PERIOD TIMER register.
 *          This table sets Txm SLM Measurement Period Timer init/wrap value.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP PROFILE content.
 * \return
 *   shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_slm_measurement_period_set(
    int unit,
    const dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_SLM_MEASUREMENT_PERIOD_TIMER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLM_MEASUREMENT_PERIOD_TIMER_PROFILE_INDEX,
                               hw_write_data->measurement_period_write_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXM_SLM_MEASUREMENT_PERIOD_TIMER_INIT_VALUE__N,
                                 INST_SINGLE, hw_write_data->measurement_period_write_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Adds loss session with PCP, including mep_profile
 *          if needed.
 *
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP PROFILE content.
 * \param [in] hw_write_info - Pointer to the structure,
 *                 containing information about the
 *                 OAMP LMM DMM HW data
 * \param [in] endpoint_info - Pointer to the endpoint info,
 * \param [in] ccm_mep_db_entry - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP DB data
 * \param [in] entry_mep_id - Pointer to the requested loss session
 *                 data,
 * \param [in] ccm_mep_id - ID of the ccm entry in case requested by user.
 *                          Should be left 0 otherwise
 * \param [in] priority - PCP of the requested loss session
 * \param [in] entry_exists - flags whether a session entry already exists
 *
 * \return
 *   shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_add_loss_delay_pcp(
    int unit,
    dnx_oam_mep_profile_t * mep_profile_data,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_info,
    bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    bcm_oam_endpoint_t * entry_mep_id,
    uint32 *ccm_mep_id,
    const int priority,
    const uint8 entry_exists)
{
    int system_headers_mode;
    dnx_oamp_mep_db_memory_type_t memory_type;
    bcm_oam_endpoint_t mep_id = 0;
    uint8 mpls_pwe_mep;
    uint32 rfc_6374_id;
    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    if (entry_exists)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                          mep_profile_data, hw_write_info));
    }
    else
    {
        /*
         * Add the OAMP MEP profile if needed
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_add(unit, mep_profile_data, hw_write_info));
    }

    /*
     * Duplicate ccm entry
     */
    mep_id = *entry_mep_id;
    /** Get ID for the CCM entry */
    if (entry_exists)
    {
        /*
         * if entry exists, use its id
         */
        *ccm_mep_id = *entry_mep_id;
    }
    else
    {
        if (*ccm_mep_id != 0)
        {
            /** if specific id is requested use the given id */
            endpoint_info->flags |= BCM_OAM_ENDPOINT_WITH_ID;
            endpoint_info->id = *ccm_mep_id;
            SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id(unit, endpoint_info, NULL, 0));
            *entry_mep_id = *ccm_mep_id;
        }
        else
        {
            /** if there is no existing entry or requested id, get an id */
            endpoint_info->flags &= (0xffffffff ^ BCM_OAM_ENDPOINT_WITH_ID);
            SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id(unit, endpoint_info, NULL, 0));
            *ccm_mep_id = endpoint_info->id;
            *entry_mep_id = endpoint_info->id;
        }
        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            if (hw_write_info->flex_lm_dm_entry != 0)
            {
                /*
                 * Since we pass the ID, memory_type info need not be filled.
                 */
                rfc_6374_id = MEP_DB_ENTRY_TO_OAM_ID(hw_write_info->flex_lm_dm_entry);
                SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc
                                (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &rfc_6374_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: only WITH_ID is supported for the duplicated RFC_6374 entry.\n");
            }
            ccm_mep_db_entry->extra_data_header = rfc_6374_id;
            ccm_mep_db_entry->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(rfc_6374_id);
            hw_write_info->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(rfc_6374_id);
        }
    }

    /*
     * In case of Downmep except egress_injection case in JR2, we need to update the counter pointer. counter_pointer is
     * set with counter base in endpoint create. Here we add the priority to get the counter pointer.
     */
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
    {
        if (!((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)) &&
              (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)))
        {
            ccm_mep_db_entry->counter_ptr += priority;
        }
    }
    else
    {
        ccm_mep_db_entry->counter_ptr += priority;
    }

    ccm_mep_db_entry->ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;

    /** Sign if ETH OAM is upmep */
    ccm_mep_db_entry->flags |= DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? DNX_OAMP_OAM_CCM_MEP_IS_UPMEP : 0;
    ccm_mep_db_entry->flags &= ~DNX_OAMP_OAM_CCM_MEP_UPDATE;

    /*
     *  For MPLS/PWE, push_profile points to PCP that is copied to EXP field of the packet
     *  push_profile maps the push_profile_id to EXP
     */
    if (mpls_pwe_mep)
    {
        dnx_oam_ttl_exp_profile_t mpls_profile;
        int profile_id;
        uint8 first_reference;

        mpls_profile.ttl = endpoint_info->egress_label.ttl;
        mpls_profile.exp = priority;

        SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.allocate_single
                        (unit, 0, &mpls_profile, NULL, &profile_id, &first_reference));

        /** Check if profile is new */
        if (first_reference)
        {
            /** Set profile parameters */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set
                            (unit, profile_id, mpls_profile.ttl, mpls_profile.exp));
        }

        ccm_mep_db_entry->push_profile = profile_id;
    }
    else
    {
        /*
         * PCP is taken by: PCP, CFI <--- MEP.VidDeiPcp
         */
        ccm_mep_db_entry->inner_pcp_dei = priority << 1;
    }

    if (!entry_exists)
    {
        /*
         * Add The LMM-PCP entry to MEP_DB
         */
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, *ccm_mep_id, ccm_mep_db_entry));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_rfc_6374_endpoint_set(unit, *ccm_mep_id, ccm_mep_db_entry, TRUE, FALSE));
        }

        /*
         * Set oam_id session_map
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_set(unit, mep_id, *ccm_mep_id, priority));
        /*
         * Set oam_id session_map
         */
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_session_oam_id_set(unit, (uint16) *ccm_mep_id, (uint16) mep_id, priority));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_loss_add_internal(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    bcm_oam_profile_type_t oam_profile_type;
    uint32 oam_profile_index = 0;
    uint8 is_ingress = 0, lm_type = 0, dummy = 0;
    uint8 update = 0, exchange_mac_prof = 0;
    uint8 piggy_back_lm = 0;
    uint8 mpls_pwe_mep = 0;
    uint8 offloaded = 0;
    uint32 dm_entry_idx = 0;
    int counter = 0;
    int priority = 0;
    int priority_index;
    int is_jumbo_dm_config = 0;
    uint32 loss_ptr_id = 0;
    uint32 mep_id;

    bcm_mac_t oamp_mac_da_msb = { 0 }, old_oamp_mac_da_msb =
    {
    0};
    bcm_mac_t oamp_mac_da_lsb = { 0 }, old_oamp_mac_da_lsb =
    {
    0};
    uint8 old_oamp_da_lsb_prof = 0;
    uint8 dm_exists = 0, lm_exists = 0, lm_stat_exists = 0, flexibe_da_enabled = 0, temp_dm_exists =
        0, rfc_6374_exists = 0;
    uint8 lm_dm_exists = 0;
    SHR_FUNC_INIT_VARS(unit);

    update = (loss_ptr->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;
    if ((loss_ptr->flags & BCM_OAM_LOSS_SLM) != 0)
    {
        /*
         * In case of SLM the single ended flag is not set
         */
        piggy_back_lm = 0;
    }
    else
    {
        piggy_back_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0);
    }

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    mep_id = loss_ptr->id;
    /*
     * Loss add is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(loss_ptr->id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss add is not supported on unaccelerated endpoints.\n");
    }

    /*
     * Loss add is not supported on unaccelerated endpoints
     */
    if (!DNX_OAM_MEP_IS_BETWEEN_4_7_BANK(loss_ptr->id) &&
        (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_txm_bank_limitation)) &&
        (loss_ptr->flags & BCM_OAM_LOSS_SLM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: SLM entries should be configured at banks 4-7.\n");
    }

    if ((loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) && (loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_OAM_LOSS_REPORT_MODE and BCM_OAM_LOSS_STATISTICS_EXTENDED cannot be used together.\n");
    }

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_oam_group_get(unit, endpoint_info.group, &group_info));
    }

    if (IS_OAM_SERVER((&endpoint_info)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss add is not supported for OAM server endpoints.\n");
    }

    if (endpoint_info.acc_profile_id != BCM_OAM_PROFILE_INVALID)
    {
        oam_profile_type = DNX_OAM_PROFILE_TYPE_GET(endpoint_info.acc_profile_id);
        oam_profile_index = DNX_OAM_PROFILE_IDX_GET(endpoint_info.acc_profile_id);
        is_ingress = DNX_OAM_PROFILE_IS_INGRESS(oam_profile_type);
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_get(unit, oam_profile_index, is_ingress, TRUE, &lm_type, &dummy));

        if (piggy_back_lm)
        {
            if (lm_type != DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_DUAL_ENDED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: Dual-Ended accelerated profile should be configured for Dual-Ended LM.\n");
            }
        }
        else
        {
            /*
             * Single-Ended LM or SLM
             */
            if (lm_type != DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_LM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: Single-Ended accelerated profile should be configured for SLM and Single-Ended LM.\n");
            }
        }
    }

    /*
     * Get the current CCM mep db entry
     */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * Check if Delay is already configured on this endpoint
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, loss_ptr->id, dm_mep_db_entry, &dm_exists));

    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
        /*
         * Verify that loss without priority was not previously defined on this endpoint
         */
        if (dm_exists)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Loss/Delay with priority may not be defined on a MEP with Loss/Delay");
        }

        /*
         * Session map entry may exist in two cases:
         * 1. if delay was already configured on this priority
         * 2. if loss was already configured on this priority, and this is update
         */
        DNX_OAM_GET_PRIORITY_FROM_BITMAP(loss_ptr->pkt_pri_bitmap, priority);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get(unit, loss_ptr->id, priority, &loss_ptr_id, &lm_dm_exists));
        dm_exists = 0;
        if (lm_dm_exists)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, loss_ptr_id, dm_mep_db_entry, &dm_exists));
            loss_ptr->id = loss_ptr_id;
            if (dm_exists)
            {
                ccm_mep_db_entry->mep_profile = dm_mep_db_entry->mep_profile;
            }
            else
            {
                /*
                 * Loss is already allocated on this priority
                 */
                if (!update)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss is already allocated on this priority.\n");
                }
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                                (unit, loss_ptr->id, lm_mep_db_entry, &lm_exists, &lm_stat_exists));
                ccm_mep_db_entry->mep_profile = lm_mep_db_entry->mep_profile;
            }
        }
    }
    else
    {
        /*
         * Verify that loss/delay with priority was not previously defined on this endpoint
         */
        for (priority_index = 0; priority_index < 8; priority_index++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get
                            (unit, loss_ptr->id, priority_index, &loss_ptr_id, &temp_dm_exists));
            if (temp_dm_exists && !DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Loss/Delay may not be defined on a MEP with Loss/Delay with priority");
            }
        }

    }

    if (dm_exists)
    {
        DNX_OAM_GET_DELAY_ENTRY_CONFIGURED(offloaded, dm_mep_db_entry, dm_entry_idx);
        hw_write_info->flags |= DNX_OAM_LM_DM_DM_STAT_EXISTS;
    }

    if (DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        rfc_6374_exists = 1;
        hw_write_info->flags |= DNX_OAM_LM_DM_DM_STAT_EXISTS;
        ccm_mep_db_entry->flags |= DNX_OAMP_OAM_MEP_IS_RF6374;
    }

    /*
     * Get the current OAMP MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));
    /*
     * -- SW configurations --
     */
    if (update)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                        (unit, loss_ptr->id, lm_mep_db_entry, &lm_exists, &lm_stat_exists));
        if (!lm_exists)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Loss is not enabled on this endpoint.\n");
        }

        if ((piggy_back_lm && !curr_mep_profile_data->piggy_back_lm) ||
            ((loss_ptr->flags & BCM_OAM_LOSS_SLM) && !curr_mep_profile_data->slm_lm) ||
            ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)
             && (curr_mep_profile_data->piggy_back_lm || curr_mep_profile_data->slm_lm)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss update is not supported for one LM type to another.\n");
        }

        if (lm_stat_exists)
        {
            hw_write_info->flags |= DNX_OAM_LM_DM_LM_STAT_EXISTS;
        }
    }
    /*
     * Call verify function
     */
    SHR_IF_ERR_EXIT(dnx_oam_loss_add_verify
                    (unit, loss_ptr, ccm_mep_db_entry, (dm_exists || rfc_6374_exists), dm_entry_idx, &priority));

    /*
     * Get if LM/DM flexible DMAC is enabled
     */
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, mep_id, &sw_endpoint_info));
    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        flexibe_da_enabled = (sw_endpoint_info.extra_data_index == DNX_OAM_EXTRA_DATA_HEADER_INVALID) ? 0 : 1;
    }

    /*
     * Verify LM/DM flexible DMAC conflict
     */
    if (flexibe_da_enabled && dm_exists)
    {
        SHR_IF_ERR_EXIT(dnx_oam_lmdm_flexible_dmac_conflict_verify
                        (unit, loss_ptr->peer_da_mac_address, sw_endpoint_info.extra_data_index));
    }

    /*
     * Verify LMM/DMM rate conflict for Jumbo Dm feature
     */
    if (sw_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV)
    {
        /** For Jumbo DM feature: CCM>=3.33ms DMM/DMR>=100ms LMM/LMR>=100ms*/
        if ((loss_ptr->period < DBAL_ENUM_FVAL_CCM_INTERVAL_100MS)
            && (loss_ptr->period != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For Jumbo DM feature: CCM>=3.33ms DMM/DMR>=100ms LMM/LMR>=100ms\n");
        }
        is_jumbo_dm_config = 1;
    }

    /*
     * If this is an update case or in case Delay was already configured, there will be already a MAC configured.
     * Exchange is true for those 2 cases.
     */
    exchange_mac_prof = ((update || dm_exists) && !flexibe_da_enabled);
    if (update)
    {
        old_oamp_da_lsb_prof = lm_mep_db_entry->lmm_dmm_da_profile;
    }
    else if (dm_exists)
    {
        old_oamp_da_lsb_prof = dm_mep_db_entry->lmm_dmm_da_profile;
    }

    /*
     * For exchange, get the existing mac information
     */
    if (exchange_mac_prof && !piggy_back_lm && !mpls_pwe_mep)
    {
        /*
         * Get the current LMM MAC DA MSB
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                     curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                     &counter, &old_oamp_mac_da_msb));

        /*
         * Get the current LMM MAC DA LSB
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                     old_oamp_da_lsb_prof, &counter,
                                                                                     &old_oamp_mac_da_lsb));
    }

    if (!piggy_back_lm && !mpls_pwe_mep && !flexibe_da_enabled)
    {
        /*
         * Extract the OUI (MSB) part from the mac address
         */
        GET_MSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address, oamp_mac_da_msb);
        /*
         * Alloc/exchange the MSB profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_alloc(unit, oamp_mac_da_msb,
                                                              old_oamp_mac_da_msb,
                                                              curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                              exchange_mac_prof, hw_write_info));

        /*
         * Extract the NIC (LSB) part from the mac address
         */
        GET_LSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address, oamp_mac_da_lsb);
        /*
         * Alloc/exchange the LSB profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_alloc(unit, oamp_mac_da_lsb,
                                                              old_oamp_mac_da_lsb, old_oamp_da_lsb_prof,
                                                              exchange_mac_prof, hw_write_info));
    }

    /*
     * Fill the current MEP profile with Loss params
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_loss_info_to_mep_profile_data
                    (unit, loss_ptr, is_jumbo_dm_config, ccm_mep_db_entry, curr_mep_profile_data,
                     (endpoint_info.type == bcmOAMEndpointTypeMplsLmDmLsp),
                     (_SHR_IS_FLAG_SET
                      (sw_endpoint_info.sw_state_flags,
                       DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_DOWN_EGRESS_INJECTION))));

    /** In can of PG-B and existing DM entry, keep the DA MSB and LSB profiles. */
    if (dm_exists && piggy_back_lm && !mpls_pwe_mep)
    {
        hw_write_info->oamp_da_lsb_prof = old_oamp_da_lsb_prof;
    }
    else
    {
        curr_mep_profile_data->oamp_mac_da_oui_prof = hw_write_info->oamp_da_msb_prof;
    }

    if ((loss_ptr->flags & BCM_OAM_LOSS_SLM))
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_slm_measurement_profile_sw_update
                        (unit, loss_ptr, curr_mep_profile_data, hw_write_info));

        sw_endpoint_info.lm_counter_idx = loss_ptr->slm_counter_base_id;
        sw_endpoint_info.lm_counter_if = loss_ptr->slm_counter_core_id;

        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_set(unit, (uint32) loss_ptr->id, &sw_endpoint_info, TRUE));
    }

    /*
     * Write the session information for lm with priority
     */
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0) && !(loss_ptr->flags & BCM_OAM_LOSS_SLM))
    {
        /*
         * Add lm-pcp
         */
        hw_write_info->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(loss_ptr->loss_id);
        SHR_IF_ERR_EXIT(dnx_oam_add_loss_delay_pcp
                        (unit, curr_mep_profile_data, hw_write_info, &endpoint_info, ccm_mep_db_entry,
                         &(loss_ptr->id), &(loss_ptr->pm_id), priority, (dm_exists || update)));
    }
    else
    {
        /*
         * Exchange the OAMP MEP profile with existing MEP profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                          curr_mep_profile_data, hw_write_info));
    }

    /*
     * Allocate mep_db index(es).
     */
    SHR_IF_ERR_EXIT(dnx_oam_loss_mep_id_alloc
                    (unit, loss_ptr, ccm_mep_db_entry, dm_exists, dm_entry_idx, lm_mep_db_entry->lm_db_entry_idx,
                     lm_stat_exists, lm_mep_db_entry->lm_stat_entry_idx));

    /*
     * Update the HW write info from SW info for LM MEP DB
     */
    dnx_oam_loss_info_to_lm_mep_db_hw_write_info(unit, loss_ptr, ccm_mep_db_entry, hw_write_info);

    /*
     * -- HW configurations --
     */

    /*
     * Write the DA MAC msb into register.
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_set(unit,
                                                            hw_write_info->oamp_da_msb_prof,
                                                            hw_write_info->oamp_mac_da_msb));
    }

    /**
    * Configure slm measurement period
    */
    if (hw_write_info->measurement_period && (loss_ptr->flags & BCM_OAM_LOSS_SLM))
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_slm_measurement_period_set(unit, hw_write_info));
    }

    /*
     * Write the DA MAC lsb into register.
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_set(unit,
                                                            hw_write_info->oamp_da_lsb_prof,
                                                            hw_write_info->oamp_mac_da_lsb));
    }

    /*
     * Write the OAMP_MEP_PROFILE (along with DA MAC MSB profile)
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     hw_write_info->oamp_mep_profile,
                                                     DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info),
                                                     &(hw_write_info->mep_profile_data)));
    }

    /*
     * Write extra data entries along with flexible DMAC
     */
    if (flexibe_da_enabled)
    {
        dnx_oamp_ccm_endpoint_extra_data_t extra_data;
        sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
        extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info.extra_data_index);
        extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE | DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC;
        extra_data.opcodes_to_prepend = DNX_OAMP_EXTRA_DATA_TO_PREPEND_LMM | DNX_OAMP_EXTRA_DATA_TO_PREPEND_DMM;
        sal_memcpy(extra_data.dmac, loss_ptr->peer_da_mac_address, sizeof(bcm_mac_t));
        if (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE))
        {
            extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID20;
            extra_data.opcodes_to_prepend |= DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
            sal_memcpy(extra_data.maid48, group_info.name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        else if (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE))
        {
            extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID40;
            extra_data.opcodes_to_prepend |= DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
            sal_memcpy(extra_data.maid48, group_info.name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        else if (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
        {
            extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID48;
            extra_data.opcodes_to_prepend |= DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
            sal_memcpy(extra_data.maid48, group_info.name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_set(unit, &extra_data));
    }

    /*
     * Write the MEP DB entry(ies) (along with DA MAC LSB profile)
     */
    SHR_IF_ERR_EXIT(dnx_oam_act_on_oamp_lm_mep_db_hw_write_info(unit, hw_write_info));

exit:
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(lm_mep_db_entry);
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set MEASURE_NEXT_RECEIVED_SLR in LM_DB.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 * \param [in] entry_values - Struct containing the values to be
 *        written to the entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_loss_measure_next_slr_set(
    int unit,
    bcm_oam_loss_t * loss_ptr,
    dnx_oam_oamp_ccm_endpoint_t entry_values)
{

    uint32 value, entry_handle_id;
    uint8 is_offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Loss add is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(loss_ptr->loss_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss add is not supported on unaccelerated endpoints.\n");
    }

    /** Offloaded Endpoints required additional configuration */
    if ((entry_values.flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) == DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED)
    {
        is_offloaded = 1;
    }

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    /** LM stats for Offloaded Endpoints reside in the main entry */
    value = is_offloaded ? loss_ptr->id : loss_ptr->loss_id;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, value);

    if (is_offloaded)
    {
        /** Set result type   */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1);
        /** Set MEP type   */
        switch (entry_values.mep_type)
        {
            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM:
                value = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_CCM_ETHERNET;
                break;

            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP:
                value = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_MPLSTP;
                break;

            case DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE:
                value = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_PWE;
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error.  MEP type does not match result type.");
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFLOADED_MEP_TYPE_SUFFIX, INST_SINGLE, value);
    }
    else
    {
        /** Set result type   */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR, INST_SINGLE, 1);

    /** Write entry   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_add(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{

    bcm_oam_endpoint_info_t endpoint_info;
    dnx_oam_oamp_ccm_endpoint_t mep_db_entry;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_loss_t), loss_ptr);

    bcm_oam_endpoint_info_t_init(&endpoint_info);

    if (loss_ptr->flags & BCM_OAM_LOSS_UPDATE_NEXT_RECEIVED_SLR)
    {
        /** Check the memory type of the endpoint */
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));
        mep_db_entry.flags =
            endpoint_info.endpoint_memory_type ==
            bcmOamEndpointMemoryTypeLmDmOffloadedEntry ? DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED : 0;
        /** Map bcm ep type to mep db type */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type
                        (unit, endpoint_info.type, &mep_db_entry.mep_type));
        /** Update the OAMP_MEP_DB_LM_DB to measure next slr only */
        SHR_IF_ERR_EXIT(dnx_oam_loss_measure_next_slr_set(unit, loss_ptr, mep_db_entry));
    }
    else
    {
        /** Add an OAM Loss entry */
        SHR_IF_ERR_EXIT(dnx_oam_loss_add_internal(unit, loss_ptr));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write OAMP SLM MEASUREMENT PERIOD TIMER register.
 *          This table sets Txm SLM Measurement Period Timer init/wrap value.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] measurement_period_index - index to slm_measurement_period profile
 * \param [out] measurement_period - value for measuremnt period
 * \return
 *   shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_slm_measurement_period_get(
    int unit,
    uint8 measurement_period_index,
    int *measurement_period)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_SLM_MEASUREMENT_PERIOD_TIMER, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLM_MEASUREMENT_PERIOD_TIMER_PROFILE_INDEX,
                               measurement_period_index);

    /** Get all fields   */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Read format   */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                        DBAL_FIELD_TXM_SLM_MEASUREMENT_PERIOD_TIMER_INIT_VALUE__N,
                                                        INST_SINGLE, (uint32 *) measurement_period));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_get(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    uint32 session_oam_id;
    int priority;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    bcm_mac_t oamp_mac_da_msb = { 0 };
    bcm_mac_t oamp_mac_da_lsb = { 0 };
    uint8 offloaded = 0, extended_statistics = 0, lm_exists = 0, lm_stat_exists = 0, flexibe_da_enabled = 0;
    uint8 mpls_pwe_mep = 0;
    int counter = 0;
    int dummy = 0;
    dnx_bfd_tx_period_t tx_rate_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Loss is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(loss_ptr->id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss is not supported on unaccelerated endpoints.\n");
    }

    /*
     * Get if LM/DM flexible DMAC is enabled
     */
    sal_memset(&sw_endpoint_info, 0, sizeof(dnx_oam_endpoint_info_t));
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, loss_ptr->id, &sw_endpoint_info));
    flexibe_da_enabled = (sw_endpoint_info.extra_data_index == DNX_OAM_EXTRA_DATA_HEADER_INVALID) ? 0 : 1;

    /*
     * Get lm with priority info
     */
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0)
        && !DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));

        SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));

        DNX_OAM_GET_PRIORITY_FROM_BITMAP(loss_ptr->pkt_pri_bitmap, priority)
            SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_get(unit, loss_ptr->id, priority, &session_oam_id));

        loss_ptr->id = session_oam_id;
    }

    /*
     * First, read the CCM entry
     */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    /*
     * Get the LM MEP DB entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get(unit, loss_ptr->id, loss_params, &lm_exists, &lm_stat_exists));
    if (!lm_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Loss is not enabled on this endpoint.\n");
    }

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * Get the OAMP MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit,
                                                                 ccm_mep_db_entry->mep_profile,
                                                                 &counter, curr_mep_profile_data));

    if (!curr_mep_profile_data->piggy_back_lm && !mpls_pwe_mep)
    {
        if (!flexibe_da_enabled)
        {
            /*
             * Get the LMM DA OUI
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                         curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                         &counter, oamp_mac_da_msb));

            /*
             * Get the LMM DA NIC
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                         loss_params->lmm_dmm_da_profile,
                                                                                         &counter, oamp_mac_da_lsb));

            /*
             * Combined MSB and LSB to form FULL MAC DA
             */
            GET_MAC_FROM_MSB_LSB(oamp_mac_da_msb, oamp_mac_da_lsb, loss_ptr->peer_da_mac_address);
        }
        else
        {
            dnx_oamp_ccm_endpoint_extra_data_t extra_data;
            sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
            extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info.extra_data_index);
            SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_get(unit, &extra_data));
            sal_memcpy(loss_ptr->peer_da_mac_address, extra_data.dmac, sizeof(bcm_mac_t));
        }
    }

    if (curr_mep_profile_data->slm_lm)
    {
        /*
         * Get the SLM measurement period in number of HW scans
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_slm_measurement_period_get
                        (unit, curr_mep_profile_data->slm_measurement_period_index, &loss_ptr->measurement_period));

        /*
         * Convert the number of HW scans to MS and store in entry info when the measurement period is different than 0\
         */
        if (loss_ptr->measurement_period)
        {
            loss_ptr->measurement_period = BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(loss_ptr->measurement_period);
        }
        loss_ptr->slm_counter_base_id = ccm_mep_db_entry->counter_ptr;
        loss_ptr->slm_counter_core_id = ccm_mep_db_entry->counter_interface;
    }

    /*
     * Fill the Loss pointer
     */
    if (offloaded & !curr_mep_profile_data->report_mode_lm)
    {
        /*
         * In case of offloaded, statistics is always enabled
         */
        extended_statistics = 1;
        loss_ptr->loss_id = loss_params->flex_lm_entry;
    }
    else
    {
        extended_statistics = (loss_params->lm_stat_entry_idx != 0);
        loss_ptr->loss_id = loss_params->lm_db_entry_idx;
    }

    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, curr_mep_profile_data->lmm_rate, &dummy,
                                                                      &tx_rate_data));
        tx_rate_data.tx_period ? loss_ptr->period =
            BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(tx_rate_data.tx_period) : 0;
    }
    else
    {
        loss_ptr->period = curr_mep_profile_data->lmm_rate;
    }

    if (extended_statistics)
    {
        loss_ptr->flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
    }
    if (!curr_mep_profile_data->piggy_back_lm && !curr_mep_profile_data->slm_lm)
    {
        loss_ptr->flags |= BCM_OAM_LOSS_SINGLE_ENDED;
    }
    if (curr_mep_profile_data->slm_lm)
    {
        loss_ptr->flags |= BCM_OAM_LOSS_SLM;
    }

    if (!curr_mep_profile_data->report_mode_lm)
    {
        loss_ptr->tx_nearend = loss_params->lm_my_tx;
        loss_ptr->rx_nearend = loss_params->lm_my_rx;
        loss_ptr->tx_farend = loss_params->lm_peer_tx;
        loss_ptr->rx_farend = loss_params->lm_peer_rx;
        if (extended_statistics)
        {
            loss_ptr->loss_farend =
                (loss_params->lm_my_tx == 0) ? 0 : (100 * loss_params->acc_lm_far) / loss_params->lm_my_tx;
            loss_ptr->loss_nearend =
                (loss_params->lm_peer_tx == 0) ? 0 : (100 * loss_params->acc_lm_near) / loss_params->lm_peer_tx;
            loss_ptr->loss_nearend_max = loss_params->max_lm_near;
            loss_ptr->loss_nearend_acc = loss_params->acc_lm_near;
            loss_ptr->loss_farend_max = loss_params->max_lm_far;
            loss_ptr->loss_farend_acc = loss_params->acc_lm_far;
        }
    }
    else
    {
        loss_ptr->flags |= BCM_OAM_LOSS_REPORT_MODE;
    }

exit:
    SHR_FREE(loss_params);
    SHR_FREE(curr_mep_profile_data);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free the slm measurement period profile and clear dbal if it is required.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile_data - contain needed profile information.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_slm_measurement_period_clear(
    int unit,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    uint32 entry_handle_id;
    uint8 delete_old_prof;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Free the slm measurement period profile or (ref_count - 1)
     */
    SHR_IF_ERR_EXIT(algo_oam_db.slm_oamp_measurement_period.free_single(unit,
                                                                        mep_profile_data->slm_measurement_period_index,
                                                                        &delete_old_prof));

    if (delete_old_prof)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_SLM_MEASUREMENT_PERIOD_TIMER, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLM_MEASUREMENT_PERIOD_TIMER_PROFILE_INDEX,
                                   mep_profile_data->slm_measurement_period_index);

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Internal function for deleting an OAM loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *                 In case it's a loss with priority, loss->id is the
 *                 duplicated ccm entry
 * \param [in] mep_id - The ID of the ccm_entry of the MEP
 * \param [in] extra_data_entries_on_mdb - Existence of extra entries
 *
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_oamp_loss_delete_internal(
    int unit,
    bcm_oam_loss_t * loss_ptr,
    bcm_oam_endpoint_t mep_id,
    uint8 extra_data_entries_on_mdb)
{
    dnx_oam_endpoint_info_t sw_endpoint_info;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    uint8 oamp_da_lsb_prof = 0;
    int counter = 0;
    uint8 dm_exists = 0, delete_old_prof = 0, mpls_pwe_mep = 0, tx_rate_write_delete = 0;
    uint32 dm_entry_idx = 0;
    uint8 offloaded = 0, lm_exists = 0, lm_stat_exists = 0, flexibe_da_enabled = 0;
    uint8 piggy_back_lm = 0;
    int priority = 0;
    uint8 is_last_entry = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Check if LM/DM flexible DMAC is enabled
     */
    sal_memset(&sw_endpoint_info, 0, sizeof(dnx_oam_endpoint_info_t));
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, mep_id, &sw_endpoint_info));
    flexibe_da_enabled = (sw_endpoint_info.extra_data_index == DNX_OAM_EXTRA_DATA_HEADER_INVALID) ? 0 : 1;

    /*
     * Get the current CCM mep db entry
     */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * Get the LM MEP DB entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get(unit, loss_ptr->id, loss_params, &lm_exists, &lm_stat_exists));

    if (!lm_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Loss is not enabled on this endpoint.\n");
    }

    if (offloaded)
    {
        loss_ptr->loss_id = loss_params->flex_lm_entry;
    }
    else
    {
        loss_ptr->loss_id = loss_params->lm_db_entry_idx;
    }
    oamp_da_lsb_prof = loss_params->lmm_dmm_da_profile;

    /*
     * Get the current OAMP MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));
    piggy_back_lm = curr_mep_profile_data->piggy_back_lm;

    /*
     * -- SW configurations --
     */

    /*
     * Check if Delay add is already configured on this endpoint
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, loss_ptr->id, dm_mep_db_entry, &dm_exists));
    if (dm_exists)
    {
        DNX_OAM_GET_DELAY_ENTRY_CONFIGURED(offloaded, dm_mep_db_entry, dm_entry_idx);
    }

    if (!piggy_back_lm && !mpls_pwe_mep && !dm_exists && !flexibe_da_enabled)
    {
        /*
         * Free the DA MAC LSB profile or (ref_count - 1)
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.free_single(unit,
                                                                                curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                &delete_old_prof));
        /*
         * Free the DA MAC MSB profile or (ref_count - 1)
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.free_single(unit,
                                                                                oamp_da_lsb_prof, &delete_old_prof));
    }

    if (curr_mep_profile_data->slm_lm)
    {
        /*
         * Free the slm measurement period profile from the SW and HW
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_slm_measurement_period_clear(unit, curr_mep_profile_data));
    }

    /** Clear the TX rate profile from the SW */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.free_single
                        (unit, curr_mep_profile_data->lmm_rate, &tx_rate_write_delete));
    }

    /*
     * Free the OAMP MEP profile from loss params.
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_remove_loss_info_from_mep_profile_data
                    (unit, loss_ptr->id,
                     (sw_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV),
                     ccm_mep_db_entry, curr_mep_profile_data,
                     (sw_endpoint_info.type == bcmOAMEndpointTypeMplsLmDmLsp),
                     (_SHR_IS_FLAG_SET
                      (sw_endpoint_info.sw_state_flags,
                       DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_DOWN_EGRESS_INJECTION))));

    /*
     * In case it's lm with priorty, delete the profile only once
     */

    if (!((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0)))
    {
        /*
         * Exchange the OLD profile and get NEW profile.
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                          curr_mep_profile_data, hw_write_info));

    }
    else
    {
        uint8 last_reference;

        DNX_OAM_GET_PRIORITY_FROM_BITMAP(loss_ptr->pkt_pri_bitmap, priority);

        if (!dm_exists)
        {
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_mep_profile.free_single(unit, (int) ccm_mep_db_entry->mep_profile, &last_reference));

            if (last_reference)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_clear(unit, ccm_mep_db_entry->mep_profile));
            }
        }
        else
        {
            /*
             * Exchange the OLD profile and get NEW profile.
             */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                              curr_mep_profile_data, hw_write_info));
        }

        /*
         * For MPLS/PWE - free the push profile
         */
        if (mpls_pwe_mep)
        {
            if (!dm_exists)
            {

                uint8 push_profile = ccm_mep_db_entry->push_profile;
                SHR_IF_ERR_EXIT(algo_oam_db.
                                oam_mpls_pwe_exp_ttl_profile.free_single(unit, push_profile, &last_reference));
                if (last_reference)
                {
                /** Set profile parameters */
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_clear(unit, push_profile));
                }
            }

        }
    }
    /*
     * Free up the OAMP MEP ID(s)
     */
    SHR_IF_ERR_EXIT(dnx_oam_loss_mep_id_free(unit, loss_ptr->loss_id, ccm_mep_db_entry,
                                             dm_exists, dm_entry_idx, lm_stat_exists, loss_params->lm_stat_entry_idx));

    /*
     * ---- HW configurations ---
     */

    /*
     * Write the new OAMP MEP profile if required
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set
                        (unit, hw_write_info->oamp_mep_profile, DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info),
                         &(hw_write_info->mep_profile_data)));
    }

    /*
     * Clear the Loss MEP DB entry in HW
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_clear
                    (unit, loss_ptr->id, DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info)));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
        /** Clear the TX rate profile from the HW */
        if (tx_rate_write_delete)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_tx_rate_clear(unit, curr_mep_profile_data->lmm_rate));
        }
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }

    /*
     * In case it's lm with priority, clear the ccm entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
        if (!dm_exists)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear(unit, loss_ptr->id, extra_data_entries_on_mdb));

            SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_clear(unit, mep_id, priority));

            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, loss_ptr->id));

        }
        else
        {
            /*
             * Update the CCM mep db entry with new OAMP MEP profile
             */
            ccm_mep_db_entry->mep_profile = hw_write_info->oamp_mep_profile;
            ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_UPDATE;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, loss_ptr->id, ccm_mep_db_entry));
        }
    }
    else
    {
        /*
         * Update the CCM mep db entry with new OAMP MEP profile
         */
        ccm_mep_db_entry->mep_profile = hw_write_info->oamp_mep_profile;
        ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_UPDATE;

        if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
        {
            if (!dm_exists)
            {
                is_last_entry = TRUE;
            }
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_rfc_6374_endpoint_set
                            (unit, loss_ptr->id, ccm_mep_db_entry, is_last_entry, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, loss_ptr->id, ccm_mep_db_entry));
        }

    }

exit:
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(loss_params);
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_delete(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    uint8 extra_data_entries_on_mdb = 0;
    int priority = 0;
    bcm_oam_endpoint_t mep_id = 0;
    uint32 session_oam_id;
    uint8 loss_with_priority_exists;
    uint8 is_exists, lm_stat_exists = 0;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_loss_t), loss_ptr);

    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Loss is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(loss_ptr->id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss is not supported on unaccelerated endpoints.\n");
    }

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));
        extra_data_entries_on_mdb = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) &&
            (endpoint_info.endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained);

    }

    mep_id = loss_ptr->id;

    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
        /*
         * In case it's lm with priorty, delete the session map entry
         */

        DNX_OAM_GET_PRIORITY_FROM_BITMAP(loss_ptr->pkt_pri_bitmap, priority);

        SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get(unit, mep_id, priority, &session_oam_id, &is_exists));
        if (!is_exists)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Loss is not enabled on this endpoint and priority.\n");
        }

        loss_ptr->id = session_oam_id;

        /*
         * Delete loss with priority
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_loss_delete_internal(unit, loss_ptr, mep_id, extra_data_entries_on_mdb));
    }
    else
    {

        /*
         * if loss with priority was alllocated delete all priorities
         */
        loss_with_priority_exists = 0;
        for (priority = 0; priority < 8; priority++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get(unit, mep_id, priority, &session_oam_id, &is_exists));
            if (is_exists)
            {
                /*
                 * Delete loss with priority
                 */
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                                (unit, session_oam_id, loss_params, &loss_with_priority_exists, &lm_stat_exists));
                if (loss_with_priority_exists)
                {
                    loss_ptr->id = session_oam_id;
                    loss_ptr->pkt_pri_bitmap = 1 << priority;
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_loss_delete_internal
                                    (unit, loss_ptr, mep_id, extra_data_entries_on_mdb));
                }
            }
        }
        if (loss_with_priority_exists == 0)
        {
            /*
             * Delete loss without priority
             */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_loss_delete_internal(unit, loss_ptr, mep_id, extra_data_entries_on_mdb));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(loss_params);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the delay information structure
 *          for correct parameters.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_exists - Indicates whether LM entry exists.
 * \param [in] rfc_6374_exists - Indicates whether RFC6374 entry exists.
 * \param [in] lm_db_entry_ptr - Other LM entry's ID.
 * \param [out] priority - Priority value in case of dm with priority is being used
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_delay_add_verify(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_exists,
    uint8 rfc_6374_exists,
    uint32 lm_db_entry_ptr,
    uint8 *priority)
{
    uint32 correct_flags = 0;
    uint8 mpls_pwe_mep = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_oam.oamp.lmm_dmm_supported_get(unit) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Delay add is not supported on this device.");
    }
    /*
     * Delay add is not supported on quarter entry endpoints
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Invalid memory type.\n");

    }

    /*
     * Flags verification
     */
    correct_flags |= BCM_OAM_DELAY_ONE_WAY | BCM_OAM_DELAY_UPDATE | BCM_OAM_DELAY_WITH_ID | BCM_OAM_DELAY_REPORT_MODE;

    if (delay_ptr->flags & ~correct_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal flags set\n");
    }

    if ((ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) && (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: 1DM is not supported on OFFLOADED endpoints\n");
    }

    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * MPLS/PWE endpoints - DA needs to be zero
     */
    if (mpls_pwe_mep)
    {
        if (!BCM_MAC_IS_ZERO(delay_ptr->peer_da_mac_address))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: DA MAC needs to be zero.\n");
        }
    }

    if (delay_ptr->timestamp_format != bcmOAMTimestampFormatIEEE1588v1 && rfc_6374_exists != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only 1588 time stamp format supported for y1731.");
    }

    if (delay_ptr->int_pri || delay_ptr->pkt_pri || delay_ptr->rx_oam_packets || delay_ptr->tx_oam_packets)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fields(INT pri, PKT pri, oam packets) are being set");
    }

    if (delay_ptr->period < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Period below 0 not supported.");
    }

    /*
     * Verify DMM rate   for Jumbo Dm feature
     */
    if (delay_ptr->tlvs[0].tlv_type == bcmOamTlvTypeData)
    {
        /** For Jumbo DM feature: CCM>=3.33ms DMM/DMR>=100ms LMM/LMR>=100ms*/
        if ((delay_ptr->period < DBAL_ENUM_FVAL_CCM_INTERVAL_100MS)
            && (delay_ptr->period != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For Jumbo DM feature: CCM>=3.33ms DMM/DMR>=100ms LMM/LMR>=100ms\n");
        }
    }

    if ((delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) && (delay_ptr->tlvs[0].tlv_type == bcmOamTlvTypeData))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "1DM can't support with Jumb TLV DM");
    }

    /*
     * priority verification for lm with priority feature
     */
    DNX_OAM_GET_PRIORITY_FROM_BITMAP(delay_ptr->pkt_pri_bitmap, *priority)
        /*
         * Call mep db index verify function
         */
        SHR_IF_ERR_EXIT(dnx_oam_lm_dm_add_mep_db_entry_verify(unit, delay_ptr, ccm_mep_db_entry, 1 /* DM */ , lm_exists
                                                              || rfc_6374_exists, lm_db_entry_ptr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill OAMP MEP profile data based on Delay information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP PROFILE content.
 * \param [in] is_rfc_6374_lsp_type - indicates if RFC6374 entry is LSP.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_delay_info_to_mep_profile_data(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data,
    uint8 is_rfc_6374_lsp_type)
{
    uint8 is_jumbo_tlv_dm = 0;
    uint32 new_tx_rate = 0;
    uint8 is_mpls_lm_dm_without_bfd_endpoint;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Fill up with DM params
     */
    mep_profile_data->report_mode_dm = (delay_ptr->flags & BCM_OAM_DELAY_REPORT_MODE) ? 1 : 0;
    mep_profile_data->dm_measurement_type = ((delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) ?
                                             DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4 :
                                             DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_2_WAY);

    if (delay_ptr->tlvs[0].tlv_type == bcmOamTlvTypeData)
    {
        is_jumbo_tlv_dm = 1;
    }

    /*
     * Setting phase count for DMM
     */
    if (ccm_mep_db_entry->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        mep_profile_data->dmm_rate = delay_ptr->period;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, delay_ptr->id, is_jumbo_tlv_dm, (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY), ccm_mep_db_entry->ccm_interval, 0  /* don't
                                                                                                                                                                                         * care
                                                                                                                                                                                         * for
                                                                                                                                                                                         * opcodes
                                                                                                                                                                                         * other
                                                                                                                                                                                         * than CCM 
                                                                                                                                                                                         */ ,
                                                     OAMP_MEP_TX_OPCODE_DMM, mep_profile_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_lmm_dmm_rate_to_rfc6374_rate
                        (unit, delay_ptr->period, _SHR_IS_FLAG_SET(delay_ptr->flags, BCM_OAM_DELAY_UPDATE),
                         1 /** DM entry */ ,
                         mep_profile_data, &new_tx_rate));
        mep_profile_data->dmm_rate = new_tx_rate;

         /** Check if there is no BFD endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, delay_ptr->id, &is_mpls_lm_dm_without_bfd_endpoint));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_rfc_6374_lm_dm
                        (unit, delay_ptr->period, OAMP_MEP_TX_OPCODE_DMM, mep_profile_data,
                         is_mpls_lm_dm_without_bfd_endpoint));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove delay information from OAMP MEP profile data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id  - Mep id
 * \param [in] is_jumbo_tlv_dm  indicate jumbo tlv is config.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP MEP PROFILE content.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_remove_delay_info_from_mep_profile_data(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    int dummy = 0;
    dnx_bfd_tx_period_t tx_rate_data;
    uint8 is_mpls_lm_dm_without_bfd_endpoint;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Remove the DM params
     */
    mep_profile_data->report_mode_dm = 0;
    mep_profile_data->dmm_rate = 0;
    mep_profile_data->dm_measurement_type = 0;
    /*
     * Setting phase count for DMM
     */
    if (ccm_mep_db_entry->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, is_jumbo_tlv_dm ? endpoint_id : 0    /* dont care for
                                                                                                 * opcodes other than
                                                                                                 * CCM */ ,
                                                     is_jumbo_tlv_dm, (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY), ccm_mep_db_entry->ccm_interval, 0       /* don't
                                                                                                                                                                         * care
                                                                                                                                                                         * for
                                                                                                                                                                         * opcodes
                                                                                                                                                                         * other
                                                                                                                                                                         * than CCM 
                                                                                                                                                                         */ ,
                                                     OAMP_MEP_TX_OPCODE_DMM, mep_profile_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, mep_profile_data->dmm_rate, &dummy,
                                                                      &tx_rate_data));
        /** Check if there is no BFD endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, endpoint_id, &is_mpls_lm_dm_without_bfd_endpoint));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_rfc_6374_lm_dm
                        (unit, BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(tx_rate_data.tx_period),
                         OAMP_MEP_TX_OPCODE_DMM, mep_profile_data, is_mpls_lm_dm_without_bfd_endpoint));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate the MEP DB index for DM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to delay information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_db_exists - Whether LM DB already exists or not.
 * \param [in] lm_db_entry_ptr - If exists, what is the LM DB entry ID.
 * \param [in] lm_stat_exists - Whether LM stat already exists or not.
 * \param [in] lm_stat_entry_ptr - If exists, what is the LM stat entry ID.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_delay_mep_id_alloc(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_db_exists,
    uint32 lm_db_entry_ptr,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_ptr)
{
    uint32 entry_id = 0;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;
    uint8 offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!lm_stat_exists)
        {
            /*
             * part2 ptr given by user
             */
            entry_id = delay_ptr->delay_id;
        }       /* Else If stat entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        /*
         * If there is no LM already, use the ID passed by application
         */
        if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_MEP_IS_RF6374)
        {
            entry_id = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(ccm_mep_db_entry->extra_data_header);
        }
        else
        {
            entry_id = delay_ptr->delay_id;
        }
        if (lm_db_exists)
        {

            /*
             * calculate the ID(s) from the DB/STAT entry.
             */
            entry_id = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(lm_db_entry_ptr);
            if (lm_stat_exists)
            {
                entry_id = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(lm_stat_entry_ptr);
            }
        }
    }

    if (entry_id)
    {
        /*
         * Since we pass the ID, mep_id_memory_type info need not be filled.
         */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit,
                                                   DNX_ALGO_RES_ALLOCATE_WITH_ID, &mep_id_memory_type, &entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free up the MEP DB indexes for DM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_id - MEP DB index of the LM DB entry
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_exists - Whether LM already exists or not.
 * \param [in] lm_db_entry_idx - Index of LM_DB entry if exists.
 * \param [in] lm_stat_exists - Whether LM_STAT exists or not.
 * \param [in] lm_stat_entry_idx - Index of LM_STAT entry if exists.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_delay_mep_id_free(
    int unit,
    uint32 delay_id,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_exists,
    uint32 lm_db_entry_idx,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_idx)
{
    uint32 entry_id = 0;
    uint8 offloaded = 0;
    uint32 entry_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!lm_exists)
        {
            /*
             * part2 ptr given by user
             */
            entry_id = delay_id;
        }       /* Else If LM entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        if (ccm_mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
        {
            /** Since the RFC6374 is always first entry, move the pointer to next in chain. */
            entry_index = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(ccm_mep_db_entry->extra_data_header);
        }
        else
        {
            entry_index = MEP_DB_ENTRY_TO_OAM_ID(ccm_mep_db_entry->flex_lm_dm_entry);
        }
        if (lm_exists && (entry_index != lm_db_entry_idx))
        {

            /*
             * DM entry is first.
             * LM_DB entry will be moved to DM entry index.
             * LM_STAT entry (if exists) will be moved to LM_DB entry index.
             */
            if (lm_stat_exists)
            {
                /*
                 * Deallocate LM_STAT entry
                 */
                entry_id = lm_stat_entry_idx;
            }
            else
            {
                /*
                 * Deallocate LM_DB entry
                 */
                entry_id = lm_db_entry_idx;
            }
        }
        else
        {
            /*
             * LM entries are first.
             * Deallocate DM entry
             */
            entry_id = delay_id;
        }
    }

    if (entry_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert Delay structure information to OAMP DM MEP DB
 *          HW contents.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to delay information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP DM MEP DB content
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_delay_info_to_dm_mep_db_hw_write_info(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    uint32 mep_pe_profile_dm = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Offloaded endpoint is true if CCM MEP db entry get says offloaded
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED)
    {
        hw_write_data->offloaded = 1;
    }

    /*
     * CCM OAM id  = delay -> id
     */
    hw_write_data->ccm_oam_id = delay_ptr->id;
    /*
     * stat enable should be true on delay add API call
     */
    hw_write_data->stat_enable = 1;
    hw_write_data->update_mep_db = (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) ? 1 : 0;
    hw_write_data->is_1dm = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) ? 1 : 0;

    if (hw_write_data->is_1dm)
    {
        if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_48B_MAID)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, &mep_pe_profile_dm));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, &mep_pe_profile_dm));
        }

        hw_write_data->mep_pe_profile = (uint8) mep_pe_profile_dm;
    }
    else if (hw_write_data->is_jumbo_dm)
    {
        if (hw_write_data->is_48b_group)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48, &mep_pe_profile_dm));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV, &mep_pe_profile_dm));
        }
        hw_write_data->mep_pe_profile = (uint8) mep_pe_profile_dm;
    }
    else
    {
        hw_write_data->mep_pe_profile = ccm_mep_db_entry->mep_pe_profile;
    }

    /*
     * part_2_ptr is filled if offloaded is true.
     * dm_stat entry is filled if selfcontained is true
     */
    hw_write_data->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(delay_ptr->delay_id);
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_MEP_IS_RF6374)
    {
        hw_write_data->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(ccm_mep_db_entry->extra_data_header);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write OAMP DM MEP DB info
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_write_data - Pointer to the structure,
 *                 containing information about the
 *                 OAMP DM MEP DB content
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_act_on_oamp_dm_mep_db_hw_write_info(
    int unit,
    const dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    dnx_oam_oamp_endpoint_dm_params_t dm_mep_db_write_data = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    dm_mep_db_write_data.flags |=
        hw_write_data->is_1dm ? DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY : DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;

    dm_mep_db_write_data.flags |= (hw_write_data->update_mep_db ? DNX_OAMP_OAM_DM_MEP_UPDATE : 0);

    if (hw_write_data->offloaded)
    {
        /*
         * For offloaded, setting TWO WAY entry is must, since part 2 entry is required.
         */
        dm_mep_db_write_data.flags |= DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    }

    dm_mep_db_write_data.lmm_dmm_da_profile = hw_write_data->oamp_da_lsb_prof;
    dm_mep_db_write_data.mep_profile = hw_write_data->oamp_mep_profile;
    dm_mep_db_write_data.flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(hw_write_data->flex_lm_dm_entry);
    dm_mep_db_write_data.mep_pe_profile = hw_write_data->mep_pe_profile;

    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_set(unit, hw_write_data->ccm_oam_id, &dm_mep_db_write_data));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Add an OAM Delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_add(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    uint8 lm_db_exists = 0, update = 0, exchange_mac_prof = 0, lm_stat_exists = 0, flexibe_da_enabled = 0;
    uint8 mpls_pwe_mep = 0;
    uint32 lm_db_entry_ptr = 0, lm_stat_entry_ptr = 0;
    int counter = 0;
    uint8 priority = 0;
    uint16 data_pattern_lsb = 0, data_pattern_msb = 0;
    uint32 delay_ptr_id = 0;
    int priority_index;

    bcm_mac_t oamp_mac_da_msb = { 0 }, old_oamp_mac_da_msb =
    {
    0};
    bcm_mac_t oamp_mac_da_lsb = { 0 }, old_oamp_mac_da_lsb =
    {
    0};
    uint8 old_oamp_da_lsb_prof = 0;
    uint8 offloaded = 0, dm_exists = 0, rfc_6374_exists = 0;
    uint32 mep_pe_profile = 0, mep_pe_profile_maid_48 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_delay_t), delay_ptr);

    update = (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) ? 1 : 0;

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Delay add is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(delay_ptr->id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Delay add is not supported on unaccelerated endpoints.\n");
    }

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, delay_ptr->id, &endpoint_info));
    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_oam_group_get(unit, endpoint_info.group, &group_info));
    }

    if (IS_OAM_SERVER((&endpoint_info)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Delay add is not supported for OAM server endpoints.\n");
    }

    /*
     * Get the current CCM mep db entry
     */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));

    if (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY)
    {
        if (update)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, &mep_pe_profile));
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, &mep_pe_profile_maid_48));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT, &mep_pe_profile));
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &mep_pe_profile_maid_48));
        }

        if ((ccm_mep_db_entry->mep_pe_profile != ((uint8) mep_pe_profile)) &&
            (ccm_mep_db_entry->mep_pe_profile != ((uint8) mep_pe_profile_maid_48)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: One-way delay is not supported for this endpoint.\n");
        }
    }

    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;
    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info.flags))
    {
        ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_48B_MAID;
    }

    /*
     * Get if LM/DM flexible DMAC is enabled
     */
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, delay_ptr->id, &sw_endpoint_info));
    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        flexibe_da_enabled = (sw_endpoint_info.extra_data_index == DNX_OAM_EXTRA_DATA_HEADER_INVALID) ? 0 : 1;
    }

    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
        /*
         * Verify that loss without priority was not previously defined on this endpoint
         */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                        (unit, delay_ptr->id, lm_mep_db_entry, &lm_db_exists, &lm_stat_exists));
        if (lm_db_exists && !DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Loss/Delay with priority may not be defined on a MEP with Loss/Delay.\n");
        }

        /*
         * Session map entry may exist in two cases:
         * 1. if delay was already configured on this priority
         * 2. if loss was already configured on this priority, and this is update
         */

        DNX_OAM_GET_PRIORITY_FROM_BITMAP(delay_ptr->pkt_pri_bitmap, priority);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get(unit, delay_ptr->id, priority, &delay_ptr_id, &lm_db_exists));
        if (lm_db_exists)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                            (unit, delay_ptr_id, lm_mep_db_entry, &lm_db_exists, &lm_stat_exists));
            delay_ptr->id = delay_ptr_id;
            if (lm_db_exists)
            {
                ccm_mep_db_entry->mep_profile = lm_mep_db_entry->mep_profile;
            }
            else
            {
                /*
                 * Delay is already allocated on this priority
                 */
                if (!update)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss is already allocated on this priority.\n");
                }
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, dm_mep_db_entry, &dm_exists));
                ccm_mep_db_entry->mep_profile = dm_mep_db_entry->mep_profile;
            }
        }
    }
    else
    {
        /*
         * Verify that loss/delay with priority was not previously defined on this endpoint
         */
        for (priority_index = 0; priority_index < 8; priority_index++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get
                            (unit, delay_ptr->id, priority_index, &delay_ptr_id, &lm_db_exists));
            if (lm_db_exists)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Loss/Delay may not be defined on a MEP with Loss/Delay with priority\n");
            }
        }

        /*
         * Check if Loss add is already configured on this endpoint
         */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                        (unit, delay_ptr->id, lm_mep_db_entry, &lm_db_exists, &lm_stat_exists));
    }

    /*
     * LM DB has to exist for LM stat to exist
     */
    if (lm_db_exists)
    {
        DNX_OAM_GET_LOSS_ENTRY_CONFIGURED(offloaded, lm_mep_db_entry, lm_db_entry_ptr);
        if (lm_stat_exists)
        {
            DNX_OAM_GET_LOSS_STAT_ENTRY_CONFIGURED(lm_mep_db_entry, lm_stat_entry_ptr);
        }
    }

    if (DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        rfc_6374_exists = 1;
        hw_write_info->flags |= DNX_OAM_LM_DM_DM_STAT_EXISTS;
        ccm_mep_db_entry->flags |= DNX_OAMP_OAM_MEP_IS_RF6374;
    }

    /*
     * -- SW configurations --
     */
    if (update)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, dm_mep_db_entry, &dm_exists));
        if (!dm_exists)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Delay is not enabled on this endpoint.\n");
        }
    }

    /*
     * Call verify function
     */
    SHR_IF_ERR_EXIT(dnx_oam_delay_add_verify
                    (unit, delay_ptr, ccm_mep_db_entry, lm_db_exists, rfc_6374_exists, lm_db_entry_ptr, &priority));

    /*
     * Verify LM/DM flexible DMAC conflict
     */
    if (flexibe_da_enabled && lm_db_exists)
    {
        SHR_IF_ERR_EXIT(dnx_oam_lmdm_flexible_dmac_conflict_verify
                        (unit, delay_ptr->peer_da_mac_address, sw_endpoint_info.extra_data_index));
    }

    /*
     * Get the current OAMP MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));

    /*
     * If this is an update case or in case Loss was already configured, there will be already a MAC configured.
     * Exchange is true for those 2 cases.
     */
    exchange_mac_prof = (update || (lm_db_exists && !curr_mep_profile_data->piggy_back_lm));

    if (update)
    {
        old_oamp_da_lsb_prof = dm_mep_db_entry->lmm_dmm_da_profile;
    }
    else if (lm_db_exists)
    {
        old_oamp_da_lsb_prof = lm_mep_db_entry->lmm_dmm_da_profile;
    }

    /*
     * For exchange, get the existing mac information
     */
    if (exchange_mac_prof && !mpls_pwe_mep && !flexibe_da_enabled)
    {
        /*
         * Get the current DMM MAC DA MSB
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                     curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                     &counter, &old_oamp_mac_da_msb));

        /*
         * Get the current DMM MAC DA LSB
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                     old_oamp_da_lsb_prof, &counter,
                                                                                     &old_oamp_mac_da_lsb));
    }

    if (!mpls_pwe_mep && !flexibe_da_enabled)
    {
        /*
         * Extract the OUI (MSB) part from the mac address
         */
        GET_MSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address, oamp_mac_da_msb);
        /*
         * Alloc/exchange the MSB profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_alloc(unit, oamp_mac_da_msb,
                                                              old_oamp_mac_da_msb,
                                                              curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                              exchange_mac_prof, hw_write_info));

        /*
         * Extract the NIC (LSB) part from the mac address
         */
        GET_LSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address, oamp_mac_da_lsb);
        /*
         * Alloc/exchange the LSB profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_alloc(unit, oamp_mac_da_lsb,
                                                              old_oamp_mac_da_lsb, old_oamp_da_lsb_prof,
                                                              exchange_mac_prof, hw_write_info));
    }

    /*
     * Fill the current MEP profile with delay params
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_delay_info_to_mep_profile_data
                    (unit, delay_ptr, ccm_mep_db_entry, curr_mep_profile_data,
                     (endpoint_info.type == bcmOAMEndpointTypeMplsLmDmLsp)));

    /*
     * update the current MEP profile with OUI (MSB) Profile
     */
    curr_mep_profile_data->oamp_mac_da_oui_prof = hw_write_info->oamp_da_msb_prof;

    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
        /*
         * Add lm-pcp
         */
        ccm_mep_db_entry->dm_t_flag = TRUE;
        ccm_mep_db_entry->dm_priority = priority;
        hw_write_info->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(delay_ptr->delay_id);
        SHR_IF_ERR_EXIT(dnx_oam_add_loss_delay_pcp
                        (unit, curr_mep_profile_data, hw_write_info, &endpoint_info, ccm_mep_db_entry,
                         &(delay_ptr->id), &(delay_ptr->pm_id), priority, (lm_db_exists || update)));
    }
    else
    {
        /*
         * Exchange the OAMP MEP profile with existing MEP profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                          curr_mep_profile_data, hw_write_info));
    }

    /*
     * If not update, go ahead and alloc mep_db index(es).
     */
    if (!update)
    {
        SHR_IF_ERR_EXIT(dnx_oam_delay_mep_id_alloc(unit, delay_ptr, ccm_mep_db_entry,
                                                   lm_db_exists, lm_db_entry_ptr, lm_stat_exists, lm_stat_entry_ptr));
    }
    /*
     * For jumbo tlv , store TLV data to gen mem. mep_id should be 16*N
     */
    if (delay_ptr->tlvs[0].tlv_type == bcmOamTlvTypeData)
    {
        data_pattern_lsb = delay_ptr->tlvs[0].four_byte_repeatable_pattern & 0xffff;
        data_pattern_msb = (delay_ptr->tlvs[0].four_byte_repeatable_pattern >> 16) & 0xffff;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tlv_data_set
                        (unit, delay_ptr->id, data_pattern_msb, data_pattern_lsb, delay_ptr->tlvs[0].tlv_length));
        hw_write_info->is_jumbo_dm = 1;
        hw_write_info->is_48b_group = (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE) ||
                                       _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE) ||
                                       _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE));
        sw_endpoint_info.sw_state_flags |= DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV;
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_set(unit, (uint32) delay_ptr->id, &sw_endpoint_info, TRUE));

    }
    /*
     * Update the HW write info from SW info for DM MEP DB
     */
    SHR_IF_ERR_EXIT(dnx_oam_delay_info_to_dm_mep_db_hw_write_info(unit, delay_ptr, ccm_mep_db_entry, hw_write_info));

    /*
     * -- HW configurations --
     */

    /*
     * Write the DA MAC msb into register.
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_set(unit,
                                                            hw_write_info->oamp_da_msb_prof,
                                                            hw_write_info->oamp_mac_da_msb));
    }

    /*
     * Write the DA MAC lsb into register.
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_set(unit,
                                                            hw_write_info->oamp_da_lsb_prof,
                                                            hw_write_info->oamp_mac_da_lsb));
    }

    /*
     *  Write the OAMP_MEP_PROFILE (along with DA MAC MSB profile)
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit, hw_write_info->oamp_mep_profile,
                                                     DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info),
                                                     &(hw_write_info->mep_profile_data)));
    }

    /*
     * Write extra data entries along with flexible DMAC
     */
    if (flexibe_da_enabled)
    {
        dnx_oamp_ccm_endpoint_extra_data_t extra_data;
        sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
        extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info.extra_data_index);
        extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE | DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC;
        extra_data.opcodes_to_prepend = DNX_OAMP_EXTRA_DATA_TO_PREPEND_LMM | DNX_OAMP_EXTRA_DATA_TO_PREPEND_DMM;
        sal_memcpy(extra_data.dmac, delay_ptr->peer_da_mac_address, sizeof(bcm_mac_t));
        if (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE))
        {
            extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID20;
            extra_data.opcodes_to_prepend |= DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
            sal_memcpy(extra_data.maid48, group_info.name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        else if (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE))
        {
            extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID40;
            extra_data.opcodes_to_prepend |= DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
            sal_memcpy(extra_data.maid48, group_info.name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        else if (_SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
        {
            extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID48;
            extra_data.opcodes_to_prepend |= DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
            sal_memcpy(extra_data.maid48, group_info.name, BCM_OAM_GROUP_NAME_LENGTH);
        }
        SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_set(unit, &extra_data));
    }

    /*
     * Write the MEP DB entry(ies) (along with DA MAC LSB profile)
     */
    SHR_IF_ERR_EXIT(dnx_oam_act_on_oamp_dm_mep_db_hw_write_info(unit, hw_write_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(lm_mep_db_entry);
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_get(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{
    uint32 oam_id, session_oam_id;
    int priority;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    dnx_oam_oamp_endpoint_dm_params_t *delay_params = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    bcm_mac_t oamp_mac_da_msb = { 0 };
    bcm_mac_t oamp_mac_da_lsb = { 0 };
    uint8 offloaded = 0, dm_exists = 0, flexibe_da_enabled = 0;
    uint8 mpls_pwe_mep = 0;
    int counter = 0;
    int tlv_length = 0;
    uint16 data_pattern_lsb = 0, data_pattern_msb = 0;
    dnx_bfd_tx_period_t tx_rate_data;
    int dummy = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ALLOC_SET_ZERO(delay_params, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Delay is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(delay_ptr->id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Delay is not supported on unaccelerated endpoints.\n");
    }

    /*
     * Get if LM/DM flexible DMAC is enabled
     */
    sal_memset(&sw_endpoint_info, 0, sizeof(dnx_oam_endpoint_info_t));
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, delay_ptr->id, &sw_endpoint_info));
    flexibe_da_enabled = (sw_endpoint_info.extra_data_index == DNX_OAM_EXTRA_DATA_HEADER_INVALID) ? 0 : 1;
    oam_id = delay_ptr->id;

    /*
     * Get lm with priority info
     */
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
        DNX_OAM_GET_PRIORITY_FROM_BITMAP(delay_ptr->pkt_pri_bitmap, priority)
            SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_get(unit, delay_ptr->id, priority, &session_oam_id));

        delay_ptr->id = session_oam_id;
    }

    /*
     * First, read the CCM entry
     */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));
    /*
     * Get the DM MEP DB entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, delay_params, &dm_exists));

    if (!dm_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Delay is not enabled on this endpoint.\n");
    }

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * Get the OAMP MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit,
                                                                 ccm_mep_db_entry->mep_profile,
                                                                 &counter, curr_mep_profile_data));

    if (!mpls_pwe_mep)
    {
        if (!flexibe_da_enabled)
        {
            /*
             * Get the DMM DA OUI
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                         curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                         &counter, &oamp_mac_da_msb));

            /*
             * Get the DMM DA NIC
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                         delay_params->lmm_dmm_da_profile,
                                                                                         &counter, &oamp_mac_da_lsb));

            /*
             * Combined MSB and LSB to form FULL MAC DA
             */
            GET_MAC_FROM_MSB_LSB(oamp_mac_da_msb, oamp_mac_da_lsb, delay_ptr->peer_da_mac_address);
        }
        else
        {
            dnx_oamp_ccm_endpoint_extra_data_t extra_data;
            sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
            extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info.extra_data_index);
            SHR_IF_ERR_EXIT(dnx_oamp_ccm_endpoint_extra_data_get(unit, &extra_data));
            sal_memcpy(delay_ptr->peer_da_mac_address, extra_data.dmac, sizeof(bcm_mac_t));
        }
    }

    /*
     * For jumbo tlv , store TLV data to gen mem. mep_id should be 16*N
     */
    if (sw_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tlv_data_get
                        (unit, delay_ptr->id, &data_pattern_msb, &data_pattern_lsb, &tlv_length));
        if (tlv_length != 0)
        {
            delay_ptr->tlvs[0].tlv_type = bcmOamTlvTypeData;
            delay_ptr->tlvs[0].tlv_length = tlv_length;
            delay_ptr->tlvs[0].four_byte_repeatable_pattern = (data_pattern_msb << 16) | data_pattern_lsb;
        }
    }

    /*
     * Fill the delay pointer
     */
    if (offloaded)
    {
        delay_ptr->delay_id = delay_params->flex_dm_entry;
    }
    else
    {
        delay_ptr->delay_id = delay_params->dm_stat_entry_idx;
    }
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, curr_mep_profile_data->dmm_rate, &dummy,
                                                                      &tx_rate_data));
        tx_rate_data.tx_period ? delay_ptr->period =
            BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(tx_rate_data.tx_period) : 0;
    }
    else
    {
        delay_ptr->period = curr_mep_profile_data->dmm_rate;
    }

    if (!curr_mep_profile_data->report_mode_dm)
    {
        if (delay_params->flags & DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY)
        {
            delay_ptr->delay.nanoseconds =
                (delay_params->last_delay_one_way[0]) | (delay_params->last_delay_one_way[1] << 8) |
                (delay_params->last_delay_one_way[2] << 16) | ((delay_params->last_delay_one_way[3] & 0x3) << 24);
            COMPILER_64_SET(delay_ptr->delay.seconds, 0, ((delay_params->last_delay_one_way[3] & 0x1C) >> 2));

            delay_ptr->delay_max.nanoseconds =
                (delay_params->max_delay_one_way[0]) | (delay_params->
                                                        max_delay_one_way[1] << 8) | (delay_params->max_delay_one_way[2]
                                                                                      << 16) | ((delay_params->
                                                                                                 max_delay_one_way[3] &
                                                                                                 0x3) << 24);
            COMPILER_64_SET(delay_ptr->delay_max.seconds, 0, ((delay_params->max_delay_one_way[3] & 0x1C) >> 2));

            delay_ptr->delay_min.nanoseconds =
                (delay_params->min_delay_one_way[0]) | (delay_params->
                                                        min_delay_one_way[1] << 8) | (delay_params->min_delay_one_way[2]
                                                                                      << 16) | ((delay_params->
                                                                                                 min_delay_one_way[3] &
                                                                                                 0x3) << 24);
            COMPILER_64_SET(delay_ptr->delay_min.seconds, 0, ((delay_params->min_delay_one_way[3] & 0x1C) >> 2));

            delay_ptr->delay_near_end.nanoseconds =
                (delay_params->last_delay_one_way_near_end[0]) | (delay_params->last_delay_one_way_near_end[1] << 8) |
                (delay_params->last_delay_one_way_near_end[2] << 16) |
                ((delay_params->last_delay_one_way_near_end[3] & 0x3) << 24);
            COMPILER_64_SET(delay_ptr->delay_near_end.seconds, 0,
                            ((delay_params->last_delay_one_way_near_end[3] & 0x1C) >> 2));

            delay_ptr->delay_max_near_end.nanoseconds =
                (delay_params->max_delay_one_way_near_end[0]) | (delay_params->max_delay_one_way_near_end[1] << 8) |
                (delay_params->max_delay_one_way_near_end[2] << 16) |
                ((delay_params->max_delay_one_way_near_end[3] & 0x3) << 24);
            COMPILER_64_SET(delay_ptr->delay_max_near_end.seconds, 0,
                            ((delay_params->max_delay_one_way_near_end[3] & 0x1C) >> 2));

            delay_ptr->delay_min_near_end.nanoseconds =
                (delay_params->min_delay_one_way_near_end[0]) | (delay_params->min_delay_one_way_near_end[1] << 8) |
                (delay_params->min_delay_one_way_near_end[2] << 16) |
                ((delay_params->min_delay_one_way_near_end[3] & 0x3) << 24);
            COMPILER_64_SET(delay_ptr->delay_min_near_end.seconds, 0,
                            ((delay_params->min_delay_one_way_near_end[3] & 0x1C) >> 2));

            /*
             * OAMP truncates the 6 lower bits of the nanoseconds.
             * SW must recompensate
             */
            delay_ptr->delay.nanoseconds <<= 6;
            delay_ptr->delay_max.nanoseconds <<= 6;
            delay_ptr->delay_min.nanoseconds <<= 6;
            delay_ptr->delay_near_end.nanoseconds <<= 6;
            delay_ptr->delay_max_near_end.nanoseconds <<= 6;
            delay_ptr->delay_min_near_end.nanoseconds <<= 6;
        }
        else
        {
            delay_ptr->delay.nanoseconds = (delay_params->last_delay[0]) |
                (delay_params->last_delay[1] << 8) |
                (delay_params->last_delay[2] << 16) | ((delay_params->last_delay[3] & 0x3F) << 24);
            COMPILER_64_SET(delay_ptr->delay.seconds, 0, (((delay_params->last_delay[3] & 0xC0) >> 6) |
                                                          (delay_params->last_delay[4] << 2) |
                                                          ((delay_params->last_delay[5] & 0x3) << 10)));
            delay_ptr->delay_max.nanoseconds =
                (delay_params->max_delay[0]) | (delay_params->max_delay[1] << 8) | (delay_params->max_delay[2] << 16) |
                ((delay_params->max_delay[3] & 0x3F) << 24);
            COMPILER_64_SET(delay_ptr->delay_max.seconds, 0,
                            (((delay_params->max_delay[3] & 0xC0) >> 6) | (delay_params->max_delay[4] << 2) |
                             ((delay_params->max_delay[5] & 0x3) << 10)));
            delay_ptr->delay_min.nanoseconds =
                (delay_params->min_delay[0]) | (delay_params->min_delay[1] << 8) | (delay_params->min_delay[2] << 16) |
                ((delay_params->min_delay[3] & 0x3F) << 24);
            COMPILER_64_SET(delay_ptr->delay_min.seconds, 0,
                            (((delay_params->min_delay[3] & 0xC0) >> 6) | (delay_params->min_delay[4] << 2) |
                             ((delay_params->min_delay[5] & 0x3) << 10)));

            /*
             * OAMP truncates the two lower bits of the nanoseconds.
             * SW must recompensate
             */
            delay_ptr->delay.nanoseconds <<= 2;
            delay_ptr->delay_max.nanoseconds <<= 2;
            delay_ptr->delay_min.nanoseconds <<= 2;
        }
    }
    else
    {
        delay_ptr->flags |= BCM_OAM_DELAY_REPORT_MODE;
    }
    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        delay_ptr->timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    }

    if (delay_params->flags & DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY)
    {
        delay_ptr->flags |= BCM_OAM_DELAY_ONE_WAY;
    }
    delay_ptr->id = oam_id;
exit:
    SHR_FREE(delay_params);
    SHR_FREE(curr_mep_profile_data);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Internal function for deleting an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *                 In case it's a delay with priority, delay->id is the
 *                 duplicated ccm entry
 * \param [in] mep_id - The ID of the ccm_entry of the MEP
 * \param [in] extra_data_entries_on_mdb - Existence of extra entries
 *
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_oamp_delay_delete_internal(
    int unit,
    bcm_oam_delay_t * delay_ptr,
    bcm_oam_endpoint_t mep_id,
    uint8 extra_data_entries_on_mdb)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    int priority = 0;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *delay_params = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    uint32 mep_pe_profile_dm = 0;
    uint8 oamp_da_lsb_prof = 0;
    int counter = 0;
    uint8 lm_db_exists = 0, delete_old_prof = 0, mpls_pwe_mep = 0, lm_stat_exists = 0, flexibe_da_enabled = 0;
    uint8 offloaded = 0, dm_exists = 0;
    uint32 mep_pe_profile_1dm_48_maid = 0, mep_pe_profile_1dm = 0, new_mep_pe_profile = 0;
    uint8 is_1dm = 0, is_last_entry = 0, tx_rate_write_delete = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(delay_params, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Check if LM/DM flexible DMAC is enabled
     */
    sal_memset(&sw_endpoint_info, 0, sizeof(dnx_oam_endpoint_info_t));
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, mep_id, &sw_endpoint_info));
    flexibe_da_enabled = (sw_endpoint_info.extra_data_index == DNX_OAM_EXTRA_DATA_HEADER_INVALID) ? 0 : 1;

    /*
     * Get the current CCM mep db entry
     */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        ccm_mep_db_entry->mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * Get the DM MEP DB entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, delay_params, &dm_exists));

    if (!dm_exists)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Delay is not enabled on this endpoint.\n");
    }

    if (offloaded)
    {
        delay_ptr->delay_id = delay_params->flex_dm_entry;
    }
    else
    {
        delay_ptr->delay_id = delay_params->dm_stat_entry_idx;
    }
    oamp_da_lsb_prof = delay_params->lmm_dmm_da_profile;

    /*
     * Get the current OAMP MEP profile
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));

    if (curr_mep_profile_data->dm_measurement_type ==
        DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4)
    {
        is_1dm = TRUE;
    }

    /*
     *  -- SW configurations --
     */

    /*
     * Check if Loss add is already configured on this endpoint
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                    (unit, delay_ptr->id, lm_mep_db_entry, &lm_db_exists, &lm_stat_exists));

    if (!mpls_pwe_mep && !lm_db_exists && !flexibe_da_enabled)
    {
        /*
         * Free the DA MAC LSB profile or (ref_count - 1)
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.free_single(unit,
                                                                                curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                &delete_old_prof));
        /*
         * Free the DA MAC MSB profile or (ref_count - 1)
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.free_single(unit,
                                                                                oamp_da_lsb_prof, &delete_old_prof));
    }

    /** Clear the TX rate profile from the SW */
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.free_single
                        (unit, curr_mep_profile_data->dmm_rate, &tx_rate_write_delete));
    }

    /*
     * Free the OAMP MEP profile from delay params.
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_remove_delay_info_from_mep_profile_data
                    (unit, delay_ptr->id,
                     (sw_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV),
                     ccm_mep_db_entry, curr_mep_profile_data));
    /*
     * In case it's lm with priorty, delete the profile only once
     */

    if (!((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0)))
    {
        /*
         * Exchange the OLD profile and get NEW profile.
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                          curr_mep_profile_data, hw_write_info));
    }
    else
    {
        uint8 last_reference;

        DNX_OAM_GET_PRIORITY_FROM_BITMAP(delay_ptr->pkt_pri_bitmap, priority);

        if (!lm_db_exists)
        {
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_mep_profile.free_single(unit, (int) ccm_mep_db_entry->mep_profile, &last_reference));

            if (last_reference)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_clear(unit, ccm_mep_db_entry->mep_profile));
            }
        }
        else
        {

            /*
             * Exchange the OLD profile and get NEW profile.
             */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                              curr_mep_profile_data, hw_write_info));
        }

        /*
         * For MPLS/PWE - free the push profile
         */
        if (mpls_pwe_mep)
        {
            if (!lm_db_exists)
            {

                uint8 push_profile = ccm_mep_db_entry->push_profile;
                SHR_IF_ERR_EXIT(algo_oam_db.
                                oam_mpls_pwe_exp_ttl_profile.free_single(unit, push_profile, &last_reference));
                if (last_reference)
                {
                /** Set profile parameters */
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_clear(unit, push_profile));
                }
            }

        }
    }

    /*
     * Free up the OAMP MEP ID(s)
     */
    SHR_IF_ERR_EXIT(dnx_oam_delay_mep_id_free(unit, delay_ptr->delay_id, ccm_mep_db_entry,
                                              lm_db_exists, lm_mep_db_entry->lm_db_entry_idx,
                                              lm_stat_exists, lm_mep_db_entry->lm_stat_entry_idx));

    /*
     * ---- HW configurations ---
     */

    /*
     * Write the new OAMP MEP profile if required
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set
                        (unit, hw_write_info->oamp_mep_profile, DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info),
                         &(hw_write_info->mep_profile_data)));
    }

    /*
     * Clear the delay MEP DB entry in HW
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_clear
                    (unit, delay_ptr->id, DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info)));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
    {
        ccm_mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
        /** Clear the TX rate profile from the HW */
        if (tx_rate_write_delete)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_tx_rate_clear(unit, curr_mep_profile_data->dmm_rate));
        }
    }

    /*
     * clear jumbo pe profile and gen memory data
     */
    if ((sw_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV))
    {
        /** clear gen memory */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tlv_data_set(unit, delay_ptr->id, 0, 0, 0));

        /** if 48MAID, recover the mep pe profile*/
        SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, delay_ptr->id, &endpoint_info));
        SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));
        if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info.flags))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &mep_pe_profile_dm));
            ccm_mep_db_entry->mep_pe_profile = mep_pe_profile_dm;
        }
        sw_endpoint_info.sw_state_flags &= (~DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV);
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_set(unit, (uint32) delay_ptr->id, &sw_endpoint_info, TRUE));
    }

    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
        if (!lm_db_exists)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear(unit, delay_ptr->id, extra_data_entries_on_mdb));

            SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_clear(unit, mep_id, priority));

            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, delay_ptr->id));

            if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
            {
                /** Clear RFC_6374 endpoint entry */
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear
                                (unit, MEP_DB_ENTRY_TO_OAM_ID(ccm_mep_db_entry->flex_lm_dm_entry), 0));
                SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc
                                (unit, MEP_DB_ENTRY_TO_OAM_ID(ccm_mep_db_entry->flex_lm_dm_entry)));
            }
        }
        else
        {
            /*
             * Update the CCM mep db entry with new OAMP MEP profile
             */
            ccm_mep_db_entry->mep_profile = hw_write_info->oamp_mep_profile;

            /*
             * Update the CCM mep db entry with new OAMP MEP PE profile
             * in case one-way delay was deleted
             */
            if (is_1dm)
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, &mep_pe_profile_1dm_48_maid));

                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, &mep_pe_profile_1dm));

                if (ccm_mep_db_entry->mep_pe_profile == ((uint8) mep_pe_profile_1dm_48_maid))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &new_mep_pe_profile));
                }
                else if (ccm_mep_db_entry->mep_pe_profile == ((uint8) mep_pe_profile_1dm))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT, &new_mep_pe_profile));
                }

                ccm_mep_db_entry->mep_pe_profile = (uint8) new_mep_pe_profile;
            }
            ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_UPDATE;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, delay_ptr->id, ccm_mep_db_entry));
        }
    }
    else
    {
        /*
         * Update the CCM mep db entry with new OAMP MEP profile
         */
        ccm_mep_db_entry->mep_profile = hw_write_info->oamp_mep_profile;

        /*
         * Update the CCM mep db entry with new OAMP MEP PE profile
         * in case one-way delay was deleted
         */
        if (is_1dm)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, &mep_pe_profile_1dm_48_maid));

            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, &mep_pe_profile_1dm));

            if (ccm_mep_db_entry->mep_pe_profile == ((uint8) mep_pe_profile_1dm_48_maid))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &new_mep_pe_profile));
            }
            else if (ccm_mep_db_entry->mep_pe_profile == ((uint8) mep_pe_profile_1dm))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT, &new_mep_pe_profile));
            }

            ccm_mep_db_entry->mep_pe_profile = (uint8) new_mep_pe_profile;
        }
        ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_UPDATE;
        if (DNX_OAM_IS_ENDPOINT_RFC6374(&sw_endpoint_info))
        {
            if (!lm_db_exists)
            {
                is_last_entry = TRUE;
            }
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_rfc_6374_endpoint_set
                            (unit, delay_ptr->id, ccm_mep_db_entry, is_last_entry, TRUE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, delay_ptr->id, ccm_mep_db_entry));
        }
    }

exit:
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(delay_params);
    SHR_FREE(lm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_delete(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    uint8 extra_data_entries_on_mdb = 0;
    int priority = 0;
    bcm_oam_endpoint_t mep_id = 0;
    uint32 session_oam_id;
    uint8 delay_with_priority_exists;
    uint8 is_exists;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_delay_t), delay_ptr);

    /*
     * Delay is not supported on unaccelerated endpoints
     */
    if (DNX_OAM_MEP_IS_UNACCELERATED(delay_ptr->id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Delay is not supported on unaccelerated endpoints.\n");
    }

    mep_id = delay_ptr->id;

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, delay_ptr->id, &endpoint_info));

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));
        extra_data_entries_on_mdb = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) &&
            (endpoint_info.endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained);
    }

    /*
     * In case it's lm with priorty, delete the session map entry
     */
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
        DNX_OAM_GET_PRIORITY_FROM_BITMAP(delay_ptr->pkt_pri_bitmap, priority);

        SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get(unit, mep_id, priority, &session_oam_id, &is_exists));
        if (!is_exists)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Loss is not enabled on this endpoint and priority.\n");
        }

        delay_ptr->id = session_oam_id;
        /*
         * Delete loss with priority
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_delay_delete_internal(unit, delay_ptr, mep_id, extra_data_entries_on_mdb));
    }
    else
    {

        /*
         * if loss with priority was alllocated delete all priorities
         */
        delay_with_priority_exists = 0;
        for (priority = 0; priority < 8; priority++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_dm_session_get(unit, mep_id, priority, &session_oam_id, &is_exists));
            if (is_exists)
            {
                /*
                 * Delete loss with priority
                 */
                delay_with_priority_exists |= is_exists;
                delay_ptr->id = session_oam_id;
                delay_ptr->pkt_pri_bitmap = 1 << priority;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_delay_delete_internal(unit, delay_ptr, mep_id, extra_data_entries_on_mdb));
            }
        }
        if (delay_with_priority_exists == 0)
        {
            /*
             * Delete loss without priority
             */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_delay_delete_internal(unit, delay_ptr, mep_id, extra_data_entries_on_mdb));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
