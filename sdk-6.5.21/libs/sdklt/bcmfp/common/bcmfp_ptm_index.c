/*! \file bcmfp_ptm_index.c
 *
 * APIs to insert, delete or lookup LT entries using
 * PTM indexed modeled path APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static uint32_t req_ew[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t rsp_ew[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

int
bcmfp_ptm_index_lookup(int unit,
                       uint32_t trans_id,
                       int tbl_inst,
                       bcmltd_sid_t req_ltid,
                       bcmdrd_sid_t profile_sid,
                       uint32_t *profile_data,
                       int index)
{
    uint32_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = tbl_inst;

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

    sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    if (sal_memcmp(profile_data, rsp_ew[unit],
            rsp_entry_wsize * sizeof(uint32_t))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_index_insert(int unit,
                       uint32_t trans_id,
                       int tbl_inst,
                       bcmltd_sid_t req_ltid,
                       bcmdrd_sid_t profile_sid,
                       uint32_t *profile_data,
                       int index)
{
    uint32_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = tbl_inst;

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

    sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    profile_data,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_EXIT((req_ltid != rsp_ltid) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_index_delete(int unit,
                       uint32_t trans_id,
                       int tbl_inst,
                       bcmltd_sid_t req_ltid,
                       bcmdrd_sid_t profile_sid,
                       int index)
{
    uint32_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = tbl_inst;

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

    sal_memset(req_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    req_ew[unit],
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_EXIT((req_ltid != rsp_ltid) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_index_read(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     bcmdrd_sid_t profile_sid,
                     uint32_t *profile_data,
                     int index)
{
    uint32_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

    pt_dyn_info.index = index;
    pt_dyn_info.tbl_inst = tbl_inst;

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

    sal_memset(rsp_ew[unit], 0 , BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    sal_memcpy(profile_data, rsp_ew[unit],
              (rsp_entry_wsize * sizeof(uint32_t)));

exit:
    SHR_FUNC_EXIT();
}
