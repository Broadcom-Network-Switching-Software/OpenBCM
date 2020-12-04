/*! \file bcmcth_tnl_mpls_drv.h
 *
 * BCMCTH TNL MPLS driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_MPLS_DRV_H
#define BCMCTH_TNL_MPLS_DRV_H

#include <sal/sal_types.h>
#include <bcmcth/bcmcth_tnl_mpls_exp_qos_ctrl.h>
#include <bcmcth/bcmcth_tnl_mpls_exp_remark_ctrl.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Set the hardware for MPLS_EXP_QOS_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] cfg Data structure to save the info of MPLS_EXP_QOS_CONTROLt entry.
 * \param [in] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_tnl_mpls_qos_ctrl_set_f)(int unit,
                                              bcmcth_tnl_mpls_exp_qos_ctrl_t *lt_entry);

/*!
 * \brief Set the hardware for MPLS_EXP_REMARK_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] cfg Data structure to save the info of MPLS_EXP_REMARK_CONTROLt entry.
 * \param [in] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_tnl_mpls_remark_ctrl_set_f)(int unit,
                                                bcmcth_tnl_mpls_exp_remark_ctrl_t *lt_entry);

/*!
 * \brief MPLS driver object
 *
 * MPLS driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH MPLS module by
 * \ref bcmcth_tnl_mpls_drv_init from the top layer. BCMCTH MPLS internally
 * uses this interface to get the corresponding information.
 */
typedef struct bcmcth_tnl_mpls_drv_s {
    /*! Set the hardware for MPLS_EXP_QOS_CONTROLt  driver. */
    bcmcth_tnl_mpls_qos_ctrl_set_f    qos_ctrl_set;

    /*! Set the hardware for MPLS_EXP_REMARK_CONTROLt driver. */
    bcmcth_tnl_mpls_remark_ctrl_set_f remark_ctrl_set;
} bcmcth_tnl_mpls_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_tnl_mpls_drv_t *_bc##_cth_tnl_mpls_drv_get(int unit);
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
bcmcth_tnl_mpls_drv_init(int unit);
#endif /* BCMCTH_TNL_MPLS_DRV_H */
