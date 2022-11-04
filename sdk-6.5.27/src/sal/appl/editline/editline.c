/*  $Revision: 1.21 $
**
**  Main editing routines for editline library.
**
** There are two editline implementations: synchronous and asynchronsou.
** Synchronous is a blocking read call readline(CONST char *prompt).
** This call returns after a whole line has been read, returning that
** line to the user. The returned line could have been pulled from history
** or directly from stdin.
**
** Asynchronous is a character process non-blocking call, which make 
** a callout to an end-of-line handler. The entry points for these calls
** are:
** rl_callback_handler_install(CONST char *prompt, rl_vcpfunc_t eol_handler, 
** rl_callback_read_char(void)
** rl_callback_handler_remove(void)
**
** Algorithm 
** 
** The major steps taken for synchronous API are:
** READ LINE
**   TTYInfo
**   rl_ttyset(0)
**   hist_add(nil)
**   TTYputs(Prompt)
**   read line
**   TTYputs(NEWLINE)
**   rl_ttyset(1)
**   DISPOSE(H.Lines[-H.Size]);
**   return line
**   add to history
** All operations are done during the readline call.
**
** The Major steps for asynchronous are broken into three stages.
**
** START <- rl_callback_handler_install
**   TTYInfo
**   rl_ttyset(0)
**   hist_add(nil)
**                                    
** READ CHAR <- rl_callback_read_char
**   TTYInfo
**   hist_add(nil)
**   read character
**   DISPOSE(H.Lines[-H.Size]);
**   if end-of-line then call eol_callback with line
**   hist_add(nil)
**   TTYputs(Prompt)
**
** CLEAN <- rl_callback_handler_remove
**  rl_ttyset(1)
**
** START is done initially once. CLEAN is done at termination. READ
*/

#ifdef INCLUDE_EDITLINE

#include "sal/appl/editline/editline.h"
#include "editline.h"
#if defined(INCLUDE_AUTOCOMPLETE)
#include <sal/appl/editline/autocomplete.h>
#endif

#include <appl/diag/shell.h>

#if     defined(USE_POSIX_SIGNALS)
#include <signal.h>
#include <errno.h>
#else
/* Dummy defines to avoid too many ifdefs */
#define	SIGINT	        2
#define	SIGQUIT	        3
#define	SIGTSTP	        20
#endif

/*
**  Manifest constants.
*/
#define SCREEN_WIDTH	80
#define SCREEN_ROWS	24
#define NO_ARG		(-1)
#define DEL		127
#define TAB		'\t'
#define CTL(x)		((x) & 0x1F)
#define ISCTL(x)	((x) && (x) < ' ')
#define UNCTL(x)	((x) + 64)
#define META(x)		((x) | 0x80)
#define ISMETA(x)	((x) & 0x80)
#define UNMETA(x)	((x) & 0x7F)
#define MAPSIZE		32
#define METAMAPSIZE	20
#if	!defined(HIST_SIZE)
#define HIST_SIZE	20
#endif	/* !defined(HIST_SIZE) */


typedef CONST CHAR	*STRING;

/*
**  Command status codes.
*/
typedef enum _STATE {
    CSdone, CSeof, CSmove, CSdispatch, CSstay, CSsignal
} STATE;

/*
**  The type of case-changing to perform.
*/
typedef enum _CASE {
    TOupper, TOlower
} CASE;

/*
**  Key to command mapping.
*/
typedef struct _KEYMAP {
    CHAR	Key;
    char	Active;
    STATE	(*Function)(void);
} KEYMAP;

/*
**  Command history structure.
*/
typedef struct _HISTORY {
    int		Size;
    int		Pos;
    CHAR	*Lines[HIST_SIZE];
} HISTORY;

/*
**  Globals.
*/
int		rl_eof;
int		rl_erase;
int		rl_intr;
int		rl_kill;
int		rl_quit;
#if	defined(DO_SIGTSTP)
int		rl_susp;
#endif	/* defined(DO_SIGTSTP) */
char		*(*rl_complete)(char    *pathname, int        *unique) = rl_complete_file;
int		(*rl_list_possib)(char    *pathname, char    ***avp) = rl_list_possib_file;

STATIC CHAR		NIL[] = "";
STATIC STRING		Input = NIL;
STATIC CHAR		*Line;
STATIC CONST char	*Prompt;
STATIC CHAR		*Yanked;
STATIC char		*Screen;
STATIC char		NEWLINE[]= CRLF;
STATIC HISTORY		H;
STATIC int		Repeat;
STATIC int		End;
STATIC int		Mark;
STATIC int		CursorPos;	/* Relative to beginning of prompt */
STATIC int		OldPoint;
STATIC int		Point;
STATIC int		PushBack;
STATIC int		Pushed;
STATIC int		Signal;
FORWARD KEYMAP		Map[MAPSIZE];
FORWARD KEYMAP		MetaMap[METAMAPSIZE];
STATIC SIZE_T		Length;
STATIC SIZE_T		ScreenCount;
STATIC SIZE_T		ScreenSize;
STATIC char		*backspace;
STATIC int		TTYwidth;
STATIC int		TTYrows;

/* Display print 8-bit chars as `M-x' or as the actual 8-bit char? */
int		rl_meta_chars = 1;

STATIC rl_vcpfunc_t gCALLBACK_EOL_HANDLER     = NULL;
STATIC void        *gCALLBACK_EOL_HANDLER_CTX = NULL;
STATIC rf_vcpfunc_t gCALLBACK_EOF_HANDLER     = NULL;
STATIC void        *gCALLBACK_EOF_HANDLER_CTX = NULL;

/*
**  Declarations.
*/
STATIC CHAR	*editinput(void);
#if	defined(USE_TERMCAP)
extern char	*getenv();
extern char	*tgetstr();
extern int	tgetent();
extern int	tgetnum();
#endif	/* defined(USE_TERMCAP) */

/*
**  TTY input/output functions.
*/

STATIC void
TTYflush(void)
{
  if (ScreenCount) {
	(void)sal_console_write(Screen, ScreenCount);
	ScreenCount = 0;
  }
}

STATIC void
TTYput(CONST CHAR c)
{
    Screen[ScreenCount] = c;
    switch (c) {
    case '\b':
	CursorPos--;
	break;
    case '\r':
	/* Actually, never use CR any more; use TTYbol() instead */
	CursorPos = 0;
	break;
    default:
	if (c >= ' ')
	    CursorPos++;
    }
    if (++ScreenCount >= ScreenSize - 1) {
	char *p;
	ScreenSize += SCREEN_INC;
	p = NEW(char, ScreenSize);
	memcpy(p, Screen, (ScreenSize - SCREEN_INC));
	DISPOSE(Screen);
	Screen = p;
    }
}

STATIC void
TTYputs(STRING p)
{
    while (*p)
	TTYput(*p++);
}

STATIC void
TTYshow(CHAR c)
{
  if (c == DEL) {
	TTYput('^');
	TTYput('?');
  } else if (c == TAB) {
	/* XXX */
  } else if (ISCTL(c)) {
	TTYput('^');
	TTYput(UNCTL(c));
  } else if (rl_meta_chars && ISMETA(c)) {
    TTYput('M');
	TTYput('-');
	TTYput(UNMETA(c));
  } else {
	TTYput(c);
  }
}

STATIC void
TTYstring(CHAR *p)
{
    while (*p)
	TTYshow(*p++);
}

STATIC unsigned int
TTYget(void)
{
    CHAR	c;
    int		n;

    TTYflush();
    if (Pushed) {
      Pushed = 0;
      return PushBack;
    }

    if (*Input) {
      return *Input++;
    }

    n = sal_console_read(&c, 1) == 1 ? c : EOF;

    return n;
}

#define TTYback()	(backspace ? TTYputs((STRING)backspace) : TTYput('\b'))

STATIC void
TTYbackn(int n)
{
    while (--n >= 0)
	TTYback();
}

STATIC void
TTYbol(void)
{
    while (CursorPos > 0)
	TTYback();
}

STATIC void
TTYinfo(void)
{
    static int		init;
    sal_console_info_t  info;
#if	defined(USE_TERMCAP)
    char		*term;
    char		buff[2048];
    char		*bp;
    char		*p;
#endif	/* defined(USE_TERMCAP) */

    if (init) {
	/* Perhaps we got resized. */
	if (sal_console_info_get(&info) >= 0
	 && info.cols > 0 && info.rows > 0) {
	    TTYwidth = info.cols;
	    TTYrows = info.rows;
	}
	return;
    }
    init++;

    TTYwidth = TTYrows = 0;
#if	defined(USE_TERMCAP)
    bp = &buff[0];
    if ((term = getenv("TERM")) == NULL)
	term = "dumb";
    if (tgetent(buff, term) < 0) {
	TTYwidth = SCREEN_WIDTH;
	TTYrows = SCREEN_ROWS;
	return;
    }
    p = tgetstr("le", &bp);
    backspace = p ? sal_strdup(p) : NULL;
    TTYwidth = tgetnum("co");
    TTYrows = tgetnum("li");
#endif	/* defined(USE_TERMCAP) */

    if (sal_console_info_get(&info) >= 0) {
        TTYwidth = info.cols;
        TTYrows = info.rows;
    }

    if (TTYwidth <= 0 || TTYrows <= 0) {
	TTYwidth = SCREEN_WIDTH;
	TTYrows = SCREEN_ROWS;
    }
}


/*
**  Print an array of words in columns.
*/
STATIC void
columns(int ac, CHAR **av)
{
    CHAR	*p;
    int		i;
    int		j;
    int		k;
    int		len;
    int		skip;
    int		longest;
    int		cols;

    /* Find longest name, determine column count from that. */
    for (longest = 0, i = 0; i < ac; i++)
	if ((j = strlen((char *)av[i])) > longest)
	    longest = j;
    cols = TTYwidth / (longest + 3);

    TTYputs((STRING)NEWLINE);
    for (skip = ac / cols + 1, i = 0; i < skip; i++) {
	for (j = i; j < ac; j += skip) {
	    for (p = av[j], len = strlen((char *)p), k = len; --k >= 0; p++)
		TTYput(*p);
	    if (j + skip < ac)
		while (++len < longest + 3)
		    TTYput(' ');
	}
	TTYputs((STRING)NEWLINE);
    }
}

STATIC void
reposition(void)
{
    int		i;
    CHAR	*p;

    TTYbol();
    TTYputs((STRING)Prompt);
    for (i = Point, p = Line; --i >= 0; p++) {
      TTYshow(*p);
    }
}

STATIC void
left(STATE Change)
{
    CHAR	c;

    TTYback();
    if (Point) {
	c = Line[Point - 1];
	if (c == TAB) {
	    /* XXX */
	}
	else if (ISCTL(c))
	    TTYback();
	else if (rl_meta_chars && ISMETA(c)) {
	    TTYback();
	    TTYback();
	}
    }
    if (Change == CSmove)
	Point--;
}

STATIC void
right(STATE Change)
{
    TTYshow(Line[Point]);
    if (Change == CSmove)
	Point++;
}

STATIC STATE
ring_bell(void)
{
    TTYput('\07');
    TTYflush();
    return CSstay;
}

STATIC STATE
do_macro(unsigned int c)
{
#if	defined(USE_TERMCAP)
    CHAR		name[4];

    name[0] = '_';
    name[1] = c;
    name[2] = '_';
    name[3] = '\0';

    if ((Input = (CHAR *)getenv((char *)name)) == NULL) {
	Input = NIL;
	return ring_bell();
    }
#endif
    return CSstay;
}

STATIC STATE
do_forward(STATE move)
{
    int		i;
    CHAR	*p;

    i = 0;
    do {
	p = &Line[Point];
	for ( ; Point < End && (*p == ' ' || !isalnum(*p)); Point++, p++)
	    if (move == CSmove)
		right(CSstay);

	for (; Point < End && isalnum(*p); Point++, p++)
	    if (move == CSmove)
		right(CSstay);

	if (Point == End)
	    break;
    } while (++i < Repeat);

    return CSstay;
}

STATIC STATE
do_case(CASE type)
{
    int		i;
    int		end;
    int		count;
    CHAR	*p;

    (void)do_forward(CSstay);
    if (OldPoint != Point) {
	if ((count = Point - OldPoint) < 0)
	    count = -count;
	Point = OldPoint;
	if ((end = Point + count) > End)
	    end = End;
	for (i = Point, p = &Line[i]; i < end; i++, p++) {
	    if (type == TOupper) {
		if (islower(*p))
		    *p = toupper(*p);
	    }
	    else if (isupper(*p))
		*p = tolower(*p);
	    right(CSmove);
	}
    }
    return CSstay;
}

STATIC STATE
case_down_word(void)
{
    return do_case(TOlower);
}

STATIC STATE
case_up_word(void)
{
    return do_case(TOupper);
}

STATIC void
ceol(void)
{
    int		extras;
    int		i;
    CHAR	*p;

    for (extras = 0, i = Point, p = &Line[i]; i <= End; i++, p++) {
	TTYput(' ');
	if (*p == TAB) {
	    /* XXX */
	}
	else if (ISCTL(*p)) {
	    TTYput(' ');
	    extras++;
	}
	else if (rl_meta_chars && ISMETA(*p)) {
	    TTYput(' ');
	    TTYput(' ');
	    extras += 2;
	}
    }

    for (i += extras; i > Point; i--)
	TTYback();
}

#if	defined(SEARCH_HISTORY)
STATIC void
clear_line()
{
    Point = -strlen(Prompt);
    TTYbol();
    ceol();
    Point = 0;
    End = 0;
    Line[0] = '\0';
}
#endif

STATIC STATE
insert_string(CHAR *p)
{
    SIZE_T	len;
    int		i;
    CHAR	*new;
    CHAR	*q;

    len = strlen((char *)p);
    if (End + len >= Length) {
	if ((new = NEW(CHAR, Length + len + MEM_INC)) == NULL)
	    return CSstay;
	if (Length) {
	    COPYFROMTO(new, Line, Length);
	    DISPOSE(Line);
	}
	Line = new;
	Length += len + MEM_INC;
    }

    for (q = &Line[Point], i = End - Point; --i >= 0; )
	q[len + i] = q[i];
    COPYFROMTO(&Line[Point], p, len);
    End += len;
    Line[End] = '\0';
    TTYstring(&Line[Point]);
    Point += len;

    return Point == End ? CSstay : CSmove;
}

STATIC STATE
redisplay(void)
{
    TTYputs((STRING)NEWLINE);
    TTYputs((STRING)Prompt);
    TTYstring(Line);
    return CSmove;
}

STATIC STATE
redisplay_no_nl(void)
{
    TTYbol();
    TTYputs((STRING)Prompt);
    TTYstring(Line);
    return CSmove;
}

STATIC STATE
toggle_meta_mode(void)
{
    rl_meta_chars = !rl_meta_chars;
    return redisplay();
}


STATIC CHAR *
next_hist(void)
{
    return H.Pos >= H.Size - 1 ? NULL : H.Lines[++H.Pos];
}

STATIC CHAR *
prev_hist(void)
{
    return H.Pos == 0 ? NULL : H.Lines[--H.Pos];
}

STATIC STATE
do_insert_hist(CHAR *p)
{
    if (p == NULL)
	return ring_bell();
    Point = 0;
    reposition();
    ceol();
    End = 0;
    return insert_string(p);
}

STATIC STATE
do_hist(CHAR *(*move)(void) )
{
    CHAR	*p;
    int		i;

    i = 0;
    do {
	if ((p = (*move)()) == NULL)
	    return ring_bell();
    } while (++i < Repeat);
    return do_insert_hist(p);
}

STATIC STATE
h_next(void)
{
    return do_hist(next_hist);
}

STATIC STATE
h_prev(void)
{
    return do_hist(prev_hist);
}

STATIC STATE
h_first(void)
{
    return do_insert_hist(H.Lines[H.Pos = 0]);
}

STATIC STATE
h_last(void)
{
    return do_insert_hist(H.Lines[H.Pos = H.Size - 1]);
}

#if	defined(SEARCH_HISTORY)
/*
**  Return zero if pat appears as a substring in text.
*/
STATIC int
_substrcmp(char *text, char *pat, int len)
{
    CHAR	c;

    if ((c = *pat) == '\0')
	return *text == '\0';
    for ( ; *text; text++)
	if (*text == c && strncmp(text, pat, len) == 0)
	    return 0;
    return 1;
}
#endif

STATIC STATE
do_prefix_search(CHAR *(*move)(void) )
{
    int		old_point;

    old_point = Point;

    for ( ; (*move)() != NULL; )
	if (strncmp((char *)H.Lines[H.Pos], (char *)Line, Point) == 0) {
	    (void) do_insert_hist(H.Lines[H.Pos]);
	    Point = old_point;
	    return CSmove;
	}

    return ring_bell();
}

STATIC STATE
h_search_prev(void)
{
    return do_prefix_search(prev_hist);
}

STATIC STATE
h_search_next(void)
{
    return do_prefix_search(next_hist);
}

#if	defined(SEARCH_HISTORY)
STATIC CHAR *
search_hist(CHAR *search, CHAR *(*move)(void) )
{
    static CHAR	*old_search;
    int		len;
    int		pos;
    int		(*match)();
    char	*pat;

    /* Save or get remembered search pattern. */
    if (search && *search) {
	if (old_search)
	    DISPOSE(old_search);
	old_search = (CHAR *)sal_strdup((char *)search);
    }
    else {
	if (old_search == NULL || *old_search == '\0')
	    return NULL;
	search = old_search;
    }

    /* Set up pattern-finder. */
    if (*search == '^') {
	match = strncmp;
	pat = (char *)(search + 1);
    }
    else {
	match = _substrcmp;
	pat = (char *)search;
    }
    len = strlen(pat);

    for (pos = H.Pos; (*move)() != NULL; )
	if ((*match)((char *)H.Lines[H.Pos], pat, len) == 0)
	    return H.Lines[H.Pos];
    H.Pos = pos;
    return NULL;
}
#endif

#if	defined(SEARCH_HISTORY)
STATIC STATE
h_search(void)
{
    static int	Searching;
    CONST char	*old_prompt;
    CHAR	*(*move)();
    CHAR	*p;

    if (Searching)
	return ring_bell();
    Searching = 1;

    clear_line();
    old_prompt = Prompt;
    Prompt = "Search: ";
    TTYputs((STRING)Prompt);
    move = Repeat == NO_ARG ? prev_hist : next_hist;
    CursorPos = strlen(Prompt);
    p = editinput();
    Searching = 0;
    if (p == NULL && Signal > 0) {
	Signal = 0;
	clear_line();
	Prompt = old_prompt;
	return redisplay_no_nl();
    }
    p = search_hist(p, move);
    clear_line();
    Prompt = old_prompt;
    if (p == NULL) {
	(void)ring_bell();
	return redisplay_no_nl();
    }
    return do_insert_hist(p);
}
#endif

STATIC STATE
fd_char(void)
{
    int		i;

    i = 0;
    do {
	if (Point >= End)
	    break;
	right(CSmove);
    } while (++i < Repeat);
    return CSstay;
}

STATIC void
save_yank(int begin,int i)
{
    if (Yanked) {
	DISPOSE(Yanked);
	Yanked = NULL;
    }

    if (i < 1)
	return;

    if ((Yanked = NEW(CHAR, (SIZE_T)i + 1)) != NULL) {
	COPYFROMTO(Yanked, &Line[begin], i);
	Yanked[i] = '\0';
    }
}

STATIC STATE
delete_string(int count)
{
    int		i;
    CHAR	*p;

    if (count <= 0 || End == Point)
	return ring_bell();

    if (count == 1 && Point == End - 1) {
	/* Optimize common case of delete at end of line. */
	End--;
	p = &Line[Point];
	i = 1;
	TTYput(' ');
	if (*p == TAB) {
	    /* XXX */
	}
	else if (ISCTL(*p)) {
	    i = 2;
	    TTYput(' ');
	}
	else if (rl_meta_chars && ISMETA(*p)) {
	    i = 3;
	    TTYput(' ');
	    TTYput(' ');
	}
	TTYbackn(i);
	*p = '\0';
	return CSmove;
    }
    if (Point + count > End && (count = End - Point) <= 0)
	return CSstay;

    if (count > 1)
	save_yank(Point, count);

    ceol();
    for (p = &Line[Point], i = End - (Point + count) + 1; --i >= 0; p++)
	p[0] = p[count];
    End -= count;
    TTYstring(&Line[Point]);
    return CSmove;
}

STATIC STATE
bk_char(void)
{
    int		i;

    i = 0;
    do {
	if (Point == 0)
	    break;
	left(CSmove);
    } while (++i < Repeat);

    return CSstay;
}

STATIC STATE
bk_del_char(void)
{
    int		i;

    i = 0;
    do {
	if (Point == 0)
	    break;
	left(CSmove);
    } while (++i < Repeat);

    return delete_string(i);
}

STATIC STATE
kill_line(void)
{
    int		i;

    if (Repeat != NO_ARG) {
	if (Repeat < Point) {
	    i = Point;
	    Point = Repeat;
	    reposition();
	    (void)delete_string(i - Point);
	}
	else if (Repeat > Point) {
	    right(CSmove);
	    (void)delete_string(Repeat - Point - 1);
	}
	return CSmove;
    }

    save_yank(Point, End - Point);
    ceol();
    Line[Point] = '\0';
    End = Point;
    return CSstay;
}

STATIC STATE
insert_char(int c)
{
    STATE	s;
    CHAR	buff[2];
    CHAR	*p;
    CHAR	*q;
    int		i;

    if (Repeat == NO_ARG || Repeat < 2) {
	buff[0] = c;
	buff[1] = '\0';
	return insert_string(buff);
    }

    if ((p = NEW(CHAR, Repeat + 1)) == NULL)
	return CSstay;
    for (i = Repeat, q = p; --i >= 0; )
	*q++ = c;
    *q = '\0';
    Repeat = 0;
    s = insert_string(p);
    DISPOSE(p);
    return s;
}

static STATE
meta(void)
{
    int	c;
    KEYMAP		*kp;

    if ((c = TTYget()) == EOF)
	return CSeof;
#if	defined(ANSI_ARROWS)
    /* Also include VT-100 arrows. */
    if (c == '[' || c == 'O')
	switch ((int)(c = TTYget())) {
	default:	return ring_bell();
	case EOF:	return CSeof;
	case 'A':	return h_prev();
	case 'B':	return h_next();
	case 'C':	return fd_char();
	case 'D':	return bk_char();
	}
#endif	/* defined(ANSI_ARROWS) */

    if (isdigit(c)) {
	for (Repeat = c - '0'; (c = TTYget()) != EOF && isdigit(c); )
	    Repeat = Repeat * 10 + c - '0';
	Pushed = 1;
	PushBack = c;
	return CSstay;
    }

    if (isupper(c))
	return do_macro((unsigned int)c);
    for (OldPoint = Point, kp = MetaMap; kp < &MetaMap[METAMAPSIZE]; kp++)
	if (kp->Key == c && kp->Active)
	    return (*kp->Function)();

    return ring_bell();
}

STATIC STATE
emacs(unsigned int c)
{
    STATE		s;
    KEYMAP		*kp;

#if	0
    /* This test makes it impossible to enter eight-bit characters when
     * meta-char mode is enabled. */
    if (rl_meta_chars && ISMETA(c)) {
	Pushed = 1;
	PushBack = UNMETA(c);
	return meta();
    }
#endif	/* 0 */
    for (kp = Map; kp < &Map[MAPSIZE]; kp++)
	if (kp->Key == c && kp->Active)
	    break;
    s = kp < &Map[MAPSIZE] ? (*kp->Function)() : insert_char((int)c);
    if (!Pushed)
	
	Repeat = NO_ARG;
    return s;
}

STATIC STATE
TTYspecial(int c)
{
  if (rl_meta_chars && ISMETA(c)) {
      return CSdispatch;
  }

  if (c == rl_erase || c == DEL) {
	return bk_del_char();
  }
  if (c == rl_kill) {
	if (Point != 0) {
      Point = 0;
	  reposition();
	}
	Repeat = NO_ARG;
	return kill_line();
  }
  if (c == rl_eof && Point == 0 && End == 0) {
	return CSeof;
  }

  if (c == rl_intr) {
	Signal = SIGINT;
	return CSsignal;
  }
  if (c == rl_quit) {
	Signal = SIGQUIT;
	return CSsignal;
  }
#if	defined(DO_SIGTSTP)
  if (c == rl_susp) {
	Signal = SIGTSTP;
	return CSsignal;
  }
#endif	/* defined(DO_SIGTSTP) */

  return CSdispatch;
}

STATIC CHAR *
editinput(void)
{
    int	c;
#if defined(INCLUDE_AUTOCOMPLETE)
    extern int diag_user_var_unit;
#endif

    Repeat = NO_ARG;
    OldPoint = Point = Mark = End = 0;
    Line[0] = '\0';
    /* This is a formality to keep consistent with the asynchronous char
       process. 
       Need to track if the previous character was EOL so that asynchronous
       support knows when to apply the three lines above.
    */

    Signal = -1;
    while ((c = TTYget()) != EOF) {
#if defined(INCLUDE_AUTOCOMPLETE)
      if (c == TAB) {
          Input = (STRING)autocomplete_print(diag_user_var_unit, (char*)Line, (char*)Prompt);
          TTYflush();
          continue;
      }
#endif
      switch (TTYspecial(c)) {
	  case CSdone:
        return Line;
	  case CSeof:
	    return NULL;
	  case CSsignal:
	    return (CHAR *)"";
	  case CSmove:
	    reposition();
	    break;
	  case CSdispatch:
	    switch (emacs(c)) {
	    case CSdone:
          return Line;
	    case CSeof:
          return NULL;
	    case CSsignal:
          return (CHAR *)"";
	    case CSmove:
          reposition();
          break;
	    case CSdispatch:
        case CSstay:
          break;
	    }
	    break;
      case CSstay:
	    break;
      }
    }
    return NULL;
}

STATIC void
hist_add(CHAR *p)
{
    int		i;

    if ((p = (CHAR *)sal_strdup((char *)p)) == NULL) {
      return;
    }
    if (H.Size < HIST_SIZE)
      H.Lines[H.Size++] = p;
    else {
      DISPOSE(H.Lines[0]);
      for (i = 0; i < HIST_SIZE - 1; i++)
	    H.Lines[i] = H.Lines[i + 1];
      H.Lines[i] = p;
    }
    H.Pos = H.Size - 1;
}

STATIC char *
read_redirected(void)
{
    int		size;
    char	*p;
    char	*line;
    char	*end;

    size = MEM_INC;
    line = NEW(char, size);
    end = line + size;

    for (p = line; ; p++) {
	if (p == end) {
	    p = NEW(char, size + MEM_INC);
	    memcpy(p, line, size);
	    DISPOSE(line);
	    line = p;
	    p = line + size;
	    size += MEM_INC;
	    end = line + size;
	}
	if (sal_console_read(p, 1) <= 0) {
	    /* Ignore "incomplete" lines at EOF, just like we do for a tty. */
	    DISPOSE(line);
	    return NULL;
	}
	
	if (*p == '\n')
	    break;
    }
    *p = '\0';
    return line;
}

/*
**  For compatibility with FSF readline.
*/
/* ARGSUSED0 */
void
rl_reset_terminal(char *p)
{
}

int
el_insert(int count,int c)
{
    if (count > 0) {
	Repeat = count;
	(void)insert_char(c);
	(void)redisplay_no_nl();
    }
    return 0;
}

int (*rl_event_hook)(void);

int
rl_key_action(int c, char flag)
{
    KEYMAP	*kp;
    int		size;

    if (ISMETA(c)) {
	kp = MetaMap;
	size = METAMAPSIZE;
    }
    else {
	kp = Map;
	size = MAPSIZE;
    }
    for ( ; --size >= 0; kp++)
	if (kp->Key == c) {
	    kp->Active = c ? 1 : 0;
	    return 1;
	}
    return -1;
}

int
readchar(CONST char *prompt)
{
    char c;
#ifdef UNIX
    int gdb = 0;

    if (getenv("GDB") != NULL && getenv("DCON") == NULL) {
	gdb = 1;
    }
#endif

    if (Screen == NULL) {
	ScreenSize = SCREEN_INC;
	Screen = NEW(char, ScreenSize);
    }

    if (sal_console_info_get(NULL) < 0) {
	TTYflush();
	return EOF;
    }

#ifdef UNIX
    if (gdb) {
	char *t;
        char p;
	printf("%s", prompt);
	fflush(stdout);
	t = read_redirected();
        if(t == NULL) {
           return EOF;
        } else {
           p = *(t+0);
           sal_free(t);
           return p;
        }  
    }
#endif

    TTYinfo();
    rl_ttyset(0);
    Prompt = prompt ? prompt : (char *)NIL;
    TTYputs((STRING)Prompt);
    c = TTYget();
    if (c == rl_intr)
	Signal = SIGINT;
    else if (c == rl_quit)
	Signal = SIGQUIT;
#if	defined(DO_SIGTSTP)
    else if (c == rl_susp)
	Signal = SIGTSTP;
#endif	/* defined(DO_SIGTSTP) */
    rl_ttyset(1);

#if     defined(USE_POSIX_SIGNALS)
    if (Signal > 0) {
        int s = Signal;
	Signal = 0;
	(void)kill(getpid(), s);
    }
#endif
    return c;
}    

char *
el_readline(CONST char *prompt)
{
    CHAR	*line;
#ifdef UNIX
    int		gdb = 0;

    if (getenv("GDB") != NULL && getenv("DCON") == NULL) {
	gdb = 1;
    }
#endif

    if (Screen == NULL) {
	ScreenSize = SCREEN_INC;
	Screen = NEW(char, ScreenSize);
    }

    /*
     * If input is not a TTY, just do a read.
     * Also just do a read if TTY data is pending.
     */
    if (sal_console_info_get(NULL) < 0) {
      TTYflush();
      return read_redirected();
    }

#ifdef UNIX
    if (gdb) {
	printf("%s", prompt);
	fflush(stdout);
	return read_redirected();
    }
#endif

    if (Line == NULL) {
      Length = MEM_INC;
      if ((Line = NEW(CHAR, Length)) == NULL) {
	    return NULL;
      }
    }

    TTYinfo();
    rl_ttyset(0);
    hist_add(NIL);
    Prompt = prompt ? prompt : (char *)NIL;
    TTYputs((STRING)Prompt);
    CursorPos = strlen(Prompt);
    if ((line = editinput()) != NULL) {
      line = (CHAR *)sal_strdup((char *)line);
      TTYputs((STRING)NEWLINE);
      TTYflush();
    }
    rl_ttyset(1);

    DISPOSE(H.Lines[--H.Size]);
#if     defined(USE_POSIX_SIGNALS)
    if (Signal > 0) {
      int s = Signal;
      errno = EINTR;
      Signal = 0;
      (void)kill(getpid(), s);
      if (line) {
	    DISPOSE(line);
	    line = NULL;
      }
    } else {
        errno = 0;
    }
#endif
    return (char *)line;
}

void
el_add_history(char *p)
{
  if (p == NULL || *p == '\0') {
      return;
  }

#if	defined(UNIQUE_HISTORY)
  if (H.Size && strcmp(p, (char *)H.Lines[H.Size - 1]) == 0) {
	return;
  }
#endif	/* defined(UNIQUE_HISTORY) */
  hist_add((CHAR *)p);
}

void
list_history(int count)
{
    int h;
    if ((h = H.Size - count) < 0)
	h = 0;
    while (h < H.Size) {
	TTYputs((STRING)"   ");
	TTYputs((STRING)H.Lines[h++]);
	TTYputs((STRING)NEWLINE);
	TTYflush();
    }
}



STATIC STATE
beg_line(void)
{
    if (Point) {
	Point = 0;
	return CSmove;
    }
    return CSstay;
}

STATIC STATE
del_char(void)
{
    return delete_string(Repeat == NO_ARG ? 1 : Repeat);
}

STATIC STATE
end_line(void)
{
    if (Point != End) {
	Point = End;
	return CSmove;
    }
    return CSstay;
}

/*
**  Return allocated copy of word under cursor, moving cursor after the
**  word.
*/
STATIC CHAR *
find_word(void)
{
    static char	SEPS[] = "\"#;&|^$=`'{}()<>\n\t ";
    CHAR	*p;
    CHAR	*new;
    SIZE_T	len;

    /* Move forward to end of word. */
    p = &Line[Point];
    for ( ; Point < End && strchr(SEPS, (char)*p) == NULL; Point++, p++)
	right(CSstay);

    /* Back up to beginning of word. */
    for (p = &Line[Point]; p > Line && strchr(SEPS, (char)p[-1]) == NULL; p--)
	continue;
    len = Point - (p - Line) + 1;
    if ((new = NEW(CHAR, len)) == NULL)
	return NULL;
    COPYFROMTO(new, p, len);
    new[len - 1] = '\0';
    return new;
}

STATIC STATE
c_complete(void)
{
    CHAR	*p;
    CHAR	*word;
    int		unique;

    word = find_word();
    p = (CHAR *)(*rl_complete)((char *)word, &unique);
    if (word)
	DISPOSE(word);
    if (p) {
	if (*p)
	    (void)insert_string(p);
	if (!unique)
	    (void)ring_bell();
	DISPOSE(p);
	return redisplay_no_nl();
    }
    return ring_bell();
}

void rl_input_state(rl_input_state_t *state)
{
    state->line = Line;
    state->point = Point;
}

STATIC STATE
c_possible(void)
{
    CHAR	**av, ***pav = &av;
    CHAR	*word;
    int		ac;

    word = find_word();
    ac = (*rl_list_possib)((char *)word, (char ***)pav);
    if (word)
	DISPOSE(word);
    if (ac) {
	columns(ac, av);
	while (--ac >= 0)
	    DISPOSE(av[ac]);
	DISPOSE(av);
	return redisplay_no_nl();
    }
    return ring_bell();
}

STATIC STATE
accept_line(void)
{
    Line[End] = '\0';
    return CSdone;
}

STATIC STATE
transpose(void)
{
    CHAR	c;

    if (Point) {
	if (Point == End)
	    left(CSmove);
	c = Line[Point - 1];
	left(CSstay);
	Line[Point - 1] = Line[Point];
	TTYshow(Line[Point - 1]);
	Line[Point++] = c;
	TTYshow(c);
    }
    return CSstay;
}

STATIC STATE
quote(void)
{
    int	c;

    return (c = TTYget()) == EOF ? CSeof : insert_char((int)c);
}

STATIC STATE
wipe(void)
{
    int		i;

    if (Mark > End)
	return ring_bell();

    if (Point > Mark) {
	i = Point;
	Point = Mark;
	Mark = i;
	reposition();
    }

    return delete_string(Mark - Point);
}

STATIC STATE
mk_set(void)
{
    Mark = Point;
    return CSstay;
}

STATIC STATE
exchange(void)
{
    int	c;

    if ((c = TTYget()) != CTL('X'))
	return c == EOF ? CSeof : ring_bell();

    if ((c = Mark) <= End) {
	Mark = Point;
	Point = c;
	return CSmove;
    }
    return CSstay;
}

STATIC STATE
yank(void)
{
    if (Yanked && *Yanked)
	return insert_string(Yanked);
    return CSstay;
}

STATIC STATE
copy_region(void)
{
    if (Mark > End)
	return ring_bell();

    if (Point > Mark)
	save_yank(Mark, Point - Mark);
    else
	save_yank(Point, Mark - Point);

    return CSstay;
}

STATIC STATE
move_to_char(void)
{
    int	c;
    int			i;
    CHAR		*p;

    if ((c = TTYget()) == EOF)
	return CSeof;
    for (i = Point + 1, p = &Line[i]; i < End; i++, p++)
	if (*p == c) {
	    Point = i;
	    return CSmove;
	}
    return CSstay;
}

STATIC STATE
fd_word(void)
{
    return do_forward(CSmove);
}

STATIC STATE
fd_kill_word(void)
{
    int		i;

    (void)do_forward(CSstay);
    if (OldPoint != Point) {
	i = Point - OldPoint;
	Point = OldPoint;
	return delete_string(i);
    }
    return CSstay;
}

STATIC STATE
bk_word(void)
{
    int		i;
    CHAR	*p;

    i = 0;
    do {
	for (p = &Line[Point]; p > Line && !isalnum(p[-1]); p--)
	    left(CSmove);

	for (; p > Line && p[-1] != ' ' && isalnum(p[-1]); p--)
	    left(CSmove);

	if (Point == 0)
	    break;
    } while (++i < Repeat);

    return CSstay;
}

STATIC STATE
bk_kill_word(void)
{
    (void)bk_word();
    if (OldPoint != Point)
	return delete_string(OldPoint - Point);
    return CSstay;
}

STATIC int
argify(CHAR *line, CHAR ***avp)
{
    CHAR	*c;
    CHAR	**p;
    CHAR	**new;
    int		ac;
    int		i;

    i = MEM_INC;
    if ((*avp = p = NEW(CHAR*, i))== NULL)
	 return 0;

    for (c = line; isspace(*c); c++)
	continue;
    if (*c == '\n' || *c == '\0')
	return 0;

    for (ac = 0, p[ac++] = c; *c && *c != '\n'; ) {
	if (isspace(*c)) {
	    *c++ = '\0';
	    if (*c && *c != '\n') {
		if (ac + 1 == i) {
		    new = NEW(CHAR*, i + MEM_INC);
		    if (new == NULL) {
			p[ac] = NULL;
			return ac;
		    }
		    COPYFROMTO(new, p, i * sizeof(CHAR*));
		    i += MEM_INC;
		    DISPOSE(p);
		    *avp = p = new;
		}
		p[ac++] = c;
	    }
	}
	else
	    c++;
    }
    *c = '\0';
    p[ac] = NULL;
    return ac;
}

STATIC STATE
last_argument(void)
{
    CHAR	**av;
    CHAR	*p;
    STATE	s;
    int		ac;

    if (H.Size == 1 || (p = H.Lines[H.Size - 2]) == NULL)
	return ring_bell();

    if ((p = (CHAR *)sal_strdup((char *)p)) == NULL)
	return CSstay;
    ac = argify(p, &av);

    if (Repeat != NO_ARG)
	s = Repeat < ac ? insert_string(av[Repeat]) : ring_bell();
    else
	s = ac ? insert_string(av[ac - 1]) : CSstay;

    if (av)
	DISPOSE(av);
    DISPOSE(p);
    return s;
}

STATIC KEYMAP	Map[MAPSIZE] = {
    {	CTL('@'),	1,	mk_set		},
    {	CTL('A'),	1,	beg_line	},
    {	CTL('B'),	1,	bk_char		},
    {	CTL('D'),	1,	del_char	},
    {	CTL('E'),	1,	end_line	},
    {	CTL('F'),	1,	fd_char		},
    {	CTL('G'),	1,	ring_bell	},
    {	CTL('H'),	1,	bk_del_char	},
    {	CTL('I'),	1,	c_complete	},
    {	CTL('J'),	1,	accept_line	},
    {	CTL('K'),	1,	kill_line	},
    {	CTL('L'),	1,	redisplay	},
    {	CTL('M'),	1,	accept_line	},
    {	CTL('N'),	1,	h_next		},
    {	CTL('O'),	1,	ring_bell	},
    {	CTL('P'),	1,	h_prev		},
    {	CTL('Q'),	1,	ring_bell	},
#if	defined(SEARCH_HISTORY)
    {	CTL('R'),	1,	h_search	},
#endif
    {	CTL('S'),	1,	ring_bell	},
    {	CTL('T'),	1,	transpose	},
    {	CTL('U'),	1,	ring_bell	},
    {	CTL('V'),	1,	quote		},
    {	CTL('W'),	1,	wipe		},
    {	CTL('X'),	1,	exchange	},
    {	CTL('Y'),	1,	yank		},
    {	CTL('Z'),	1,	ring_bell	},
    {	CTL('['),	1,	meta		},
    {	CTL(']'),	1,	move_to_char	},
    {	CTL('^'),	1,	ring_bell	},
    {	CTL('_'),	1,	ring_bell	},
};

STATIC KEYMAP	MetaMap[METAMAPSIZE]= {
    {	CTL('H'),	1,	bk_kill_word	},
    {	CTL('['),	1,	c_possible	},
    {	DEL,		1,	bk_kill_word	},
    {	' ',		1,	mk_set		},
    {	'.',		1,	last_argument	},
    {	'<',		1,	h_first		},
    {	'>',		1,	h_last		},
    {	'?',		1,	c_possible	},
    {	'b',		1,	bk_word		},
    {	'd',		1,	fd_kill_word	},
    {	'f',		1,	fd_word		},
    {	'l',		1,	case_down_word	},
    {	'm',		1,	toggle_meta_mode},
    {	'n',		1,	h_search_next	},
    {	'p',		1,	h_search_prev	},
    {	'u',		1,	case_up_word	},
    {	'y',		1,	yank		},
    {	'w',		1,	copy_region	},
};

/* Asynchronous event support */
void
_rl_editline_input_start(void)
{

    Repeat = NO_ARG;
    OldPoint = Point = Mark = End = 0;
    Line[0] = '\0';

    Signal = -1;
}

/*
 * Description:
 *  process individual characters and determines if history should be
 *  be presented to the user.
 */
int
_rl_editline_input_process(int c, CHAR **linep)
{
    int eol = FALSE;

    switch (TTYspecial(c)) {
    case CSdone:
        *linep = Line;
        eol = TRUE;
        break;
    case CSeof:
        if (gCALLBACK_EOF_HANDLER) {
          gCALLBACK_EOF_HANDLER(gCALLBACK_EOF_HANDLER_CTX);
        }
        break;
    case CSsignal:
        *linep = (CHAR *)"";
        eol = TRUE;
        break;
    case CSmove:
        reposition();
        break;
    case CSdispatch:
        switch (emacs(c)) {
        case CSdone:
          *linep = Line;
          eol = TRUE;
          break;
        case CSeof:
          if (gCALLBACK_EOF_HANDLER) {
            gCALLBACK_EOF_HANDLER(gCALLBACK_EOF_HANDLER_CTX);
          }
          *linep = NULL;
          eol = TRUE;
          break;
        case CSsignal:
            *linep =  (CHAR *)"";
            eol = TRUE;
            break;
        case CSmove:
            reposition();
            break;
        case CSdispatch:
        case CSstay:
            break;
        }
        break;
    case CSstay:
        break;
    }

    return eol;
}

/*
 Set new prompt.
 */
void rl_prompt_set(CONST char *prompt)
{
  int changed = 0;
  if (!Prompt || !prompt) {
    changed = 1;
  } else if (Prompt && prompt && sal_strcmp(Prompt, prompt)) {
    changed = 1;
  }

  Prompt = prompt ? prompt : (char *)NIL;
  if (changed || (CursorPos == 0)) {
    /* CursorPos == 0 means that nothing has been displayed as a prompt.*/
    if (CursorPos != 0) {
      /* We're changing prompts in the middle of a line. 
         put a new line to continue. */
      TTYputs((STRING)NEWLINE);
    }
    TTYputs((STRING)Prompt);
    TTYflush();
    CursorPos = strlen((char *)Prompt);
  }
}

/*
 * Initialize asynchronous editline support.
 *
 * Must call _rl_done when finished.
 */
int
_rl_initialize(CONST char *prompt)
{
    if (Screen == NULL) {
      ScreenSize = SCREEN_INC;
      if(!(Screen = NEW(char, ScreenSize))) {
        return 0;
      }
    }

    if (Line == NULL) {
      Length = MEM_INC;
      if ((Line = NEW(CHAR, Length)) == NULL) {
	    return 0;
      }
    }

    TTYinfo();
    rl_ttyset(0);
    hist_add(NIL);
    rl_prompt_set(prompt);

    return 1;
}

void
_rl_done(void)
{
    rl_ttyset(1);

#if     defined(USE_POSIX_SIGNALS)
    if (Signal > 0) {
      int s = Signal;
      errno = EINTR;
      Signal = 0;
      (void)kill(getpid(), s);
    } else {
      errno = 0;
    }
#endif
}

/**
 Read a single character and post end-of-line handler.

 Notes:
 see rl_callback_handler_install to register the end-of-line handler.
 */
void rl_callback_read_char(CONST char *prompt)
{
    /* Start with yes previous character processed was eol so that 
       initialization will work. */
    static int PrevEol = TRUE;

    /* read a char from input */
    CHAR *line = NULL;
    int   c    = 0;

    /* 
       When we transition from synchronous to asynchronous H.Size == H.Pos
       that is how we tell we are at a new line. We may need to change 
       the prompt and cleanout output. 

       The PrevEol state is strictly internal because PrevEol is only
       set to TRUE when eol handler is called a new line has been read.
    */
    if ((H.Size == H.Pos) || (PrevEol == TRUE)) {
      _rl_initialize(prompt);
      _rl_editline_input_start();
      PrevEol = FALSE; /* Start of a process a new line. */
    }
    c = TTYget();
    
    /* if the line is complete, execute eol handler */
    if (_rl_editline_input_process(c, &line)) {
      PrevEol = TRUE;
      /* Once a new line has been found pos to eol handler. 
         We can call the handler anywhere before hist_add but after
         NEWLINE is good for printf debugging from eol handler. */
      TTYputs((STRING)NEWLINE);
      TTYflush();
      DISPOSE(H.Lines[--H.Size]);
      gCALLBACK_EOL_HANDLER((char *)line, gCALLBACK_EOL_HANDLER_CTX);
      /* We need to push the prompt after we're done with the end of line.
         Because the end of line handler might process TCL script causing 
         output which needs to occur before the prompt.

         Note that EOL_HANDLER might call into another shell changing the 
         prompt. When returnning the following needs to occur verses just 
         flushing Prompt.
      */
      rl_prompt_set(prompt);
    } 

    /* flush is needed because at this point input was processed and 
       pushed out. This could have been just the input character or
       the history information processed i.e. up down arrows. */
    TTYflush();

#if     defined(USE_POSIX_SIGNALS)
    if (Signal > 0) {
      int s = Signal;
      errno = EINTR;
      Signal = 0;
      (void)kill(getpid(), s);
      if (line) {
	    DISPOSE(line);
	    line = NULL;
      }
    } else {
      errno = 0;
    }
#endif
}

/**
  Install an end-of-line handler.

  The end-of-line handler is only called when a complete line is parsed.
  Users must call rl_callback_handler_remove when done.
 */
void rl_callback_handler_install(CONST char *prompt, 
                                 rl_vcpfunc_t eol_handler, void *eolCtx,
                                 rf_vcpfunc_t eof_handler, void *eofCtx)
{
  if (_rl_initialize(prompt)) {
    /* save handler */
    gCALLBACK_EOL_HANDLER     = eol_handler;
    gCALLBACK_EOL_HANDLER_CTX = eolCtx;

    gCALLBACK_EOF_HANDLER     = eof_handler;
    gCALLBACK_EOF_HANDLER_CTX = eofCtx;

    /* set terminal and init library */
    _rl_editline_input_start();
  }
}

/**
  Called when asynchronous editline is done.
 */
void rl_callback_handler_remove(void **eolCtx, void **eofCtx)
{
  if (eolCtx) {
    (*eolCtx) = gCALLBACK_EOL_HANDLER_CTX;
  }
  if (eofCtx) {
    (*eofCtx) = gCALLBACK_EOF_HANDLER_CTX;
  }
  _rl_done();
  /* remove handler */
  gCALLBACK_EOL_HANDLER     = NULL;
  gCALLBACK_EOL_HANDLER_CTX = NULL;

  gCALLBACK_EOF_HANDLER     = NULL;
  gCALLBACK_EOF_HANDLER_CTX = NULL;
}

#else /* INCLUDE_EDITLINE */
typedef int _editline_editline_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_EDITLINE */
