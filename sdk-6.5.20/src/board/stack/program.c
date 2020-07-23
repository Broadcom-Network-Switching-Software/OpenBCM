/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        program.c
 * Purpose:     board stack programming
 *
 *
 *
 * Methodology:
 *
 *   Create a connection list formed by the merger of the board's
 *   internal connections and external topology connections from the
 *   CPUDB. Create a tree driver from this connection list and the
 *   stack vertex and edge programming functions, and use
 *   board_tree_connect to program all the devices.
 *
 *   The edge programmer programs any trunks found on local units, and
 *   adds the edge port to the "allowable" stack ports (by default,
 *   all stack ports are disallowed).
 *
 *   The vertex programmer programs the modport map, modid enable, and
 *   topology map for every local unit.
 *
 *   Note that the unit numbers for remote units are synthesized, and
 *   must not correspond to any local unit number. bcm_unit_local()
 *   must return FALSE for any such remote unit, and the unit number
 *   used must be usable by BCM_GPORT_DEVPORT. These remote units
 *   numbers are not used as a unit argument except to bcm_unit_local,
 *   and are are relatively small, as they are used as an index into
 *   board_stk_pgm_t.stk_unit. This requirement is driven by the use
 *   of a connection list to drive the tree programming.
 *
 *   The topology map used for bcm_topo_port_get() is a two
 *   dimensional array constructed at stack programming time, and so
 *   is common for all systems.
 *
 *   The methodology used here is a significant departure from that
 *   used by stktask, and is more data driven, rather than using hand
 *   coded interfaces for each board.
 *
 *   Note that this subsystem, unlike stktask board programming,
 *   expects that internal stack connections between local devices
 *   have already been registered with BCM stack during local board
 *   initialization, so stack programming only has to deal with modid
 *   mapping on internal ports.
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/trunk.h>
#include <bcm/topo.h>
#if defined(INCLUDE_LIB_CPUDB)
#include <appl/cpudb/cpudb.h>
#endif
#if defined(INCLUDE_LIB_STKTASK)
#include <appl/stktask/topology.h>
#include <appl/stktask/stktask.h>       /* bcm_st_reserved_modid_enable_get */
#include <appl/cputrans/nh_tx.h>        /* nh_tx_src_get */
#endif


#include <board/manager.h>
#include <board/stack.h>
#include <board_int/support.h>

#if defined(INCLUDE_LIB_CPUDB) && defined(INCLUDE_LIB_STKTASK)


#ifndef BOARD_PGM_ABS_MAX_MODID
#define BOARD_PGM_ABS_MAX_MODID 32
#endif

typedef struct board_stk_unit_s {
    int local;                  /* true if a local unit */
    int unit_id;                /* unit identifier */
    int modid;                  /* modid for this unit */
    bcm_pbmp_t disports;        /* inactive stack ports (ext only) */
    bcm_pbmp_t enaports;        /* active stack ports (int+ext) */
    bcm_pbmp_t ethports;        /* non-stack ports (ext only) */
} board_stk_unit_t;

/* The topology map is a dynamically allocated 2D array indexed by
   unit number and modid containing a port */

typedef bcm_port_t board_topo_unit[BOARD_PGM_ABS_MAX_MODID];


/* Persistant data used by the stacking board programming subsystem */
typedef struct board_stk_info_s {
    int topo_valid;             /* true if this entry is valid */
    int max_modid;              /* maximum modid in the maps */
    int topo_units;             /* number of topo_map entries */
    board_topo_unit *topo_map;  /* topo map */
} board_stk_info_t;

/* Transient programming data */
typedef struct board_stk_pgm_s {
    void *alloc;                /* memory buffer */
    int unit_last;              /* last unit_id + 1 */
    board_stk_unit_t *stk_unit; /* per unit data */
    int *ru_offset;             /* indexed by topo_idx */
    int max_conn;               /* maximum number of connections managed */
    board_tree_driver_t td;     /* tree driver used */
} board_stk_pgm_t;

STATIC board_stk_info_t _board_stack_info;

/*
 * Function:
 *     _board_topo_port_get
 * Purpose:
 *     Topology mapping function registered with BCM
 * Parameters:
 *     unit       - (IN)  unit to map
 *     dest_modid - (IN)  modid to map
 *     exit_port  - (OUT) physical port (unit,modid) egresses
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_topo_port_get(int unit, int dest_modid, bcm_port_t *exit_port)
{
    int rv = BCM_E_PARAM;

    if (_board_stack_info.topo_map == NULL ||!_board_stack_info.topo_valid) {
        rv = BCM_E_UNAVAIL;
    } else if (unit >= 0 && unit < _board_stack_info.topo_units &&
               dest_modid >= 0 &&  dest_modid < _board_stack_info.max_modid) {
        *exit_port = _board_stack_info.topo_map[unit][dest_modid];
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     _board_program_info
 * Purpose:
 *     Tree Programmer 'info' function.
 *     Sets modid/trunk info for given unit
 * Parameters:
 *     unit      - (IN)  current unit (see notes above)
 *     modid     - (OUT) modid for this unit
 *     ti        - (OUT) trunk info for this unit (local only)
 *     user_data - (IN)  user data registered
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_program_info(int unit, int *modid, bcm_trunk_chip_info_t *ti,
                    void *user_data)
{
    board_stk_pgm_t *pgm = (board_stk_pgm_t *)user_data;
    int rv = BCM_E_NONE;

    /* Copy modid out. Modid assignments have already been made
       available via the CPUDB, so this function just copies out the
       result of that analysis. */
    *modid = pgm->stk_unit[unit].modid;
    if (pgm->stk_unit[unit].local) {
        /* get chip info for local units */
        rv = bcm_trunk_chip_info_get(unit, ti);
    }

    return rv;
}

/*
 * Function:
 *     _board_program_edge
 * Purpose:
 *     Tree Programmer 'edge' function.
 *     Program trunk if needed and add port to 'enable' list.
 * Parameters:
 *     unit      - (IN)  current unit (see notes above)
 *     ti        - (IN)  trunk info for this unit (local only)
 *     ta        - (OUT) trunk add info for this unit (local only)
 *     user_data - (IN)  user data registered
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_board_program_edge(int unit, bcm_trunk_chip_info_t *ti,
                    bcm_trunk_add_info_t *ta, void *user_data)
{
    /* program trunk if needed */
    int i, rv, tid;
    bcm_trunk_member_t *member_array = NULL;
    bcm_trunk_info_t tinfo;

    rv = BCM_E_INTERNAL;

    if (ta->num_ports > 1) {
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
                member_array = sal_alloc(ta->num_ports *
                        sizeof(bcm_trunk_member_t), "trunk member array");
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
                rv = bcm_trunk_set(unit, tid, &tinfo, ta->num_ports,
                        member_array);
                if (BCM_SUCCESS(rv)) {
                    ti->trunk_fabric_id_max--;
                }
                sal_free(member_array);
            }
        } else {
            /* No more trunks */
            rv = BCM_E_FULL;
        }
    }

    if (BCM_SUCCESS(rv)) {
        for (i=0; i<ta->num_ports; i++) {
            /* Add port to enable list */
            
            rv = BCM_E_UNAVAIL;
        }
    }

    return rv;

}

/*
 * Function:
 *     _board_program_vertex
 * Purpose:
 *     Tree Programmer 'vertex' function.
 *     Programs device modmap, modid enables, and topo map.
 * Parameters:
 *     unit      - (IN) current unit (see notes above)
 *     modid     - (IN) modid for this unit
 *     port      - (IN) egressing stack port on this vertex
 *     user_data - (IN) user data registered
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */

STATIC int
_board_program_vertex(int unit, int modid, bcm_gport_t port, void *user_data)
{
    board_stk_pgm_t *pgm = (board_stk_pgm_t *)user_data;
    int rv;

    rv = BCM_E_NONE;

    do {
        if (!pgm->stk_unit[unit].local) {
            break;
        }

        if (modid < 0) {
            break;
        }

        /* modmap modid to port */
        LOG_VERBOSE(BSL_LS_BOARD_COMMON,
                    (BSL_META_U(unit,
                    "Board modmap mod=%d u:%d,%d\n"),
                     modid, unit, port));
        rv = bcm_stk_modport_set(unit, modid, port);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* allow modid on port */
        rv = bcm_port_modid_enable_set(unit, port, modid, TRUE);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* add port to topo map */
        _board_stack_info.topo_map[unit][modid] =
            BCM_GPORT_DEVPORT_PORT_GET(port);

    } while (0);

    return rv;
}

/*
 * Function:
 *     _stk_gport
 * Purpose:
 *     Return a DEVPORT for the given stack port.
 * Parameters:
 *      sp     - (IN) CPUDB stack port structure
 *      offset - (IN) synthetic remote unit offset
 * Returns:
 *      DEVPORT
 * Notes:
 *     Units for local stack ports are real, and units for remote
 *     stack ports are synthetic, and must not correspond to any local
 *     unit number. They could land up corresponding to a currently
 *     attached remote unit number, but that's OK, because they should
 *     never be used as an argument to a BCM API call.
 */
STATIC bcm_gport_t
_stk_gport(cpudb_unit_port_t *sp, int offset)
{
    return BCM_GPORT_DEVPORT(sp->unit + offset, sp->port);
}

/*
 * Function:
 *     _topo_port
 * Purpose:
 *     Determine the egressing port for packets from src to dst.
 * Parameters:
 *     db_ref    - (IN)  CPUDB
 *     src       - (IN)  source entry
 *     dst       - (IN)  destination entry
 *     ru_offset - (IN)  remote unit offset array
 *     devport   - (OUT) egress port
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 * Notes:
 *     This is a deliberate alternative to using the TOPO_CPU construction.
 *     This will reduce the coupling to stktask, and eliminate such coupling
 *     if and when topology is moved to a separate module.
 */
STATIC int
_topo_port(cpudb_ref_t db_ref,
           cpudb_entry_t *src,
           cpudb_entry_t *dst,
           int *ru_offset,
           bcm_gport_t *devport)
{
    int idx;
    int rv;
    int offset;

    offset = ru_offset[src->topo_idx];
    rv = topo_tx_port_get(db_ref, src, dst, &idx);
    if (BCM_SUCCESS(rv)) {
        *devport = _stk_gport(&src->base.stk_ports[idx], offset);
    }
    return rv;
}


/*
 * Function:
 *     _cpudb_connection
 * Purpose:
 *     Returns a connection list from the CPUDB suitable for the
 *     stack Tree programmer.
 * Parameters:
 *      db_ref         - (IN)  CPUDB
 *      max_connection - (IN)  max size of connection list
 *      list           - (OUT) connection list
 *      actual         - (OUT) actual size of list
 *      ru_offset      - (IN)  ru_offset array
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_cpudb_connection(cpudb_ref_t db_ref,
                  int max_connection,
                  board_connection_t *list,
                  int *actual,
                  int *ru_offset)
{
    cpudb_entry_t *src, *dst;
    board_connection_t conn;
    int count = 0;

    /* This gets *all* the connections in the topology, not just
       the connections from the local system.
    */
    CPUDB_FOREACH_ENTRY(db_ref, src) {
        CPUDB_FOREACH_ENTRY(db_ref, dst) {
            if (src == dst) {
                /* only iterate across the lower triangle of the matrix
                   below the diagonal. */
                break;
            }
            BCM_IF_ERROR_RETURN
                (_topo_port(db_ref, src, dst, ru_offset, &conn.from));
            BCM_IF_ERROR_RETURN
                (_topo_port(db_ref, dst, src, ru_offset, &conn.to));
            if (max_connection > 0) {
                *list++ = conn;
                max_connection--;
                count++;
            } else {
                return BCM_E_FULL;
            }
        }
    }

    if (actual) {
        *actual = count;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _pgm_unit_init
 * Purpose:
 *     Initialize board_stk_pgm_t structure
 * Parameters:
 *     db_ref  - (IN)  CPUDB
 *     pgm     - (OUT) board_stk_pgm_t to initialize
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_pgm_unit_init(cpudb_ref_t db_ref, board_stk_pgm_t *pgm)
{
    int offset = board_num_devices();
    int i, unit_id_base, local, unit_id_last = 0;
    cpudb_entry_t *entry;
    board_stk_unit_t *stkunit;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        /* local or remote */
        if (entry == db_ref->local_entry) {
            unit_id_base = 0;
            local = TRUE;
        } else {
            unit_id_base = offset;
            offset += entry->base.num_units;
            local = FALSE;
        }
        pgm->ru_offset[entry->topo_idx] = unit_id_base;

        for (i=0; i<entry->base.num_units; i++) {
            stkunit = &pgm->stk_unit[unit_id_base + i];
            stkunit->local = local;
            stkunit->unit_id = unit_id_base + i;
            stkunit->modid = entry->mod_ids[i];
        }
        unit_id_last = unit_id_base + entry->base.num_units;
    }
    pgm->unit_last = unit_id_last;
    return BCM_E_NONE;
}

#define NOT_DISABLED (BCM_STK_ENABLE|CPUDB_SPF_INACTIVE)

/*
 * Function:
 *     _bcm_stk_port_change
 * Purpose:
 *     Change stack port to given flags, avoiding unnecessary callbacks
 * Parameters:
 *     unit  - (IN) stack port unit
 *     port  - (IN) stack port on unit
 *     flags - (IN) requested flags
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_bcm_stk_port_change(int unit, int port, uint32 flags)
{
    int changing;
    uint32 cur_flags;

    /* get the current flags */
    BCM_IF_ERROR_RETURN(bcm_stk_port_get(unit, port, &cur_flags));
    changing = FALSE;

    /* If the stack port is going to be disabled and is currently
    enabled, or the BCM_STK_ENABLE or CPUDB_SPF_INACTIVE flags are
    changing state, then set the new stack port state. */

    

    if ((flags == 0 && (cur_flags & BCM_STK_ENABLE)) ||
        ((flags & NOT_DISABLED) != (cur_flags & NOT_DISABLED))) {
        changing = TRUE;
    }

    if (changing) {
        BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, port, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _pgm_stk_ports
 * Purpose:
 *     Program stack ports in local CPUDB entry
 * Parameters:
 *     db_ref - (IN) CPUDB
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_pgm_stk_ports(cpudb_ref_t db_ref)
{
    int rv, unit, port, i;
    uint32 flags, spflags;
    cpudb_entry_t *local;

    local = db_ref->local_entry;
    rv = BCM_E_NONE;

    /* TO DO:  Scan for ports that are no longer stack ports
     * Foreach local device
     *    get current bitmap
     *    foreach port in bitmap
     *       search local entry stack ports for (unit, port)
     *           if found, continue
     *           otherwise, call bcm_stk_port_set with flags = 0
     */

    /* Update stack ports */
    for (i = 0; i < local->base.num_stk_ports; i++) {
        unit = local->base.stk_ports[i].unit;
        port = local->base.stk_ports[i].port;
        flags = BCM_STK_ENABLE;
        spflags = local->sp_info[i].flags;

        /* See if port is inactive (explicit, cut or no link) */
        if (spflags & CPUDB_SPF_ETHERNET) {
            flags = 0; /* Disable - not a stack port anymore */
        } else if (spflags & CPUDB_SPF_NO_LINK) {
            flags |= BCM_STK_INACTIVE | BCM_STK_NO_LINK;
        } else if (spflags & CPUDB_SPF_INACTIVE) {
            flags |= BCM_STK_INACTIVE;
        } else if (spflags & CPUDB_SPF_CUT_PORT) {
            flags |= BCM_STK_CUT;
        } else { /* Port is active */
            /* Simplex/duplex: Only mark simplex if not duplex and resolved */
            if (spflags & CPUDB_SPF_DUPLEX) {
                flags |= BCM_STK_DUPLEX;
            } else {
                if ((spflags & CPUDB_SPF_TX_RESOLVED) &&
                    (spflags & CPUDB_SPF_RX_RESOLVED)) {
                    flags |= BCM_STK_SIMPLEX;
                }
            } /* Else, port is not yet resolved */
        }

        rv = _bcm_stk_port_change(unit, port, flags);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *     _stk_gport_enable
 * Purpose:
 *     Adds given stack port to enable list
 * Parameters:
 *     pgm   - (INOUT) pgm structure
 *     gport - (IN)    stack port
 * Returns:
 *     void
 */
STATIC void
_stk_gport_enable(board_stk_pgm_t *pgm, bcm_gport_t gport)
{
    int unit;
    int port;

    unit = BCM_GPORT_DEVPORT_DEVID_GET(gport);
    port = BCM_GPORT_DEVPORT_PORT_GET(gport);

    BCM_PBMP_PORT_ADD(pgm->stk_unit[unit].enaports, port);
}

/*
 * Function:
 *     _pgm_internal_sp
 * Purpose:
 *     Callback function to add connection as internal stack ports.
 * Parameters:
 *     conn      - (IN)    internal stack port connection
 *     user_data - (INOUT) user data from callback registration
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_pgm_internal_sp(board_connection_t *conn, void *user_data)
{
    board_stk_pgm_t *pgm = (board_stk_pgm_t *)user_data;

    /* add each port to enable list */
    _stk_gport_enable(pgm, conn->from);
    _stk_gport_enable(pgm, conn->to);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _manage_internal_stack_ports
 * Purpose:
 *     Add all connection pairs in list as internal stack ports
 * Parameters:
 *     pgm         - (INOUT) pgm structure
 *     count       - (IN)    connection list length
 *     connection  - (IN)    connection list
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_manage_internal_stack_ports(board_stk_pgm_t *pgm,
                             int count,
                             board_connection_t *connection)
{
    return
        board_connection_connected_stack_port_traverse(count,
                                                       connection,
                                                       _pgm_internal_sp,
                                                       pgm);
}

/*
 * Function:
 *     _manage_external_stack_ports
 * Purpose:
 *     Add CPUDB external ports to appropriate pgm members
 * Parameters:
 *     pgm     - (INOUT) pgm structure
 *     db_ref  - (IN)    CPUDB
 * Returns:
 *     BCM_E_NONE - success
 */
STATIC int
_manage_external_stack_ports(board_stk_pgm_t *pgm, cpudb_ref_t db_ref)
{
    cpudb_entry_t *local = db_ref->local_entry;
    int unit, port, i;
    uint32 spflags;
    /* Update stack ports */

    for (i = 0; i < local->base.num_stk_ports; i++) {
        unit = local->base.stk_ports[i].unit;
        port = local->base.stk_ports[i].port;
        spflags = local->sp_info[i].flags;

        if ((spflags & CPUDB_SPF_TX_RESOLVED) &&
            (spflags & CPUDB_SPF_RX_RESOLVED)) {
            /* rx/tx resolved -> enaports */
            BCM_PBMP_PORT_ADD(pgm->stk_unit[unit].enaports, port);
        } else if (spflags & CPUDB_SPF_ETHERNET) {
            /* not stacking -> ethports */
            BCM_PBMP_PORT_ADD(pgm->stk_unit[unit].ethports, port);
        } else {
            /* otherwise -> disports */
            BCM_PBMP_PORT_ADD(pgm->stk_unit[unit].disports, port);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _pgm_conn_init
 * Purpose:
 *     Initialize pgm structure connection data
 * Parameters:
 *     pgm    - (INOUT) pgm structure
 *     board  - (IN)    board driver
 *     db_ref - (IN)    CPUDB
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_pgm_conn_init(board_stk_pgm_t *pgm, board_driver_t *board, cpudb_ref_t db_ref)
{
    int rv, added;

    /* Get internal connections */

    rv = BCM_E_FULL;
    if (board->num_connection < (pgm->max_conn - pgm->td.num_connection)) {

        /* get the internal connections */
        sal_memcpy(pgm->td.connection, board->connection,
                   board->num_connection * sizeof(pgm->td.connection[0]));
        pgm->td.num_connection += board->num_connection;

        /* manage internal connections */
        BCM_IF_ERROR_RETURN
            (_manage_internal_stack_ports(pgm,
                                          board->num_connection,
                                          board->connection));
    }

    BCM_IF_ERROR_RETURN(_manage_external_stack_ports(pgm, db_ref));

    BCM_IF_ERROR_RETURN(_pgm_stk_ports(db_ref));

    /* Get external connections, and remote unit offset */
    rv = _cpudb_connection(db_ref, pgm->max_conn,
                           pgm->td.connection+pgm->td.num_connection,
                           &added, pgm->ru_offset);
    if (BCM_SUCCESS(rv)) {
        pgm->td.num_connection += added;
        pgm->td.user_data = pgm;
        pgm->td.info = _board_program_info;
        pgm->td.edge = _board_program_edge;
        pgm->td.vertex = _board_program_vertex;
    }

    return rv;
}

/* byte offset in allocation */
#define OFFSET(m,offset) ((void *)(((uint8 *)(m)) + (int)(offset)))

/*
 * Function:
 *     _stk_pgm_init
 * Purpose:
 *     Initialze pgm structure
 * Parameters:
 *     pgm    - (OUT) pgm structure
 *     board  - (IN)  board driver
 *     db_ref - (IN)  CPUDB
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_stk_pgm_init(board_stk_pgm_t *pgm, board_driver_t *board, cpudb_ref_t db_ref)
{
    int rv, units, entries, su_size, ru_size, cn_size;
    cpudb_entry_t *entry;

    /*
      Count how many entries and units are going to be managed.  Units
      are in stk_unit by unit id. The unit id is equivalant to the SDK
      unit number when the unit is local, and is offset by the number
      of units on the board (to force such units to be recognized as
      remote) for all remote units, so the total allocation needed is
      the total number of units found in the CPUDB plus the number of
      units on this board.

    */

    rv = BCM_E_NONE;
    units = entries = 0;
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        entries++;
        units += entry->base.num_units;
    }
    units += board_num_devices();

    /* init struct */
    sal_memset(pgm, 0, sizeof(*pgm));

    /* allocate and init stk_unit */
    su_size = units * sizeof(pgm->stk_unit[0]);
    ru_size = entries * sizeof(pgm->ru_offset[0]);

    /* cn_size, at most, is the number of internal connections plus
       the square of the number of CPUDB entries divided by two, as
       there is only one topological connection between any given pair
       of systems.
    */
    pgm->max_conn = board->num_connection + (entries*entries/2);
    pgm->td.num_connection = 0;
    cn_size = pgm->max_conn * sizeof(pgm->td.connection[0]);
    pgm->alloc = ALLOC(su_size + ru_size + cn_size);
    if (pgm->alloc != NULL) {
        sal_memset(pgm->alloc, 0, su_size + ru_size + cn_size);
        pgm->stk_unit = OFFSET(pgm->alloc, 0);
        pgm->ru_offset = OFFSET(pgm->alloc, su_size);
        pgm->td.connection = OFFSET(pgm->alloc, su_size + ru_size);
        rv = _pgm_unit_init(db_ref, pgm);
        if (BCM_SUCCESS(rv)) {
            rv = _pgm_conn_init(pgm, board, db_ref);
        }
    }

    if (BCM_FAILURE(rv) && pgm->alloc != NULL) {
        FREE(pgm->alloc);
    }

    return rv;
}

/*
 * Function:
 *     _nh_reserved_modid_set
 * Purpose:
 *     Gets the reserved Next Hop modid for the given unit and port
 * Parameters:
 *     unit - (IN) stack unit
 *     port - (IN) port on stack unit
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_nh_reserved_modid_set(int unit, bcm_port_t port)
{
    int nhmod = -1;
    int nhport;
    int rv = BCM_E_NONE;

    if (bcm_st_reserved_modid_enable_get()) {
        rv = nh_tx_src_mod_port_get(unit, port, &nhmod, &nhport);
        if (BCM_FAILURE(rv) || nhmod < 0) {
            rv = nh_tx_src_get(&nhmod, &nhport);
        }
        if (BCM_SUCCESS(rv) && nhmod >= 0) {
            rv = bcm_port_modid_enable_set(unit, port, nhmod, TRUE);
        }
    }

    return rv;
}

/*
 * Function:
 *     _stk_program_unit
 * Purpose:
 *     Program the given unit for stacking
 * Parameters:
 *     stkunit - (IN) programming data
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_stk_program_unit(board_stk_unit_t *stkunit)
{
    bcm_port_t from, to;
    int rv = BCM_E_NONE;

    BCM_PBMP_ITER(stkunit->disports, from) {
        /* block all disports -> enaports */
        BCM_PBMP_ITER(stkunit->enaports, to) {
            rv = bcm_port_flood_block_set(stkunit->unit_id, from, to, -1U);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
        /* block all disports -> disports */
        BCM_PBMP_ITER(stkunit->disports, to) {
            if (from == to) {
                continue;
            }
            rv = bcm_port_flood_block_set(stkunit->unit_id, from, to, -1U);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    BCM_PBMP_ITER(stkunit->enaports, from) {
        /* allow all enaports -> enaports */
        BCM_PBMP_ITER(stkunit->enaports, to) {
            rv = bcm_port_flood_block_set(stkunit->unit_id, from, to, 0);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
        /* block all enaports -> disports */
        BCM_PBMP_ITER(stkunit->enaports, to) {
            rv = bcm_port_flood_block_set(stkunit->unit_id, from, to, -1U);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    /* reserved modid enabled on all enaports */
    BCM_PBMP_ITER(stkunit->enaports, from) {
        rv = _nh_reserved_modid_set(stkunit->unit_id, from);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* all modids enabled on all ethports */
    BCM_PBMP_ITER(stkunit->ethports, from) {
        rv = bcm_port_modid_enable_set(stkunit->unit_id, from, -1, TRUE);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *     _stk_pgm_finish
 * Purpose:
 *     Finish stack programming for all local devices
 * Parameters:
 *     pgm    - (IN) pgm structure
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_stk_pgm_finish(board_stk_pgm_t *pgm)
{
    int i, rv;

    rv = BCM_E_NONE;

    for (i=0; i<pgm->unit_last; i++) {
        if (pgm->stk_unit[i].local) {
            rv = _stk_program_unit(&pgm->stk_unit[i]);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     _stk_pgm_free
 * Purpose:
 *     Free allocate pgm data
 * Parameters:
 *     pgm    - (IN) pgm structure
 * Returns:
 *     void
 */
STATIC void
_stk_pgm_free(board_stk_pgm_t *pgm)
{
    FREE(pgm->alloc);
}

/*
 * Function:
 *     _stk_topo_map_init
 * Purpose:
 *     Initializes board topology data
 * Parameters:
 *     db_ref  - (IN) CPUDB
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
STATIC int
_stk_topo_map_init(cpudb_ref_t db_ref)
{
    int units;
    int rv = BCM_E_FAIL;
    void *map;

    _board_stack_info.topo_valid = FALSE;
    map = _board_stack_info.topo_map;
    if (map) {
        /* Force current map to be unavailable */
        _board_stack_info.topo_map = NULL;

        /* now free the memory */
        FREE(_board_stack_info.topo_map);
    }

    /* Allocate topo map */
    units = db_ref->local_entry->base.num_units;
    _board_stack_info.topo_map =
        ALLOC(units * sizeof(_board_stack_info.topo_map[0]));

    if (_board_stack_info.topo_map) {
        _board_stack_info.topo_units = db_ref->local_entry->base.num_units;
        rv = BCM_E_NONE;
    } else {
        rv = BCM_E_MEMORY;
    }

    return rv;
}

/*
 * Function:
 *     board_stack_program
 * Purpose:
 *     Program the current board for stacking operation
 * Parameters:
 *     db_ref - (IN) CPUDB
 *     flags  - (IN) flags
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX - failed
 */
int
board_stack_program(cpudb_ref_t db_ref, uint32 flags)
{
    board_stk_pgm_t pgm;
    board_driver_t *board;

    board = board_driver();

    if (board == NULL) {
        return BCM_E_INIT;
    }

    /* init pgm struct */
    BCM_IF_ERROR_RETURN(_stk_pgm_init(&pgm, board, db_ref));

    /* init topo map */
    BCM_IF_ERROR_RETURN(_stk_topo_map_init(db_ref));

    /* run tree programmer */
    BCM_IF_ERROR_RETURN(board_tree_connect(&pgm.td));

    /* deal with enabled/disabled ports and flood blocking */
    BCM_IF_ERROR_RETURN(_stk_pgm_finish(&pgm));

    /* set topo map, and make available */
    BCM_IF_ERROR_RETURN(bcm_topo_map_set(_board_topo_port_get));
    _board_stack_info.topo_valid = TRUE;

    _stk_pgm_free(&pgm);

    return BCM_E_NONE;
}

#endif
