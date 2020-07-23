/*! \file bcmevm_internal.h
 *
 *  Notification internal header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMEVM_INTERNAL_H
#define BCMEVM_INTERNAL_H

/*!
 * \brief Initialize notification for specific unit.
 *
 * This function initializes a specific unit for notification.
 *
 * \param [in] unit Is the unit number to initialize.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmevm_unit_init(int unit);

/*!
 * \brief Shutdown notification for specific unit.
 *
 * This function shuts down a specific unit for notification.
 *
 * \param [in] unit Is the unit number to shut down.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmevm_unit_shutdown(int unit);


#endif /* BCMEVM_INTERNAL_H */
