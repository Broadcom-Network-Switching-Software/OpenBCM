/*! \file rm_tcam_traverse.c
 *
 * This file contains functions to dispatch GET_FIRST/GET_NEXT requests
 * with_snapshot ot without_snapshot functions
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_rm_tcam_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

int
bcmptm_rm_tcam_traverse_info_entry_get_first(int unit,
                                       uint64_t req_flags,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                                       bcmptm_rm_tcam_req_t *req_info,
                                       bcmptm_rm_tcam_rsp_t *rsp_info,
                                       bcmltd_sid_t *rsp_ltid,
                                       uint32_t *rsp_flags,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    SHR_FUNC_ENTER(unit);

    if ((req_flags & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) ||
        (req_flags & BCMLT_ENT_ATTR_TRAVERSE_DONE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_traverse_entry_get_first_with_snapshot(unit,
                                                             req_flags,
                                                             ltid,
                                                             ltid_info,
                                                             pt_dyn_info,
                                                             req_info,
                                                             rsp_info,
                                                             rsp_ltid,
                                                             rsp_flags,
                                                             iomd));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_traverse_entry_get_first_without_snapshot(unit,
                                                             req_flags,
                                                             ltid,
                                                             ltid_info,
                                                             pt_dyn_info,
                                                             req_info,
                                                             rsp_info,
                                                             rsp_ltid,
                                                             rsp_flags,
                                                             iomd));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_traverse_info_entry_get_next(int unit,
                                       uint64_t req_flags,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                                       bcmptm_rm_tcam_req_t *req_info,
                                       bcmptm_rm_tcam_rsp_t *rsp_info,
                                       bcmltd_sid_t *rsp_ltid,
                                       uint32_t *rsp_flags,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    SHR_FUNC_ENTER(unit);

    if ((req_flags & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) ||
        (req_flags & BCMLT_ENT_ATTR_TRAVERSE_DONE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_traverse_entry_get_next_with_snapshot(unit,
                                                            req_flags,
                                                            ltid,
                                                            ltid_info,
                                                            pt_dyn_info,
                                                            req_info,
                                                            rsp_info,
                                                            rsp_ltid,
                                                            rsp_flags,
                                                            iomd));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_traverse_entry_get_next_without_snapshot(unit,
                                                             req_flags,
                                                             ltid,
                                                             ltid_info,
                                                             pt_dyn_info,
                                                             req_info,
                                                             rsp_info,
                                                             rsp_ltid,
                                                             rsp_flags,
                                                             iomd));
    }
exit:
    SHR_FUNC_EXIT();
}
