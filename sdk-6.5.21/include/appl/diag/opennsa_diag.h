/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This header file defines important constants and structures for
 * use with Broadcom Diagnostics Shell which is built upon the SOC
 * driver and SAL (System Abstraction Layer)
 */

#ifndef _OPENNSA_DIAG_H
#define _OPENNSA_DIAG_H

/*
 * Shell routines: used for diagnostics shell.
 */

extern void    diag_init(void);
extern void    cmdlist_init(void);
extern void    diag_rc_set(int unit, const char *fname);
extern void    diag_rc_get(int unit, const char **fname);
extern int     diag_rc_load(int unit);
extern void    diag_shell(void);

extern int     system_init(int unit);
extern int     system_shutdown(int unit, int cleanup);

extern int     (*soc_mem_config_set)(char *name, char *value);

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

cmd_result_t   sh_process(int unit, const char *, int eof);
cmd_result_t   sh_process_command(int unit, char *c);
cmd_result_t   sh_process_command_check(int unit, char *c);
int            sh_check_attached(const char *pfx, const int unit);

#endif /* !_OPENNSA_DIAG_H */
