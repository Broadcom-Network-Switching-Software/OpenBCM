/*! \file bcmltm_sysm.h
 *
 * Logical Table Manager - System Manager Integration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_SYSM_H
#define BCMLTM_SYSM_H

/*!
 * \brief LTM System Manager start function.
 *
 * Called by System Manager to start LTM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_start(void);

#endif /* BCMLTM_SYSM_H */
