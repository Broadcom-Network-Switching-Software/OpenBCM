/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * KNETSync Management
 */

#include <shared/bsl.h>

#include <bcm/knet.h>
#include <bcm/port.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm/error.h>
#include <bcm_int/common/knetsync.h>
#include <bcm_int/dpp/utils.h>

/*
 * Function:
 *      bcm_petra_knetsync_init
 * Purpose:
 *      Initialize the KNETSync subsystem and Firmware.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knetsync_init(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    bcm_port_t port, gport;
    bcm_port_config_t port_config;
    _bcm_knetsync_info_t knetsync_info;
    int user_header_size = 0, user_header_ndx = 0;
    uint32 value = 0;

    sal_memset(&knetsync_info, 0, sizeof(knetsync_info));

    knetsync_info.uc_port_num = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ptp_cosq_port", 0);

    rv = bcm_petra_stk_gport_sysport_get(unit, knetsync_info.uc_port_num, &gport);
    BCM_IF_ERROR_RETURN(rv);
    knetsync_info.uc_port_sysport = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

    rv = bcm_petra_port_config_get(unit, &port_config);
    BCM_PBMP_ITER(port_config.cpu, port) {
        knetsync_info.host_cpu_port = port;
        rv = bcm_petra_stk_gport_sysport_get(unit, port, &gport);
        BCM_IF_ERROR_RETURN(rv);

        knetsync_info.host_cpu_sysport = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        /* First port in the list is host cpu port. */
        break;
    }

    /* Send UDH Header information to firmware */
    /* NOF User-header-ndx:
     *    0-UH0 for PMF,
     *    1-UH1 for PMF,
     *    2-UH0 for Egress Editor,
     *    3-UH1 for Egress Editor
     */
    for (user_header_ndx = 0; user_header_ndx < 4; user_header_ndx++) {

        value = soc_property_port_get(unit, user_header_ndx, spn_FIELD_CLASS_ID_SIZE, 0);

        user_header_size += BITS2BYTES(value);

    }
    knetsync_info.udh_len = user_header_size;

    rv = bcm_common_knetsync_init(unit, knetsync_info);
    return BCM_SUCCESS(rv) ? BCM_E_NONE : rv; 
#endif
}

/*
 * Function:
 *      bcm_petra_knetsync_deinit
 * Purpose:
 *      Deinitialize the knetsync networking subsystem and firmware.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knetsync_deinit(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    rv = bcm_common_knetsync_deinit(unit);

    return BCM_SUCCESS(rv) ? BCM_E_NONE : rv; 
#endif
}
