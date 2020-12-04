/*! \file bcmltcmd_complete_internal.h
 *
 * Definitions intended for bcmltcmd completion internal use only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTCMD_COMPLETE_INTERNAL_H
#define BCMLTCMD_COMPLETE_INTERNAL_H

#include <bcma/clirlc/bcma_clirlc.h>

/*!
 * \brief Auto-complete function for BCMLT CLI command.
 *
 * \param [in] cli CLI object.
 * \param [in] ctoks CLI argument tokens.
 * \param [in] physical Physical table if non-zero, otherwise logical table.
 * \param [in] text A character string to be completed.
 * \param [in] start Start index of \c text in \c rl_line_buffer.
 * \param [in] end End index of \c text in \c rl_line_buffer.
 * \param [out] complete Customized variables for completion support.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcmltcmd_complete(bcma_cli_t *cli, bcma_cli_tokens_t *ctoks, bool physical,
                  const char *text, int start, int end,
                  bcma_clirlc_t *complete);

#endif /* BCMLTCMD_COMPLETE_INTERNAL_H */
