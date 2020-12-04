/*! \file bcmcth_tnl_mpls_prot.c
 *
 * TNL_MPLS_PROTECTION_ENABLE Custom Handler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_tnl_mpls_prot.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

/******************************************************************************
 * Local definitions
 */

/* Start- For imm-based TNL_MPLS_PROTECTION_ENABLE LT. */
typedef bcmcth_tnl_mpls_prot_drv_t *(*bcmcth_tnl_mpls_prot_drv_get_f)(int unit);

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {_bc##_cth_tnl_mpls_prot_drv_get},
static struct {
    bcmcth_tnl_mpls_prot_drv_get_f drv_get;
} mpls_prot_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 }  /* end-of-list */
};
/* End- For imm-based TNL_MPLS_PROTECTION_ENABLE LT. */

static bcmcth_tnl_mpls_prot_drv_t *mpls_prot_drv[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Public functions
 */
int
bcmcth_tnl_mpls_prot_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    mpls_prot_drv[unit] = mpls_prot_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_tnl_mpls_prot_enable_set(int unit, const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id, uint32_t nhop_id, bool prot)
{
    if (mpls_prot_drv[unit] && mpls_prot_drv[unit]->mpls_prot_set) {
        return mpls_prot_drv[unit]->mpls_prot_set(unit,
                                                  op_arg,
                                                  lt_id,
                                                  nhop_id,
                                                  prot);
    }

    return SHR_E_NONE;
}

int
bcmcth_tnl_mpls_prot_enable_get(int unit, const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id, uint32_t nhop_id, bool *prot)
{
    if (mpls_prot_drv[unit] && mpls_prot_drv[unit]->mpls_prot_get) {
        return mpls_prot_drv[unit]->mpls_prot_get(unit,
                                                  op_arg,
                                                  lt_id,
                                                  nhop_id,
                                                  prot);
    }

    return SHR_E_NONE;
}
