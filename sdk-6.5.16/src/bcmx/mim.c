/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/mim.c
 * Purpose:     BCMX MAC-in-MAC APIs
 */
#ifdef INCLUDE_L3

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/mim.h>

#include "bcmx_int.h"

#define BCMX_MIM_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_MIM_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MIM_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MIM_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MIM_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_MIM_VPN_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_mim_vpn_config_t *)(_config))

#define BCMX_MIM_PORT_T_PTR_TO_BCM(_port)    \
    ((bcm_mim_port_t *)(_port))


/*
 * Function:
 *     bcmx_mim_vpn_config_t_init
 */
void
bcmx_mim_vpn_config_t_init(bcmx_mim_vpn_config_t *mim_vpn_config)
{
    if (mim_vpn_config != NULL) {
        bcm_mim_vpn_config_t_init(BCMX_MIM_VPN_CONFIG_T_PTR_TO_BCM
                                  (mim_vpn_config));
    }
}

/*
 * Function:
 *     bcmx_mim_port_t_init
 */
void
bcmx_mim_port_t_init(bcmx_mim_port_t *mim_port)
{
    if (mim_port != NULL) {
        bcm_mim_port_t_init(BCMX_MIM_PORT_T_PTR_TO_BCM(mim_port));
    }
}


/*
 * Function:
 *     bcmx_mim_init
 * Purpose:
 *     Initialize the MIM software module.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MIM_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mim_detach
 * Purpose:
 *     Detach the MIM software module.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_detach(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_detach(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MIM_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mim_vpn_create
 * Purpose:
 *     Create a VPN instance.
 * Parameters:
 *     info - (IN/OUT) VPN configuration info
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_vpn_create(bcmx_mim_vpn_config_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_MIM_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = info->flags & BCM_MIM_VPN_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_vpn_create(bcm_unit,
                                    BCMX_MIM_VPN_CONFIG_T_PTR_TO_BCM(info));
        if (BCM_FAILURE(BCMX_MIM_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if ID
         * is not specified.
         */
        if (!(info->flags & BCM_MIM_VPN_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                info->flags |= BCM_MIM_VPN_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    info->flags &= ~BCM_MIM_VPN_WITH_ID;
    info->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_mim_vpn_destroy
 * Purpose:
 *     Delete a VPN instance.
 * Parameters:
 *     vpn - VPN instance ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_vpn_destroy(bcm_mim_vpn_t vpn)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_vpn_destroy(bcm_unit, vpn);
        BCM_IF_ERROR_RETURN(BCMX_MIM_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mim_vpn_destroy_all
 * Purpose:
 *     Delete all VPN instances.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_vpn_destroy_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_vpn_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MIM_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mim_vpn_get
 * Purpose:
 *     Get configuration for a given VPN instance id.
 * Parameters:
 *     vpn  - VPN instance ID
 *     info - (OUT) VPN configuration info
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_vpn_get(bcm_mim_vpn_t vpn, bcmx_mim_vpn_config_t *info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mim_vpn_get(bcm_unit, vpn,
                             BCMX_MIM_VPN_CONFIG_T_PTR_TO_BCM(info));
        if (BCMX_MIM_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mim_port_add
 * Purpose:
 *     Add a MAC-in-MAC port to a VPN.
 * Parameters:
 *     vpn      - VPN instance ID
 *     mim_port - (IN/OUT) MAC-in-MAC port information
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_port_add(bcm_mim_vpn_t vpn, bcmx_mim_port_t *mim_port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_MIM_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mim_port);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = mim_port->flags & BCM_MIM_PORT_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_port_add(bcm_unit, vpn,
                                  BCMX_MIM_PORT_T_PTR_TO_BCM(mim_port));
        if (BCM_FAILURE(BCMX_MIM_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'add' if ID
         * is not specified.
         */
        if (!(mim_port->flags & BCM_MIM_PORT_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                mim_port->flags |= BCM_MIM_PORT_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    mim_port->flags &= ~BCM_MIM_PORT_WITH_ID;
    mim_port->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_mim_port_delete
 * Purpose:
 *     Delete a MAC-in-MAC port from a VPN.
 * Parameters:
 *     vpn         - VPN instance ID
 *     mim_port_id - MAC-in-MAC port ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_port_delete(bcm_mim_vpn_t vpn, bcm_gport_t mim_port_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_port_delete(bcm_unit, vpn, mim_port_id);
        BCM_IF_ERROR_RETURN(BCMX_MIM_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mim_port_delete_all
 * Purpose:
 *     Delete all MAC-in-MAC ports from a VPN.
 * Parameters:
 *     vpn - VPN instance ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_port_delete_all(bcm_mim_vpn_t vpn)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mim_port_delete_all(bcm_unit, vpn);
        BCM_IF_ERROR_RETURN(BCMX_MIM_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mim_port_get
 * Purpose:
 *     Get MAC-in-MAC port information for a VPN.
 * Parameters:
 *     vpn       - VPN instance ID
 *     mim_port  - (IN/OUT) mim port information
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_mim_port_get(bcm_mim_vpn_t vpn, bcmx_mim_port_t *mim_port)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mim_port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mim_port_get(bcm_unit, vpn,
                              BCMX_MIM_PORT_T_PTR_TO_BCM(mim_port));
        if (BCMX_MIM_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
     
/*
 * Function:
 *     bcmx_mim_port_get_all
 * Purpose:
 *     Get all MAC-in-MAC ports information for a VPN.
 * Parameters:
 *     vpn        - VPN instance ID
 *     port_max   - Maximum number of interfaces in array
 *     port_array - (OUT) Array of mim ports
 *     port_count - (OUT) Number of interfaces returned in array
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     MAC-in-MAC ports are added to all devices since they are global
 *     entities.  API '_get_all()' can just return port information
 *     from call on first switch.
 */
int
bcmx_mim_port_get_all(bcm_mim_vpn_t vpn, int port_max, 
                      bcmx_mim_port_t *port_array, int *port_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MIM_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(port_array);
    BCMX_PARAM_NULL_CHECK(port_count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mim_port_get_all(bcm_unit, vpn, port_max,
                                  BCMX_MIM_PORT_T_PTR_TO_BCM(port_array),
                                  port_count);
        if (BCMX_MIM_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

#endif  /* INCLUDE_L3 */
