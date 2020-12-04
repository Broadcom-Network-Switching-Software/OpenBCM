/*! \file bcma_cint.h
 *
 * Interfaces to diag shell CINT functions.
 *
 * Note - this file needs to be includable by cint_yy.h, so cannot
 * include any SDK interfaces directly.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   BCMA_CINT_H
#define   BCMA_CINT_H

/*!
 * \brief Output fatal error message from CINT core library.
 *
 * Fatal error interface to lexer - abort to parent shell.
 *
 * \param [in] msg Error message to be displayed.
 */
extern void
bcma_cint_fatal_error(char *msg);

#endif /* BCMA_CINT_H */
