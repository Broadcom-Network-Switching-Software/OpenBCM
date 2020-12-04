/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_common.c
 * Purpose:     Common XGS board programming functions
 */
#include <shared/bsl.h>
#include <shared/alloc.h>

#include <sdk_config.h>
#include <sal/core/sync.h>

#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/init.h>               /* bcm_info_get */
#include <bcm/trunk.h>
#include <bcm/topo.h>
#include <bcm/mirror.h>
#include <bcm/switch.h>
#include <bcm_int/esw/mirror.h>

#include <appl/cputrans/nh_tx.h>    /* nh_tx_src_get */
#include <appl/cputrans/cpu2cpu.h>    /* nh_tx_src_get */
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/stktask.h>       /* bcm_st_reserved_modid_enable_get */

#include "topo_int.h"

#if defined(BCM_BOARD_AUTO_TRUNK)
#define AUTO_TRUNK_DEFAULT TRUE
#else
#define AUTO_TRUNK_DEFAULT FALSE
#endif

typedef struct _trunk_info_s {
    int num_ports;
    bcm_trunk_member_t member[BCM_TRUNK_FABRIC_MAX_PORTCNT];
    bcm_trunk_info_t info;
} _trunk_info_t;

static int _bcm_board_auto_trunk_enable = AUTO_TRUNK_DEFAULT;

#ifndef BCM_BOARD_MAX_MODID
#define BCM_BOARD_MAX_MODID   32
#endif

static int _bcm_board_num_modids = BCM_BOARD_MAX_MODID;

/*
 * Function:
 *     bcm_board_auto_trunk_set
 * Purpose:
 *     Sets the auto trunk state for this CPU subsystem
 * Parameters:
 *     flag - FALSE=no automatic trunking !FALSE=automatic trunking
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */

int
bcm_board_auto_trunk_set(int flag)
{
    _bcm_board_auto_trunk_enable = flag;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_board_auto_trunk_get
 * Purpose:
 *     Gets the current auto trunk state for this CPU subsystem
 * Parameters:
 *     flag - pointer to state variable
 * Returns:
 *     BCM_E_NONE
 * Notes:
 *      See bcm_board_auto_trunk_set for the meaning of the flag variable
 */

int
bcm_board_auto_trunk_get(int *flag)
{
    if (flag) {
        *flag = _bcm_board_auto_trunk_enable;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_board_num_modid_set
 * Purpose:
 *     Gets the maximum number of modids for this CPU subsystem
 * Parameters:
 *     num - maximum number of modids
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */

int
bcm_board_num_modid_set(int num)
{
    _bcm_board_num_modids = num;

    return BCM_E_NONE;
}

/*
 * Function: bcm_board_num_modid_get
 *     
 * Purpose:
 *     Gets the maximum number of modids for this CPU subsystem
 * Parameters:
 *     num - pointer to variable
 * Returns:
 *     BCM_E_NONE
 */

int
bcm_board_num_modid_get(int *num)
{
    if (num) {
        *num = _bcm_board_num_modids;
    }

    return BCM_E_NONE;
}

STATIC bcm_board_trunk_cb_f bcm_board_trunk_callback_func;
STATIC void *               bcm_board_trunk_callback_cookie;

int bcm_board_trunk_callback_add(bcm_board_trunk_cb_f func,
                                 void *cookie)
{
    bcm_board_trunk_callback_func   = func;
    bcm_board_trunk_callback_cookie = cookie;

    return BCM_E_NONE;
}

int bcm_board_trunk_callback_remove(bcm_board_trunk_cb_f func,
                                    void *cookie)
{
    bcm_board_trunk_callback_func   = NULL;
    bcm_board_trunk_callback_cookie = NULL;

    return BCM_E_NONE;
}

#define TRUNK_STATUS_FREE 1
#define TRUNK_STATUS_BUSY 2

STATIC int
_to_devport(int unit, bcm_gport_t gport)
{
    return (BCM_GPORT_IS_DEVPORT(gport)) ?
            gport : BCM_GPORT_DEVPORT(unit, gport);
}

STATIC int
_bcm_board_port_match(int unit, bcm_trunk_member_t *a, bcm_trunk_member_t *b)
{
    bcm_gport_t ga, gb;

    ga = _to_devport(unit, a->gport);
    gb = _to_devport(unit, b->gport);

    if (ga != gb) {
        return FALSE;
    }

    if (a->flags != b->flags) {
        return FALSE;
    }

    return TRUE;
                     
}
STATIC int
_bcm_board_trunk_match(int unit,
                       _trunk_info_t *a, _trunk_info_t *b)
{
    int i;

    if (a->num_ports != b->num_ports) {
        return FALSE;
    }

    for (i=0; i<a->num_ports; i++) {
        if (!_bcm_board_port_match(unit, a->member + i, b->member + i)) {
            return FALSE;
        }
    }

    return TRUE;
}

STATIC int
_bcm_board_trunk_make(int unit, _trunk_info_t *trunk,
                      int start_tid, int *status, int trunk_maxnum)
{
    int tid_offset;
    int tid_available;
    int tid;
    int rv;
    _trunk_info_t current;

    tid_available = -1;
    for (tid_offset = 0; tid_offset < trunk_maxnum; tid_offset++) {
        if (status[tid_offset] == 0) {
            tid = start_tid + tid_offset;
            rv = bcm_trunk_get(unit, tid, &current.info,
                               COUNTOF(current.member), current.member,
                               &current.num_ports);

            if (BCM_FAILURE(rv)) {
                if (rv == BCM_E_NOT_FOUND) {
                    if (tid_available < 0) {
                        tid_available = tid_offset;
                        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                                    (BSL_META_U(unit,
                                    "Available trunk %d\n"),
                                     tid));
                    } else {
                        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                                    (BSL_META_U(unit,
                                    "Free trunk %d\n"),
                                     tid));
                    }
                    status[tid_offset] = TRUNK_STATUS_FREE;
                } else if (rv == BCM_E_PARAM) {
                    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                                (BSL_META_U(unit,
                                "Illegal trunk %d\n"),
                                 tid));
                    break;
                } else {
                    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                                (BSL_META_U(unit,
                                "Trunk %d error %d\n"),
                                 tid, rv));
                    return rv;
                }
            } else if (_bcm_board_trunk_match(unit, trunk, &current)) {
                /* have a trunk that matches requirements */
                status[tid_offset] = TRUNK_STATUS_BUSY;
                LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                            (BSL_META_U(unit,
                            "Reusing trunk %d\n"),
                             tid));
                return BCM_E_NONE;
            }
        } else if (status[tid_offset] == TRUNK_STATUS_FREE &&
                   tid_available < 0) {
            tid_available = tid_offset;
        }
    }

    if (tid_available < 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META_U(unit,
                    "No free trunk was found\n")));
        return BCM_E_UNAVAIL;
    }
    tid = start_tid + tid_available;
    BCM_IF_ERROR_RETURN
        (bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid));
    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit, tid, &trunk->info,
                                      trunk->num_ports,
                                      trunk->member));
    status[tid_available] = TRUNK_STATUS_BUSY;
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META_U(unit,
                "Created trunk %d\n"),
                 tid));
    return BCM_E_NONE;
}

STATIC int
_bcm_board_trunk_remove(int unit, 
                        int start_tid, int *status, int trunk_maxnum)
{
    int tid_offset;
    int tid;
    int rv;

    for (tid_offset = 0; tid_offset < trunk_maxnum; tid_offset++) {
        if (status[tid_offset] == 0) {
            tid = start_tid + tid_offset;
            rv = bcm_trunk_destroy(unit, tid);
            if (BCM_FAILURE(rv) && (BCM_E_NOT_FOUND != rv)) {
                break;
            }
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META_U(unit,
                        "Destroyed trunk %d\n"),
                         tid));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 * _bcm_board_trunk_interconnect_mark
 *
 * Purpose:
 * Iterate through all the existing fabric trunks and find out the
 * one's which are interconnect trunks. For these trunks mark the
 * status as 'TRUNK_STATUS_BUSY' in the given status array.
 *
 * Parameters:
 *     unit   - device of fabric unit
 *     entry  - local board's cpudb entry
 *     start  - starting tid
 *     status  - array to maintain trunk status
 *     trunk_maxnum - max number of fabric trunks
 * Returns:
 *      BCM_E_NONE
 */
STATIC int
_bcm_board_trunk_interconnect_mark(int unit, cpudb_entry_t *entry,
                                   int start, int *status, int trunk_maxnum)
{
    int             rv;
    int             tid_offset;
    int             member_prt;
    int             cpudb_prt;
    _trunk_info_t   trunk;

    /*
     * Check the status of the interconnect/internal trunk and if exists then
     * do not destroy the interconnects while setting up External Stack Links
     */
    for (tid_offset = 0; tid_offset < trunk_maxnum; tid_offset++) {
        if (status[tid_offset] == 0) {
            rv = bcm_trunk_get(unit, (start + tid_offset), &trunk.info,
                               COUNTOF(trunk.member), trunk.member,
                               &trunk.num_ports);
            if (BCM_SUCCESS(rv)) {

                /*
                 * Check whether this trunk is an interconnect trunk by
                 * comparing the IC trunk ports with the external stacking ports
                 */
                for (member_prt = 0, cpudb_prt = 0;
                    member_prt < trunk.num_ports; member_prt++) {

                    /* Iterate through the ports in CPUDB */
                    for (; cpudb_prt < entry->base.num_stk_ports; cpudb_prt++) {
                        /* Skip if port unit is not the one being processed */
                        if (entry->base.stk_ports[cpudb_prt].unit != unit) {
                            continue;
                        }

                        /*
                         * Here stk_ports are external stacking ports only, They
                         * do not  include interconnect  stacking links.  If the
                         * member of this trunk matches with any stacking ports,
                         * then mark the current trunk as non-interconnect.
                         */
                        if (trunk.member[member_prt].gport
                             == BCM_GPORT_DEVPORT(unit,
                                    entry->base.stk_ports[cpudb_prt].port)) {
                            break;
                        }
                    }

                    /*
                     * If the trunk port is external stack port then
                     * do not continue with other ports of the trunk.
                     */
                    if (cpudb_prt < entry->base.num_stk_ports) break;
                }

                /* If it is interconnect trunk, then update the status */
                if ((member_prt >= trunk.num_ports)
                    && (cpudb_prt >= entry->base.num_stk_ports)) {
                    status[tid_offset] = TRUNK_STATUS_BUSY;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_board_trunk_unit
 * Purpose:
 *     Program fabric trunks for external stack ports (unit specific).
 *     External stack ports for the specified unit are trunked if
 *     the following conditions are met:
 *     - TX CPU keys are the same, and
 *     - If stack-port information for the remote TX CPU entry is valid,
 *       check that the remote stack-port units are the same.
 *
 *     This routine can be used to create/destroy trunks that are 
 *     symmetric as well as asymmetric.
 *
 *     Symmetric trunks are defined when stack links on a unit are
 *     connected to the same remote unit, or, if different remote units,
 *     these must reside in different CPUs.
 *
 *     Asymmetric trunks are defined when stack links on a unit are
 *     connected to two different remote units that reside on the
 *     same CPU.  To set the trunks correctly, the CPU database must
 *     contain valid stack-port information for remote CPU entries.
 *
 *     If stack-port information for remote CPU entries is not
 *     available, perform old behavior.
 *
 *     If a trunk create callback is set, the the callback will be
 *     called with the constructed trunk parameters; the application
 *     must then call bcm_trunk_set(). This allows the application
 *     to affect trunk setup (before trunk creation) and also
 *     perform trunk post processing.
 *
 *     If there is no callback, then this function will call
 *     bcm_trunk_set() to construct the trunk.
 *
 * Parameters:
 *     unit   - device of fabric unit
 *     entry  - local board's cpudb entry
 *     db_ref - database of current configuration
 *     start  - starting tid
 *     flags  - control flags
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     Some early discovery versions do not carry stack-port information
 *     for remote CPU entries.
 */


STATIC int
_bcm_board_trunk_unit(int unit, cpudb_entry_t *entry, cpudb_ref_t db_ref,
                      int start, int flags)
{
    int                     tid, rv = BCM_E_NONE, i, j;
    int                     idx;
    _trunk_info_t           trunk;
    cpudb_entry_t           *tx_entry;
    cpudb_key_t             tx_key;   /* TX key, unit on master port to .. */
    int                     tx_unit;  /* .. compare against */
    cpudb_key_t             pkey;     /* TX key, unit on other ports being ..*/
    int                     punit;    /* .. compare to master port */
    bcm_gport_t             devport;
    SHR_BITDCLNAME(pseen, CPUDB_CXN_MAX);
    int                     *trunk_status;
    bcm_trunk_chip_info_t   ti;
    int                     trunk_maxnum;

    /* Destroy any pre-existing fabric trunks */
    if (flags & BCM_BOARD_TRUNK_DESTROY) {
        for (tid = start;; tid++) {
            rv = bcm_trunk_destroy(unit, tid);
            if (BCM_FAILURE(rv) && (BCM_E_NOT_FOUND != rv)) {
                break;
            }
        }
        if (tid != start) {
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META_U(unit,
                        "TOPO: destroyed %d trunks\n"),
                         tid-start));
        }
        return BCM_E_NONE;
    }


    BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(unit, &ti));
    trunk_maxnum = (ti.trunk_fabric_id_max - ti.trunk_fabric_id_min + 1);
    trunk_status = sal_alloc(trunk_maxnum * sizeof(int), "trunk_status_ptr");
    sal_memset(trunk_status, 0, trunk_maxnum * sizeof(int));
    
    sal_memset(pseen, 0, SHR_BITALLOCSIZE(CPUDB_CXN_MAX));
    sal_memset(&trunk, 0, sizeof(trunk));
    trunk.info.psc = -1; /* Default hash algorithm */
    trunk.info.dlf_index = -1;
    trunk.info.mc_index = -1;
    trunk.info.ipmc_index = -1;
    tid = start;

    /*
     * For each external stack port in entry (this is
     * the master port to compare against
     */
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        /* Skip if stack port was already analyzed */
        if (SHR_BITGET(pseen, i)) {
            continue;
        }

        /* Set flag to indicate stack port is being analyzed */
        SHR_BITSET(pseen, i);

        /* Skip if TX is not known */
        if (!(entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED)) {
            continue;
        }

        /* Skip if stack-port unit is not the one being processed */
        if (entry->base.stk_ports[i].unit != unit) {
            continue;
        }

        /*
         * Get the remote TX CPU key and unit for the master port
         */
        CPUDB_KEY_COPY(tx_key, entry->sp_info[i].tx_cpu_key);
        tx_entry = NULL;
        tx_unit = -1;
        if (db_ref) {
            CPUDB_KEY_SEARCH(db_ref, tx_key, tx_entry);
            if (tx_entry) {
                idx = entry->sp_info[i].tx_stk_idx;
                if ((idx >= 0) && (idx < CPUDB_CXN_MAX)) {
                    tx_unit = tx_entry->base.stk_ports[idx].unit;
                }
            }
        }

        /*
         * Trunk only stack ports with same TX CPU key and
         * same remote stack-port unit.
         *
         * Note: If stack-port information for remote CPU entries
         *       is not available, then check for CPU key only
         *       (old behavior).
         */
        trunk.num_ports = 0;
        devport = BCM_GPORT_DEVPORT(unit, entry->base.stk_ports[i].port);
        bcm_trunk_member_t_init(&trunk.member[trunk.num_ports]);
        trunk.member[trunk.num_ports].gport = devport;
        trunk.num_ports++;
        for (j = i+1; j < entry->base.num_stk_ports; j++) {
            /*
             * Skip if: stack port was already analyzed, or
             *          TX is not known
             *          stack-port unit is not the one being processed
             */
            if (SHR_BITGET(pseen, j)) {
                continue;
            }
            if (!(entry->sp_info[j].flags & CPUDB_SPF_TX_RESOLVED)) {
                continue;
            }
            if (entry->base.stk_ports[j].unit != unit) {
                continue;
            }

            /*
             * Get other stack port TX CPU key and unit to
             * compare against master port
             */

            if ((flags & BCM_BOARD_TRUNK_ALL) == 0) {
                CPUDB_KEY_COPY(pkey, entry->sp_info[j].tx_cpu_key);
                punit = -1;
                if (tx_unit >= 0) {
                    idx = entry->sp_info[j].tx_stk_idx;
                    if ((idx >= 0) && (idx < CPUDB_CXN_MAX) && tx_entry) {
                        punit = tx_entry->base.stk_ports[idx].unit;
                    }
                }

                /* Skip port if TX CPU key or unit are different */
                if (!CPUDB_KEY_EQUAL(tx_key, pkey) || (tx_unit != punit)) {
                    continue;
                }

            }

            /* Mark stack port as analyzed, add to trunk group */
            SHR_BITSET(pseen, j);
            devport = BCM_GPORT_DEVPORT(unit, entry->base.stk_ports[j].port);
            bcm_trunk_member_t_init(trunk.member + trunk.num_ports);
            trunk.member[trunk.num_ports].gport = devport;
            trunk.num_ports++;
        }

        if (trunk.num_ports > 1) {
            if (bcm_board_trunk_callback_func) {
                rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
                if (BCM_FAILURE(rv) && (BCM_E_EXISTS != rv)) {
                    goto cleanup;
                }
                rv = bcm_board_trunk_callback_func(
                        unit, tid, &trunk.info, trunk.num_ports, 
                        trunk.member, bcm_board_trunk_callback_cookie);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                trunk_status[tid - start] = TRUNK_STATUS_BUSY;
            } else {
                rv = _bcm_board_trunk_make(
                        unit, &trunk, start, trunk_status, trunk_maxnum);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            }
            tid++;
        }
    }

    _bcm_board_trunk_interconnect_mark(unit, entry, start,
                                        trunk_status, trunk_maxnum);

    /* destroy any left over fabric trunks */
    rv = _bcm_board_trunk_remove(unit, start, trunk_status, trunk_maxnum);
cleanup:
    sal_free(trunk_status);

    return rv;
}

/*
 * Function:
 *     bcm_board_trunk_system
 * Purpose:
 *     Program fabric trunks for stack ports using system information
 *     provided by CPU database.  This routine can be used
 *     to create/destroy trunks that are symmetric as well as asymmetric.
 *
 *     Symmetric trunks are defined when stack links on a unit are
 *     connected to the same remote unit, or, if different remote units,
 *     these must reside in different CPUs.
 *
 *     Asymmetric trunks are defined when stack links on a unit are
 *     connected to two different remote units that reside on the
 *     same CPU.  To set the trunks correctly, the CPU database must
 *     contain valid stack-port information for remote CPU entries.
 *
 * Parameters:
 *     entry  - local board's cpudb entry
 *     db_ref - database of current configuration
 *     flags  - trunk flags
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     Some early discovery versions do not carry stack-port information
 *     for remote CPU entries.
 */
int
bcm_board_trunk_system(cpudb_entry_t *entry, cpudb_ref_t db_ref, int flags)
{
    int                    unit;
    bcm_trunk_chip_info_t  ti;
    int                    auto_trunk;

    
    BCM_IF_ERROR_RETURN(bcm_board_auto_trunk_get(&auto_trunk));
    if (auto_trunk) {
        /* scan for board fabric units */
        for (unit = 0; unit < entry->base.num_units; unit++) {
            BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(unit, &ti));
            if (ti.trunk_fabric_id_min >= 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_board_trunk_unit(unit, entry, db_ref, 
                                           ti.trunk_fabric_id_min, 
                                           flags));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_board_trunk_local
 * Purpose:
 *     Program fabric trunks for stack ports using local information.
 *     When creating trunks, this routine should be used for
 *     symmetric trunks only (see bcm_board_trunk_system for more
 *     information).
 *
 * Parameters:
 *     entry  - local board's cpudb entry
 *     flags  - trunk flags
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_board_trunk_local(cpudb_entry_t *entry, int flags)
{
    return bcm_board_trunk_system(entry, NULL, flags);
}

/*
 * Function:
 *     bcm_board_trunk_destroy
 * Purpose:
 *     Destroy all fabric trunks on board
 *
 * Parameters:
 *     entry  - local board's cpudb entry
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_board_trunk_destroy(cpudb_entry_t *entry)
{
    return bcm_board_trunk_system(entry, NULL, BCM_BOARD_TRUNK_DESTROY);
}

/*
 * Function:
 *    bcm_board__unknown_src_modid_setup
 * Purpose:
 *    Iterate over local stack ports and set NH src mod ID appropriately
 * Parameters:
 *    entry  - local board's cpudb entry
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_board_unknown_src_modid_setup(cpudb_entry_t *entry)
{
    int i;
    bcm_port_t port;
    int unit, modid;

    nh_tx_unknown_modid_get(&modid);
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        unit = entry->base.stk_ports[i].unit;
        port = entry->base.stk_ports[i].port;

        if (!(entry->sp_info[i].flags & CPUDB_SPF_RX_RESOLVED) &&
            !(entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED) &&
            !(entry->sp_info[i].flags & CPUDB_SPF_ETHERNET)) {
            BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit,
                         port, modid, TRUE));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_board_module_filter
 * Purpose:
 *     Program fabric source module filtering to prevent
 *     fabric loops.  Also programs egress port masking.
 * Parameters:
 *     unit   - fabric device
 *     tp_cpu - topology programming structure
 *     db_ref - database of current configuration
 *     config - port configuration for fabric device
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Must be called once per board fabric device.
 *
 *     Some early discovery versions do not carry stack-port information
 *     for remote CPU entries.  This is needed for cases where stack
 *     links on a unit are connected to two different remote units 
 *     that reside on the same CPU and are interconnected through
 *     their internal stack links.
 *
 *     If stack-port information for remote CPU entries is not
 *     available, perform old behavior.
 *
 *     Also, verify that device has the corresponding support
 *     for SOURCE MODID BLOCKING.
 */


int
bcm_board_module_filter(int unit,
                        topo_cpu_t *tp_cpu,
                        cpudb_ref_t db_ref,
                        bcm_port_config_t *config)
{
    cpudb_entry_t    *entry;
    int              i, j, m, rv, enable;
    bcm_port_t       port, eport;
    bcm_module_t     mod;
    bcm_pbmp_t       intports, extports, disports, enaports, ethports;
    topo_stk_port_t  *tsp;
    int              nhmod = -1, nhport;
    int              idx;
    cpudb_entry_t    *rx_entry;
    cpudb_key_t      rx_key;   /* RX key, unit on master port to .. */
    int              rx_unit;  /* .. compare against */
    cpudb_key_t      pkey;     /* RX key, unit on other ports being ..*/
    int              punit;    /* .. compare to master port */
    int              num_modids;
    SHR_BITDCLNAME(rxmod, BCM_BOARD_MAX_MODID);

    entry = &tp_cpu->local_entry;
    BCM_PBMP_ASSIGN(intports, config->stack_ext);
    BCM_PBMP_CLEAR(extports);
    BCM_PBMP_CLEAR(disports);
    BCM_PBMP_CLEAR(ethports);

    BCM_IF_ERROR_RETURN(bcm_board_num_modid_get(&num_modids));
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        if (entry->base.stk_ports[i].unit != unit) {
            continue;
        }
        port = entry->base.stk_ports[i].port;
        
        if ((entry->sp_info[i].flags & CPUDB_SPF_ETHERNET)) {
            BCM_PBMP_PORT_ADD(ethports, port);
            continue;
        }

        /* If we're using a reserved NH modid, or there's no
           nhmod defined for this stack port, or there was a nhmod
           for this stack port but it was was less than zero, then get the
           default */

        if (bcm_st_reserved_modid_enable_get() ||
            nh_tx_src_mod_port_get(unit, port, &nhmod, &nhport) < 0 ||
            nhmod < 0) {
            if (nh_tx_src_get(&nhmod, &nhport) < 0 || nhmod < 0) {
                LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                            (BSL_META_U(unit,
                            "TOPO: Could not get default NH src modid.\n")));
            }
        }
        
        sal_memset(rxmod, 0, SHR_BITALLOCSIZE(BCM_BOARD_MAX_MODID));
        if (nhmod >= 0 && nhmod < num_modids) {
            SHR_BITSET(rxmod, nhmod);    /* add nexthop module */
        }

        /*
         * Get the remote RX CPU key and unit for the master port
         */
        CPUDB_KEY_COPY(rx_key, entry->sp_info[i].rx_cpu_key);
        rx_entry = NULL;
        rx_unit = -1;
        if (db_ref) {
            CPUDB_KEY_SEARCH(db_ref, rx_key, rx_entry);
            if (rx_entry) {
                idx = entry->sp_info[i].rx_stk_idx;
                if ((idx >= 0) && (idx < CPUDB_CXN_MAX)) {
                    rx_unit = rx_entry->base.stk_ports[idx].unit;
                }
            }
        }

        /*
         * Allow only mod IDs of those stack ports with same RX CPU key
         * and same remote stack-port unit.
         *
         * Note: If stack-port information for remote CPU entries
         *       is not available, then check for CPU key only
         *       (old behavior).
         */
        for (j = 0; j < entry->base.num_stk_ports; j++) {
            if (entry->base.stk_ports[j].unit != unit) {
                continue;
            }
            if (!(entry->sp_info[j].flags & CPUDB_SPF_RX_RESOLVED)) {
                continue;
            }

            /*
             * Get other stack port RX CPU key and unit to
             * compare against master port
             */
            CPUDB_KEY_COPY(pkey, entry->sp_info[j].rx_cpu_key);
            punit = -1;
            if (rx_unit >= 0) {
                idx = entry->sp_info[j].rx_stk_idx;
                if ((idx >= 0) && (idx < CPUDB_CXN_MAX) && rx_entry) {
                    punit = rx_entry->base.stk_ports[idx].unit;
                }
            }

            /* Skip port if RX CPU key or unit are different */
            if (!CPUDB_KEY_EQUAL(rx_key, pkey) || (rx_unit != punit)) {
                continue;
            }

            /* add modules that are rxable on this stack port */
            tsp = &tp_cpu->tp_stk_port[j];
            for (m = 0; m < tsp->rx_mod_num; m++) {
                SHR_BITSET(rxmod, tsp->rx_mods[m]);
            }
        }
        
        for (mod = 0; mod < BCM_BOARD_MAX_MODID; mod++) {
            enable = SHR_BITGET(rxmod, mod) ? TRUE : FALSE;
            rv = bcm_port_modid_enable_set(unit, port, mod, enable);
            if (rv < 0) {
                break;
            }
        }
        BCM_PBMP_PORT_REMOVE(intports, port);
        if (entry->sp_info[i].flags &
            (CPUDB_SPF_TX_RESOLVED|CPUDB_SPF_RX_RESOLVED)) {
            BCM_PBMP_PORT_ADD(extports, port);
        } else {
            BCM_PBMP_PORT_ADD(disports, port);
        }
    }
    
    /*
     * HG ports are now partitioned into three bitmaps:
     * extports - external stack ports that have connectivity
     * disports - external stack ports with no connectivity
     * intports - internal ports to other switches on the board
     *
     * enaports is a bitmap of all enabled ports (intports | extports)
     */

    BCM_PBMP_ASSIGN(enaports, intports);
    BCM_PBMP_OR(enaports, extports);

    /*
     * Egress port masking works in two classes:
     * - disabled ingress ports (disports) cannot send to any egress port
     *   (other than the cpu)
     * - enabled ingress ports (enaports) can send to any other enabled
     *   port
     */
    
    BCM_PBMP_ITER(disports, port) {
        BCM_PBMP_ITER(config->stack_ext, eport) {
            rv = bcm_port_flood_block_set(unit, port, eport, -1U);
            if (rv < 0) {
                break;
            }
        }
    }

    BCM_PBMP_ITER(enaports, port) {
        BCM_PBMP_ITER(config->stack_ext, eport) {
            int flag = BCM_PBMP_MEMBER(enaports, eport) ? 0 : -1;
            rv = bcm_port_flood_block_set(unit, port, eport, flag);
            if (rv < 0) {
                break;
            }
        }
    }

    /*
     * internal and Ethernet ports allow any module ids
     */
    enable = TRUE;
    BCM_PBMP_OR(intports, ethports);
    BCM_PBMP_ITER(intports, port) {
        rv = bcm_port_modid_enable_set(unit, port, -1, enable);
    }

    return BCM_E_NONE;
}


/*
 * Add stack ports according to configuration;
 * Adds internal ports assuming:
 *    Single fabric (funit >= 0)
 *    Each device attached to the fabric has an IPIC port specified.
 *    unitport is indexed by unit number and gives port on fabric
 */

int
bcm_board_internal_stk_port_add(int unit, int port)
{
    uint32 flags;

    BCM_IF_ERROR_RETURN(bcm_stk_port_get(unit, port, &flags));
    if (!(flags & BCM_STK_ENABLE)) {
        BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, port,
                                             BCM_BOARD_INT_STK_FLAGS));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_board_xgs_stk_port_add(int funit, topo_cpu_t *tp_cpu, int *unitport)
{
    int unit, port;
    cpudb_entry_t *l_entry;
    uint32 flags;

    l_entry = &tp_cpu->local_entry;

    /* First, make sure internal ports have been added */
    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        port = unitport[unit];
        if (port < 0) { /* Skip device (eg fabric module) */
            continue;
        }
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(funit, port));

        if (bcm_stk_port_get(unit, BCM_STK_USE_HG_IF, &flags) < 0) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META_U(unit,
                     "Could not get IPIC stack info for unit %d\n"),
                      unit));
            continue;
        }
        if (!(flags & BCM_STK_ENABLE)) {
            BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, BCM_STK_USE_HG_IF,
                                                 BCM_BOARD_INT_STK_FLAGS));
        }
    }

    return BCM_E_NONE;
}


/* Program stack port mod IDs */
int
bcm_board_fab_mod_map(int funit, topo_cpu_t *tp_cpu, cpudb_entry_t *l_entry)
{
    int i, m;
    topo_stk_port_t *tsp;
    bcm_port_t port;
    bcm_module_t mod;

    /* set up module routing for fabric ports */
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(funit, mod, port));
        }
    }

    return BCM_E_NONE;
}

/*
 * Common code for XGS boards with
 * a single fabric device (5670, 5671) and
 * 0 or more switch devices (5690, 5673, etc)
 */

STATIC int
_bcm_board_xgs_connection_add(int funit, 
                              bcm_board_port_connection_t *connection,
                              int num_connections)
{
    int unit, fport, sport;
    uint32 flags;

    /* First, make sure internal ports have been added */
    for (unit = 0; unit < num_connections; unit++) {
        fport = connection[unit].to;
        if (fport < 0) { /* Skip device (eg fabric module) */
            continue;
        }
        BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(funit, fport));

        sport = connection[unit].from;
        if (bcm_stk_port_get(unit, sport, &flags) < 0) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META_U(unit,
                     "Could not get stack info for (%d,%d)\n"),
                      unit, sport));
            continue;
        }
        if (!(flags & BCM_STK_ENABLE)) {
            BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, sport,
                                                 BCM_BOARD_INT_STK_FLAGS));
        }
    }

    return BCM_E_NONE;
}


int
bcm_board_xgs_local_switch(int funit,
                           topo_cpu_t *tp_cpu,
                           bcm_port_config_t *config,
                           bcm_board_port_connection_t *connection)
{
    int                i, m, unit;
    cpudb_entry_t      *l_entry;
    bcm_port_t         hport, port;
    bcm_pbmp_t         pbm, empty;
    bcm_module_t       mod;

    l_entry = &tp_cpu->local_entry;

    BCM_IF_ERROR_RETURN(
        _bcm_board_xgs_connection_add(funit,
                                      connection,
                                      l_entry->base.num_units));

    /* program fabric for how to get to local units */
    BCM_PBMP_CLEAR(empty);
    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        port = connection[unit].to;
        if (port < 0) {
            continue;
        }
        mod = l_entry->mod_ids[unit];
        if (mod < 0) {
            continue;
        }
        m = l_entry->base.mod_ids_req[unit];
        BCM_PBMP_PORT_SET(pbm, port);
        BCM_PBMP_ITER(config->hg, hport) {
            for (i = 0; i < m; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_stk_ucbitmap_set(funit, hport, mod+i,
                                          hport == port ? empty : pbm));
            }
        }
    }

    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        if (unit == funit) {    /* skip fabric */
            continue;
        }
        mod = l_entry->mod_ids[unit];
        if (mod >= 0) {
            BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, mod));
            BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(unit));
        }
    }

    return BCM_E_NONE;
}


int
bcm_board_xgs_local_fabric(int funit,
                           topo_cpu_t *tp_cpu,
                           bcm_port_config_t *config)
{
    int                i, m;
    topo_stk_port_t    *tsp;
    cpudb_entry_t      *l_entry;
    bcm_module_t       mod;
    bcm_info_t         info;
    bcm_port_t         port, hport;
    bcm_pbmp_t         pbm;

    /*
     * Module programming for fabric
     * (usually only occurs if board is fabric only)
     */
    l_entry = &tp_cpu->local_entry;
    mod = l_entry->mod_ids[funit];
    if (mod >= 0) {
        BCM_IF_ERROR_RETURN(bcm_info_get(funit, &info));
        if (info.capability & BCM_INFO_FABRIC) {
            BCM_PBMP_ITER(config->hg, hport) {
                BCM_IF_ERROR_RETURN
                    (bcm_stk_ucbitmap_set(funit, hport, mod, config->cpu));
            }
        }
    }

    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        tsp = &tp_cpu->tp_stk_port[i];
        port = l_entry->base.stk_ports[i].port;

        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];

            BCM_PBMP_ITER(config->hg, hport) {
                BCM_PBMP_CLEAR(pbm);
                if (hport != port) {
                    BCM_PBMP_PORT_ADD(pbm, port);
                }
#ifdef BCM_ESW_SUPPORT
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_mirror_stk_update(funit, mod, hport, pbm));
#endif
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Common code for XGS boards with
 * a single fabric device (5670, 5671) and
 * 0 or more switch devices (5690, 5673, etc)
 */



int
bcm_board_xgs_common(int funit, topo_cpu_t *tp_cpu, cpudb_ref_t db_ref,
                     int *unitport)
{
    int                i, m, unit;
    topo_stk_port_t    *tsp;
    bcm_pbmp_t         pbm, empty;
    cpudb_entry_t      *l_entry;
    bcm_port_t         port, hport;
    bcm_module_t       mod;
    bcm_port_config_t  config;
    bcm_info_t         info;

    /* Notify BCM layer of internal stack ports */
    BCM_IF_ERROR_RETURN(_bcm_board_xgs_stk_port_add(funit, tp_cpu, unitport));

    l_entry = &tp_cpu->local_entry;

    BCM_IF_ERROR_RETURN(bcm_port_config_get(funit, &config));

    /* program fabric for how to get to local units */
    BCM_PBMP_CLEAR(empty);
    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        port = unitport[unit];
        if (port < 0) {
            continue;
        }
        mod = l_entry->mod_ids[unit];
        if (mod < 0) {
            continue;
        }
        m = l_entry->base.mod_ids_req[unit];
        BCM_PBMP_PORT_SET(pbm, port);
        BCM_PBMP_ITER(config.hg, hport) {
            for (i = 0; i < m; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_stk_ucbitmap_set(funit, hport, mod+i,
                                          hport == port ? empty : pbm));
            }
        }
    }
    
    unit = funit;  /* board fabric unit */

    /*
     * Module programming for fabric
     * (usually only occurs if board is fabric only)
     */
    mod = l_entry->mod_ids[unit];
    if (mod >= 0) {
        BCM_IF_ERROR_RETURN(bcm_info_get(unit, &info));
        if (info.capability & BCM_INFO_FABRIC) {
            BCM_PBMP_ITER(config.hg, hport) {
                BCM_IF_ERROR_RETURN
                    (bcm_stk_ucbitmap_set(unit, hport, mod, config.cpu));
            }
        }
    }

    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
         tsp = &tp_cpu->tp_stk_port[i];
         port = l_entry->base.stk_ports[i].port;

         for (m = 0; m < tsp->tx_mod_num; m++) {
              mod = tsp->tx_mods[m];

              BCM_PBMP_ITER(config.hg, hport) {
                 BCM_PBMP_CLEAR(pbm);
                 if (hport != port) {
                     BCM_PBMP_PORT_ADD(pbm, port);
                 }
#ifdef BCM_ESW_SUPPORT
                 BCM_IF_ERROR_RETURN
                    (_bcm_esw_mirror_stk_update(unit, mod, hport, pbm));
#endif
              }
         }
    }

    BCM_IF_ERROR_RETURN(bcm_board_fab_mod_map(funit, tp_cpu, l_entry));
    BCM_IF_ERROR_RETURN(bcm_board_module_filter(unit, tp_cpu, db_ref,
                                                &config));
    BCM_IF_ERROR_RETURN(bcm_board_trunk_system(l_entry, db_ref, TRUE));

    /*
     * Transient state setup: program  dflt modid if ports
     * are not resolved
     */
    bcm_board_unknown_src_modid_setup(l_entry);

    /*
     * Module programming for switches
     */
    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        if (unit == funit) {    /* skip fabric */
            continue;
        }
        mod = l_entry->mod_ids[unit];
        if (mod >= 0) {
            BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, mod));
            BCM_IF_ERROR_RETURN(bcm_stk_modport_clear_all(unit));
        }
    }

#ifdef BCM_BOARD_AUTO_E2E
    /* intra-board end to end flow control, if available */
    BCM_IF_ERROR_RETURN(bcm_board_e2e_set());
#endif  /* BCM_BOARD_AUTO_E2E */

    return BCM_E_NONE;
}

/*
 * Find output stack port on src_unit to get to a remote modid
 */
int
bcm_board_topomap_stk(int src_unit, int dest_modid, bcm_port_t *exit_port)
{
    cpudb_unit_port_t *stk_port;
    topo_cpu_t *tp_cpu;
    int sp_idx, m_idx;
    int mod_id;

    /* Remote module ID; look up in CPU database */
    bcm_board_topo_get(&tp_cpu);
    if (tp_cpu == NULL) {
        return BCM_E_INIT;
    }

    /* Search topo info for the stack port with the given mod id */
    TOPO_FOREACH_STK_PORT(stk_port, tp_cpu, sp_idx) {
        TOPO_FOREACH_TX_MODID(mod_id, tp_cpu, sp_idx, m_idx) {
            if (dest_modid == mod_id && stk_port->unit == src_unit) {
                *exit_port = stk_port->port;
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

int
bcm_board_untrunkable_stack_ports(int unit, topo_cpu_t *tp_cpu)
{
    int                 i, punit;
    bcm_port_t          port;
    cpudb_entry_t       *l_entry;

    l_entry = &tp_cpu->local_entry;
    for (i=0; i < l_entry->base.num_stk_ports; i++) {
        punit = l_entry->base.stk_ports[i].unit;
        port = l_entry->base.stk_ports[i].port;

        if (unit != punit) {
            continue;
        }

        if (l_entry->sp_info[i].flags & CPUDB_SPF_CUT_PORT) {
            BCM_IF_ERROR_RETURN(bcm_port_modid_enable_set(unit,
                                                          port,
                                                          -1,
                                                          0)); 
        }
    }

    return BCM_E_NONE;
}

/*
 * Rapid Recovery
 */

/*
 * Function:
 *     bcm_board_topo_trunk_failover
 * Purpose:
 *     Re-programs the local board to recover from a trunk link failure
 *     case.
 * Parameters:
 *     topo_cpu     - Local topology
 *     unit         - Unit of failed stack port
 *     fail_sp      - Failed stack port
 *     faiL_sp_idx  - Failed stack port index
 *     tid          - Trunk id of failed stack port
 *     trunk        - New trunk information; should NOT contain the failed
 *                    stack port
 *     member_count - Number of trunk members in member_array
 *     member_array - Array of trunk members
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_XXX   - System cannot perform rapid recovery
 *
 * Note:
 *     Caller MUST provide new trunk information in argument.
 */
int
bcm_board_topo_trunk_failover(topo_cpu_t *topo_cpu,
                              int unit, bcm_port_t fail_sp, int fail_sp_idx,
                              bcm_trunk_t tid, bcm_trunk_info_t *trunk,
                              int member_count,
                              bcm_trunk_member_t *member_array)
{
    int              i, rv;
    bcm_module_t     mod;
    bcm_port_t       port;
    bcm_port_t       repl_sp;        /* Port to replace failed port */
    topo_stk_port_t  *fail_topo_sp;  /* Topo info for failed port */
    int              mirr_dir;       /* True if mode is directed mirroring */

    
    /* Get first active port of trunk to replace failed port */
    if (member_count <= 0) {    /* Trunk should contain at least 1 port */
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN
        (bcm_port_local_get(unit, member_array[0].gport, &repl_sp));
    
    /*
     * Destination-module port table
     */
    fail_topo_sp = &topo_cpu->tp_stk_port[fail_sp_idx];

    for (i = 0; i < fail_topo_sp->tx_mod_num; i++) {
        mod = fail_topo_sp->tx_mods[i];
        BCM_IF_ERROR_RETURN(bcm_stk_modport_get(unit, mod, &port));
        if (port == fail_sp) {
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(unit, mod, repl_sp));
        }
    }
    
    /*
     * Fabric trunk
     *
     * Note: The trunk is kept with 1 additional port member left
     */
    BCM_IF_ERROR_RETURN
        (bcm_trunk_set(unit, tid, trunk, member_count, member_array));

    /*
     * Mirror
     */
    rv = bcm_switch_control_get(unit, bcmSwitchDirectedMirroring, &mirr_dir);
    if (BCM_FAILURE(rv)) {
        mirr_dir = 0;
    }
    /* Non-directed mirroring or Draco compat mode requires update */
    if (!mirr_dir) {
        bcm_port_config_t  config;
        bcm_port_t         hport;
        bcm_pbmp_t         mtp_pbmp;
        
        /* MTP bitmap on HG ports */
        BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
        BCM_PBMP_ITER(config.hg, hport) {
            BCM_PBMP_CLEAR(mtp_pbmp);
            rv = bcm_mirror_to_pbmp_get(unit, hport, &mtp_pbmp);
            if (rv == BCM_E_UNAVAIL) {
                continue;
            }
            BCM_IF_ERROR_RETURN(rv);
            
            /* Update if failed port is part of the mirror mtp pbmp */
            if (BCM_PBMP_MEMBER(mtp_pbmp, fail_sp)) {
                BCM_PBMP_PORT_REMOVE(mtp_pbmp, fail_sp);
                BCM_PBMP_PORT_ADD(mtp_pbmp, repl_sp);
                BCM_IF_ERROR_RETURN(bcm_mirror_to_pbmp_set(unit, hport,
                                                           mtp_pbmp));
            }
        }

        /* MTP port */
        BCM_IF_ERROR_RETURN(bcm_mirror_to_get(unit, &port));
        if (port == fail_sp) {
            if (BCM_FAILURE(bcm_mirror_to_set(unit, repl_sp))) {
                LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                          (BSL_META_U(unit,
                          "Cannot set MTP on unit %d, port %d\n"),
                           unit, repl_sp));
                return BCM_E_FAIL;
            }
        }
    }
    
    return BCM_E_NONE;
}
