/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topo_brd.c
 * Purpose:
 * Requires:    
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/topo.h>
#include <bcm/stack.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#ifdef BCM_RPC_SUPPORT
#include <bcm_int/rpc/rpc.h>
#endif /* BCM_RPC_SUPPORT */

#include <appl/stktask/stktask.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/topo_pkt.h>  
#include <appl/stktask/attach.h>
#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/nh_tx.h>
#include <appl/cputrans/cpu2cpu.h>
#include <appl/cputrans/atp.h>
#include <appl/discover/disc.h>


#include <soc/drv.h>

#include "topo_int.h"
#include "brd_chassis_int.h"

int topo_board_op(topo_board_control_t op,
                  cpudb_ref_t db_ref, topo_cpu_t *topo_cpu);

static sal_mutex_t topo_data_lock;   /* Serves as init done indication */
#define TD_INIT (topo_data_lock != NULL)

#define TOPO_DATA_LOCK sal_mutex_take(topo_data_lock, sal_mutex_FOREVER)
#define TOPO_DATA_UNLOCK sal_mutex_give(topo_data_lock)

/* Manage the system topology information here */
topo_cpu_t bcm_board_topo_data;
volatile int topo_data_set;

/* Indicates whether stack ports can be set to HW linkscan mode */
int _topo_hw_linkscan = 1;


/*
 * Rapid Recovery
 *
 * The following data structure stores information for local stack ports
 * used for rapid recovery.
 *
 * Note:  The stack port index correspond to the same index used
 *        for stack port in databases such as cpudb, topo, and stacking.
 *        This convention has been used throughout the stacking code.
 */
typedef struct topo_sp_info_s {
    int          unit;   /* Stack port unit */
    bcm_port_t   port;   /* Stack port */
    bcm_trunk_t  tid;    /* Stack port trunk ID */
} topo_sp_info_t;

static topo_sp_info_t  topo_sp[CPUDB_STK_PORTS_MAX];

#define TOPO_SP_TID_GET(_idx)          (topo_sp[_idx].tid)
#define TOPO_SP_TID_SET(_idx, _tid)    (topo_sp[_idx].tid = _tid)

STATIC int topo_sp_idx_find(int unit, bcm_port_t port);
STATIC int topo_sp_init(cpudb_entry_t *entry);
STATIC int topo_sp_info_update(cpudb_entry_t *entry);


/*
 * Set/get the current topology CPU information.
 *
 * Should be set to NULL when in transition or invalid
 *
 * This is exposed inside this library, but shouldn't be used
 * above that level.
 */

int
bcm_board_topo_set(topo_cpu_t *tp_cpu)
{
    topo_data_set = FALSE;
    if (!TD_INIT) {
        if ((topo_data_lock = sal_mutex_create("topo_data_lock")) == NULL) {
            return BCM_E_MEMORY;
        }
    }

    TOPO_DATA_LOCK;
    if (tp_cpu != NULL) {
        if (tp_cpu != &bcm_board_topo_data) {
            /* Only copy if not the default */
            sal_memcpy(&bcm_board_topo_data, tp_cpu, sizeof(topo_cpu_t));
        }
        topo_data_set = TRUE;
    } else {
        topo_data_set = FALSE;
    }
    TOPO_DATA_UNLOCK;

    return BCM_E_NONE;
}

int
bcm_board_topo_get(topo_cpu_t **tp_cpu)
{
    if (topo_data_set) {
        *tp_cpu = &bcm_board_topo_data;
    } else {
        *tp_cpu = NULL;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *     topo_cpu_trans_purge
 * Purpose:
 *     Clears any pending transactions for given remote destination CPU.
 * Parameters:
 *     entry    - Entry for remote CPU
 * Returns:
 *     N/A
 */

STATIC void
topo_cpu_trans_purge(cpudb_entry_t *entry)
{
    atp_key_purge(entry->base.key);
#ifdef BCM_RPC_SUPPORT
    bcm_rpc_cleanup(entry->base.key);
#endif /* BCM_RPC_SUPPORT */
}


/*
 * Function:
 *     topo_cpu_trans_flush
 * Purpose:
 *     Checks and clears pending transactions for remote CPUs where
 *     a different local stack port is now used to transmit to.
 * Parameters:
 *     db_ref    - Database of current system configuration
 * Returns:
 *     BCM_E_NONE  Success
 * Notes:
 *     This routine should be called before a new topology is
 *     set for the system.
 */

STATIC int
topo_cpu_trans_flush(cpudb_ref_t db_ref)
{
    cpudb_ref_t    prev_db;
    cpudb_entry_t  *entry;
    cpudb_entry_t  *l_entry;
    cpudb_entry_t  *prev_entry;

    if ((prev_db = db_ref->old_db) == NULL) {
        return BCM_E_NONE;
    }

    l_entry = db_ref->local_entry;

    /* Check if path to get to this remote CPU has changed */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry == l_entry) {
            continue;
        }

        CPUDB_KEY_SEARCH(prev_db, entry->base.key, prev_entry);
        if (prev_entry != NULL) {
            if ((prev_entry->tx_unit == entry->tx_unit) &&
                (prev_entry->tx_port == entry->tx_port)) {
                continue;
            }
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META("TX path changed to CPU " CPUDB_KEY_FMT_EOLN),
                         CPUDB_KEY_DISP(entry->base.key)));            
            topo_cpu_trans_purge(entry);
        }
    }
                
    return BCM_E_NONE;
}

/*
 * Function:
 *      topo_board_setup
 * Purpose:
 *      Program a board according to the given CPUDB
 * Parameters:
 *      db_ref     - Data base with system info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Currently supports 24 Gig, 48 Gig and 48 FE stacking
 *      reference designs.
 *
 *      If CPUDB contains one entry, set up the topo CPU structure directly.
 *      Otherwise, (for now) call the topo packet generate and parse
 *      routines to generate the topo CPU structure.
 *
 *      This routine clears "topo_data_set" and will only set it back to true
 *      if the data is successfully generated.
 */

int
topo_board_setup(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Topology board setup\n")));
    
    topo_data_set = FALSE;
    if (!TD_INIT) {
        if ((topo_data_lock = sal_mutex_create("topo_data_lock")) == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (db_ref->local_entry == NULL || db_ref->num_cpus <= 0 ||
        db_ref->num_cpus > CPUDB_CPU_MAX) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO SETUP: Local entry %p, num cpus %d\n"),
                  db_ref->local_entry, db_ref->num_cpus));
        return BCM_E_PARAM;
    }

    TOPO_DATA_LOCK;
    /* Clear current topo data */
    sal_memset(&bcm_board_topo_data, 0, sizeof(topo_cpu_t));
    if (db_ref->num_cpus == 1) {

        /* All local external stack ports have 0 mod id associations. */
        if ((rv = cpudb_entry_copy(&bcm_board_topo_data.local_entry,
                db_ref->local_entry)) < 0) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("TOPO SETUP: Failed to copy entry\n")));
        } else {
            bcm_board_topo_data.num_cpus = 1;
            topo_data_set = TRUE;

            if ((rv = topo_board_program(db_ref, &bcm_board_topo_data)) < 0) {
                LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                         (BSL_META("TOPO SETUP: Failed topo brd program: %s\n"),
                          bcm_errmsg(rv)));
            }
        }
    } else {
        /* For now, use packet generate/parse */
        static uint8 _pkt_buf[TOPO_PKT_BYTES_MAX];
        int len;

        /* Generate topo pkt; then parse it to get topo cpu struct */
        if ((rv = topo_pkt_gen(db_ref, db_ref->local_entry,
                               _pkt_buf, TOPO_PKT_BYTES_MAX, &len)) < 0) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("TOPO SETUP: Failed pkt gen\n")));
        } else if ((rv = topo_pkt_parse(db_ref, db_ref->local_entry, _pkt_buf,
                             len, &bcm_board_topo_data, NULL)) < 0) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("TOPO SETUP: Failed pkt parse\n")));
        } else {
            bcm_board_topo_data.num_cpus = db_ref->num_cpus;
            topo_data_set = TRUE;
        }
    }
    /* Set master seq num if present */
    if (topo_data_set && db_ref->master_entry != NULL) {
        bcm_board_topo_data.master_seq_num =
            db_ref->master_entry->base.dseq_num;
    }
    TOPO_DATA_UNLOCK;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO SETUP: Exit rv %d, data set %d\n"),
                 rv, topo_data_set));
    return rv;
}

/* Set stack ports */

STATIC int
_topo_stk_ports_update(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu)
{
    int unit, port;
    uint32 flags, spflags;
    cpudb_entry_t *l_entry;
    int i;

    l_entry = db_ref->local_entry;

    /* TO DO:  Scan for ports that are no longer stack ports
     * Foreach local device
     *    get current bitmap
     *    foreach port in bitmap
     *       search local entry stack ports for (unit, port)
     *           if found, continue
     *           otherwise, call bcm_stk_port_set with flags = 0
     */

    /* Update stack ports */
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        unit = l_entry->base.stk_ports[i].unit;
        port = l_entry->base.stk_ports[i].port;
        flags = BCM_STK_ENABLE;
        spflags = l_entry->sp_info[i].flags;


#if !defined(TOPO_BRD_UPDATE_ALL_PORTS)
        /* See if stack port flags have changed since the old db.
           This is an optimization that depends on the old CPUDB being
           propagated. Applications that require pre SDK-5.6.3
           behavior can define TOPO_BRD_UPDATE_ALL_PORTS.
        */
        if ((cpudb_valid(db_ref->old_db)) &&
            (db_ref->old_db->local_entry->base.num_stk_ports ==
             l_entry->base.num_stk_ports) &&
            (db_ref->old_db->local_entry->base.stk_ports[i].unit == unit) &&
            (db_ref->old_db->local_entry->base.stk_ports[i].port == port) &&
            (db_ref->old_db->local_entry->sp_info[i].flags == spflags)) {
            continue;
        }
#endif

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
        BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, port, flags));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      topo_board_program
 * Purpose:
 *      Program a board given local topology info
 * Parameters:
 *      db_ref    - Data base of current configuration
 *      topo_cpu  - Info for local board programming
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Currently supports 24 Gig, 48 Gig and 48 FE stacking
 *      reference designs.
 */

STATIC int
_topo_board_default_program(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu)
{
    int i, j;
    int done;
    cpudb_entry_t *entry;
    cpudb_entry_t *l_entry;
    int rv_tmp, rv = BCM_E_FAIL;
    cpudb_unit_port_t *sp;
    uint32 flags;
            
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Topology board programming db_ref %p. old_db %p\n"),
                 db_ref, db_ref->old_db));
               
    l_entry = db_ref->local_entry;

    /* First, handle some things for the local entry */
    l_entry->flags |= CPUDB_F_TX_KNOWN;

    /* If not using the reserved modid feature, set the NH source
       modid on each resolved stack port */
    if (!bcm_st_reserved_modid_enable_get()) {
        int loc_mod_id = -1;

        /*
         * Program NH TX source module to a local mod ID;
         * First, find a local mod ID, if present
         */
        for (i = 0; i < l_entry->base.num_units; i++) {
            if (l_entry->mod_ids[i] >= 0) {
                loc_mod_id = l_entry->mod_ids[i];
                break;  /* Found a local mod ID */
            }
        } /* for each unit */

        /* Even if loc_mod_id < 0, that's what we want to use */
        for (i = 0; i < l_entry->base.num_stk_ports; i++) {
            sp = &l_entry->base.stk_ports[i];
            BCM_IF_ERROR_RETURN(bcm_stk_port_get(sp->unit, sp->port, &flags));
            if (!(flags & BCM_STK_SL) &&
                l_entry->sp_info[i].flags & CPUDB_SPF_RX_RESOLVED) {
                rv_tmp = nh_tx_src_mod_port_set(sp->unit, sp->port,
                                                loc_mod_id, -1);
                if (rv_tmp < 0) {
                    LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                             (BSL_META("TOPOLOGY WARNING: "
                              "Could not set NH mod/port for (%d, %d): %s\n"),
                              sp->unit, sp->port, bcm_errmsg(rv_tmp)));
                }
            } else {
                /* Allow default NH source mod id */
                rv_tmp = nh_tx_src_mod_port_set(sp->unit, sp->port, -1, -1);
                if (rv_tmp < 0) {
                    LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                             (BSL_META("TOPOLOGY WARNING: "
                              "Could not clear NH mod/port for (%d, %d): "
                              "%s\n"),
                              sp->unit, sp->port, bcm_errmsg(rv_tmp)));
                }
            }
        }
    }

    /* Set up the tx unit/port for each other destination in the system */

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry == l_entry) {
            continue;
        }

        done = FALSE;
        /* Find the stack port to get to this CPU */
        for (i = 0; i < l_entry->base.num_stk_ports && !done; i++) {
            for (j = 0; j < topo_cpu->tp_stk_port[i].tx_mod_num && !done; j++) {
                if (entry->dest_mod == topo_cpu->tp_stk_port[i].tx_mods[j]) {
                    entry->tx_unit = l_entry->base.stk_ports[i].unit;
                    entry->tx_port = l_entry->base.stk_ports[i].port;
                    done = TRUE;
                }
            }
        }
        if (!done) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("TOPOLOGY WARNING: Did not find tx stack port for "
                      CPUDB_KEY_FMT " mod %d\n"), CPUDB_KEY_DISP(entry->base.key),
                      entry->dest_mod));
        }
        /* This amounts to ignoring the warning */
        entry->flags |= CPUDB_F_TX_KNOWN;
    }


    /* Check and clear transactions for remote CPUs with a different path */
    topo_cpu_trans_flush(db_ref);
    
    /* Set new topology information */
    bcm_board_topo_set(topo_cpu);

    /* Initialize topology stack port info */
    topo_sp_init(l_entry);

#if defined(TOPO_BRD_UPDATE_BEFORE_PROGRAM)
    /* Currently, stack port status is updated after the rest of board
       programming. The behavior before SDK-5.6.3 was to update the
       stack ports before board programming. In principle, the order
       should not matter. Define TOPO_BRD_UPDATE_BEFORE_PROGRAM to
       restore the older behavior if it is needed. */
    BCM_IF_ERROR_RETURN(_topo_stk_ports_update(db_ref, topo_cpu));
#endif

    /* Set up the board based on device detection. */
    if (soc_ndev == 1 &&
               SOC_IS_HERCULES15(0)) {
        rv = bcm_board_topo_cfm_xgs2(topo_cpu, db_ref);
    } else if (soc_ndev == 1 &&
               SOC_IS_XGS12_FABRIC(0)) {
        rv = bcm_board_topo_8h(topo_cpu, db_ref);
    } else if (soc_ndev == 1 &&
               SOC_IS_RAPTOR(0)) {
        rv = bcm_board_topo_xgs3_48g5g(topo_cpu, db_ref);
    } else if (soc_ndev == 1 &&
               SOC_IS_FB_FX_HX(0)) {
        rv = bcm_board_topo_sdk_xgs3_24g(topo_cpu, db_ref);
    } else {
        
        switch (db_ref->local_entry->base.board_id) {
        case cpudb_board_id_sdk_xgs3_24g:
            rv = bcm_board_topo_sdk_xgs3_24g(topo_cpu, db_ref);
            break;
        case cpudb_board_id_sdk_xgs3_20x:
            rv = bcm_board_topo_xgs3_20x(topo_cpu, db_ref);
            break;
        case cpudb_board_id_cfm_xgs3:
            rv = bcm_board_topo_cfm_xgs3(topo_cpu, db_ref);
            break;
        case cpudb_board_id_lm_56800_12x:
            rv = bcm_board_topo_lm_56800_12x(topo_cpu, db_ref);
            break;
        case cpudb_board_id_sdk_xgs3_48g:
            rv = bcm_board_topo_xgs3_48g(topo_cpu, db_ref);
            break;
        case cpudb_board_id_sdk_xgs3_48f:
            rv = bcm_board_topo_xgs3_48f(topo_cpu, db_ref);
            break;
        case cpudb_board_id_lm_xgs3_48g:
            rv = bcm_board_topo_lm_xgs3_48g(topo_cpu, db_ref);
            break;
        case cpudb_board_id_sdk_xgs3_48g5g:
            rv = bcm_board_topo_xgs3_48g5g(topo_cpu, db_ref);
            break;
        case cpudb_board_id_sdk_xgs3_48g2:
            rv = bcm_board_topo_xgs3_48g2(topo_cpu, db_ref);
            break;
        default:
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPOLOGY ERROR: Unknown board\n")));
            rv = BCM_E_NOT_FOUND;
            break;
        }
    }
    return rv;
}

int
topo_board_program(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu)
{
    int i, j;
    int done;
    cpudb_entry_t *entry;
    cpudb_entry_t *l_entry;
    int rv_tmp, rv = BCM_E_FAIL;
    cpudb_unit_port_t *sp;
    uint32 flags;
    int nh_vlan;
            
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Topology board programming db_ref %p. old_db %p\n"),
                 db_ref, db_ref->old_db));
               
    l_entry = db_ref->local_entry;

    /* First, handle some things for the local entry */
    l_entry->flags |= CPUDB_F_TX_KNOWN;

    /* If not using the reserved modid feature, set the NH source
       modid on each resolved stack port */
    if (!bcm_st_reserved_modid_enable_get()) {
        int loc_mod_id = -1;

        /*
         * Program NH TX source module to a local mod ID;
         * First, find a local mod ID, if present
         */
        for (i = 0; i < l_entry->base.num_units; i++) {
            if (l_entry->mod_ids[i] >= 0) {
                loc_mod_id = l_entry->mod_ids[i];
                break;  /* Found a local mod ID */
            }
        } /* for each unit */

        /* Even if loc_mod_id < 0, that's what we want to use */
        for (i = 0; i < l_entry->base.num_stk_ports; i++) {
            sp = &l_entry->base.stk_ports[i];
            BCM_IF_ERROR_RETURN(bcm_stk_port_get(sp->unit, sp->port, &flags));
            if (!(flags & BCM_STK_SL) &&
                l_entry->sp_info[i].flags & CPUDB_SPF_RX_RESOLVED) {
                rv_tmp = nh_tx_src_mod_port_set(sp->unit, sp->port,
                                                loc_mod_id, -1);
                if (rv_tmp < 0) {
                    LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                             (BSL_META("TOPOLOGY WARNING: "
                              "Could not set NH mod/port for (%d, %d): %s\n"),
                              sp->unit, sp->port, bcm_errmsg(rv_tmp)));
                }
            } else {
                /* Allow default NH source mod id */
                rv_tmp = nh_tx_src_mod_port_set(sp->unit, sp->port, -1, -1);
                if (rv_tmp < 0) {
                    LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                             (BSL_META("TOPOLOGY WARNING: "
                              "Could not clear NH mod/port for (%d, %d): "
                              "%s\n"),
                              sp->unit, sp->port, bcm_errmsg(rv_tmp)));
                }
            }
        }
    }

    /* Set up the tx unit/port for each other destination in the system */

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry == l_entry) {
            continue;
        }

        done = FALSE;
        /* Find the stack port to get to this CPU */
        for (i = 0; i < l_entry->base.num_stk_ports && !done; i++) {
            for (j = 0; j < topo_cpu->tp_stk_port[i].tx_mod_num && !done; j++) {
                if (entry->dest_mod == topo_cpu->tp_stk_port[i].tx_mods[j]) {
                    entry->tx_unit = l_entry->base.stk_ports[i].unit;
                    entry->tx_port = l_entry->base.stk_ports[i].port;
                    done = TRUE;
                }
            }
        }
        if (!done) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("TOPOLOGY WARNING: Did not find tx stack port for "
                      CPUDB_KEY_FMT " mod %d\n"), CPUDB_KEY_DISP(entry->base.key),
                      entry->dest_mod));
        }
        /* This amounts to ignoring the warning */
        entry->flags |= CPUDB_F_TX_KNOWN;
    }


    /* Check and clear transactions for remote CPUs with a different path */
    topo_cpu_trans_flush(db_ref);
    
    /* Set new topology information */
    bcm_board_topo_set(topo_cpu);

    /* Initialize topology stack port info */
    topo_sp_init(l_entry);

    rv = topo_board_op(topoBoardProgram, db_ref, topo_cpu);

#if !defined(TOPO_BRD_UPDATE_BEFORE_PROGRAM)
    BCM_IF_ERROR_RETURN(_topo_stk_ports_update(db_ref, topo_cpu));
#endif
    /*
     * Re-install NH TX destination mac address since mod ID
     * for local CPU could be different after a topology run
     */
    next_hop_vlan_get(&nh_vlan);
    nh_tx_dest_install(TRUE, nh_vlan);
     
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO board prg: Board programming failed: %s\n"),
                  bcm_errmsg(rv)));
        bcm_board_topo_set(NULL);
    } else {
        rv = topo_sp_info_update(l_entry);
    }

    return rv;
}

/*
 * Set HW linkscan when this is allowed by flag '_topo_hw_linkscan'
 * and Stack Trunk Failover is supported by device.
 */
STATIC int
_topo_linkscan_mode_set(cpudb_entry_t *entry)
{
    int  i;
    int  unit;
    int  port;
    int  trunk_failover;
    
    if (!_topo_hw_linkscan) {
        return BCM_E_NONE;
    }

    /* Using CPUDB same stack port index */
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        unit = entry->base.stk_ports[i].unit;
        port = entry->base.stk_ports[i].port;

        if (BCM_SUCCESS(bcm_switch_control_get(unit,
                                               bcmSwitchFailoverStackTrunk,
                                               &trunk_failover))) {
            if (trunk_failover) {
                BCM_IF_ERROR_RETURN
                    (bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_HW));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Some devices have restrictions on what the allowable modid
 * may be, such as the modid must be even when the device requires
 * two modids. This isn't true for all such devices, so we just test
 * for that property
 */

STATIC int
_topo_set_unit_modid_preference(int unit, int num_modid, int *preference)
{
    int current_modid, new_modid;
    
    if (num_modid == 2) {

        /* If the device requires two modids and the modid is even,
           see if the modid can be odd. */
        
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &current_modid));
        if ((current_modid & 1) == 0) {
            int rv;

            new_modid = current_modid+1;
            rv = bcm_stk_my_modid_set(unit, new_modid);
            if (rv == BCM_E_BADID) {
                *preference = CPUDB_TOPO_MODID_EVEN;
                LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                            (BSL_META_U(unit,
                            "unit %d modid preference is even\n"),
                             unit));
            }
            BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, current_modid));
        }
    }
    return BCM_E_NONE;
}


/*
 * Update the CPU database module ID parameters based on actual device
 * configuration.
 */

STATIC int
_topo_set_modid_parameters(cpudb_entry_t *entry)
{
    int unit;
    int *preference, *required, num_modid;

    for ( unit=0; unit<entry->base.num_units; unit++ ) {
        required = &entry->base.mod_ids_req[unit];

        if (*required > 0) {

            preference = &entry->base.pref_mod_id[unit];
            
            if (*preference == CPUDB_TOPO_MODID_ANY) {
                
                BCM_IF_ERROR_RETURN(bcm_stk_modid_count(unit, &num_modid));

                /*
                  If the number of modids the device needs is more
                  than what is requested, then update the number of
                  required modids to what the device supports.

                  This is intended to support devices that are
                  configurable to use one or more modids.
                */
                
                if (num_modid > *required) {
                    *required = num_modid;
                }
                
                BCM_IF_ERROR_RETURN
                    (_topo_set_unit_modid_preference(unit,
                                                     num_modid, preference));
            }
        }
    }
    return BCM_E_NONE;
}

static int
_topo_board_default_initialize(cpudb_ref_t db_ref)
{
    switch (db_ref->local_entry->base.board_id) {
    case cpudb_board_id_cfm_xgs2:
    case cpudb_board_id_cfm_xgs3:
    case cpudb_board_id_lm_xgs2_6x:
    case cpudb_board_id_lm_xgs2_48g:
    case cpudb_board_id_lm_xgs3_12x:
    case cpudb_board_id_lm_xgs3_48g:
    case cpudb_board_id_lm_56800_12x:
#if defined(INCLUDE_CHASSIS)

#define CHASSIS_C2C_SRC_MODID     31    /* Src mod ID for c2c */

        db_ref->local_entry->base.flags |= CPUDB_BASE_F_CHASSIS;
        if (db_ref->local_entry->base.board_id == cpudb_board_id_cfm_xgs3) {
            db_ref->local_entry->base.flags |= CPUDB_BASE_F_CHASSIS_10SLOT;
        }
        if (CHASSIS_SM(db_ref->local_entry->base.flags)) {
            /* Set c2c src modid for for SM load balancing only */
            c2c_src_mod_set(CHASSIS_C2C_SRC_MODID);
            /* Use special modid assignment */
            topology_mod_id_assign_function(topo_chassis_modids_assign);
        }
        /* Don't touch discovery minimum TTL; set maximum TTL to 2 */
        disc_ttl_max_set(2);
        bcm_board_chassis_setup(db_ref);
#endif
        break;
    case cpudb_board_id_sdk_xgs3_48f:
    case cpudb_board_id_sdk_xgs3_48g:
    case cpudb_board_id_sdk_xgs3_24g:
    case cpudb_board_id_sdk_xgs3_20x:
    case cpudb_board_id_sdk_xgs3_48g5g:
    case cpudb_board_id_sdk_xgs3_48g2:
        disc_ttl_max_set(2);
        break;
    default:
        break;
    }

    return BCM_E_NONE;
}

int
topo_board_initialize(cpudb_ref_t db_ref)
{
    BCM_IF_ERROR_RETURN(topo_board_op(topoBoardInit, db_ref, NULL));
    
    /* Set stack ports linkscan mode */
    _topo_linkscan_mode_set(db_ref->local_entry);

    _topo_set_modid_parameters(db_ref->local_entry);

    return BCM_E_NONE;
}


/*
 * Rapid Recovery
 */

/*
 * Function:
 *     topo_sp_idx_find
 * Purpose:
 *     Looks up given stack unit/port in local topology stack information
 *     and returns database index.
 * Parameters:
 *     unit  - Unit of stack port
 *     port  - Stack port
 * Returns:
 *     >= 0  Stack port index
 *     < 0   Cannot find given stack port in local topology information
 * Note:
 *     Caller must apply LOCK.
 */

STATIC int
topo_sp_idx_find(int unit, bcm_port_t port)
{
    int i;

    for (i = 0; i < COUNTOF(topo_sp); i++) {
        if ((topo_sp[i].unit == unit) && (topo_sp[i].port == port)) {
            return i;
        }
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *     topo_sp_init
 * Purpose:
 *     Initializes topology stack port information and
 *     set HW linkscan mode for stack ports on devices with
 *     stack port trunk failover support.
 * Parameters:
 *     entry  - Local CPUDB entry
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_XXX   - Number of given stack ports is larger than local db
 */

STATIC int
topo_sp_init(cpudb_entry_t *entry)
{
    int  i;
    int  topo_sp_max = COUNTOF(topo_sp);
        
        
    TOPO_DATA_LOCK;
    for (i = 0; i < topo_sp_max; i++) {
        topo_sp[i].unit = -1;
        topo_sp[i].port = -1;
        topo_sp[i].tid  = -1;
    }
    
    if (entry->base.num_stk_ports > topo_sp_max) {
        TOPO_DATA_UNLOCK;
        return BCM_E_FAIL;
    }

    /* Using CPUDB same stack port index */
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        topo_sp[i].unit = entry->base.stk_ports[i].unit;
        topo_sp[i].port = entry->base.stk_ports[i].port;
    }

    TOPO_DATA_UNLOCK;
    return BCM_E_NONE;
}


/*
 * Function:
 *     topo_sp_info_update
 * Purpose:
 *     Updates the topology stack port database with the system's
 *     current configuration.  Currently, only trunk information
 *     is being stored.
 * Parameters:
 *     entry  - Local CPUDB entry
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_XXX   - Error obtaining system's information
 */

STATIC int
topo_sp_info_update(cpudb_entry_t *entry)
{
    int           i;
    int           unit;
    bcm_port_t    port;
    bcm_trunk_t   tid;
    bcm_module_t  modid = -1;
    int           topo_sp_max = COUNTOF(topo_sp);
    
    /* Load information from current system configuration */
    TOPO_DATA_LOCK;
    for (i = 0; i < topo_sp_max; i++) {
        unit = topo_sp[i].unit;
        port = topo_sp[i].port;
        
        if ((unit < 0) || (port < 0)) {
            continue;
        }
        
        if (BCM_SUCCESS(bcm_trunk_find(unit, modid,
                                       BCM_GPORT_DEVPORT(unit, port), &tid))) {
            topo_sp[i].tid = tid;
        } else {
            topo_sp[i].tid = -1;
        }
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META_U(unit,
                    "Stack port %d: unit %d, port %d, tid %d\n"),
                     i, unit, port, topo_sp[i].tid));
    }
    TOPO_DATA_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *     topo_sp_tx_cpu_replace
 * Purpose:
 *     Replaces the TX stack port if the failed port is used to
 *     reach a destination in the system.  The routine will:
 *         - replace port in CPU database,
 *         - update C2C database,
 *         - clear any pending transaction to destination CPU
 * Parameters:
 *     db_ref    - (IN/OUT) Current CPU database
 *     unit      - Unit of failed stack port
 *     fail_sp   - Failed stack port to replace
 *     repl_sp   - Replacing stack port
 * Returns:
 *     N/A
 * NOTE:
 *     Assumes that the failed and replacing ports are in the same unit.
 */

STATIC void
topo_sp_tx_cpu_replace(cpudb_ref_t db_ref, int unit,
                       bcm_port_t fail_sp, bcm_port_t repl_sp)
{
    cpudb_entry_t  *entry;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry == db_ref->local_entry) {
            continue;
        }
        /* Replace if failed port is being used */
        if ((entry->tx_unit == unit) && (entry->tx_port == fail_sp)) {
            entry->tx_port = repl_sp;
            topo_cpu_trans_purge(entry);
            c2c_cpu_add(entry);
            /*
             * Inform ATP that the db entry has been modified
             * so that in-flight tx pkts are handled gracefully
             */
            atp_db_update_notify();
        }
    }
    
}


/*
 * Function:
 *     topo_board_rapid_recovery
 * Purpose:
 *     Performs a rapid recovery of the local system for
 *     the following defined cases:
 *
 *     a) Trunk link failure 
 *        When the failure occurs on a stack port that is a member
 *        of a trunk.  Trunk should contain at least one more additional
 *        stack port.
 *
 *     Any other cases not defined above will return failure.
 *
 * Parameters:
 *     db_ref    - (IN/OUT) Current CPU database
 *     unit      - Unit of failed stack port
 *     fail_sp   - Failed stack port
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_XXX   - System cannot perform rapid recovery
 */

int
topo_board_rapid_recovery(cpudb_ref_t db_ref, int unit, bcm_port_t fail_sp)
{
    topo_cpu_t   *topo_cpu;    /* Current topology */
    int          fail_sp_idx;  /* Failed stack port index */
    bcm_trunk_t  tid;          /* Trunk ID of failed stack port */
    int          rv = BCM_E_UNAVAIL;

    TOPO_DATA_LOCK;

    /* Get current topology */
    bcm_board_topo_get(&topo_cpu);
    if (topo_cpu == NULL) {
        TOPO_DATA_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META_U(unit,
                  "Current topology is null\n")));
        return BCM_E_FAIL;
    } 

    
    /*
     * Trunk Link Failure
     *
     * Note:  The topology database is currently NOT being updated for now.
     *        The original information created by topology after
     *        a full discovery cycle remains unchanged.
     */
    if ((fail_sp_idx = topo_sp_idx_find(unit, fail_sp)) < 0) {
        TOPO_DATA_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META_U(unit,
                  "Stack port unit %d, port %d, not found\n"),
                   unit, fail_sp));
        return BCM_E_NOT_FOUND;
    }

    tid = TOPO_SP_TID_GET(fail_sp_idx);
    if (tid >= 0) {
        int                   i, j;
        bcm_trunk_info_t      t_info;
        bcm_port_t            repl_sp;  /* Replacing port */
        int                   member_count;
        bcm_trunk_member_t    member_array[BCM_TRUNK_FABRIC_MAX_PORTCNT];
        bcm_port_t            local_port;
        
        /* Get current trunk information and remove failed port */
        rv = bcm_trunk_get(unit, tid, &t_info, BCM_TRUNK_FABRIC_MAX_PORTCNT,
                           member_array, &member_count);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META_U(unit,
                      "Cannot get trunk information for unit %d, tid %d\n"),
                       unit, tid));
            TOPO_DATA_UNLOCK;
            return rv;
        }
        
        /* Setup new trunk information with failed port removed */
        j = 0;
        for (i = 0; i < member_count; i++) {
            BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, member_array[i].gport,
                                                   &local_port));
            if (local_port == fail_sp) {
                continue;
            }
            sal_memcpy(&member_array[j], &member_array[i],
                       sizeof(bcm_trunk_member_t));
            j++;
        }
        member_count = j;
        
        /* Trunk failover requires at least one port left in trunk */
        if (member_count < 1) {
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META_U(unit,
                        "Link failure on unit %d, port %d, is not a "
                         "trunk failover case\n"), unit, fail_sp));
            TOPO_DATA_UNLOCK;
            return BCM_E_FAIL;
        }

        /* Get first active port of trunk to replace failed port */
        BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, member_array[0].gport,
                                               &repl_sp));

        /* Board programming */
        rv = bcm_board_topo_trunk_failover(topo_cpu, unit,
                                           fail_sp, fail_sp_idx,
                                           tid, &t_info, member_count,
                                           member_array);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META_U(unit,
                      "Fail to reconfigure system for trunk failover\n")));
            TOPO_DATA_UNLOCK;
            return BCM_E_FAIL;
        }

        /* Update CPU transport */
        topo_sp_tx_cpu_replace(db_ref, unit, fail_sp, repl_sp);

        /* Update internal information */
        TOPO_SP_TID_SET(fail_sp_idx, -1);
    } 

    TOPO_DATA_UNLOCK;

    return rv;
}

/* Registration interface */
int
topo_board_default_board_program(topo_board_control_t op,
                                 cpudb_ref_t db_ref,
                                 topo_cpu_t *topo_cpu,
                                 void *user_data)
{
    int rv = BCM_E_PARAM;

    switch (op) {
    case topoBoardInit:
        rv = _topo_board_default_initialize(db_ref);
        break;

    case topoBoardProgram:
        rv = _topo_board_default_program(db_ref, topo_cpu);
        break;

    default:
        break;
        
    }

    return rv;
}



static topo_board_callback_t _topo_board_handler;
static void *_topo_board_user_data;

int
topo_board_register(topo_board_callback_t func, void *user_data)
{
    _topo_board_handler = func;
    _topo_board_user_data = user_data;

    return BCM_E_NONE;
}

int
topo_board_unregister(topo_board_callback_t func, void *user_data)
{
    int rv = BCM_E_NOT_FOUND;

    if (_topo_board_handler == func && _topo_board_user_data == user_data) {
        _topo_board_handler = NULL;
        _topo_board_user_data = NULL;
        rv = BCM_E_NONE;
    }

    return rv;
}

int
topo_board_op(topo_board_control_t op,
              cpudb_ref_t db_ref, topo_cpu_t *topo_cpu)
{
    return _topo_board_handler ?
        _topo_board_handler(op, db_ref, topo_cpu, _topo_board_user_data)
        : BCM_E_UNAVAIL;
}
