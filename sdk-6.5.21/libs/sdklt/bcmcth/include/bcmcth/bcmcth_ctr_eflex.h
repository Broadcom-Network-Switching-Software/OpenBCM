/*! \file bcmcth_ctr_eflex.h
 *
 * BCMCTH CTR_EFLEX top-level APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EFLEX_H
#define BCMCTH_CTR_EFLEX_H

/*!
 * \brief Attach CTR EFLEX device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ctr_eflex_attach(int unit, bool warm);

/*!
 * \brief Clean up CTR EFLEX device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ctr_eflex_detach(int unit);

/*!
 * \brief Attach STATE EFLEX device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_state_eflex_attach(int unit, bool warm);

/*!
 * \brief Clean up STATE EFLEX device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_state_eflex_detach(int unit);

#endif /* BCMCTH_CTR_EFLEX_H */
