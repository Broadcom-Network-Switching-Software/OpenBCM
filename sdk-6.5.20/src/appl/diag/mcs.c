/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	mcs.c
 * Purpose: 	Micro Controller Subsystem Support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <soc/types.h>
#include <soc/dma.h>
#if defined(BCM_JERICHO_SUPPORT)
#include <soc/dpp/JER/jer_drv.h>
#endif
#include <appl/diag/shell.h>

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/diag_mcs.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#include <soc/uc.h>
#include <soc/iproc.h>
#include <soc/shared/mos_coredump.h>
#include <soc/uc_msg.h>
#include <soc/uc_dbg.h>

extern void format_uint64_decimal(char *buf, uint64 n, int comma);
uKernel_info_t fwInfo[MAX_UCS];

void dumpbuf(uint8 *buf, int count) {
    int i;
    uint8 *bp = buf;
    for(i = 0; i < count; i++) {
        cli_out("%02X", *bp++);
        if ((i % 16) == 15) {
            cli_out("\n");
        }
    }
}

#ifndef NO_FILEIO

#define DUMP_BUFFER_SIZE 1024

STATIC cmd_result_t
_mcs_dump_region(int unit, FILE *fp, uint32 *buffer, mos_coredump_region_t *reg) {

    uint32 addr = reg->baseaddr;
    uint32 len = reg->end - reg->start;
    int buf_idx = 0;
    while (len > 0) {
        buffer[buf_idx] = soc_uc_mem_read(unit, addr);

        len -= sizeof(uint32);
        addr += sizeof(uint32);

        /* Flush buffer to disk if it is full, or we are done with the region */
        if (++buf_idx >= (DUMP_BUFFER_SIZE / sizeof(uint32)) || (len == 0)) {
            if (sal_fwrite(buffer, 4, buf_idx, fp) != buf_idx) {
                return(CMD_FAIL);
            }
            buf_idx = 0;
        }
    } 

    return (CMD_OK);
}

STATIC cmd_result_t
mcs_dump(int unit, FILE *fp, int addr, int mem_size)
{
    /* Set up the region */
    mos_coredump_region_t reg;
    uint32 *buffer;

    /* Init the regions descriptors */
        reg.cores = 1;
        reg.baseaddr = addr;
        reg.start = addr;
        reg.end = reg.start + mem_size;

    /* allocate 1k for data */
    buffer = (uint32 *) soc_cm_salloc(unit, DUMP_BUFFER_SIZE, "MCS Dump Buffer");
    if(buffer == NULL) {
        cli_out("Unable to allocate buffer\n");
        return(CMD_FAIL);
    }

    /* Write the desriptors */
    sal_memcpy(buffer, &reg, sizeof(mos_coredump_region_t));
    /* Ensure that descriptors are in network byte order */
    buffer[0] = soc_htonl(buffer[0]);

    if (sal_fwrite(buffer, sizeof(mos_coredump_region_t), 1, fp) != 1) {
        cli_out("Error writing header\n");
        return(CMD_FAIL);
    }

    if (_mcs_dump_region(unit, fp, buffer, &reg) != CMD_OK) {
        cli_out("Error writing dump\n");
        return(CMD_FAIL);
    }

    soc_cm_sfree(unit, buffer);
    
    return(CMD_OK);
}

uint32 ihex_ext_addr[SOC_MAX_NUM_DEVICES]; /* Extended Address */


/*
 * returs -1 if error
 * returns number of bytes. (0 if this record doesn't contain any data).
 */
STATIC int
mcs_parse_ihex_record(int unit, char *line, uint32 *off)
{
    int count;
    uint32 address;

    switch (line[8]) {      /* Record Type */
        case '0' :      /* Data Record */
            count = (xdigit2i(line[1]) << 4) |
                    (xdigit2i(line[2]));

            address = (xdigit2i(line[3]) << 12) |
                      (xdigit2i(line[4]) << 8) |
                      (xdigit2i(line[5]) << 4) |
                      (xdigit2i(line[6]));

            *off = ihex_ext_addr[unit] + address;
            return count;
            break; /* Not Reachable */
        case '4' :      /* Extended Linear Address Record */
            address = (xdigit2i(line[9]) << 12) |
                      (xdigit2i(line[10]) << 8) |
                      (xdigit2i(line[11]) << 4) |
                      (xdigit2i(line[12]));

            ihex_ext_addr[unit] = (address << 16);
            cli_out("Exteded Linear Address 0x%x\n", ihex_ext_addr[unit]);
            return 0;
            break; /* Not Reachable */
        default :       /* We don't parse all other records */
            cli_out("Unsupported Record\n");
            return 0;
    }
    return -1; /* Why are we here? */ /* Not Reachable */
}

STATIC int
mcs_parse_srec_record (int unit, char *line, uint32 *off)
{
    int count;
    uint32 address;

    switch (line[1]) {      /* Record Type */
        case '0':           /* Header record - ignore */
            return 0;
            break; /* Not Reachable */
            
        case '1' :          /* Data Record with 2 byte address */
            count = (xdigit2i(line[2]) << 4) |
                    (xdigit2i(line[3]));
            count -= 3; /* 2 address + 1 checksum */

            address = (xdigit2i(line[4]) << 12) |
                      (xdigit2i(line[5]) << 8) |
                      (xdigit2i(line[6]) << 4) |
                      (xdigit2i(line[7]));

            *off = address;
            return count;
            break; /* Not Reachable */

        case '2' :          /* Data Record with 3 byte address */
            count = (xdigit2i(line[2]) << 4) |
                    (xdigit2i(line[3]));
            count -= 4; /* 3 address + 1 checksum */

            address = (xdigit2i(line[4]) << 20) |
                      (xdigit2i(line[5]) << 16) |
                      (xdigit2i(line[6]) << 12) |
                      (xdigit2i(line[7]) << 8) |
                      (xdigit2i(line[8]) << 4) |
                      (xdigit2i(line[9]));

            *off = address;
            return count;
            break; /* Not Reachable */

        case '3' :          /* Data Record with 4 byte address */
            count = (xdigit2i(line[2]) << 4) |
                    (xdigit2i(line[3]));
            count -= 5; /* 4 address + 1 checksum */

            address = (xdigit2i(line[4]) << 28) |
                      (xdigit2i(line[5]) << 24) |
                      (xdigit2i(line[6]) << 20) |
                      (xdigit2i(line[7]) << 16) |
                      (xdigit2i(line[8]) << 12) |
                      (xdigit2i(line[9]) << 8) |
                      (xdigit2i(line[10]) << 4) |
                      (xdigit2i(line[11]));

            *off = address;
            return count;
            break; /* Not Reachable */
            
        case '5':         /* Record count - ignore */
        case '6':         /* End of block - ignore */
        case '7':         /* End of block - ignore */
        case '8':         /* End of block - ignore */
        case '9':         /* End of block - ignore */
            return 0;
            break; /* Not Reachable */
            
        default :       /* We don't parse all other records */
            cli_out("Unsupported Record S%c\n", line[1]);
            return 0;
    }
    return -1; /* Why are we here? */ /* Not Reachable */
}


/*
 * It is assumed that by the time this routine is called
 * the record is already validated. So, blindly get data
 */
STATIC void
mcs_get_rec_data(char *line, int count, uint8 *dat)
{
    int i, datpos = 9;  /* 9 is valid for ihex */

    if (!count) {
        return;
    }

    if (line[0] != ':') {
        /* not ihex */
        switch (line[1]) {
        case '1':  /* S1 record */
            datpos = 8;
            break;
        case '2':  /* S2 record */
            datpos = 10;
            break;
        case '3':  /* S3 record */
            datpos = 12;
            break;
        default:
            cli_out("Unexpected record type: '%c'\n", line[1]);
            SOC_CMIC_UCDBG_LOG_ADD((0, _bcmCmicUcDbgLogHostMcsLoad,
                "%s: Unexpected record type\n", FUNCTION_NAME()));
            break;
        }
    }

    /*1 count = 2 hex chars */
    for(i = 0; i < count; i++) {
        *(dat + i) = ((xdigit2i(line[datpos + (i * 2)])) << 4) |
                    (xdigit2i(line[datpos + (i * 2) + 1]));
    }
}

STATIC cmd_result_t
mcs_file_load(int unit, FILE *fp, int ucnum, uint32 *lowaddr, int stage, uint32 twostageaddr)
{
    uint32 addr=0;
    int rv = 0;
    char input[256], *cp = NULL;
    unsigned char data[256];

    if (lowaddr != NULL) {
        *lowaddr = 0xffff0000;
    }
    ihex_ext_addr[unit] = 0; /* Until an extended addr record is met.. */

    while (NULL != (cp = sal_fgets(input, sizeof(input) - 1, fp))) {
        if (input[0] == 'S') {
            rv = mcs_parse_srec_record(unit, cp, &addr);
        } else if (input[0] == ':') {
            rv = mcs_parse_ihex_record(unit, cp, &addr);
        } else {
            cli_out("unknown Record Type\n");
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                "%s: unknown Record Type\n", FUNCTION_NAME()));
            rv = -1;
        }

        if (-1 == rv) {
            return (CMD_FAIL);
        }

        if (rv % 4) {
            cli_out("record Not Multiple of 4\n");
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                "%s: record Not Multiple of 4\n", FUNCTION_NAME()));
            return (CMD_FAIL);
        }

        /* Do this only if first stage */        
        if (stage == 0) {
            /* track lowest seen address with data */
            if ((rv > 0) && (lowaddr != NULL) && (addr < *lowaddr)) {
                *lowaddr = addr;
            }
        }

        if ((stage == 0) && (addr >= twostageaddr)) {
		  /* No Need to parse teh remaining now... */
            return CMD_OK;
        }

        if ( ((stage == 0) && (addr < twostageaddr)) ||
             ((stage == 1) && (addr >= twostageaddr))) {
                mcs_get_rec_data(cp, rv, data);
                soc_uc_load(unit, ucnum, addr, rv, data);
        }
    }
    return(CMD_OK);
}

#endif


char mcsload_usage[] =
    "Parameters: <uCnum> <file.hex>\n\t\t"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "[InitMCS|ResetUC|StartUC|StartMSG]=true|false]\n\t\t"
    "Load the MCS memory area from <file.srec>.\n\t"
    "uCnum = uC number to be loaded.\n\t"
    "InitMCS = Init the MCS (not just one UC) (false)\n\t"
    "ResetUC = Reset the uC (true)\n\t"
    "StartUC = uC out of halt after load (true)\n\t"
    "StartMSG = Start messaging (true)\n"
#endif    
    ;

cmd_result_t
mcsload_cmd(int unit, args_t *a)
/*
 * Function: 	mcsload_cmd
 * Purpose:	Load a file into MCS.
 * Parameters:	unit - unit
 *		a - args, each of the files to be displayed.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    cmd_result_t rv = CMD_OK;
    parse_table_t  pt;
    char *c , *filename;
    int ucnum, resetuc = 1, startuc=1, startmsg=1, initmcs = 0, twostage = 0;
    uint32 twostageaddr = 0xffffffff;

#ifndef NO_FILEIO
#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif    
    FILE * volatile fp = NULL;
    uint32 lowaddr;
#endif
#ifdef BCM_SABER2_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
#endif

    if (!sh_check_attached("mcsload", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return (CMD_FAIL);
    } 

    if (ARG_CNT(a) < 2) {
        return(CMD_USAGE);
    }

    c = ARG_GET(a);
    if (!isint(c)) {
        cli_out("%s: Error: uC Num not specified\n", ARG_CMD(a));
        return(CMD_USAGE);
    }

    ucnum = parse_integer(c);
    if (ucnum < 0 || ucnum >= SOC_INFO(unit).num_ucs) {
        cli_out("Invalid uProcessor number: %d\n", ucnum); 
        return(CMD_FAIL);
    }

    c = ARG_GET(a);
    filename = c;
    if (filename == NULL) {
        cli_out("%s: Error: No file specified\n", ARG_CMD(a));
        return(CMD_USAGE);
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "InitMCS", PQ_DFL|PQ_BOOL,
                        0, &initmcs,  NULL);
        parse_table_add(&pt, "ResetUC", PQ_DFL|PQ_BOOL,
                        0, &resetuc,  NULL);
        parse_table_add(&pt, "StartUC", PQ_DFL|PQ_BOOL,
                        0, &startuc,  NULL);
        parse_table_add(&pt, "StartMSG", PQ_DFL|PQ_BOOL,
                        0, &startmsg,  NULL);
        parse_table_add(&pt, "TwoStage", PQ_DFL|PQ_BOOL,
                        0, &twostage,  NULL);
        parse_table_add(&pt, "TwoStageAddress", PQ_DFL|PQ_INT,
                        0, &twostageaddr, NULL);
        if (!parseEndOk(a, &pt, &rv)) {
            if (CMD_OK != rv) {
                return rv;
            }
        }
    }

#ifdef BCM_SABER2_SUPPORT
    /* mHost1 is disabled in dagger2 (BCM56233) */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (SOC_IS_SABER2(unit) && (dev_id == BCM56233_DEVICE_ID) && (ucnum == 1)) {
        return(CMD_FAIL);
    }
#endif

    if (twostage) {
        startuc = 1; /* Force Start */
    } else {
        twostageaddr = 0xffffffff;
    }
    
    /* check for simulation*/
    if (SAL_BOOT_BCMSIM) {
        return(rv);
    }

    /* initialize ukernel debug module */
    soc_cmic_ucdebug_init(unit, ucnum);


#ifdef NO_FILEIO
    cli_out("no filesystem\n");
#else
#ifndef NO_CTRL_C    
    if (!setjmp(ctrl_c)) {
        sh_push_ctrl_c(&ctrl_c);
#endif
	/* See if we can open the file before doing anything else */
        fp = sal_fopen(filename, "r");
        if (!fp) {
            cli_out("%s: Error: Unable to open file: %s\n",
                    ARG_CMD(a), filename);
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                "Error: Unable to open file: %s\n", filename));
            rv = CMD_FAIL;
#ifndef NO_CTRL_C    
            sh_pop_ctrl_c();
#endif
            return rv;
        }

        /* Reset the UC to stop messages before stopping the msg system */
        if (resetuc || initmcs) {
            soc_uc_reset(unit, ucnum);
        }

        if (initmcs) {
            soc_uc_init(unit);
        }
            
        soc_uc_preload(unit, ucnum);

        rv = mcs_file_load(unit, fp, ucnum, &lowaddr, 0, twostageaddr);

        if (startuc) {
            soc_uc_start(unit, ucnum, lowaddr);
            sal_usleep(100 * 1000);
        }

#if defined(BCM_IPROC_SUPPORT)
        if (twostage) {
            int retries;
            uint32 rval;
            rv = CMD_OK;
            if (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_ALT) {
                WRITE_PAXB_1_PCIE_EP_AXI_CONFIGr(unit, 0xfffdff7f);
            } else {
                WRITE_PAXB_0_PCIE_EP_AXI_CONFIGr(unit, 0xfffdff7f);
            }

            cli_out("Waiting for Core-%d to start", ucnum);
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                "Waiting for Core-%d to start\n", ucnum));
            rval = 0;
            retries = 0;
            while (rval != 0x10c8ed) { /* Locked?? */
                sal_usleep(10 * 1000);
                rval = soc_cm_iproc_read(unit, 0x1b07f000);
                cli_out(".");
                if (++retries > 100) {
                    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                            "Error: Unable to start Core-%d rval:0x%x\n", 
                            ucnum, rval));
                    rv = CMD_FAIL;
                    break;
                }
            }
            /* L2$ Cache has been Initialized */

            if (rv == CMD_OK) {
                cli_out("\nWaiting for L2$ to be configured");
                rval = 0;
                retries = 0;
                while (rval != 0xdeadc0de) {
                    sal_usleep(10 * 1000);
                    rval = soc_cm_iproc_read(unit, twostageaddr);
                    cli_out(".");
                    if (++retries > 100) {
                        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                         "Error: Waiting for L2$ to be configured rval: 0x%x\n",
                         rval));
                        rv = CMD_FAIL;
                        break;
                    }
                }
                cli_out("\n");
            }
            /* L2$ Cache has been configured as RAM */

            /* Last read was for first line of 2nd stage.  So close/reopen
               so that next load doesn't miss that line
            */

            sal_fclose((FILE *)fp);
            fp = NULL;

            if (rv != CMD_OK) {
                /* Last Load failed: Timed Out */
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return rv;
            }

            fp = sal_fopen(filename, "r");
            if (!fp) {
                cli_out("%s: Error: Unable to open file: %s\n",
                        ARG_CMD(a), filename);
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                    "%s: Error: Unable to open file: %s\n",
                    ARG_CMD(a), filename));
                rv = CMD_FAIL;
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return rv;
            }

            rv = mcs_file_load(unit, fp, ucnum, &lowaddr, 1, twostageaddr);
            soc_cm_iproc_write(unit, twostageaddr, 0x10aded); /*Loaded */
        }
#endif /* IPROC_SUPPORT */

        sal_fclose((FILE *)fp);
        fp = NULL;

#ifdef BCM_MONTEREY_SUPPORT
        /* In monterey ucnum:2 is M7 and no messaging required */
        if (!(SOC_IS_MONTEREY(unit) && (ucnum == 2))) {
#endif
            if (startmsg) {
                soc_cmic_uc_msg_start(unit);
                soc_cmic_uc_msg_uc_start(unit, ucnum);
            }
#ifdef BCM_MONTEREY_SUPPORT
        }
#endif

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

    /* WA for "bus error" issue that SDK configure the RSVD field of 
     * MHOST_0_CR5_CFG_CTRL, then uKernel polling this register field to kill 
     * or stop the thread. 
     */
#if defined(BCM_JERICHO_SUPPORT)
    if (SOC_IS_JERICHO(unit)) {
        soc_restore_bcm88x7x(unit, 0);
    }
#endif

    return(rv);
}

char mcsdump_usage[] =
    "Parameters: [uCnum|<dumpfile>] <ADDR> <MEM_SIZE> [Halt=true|false]\n\t"
    "Dump both uCs memory in BRCM dumpfile format.\n\t"
    "<file.hex> = dumpfile to write.\n\t"
    "Halt = Halt the UCs prior to the dump.\n\t"
    "Address = Base address + Offset.\n\t"
    "Size of memory to be read.\n\t"
    "uCnum = uC core Cpu and Memory usage report \n";

cmd_result_t
mcsdump_cmd(int unit, args_t *a)
/*
 * Function: 	mcsdump_cmd
 * Purpose:	Dump the MCS.
 * Parameters:	unit - unit
 *		a - args, each of the files to be displayed.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    cmd_result_t rv = CMD_OK;
    parse_table_t  pt;
    char *c , *filename;
    int halt = 0, ucnum = 0;
    uint32 reg0, reg1, regs;
    char *thread_info;
#ifndef NO_FILEIO
    int addr, mem_size;
#endif

#ifndef NO_FILEIO
#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif
    FILE * volatile fp = NULL;
#endif

    if (!sh_check_attached("mcsload", unit)) {
        return(CMD_FAIL);
    }

    /* CMICm only at the moment */
    if (!soc_feature(unit, soc_feature_uc)) {
        return (CMD_FAIL);
    } 

    if (ARG_CNT(a) < 1) {
        return(CMD_USAGE);
    }

    c = ARG_GET(a);
    if (isint(c)) {
        ucnum = parse_integer(c);
        if (ucnum < SOC_INFO(unit).num_ucs) {
            cli_out("ID         Name               %%       Base        Size        sp        free  Priority   run_time\n");
            thread_info = soc_uc_firmware_thread_info(unit, ucnum);
            if (thread_info) {
                cli_out("%s\n", thread_info);
                soc_cm_sfree(unit, thread_info);
            }
            return (CMD_OK);
        } else {
            cli_out("%s: Error: uC Num not legal\n", ARG_CMD(a));
            return(CMD_USAGE);
        }
    }
    filename = c;
    if (filename == NULL) {
        cli_out("%s: Error: No file specified\n", ARG_CMD(a));
        return(CMD_USAGE);
    }

#ifndef NO_FILEIO
    if (ARG_CNT(a) < 2) {
        cli_out("%s: Error: provide valid address and size\n", ARG_CMD(a));
        return(CMD_USAGE);
    }
    c = ARG_GET(a);
    if (isint(c)) {
        addr = parse_integer(c);
        c = ARG_GET(a);
        if (isint(c)) {
            mem_size = parse_integer(c);
        } else {
            cli_out("%s: Error: provide memory size\n", ARG_CMD(a));
            return(CMD_USAGE);
        }
    } else {
        cli_out("%s: Error: provide base address\n", ARG_CMD(a));
        return(CMD_USAGE);
    }
#endif

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Halt", PQ_DFL|PQ_BOOL,
                    0, &halt,  NULL);
    if (!parseEndOk(a, &pt, &rv)) {
        if (CMD_OK != rv) {
            return rv;
        }
    }


    if (halt) {
        /* Halt both uCs so that the dump is clean */
        READ_UC_0_RST_CONTROLr(unit, &reg0);
        regs = reg0;
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &regs, CPUHALT_Nf, 0);
        WRITE_UC_0_RST_CONTROLr(unit, regs);
            
        READ_UC_0_RST_CONTROLr(unit, &reg1);
        regs = reg0;
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &regs, CPUHALT_Nf, 0);
        WRITE_UC_1_RST_CONTROLr(unit, regs);
    }
        
#ifdef NO_FILEIO
    cli_out("no filesystem\n");
#else
#ifndef NO_CTRL_C 
    if (!setjmp(ctrl_c)) {
        sh_push_ctrl_c(&ctrl_c);
#endif

        fp = sal_fopen(filename, "w");
        if (!fp) {
            cli_out("%s: Error: Unable to open file: %s\n",
                    ARG_CMD(a), filename);
            rv = CMD_FAIL;
        } else {
            rv = mcs_dump(unit, fp, addr, mem_size);
            sal_fclose((FILE *)fp);
            fp = NULL;
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

    if (halt) {
        /* Put both uCs back in previous state */
        WRITE_UC_0_RST_CONTROLr(unit, reg0);
        WRITE_UC_1_RST_CONTROLr(unit, reg1);
    }

        
    return(rv);
}

char mcsmsg_usage[] =
    "Parameters: [ucnum|Start|Stop]\n\t"
    "Control messaging with the UCs.\n\t"
    "ucnum = ucnum to start comminicating with (must start first).\n\t"
    "Init = Init messaging with all uCs.\n\t"
    "Halt = Halt messaging with all uCs.\n\t"
    "Stop = Stop messaging with uC.\n";

cmd_result_t
mcsmsg_cmd(int unit, args_t *a)
/*
 * Function: 	mcsmsg_cmd
 * Purpose:	Start messading with the MCS.
 * Parameters:	unit - unit
 *		a - args, 0, 1, stop.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    char *c;
    int ucnum;

    if (!sh_check_attached("mcsmsg", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return (CMD_FAIL);
    } 

    if (ARG_CNT(a) < 1) {
        return(CMD_USAGE);
    }

    c = ARG_GET(a);
    if (isint(c)) {
        ucnum = parse_integer(c);
        if (ucnum < SOC_INFO(unit).num_ucs) {
            soc_cmic_uc_msg_uc_start(unit, ucnum);
        } else {
            cli_out("%s: Error: uC Num not legal\n", ARG_CMD(a));
            return(CMD_USAGE);
        }
        
    } else if (sal_strcasecmp(c, "INIT") == 0) {
        soc_cmic_uc_msg_start(unit);
        
    } else if (sal_strcasecmp(c, "HALT") == 0) {
        soc_cmic_uc_msg_shutdown_halt(unit);
        
    } else if (sal_strcasecmp(c, "STOP") == 0) {
        if (ARG_CNT(a) < 1) {
            return(CMD_USAGE);
        }
        c = ARG_GET(a);
        ucnum = parse_integer(c);
        if (ucnum < SOC_INFO(unit).num_ucs) {
            soc_cmic_uc_msg_uc_shutdown_halt(unit, ucnum);
        } else {
            cli_out("%s: Error: uC Num not legal\n", ARG_CMD(a));
            return(CMD_USAGE);
        }
    } else {
        cli_out("%s: Error: Invalid parameter\n", ARG_CMD(a));
        return(CMD_USAGE);
    } 

    return (CMD_OK);

}

char mcscmd_usage[] =
    "Parameters: <ucnum> <cmd>\n\t"
    "Execute cmd on uC\n\t"
    "ucnum = ucnum to start comminicating with (must start first).\n\t"
    "cmd = <stats reset>.\n";

/*
 * Function: 	mcsmsg_cmd
 * Purpose:	Start messading with the MCS.
 * Parameters:	unit - unit
 *		a - args, 0, 1, stop.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t
mcscmd_cmd(int unit, args_t *a)
{
    char         *c;
    int           uC;

    if (!sh_check_attached("mcsmsg", unit)) {
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return CMD_FAIL;
    }


    if (ARG_CNT(a) < 1) {
        return CMD_USAGE;
    }

    c = ARG_GET(a);
    if (isint(c)) {
        uC = parse_integer(c);
        if (uC >= SOC_INFO(unit).num_ucs) {
            cli_out("%s: Error: uC Num not legal\n", ARG_CMD(a));
            return CMD_USAGE;
        }
    } else {
        cli_out("Error: uC Num not specified\n");
        return CMD_USAGE;
    }

    if (ARG_CNT(a) == 2) {
        c = ARG_GET(a);
        if (sal_strcasecmp(c, "STATS") == 0) {
            c = ARG_GET(a);
            if (sal_strcasecmp(c, "RESET") == 0) {
                if (soc_uc_stats_reset(unit, uC) != SOC_E_NONE) {
                    cli_out("Error: stats reset cmd fail\n");
                }
                return CMD_OK;
            }
        } else if (sal_strcasecmp(c, "CONSOLE") == 0) {
            c = ARG_GET(a);
            if (sal_strcasecmp(c, "ON") == 0) {
                if (soc_uc_console_log(unit, uC, 1) != SOC_E_NONE) {
                    cli_out("Error: console log ON cmd fail\n");
                }
                return CMD_OK;
            } else if(sal_strcasecmp(c, "OFF") == 0) {
                if (soc_uc_console_log(unit, uC, 0) != SOC_E_NONE) {
                    cli_out("Error: console log OFF cmd fail\n");
                }
                return CMD_OK;
            }
        }
    }

    /* Only stats reset supported for now */
    return CMD_USAGE;
}

void mcs_core_status(int unit, int uC, uint32 base)
{
    int i;
    uint32 cpsr;
    char *version;
    uint16 dev_id;
    uint8 rev_id;
    uint32  regVal;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Skip getting status of Firmware for Core-1 in Dagger2 */
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return;
    }

    /* CPSR is at offset 0x60 from start of TCM */
    /* Core 0's TCM is at a PCI address of 0x100000, Core 1's is at 0x200000 */
    cpsr = soc_uc_mem_read(unit, base + 0x60);
    cli_out("uC %d status: %s\n", uC, (cpsr) ? "Fault" : "Ok");
    if (uC < MAX_UCS)
        sal_memcpy(fwInfo[uC].status,
                   (cpsr) ? "Fault" : "Ok",
                   (cpsr) ? sizeof("Fault") : sizeof("Ok"));
    if (cpsr) {
        regVal = soc_uc_mem_read(unit, base + 0x64);
        cli_out("\tcpsr\t0x%08x\n", cpsr);
        cli_out("\ttype\t0x%08x\n", regVal);
        if (uC < MAX_UCS) {
            fwInfo[uC].cpsr = cpsr;
            fwInfo[uC].type = regVal;
        }

        for (i = 0; i < 16; ++i) {
            regVal = soc_uc_mem_read(unit, base + 0x20 + i*4);
            cli_out("\tr%d\t0x%08x\n",
                    i, regVal);
            if (uC < MAX_UCS)
                fwInfo[uC].armReg[i] = regVal;
        }
    }
    else {
        version = soc_uc_firmware_version(unit, uC);
        if (version) {
            cli_out("version: %s\n", version);
            if (uC < MAX_UCS)
                sal_memcpy(fwInfo[uC].fw_version, version, 100);
            soc_cm_sfree(unit, version);
        }
    }
}

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
extern int _bcm_esw_gdpll_m7_status (int unit, uint8 *pBuff);
#endif

char mcsstatus_usage[] =
    "Prints fault status of microcontrollers\n\t\t"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "Parameters: [Quick=true|false] [TimeOutUs=<val>]\n\t\t"
    "Quick = Call Ping function instead of FW Ver.(false)\n\t\t"
    "TimeOutUs = Ping Timeout in uSecs. Used only in Quick mode. (1Sec)\n"
#endif
    ;

cmd_result_t
mcsstatus_cmd(int unit, args_t *a)
/*
 * Function: mcsstatus_cmd
 * Purpose: Show fault status of MCS
 * Parameters: unit - unit
 *      a - args (none)
 * Returns: CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    int quick = 0, uc_num = 0, rv = 0, to = 0;
    parse_table_t  pt;
    char         *c;
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    uint8 *version = NULL;
    uint8 buffer[20/*DPLL_FW_VERSION_SIZE*/];
#endif

    if (!sh_check_attached("mcsmsg", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return (CMD_FAIL);
    }

    if (ARG_CNT(a) > 0) {
        /* Just peek into the argument without consuming it
         * In case it is not an integer, it is needed for the parse table.
         */
        c = ARG_CUR(a);
        if (isint(c)) {
            uint32 addr = parse_integer(c);
            cli_out("%08x: %08x\n", addr, soc_uc_mem_read(unit, addr));
            ARG_DISCARD(a);
            return(CMD_OK);
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Quick", PQ_DFL|PQ_BOOL,
                        0, &quick,  NULL);
        parse_table_add(&pt, "TimeOutUs", PQ_DFL|PQ_INT,
                        (void *)(1*1000*1000), &to, NULL);
        if (!parseEndOk(a, &pt, &rv)) {
            if (CMD_OK != rv) {
                return rv;
            }
        }
    }

    if (quick) {
        /* Just Ping */
        for(uc_num = 0; uc_num < SOC_INFO(unit).num_ucs; uc_num++) {
            rv = soc_uc_ping(unit, uc_num, (sal_usecs_t)to);
            cli_out("uC %d status: ", uc_num);
            switch(rv) {
            case SOC_E_UNAVAIL:
                cli_out("No uC / MCS support\n");
                break;
            case SOC_E_PARAM:
                cli_out("uC Invalid for this chip\n");
                break;
            case SOC_E_DISABLED:
                cli_out("ARM core is in reset\n");
                break;
            case SOC_E_INIT:
                cli_out("uKernel messaging is not initialized\n");
                break;
            case SOC_E_TIMEOUT:
                cli_out("uKernel not responding\n");
                break;
            case SOC_E_NONE:
                cli_out("OK\n");
                break;
            default:
                cli_out("Unknown Error\n");
                break;
            }
        }
        return CMD_OK;
    }

    for(uc_num=0; uc_num < MAX_UCS; uc_num++) {
        if (uc_num < SOC_INFO(unit).num_ucs) {
            fwInfo[uc_num].status[0] = '\0';
            fwInfo[uc_num].fw_version[0] = '\0';
        }
    }

    /* If a fault happens, the register dump will include a non-zero CPSR */

    if (soc_feature(unit, soc_feature_iproc)) {
    
#if defined(BCM_UC_MHOST_SUPPORT)
        if (soc_feature(unit, soc_feature_uc_mhost)) {
        
            int uc, addr;
            
            /* iProc mHost: vectors and exception registers are in ATCM */
            for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
                version = &buffer[0];
                if ((SOC_IS_MONTEREY(unit) && (uc == 2))) {
                    rv = _bcm_esw_gdpll_m7_status(unit, buffer);
                    if (rv == BCM_E_NONE) {
                        version = &buffer[0];
                        cli_out("uC 2 status: Ok\n");
                        cli_out("version: %.6s %.5s %s\n", version, version+6, version+11);
                    } else {
                        cli_out("uC 2 status: RESET\n");
                    }
                    continue;
                }
#endif
                rv = soc_uc_in_reset(unit, uc);
                if (rv == 1) {
                    cli_out("uC %d status: RESET\n", uc);
                    if (uc < MAX_UCS)
                        sal_memcpy(fwInfo[uc].status, "RESET", sizeof("RESET"));
                } else if (rv == 2) {
                    cli_out("uC %d status: No Valid FW\n", uc);
                    if (uc < MAX_UCS)
                        sal_memcpy(fwInfo[uc].status, "No Valid FW", sizeof("No Valid FW"));
                } else {
                    addr = 0x01000000 + 0x00100000 * uc;
#ifdef BCM_SABER2_SUPPORT
                    if (SOC_IS_SABER2(unit)) {
                        if (uc == 2) {
                            addr = 0x01160000;
                        }
                    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
                    if (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK2(unit) ||
                        SOC_IS_TOMAHAWK3(unit) || SOC_IS_MAVERICK2(unit) ||
                        SOC_IS_FIRELIGHT(unit)) {
                        addr = 0x01100000 + 0x00060000 * uc;
                    }
#endif
#if defined(BCM_DNX_SUPPORT)
                    if (SOC_IS_DNX(unit)) {
                        addr = 0x01100000 + 0x00060000 * uc;
                    }
#endif
#ifdef BCM_APACHE_SUPPORT
                    if (SOC_IS_APACHE(unit)) {
                        addr = 0x01100000 + 0x00060000 * uc;
                    }
#endif
                    mcs_core_status(unit, uc, addr);
                }
            }

            return CMD_OK;
        }
#endif

        /* iProc - L2Cache + SRAM for now. */
        if (soc_uc_in_reset(unit, 0)) {
            cli_out("uC 0 status: RESET\n");
            sal_memcpy(fwInfo[0].status, "RESET", sizeof("RESET"));
        } else {
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit)) {
                mcs_core_status(unit, 0, 0x00004000);
            } else
#endif /* BCM_HURRICANE2_SUPPORT || BCM_HURRICANE3_SUPPORT */

            mcs_core_status(unit, 0, 0x1b004000);
        }
        if (SOC_INFO(unit).num_ucs > 1) {
            if (soc_uc_in_reset(unit, 1)) {
                cli_out("uC 1 status: RESET\n");
                sal_memcpy(fwInfo[1].status, "RESET", sizeof("RESET"));
            }
            else {
                mcs_core_status(unit, 1, 0x1b034000);
            }
        }
    }
    else {
        /* CMICm with TCMs */
        if (soc_uc_in_reset(unit, 0)) {
            cli_out("uC 0 status: RESET\n");
            sal_memcpy(fwInfo[0].status, "RESET", sizeof("RESET"));
        }
        else {           
            mcs_core_status(unit, 0, 0x100000);
        }
        if (soc_uc_in_reset(unit, 1)) {
            cli_out("uC 1 status: RESET\n");
            sal_memcpy(fwInfo[1].status, "RESET", sizeof("RESET"));
        }
        else {           
            mcs_core_status(unit, 1, 0x200000);
        }
    }

    return CMD_OK;
}


char mcstimestamp_usage[] =
    "Parameters: [event_id] [ENable|DISable|CHanGe]\n\t"
    "Display or configure timestamps captured by UC\n\t"
    "event_id = timestamp event index.  If empty, all will print.\n\t"
    "ENable = enable the specified timestamp event\n\t"
    "DISable = disable specified timestamp event, unless used by firmware\n\t"
    "CHanGe = poll specified timestamp event for a change before displaying\n";

cmd_result_t
mcstimestamp_cmd(int unit, args_t *a)
{
    char uint64_decimal_string[27];
    int rv, i;
    int min_event_id = 0;
    int max_event_id = (soc_feature(unit, soc_feature_iproc)) ? (MOS_MSG_MAX_TS_EVENTS-1)/*18*/ : 12;

    char *arg = ARG_GET(a);
    uint64 diff;
    int poll_for_change = 0;

    if (!sh_check_attached("mcsmsg", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return (CMD_FAIL);
    }

    if (arg) {
        int event_id;

        if (!isint(arg)) {
            return CMD_USAGE;
        }
        event_id = parse_integer(arg);

        arg = ARG_GET(a);
        if (arg) {
            if (parse_cmp("ENable", arg, 0)) {
                if (SOC_FAILURE(soc_cmic_uc_msg_timestamp_enable(unit, event_id))) {
                    return CMD_FAIL;
                } else {
                    return CMD_OK;
                }
            }
            if (parse_cmp("DISable", arg, 0)) {
                if (SOC_FAILURE(soc_cmic_uc_msg_timestamp_disable(unit, event_id))) {
                    return CMD_FAIL;
                } else {
                    return CMD_OK;
                }
            }
            if (parse_cmp("CHanGe", arg, 0)) {
                poll_for_change = 1;
            } else {
                return CMD_USAGE;
            }
        }

        /* only a single integer argument */
        min_event_id = event_id;
        max_event_id = event_id;
    }

    cli_out(" #  Last Timestamp    Prev Timestamp    <difference>  Equivalent TS1     Equivalent Full Time\n");

    for (i = min_event_id; i <= max_event_id; ++i) {
        soc_cmic_uc_ts_data_t ts_data;

        rv = soc_cmic_uc_msg_timestamp_get(unit, i, &ts_data);
        if (poll_for_change) {
            soc_cmic_uc_ts_data_t ts_data_orig = ts_data;
            int iter = 0;
            const int max_iter = 100000;
            while (rv == SOC_E_NONE && COMPILER_64_EQ(ts_data.hwts,ts_data_orig.hwts) && ++iter < max_iter) {
                rv = soc_cmic_uc_msg_timestamp_get(unit, i, &ts_data);
            }
            if (iter == max_iter) {
                cli_out("Timed out waiting for change in timestamp\n");
            }
        }
        if (SOC_FAILURE(rv)) {
            cli_out("soc_cmic_uc_msg_timestamp_get failed: %s (%d)\n", soc_errmsg(rv), rv);
        } else {
            diff = ts_data.hwts;
            COMPILER_64_SUB_64(diff, ts_data.prev_hwts);

            format_uint64_decimal(uint64_decimal_string, ts_data.time.seconds, 0);

            cli_out("%2d: %08x:%08x %08x:%08x <%10u>  %08x:%08x  %s.%09u\n",
                    i,
                    (unsigned)COMPILER_64_HI(ts_data.hwts), (unsigned)COMPILER_64_LO(ts_data.hwts),
                    (unsigned)COMPILER_64_HI(ts_data.prev_hwts), (unsigned)COMPILER_64_LO(ts_data.prev_hwts),
                    (unsigned)COMPILER_64_LO(diff),
                    (unsigned)COMPILER_64_HI(ts_data.hwts_ts1), (unsigned)COMPILER_64_LO(ts_data.hwts_ts1),
                    uint64_decimal_string, (unsigned)ts_data.time.nanoseconds);
        }
    }

    return CMD_OK;
}


#endif /* CMICM support */
