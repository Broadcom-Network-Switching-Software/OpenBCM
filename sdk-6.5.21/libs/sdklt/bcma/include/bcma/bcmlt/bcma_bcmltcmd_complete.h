/*! \file bcma_bcmltcmd_complete.h
 *
 * Completion support for BCMLT commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_COMPLETE_H
#define BCMA_BCMLTCMD_COMPLETE_H

#include <bcma/clirlc/bcma_clirlc.h>

/*!
 * \brief Completion function for logical table access command.
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
extern int
bcma_bcmltcmd_lt_complete(bcma_cli_t *cli, bcma_cli_tokens_t *ctoks,
                          const char *text, int start, int end,
                          bcma_clirlc_t *complete);

/*!
 * \brief Completion function for physical table access command.
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
extern int
bcma_bcmltcmd_pt_complete(bcma_cli_t *cli, bcma_cli_tokens_t *ctoks,
                          const char *text, int start, int end,
                          bcma_clirlc_t *complete);

#endif /* BCMA_BCMLTCMD_COMPLETE_H */
