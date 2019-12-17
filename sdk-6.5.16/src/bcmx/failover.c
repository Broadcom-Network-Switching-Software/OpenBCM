/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/failover.c
 * Purpose:     BCMX Forwarding Failover Protection APIs
 */
#include <bcm/types.h>

#include <bcmx/failover.h>

#ifdef INCLUDE_L3

#include "bcmx_int.h"

#define BCMX_FAILOVER_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_FAILOVER_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_FAILOVER_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_FAILOVER_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_FAILOVER_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


/*
 * Function:
 *     bcmx_failover_init
 */
int
bcmx_failover_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FAILOVER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_failover_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_FAILOVER_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_failover_cleanup
 */
int
bcmx_failover_cleanup(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FAILOVER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_failover_cleanup(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_FAILOVER_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_failover_create
 */
int
bcmx_failover_create(uint32 flags, bcm_failover_t *failover_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FAILOVER_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(failover_id);

    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_failover_create(bcm_unit, flags, failover_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FAILOVER_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if group ID
         * is not specified.
         */
        if (!(flags & BCM_FAILOVER_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                flags |= BCM_FAILOVER_WITH_ID;
            }
        }
    }

    return rv;
}


/*
 * Function:
 *     bcmx_failover_destroy
 */
int
bcmx_failover_destroy(bcm_failover_t failover_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FAILOVER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_failover_destroy(bcm_unit, failover_id);
        BCM_IF_ERROR_RETURN
            (BCMX_FAILOVER_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_failover_set
 */
int
bcmx_failover_set(bcm_failover_t failover_id, int enable)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_FAILOVER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_failover_set(bcm_unit, failover_id, enable);
        BCM_IF_ERROR_RETURN
            (BCMX_FAILOVER_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     bcmx_failover_get
 */
int
bcmx_failover_get(bcm_failover_t failover_id, int *enable)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_FAILOVER_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_failover_get(bcm_unit, failover_id, enable);
        if (BCMX_FAILOVER_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

#else /* INCLUDE_L3 */
/*
 * Function:
 *     bcmx_failover_init
 */
int
bcmx_failover_init(void)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_failover_cleanup
 */
int
bcmx_failover_cleanup(void)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_failover_create
 */
int
bcmx_failover_create(uint32 flags, bcm_failover_t *failover_id)
{
    COMPILER_REFERENCE(flags);
    COMPILER_REFERENCE(failover_id);
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_failover_destroy
 */
int
bcmx_failover_destroy(bcm_failover_t failover_id)
{
    COMPILER_REFERENCE(failover_id);
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_failover_set
 */
int
bcmx_failover_set(bcm_failover_t failover_id, int enable)
{
    COMPILER_REFERENCE(failover_id);
    COMPILER_REFERENCE(enable);
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_failover_get
 */
int
bcmx_failover_get(bcm_failover_t failover_id, int *enable)
{
    COMPILER_REFERENCE(failover_id);
    COMPILER_REFERENCE(enable);
    return BCM_E_UNAVAIL;
}
#endif /* INCLUDE_L3 */
