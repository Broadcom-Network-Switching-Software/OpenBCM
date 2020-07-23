/*
 * $Id: cint_porting_sdklt.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_porting_sdklt.h
 * Purpose:     CINT porting SDKLT interfaces
 */

#ifndef CINT_PORTING_SDKLT_H
#define CINT_PORTING_SDKLT_H

#include "cint_config.h"

/*
 * The following portability macros must be defined
 * before your can build the interpreter.
 */

/*
 * Use the SDKLT SAL interface
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <bsl/bsl.h>
#include <bcma/io/bcma_io_file.h>

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE SAL_ATTR
#endif

#ifndef CINT_PRINTF
#define CINT_PRINTF cli_out
#endif

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
#ifndef CINT_READLINE
#include <bcma/editline/bcma_readline.h>
#define CINT_READLINE readline
#endif
#ifndef CINT_READLINE_FREE
#define CINT_READLINE_FREE bcma_rl_free
#endif
#endif

#if CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
#ifndef CINT_ADD_HISTORY
#include <bcma/editline/bcma_readline.h>
#define CINT_ADD_HISTORY add_history
#endif
#endif

#ifndef CINT_FATAL_ERROR
#include <bcma/cint/bcma_cint_cmd.h>
#define CINT_FATAL_ERROR(msg) cmd_cint_fatal_error(msg)
#endif

#endif /* CINT_PORTING_SDKLT_H */

