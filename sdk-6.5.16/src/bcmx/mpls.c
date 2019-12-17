/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 * 
 * File:     mpls.c
 * Purpose:  Manages MPLS functionality at a system level
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/l3.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"


#include <bcmx/mpls.h>

#ifdef INCLUDE_L3

#define BCMX_MPLS_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_MPLS_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MPLS_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MPLS_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MPLS_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_MPLS_VPN_CONFIG_T_PTR_TO_BCM(_info)    \
    ((bcm_mpls_vpn_config_t *)(_info))

#define BCMX_MPLS_PORT_T_PTR_TO_BCM(_mpls_port)    \
    ((bcm_mpls_port_t *)(_mpls_port))

#define BCMX_MPLS_EGRESS_LABEL_T_PTR_TO_BCM(_label)    \
    ((bcm_mpls_egress_label_t *)(_label))

#define BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM(_info)    \
    ((bcm_mpls_tunnel_switch_t *)(_info))


/*
 * Function:
 *     bcmx_mpls_vpn_config_t_init
 * Purpose:
 *     Initialize the MPLS VPN config structure.
 * Parameters:
 *     info - Pointer to the struct to be initialized
 * Returns:
 *     None
 */
void
bcmx_mpls_vpn_config_t_init(bcmx_mpls_vpn_config_t *info)
{
    if (info != NULL) {
        bcm_mpls_vpn_config_t_init(BCMX_MPLS_VPN_CONFIG_T_PTR_TO_BCM(info));
    }
}

/*
 * Function:
 *     bcmx_mpls_port_t_init
 * Purpose:
 *     Initialize the MPLS port structure
 * Parameters:
 *     mpls_port - Pointer to the struct to be initialized
 * Returns:
 *     None
 */
void
bcmx_mpls_port_t_init(bcmx_mpls_port_t *mpls_port)
{
    if (mpls_port != NULL) {
        bcm_mpls_port_t_init(BCMX_MPLS_PORT_T_PTR_TO_BCM(mpls_port));
    }
}

/*
 * Function:
 *     bcmx_mpls_egress_label_t_init
 * Purpose:
 *     Initialize the MPLS egress label structure
 * Parameters:
 *     label - Pointer to the struct to be initialized
 * Returns:
 *     None
 */
void
bcmx_mpls_egress_label_t_init(bcmx_mpls_egress_label_t *label)
{
    if (label != NULL) {
        bcm_mpls_egress_label_t_init(BCMX_MPLS_EGRESS_LABEL_T_PTR_TO_BCM
                                     (label));
    }
}

/*
 * Function:
 *     bcmx_mpls_tunnel_switch_t_init
 * Purpose:
 *     Initialize the MPLS tunnel switch structure
 * Parameters:
 *     info - Pointer to the struct to be initialized
 * Returns:
 *     None
 */
void
bcmx_mpls_tunnel_switch_t_init(bcmx_mpls_tunnel_switch_t *info)
{
    if (info != NULL) {
        bcm_mpls_tunnel_switch_t_init(BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                      (info));
    }
}

/*
 * Function:
 *      bcmx_mpls_init
 * Purpose:
 *      Initialize MPLS module system-wide
 * Parameters:
 *      None
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_mpls_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MPLS_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_cleanup
 * Purpose:
 *     Disable MPLS system-wide
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_mpls_cleanup(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_cleanup(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MPLS_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_mpls_exp_map_create
 * Purpose:
 *      Create an MPLS EXP map instance.
 * Parameters:
 *      flags       - (IN)  MPLS flags
 *      exp_map_id  - (OUT) Allocated EXP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_mpls_exp_map_create(uint32 flags, int *exp_map_id)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(exp_map_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_exp_map_create(bcm_unit, flags, exp_map_id);
        BCM_IF_ERROR_RETURN(BCMX_MPLS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_mpls_exp_map_destroy
 * Purpose:
 *      Destroy an existing MPLS EXP map instance.
 * Parameters:
 *      exp_map_id - (IN) EXP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_mpls_exp_map_destroy(int exp_map_id)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_exp_map_destroy(bcm_unit, exp_map_id);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mpls_exp_map_set
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a EXP value for MPLS headers
 *      in the specified EXP map instance.
 * Parameters:
 *      exp_map_id   - (IN) EXP map ID
 *      exp_map      - (IN) EXP map info
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_mpls_exp_map_set(int exp_map_id,
                      bcm_mpls_exp_map_t *exp_map)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(exp_map);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_exp_map_set(bcm_unit, exp_map_id, exp_map);
        BCM_IF_ERROR_RETURN(BCMX_MPLS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mpls_exp_map_get
 * Purpose:
 *      Get the EXP map information for the specified EXP map instance.
 * Parameters:
 *      exp_map_id   - (IN)  EXP map ID
 *      exp_map      - (OUT) Returning EXP map info
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_mpls_exp_map_get(int exp_map_id,
                      bcm_mpls_exp_map_t *exp_map)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mpls_exp_map_get(bcm_unit, exp_map_id, exp_map);
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mpls_vpn_id_create
 */
int
bcmx_mpls_vpn_id_create(bcmx_mpls_vpn_config_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;
    uint32  flags_orig;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = info->flags & BCM_MPLS_VPN_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_vpn_id_create(bcm_unit,
                                        BCMX_MPLS_VPN_CONFIG_T_PTR_TO_BCM
                                        (info));
        if (BCM_FAILURE(BCMX_MPLS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }
        
        /*
         * If vpn id is not specified, use returned id from
         * first successful 'create' for remaining units.
         */
        if (!(info->flags & BCM_MPLS_VPN_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                info->flags |= BCM_MPLS_VPN_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    info->flags &= ~BCM_MPLS_VPN_WITH_ID;
    info->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_vpn_id_destroy
 */
int
bcmx_mpls_vpn_id_destroy(bcm_vpn_t vpn)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_vpn_id_destroy(bcm_unit, vpn);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_vpn_id_destroy_all
 */
int
bcmx_mpls_vpn_id_destroy_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_vpn_id_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_vpn_id_get
 */
int
bcmx_mpls_vpn_id_get(bcm_vpn_t vpn, bcmx_mpls_vpn_config_t *info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mpls_vpn_id_get(bcm_unit,  vpn,
                                 BCMX_MPLS_VPN_CONFIG_T_PTR_TO_BCM(info));
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mpls_port_add
 */
int
bcmx_mpls_port_add(bcm_vpn_t vpn, bcmx_mpls_port_t *mpls_port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;
    uint32  flags_orig;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mpls_port);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = mpls_port->flags & BCM_MPLS_PORT_WITH_ID;

    /*
     * Use returned 'mpls_port_id' from first success 'add'
     * to set rest of the units.
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_port_add(bcm_unit, vpn,
                                   BCMX_MPLS_PORT_T_PTR_TO_BCM(mpls_port));
        if (BCM_FAILURE(BCMX_MPLS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * If mpls id is not specified, use returned id from
         * first successful 'add' for remaining units.
         */
        if (!(mpls_port->flags & BCM_MPLS_PORT_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                mpls_port->flags |= BCM_MPLS_PORT_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    mpls_port->flags &= ~BCM_MPLS_PORT_WITH_ID;
    mpls_port->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_port_delete
 */
int
bcmx_mpls_port_delete(bcm_vpn_t vpn, bcm_gport_t mpls_port_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_port_delete(bcm_unit, vpn, mpls_port_id);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_port_delete_all
 */
int
bcmx_mpls_port_delete_all(bcm_vpn_t vpn)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_port_delete_all(bcm_unit, vpn);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_port_get
 */
int
bcmx_mpls_port_get(bcm_vpn_t vpn, bcmx_mpls_port_t *mpls_port)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mpls_port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mpls_port_get(bcm_unit,  vpn,
                               BCMX_MPLS_PORT_T_PTR_TO_BCM(mpls_port));
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mpls_port_get_all
 */
int
bcmx_mpls_port_get_all(bcm_vpn_t vpn, int port_max,
                       bcmx_mpls_port_t *port_array, int *port_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(port_max, port_array);
    BCMX_PARAM_NULL_CHECK(port_count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mpls_port_get_all(bcm_unit,  vpn, port_max,
                                   BCMX_MPLS_PORT_T_PTR_TO_BCM(port_array),
                                   port_count);
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_initiator_set
 */
int
bcmx_mpls_tunnel_initiator_set(bcm_if_t intf, int num_labels,
                               bcmx_mpls_egress_label_t *label_array)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(label_array);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_tunnel_initiator_set(bcm_unit, intf, num_labels,
                                               BCMX_MPLS_EGRESS_LABEL_T_PTR_TO_BCM
                                               (label_array));
        BCM_IF_ERROR_RETURN(BCMX_MPLS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_initiator_get
 */
int
bcmx_mpls_tunnel_initiator_get(bcm_if_t intf, int label_max,
                               bcmx_mpls_egress_label_t *label_array,
                               int *label_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(label_max, label_array);
    BCMX_PARAM_NULL_CHECK(label_count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mpls_tunnel_initiator_get(bcm_unit, intf, label_max,
                                           BCMX_MPLS_EGRESS_LABEL_T_PTR_TO_BCM
                                           (label_array),
                                           label_count);
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_initiator_clear
 */
int
bcmx_mpls_tunnel_initiator_clear(bcm_if_t intf)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_tunnel_initiator_clear(bcm_unit, intf);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_initiator_clear_all
 */
int
bcmx_mpls_tunnel_initiator_clear_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_tunnel_initiator_clear_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_switch_add
 */
int
bcmx_mpls_tunnel_switch_add(bcmx_mpls_tunnel_switch_t *info)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /*
     * If 'port' member is a valid BCMX logical port, then port
     * refers to a specific physical port (mod/port or dev/port).
     * Else, 'port' is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(info->port,
                                            &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_mpls_tunnel_switch_add(bcm_unit,
                                          BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                          (info));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_tunnel_switch_add(bcm_unit,
                                            BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                            (info));
        BCM_IF_ERROR_RETURN(BCMX_MPLS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_switch_delete
 */
int
bcmx_mpls_tunnel_switch_delete(bcmx_mpls_tunnel_switch_t *info)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /*
     * If 'port' member is a valid BCMX logical port, then port
     * refers to a specific physical port (mod/port or dev/port).
     * Else, 'port' is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(info->port,
                                            &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_mpls_tunnel_switch_delete(bcm_unit,
                                             BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                             (info));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_tunnel_switch_delete(bcm_unit,
                                               BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                               (info));
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_switch_delete_all
 */
int
bcmx_mpls_tunnel_switch_delete_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i;
    int  bcm_unit;

    BCMX_MPLS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_tunnel_switch_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_tunnel_switch_get
 */
int
bcmx_mpls_tunnel_switch_get(bcmx_mpls_tunnel_switch_t *info)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /*
     * If 'port' member is a valid BCMX logical port, then port
     * refers to a specific physical port (mod/port or dev/port).
     * Else, 'port' is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(info->port,
                                            &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_mpls_tunnel_switch_get(bcm_unit,
                                          BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                          (info));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mpls_tunnel_switch_get(bcm_unit,
                                        BCMX_MPLS_TUNNEL_SWITCH_T_PTR_TO_BCM
                                        (info));
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_mpls_label_stat_get
 */
int
bcmx_mpls_label_stat_get(bcm_mpls_label_t label, bcm_gport_t port,
                         bcm_mpls_stat_t stat, uint64 *val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint64      tmp_val;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value, gather stats from all units.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_mpls_label_stat_get(bcm_unit, label, port, stat, val);
    }

    COMPILER_64_ZERO(*val);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_label_stat_get(bcm_unit, label, port, stat,
                                         &tmp_val);
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                COMPILER_64_ADD_64(*val, tmp_val);
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_label_stat_get32
 */
int
bcmx_mpls_label_stat_get32(bcm_mpls_label_t label, bcm_gport_t port,
                           bcm_mpls_stat_t stat, uint32 *val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint32      tmp_val;

    BCMX_MPLS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value, gather stats from all units.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_mpls_label_stat_get32(bcm_unit, label, port, stat, val);
    }

    *val = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_label_stat_get32(bcm_unit, label, port, stat,
                                           &tmp_val);
        if (BCMX_MPLS_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                *val += tmp_val;
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_mpls_label_stat_clear
 */
int
bcmx_mpls_label_stat_clear(bcm_mpls_label_t label, bcm_gport_t port,
                           bcm_mpls_stat_t stat)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MPLS_INIT_CHECK;

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_mpls_label_stat_clear(bcm_unit, label, port, stat);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mpls_label_stat_clear(bcm_unit, label, port, stat);
        BCM_IF_ERROR_RETURN
            (BCMX_MPLS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

#endif  /* INCLUDE_L3 */


