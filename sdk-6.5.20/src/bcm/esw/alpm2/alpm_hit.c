/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm_hit.c
 * Purpose: ALPM Distributed HITBIT (device independent implementation).
 * Requires:
 */

/* Implementation notes:
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

_alpm_dist_hitbit_t *alpm_dist_hitbit[SOC_MAX_NUM_DEVICES];

/* Find the mem Hw location in upper CB bucket (upr_bkt_node->ent_idx)
 * for a pivot in current CB
 */
int
alpm_pvt_ent_idx_get(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                     int *ent_idx)
{
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;
    _alpm_pvt_node_t *upr_pvt_node = NULL;
    _alpm_bkt_node_t *upr_bkt_node = NULL;

    if (ACB_HAS_TCAM(acb)) { /* skip top CB */
        return BCM_E_INTERNAL;
    }

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (PVT_BKT_TRIE(pvt_node) &&
        PVT_BKT_TRIE(pvt_node)->v6_key) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Key & Length (lpm_cfg.user_data will be used in _alpm_find()) */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, pvt_node->key_len, &lpm_cfg);
    sal_memcpy(lpm_cfg.user_data, pvt_node->key, sizeof(pvt_node->key));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, PVT_BKT_VRF(pvt_node));

    /* Find corresponding upper CB pivot and bucket */
    rv = alpm_cb_find(u, ACB_UPR(u, acb), &lpm_cfg,
                    &upr_pvt_node, &upr_bkt_node);

    if (BCM_SUCCESS(rv)) {
        *ent_idx = (int)upr_bkt_node->ent_idx;
    }

    return rv;
}

/*
 * Function:
 *      alpm_rte_hit_get
 * Purpose:
 *      Get ALPM route hitbit status via hitbit cache sync with Hw
 *      and gathering pivot hitbit status whose BPM is the route.
 * Parameters:
 *      u       - Device unit #.
 *      vrf_id  - VRF ID for logic hit table PID in Parallel/Mixed Mode
 *      idx     - ALPM route bucket level table index
 *      ent     - Route entry ID (0-15)
 * Returns:
 *      Route hit bit (0 or 1)
 */
int
alpm_rte_hit_get(int u, int app, int vrf_id, _alpm_tbl_t tbl, int idx, int ent)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_bkt_hit_get(u, vrf_id, ACB_VRF_BTM(u, vrf_id, app),
                                       tbl, idx, ent);
    return rv;
}

/* Get bucket entry hitbit status from cached Hit table in ent_idx */
int
alpm_bkt_hit_get(int u, int vrf_id, _alpm_cb_t *acb, _alpm_tbl_t tbl, int ent_idx)
{
    int rv = BCM_E_UNAVAIL;
    int idx, ent;

    ent = ALPM_IDX_TO_ENT(ent_idx);
    idx = ALPM_TAB_IDX_GET(ent_idx);

    rv = ALPM_DRV(u)->alpm_bkt_hit_get(u, vrf_id, acb, tbl, idx, ent);
    return rv;
}

/* Set ALPM route hitbit=1 via cached Hit table by idx and ent */
int
alpm_rte_hit_set(int u, int app, int vrf_id, _alpm_tbl_t tbl, int idx, int ent)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_bkt_hit_set(u, vrf_id, ACB_VRF_BTM(u, vrf_id, app),
                                       tbl, idx, ent, 1);
    return rv;
}

/* Set ALPM bucket entry hitbit status to cached Hit table
   in ent_idx with hit_val */
/* Not used
int
alpm_bkt_hit_set(int u, int vrf_id, _alpm_cb_t *acb,
                 _alpm_tbl_t tbl, int ent_idx, int hit_val)
{
    int rv = BCM_E_UNAVAIL;
    int idx, ent;

    ent = ALPM_IDX_TO_ENT(ent_idx);
    idx = ALPM_TAB_IDX_GET(ent_idx);

    rv = ALPM_DRV(u)->alpm_bkt_hit_set(u, vrf_id, acb, tbl, idx, ent, hit_val);

    return rv;
}
*/

/* Set ALPM bucket entry hitbit status to hitbit table cache
   and write to Hw (by setting Entry Move & Mask to trigger write to Hw
   in next Dist Hitbit thread clycle). */
int
alpm_bkt_hit_write_hw(int u, int vrf_id, _alpm_cb_t *acb,
                      _alpm_tbl_t tbl, int ent_idx, int hit_val)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_bkt_hit_write(u, vrf_id, acb, tbl, ent_idx, hit_val);
    return rv;
}

/* Get ALPM pivot hitbit status from cached Hit table */
int
alpm_pvt_hit_get(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    int         rv = BCM_E_NONE;
    int         tcam_idx, ent_idx, vrf_id;
    _alpm_tbl_t tbl = alpmTblInvalid;

    if (ACB_HAS_TCAM(acb)) { /* TCAM pre-pivot cb=0 */
        int pkm, ipt = PVT_BKT_IPT(pvt_node);
        tcam_idx = PVT_IDX(pvt_node);
        pkm = ALPM_PKM_RECOVER(u, ipt);
        rv = tcam_cache_hit_get(u, pvt_node, acb->app, pkm, tcam_idx);

    } else { /* Pivot cb=1 */
        vrf_id = PVT_BKT_VRF(pvt_node);
        /* Get upper CB bucket ent_idx for the pivot */
        ALPM_IER(alpm_pvt_ent_idx_get(u, acb, pvt_node, &ent_idx));
        rv = alpm_bkt_hit_get(u, vrf_id, ACB_UPR(u, acb), tbl, ent_idx);
    }

    return rv;
}

/* Clear ALPM pivot hitbit (Hw and cache) */
int
alpm_pvt_hit_clear(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    int         rv = BCM_E_NONE;
    int         tcam_idx, ent_idx, vrf_id;
    _alpm_tbl_t tbl = alpmTblInvalid;

    if (ACB_HAS_TCAM(acb)) { /* TCAM pre-pivot cb=0 */
        int pkm, ipt = PVT_BKT_IPT(pvt_node);
        tcam_idx = PVT_IDX(pvt_node);
        pkm = ALPM_PKM_RECOVER(u, ipt);
        rv = tcam_hit_write(u, pvt_node, acb->app, pkm, tcam_idx, 0);

    } else { /* Pivot cb=1 */
        vrf_id = PVT_BKT_VRF(pvt_node);
        /* Get upper CB bucket ent_idx for the pivot */
        ALPM_IER(alpm_pvt_ent_idx_get(u, acb, pvt_node, &ent_idx));
        rv = alpm_bkt_hit_write_hw(u, vrf_id, ACB_UPR(u, acb), tbl, ent_idx, 0);
    }

    return rv;
}

/* ALPM pivot dist hitbit gathering to BPM route */
int
alpm_pvt_hit_cb(int u, _alpm_cb_t *acb,
                _alpm_pvt_node_t *pvt_node, void *user_data)
{
    int rv = BCM_E_NONE;
    int idx, ent;
    int vrf_id = PVT_BKT_VRF(pvt_node);
    _alpm_bkt_node_t *def_node = PVT_BKT_DEF(pvt_node);
    _alpm_tbl_t tbl = alpmTblInvalid;

    if (def_node != NULL) { /* pivot BPM route exists */
        ent = ALPM_IDX_TO_ENT(def_node->ent_idx);
        idx = ALPM_TAB_IDX_GET(def_node->ent_idx);

        rv = alpm_rte_hit_get(u, acb->app, vrf_id, tbl, idx, ent);
        if (rv) { /* continue if route hit=0 */
            return rv;
        }

        rv = alpm_pvt_hit_get(u, acb, pvt_node);
        if (rv <= 0) { /* continue if pivot hit=1 */
            return rv;
        }

        /* Set route hit to 1 if BPM pivot hit=1 */
        rv = alpm_rte_hit_set(u, acb->app, vrf_id, tbl, idx, ent);
    }

    return BCM_E_NONE;
}

/* ALPM pivot hit clear if BPM = route */
int
alpm_pvt_hit_clear_cb(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, void *user_data)
{
    int rv = BCM_E_NONE;
    int *route_ent_idx = (int *)user_data;
    _alpm_bkt_node_t *def_node = PVT_BKT_DEF(pvt_node);

    if (def_node && (def_node->ent_idx == *route_ent_idx)) {
        rv = alpm_pvt_hit_clear(u, acb, pvt_node);
    }

    return rv;
}

/* Walk pivot tries and gather hitbit to BPM route */
int
alpm_hitbit_gather(int u)
{
    int         i, vrf_id, ipt;
    int         rv = BCM_E_NONE;
    _alpm_cb_t  *acb;
    int app = APP0;

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i, app);
        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            for (ipt = 0; ipt < ALPM_IPT_CNT; ipt++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, ipt)) {
                    continue;
                }
                rv = bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                                               alpm_pvt_hit_cb, NULL);
            }
        }
    }
    return rv;
}

/* Sync ALPM non-TCAM level (i:0/1 for level 2/3) hitbit cache with Hw */
int
alpm_hitbit_sync_cache(int u, int app, int i, int pid)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_hit_cache_sync(u, app, i, pid);

    return rv;
}

/* Write ALPM non-TCAM level (i:0/1 for level 2/3) hitbit cache to Hw */
int
alpm_hitbit_write_hw(int u, int app, int i, int pid)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_hit_hw_write(u, app, i, pid);

    return rv;
}

/* Update hitbit for entry move on level i by
 * hit_cache = (hit_cache & ~hit_move_mask) | (hit_move & hit_move_mask)
 */
int
alpm_hitbit_ent_move_update(int u, int i, int pid)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_hit_move_update(u, i, pid);

    return rv;
}

/* Reset hitbit ent move, move mask and moved flag
 * for next cycle of dist hitbit thread
 */
int
alpm_hitbit_ent_move_reset(int u, int i, int pid)
{
    int alloc_sz = ALPM_HTBL_SIZE(u, i, pid);

    sal_memset(ALPM_HTBL_MOVE(u, i, pid), 0, alloc_sz);
    sal_memset(ALPM_HTBL_MOVE_MASK(u, i, pid), 0, alloc_sz);

    ALPM_HTBL_ENT_MOVED(u, i, pid) = FALSE;
    ALPM_HTBL_ENT_MOVED_CNT(u, i, pid) = 0;
    return BCM_E_NONE;
}

/* Get TCAM hitbit status from a TCAM entry */
int
tcam_entry_hit_get(int u, int app, int pkm, void *e, int sub_idx)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_hit_get(u, app, pkm, e, sub_idx);

    return rv;
}

/* Get TCAM cached entry hitbit status from pivot's tcam_idx */
int
tcam_cache_hit_get(int u, _alpm_pvt_node_t *pvt_node, int app, int pkm, int tcam_idx)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_cache_hit_get(u, pvt_node, app, pkm, tcam_idx);

    return rv;
}

/* Write TCAM hitbit from pivot's tcam_idx */
int
tcam_hit_write(int u, _alpm_pvt_node_t *pvt_node,
               int app, int pkm, int tcam_idx, int hit_val)
{
    int         rv = BCM_E_UNAVAIL;

    if (ALPM_DRV(u)->tcam_hit_write) {
        rv = ALPM_DRV(u)->tcam_hit_write(u, pvt_node, app, pkm, tcam_idx, hit_val);
    }

    return rv;
}

/* Sync TCAM hitbit cache with Hw */
int
tcam_hitbit_sync_cache(int u)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_hit_cache_sync(u);

    return rv;
}

/*
 * Function:
 *      bcm_esw_alpm_clear_hit
 * Purpose:
 *      Clear ALPM route hit bit from found lpm_cfg->bkt_node->ent_idx.
 * Parameters:
 *      u       - (IN)SOC unit number.
 *      lpm_cfg - (IN)Found lpm defip_cfg info
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_alpm_clear_hit(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;

    _alpm_pvt_node_t *pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    _alpm_bkt_node_t *bkt_node = (_alpm_bkt_node_t *)lpm_cfg->bkt_node;
    _alpm_cb_t  *acb;
    int vrf_id, ipt, i;
    _alpm_tbl_t tbl;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    vrf_id = PVT_BKT_VRF(pvt_node);
    acb = ACB_VRF_BTM(u, vrf_id, app);
    tbl = BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, vrf_id));
    ipt = ALPM_LPM_IPT(u, lpm_cfg);

    if (ALPM_HIT_SKIP(u)) {
        return rv;
    }

    if (pvt_node && bkt_node) {
        /* clear route hitbit */
        rv = alpm_bkt_hit_write_hw(u, vrf_id, acb, tbl, bkt_node->ent_idx, 0);

        if (!ALPM_TCAM_HB_IS_PTR(u)) {
            /* clear also hitbit of all pivots with BPM = route (ent_idx) */
            for (i = 0; i <= acb->acb_idx; i++) {
                rv = bcm_esw_alpm_pvt_traverse(u, app, i, vrf_id, ipt,
                                               alpm_pvt_hit_clear_cb, &bkt_node->ent_idx);
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      alpm_dist_hitbit_thread
 * Purpose:
 *      ALPM distributed hitbit thread running periodically
 *      to sync hitbit cache with Hw, gathering pivot hitbit status
 *      to BPM route, and write hitbit entry move to Hw.
 * Parameters:
 *      u - Device unit #.
 * Returns:
 *      Nothing
 */
STATIC void
alpm_dist_hitbit_thread(int u)
{
    int i, pid;
    _alpm_cb_t *acb;
    sal_usecs_t t0, t1, t2, t3, t4, t5;
    int app = APP0;

    ALPM_INFO(("thread starting on unit %d\n", u));
    ALPMDH(u)->thread = sal_thread_self();

    while (ALPMDH(u)->interval != 0) {
        ALPM_HIT_LOCK(u);
        t0 = sal_time_usecs();
        if (soc_feature(u, soc_feature_distributed_hit_bits)) {
            /* Sync TCAM hitbit cache with Hw */
            tcam_hitbit_sync_cache(u);
        }

        t1 = sal_time_usecs();
        /* Sync Bucket level hitbit caches with Hw */
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {
                alpm_hitbit_sync_cache(u, app, i, pid);
            }
        }
        t2 = sal_time_usecs();
        /* Entry move hitbits handling for bucket levels.
           TCAM entry move writes hitbit in combined view
           directly to cache/Hw by soc_mem_write() */
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {
                alpm_hitbit_ent_move_update(u, i, pid);
            }
        }
        t3 = sal_time_usecs();
        if (soc_feature(u, soc_feature_distributed_hit_bits)) {
            alpm_hitbit_gather(u);
        }
        t4 = sal_time_usecs();
        /* Wtite to Hw if moved and reset ent move and move mask */
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {
                if (ALPM_HTBL_ENT_MOVED(u, i, pid)) {
                    if (ALPMC(u)->_alpm_dbg_info_bmp & _ALPM_DBG_INFO_HIT) {
                        LOG_CLI(("Writes to L%d PID %d hit table: %d (entries)\n",
                                 i+2, pid, ALPM_HTBL_ENT_MOVED_CNT(u, i, pid)));
                    }
                    t5 = sal_time_usecs();
                    alpm_hitbit_write_hw(u, app, i, pid);
                    if (ALPMC(u)->_alpm_dbg_info_bmp & _ALPM_DBG_INFO_HIT) {
                        LOG_CLI(("Hit table L%d PID %d write latency: %u (us).\n", i+2, pid,
                                 sal_time_usecs() - t5));
                    }
                }
                alpm_hitbit_ent_move_reset(u, i, pid);
            }
        }
        if (ALPMC(u)->_alpm_dbg_info_bmp & _ALPM_DBG_INFO_HIT) {
            t5 = sal_time_usecs();
            ALPM_INFO(("\nTime: %lu (sec), latency: %d %d %d %d %d (us)\n",
                       sal_time(), t1-t0, t2-t1, t3-t2, t4-t3, t5-t4));
        }
        ALPM_HIT_UNLOCK(u);
        /* wait for next cycle after interval time */
        (void)sal_sem_take(ALPMDH(u)->sema, ALPMDH(u)->interval);
    }

    ALPM_INFO(("thread exiting on unit %d\n", u));

    ALPMDH(u)->thread = NULL;
    sal_thread_exit(0);
}

/*
 * Function:
 *      alpm_dist_hitbit_enable_set
 * Purpose:
 *      Enable or disable the ALPM distributed hitbits
 *      background thread feature.
 * Parameters:
 *      u  - Device unit #.
 *      us - Specifies the software polling interval in micro-seconds;
 *           the value 0 disables Dist hitbits thread.
 * Returns:
 *      BCM_E_XXX
 */

int
alpm_dist_hitbit_enable_set(int u, int us)
{
    int                 rv = BCM_E_NONE;
    soc_timeout_t       to;
    sal_usecs_t         wait_usec;

    /* Time to wait for thread to start/end (longer for BCMSIM) */
    wait_usec = (SAL_BOOT_BCMSIM || SAL_BOOT_QUICKTURN) ? 30000000 : 10000000;

    if (ALPMDH(u) == NULL) { /* No error to disable if not inited */
        return(BCM_E_NONE);
    }

    sal_snprintf(ALPMDH(u)->taskname, sizeof(ALPMDH(u)->taskname),
                 "bcmALPMDH.%d", u);

    if (us) {
        ALPMDH(u)->interval = us;

        if (ALPMDH(u)->thread != NULL) {
            /* Dist hit thread is already running; just update the period */
            sal_sem_give(ALPMDH(u)->sema);
            return BCM_E_NONE;
        }

        if (sal_thread_create(ALPMDH(u)->taskname,
                              SAL_THREAD_STKSZ,
                              ALPMDH(u)->priority,
                              (void (*)(void*))alpm_dist_hitbit_thread,
                              INT_TO_PTR(u)) == SAL_THREAD_ERROR) {
            ALPM_ERR(("%s: Thread create error\n", ALPMDH(u)->taskname));
            ALPMDH(u)->interval = 0;
            rv = BCM_E_MEMORY;
        } else {
            soc_timeout_init(&to, wait_usec, 0);

            while (ALPMDH(u)->thread == NULL) {
                if (soc_timeout_check(&to)) {
                    ALPM_ERR(("%s: Thread did not start\n",
                              ALPMDH(u)->taskname));
                    ALPMDH(u)->interval = 0;
                    rv = BCM_E_INTERNAL;
                    break;
                }
            }

            if (BCM_SUCCESS(rv)) {
                LOG_CLI(("ALPM Distributed HITBIT thread enabled (interval %d us)\n", us));
            }
        }
    } else if (ALPMDH(u)->thread != NULL) {
        ALPMDH(u)->interval = 0;

        sal_sem_give(ALPMDH(u)->sema);
        soc_timeout_init(&to, 5 * wait_usec, 0);

        while (ALPMDH(u)->thread != NULL) {
            if (soc_timeout_check(&to)) {
                ALPM_ERR(("%s: Thread did not exit\n", ALPMDH(u)->taskname));
                rv = BCM_E_INTERNAL;
                break;
            }
        }

        if (BCM_SUCCESS(rv)) {
            LOG_CLI(("ALPM Distributed HITBIT thread disabled\n"));
        }
    }

    return rv;
}

int
alpm_dist_hitbit_interval_get(int u)
{
    if (ALPMDH(u)) {
        return ALPMDH(u)->interval;
    } else {
        return 0;
    }
}

/* Deinit ALPM distributed hitbit */
int
bcm_esw_alpm_hit_deinit(int u)
{
    int rv = BCM_E_NONE;

    if (ALPM_HIT_SKIP(u)) {
        return rv;
    }

    if (ALPMDH(u) != NULL) {
        ALPM_INFO(("ALPM distributed hitbits thread end\n"));
        rv = alpm_dist_hitbit_enable_set(u, 0);

        if (ALPMDH(u)->sema != NULL) {
            sal_sem_destroy(ALPMDH(u)->sema);
            ALPMDH(u)->sema = NULL;
        }

        ALPM_DRV(u)->alpm_hit_deinit(u);

        alpm_util_free(ALPMDH(u));
        ALPMDH(u) = NULL;
    }

    return rv;
}

/* Initialize ALPM distributed hitbit */
int
bcm_esw_alpm_hit_init(int u)
{
    int         rv = BCM_E_NONE;
    sal_usecs_t usec, def;

    if (ALPM_HIT_SKIP(u)) {
        return rv;
    }

    if (ALPMDH(u) != NULL) {
        bcm_esw_alpm_hit_deinit(u);
    }

    ALPM_ALLOC_EG(ALPMDH(u), sizeof(_alpm_dist_hitbit_t), "ALPMDH");

    ALPM_IEG(ALPM_DRV(u)->alpm_hit_init(u));

    /* Initialize ALPM dist hitbit thread */

    def = (SAL_BOOT_BCMSIM || SAL_BOOT_QUICKTURN) ? 30000000 : ALPM_HIT_INTERVAL_DEF;
    usec = soc_property_get(u, "alpm_dist_hit_interval_us", def);
    /* Set usec=0 to disable thread */
    if ((usec < ALPM_HIT_INTERVAL_MIN) && (usec > 0)) {
        usec = ALPM_HIT_INTERVAL_MIN;
    }
    if (usec > ALPM_HIT_INTERVAL_MAX) {
        usec = ALPM_HIT_INTERVAL_MAX;
    }
    ALPMDH(u)->priority = soc_property_get(u, "alpm_dist_hit_priority",
                                           ALPM_HIT_PRIORITY_DEF);
    ALPMDH(u)->sema = sal_sem_create("bcmHit_SLEEP", sal_sem_BINARY, 0);
    if (ALPMDH(u)->sema == NULL) {
        ALPM_IEG(BCM_E_MEMORY);
    }

    ALPM_IEG(alpm_dist_hitbit_enable_set(u, usec));

    return rv;

bad:
    bcm_esw_alpm_hit_deinit(u);
    return rv;
}

int
bcm_esw_alpm_hit_enabled(int unit)
{
    if (!bcm_esw_alpm_inited(unit)) {
        return FALSE;
    }

    return (ALPM_HIT_SKIP(unit)) ? FALSE : TRUE;
}

int
bcm_esw_alpm_hit_enable(int unit, int enable)
{
    if (!bcm_esw_alpm_inited(unit)) {
        return BCM_E_INIT;
    }
    ALPM_HIT_SKIP(unit) = (enable ? FALSE : TRUE);

    return BCM_E_NONE;
}

/*
 * Function:
 *      alpm_dist_hitbit_thread_stop
 * Purpose:
 *      Stop the ALPM distributed hitbits thread
 * Parameters:
 *      u  - Device unit #.
 * Returns:
 *      BCM_E_XXX
 */
int
alpm_dist_hitbit_thread_stop(int unit)
{
    int rv = BCM_E_NONE;
    if (ALPMC(unit) && !ALPM_HIT_SKIP(unit)) {
        rv = alpm_dist_hitbit_enable_set(unit, 0);
    }
    return rv;
}

#else
typedef int bcm_esw_alpm2_alpm_hit_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
