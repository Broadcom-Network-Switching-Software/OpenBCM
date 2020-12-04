/*! \file bcmptm_ptcache_upgrade.c
 *
 * ISSU utilities for PTcache.
 *
 * This file implements ISSU functions related to PTcache (SW cache).
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
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_util.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/generated/bcmptm_ptcache_local.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CACHE
#define COPY 1

#define SINFO_RSVD_INDEX_SEDS_COUNT_WORD -4
    /* word in sinfo_array to store num of words in seds section of seds_cwme */

#define SINFO_RSVD_INDEX_CWME_COUNT_WORD -3
    /* word in sinfo_array to store num of cwme elements */


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 * - These are local to this file.
 */
static bcmptm_ptcache_sinfo_t *sinfo_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmptm_ptcache_pt_group_info_t *pt_group_info_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmptm_ptcache_pt_group_ptrs_t *pt_group_ptrs_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static uint32_t *seds_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmptm_ptcache_cwme_t *cwme_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmptm_ptcache_issu_ctl_t *issu_ctl_p[BCMDRD_CONFIG_MAX_UNITS];
static bool ltid_size16b[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

/* Will free PTcache.Heap */
static int
ptcache_copy_free(int u)
{
    uint32_t pt_group, pt_group_count = 0;
    const bcmdrd_sid_t *pt_list;
    SHR_FUNC_ENTER(u);

    SHR_FREE(sinfo_bp[u][COPY]);
    SHR_FREE(pt_group_info_bp[u][COPY]);
    SHR_IF_ERR_MSG_EXIT(
        bcmptm_pt_group_info_get(u, BCMPTM_PT_GROUP_DEFAULT,
                                 &pt_group_count, NULL, &pt_list),
        (BSL_META_U(u, "pt_group_info_get failed (group_count = %0d)\n"),
         pt_group_count));
    if (pt_group_ptrs_bp[u][COPY]) {
        pt_group = pt_group_count - 1;
        do {
            bcmptm_ptcache_pt_group_ptrs_t *pt_group_ptrs_pcopy;
            pt_group_ptrs_pcopy = pt_group_ptrs_bp[u][COPY] + pt_group;
            SHR_FREE(pt_group_ptrs_pcopy->vinfo_bp);
            SHR_FREE(pt_group_ptrs_pcopy->ltid_seg_base_ptr);
            if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
                break; /* from do-while */
            } else {
                pt_group--;
            }
        } while (1); /* foreach pt_group */
    }
    SHR_FREE(pt_group_ptrs_bp[u][COPY]);
    SHR_FREE(seds_bp[u][COPY]);
    SHR_FREE(cwme_bp[u][COPY]);

exit:
    SHR_FUNC_EXIT();
}

/* Will copy PTcache.HA to PTcache.Heap */
static int
ptcache_copy_create(int u)
{
    size_t sid_count = 0;
    uint32_t req_size, *alloc_p;
    uint32_t seds_count, cwme_count;
    uint32_t pt_group_count = 0, pt_group = 0, vinfo_word_count = 0;
    const bcmdrd_sid_t *pt_list;
    char alloc_str_id[19];
    int s = 0;
    SHR_FUNC_ENTER(u);

    /* To avoid freeing up of unallocated mem - in case of errors */
    sinfo_bp[u][COPY] = NULL;
    pt_group_info_bp[u][COPY] = NULL;
    pt_group_ptrs_bp[u][COPY] = NULL;
    seds_bp[u][COPY] = NULL;
    cwme_bp[u][COPY] = NULL;

    /* Basic info */
    SHR_IF_ERR_EXIT(bcmptm_pt_sid_count_get(u, &sid_count));

    /* Alloc space to store copy of sinfo array.
     * Copy simplification: No space allocated for reserved words. */
    req_size = sid_count * sizeof(bcmptm_ptcache_sinfo_t);
    alloc_p = sal_alloc(req_size, "bcmptmPtcacheCopySinfo");
    SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
    sal_memcpy(alloc_p, sinfo_bp[u][s], req_size);
    sinfo_bp[u][COPY] = (bcmptm_ptcache_sinfo_t *)alloc_p;

    /* Alloc space to store copy of pt_group_info array.
     * Copy simplification: No space allocated for signature word. */
    SHR_IF_ERR_MSG_EXIT(
        bcmptm_pt_group_info_get(u, BCMPTM_PT_GROUP_DEFAULT,
                                 &pt_group_count, NULL, &pt_list),
        (BSL_META_U(u, "pt_group_info_get failed (group_count = %0d)\n"),
         pt_group_count));
    req_size = pt_group_count * sizeof(bcmptm_ptcache_pt_group_info_t);
    alloc_p = sal_alloc(req_size, "bcmptmPtcacheCopyPtGroupInfo");
    SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
    sal_memcpy(alloc_p, pt_group_info_bp[u][s], req_size);
    pt_group_info_bp[u][COPY] = (bcmptm_ptcache_pt_group_info_t *)alloc_p;

    /* Alloc space to store vinfo_bp, ltid_seg_base_ptr for all pt_groups.
     * Note: This is NOT a COPY - but should point to copies of vinfo array,
     *       ltid_seg created in Heap. */
    req_size = pt_group_count * sizeof(bcmptm_ptcache_pt_group_ptrs_t);
    alloc_p = sal_alloc(req_size, "bcmptmPtcachePtGroupPtrs");
    SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
    sal_memset(alloc_p, 0, req_size);
    pt_group_ptrs_bp[u][COPY] = (bcmptm_ptcache_pt_group_ptrs_t *)alloc_p;

    /* Improvement::
     * Allocate vinfo array, ltid_seg - BUT ONLY for PT_group(s) where change is
     * expected. This will reduce amount of Heap memory needed for upgrade func.
     */
    for (pt_group = 0; pt_group < pt_group_count; pt_group++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Creating PTcache.copy for pt_group=%0d\n"),
             pt_group));

        bcmptm_ptcache_pt_group_info_t *c_pt_group_info_p;
        bcmptm_ptcache_pt_group_ptrs_t *c_pt_group_ptrs_p,
                                       *pt_group_ptrs_pcopy;

        c_pt_group_info_p= pt_group_info_bp[u][s] + pt_group;
        c_pt_group_ptrs_p= pt_group_ptrs_bp[u][s] + pt_group;
        pt_group_ptrs_pcopy = pt_group_ptrs_bp[u][COPY] + pt_group;

        /* Size for vinfo array */
        vinfo_word_count = c_pt_group_info_p->valid_seg_word_count +
                           c_pt_group_info_p->data_seg_word_count +
                           c_pt_group_info_p->dfid_seg_word_count +
                           c_pt_group_info_p->info_seg_word_count +
                           c_pt_group_info_p->tc_seg_word_count;
        req_size = vinfo_word_count * sizeof(uint32_t); /* no sign_word */

        /* Alloc space and store copy of vinfo array for this group */
        if (req_size) {
            (void)sal_snprintf(alloc_str_id, 19, "%s%03d",
                               "bcmptmCacheVinf", pt_group);
            alloc_p = sal_alloc(req_size, alloc_str_id);
            SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
            sal_memcpy(alloc_p, c_pt_group_ptrs_p->vinfo_bp, req_size);
            pt_group_ptrs_pcopy->vinfo_bp = alloc_p;
        } else {
            pt_group_ptrs_pcopy->vinfo_bp = NULL;
        }

        /* Allocate space and store ltid seg for this group */
        req_size = c_pt_group_info_p->ltid_seg_word_count *
                   (ltid_size16b[u] ? sizeof(uint16_t) : sizeof(uint32_t));
                   /* no sign_word */
        if (req_size) {
            (void)sal_snprintf(alloc_str_id, 19, "%s%03d",
                               "bcmptmCacheLtid", pt_group);
            alloc_p = sal_alloc(req_size, alloc_str_id);
            SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
            sal_memcpy(alloc_p, c_pt_group_ptrs_p->ltid_seg_base_ptr,
                       req_size);
            pt_group_ptrs_pcopy->ltid_seg_base_ptr = alloc_p;
        } else {
            pt_group_ptrs_pcopy->ltid_seg_base_ptr = NULL;
        }
    } /* foreach PT group */

    /* Allocate space for seds array - pt_group 0 ONLY */
    seds_count = (sinfo_bp[u][s] + SINFO_RSVD_INDEX_SEDS_COUNT_WORD)->cw_index;
    req_size = seds_count * sizeof(uint32_t);
    alloc_p = sal_alloc(req_size, "bcmptmCacheSedsCopy");
    SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
    sal_memcpy(alloc_p, seds_bp[u][s], req_size);
    seds_bp[u][COPY] = alloc_p;

    /* Allocate space for cwme array - pt_group 0 ONLY */
    cwme_count = (sinfo_bp[u][s] +
                  SINFO_RSVD_INDEX_CWME_COUNT_WORD)->cw_index;
    req_size = cwme_count * sizeof(bcmptm_ptcache_cwme_t);
    alloc_p = sal_alloc(req_size, "bcmptmCacheCwmeCopy");
    SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
    sal_memcpy(alloc_p, cwme_bp[u][s], req_size);
    cwme_bp[u][COPY] = (bcmptm_ptcache_cwme_t *)alloc_p;

exit:
    SHR_FUNC_EXIT();
}

static int
sinfo_compare(int u, bcmdrd_sid_t sid, bcmptm_ptcache_sinfo_t *sinfo_p,
              bcmptm_ptcache_sinfo_t *c_sinfo_p,
              bool *mismatch)
{
    int tmp_rv;

    /* default */
    if (mismatch) {
        *mismatch = FALSE;
    }
    tmp_rv = SHR_E_NONE;

    if (sinfo_p->index_en != c_sinfo_p->index_en
        || sinfo_p->tbl_inst_en != c_sinfo_p->tbl_inst_en
        || sinfo_p->ptcache_type != c_sinfo_p->ptcache_type
        /* || sinfo_p->cw_index != c_sinfo_p->cw_index */
       ) {

        if (sinfo_p->index_en == c_sinfo_p->index_en
            && sinfo_p->tbl_inst_en == c_sinfo_p->tbl_inst_en
            && sinfo_p->ptcache_type == BCMPTM_PTCACHE_TYPE_SE_NOT_VALID
            && c_sinfo_p->ptcache_type == BCMPTM_PTCACHE_TYPE_SE_VALID) {
            /* This is not a real issue. */
            return tmp_rv;
        }
        if (sinfo_p->ptcache_type == BCMPTM_PTCACHE_TYPE_NO_CACHE
            && c_sinfo_p->ptcache_type > BCMPTM_PTCACHE_TYPE_NO_CACHE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "sinfo.mismatch for sid=%s(%0d): (index_en, "
                            "tbl_inst_en, ptcache_type) = new (%0d, %0d, %0d, %0d),"
                            "old (%0d, %0d, %0d, %0d). PTCACHE_TYPE became "
                            "NO_CACHE\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid,
                 sinfo_p->index_en, sinfo_p->tbl_inst_en,
                 sinfo_p->ptcache_type, sinfo_p->cw_index,
                 c_sinfo_p->index_en, c_sinfo_p->tbl_inst_en,
                 c_sinfo_p->ptcache_type, c_sinfo_p->cw_index));
            /* This is also not a real issue. Some PT became not cacheable.*/
            return tmp_rv;
        }
        LOG_VERBOSE(BSL_LOG_MODULE, /* WARN */
            (BSL_META_U(u, "sinfo.mismatch for sid=%s(%0d): (index_en, "
                        "tbl_inst_en, ptcache_type) = new (%0d, %0d, %0d, %0d),"
                        "old (%0d, %0d, %0d, %0d)\n"),
             bcmdrd_pt_sid_to_name(u, sid), sid,
             sinfo_p->index_en, sinfo_p->tbl_inst_en,
             sinfo_p->ptcache_type, sinfo_p->cw_index,
             c_sinfo_p->index_en, c_sinfo_p->tbl_inst_en,
             c_sinfo_p->ptcache_type, c_sinfo_p->cw_index));
        if (mismatch) {
            *mismatch = TRUE;
        }
    }

    /* always */
    return tmp_rv;
}

static int
xinfo_compare(int u, bcmdrd_sid_t sid, bcmptm_ptcache_xinfo_t *xinfo,
              bcmptm_ptcache_xinfo_t *c_xinfo,
              bool *mismatch)
{
    int tmp_rv;

    /* default */
    if (mismatch) {
        *mismatch = FALSE;
    }
    tmp_rv = SHR_E_NONE;

    if (xinfo->data_seg_en != c_xinfo->data_seg_en ||
        xinfo->dfid_seg_en != c_xinfo->dfid_seg_en ||
        xinfo->info_seg_en != c_xinfo->info_seg_en ||
        xinfo->tc_seg_en != c_xinfo->tc_seg_en ||
        xinfo->ltid_seg_en != c_xinfo->ltid_seg_en ||
        xinfo->overlay_mode != c_xinfo->overlay_mode ||
        xinfo->num_we_field != c_xinfo->num_we_field ||
        xinfo->pt_group != c_xinfo->pt_group) {
        LOG_VERBOSE(BSL_LOG_MODULE, /* WARN */
            (BSL_META_U(u, "xinfo.mismatch for sid=%s(%0d)\n"),
             bcmdrd_pt_sid_to_name(u, sid), sid));
        if (mismatch) {
            *mismatch = TRUE;
        }
    }

    /* always */
    return tmp_rv;
}

/* Assumption:
 *
 * Prior to calling this function the caller has already executed
 * cwme_vinfo_init_group for this pt_group with skip_vinfo_ltid=False.
 * Meaning, HA mem for vinfo array, ltid_seg has been reallocated and
 * cwme_vinfo_init_group has filled these with coldboot values of sentinels,
 * overlay_info words, reset values, key, mask field info words,
 * cci_map_id, etc.
 *
 * Also, changes to ltid due to ISSU are already done in old.PTcache.HA
 * - and these have been copied into PTcache.Heap as part of this upgrade
 * function.
 *
 * What remains? Copy the following from PTcache.Heap to PTcache.HA:
 *     valid bits, data words, dfid, ltid, tcs
 */
static int
ptcache_vinfo_ltid_copy(int u, uint32_t pt_group)
{
    bcmdrd_sid_t sid = BCMDRD_INVALID_ID;
    const bcmdrd_sid_t *pt_list = NULL;
    int s = 0, tmp_rv;
    uint32_t pt_count = 0, i, inst_index_count, total_index_count,
             word_count, tbl_inst_count = 0, inst,
             *vinfo_bp = NULL, *c_vinfo_bp, *vinfo_p, *c_vinfo_p;
    void *ltid_seg_base_ptr, *c_ltid_seg_base_ptr;
    bcmptm_ptcache_sinfo_t *sinfo_p, *c_sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo = NULL, *c_xinfo = NULL;
    bcmptm_ptcache_cwme_t *cwme_p = NULL, *c_cwme_p = NULL;
    /* bcmptm_ptcache_pt_group_info_t *pt_group_info_p, *c_pt_group_info_p; */
    bcmptm_ptcache_pt_group_ptrs_t *pt_group_ptrs_p, *c_pt_group_ptrs_p;
    bool mismatch;
    SHR_FUNC_ENTER(u);

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "Running vinfo_ltid_copy for pt_group=%0d\n"),
         pt_group));

    /* Determine PTs in this group */
    if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
        /* PT sizes are fixed */
        size_t all_sid_count = 0;
        SHR_IF_ERR_EXIT(bcmptm_pt_sid_count_get(u, &all_sid_count));
        pt_count = (uint32_t)all_sid_count;
    } else {
        tmp_rv = bcmptm_pt_group_info_get(u, pt_group, &pt_count, NULL,
                                          &pt_list);
        SHR_IF_ERR_MSG_EXIT(
            ((tmp_rv != SHR_E_NONE) || !pt_count) ? SHR_E_INTERNAL : SHR_E_NONE,
            (BSL_META_U(u, "group_info_get returned rv = %0d, "
                        "pt_count = %0d for group_id = %0d\n"),
             tmp_rv, pt_count, pt_group));
    }

    for (i = 0; i < pt_count; i++) {
        if (pt_group != BCMPTM_PT_GROUP_DEFAULT) {
            sid = pt_list[i];
        } else {
            sid = i;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "\n\nNow working on SID=%s(%0d)...............\n"),
             bcmdrd_pt_sid_to_name(u, sid), sid));

        sinfo_p = sinfo_bp[u][s] + sid;
        c_sinfo_p = sinfo_bp[u][COPY] + sid;

        /* ptcache_diff */
        SHR_IF_ERR_EXIT(sinfo_compare(u, sid, sinfo_p, c_sinfo_p, NULL));

        if (sinfo_p->tbl_inst_en && c_sinfo_p->tbl_inst_en) {
            SHR_IF_ERR_EXIT(
                xinfo_compare(u, sid, xinfo, c_xinfo, NULL));
        }

        switch (sinfo_p->ptcache_type) { /* PTcache.HA */
            case BCMPTM_PTCACHE_TYPE_CCI_ONLY:
                /* As CCI was WB, filling of cci_map_id in sinfo.cw_index
                 * was skipped during cwme_vinfo_init. So copy from
                 * PTcache.Heap */
                sinfo_p->cw_index = c_sinfo_p->cw_index;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Copied cci_map_id for CCI_ONLY\n")));
                break;

            case BCMPTM_PTCACHE_TYPE_ME_CCI:
            case BCMPTM_PTCACHE_TYPE_ME:
                /* pt_group_info_p = pt_group_info_bp[u][s] + pt_group; */
                pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + pt_group;

                /* c_pt_group_info_p = pt_group_info_bp[u][COPY] + pt_group; */
                c_pt_group_ptrs_p = pt_group_ptrs_bp[u][COPY] + pt_group;

                if (sinfo_p->tbl_inst_en && !c_sinfo_p->tbl_inst_en) {
                    /* tbl_inst_en has changed, as acc_type is different */

                    if (c_sinfo_p->ptcache_type ==
                        BCMPTM_PTCACHE_TYPE_SE_VALID ||
                        c_sinfo_p->ptcache_type ==
                        BCMPTM_PTCACHE_TYPE_SE_NOT_VALID) {

                        /* SE.acc_type change */
                        LOG_VERBOSE(BSL_LOG_MODULE, /* WARN */
                            (BSL_META_U(u, "SE.acc_type changed from DUP to "
                                    "UNIQUE for sid=%s(%0d)\n"),
                             bcmdrd_pt_sid_to_name(u, sid), sid));

                        /* sinfo_compare */
                        /* tbl_inst_en, ptcache_type are different */
                        SHR_IF_ERR_EXIT(
                            sinfo_p->index_en != c_sinfo_p->index_en ?
                            SHR_E_INTERNAL : SHR_E_NONE);

                        /* Because old_SID was of type SE, so all we had in
                         * PTcache was implicit valid bit, contents of the
                         * entry.
                         *
                         * Also, from looking at sinfo_init, we know that
                         * SE.old_SID cannot be CAM or ctr_with_cfg_fields, or
                         * overlay or have ltid_seg, dfid_seg and belongs to
                         * DEFAULT group.
 */

                        /* Assumptions for this upgrade:
                         * Nothing except for tbl_inst_count has changed.
                         */

                        /* Checks to test our assumption.
                         * (note: there is no c_xinfo)
                         */
                        if (xinfo->info_seg_en || xinfo->tc_seg_en ||
                            xinfo->ltid_seg_en || xinfo->overlay_mode ||
                            xinfo->num_we_field ||
                            xinfo->pt_group != BCMPTM_PT_GROUP_DEFAULT) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(u, "SE_to_ME: Cannot fix xinfo "
                                   "changes for sid=%s(%0d)\n"),
                                 bcmdrd_pt_sid_to_name(u, sid), sid));
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }

                        /* Assume: index_count, index_min etc have not been
                         * introduced.
                         */
                        if (cwme_p->pt_index_count != c_cwme_p->pt_index_count
                            || cwme_p->pt_index_min != c_cwme_p->pt_index_min) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(u, "SE_to_ME.cwme: Cannot support "
                                            "pt_index_count, "
                                            "pt_index_min changes for sid="
                                            "%s(%0d)\n"),
                                 bcmdrd_pt_sid_to_name(u, sid), sid));
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }

                        /* Copy valid bits - must 'pack' valid bits for all
                         * instances in a 32 bit array. If we have 55 instances,
                         * we will need two 32 bit words.
                         */
                        if (c_sinfo_p->ptcache_type ==
                            BCMPTM_PTCACHE_TYPE_SE_VALID) {
                            uint32_t word, word_count;

                            /* Where we need to write valid bits */
                            vinfo_p = vinfo_bp + cwme_p->vs_base;

                            /* Total number of valid bits to be set */
                            total_index_count = cwme_p->tbl_inst_max + 1;

                            word_count = BCMPTM_BITS2WORDS(total_index_count);

                            /* we need to set all these bits to 1 - including
                             * the last word which may have < 32 valid bits and
                             * thus some extra bits..
                             *
                             * Following will set even those extra bits in last
                             * word - but because these should never be
                             * referenced, we should be ok. */
                            for (word = 0; word < word_count; word++) {
                                *vinfo_p = 0xFFFFFFFF;
                                vinfo_p++;
                            }
                        }
                        /* else
                         *  - cwme_init_info has already set valid bits to 0
                         */

                        /* Copy data bits */
                        vinfo_p = vinfo_bp + cwme_p->ds_base;
                        c_vinfo_p = seds_bp[u][COPY] + c_sinfo_p->cw_index,
                        word_count = bcmdrd_pt_entry_wsize(u, sid);
                        for (inst = 0; inst < tbl_inst_count; inst ++) {
                            sal_memcpy(vinfo_p, c_vinfo_p, 4 * word_count);
                            vinfo_p += word_count; /* dst */
                        }
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(u, "Finished fixing SE.acc_type change "
                                        "for sid=%0d\n"),
                             sid));

                    } else { /* ME.acc_type change */
                        LOG_VERBOSE(BSL_LOG_MODULE, /* WARN */
                            (BSL_META_U(u, "ME.acc_type changed from DUP to "
                               "UNIQUE for sid=%s(%0d)\n"),
                             bcmdrd_pt_sid_to_name(u, sid), sid));

                        /* sinfo_compare */
                        SHR_IF_ERR_EXIT(
                            sinfo_p->ptcache_type != c_sinfo_p->ptcache_type ?
                            SHR_E_INTERNAL : SHR_E_NONE);
                        SHR_IF_ERR_EXIT(
                            sinfo_p->index_en != c_sinfo_p->index_en ?
                            SHR_E_INTERNAL : SHR_E_NONE);

                        cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
                        c_cwme_p = cwme_bp[u][COPY] + c_sinfo_p->cw_index;

                        /* Assume:
                         * index_count, tbl_inst_count, etc have not changed
                         */
                        if (cwme_p->pt_index_count != c_cwme_p->pt_index_count
                            || cwme_p->pt_index_min != c_cwme_p->pt_index_min) {
                            /* cwme_p->tbl_inst_max != * c_cwme_p->tbl_inst_max) */
                            LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(u, "cwme.mismatch for sid=%s(%0d)"
                                            "\n"),
                                 bcmdrd_pt_sid_to_name(u, sid), sid));
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }

                        inst_index_count = cwme_p->pt_index_count; /* one inst */
                        if (sinfo_p->tbl_inst_en) {
                            tbl_inst_count = cwme_p->tbl_inst_max + 1;
                        } else {
                            tbl_inst_count = 1;
                        }
                        total_index_count = inst_index_count * tbl_inst_count;

                        xinfo = &cwme_p->xinfo;
                        c_xinfo = &c_cwme_p->xinfo;

                        SHR_IF_ERR_EXIT(
                            (pt_group != xinfo->pt_group ||
                             pt_group != c_xinfo->pt_group) ? SHR_E_INTERNAL
                                                            : SHR_E_NONE);

                        /* Improvements:
                         *  - pt_group_info_compare() - must compare counts ???
                         *  - Other checks?
                         */
                        SHR_IF_ERR_EXIT(
                            xinfo_compare(u, sid, xinfo, c_xinfo,
                                          &mismatch));
                        SHR_IF_ERR_EXIT(mismatch ? SHR_E_INTERNAL : SHR_E_NONE);

                        vinfo_bp = pt_group_ptrs_p->vinfo_bp;
                        ltid_seg_base_ptr = pt_group_ptrs_p->ltid_seg_base_ptr;

                        c_vinfo_bp = c_pt_group_ptrs_p->vinfo_bp;
                        c_ltid_seg_base_ptr =
                            c_pt_group_ptrs_p->ltid_seg_base_ptr;

                        /* Note:
                         *     word_counts stored in pt_group_info_p represent
                         *     total_word_count for all PTs in these segments -
                         *     whereas we are copying these segments one PT at a
                         *     time - so we cannot use the word_counts stored
                         *     in pt_group_info_p.
                         */

                        /* Copy valid seg */
                        vinfo_p = vinfo_bp + cwme_p->vs_base;
                        c_vinfo_p = c_vinfo_bp + c_cwme_p->vs_base;
                        if (inst_index_count % 32) { /* cannot use sal_memcpy */
                            /* PTcache.Heap has valid bit array for one
                             * instance stored in a 32 bit array. Entire valid
                             * bit array can be viewed as one 'field' of width
                             * 'inst_index_count' bits and the 32 bit array as
                             * 'field_buf' which holds value of this 'field'.
                             *
                             * PTcache.HA needs to have valid bit arrays for all
                             * instances 'packed' into a 32 bit array (so, we
                             * cannot use sal_memcpy). We can, however, view
                             * 'packed' arrays in PTcache.HA as 'one wide memory
                             * entry' which has same 'field' value concatenated
                             * multiple times.
                             *
                             * Summary:
                             *      stuff same 'field_buf' into 'memory_entry'
                             *      at different offsets.
                             */
                            uint32_t dst_sbit = 0,
                                     dst_ebit = inst_index_count - 1;
                            for (inst = 0; inst < tbl_inst_count; inst ++) {
                                bcmdrd_field_set(
                                    vinfo_p,    /* ent_buf */
                                    dst_sbit,   /* in ent_buf */
                                    dst_ebit,   /* in ent_buf */
                                    c_vinfo_p); /* field_buf */
                                dst_sbit += inst_index_count;
                                dst_ebit += inst_index_count;
                                /*
                                 * Example: for inst_index_count 5, for loop:
                                 *
                                 * inst = 0, src_ebit = 4, sbit = 0
                                 *           dst_ebit = 4, sbit = 0
                                 *
                                 * inst = 1, src_ebit = 4, sbit = 0
                                 *           dst_ebit = 9, sbit = 5
                                 *
                                 * inst = 2, src_ebit =  4, sbit =  0
                                 *           dst_ebit = 14, sbit = 10
                                 *
                                 * inst = 3, src_ebit =  4, sbit =  0
                                 *           dst_ebit = 19, sbit = 15
                                 */
                            }
                        } else {
                            /* inst_index_count is multiple of 32 bits, so valid
                             * bit array for each instance starts on a word
                             * boundary. */
                            word_count = BCMPTM_BITS2WORDS(inst_index_count);
                            sal_memcpy(vinfo_p, c_vinfo_p, 4 * word_count);
                            vinfo_p += word_count; /* dst */
                        }

                        /* Copy data seg */
                        if (xinfo->data_seg_en) {
                            vinfo_p = vinfo_bp + cwme_p->ds_base;
                            c_vinfo_p = c_vinfo_bp + c_cwme_p->ds_base;
                            word_count = inst_index_count *
                                         bcmdrd_pt_entry_wsize(u, sid);
                            for (inst = 0; inst < tbl_inst_count; inst ++) {
                                sal_memcpy(vinfo_p, c_vinfo_p, 4 * word_count);
                                vinfo_p += word_count; /* dst */
                            }
                        }

                        /* Copy overlay info words (in data_seg).
                         *
                         * data_seg stores overlay info words when
                         * (!data_seg_en &&
                         *  (xinfo->overlay_mode != BCMPTM_OINFO_MODE_NONE)
                         * )
                         *
                         * But, overlay_info words were already written in vinfo
                         * array during reinit for this pt_group.
                         *
                         * We dont expect these words to change - so we can add
                         * some checks to verify that overlay info words are
                         * same between PTcache.HA and PTcache.Heap
                         *
                         * Note: overlay info words are not on per inst basis
                         *       and not on per SID basis.
                         */

                        /* Copy dfid seg */
                        if (xinfo->dfid_seg_en) {
                            word_count = inst_index_count; /* dfid_count */
                            if (xinfo->dfid_seg_en == 2) { /* dfid_size16b */
                                uint16_t *dfid_p, *c_dfid_p;
                                dfid_p = (uint16_t *)vinfo_bp +
                                    cwme_p->dfs_base;
                                c_dfid_p = (uint16_t *)c_vinfo_bp +
                                    cwme_p->dfs_base;
                                for (inst = 0; inst < tbl_inst_count; inst ++) {
                                    sal_memcpy(dfid_p, c_dfid_p,
                                               2 * word_count); /* 16b dfid */
                                    dfid_p += word_count; /* dfid_count */
                                }
                            } else { /* dfid_size8b */
                                uint8_t *dfid_p, *c_dfid_p;
                                dfid_p = (uint8_t *)vinfo_bp +
                                    cwme_p->dfs_base;
                                c_dfid_p = (uint8_t *)c_vinfo_bp +
                                    cwme_p->dfs_base;
                                for (inst = 0; inst < tbl_inst_count; inst ++) {
                                    sal_memcpy(dfid_p, c_dfid_p,
                                               word_count); /* 8b dfid */
                                    dfid_p += word_count; /* dfid_count */
                                }
                            }
                        }

                        /* Copy info seg
                         * - During reinit of pt_group0, vinfo was already
                         *   populated with key, mask in case of TCAM SIDs.
                         *   So there is no need to copy info_seg from
                         *   PTcache.Heap to PTcache.HA.
                         *
                         *   But as CCI is going through WB, so for ME_CCI
                         *   writing of cci_map_id was skipped, so we must copy
                         *   info_seg PTcache.Heap to PTcache.HA.
                         *
                         * - Also, note that info words are not on per instance basis
                         *   but on per SID basis.
                         */
                        if (xinfo->info_seg_en) {
                            uint32_t pt_iw_count;
                            if (sinfo_p->ptcache_type ==
                                BCMPTM_PTCACHE_TYPE_ME_CCI) {
                                pt_iw_count = 1;
                                LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(u, "Will copy info_seg for ME_"
                                                "CCI-ME_acc_type changed \n")));
                            } else {
                                SHR_IF_ERR_EXIT(
                                    bcmptm_pt_iw_count(u, sid, &pt_iw_count));
                                SHR_IF_ERR_EXIT(pt_iw_count == 0 ?
                                                SHR_E_INTERNAL : SHR_E_NONE);
                                LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(u, "Will copy info_seg for ME "
                                                "(TCAM SID)-ME_acc_type changed"
                                                "\n")));
                            }
                            sal_memcpy(vinfo_bp + cwme_p->is_base,
                                       c_vinfo_bp + c_cwme_p->is_base,
                                       4 * pt_iw_count);
                        }

                        /* Copy tc seg */
                        /* Like valid_seg - we have one bit per index - and once
                         * again we can do what we did for copying valid bits.
                         * But, at present none of SIDs have tc seg - so exit
                         * with error if we notice a mismatch in tbl_inst_en for
                         * an SID with tc seg.
                         */
                        SHR_IF_ERR_EXIT(
                            xinfo->tc_seg_en ? SHR_E_INTERNAL : SHR_E_NONE);

                        /* Copy ltid_seg */
                        if (xinfo->ltid_seg_en) {
                            word_count = inst_index_count; /* ltid_count */
                            if (ltid_size16b[u]) {
                                uint16_t *ltid_p, *c_ltid_p;
                                ltid_p = (uint16_t *)ltid_seg_base_ptr +
                                    cwme_p->ls_base;
                                c_ltid_p = (uint16_t *)c_ltid_seg_base_ptr +
                                    cwme_p->ls_base;
                                for (inst = 0; inst < tbl_inst_count; inst ++) {
                                    sal_memcpy(ltid_p, c_ltid_p,
                                               2 * word_count); /* 16b ltid */
                                    ltid_p += word_count; /* ltid_count */
                                }
                            } else { /* ltid_size32b */
                                uint32_t *ltid_p, *c_ltid_p;
                                ltid_p = (uint32_t *)ltid_seg_base_ptr +
                                    cwme_p->ls_base;
                                c_ltid_p = (uint32_t *)c_ltid_seg_base_ptr +
                                    cwme_p->ls_base;
                                for (inst = 0; inst < tbl_inst_count; inst ++) {
                                    sal_memcpy(ltid_p, c_ltid_p,
                                               4 * word_count); /* 32b_ltid */
                                    ltid_p += word_count; /* ltid_count */
                                }
                            }
                        }
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(u, "finished fixing ME.acc_type change "
                                        "for sid=%0d\n"),
                             sid));
                    } /* ME.acc_type change */

                } else if (!sinfo_p->tbl_inst_en && c_sinfo_p->tbl_inst_en) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Cannot handle acc_type change from "
                                    "UNIQUE to DUP for sid=%s(%0d)\n"),
                         bcmdrd_pt_sid_to_name(u, sid), sid));
                } else { /* no tbl_inst_en change */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "No tbl_inst change - direct copy\n")));

                    /* Acc_type has not changed.
                     * Direct copy PTcache.Heap to
                     * PTcache.HA */

                    /* sinfo_compare */
                    /* we are here because tbl_inst_en is same */
                    SHR_IF_ERR_EXIT(
                        sinfo_p->ptcache_type != c_sinfo_p->ptcache_type ?
                        SHR_E_INTERNAL : SHR_E_NONE);
                    SHR_IF_ERR_EXIT(
                        sinfo_p->index_en != c_sinfo_p->index_en ?
                        SHR_E_INTERNAL : SHR_E_NONE);

                    cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
                    c_cwme_p = cwme_bp[u][COPY] + c_sinfo_p->cw_index;

                    /* Assume:
                     * index_count, tbl_inst_count, etc have not changed
                     */
                    if (cwme_p->pt_index_count != c_cwme_p->pt_index_count ||
                        cwme_p->pt_index_min != c_cwme_p->pt_index_min ||
                        cwme_p->tbl_inst_max != c_cwme_p->tbl_inst_max) {
                        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(u, "Cwme.mismatch for SID=%s(%0d)\n"),
                             bcmdrd_pt_sid_to_name(u, sid), sid));
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }

                    inst_index_count = cwme_p->pt_index_count; /* one inst */
                    if (sinfo_p->tbl_inst_en) {
                        tbl_inst_count = cwme_p->tbl_inst_max + 1;
                    } else {
                        tbl_inst_count = 1;
                    }
                    total_index_count = inst_index_count * tbl_inst_count;

                    xinfo = &cwme_p->xinfo;
                    c_xinfo = &c_cwme_p->xinfo;

                    if (pt_group != xinfo->pt_group ||
                        pt_group != c_xinfo->pt_group) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(u, "Skipping, sid=%0d which belongs to "
                               " pt_group=(%0d, %0d), pt_group%0d is being "
                               "processed now\n"),
                             sid, xinfo->pt_group, c_xinfo->pt_group,
                             pt_group));
                        continue;
                    }
                    /* Improvements:
                     *  - pt_group_info_compare() - must compare counts ???
                     *  - Other checks?
                     */
                    SHR_IF_ERR_EXIT(
                        xinfo_compare(u, sid, xinfo, c_xinfo,
                                      &mismatch));
                    SHR_IF_ERR_EXIT(mismatch ? SHR_E_INTERNAL : SHR_E_NONE);

                    vinfo_bp = pt_group_ptrs_p->vinfo_bp;
                    ltid_seg_base_ptr = pt_group_ptrs_p->ltid_seg_base_ptr;

                    c_vinfo_bp = c_pt_group_ptrs_p->vinfo_bp;
                    c_ltid_seg_base_ptr = c_pt_group_ptrs_p->ltid_seg_base_ptr;

                    /* Copy valid seg */
                    vinfo_p = vinfo_bp + cwme_p->vs_base;
                    c_vinfo_p = c_vinfo_bp + c_cwme_p->vs_base;
                    word_count = BCMPTM_BITS2WORDS(total_index_count);
                    sal_memcpy(vinfo_p, c_vinfo_p, 4 * word_count);

                    /* Copy data seg */
                    if (xinfo->data_seg_en) {
                        vinfo_p = vinfo_bp + cwme_p->ds_base;
                        c_vinfo_p = c_vinfo_bp + c_cwme_p->ds_base;
                        word_count = total_index_count *
                                     bcmdrd_pt_entry_wsize(u, sid);
                        sal_memcpy(vinfo_p, c_vinfo_p, 4 * word_count);
                    }

                    /* Copy overlay info words (in data_seg).
                     * - Not needed as explained earlier.
                     */

                    /* Copy dfid seg */
                    if (xinfo->dfid_seg_en) {
                        word_count = total_index_count; /* dfid_count */
                        if (xinfo->dfid_seg_en == 2) { /* dfid_size16b */
                            uint16_t *dfid_p, *c_dfid_p;
                            dfid_p = (uint16_t *)vinfo_bp +
                                cwme_p->dfs_base;
                            c_dfid_p = (uint16_t *)c_vinfo_bp +
                                cwme_p->dfs_base;
                            sal_memcpy(dfid_p, c_dfid_p,
                                       2 * word_count); /* 16b dfid */
                        } else { /* dfid_size8b */
                            uint8_t *dfid_p, *c_dfid_p;
                            dfid_p = (uint8_t *)vinfo_bp +
                                cwme_p->dfs_base;
                            c_dfid_p = (uint8_t *)c_vinfo_bp +
                                cwme_p->dfs_base;
                            sal_memcpy(dfid_p, c_dfid_p,
                                       word_count); /* 8b dfid */
                        }
                    }

                    /* Copy info seg
                     * Not on per instance basis.
                     */
                    if (xinfo->info_seg_en) {
                        uint32_t pt_iw_count;
                        if (sinfo_p->ptcache_type ==
                            BCMPTM_PTCACHE_TYPE_ME_CCI) {
                            pt_iw_count = 1;
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(u, "Will copy info_seg for ME_CCI "
                                            "- no acc_type change\n")));
                        } else {
                            SHR_IF_ERR_EXIT(
                                bcmptm_pt_iw_count(u, sid, &pt_iw_count));
                            SHR_IF_ERR_EXIT(pt_iw_count == 0 ?
                                            SHR_E_INTERNAL : SHR_E_NONE);
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(u, "Will copy info_seg for ME "
                                            "(TCAM SID) - no acc_type change\n"
                                            )));
                        }
                        sal_memcpy(vinfo_bp + cwme_p->is_base,
                                   c_vinfo_bp + c_cwme_p->is_base,
                                   4 * pt_iw_count);
                    }

                    /* Copy tc seg */
                    if (xinfo->tc_seg_en) {
                        /* Like valid_seg - we have one bit per index. */
                        vinfo_p = vinfo_bp + cwme_p->tcs_base;
                        c_vinfo_p = c_vinfo_bp + c_cwme_p->tcs_base;
                        word_count = BCMPTM_BITS2WORDS(total_index_count);
                        sal_memcpy(vinfo_p, c_vinfo_p, 4 * word_count);
                    }

                    /* Copy ltid_seg */
                    if (xinfo->ltid_seg_en) {
                        word_count = total_index_count; /* ltid_count */
                        if (ltid_size16b[u]) {
                            uint16_t *ltid_p, *c_ltid_p;
                            ltid_p = (uint16_t *)ltid_seg_base_ptr +
                                cwme_p->ls_base;
                            c_ltid_p = (uint16_t *)c_ltid_seg_base_ptr +
                                cwme_p->ls_base;
                            sal_memcpy(ltid_p, c_ltid_p,
                                       2 * word_count); /* 16b ltid */
                        } else { /* ltid_size32b */
                            uint32_t *ltid_p, *c_ltid_p;
                            ltid_p = (uint32_t *)ltid_seg_base_ptr +
                                cwme_p->ls_base;
                            c_ltid_p = (uint32_t *)c_ltid_seg_base_ptr +
                                cwme_p->ls_base;
                            sal_memcpy(ltid_p, c_ltid_p,
                                       4 * word_count); /* 32b ltid */
                        }
                    }

                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Finished copying info for sid=%0d\n"),
                         sid));
                } /* no tbl_inst_en change */
                break;
            case BCMPTM_PTCACHE_TYPE_SE_VALID:
                /* cwme_vinfo_init for this pt_group should have marked all
                 * single entry with BCMPTM_PTCACHE_TYPE_SE_NOT_VALID. Right? */
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "SE_VALID in PTcache.HA for sid=%s(%0d)\n"),
                     bcmdrd_pt_sid_to_name(u, sid), sid));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
                break;


                /*
                 * In general:
                 *      Corresponding entry in seds array should have been
                 *      filled with default regsfile value by cwme_vinfo_init.
                 *      But, it is also possible that this value was changed
                 *      multiple times by user with set_cache_vbit=T, then F.
                 *      Main point, we must not assume that SE_NOT_VALID means
                 *      the contents are default HW value.
                 * But, what if new regsfile has say a new resetvalue for this
                 * PT? In that case, we want to take in that new value, right
                 * and may not want to copy value from previous regsfile?
                 *
                 * For now, we know that there are no regsfile changes for SE
                 * entries, so copy the contents.
                 *
                 * Summary: Copy the contents from PTcache.Heap and also the
                 *          valid bits for both SE, ME entries.
                 */
            case BCMPTM_PTCACHE_TYPE_SE_NOT_VALID:
                /* Entry in PTcache.Heap may be SE_VALID or SE_NOT_VALID.
                 * But just because it is SE_NOT_VALID in PTcache.heap does not
                 * mean that value in PTcache.heap is the 'default reset value'.
                 * RM could have written non-default value with cache_vbit=F.
                 *
                 * As we are not expecting a regsfile reset value change as part
                 * of this upgrade function, we should always overwrite PTcache.HA
                 * with PTcache.Heap value.
                 * (This is exactly what we do for ME entries:
                 *      1. copy contents of data_seg.
                 *      2. also copy valid bits
                 */

                /* Checks */
                SHR_IF_ERR_EXIT(
                    sinfo_p->tbl_inst_en != c_sinfo_p->tbl_inst_en ?
                    SHR_E_INTERNAL : SHR_E_NONE);
                SHR_IF_ERR_EXIT(
                    sinfo_p->index_en != c_sinfo_p->index_en ?
                    SHR_E_INTERNAL : SHR_E_NONE);

                /* Copy valid bit */
                if (c_sinfo_p->ptcache_type == BCMPTM_PTCACHE_TYPE_SE_VALID) {
                    sinfo_p->ptcache_type = BCMPTM_PTCACHE_TYPE_SE_VALID;
                }

                /* Copy contents */
                sal_memcpy(seds_bp[u][s] + sinfo_p->cw_index,
                           seds_bp[u][COPY] + c_sinfo_p->cw_index,
                           4 * bcmdrd_pt_entry_wsize(u, sid));

                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Finished copying info for SE.sid=%0d\n"),
                     sid));
                break;
            default:
                /*
                 * BCMPTM_PTCACHE_TYPE_SE_CACHE0:
                 *      Corresponding 'fake' entry is in seds array and this
                 *      array was cleared during bcmptm_ptcache_reinit.
                 *
                 * BCMPTM_PTCACHE_TYPE_NO_CACHE:
                 *      For this upgrade, we don't service change in cacheability.
                 *
                 * BCMPTM_PTCACHE_TYPE_ME_RSVD:
                 *      Remains reserved.
                 */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "No changes for sid=%0d, ptcache_type=%0d\n"),
                     sid, sinfo_p->ptcache_type));
                break;
        } /* ptcache_type */

    } /* foreach sid */

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "Exited ltid_copy with ERROR, last sid=%s(%0d)\n"),
             bcmdrd_pt_sid_to_name(u, sid), sid));
    }
    SHR_FUNC_EXIT();
}

/* Assumptions:
 * - comp_config of ptcache (and entire bcmptm is already done.
 * - tbl_inst_count has changed only for SIDs in default PT group.
 *
 * Function bcmptm_ptcache_init() was copied and modified for the upgrade
 * function.
 *
 * Differences w.r.t bcmptm_ptcache_init()
 * - Coldboot for most part - BUT bank_counts/bank_enables for PTs of
 *   non-default group are from HA state.
 * - Must use ha_mem_realloc to extend the memories.
 * - Will reset contents of PTcache to match default (reset) values.
 */
static int
bcmptm_ptcache_reinit(int u)
{
    size_t sid_count = 0;
    uint32_t req_size, alloc_size, *alloc_ptr;
    uint32_t seds_index, cwme_index, group_seds_count, group_cwme_count,
             vinfo_word_count, ltid_seg_word_count;
    bool pt_size_change = FALSE, warm_false = FALSE;
    uint32_t pt_group_count = 0, pt_group = 0;
    const bcmdrd_sid_t *pt_list;
    int s = 0; /*  Assume: This fn is never called when acc_for_ser = True. */
    char alloc_str_id[16];
    SHR_FUNC_ENTER(u);

    /* Basic info */
    SHR_IF_ERR_EXIT(bcmptm_pt_sid_count_get(u, &sid_count));

    /* Determine num of pt_groups supported for this u */
    SHR_IF_ERR_MSG_EXIT(
        bcmptm_pt_group_info_get(u, BCMPTM_PT_GROUP_DEFAULT,
                                 &pt_group_count, NULL, &pt_list),
        (BSL_META_U(u, "pt_group_info_get failed (group_count = %0d)\n"),
         pt_group_count));
    SHR_IF_ERR_MSG_EXIT(
        (pt_group_count > BCMPTM_PT_GROUP_COUNT_MAX) ? SHR_E_MEMORY : SHR_E_NONE,
        (BSL_META_U(u, "pt_group_count = %0d cannot be supported\n"),
         pt_group_count));

    /* sinfo_array was refreshed during init(warm) and PTcache ptrs are valid.
     *
     * But, because tbl_inst_count has changed for some PTs, so sinfo contents
     * may change for some PTs. We have already made copy of old contents, so we
     * can now 0 sinfo array (as 1st step for reprogramming based on new
     * regsfile).
     *
     * Size (depth) should remain same.
     */
    sal_memset(sinfo_bp[u][s], 0, sid_count * sizeof(bcmptm_ptcache_sinfo_t));
        /* For ALL sid set sinfo to 0 (PTcache_type UNINIT).
         * Note: Reserved words are not zeroed - they will retain values from
         *       init(warm).
         *
         * Note: We also cleared sinfo for CCI related PTs.
         *       Issu struct is reported only during init(cold).
         */

    /* pt_group_info array was also refreshed during init(warm) - so now we
     * already have correct num_banks, start_bank, word_counts for PTs in non-
     * default groups. If we assume that PTs in non-default group have not
     * changed, then all this info is still valid.
     *
     * But, because tbl_inst_count for some PTs in default group has changed, so
     * values for pt_group0 are stale and should be zeroed.
     */
    SHR_NULL_CHECK(pt_group_info_bp[u][s], SHR_E_INTERNAL);
    sal_memset(pt_group_info_bp[u][s], 0,
               sizeof(bcmptm_ptcache_pt_group_info_t)); /* only for pt_group0 */
            /* Issu struct is reported only during init(cold). */


    /* Space in heap was allocated during init(warm) to store vinfo_bp,
     * ltid_seg_base_ptr for all pt_groups.
     * But, because tbl_inst_count has changed only for PTs in default pt_group0,
     * so vinfo_bp, ltid_seg_base_ptr for pt_group0 are stale - and should be
     * zeroed out. */
    SHR_NULL_CHECK(pt_group_ptrs_bp[u][s], SHR_E_INTERNAL);
    sal_memset(pt_group_ptrs_bp[u][s], 0,
               sizeof(bcmptm_ptcache_pt_group_ptrs_t)); /* only for pt_group0 */

    /* Iterate through each group starting from last PT group down to
     * DEFAULT PT group. */
    seds_index = 0;
    cwme_index = 0;
    pt_group = pt_group_count - 1;
    do {
        bcmptm_ptcache_pt_group_info_t *pt_group_info_p;
        bcmptm_ptcache_pt_group_ptrs_t *pt_group_ptrs_p;
        pt_group_info_p = pt_group_info_bp[u][s] + pt_group;
        pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + pt_group;

        /* Changes are only for default pt_group0, and index_count_set for PTs
         * in non-default groups has already been done during init(warm)
         *
         * But, because we have cleared sinfo array, we need to run sinfo_init
         * for all PTs in ALL PT groups.
         *
         * Side_effect: sinfo_init_group(warm_false) will also reinit
         * pt_group_info_p counts for all PT groups. For non-default groups
         * nothing has changed - so such reinit is a rewrite of same values.
         * But for default pt_group, these are values based on new regsfile and
         * these writes are necessary. */
        sal_memset(issu_ctl_p[u], 0, sizeof(bcmptm_ptcache_issu_ctl_t));
        issu_ctl_p[u]->skip_sinfo_cci_init = TRUE;
        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_sinfo_init_group(
                u, warm_false, pt_size_change, sid_count,
                pt_group,
                seds_index, cwme_index,
                /* out */
                &vinfo_word_count, &ltid_seg_word_count,
                &group_seds_count, &group_cwme_count));
        sal_memset(issu_ctl_p[u], 0, sizeof(bcmptm_ptcache_issu_ctl_t));

        /* Update cumulative offset into seds, cwme arrays */
        seds_index += group_seds_count;
        cwme_index += group_cwme_count;

        /* seds_index should not increment for non DEFAULT group. */
        SHR_IF_ERR_EXIT((seds_index &&
                         (pt_group != BCMPTM_PT_GROUP_DEFAULT)) ?
                        SHR_E_INTERNAL : SHR_E_NONE);

        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            /* vinfo_word_count could have changed, so may need to realloc
             * space for vinfo array */
            (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                               "ptmCacheVinf", pt_group);
            req_size = vinfo_word_count * sizeof(uint32_t); /* new req_size */
            alloc_size = 1; /* To know what is size of old ha block */
            alloc_ptr = /* old vinfo_bp */
                shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                 BCMPTM_HA_SUBID_PTCACHE_VINFO + pt_group,
                                 alloc_str_id, &alloc_size);
            if (alloc_size < req_size) {
                /* old ha mem size was < what we are now requesting */
                uint32_t *new_bp = NULL;
                new_bp = shr_ha_mem_realloc(u, alloc_ptr, req_size);
                SHR_NULL_CHECK(new_bp, SHR_E_MEMORY);
                alloc_ptr = new_bp;
                alloc_size = req_size;
            }
            sal_memset(alloc_ptr, 0, alloc_size);
            *alloc_ptr = BCMPTM_HA_SIGN_PTCACHE_VINFO + sid_count + pt_group;
            pt_group_ptrs_p->vinfo_bp = alloc_ptr + 1; /* skip_sign_word */
            /* vinfo_word_count was already written into pt_group_info by
             * sinfo_init_group. */

            /* Similarly, ltid_seg_word_count could have changed, so may need to
             * realloc space for ltid_seg */
            (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                               "ptmCacheLtid", pt_group);
            if (ltid_size16b[u]) {
                req_size = ltid_seg_word_count * sizeof(uint16_t);
            } else {
                req_size = ltid_seg_word_count * sizeof(uint32_t);
            }
            alloc_size = 1; /* To know what is current size of ha block */
            alloc_ptr = /* old ltid_seg_base_ptr */
                shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                 BCMPTM_HA_SUBID_PTCACHE_LTID_SEG + pt_group,
                                 alloc_str_id, &alloc_size);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "LTID_SEG%0d: alloc_size=%0u Bytes, req_size="
                            "%0u Bytes\n"),
                 pt_group, alloc_size, req_size));
            if (alloc_size < req_size) {
                /* old ha mem size was < what we are now requesting */
                uint32_t *new_bp = NULL;
                new_bp = shr_ha_mem_realloc(u, alloc_ptr, req_size);
                SHR_NULL_CHECK(new_bp, SHR_E_MEMORY);
                alloc_ptr = new_bp;
                alloc_size = req_size;
            }
            sal_memset(alloc_ptr, 0, alloc_size);
            *alloc_ptr = BCMPTM_HA_SIGN_PTCACHE_LTID_SEG + sid_count + pt_group;
            pt_group_ptrs_p->ltid_seg_base_ptr = alloc_ptr + 1;
            /* Recall: ltid_seg_word_count was already written into pt_group_
             * info by sinfo_init_group. */

            /* Can allocate space for cwme, seds only when we finish processing
             * SIDs in DEFAULT group because:
             * - cwme_index can be incremented in all groups
             * - seds_index can only be incremented in group 0
             * - cwme, seds array share same HW block.
             */

            /* sinfo_init done for for all groups. */
            /* Store counts in sinfo array for CB */
            (sinfo_bp[u][s] + SINFO_RSVD_INDEX_SEDS_COUNT_WORD)->cw_index =
                seds_index;
            (sinfo_bp[u][s] + SINFO_RSVD_INDEX_CWME_COUNT_WORD)->cw_index =
                cwme_index;

            /* tbl_inst_count for SE PTs could have changed - so may need to
             * need less space for seds array - pt_group 0 ONLY */
            req_size = seds_index * sizeof(uint32_t); /* new req_size */
            alloc_size = 1; /* To know current size of ha block */
            alloc_ptr = /* old seds_bp */
                shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                 BCMPTM_HA_SUBID_PTCACHE_SEDS,
                                 "ptmCacheSeds", &alloc_size);
            if (alloc_size < req_size) {
                /* old ha mem size was < what we are now requesting */
                uint32_t *new_bp = NULL;
                new_bp = shr_ha_mem_realloc(u, alloc_ptr, req_size);
                SHR_NULL_CHECK(new_bp, SHR_E_MEMORY);
                alloc_ptr = new_bp;
                alloc_size = req_size;
            }
            sal_memset(alloc_ptr, 0, alloc_size);
            *alloc_ptr = BCMPTM_HA_SIGN_PTCACHE_SEDS + sid_count;
            seds_bp[u][s] = (uint32_t *)(alloc_ptr + 1); /* skip sign_word */

            /* tbl_inst_count for SE PTs could have changed resulting in
             * increase of cwme - so may need to realloc space for cwme array - pt_group 0 ONLY */
            req_size = cwme_index * sizeof(bcmptm_ptcache_cwme_t);
            alloc_size = 1; /* To know what is current size of ha block */
            alloc_ptr = /* old cwme_bp */
                shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                 BCMPTM_HA_SUBID_PTCACHE_CWME_SEG,
                                 "ptmCacheCwme", &alloc_size);
            if (alloc_size < req_size) {
                /* old ha mem size was < what we are now requesting */
                uint32_t *new_bp = NULL;
                new_bp = shr_ha_mem_realloc(u, alloc_ptr, req_size);
                SHR_NULL_CHECK(new_bp, SHR_E_MEMORY);
                alloc_ptr = new_bp;
                alloc_size = req_size;
            }
            sal_memset(alloc_ptr, 0, alloc_size);
            *alloc_ptr = BCMPTM_HA_SIGN_PTCACHE_CWME_SEG + sid_count;
            cwme_bp[u][s] = (bcmptm_ptcache_cwme_t *)(alloc_ptr + 1);
                                    /* skip sign_word */
            pt_group_info_p->num_banks = 0; /* !applicable for DEFAULT group */
        } /* pt_group0 */

        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            break; /* from do-while */
        } else {
            pt_group--;
        }
    } while (1); /* foreach pt_group */

    /* We have done coldboot for:
     *      - sinfo array - filled for all pt_groups by sinfo_init_group.
     *      - CCI
     *      - seds, cwme arrays - zeroed - not yet filled.
     *      - {vinfo array, ltid_seg} for pt_group0 - not yet filled.
     *
     * We have done WB for:
     *      - {vinfo array, ltid_seg} for non-DEFAULT pt_groups.
     *
     * pt_group_info_p were refreshed by init(warm). Counts for:
     *      - pt_group_0 were updated by sinfo_init_group.
     *      - non_pt_group0 were also updated by sinfo_init_group (even though
     *      there was no need).
     *
     * pt_group_ptrs_p were refreshed by init(warm) and ptrs for:
     *      - pt_group0 were updated to point to reallocated HA blocks for
     *        vinfo array, ltid_seg.
     *      - non_pt_group0 point to old HA blocks for vinfo array, ltid_seg.
     *
     * Static PTcache bp vars point to correct sinfo, cwme, seds, etc.
     */

    /* What remains:
     *      - Fill cwme for non_pt_group0. Must not do vinfo, ltid_seg
     *        changes for these groups.
     *        (have modified cwme_vinfo_init_group to do this).
     *
     *      - Fill cwme for pt_group0.
     *        Initially fill seds, vinfo array, ltid_seg with default values -
     *        as if this coldboot.
     *        Above is exactly what cwme_vinfo_group does.
     *
     *      - Finally, copy pt_group0 seds, vinfo, ltid from Heap into PTcache.
     *        PTcache.HA
     */

    /* Pass_2: Populate seds, cwme array, vinfo array, ltid_seg */

    /* Iterate through each group starting from last PT group down to
     * DEFAULT PT group. */
    pt_group = pt_group_count - 1;
    do {
        /* Run cwme_vinfo for all PTs in this group. This will init cwme array
         * for non-DEFAULT pt_groups BUT also cleanup vinfo array.
         */
        sal_memset(issu_ctl_p[u], 0, sizeof(bcmptm_ptcache_issu_ctl_t));
        issu_ctl_p[u]->skip_sinfo_cci_init = TRUE;
        issu_ctl_p[u]->skip_vinfo_ltid = pt_group != BCMPTM_PT_GROUP_DEFAULT;
                /* skip_vinfo_ltid = F for pt_group0,
                 * skip_vinfo_ltid = T for other groups */
        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_cwme_vinfo_init_group(
                u, warm_false, pt_size_change, sid_count,
                pt_group));

        /* We initialized cwme for PTs in pt_group0, but need to fill correct
         * values in seds, vinfo, ltid array for these PTs.
         */
        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            /* Copy vinfo, seds from PTcache.heap to PTcache.ha */
            SHR_IF_ERR_EXIT(ptcache_vinfo_ltid_copy(u, pt_group));
        }
        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            break; /* from do-while */
        } else {
            pt_group--;
        }
    } while (1); /* foreach pt_group */
    sal_memset(issu_ctl_p[u], 0, sizeof(bcmptm_ptcache_issu_ctl_t));

    /* Verify all sentinels (practice) */
    SHR_IF_ERR_EXIT(bcmptm_ptcache_verify(u, FALSE)); /* non_ser */

    /* Registration with evm to know the uft bank changes sizes was already
     * done during init(warm). */

    /* flags_word, sid_count, sign words has not changed in reserved sinfo
     * section since init(warm). */
exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "Exiting reinit with ERROR\n")));
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/* Called by ISSU:
 * - after comp_config(warm) phase, but
 * - before pre_config phase.
 */
int
bcmptm_ptcache_upgrade1(int u)
{
    SHR_FUNC_ENTER(u);

    /* Get PTcache pointers */
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_info_get(
            u,
            &sinfo_bp[u][0],
            &pt_group_info_bp[u][0],
            &pt_group_ptrs_bp[u][0],
            &seds_bp[u][0],
            &cwme_bp[u][0],
            &issu_ctl_p[u],
            &ltid_size16b[u]));

    /* Create copy of PTcache in Heap */
    SHR_IF_ERR_EXIT(ptcache_copy_create(u));

    /* Re-initialize PTcache */
    SHR_IF_ERR_EXIT(bcmptm_ptcache_reinit(u));

    /* Set PTcache pointers */
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_info_set(
            u,
            sinfo_bp[u][0],
            pt_group_info_bp[u][0],
            pt_group_ptrs_bp[u][0],
            seds_bp[u][0],
            cwme_bp[u][0]));

    /* Reset PTcache behavior */
    sal_memset(issu_ctl_p[u], 0, sizeof(bcmptm_ptcache_issu_ctl_t));

exit:
    /* Errors, no errors - must free up PTcache.Heap */
    SHR_IF_ERR_EXIT(ptcache_copy_free(u));
    SHR_FUNC_EXIT();
}
