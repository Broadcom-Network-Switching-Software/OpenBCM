/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/wlan.c
 * Purpose:     BCMX Wireless LAN APIs
 */

#ifdef INCLUDE_L3

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/wlan.h>
#include "bcmx_int.h"

#define BCMX_WLAN_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_WLAN_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_WLAN_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_WLAN_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_WLAN_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_WLAN_PORT_T_PTR_TO_BCM(_port)    \
    ((bcm_wlan_port_t *)(_port))

#define BCMX_WLAN_CLIENT_T_PTR_TO_BCM(_client)    \
    ((bcm_wlan_client_t *)(_client))

#define BCMX_TUNNEL_INITIATOR_T_PTR_TO_BCM(_tunnel)    \
    ((bcm_tunnel_initiator_t *)(_tunnel))


/*
 * Function:
 *     bcmx_wlan_port_t_init
 */
void
bcmx_wlan_port_t_init(bcmx_wlan_port_t *wlan_port)
{
    if (wlan_port != NULL) {
        bcm_wlan_port_t_init(BCMX_WLAN_PORT_T_PTR_TO_BCM(wlan_port));
    }
}

/*
 * Function:
 *     bcmx_wlan_client_t_init
 */
void
bcmx_wlan_client_t_init(bcmx_wlan_client_t *wlan_client)
{
    if (wlan_client != NULL) {
        bcm_wlan_client_t_init(BCMX_WLAN_CLIENT_T_PTR_TO_BCM(wlan_client));
    }
}


/*
 * Function:
 *     bcmx_wlan_init
 * Purpose:
 *     Initialize the Wireless LAN module.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_wlan_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_WLAN_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_detach
 * Purpose:
 *     Detach the Wireless LAN software module.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_wlan_detach(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_detach(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_WLAN_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_port_add
 * Purpose:
 *     Create or replace a WLAN port.
 * Parameters:
 *     info - (IN/OUT) WLAN port configuration info
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     WLAN port is a global entity, apply to all units.
 */
int
bcmx_wlan_port_add(bcmx_wlan_port_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_WLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = info->flags & BCM_WLAN_PORT_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_port_add(bcm_unit,
                                   BCMX_WLAN_PORT_T_PTR_TO_BCM(info));
        if (BCM_FAILURE(BCMX_WLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if WLAN port ID
         * is not specified.
         */
        if (!(info->flags & BCM_WLAN_PORT_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                info->flags |= BCM_WLAN_PORT_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    info->flags &= ~BCM_WLAN_PORT_WITH_ID;
    info->flags |= flags_orig;


    return rv;
}

/*
 * Function:
 *     bcmx_wlan_port_delete
 * Purpose:
 *     Delete a WLAN port.
 * Parameters:
 *     wlan_port_id - WLAN port id to delete
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_port_delete(bcm_gport_t wlan_port_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_port_delete(bcm_unit, wlan_port_id);
        BCM_IF_ERROR_RETURN
            (BCMX_WLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_port_delete_all
 * Purpose:
 *     Delete all WLAN ports.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_port_delete_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_port_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_WLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}
    
/*
 * Function:
 *     bcmx_wlan_port_get
 * Purpose:
 *     Get WLAN port information for given ID.
 * Parameters:
 *     wlan_port_id - WLAN port id
 *     info         - (IN/OUT) WLAN port configuration info
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_port_get(bcm_gport_t wlan_port_id, bcmx_wlan_port_t *info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_wlan_port_get(bcm_unit, wlan_port_id,
                               BCMX_WLAN_PORT_T_PTR_TO_BCM(info));
        if (BCMX_WLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_wlan_client_add
 * Purpose:
 *     Add a wireless client to the database.
 * Parameters:
 *     info - (IN/OUT) Wireless client structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     WLAN client is a global entity, apply to all units.
 */
int
bcmx_wlan_client_add(bcmx_wlan_client_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_client_add(bcm_unit,
                                     BCMX_WLAN_CLIENT_T_PTR_TO_BCM(info));
        BCM_IF_ERROR_RETURN(BCMX_WLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_client_delete
 * Purpose:
 *     Delete a wireless client from the database.
 * Parameters:
 *     mac - MAC address of client (lookup key)
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_client_delete(bcm_mac_t mac)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_client_delete(bcm_unit, mac);
        BCM_IF_ERROR_RETURN
            (BCMX_WLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_client_delete_all
 * Purpose:
 *     Delete all wireless clients from the database.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_client_delete_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_client_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_WLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_client_get
 * Purpose:
 *     Get a wireless client information for given MAC.
 * Parameters:
 *     mac  - MAC address of client (lookup key)
 *     info - (IN/OUT) Wireless client info
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_client_get(bcm_mac_t mac, bcmx_wlan_client_t *info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_wlan_client_get(bcm_unit, mac,
                                 BCMX_WLAN_CLIENT_T_PTR_TO_BCM(info));
        if (BCMX_WLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_wlan_tunnel_initiator_create
 * Purpose:
 *     Create an outgoing CAPWAP tunnel.
 * Parameters:
 *     info - (IN/OUT) Tunnel initiator structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_tunnel_initiator_create(bcmx_tunnel_initiator_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_WLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_tunnel_initiator_create
            (bcm_unit,
             BCMX_TUNNEL_INITIATOR_T_PTR_TO_BCM(info));
        if (BCM_FAILURE(BCMX_WLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if ID
         * is not specified.
         */
        if (!(info->flags & BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                info->flags |= BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    info->flags &= ~BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID;
    info->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_tunnel_initiator_destroy
 * Purpose:
 *     Destroy an outgoing CAPWAP tunnel.
 * Parameters:
 *     wlan_tunnel_id - Tunnel ID (gport)
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_tunnel_initiator_destroy(bcm_gport_t wlan_tunnel_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_wlan_tunnel_initiator_destroy(bcm_unit, wlan_tunnel_id);
        BCM_IF_ERROR_RETURN
            (BCMX_WLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_wlan_tunnel_initiator_get
 * Purpose:
 *     Get an outgoing CAPWAP tunnel.
 * Parameters:
 *     info - (IN/OUT) Tunnel initiator structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_wlan_tunnel_initiator_get(bcmx_tunnel_initiator_t *info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_WLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_wlan_tunnel_initiator_get(bcm_unit,
                                           BCMX_TUNNEL_INITIATOR_T_PTR_TO_BCM
                                           (info));
        if (BCMX_WLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

#endif /* INCLUDE_L3 */
