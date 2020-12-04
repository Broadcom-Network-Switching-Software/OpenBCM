/*! \file bcmcth_tnl_mpls_prot.h
 *
 * TNL_MPLS_PROTECTION_ENABLE  Custom Handler header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_MPLS_PROT_H
#define BCMCTH_TNL_MPLS_PROT_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_bitop.h>

/*!
 * MPLS protection switching lt structure
 */
typedef struct bcmcth_tnl_mpls_prot_en_lt_s {
    /*! Nexthop id. */
    uint32_t    nhop_id;
    /*! Enable/Disable protection. */
    bool        protection;
    /*! Validity of protect field. */
    bool        protection_valid;
} bcmcth_tnl_mpls_prot_en_lt_t;

/*!
 * \brief Enable/disable TNL_MPLS_PROTECTION_ENABLE.PROTECTION PTR.
 * Note: just used for imm-based TNL_MPLS_PROTECTION_ENABLE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical tabld ID.
 * \param [in] nhop Nexthop ID.
 * \param [in] prot Enable/disable protection.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*bcmcth_tnl_mpls_prot_set_f)(int unit,
                                          const bcmltd_op_arg_t *op_arg,
                                          bcmltd_sid_t lt_id,
                                          uint32_t nhop_id, bool prot);

/*!
 * \brief Get TNL_MPLS_PROTECTION_ENABLE.PROTECTION PTR.
 * Note: just used for imm-based TNL_MPLS_PROTECTION_ENABLE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical tabld ID.
 * \param [in] nhop Nexthop ID.
 * \param [in] prot Enable/disable protection ptr.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*bcmcth_tnl_mpls_prot_get_f)(int unit,
                                          const bcmltd_op_arg_t *op_arg,
                                          bcmltd_sid_t lt_id,
                                          uint32_t nhop_id, bool* prot);

/*!
 * \brief MPLS protect driver object
 *
 * MPLS protect driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH MPLS module by
 * \ref bcmcth_tnl_mpls_prot_drv_init from the top layer. BCMCTH MPLS internally
 * uses this interface to get the corresponding information.
 */
typedef struct bcmcth_tnl_mpls_prot_drv_s {
    /*! Set the hardware for TNL_MPLS_PROTECTION_ENABLEt.PROTECTION driver. */
    bcmcth_tnl_mpls_prot_set_f  mpls_prot_set;

    /*! Get the hardware for TNL_MPLS_PROTECTION_ENABLEt.PROTECTION driver. */
    bcmcth_tnl_mpls_prot_get_f  mpls_prot_get;
} bcmcth_tnl_mpls_prot_drv_t;


/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_tnl_mpls_prot_drv_t *_bc##_cth_tnl_mpls_prot_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific MPLS driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_tnl_mpls_prot_drv_init(int unit);

/*!
 * \brief Enable/Disable mpls protection, applicable for XFS device.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_tnl_mpls_prot_enable_set(int unit, const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id, uint32_t nhop_id, bool prot);

/*!
 * \brief Get mpls protection, applicable for XFS device.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection ptr.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_tnl_mpls_prot_enable_get(int unit, const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                uint32_t nhop_id, bool *prot);
#endif /* BCMCTH_TNL_MPLS_PROT_H */
