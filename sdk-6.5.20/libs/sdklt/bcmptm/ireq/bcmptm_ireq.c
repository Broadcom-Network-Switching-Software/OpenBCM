/*! \file bcmptm_ireq.c
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
#include <sal/sal_sleep.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmlrd_ptrm.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_IREQ
#define BCMPTM_CFG_IREQ_SERC_RETRY_COUNT 100
#define BCMPTM_CFG_IREQ_SERC_WAIT_USEC 10000 /* 10 mS */
#define ACC_FOR_SER FALSE

#define BCMPTM_DO_IREQ_SER_RETRY_PREP do {\
    retry_count++;\
    SHR_IF_ERR_EXIT(!have_ireq_lock ? SHR_E_INTERNAL : SHR_E_NONE);\
    (void)(bcmptm_ireq_unlock(unit));\
    have_ireq_lock = FALSE;\
    sal_usleep(BCMPTM_CFG_IREQ_SERC_WAIT_USEC);\
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));\
    have_ireq_lock = TRUE;\
} while (0)


/*******************************************************************************
 * Typedefs
 */
typedef enum {
    BCMPTM_CTR_REQ_COUNT_F = 0,
    BCMPTM_CTR_REQ_PASSTHRU = 1,
    BCMPTM_CTR_REQ_CFG_F = 2
} bcmptm_ctr_format_t;


/*******************************************************************************
 * Private variables
 */
static bcmptm_pt_status_ireq_hash_lt_cb \
              lt_ireq_hash_cb[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_pt_status_ireq_op_cb \
              lt_ireq_op_cb[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static int
ireq_pt_check(int unit, bcmdrd_sid_t sid, bcmbd_pt_dyn_info_t *pt_dyn_info,
              uint64_t flags, const char *caller_str)
{
    int tmp_rv = SHR_E_NONE;
    const char *pt_name = "INVALIDm";
    bool pt_valid = FALSE, index_valid = FALSE;
    pt_valid = bcmdrd_pt_is_valid(unit, sid);
    index_valid = bcmdrd_pt_index_valid(unit, sid,
                                        pt_dyn_info->tbl_inst,
                                        pt_dyn_info->index);
    pt_name = bcmdrd_pt_sid_to_name(unit, sid);
    if (!pt_valid || !index_valid) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s failed for sid=%s(%d), tbl_inst=%d,"
                        " index=%d, flags=0x%" PRIx64 ", pt_valid=%d, "
                        " index_valid=%d !!\n"),
             caller_str, pt_name, sid, pt_dyn_info->tbl_inst,
             pt_dyn_info->index, flags, pt_valid ? 1 : 0,
             index_valid ? 1 : 0));
        tmp_rv = SHR_E_PARAM;
    } else if ((pt_dyn_info->index < bcmptm_pt_index_min(unit, sid)) ||
               (pt_dyn_info->index > bcmptm_pt_index_max(unit, sid)) ||
               (bcmdrd_pt_index_class(
                       unit, sid, pt_dyn_info->tbl_inst,
                       pt_dyn_info->index) == BCMDRD_IC_INVALID)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "%s: pt_index_problem for sid=%s(%d), "
                        "tbl_inst=%d, index=%d, flags=0x%" PRIx64 ", "
                        "min_index=%d, max_index=%d, index_class = %d\n"),
             caller_str, pt_name, sid, pt_dyn_info->tbl_inst,
             pt_dyn_info->index, flags,
             bcmptm_pt_index_min(unit, sid), bcmptm_pt_index_max(unit, sid),
             bcmdrd_pt_index_class(unit, sid, pt_dyn_info->tbl_inst,
                                   pt_dyn_info->index)));
        tmp_rv = SHR_E_PARAM;
    }
    return tmp_rv;
}

static int
ctr_entry_format_get(int unit, uint64_t flags, bcmltd_sid_t req_ltid,
                     bool ctr_is_cci_only,
                     bcmptm_ctr_format_t *ctr_entry_format)
{
    int tmp_rv;
    const lt_mreq_info_t *lrd_info = NULL;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctr_entry_format, SHR_E_PARAM);

    if (flags & BCMPTM_REQ_FLAGS_PASSTHRU) {
        *ctr_entry_format = BCMPTM_CTR_REQ_PASSTHRU;
    } else if (ctr_is_cci_only) {
        /* read entry in ctr_cache_format */
        *ctr_entry_format = BCMPTM_CTR_REQ_COUNT_F;
    } else { /* SID which has both counter fields and cfg fields */
        /* default: read entry in ctr_cache_format */
        *ctr_entry_format = BCMPTM_CTR_REQ_COUNT_F;

        /* query for lrd_info */
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, req_ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            if (lrd_info->pt_type == LT_PT_TYPE_CTR_CFG_ONLY) {
                *ctr_entry_format = BCMPTM_CTR_REQ_CFG_F;
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
           *    (tmp_rv == SHR_E_UNAVAIL) implies LT is not referring to cfg
           *    fields so,
           *    *ctr_entry_format = BCMPTM_CTR_REQ_COUNT_F (default)
           * }
           */
    } /* SID which has both counter fields and cfg fields */

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/* Will first try reading from PTcache and then from HW.
 * Will NOT do TCAM(x,y) to (k,m) conversion */
int
bcmptm_ireq_read_cci(int unit,
                     bool acc_for_ser,
                     uint64_t flags,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     void *in_pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,

                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags)
{
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    bool rsp_entry_cache_vbit, have_ireq_lock = FALSE;
    int tmp_rv = SHR_E_NONE, retry_count = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);

    /* Now PTM is executing this cmd, so perform all checks */
    SHR_IF_ERR_EXIT(
        ireq_pt_check(unit, sid, pt_dyn_info, flags, "ireq_read_cci"));

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;

    /* Try reading from SW cache */
    if (!(flags & BCMLT_ENT_ATTR_GET_FROM_HW) &&
        (bcmptm_pt_ser_resp(unit, sid) != BCMDRD_SER_RESP_WRITE_CACHE_RESTORE)
       ) {
        tmp_rv = bcmptm_ptcache_read(unit,
                                     acc_for_ser,
                                     0, /* flags */
                                     sid,
                                     pt_dyn_info,
                                     1, /* entry_count */
                                     rsp_entry_wsize,
                                     rsp_entry_words,
                                     &rsp_entry_cache_vbit,
                                     &rsp_entry_dfid,  /* Note: unused */
                                     &rsp_entry_ltid);
        switch (tmp_rv) {
            case SHR_E_UNAVAIL:
                /* Cache is not alloc for this pt, must read from HW */
                break;
            case SHR_E_NONE:
                if (rsp_entry_cache_vbit) {
                    /* Read was satisfied from cache, we are done
                     * Note: cache stores KM format for tcam tables
                     */
                    *rsp_ltid = rsp_entry_ltid;
                    SHR_EXIT();
                }
                /* else
                 *     Cache is alloc for this pt, but entry is !valid in cache
                 *     Must read entry from HW.
                 */
                break; /* we will need this if rsp_entry_cache_vbit is FALSE */
            default:
                /* Propagate error to caller */
                *rsp_ltid = rsp_entry_ltid;
                SHR_IF_ERR_EXIT(tmp_rv);
                break; /* Redundant */
        }
    } else {
        /* Debug messages - informational only */
        if (flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %d, pt %s, "
                            "BCMLT_ENT_ATTR_GET_FROM_HW is set... will "
                            "read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %d, pt %s, SER_RESP_WRITE_CACHE_RESTORE"
                            " is set... will read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }

    /* Read from HW - if read from cache did not go through */
    *rsp_ltid = BCMPTM_LTID_RSP_HW;

    /* Read from HW could fail because of SER - so must wait for SER correction
     * if needed. */
    do {
        tmp_rv = bcmbd_pt_read(unit,
                               sid,
                               pt_dyn_info,
                               in_pt_ovrr_info,
                               rsp_entry_words, /* read_data */
                               rsp_entry_wsize);
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv) && (retry_count == 0)) {
            if (bcmptm_pt_ser_resp(unit, sid) == BCMDRD_SER_RESP_NONE) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Will not retry failed ireq_read_cci\n")));
                break; /* from while loop with retry_count = 0 */
            }
        }
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv) &&
            (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT)) {
            BCMPTM_DO_IREQ_SER_RETRY_PREP;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Will retry failed ireq_read\n")));
        } else {
            break; /* from while loop */
        }
    } while (1);
    if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "ireq_read to sid=%d inst=%d index=%d failed (after %d attempts) !!\n"),
             sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, retry_count));
    }
    SHR_IF_ERR_EXIT(tmp_rv);

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    SHR_FUNC_EXIT();
}

/* Approach that is followed for key, mask fields in TCAM tables:
 *     1. Caller of bcmptm_ireq_write (mostly LTM) will always present
 *        key, mask in KM format.
 *        PTM will convert KM format to XY format as needed b4 sending bcmbd req
 *        For some devices the conversion may be done by HW.
 *
 *     2. cache stores KM format
 *        Reads from cache will provide exact KM that was sent during Write
 *
 *     3. reading of tcam table from HW always returns data in XY format
 *
 *     4. bcmptm_ireq_read will by default try read from cache
 *        If table is not cached, etc or user has specified DONT_USE_CACHE
 *        read will be sent to HW.
 *
 *     5. bcmptm_ireq_read will by default, return (key, mask) in KM format
 *        If read was serviced from cache, KM_format is exact
 *        If read was serviced from HW, KM_format is approx (info loss for masked bits)
 *        Optionally, caller can specify DONT_CONVERT_XY2DM - this flag is
 *        looked at ONLY if read was from HW (user can enforce this by setting
 *        DONT_USE_CACHE)
 */
int
bcmptm_ireq_read(int unit,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 void *in_pt_dyn_info,
                 void *in_pt_ovrr_info,
                 size_t rsp_entry_wsize,
                 bcmltd_sid_t req_ltid,

                 uint32_t *rsp_entry_words,
                 bcmltd_sid_t *rsp_ltid,
                 uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint16_t rsp_entry_dfid;
    bcmltd_sid_t rsp_entry_ltid;
    bool rsp_entry_cache_vbit;
    bool have_ireq_lock = FALSE, wo_aggr = FALSE;
    bool ser_failure = FALSE, retry_needed = FALSE;
    bcmptm_wo_aggr_info_t wo_aggr_info;
    int tmp_rv, retry_count = 0;

    SHR_FUNC_ENTER(unit);

    bcmptm_lt_stat_increment(unit, req_ltid,
                             TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_IF_ERR_EXIT(!rsp_entry_wsize? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    /* Check for CCI dispatch */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid, &ptcache_ctr_info),
        SHR_E_UNAVAIL);

    if (ptcache_ctr_info.pt_is_ctr) { /* is counter table */
        bcmptm_cci_ctr_info_t cci_info;
        bcmptm_ctr_format_t rsp_entry_format;

        SHR_IF_ERR_EXIT(
            ctr_entry_format_get(unit, flags, req_ltid,
                                 ptcache_ctr_info.ctr_is_cci_only,
                                 &rsp_entry_format));

        cci_info.flags = flags;
        cci_info.map_id = ptcache_ctr_info.cci_map_id;
        cci_info.sid = sid;
        cci_info.in_pt_dyn_info = pt_dyn_info;
        cci_info.in_pt_ovrr_info = in_pt_ovrr_info;
        cci_info.req_ltid = req_ltid;
        cci_info.rsp_ltid = rsp_ltid;
        cci_info.rsp_flags = NULL;

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %d, pt %s, cci_map_id %d, "
                        "Read request dispatched to CCI, "
                        "ctr_rsp_entry_format = %s\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid),
             ptcache_ctr_info.cci_map_id,
             (rsp_entry_format == BCMPTM_CTR_REQ_PASSTHRU ? "PASSTHRU" :
              rsp_entry_format == BCMPTM_CTR_REQ_CFG_F ? "CFG_FIELDS" :
              "COUNT_FIELDS")));

        switch (rsp_entry_format) {
            case BCMPTM_CTR_REQ_PASSTHRU:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_passthru_read(unit, &cci_info,
                                                 rsp_entry_words,
                                                 rsp_entry_wsize));
                break;
            case BCMPTM_CTR_REQ_CFG_F:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_config_read(unit, &cci_info,
                                               rsp_entry_words,
                                               rsp_entry_wsize));
                break;
            default: /* BCMPTM_CTR_REQ_COUNT_F */
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_read(unit, &cci_info,
                                        rsp_entry_words,
                                        rsp_entry_wsize));
                break;
        }
        if (lt_ireq_op_cb[unit] != NULL) {
            SHR_IF_ERR_EXIT
                (lt_ireq_op_cb[unit](unit, cci_info.req_ltid, 0xFFFFFFFF,
                         cci_info.sid,
                         (bcmbd_pt_dyn_info_t *)&cci_info.dyn_info));
        }

        /* we are done */
        SHR_EXIT();
    } /* is counter table */

    /* All other tables */
    SHR_IF_ERR_EXIT(
        ireq_pt_check(unit, sid, pt_dyn_info, flags, "ireq_read"));
    /* CCI does checking of sid, index, tbl_inst for counter type SIDs and also
     * transformation of index, tbl_inst for MIB counters. So ireq_pt_check()
     * must be called only for non-counter SIDs */

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;
            /* Take lock outside of ireq_read_internal - to ensure that we take
             * ireq_mutex only for non-counter tables */

    /* Try reading from SW cache */
    if (!(flags & BCMLT_ENT_ATTR_GET_FROM_HW) &&
        (bcmptm_pt_ser_resp(unit, sid) != BCMDRD_SER_RESP_WRITE_CACHE_RESTORE)
       ) {
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmptm_ptcache_read(unit,
                                     ACC_FOR_SER,
                                     0, /* flags */
                                     sid,
                                     pt_dyn_info,
                                     1, /* entry_count */
                                     rsp_entry_wsize,
                                     rsp_entry_words,
                                     &rsp_entry_cache_vbit,
                                     &rsp_entry_dfid,  /* Note: unused */
                                     &rsp_entry_ltid);
        switch (tmp_rv) {
            case SHR_E_UNAVAIL:
                /* Cache is not alloc for this pt, must read from HW */
                break;
            case SHR_E_NONE:
                if (rsp_entry_cache_vbit) {
                    /* Read was satisfied from cache, we are done
                     * Note: cache stores KM format for tcam tables
                     */
                    *rsp_ltid = rsp_entry_ltid;
                    SHR_EXIT();
                }
                /* else
                 *     Cache is alloc for this pt, but entry is !valid in cache
                 *     Must read entry from HW.
                 */
                break; /* we will need this if rsp_entry_cache_vbit is FALSE */
            default:
                /* Propagate error to caller */
                *rsp_ltid = rsp_entry_ltid;
                SHR_IF_ERR_EXIT(tmp_rv);
                break; /* Redundant */
        }
    } else {
        /* Debug messages - informational only */
        if (flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %d, pt %s, "
                            "BCMLT_ENT_ATTR_GET_FROM_HW is set... will "
                            "read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "sid %d, pt %s, SER_RESP_WRITE_CACHE_RESTORE"
                            " is set... will read from HW\n"),
                 sid, bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }

    /* Read from HW - if read from cache did not go through */
    *rsp_ltid = BCMPTM_LTID_RSP_HW;

    /* Determine if sid is wo_aggr */
    sal_memset(&wo_aggr_info, 0, sizeof(bcmptm_wo_aggr_info_t));
    tmp_rv = bcmptm_pt_wo_aggr_info_get(unit, sid, &wo_aggr_info);
    switch (tmp_rv) {
    case SHR_E_NONE:
        SHR_IF_ERR_EXIT(
            (!wo_aggr_info.n_sid_count || !wo_aggr_info.n_sid_list) ?
            SHR_E_INTERNAL : SHR_E_NONE);
        wo_aggr = TRUE;
        break;
    case SHR_E_UNAVAIL:
        wo_aggr = FALSE;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
        break;
    }

    do {
        ser_failure = FALSE;
        retry_needed = FALSE;
        if (wo_aggr) {
            tmp_rv =
                bcmptm_wo_aggr_read_hw(unit, flags, sid,
                                       pt_dyn_info, in_pt_ovrr_info,
                                       &wo_aggr_info, rsp_entry_wsize,
                                       rsp_entry_words,
                                       &ser_failure, &retry_needed);
        } else {
            tmp_rv = bcmbd_pt_read(unit,
                                   sid,
                                   pt_dyn_info,
                                   in_pt_ovrr_info,
                                   rsp_entry_words, /* read_data */
                                   rsp_entry_wsize);
            if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
                ser_failure = TRUE;
                if (bcmptm_pt_ser_resp(unit, sid) != BCMDRD_SER_RESP_NONE) {
                    retry_needed = TRUE;
                }
            }
        }
        if (ser_failure) {
            if (!retry_needed) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Will not retry failed ireq read\n")));
            } else if (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT) {
                BCMPTM_DO_IREQ_SER_RETRY_PREP;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Will retry failed ireq_read\n")));
            }
            else {
                break; /* from while loop */
            }
        } else {
            break; /* from while loop */
        }
    } while (1);
    if (ser_failure) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "ireq_read to sid=%d inst=%d index=%d failed (after %d attempts) !!\n"),
             sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, retry_count));
    }
    SHR_IF_ERR_EXIT(tmp_rv); /* don't attempt tcam_conv if HW read failed */

    /* TCAM -> KM format conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid) &&
        !(flags & BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM)
       ) {
        bcmptm_pt_tcam_km_format_t tcam_km_format;
        SHR_IF_ERR_EXIT(bcmptm_pt_tcam_km_format(unit, sid, &tcam_km_format));
        if (tcam_km_format != BCMPTM_TCAM_KM_FORMAT_IS_KM) {
            /* Note:
             * Following will fail if PTcache is not avail for this PT
             * (PTcache is needed to store key, mask field info).
             */
            SHR_IF_ERR_MSG_EXIT(
                bcmptm_pt_tcam_xy_to_km(unit,
                                        sid,
                                        1,                 /* entry_count */
                                        rsp_entry_words,   /* xy_entry */
                                        rsp_entry_words),  /* km_entry */
                (BSL_META_U(unit, "Could not convert xy_to_km for read of sid "
                            "= %s(0%d)\n"),
                 bcmdrd_pt_sid_to_name(unit, sid), sid));
            /* Note: we are requesting called func to REPLACE xy words
             * with km words to save on un-necessary copy from km words array
             * back to rsp words array.
             */
        }
    }
    if (lt_ireq_op_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_ireq_op_cb[unit](unit, req_ltid, 0xFFFFFFFF, sid,
                                 (bcmbd_pt_dyn_info_t *)pt_dyn_info));
    }

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    if (SHR_FUNC_ERR()) {
        bcmptm_lt_stat_increment(unit, req_ltid,
                                 TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ireq_write(int unit,
                  uint64_t flags,
                  bcmdrd_sid_t sid,
                  void *in_pt_dyn_info,
                  void *in_pt_ovrr_info,
                  uint32_t *entry_words,
                  bcmltd_sid_t req_ltid,

                  bcmltd_sid_t *rsp_ltid,
                  uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    uint32_t tmp_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    uint32_t is_cam = 0;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bool have_ireq_lock = FALSE;
    int tmp_rv, retry_count = 0;

    SHR_FUNC_ENTER(unit);

    bcmptm_lt_stat_increment(unit, req_ltid,
                             TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    *rsp_ltid = req_ltid;

    /* Check for CCI dispatch */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid, &ptcache_ctr_info),
        SHR_E_UNAVAIL);

    if (ptcache_ctr_info.pt_is_ctr) { /* is counter table */
        bcmptm_cci_ctr_info_t cci_info;
        bcmptm_ctr_format_t req_entry_format;

        SHR_IF_ERR_EXIT(
            ctr_entry_format_get(unit, flags, req_ltid,
                                 ptcache_ctr_info.ctr_is_cci_only,
                                 &req_entry_format));

        cci_info.flags = flags;
        cci_info.map_id = ptcache_ctr_info.cci_map_id;
        cci_info.sid = sid;
        cci_info.in_pt_dyn_info = pt_dyn_info;
        cci_info.in_pt_ovrr_info = in_pt_ovrr_info;
        cci_info.req_ltid = req_ltid;
        cci_info.rsp_ltid = rsp_ltid;
        cci_info.rsp_flags = NULL;

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %d, pt %s, cci_map_id %d, "
                        "Write request dispatched to CCI, "
                        "ctr_req_entry_format = %s\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid),
             ptcache_ctr_info.cci_map_id,
             (req_entry_format == BCMPTM_CTR_REQ_PASSTHRU ? "PASSTHRU" :
              req_entry_format == BCMPTM_CTR_REQ_CFG_F ? "CFG_FIELDS" :
              "COUNT_FIELDS")));

        switch (req_entry_format) {
            case BCMPTM_CTR_REQ_PASSTHRU:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_passthru_write(unit, &cci_info,
                                                  entry_words));
                break;
            case BCMPTM_CTR_REQ_CFG_F:
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_config_write(unit, &cci_info,
                                                entry_words));
                break;
            default: /* BCMPTM_CTR_REQ_COUNT_F */
                SHR_IF_ERR_EXIT(
                    bcmptm_cci_ctr_write(unit, &cci_info,
                                         entry_words));
                break;
        }
        if (lt_ireq_op_cb[unit] != NULL) {
            SHR_IF_ERR_EXIT
                (lt_ireq_op_cb[unit](unit, cci_info.req_ltid, 0xFFFFFFFF,
                         cci_info.sid,
                         (bcmbd_pt_dyn_info_t *)&cci_info.dyn_info));
        }

        /* we are done */
        SHR_EXIT();

        /* Note:
         * if (CCI_HW_ENTRY is set) {
         *    CCI must also update PTcache
         * }
         */
    } /* is counter table */

    /* All other tables */
    SHR_IF_ERR_EXIT(
        ireq_pt_check(unit, sid, pt_dyn_info, flags, "ireq_write"));
    /* CCI does checking of sid, index, tbl_inst for counter type SIDs and also
     * transformation of index, tbl_inst for MIB counters. So ireq_pt_check()
     * must be called only for non-counter SIDs */


    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;

    /* KM -> TCAM format conversion */
    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        bcmptm_pt_tcam_km_format_t tcam_km_format;
        SHR_IF_ERR_EXIT(bcmptm_pt_tcam_km_format(unit, sid, &tcam_km_format));
        if (tcam_km_format != BCMPTM_TCAM_KM_FORMAT_IS_KM) {
            /* Note:
             * Following will fail if PTcache is not avail for this PT
             * (PTcache is needed to store key, mask field info).
             */
            SHR_IF_ERR_MSG_EXIT(
                bcmptm_pt_tcam_km_to_xy(unit,
                                        sid,
                                        1,                 /* entry_count */
                                        entry_words,       /* km_entry */
                                        tmp_entry_words),  /* xy_entry */
                (BSL_META_U(unit, "Could not convert km_to_xy for write of sid "
                            "= %s(0%d)\n"),
                 bcmdrd_pt_sid_to_name(unit, sid), sid));
            is_cam = 1;
        }
    }

    /* Issue write to HW */
    if (bcmdrd_pt_is_readonly(unit, sid)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %d, pt %s, attempting WRITE for"
                        " PT with read_only fields on Interactive path\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    do {
        tmp_rv = bcmbd_pt_write(unit,
                                sid,
                                pt_dyn_info,
                                in_pt_ovrr_info,
                                is_cam? tmp_entry_words /* xy_entry */
                                      : entry_words);  /* km_entry */
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv) && (retry_count == 0)) {
            if (bcmptm_pt_ser_resp(unit, sid) == BCMDRD_SER_RESP_NONE) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Will not retry failed ireq_write\n")));
                break; /* from while loop with retry_count = 0 */
            }
        }
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv) &&
            (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT)) {
            BCMPTM_DO_IREQ_SER_RETRY_PREP;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Will retry failed ireq_write\n")));
        } else {
            break; /* from while loop */
        }
    } while (1);
    if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "ireq_write to sid=%d inst=%d index=%d failed (after %d attempts) !!\n"),
             sid, pt_dyn_info->tbl_inst, pt_dyn_info->index, retry_count));
    }
    SHR_IF_ERR_EXIT(tmp_rv); /* don't update ptcache if HW write failed */

    /* Update cache (if it exists for this pt) */
    /* Note: DONT_USE_CACHE does not affect cache update of mem */
    /* Following will preserve LTID, DFID if entry was already valid in cache */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit,
                                   ACC_FOR_SER,
                                   sid,
                                   pt_dyn_info,
                                   entry_words, /* km_entry */
                                   TRUE,        /* Cache_valid_set */
                                   BCMPTM_LTID_IREQ, /* Note */
                                   BCMPTM_DFID_IREQ), /* Note */
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

    if (lt_ireq_op_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_ireq_op_cb[unit](unit, req_ltid, 0xFFFFFFFF, sid,
                                 (bcmbd_pt_dyn_info_t *)pt_dyn_info));
    }
exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    if (SHR_FUNC_ERR()) {
        bcmptm_lt_stat_increment(unit, req_ltid,
                                 TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
    }
    SHR_FUNC_EXIT();
}

/* No check for CCI dispatch.
 * Will NOT check if SID is TCAM and hence no TCAM(k,m) to (x,y) conversion.
 */
int
bcmptm_ireq_write_cci(int unit,
                      bool acc_for_ser,
                      uint64_t flags,
                      bcmdrd_sid_t sid,
                      void *in_pt_dyn_info,
                      void *in_pt_ovrr_info,
                      uint32_t *entry_words,
                      bcmltd_sid_t req_ltid,

                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    bool have_ireq_lock = FALSE;
    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);

    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    *rsp_ltid = req_ltid;

    /* Now PTM is executing this cmd, so perform all checks */
    SHR_IF_ERR_EXIT(
        ireq_pt_check(unit, sid, pt_dyn_info, flags, "ireq_write_cci"));

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;

    /* Issue write to HW */
    if (bcmdrd_pt_is_readonly(unit, sid)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "sid %d, pt %s, attempting WRITE for"
                        " PT with read_only fields on Interactive path\n"),
             sid, bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_IF_ERR_EXIT(
        bcmbd_pt_write(unit,
                       sid,
                       pt_dyn_info,
                       in_pt_ovrr_info,
                       entry_words));
    /* don't update ptcache if HW write failed */

    /* Update cache (if it exists for this pt) */
    /* Note: DONT_USE_CACHE does not affect cache update of mem */
    /* Following will preserve LTID, DFID if entry was already valid in cache */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit, acc_for_ser,
                                   sid,
                                   pt_dyn_info,
                                   entry_words, /* km_entry */
                                   TRUE,        /* Cache_valid_set */
                                   BCMPTM_LTID_IREQ, /* Note */
                                   BCMPTM_DFID_IREQ), /* Note */
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ireq_hash_lt(int unit,
                    uint64_t flags,
                    bcmdrd_sid_t sid,
                    void *in_pt_dyn_info,
                    void *in_pt_ovrr_hash_info,
                    uint32_t *entry_key_words,
                    uint32_t *entry_data_words,
                    bcmptm_op_type_t req_op,
                    size_t rsp_entry_buf_wsize,
                    bcmltd_sid_t req_ltid,

                    uint32_t *rsp_entry_key_words,
                    uint32_t *rsp_entry_data_words,
                    uint32_t *rsp_entry_index,
                    bcmltd_sid_t *rsp_ltid,
                    uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_hash_info_t *pt_dyn_info;
    uint32_t tmp_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    bcmbd_tbl_resp_info_t bd_rsp_info;
    bcmptm_rsp_info_t rsp_info;
    int bd_rv = SHR_E_NONE, retry_count = 0;
    unsigned int i;
    bool have_ireq_lock = FALSE;
    const char *op_name;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
    /* SHR_NULL_CHECK(in_pt_ovrr_info, SHR_E_PARAM); ok to be null */
    SHR_NULL_CHECK(entry_key_words, SHR_E_PARAM);

    SHR_NULL_CHECK(rsp_entry_key_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_data_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_index, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(bcmdrd_pt_attr_is_hashed(unit, sid)? SHR_E_NONE : SHR_E_PARAM);
    SHR_IF_ERR_EXIT(!bcmdrd_pt_is_valid(unit, sid) ? SHR_E_PARAM : SHR_E_NONE);

    /* Take ireq_mutex for non counter tables */
    SHR_IF_ERR_EXIT(bcmptm_ireq_lock(unit));
    have_ireq_lock = TRUE;

    *rsp_ltid = BCMPTM_LTID_RSP_HW;

    pt_dyn_info = (bcmbd_pt_dyn_hash_info_t *)in_pt_dyn_info;

    *rsp_flags = 0; /* default */
    *rsp_entry_index = 0xFFFFFFFF; /* default */

    sal_memset(&rsp_info, 0, sizeof(bcmptm_rsp_info_t)); /* default */
    do {
        switch (req_op) {
            case BCMPTM_OP_LOOKUP:
                bd_rv = bcmbd_pt_lookup(unit,
                                        sid,
                                        pt_dyn_info,
                                        in_pt_ovrr_hash_info,
                                        entry_key_words,
                                        rsp_entry_buf_wsize,

                                        &bd_rsp_info,
                                        rsp_entry_data_words);
                break; /* LOOKUP */

            case BCMPTM_OP_DELETE:
                bd_rv = bcmbd_pt_delete(unit,
                                        sid,
                                        pt_dyn_info,
                                        in_pt_ovrr_hash_info,
                                        entry_key_words,
                                        rsp_entry_buf_wsize,

                                        &bd_rsp_info,
                                        rsp_entry_data_words);
                /* Note: Entry is not deleted from cache - rsrc_mgr must be invoked
                 *       from interactive thread to keep PTcache consistent - this
                 *       may collide with rsrc_mgr running in modeled thread */
                break; /* DELETE */

            case BCMPTM_OP_INSERT:
                SHR_NULL_CHECK(entry_data_words, SHR_E_PARAM);
                SHR_IF_ERR_EXIT(BCMPTM_MAX_PT_ENTRY_WORDS < rsp_entry_buf_wsize?
                                SHR_E_INTERNAL : SHR_E_NONE);
                for (i = 0; i < rsp_entry_buf_wsize; i++) {
                    tmp_entry_words[i] = entry_key_words[i] | entry_data_words[i];
                }
                bd_rv = bcmbd_pt_insert(unit,
                                        sid,
                                        pt_dyn_info,
                                        in_pt_ovrr_hash_info,
                                        tmp_entry_words,
                                        rsp_entry_buf_wsize,

                                        &bd_rsp_info,
                                        rsp_entry_data_words);
                /* Note: Entry is not inserted in cache - rsrc_mgr must be invoked
                 *       from interactive thread to keep PTcache consistent - this
                 *       may collide with rsrc_mgr running in modeled thread */
                break; /* INSERT */

            default: /* ill */
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        } /* req_op */

        /* Done */
        if (bd_rv == SHR_E_NONE) {
            SHR_IF_ERR_EXIT(
                bcmptm_rsp_info_get(unit, req_op, &bd_rsp_info, &rsp_info));
            break; /* from while loop */
        }

        switch (req_op) { /* can be here only for lookup, del, ins */
        case BCMPTM_OP_LOOKUP:
            op_name = "LOOKUP";
            break;
        case BCMPTM_OP_DELETE:
            op_name = "DELETE";
            break;
        default: /* BCMPTM_OP_INSERT */
            op_name = "INSERT";
            break;
        }
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "ireq_hash_lt failed for sid=%s(%d), "
                        "op=%s, tbl_inst=%d, bank=0x%x, flags=0x%" PRIx64 ", "
                        "pt_is_valid=%d, bd_rv=%d\n"),
             bcmdrd_pt_sid_to_name(unit, sid), sid, op_name,
             pt_dyn_info->tbl_inst, pt_dyn_info->bank, flags,
             bcmdrd_pt_is_valid(unit, sid) ? 1 : 0, bd_rv));

        /* Exit if any other error besides SER error */
        SHR_IF_ERR_EXIT(!BCMPTM_SHR_FAILURE_SER(bd_rv) ? bd_rv : SHR_E_NONE);

        /* Interpret rsp_info from hw */
        SHR_IF_ERR_EXIT(
            bcmptm_rsp_info_get(unit, req_op, &bd_rsp_info, &rsp_info));
        if (rsp_info.ser_retry && (retry_count == 0)) {
            if (bcmptm_pt_ser_resp(unit, sid) == BCMDRD_SER_RESP_NONE) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Will not retry failed ireq_hash_lt\n")));
                break; /* from while loop with retry_count = 0 */
            }
        }
        if (rsp_info.ser_retry &&
            (retry_count < BCMPTM_CFG_IREQ_SERC_RETRY_COUNT)) {
            BCMPTM_DO_IREQ_SER_RETRY_PREP;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Will retry failed ireq_hash_lt\n")));
        } else {
            break; /* from while loop */
        }
    } while (1);

    if (SHR_FAILURE(bd_rv)) {
        if (rsp_info.ser_retry) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "ireq_hash_lt failed (after %d attempts) !!\n"),
                 retry_count));
        }
        SHR_ERR_EXIT(bd_rv);
    }

    *rsp_entry_index = rsp_info.entry_index;
    sal_memcpy(rsp_entry_key_words, /* dst */
               rsp_entry_data_words, /* src */
               rsp_entry_buf_wsize*4);
    if (rsp_info.replaced) {
        *rsp_flags |= BCMPTM_RSP_FLAGS_REPLACED;
        /* Note:
         * We can, instead, return SHR_E_EXISTS in this case like SDK6. But,
         * this approach does not scale - may have to invent more return
         * codes for different cases.
         *
         * Dont make rsp_info_get do it - else we will not get chance to set
         * *rsp_entry_index, *rsp_flags, copy rsp_entry_data_words to
         * rsp_entry_key_words.
         */
    }

exit:
    if (have_ireq_lock) {
        (void)(bcmptm_ireq_unlock(unit));
    }
    SHR_FUNC_EXIT();
}

void
bcmptm_pt_status_ireq_hash_lt_register(int unit,
                          bcmptm_pt_status_ireq_hash_lt_cb pt_status_cb)
{
    lt_ireq_hash_cb[unit] = pt_status_cb;
}

void
bcmptm_pt_status_ireq_op_register(int unit,
                          bcmptm_pt_status_ireq_op_cb pt_status_cb)
{
    lt_ireq_op_cb[unit] = pt_status_cb;
}

int
bcmptm_ltm_ireq_read(int unit,
                     uint64_t flags,
                     bcmdrd_sid_t sid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,
                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          flags,
                          sid,
                          pt_dyn_info,
                          pt_ovrr_info,
                          rsp_entry_wsize,
                          req_ltid,
                          rsp_entry_words,
                          rsp_ltid,
                          rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ltm_ireq_write(int unit,
                      uint64_t flags,
                      bcmdrd_sid_t sid,
                      void *pt_dyn_info,
                      void *pt_ovrr_info,
                      uint32_t *entry_words,
                      bcmltd_sid_t req_ltid,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           flags,
                           sid,
                           pt_dyn_info,
                           pt_ovrr_info,
                           entry_words,
                           req_ltid,
                           rsp_ltid,
                           rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ltm_ireq_hash_lt(int unit,
                        uint64_t flags,
                        bcmdrd_sid_t sid,
                        void *pt_dyn_info,
                        void *pt_ovrr_info,
                        uint32_t *entry_key_words,
                        uint32_t *entry_data_words,
                        bcmptm_op_type_t req_op,
                        size_t rsp_entry_buf_wsize,
                        bcmltd_sid_t req_ltid,
                        uint32_t *rsp_entry_key_words,
                        uint32_t *rsp_entry_data_words,
                        uint32_t *rsp_entry_index,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);

    bcmptm_lt_stat_increment(unit, req_ltid,
                             TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_hash_lt(unit,
                             flags,
                             sid,
                             pt_dyn_info,
                             pt_ovrr_info,
                             entry_key_words,
                             entry_data_words,
                             req_op,
                             rsp_entry_buf_wsize,
                             req_ltid,
                             rsp_entry_key_words,
                             rsp_entry_data_words,
                             rsp_entry_index,
                             rsp_ltid,
                             rsp_flags));

    if (lt_ireq_hash_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_ireq_hash_cb[unit](unit, req_ltid, 0xFFFFFFFF, sid,
                                   *rsp_entry_index,
                                   (bcmbd_pt_dyn_hash_info_t *) pt_dyn_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_lt_stat_increment(unit, req_ltid,
                                 TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
    }
    SHR_FUNC_EXIT();
}
