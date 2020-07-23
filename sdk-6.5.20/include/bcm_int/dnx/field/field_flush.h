/**
 * \file bcm_int/dnx/field/field_flush.h
 *
 *
 * Field Flush definitions for database for DNX
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_FLUSH_H_INCLUDED
/* { */
#define FIELD_FIELD_FLUSH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Include files
 * {
 */

/*
 * }
 */
/*
 * Defines
 * {
 */

/**
 * The size of the exem entry in the flush machine.
 */
#define DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY \
        ((DNX_DATA_MAX_FIELD_EXEM_LEARN_FLUSH_MACHINE_LEXEM_ENTRY_MAX_SIZE + SAL_UINT32_NOF_BITS - 1) / SAL_UINT32_NOF_BITS)

/*
 * }
 */
/*
 * Typedefs
 * {
 */

/**
 * Field flush machine for EXEM learning entry ID, indicating entry position on the TCAM.
 */
typedef int dnx_field_flush_entry_id_t;

/**
 * Holds what to qualify upon in the qualifier of the EXEM entry for the flush machine entry.
 */
typedef struct
{
    /** Dnx qualifier */
    dnx_field_qual_t dnx_qual;
    /*
     * The value to be compared against
     */
    uint32 qual_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    /*
     * The mask for what bits are to be compared
     */
    uint32 qual_mask[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];

} dnx_field_flush_entry_key_qual_t;

/**
 * Holds what to qualify upon in the action of the EXEM entry for the flush machine entry.
 */
typedef struct
{
    /** Dnx action */
    dnx_field_action_t dnx_action;
    /*
     * The value to be compared against
     */
    uint32 action_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
    /*
     * The mask for what bits are to be compared
     */
    uint32 action_mask[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
    /*
     * The value of the valid bit to be compared (for actions that have a valid bit).
     */
    uint8 valid_bit_value;
    /*
     * Whether to check the valid bit (for actions that have valid bit).
     */
    uint8 valid_bit_mask;

} dnx_field_flush_entry_key_action_t;

/**
 * Holds the in params for adding a TCAM entry to the EXEM learn flush machine,
 * the values to be qualified upon
 */
typedef struct
{
    /*
     *  The array of qualifiers from the key of the EXEM entry we qualify upon to
     *  check in the flush entry.
     */
    dnx_field_flush_entry_key_qual_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

    /*
     *  The array of actions from the payload of the EXEM entry we qualify upon to
     *  check in the flush entry.
     */
    dnx_field_flush_entry_key_action_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    /*
     *  The value to expect from the hit bit
     */
    uint8 hit_bit_value;

    /*
     *  Whether to check the hit bit
     */
    uint8 hit_bit_mask;

} dnx_field_flush_entry_key_t;

/**
 * Holds information should be changed for the action the payload of the EXEM entry if the
 * flush machine entry hits.
 */
typedef struct
{
    /** Dnx action */
    dnx_field_action_t dnx_action;
    /*
     * The value to be written to the action
     */
    uint32 action_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
    /*
     * The mask for what bits to be changed
     */
    uint32 action_altering_mask[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
    /*
     * The value of the valid bit to be written(for actions that have a valid bit).
     */
    uint8 valid_bit_value;
    /*
     * Whether to change the valid bit (for actions that have valid bit).
     */
    uint8 valid_bit_mask;

} dnx_field_flush_entry_payload_altering_action_t;

/**
 * Holds the in params for adding a TCAM entry to the EXEM learn flush machine,
 * the values to be qualified upon
 */
typedef struct
{
    /*
     * Whether to clear the hit bit of the EXEM entry.
     */
    uint8 hit_bit_clear;
    /*
     * Whether to delete the EXEM entry.
     */
    uint8 delete;
    /*
     * Whether to edit the EXEM entry.
     */
    uint8 transplant;
    /*
     *  The array of actions from the payload of the EXEM entry we change if the flush entry hits.
     */
    dnx_field_flush_entry_key_action_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

} dnx_field_flush_entry_payload_t;

/**
 * Holds the in params for adding a TCAM entry to the EXEM learn flush machine
 */
typedef struct
{
    /** Hold the What should be qualified upon in the TCAM (including the full EXEM entry).*/
    dnx_field_flush_entry_key_t key_info;

    /** Hold The changes to the payload and commands to be performed if the entry is hit.*/
    dnx_field_flush_entry_payload_t payload_info;

} dnx_field_flush_entry_t;

/*
 * } Types
 */

/**
 * \brief - Configures the given EXEM index with the given scan 
 *        period.
 *
 * \param [in] unit              - Relevant unit.
 * \param [in] exem_index     - exem index to configure.
 * \param [in] scan_period_us  - the scan period in uSec.
 *
 * \remark
 *   None
 * \see
 *
 *   * None
 */
shr_error_e dnx_switch_control_exem_scan_period_set(
    int unit,
    int exem_index,
    uint32 scan_period_us);

/**
 * \brief - Retrieves EXEM scan period. 
 *
 * \param [in] unit              - Relevant unit.
 * \param [in] exem_index     - exem index to retrive its scan 
 *        period.
 * \param [out] scan_period_us  - the scan period in uSec.
 *
 * \remark
 *   None
 * \see
 *
 *   * None
 */
shr_error_e dnx_switch_control_exem_scan_period_get(
    int unit,
    int exem_index,
    uint32 *scan_period_us);

/**
 * \brief - Configures the given EXEM flush profile payload offset 
 *        period.
 *
 * \param [in] unit              - Relevant unit.
 * \param [in] exem_index     - exem index to configure.
 * \param [in] payload_offset  - the flush profile payload offset.
 *
 * \remark
 *   None
 * \see
 *
 *   * None
 */
shr_error_e dnx_switch_control_exem_flush_profile_payload_offset_set(
    int unit,
    int exem_index,
    uint32 payload_offset);

/**
 * \brief - Retrieves EXEM flush profile payload offset. 
 *
 * \param [in] unit              - Relevant unit.
 * \param [in] exem_index     - exem index to retrive its scan 
 *        period.
 * \param [out] payload_offset  - the flush profile payload offset.
 *
 * \remark
 *   None
 * \see
 *
 *   * None
 */
shr_error_e dnx_switch_control_exem_flush_profile_payload_offset_get(
    int unit,
    int exem_index,
    uint32 *payload_offset);

/**
 * \brief
 *  Init structure type of dnx_field_flush_entry_t.
 * \param [in] unit                - Device ID.
 * \param [in] flush_entry_info_p  - Pointer to structure type of dnx_field_flush_entry_t.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_flush_entry_t_init(
    int unit,
    dnx_field_flush_entry_t * flush_entry_info_p);

/**
 * \brief
 *  Add an entry to the TCAM of the flush machine for EXEM learn.
 * \param [in] unit                - Device ID.
 * \param [in] flags               - The flags for the entry add.
 * \param [in] fg_id               - The EXEM field group upon which we qualify.
 * \param [in] entry_id            - The number of the entry in the TCAM.
 * \param [in] flush_entry_info_p  - The parameters of the entry to be added.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_flush_machine_entry_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    dnx_field_flush_entry_t * flush_entry_info_p);

/**
 * \brief
 *  Get an entry to the TCAM of the flush machine for EXEM learn.
 * \param [in] unit                - Device ID.
 * \param [in] flags               - The flags for the entry add.
 * \param [in] fg_id               - The EXEM field group upon which we qualify.
 * \param [in] entry_id            - The number of the entry in the TCAM.
 * \param [out] flush_entry_info_p  - The parameters of the entry.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_flush_machine_entry_get(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    dnx_field_flush_entry_t * flush_entry_info_p);

/**
 * \brief
 *  Delete an entry from the TCAM of the flush machine for EXEM learn.
 * \param [in] unit                - Device ID.
 * \param [in] flags               - The flags for the entry delete.
 * \param [in] fg_id               - The EXEM field group upon which the entry qualifies.
 * \param [in] entry_id            - The number of the entry in the TCAM.
 * \return
 *   shr_error_e - Error Type
 * \remark
  *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_flush_machine_entry_delete(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id);

/* } */

#endif
