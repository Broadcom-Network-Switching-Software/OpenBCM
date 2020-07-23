/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if !defined(_SAL_EDITLINE_H)
#define _SAL_EDITLINE_H

#ifdef INCLUDE_EDITLINE

#if	!defined(CONST)
#if	defined(__STDC__)
#define CONST	const
#else
#define CONST
#endif	/* defined(__STDC__) */
#endif	/* !defined(CONST) */

/*
 * In hybrid system(concatenating SDKLT to SDK6), both SDK6 and SDKLT have
 * editline library and command line completion. But they shared the same
 * function name. e.g. readline(), add_history() and rl_insert(). In order
 * to keep the completion for each command line, these three functions are
 * renamed to el_readline(), el_add_history and el_insert().
 */
#define readline el_readline
#define add_history el_add_history

extern char	*readline(CONST char *prompt);
extern void	add_history(char *p);

/* editline asynchronous callback interface */

/** Called when an end of line is reached. */
typedef void (*rl_vcpfunc_t)(char *line, void *ctx);
/** Called when an end of file is reached. */
typedef void (*rf_vcpfunc_t)(void *ctx);
extern void rl_callback_read_char(CONST char *prompt);

/** Called when asynchronous edit line support is no longer needed. */
extern void rl_callback_handler_remove(void **eolCtx, void **eofCtx);
                                                                               
/** Must be called initially to enable asynchronous edit line support. */
extern void rl_callback_handler_install(CONST char *prompt, 
                                       rl_vcpfunc_t eol_handler, void *eolCtx,
                                       rf_vcpfunc_t eof_handler, void *eofCtx);


typedef struct rl_input_state_s {
    unsigned char *line;
    int point;
} rl_input_state_t;

extern void rl_input_state(rl_input_state_t *state);

extern char	*(*rl_complete)(char *, int *);
extern int	(*rl_list_possib)(char *, char ***);

#endif /* INCLUDE_EDITLINE */

#endif /* _SAL_EDITLINE_H */

