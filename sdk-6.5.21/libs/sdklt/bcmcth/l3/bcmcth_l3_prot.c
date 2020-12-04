/*! \file bcmcth_l3_prot.c
 *
 * L3_PROTECTION_ENABLE Custom Handler
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
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_l3_prot.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_L3

/******************************************************************************
 * Local definitions
 */

/* Start- For imm-based L3_PROTECTION_ENABLE LT. */
typedef int (*l3_prot_enable_set_f)(int unit, uint32_t trans_id,
                                    bcmltd_sid_t lt_id, uint32_t nhop_id, bool prot);

/* Array of device specific fnptr_attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    _bc##_l3_prot_enable_set,

static l3_prot_enable_set_f l3_prot_enable_set[] = {
    NULL, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    NULL  /* end-of-list */
};
/* End- For imm-based L3_PROTECTION_ENABLE LT. */

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Enable/disable L3_PROTECTION_ENABLE.PROTECTION.
 * Note: just used for imm-based L3_PROTECTION_ENABLE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id The transaction ID associated with this operation.
 * \param [in] lt_id Logical tabld ID.
 * \param [in] nhop Nexthop ID.
 * \param [in] prot Enable/disable protection.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
int
bcmcth_l3_prot_enable_set(int unit, uint32_t trans_id,
                          bcmltd_sid_t lt_id, uint32_t nhop_id, bool prot)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_prot_enable_set[dev_type](unit, trans_id, lt_id, nhop_id, prot));

exit:
    SHR_FUNC_EXIT();
}
