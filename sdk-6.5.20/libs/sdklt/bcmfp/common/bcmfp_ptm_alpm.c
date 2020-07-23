/*! \file bcmfp_ptm_alpm.c
 *
 * APIs to insert, delete or lookup LT entries using
 * ALPM resource manager in BCMPTM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmptm/bcmptm_types.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_ptm_alpm_table_info_get(int unit,
                              uint32_t trans_id,
                              bcmltd_sid_t req_ltid,
                              const bcmltd_table_entry_limit_arg_t *table_arg,
                              bcmltd_table_entry_limit_t *table_data,
                              const bcmltd_generic_arg_t *arg)
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
    uint32_t **rsp_ekw = NULL;
    uint32_t **rsp_edw = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;
    uint32_t *rsp_ekw_ew = NULL;
    uint32_t *rsp_edw_ew = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_data, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmAlpmInsert");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmAlpmInsert");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpCompressAlpmEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpCompressAlpmEdw");
    BCMFP_ALLOC(rsp_ekw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEkw");
    BCMFP_ALLOC(rsp_edw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpCompressAlpmEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpCompressAlpmEdwEntryWords");
    BCMFP_ALLOC(rsp_ekw_ew, size, "bcmfpCompressAlpmRspEkwEntryWords");
    BCMFP_ALLOC(rsp_edw_ew, size, "bcmfpCompressAlpmRspEdwEntryWords");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw[0] = rsp_ekw_ew;
    rsp_edw[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info->rsp_misc = table_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
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

exit:
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(rsp_ekw_ew);
    SHR_FREE(rsp_edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_alpm_insert(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t req_ltid,
                      uint32_t *entry_words)
{
    int rv;
    uint32_t wsize;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    size_t size = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **rsp_ekw = NULL;
    uint32_t **rsp_edw = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;
    uint32_t *rsp_ekw_ew = NULL;
    uint32_t *rsp_edw_ew = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmAlpmInsert");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmAlpmInsert");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpCompressAlpmEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpCompressAlpmEdw");
    BCMFP_ALLOC(rsp_ekw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEkw");
    BCMFP_ALLOC(rsp_edw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpCompressAlpmEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpCompressAlpmEdwEntryWords");
    BCMFP_ALLOC(rsp_ekw_ew, size, "bcmfpCompressAlpmRspEkwEntryWords");
    BCMFP_ALLOC(rsp_edw_ew, size, "bcmfpCompressAlpmRspEdwEntryWords");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw[0] = rsp_ekw_ew;
    rsp_edw[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    sal_memcpy(ekw_ew, entry_words, size);
    sal_memcpy(edw_ew, entry_words, size);

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    rv = bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_LOOKUP,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags);

    if (rv == SHR_E_NOT_FOUND) {
        lt_mreq_info->same_key = 0;
    } else {
        SHR_IF_ERR_EXIT(rv);
        lt_mreq_info->same_key = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_INSERT,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));
exit:
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(rsp_ekw_ew);
    SHR_FREE(rsp_edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_alpm_delete(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t req_ltid,
                      uint32_t *entry_words)
{
    uint32_t wsize;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    size_t size = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **rsp_ekw = NULL;
    uint32_t **rsp_edw = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;
    uint32_t *rsp_ekw_ew = NULL;
    uint32_t *rsp_edw_ew = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmAlpmDelete");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmAlpmDelete");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpCompressAlpmEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpCompressAlpmEdw");
    BCMFP_ALLOC(rsp_ekw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEkw");
    BCMFP_ALLOC(rsp_edw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpCompressAlpmEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpCompressAlpmEdwEntryWords");
    BCMFP_ALLOC(rsp_ekw_ew, size, "bcmfpCompressAlpmRspEkwEntryWords");
    BCMFP_ALLOC(rsp_edw_ew, size, "bcmfpCompressAlpmRspEdwEntryWords");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw[0] = rsp_ekw_ew;
    rsp_edw[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    sal_memcpy(ekw_ew, entry_words, size);

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  BCMPTM_OP_DELETE,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));
exit:
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(rsp_ekw_ew);
    SHR_FREE(rsp_edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_alpm_lookup(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t req_ltid,
                      uint32_t *entry_words,
                      uint32_t *rsp_ekw_ew,
                      uint32_t *rsp_edw_ew)
{
    uint32_t wsize;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    size_t size = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **rsp_ekw = NULL;
    uint32_t **rsp_edw = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ekw_ew, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_edw_ew, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmAlpmInsert");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmAlpmInsert");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpCompressAlpmEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpCompressAlpmEdw");
    BCMFP_ALLOC(rsp_ekw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEkw");
    BCMFP_ALLOC(rsp_edw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpCompressAlpmEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpCompressAlpmEdwEntryWords");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw[0] = rsp_ekw_ew;
    rsp_edw[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    sal_memcpy(ekw_ew, entry_words, size);
    sal_memcpy(edw_ew, entry_words, size);

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
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
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_ptm_alpm_traverse(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t req_ltid,
                        bool get_first,
                        uint32_t *entry_words,
                        uint32_t *rsp_ekw_ew,
                        uint32_t *rsp_edw_ew)
{
    uint32_t wsize;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    size_t size = 0;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t *lt_mreq_info = NULL;
    bcmptm_keyed_lt_mrsp_info_t *lt_mrsp_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **rsp_ekw = NULL;
    uint32_t **rsp_edw = NULL;
    uint32_t *ekw_ew = NULL;
    uint32_t *edw_ew = NULL;
    bcmptm_op_type_t ptm_opcode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ekw_ew, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_edw_ew, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    BCMFP_ALLOC(lt_mreq_info,
                sizeof(bcmptm_keyed_lt_mreq_info_t),
                "bcmptmKeyedLtMreqInfoInBcmFpPtmAlpmInsert");
    BCMFP_ALLOC(lt_mrsp_info,
                sizeof(bcmptm_keyed_lt_mrsp_info_t),
                "bcmptmKeyedLtMrspInfoInBcmFpPtmAlpmInsert");

    BCMFP_ALLOC(ekw, sizeof(uint32_t *), "bcmfpCompressAlpmEkw");
    BCMFP_ALLOC(edw, sizeof(uint32_t *), "bcmfpCompressAlpmEdw");
    BCMFP_ALLOC(rsp_ekw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEkw");
    BCMFP_ALLOC(rsp_edw, sizeof(uint32_t *), "bcmfpCompressAlpmRspEdw");

    size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    BCMFP_ALLOC(ekw_ew, size, "bcmfpCompressAlpmEkwEntryWords");
    BCMFP_ALLOC(edw_ew, size, "bcmfpCompressAlpmEdwEntryWords");

    ekw[0] = ekw_ew;
    edw[0] = edw_ew;
    rsp_ekw[0] = rsp_ekw_ew;
    rsp_edw[0] = rsp_edw_ew;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    sal_memcpy(ekw_ew, entry_words, size);
    sal_memcpy(edw_ew, entry_words, size);

    pt_dyn_info.tbl_inst = -1;
    wsize = BCMFP_MAX_WSIZE;
    lt_mreq_info->entry_key_words = ekw;
    lt_mreq_info->entry_data_words = edw;
    lt_mreq_info->pdd_l_dtyp = NULL;
    lt_mreq_info->same_key = 0;
    lt_mreq_info->rsp_entry_key_buf_wsize = wsize;
    lt_mreq_info->rsp_entry_data_buf_wsize = wsize;
    lt_mrsp_info->rsp_entry_key_words = rsp_ekw;
    lt_mrsp_info->rsp_entry_data_words = rsp_edw;
    lt_mrsp_info->rsp_pdd_l_dtyp = NULL;

    if (get_first == TRUE) {
        ptm_opcode = BCMPTM_OP_GET_FIRST;
    } else {
        ptm_opcode = BCMPTM_OP_GET_NEXT;
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  (void *)&pt_dyn_info,
                                  NULL,
                                  ptm_opcode,
                                  lt_mreq_info,
                                  op_arg->trans_id,
                                  lt_mrsp_info,
                                  &rsp_ltid,
                                  &rsp_flags));

exit:
    SHR_FREE(ekw);
    SHR_FREE(edw);
    SHR_FREE(rsp_ekw);
    SHR_FREE(rsp_edw);
    SHR_FREE(ekw_ew);
    SHR_FREE(edw_ew);
    SHR_FREE(lt_mreq_info);
    SHR_FREE(lt_mrsp_info);
    SHR_FUNC_EXIT();
}
