/*! \file bcma_cli_token.h
 *
 * CLI token functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_TOKEN_H
#define BCMA_CLI_TOKEN_H

#include <bcma/cli/bcma_cli_config.h>

/*! Tokens structure used for storing tokenized result of specified string. */
typedef struct bcma_cli_tokens_s {

    /*! Local storage for tokens */
    char str[BCMA_CLI_CONFIG_INPUT_BUFFER_MAX];

    /*! Number of tokens */
    int argc;

    /*! Token strings */
    char *argv[BCMA_CLI_CONFIG_ARGS_CNT_MAX + 1];

} bcma_cli_tokens_t;

/*!
 * \brief Parse string into \ref bcma_cli_tokens_t structure.
 *
 * Parse string \c str into tokens in \c tok structure by token delimiters
 * \c delim. The string \c str is remain unchanged. This function is useful
 * for advanced parsing CLI command argument string into tokens by
 * user-specified delimiters.
 *
 * \param [in] str String to split (will not be modified)
 * \param [out] tok Token structure to store the result
 * \param [in] delim String of allowed token delimiters
 *
 * \retval Always 0
 */
extern int
bcma_cli_tokenize(const char *str, bcma_cli_tokens_t *tok, const char *delim);

#endif /* BCMA_CLI_TOKEN_H */
