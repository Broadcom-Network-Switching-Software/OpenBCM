/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_eval_ast_print.c
 * Purpose:     CINT print function
 */

#include "cint_eval_ast_print.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_internal.h"
#include "cint_error.h"
#include "cint_debug.h"
#include "cint_eval_asts.h"


cint_variable_t*
cint_eval_ast_Print(cint_ast_t* ast)
{
    cint_variable_t* v = NULL; 
    char _rstr[256] = {0};

    if(ast) {
        
        switch(ast->utype.print.expression->ntype)
            {
                /*
                 * Make the output nicer for immediate strings and integers.
                 */
            case cintAstString:
                {
                    /* print immediate string value */
                    char *tmp, *nl; 

                    tmp = cint_cstring_value
                        (ast->utype.print.expression->utype.string.s);
                    CINT_PRINTF("%s", tmp);

                    /* Check if the string terminates in a newline. If
                       not, then issue one. */
                    nl=CINT_STRRCHR(tmp, '\n');
                    if (nl == NULL || nl[1] != 0) {
                        /*
                          nl is NULL if there were no newlines in the
                          string at all. nl[0] is non-zero if the
                          last newline in the string is not before the
                          terminating null.
                         */
                        CINT_PRINTF("\n");
                    }
                    CINT_FREE(tmp); 
                    break; 
                }
            case cintAstInteger:
                {
                    /* print immediate integer value */
                    CINT_PRINTF("%ld\n", ast->utype.print.expression->utype.integer.i); 
                    break; 
                }
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
            case cintAstLongLong:
                {
                    long long i =
                        ast->utype.print.expression->utype._longlong.i;
                    char buf1[50], *s1;
                    s1 = cint_lltoa(buf1, sizeof(buf1), i, 1, 10, 0);
                    /* print immediate integer value */
                    CINT_PRINTF("%s\n", s1); 
                    break; 
                }
#endif
            case cintAstType:
                {
                    /* Lookup the type and print information about it */
                    cint_datatype_t dt; 
                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    if(cint_datatype_find(ast->utype.print.expression->utype.type.s, &dt) == CINT_E_NONE) {
                        switch((dt.flags & CINT_DATATYPE_FLAGS_TYPE))
                            {
                            case CINT_DATATYPE_F_ATOMIC:
                                {
                                    CINT_PRINTF("%s: atomic datatype, size %d bytes\n", dt.basetype.ap->name, dt.basetype.ap->size); 
                                    break; 
                                }
                            case CINT_DATATYPE_F_STRUCT:
                                {       
                                    const cint_parameter_desc_t* sm; 
                                    CINT_PRINTF("struct %s {\n", dt.basetype.sp->name); 
                                    for(sm = dt.basetype.sp->struct_members; sm->basetype; sm++) {
                                        cint_datatype_t dt;
                                        dt.desc = *sm; 
                                        CINT_PRINTF("    %s %s;\n", cint_datatype_format(&dt, _rstr, 0), sm->name); 
                                    }
                                    CINT_PRINTF("}\n"); 
                                    CINT_PRINTF("size is %d bytes\n", dt.basetype.sp->size); 
                                    break; 
                                }
                            case CINT_DATATYPE_F_ENUM:
                                {
                                    const cint_enum_map_t* ep; 
                                    CINT_PRINTF("enum %s {\n", dt.basetype.ep->name); 
                                    for(ep = dt.basetype.ep->enum_map; ep->name; ep++) {
                                        CINT_PRINTF("    %s = %d\n", ep->name, ep->value);      
                                    }   
                                    CINT_PRINTF("}\n"); 
                                    break; 
                                }
                            case CINT_DATATYPE_F_FUNC_POINTER:
                                {
                                    cint_parameter_desc_t* p = dt.basetype.fpp->params;
                                    CINT_PRINTF("function pointer: '%s (*%s)", cint_datatype_format_pd(p, _rstr, 0), dt.basetype.fpp->name);  
                                    cint_fparams_print(p+1); 
                                    CINT_PRINTF("'\n"); 
                                    break; 
                                }
                            case CINT_DATATYPE_F_TYPEDEF:
                                {
                                    /* Nothing yet */
                                    break; 
                                }
                            }   
                    }   
                else {
                        /* Should never get here */
                    }
                    break; 
                }
            default:
                {
                    v = cint_eval_ast(ast->utype.print.expression); 

                    if(v) {
                        cint_variable_print(v, 0, v->name);     
                    }   
                    else {
                        /* No error necessary -- an error would have been printed during the evaluation */
                    }
                    break; 
                }   
            }       
    }   

    return v; 
}
