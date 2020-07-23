/**
 * \file bcm_int/dnx/field/field_group.h
 *
 *
 * Field Processor definitions for database for DNX
 *
 * Purpose:
 *     'Field Processor' (FP) definitions for Field Group functions for other field modules for Packet Processor.
 *     Devices starting at DNX
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_GROUP_H_INCLUDED
/* { */
#define FIELD_FIELD_GROUP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Include files
 * {
 */
#include <include/bcm_int/dnx/field/tcam/tcam_handler.h>
#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_key.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
/*
 * field_actions.h is required for 'dnx_field_actions_fes_get_info_t' (definitions
 * related to 'get' procedures).
 */
#include <include/bcm_int/dnx/field/field_actions.h>
/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * }
 */
/*
 * Typedefs
 * {
 */

/**
 * TCAM information for pre-allocating TCAM banks.
 * (Used only for TCAM FGs).
 */
typedef struct
{
    /**
     * The list of TCAM bank IDs to reserve for the field group to be created.
     * This list is only valid for TCAM field groups.
     */
    int tcam_bank_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    /**
     * Number of TCAM banks to reserve for the given field group.
     */
    int nof_tcam_banks;
    /**
     * Specifies TCAM bank allocation mode, see Enum definition for more info.
     */
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;
} dnx_field_group_tcam_info_t;

/**
 * KBP information for adding properties to KBP.
 */
typedef struct
{
    /** True when min_capacity has a meaningful info, otherwise default values are set. */
    uint8 min_capacity_valid;
    /*
     * Set the minimum capacity reserved in EXTERNAL TCAM. Used if capacity_valid is true.
     */
    uint32 min_capacity;
    /*
     * Whether to set the property ALGORITHMIC for algorithmic mode
     */
    uint8 algorithmic_mode_valid;
    /*
     * Set the algorithmic mode (algorithmic property) to this value. Used if algorithmic_mode_valid is true.
     */
    uint32 algorithmic_mode;
} dnx_field_group_add_kbp_info_t;

/**
 * Struct contains the information about the Field group that would appear in SW state.
 * This structure exist here to allow functions to be used jointly without worrying about different sizes of arrays.
 * See dnx_field_group_fg_sw_info_t.
 */
typedef struct
{
    /**
     * The Field stage in which the field group exists.
     */
    dnx_field_stage_e field_stage;
    /**
     * The Field group type (TCAM, Direct Extraction, EXEM...)
     */
    dnx_field_group_type_e fg_type;
    /**
     * Field group name.
     */
    field_group_name_t group_name;
    /**
     * The list of contexts attached to the field group.
     */
    dnx_field_attach_context_info_t context_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    /**
     * Mapping of the qualifier inside the key
     */
    dnx_field_key_template_t key_template;
    /**
     * This enum indicate the size of the Key, some Field Groups use only part of the key
     */
    dnx_field_key_length_type_e key_length_type;
    /**
     * This enum indicate the size of the payload block used.
     * For example a TCAM field group can use 32, 64 or 128 bit payloads.
     * Not used by HASH or direct extraction field groups.
     */
    dnx_field_action_length_type_e action_length_type;
    /**
     * Hold the information regarding the configured actions for Field Group.
     * This field is different from SW state, with different array sizes.
     */
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    /**
     * Holds the ID of DBAL table that was created for the field group.
     */
    dbal_tables_e dbal_table_id;
    /**
     * Flags with which Field Group was created, needed for Get functionality (dnx_field_group_get).
     */
    dnx_field_group_add_flags_e flags;
    /**
     * TCAM info for pre-allocating banks.
     */
    dnx_field_group_tcam_info_t tcam_info;
} dnx_field_group_fg_info_for_sw_t;

/**
 * Struct contains the information about the ACE format that would appear in SW state.
 * This structure exist here to allow functions to be used jointly without worrying about different sizes of arrays.
 * See dnx_field_ace_format_sw_info_t.
 */
typedef struct
{
    /**
     * Hold the information regarding the configured actions for ACE Format.
     * This field is different from SW state, with different array sizes.
     */
    dnx_field_actions_ace_payload_info_t actions_payload_info;
    /**
     * Flags with which ACE format was created, needed for Get functionality (dnx_field_ace_format_get).
     */
    dnx_field_ace_format_add_flags_e flags;
    /**
     * Name of the ACE Format.
     */
    char name[DBAL_MAX_STRING_LENGTH];
} dnx_field_ace_format_info_for_sw_t;

/**
 * This structure holds the basic parameters for dnx_field_group_add() function
 * This is not part of field SW state
 * See dnx_field_group_get().
 */
typedef struct
{
    /** Field stage, for which stage the Database was created*/
    dnx_field_stage_e field_stage;
    /**
     *  Database type: The database may be:TCAM, Direct Table,
     *  or Direct Extraction.
     */
    dnx_field_group_type_e fg_type;

    /**
     * DNX level qualifier look at dnx_field_qual_t for more info
     * Number of meaningful elements on this array: All elements until
     * DNX_FIELD_QUAL_TYPE_INVALID is encountered.
     */
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    /**
     * For each qualifier in dnx_quals, we have an indication whether or not the qualifier expects to receive ranges.
     * Only external (KBP) field groups support ranged qualifiers.
     */
    uint8 qual_is_ranged[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    /**
     * DNX Actions applied by the database.
     * Can hold any of the dnx action depending on class and stage:
     * See List of available ones in field_map_data.c
     * Number of meaningful elements on this array: All elements until
     * DNX_FIELD_ACTION_INVALID is encountered.
     */
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    /**
     * For each action in dnx_actions, we have an indication whether or not the action uses a valid bit or not.
     */
    uint8 use_valid_bit[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

     /**
      * This struct holds all advanced parameters for TCAM field group.
      * For further information see struct definition.
      */
    dnx_field_group_tcam_info_t tcam_info;
    /**
      * This struct holds all advanced parameters for KBP (external TCAM) field group.
      * Specifically, it indicated properties to be set and minimum capacity.
      */
    dnx_field_group_add_kbp_info_t kbp_info;
    /**
     * Name of the field group.
     */
    char name[DBAL_MAX_STRING_LENGTH];
} dnx_field_group_info_t;

/**
 * This structure holds the basic parameters for dnx_field_ace_format_add() function
 * This is not part of field SW state
 * See dnx_field_ace_format_get().
 */
typedef struct
{
    /**
     * DNX Actions applied by the ACE format.
     * Can hold any of the dnx action depending on class and stage:
     * See List of available ones in field_map_data.c
     * Number of meaningful elements on this array: All elements until
     * DNX_FIELD_ACTION_INVALID is encountered.
     */
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    /**
     * For each action in dnx_actions, we have an indication whether or not the action uses a valid bit or not.
     */
    uint8 use_valid_bit[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    /**
     * Name of the ACE Format.
     */
    char name[DBAL_MAX_STRING_LENGTH];
} dnx_field_ace_format_info_t;

/**
 * This structure holds the full detailed parameters for dnx_field_group_add function
 * This is not part of field SW state
 * Note that there is some redundancy since 'dnx_field_group_basic_info_t' contains info
 * which may be extracted from 'dnx_field_group_fg_info_for_sw_t'
 * See dnx_field_group_get().
 * See dnx_field_group_add().
 */
typedef struct
{
    /**
     * Basic info containing information related to dnx_field_group_add()
     */
    dnx_field_group_info_t group_basic_info;
    /**
     * Detailed info containing information related to dnx_field_group_add().
     * This is the sw-state database of field group.
     */
    dnx_field_group_fg_info_for_sw_t group_full_info;
    /**
     * Identifier of the field group on TCAM driver.
     * Get 'tcam_handler_id' using dbal_tables_tcam_handler_id_get() with
     * 'dbal_table_id' as input. 'dbal_table_id' is an element on the sw state
     * in 'dnx_field_group_fg_info_for_sw_t'.
     */
    uint32 tcam_handler_id;
    /**
     * The HW identifier of the logical database.
     * In tcam, it is the access_profile_id:
     *    The index, in the access profile memory, corresponding to 'tcam_handler_id'
     *    Get access_profile_id using dnx_field_tcam_handler_access_profile_id_get()
     *    with 'tcam_handler_id' as input.
     * In MDB databases (such as EXEM), it is the APP_DB_ID of the database in MDB.
     */
    dnx_field_app_db_id_t app_db_id;
} dnx_field_group_full_info_t;

/**
 * This structure holds the full detailed parameters for dnx_field_ace_format_add function
 * This is not part of field SW state
 * Note that there is some redundancy since 'ace_format_basic_info_t' contains info
 * which may be extracted from 'dnx_field_ace_format_info_for_sw_t'.
 * See dnx_field_ace_format_get().
 * See dnx_field_ace_format_add().
 * See dnx_field_group_full_info_t.
 * See dnx_field_group_context_full_info_t.
 */
typedef struct
{
    /**
     * Basic info containing information related to dnx_field_ace_format_add()
     */
    dnx_field_ace_format_info_t format_basic_info;
    /**
     * Detailed info containing information related to dnx_field_ace_format_add().
     * This is the sw-state database of ACE format.
     */
    dnx_field_ace_format_info_for_sw_t format_full_info;
    /**
     * Information about the FES instructions used by the ACE ID.
     */
    dnx_field_actions_fes_ace_get_info_t actions_fes_info;
} dnx_field_ace_format_full_info_t;

/**
 * This structure holds the limited detailed parameters for  for API's:
 * See dnx_field_group_context_attach().
 * See dnx_field_group_context_get().
 */
typedef struct
{
    /**
     * The qualifier order, type and number must be the same as was passed to dnx_field_group_add API
     * Number of meaningful elements on this array: All elements until
     * DNX_FIELD_QUAL_TYPE_INVALID is encountered.
     */
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    /**
     * Indicate how to read the relevant qualifier from PBUS
     * The information is aligned to the qualifier types array above
     * Number of elements set by qual_types array
     */
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    /**
     * DNX Actions applied by this context.
     * Can hold any of the dnx action depending on class and stage:
     * See List of available ones in field_map_data.c
     * Number of meaningful elements on this array: All elements until
     * DNX_FIELD_ACTION_INVALID is encountered.
     * The action order, type and number MUST be the same as was passed to dnx_field_group_add API
     */
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    /**
     * Include information on each action that was configured in attach API, such as action priority.
     * Number of valid elements is set as per 'dnx_actions' array
     */
    dnx_field_action_attach_info_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    /**
     * An array of FEMs that are added to the field group, but are not to be used by the context.
     */
    dnx_field_fem_id_t fems_invalidate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID];
    /**
     * Compare attributes - in case the context has compare mode - which key/tcam result to compare
     * CMP1 : this FG will be compared against CMP1, internally SDK will try to allocate key B
     * CMP2 : this FG will be compared against CMP2, internally SDK will try to allocate key D*
     */
    dnx_field_group_compare_id_e compare_id;
    /**
     * Payload Offset - Relevant ONLY for the KBP FG
     * Payload Offset is the offset of the result of current FG inside the result buffer received from the KBP
     * The KBP result buffer is a concatenation of the result buffers of it's field groups and it's size is 240b.
     * This offset may vary for the same FG when it is attached to different contexts.
     */
    uint32 payload_offset;
    /**
     * Payload id - Relevant ONLY for the KBP FG
     * Payload id is the [0-7] id of the result of current FG inside the result buffer received from the KBP
     * The KBP result buffer is a concatenation of the result buffers of it's field groups and it's size is 240b.
     * The id represents the relevant hit-bit that indicates if the payload is valid.
     * This id may vary for the same FG when it is attached to different contexts.
     */
    uint32 payload_id;
} dnx_field_group_attach_info_t;

/**
 * Struct contains the origin in parameter for the dnx_field_group_context_attach() and addtional diteailed info
 * See dnx_field_group_context_get().
 * See dnx_field_group_context_attach().
 */
typedef struct
{
    /**
     * Basic info containing information related to dnx_field_context_attach()
     */
    dnx_field_group_attach_info_t attach_basic_info;
    /**
     * FFC detailed info that was supplied by key module
     * Holds FFC id allocated per qualifier and Key id allocated for the context
 */
    dnx_field_key_group_ffc_info_t group_ffc_info;
    /**
     * Field IO, indicator of where the output is written to.
     */
    dbal_enum_value_field_field_io_e field_io;
    /**
     * Attached key info
     */
    /**
     * Keys allocated for this context
     */
    dnx_field_key_id_t key_id;
    /**
     * Lenght-type of key which has been allocated by key module
     */
    dnx_field_key_length_type_e key_length_type;
    /**
     * Access parameter for KBR (See, for example, DBAL table KLEAP_IPMF1_KBR_INFO (IPPC_PMF_KBR_PASS_1/))
     * Used from TCAM/MDB/EXEM - To know how to perform HW lookup within the databases
     */
    int app_db_id;
    /**
     * Information about the FES quartets
     */
    dnx_field_actions_fes_context_group_get_info_t actions_fes_info;
} dnx_field_group_context_full_info_t;

/**
 * Struct contains the parameters of the EXEM PDB in use (LEXEM or SEXEM)
 * See dnx_field_group_exem_info().
 */
typedef struct
{
    /**
     * The maxcimum size of the key.
     */
    unsigned int max_key_size;
    /**
     * The maxcimum size of the payload.
     */
    unsigned int max_payload_size;
    /**
     * The number of bits in the APP_DB_ID.
     */
    unsigned int app_db_id_size;
    /**
     * The key length type that refers to the PDB.
     */
    dnx_field_key_length_type_e key_length_type;
    /**
     * The action length type that refers to the PDB.
     */
    dnx_field_action_length_type_e action_length_type;
    /**
     * The DB ID of the PDB.
     */
    dbal_physical_tables_e dbal_physical_table_id;

} dnx_field_group_exem_info_t;

/*
 * } Types
 */

/**
 * \brief
 *   Convert IO enum values to text (for display purposes only)
 *   See 'dbal_enum_value_field_field_io_e'.
 * \param [in] field_io -
 *   IO identifier (from enum) to trabslate to text.
 * \return
 *   Null-terminated string describing input IO value.
 * \remark
 *   None
 * \see
 *   * dbal_enum_value_field_field_io_e
 */
char *dnx_field_io_text(
    dbal_enum_value_field_field_io_e field_io);
/**
* \brief
*  Clear the dnx_field_group_info_t, set it to preferred init values
* \param [in] unit          - Device ID
* \param [in] fg_info_in_p  - Pointer to input structure of dnx_field_group_info_t that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_info_t_init(
    int unit,
    dnx_field_group_info_t * fg_info_in_p);

/**
* \brief
*  Clear the dnx_field_ace_format_info_t, set it to preferred init values
* \param [in] unit                 - Device ID
* \param [in] ace_format_info_in_p  - Pointer to input structure of dnx_field_ace_format_info_t that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_ace_format_info_t_init(
    int unit,
    dnx_field_ace_format_info_t * ace_format_info_in_p);

/**
* \brief
*  Init Structure dnx_field_group_attach_info_t to invalid params
* \param [in] unit           - Device Id
* \param [in] attach_info_p  - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_attach_info_t_init(
    int unit,
    dnx_field_group_attach_info_t * attach_info_p);

/**
 * \brief
 *  This function creates a field group, i.e. per set (qualifier,action).
 * \param [in] unit     - Device ID
 * \param [in] flags    - Flags of field group add API, look at dnx_field_group_add_flags_e
 * \param [in] fg_info_in_p  -pointer to database info that will be created
                                     look at dnx_field_group_info_t
 * \param [in,out] fg_id_p    - Field group ID,
            as in - in case flag WITH_ID is set, will hold the field group ID
            as out - in case flag WITH_ID is not set, will return the created field group ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * If this procedure fails to allocate because of resource issues, an
 *     error, _SHR_E_RESOURCE, will be returned.
 * \see
 *   * None
 */
shr_error_e dnx_field_group_add(
    int unit,
    dnx_field_group_add_flags_e flags,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_t * fg_id_p);

/**
* \brief
*  Init the Sw state of Field Group Database module
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_sw_state_init(
    int unit);

/**
* \brief
*  De-Init the Sw state of Field Group Database module
* Destroy all memory
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_sw_state_deinit(
    int unit);

/**
 *  \brief
 *    Given HW identifier of data base, get an array of all context IDs which
 *    are on it.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_id -
 *    Field group id. Indicate field gourp id in DNX lavel that being configured.
 * \param [out] context_id -
 *     array of uint32 size of DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS
 *    This procedure loads pointed memory by all context IDs
 *    on specified data base ('db_id_ndx'). Number of elements on this array is
 *    is defined by last parameter DNX_FIELD_CONTEXT_ID_INVALID
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 */
shr_error_e dnx_field_group_context_id_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS]);
/**
 * \brief
 *   Given 'field group' and 'context id', get an indication on whether 'context id'
 *   is attached to that 'field group'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fg_id -
 *   Field Group id to search list of contexts of
 * \param [in] context_id -
 *   Identifier of context to search for within the list of contexts attached to
 *   this field group.
 * \param [out] found_p -
 *   Pointer to unsigned int. This procedure loads pointed memory by a non-zero value
 *   if context is attached to this field group (fg_id). Otherwise, zero is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * dnx_field_group_context_id_arr_get()
 */
shr_error_e dnx_field_group_is_context_id_on_arr(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    unsigned int *found_p);
/**
 *  \brief
 *    Given qualifier ID, get info about it within specified data base:
 *    Bit location, within 'key template', where it resides, width (bits).
 *    Note that this is not the offset on the actual key since allocation
 *    on key depends on algorithm. For example, if allocation is in 80 bits
 *    resolution then offset '0' on 'key template' may be offset '80' on
 *    key.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_id -
 *    Field group id. Indicate field gourp id in DNX level that being configured.
 * \param [in] qualifier -
 *    Variable of type dnx_field_qual_t. Identifier of the qualifier to search
 *    for within specified data base.
 * \param [out] qualifier_bit_lsb_location_p -
 *    Pointer to uint. This procedure loads pointed memory by the bit location (starting
 *    from the LSB, which is marked as 0) within the carrying 'key template'.
 *    Expected range is 0 - 159
 * \param [out] qualifier_bit_width_p -
 *    Pointer to uint. This procedure loads pointed memory by the number of bits which this
 *    qualifier occupies on specified key template.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_qualifier_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t qualifier,
    unsigned int *qualifier_bit_lsb_location_p,
    unsigned int *qualifier_bit_width_p);

/**
* \brief
*  Get context ID info from SW state of Field group
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id that context ID being attached to
* \param [in] context_id      - Context ID
* \param [out] context_info_p - Pointer type of dnx_field_attach_context_info_t that hold the context ID info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_context_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_attach_context_info_t * context_info_p);

/**
 * \brief
 *  Returns the configured key per Field database
 * \param [in] unit       - Device Id
 * \param [in] fg_id      - Database ID
 * \param [in] context_id - Each context ID has its own Key id that allocated for Field Group Database
 * \param [in] key_id_p   - Structure that holds an array of key IDs.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_context_key_id_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Retruns the position of a direct extraction field group attached to a context.
 * \param [in] unit        - Device Id
 * \param [in] fg_id       - Database ID
 * \param [in] context_id  - Each context ID has its own Key id that allocated for Field Group Database
 * \param [in] group_lsb_p - The LSB of the group within the key that it is in.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_context_de_position_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int *group_lsb_p);

/**
 * \brief
 *  Returns the key attach info for a field group
 * \param [in] unit       - Device Id
 * \param [in] fg_id      - Database ID
 * \param [in] context_id - Each context ID has different attach info for the same field group
 * \param [out] qual_info - Array of size DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG.
 *                          Filled with the attach info for each qualifier.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_context_qualifier_attach_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
* \brief
* Return the DBAL table that was created for Field Group
* DBAL Table is not created for FG type of Direct Extract
* \param [in] unit             - Device ID
* \param [in] fg_id            - Field Group ID
* \param [out] dbal_table_id_p  - DBAL table id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_dbal_table_id_get(
    int unit,
    dnx_field_group_t fg_id,
    dbal_tables_e * dbal_table_id_p);

/**
* \brief
* Return result_type_index, the DBAL ID that was created for ACE format.
* \param [in] unit                  - Device ID
* \param [in] ace_id                - ACE ID
* \param [out] result_type_index_p  - The DBAL ID of the ACE format.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_dbal_table_id_get
*/
shr_error_e dnx_field_ace_format_result_type_index_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    int *result_type_index_p);

/**
* \brief
* Return The ACE_ID associated with result_type_index, the DBAL ID that was created for ACE format.
* \param [in] unit                  - Device ID.
* \param [in] result_type_index     - The DBAL ID of the ACE format.
* \param [out] ace_id_p             - ACE ID.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_ace_format_result_type_index_get
*/
shr_error_e dnx_field_ace_format_result_type_index_to_ace_id(
    int unit,
    int result_type_index,
    dnx_field_ace_id_t * ace_id_p);

/**
* \brief
*  Get Field Stage from certain Field group
* \param [in] unit           - Device ID
* \param [in] fg_id          - Field Group ID
* \param [in] field_stage_p  - Field Group stage (look dnx_field_stage_e)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_field_stage_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_stage_e * field_stage_p);

/**
* \brief
*  Get field group type of a group
* \param [in] unit       - Device ID
* \param [in] fg_id      - Field Group ID
* \param [out] fg_type_p  - Field Group Type
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e * fg_type_p);

/**
* \brief
*  Get field group type of a group
* \param [in] unit                - Device ID
* \param [in] fg_id               - Field Group ID
* \param [out] key_length_type_p  - Enum indicating the size of the lookup key.
*                                   Not necessariliy all of it is used, for example TCAM can have a key size of
*                                   half key (80 bits), but only use 5 bits (and they would be aligned to the LSB).
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_key_length_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_key_length_type_e * key_length_type_p);

/**
* \brief
*  Get field group type of a group
* \param [in] unit                   - Device ID
* \param [in] fg_id                  - Field Group ID
* \param [out] action_length_type_p  - Enum indicating the size of the lookup key.
*                                      Not necessariliy all of it is used, for example TCAM can have a result size of
*                                      half key (32 bits), but only use 5 bits (and they would be aligned to the MSB).
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_action_length_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_length_type_e * action_length_type_p);

/**
* \brief
*  Get action template
* \param [in] unit                    - Device ID
* \param [in] fg_id                   - Field Group
* \param [out] actions_payload_info_p  - Action tempalte as was created in field group add
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_action_template_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_actions_fg_payload_info_t * actions_payload_info_p);

/**
* \brief
*  Get action template
* \param [in] unit           - Device ID.
* \param [in] fg_id          - Field Group.
* \param [out] nof_actions_p - Number of actions in the field group's template.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_action_nof_get(
    int unit,
    dnx_field_group_t fg_id,
    unsigned int *nof_actions_p);

/**
* \brief
*  This function provides the VMV size of a specific physical DB.
* \param [in] unit                 - Device ID
* \param [in] dbal_physical_table  - The EXEM physical table
* \param [out] vmv_size_p          - The size of the VMV
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_physical_db_vmv_size_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint8 *vmv_size_p);

/**
* \brief
*  Get the starting position of the payload buffer on the field group in the HW, relative to the action buffer.
* \param [in] unit                    - Device ID
* \param [in] fg_id                   - Field Group
* \param [out] buffer_shift_p         - Action tempalte as was created in field group add, adjusted by
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_action_template_get
*/
shr_error_e dnx_field_group_action_buffer_shift_get(
    int unit,
    dnx_field_group_t fg_id,
    int *buffer_shift_p);

/**
* \brief
*  Get action template
* \param [in] unit                     - Device ID
* \param [in] ace_id                   - ACE ID
* \param [out] actions_payload_info_p  - Action template as was created in ACE format add
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_ace_format_action_template_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_ace_payload_info_t * actions_payload_info_p);

/**
* \brief
*  Get the minimum and maximum bits of the payload template.
* \param [in] unit                     - Device ID
* \param [in] fg_id                    - Field group ID
* \param [out] min_offset_p            - The lsb of the first action. -1 if no action with positive size found.
* \param [out] max_offset_p            - The msb of the last action. -1 if no action with positive size found.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_ace_format_payload_template_min_max_get
*/
shr_error_e dnx_field_group_payload_template_min_max_get(
    int unit,
    dnx_field_group_t fg_id,
    int *min_offset_p,
    int *max_offset_p);

/**
* \brief
*  Get the amount of bits used by the actions on the payload, including spaces between actions.
* \param [in] unit                     - Device ID
* \param [in] ace_id                   - ACE ID
* \param [out] min_offset_p            - The lsb of the first action. -1 if no action with positive size found.
* \param [out] max_offset_p            - The lsb of the first action. -1 if no action with positive size found.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_payload_template_min_max_get
*/
shr_error_e dnx_field_ace_format_payload_template_min_max_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    int *min_offset_p,
    int *max_offset_p);

/**
* \brief
*  Get the total sum of bits on all actions on the paylaod.
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group ID
* \param [out] payload_size_p - The total size of all actions on payload.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_actions_spread_on_payload_get
*/
shr_error_e dnx_field_group_actions_payload_size_get(
    int unit,
    dnx_field_group_t fg_id,
    unsigned int *payload_size_p);

/**
* \brief
*
* \param [in] unit            -
* \param [in] fg_id           -
* \param [out] key_template_p  -
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_key_template_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_key_template_t * key_template_p);

/**
* \brief
*  Get the TCAM handler ID for specific Field Group
* \param [in] unit               - Device ID
* \param [in] fg_id              - Field Group
* \param [out] tcam_handler_id_p  - TCAM handler ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_tcam_handler_id_get(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *tcam_handler_id_p);

/**
* \brief
*  Get the payload field_io of a field group for a specific context.
* \param [in] unit               - Device ID
* \param [in] fg_id              - Field group ID.
* \param [in] context_id         - The context the field group is attached to.
* \param [out] field_io_p        - To be loaded with the payload field_io.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_field_io_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_io_e * field_io_p);

/**
* \brief
*  Get the list of FEMs that are invalidated for a specific field group in s specific context.
* \param [in] unit               - Device ID
* \param [in] fg_id              - Field group ID.
* \param [in] context_id         - The context the field group is attached to.
* \param [out] fems_invalidate   - To be loaded with the invalidated FEMs
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_fem_invalidated_fems(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fems_invalidate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID]);

/**
* \brief
*  Goes over all FGs in the system, searching for the one and only that meets the following
*  conditions we are looking for:
*   1) Is attached on the given context ID
*   2) Has the exact same FG type
*   3) The 'first' key ID it resides on (in the given context) is equal to the 'first' key ID given
*  In case no such FG exists, this function throws an error.
*
* \param [in] unit       - Device ID
* \param [in] context_id - Context ID the FG ID we're looking for is attached on
* \param [in] fg_type    - FG Type that we're looking for
* \param [in] fg_key     - The key ID that the FG should reside on (in the given context)
* \param [out] fg_id_p   - The FG ID that meets the above conditions, in case it exists
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_convert_context_key_to_fg_id(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t fg_key,
    dnx_field_group_t * fg_id_p);

/**
* \brief
*  Attach Field Group to context IDs
* \param [in] unit         - Device id
* \param [in] flags        - context attach flags (see dnx_field_group_context_attach_flags_e)
* \param [in] fg_id        - Field Group id to attach programs to
* \param [in] context_id        - PMF PGM id to attach to FG
* \param [in] group_attach_info_p  -Pointer to attach info look at (dnx_field_group_attach_info_t)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_context_attach(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * group_attach_info_p);
/*
 * \brief
 *   Check whether specified Field Group resides on some FEMs and, in that case,
 *   detach specified contexts from those FEMs by updating contexts table.
 *   This procedure is for internal usa only. Do not use on operational code.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fg_id -
 *   Identifier of a field group to search for on the list of owners of all
 *   'fem_id's.
 * \param [in] context_id -
 *   Context ID to be detached from specified 'fg_id' and which needs to be updated
 *   in HW.
 *   FEMs)
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * This should actually be a 'static' proceduer and is made global
 *      only for the sake of negative tests.
 * \see
 *    * dnx_field_group_fems_context_attach
 *    * dnx_field_fem_is_fg_owner
 *    * dnx_field_fem_is_fg_secondary
 *    * dnx_field_group_fes_unconfig()
 */
shr_error_e dnx_field_group_fems_context_detach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id);

/**
 * \brief
 *  Reset/deallocate all resources that were allocated for the context ID at input to the FG at input
 * \param [in] unit        - Device Id
 * \param [in] fg_id       - Field Group to detach Program From
 * \param [in] context_id  - Context ID to detach
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_context_detach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id);

/**
 * \brief
 *  Get the action array out of payload info that was saved in Field group Sw state
 * \param [in] unit                  - Device ID
 * \param [in] fg_id     - Field group id to get action types
 * \param [out] dnx_actions  -
 *   This is a pointer to an array.
 *   Maximal number of actions accepted here: DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP.
 *   The list of actions ends when an action of value 'DNX_FIELD_ACTION_INVALID' is detected.
 * \param [out] nof_actions_p -Loaded with the number of valid elements in each the array dnx_actions.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_dnx_action_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *nof_actions_p);

/**
 * \brief
 *  Get the action array out of payload info that was saved in ACE format Sw state
 * \param [in] unit                  - Device ID
 * \param [in] ace_id     - ACE id to get action types
 * \param [out] dnx_actions  -
 *   Action arrays to contain all actions.
 *   Maximal number of actions accepted here: DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP.
 * \param[out] nof_actions_p - The number of valid elements in the array dnx_actions.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_ace_format_dnx_action_arr_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *nof_actions_p);

/**
* \brief
*  Returns the offset on the payload buffer of the given dnx_action on the given fg_id.
*  If the action is not present in the FG, an error is returned.
*
* \param [in] unit       - Device ID
* \param [in] fg_id      - FG ID that contains the action
* \param [in] dnx_action - The action to return its offset on the payload buffer
* \param [out] offset_p    - Offset of the action in the payload buffer of the given FG
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_action_offset_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_t dnx_action,
    int *offset_p);

/**
* \brief
*  Returns the indices on the master key of the segments uses by each qualifier of a field group.
*
* \param [in] unit                - Device ID
* \param [in] fg_id               - field group
* \param [in] opcode_id           - Opcode
* \param [out] seg_index_on_master_by_qual - For each qualifier, the index of the corresponding segment on the
*                                            opcode's master key.
* \param [out] qual_index_on_segment    - For each qualifier, it's index within the segment.
*                                            Zero for non packed quals, as when ther are
*                                            multiple quals per segment they are packed.
* \param [out] qual_offset_on_segment   - For each qualifier, it's offset from the lsb of the segment in bits.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_kbp_segment_indices(
    int unit,
    dnx_field_group_t fg_id,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
* \brief
*  Returns the number of segmentd used by FWD for a opcode.
*
* \param [in] unit                - Device ID
* \param [in] opcode_id           - Opcode
* \param [out] nof_fwd_segments_p - Number of FWD segments
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_kbp_opcode_nof_fwd_segments(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 *nof_fwd_segments_p);

/**
* \brief
*  Get the qualifier array out of key template  that was saved in Field group Sw state
* \param [in] unit                  - Device ID
* \param [in] fg_id     - Field group id to get qualifier types
* \param [out] dnx_quals  - Array of DNX qualifiers
* \param[out] nof_quals_p - Number of valid elements in the array dnx_quals.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_dnx_qual_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    unsigned int *nof_quals_p);

/**
* \brief
* Compare a qualifiers array with the qualifiers of a field group, verifying that they are both the same.
* Returns _SHR_E_PARAM if the arrays are not identical.
* \param [in] unit                    - Device ID
* \param [in] fg_id                   - The ID of the field group to whose qualifiers we compare.
* \param [in] dnx_quals               - The qualifier array to be compared.
*                                       The number of qualifiers in the array is determined by the first element
*                                       that equals DNX_FIELD_ACTION_INVALID. If no such element is found, the
*                                       number of elements is regarded as DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_qual_arr_compare_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
* \brief
* Compare a qualifiers array with the qualifiers of a field group, verifying that all the qualifiers present in the field group,
* and there are no duplicates on dnx_quals.
* Returns _SHR_E_PARAM if not.
* \param [in] unit                    - Device ID
* \param [in] fg_id                   - The ID of the field group to whose qualifiers we compare.
* \param [in] dnx_quals               - The qualifier array to be compared.
*                                       The number of qualifiers in the array is determined by the first element
*                                       that equals DNX_FIELD_ACTION_INVALID. If no such element is found, the
*                                       number of elements is regarded as DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_qual_arr_subset_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
* \brief
* Compare a actions array with the actions of a field group, verifying that all the actions present in the field group,
* and there are no duplicates on dnx_actions.
* Returns _SHR_E_PARAM if not.
* \param [in] unit                    - Device ID
* \param [in] fg_id                   - The ID of the field group to whose actions we compare.
* \param [in] dnx_actions             - The action array to be compared.
*                                       The number of actions in the array is determined by the first element
*                                       that equals DNX_FIELD_ACTION_INVALID. If no such element is found, the
*                                       number of elements is regarded as DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_action_arr_subset_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG]);

/**
* \brief
*  Get Context information that was configured for specific Field Group context attach.
*  Can also optionally return EFES information.
* \param [in] unit           - Device ID
* \param [in] fg_id          -  Field Group ID
* \param [in] context_id     - Context ID that's conntected to Field group, to gets its info
* \param [out] attach_basic_info_p  - Field group context attach information
* \param [out] fes_group_info_p     - EFES information. If NULL, won't be filled.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_context_get
*/
shr_error_e dnx_field_group_context_basic_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_basic_info_p,
    dnx_field_actions_fes_context_group_get_info_t * fes_group_info_p);

/**
* \brief
*  Get Context atach information for specific Field Group
* \param [in] unit           - Device ID
* \param [in] fg_id          -  Field Group ID
* \param [in] context_id     - Context ID that's conntected to Field group, to gets its info
* \param [out] attach_full_info_p  - pointer to dnx_field_group_context_full_info_t (look on struct description)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_context_basic_info_get
*/
shr_error_e dnx_field_group_context_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_context_full_info_t * attach_full_info_p);

/**
* \brief
*  Delete Field Group, deallocate all relevant resources
* \param [in] unit   - Device Id
* \param [in] fg_id  - Field Group Id to delete
* \return
*   shr_error_e - Error Type
* \remark
*   * Note that this operation is rejected if 'fg_id' is marked
*     as 'secondary field group' on any FEM which has been added
*     to any field group. In that case, this FEM needs, first, to
*     be removed (dnx_field_fem_action_remove()).
* \see
*   * None
*/
shr_error_e dnx_field_group_delete(
    int unit,
    dnx_field_group_t fg_id);

/**
* \brief
*  Delete ACE format, deallocate all relevant resources
* \param [in] unit   - Device Id
* \param [in] ace_id - ACE format Id to delete
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_ace_format_delete(
    int unit,
    dnx_field_ace_id_t ace_id);

/**
* \brief
*  Updates FESes for newly created cascading contexts when the FESes are shared between two stages.
*  Meaning, when we create a context in iPMF2, we need to update the iPMF1 FESes of the context we cascades from.
* \param [in] unit                - Device ID
* \param [in] stage               - Field Stage to create context for
* \param [in] context_id          - The created context ID.
* \param [in] cascaded_from       - The context we cascade from.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_context_create()
*   * dnx_field_group_fes_config()
 */
shr_error_e dnx_field_group_context_create_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from);

/**
* \brief
*  Delete FESes for destroyed cascading contexts when the FESes are shared between two stages.
*  Meaning, when we destroy a context in iPMF2, we need to delete the iPMF1 FESes of the context we cascades from.
* \param [in] unit                - Device ID
* \param [in] stage               - Field Stage to create context for
* \param [in] context_id          - The created context ID.
* \param [in] cascaded_from       - The iPMF1 context context_id cascade from.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_context_destroy()
*   * dnx_field_group_fes_unconfig()
*   * dnx_field_group_context_destroy_fem_update()
*/
shr_error_e dnx_field_group_context_destroy_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from);

/**
* \brief
*  Updates FEMs for newly created cascading contexts when the FEMs are shared between two stages.
*  Meaning, when we create a context in iPMF2, we need to update the iPMF1 FEMs of the context we cascades from.
* \param [in] unit                - Device ID
* \param [in] stage               - Field Stage to create context for
* \param [in] context_id          - The created context ID.
* \param [in] cascaded_from       - The context we cascade from.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_context_create()
*   * dnx_field_group_fems_context_attach()
*   * dnx_field_group_context_create_fes_update()
*/
shr_error_e dnx_field_group_context_create_fem_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from);

/**
* \brief
*  Delete FEM references for destroyed cascading contexts when the FEMs are shared between two stages.
*  Meaning, when we destroy a context in iPMF2, we need to delete the iPMF1 FEM references of the context we
*  cascades from.
* \param [in] unit                - Device ID
* \param [in] stage               - Field Stage to create context for
* \param [in] context_id          - The created context ID.
* \param [in] cascaded_from       - The iPMF1 context context_id cascade from.
* \return
*   shr_error_e - Error Type
* \remark
*   * We do not check that the context is attached to the field group.
*   * Unlike dnx_field_group_context_destroy_fes_update(), we delete all references to FEMs from the context.
* \see
*   * dnx_field_context_destroy()
*   * dnx_field_group_fems_context_detach()
*   * dnx_field_group_context_destroy_fes_update();
*/
shr_error_e dnx_field_group_context_destroy_fem_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from);

/**
 * \brief
 *  Get the 'flags' that were configured for field group in dnx_field_group_add()
 * \param [in] unit       - Device ID
 * \param [in] fg_id      - Field group ID
 * \param [in] context_id - Contexts field group is attached to.
 * \param [out] flags_p   - Flags that were set for the atach (of type dnx_field_group_context_attach_flags_e)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_context_attach_flags_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_context_attach_flags_e * flags_p);

/**
 * \brief
 *  Get the 'flags' that were configured for field group in dnx_field_group_add()
 * \param [in] unit     - Device ID
 * \param [in] fg_id    - Field group ID
 * \param [out] flags_p  - Flags that were set for the Field Group (of type dnx_field_group_add_flags_e)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_flags_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_add_flags_e * flags_p);

/**
 * \brief
 *  Get the 'flags' that were configured for ACE format in dnx_field_ace_format_add()
 * \param [in] unit     - Device ID
 * \param [in] ace_id    - ACE ID
 * \param [out] flags_p  - Flags that were set for the ACE format (of type dnx_field_ace_format_add_flags_e)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_ace_format_flags_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_add_flags_e * flags_p);

/**
* \brief
*  Obtains if an exem field group is LEXEM.
* \param [in] unit                    - Device ID
* \param [in] fg_id                   - Field Group
* \param [out] is_lexem_p             - If true the field group is LEXEM, if false SEXEM.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_exem_is_lexem(
    int unit,
    dnx_field_group_t fg_id,
    int *is_lexem_p);

/**
* \brief
*  Obtains if an exem field group is LEXEM.
* \param [in] unit                    - Device ID.
* \param [in] fg_id                   - Field Group.
* \param [out] app_db_id_size_p       - The size in bits of the app_db_id.
* \param [out] app_db_id_p            - The app_db_id of the field group.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_app_db_id_get(
    int unit,
    dnx_field_group_t fg_id,
    int *app_db_id_size_p,
    dnx_field_app_db_id_t * app_db_id_p);

/**
* \brief
*  Obtains if an exem field group is LEXEM.
* \param [in] unit                    - Device ID.
* \param [in] fg_id                   - Field Group.
* \param [out] vmv_size_p             - The size in bits of the vmv.
* \param [out] vmv_value_p            - The vmv the field group.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_vmv_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *vmv_value_p,
    uint8 *vmv_size_p);

shr_error_e dnx_field_group_name_get(
    int unit,
    dnx_field_group_t fg_id,
    char name[DBAL_MAX_STRING_LENGTH]);

/**
 * \brief
 *  Get Whetehr a KBP field group supports packing (has the packing flag used, BCM_FIELD_FLAG_QUAL_PACKING).
 * \param [in] unit     - Device ID
 * \param [in] fg_id    - Field group ID
 * \param [out] is_support_packing_p - TRUE if BCM_FIELD_FLAG_QUAL_PACKING flag is used for the field group.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_kbp_support_packing(
    int unit,
    dnx_field_group_t fg_id,
    int *is_support_packing_p);

/**
 * \brief
 *  Get the field group ID by name;
 * \param [in] unit     - Device ID
 * \param [in] name     - The name of the field group. Number of elements in array DBAL_MAX_STRING_LENGTH.
 * \param [out] fg_id_p - Field group ID. If not found, DNX_FIELD_GROUP_INVALID is returned.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_group_name_to_id(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    dnx_field_group_t * fg_id_p);

/**
 * \brief
 *  Get name of an ACE format.
 * \param [in] unit     - Device ID
 * \param [in] ace_id   - Field group ID
 * \param [out] name    - The name of the field group. Number of elements in array DBAL_MAX_STRING_LENGTH.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_ace_format_name_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    char name[DBAL_MAX_STRING_LENGTH]);

/**
 * \brief
 *  Get an ACE format ID by name;
 * \param [in] unit      - Device ID
 * \param [in] name      - The name of the field group. Number of elements in array DBAL_MAX_STRING_LENGTH.
 * \param [out] ace_id_p - Field group ID. If not found, DNX_FIELD_GROUP_INVALID is returned.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_ace_format_name_to_id(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    dnx_field_ace_id_t * ace_id_p);

/**
 * \brief
 *  Gets all field groups that are attached to the given program id in the given stage
 *
 * \param [in] unit        - Device ID
 * \param [in] field_stage - The stage of the program ID
 * \param [in] context_id  - Program ID to retrieve the field groups attached to it
 * \param [out] fg_ids     - Array of IDs of the field groups attached to the given program ID
 * \param [out] fg_count   - The total count of the field groups array
 *
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
shr_error_e dnx_field_group_context_fgs_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t * fg_ids,
    int *fg_count);

/**
 * \brief
 *   Calculates the total number of bits used by the actions and their valid bits on the payload for function
 *   dnx_field_group_tcam_key_and_action_length_type_calc().
 *   This function is used for field groups that use lookup and 2msb feature (TCAM, EXEM).
 * \param [in] unit            -
 *   Device ID
 * \param [in] field_stage     -
 *   Field Stage (iPMF1/2..)
 * \param [in]  actions_info_arr    -
 *   Array of type dnx_field_action_in_group_info_t.
 *   This is a pointer to an array.
 *   Maximal number of actions accepted here: DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP.
 *   The list of actions ends when an action of value 'DNX_FIELD_ACTION_INVALID' is detected.
 *   Only the 'dnx_action' and 'dont_use_valid_bit' are read to calculate the action size of the sizes,
 *   'lsb' field need not be filled.
 * \param [out] actions_size_in_bits_p  -
 *   This procedure loads each pointed memory by the sum of sizes, in bit resolution, of all actions and valid bits
 *   in the array of 'actions_info_arr'.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure takes a two dimensional array of actions, and uses
 *     dnx_field_group_calc_actions_bit_size_1_arr() to find the bit size of each of the four one dimensional arrays.
 * \see
 *   * dnx_field_ace_format_calc_actions_bit_size().
 */
shr_error_e dnx_field_group_calc_actions_bit_size(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *actions_size_in_bits_p);

/**
* \brief
*  Find the size in bits of the payload block used by the lookup mechanism for a certain action_length_type enum.
* \param [in] unit                  - Device ID
* \param [in] action_length_type    - An enum indication the size of the action block used by the lookup mechanism.
* \param [out] block_size           - The number of bits available on the payload to write to
*                                     (to which we do MDB align to the payload we want to write).
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_action_length_to_bit_size(
    int unit,
    dnx_field_action_length_type_e action_length_type,
    unsigned int *block_size);

/**
 * \brief
 *  Get the information about Field group which is related to dnx_field_group_add();
 * \param [in] unit          - Device ID
 * \param [in] fg_id         - Field Group id to get its info
 * \param [out] fg_basic_info_p - Pointer to dnx_field_group_info_t that holds basic FG information.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * In case of TCAM field group retrieve the TCAM handler-ID and the access profile-ID for the Field group
 * \see
 *   * dnx_field_group_get
 */
shr_error_e dnx_field_group_basic_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_basic_info_p);

/**
 * \brief
 *  Get the information about Field group.
 * \param [in] unit          - Device ID
 * \param [in] fg_id         - Field Group id to get its info
 * \param [out] fg_info_p    -
 *   Pointer to dnx_field_group_full_info_t that holds full FG information,
 *   mainly from sw state.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * In case of TCAM field group retrieve the TCAM handler-ID and the access profile-ID for the Field group
 * \see
 *   * dnx_field_group_basic_info_get
 */
shr_error_e dnx_field_group_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p);

/**
 * \brief
 *  Get the information about ACE format which is related to dnx_field_ace_format_add();
 * \param [in] unit          - Device ID
 * \param [in] ace_id        - ACE format ID.
 * \param [out] ace_id_info_p -
 *   Pointer to dnx_field_ace_format_full_info_t that holds full ACE format information,
 *   mainly from sw state.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_ace_format_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_full_info_t * ace_id_info_p);

/**
* \brief
*  Adds a field group of ACE table (PPMC table) lookup for MC replication entries and ACE pointers.
*  Parrallels field group add and field group context attach.
* \param [in] unit                - Device ID
* \param [in] flags               - Can be WITH_ID
* \param [in] ace_format_info_in_p - All the information required to configure the ACE format.
*                                   Includes an array of actions that the ACE format can perform.
* \param [in,out] ace_id_p        - ID of the ACE format.
*                                   Also serves as result type of the PPMC entries and context of the ACE ACR EFES).
*                                   Serves as input if WITH_ID flag is set.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_ace_format_add(
    int unit,
    dnx_field_ace_format_add_flags_e flags,
    dnx_field_ace_format_info_t * ace_format_info_in_p,
    dnx_field_ace_id_t * ace_id_p);

/**
 * \brief
 * Returns the first entry handle for the given fg_id.
 * The first entry handle for a given handler is defined to be the handle
 * of the entry that both belongs to the field group and is the first
 * in line between all other entries that belongs to the same field group
 * inside the TCAM banks of the given core.
 *
 * \param [in] unit            - Device ID
 * \param [in] core            - Core ID
 * \param [in] fg_id           - ID of the field group to get the first entry id for
 * \param [out] first_entry_handle - The id of the first entry for the given field group - if exists.
 *                               DNX_FIELD_ENTRY_ACCESS_ID_INVALID otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_group_entry_first(
    int unit,
    int core,
    dnx_field_group_t fg_id,
    uint32 *first_entry_handle);

/**
 * \brief
 * Returns the next entry handle following the given current entry handle for the specified field group id.
 * The next entry is defined to be the next-in-line to come inside the TCAM Banks of the given core after
 * the given entry handle that belongs to the given field group.
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] fg_id              - ID of the field group to get the next entry handle for
 * \param [in] cur_entry_handle   - The entry handle that precedes the entry that will be returned
 * \param [out] next_entry_handle - The entry handle that both belongs to the handler and comes
 *                                  after the given entry_id - if exists.
 *                                  DNX_FIELD_ENTRY_ACCESS_ID_INVALID otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_group_entry_next(
    int unit,
    int core,
    dnx_field_group_t fg_id,
    uint32 cur_entry_handle,
    uint32 *next_entry_handle);

/* Group Entry Section */
/* { */

/**
* \brief
*  Delete all HW entries that were created for specific Field Group
* \param [in] unit   - Device ID
* \param [in] fg_id  - Field group to delete entries for
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_entry_delete_all(
    int unit,
    dnx_field_group_t fg_id);

/**
* \brief
*  Delete all HW entries that were created for all field groups.
* \param [in] unit   - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_group_all_entry_delete_all(
    int unit);

shr_error_e dnx_field_group_kbp_set(
    int unit);

/**
* \brief
*  Returns the TCAM handler IDs of all the FGs that share a context
*  with the given tcam_handler_id's FG.
*
* \param [in] unit              - Device ID
* \param [in] tcam_handler_id   - The TCAM handler ID for the FG to get sharing context FGs tcam handler IDs for
* \param [out] tcam_handler_ids - Array of all the TCAM handler IDs that their corresponding FGs share a context with the given TCAM handler ID's FG (Should be allocated by caller with size DNX_DATA_MAX_FIELD_TCAM_NOF_TCAM_HANDLERS + 1). The array ends with DNX_FIELD_TCAM_HANDLER_INVALID define.
*
* \return
*   shr_error_e - Error Type
* \remark
*   * This function is used by the TCAM bank manager to figure out which
*     TCAM handlers share the same context in order to prevent them from
*     sharing the same bank (HW limitation)
* \see
*   * tcam/tcam_bank_manager.c
*/
shr_error_e dnx_field_group_context_sharing_tcam_handlers_get(
    int unit,
    uint32 tcam_handler_id,
    uint32 tcam_handler_ids[]);

/**
 * \brief
 * Evacuates the given fg_id from the given bank_id on all cores.
 * The evacuation process involves moving all fg's entries on the
 * given bank to other banks allocated by the same fg.
 * If the evacuation process succeeds, the fg is removed from the bank (therefore,
 * helping to remove constraints for other fgs to use the evacuated bank).
 *
 * This function starts moving entries to other banks allocated by the given fg
 * until either all fg's entries on the bank are moved to other banks, or no
 * more empty space is left in the other banks allocated by the given fg.
 * In case no empty space is left, all the entries that were moved until this point stay
 * as-is and the function fails.
 *
 * \param[in] unit       - Device ID
 * \param[in] fg_id      - FG ID to move its entries from given bank
 * \param[in] nof_banks  - Number of banks to evacuate
 * \param[in] bank_ids   - Array of bank IDs to evacuate
 *
* \return
*   shr_error_e - Error Type
 */

shr_error_e dnx_field_group_tcam_bank_evacuate(
    int unit,
    dnx_field_group_t fg_id,
    int nof_banks,
    int bank_ids[]);

/**
 * \brief
 * Adds the given bank_ids to the given FG-ID.
 *
 * \param[in] unit       - Device ID
 * \param[in] fg_id      - FG ID to add to banks
 * \param[in] nof_banks  - Number of banks to add
 * \param[in] bank_ids   - Array of bank IDs to add the FG to
 *
* \return
*   shr_error_e - Error Type
 */

shr_error_e dnx_field_group_tcam_bank_add(
    int unit,
    dnx_field_group_t fg_id,
    int nof_banks,
    int bank_ids[]);

/**
 * \brief
 *  Returns the FG ID of the given entry ID.
 *
 * \param [in] unit        - Device ID
 * \param [in] entry_id    - Entry ID to return the FG ID for
 * \param [out] fg_id      - FG ID of the given entry ID
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
shr_error_e dnx_field_group_tcam_entry_fg_id(
    int unit,
    bcm_field_entry_t entry_id,
    bcm_field_group_t * fg_id);

/**
 * \brief
 * Initialize the master key info in SW state, specifically the number of segments used by FWD.
 * \param[in] unit       - Device ID
 * \return
 *   shr_error_e - Error Type
 */
shr_error_e dnx_field_group_kbp_master_key_init(
    int unit);

/* } */

#endif
