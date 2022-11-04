/*  $Revision: 1.5 $
**
**  Internal header file for editline library.
*/

#ifdef INCLUDE_EDITLINE

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>

#if	defined(HAVE_STDLIB)
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif	/* defined(HAVE_STDLIB) */

#if	defined(SYS_UNIX)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#if     defined(USE_DIRENT)
#include <dirent.h>
typedef struct dirent   DIRENTRY;
#else
#include <sys/dir.h>
typedef struct direct   DIRENTRY;
#endif  /* defined(USE_DIRENT) */
#if     !defined(S_ISDIR)
#define S_ISDIR(m)              (((m) & S_IFMT) == S_IFDIR)
#endif  /* !defined(S_ISDIR) */
#endif	/* defined(SYS_UNIX) */

#if	!defined(SIZE_T)
#define SIZE_T	unsigned int
#endif	/* !defined(SIZE_T) */

#define CRLF		"\r\n"
#define FORWARD		STATIC

typedef unsigned char	CHAR;

#ifndef STATIC
#if	defined(HIDE)
#define STATIC	static
#else
#define STATIC	/* NULL */
#endif	/* !defined(HIDE) */
#endif

#define MEM_INC		64
#define SCREEN_INC	256

#define DISPOSE(p)	sal_free((char *)(p))
#define NEW(T, c)	\
	((T *)sal_alloc((unsigned int)(sizeof (T) * (c)), "editline"))
#define COPYFROMTO(new, p, len)	\
	(void)memcpy((char *)(new), (char *)(p), (int)(len))
/*
**  Variables and routines internal to this package.
*/
extern int	rl_eof;
extern int	rl_erase;
extern int	rl_intr;
extern int	rl_kill;
extern int	rl_quit;
#if	defined(DO_SIGTSTP)
extern int	rl_susp;
#endif	/* defined(DO_SIGTSTP) */
extern char	*(*rl_complete)(char    *pathname, int        *unique);
extern char	*rl_complete_file(char    *pathname, int        *unique);
extern int	(*rl_list_possib)(char    *pathname, char    ***avp);
extern int	rl_list_possib_file(char    *pathname, char    ***avp);
extern void	rl_ttyset(int reset);
extern void	rl_add_slash(char *path, char *p);
void rl_prompt_set(CONST char *prompt);

extern void	list_history(int count);

#if	!defined(HAVE_STDLIB)
extern char	*getenv();
extern char	*memcpy();
extern char	*strcat();
extern char	*strchr();
extern char	*strrchr();
extern char	*strcpy();
extern int	strcmp();
extern int	strlen();
extern int	strncmp();
#endif	/* !defined(HAVE_STDLIB) */

#endif /* INCLUDE_EDITLINE */
