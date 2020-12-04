/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        connection.c
 * Purpose:     Board connection functions
 */


#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>
#include <board/board.h>
#include <board/manager.h>
#include <board_int/support.h>


/*
 * Function:
 *     board_connection_gport_stackable
 * Purpose:
 *     Return true if the port is capable of being some sort of Higig
 *     port.
 * Parameters:
 *     gport - (IN) stack port
 * Returns:
 *     TRUE - port is stackable
 *     BCM_E_NONE - port is not stackable
 *     BCM_E_XXX - failure
 * Notes:
 *  Not all Higig modes are usable for stacking purposes.
 *
 *  This API is currently broken. See PR 36640 -
 *  bcm_port_ability_local_get only gets current encap of port
 */
int
board_connection_gport_stackable(bcm_gport_t gport)
{
    bcm_port_ability_t ability;
    int unit;
    int rv = BCM_E_NONE;

    unit = BCM_GPORT_DEVPORT_DEVID_GET(gport);

    rv = bcm_port_ability_local_get(unit, gport, &ability);
    if (BCM_SUCCESS(rv)) {
        
        if ((ability.encap & BCM_PA_ENCAP_HIGIG) ||
            (ability.encap & BCM_PA_ENCAP_HIGIG2)) {
                rv = TRUE;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_connection_gport_stacking
 * Purpose:
 *     Return TRUE if the stack port is currently in a stacking mode
 * Parameters:
 *     gport - (IN) stack port
 * Returns:
 *     TRUE - port is in a stacking mode
 *     BCM_E_NONE - port is not in a stacking mode
 *     BCM_E_XXX - failure
 */
int
board_connection_gport_stacking(bcm_gport_t gport)
{
    int mode;
    int unit;
    int rv = BCM_E_NONE;

    unit = BCM_GPORT_DEVPORT_DEVID_GET(gport);

    rv = bcm_port_encap_get(unit, gport, &mode);
    if (BCM_SUCCESS(rv)) {
        if ((mode == BCM_PORT_ENCAP_HIGIG) ||
            (mode == BCM_PORT_ENCAP_HIGIG2) ||
            (mode == BCM_PORT_ENCAP_HIGIG2_LITE)) {
                rv = TRUE;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_connections_discover
 * Purpose:
 *     Discovers internal connectivity between devices on a board.
 *     Discovery algorithm is guaranteed to disrupt traffic,
 *     and may be slow for large numbers of devices or internal ports.
 *
 *     This function is provided for debugging and initial board
 *     implementation, and is not intended for production use.
 * Parameters:
 *     max_num    - (IN)  maximum number of entries in connection array
 *     connection - (OUT) array of board_connection_t
 *     actual     - (OUT) actual number of valid entries in connection array
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_connections_discover(int max_num,
                           board_connection_t *connection,
                           int *actual)
{
    int u;
    int unit = -1;
    int num_units;
    int count = 0;
    int rv = BCM_E_NONE;
    bcm_port_config_t port_info;
    bcm_port_t port;
    bcm_port_t gport;

    for ( num_units=u=0; u<bcm_unit_max(); u++ ) {
        if (bcm_unit_local(u)) {
            if (unit < 0) {
                unit = u;
                num_units++;
            } else {
                
                return BCM_E_CONFIG;
            }
        }
    }

    if (unit >= 0) {
        BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_info));
        BCM_PBMP_ITER(port_info.port, port) {
            gport = BCM_GPORT_DEVPORT(unit, port);
            rv = board_connection_gport_stackable(gport);
            if (BCM_SUCCESS(rv) && rv > 0) {
                if (count < max_num) {
                    connection[count].from = BCM_GPORT_DEVPORT(unit,port);
                    connection[count].to   = BCM_GPORT_TYPE_NONE;
                }
                count++;
            } else if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    if (max_num == 0) {
        if (actual) {
            *actual = count;
        } else {
            rv = BCM_E_PARAM;
        }
    } else {
        if (actual) {
            *actual = count;
        }
    }

    return rv;
}

/*
 * Function:
 *     board_connections_alloc
 * Purpose:
 *     Allocate a worst case sized connection list for the current board.
 * Parameters:
 *     driver     - (IN)  current board driver
 *     connection - (OUT) allocated connection array
 *     actual     - (OUT) size of connection array
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_connections_alloc(board_driver_t *driver,
                        board_connection_t **connection,
                        int *actual)
{
    int count;
    int rv = BCM_E_MEMORY;
    board_connection_t *conn;

    if (driver == NULL || connection == NULL || actual == NULL) {
        return BCM_E_PARAM;
    }

    count = board_num_devices() * BCM_PBMP_PORT_MAX;
    conn = sal_alloc(count * sizeof(*conn), driver->name);
    if (conn) {
        sal_memset(conn, 0, count * sizeof(*conn));
        *connection = conn;
        *actual = count;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     board_connection_stack_ports
 * Purpose:
 *     Returns all stack ports on all devices, without regard to internal
 *     connectivity.
 * Parameters:
 *     max_num    - (IN)    maximum size of connection list
 *     connection - (INOUT) connection list
 *     actual     - (OUT)   actual size of connection list
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_connection_stack_ports(int max_num,
                             board_connection_t *connection,
                             int *actual)
{
    int unit;
    int count;
    int rv = BCM_E_NONE;
    bcm_port_config_t port_info;
    bcm_port_t port;
    bcm_port_t gport;

    count = 0;
    for ( unit=0; unit<bcm_unit_max(); unit++ ) {
        if (bcm_unit_local(unit)) {
            BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_info));
            BCM_PBMP_ITER(port_info.port, port) {
                gport = BCM_GPORT_DEVPORT(unit, port);
                rv = board_connection_gport_stackable(gport);
                if (BCM_SUCCESS(rv) && rv > 0) {
                    if (count < max_num) {
                        connection[count].from = BCM_GPORT_DEVPORT(unit,port);
                        connection[count].to   = BCM_GPORT_TYPE_NONE;
                    }
                    count++;
                } else if (BCM_FAILURE(rv)) {
                    return rv;
                }
            }
        }
    }

    if (actual) {
        *actual = count;
    }

    return rv;
}

#define INT_STK_FLAGS (BCM_STK_ENABLE | BCM_STK_DUPLEX | BCM_STK_INTERNAL)

/* Traverse  */

/*
 * Function:
 *     board_connection_connected_stack_port_traverse
 * Purpose:
 *     Tranverses all connected stack ports in the list, ignoring
 *     external ports, and non-stack ports.
 * Parameters:
 *      count      - (IN) length of connection list
 *      connection - (IN) connection list
 *      func       - (IN) callback function
 *      user_data  - (IN) user data passed to callback
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_connection_connected_stack_port_traverse(int count,
                                               board_connection_t *connection,
                                               board_connection_sp_cb_t func,
                                               void *user_data)
{
    int i;
    int rv = BCM_E_NONE;

    for (i=0; i<count; i++) {

        /* both have to be GPORTs */
        if (connection[i].from == BCM_GPORT_TYPE_NONE ||
            connection[i].to   == BCM_GPORT_TYPE_NONE) {
            continue;
        }

        /* both have to be stacking */
        rv = board_connection_gport_stacking(connection[i].from);

        if (BCM_FAILURE(rv)) {
            break;
        } else  if (rv != TRUE) {
            continue;
        }

        rv = board_connection_gport_stacking(connection[i].to);

        if (BCM_FAILURE(rv)) {
            break;
        } else  if (rv != TRUE) {
            continue;
        }

        /* Call the callback for each port */
        rv = func(&connection[i], user_data);

        if (BCM_FAILURE(rv)) {
            break;
        }

    }

    return rv;
}

/*
 * Function:
 *
 * Purpose:
 *     Mark the port as an internal stack port if the stack port
 *     is not yet enabled.
 * Parameters:
 *     gport - (IN) stack port
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_internal_stack_port_set(bcm_gport_t gport)
{
    int unit;
    uint32 flags;

    unit = BCM_GPORT_DEVPORT_DEVID_GET(gport);
    BCM_IF_ERROR_RETURN(bcm_stk_port_get(unit, gport, &flags));
    if (!(flags & BCM_STK_ENABLE)) {
        /* add as a stack port */
        BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, gport, INT_STK_FLAGS));
        /* allow all modids */
        BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit, gport,
                                                      -1, TRUE));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _internal_stack_connection_set
 * Purpose:
 *     Set the connection as internal stack ports
 * Parameters:
 *     conn      - (IN) internal stack port connection
 *     user_data - (IN) callback user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_internal_stack_connection_set(board_connection_t *conn, void *user_data)
{
    COMPILER_REFERENCE(user_data);
    BCM_IF_ERROR_RETURN(_internal_stack_port_set(conn->from));
    BCM_IF_ERROR_RETURN(_internal_stack_port_set(conn->to));

    return BCM_E_NONE;
}

/*
 * Function:
 *     board_connection_add_internal_stack_ports
 * Purpose:
 *     Add all connection pairs in list as internal stack ports
 * Parameters:
 *     count      - (IN) length of connection list
 *     connection - (IN) connection list
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_connection_add_internal_stack_ports(int count,
                                          board_connection_t *connection)
{
    return
        board_connection_connected_stack_port_traverse
           (count, connection, _internal_stack_connection_set, NULL);
}

/*
 * Function:
 *     _board_connection_info
 * Purpose:
 *     Tree Programmer 'info' function.
 *     Local interdevice programming
 *     Sets modid/trunk info for given unit
 * Parameters:
 *     unit      - (IN)  current unit
 *     modid     - (OUT) modid for this unit
 *     ti        - (OUT) trunk info for this unit
 *     user_data - (IN)  user data registered
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_connection_info(int unit, int *modid, bcm_trunk_chip_info_t *ti,
                       void *user_data)
{
    int rv;

    rv = bcm_trunk_chip_info_get(unit, ti);
    if (BCM_SUCCESS(rv)) {
        rv = bcm_stk_my_modid_get(unit, modid);
        if (BCM_FAILURE(rv) && (rv == BCM_E_UNAVAIL)) {
            /* Assume that this is a fabric */
            *modid = -1;
            rv = BCM_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *     _board_connection_trunk
 * Purpose:
 *     Tree Programmer 'edge' function.
 *     Program trunk if needed.
 * Parameters:
 *     unit      - (IN)  current unit
 *     ti        - (IN)  trunk info for this unit
 *     ta        - (OUT) trunk add info for this unit
 *     user_data - (IN)  user data registered
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_connection_trunk(int unit, bcm_trunk_chip_info_t *ti,
                        bcm_trunk_add_info_t *ta, void *user_data)
{
    int rv, tid;
    int i;
    bcm_trunk_member_t *member_array = NULL;
    bcm_trunk_info_t tinfo;

    if (ta->num_ports < 2) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    tid = ti->trunk_fabric_id_max;
    if (tid >= ti->trunk_fabric_id_min) {
        ta->psc = -1; /* Default hash algorithm */
        ta->dlf_index = -1;
        ta->mc_index = -1;
        ta->ipmc_index = -1;
        rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
        if (BCM_SUCCESS(rv)) {
            LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                        (BSL_META_U(unit,
                        "Board trunk u:%d tid:%d\n"),
                         unit, tid));
            member_array = sal_alloc(ta->num_ports * sizeof(bcm_trunk_member_t),
                    "trunk member array");
            if (NULL == member_array) {
                return BCM_E_MEMORY;
            }
            for (i = 0; i < ta->num_ports; i++) {
                bcm_trunk_member_t_init(&member_array[i]);
                if (BCM_GPORT_IS_SET(ta->tp[i])) {
                    member_array[i].gport = ta->tp[i];
                } else {
                    BCM_GPORT_LOCAL_SET(member_array[i].gport, ta->tp[i]);
                } 
            }
            bcm_trunk_info_t_init(&tinfo);
            tinfo.flags = ta->flags;
            tinfo.psc = ta->psc;
            tinfo.dlf_index = ta->dlf_index;
            tinfo.mc_index = ta->mc_index;
            tinfo.ipmc_index = ta->ipmc_index;
            rv = bcm_trunk_set(unit, tid, &tinfo, ta->num_ports, member_array);
            if (BCM_SUCCESS(rv)) {
                ti->trunk_fabric_id_max--;
            }
            sal_free(member_array);
        }
    } else {
        /* No more trunks */
        rv = BCM_E_FULL;
    }

    return rv;
}

/*
 * Function:
 *     _board_connection_modmap
 * Purpose:
 *     Tree Programmer 'vertex' function.
 *     Programs device modmap
 * Parameters:
 *     unit      - (IN) current unit
 *     modid     - (IN) modid for this unit
 *     port      - (IN) egressing stack port on this vertex
 *     user_data - (IN) user data registered
 * Returns:
 */

STATIC int
_board_connection_modmap(int unit, int modid, bcm_gport_t port,
                         void *user_data)
{
    int rv = BCM_E_NONE;

    if (modid >= 0) {
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META_U(unit,
                    "Board modmap mod=%d u:%d,%d\n"),
                     modid, unit, port));
        rv = bcm_stk_modport_set(unit, modid, port);
    }

    return rv;
}


/*
 * Function:
 *     board_connection_setup
 * Purpose:
 *     Sets up a board in a standard Ethernet switch configuration
 *     according to the board driver connection
 *     array. board_modid_set() must be called prior to calling this
 *     function.
 *
 *     Multiple Higig links between devices will be trunked, and module
 *     mapping configured for multiple devices.
 *
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_connection_setup(void)
{
    int rv;
    board_driver_t *driver;
    board_tree_driver_t tree_driver;

    driver = board_driver();

    if (!driver) {
        rv = BCM_E_INIT;
    } else if (driver->num_connection > 0) {
        tree_driver.num_connection = driver->num_connection;
        tree_driver.connection = driver->connection;
        tree_driver.user_data = NULL;
        tree_driver.info = _board_connection_info;
        tree_driver.edge = _board_connection_trunk;
        tree_driver.vertex = _board_connection_modmap;
        rv = board_tree_connect(&tree_driver);
    } else {
        /* No connections, nothing to set up */
        rv = BCM_E_NONE;
    }

    return rv;
}
