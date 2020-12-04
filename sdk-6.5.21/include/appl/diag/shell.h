/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    sh.h
 * Purpose:    Types and structures for command table entires and
 *        command return values for cshell commands.
 */

#ifndef _DIAG_SH_H
#define _DIAG_SH_H

#ifdef __KERNEL__
/* Need jmp_buf to satisfy prototypes */
#define jmp_buf int
#else
#include <setjmp.h>
#endif
#if !defined(SWIG)
#include <sal/appl/io.h>
#endif
#include <appl/diag/parse.h>

/*
 * Define:    RCLOAD_DEPTH_MAX
 * Purpose:    Maximum depth of rcload files.
 */
#define RCLOAD_DEPTH_MAX    32

/*
 * Define:    LOG_DEFAULT_FILE
 * Purpose:    Default file name for "log" command
 */
#define LOG_DEFAULT_FILE    "bcm.log"

/*
 * Define:    PUSH_CTRL_C_CNT
 * Purpose:    Defines the maximum number of pushed Control-C handlers.
 * Notes:    Must be more than RCLOAD_DEPTH_MAX.
 */
#define    PUSH_CTRL_C_CNT    (RCLOAD_DEPTH_MAX + 4)

/*
 * Typedef:     cmd_result_t
 * Purpose:    Type retured from all commands indicating success, fail, 
 *        or print usage.
 */
typedef enum cmd_result_e {
    CMD_OK   = 0,            /* Command completed successfully */
    CMD_FAIL = -1,            /* Command failed */
    CMD_USAGE= -2,            /* Command failed, print usage  */
    CMD_NFND = -3,            /* Command not found */
    CMD_EXIT = -4,            /* Exit current shell level */
    CMD_INTR = -5,            /* Command interrupted */
    CMD_NOTIMPL = -6            /* Command not implemented */
} cmd_result_t;

/*
 * Typedef:    cmd_func_t
 * Purpose:    Defines command function type
 */
typedef cmd_result_t (*cmd_func_t)(int, args_t *);

/*
 * Typedef:    cmd_t
 * Purpose:    Table command match structure.
 */
typedef struct cmd_s {
    parse_key_t    c_cmd;            /* Command string */
    cmd_func_t    c_f;            /* Function to call */
    const char     *c_usage;        /* Usage string */
    const char    *c_help;        /* Help string */
} cmd_t;

/*
 * Shell rc file load depth
 */
extern int sh_rcload_depth;


/*
 * Shell 'set' variables
 */

extern int    sh_set_rcload;
extern int    sh_set_rcerror;
extern int    sh_set_rcnoprinterror;
extern int    sh_set_lperror;
extern int    sh_set_iferror;
extern int    sh_set_rctest;
extern int    sh_set_more_lines;
extern int    sh_set_report_time;
extern int    sh_set_report_status;


/*
 * Exported shell.c functions.
 */

cmd_result_t     sh_process(int unit, const char *, int eof);
cmd_result_t    sh_process_command(int unit, char *c);
cmd_result_t    sh_process_command_check(int unit, char *c);
int        sh_check_attached(const char *pfx, const int unit);
cmd_result_t    sh_rcload_file(int u, args_t *, char *f, int add_rc);
void        sh_swap_unit_vars(int new_unit);

/*
 * Functions for pushing/poping where to longjump on control-c.
 */
extern void    sh_push_ctrl_c(jmp_buf *);
extern void    sh_pop_ctrl_c(void);
extern void    sh_block_ctrl_c(int);
extern void    sh_ctrl_c_take(void);
extern void    sh_print_version(int verbose);

extern void    sh_bg_init(void);

#endif /* _DIAG_SH_H */
