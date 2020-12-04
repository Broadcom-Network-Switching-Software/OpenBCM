/*! \file bcma_bcmbdcmd_qspi.c
 *
 * CLI 'qspi' command handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_pb.h>

#include <bcmbd/bcmbd_qspi.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/flash/bcma_flash.h>
#include <bcma/bcmbd/bcma_bcmbd_qspi.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_qspi.h>


#define BSL_LOG_MODULE BSL_LS_APPL_QSPI

/*******************************************************************************
 * Private functions
 */

static void
bcmbdcmd_qspi_data_set(bcma_cli_t *cli, const uint8_t *data, int size,
                       const char *env_var)
{
    shr_pb_t *pb;
    int idx;

    if (data == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("NULL pointer\n")));
        return;
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "0x");
    for (idx = size - 1; idx >= 0; idx--) {
        shr_pb_printf(pb, "%02x", data[idx]);
    }

    bcma_cli_var_set(cli, env_var, (char *)shr_pb_str(pb), TRUE);
    shr_pb_destroy(pb);
}

static void
bcmbdcmd_qspi_data_dump(const uint8_t *data, int size)
{
    shr_pb_t *pb;
    int idx;

    if (data == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("NULL pointer\n")));
        return;
    }

    pb = shr_pb_create();

    for (idx = 0; idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            shr_pb_printf(pb, "%04x: ", idx);
        }
        if ((idx & 0xf) == 8) {
            shr_pb_printf(pb, "- ");
        }
        shr_pb_printf(pb, "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            shr_pb_printf(pb, "\n");
        }
    }
    if ((idx & 0xf) != 0) {
        shr_pb_printf(pb, "\n");
    }

    cli_out("%s\n", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

static int
bcmbdcmd_qspi_fdev_args_parse(bcma_cli_t *cli, bcma_cli_args_t *args,
                              bcma_flash_dev_t *fdev)
{
    int rv;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "ID", "int", &fdev->id, NULL);
    bcma_cli_parse_table_add(&pt, "SiZe", "int", &fdev->size, NULL);
    bcma_cli_parse_table_add(&pt, "PageSiZe", "int", &fdev->page_size, NULL);
    bcma_cli_parse_table_add(&pt, "SectorSiZe", "int",
                             &fdev->sector_size, NULL);

    rv = bcma_cli_parse_table_do_args(&pt, args);
    bcma_cli_parse_table_done(&pt);
    if (rv < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        return -1;
    }

    if (fdev->page_size == 0 || fdev->sector_size == 0 || fdev->size == 0) {
        cli_out("%s: All size values must be specified and greater than 0.\n",
                BCMA_CLI_ARG_CMD(args));
        return -1;
    }

    fdev->num_sectors = fdev->size / fdev->sector_size;
    fdev->sector_type = 0;

    return 0;
}


/*******************************************************************************
 * Sub-command handlers
 */

static int
bcmbdcmd_qspi_cmd_write(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *arg;
    int rv, wlen = 0, unit = cli->cmd_unit;
    uint32_t rlen = 0;
    uint8_t *wdata = NULL, *rdata = NULL;

    /* Get the write data */
    arg = BCMA_CLI_ARG_GET(args);
    if (!arg) {
        cli_out("%sLength value is not specified.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    wlen = bcma_cli_parse_array_uint8(arg, 0, NULL);
    if (wlen <= 0) {
        cli_out("%sInvalid length value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    wdata = sal_alloc(wlen, "bcmaBcmbdQspiWdata");
    if (!wdata) {
        cli_out("%sFailed to allocate memory for write buffer.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    if (bcma_cli_parse_array_uint8(arg, wlen, wdata) < 0) {
        cli_out("%sFailed to parse the write data %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        sal_free(wdata);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get the read bytes if 'Read' is specified. */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg && sal_strcasecmp(arg, "Read") == 0) {
        arg = BCMA_CLI_ARG_GET(args);
        if (!arg) {
            cli_out("%sLength value is not specified.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            sal_free(wdata);
            return BCMA_CLI_CMD_FAIL;
        }
        if (bcma_cli_parse_uint32(arg, &rlen) < 0) {
            cli_out("%sInvalid length value %s.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            sal_free(wdata);
            return BCMA_CLI_CMD_FAIL;
        }
        if (rlen > 0) {
            rdata = sal_alloc(rlen, "bcmaBcmbdQspiRdata");
            if (!rdata) {
                cli_out("%sFailed to allocate memory for read buffer.\n",
                        BCMA_CLI_CONFIG_ERROR_STR);
                sal_free(wdata);
                return BCMA_CLI_CMD_FAIL;
            }
        }
    }

    /* Initialize QSPI. */
    rv = bcma_bcmbd_qspi_init(unit);
    if (SHR_SUCCESS(rv)) {
        /* Execute the command. */
        rv = bcmbd_qspi_transfer_one(unit, wlen, wdata, rlen, rdata);
        if (SHR_SUCCESS(rv)) {
            /* Show the received data if required. */
            if (rlen > 0) {
                bcmbdcmd_qspi_data_dump(rdata, rlen);
            }
        } else {
            cli_out("%sFailed to execute QSPI opeartion (error code %d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, rv);
        }
    } else {
        cli_out("%sFailed to initialize QSPI (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
    }

    sal_free(wdata);
    if (rdata) {
        sal_free(rdata);
    }

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
bcmbdcmd_qspi_cmd_flash_read(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *arg, *env_var = NULL;
    int rv, unit = cli->cmd_unit;
    uint32_t offset, len;
    uint8_t *data = NULL;
    bcma_cli_parse_table_t pt;

    /* Get offset. */
    arg = BCMA_CLI_ARG_GET(args);
    if (!arg) {
        return BCMA_CLI_CMD_USAGE;
    }
    if (bcma_cli_parse_uint32(arg, &offset) < 0) {
        cli_out("%sInvalid offset value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get length. */
    arg = BCMA_CLI_ARG_GET(args);
    if (!arg) {
        return BCMA_CLI_CMD_USAGE;
    }
    if (bcma_cli_parse_uint32(arg, &len) < 0) {
        cli_out("%sInvalid length value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    if (len == 0) {
        cli_out("The specified length to read is 0.\n");
        return BCMA_CLI_CMD_OK;
    }

    /* Get the output environment variable if any. */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "envVAR", "str", &env_var, NULL);
    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    data = sal_alloc(len, "bcmaBcmbdFlashRead");
    if (!data) {
        cli_out("%sFailed to allocate memory for read buffer.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Read flash data. */
    rv = bcma_bcmbd_qspi_flash_read(unit, offset, len, data);
    if (SHR_SUCCESS(rv)) {
        /* Show the read data. */
        if (env_var) {
            bcmbdcmd_qspi_data_set(cli, data, len, env_var);
        } else {
            bcmbdcmd_qspi_data_dump(data, len);
        }
    } else {
        cli_out("%sFailed to execute QSPI flash read (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
    }

    bcma_cli_parse_table_done(&pt);
    sal_free(data);

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
bcmbdcmd_qspi_cmd_flash_write(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *arg;
    int rv, unit = cli->cmd_unit;
    int len;
    uint32_t offset;
    uint8_t *data = NULL;

    if (BCMA_CLI_ARG_CNT(args) != 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get offset. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &offset) < 0) {
        cli_out("%sInvalid offset value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get the data to write. */
    arg = BCMA_CLI_ARG_GET(args);
    len = bcma_cli_parse_array_uint8(arg, 0, NULL);
    if (len <= 0) {
        cli_out("%sFailed to parse the write data %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    data = sal_alloc(len, "bcmaBcmbdFlashWrite");
    if (!data) {
        return BCMA_CLI_CMD_FAIL;
    }
    if (bcma_cli_parse_array_uint8(arg, len, data) < 0) {
        sal_free(data);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Initialize QSPI. */
    rv = bcma_bcmbd_qspi_init(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to initialize QSPI (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        sal_free(data);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Write data to flash. */
    rv = bcma_flash_write(unit, offset, len, data);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to execute QSPI flash write (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
    } else {
        cli_out("Flash write done.\n");
    }

    sal_free(data);

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
bcmbdcmd_qspi_cmd_flash_erase(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv, unit = cli->cmd_unit;
    uint32_t offset, len;

    if (BCMA_CLI_ARG_CNT(args) != 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get offset. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &offset) < 0) {
        cli_out("%sInvalid offset value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get length. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &len) < 0) {
        cli_out("%sInvalid length value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }
    if (len == 0) {
        cli_out("The specified length to erase is 0.\n");
        return BCMA_CLI_CMD_OK;
    }

    /* Initialize QSPI. */
    rv = bcma_bcmbd_qspi_init(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to initialize QSPI (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Erase flash contents. */
    rv = bcma_flash_erase(unit, offset, len);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to execute QSPI flash erase (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("Flash erase done.\n");
    return BCMA_CLI_CMD_OK;
}

static int
bcmbdcmd_qspi_cmd_flash_load(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char *arg;
    int rv, unit = cli->cmd_unit;
    uint32_t offset;

    if (BCMA_CLI_ARG_CNT(args) != 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get offset. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &offset) < 0) {
        cli_out("%sInvalid offset value %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get filename. */
    arg = BCMA_CLI_ARG_GET(args);

    /* Load file contents to flash. */
    rv = bcma_bcmbd_qspi_flash_load(unit, offset, arg);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to execute QSPI flash load (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("Flash load done.\n");
    return BCMA_CLI_CMD_OK;
}

static int
bcmbdcmd_qspi_cmd_flash_dev_set(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv, unit = cli->cmd_unit;
    bcma_flash_dev_t fdev;

    sal_memset(&fdev, 0, sizeof(fdev));

    if (bcmbdcmd_qspi_fdev_args_parse(cli, args, &fdev) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    rv = bcma_flash_dev_set(unit, &fdev);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to set flash information (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/* Sub-command list. */
static bcma_cli_command_t bd_qspi_cmds[] = {
    { "Write", bcmbdcmd_qspi_cmd_write },
    { "FlashRead", bcmbdcmd_qspi_cmd_flash_read },
    { "FlashWrite", bcmbdcmd_qspi_cmd_flash_write },
    { "FlashErase", bcmbdcmd_qspi_cmd_flash_erase },
    { "FlashLoaD", bcmbdcmd_qspi_cmd_flash_load },
    { "FlashDEV", bcmbdcmd_qspi_cmd_flash_dev_set },
};


/*******************************************************************************
 * QSPI command handler
 */

int
bcma_bcmbdcmd_qspi(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int idx;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0; idx < COUNTOF(bd_qspi_cmds); idx++) {
        if (bcma_cli_parse_cmp(bd_qspi_cmds[idx].name, arg, '\0')) {
            return (*bd_qspi_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
