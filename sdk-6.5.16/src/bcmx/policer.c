/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/policer.c
 * Purpose:     BCMX Policer APIs
 */

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/policer.h>

#include "bcmx_int.h"

#define BCMX_POLICER_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_POLICER_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_POLICER_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_POLICER_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_POLICER_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_POLICER_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_policer_config_t *)(_config))


/*
 * Function:
 *     bcmx_policer_config_t_init
 */
void
bcmx_policer_config_t_init(bcmx_policer_config_t *pol_cfg)
{
    if (pol_cfg != NULL) {
        bcm_policer_config_t_init(BCMX_POLICER_CONFIG_T_PTR_TO_BCM(pol_cfg));
    }
}

/*
 * Function:
 *     bcmx_policer_init
 */
int
bcmx_policer_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_POLICER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_POLICER_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_create
 * Notes:
 *     If policer ID is not specified, the ID returned
 *     from the first successful 'create' is used for the remaining
 *     units.
 */
int
bcmx_policer_create(bcmx_policer_config_t *pol_cfg, 
                    bcm_policer_t *policer_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    bcmx_policer_config_t  tmp_pol_cfg;

    BCMX_POLICER_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pol_cfg);
    BCMX_PARAM_NULL_CHECK(policer_id);

    tmp_pol_cfg = *pol_cfg;
    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_policer_create(bcm_unit,
                                    BCMX_POLICER_CONFIG_T_PTR_TO_BCM
                                    (&tmp_pol_cfg),
                                    policer_id);
        BCM_IF_ERROR_RETURN
            (BCMX_POLICER_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if policer ID
         * is not specified.
         */
        if (!(tmp_pol_cfg.flags & BCM_POLICER_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                tmp_pol_cfg.flags |= BCM_POLICER_WITH_ID;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_destroy
 */
int
bcmx_policer_destroy(bcm_policer_t policer_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_POLICER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_destroy(bcm_unit, policer_id);
        BCM_IF_ERROR_RETURN
            (BCMX_POLICER_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_destroy_all
 */
int
bcmx_policer_destroy_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_POLICER_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_POLICER_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_set
 */
int
bcmx_policer_set(bcm_policer_t policer_id, 
                 bcmx_policer_config_t *pol_cfg)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_POLICER_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pol_cfg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_set(bcm_unit, policer_id,
                                 BCMX_POLICER_CONFIG_T_PTR_TO_BCM
                                 (pol_cfg));
        BCM_IF_ERROR_RETURN
            (BCMX_POLICER_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_get
 */
int
bcmx_policer_get(bcm_policer_t policer_id,
                 bcmx_policer_config_t *pol_cfg)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_POLICER_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pol_cfg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_policer_get(bcm_unit,  policer_id,
                             BCMX_POLICER_CONFIG_T_PTR_TO_BCM(pol_cfg));
        if (BCMX_POLICER_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

