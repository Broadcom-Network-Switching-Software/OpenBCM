/*! \file bcmptm_misc.c
 *
 * Traverse alloc, de-alloc for PTM
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC

/* SOC memory/register SID section number in symbol table */
#define SID_SECT_SOC 0


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_traverse_alloc(int unit, size_t sz, char *s, void **ptr)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ptr, SHR_E_PARAM);
    SHR_ALLOC(*ptr, sz, s);
    SHR_NULL_CHECK(*ptr, SHR_E_MEMORY);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_traverse_free(int unit, void *ptr)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ptr, SHR_E_PARAM);
    SHR_FREE(ptr);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wo_aggr_read_hw(int unit, uint64_t flags,
                       bcmdrd_sid_t sid,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       void *pt_ovrr_info,
                       bcmptm_wo_aggr_info_t *wo_aggr_info,
                       size_t rsp_entry_wsize,

                       uint32_t *rsp_entry_words,
                       bool *ser_failure,
                       bool *retry_needed)
{
    int i, tmp_rv = SHR_E_NONE;
    bcmdrd_sid_t n_sid;
    uint32_t tmp_entry_buf[BCMPTM_MAX_PT_ENTRY_WORDS];
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(wo_aggr_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(ser_failure, SHR_E_PARAM);
    SHR_NULL_CHECK(retry_needed, SHR_E_PARAM);

    for (i = 0; i < wo_aggr_info->n_sid_count; i++) {
        uint32_t w_sbit, w_ebit;
        const bcmptm_overlay_info_t *oinfo;
        n_sid = wo_aggr_info->n_sid_list[i];
        sal_memset(tmp_entry_buf, 0,
                   BCMPTM_MAX_PT_ENTRY_WORDS * sizeof(uint32_t));
        tmp_rv = bcmbd_pt_read(unit,
                               n_sid,
                               pt_dyn_info,
                               pt_ovrr_info,
                               tmp_entry_buf,
                               BCMPTM_MAX_PT_ENTRY_WORDS);
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
            *ser_failure = TRUE;
            if (bcmptm_pt_ser_resp(unit, n_sid) != BCMDRD_SER_RESP_NONE) {
                *retry_needed = TRUE;
                SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
            }
        }
        SHR_IF_ERR_EXIT(tmp_rv);

        /* TCAM format -> KM conversion */
        /* Will be done by caller on rsp_entry_words */

        /* Stuff member_rsp_data in rsp_entry_words */
        if (wo_aggr_info->w_mult) {
            SHR_IF_ERR_EXIT(
                bcmptm_pt_overlay_mult_info_get(unit, n_sid, sid, &oinfo));
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_pt_overlay_info_get(unit, n_sid, &oinfo));
        }
        /* Other checks:
         * (oinfo->mode == BCMPTM_OINFO_MODE_TCAM_DATA);
         * (oinfo->is_alpm == FALSE);
         * (oinfo->n_sid == n_sid);
         * (oinfo->w_sid == sid);
         * (oinfo->num_ne == 1);
         * (oinfo->num_we_field == 1);
         * (oinfo->we_field_array == NULL);
         */
        w_sbit = oinfo->start_bit_ne_in_we;
        w_ebit = w_sbit + oinfo->width_ne - 1;
        if ((w_sbit >= BCMPTM_MAX_PT_ENTRY_WORDS * 32) ||
            (w_ebit >= BCMPTM_MAX_PT_ENTRY_WORDS * 32) ||
            (w_sbit >= rsp_entry_wsize * 32) ||
            (w_ebit >= rsp_entry_wsize * 32)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        bcmdrd_field_set(rsp_entry_words,   /* w_entry_buf */
                         w_sbit, w_ebit,
                         tmp_entry_buf);    /* n_field_buf */
    } /* foreach ne of wo_aggr */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_sid_count_get(int unit, size_t *sid_count)
{
    bcmdrd_sid_t sid_list[1];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_sid_sect_list_get(unit, SID_SECT_SOC,
                                    COUNTOF(sid_list), sid_list,
                                    sid_count));
    if (*sid_count == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Number of cacheable SIDs should not be 0\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (sid_list[0] != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "First SID should not be non-zero (%d)\n"),
                   sid_list[0]));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

