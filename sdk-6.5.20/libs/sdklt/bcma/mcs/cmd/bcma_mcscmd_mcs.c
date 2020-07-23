/*! \file bcma_mcscmd_mcs.c
 *
 * CLI Micro Controler Subsystem shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_pb_format.h>
#include <shr/shr_util.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_mcs.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/mcs/bcma_mcs.h>
#include <bcma/mcs/bcma_mcscmd_mcs.h>

static int
mcs_cmd_load(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    uint32_t flags = 0;
    int rv, uc = -1, startuc = 1, startmsg = 1, autocfg = 1;
    int sramsize = 0, sramuncsize = 0, hostramsize = 0;
    int num_sbusdma = 0, num_pktdma = 0, num_schanfifo = 0;
    const char *fname;
    bcmbd_mcs_res_req_t req;
    bcma_cli_parse_table_t pt;

    fname = BCMA_CLI_ARG_GET(args);
    if (fname == NULL) {
        return bcma_cli_parse_error("FileName", fname);
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "StartUC", "bool", &startuc, NULL);
    bcma_cli_parse_table_add(&pt, "StartMSG", "bool", &startmsg, NULL);
    bcma_cli_parse_table_add(&pt, "AutoConFiG", "bool", &autocfg, NULL);
    bcma_cli_parse_table_add(&pt, "UC", "int", &uc, NULL);
    /* Resource requests*/
    bcma_cli_parse_table_add(&pt, "SRamSiZe", "int", &sramsize, NULL);
    bcma_cli_parse_table_add(&pt, "SRamUncSiZe", "int", &sramuncsize, NULL);
    bcma_cli_parse_table_add(&pt, "HostRamSiZe", "int", &hostramsize, NULL);
    bcma_cli_parse_table_add(&pt, "NumSbusDMA", "int", &num_sbusdma, NULL);
    bcma_cli_parse_table_add(&pt, "NumPktDMA", "int", &num_pktdma, NULL);
    bcma_cli_parse_table_add(&pt, "NumSchanFIFO", "int", &num_schanfifo, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (startuc) {
        flags |= BCMA_MCS_LOAD_FLAG_STARTUC;
    }
    if (startmsg) {
        flags |= BCMA_MCS_LOAD_FLAG_STARTMSG;
    }
    if (autocfg) {
        flags |= BCMA_MCS_LOAD_FLAG_AUTO_CONFIG;
    }
    if (uc < 0) {
        flags |= BCMA_MCS_LOAD_FLAG_AUTO_CORE;
    }

    sal_memset(&req, 0, sizeof(bcmbd_mcs_res_req_t));
    req.sram_size = sramsize;
    req.sram_unc_size = sramuncsize;
    req.hostram_size = hostramsize;
    req.num_sbusdma = num_sbusdma;
    req.num_pktdma = num_pktdma;
    req.num_schanfifo = num_schanfifo;

    rv = bcma_mcs_load(cli->cmd_unit, uc, fname, &req, flags);

    if (rv == BCMA_MCS_FILE_NOT_FOUND) {
        cli_out("MCS load error: file not found: %s\n", fname);
    } else if (rv == BCMA_MCS_NO_RESOURCES) {
        cli_out("MCS load error: Failed to reserve resources\n");
    } else if (rv == BCMA_MCS_FAIL) {
        cli_out("MCS load error: Generic failure\n");
    }

    return (rv == BCMA_MCS_OK) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
mcs_cmd_status(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv, uc, showcfg = 0, showdbg = 0;
    bcma_cli_parse_table_t pt;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        uc = -1; /* All uC's */
    } else if (bcma_cli_parse_int(arg, &uc) < 0) {
        uc = -1; /* All uC's */
        BCMA_CLI_ARG_PREV(args);
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "ShowConFiG", "bool", &showcfg, NULL);
    bcma_cli_parse_table_add(&pt, "ShowDeBuG", "bool", &showdbg, NULL);
    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    rv = bcma_mcs_status(cli->cmd_unit, uc, showcfg, showdbg);

    return (rv == BCMA_MCS_OK) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
mcs_cmd_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    uint32_t uc;

    cli_out("mcs dump command\n");

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_cli_parse_uint32(arg, &uc) < 0) {
        return bcma_cli_parse_error("MCS instance", arg);
    }
    return BCMA_CLI_CMD_OK;
}

static int
mcs_cmd_console(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int uc;
    const char *subcmd;
    char prompt[16];
    bool enabled = false;
    uint32_t buf[32];
    char *bufp = (char *)&buf[0];

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_cli_parse_int(arg, &uc) < 0) {
        return bcma_cli_parse_error("MCS instance", arg);
    }

    subcmd = BCMA_CLI_ARG_GET(args);
    if (subcmd != NULL) {
        if (sal_strcasecmp("start", subcmd) == 0) {
            bcmbd_mcs_con_mode_set(cli->cmd_unit, uc, true);
            return BCMA_CLI_CMD_OK;
        } else if (sal_strcasecmp("stop", subcmd) == 0) {
            bcmbd_mcs_con_mode_set(cli->cmd_unit, uc, false);
            return BCMA_CLI_CMD_OK;
        } else if (sal_strcasecmp("cmd", subcmd) == 0) {
            if (args->argc == 5) {
                arg = BCMA_CLI_ARG_GET(args);
                bcmbd_mcs_con_cmd_send(cli->cmd_unit, uc, (char *)arg, 0);
            } else {
                cli_out("Give exactly one command\n");
                cli_out("If multiple words, enclose them within quotes\n");
            }
            return BCMA_CLI_CMD_OK;
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    }

    sal_snprintf(prompt, sizeof(prompt), "mcs.%d.%d>", cli->cmd_unit, uc);
    bcmbd_mcs_con_mode_get(cli->cmd_unit, uc, &enabled);

    if (!enabled) {
        bcmbd_mcs_con_mode_set(cli->cmd_unit, uc, true);
    }

    do {
        if (bcma_cli_gets(cli, prompt, sizeof(buf), bufp) == BCMA_CLI_CMD_OK)
        {
            if ((sal_strcasecmp(bufp, "quit") == 0) ||
                (sal_strcasecmp(bufp, "exit") == 0)) {
                if (!enabled) {
                    bcmbd_mcs_con_mode_set(cli->cmd_unit, uc, false);
                }
                return BCMA_CLI_CMD_OK;
            }
            bcmbd_mcs_con_cmd_send(cli->cmd_unit, uc, bufp, 500);
        }
    } while (1);

    return BCMA_CLI_CMD_OK;
}

static int
mcs_cmd_message(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int uc = -1, action, rv;
    const char *c;

    c = BCMA_CLI_ARG_GET(args);
    if (c == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp("init", c) == 0) {
        action = BCMA_MCS_MSG_ACT_INIT;
    } else if (sal_strcasecmp("start", c) == 0) {
        action = BCMA_MCS_MSG_ACT_START;
    } else if (sal_strcasecmp("stop", c) == 0) {
        action = BCMA_MCS_MSG_ACT_STOP;
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    if ((action == BCMA_MCS_MSG_ACT_START) ||
        (action == BCMA_MCS_MSG_ACT_STOP)) {
        c = BCMA_CLI_ARG_GET(args);
        if (c == NULL || bcma_cli_parse_int(c, &uc) < 0) {
            return bcma_cli_parse_error("MCS instance", c);
        }
    }

    rv = bcma_mcs_message(cli->cmd_unit, uc, action);

    return (rv == BCMA_MCS_OK) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
mcs_cmd_rm(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int uc, type;
    const char *c;
    bcmbd_ukernel_config_t cfg;
    shr_pb_t *pb;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL || bcma_cli_parse_int(arg, &uc) < 0) {
        return bcma_cli_parse_error("MCS instance", arg);
    }
    c = BCMA_CLI_ARG_GET(args);
    if (c != NULL) {
        if (sal_strcasecmp("show", c) == 0) {
            c = BCMA_CLI_ARG_GET(args);
            if (c == NULL) {
                return BCMA_CLI_CMD_USAGE;
            }
            if (sal_strcasecmp("used", c) == 0) {
                type = BCMBD_MCS_RM_RESOURCES_USED;
                cli_out("Resources used by uC-%d\n", uc);
            } else if (sal_strcasecmp("free", c) == 0) {
                type = BCMBD_MCS_RM_RESOURCES_FREE;
                cli_out("Resources available for uC-%d\n", uc);
            } else if (sal_strcasecmp("all", c) == 0) {
                type = BCMBD_MCS_RM_RESOURCES_ALL;
                cli_out("Resources accessible by uC-%d (used or free)\n", uc);
            } else {
                return BCMA_CLI_CMD_USAGE;
            }

            if (bcmbd_mcs_rm_get_resources(cli->cmd_unit, uc, type, &cfg, 0) != SHR_E_NONE) {
                cli_out("Error getting RM data\n");
                return BCMA_CLI_CMD_FAIL;
            }
            if (type == BCMBD_MCS_RM_RESOURCES_USED) {
                pb = shr_pb_create();
                shr_pb_format_uint64(pb, NULL, &(cfg.hostram_paddr), 1, 64);
                cli_out("\tHostRAM base\t\t%s\n", shr_pb_str(pb));
                shr_pb_destroy(pb);
                cli_out("\tHostRAM size\t\t0x%"PRIx32"\n", cfg.hostram_size);
                cli_out("\tSRAM base\t\t0x%"PRIx32"\n", cfg.sram_base);
                cli_out("\tSRAM uncached size\t0x%"PRIx32"\n", cfg.sram_unc_size);
            }
            cli_out("\tSRAM size\t\t0x%"PRIx32"\n", cfg.sram_size);
            cli_out("\tSBUSDMA channels\t0x%"PRIx32"\n", cfg.sbusdma_bmp);
            cli_out("\tPKTDMA channels\t\t0x%"PRIx32"\n", cfg.pktdma_bmp);
            cli_out("\tSCHANFIFO channels\t0x%"PRIx32"\n", cfg.schanfifo_bmp);
            return BCMA_CLI_CMD_OK;
        } else if (sal_strcasecmp("release", c) == 0) {
            if (bcmbd_mcs_rm_release(cli->cmd_unit, uc) != SHR_E_NONE) {
                cli_out("Error freeing RM data\n");
                return BCMA_CLI_CMD_FAIL;
            }
            return BCMA_CLI_CMD_OK;
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    }
    return BCMA_CLI_CMD_USAGE;
}

static bcma_cli_command_t mcs_cmds[] = {
    {"load", mcs_cmd_load},
    {"status", mcs_cmd_status},
    {"dump", mcs_cmd_dump},
    {"console", mcs_cmd_console},
    {"message", mcs_cmd_message},
    {"rm", mcs_cmd_rm}
};


int
bcma_mcscmd_mcs(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *cmd;
    int idx;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    cmd = BCMA_CLI_ARG_CUR(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0; idx < COUNTOF(mcs_cmds); idx++) {
        if (bcma_cli_parse_cmp(mcs_cmds[idx].name, cmd, 0)) {
            BCMA_CLI_ARG_NEXT(args);
            return (*mcs_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}

