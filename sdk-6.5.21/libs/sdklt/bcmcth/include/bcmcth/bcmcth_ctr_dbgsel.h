/*! \file bcmcth_ctr_dbgsel.h
 *
 * BCMCTH CTR_DBGSEL top-level APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_DBGSEL_H
#define BCMCTH_CTR_DBGSEL_H

/*!
 * \brief Attach CTR_DBGSEL device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ctr_dbgsel_attach(int unit);

/*!
 * \brief Clean up CTR_DBGSEL device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_ctr_dbgsel_detach(int unit);

#endif /* BCMCTH_CTR_DBGSEL_H */

