/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        populate.c
 * Purpose:     Stacking related functions
 *              Populate CPUDB based on board hardware
 */

#include <sal/core/alloc.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <board/board.h>
#if defined(INCLUDE_LIB_CPUDB)
#include <appl/cpudb/cpudb.h>
#endif

#include <board/stack.h>
#include <board_int/support.h>

#if defined(INCLUDE_LIB_CPUDB)

/* TRUE if sp_id is valid for cpudb entry */
#define CPUDB_VALID_SP_ID(entry, sp_id) \
    (entry != NULL && (sp_id) >= 0 && (sp_id) < entry->base.num_stk_ports)

/* Last stack port */
#define CPUDB_STK_PORT_LAST (CPUDB_STK_PORTS_MAX - 1)

/*
 * Function:
 *     _cpudb_stk_port_next_id_get
 * Purpose:
 *     Get the next valid stack port ID for the entry
 * Parameters:
 *     entry - (INOUT) CPUDB entry
 *     sp_id - (OUT)   stack port ID
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
STATIC int
_cpudb_stk_port_next_id_get(cpudb_entry_t *entry, int *sp_id)
{
    int rv = BCM_E_FAIL;

    if (entry->base.num_stk_ports < CPUDB_STK_PORT_LAST) {
        if (sp_id) {
            *sp_id = entry->base.num_stk_ports++;
            rv = BCM_E_NONE;
        } else {
            rv = BCM_E_PARAM;
        }
    }

    return rv;
}

/*
 * Function:
 *     _cpudb_stk_gport_add
 * Purpose:
 *     Add given DEVPORT as the given stack port ID in CPUDB entry
 * Parameters:
 *     entry - (INOUT) CPUDB entry
 *     sp_id - (OUT)   stack port ID
 *     gport - (IN)    stack port
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
STATIC int
_cpudb_stk_gport_add(cpudb_entry_t *entry, int sp_id, bcm_gport_t gport)
{
    int rv = BCM_E_FAIL;

    if (CPUDB_VALID_SP_ID(entry, sp_id)) {
        entry->base.stk_ports[sp_id].unit = BCM_GPORT_DEVPORT_DEVID_GET(gport);
        entry->base.stk_ports[sp_id].port = BCM_GPORT_DEVPORT_PORT_GET(gport);
        rv = BCM_E_NONE;
    }

    return rv;
}


/*
 * Function:
 *     _cpudb_stk_port_flags_set
 * Purpose:
 *     Set parameters for indicated stack port id
 * Parameters:
 *     entry  - (INOUT) CPUDB entry
 *     sp_id  - (OUT)   stack port ID
 *     flags  - (IN)    stack port flags
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
STATIC int
_cpudb_stk_port_flags_set(cpudb_entry_t *entry, int sp_id, uint32 flags)
{
    int rv = BCM_E_FAIL;

    if (CPUDB_VALID_SP_ID(entry, sp_id)) {
        cpudb_stk_port_t *sp;

        sp = &entry->sp_info[sp_id];
        sp->flags = flags;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     _board_probe_board_id
 * Purpose:
 *     Set the board ID based on a board attribute.
 * Parameters:
 *     driver - (IN)    board driver
 *     db_ref - (INOUT) CPUDB to set
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 * Note: This is only intended as a fallback for generic drivers.
 */

STATIC int
_board_probe_board_id(board_driver_t *driver, cpudb_ref_t db_ref)
{
    int rv;
    int board_attr_id;

    rv = board_attribute_get("cpudb_id", &board_attr_id);
    if (BCM_SUCCESS(rv)) {
        db_ref->local_entry->base.board_id = (CPUDB_BOARD_ID) board_attr_id;
    }

    return rv;
}

/*
 * Function:
 *     _external_stack
 * Purpose:
 *     Return information if a connection is an external
 *     stack port.
 * Parameters:
 *      connection - (IN)  connection to check
 *      gport      - (OUT) external stack port
 *      now_higig  - (OUT) true if external stack port is stacking
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
STATIC int
_external_stack(board_connection_t *connection,
               bcm_gport_t *gport, int *now_higig)
{
    int ext, encap;
    int rv = BCM_E_FAIL;

    ext = (connection->to == BCM_GPORT_TYPE_NONE);
    if (ext) {

        *gport = connection->from;

        rv = board_connection_gport_stackable(connection->from);

        if (BCM_SUCCESS(rv)) {
            rv = bcm_port_encap_get
                (BCM_GPORT_DEVPORT_DEVID_GET(connection->from),
                 connection->from,
                 &encap);
            if (BCM_SUCCESS(rv)) {
                if ((encap & BCM_PORT_ENCAP_HIGIG) ||
                    (encap & BCM_PORT_ENCAP_HIGIG2)) {
                    *now_higig = TRUE;
                }
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _add_sp
 * Purpose:
 *     Add a connection to CPUDB if it is an external stack port
 * Parameters:
 *     connection - (IN)    connection to check
 *     entry      - (INOUT) CPUDB entry
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
STATIC int
_add_sp(board_connection_t *connection, cpudb_entry_t *entry)
{
    bcm_gport_t gport;
    int sp_id;
    int now_higig = FALSE;
    int rv = BCM_E_NONE;

    if (BCM_SUCCESS(_external_stack(connection, &gport, &now_higig))) {
        rv = _cpudb_stk_port_next_id_get(entry, &sp_id);
        if (BCM_SUCCESS(rv)) {
            rv = _cpudb_stk_gport_add(entry, sp_id, gport);
            if (BCM_SUCCESS(rv)) {
                if (!now_higig) {
                    rv = _cpudb_stk_port_flags_set(entry, sp_id,
                                                   CPUDB_SPF_ETHERNET);
                }
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     board_cpudb_get
 * Purpose:
 *     Populate a CPUDB appropriate for the board driver
 * Parameters:
 *     driver - (IN)    current board driver
 *     db_ref - (INOUT) CPUDB to populate
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
int
board_cpudb_get(board_driver_t *driver, cpudb_ref_t db_ref)
{
    int i;
    int rv;
    int unit;
    cpudb_entry_t *entry;
    bcm_port_config_t port_info;
    int num_modids;
    int port;
    int cpu_unit;
    int cpu_port;
    int first_cpu_port;

    entry = db_ref->local_entry;

    /* Set fields */
    entry->base.master_pri = 0;
    entry->base.slot_id = -1;

    entry->tx_unit = -1;
    entry->tx_port = -1;
    entry->dest_mod = -1;
    entry->dest_port = -1;
    entry->topo_idx = -1;
    entry->base.num_units = board_num_devices();
    entry->base.num_stk_ports = 0;

    rv = BCM_E_FAIL;

    for (i=0; i<driver->num_connection; i++) {
        rv = _add_sp(&driver->connection[i], entry);
        if (BCM_FAILURE(rv)) {
            goto fail;
        }
    }

    /* modids */

    cpu_unit = -1;
    cpu_port = -1;
    first_cpu_port = -1;

    for (unit=0; unit < entry->base.num_units; unit++) {
        num_modids = 0;
        rv = bcm_stk_modid_count(unit, &num_modids);
        if (BCM_FAILURE(rv)) {
            goto fail;
        }
        entry->mod_ids[unit] = -1;

        /* -1 for single modid, -2 for dual modid (even) */
        entry->base.pref_mod_id[unit] = -num_modids;
        entry->base.mod_ids_req[unit] = num_modids;

        /* Select CPU port

        Use the first CPU port of a device that is assignable a modid.
        If no such device is found, use the CPU port of the first device.

        */

        if (cpu_port < 0) {

            if (unit > 0 && num_modids == 0) {
                /* More than one unit, but this units appears to be a fabric */
                continue;
            }

            /* Collect device info */
            rv = bcm_port_config_get(unit, &port_info);

            if (BCM_FAILURE(rv)) {
                goto fail;
            }

            BCM_PBMP_ITER(port_info.cpu, port) {
                break;
            }
            if (num_modids > 0) {
                cpu_unit = unit;
                cpu_port = port;
            } else if (unit == 0) {
                first_cpu_port = port;
            }
        }
    }

    /* If there still isn't a CPU port, just assign it to the first unit */
    if (cpu_unit == -1) {
        cpu_unit = 0;
        cpu_port = first_cpu_port;
    }

    entry->base.dest_unit = cpu_unit;
    entry->base.dest_port = cpu_port;

    if (entry->base.dest_unit < 0 || entry->base.dest_port < 0) {
        /* No point in having a CPUDB entry without a CPU */
        rv = BCM_E_CONFIG;
        goto fail;
    }

    /* board id */
    if (entry->base.num_units == 1) {
        (void)(_board_probe_board_id(driver, db_ref));
    }

    rv = BCM_E_NONE;

 fail:

    return rv;
}

#else
int board_cpudb_get;
#endif
