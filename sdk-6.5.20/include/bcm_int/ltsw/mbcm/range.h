/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file range.h
 *
 * BCM range module APIs and structures used only internally.
 */

#ifndef BCMINT_LTSW_MBCM_RANGE_H
#define BCMINT_LTSW_MBCM_RANGE_H

#include <bcm_int/ltsw/range_int.h>

/*!
 * \brief Initialize range LT tables information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*range_tbls_info_init_f)(
    int unit,
    bcmint_range_tbls_info_t **tbls_info);

typedef int (*range_clear_f)(int unit);

/*!
 * \brief Get HW RTYPE val bsed on S/W Enum.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*range_rtype_add_f)(
    int unit,
    bcmlt_entry_handle_t range_check_lt,
    bcmint_range_tbls_info_t *tbls_info,
    bcm_range_config_t *range_config);

/*!
 * \brief Get HW RTYPE val bsed on S/W Enum.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*range_rtype_get_f)(
    int unit,
    bcmlt_entry_handle_t range_check_lt,
    bcmint_range_tbls_info_t *tbls_info,
    bcm_range_config_t *range_config);

/*!
 * \brief Field driver structure.
 */
typedef struct mbcm_ltsw_range_drv_s {

    /*! Initialize Range LT Tables information. */
    range_tbls_info_init_f range_tbls_info_init;

    /*! Clear Range LT Tables. */
    range_clear_f range_clear;

    /*! get Range_Type Hw Value . */
    range_rtype_add_f range_rtype_add;

    /*! get Range_Type Sw Enum . */
    range_rtype_get_f range_rtype_get;

} mbcm_ltsw_range_drv_t;

/*!
 * \brief Set the Range driver of the device.
 *
 * \param [in] unit Unit Number.
 * \param [in] drv Range driver to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_range_drv_set(
    int unit,
    mbcm_ltsw_range_drv_t *drv);

/*!
 * \brief Initialize Range LT Tables information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_range_tbls_info_init(
    int unit,
    bcmint_range_tbls_info_t **tbls_info);

extern int
mbcm_ltsw_range_clear(int unit);

extern int
mbcm_ltsw_range_rtype_add(
    int unit,
    bcmlt_entry_handle_t range_check_lt,
    bcmint_range_tbls_info_t *tbls_info,
    bcm_range_config_t *range_config);

extern int
mbcm_ltsw_range_rtype_get(
    int unit,
    bcmlt_entry_handle_t range_check_lt,
    bcmint_range_tbls_info_t *tbls_info,
    bcm_range_config_t *range_config);

#endif /*! BCMINT_LTSW_MBCM_RANGE_H */
