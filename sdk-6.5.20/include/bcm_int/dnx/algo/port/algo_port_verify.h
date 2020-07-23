/** \file algo_port_verify.h
 *
 *  PORT manager, Port utilities, and Port verifiers.
 *
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_PORT_VERIFY_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_VERIFY_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief -
 * Verify that logical port in range and free
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_free_verify(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * Verify that logical port in range and valid (i.e. allocated)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_valid_verify(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * Verify that port type is suppored in device.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] port_type - port type to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_type_supported_verify(
    int unit,
    dnx_algo_port_type_e port_type);

/**
 * \brief -
 * Verify nif phy id supported
 *
 * \param [in] unit - unit #
 * \param [in] nif_phy - nif phy id
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phy_verify(
    int unit,
    int nif_phy);

/**
 * \brief -
 * Verify fabric phy id supported
 *
 * \param [in] unit - unit #
 * \param [in] fabric_phy - fabric phy id
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_phy_verify(
    int unit,
    int fabric_phy);

/**
 * \brief -
 * Verify new nif port
 * Relevant for both ethrnet port and interlaken ports.
 * * If channlized interface - makes sure that nif is identical
 * * Otherwise - checks that other ports dont the same phys.
 *
 * \param [in] unit - unit #
 * \param [in] logical_port - required logical port.
 * \param [in] port_info - basic info about port.
 * \param [in] info - info required for adding nif port (see struct definition)
 *
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_if_add_t * info);

/**
 * \brief -
 * Should be used before adding or changing ilkn over fabric master port phys.
 * * Verify that all phys supported by the current device (using dnx data)
 * * Verify that the phys are not used by any other port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] phys- required phys bitmap to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_over_fabric_phys_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t phys);

/**
 * \brief -
 * Should be used before adding or changing ilkn  master port phys.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - device core.
 *   \param [in] port - logical port #
 *   \param [in] ilkn_id - ilkn interface id
 *   \param [in] phys- required phys bitmap to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_phys_verify(
    int unit,
    int core,
    bcm_port_t port,
    uint32 ilkn_id,
    bcm_pbmp_t phys);

/**
 * \brief -
 * Should be used before adding or changing master port phys.
 * * Verify that all phys supported by the current device (using dnx data)
 * * Verify that the phys are not used by any other port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] port_info - basic info about port
 *   \param [in] phys- required phys bitmap to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phys_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    bcm_pbmp_t phys);

/**
 * \brief -
 * Verify that core X tm_port are in supported range (per device)
 * And the relevant port mark as valid.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] tm_port - tm port #
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_port_valid_verify(
    int unit,
    bcm_core_t core,
    int tm_port);

/**
 * \brief -
 * Verify that core X tm_port are in supported range (per device)
 * And the relevant port mark as free.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port_info - basic info about port
 * \param [in] core - core #
 * \param [in] tm_port - tm port #
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_port_free_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    bcm_core_t core,
    int tm_port);

/**
 * \brief -
 * Verify that tm_port is in range defined by actual number of FAP IDs
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] tm_port - tm port #
 * \param [in] nof_fap_ids_per_core - actual number of FAP IDs per core
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * if actual number of FAP IDs per core is zero, the check is not performed and
 *   * needs to be called later after registering FAP IDs
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_port_allowed_verify(
    int unit,
    bcm_core_t core,
    int tm_port,
    int nof_fap_ids_per_core);

/**
 * \brief -
 * Verify that core X HR are in supported range (per device)
 * And the relevant HR mark as valid.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] hr_id - HR id #
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_hr_valid_verify(
    int unit,
    bcm_core_t core,
    int hr_id);

/**
 * \brief -
 * Verify that core X HR are in supported range (per device)
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] hr_id - HR id #
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_hr_param_verify(
    int unit,
    bcm_core_t core,
    int hr_id);

/**
 * \brief -
 * Verify that core X pp_port are in supported range (per device)
 * And the relevant port mark as valid.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - core #
 *   \param [in] pp_port - pp port #
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_port_valid_verify(
    int unit,
    bcm_core_t core,
    int pp_port);

/**
 * \brief -
 * Verify that fabric link is in supported range (per device)
 * And the relevant link is mark as valid.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] fabric_link - fabric link #
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_link_valid_verify(
    int unit,
    int fabric_link);

/**
 * \brief -
 * Verify that client index is in supported range (per device)
 * And the relevant client is mark as valid.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] port_type - might be FLEXE_SAR FLEXE_MAC or FLEXE_CLIENT
 *   \param [in] client_index - client index
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_flexe_client_index_valid_verify(
    int unit,
    dnx_algo_port_type_e port_type,
    int client_index);

/**
 * \brief -
 * Verify that tdm mode is supported in device.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] tdm_mode - tdm mode to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tdm_mode_supported_verify(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode);

/**
 * \brief -
 * Verify that egress interface id is supported.
 *
 * \param [in] unit - unit #.
 * \param [in] if_id - egress interface id
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_egr_if_id_verify(
    int unit,
    int if_id);

/**
 * \brief -
 * Verify that sch interface id is supported.
 *
 * \param [in] unit - unit #.
 * \param [in] if_id - sch interface id
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_if_id_verify(
    int unit,
    int if_id);

/**
 * \brief -
 * Verify the logical port port_type
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port #
 * \param [in] port_type - required port type
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_type_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type);

/**
 * \brief -
 * Verify that inteface offset is supported
 *
 * \param [in] unit - Unit #.
 * \param [in] port_info - basic port info
 * \param [in] interface_offset - interface_offset
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_special_interface_offset_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    int interface_offset);

/**
 * \brief -
 * Verify that STIF and none STIF ports are not mixed in one port macro
 *
 * \param [in] unit - Unit #.
 * \param [in] port - logical port
 * \param [in] port_info - basic port info
 * \param [in] first_phy - first phy of the port
 * \param [in] is_over_fabric - is_over_fabric. relevant for ilkn port
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_stif_verify(
    int unit,
    bcm_port_t port,
    dnx_algo_port_info_s port_info,
    int first_phy,
    int is_over_fabric);

/**
 * \brief - Verifies that TDM configurations can't be made in case
 * where FlexE is enabled
 *
 * \param [in] unit - chip unit id.
 * \param [in] tdm_is_set - tdm configuration is set
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tdm_usage_verify(
    int unit,
    int tdm_is_set);

#endif /* ALGO_PORT_VERIFY_H_INCLUDED */
