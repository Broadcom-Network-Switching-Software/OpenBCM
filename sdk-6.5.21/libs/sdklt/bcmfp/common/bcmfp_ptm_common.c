/*! \file bcmfp_ptm_common.c
 *
 * API to insert, delete or lookup LT entries using
 * bcmptm_ltm_mreq_keyed_lt
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
bcmfp_ptm_ltm_mreq_keyed_lt(int unit,
                            uint64_t req_flags,
                            bcmltd_sid_t req_ltid,
                            void *pt_dyn_info,
                            void *pt_ovrr_info,
                            bcmptm_op_type_t req_op,
                            bcmptm_keyed_lt_mreq_info_t *req_info,
                            uint32_t trans_id,
                            bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                            bcmltd_sid_t *rsp_ltid,
                            uint32_t *rsp_flags)
{
    bool state = FALSE;

    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT
        (bcmfp_trans_atomic_state_get(unit, trans_id, &state));

    if (state == FALSE) {
        req_flags |= BCMPTM_REQ_FLAGS_ALLOW_MULTIPLE_OPS;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_keyed_lt(unit,
                                  req_flags,
                                  req_ltid,
                                  pt_dyn_info,
                                  pt_ovrr_info,
                                  req_op,
                                  req_info,
                                  trans_id,
                                  rsp_info,
                                  rsp_ltid,
                                  rsp_flags));

exit:
    SHR_FUNC_EXIT();
}
