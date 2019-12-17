/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:	bcmx/auth.c
 * Purpose:	BCMX 802.1x support
 */

#include <bcm/types.h>

#include <bcmx/auth.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

#define BCMX_AUTH_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_AUTH_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

int
bcmx_auth_mode_set(bcmx_lport_t port, uint32 mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_mode_set(bcm_unit, bcm_port, mode);
}

int
bcmx_auth_mode_get(bcmx_lport_t port, uint32 *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_mode_get(bcm_unit, bcm_port, mode);
}

/* Currently, only one auth_unauth callback is supported
 * at the BCMX layer.
 */

STATIC bcmx_auth_cb_t ap_auth_cb_fn = NULL;
STATIC void *ap_auth_cookie = NULL;

STATIC void
_bcmx_auth_cb(void *cookie, int unit, int port, int reason)
{
    bcmx_lport_t lport;

    if (BCM_FAILURE(_bcmx_dest_from_unit_port(&lport,
                                              unit, (bcm_port_t)port,
                                              BCMX_DEST_CONVERT_DEFAULT))) {
        return;
    }

    if (ap_auth_cb_fn != NULL) {
        (*ap_auth_cb_fn)(ap_auth_cookie, lport, reason);
    }
}


/*
 * This routine registers through rlink to get remote device's
 * calls.
 */

int
bcmx_auth_unauth_callback(bcmx_auth_cb_t func, void *cookie)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_auth_cb_t cb_f = NULL;

    BCMX_AUTH_INIT_CHECK;

    ap_auth_cb_fn = func;
    ap_auth_cookie = cookie;
    if (func != NULL) {
        /* If ap function is non-NULL, register with BCMX's callback */
        cb_f = _bcmx_auth_cb;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_auth_unauth_callback(bcm_unit, cb_f, cookie);
        BCM_IF_ERROR_RETURN(BCMX_AUTH_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_auth_egress_set(bcmx_lport_t port, int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_egress_set(bcm_unit, bcm_port, enable);
}

int
bcmx_auth_egress_get(bcmx_lport_t port, int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_egress_get(bcm_unit, bcm_port, enable);
}

int
bcmx_auth_mac_add(bcmx_lport_t port, bcm_mac_t mac)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_mac_add(bcm_unit, bcm_port, mac);
}

int
bcmx_auth_mac_delete(bcmx_lport_t port, bcm_mac_t mac)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_mac_delete(bcm_unit, bcm_port, mac);
}

int
bcmx_auth_mac_delete_all(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_AUTH_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_auth_mac_delete_all(bcm_unit, bcm_port);
}
