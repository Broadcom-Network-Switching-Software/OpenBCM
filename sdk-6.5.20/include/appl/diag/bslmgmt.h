/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Management
 */

#ifndef _DIAG_BSLMGMT_H
#define _DIAG_BSLMGMT_H

#include <stdarg.h>
#include <shared/bslext.h>

extern int
bslmgmt_cleanup(void);

extern int
bslmgmt_init(void);

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
bslmgmt_redir_hook_set(bsl_out_hook_f out_hook);

#endif /* !_DIAG_BSLMGMT_H */
