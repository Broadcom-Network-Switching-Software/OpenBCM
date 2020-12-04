/*! \file bcma_editline.h
 *
 * Editline is a simple readline replacement.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_EDITLINE_H
#define BCMA_EDITLINE_H

#ifndef BCMA_READLINE_H
#error Please include bcma_readline.h instead.
#endif

#include <bcma/editline/bcma_editline_io.h>

typedef char *rl_compentry_func_t(const char *, int);
typedef char **rl_completion_func_t(const char *, int, int);

extern int rl_point;
extern int rl_end;
extern int rl_mark;
extern int rl_catch_signals;
extern char *rl_line_buffer;
extern int rl_attempted_completion_over;
extern int rl_completion_append_character;
extern const char *rl_basic_word_break_characters;

extern rl_completion_func_t *rl_attempted_completion_function;

extern int history_base;

extern char *
readline(const char *prompt);

extern void
add_history(char *p);

extern int
history_expand(char *str, char **output);

extern int
rl_insert(int count, int c);

extern char **
rl_completion_matches(const char *text, rl_compentry_func_t *match_fn);

extern int
rl_on_new_line(void);

extern int
bcma_editline_init(bcma_editline_io_cb_t *io_cb,
                   bcma_editline_term_keys_t *term_keys);

extern int
bcma_editline_cleanup(void);

extern char *
bcma_editline_history_get(int idx);

extern int
bcma_editline_history_length(void);

#endif /* BCMA_EDITLINE_H */
