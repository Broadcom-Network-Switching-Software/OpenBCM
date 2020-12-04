/*! \file bcma_bslmgmt.h
 *
 * System Log Management.
 *
 * This module implements the BSL "check" and output hooks.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLMGMT_H
#define BCMA_BSLMGMT_H

#include <bsl/bsl_ext.h>

/*!
 * \brief Clean up all BSL management resources.
 *
 * \return Always 0.
 */
extern int
bcma_bslmgmt_cleanup(void);

/*!
 * \brief Initialize the BSL management module.
 *
 * This function installs the BSL "check" and output hooks in the core
 * BSL component and initializes the output sinks (see \ref
 * bcma_bslsink.h) and the enable database (see \ref
 * bcma_bslenable.h).
 *
 * \return Always 0.
 */
extern int
bcma_bslmgmt_init(void);

/*!
 * \brief Install output redirection hook.
 *
 * An output redirection hook is used when the output from one command
 * should be used as input for another.
 *
 * If the redirection output hook returns a non-zero value, the log
 * message (command output) will not be sent to any of the installed
 * output sinks.
 *
 * \param [in] out_hook Output hook for redirection.
 *
 * \return Always 0.
 */
extern int
bcma_bslmgmt_redir_hook_set(bsl_out_hook_f out_hook);

#endif /* BCMA_BSLMGMT_H */
