/*! \file mbcm.h
 *
 * BCM multiplexing APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_H
#define BCMI_LTSW_MBCM_H

/*!
 * \brief Common declaration of function pointer for mbcm driver.
 */
typedef int (*mbcm_ltsw_fnptr_t)(int unit, ...);

/*!
 * \brief Common declaration of mbcm driver structure.
 */
typedef struct mbcm_ltsw_drv_s mbcm_ltsw_drv_t;

/*! Size of function pointer. */
#define MBCM_LTSW_FNPTR_SZ  (sizeof(mbcm_ltsw_fnptr_t))

/*!
 * \brief Initialize the mbcm dirver for the indicated unit.
 *
 * \param [in]  unit   Unit number
 *
 * \retval SHR_E_NONE  No errors.
 */
extern int
mbcm_ltsw_init(int unit);

/*!
 * \brief Detach the mbcm driver for the specified unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_deinit(int unit);

/*!
 * \brief Initialize the per-module mbcm driver.
 *
 * \param [in,out] dst The mbcm driver to be updated.
 * \param [in] src Source mbcm driver.
 * \param [in] cnt Count of function pointers in mbcm driver.
 *
 * \retval SHR_E_NONE No errors.
 */
extern void
mbcm_ltsw_drv_init(mbcm_ltsw_drv_t *dst,
                   mbcm_ltsw_drv_t *src,
                   int cnt);

#endif /* BCMI_LTSW_MBCM_H */

