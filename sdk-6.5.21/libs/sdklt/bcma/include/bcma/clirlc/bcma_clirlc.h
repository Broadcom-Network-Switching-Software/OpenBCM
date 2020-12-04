/*! \file bcma_clirlc.h
 *
 * Readline completion support for CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLIRLC_H
#define BCMA_CLIRLC_H

#include <sal/sal_types.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcma/editline/bcma_readline.h>

/*!
 * Structure returned from \ref bcma_clirlc_func_t to customize
 * the completion support in a CLI command.
 */
typedef struct bcma_clirlc_s {

    /*!
     * Complete entry function to return the matched string one at a time and
     * end by returning a NULL pointer.
     */
    rl_compentry_func_t *entry_func;

    /*!
     * Customized character append to the single match string.
     * Default is a space.
     */
    int append_char;

} bcma_clirlc_t;

/*!
 * \brief Auto-complete function for CLI command.
 *
 * \param [in] cli CLI object.
 * \param [in] ctoks CLI argument tokens.
 * \param [in] text A character string to be completed.
 * \param [in] start Start index of \c text in \c rl_line_buffer.
 * \param [in] end End index of \c text in \c rl_line_buffer.
 * \param [out] complete Customized variables for completion support.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
typedef int (*bcma_clirlc_func_t)(bcma_cli_t *cli, bcma_cli_tokens_t *ctoks,
                                  const char *text, int start, int end,
                                  bcma_clirlc_t *complete);

/*!
 * \brief CLI command completion structure.
 *
 * This structure is used to define the CLI commands that support
 * auto-complete feature.
 */
typedef struct bcma_clirlc_cmd_s {

    /*! Command name */
    const char *name;

    /*! Command complete function */
    bcma_clirlc_func_t func;

    /*! CLI complete list link (internal) */
    struct bcma_clirlc_cmd_s *next;

} bcma_clirlc_cmd_t;

/*!
 * \brief Initialization for CLI readline completion.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_clirlc_init(void);

/*!
 * \brief Add command completion structure to completion command table.
 *
 * If NULL is passed as \c compcmd, the completion command table is cleared.
 *
 * \param [in] compcmd CLI command completion structure.
 *
 * \return Always 0.
 */
extern int
bcma_clirlc_add_cmd(bcma_clirlc_cmd_t *compcmd);

/*!
 * \brief Display message in CLI command completion function.
 *
 * \param [in] msg Message to be displayed.
 */
extern void
bcma_clirlc_info_show(const char *msg);

#endif /* BCMA_CLIRLC_H */
