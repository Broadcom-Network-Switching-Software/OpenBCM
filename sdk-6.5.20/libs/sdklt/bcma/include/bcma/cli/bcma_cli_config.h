/*! \file bcma_cli_config.h
 *
 * CLI configuration setting.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_CONFIG_H
#define BCMA_CLI_CONFIG_H

/*! Use unit prefix in CLI command prompt. */
#ifndef BCMA_CLI_CONFIG_UNIT_PREFIX
#define BCMA_CLI_CONFIG_UNIT_PREFIX          1
#endif

/*! Maximum number of help line entries in command structure. */
#ifndef BCMA_CLI_CONFIG_MAX_HELP_LINES
#define BCMA_CLI_CONFIG_MAX_HELP_LINES       2
#endif

/*! Maximum length of raw command line. */
#ifndef BCMA_CLI_CONFIG_INPUT_BUFFER_MAX
#define BCMA_CLI_CONFIG_INPUT_BUFFER_MAX     8192
#endif

/*! Maximum length of buffer for parsed commands. */
#ifndef BCMA_CLI_CONFIG_ARGS_BUFFER_MAX
#define BCMA_CLI_CONFIG_ARGS_BUFFER_MAX      BCMA_CLI_CONFIG_INPUT_BUFFER_MAX
#endif

/*! Maximum number tokens in a single CLI command. */
#ifndef BCMA_CLI_CONFIG_ARGS_CNT_MAX
#define BCMA_CLI_CONFIG_ARGS_CNT_MAX         512
#endif

/*! Maximum size of a token */
#ifndef BCMA_CLI_CONFIG_ARGS_SIZE_MAX
#define BCMA_CLI_CONFIG_ARGS_SIZE_MAX       80
#endif

/*! Maximum length of CLI command prompt. */
#ifndef BCMA_CLI_CONFIG_IO_MAX_PROMPT
#define BCMA_CLI_CONFIG_IO_MAX_PROMPT        32
#endif

/*! Prefix string for CLI error messages. */
#ifndef BCMA_CLI_CONFIG_ERROR_STR
#define BCMA_CLI_CONFIG_ERROR_STR            "ERROR: "
#endif

/*! Default unit to operate on. */
#ifndef BCMA_CLI_DEFAULT_UNIT
#define BCMA_CLI_DEFAULT_UNIT                0
#endif

#endif /* BCMA_CLI_CONFIG_H */
