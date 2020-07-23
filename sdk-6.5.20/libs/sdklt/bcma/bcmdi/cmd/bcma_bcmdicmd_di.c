/*! \file bcma_bcmdicmd_di.c
 *
 * CLI commands related to firmware loader.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmdi/bcma_bcmdicmd_di.h>
#include <bcmdi/bcmdi.h>

static int
cmd_devicecode_fileinfo_pb_gen(int unit, bcmdi_dvc_file_info_t *fi,
                               shr_pb_t *pb)
{
    if (fi) {
        shr_pb_printf(pb, "Devicecode File Info:\n");
        shr_pb_printf(pb, "\tChip:         %s_%s\n" ,
                      fi->chip_id, fi->chip_rev);
        shr_pb_printf(pb, "\tIdentifier:   0x%08X\n", fi->fid);
        shr_pb_printf(pb, "\tDescription:  %s\n", fi->desc);
        return BCMA_CLI_CMD_OK;
    }
    return BCMA_CLI_CMD_BAD_ARG;
}

static int
cmd_devicecode_status_pb_gen(int unit, bcmdi_dvc_status_t *status, shr_pb_t *pb)
{
    shr_pb_printf(pb, "DeviceCode Status:\n");
    shr_pb_printf(pb, "\tLoaded:       %s\n" , status->loaded? "YES": "NO");
    if (status->loaded) {
        cmd_devicecode_fileinfo_pb_gen(unit, &status->file_info, pb);
        shr_pb_printf(pb, "\n");
    }
    return BCMA_CLI_CMD_OK;
}

static int
cmd_devicecode_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int unit = cli->cmd_unit;
    bcmdi_dvc_status_t *status;
    shr_pb_t *pb;

    rv = bcmdi_devicecode_status_get(unit, &status);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to retrieve DeviceCode status (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    pb = shr_pb_create();
    cmd_devicecode_status_pb_gen(unit, status, pb);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_flexcode_fileinfo_pb_gen(int unit, bcmdi_flc_file_info_t *fi,
                             shr_pb_t *pb)
{
    if (fi) {
        shr_pb_printf(pb, "Flexcode File Info:\n");
        shr_pb_printf(pb, "\tChip:         %s_%s\n" , fi->chip_id, fi->chip_rev);
        shr_pb_printf(pb, "\tApplication:  %s\n", fi->app);
        shr_pb_printf(pb, "\tVersion:      %s\n", fi->app_ver);
        shr_pb_printf(pb, "\tBuild Time:   %s\n", fi->app_build_time);
        shr_pb_printf(pb, "\tIdentifier:   %s\n", fi->fid);
        shr_pb_printf(pb, "\tProvider:     %s\n", fi->pid);
        shr_pb_printf(pb, "\tNCS Version:  %s\n", fi->ncs_ver);
        shr_pb_printf(pb, "\tDescription:  %s\n", fi->desc);
        return BCMA_CLI_CMD_OK;
    }
    return BCMA_CLI_CMD_BAD_ARG;
}

static int
cmd_flexcode_status_pb_gen(int unit, bcmdi_flc_status_t *status, shr_pb_t *pb)
{
    shr_pb_printf(pb, "Flexcode Status:\n");
    shr_pb_printf(pb, "\tLoaded:       %s\n" , status->loaded? "YES": "NO");
    if (status->loaded) {
        cmd_flexcode_fileinfo_pb_gen(unit, &status->file_info, pb);
        shr_pb_printf(pb, "\n");
    }
    return BCMA_CLI_CMD_OK;
}

static int
cmd_flexcode_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bcmdi_flc_status_t *status;
    shr_pb_t *pb;

    rv = bcmdi_flexcode_status_get(unit, &status);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to retrieve flexcode status (%d)\n", rv);
        return BCMA_CLI_CMD_FAIL;
    }
    pb = shr_pb_create();
    cmd_flexcode_status_pb_gen(unit, status, pb);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_flexcode_list_pb_gen(int unit, const uint8_t file_count,
                         const bcmdi_flc_file_list_t *file_list, shr_pb_t *pb)
{
    int i;
    shr_pb_printf(pb, "Available FlexCode types:\n");

    for (i = 0; i < file_count; i++) {
        shr_pb_printf(pb, "  %s\n", file_list[i].type);
    }
    return BCMA_CLI_CMD_OK;
}

static int
cmd_flexcode_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    shr_pb_t *pb;
    uint8_t file_count;
    const bcmdi_flc_file_list_t *file_list;

    if (SHR_FAILURE(bcmdi_flexcode_file_list_get(unit, &file_count, &file_list)))
        return BCMA_CLI_CMD_FAIL;

    pb = shr_pb_create();
    cmd_flexcode_list_pb_gen(unit, file_count, file_list, pb);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static int
cmd_flexcode_debug_switch_pb_gen(int unit, bcmdi_flc_debug_switch_t *dbg,
                                 shr_pb_t *pb)
{
    if (dbg) {
        shr_pb_printf(pb, "FlexCode debug switch (unit %d):\n", unit);
        shr_pb_printf(pb, "\tPre-FlexCode Verify:  %d\n", dbg->pre_verify);
        shr_pb_printf(pb, "\tPost-FlexCode Verify: %d\n", dbg->post_verify);
        return BCMA_CLI_CMD_OK;
    }
    return BCMA_CLI_CMD_BAD_ARG;
}

static int
cmd_flexcode_debug(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    char *cmd;

    bcmdi_flc_debug_switch_t dbg_switch = {0};
    cmd = BCMA_CLI_ARG_GET(args);

    if (!cmd || bcma_cli_parse_cmp("Show", cmd, 0)) {
        shr_pb_t *pb;
        bcmdi_flexcode_debug_switch_get(unit, &dbg_switch);
        pb = shr_pb_create();
        cmd_flexcode_debug_switch_pb_gen(unit, &dbg_switch, pb);
        cli_out("%s", shr_pb_str(pb));
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("All", cmd, 0)) {
        dbg_switch.pre_verify = TRUE;
        dbg_switch.post_verify = TRUE;
        cli_out("Turn on all FlexCode debug switches.\n");
    } else if (bcma_cli_parse_cmp("Verify", cmd, 0)) {
        dbg_switch.pre_verify = TRUE;
        dbg_switch.post_verify = TRUE;
        cli_out("Turn on FlexCode debug: Verify.\n");
    } else if (bcma_cli_parse_cmp("Clear", cmd, 0)) {
        dbg_switch = (bcmdi_flc_debug_switch_t) {0};
        cli_out("Turn off all FlexCode debug options.\n");
    } else {
        cli_out("Undefined debug option: '%s'\n", cmd);
        return BCMA_CLI_CMD_FAIL;
    }

    if (SHR_FAILURE
            (bcmdi_flexcode_debug_switch_set(cli->cmd_unit, dbg_switch))) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

static int
cmd_flexcode(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    int rv;
    char *cmd;

    cmd = BCMA_CLI_ARG_GET(args);

    if (cmd && bcma_cli_parse_cmp("Debug", cmd, 0)) {
        return cmd_flexcode_debug(cli, args);
    }

    if (bcmdi_flexcode_is_supported(unit) == SHR_E_INIT) {
        cli_out("FlexCode driver is not initialized yet.\n");
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmdi_flexcode_is_supported(unit) == FALSE) {
        cli_out("This device doesn't support FlexCode.\n");
        return BCMA_CLI_CMD_OK;
    }

    if (!cmd || bcma_cli_parse_cmp("Info", cmd, 0)) {
        return cmd_flexcode_info(cli, args);
    }

    if (!cmd || bcma_cli_parse_cmp("LIst", cmd, 0)) {
        return cmd_flexcode_list(cli, args);
    }

    if (bcma_cli_parse_cmp("LoaD", cmd, 0)) {
        char *type = BCMA_CLI_ARG_GET(args);
        if (!type) {
            type = "DEFAULT";
        }
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_FLEXCODE)) {
            rv = bcmdi_flexcode_load(unit, type, BCMPTM_DIRECT_TRANS_ID, false);
            if (SHR_SUCCESS(rv)) {
                cli_out("Flexcode (Type: %s) was loaded.\n", type);
                return BCMA_CLI_CMD_OK;
            } else if (rv == SHR_E_NOT_FOUND) {
                cli_out("Flexcode type %s is not valid.\n", type);
            } else {
                cli_out("Failed to load Flexcode type %s!\n", type);
            }
            return BCMA_CLI_CMD_FAIL;
        } else {
            cli_out("FlexCode feature was disabled.\n");
        }
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_cmp("Verify", cmd, 0)) {
        int mode;
        cmd = BCMA_CLI_ARG_GET(args);
        if (cmd) {
            if (bcma_cli_parse_cmp("ResetVal", cmd, 0)) {
                mode = BCMDI_FLC_VERIFY_MODE_RESETVAL;
            } else if (bcma_cli_parse_cmp("DataBase", cmd, 0)) {
                mode = BCMDI_FLC_VERIFY_MODE_DATABASE;
            } else {
                cli_out("Undefined verify mode: '%s'\n", cmd);
                return BCMA_CLI_CMD_FAIL;
            }
        } else {
            cli_out("Please specify verify mode.\n");
            return BCMA_CLI_CMD_FAIL;
        }
        if (SHR_FAILURE(bcmdi_flexcode_hw_verify(cli->cmd_unit, "DEFAULT", mode)))
            return BCMA_CLI_CMD_FAIL;
        cli_out("Flexcode verify completed.\n");
        return BCMA_CLI_CMD_OK;
    }

    cli_out("%sUnknown FlexCode operation: '%s'\n",
            BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

static int
cmd_devicecode(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;

    if (!(cmd = BCMA_CLI_ARG_GET(args)) ||
        bcma_cli_parse_cmp("Info", cmd, 0)) {
        return cmd_devicecode_info(cli, args);
    }

    if (bcma_cli_parse_cmp("LoaD", cmd, 0)) {
        cli_out("DeviceCode operation: '%s' is not supported yet.\n", cmd);
        return BCMA_CLI_CMD_OK;
    }

    cli_out("%sUnknown DeviceCode operation: %s\n",
            BCMA_CLI_CONFIG_ERROR_STR, cmd);
    return BCMA_CLI_CMD_FAIL;
}

int
bcma_bcmdicmd_di(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *cmd;
    bcma_cmd_result_t rv = BCMA_CLI_CMD_BAD_ARG;

    if (!(cmd = BCMA_CLI_ARG_GET(args))) {
        return BCMA_CLI_CMD_USAGE;
    }

    do {
        if (bcma_cli_parse_cmp("All", cmd, 0) ||
            bcma_cli_parse_cmp("DeViceCode", cmd, 0)) {
            rv = cmd_devicecode(cli, args);
            if (rv != BCMA_CLI_CMD_OK)
                break;
        }

        if (bcma_cli_parse_cmp("All", cmd, 0) ||
            bcma_cli_parse_cmp("FLexCode", cmd, 0)) {
            rv = cmd_flexcode(cli, args);
            if (rv != BCMA_CLI_CMD_OK)
                break;
        }
    } while (0);

    if (rv == BCMA_CLI_CMD_BAD_ARG) {
        cli_out("%sUnknown component: %s\n", BCMA_CLI_CONFIG_ERROR_STR, cmd);
    }
    return rv;
}

