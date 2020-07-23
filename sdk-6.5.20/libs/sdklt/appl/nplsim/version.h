/*! \file version.h
 *
 * Application version information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef VERSION_H
#define VERSION_H

/*!
 * \brief Initialize version string.
 */
extern void
version_init(void);

/*!
 * \brief Print sign-on message.
 */
extern void
version_signon(void);

#endif /* VERSION_H */
