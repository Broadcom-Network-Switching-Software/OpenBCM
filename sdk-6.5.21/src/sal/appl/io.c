/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SAL I/O abstraction
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/thread.h>
#include <sal/core/dpc.h>

#include <shared/bsl.h>

#include <sal/appl/io.h>

#include "sal/appl/editline/editline.h"

#include <stdarg.h>

#ifdef ECOS_LOGGING_SUPPORT
/* Logging mechanism for Mozart */
#include <cyg/logging/logging.h>
#endif /* ECOS_LOGGING_SUPPORT */


#ifndef SDK_CONFIG_SAL_READLINE
/*
 * Read input line, providing Emacs-based command history editing.
 */

char *
sal_readline(char *prompt, char *buf, int bufsize, char *defl)
{
    char *s, *full_prompt, *cont_prompt;
#ifdef INCLUDE_EDITLINE
    extern void rl_prompt_set(CONST char *prompt);
#else
    char *t;
#endif
    int len;

    if (bufsize == 0)
        return NULL;

    cont_prompt = prompt[0] ? "? " : "";
    full_prompt = sal_alloc(sal_strlen(prompt) + (defl ? sal_strlen(defl) : 0) + 8, __FILE__);
    /* coverity[secure_coding] */
    sal_strcpy(full_prompt, prompt);
    if (defl)
	sal_sprintf(full_prompt + sal_strlen(full_prompt), "[%s] ", defl);

#ifdef INCLUDE_EDITLINE

    LOG_INFO(BSL_LS_APPL_ECHO,
             (BSL_META("%s"), full_prompt));
    s = readline(full_prompt);

#else /* !INCLUDE_EDITLINE */

    t = sal_alloc(bufsize, __FILE__);    
#if defined(KEYSTONE) && defined(__ECOS)
    diag_printf("%s", full_prompt);
#else
    cli_out("%s", full_prompt);
#endif

    if ((s = sal_console_gets(t, bufsize)) == 0) {
      sal_free(t);
    } else {
	    s[bufsize - 1] = 0;
	    if ((t = strchr(s, '\n')) != 0) {
	      *t = 0;
        /* Replace garbage characters with spaces */
      }
      for (t = s; *t; t++) {
        if (*t < 32 && *t != 7 && *t != 9) {
          *t = ' ';
        }
      }
    }

#endif /* !INCLUDE_EDITLINE */

    if (s == 0) {                       /* Handle Control-D */
        buf[0] = 0;
	/* EOF */
	buf = 0;
	goto done;
    } else {
        LOG_INFO(BSL_LS_APPL_ECHO,
                 (BSL_META("%s\n"), s));
    }

    len = 0;
    if (s[0] == 0) {
	if (defl && buf != defl) {
            if ((len = sal_strlen(defl)) >= bufsize) {
                len = bufsize - 1;
            }
	    sal_memcpy(buf, defl, len);
	}
    } else {
	if ((len = sal_strlen(s)) >= bufsize) {
            len = bufsize - 1;
            cli_out("WARNING: input line truncated to %d chars\n", len);
        }
	sal_memcpy(buf, s, len);
    }
    buf[len] = 0;

    sal_free(s);

    /*
     * If line ends in a backslash, perform a continuation prompt.
     */

    if (sal_strlen(buf) != 0) {
        /*
         * Ensure that there is atleast one character available
         */
        s = buf + sal_strlen(buf) - 1;
        if (*s == '\\' && sal_readline(cont_prompt, s, bufsize - (s - buf), 0) == 0) {
            buf = 0;
        }
    }

 done:
#ifdef INCLUDE_EDITLINE
    rl_prompt_set(NULL);
#endif
    sal_free(full_prompt);
    
    return buf;
}
#endif /* !SDK_CONFIG_SAL_READLINE */

int sal_readchar(const char *prompt)
{
#ifdef INCLUDE_EDITLINE
    extern int readchar(const char *prompt);
#else
    char	buf[64];
#endif

#ifdef INCLUDE_EDITLINE
    return(readchar(prompt));
#else
    cli_out("%s", prompt);
    if (NULL == (sal_console_gets(buf, sizeof(buf)))) {
	return(EOF);
    } else {
	return(buf[0]);
    }
#endif
}

#ifdef INCLUDE_EDITLINE
/*
 * Configure readline by passing it completion routine (for TAB
 * completion handling) and a list-possible routine (for ESC-? handling).
 */

void sal_readline_config(char *(*complete)(char *pathname, int *unique),
			 int (*list_possib)(char *pathname, char ***avp))
{
    extern char		*(*rl_complete)(char *, int *);
    extern int		(*rl_list_possib)(char *, char ***);

    rl_complete = complete;
    rl_list_possib = list_possib;
}
#endif /* INCLUDE_EDITLINE */
