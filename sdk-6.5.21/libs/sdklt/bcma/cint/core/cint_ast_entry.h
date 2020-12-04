/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_ast_entry.h
 * Purpose:     Declare AST entries based on CINT_AST_LIST_ENTRY
 */

#ifndef CINT_AST_LIST_ENTRY
#error CINT_AST_LIST_ENTRY needs definition
#endif

CINT_AST_LIST_ENTRY(Integer)
#if CINT_CONFIG_INCLUDE_DOUBLES == 1
CINT_AST_LIST_ENTRY(Double)
#endif
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
CINT_AST_LIST_ENTRY(LongLong)
#endif
CINT_AST_LIST_ENTRY(String)
CINT_AST_LIST_ENTRY(Type)
CINT_AST_LIST_ENTRY(Identifier)
CINT_AST_LIST_ENTRY(Declaration)
CINT_AST_LIST_ENTRY(Initializer)
CINT_AST_LIST_ENTRY(Operator)
CINT_AST_LIST_ENTRY(Function)
CINT_AST_LIST_ENTRY(Elist)
CINT_AST_LIST_ENTRY(While)
CINT_AST_LIST_ENTRY(For)
CINT_AST_LIST_ENTRY(Continue)
CINT_AST_LIST_ENTRY(Break)
CINT_AST_LIST_ENTRY(Return)
CINT_AST_LIST_ENTRY(If)
CINT_AST_LIST_ENTRY(Print)
CINT_AST_LIST_ENTRY(FunctionDef)
CINT_AST_LIST_ENTRY(StructureDef)
CINT_AST_LIST_ENTRY(Switch)
CINT_AST_LIST_ENTRY(Case)
CINT_AST_LIST_ENTRY(Cint)
CINT_AST_LIST_ENTRY(Enumerator)
CINT_AST_LIST_ENTRY(EnumDef)
CINT_AST_LIST_ENTRY(Empty)

#undef CINT_AST_LIST_ENTRY
