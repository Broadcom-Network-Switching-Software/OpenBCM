/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: edk.c
 * Purpose: EDK support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <appl/diag/shell.h>
#include <soc/defs.h>

#ifdef BCM_EDK_SUPPORT
#if (!defined(LINUX) && !defined(UNIX))
#error "EDK-Host cannot be built on non-Linux/UNIX platforms"
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif /* Linux/UNIX */

char edkhost_usage[] =
    "Start EDK-Host console\n\t\t"
    "Parameters: none\n";

pid_t edkhost_pid = -1;

#ifdef EDKHLIB
extern int edk_main(int argc, char *argv[]);
#endif
cmd_result_t
edkhost_cmd(int unit, args_t *a)
/*
 * Function:    edkhost_cmd
 * Purpose: Start the EDK-Host console
 * Parameters:  unit - unit
 *      a - args, each of the files to be displayed.
 * Returns: CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
#ifdef EDKHLIB
    char* argv_list[ARGS_CNT] = {"edk_main"};
    char *arg_string_p = NULL;
    int argc = 1;

    arg_string_p = ARG_GET(a);
    if (arg_string_p != NULL) {
        if (parse_cmp("-c", arg_string_p, 0)) {
            while(1) {
                arg_string_p = ARG_GET(a);
                if (arg_string_p == NULL) {
                    break;
                }
                argv_list[argc] = arg_string_p; argc++;
            }
        } else {
            return CMD_USAGE;
        }
    }
    edk_main(argc, argv_list);
    return CMD_OK;
#else
    char* argv_list[] = {"edk-host.elf", NULL};
    int wstatus;

    if (edkhost_pid == -1) {
        FILE * volatile fp = NULL;

        /* Check if the elf is readable. */
        fp = sal_fopen(argv_list[0], "r");
        if (fp == NULL) {
            cli_out("%s: Error: Unable to open file: %s\n",
                    ARG_CMD(a), argv_list[0]);
            return CMD_FAIL;
        }
        sal_fclose((FILE *)fp);

        edkhost_pid = fork();
        if (edkhost_pid == 0) {
            /* Creates EDK-Host microservice */
            execv("edk-host.elf", argv_list);

            /*
             * If control reaches here, then it means loading failed for some
             * reason.
             */
            cli_out("%s: Error: %s loading failed. \n",
                    ARG_CMD(a), argv_list[0]);
            exit(1);
        } else {
            /* Parent process waits for child process signal */
            waitpid(edkhost_pid, &wstatus, WUNTRACED);
            if (WIFSTOPPED(wstatus)) {
                tcsetpgrp(0, getpid());
                /* Send the continue signal */
                kill(edkhost_pid, SIGCONT);
            }
        }
    } else {
        /* If already launched launched, signal continue and wait for child process switch */
        kill(edkhost_pid, SIGCONT);
        waitpid(edkhost_pid, &wstatus, WUNTRACED);
        if (WIFSTOPPED(wstatus)) {
            tcsetpgrp(0, getpid());
            kill(edkhost_pid, SIGCONT);
        }
        
    }
#endif

    return CMD_OK;
}
#else
typedef int _edk_not_empty; /* Make ISO compilers happy. */
#endif /* EDK support */
