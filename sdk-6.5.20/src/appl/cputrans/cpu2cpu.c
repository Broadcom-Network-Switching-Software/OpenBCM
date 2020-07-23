/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cpu2cpu.c
 * Purpose:     Provides CPU to CPU packet transmit
 * Requires:    CPU keys for destination; cputrans for pkt alloc
 * Notes:
 *     Applications must directly register with some RX to receive packets.
 *     They may then use c2c_pkt_recognize to determine if a packet
 *     is a cpu-to-cpu packet.
 *
 *     Since RX is not a service of CPU-to-CPU, loopback is not
 *     supported at this level.
 *
 *     CPU indexing is managed by a local cpudb.  That module must be properly
 *     initialized before using cpu2cpu transport including calls to
 *     add CPU keys and other info to the local CPU.
 *
 *     This module uses the tx packet allocation routines implemented in
 *     cputrans.
 */

#include <shared/bsl.h>

#include <assert.h>

#include <shared/idents.h>

#include <sal/core/sync.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <appl/cpudb/cpudb.h>

#include <appl/cputrans/cpu2cpu.h>
#include <appl/cputrans/cputrans.h>

#include "t_util.h"

/****************************************************************
 *
 * Configurable parameters
 */
static bcm_trans_ptr_t *c2c_trans_ptr   = C2C_TRANS_PTR_DEFAULT;
static uint16           c2c_snap_type   = C2C_SNAP_TYPE_DEFAULT;
static uint16           c2c_trans_type  = C2C_TRANS_TYPE_DEFAULT;
static bcm_mac_t        c2c_snap_mac    = C2C_SNAP_MAC_DEFAULT;
static uint32           c2c_flags;
static int              c2c_src_modid = C2C_SRC_MODID_DEFAULT;

#define C2C_LOCAL_MAC (c2c_db_ref->local_entry->base.mac)

static sal_mutex_t c2c_mutex;
static int init_done;

static cpudb_ref_t c2c_db_ref = CPUDB_REF_NULL;

#define C2C_INIT if (!init_done) BCM_IF_ERROR_RETURN(c2c_init())

#define C2C_LOCK sal_mutex_take(c2c_mutex, sal_sem_FOREVER)
#define C2C_UNLOCK sal_mutex_give(c2c_mutex)

#define PKT_TRANS_PTR(pkt) ((bcm_trans_ptr_t *)((pkt)->trans_ptr))

typedef struct {
    c2c_cb_f callback;
} c2c_cb_cookie_t;

STATIC void *
_c2c_callback_cookie_set(c2c_cb_f callback)
{
    c2c_cb_cookie_t *cookie = sal_alloc(sizeof(*cookie), "c2c_cookie");

    if (cookie) {
        cookie->callback = callback;
    }

    return (void *)cookie;
}

STATIC void
_c2c_callback_cookie_get(void *cookie, c2c_cb_f *cb)
{
    if (cookie) {
        c2c_cb_cookie_t *c2c_cookie = (c2c_cb_cookie_t *)cookie;
        *cb = c2c_cookie->callback;
        sal_free(c2c_cookie);
    } else {
        *cb = NULL;
    }
}

int
c2c_src_mod_set(int src_mod)
{
    c2c_src_modid = src_mod;

    return BCM_E_NONE;
}

int
c2c_src_mod_get(int *src_mod)
{
    *src_mod = c2c_src_modid;

    return BCM_E_NONE;
}

static INLINE int
c2c_src_mod(void)
{
    return (c2c_src_modid < 0) ?
        c2c_db_ref->local_entry->dest_mod : c2c_src_modid;
}

STATIC int
c2c_init(void)
{
    if (c2c_mutex == NULL) {
        c2c_mutex = sal_mutex_create("C2C_mutex");
        if (c2c_mutex == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (c2c_db_ref == CPUDB_REF_NULL) {
        c2c_db_ref = cpudb_create();
        if (c2c_db_ref == CPUDB_REF_NULL) {
            return BCM_E_MEMORY;
        }
    }

    init_done = TRUE;

    return BCM_E_NONE;
}

/* Assumes lock held */
STATIC int
_c2c_cpu_add(const cpudb_entry_t *db_ent)
{
    cpudb_entry_t *ent;

    LOG_INFO(BSL_LS_TKS_C2C,
             (BSL_META("C2C: add key " CPUDB_KEY_FMT ", flags 0x%x (%d,%d,%d,%d)\n"),
              CPUDB_KEY_DISP(db_ent->base.key), db_ent->flags,
              db_ent->tx_unit,db_ent->tx_port,
              db_ent->dest_mod,db_ent->dest_port));

    CPUDB_KEY_SEARCH(c2c_db_ref, db_ent->base.key, ent);
    if (ent == NULL) {
        ent = cpudb_entry_create(c2c_db_ref, db_ent->base.key, FALSE);
        if (ent == NULL) {
            return BCM_E_MEMORY;
        }
    }

    sal_memcpy(&ent->base, &db_ent->base, sizeof(cpudb_base_t));
    ent->tx_unit = db_ent->tx_unit;
    ent->tx_port = db_ent->tx_port;
    ent->dest_mod = db_ent->dest_mod;
    ent->dest_port = db_ent->dest_port;
    ent->flags = db_ent->flags;
    ent->trans_ptr = db_ent->trans_ptr;

    if (ent->flags & CPUDB_F_IS_LOCAL) {
        c2c_db_ref->local_entry = ent;
    }

    return BCM_E_NONE;
}

int
c2c_cpu_add(const cpudb_entry_t *db_ent)
{
    int rv;

    C2C_INIT;
    LOG_INFO(BSL_LS_TKS_C2C,
             (BSL_META("C2C added " CPUDB_KEY_FMT_EOLN),
              CPUDB_KEY_DISP(db_ent->base.key)));

    C2C_LOCK;
    rv = _c2c_cpu_add(db_ent);
    C2C_UNLOCK;

    return rv;
}

int
c2c_cpu_remove(const cpudb_key_t key)
{
    int rv;

    C2C_INIT;
    LOG_INFO(BSL_LS_TKS_C2C,
             (BSL_META("C2C removed " CPUDB_KEY_FMT_EOLN),
              CPUDB_KEY_DISP(key)));

    C2C_LOCK;
    rv = cpudb_entry_remove(c2c_db_ref, key);
    C2C_UNLOCK;

    return rv;
}

int
c2c_cpu_clear(void)
{
    int rv;

    C2C_INIT;
    C2C_LOCK;
    rv = cpudb_clear(c2c_db_ref, FALSE);
    C2C_UNLOCK;

    return rv;
}

/* Assumes local entry does not change */
int
c2c_cpu_update(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry, *new_ref_entry;
    int rv = BCM_E_NONE;
    static cpudb_key_t del_keys[CPUDB_CPU_MAX];
    int count, i;

    LOG_INFO(BSL_LS_TKS_C2C,
             (BSL_META("C2C CPU DB %s\n"),
              (db_ref==NULL) ? "clear" : "update"));
    C2C_INIT;

    C2C_LOCK;
    if (db_ref == CPUDB_REF_NULL) { /* Clear out DB completely */
        rv = cpudb_clear(c2c_db_ref, FALSE);
    } else {
        /*
         * Remove entries from c2c db that are not in new DB;
         * Can't remove entries while looping over entries, so accumulate
         * in del_keys structure
         */
        count = 0;
        CPUDB_FOREACH_ENTRY(c2c_db_ref, entry) {
            CPUDB_KEY_SEARCH(db_ref, entry->base.key, new_ref_entry);
            if (new_ref_entry == NULL) {
                if (entry == c2c_db_ref->local_entry) {
                    LOG_INFO(BSL_LS_TKS_C2C,
                             (BSL_META("C2C: Update error, can't remove local\n")));
                    continue;
                }
                CPUDB_KEY_COPY(del_keys[count], entry->base.key);
                if (++count >= CPUDB_CPU_MAX) {
                    break;
                }
            }
        }
        for (i = 0; i < count; i++) {
            rv = cpudb_entry_remove(c2c_db_ref, del_keys[i]);
            if (rv < 0) {
                LOG_INFO(BSL_LS_TKS_C2C,
                         (BSL_META("C2C: Failed to remove entry on update\n")));
                break;
            }
        }

        /* Add entries that are in new DB */
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            rv = _c2c_cpu_add(entry);
            if (rv < 0) {
                LOG_INFO(BSL_LS_TKS_C2C,
                         (BSL_META("C2C: Failed to remove entry on update\n")));
                break;
            }
        }
    }
    C2C_UNLOCK;

    return rv;
}


/****************************************************************
 *
 * Forward declarations
 */

STATIC void c2c_send_callback(int unit, bcm_pkt_t *pkt, void *cookie);
STATIC void c2c_send_callback_nofree(int unit, bcm_pkt_t *pkt, void *cookie);

STATIC int           good_c2c_pkt(uint8 *pkt_data,
                                  cpudb_key_t *src_key,
                                  uint16 *mplx_num);
STATIC void          c2c_l2_hdr_pack(const bcm_mac_t dest_mac,
                                     const bcm_mac_t src_mac,
                                     uint8 *pkt_buf,
                                     uint16 mplx_num,
                                     uint16 vlan);


/****************************************************************
 *
 * Configuration functions
 */

/*
 * Function:
 *      c2c_config_set
 * Purpose:
 *      Set the configurable parameters for CPU to CPU config
 * Parameters:
 *      trans_ptr       - Tranport pointer structure
 *      local_mac       - Local MAC address  IGNORED
 * Returns:
 *      BCM_E_XXX
 */

int
c2c_config_set(
    bcm_trans_ptr_t *trans_ptr,
    bcm_mac_t local_mac,
    uint32 flags)
{
    if (trans_ptr->tp_tx == NULL ||
            trans_ptr->tp_setup_tx == NULL) {
        return BCM_E_PARAM;
    }

    c2c_trans_ptr = trans_ptr;

    if (local_mac != NULL) {
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c local mac set is deprecated.\n")));
    }

    c2c_flags = flags;

    return BCM_E_NONE;
}


/*
 * Function:
 *      c2c_config_get
 * Purpose:
 *      Get the configurable parameters for CPU to CPU config
 * Parameters:
 *      trans_ptr       - (OUT) Pointer to tranport pointer structure
 *      local_mac       - (OUT) Local MAC address
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
c2c_config_get(
    bcm_trans_ptr_t **trans_ptr,
    bcm_mac_t local_mac,
    uint32 *flags)
{
    C2C_LOCK;
    *trans_ptr = c2c_trans_ptr;
    if (c2c_db_ref != NULL && c2c_db_ref->local_entry != NULL) {
        sal_memcpy(local_mac, C2C_LOCAL_MAC, sizeof(bcm_mac_t));
    } else {
        sal_memset(local_mac, 0, sizeof(bcm_mac_t));
    }
    *flags = c2c_flags;
    C2C_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *      c2c_snap_set
 * Purpose:
 *      Set the SNAP mac and type ID
 * Parameters:
 *      new_mac   - MAC to use
 *      new_type  - Type ID to use; host order
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      It defaults to Broadcom, so generally not required
 */

int
c2c_snap_set(bcm_mac_t new_mac, uint16 new_type, uint16 new_trans_type)
{
    sal_memcpy(c2c_snap_mac, new_mac, sizeof(bcm_mac_t));
    c2c_snap_type = new_type;
    c2c_trans_type = new_trans_type;

    return BCM_E_NONE;
}


/*
 * Function:
 *      c2c_snap_get
 * Purpose:
 *      Get the SNAP mac and type ID
 * Parameters:
 *      snap_mac   - (OUT) MAC to use
 *      snap_type  - (OUT) Type ID to use; host order
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      It defaults to Broadcom, so generally not required
 */

int
c2c_snap_get(bcm_mac_t snap_mac, uint16 *snap_type, uint16 *trans_type)
{
    sal_memcpy(snap_mac, c2c_snap_mac, sizeof(bcm_mac_t));
    *snap_type = c2c_snap_type;
    *trans_type = c2c_trans_type;

    return BCM_E_NONE;
}



/****************************************************************
 *
 * CPU to CPU transmit routines
 *
 *    There are two choices here:
 *       c2c_tx:          Allocate a bcm packet, send it, deallocate it
 *    Or:
 *       c2c_pkt_create:  Create a c2c pkt that can be sent multiple times
 *       c2c_pkt_update:  Update pkt info according to current cpudb
 *       c2c_pkt_send:    Send a pkt created with c2c_pkt_create
 *       c2c_pkt_destroy: Deallocate a c2c packet.
 */

/*
 * Function:
 *      c2c_pkt_create
 * Purpose:
 *      Returns a bcm_pkt_t that can be sent by c2c
 * Parameters:
 *      See c2c_tx below
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The buffer should include C2C_DATA_OFFSET bytes for the
 *      packet headers.
 *
 *      This routine is meant to create a packet which is repeatedly
 *      sent without being changed.  It is not meant to allocate
 *      a packet which is updated and sent out for different
 *      reasons or to different CPUs.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

bcm_pkt_t *
c2c_pkt_create(
    cpudb_key_t dest_key,
    uint8 *pkt_buf,
    int len,
    int cos,
    int vlan,
    int seg_len,
    uint16 mplx_num,
    uint32 ct_flags,
    int *tot_segs,
    int *rvp)
{
    bcm_pkt_t *pkt, *cur_pkt;
    const cpudb_entry_t *db_ent;
    uint8 *pkt_data;
    int src_mod, src_port, dest_mod, dest_port;
    int tx_unit, tx_port;
    cpudb_key_t local_key;
    bcm_mac_t dest_mac, src_mac;
    int pkt_cos, pkt_int_prio;
    int rv;

    if (!init_done) {
        if ((rv = c2c_init()) != BCM_E_NONE) {
            LOG_INFO(BSL_LS_TKS_C2C,
                     (BSL_META("c2c pc. init failed\n")));
            *rvp = rv;
            return NULL;
        }
    }

    C2C_LOCK;
    CPUDB_KEY_SEARCH(c2c_db_ref, dest_key, db_ent);
    if (db_ent == NULL || c2c_db_ref->local_entry == NULL) {
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c: Could not find %s%s%s for "
                           CPUDB_KEY_FMT_EOLN),
                  db_ent == NULL ? "db entry" : "",
                  db_ent || c2c_db_ref->local_entry ? "" : ",",
                  c2c_db_ref->local_entry == NULL ? "local entry" : "",
                  CPUDB_KEY_DISP(dest_key)));
        *rvp = (db_ent == NULL) ? BCM_E_NOT_FOUND : BCM_E_EMPTY;
        C2C_UNLOCK;
        return NULL;
    }
#if defined(BROADCOM_DEBUG)
    if (!(db_ent->flags & CPUDB_F_TX_KNOWN)) {
        C2C_UNLOCK;
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c: Unknown transmit port (flags 0x%x) for dest key "
                           CPUDB_KEY_FMT_EOLN),
                  db_ent->flags,
                  CPUDB_KEY_DISP(dest_key)));
        *rvp = BCM_E_INTERNAL;
        return NULL;
    }
#endif /* BROADCOM_DEBUG */

    /* Get needed info from db entries */
    src_mod        = c2c_src_mod();
    src_port       = c2c_db_ref->local_entry->dest_port;
    CPUDB_KEY_COPY(local_key, c2c_db_ref->local_entry->base.key);
    dest_mod = db_ent->dest_mod;
    dest_port = db_ent->dest_port;
    tx_unit = db_ent->tx_unit;
    tx_port = db_ent->tx_port;
    sal_memcpy(&dest_mac, db_ent->base.mac, sizeof(bcm_mac_t));
    sal_memcpy(&src_mac, c2c_db_ref->local_entry->base.mac, sizeof(bcm_mac_t));
    C2C_UNLOCK;

    pkt = cputrans_tx_pkt_list_alloc(pkt_buf, len, seg_len, ct_flags,
                                     tot_segs);
    if (pkt == NULL) {
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c: Could not alloc cputrans pkt\n")));
        *rvp = BCM_E_MEMORY;
        return NULL;
    }
#if defined(BROADCOM_DEBUG)
    assert(pkt->next != pkt);
#endif   /* BROADCOM_DEBUG */

#if defined(BCM_C2C_TRANSPORT_SWITCHING)
    /* Record the current entry in packet meta-data */
    pkt->trans_ptr = (void *)(db_ent->trans_ptr);
#endif

    /* If internal priority is not provided, set value to cos */
    pkt_cos = CPUTRANS_COS_GET(cos);
    if (cos & CPUTRANS_INT_PRIO_VALID) {
        pkt_int_prio = CPUTRANS_INT_PRIO_GET(cos);
    } else {
        pkt_int_prio = pkt_cos;
    }

    /* Set up packet data in each segment */
    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {

        cur_pkt->unit           = tx_unit;
        BCM_PBMP_PORT_SET(cur_pkt->tx_pbmp, tx_port);
        cur_pkt->dest_mod       = dest_mod;
        cur_pkt->dest_port      = dest_port;

        
        cur_pkt->flags          |= (BCM_TX_SRC_MOD|BCM_TX_SRC_PORT);
        cur_pkt->src_mod        = src_mod;
        cur_pkt->src_port       = src_port;
        cur_pkt->opcode         = BCM_HG_OPCODE_UC;
        cur_pkt->cos            = pkt_cos;
        cur_pkt->prio_int       = pkt_int_prio;
        cur_pkt->flags         |= BCM_TX_PRIO_INT;

        /* L2 header is always in block 0 */
        pkt_data = cur_pkt->pkt_data[0].data;
        c2c_l2_hdr_pack(dest_mac, src_mac, pkt_data,
                        mplx_num, BCM_VLAN_CTRL(cos, 0, vlan));
        CPUDB_KEY_PACK(&pkt_data[CPUTRANS_SRC_KEY_OFS], local_key);
    }

    LOG_DEBUG(BSL_LS_TKS_C2C,
              (BSL_META("c2c: pkt from (%d.%d) to (%d.%d) "
                        "op %d cos %d int_prio %d\n"),
               pkt->src_mod, pkt->src_port, 
               pkt->dest_mod, pkt->dest_port,
               pkt->opcode, pkt_cos, pkt_int_prio));

    *rvp = BCM_E_NONE;
    return pkt;
}

/*
 * Function:
 *      c2c_pkt_update
 * Purpose:
 *      Update the routing information in a packet created by c2c_pkt_create
 * Parameters:
 *      pkt   - Returned by c2c_pkt_create
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If topology info in the system changes, this can be used
 *      to update packets to reflect that.
 */

int
c2c_pkt_update(
    bcm_pkt_t *pkt,
    cpudb_key_t dest_key)
{
    bcm_pkt_t *cur_pkt;
    const cpudb_entry_t *db_ent;
    int src_mod, src_port, dest_mod, dest_port;
    int tx_unit, tx_port;

    if (!init_done) {
        if (c2c_init() != BCM_E_NONE) {
            LOG_INFO(BSL_LS_TKS_C2C,
                     (BSL_META("c2c update. init failed\n")));
            return BCM_E_INIT;
        }
    }

    C2C_LOCK;
    CPUDB_KEY_SEARCH(c2c_db_ref, dest_key, db_ent);
#if defined(BROADCOM_DEBUG)
    if (db_ent == NULL || c2c_db_ref->local_entry == NULL) {
        C2C_UNLOCK;
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c update: key %sfound, local_entry %sNULL, dest key "
                           CPUDB_KEY_FMT_EOLN),
                  (db_ent == NULL) ? "not " : "",
                  (c2c_db_ref->local_entry) == NULL ? "" : "not ",
                  CPUDB_KEY_DISP(dest_key)));
        return BCM_E_NOT_FOUND;
    }
    if (!(db_ent->flags & CPUDB_F_TX_KNOWN)) {
        C2C_UNLOCK;
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c update: Unknown tx port (flags 0x%x) for dest key "
                           CPUDB_KEY_FMT_EOLN),
                  db_ent->flags,
                  CPUDB_KEY_DISP(dest_key)));
        return BCM_E_NOT_FOUND;
    }
#else
    if (db_ent == NULL) {
        C2C_UNLOCK;
        return BCM_E_NOT_FOUND;
    }
#endif /* BROADCOM_DEBUG */

    /* Get needed info from db entries */
    src_mod        = c2c_src_mod();
    src_port       = c2c_db_ref->local_entry->dest_port;
    dest_mod = db_ent->dest_mod;
    dest_port = db_ent->dest_port;
    tx_unit = db_ent->tx_unit;
    tx_port = db_ent->tx_port;
    C2C_UNLOCK;

    /* Set up topology packet data in each segment */
    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        cur_pkt->unit           = tx_unit;
        BCM_PBMP_PORT_SET(cur_pkt->tx_pbmp, tx_port);
        cur_pkt->dest_mod       = dest_mod;
        cur_pkt->dest_port      = dest_port;

        
        cur_pkt->flags          |= (BCM_TX_SRC_MOD|BCM_TX_SRC_PORT);
        cur_pkt->src_mod        = src_mod;
        cur_pkt->src_port       = src_port;
        /* Assume mac/key for local box haven't changed */
    }

    return BCM_E_NONE;
}

#if defined(BCM_C2C_TRANSPORT_SWITCHING)
#define SETUP_TX_RESOLVE(_pkt, _setup, _tx_list) do {                       \
        bcm_trans_ptr_t *_tp;                                               \
        _setup = c2c_trans_ptr->tp_setup_tx;                                \
        _tx_list = c2c_trans_ptr->tp_tx_list;                               \
        _tp = PKT_TRANS_PTR(_pkt);                                          \
        if (_tp != NULL) {                                                  \
            if (_tp->tp_setup_tx != NULL) {                                 \
                _setup = _tp->tp_setup_tx;                                  \
            }                                                               \
            if (_tp->tp_tx_list != NULL) {                                  \
                _tx_list = _tp->tp_tx_list;                                 \
            }                                                               \
        }                                                                   \
    } while (0)
#else
#define SETUP_TX_RESOLVE(_pkt, _setup, _tx_list) do {                       \
        _setup = c2c_trans_ptr->tp_setup_tx;                                \
        _tx_list = c2c_trans_ptr->tp_tx_list;                               \
    } while (0)
#endif  /* Transport switching */

/*
 * Function:
 *      c2c_pkt_send
 * Purpose:
 *      Send a packet prepared by c2c_pkt_create.  Will not
 *      deallocate on completion.
 * Parameters:
 *      pkt        - The packet or list of packets to send
 *      callback   - If non-NULL, send async and make callback
 *      cookie     - Passed on callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This function may be called multiple times on the same
 *      packet(s).  The header of the packet should not be altered.
 *      If callback is non-NULL, it will be made after the last packet
 *      is sent.
 */

int
c2c_pkt_send(
    bcm_pkt_t *pkt,
    c2c_cb_f callback,
    void *cookie)
{
    int rv;
    bcm_pkt_cb_f  local_callback = NULL;
    bcm_tx_pkt_setup_f setup_fn;
    bcm_tx_list_f tx_list_fn;
    c2c_cb_cookie_t *c2c_cookie = NULL;

    C2C_INIT;

    SETUP_TX_RESOLVE(pkt, setup_fn, tx_list_fn);

    if (setup_fn != NULL) {
        rv = setup_fn(pkt->unit, pkt);    /* Set up the packet */
        if (rv < 0) {  /* Problem packet configuration */
            LOG_INFO(BSL_LS_TKS_C2C,
                     (BSL_META("c2c tx: Error setting up packet\n")));
            return rv;
        }
    }

    if (callback != NULL) {    /* Async send; but don't free after call */
        local_callback = c2c_send_callback_nofree;
        pkt->_last_pkt->cookie = cookie;
        c2c_cookie = _c2c_callback_cookie_set(callback);
        if (c2c_cookie == NULL) {
            return BCM_E_MEMORY;
        }
    }

    assert(tx_list_fn != NULL);
    rv = tx_list_fn(pkt->unit, pkt, local_callback, c2c_cookie);
    return rv;
}


/*
 * Function:
 *      c2c_pkt_destroy
 * Purpose:
 *      Deallocate a packet created with c2c_pkt_create
 * Parameters:
 *      pkt     - The packet to deallocate
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

void
c2c_pkt_destroy(bcm_pkt_t *pkt)
{
    assert(pkt->next != pkt);
    cputrans_tx_pkt_list_free(pkt);
}

/*
 * Function:
 *      c2c_tx
 * Purpose:
 *      Transmit a (single) packet to a CPU given by index.
 * Parameters:
 *      dest_key    - To what CPU is this being sent
 *      pkt_buf     - DMA-able buffer to be sent
 *      len         - Total bytes in packet
 *      vlan        - VLAN id (12 bits)
 *      cos         - COS and internal priority to use
 *      seg_len     - Bytes per data segment
 *      mplx_num    - (TCP-like) multiplexing number for packet
 *      ct_flags    - Indicate if packet contains space for header:
 *      callback    - If non-null, send async and make callback
 *      cookie      - Passed to callback when complete
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The length field in the L2 header of the packet is not touched.
 *      Does not support segmentation/reassembly.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
c2c_tx(
    cpudb_key_t dest_key,
    uint8 *pkt_buf,
    int len,
    int cos,
    int vlan,
    int seg_len,
    uint16 mplx_num,
    uint32 ct_flags,
    c2c_cb_f callback,
    void *cookie)
{
    bcm_pkt_t *pkt, *cur_pkt;
    int rv;
    bcm_pkt_cb_f  local_callback = NULL;
    bcm_tx_pkt_setup_f setup_fn;
    bcm_tx_list_f tx_list_fn;
    c2c_cb_cookie_t *c2c_cookie = NULL;

    C2C_INIT;

    pkt = c2c_pkt_create(dest_key, pkt_buf, len, cos, vlan,
                         seg_len, mplx_num, ct_flags, NULL, &rv);
    if (rv != BCM_E_NONE) {
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("c2c send. can't create (%s)\n"),
                  bcm_errmsg(rv)));
        return rv;
    }

    SETUP_TX_RESOLVE(pkt, setup_fn, tx_list_fn);

    /* Set up the packet */
    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        rv = setup_fn(pkt->unit, cur_pkt);
        if (rv < 0) {  /* Problem packet configuration */
            LOG_INFO(BSL_LS_TKS_C2C,
                     (BSL_META("c2c tx: Error setting up packet\n")));
            cputrans_tx_pkt_list_free(pkt);
            return rv;
        }
    }

    /* Callback should deallocate packet when called */
    if (callback != NULL) {    /* Async send */
        local_callback = c2c_send_callback;
        pkt->_last_pkt->cookie = cookie;
        c2c_cookie = _c2c_callback_cookie_set(callback);
        if (c2c_cookie == NULL) {
            cputrans_tx_pkt_list_free(pkt);
            return BCM_E_MEMORY;
        }
    }

    rv = tx_list_fn(pkt->unit, pkt, local_callback, c2c_cookie);

    /* Free packet if sending sync, or if an error occurs */
    if (callback == NULL || rv != BCM_E_NONE) {
        cputrans_tx_pkt_list_free(pkt);
    }

    return rv;
}


/*
 * Function:
 *      c2c_pkt_recognize
 * Purpose:
 *      Check if the given packet is a CPU2CPU packet and
 *      get the index if it is.
 * Parameters:
 *      pkt_data    - The buffer to examine, from start of L2 hdr
 *      src_key     - (OUT) Source KEY
 *      mplx_num    - (OUT) Packet multiplexing value, if packet recognized
 * Returns:
 *      BCM_E_NONE if it is a CPU 2 CPU packet; sets cpu_idx.
 *      BCM_E_NOT_FOUND if not a CPU 2 CPU packet.
 * Notes:
 *      If the DB reference is non-NULL, then it is searched for the source
 *      MAC address of the packet as well.
 *
 *      If src_key is non-NULL as well, it is filled in with a pointer
 *      to the found entry.
 */

int
c2c_pkt_recognize(uint8 *pkt_data,
                  cpudb_key_t *src_key,
                  uint16 *mplx_num)
{
    int is_good;

    if (!pkt_data) {
        return 0;
    }

    C2C_INIT;

    C2C_LOCK;
    is_good = good_c2c_pkt(pkt_data, src_key, mplx_num);
    C2C_UNLOCK;

    LOG_DEBUG(BSL_LS_TKS_C2C,
              (BSL_META("c2c: pkt is %srecognized. type, %d\n"),
               is_good ? "" : "not ", mplx_num ? *mplx_num : -1));

    return is_good ? BCM_E_NONE : BCM_E_NOT_FOUND;
}


/*
 * Boolean function that returns true if the packet is recognized
 * as cpu-to-cpu transport packet.
 * If the DB reference is non-NULL, then it is searched for the source
 * MAC address of the packet as well.
 */

STATIC int
good_c2c_pkt(uint8 *pkt_data,
             cpudb_key_t *src_key,
             uint16 *mplx_num)
{
    uint16 snap_type, trans_type;

    /* Dest MAC is ours? */
    if (c2c_flags & C2C_F_DEST_MAC_CHECK_ENABLE) {
        if (c2c_db_ref != NULL && c2c_db_ref->local_entry != NULL) {
            if (sal_memcmp(pkt_data, C2C_LOCAL_MAC, sizeof(bcm_mac_t))) {
                return 0;
            }
        }
    }

    

    /* SNAP pkt? */
    if (sal_memcmp(&pkt_data[CPUTRANS_SNAP_OFS], c2c_snap_mac,
                   sizeof(bcm_mac_t))) {
        LOG_DEBUG(BSL_LS_TKS_C2C,
                  (BSL_META("c2c: Not SNAP mac\n")));
        return 0;
    }

    /* SNAP type correct? */
    UNPACK_SHORT(&pkt_data[CPUTRANS_SNAP_TYPE_OFS], snap_type);
    if (snap_type != c2c_snap_type) {
        LOG_DEBUG(BSL_LS_TKS_C2C,
                  (BSL_META("c2c: Wrong SNAP type 0x%04x\n"),
                   snap_type));
        return 0;
    }

    /* SNAP local type correct? */
    UNPACK_SHORT(&pkt_data[CPUTRANS_TR_TYPE_OFS], trans_type);
    if (trans_type != c2c_trans_type) {
        LOG_DEBUG(BSL_LS_TKS_C2C,
                  (BSL_META("c2c: Wrong trans type 0x%04x\n"),
                   trans_type));
        return 0;
    }

    /* It is a c2c packet; get application packet type and source key */
    if (src_key != NULL) {
        CPUDB_KEY_UNPACK(&pkt_data[CPUTRANS_SRC_KEY_OFS], *src_key);
    }
    if (mplx_num != NULL) {
        /* Get the local packet type from the next short in the pkt */
        UNPACK_SHORT(&pkt_data[CPUTRANS_TR_MULT_OFS], (*mplx_num));
    }

    return 1;
}

STATIC void
_do_callback(bcm_pkt_t *pkt, void *cookie)
{
    c2c_cb_f cb;
    int pkt_data_blk;
    /* Grab the callback from the packet */
    _c2c_callback_cookie_get(cookie, &cb);
    /* Check where packet data is; see cputrans for more info */
    pkt_data_blk = 1;
    if (pkt->blk_count == 1) {
        pkt_data_blk = 0;
    }
    if (cb) {
        cb(pkt->pkt_data[pkt_data_blk].data, pkt->cookie);
    }
}

/* Callback and deallocate packet after being sent.  Does not touch
 * user data linked into the packet.
 */
STATIC void
c2c_send_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    _do_callback(pkt, cookie);
    assert(pkt->next != pkt);
    cputrans_tx_pkt_list_free(pkt);
}


/* Callback and don't deallocate packet; appl must call pkt_destroy. */
STATIC void
c2c_send_callback_nofree(int unit, bcm_pkt_t *pkt, void *cookie)
{
    _do_callback(pkt, cookie);
}

/* Set up the L2 header for the CPU to CPU packet; destination is in cm */

STATIC void
c2c_l2_hdr_pack(const bcm_mac_t dest_mac,
                const bcm_mac_t src_mac,
                uint8 *pkt_buf,
                uint16 mplx_num,
                uint16 vlan)
{
    /* Set up the L2 header in the packet */
    sal_memcpy(pkt_buf, dest_mac, sizeof(bcm_mac_t));
    sal_memcpy(&pkt_buf[CPUTRANS_SMAC_OFS], src_mac, sizeof(bcm_mac_t));
    PACK_SHORT(&pkt_buf[CPUTRANS_VTAG_OFS], 0x8100);
    PACK_SHORT(&pkt_buf[CPUTRANS_VTAG_OFS + sizeof(uint16)], vlan);
    /* Skip type/len field */
    /* Following three "packs" could be combined */
    sal_memcpy(&pkt_buf[CPUTRANS_SNAP_OFS], c2c_snap_mac, sizeof(bcm_mac_t));
    PACK_SHORT(&pkt_buf[CPUTRANS_SNAP_TYPE_OFS], c2c_snap_type);
    PACK_SHORT(&pkt_buf[CPUTRANS_TR_TYPE_OFS], c2c_trans_type);
    PACK_SHORT(&pkt_buf[CPUTRANS_TR_MULT_OFS], mplx_num);
}

#if defined(BROADCOM_DEBUG)
void
c2c_dump(void)
{
    cpudb_entry_t *entry;

    if (!init_done) {
        return;
    }
    
    LOG_INFO(BSL_LS_TKS_C2C,
             (BSL_META("C2C Database\n")));

    C2C_LOCK;

    if (c2c_db_ref == CPUDB_REF_NULL) {
        LOG_INFO(BSL_LS_TKS_C2C,
                 (BSL_META("    Empty\n")));
    } else {
        CPUDB_FOREACH_ENTRY(c2c_db_ref, entry) {
            LOG_INFO(BSL_LS_TKS_C2C,
                     (BSL_META("    CPU Key " CPUDB_KEY_FMT ", flags 0x%x\n"),
                      CPUDB_KEY_DISP(entry->base.key), entry->flags));
        }
    }
    
    C2C_UNLOCK;

    return;
}
#endif  /* BROADCOM_DEBUG */
