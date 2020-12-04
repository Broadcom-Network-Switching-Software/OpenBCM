/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dpp.c
 * Purpose:     Other CLI commands
 */

#include <shared/bsl.h>
#include <appl/diag/diag.h>

#if  defined(INCLUDE_DUNE_UI)
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/dpp/UserInterface/ui_ram_defi.h>
#endif /* INCLUDE_DUNE_UI */

#include <bcm/error.h>
#include <bcm/stack.h>


#if defined(INCLUDE_DUNE_UI)

char cmd_dpp_ui_usage[] =
    "Parameters: <command>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tEnter the Dune debug CLI. \n"
    "\tParameters:\n"
    "\t\tcommand: Optional. Dune CLI command to run.\n"
    "\t\t         If not given - enters interactive UI.\n"
#endif
    ;

cmd_result_t
cmd_dpp_ui(int unit, args_t* a)
{
    char        *c;

    static int first_call = 1;

    if (first_call) {
        init_ui_module();

        first_call = 0;
    }    
    
    if (ARG_CNT(a) > 0) {
        c = ARG_GET(a);
        do {            
            if (*c == '\0') {
                handle_next_char(&Current_line_ptr,'\n') ;
                cli_out("\n");
                break;
            } else {
                handle_next_char(&Current_line_ptr,*c) ;
                ++c;
            }
        } while (1);
    } else {    
        
        cli_out("Entering Dune UI shell. Type '`' (above the tab key) to quit.\n\n"); 
        ui_proc(); 
    }

    return 0; 
}

#endif /* INCLUDE_DUNE_UI */




