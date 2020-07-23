/*! \file rm_alpm_stub.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmptm/bcmptm_internal.h>


/*******************************************************************************
  Typedefs
 */


/*******************************************************************************
  Function prototypes
 */
int
bcmptm_rm_alpm_req(int u,
                   uint64_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t req_ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_alpm_req_info_t *req_info,

                   bcmptm_rm_alpm_rsp_info_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags)
{
    return SHR_E_DISABLED;
}


int
bcmptm_rm_alpm_init(int u, bool warm)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_stop(int u)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_cleanup(int u, bool graceful)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_trans_cmd(int u,
                         uint64_t flags,
                         uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_global_init(void)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_global_cleanup(void)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_is_urpf(int u)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_is_parallel(int u)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_mode(int u)
{
    return SHR_E_NONE;
}

int
bcmptm_rm_alpm_max_levels(int u, int db)
{
    return SHR_E_NONE;
}
