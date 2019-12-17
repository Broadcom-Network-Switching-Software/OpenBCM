/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/qos.c
 * Purpose:     BCMX Quality of Service APIs
 */

#include <sal/core/libc.h>

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/qos.h>

#include "bcmx_int.h"

#define BCMX_QOS_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_QOS_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_QOS_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_QOS_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_QOS_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_QOS_MAP_T_PTR_TO_BCM(_qos_map)  ((bcm_qos_map_t *)(_qos_map))


/*
 * Function:
 *     bcmx_qos_map_t_init
 */
void
bcmx_qos_map_t_init(bcmx_qos_map_t *qos_map)
{
    if (qos_map != NULL) {
        bcm_qos_map_t_init(BCMX_QOS_MAP_T_PTR_TO_BCM(qos_map));
    }
}

/*
 * Function:
 *     bcmx_qos_init
 */
int
bcmx_qos_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_QOS_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_detach
 */
int
bcmx_qos_detach(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_detach(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_QOS_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_map_create
 */
int
bcmx_qos_map_create(uint32 flags, int *map_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(map_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_map_create(bcm_unit, flags, map_id);

        BCM_IF_ERROR_RETURN(BCMX_QOS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if group ID
         * is not specified.
         */
        if (!(flags & BCM_QOS_MAP_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                flags |= BCM_QOS_MAP_WITH_ID;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_map_destroy
 */
int
bcmx_qos_map_destroy(int map_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_map_destroy(bcm_unit, map_id);
        BCM_IF_ERROR_RETURN(BCMX_QOS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_map_add
 */
int
bcmx_qos_map_add(uint32 flags, bcmx_qos_map_t *map, int map_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(map);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_map_add(bcm_unit, flags,
                                 BCMX_QOS_MAP_T_PTR_TO_BCM(map), map_id);
        BCM_IF_ERROR_RETURN(BCMX_QOS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_map_delete
 */
int
bcmx_qos_map_delete(uint32 flags, bcmx_qos_map_t *map, int map_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(map);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_map_delete(bcm_unit, flags,
                                    BCMX_QOS_MAP_T_PTR_TO_BCM(map), map_id);
        BCM_IF_ERROR_RETURN(BCMX_QOS_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_map_multi_get
 * Notes:
 *     Assumes homogeneuous systems and all devices in stack
 *     has the same QoS mapping.
 */
int
bcmx_qos_map_multi_get(uint32 flags, int map_id, 
                       int array_size, bcmx_qos_map_t *array,
                       int *array_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(array_count);

    /*
     * No need to gather entries from all units.  BCMX assumes
     * that QoS mapping were created with BCMX APIs.
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_qos_map_multi_get(bcm_unit, flags, map_id,
                                   array_size,
                                   BCMX_QOS_MAP_T_PTR_TO_BCM(array),
                                   array_count);
        if (BCMX_QOS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_qos_port_map_set
 */
int
bcmx_qos_port_map_set(bcm_gport_t port, int ing_map, int egr_map)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_qos_port_map_set(bcm_unit, port, ing_map, egr_map);
        BCM_IF_ERROR_RETURN(BCMX_QOS_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_qos_port_map_get
 */
int
bcmx_qos_port_map_get(bcm_gport_t port, int *ing_map, int *egr_map)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ing_map);
    BCMX_PARAM_NULL_CHECK(egr_map);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_qos_port_map_get(bcm_unit, port, ing_map, egr_map);
        if (BCMX_QOS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_qos_port_vlan_map_set
 * Notes:
 *     Port is a physical port.
 */
int bcmx_qos_port_vlan_map_set(bcm_gport_t port, bcm_vlan_t vid,
                               int ing_map, int egr_map)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_QOS_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_qos_port_vlan_map_set(bcm_unit, bcm_port, vid,
                                     ing_map, egr_map);
}

/*
 * Function:
 *     bcmx_qos_port_vlan_map_get
 * Notes:
 *     Port is a physical port.
 */
int
bcmx_qos_port_vlan_map_get(bcm_gport_t port, bcm_vlan_t vid, 
                           int *ing_map, int *egr_map)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ing_map);
    BCMX_PARAM_NULL_CHECK(egr_map);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_qos_port_vlan_map_get(bcm_unit, bcm_port, vid,
                                     ing_map, egr_map);
}

/*
 * Function:
 *     bcmx_qos_multi_get
 * Notes:
 *     Assumes homogeneuous systems and all devices in stack
 *     has the same QoS mapping.
 */
int
bcmx_qos_multi_get(int array_size, int *map_ids_array, 
                   int *flags_array, int *array_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_QOS_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(array_count);

    /*
     * No need to gather entries from all units.  BCMX assumes
     * that QoS mapping were created with BCMX APIs.
     */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_qos_multi_get(bcm_unit, array_size, map_ids_array,
                               flags_array, array_count);
        if (BCMX_QOS_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
