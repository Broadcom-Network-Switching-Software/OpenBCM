/*! \file bcmfp_ptm_misc.c
 *
 * FP interfacing APIs to miscellaneous lookup operations in
 * to PTM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_cth_group.h>
#include <bcmfp/bcmfp_cth_entry.h>
#include <bcmfp/bcmfp_cth_policy.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmdrd/bcmdrd_field.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_ptm_group_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t group_id,
                       bcmfp_group_oper_info_t *opinfo,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs)
{
    bool is_presel = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t wsize = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_GROUP_INFO);

    if (flags & BCMFP_ENTRY_PRESEL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
        if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
        }
    }

    /* In BCMFP group ids ranges from 1-<MaxId>  and in RM_TCAM,
     * this range is mapped to 0-<MaxId-1>.
     */
    attrs->group_id = group_id - 1;

    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);

    attrs->pipe_id = opinfo->tbl_inst;
    attrs->trans_id = op_arg->trans_id;
    lt_mreq_info->entry_attrs = attrs;

    lt_mreq_info->entry_id = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_tcam_size_get(unit,
                                   is_presel,
                                   stage_id,
                                   group_id,
                                   opinfo,
                                   &wsize));
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;

    wsize = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_policy_size_get(unit,
                                     is_presel,
                                     stage_id,
                                     group_id,
                                     opinfo,
                                     &wsize));
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;

    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = opinfo->tbl_inst;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     req_ltid,
                                     (void *)&pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_LOOKUP,
                                     lt_mreq_info,
                                     op_arg->trans_id,
                                     lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_stage_entry_limit_get(int unit,
                       uint32_t trans_id,
                       bcmltd_sid_t req_ltid,
                       uint32_t *count)
{
    uint32_t wsize;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t size = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **rsp_ekw1 = NULL;
    uint32_t **rsp_edw1 = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;
    uint32_t *rsp_ekw_ew = NULL;
    uint32_t *rsp_edw_ew = NULL;
    bcmltd_table_entry_limit_t *table_data = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmEntryLimitGet");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmEntryLimitGet");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpEntryLimitEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpEntryLimitEdw");
    BCMFP_ALLOC(rsp_ekw1, sizeof(uint32_t *), "bcmfpEntryLimitRspEkw");
    BCMFP_ALLOC(rsp_edw1, sizeof(uint32_t *), "bcmfpEntryLimitRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpEntryLimitEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpEntryLimitEdwEntryWords");
    BCMFP_ALLOC(rsp_ekw_ew, size, "bcmfpEntryLimitRspEkwEntryWords");
    BCMFP_ALLOC(rsp_edw_ew, size, "bcmfpEntryLimitRspEdwEntryWords");

    BCMFP_ALLOC(table_data,
                sizeof(bcmltd_table_entry_limit_t),
                "bcmfpEntryLimitTableData");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw1[0] = rsp_ekw_ew;
    rsp_edw1[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(table_data, 0, sizeof(bcmltd_table_entry_limit_t));

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw1;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw1;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->rsp_misc = table_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     req_ltid,
                                     &pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_GET_TABLE_INFO,
                                     lt_mreq_info,
                                     trans_id,
                                     lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));

    *count = table_data->entry_limit;

exit:
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw1);
    SHR_FREE(rsp_edw1);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(rsp_ekw_ew);
    SHR_FREE(rsp_edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(table_data);
    SHR_FUNC_EXIT();
}
int
bcmfp_ptm_stage_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs)
{
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_STAGE_INFO);

    if (flags & BCMFP_ENTRY_PRESEL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
        if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
        }
    }

    attrs->group_id = -1;
    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);
    attrs->pipe_id = -1;
    attrs->trans_id = op_arg->trans_id;
    lt_mreq_info->entry_attrs = attrs;
    lt_mreq_info->entry_id = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = 0;
    lt_mreq_info->rsp_entry_data_buf_wsize = 0;
    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     req_ltid,
                                     (void *)&pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_LOOKUP,
                                     lt_mreq_info,
                                     op_arg->trans_id,
                                     lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_slice_lookup(int unit,
                       uint32_t flags,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmltd_sid_t req_ltid,
                       bcmfp_entry_id_t slice_id,
                       int pipe_id,
                       bcmfp_buffers_t *req_buffers,
                       bcmfp_buffers_t *rsp_buffers,
                       bcmptm_rm_tcam_entry_attrs_t *attrs)
{
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;

    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_SLICE_INFO);

    if (flags & BCMFP_ENTRY_PRESEL) {
        SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_PRESEL_ENTRY);
        if (flags & BCMFP_ENTRY_PRESEL_DEFAULT) {
            SHR_BITSET(attrs->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT);
        }
    }

    attrs->group_id = -1;
    attrs->slice_num = slice_id;

    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);

    attrs->pipe_id = pipe_id;
    attrs->trans_id = op_arg->trans_id;
    lt_mreq_info->entry_attrs = attrs;
    lt_mreq_info->entry_id = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = 0;
    lt_mreq_info->rsp_entry_data_buf_wsize = 0;
    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     req_ltid,
                                     (void *)&pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_LOOKUP,
                                     lt_mreq_info,
                                     op_arg->trans_id,
                                     lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_resource_scope(int unit,
                         uint32_t flags,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_buffers_t *req_buffers,
                         bcmfp_buffers_t *rsp_buffers,
                         bcmptm_rm_tcam_fp_resources_req_t *req_res,
                         uint16_t req_res_count,
                         bool *resources_available)
{
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    bcmptm_pt_op_info_t *lt_mrsp_pt_op_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(req_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(req_res, SHR_E_PARAM);
    SHR_NULL_CHECK(resources_available, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmfpPtmGroupLookupPtmReqInfo");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmfpPtmGroupLookupPtmRspInfo");
    BCMFP_ALLOC(lt_mrsp_pt_op_info,
                sizeof(bcmptm_pt_op_info_t),
                "bcmfpPtmGroupLookupPtmRspPtInfo");
    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmfpGroupPartInfoEntryAttrs");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    lt_mreq_info->entry_id = 0;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->entry_attrs = attrs;
    lt_mreq_info->entry_id = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = 0;
    lt_mreq_info->rsp_entry_data_buf_wsize = 0;
    lt_mreq_info->entry_key_words = req_buffers->ekw;
    lt_mreq_info->entry_data_words = req_buffers->edw;
    lt_mrsp_info->rsp_entry_key_words = rsp_buffers->ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_buffers->edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->pt_op_info = lt_mrsp_pt_op_info;

    attrs->trans_id = op_arg->trans_id;
    SHR_BITSET(attrs->flags.w, BCMPTM_FP_FLAGS_RESOURCE_SCOPE);
    attrs->group_id = -1;
    BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(stage_id, attrs->stage_id);
    attrs->req_res = req_res;
    attrs->num_req_res = req_res_count;
    attrs->resources_available = FALSE;
    attrs->pipe_id = -1;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     req_ltid,
                                     &pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_LOOKUP,
                                     lt_mreq_info,
                                     op_arg->trans_id,
                                     lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));

    *resources_available = attrs->resources_available;
exit:
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FREE(lt_mrsp_pt_op_info);
    SHR_FREE(attrs);
    SHR_FUNC_EXIT();
}
