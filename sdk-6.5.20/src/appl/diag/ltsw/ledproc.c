/*! \file ledproc.c
 *
 * LTSW LED demo command.
 *
 * This is a LED demo command at application layer.
 * You can find LED firmware example at following location.
 * CMICx device: $SDK/tools/led/cmicx/custom_led.c.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <bcmlt/bcmlt.h>
#include <shared/cmicfw/cmicx_led_public.h>

/* For BCM LED API use. */
#define LED_STOP      0
#define LED_START     1

/* Local Variable */
static char *cmd_name;

/*!
 * \brief Get physical port number from port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] phy_port Physical port number.
 *
 * \retval BCM_E_NONE if successful.
 */
static int
ledporc_pport_number_get(int unit, soc_port_t port, soc_port_t *phy_port)
{
    int rv;
    uint64 val64;
    bcmlt_entry_handle_t entry;
    bcmlt_entry_info_t entry_info;

    /* Get link state from PC_PORT_PHYS_MAP. */
    rv = bcmlt_entry_allocate(unit, "PC_PORT_PHYS_MAP", &entry);
    if (BCM_FAILURE(rv)) return rv;

    rv = bcmlt_entry_field_add(entry, "PORT_ID", port);
    if (BCM_FAILURE(rv)) goto error;

    rv = bcmlt_entry_commit(entry,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (BCM_FAILURE(rv)) goto error;

    rv = bcmlt_entry_info_get(entry, &entry_info);
    if (BCM_FAILURE(rv)) goto error;
    if (BCM_FAILURE(entry_info.status)) goto error;

    rv = bcmlt_entry_field_get(entry, "PC_PHYS_PORT_ID", &val64);
    if (BCM_FAILURE(rv)) goto error;

    *phy_port = (soc_port_t) val64;

error:
    bcmlt_entry_free(entry);
    return rv;
}

/*!
 * \brief Load LED firmware of LED uC from buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED uC number.
 * \param [in] buf Buffer of LED firmware.
 * \param [in] bytes Number of bytes in array.
 *
 * \retval N/A.
 */
static int
ledproc_ltsw_load(int unit, int led_uc, uint8 *buf, int bytes)
{
    int start, rv;

    rv = bcm_switch_led_fw_start_get(unit, led_uc, &start);
    if (BCM_FAILURE(rv)) {
        cli_out("Fail to get LED firmware running status fail.\n");
        return BCM_E_FAIL;
    }

    if (start) {
        cli_out("Firmware download stop since LED Firmware is running.\n");
        return BCM_E_FAIL;
    }

    rv = bcm_switch_led_fw_load(unit, led_uc, buf, bytes);
    if (BCM_FAILURE(rv)) {
        cli_out("Fail to download LED firmware.\n");
    }

    return rv;
}

#ifndef NO_FILEIO
/*!
 * \brief Load LED firmware of LED uC from hex file.
 *        File format (ASCII FILE)
 *        00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *        10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *        ... white space ignored.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED uC number.
 * \param [in] buf Buffer of LED firmware.
 * \param [in] bytes Number of bytes in array.
 *
 * \retval BCM_E_NONE if successful.
 */
static int
ledproc_ltsw_load_from_binfile(int unit,
                               int led_uc,
                               char *file_name)
{
    int fsize, size, rv = BCM_E_NONE;
    uint8 *buf = NULL;
    FILE  *f;

    f = sal_fopen(file_name, "rb");
    if (f == NULL) {
        cli_out("%s: Error: Fail to open %s\n",
                cmd_name, file_name);
        return BCM_E_FAIL;
    }

    fsize = sal_fsize(f);
    if (fsize <= 0) {
        cli_out("%s: Error: file size is not valid: size = %d\n",
                 cmd_name, fsize);
        goto error;
    }

    buf = sal_alloc(fsize, "M0FW");
    if (buf == NULL) {
        cli_out("%s: Memory allocate fail\n", cmd_name);
        goto error;
    }

    size = sal_fread(buf, 1, fsize, f);

    rv = ledproc_ltsw_load(unit, led_uc, buf, size);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Fail to load file to firmware.\n", cmd_name);
        goto error;
    }

    cli_out("%s: Loading %d bytes into LED program memory\n",
            cmd_name, size);

error:
    sal_fclose((FILE *) f);
    if (buf) sal_free(buf);

    return rv;

}

/*!
 * \brief Load LED firmware of LED uC from hex file.
 *        File format (ASCII FILE)
 *        00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *        10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *        ... white space ignored.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED uC number.
 * \param [in] buf Buffer of LED firmware.
 * \param [in] bytes Number of bytes in array.
 *
 * \retval BCM_E_NONE if successful.
 */
static int
ledproc_ltsw_load_from_hexfile(int unit,
                               int led_uc,
                               char *file_name)
{
    uint8 program[CMIC_LED_PROGRAM_RAM_SIZE];
    char input[256], *cp = NULL, *error = NULL;
    int bytes = 0, line = 0;
    int offset_size = 0;
    FILE *f;

    f = sal_fopen(file_name, "r");
    if (f == NULL) {
        cli_out("%s: Error: Fail to open %s\n",
                cmd_name, file_name);
        return BCM_E_FAIL;
    }

    while (!error && (cp = sal_fgets(input, sizeof(input) - 1, f))) {
        line++;

        while (*cp) {
            if (isspace((unsigned)(*cp))) { /* Skip whitespace */
                cp ++;
            } else {
                if (!isxdigit((unsigned)*cp) ||
                    !isxdigit((unsigned)*(cp + 1))) {
                    error = "Invalid hex digit";
                    break;
                }
                offset_size = sizeof(program);
                if (bytes >= offset_size) {
                    error = "Program memory exceeded";
                    break;
                }
                program[bytes ++] = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
                cp += 2;
            }
        }
    }

    if (error) {
        cli_out("%s: Error loading file %s line %d: %s\n",
                cmd_name, file_name, line, error);
        return(CMD_FAIL);
    }

    cli_out("%s: Loading %d bytes into LED program memory\n",
            cmd_name, bytes);

    return ledproc_ltsw_load(unit, led_uc, program, bytes);
}
#endif /* NO_FILEIO */

/*!
 * \brief Load LED firmware of LED uC from command args.
 *        Args format (ASCII FILE)
 *        00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *        10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *        ... white space is optional.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED uC number.
 * \param [in] args Remainder of command line containing raw hex data.
 *
 * \retval BCM_E_NONE if successful.
 */
static int
ledproc_ltsw_load_args(int unit, int led_uc, args_t *args)
{
    uint8 program[CMIC_LED_PROGRAM_RAM_SIZE];
    int bytes = 0;
    char *cp;
    int offset_size = 0;

    sal_memset(program, 0, sizeof (program));

    while ((cp = ARG_GET(args)) != NULL) {
        while (*cp) {
            if (isspace((int)(*cp))) {
                cp++;
            } else {
                if (!isxdigit((unsigned)*cp) ||
                    !isxdigit((unsigned)*(cp+1))) {
                    cli_out("%s: Invalid character\n", cmd_name);
                    return(CMD_FAIL);
                }

                        offset_size = sizeof(program);
                if (bytes >= offset_size) {
                    cli_out("%s: Program memory exceeded\n", cmd_name);
                    return(CMD_FAIL);
                }
                program[bytes++] = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
                cp += 2;
            }
        }
    }

    return ledproc_ltsw_load(unit, led_uc, program, bytes);
}

/*!
 * \brief Call back function for LED update on link change.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] info Port status.
 *
 * \retval N/A.
 */
static void
ledproc_ltsw_linkscan_cb(int unit, soc_port_t port, bcm_port_info_t *info)
{
    int phy_port = 0;  /* physical port number */
    uint8 led_control_data = 0, link_status;
    int rv = BCM_E_NONE;
    int led_uc, led_uc_port;

    link_status = (info->linkstatus == BCM_PORT_LINK_STATUS_UP);

    /* Get led_uc and led_uc_port. */
    ledporc_pport_number_get(unit, port, &phy_port);
    rv = bcm_switch_led_port_to_uc_port_get(unit, phy_port,
                                            &led_uc, &led_uc_port);
    if (BCM_FAILURE(rv)) {
        cli_out("Error rv %d: Fail to get led port from physical port.\n", rv);
    }

    /* Collect link status and port speed then write to
     * LED firmware's led_control_data.
     */
    led_control_data = link_status;

    rv = bcm_switch_led_control_data_write(unit, led_uc, led_uc_port * sizeof(led_control_data),
                                           &led_control_data, sizeof(led_control_data));
    if (BCM_FAILURE(rv)) {
        cli_out("Error rv %d: Unable to get speed for port %d\n", rv, port);
    }
}

/*!
 * \brief LED demo command.
 *        Load a led program, start and use it.
 *
 * \param [in] unit Unit number.
 * \param [in] a Command args.
 *
 * \retval CMD_OK if successful.
 * \retval CMD_USAGE if usage is wanted.
 * \retval CMD_FAIL if fail.
 */
cmd_result_t
cmd_ltsw_ledproc(int unit, args_t *a)
{
    VOL cmd_result_t rv = CMD_OK;
    char *c;
    volatile int auto_on;
    int led_uc_max = 1;
    int led_uc, show_uc_status;
    int led_start = 0;
    int is_hex, len;
    int link;
    bcm_port_info_t info;
    bcm_port_config_t cfg;
    soc_port_t port;

    if (!sh_check_attached("ledproc", unit)) {
        return(CMD_FAIL);
    }
    cmd_name = ARG_CMD(a);

    bcm_switch_led_uc_num_get(unit, &led_uc_max);

    if (bcm_linkscan_unregister(unit, ledproc_ltsw_linkscan_cb) < 0) {
        auto_on = FALSE;
    } else {
        (void)bcm_linkscan_register(unit, ledproc_ltsw_linkscan_cb);
        auto_on = TRUE;
    }

    led_uc = 0;
    show_uc_status = 0;

    /* If input argument count is zero then show all LED uc status. */
    if (ARG_CNT(a) == 0) {
        show_uc_status = 1;
    } else {

        /* check if command has processor number specified */
        c = ARG_GET(a);
        if (isint(c)) {
            /* led_uc is specified here. */
            led_uc = parse_integer(c);
            if (led_uc > (led_uc_max - 1)) {
                cli_out("Invalid LED uC number: %d.\n", led_uc);
                return CMD_FAIL;
            }
            c = ARG_GET(a);
            if (c == NULL) {
                show_uc_status = 1;
            }
        }
    }

    if (show_uc_status || !sal_strcasecmp(c, "status")) {

        /* Check if LED firmware is started or not. */
        rv = bcm_switch_led_fw_start_get(unit, led_uc, &led_start);
        if (BCM_FAILURE(rv)) {
            cli_out("Fail to get LED firmware status.\n");
            return CMD_FAIL;
        }

        /* Print out LED status. */
        cli_out("LED M0 FW is %s Init DONE; "
                "auto updating is %s.\n",
                led_start ? "ON" : "OFF",
                auto_on ? "ON" : "OFF");

    } else if (!sal_strcasecmp(c, "start")) {

        rv = bcm_switch_led_fw_start_set(unit, led_uc, LED_START);
        if (BCM_FAILURE(rv)) {
            cli_out("Error:Unable to start LED FW\n");
            return(CMD_FAIL);
        }

    } else if (!sal_strcasecmp(c, "stop")) {

        rv = bcm_switch_led_fw_start_set(unit, led_uc, LED_STOP);
        if (BCM_FAILURE(rv)) {
            cli_out("Error:Unable to stop LED FW\n");
            return(CMD_FAIL);
        }

   } else if (!sal_strcasecmp(c, "load")) {
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
#else
        /* Get file name. */
        c = ARG_GET(a);

        /* Check suffix. */
        if ((len = sal_strlen(c)) < 5 ||
            (sal_strncmp(&c[len - 4], ".bin", 4) &&
             sal_strncmp(&c[len - 4], ".hex", 4)))
        {
            cli_out("%s: File name \"%s\" has not format suffix.\n",
                    cmd_name, &c[len - 4]);
        }
        is_hex = !sal_strncmp(&c[len - 4], ".hex", 4);

        /* Keep previous state. */
        bcm_switch_led_fw_start_get(unit, led_uc, &led_start);

        /* Stop LED processor.*/
        bcm_switch_led_fw_start_set(unit, led_uc, LED_STOP);

        /* Load LED firmware. */
        if (is_hex) {
            ledproc_ltsw_load_from_hexfile(unit,
                                           led_uc,
                                           c);
        } else {
            ledproc_ltsw_load_from_binfile(unit,
                                           led_uc,
                                           c);
        }

        /* Restore LED processor if it was running */
        bcm_switch_led_fw_start_set(unit, led_uc, led_start);
#endif /* NO_FILEIO */

    } else if (!sal_strcasecmp(c, "prog")) {
        if (ARG_CUR(a) == NULL) {
            return(CMD_USAGE);
        }

        /* Stop LED processor if it was running. */
        bcm_switch_led_fw_start_set(unit, led_uc, LED_STOP);

        /* Load LED firmware. */
        rv = ledproc_ltsw_load_args(unit, led_uc, a);
        if (BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }

    } else if (!sal_strcasecmp(c, "auto")) {

        if ((c = ARG_GET(a)) != NULL) {
            if (!sal_strcasecmp(c, "off")) {
                bcm_linkscan_unregister(unit, ledproc_ltsw_linkscan_cb);
            } else if (!sal_strcasecmp(c, "on")) {
                if (auto_on == FALSE) {
                    bcm_linkscan_register(unit, ledproc_ltsw_linkscan_cb);

                    bcm_port_config_t_init(&cfg);
                    if (bcm_port_config_get(unit, &cfg) != BCM_E_NONE) {
                        cli_out("%s: Error: bcm ports not initialized\n", cmd_name);
                        return CMD_FAIL;
                    }

                    /* Sync LED status with Linkscan module. */
                    BCM_PBMP_ITER(cfg.port, port) {
                        bcm_port_link_status_get(unit, port, &link);
                        if (link) {
                            info.linkstatus = BCM_PORT_LINK_STATUS_UP;
                        } else {
                            info.linkstatus = BCM_PORT_LINK_STATUS_DOWN;
                        }
                        ledproc_ltsw_linkscan_cb(unit, port, &info);
                    }
                }
            } else {
                return CMD_USAGE;
            }
        }
   } else {
        cli_out("Unsupported LED CLI command\n");
        return CMD_FAIL;
   }

   return rv;
}
