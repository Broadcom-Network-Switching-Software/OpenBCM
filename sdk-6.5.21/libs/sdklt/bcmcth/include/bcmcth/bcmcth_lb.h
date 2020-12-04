/*! \file bcmcth_lb.h
 *
 * BCMCTH Load Balance (LB) top-level APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LB_H
#define BCMCTH_LB_H

/*!
 * \brief Attach LB device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_lb_attach(int unit, bool warm);

/*!
 * \brief Clean up LB device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_lb_detach(int unit);

#endif /* BCMCTH_LB_H */
