/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bregex.c
 * Purpose:    Regex compiler
 */

#if 0
#define STANDALONE_MODE 1
#endif

#ifdef STANDALONE_MODE
#define INCLUDE_REGEX
#else
#define BROADCOM_SDK
#endif

#define START_ANCHOR_CHAR '^'

#define BITMASK_IS_SET(mask, bit)  (mask[bit / WORDS2BITS(1)] &   (1 << (bit % WORDS2BITS(1))))
#define BITMASK_SET(mask, bit)      mask[bit / WORDS2BITS(1)] |=  (1 << (bit % WORDS2BITS(1)))
#define BITMASK_UNSET(mask, bit)    mask[bit / WORDS2BITS(1)] &= ~(1 << (bit % WORDS2BITS(1)))

#define OPTIMIZE_PATTERN    1
#define DFA_STATE_BLOCK_SIZE    1024
#define NFA_BLOCK_SIZE          16300
#define NFA_STATE_BLOCK_SIZE    1000
#define REGEX_MAX_DFA_STATES    100000 
#define REGEX_MAX_MEM_USAGE     100000000 /* 100MB */ 
#define REGEX_MAX_TIMEOUT       1800 * 1000000 /* 30 Mins */ 
#define NFA_STACK_SIZE          4096 

/*
 * Enable DFA minimization after computation of DFA from NFA. This is 
 * highly recommeneded as it leads to compressing/optimizing the DFA
 * to in most cases to 2-4% of the orginal DFA.
 */
#define DFA_MINIMIZE            1

/*
 * Disable nextline character to not be matched when '.' Dot character
 * is encountered in the regex pattern.
 */
#define OPT_DONT_MATCH_NL_ON_DOT    0

#ifdef BROADCOM_SDK
/* Enabling the dump of final DFA. */
#define ENABLE_FINAL_DFA_DUMP   0

/* Enable the dump of inverted DFA before minimization step. */
#define ENABLE_INVERTED_DFA_DUMP 0

/* Enable the dump of DFA before minimization step. */
#define ENABLE_DFA_DUMP_BEFORE_MINIMIZATION 0

/* enable the dump of DFA states as the list of NFA state list. */
#define DUMP_DFA_STATE          0

/* dump the symbol classes that represent the pattern. */
#define DUMP_SYMBOL_CLASSES     0

#define ENABLE_NFA_DUMP         0

/* dump regular expression in postfix format */
#define ENABLE_POST_DUMP        0

#else

/* Enabling the dump of final DFA. */
#define ENABLE_FINAL_DFA_DUMP                   0

/* Enable the dump of inverted DFA before minimization step. */
#define ENABLE_INVERTED_DFA_DUMP                0

/* Enable the dump of DFA before minimization step. */
#define ENABLE_DFA_DUMP_BEFORE_MINIMIZATION     0

/* enable the dump of DFA states as the list of NFA state list. */
#define DUMP_DFA_STATE                          0

/* dump the symbol classes that represent the pattern. */
#define DUMP_SYMBOL_CLASSES                     0

#define ENABLE_NFA_DUMP                         0

/* dump regular expression in postfix format */
#define ENABLE_POST_DUMP        0

#endif

#if defined(INCLUDE_REGEX)

#include <shared/bsl.h>

#include <assert.h>

#ifdef BROADCOM_SDK
#include <soc/defs.h>
#include <soc/util.h>
#include <shared/alloc.h>
#include <shared/util.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/appl/io.h>
#include <bcm/error.h>
#include <bcm_int/regex_api.h>
#define RE_SAL_TOI(s1,base) sal_ctoi((s1),NULL)
#define RE_SAL_ITOA(s1,u,base)  sal_itoa((s1),u,base,0,0)
#define RE_SAL_DPRINT(args)  LOG_INFO(BSL_LS_BCM_REGEX, args)

#else

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "regex_api.h"

#define MEM_PROFILE     0

#if MEM_PROFILE==1
#define sal_alloc(s,t)  re_alloc_wrap((s),(t))
#define sal_free(p)   re_free_wrap((p))
#else
#define sal_alloc(s,t)  malloc((s))
#define sal_free(p)   free((p))
#endif

#define sal_strcpy(s1,s2) strcpy((s1),(s2))
#define RE_SAL_TOI(s1,base) strtol((s1),NULL, (base))
#define RE_SAL_ITOA(s1,u,base)  itoa(u,(s1),(base))
#define RE_SAL_DPRINT(p)   printf p
#define _shr_sort(a,b,s,n)    qsort((void*)(a),(b),(s),(n))
#define sal_strlen   strlen
#define sal_memset   memset
#define sal_memcpy   memcpy
#define sal_strdup   strdup
#define sal_memcmp   memcmp

#if MEM_PROFILE==1

static int peak_alloc = 0;
static int total_alloc = 0;

typedef struct _mem_profile_s {
    struct _mem_profile_s *next;
    void *buf;
    char loc[32];
    int size;
} _mem_profile_t;

static _mem_profile_t *alloc_list = NULL;

static void *re_alloc_wrap(int size, char *tag)
{
    void *b;
    _mem_profile_t *mp;

    b = malloc(size+sizeof(_mem_profile_t));
    if (b) {
         mp = (_mem_profile_t*)b;
         mp->size = size;
         mp->next = alloc_list;
         alloc_list = mp;
         sal_strcpy(mp->loc, tag);
         mp->buf = b + sizeof(_mem_profile_t);
         b += sizeof(_mem_profile_t);
         total_alloc += size;
         if (total_alloc > peak_alloc) {
            peak_alloc = total_alloc;
         }
    }
    return b;
}

static void re_free_wrap(void *b)
{
    _mem_profile_t **p = &alloc_list, *tmp;

    while(*p) {
        if (b == (*p)->buf) {
            tmp = *p;
            *p = tmp->next;
            total_alloc -= tmp->size;
            free(tmp);
            break;
        } else {
            p = &(*p)->next;
        }
    }
    return;
}

static void re_dump_mem_leak(void)
{
    int total = 0;
    _mem_profile_t *mp = alloc_list;

    while(mp) {
        total += mp->size;
        RE_SAL_DPRINT(("leak at :%s\n", mp->loc));
        mp = mp->next;
    }

    RE_SAL_DPRINT(("Total memory leak : %d bytes\n", total));
    if (peak_alloc > 0x100000) {
        RE_SAL_DPRINT(("Total Peak alloc  : %d MB \n", peak_alloc/0x100000));
    } else {
        RE_SAL_DPRINT(("Total Peak alloc  : %d KB\n", peak_alloc/1024));
    }
    return;
}

#endif /* MEM_PROFILE */

#endif

typedef struct re_wc {
    /*
    If ctrl indicates a meta-character, then c is an actual
    character.  However, if ctrl indicates data, then c is
    actually a character class, not a character
    */
    unsigned  char ctrl;
    unsigned  char c;
} re_wc;

#define POST_EN_DATA    0x01
#define POST_EN_META    0x02

#define POST_ENTRY(p)   *(p)

#define POST_EN_IS_META(p)  ((p)->ctrl == POST_EN_META)

#define WSTR_DATA_VALID(pws)  \
    (((pws)->ctrl == POST_EN_META) && ((pws)->c == '\0') ? 0 : 1)

#define WSTR_GET_DATA(pws)  (pws)->c

#define WSTR_GET_DATA_AT_OFF(pws, off)  WSTR_GET_DATA((pws)+(off))

#define WSTR_PTR_INC(pws,i)   (pws) += (i)

static int wstrlen(re_wc *wstring)
{
    int i = 0;

    while(WSTR_DATA_VALID(wstring)) {
        i++;
        WSTR_PTR_INC(wstring,1);
    }
    return i;
}

static void add_data_cmn(char c, re_wc **o, int *offset, int *pmax,
                          unsigned char dtype) 
{
    re_wc *tmp, *pwc; 
    int  max = *pmax, current_size, new_size;

    if (*offset == max) {
        current_size = max * sizeof(re_wc);
        new_size = max*2*sizeof(re_wc);
        tmp = sal_alloc(new_size,"re_wc1");
        sal_memset(tmp, 0, current_size);
        sal_memcpy(tmp, *o, current_size);
        sal_free(*o);
        *o = tmp;
        *pmax = max * 2;
    }

    /* first add the data type and then data */
    pwc = *o + *offset;
    pwc->ctrl = dtype;
    pwc->c = c;
    *offset = *offset + 1;
    return;
}

#define ADD_DATA(c, po, of, m)  \
        add_data_cmn((c), (po), (of), (m), POST_EN_DATA)

#define ADD_META_DATA(c, po, of, m)  \
        add_data_cmn((c), (po), (of), (m), POST_EN_META)

static int is_metachar(char c)
{
    if ((c == '(') || (c == '.') || (c == ')') || (c == '+') || 
        (c == '*') || (c == '|') || (c == '{') || (c == '}') ||
        (c == '?') || (c == '\\')) {
        return 1;
    }
    return 0;
}

#define INFINITE -1

static int parse_digit_from_range(re_wc **pb, char *mc)
{
    /* re_wc *s; */
    re_wc *b;
    int   d, ti = 0;
    char  tmp[12];

    b = *pb;
    WSTR_PTR_INC(b, 1);

    /* skip whitespaces */
    while (WSTR_DATA_VALID(b) && 
           ((WSTR_GET_DATA(b) < '0') || (WSTR_GET_DATA(b) > '9')) && 
           (WSTR_GET_DATA(b) != ',') && (WSTR_GET_DATA(b) != '}')) {
        WSTR_PTR_INC(b,1);
    }

    /* s = b;*/
    while (WSTR_DATA_VALID(b) && 
            ((WSTR_GET_DATA(b) >= '0') && (WSTR_GET_DATA(b) <= '9'))) {
        tmp[ti++] = WSTR_GET_DATA(b);
        WSTR_PTR_INC(b,1);
    }
    tmp[ti] = '\0';

    if (ti > 0) {
        d = RE_SAL_TOI(tmp,10);
    } else {
        d = INFINITE;
    }

    /* eat everything upto sep */
    *mc = '\0';
    while (WSTR_DATA_VALID(b) && 
            (WSTR_GET_DATA(b) != ',') && (WSTR_GET_DATA(b) != '}')) {
        WSTR_PTR_INC(b,1);
    }
    *mc = WSTR_GET_DATA(b);
    *pb = b;
    return d;
}

static char *classed="dDwWsSnrt.";

typedef struct re_class_info {
    char        c;
    re_wc       *exp;
    int         (*make_class_str)(int);
    unsigned int  flags;
} re_class_info;

static const unsigned int digit_tbl[] = {
        0x00000000, 
        0x03ff0000, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000
};

static const unsigned int wtbl[] = {
        0x00000000, 
        0x00000000, 
        0x07fffffe, 
        0x07fffffe, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000
};

static const unsigned int spacetbl[] = {
        0x00003e00, 
        0x00000001, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000
};

static const unsigned int isupper[] = {
        0x00000000, 
        0x00000000, 
        0x07fffffe, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000
};

static const unsigned int islower[] = {
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x07fffffe, 
        0x00000000, 
        0x00000000, 
        0x00000000, 
        0x00000000
};

static const unsigned int isprintable[] = {
        0x00000000,
        0xffffffff,
        0xffffffff,
        0x7fffffff,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000
};

#if (DUMP_SYMBOL_CLASSES == 1) || (ENABLE_FINAL_DFA_DUMP == 1) || (ENABLE_POST_DUMP == 1)
static int printable(unsigned char c)
{
    if (isprintable[c/32] & (1 << (c % 32))) {
        return 1;
    }
    return 0;
}
#endif

static int re_tolower(int c)
{
    c = c & 0xff;

    return (isupper[c/32] & (1 << (c%32))) ? c + 32 : c;
}

static int re_toupper(int c)
{
    c = c & 0xff;

    return (islower[c/32] & (1 << (c%32))) ? c - 32 : c;
}

static int re_isdigit(int c)
{
    c = c & 0xff;
    return (digit_tbl[c/32] & (1 << (c%32))) ? 1 : 0;
}

static int iswordc(int c)
{
    c = c & 0xff;
    return (wtbl[c/32] & (1 << (c%32))) ? 1 : 0;
}

static int re_isspace(int c)
{
    c = c & 0xff;
    return (spacetbl[c/32] & (1 << (c%32))) ? 1 : 0;
}


static int isnextline(int i)
{
    if (i == 0xa) {
        return 1;
    }
    return 0;
}
static int isreturn(int i)
{
    if (i == 0xd) {
        return 1;
    }
    return 0;
}
static int istab(int i)
{
    if (i == 0x9) {
        return 1;
    }
    return 0;
}

static int dot(int i)
{
#if OPT_DONT_MATCH_NL_ON_DOT == 1
    if (i == 0xa) {
        return 0;
    }
#endif
    return 1;
}

static re_class_info class_info[] = {
    /* \d class */
    {
        'd', NULL, re_isdigit, 1
    },
    /* \D class */
    {
        'D', NULL, re_isdigit, 0
    },
    /* \w class */
    {
        'w', NULL, iswordc, 1
    },
    /* \W class */
    {
        'W', NULL, iswordc, 0
    },
    /* \s class */
    {
        's', NULL, re_isspace, 1
    },
    /* \s class */
    {
        'S', NULL, re_isspace, 0
    },
    {
        'n', NULL, isnextline, 1
    },
    {
        'r', NULL, isreturn, 1
    },
    {
        't', NULL, istab, 1
    },
    {
        '.', NULL, dot, 1
    },
};

static re_wc *get_class_bitmap(char c, unsigned int *bitmap)
{
    int idx, r;
    char *pc = classed;
    re_class_info *pclass_info;
    int i;

    while(*pc && (*pc != c)) {
        pc++;
    }
    if (!*pc) {
        RE_SAL_DPRINT(("Failed to find class info : %c\n", c));
        return NULL;
    }
    idx = pc - classed;
    pclass_info = &class_info[idx];

    for (i = 0; i < 8; i++) {
        bitmap[i] = 0;
    }

    for (i = 0; i < REGEX_MAX_CHARACTER_CLASSES; i++) {
        r = !!pclass_info->make_class_str(i);
        if (r == pclass_info->flags) {
            bitmap[i/32] |= (1 << (i % 32));
        }
    }
        /*RE_SAL_DPRINT(("string for class %c : %s\n", c, s)); */
    return 0;
}

static int _regex_decode_hex(char *pre, unsigned int *val)
{
    char tmp[5], *re = pre;
    int d;

    tmp[0] = '0';
    tmp[1] = 'x';
    tmp[2] = *re;
    tmp[3] = *(re+1);
    tmp[4] = '\0';
    d = RE_SAL_TOI(tmp,16);
    *val = (unsigned int ) d;
    return 0;
}

static int 
re_make_char_class_bitmap(char **pre, unsigned int *bitmap, int ignore_neg,
                        unsigned int re_flag)
{
    unsigned int lc = 0, tc, d;
    int  esc = 0, neg = 0;
    int i, range = 0, j, oct[4];
    int lcuc = (re_flag & BCM_TR3_REGEX_CFLAG_EXPAND_LCUC) ? 1 : 0;
    char *re = *pre, *start;
    char bitmap_tmp[32];

    if (*re++ != '[') {
        return -1;
    }
    start = re;
    if (*re == '^') {
        if (!ignore_neg) {
            neg = 1;
        }
        re++;
        start = re;
    }

    sal_memset(bitmap, 0, sizeof(unsigned int)*8);
    sal_memset(bitmap_tmp, 0, 32);

    for(; *re && !(*re == ']' && !esc); re++) {
        if ((*re == '-') && !esc && (re != start)) {
            range = 1;
            continue;
        }
        if (!esc && (*re == '\\')) {
            esc = 0;
            switch(*(re+1)) {
                case 'n':
                    tc = 0xa;
                    re++;
                break;
                case 'r':
                    tc = 0xd;
                    re++;
                break;
                case 't':
                    tc = 9;
                    re++;
                break;
                case 'u':
                    re += 2; 
                case 'x':
                    re += 1;
                    if (_regex_decode_hex(re+1, &d)) {
                        return -1;
                    }
                    re += 2;
                    tc = d;
                    break;
                default:
                    if ((*re >= '0') && (*re <= '9')) {
                        /* octal */
                        j = 0;
                        while (*re && (*re >= '0') && (*re <= '9') && (j < 2)) {
                            oct[j++] = *re++;
                        }
                        re--;
                        tc = 0;
                        while(j > 0) {
                            tc = (tc*8) + (oct[j-1] - '0');
                            j--;
                        }
                    } else {
                        esc = 1;
                    }
                break;
            }
            if (esc) {
                continue;
            }
        } else {
            tc = *re;
        }
        if (!range) {
            lc = tc;
        }
        /* add range from lc to *re */
        for (i=lc; i<=tc;i++) {
            bitmap_tmp[i/8] |= 1 << (i % 8);
        }
        esc = 0;
        range = 0;
    }
    *pre = re;

    for (i = 0; i < REGEX_MAX_CHARACTER_CLASSES; i++) {
        if (!!(bitmap_tmp[i/8] & (1 << (i % 8))) == neg) {
            continue;
        }
        bitmap[i/32] |= 1 << (i % 32);
        if (lcuc && (wtbl[i/32] & (1 << (i % 32)))) {
            tc = i + ((i >= 'a') ? -32 : 32 );
            bitmap[tc/32] |= 1 << (tc % 32);
        }
    }
    return 0;
}

#ifndef REGEX_DEBUG_STACK

#define DEFINE_STACK(nt,t)    \
struct stack_of_##nt {    \
    t    *pstack;  \
    t    *stack;   \
};

#define DECLARE_STACK(nt)   struct stack_of_##nt

#define STACK_INIT(s,size,sn)  \
    (s)->pstack = (s)->stack = sal_alloc(sizeof(*(s)->stack)*size, (sn))

#define STACK_RESET(s)  (s)->pstack = (s)->stack

#define STACK_VALID(s)  ((s)->stack ? 1 : 0)

#define STACK_DEINIT(s)     if ((s)->stack) sal_free((s)->stack)

#define STACK_PUSH(s,p)     *(s)->pstack++ = (p)

#define STACK_POP(s)      *--(s)->pstack

#define STACK_EMPTY(s) ((s)->pstack == (s)->stack)

#else

#define DEFINE_STACK(nt,t)                      \
    struct stack_of_##nt {                      \
        t    *stack;                            \
        int   size;                             \
        int   top;                              \
        int   high;                             \
    }

#define DECLARE_STACK(nt)   struct stack_of_##nt

#define STACK_INIT(s, sz, sn)                                           \
    do {                                                                \
        (s)->stack = sal_alloc(sizeof(*(s)->stack) * sz, (sn));         \
        if ((s)->stack != NULL) {                                       \
            (s)->size = sz;                                             \
            (s)->top = 0;                                               \
            (s)->high = 0;                                              \
        }                                                               \
    } while (0)

#define STACK_RESET(s)          \
    assert(NULL != (s)->stack); \
    (s)->top = 0

#define STACK_VALID(s)  (NULL != (s)->stack ? 1 : 0)

#define STACK_DEINIT(s)                         \
    if (NULL != (s)->stack) {                   \
        sal_free((s)->stack);                   \
    }

#define STACK_PUSH(s,p)           \
    assert(NULL != (s)->stack);   \
    assert((s)->size > (s)->top); \
    (s)->stack[(s)->top] = (p);   \
    (s)->top++;                   \
    if ((s)->top > (s)->high) {   \
        (s)->high = (s)->top;     \
    }

#define STACK_POP(s)                            \
    ({                                          \
        assert(NULL != (s)->stack);             \
        assert((s)->top > 0);                   \
        (s)->top--;                             \
        (s)->stack[(s)->top];                   \
    })

#define STACK_EMPTY(s) (0 == (s)->top)

#endif  /* REGEX_DEBUG_STACK */

typedef struct re_transition_group_s {
    int num_transitions;
    unsigned int transition_map[8];
} re_transition_group;

typedef struct re_transition_class_s {
    int             id;
    int             num_transitions;
    unsigned int    transition_map[8];
} re_transition_class;

typedef struct re_membuf_blk_s {
    void    *buf;
    int     size;
    int     free_offset;
    int     unit_size;
    struct  re_membuf_blk_s *next;
} re_membuf_blk_t;

struct re_dfa_state_t;

typedef struct avn_s {
    int balance;
    struct re_dfa_state_t *data;
    struct avn_s *ds[2];
} avn_t;

typedef struct re_dfa_state_t re_dfa_state_t;
typedef struct re_dfa_t
{
    re_dfa_state_t **states;
    int num_states;
    int size;
    re_transition_class *class_array;
    int num_classes;
    re_membuf_blk_t *nbuf;
    re_membuf_blk_t *avbuf;
    avn_t   *avtree;
} re_dfa_t;

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == Match, no arrows out; matching state.
 * If c == Split, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */
enum
{
    Split = 256,
    Match = 257
};

typedef struct re_nfa_state_t re_nfa_state_t;
struct re_nfa_state_t
{
    int class;
    re_nfa_state_t *out;
    re_nfa_state_t *out1;
    int       id;
    int lastlist;
};

#define RE_NFA_STATES(l)        (l)->state_map

#define RE_NFA_LIST_LENGTH(l)   (l)->num_states

#define RE_NFA_STATE_IN_LIST(l,i) (l)->state_map[(i)]

DEFINE_STACK(nfa_states, re_nfa_state_t *);

static int state_map_byte_size;

typedef struct re_nfa_list_t
{
    int num_states;
    unsigned int *state_map;
} re_nfa_list_t;

#define NFA_SMAP_BYTE_SIZE(n)    ((n)->state_map_byte_size)

#define NFA_SMAP_WORD_SIZE(n)    (((n)->num_states+31)/32)

#define NFA_LIST_RESET(nfa,l)   \
{   \
    (l)->num_states = 0; \
    sal_memset((l)->state_map, 0, NFA_SMAP_BYTE_SIZE(nfa)); \
}

typedef struct re_nfa_t {
    re_nfa_state_t *root_state;
    int             state_map_byte_size;
    re_nfa_state_t **state_map;
    unsigned int    *class_map[REGEX_MAX_CHARACTER_CLASSES];
    int num_states;
    int state_map_size;
    re_membuf_blk_t *nbuf;
    DECLARE_STACK(nfa_states) *stk;
} re_nfa_t;

static int nfa_free(re_nfa_t *ns);

static re_membuf_blk_t* 
_alloc_membuf_block(re_membuf_blk_t **plist, int count, int unit_size)
{
    int size;
    re_membuf_blk_t *pb;

    size = sizeof(re_membuf_blk_t) + (unit_size * count);
    pb = sal_alloc(size,"membuf");
    sal_memset(pb, 0, size);
    pb->buf = (void*) (pb+1);
    pb->free_offset = 0;
    pb->unit_size = unit_size;
    pb->size = count;
    
    pb->next = *plist;
    *plist = pb;
    return pb;
}

static void *
_alloc_space_for_nfa_state_list(re_dfa_t *pdfa, int llen)
{
    int alloc;
    re_membuf_blk_t *pb;
    void *b;

    pb = pdfa->nbuf;
    alloc = ((pb == NULL) || ((pb->size - (pb->free_offset+1)) < llen));
    if (alloc) {
        pb = _alloc_membuf_block(&pdfa->nbuf, 
                                 NFA_BLOCK_SIZE, sizeof(unsigned int));
    }

    b = pb->buf + sizeof(unsigned int)*pb->free_offset;
    pb->free_offset += llen;
    return b;
}

static avn_t*
_alloc_space_for_avltree_nodes(re_dfa_t *pdfa, int count, int llen)
{
    int alloc;
    re_membuf_blk_t *pb;
    avn_t *b;

    pb = pdfa->avbuf;
    alloc = ((pb == NULL) || ((pb->size - (pb->free_offset+1)) < llen));
    if (alloc) {
        pb = _alloc_membuf_block(&pdfa->avbuf, count, sizeof(avn_t));
    }

    b = pb->buf + sizeof(avn_t)*pb->free_offset;
    pb->free_offset += llen;
    return b;
}

static re_nfa_state_t*
_alloc_space_for_nfa_nodes(re_nfa_t *pnfa, int count, int llen)
{
    int alloc;
    re_membuf_blk_t *pb;
    re_nfa_state_t *b;

    pb = pnfa->nbuf;
    alloc = ((pb == NULL) || ((pb->size - (pb->free_offset+1)) < llen));
    if (alloc) {
        pb = _alloc_membuf_block(&pnfa->nbuf, count, sizeof(re_nfa_state_t));
    }

    b = pb->buf + sizeof(re_nfa_state_t)*pb->free_offset;
    pb->free_offset += llen;
    return b;
}

static void _free_buf_blocks(re_membuf_blk_t *nb)
{
    re_membuf_blk_t *t;

    while(nb) {
        t = nb->next;
        sal_free(nb);
        nb = t;
    }
}

static int listid;

/*
 * Represents a DFA state: a cached NFA state list.
 */
struct re_dfa_state_t
{
    re_nfa_list_t l;
    unsigned int flags;
    int     *transition_list;
};

#define RE_DFA_STATE_ARCS(d)    (d)->transition_list

/*
 * In order to conserve space, flags field is encoded and contains the
 * following in encoded fashion.
 *   - state_id
 *   - final
 *   - marked
 *   ..
 */
#define RE_DFA_STATEID(d)       ((d)->flags & 0x7ffff)
#define RE_DFA_SET_STATEID(d,s)  \
                (d)->flags = ((d)->flags & 0xfff80000) | ((s) & 0x7ffff)

#define RE_DFA_IS_FINAL(d)      ((((d)->flags >> 19) & 0x1ff) > 0)
#define RE_DFA_FINAL(d)         (((d)->flags >> 19) & 0x1ff)
#define RE_DFA_SET_FINAL(d,m)   \
            (d)->flags = ((d)->flags & 0xf007ffff) | (((m) & 0x1ff) << 19)

#define RE_DFA_IS_MARKED(d)      (((d)->flags >> 28) & 0x1)
#define RE_DFA_SET_MARKED(d,m)     \
             (d)->flags = ((d)->flags & 0xefffffff) | (((m) & 0x1) << 28)

#define RE_DFA_IS_ADDED_TO_LIST(d)  (((d)->flags >> 29) & 0x1)
#define RE_DFA_ADD_TO_LIST(d)   (d)->flags |= (1 << 29)

#define RE_DFA_SET_BLOCK_MSTR(d) (d)->flags |= (1 << 30)

#define RE_CONNECT_DFA_STATES(d1,d2,c) (d1)->transition_list[(c)] = RE_DFA_STATEID(d2)

#define DFA_BLOCK_SIZE  1024

unsigned int mem_cou;

static re_dfa_state_t* _alloc_dfa_state_block(re_dfa_state_t**tbl,
                                              int num_classes)
{
    re_dfa_state_t *ps;
    int *transition_list = NULL, szs, szt, i, count;

    count = DFA_BLOCK_SIZE;
    szs = sizeof(re_dfa_state_t)*count;
    ps = sal_alloc(szs,"dfasb");
    if (!ps) {
        goto error;
    }
    mem_cou += szs;
    /* allocate transition list and attach to states */
    szt = sizeof(int)*count*num_classes;
    transition_list = sal_alloc(szt,"dfalst");
    if (!transition_list) {
        goto error;
    }
    mem_cou += szt;
    sal_memset(ps, 0, szs);
    sal_memset(transition_list, 0xff, szt);

    for (i = 0; i < count; i++) {
        tbl[i] = &ps[i];
        ps[i].transition_list = transition_list;
        transition_list += num_classes;
    }

    RE_DFA_SET_BLOCK_MSTR(&ps[0]);

    return ps;

error:
    if (ps) {
        sal_free(ps);
    }

    if (transition_list) {
        sal_free(transition_list);
    }
    return NULL;
}

static re_transition_group *_add_to_transition_group_list(re_transition_group **top, int top_size, re_transition_group *this)
{
    re_transition_group *new_grp, *ptmp;
    int matched = 0, i, j, off;

    off = -1;
    for (i = 0; i < top_size; i++) {
        ptmp = top[i]; 
        if (ptmp == NULL) {
            if (off == -1) {
                off = i;
            }
            continue;
        }
        matched = 1;
        /* compare the tr group */
        for (j = 0; j < 8; j++) {
            if (this->transition_map[j] != ptmp->transition_map[j]) {
                matched = 0;
                break;
            }
        }
        if (matched) {
            break;
        }
    }

    if (!matched) {
        assert((off >= 0) && (off < top_size));
        new_grp = sal_alloc(sizeof(re_transition_group),"trgrp");
        sal_memcpy(new_grp, this, sizeof(re_transition_group));
        top[off] = new_grp;
    }
    return 0;
}

#define RE_RESET_TRANSITION_MAP(ptr) sal_memset((ptr), 0, sizeof(re_transition_group))

#define RE_ADD_TRANSITION_TO_GROUP(ptr,tr)                                \
{                                                               \
    if (((ptr)->transition_map[(tr)/32] & (1 << ((tr) % 32))) == 0) {    \
        (ptr)->num_transitions++;                                        \
        (ptr)->transition_map[(tr)/32] |= (1 << ((tr) % 32));            \
    }                                                           \
}

#if 0
static int
sort_transition_group(void *a, void *b)
{
    re_transition_group *pa, *pb;
    pa = *((re_transition_group**) a);
    pb = *((re_transition_group**) b);
    return pa->num_transitions - pb->num_transitions;
}
#endif

static int
sort_transition_class(void *a, void *b)
{
    re_transition_class *pa, *pb;
    int ma, mb, i, j;

    pa = (re_transition_class*) a;
    pb = (re_transition_class*) b;
    
    ma = -1;
    for (i = 0; i < 8; i++) {
        if (pa->transition_map[i] == 0) {
            continue;
        }
        for (j = 0; j < 32; j++) {
            if (pa->transition_map[i] & (1 << (j % 32))) {
                ma = (i*32) + j;
                break;
            }
        }
        if (ma >= 0) {
            break;
        }
    }
    mb = -1;
    for (i = 0; i < 8; i++) {
        if (pb->transition_map[i] == 0) {
            continue;
        }
        for (j = 0; j < 32; j++) {
            if (pb->transition_map[i] & (1 << (j % 32))) {
                mb = (i*32) + j;
                break;
            }
        }
        if (mb >= 0) {
            break;
        }
    }
    return ma - mb;
}

static int _regex_add_token_one_char(re_transition_group **top, int top_size, unsigned char c)
{
    re_transition_group curmap;

    RE_RESET_TRANSITION_MAP(&curmap);
    RE_ADD_TRANSITION_TO_GROUP(&curmap, c);
    _add_to_transition_group_list(top, top_size, &curmap);
    return 0;
}

static int _regex_add_token_multi(re_transition_group **top, int top_size, unsigned int *bitmap)
{
    re_transition_group curmap;
    int i;

    RE_RESET_TRANSITION_MAP(&curmap);
    for (i = 0; i < REGEX_MAX_CHARACTER_CLASSES; i++) {
        if (bitmap[i/32] & (1 << (i % 32))) {
            RE_ADD_TRANSITION_TO_GROUP(&curmap, i);
        }
    }
    _add_to_transition_group_list(top, top_size, &curmap);
    return 0;
}

static int 
re_add_single_class(re_transition_class *pclass_array, int num_classes, unsigned int c,
                                  re_wc **o, int *used, int *size)
{
    int i, class_id;

    for (i = 0; i < num_classes; i++) {
        #if 0
        if (pclass_array[i].num_transitions != 1) {
            continue;
        }
        #endif
        if (pclass_array[i].transition_map[c/32] & (1 << (c % 32))) {
            class_id = pclass_array[i].id;
            if (is_metachar(class_id)) {
                ADD_DATA('\\', o, used, size);
            }
            ADD_DATA(class_id, o, used, size);
            return 0;
        }
    }
    return -1;
}

static int 
re_add_multi_class(re_transition_class *pclass_array, int num_classes,
                      unsigned int *bitmap, re_wc **o, int *used, int *size)
{
    int i, j, class_id, first = 1, matched;

    ADD_DATA('(', o, used, size);
    for (i = 0; i < num_classes; i++) {
        for (j = 0; j < 8; j++) {
            matched = 0;
            if (pclass_array[i].transition_map[j] & (bitmap[j])) {
                matched = 1;
            }
            if (matched) {
                class_id = pclass_array[i].id;
                if (!first) {
                    ADD_DATA('|', o, used, size);
                }
                if (is_metachar(class_id)) {
                    ADD_DATA('\\', o, used, size);
                }
                ADD_DATA(class_id, o, used, size);
                first = 0;
                break;
            }
        }
    }
    ADD_DATA(')', o, used, size);
    return 0;
}

#ifdef OPTIMIZE_PATTERN
/*
 * Optimize the patterns. Some of the optimizations are:
 *  remove trailing .*
 */
static int
re_optimize_patterns(char **patterns, int num_pattern)
{
    int p, esc = 0;
    char *mark = NULL, *re;
    return 0;
    for(p=0; p <num_pattern; p++) {
        re = patterns[p];
        while(*re) {
            if (*re == '\\') {
                esc = !!esc;
            } else if (!esc && (*re == '.')) {
                if (*(re + 1) == '+') {
                    re++;
                    mark = re;
                    re++;
                } else if (*(re+1) == '*') {
                    mark = re;
                    re++;
                }
            } else {
                mark = NULL;
            }
            re++;
        }
        if (mark) {
            *mark = '\0';
        }
    }
    return 0;
}
#endif /* OPTIMIZE_PATTERN */

static int
re_case_adjust(char **patterns, int num_pattern, unsigned int *re_flags)
{
    int pi, esc = 0,remap=0;
    char *re;
    unsigned int u;
    char buf[5];

    for (pi = 0; pi < num_pattern; pi++) {
        if ((re_flags[pi] & (BCM_TR3_REGEX_CFLAG_EXPAND_UC |
                       BCM_TR3_REGEX_CFLAG_EXPAND_LC)) == 0) {
            continue;
        }
        re = patterns[pi];
        while(*re) {
            if (*re == '\\') {
                esc = !esc;
                re++;
                continue;
            }
            if (esc) {
                switch(*re) {
                    case 'x':
                    case 'X':
                        if (_regex_decode_hex(re+1, &u)) {
                            return -1;
                        }
                        if (re_flags[pi] & BCM_TR3_REGEX_CFLAG_EXPAND_UC) {
                            if ((u >= 97) && (u <= 122)) {
                                u = u - 32;
                                remap=1;
                            }
                        } else if (re_flags[pi] & BCM_TR3_REGEX_CFLAG_EXPAND_LC) {
                            if ((u >= 65) && (u <= 90)) {
                                u = u + 32;
                                remap=1;
                            }
                        }
                        if(remap)
                        {
                            sal_memset(buf,0,sizeof(buf));
                            RE_SAL_ITOA(buf,u,16);
                            *(re+1) = *buf;
                            *(re+2) = *(buf+1);
                            remap=0;
                        }    
                        re += 3;
                        break;
                    default:
                        re++;
                        break;
                }
                esc = 0;
                continue;
            }
            if (re_flags[pi] & BCM_TR3_REGEX_CFLAG_EXPAND_UC) {
                if ((*re >= 97) && (*re <= 122)) {
                    *re = *re - 32;
                }
            } else if (re_flags[pi] & BCM_TR3_REGEX_CFLAG_EXPAND_LC) {
                if ((*re >= 65) && (*re <= 90)) {
                    *re = *re + 32;
                }
            }
            re++;
        }
    }
    return 0;
}

/*
 * decompose the transitions into unique classes. The idea is to
 * extract the least common individual transitions which can 
 * be uniquely represented. Say for example if the pattern
 * is /abc/, it can be represented by character a, b and c. But if
 * the pattern is /a.c/, in this case '.' will be represented as
 * (\0|\1|\2|...\254|\255), it not only makes the pattern big, also
 * the possible symbols in the patterns are 256. Whereas this can be 
 * represented as 3 symbols, ie. S1 ==> {a}, S2 ==> {b} and 
 *  S3 =={anything not a and b}, so we can now represent the same pattern
 * /a.c/ as /S1(S1|S2|S3)S2/ which makes the symbol table size just 3
 * instead of 256.
 */
static int
re_make_symbol_classes_from_pattern(char **patterns, int num_pattern, 
                            unsigned int *re_flag,
                                    re_transition_class **pclass_array, int *num_classes)
{
    int   cl_num = 0, i, j, k, count, p, esc = 0, rv = 0;
    unsigned int bitmap[REGEX_MAX_CHARACTER_CLASS_WORDS], oct[4], u;
    re_transition_group **transition_list;
    re_transition_class *class_array = NULL, *class;
    char *re;
#if DUMP_SYMBOL_CLASSES == 1
    int lc = -1;
#endif
    int first_unused;
    int transition_list_max;
    int patterns_length = 0;

    for (p = 0; p < num_pattern; p++) {
        patterns_length += sal_strlen(patterns[p]);
    }
    /*
     * This calculates the maximum number of classes that can be coded in the patterns.
     * There are 9 classes that are signified by a backslash followed by a letter (see
     * below).  Each of those are use two characters.  Other character classes (ranges)
     * take, at a minimum, two characters for an opening and closing square bracket, two
     * characters for the beginning and ending values and one character for the dash for a
     * total of five characters.  Add to this the single character classes to get a safe
     * maximum.
     *
     * Note that these classes are later condensed into the minimum number of classes
     * required which can be, at most, REGEX_MAX_CHARACTER_CLASSES.
     */
#define NUM_2CHAR_CLASSES 9
#define RANGE_SIZE        5
    transition_list_max = ((patterns_length - NUM_2CHAR_CLASSES * 2) / RANGE_SIZE) + NUM_2CHAR_CLASSES + REGEX_MAX_CHARACTER_CLASSES;
    transition_list = sal_alloc(transition_list_max * sizeof(transition_list[0]), "transition_list");
   
    sal_memset(transition_list, 0, transition_list_max * sizeof(transition_list[0]));

    for(p=0; p <num_pattern; p++) {
        re = patterns[p];
        while(*re) {
            switch(*re) {
                case '\\':
                    if (!esc) {
                        esc = 1;
                    } else {
                        _regex_add_token_one_char(transition_list, transition_list_max, *re);
                        esc = 0;
                    }
                    break;
                case '[':
                    if (esc) {
                        _regex_add_token_one_char(transition_list, transition_list_max, *re);
                        esc = 0;
                    } else {
                        re_make_char_class_bitmap(&re, bitmap, 1,
                                                re_flag ? re_flag[p] : 0);
                        _regex_add_token_multi(transition_list, transition_list_max, bitmap);
                    }
                    break;
                case '{':
                    if (esc) {
                        _regex_add_token_one_char(transition_list, transition_list_max, *re);
                        esc = 0;
                    } else {
                        while (*re != '}') {
                            re++;
                        }
                    }
                    break;
                default:
                    if (!esc) {
                        if (!((*re == '(') || (*re == ')') || 
                             (*re == '|') || (*re == '*') || (*re == '+'))) {
                            if (*re == '.') {
                                #if 0
                                get_class_bitmap(*re, bitmap);
                                _regex_add_token_multi(transition_list, transition_list_max, bitmap);
                                #endif
                            } else {
                                if ((re_flag) && 
                                    (re_flag[p] & BCM_TR3_REGEX_CFLAG_EXPAND_LCUC) &&
                                    (re_tolower(*re) != re_toupper(*re))) {
                                    _regex_add_token_one_char(transition_list, transition_list_max, re_tolower(*re));
                                    _regex_add_token_one_char(transition_list, transition_list_max, re_toupper(*re));
                                } else {
                                    _regex_add_token_one_char(transition_list, transition_list_max, *re);
                                }
                            }
                        }
                    } else {
                        switch(*re) {
                            case 'd':
                            case 'D':
                            case 'w':
                            case 'W':
                            case 's':
                            case 'S':
                            case 'n':
                            case 'r':
                            case 't':
                                get_class_bitmap(*re, bitmap);
                                _regex_add_token_multi(transition_list, transition_list_max, bitmap);
                                break;
                            case 'u':
                                re += 2;
                            case 'x':
                                if (_regex_decode_hex(re+1, &u)) {
                                    rv = -1;
                                    goto fail;
                                }
                                re += 2;
                                _regex_add_token_one_char(transition_list, transition_list_max, (char)u);
                                break;
                            default:
                                if ((*re >= '0') && (*re <= '9')) {
                                    /* octal */
                                    j = 0;
                                    while (*re && (*re >= '0') && (*re <= '9') && (j < 2)) {
                                        oct[j++] = *re++;
                                    }
                                    re--;
                                    i = 0;
                                    while(j > 0) {
                                        i = (i*8) + (oct[j-1] - '0');
                                        j--;
                                    }
                                    _regex_add_token_one_char(transition_list, transition_list_max, i);
                                } else {
                                    _regex_add_token_one_char(transition_list, transition_list_max, *re);
                                }
                                break;
                        }
                        esc = 0;
                    }
                    break;
            }
            re++;
        }
    }

#if OPT_DONT_MATCH_NL_ON_DOT == 1
    _regex_add_token_one_char(transition_list, transition_list_max, 0xa);
#endif

    sal_memset(bitmap, 0xff, sizeof(bitmap));
    for (i = 0; (NULL != transition_list[i]) && (i < transition_list_max); i++) {
        for (j = 0; j < 8; j++) {
            bitmap[j] &= ~transition_list[i]->transition_map[j];
        }
    }

    /* Now add a transition for all characters not already in a transition. */
    for (j = 0; j < COUNTOF(bitmap); j++) {
        if (bitmap[j]) {
            _regex_add_token_multi(transition_list, transition_list_max, bitmap);
            break;
        }
    }

    /* Put characters that are listed in more than one class in their own class.  This
     * should result in each character appearing in at most one class. */
    for (i = 0; i < REGEX_MAX_CHARACTER_CLASSES; i++) {
        /* k counts the number of classes in which a character is found. */
        k = 0;
        for (j = 0; j < transition_list_max; j++) {
            if (transition_list[j] == NULL) {
                continue;
            }
            if (BITMASK_IS_SET(transition_list[j]->transition_map, i)) {
                k++;
            }
            /* If a character is in more than one class, no need to search for more
             * classes.  It will definitely be moved to a new class.  Proceed to the code
             * that removes this character from all classes. */
            if (k >= 2) {
                break;
            }
        }
        if (k >= 2) {
            for (j = 0; j < transition_list_max; j++) {
                if (transition_list[j] == NULL) {
                    continue;
                }
                if (BITMASK_IS_SET(transition_list[j]->transition_map, i)) {
                    /* if this was the last character in the class, delete the class. */
                    if (transition_list[j]->num_transitions == 1) {
                        sal_free(transition_list[j]);
                        transition_list[j] =  NULL;
                    } else {
                        BITMASK_UNSET(transition_list[j]->transition_map, i);
                        transition_list[j]->num_transitions--;
                    }
                }
            }
            /* Now create a new class with only this character. */
            _regex_add_token_one_char(transition_list, transition_list_max, i);
        }
    }

    /* This variable holds the index of the first unused entry in transition_list.  Initialize to
     * an invalid index. */
    first_unused = -1;
    /* Compact and remove any null class pointers and count the number of classes. */
    for (count = 0, i = 0; i < transition_list_max; i++) {
        if (NULL == transition_list[i]) {
            if (first_unused < 0) {
                first_unused = i;
            }
        } else {
            if (first_unused >= 0) {
                transition_list[first_unused] = transition_list[i];
                transition_list[i] = NULL;
                for (j = first_unused; j <= i; j++) {
                    if (NULL == transition_list[j]) {
                        first_unused = j;
                        break;
                    }
                }
        }
            count++;
        }
    }

    class_array = sal_alloc(sizeof(class_array[0]) * REGEX_MAX_CHARACTER_CLASSES, "class_array");
    sal_memset(class_array, 0, sizeof(class_array[0]) * REGEX_MAX_CHARACTER_CLASSES);
    cl_num = 0;
    for (i = 0; i < count; i++) {
        assert(NULL != transition_list[i]);
        class = &class_array[cl_num];
        class->num_transitions = 0;
        for (j = 0; j < REGEX_MAX_CHARACTER_CLASSES; j++) {
            if (BITMASK_IS_SET(transition_list[i]->transition_map, j)) {
                BITMASK_SET(class->transition_map, j);
                class->num_transitions++;
            }
        }
        if (class->num_transitions) {
            cl_num++;
        }
    }

    _shr_sort(class_array, count, sizeof(re_transition_class), sort_transition_class);

    for (i = 0; i < cl_num; i++) {
        class_array[i].id = i;
    }

#if DUMP_SYMBOL_CLASSES == 1
    RE_SAL_DPRINT(("\n\nNumber of classes: %d: \n", cl_num));
    for (i = 0; i < cl_num; i++) {
        class = &class_array[i];
        lc = -1;
        RE_SAL_DPRINT(("\nClass %d: ", class->id));
        for (j = 0; j < REGEX_MAX_CHARACTER_CLASSES; j++) {
            if (BITMASK_IS_SET(class->transition_map, j)) {
                if (lc == -1) {
                    lc = j;
                }
            } else {
                if (lc >= 0) {
                    RE_SAL_DPRINT(("["));
                    if (printable(lc)) {
                        RE_SAL_DPRINT(("%c", lc));
                    } else {
                        RE_SAL_DPRINT(("\\%d", lc));
                    }
                    RE_SAL_DPRINT(("-"));
                    if (printable(j - 1)) {
                        RE_SAL_DPRINT(("%c", j - 1));
                    } else {
                        RE_SAL_DPRINT(("\\%d", j - 1));
                    }
                    RE_SAL_DPRINT(("]"));
                    lc = -1;
                }
            }
        }
        if (lc >= 0)  {
            if (printable(lc)) {
                RE_SAL_DPRINT(("[%c", lc));
            } else {
                RE_SAL_DPRINT(("[\\%d", lc));
            }
            RE_SAL_DPRINT(("-\\255]"));
        }
        RE_SAL_DPRINT(("\n"));
        
    }
#endif
   
    *pclass_array = class_array;
    *num_classes  = cl_num;

fail:
    /* free up transition_lists */
    if (transition_list) {
        for (i = 0; i < transition_list_max; i++) {
            if (transition_list[i]) {
                sal_free(transition_list[i]);
        }
    }
        sal_free(transition_list);
    }
    if (rv) {
        *num_classes = 0;
        *pclass_array = NULL;
        sal_free(class_array);
    }
    return rv;
}

#if ENABLE_POST_DUMP==1
static void dump_post(re_wc *postfix, int detail)
{
    re_wc *p;

    RE_SAL_DPRINT(("\n-------Postfix for re\n\n"));
    for (p = postfix; WSTR_DATA_VALID(p); WSTR_PTR_INC(p, 1)) {
        RE_SAL_DPRINT(("%s%d:", POST_EN_IS_META(p) ? "m" : "c", p->c));
    }
    RE_SAL_DPRINT(("\n"));
    for(p=postfix; WSTR_DATA_VALID(p); WSTR_PTR_INC(p,1)) {
        if (POST_EN_IS_META(p)) {
            if (printable(p->c)) {
                RE_SAL_DPRINT(("%c", p->c));
            }
            else
            {
                RE_SAL_DPRINT(("[m%d]", p->c));
            }
        } else {
            RE_SAL_DPRINT(("[%d]", p->c));
        }
    }
    RE_SAL_DPRINT(("\n"));
}
#endif

static re_wc* 
re_convert_tokens_to_class(char *re, unsigned int re_flag, 
                                re_transition_class *pclass_array, int num_classes)
{
    re_wc *output_wchar;
    int   re_length, used = 0, esc = 0, mod_re_length = 0, c;
    unsigned int d;
    int oct[4], j;
    unsigned int bitmap[8];

    re_length = sal_strlen(re);
    if (!re_length) {
        return NULL;
    }

    /* 
     * each character uses 2 bytes, since \0 is valid data, so inorder to
     * distinguish end of string with \0, we store metadata and character.
     */
    mod_re_length = re_length + 1;
    if (0 == (re_flag & BCM_TR3_REGEX_CFLAG_ANCHORED)) {
        mod_re_length += 2;
    }
    output_wchar = sal_alloc(mod_re_length * sizeof(re_wc), "re2class");
    used = 0;

    /* If the RE is not anchored at the beginning of the packet, then insert an atom that
     * will match anything (.*) to begin the RE. */
    if (0 == (re_flag & BCM_TR3_REGEX_CFLAG_ANCHORED)) {
        get_class_bitmap('.', bitmap);
        if (re_add_multi_class(pclass_array, num_classes,
                               bitmap, &output_wchar, &used, &mod_re_length)) {
            RE_SAL_DPRINT(("\n-------Error re_add_multi_class '.'\n\n"));
            goto fail;
        }
        ADD_DATA('*', &output_wchar, &used, &mod_re_length);
    }

    while(*re) {
        switch(*re) {
            case '\\':
                if (!esc) {
                    esc = 1;
                } else {
                    if (re_add_single_class(pclass_array, num_classes, *re,
                                                        &output_wchar, &used, &mod_re_length)) {
                        RE_SAL_DPRINT(("\n-------Error re_add_single_class '\'\n\n"));
                        goto fail;
                    }
                    esc = 0;
                }
                break;
            case '[':
                if (esc) {
                    esc = 0;
                    if (re_add_single_class(pclass_array, num_classes,
                                                *re, &output_wchar, &used, &mod_re_length)) {
                        RE_SAL_DPRINT(("\n-------Error re_add_single_class '['\n\n"));
                        goto fail;
                    }
                } else {
                    re_make_char_class_bitmap(&re, bitmap, 0, re_flag ? re_flag : 0);
                    if (re_add_multi_class(pclass_array, num_classes,
                                            bitmap, &output_wchar, &used, &mod_re_length)) {
                        RE_SAL_DPRINT(("\n-------Error re_add_multi_class '['\n\n"));
                        goto fail;
                    }
                }
                break;
            case '{':
                if (esc) {
                    esc = 0;
                    if (re_add_single_class(pclass_array, num_classes,
                                                *re, &output_wchar, &used, &mod_re_length)) {
                        RE_SAL_DPRINT(("\n-------Error re_add_single_class '{'\n\n"));
                        goto fail;
                    }
                } else {
                    do {
                        ADD_DATA(*re, &output_wchar, &used, &mod_re_length);
                    } while (*re++ != '}');
                    continue;
                }
                break;
            default:
                if (!esc) {
                    if (*re == '.') {
                        get_class_bitmap(*re, bitmap);
                        if (re_add_multi_class(pclass_array, num_classes,
                                                bitmap, &output_wchar, &used, &mod_re_length)) {
                            RE_SAL_DPRINT(("\n-------Error re_add_multi_class default '.'\n\n"));
                            goto fail;
                        }
                    } else {
                        if (is_metachar(*re)) {
                            ADD_DATA(*re, &output_wchar, &used, &mod_re_length);
                        } else {
                            if ((re_flag & BCM_TR3_REGEX_CFLAG_EXPAND_LCUC) &&
                                (re_tolower(*re) != re_toupper(*re))) {
                                ADD_DATA('(', &output_wchar, &used, &mod_re_length);
                                if (re_add_single_class(pclass_array, num_classes,
                                                 re_tolower(*re), &output_wchar, &used, &mod_re_length)) {
                                    RE_SAL_DPRINT(("\n-------Error re_add_single_class meta '{'\n\n"));
                                    goto fail;
                                }
                                ADD_DATA('|', &output_wchar, &used, &mod_re_length);
                                if (re_add_single_class(pclass_array, num_classes,
                                                  re_toupper(*re), &output_wchar, &used, &mod_re_length)) {
                                    RE_SAL_DPRINT(("\n-------Error re_add_single_class meta '|'\n\n"));
                                    goto fail;
                                }
                                ADD_DATA(')', &output_wchar, &used, &mod_re_length);
                            } else {
                                if (re_add_single_class(pclass_array, num_classes,
                                                            *re, &output_wchar, &used, &mod_re_length)) {
                                    RE_SAL_DPRINT(("\n-------Error re_add_single_class meta '}'\n\n"));
                                    goto fail;
                                }
                            }
                        }
                    }
                } else {
                    switch(*re) {
                        case 'd':
                        case 'D':
                        case 'w':
                        case 'W':
                        case 's':
                        case 'S':
                        case 'n':
                        case 'r':
                        case 't':
                            get_class_bitmap(*re, bitmap);
                            if (re_add_multi_class(pclass_array, num_classes,
                                                        bitmap, &output_wchar, &used, &mod_re_length)) {
                                RE_SAL_DPRINT(("\n-------Error re_add_multi_class special 'dDwWsSnrt'\n\n"));
                                goto fail;
                            }
                            break;
                        case 'u':
                            re += 2; /* ?? */
                        case 'x':
                            if (_regex_decode_hex(re+1, &d)) {
                                goto fail;
                            }
                            re += 2;
                            if (re_add_single_class(pclass_array, num_classes, d,
                                                &output_wchar, &used, &mod_re_length)) {
                                RE_SAL_DPRINT(("\n-------Error re_add_single_class 'x'\n\n"));
                                goto fail;
                            }
                            break;
                        default:
                            c = *re;
                            if ((*re >= '0') && (*re <= '9')) {
                                /* octal */
                                j = 0;
                                while (*re && (*re >= '0') && (*re <= '9') && (j < 2)) {
                                    oct[j++] = *re++;
                                }
                                re--;
                                c = 0;
                                while(j > 0) {
                                    c = (c*8) + (oct[j-1] - '0');
                                    j--;
                                }
                            }
                            if (re_add_single_class(pclass_array, num_classes,
                                                    c, &output_wchar, &used, &mod_re_length)) {
                                RE_SAL_DPRINT(("\n-------Error re_add_single_class default\n\n"));
                                goto fail;
                            }
                            break;
                    }
                    esc = 0;
                }
                break;
        }
        re++;
    }

    ADD_META_DATA('\0', &output_wchar, &used, &mod_re_length);

#if ENABLE_POST_DUMP==1
    RE_SAL_DPRINT(("\nchar re\n"));
    dump_post(output_wchar, 1);
#endif

    return output_wchar;

fail:
    sal_free(output_wchar);
    return NULL;
}

static re_wc * re_preprocess(re_wc *re)
{
    int   min, max;
    int   l, used = 0, esc = 0, m, c, j, skip;
    char  mc;
    int   toklen = 0, star;
    int    ts[100], stackp, curtok_off = 0;
    re_wc *curtok = NULL, *o;

#define pusht(off) ts[stackp++] = (off)
#define popt() (stackp == 0) ? -1 : ts[--stackp]

    stackp = 0;

    l = wstrlen(re);
    if (!l) {
        return NULL;
    }
    
    m = l+1;
    o = sal_alloc(m*sizeof(re_wc),"repreprocess");
    used = 0;

    while(WSTR_DATA_VALID(re)) {
        /* make sure we have enough room */
        switch(WSTR_GET_DATA(re)) {
            case '{':
                if (esc) {
                    ADD_DATA(WSTR_GET_DATA(re), &o, &used, &m);
                } else {
                    #if 0
                    if (!curtok) {
                        goto FAIL;
                    }
                    #endif
                    if (toklen <= 0) {
                        goto FAIL;
                    }
                    /* decode min, max */
                    min = parse_digit_from_range(&re, &mc);
                    if (min == INFINITE) {
                        goto FAIL;
                    } else if (mc == '}') {
                        max = min;
                    } else {
                        max = parse_digit_from_range(&re, &mc);
                    }
                    skip = 1;

                    /* repeat the previous token */
                    if (min) {
                        /* since token is already put in output once, do it min-1*/
                        for (c = 0; c < min; c++) {
                            if (skip) {
                                skip--;
                                continue;
                            }
                            j = 0;
                            while (j < toklen) {
                                curtok = o + curtok_off;
                                ADD_DATA(WSTR_GET_DATA_AT_OFF(curtok,j), 
                                                            &o, &used, &m);
                                j++;
                            }
                        }
                    }
                    star = 0;
                    if (max == INFINITE) {
                        max = 1;
                        star = 1;
                    } else {
                        max -= min;
                    }
                    if (max > 0) {
                        for (c = 0; c < max; c++) {
                            if (skip == 0) {
                                j = 0;
                                while (j < toklen) {
                                    curtok = o + curtok_off;
                                    ADD_DATA(WSTR_GET_DATA_AT_OFF(curtok,j), 
                                             &o, &used, &m);
                                    j++;
                                }
                            } else {
                                skip--;
                            }
                            ADD_DATA(star ? '*' : '?', &o, &used, &m);
                        }
                    } 
                    curtok = NULL;
                    toklen = 0;
                }
                break;

            case '(':
                pusht(used);
                ADD_DATA(WSTR_GET_DATA(re), &o, &used, &m);
                break;

            case ')':
                ADD_DATA(WSTR_GET_DATA(re), &o, &used, &m);
                curtok_off = popt();
                toklen = used - curtok_off;
                break;

            case '\\':
                curtok_off = used;
                ADD_DATA(WSTR_GET_DATA(re), &o, &used, &m);
                WSTR_PTR_INC(re,1);
                ADD_DATA(WSTR_GET_DATA(re), &o, &used, &m);
                toklen = 2;
                break;

            default:
                curtok_off = used;
                toklen = 1;
                ADD_DATA(WSTR_GET_DATA(re), &o, &used, &m);
                break;
        }
        WSTR_PTR_INC(re,1);
    }
    ADD_META_DATA('\0', &o, &used, &m);
    return o;
FAIL:
    sal_free(o);
    return NULL;
}

#define REPOST_STK_SIZE 100
static re_wc* re2post(re_wc *re)
{
    int nalt, natom, lmc = 0;
    int      size, used = 0;
    re_wc *dst, *post; /* *start */
    struct {
        int nalt;
        int natom;
    } paren[REPOST_STK_SIZE], *p;

    if (!re) {
        return NULL;
    }
    /* start = re; */
    p = paren;
    size = wstrlen(re) * 4;
    post = dst = sal_alloc(sizeof(re_wc)*size,"re2post");
    nalt = 0;
    natom = 0;
    for(; WSTR_DATA_VALID(re); WSTR_PTR_INC(re,1)) {
        switch(WSTR_GET_DATA(re)) {
            case '(':
                if (natom > 1) {
                    --natom;
                    ADD_META_DATA('.', &dst, &used, &size);
                }
                if (p >= paren+REPOST_STK_SIZE) {
                    RE_SAL_DPRINT(("paren stack full..\n"));
                    goto fail;
                }
                p->nalt = nalt;
                p->natom = natom;
                p++;
                nalt = 0;
                natom = 0;
                lmc = 0;
                break;
            case '|':
                if (natom == 0) {
                    RE_SAL_DPRINT(("No atoms to connect..\n"));
                    goto fail;
                }
                while(--natom > 0) {
                    ADD_META_DATA('.', &dst, &used, &size);
                }
                nalt++;
                lmc = 0;
                break;
            case ')':
                if (p == paren) {
                    RE_SAL_DPRINT(("point to paren to and yet paren found..\n"));
                    goto fail;
                }
                if (natom == 0) {
                    RE_SAL_DPRINT(("No atom in ) case....\n"));
                    return NULL;
                }
                while(--natom > 0) {
                    ADD_META_DATA('.', &dst, &used, &size);
                }
                for(; nalt > 0; nalt--) {
                    ADD_META_DATA('|', &dst, &used, &size);
                }
                --p;
                nalt = p->nalt;
                natom = p->natom;
                natom++;
                lmc = 0;
                break;
            case '*':
            case '+':
            case '?':
                if (lmc) {
                    RE_SAL_DPRINT(("error: meta char without atoms\n"));
                    goto fail;
                }
                if (natom == 0) {
                    RE_SAL_DPRINT(("No atom in + or * or ? case....\n"));
                    goto fail;
                }
                ADD_META_DATA(WSTR_GET_DATA(re), &dst, &used, &size);
                lmc = 1;
                break;
            default:
                lmc = 0;
                if (natom > 1) {
                    --natom;
                    ADD_META_DATA('.', &dst, &used, &size);
                }
                if (WSTR_GET_DATA(re) == '\\') {
                    WSTR_PTR_INC(re,1);
                }
                ADD_DATA(WSTR_GET_DATA(re), &dst, &used, &size);
                natom++;
                break;
        }
    }
    if (p != paren) {
        return NULL;
    }
    while(--natom > 0) {
        ADD_META_DATA('.', &dst, &used, &size);
    }
    for(; nalt > 0; nalt--) {
        ADD_META_DATA('|', &dst, &used, &size);
    }
    ADD_META_DATA('\0', &dst, &used, &size);

#if ENABLE_POST_DUMP==1
    dump_post(post, 1);
#endif

    return post;
    
fail:
    sal_free(dst);
    return NULL;
}


/* Allocate and initialize re_nfa_state_t */
re_nfa_state_t*
new_nfa_state(re_nfa_t *pnfa, int class, re_nfa_state_t *out, re_nfa_state_t *out1)
{
    re_nfa_state_t *state, **prev;
    int size;

    if (pnfa->state_map_size == pnfa->num_states) {
        prev = pnfa->state_map;
        size = sizeof(re_nfa_state_t*)*(pnfa->state_map_size+NFA_STATE_BLOCK_SIZE);
        pnfa->state_map = sal_alloc(size,"nfa_stpp");
        sal_memset(pnfa->state_map, 0, size);
        if (prev) {
            sal_memcpy(pnfa->state_map, prev,
                            sizeof(re_nfa_state_t*)*pnfa->state_map_size);
            sal_free(prev);
        }

        pnfa->state_map_size += NFA_STATE_BLOCK_SIZE;
    }


    /* alloc new block for NFA states if full */
    state = _alloc_space_for_nfa_nodes(pnfa, DFA_STATE_BLOCK_SIZE, 1);
    
    state->lastlist = 0;
    state->class = class;
    state->out = out;
    state->out1 = out1;
    state->id = pnfa->num_states++;
    pnfa->state_map[state->id] = state;
    return state;
}

typedef struct re_nfa_frag_t re_nfa_frag_t;
typedef union re_nfa_ptrl_t re_nfa_ptrl_t;
struct re_nfa_frag_t
{
    re_nfa_state_t *start;
    re_nfa_ptrl_t *out;
};

DEFINE_STACK(frag, re_nfa_frag_t);

/* Initialize re_nfa_frag_t struct. */
static re_nfa_frag_t frag(re_nfa_state_t *start, re_nfa_ptrl_t *out)
{
    re_nfa_frag_t n = { start, out };
    return n;
}

/*
 * Since the out pointers in the list are always 
 * uninitialized, we use the pointers themselves
 * as storage for the Ptrlists.
 */
union re_nfa_ptrl_t
{
    re_nfa_ptrl_t *next;
    re_nfa_state_t *s;
};

/* Create singleton list containing just outp. */
static re_nfa_ptrl_t* list1(re_nfa_state_t **outp)
{
    re_nfa_ptrl_t *l;

    l = (re_nfa_ptrl_t*)outp;
    l->next = NULL;
    return l;
}

/* Patch the list of states at out to point to start. */
static void patch(re_nfa_ptrl_t *l, re_nfa_state_t *s)
{
    re_nfa_ptrl_t *next;

    for(; l; l=next) {
        next = l->next;
        l->s = s;
    }
}

/* Join the two lists l1 and l2, returning the combination. */
static re_nfa_ptrl_t* ptrl_append(re_nfa_ptrl_t *l1, re_nfa_ptrl_t *l2)
{
    re_nfa_ptrl_t *oldl1;

    oldl1 = l1;
    while(l1->next) {
        l1 = l1->next;
    }
    l1->next = l2;
    return oldl1;
}

static re_nfa_state_t* post2nfa(re_nfa_t *pnfa, re_wc *postfix, int match_idx)
{
    re_wc *p;
    re_nfa_frag_t e1, e2, e;
    re_nfa_state_t *s;
    re_nfa_state_t *matchstate;
    DECLARE_STACK(frag) fstack;

    if (postfix == NULL) {
        return NULL;
    }

    STACK_INIT(&fstack, 1024, "post_stk");

    if (!STACK_VALID(&fstack)) {
        return NULL;
    }

#define push(s) STACK_PUSH(&fstack,s)
#define pop()   STACK_POP(&fstack)
#define validate_stack if (STACK_EMPTY(&fstack)) break
 
    for(p=postfix; WSTR_DATA_VALID(p); WSTR_PTR_INC(p,1)) {
        if (POST_EN_IS_META(p)) {
            switch(p->c) {
                case '.':	/* catenate */
                    validate_stack; 
                    e2 = pop();
                    validate_stack; 
                    e1 = pop();
                    patch(e1.out, e2.start);
                    push(frag(e1.start, e2.out));
                    break;
                case '|':	/* alternate */
                    validate_stack; 
                    e2 = pop();
                    validate_stack; 
                    e1 = pop();
                    s = new_nfa_state(pnfa, Split, e1.start, e2.start);
                    push(frag(s, ptrl_append(e1.out, e2.out)));
                    break;
                case '?':	/* zero or one */
                    validate_stack; 
                    e = pop();
                    s = new_nfa_state(pnfa, Split, e.start, NULL);
                    push(frag(s, ptrl_append(e.out, list1(&s->out1))));
                    break;
                case '*':	/* zero or more */
                    validate_stack; 
                    e = pop();
                    s = new_nfa_state(pnfa, Split, e.start, NULL);
                    patch(e.out, s);
                    push(frag(s, list1(&s->out1)));
                    break;
                case '+':	/* one or more */
                    validate_stack; 
                    e = pop();
                    s = new_nfa_state(pnfa, Split, e.start, NULL);
                    patch(e.out, s);
                    push(frag(e.start, list1(&s->out1)));
                    break;
            }
        } else {
            s = new_nfa_state(pnfa, p->c, NULL, NULL);
            push(frag(s, list1(&s->out)));
        }
    }

    e = pop();
    if (!STACK_EMPTY(&fstack))  {
        
        return NULL;
    }
    STACK_DEINIT(&fstack);

    matchstate = new_nfa_state(pnfa, Match + match_idx, NULL, NULL);
    patch(e.out, matchstate);
    return e.start;
#undef pop
#undef push
}

static void all_states_connected_by_epsilon(re_nfa_t *nfa,
                            re_nfa_list_t *l, re_nfa_state_t *s);
static void step(re_nfa_t *nfa, re_nfa_list_t*, int, re_nfa_list_t*);

/* Compute initial state list */
static re_nfa_list_t* closure_0(re_nfa_t *nfa, re_nfa_state_t *start, 
                                re_nfa_list_t *l)
{
    listid++;
    NFA_LIST_RESET(nfa, l);
    all_states_connected_by_epsilon(nfa, l, start);
    return l;
}

static void all_states_connected_by_epsilon(re_nfa_t *nfa, 
                                re_nfa_list_t *nlist, re_nfa_state_t *s)
{
    DECLARE_STACK(nfa_states) *stk;

    stk = nfa->stk;

    STACK_RESET(stk);
    STACK_PUSH(stk, s);

    while(!STACK_EMPTY(stk)) {
        s = STACK_POP(stk);
        if (s->lastlist == listid) {
            continue;
        }

        if (s->class == Split) {
            STACK_PUSH(stk, s->out);
            STACK_PUSH(stk, s->out1);
            continue;
        }
        s->lastlist = listid;
        nlist->state_map[s->id/32] |= (1 << (s->id % 32));
        nlist->num_states++;
    }
}

static void step(re_nfa_t *nfa, re_nfa_list_t *clist, 
                    int c, re_nfa_list_t *nlist)
{
    int i, j, word_size;
    re_nfa_state_t *s;
    unsigned int val, *cmap;

    listid++;
    NFA_LIST_RESET(nfa, nlist);

    cmap = nfa->class_map[c];
    word_size = NFA_SMAP_WORD_SIZE(nfa);
    for (i=0; i<word_size; i++) {
        val = cmap[i] & clist->state_map[i];
        for (j = 0; val && (j < 32); j++) {
            if ((val & (1 << j)) == 0) {
                continue;
            }
            val &= ~(1 << j);
            s = nfa->state_map[(i*32)+j];
            all_states_connected_by_epsilon(nfa, nlist, s->out);
        }
    }
}

/* Compare lists: first by length, then by members. */
static int
listcmp(re_nfa_list_t *l1, re_nfa_list_t *l2)
{
    int i;

    if (l1->num_states < l2->num_states) {
        return -1;
    }
    if (l1->num_states > l2->num_states) {
        return 1;
    }

    i = sal_memcmp(l1->state_map, l2->state_map, state_map_byte_size);
    i = (i < 0) ? -1 : ((i > 0) ? 1 : 0);
    return i;
}

static re_dfa_state_t* 
_lookup_dfa_from_cache(re_dfa_t *pdfa, re_nfa_list_t *l)
{
    int i;
    avn_t *node;

    node = pdfa->avtree;
    while(node) {
        i = listcmp(&node->data->l, l);
        if (i == 0) {
            return node->data;
        } else {
            node = (i < 0) ? node->ds[1] : node->ds[0];
        }
    }
    return NULL;
}

static avn_t *rotate_single(avn_t *root, int dir)
{
    avn_t *save = root->ds[!dir];
    root->ds[!dir] = save->ds[dir];
    save->ds[dir] = root;
    return save;
}

static avn_t *rotate_double (avn_t *root, int dir)
{
    avn_t *save = root->ds[!dir]->ds[dir];

    root->ds[!dir]->ds[dir] = save->ds[!dir];
    save->ds[!dir] = root->ds[!dir];
    root->ds[!dir] = save;

    save = root->ds[!dir];
    root->ds[!dir] = save->ds[dir];
    save->ds[dir] = root;

    return save;
}

static avn_t *
_add_dfa_state_to_cache(avn_t *root, avn_t *newen, 
                        re_dfa_state_t* d, int *done)
{
    int dir, bal, diff;
    avn_t *n, *nn;

    if (!root) {
        newen->balance = 0;
        newen->data = d;
        newen->ds[0] = newen->ds[1] = NULL;
        return newen;
    } else {
        diff = listcmp(&root->data->l, &d->l);
        if (diff == 0) {
            return root;
        }
        dir = (diff < 0);
        root->ds[dir] = _add_dfa_state_to_cache(root->ds[dir], newen, d, done);
        if ( !*done ) {
            root->balance += (dir == 0) ? -1 : +1;
            if (root->balance == 0) {
                *done = 1;
            } else if ((root->balance > 1) || (root->balance < -1)) {
                n = root->ds[dir];
                bal = (dir == 0) ? -1 : 1;
                if ( n->balance == bal ) {
                    root->balance = n->balance = 0;
                    root = rotate_single(root, !dir);
                }
                else { 
                    n = root->ds[dir];
                    nn = n->ds[!dir];

                    if (nn->balance == 0) {
                        root->balance = n->balance = 0;
                    } else if (nn->balance == bal) {
                        root->balance = -bal;
                        n->balance = 0;
                    } else {
                        root->balance = 0;
                        n->balance = bal;
                    }

                    nn->balance = 0;
                    root = rotate_double(root, !dir );
                }
                *done = 1;
            }
        }
    }
    return root;
}


/*
 * Return the cached re_dfa_state_t for list l,
 * creating a new one if needed.
 */
static int state_id = 0;

static re_dfa_state_t* 
new_dfa_state(re_nfa_list_t *l, int num_classes, re_dfa_t *pdfa, re_nfa_t *nfa)
{
    int size, done;
    re_dfa_state_t *d, **prev;
    avn_t *avn;

    /* lookup dfa state (list of nfa states) in cache if exist. */
    d = _lookup_dfa_from_cache(pdfa, l);
    if (d) {
        return d;
    }

    /* DFA states are stored as an array. If the number of entries
     * in the array is equal to array size, grow the array */
    if (pdfa->num_states == pdfa->size) {
        if(pdfa->num_states >= REGEX_MAX_DFA_STATES) {
            /* Stop */ 
            return NULL;
        }
        prev = pdfa->states;
        size = sizeof(re_dfa_state_t*)*(pdfa->size+DFA_BLOCK_SIZE);
        pdfa->states = sal_alloc(size,"dfa_stpp");
        if(! pdfa->states) {
            pdfa->states = prev;
            return NULL;
        }
        mem_cou += size;
        if (prev) {
            sal_memcpy(pdfa->states, prev, sizeof(re_dfa_state_t*)*pdfa->size);
            sal_free(prev);
        }

        /* Alloc dfa state block */
        if (!_alloc_dfa_state_block(pdfa->states + pdfa->size, num_classes)) {
            return NULL;
        }
        pdfa->size += DFA_BLOCK_SIZE;
    }

    d = pdfa->states[pdfa->num_states];
    d->l.state_map = _alloc_space_for_nfa_state_list(pdfa, NFA_SMAP_WORD_SIZE(nfa));
    mem_cou += NFA_SMAP_WORD_SIZE(nfa);
    sal_memcpy(d->l.state_map, l->state_map, NFA_SMAP_BYTE_SIZE(nfa));
    d->l.num_states = RE_NFA_LIST_LENGTH(l);

    /* alloc space for avl node */
    avn = _alloc_space_for_avltree_nodes(pdfa, DFA_BLOCK_SIZE, 1);
    mem_cou += DFA_BLOCK_SIZE;
    done = 0;
    pdfa->avtree = _add_dfa_state_to_cache(pdfa->avtree, avn, d, &done);

    return d;
}

static re_dfa_state_t* 
next_dfa_state_on_transition(re_dfa_state_t *d, int c, re_nfa_list_t *l1, 
                             int num_classes, re_dfa_t *pdfa, re_nfa_t *nfa)
{
    step(nfa, &d->l, c, l1);
    return new_dfa_state(l1, num_classes, pdfa, nfa);
}

static int add_state_to_dfa(re_dfa_t *pdfa, re_dfa_state_t *d)
{
#if DUMP_DFA_STATE == 1
    int num_states = 0;
#endif

    /* check if already exist */
    if (RE_DFA_IS_ADDED_TO_LIST(d)) {
        return 0;
    }
    RE_DFA_SET_STATEID(d, pdfa->num_states);
    RE_DFA_ADD_TO_LIST(d);

#if DUMP_DFA_STATE == 1
    {
        int k, l, word_size;
        word_size = state_map_byte_size / sizeof(unsigned int);
        RE_SAL_DPRINT(("DFA State %d represents nfa states:\n", pdfa->num_states));
        for (k = 0; k < word_size; k++) {
            if (d->l.state_map[k] == 0) {
                continue;
            }
            for (l = 0; l < 32; l++) {
                if (d->l.state_map[k] & (1 << l)) {
                    num_states++;
                    RE_SAL_DPRINT((" %d,", (k*32)+l));
                }
            }
        }
        RE_SAL_DPRINT(("\n"));
        RE_SAL_DPRINT(("DFA State %d represents %d nfa states total\n\n", pdfa->num_states, num_states));
    }
#endif
    pdfa->num_states++;
    return 0;
}

static re_dfa_state_t*
get_next_unmarked_dstate(re_dfa_t *l, int skip)
{
    int ii;
    re_dfa_state_t *d;

    if (skip < 0) {
        skip = 0;
    }

    for (ii = skip; ii < l->num_states; ii++) {
        d = l->states[ii];
        if(!d) {
            return NULL;
        }
        if (!RE_DFA_IS_MARKED(d)) {
            return d;
        }
    }
    return NULL;
}

static int get_transition_list(re_nfa_t *nfa, re_dfa_state_t *d, 
                                unsigned char *transition_list, int *ntr)
{
    re_nfa_state_t *s;
    int     c, i, j, word_size, ctr;
    unsigned int val;
    unsigned int transition_map[8];

    ctr = *ntr = 0;
    sal_memset(transition_map, 0, sizeof(unsigned int)*8);

    word_size = NFA_SMAP_WORD_SIZE(nfa);
    for (i=0; i<word_size; i++) {
        if (d->l.state_map[i] == 0) {
            continue;
        }
        val = d->l.state_map[i];
        for (j = 0; val && (j < 32); j++) {
            if ((val & (1 << j)) == 0) {
                continue;
            }
            val &= ~(1 << j);
            s = nfa->state_map[(i*32)+j];
            if (s->class >= Match) {
                continue;
            }
            c = s->class & 0xff;
            if ((transition_map[c/32] & (1 << (c % 32))) == 0) {
                transition_list[ctr++] = c;
                transition_map[c/32] |= (1 << (c % 32));
            }
        }
    }
    *ntr = ctr; 
    return 0;
}

static int dfa_free(re_dfa_t *l);

static int nfa_to_dfa(re_nfa_t *nfa, re_dfa_t *pdfa, int num_classes)
{
    re_dfa_state_t *d, *d1;
    re_nfa_state_t *s;
    int     i, k, l, c, word_size, byte_size, from = 0, rv = REGEX_ERROR_NONE, ntr, match_id;
    unsigned int val;
    re_nfa_list_t l1 = { 0, NULL };
    re_nfa_list_t l2 = { 0, NULL };
    unsigned char transition_list[REGEX_MAX_CHARACTER_CLASSES];
    DECLARE_STACK(nfa_states) nfa_stk;
    int running_divisor = 0;
    soc_timeout_t timeout;
#if DUMP_DFA_STATE == 1
    int *nlist = NULL, nc;
#endif

    mem_cou=0;
    listid = 0;

    byte_size = NFA_SMAP_BYTE_SIZE(nfa);
    l1.state_map = sal_alloc(byte_size, "tmp_l1");
    if(!l1.state_map) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
    sal_memset(l1.state_map, 0, byte_size);
    l2.state_map = sal_alloc(byte_size, "tmp_l2");
    if(!l2.state_map) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
    sal_memset(l2.state_map, 0, byte_size);
#if DUMP_DFA_STATE == 1
    nlist = sal_alloc((nfa->num_states * sizeof(int)), "dbg_nlist");
    if (nlist == NULL) {
        RE_SAL_DPRINT(("\nUnable to allocate NFA debug info for %d states\n", nfa->num_states));
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
#endif

    STACK_INIT(&nfa_stk, NFA_STACK_SIZE, "nfa_stk");
    nfa->stk = &nfa_stk;

    d = new_dfa_state(closure_0(nfa, nfa->root_state, &l1), num_classes, pdfa, nfa);
    if (!d) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }

#if DUMP_DFA_STATE == 1
    RE_SAL_DPRINT(("\nDFA State representation of %d NFA states:\n", nfa->num_states));
#endif

    add_state_to_dfa(pdfa, d);

    while((d = get_next_unmarked_dstate(pdfa, from++))) {
        get_transition_list(nfa, d, transition_list, &ntr);
        for (i = 0; i < ntr; i++) {
            c = transition_list[i];
            d1 = next_dfa_state_on_transition(d, c, &l1, num_classes, pdfa, nfa);
            if (!d1) {
                rv = REGEX_ERROR_EXPANSION_FAIL;
                goto fail;
            }
            /* Case of memory corruption */
            if (mem_cou >= REGEX_MAX_MEM_USAGE) {  /* 100 MB */
                rv = REGEX_ERROR_EXPANSION_FAIL;
                goto fail;
            }
            /* Timeout is checked when there is change in the mem_cou 
               to reduce the cycles. */ 
            if ((running_divisor) != (mem_cou & 0xf00000)) {
                if (running_divisor == 0) {
                    soc_timeout_init(&timeout, REGEX_MAX_TIMEOUT, 0);
                }
                running_divisor = mem_cou & 0xf00000;
                if (soc_timeout_check(&timeout)) {
                    rv = REGEX_ERROR_EXPANSION_FAIL;
                    goto fail;
                }
            } 
            add_state_to_dfa(pdfa, d1);
            RE_CONNECT_DFA_STATES(d, d1, c);
        }
        RE_DFA_SET_MARKED(d, 1);
    }
    if((from-1) < pdfa->num_states) {
        /* Case of stack corruption */
        rv = REGEX_ERROR_EXPANSION_FAIL;
        goto fail;
    }

    /* update the final states distinctly to differentiate nfa fragments */
    word_size = NFA_SMAP_WORD_SIZE(nfa);
    for (i = 0; i < pdfa->num_states; i++) {
        match_id = -1;
#if DUMP_DFA_STATE == 1
        nc = 0;
#endif
        d = pdfa->states[i];
        for (k = 0; k < word_size; k++) {
            if (d->l.state_map[k] == 0) {
                continue;
            }
            val = d->l.state_map[k];
            for (l = 0; val && (l < 32); l++) {
                if ((d->l.state_map[k] & (1 << l)) == 0) {
                    continue;
                }
                val &= ~(1 << l);
#if DUMP_DFA_STATE == 1
                if (nlist != NULL) {
                nlist[nc++] = (k*32)+l;
                }
#endif
                s = nfa->state_map[(k * 32) + l];
                if ((s->class >= Match) && (match_id == -1)) {
                    RE_DFA_SET_FINAL(d, (s->class - Match + 1));
                    match_id = RE_DFA_FINAL(d);
                }
            }
        }
#if DUMP_DFA_STATE == 1
        if ((match_id != -1) && (nlist != NULL)) {
            RE_SAL_DPRINT(("DFA FINAL State %d represents %d nfa states : matchid %d\n",
                            i, nc, RE_DFA_FINAL(d)));
            for (k = 0; k < nc; k++) {
                RE_SAL_DPRINT((" %d,", nlist[k]));
            }
            RE_SAL_DPRINT(("\n\n"));
        }
#endif
    }

fail:
    /* free up temp nfa stack */
    STACK_DEINIT(&nfa_stk);
    /* free up temp list */
    if (l1.state_map != NULL) {
        sal_free(l1.state_map);
    }
    if (l2.state_map != NULL) {
        sal_free(l2.state_map);
    }
#if DUMP_DFA_STATE == 1
    if (nlist != NULL) {
        sal_free(nlist);
    }
#endif

    if (rv < 0) {
        /* error, free up resources. */
        dfa_free(pdfa);
        pdfa = NULL;
    }
    return rv;
}

#ifdef DFA_MINIMIZE

typedef struct re_inv_dfa_s {
    int *inv_delta;
    int *inv_delta_set;
} re_inv_dfa_t;

static int create_inverted_dfa(re_dfa_t *dfa, re_inv_dfa_t *idfa)
{
    int n, c;
    int lastDelta = 0, *inv_lists, *inv_list_last;
    int s, i, j, t, go_on;
    int rv = REGEX_ERROR_NONE;

    n = dfa->num_states + 1;
    inv_lists = inv_list_last = NULL;

    idfa->inv_delta = sal_alloc(sizeof(int) * n*dfa->num_classes,"inv_delta");
    if(!(idfa->inv_delta)) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
    sal_memset(idfa->inv_delta, 0, sizeof(int) * n*dfa->num_classes);
    idfa->inv_delta_set = sal_alloc(sizeof(int) *2*n*dfa->num_classes,"inv_delta_set");
    if(!(idfa->inv_delta_set)) {
        rv = REGEX_ERROR_NO_MEMORY; 
        goto fail;
    }
    sal_memset(idfa->inv_delta_set, 0, sizeof(int) *2*n*dfa->num_classes);

    lastDelta = 0;
    inv_lists = sal_alloc(sizeof(int)*n,"inv_lists");
    if(!inv_lists) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
    sal_memset(inv_lists, 0, sizeof(int)*n);
    inv_list_last = sal_alloc(sizeof(int)*n,"inv_list_last");
    if(!inv_list_last) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
    sal_memset(inv_list_last, 0, sizeof(int)*n);

#if ENABLE_INVERTED_DFA_DUMP == 1
    RE_SAL_DPRINT(("\nInverted DFA:\n"));
#endif
    for (c = 0; c < dfa->num_classes; c++) {
        for (s = 0; s < n; s++) {
            inv_list_last[s] = -1;
            idfa->inv_delta[(s*dfa->num_classes) + c] = -1;
        }

        idfa->inv_delta[(0*dfa->num_classes)+c] = 0;
        inv_list_last[0] = 0;

        for (s = 1; s < n; s++) {
            t = dfa->states[s-1]->transition_list[c] + 1;

            if (inv_list_last[t] == -1) {
#if ENABLE_INVERTED_DFA_DUMP == 1
                RE_SAL_DPRINT(("  Class %3d, state %3d, transition %3d, inv_delta[%3d]: %3d\n",
                               c, s, t-1, (t*dfa->num_classes)+c, s));
#endif
                idfa->inv_delta[(t*dfa->num_classes) + c] = s;
                inv_list_last[t] = s;
            }
            else {
                inv_lists[inv_list_last[t]] = s;
                inv_list_last[t] = s; 
            }
        }

        for (s = 0; s < n; s++) {
            i = idfa->inv_delta[(s*dfa->num_classes) + c];
            idfa->inv_delta[(s*dfa->num_classes) + c] = lastDelta;
#if ENABLE_INVERTED_DFA_DUMP == 1
                RE_SAL_DPRINT(("  Class %3d, state %3d, inv_delta[%3d]: %3d\n",
                               c, s, (s*dfa->num_classes)+c, lastDelta));
#endif
            j = inv_list_last[s];
            go_on = (i != -1);
            while (go_on) {
                go_on = (i != j);
#if ENABLE_INVERTED_DFA_DUMP == 1
                RE_SAL_DPRINT(("  Class %3d, state %3d, inv_delta_set[%3d]: %3d\n",
                               c, s, lastDelta, i));
#endif
                idfa->inv_delta_set[lastDelta++] = i;
                i = inv_lists[i];
            }
            idfa->inv_delta_set[lastDelta++] = -1;
        }
    }
 
fail:
    if(inv_lists)
        sal_free(inv_lists);
    if(inv_list_last) 
        sal_free(inv_list_last);
	/* idfa freed in calling func*/ 
    return rv;
}

static void free_inverted_dfa(re_inv_dfa_t *idfa)
{
    if (idfa->inv_delta) {
        sal_free(idfa->inv_delta);
        idfa->inv_delta = NULL;
    }
    if (idfa->inv_delta_set) {
        sal_free(idfa->inv_delta_set);
        idfa->inv_delta_set = NULL;
    }
}

typedef struct re_dm_block_cb_s {
    int *block;
    int *b_forward;
    int *b_backward;
    int num_block;
    int b0_off;
    int b_max;
} re_dm_block_cb_t;

static int create_block_list(re_dfa_t *l, re_inv_dfa_t *idfa,
                             re_dm_block_cb_t *bcb)
{
    int n = l->num_states + 1, s, found, t, last, b_i;

    bcb->block = sal_alloc(sizeof(int)*2*n,"blklist");
    if (bcb->block == NULL) {
        goto fail;
    }
    sal_memset(bcb->block, 0, sizeof(int)*2*n);
    bcb->b_forward = sal_alloc(sizeof(int)*2*n,"bbfwd");
    if (bcb->b_forward == NULL) {
        goto fail;
    }
    sal_memset(bcb->b_forward, 0, sizeof(int)*2*n);
    bcb->b_backward = sal_alloc(sizeof(int)*2*n,"bbbwd");
    if (bcb->b_backward == NULL) {
        goto fail;
    }
    sal_memset(bcb->b_backward, 0, sizeof(int)*2*n);

    bcb->num_block = n;
    bcb->b0_off = n; 

    bcb->b_forward[bcb->b0_off]  = 0;
    bcb->b_backward[bcb->b0_off] = 0;          
    bcb->b_forward[0]   = bcb->b0_off;
    bcb->b_backward[0]  = bcb->b0_off;
    bcb->block[0]  = bcb->b0_off;
    bcb->block[bcb->b0_off] = 1;

    for (s = 1; s < n; s++) {
        int b = bcb->b0_off+1;
        found = 0;
        while (!found && b <= bcb->num_block) {
            t = bcb->b_forward[b];

            if (RE_DFA_IS_FINAL(l->states[s-1])) {
                found = RE_DFA_IS_FINAL(l->states[t-1]) &&
                    (RE_DFA_FINAL(l->states[s-1]) == RE_DFA_FINAL(l->states[t-1]));
            }
            else {
                found = !RE_DFA_IS_FINAL(l->states[t-1]);
            }

            if (found) {
                bcb->block[s] = b;
                bcb->block[b]++;

                last = bcb->b_backward[b];
                bcb->b_forward[last] = s;
                bcb->b_forward[s] = b;

                bcb->b_backward[b] = s;
                bcb->b_backward[s] = last;
            }

            b++;
        }

        if (!found) {
            bcb->block[s] = b;
            bcb->block[b]++;

            bcb->b_forward[b] = s;
            bcb->b_forward[s] = b;
            bcb->b_backward[b] = s;
            bcb->b_backward[s] = b;

            bcb->num_block++;
        }
    } 

    bcb->b_max = bcb->b0_off;
    for (b_i = bcb->b0_off+1; b_i <= bcb->num_block; b_i++) {
        if (bcb->block[bcb->b_max] < bcb->block[b_i]) {
            bcb->b_max = b_i;
        }
    }
    return REGEX_ERROR_NONE;
fail:
    /* All free at calling func */
    return REGEX_ERROR_NO_MEMORY; 
}

static void free_dm_block(re_dm_block_cb_t *bcb)
{
    if (bcb->block) {
        sal_free(bcb->block);
        bcb->block = NULL;
    }
    if (bcb->b_forward) {
        sal_free(bcb->b_forward);
        bcb->b_forward = NULL;
    }
    if (bcb->b_backward) {
        sal_free(bcb->b_backward);
        bcb->b_backward = NULL;
    }
}

static re_dfa_t * dfa_minimize(re_dfa_t *dfa_list)
{
    int numStates = dfa_list->num_states, n, c;
    re_dm_block_cb_t bcb;
    int *l_forward, *l_backward, anchorL;
    re_inv_dfa_t idfa;
    int numSplit, *twin, *vsd, *vd, numd;
    int s, i, j, t, last, blk_i;
    int index, indexD, indexTwin, b;
    int *trans, *move, amount, size;
    unsigned int *kill;
    int B_j, a, min_s;

    if (dfa_list->num_states == 0) {
        return dfa_list;
    }

#ifndef SEARCH_FOR_MORE_MATCHES_AFTER_MATCH
    /*
    Eliminate transitions from final states before inverting
    the DFA. (Such states can arise when there are regexes
    with wildcards/ranges.)  This code assumes that only the
    first match is necessary -- there will be no further
    searches for "greedy" regexes/matches.  Once one of the
    regexes is a match, the match ID will be returned.  For
    non-anchored regexes, this can save considerable state
    space. Also, all the transitions from final states are
    not actually installed in the hardware anyway - what
    actually gets installed are jump to idle transitions.
    This code eliminates not only those extra transitions,
    but also results in eliminating any states that become
    non-reachable as a result of removing transitions from
    final states, as those states will then be removed when
    minimizing the DFA.
    */

    for (i = 0; i < numStates; i++) {
        if (RE_DFA_FINAL(dfa_list->states[i])) {
            for (c = 0; c < dfa_list->num_classes; c++) {
                dfa_list->states[i]->transition_list[c] = -1;
            }
    }
    }
#endif
    l_forward = l_backward = twin = vsd = vd = trans = move = NULL;
    kill = NULL;
    n = numStates+1;

    sal_memset(&idfa, 0, sizeof(re_inv_dfa_t));
    sal_memset(&bcb, 0, sizeof(re_dm_block_cb_t));
    if (create_inverted_dfa(dfa_list, &idfa)) {
        goto fail;
    }

    /* create blocks */
    if (create_block_list(dfa_list, &idfa, &bcb)) {
        goto fail;
    }

    l_forward = sal_alloc(sizeof(int)*((n*dfa_list->num_classes)+1),"lfwd");
    if(!l_forward) {
        goto fail;
    }
    sal_memset(l_forward, 0, sizeof(int)*((n*dfa_list->num_classes)+1));
    l_backward = sal_alloc(sizeof(int)*((n*dfa_list->num_classes)+1),"lbwd");
    if(!l_backward) {
        goto fail;
    }
    sal_memset(l_backward, 0, sizeof(int)*((n*dfa_list->num_classes)+1));

    anchorL = n*dfa_list->num_classes;

    l_forward[anchorL] = anchorL;
    l_backward[anchorL] = anchorL;

    blk_i = (bcb.b_max == bcb.b0_off) ? bcb.b0_off+1 : bcb.b0_off;

    index = (blk_i - bcb.b0_off)*dfa_list->num_classes;
    while (index < (blk_i + 1 - bcb.b0_off)*dfa_list->num_classes) {
        last = l_backward[anchorL];
        l_forward[last]     = index;
        l_forward[index]    = anchorL;
        l_backward[index]   = last;
        l_backward[anchorL] = index;
        index++;
    }

    while (blk_i <= bcb.num_block) {
        if (blk_i != bcb.b_max) {
            index = (blk_i - bcb.b0_off)*dfa_list->num_classes;
            while (index < (blk_i + 1 - bcb.b0_off)*dfa_list->num_classes) {
                last = l_backward[anchorL];
                l_forward[last]     = index;
                l_forward[index]    = anchorL;
                l_backward[index]   = last;
                l_backward[anchorL] = index;
                index++;
            }
        }
        blk_i++;
    } 

    twin = sal_alloc(sizeof(int)*2*n,"twin");
    if(!twin){
        goto fail;
    }
    sal_memset(twin, 0, sizeof(int)*2*n);
    vsd = sal_alloc(sizeof(int)*2*n,"vsd");
    if(!vsd) {
        goto fail;
    }
    sal_memset(vsd, 0, sizeof(int)*2*n);
    vd = sal_alloc(sizeof(int)*n,"vd");
    if(!vd) {
        goto fail;
    }
    sal_memset(vd, 0, sizeof(int)*n);

    while (l_forward[anchorL] != anchorL) {
        int B_j_a = l_forward[anchorL];      
        l_forward[anchorL] = l_forward[B_j_a];
        l_backward[l_forward[anchorL]] = anchorL;
        l_forward[B_j_a] = 0;
        B_j = bcb.b0_off + B_j_a / dfa_list->num_classes;
        a   = B_j_a % dfa_list->num_classes;

        numd = 0;
        s = bcb.b_forward[B_j];
        while (s != B_j) {
            t = idfa.inv_delta[(s*dfa_list->num_classes) + a];
            while (idfa.inv_delta_set[t] != -1) {
                vd[numd++] = idfa.inv_delta_set[t++];
            }
            s = bcb.b_forward[s];
        }      

        numSplit = 0;

        for (indexD = 0; indexD < numd; indexD++) {
            s = vd[indexD];
            blk_i = bcb.block[s];
            vsd[blk_i] = -1; 
            twin[blk_i] = 0;
        }

        for (indexD = 0; indexD < numd; indexD++) {
            s = vd[indexD];
            blk_i = bcb.block[s];

            if (vsd[blk_i] < 0) {
                vsd[blk_i] = 0;
                t = bcb.b_forward[blk_i];
                while (t != blk_i && (t != 0 || bcb.block[0] == B_j) && 
                       (t == 0 || bcb.block[dfa_list->states[t-1]->transition_list[a]+1] == B_j)) {
                    vsd[blk_i]++;
                    t = bcb.b_forward[t];
                }
            }
        }

        for (indexD = 0; indexD < numd; indexD++) {
            s = vd[indexD];
            blk_i = bcb.block[s];

            if (vsd[blk_i] != bcb.block[blk_i]) {
                int B_k = twin[blk_i];
                if (B_k == 0) { 
                    B_k = ++bcb.num_block;
                    bcb.b_forward[B_k] = B_k;
                    bcb.b_backward[B_k] = B_k;

                    twin[blk_i] = B_k;

                    twin[numSplit++] = blk_i;
                }

                bcb.b_forward[bcb.b_backward[s]] = bcb.b_forward[s];
                bcb.b_backward[bcb.b_forward[s]] = bcb.b_backward[s];

                last = bcb.b_backward[B_k];
                bcb.b_forward[last] = s;
                bcb.b_forward[s] = B_k;
                bcb.b_backward[s] = last;
                bcb.b_backward[B_k] = s;

                bcb.block[s] = B_k;
                bcb.block[B_k]++;
                bcb.block[blk_i]--;

                vsd[blk_i]--; 
            }
        } 

        for (indexTwin = 0; indexTwin < numSplit; indexTwin++) {
            int blk_i = twin[indexTwin];
            int B_k = twin[blk_i];
            for (c = 0; c < dfa_list->num_classes; c++) {
                int B_i_c = (blk_i-bcb.b0_off)*dfa_list->num_classes+c;
                int B_k_c = (B_k-bcb.b0_off)*dfa_list->num_classes+c;
                if (l_forward[B_i_c] > 0) {
                    last = l_backward[anchorL];
                    l_backward[anchorL] = B_k_c;
                    l_forward[last] = B_k_c;
                    l_backward[B_k_c] = last;
                    l_forward[B_k_c] = anchorL;
                }
                else {
                    if (bcb.block[blk_i] <= bcb.block[B_k]) {
                        last = l_backward[anchorL];
                        l_backward[anchorL] = B_i_c;
                        l_forward[last] = B_i_c;
                        l_backward[B_i_c] = last;
                        l_forward[B_i_c] = anchorL;              
                    }
                    else {
                        last = l_backward[anchorL];
                        l_backward[anchorL] = B_k_c;
                        l_forward[last] = B_k_c;
                        l_backward[B_k_c] = last;
                        l_forward[B_k_c] = anchorL;              
                    }
                }
            }
        }
    }

    free_inverted_dfa(&idfa);
    sal_free(twin);
    sal_free(vsd);
    sal_free(vd);
    twin = vsd = vd = NULL;

    trans = sal_alloc(sizeof(int)*numStates,"trans");
    if(!trans) {
        goto fail;
    }
    sal_memset(trans, 0, sizeof(int)*numStates);

    size = ((numStates+31)/32)*sizeof(unsigned int);
    kill = sal_alloc(size,"kill");
    if(!kill) {
        goto fail;
    }
    sal_memset(kill, 0, size);

    move = sal_alloc(sizeof(int)*numStates,"move");
    if(!move) {
        goto fail;
    }
    sal_memset(move, 0, sizeof(int)*numStates);

    for (b = bcb.b0_off+1; b <= bcb.num_block; b++) {
        s = bcb.b_forward[b];
        min_s = s;
        for (; s != b; s = bcb.b_forward[s]) {
            if (min_s > s) {
                min_s = s;
            }
        }
        min_s--; 
        for (s = bcb.b_forward[b]-1; s != b-1; s = bcb.b_forward[s+1]-1) {
            trans[s] = min_s;
            kill[s/32] |= (s != min_s) ? (1 << (s % 32)) : 0 ;
        }
    }

    free_dm_block(&bcb);
    
    sal_free(l_forward);
    sal_free(l_backward);
    l_forward = l_backward = NULL;

    amount = 0;
    size = ((numStates+31)/32)*sizeof(unsigned int);
    for (i = 0; i < numStates; i++) {
        if (kill[i/32] & (1 << (i%32))) {
            amount++;
        } else {
            move[i] = amount;
        }
    }

    for (i = 0, j = 0; i < numStates; i++) {
        if ((kill[i/32] & (1 << (i % 32))) == 0) {
            for (c = 0; c < dfa_list->num_classes; c++) {
                if ( dfa_list->states[i]->transition_list[c] >= 0 ) {
                    dfa_list->states[j]->transition_list[c]  = trans[ dfa_list->states[i]->transition_list[c] ];
                    dfa_list->states[j]->transition_list[c] -= move[ dfa_list->states[j]->transition_list[c] ];
                }
                else {
                    dfa_list->states[j]->transition_list[c] = dfa_list->states[i]->transition_list[c];
                }
            }

            RE_DFA_SET_FINAL(dfa_list->states[j], RE_DFA_FINAL(dfa_list->states[i]));
            j++;
        }
    }
    numStates = j;

    sal_free(trans);
    sal_free(kill);
    sal_free(move);
    trans = move = NULL;
    kill = NULL;
    /* free up unused states */
    if (j % DFA_BLOCK_SIZE) {
        j += DFA_BLOCK_SIZE - (j % DFA_BLOCK_SIZE);
    }

    /*
    Note, this unusual loop and free is because of the way
    the transition list is allocated in blocks of
    DFA_BLOCK_SIZE.
    */

    for (; j < dfa_list->num_states; j += DFA_BLOCK_SIZE) {
        sal_free(dfa_list->states[j]->transition_list);
        sal_free(dfa_list->states[j]);
    }

    dfa_list->num_states = numStates;
    return dfa_list;

fail:
    free_inverted_dfa(&idfa);
    free_dm_block(&bcb);
    dfa_free(dfa_list);
    if(l_forward) {
        sal_free(l_forward); 
    }
    if(l_backward) {
        sal_free(l_backward); 
    }
    if(twin) {
        sal_free(twin); 
    }
    if(vsd) {
        sal_free(vsd); 
    }
    if(vd) {
        sal_free(vd); 
    }
    if(trans) {
        sal_free(trans); 
    }
    if(move) {
        sal_free(move); 
    }
    if(kill) {
        sal_free(kill); 
    }

    return NULL;
}

#endif /* DFA_MINIMIZE */

#if ENABLE_NFA_DUMP == 1
static void dump_nfa(re_nfa_t *nfa, re_nfa_state_t *root)
{
    int i;
    re_nfa_state_t *s;

    RE_SAL_DPRINT(("Entry state is : %d\n", root->id));
    for (i=0; i<nfa->num_states; i++) {
        s = nfa->state_map[i];
        if (s->class >= Match) {
            RE_SAL_DPRINT(("State [FINAL match %d] %d:\n", (s->class - Match), s->id));
        } else {
            RE_SAL_DPRINT(("State %d:\n", s->id));
        }
        if (s->class == Split) {
            RE_SAL_DPRINT(("\t With epsilon to out  %3d\n", s->out->id));
            RE_SAL_DPRINT(("\t With epsilon to out1 %3d\n", s->out1->id));
        } else {
            if (s->out) {
                RE_SAL_DPRINT(("\tWith input %3d to out  %3d\n", s->class, s->out->id));
            }
            if (s->out1) {
                RE_SAL_DPRINT(("\tWith input %3d to out1 %3d\n", s->class, s->out1->id));
            }
        }
    }
}
#endif

/*
 * Create a single NFA corresponding to all the patterns.
 * The function does the following:
 *      - convert each pattern to class representation
 *      - preprocess the pattern and expand it for example
 *          \d is expanded to (0|1|2|..|8|9) etc.
 *      - convert the pattern to postfix represetation. Note 
 *          the postfix representation is not string but utilizes 2bytes
 *          to represent the character since the pattern might have \0 in
 *          between which would terminate the string.
 *      - postfix representation of the pattern is then converted to NFA.
 *      - All the NFA are joined together using Split (epsilon transition.)
 *      - If all goes well, the final NFA is returned back to caller.
 */
static int make_nfa(char **re, unsigned int *res_flags, 
                    int num_pattern, re_nfa_t **ppnfa,
                    re_transition_class *pclass_array, int num_classes)
{
    re_wc *post;
    re_nfa_state_t **sub_nfa = NULL, *n1, *n2, *s;
    re_nfa_t *pnfa;
    int     pattern;
    re_wc   *re1, *re2;
    int     rv = REGEX_ERROR_NONE, i;

    pnfa = sal_alloc(sizeof(*pnfa), "NFA");
    if(!pnfa) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }
    sal_memset(pnfa, 0, sizeof(*pnfa));

    /* 
     * array to store the individual NFA strands till they are all
     * combined into single final NFA.
     */
    sub_nfa = sal_alloc(sizeof(sub_nfa[0]) * num_pattern, "sub_nfa");
    if (sub_nfa == NULL) {
        rv = REGEX_ERROR_NO_MEMORY;
        goto fail;
    }

    for (pattern=0; pattern < num_pattern; pattern++) {
        re1 = re_convert_tokens_to_class(re[pattern], 
                                         res_flags ? res_flags[pattern] : 0,
                                         pclass_array, num_classes);
        if (!re1) {
            rv = REGEX_ERROR_EXPANSION_FAIL;
            RE_SAL_DPRINT(("\n-------Error %d converting tokens\n\n", rv));
            goto fail;
        }
        re2 = re_preprocess(re1);
        sal_free(re1);
        if (!re2) {
            rv = REGEX_ERROR_EXPANSION_FAIL;
            RE_SAL_DPRINT(("\n-------Error %d re_preprocess\n\n", rv));
            goto fail;
        }

        post = re2post(re2);
        sal_free(re2);
        if (!post) {
            rv = REGEX_ERROR_NO_POST;
            RE_SAL_DPRINT(("\n-------Error %d re2post\n\n", rv));
            goto fail;
        }
        sub_nfa[pattern] = post2nfa(pnfa, post, pattern);
        sal_free(post);
        if (sub_nfa[pattern] == NULL) {
            rv = REGEX_ERROR_EXPANSION_FAIL;
            RE_SAL_DPRINT(("\n-------Error %d post2nfa\n\n", rv));
            goto fail;
        }
#if ENABLE_NFA_DUMP == 1
        RE_SAL_DPRINT(("\n-------NFA for %s\n\n", re[pattern]));
        dump_nfa(pnfa, sub_nfa[pattern]);
#endif
    }

    if (num_pattern == 1) {
        pnfa->root_state = sub_nfa[0];
    } else {
        for (pattern = 0; pattern < num_pattern - 1; pattern++) {
            n1 = sub_nfa[pattern];
            n2 = sub_nfa[pattern+1];
            sub_nfa[pattern+1] = new_nfa_state(pnfa, Split, n1, n2);
        }
        pnfa->root_state = sub_nfa[pattern];
    }

#if ENABLE_NFA_DUMP == 1
    RE_SAL_DPRINT(("--------\n\n\n--- Final NFA ---\n\n"));
    dump_nfa(pnfa, pnfa->root_state);
#endif

    /* 
     * store the byte size required to represent all the states in NFA. 
     * The reason to store it, the information is required in dfa 
     * computation and required like millions of times, this just 
     * optimizes computation a bit.
     */
    pnfa->state_map_byte_size = ((pnfa->num_states + 31) / 32) * sizeof(unsigned int);
    state_map_byte_size = pnfa->state_map_byte_size;

    /*
     * create a class map bitvector. This optimizes traversing the
     * NFA of specified transation.
     */
    for (i = 0; i < num_classes; i++) {
        pnfa->class_map[i] = sal_alloc(NFA_SMAP_BYTE_SIZE(pnfa), "nfa_class_map");
        if(!(pnfa->class_map[i])) {
            rv = REGEX_ERROR_NO_MEMORY;
            goto fail;
        }
        sal_memset(pnfa->class_map[i], 0, NFA_SMAP_BYTE_SIZE(pnfa));
    }

    for (i = 0; i < pnfa->num_states; i++) {
        s = pnfa->state_map[i];
        if (s->class > 255) {
            continue;
        }
        if (s->class >= num_classes) {
            rv = REGEX_ERROR_EXPANSION_FAIL;
            RE_SAL_DPRINT(("\n-------Error %d too many classes\n\n", rv));
            goto fail;
        }
        pnfa->class_map[s->class][s->id / 32] |= (1 << (s->id % 32));
    }

fail:
    /* free sub NFA */
    if (rv) {
        RE_SAL_DPRINT(("\n-------Error %d creating NFA\n\n", rv));
        nfa_free(pnfa);
        pnfa = NULL;
    }
    if(sub_nfa) {
        sal_free(sub_nfa);
    }
    *ppnfa = pnfa;
    return rv;
}

#if ENABLE_FINAL_DFA_DUMP == 1
regex_cb_error_t 
bcm_regex_dfa_dump(unsigned int flags, int match_idx, int in_state, 
                           int from_c, int to_c, int to_state, 
                           int num_dfa_state, void *user_data)
{
    static int last_state = -1;
    static int numFinal = 0;

    if (flags & DFA_TRAVERSE_START) {
        last_state = -1;
        numFinal = 0;
        return REGEX_CB_OK;
    }

    if (flags & DFA_TRAVERSE_END) {
        RE_SAL_DPRINT(("\nNum FINAL states %d\n", numFinal));
        return REGEX_CB_OK;
    }

    /* if last state is not same as this state, insert goto IDLE state */
    if (last_state != in_state) {
        if (flags & DFA_STATE_FINAL) {
            numFinal++;
            RE_SAL_DPRINT(("\nState %d [FINAL]: Match ID %d\n",
                            in_state, match_idx));
        } else {
            RE_SAL_DPRINT(("\nState %d:\n", in_state));
        }
        last_state = in_state;
    }

    if ((from_c == -1) || (to_c == -1)) {
        return 0;
    }

    RE_SAL_DPRINT(("   %4d -> %4d ", in_state, to_state));
    RE_SAL_DPRINT(("["));
    RE_SAL_DPRINT(("\\%-3d", from_c));
    RE_SAL_DPRINT(("-"));
    RE_SAL_DPRINT(("\\%-3d", to_c));
    RE_SAL_DPRINT(("]"));
    if (printable(from_c) && printable(to_c)) {
        RE_SAL_DPRINT((" (%2c-%-2c)", from_c, to_c));
    } else if (printable(from_c)) {
        RE_SAL_DPRINT((" (%2c-'')", from_c));
    } else if (printable(to_c)) {
        RE_SAL_DPRINT((" (''-%-2c)", to_c));
    }
    RE_SAL_DPRINT((";\n"));
    return REGEX_CB_OK;
}
#endif

static int dfa_free(re_dfa_t *l)
{
    int i;

    if (!l) {
        return 0;
    }

    for (i = 0; i < l->num_states; i+= DFA_BLOCK_SIZE) {
        sal_free(l->states[i]->transition_list);
        sal_free(l->states[i]);
    }

    sal_free(l->states);

    if (l->class_array) {
        sal_free(l->class_array);
    }

    _free_buf_blocks(l->nbuf);
    _free_buf_blocks(l->avbuf);
    
    sal_free(l);
    return 0;
}

static int
make_dfa(int num_pattern, re_dfa_t **ppdfa, re_nfa_t **ppnfa, re_transition_class *class_array,
         int num_classes)
{
    re_dfa_t  *pdfa;
    re_nfa_t  *pnfa = *ppnfa;
    int rv = 0;

    pdfa = sal_alloc(sizeof(*pdfa), "DFA");
    sal_memset(pdfa, 0, sizeof(*pdfa));
    pdfa->size = 0;
    pdfa->states = NULL;
    pdfa->num_states = 0;
    pdfa->class_array = class_array;
    pdfa->num_classes = num_classes;

    rv = nfa_to_dfa(pnfa, pdfa, num_classes);
    /* free up NFA, check rv after it */
    nfa_free(pnfa);
    *ppnfa = NULL;
    
    if (rv) {
        /* pdfa freed inside nfa_to_dfa */
        rv = REGEX_ERROR_NO_DFA;
        goto fail;
    }

#if ENABLE_DFA_DUMP_BEFORE_MINIMIZATION == 1
    RE_SAL_DPRINT(("\nTotal of %d DFA states before minimization\n", pdfa->num_states));
    RE_SAL_DPRINT(("\nTotal of %d char classes before minimization\n\n", pdfa->num_classes));
    if (bcm_regex_dfa_traverse(pdfa, bcm_regex_dfa_dump, NULL)) {
        return -1;
    }
    RE_SAL_DPRINT(("-----------------------------------\n\n\n"));
#endif

#if DFA_MINIMIZE == 1
    RE_SAL_DPRINT(("\nMinimizing %d DFA states\n", pdfa->num_states));
    pdfa = dfa_minimize(pdfa);
    if (!pdfa) {
        rv = REGEX_ERROR_NO_DFA;
        goto fail;
    }
    RE_SAL_DPRINT(("-----------------------------------\n\n\n"));

#if ENABLE_FINAL_DFA_DUMP == 1
    RE_SAL_DPRINT(("\nTotal of %d DFA states after minimization\n", pdfa->num_states));
    RE_SAL_DPRINT(("\nTotal of %d char classes after minimization\n\n", pdfa->num_classes));
    RE_SAL_DPRINT(("Minimal DFA is\n"));
    if (bcm_regex_dfa_traverse(pdfa, bcm_regex_dfa_dump, NULL)) {
        return -1;
    }
#endif
#endif
    *ppdfa = pdfa;
    return rv;
fail:
    *ppdfa = NULL;
    
    return rv;
}

static int nfa_free(re_nfa_t *nfa)
{
    int i;

    if (nfa == NULL) {
        return 0;
    }

    for (i = 0; i < COUNTOF(nfa->class_map); i++) {
        if (nfa->class_map[i]) {
            sal_free(nfa->class_map[i]);
            nfa->class_map[i] = NULL;
        }
    }

    if (nfa->state_map) {
        sal_free(nfa->state_map);
        nfa->state_map = NULL;
    }
  
    _free_buf_blocks(nfa->nbuf);
    nfa->nbuf = NULL;
    sal_free(nfa);
    nfa = NULL;
    return 0;
}

/*
 * Compiles the set of patterns into a single DFA. If the ppdfa is
 * not NULL, computed DFA is not freed and is preserved so that
 * user/application can inspect the DFA, ie for the API layer to
 * transform the DFA to device specific HW representation of the DFA.
 *
 * Note: This function is not reentrant.
 */
int
bcm_regex_compile(char **re, unsigned int *res_flags, int num_pattern,
                  unsigned int cflags, void** ppdfa)
{
    int rv = REGEX_ERROR_NONE, num_classes = 0, p, ptlen;
    re_dfa_t *pdfa = NULL;
    re_transition_class *class_array = NULL;
    re_nfa_t *pnfa = NULL;
    char **nre;

    *ppdfa = NULL;

    listid = 0;
    state_id = 0;

    if (num_pattern <= 0) {
        return 0;
    }

    nre = sal_alloc(sizeof(char*)*num_pattern,"tmp_re");
    if (nre == NULL) {
        return REGEX_ERROR_NO_MEMORY;
    }
    sal_memset(nre, 0, sizeof(char*)*num_pattern);
    
    for(p=0; p<num_pattern; p++) {
        if (re[p] == NULL) {
            continue;
        }
        /* If the re begins with the start anchor metacharacter, ignore that character and
         * set the flag which indicates this regex is anchored at the start of the
         * packet. */
        if (START_ANCHOR_CHAR == re[p][0]) {
            if (sal_strlen(re[p]) <= 1) {
                continue;
            }
            nre[p] = sal_strdup(&re[p][1]);
            res_flags[p] |= BCM_TR3_REGEX_CFLAG_ANCHORED;
        } else {
        nre[p] = sal_strdup(re[p]);
        }
        if (NULL == nre[p]) {
            rv = REGEX_ERROR_NO_MEMORY;
            goto fail;
        }
        ptlen = sal_strlen(nre[p]);
        if ((nre[p][ptlen - 1] == '$') && 
            ((ptlen > 1) ? (nre[p][ptlen - 2] != '\\') :  1)) {
            nre[p][ptlen - 1] = '\0';
        }
    }

#ifdef OPTIMIZE_PATTERN
    re_optimize_patterns(nre, num_pattern);
#endif

    rv = re_case_adjust(nre, num_pattern, res_flags);

    /* 
     * make the classes so that we can replace the tokens with the classes.
     * this reduces the number of transitions and arcs and hence 
     * computational complexity.
     */
    rv = re_make_symbol_classes_from_pattern(nre, num_pattern, 
                                             res_flags, &class_array, &num_classes);
    if (rv) {
        rv = REGEX_ERROR_INVALID_CLASS;
        goto fail;
    }

    rv = make_nfa(nre, res_flags, num_pattern, &pnfa, class_array, num_classes);
    if (rv) {
        sal_free(class_array);
        rv = REGEX_ERROR_NO_NFA;
        goto fail;
    }

    if (make_dfa(num_pattern, &pdfa, &pnfa, class_array, num_classes) || !pdfa) {
        rv = REGEX_ERROR_NO_DFA;
        goto fail;
    }

    /* return the number of DFA states */
    rv = pdfa->num_states;

fail:
    for(p=0; p<num_pattern; p++) {
        if (nre[p]) {
            sal_free(nre[p]);
        }
    }
    sal_free(nre);
    if (pnfa) {
        nfa_free(pnfa);
    }
    if ((rv < 0) || (!ppdfa)) {
        if (pdfa) {
            dfa_free(pdfa);
        }
        return rv;
    }
    *ppdfa = (void*)pdfa;

    RE_SAL_DPRINT(("\nCreated DFA with %d states\n", pdfa->num_states));

    return rv;
}

typedef struct _re_state_compress_s {
    int state_id;
    unsigned int transition_map[8];
    unsigned int num_ranges;
    struct _re_state_compress_s *next;
} _re_state_compress;

static _re_state_compress* 
_re_find_compress_state(_re_state_compress **h, int state_id)
{
    /*
    _re_state_compress *ps;
    */
    
    while (*h && ((*h)->state_id != state_id)) {
        h = &(*h)->next;
    }
    return *h;
}

static int
_re_add_tr_to_compress_state(_re_state_compress **h,
                             int to_state_id, unsigned int *bitmap)
{
    _re_state_compress *ps;
    int i;

    ps = _re_find_compress_state(h, to_state_id);
    if (!ps) {
        ps = sal_alloc(sizeof(_re_state_compress),"zipst");
        sal_memset(ps, 0, sizeof(_re_state_compress));
        /* add to list */
        ps->state_id = to_state_id;
        ps->next = *h;
        *h = ps;
    }

    for (i = 0; i < 8; i++) {
        ps->transition_map[i] |= bitmap[i];
    }
    return 0;
}

int bcm_regex_dfa_traverse(void *dfa, regex_dfa_state_cb compile_dfa_cb,
                            void *user_data)
{
    re_dfa_state_t *state;
    re_dfa_t  *dfa_list = (re_dfa_t *)dfa;
    unsigned flags = 0;
    int c, to_state, rv = REGEX_ERROR_NONE;
    int match_idx, i, j, k, lc, has_transition;
    re_transition_class  *class;
    _re_state_compress *dlist[8], *pd, *else_jump_destination=NULL;
    int jump_count;
    int jump_to_all_classes;


    sal_memset(dlist, 0, sizeof(_re_state_compress*)*8);

    /*
     * Dummy callback to indicate start of iteration. Application might do
     * something specific like init or something.
     */
    if (compile_dfa_cb(DFA_TRAVERSE_START, -1, -1, -1, -1, -1, dfa_list->num_states, user_data)) {
        return REGEX_ERROR;
    }

    /*
    Call user provided callback for each DFA state. compress the
    transitions so as to minimize the memory requirements.

    Note that each state will originally have a transition for
    each character class.  During compression, these transitions
    are folded together to make a smaller number of transitions
    that go to the same states.

    The optimization below reduces the number of transitions
    even further.

    Note: class->transition_map really represents a map of the
    different characters that are covered by the classes.

    For the future, the transition compression should be
    changed to be done only once rather than for each
    traversal.
    */
    for (i = 0; i < dfa_list->num_states; i++) {
        state = dfa_list->states[i];
        flags = 0;
        match_idx = -1;
        has_transition = 0;
        jump_to_all_classes = 1;

        for (j = 0; j < dfa_list->num_classes; j++) {
            if (state->transition_list[j] == -1) {
                jump_to_all_classes = 0;
                continue;
            }
            has_transition++;
            c = j;
            to_state = state->transition_list[j];
            class = &dfa_list->class_array[c];

            _re_add_tr_to_compress_state(&dlist[to_state % COUNTOF(dlist)],
                                         to_state, class->transition_map);
        }

        /*
         * dlist now contains one entry for each jump destination.  If there are jumps to
         * every character class, then find the destination with the largest number of
         * ranges (i.e., will have the most number of jump instructions) and replace it
         * with a single jump instruction for all characters (i.e., an else clause).
         */

        else_jump_destination = NULL;
        if (0 != jump_to_all_classes) {
            jump_count = 0;
            for (j = 0; j < COUNTOF(dlist); j++) {
                pd = dlist[j];
                while (pd) {
                    lc = -1;
                    for (k = 0; k < REGEX_MAX_CHARACTER_CLASSES; k++) {
                        if (pd->transition_map[k / WORDS2BITS(1)] & (1 << (k % WORDS2BITS(1)))) {
                            if (lc == -1) {
                                pd->num_ranges++;
                                lc = k;
                            }
                        } else {
                            lc = -1;
                        }
                    }
                    if (pd->num_ranges > jump_count) {
                        jump_count = pd->num_ranges;
                        else_jump_destination = pd;
                    }
                    pd = pd->next;
                }
            }

            /*
             * A destination is found which can save at least one jump, remove it from
             * the list of destinations.
             */
            if (jump_count > 1) {
                for (j = 0; j < COUNTOF(dlist); j++) {
                    if (dlist[j] == else_jump_destination) {
                        dlist[j] = else_jump_destination->next;
                    } else {
                        pd = dlist[j];
                        while (pd) {
                            if (pd->next == else_jump_destination) {
                                pd->next = else_jump_destination->next;
                                break;
                            }
                            pd = pd->next;
                        }
                    }
                }
            } else {
                else_jump_destination = NULL;
            }
        }

        if (RE_DFA_IS_FINAL(state)) {
            match_idx = RE_DFA_FINAL(state) - 1;
            flags = DFA_STATE_FINAL;
            if (!has_transition) {
                if (compile_dfa_cb(DFA_STATE_FINAL, 
                                   match_idx, RE_DFA_STATEID(state),
                                   -1, -1, -1, dfa_list->num_states, user_data)) {
                    rv = REGEX_ERROR;
                    goto done;
                }
            }
        }

        for (j = 0; j < COUNTOF(dlist); j++) {
            while (dlist[j]) {
                pd = dlist[j];
                to_state = pd->state_id;
                lc = -1;
                for (k = 0; k < REGEX_MAX_CHARACTER_CLASSES; k++) {
                    if (pd->transition_map[k / WORDS2BITS(1)] & (1 << (k % WORDS2BITS(1)))) {
                        if (lc == -1) {
                            lc = k;
                        }
                    } else if (lc >= 0) {
                        if (compile_dfa_cb(flags, match_idx, 
                                           RE_DFA_STATEID(state), lc, k-1,
                                           pd->state_id, dfa_list->num_states,
                                       user_data)) {
                            rv = REGEX_ERROR;
                            goto done;
                        }
                        lc = -1;
                    }
                }
                if (lc >= 0)  {
                    if (compile_dfa_cb(flags, match_idx, RE_DFA_STATEID(state),
                                       lc, REGEX_MAX_CHARACTER_CLASSES-1, pd->state_id, dfa_list->num_states, user_data)) {
                        rv = REGEX_ERROR;
                        goto done;
                    }
                }
                dlist[j] = pd->next;
                sal_free(pd);
            }
            dlist[j] = NULL;
        }

        /*
        If an "else" jump destination has been found, process it here.
        */

        if (NULL != else_jump_destination) {
            if (compile_dfa_cb(flags, match_idx, RE_DFA_STATEID(state),
                               0, REGEX_MAX_CHARACTER_CLASSES - 1,
                               else_jump_destination->state_id, dfa_list->num_states, user_data)) {
                rv = REGEX_ERROR;
                goto done;
            }
            sal_free(else_jump_destination);
            else_jump_destination = NULL;
        }
    }

    /* indicate end of traverse, so that any house keeping can be done
     * now */
    compile_dfa_cb(DFA_TRAVERSE_END, -1, -1, -1, -1, -1, dfa_list->num_states, user_data);

done:
    if (NULL != else_jump_destination) {
        sal_free(else_jump_destination);
    }
    for (i = 0; i < COUNTOF(dlist); i++) {
        while(dlist[i]) {
            pd = dlist[i]->next;
            sal_free(dlist[i]);
            dlist[i] = pd;
        }
    }
    return rv;
}

int bcm_regex_dfa_free(void *dfa)
{
    dfa_free((re_dfa_t*)dfa);
    return 0;
}

#ifndef BROADCOM_SDK 

int
main(int argc, char **argv)
{
    int rv, num_pat = 0, i, valid, bi;
    char *pc[64], tmp_re[512], c;
    unsigned int re_flags[512];
    unsigned int def_flags = 0 /* BCM_TR3_REGEX_CFLAG_EXPAND_LCUC */;
    void *dfa;
    FILE *fp;

    fp = fopen("patterns.txt", "r");

    bi = 0;
    while((c = getc(fp)) != EOF) {
        if ((c == '\n') || (c == '\r')) {
            valid = 0;
            i = 0;
            while (i < bi) {
                if ((tmp_re[i] != ' ') && (tmp_re[i] != '\t')) {
                    valid = 1;
                    break;
                }
            }
            
            if (valid) {
                tmp_re[bi] = '\0';
                pc[num_pat] = sal_alloc(512,"main");
                strcpy(pc[num_pat], tmp_re);
                re_flags[num_pat] = def_flags;
                num_pat++;
            }
            bi = 0;
            continue;
        }
        tmp_re[bi++] = c;
    }
    
    rv = bcm_regex_compile(pc, re_flags, num_pat, 0, &dfa);
    if (rv <= 0) {
        RE_SAL_DPRINT(("\n FAILED error=%d, re=%s\n", rv, argv[1]));
    }

    dfa_free(dfa);

    for (i=0; i<num_pat; i++) {
        sal_free(pc[i]);
    }

#if MEM_PROFILE == 1
    re_dump_mem_leak();
#endif

    return 0;
}
#endif


#else
int regex_supported = 0;
#endif
