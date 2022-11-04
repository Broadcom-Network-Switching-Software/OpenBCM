/** \file bcm_int/dnx/port/port.h
 *
 * Adding and removing ports
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_PORT_INCLUDED_H
/*
 * {
 */
#define DNX_PORT_INCLUDED_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/port.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#if defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#endif
/*
 * }
 */

/*
 * DEFINES
 * {
 */
/** Indicate port type NONE in first header size template manager */
#define DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_NONE 0
/** Indicate port type ETH in first header size template manager */
#define DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_ETH 1
/** Indicate port type PTCH1 in first header size template manager */
#define DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH1 2
/** Indicate port type PTCH2 in first header size template manager */
#define DNX_PORT_FIRST_HEADER_SIZE_DATA_PORT_TYPE_PTCH2 3
/*
 * }
 */

/*
 * TYPES
 * {
 */
typedef enum dnx_port_nif_l1_only_mode_e
{
    DNX_PORT_NIF_L1_ONLY_MODE_DISABLED,
    DNX_PORT_NIF_L1_ONLY_MODE_WITHOUT_ILKN,
    DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN,
    DNX_PORT_NIF_L1_ONLY_MODE_COUNT
} dnx_port_nif_l1_only_mode_t;

/*
 * PRT configuration structure
 */
typedef struct dnx_port_prt_configuration_s
{
    /** Port terminatiom PTC profile */
    dbal_enum_value_field_port_termination_ptc_profile_e port_termination_ptc_profile;
    /** Recycle CMD PRT profile*/
    dbal_enum_value_field_recycle_cmd_prt_profile_e recycle_cmd_prt_profile;
    /** FFC0 offset */
    uint32 ffc_0_offset;
    /** FFC0 width */
    uint32 ffc_0_width;
    /** FFC0 input offset */
    uint32 ffc_0_input_offset;
    /** FFC1 offset */
    uint32 ffc_1_offset;
    /** FFC1 width */
    uint32 ffc_1_width;
    /** FFC1 input offset */
    uint32 ffc_1_input_offset;
    /** FFC2 offset */
    uint32 ffc_2_offset;
    /** FFC2 width */
    uint32 ffc_2_width;
    /** FFC2 input offset */
    uint32 ffc_2_input_offset;
    /** FFC3 offset */
    uint32 ffc_3_offset;
    /** FFC3 width */
    uint32 ffc_3_width;
    /** FFC3 input offset */
    uint32 ffc_3_input_offset;
    /** FFC4 offset */
    uint32 ffc_4_offset;
    /** FFC4 width */
    uint32 ffc_4_width;
    /** FFC4 input offset */
    uint32 ffc_4_input_offset;
    /** FFC5 offset */
    uint32 ffc_5_offset;
    /** FFC5 width */
    uint32 ffc_5_width;
    /** FFC5 input offset */
    uint32 ffc_5_input_offset;
    /** KBR_0 Valid Bitmap - Parsing start offset KBR key */
    uint32 parsing_start_offset_kbr_key;
    /** KBR_1 Valid Bitmap - VP TCAM or PP port direct KBR key */
    uint32 vp_tcam_or_pp_port_direct_kbr_key;
    /** KBR_2 Valid Bitmap - Source system port KBR key */
    uint32 source_system_port_kbr_key;
    /** KBR_3 Valid Bitmap - Parser context KBR key */
    uint32 parser_context_kbr_key;
    /** KBR_4 Valid Bitmap - PRT Qualifier KBR key */
    uint32 prt_qualifier_kbr_key;
    /** PP port select */
    uint32 pp_port_enable;
    /** Source system port PRT mode */
    uint32 source_system_port_prt_mode;
    /** Context PRT mode */
    uint32 context_prt_mode;
    /** PRT qualifier mode */
    uint32 prt_qualifier_mode;
    /** TCAM mode full enable */
    uint32 tcam_mode_full_enable;
    /** Layer offset */
    uint32 layer_offset;
} dnx_port_prt_configuration_t;
/*
 * }
 */

/**
 * \brief - Set per tm_port pp_port and system_port
 *          Provide mapping between source system port and gport
 *
 * \param [in] unit - Relevant unit
 * \param [in] logical_port - Logical port
 * \param [in] pp_port - PP port
 * \param [in] system_port - System port
 * \param [in] is_lag - Indication if the port is in lag
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_mapping_set(
    int unit,
    bcm_port_t logical_port,
    uint32 pp_port,
    uint32 system_port,
    uint32 is_lag);

/**
 * \brief - Configure general PP port configuration for the basic PP port addition
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_add_pp_init(
    int unit);

/**
 * \brief - Configure general PP port configuration for the basic PP port removal
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_remove_pp_init(
    int unit);

/**
 * \brief - retrieve status queries the PHY
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - port
 * \param [out] link - link status
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_link_get(
    int unit,
    bcm_port_t port,
    int *link);

/**
 * \brief - clear pp_dsp mapping table.
 *
 * \param [in] unit - unit number
 * \param [in] core - core index
 * \param [in] pp_dsp - pp_dsp
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_dsp_table_mapping_clear(
    int unit,
    int core,
    int pp_dsp);

/**
 * \brief - set pp_dsp to out tm port mapping.
 *             this mapping for existing local ports should be 1 to 1 and it is taken as an assumption in many places.
 *             pp dsps that are currently not associated with any local ports should be set to invlaid_otm_port value.
 *
 * \param [in] unit - unit number
 * \param [in] core - core index
 * \param [in] pp_dsp - pp_dsp
 * \param [in] out_tm_port - out tm port to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_dsp_to_out_tm_mapping_set(
    int unit,
    int core,
    int pp_dsp,
    int out_tm_port);

/**
 * \brief - set dsp_ptr_table
 *
 * \param [in] unit - unit number
 * \param [in] pp_dsp - PP DSP
 * \param [in] core - core index
 * \param [in] out_pp_port - pp port to set
 * \param [in] destination_system_port - destination system port
 *        to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * it is assumed that TM_PORT == PP_DSP when this function is used.
 * \see
 *   * None
 */
shr_error_e dnx_port_dsp_table_mapping_set(
    int unit,
    int pp_dsp,
    int core,
    int out_pp_port,
    uint32 destination_system_port);

/**
 * \brief - get dsp_ptr_table
 *
 * \param [in] unit - unit number
 * \param [in] pp_dsp - PP DSP
 * \param [in] core - core index
 * \param [in] out_pp_port - recieved pp port
 * \param [in] destination_system_port - recieved destination
 *        system port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * it is assumed that TM_PORT == PP_DSP when this function is used.
 * \see
 *   * None
 */
shr_error_e dnx_port_dsp_table_mapping_get(
    int unit,
    int core,
    int pp_dsp,
    int *out_pp_port,
    uint32 *destination_system_port);

/**
 * \brief - Get PP_DSP for a port
 *
 * \param [in] unit - Unit ID
 * \param [in] port - port ID
 * \param [out] pp_dsp - pp_dsp for the port
 * \param [out] core - core for coe port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_dsp_get(
    int unit,
    bcm_port_t port,
    int *pp_dsp,
    bcm_core_t * core);

/**
 * \brief - Get the port info according the aciton mask.
 */
shr_error_e dnx_port_selective_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info);

/**
 * \brief - Get all the support port info for the given port.
 */
shr_error_e dnx_port_info_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info);

/**
 * \brief
 *   Verify that port is from allowed types.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port
 *
 * \return
 *     shr_err_e
 * \remark
 *    * None
 * \see
 *    * None
 */
shr_error_e dnx_port_logical_verify(
    int unit,
    bcm_gport_t port);

/**
 * \brief
 *   Get the logical port of a specific port
 *
 * \param [in] unit - unit ID
 * \param [in] port - port
 * \param [out] logical_port - logical port
 * \return
 *     shr_err_e
 *
 * \remark
 *    * None
 * \see
 *    * None
 */
shr_error_e dnx_port_logical_get(
    int unit,
    bcm_gport_t port,
    bcm_port_t * logical_port);

/**
 * \brief - PHY Measure for all the IMB ports.
 */
shr_error_e dnx_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure);

/*
 * \brief
 *   Get SoC property values of phy lane config bits.
 *   set/clear the bits in port_resource.phy_lane_config accordingly
 */
shr_error_e dnx_port_resource_lane_config_soc_properties_get(
    int unit,
    bcm_port_t port,
    bcm_port_resource_t * port_resource);

/**
 * \brief - Configure the FlexE client resource
 *
 */
shr_error_e dnx_port_flexe_resource_set(
    int unit,
    bcm_port_t port,
    uint32 flags);

/**
 * \brief - ILKN RX link status change interrupt handler
 *
 * \param [in] unit - Relevant unit
 * \param [in] block_instance - ILKN block instance.
 * \param [in] port_in_core - Internal port index in core (0,1)
 */
shr_error_e dnx_port_ilkn_rx_status_change_interrupt_handler(
    int unit,
    int block_instance,
    uint8 *port_in_core);

/**
 * \brief - ILKN RX lane lock status change interrupt handler
 *
 * \param [in] unit              - Relevant unit
 * \param [in] cdu_id            - CDU instance index
 * \param [in] lane_in_cdu       - index of the lane in cdu
 */
shr_error_e dnx_port_ilkn_rx_lane_lock_status_change_interrupt_handler(
    int unit,
    int cdu_id,
    uint8 *lane_in_cdu);

/**
 * \brief - Configure per port the first header offset to skip
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] first_header_size_value - The size (in bytes) of the first header offset to skip
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_first_header_size_to_skip_set(
    int unit,
    bcm_port_t port,
    uint32 first_header_size_value);

/**
 * \brief - According to port header type return user defined port type that is used in the template manager for
 *      handling the profiles for skipping the first header size
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 * \param [in] switch_header_type - Port header type according to BCM_SWITCH_PORT_HEADER_TYPE_*
 * \param [out] user_defined_port_type - User defined port type: None, PTCH1, PTCH2 or ETH
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_first_header_size_to_skip_port_type_from_port_header_type_convert(
    int unit,
    bcm_port_t port,
    int switch_header_type,
    uint32 *user_defined_port_type);

/**
 * \brief - Get layer_offset from INGRESS_PRT_INFO
 *
 * \param [in] unit - Relevant unit
 * \param [in] prt_recycle_cmd - Prt recycle command according to dbal_enum_value_field_recycle_cmd_prt_profile_e
 * \param [in] ptc_profile - Port termination ptc profile according to dbal_enum_value_field_port_termination_ptc_profile_e
 * \param [out] layer_offset  - Layer offset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ingress_prt_info_layer_offset_get(
    int unit,
    dbal_enum_value_field_recycle_cmd_prt_profile_e prt_recycle_cmd,
    dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile,
    int *layer_offset);

/**
 * \brief - Get PRT configuration from DBAL
 *
 * \param [in] unit - Relevant unit
 * \param [in] port_termination_ptc_profile - Port Termination PTC profile
 * \param [in] recycle_cmd_prt_profile - Recycle command PRT profile
 * \param [out] prt_configuration - PRT configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_configuration_get(
    int unit,
    dbal_enum_value_field_port_termination_ptc_profile_e port_termination_ptc_profile,
    dbal_enum_value_field_recycle_cmd_prt_profile_e recycle_cmd_prt_profile,
    dnx_port_prt_configuration_t * prt_configuration);

/**
 * \brief - DNX wrapper function for portmod_thread_stop
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_portmod_thread_stop(
    int unit);

#if defined(INCLUDE_GDPLL)
/**
 * \brief - Get Nanosync input port
 *
 */
int dnx_port_to_nanosync_input_port_get(
    int unit,
    bcm_port_t logical_port,
    int lane_num,
    bcm_gdpll_port_event_t event_type,
    int *ns_port);
/**
 * \brief - Get Nanosync output port
 *
 */
int dnx_port_to_nanosync_output_port_get(
    int unit,
    bcm_port_t logical_port,
    int lane_num,
    int *ns_port);
#endif /* INCLUDE_GDPLL */
/**
 * \brief - Recover the original link config for
 *    FLR link up and non-FLR link up
 *
 */
shr_error_e dnx_port_advanced_flr_link_config_recover(
    int unit,
    bcm_port_t logical_port,
    int phy_lane);
/**
 * \brief - W40 FLR interrupt handler
 *
 */
shr_error_e dnx_port_w40_flr_interrupt_handler(
    int unit,
    int block_instance,
    int *phy_lane);
/*
 * }
 */
#endif /** DNX_PORT_INCLUDED_H */
