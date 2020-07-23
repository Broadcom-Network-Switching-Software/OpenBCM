/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_ast_debug.c
 * Purpose:     CINT debug functions
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_internal.h"
#include "cint_ast.h"

static void
__indent(int count)
{
    while(count--) CINT_PRINTF(" ");
}

static int
__print(int indent, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 2, 3)));

static int
__print(int indent, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __indent(indent);
    CINT_VPRINTF(fmt, args);
    va_end(args);
    return 0;
}

static int
__start_member(int indent, const char* str)
{
    return __print(indent, "{ %s\n", str);
}

static int
__end_member(int indent)
{
    return __print(indent, "}\n");
}

static int
__member(int indent, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 2, 3)));

static int
__member(int indent, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __indent(indent);
    CINT_PRINTF("{ ");
    CINT_VPRINTF(fmt, args);
    CINT_PRINTF(" }\n");
    va_end(args);
    return 0;
}

#define START(_str) __start_member(indent, _str)
#define END() __end_member(indent)
#define MEMBER(expr) __member expr


static void
__cint_ast_dump_Empty(cint_ast_t* ast, int indent)
{
}

static void
__cint_ast_dump_Integer(cint_ast_t* ast, int indent)
{
    int i = ast->utype.integer.i;
    __print(indent, "0x%x - %d - %u\n", i, i, i);
}

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
static void
__cint_ast_dump_LongLong(cint_ast_t* ast, int indent)
{
    long long i = ast->utype._longlong.i;
    char buf1[50], *s1;
    char buf2[50], *s2;
    char buf3[50], *s3;

    s1 = cint_lltoa(buf1, sizeof(buf1), i, 0, 16, 16);
    s2 = cint_lltoa(buf2, sizeof(buf2), i, 0, 10, 0);
    s3 = cint_lltoa(buf3, sizeof(buf3), i, 0, 10, 0);
    __print(indent, "0x%s - %s - %s\n", s1, s2, s3);
}
#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
static void
__cint_ast_dump_Double(cint_ast_t* ast, int indent)
{
    double d = ast->utype._double.d;
    __print(indent, "%f\n", d);
}
#endif

static void
__cint_ast_dump_String(cint_ast_t* ast, int indent)
{
    __print(indent, "'%s'\n",
            ast->utype.string.s);
}


static void
__cint_ast_dump_Type(cint_ast_t* ast, int indent)
{
    __print(indent, "'%s'\n",
            ast->utype.type.s);
}


static void
__cint_ast_dump_Identifier(cint_ast_t* ast, int indent)
{
    __print(indent, "'%s'\n",
            ast->utype.identifier.s);
}

static void
__cint_ast_dump_Initializer(cint_ast_t* ast, int indent)
{
    cint_ast_dump(ast->utype.initializer.initializers, indent+4);
}

static void
__cint_ast_dump_Declaration(cint_ast_t* ast, int indent)
{
    int dimension = 0;

    START("TYPE");
    cint_ast_dump(ast->utype.declaration.type, indent+4);
    END();

    MEMBER((indent, "PCOUNT %d", ast->utype.declaration.pcount));

    START("DIMENSIONS");
    for(dimension = 0;
        dimension < CINT_CONFIG_ARRAY_DIMENSION_LIMIT;
        ++dimension) {
        cint_ast_dump(
            ast->utype.declaration.dimension_initializers[dimension],
            indent+4);
    }
    END();

    START("IDENT");
    cint_ast_dump(ast->utype.declaration.identifier, indent+4);
    END();

    START("INIT");
    cint_ast_dump(ast->utype.declaration.init, indent+4);
    END();

    MEMBER(
        (indent,
        "ARRAYDIMENSIONS %d",
        ast->utype.declaration.num_dimension_initializers));
}


static void
__cint_ast_dump_Operator(cint_ast_t* ast, int indent)
{
    MEMBER((indent, "OP = '%s'", cint_operator_name(ast->utype.operator.op)));

    START("LEFT");
    cint_ast_dump(ast->utype.operator.left, indent+4);
    END();

    START("RIGHT");
    cint_ast_dump(ast->utype.operator.right, indent+4);
    END();

    START("EXTRA");
    cint_ast_dump(ast->utype.operator.extra, indent+4);
    END();
}

static void
__cint_ast_dump_Function(cint_ast_t* ast, int indent)
{
    MEMBER((indent, "NAME = '%s'", ast->utype.function.name));

    START("PARAMS");
    cint_ast_dump(ast->utype.function.parameters, indent+4);
    END();
}

static void
__cint_ast_dump_FunctionDef(cint_ast_t* ast, int indent)
{
    START("DECLARATION");
    cint_ast_dump(ast->utype.functiondef.declaration, indent+4);
    END();

    START("PARAMETERS");
    cint_ast_dump(ast->utype.functiondef.parameters, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype.functiondef.statements, indent+4);
    END();
}

static void
__cint_ast_dump_StructureDef(cint_ast_t* ast, int indent)
{
    START("NAME");
    cint_ast_dump(ast->utype.structuredef.name, indent+4);
    END();

    START("MEMBERS");
    cint_ast_dump(ast->utype.structuredef.members, indent+4);
    END();
}

static void
__cint_ast_dump_Elist(cint_ast_t* ast, int indent)
{
    cint_ast_t* p;
    for(p = ast->utype.elist.list; p; p = p->next) {
        cint_ast_dump(p, indent);
    }
}

static void
__cint_ast_dump_Enumerator(cint_ast_t* ast, int indent)
{
    START("IDENTIFIER");
    cint_ast_dump(ast->utype.enumerator.identifier, indent+4);
    END();

    START("VALUE");
    cint_ast_dump(ast->utype.enumerator.value, indent+4);
    END();
}

static void
__cint_ast_dump_EnumDef(cint_ast_t* ast, int indent)
{
    START("IDENTIFIER");
    cint_ast_dump(ast->utype.enumdef.identifier, indent+4);
    END();

    START("ENUMERATORS");
    cint_ast_dump(ast->utype.enumdef.enumerators, indent+4);
    END();
}


static void
__cint_ast_dump_While(cint_ast_t* ast, int indent)
{
    MEMBER((indent, "ORDER = %d", ast->utype._while.order));

    START("CONDITION");
    cint_ast_dump(ast->utype._while.condition, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype._while.statements, indent+4);
    END();
}


static void
__cint_ast_dump_For(cint_ast_t* ast, int indent)
{
    START("PRE");
    cint_ast_dump(ast->utype._for.pre, indent+4);
    END();

    START("CONDITION");
    cint_ast_dump(ast->utype._for.condition, indent+4);
    END();

    START("POST");
    cint_ast_dump(ast->utype._for.post, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype._for.statements, indent+4);
    END();
}


static void
__cint_ast_dump_If(cint_ast_t* ast, int indent)
{
    START("CONDITION");
    cint_ast_dump(ast->utype._if.condition, indent+4);
    END();

    START("STATEMENTS");
    cint_ast_dump(ast->utype._if.statements, indent+4);
    END();

    START("ELSE");
    cint_ast_dump(ast->utype._if._else, indent+4);
    END();
}

static void
__cint_ast_dump_Return(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype._return.expression, indent+4);
    END();
}

static void
__cint_ast_dump_Continue(cint_ast_t* ast, int indent)
{
    /* Nothing */
}

static void
__cint_ast_dump_Break(cint_ast_t* ast, int indent)
{
    /* Nothing */
}

static void
__cint_ast_dump_Print(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype.print.expression, indent+4);
    END();
}

static void
__cint_ast_dump_Cint(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype.cint.arguments, indent+4);
    END();
}

static void
__cint_ast_dump_Switch(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype._switch.expression, indent+4);
    END();
    START("STATEMENTS");
    cint_ast_dump(ast->utype._switch.statements, indent+4);
    END();
}

static void
__cint_ast_dump_Case(cint_ast_t* ast, int indent)
{
    START("EXPR");
    cint_ast_dump(ast->utype._case.expression, indent+4);
    END();
    START("STATEMENTS");
    cint_ast_dump(ast->utype._case.statements, indent+4);
    END();
}



struct {
    const char* name;
    void (*dump)(cint_ast_t* node, int indent);
} __ast_table[] = {

#define CINT_AST_LIST_ENTRY(_entry) { #_entry, __cint_ast_dump_##_entry },
#include "cint_ast_entry.h"
    { NULL }
};

void
cint_ast_dump_single(cint_ast_t* ast, int indent)
{
    cint_ast_type_t t;

    if(ast == NULL) {
        __print(indent, "NULL\n");
    } else if (ast == CINT_AST_PTR_VOID) {
        __print(indent, "VOID *\n");
    } else if (ast == CINT_AST_PTR_AUTO) {
        __print(indent, "AUTO\n");
    } else {

        t = ast->ntype;

        if(!CINT_AST_TYPE_VALID(t)) {
            __print(indent, "INVALID AST TYPE %d", t);
        }
        else {
            __print(indent, "{ %s\n", __ast_table[t].name);
            __ast_table[t].dump(ast, indent+4);
            __print(indent, "}\n");
        }
    }
}

void
cint_ast_dump(cint_ast_t* ast, int indent)
{
    cint_ast_dump_single(ast, indent);
    if(ast &&
       ast != CINT_AST_PTR_VOID &&
       ast != CINT_AST_PTR_AUTO && ast->next) {
        cint_ast_dump(ast->next, indent);
    }
}
