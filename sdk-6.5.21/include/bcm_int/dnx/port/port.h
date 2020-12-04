/** \file bcm_int/dnx/port/port.h
 *
 * Adding and removing ports
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

/**
 * \brief - Set per tm_port out_pp_port and destination_system_port
 *          Provide mapping between source system port and gport
 *
 * \param [in] unit - Relevant unit
 * \param [in] logical_port - Logical port
 * \param [in] out_pp_port - Out pp port
 * \param [in] src_system_port - Source system port
 * \param [in] dst_system_port - Destination system port
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
    uint32 out_pp_port,
    uint32 src_system_port,
    uint32 dst_system_port);

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
 * \brief - get pp_dsp to out tm port mapping.
 *             this mapping for existing local ports should be 1 to 1 and it is taken as an assumption in many places.
 *             pp dsps that are currently not assotiated with any local ports will be set to invlaid_otm_port value.
 *
 * \param [in] unit - unit number
 * \param [in] core - core index
 * \param [in] pp_dsp - pp_dsp
 * \param [out] out_tm_port - out tm port to get
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_dsp_to_out_tm_mapping_get(
    int unit,
    int core,
    int pp_dsp,
    int *out_tm_port);

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
    int pp_dpp,
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
/*
 * }
 */
#endif /** DNX_PORT_INCLUDED_H */
