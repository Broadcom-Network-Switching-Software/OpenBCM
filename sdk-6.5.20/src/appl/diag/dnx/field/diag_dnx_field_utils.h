/** \file diag_dnx_field_utils.h
 *
 * Diagnostics utilities to be used for DNX diagnostics of Field Module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_UTILS_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_UTILS_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * Maximum size of the 'str' string in the DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(),
 * to be sure that the given input will be stored in. It is used only inside the
 * string verify macro, memory about it is being allocated and freed after calculations
 * or in case of an error.
 * The value of 5000 is chosen, because we assume that for the moment it can be the max
 * input, which can be parsed to the macro. It can be increased in the future.
 */
#define DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE               5000
/**
 * Maximum string size, to be used in field diagnostics for string buffers like:
 * CTX IDs, FG IDs, presel qualifiers (names, values, args, masks),
 * FFC Groups info (used/free IDs), Key IDs, compare modes, Entry IDs.
 */
#define DIAG_DNX_FIELD_UTILS_STR_SIZE               200
/**
 * \brief
 *  Verify that the size of the input value to buff_str[] is not bigger than
 *  the maximum allowed in the field diagnostics DIAG_DNX_FIELD_UTILS_STR_SIZE.
 *  In case yes, an error will be returned (_SHR_E_PARAM).
 *  Used also in case we want to concatenate the buff_str[] to the given
 *  '_current_str'. Then we will check if the '_current_str' length plus
 *  the input value length is not bigger then the total length of the '_current_str'.
 *
 * param [in] _current_str - In case of concatenation, the string to which we
 *  are going to concatenate the 'buff_str[]'. In other cases, like if we want
 *  to verify the 'buff_str[]' input, this parameter must be '0'.
 * param [in] _format - Format of the string, which will be stored in the buff_str[].
 *  Like "%d, %s, %X...".
 * param [in] __VA_ARGS__ - All input params, which are described in the given _format, to be
 *  stored in the buff_str[]. Like (num_one, str_one, hex_num_one...).
 *  In case of concatenation we should give the 'buff_str[]' and '_format' of "%s".
 *
 * return
 *   Error code (as per 'shr_error_e').
 */
#define DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(_current_str, _format, ...)                               \
{                                                                                                       \
    char *_str = NULL;                                                                                  \
    int *_current_str_addr = (int *) _current_str;                                                      \
    int _str_size = 0, _current_str_len = 0, _current_str_total_len = 0;                                \
    SHR_ALLOC(_str, DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE, "_str", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);    \
    sal_snprintf(_str, DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE, _format, __VA_ARGS__);                      \
    _str_size = sal_strnlen(_str, DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE);                                 \
    if (_current_str_addr != 0)                                                                         \
    {                                                                                                   \
        _current_str_total_len = sizeof(_current_str);                                                  \
        _current_str_len = sal_strnlen(_current_str, sizeof(_current_str));                             \
        if ((_current_str_len + _str_size) > _current_str_total_len)                                    \
        {                                                                                               \
            LOG_ERROR_EX(BSL_LOG_MODULE,                                                                \
                     "\r\n"                                                                             \
                     "Given string (%s) size (%d) is bigger then the current string size %d!%s\r\n",    \
                     _str, (_str_size + _current_str_len), _current_str_total_len, EMPTY);              \
            SHR_FREE(_str);                                                                             \
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);                                                       \
            SHR_EXIT();                                                                                 \
        }                                                                                               \
    }                                                                                                   \
    else if (_str_size > DIAG_DNX_FIELD_UTILS_STR_SIZE)                                                 \
    {                                                                                                   \
        LOG_ERROR_EX(BSL_LOG_MODULE,                                                                    \
                     "\r\n"                                                                             \
                     "Given string (%s) size (%d) is bigger then the maximum allowed size %d!%s\r\n",   \
                     _str, _str_size, DIAG_DNX_FIELD_UTILS_STR_SIZE, EMPTY);                            \
        SHR_FREE(_str);                                                                                 \
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);                                                           \
        SHR_EXIT();                                                                                     \
    }                                                                                                   \
    SHR_FREE(_str);                                                                                     \
}
/*
 * }
 */

/*
 * typedefs
 * {
 */
typedef struct
{
    /*
     * IRPP, ERPP, ETPP...
     */
    char block[RHSTRING_MAX_SIZE];
    /*
     * The Stage that show the signals 'from' for iPMF2 it iPMF1 and not iPMF2
     */
    char stage[RHSTRING_MAX_SIZE];
    /*
     * The next block that signal are show to, for example for iPMF1 its FER and not iPMF2
     */
    char to[RHSTRING_MAX_SIZE];
    /*
     * Stage that access to tcam such as iPMF2
     */
    char to_tcam[RHSTRING_MAX_SIZE];
    /*
     * TCAM block name
     */
    char tcam[RHSTRING_MAX_SIZE];
} diag_dnx_field_utils_sig_block_names_t;
/*
 * }
 */

/**
 * \brief
 *  init structure diag_dnx_field_utils_sig_block_names_t
 * \param [in] unit - The unit number.
 * \param [out] names_p - pointer to the structure to init.
 */
shr_error_e diag_dnx_field_utils_sig_block_names_t_init(
    int unit,
    diag_dnx_field_utils_sig_block_names_t * names_p);

/**
 * \brief
 *  init structure diag_dnx_field_utils_sig_block_names_t
 * \param [in] unit - The unit number.
 * \param [in] block - Block of the signal.
 * \param [in] stage - PMF stage name to read the signal from.
 * \param [in] to - the next stage after PMF.
 * \param [in] to_tcam - interface that access TCAM
 * \param [in] tcam - TCAM block name.
 * \param [out] names_p - pointer to the structure to set.
 */
shr_error_e diag_dnx_field_utils_sig_block_names_t_set(
    int unit,
    char block[RHSTRING_MAX_SIZE],
    char stage[RHSTRING_MAX_SIZE],
    char to[RHSTRING_MAX_SIZE],
    char to_tcam[RHSTRING_MAX_SIZE],
    char tcam[RHSTRING_MAX_SIZE],
    diag_dnx_field_utils_sig_block_names_t * names_p);

/**
 * \brief
 *  Converts field group type name to an appropriate string, which will be exposed in the diagnostic.
 * \param [in] unit - The unit number.
 * \param [in] fg_type          - Field group type
 * \param [out] converted_fg_type_p - The converted string of FG type
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - The field group type is invalid
 */
shr_error_e diag_dnx_field_utils_group_type_conversion(
    int unit,
    dnx_field_group_type_e fg_type,
    char *converted_fg_type_p);

/**
 * \brief
 *  Converts field key_size enum (dnx_field_key_length_type_e) to its corresponding
 *  value in bits (only for TCAM).
 * \param [in] unit                 - unit
 * \param [in] key_size          - Field's key size enum we want to convert
 * \param [out] key_size_in_bits - The converted key size in bits
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Key size enum received is not of type TCAM
 */
shr_error_e diag_dnx_field_utils_key_size_conversion(
    int unit,
    dnx_field_key_length_type_e key_size,
    uint32 *key_size_in_bits);

/**
 * \brief
 *  Converts field action_size enum (dnx_field_action_length_type_e) to its corresponding
 *  value in bits (only for TCAM).
 * \param [in] unit                 - Device ID
 * \param [in] action_size          - Field's action size enum we want to convert
 * \param [out] action_size_in_bits - The converted action size in bits
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - Action size enum received is not of type TCAM
 */
shr_error_e diag_dnx_field_utils_action_size_conversion(
    int unit,
    dnx_field_action_length_type_e action_size,
    uint32 *action_size_in_bits);

/**
 * \brief
 *  Converts context cmp, hash, state mode names to an appropriate string, which will be exposed in the diagnostic.
 * \param [in] unit  - The unit number.
 * \param [in] mode  - Context cmp, hash, state mode to be converted.
 * \param [out] converted_mode_p - The converted string of Context cmp, hash, state mode
 * \param [in] is_hash  - Indicates if context mode is hash.
 * \param [in] is_state  - Indicates if context mode is state table.
 * \return
 *  \retval _SHR_E_NONE  - success
 */
shr_error_e diag_dnx_field_utils_cmp_hash_state_mode_conversion(
    int unit,
    int mode,
    char *converted_mode_p,
    int is_hash,
    int is_state);

/**
 * \brief
 *  Returns a string, which contains all valid context modes, based on given
 *  context_mode info.
 * \param [in] unit  - The unit number.
 * \param [in] context_mode  - Context mode info (cmp, hash, state table).
 * \param [out] mode_string_p - String, which will store all the context modes.
 *  Please make sure the given array of char is with size of DIAG_DNX_FIELD_UTILS_STR_SIZE.
 *
 * \return
 *  \retval _SHR_E_NONE  - success
 */
shr_error_e diag_dnx_field_utils_context_modes_string_get(
    int unit,
    dnx_field_context_mode_t context_mode,
    char *mode_string_p);

/**
 * \brief
 *  This functions returns information for compare and hash modes for a given context.
 * \param [in] unit  - The unit number.
 * \param [in] context_id  - Context Id for which info should be retrieved.
 * \param [in] mode  - Context mode info (cmp, hash, state table).
 * \param [out] compare_info_p - Structure to store the returned information about context compare.
 * \param [out] hashing_info_p - Structure to store the returned information about context hashing.
 * \return
 *  \retval _SHR_E_NONE  - success
 */
shr_error_e diag_dnx_field_utils_cmp_hash_key_usage_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t mode,
    dnx_field_context_compare_info_t * compare_info_p,
    dnx_field_context_hashing_info_t * hashing_info_p);

/**
 * \brief
 *    Retrieves the name of an FFC group type.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *  Field stage for which the FFC Groups info will be taken.
 * \param [in] ffc_group_num -
 *    The FFC group within the stage.
 * \param [out] ffc_group_name -
 *    The name of the field ffc group.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_ffc_groups_names_get(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int ffc_group_num,
    char ffc_group_name[DIAG_DNX_FIELD_UTILS_STR_SIZE]);

/**
 * \brief
 *    Retrieves the number of allocated FEM IDs for given Field Group.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] context_id -
 *    Indicator for which 'context' this function checks against. used for FEMs that are invalidated per context.
 *    If set to 'DNX_FIELD_CONTEXT_ID_INVALID', this filter is ignored.
 * \param [in] fg_id -
 *  Field Group, to retrieve allocated FEMs.
 * \param [out] nof_fems_p -
 *  Number of allocated FEMs for the given FG.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_nof_alloc_fems_per_fg_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * nof_fems_p);

/**
 * \brief
 *    Retrieves the number of allocated added EFES IDs for given Field Group and context.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] context_id - Context id.
 * \param [in] fg_id -
 *  Field Group, to retrieve added EFESs.
 * \param [out] nof_allocated_added_efes_p -
 *  Number of allocated added EFESs for the given FG and context.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_nof_alloc_added_efes_per_fg_ctx_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_fes_id_t * nof_allocated_added_efes_p);

/**
 * \brief
 *    Retrieves the last key information for a given DBAL table, core and key_id.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] table_id -
 *  The last key DBAL table.
 * \param [in] core_id -
 *  Core Id for which the key value will be returned.
 * \param [in] key_id -
 *  Key for which value will be returned.
 * \param [out] field_key_value_p -
 *  The value of the given key.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_last_key_table_info_get(
    int unit,
    dbal_tables_e table_id,
    bcm_core_t core_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *field_key_value_p);

/**
 * \brief
 *    Retrieves the entry key information for an EXEM field group.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_info_p -
 *  Pointer, which contains all info about the EXEM FG.
 * \param [in] key_values_p -
 *  Values of the used Keys, by EXEM FG, taken from the signals.
 * \param [out] entry_key_p -
 *  The entry key info.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_exem_entry_key_info_get(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    uint32 *key_values_p,
    dnx_field_entry_key_t * entry_key_p);

/**
 * \brief
 *    Retrieves the number of available DT entries per one TCAM bank.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fg_id -
 *  Direct table field group ID.
 * \param [in] core_id -
 *  Core ID to be used for entry_handle construction.
 * \param [out] num_available_entries_p -
 *  Number of available entries in one TCAM bank.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_dt_available_entries_get(
    int unit,
    dnx_field_group_t fg_id,
    bcm_core_t core_id,
    uint32 *num_available_entries_p);

/**
 * \brief
 *    Returns the signal value, of given DNX_QUAL, as a raw data.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP (depends on the field_stage).
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_qual - DNX qualifier.
 * \param [out] out_qual_sig_value - Array to store that Qual signal raw value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_qual_signal_value_raw_data_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    uint32 out_qual_sig_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY]);

/**
 * \brief
 *    Returns the signal value, of given DNX_ACTION, as a raw data.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP (depends on the field_stage).
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_action - DNX action.
 * \param [out] out_action_sig_value_p - Pointer to return the action signal raw value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_action_signal_value_raw_data_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_action,
    uint32 *out_action_sig_value_p);

/**
 * \brief
 *    Retrieves the value of given user define MetaData qualifier, from the PBUS, by given offset.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP (depends on the field_stage).
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_qual - DNX qualifier.
 * \param [in] qual_attach_offset - User qualifier offset, which was set by the user in
 *  the attach info.
 * \param [out] out_qual_value - String with the constructed user qualifier value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_user_qual_metadata_value_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    int qual_attach_offset,
    char *out_qual_value);

/**
 * \brief
 *    Returns key size used by FWD stage, from the master key (in bits).
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] context_profile -
 *  Context profile (FWD_CONTEXT) for which value will be returned.
 * \param [out] key_size_used_by_fwd_p -
 *  The used key size.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_fwd_key_size_get(
    int unit,
    uint8 context_profile,
    uint32 *key_size_used_by_fwd_p);

/**
 * \brief
 *    Retrieves an array with Payload IDs used by External ACL field groups.
 *
 * \param [in] unit -
 *  Identifier of HW platform.
 * \param [in] context_id -
 *  Context ID.
 * \param [out] payload_ids -
 *  Array to store the Payload IDs per FG. Invalid values are
 *  initialized to BCM_FIELD_INVALID.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_payload_ids_per_fg_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 payload_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS]);

/**
 * \brief
 *    Updates a string for the field groups / FWD that share a segment for a specific qualifier.
 *
 * \param [in] unit -
 *  Identifier of HW platform.
 * \param [in] fg_id -
 *  Field group ID. Only used for error messages.
 * \param [in] opcode_id -
 *  Opcode ID.
 * \param [in] seg_index -
 *  The segment index in the master key.
 * \param [in] ms_key_segments -
 *  The segments on the master key.
 * \param [in] nof_fwd_segments -
 *  The number of FWD segments on the master key.
 * \param [in] fg_payload_ids -
 *  The paylaod IDs being used by each field group attached to the opcode.
 * \param [in] attach_basic_info_p -
 *  The attach info of the qualifier.
 * \param [in] qual_iter -
 *  The qualifier index in the field group.
 * \param [in,out] segment_sharing_string -
 *  The string with the list of what shares the segment.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_kbp_segment_share_string_update(
    int unit,
    dnx_field_group_t fg_id,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int seg_index,
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint8 nof_fwd_segments,
    uint32 fg_payload_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS],
    dnx_field_group_attach_info_t * attach_basic_info_p,
    int qual_iter,
    char segment_sharing_string[DIAG_DNX_FIELD_UTILS_STR_SIZE]);

/**
 * \brief
 *    Retrieves the value of given Header qualifier, from the packet header signal.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] context_id - Context ID.
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_qual - DNX qualifier.
 * \param [in] attach_info_p - The attach info of the qualifier.
 * \param [out] out_qual_value - String with the qualifier value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_header_qual_signal_value_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    dnx_field_qual_attach_info_t * attach_info_p,
    char out_qual_value[DIAG_DNX_FIELD_UTILS_STR_SIZE]);

/**
 * \brief
 *    Retrieves the value of given LR qualifier, from the layer records info.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] core_id - Core Id on from which signal values will be retrieved.
 * \param [in] context_id - Context ID.
 * \param [in] field_stage - Field stage on which the qualifier was used.
 * \param [in] dnx_qual - DNX qualifier.
 * \param [in] attach_info_p - The attach info of the qualifier.
 * \param [out] out_qual_value - String with the qualifier value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_layer_record_qual_signal_value_get(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    dnx_field_qual_attach_info_t * attach_info_p,
    char out_qual_value[DIAG_DNX_FIELD_UTILS_STR_SIZE]);

/**
 * \brief
 *    Returns name of the given EFES key select value per field stage.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage - Field stage on which efes is configured.
 * \param [in] key_select - Key select value to be converted to text.
 * \param [out] key_select_name - Name of the given key select value.
 *
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_efes_key_select_text_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_fes_key_select_t key_select,
    char *key_select_name);

/**
 * \brief
 *  Converts given stage string to the relevant DNX stage enum value (dnx_field_stage_e).
 * \param [in] unit - The unit number.
 * \param [in] field_stage_str_p  - Stage string taken from the relevant stage input param enum.
 * \param [out] field_stage_enum_val_p - The converted DNX stage enum value.
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - The stage string is invalid
 */
shr_error_e diag_dnx_field_utils_stage_str_to_enum_val_conversion(
    int unit,
    char *field_stage_str_p,
    dnx_field_stage_e * field_stage_enum_val_p);

/**
 * \brief
 *  Converts given range type string to the relevant DNX range type enum value (dnx_field_range_type_e).
 * \param [in] unit - The unit number.
 * \param [in] field_range_type_str_p  - Range type string taken from the relevant range type input param enum.
 * \param [out] field_range_type_enum_val_p - The converted DNX range type enum value.
 * \return
 *  \retval _SHR_E_NONE  - success
 *  \retval _SHR_E_PARAM - The range type string is invalid
 */
shr_error_e diag_dnx_field_utils_range_type_str_to_enum_val_conversion(
    int unit,
    char *field_range_type_str_p,
    dnx_field_range_type_e * field_range_type_enum_val_p);

/**
 * \brief
 *    Configure and send an ITMH packet.
 * \param [in] unit -
 *    Identifier of HW platform.
 *
 * \return
 *    \retval SHR_E_NONE - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    None
 */
shr_error_e diag_dnx_field_utils_send_itmh_packet(
    int unit);

#endif /* DIAG_DNX_UTILS_H_INCLUDED */
