/*! \file bcmlu_knet.h
 *
 * Linux user mode connector for KNET module.
 *
 * This module provides a number of APIs for communicating with Linux KNET.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLU_KNET_H
#define BCMLU_KNET_H

/*!
 * \brief Attach user mode driver to kernel driver.
 *
 * \retval 0 No errors
 */
extern int
bcmlu_knet_attach(void);

/*!
 * \brief Detach user mode driver from kernel driver.
 *
 * \retval 0 No errors
 */
extern int
bcmlu_knet_detach(void);

#endif /* BCMLU_KNET_H */
