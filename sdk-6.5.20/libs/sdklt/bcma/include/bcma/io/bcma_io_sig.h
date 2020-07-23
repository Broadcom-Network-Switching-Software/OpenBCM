/*! \file bcma_io_sig.h
 *
 * POSIX signal abstraction.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_IO_SIG_H
#define BCMA_IO_SIG_H

/*!
 * \brief Send POSIX signal SIGINT.
 *
 * \retval 0 Signal sent successfully.
 */
extern int
bcma_io_send_sigint(void);

/*!
 * \brief Send POSIX signal SIGQUIT.
 *
 * \retval 0 Signal sent successfully.
 */
extern int
bcma_io_send_sigquit(void);

#endif /* BCMA_IO_SIG_H */
