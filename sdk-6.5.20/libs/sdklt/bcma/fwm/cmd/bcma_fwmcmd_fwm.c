/*! \file bcma_fwmcmd_fwm.c
 *
 *  CLI command for Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_pb.h>
#include <shr/shr_pb_format.h>
#include <bcmbd/bcmbd_fwm.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/fwm/bcma_fwmcmd_fwm.h>
#include <bcma/io/bcma_io_file.h>

/* Modes of processor memory access width. */
#define MEM_MODE_B 0 /* Byte mode. */
#define MEM_MODE_H 1 /* Half-word mode. */
#define MEM_MODE_W 2 /* Word mode. */

/* Memory width parameter for command read/write/dump */
static bcma_cli_parse_enum_t mem_mode_enum[] = {
    { "x1", MEM_MODE_B },
    { "x2", MEM_MODE_H },
    { "x4", MEM_MODE_W },
    { NULL, 0 }
};

/*
 * Number of data displayed in a row.
 * Depending on the Byte, Half-word and Word modes selected.
 */
#define DUMP_MW_COLS 4
#define DUMP_MH_COLS 8
#define DUMP_MB_COLS 16

/*
 * Function for sub-command 'query'.
 */
static int
fwm_cmd_query(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    const char *fname;
    bcma_io_file_handle_t fh;
    int fsize;
    uint8_t *buffer;
    int fpkg_id;
    bcmbd_fwm_fwinfo_t fwinfo;
    char prop_name[32];
    char prop_val[32];
    int i;
    uint32_t dev_id;
    uint32_t dev_id_hi_bits;
    int op_cnt = 0;
    bcmbd_fwm_fw_op_t op;
    shr_pb_t *pb;

    arg = BCMA_CLI_ARG_GET(args);

    if (arg == NULL) {
        cli_out("No firmware load parameter specified.\n");
        return BCMA_CLI_CMD_USAGE;
    }

    fname = arg;

    /* Load firmware. */
    fh = bcma_io_file_open(fname, "r");

    if (fh == 0) {
        cli_out("%s: Failed to open file %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        return BCMA_CLI_CMD_FAIL;
    }

    fsize = bcma_io_file_size(fh);
    if (fsize <= 0) {
        cli_out("%sInvalid file size: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        bcma_io_file_close(fh);
        return BCMA_CLI_CMD_FAIL;
    }

    buffer = sal_alloc(fsize, "bcmaFwmFileBuffer");
    if (buffer == NULL) {
        cli_out("%s%s\n", BCMA_CLI_CONFIG_ERROR_STR,
                          shr_errmsg(SHR_E_MEMORY));
        bcma_io_file_close(fh);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_io_file_read(fh, (void *)buffer, 1, fsize) != fsize) {
        cli_out("%s: failed to read file %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_fpkg_open(buffer, fsize, &fpkg_id) < 0) {
        cli_out("Failed to open firmware package in file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_fpkg_info_get(fpkg_id, &fwinfo) < 0) {
        cli_out("Failed to get firmware package info of file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();

    /* Dump firmware package info. */
    shr_pb_printf(pb, "%-29s %s\n", "Firmware:", fwinfo.fw_type);
    shr_pb_printf(pb, "%-29s %s\n", "Version:", fwinfo.fw_version);
    shr_pb_printf(pb, "%-29s", "Supported chips:");
    for (i = 0; i < BCMBD_FWM_FWINFO_CHIPS_MAX; i++) {
        if (fwinfo.chips[i]) {
            dev_id = fwinfo.chips[i] & 0xfff;
            dev_id_hi_bits = (fwinfo.chips[i] & 0xf000) >> 12;
            dev_id_hi_bits = (dev_id_hi_bits - 5) | 0x50;
            dev_id = (dev_id_hi_bits << 12) | dev_id;
            shr_pb_printf(pb, " BCM%x", dev_id);
        }
    }
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "%-29s %s\n", "Default processor:",
                  fwinfo.default_processor);
    shr_pb_printf(pb, "%-29s ", "Default address:");
    shr_pb_format_uint64(pb, "", &fwinfo.default_address, 1, 0);
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "%-29s %s\n", "Image format:",
                  fwinfo.image_format);
    shr_pb_printf(pb, "%-29s %s\n", "Signature type:",
                  fwinfo.signature_type);

    shr_pb_printf(pb, "Properties:\n");
    if (fwinfo.num_properties == 0) {
        shr_pb_printf(pb, "     NA\n");
    } else {
        for (i = 0; i < fwinfo.num_properties; i++) {
            sal_memset(prop_name, 32, sizeof(char));
            sal_memset(prop_val, 32, sizeof(char));
            if (bcmbd_fwm_fpkg_prop_key_get(fpkg_id, i, 32, prop_name) < 0) {
                cli_out("Failed to get firmware property key #%d.\n", i);
                bcma_io_file_close(fh);
                sal_free(buffer);
                shr_pb_destroy(pb);
                return BCMA_CLI_CMD_FAIL;
            }
            if (bcmbd_fwm_fpkg_prop_val_get(fpkg_id, i, 32, prop_val) < 0) {
                cli_out("Failed to get firmware property value #%d.\n", i);
                bcma_io_file_close(fh);
                sal_free(buffer);
                shr_pb_destroy(pb);
                return BCMA_CLI_CMD_FAIL;
            }
            shr_pb_printf(pb, "     %-25s%s\n", prop_name, prop_val);
        }
    }

    shr_pb_printf(pb, "Operations supported:\n");
    if (bcmbd_fwm_fw_op_count_get(cli->cmd_unit, fwinfo.fw_type, &op_cnt) < 0) {
        cli_out("Failed to get firmware operations.\n");
        bcma_io_file_close(fh);
        sal_free(buffer);
        shr_pb_destroy(pb);
        return BCMA_CLI_CMD_FAIL;
    }

    for (i = 0; i < op_cnt; i++) {
        if (bcmbd_fwm_fw_op_get(cli->cmd_unit, fwinfo.fw_type, i, &op) < 0) {
            cli_out("Failed to get firmware operation #%d.\n", i);
            bcma_io_file_close(fh);
            sal_free(buffer);
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }

        shr_pb_printf(pb, "     %-25s%s\n", op.op_name, op.op_help);
    }

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    if (bcmbd_fwm_fpkg_close(fpkg_id) < 0) {
        cli_out("Failed to close firmware package file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_io_file_close(fh);
    sal_free(buffer);

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for sub-command 'load'.
 */
static int
fwm_cmd_load(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg = NULL;
    char *proc_name = NULL;
    char *save_str = NULL;
    const char *fname = NULL;
    bcma_io_file_handle_t fh;
    int fsize;
    uint8_t *buffer;
    int fpkg_id;
    int inst_id;
    char fw_inst[BCMBD_FWM_FWINST_FW_NAME_MAX];
    bcmbd_fwm_fwinfo_t fwinfo;
    char *op_name;
    char *op_param;
    bcma_cli_parse_table_t pt;

    /* Processor=<procname>[.<inst>] */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Processor", "str", &(proc_name), NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    fname = BCMA_CLI_ARG_GET(args);

    /* Load firmware. */
    fh = bcma_io_file_open(fname, "r");

    if (fh == 0) {
        cli_out("%sFile open fail: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    fsize = bcma_io_file_size(fh);
    if (fsize <= 0) {
        cli_out("%sInvalid file size: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        bcma_io_file_close(fh);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    buffer = sal_alloc(fsize, "bcmaFwmFileBuffer");
    if (buffer == NULL) {
        cli_out("%s%s\n", BCMA_CLI_CONFIG_ERROR_STR,
                          shr_errmsg(SHR_E_MEMORY));
        bcma_io_file_close(fh);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_io_file_read(fh, (void *)buffer, 1, fsize) != fsize) {
        cli_out("%s: failed to read file %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_fpkg_open(buffer, fsize, &fpkg_id) < 0) {
        cli_out("Failed to open firmware package file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_fpkg_load(cli->cmd_unit, fpkg_id, proc_name, &inst_id) < 0) {
        cli_out("Failed to load firmware package file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_fpkg_info_get(fpkg_id, &fwinfo) < 0) {
        cli_out("Failed to get firmware package info of file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_fpkg_close(fpkg_id) < 0) {
        cli_out("Failed to close firmware package file %s.\n", fname);
        bcma_io_file_close(fh);
        sal_free(buffer);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_io_file_close(fh);
    sal_free(buffer);
    bcma_cli_parse_table_done(&pt);

    /* Get the firmware instance name. */
    sal_snprintf(fw_inst, BCMBD_FWM_FWINST_FW_NAME_MAX,
                 "%s.%d", fwinfo.fw_type, inst_id);

    /* Process all operations. */
    while ((arg = BCMA_CLI_ARG_CUR(args)) != NULL) {
        /* Check 'A=B' format. */
        op_name = sal_strtok_r((char *)arg, "=", &save_str);
        op_param = sal_strtok_r(NULL, "=", &save_str);
        if (bcmbd_fwm_fw_op_exec(cli->cmd_unit, fw_inst, op_name,
                                 sal_ctoi(op_param, NULL)) < 0) {
            cli_out("Op %s failed.\n", op_name);
            return BCMA_CLI_CMD_FAIL;
        }

        BCMA_CLI_ARG_NEXT(args);
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for sub-command 'exec'.
 */
static int
fwm_cmd_exec(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    char *save_str = NULL;
    const char *fw_inst;
    char *op_name;
    char *op_param;
    int op_cnt;

    arg = BCMA_CLI_ARG_GET(args);

    if (arg == NULL) {
        cli_out("No firmware instance specified.\n");
        return BCMA_CLI_CMD_USAGE;
    }
    fw_inst = arg;

    /* Process all operations. */
    op_cnt = 0;
    while ((arg = BCMA_CLI_ARG_CUR(args)) != NULL) {
        /* Check 'A=B' format. */
        op_name = sal_strtok_r((char *)arg, "=", &save_str);
        op_param = sal_strtok_r(NULL, "=", &save_str);
        if (bcmbd_fwm_fw_op_exec(cli->cmd_unit, fw_inst, op_name,
                                 sal_ctoi(op_param, NULL)) < 0) {
            cli_out("Op %s failed.\n", op_name);
            return BCMA_CLI_CMD_FAIL;
        }
        op_cnt++;
        BCMA_CLI_ARG_NEXT(args);
    }

    if (op_cnt == 0) {
        int i;
        bcmbd_fwm_fw_op_t op;
        shr_pb_t *pb;

        pb = shr_pb_create();
        shr_pb_printf(pb, "Operations supported:\n");
        if (bcmbd_fwm_fw_op_count_get(cli->cmd_unit, fw_inst, &op_cnt) < 0) {
            cli_out("Failed to get firmware operations.\n");
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }

        for (i = 0; i < op_cnt; i++) {
            if (bcmbd_fwm_fw_op_get(cli->cmd_unit, fw_inst, i, &op) < 0) {
                cli_out("Failed to get firmware operation #%d.\n", i);
                shr_pb_destroy(pb);
                return BCMA_CLI_CMD_FAIL;
            }

            shr_pb_printf(pb, "     %-25s%s\n", op.op_name, op.op_help);
        }

        cli_out("%s", shr_pb_str(pb));

        shr_pb_destroy(pb);
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for sub-command 'status'.
 */
static int
fwm_cmd_status(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int i;
    int j;
    int k;
    int fw_inst_cnt;
    bcmbd_fwm_fwinst_t fwinst;
    bcmbd_fwm_proc_state_t state;
    bcmbd_fwm_proc_t proc_drv;
    int proc_cnt;
    int proc_inst_num;
    char proc_inst_name[BCMBD_FWM_PROC_INST_NAME_MAX];

    cli_out("==================================="
            "===================================\n");
    cli_out("%-20s%-20s%s\n", "Firmware", "Version", "Processor");
    cli_out("==================================="
            "===================================\n");

    /* Show by firmware instance view. */
    if (bcmbd_fwm_fw_instance_count_get(cli->cmd_unit, &fw_inst_cnt) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    for (i = 0; i < fw_inst_cnt; i++) {
        if (bcmbd_fwm_fw_instance_get(cli->cmd_unit, i, &fwinst) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }
        if (bcmbd_fwm_proc_state_get(cli->cmd_unit, fwinst.processor,
                                     &state) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }

        cli_out("%-20s%-20s%s - ", fwinst.name, fwinst.fw_version,
                                   fwinst.processor);
        if (state == BCMBD_FWM_PROC_STATE_STARTED) {
            cli_out("Started\n");
        } else if (state == BCMBD_FWM_PROC_STATE_STOPPED) {
            cli_out("Stopped\n");
        } else {
            cli_out("\n");
        }
    }

    /* Show by processor instance view, except those have firmware loaded. */
    if (bcmbd_fwm_proc_count_get(cli->cmd_unit, &proc_cnt) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }
    for (i = 0; i < proc_cnt; i++) {
        if (bcmbd_fwm_proc_get(cli->cmd_unit, i,
                               &proc_drv, &proc_inst_num) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }

        for (j = 0; j < proc_inst_num; j++) {
            int skip = 0;
            int rv;
            rv = sal_snprintf(proc_inst_name, BCMBD_FWM_PROC_INST_NAME_MAX,
                              "%s.%d", proc_drv.name, j);
            if (rv < 0 || rv >= BCMBD_FWM_PROC_INST_NAME_MAX) {
                return BCMA_CLI_CMD_FAIL; /* should not happen */
            }

            /*
             * Filter out processors already displayed in the firmware
             * instance section.
             */
            for (k = 0; k < fw_inst_cnt; k++) {
                if (bcmbd_fwm_fw_instance_get(cli->cmd_unit, k, &fwinst) < 0) {
                    return BCMA_CLI_CMD_FAIL;
                }
                if (!sal_strcasecmp(fwinst.processor, proc_inst_name)) {
                    skip = 1;
                    continue;
                }
            }

            if (!skip) {
                if (bcmbd_fwm_proc_state_get(cli->cmd_unit, proc_inst_name,
                                             &state) < 0) {
                    return BCMA_CLI_CMD_FAIL;
                }

                cli_out("%-20s%-20s%s - ", "N/A", "", proc_inst_name);
                if (state == BCMBD_FWM_PROC_STATE_STARTED) {
                    cli_out("Started\n");
                } else if (state == BCMBD_FWM_PROC_STATE_STOPPED) {
                    cli_out("Stopped\n");
                } else {
                    cli_out("\n");
                }
            }
        }
    }
    return BCMA_CLI_CMD_OK;
}

/*
 * Function for sub-command 'start'.
 */
static int
fwm_cmd_proc_start(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        cli_out("No processor specified.\n");
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcmbd_fwm_proc_control(cli->cmd_unit, arg,
                               BCMBD_FWM_PROC_CONTROL_START) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for sub-command 'stop'.
 */
static int
fwm_cmd_proc_stop(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        cli_out("No processor specified.\n");
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcmbd_fwm_proc_control(cli->cmd_unit, arg,
                               BCMBD_FWM_PROC_CONTROL_STOP) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for sub-command 'reset'.
 */
static int
fwm_cmd_proc_reset(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        cli_out("No processor specified.\n");
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcmbd_fwm_proc_control(cli->cmd_unit, arg,
                               BCMBD_FWM_PROC_CONTROL_RESET) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * General parser for processor memory operation sub-commands 'read/write/dump'.
 *     [in] cli BCMA CLI structure.
 *     [in] args Command arguments.
 *     [out] proc_name Processor name.
 *     [out] mem_mode Memory operation mode of byte/half-word/word.
 *     [out] proc_addr Processor memory address.
 *     [out] proc_idx Index of the processor database.
 *     [out] proc_inst_idx Processor instance number.
 *     [out] data Value to be written to processor memory. Only for 'write'.
 */
static int
fwm_cmd_proc_dbg_common(bcma_cli_t *cli, bcma_cli_args_t *args,
                        const char **proc_name, int *mem_mode,
                        uint64_t *proc_addr,
                        int *proc_idx, int *proc_inst_idx, uint32_t *data)
{
    const char *arg = NULL;
    bcma_cli_parse_table_t pt;

    /*
     * Check if Format=x1|x2|x4 specified.
     */
    *mem_mode = MEM_MODE_W; /* In words by default. */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Format", "enum", mem_mode, mem_mode_enum);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    *proc_name = BCMA_CLI_ARG_GET(args);
    if (*proc_name == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcmbd_fwm_proc_index_get(cli->cmd_unit, *proc_name,
                                 proc_idx, proc_inst_idx) < 0) {
        cli_out("Processor %s is not supported on this device.\n", *proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        if (bcma_cli_parse_uint64(arg, proc_addr) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    /* For write and dump, parse the value/length data. */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        *data = sal_ctoi(arg, NULL);
    } else {
        *data = 0;
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for processor memory operation sub-command 'read'.
 */
static int
fwm_cmd_proc_read(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *proc_name = NULL;
    uint64_t proc_addr;
    int mem_mode;
    uint32_t data;
    int proc_idx;
    int inst_idx;
    bcmbd_fwm_proc_t *proc_drv = NULL;
    uint8_t buf_b = 0;
    uint16_t buf_h = 0;
    uint32_t buf_w = 0;
    shr_pb_t *pb;

    if (fwm_cmd_proc_dbg_common(cli, args, &proc_name, &mem_mode, &proc_addr,
                                &proc_idx, &inst_idx, &data) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_proc_drv_get(cli->cmd_unit, proc_idx, &proc_drv) < 0) {
        cli_out("Processor %s is not supported on this device.\n", proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!proc_drv || !proc_drv->read) {
        cli_out("Processor %s is not supported on this device.\n", proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();

    if (mem_mode == MEM_MODE_B) {
        proc_drv->read(cli->cmd_unit, inst_idx, proc_addr, &buf_b, 1);

        shr_pb_format_uint64(pb, "", &proc_addr, 1, 0);
        shr_pb_printf(pb, ": %02x\n", buf_b);
    } else if (mem_mode == MEM_MODE_H) {
        proc_drv->read(cli->cmd_unit, inst_idx, proc_addr,
                       (uint8_t *)&buf_h, 2);
        shr_pb_format_uint64(pb, "", &proc_addr, 1, 0);
        shr_pb_printf(pb, ": %04x\n", buf_h);
    } else {
        proc_drv->read(cli->cmd_unit, inst_idx, proc_addr,
                       (uint8_t *)&buf_w, 4);
        shr_pb_format_uint64(pb, "", &proc_addr, 1, 0);
        shr_pb_printf(pb, ": %08x\n", buf_w);
    }

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

/*
 * Function for processor memory operation sub-command 'write'.
 */
static int
fwm_cmd_proc_write(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *proc_name = NULL;
    uint64_t proc_addr;
    int mem_mode;
    uint32_t value;
    int proc_idx;
    int inst_idx;
    bcmbd_fwm_proc_t *proc_drv = NULL;

    if (fwm_cmd_proc_dbg_common(cli, args, &proc_name, &mem_mode, &proc_addr,
                                &proc_idx, &inst_idx, &value) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_proc_drv_get(cli->cmd_unit, proc_idx, &proc_drv) < 0) {
        cli_out("Processor %s is not supported on this device.\n", proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!proc_drv || !proc_drv->write) {
        cli_out("Processor %s is not supported on this device.\n", proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    if (mem_mode == MEM_MODE_B) {
        proc_drv->write(cli->cmd_unit, inst_idx, proc_addr,
                        (uint8_t *)&value, 1);
    } else if (mem_mode == MEM_MODE_H) {
        proc_drv->write(cli->cmd_unit, inst_idx, proc_addr,
                        (uint8_t *)&value, 2);
    } else {
        proc_drv->write(cli->cmd_unit, inst_idx, proc_addr,
                        (uint8_t *)&value, 4);
    }
    return BCMA_CLI_CMD_OK;
}

/*
 * Function for processor memory operation sub-command 'dump'.
 */
static int
fwm_cmd_proc_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *proc_name = NULL;
    uint64_t proc_addr;
    int mem_mode;
    uint32_t *buffer32;
    uint16_t *buffer16;
    uint8_t *buffer8;
    uint32_t length;
    int proc_idx;
    int inst_idx;
    bcmbd_fwm_proc_t *proc_drv = NULL;
    int i;
    shr_pb_t *pb;

    if (fwm_cmd_proc_dbg_common(cli, args, &proc_name, &mem_mode, &proc_addr,
                                &proc_idx, &inst_idx, &length) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcmbd_fwm_proc_drv_get(cli->cmd_unit, proc_idx, &proc_drv) < 0) {
        cli_out("Processor %s is not supported on this device.\n", proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!proc_drv || !proc_drv->read) {
        cli_out("Processor %s is not supported on this device.\n", proc_name);
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();

    if (mem_mode == MEM_MODE_B) {
        buffer8 = sal_alloc(length, "bcmaFwmProcDump");
        if (!buffer8) {
            cli_out("Cannot allocate buffer\n");
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        proc_drv->read(cli->cmd_unit, inst_idx, proc_addr, buffer8, length);
        for (i = 0; i < (int)length; i++, proc_addr += 1) {
            if ((i % DUMP_MB_COLS) == 0) {
                shr_pb_format_uint64(pb, "", &proc_addr, 1, 0);
                shr_pb_printf(pb, ": ");
            }
            shr_pb_printf(pb, "%02x%c", *(buffer8 + i),
                          ((i + 1) % DUMP_MB_COLS) == 0 ? '\n' : ' ');
        }
        if (i % DUMP_MB_COLS) {
            shr_pb_printf(pb, "\n");
        }
        sal_free(buffer8);
    } else if (mem_mode == MEM_MODE_H) {
        buffer16 = sal_alloc(length * 2, "bcmaFwmProcDump");
        if (!buffer16) {
            cli_out("Cannot allocate buffer\n");
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        proc_drv->read(cli->cmd_unit, inst_idx, proc_addr,
                       (uint8_t *)buffer16, length * 2);
        for (i = 0; i < (int)length; i++, proc_addr += 2) {
            if ((i % DUMP_MH_COLS) == 0) {
                shr_pb_format_uint64(pb, "", &proc_addr, 1, 0);
                shr_pb_printf(pb, ": ");
            }
            shr_pb_printf(pb, "%04x%c", *(buffer16 + i),
                          ((i + 1) % DUMP_MH_COLS) == 0 ? '\n' : ' ');
        }
        if (i % DUMP_MH_COLS) {
            shr_pb_printf(pb, "\n");
        }
        sal_free(buffer16);
    } else {
        buffer32 = sal_alloc(length * 4, "bcmaFwmProcDump");
        if (!buffer32) {
            cli_out("Cannot allocate buffer\n");
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        proc_drv->read(cli->cmd_unit, inst_idx, proc_addr,
                       (uint8_t *)buffer32, length * 4);
        for (i = 0; i < (int)length; i++, proc_addr += 4) {
            if ((i % DUMP_MW_COLS) == 0) {
                shr_pb_format_uint64(pb, "", &proc_addr, 1, 0);
                shr_pb_printf(pb, ": ");
            }
            shr_pb_printf(pb, "%08x%c", *(buffer32 + i),
                          ((i + 1) % DUMP_MW_COLS) == 0 ? '\n' : ' ');
        }
        if (i % DUMP_MW_COLS) {
            shr_pb_printf(pb, "\n");
        }
        sal_free(buffer32);
    }
    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t fwm_cmds[] = {
    {"query", fwm_cmd_query},
    {"load", fwm_cmd_load},
    {"exec", fwm_cmd_exec},
    {"status", fwm_cmd_status},
    {"start", fwm_cmd_proc_start},
    {"stop", fwm_cmd_proc_stop},
    {"reset", fwm_cmd_proc_reset},
    {"read", fwm_cmd_proc_read},
    {"write", fwm_cmd_proc_write},
    {"dump", fwm_cmd_proc_dump}
};

/* Main entrance for Firmware Management commands. */
int
bcma_fwmcmd_fwm(bcma_cli_t *cli, bcma_cli_args_t *args)
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

    for (idx = 0; idx < COUNTOF(fwm_cmds); idx++) {
        if (sal_strcasecmp(fwm_cmds[idx].name, cmd) == 0) {
            BCMA_CLI_ARG_NEXT(args);
            return (*fwm_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
