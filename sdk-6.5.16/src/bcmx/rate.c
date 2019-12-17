/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/rate.c
 * Purpose: BCMX Packet Rate Control APIs
 */

#include <bcm/types.h>

#include <bcmx/rate.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

#define BCMX_RATE_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_RATE_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_RATE_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


/*
 * Function:
 *      bcmx_rate_set
 */

int
bcmx_rate_set(int val, int flags)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_RATE_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_rate_set(bcm_unit, val, flags);
        BCM_IF_ERROR_RETURN(BCMX_RATE_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_rate_get
 */

int
bcmx_rate_get(int *val, int *flags)
{
    int rv;
    int i, bcm_unit;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);
    BCMX_PARAM_NULL_CHECK(flags);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_rate_get(bcm_unit, val, flags);
        if (BCMX_RATE_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_rate_mcast_set
 */

int
bcmx_rate_mcast_set(int limit, int flags, bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_mcast_set(bcm_unit,
                              limit,
                              flags,
                              bcm_port);
}


/*
 * Function:
 *      bcmx_rate_bcast_set
 */

int
bcmx_rate_bcast_set(int limit, int flags, bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_bcast_set(bcm_unit,
                              limit,
                              flags,
                              bcm_port);
}


/*
 * Function:
 *      bcmx_rate_dlfbc_set
 */

int
bcmx_rate_dlfbc_set(int limit, int flags, bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_dlfbc_set(bcm_unit,
                              limit,
                              flags,
                              bcm_port);
}


/*
 * Function:
 *      bcmx_rate_mcast_get
 */

int
bcmx_rate_mcast_get(int *limit, int *flags, bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(limit);
    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_mcast_get(bcm_unit,
                              limit,
                              flags,
                              bcm_port);
}


/*
 * Function:
 *      bcmx_rate_dlfbc_get
 */

int
bcmx_rate_dlfbc_get(int *limit, int *flags, bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(limit);
    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_dlfbc_get(bcm_unit,
                              limit,
                              flags,
                              bcm_port);
}


/*
 * Function:
 *      bcmx_rate_bcast_get
 */

int
bcmx_rate_bcast_get(int *limit, int *flags, bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(limit);
    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_bcast_get(bcm_unit,
                              limit,
                              flags,
                              bcm_port);
}


/*
 * Function:
 *      bcmx_rate_type_get
 */

int
bcmx_rate_type_get(bcm_rate_limit_t *rl)
{
    int rv;
    int i, bcm_unit;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(rl);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_rate_type_get(bcm_unit, rl);
        if (BCMX_RATE_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_rate_type_set
 */

int
bcmx_rate_type_set(bcm_rate_limit_t *rl)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(rl);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_rate_type_set(bcm_unit, rl);
        BCM_IF_ERROR_RETURN(BCMX_RATE_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_rate_bandwidth_get
 */
int
bcmx_rate_bandwidth_get(bcmx_lport_t port, int flags,
                        uint32 *kbits_sec, uint32 *kbits_burst)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(kbits_sec);
    BCMX_PARAM_NULL_CHECK(kbits_burst);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_bandwidth_get(bcm_unit, bcm_port,
                                  flags,
                                  kbits_sec,
                                  kbits_burst);
}

/*
 * Function:
 *      bcmx_rate_bandwidth_set
 */
int
bcmx_rate_bandwidth_set(bcmx_lport_t port, int flags,
                        uint32 kbits_sec, uint32 kbits_burst)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_RATE_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_rate_bandwidth_set(bcm_unit, bcm_port,
                                  flags,
                                  kbits_sec,
                                  kbits_burst);
}
