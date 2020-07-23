/** \file oam_group.c
 * 
 *
 * OAM group procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* 
        */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/*
 * 
Include files.
{

 */
#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm/types.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>

#include <soc/dnx/dnx_err_recovery_manager.h>

/** 
 *  This struct is used to store data temporarily for the
 *  process of writing OAM group ICC data into HW
 *  Any change in a OAM group is spread across several
 *  functions.  This structure is used to save data that is
 *  determined in one function, to be applied in a subsequent
 *  stage of the creating/modifying/reading/deleting process.
 */
typedef struct
{
    int profile_index;
    dnx_oam_group_icc_profile_data_t icc_profile_data;
    uint8 first_or_last_reference;
} dnx_oam_group_temp_data_t;

shr_error_e
dnx_oam_group_info_get(
    int unit,
    bcm_oam_group_t group,
    bcm_oam_group_info_t * group_info)
{
    uint32 entry_handle_id;
    uint8 maid_name[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    uint8 remote_maid_name[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(group_info, _SHR_E_PARAM, "group info");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, group);

    /** Setting pointers value to receive the fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** get MAID name */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_NAME, INST_SINGLE, maid_name));

    /** get remote MAID name */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_RX_NAME, INST_SINGLE, remote_maid_name));

    /** get current flags */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FLAGS, INST_SINGLE, (&group_info->flags)));

    /** get group name index */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_GROUP_NAME_INDEX, INST_SINGLE,
                     (uint32 *) (&group_info->group_name_index)));

    /** set group id */
    group_info->id = group;

    /** set group name */
    sal_memcpy(group_info->name, maid_name, sizeof(group_info->name));

    /** set remote group */
    sal_memcpy(group_info->rx_name, remote_maid_name, sizeof(group_info->rx_name));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validates OAM group ICC information 
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_info- Pointer to
 *   the structure in which we store information about the
 *   group_info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See
 * \remark
 *   * None
 * \see 
 *  * None
 */
static shr_error_e
dnx_oam_group_long_format_data_verify(
    int unit,
    const bcm_oam_group_info_t * group_info)
{
    uint8 temp_group_name[BCM_OAM_GROUP_NAME_LENGTH];
    SHR_FUNC_INIT_VARS(unit);

    switch (group_info->name[0])
    {   /* MD Format */
        case DNX_OAMP_OAM_GROUP_MD_FORMAT:
            /*
             * Two formats supported: short MA name: {1,3,2,{two byte name},0,0,...,0} and long ICC based:
             * {1,32,13,{ICC data-6bytes},{UMC data - 5 bytes},{two byte name},0,0...0}
             */
            switch (group_info->name[1])
            {
                case DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_SHORT:  /* MA_NAME_FORMAT SHORT */
                    if (group_info->name[2] != 2)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error: Error in group name. Unsupported length: %d. Short name format supports only length 2\n",
                                     group_info->name[2]);
                    }
                    break;
                case DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_ICC:    /* MA_NAME_FORMAT ICC_BASED */
                    if (group_info->name[2] != 13)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error: Error in group name. Unsupported length: %d. Long name format supports only length 13.\n",
                                     group_info->name[2]);
                    }
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error:  Unsupported group name format: %d.\n", group_info->name[0]);
                    break;
            }

            break;
        case DNX_OAM_GROUP_NON_ACCELERATED_MD_FORMAT:
            sal_memset(temp_group_name, 0, BCM_OAM_GROUP_NAME_LENGTH);
            if (sal_memcmp(group_info->name, temp_group_name, sizeof(group_info->name)) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error:   Error in group name.Unsupported group name format: %d.\n",
                             group_info->name[0]);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error:   Error in group name.Unsupported group name format: %d.\n",
                         group_info->name[0]);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function writes the ICC profile data into OAMP HW register
 * \param [in] unit - Number of hardware unit used.
 * \param [in, out] hw_write_data-
 *  pointer to HW data to write/clear
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_hw_write_icc_profile_data(
    int unit,
    const dnx_oam_group_temp_data_t * hw_write_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * If this is the first/last time this profile is allocated/deallocated write it into the HW table.
     */
    if (hw_write_data->first_or_last_reference)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ICC_MAP, &entry_handle_id));
        /*
         * Key
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ICC_MAP_INDEX,
                                   (uint32) hw_write_data->profile_index);
        /*
         * Values
         */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_ICC_DATA, INST_SINGLE,
                                        (uint8 *) (&hw_write_data->icc_profile_data));
        /*
         * Perform the commit 
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate and set the OAM group ICC info in ICC MAP register
 *   Allocates/Deallocates an ICC profile index and use it for HW
 *   register write/clear
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group-
 *  ID of the OAM group
 * \param [in] create-
 *  group create/delete
 * \param [in, out] hw_write_data-
 *  pointer to HW data to write/clear
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See
 * \remark
 *   * None
 * \see 
 *  * None
 */
static shr_error_e
dnx_oam_group_handle_sw_icc_data(
    int unit,
    bcm_oam_group_t group,
    uint8 create,
    dnx_oam_group_temp_data_t * hw_write_data)
{
    dnx_oam_ma_name_type_e name_type;
    dnx_oam_group_icc_profile_data_t icc_profile_data;
    int profile_index, i;
    uint8 first_reference = 0, last_reference = 0;
    bcm_oam_group_info_t group_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get group info from sw table */
    SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, group, &group_info));

    sal_memset(icc_profile_data, 0, sizeof(dnx_oam_group_icc_profile_data_t));

    name_type = BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_info.name);

    if (name_type == DNX_OAMP_OAM_MA_NAME_TYPE_ICC)
    {
        for (i = 0; i < DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH; i++)
        {
            icc_profile_data[i] =
                group_info.name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET + (DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH - 1) - i];
        }
        if (create)
        {

            SHR_IF_ERR_EXIT(algo_oam_db.oam_group_icc_profile.allocate_single
                            (unit, 0, icc_profile_data, NULL, &profile_index, &first_reference));
            hw_write_data->profile_index = profile_index;
            sal_memcpy(hw_write_data->icc_profile_data, icc_profile_data, sizeof(dnx_oam_group_icc_profile_data_t));
            hw_write_data->first_or_last_reference = first_reference;
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_oam_db.oam_group_icc_profile.profile_get(unit, icc_profile_data, &profile_index));
            SHR_IF_ERR_EXIT(algo_oam_db.oam_group_icc_profile.free_single(unit, profile_index, &last_reference));
            hw_write_data->profile_index = profile_index;
            hw_write_data->first_or_last_reference = last_reference;
            sal_memset(icc_profile_data, 0, sizeof(dnx_oam_group_icc_profile_data_t));
            sal_memcpy(hw_write_data->icc_profile_data, icc_profile_data, sizeof(dnx_oam_group_icc_profile_data_t));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function calculates the 16-bit CRC for
 *   a group with a 48B MAID according to that
 *   group's parameters
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] group_info  -
 *   group parameters.
 * \param [out] calculated_crc-
 *  resulting 16-bit CRC
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *  * None
 */
static shr_error_e
dnx_oam_group_calculate_crc16(
    int unit,
    const bcm_oam_group_info_t * group_info,
    uint16 *calculated_crc)
{
    const uint8 *rx_maid;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_RX_NAME))
    {
        rx_maid = group_info->rx_name;
    }
    else
    {
        rx_maid = group_info->name;
    }

    /** Calculate CRC */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) rx_maid, DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE        /* start
                                                                                                                                 * index */ ,
                                       BCM_OAM_GROUP_NAME_LENGTH /* data size */ ,
                                       calculated_crc));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   set the OAM group info/entry of a group in SW table
 *   according to group_info->id.
  * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_info- Pointer to
 *   the structure in which we store information about the
 *   group_info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See
 * \remark
 *   * None
 * \see 
 *  * None
 */
static shr_error_e
dnx_oam_group_info_store(
    int unit,
    bcm_oam_group_info_t * group_info)
{
    uint32 entry_handle_id;
    uint16 calculated_crc = 0;
    dbal_entry_action_flags_e commit_flag = DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(group_info, _SHR_E_PARAM, "group info");

    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        /** Calculate the 16-bit CRC */
        dnx_oam_group_calculate_crc16(unit, group_info, &calculated_crc);
    }

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_REPLACE) != 0)
    {
        commit_flag = DBAL_COMMIT_UPDATE;
    }

    /** Taking a handle of  OAM_GROUP_INFO table  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    /** Setting key field according to group_info->id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, group_info->id);

    /** Setting the group name(maid)  */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_NAME, INST_SINGLE, group_info->name);

    /** Setting the rx group name(maid) */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_RX_NAME, INST_SINGLE, group_info->rx_name);

    /** Setting the present group flags */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLAGS, INST_SINGLE, group_info->flags);

    /** Setting the 16-bit CRC */
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RX_CRC, INST_SINGLE, calculated_crc);

    /** Setting the group name index in oamp mep db */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_NAME_INDEX, INST_SINGLE,
                                 group_info->group_name_index);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_flag));

    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags) &&
        (group_info->group_name_index != 0) && (group_info->group_name_index != -1))
    {
        /** Valid group_name_index value */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_48b_maid_set(unit, group_info, calculated_crc));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   verify the input of group_info.
 *   Currently only BCM_OAM_GROUP_WITH_ID flag is supported.
 *   The function also checks that if flag is
 *   BCM_OAM_GROUP_WITH_ID present than the group ID is not
 *   alocated.
 *   The function will return Zero if no error was detected,
 *   Negative if error was detected.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_info- Pointer to
 *   the structure in which we verify information about the
 *   group_info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
static shr_error_e
dnx_oam_group_create_verify(
    int unit,
    bcm_oam_group_info_t * group_info)
{
    int legal_flags = 0;
    uint8 is_allocated = 0;
    uint32 mep_db_threshold = 0;
    int short_entries_in_full;
    bcm_oam_group_info_t *existing_group_info = NULL;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(group_info, _SHR_E_PARAM, "group info");

    /*
     * currently BCM_OAM_GROUP_REPLACE, BCM_OAM_GROUP_WITH_ID, BCM_OAM_GROUP_RX_NAME
     * and BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE are the only supported flags.
     * Return error for any other flag configuration
     */
    legal_flags |= BCM_OAM_GROUP_WITH_ID;
    legal_flags |= BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE;
    legal_flags |= BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE;
    legal_flags |= BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
    legal_flags |= BCM_OAM_GROUP_RX_NAME;
    legal_flags |= BCM_OAM_GROUP_REPLACE;
    if (group_info->flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination\n");
    }

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_REPLACE))
    {
        if ((_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_WITH_ID) == 0) ||
            !DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Error: Only existing groups with 48 byte MAIDs can be renamed\n");
        }
    }

    /** check allocation if BCM_OAM_GROUP_WITH_ID is present */
    if (group_info->flags & BCM_OAM_GROUP_WITH_ID)
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oam_group_id.is_allocated(unit, group_info->id, &is_allocated));
        if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_REPLACE))
        {
            if (!is_allocated)
            {
                /** Must be true for group rename */
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Group with id %d does not exist\n", group_info->id);
            }
            SHR_ALLOC_SET_ZERO(existing_group_info, sizeof(bcm_oam_group_info_t),
                               "Current data about group, needed for comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, group_info->id, existing_group_info));
            if (existing_group_info->group_name_index == 0)
            {
                /** Can't add extra data entries to an existing group */
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: groups with group_name_index=0 cannot be modified.");
            }

            if (group_info->group_name_index != existing_group_info->group_name_index)
            {
                /** Extra data entries must stay in the same place */
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: extra_data_index may not be modified.");
            }
        }
        else
        {
            /** if group ID already exists, return error*/
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "Error: Group with id %d exists\n", group_info->id);
            }
        }
    }

    if (!DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        /** for 48B MAID, there no rules.  Any 48 bytes are legal */
        SHR_IF_ERR_EXIT(dnx_oam_group_long_format_data_verify(unit, group_info));
    }
    else
    {
        short_entries_in_full = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if (group_info->group_name_index % short_entries_in_full != 0)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Error: Pointer to extended data %d should be multiple of 4\n",
                         group_info->group_name_index);
        }

        mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
        if ((group_info->group_name_index != 0) &&
            (group_info->group_name_index != -1) && (mep_db_threshold > group_info->group_name_index))
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS,
                         "Error: Pointer to extended data %d should be above the full entry threshold %d \n",
                         group_info->group_name_index, mep_db_threshold);
        }

    }

exit:
    SHR_FREE(existing_group_info);
    SHR_FUNC_EXIT;
}

/*
 * See oam_internal.h for details
 */
shr_error_e
dnx_oam_group_verify_is_allocated(
    int unit,
    bcm_oam_group_t group)
{

    uint8 is_allocated = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** check group info is allocated/exsits */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_group_id.is_allocated(unit, group, &is_allocated));

    /** return error if the group id does not exist */
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error: Group with id %d does not exist\n", group);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Clear oam group entry in SW table, meaning set all fields
 *   in entry to be zero, entry number to delete accoridng to
 *   group_id input.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_id- 
 *  ID of the OAM group to get
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. 
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_group_clear_entry(
    int unit,
    int group_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    /** Setting key fields according to group id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, group_id);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Create an OAM group according to the info in group_info
 *  structure. Group ID will be according to group_info->id in
 *  case BCM_OAM_GROUP_WITH_ID is present or will be allocated
 *  internally in case flag is not present,
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_info- 
 *   Pointer to the structure in which we have information about
 *   the group_info.
 *   group_info->flags - OAM group flags
 *     BCM_OAM_GROUP_REPLACE(0x1) - currently not supported
 *     BCM_OAM_GROUP_WITH_ID(0x2) - supported
 *     BCM_OAM_GROUP_REMOTE_DEFECT_TX(0x4) - not supported
 *     BCM_OAM_GROUP_COPY_ERRORS_TO_CPU(0x8) - not supported
 *     BCM_OAM_GROUP_COPY_TO_CPU(0x10) - not supported
 *     BCM_OAM_GROUP_GET_FAULTS_ONLY(0x20) - not supported
 *     BCM_OAM_GROUP_TYPE_BHH(0x40) - not supported
 *     BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE(0x80) - supported
 *     BCM_OAM_GROUP_RX_NAME(0x1000) - supported
 *   group_info->id - group id
 *   group_info->name - The Maintenance Association ID(MAID)
 *   group_info->group_name_index - Index to group name in
 *   MEP-DB
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * Currently create only sets group info in SW state
 * \see
 *   * bcm_dnx_oam_group_destroy
 *   * bcm_dnx_oam_group_destroy_all
 *   * bcm_dnx_oam_group_get
 *   * bcm_dnx_oam_group_traverse
*/
shr_error_e
bcm_dnx_oam_group_create(
    int unit,
    bcm_oam_group_info_t * group_info)
{
    int alloc_flags = 0;
    dnx_oam_group_temp_data_t *hw_write_data = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_oam_group_create_verify(unit, group_info));

    SHR_ALLOC_SET_ZERO(hw_write_data, sizeof(dnx_oam_group_temp_data_t), "Data saved to write to hard-logic tables",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_REPLACE) == 0)
    {
        /*
         * in case BCM_OAM_GROUP_WITH_ID flag exists, allocate group id, according to group_info->id
         */
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID)
        {
            alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }

        /** allocate the group ID */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_group_id.allocate_single(unit, alloc_flags, NULL, &group_info->id));
    }

    /** store group info in SW state table */
    SHR_IF_ERR_EXIT(dnx_oam_group_info_store(unit, group_info));

    /** allocate a SW template entry to ICCMap register that holds Bytes 5:0 of group name. */
    if (!DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        SHR_IF_ERR_EXIT(dnx_oam_group_handle_sw_icc_data
                        (unit, group_info->id, DNX_OAMP_OAM_GROUP_CREATE, hw_write_data));
    }

    /** Set HW ICCMap register data that holds Bytes 5:0 of group name */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_hw_write_icc_profile_data(unit, hw_write_data));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroy an OAM group according to group id input
 *  clear entry in SW table and free group ID in allocation manager
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group- 
 *  ID of the OAM group to destroy
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_group_create
 *   * bcm_dnx_oam_group_destroy_all
 *   * bcm_dnx_oam_group_get
 *   * bcm_dnx_oam_group_traverse
*/
shr_error_e
bcm_dnx_oam_group_destroy(
    int unit,
    bcm_oam_group_t group)
{
    dnx_oam_group_temp_data_t *hw_delete_data = NULL;
    bcm_oam_group_info_t *group_info = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ALLOC_SET_ZERO(hw_delete_data, sizeof(dnx_oam_group_temp_data_t), "Data saved to write to hard-logic tables",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t), "Data about group, needed for 48B MAID",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, group, group_info));
    if ((group_info->group_name_index != 0) && (group_info->group_name_index != -1))
    {
        /** Valid group_name_index value */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_48b_maid_clear(unit, group_info->group_name_index));
    }

    /** verify group info is allocated/exsits */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_group_verify_is_allocated(unit, group));

    /** destroy all endpoint in group */
    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_destroy_all(unit, group));

    /** deallocate a SW template entry from ICCMap register that holds Bytes 5:0 of group name */
    if (!DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        SHR_IF_ERR_EXIT(dnx_oam_group_handle_sw_icc_data(unit, group, DNX_OAMP_OAM_GROUP_DESTROY, hw_delete_data));
    }

    /** Clear HW ICCMap register data that holds Bytes 5:0 of group name */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_hw_write_icc_profile_data(unit, hw_delete_data));

    /** clear group from SW state table */
    SHR_IF_ERR_EXIT(dnx_oam_group_clear_entry(unit, group));

    /** free group id in allocation manager */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_group_id.free_single(unit, group));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_delete_data);
    SHR_FREE(group_info);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destroy all OAM groups.
*  This function creates an iterator for the OAM group SW
*  state table, then uses bcm_dnx_oam_group_destroy on each
*  group in that table.
*
 * \param [in] unit - Relevant unit.
*
* \return
*   * Error indication based on enum shr_error_e
*
* \remark
*   * None
*
* \see
*   * \ref bcm_dnx_oam_group_create
*   * \ref bcm_dnx_oam_group_destroy
*   * \ref bcm_dnx_oam_group_get
*   * \ref bcm_dnx_oam_group_traverse
*/
shr_error_e
bcm_dnx_oam_group_destroy_all(
    int unit)
{

    uint32 entry_handle_id, group_id;
    int is_end;
    uint32 *field_val;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    /** Allocate array for reading key */
    field_val = NULL;
    SHR_ALLOC(field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS * sizeof(uint32),
              "Array for reading profile table entry key", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Create iterator */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /** Read first entry */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /** Loop over all entries in table */
    while (!is_end)
    {
        /** Get group ID from the entry key */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, field_val));
        group_id = field_val[0];

        /** Use the endpoint ID to destroy the endpoint */
        SHR_IF_ERR_EXIT(bcm_dnx_oam_group_destroy(unit, group_id));

        /** Read next entry */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get an OAM group according to group id input.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group- 
 *  ID of the OAM group to get
 * \param [in] group_info- Pointer to
 *   the structure in which we store information about the
 *   group_info.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_oam_group_create
 *   * bcm_dnx_oam_group_destroy
 *   * bcm_dnx_oam_group_destroy_all
 *   * bcm_dnx_oam_group_traverse
*/
shr_error_e
bcm_dnx_oam_group_get(
    int unit,
    bcm_oam_group_t group,
    bcm_oam_group_info_t * group_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify group info is allocated/exsits */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_group_verify_is_allocated(unit, group));

    /** get group info from sw table */
    SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, group, group_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Traverse the set of OAM groups and call a
*  function with the data for each entry found.
* \param [in] unit -  The unit number.
* \param [in] cb - the name of the callback function that is
*        going to be called for each valid entry
* \param [in] user_data - data that is passed to the callback
 *       function
* \return
*   * Error indication based on enum shr_error_e
* \remark
*   * None
* \see
*   * bcm_dnx_oam_group_create
*   * bcm_dnx_oam_group_get
*   * bcm_dnx_oam_group_destroy
*   * bcm_dnx_oam_group_destroy_all
*/
shr_error_e
bcm_dnx_oam_group_traverse(
    int unit,
    bcm_oam_group_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;

    uint32 *field_value;
    int is_end;
    bcm_oam_group_info_t dbal_entry;
    uint8 maid_name[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
    uint8 remote_maid_name[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Allocate array for reading key */
    field_value = NULL;
    SHR_ALLOC(field_value, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS * sizeof(uint32),
              "Array for reading profile table entry key", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    /** Create iterator */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        bcm_oam_group_info_t_init(&dbal_entry);

        /*
         * Receive key and value fields of the entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, field_value));
        dbal_entry.id = field_value[0];

        /** get MAID name */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_NAME, INST_SINGLE, maid_name));
        /** set group name */
        sal_memcpy(dbal_entry.name, maid_name, sizeof(dbal_entry.name));

        /** get remote MAID name */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_RX_NAME, INST_SINGLE, remote_maid_name));
        /** set remote group */
        sal_memcpy(dbal_entry.rx_name, remote_maid_name, sizeof(dbal_entry.rx_name));

        /** get current flags */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FLAGS, INST_SINGLE, (&dbal_entry.flags)));

        /** get group name index */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GROUP_NAME_INDEX, INST_SINGLE,
                         (uint32 *) (&dbal_entry.group_name_index)));

        /*
         * If user provided a name of the callback function, it will be invoked with sending the dbal_entry structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &dbal_entry, user_data));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong callback function was provided");
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    SHR_FREE(field_value);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
