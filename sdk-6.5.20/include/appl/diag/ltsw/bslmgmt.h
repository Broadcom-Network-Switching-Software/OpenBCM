/*! \file bslmgmt.h
 *
 * Bsl management definition for LTSW.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_LTSW_BSLMGMT_H
#define DIAG_LTSW_BSLMGMT_H

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
appl_ltsw_bslmgmt_redir_hook_set(bsl_out_hook_f out_hook);

#endif /* DIAG_LTSW_BSLMGMT_H */
