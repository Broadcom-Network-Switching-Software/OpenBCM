/*! \file bcma_clicmd_rcload.c
 *
 * CLI 'rcload' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_rccache.h>
#include <bcma/cli/bcma_clicmd_rcload.h>

/*
 * Cookie for Ctrl-C wrapper function rcload_file_ctrlc.
 */
typedef struct rcload_info_s {

    /* CLI object */
    bcma_cli_t *cli;

    /* CLI command arguments */
    bcma_cli_args_t *args;

    /* File name to be loaded */
    const char *filename;

    /*
     * If 'file_ext' is true, add file extension to filename if the file
     * does not have the default file extension.
     */
    bool file_ext;

    /*
     * If 'err_stop' is false, all the commands in the script file
     * will be executed without returning on errors.
     */
    bool err_stop;

    /* File handle of script file. Non-NULL if file is opened from file I/O */
    bcma_io_file_handle_t fh;

    /*
     * Pointer to file cache content.
     * Non-NULL if file is retrieved from cache.
     */
    const char *fcache;

    /* Duplicated file name (with file extension examined) to be loaded */
    char *fname;

    /* Command line buffer for the loaded file */
    char *cmd;

    /* Arguments for the command line buffer for the loaded file */
    bcma_cli_args_t *cmd_args;

} rcload_info_t;

/*
 * Function to parse and execute the commands in script file.
 */
static int
do_rcload_ctrlc(void *data)
{
    rcload_info_t *ri = (rcload_info_t *)data;
    char *add_ext = "";
    const char *fcache, *pcache;
    char *fname;
    bcma_io_file_handle_t fh = NULL;
    char *cmd;
    int cmd_size;
    char *ptr, *c;
    int len, rv = BCMA_CLI_CMD_OK, line_num = 0;
    bool rcerr_stop = ri->err_stop;
    int done = FALSE;
    bcma_cli_args_t *cmd_args;

    /* Add to local scope variables if there exist arguments. */
    if (ri->args && BCMA_CLI_ARG_CNT(ri->args) > 0) {
        int var;
        char var_name[12];
        char *var_val;

        for (var = 1, var_val = BCMA_CLI_ARG_GET(ri->args); var_val != NULL;
             var++, var_val = BCMA_CLI_ARG_GET(ri->args)) {
            sal_sprintf(var_name, "%d", var);
            bcma_cli_var_set(ri->cli, var_name, var_val, TRUE);
        }
    }

    /* Check if file extension needs to be added to filename */
    if (ri->file_ext) {
        int ext_slen = sal_strlen(BCMA_CLICMD_RCLOAD_FILE_EXT);
        int file_slen = sal_strlen(ri->filename);

        if (file_slen <= ext_slen ||
            sal_strcmp(ri->filename + file_slen - ext_slen,
                       BCMA_CLICMD_RCLOAD_FILE_EXT) != 0) {
            add_ext = BCMA_CLICMD_RCLOAD_FILE_EXT;
        }
    }

    /* Format file name to 'fname' */
    fname = sal_alloc(sal_strlen(ri->filename) + 20, "bcmaCliRcloadFilename");
    if (fname == NULL) {
        cli_out("%sFailed to allocate for rcload file %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, ri->filename);
        return BCMA_CLI_CMD_FAIL;
    }
    sal_strcpy(fname, ri->filename);
    sal_strcat(fname, add_ext);

    /* Check if the file has been cached already. */
    fcache = bcma_clicmd_rccache_lock(fname);
    if (fcache == NULL) {
        /* Try to open the file if it's not cached. */
        fh = bcma_clicmd_rccache_file_open(ri->cli, fname);
        if (fh == NULL) {
            sal_free(fname);
            return BCMA_CLI_CMD_NOT_FOUND;
        }
    }

    cmd_size = BCMA_CLI_CONFIG_ARGS_BUFFER_MAX;
    cmd = sal_alloc(cmd_size, "bcmaCliRcloadCmd");
    if (cmd == NULL) {
        sal_free(fname);
        if (fh) {
            bcma_io_file_close(fh);
        }
        if (fcache) {
            bcma_clicmd_rccache_unlock(fname);
        }
        cli_out("%sFailed to allocate memory for rcload_cmd\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    /*
     * Need a new CLI command arguments for processing the commands within
     * the 'rcload' command arguments. The 'rcload' command arguements can not
     * be reused in case the parsed arguments are messed up.
     */
    cmd_args = sal_alloc(sizeof(*cmd_args), "bcmaCliRclodCmdArgs");
    if (cmd_args == NULL) {
        sal_free(fname);
        if (fh) {
            bcma_io_file_close(fh);
        }
        if (fcache) {
            bcma_clicmd_rccache_unlock(fname);
        }
        sal_free(cmd);
        cli_out("%sFailed to allocate memory for rcload_cmd arguments\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Store the resources needed to be freed in case interrupted by ctrl-c */
    ri->fh = fh;
    ri->fcache = fcache;
    ri->fname = fname;
    ri->cmd = cmd;
    ri->cmd_args = cmd_args;

    pcache = fcache;

    while (!done) {
        /* Get each command in script file */
        ptr = cmd;
        *ptr = '\0';
        while (1) {
            /* Read each line from file or cache to 'ptr' */
            if (fh) {
                /* From file I/O */
                if (bcma_io_file_gets(fh, ptr, cmd_size + cmd - ptr) == NULL) {
                    done = TRUE;
                }
            } else {
                /* From cache */
                c = ptr;
                while (c != (cmd + cmd_size - 1)) {
                    *c++ = *pcache;
                    if (*pcache == '\0') {
                        done = TRUE;
                        break;
                    }
                    if (*pcache == '\n') {
                        *c++ = '\0';
                        pcache++;
                        break;
                    }
                    pcache++;
                }
            }
            if (!done) {
                line_num++;
            }

            /* Strip out comments */
            if ((c = sal_strchr(ptr, '#')) != NULL) {
                *c = '\0';
            }

            /* Remove newline */
            if ((c = sal_strrchr(ptr, '\n')) != NULL) {
                *c = '\0';
            }

            /* Remove carriage return */
            if ((c = sal_strrchr(ptr, '\r')) != NULL) {
                *c = '\0';
            }

            /* Check backslash for continuation */
            len = sal_strlen(ptr);
            if (len > 0 && ptr[len - 1] == '\\') {
                ptr += len - 1;
                /* Concatenate the following line to cmd */
                continue;
            }

            break;
        }

        /* Skip for blank space command line */
        c = cmd;
        while (*c != '\0') {
            if (!sal_isspace(*c)) {
                break;
            }
            c++;
        }
        if (*c == '\0') {
            continue;
        }

        /* Process each command in script file */
        rv = bcma_cli_cmd_args_process(ri->cli, cmd, cmd_args);
        bcma_cli_var_result_set(ri->cli, rv);

        if (rv == BCMA_CLI_CMD_EXIT) {
            /* An 'exit' command in script is normal completion of script. */
            rv = BCMA_CLI_CMD_OK;
            break;
        } else if (rv == BCMA_CLI_CMD_INTR) {
            break;
        } else if (rv != BCMA_CLI_CMD_OK) {
            if (sal_strncasecmp("expr", cmd, 4) != 0) {
                cli_out("%sFile %s: line %d (error code %d): script %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, fname, line_num, rv,
                        rcerr_stop ? "terminated" : "continuing");

                if (rcerr_stop) {
                    rv = BCMA_CLI_CMD_FAIL;
                    break;
                }
            }
        }
    }

    if (line_num == 0) {
        cli_out("%sFile %s: empty script\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        rv = BCMA_CLI_CMD_FAIL;
    }

    if (fh) {
        bcma_io_file_close(fh);
        ri->fh = NULL;
    }

    if (fcache) {
        bcma_clicmd_rccache_unlock(fname);
        ri->fcache = NULL;
    }

    sal_free(cmd_args);
    ri->cmd_args = NULL;
    sal_free(cmd);
    ri->cmd = NULL;
    sal_free(fname);
    ri->fname = NULL;

    return rv;
}

/*
 * This function is to Execute CLI commands in a script file.
 * File extension will be checked(appended) if 'file_ext' is TRUE.
 */
static int
rcload_file(bcma_cli_t *cli, bcma_cli_args_t *args,
            const char *filename, bool file_ext)
{
    int rv;
    rcload_info_t rcload_info, *ri = &rcload_info;

    sal_memset(ri, 0, sizeof(*ri));
    ri->cli = cli;
    ri->args = args;
    ri->filename = filename;
    ri->file_ext = file_ext;
    ri->err_stop = (bcma_cli_var_bool_get(cli, BCMA_CLICMD_RCERROR, TRUE) != 0);

    bcma_cli_var_scope_push(cli);
    bcma_cli_var_result_set(cli, 0);

    rv = bcma_cli_ctrlc_exec(cli, do_rcload_ctrlc, ri, 1);

    bcma_cli_var_scope_pop(cli);

    /* Interrupted by Ctrl-C */
    if (rv == BCMA_CLI_CMD_INTR) {
        /*
         * For the case that the command in execution does not
         * support ctrl-c, we need to free the allocated resources
         * in do_rcload here.
         */
        if (ri->fh) {
            bcma_io_file_close(ri->fh);
        }

        if (ri->fcache && ri->fname) {
            bcma_clicmd_rccache_unlock(ri->fname);
        }

        if (ri->fname) {
            sal_free(ri->fname);
        }

        if (ri->cmd) {
            sal_free(ri->cmd);
        }

        if (ri->cmd_args) {
            sal_free(ri->cmd_args);
        }
    }

    return rv;
}

int
bcma_clicmd_rcload_file(bcma_cli_t *cli, bcma_cli_args_t *args,
                        const char *filename, bool auto_ext)
{
    int rv = BCMA_CLI_CMD_NOT_FOUND;
    int rc_load = bcma_cli_var_bool_get(cli, BCMA_CLICMD_RCLOAD, TRUE);

    if (rc_load) {
        rv = rcload_file(cli, args, filename, auto_ext);
    }

    return rv;
}

int
bcma_clicmd_rcload(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *filename;
    int rv;

    filename = BCMA_CLI_ARG_GET(args);
    if (filename == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    rv = rcload_file(cli, args, filename, false);
    if (rv == BCMA_CLI_CMD_NOT_FOUND) {
        cli_out("%s%s: file not found: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args),
                filename);
        rv = BCMA_CLI_CMD_FAIL;
    }

    return rv;
}
