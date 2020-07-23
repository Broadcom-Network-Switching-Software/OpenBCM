/*! \file bcma_clicmd_expr.c
 *
 * CLI 'expr' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_expr.h>

/*! Stack size for operands */
#define INFIX_ASTK_SIZE         16

/*! Stack size for operators */
#define INFIX_OSTK_SIZE         8

/*! Function prototype for operator calculation */
typedef int (*calculator_t)(int v, int v2);

/*! Structure for expression operator */
typedef struct infix_op_s {

    /*! String of operator */
    char *str;

    /*! TRUE if operator needs single operand */
    int unary;

    /*! Operator proiorty */
    int priority;

    /*! Operator calculator function */
    calculator_t cal_fun;

} infix_op_t;

/*! Structure for infix expression */
typedef struct infix_data_s {

    /*! Stack for operands */
    int astk[INFIX_ASTK_SIZE];

    /*! Stack pointer of operands */
    int *asp;

    /*! Stack for operator */
    infix_op_t *ostk[INFIX_OSTK_SIZE];

    /*! Stack pointer of operators */
    infix_op_t **osp;

} infix_data_t;

/*! Macro for declaring function of operator calculator */
#define DECLARE_OPF(_op, _func) \
    static int \
    op_##_op(int v, int v2) \
    { \
        v = _func; \
        return v; \
    }

DECLARE_OPF(bnot, ~v)

DECLARE_OPF(cnot, !v)

DECLARE_OPF(uplus, v)

DECLARE_OPF(uneg, 0 - v)

DECLARE_OPF(mul, v2 * v)

DECLARE_OPF(div, v2 / v)

DECLARE_OPF(mod, v2 % v)

DECLARE_OPF(add, v2 + v)

DECLARE_OPF(sub, v2 - v)

DECLARE_OPF(lsh, v2 << v)

DECLARE_OPF(rsh, v2 >> v)

DECLARE_OPF(lt, v2 < v)

DECLARE_OPF(gt, v2 > v)

DECLARE_OPF(le, v2 <= v)

DECLARE_OPF(ge, v2 >= v)

DECLARE_OPF(ne, v2 != v)

DECLARE_OPF(eq, v2 == v)

DECLARE_OPF(band, v2 & v)

DECLARE_OPF(bxor, v2 ^ v)

DECLARE_OPF(bor, v2 | v)

DECLARE_OPF(cand, v2 && v)

DECLARE_OPF(cor, v2 || v)

/*! Definition of supported operators */
static infix_op_t infix_ops[] = {
    /* Operators with longer characters are first. */
    { "<<", 0,  4,  op_lsh   },
    { ">>", 0,  4,  op_rsh   },
    { "<=", 0,  5,  op_le    },
    { ">=", 0,  5,  op_ge    },
    { "!=", 0,  6,  op_ne    },
    { "==", 0,  6,  op_eq    },
    { "&&", 0,  10, op_cand  },
    { "||", 0,  11, op_cor   },

    { "(",  0,  0,  NULL     },
    { "~",  1,  1,  op_bnot  },
    { "!",  1,  1,  op_cnot  },
    { "*",  0,  2,  op_mul   },
    { "/",  0,  2,  op_div   },
    { "%",  0,  2,  op_mod   },

    /*
     * The plus sign could be notation of addition and positive number.
     * The order of these ambigous operators can't be changed.
     */
    { "+",  0,  3,  op_add   },
    { "+",  1,  1,  op_uplus },

    /* The minus sign could be notation of subtration and negtive number.
     * The order of these ambigous operators can't be changed.
     */
    { "-",  0,  3,  op_sub   },
    { "-",  1,  1,  op_uneg  },

    { "<",  0,  5,  op_lt    },
    { ">",  0,  5,  op_gt    },
    { "&",  0,  7,  op_band  },
    { "^",  0,  8,  op_bxor  },
    { "|",  0,  9,  op_bor   },
    { ")",  0,  12, NULL     },
    { NULL, -1, -1, NULL     },
};

/*! Push value to operand stack */
static int
infix_push(infix_data_t *id, int v)
{
    if (id->asp == &id->astk[INFIX_ASTK_SIZE]) {
        return -1;
    }

    *id->asp++ = v;

    return 0;
}

/*! Pop value from operand stack */
static int
infix_pop(infix_data_t *id, int *v)
{
    if (id->asp == &id->astk[0]) {
        return -1;
    }

    *v = *--id->asp;

    return 0;
}

/*! Funtion to calculate the result of specified operator */
static int
infix_do_op(infix_data_t *id, infix_op_t *op)
{
    int v, v2 = 0;

    /* No OP for left or right parenthesis */
    if (op->cal_fun == NULL) {
        return 0;
    }

    /* Pop operand for the operator */
    if (infix_pop(id, &v) < 0) {
        return -1;
    }

    /* Pop second operand if the operator is not a unary operator */
    if (!op->unary) {
        if (infix_pop(id, &v2) < 0) {
            return -1;
        }
    }

    if (v == 0 && (op->cal_fun == op_div || op->cal_fun == op_mod)) {
        /* Invalid operation for dividing by zero. */
        return -1;
    }

    /* Get calculation result */
    v = op->cal_fun(v, v2);

    /* Push calculation result to operand stack */
    return infix_push(id, v);
}

/*
 * Get the matching operator from 's' and update 's' if a matched operator
 * is found.
 */
static infix_op_t *
infix_getop(char **s)
{
    int len;
    infix_op_t *op = infix_ops;

    while (op->str != NULL) {
        len = sal_strlen(op->str);
        if (sal_strncmp(*s, op->str, len) == 0) {
            *s = *s + len;
            return op;
        }
        op++;
    }

    return NULL;
}

/* Evaluate the result of 's', where s is infix arithmetic expression */
static int
infix_eval(char *s, int *result)
{
    int op_preceding = TRUE;
    infix_op_t *op;
    infix_data_t infix_data, *id = &infix_data;

    /* Initialize stack pointer of operand and operator */
    id->asp = id->astk;
    id->osp = id->ostk;

    while(*s != '\0') {
        if (sal_isspace(*s)) {
            s++;
        } else if (sal_isdigit(*s)) {
            /* Push operand to stack */
            infix_push(id, sal_ctoi(s, &s));
            op_preceding = FALSE;
        } else {
            /* Check for operator */
            op = infix_getop(&s);
            if (op == NULL) {
                return -1;
            }

            /* Check for ambigous operator */
            if (op->cal_fun == op_sub && op_preceding) {
                /* Check if minus sign represents negtive number but not subtraction */
                op++;
            } else if (op->cal_fun == op_add && op_preceding) {
                /* Check if plus sign represents positive number but not addition */
                op++;
            }

            if (!op->unary) {
                /*
                 * Process previous operands that has higher priority than current one,
                 * except previous operand is left parenthesis.
                 */
                while (id->osp > id->ostk && (*(id->osp - 1))->str[0] != '(' &&
                       (*(id->osp - 1))->priority <= op->priority) {
                    if (infix_do_op(id, *--id->osp) < 0) {
                        return -1;
                    }
                }
            }

            if (id->osp > id->ostk &&
                (*(id->osp - 1))->str[0] == '(' && op->str[0] == ')') {
                /* Remove left and right parenthesis */
                if (infix_do_op(id, *--id->osp) < 0) {
                    return -1;
                }
            } else {
                /* Add current operator to stack. */
                *id->osp++ = op;
                op_preceding = op->str[0] == ')' ? FALSE : TRUE;
            }
        }
    }

    /* Process all operands in stack. */
    while (id->osp > id->ostk) {
        if (infix_do_op(id, *--id->osp) < 0) {
            return -1;
        }
    }

    /* Pop the final result as the return value */
    return infix_pop(id, result);
}

int
bcma_clicmd_expr(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char expr[128];
    char *c;
    int len, result;

    c = BCMA_CLI_ARG_GET(args);
    if (c == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    sal_strncpy(expr, c, sizeof(expr));
    expr[sizeof(expr) - 1] = '\0';

    while ((c = BCMA_CLI_ARG_GET(args)) != NULL) {
        len = sal_strlen(expr);
        sal_strncpy(expr + len, c, sizeof(expr) - len);
        expr[sizeof(expr) - 1] = '\0';
    }

    if (infix_eval(expr, &result) < 0) {
        cli_out("Invalid expression: %s\n", expr);
        return BCMA_CLI_CMD_FAIL;
    }

    return result;
}
