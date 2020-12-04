/*! \file bcma_editline.c
 *
 * Modified version of the editline library.
 *
 * Please see README file for additional description, credits and
 * copyrights.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/editline/bcma_readline.h>

#if BCMA_NO_EDITLINE == 1

int bcma_editline_not_empty;

#else

#define CRLF		"\r\n"

#ifndef EOF
#define EOF		-1
#endif

#define MEM_INC		64
#define SCREEN_INC	256

#define EL_FREE(_p) \
    bcma_rl_free(_p)

#define EL_MALLOC(_sz)	\
    bcma_rl_alloc(_sz)

#define EL_STRDUP(_s)	\
    bcma_rl_strdup(_s)

/*
 *  Manifest constants.
 */
#define SCREEN_WIDTH    80
#define SCREEN_ROWS     24
#define NO_ARG          (-1)
#define DEL             127
#define TAB             '\t'
#define CTL(x)          ((x) & 0x1F)
#define ISCTL(x)        ((x) && (x) < ' ')
#define UNCTL(x)        ((x) + 64)
#define META(x)         ((x) | 0x80)
#define ISMETA(x)       ((x) & 0x80)
#define UNMETA(x)       ((x) & 0x7F)
#define MAP_SIZE        32
#define META_MAP_SIZE   20

#ifndef EL_HIST_SIZE
#define EL_HIST_SIZE    64
#endif

/*
 *  O/S-level interrupts (if supported).
 */
typedef enum {
    SIG_NONE,
    SIG_INT,
    SIG_QUIT
} el_signal_t;

/*
 *  Command status codes.
 */
typedef enum el_status_e {
    CS_DONE,
    CS_EOF,
    CS_MOVE,
    CS_DISPATCH,
    CS_STAY,
    CS_SIGNAL
} el_status_t;

/*
 *  The type of case-changing to perform.
 */
typedef enum el_case_e {
    TO_UPPER,
    TO_LOWER
} el_case_t;

/*
 *  Key to command mapping.
 */
typedef struct el_keymap_s {
    char key;
    char active;
    el_status_t (*function)(void);
} el_keymap_t;

/*
 *  Command history structure.
 */
typedef struct _el_hist_t {
    int size;
    int pos;
    char *lines[EL_HIST_SIZE];
} el_hist_t;

static char             nil[] = "";
static const char       *input = nil;
static char             *yanked;
static char             *screen;
static char             newline[]= CRLF;
static el_hist_t        hist;
static int              repeat;
static int              cursor_pos;      /* Relative to beginning of prompt */
static int              old_point;
static int              push_back;
static int              pushed;
static el_signal_t      pending_signal;
static size_t           length;
static size_t           screen_count;
static size_t           screen_size;
static int              tty_width;
static int              tty_rows;
static int              prev_compl_len;

/* Forward declarations */
static el_keymap_t      map[MAP_SIZE];
static el_keymap_t      meta_map[META_MAP_SIZE];

/* Display print 8-bit chars as `M-x' or as the actual 8-bit char? */
static int              meta_chars = 1;

static bcma_editline_term_keys_t el_term_keys;
static bcma_editline_io_cb_t el_io_cb;

/*
 *  Globals.
 */

int rl_point;
int rl_end;
int rl_mark;
int rl_catch_signals = 1;
char *rl_line_buffer;
char *rl_prompt;
int rl_attempted_completion_over;
int rl_completion_append_character = ' ';
const char *rl_basic_word_break_characters = "\"#;&|^$=`'{}()<>\n\t @";

rl_completion_func_t *rl_attempted_completion_function;

int history_base = 0;

/*
 *  Forward declarations.
 */
static char *editinput(void);

/*
 * Terminal I/O wrappers.
 */

static int
term_read(void *buf, int max)
{
    if (el_io_cb.read) {
        return el_io_cb.read(buf, max);
    }
    return -1;
}

static int
term_write(const void *buf, int count)
{
    if (el_io_cb.write) {
        return el_io_cb.write(buf, count);
    }
    return -1;
}

static int
tty_set(int reset)
{
    if (el_io_cb.tty_set) {
        return el_io_cb.tty_set(reset);
    }
    return -1;
}

static int
winsize_get(int *cols, int *rows)
{
    if (el_io_cb.winsize_get) {
        return el_io_cb.winsize_get(cols, rows);
    }
    if (rows) {
        rows = 0;
    }
    if (cols) {
        cols = 0;
    }
    return 0;
}

static int
send_sigint(void)
{
    if (el_io_cb.send_sigint) {
        return el_io_cb.send_sigint();
    }
    return -1;
}

static int
send_sigquit(void)
{
    if (el_io_cb.send_sigquit) {
        return el_io_cb.send_sigquit();
    }
    return -1;
}

/*
 *  TTY input/output functions.
 */

static void
tty_flush(void)
{
    if (screen_count) {
        term_write(screen, screen_count);
        screen_count = 0;
    }
}

static void
tty_put(const char c)
{
    screen[screen_count] = c;
    switch (c) {
    case '\b':
        cursor_pos--;
        break;
    case '\r':
        /* Actually, never use CR any more; use tty_bol() instead */
        cursor_pos = 0;
        break;
    default:
        if (c >= ' ')
            cursor_pos++;
    }
    if (++screen_count >= screen_size - 1) {
        char *p;
        screen_size += SCREEN_INC;
        p = EL_MALLOC(screen_size);
        sal_memcpy(p, screen, (screen_size - SCREEN_INC));
        EL_FREE(screen);
        screen = p;
    }
}

static void
tty_puts(const char *p)
{
    while (*p)
        tty_put(*p++);
}

static void
tty_show(char c)
{
    if (c == DEL) {
        tty_put('^');
        tty_put('?');
    }
    else if (c == TAB) {
        /* XXX */
    }
    else if (ISCTL(c)) {
        tty_put('^');
        tty_put(UNCTL(c));
    }
    else if (meta_chars && ISMETA(c)) {
        tty_put('M');
        tty_put('-');
        tty_put(UNMETA(c));
    }
    else
        tty_put(c);
}

static void
tty_string(char *p)
{
    while (*p) {
        tty_show(*p++);
    }
}

static unsigned int
tty_get(void)
{
    char c;
    int n;

    tty_flush();
    if (pushed) {
        pushed = 0;
        return push_back;
    }

    if (*input) {
        return *input++;
    }

    n = term_read(&c, 1) == 1 ? c : EOF;

    return n;
}

static void
tty_back(void)
{
    tty_put('\b');
}

static void
tty_backn(int n)
{
    while (--n >= 0) {
        tty_back();
    }
}

static void
tty_bol(void)
{
    while (cursor_pos > 0) {
        tty_back();
    }
}

static void
tty_info(void)
{
    static int init;
    int cols;
    int rows;

    if (init) {
        /* Perhaps we got resized. */
        if (winsize_get(&cols, &rows) >= 0 && cols > 0 && rows > 0) {
            tty_width = cols;
            tty_rows = rows;
        }
        return;
    }
    init++;

    tty_width = tty_rows = 0;

    if (winsize_get(&cols, &rows) >= 0) {
        tty_width = cols;
        tty_rows = rows;
    }

    if (tty_width <= 0 || tty_rows <= 0) {
        tty_width = SCREEN_WIDTH;
        tty_rows = SCREEN_ROWS;
    }
}


/*
 *  Print an array of words in columns.
 */
static void
columns(int ac, char **av)
{
    char *p;
    int i;
    int j;
    int k;
    int len;
    int skip;
    int longest;
    int cols;

    /* Find longest name, determine column count from that. */
    for (longest = 0, i = 0; i < ac; i++) {
        if ((j = sal_strlen((char *)av[i])) > longest) {
            longest = j;
        }
    }
    cols = tty_width / (longest + 3);

    tty_puts(newline);
    for (skip = ac / cols + 1, i = 0; i < skip; i++) {
        for (j = i; j < ac; j += skip) {
            p = av[j];
            len = sal_strlen((char *)p);
            for (k = len; --k >= 0; p++) {
                tty_put(*p);
            }
            if (j + skip < ac) {
                while (++len < longest + 3) {
                    tty_put(' ');
                }
            }
        }
        tty_puts(newline);
    }
}

static void
reposition(void)
{
    int i;
    char *p;

    tty_bol();
    tty_puts(rl_prompt);
    for (i = rl_point, p = rl_line_buffer; --i >= 0; p++) {
        tty_show(*p);
    }
}

static void
left(el_status_t change)
{
    char c;

    tty_back();
    if (rl_point) {
        c = rl_line_buffer[rl_point - 1];
        if (c == TAB) {
            /* XXX */
        }
        else if (ISCTL(c))
            tty_back();
        else if (meta_chars && ISMETA(c)) {
            tty_back();
            tty_back();
        }
    }
    if (change == CS_MOVE)
        rl_point--;
}

static void
right(el_status_t change)
{
    tty_show(rl_line_buffer[rl_point]);
    if (change == CS_MOVE) {
        rl_point++;
    }
}

static el_status_t
ring_bell(void)
{
    tty_put('\07');
    tty_flush();
    return CS_STAY;
}

static el_status_t
do_macro(unsigned int c)
{
#if     defined(USE_TERMCAP)
    char         name[4];

    name[0] = '_';
    name[1] = c;
    name[2] = '_';
    name[3] = '\0';

    if ((input = (char *)getenv((char *)name)) == NULL) {
        input = nil;
        return ring_bell();
    }
#endif
    return CS_STAY;
}

static el_status_t
do_forward(el_status_t move)
{
    int i;
    char *p;

    i = 0;
    do {
        p = &rl_line_buffer[rl_point];
        for ( ; rl_point < rl_end && (*p == ' ' || !sal_isalnum(*p)); rl_point++, p++) {
            if (move == CS_MOVE) {
                right(CS_STAY);
            }
        }

        for (; rl_point < rl_end && sal_isalnum(*p); rl_point++, p++) {
            if (move == CS_MOVE) {
                right(CS_STAY);
            }
        }

        if (rl_point == rl_end) {
            break;
        }
    } while (++i < repeat);

    return CS_STAY;
}

static el_status_t
do_case(el_case_t type)
{
    int i;
    int e;
    int count;
    char *p;

    (void)do_forward(CS_STAY);
    if (old_point != rl_point) {
        if ((count = rl_point - old_point) < 0) {
            count = -count;
        }
        rl_point = old_point;
        if ((e = rl_point + count) > rl_end) {
            e = rl_end;
        }
        for (i = rl_point, p = &rl_line_buffer[i]; i < e; i++, p++) {
            if (type == TO_UPPER) {
                if (sal_islower(*p))
                    *p = sal_toupper(*p);
            }
            else if (sal_isupper(*p)) {
                *p = sal_tolower(*p);
            }
            right(CS_MOVE);
        }
    }
    return CS_STAY;
}

static el_status_t
case_down_word(void)
{
    return do_case(TO_LOWER);
}

static el_status_t
case_up_word(void)
{
    return do_case(TO_UPPER);
}

static void
ceol(void)
{
    int extras;
    int i;
    char *p;

    for (extras = 0, i = rl_point, p = &rl_line_buffer[i]; i <= rl_end; i++, p++) {
        tty_put(' ');
        if (*p == TAB) {
            /* XXX */
        }
        else if (ISCTL(*p)) {
            tty_put(' ');
            extras++;
        }
        else if (meta_chars && ISMETA(*p)) {
            tty_put(' ');
            tty_put(' ');
            extras += 2;
        }
    }

    for (i += extras; i > rl_point; i--) {
        tty_back();
    }
}

static void
clear_line(void)
{
    rl_point = -sal_strlen(rl_prompt);
    tty_bol();
    ceol();
    rl_point = 0;
    rl_end = 0;
    rl_line_buffer[0] = '\0';
}

static el_status_t
insert_string(char *p)
{
    size_t len;
    int i;
    char *new;
    char *q;

    len = sal_strlen((char *)p);
    if (rl_end + len >= length) {
        if ((new = EL_MALLOC(length + len + MEM_INC)) == NULL) {
            return CS_STAY;
        }
        if (length) {
            sal_memcpy(new, rl_line_buffer, length);
            EL_FREE(rl_line_buffer);
        }
        rl_line_buffer = new;
        length += len + MEM_INC;
    }

    for (q = &rl_line_buffer[rl_point], i = rl_end - rl_point; --i >= 0; ) {
        q[len + i] = q[i];
    }
    sal_memcpy(&rl_line_buffer[rl_point], p, len);
    rl_end += len;
    rl_line_buffer[rl_end] = '\0';
    tty_string(&rl_line_buffer[rl_point]);
    rl_point += len;

    return rl_point == rl_end ? CS_STAY : CS_MOVE;
}

static el_status_t
redisplay(void)
{
    tty_puts(newline);
    tty_puts(rl_prompt);
    tty_string(rl_line_buffer);
    return CS_MOVE;
}

static el_status_t
redisplay_no_nl(void)
{
    tty_bol();
    tty_puts(rl_prompt);
    tty_string(rl_line_buffer);
    return CS_MOVE;
}

static el_status_t
toggle_meta_mode(void)
{
    meta_chars = !meta_chars;
    return redisplay();
}


static char *
next_hist(void)
{
    return hist.pos >= hist.size - 1 ? NULL : hist.lines[++hist.pos];
}

static char *
prev_hist(void)
{
    return hist.pos == 0 ? NULL : hist.lines[--hist.pos];
}

static el_status_t
do_insert_hist(char *p)
{
    if (p == NULL) {
        return ring_bell();
    }
    rl_point = 0;
    reposition();
    ceol();
    rl_end = 0;
    return insert_string(p);
}

static el_status_t
do_hist(char *(*move)(void))
{
    char *p;
    int i;

    i = 0;
    do {
        if ((p = (*move)()) == NULL) {
            return ring_bell();
        }
    } while (++i < repeat);
    return do_insert_hist(p);
}

static el_status_t
h_next(void)
{
    return do_hist(next_hist);
}

static el_status_t
h_prev(void)
{
    return do_hist(prev_hist);
}

static el_status_t
h_first(void)
{
    return do_insert_hist(hist.lines[hist.pos = 0]);
}

static el_status_t
h_last(void)
{
    return do_insert_hist(hist.lines[hist.pos = hist.size - 1]);
}

/*
 * Completion wrappers.
 */

static int
el_list_possib(char *text, char ***avp)
{
    int ac = 0;
    int len = 0;
    char **av;

    if (text) {
        len = sal_strlen(text);
    }
    if (rl_attempted_completion_function) {
        av = rl_attempted_completion_function(text, rl_point - len, rl_point);
        if (avp) {
            *avp = av;
        }
        if (av) {
            while (*av++) {
                ac++;
            }
        }
    }
    return ac;
}

static char *
el_complete(char *text, int list_possible, int *unique)
{
    char *p = NULL;
    char **av = NULL;
    int ac;
    int len = 0;
    int c_len, end, i, j;

    if (text) {
        len = sal_strlen(text);
    }
    ac = el_list_possib(text, &av);
    if (ac == 1) {
        *unique = 1;
        c_len = sal_strlen(av[0]) - len;
        p = EL_MALLOC(c_len + 2);
        if (p) {
            sal_memcpy(p, &av[0][len], c_len);
            p[c_len] = rl_completion_append_character;
            p[c_len + 1] = '\0';
        }
    } else if (ac) {
        *unique = 0;
        for (i = len, end = sal_strlen(av[0]); i < end; i++) {
            for (j = 1; j < ac; j++) {
                if (av[0][i] != av[j][i]) {
                    goto breakout;
                }
            }
        }
      breakout:
        if (i > len) {
            j = i - len + 1;
            if ((p = EL_MALLOC(j)) != NULL) {
                sal_memcpy(p, av[0] + len, j);
                p[j - 1] = '\0';
            }
        }
    }

    if (ac) {
        if (p == NULL && list_possible) {
            columns(ac, av);
            redisplay_no_nl();
        }
        while (--ac >= 0) {
            EL_FREE(av[ac]);
        }
        EL_FREE(av);
    }

    return p;
}

/*
 *  Return zero if pat appears as a substring in text.
 */
static int
el_substrcmp(char *text, char *pat, int len)
{
    char c;

    if ((c = *pat) == '\0') {
        return *text == '\0';
    }
    for ( ; *text; text++) {
        if (*text == c && sal_strncmp(text, pat, len) == 0) {
            return 0;
        }
    }
    return 1;
}

static int
el_strncmp(char *s1, char *s2, int len)
{
    return sal_strncmp(s1, s2, len);
}

static char *
search_hist(char *search, char *(*move)(void))
{
    static char *old_search;
    int len;
    int pos;
    int (*match)(char *, char *, int);
    char *pat;

    /* Save or get remembered search pattern. */
    if (search && *search) {
        if (old_search) {
            EL_FREE(old_search);
        }
        old_search = EL_STRDUP(search);
    }
    else {
        if (old_search == NULL || *old_search == '\0') {
            return NULL;
        }
        search = old_search;
    }

    /* Set up pattern-finder. */
    if (*search == '^') {
        match = el_strncmp;
        pat = (char *)(search + 1);
    }
    else {
        match = el_substrcmp;
        pat = (char *)search;
    }
    len = sal_strlen(pat);

    for (pos = hist.pos; (*move)() != NULL; ) {
        if ((*match)((char *)hist.lines[hist.pos], pat, len) == 0) {
            return hist.lines[hist.pos];
        }
    }
    hist.pos = pos;
    return NULL;
}

static el_status_t
h_search(void)
{
    static int searching;
    char *old_prompt;
    char *(*move)(void);
    char *p;

    if (searching) {
        return ring_bell();
    }
    searching = 1;

    clear_line();
    old_prompt = rl_prompt;
    rl_prompt = "Search: ";
    tty_puts(rl_prompt);
    move = repeat == NO_ARG ? prev_hist : next_hist;
    cursor_pos = sal_strlen(rl_prompt);
    p = editinput();
    searching = 0;
    if (p == NULL && pending_signal != SIG_NONE) {
        pending_signal = SIG_NONE;
        clear_line();
        rl_prompt = old_prompt;
        return redisplay_no_nl();
    }
    p = search_hist(p, move);
    clear_line();
    rl_prompt = old_prompt;
    if (p == NULL) {
        (void)ring_bell();
        return redisplay_no_nl();
    }
    return do_insert_hist(p);
}

static el_status_t
fd_char(void)
{
    int i;

    i = 0;
    do {
        if (rl_point >= rl_end) {
            break;
        }
        right(CS_MOVE);
    } while (++i < repeat);
    return CS_STAY;
}

static void
save_yank(int begin,int i)
{
    if (yanked) {
        EL_FREE(yanked);
        yanked = NULL;
    }

    if (i < 1) {
        return;
    }

    if ((yanked = EL_MALLOC(i + 1)) != NULL) {
        sal_memcpy(yanked, &rl_line_buffer[begin], i);
        yanked[i] = '\0';
    }
}

static el_status_t
delete_string(int count)
{
    int i;
    char *p;

    if (count <= 0 || rl_end == rl_point) {
        return ring_bell();
    }

    if (count == 1 && rl_point == rl_end - 1) {
        /* Optimize common case of delete at rl_end of line. */
        rl_end--;
        p = &rl_line_buffer[rl_point];
        i = 1;
        tty_put(' ');
        if (*p == TAB) {
            /* XXX */
        }
        else if (ISCTL(*p)) {
            i = 2;
            tty_put(' ');
        }
        else if (meta_chars && ISMETA(*p)) {
            i = 3;
            tty_put(' ');
            tty_put(' ');
        }
        tty_backn(i);
        *p = '\0';
        return CS_MOVE;
    }
    if (rl_point + count > rl_end && (count = rl_end - rl_point) <= 0) {
        return CS_STAY;
    }

    if (count > 1) {
        save_yank(rl_point, count);
    }

    ceol();
    for (p = &rl_line_buffer[rl_point], i = rl_end - (rl_point + count) + 1; --i >= 0; p++) {
        p[0] = p[count];
    }
    rl_end -= count;
    tty_string(&rl_line_buffer[rl_point]);
    return CS_MOVE;
}

static el_status_t
bk_char(void)
{
    int i;

    i = 0;
    do {
        if (rl_point == 0) {
            break;
        }
        left(CS_MOVE);
    } while (++i < repeat);

    return CS_STAY;
}

static el_status_t
bk_del_char(void)
{
    int i;

    i = 0;
    do {
        if (rl_point == 0) {
            break;
        }
        left(CS_MOVE);
    } while (++i < repeat);

    return delete_string(i);
}

static el_status_t
kill_line(void)
{
    int i;

    if (repeat != NO_ARG) {
        if (repeat < rl_point) {
            i = rl_point;
            rl_point = repeat;
            reposition();
            (void)delete_string(i - rl_point);
        }
        else if (repeat > rl_point) {
            right(CS_MOVE);
            (void)delete_string(repeat - rl_point - 1);
        }
        return CS_MOVE;
    }

    save_yank(rl_point, rl_end - rl_point);
    ceol();
    rl_line_buffer[rl_point] = '\0';
    rl_end = rl_point;
    return CS_STAY;
}

static el_status_t
insert_char(int c)
{
    el_status_t       s;
    char buff[2];
    char *p;
    char *q;
    int i;

    if (repeat == NO_ARG || repeat < 2) {
        buff[0] = c;
        buff[1] = '\0';
        return insert_string(buff);
    }

    if ((p = EL_MALLOC(repeat + 1)) == NULL) {
        return CS_STAY;
    }
    for (i = repeat, q = p; --i >= 0; ) {
        *q++ = c;
    }
    *q = '\0';
    repeat = 0;
    s = insert_string(p);
    EL_FREE(p);
    return s;
}

static el_status_t
meta(void)
{
    int c;
    el_keymap_t              *kp;

    if ((c = tty_get()) == EOF) {
        return CS_EOF;
    }
    /* Also include VT-100 arrows. */
    if (c == '[' || c == 'O')
        switch ((int)(c = tty_get())) {
        default:        return ring_bell();
        case EOF:       return CS_EOF;
        case 'A':       return h_prev();
        case 'B':       return h_next();
        case 'C':       return fd_char();
        case 'D':       return bk_char();
        }

    if (sal_isdigit(c)) {
        for (repeat = c - '0'; (c = tty_get()) != EOF && sal_isdigit(c); ) {
            repeat = repeat * 10 + c - '0';
        }
        pushed = 1;
        push_back = c;
        return CS_STAY;
    }

    if (sal_isupper(c)) {
        return do_macro((unsigned int)c);
    }
    old_point = rl_point;
    for (kp = meta_map; kp < &meta_map[META_MAP_SIZE]; kp++) {
        if (kp->key == c && kp->active) {
            return (*kp->function)();
        }
    }

    return ring_bell();
}

static el_status_t
emacs(int c)
{
    el_status_t s;
    el_keymap_t *kp;

    for (kp = map; kp < &map[MAP_SIZE]; kp++) {
        if (kp->key == c && kp->active) {
            break;
        }
    }
    s = kp < &map[MAP_SIZE] ? (*kp->function)() : insert_char(c);
    if (!pushed) {
        /* No pushback means no repeat count. */
        repeat = NO_ARG;
    }
    return s;
}

static el_status_t
tty_special(int c)
{
    if (meta_chars && ISMETA(c))
        return CS_DISPATCH;

    if (c == el_term_keys.erase || c == DEL)
        return bk_del_char();
    if (c == el_term_keys.kill) {
        if (rl_point != 0) {
            rl_point = 0;
            reposition();
        }
        repeat = NO_ARG;
        return kill_line();
    }
    if (c == el_term_keys.eof && rl_point == 0 && rl_end == 0)
        return CS_EOF;
    if (c == el_term_keys.intr) {
        pending_signal = SIG_INT;
        return CS_SIGNAL;
    }
    if (c == el_term_keys.quit) {
        pending_signal = SIG_QUIT;
        return CS_SIGNAL;
    }

    return CS_DISPATCH;
}

static char *
editinput(void)
{
    int c;

    repeat = NO_ARG;
    prev_compl_len = -1;
    old_point = rl_point = rl_mark = rl_end = 0;
    rl_line_buffer[0] = '\0';

    pending_signal = SIG_NONE;
    while ((c = tty_get()) != EOF) {
        switch (tty_special(c)) {
        case CS_DONE:
            return rl_line_buffer;
        case CS_EOF:
            return NULL;
        case CS_SIGNAL:
            return (char *)"";
        case CS_MOVE:
            reposition();
            break;
        case CS_DISPATCH:
            switch (emacs(c)) {
            case CS_DONE:
                return rl_line_buffer;
            case CS_EOF:
                return NULL;
            case CS_SIGNAL:
                return (char *)"";
            case CS_MOVE:
                reposition();
                break;
            case CS_DISPATCH:
            case CS_STAY:
                break;
            }
            break;
        case CS_STAY:
            break;
        }
    }
    return NULL;
}

static void
hist_add(char *p)
{
    int i;

    if ((p = EL_STRDUP(p)) == NULL) {
        return;
    }
    if (hist.size < EL_HIST_SIZE) {
        hist.lines[hist.size++] = p;
    }
    else {
        EL_FREE(hist.lines[0]);
        for (i = 0; i < EL_HIST_SIZE - 1; i++) {
            hist.lines[i] = hist.lines[i + 1];
        }
        hist.lines[i] = p;
    }
    hist.pos = hist.size - 1;
}

static char *
read_redirected(void)
{
    int size;
    char *p;
    char *line;
    char *eol;

    size = MEM_INC;
    line = EL_MALLOC(size);
    eol = line + size;

    for (p = line; ; p++) {
        if (p == eol) {
            p = EL_MALLOC(size + MEM_INC);
            sal_memcpy(p, line, size);
            EL_FREE(line);
            line = p;
            p = line + size;
            size += MEM_INC;
            eol = line + size;
        }
        if (term_read(p, 1) <= 0) {
            /* Ignore "incomplete" lines at EOF, just like we do for a tty. */
            EL_FREE(line);
            return NULL;
        }

        if (*p == '\n') {
            break;
        }
    }
    *p = '\0';
    return line;
}

int
rl_insert(int count, int c)
{
    if (count > 0) {
        repeat = count;
        (void)insert_char(c);
        (void)redisplay_no_nl();
    }
    return 0;
}

/*
 *  strcmp-like sorting predicate for qsort.
 */
static int
compare(const void *p1, const void *p2)
{
    const char **v1;
    const char **v2;

    v1 = (const char **)p1;
    v2 = (const char **)p2;
    return sal_strcmp(*v1, *v2);
}

char **
rl_completion_matches(const char *text, rl_compentry_func_t *match_fn)
{
    int state = 0;
    int ac;
    char **av;
    char **new;
    char *match;

    av = NULL;
    ac = 0;
    while (1) {
        if ((ac % MEM_INC) == 0) {
            new = EL_MALLOC((ac + MEM_INC) * sizeof(char *));
            if (new == NULL) {
                break;
            }
            if (ac) {
                sal_memcpy(new, av, ac * sizeof(char *));
                EL_FREE(av);
            }
            av = new;
        }
        match = match_fn(text, state);
        av[ac] = match;
        if (match == NULL) {
            break;
        }
        ac++;
        state = 1;
    }
    if (ac) {
        sal_qsort(av, ac, sizeof(char *), compare);
    } else if (av) {
        EL_FREE(av);
        av = NULL;
    }
    return av;
}

int
rl_on_new_line(void)
{
    redisplay_no_nl();
    return 0;
}

int
bcma_editline_init(bcma_editline_io_cb_t *io_cb,
                   bcma_editline_term_keys_t *term_keys)
{
    if (io_cb) {
        sal_memcpy(&el_io_cb, io_cb, sizeof(el_io_cb));
    }

    if (term_keys) {
        sal_memcpy(&el_term_keys, term_keys, sizeof(el_term_keys));
    }

    if (el_term_keys.eof == 0) {
        el_term_keys.eof = CTL('D');
    }
    if (el_term_keys.erase == 0) {
        el_term_keys.erase = CTL('H');
    }
    if (el_term_keys.intr == 0) {
        el_term_keys.intr = CTL('C');
    }
    if (el_term_keys.kill == 0) {
        el_term_keys.kill = CTL('U');
    }
    if (el_term_keys.quit == 0) {
        el_term_keys.quit = CTL('\\');
    }
    return 0;
}

int
bcma_editline_cleanup(void)
{
    int i;

    for (i = 0; i < EL_HIST_SIZE - 1; i++) {
        if (hist.lines[i]) {
            EL_FREE(hist.lines[i]);
            hist.lines[i] = NULL;
        }
    }
    sal_memset(&hist, 0, sizeof(hist));

    if (rl_line_buffer) {
        EL_FREE(rl_line_buffer);
        rl_line_buffer = NULL;
    }
    if (screen) {
        EL_FREE(screen);
        screen = NULL;
    }

    return 0;
}

char *
bcma_editline_history_get(int idx)
{
    if (idx < 0) {
        idx = hist.size + idx;
    }
    if (idx >= 0 && idx < hist.size) {
        return hist.lines[idx];
    }
    return NULL;
}

int
bcma_editline_history_length(void)
{
    return hist.size;
}

char *
readline(const char *prompt)
{
    char *line;

    rl_prompt = prompt ? (char *)prompt : (char *)nil;

    /*
     * If input is not a TTY, just do a read.
     * Also just do a read if TTY data is pending.
     */
    if (tty_set(0) < 0) {
        term_write(rl_prompt, sal_strlen(rl_prompt));
        return read_redirected();
    }

    if (screen == NULL) {
        screen_size = SCREEN_INC;
        screen = EL_MALLOC(screen_size);
    }

    if (rl_line_buffer == NULL) {
        length = MEM_INC;
        if ((rl_line_buffer = EL_MALLOC(length)) == NULL)
            return NULL;
    }

    pending_signal = SIG_NONE;

    tty_info();
    hist_add(nil);
    tty_puts(rl_prompt);
    cursor_pos = sal_strlen(rl_prompt);
    if ((line = editinput()) != NULL) {
        if (rl_catch_signals == 0 && pending_signal == SIG_INT) {
            /* Emulate GNU readline behavior */
        } else {
            tty_puts(newline);
            tty_flush();
        }
    }

    tty_set(1);

    --hist.size;
    EL_FREE(hist.lines[hist.size]);
    hist.lines[hist.size] = NULL;

    switch (pending_signal) {
    case SIG_INT:
	send_sigint();
	pending_signal = SIG_NONE;
        break;
    case SIG_QUIT:
	send_sigquit();
	pending_signal = SIG_NONE;
        break;
    default:
        break;
    }

    return line ? EL_STRDUP(line) : NULL;
}

void
add_history(char *p)
{
    if (p == NULL || *p == '\0') {
        return;
    }

    hist_add((char *)p);
}

int
history_expand(char *str, char **output)
{
    int rv = 0;
    int idx;
    char *end = str;
    char *cmd;
    char buf[80];

    if (str == NULL || output == NULL) {
        return 0;
    }

    if (str[0] == '!') {
        if (sal_strchr(" \t\n=", str[1]) == NULL) {
            if (str[1] == '!' && str[2] == '\0') {
                idx = -1;
            } else {
                idx = sal_strtol(str + 1, &end, 0);
            }
            if (idx != 0 || *end == '\0') {
                /* Expand from index */
                cmd = bcma_editline_history_get(idx);
                if (cmd) {
                    rv = 1;
                } else {
                    rv = -1;
                }
            } else {
                /* Expand from latest matching string */
                for (idx = hist.size - 1; idx >= 0; idx--) {
                    cmd = hist.lines[idx];
                    if (sal_strstr(cmd, str + 1) != NULL) {
                        rv = 1;
                        break;
                    }
                }
            }
        }
    }

    if (rv == 0) {
        *output = sal_strdup(str);
    } else if (rv > 0) {
        *output = sal_strdup(cmd);
    } else {
        sal_snprintf(buf, sizeof(buf), "%s: event not found", str);
        *output = sal_strdup(buf);
    }

    return rv;
}

static el_status_t
beg_line(void)
{
    if (rl_point) {
        rl_point = 0;
        return CS_MOVE;
    }
    return CS_STAY;
}

static el_status_t
del_char(void)
{
    return delete_string(repeat == NO_ARG ? 1 : repeat);
}

static el_status_t
end_line(void)
{
    if (rl_point != rl_end) {
        rl_point = rl_end;
        return CS_MOVE;
    }
    return CS_STAY;
}

/*
 *  Return allocated copy of word under cursor, moving cursor after the
 *  word.
 */
static char *
find_word(void)
{
    const char *SEPS = rl_basic_word_break_characters;
    char *p;
    char *new;
    size_t len;

    /* Move forward to end of word. */
    for (p = &rl_line_buffer[rl_point];
         rl_point < rl_end && sal_strchr(SEPS, (char)*p) == NULL;
         rl_point++, p++) {
        right(CS_STAY);
    }

    /* Back up to beginning of word. */
    for (p = &rl_line_buffer[rl_point];
         p > rl_line_buffer && sal_strchr(SEPS, (char)p[-1]) == NULL;
         p--) {
        continue;
    }
    len = rl_point - (p - rl_line_buffer) + 1;
    if ((new = EL_MALLOC(len)) == NULL) {
        return NULL;
    }
    sal_memcpy(new, p, len);
    new[len - 1] = '\0';
    return new;
}

static el_status_t
c_possible(void)
{
    char **av = NULL;
    char *word;
    int ac;

    word = find_word();
    ac = el_list_possib(word, &av);
    if (word) {
        EL_FREE(word);
    }
    if (ac) {
        columns(ac, av);
        while (--ac >= 0) {
            EL_FREE(av[ac]);
        }
        EL_FREE(av);
        return redisplay_no_nl();
    }
    return ring_bell();
}

static el_status_t
c_complete(void)
{
    char *p;
    char *word;
    int unique = 0;
    int len = 0;
    int list_possible;

    word = find_word();
    if (word) {
        len = sal_strlen(word);
    }
    list_possible = (len == prev_compl_len) ? 1 : 0;
    prev_compl_len = len;
    p = el_complete(word, list_possible, &unique);
    if (word) {
        EL_FREE(word);
    }
    if (p) {
        if (*p) {
            (void)insert_string(p);
        }
        if (!unique) {
            (void)ring_bell();
        }
        EL_FREE(p);
        return redisplay_no_nl();
    }
    return ring_bell();
}

static el_status_t
accept_line(void)
{
    rl_line_buffer[rl_end] = '\0';
    return CS_DONE;
}

static el_status_t
transpose(void)
{
    char c;

    if (rl_point) {
        if (rl_point == rl_end) {
            left(CS_MOVE);
        }
        c = rl_line_buffer[rl_point - 1];
        left(CS_STAY);
        rl_line_buffer[rl_point - 1] = rl_line_buffer[rl_point];
        tty_show(rl_line_buffer[rl_point - 1]);
        rl_line_buffer[rl_point++] = c;
        tty_show(c);
    }
    return CS_STAY;
}

static el_status_t
quote(void)
{
    int c;

    return (c = tty_get()) == EOF ? CS_EOF : insert_char((int)c);
}

static el_status_t
wipe(void)
{
    int i;

    if (rl_mark > rl_end) {
        return ring_bell();
    }

    if (rl_point > rl_mark) {
        i = rl_point;
        rl_point = rl_mark;
        rl_mark = i;
        reposition();
    }

    return delete_string(rl_mark - rl_point);
}

static el_status_t
mk_set(void)
{
    rl_mark = rl_point;
    return CS_STAY;
}

static el_status_t
exchange(void)
{
    int c;

    if ((c = tty_get()) != CTL('X')) {
        return c == EOF ? CS_EOF : ring_bell();
    }

    if ((c = rl_mark) <= rl_end) {
        rl_mark = rl_point;
        rl_point = c;
        return CS_MOVE;
    }
    return CS_STAY;
}

static el_status_t
yank(void)
{
    if (yanked && *yanked) {
        return insert_string(yanked);
    }
    return CS_STAY;
}

static el_status_t
copy_region(void)
{
    if (rl_mark > rl_end) {
        return ring_bell();
    }

    if (rl_point > rl_mark) {
        save_yank(rl_mark, rl_point - rl_mark);
    } else {
        save_yank(rl_point, rl_mark - rl_point);
    }

    return CS_STAY;
}

static el_status_t
move_to_char(void)
{
    int c;
    int i;
    char *p;

    if ((c = tty_get()) == EOF) {
        return CS_EOF;
    }
    for (i = rl_point + 1, p = &rl_line_buffer[i]; i < rl_end; i++, p++) {
        if (*p == c) {
            rl_point = i;
            return CS_MOVE;
        }
    }
    return CS_STAY;
}

static el_status_t
fd_word(void)
{
    return do_forward(CS_MOVE);
}

static el_status_t
fd_kill_word(void)
{
    int i;

    (void)do_forward(CS_STAY);
    if (old_point != rl_point) {
        i = rl_point - old_point;
        rl_point = old_point;
        return delete_string(i);
    }
    return CS_STAY;
}

static el_status_t
bk_word(void)
{
    int i;
    char *p;

    i = 0;
    do {
        for (p = &rl_line_buffer[rl_point]; p > rl_line_buffer && !sal_isalnum(p[-1]); p--) {
            left(CS_MOVE);
        }

        for (; p > rl_line_buffer && p[-1] != ' ' && sal_isalnum(p[-1]); p--) {
            left(CS_MOVE);
        }

        if (rl_point == 0) {
            break;
        }
    } while (++i < repeat);

    return CS_STAY;
}

static el_status_t
bk_kill_word(void)
{
    (void)bk_word();
    if (old_point != rl_point) {
        return delete_string(old_point - rl_point);
    }
    return CS_STAY;
}

static int
argify(char *line, char ***avp)
{
    char *c;
    char **p;
    char **new;
    int ac;
    int i;

    i = MEM_INC;
    if ((*avp = p = EL_MALLOC(sizeof(char *) * i))== NULL) {
         return 0;
    }

    for (c = line; sal_isspace(*c); c++) {
        continue;
    }
    if (*c == '\n' || *c == '\0') {
        return 0;
    }

    for (ac = 0, p[ac++] = c; *c && *c != '\n'; ) {
        if (sal_isspace(*c)) {
            *c++ = '\0';
            if (*c && *c != '\n') {
                if (ac + 1 == i) {
                    new = EL_MALLOC(sizeof(char *) * (i + MEM_INC));
                    if (new == NULL) {
                        p[ac] = NULL;
                        return ac;
                    }
                    sal_memcpy(new, p, i * sizeof (char *));
                    i += MEM_INC;
                    EL_FREE(p);
                    *avp = p = new;
                }
                p[ac++] = c;
            }
        } else {
            c++;
        }
    }
    *c = '\0';
    p[ac] = NULL;
    return ac;
}

static el_status_t
last_argument(void)
{
    char **av;
    char *p;
    el_status_t  s;
    int ac;

    if (hist.size == 1 || (p = hist.lines[hist.size - 2]) == NULL) {
        return ring_bell();
    }

    if ((p = EL_STRDUP(p)) == NULL) {
        return CS_STAY;
    }
    ac = argify(p, &av);

    if (repeat != NO_ARG) {
        s = repeat < ac ? insert_string(av[repeat]) : ring_bell();
    } else {
        s = ac ? insert_string(av[ac - 1]) : CS_STAY;
    }

    if (av) {
        EL_FREE(av);
    }
    EL_FREE(p);
    return s;
}

static el_keymap_t   map[MAP_SIZE] = {
    {   CTL('@'),       1,      mk_set          },
    {   CTL('A'),       1,      beg_line        },
    {   CTL('B'),       1,      bk_char         },
    {   CTL('D'),       1,      del_char        },
    {   CTL('E'),       1,      end_line        },
    {   CTL('F'),       1,      fd_char         },
    {   CTL('G'),       1,      ring_bell       },
    {   CTL('H'),       1,      bk_del_char     },
    {   CTL('I'),       1,      c_complete      },
    {   CTL('J'),       1,      accept_line     },
    {   CTL('K'),       1,      kill_line       },
    {   CTL('L'),       1,      redisplay       },
    {   CTL('M'),       1,      accept_line     },
    {   CTL('N'),       1,      h_next          },
    {   CTL('O'),       1,      ring_bell       },
    {   CTL('P'),       1,      h_prev          },
    {   CTL('Q'),       1,      ring_bell       },
    {   CTL('R'),       1,      h_search        },
    {   CTL('S'),       1,      ring_bell       },
    {   CTL('T'),       1,      transpose       },
    {   CTL('U'),       1,      ring_bell       },
    {   CTL('V'),       1,      quote           },
    {   CTL('W'),       1,      wipe            },
    {   CTL('X'),       1,      exchange        },
    {   CTL('Y'),       1,      yank            },
    {   CTL('Z'),       1,      ring_bell       },
    {   CTL('['),       1,      meta            },
    {   CTL(']'),       1,      move_to_char    },
    {   CTL('^'),       1,      ring_bell       },
    {   CTL('_'),       1,      ring_bell       },
};

static el_keymap_t   meta_map[META_MAP_SIZE]= {
    {   CTL('H'),       1,      bk_kill_word    },
    {   CTL('['),       1,      c_possible      },
    {   DEL,            1,      bk_kill_word    },
    {   ' ',            1,      mk_set          },
    {   '.',            1,      last_argument   },
    {   '<',            1,      h_first         },
    {   '>',            1,      h_last          },
    {   '?',            1,      c_possible      },
    {   'b',            1,      bk_word         },
    {   'd',            1,      fd_kill_word    },
    {   'f',            1,      fd_word         },
    {   'l',            1,      case_down_word  },
    {   'm',            1,      toggle_meta_mode},
    {   'u',            1,      case_up_word    },
    {   'y',            1,      yank            },
    {   'w',            1,      copy_region     },
};

#endif /* BCMA_NO_EDITLINE */
