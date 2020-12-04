/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_eval_ast_cint.c
 * Purpose:     CINT AST node evaluators
 */

#include "cint_eval_ast_cint.h"
#include "cint_porting.h"
#include "cint_interpreter.h"
#include "cint_internal.h"
#include "cint_eval_asts.h"

/*
 * Command Handlers
 */

#define CCINT_PRINTF CINT_PRINTF("cint> "); CINT_PRINTF


#if CINT_CONFIG_INCLUDE_CINT_LOAD == 1
#include <dlfcn.h>

void
cint_ast_cint_cmd_unload(void *handle)
{
    if (handle && dlclose(handle)) {
        CCINT_PRINTF("%s", dlerror()); 
    }
}
#endif

cint_variable_t* 
__cint_ast_cint_cmd_load(cint_ast_t* ast, const char* argv[])
{
#if CINT_CONFIG_INCLUDE_CINT_LOAD == 0
    cint_ast_error(ast, CINT_E_UNSUPPORTED, "dynamic library loading support was not compiled in."); 
#else
    void* handle;
    const char** s = NULL; 

    for(s = argv; *s; s++) {

        if((handle = dlopen(*s, RTLD_NOW)) == NULL) {
            cint_ast_error(ast, CINT_E_BAD_AST, "loading library '%s': %s", *s, dlerror()); 
        }
        else {
#define STRINGIFY0(x) #x
#define STRINGIFY(x) STRINGIFY0(x)
            void* data;             
            char *sym = STRINGIFY(CINT_LOAD_DATA_SYMBOL);

            if((data = dlsym(handle, sym)) == NULL) {
                cint_ast_error(ast, CINT_E_BAD_AST, "%s", dlerror()); 
                dlclose(handle); 
                return NULL; 
            }       
            else {
                cint_interpreter_add_data(*(cint_data_t**)data, handle); 
            }
        }
    }
#endif /* CINT_CONFIG_INCLUDE_CINT_LOAD */

    return NULL; 
}

cint_variable_t*
__cint_ast_cint_cmd_typecheck(cint_ast_t* ast, const char* argv[])
{
    cint_datatype_checkall(1); 
    return NULL; 
}

cint_variable_t*
__cint_ast_cint_cmd_var(cint_ast_t* ast, const char* argv[])
{
    cint_variable_t* v = NULL; 
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};

    if(*argv == NULL) {
        CCINT_PRINTF("no variable specified"); 
        return NULL; 
    }

    v = cint_variable_find(*argv, 0); 

    if(v) {
        CCINT_PRINTF("variable '%s' is type '%s' @ %p size=%d\n", v->name, cint_datatype_format(&v->dt,_rstr,0), v->data, v->size);  
    }
    else {        
        CCINT_PRINTF("variable '%s' not found\n", *argv); 
    }
    return NULL; 
}       


static int
__cint_list_datatype_traverse(void* cookie, const cint_datatype_t* dt)
{
    const char* substr = (const char*)cookie; 
    int flags = dt->flags;
    char _rstr[256] = {0};

    if(flags & CINT_DATATYPE_F_TYPEDEF) {
        if(substr && CINT_STRSTR(dt->type, substr) == NULL) {
            /* No Match */
            return 0; 
        }
        CINT_PRINTF("%s typedef for %s ", dt->type, cint_datatype_format_pd(&dt->desc,_rstr, 0)); 
        flags &= ~CINT_DATATYPE_F_TYPEDEF; 
    }
    else if (flags & CINT_DATATYPE_F_CONSTANT) {
        if(substr && CINT_STRSTR(dt->basetype.cp->name, substr) == NULL) {
            /* No Match */
            return 0; 
        }
    }   
    else {
        if(substr && CINT_STRSTR(dt->desc.basetype, substr) == NULL) {
            /* No Match */
            return 0; 
        }
        CINT_PRINTF("%s ", dt->desc.basetype); 
    }

    switch(flags) 
        {
        case CINT_DATATYPE_F_ATOMIC: CINT_PRINTF("(atomic)"); break; 
        case CINT_DATATYPE_F_STRUCT: CINT_PRINTF("(struct)"); break; 
        case CINT_DATATYPE_F_ENUM:     CINT_PRINTF("(enum)"); break; 
        case CINT_DATATYPE_F_FUNC_DYNAMIC:     
        case CINT_DATATYPE_F_FUNC:   CINT_PRINTF("(function)"); break; 
        case CINT_DATATYPE_F_CONSTANT: CINT_PRINTF("%s (constant)", dt->basetype.cp->name); break; 
        case CINT_DATATYPE_F_FUNC_POINTER: CINT_PRINTF("(function pointer)"); break; 
        case CINT_DATATYPE_F_MACRO: CINT_PRINTF("(macro)"); break; 
        case CINT_DATATYPE_F_ITERATOR: CINT_PRINTF("(iterator)"); break; 
        default:  CINT_PRINTF("(unknown datatype flags 0x%x)", flags); break; 
        }
    
    CINT_PRINTF("\n"); 
    return 0; 
}

cint_variable_t*
__cint_ast_cint_cmd_list(cint_ast_t* ast, const char* argv[])
{
    const char** arg = argv; 
    unsigned dtflags = 0; 
    const char* str = NULL; 

    for(arg = argv; *arg; arg++) {
        if(!CINT_STRCMP(*arg, "atomic")) {
            dtflags |= CINT_DATATYPE_F_ATOMIC; 
        }
        else if(!CINT_STRCMP(*arg, "struct")) {
            dtflags |= CINT_DATATYPE_F_STRUCT;
        }
        else if(!CINT_STRCMP(*arg, "enum")) {
            dtflags |= CINT_DATATYPE_F_ENUM; 
        }       
        else if(!CINT_STRCMP(*arg, "fp")) {
            dtflags |= CINT_DATATYPE_F_FUNC_POINTER; 
        }       
        else if(!CINT_STRCMP(*arg, "function")) {
            dtflags |= CINT_DATATYPE_F_FUNC|CINT_DATATYPE_F_FUNC_DYNAMIC; 
        }
        else if(!CINT_STRCMP(*arg, "constant")) {
            dtflags |= CINT_DATATYPE_F_CONSTANT; 
        }
        else if(!CINT_STRCMP(*arg, "macro")) {
            dtflags |= CINT_DATATYPE_F_MACRO; 
        }
        else if(!CINT_STRCMP(*arg, "iterator")) {
            dtflags |= CINT_DATATYPE_F_ITERATOR; 
        }
        else {
            if(str != NULL) {
                cint_ast_error(ast, CINT_E_BAD_AST, "only one search substring may be specified"); 
                return NULL; 
            }
            else {
                str = *arg; 
            }
        }
    }
    
    /* 
     * Traverse all datatypes 
     */
    cint_datatype_traverse(dtflags, __cint_list_datatype_traverse, (void*)str); 
    
    return NULL; 
}

#if CINT_CONFIG_YAPI

typedef struct yapi_data_s {
    int level;
    unsigned int flags;
} yapi_data_t;

#define NULLSTR(s) ((s) ? (s) : "NULL")

static void
_yapi_indent(int level)
{
    while (level-- > 0) {
        CINT_PRINTF("  ");
    }
}

static int
_yapi_desc(const cint_parameter_desc_t *desc, int level)
{
    int dim_index;
    _yapi_indent(level);
    CINT_PRINTF("basetype: %s\n", NULLSTR(desc->basetype));
    _yapi_indent(level);
    CINT_PRINTF("pcount: %d\n", desc->pcount);
    _yapi_indent(level);
    CINT_PRINTF("name: %s\n", NULLSTR(desc->name));
    _yapi_indent(level);
    CINT_PRINTF(
        "arrayDimensions: %d\n", 
        desc->utype.declaration.arrayDimensions);
    for(dim_index = 0; 
        dim_index < desc->utype.declaration.arrayDimensions;
        ++dim_index) {
        _yapi_indent(level);
        CINT_PRINTF(
            "dimensions[%d]: %d\n",
            dim_index,
            desc->dimensions[dim_index]);
    }
    _yapi_indent(level);
    CINT_PRINTF("flags: %d\n", desc->flags);

    return 0;
}

static void
_yapi_func(const char *name, const cint_parameter_desc_t *param, int *level)
{
    int idx;
    _yapi_indent(*level);
    CINT_PRINTF("%s:\n", name);
    *level += 1;
    if (param) {
        for (idx=0; param->name; idx++, param++) {
            if (idx == 0) {
                _yapi_indent(*level);
                CINT_PRINTF("return:\n");
            } else if (idx == 1) {
                _yapi_indent(*level);
                CINT_PRINTF("arg:\n");
            }
            if (idx > 0) {
                _yapi_indent(*level+1);
                CINT_PRINTF("-\n");
                _yapi_desc(param, *level+2);
            } else {
                _yapi_desc(param, *level+1);
            }
        }
    } else {
        _yapi_indent(*level);
        CINT_PRINTF("vararg: 1\n");
    }
}

static int
_yapi_show(const cint_datatype_t *dt, yapi_data_t *data)
{
    int show_desc = 0;
    int level = data->level;

    if (level > 5) {
        _yapi_indent(level);
        CINT_PRINTF("error: DEPTH\n");
        return -1;
    }

    if (dt == NULL) {
        _yapi_indent(level);
        CINT_PRINTF("error: NULLDT\n");
        return -1;
    }
    
    /* consistency check */
    if (dt->flags == CINT_DATATYPE_F_ATOMIC && dt->flags == data->flags) {
        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->desc.basetype);
        show_desc = 1;
        level += 1;
    }
    if (dt->flags & CINT_DATATYPE_F_STRUCT) {
        const cint_parameter_desc_t *m;
        show_desc = 1;
        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->desc.basetype);
        level += 1;
        _yapi_indent(level);
        CINT_PRINTF("members:\n");
        for (m=dt->basetype.sp->struct_members; m->basetype; m++) {
            _yapi_indent(level+1);
            CINT_PRINTF("-\n");
            _yapi_desc(m, level+2);
        }
    }

    if (dt->flags & CINT_DATATYPE_F_ENUM) {
        cint_enum_type_t* ep = dt->basetype.ep;
        cint_enum_map_t* enum_map;

        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->desc.basetype);
        level += 1;

        for (enum_map = ep->enum_map; enum_map->name; enum_map++) {
            _yapi_indent(level);
            printf("%s: %d\n", enum_map->name, enum_map->value);
        }
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC) {
        show_desc = 1;
        _yapi_func(dt->desc.basetype, dt->basetype.fp->params, &level);
    }
    if (dt->flags & CINT_DATATYPE_F_CONSTANT) {
        _yapi_indent(level);
        printf("%s: %d\n", dt->basetype.cp->name, dt->basetype.cp->value);
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC_DYNAMIC) {
        show_desc = 1;
        _yapi_indent(level);
        CINT_PRINTF("dynamic_function: 1\n");
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC_POINTER) {
        show_desc = 1;
        _yapi_func(dt->desc.basetype, dt->basetype.fpp->params, &level);
    }
    if (dt->flags & CINT_DATATYPE_F_ITERATOR) {
        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->basetype.ip->name);
    }
    if (dt->flags & CINT_DATATYPE_F_MACRO) {
        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->basetype.mp->name);
    }
    if (dt->flags & CINT_DATATYPE_F_FUNC_VARARG) {
        show_desc = 1;
        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->desc.basetype);
    }
    if (dt->flags & CINT_DATATYPE_F_TYPEDEF) {
        show_desc = 1;
        _yapi_indent(level);
        CINT_PRINTF("%s:\n", dt->desc.name);
        level += 1;
        _yapi_indent(level);
        CINT_PRINTF("typedef:\n");
        if (dt->flags & CINT_DATATYPE_F_ATOMIC) {
            _yapi_indent(level);
            CINT_PRINTF("atomic:\n");
        }
    }

    if (show_desc) {
        _yapi_desc(&dt->desc, level);
    }

    return 0;
}


static int
__yapi(void* cookie, const cint_datatype_t* dt)
{
    yapi_data_t *data = (yapi_data_t *)cookie;

    (void)_yapi_show(dt, data);

    return 0;
}

static void _yapi_trav(const char *name, unsigned int flags)
{
    yapi_data_t data;

    CINT_PRINTF("%s:\n", name);
    data.level = 1;
    data.flags = flags;
    cint_datatype_traverse(data.flags, __yapi, &data);
}

static void
_yapi(void)
{
    CINT_PRINTF("---\n");
    _yapi_trav("ATOMIC", CINT_DATATYPE_F_ATOMIC);
    _yapi_trav("STRUCT", CINT_DATATYPE_F_STRUCT);
    _yapi_trav("ENUM", CINT_DATATYPE_F_ENUM);
    _yapi_trav("CONSTANT", CINT_DATATYPE_F_CONSTANT);
    _yapi_trav("FUNC", CINT_DATATYPE_F_FUNC);
    _yapi_trav("MACRO", CINT_DATATYPE_F_MACRO);
    _yapi_trav("ITERATOR", CINT_DATATYPE_F_ITERATOR);
    _yapi_trav("TYPEDEF", CINT_DATATYPE_F_TYPEDEF);
    _yapi_trav("FUNC_POINTER", CINT_DATATYPE_F_FUNC_POINTER);
    CINT_PRINTF("\n");
}
#endif

cint_variable_t*
__cint_ast_cint_cmd_yapi(cint_ast_t* ast, const char* argv[])
{
#if CINT_CONFIG_YAPI == 0
    cint_ast_error(ast, CINT_E_UNSUPPORTED, "YAML emitter not compiled in."); 
#else
    _yapi();
#endif /* CINT_CONFIG_YAPI */

    return NULL; 
}



/*
 * Command Handler Dispatch
 */
struct {
    const char* cmd; 
    cint_variable_t* (*handler)(cint_ast_t* ast, const char* args[]); 
} __handler_table[] = {

#define CINT_AST_CINT_CMD_ENTRY(_e) { #_e, __cint_ast_cint_cmd_##_e }, 
#include "cint_eval_ast_cint.h"
    { NULL }
}; 

cint_variable_t* 
cint_eval_ast_Cint(cint_ast_t* ast)
{
    int i; 
    cint_ast_t* cmd; 
    cint_ast_t* args;    
    const char* argv[64] = { NULL }; 
        
    if(ast == NULL) {
        return NULL; 
    }   
    
    cmd = ast->utype.cint.arguments; 
    
    if(cmd == NULL) {
        return NULL; 
    }
    
    /* Process string arguments */
    for(i = 0, args = cmd->next; args; args = args->next, i++) {
        switch(args->ntype)
            {
            case cintAstIdentifier:     argv[i] = args->utype.identifier.s; break; 
            case cintAstString:         argv[i] = args->utype.string.s; break; 
            case cintAstType:           argv[i] = args->utype.type.s; break; 
            default: break; 
            }   
    }   
    argv[i] = NULL; 
    
    for(i = 0; __handler_table[i].cmd; i++) {
        if(!CINT_STRCMP(cmd->utype.identifier.s, __handler_table[i].cmd)) {
            return __handler_table[i].handler(cmd, argv); 
        }
    }   

    return NULL; 
}
