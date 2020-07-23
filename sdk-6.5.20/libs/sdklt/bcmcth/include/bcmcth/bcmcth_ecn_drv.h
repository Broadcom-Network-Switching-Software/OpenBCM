/*! \file bcmcth_ecn_drv.h
 *
 * BCMCTH Explicit Congestion Notification (ECN) driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_ECN_DRV_H
#define BCMCTH_ECN_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmcth/bcmcth_ecn_protocol.h>
#include <bcmcth/bcmcth_ecn_exp_to_ecn.h>

/*!
 * \brief Get device-specific ECN protocol register information.
 *
 * \param [in] unit Unit number.
 * \param [out] proto_info Device-specific protocol information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmcth_ecn_proto_info_get_f)(int unit, bcmcth_ecn_proto_info_t *proto_info);

/*!
 * \brief Get device-specific EXP_TO_ECN map register information.
 *
 * \param [in] unit Unit number.
 * \param [in] proto_info Device-specific protocol information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmcth_ecn_exp_to_ecn_map_set_f)(int unit, bcmcth_ecn_exp_to_ecn_map_t *lt_entry);

/*!
 * \brief ECN driver object
 *
 * ECN driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH ECN module by
 * \ref bcmcth_ecn_drv_init() from the top layer. BCMCTH ECN internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_ecn_drv_s {

    /*! Get ECN protocol register information. */
    bcmcth_ecn_proto_info_get_f proto_info_get;

    /*! Set EXP_TO_ECN map entry. */
    bcmcth_ecn_exp_to_ecn_map_set_f exp_to_ecn_map_set;

} bcmcth_ecn_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_ecn_drv_t *_bc##_cth_ecn_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the ECN driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ecn_drv_init(int unit);

/*!
 * \brief Get device-specific ECN protocol information.
 *
 * \param [in] unit Unit number.
 * \param [out] proto_info Device-specific ECN information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ecn_proto_info_get(int unit, bcmcth_ecn_proto_info_t *proto_info);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The ECN custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ecn_imm_init(int unit);

#endif /* BCMCTH_ECN_DRV_H */
