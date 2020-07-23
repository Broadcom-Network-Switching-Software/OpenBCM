/*! \file bcma_cli_arg.h
 *
 * Definition of CLI argument.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_ARG_H
#define BCMA_CLI_ARG_H

#include <sal/sal_types.h>

#include <bcma/cli/bcma_cli_config.h>

/*!
 * \brief CLI command arguments structure.
 *
 * A raw command line is parsed into a number of tokens stored within
 * this structure. A CLI command implementation can use a set of macros
 * (BCMA_CLI_ARG_xxx) to extract the command arguments.
 *
 * If the macro \ref BCMA_CLI_ARG_GET is used to consume parameters, unused
 * parameters may be passed to lower levels by simply passing the
 * args_t struct.
 */
typedef struct bcma_cli_args_s {

    /*! Storage for parsed arguments. */
    char buffer[BCMA_CLI_CONFIG_ARGS_BUFFER_MAX];

    /*! Number parsed command line arguments. */
    int argc;

    /*! Array of parsed command line arguments. */
    char *argv[BCMA_CLI_CONFIG_ARGS_CNT_MAX];

    /*! Pointer to next argument. */
    int arg;

    /*! Device number. */
    int unit;

} bcma_cli_args_t;


/*! Return first argument in argument list (presumably the command). */
#define BCMA_CLI_ARG_CMD(_a) \
    (_a)->argv[0]

/*! Return the number of arguments in argument list. */
#define BCMA_CLI_ARG_CNT(_a) \
    ((_a)->argc - (_a)->arg)

/*! Get the current argument in argument list. */
#define BCMAi_CLI_ARG_CUR(_a) \
    ((_a)->argv[(_a)->arg])

/*! Get the current argument in argument list with boundary check. */
#define BCMA_CLI_ARG_CUR(_a) \
    (((_a)->arg >= (_a)->argc) ? NULL : BCMAi_CLI_ARG_CUR(_a))

/*! Get the current argument and move to next argument. */
#define BCMAi_CLI_ARG_GET(_a) \
    ((_a)->argv[(_a)->arg++])

/*! Get the current argument and move to next argument with boundary check. */
#define BCMA_CLI_ARG_GET(_a) \
    (((_a)->arg >= (_a)->argc) ? NULL : BCMAi_CLI_ARG_GET(_a))

/*! Move to next argument. */
#define BCMA_CLI_ARG_NEXT(_a) \
    (_a)->arg++

/*! Move to previous argument. */
#define BCMA_CLI_ARG_PREV(_a) \
    (_a)->arg--

/*! Discard remaining arguments. */
#define BCMA_CLI_ARG_DISCARD(_a) \
    ((_a)->arg = (_a)->argc)

#endif /* BCMA_CLI_ARG_H */
