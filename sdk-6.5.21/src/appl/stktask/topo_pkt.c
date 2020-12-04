/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topo_pkt.c
 * Purpose:     All functions related to topo packet generation and
 *              handling.
 *
 * This file contains code related to topology packet generation
 * and handling.  It provides default functions that are used by
 * stack task if customer applications do not provide alternatives.
 * In addition, it provides some building blocks that customers
 * may integrate with their own code.
 *
 * Here's how the sample topology stack task code works:
 *
 *     1.  stk task enters DISC mode:  Application should call
 *         topo_pkt_expect_set(TRUE) to indicate topo pkts
 *         should be expected.
 *
 *     2.  stk task enters TOPO mode:  Stack task calls
 *         bcm_stack_topo_update (as programmed into st_config).
 *
 *     3.  topology packet is received:
 *         If both 1 and 2 have occurred, process packet.
 *         If only 1 has occurred, bank packet and wait for (2).
 *         If only 2 has occurred, it's an error.
 *
 * If stack task exits TOPO mode, or whenever it goes into blocked
 * mode, the application should call topo_pkt_expect_set(FALSE).
 */

#include <shared/bsl.h>

#include <sdk_config.h>
#include <shared/alloc.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/stack.h>

#include <appl/cpudb/cpudb.h>

#include <appl/cputrans/cputrans.h>
#include <appl/cputrans/atp.h>
#include <appl/cputrans/nh_tx.h>

#include <appl/stktask/stktask.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_pkt.h>
#include <appl/stktask/topo_brd.h>

#include "topo_int.h"
#include "brd_chassis_int.h"

static sal_mutex_t st_topo_lock;   /* Serves as init done indication */
static volatile int topo_enabled;  /* Accept topo packets; control w/ expect */

static cpudb_ref_t topo_current_db = CPUDB_REF_NULL;
static uint8 topo_pkt_buf[TOPO_PKT_BYTES_MAX];
static int topo_pkt_len; /* Also signals pkt is present */

#define TOPO_PKT_CLEAR         topo_pkt_len = 0
#define TOPO_PKT_SET(_pkt, _len, _src_key)   do {               \
        sal_memcpy(topo_pkt_buf, (_pkt), (_len));               \
        topo_pkt_len = (_len);                                  \
    } while (0)
#define TOPO_PKT_PRESENT       (topo_pkt_len > 0)

#define ST_TOPO_LOCK sal_mutex_take(st_topo_lock, sal_mutex_FOREVER)
#define ST_TOPO_UNLOCK sal_mutex_give(st_topo_lock)

/* Process topo pkts when current DB is non NULL */
#define TOPO_ACTIVE (topo_current_db != CPUDB_REF_NULL)

#ifndef TOPO_THREAD_STACK
#define TOPO_THREAD_STACK       SAL_THREAD_STKSZ
#endif
#ifndef TOPO_THREAD_PRIO
#define TOPO_THREAD_PRIO        50
#endif


#define TOPO_DEFERRED 1

/****************************************************************
 *
 * Default topology update callback function
 */

STATIC void topo_pkt_process(void);
STATIC int topo_pkt_send(cpudb_ref_t db_ref, cpudb_entry_t *entry);
STATIC int topo_pkt_send_depth_first(cpudb_ref_t db_ref, cpudb_entry_t *entry);

int
bcm_stack_topo_update(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;
    cpudb_entry_t      *entry;

    if (st_topo_lock == NULL) { /* Not initialized */
        return BCM_E_INIT;
    }

    ST_TOPO_LOCK;
    if (!topo_enabled) { /* Not enabled */
        ST_TOPO_UNLOCK;
        return BCM_E_INIT;
    }
    if (TOPO_ACTIVE) {
        ST_TOPO_UNLOCK;
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO: Activated twice\n")));
        return BCM_E_FAIL;
    }

    topo_current_db = db_ref;

#if !defined(TOPO_PROCESSING_LOCAL) /* Send out topo info from master */
    if (db_ref->local_entry == db_ref->master_entry) { /* Local is master */
        /* Generate the topology and send out topo packets */
        if ((rv = topology_mod_ids_assign(db_ref)) < 0) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("Could not assign all mod ids: %s\n"),
                       bcm_errmsg(rv)));
        } else if ((rv = topology_create(db_ref)) < 0) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("Failed to create topology: %s\n"),
                       bcm_errmsg(rv)));
        } else if ((rv = topo_pkt_send_depth_first(db_ref, 
                                            db_ref->master_entry)) < 0) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("Failed to send topo pkt: %s\n"),
                       bcm_errmsg(rv)));
        }

        /*
         * Clear topo visited flags for all cpus
         */
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            CPUDB_TOPO_NOT_VISITED_SET(entry);
        }
    } else { /* Not master */
        if (TOPO_PKT_PRESENT) { /* Ready to process packet */
            topo_pkt_process();
        }
    }
#else
    /* To do:  Implement local topology processing */
#endif

    /* Check rv == BCM_E_NONE before doing anything else */
    ST_TOPO_UNLOCK;

    return rv;
}

STATIC int
_topo_handle_init(void)
{
    if (st_topo_lock == NULL) {
        st_topo_lock = sal_mutex_create("topo_handle");
        if (st_topo_lock == NULL) {
            return BCM_E_MEMORY;
        }
    }

    return BCM_E_NONE;
}

/****************************************************************
 *
 * Application to topology interface API functions
 *
 *     topo_pkt_handle_init:
 *         Register w/ ATP to get topo pkts.
 *     topo_pkt_expect_set(int enable)
 *         Indicate if topology packet should be expected and
 *         processed.
 *
 *     topo_stack_topo_update
 *         Normally called by stack task on transition to
 *         TOPO state.
 */

STATIC bcm_rx_t topo_pkt_handler(cpudb_key_t src_key,
                                 int client_id,
                                 bcm_pkt_t *pkt,
                                 uint8 *payload,
                                 int payload_len,
                                 void *cookie);

/*
 * (Re-)Initialize the topo packet handler subsystem.
 * Creates lock and registers with ATP with given flags
 */

int
topo_pkt_handle_init(uint32 topo_atp_flags)
{
    int rv = BCM_E_NONE;

    if (st_topo_lock == NULL) {
        rv = _topo_handle_init();
        if (rv < 0) {
            return rv;
        }
    }

    /* Handle ATP-NH registration to receive topo packets from ATP */
    atp_unregister(BCM_ST_TOPO_CLIENT_ID);
    rv = atp_register(BCM_ST_TOPO_CLIENT_ID, topo_atp_flags,
                      topo_pkt_handler, NULL, -1, -1);

    return rv;
}

/*
 * Indicate to topo processing whether to expect topo pkts.
 * This should be enabled at the transition into DISC.  It
 * should be disabled at the transition to BLOCKED.
 *
 * This will always clear the local DB pointer.  That pointer
 * is set when stack task calls with "start topo processing".
 * It must not be changed by stack task until after
 * topo_pkt_handle_enable_set(FALSE) is called.
 */

int
topo_pkt_expect_set(int enable)
{
    int rv = BCM_E_NONE;

    if (st_topo_lock == NULL) {
        return BCM_E_INIT;
    }

    ST_TOPO_LOCK;
    topo_enabled = enable;
    topo_current_db = CPUDB_REF_NULL;
    ST_TOPO_UNLOCK;

    return rv;
}


/*
 * _nh_src_modid_enable
 *
 *   Enables the mod ID that will be used by the given destination
 *   CPU when sending NH packets.
 */
STATIC int
_nh_src_modid_enable(cpudb_ref_t db_ref, cpudb_entry_t *entry)
{
    int                i;    
    int                nh_mod_id = -1;
    cpudb_entry_t      *l_entry;
    cpudb_unit_port_t  *sp;
    uint32             flags;    

    /*
     * If a reserved mod ID is used for NH,
     * nothing to do.  This reserved mod ID
     * should never be blocked.
     */
    if (bcm_st_reserved_modid_enable_get()) {
        return BCM_E_NONE;
    }

    l_entry = db_ref->local_entry;

    /* If not using the reserved modid feature,
     * enable remote mod ID since remote CPU will
     * be transmitting NH packets using its local mod ID.
     */

    /*
     * Get first mod ID in CPU entry
     */
    for (i = 0; i < entry->base.num_units; i++) {
        if (entry->mod_ids[i] >= 0) {
            nh_mod_id = entry->mod_ids[i];
            break;  /* Found a local mod ID */
        }
    }

    if (nh_mod_id < 0) {
        return BCM_E_NOT_FOUND;
    }

    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        sp = &l_entry->base.stk_ports[i];
        BCM_IF_ERROR_RETURN(bcm_stk_port_get(sp->unit, sp->port, &flags));
        if (!(flags & BCM_STK_SL) &&
            l_entry->sp_info[i].flags & CPUDB_SPF_RX_RESOLVED) {
            bcm_port_modid_enable_set(sp->unit, sp->port, nh_mod_id, TRUE);
        }
    }

    return BCM_E_NONE;
}

#if !defined(TOPO_PROCESSING_LOCAL)

STATIC int
topo_pkt_send(cpudb_ref_t db_ref, cpudb_entry_t *entry)
{
    uint8 *buf, *start_buf;
    int len = 0;
    int rv = BCM_E_NONE;

    /* Now send out the packets */
    buf = start_buf = atp_tx_data_alloc(TOPO_PKT_BYTES_MAX);
    if (buf == NULL) {
        return BCM_E_MEMORY;
    }

    buf += CPUTRANS_HEADER_BYTES;

    topo_pkt_gen(db_ref, entry, buf, TOPO_PKT_BYTES_MAX, &len);

    /*
     * Enable mod ID that remote CPU will be using for NH packets
     */
    _nh_src_modid_enable(db_ref, entry);

    /*
     * Send topo packet requesting an immediate ACK back.
     *
     * This is necessary to avoid topo ACKs to travel
     * back with a NH mod-ID (as a result of the topo packet
     * processing) that the rest of the systems are not ready
     * to accept (such as the 32-mod ID support).
     */
    rv = atp_tx(entry->base.key,
                BCM_ST_TOPO_CLIENT_ID,
                start_buf,
                CPUTRANS_HEADER_BYTES + len,
                CPUTRANS_NO_HEADER_ALLOC | CPUTRANS_IMMEDIATE_ACK,
                NULL,
                NULL);
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("topo send %d:  %s. Failed to tx pkt to "
                           CPUDB_KEY_FMT_EOLN),
                  rv, bcm_errmsg(rv),
                  CPUDB_KEY_DISP(entry->base.key)));
    }

    atp_tx_data_free(start_buf);
    return rv;
}

/*
 * Send topo pkts in depth first
 * order - to avoid src mod blocking
 */
STATIC int
topo_pkt_send_depth_first(cpudb_ref_t db_ref, cpudb_entry_t *entry)
{
    cpudb_stk_port_t *sp = NULL;
    cpudb_entry_t *other_end;
    int i, rv;

    if (CPUDB_TOPO_VISITED(entry)) {
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META("TOPO ERROR: recursing infinitely")));
        return -1;
    }

    CPUDB_TOPO_VISITED_SET(entry);

    for (i = 0; i < entry->base.num_stk_ports; i++) {
        sp = &entry->sp_info[i];

        if (entry->sp_info[i].flags & CPUDB_SPF_TX_RESOLVED) {
            CPUDB_KEY_SEARCH(db_ref, sp->tx_cpu_key, other_end);

            if (other_end != NULL) {
                if (CPUDB_TOPO_VISITED(other_end)) {
                    continue;
                }

                /* depth-first traversal */
                topo_pkt_send_depth_first(db_ref, other_end);
           }
        }
    }

#ifdef TOPO_PKT_SEND_CONFIG_IN_SET
    /* Only send a topo packet to an entry if CPUDB_F_CONFIG_IN is
       set.

       Normally, on the master, Discovery will have CONFIG_IN set for
       each slave entry that has participated in discovery.

       If #define TOPO_PKT_SEND_CONFIG_IN_SET is set and CPUDB entry flag
       CPUDB_F_CONFIG_IN is clear, then do not send a topo pkt to the
       entry.

       An application can clear CPUDB_F_CONFIG_IN before calling
       bcm_stack_topo_update() to suppress the transmission of a topo
       pkt to that entry. The entry will still be used for topology
       calculation and modid assignment.
       
       This could allow a system to change stacking software from a
       'stktask' based topo pkt protocol to something not using topo
       pkt by leaving those entries that need a topo pkt_send with
       CONFIG_IN set, and clearing CONFIG_IN from those that don't.

       The master is still responsible for ensuring that those systems
       with CONFIG_IN clear can still forward packets as needed for
       the systems with CONFIG_IN set.

    */
    if (entry != db_ref->master_entry && !entry->flags & CPUDB_F_CONFIG_IN) {
        return BCM_E_NONE;
    }
#endif

    rv = topo_pkt_send(db_ref, entry);
    return rv;
}

#ifndef TOPO_DELAY_DEFAULT
#define TOPO_DELAY_DEFAULT 500000 /* 0.5 second, for master only */
#endif


static int topo_delay_us = 0;
static int topo_master_delay_us = TOPO_DELAY_DEFAULT;

int
topo_delay_set(int delay_us)
{
    topo_delay_us = delay_us;
    return BCM_E_NONE;
}

int
topo_delay_get(int *delay_us)
{
    if (delay_us != NULL) {
        *delay_us = topo_delay_us;
    }
    return BCM_E_NONE;
}

int
topo_master_delay_set(int delay_us)
{
    topo_master_delay_us = delay_us;
    return BCM_E_NONE;
}

int
topo_master_delay_get(int *delay_us)
{
    if (delay_us != NULL) {
        *delay_us = topo_master_delay_us;
    }
    return BCM_E_NONE;
}

/*
 * Process current topo packet
 */

STATIC void
_topo_pkt_process_thread(void *cookie)
{
    int rv;
    static topo_cpu_t topo_cpu;  /* Pretty big, so keep off the stack */

    COMPILER_REFERENCE(cookie);

    ST_TOPO_LOCK;
    /* Note that a previous invocation may have cleared a packet
       between the time the lock was released after calling
       topo_pkt_process(), and aquired above, so check if packet is
       still present. */
    if (TOPO_ACTIVE && TOPO_PKT_PRESENT) {
        /* Calculate local CPU connection information; call board programming */
        sal_memset(&topo_cpu, 0, sizeof(topo_cpu));
        rv = topo_pkt_parse(topo_current_db,
                            topo_current_db->local_entry,
                            topo_pkt_buf,
                            topo_pkt_len,
                            &topo_cpu,
                            NULL);

        if (rv < 0) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO PKT ERROR: parse failure: %s\n"),
                       bcm_errmsg(rv)));
        } else if ((rv = topo_board_program(topo_current_db, &topo_cpu)) < 0) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO PKT ERROR: appl fail: %s\n"),
                       bcm_errmsg(rv)));
        }

        TOPO_PKT_CLEAR; /* Discard packet once processed */

        if (rv == BCM_E_NONE) {
            int delay;

            /* Defer topology processing allowing all devices to update */
            delay = topo_delay_us;
            if (topo_current_db->local_entry ==
                    topo_current_db->master_entry) {
                delay = topo_master_delay_us;
            }
            
            if ((topo_current_db->num_cpus > 1) && (delay > 0)) {
                LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                            (BSL_META("TOPO: Sleeping for %d us\n"),
                             delay));
                sal_usleep(delay);
            }
            /* Successfully processed topo pkt; signal stack task */
            bcm_st_event_send(BCM_STE_TOPO_SUCCESS);
        }
    }
    ST_TOPO_UNLOCK;
    sal_thread_exit(0);
}

STATIC void
topo_pkt_process(void)
{
    (void) sal_thread_create("topoDPC",
                             TOPO_THREAD_STACK,
                             TOPO_THREAD_PRIO,
                             _topo_pkt_process_thread,
                             NULL);
}
  

/*
 * This routine needs to handle the case where the local machine
 * hasn't realized it's ready for a topo pkt, but the master
 * has sent one.  This is done by queuing any topo packet received.
 */

STATIC bcm_rx_t
topo_pkt_handler(cpudb_key_t src_key,
                 int client_id,
                 bcm_pkt_t *pkt,
                 uint8 *payload,
                 int payload_len,
                 void *cookie)
{
    COMPILER_REFERENCE(client_id);
    COMPILER_REFERENCE(cookie);
    COMPILER_REFERENCE(pkt);

    if (st_topo_lock == NULL) { /* Not active */
        return BCM_RX_HANDLED;
    }

    ST_TOPO_LOCK; /* Synchronize with topo_update call */
    if (!topo_enabled) {
        ST_TOPO_UNLOCK;
        return BCM_RX_HANDLED;
    }
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("ST TOPO: pkt len %d in from "
                          CPUDB_KEY_FMT_EOLN),
                 payload_len, CPUDB_KEY_DISP(src_key)));

    /* Record this info - this may overwrite a previous packet that
       hasn't been processed yet. */
    TOPO_PKT_SET(payload, payload_len, src_key);
    if (TOPO_ACTIVE) { /* Topo is active meaning ready to process topo pkt */
        topo_pkt_process();
    }

    ST_TOPO_UNLOCK;
    return BCM_RX_HANDLED;
}

#endif  /* not topology generated locally */

#if 1
#define CHECK_PACK(_buf, _len, _cnt, _data) \
    if (_cnt + 1 > _len) return -1; else (_buf)[(_cnt)++] = _data
#else /* Debug version of the function */
#define CHECK_PACK(_buf, _len, _cnt, _data)  do { \
    LOG_INFO(BSL_LS_TKS_TOPOLOGY, \
             (BSL_META(\ \
                       (BSL_META"Packing mod %d\n")),
              _data)); \ \
    if (_cnt + 1 > _len) return -1; else (_buf)[(_cnt)++] = _data; \
} while (0)
#endif

#if defined(INCLUDE_CHASSIS)

/****************************************************************
 *
 * CHASSIS SPECIFIC CODE
 */



/*
 * The following generate the chassis topology based on
 * conventions.  This is done by overriding the generation
 * of the TX and RX module-ID lists per stack port.
 */

/* Pack mod IDs for an LM on an external LM stack port; sp_idx is active */
STATIC int
tp_chassis_lm_pack_mods(
    int cfm_count, /* How many CFMs active in the system? */
    /* The rest follow tp_pack_mods semantics */
    cpudb_ref_t db_ref, cpudb_entry_t *entry,
    uint8 *pkt_data, int len, int sp_idx, int tx)
{
    int slot_id;  /* This slot ID */
    int cnt = 0; /* byte count */
    int cfm_slot;
    int slot, mod;

    slot_id = entry->base.slot_id;
    if ((cfm_slot = cpudb_sp_idx_to_slot(db_ref, entry, sp_idx, NULL)) < 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: LM %d no cxn on %d\n"),
                     slot_id, sp_idx));
        return 0;
    }

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Packing LM slot %d, sp idx %d to slot %d, %s; %d cfms\n"),
                 slot_id, sp_idx, cfm_slot, tx ? "tx" : "rx", cfm_count));

    if (!tx) { /* RX: Expect any mod ID on any active stack port */
        CHECK_PACK(pkt_data, len, cnt, CFM_MODID(cfm_slot));
        FOREACH_OTHER_MODID(slot_id, slot, mod) {
            CHECK_PACK(pkt_data, len, cnt, mod);
        }
        return cnt;
    }

    /* TX: Pack mod IDs for transmitting to other LMs */
    CHECK_PACK(pkt_data, len, cnt, CFM_MODID(cfm_slot));
    if (cfm_count == 1) { /* Only one CFM in system */
        FOREACH_OTHER_MODID(slot_id, slot, mod) {
            CHECK_PACK(pkt_data, len, cnt, mod);
        }
    } else { /* assume cfm_count is 2; (if 0, sp is not active) */
        FOREACH_OTHER_MODID(slot_id, slot, mod) {
            if (cfm_slot % 2 == mod % 2) {
                CHECK_PACK(pkt_data, len, cnt, mod);
            }
        }
    }

    return cnt;
}

/* Pack mod IDs for a CFM on an external LM stack port; sp_idx is active */
STATIC int
tp_chassis_cfm_pack_mods(
    int cfm_count, /* How many CFMs active in the system; > 0 */
    /* The rest follow tp_pack_mods semantics */
    cpudb_ref_t db_ref, cpudb_entry_t *entry,
    uint8 *pkt_data, int len, int sp_idx, int tx)
{
    int slot_id;              /* local slot ID */
    int cnt = 0;              /* byte count */
    int lm_slot;              /* To which LM slot does sp_idx connect? */
    int mod;

    slot_id = entry->base.slot_id;
    if ((lm_slot = cpudb_sp_idx_to_slot(db_ref, entry, sp_idx, NULL)) < 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: CFM %d no cxn on %d\n"),
                     slot_id, sp_idx));
        return 0;
    }

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Packing CFM slot %d sp idx %d to slot %d, %s; %d cfms\n"),
                 slot_id, sp_idx, lm_slot, tx ? "tx" : "rx", cfm_count));

    if (!tx) { /* If one CFM or "parity" matches, expect mod id */
        /* Expect other CFMs mod ID */
        CHECK_PACK(pkt_data, len, cnt, CFM_MODID(1-slot_id));
        FOREACH_MODID(lm_slot, mod) {
            if ((cfm_count == 1) || ((slot_id % 2) == (mod % 2))) {
                CHECK_PACK(pkt_data, len, cnt, mod);
            }
        }
        return cnt;
    }

    CHECK_PACK(pkt_data, len, cnt, CFM_MODID(1-slot_id));
    FOREACH_MODID(lm_slot, mod) {    /* Send to all slot's mod ids */
        CHECK_PACK(pkt_data, len, cnt, mod);
    }

    return cnt;
}

STATIC int
tp_chassis_pack_mods(
    cpudb_ref_t db_ref, cpudb_entry_t *entry,
    uint8 *pkt_data, int len, int sp_idx, int tx)
{
    int cfm_count = 0;
    cpudb_entry_t *db_ent;

    /* If not resolved or enabled, no activity on the port */
    if ((entry->sp_info[sp_idx].flags & CPUDB_SPF_NO_LINK) ||
        (entry->sp_info[sp_idx].flags & CPUDB_SPF_INACTIVE) ||
        (entry->sp_info[sp_idx].flags & CPUDB_SPF_ETHERNET)) {
        return 0;
    }

    /* Count CFMs in DB */
    CPUDB_FOREACH_ENTRY(db_ref, db_ent) {
        if (db_ent->base.slot_id < 2) {
            ++cfm_count;
        }
    }

    if (entry->base.slot_id < 2) { /* This is a CFM card */
        return tp_chassis_cfm_pack_mods(cfm_count, db_ref, entry,
                                        pkt_data, len, sp_idx, tx);
    }
    
    return tp_chassis_lm_pack_mods(cfm_count, db_ref, entry, pkt_data, len,
                                   sp_idx, tx);
}

#else

#define tp_chassis_pack_mods(a, b, c, d, e, f) 0

#endif  /* INCLUDE_CHASSIS */


/****************************************************************
 *
 * Topology packet generation and parsing.
 */

STATIC int
tp_pack_mods(cpudb_ref_t db_ref, cpudb_entry_t *entry,
             uint8 *pkt_data, int len, int sp_idx, int tx)
{
    int i, nmod, mod;
    int entry_idx, cpu_idx;
    int cnt;
    cpudb_entry_t *cpu_entry;

#if defined(INCLUDE_CHASSIS)
    if (CHASSIS_SM(db_ref->local_entry->base.flags)) {
        return tp_chassis_pack_mods(db_ref, entry, pkt_data,
                                    len, sp_idx, tx);
    }
#endif

    entry_idx = entry->topo_idx;
    cnt = 0;
    CPUDB_FOREACH_ENTRY(db_ref, cpu_entry) {
        cpu_idx = cpu_entry->topo_idx;
        if (cpu_idx == entry_idx) {
            continue;
        }

        if (tx) {
            if (TP_TX_CXN(db_ref, entry_idx, cpu_idx) != sp_idx) {
                continue;
            }
        } else {
            if (TP_RX_CXN(db_ref, entry_idx, cpu_idx) != sp_idx) {
                continue;
            }
        }

        for (i = 0; i < cpu_entry->base.num_units; i++) {
	    mod = cpu_entry->mod_ids[i];
	    for (nmod = 0; nmod < cpu_entry->base.mod_ids_req[i]; nmod++) {
                CHECK_PACK(pkt_data, len, cnt, (mod + nmod));
	    }
        }
    }

    return cnt;
}

/*
 * Function:
 *      topo_pkt_gen
 * Purpose:
 *      Generate a topology packet for the given CPU entry
 * Parameters:
 *      db_ref              - DB to use
 *      entry               - Packet is destined for this entry's CPU
 *      pkt_data            - Packet buffer to use
 *      len                 - Bytes available in the buffer
 *      packed_bytes        - (OUT) bytes put in buffer
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The base mod ID must be set for all entries before this is called.
 *      Assumes pkt_data points to the start of the topology payload
 *      (topo version) following the CPUTRANS header.
 */

int
topo_pkt_gen(cpudb_ref_t db_ref, cpudb_entry_t *entry,
             uint8 *pkt_data, int len, int *packed_bytes)
{
    int sp_ofs;        /* Start of stack port entry */
    int rx_count_ofs;  /* Where count of rx mods goes */
    int cnt, sp_idx;
    cpudb_entry_t *db_ent;
    int i;
    uint8 *cur_ptr;
    int tot_bytes;

    if (TOPO_HEADER_BYTES(entry->base.num_units) > len) {
        return BCM_E_RESOURCE;
    }

    cur_ptr = &pkt_data[TOPO_VER_OFS];
    PACK_LONG(cur_ptr, TOPO_PKT_VERSION);
    cur_ptr += sizeof(uint32);
    PACK_LONG(cur_ptr, 0);  /* Reserved:  TOPO_RSVD_OFS */
    cur_ptr += sizeof(uint32);
    PACK_LONG(cur_ptr, db_ref->master_entry->base.dseq_num);
    cur_ptr += sizeof(uint32);
    cur_ptr += sizeof(uint32);  /* BASE_LEN_OFS: Base length is set later */
    PACK_LONG(cur_ptr, 0);   /* FLAGS  TOPO_FLAGS_OFS */
    cur_ptr += sizeof(uint32);

    /* Pack up module IDs per unit for this entry */
    for (i = 0; i < entry->base.num_units; i++) {
        PACK_LONG(cur_ptr, entry->mod_ids[i]);
        cur_ptr += sizeof(uint32);
    }

    tot_bytes = TOPO_HEADER_BYTES(entry->base.num_units);
    sp_ofs = TOPO_MOD_IDS_OFS + sizeof(uint32) * entry->base.num_units;
    /* Pack up stack info for this entry */
    for (sp_idx = 0; sp_idx < entry->base.num_stk_ports; sp_idx++) {
        pkt_data[sp_ofs] = 0; /* FLAGS */
        if (entry->sp_info[sp_idx].flags & CPUDB_SPF_CUT_PORT) {
            pkt_data[sp_ofs] |= TOPO_CUT_PORT;
        }

        cnt = tp_pack_mods(db_ref, entry, &pkt_data[sp_ofs + 2],
                           len - (sp_ofs + 2), sp_idx, TRUE);
        if (cnt < 0) {
            return BCM_E_RESOURCE;
        }
        pkt_data[sp_ofs + 1] = cnt;
        tot_bytes += cnt + 2;   /* FLAGS + num tx mods */

        rx_count_ofs = sp_ofs + cnt + 2;
        cnt = tp_pack_mods(db_ref, entry, &pkt_data[rx_count_ofs + 1],
                           len - (rx_count_ofs + 1), sp_idx, FALSE);
        if (cnt < 0) {
            return BCM_E_RESOURCE;
        }
        pkt_data[rx_count_ofs] = cnt;
        tot_bytes += cnt + 1;   /* num rx mods */

        sp_ofs = rx_count_ofs + 1 + cnt;
    }

    /* After the stack ports, add the dest mod ids, dest port per CPU */
    pkt_data[tot_bytes++] = db_ref->num_cpus;
    if (tot_bytes + (db_ref->num_cpus * (CPUDB_KEY_BYTES + 2)) > len) {
        return -1;
    }
    CPUDB_FOREACH_ENTRY(db_ref, db_ent) {
        CPUDB_KEY_PACK(&pkt_data[tot_bytes], db_ent->base.key);
        tot_bytes += CPUDB_KEY_BYTES;
        pkt_data[tot_bytes++] = db_ent->mod_ids[db_ent->base.dest_unit];
        /* Dest port may change with mod id remapping */
        pkt_data[tot_bytes++] = db_ent->base.dest_port;
    }

    if (tot_bytes + 4 > len) {
        return -1;
    }

    /* Clear app cookie; add to cur_ofs */
    pkt_data[tot_bytes++] = 0;
    pkt_data[tot_bytes++] = 0;
    pkt_data[tot_bytes++] = 0;
    pkt_data[tot_bytes++] = 0;

    /* Pack the length of the packet, from mod IDs thru ap cookie */
    PACK_LONG(&pkt_data[TOPO_BASE_LEN_OFS], tot_bytes - TOPO_FLAGS_OFS);

    if (packed_bytes != NULL) {
        *packed_bytes = tot_bytes;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      topo_pkt_parse
 * Purpose:
 *      Parse a topology packet; checks data against DB ref
 * Parameters:
 *      db_ref              - DB to use
 *      entry               - (IN/OUT) The local entry (see notes)
 *      topo_data           - Packet buffer to use
 *      len                 - Length of buffer
 *      tp_cpu              - (OUT) Where to place results
 *      ap_data_ofs         - (OUT) offset of application data relative
 *                            to topo_data
 * Returns:
 *      BCM_E_FAIL if any mismatch is detected or buffer is too short.
 * Notes:
 *      Checks against local entry.  Length of buffer is only used
 *      for checking if there's not enough data in the packet.
 *
 *      Note that the local MOD IDs are recorded (taken from the packet)
 *      into entry->mod_ids (as well as into tp_cpu->mod_ids).
 */

int
topo_pkt_parse(
    cpudb_ref_t db_ref,
    cpudb_entry_t *entry,
    uint8 *topo_data,
    int len,
    topo_cpu_t *tp_cpu,
    int *ap_data_ofs)
{
    int cur_ofs;                /* Index into topo_data */
    cpudb_base_t *l_base;       /* Local base info */
    int sp_idx;                 /* Index of stack port */
    topo_stk_port_t *tp_sp;     /* Topology stk port info */
    int i;
    cpudb_entry_t *db_ent;
    int cpu_count;
    cpudb_key_t db_key;
    int cpu_idx;
    uint8 *cur_ptr;
    int num_modids;

    if     (entry == NULL ||
            db_ref == CPUDB_REF_NULL ||
            topo_data == NULL ||
            tp_cpu == NULL) {
        return BCM_E_PARAM;
    }

    l_base = &entry->base;

    if (len < TOPO_HEADER_BYTES(db_ref->num_cpus) + 5) {
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META("TOPO PARSE ERROR: Packet too short (%d<%d)[#%d]\n"),
                   len,
                   (int)TOPO_HEADER_BYTES(db_ref->num_cpus) + 5,
                   db_ref->num_cpus));
        return BCM_E_FAIL;
    }
    cur_ptr = &topo_data[TOPO_VER_OFS];
    UNPACK_LONG(cur_ptr, tp_cpu->version);        /* TOPO_VER_OFS */
    cur_ptr += sizeof(uint32);
    cur_ptr += sizeof(uint32);   /* Reserved:  TOPO_RSVD_OFS */
    UNPACK_LONG(cur_ptr, tp_cpu->master_seq_num); /* TOPO_MSEQ_NUM_OFS */
    cur_ptr += sizeof(uint32);
    /* skip TOPO_BASE_LEN_OFS */
    cur_ptr += sizeof(uint32);
    UNPACK_LONG(cur_ptr, tp_cpu->flags);          /* TOPO_FLAGS_OFS */
    cur_ptr += sizeof(uint32);

    /* Pull the local device module IDs off of the packet */
    for (i = 0; i < entry->base.num_units; i++) {
        UNPACK_LONG(cur_ptr, entry->mod_ids[i]);
        cur_ptr += sizeof(uint32);
    }

    /* Now parse the mod id lists per local stack port */
    cur_ofs = TOPO_MOD_IDS_OFS + sizeof(uint32) * entry->base.num_units;
    for (sp_idx = 0; sp_idx < l_base->num_stk_ports; sp_idx++) {
        tp_sp = &tp_cpu->tp_stk_port[sp_idx];

        tp_sp->flags = topo_data[cur_ofs++];

        /* Check cut port flag */
        if (tp_sp->flags & TOPO_CUT_PORT) {
            entry->sp_info[sp_idx].flags |= CPUDB_SPF_CUT_PORT;
        } else {
            entry->sp_info[sp_idx].flags &= ~CPUDB_SPF_CUT_PORT;
        }

        /* Get TX modid count and check database limit */
        num_modids = topo_data[cur_ofs++];
        if (num_modids > TOPO_MODIDS_MAX) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO PARSE ERROR: TX mod id count %d exceeds DB limit "
                       "%d\n"), num_modids, TOPO_MODIDS_MAX));
            return BCM_E_FAIL;
        }
        tp_sp->tx_mod_num = num_modids;
        for (i = 0; i < tp_sp->tx_mod_num; i++) {
            tp_sp->tx_mods[i] = topo_data[cur_ofs++];
            LOG_INFO(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("ST TOPO: Map TX mod %d to stk idx %d\n"),
                      tp_sp->tx_mods[i], sp_idx));
        }

        /* Get RX modid count and check database limit */
        num_modids = topo_data[cur_ofs++];
        if (num_modids > TOPO_MODIDS_MAX) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO PARSE ERROR: RX mod id count %d exceeds DB limit "
                       "%d\n"), num_modids, TOPO_MODIDS_MAX));
            return BCM_E_FAIL;
        }
        tp_sp->rx_mod_num = num_modids;
        for (i = 0; i < tp_sp->rx_mod_num; i++) {
            tp_sp->rx_mods[i] = topo_data[cur_ofs++];
            LOG_INFO(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META("ST TOPO: Map RX mod %d to stk idx %d\n"),
                      tp_sp->rx_mods[i], sp_idx));
        }
        
        if (cur_ofs + 2 > len) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO PARSE ERROR: Packet too short at %d\n"),
                       cur_ofs));
            return BCM_E_FAIL;
        }
    }

    /* Now that mod ids are unpacked, copy local entry info to tp_cpu */
    cpudb_entry_copy(&tp_cpu->local_entry, entry);

    /* The base mod ids per CPU follow stack port info */
    cpu_count = topo_data[cur_ofs++];
    if (cur_ofs + cpu_count * (CPUDB_KEY_BYTES + 1) > len) {
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META("TOPO PARSE ERROR: Packet too short at %d\n"),
                   cur_ofs));
        return BCM_E_FAIL;
    }
    for (cpu_idx = 0; cpu_idx < cpu_count; cpu_idx++) {
        CPUDB_KEY_UNPACK(&topo_data[cur_ofs], db_key);
        cur_ofs += CPUDB_KEY_BYTES;
        CPUDB_KEY_SEARCH(db_ref, db_key, db_ent);
        if (db_ent == NULL) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO PARSE ERROR: Could not find DB key "
                                CPUDB_KEY_FMT_EOLN),
                       CPUDB_KEY_DISP(db_key)));
            return BCM_E_FAIL;
        }
        db_ent->dest_mod = topo_data[cur_ofs++];
        db_ent->dest_port = topo_data[cur_ofs++];
        db_ent->flags |= CPUDB_F_DEST_KNOWN;
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: Mapped " CPUDB_KEY_FMT " to (%d, %d)\n"),
                     CPUDB_KEY_DISP(db_key), db_ent->dest_mod,
                     db_ent->dest_port));
    }

    UNPACK_LONG(&topo_data[cur_ofs], tp_cpu->ap_cookie);
    cur_ofs += sizeof(uint32);
    if (ap_data_ofs != NULL) {
        *ap_data_ofs = cur_ofs;
    }

    return BCM_E_NONE;
}
