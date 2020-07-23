/*! \file bcmptm_cmdproc.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file implements cmdproc sub-block of PTM
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm_types.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MREQ
#define ACC_FOR_SER FALSE

#define BCMPTM_MREQ_USLEEP_TIME 1000 /* uSec */
#define BCMPTM_MREQ_RETRY_COUNT_MAX 1000
#define BCMPTM_MREQ_HW_READ_RETRY_COUNT_MAX 1000


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */
static inline int regain_mreq_lock(int unit)
{
    uint32_t rcnt = 0;
    int tmp_rv;
    SHR_FUNC_ENTER(unit);
    do {
        sal_usleep(BCMPTM_MREQ_USLEEP_TIME);
        tmp_rv = bcmptm_mreq_lock(unit);
        if (SHR_SUCCESS(tmp_rv)) {
            break;
        }
        rcnt++; /* all other errors, retry */
    } while (rcnt < BCMPTM_MREQ_RETRY_COUNT_MAX);
    SHR_IF_ERR_MSG_EXIT(
        tmp_rv,
        (BSL_META_U(unit, "!! bcmptm_cmdproc gave up mreq_lock for "
                    "SER correction but could not regain it \n")));
exit:
    SHR_FUNC_EXIT();
}

static inline int retry_wal_read(int unit,
                                 uint64_t flags,
                                 bcmdrd_sid_t sid,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_misc_info_t *misc_info,
                                 size_t rsp_entry_wsize,

                                 uint32_t *rsp_entry_words)
{
    uint32_t rcnt = 0;
    int tmp_rv;
    SHR_FUNC_ENTER(unit);
    rcnt = 0;
    do {
        tmp_rv = bcmptm_wal_read(unit,
                                 flags,
                                 sid,
                                 pt_dyn_info,
                                 misc_info,
                                 rsp_entry_wsize,

                                 rsp_entry_words);
        if (tmp_rv == SHR_E_BUSY) {
            /* Give SERC a chance */
            SHR_IF_ERR_EXIT(bcmptm_mreq_unlock(unit));

            /* Regain mreq_lock */
            SHR_IF_ERR_EXIT(regain_mreq_lock(unit));

            /* We again have the lock, retry the read */
            rcnt++;
        }
    } while ((tmp_rv == SHR_E_BUSY) &&
             (rcnt < BCMPTM_MREQ_HW_READ_RETRY_COUNT_MAX));
    SHR_IF_ERR_VERBOSE_EXIT(tmp_rv); /* tmp_rv may or may not be SHR_E_BUSY */
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_cmdproc_write(int unit,
                     uint64_t flags,
                     uint32_t cseq_id,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     bcmptm_misc_info_t *misc_info,
                     uint32_t *entry_words,
                     bool write_hw_tmp,
                     bool write_cache,
                     bool set_cache_vbit,
                     bcmptm_rm_op_t op_type,
                     bcmptm_rm_slice_change_t *slice_mode_change_info,
                     bcmltd_sid_t req_ltid,
                     uint16_t req_dfield_format_id,

                     uint32_t *rsp_flags)
{
    bool slam_req, write_hw;
    bcmdrd_index_class_t ic;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(sid == INVALIDm? SHR_E_INTERNAL : SHR_E_NONE);

    ic = (bcmdrd_pt_index_class(unit, sid, pt_dyn_info->tbl_inst,
                                pt_dyn_info->index));
    switch (ic) {
        case BCMDRD_IC_IGNORE:
            write_hw = FALSE; /* write only to cache */
            break;
        case BCMDRD_IC_INVALID:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "cmdproc_write: Invalid index_class: sid=%0d, "
                            "tbl_inst=%0d, index=%0d\n"),
                            sid, pt_dyn_info->tbl_inst, pt_dyn_info->index));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
            break;
        default: /* BCMDRD_IC_VALID */
            write_hw = write_hw_tmp;
            break;
    }
    slam_req = misc_info && misc_info->dma_enable;
    if (write_hw && !slam_req) {
        SHR_IF_ERR_EXIT(
            bcmptm_wal_write(
                unit,
                flags,
                sid,
                pt_dyn_info,
                misc_info,
                entry_words,
                op_type,
                slice_mode_change_info));

    } else if (write_hw && slam_req) {
        bool dma_avail;
        bcmptm_wal_dma_avail(unit, FALSE, /* read_op */
                             misc_info->dma_entry_count, &dma_avail);

        /* Ignore slice_mode_change in misc_info */

        if (dma_avail) {
            SHR_IF_ERR_EXIT(
                bcmptm_wal_write(
                    unit,
                    flags,
                    sid,
                    pt_dyn_info,
                    misc_info,
                    entry_words,
                    op_type,
                    slice_mode_change_info));
        } else { /* !dma_avail */
            uint32_t i, iter_count, *iter_entry_words = NULL, pt_entry_wsize = 0;
            bcmbd_pt_dyn_info_t tmp_pt_dyn_info;

            iter_count = misc_info->dma_entry_count;
            SHR_IF_ERR_EXIT(!iter_count ? SHR_E_PARAM : SHR_E_NONE);

            iter_entry_words = entry_words;
            pt_entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
            tmp_pt_dyn_info = *pt_dyn_info;

            for (i = 0; i < iter_count; i++) {
                SHR_IF_ERR_EXIT(
                    bcmptm_wal_write(
                        unit,
                        flags,
                        sid,
                        &tmp_pt_dyn_info,
                        NULL, /* misc_info */
                        iter_entry_words,
                        op_type,
                        slice_mode_change_info));
                iter_entry_words += pt_entry_wsize;
                tmp_pt_dyn_info.index += 1;
            }
        } /* !dma_avail */
    } /* else { write_hw = 0 } */

    /* Notes:
       1. Must return if wal_write fails for some reason.
       2. Update ptcache ONLY IF posting of write to HW is successful
       3. XY conversion for Tcam type SIDs will be done inside WAL.
       4. Reason to implement iteration of wal_write when slam is not avail
          - is to allow use of schanfifo (when avail).
     */


    /* Update cache entry - AFTER wal_write.
     * This is needed because wal_write() will read 'old' entry from ptcache
     * to support wal_abort (ptcache_undo) */
    if (write_cache) {
        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_update(unit, ACC_FOR_SER,
                                  sid,
                                  pt_dyn_info,
                                  slam_req ? misc_info->dma_entry_count : 1,
                                  entry_words,
                                  set_cache_vbit,
                                  req_ltid,
                                  req_dfield_format_id));
    }

    /* Notes:
     *      If DMA was requested by the caller of cmdproc_write, we will give
     *      the control to SERC only when the DMA write is complete.
     *
     *          If DMA was enabled, above is ok because DMA is faster than PIO.
     *
     *          If DMA was disabled - because it is passive_sim, we are ok as we
     *          don't care about SER in passive sim.
     *
     *          If DMA was disabled because entry_count is < thr_for_dma_en,
     *          then we are still ok, because we given chance to SERC after
     *          completing N PIO writes - and this is still better than making
     *          SERC wait till end of transaction.
     */
    if (bcmptm_scor_req_pending(unit)) {
        bool allow_serc = FALSE;

        SHR_IF_ERR_EXIT(bcmptm_wal_allow_serc_check(unit, &allow_serc));

        if (allow_serc) {
            /* Give SERC a chance */
            SHR_IF_ERR_EXIT(bcmptm_mreq_unlock(unit));
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Gave up mreq_lock in middle of an entry op\n")));

            /* Regain mreq_lock */
            SHR_IF_ERR_EXIT(regain_mreq_lock(unit));
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Regained mreq_lock\n")));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cmdproc_read(int unit,
                    uint64_t flags,
                    uint32_t cseq_id,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                    bcmptm_misc_info_t *misc_info,
                    size_t rsp_entry_wsize,
                    void *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    bcmltd_sid_t *rsp_ltid,
                    uint16_t *rsp_dfield_format_id,
                    uint32_t *rsp_flags)
{
    bool slam_req;
    bcmdrd_index_class_t ic;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_cache_vbit, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_dfield_format_id, SHR_E_PARAM);

    ic = (bcmdrd_pt_index_class(unit, sid, pt_dyn_info->tbl_inst,
                                pt_dyn_info->index));
    if (ic == BCMDRD_IC_INVALID) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "cmdproc_read: Invalid index_class: sid=%0d, "
                        "tbl_inst=%0d, index=%0d\n"),
                        sid, pt_dyn_info->tbl_inst, pt_dyn_info->index));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    slam_req = (misc_info && misc_info->dma_enable);

    /* No support for returning pointer only for slam_req, hw read cases */

    if (flags & BCMPTM_REQ_FLAGS_PTCACHE_RETURN_ENTRY_P) {
        SHR_IF_ERR_EXIT(slam_req || (flags & BCMLT_ENT_ATTR_GET_FROM_HW) ?
                        SHR_E_PARAM : SHR_E_NONE);
    }

    /* 1. For modeled path, all SIDs are expected to be cacheable.
          If cache was not allocated for this SID, we must return with error.
       2. read_data from PTcache for TCAMs is already in KM format - so no
          conversions needed.
       3. Reading of wo_aggr views from PTcache is supported.
     */
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_read(unit, ACC_FOR_SER,
                            flags,
                            sid,
                            pt_dyn_info,
                            slam_req ? misc_info->dma_entry_count : 1,
                            rsp_entry_wsize,

                            rsp_entry_words,
                            rsp_entry_cache_vbit,
                            rsp_dfield_format_id,
                            rsp_ltid));

    /* This is debug read from HW, we want vbit, ltid, dfield_format_id info
     * from cache, but want data from HW. Info from cache is the meta-data for
     * this entry. */
    if (flags & BCMLT_ENT_ATTR_GET_FROM_HW) {

        /* We already read cache and have: cache_vbit, dfield_format_id, ltid
         * - These are NEEDED (by PTM) to interpret contents of cache/hw entry
         *   for cases where multiple PDD formatted data could be living in
         *   entry.
         * - If cache_vbit is false, reader will know that it cannot use
         *   dfield_format_id, ltid.
         */

        /* Read data from HW - over-write rsp_entry_words with hw_entry_words */

        if (!slam_req) {
            sal_memset(rsp_entry_words, 0, rsp_entry_wsize * sizeof(uint32_t));
            SHR_IF_ERR_VERBOSE_EXIT(
                retry_wal_read(unit,
                               flags,
                               sid,
                               pt_dyn_info,
                               misc_info,
                               rsp_entry_wsize,

                               rsp_entry_words));

        } else { /* slam_req */
            bool dma_avail, wo_aggr = FALSE;
            int tmp_rv;
            bcmptm_wal_dma_avail(unit, TRUE, /* read_op */
                                 misc_info->dma_entry_count, &dma_avail);

            /* Cannot support DMA read for wo_aggr mems */
            tmp_rv = bcmptm_pt_wo_aggr_info_get(unit, sid,
                                                NULL); /* wo_aggr_info */
            switch (tmp_rv) {
            case SHR_E_NONE:
                wo_aggr = TRUE;
                break;
            case SHR_E_UNAVAIL:
                wo_aggr = FALSE;
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
                break;
            }
            if (dma_avail && !wo_aggr) {
                sal_memset(rsp_entry_words, 0, rsp_entry_wsize * sizeof(uint32_t));
                if (ic != BCMDRD_IC_IGNORE) {
                    SHR_IF_ERR_VERBOSE_EXIT(
                        retry_wal_read(unit,
                                       flags,
                                       sid,
                                       pt_dyn_info,
                                       misc_info,
                                       rsp_entry_wsize,

                                       rsp_entry_words));
                }
            } else { /* !dma_avail or wo_aggr case */
                uint32_t i, iter_count, *iter_rsp_entry_words = NULL,
                         pt_entry_wsize = 0;
                bcmbd_pt_dyn_info_t tmp_pt_dyn_info;

                iter_count = misc_info->dma_entry_count;
                SHR_IF_ERR_EXIT(!iter_count ? SHR_E_PARAM : SHR_E_NONE);

                iter_rsp_entry_words = rsp_entry_words;
                pt_entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

                SHR_IF_ERR_EXIT(
                    (rsp_entry_wsize < (pt_entry_wsize * iter_count)) ?
                     SHR_E_PARAM : SHR_E_NONE);

                tmp_pt_dyn_info = *pt_dyn_info;

                sal_memset(rsp_entry_words, 0, rsp_entry_wsize * sizeof(uint32_t));
                for (i = 0; i < iter_count; i++) {
                    SHR_IF_ERR_VERBOSE_EXIT(
                        retry_wal_read(unit,
                                       flags,
                                       sid,
                                       &tmp_pt_dyn_info,
                                       NULL, /* misc_info */
                                       rsp_entry_wsize,

                                       iter_rsp_entry_words));

                    iter_rsp_entry_words += pt_entry_wsize;
                    tmp_pt_dyn_info.index += 1;
                } /* iter loop */
            } /* !dma_avail */
        } /* slam_req */

        /* TCAM -> KM format conversion - is done in wal_read() */

        if (!(*rsp_entry_cache_vbit)) {
            *rsp_ltid = BCMPTM_LTID_RSP_HW;
            *rsp_dfield_format_id = BCMPTM_DFID_RSP_HW;
            SHR_EXIT();
        }
    } /* get rsp_entry_words from HW */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cmdproc_ptrm_info_get(int unit,
                             bcmltd_sid_t req_ltid,
                             uint16_t dfield_format_id,
                             bool dfield_format_id_valid,

                             void *rsp_ptrm_info,
                             int *rsp_index_hw_entry_info,
                             uint16_t **rsp_hw_data_type_val)
{
    bcmptm_rm_tcam_lt_info_t *rm_tcam_ptrm_info;
    bcmptm_rm_hash_lt_info_t *rm_hash_ptrm_info;
    const lt_mreq_info_t *lrd_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(rsp_ptrm_info, SHR_E_PARAM);
    if (dfield_format_id_valid) {
        SHR_NULL_CHECK(rsp_index_hw_entry_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_hw_data_type_val, SHR_E_PARAM);
        SHR_NULL_CHECK(*rsp_hw_data_type_val, SHR_E_PARAM);
    }

    /* Query for lrd_info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlrd_lt_mreq_info_get(unit,
                                req_ltid,
                                &lrd_info));
    SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);

    switch (lrd_info->pt_type) {
        case LT_PT_TYPE_HASH:
        case LT_PT_TYPE_FP_EM:
        case LT_PT_TYPE_HASH_MPLS_DECAP:
            rm_hash_ptrm_info = (bcmptm_rm_hash_lt_info_t *)rsp_ptrm_info;
            rm_hash_ptrm_info->enh_more_info = lrd_info->rm_more_info;
            rm_hash_ptrm_info->enh_more_info_count = lrd_info->rm_more_info_count;
            rm_hash_ptrm_info->dynamic_banks = lrd_info->dynamic_banks;
            break; /* HASH */

        case LT_PT_TYPE_TCAM_PRI_ONLY:
        case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
        case LT_PT_TYPE_LPM:
        case LT_PT_TYPE_FP:
            rm_tcam_ptrm_info = (bcmptm_rm_tcam_lt_info_t *)rsp_ptrm_info;
            rm_tcam_ptrm_info->num_ekw = lrd_info->num_ekw;
            rm_tcam_ptrm_info->num_edw = lrd_info->num_edw;
            rm_tcam_ptrm_info->non_aggr = lrd_info->aggr_ekw_edw? FALSE : TRUE;
            rm_tcam_ptrm_info->dynamic_banks = lrd_info->dynamic_banks;
            rm_tcam_ptrm_info->pt_type = lrd_info->pt_type;
            rm_tcam_ptrm_info->hw_entry_info = lrd_info->hw_entry_info;
            rm_tcam_ptrm_info->hw_entry_info_count = lrd_info->hw_entry_info_count;
            rm_tcam_ptrm_info->rm_more_info = lrd_info->rm_more_info;

            if (dfield_format_id_valid) { /* for INSERT, or entry in cache */
                *rsp_index_hw_entry_info = 0; /* hw_entry_info_count is 1 */
                *rsp_hw_data_type_val = NULL; /* no data_type field */
            }
            break; /* TCAM_PRI_ONLY */

        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "req_ltid=%0d, pt_type = %0d is unsupported !\n"),
                 req_ltid, lrd_info->pt_type));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    } /* pt_type */

exit:
    SHR_FUNC_EXIT();
}
