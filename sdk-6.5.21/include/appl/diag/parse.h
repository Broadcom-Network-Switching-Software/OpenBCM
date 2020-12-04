/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        parse.h
 * Purpose:     Defines and types relating to parsing command lines.
 */

#ifndef _DIAG_ARGS_H
#define _DIAG_ARGS_H

#include <sal/types.h>

#define ARGS_CNT        1024            /* Max argv entries */
#define ARGS_BUFFER     8192           /* # bytes total for arguments */

/*
 * Define:      PARSE_VARIABLE_PREFIX1/PREFIX2/SYSTEM
 * Purpose:     Defines the character that indicate a variable reference,
 *              and the character the indicates a system variable.
 */
#define PARSE_VARIABLE_PREFIX1  '@'     /* Deprecated */
#define PARSE_VARIABLE_PREFIX2  '$'
#define PARSE_VARIABLE_SYSTEM   '_'

/*
 * Typedef:     parse_key_t
 * Purpose:     Type used as FIRST entry in structures passed to
 *              parse_lookup.
 */
typedef char *parse_key_t;

/*
 * Define:      PARSE_ENTRIES
 * Purpose:     Determine the number of elements in a parse table.
 */
#define PARSE_ENTRIES(_p)       COUNTOF(_p)

/*
 * Typedef:     args_t
 * Purpose:     argc/argv (sorta) structure for parsing arguments.
 *
 * If the macro ARG_GET is used to consume parameters, unused parameters
 * may be passed to lower levels by simply passing the args_t struct.
 */
typedef struct args_s {
    parse_key_t a_cmd;                  /* Initial string */
    char        *a_argv[ARGS_CNT];      /* argv pointers */
    char        a_buffer[ARGS_BUFFER];  /* Split up buffer */
    int         a_argc;                 /* Parsed arg counter */
    int         a_arg;                  /* Pointer to NEXT arg */
} args_t;

#define ARG_CMD(_a)     (_a)->a_cmd

#define _ARG_CUR(_a)     \
    ((_a)->a_argv[(_a)->a_arg])

#define ARG_CUR(_a)     \
    (((_a)->a_arg >= (_a)->a_argc) ? NULL : _ARG_CUR(_a))

#define _ARG_GET(_a)     \
    ((_a)->a_argv[(_a)->a_arg++])

#define ARG_GET(_a)     \
    (((_a)->a_arg >= (_a)->a_argc) ? NULL : _ARG_GET(_a))

#define ARG_NEXT(_a)    (_a)->a_arg++
#define ARG_PREV(_a)    (_a)->a_arg--
#define ARG_CUR_INDEX(_a)   ((_a)->a_arg)

#define ARG_DISCARD(_a) \
    ((_a)->a_arg = (_a)->a_argc)

#define ARG_GET_WITH_INDEX(_a, id)     \
    ((_a)->a_argv[id])
/*
 * Macro:       ARG_CNT
 * Purpose:     Return the number of unconsumed arguments.
 */
#define ARG_CNT(_a)     ((_a)->a_argc - (_a)->a_arg)

/* 
 * Typedef:     parse_pm_t
 * Purpose:     Defines a "mask" type for parsing the common
 *              "+foo -bar" type arguments.
 */
typedef struct parse_pm_s {
    parse_key_t pm_s;                   /* string to match */
    uint32      pm_value;               /* Value associated with string */
} parse_pm_t;

/*
 * Typedef:     parse_eq_t
 * Purpose:     Generic type for parsing a series of options of the 
 *              form "abc=X".
 *
 * Notes: 
 *      Entries of type PQ_INT/PQ_HEX cause the value to be stored
 *      in *pq_value as type "int". If an entry is marked as PQ_HEX, the
 *      only difference is the value is displayed as a hex #.
 *
 *      Entries of type PQ_STRING cause a malloc'd string pointer to be
 *      stored in *pq_value as type "char *".
 *
 *      Entries of type PQ_BOOL cause a boolean value of 1 or 0 to be 
 *      stored in *pq_value as type "int".
 *
 *      Entries of type PQ_MAC cause a 6-byte raw mac address to be
 *      stored at *pq_value.
 *
 *      Entries of type PQ_IP cause a 4-byte raw IP address to be stored
 *      in *pq_value as type ip_addr_t.
 *
 *      Entries of type PQ_PBMP cause a N-word integer value to be stored
 *      in *pq_value as type "pbmp_t".
 *
 *      Entries of type PQ_PORTMODE cause an integer value to be stored
 *      in *pq_value as type soc_port_mode_t.
 *
 *      Entries of type PQ_PORT cause an integer value to be stored
 *      in *pq_value as type soc_port_t.
 *
 *      Entries of type PQ_MOD_PORT cause an integer values to be stored
 *      in *pq_value as type bcm_mod_port_t.
 *
 *      Entries with PQ_DFL set use the pq_value instead of the pq_default
 *      field.
 *
 *      Entries with PQ_MULTI set assume XXXX is a pointer to a NULL
 *      terminated array of strings that are possible matches.
 *
 *      Fields in this structure are modified by parse_arg_eq, and 
 *      parse_arg_eq_done. After processing is complete on the parsed
 *      arguments, parse_arg_eq_done should be called to free any
 *      malloc'd buffers, and possibly clean up internal state.
 */
typedef struct parse_eq_s {
    parse_key_t pq_s;                   /* String to match */
#   define      PQ_BCM          0x40000 /* Parse as BCM API port */
#   define      PQ_STATIC       0x20000 /* do not deallocate string;
                                           overrides PQ_MALLOC */
#   define      PQ_LAB          0x10000 /* warn if used (lab compatibility) */
#   define      PQ_IGNORE       0x8000  /* Dynamically set if required */
#   define      PQ_NO_EQ_OPT    0x4000  /* May or may not take = */
#   define      PQ_SEEN         0x2000  /* Was string seen w/o =? */
#   define      PQ_PARSED       0x1000  /* Set if seen during parse call */
#   define      PQ_DFL          0x800   /* Use VALUE for default */
#   define      PQ_PTR          0x400   /* PTR type ARG */
#   define      PQ_MALLOC       0x100   /* Value is malloc'd */
#   define      PQ_VAL          0x000   /* VALUE type ARG */
#   define      PQ_INT          (PQ_VAL|0x01)   /* Normal integer */
#   define      PQ_HEX          (PQ_VAL|0x02)   /* Hex integer (like int) */
#   define      PQ_BOOL         (PQ_VAL|0x03)   /* Yes/No */
#   define      PQ_STRING       (PQ_VAL|0x04)   /* String value */
#   define      PQ_MAC          (PQ_PTR|0x05)   /* MAC Address */
#   define      PQ_IP           (PQ_VAL|0x06)   /* IP Address */
#   define      PQ_PBMP         (PQ_PTR|0x07)   /* Port bitmap */
#   define      PQ_MULTI        (PQ_VAL|0x08)   /* Multiple choice */
#   define      PQ_PORTMODE     (PQ_VAL|0x09)   /* Port mode */
#   define      PQ_PORT         (PQ_VAL|0x0a)   /* Port */
#   define      PQ_MOD_PORT     (PQ_PTR|0x0b)   /* Port, mod id pair */
#   define      PQ_IP6          (PQ_PTR|0x0c)   /* IPv6 Address */
#   define      PQ_INT64        (PQ_PTR|0x0d)   /* 64 bit integer */
#   define      PQ_LR_PHYAB     (PQ_VAL|0x0e)   /* Longreach PHY ability */
#   define      PQ_PORTABIL     (PQ_PTR|0x0f)   /* port ability */
#   define      PQ_INT8         (PQ_VAL|0x10)   /* 8bit */
#   define      PQ_INT16        (PQ_VAL|0x20)   /* 16bit */
#   define      PQ_TYPE(_x)     ((_x) & (0xff|PQ_PTR|PQ_VAL))
    uint32      pq_unit;                /* Unit number */
    uint32      pq_type;                /* Type after '=' */
    const void  *pq_default;            /* Default Value */
    void        *pq_value;              /* Value parsed */
    char        **pq_fm;                /* Multi */
} parse_eq_t;

typedef struct parse_table_s {
    int         pt_unit;                /* unit number */
    int         pt_cnt;                 /* # valid entries */
    int         pt_alloc;               /* number of allocated entries */
#   define      PT_ALLOC        32      /* entries to allocate */
    parse_eq_t  *pt_entries;            /* Actual entries */
    int         cmd_flag;               /* The on-going config cmd */
#   define      CMD_DC          0       /* config cmd is don't care */
#   define      CMD_ADD         1       /* config add cmd */
} parse_table_t;

/* parse.c exported routines */

extern  char    *(*parse_user_var_get)(char *varname);

extern  void    parse_args_copy(args_t *dst, args_t *src);
extern  int     parse_cmp(const char *p, const char *s, const char term);
extern  int     diag_parse_args(const char *s, char **s_ret, args_t *);
extern  int     parse_mask(const char *s, const parse_pm_t *, uint32 *mask);
extern  void    parse_mask_format(const int, const parse_pm_t *,uint32 mask);
extern  const void *parse_lookup(const char *, const void *tbl, int size, 
                                 int cnt);
extern  int     parse_arg_eq(args_t *, parse_table_t *);
extern  int     parse_arg_eq_keep_index(args_t *a, parse_table_t *pq_table);
extern  void    parse_arg_eq_done(parse_table_t *);
extern  void    parse_eq_format(parse_table_t *);
extern  int     parse_default_fill(parse_table_t *pq_table);
extern int      parse_table_add(parse_table_t *, char *key, uint32 type, 
                                void *def, void *value, void *func);
extern void     parse_table_init(int unit, parse_table_t *);

/* variable.c exported routines */

extern  void    *var_push_scope(void);
extern  void    var_pop_scope(void *);
extern  char    *var_get(const char *);
extern  int     var_set(const char *v_name, char *v_value, int local, 
                        int system);
extern  int     var_set_integer(const char *v_name, int v_value, int local, 
                                int system);
extern  int     var_set_hex(const char *v_name, int v_value, int local, 
                                int system);
extern  int     var_unset(const char *v_name, int local, int global,  
                          int system);

#endif /* _DIAG_ARGS_H */
