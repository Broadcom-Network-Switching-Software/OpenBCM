/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/stat.c
 * Purpose: BCMX Port Statistics APIs
 */

#include <bcm/types.h>

#include <bcmx/stat.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

#define BCMX_STAT_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_STAT_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)


/*
 * Function:
 *      bcmx_stat_init
 */

int
bcmx_stat_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STAT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stat_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_STAT_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_stat_sync(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STAT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stat_sync(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_STAT_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_stat_clear
 */

int
bcmx_stat_clear(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_clear(bcm_unit, bcm_port);
}

/*
 * Function:
 *      bcmx_stat_get
 */

int
bcmx_stat_get(bcmx_lport_t port,
              bcm_stat_val_t type,
              uint64 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_get(bcm_unit, bcm_port,
                        type, value);
}


/*
 * Function:
 *      bcmx_stat_get32
 */

int
bcmx_stat_get32(bcmx_lport_t port,
                bcm_stat_val_t type,
                uint32 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_get32(bcm_unit, bcm_port,
                          type, value);
}

/*
 * Function:
 *      bcmx_stat_multi_get
 */
int
bcmx_stat_multi_get(bcm_gport_t port, 
                    int nstat, bcm_stat_val_t *stat_arr, uint64 *value_arr)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_multi_get(bcm_unit, bcm_port,
                              nstat, stat_arr, value_arr);
}

/*
 * Function:
 *      bcmx_stat_multi_get
 */
int
bcmx_stat_multi_get32(bcm_gport_t port, 
                      int nstat, bcm_stat_val_t *stat_arr, uint32 *value_arr)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_multi_get32(bcm_unit, bcm_port,
                                nstat, stat_arr, value_arr);
}


/*
 * Function:
 *      bcmx_stat_custom_set
 */

int
bcmx_stat_custom_set(bcmx_lport_t port,
                     bcm_stat_val_t type,
                     uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_set(bcm_unit, bcm_port,
                               type, flags);
}


/*
 * Function:
 *      bcmx_stat_custom_get
 */

int
bcmx_stat_custom_get(bcmx_lport_t port,
                     bcm_stat_val_t type,
                     uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_get(bcm_unit, bcm_port,
                               type, flags);
}


/*
 * Function:
 *      bcmx_stat_custom_add
 */

int
bcmx_stat_custom_add(bcmx_lport_t port, bcm_stat_val_t type, 
                     bcm_custom_stat_trigger_t trigger)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_add(bcm_unit, bcm_port,
                               type, trigger);
}


/*
 * Function:
 *      bcmx_stat_custom_delete
 */

int
bcmx_stat_custom_delete(bcmx_lport_t port, bcm_stat_val_t type, 
                        bcm_custom_stat_trigger_t trigger)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_delete(bcm_unit, bcm_port,
                                  type, trigger);
}


/*
 * Function:
 *      bcmx_stat_custom_delete_all
 */

int
bcmx_stat_custom_delete_all(bcmx_lport_t port, bcm_stat_val_t type)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_delete_all(bcm_unit, bcm_port,
                                      type);
}


/*
 * Function:
 *      bcmx_stat_custom_check
 */

int
bcmx_stat_custom_check(bcmx_lport_t port, bcm_stat_val_t type, 
                       bcm_custom_stat_trigger_t trigger, int *result)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STAT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(result);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_check(bcm_unit, bcm_port,
                                 type, trigger, result);
}
