/*! \file bcma_i2ccmd_i2c.c
 *
 *  CLI command for I2C operation user interface.
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
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2ccmd_op.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>
#include <shr/shr_pb.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/*!
 * \brief Utility function to dump collected CLI input data.
 *
 * \param[in] i2c_cli_op Data structure to hold input data.
 *
 * \return converted value.
 */
static void
i2c_output_op_result(int unit, bcma_i2c_cli_struct_t *i2c_cli_op)
{
    int ix;

    if ((i2c_cli_op->op_code == BCMA_I2C_READ_BYTE) ||
        (i2c_cli_op->op_code == BCMA_I2C_READ_BYTE_DATA)) {
        for (ix = 0; ix < i2c_cli_op->count; ix++) {
            cli_out("byte[%d]: 0x%x\n", ix, i2c_cli_op->data[ix]);
        }
    } else if (i2c_cli_op->op_code == BCMA_I2C_READ_WORD_DATA) {
        cli_out("word: 0x%x\n", i2c_cli_op->data_word);
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        shr_pb_t *pb = shr_pb_create();

        shr_pb_printf(pb, "I2C operation result:\n");
        shr_pb_printf(pb, "Opcode: %d, Addr: %d, Command: %d, count: %d\n",
                      i2c_cli_op->op_code, i2c_cli_op->saddr,
                      i2c_cli_op->command, i2c_cli_op->count);
        shr_pb_printf(pb, "Data Word: 0x%x\n",
                      i2c_cli_op->data_word);
        for (ix = 0; ix < i2c_cli_op->count; ix++) {
            shr_pb_printf(pb, "Data Byte[%d]: 0x%x\n",
                          ix, i2c_cli_op->data[ix]);
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }
}

/*!
 * \brief CLI 'i2c' command for I2C controlling.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_i2ccmd_i2c(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *arg;
    const char *arg_buf[I2C_MAX_ARG_COUNT];
    i2c_device_t *dev_info = NULL;
    int total = 0;
    int argc;
    int ix;
    bcma_i2c_cli_struct_t i2c_op;
    int rv;
    uint32_t data;

    /* Variables initialization */
    unit = cli->cmd_unit;

    /* Reset data holder for each CLI call. */
    sal_memset((char *)&i2c_op, 0, sizeof(bcma_i2c_cli_struct_t));
    sal_memset((char *)arg_buf, 0, sizeof(arg_buf));
    argc = 0;

    /* Sort CLI arguments into buffer. */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        while (arg) {
           arg_buf[argc]=arg;

           arg = BCMA_CLI_ARG_GET(args);
           argc++;
           if (argc >= I2C_MAX_ARG_COUNT) {
               return BCMA_CLI_CMD_USAGE;
           }
        }
    }

    if (argc == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Arg count: %d\n"),
                 argc));

    for (ix = 0; ix < argc; ix++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "arg[%d]: %s\n"),
                     ix, arg_buf[ix]));
    }

    if (sal_strncmp(arg_buf[0], "reset", 5) == 0) {
        if (argc != 1) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.op_code = BCMA_I2C_RESET;
    } else if (sal_strncmp(arg_buf[0], "show", 4) == 0) {
        if (argc != 1) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.op_code = BCMA_I2C_PROBE;

        if (!bcma_i2c_device_drv[unit]) {
            bcma_i2c_board_drv_init(unit);
        }

        if (bcma_i2c_device_drv[unit] &&
            bcma_i2c_device_drv[unit]->dev_get) {
            /* Query driver total device count */
            bcma_i2c_device_drv[unit]->dev_get(unit, &total, dev_info);

            /* Allocate required mem to hold dev info */
            if ((total != 0) && (total < 20)) {
                dev_info = sal_alloc(sizeof(i2c_device_t)*total, "bcmi2cDevices");
                if (dev_info == NULL) {
                    return SHR_E_MEMORY;
                }

                /* Call driver again to get dev info */
                bcma_i2c_device_drv[unit]->dev_get(unit, &total, dev_info);
                cli_out("\nList \t Name \t\t Addr\n\n");

                for (ix = 0; ix < total; ix++) {
                    cli_out("%2d \t %s \t 0x%x\n", ix,
                                                   (char *)dev_info[ix].devname,
                                                   (int)dev_info[ix].saddr);
                }

                cli_out("\n");
                sal_free(dev_info);
            }
        } else {
            return SHR_E_UNAVAIL;
        }
    } else if (sal_strncmp(arg_buf[0], "readb", 5) == 0) {
        if (argc != 3) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.op_code = BCMA_I2C_READ_BYTE;

        if (bcma_cli_parse_uint32(arg_buf[1], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.saddr  = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[2], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.count  = data & 0xff;

        for (ix = 0; ix < i2c_op.count; ix++) {
            rv = bcmbd_i2c_read_byte(unit,
                                     i2c_op.saddr,
                                     &i2c_op.data[ix]);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "READB operation failed: %d\n"), rv));

                return rv;
            }
        }
    } else if (sal_strncmp(arg_buf[0], "readw", 5) == 0) {
        if (argc != 3) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.op_code = BCMA_I2C_READ_WORD_DATA;
        if (bcma_cli_parse_uint32(arg_buf[1], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.saddr  = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[2], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.command = data & 0xff;

        i2c_op.count = 1;
        rv = bcmbd_i2c_read_word_data(unit,
                                      i2c_op.saddr,
                                      i2c_op.command,
                                      &i2c_op.data_word);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "READW operation failed: %d\n"), rv));
            return rv;
        }
    } else if (sal_strncmp(arg_buf[0], "read", 4) == 0) {
        if (argc != 4) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.op_code = BCMA_I2C_READ_BYTE_DATA;

        if (bcma_cli_parse_uint32(arg_buf[1], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.saddr  = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[2], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.command = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[3], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.count  = data & 0xff;

        rv = bcmbd_i2c_read_byte_data(unit,
                                      i2c_op.saddr,
                                      i2c_op.command,
                                      &i2c_op.data[0]);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "READ operation failed: %d\n"), rv));
            return rv;
        }
    } else if (sal_strncmp(arg_buf[0], "writew", 6) == 0) {
        if (argc != 4) {
            return BCMA_CLI_CMD_USAGE;
        }

        i2c_op.op_code = BCMA_I2C_WRITE_WORD_DATA;
        if (bcma_cli_parse_uint32(arg_buf[1], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.saddr  = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[2], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.command = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[3], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.data_word  = data & 0xffff;
        i2c_op.count = 2;

        rv = bcmbd_i2c_write_word_data(unit,
                                       i2c_op.saddr,
                                       i2c_op.command,
                                       i2c_op.data_word);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "WRITEW operation failed: %d\n"), rv));
            return rv;
        }

    } else if (sal_strncmp(arg_buf[0], "write", 5) == 0) {
        if (argc != 4) {
            return BCMA_CLI_CMD_USAGE;
        }

        i2c_op.op_code = BCMA_I2C_WRITE_BYTE_DATA;
        if (bcma_cli_parse_uint32(arg_buf[1], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.saddr  = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[2], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.command = data & 0xff;

        if (bcma_cli_parse_uint32(arg_buf[3], &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        i2c_op.data[0]  = data & 0xff;
        i2c_op.count = 1;

        rv = bcmbd_i2c_write_byte_data(unit,
                                       i2c_op.saddr,
                                       i2c_op.command,
                                       i2c_op.data[0]);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "WRITE operation failed: %d\n"), rv));
            return rv;
        }
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Output read operation result and debug info */
    i2c_output_op_result(unit, &i2c_op);

    return BCMA_CLI_CMD_OK;
}
