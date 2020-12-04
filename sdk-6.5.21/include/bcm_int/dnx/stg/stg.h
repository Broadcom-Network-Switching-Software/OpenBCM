/*! \file bcm_int/dnx/stg/stg.h
 *
 *  Internal DNX STG APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */
#include <bcm_int/dnx_dispatch.h>
#ifndef STG_H_INCLUDED
/* { */
#define STG_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */
#define BCM_STG_MIN             BCM_STG_DEFAULT

/*
 * }
 */
/*
 * MACROs
 * {
 */

/** Encode topology_id with BCM stg_id.*/
#define BCM_DNX_STG_TO_TOPOLOGY_ID_GET(stg_id)             (stg_id - 1)

/** Encode BCM stg_id with topology_id.*/
#define BCM_DNX_TOPOLOGY_ID_TO_STG_GET(stg_topology_id)    (stg_topology_id + 1)

/*
 * }
 */

/*
 * DECLARATIONs
 * {
 */

/**
 * \brief
 *  Initialize BCM STG module.
 *  The actions include: create the default stg, initialize all ports in it with
 *  forward state and add the default vlan to the stg.
 *
 * \param [in] unit - Unit-ID 
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  The function is used on init time.
 *  It is also a API can be called in applications.
 *
 * \see
 *  None.
 */
shr_error_e dnx_stg_init(
    int unit);

/**
 * \brief
 *   Set the spanning tree state for a port.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Port - physical port or logical port
 * \param [in] state - State to place port in, one of BCM_PORT_STP_xxx.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   All STGs containing all VLANs containing the given port are updated.
 *
 * \see
 *   dnx_port_stp_get
 */
shr_error_e dnx_port_stp_set(
    int unit,
    bcm_port_t port,
    int state);

/**
 * \brief
 *   Get the spanning tree state for a port in the default STG.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - Port - physical port or logical port
 * \param [out] state - Pointer where state stored.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   dnx_port_stp_get
 */
shr_error_e dnx_port_stp_get(
    int unit,
    bcm_port_t port,
    int *state);

/**
 * \brief
 *   Removes a VSI from a spanning tree group.
 *   The VSI is placed back in the default spanning tree group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The specified Spanning Tree Group.
 * \param [in] vsi - The incoming VSI
 * \param [in] add_defl - Whether add the VSI to default STG or not.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   If this procedure is called in destroy operation, like destroy vlan,
 *   then should add_defl=FALSE. With this flag, the VSI won't be added to
 *   default STG.
 *
 * \see
 *   None.
 */
shr_error_e dnx_stg_vlan_remove(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi,
    int add_defl);

/**
 * \brief
 *   Check if the port is applicable to spanning tree protocol.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] port - port that will be set with spanning tree protocol state.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None
 *
 * \see
 *   None.
 */
shr_error_e dnx_stg_port_verify(
    int unit,
    bcm_port_t port);

/**
 * \brief - Add the given VSI to the specified stg.
 *
 * \param [in] unit - The unit ID
 * \param [in] stg  - The given stg_id
 * \param [in] vsi  - The given vsi
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   Add the VSI to the stg linked list first (sw_state), then update The VSI
 *   information with the stg (ING_VSI_INFO/EGR_VSI_INFO) in MDB.
 *
 * \see
 *   dnx_stg_vlan_first_set
 *   dnx_vlan_stg_set
 */
shr_error_e dnx_stg_vlan_map_add(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi);

/*
 * }
 */
#endif /* STG_H_INCLUDED */
