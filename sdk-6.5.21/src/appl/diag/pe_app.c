/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       pe_app.c
 */


#include <shared/bsl.h>
#include <appl/diag/system.h>
 
#ifdef PE_SDK
extern int
pe_app(int testnum, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7);
#endif /* PE_SDK */


/*
 * Function: cmd_peapp
 * Purpose: calls pe app cmds
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns: CMD_OK/CMD_FAIL
 */
cmd_result_t
cmd_peapp(int u, args_t *a)
{
    int numargs;
    int tn=0;
    int a0=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0;
    char *cmd;
    
    numargs = ARG_CNT(a);
    /* check for test number */
    if (numargs>=1){
        cmd = ARG_GET(a);
        tn = sal_ctoi(cmd, NULL);
    }
    /* check for arg0 */
    if (numargs>=2){
        cmd = ARG_GET(a);
        a0 = sal_ctoi(cmd, NULL);
    }
    /* check for arg1 */
    if (numargs>=3){
        cmd = ARG_GET(a);
        a1 = sal_ctoi(cmd, NULL);
    }
    /* check for arg2 */
    if (numargs>=4){
        cmd = ARG_GET(a);
        a2 = sal_ctoi(cmd, NULL);
    }
    /* check for arg3 */
    if (numargs>=5){
        cmd = ARG_GET(a);
        a3 = sal_ctoi(cmd, NULL);
    }
    /* check for arg4 */
    if (numargs>=6){
        cmd = ARG_GET(a);
        a4 = sal_ctoi(cmd, NULL);
    }
    /* check for arg5 */
    if (numargs>=7){
        cmd = ARG_GET(a);
        a5 = sal_ctoi(cmd, NULL);
    }
    /* check for arg6 */
    if (numargs>=8){
        cmd = ARG_GET(a);
        a6 = sal_ctoi(cmd, NULL);
    }
    /* check for arg7 */
    if (numargs>=9){
        cmd = ARG_GET(a);
        a7 = sal_ctoi(cmd, NULL);
    }

#ifdef PE_SDK
    pe_app(tn, a0, a1, a2, a3, a4, a5, a6, a7);
#else
    cli_out("cmd_peapp() commented out call to pe_app(%d, %d, %d, %d, %d, %d, %d, %d, %d)\n", tn, a0, a1, a2, a3, a4, a5, a6, a7);
#endif /* PE_SDK */

    return CMD_OK;
}


