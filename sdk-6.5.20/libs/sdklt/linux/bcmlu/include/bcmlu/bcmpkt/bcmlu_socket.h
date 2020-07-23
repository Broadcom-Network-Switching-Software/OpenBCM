/*! \file bcmlu_socket.h
 *
 * Linux user mode connector for SOCKET driver.
 *
 * This module provides a number of APIs for communicating with SOCKET.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLU_SOCKET_H
#define BCMLU_SOCKET_H

/*!
 * \brief Attach user mode driver to kernel driver.
 *
 * \retval 0 No errors
 */
extern int
bcmlu_socket_attach(void);

/*!
 * \brief Detach user mode driver from kernel driver.
 *
 * \retval 0 No errors
 */
extern int
bcmlu_socket_detach(void);

#endif /* BCMLU_SOCKET_H */
