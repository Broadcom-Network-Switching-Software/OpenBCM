/*! \file bcmcth_tnl_decap_drv.c
 *
 * BCMCTH Tunnel Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_tnl_decap_internal.h>
#include <bcmcth/bcmcth_tnl_drv.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

/*! Device specific attach function type for tunnel decap. */
typedef bcmcth_tnl_decap_drv_t *(*bcmcth_tnl_decap_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_tnl_decap_drv_get },
static struct {
    bcmcth_tnl_decap_drv_get_f drv_get;
} tnl_decap_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_tnl_decap_drv_t *tnl_decap_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Private functions
 */
static int
tnl_mpls_decap_hw_entry_read(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             bcmdrd_sid_t hw_sid,
                             bcmptm_rm_hash_entry_mode_t mode,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out)
{
    int         rv = 0, i = 0, ent_count;
    uint32_t    **ekw = NULL;
    uint32_t    **edw = NULL;
    uint32_t    **rsp_ekw = NULL;
    uint32_t    **rsp_edw = NULL;
    uint32_t    wsize = 0, alloc_sz = 0;
    uint64_t    req_flags = 0;
    uint32_t    rsp_flags = 0;
    bcmptm_op_type_t    ptm_op;
    bcmltd_sid_t        rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;
    bcmptm_pt_op_info_t lt_pt_op_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));
    sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));
    sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(&lt_pt_op_info, 0, sizeof(lt_pt_op_info));

    ent_count = 4;
    alloc_sz = (sizeof(uint32_t *) * ent_count);
    SHR_ALLOC(ekw, alloc_sz, "bcmcthtnlReqKeyPtr");
    SHR_NULL_CHECK(ekw, SHR_E_MEMORY);
    sal_memset(ekw, 0, alloc_sz);
    SHR_ALLOC(edw, alloc_sz, "bcmcthtnlReqDataPtr");
    SHR_NULL_CHECK(edw, SHR_E_MEMORY);
    sal_memset(edw, 0, alloc_sz);

    SHR_ALLOC(rsp_ekw, alloc_sz, "bcmcthtnlRspKeyPtr");
    SHR_NULL_CHECK(rsp_ekw, SHR_E_MEMORY);
    sal_memset(rsp_ekw, 0, alloc_sz);
    SHR_ALLOC(rsp_edw, alloc_sz, "bcmcthtnlRspDataPtr");
    SHR_NULL_CHECK(rsp_edw, SHR_E_MEMORY);
    sal_memset(rsp_edw, 0, alloc_sz);

    for (i = 0; i < ent_count; i++) {
        alloc_sz = (sizeof(uint32_t) * ent_count);
        SHR_ALLOC(ekw[i], alloc_sz, "bcmcthtnlReqKey");
        SHR_NULL_CHECK(ekw[i], SHR_E_MEMORY);
        sal_memset(ekw[i], 0, alloc_sz);

        SHR_ALLOC(edw[i], alloc_sz, "bcmcthtnlReqData");
        SHR_NULL_CHECK(edw[i], SHR_E_MEMORY);
        sal_memset(edw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_ekw[i], alloc_sz, "bcmcthtnlRspKey");
        SHR_NULL_CHECK(rsp_ekw[i], SHR_E_MEMORY);
        sal_memset(rsp_ekw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_edw[i], alloc_sz, "bcmcthtnlRspData");
        SHR_NULL_CHECK(rsp_edw[i], SHR_E_MEMORY);
        sal_memset(rsp_edw[i], 0, alloc_sz);
    }

    wsize = bcmdrd_pt_entry_wsize(unit, hw_sid);

    if (in != NULL) {
        BCMCTH_TNL_MPLS_DECAP_LT_TO_HW_MAP(unit)(unit, sid, hw_sid, in, ekw, edw);
    }

    hash_attrs.entry_mode = mode;
    lt_mreq_info.entry_attrs = &hash_attrs;
    lt_mreq_info.entry_key_words = ekw;
    lt_mreq_info.entry_data_words = edw;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;
    lt_mreq_info.rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    lt_mrsp_info.rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info.rsp_entry_data_words = rsp_edw;
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info.pt_op_info = &lt_pt_op_info;

    pt_dyn_info.tbl_inst = -1;

    if (opcode == BCMLT_OPCODE_LOOKUP) {
        ptm_op = BCMPTM_OP_LOOKUP;
    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        if (in == NULL) {
            ptm_op = BCMPTM_OP_GET_FIRST;
        } else {
            ptm_op = BCMPTM_OP_GET_NEXT;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        req_flags |= BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT;
    }

    rv = bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  sid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  ptm_op,
                                  &lt_mreq_info,
                                  op_arg->trans_id,
                                  &lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags);

    if (SHR_FAILURE(rv)) {
        if (opcode == BCMLT_OPCODE_TRAVERSE) {
            req_flags &= ~BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT;
            req_flags |= BCMLT_ENT_ATTR_TRAVERSE_DONE;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_ltm_mreq_keyed_lt(unit,
                                 req_flags,
                                 sid,
                                 (void *)&pt_dyn_info,
                                 NULL,
                                 ptm_op,
                                 &lt_mreq_info,
                                 op_arg->trans_id,
                                 &lt_mrsp_info,
                                 &rsp_ltid,
                                 &rsp_flags));
        }
        SHR_ERR_EXIT(rv);
    }

    if (out != NULL) {
        BCMCTH_TNL_MPLS_DECAP_HW_TO_LT_MAP(unit)(unit,
                                                 sid,
                                                 hw_sid,
                                                 rsp_ekw,
                                                 rsp_edw,
                                                 out);
    }

exit:
    for (i = 0; i < ent_count; i++) {
        SHR_FREE(ekw[i]);
        SHR_FREE(edw[i]);
        SHR_FREE(rsp_ekw[i]);
        SHR_FREE(rsp_edw[i]);
    }
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);

    SHR_FUNC_EXIT();
}

static int
tnl_mpls_decap_hw_entry_write(int unit,
                              bcmlt_opcode_t opcode,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t sid,
                              bcmdrd_sid_t hw_sid,
                              bcmptm_rm_hash_entry_mode_t mode,
                              const bcmltd_fields_t *in)
{
    int         i = 0, ent_count = 0;
    uint32_t    **ekw = NULL;
    uint32_t    **edw = NULL;
    uint32_t    **rsp_ekw = NULL;
    uint32_t    **rsp_edw = NULL;
    uint32_t    wsize = 0, alloc_sz = 0;
    uint64_t    req_flags = 0;
    uint32_t    rsp_flags = 0;
    bcmptm_op_type_t    ptm_op;
    bcmltd_sid_t        rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;
    bcmptm_pt_op_info_t lt_pt_op_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));
    sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));
    sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(&lt_pt_op_info, 0, sizeof(lt_pt_op_info));

    ent_count = 4;
    alloc_sz = (sizeof(uint32_t *) * ent_count);
    SHR_ALLOC(ekw, alloc_sz, "bcmcthtnlReqKeyPtr");
    SHR_NULL_CHECK(ekw, SHR_E_MEMORY);
    sal_memset(ekw, 0, alloc_sz);
    SHR_ALLOC(edw, alloc_sz, "bcmcthtnlReqDataPtr");
    SHR_NULL_CHECK(edw, SHR_E_MEMORY);
    sal_memset(edw, 0, alloc_sz);

    SHR_ALLOC(rsp_ekw, alloc_sz, "bcmcthtnlRspKeyPtr");
    SHR_NULL_CHECK(rsp_ekw, SHR_E_MEMORY);
    sal_memset(rsp_ekw, 0, alloc_sz);
    SHR_ALLOC(rsp_edw, alloc_sz, "bcmcthtnlRspDataPtr");
    SHR_NULL_CHECK(rsp_edw, SHR_E_MEMORY);
    sal_memset(rsp_edw, 0, alloc_sz);

    for (i = 0; i < ent_count; i++) {
        alloc_sz = (sizeof(uint32_t) * ent_count);
        SHR_ALLOC(ekw[i], alloc_sz, "bcmcthtnlReqKey");
        SHR_NULL_CHECK(ekw[i], SHR_E_MEMORY);
        sal_memset(ekw[i], 0, alloc_sz);

        SHR_ALLOC(edw[i], alloc_sz, "bcmcthtnlReqData");
        SHR_NULL_CHECK(edw[i], SHR_E_MEMORY);
        sal_memset(edw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_ekw[i], alloc_sz, "bcmcthtnlRspKey");
        SHR_NULL_CHECK(rsp_ekw[i], SHR_E_MEMORY);
        sal_memset(rsp_ekw[i], 0, alloc_sz);

        SHR_ALLOC(rsp_edw[i], alloc_sz, "bcmcthtnlRspData");
        SHR_NULL_CHECK(rsp_edw[i], SHR_E_MEMORY);
        sal_memset(rsp_edw[i], 0, alloc_sz);
    }

    wsize = bcmdrd_pt_entry_wsize(unit, hw_sid);

    BCMCTH_TNL_MPLS_DECAP_LT_TO_HW_MAP(unit)(unit, sid, hw_sid, in, ekw, edw);

    hash_attrs.entry_mode = mode;
    lt_mreq_info.entry_attrs = &hash_attrs;
    lt_mreq_info.entry_key_words = ekw;
    lt_mreq_info.entry_data_words = edw;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;
    lt_mreq_info.rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    lt_mrsp_info.rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info.rsp_entry_data_words = rsp_edw;
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info.pt_op_info = &lt_pt_op_info;

    pt_dyn_info.tbl_inst = -1;

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    if (opcode == BCMLT_OPCODE_UPDATE) {
        /* Read first for update. */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_ltm_mreq_keyed_lt(unit,
                                     req_flags,
                                     sid,
                                     (void *)&pt_dyn_info,
                                     NULL,
                                     BCMPTM_OP_LOOKUP,
                                     &lt_mreq_info,
                                     op_arg->trans_id,
                                     &lt_mrsp_info,
                                     &rsp_ltid,
                                     &rsp_flags));

        /* Copy response data into request data. */
        alloc_sz = (sizeof(uint32_t) * ent_count);
        for (i = 0; i < ent_count; i++) {
            sal_memset(ekw[i], 0, alloc_sz);
            sal_memset(edw[i], 0, alloc_sz);
            sal_memcpy(edw[i], rsp_edw[i], alloc_sz);
        }
        /* Update the data buffer with user input. */
        BCMCTH_TNL_MPLS_DECAP_LT_TO_HW_MAP(unit)(unit, sid, hw_sid, in, ekw, edw);
    }

    if (opcode == BCMLT_OPCODE_INSERT) {
        ptm_op = BCMPTM_OP_INSERT;
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        ptm_op = BCMPTM_OP_DELETE;
    } else if (opcode == BCMLT_OPCODE_UPDATE) {
        ptm_op = BCMPTM_OP_INSERT;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ltm_mreq_keyed_lt(unit,
                                                     req_flags,
                                                     sid,
                                                     (void *)&pt_dyn_info,
                                                     NULL,
                                                     ptm_op,
                                                     &lt_mreq_info,
                                                     op_arg->trans_id,
                                                     &lt_mrsp_info,
                                                     &rsp_ltid,
                                                     &rsp_flags));

exit:
    for (i = 0; i < ent_count; i++) {
        SHR_FREE(ekw[i]);
        SHR_FREE(edw[i]);
        SHR_FREE(rsp_ekw[i]);
        SHR_FREE(rsp_edw[i]);
    }
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);

    SHR_FUNC_EXIT();
}

/******************************************************************************
  Public functions
 */

int
bcmcth_tnl_decap_mpls_validate(int unit,
                               bcmlt_opcode_t opcode,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in)
{
    int         count = 0, idx = 0;
    uint32_t    fid = 0;
    bool        key1 = false, key2 = false;

    SHR_FUNC_ENTER(unit);

    count = in->count;

    for (idx = 0; idx < count; idx++) {
        fid = in->field[idx]->id;

        switch (sid) {
        case TNL_MPLS_DECAPt:
            if (fid == TNL_MPLS_DECAPt_MPLS_LABELf) {
                key1 = true;
            }

            if (fid == TNL_MPLS_DECAPt_MODPORTf) {
                key2 = true;
            }
            break;
        case TNL_MPLS_DECAP_TRUNKt:
            if (fid == TNL_MPLS_DECAP_TRUNKt_MPLS_LABELf) {
                key1 = true;
            }

            if (fid == TNL_MPLS_DECAP_TRUNKt_TRUNK_IDf) {
                key2 = true;
            }
            break;
        default:
            break;
        }
    }

    if (key1 == false || key2 == false) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_mpls_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in)
{
    int         rv = 0, hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMCTH_TNL_DECAP_SID_COUNT(unit);

    SHR_IF_ERR_EXIT(bcmptm_mreq_atomic_trans_enable(unit));

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMCTH_TNL_DECAP_SID_LIST(unit)[i];
        mode = BCMCTH_TNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        rv = 0;
        rv = tnl_mpls_decap_hw_entry_read(unit,
                                          BCMLT_OPCODE_LOOKUP,
                                          op_arg,
                                          sid,
                                          hw_sid,
                                          mode,
                                          in,
                                          NULL);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_decap_hw_entry_write(unit,
                                          BCMLT_OPCODE_INSERT,
                                          op_arg,
                                          sid,
                                          hw_sid,
                                          mode,
                                          in));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_mpls_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_sid_t sid,
                             bcmltd_fields_t *out)
{
    int         hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMCTH_TNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMCTH_TNL_DECAP_SID_LIST(unit)[i];
        mode = BCMCTH_TNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_decap_hw_entry_read(unit,
                                         BCMLT_OPCODE_LOOKUP,
                                         op_arg,
                                         sid,
                                         hw_sid,
                                         mode,
                                         in,
                                         out));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_mpls_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in)
{
    int         hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMCTH_TNL_DECAP_SID_COUNT(unit);

    SHR_IF_ERR_EXIT(bcmptm_mreq_atomic_trans_enable(unit));

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMCTH_TNL_DECAP_SID_LIST(unit)[i];
        mode = BCMCTH_TNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_decap_hw_entry_read(unit,
                                         BCMLT_OPCODE_LOOKUP,
                                         op_arg,
                                         sid,
                                         hw_sid,
                                         mode,
                                         in,
                                         NULL));

        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_decap_hw_entry_write(unit,
                                          BCMLT_OPCODE_DELETE,
                                          op_arg,
                                          sid,
                                          hw_sid,
                                          mode,
                                          in));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_mpls_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in)
{
    int         hw_count = 0, i = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMCTH_TNL_DECAP_SID_COUNT(unit);

    SHR_IF_ERR_EXIT(bcmptm_mreq_atomic_trans_enable(unit));

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMCTH_TNL_DECAP_SID_LIST(unit)[i];
        mode = BCMCTH_TNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_decap_hw_entry_read(unit,
                                         BCMLT_OPCODE_LOOKUP,
                                         op_arg,
                                         sid,
                                         hw_sid,
                                         mode,
                                         in,
                                         NULL));

        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_decap_hw_entry_write(unit,
                                          BCMLT_OPCODE_UPDATE,
                                          op_arg,
                                          sid,
                                          hw_sid,
                                          mode,
                                          in));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_mpls_traverse(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_sid_t sid,
                               bcmltd_fields_t *out)
{
    int         hw_count = 0, i = 0, rv = 0;
    uint32_t    hw_sid = 0;
    bcmptm_rm_hash_entry_mode_t mode;

    SHR_FUNC_ENTER(unit);

    hw_count = BCMCTH_TNL_DECAP_SID_COUNT(unit);

    for (i = 0; i < hw_count; i++) {
        hw_sid = BCMCTH_TNL_DECAP_SID_LIST(unit)[i];
        mode = BCMCTH_TNL_DECAP_ENTRY_ATTRS_LIST(unit)[i];
        rv = tnl_mpls_decap_hw_entry_read(unit,
                                          BCMLT_OPCODE_TRAVERSE,
                                          op_arg,
                                          sid,
                                          hw_sid,
                                          mode,
                                          in,
                                          out);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    tnl_decap_drv[unit] = tnl_decap_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_tnl_decap_drv_get(int unit, bcmcth_tnl_decap_drv_t **drv)
{

    SHR_FUNC_ENTER(unit);

    if (tnl_decap_drv[unit] != NULL) {
        *drv = tnl_decap_drv[unit];
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
