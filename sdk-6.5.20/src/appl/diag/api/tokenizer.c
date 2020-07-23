/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tokenizer.c
 * Purpose:     API mode tokenizer
 */

/*
  //
  // dot format tokenizer state machine
  //
  // character classes
  // W = whitespace
  // Q = quote characters
  // Q' = starting quote character
  // E = escape character '\'
  // S = separator ;=?.{}
  // T = token character !(W|Q|E)
  // C = comment character
  // 0 = end of string

  // + = allocate token
  // - = delimit token

  digraph tokenizer {
    start -> search
    search -> search   [label="W"]
    search -> quote    [label="Q+"]
    search -> error    [label="E"]
    search -> token    [label="T+"]
    search -> sep      [label="S+"]
    search -> comment  [label="C+"]
    search -> end      [label="0-"]
    quote -> quote     [label="!Q'"]
    quote -> delim     [label="Q'"]
    quote -> error     [label="0-"]
    delim -> search    [label="W-"]
    delim -> sep       [label="S-+"]
    delim -> error     [label="T|Q|E"]
    delim -> comment   [label="C+"]
    delim -> end       [label="0-"]
    token -> token     [label="T"]
    token -> search    [label="W-"]
    token -> sep       [label="S-+"]
    token -> comment   [label="C+"]
    token -> error     [label="Q|E"]
    token -> end       [label="0-"]
    sep -> search      [label="W-"]
    sep -> token       [label="T-+"]
    sep -> quote       [label="Q-+"]
    sep -> sep         [label="S-+"]
    sep -> comment     [label="C+"]
    sep -> error       [label="E"]
    sep -> end         [label="0-"]
    comment -> comment [label="!(0|N)"]
    comment -> search  [label="N"]
    comment -> end     [label="0"]
  }
 */

#include "sal/core/alloc.h"
#include "sal/core/libc.h"
#include "tokenizer.h"

typedef enum {
    SEARCH,
    TOKEN,
    QUOTE,
    DELIM,
    SEP,
    COMMENT,
    TKERR       /* Something in vxWorks defined an ERROR macro */
} tkstate_t;

typedef enum {
    C,          /* comment */
    E,          /* escape */
    N,          /* newline */
    Q,          /* quote */
    S,          /* separator */
    T,          /* token */
    W           /* whitespace */
} tkchar_t;

/*
  Tokenize input string.

  An unquoted token is a contiguous sequence of non-whitespace
  characters, except single quote (') douple quote (") and backslash
  (\).

  A quoted token is delimited by single or double quotes. A quote
  character may be escaped with a backslash.

  It is an error for a backslash character to appear outside of a
  quoted token, or for a quoted token to be immediately followed by
  another token (quoted or not) with no intervening whitespace.
  
  Returns 1 if the tokenization completed without error, or zero if
  there was an error. Tokenization stops when an error is detected, so
  if there is an error, the last token is the erroneous one.

  input: input string to tokenize
  tokens: token buffer
  write: true to write tokens out, false to only count

 */

/* return true if the character is the beginning of an identifier */
STATIC int
_is_ident(int c)
{
    return ((c == '_') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z'));
}

/*
 * Function:
 *     _api_mode_tokenizer(const char *input,
 *                         api_mode_tokens_t *tokens, int write)
 *     
 * Purpose:
 *     Tokenize a string.
 * Parameters:
 *      input    - (IN)     Input string
 *      tokens   - (IN/OUT) Token structure
 *      write    - (IN)     Create tokens if true, Count tokens if false
 * Returns:
 *      0     - no errors
 *     -1     - parse error
 * Notes:
 *      Tokenizer kernel.
 */

STATIC int
_api_mode_tokenizer(const char *input, api_mode_tokens_t *tokens, int write)
{
    tkstate_t state;
    tkchar_t cclass;
    int c;
    int start_quote;
    int escape;
    int delim;
    int token;
    int line;
    int new_line;
    int first_line;
    int first_column;
    int last_column;
    int ident;
    const char *base;
    char *p;
    api_mode_token_type_t token_type;

    p = (write) ? tokens->buf : NULL;
    tokens->len = 0;
    state = SEARCH;
    start_quote = 0;
    escape = 0;
    delim = 0;
    token = 0;
    line = 0;
    new_line = 0;
    first_line = 0;
    first_column = 0;
    ident = 0;
    base = input;
    token_type = API_MODE_TOKEN_TYPE_ERROR;
    tokens->error = API_MODE_TOKEN_ERROR_INIT;

    for (;(c=*input) && state != TKERR; input++) {

        if (new_line) {
            line++;
            base = input;
            new_line = 0;
        }
        
        /* classify character */
        if (c <= ' ') {
            cclass = W;
            if (c == '\n') {
                new_line = 1;
            }
        } else if (c == '"' || c == '\'') {
            cclass = Q;
        } else if (   c == ';'
                   || c == '='
                   || c == '?'
                   || c == '.'
                   || c == '!'
                   || c == '@'
                   || c == '$' 
                   || c == '%'
                   || c == '^'
                   || c == '&'
                   || c == '*' 
                   || c == '+'
                   || c == '-'
                   || c == '~'
                   || c == '`' 
                   || c == '|'
                   || c == ':'
                   || c == ','
                   || c == '/' 
                   || c == '{'
                   || c == '}'
                   || c == '('
                   || c == ')'
                   || c == '<'
                   || c == '>'
                   || c == '['
                   || c == ']') {
            cclass = S;
        } else if (c == '\\') {
            cclass = E;
        } else if (c == '#') {
            cclass = C;
        } else {
            cclass = T;
        }

        switch (state) {
        case SEARCH:
            if (cclass == T) {
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_VALUE;
                state = TOKEN;
                ident = _is_ident(c);
            } else if (cclass == S) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_SEPARATOR;
                state = SEP;
            } else if (cclass == Q) {
                start_quote = c;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_QUOTE;
                state = QUOTE;
            } else if (cclass == C) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_COMMENT;
                state = COMMENT;
            } else if (cclass == E) {
                tokens->error = API_MODE_TOKEN_ERROR_ESCAPE;
                state = TKERR;
            }
            break;
        case TOKEN:
            if (cclass == W) {
                delim = 1;
                state = SEARCH;
            } else if (cclass == S) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_SEPARATOR;
                state = SEP;
            } else if (cclass == C) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_COMMENT;
                state = COMMENT;
            } else if (cclass == Q) {
                tokens->error = API_MODE_TOKEN_ERROR_QUOTE;
                state = TKERR;
            } else if (cclass == E) {
                tokens->error = API_MODE_TOKEN_ERROR_ESCAPE;
                state = TKERR;
            }
            break;
        case QUOTE:
            if (escape) {
                escape = 0;
            } else if (c == start_quote) {
                state = DELIM;
            } else if (cclass == E) {
                escape = 1;
            }
            break;
        case DELIM:
            if (cclass == W) {
                delim = 1;
                state = SEARCH;
            } else if (cclass == S) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_SEPARATOR;
                state = SEP;
            } else if (cclass == C) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_COMMENT;
                state = COMMENT;
            } else if (cclass == Q) {
                tokens->error = API_MODE_TOKEN_ERROR_QUOTE;
                state = TKERR;
            } else if (cclass == E) {
                tokens->error = API_MODE_TOKEN_ERROR_ESCAPE;
                state = TKERR;
            } else {
                tokens->error = API_MODE_TOKEN_ERROR_CHAR;
                state = TKERR;
            }
            break;
        case SEP:
            if (cclass == W) {
                delim = 1;
                state = SEARCH;
            } else if (cclass == S) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_SEPARATOR;
                state = SEP;
            } else if (cclass == Q) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_QUOTE;
                state = QUOTE;
                start_quote = c;
            } else if (cclass == T) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_VALUE;
                state = TOKEN;
                ident = _is_ident(c);
            } else if (cclass == C) {
                delim = 1;
                token = 1;
                token_type = API_MODE_TOKEN_TYPE_COMMENT;
                state = COMMENT;
            } else {
                /* should never get here unless there's an error in
                   character classification */
                tokens->error = API_MODE_TOKEN_ERROR_UNEXPECTED;
                state = TKERR;
            }
            break;
        case COMMENT:
            /* just throw away characters until newline or done */
            if (new_line) {
                delim = 1;
                state = SEARCH;
            }
            break;
        default:
            /* should never get here unless a state got added but not
               handled */
            tokens->error = API_MODE_TOKEN_ERROR_STATE;
            state = TKERR;
            /* fall through */
        case TKERR:
            /* should never get here unless the loop fails to exit on
               the error state. */
            break;
        }

        /* delimit */
        if (p && delim) {
            *p++ = 0;
            delim = 0;
        }

        /* new token */
        if (token) {
            if (write) {
                last_column = input - base;
                tokens->token[tokens->len].str = p;
                tokens->token[tokens->len].first_line = first_line;
                tokens->token[tokens->len].first_column = first_column;
                tokens->token[tokens->len].last_line = line;
                tokens->token[tokens->len].last_column = last_column;
                tokens->token[tokens->len].token_type = token_type;
                tokens->token[tokens->len].ident =
                    (token_type == API_MODE_TOKEN_TYPE_VALUE) && ident;
                first_line = line;
                first_column = last_column+1;
                
            }
            tokens->len++;
            token = 0;
        }

        if (p && state != SEARCH) {
            *p++ = c;
        }
    }

    /* delimit final string */
    if (p && ((p > tokens->buf && p[-1] != 0) || (p == tokens->buf))) {
        *p = 0;
    }

    if (state != TKERR && state != QUOTE) {
        tokens->error = API_MODE_TOKEN_ERROR_NONE;
    }
    
    return tokens->error;
}

/*
 * Function:
 *     api_mode_tokenizer(const char *input,
 *                        api_mode_tokens_t *tokens)
 *     
 * Purpose:
 *     Tokenize a string.
 * Parameters:
 *      input    - (IN)     Input string
 *      tokens   - (IN/OUT) Token structure
 * Returns:
 *      0    - no errors
 *     -1    - memory or parse error
 * Notes:
 *      Top level tokenizer. Call api_mode_tokenizer_free() when
 *      done with the token structure.
 */

int
api_mode_tokenizer(const char *input, api_mode_tokens_t *tokens)
{
    int rv,buf_len, alloc_len;

    /* first pass to count characters */
    if ((rv=_api_mode_tokenizer(input, tokens, 0)) == 0) {
        buf_len = alloc_len = (sal_strlen(input)+1)*2;
        alloc_len += (tokens->len * sizeof(tokens->token[0]));
    
        tokens->buf = sal_alloc(alloc_len, "tokenizer");
        if (tokens->buf) {
            char *token_buf = tokens->buf + buf_len;
            tokens->token = (api_mode_token_t *)token_buf;
            /* second pass to actually tokenize */
            rv = _api_mode_tokenizer(input, tokens, 1);
        } else {
            rv = -1;
        }
    } else {
        tokens->buf = NULL;
    }

    return rv;
}

/*
 * Function:
 *     api_mode_tokenizer_free(api_mode_tokens_t *tokens)
 *     
 * Purpose:
 *     Free allocated token data
 * Parameters:
 *      tokens   - (IN) Token structure
 * Returns:
 *      0    - no errors
 *     -1    - memory or parse error
 * Notes:
 *      Top level tokenizer. Call api_mode_tokenizer_free() when
 *      done with the token structure.
 */

int
api_mode_tokenizer_free(api_mode_tokens_t *tokens)
{
    if (tokens->buf) {
        sal_free(tokens->buf);
    }

    return 0;
}

STATIC const char *tokenizer_error[] = {
    "no error",
    "never progressed beyond initial state",
    "escape character unexpected",
    "quote character unexpected",
    "non-delimiter character unexpected",
    "character classification error",
    "unknown tokenizer state",
    "unknown error"
};

/*
 * Function:
 *     api_mode_tokenizer_error_str(api_mode_token_error_t error)
 *     
 * Purpose:
 *     Return an error string corresponding to a token error number.
 * Parameters:
 *      error   - (IN) token error number
 * Returns:
 *      string
 */

const char *
api_mode_tokenizer_error_str(api_mode_token_error_t error)
{
    if (error < 0 || error > API_MODE_TOKEN_ERROR_LAST) {
        error = API_MODE_TOKEN_ERROR_LAST;
    }
    return tokenizer_error[error];
}
