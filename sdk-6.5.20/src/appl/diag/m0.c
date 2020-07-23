/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	m0.c
 * Purpose: 	ARM Cortex-M0 subsystem quad (M0SSQ) Support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>

#if defined(BCM_CMICX_SUPPORT)
#include <soc/iproc.h>
#include <shared/cmicfw/m0_ver.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/cmicfw/iproc_fwconfig.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#if defined (BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/linkctrl.h>
#endif
#define M0FW_FILENAME_SIZE            (256)
#define _SOC_M0FW_DEFAULT_PATHS_NUM   (7)

#ifndef NO_FILEIO
STATIC int
_soc_m0fw_default_filename(int unit, char *filename) {
    int rv = SOC_E_NONE;
    int i = 0, found = 0;
    FILE *fp = NULL;
    char filepath[M0FW_FILENAME_SIZE] = {0};

    /* Search for M0 FW binary file in these locations */
    char paths[_SOC_M0FW_DEFAULT_PATHS_NUM][80] = \
                {"../../rc/cmicfw/", "../rc/cmicfw", "rc/cmicfw/", "cmicfw/",
                 "../../../rc/cmicfw/","../../../../rc/cmicfw/", ""};

    if (sal_strlen(filename) > M0FW_FILENAME_SIZE) {
        cli_out("Filename cannot be longer than %d\n", M0FW_FILENAME_SIZE);
        return SOC_E_PARAM;
    }

    for(i = 0; i < _SOC_M0FW_DEFAULT_PATHS_NUM; i++) {
        sal_strncpy(filepath, paths[i], M0FW_FILENAME_SIZE-1);
        /* M0 FW binary filename is fixed */
        sal_strncat(filepath, filename, sal_strlen(filename));
        fp = sal_fopen(filepath, "rb");
        if (fp) {
            found = 1;
            strncpy(filename, filepath, M0FW_FILENAME_SIZE-1);
            sal_fclose(fp);
            break;
        }
    }

    if (!found) {
        rv = SOC_E_EXISTS;
    }

    return rv;
}
#endif /* NO_FILEIO */


char m0_usage[] =
    "Parameters: <command> [<ucore>] [<address>] [<file>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tcommand = [load/status/stats/start/stop/log]\n"
    "\tucore   = Cortex-M0 ucore number to be loaded.\n"
    "\taddress = offset of shared SRAM to load binary file\n"
    "\tfile    = binary file name\n"
#endif
    ;

cmd_result_t
m0_cmd(int unit, args_t *a)
/*
 * Function: 	m0_cmd
 * Purpose:	Load a file into M0SSQ Cortex-M0 Private TCM
 * Parameters:	unit - unit
 *		a - args, each of the files to be displayed.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    cmd_result_t rv = CMD_OK;
    char *c;
    int ucnum;
    int err;
    soc_stat_t          *stat;
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
    soc_ls_heartbeat_t ls_hbeat;
    int image_size = 0;
    uint32 load_crc, image_crc;
#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
#ifndef NO_FILEIO
    uint8 *buf;
    uint32 addr;
    uint32 crc32;
    uint8 tmp_buf[512];
    int fsize = 0, size = 0, pos;
    char filename[M0FW_FILENAME_SIZE] = {0};
#endif

#ifndef NO_FILEIO
#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif
    FILE * volatile fp = NULL;
#endif

    /* check for simulation*/
    if (SAL_BOOT_BCMSIM) {
        return(rv);
    }

    if (!soc_feature(unit, soc_feature_cmicx)) {
        return (CMD_FAIL);
    }

    if (!sh_check_attached("m0", unit)) {
        return(CMD_FAIL);
    }

    if (!ARG_CNT(a)) {
        return(CMD_USAGE);
    }

    stat = SOC_STAT(unit);
    c = ARG_GET(a);

    if (!sal_strcasecmp(c, "load")) {
        c = ARG_GET(a);
        if (!isint(c)) {
            cli_out("%s: Error: uC Num not specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }

        ucnum = parse_integer(c);
        if (ucnum < 0 || ucnum > 3 /* SOC_INFO(unit).num_ucs */) {
            cli_out("Invalid uCnum number: %d, must be in the range 0 to 3", ucnum);
            return(CMD_FAIL);
        }

#ifndef NO_FILEIO
        c = ARG_GET(a);
        if (!isint(c)) {
            cli_out("%s: Error: Address offset not specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }
        addr = parse_address(c);
#endif

        c = ARG_GET(a);

        if (c == NULL) {
            cli_out("%s: Error: No file specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }

#ifdef NO_FILEIO
        cli_out("no filesystem\n");
#else
        if (sal_strlen(c) > M0FW_FILENAME_SIZE) {
            cli_out("Filename cannot be longer than %d\n", M0FW_FILENAME_SIZE);
            return(CMD_FAIL);
        }

        sal_strncpy(filename, c, sal_strlen(c));

        /* Check if file can be opened */
        fp = sal_fopen(filename, "rb");

        if (fp) {
            sal_fclose((FILE *)fp);
            fp = NULL;
        } else {
            /* Search for M0 FW binary file in default locations */
            err = _soc_m0fw_default_filename(unit, filename);
            if (err != SOC_E_NONE) {
                cli_out("%s: Error: M0 Firmware file not found!!\n", ARG_CMD(a));
                return (CMD_FAIL);
            }
        }
        cli_out("Loading M0 Firmware located at %s\n", filename);
#endif /* NO_FILEIO */

#ifdef NO_FILEIO
        cli_out("no filesystem\n");
#else
#ifndef NO_CTRL_C
        if (!setjmp(ctrl_c)) {
             sh_push_ctrl_c(&ctrl_c);
#endif
             /* See if we can open the file before doing anything else */
            fp = sal_fopen(filename, "rb");
            if (!fp) {
                cli_out("%s: Error: Unable to open file: %s\n",
                        ARG_CMD(a), filename);
                rv = CMD_FAIL;
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return rv;
            }

            fsize = sal_fsize(fp);

            if (fsize <= 0) {
                cli_out("%s: Error: file size is not valid: size = %d\n",
                        ARG_CMD(a), fsize);
                sal_fclose((FILE *)fp);
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return CMD_FAIL;
            }

            buf = sal_alloc(fsize + 4, "M0FW");
            if (buf == NULL) {
                cli_out("%s: Memory allocate fail\n", ARG_CMD(a));
                sal_fclose((FILE *)fp);
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return CMD_FAIL;
            }

            /* Add secure fread to prevent coverity error. */
            pos = 0;
            do {
                size = sal_fread(tmp_buf, 1, sizeof(tmp_buf), fp);
                if (size <= 0) {
                    break;
                }
                sal_memcpy(&buf[pos], tmp_buf, size);
                pos += size;
            } while (1);

            crc32 = shr_crc32(~0, buf, fsize);
            rv = soc_iproc_m0ssq_uc_fw_load(unit, ucnum, addr, buf, fsize);

            sal_free(buf);
            sal_fclose((FILE *)fp);
            fp = NULL;
            buf = NULL;
#ifndef NO_CTRL_C
            sh_pop_ctrl_c();
#endif
            if (SOC_FAILURE(rv)) {
                cli_out("%s: Error: firmware download failed\n", ARG_CMD(a));
                return CMD_FAIL;
            } else {
                cli_out("Firmware download successed (0x%x).\n", crc32);
                return CMD_OK;
            }

#ifndef NO_CTRL_C
        } else if (fp) {
            sal_fclose((FILE *)fp);
            fp = NULL;
            rv = CMD_INTR;
        }
        sh_pop_ctrl_c();
#endif
#endif /* NO_FILEIO */
        sal_usleep(10000);
    } else if (!sal_strcasecmp(c, "status")) {
#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
        int st = 0;

        rv = soc_iproc_m0ssq_uc_fw_crc(unit, 0, &image_size, &load_crc, &image_crc);
        st = soc_cmicx_linkscan_heartbeat(unit, &ls_hbeat);
        cli_out("M0 FW is %s\n", ((st == SOC_E_NONE) ? "Running" : "NOT Running"));
        cli_out("M0 FW Version is %d.%d, FW size is %d, crc=0x%x\n",
                (ls_hbeat.m0_fw_version >> 16), (ls_hbeat.m0_fw_version & 0xFFFF), 
                image_size, image_crc);
        if (load_crc != image_crc)
        {
            cli_out("    M0FW CRC do not match load CRC(load crc=0x%x)!!!\n", load_crc);
        }
        cli_out("Host FW Version is %d.%d (%x)\n",
                (ls_hbeat.host_fw_version >> 16), (ls_hbeat.host_fw_version & 0xFFFF), HOST_FW_SIGNATURE);
        if (ls_hbeat.host_fw_version != ls_hbeat.m0_fw_version) {
            cli_out("Host and M0 FW Versions do not match!!!\n");
        }

#endif /* BCM_ESW_SUPPORT || BCM_DNXF_SUPPORT */
    } else if (!sal_strcasecmp(c, "stats")) {
        cli_out("M0: msg=%d resp=%d intr=%d\n",
                stat->m0_msg, stat->m0_resp,
                stat->m0_intr);

        cli_out("Uc sw programmable total intr=%d\n",
             stat->uc_sw_prg_intr);

        cli_out("M0 sw programmable intr: u0=%d u1=%d u2=%d u3=%d\n",
             stat->m0_u0_sw_intr, stat->m0_u1_sw_intr,
             stat->m0_u2_sw_intr, stat->m0_u3_sw_intr);
#ifdef U0_DEBUG
    } else if (!sal_strcasecmp(c, "log")) {

        err = soc_iproc_m0ssq_log_dump(unit);
        if (err == SOC_E_UNAVAIL) {
            cli_out("M0 log is not available.\n");
        } else if (SOC_FAILURE(err)) {
            cli_out("M0 log dump fail\n");
        }
#endif
    } else if (!sal_strcasecmp(c, "stop")) {

        c = ARG_GET(a);
        if (!isint(c)) {
            cli_out("%s: Error: uC Num not specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }

        ucnum = parse_integer(c);
        if (ucnum < 0 || ucnum > 3 /* SOC_INFO(unit).num_ucs */) {
            cli_out("Invalid uCnum number: %d, must be in the range 0 to 3", ucnum);
            return(CMD_FAIL);
        }

        err = soc_iproc_m0ssq_reset_ucore(unit, ucnum, 1);
        if (SOC_FAILURE(err)) {
            cli_out("M0 stop fail\n");
        }

    } else if  (!sal_strcasecmp(c, "start")) {

        c = ARG_GET(a);
        if (!isint(c)) {
            cli_out("%s: Error: uC Num not specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }

        ucnum = parse_integer(c);
        if (ucnum < 0 || ucnum > 3 /* SOC_INFO(unit).num_ucs */) {
            cli_out("Invalid uCnum number: %d, must be in the range 0 to 3", ucnum);
            return(CMD_FAIL);
        }

        err = soc_iproc_m0ssq_reset_ucore(unit, ucnum, 0);
        if (SOC_FAILURE(err)) {
            cli_out("M0 start fail\n");
        }
#if defined (BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    } else if  (!sal_strcasecmp(c, "pause")) {
        err = soc_linkctrl_linkscan_pause(unit);
        if (SOC_FAILURE(err)) {
            cli_out("M0 linkscan pause fail\n");
        }
    } else if  (!sal_strcasecmp(c, "continue")) {
        err = soc_linkctrl_linkscan_continue(unit);
        if (SOC_FAILURE(err)) {
            cli_out("M0 linkscan continue fail\n");
        }
#endif
    }

    return(rv);
}
#endif /* CMICX support */
