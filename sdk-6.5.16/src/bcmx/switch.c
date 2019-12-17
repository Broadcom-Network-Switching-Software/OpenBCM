/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * BCMX Switch Control
 */

#include <bcm/types.h>

#include <bcmx/switch.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

#define BCMX_SWITCH_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_SWITCH_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_SWITCH_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


int
bcmx_switch_control_set(bcm_switch_control_t type, int arg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_SWITCH_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_switch_control_set(bcm_unit, type, arg);
        BCM_IF_ERROR_RETURN(BCMX_SWITCH_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_switch_control_get(bcm_switch_control_t type, int *arg)
{
    int rv;
    int i, bcm_unit;

    BCMX_SWITCH_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_switch_control_get(bcm_unit, type, arg);
        if (BCMX_SWITCH_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_switch_control_port_set(bcmx_lport_t port, bcm_switch_control_t type,
                             int arg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_SWITCH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_switch_control_port_set(bcm_unit, bcm_port,
                                       type, arg);
}



int
bcmx_switch_control_port_get(bcmx_lport_t port, bcm_switch_control_t type,
                             int *arg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_SWITCH_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_switch_control_port_get(bcm_unit, bcm_port,
                                       type, arg);
}


int
bcmx_switch_rcpu_encap_priority_map_set(uint32 flags, 
                                        int internal_cpu_pri, int encap_pri)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_SWITCH_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_switch_rcpu_encap_priority_map_set(bcm_unit, flags,
                                                        internal_cpu_pri,
                                                        encap_pri);
        BCM_IF_ERROR_RETURN(BCMX_SWITCH_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_switch_rcpu_encap_priority_map_get(uint32 flags,
                                        int internal_cpu_pri, int *encap_pri)
{
    int rv;
    int i, bcm_unit;

    BCMX_SWITCH_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_pri);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_switch_rcpu_encap_priority_map_get(bcm_unit, flags,
                                                    internal_cpu_pri,
                                                    encap_pri);
        if (BCMX_SWITCH_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
