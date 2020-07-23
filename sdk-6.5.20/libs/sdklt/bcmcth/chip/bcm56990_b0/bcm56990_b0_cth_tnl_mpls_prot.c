/*! \file bcm56990_b0_cth_tnl_mpls_prot.c
 *
 * This file defines the detach, attach routines of
 * TNL mpls protection switching driver for bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmltd/bcmltd_handler.h>

#include <bcmcth/bcmcth_tnl_mpls_prot.h>
#include <bcmcth/bcmcth_tnl_util.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL
#define NHOPS_PER_ENTRY 128

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Tunnel MPLS protection enable set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56990_b0_tnl_mpls_prot_enable_set(int unit, const bcmltd_op_arg_t *op_arg,
                                     bcmltd_sid_t lt_id, uint32_t nhop_id,
                                     bool prot)
{
    PROT_NHI_TABLE_1m_t prot_buf;
    uint32_t ent_idx, position;
    uint32_t alloc_sz;
    SHR_BITDCL *fld_buf = NULL;

    SHR_FUNC_ENTER(unit);

    ent_idx = (nhop_id / NHOPS_PER_ENTRY);
    position = (nhop_id % NHOPS_PER_ENTRY);
    alloc_sz = SHR_BITALLOCSIZE(NHOPS_PER_ENTRY);
    SHR_ALLOC(fld_buf, alloc_sz, "bcmcthTnlMplsProtHwFld");
    SHR_NULL_CHECK(fld_buf, SHR_E_MEMORY);
    sal_memset(fld_buf, 0, alloc_sz);

    PROT_NHI_TABLE_1m_CLR(prot_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_tnl_hw_read(unit, op_arg, lt_id, PROT_NHI_TABLE_1m,
                            ent_idx, &prot_buf));
    PROT_NHI_TABLE_1m_REPLACE_ENABLE_BITMAPf_GET(prot_buf, fld_buf);
    if (prot) {
        SHR_BITSET(fld_buf, position);
    } else {
        SHR_BITCLR(fld_buf, position);
    }
    PROT_NHI_TABLE_1m_REPLACE_ENABLE_BITMAPf_SET(prot_buf, fld_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_tnl_hw_write(unit, op_arg, lt_id, PROT_NHI_TABLE_1m,
                             ent_idx, &prot_buf));
exit:
    SHR_FREE(fld_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tunnel MPLS protection enable get.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection PTR.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56990_b0_tnl_mpls_prot_enable_get(int unit, const bcmltd_op_arg_t *op_arg,
                                     bcmltd_sid_t lt_id, uint32_t nhop_id,
                                     bool *prot)
{
    PROT_NHI_TABLE_1m_t prot_buf;
    uint32_t ent_idx, position;
    uint32_t alloc_sz;
    SHR_BITDCL *fld_buf = NULL;

    SHR_FUNC_ENTER(unit);

    ent_idx = (nhop_id / NHOPS_PER_ENTRY);
    position = (nhop_id % NHOPS_PER_ENTRY);
    alloc_sz = SHR_BITALLOCSIZE(NHOPS_PER_ENTRY);
    SHR_ALLOC(fld_buf, alloc_sz, "bcmcthTnlMplsProtHwFld");
    SHR_NULL_CHECK(fld_buf, SHR_E_MEMORY);
    sal_memset(fld_buf, 0, alloc_sz);

    PROT_NHI_TABLE_1m_CLR(prot_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_tnl_hw_read(unit, op_arg, lt_id, PROT_NHI_TABLE_1m,
                            ent_idx, &prot_buf));
    PROT_NHI_TABLE_1m_REPLACE_ENABLE_BITMAPf_GET(prot_buf, fld_buf);
    *prot = SHR_BITGET(fld_buf, position);
exit:
    SHR_FREE(fld_buf);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/* Driver structure. */
bcmcth_tnl_mpls_prot_drv_t bcm56990_b0_tnl_mpls_prot_drv = {
    .mpls_prot_set = bcm56990_b0_tnl_mpls_prot_enable_set,
    .mpls_prot_get = bcm56990_b0_tnl_mpls_prot_enable_get,
};

bcmcth_tnl_mpls_prot_drv_t *
bcm56990_b0_cth_tnl_mpls_prot_drv_get(int unit)
{
    return &bcm56990_b0_tnl_mpls_prot_drv;
}
