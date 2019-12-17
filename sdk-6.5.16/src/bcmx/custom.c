/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Custom API callouts
 */

#include <bcm/types.h>

#include <bcmx/custom.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

#define BCMX_CUSTOM_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_CUSTOM_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_CUSTOM_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)

/*
 * Notes:
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_custom_port_set(bcmx_lport_t port, int type, int len, uint32 *args)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_CUSTOM_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        /*
         * Convert to old bcm port format to ensure application
         * custom routines continue to work.
         */
        if (BCM_FAILURE(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_NON_GPORT))) {
            return BCM_E_PORT;
        }
        return bcm_custom_port_set(bcm_unit, bcm_port, type, len, args);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_custom_port_set(bcm_unit, port, type, len, args);
        BCM_IF_ERROR_RETURN
            (BCMX_CUSTOM_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_custom_port_get(bcmx_lport_t port, int type, int max_len,
                     uint32 *args, int *actual_len)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_CUSTOM_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        /*
         * Convert to old bcm port format to ensure application
         * custom routines continue to work.
         */
        if (BCM_FAILURE(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_NON_GPORT))) {
            return BCM_E_PORT;
        }
        return bcm_custom_port_get(bcm_unit, bcm_port, type,
                                   max_len, args, actual_len);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_custom_port_get(bcm_unit, port, type, max_len, args, actual_len);
        if (BCMX_CUSTOM_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_custom_unit_set(int type, int len, uint32 *args)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_CUSTOM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_custom_port_set(bcm_unit, -1, type, len, args);
        BCM_IF_ERROR_RETURN
            (BCMX_CUSTOM_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_custom_unit_get(int type, int max_len, uint32 *args, int *actual_len)
{
    int rv = BCM_E_UNAVAIL;
    int i, bcm_unit;

    BCMX_CUSTOM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_custom_port_get(bcm_unit, -1, type, max_len, args, actual_len);
        if (BCMX_CUSTOM_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
