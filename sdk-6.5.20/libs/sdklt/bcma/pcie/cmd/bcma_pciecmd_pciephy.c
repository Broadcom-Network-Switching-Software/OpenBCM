/*! \file bcma_pciecmd_pciephy.c
 *
 * CLI 'pciephy' command handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmbd/bcmbd_pcie.h>
#include <bcmbd/bcmbd_pcie.h>
#include <bcmbd/bcmbd_m0ssq.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <pciephy/pciephy.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmbd/bcma_bcmbd_qspi.h>
#include <bcma/pcie/bcma_pcie.h>
#include <bcma/pcie/bcma_pciecmd_pciephy.h>


/*******************************************************************************
 * Local defines
 */

/* Address register for PMI access. */
#define PCIE_SERDES_PMI_ADDR    0x1130

/* Write-data register for PMI access. */
#define PCIE_SERDES_PMI_WDATA   0x1134

/* Read-data register for PMI access. */
#define PCIE_SERDES_PMI_RDATA   0x1138

/* Offset of field RCMD in SERDES_PMI_WDATA register. */
#define PCIE_PMI_WDATA_RCMD     (1 << 30)

/* Offset of field WCMD in SERDES_PMI_WDATA register. */
#define PCIE_PMI_WDATA_WCMD     (1 << 31)

/* Offset of field VALID in SERDES_PMI_RDATA register. */
#define PCIE_PMI_RDATA_VALID    (1 << 31)

/* Magic number of the firmware image. */
#define PCIE_FW_MAGIC_NUM       0x50434549

/* Firmware header offset in the flash. */
#define PCIE_FW_HDR_OFFSET      0x2000

/* String length of the firmware version. */
#define PCIE_FW_VER_LEN         12

/*! PCIE trace start symbol. ASCII of "TRAC". */
#define PCIE_TRACE_SYMBOL0      0x54524143
/*! PCIE trace start symbol. ASCII of "ELOG". */
#define PCIE_TRACE_SYMBOL1      0x454c4f47
/* Handle endianess. */
#define ENDIAN_HANDLE(_v)       shr_util_host_to_le32(&_v, &_v, 1)

/* Firmware header format. */
typedef struct pciephy_fw_hdr_s  {

    /* Magic number to verify if FW is programmed into flash */
    uint32_t magic;

    /*
     * FW loader version.
     * Major version: bits[31-16], minor version: bits[15-0]
     */
    uint32_t loader_ver;

    /* FW image size */
    uint32_t size;

    /* FW image CRC */
    uint32_t crc;

    /* FW version string */
    char fw_ver[PCIE_FW_VER_LEN];

    /* Loader built date */
    uint32_t date;

} pciephy_fw_hdr_t;


/*******************************************************************************
 * PCIE core and phy register access functions
 */

static int
pcie_phy_type_get(int unit, pciephy_driver_type_t *type)
{
    int rv;

    rv = bcma_pcie_drv_init(unit);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    rv = bcma_pcie_phy_type_get(unit, type);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    return 0;
}

static int
pcie_phy_read(void *user_acc, uint32_t addr, uint16_t *val)
{
    pciephy_access_t *sa = user_acc;
    int rv, unit = sa->unit;
    uint32_t data, cmd;

    /* Accesse PHY indirectly via PCIE_SERDES_PMI_ADDR/DATA registers. */
    rv = bcmbd_pcie_core_reg_write(unit, PCIE_SERDES_PMI_ADDR, addr);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    /* Initiate read cycle */
    data = PCIE_PMI_WDATA_RCMD;
    rv = bcmbd_pcie_core_reg_write(unit, PCIE_SERDES_PMI_WDATA, data);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    /* Read data and check if read is complete */
    while (1) {
        rv = bcmbd_pcie_core_reg_read(unit, PCIE_SERDES_PMI_WDATA, &cmd);
        if (SHR_FAILURE(rv)) {
            return -1;
        }

        rv = bcmbd_pcie_core_reg_read(unit, PCIE_SERDES_PMI_RDATA, &data);
        if (SHR_FAILURE(rv)) {
            return -1;
        }

        if (((cmd & PCIE_PMI_WDATA_RCMD) == 0) &&
            ((data & PCIE_PMI_RDATA_VALID) != 0)) {
            break;
        }
    }

    *val = data;
    return 0;
}

static int
pcie_phy_write(void *user_acc, uint32_t addr, uint16_t val)
{
    pciephy_access_t *sa = user_acc;
    int rv, unit = sa->unit;
    uint32_t data, cmd;

    /* Accesse PHY indirectly via PCIE_SERDES_PMI_ADDR/DATA registers. */
    rv = bcmbd_pcie_core_reg_write(unit, PCIE_SERDES_PMI_ADDR, addr);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    /* Initiate write cycle */
    data = PCIE_PMI_WDATA_WCMD | val;
    rv = bcmbd_pcie_core_reg_write(unit, PCIE_SERDES_PMI_WDATA, data);
    if (SHR_FAILURE(rv)) {
        return -1;
    }

    /* Check if write is complete */
    do {
        rv = bcmbd_pcie_core_reg_read(unit, PCIE_SERDES_PMI_WDATA, &cmd);
        if (SHR_FAILURE(rv)) {
            return -1;
        }
    } while ((cmd & PCIE_PMI_WDATA_WCMD) != 0);

    return 0;
}

static pciephy_acc_bus_t pcie_phy_bus = {
    .name = "pcie_phy_bus",
    .read = pcie_phy_read,
    .write = pcie_phy_write
};


/*******************************************************************************
 * Sub-command handlers
 */

/* Get the PCIe core register. */
static int
pciephy_cmd_getepreg(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int unit = cli->cmd_unit;
    uint32_t addr, val;

    if (BCMA_CLI_ARG_CNT(args) != 1) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input address. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Read PCIE register value. */
    if (SHR_FAILURE(bcmbd_pcie_core_reg_read(unit, addr, &val))) {
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("PCIe getepreg: address = 0x%"PRIx32", data = 0x%"PRIx32"\n",
            addr, val);

    return BCMA_CLI_CMD_OK;
}

/* Set the PCIe core register. */
static int
pciephy_cmd_setepreg(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int unit = cli->cmd_unit;
    uint32_t addr, data, val;

    if (BCMA_CLI_ARG_CNT(args) != 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input address. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input data. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &data) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Write the given data to PCIE register. */
    if (SHR_FAILURE(bcmbd_pcie_core_reg_write(unit, addr, data))) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Read PCIE register value back. */
    if (SHR_FAILURE(bcmbd_pcie_core_reg_read(unit, addr, &val))) {
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("PCIe setepreg: address = 0x%"PRIx32", data = 0x%"PRIx32"\n",
            addr, val);

    return BCMA_CLI_CMD_OK;
}

/* Get the PCIe Serdes register. */
static int
pciephy_cmd_getreg(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv, unit = cli->cmd_unit;
    uint32_t addr;
    uint16_t val;
    pciephy_access_t sa;
    pciephy_driver_type_t type;
    uint32_t count = 1, lane = 0, i;

    if (BCMA_CLI_ARG_CNT(args) < 1 || BCMA_CLI_ARG_CNT(args) > 3) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input address. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get count and lane mask. */
    arg = BCMA_CLI_ARG_GET(args);
    if (!bcma_cli_parse_uint32(arg, &count)) {
        arg = BCMA_CLI_ARG_GET(args);
        if (bcma_cli_parse_uint32(arg, &lane) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    if (pcie_phy_type_get(unit, &type) < 0) {
        cli_out("%sFailed to get PCIe SerDes type.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&sa, 0, sizeof(sa));
    sa.unit = unit;
    sa.bus = &pcie_phy_bus;
    sa.type = type;
    sa.lane_mask = 0x1 << lane;
    for (i = 0; i < count; i++) {
        rv = pciephy_diag_reg_read(&sa, addr + i, &val);
        if (rv != PCIEPHY_E_NONE) {
            cli_out("%sFailed to read PCIe SerDes register (error code %d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, rv);
        } else {
            cli_out("PCIe getreg: address = 0x%"PRIx32", data = 0x%"PRIx32"\n",
                    addr + i, val);
        }
    }

    return BCMA_CLI_CMD_OK;
}

/* Set the PCIe Serdes register. */
static int
pciephy_cmd_setreg(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv, unit = cli->cmd_unit;
    uint32_t addr, data;
    pciephy_access_t sa;
    pciephy_driver_type_t type;
    uint32_t lane = 0;

    if (BCMA_CLI_ARG_CNT(args) < 2 || BCMA_CLI_ARG_CNT(args) > 3) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input address. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input data. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &data) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get lane mask. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &lane) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (pcie_phy_type_get(unit, &type) < 0) {
        cli_out("%sFailed to get PCIe SerDes type.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&sa, 0, sizeof(sa));
    sa.unit = unit;
    sa.bus = &pcie_phy_bus;
    sa.type = type;
    sa.lane_mask = 0x1 << lane;
    rv = pciephy_diag_reg_write(&sa, addr, data);
    if (rv != PCIEPHY_E_NONE) {
        cli_out("%sFailed to write PCIe SerDes register (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
    } else {
        cli_out("PCIe setreg: address = 0x%"PRIx32", data = 0x%"PRIx32"\n",
                addr, data);
    }

    return BCMA_CLI_CMD_OK;
}

/* Read the PCIe Serdes pram. */
static int
pciephy_cmd_pramread(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv, unit = cli->cmd_unit;
    uint32_t addr, size;
    pciephy_access_t sa;
    pciephy_driver_type_t type;

    if (BCMA_CLI_ARG_CNT(args) != 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input address. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &addr) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input size. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &size) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (pcie_phy_type_get(unit, &type) < 0) {
        cli_out("%sFailed to get PCIe SerDes type.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(&sa, 0, sizeof(sa));
    sa.unit = unit;
    sa.bus = &pcie_phy_bus;
    sa.type = type;
    rv = pciephy_diag_pram_read(&sa, addr, size);
    if (rv != PCIEPHY_E_NONE) {
        cli_out("%sFailed to read PCIe PRAM (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
    }

    return BCMA_CLI_CMD_OK;
}

/* PCIe diagnostics. */
static int
pciephy_cmd_diag(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv, unit = cli->cmd_unit;
    pciephy_access_t sa[PCIEPHY_MAX_CORES];
    pciephy_driver_type_t type;
    uint32_t lmap, cnt, lmask, cidx, sa_lmask;

    if (BCMA_CLI_ARG_CNT(args) != 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Get the input lane bitmap. */
    arg = BCMA_CLI_ARG_GET(args);
    if (bcma_cli_parse_uint32(arg, &lmap) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (pcie_phy_type_get(unit, &type) < 0) {
        cli_out("%sFailed to get PCIe SerDes type.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Fill the pciephy_access_t array. */
    sal_memset(sa, 0, sizeof(sa));
    cnt = 0;
    lmask = (1 << PCIEPHY_MAX_LANES_PER_CORE) - 1;
    for (cidx = 0; cidx < PCIEPHY_MAX_CORES; cidx++) {
        sa_lmask = lmap >> (PCIEPHY_MAX_LANES_PER_CORE * cidx) & lmask;
        if (sa_lmask == 0) {
            continue;
        }

        sa[cnt].unit = unit;
        sa[cnt].bus = &pcie_phy_bus;
        sa[cnt].type = type;
        sa[cnt].core = cidx;
        sa[cnt].lane_mask = sa_lmask;
        cnt++;
    }

    /* Execute the diagnostic command. */
    arg = BCMA_CLI_ARG_GET(args);
    if (sal_strcasecmp(arg, "dsc") == 0) {
        rv = pciephy_diag_dsc(sa, cnt);
    } else if (sal_strcasecmp(arg, "state") == 0) {
        rv = pciephy_diag_state(sa, cnt);
    } else if (sal_strcasecmp(arg, "eyescan") == 0) {
        rv = pciephy_diag_eyescan(sa, cnt);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    if (rv != PCIEPHY_E_NONE) {
        cli_out("%sFailed to execute PCIe diagnostics (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
    }

    return BCMA_CLI_CMD_OK;
}

/* PCIe firmware load. */
static int
pciephy_cmd_fwload(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv, unit = cli->cmd_unit;
    const char *fname;

    if (BCMA_CLI_ARG_CNT(args) != 1) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Load the fw image into flash. */
    fname = BCMA_CLI_ARG_GET(args);
    rv = bcma_bcmbd_qspi_flash_load(unit, 0, fname);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to execute QSPI flash load (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcmbd_pcie_fw_prog_done(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to refresh the new firmware (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("PCIE firmware updated successfully. Please reset the system.\n");

    return BCMA_CLI_CMD_OK;
}

/* PCIe firmware information. */
static int
pciephy_cmd_fwinfo(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv, unit = cli->cmd_unit;
    pciephy_driver_type_t type;
    pciephy_fw_hdr_t fw_hdr;
    char *cmd;
    bool dump_raw = false;

    /* Parse sub-command */
    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd && bcma_cli_parse_cmp("raw", cmd, ' ')) {
        dump_raw = true;
    }

    if (pcie_phy_type_get(unit, &type) < 0) {
        cli_out("%sFailed to get PCIe SerDes type.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Get the fw header from flash. */
    rv = bcma_bcmbd_qspi_flash_read(unit, PCIE_FW_HDR_OFFSET, sizeof(fw_hdr),
                                    (uint8_t *)&fw_hdr);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to execute QSPI flash read (error code %d)\n",
                BCMA_CLI_CONFIG_ERROR_STR, rv);
        return BCMA_CLI_CMD_FAIL;
    }
    ENDIAN_HANDLE(fw_hdr.magic);
    ENDIAN_HANDLE(fw_hdr.loader_ver);
    ENDIAN_HANDLE(fw_hdr.crc);
    ENDIAN_HANDLE(fw_hdr.size);
    ENDIAN_HANDLE(fw_hdr.date);

    /* Check the magic number. */
    if (fw_hdr.magic == PCIE_FW_MAGIC_NUM) {
        uint32_t ver_maj = fw_hdr.loader_ver >> 16;
        uint32_t ver_min = fw_hdr.loader_ver & 0xffff;
        uint32_t i;
        uint32_t idx = 0;
        uint32_t val, val1;
        uint16_t val16;
        int fcount = 0;
        pciephy_access_t pa;
        const char *sname;
        bcmdrd_sym_info_t sinfo;

        /* Loader version > 2.6xxx. */
        if (ver_maj > 2 ||
            (ver_maj == 2 && ((ver_min < 1000 && ver_min > 6) ||
             ver_min >= 7000))) {
            cli_out("\tPCIe FW loader version: %d.%d\n"
                    "\tPCIe FW version: %s\n"
                    "\tPCIe FW loader built date: %x\n",
                    fw_hdr.loader_ver>>16,
                    fw_hdr.loader_ver & 0xffff,
                    fw_hdr.fw_ver, fw_hdr.date);

            /* Check Serdes firmware version. */
            sal_memset(&pa, 0, sizeof(pa));
            pa.unit = unit;
            pa.bus = &pcie_phy_bus;
            pa.type = type;
            pa.lane_mask = 0;
            rv = bcma_pcie_fw_ver_get(&pa, &val16);
            if (SHR_FAILURE(rv)) {
                cli_out("pciephy_diag_reg_read failed: %d\n", rv);
            }
            else {
                if (val16) {
                    cli_out("\nFirmware was loaded! (version: 0x%x)\n", val16);
                } else {
                    cli_out("\nFirmware was not loaded!\n");
                }
            }

            /* Dump raw log. */
            if (dump_raw) {
                cli_out("PCIE firmware log raw data:\n");
                for (i = 0; i < 128; i++){
                    bcma_pcie_trace_get(unit, i, &val);
                    ENDIAN_HANDLE(val);
                    cli_out("\t[%3d]: 0x%x\n", i, val);
                }
            } else {
                /* Check trace log start symbols. */
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                bcma_pcie_trace_get(unit, idx++, &val1);
                ENDIAN_HANDLE(val);
                if (val != PCIE_TRACE_SYMBOL0 ||
                    val1 != PCIE_TRACE_SYMBOL1) {
                    cli_out("Trace log not found, please check bootstrap\n"
                            "and QSPI configuration on the board!\n"
                            "(0x%08x, 0x%08x)\n", val, val1);

                    sname = "ICFG_MHOST0_STRAPSr";
                    sal_memset(&sinfo, 0, sizeof(sinfo));
                    rv = bcmdrd_pt_info_get_by_name(unit, sname, &sinfo);
                    if (SHR_SUCCESS(rv)) {
                        bcmdrd_hal_iproc_read(unit, sinfo.offset, &val);
                        ENDIAN_HANDLE(val);
                        cli_out("ICFG_MHOST0_STRAPSr[0x%08x]: 0x%08x\n",
                                sinfo.offset, val);
                    }

                    sname = "ICFG_ROM_STRAPSr";
                    sal_memset(&sinfo, 0, sizeof(sinfo));
                    rv = bcmdrd_pt_info_get_by_name(unit, sname, &sinfo);
                    if (SHR_SUCCESS(rv)) {
                        bcmdrd_hal_iproc_read(unit, sinfo.offset, &val);
                        ENDIAN_HANDLE(val);
                        cli_out("ICFG_ROM_STRAPSr[0x%08x]: 0x%08x\n",
                                sinfo.offset, val);
                    }
                    return BCMA_CLI_CMD_OK;
                }
                /* Print heart-beat status. */
                bcma_pcie_trace_get(unit, idx, &val);
                sal_usleep(500000);
                bcma_pcie_trace_get(unit, idx++, &val1);
                if (val == val1) {
                    cli_out("Loader stopped! (heart-beat = 0x%x)\n", val);
                } else {
                    cli_out("Loader is running! (Last heart-beat = 0x%x)\n",
                            val1);
                }
                /* Print summary. */
                cli_out("Loader running summary:\n");
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                cli_out("\tStage1 loader run times: %d\n", val);
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                cli_out("\tStage2 loader run times: %d\n", val);
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                cli_out("\tLoading process run times: %d\n", val);
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                cli_out("\tLoading failure times: %d\n", val);
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                if (val) {
                    cli_out("\tFailure log count: %d\n", val);
                }
                fcount = val;
                bcma_pcie_trace_get(unit, idx++, &val);
                ENDIAN_HANDLE(val);
                if (val) {
                    cli_out("\tLog internal failure: 0x%x\n", val);
                }
                for (i = 0; i < 5; i++){
                    bcma_pcie_trace_get(unit, idx++, &val);
                    ENDIAN_HANDLE(val);
                    if (val) {
                        cli_out("\tPCIE Reset status in %d time run: 0x%x\n",
                                i + 1, val);
                    }
                }
                /* bypass 6 reserved buffers. */
                idx += 6;
                cli_out("Stage1 time logs:\n");
                for (i = 0; i < 8; i++){
                    bcma_pcie_trace_get(unit, idx++, &val);
                    ENDIAN_HANDLE(val);
                    bcma_pcie_trace_get(unit, idx++, &val1);
                    ENDIAN_HANDLE(val);
                    if (val) {
                        cli_out("\tLine %d time: %d\n", val, val1);
                    }
                }
                cli_out("Stage2 time logs:\n");
                for (i = 0; i < 16; i++){
                    bcma_pcie_trace_get(unit, idx++, &val);
                    ENDIAN_HANDLE(val);
                    bcma_pcie_trace_get(unit, idx++, &val1);
                    ENDIAN_HANDLE(val);
                    if (val) {
                        cli_out("\tLine %d time: %d\n", val, val1);
                    }
                }
                if (fcount) {
                    cli_out("Failure logs:\n");
                    for (i = 0; i < 16; i++){
                        bcma_pcie_trace_get(unit, idx++, &val);
                        ENDIAN_HANDLE(val);
                        if (val) {
                            cli_out("\tstatus %d line: %d\n",
                                    val >> 16, val & 0xffff);
                        }
                    }
                }
            }
        } else {
            cli_out("\tPCIe FW loader version: %d.%d\n\tPCIe FW version: %s\n",
                    fw_hdr.loader_ver >> 16,
                    fw_hdr.loader_ver & 0xffff,
                    fw_hdr.fw_ver);
        }
    } else {
        cli_out("Firmware is invalid!\n");
    }

    return BCMA_CLI_CMD_OK;
}



/* Sub-command list. */
static bcma_cli_command_t pciephy_cmds[] = {
    { "getepreg", pciephy_cmd_getepreg },
    { "setepreg", pciephy_cmd_setepreg },
    { "getreg", pciephy_cmd_getreg },
    { "setreg", pciephy_cmd_setreg },
    { "pramread", pciephy_cmd_pramread },
    { "diag", pciephy_cmd_diag },
    { "fwload", pciephy_cmd_fwload },
    { "fwinfo", pciephy_cmd_fwinfo },
};


/*******************************************************************************
 * pcie command handler
 */

int
bcma_pciecmd_pciephy(bcma_cli_t *cli, bcma_cli_args_t *args)
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

    for (idx = 0; idx < COUNTOF(pciephy_cmds); idx++) {
        if (sal_strcasecmp(pciephy_cmds[idx].name, arg) == 0) {
            return (*pciephy_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
