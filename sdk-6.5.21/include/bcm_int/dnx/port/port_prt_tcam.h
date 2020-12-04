/** \file bcm_int/dnx/port/port_prt_tcam.h
 *
 * Port Prt Tcam functions
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_PORT_PRT_TCAM_INCLUDED_H
/*
 * {
 */
#define DNX_PORT_PRT_TCAM_INCLUDED_H

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

/*
 * }
 */

/**
 * \brief -  Get virtual port TCAM access id according to TCAM key and core id.
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] prttcam_key_field_id - DBAL field id of TCAM entry`s key
 * \param [in] prttcam_key_value - The key of TCAM entry
 * \param [out] entry_access_id - hold access id of TCAM entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_access_id_get(
    int unit,
    bcm_port_t core_id,
    dbal_fields_e prttcam_key_field_id,
    uint32 prttcam_key_value,
    uint32 *entry_access_id);

/**
 * \brief
 *   Set sw table for virtual port TCAM management.
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] prt_key_field_id - DBAL key field id
 * \param [in] key_value - Key of TCAM entry
 * \param [in] access_id - Access id for TCAM entry
 * \param [in] priority - Priority of TCAM entry, useless for COE
 * \param [in] pp_port - Result.pp_port of TCAM entry
 * \param [in] sys_port - Result.sys_port of TCAM entry, useless for COE
 * \param [in] is_entry_clear - '1' means dbal entry clear, '0' means dbal commit
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_tcam_sw_set(
    int unit,
    bcm_core_t core_id,
    dbal_fields_e prt_key_field_id,
    uint32 key_value,
    uint32 access_id,
    uint16 priority,
    uint16 pp_port,
    uint16 sys_port,
    uint16 is_entry_clear);

/**
 * \brief
 *   Get virtual port TCAM management sw information according to key
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] prt_key_field_id - DBAL field id of TCAM entry`s key
 * \param [in] key_value - Key of TCAM entry
 * \param [out] access_id - Access id for TCAM entry
 * \param [out] is_used  - Indicates if the access id is in use
 * \param [out] priority - Ppriority of TCAM entry
 * \param [out] pp_port - Result.pp_port of TCAM entry
 * \param [out] sys_port - Result.sys_port of TCAM entry
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_tcam_sw_get(
    int unit,
    bcm_core_t core_id,
    dbal_fields_e prt_key_field_id,
    uint32 key_value,
    uint32 *access_id,
    uint8 *is_used,
    uint16 *priority,
    uint32 *pp_port,
    uint16 *sys_port);

/**
 * \brief
 *   Configure PRT_VIRTUAL_PORT_TCAM
 *
 * \param [in] unit - unit ID
 * \param [in] core_id - Core id
 * \param [in] prt_key_field_id - DBAL key field id
 * \param [in] key_value - Key of TCAM entry
 * \param [in] key_mask - Key mask of TCAM entry
 * \param [in] access_id - Access id
 * \param [in] pp_port - Result.pp_port of TCAM entry
 * \param [in] sys_port - Result.sys_port of TCAM entry, useless for COE
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_tcam_hw_set(
    int unit,
    bcm_core_t core_id,
    dbal_fields_e prt_key_field_id,
    uint32 key_value,
    uint32 *key_mask,
    uint32 access_id,
    uint16 pp_port,
    uint16 sys_port);

/**
 * \brief
 *   Clear an entry in PRT_VIRTUAL_PORT_TCAM
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] access_id - PRT_VIRTUAL_PORT_TCAM access id
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_tcam_hw_clear(
    int unit,
    bcm_core_t core_id,
    uint32 access_id);
/**
 * \brief - Allocate access id SW TCAM - update tables VIRTUAL_PORT_TCAM_SW and VIRTUAL_PORT_TCAM_SW_CONTROL
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] prttcam_key_field_id - DBAL field id of TCAM entry`s key
 * \param [in] key_value - Key of TCAM entry
 * \param [out] entry_access_id - Entry access id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_sw_access_id_alloc(
    int unit,
    bcm_core_t core_id,
    dbal_fields_e prttcam_key_field_id,
    uint32 key_value,
    uint32 *entry_access_id);

/**
 * \brief - Set core + pp_port before COE port adding into a LAG
 *
 * \param [in] unit - Unit ID
 * \param [in] local_port - COE local port ID
 * \param [in] core_id - core ID
 * \param [in] pp_port - pp port ID
 * \param [in] tcam_access_id - TCAM access ID
 * \param [in] key_value - TCAM key value
 * \param [in] is_entry_clear - 0: don't clear the entry, others clear the entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_set(
    int unit,
    uint16 local_port,
    uint32 core_id,
    uint32 pp_port,
    uint32 tcam_access_id,
    uint32 key_value,
    uint16 is_entry_clear);

/**
 * \brief - Get core + pp_port before COE port adding into a LAG
 *
 * \param [in] unit - Unit ID
 * \param [in] local_port - COE local port ID
 * \param [out] core_id - CORE id
 * \param [out] pp_port - pp port id
 * \param [out] tcam_access_id - TCAM access id
 * \param [out] key_value - TCAM key value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get(
    int unit,
    uint16 local_port,
    uint32 *core_id,
    uint32 *pp_port,
    uint32 *tcam_access_id,
    uint32 *key_value);

/*
 * }
 */
#endif /** DNX_PORT_PRT_TCAM_INCLUDED_H */
