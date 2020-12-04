/*! \file bcm56880_a0_cth_l3_prot.c
 *
 * This file defines the detach, attach routines of
 * L3 nexthop protection switching driver for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmcth/bcmcth_l3_prot.h>
#include <bcmcth/bcmcth_l3_util.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L3

#define NHOPS_PER_ENTRY 256

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief L3 nexthop protection enable set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      Transaction id.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm56880_a0_l3_prot_enable_set(int unit, uint32_t trans_id,
                               bcmltd_sid_t lt_id, uint32_t nhop_id,
                               bool prot)
{
    PROT_SWT_PROT_NHI_TABLEm_t  prot_buf;
    uint32_t ent_idx, position;
    uint32_t alloc_sz;
    SHR_BITDCL *fld_buf = NULL;

    SHR_FUNC_ENTER(unit);

    ent_idx = (nhop_id / NHOPS_PER_ENTRY);
    position = (nhop_id % NHOPS_PER_ENTRY);
    alloc_sz = SHR_BITALLOCSIZE(NHOPS_PER_ENTRY);
    SHR_ALLOC(fld_buf, alloc_sz, "bcmcthL3ProtHwFld");
    SHR_NULL_CHECK(fld_buf, SHR_E_MEMORY);
    sal_memset(fld_buf, 0, alloc_sz);

    PROT_SWT_PROT_NHI_TABLEm_CLR(prot_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_l3_hw_read(unit, trans_id, lt_id, PROT_SWT_PROT_NHI_TABLEm,
                           ent_idx, &prot_buf));
    PROT_SWT_PROT_NHI_TABLEm_REPLACE_ENABLE_BITMAPf_GET(prot_buf, fld_buf);
    if (prot) {
        SHR_BITSET(fld_buf, position);
    } else {
        SHR_BITCLR(fld_buf, position);
    }
    PROT_SWT_PROT_NHI_TABLEm_REPLACE_ENABLE_BITMAPf_SET(prot_buf, fld_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_l3_hw_write(unit, trans_id, lt_id, PROT_SWT_PROT_NHI_TABLEm,
                            ent_idx, &prot_buf));
exit:
    SHR_FREE(fld_buf);
    SHR_FUNC_EXIT();
}
