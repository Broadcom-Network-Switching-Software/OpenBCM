/** \file appl_ref_compensation_init.c
 * 
 *
 * L@ application procedures for DNX. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
* Include files.
* {
*/
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <bcm/cosq.h>
#include "appl_ref_sys_device.h"
#include "appl_ref_compensation_init.h"

/*
 * }
 */

/** See .h file */
shr_error_e
appl_dnx_compensation_port_delta_set(
    int unit,
    bcm_port_t port,
    int delta)
{
    bcm_cosq_pkt_size_adjust_info_t pkt_size_adjust_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * setting default ingress compensation delta (for crps and stif) of 4 bytes for all ports, 
     * in order to compensate for 4 bytes packet CRC which is removed in the device. 
     */
    sal_memset(&pkt_size_adjust_info, 0x0, sizeof(bcm_cosq_pkt_size_adjust_info_t));
    BCM_GPORT_LOCAL_SET(pkt_size_adjust_info.gport, port);
    pkt_size_adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_set(unit, &pkt_size_adjust_info, delta));
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
appl_dnx_compensation_init(
    int unit)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_port_config_t port_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /** Create a bitmap of nif ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_OR(pbmp, port_config.nif);

    /** go over all ports */
    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(appl_dnx_compensation_port_delta_set(unit, port, APPL_DNX_REF_COMPENSATION_ING_PORT_DELTA));
    }

exit:
    SHR_FUNC_EXIT;
}
