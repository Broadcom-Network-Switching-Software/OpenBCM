/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/trunk.c
 * Purpose: BCMX Trunk APIs
 *
 * BCMX trunking takes place on switch ports, not fabric ports.
 * Fabric port trunking may be controlled by the stack/toppology
 * code, so we are careful to skip any fabric devices and ports.
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <bcm/types.h>

#include <bcmx/trunk.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include <bcm_int/control.h>
#include "bcmx_int.h"

#define BCMX_TRUNK_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_TRUNK_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TRUNK_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TRUNK_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TRUNK_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_TRUNK_MEMBER_T_PTR_TO_BCM(_trunk_member)   \
    ((bcm_trunk_member_t *)(_trunk_member))


/*
 * Function:
 *      bcmx_trunk_add_info_t_init
 * Purpose:
 *      Initialize the bcmx_trunk_add_info_t structure.
 * Parameters:
 *      trunk_add_info - Pointer to structure to be initialized.
 * Returns:
 *      None
 * Notes:
 *      Memory is allocated for the port list member in this
 *      call.  The corresponding 'free' API needs to be called
 *      when structure is no longer needed.
 */
void
bcmx_trunk_add_info_t_init(bcmx_trunk_add_info_t *trunk_add_info)
{
    if (trunk_add_info != NULL) {
        sal_memset(trunk_add_info, 0, sizeof(bcmx_trunk_add_info_t));
        bcmx_lplist_t_init(&trunk_add_info->ports);
    }
}


/*
 * Function:
 *      bcmx_trunk_add_info_t_free
 * Purpose:
 *      Free memory allocated by bcmx_trunk_add_info_t_init().
 * Parameters:
 *      trunk_add_info - Pointer to structure to free memory.
 * Returns:
 *      None
 */
void
bcmx_trunk_add_info_t_free(bcmx_trunk_add_info_t *trunk_add_info)
{
    if (trunk_add_info != NULL) {
        bcmx_lplist_free(&trunk_add_info->ports);
    }
}

/*
 * Function:
 *      bcmx_trunk_member_t_init
 * Purpose:
 *      Trunk member structure initializer.
 * Parameters:
 *      trunk_member - (IN/OUT) Pointer to structure to be initialized.
 * Returns:
 *      None
 */
void
bcmx_trunk_member_t_init(bcmx_trunk_member_t *trunk_member)
{
    if (trunk_member != NULL) {
        bcm_trunk_member_t_init(BCMX_TRUNK_MEMBER_T_PTR_TO_BCM(trunk_member));
    }
}


/*
 * Function:
 *      bcmx_trunk_init
 */

int
bcmx_trunk_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_detach
 */

int
bcmx_trunk_detach(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_detach(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_create
 */

int
bcmx_trunk_create(bcm_trunk_t *tid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_create(bcm_unit, 0, tid);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_create_id
 */

int
bcmx_trunk_create_id(bcm_trunk_t tid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_create(bcm_unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_psc_set
 */

int
bcmx_trunk_psc_set(bcm_trunk_t tid, int psc)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_psc_set(bcm_unit, tid, psc);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_psc_get
 */

int
bcmx_trunk_psc_get(bcm_trunk_t tid, int *psc)
{
    int rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(psc);

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        rv = bcm_trunk_psc_get(bcm_unit, tid, psc);
        if (BCMX_TRUNK_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_trunk_chip_info_get
 * Notes:
 *      Returns the common denominator from all switch devices.
 */

int
bcmx_trunk_chip_info_get(bcm_trunk_chip_info_t *ta_info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_trunk_chip_info_t tmp_info;
    int first = TRUE;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ta_info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }

        tmp_rv = bcm_trunk_chip_info_get(bcm_unit, &tmp_info);
        if (BCMX_TRUNK_GET_IS_VALID(bcm_unit, rv)) {
            rv = tmp_rv;

            BCM_IF_ERROR_RETURN(rv);

            if (first) {
                *ta_info = tmp_info;
                first = FALSE;
                continue;
            }

            if (tmp_info.trunk_group_count < ta_info->trunk_group_count) {
                ta_info->trunk_group_count = tmp_info.trunk_group_count;
            }
            if (tmp_info.trunk_id_min > ta_info->trunk_id_min) {
                ta_info->trunk_id_min = tmp_info.trunk_id_min;
            }
            if (tmp_info.trunk_id_max < ta_info->trunk_id_max) {
                ta_info->trunk_id_max = tmp_info.trunk_id_max;
            }
            if (tmp_info.trunk_ports_max < ta_info->trunk_ports_max) {
                ta_info->trunk_ports_max = tmp_info.trunk_ports_max;
            }
            if (tmp_info.trunk_fabric_id_min > ta_info->trunk_fabric_id_min) {
                ta_info->trunk_fabric_id_min = tmp_info.trunk_fabric_id_min;
            }
            if (tmp_info.trunk_fabric_id_max < ta_info->trunk_fabric_id_max) {
                ta_info->trunk_fabric_id_max = tmp_info.trunk_fabric_id_max;
            }
            if (tmp_info.trunk_fabric_ports_max < ta_info->trunk_fabric_ports_max) {
                ta_info->trunk_fabric_ports_max = tmp_info.trunk_fabric_ports_max;
            }
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_set
 */

int
bcmx_trunk_set(bcm_trunk_t tid, bcmx_trunk_add_info_t *add_info)
{
    int                   rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_trunk_info_t      badd;
    int                   i, bcm_unit, count;
    bcmx_lport_t          lport;
    int                   member_count;
    bcm_trunk_member_t    *member_array = NULL;
    bcm_module_t          mod;
    bcm_port_t            port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(add_info);

    sal_memset(&badd, 0, sizeof(bcm_trunk_info_t));
    badd.psc = add_info->psc;
    badd.ipmc_psc = add_info->ipmc_psc;
    badd.dlf_index = -1;
    badd.mc_index = -1;
    badd.ipmc_index = -1;

    member_count = BCMX_LPLIST_COUNT(&(add_info->ports));
    member_array = sal_alloc(member_count * sizeof(bcm_trunk_member_t),
            "trunk member array");
    if (NULL == member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(member_array, 0, member_count * sizeof(bcm_trunk_member_t));

    BCMX_LPLIST_ITER(add_info->ports, lport, count) {
        if (count >= BCM_TRUNK_MAX_PORTCNT) {
            sal_free(member_array);
            return BCM_E_PARAM;
        }
        if (lport == add_info->dlf_port) {
            badd.dlf_index = count;
        }
        if (lport == add_info->mc_port) {
            badd.mc_index = count;
        }
        if (lport == add_info->ipmc_port) {
            badd.ipmc_index = count;
        }

        tmp_rv = _bcmx_dest_to_modid_port(lport,
                                      &mod, &port,
                                      BCMX_DEST_CONVERT_DEFAULT);
        if (BCM_FAILURE(tmp_rv)) {
            sal_free(member_array);
            return tmp_rv;
        }

        bcm_trunk_member_t_init(&member_array[count]);
        BCM_GPORT_MODPORT_SET(member_array[count].gport, mod, port);
    }
    
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_set(bcm_unit, tid, &badd, member_count,
                               member_array);
        tmp_rv = BCMX_TRUNK_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv);
        if (BCM_FAILURE(tmp_rv)) {
            sal_free(member_array);
            return tmp_rv;
        }
    }

    sal_free(member_array);
    return rv;
}


/*
 * Function:
 *      bcmx_trunk_destroy
 */

int
bcmx_trunk_destroy(bcm_trunk_t tid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_destroy(bcm_unit, tid);
        BCM_IF_ERROR_RETURN
            (BCMX_TRUNK_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_get
 */

int
bcmx_trunk_get(bcm_trunk_t tid, bcmx_trunk_add_info_t *t_data)
{
    bcm_trunk_info_t   badd;
    int                rv, i, bcm_unit;
    bcmx_lport_t       lport;
    int                member_count, unused_count;
    bcm_trunk_member_t *member_array = NULL;
    bcm_module_t       mod;
    bcm_port_t         port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(t_data);

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }

        rv = bcm_trunk_get(bcm_unit, tid, &badd, 0, NULL, &member_count);
        if (!BCMX_TRUNK_GET_IS_VALID(bcm_unit, rv)) {
            continue;
        }

        if (member_count > 0) {
            member_array = sal_alloc(member_count * sizeof(bcm_trunk_member_t),
                                     "trunk member array"); 
            if (NULL == member_array) {
                return BCM_E_MEMORY;
            }
        }
        rv = bcm_trunk_get(bcm_unit, tid, &badd, member_count, member_array,
                           &unused_count);
        if (BCMX_TRUNK_GET_IS_VALID(bcm_unit, rv)) {
            if (BCM_SUCCESS(rv)) {
                sal_memset(t_data, 0, sizeof(bcmx_trunk_add_info_t));
                bcmx_lplist_init(&t_data->ports, member_count, 0);
                t_data->psc = badd.psc;
                t_data->ipmc_psc = badd.ipmc_psc;
                t_data->dlf_port = BCMX_LPORT_INVALID;
                t_data->mc_port = BCMX_LPORT_INVALID;
                t_data->ipmc_port = BCMX_LPORT_INVALID;

                for (i = 0; i < member_count; i++) {
                    /* Convert to lport */
                    mod = BCM_GPORT_MODPORT_MODID_GET(member_array[i].gport);
                    port = BCM_GPORT_MODPORT_PORT_GET(member_array[i].gport);
                    rv = _bcmx_dest_from_modid_port(&lport,
                                                    mod, port,
                                                    BCMX_DEST_CONVERT_DEFAULT);
                    if (BCM_FAILURE(rv)) {
                        sal_free(member_array);
                        return rv;
                    }
                    bcmx_lplist_add(&t_data->ports, lport);
                    if (i == badd.dlf_index) {
                        t_data->dlf_port = lport;
                    }
                    if (i == badd.mc_index) {
                        t_data->mc_port = lport;
                    }
                    if (i == badd.ipmc_index) {
                        t_data->ipmc_port = lport;
                    }
                }
            }

            if (NULL != member_array) {
                sal_free(member_array);
            }
            return rv;
        }

        if (NULL != member_array) {
            sal_free(member_array);
            member_array = NULL;
        }
    }

    return BCM_E_UNAVAIL;
}



/*
 * Function:
 *      bcmx_trunk_mcast_join
 */

int
bcmx_trunk_mcast_join(bcm_trunk_t tid,
                      bcm_vlan_t vid,
                      bcm_mac_t mac)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        tmp_rv = bcm_trunk_mcast_join(bcm_unit, tid, vid, mac);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_egress_set
 */

int
bcmx_trunk_egress_set(bcm_trunk_t tid, bcmx_lplist_t lplist)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t  pbm;
    int         bcm_unit, i;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }
        BCM_PBMP_CLEAR(pbm);
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbm);
        tmp_rv = bcm_trunk_egress_set(bcm_unit, tid, pbm);
        BCM_IF_ERROR_RETURN(BCMX_TRUNK_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_trunk_egress_get
 */

int
bcmx_trunk_egress_get(bcm_trunk_t tid, bcmx_lplist_t *lplist)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t  pbm;
    int         bcm_unit, i;

    BCMX_TRUNK_INIT_CHECK;

    bcmx_lplist_clear(lplist);
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }

        tmp_rv = bcm_trunk_egress_get(bcm_unit, tid, &pbm);
        if (BCMX_TRUNK_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbm);
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_trunk_find
 */
int
bcmx_trunk_find(bcmx_lport_t lport, bcm_trunk_t *tid)
{
    int rv = BCM_E_UNAVAIL, bcm_unit, i;
    bcm_module_t modid;
    bcm_port_t   modport;

    BCMX_TRUNK_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(lport, &modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_trunk_find(bcm_unit,
                            modid,
                            modport,
                            tid);

        if (BCM_SUCCESS(rv)) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trunk_failover_set
 * Purpose:
 *     Assign the failover port list for a specific trunk port.
 * Parameters:
 *     tid           - Trunk id
 *     failport      - Port in trunk for which to specify failover port list
 *     psc           - Port selection criteria for failover port list
 *     flags         - BCM_TRUNK_FLAG_FAILOVER_xxx
 *     count         - Number of ports in failover port list
 *     fail_to_array - Failover port list
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_trunk_failover_set(bcm_trunk_t tid, bcm_gport_t failport, 
                        int psc, uint32 flags, int count, 
                        bcm_gport_t *fail_to_array)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(count, fail_to_array);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(failport, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trunk_failover_set(bcm_unit, tid, failport, 
                                  psc, flags, count, fail_to_array);
}

/*
 * Function:
 *     bcmx_trunk_failover_get
 * Purpose:
 *     Retrieve the failover port list for a specific trunk port.
 * Parameters:
 *     tid           - Trunk id
 *     failport      - Port in trunk for which to retrieve failover port list
 *     psc           - (OUT) Port selection criteria for failover port list
 *     flags         - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx
 *     array_size    - Maximum number of ports in provided failover port list
 *     fail_to_array - (OUT) Failover port list
 *     array_count   - (OUT) Number of ports in returned failover port list
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_trunk_failover_get(bcm_trunk_t tid, bcm_gport_t failport, 
                        int *psc, uint32 *flags, int array_size, 
                        bcm_gport_t *fail_to_array, int *array_count)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(psc);
    BCMX_PARAM_NULL_CHECK(flags);
    BCMX_PARAM_ARRAY_NULL_CHECK(array_size, fail_to_array);
    BCMX_PARAM_NULL_CHECK(array_count);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(failport, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trunk_failover_get(bcm_unit, tid, failport, 
                                  psc, flags, array_size,
                                  fail_to_array, array_count);
}

/*
 * Function:
 *     bcmx_trunk_member_add
 * Purpose:
 *     Add a member to a trunk group.
 * Parameters:
 *     tid    - Trunk id
 *     member - Pointer to a trunk member structure.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_trunk_member_add(bcm_trunk_t tid, bcmx_trunk_member_t *member)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(member);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(member->gport,
                                                &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trunk_member_add(bcm_unit, tid,
                                BCMX_TRUNK_MEMBER_T_PTR_TO_BCM(member));
}

/*
 * Function:
 *     bcmx_trunk_member_delete
 * Purpose:
 *     Remove a member from a trunk group.
 * Parameters:
 *     tid    - Trunk id
 *     member - Pointer to a trunk member structure.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_trunk_member_delete(bcm_trunk_t tid, bcmx_trunk_member_t *member)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(member);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(member->gport,
                                                &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trunk_member_delete(bcm_unit, tid,
                                   BCMX_TRUNK_MEMBER_T_PTR_TO_BCM(member));
}

/*
 * Function:
 *     bcmx_trunk_member_delete_all
 * Purpose:
 *     Remove all members of a trunk group.
 * Parameters:
 *     tid - Trunk id
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trunk_member_delete_all(bcm_trunk_t tid)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trunk_member_delete_all(bcm_unit, tid);
        BCM_IF_ERROR_RETURN
            (BCMX_TRUNK_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trunk_member_set
 * Purpose:
 *     Assign a set of members to a trunk group.
 * Parameters:
 *     tid          - Trunk id
 *     member_count - Number of trunk members in member_array.
 *     member_array - Array of trunk member structures.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Assumes all ports are in the same device (pick first one).
 *     BCM will report an error if a port is not local to designated device.
 */
int
bcmx_trunk_member_set(bcm_trunk_t tid, int member_count, 
                      bcmx_trunk_member_t *member_array)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(member_array);

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(member_array[0].gport,
                                                &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));
    /* bcm_trunk_member_set is deprecated.
     *
     * return bcm_trunk_member_set(bcm_unit, tid, member_count,
     *                           BCMX_TRUNK_MEMBER_T_PTR_TO_BCM(member_array));
     */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_trunk_member_get
 * Purpose:
 *     Retrieve members of a trunk group. If member_max is 0,
 *     the number of member ports in the given trunk group
 *     is returned in member_count.
 * Parameters:
 *     tid          - Trunk id
 *     member_max   - Size of member_array
 *     member_array - (OUT) Array of trunk member structures.
 *     member_count - (OUT) Number of trunk members returned.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trunk_member_get(bcm_trunk_t tid, int member_max, 
                      bcmx_trunk_member_t *member_array, int *member_count)
{
    BCMX_TRUNK_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(member_max, member_array);
    BCMX_PARAM_NULL_CHECK(member_count);

    /* bcm_trunk_member_get is deprecated.
     *
     * BCMX_UNIT_ITER(bcm_unit, i) {
     *     rv = bcm_trunk_member_get(bcm_unit, tid, member_max,
     *                             BCMX_TRUNK_MEMBER_T_PTR_TO_BCM(member_array),
     *                             member_count);
     *     if (BCMX_TRUNK_GET_IS_VALID(bcm_unit, rv)) {
     *         return rv;
     *     }
     * }
     */

    return BCM_E_UNAVAIL;
}

