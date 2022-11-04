/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This file contains dnxInstrumentation module definitions internal to the BCM library.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#ifndef _INSTRU_H_INCLUDED__
#define _INSTRU_H_INCLUDED__

#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_instru.h>

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

typedef struct
{
    uint8 pms[DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PMS_FOR_PFC]; /** PMs to be counted */
    bcm_port_t nif_ports_in_pms[DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC]; /** NIF ports ID internal to PMs to be counted */
    uint8 tcs_per_port[DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC][DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_TCS_PER_PORT]; /** Two TCs to count for each port */
}
bcm_instru_synced_counters_port_for_pfc_hw_t;

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
