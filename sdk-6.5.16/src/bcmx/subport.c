/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/subport.c
 * Purpose:     BCMX Subport Configuration APIs
 */

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/subport.h>

#include "bcmx_int.h"

#define BCMX_SUBPORT_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_SUBPORT_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_SUBPORT_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_SUBPORT_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_SUBPORT_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)

#define BCMX_SUBPORT_ERROR_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_SUBPORT_GROUP_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_subport_group_config_t *)(_config))

#define BCMX_SUBPORT_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_subport_config_t *)(_config))


/*
 * Function:
 *     bcmx_subport_group_config_t_init
 */
void
bcmx_subport_group_config_t_init(bcmx_subport_group_config_t *config)
{
    if (config != NULL) {
        bcm_subport_group_config_t_init
            (BCMX_SUBPORT_GROUP_CONFIG_T_PTR_TO_BCM(config));
    }
}

/*
 * Function:
 *     bcmx_subport_config_t_init
 */
void
bcmx_subport_config_t_init(bcmx_subport_config_t *config)
{
    if (config != NULL) {
        bcm_subport_config_t_init
            (BCMX_SUBPORT_CONFIG_T_PTR_TO_BCM(config));
    }
}


/*
 * Function:
 *     bcmx_subport_init
 */
int
bcmx_subport_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_subport_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_SUBPORT_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_subport_cleanup
 */
int
bcmx_subport_cleanup(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_subport_cleanup(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_SUBPORT_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_subport_group_create
 */
int
bcmx_subport_group_create(bcmx_subport_group_config_t *config, 
                          bcm_gport_t *group)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    bcmx_subport_group_config_t  tmp_config;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);
    BCMX_PARAM_NULL_CHECK(group);

    tmp_config = *config;
    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_subport_group_create(bcm_unit,
                                          BCMX_SUBPORT_GROUP_CONFIG_T_PTR_TO_BCM
                                          (&tmp_config),
                                          group);
        BCM_IF_ERROR_RETURN
            (BCMX_SUBPORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if group ID
         * is not specified.
         */
        if (!(tmp_config.flags & BCM_SUBPORT_GROUP_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                tmp_config.flags |= BCM_SUBPORT_GROUP_WITH_ID;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_subport_group_destroy
 */
int
bcmx_subport_group_destroy(bcm_gport_t group)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_subport_group_destroy(bcm_unit, group);
        BCM_IF_ERROR_RETURN
            (BCMX_SUBPORT_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_subport_group_get
 */
int
bcmx_subport_group_get(bcm_gport_t group, bcmx_subport_group_config_t *config)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_subport_group_get(bcm_unit, group,
                                   BCMX_SUBPORT_GROUP_CONFIG_T_PTR_TO_BCM
                                   (config));
        if (BCMX_SUBPORT_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_subport_port_add
 */
int
bcmx_subport_port_add(bcmx_subport_config_t *config, bcm_gport_t *port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);
    BCMX_PARAM_NULL_CHECK(port);

    /*
     * A subport port is added only in the unit where the physical
     * port resides, and this information is known at the BCM layer.
     *
     * The agreed behavior between BCM and BCMX is that
     * the BCM will return BCM_E_PARAM whenever the 'add' function
     * is called on a unit where the physical port does not belong.
     *
     * The BCMX, on the other hand, will iterate over the units
     * and returns on the first successful 'add' of the subport port.
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_subport_port_add(bcm_unit,
                                      BCMX_SUBPORT_CONFIG_T_PTR_TO_BCM
                                      (config),
                                      port);
        if (!BCMX_SUBPORT_ERROR_IS_VALID(bcm_unit, tmp_rv)) {  /* Ignore */
            continue;
        }

        rv = tmp_rv;
        if (tmp_rv == BCM_E_PARAM) {  /* Port is not in unit, try next */
            continue;
        }

        break;
    }

    return rv;
}

/*
 * Function:
 *     bcmx_subport_port_delete
 */
int
bcmx_subport_port_delete(bcm_gport_t port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_subport_port_delete(bcm_unit, port);
        BCM_IF_ERROR_RETURN
            (BCMX_SUBPORT_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_subport_port_get
 */
int bcmx_subport_port_get(bcm_gport_t port, bcmx_subport_config_t *config)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_SUBPORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_subport_port_get(bcm_unit, port,
                                  BCMX_SUBPORT_CONFIG_T_PTR_TO_BCM(config));
        if (BCMX_SUBPORT_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}



