/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains dnxInstrumentation module definitions internal to the BCM library.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef _INSTRU_H_INCLUDED__
#define _INSTRU_H_INCLUDED__

#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <soc/dnx/dbal/dbal_structures.h>

/*
 * structure to hold all information related to trajectory trace EEDB entry  
 */
typedef struct
{
    uint32 flags;               /* Configuration flags. */
    bcm_gport_t trajectory_trace_id;    /* Tunnel id */
    int observation_domain;     /* IPFIX obseravtion domain. */
    int template_id;            /* PSAMP template-ID */
    bcm_if_t egress_tunnel_if;  /* next_ptr */
} bcm_instru_trajectory_trace_t;

/**
 * \brief
 * * Write to IPFIX/PSAMP encapsulation out-LIF table.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *     \param [in] trajectory_trace -
 *     A pointer to the struct that holds information for the trajectory trace entry,
 *     see \ref bcm_instru_trajectory_trace_t. 
 * \par DIRECT OUTPUT:
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error, see \ref shr_error_e.
 */
shr_error_e dnx_instru_tragectory_trace_create(
    int unit,
    bcm_instru_trajectory_trace_t * trajectory_trace);

/**
 * \brief -
 * Init procedure for intru module
 *
 * \param [in] unit -  Unit-ID
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_init(
    int unit);

/**
 * \brief -
 * De-Init procedure for intru module
 *
 * \param [in] unit -  Unit-ID
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_deinit(
    int unit);

#endif /* _INSTRU_H_INCLUDED__ */
