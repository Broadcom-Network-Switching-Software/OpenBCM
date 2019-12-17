/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/stg.c
 * Purpose: stg.c BCMX source file
 */

#include <bcm/types.h>

#include <bcmx/stg.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

#define BCMX_STG_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_STG_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_STG_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_STG_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_STG_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)

#define BCMX_STG_ERROR_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


int
bcmx_stg_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_STG_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_stg_default_get
 */

int
bcmx_stg_default_get(bcm_stg_t *stg_ptr)
{
    int rv, i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stg_ptr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_default_get(bcm_unit, stg_ptr);
        if (BCMX_STG_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_stg_default_set
 */

int
bcmx_stg_default_set(bcm_stg_t stg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_default_set(bcm_unit, stg);
        BCM_IF_ERROR_RETURN(BCMX_STG_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_vlan_add
 */

int
bcmx_stg_vlan_add(bcm_stg_t stg,
                  bcm_vlan_t vid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_vlan_add(bcm_unit, stg, vid);
        BCM_IF_ERROR_RETURN(BCMX_STG_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_vlan_remove
 */

int
bcmx_stg_vlan_remove(bcm_stg_t stg,
                     bcm_vlan_t vid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_vlan_remove(bcm_unit, stg, vid);
        BCM_IF_ERROR_RETURN(BCMX_STG_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_vlan_remove_all
 */

int
bcmx_stg_vlan_remove_all(bcm_stg_t stg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_vlan_remove_all(bcm_unit, stg);
        BCM_IF_ERROR_RETURN(BCMX_STG_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_vlan_list
 */

int
bcmx_stg_vlan_list(bcm_stg_t stg,
                   bcm_vlan_t **list,
                   int *count)
{
    int rv, i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(list);
    BCMX_PARAM_NULL_CHECK(count);

    BCMX_LOCAL_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_vlan_list(bcm_unit, stg, list, count);
        if (BCMX_STG_ERROR_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_stg_vlan_list_destroy
 */

int
bcmx_stg_vlan_list_destroy(bcm_vlan_t *list,
                           int count)
{
    int rv, i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(count, list);

    BCMX_LOCAL_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_vlan_list_destroy(bcm_unit, list, count);
        if (BCMX_STG_ERROR_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_stg_create
 * Notes:
 *      Attempt to create the same STGID across all chips; the
 *      first time we succeed, we call "create_id".  Will
 *      return BCM_E_NONE if the stg id exists in some chips.
 */

int
bcmx_stg_create(bcm_stg_t *stg_ptr)
{
    int rv = BCM_E_UNAVAIL;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_create(bcm_unit, stg_ptr);
        if (BCMX_STG_ERROR_IS_VALID(bcm_unit, rv)) {
            if (BCM_SUCCESS(rv)) {  /* Found an STG */
                rv = bcmx_stg_create_id(*stg_ptr);
            }
            break;
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_create_id
 * Notes:
 *      Returns BCM_E_NONE if the STG already exists.
 */

int
bcmx_stg_create_id(bcm_stg_t stg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_create_id(bcm_unit, stg);
        BCM_IF_ERROR_RETURN(BCMX_STG_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_destroy
 */

int
bcmx_stg_destroy(bcm_stg_t stg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stg_destroy(bcm_unit, stg);
        BCM_IF_ERROR_RETURN(BCMX_STG_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_stg_list
 */

int
bcmx_stg_list(bcm_stg_t **list,
              int *count)
{
    int rv, i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(list);
    BCMX_PARAM_NULL_CHECK(count);

    BCMX_LOCAL_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_list(bcm_unit, list, count);
        if (BCMX_STG_ERROR_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_stg_list_destroy
 */

int
bcmx_stg_list_destroy(bcm_stg_t *list,
                      int count)
{
    int rv, i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(count, list);

    BCMX_LOCAL_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_list_destroy(bcm_unit, list, count);
        if (BCMX_STG_ERROR_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_stg_stp_set
 */

int
bcmx_stg_stp_set(bcm_stg_t stg,
                 bcmx_lport_t port,
                 int stp_state)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STG_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stg_stp_set(bcm_unit,
                           stg,
                           bcm_port,
                           stp_state);
}


/*
 * Function:
 *      bcmx_stg_stp_get
 */

int
bcmx_stg_stp_get(bcm_stg_t stg,
                 bcmx_lport_t port,
                 int *stp_state)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_STG_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stg_stp_get(bcm_unit,
                           stg,
                           bcm_port,
                           stp_state);
}


/*
 * Function:
 *      bcmx_stg_count_get
 */

int
bcmx_stg_count_get(int *max_stg)
{
    int rv, i, bcm_unit;

    BCMX_STG_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(max_stg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_stg_count_get(bcm_unit, max_stg);
        if (BCMX_STG_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
