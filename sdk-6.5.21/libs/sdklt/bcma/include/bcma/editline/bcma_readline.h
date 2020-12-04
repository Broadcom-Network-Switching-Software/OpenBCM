/*! \file bcma_readline.h
 *
 * Top-level header file for readline API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_READLINE_H
#define BCMA_READLINE_H

/*
 * By default we use the self-contained and compact editline
 * implementation of the major readline APIs.
 *
 * If BCMA_NO_EDITLINE is defined, we will expect an external version
 * of the GNU readline library to be present.
 */
#ifndef BCMA_NO_EDITLINE
#define BCMA_NO_EDITLINE 0
#endif

#if BCMA_NO_EDITLINE == 1

/*******************************************************************************
 * Definitions for GNU readline
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

/*
 * Dummy functions to allow seamless switching between editline and
 * GNU readline, i.e. w/o any need for #ifdefs.
 */
#include <bcma/editline/bcma_editline_io.h>

static inline int
bcma_editline_init(bcma_editline_io_cb_t *io_cb,
                   bcma_editline_term_keys_t *term_keys)
{
    using_history();
    return 0;
}

static inline int
bcma_editline_cleanup(void)
{
    clear_history();
    return 0;
}

static inline char *
bcma_editline_history_get(int idx)
{
    HIST_ENTRY *hist_en;

    if (idx < 0) {
        idx = history_base + history_length + idx;
    }
    hist_en = history_get(idx);
    if (hist_en) {
        return hist_en->line;
    }
    return NULL;
}

static inline int
bcma_editline_history_length(void)
{
    return history_length;
}

/*
 * The readline functions may use a different set of allocation APIs
 * than the rest of the application, so we need special versions to
 * allow the application to free readline-allocated memory and vice
 * versa.
 */

#define bcma_rl_alloc(_sz) \
    malloc(_sz)

#define bcma_rl_free(_ptr) \
    free(_ptr)

#define bcma_rl_strdup(_str) \
    strdup(_str)

#else /* BCMA_NO_EDITLINE == 0 */

/*******************************************************************************
 * Definitions for SDK readline (editline)
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/editline/bcma_editline.h>

#define bcma_rl_alloc(_sz) \
    sal_alloc(_sz, "bcmaCliEditline")

#define bcma_rl_free(_ptr) \
    sal_free(_ptr)

#define bcma_rl_strdup(_str) \
    sal_strdup(_str)

#endif /* BCMA_NO_EDITLINE */

/*
 * Multiple CLI instances are created to implement different shell
 * interfaces, but all the CLI instances share the same terminal input
 * function, which normally is readline.
 *
 * For readline completion support we need to know the current CLI
 * context, because this affects how the CLI command parameters should
 * be completed.
 *
 * The bcma_readline_context variable is used to identify the currently active
 * CLI instance in order to supply the correct CLI context for readline
 * completion support.
 */
void *bcma_readline_context;

static inline void
bcma_readline_context_set(void *context)
{
    bcma_readline_context = context;
}

static inline void*
bcma_readline_context_get(void)
{
    return bcma_readline_context;
}

#endif /* BCMA_READLINE_H */
