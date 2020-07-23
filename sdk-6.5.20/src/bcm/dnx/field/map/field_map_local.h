/** \file field_map_local.h
 *  *
 * Contains definitions to be used only internally by map module only
 *
 *  Created on: Dec 6, 2017
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SRC_BCM_DNX_FIELD_FIELD_MAP_LOCAL_H_
#define SRC_BCM_DNX_FIELD_FIELD_MAP_LOCAL_H_

/*
 * Include files
 * {
 */
#include <bcm_int/dnx/field/field_context.h>
/*
 * }
 */

/*
 * Globals
 *   For internal map submodule usage
 * {
 */
extern const dnx_field_context_param_t context_param_set[bcmFieldContextParamCount][bcmFieldStageCount];
extern const dnx_field_range_map_t range_map_legacy[DNX_FIELD_RANGE_TYPE_NOF][DNX_FIELD_STAGE_NOF];

extern dnx_field_sw_qual_info_t dnx_sw_qual_info[DNX_FIELD_SW_QUAL_NOF];
extern const dnx_field_qual_input_type_info_t dnx_input_type_info[DNX_FIELD_INPUT_TYPE_NOF];
extern const dnx_field_qual_map_t dnx_global_qual_map[bcmFieldQualifyCount];
extern const char *bcm_qual_description[bcmFieldQualifyCount];

extern dnx_field_header_qual_info_t dnx_header_qual_info[DNX_FIELD_HEADER_QUAL_NOF];
extern dnx_field_layer_record_qual_info_t dnx_layer_record_qual_info[DNX_FIELD_LR_QUAL_NOF];

extern const dnx_field_action_map_t dnx_global_action_map[bcmFieldActionCount];
extern dnx_field_base_action_info_t dnx_sw_action_info[DNX_FIELD_SW_ACTION_NOF];

extern const char *dnx_field_qual_class_names[DNX_FIELD_QUAL_CLASS_NOF];
extern const char *dnx_field_action_class_names[DNX_FIELD_ACTION_CLASS_NOF];
extern const char *bcm_action_description[bcmFieldActionCount];
extern const char *dnx_field_input_types_names[DNX_FIELD_INPUT_TYPE_NOF];

/*
 * }
 */
/*
 * Macros
 *   For internal map submodule usage
 * {
 */
/** Flags for BCM to DNX conversion cb */
#define         CHECK_BARE_METAL_SUPPORT     0x01
/**
 * Check whether the conversion CB is supported in BareMetal mode.
 * if yes, _SHR_E_NONE is returned, otherwise _SHR_E_UNAVAIL
 */
#define DNX_FIELD_CB_BLOCK_FOR_BARE_METAL(_flags)\
    if(_flags & CHECK_BARE_METAL_SUPPORT)        \
    {                                            \
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);     \
        SHR_EXIT();                              \
    }                                            \

#define DNX_FIELD_CB_UNBLOCK_FOR_BARE_METAL(_flags) \
    if(_flags & CHECK_BARE_METAL_SUPPORT)        \
    {                                            \
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);        \
        SHR_EXIT();                              \
    }                                            \
 /* 
  * } Macros
  */

/*
 * Type definitions
 *   For internal map submodule usage
 * {
 */

/**
 * Information assigned to each 'dnx action'
 * See dnx_field_map_action_info.
 */
typedef struct
{
    /*
     * Keep class inside info
     */
    dnx_field_action_class_e class;
    /*
     * Keep dnx stage inside info
     */
    dnx_field_stage_e dnx_stage;
    /**
     * Field ID associated with this action, to be used while configuring TCAM entries
     */
    dbal_fields_e field_id;
    /**
     * Actual action that is used to configure FES
     * This field in info should be used rather than action id part of dnx action
     */
    dnx_field_action_type_t action_type;
    /**
     * BCM action id
     */
    bcm_field_action_t bcm_action;
    /**
     * If the DNX action is a user defined action (class is DNX_FIELD_ACTION_CLASS_USER),
     * contains the action upon which the UDA is based.
     */
    dnx_field_action_t base_dnx_action;
    /**
     * If the DNX action is a user defined action (class is DNX_FIELD_ACTION_CLASS_USER),
     * contains the prefix of the action.
     */
    uint32 prefix;
    /**
     * If the DNX action is a user defined action (class is DNX_FIELD_ACTION_CLASS_USER),
     * contains the size in bits of the prefix of the action.
     */
    unsigned int prefix_size;
    /**
     * Signal or signals, which are corresponding to the action.
     */
    char *signal_name[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];

} dnx_field_map_action_info_t;

/**
 * \brief Structure returned by dnx qualifier to info mapping, include all data for CE(FFC) configuration
 */
typedef struct
{
    int size;
    /*
     * Keep class inside info
     */
    dnx_field_qual_class_e class;
    /*
     * Keep dnx stage inside info
     */
    dnx_field_stage_e dnx_stage;
    /**
     * Field ID associated with this qualifier, to be used while configuring TCAM entries
     */
    dbal_fields_e field_id;
    /**
     * Offset from base - type of base depends on FCC type
     */
    int offset;
    /*
     * bcm qualifier that is mapped to this dnx one, if 0 no bcm qualifier is mapped to it
     */
    bcm_field_qualify_t bcm_qual;
    /*
     * Info about signal or signals, which are corresponding to the qualifier.
     */
    dnx_field_map_qual_signal_info_t signal_info[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
} dnx_field_qual_info_t;

/**
 * Information collected from DNX DATA regarding a layer record qualifier.
 * See dnx_field_map_qual_layer_record_info.
 * see dnx_field_map_qual_info.
 */
typedef struct
{
    /*
     * Whether the layer record is valid for the unit/stage.
     */
    int valid;
    /*
     * The offset of the qualifier within the layer record.
     */
    int offset;
    /*
     * The size in bits of the qualifier
     */
    int size;
} dnx_field_map_qual_layer_record_pbus_info_t;

/*
 * }
 */

/*
 * All routines in this section serves for callback converting bcm quals and action data into dnx one
 * {
 */
shr_error_e dnx_field_convert_gport_to_dst(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_rif_intf_to_dst(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_gport_to_port_with_core(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_sys_port_gport_to_port(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_gport_to_port_without_core(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_trap_gport(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_trap_gport_to_hw_dest(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ingress_sniff_gport_to_code(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ingress_snoop_gport_to_code(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_egress_trap_gport_to_strength(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_egress_snoop_gport_to_strength(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_egress_trap_id(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_egress_sniff_gport_to_profile(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_gport_to_global_in_lif(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_gport_to_global_out_lif(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_gport_to_global_out_lif_add_valid(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_rif_intf_to_lif(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_rif_intf_to_lif_add_valid(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_rif_intf_to_rpf_out_lif_encoded(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_gport_to_rpf_out_lif_encoded(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_color(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_stat_lm_index(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_vlan_format(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_app_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_app_type_predef_only(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ext_stat(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ext_stat_with_valid(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_latency_flow_id(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_stat_profile(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_fwd_domain(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_oam(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ipt_command(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_packet_strip(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ace_stat_meter_object(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ace_stat_counter_object(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_parsing_start_offset(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_parsing_start_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_system_header_profile(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_learn_info_key_2(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_forwarding_type(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);

shr_error_e dnx_field_convert_ac_in_lif_wide_data_extended(
    int unit,
    uint32 flags,
    int core,
    uint32 *bcm_data,
    uint32 *dnx_data);
/*
 * }
 */

/*
 * All routines in this section serves for callback converting context parameters
 * {
 */
shr_error_e dnx_field_convert_context_sys_header_profile(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t context_id,
    uint32 param_value,
    struct dnx_field_context_param_e *context_param,
    dnx_field_dbal_entry_t * field_dbal_entry);

shr_error_e dnx_field_convert_context_param_key_val(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t field_context_id,
    uint32 param_value,
    struct dnx_field_context_param_e *context_param,
    dnx_field_dbal_entry_t * field_dbal_entry);

shr_error_e dnx_field_convert_context_param_header_strip(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_context_t field_context_id,
    uint32 param_value,
    struct dnx_field_context_param_e *context_param,
    dnx_field_dbal_entry_t * field_dbal_entry);
/*
 * }
 */

/*
 * All routines in this section serves for callback converting CS qual types
 * {
 */
/**
 * \brief
 *  Returns the context selection DNX qualifier for the cascaded BCM qualifier
 *  with the given fg_id (given as cs_qual_index) in the given context_id.
*/
shr_error_e dnx_field_convert_cs_qual_type_cascaded_group(
    int unit,
    uint32 cs_qual_index,
    dnx_field_context_t context_id,
    bcm_field_qualify_t bcm_qual,
    dbal_fields_e * cs_dnx_qual_p);

/**
 * \brief
 *  Returns the context selection BCM qualifier for the cascaded DNX qualifier
 *  and provides the fg_id (given as cs_qual_index_p) in the given context_id.
*/
shr_error_e dnx_field_convert_cs_qual_type_cascaded_group_back(
    int unit,
    dnx_field_context_t context_id,
    dbal_fields_e cs_dnx_qual,
    bcm_field_qualify_t * bcm_qual_p,
    uint32 *cs_qual_index_p);

/*
 * }
 */

/**
 * \brief  Retrieve the DNX DATA information about a layer record qualifier ID.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] stage - Stage identifier
 * \param [in] dnx_qual_id - DNX qualifier ID
 * \param [out] lr_qual_info_p - The information retrieved
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 */
shr_error_e dnx_field_map_qual_layer_record_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t dnx_qual_id,
    dnx_field_map_qual_layer_record_pbus_info_t * lr_qual_info_p);

/**
* \brief
*  Maps BCM System header profile to DNX system header profile ENUMS
* \param [in] unit                - Device Id.
* \param [in] bcm_sys_hdr_profile     - BCM Sys hdr profile Enum.
* \param [out] dnx_sys_hdr_profile_p  - DNX Sys hdr profile Enum.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_map_system_header_profile_bcm_to_dnx(
    int unit,
    bcm_field_system_header_profile_t bcm_sys_hdr_profile,
    dnx_field_context_sys_hdr_profile_e * dnx_sys_hdr_profile_p);

/**
* \brief
*  Gets the offset in the metadata of a signal that can be used by a qualifier.
*  Used for virtual wire qualifiers.
* \param [in] unit              - Device ID.
* \param [in] stage              - Stage
* \param [in] signal_name         - The name of the signal the VW is mapped to.
* \param [in] offset_within_signal - When an offset within the signal is required.
* \param [out] is_qual_available_p - Whether the signal can be read as qualifier.
* \param [out] offset_p         - The offset on the metadata of the signal.
*                                 only relevant if is_qual_available_p is TRUE.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_qual_vw_signal_offset(
    int unit,
    dnx_field_stage_e stage,
    char *signal_name,
    int offset_within_signal,
    int *is_qual_available_p,
    int *offset_p);

/**
* \brief
*  Checks whether the conversion CB has a BareMetal support check.
* \param [in] unit              - Device ID
* \param [in] conversion_cb     - conversion CB
* \param [in] bare_metal_support - param to verify the CB function support BareMetal
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_bare_metal_support_check(
    int unit,
    field_data_conversion_cb conversion_cb,
    int *bare_metal_support);

#endif /* SRC_BCM_DNX_FIELD_FIELD_MAP_LOCAL_H_ */
