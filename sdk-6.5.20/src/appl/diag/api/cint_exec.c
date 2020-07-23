/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_exec.c
 * Purpose:     API mode CINT execution interface
 */

#include "bcm/port.h"
#include "sal/types.h"
#include "sal/core/alloc.h"
#include "sal/core/libc.h"
#include "sal/appl/io.h"
#include "shared/util.h"
#include <soc/drv.h>
#include "appl/diag/system.h"
#include "appl/diag/shell.h"
#include "cint_porting.h"

/* The following declarations allows only the %token identifiers to be
   included, and not any of the other Bison types. This facilitates
   including multiple Bison interpreter headers, provided that the
   individual token types don't conflict. */
#define YYSTYPE_IS_DECLARED
#define YYLTYPE_IS_DECLARED
#define YYPUSH_DECLS

#include "cint_interpreter.h"
#include "cint_porting.h"
#include "cint_internal.h"

#include "context.h"
#include "api_grammar.tab.h"
#include "api_mode.h"

typedef enum prompt_assign_e {
    prompt_assign_none,
    prompt_assign_error,
    prompt_assign_create_tmp,
    prompt_assign_output_var,
    prompt_assign_output_var_c,
    prompt_assign_output_var_r,
    prompt_assign_output_tmp,
    prompt_assign_bidir_var_c,
    prompt_assign_bidir_tmp,
    prompt_assign_input_constant,
    prompt_assign_input_var,
    prompt_assign_input_tmp
} prompt_assign_t;

/* Note that the cint_parameter_desc_t have to be structures,
   not references because of how CINT typedef resolution works */

/* prompt_arg_t flags */
#define PA_LAST       0x00000001
#define PA_EXPAND     0x00000002

/*
  CINT function arguments are fully and recursively expanded to
  facilitate prompt assignment. This becomes the assignment structure
  used to create the resulting CINT program. The function arguments
  are represented as 'level 0' structures, and any aggregrate
  (structure and array) arguments are expanded as level 1 and
  higher. Aggregates inherit the directionality of the parent
  arguments.
 */
typedef struct prompt_arg_s {
    prompt_assign_t assign_op;          /* assignment operation */
    cint_parameter_desc_t desc;         /* CINT parameter desc */
    cint_datatype_t dt;                 /* CINT datatype */
    const char *path;                   /* variable components */
    const char *value;                  /* assigned value */
    int idx;                            /* array index if array member */
    int level;                          /* arg level;0=toplevel */
    int pos;                            /* positional index */
    uint32 p_flags;                     /* PA flags */
    uint32 c_flags;                     /* CINT flags */
    struct prompt_arg_s *next;          /* next in list */
    struct prompt_arg_s *prev;          /* prev in list */
} prompt_arg_t;

/* subset of prompt_arg_t used for construction */
typedef struct desc_data_s {
    uint32 p_flags;                       /* PA flags */
    uint32 c_flags;                       /* CINT flags */
    int idx;
    int level;
    int pos;
} desc_data_t;

/* track assignment context */
typedef enum prompt_ctx_e {
    prompt_ctx_unknown, /* unknown assignment context */
    prompt_ctx_outer,   /* outermost level */
    prompt_ctx_kv,      /* keyword - value */
    prompt_ctx_pos      /* positional */
} prompt_ctx_t;

/* root prompt arg struct */
typedef struct {
    prompt_arg_t *head;         /* head of API argument list */
    prompt_arg_t *tail;         /* tail of API argument list */
    prompt_arg_t *search;       /* current search pointer */
    prompt_arg_t *assign;       /* current assign pointer */
    prompt_ctx_t context;       /* current assignment context */
} prompt_t;

/* temporary staging buffer size */
#ifndef API_MODE_STAGE_SIZE
#define API_MODE_STAGE_SIZE 256
#endif

/* prompt mode prompt string buffer */
#ifndef API_MODE_PROMPT_SIZE
#define API_MODE_PROMPT_SIZE 256
#endif

/* buffer to construct CINT program */
#ifndef API_MODE_PROGRAM_SIZE
#define API_MODE_PROGRAM_SIZE 16*1024
#endif

/* CINT program indent level */
#ifndef API_MODE_INDENT
#define API_MODE_INDENT "  "
#endif

/* global data used for program construction */
typedef struct {
    int unit;                   /* default unit */
    int debug;                  /* true if tracing requested */
    int rv;                     /* internal return value */
    char *prompt;               /* program construction buffers */
    char *stage;
    char *program;
} api_mode_data_t;

typedef int (*pfunc_t)(api_mode_data_t *data, prompt_arg_t *p_arg);
typedef int (*pfunc_s_t)(api_mode_data_t *data, prompt_arg_t *p_arg,
                          const char *s);
typedef int (*paf)(api_mode_data_t *data, prompt_arg_t *dst);

typedef struct prompt_arg_helper_s {
    const char *key;
    pfunc_t help;
    pfunc_s_t convert;
    pfunc_t format;
} prompt_arg_helper_t;

typedef struct prompt_arg_type_inference_s {
    const char *basetype_out;
    const char *basetype_in;
    const char *name;
} prompt_arg_type_inference_t;

typedef enum match_op_e {
    OP_NONE,
    OP_FPROMPT,
    OP_VPROMPT,
    OP_HELP,
    OP_KWARG,
    OP_PRINT,
    OP_VAR,
    OP_CREATE
} match_op_t;


/* run a CINT program */
STATIC int
cmd_api_run(api_mode_data_t *data)
{
    cint_ast_t* ast;
    int rv = API_MODE_E_NONE;

    if (data->program) {
        if (data->debug) {
            sal_printf("Program: \n%s\n", data->program);
            sal_printf("Result:\n");
        }
        ast = cint_interpreter_parse_string(data->program);
        if (ast) {
            rv = cint_interpreter_evaluate(ast);
        }
    }

    return rv;
}

/* Return a string indicating parameter direction */
STATIC char *
param_dir_str(uint32 flags)
{
    char *s = "?";
    if ((flags & CINT_PARAM_INOUT) == CINT_PARAM_INOUT) {
        s = "IO";
    } else if (flags & CINT_PARAM_IN) {
        s = "I";
    } else if (flags & CINT_PARAM_OUT) {
        s = "O";
    } else if (flags == 0) {
        s = "";
    } else {
        s = "!";
    }
    return s;
}


/*
  Try to match identifier arguments in arg to a CINT function. Return
  TRUE if there is a match. 'out' is set to the first non-identfier
  argument from 'in', and 'dt' is the CINT function datatype.

  'buffer' contains the matched function name.

  If help, prompt-assignment, or keyword/value arguments, then only the
  identifiers are matched.  If positional arguments, then the arity must
  either match or be one greater if the first argument name is "unit".

 */



STATIC int
match_command(api_mode_arg_t *in, api_mode_arg_t **out,
              const cint_datatype_t **dt, int *op_p)
{
    api_mode_arg_t *a, *argp;
    match_op_t op = OP_NONE;
    int rv = API_MODE_E_FAIL;
    int has_arg = 0;
    argp = NULL;

    for (a=in; a; a=a->next) {
        switch (a->kind) {
        case IDENT:
            break;
        case HELP:
            op = OP_HELP;
            break;
        case PRINT:
            op = OP_PRINT;
            has_arg = 1;
            break;
        case VAR:
            op = OP_VAR;
            has_arg = 1;
            break;
        case CREATE:
            op = OP_CREATE;
            has_arg = 1;
            break;
        case PROMPT:
            op = (op == OP_VAR) ? OP_VPROMPT : OP_FPROMPT;
            break;
        case KEY_VALUE:
            op = OP_KWARG;
            has_arg = 1;
            break;
        default:
            has_arg = 1;
            break;
        }
        if (has_arg && !argp) {
            argp = a;
        }

    }

    switch (op) {
    case OP_NONE:
    case OP_HELP:
    case OP_FPROMPT:
    case OP_KWARG:
        if (in->dt) {
            *dt = in->dt;
            rv = API_MODE_E_NONE;
        } else {
            rv = API_MODE_E_NOT_FOUND;
        }
        break;
    case OP_PRINT:
    case OP_VAR:
    case OP_CREATE:
    case OP_VPROMPT:
        rv = has_arg ? API_MODE_E_NONE : API_MODE_E_FAIL;
        break;
    default:
        rv = API_MODE_E_NOT_FOUND; /* or fail? */
        break;
    }
    
    *out = argp;
    *op_p = op;

    return rv;
}

/* show partial help for a CINT datatype */
STATIC int
_partial_help(void* cookie, const cint_datatype_t* dt)
{
    char *str = (char *)cookie;
    int len = sal_strlen(str);
    if (!strncmp(str, dt->desc.basetype, len)) {
        sal_printf("%s\n", dt->desc.basetype);
    }

    return API_MODE_E_NONE;
}

/* partial help for a set of functions */
STATIC int
api_mode_partial_help(api_mode_arg_t *in)
{
    if (in->dt) {
        const char *name = in->dt->desc.basetype;
        sal_printf("Functions beginning with %s:\n", name);
        cint_datatype_traverse(CINT_DATATYPE_FLAGS_FUNC,
                               _partial_help, (void *)name);
    } else {
        return API_MODE_E_UNAVAIL;
    }
    return API_MODE_E_NONE;
}



typedef int (*_ccb_d_t_f)(const cint_parameter_desc_t *desc,
                          const cint_parameter_desc_t *parent,
                          const cint_datatype_t *dt,
                          int level,
                          void *data);

/* CINT parameter desc traversal */
int
_ccb_desc_traverse(const cint_parameter_desc_t *desc,
                   const cint_parameter_desc_t *parent,
                   _ccb_d_t_f func,
                   int level,
                   void *data)
{
    int rv;
    cint_datatype_t dt;

    if (!desc->basetype) {
        return API_MODE_E_NONE;
    }

    rv = cint_datatype_find(desc->basetype, &dt);
    rv = func(desc, parent, (rv == CINT_E_NONE) ? &dt : NULL, level, data);
    if (API_MODE_SUCCESS(rv)) {
        if (dt.flags & CINT_DATATYPE_F_STRUCT) {
            const cint_parameter_desc_t *m;
            for (m=dt.basetype.sp->struct_members; m->basetype; m++) {
                if (_ccb_desc_traverse(m, desc, func, level+1, data) < 0) {
                    break;
                }
            }
        }
        if (dt.flags & CINT_DATATYPE_F_TYPEDEF) {
            if ((desc->name != NULL)) {
                _ccb_desc_traverse(&dt.desc, desc, func, level+1, data);
            }
        }
    }

    return rv;
}

/* show datatype information */
STATIC int
_ccb_show_callback(const cint_parameter_desc_t *desc,
                   const cint_parameter_desc_t *parent,
                   const cint_datatype_t *dt,
                   int level,
                   void *user_data)
{
    int i;
    int indent = level*2;

    for (i=0; i<indent; i++) {
        sal_printf("  ");
    }
    if (indent > 5) {
        sal_printf("?depth\n");
        return API_MODE_E_FAIL;
    }
    if (dt->flags & CINT_DATATYPE_F_TYPEDEF) {
        sal_printf("typedef ");
    }
    if (dt->flags & CINT_DATATYPE_F_STRUCT) {
        sal_printf("struct ");
    }
    if (dt->flags & CINT_DATATYPE_F_ENUM) {
        sal_printf("enum ");
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC) {
        sal_printf("function ");
    }
    if (dt->flags & CINT_DATATYPE_F_CONSTANT) {
        sal_printf("constant ");
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC_DYNAMIC) {
        sal_printf("dynamic function ");
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC_POINTER) {
        sal_printf("function pointer ");
    }
    if (dt->flags & CINT_DATATYPE_F_ITERATOR) {
        sal_printf("iterator ");
    }
    if (dt->flags & CINT_DATATYPE_F_MACRO) {
        sal_printf("macro ");
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC_VARARG) {
        sal_printf("variadic function ");
    }
    sal_printf("%s ", desc->basetype);
    for (i=0; i<desc->pcount; i++) {
        sal_printf("*");
    }

    sal_printf("%s", desc->name ? desc->name : "");

    if (desc->array > 0) {
        sal_printf("[%d]", desc->array);
    }
    sal_printf(";");

    sal_printf("\n");
    return API_MODE_E_NONE;
}

/* CINT parameter desc traversal */
STATIC void
_ccb_show_desc(const cint_parameter_desc_t *desc)
{
    _ccb_desc_traverse(desc, NULL, _ccb_show_callback, 0, NULL);
}

/* help string for CINT parameter desc */
STATIC void
_help_arg(const cint_parameter_desc_t *desc)
{
    int i;

    sal_printf("{%s} ", param_dir_str(desc->flags));
    sal_printf("%s ", desc->basetype);
    for (i=0; i<desc->pcount; i++) {
        sal_printf("*");
    }

    sal_printf("%s", desc->name ? desc->name : "");

    if (desc->array > 0) {
        sal_printf("[%d]", desc->array);
    }
}

/* function and structure help */
STATIC int
api_mode_help(const cint_datatype_t *dt)
{
    if (dt->flags & CINT_DATATYPE_FLAGS_FUNC) {
        const cint_parameter_desc_t *param;

        param = dt->basetype.fp->params;
        sal_printf("%s %s(", param->basetype, dt->desc.basetype);
        param++;
        while (param->basetype != NULL) {
            _help_arg(param);
            if (param[1].basetype) {
                sal_printf(", ");
            }
            param++;
        }
        sal_printf(")");
    } else if (dt->flags & CINT_DATATYPE_F_STRUCT) {
        const cint_parameter_desc_t *m;
        sal_printf("typedef struct %s {\n", dt->desc.basetype);
        for (m=dt->basetype.sp->struct_members; m->basetype; m++) {
            sal_printf("  %s %s;\n", m->basetype, m->name);
        }
        sal_printf("}\n");
    } else {
        _ccb_show_desc(&dt->desc);
    }

    sal_printf("\n");
    return API_MODE_E_NONE;
}

/*
  prompt_assign operators control how the CINT program is constructed based
  on the API mode function argument assignments.

  Note that the global and bidir variables are created as a convenience. It
  is normally expected that bidir variables already exist.
  
  An API mode command is translated to a CINT program with the
  following structure:

  // Create output and bidir global variables
  {
    // Create needed input, output, and bidir temporary variables
    // Assign input constants
    // Call CINT function
    // Display return value
    // Display output variables
  }

  prompt_assign_none

    Assignments were not made to this argument. If this a top level
    function parameter, create a temporary variable.  If the function
    argument is treated as an input and if a constructor exists, call
    the constructor. The temporary variable is passed to the function,
    and, if it is an output, schedule for printing. op changed to
    prompt_assign_*_tmp where * is input, output, or bidir. If this is
    not a top level argument, i.e., a structure or array member, then
    ignore.
  
  prompt_assign_error

    Terminate program construction and return an error.
  
  prompt_assign_output_var

    Output variable exists - pass directly to function and schedule
    for printing. Not applicable to non-top level arguments.
  
  prompt_assign_output_var_c

    Output variable does not exist - create as global, pass directly
    to function, and schedule for printing. Output variable values are
    constructed by the function being called, so do not need to have a
    constructor called. Not applicable to non-top level arguments.
  
  prompt_assign_output_var_r

    Output variable exists but the type is incompatible - delete the
    global variable first and then process as with
    prompt_assign_output_var_c. Not applicable to non-top level
    arguments.
  
  prompt_assign_bidir_var_c

    Bidir variable does not exist - create as global, construct if
    necessary, pass directly to function, and schedule for
    printing. Not applicable to non-top level arguments.
  
  prompt_assign_input_constant

    The input is a constant. For a top level argument, create a
    temporary input variable, assign the constant to the variable, and
    pass the temporary variable to the function. For non-top-level arguments,
    assign to input component.
  
  prompt_assign_input_var
  
    The input is an existing variable - pass the input variable to the
    function. Not applicable to non-top level arguments.
  
 */

/* value assignment */
STATIC int
pa_value(prompt_arg_t *p_arg, const char *value)
{
    int rv = API_MODE_E_FAIL;

    if (p_arg->value) {
        sal_free((void *)p_arg->value);
    }
    
    p_arg->value = sal_strdup(value);
    if (p_arg->value != NULL) {
        rv = API_MODE_E_NONE;
    }

    return rv;
}

/* port converter help */
STATIC int
pa_bcm_port_t_help(api_mode_data_t *data, prompt_arg_t *p_arg)
{
    sal_printf("Physical port, gport, diag shell port\n");
    return API_MODE_E_NONE;
}

/* port converter */
STATIC int
pa_bcm_port_t_convert(api_mode_data_t *data, prompt_arg_t *p_arg, const char *s)
{
    bcm_port_t port;
    char buf[16];
    int rv = 0;
    
    rv = parse_bcm_port(data->unit, (char *)s, &port);
    if (API_MODE_SUCCESS(rv)) {
        sal_printf("convert: %s=%d\n", s, port);
        sal_sprintf(buf, "%d", port);
        rv = pa_value(p_arg, buf);
    } else {
        sal_printf("convert %s failed:%d\n", s, rv);
    }
    return rv;
}

/* MAC converter help */
STATIC int
pa_bcm_mac_t_help(api_mode_data_t *data, prompt_arg_t *p_arg)
{
    sal_printf("MAC address in the form of xx:xx:xx:xx:xx:xx\n");
    return API_MODE_E_NONE;
}

/* MAC converter */
STATIC int
pa_bcm_mac_t_convert(api_mode_data_t *data, prompt_arg_t *p_arg, const char *s)
{
    sal_mac_addr_t mac;
    char buf[32];
    int rv = API_MODE_E_NONE;
    
    sal_printf("convert %s\n", s);
    rv = parse_macaddr((char *)s, mac);
    if (API_MODE_SUCCESS(rv)) {
        sal_sprintf(buf, "\"%x:%x:%x:%x:%x:%x\"",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        rv = pa_value(p_arg, buf);
    }
    return rv;
}

/* MAC printer */
STATIC int
pa_bcm_mac_t_format(api_mode_data_t *data, prompt_arg_t *p_arg)
{
    CINT_STRCAT(data->program, "\"");
    CINT_STRCAT(data->program, p_arg->value);
    CINT_STRCAT(data->program, "\"");
    return API_MODE_E_NONE;
}

/* converter and type inference interfaces */
STATIC prompt_arg_helper_t pa_helper[] = {
    {
        "bcm_port_t",
        pa_bcm_port_t_help,
        pa_bcm_port_t_convert,
        NULL,
    },
    {
        "bcm_mac_t",
        pa_bcm_mac_t_help,
        pa_bcm_mac_t_convert,
        pa_bcm_mac_t_format
    }
};

STATIC prompt_arg_type_inference_t pa_tinf[] = {
    { "bcm_port_t",     "int",          "port" }
};

/* search for a type inferrer */
STATIC const char *
pa_infer(const cint_parameter_desc_t *desc)
{
    int i;
    
    for (i=0; i<COUNTOF(pa_tinf); i++) {
        if (!strcmp(pa_tinf[i].basetype_in, desc->basetype) &&
            !strcmp(pa_tinf[i].name, desc->name)) {
            return (pa_tinf[i].basetype_out);
        }
    }

    return desc->basetype;
}

/* search for a conversion helper */
STATIC prompt_arg_helper_t *
pa_helper_find(const cint_parameter_desc_t *desc)
{
    int i;
    const char *basetype = pa_infer(desc);
    
    for (i=0; i<COUNTOF(pa_helper); i++) {
        if (!strcmp(pa_helper[i].key, basetype)) {
            return (pa_helper+i);
        }
    }

    return NULL;
}

/* arg help */
STATIC int
pa_help(api_mode_data_t *data, prompt_arg_t *p_arg)
{
    prompt_arg_helper_t *helper;
    int rv = API_MODE_E_NONE;

    helper = pa_helper_find(&p_arg->desc);
    if (helper && helper->help) {
        rv = helper->help(data, p_arg);
    }
    return rv;
}

/* arg format */
STATIC int
pa_format(api_mode_data_t *data, prompt_arg_t *p_arg)
{
    prompt_arg_helper_t *helper;
    int rv = API_MODE_E_NONE;

    helper = pa_helper_find(&p_arg->desc);
    if (helper && helper->format) {
        rv = helper->format(data, p_arg);
    } else {
        CINT_STRCAT(data->program, p_arg->value);
    }
    return rv;
}

/* convert 's' */
STATIC int
pa_convert(api_mode_data_t *data, prompt_arg_t *p_arg, const char *s)
{
    prompt_arg_helper_t *helper;
    int rv = API_MODE_E_NONE;

    helper = pa_helper_find(&p_arg->desc);
    if (helper && helper->convert) {
        rv = helper->convert(data, p_arg, s);
    } else {
        rv = pa_value(p_arg, s);
    }
    return rv;
}

/* add prompt_arg to list */
STATIC void
add_arg(prompt_t *prompt, prompt_arg_t *arg)
{
    if (!prompt->head) {
        prompt->head = prompt->tail = arg;
    } else {
        prompt->tail->next = arg;
        arg->prev = prompt->tail;
        prompt->tail = arg;
    }
}

/* alloc a prompt_arg node */
STATIC prompt_arg_t *
alloc_arg(void)
{
    prompt_arg_t *arg;

    arg = sal_alloc(sizeof(*arg), "apimode");
    if (!arg) {
        return NULL;
    }
    sal_memset(arg, 0, sizeof(*arg));

    return arg;
}

/* display 'unsupported' */
STATIC void
_unsupported(void)
{
    sal_printf("unsupported\n");
}

#define MAXVARPATH 128

/* append a parameter desc as a prompt_arg node to the prompt list */
STATIC int
append_desc(prompt_t *prompt, const cint_parameter_desc_t *desc,
            const char *path, desc_data_t *data)
{
    prompt_arg_t *arg;
    cint_datatype_t dt;
    int rv;
    int expand = 0;
    char newpath[MAXVARPATH];
    const char *basetype;
    int array;
    int expand_struct;
    int expand_array;
    int path_len;
    cint_parameter_desc_t adesc;

    if (desc == NULL) {
        return api_mode_unexpected();
    }
    
    if (desc->basetype == NULL) {
        return api_mode_unexpected();
    }

    sal_memset(newpath, 0, sizeof(newpath));
    path_len = sal_strlen(desc->name);
    path_len += 1; /* NUL */
    if (path) {
        path_len += sal_strlen(path);
        path_len += 1; /* '.' */
        if (path_len < MAXVARPATH) {
            sal_strncpy(newpath, path, MAXVARPATH-1);
            CINT_STRCAT(newpath, ".");
        } else {
            /* too long */
            sal_printf("buffer overflow\n");
            return API_MODE_E_FAIL;
        }
    } else if (data->level != 0) {
        _unsupported();
        return API_MODE_E_FAIL;
    }

    if (path_len < MAXVARPATH) {
        CINT_STRCAT(newpath, desc->name);
    } else {
        sal_printf("buffer overflow\n");
        return API_MODE_E_FAIL;
    }

    basetype = desc->basetype;
    array = desc->array;
    adesc = *desc;

    for (;;) {
        rv = cint_datatype_find(basetype, &dt);
        if (rv != CINT_E_NONE) {
            sal_printf("Could not find datatype '%s'\n", basetype);
            return API_MODE_E_FAIL;
        }
        
        /* If this is a typedef, resolve typedef */
        if (dt.flags & CINT_DATATYPE_F_TYPEDEF) {
            basetype = dt.desc.basetype;
            if (dt.desc.array > 0) {
                array = (array == 0) ? dt.desc.array : array * dt.desc.array;
                adesc = dt.desc;
                adesc.name = desc->name;
            }
            if (dt.desc.pcount != 0) {
                api_mode_unexpected();
            }
        } else {
            break;
        }
    }

    expand = (data->c_flags & CINT_PARAM_IN) &&
        desc->pcount < ((path == NULL) ? 2 : 1) &&
        desc->array == 0 && (pa_helper_find(desc) == NULL);
    expand_struct = (dt.flags & CINT_DATATYPE_F_STRUCT) != 0 && expand;
    expand_array = array > 0;
    
    

    arg = alloc_arg();
    if (!arg) {
        return API_MODE_E_MEMORY;
    }

    arg->desc = *desc;
    arg->dt = dt;
    arg->path = sal_strdup(newpath);
    arg->idx = data->idx;
    arg->c_flags = data->c_flags;
    arg->level = data->level;
    arg->pos = data->pos;
    arg->p_flags = data->p_flags;
    if (expand_struct || expand_array) {
        arg->p_flags |= PA_EXPAND;
    }
    add_arg(prompt, arg);

    /* add any expanded arguments */
    if (expand_array) {
        desc_data_t adata;

        adata = *data;
        adata.level++;
        adesc.array = 0;
        for (adata.idx=0; adata.idx<array; adata.idx++) {
            rv = append_desc(prompt, &adesc, path, &adata);
            if (API_MODE_FAILURE(rv)) {
                return rv;
            }
        }
    } else if (desc->flags & CINT_PARAM_VP) {
        _unsupported();
        return API_MODE_E_UNAVAIL;
    } else if (desc->flags & CINT_PARAM_VL) {
        _unsupported();
        return API_MODE_E_UNAVAIL;
    } else if (expand_struct) {
        const cint_parameter_desc_t *m;
        desc_data_t sdata;

        sdata = *data;
        sdata.level++;
        sdata.idx = -1;
        for (m=dt.basetype.sp->struct_members; m->basetype; m++) {
            rv = append_desc(prompt, m, newpath, &sdata);
            if (API_MODE_FAILURE(rv)) {
                return rv;
            }
            
        }
    }

    return API_MODE_E_NONE;
}

/* free a prompt list */
STATIC void
prompt_free(prompt_t *prompt)
{
    prompt_arg_t *arg, *next;

    arg = prompt->head;
    while (arg != NULL) {
        if (arg->value) {
            sal_free((void *)arg->value);
            arg->value = NULL;
        }
        if (arg->path) {
            sal_free((void *)arg->path);
            arg->path = NULL;
        }
        next = arg->next;
        sal_free(arg);
        arg = next;
    }
}

/* display assignment operations */
STATIC void
_annotate_assignments(prompt_arg_t *prg)
{
    for (;prg; prg=prg->next) {
        switch (prg->assign_op) {
        case prompt_assign_none:
            sal_printf("%s n/a\n", prg->path);
            break;

        case prompt_assign_error:
            sal_printf("%s err\n", prg->path);
            break;

        case prompt_assign_output_var:
            sal_printf("%s=%s output_var\n", prg->path, prg->value);
            break;

        case prompt_assign_output_var_c:
            sal_printf("%s=%s output_var_c\n", prg->path, prg->value);
            break;

        case prompt_assign_output_var_r:
            sal_printf("%s=%s output_var_r\n", prg->path, prg->value);
            break;

        case prompt_assign_bidir_var_c:
            sal_printf("%s=%s bidir_var_c\n", prg->path, prg->value);
            break;

        case prompt_assign_input_constant:
            sal_printf("%s=%s input_constant\n", prg->path, prg->value);
            break;

        case prompt_assign_input_var:
            sal_printf("%s=%s input_var\n", prg->path, prg->value);
            break;
            
        default:
            break;
        }
    }
}

/* create a CINT variable corresponding to 'arg' */
STATIC int
_create_var(api_mode_data_t *data, prompt_arg_t *arg,
            const char *prefix, const char *assignment)
{
    if (prefix) {
        CINT_STRCAT(data->program, prefix);
    }
    CINT_STRCAT(data->program, arg->desc.basetype);
    CINT_STRCAT(data->program, " ");
    CINT_STRCAT(data->program, arg->value);
    if (assignment) {
        CINT_STRCAT(data->program, " = ");
        CINT_STRCAT(data->program, assignment);
    }
    CINT_STRCAT(data->program, ";\n");

    return API_MODE_E_NONE;
}

/* call an initializer for CINT variable corresponding to 'arg' if an
   initializer exists. */
STATIC int
_init_var(api_mode_data_t *data, prompt_arg_t *arg)
{
    int rv;
    cint_datatype_t dt;

    /* locate a constructor */
    sal_strcpy(data->stage, arg->desc.basetype);
    CINT_STRCAT(data->stage, "_init");
    
    rv = cint_datatype_find(data->stage, &dt);

    if (rv == CINT_E_NONE &&
        (dt.flags & CINT_DATATYPE_FLAGS_FUNC) != 0 &&
        dt.basetype.fp->params[0].basetype != NULL &&
        dt.basetype.fp->params[1].basetype != NULL &&
        dt.basetype.fp->params[2].basetype == NULL &&
        !sal_strcmp(arg->desc.basetype, dt.basetype.fp->params[1].basetype) &&
        arg->desc.pcount == dt.basetype.fp->params[1].pcount &&
        arg->desc.array == dt.basetype.fp->params[1].array) {
        /* Function was found and accepts a single parameter of a
           given argument's datatype. Rely on CINT auto-pointer,
           so don't bother to take address of argument. */
        CINT_STRCAT(data->program, data->stage);
        CINT_STRCAT(data->program, "(");
        CINT_STRCAT(data->program, arg->value);
        CINT_STRCAT(data->program, ");\n");
    }

    /* it is not a problem if an initializer does not exist */
    return API_MODE_E_NONE;
}

/* create a CINT temporary variable corresponding to 'arg' */
STATIC int
_temp_var(api_mode_data_t *data, prompt_arg_t *arg)
{
    int rv = API_MODE_E_FAIL;

    if (arg->value == NULL) {
        data->stage[0] = 0;
        CINT_STRCAT(data->stage, "__");
        CINT_STRCAT(data->stage, arg->desc.name);
        arg->value = sal_strdup(data->stage);
        rv = (arg->value) ? API_MODE_E_NONE : API_MODE_E_FAIL;
    }

    return rv;
}

/* call 'func' for every element in the prompt list */
STATIC int
_prompt_apply(api_mode_data_t *data, prompt_t *prompt, paf func)
{
    prompt_arg_t *arg;
    int rv = API_MODE_E_NONE;

    for (arg=prompt->head; arg; arg=arg->next) {
        rv = func(data, arg);
        if (API_MODE_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

/* return an error code if an assign error was found */
STATIC int
_paf_error(api_mode_data_t *data, prompt_arg_t *dst)
{
    return (dst->assign_op == prompt_assign_error) ?
        API_MODE_E_FAIL : API_MODE_E_NONE;
}

/* delete any existing variables for toplevel prompt_assign_output_var_r */
STATIC int
_paf_delete_existing(api_mode_data_t *data, prompt_arg_t *dst)
{
    if (dst->level == 0 && dst->assign_op == prompt_assign_output_var_r) {
        if (data->debug) {
            sal_printf("// deleting variable '%s'\n", dst->value);
        }
        cint_variable_delete(dst->value);
    }

    return API_MODE_E_NONE;
}

/* create needed toplevel global variables */
STATIC int
_paf_create_global(api_mode_data_t *data, prompt_arg_t *dst)
{
    int rv = API_MODE_E_NONE;

    if (dst->level == 0) {
        switch (dst->assign_op) {
        case prompt_assign_output_var_c:
        case prompt_assign_output_var_r:
        case prompt_assign_bidir_var_c:
            rv = _create_var(data, dst, NULL, NULL);
            break;
        default:
            break;
        }
    }

    return rv;
}

/* create needed local variables */
STATIC int
_paf_create_local(api_mode_data_t *data, prompt_arg_t *dst)
{
    int rv = API_MODE_E_NONE;

    if (dst->level == 0) {
        switch (dst->assign_op) {
        case prompt_assign_none:

            if (dst->desc.pcount > 1) {
                /* unsupported - ignore for now */
            } else {
                rv = _temp_var(data, dst);
                if (API_MODE_SUCCESS(rv)) {
                    rv = _create_var(data, dst, API_MODE_INDENT, NULL);
                    if (API_MODE_SUCCESS(rv)) {
                        dst->assign_op = prompt_assign_create_tmp;
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    return rv;
}

/* initialize local variables */
STATIC int
_paf_init_local(api_mode_data_t *data, prompt_arg_t *dst)
{
    int rv = API_MODE_E_NONE;

    if (dst->level == 0) {
        switch (dst->assign_op) {
        case prompt_assign_create_tmp:

            if (dst->c_flags & CINT_PARAM_IN) {
                CINT_STRCAT(data->program, API_MODE_INDENT);
                rv = _init_var(data, dst);
                if (API_MODE_SUCCESS(rv)) {
                    dst->assign_op = prompt_assign_input_tmp;
                }
            }
            if (dst->c_flags & CINT_PARAM_OUT) {
                dst->assign_op = (dst->assign_op == prompt_assign_input_tmp) ?
                    prompt_assign_bidir_tmp : prompt_assign_output_tmp;
            }
            break;
        default:
            break;
        }
    }

    return rv;
}

/* assign input constants - must be called after _paf_create_local */
STATIC int
_paf_init_input_const(api_mode_data_t *data, prompt_arg_t *dst)
{
    int rv = API_MODE_E_NONE;

    switch (dst->assign_op) {
    case prompt_assign_input_constant:
        /* top level (function) arguments that are assigned a constant
           will have that constant passed directly, as it would be, by
           definition, a scalar constant. Anything deeper than that,
           i.e., composite arguments will have such constants assigned
           to a termporary variable. */
        if (dst->level > 0) {
            CINT_STRCAT(data->program, API_MODE_INDENT "__");
            CINT_STRCAT(data->program, dst->path);
            if (dst->idx >= 0) {
                sal_sprintf(data->stage, "[%d]", dst->idx);
                CINT_STRCAT(data->program, data->stage);
            }
            CINT_STRCAT(data->program, "=");
            pa_format(data, dst);
            CINT_STRCAT(data->program, ";\n");
        }
        break;
        
    case prompt_assign_input_tmp:
        break;
        
    default:
        break;
    }

    return rv;
}

/* print outputs */
STATIC int
_paf_print_output(api_mode_data_t *data, prompt_arg_t *dst)
{
    switch (dst->assign_op) {
    case prompt_assign_output_var_c:
    case prompt_assign_output_var_r:
    case prompt_assign_bidir_var_c:
    case prompt_assign_bidir_tmp:
    case prompt_assign_output_tmp:
        CINT_STRCAT(data->program, API_MODE_INDENT "print ");
        CINT_STRCAT(data->program, dst->value);
        CINT_STRCAT(data->program, ";\n");
        break;
    default:
        break;
    }

    return API_MODE_E_NONE;
}

/* emit function call arguments */
STATIC int
_paf_call_arg(api_mode_data_t *data, prompt_arg_t *dst)
{
    if (dst->level == 0) {
        CINT_STRCAT(data->program, dst->value);
        if ((dst->p_flags & PA_LAST) == 0) {
            CINT_STRCAT(data->program, ", ");
        }
    }

    return API_MODE_E_NONE;
}

#define RV "__rv"


/* function return type initialization */
STATIC int   
_init_func(api_mode_data_t *data, prompt_t *prompt,
           const cint_datatype_t *dt, int *vret)
{
    const cint_parameter_desc_t *desc;
    int void_return = 0;

    /* create return type */
    desc = dt->basetype.fp->params;
    void_return = !strcmp(desc->basetype, "void");

    if (!void_return) {
        CINT_STRCAT(data->program, API_MODE_INDENT);
        CINT_STRCAT(data->program, desc->basetype);
        CINT_STRCAT(data->program, " " RV ";\n");
    }

    *vret = void_return;

    return API_MODE_E_NONE;
}


/* call CINT function */    
STATIC int   
_call_func(api_mode_data_t *data, prompt_t *prompt,
           const cint_datatype_t *dt, int void_return)
{
    int rv;

    /* function call */
    CINT_STRCAT(data->program, API_MODE_INDENT);
    if (!void_return) {
        CINT_STRCAT(data->program, RV " = ");
    }
    CINT_STRCAT(data->program, dt->desc.basetype);
    CINT_STRCAT(data->program, "(");
    rv = _prompt_apply(data, prompt, _paf_call_arg);
    if (API_MODE_SUCCESS(rv)) {
        CINT_STRCAT(data->program, ");\n");

        /* print return value */
        if (!void_return && sh_rcload_depth == 0) {
            CINT_STRCAT(data->program, API_MODE_INDENT "print " RV ";\n");
            CINT_STRCAT(data->program,
                        API_MODE_INDENT
                        "printf(\"%s\\n\", "
                        "bcm_errmsg(" RV "));\n");
        }
    }
    return rv;
}
    
/* execute an argument list as a CINT program */
STATIC int
_api_mode_exec(api_mode_data_t *data, prompt_t *prompt,
               const cint_datatype_t *dt)
{
    int void_return;
    int rv;
    
    if (data->debug) {
        _annotate_assignments(prompt->head);
    }
    rv = _prompt_apply(data, prompt, _paf_error);
    API_MODE_IF_ERROR_RETURN(rv);
    rv = _prompt_apply(data, prompt, _paf_delete_existing);
    API_MODE_IF_ERROR_RETURN(rv);
    rv = _prompt_apply(data, prompt, _paf_create_global);
    API_MODE_IF_ERROR_RETURN(rv);
    /* open new scope */
    {
        CINT_STRCAT(data->program, "{\n");
        rv = _init_func(data, prompt, dt, &void_return);
        API_MODE_IF_ERROR_RETURN(rv);
        /* set up input */
        rv = _prompt_apply(data, prompt, _paf_create_local);
        API_MODE_IF_ERROR_RETURN(rv);
        rv = _prompt_apply(data, prompt, _paf_init_local);
        API_MODE_IF_ERROR_RETURN(rv);
        rv = _prompt_apply(data, prompt, _paf_init_input_const);
        API_MODE_IF_ERROR_RETURN(rv);
        /* call API */
        rv = _call_func(data, prompt, dt, void_return);
        API_MODE_IF_ERROR_RETURN(rv);
        /* print output */
        if (sh_rcload_depth == 0) {
            rv = _prompt_apply(data, prompt, _paf_print_output);
            API_MODE_IF_ERROR_RETURN(rv);
        }
        /* close scope */
        CINT_STRCAT(data->program, "}\n");
    }
    rv = cmd_api_run(data);

    return rv;
}

/* prompt mode handler */
STATIC prompt_arg_t *
api_mode_prompt_arg(api_mode_data_t *data, prompt_arg_t *arg)
{
    prompt_arg_t *nextarg = arg;
    char *s;

    sal_strncpy(data->prompt, param_dir_str(arg->c_flags),
                API_MODE_PROMPT_SIZE-1);
    while (sal_strlen(data->prompt) < 3) {
        CINT_STRCAT(data->prompt, " ");
    }
    CINT_STRCAT(data->prompt, arg->dt.desc.basetype);
    CINT_STRCAT(data->prompt, " ");
    CINT_STRCAT(data->prompt, arg->path);
    if (arg->idx >= 0) {
        sal_sprintf(data->stage, "[%d]", arg->idx);
        CINT_STRCAT(data->prompt, data->stage);
    }
    CINT_STRCAT(data->prompt, "{");
    if (arg->value) {
        CINT_STRCAT(data->prompt, arg->value);
    }
    CINT_STRCAT(data->prompt, "}");
    CINT_STRCAT(data->prompt, ": ");
    s = sal_readline(data->prompt, data->stage, API_MODE_STAGE_SIZE, NULL);
    if (s == NULL) {
        nextarg = NULL;
    } else if (!strncmp(data->stage, "-", 1)) {
        /* don't go back if at first arg */
        if (arg->prev) {
            nextarg = arg->prev;
            if ((nextarg->p_flags & PA_EXPAND)) {
                nextarg = nextarg->prev;
            }
        }
    } else if (*data->stage == 0) {
        nextarg = arg->next;
    } else if (!strncmp(data->stage, "?", 1)) {
        pa_help(data, arg);
    } else {
        if (!pa_convert(data, arg, data->stage)) {
            arg->assign_op = prompt_assign_input_constant;
            nextarg = arg->next;
        }
    }
    return nextarg;
}

#if 0
STATIC void
api_mode_prompt_show(prompt_arg_t *arg)
{
    if (arg->idx < 0) {
        sal_printf("%s %s", arg->dt.desc.basetype, arg->path);
    } else {
        sal_printf("%s %s[%d]", arg->dt.desc.basetype, arg->path, arg->idx);
    }
    if (arg->value) {
        sal_printf("=%s", arg->value);
    }
    sal_printf("\n");
}
#endif

/* set the default unit */
STATIC int
set_default_unit(api_mode_data_t *data, prompt_t *dst)
{
    char ubuf[16];
    int rv = API_MODE_E_NONE;
    prompt_arg_t *arg = dst->assign;

    if (arg &&
        !arg->value &&
        !strcmp(arg->desc.basetype, "int") &&
        !strcmp(arg->desc.name, "unit")) {
        sal_sprintf(ubuf, "%d", data->unit);
        rv = pa_value(arg, ubuf);
        dst->assign->assign_op = prompt_assign_input_constant;
    }

    return rv;
}

/* initialize prompt list */
STATIC int
init_prompt(api_mode_data_t *data, prompt_t *prompt,
            api_mode_arg_t *arg,
            const cint_datatype_t *dt)
{
    const cint_parameter_desc_t *desc;
    api_mode_arg_t *argctr;
    desc_data_t ddata;
    int pos;
    int rv;

    desc = dt->basetype.fp->params;

    if (desc == NULL) {
        /* vararg func slipped through */
        return API_MODE_E_INTERNAL;
    }

    sal_memset(prompt, 0, sizeof(*prompt));
    desc++; /* skip return */

    /* iterate across function parameters to create prompt list */
    sal_memset(&ddata, 0, sizeof(ddata));
    ddata.idx = -1;
    pos = 0;
    while (desc->basetype != NULL) {
        ddata.c_flags = desc->flags;
        if (desc[1].basetype == NULL) {
            ddata.p_flags |= PA_LAST;
        }
        rv = append_desc(prompt, desc, NULL, &ddata);
        if (API_MODE_FAILURE(rv)) {
            return rv;
        }
        desc++;
        pos++;
    }

    for (argctr=arg; argctr != NULL; argctr=argctr->next) {
        pos--;
    }

    /* set search and assignment pointers */
    prompt->search = prompt->head;
    prompt->assign = prompt->head;
    prompt->context = prompt_ctx_outer;

    /* set default value for unit */
    rv = set_default_unit(data, prompt);

    if (prompt->assign &&
        prompt->assign->assign_op == prompt_assign_input_constant &&
        pos > 0 && arg && arg->kind != KEY_VALUE && prompt->assign->next) {
        /* special case: if the number of supplied arguments is less
           than the number of required arguments, the unit has a
           default value (which also means it is the first argument),
           the first supplied argument is not key/value, and there
           actually *is* another argument, then assume the default
           unit assignment and advance the search and assignment
           position */
        prompt->assign = prompt->assign->next;
        prompt->search = prompt->search->next;
    }
    return rv;
}

#define CINT_INPUT(f) (((f) & CINT_PARAM_INOUT) == CINT_PARAM_IN)
#define CINT_OUTPUT(f) (((f) & CINT_PARAM_INOUT) == CINT_PARAM_OUT)
#define CINT_BIDIR(f) (((f) & CINT_PARAM_INOUT) == CINT_PARAM_INOUT)

/* Special assignments - currently unimplemented */
STATIC int
_assign_special(api_mode_data_t *data, prompt_arg_t *prg, api_mode_arg_t *arg)
{
    return API_MODE_E_UNAVAIL;
}

/* Display ident error */
STATIC void
_ident_error(api_mode_arg_t *arg)
{
    /* error - must exist */
    sal_printf("Identifier '%s' must exist "
               "for assignment to an input parameter.\n",
               arg->value);
}

/* Display type incompatible error */
STATIC void
_compat_error(api_mode_arg_t *arg)
{
    /* error - type incompatible */
    sal_printf("Identifier '%s' type incompatible.\n",
               arg->value);
}

/* assignment forward references */
STATIC int
_agg_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src);

STATIC int
_generic_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src);


/* skip over expanded destinations */
STATIC void
_advance_expanded(prompt_t *dst)
{
    int level = dst->assign->level;

    do {
        dst->assign = dst->assign->next;
        if (!dst->assign) {
            /* no more */
            break;
        }
    } while (dst->assign->level != level);
}

/* assign a value */
STATIC int
_value_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src)
{
    int rv = API_MODE_E_FAIL;
    cint_variable_t *v;
    const char *name = (const char *)src->value;
    cint_datatype_t dt;

    if (dst->assign->c_flags & CINT_PARAM_OUT) { /* out or bidir */
        /* outputs *must* be identifiers corresponding to variables */
        if (src->token->ident) {
            /* look it up */
            v = cint_variable_find(name, TRUE);
            if (dst->assign->c_flags & CINT_PARAM_IN) { /* bidir */
                if (v != NULL) { /* exists */
                    if (cint_type_check(&dst->assign->dt, &v->dt)) {
                        /* compatible - use directly */
                        dst->assign->assign_op = prompt_assign_output_var;
                        rv = API_MODE_E_NONE;
                    } else {
                        _compat_error(src);
                        /* incompatible - error */
                        dst->assign->assign_op = prompt_assign_error;
                        rv = API_MODE_E_FAIL;
                    }
                } else {
                    /* as a convenience, treat bidir idents as
                       outputs; the input will be whatever the default
                       initializer provides, if it exists. */
                    dst->assign->assign_op = prompt_assign_bidir_var_c;
                    rv = API_MODE_E_NONE;
                }
            } else { /* out */
                if (v != NULL) { /* exists */
                    if (cint_type_check(&dst->assign->dt, &v->dt)) {
                        /* compatible - use directly */
                        dst->assign->assign_op = prompt_assign_output_var;
                    } else {
                        /* incompatible - recreate */
                        dst->assign->assign_op = prompt_assign_output_var_r;
                    }
                } else {
                    /* doesn't exists - create */
                    dst->assign->assign_op = prompt_assign_output_var_c;
                }
                rv = API_MODE_E_NONE;
            }
        } else {
            /* error */
            sal_printf("Expected an identifier "
                       "for assignment to an output parameter.\n");
        }
    } else if (dst->assign->c_flags & CINT_PARAM_IN) {
        if (src->token->ident) {
            v = cint_variable_find(name, TRUE);
            /* inputs *may* be identifiers corresponding to variables,
               manifest constants, or enums */
            if (v != NULL) {
                if (cint_type_check(&dst->assign->dt, &v->dt)) {
                    dst->assign->assign_op = prompt_assign_input_var;
                    rv = API_MODE_E_NONE;
                } else {
                    _compat_error(src);
                    dst->assign->assign_op = prompt_assign_error;
                    rv = API_MODE_E_FAIL;
                }
            } else if (cint_datatype_enum_find(name, &dt, NULL)==CINT_E_NONE) {
                dst->assign->assign_op = prompt_assign_input_var;
                rv = API_MODE_E_NONE;
            } else if (cint_datatype_find(name, &dt) == CINT_E_NONE) {
                dst->assign->assign_op = prompt_assign_input_var;
                rv = API_MODE_E_NONE;
            } else {
                _ident_error(src);
            }
        } else {
            /* must be assigning a scalar, which is never assigned
               directly to a composite value. */
            while (dst->assign && (dst->assign->p_flags & PA_EXPAND)) {
                dst->assign = dst->assign->next;
            }
            if (dst->assign) {
                if (!_assign_special(data, dst->assign, src)) {
                    dst->assign->assign_op = prompt_assign_input_constant;
                    rv = 1; /* value already assigned */
                } else if (dst->assign->c_flags & CINT_DATATYPE_FLAGS_FUNC) {
                    _unsupported();
                    rv = API_MODE_E_FAIL;
                } else {
                    dst->assign->assign_op = prompt_assign_input_constant;
                    rv = API_MODE_E_NONE;
                    /* scalar */
                }
            }
        }
    } else {
        sal_printf("Parameter %s is neither in or out.\n", dst->assign->path);
    }

    if (rv == API_MODE_E_NONE) {
        rv = pa_value(dst->assign, src->value);
    }
    
    return rv;
}

/* display missing key error */
STATIC void
_no_key(api_mode_arg_t *arg)
{
    sal_printf("Could not match key '%s'\n", arg->value);
}

/* keyword/value assignment */
STATIC int
_kw_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src)
{
    int level;
    int len;
    const char *key;
    int found = 0;
    int rv = API_MODE_E_NONE;
    prompt_arg_t *dict;
    prompt_arg_t *search;
    prompt_arg_t *assign;
    
    dst->context = prompt_ctx_kv;
    key = src->value;
    len = sal_strlen(key);

    if ((dst->search->p_flags & PA_EXPAND) && dst->search->next) {
        dst->search = dst->search->next;
    }

    level = dst->search->level;

    for (dict = dst->search; dict; dict=dict->next) {
        int short_name, long_name;

        /* skip anything deeper than our current level */
        if (dict->level > level) {
            continue;
        }

        /* stop if anything shallower than our current level */
        if (dict->level < level) {
            break;
        }

        long_name  = strncmp(dict->path, key, len);
        short_name = strcmp(dict->desc.name, key);

        if (!long_name || !short_name) {
            if (src->kind != KEY_VALUE) {
                return api_mode_unexpected();
            }
            /* save assign, search point */
            search = dst->search;
            assign = dst->assign;
            dst->search = dict;
            dst->assign = dict;
            rv = _generic_assign(data, dst, src->sub ? src->sub : src);
            /* restore assign, search point */
            dst->search = search;
            dst->assign = assign;
            found = 1;
            break;
        }
    }

    if (!found) {
        _no_key(src);
    }

    return rv;
}

/* display no more arguments error */
STATIC void
_no_arg(void)
{
    sal_printf("Ran out of arguments to set\n");
}

/* positional assignment */
STATIC int
_pos_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src)
{
    int rv = API_MODE_E_FAIL;

    if (dst->assign) {
        dst->context = prompt_ctx_pos;
        rv = _value_assign(data, dst, src);

        if (API_MODE_SUCCESS(rv)) {
            if ((dst->assign->p_flags & PA_EXPAND) &&
                dst->assign->assign_op == prompt_assign_input_var) {
                /* skip over expansion */
                _advance_expanded(dst);
            } else {
                dst->assign = dst->assign->next;
            }
            dst->search = dst->assign;
        }
    } else {
        _no_arg();
    }

    return rv;

}

/* subvalue prompt assignment */
STATIC int
_prompt_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src)
{
    return API_MODE_E_UNAVAIL;
}

/* generic assignment based on src */
STATIC int
_generic_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src)
{
    int rv = API_MODE_E_NONE;
    prompt_ctx_t current = dst->context;

    switch (src->kind) {
    case AGGREGATE:
        rv = _agg_assign(data, dst, src->sub);
        break;
    case KEY_VALUE:
        {
            prompt_arg_t *search = dst->search; /* save */
            rv = _kw_assign(data, dst, src);
            dst->search = search;               /* restore */
        }
        break;
    case PROMPT:
        rv = _prompt_assign(data, dst, src);
        break;
    default:
        rv = _pos_assign(data, dst, src);
        break;
    }

    dst->context = current;
    return rv;
}

/* aggregate assignment */
STATIC int
_agg_assign(api_mode_data_t *data, prompt_t *dst, api_mode_arg_t *src)
{
    api_mode_arg_t *arg;
    int rv = API_MODE_E_NONE;

    for (arg=src; arg; arg=arg->next) {
        rv = _generic_assign(data, dst, arg);
        if (API_MODE_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

/* variable prompt mode */
STATIC int
api_mode_var_prompt(api_mode_data_t *data, const cint_datatype_t *dt)
{
    _unsupported();
    return API_MODE_E_NONE;
}

/* function prompt mode */
STATIC int
api_mode_func_prompt(api_mode_data_t *data, const cint_datatype_t *dt)
{
    prompt_t prompt;
    prompt_arg_t *arg;
    int rv;

    rv = init_prompt(data, &prompt, NULL, dt);

    if (API_MODE_SUCCESS(rv)) {
        /* prompt parameters */
        for (arg=prompt.head; arg; ) {
            if ((arg->p_flags & PA_EXPAND)) {
                if (arg->next == NULL) {
                    /* Should not happen */
                    return api_mode_unexpected();
                }
                arg = arg->next;
            }
            arg = api_mode_prompt_arg(data, arg);
        }
        _api_mode_exec(data, &prompt, dt);

        prompt_free(&prompt);
    }

    return rv;
}

/* call an API */
STATIC int
api_mode_call(api_mode_data_t *data,
              api_mode_arg_t *arg_in, const cint_datatype_t *dt)
{
    prompt_t prompt;
    int rv;

    rv = init_prompt(data, &prompt, arg_in, dt);

    if (API_MODE_SUCCESS(rv)) {
        rv = _agg_assign(data, &prompt, arg_in);
    }

    if (API_MODE_SUCCESS(rv)) {
        _api_mode_exec(data, &prompt, dt);
    }

    return rv;
}

/* print a variable */
STATIC int
api_mode_print(api_mode_data_t *data, api_mode_arg_t *arg)
{
    api_mode_arg_t *mbr;
    int rv;

    /* Force prints to be in a new scope. */
    
    CINT_STRCAT(data->program, "{\n");
    while (arg) {
        CINT_STRCAT(data->program, "print ");
        CINT_STRCAT(data->program, arg->value);
        for (mbr=arg->sub; mbr; mbr=mbr->next) {
            CINT_STRCAT(data->program, ".");
            CINT_STRCAT(data->program, mbr->value);
        }
        CINT_STRCAT(data->program, ";\n");
        arg = api_mode_arg_next(arg);
    }
    CINT_STRCAT(data->program, "}\n");
    rv = cmd_api_run(data);

    return rv;
}

/* set a variable */
STATIC int
api_mode_var(api_mode_data_t *data, api_mode_arg_t *arg)
{
    api_mode_arg_t *mbr;
    int rv;

    while (arg) {
        CINT_STRCAT(data->program, arg->value);
        for (mbr=arg->sub; mbr; mbr=mbr->next) {
            CINT_STRCAT(data->program, ".");
            CINT_STRCAT(data->program, mbr->value);
        }
        if (arg->next) {
            CINT_STRCAT(data->program, "=");
        }
        arg = api_mode_arg_next(arg);
    }
    CINT_STRCAT(data->program, ";\n");
    rv = cmd_api_run(data);

    return rv;
}

/* create a variable */
STATIC int
api_mode_create(api_mode_data_t *data, api_mode_arg_t *arg)
{
    int rv = API_MODE_E_FAIL;
    prompt_arg_t pa; /* just need desc and value */

    sal_memset(&pa, 0, sizeof(pa));
    if (arg) {
        pa.path = arg->value;
        rv = cint_datatype_find(pa.path, &pa.dt);
        if (rv == CINT_E_NONE) {
            arg = arg->next;
            pa.desc = pa.dt.desc;
            pa.desc.pcount++;
            if (arg == NULL) {
                sal_printf("Missing variable name\n");
                rv = API_MODE_E_PARAM;
            }
        } else {
            sal_printf("Could not find type '%s'\n", pa.path);
        }
    }
            
    if (API_MODE_SUCCESS(rv)) {
        while (arg) {
            pa.value = arg->value;
            CINT_STRCAT(data->program, pa.path);
            CINT_STRCAT(data->program, " ");
            CINT_STRCAT(data->program, pa.value);
            CINT_STRCAT(data->program, ";\n");
            rv = _init_var(data, &pa);
            if (API_MODE_FAILURE(rv)) {
                break;
            }
            arg = arg->next;
        }
    }

    if (API_MODE_SUCCESS(rv)) {
        rv = cmd_api_run(data);
    }

    return rv;
}

/* handle an API mode command */
STATIC int
handle_command(api_mode_data_t *data, api_mode_arg_t *cmd)
{
    int rv;
    const cint_datatype_t *dt;
    api_mode_arg_t *arg;
    int op;

    dt = NULL;
    arg = NULL;
    rv = match_command(cmd, &arg, &dt, &op);
    if (API_MODE_SUCCESS(rv)) {
        switch (op) {
        case OP_FPROMPT:
            rv = api_mode_func_prompt(data, dt);
            break;
        case OP_VPROMPT:
            rv = api_mode_var_prompt(data, dt);
            break;
        case OP_HELP:
            rv = api_mode_help(dt);
            break;
        case OP_PRINT:
            rv = api_mode_print(data, arg->next);
            break;
        case OP_VAR:
            rv = api_mode_var(data, arg->next);
            break;
        case OP_CREATE:
            rv = api_mode_create(data, arg->next);
            break;
        case OP_KWARG:
        default:
            rv = api_mode_call(data, arg, dt);
            break;
        }
    } else if (op == OP_HELP) {
        rv = api_mode_partial_help(cmd);
    }
    return rv;
}

/* execute an API mode command */
STATIC int
exec_command(api_mode_arg_t *cmd, void *user_data)
{
    api_mode_data_t *data = (api_mode_data_t *)user_data;
    int rv = API_MODE_E_NONE;
    int debug = 0;

    if (cmd->kind != EMPTY) {
        if (cmd->kind == INFO) {
            cmd = cmd->next;
            debug = 1;
        }
        if (cmd) {
            int old_debug = 0;
            if (debug) {
                old_debug = data->debug;
                data->debug = !data->debug;
            }

            rv = handle_command(data, cmd);

            if (debug) {
                data->debug = old_debug;
            }
        } else if (debug) {
            data->debug = !data->debug;
            sal_printf("API mode debug is %s\n", data->debug?"on":"off");
        }
    }

    /* if rv indicates an error, then the original command string will
       be passed to the subsequent shell, so 'error' really means "the
       string means nothing to me". All other errors should return
       API_MODE_E_NONE so the string stays within the API mode realm.
    */
#if 0
    data->rv = ((rv == API_MODE_E_FAIL) ? API_MODE_E_NONE : rv);
#else
    data->rv = ((rv == API_MODE_E_NOT_FOUND) ? rv : API_MODE_E_NONE);
#endif
    return rv;
}

#define SIZE (API_MODE_STAGE_SIZE+API_MODE_PROMPT_SIZE+API_MODE_PROGRAM_SIZE)

STATIC api_mode_data_t api_data;

/* process an API mode command from the CLI */
int
api_mode_process_command(int u, char *s)
{
    int rv;

    api_data.unit = u;
    if (!api_data.stage) {
        api_data.stage = sal_alloc(SIZE, "apimode");
        if (!api_data.stage) {
            return CMD_FAIL;
        }
        api_data.prompt = api_data.stage + API_MODE_STAGE_SIZE;
        api_data.program = api_data.prompt + API_MODE_PROMPT_SIZE;
    }
    sal_memset(api_data.stage, 0, SIZE);
    rv = api_mode_parse_string(s, PARSE_VERBOSE, exec_command, &api_data);
    if (API_MODE_SUCCESS(rv)) {
        rv = api_data.rv;
    }
    return rv;
}
