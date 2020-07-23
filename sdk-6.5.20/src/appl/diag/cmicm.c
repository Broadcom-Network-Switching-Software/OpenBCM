/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMICm peripherals commands
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/mspi.h>
#include <soc/cmicm.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include "appl/diag/diag.h"

#ifdef BCM_CMICM_SUPPORT

#define CMICM_MSPI_BLOCK_SIZE 16


char mspi_usage[] =
    "Usage :\n\t"
    "mspi [device = n] [CPHA=n][CPOL=n]\n\t"
    "mspi [device = n] [CPHA=n][CPOL=n] load <hexfile>\n\t"
    "mspi [device = n] [CPHA=n][CPOL=n] write <hexdata>\n\t"
    "mspi [device = n] [CPHA=n][CPOL=n] read <numbytes>\n\t"
    "mspi [device = n] [CPHA=n][CPOL=n] write <hexdata> read <numbytes>\n\t"
    "Optionally select a device and configres. The device number is specific to chip/board.\n\t"
    "<hexfile> is ASCII file, with hex bytes separated by white space\n";

cmd_result_t
mspi_cmd(int unit, args_t *a)
{
    volatile cmd_result_t rv = CMD_OK;
    char *c , *filename;
    int dev,cpol, cpha, offset_size = 0;
    volatile int wbytes = 0, rbytes=0;
    int start_byte, num_wbytes, num_rbytes;
    uint8 wdata[256];
    uint8 rdata[256];
    parse_table_t pt;
#ifndef NO_FILEIO
    char    input[256];
#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif
    FILE * volatile fp = NULL;
#endif

    if (!sh_check_attached("mspi", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_cmicm)) {
        return(CMD_FAIL);
    }

    if (ARG_CNT(a) == 0) {
        return CMD_USAGE;
    }

    sal_memset(rdata, 0, sizeof (rdata));
    dev=-1;
    cpol=-1;
    cpha=-1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Device",  PQ_DFL|PQ_INT,
                                 (void *)( 0), &dev, NULL);
    parse_table_add(&pt, "CPOL",  PQ_DFL|PQ_INT,
                                 (void *)( 0), &cpol, NULL);
    parse_table_add(&pt, "CPHA",  PQ_DFL|PQ_INT,
                                (void *)( 0), &cpha, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    if ((dev != -1) || (cpol != -1) || (cpha != -1)) {
        if (soc_mspi_config(unit, dev, cpol, cpha) != SOC_E_NONE){
            cli_out("MSPI: Failure Configuring\n");
            return CMD_FAIL;
        }
    }

    if (ARG_CNT(a) == 0) {
        return CMD_OK;
    }

     c = ARG_GET(a);

    if (!sal_strcasecmp(c, "load")) {
        c = ARG_GET(a);
        filename = c;
        if (filename == NULL) {
            cli_out("MSPI: %s: Error: No file specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
        rv = CMD_FAIL;
#else
#ifndef NO_CTRL_C
        if (!setjmp(ctrl_c)) {
            sh_push_ctrl_c(&ctrl_c);
#endif            
            fp = sal_fopen(filename, "r");
            if (!fp) {
                cli_out("MSPI: %s: Error: Unable to open file: %s\n",
                        ARG_CMD(a), filename);
                rv = CMD_FAIL;
            } else {
                while ((rv == CMD_OK) && (c = sal_fgets(input, sizeof(input) - 1, fp))) {
                    while (*c) {
                        if (isspace((unsigned)(*c))) { /* Skip whitespace */
                            c++;
                        } else {
                            if (!isxdigit((unsigned)*c) ||
                                !isxdigit((unsigned)*(c+1))) {
                                cli_out("MSPI: %s: Invalid character\n", ARG_CMD(a));
                                rv = CMD_FAIL;
                                break;
                            }
                            offset_size = sizeof(wdata);
                            if (wbytes >= offset_size) {
                                cli_out("MSPI: %s: Data memory exceeded\n", ARG_CMD(a));
                                rv = CMD_FAIL;
                                break;
                            }
                            wdata[wbytes++] = (xdigit2i(*c) << 4) | xdigit2i(*(c + 1));
                            c += 2;
                        }
                    }
                }
                sal_fclose((FILE *)fp);
                fp = NULL;
            }
#ifndef NO_CTRL_C
        } else if (fp) {
            sal_fclose((FILE *)fp);
            fp = NULL;
            rv = CMD_INTR;
        }
#endif        

        sh_pop_ctrl_c();
#endif /* NO_FILEIO */

        if (rv != CMD_OK) {
            return rv;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MSPI: Writing %d bytes from file %s\n"), wbytes, filename));
        for(start_byte=0; start_byte < wbytes; start_byte+=CMICM_MSPI_BLOCK_SIZE) {
            num_wbytes = ((start_byte + CMICM_MSPI_BLOCK_SIZE) > wbytes) ?
                        (wbytes - start_byte) : CMICM_MSPI_BLOCK_SIZE;
            if (soc_mspi_write8(unit, (uint8 *) &wdata[start_byte], num_wbytes) != SOC_E_NONE) {
                cli_out("MSPI: Write Fail\n");
                return CMD_FAIL;
            }
        }
        return rv;
    }

    if (sal_strcasecmp(c, "write") &&
        sal_strcasecmp(c, "read")) {
        cli_out("Neither Write Nor Read\n");
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(c, "write")) {
        while ((c = ARG_GET(a)) != NULL) {
            if (!sal_strcasecmp(c, "read")) {
                break;
            }
            while (*c) {
                if (isspace((int)(*c))) {
                    c++;
                } else {
                    if (!isxdigit((unsigned)*c) ||
                        !isxdigit((unsigned)*(c+1))) {
                        cli_out("MSPI: %s: Invalid character\n", ARG_CMD(a));
                        return(CMD_FAIL);
                    }
                    offset_size = sizeof(wdata);
                    if (wbytes >= offset_size) {
                        cli_out("MSPI: %s: Data memory exceeded\n", ARG_CMD(a));
                        return(CMD_FAIL);
                    }
                    wdata[wbytes++] = (xdigit2i(*c) << 4) | xdigit2i(*(c + 1));
                    c += 2;
                }
            }
        }
    }

    if (c != NULL) {
        if (!sal_strcasecmp(c, "read")) {
            if (ARG_CNT(a) == 0) {
                return CMD_USAGE;
            }
            c = ARG_GET(a);
            if (!isint(c)) {
                return CMD_USAGE;
            }
            rbytes = parse_integer(c);
        }
    }

    if ((wbytes > 0) && (rbytes > 0)) { /* Write and read */
        if ((wbytes + rbytes) > CMICM_MSPI_BLOCK_SIZE) {
            cli_out("MSPI: Too many bytes for single Write-read operation\n");
            return(CMD_FAIL);
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MSPI: Writing %d bytes and Reading %d bytes\n"),
                     wbytes, rbytes));
        if (soc_mspi_writeread8(unit, (uint8 *) &wdata[0], wbytes,(uint8 *) &rdata[0], rbytes) != SOC_E_NONE) {
            cli_out("MSPI: Write-Read Fail\n");
            return CMD_FAIL;
        }
    } else if (wbytes > 0){     /* Write Only */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MSPI: Writing %d bytes\n"), wbytes));
        for(start_byte=0; start_byte < wbytes; start_byte+=CMICM_MSPI_BLOCK_SIZE) {
            num_wbytes = ((start_byte + CMICM_MSPI_BLOCK_SIZE) > wbytes) ?
                        (wbytes - start_byte) : CMICM_MSPI_BLOCK_SIZE;
            if (soc_mspi_write8(unit, (uint8 *) &wdata[start_byte], num_wbytes) != SOC_E_NONE) {
                cli_out("MSPI: Write Fail\n");
                return CMD_FAIL;
            }
        }
    } else if (rbytes > 0){     /* Read Only */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MSPI: Reading %d bytes\n"), rbytes));
        for(start_byte=0; start_byte < rbytes; start_byte+=CMICM_MSPI_BLOCK_SIZE) {
            num_rbytes = ((start_byte + CMICM_MSPI_BLOCK_SIZE) > rbytes) ?
                        (rbytes - start_byte) : CMICM_MSPI_BLOCK_SIZE;
            if (soc_mspi_read8(unit, (uint8 *) &rdata[start_byte], num_rbytes) != SOC_E_NONE) {
                cli_out("MSPI: Read Fail\n");
                return CMD_FAIL;
            }
        }
    } else {
        return CMD_USAGE;
    }

    if (rbytes > 0) {
        /* Dump It */
        for(start_byte=0; start_byte < rbytes; start_byte++) {
            cli_out("%02x ", rdata[start_byte]);
            if ((start_byte % 8) == 7) {
                cli_out("\n");
            }
        }
        cli_out("\n");
    }

    return rv;
}

char dpll_usage[] =
    "dpll [Config = n] [<Options>] write <addr> <data>\n\t"
    "dpll [Config = n] [<Options>] read <addr>\n\t"
    "\"Config\" overrides all the options with preset values\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tTakes default <options> from \"dpll_params\" config variable if present\n"
    "\tOptions are the following, separated by space:\n\t"
    "  DevSel=n : Override Mux for DPLL Selection\n\t" 
    "  CPOL=0/1 : Clock Polarity\n\t"
    "  CPHA=0/1 : Clock Phase\n\t"
    "  AddrBitOrder=0/1 : 0 = MSB..LSB, 1 = LSB..MSB\n\t"
    "  DataBitOrder=0/1 : 0 = MSB..LSB, 1 = LSB..MSB\n\t"
    "  AddrWidth=1/2 : No of Bytes used for Addr\n\t"
    "  UseBrstBit-0/1 : Reserve Bit-0 of Addr as Burst Bit\n\t"
    "  UseRwBit=0/1 : Use MSBit (Bit-15 / Bit-7) of Addr as R/W' Bit\n"
#endif
;

cmd_result_t
dpll_cmd(int unit, args_t *a)
{
    volatile cmd_result_t rv = CMD_OK;
    char *c;
    uint32 addr, data, temp;
    int rd_op, i;
    int cfg=0, mux_sel = MSPI_DPLL;
    int cpol=-1, cpha=-1, addr_bit_order=0, data_bit_order=0;
    int addr_width=2, use_brst_bit=0, use_rw_bit=0;
    volatile int wbytes = 2, rbytes=1;
    uint8 wdata[16];
    uint8 rdata[16];
    int params[8];
    parse_table_t pt;

    int preset_configs[3][8] = {
    /* mux_sel,cpol,cpha,addr_bit_order,data_bit_order,addr_width,use_brst_bit,use_rw_bit */
        {2,0,1,0,0,2,1,1}, /* Maxim DPLL */
        {3,1,1,1,1,1,0,1}, /* IDT DPLL */
        {0,0,0,0,0,2,1,1}
    };

    if (!sh_check_attached("dpll", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_cmicm)) {
        return(CMD_FAIL);
    }

    if (ARG_CNT(a) == 0) {
        return CMD_USAGE;
    }

    if (soc_property_get_csv(unit, spn_DPLL_PARAMS,8,params) == 8) {
        mux_sel = params[0];
        cpol = params[1];
        cpha = params[2];
        addr_bit_order = params[3];
        data_bit_order = params[4];
        addr_width = params[5];
        use_brst_bit = params[6];
        use_rw_bit = params[7];
    }

    sal_memset(rdata, 0, sizeof (rdata));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Config",  PQ_DFL|PQ_INT,
                                INT_TO_PTR( 0), &cfg, NULL);
    parse_table_add(&pt, "DevSel",  PQ_DFL|PQ_INT,
                                INT_TO_PTR(mux_sel), &mux_sel, NULL);
    parse_table_add(&pt, "CPOL",  PQ_DFL|PQ_BOOL,
                                INT_TO_PTR(cpol), &cpol, NULL);
    parse_table_add(&pt, "CPHA",  PQ_DFL|PQ_BOOL,
                                INT_TO_PTR(cpha), &cpha, NULL);
    parse_table_add(&pt, "AddrBitOrder",  PQ_DFL|PQ_BOOL,
                                INT_TO_PTR(addr_bit_order), &addr_bit_order, NULL);
    parse_table_add(&pt, "DataBitOrder",  PQ_DFL|PQ_BOOL,
                                INT_TO_PTR(data_bit_order), &data_bit_order, NULL);
    parse_table_add(&pt, "AddrWidth",  PQ_DFL|PQ_INT,
                                INT_TO_PTR(addr_width), &addr_width, NULL);
    parse_table_add(&pt, "UseBrstBit",  PQ_DFL|PQ_BOOL,
                                INT_TO_PTR(use_brst_bit), &use_brst_bit, NULL);
    parse_table_add(&pt, "UseRwBit",  PQ_DFL|PQ_BOOL,
                                INT_TO_PTR(use_rw_bit), &use_rw_bit, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    if (cfg > 0) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Using Preset %d\n"), cfg));
        mux_sel = preset_configs[cfg-1][0];
        cpol = preset_configs[cfg-1][1];
        cpha = preset_configs[cfg-1][2];
        addr_bit_order = preset_configs[cfg-1][3];
        data_bit_order = preset_configs[cfg-1][4];
        addr_width = preset_configs[cfg-1][5];
        use_brst_bit = preset_configs[cfg-1][6];
        use_rw_bit = preset_configs[cfg-1][7];
    }

    if (soc_mspi_config(unit, mux_sel, cpol, cpha) != SOC_E_NONE){
        cli_out("MSPI: Failure Configuring\n");
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        return CMD_OK;
    }

     c = ARG_GET(a);

    if (!sal_strcasecmp(c, "write")) {
        rd_op = 0;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Write DPLL\n")));
    } else if (!sal_strcasecmp(c, "read")) {
        rd_op = 1;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Read DPLL\n")));
    } else {
        return CMD_USAGE;
    }

    if (ARG_CNT(a) == 0) {
        return CMD_USAGE;
    }
    c = ARG_GET(a);
    if (!isint(c)) {
        return CMD_USAGE;
    }
    addr = parse_integer(c);

    if(addr_width == 1) {
        if (addr_bit_order) {
            temp = 0;
            for(i=0; i<8; i++) {
                if (addr & (1<<i)) {
                    temp |= (1<<(7-i));
                }
            } 
             temp  = (temp >> use_rw_bit);
             temp &= ((use_brst_bit) ? 0xfe : 0xff);
        } else {
            temp = addr;
            temp = (temp << use_brst_bit);
            temp &= ((use_rw_bit) ? 0x7f : 0xff);
        }
        if (use_rw_bit && rd_op) {
            temp |= 0x80;
        }
        wdata[0] = temp & 0xff;
        wbytes = 1;
    } else if (addr_width == 2) {
        if (addr_bit_order) {
            temp = 0;
            for(i=0; i<16; i++) {
                if (addr & (1<<i)) {
                    temp |= (1<<(15-i));
                }
            }
            temp  = (temp >> use_rw_bit);
            temp &= ((use_brst_bit) ? 0xfffe : 0xffff);
        } else {
            temp = addr;
            temp = (temp << use_brst_bit);
            temp &= ((use_rw_bit) ? 0x7fff : 0xffff);
        }
        if (use_rw_bit && rd_op) {
            temp |= 0x8000;
        }
        wdata[0] = (temp >> 8) & 0xff;
        wdata[1] = temp & 0xff;
        wbytes = 2;
    } else {
        cli_out("Only 1 or 2 byte addresses supported\n");
        return CMD_FAIL;
    }

    if(rd_op == 0) { /* Write */
        if (ARG_CNT(a) == 0) {
            return CMD_USAGE;
        }
        c = ARG_GET(a);
        if (!isint(c)) {
            return CMD_USAGE;
        }
        data = parse_integer(c);
        if (data_bit_order) {
            temp = 0;
            for(i=0; i<8; i++) {
                if (data & (1<<i)) {
                    temp |= (1<<(7-i));
                }
            }
            data = temp;
        }
        wdata[wbytes] = (data & 0xff);
        wbytes += 1;
        rbytes = 0;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MSPI: Writing %d bytes\n"), wbytes));
        if (soc_mspi_write8(unit, (uint8 *) &wdata[0], wbytes) != SOC_E_NONE) {
            cli_out("MSPI: Write Fail\n");
            return CMD_FAIL;
        }
        /* Dump It */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%02x\n"), data));
    } else { /* Read */
        rbytes = 1;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MSPI: Writing %d bytes and Reading %d bytes\n"),
                     wbytes, rbytes));
        if (soc_mspi_writeread8(unit, (uint8 *) &wdata[0], wbytes,(uint8 *) &rdata[0], rbytes) != SOC_E_NONE) {
            cli_out("MSPI: Write-Read Fail\n");
            return CMD_FAIL;
        }
        data = rdata[0];
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Read %02x\n"), data));

        if (data_bit_order) {
            temp = 0;
            for(i=0; i<8; i++) {
                if (data & (1<<i)) {
                    temp |= (1<<(7-i));
                }
            }
            data = temp;
        }
        /* Dump It */
        cli_out("%02x\n", data);
    }
    return rv;
}
#endif
