/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgs.c
 * Purpose:     Simple XGS board programming support library
 */

#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <board/board.h>
#if defined(INCLUDE_LIB_STKTASK)
#include <appl/stktask/topo_brd.h>
#endif
#if defined(INCLUDE_LIB_DISCOVER)
#include <appl/discover/disc.h>
#endif
#include <board/manager.h>
#include <board/stack.h>
#include <board/xgs.h>
#include <board_int/support.h>

#ifndef MAX_DESCRIPTION
#define MAX_DESCRIPTION 80
#endif

#define UNIT0 0

STATIC int
_board_xgs_modid_get(board_driver_t *driver, char *name, int *modid);
STATIC int
_board_xgs_modid_set(board_driver_t *driver, char *name, int modid);
STATIC int
_board_xgs_num_modid_get(board_driver_t *driver,
                         char *name, int *num_modid);

#define PRIVATE(field) \
  (((board_xgs_data_t *)(driver->user_data))->field)

board_attribute_t board_xgs_attrib[] = {
    {
        BOARD_ATTRIBUTE_MODID,
        _board_xgs_modid_get,
        _board_xgs_modid_set,
    },
    {
        BOARD_ATTRIBUTE_NUM_MODID,
        _board_xgs_num_modid_get,
        NULL
    }
};



/*
 * Function:
 *     _board_xgs_modid_get
 * Purpose:
 *     Get board modid
 * Parameters:
 *     driver  - (IN)  current driver
 *     name    - (IN)  attribute name (ignored)
 *     modid   - (OUT) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_modid_get(board_driver_t *driver, char *name, int *modid)
{
    if (!modid) {
        return BCM_E_PARAM;
    }

    *modid = PRIVATE(device[UNIT0].modid);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _adjust_modid
 * Purpose:
 *     If the given modid does not match the mask constraint,
 *     adjust it so it does
 * Parameters:
 *     device - (IN)    device data
 *     modid  - (INOUT) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_adjust_modid(board_xgs_device_data_t *device, int *modid)
{
    if ((device->modid_mask & *modid) != *modid) {
        *modid = (device->modid_mask & *modid) + device->num_modid;
    }
    device->modid = *modid;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _board_xgs_modid_set
 * Purpose:
 *     Set the board modid
 * Parameters:
 *     driver  - (IN)  current driver
 *     name    - (IN)  attribute name (ignored)
 *     modid   - (IN) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_modid_set(board_driver_t *driver, char *name, int modid)
{
    int unit;
    int current_modid;

    current_modid = modid;

    for (unit=0; unit < PRIVATE(num_device); unit++) {
        _adjust_modid(&PRIVATE(device[unit]), &current_modid);
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, current_modid));
        PRIVATE(device[unit].modid) = current_modid;
        current_modid += PRIVATE(device[unit].num_modid);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _board_xgs_num_modid_get
 * Purpose:
 *     Get the number of modids required by this board
 * Parameters:
 *     driver  - (IN)  current driver (ignored)
 *     name    - (IN)  attribute name (ignored)
 *     modid   - (OUT) modid
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_num_modid_get(board_driver_t *driver,
                        char *name, int *num_modid)
{
    int unit;
    int nm;

    if (!num_modid) {
        return BCM_E_PARAM;
    }

    nm = 0;

    for (unit=0; unit < PRIVATE(num_device); unit++) {
        nm += PRIVATE(device[unit].num_modid);
    }

    *num_modid = nm;

    return BCM_E_NONE;
}

#if defined(INCLUDE_LIB_CPUDB)

/*
 * Function:
 *     _board_xgs_stack_init
 * Purpose:
 *     Initialize cpudb and required stacking subsystems
 * Parameters:
 *     driver  - (IN)  current driver
 *     db_ref  - (OUT) stack CPUDB
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_stack_init(board_driver_t *driver, cpudb_ref_t db_ref)
{
    BCM_IF_ERROR_RETURN(board_cpudb_get(driver, db_ref));
#if defined(INCLUDE_LIB_DISCOVER)
    disc_ttl_max_set(2);
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *     _board_xgs_stack
 * Purpose:
 *     Manage stktask callbacks
 * Parameters:
 *     op        - (IN)     current driver
 *     db_ref    - (IN/OUT) stack CPUDB
 *     topo_cpu  - (IN)     stack TOPO_CPU (ignored)
 *     user_data - (IN)     user data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_stack(topo_board_control_t op,
                 cpudb_ref_t db_ref,
                 topo_cpu_t *topo_cpu,
                 void *user_data)
{
    int rv = BCM_E_PARAM;
    board_driver_t *driver = (board_driver_t *)user_data;

    switch (op) {
    case topoBoardInit:
        rv = _board_xgs_stack_init(driver, db_ref);
        break;

    case topoBoardProgram:
        rv = board_stack_program(db_ref, 0);
        break;

    default:
        break;

    }

    return rv;
}
#endif

/*
 * Function:
 *     board_xgs_description
 * Purpose:
 *     Return the board description string
 * Parameters:
 *     driver - (IN)  current driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
char *
board_xgs_description(board_driver_t *driver)
{
    return PRIVATE(description);
}

/*
 * Function:
 *     board_xgs_probe
 * Purpose:
 *     Test if board devices are appropriate for this board.
 * Parameters:
 *      driver - (IN) board driver
 *      num    - (IN) number of devices; length of info array
 *      info   - (IN) array of bcm_info_t
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_xgs_probe(board_driver_t *driver, int num, bcm_info_t *info)
{
    int i,j,idx,rv,match;

    rv = BCM_E_FAIL;

    if (num == PRIVATE(num_device)) {

        /* The number of supported devices needs to be modulo the number
           of devices on the board. */
        if (PRIVATE(num_supported_device) % PRIVATE(num_device)) {
            LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                        (BSL_META("%s: devtab not modulo numdev\n"),
                         driver->name));
            return BCM_E_CONFIG;
        }
        for (i=0; i<PRIVATE(num_supported_device); i+=PRIVATE(num_device)) {
            match=0;

            /* Test successive num_device entries for a match */
            for (j=0; j<PRIVATE(num_device); j++) {
                idx = i+j;
                if ((PRIVATE(supported_device[idx]) ==
                                                BOARD_DRIVER_ANY_DEVICE) ||
                    (PRIVATE(supported_device[idx]) == info[j].device)) {
                    match++;
                } else {
                    /* No match, try next set of devices */
                    break;
                }
            }
            if (match == PRIVATE(num_device)) {
                /* Found something */
                LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                            (BSL_META("%s: units match\n"),
                             driver->name));
                rv = BCM_E_NONE;
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _board_xgs_banner
 * Purpose:
 *     Setup the board description string
 * Parameters:
 *      driver - (IN) board driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_banner(board_driver_t *driver)
{
    if (PRIVATE(description) == NULL) {
        PRIVATE(description) = sal_alloc(MAX_DESCRIPTION, driver->name);
        if (PRIVATE(description) == NULL) {
            return BCM_E_MEMORY;
        }
    }

    sal_sprintf(PRIVATE(description), "%s board driver", driver->name);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _board_xgs_add_internal_connections
 * Purpose:
 *     Add internal connections to probed stackable ports to
 *     construct a merged connection list for this board
 * Parameters:
 *      driver - (IN) board driver
 *      count  - (IN) number of connections
 *      conn   - (IN) connection list
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 * Notes:
 *     gports are not conveniently orderable, so merge with brute force
 */
STATIC int
_board_xgs_add_internal_connections(board_driver_t *driver,
                                    int *count, board_connection_t *conn)
{
    int i,j,replace,end,d_idx;
    bcm_gport_t i_port;
    board_connection_t *internal = PRIVATE(internal_connection);

    end = *count;
    for (i=0; i<PRIVATE(num_internal_connection); i++) {

        replace = -1;
        d_idx = -1;
        for (j=0; j<end; j++) {

            i_port = BCM_GPORT_TYPE_NONE;

            /* If either internal port matches the current connection
               list .from port, copy the 'other' internal port to the
               connection list .to entry, and prepare to remove the
               other internal port from the connection list. This
               should always be found after the current connection
               list position. */
            if (conn[j].from == internal[i].from) {
                i_port = internal[i].to;
            } else if (conn[j].from == internal[i].to) {
                i_port = internal[i].from;
            }

            if (i_port != BCM_GPORT_TYPE_NONE) {
                if (replace < 0) {
                    LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                                (BSL_META("Adding internal connection %d to conn %d\n"),
                                 i, j));
                    /* if internal_connection.from matches conn.from */
                    conn[j].to = i_port;
                    replace = i;
                } else {
                    /* one side of internal matched, delete other side */
                    d_idx = j;
                    break;
                }
            }
        }

        if (d_idx >= 0) {
            /* move up entries over deleted entry */
            for (j=d_idx; j<end; j++) {
                LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                            (BSL_META("Copying %d to %d\n"),
                             j+1, j));
                conn[j] = conn[j+1];
            }
            end -= 1;
            LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                        (BSL_META("Delete, end now %d\n"),
                         end));
        } else if (replace < 0) {
            /* add the internal entry to the end of the list */
            LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                        (BSL_META("Copying internal %d to %d\n"),
                         i, end));
            sal_memcpy(&conn[end], &internal[i], sizeof(conn[end]));
            end += 1;
            LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                        (BSL_META("Add internal, end now %d\n"),
                         end));
        }
    }

    LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                (BSL_META("Board: %d internal, %d stack connections\n"),
                 PRIVATE(num_internal_connection),
                 end - PRIVATE(num_internal_connection)));

    *count = end;
    return BCM_E_NONE;
}



/*
 * Function:
 *     _board_xgs_initialize_connections
 * Purpose:
 *     Create connections for board based on requested and probed data
 * Parameters:
 *      driver - (IN) board driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_initialize_connections(board_driver_t *driver)
{
    board_connection_t *conn;
    int count;
    int rv = BCM_E_FAIL;

    /* create connection array with the largest possible length */

    count = PRIVATE(num_device) * BCM_PBMP_PORT_MAX;
    conn = sal_alloc(count * sizeof(*conn), driver->name);

    if (conn) {
        sal_memset(conn, 0, count * sizeof(*conn));
        rv = board_connection_stack_ports(count, conn, &count);
        if (BCM_SUCCESS(rv)) {
            /* Go though the internal connection list */
            rv = _board_xgs_add_internal_connections(driver, &count, conn);
        } else {
            sal_free(conn);
        }
    } else {
        rv = BCM_E_MEMORY;
    }

    if (BCM_SUCCESS(rv)) {
        /* Add connection list to driver structure */
        driver->connection = conn;
        driver->num_connection = count;
    }
    return rv;
}

/*
 * Function:
 *     _board_xgs_initialize
 * Purpose:
 *     Initialize driver data
 * Parameters:
 *     driver - (IN) board driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_initialize(board_driver_t *driver)
{
    BCM_IF_ERROR_RETURN
        (board_connection_add_internal_stack_ports
         (PRIVATE(num_internal_connection),
          PRIVATE(internal_connection)));
    BCM_IF_ERROR_RETURN(_board_xgs_initialize_connections(driver));
    BCM_IF_ERROR_RETURN(board_connection_setup());
    return BCM_E_NONE;

}

/*
 * Function:
 *     _board_xgs_cold_boot
 * Purpose:
 *     Cold boot device
 * Parameters:
 *     driver - (IN) board driver
 *     unit   - (IN) BCM device number
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_cold_boot(board_driver_t *driver, int unit)
{
    BCM_IF_ERROR_RETURN(board_device_init(unit));
    BCM_IF_ERROR_RETURN(board_port_init(unit));

    /* I2C devices are only on unit 0 */
    if (unit == UNIT0) {
#if defined(INCLUDE_I2C)
        BCM_IF_ERROR_RETURN(board_i2c_device_init(unit));
#endif
        
    }

    BCM_IF_ERROR_RETURN(bcm_info_get(unit, &PRIVATE(device[unit].info)));

    BCM_IF_ERROR_RETURN
        (bcm_stk_modid_count(unit, &PRIVATE(device[unit].num_modid)));
    
    PRIVATE(device[unit].modid_mask) =
        ~(PRIVATE(device[unit].num_modid) - 1);

    return BCM_E_NONE;
}

/*

*/
/*
 * Function:
 *     _board_xgs_init_modid
 * Purpose:
 *     Initialize modid on all devices
 * Parameters:
 *     driver - (IN) board driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_init_modid(board_driver_t *driver)
{
    int unit, modid, rv = BCM_E_NONE;

    for (unit=0; unit<PRIVATE(num_device); unit++) {
        rv = bcm_stk_my_modid_get(unit, &modid);
        if (BCM_SUCCESS(rv)) {
            rv = _board_xgs_modid_set(driver, NULL, modid);
        } else if (rv != BCM_E_UNAVAIL) {
            break; /* if the error is not UNAVAIL,
                      something bad is happening */
        }
    }

    return rv;
}

/*
 * Function:
 *     _board_xgs_warm_boot
 * Purpose:
 *     Warm Boot device
 * Parameters:
 *     driver - (IN) board driver
 *     unit   - (IN) BCM device number
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_warm_boot(board_driver_t *driver, int unit)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _board_xgs_clear
 * Purpose:
 *     Clear device
 * Parameters:
 *     driver - (IN) board driver
 *     unit   - (IN) BCM device number
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_xgs_clear(board_driver_t *driver, int unit)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     board_xgs_start
 * Purpose:
 *     XGS Board driver start function
 * Parameters:
 *     driver - (IN) board driver
 *     flags  - (IN) start flags
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_xgs_start(board_driver_t *driver, uint32 flags)
{
    int unit;

    /* allocate device instance data */
    if (PRIVATE(device) == NULL) {
        PRIVATE(device) = sal_alloc
            (sizeof(PRIVATE(device[UNIT0])) * (PRIVATE(num_device)),
             driver->name);
        if (PRIVATE(device) == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /* install attributes */
    driver->attribute = board_xgs_attrib;
    driver->num_attribute = COUNTOF(board_xgs_attrib);

    /* init all the units */
    for (unit=0; unit<PRIVATE(num_device); unit++) {
        if (flags & BOARD_START_F_WARM_BOOT) {
            BCM_IF_ERROR_RETURN(_board_xgs_warm_boot(driver, unit));
        } else if (flags & BOARD_START_F_CLEAR) {
            BCM_IF_ERROR_RETURN(_board_xgs_clear(driver, unit));
        } else {
            BCM_IF_ERROR_RETURN(_board_xgs_cold_boot(driver, unit));
        }
    }

    BCM_IF_ERROR_RETURN(_board_xgs_init_modid(driver));

    /* Hook into stacking */
#if defined(INCLUDE_LIB_STKTASK) && defined(INCLUDE_LIB_CPUDB)
    BCM_IF_ERROR_RETURN
        (topo_board_register(_board_xgs_stack, (void *)driver));
#endif
    BCM_IF_ERROR_RETURN(_board_xgs_initialize(driver));
    BCM_IF_ERROR_RETURN(_board_xgs_banner(driver));

    LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                (BSL_META_U(unit,
                "%s\n"),
                 PRIVATE(description)));

    return BCM_E_NONE;
}

/*
 * Function:
 *     board_xgs_stop
 * Purpose:
 *     XGS Board driver stop function
 * Parameters:
 *     driver - (IN) board driver
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_xgs_stop(board_driver_t *driver)
{
    board_xgs_device_data_t *dev;
    char *str;
    int unit;

    /* deinit all the units */
    for (unit=0; unit<PRIVATE(num_device); unit++) {
        BCM_IF_ERROR_RETURN(board_port_deinit(unit));
        BCM_IF_ERROR_RETURN(board_device_deinit(unit));
    }

    /* Reset dynamic data */
    driver->num_connection = 0;
    if (driver->connection) {
        sal_free(driver->connection);
        driver->connection = NULL;
    }

    dev = PRIVATE(device);
    PRIVATE(device) = NULL;
    sal_free(dev);

    str = PRIVATE(description);
    PRIVATE(description) = NULL;
    sal_free(str);

    return BCM_E_NONE;
}


