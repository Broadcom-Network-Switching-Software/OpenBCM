/*! \file l3.h
 *
 * XFS L3 fib module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XFS_L3_H
#define BCMINT_LTSW_XFS_L3_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/l3.h>

/*!
 * \brief Initialize L3 module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_init(int unit);

/*!
 * \brief De-initialize L3 module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_deinit(int unit);

/*!
 * \brief Set mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_mtu_set(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Get mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [out] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_mtu_get(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Delete mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_mtu_delete(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Recover mtu database from LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_mtu_recover(int unit, int mtu_idx);

/*!
 * \brief Get the available L3 information.
 *
 * \param [in] unit Unit Number.
 * \param [in] l3info L3 information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xfs_ltsw_l3_info_get(int unit, bcm_l3_info_t *l3info);

#endif /* BCMINT_LTSW_XFS_L3_H */
