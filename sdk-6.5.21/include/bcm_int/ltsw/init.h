/*! \file init.h
 *
 * Init header file.
 * This file contains init definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_INIT_H
#define BCMI_LTSW_INIT_H

/*!
 * \brief Indicate if the device is in bcm init stage or not.
 *
 * \param [in] unit Unit number
 *
 * \retval In bcm init stage or not.
 */
extern int
bcmi_ltsw_init_state_get(int unit);

#endif /* BCMI_LTSW_INIT_H */
