/*! \file bcma_mcs.c
 *
 * "mcs" command implementation
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

#include <bcma/mcs/bcma_mcs.h>
#include <bcma/io/bcma_io_file.h>

#include <bcmbd/bcmbd_mcs_internal.h>
/* Only the MCS_CFG_APP_UC_NUM_ADDR is used from bcmbd_mcs_internal.h */


/*! Extract Byte-x from a uint32
 * Use this macro instead of pointer cast
 * to be Endian-agonostic
 */
#define CH_X_FROM_UINT32(u, x) ((char)(u >> (x*8)) & 0xff)

/*******************************************************************************
 * Private functions
 */

/*
 * It is assumed that by the time this routine is called
 * the record is already validated. So, blindly get data.
 */
static void
mcs_get_rec_data(char *line, int count, uint32_t *dat)
{
    int i, j, datpos = 9;  /* 9 is valid for ihex */
    uint8_t bytes[4];
    uint32_t word;

    if (!count) {
        return;
    }

    if (line[0] != ':') {
        /* SREC.. Skip type, adress etc.. */
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
            break;
        }
    }

    /* Assign as uint32_t * to take care of endianess */
    for(i = 0; i < count; i++) {
        /* 1 word = 4 bytes = 8 hex chars */
        for(j = 3; j >= 0; j--,datpos+=2) {
                bytes[j] = (shr_util_xch2i(line[datpos]) << 4) |
                           (shr_util_xch2i(line[datpos + 1]));
        }
        word = (bytes[3]) +
               (bytes[2] << 8) +
               (bytes[1] << 16) +
               (bytes[0] << 24);
        *(dat+i) = word;
    }
}

static int
mcs_parse_srec_record(char *line, uint32_t *off)
{
    int count, rv = -1;
    uint32_t address;

    switch (line[1]) {      /* Record Type */
        case '0':           /* Header record - ignore */
            rv = 0;
            break;

        case '1' :          /* Data Record with 2 byte address */
            count = (shr_util_xch2i(line[2]) << 4) |
                    (shr_util_xch2i(line[3]));
            count -= 3; /* 2 address + 1 checksum */

            address = (shr_util_xch2i(line[4]) << 12) |
                      (shr_util_xch2i(line[5]) << 8) |
                      (shr_util_xch2i(line[6]) << 4) |
                      (shr_util_xch2i(line[7]));

            *off = address;
            rv = count;
            break;

        case '2' :          /* Data Record with 3 byte address */
            count = (shr_util_xch2i(line[2]) << 4) |
                    (shr_util_xch2i(line[3]));
            count -= 4; /* 3 address + 1 checksum */

            address = (shr_util_xch2i(line[4]) << 20) |
                      (shr_util_xch2i(line[5]) << 16) |
                      (shr_util_xch2i(line[6]) << 12) |
                      (shr_util_xch2i(line[7]) << 8) |
                      (shr_util_xch2i(line[8]) << 4) |
                      (shr_util_xch2i(line[9]));

            *off = address;
            rv = count;
            break;

        case '3' :          /* Data Record with 4 byte address */
            count = (shr_util_xch2i(line[2]) << 4) |
                    (shr_util_xch2i(line[3]));
            count -= 5; /* 4 address + 1 checksum */

            address = (shr_util_xch2i(line[4]) << 28) |
                      (shr_util_xch2i(line[5]) << 24) |
                      (shr_util_xch2i(line[6]) << 20) |
                      (shr_util_xch2i(line[7]) << 16) |
                      (shr_util_xch2i(line[8]) << 12) |
                      (shr_util_xch2i(line[9]) << 8) |
                      (shr_util_xch2i(line[10]) << 4) |
                      (shr_util_xch2i(line[11]));

            *off = address;
            rv = count;
            break;

        case '5':         /* Record count - ignore */
        case '6':         /* End of block - ignore */
        case '7':         /* End of block - ignore */
        case '8':         /* End of block - ignore */
        case '9':         /* End of block - ignore */
            rv = 0;
            break;

        default :       /* We don't parse all other records */
            cli_out("Unsupported Record S%c\n", line[1]);
            rv = 0;
            break;
    }
    return rv;
}

static int
mcs_parse_ihex_record(char *line, uint32_t *off)
{
    static uint32_t ihex_ext_addr; /* iHex Extended Address */
    int count, rv = -1;
    uint32_t address;

    switch (line[8]) {      /* Record Type */
        case '0' :      /* Data Record */
            count = (shr_util_xch2i(line[1]) << 4) |
                    (shr_util_xch2i(line[2]));

            address = (shr_util_xch2i(line[3]) << 12) |
                      (shr_util_xch2i(line[4]) << 8) |
                      (shr_util_xch2i(line[5]) << 4) |
                      (shr_util_xch2i(line[6]));

            *off = ihex_ext_addr + address;
            rv = count;
            break;

        case '4' :      /* Extended Linear Address Record */
            address = (shr_util_xch2i(line[9]) << 12) |
                      (shr_util_xch2i(line[10]) << 8) |
                      (shr_util_xch2i(line[11]) << 4) |
                      (shr_util_xch2i(line[12]));

            ihex_ext_addr = (address << 16);
            cli_out("Exteded Linear Address 0x%x\n", ihex_ext_addr);
            rv = 0;
            break;

        default :       /* We don't parse all other records */
            cli_out("Unsupported Record\n");
            rv = 0;
            break;
    }
    return rv;
}

/* Get eApps type from the firmware file */
static mcs_app_type_t
mcs_get_app_type(int unit, bcma_io_file_handle_t fh)
{
    int cnt = 0;
    char buf[256], *cp = NULL;
    uint32_t addr = 0, recoff = 0;
    uint32_t recdata[64];
    mcs_app_type_t at = MCS_APP_UNKNOWN;

    while ((cp = bcma_io_file_gets(fh, buf, sizeof(buf))) != NULL) {
        if (buf[0] == 'S') {
            cnt = mcs_parse_srec_record(cp, &addr);
        } else if (buf[0] == ':') {
            cnt = mcs_parse_ihex_record(cp, &addr);
        } else {
            cli_out("Unknown Record Type\n");
            cnt = -1;
        }

        if (cnt == -1) {
            at = MCS_APP_UNKNOWN;
            break;
        }

        if (cnt % 4) {
            cli_out("record not a multiple of 4\n");
            at = MCS_APP_UNKNOWN;
            break;
        }

        if ((MCS_CFG_APP_UC_NUM_ADDR >= addr) &&
            (MCS_CFG_APP_UC_NUM_ADDR <= (addr + cnt))) {
            /* Offset within this record */
            recoff = (MCS_CFG_APP_UC_NUM_ADDR - addr) / 4;
            mcs_get_rec_data(cp, (recoff + 1), recdata);
            at = (mcs_app_type_t)(recdata[recoff] >> 16);
            break;
        }
    }

    return at;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_mcs_load(int unit, int uc, const char *fname,
              bcmbd_mcs_res_req_t *req, uint32_t flags)
{
    int rv = BCMA_MCS_OK;
    bcma_io_file_handle_t fh;
    int num_uc, bd_uc = uc;
    bcmbd_ukernel_config_t bd_cfg;
    uint32_t rm_flags = BCMBD_MCS_RM_FLAG_INCLUDE_SELF;
    bcmbd_mcs_res_req_t bd_req;
    uint32_t addr = 0;
    char buf[256], *cp = NULL;
    uint32_t recdata[64];
    int i, cnt = 0;

    num_uc = bcmbd_mcs_num_uc(unit);
    if (num_uc == 0) {
        cli_out("No valid uCs\n");
        return BCMA_MCS_FAIL;
    }

    if (sal_strcmp(fname, "NONE") == 0) {
        fh = NULL;
        cli_out("Skipping file load & config\n");
        if (uc== -1) {
            cli_out("Nothing to do\n");
            return BCMA_MCS_OK;
        }
    } else {
        if ((fh = bcma_io_file_open(fname, "r")) == NULL) {
            return BCMA_MCS_FILE_NOT_FOUND;
        }

        /* Auto Core */
        if (flags & BCMA_MCS_LOAD_FLAG_AUTO_CORE) {
            rm_flags |= BCMBD_MCS_RM_FLAG_AUTO_CORE;
        }

        /* Auto Resources */
        if (flags & BCMA_MCS_LOAD_FLAG_AUTO_CONFIG) {
            bd_req.app = mcs_get_app_type(unit, fh);
            /* "rewind" the file after getting the app type */
            (void)bcma_io_file_size(fh);
            rm_flags |= BCMBD_MCS_RM_FLAG_AUTO_CONFIG;
        } else {
            sal_memcpy(&bd_req, req, sizeof(bcmbd_mcs_res_req_t));
        }

        /* Try to reserve core & resources */
        if (bcmbd_mcs_rm_reserve(unit, uc, &bd_req, &bd_cfg, rm_flags)
            != SHR_E_NONE) {
            bcma_io_file_close(fh);
            return BCMA_MCS_NO_RESOURCES;
        }
        bd_uc = bd_cfg.uc;
    }

    /* Recheck if we have a valid core number */
    if ((bd_uc < 0) || (bd_uc >= num_uc )) {
        if (fh) {
            bcma_io_file_close(fh);
        }
        return BCMA_MCS_FAIL;
    }

    /* Stop messaging if already going on */
    (void)bcmbd_mcs_uc_msg_uc_stop(unit, bd_uc);
    /* Reset Microcontroller */
    (void)bcmbd_mcs_uc_reset(unit, bd_uc);

    /* Valid file configuration.
     * Load file to uC & set configurations
     */
    if (fh) {
        while ((cp = bcma_io_file_gets(fh, buf, sizeof(buf))) != NULL) {
            if (buf[0] == 'S') {
                cnt = mcs_parse_srec_record(cp, &addr);
            } else if (buf[0] == ':') {
                cnt = mcs_parse_ihex_record(cp, &addr);
            } else {
                /* Unknown type */
                cli_out("Unknown Record Type\n");
                cnt = -1;
            }

            if (cnt == -1) {
                rv = BCMA_MCS_FAIL;
                break;
            }

            if (cnt % 4) {
                cli_out("record not a multiple of 4\n");
                rv = BCMA_MCS_FAIL;
                break;
            }

            cnt /= 4;
            addr = bcmbd_mcs_to_pci_addr(unit, bd_uc, addr);
            mcs_get_rec_data(cp, cnt, recdata);
            for (i = 0; i < cnt; i++, addr += 4) {
                bcmdrd_hal_iproc_write(unit, addr, recdata[i]);
            }
        }
        bcma_io_file_close(fh);

        if (rv == BCMA_MCS_OK) {
            if (bcmbd_mcs_uc_cfg_set(unit, bd_uc, &bd_cfg) != SHR_E_NONE) {
                return BCMA_MCS_FAIL;
            }
        } else {
            /* File load failed */
            return rv;
        }
    } else {
        /* No valid file. skip remaining operations */
        return BCMA_MCS_OK;
    }

    /* Start Microcontroller if requested */
    if (flags & BCMA_MCS_LOAD_FLAG_STARTUC) {
        if (bcmbd_mcs_uc_start(unit, bd_uc) != SHR_E_NONE) {
            return BCMA_MCS_FAIL;
        }
    } else {
        /* uC not started. Skip remaining operations */
        return BCMA_MCS_OK;
    }

    /* Start messaging if requested */
    if (flags & BCMA_MCS_LOAD_FLAG_STARTMSG) {
        /* Returns SHR_E_INIT if already initialized. This is not an error. */
        rv = bcmbd_mcs_uc_msg_start(unit);
        if ((rv != SHR_E_NONE) && (rv != SHR_E_INIT)) {
            return BCMA_MCS_FAIL;
        }
        /* Start messaging with uC */
        if (bcmbd_mcs_uc_msg_uc_start(unit, bd_uc) != SHR_E_NONE) {
            return BCMA_MCS_FAIL;
        }
    }

    return BCMA_MCS_OK;
}

int
bcma_mcs_status(int unit, int uc, int show_cfg, int show_dbg)
{
    int i, reg, ucx, start, end, num_uc;
    bcmbd_ukernel_info_t inf;
    bcmbd_ukernel_config_t cfg;
    mcs_debug_extn_t dbg;
    uint32_t idx, data;
    shr_pb_t *pb;

    num_uc = bcmbd_mcs_num_uc(unit);
    if (num_uc == 0) {
        cli_out("No valid uCs\n");
        return BCMA_MCS_FAIL;
    }

    if (uc >= 0 && uc < num_uc) {
        start = uc;
        end = uc + 1;
    } else {
        /* All Processors */
        start = 0;
        end = num_uc;
    }

    for (ucx = start; ucx < end; ucx++) {
        sal_strncpy(inf.status, "ERR", MCS_INFO_STATUS_SIZE);
        (void) bcmbd_mcs_uc_status(unit, ucx, &inf);
        cli_out("uC %d : %s\n", ucx, inf.status);
        if (sal_strcmp(inf.status, "Fault") == 0) {
            cli_out("\tcpsr\t0x%"PRIx32"\n", inf.cpsr);
            cli_out("\ttype\t0x%"PRIx32"\n", inf.type);
            cli_out("\tdfsr\t0x%"PRIx32"\n", inf.dfsr);
            cli_out("\tdfar\t0x%"PRIx32"\n", inf.dfar);
            cli_out("\tifsr\t0x%"PRIx32"\n", inf.ifsr);
            cli_out("\tifar\t0x%"PRIx32"\n", inf.ifar);
            for (reg = 0; reg < 16; reg++) {
                cli_out("\tr%d\t0x%"PRIx32"\n", reg, inf.armreg[reg]);
            }
        }
        if (sal_strcmp(inf.status, "OK") == 0) {
            cli_out("  Version: %s\n", inf.fw_version);
        }
        if (sal_strcmp(inf.status, "Reset") != 0) {
            /* mHost memories to be accessed only when it is not in reset */
            if (show_cfg) {
                bcmbd_mcs_uc_cfg_get(unit, ucx, &cfg);
                cli_out("  Config Values:\n");
                pb = shr_pb_create();
                shr_pb_format_uint64(pb, NULL, &(cfg.hostram_paddr), 1, 64);
                cli_out("\tHostRAM Base\t%s\n", shr_pb_str(pb));
                shr_pb_destroy(pb);
                cli_out("\tHostRAM Size\t0x%"PRIx32"\n", cfg.hostram_size);
                cli_out("\tSRAM Base\t0x%"PRIx32"\n", cfg.sram_base);
                cli_out("\tSRAM Size\t0x%"PRIx32"\n", cfg.sram_size);
                cli_out("\tSRAM Uncached Size\t0x%"PRIx32"\n", cfg.sram_unc_size);
                cli_out("\tSBUSDMA channels used\t0x%"PRIx32"\n", cfg.sbusdma_bmp);
                cli_out("\tPKTDMA channels used\t0x%"PRIx32"\n", cfg.pktdma_bmp);
                cli_out("\tSCHANFIFO channels used\t0x%"PRIx32"\n", cfg.schanfifo_bmp);
            }
            if (show_dbg) {
                bcmbd_mcs_uc_dbg_get(unit, ucx, &dbg);
                cli_out("  Thread History\n");
                idx = dbg.thrd_hist_idx;
                for (i = 16; i > 0; i--) {
                    data = dbg.thrd_hist[(idx + i) & 0xf];
                    cli_out("\t%c%c%c%c\n", CH_X_FROM_UINT32(data, 0),
                                            CH_X_FROM_UINT32(data, 1),
                                            CH_X_FROM_UINT32(data, 2),
                                            CH_X_FROM_UINT32(data, 3));
                }
            }
        }
    }

    return BCMA_MCS_OK;
}

int
bcma_mcs_message(int unit, int uc, int action)
{
    int num_uc, rv;

    num_uc = bcmbd_mcs_num_uc(unit);
    if (num_uc == 0) {
        cli_out("No valid uCs\n");
        return BCMA_MCS_FAIL;
    }

    if (action == BCMA_MCS_MSG_ACT_INIT) {
        rv = bcmbd_mcs_uc_msg_start(unit);
        return (rv == SHR_E_NONE) ? BCMA_MCS_OK : BCMA_MCS_FAIL;
    }

    if (uc < 0 || uc >= num_uc) {
        cli_out("uC Not valid\n");
        return BCMA_MCS_FAIL;
    }

    if (action == BCMA_MCS_MSG_ACT_START) {
        rv = bcmbd_mcs_uc_msg_uc_start(unit, uc);
    } else if (action == BCMA_MCS_MSG_ACT_STOP) {
        rv = bcmbd_mcs_uc_msg_uc_stop(unit, uc);
    } else {
        cli_out("Invalid action.\n");
        return BCMA_MCS_FAIL;
    }

    return (rv == SHR_E_NONE) ? BCMA_MCS_OK : BCMA_MCS_FAIL;
}
