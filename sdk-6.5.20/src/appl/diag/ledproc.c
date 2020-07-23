/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	ledproc.c
 * Purpose: 	LED Processor Support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <appl/diag/system.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_led.h>
#endif /* BCM_CMICX_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/mbcm.h>
#endif

#ifdef BCM_LTSW_SUPPORT
extern cmd_result_t cmd_ltsw_ledproc(int unit, args_t *a);
#endif

#ifdef BCM_TRIDENT_SUPPORT
#define TRIDENT_LEDUP_MAX              2
#define TRIDENT_LEDUP0_PORT_MAX        36
#define TRIUMPH3_LEDUP0_PORT_MAX       52
#define HELIX4_LEDUP_PORT_MAX          64
#define TRIDENT2_LEDUP0_PORT_MAX       64
#define APACHE_LEDUP_PORT_MAX          36
#define MONTEREY_LEDUP_PORT_MAX        64
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_SAND_SUPPORT)
#define CMICD_V2_LEDUP_MAX             3
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_PETRA_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
#define CMICD_V4_LEDUP_MAX             5
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
#define SABER2_LEDUP_MAX                2
#endif

#ifdef BCM_GREYHOUND2_SUPPORT
#define GREYHOUND2_LEDUP_MAX              2
#define GREYHOUND2_LEDUP0_PORT_MAX        57
#endif /* BCM_GREYHOUND2_SUPPORT */

typedef struct led_info_s {
    uint32 ctrl;
    uint32 status;
    uint32 pram_base;
    uint32 dram_base;
} led_info_t;

static led_info_t led_info_gen[] = {
    {CMIC_LED_CTRL,CMIC_LED_STATUS,CMIC_LED_PROGRAM_RAM_BASE,
     CMIC_LED_DATA_RAM_BASE}
};

#ifdef BCM_TRIDENT_SUPPORT
static led_info_t led_info_tri[] = {
    {CMICE_LEDUP0_CTRL,CMICE_LEDUP0_STATUS,CMICE_LEDUP0_PROGRAM_RAM_BASE,
     CMICE_LEDUP0_DATA_RAM_BASE},
    {CMICE_LEDUP1_CTRL,CMICE_LEDUP1_STATUS,CMICE_LEDUP1_PROGRAM_RAM_BASE,
     CMICE_LEDUP1_DATA_RAM_BASE}
};
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
static led_info_t led_info_cmicm[] = {
    {CMIC_LEDUP0_CTRL_OFFSET,CMIC_LEDUP0_STATUS_OFFSET,CMIC_LEDUP0_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP0_DATA_RAM_OFFSET},
    {CMIC_LEDUP1_CTRL_OFFSET,CMIC_LEDUP1_STATUS_OFFSET,CMIC_LEDUP1_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP1_DATA_RAM_OFFSET}
};
#endif /* BCM_CMICM_SUPPORT */

/*
 * Change 'led info' clause: This was for BCM_PETRA_SUPPORT (Arad and Jericho
 * family)and was changed to (BCM_PETRA_SUPPORT || BCM_DNX_SUPPORT) (Jericho-2 family, DNX) to
 * enable using a similar configuration file (For DNX: dnx.doc)
 * In the future, this may be changed as per actual DNX board details.
 */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
static led_info_t led_info_cmicd_v2[] = {
    {CMIC_LEDUP0_CTRL_OFFSET,CMIC_LEDUP0_STATUS_OFFSET,CMIC_LEDUP0_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP0_DATA_RAM_OFFSET},
    {CMIC_LEDUP1_CTRL_OFFSET,CMIC_LEDUP1_STATUS_OFFSET,CMIC_LEDUP1_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP1_DATA_RAM_OFFSET},
    {CMIC_LEDUP2_CTRL_OFFSET,CMIC_LEDUP2_STATUS_OFFSET,CMIC_LEDUP2_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP2_DATA_RAM_OFFSET}
};
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_PETRA_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
static led_info_t led_info_cmicd_v4[] = {
    {CMIC_LEDUP0_CTRL_OFFSET,CMIC_LEDUP0_STATUS_OFFSET,CMIC_LEDUP0_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP0_DATA_RAM_OFFSET},
    {CMIC_LEDUP1_CTRL_OFFSET,CMIC_LEDUP1_STATUS_OFFSET,CMIC_LEDUP1_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP1_DATA_RAM_OFFSET},
    {CMIC_LEDUP2_CTRL_OFFSET,CMIC_LEDUP2_STATUS_OFFSET,CMIC_LEDUP2_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP2_DATA_RAM_OFFSET},
    {CMIC_LEDUP3_CTRL_OFFSET,CMIC_LEDUP3_STATUS_OFFSET,CMIC_LEDUP3_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP3_DATA_RAM_OFFSET},
    {CMIC_LEDUP4_CTRL_OFFSET,CMIC_LEDUP4_STATUS_OFFSET,CMIC_LEDUP4_PROGRAM_RAM_OFFSET,
     CMIC_LEDUP4_DATA_RAM_OFFSET}
};
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_CMICX_SUPPORT
#define CMICX_LED 1
#endif /* BCM_CMICX_SUPPORT */

/* For BCM LED API use. */
#define LED_FW_STOP      0
#define LED_FW_START     1

/*
 * Function:	ledproc_load
 * Purpose:	Load a program into the LED microprocessor from a buffer
 * Parameters:	unit - StrataSwitch unit #
 *		program - Array of up to 256 program bytes
 *		bytes - Number of bytes in array
 * Notes:	Also clears the LED processor data RAM from 0x80-0xff
 *		so the LED program has a known state at startup.
 */

STATIC void
ledproc_load(int unit, uint8 *program, int bytes, led_info_t *led_ptr)
{
    int offset;

#ifdef BCM_CMICX_SUPPORT
    int start, rv;
    if (soc_feature(unit, soc_feature_cmicx)) {

        rv = bcm_switch_led_fw_start_get(unit, 0, &start);
        if (BCM_FAILURE(rv)) {
            cli_out("Fail to get LED firmware running status fail.\n");
            return;
        }
        if (start) {
            cli_out("FW download is not allowed since LED Firmware is running.\n");
            return;
        }
        rv = bcm_switch_led_fw_load(unit, 0, program, bytes);
        if (BCM_FAILURE(rv)) {
            cli_out("Fail to download LED firmware.\n");
            return;
        }

    } else {
#else
    {
#endif
        for (offset = 0; offset < CMIC_LED_PROGRAM_RAM_SIZE; offset++) {
             soc_pci_write(unit,
                           led_ptr->pram_base + CMIC_LED_REG_SIZE * offset,
                           (offset < bytes) ? (uint32) program[offset] : 0);
        }
        for (offset = 0x80; offset < CMIC_LED_DATA_RAM_SIZE; offset++) {
             soc_pci_write(unit,
                           led_ptr->dram_base + CMIC_LED_REG_SIZE * offset,
                           0);
        }
    }
}

#ifndef NO_FILEIO
STATIC cmd_result_t
ledproc_load_fp(int unit, char *cmd, char *file, FILE *f,
                led_info_t *led_ptr)
/*
 * Function:	ledproc_load_fp
 * Purpose:	Load a program into the LED microprocessor from a file
 * Parameters:	unit - StrataSwitch unit #
 *		cmd - Name of command for error printing
 *		file - Name of file for error printing
 *		f - Open stdio file file pointer containing program
 * Returns:	CMD_XXX
 *
 * Notes:	File format (ASCII FILE)
 *	00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *	10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *	... for a total of 256 bytes, white space ignored.
 */
{
    uint8	program[CMIC_LED_PROGRAM_RAM_SIZE];
    char        input[256], *cp = NULL, *error = NULL;
    int         bytes = 0, line = 0;
    int         offset_size = 0;

    while (!error && (cp = sal_fgets(input, sizeof(input) - 1, f))) {
        line++;

        while (*cp) {
            if (isspace((unsigned)(*cp))) { /* Skip whitespace */
                cp++;
            } else {
                if (!isxdigit((unsigned)*cp) ||
                    !isxdigit((unsigned)*(cp+1))) {
                    error = "Invalid hex digit";
                    break;
                }
                offset_size = sizeof(program);
                if (bytes >= offset_size) {
                    error = "Program memory exceeded";
                    break;
                }
                program[bytes++] = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
                cp += 2;
            }
        }
    }

    if (error) {
        cli_out("%s: Error loading file %s line %d: %s\n",
                cmd, file, line, error);
        return(CMD_FAIL);
    }

    cli_out("%s: Loading %d bytes into LED program memory\n",
            cmd, bytes);

    ledproc_load(unit, program, bytes,led_ptr);

    return(CMD_OK);
}
#endif /* NO_FILEIO */
/*
 * Function:	ledproc_load_args
 * Purpose:	Load a program into the LED microprocessor from an args_t
 * Parameters:	unit - StrataSwitch unit #
 *		a - Remainder of command line containing raw hex data
 * Returns:	CMD_XXX
 * Notes:	Input strings are concatenated and parsed the
 *		same way as ledproc_load_fp.
 */
STATIC cmd_result_t
ledproc_load_args(int unit, args_t *a, led_info_t *led_ptr)
/*
 * Notes:	Args format
 *	00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *	10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *	...
 * White space optional.
 */
{
    uint8	program[CMIC_LED_PROGRAM_RAM_SIZE];
    int		bytes = 0;
    char	*cp;
    int         offset_size = 0;

    sal_memset(program, 0, sizeof (program));

    while ((cp = ARG_GET(a)) != NULL) {
	while (*cp) {
	    if (isspace((int)(*cp))) {
		cp++;
	    } else {
		if (!isxdigit((unsigned)*cp) ||
		    !isxdigit((unsigned)*(cp+1))) {
		    cli_out("%s: Invalid character\n", ARG_CMD(a));
		    return(CMD_FAIL);
		}

                offset_size = sizeof(program);
		if (bytes >= offset_size) {
		    cli_out("%s: Program memory exceeded\n", ARG_CMD(a));
		    return(CMD_FAIL);
		}
		program[bytes++] = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
		cp += 2;
	    }
	}
    }

    ledproc_load(unit, program, bytes,led_ptr);

    return(CMD_OK);
}

/*
 * Function: ledproc_linkscan_cb
 * Purpose: Call back function for LEDs on link change.
 * Parameters: unit - unit number
 *             port - callback from this port
 *             info - pointer to structure giving status
 * Returns: nothing
 * Notes:   Each port has one byte of data at address (0x80 + portnum).
 *          For devices which fall under soc_feature_led_data_offset_a0
 *          the offset begins from 0xa0 instead of 0x80
 *      In each byte, bit 0 is used for link status.
 *      In each byte, bit 7 is used for turbo mode indication.
 */

STATIC void
ledproc_linkscan_cb(int unit, soc_port_t port, bcm_port_info_t *info)
{
    uint32	portdata = 0;
    int		byte = LS_LED_DATA_OFFSET + port;
    led_info_t *led_info;
    int led_ix;
    soc_info_t *si;
    int phy_port = 0;  /* physical port number */

    si = &SOC_INFO(unit);
    led_info = led_info_gen;
    led_ix = 0;

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        byte = LS_TUCANA_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_RAVEN_SUPPORT
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        byte = LS_RAVEN_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_RAPTOR1_SUPPORT
    if (SOC_IS_RAPTOR(unit)) {
        byte = LS_RAPTOR_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit)) {
        byte = LS_TR_VL_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_VALKYRIE_SUPPORT
    if (SOC_IS_VALKYRIE(unit)) {
        byte = LS_TR_VL_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        byte = LS_SC_CQ_LED_DATA_OFFSET + port;
    }
#endif

    if (soc_feature(unit, soc_feature_led_data_offset_a0)) {
        byte = LS_LED_DATA_OFFSET_A0 + port;
    }

    if (soc_feature(unit, soc_feature_logical_port_num)) {
        if (port >= 0 && port < SOC_MAX_NUM_PORTS) { /* safety check */
            phy_port = si->port_l2p_mapping[port];
            if (soc_feature(unit, soc_feature_led_data_offset_a0)) {
                byte = LS_LED_DATA_OFFSET_A0 + phy_port;
            } else {
                byte = LS_LED_DATA_OFFSET + phy_port;
            }
        }
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
        soc_led_link_status_t led_link_sts;
        uint8 led_control_data = 0;
        int speed = 0, rv = BCM_E_NONE;
        int led_uc, led_uc_port;

        led_link_sts.link_sts = (info->linkstatus == BCM_PORT_LINK_STATUS_UP);

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            led_link_sts.port = phy_port;
        } else {
            led_link_sts.port = port;
        }

        rv = bcm_switch_led_port_to_uc_port_get(unit, led_link_sts.port,
                                                &led_uc, &led_uc_port);
        if (BCM_FAILURE(rv)) {
            cli_out("Error rv %d: Fail to get led port from physical port.\n", rv);
        }

#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)

        /* Overwrite HW link status. */
        soc_cmicx_led_link_status(unit, &led_link_sts);
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */

        /* Collect link status and port speed then write to
         * LED firmware's led_control_data.
         */
        led_control_data = led_link_sts.link_sts;

        /* Get port speed info when link goes UP */
        if (led_link_sts.link_sts) {
            /* Use logical port number here */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
            if (SOC_IS_DNX(unit)) {
                bcm_port_resource_t resource;
                rv = bcm_port_resource_get(unit, port, &resource);
                speed = resource.speed;
            } else
#endif
            {
                rv = bcm_port_speed_get(unit, port, &speed);
            }
            if (BCM_FAILURE(rv)) {
                cli_out("Error rv %d: Unable to get speed for port %d\n", rv, port);
            }

            /* If speed > 100G then setting LED as green. */
            if (speed >= 100000) {
                led_control_data |= (LED_GREEN << 1);
            } else {
                led_control_data |= (LED_ORANGE << 1);
            }
        } else {
            led_control_data |= (LED_OFF << 1);
        }

        rv = bcm_switch_led_control_data_write(unit, led_uc, led_uc_port * sizeof(led_control_data),
                                               &led_control_data, sizeof(led_control_data));
        if (rv != BCM_E_NONE) {
            cli_out("Error rv %d: Unable to get speed for port %d\n", rv, port);
        }
#endif /* CMICX_LED */

        /* CMICx-LED port update complete. */
        return;
    }
#endif  /* BCM_CMICX_SUPPORT */


#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_QAX(unit)) {
        uint32 physical_phy;
        bcm_error_t ret;
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) { /* safety check */
            return;
        }
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove,
            (unit, phy_port , &physical_phy));
            if (ret != BCM_E_NONE) {
                return;
            }
            phy_port = physical_phy;
            byte = LS_LED_DATA_OFFSET_A0 + phy_port;
        }
    } else if (SOC_IS_JERICHO(unit)) {

        if (port < 0 || port >= SOC_MAX_NUM_PORTS) { /* safety check */
            return;
        }

        /* LED processor 0 - handles ports  1-24.  port 1  resides in index 1 in data ram */
        /* LED processor 1 - handles ports 25-84.  port 25 resides in index 1 in data ram */
        /* LED processor 2 - handles ports 85-144. port 85 resides in index 1 in data ram */

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            if (phy_port <= 24) {
                led_ix = 0;
            } else if (phy_port >= 25 && phy_port <= 84) {
                led_ix = 1;
                phy_port -= 24;
            } else if (phy_port > 84) {
                led_ix = 2;
                phy_port -= 84;
            }
            byte = LS_LED_DATA_OFFSET_A0 + phy_port;
        }

    }
#endif /* BCM_PETRA_SUPPORT */


#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit)) {
        /* trident first 36 ports in ledproc0, the other 36 ports in ledproc1 */
        /* triumph3 first 52 ports are in ledproc0, the other ports are in ledproc1 */
        /* Helix4: Port 1-48 and 53-64 are in ledproc0, rest are in ledproc1 - Adjustment made later */
        int ledup0_pmax = SOC_IS_TD_TT(unit) ? TRIDENT_LEDUP0_PORT_MAX :
                          (SOC_IS_HELIX4(unit) ? HELIX4_LEDUP_PORT_MAX : TRIUMPH3_LEDUP0_PORT_MAX);

        if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit)) && !SOC_IS_APACHE(unit)) {
            ledup0_pmax = TRIDENT2_LEDUP0_PORT_MAX;
        }

        if (SOC_IS_APACHE(unit)) {

            ledup0_pmax = APACHE_LEDUP_PORT_MAX;
        }

        if (SOC_IS_MONTEREY(unit)) {

            ledup0_pmax = MONTEREY_LEDUP_PORT_MAX;
        }
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) { /* safety check */
            return;
        }

        led_info = led_info_tri;
        phy_port = si->port_l2p_mapping[port];

        if ((phy_port > ledup0_pmax) && (!SOC_IS_TOMAHAWKX(unit))) {
            /* For Helix4 this condition should never be true */
            phy_port -= ledup0_pmax;
            led_ix = 1;
        }

        if (soc_feature(unit, soc_feature_led_data_offset_a0)) {
            if (SOC_IS_HELIX4(unit)) {
                if (phy_port > 48 && phy_port < 53) {
                    led_ix = 1;
                    phy_port -= 48;
                } else if (phy_port > 52) {
                    phy_port -= 4;
                }
                byte = LS_LED_DATA_OFFSET_A0 + phy_port;
            } else if (SOC_IS_TRIUMPH3(unit)) {
                if (led_ix) {
                    byte = LS_LED_DATA_OFFSET_A0 + phy_port + 4;
                } else {
                    byte = LS_LED_DATA_OFFSET_A0 + phy_port;
                    if (phy_port == 37) {
                        byte += 12;
                    } else if (phy_port >= 38) {
                        byte -= 4;
                    }
                }
            } else if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit)) && !SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
                int i, skip_count = 0;
                for (i=1; i<phy_port; i++) {
                    skip_count += ((si->port_p2l_mapping[i+(led_ix*TRIDENT2_LEDUP0_PORT_MAX)] == -1) ? 1: 0);
                }
                byte = LS_LED_DATA_OFFSET_A0 + phy_port - skip_count;
                if (SOC_IS_TRIDENT2(unit)) {
                    /*offset begin from 0xA0 */
                    byte -= 1;
                }
            } else if (SOC_IS_APACHE(unit)) {
                byte = LS_LED_DATA_OFFSET_A0 + phy_port - 1; /* Start from the A0 offset */
                if (phy_port > 36 && !SOC_IS_MONTEREY(unit)) { /* Ports 37-72 in ledproc1 */
                    phy_port -= APACHE_LEDUP_PORT_MAX;
                    led_ix = 1;
                }
            } else
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWK(unit)) {
                /* Tomahawk: Port 1-32 and 97-128 are in ledproc0
                 *           Port 33-96 are in ledproc1
                 *           Port 129 & 131 are in ledproc2
                 */
                if (phy_port > 32 && phy_port < 97) {
                    led_ix = 1;
                    phy_port -=  32;
                } else if (phy_port == 129 ) {
                    led_ix = 2;
                    phy_port = 1;
                } else if (phy_port == 131 ) {
                    led_ix = 2;
                    phy_port = 2;
                } else if (phy_port > 32) {
                    phy_port -= 64;
                }
                byte = LS_LED_DATA_OFFSET_A0 + phy_port - 1;
            } else
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                if (259 == phy_port) {
                    led_ix = (phy_port - 1) / 64;
                    byte = LS_LED_DATA_OFFSET_A0 + 1;
                } else {
                    led_ix = (phy_port - 1) / 64;
                    phy_port = (phy_port - 1) % 64 + 1;
                    byte = LS_LED_DATA_OFFSET_A0 + phy_port - 1;
                }
            } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                byte = LS_LED_DATA_OFFSET_A0 + phy_port;
            }
        } else {
            byte = LS_LED_DATA_OFFSET + phy_port;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        if (phy_port > GREYHOUND2_LEDUP0_PORT_MAX) {
            led_ix = 1;
            /* ledup1 physical port shifted value is GREYHOUND2_LEDUP0_PORT_MAX - 1. */
            phy_port -= (GREYHOUND2_LEDUP0_PORT_MAX - 1);
        }
        byte = LS_LED_DATA_OFFSET_A0 + phy_port;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        led_info = led_info_cmicm;
    }
#endif /* BCM_CMICM_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (soc_feature(unit, soc_feature_led_cmicd_v2)) {
        led_info = led_info_cmicd_v2;
    }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_PETRA_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_led_cmicd_v4)) {
        led_info = led_info_cmicd_v4;
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

    /* coverity[OVERRUN-LOCAL: FALSE] */
    portdata = soc_pci_read(unit,
                     led_info[led_ix].dram_base + CMIC_LED_REG_SIZE * byte);

    if (info->linkstatus == BCM_PORT_LINK_STATUS_UP) {
        portdata |= 0x01;
    } else {
        portdata &= ~0x01;
    }

    portdata &= ~0x80;

    soc_pci_write(unit, led_info[led_ix].dram_base + CMIC_LED_REG_SIZE * byte,
                      portdata);
}

char ledproc_usage[] =
    "Parameters: [num] [status | start | stop | load <file.hex> |\n\t"
    "            [auto [on | off]] | prog <hexdata> | dump\n\t"
    "Load the LED Microprocessor with the program specified in <file.hex>.\n\t"
    "Option 'auto' turns on (by default) or off automatic linkscan\n\t"
    "    updates to the LED processor.\n\t"
    "Option 'num' selects the specific LED processor. The default is 0\n";

cmd_result_t
ledproc_cmd(int unit, args_t *a)
/*
 * Function: 	sh_led
 * Purpose:	Load a led program and start it.
 * Parameters:	unit - unit
 *		a - args, each of the files to be displayed.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    VOL cmd_result_t		rv = CMD_OK;
#ifndef NO_FILEIO
#ifndef NO_CTRL_C
    jmp_buf			ctrl_c;
#endif
    FILE * volatile		f = NULL;
#endif
    char			*c;
    volatile uint32		led_ctrl, led_status;
    volatile int		auto_on;
    int led_max;
    led_info_t *led_info;
    led_info_t *led_info_cur;
    int ix, ix_max, show_uc_status;
    int phy_port_map = 0, led_up_instance = 0;
    int skip_force_link_check = FALSE;
#ifdef BCM_CMICX_SUPPORT
    int led_start = 0;
#endif /* BCM_CMICX_SUPPORT */

#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_PETRA_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)
    uint32  boot_flags;
#endif

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        return cmd_ltsw_ledproc(unit, a);
    }
#endif

    if (!sh_check_attached("ledproc", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_led_proc)) {
        cli_out("%s: LED Micro-controller not available on %s\n",
                ARG_CMD(a),
                soc_dev_name(unit));
        return(CMD_FAIL);
    }

    led_info = led_info_gen;
    led_max = 1;

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit)) {
        led_info = led_info_tri;
        led_max = TRIDENT_LEDUP_MAX;
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)){
        led_max = GREYHOUND2_LEDUP_MAX;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        led_info = led_info_cmicm;
#if defined(BCM_SABER2_SUPPORT)
        led_max = SABER2_LEDUP_MAX;
#endif
    }
#endif /* BCM_CMICM_SUPPORT */

/*
 * Change 'led info' clause: This was for BCM_PETRA_SUPPORT (Arad and Jericho
 * family)and was changed to (BCM_PETRA_SUPPORT || BCM_DNX_SUPPORT) (Jericho-2 family, DNX) to
 * enable using a similar configuration file (For DNX: dnx.doc)
 * In the future, this may be changed as per actual DNX board details.
 */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    if(soc_feature(unit, soc_feature_led_cmicd_v2)) {
        led_info = led_info_cmicd_v2;
        led_max = CMICD_V2_LEDUP_MAX;
    }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_PETRA_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
    if(soc_feature(unit, soc_feature_led_cmicd_v4)) {
        led_info = led_info_cmicd_v4;
        led_max = CMICD_V4_LEDUP_MAX;
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

#if (defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_PETRA_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)
    boot_flags = sal_boot_flags_get();
    if ((boot_flags & BOOT_F_WARM_BOOT) &&
        (!SOC_IS_ESW(unit) || SOC_IS_TOMAHAWK(unit))) {
        skip_force_link_check = TRUE;
    }
#endif

    if (bcm_linkscan_unregister(unit, ledproc_linkscan_cb) < 0) {
        auto_on = FALSE;
    } else {
        (void)bcm_linkscan_register(unit, ledproc_linkscan_cb);
        auto_on = TRUE;
    }

    ix = 0;
    show_uc_status = 0;

    /* If input argument count is zero then show all LED uc status. */
    if (ARG_CNT(a) == 0) {
        ix_max = led_max;
        show_uc_status = 1;
    } else {
        /* check if command has processor number specified */
        c = ARG_GET(a);
        if (isint(c)) {
            /* ix is specified here. */
            ix = parse_integer(c);
            if (ix > (led_max - 1)) {
                cli_out("Invalid uProcessor number: %d, set to 0\n", ix);
                ix = 0;
            }
            c = ARG_GET(a);
            if (c == NULL) {
                show_uc_status = 1;
                ix_max = ix + 1;
            }
        }
    }

    if (show_uc_status) {
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
            rv = bcm_switch_led_fw_start_get(unit, ix, &led_start);
            if (BCM_FAILURE(rv)) {
                cli_out("Get LED firmware status of start fail.\n");
                return (CMD_FAIL);
            }
            cli_out("LED M0 FW is %s Init DONE; "
                    "auto updating is %s.\n",
                    led_start ? "ON" : "OFF",
                    auto_on ? "ON" : "OFF");
#endif /* CMICX_LED */
            return(CMD_OK);
        }
#endif /* BCM_CMICX_SUPPORT */
        for (; ix < ix_max; ix++) {
            led_ctrl = soc_pci_read(unit, led_info[ix].ctrl);
            led_status = soc_pci_read(unit, led_info[ix].status);
            cli_out("%s: LED%d Micro-controller is %s (PC=0x%x State=%s%s); "
                    "auto updating is %s.\n",
                    ARG_CMD(a),
                    ix,
                    (led_ctrl & LC_LED_ENABLE) ? "ON" : "OFF",
                    (led_status & LS_LED_PC),
                    (led_status & LS_LED_INIT) ? "INIT," : "",
                    (led_status & LS_LED_RUN) ? "RUN" : "SLEEP",
                    auto_on ? "ON" : "OFF");
        }
        return(CMD_OK);
    }

    led_info_cur = led_info + ix;

#ifdef BCM_CMICX_SUPPORT
    if (!soc_feature(unit, soc_feature_cmicx))
#endif /* BCM_CMICX_SUPPORT */
    {
        /* Not for CMICx-LED. */
        led_ctrl = soc_pci_read(unit, led_info_cur->ctrl);
    }

    if (!sal_strcasecmp(c, "status")) {
        int     port;
        uint32	portdata = 0;
        int phy_port;
#ifdef BCM_TRIDENT_SUPPORT
        uint32 portdata1 = 0;
#endif
#ifdef BCM_CMICX_SUPPORT
#if CMICX_LED
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
        int rv = SOC_E_NONE;
        soc_led_port_status_t led_status;
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
#endif /* CMICX_LED */
#endif  /* BCM_CMICX_SUPPORT */

        /* coverity[overrun-local] */
        PBMP_PORT_ITER(unit, port) {

#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
                /* skipping over sfi ports */
                /* coverity[overrun-local] */
                if(IS_SFI_PORT(unit,port)) {
                    continue;
                }

                rv = bcm_switch_led_fw_start_get(unit, ix, &led_start);
                if (BCM_FAILURE(rv)) {
                    cli_out("Get LED firmware status of start fail.\n");
                    return (CMD_FAIL);
                }
                if (led_start == 0) {
                    cli_out("LED firmware is not running.\n");
                }
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
                if (soc_feature(unit, soc_feature_logical_port_num)) {
                    led_status.port = SOC_INFO(unit).port_l2p_mapping[port];
                } else {
                    led_status.port = port;
                }

                /* Display LED output ram status for the port */
                led_status.bank = LED_SEND_RAM_BANK;

                rv = soc_cmicx_led_status_get(unit, &led_status);

                if (rv != SOC_E_NONE) {
                    cli_out("Error: Unable to get LED status\n");
                    return(CMD_FAIL);
                } else {
                    cli_out("Port %d led status 0x%x\n", port, led_status.status);
                    continue;
                }
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
#else
                continue;
#endif /* CMICX_LED */
            }
#endif  /* BCM_CMICX_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
            /* SOC_IS_ARAD includes also later DNX fap devices */
            if (SOC_IS_ARAD(unit)) {
                /* skipping over sfi ports */
                /* coverity[overrun-local] */
                if(IS_SFI_PORT(unit,port)) {
                    continue;
                }
            }
#endif

            if (soc_feature(unit, soc_feature_logical_port_num)) {
                if (port >= SOC_MAX_NUM_PORTS) { /* safety check */
                    break;
                }
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }

#ifdef BCM_PETRA_SUPPORT
            if (SOC_IS_QAX(unit)) {
                uint32 physical_phy;
                bcm_error_t ret;
                ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove,
                (unit, phy_port , &physical_phy));
                if (ret != BCM_E_NONE) {
                    continue;
                }
                phy_port = physical_phy;

            } else if (SOC_IS_JERICHO(unit)) {

                /* LED processor 0 - will handle ports  1-24.  port 1  resides in index 1 in data ram */
                /* LED processor 1 - will handle ports 25-84.  port 25 resides in index 1 in data ram */
                /* LED processor 2 - will handle ports 85-144. port 85 resides in index 1 in data ram */

                if (phy_port <= 24) {
                    led_up_instance = 0;
                } else if (phy_port >= 25 && phy_port <= 84) {
                    led_up_instance = 1;
                    phy_port -= 24;
                } else if (phy_port > 84) {
                    led_up_instance = 2;
                    phy_port -= 84;
                }
                led_info_cur = led_info + led_up_instance;
            }
#endif /* BCM_PETRA_SUPPORT */


#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit)) {
                /* trident first 36 ports in ledproc0, the other 36 ports in ledproc1*/
                /* triumph3 first 52 ports are in ledproc0, the other ports are in ledproc1 */
                int ledup0_pmax = SOC_IS_TD_TT(unit) ? TRIDENT_LEDUP0_PORT_MAX :
                                  (SOC_IS_HELIX4(unit) ? HELIX4_LEDUP_PORT_MAX : TRIUMPH3_LEDUP0_PORT_MAX);
                if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit)) && !SOC_IS_APACHE(unit)) {
                    ledup0_pmax = TRIDENT2_LEDUP0_PORT_MAX;
                }

                if (SOC_IS_APACHE(unit)) {
                    ledup0_pmax = APACHE_LEDUP_PORT_MAX;
                }
                if (SOC_IS_MONTEREY(unit)) {
                    ledup0_pmax = MONTEREY_LEDUP_PORT_MAX;
                }

#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWK(unit)) {
                    /* Tomahawk: Port 1-32 and 97-128 are in ledproc0
                     *           Port 33-96 are in ledproc1
                     *           Port 129 & 131 are in ledproc2
                     */

                    if (phy_port == 129) {
                        led_info_cur = led_info + 2;
                        phy_port = 1;
                        led_up_instance = 2;
                    } else if (phy_port == 131) {
                        led_info_cur = led_info + 2;
                        phy_port = 2;
                        led_up_instance = 2;
                    } else if (phy_port > 32 && phy_port < 97) {
                        led_info_cur = led_info + 1;
                        phy_port -= 32;
                        led_up_instance = 1;
                    } else if (phy_port > 32) {
                        phy_port -= 64;
                        led_info_cur = led_info;
                        led_up_instance = 0;
                    }
                } else
#ifdef BCM_TOMAHAWK2_SUPPORT
                if (SOC_IS_TOMAHAWK2(unit)) {
                    led_up_instance = (phy_port - 1) / 64;
                    led_info_cur = led_info + led_up_instance;
                    phy_port = (phy_port - 1) % 64 + 1;
                } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
#endif /* BCM_TOMAHAWK_SUPPORT */
                    if (phy_port > ledup0_pmax) {
                        phy_port -= ledup0_pmax;
                        led_info_cur = led_info+1;
                        led_up_instance = 1;
                    } else {
                        led_info_cur = led_info;
                        led_up_instance = 0;
                    }
                if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit)) && !SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
                    int i, skip_count = 0;
                    for (i=1; i < phy_port; i++) {
                        skip_count += ((SOC_INFO(unit).port_p2l_mapping[i+(led_up_instance*TRIDENT2_LEDUP0_PORT_MAX)] == -1) ? 1: 0);
                    }
                    phy_port -= skip_count;
                }
            }
#endif /* BCM_TRIDENT_SUPPORT */

            /* Portmap */
            phy_port_map = phy_port;
            if (SOC_IS_TRIUMPH3(unit)) {
                if (led_up_instance) {
                    phy_port_map = phy_port + 4;
                } else {
                    phy_port_map = phy_port;
                    if (phy_port == 37) {
                        phy_port_map += 12;
                    } else if (phy_port >= 38) {
                        phy_port_map -= 4;
                    }
                }
            }
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                /* For Kt2 we start status from 1, not 0 */
                phy_port_map += 1;
            }

            /* phy_port_map ranges from 1 to 64, while status bits range from 0 to 63 */
#endif /* BCM_CMICM_SUPPORT */


#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD_TT(unit)) {
                uint32 byte;
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    /* link status is kept current in bit 0 of
                       data RAM byte (0xA0 + portnum) */
                    byte = LS_LED_DATA_OFFSET_A0 + phy_port_map - 1;
                    if (byte < 256) {
                        portdata1 = soc_pci_read(unit, led_info_cur->dram_base +
                                                 CMIC_LED_REG_SIZE * byte);

                        /* HW Port status in data RAM (0x00 + portnum) */
                        portdata = soc_pci_read(unit, led_info_cur->dram_base
                                + CMIC_LED_REG_SIZE * 2 * (phy_port_map - 1));
                        if (portdata1 & 0x1) {
                            portdata |= 0x80;
                        } else {
                            portdata &= ~0x80;
                        }
                    }
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    if (soc_feature(unit, soc_feature_led_data_offset_a0)) {
                        byte = LS_LED_DATA_OFFSET_A0 + phy_port_map;
                        if (SOC_IS_TRIDENT2(unit) || SOC_IS_APACHE(unit)) {
                            /*offset begin from 0xA0 */
                            byte -= 1;
                        }
                    } else {
                        byte = LS_LED_DATA_OFFSET + phy_port_map;
                    }

                    /* read tx/rx activity */
                    portdata1 = soc_pci_read(unit,
                                             led_info_cur->dram_base + CMIC_LED_REG_SIZE * byte);

                    if (portdata1 & 0x1) {
                        portdata |= 0x80;
                    } else {
                        portdata &= ~0x80;
                    }
                    if (portdata1 & 0x10) {
                        portdata |= 0x2;
                    } else {
                        portdata &= ~0x2;
                    }
                    if (portdata1 & 0x20) {
                        portdata |= 0x1;
                    } else {
                        portdata &= ~0x1;
                    }

                    if (SOC_IS_TRIDENT2PLUS(unit)) {
                        phy_port_map -= 1;
                    }

                    portdata1 = soc_pci_read(unit,
                                             led_info_cur->dram_base + CMIC_LED_REG_SIZE * (phy_port_map * 2 + 1));
                    portdata1 &= 0xFF;
                    portdata1 <<= 8;
                    portdata1 |= 0xFF & soc_pci_read(unit,
                                                      led_info_cur->dram_base + CMIC_LED_REG_SIZE * 2 * phy_port_map);
                }
            }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
            if (SOC_IS_ARAD(unit)) {

                uint32 byte;
                uint32 portdata1;

                /* link status is kept current in bit 0 of data RAM byte (0xA0 + portnum) */
                byte = LS_LED_DATA_OFFSET_A0 + phy_port_map;
                portdata1 = soc_pci_read(unit,
                                         led_info_cur->dram_base + CMIC_LED_REG_SIZE * byte);

                /* HW Port status in data RAM (0x00 + portnum) */
                portdata = soc_pci_read(unit, led_info_cur->dram_base
                                        + CMIC_LED_REG_SIZE * 2 * (phy_port_map));
                if (portdata1 & 0x1) {
                    portdata |= 0x80;
                } else {
                    portdata &= ~0x80;
                }
            }
#endif /* BCM_PETRA_SUPPORT */


            /* coverity[overrun-local] */
            if (soc_feature(unit, soc_feature_unimac) &&
                    ((IS_FE_PORT(unit, port)) || (IS_GE_PORT(unit, port)))) {
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    int speed = (portdata1 & 0x18) >> 3;
                    cli_out("%2d %4s %s %s %s %s %s %s %s %d:%d\n",
                            port,
                            SOC_PORT_VALID(unit, port) ?
                            SOC_PORT_NAME(unit, port) :"    ",
                            (portdata & 0x0001) ? "RX"  : "  " ,
                            (portdata & 0x0002) ? "TX"  : "  " ,
                            (portdata & 0x0080) ? "LI"  : "  " ,
                            (portdata1 & 0x0100)? "LE"  : "  " ,
                            (portdata1 & 0x0004)? "COL" : "   ",
                            (speed == 0) ? "10M  "  :
                            (speed == 1) ? "100M " :
                            (speed == 2) ? "1G   "   :
                            (speed == 3) ? ">=10G" : "    ",
                            (portdata1 & 0x0020) ? "HD"  : "FD" ,
                            led_up_instance, phy_port_map
                           );
                } else
#endif
                {
                    cli_out("%2d %4s %s %s %s %s %s %s %s %s %d:%d\n",
                          port,
                          SOC_PORT_VALID(unit, port) ?
                          SOC_PORT_NAME(unit, port) :"    ",
                          (portdata & 0x0001) ? "RX"  : "  " ,
                          (portdata & 0x0002) ? "TX"  : "  " ,
                          (portdata & 0x0080) ? "LI"  : "  " ,
                          (portdata & 0x0100) ? "LE"  : "  " ,
                          (portdata & 0x0004) ? "COL" : "   ",
                          (portdata & 0x0008) ? "100" : "   ",
                          (portdata & 0x0010) ? "1G"  : "  " ,
                          (portdata & 0x0020) ? "HD"  : "FD" ,
                          led_up_instance, phy_port_map
                         );
                }
            } else
#ifdef BCM_TOMAHAWK_SUPPORT
                if(SOC_IS_TOMAHAWKX(unit)) {
                    cli_out("%2d %4s %s %s %s %s %s %s %s %d:%d\n",
                            port,
                            SOC_PORT_VALID(unit, port) ?
                            SOC_PORT_NAME(unit, port) :"    ",
                            (portdata & 0x0001) ? "RX"  : "  " ,
                            (portdata & 0x0002) ? "TX"  : "  " ,
                            (portdata & 0x0080) ? "LI"  : "  " ,
                            (portdata & 0x0100) ? "LE"  : "  " ,
                            (portdata & 0x0004) ? "COL" : "   ",
                            (portdata & 0x0018) ? "10G" : "   ",
                            (portdata & 0x0020) ? "HD"  : "FD" ,
                            led_up_instance, phy_port_map
                           );
             } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                        int speed = (portdata1 & 0x18) >> 3;
                        cli_out("%2d %4s %s %s %s %s %s %s %s %d:%d\n",
                                port,
                                SOC_PORT_VALID(unit, port) ?
                                SOC_PORT_NAME(unit, port) :"    ",
                                (portdata  & 0x0001) ? "RX"  : "  " ,
                                (portdata  & 0x0002) ? "TX"  : "  " ,
                                (portdata  & 0x0080) ? "LI"  : "  " ,
                                (portdata1 & 0x0100) ? "LE"  : "  " ,
                                (portdata1 & 0x0004) ? "COL" : "   ",
                                (speed == 0) ? "10M  "  :
                                (speed == 1) ? "100M " :
                                (speed == 2) ? "1G   "   :
                                (speed == 3) ? ">=10G" : "    ",
                                (portdata1 & 0x0020) ? "FD"  : "HD" ,
                                led_up_instance, phy_port_map
                        );
                 } else
#endif /* BCM_TRIDENT_SUPPORT */
                 {
                    cli_out("%2d %4s %s %s %s %s %s %s %s %s %d:%d\n",
                            port,
                            SOC_PORT_VALID(unit, port) ?
                            SOC_PORT_NAME(unit, port) :"    ",
                            (portdata & 0x0001) ? "RX"  : "  " ,
                            (portdata & 0x0002) ? "TX"  : "  " ,
                            (portdata & 0x0080) ? "LI"  : "  " ,
                            (portdata & 0x0100) ? "LE"  : "  " ,
                            (portdata & 0x0004) ? "COL" : "   ",
                            (portdata & 0x0008) ? "100" : "   ",
                            (portdata & 0x0010) ? "1G"  : "  " ,
                            (portdata & 0x0020) ? "FD"  : "HD" ,
                            led_up_instance, phy_port_map
                           );
                 }
            }
        } else if (!sal_strcasecmp(c, "start")) {
        int		offset;

#if defined (BCM_CMICX_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
            int rv = SOC_E_NONE;
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
            rv = bcm_switch_led_fw_start_set(unit, ix, LED_FW_START);
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
            if (rv != SOC_E_NONE) {
                cli_out("Error:Unable to start LED FW\n");
                return(CMD_FAIL);
            } else {
                return(CMD_OK);
            }
#else /* CMICX_LED */
            return(CMD_OK);
#endif /* CMICX_LED */
        }
#endif /* BCM_CMICX_SUPPORT */

        /* The LED data area should be clear whenever program starts */

        soc_pci_write(unit, led_info_cur->ctrl, led_ctrl & ~LC_LED_ENABLE);

        for (offset = 0x80; offset < CMIC_LED_DATA_RAM_SIZE; offset++) {
            soc_pci_write(unit,
                    led_info_cur->dram_base + CMIC_LED_REG_SIZE * offset,
                    0);
        }

        led_ctrl |= LC_LED_ENABLE;

        soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);

        if (auto_on && !skip_force_link_check) {
            (void)bcm_link_change(unit, PBMP_PORT_ALL(unit));
        }

        return(CMD_OK);
    } else if (!sal_strcasecmp(c, "stop")) {

#if defined (BCM_CMICX_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
            int rv = BCM_E_NONE;
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
            rv = bcm_switch_led_fw_start_set(unit, ix, LED_FW_STOP);
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT || BCM_DNX_SUPPORT */
            if (BCM_FAILURE(rv)) {
                cli_out("Error:Unable to stop LED FW\n");
                return(CMD_FAIL);
            } else {
                return(CMD_OK);
            }
#else /* CMICX_LED */
            return(CMD_OK);
#endif /* CMICX_LED */
        }
#endif /* BCM_CMICX_SUPPORT */

        led_ctrl &= ~LC_LED_ENABLE;
        soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);

        return(CMD_OK);
    } else if (!sal_strcasecmp(c, "load")) {
        /*
         * Try to catch ^C to avoid leaving file open if ^C'd.
         * There are still a number of unlikely race conditions here.
         * Temporarily stop LED processor if it is currently running.
         */
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
#else

        /* Stop LED processor if it was running. And keep previous state*/
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            bcm_switch_led_fw_start_get(unit, ix, &led_start);
            bcm_switch_led_fw_start_set(unit, ix, LED_FW_STOP);
        } else {
#else
        {
#endif /* BCM_CMICX_SUPPORT */
            soc_pci_write(unit, led_info_cur->ctrl, led_ctrl & ~LC_LED_ENABLE);
        }

        if (
#ifndef NO_CTRL_C
                !setjmp(ctrl_c)
#else
                TRUE
#endif
           ) {
#ifndef NO_CTRL_C
            sh_push_ctrl_c(&ctrl_c);
#endif

            if ((c = ARG_GET(a)) != NULL) {
                f = sal_fopen(c, "r");
                if (!f) {
                    cli_out("%s: Error: Unable to open file: %s\n",
                            ARG_CMD(a), c);
                    rv = CMD_FAIL;
                } else {
                    rv = ledproc_load_fp(unit, ARG_CMD(a), c,
                            (FILE *)f,led_info_cur);
                    sal_fclose((FILE *)f); /* Cast for un-volatile */
                    f = NULL;
                }
            } else {
                cli_out("%s: Error: No file specified\n", ARG_CMD(a));
                rv = CMD_USAGE;
            }
        } else if (f) {
            sal_fclose((FILE *)f);
            f = NULL;
            rv = CMD_INTR;
        }

        /* Restore LED processor if it was running */
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            bcm_switch_led_fw_start_set(unit, ix, led_start);
        } else {
#else
        {
#endif
            soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);
        }

        if (auto_on && !skip_force_link_check) {
            (void)bcm_link_change(unit, PBMP_PORT_ALL(unit));
        }

#ifndef NO_CTRL_C
        sh_pop_ctrl_c();
#endif
#endif /* NO_FILEIO */
    } else if (!sal_strcasecmp(c, "prog")) {
        if (ARG_CUR(a) == NULL) {
            return(CMD_USAGE);
        }

        /* Stop LED processor if it was running. */
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            bcm_switch_led_fw_start_set(unit, ix, LED_FW_STOP);
        } else {
#else
        {
#endif /* BCM_CMICX_SUPPORT */
           led_ctrl &= ~LC_LED_ENABLE;
           soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);
        }

        return ledproc_load_args(unit, a, led_info_cur);
    } else if (!sal_strcasecmp(c, "auto")) {
        int new_auto = 1;

        if ((c = ARG_GET(a)) != NULL) {
            if (!sal_strcasecmp(c, "off")) {
                new_auto = 0;
            } else if (!sal_strcasecmp(c, "on")) {
                new_auto = 1;
            } else {
                return CMD_USAGE;
            }
        }

        if (new_auto && !auto_on) {
            (void)bcm_linkscan_register(unit, ledproc_linkscan_cb);
        }

        if (!new_auto && auto_on) {
            (void)bcm_linkscan_unregister(unit, ledproc_linkscan_cb);
        }

        if (new_auto && !skip_force_link_check) {
            (void)bcm_link_change(unit, PBMP_PORT_ALL(unit));
        }

    } else if (!sal_strcasecmp(c, "dump")) {
        int offset, max;

#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_cmicx_led_fw_dump(unit, CMICX_LED, CMIC_LED_PROGRAM_RAM_SIZE);
            return CMD_OK;
        }
#endif /* BCM_CMICX_SUPPORT */
        for (max = CMIC_LED_PROGRAM_RAM_SIZE - 1; max >= 0; max--) {
            if (soc_pci_read(unit,
                        led_info_cur->pram_base + CMIC_LED_REG_SIZE * max) != 0) {
                break;
            }
        }

        for (offset = 0; offset <= max; offset++) {
            cli_out(" %02x", soc_pci_read(unit,
                        led_info_cur->pram_base + CMIC_LED_REG_SIZE * offset));
            if ((offset % 16) == 15) {
                cli_out("\n");
            }
        }

        if (offset % 16 != 0) {
            cli_out("\n");
        }
#ifdef BCM_CMICX_SUPPORT
    } else if (!sal_strcasecmp(c, "setstatus")) {
        if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
            int rv = SOC_E_NONE;
            soc_led_port_status_t led_status;

            if ((c = ARG_GET(a)) != NULL) {
                led_status.port = sal_ctoi(c, 0);
            }

            if ((c = ARG_GET(a)) != NULL) {
                led_status.status = sal_ctoi(c, 0);
            }
            rv = soc_cmicx_led_status_set(unit, &led_status);
            if (rv != SOC_E_NONE) {
                cli_out("Error: Unable to set led status\n");
                return(CMD_FAIL);
            }
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
#endif /* CMICX_LED */
        } else {
            cli_out("Unsupported LED CLI command\n");
            return(CMD_FAIL);
        }
    } else if (!sal_strcasecmp(c, "accustatus")) {
            if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_LED
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
                int rv = SOC_E_NONE, port;
                soc_led_port_status_t led_status;

                rv = bcm_switch_led_fw_start_get(unit, ix, &led_start);
                if (BCM_FAILURE(rv)) {
                    cli_out("Get LED firmware status of start fail.\n");
                    return (CMD_FAIL);
                }
                if (led_start == 0) {
                    cli_out("LED firmware is not running.\n");
                }

                /* coverity[overrun-local] */
                PBMP_PORT_ITER(unit, port) {
                    /* skipping over sfi ports */
                    if(IS_SFI_PORT(unit,port)) {
                        continue;
                    }
                    if (soc_feature(unit, soc_feature_logical_port_num)) {
                        led_status.port = SOC_INFO(unit).port_l2p_mapping[port];
                    } else {
                        led_status.port = port;
                    }

                    /* Display LED accu_ram bank status for the port */
                    led_status.bank = LED_ACCU_RAM_BANK;
                    rv = soc_cmicx_led_status_get(unit, &led_status);
                    if (rv != SOC_E_NONE) {
                        cli_out("Error: Unable to get LED status\n");
                        return(CMD_FAIL);
                    } else {
                        cli_out("Port %d led accu ram status 0x%x\n", port, led_status.status);
                        continue;
                    }
                }
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
#endif /* CMICX_LED */
        } else {
            cli_out("Unsupported LED CLI command\n");
            return(CMD_FAIL);
        }
#endif  /* BCM_CMICX_SUPPORT */
    } else {
        return(CMD_USAGE);
    }

    return(rv);
}
