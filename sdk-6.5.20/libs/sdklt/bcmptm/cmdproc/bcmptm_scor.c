/*! \file bcmptm_scor.c
 *
 * Interface functions for PTM
 *
 * This file contains the top-level interface func for PTM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define BCMPTM_CFG_SCOR_WAIT_US 10000 /* 10 mS */
#define ACC_FOR_SER TRUE
#define HW_TABLES_FROZEN FALSE
        

/* Declare error if cannot get lock after this time. No retries */
#define BCMPTM_CFG_SERC_MREQ_LOCK_WAIT_USEC 2000000 /* 2 sec */
#define BCMPTM_CFG_MREQ_MREQ_LOCK_WAIT_USEC 2000000 /* 2 sec */

#define BCMPTM_CFG_SERC_IREQ_LOCK_WAIT_USEC 2000000 /* 2 sec */
#define BCMPTM_CFG_IREQ_IREQ_LOCK_WAIT_USEC 2000000 /* 2 sec */


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static sal_mutex_t mreq_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_has_mreq_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool mpath_has_mreq_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_wants_mreq_mutex[BCMDRD_CONFIG_MAX_UNITS];

static sal_mutex_t ireq_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool serc_has_ireq_mutex[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static int
serc_ireq_lock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (serc_has_ireq_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! SERC already has ireq_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
    } else if (!HW_TABLES_FROZEN) {
        tmp_rv = bcmptm_mutex_take(unit, ireq_mutex[unit],
                                   BCMPTM_CFG_SERC_IREQ_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_ireq_mutex[unit] = TRUE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC got ireq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not get ireq_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
serc_ireq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (serc_has_ireq_mutex[unit]) {
        tmp_rv = sal_mutex_give(ireq_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_ireq_mutex[unit] = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC released ireq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not release ireq_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
serc_mreq_lock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (serc_has_mreq_mutex[unit]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! SERC already has mreq_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
    } else if (!HW_TABLES_FROZEN) {
        serc_wants_mreq_mutex[unit] = TRUE;
        tmp_rv = bcmptm_mutex_take(unit, mreq_mutex[unit],
                                   BCMPTM_CFG_SERC_MREQ_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_mreq_mutex[unit] = TRUE;
            serc_wants_mreq_mutex[unit] = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC got mreq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not get mreq_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
serc_mreq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (serc_has_mreq_mutex[unit]) {
        tmp_rv = sal_mutex_give(mreq_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            serc_has_mreq_mutex[unit] = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC released mreq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! SERC could not release mreq_mutex \n")));
        }
    }
    return tmp_rv;
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_ireq_lock(int unit)
{
    int tmp_rv;
    tmp_rv = bcmptm_mutex_take(unit, ireq_mutex[unit],
                               BCMPTM_CFG_IREQ_IREQ_LOCK_WAIT_USEC);
    if (tmp_rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path got ireq_mutex \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path could not get ireq_mutex \n")));
    }
    return tmp_rv;
}

int
bcmptm_ireq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    tmp_rv = sal_mutex_give(ireq_mutex[unit]);
    if (tmp_rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path released ireq_mutex \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! ireq_path could not release ireq_mutex \n")));
    }
    return tmp_rv;
}

/* Note:
 *  - mreq_lock, mreq_unlock work correctly iff we have one modeled path
 *    thread and these APIs should be called only in that thread context.
 *  - Any caller can release the lock.
 */
int
bcmptm_mreq_lock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (!mpath_has_mreq_mutex[unit]) {
        tmp_rv = bcmptm_mutex_take(unit, mreq_mutex[unit],
                                   BCMPTM_CFG_MREQ_MREQ_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            mpath_has_mreq_mutex[unit] = TRUE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! mreq_path got mreq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! mreq_path could not get mreq_mutex \n")));
        }
    }
    return tmp_rv;
}

int
bcmptm_mreq_unlock(int unit)
{
    int tmp_rv = SHR_E_NONE;
    if (mpath_has_mreq_mutex[unit]) {
        tmp_rv = sal_mutex_give(mreq_mutex[unit]);
        if (tmp_rv == SHR_E_NONE) {
            mpath_has_mreq_mutex[unit] = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! mreq_path released mreq_mutex \n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! mreq_path could not release mreq_mutex \n")));
        }
    }
    return tmp_rv;
}

int
bcmptm_scor_read(int unit,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 void *pt_ovrr_info,
                 size_t rsp_entry_wsize,

                 uint32_t *rsp_entry_words)
{
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    bool rsp_entry_cache_vbit;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_IF_ERR_EXIT(!rsp_entry_wsize? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    /* Debug checks - must not be done during runtime */
    if (!bcmdrd_pt_is_valid(unit, sid) ||
        (pt_dyn_info->index < bcmptm_scor_pt_index_min(unit, sid)) ||
        (pt_dyn_info->index > bcmptm_scor_pt_index_max(unit, sid)) ||
        !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                               pt_dyn_info->index)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (flags & BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE) { /* Read requested from HW */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "SERC requested read from HW for sid %0d, pt %s \n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));

        /* Read from HW could fail because of SER
         * - For example this read could be to check whether SER correction
         *   worked (hard-fault detection) and failure on read from HW indicates
         *   that SER correction could not fix the bad location.
         */
        SHR_IF_ERR_EXIT(
            bcmbd_pt_read(unit,
                          sid,
                          pt_dyn_info,
                          pt_ovrr_info,
                          rsp_entry_words, /* read_data */
                          rsp_entry_wsize));

        /* TCAM -> KM format conversion */
        if (bcmdrd_pt_attr_is_cam(unit, sid) &&
            !(flags & BCMPTM_SCOR_REQ_FLAGS_DONT_CONVERT_XY2KM)) {
            /* Note: Following will fail if PTcache is not allocated.
                     (because, we will query PTcache to retrieve key, mask
                     field info. */
            SHR_IF_ERR_EXIT(
                bcmptm_pt_tcam_xy_to_km(unit,
                                        sid,
                                        1,                 /* entry_count */
                                        rsp_entry_words,   /* xy_entry */
                                        rsp_entry_words)); /* km_entry */
                /* Note: we are requesting called func to REPLACE xy words
                 * with km words to save on un-necessary copy from km words array
                 * back to rsp words array.
                 */
        }
    } else { /* Read from cache */
        int tmp_rv = SHR_E_NONE;
        tmp_rv = bcmptm_ptcache_read(unit,
                                     ACC_FOR_SER,
                                     0, /* flags */
                                     sid,
                                     pt_dyn_info,
                                     1,                     /* entry_count */
                                     rsp_entry_wsize,
                                     rsp_entry_words,
                                     &rsp_entry_cache_vbit, /* unused */
                                     &rsp_entry_dfid,       /* unused */
                                     &rsp_entry_ltid);      /* unused */
        SHR_IF_ERR_EXIT(tmp_rv);
           /* if tmp_rv was SHR_E_NONE - even if rsp_entry_cache_vbit == 0,
            *                            data is correct in cache
            * if tmp_rv was SHR_E_UNAVAIL - cache is not allocated for this PT
            * if tmp_rv was some other error - then pass on failure to caller
            */
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_write(int unit,
                  uint64_t flags,
                  bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                  void *pt_ovrr_info,
                  uint32_t *entry_words)
{
    uint32_t xy_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    uint32_t is_cam = 0;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    /* Debug checks - must not be done during runtime */
    if (!bcmdrd_pt_is_valid(unit, sid) ||
        (pt_dyn_info->index < bcmptm_scor_pt_index_min(unit, sid)) ||
        (pt_dyn_info->index > bcmptm_scor_pt_index_max(unit, sid)) ||
        !bcmdrd_pt_index_valid(unit, sid, pt_dyn_info->tbl_inst,
                               pt_dyn_info->index)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* KM -> TCAM format conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid) &&
        !(flags & BCMPTM_SCOR_REQ_FLAGS_KM_IN_XY_FORMAT)) {
        is_cam = 1;
        SHR_IF_ERR_EXIT(
            bcmptm_pt_tcam_km_to_xy(unit,
                                    sid,
                                    1,                 /* entry_count */
                                    entry_words,       /* km_entry */
                                    xy_entry_words));  /* xy_entry */
    }

    /* Issue write to HW
     * - It is possible that HW regsfile has some register which has read-only
     *   fields and also writeable fields to enable/disable parity/ecc.
     *   So we must allow SERC to be able to write to such symbols. */
    if (bcmdrd_pt_is_readonly(unit, sid)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! WARNING: SERC is attempting to write to "
                        "sid %0d, pt %s, which has read_only fields\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_IF_ERR_EXIT(
        bcmbd_pt_write(unit,
                       sid,
                       pt_dyn_info,
                       pt_ovrr_info,
                       is_cam? xy_entry_words /* xy_entry */
                             : entry_words));  /* km_entry */

    if (flags & BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE) {
        /* Update cache (if it exists for this pt) */
        /* Note: DONT_USE_CACHE does not affect cache update of mem */
        /* ptcache will complain if 'sid' supports LTID, DFID - we don't expect
         * SERC to be updating cache for such SIDs */

        if (flags & BCMPTM_SCOR_REQ_FLAGS_KM_IN_XY_FORMAT) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! ERROR: SERC is attempting to write key, "
                            "mask in xy format to ptcache for sid %0d, pt %s\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! WARNING: SERC is attempting to write to "
                        "ptcache for sid %0d, pt %s\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));

        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmptm_ptcache_update(unit,
                                  ACC_FOR_SER,
                                  sid,
                                  pt_dyn_info,
                                  1,                   /* entry_count */
                                  entry_words,         /* km_entry */
                                  TRUE,                /* cache_vbit_set */
                                  BCMPTM_LTID_RSVD,    /* req_ltid - ILL */
                                  BCMPTM_DFID_IREQ), /* req_dfid - ILL */
            SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_wait(int unit)
{
    SHR_FUNC_ENTER(unit);

    sal_usleep(BCMPTM_CFG_SCOR_WAIT_US);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_done(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_locks_take(int unit,
                       bcmdrd_sid_t sid)
{
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    SHR_FUNC_ENTER(unit);

    /* Check if sid is of type 'counter' */
    sal_memset(&ptcache_ctr_info, 0, sizeof(bcmptm_ptcache_ctr_info_t));
    if (sid != INVALIDm) {
        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                        &ptcache_ctr_info));
    }
    /* else: assume, SER logic wants to take all locks.
     * This can also be used as an indication by caller that we want to take
     * all locks. */

    /* Freeze wal reader - Not needed with new scheme */

    /* Freeze interactive path */
    SHR_IF_ERR_EXIT(serc_ireq_lock(unit));

    if (!ptcache_ctr_info.pt_is_ctr) { /* is NOT counter table */
        /* Freeze wal writer (modeled path) */
        SHR_IF_ERR_EXIT(serc_mreq_lock(unit));

        /* Wait for wal_empty */
        SHR_IF_ERR_EXIT(bcmptm_wal_drain(unit, FALSE));
                /* FALSE => end_state can be IDLE or BLOCKING_READ */
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)serc_ireq_unlock(unit);
        (void)serc_mreq_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_locks_give(int unit,
                       bcmdrd_sid_t sid)
{
    int rv_failure = FALSE;
    SHR_FUNC_ENTER(unit);

    /* Restart wal writer */
    if (SHR_FAILURE(serc_mreq_unlock(unit))) {
        rv_failure = TRUE;
    }

    /* Restart interactive path */
    if (SHR_FAILURE(serc_ireq_unlock(unit))) {
        rv_failure = TRUE;
    }

    SHR_IF_ERR_EXIT(rv_failure ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_scor_req_pending(int unit)
{
    return serc_wants_mreq_mutex[unit];
}

int
bcmptm_scor_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Nothing special about WB */

    mreq_mutex[unit] = sal_mutex_create("MODELED_PATH_REQ_LOCK");
    SHR_NULL_CHECK(mreq_mutex[unit], SHR_E_MEMORY);
    serc_has_mreq_mutex[unit] = FALSE;
    mpath_has_mreq_mutex[unit] = FALSE;
    serc_wants_mreq_mutex[unit] = FALSE;

    /* No better place to put it */
    ireq_mutex[unit] = sal_mutex_create("INTERACTIVE_PATH_REQ_LOCK");
    SHR_NULL_CHECK(ireq_mutex[unit], SHR_E_MEMORY);
    serc_has_ireq_mutex[unit] = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_scor_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    sal_mutex_destroy(mreq_mutex[unit]);
    serc_has_mreq_mutex[unit] = FALSE;
    mpath_has_mreq_mutex[unit] = FALSE;
    serc_wants_mreq_mutex[unit] = FALSE;

    /* No better place to put it */
    sal_mutex_destroy(ireq_mutex[unit]);
    serc_has_ireq_mutex[unit] = FALSE;

/* exit: */
    SHR_FUNC_EXIT();
}
