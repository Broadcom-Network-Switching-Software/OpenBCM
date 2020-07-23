/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * 
 * $Copyright: $
 */
#include <appl/diag/infix.h>

#include <assert.h>
#include <sal/core/libc.h>
#include <appl/diag/system.h>

typedef struct infix_data_s {
    INFIX_TYPE        astk[INFIX_ASTK_SIZE], *asp;
    int            ostk[INFIX_OSTK_SIZE], *osp;
    int            err;
} infix_data_t;

typedef struct infix_op_s {
    char        *str;
    int            op;
} infix_op_t;

#define OP_LPAR        '('
#define OP_RPAR        ')'
#define OP_BNOT        '~'
#define OP_CNOT        'N'
#define OP_UNEG        'U'
#define OP_UPLUS    'P'
#define OP_MUL        '*'
#define OP_DIV        '/'
#define OP_MOD        '%'
#define OP_ADD        '+'
#define OP_SUB        '-'
#define OP_LSH        'l'    /* << */
#define OP_RSH        'r'    /* >> */
#define OP_LT        '<'    /* < */
#define OP_LE        '['    /* <= */
#define OP_GT        '>'    /* > */
#define OP_GE        ']'    /* >= */
#define OP_EQ        '='    /* == */
#define OP_NE        '!'    /* != */
#define OP_BAND        '&'
#define OP_BOR        '|'
#define OP_BXOR        '^'
#define OP_CAND        'A'    /* && */
#define OP_COR        'O'    /* || */
#define OP_ERR        'x'

static infix_op_t infix_ops[] = {
    /* Two-char operators first */
    {    "<<",        OP_LSH        },
    {    ">>",        OP_RSH        },
    {    "<=",        OP_LE        },
    {    ">=",        OP_GE        },
    {    "!=",        OP_NE        },
    {    "==",        OP_EQ        },
    {    "&&",        OP_CAND        },
    {    "||",        OP_COR        },
    {    "(",        OP_LPAR        },
    {    ")",        OP_RPAR        },
    {    "~",        OP_BNOT        },
    {    "!",        OP_CNOT        },
    {    "*",        OP_MUL        },
    {    "/",        OP_DIV        },
    {    "%",        OP_MOD        },
    {    "+",        OP_ADD        },
    {    "-",        OP_SUB        },
    {    "&",        OP_BAND        },
    {    "|",        OP_BOR        },
    {    "^",        OP_BXOR        },
    {    "<",        OP_LT        },
    {    ">",        OP_GT        },
    {    NULL,        0        },
};

static int infix_pri(infix_data_t *id, int op)
{
    switch (op) {
    case OP_LPAR:            return 0;
    case OP_BNOT:   case OP_CNOT:
    case OP_UNEG:   case OP_UPLUS:    return 1;
    case OP_MUL:    case OP_DIV:
    case OP_MOD:            return 2;
    case OP_ADD:    case OP_SUB:    return 3;
    case OP_LSH:    case OP_RSH:    return 4;
    case OP_LT:     case OP_LE:
    case OP_GT:     case OP_GE:        return 5;
    case OP_EQ:     case OP_NE:        return 6;
    case OP_BAND:            return 7;
    case OP_BXOR:            return 8;
    case OP_BOR:            return 9;
    case OP_CAND:            return 10;
    case OP_COR:            return 11;
    case OP_RPAR:            return 12;
    }

    id->err = 1;

    return 0;
}

static void infix_push(infix_data_t *id, INFIX_TYPE v)
{
    if (id->asp == &id->astk[INFIX_ASTK_SIZE])
        { id->err = -1; }
    else
        { *id->asp++ = v; }
}

static INFIX_TYPE infix_pop(infix_data_t *id)
{
    if (id->asp == &id->astk[0]) {
    id->err = -1;
    return 0;
    } else {
    --id->asp;
    return *id->asp;
    }
}

static void infix_doop(infix_data_t *id, int op)
{
    INFIX_TYPE v;

    if (op == OP_LPAR || op == OP_RPAR)
        { return; }

    v = infix_pop(id);

    switch (op) {
    case OP_BNOT:    v = ~v;                break;
    case OP_UNEG:    v = 0 - v;            break;
    case OP_CNOT:    v = ! v;            break;
    case OP_MUL:    v = infix_pop(id) * v;        break;
    case OP_DIV:    if (v == 0) { id->err = 1; }
                else { v = infix_pop(id) / v; }  break;
    case OP_MOD:    if (v == 0) { id->err = 1; }
                else { v = infix_pop(id) % v; }  break;
    case OP_ADD:    v = infix_pop(id) + v;        break;
    case OP_SUB:    v = infix_pop(id) - v;        break;
    case OP_LSH:    v = infix_pop(id) << v;        break;
    case OP_RSH:    v = infix_pop(id) >> v;        break;
    case OP_BAND:    v = infix_pop(id) & v;        break;
    case OP_BXOR:    v = infix_pop(id) ^ v;        break;
    case OP_BOR:    v = infix_pop(id) | v;        break;
    case OP_LT:        v = infix_pop(id) < v;        break;
    case OP_GT:        v = infix_pop(id) > v;        break;
    case OP_LE:        v = infix_pop(id) <= v;        break;
    case OP_GE:        v = infix_pop(id) >= v;        break;
    case OP_NE:        v = infix_pop(id) != v;        break;
    case OP_EQ:        v = infix_pop(id) == v;        break;
    case OP_CAND:    v = infix_pop(id) && v;        break;
    case OP_COR:    v = infix_pop(id) || v;        break;
    }

    infix_push(id, v);
}

int infix_getop(char **s)
{
    int            i;

    for (i = 0; infix_ops[i].str != NULL; i++) {
    if (infix_ops[i].str[1] == 0) {
        if (infix_ops[i].str[0] == (*s)[0]) {
        (*s)++;
        return infix_ops[i].op;
        }
    } else if (infix_ops[i].str[0] == (*s)[0] &&
           infix_ops[i].str[1] == (*s)[1]) {
        (*s) += 2;
        return infix_ops[i].op;
    }
    }

    return OP_ERR;
}

int infix_eval(char *s, INFIX_TYPE *v)
{
    int            i, p, prev_was_op = 1;
    infix_data_t    id;

    id.asp = &id.astk[0];
    id.osp = &id.ostk[0];
    id.err = 0;

    while ((i = *s) != 0) {
    if (isspace(i))
        { s++; }
    else if (isdigit(i)) {
        infix_push(&id, sal_ctoi(s, &s));
        prev_was_op = 0;
    } else {
        i = infix_getop(&s);
        if (i == OP_ERR) {
        id.err = 1;
        break;
        }
        if (i == OP_SUB && prev_was_op)
            { i = OP_UNEG; }
        else if (i == OP_ADD && prev_was_op)
        i = OP_UPLUS;
        else if ((i == OP_CNOT || i == OP_BNOT) && prev_was_op) {
        /* skip */
        } else {
        p = infix_pri(&id, i);
        while (id.osp > id.ostk && infix_pri(&id, id.osp[-1]) <= p)
            infix_doop(&id, *--id.osp);
        }
        *id.osp++ = i;
        prev_was_op = (i != OP_RPAR);
    }
    }

    while (id.osp > id.ostk)
    infix_doop(&id, *--id.osp);

    *v = infix_pop(&id);

    if (id.asp != id.astk)
        { id.err = -1; }

    return id.err;
}
