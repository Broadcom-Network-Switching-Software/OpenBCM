/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 * 
 * Purpose: PCIe Diagnostics utilities
 */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) || (defined(BCM_ESW_SUPPORT) && defined(PCIEG3_DIAG_SUPPORT))
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <sal/compiler.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/iproc.h>
#include <soc/cmic.h>
#include <soc/cmicx_qspi.h>
#ifdef INCLUDE_CPU_I2C
#include <sal/appl/i2c.h>
#endif
#include <sal/core/time.h>
#include <soc/mcm/memregs.h>
#include <shared/bsl.h>
#include <sdk_config.h>
#include <soc/soc_flash.h>
#include <pcig3_phy_acc.h>

# define SPI_SPEED    62500000 /* BSPI */
# define SPI_MODE     3 /* (CPOL|CPHA) */

#define SWAP_ENDIAN(x) ( \
    (((x) << 24) & 0xff000000) | \
    (((x) <<  8) & 0x00ff0000) | \
    (((x) >>  8) & 0x0000ff00) | \
    (((x) >> 24) & 0x000000ff))

#ifdef INCLUDE_CPU_I2C
#define IPROC_ACCESS_READ(unit, addr, read_value_p) iproc_qspi_read(unit, addr, read_value_p)
#define IPROC_ACCESS_WRITE(unit, addr, value) iproc_qspi_write(unit, addr, value)
#else
#define IPROC_ACCESS_READ(unit, addr, rvp) soc_iproc_getreg(unit, addr, rvp)
#define IPROC_ACCESS_WRITE(unit, addr, rvp) soc_iproc_setreg(unit, addr, rvp)
#endif /* INCLUDE_CPU_I2C */
#define IPROC_ACCESS_METHOD_PCIE 0 /* Access using the 1st PCIe BAR (default) */
#define IPROC_ACCESS_METHOD_I2C 1 /* Access by I2C using specified IDs */
#define IPROC_ACCESS_METHOD_I2C_BDE 2 /* by I2C using the BDE */


static int
pcie_srds_bus_read(void *user_acc, uint32_t addr, uint16_t *val)
{
    srds_access_t *sa = user_acc;
    int unit = sa->unit;

    if (soc_pcie_phy_read(unit, addr, val) != SOC_E_NONE) {
        return -1;
    }

    return 0;
}

static int
pcie_srds_bus_write(void *user_acc, uint32_t addr, uint16_t val)
{
    srds_access_t *sa = user_acc;
    int unit = sa->unit;

    if (soc_pcie_phy_write(unit, addr, val) != SOC_E_NONE) {
        return -1;
    }

    return 0;
}

static srds_bus_t pcie_srds_bus = {
    .name = "pcie_bus_merlin16",
    .read = pcie_srds_bus_read,
    .write = pcie_srds_bus_write
};

static int
_pcie_phy_fw_flash_init(int unit,
                        soc_flash_conf_t *flash)
{
    unsigned int speed = SPI_SPEED;
    unsigned int mode = SPI_MODE;
    int rv = SOC_E_NONE;

    rv = soc_flash_init(unit, speed, mode, flash);
    if (SOC_FAILURE(rv)) {
        cli_out("Error: Init SPI flash speed = %u, mode =%u\n", speed, mode);
    }
    return rv;
}
#ifndef NO_FILEIO
STATIC int
_pcie_phy_fw_write_block(int unit,
                         uint32 offset,
                         size_t len,
                         const uint8 *buf)
{
    int rv = SOC_E_NONE;
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    sal_usecs_t diff;
#define PCIEFW_MEASURE_START diff = sal_time_usecs();
#define PCIEFW_MEASURE_END(testname) \
    diff = sal_time_usecs() - diff; \
    total_##testname += diff; \
    if (max_##testname < diff) max_##testname = diff; \
    if (min_##testname > diff) min_##testname = diff;
#define PCIEFW_MEASURE_END_SINGLE(testname) \
    diff = sal_time_usecs() - diff; \
    cli_out("%s operation took %u us\n", #testname, diff);
#define PCIEFW_MEASURE_SUMMARY(testname) \
    cli_out("Test %s min/average/max us: %u/%u/%u\n", #testname, min_ ## testname, \
      (total_ ## testname + iters / 2) / iters, max_ ## testname);
#else
#define PCIEFW_MEASURE_START
#define PCIEFW_MEASURE_END(testname)
#define PCIEFW_MEASURE_END_SINGLE(testname)
#define PCIEFW_MEASURE_SUMMARY(testname)
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */

    PCIEFW_MEASURE_START;
    rv = soc_flash_erase(unit, offset, len);
    PCIEFW_MEASURE_END_SINGLE(erase);

    if (SOC_SUCCESS(rv)) {
        PCIEFW_MEASURE_START;
        rv = soc_flash_write(unit, offset, len, buf);
        PCIEFW_MEASURE_END_SINGLE(write);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to write = 0x%x\n", offset);
        }
    } else {
        cli_out("Failed to erase = 0x%x\n", offset);
    }

    return rv;
}
#endif

STATIC cmd_result_t
_pcie_phy_fw_load(int unit, args_t *args)
{
#ifndef NO_FILEIO
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    uint32 iters = 0;
    sal_usecs_t diff, total_enable = 0, min_enable = -1, max_enable = 0, total_write = 0, min_write = -1, max_write = 0, total_read = 0, min_read = -1, max_read = 0;
#define PCIEFW_MEASURE_START diff = sal_time_usecs();
#define PCIEFW_MEASURE_END(testname) \
    diff = sal_time_usecs() - diff; \
    total_##testname += diff; \
    if (max_##testname < diff) max_##testname = diff; \
    if (min_##testname > diff) min_##testname = diff;
#define PCIEFW_MEASURE_SUMMARY(testname) \
    cli_out("Test %s min/average/max us: %u/%u/%u\n", #testname, min_ ## testname, \
      (total_ ## testname + iters / 2) / iters, max_ ## testname);
#else
#define PCIEFW_MEASURE_START
#define PCIEFW_MEASURE_END(testname)
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */
    int rv = SOC_E_NONE;
    char *file_str;
    long len;
    uint32 offset = 0, val;
    uint8 *wbuf = NULL;
    uint8 *rbuf = NULL;
    soc_flash_conf_t flash;
    FILE *in = NULL;

    /* Parse command */
    if ((file_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* initialize the flash driver */
    rv = _pcie_phy_fw_flash_init(unit, &flash);
    if (SOC_FAILURE(rv)) {
        goto error_init;
    }
    wbuf = sal_alloc(flash.sector_size, "FLASH_W");
    if (!wbuf) {
        rv = SOC_E_MEMORY;
        goto error;
    }

    rbuf = sal_alloc(flash.sector_size, "FLASH_R");
    if (!rbuf) {
        rv = SOC_E_MEMORY;
        goto error;
    }
    /* Open file and get the file size */
    cli_out("Opening file: %s\n", file_str);

    in = sal_fopen(file_str, "r");
    if (!in) {
        /* failure to open existing file is error */
        rv = _SHR_E_EXISTS;
        goto error;
    }

    sal_fseek(in, 0, SEEK_END);
    if((len = sal_ftell(in)) == -1) {
        rv = _SHR_E_INTERNAL;
        goto error;
    }
    if ((sal_fseek(in, 0, SEEK_SET) != 0) || (len == 0)) {
        rv = _SHR_E_INTERNAL;
        goto error;
    }
    cli_out("Updating PCIE firmware\n");
    while (len) {
        size_t size = (flash.sector_size > len) ? len : flash.sector_size;
        int i;
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
        ++iters;
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */
        /* Read the sector data */
        /*    coverity[tainted_data_argument]    */
        if (sal_fread(wbuf, sizeof(uint8), size, in) != size) {
            rv = _SHR_E_INTERNAL;
            goto error;
        }

        PCIEFW_MEASURE_START
        rv = soc_flash_cmd_write_status(unit, 0);
        PCIEFW_MEASURE_END(enable)
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to unlock all sectors\n");
            break;
        }
        PCIEFW_MEASURE_START
        rv = _pcie_phy_fw_write_block(unit, offset, size, wbuf);
        PCIEFW_MEASURE_END(write)
        if (SOC_FAILURE(rv)) {
            break;
        }

        /* Read back to verify */
        PCIEFW_MEASURE_START
        rv = soc_flash_read(unit, offset, size, rbuf);
        PCIEFW_MEASURE_END(read)
        if (SOC_FAILURE(rv)) {
            break;
        }
        for (i = 0; i < size; i++) {
            if (wbuf[i] != rbuf[i]) {
                cli_out("Data Mismatch: offset= 0x%x, write = 0x%x, read = 0x%x\n",
                        i, wbuf[i], rbuf[i]);
                rv = SOC_E_FAIL;
                break;
            }
        }
        len = len - size;
        offset += size;
    }
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    cli_out("Ran %u iterations sector size: %u\n", iters, (unsigned)flash.sector_size);
    PCIEFW_MEASURE_SUMMARY(enable);
    PCIEFW_MEASURE_SUMMARY(write);
    PCIEFW_MEASURE_SUMMARY(read);
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */
    if (SOC_SUCCESS(rv)) {
        /* Clear UC_PROG_DONE bit so updated FW can take effect upon reset */
        READ_PAXB_0_GEN3_UC_LOADER_STATUSr(unit, &val);
        soc_reg_field_set(unit, PAXB_0_GEN3_UC_LOADER_STATUSr, &val,
                                  UC_PROG_DONEf, 0);
        WRITE_PAXB_0_GEN3_UC_LOADER_STATUSr(unit, val);
        cli_out("PCIE firmware updated successfully. Please reset the system... \n");
    }
error:
    /*  Close the driver */
    soc_flash_cleanup(unit);
    if (in) {
        sal_fclose(in);
    }
error_init:
    if (rbuf) {
        sal_free(rbuf);
    }
    if (wbuf) {
        /* coverity[tainted_data] */
        sal_free(wbuf);
    }
    if (SOC_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
#else
    cli_out("File system not supported\n");
    return CMD_FAIL;
#endif
}

#define PCIE_G3_FW_MAGIC_NUM  (0x50434549)
#define PCIE_GE_FW_HDR_OFFSET (0x2000)

typedef struct soc_pcieg3_fw_hdr_s{
    uint32 magic; /* Magic number to verify if FW is programmed into flash */
    uint32 ldrver; /* FW loader version, major version: bits[31-16], minor version: bits[15-0] */
    uint32 size; /* FW image size */
    uint32 crc;  /* FW image CRC */
    char   version[12]; /* FW version string */
}soc_pcieg3_fw_hdr_t;

STATIC cmd_result_t
_pcie_phy_fw_version(int unit)
{
    soc_pcieg3_fw_hdr_t fwhdr;
    soc_flash_conf_t flash;
    uint32 endian = 1;
    int rv = SOC_E_NONE;

    if(*(uint8 *)(&endian) == 1)
        endian = 0;

    /* initialize the flash driver */
    rv = _pcie_phy_fw_flash_init(unit, &flash);
    if (SOC_FAILURE(rv)) {
        goto error_version;
    }

    rv = soc_flash_read(unit, PCIE_GE_FW_HDR_OFFSET, sizeof(soc_pcieg3_fw_hdr_t), (uint8 *) &fwhdr);
    if (SOC_FAILURE(rv)) {
        goto error_version;
    }

    if(endian) {
       fwhdr.magic = SWAP_ENDIAN(fwhdr.magic);
       fwhdr.ldrver = SWAP_ENDIAN(fwhdr.ldrver);
       fwhdr.size = SWAP_ENDIAN(fwhdr.size);
       fwhdr.crc = SWAP_ENDIAN(fwhdr.crc);
    }
    if(fwhdr.magic == PCIE_G3_FW_MAGIC_NUM) {
        cli_out("\tPCIe FW loader version: %d.%d\n\tPCIe FW version: %s\n",
                                          fwhdr.ldrver >>16,
                                          fwhdr.ldrver & 0xffff,
                                          fwhdr.version);
    }
    else {
        cli_out("Valid firmware not found\n");
    }

error_version:
    if (SOC_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
}

STATIC cmd_result_t
_pcie_phy_fw_set_access_method(int unit, uint8 access_method, uint32 i2c_bus, uint32 i2c_dev)
{
    uint8 bus = 0, dev = 0;
    if (access_method == QSPI_ACCESS_METHOD_PCIE) {
#ifdef INCLUDE_CPU_I2C
    } else if(access_method == QSPI_ACCESS_METHOD_I2C) {
        bus = i2c_bus, dev = i2c_dev;
    }
    else if (access_method == QSPI_ACCESS_METHOD_I2C_BDE) {
#endif /* INCLUDE_CPU_I2C */
    } else {
        cli_out("The specified access mode in not supported\n");
        return CMD_USAGE;
    }
    iproc_qspi_set_access_method(unit, access_method, bus, dev);
    return CMD_OK;
}

STATIC cmd_result_t
_pcie_phy_fw_access(int unit, args_t *args)
{
    int rv = CMD_FAIL;
    uint32 address, value = 0;
    char *cmd_str = ARG_GET(args), *c;

    /* Parse sub-command */
    if (cmd_str == NULL) {
        return CMD_USAGE;
    } else if (sal_strcasecmp(cmd_str, "pcie") == 0) {
        rv = _pcie_phy_fw_set_access_method(unit, IPROC_ACCESS_METHOD_PCIE, 0, 0);
    } else if (sal_strcasecmp(cmd_str, "i2c_bde") == 0) {
        rv = _pcie_phy_fw_set_access_method(unit, IPROC_ACCESS_METHOD_I2C_BDE, 0, 0);
    } else if (sal_strcasecmp(cmd_str, "i2c") == 0) {
        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0); /* I2C bus number */
        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        value = sal_ctoi(c, 0); /* I2C slave device number */
        rv = _pcie_phy_fw_set_access_method(unit, IPROC_ACCESS_METHOD_I2C, address, value);
    } else if (sal_strcasecmp(cmd_str, "read") == 0) {
        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0); /* AXI address */
        if ((rv = IPROC_ACCESS_READ(unit, address, &value)) == SOC_E_NONE) {
            cli_out("The 4 bytes read from address 0x%x are 0x%x\n", address, value);
        } else {
            cli_out("Failed to read from address 0x%x\n", address);
        }
    } else if (sal_strcasecmp(cmd_str, "write") == 0) {
        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0); /* AXI address */
        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        value = sal_ctoi(c, 0); /* value to write */
        if ((rv = IPROC_ACCESS_WRITE(unit, address, value)) == SOC_E_NONE) {
            cli_out("Wrote 4 bytes read to address 0x%x with the value 0x%x\n", address, value);
        } else {
            cli_out("Failed to write to address 0x%x\n", address);
        }
    } else if (sal_strcasecmp(cmd_str, "testperf") == 0) {
#define PCIEFW_MEASURE_INIT_TEST total = 0; min = -1; max = 0
#define PCIEFW_MEASURE_OPERATION_START diff = sal_time_usecs()
#define PCIEFW_MEASURE_OPERATION_END \
    diff = sal_time_usecs() - diff; \
    total += diff; \
    if (max < diff) max = diff; \
    if (min > diff) min = diff
#define PCIEFW_MEASURE_END_TEST(testname, bits) \
    cli_out("Test %s results: min/average/max us: %u/%u/%u %uHz\n", #testname, min, \
      (total + nof_iters / 2) / nof_iters, max, total >= 100 ? nof_iters * 10000 * (bits) / (total / 100) : 0)

        unsigned i;
        uint32 value, nof_iters;
        sal_usecs_t total, diff, min, max;

        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0); /* AXI address to use for testing performance */

        if ((c = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        nof_iters = sal_ctoi(c, 0); /* number of test iterations */

        PCIEFW_MEASURE_INIT_TEST;
        for (i = nof_iters; i; --i) {
            PCIEFW_MEASURE_OPERATION_START;
            rv = IPROC_ACCESS_READ(unit, address, &value);
            PCIEFW_MEASURE_OPERATION_END;
            if (rv != SOC_E_NONE) {
                cli_out("Test read number %u failed, from address 0x%x\n", nof_iters + 1 - i, value);
                return CMD_FAIL;
            }
        }
        PCIEFW_MEASURE_END_TEST("read AXI word", 9 * 8 + 2);

        PCIEFW_MEASURE_INIT_TEST;
        for (i = nof_iters; i; --i) {
            PCIEFW_MEASURE_OPERATION_START;
            rv = IPROC_ACCESS_WRITE(unit, address, value);
            PCIEFW_MEASURE_OPERATION_END;
            if (rv != SOC_E_NONE) {
                cli_out("Test write number %u failed, from address 0x%x\n", nof_iters + 1 - i, value);
                return CMD_FAIL;
            }
        }
        PCIEFW_MEASURE_END_TEST("write AXI word", 9 * 8 + 2);

        return CMD_OK;
    } else {
        return CMD_USAGE;
    }

    return rv;
}

STATIC cmd_result_t
_pcie_phy_fw(int unit, args_t *args)
{
    int rv = CMD_OK;
    char *cmd_str;

    /* Parse command */
    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(cmd_str, "load") == 0) {
        rv = _pcie_phy_fw_load(unit, args);
    } else if (sal_strcasecmp(cmd_str, "version") == 0) {
        rv = _pcie_phy_fw_version(unit);
    } else if (sal_strcasecmp(cmd_str, "access") == 0) {
        rv = _pcie_phy_fw_access(unit, args);
    }
    else {
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
_pcie_serdes_pram_read(int unit, args_t *args)
{
    char *c;
    uint32_t address;
    uint32_t size;
    srds_access_t sa;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    size = sal_ctoi(c, 0);

    sa.unit = unit;
    sa.bus = &pcie_srds_bus;

    (void)pcie_phy_diag_pram_read(&sa, address, size);

    return CMD_OK;
}

STATIC cmd_result_t
_pcie_get_reg(int unit, args_t *args)
{
    char *c;
    uint16_t address;
    uint32_t val;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    val= 0;
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, address);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &val);
    cli_out("\nPCIe getepreg: address: 0x%x,  data = 0x%x\n", address, val);

    return CMD_OK;
}

STATIC cmd_result_t
_pcie_set_reg(int unit, args_t *args)
{
    char *c;
    uint16_t address;
    uint32_t val;
    uint32_t data;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    val= 0;

    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, address);
    sal_udelay(1000);
    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    data = sal_ctoi(c, 0);
    WRITE_PAXB_0_CONFIG_IND_DATAr(unit, data);
    sal_udelay(1000);

    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, address);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &val);
    cli_out("\nPCIe setepreg: address: 0x%x,  data = 0x%x\n", address, val);

    return CMD_OK;
}

/* Arguments: address, field data, field mask, field bit shift(left) */
STATIC cmd_result_t
_pcie_rmw_reg(int unit, args_t *args)
{
    char *c;
    uint16_t address, fdata, fmask, fshift;
    uint32_t iaddr, data;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    fdata = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    fmask = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    fshift = sal_ctoi(c, 0);

    iaddr = address;
    /* Read register */
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, iaddr);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &data);

    /* modify data */
    data &= fmask << fshift;
    data |= (fdata & fmask) << fshift;

    /* Write data */
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, iaddr);
    sal_udelay(1000);
    WRITE_PAXB_0_CONFIG_IND_DATAr(unit, data);

    /* Read back data */
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, iaddr);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &data);

    cli_out("\nPCIe modepreg: address: 0x%x,  data = 0x%x\n", address, data);

    return CMD_OK;
}

/*
 * Diagnostic utilities for serdes and PHY devices.
 *
 * Command format used in BCM diag shell:
 * phy diag <pbm> <sub_cmd> [sub cmd parameters]
 * The list of sub commands:
 *   dsc - display tx/rx equalization information.
 *   dsc - display Diag information
 *            BCM.0> pciephy diag <lane bit map> dsc
 *            BCM.0> pciephy diag <lane bit map> state
 *            BCM.0> pciephy diag <lane bit map> eyescan
*/
STATIC cmd_result_t
_pcie_phy_diag(int unit, args_t *args)
{
    int rv;
    char *cmd_str, *lane_str;
    soc_pbmp_t pbmp;
    soc_port_t port;
    srds_access_t *sa, *sa_l = NULL;
    int core = 0, prev_core = -1;
    int lane = 0, count = 0;

    /* Parse lane bitmap */
    if ((lane_str= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    (void)parse_pbmp(unit, lane_str, &pbmp);

    /* Parse command */
    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    sa = sal_alloc(sizeof(srds_access_t) * PCIE_MAX_CORES, "access array");
    if (NULL == sa) {
        cli_out("_pcie_phy_diag: memory allocation failed/n");
        return CMD_FAIL;
    }

    SOC_PBMP_ITER(pbmp, port) {
        core = port / 4;
        lane = port % 4;
        if (core != prev_core) {
            prev_core = core;
            if(sa_l == NULL) {
                sa_l = sa;
            }
            else {
                sa_l++;
            }
            sa_l->unit = unit;
            sa_l->core = core;
            sa_l->lane_mask = 0;
            sa_l->bus = &pcie_srds_bus;
            count++;
        }
        if (sa_l != NULL) {
            sa_l->lane_mask |= 0x1 << lane;
        }
    }

    if (sal_strcasecmp(cmd_str, "dsc") == 0) {
        rv = pcie_phy_diag_dsc(sa, count);
        if (rv != SRDS_E_NONE) {
            cli_out("pcie_phy_diag_dsc() failed, Error: %d\n", rv);
        }
    } else if (sal_strcasecmp(cmd_str, "eyescan") == 0) {
        rv = pcie_phy_diag_eyescan(sa, count);
        if (rv != SRDS_E_NONE) {
            cli_out("pcie_phy_diag_eyescan() failed, Error: %d\n", rv);
        }
    } else if (sal_strcasecmp(cmd_str, "state") == 0) {
        rv = pcie_phy_diag_state(sa, count);
        if (rv != SRDS_E_NONE) {
            cli_out("pcie_phy_diag_state() failed, Error: %d\n", rv);
        }
    } else {
        sal_free(sa);
        return CMD_FAIL;
    }

    sal_free(sa);
    return CMD_OK;
}


char pciephy_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "pciephy <sub_cmd> [sub cmd parameters]\n\t"
    "Default: None\n"
    "Subcommands: diag, fw\n";
#else
    "Parameters: <sub_cmd> [sub cmd parameters]\n\t"
    "The list of sub commands:\n\t"
    "   getreg - displays value of given phy register\n\t"
    "          Syntax: pciephy getreg <PHY register address>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy getreg 0xd230\n\t"
    "   setreg - writes given value into given phy register\n\t"
    "          Syntax: pciephy setreg <PHY register address> <value>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy setreg 0xd230 0xa555\n\t"
    "   getepreg - displays value of given pcie core(endpoint) register\n\t"
    "          Syntax: pciephy getepreg <pcie core register address>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy getepreg 0xbc\n\t"
    "   setepreg - writes given value into given pcie core(endpoint) register\n\t"
    "          Syntax: pciephy setepreg <pcie core register address> <value>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy setepreg 0xbc 0x12345678\n\t"
    "   pramread - Reads and displays pcie phy pram content at given address\n\t"
    "          Syntax: pciephy pramread <pram address offset> <number of 16-bit words to read>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy pramread 0x0 0x20\n\t"
    "   diag - display SerDes lane information.\n\t"
    "          Syntax: pciephy diag <lane bit map> <diag sub-command>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy diag 0x1 dsc\n\t"
    "            BCM.0> pciephy diag 0x1 state\n\t"
    "            BCM.0> pciephy diag 0x1 eyescan\n\t"
    "          diag - sub commands:\n\t"
    "            dsc - Display core state and lane state\n\t"
    "            state - Discplay core state, lane state for all lanes and event log\n\t"
    "            eyescan - Display eyescan for selected lane\n\t"
    "   fw - Load SerDes(PCIe Gen3) firmware, display firmware version\n\t"
    "        Syntax: pciephy fw <fw sub-command> [fw sub cmd parameters]\n\t"
    "        Example:\n\t"
    "          BCM.0> pciephy fw load <firmware file>\n\t"
    "        fw - sub commands:\n\t"
    "          load - programs contents of given firmware file to QSPI flash\n\t"
    "          version - displays PCIe Gen3 fw loader and SerDes fw versions\n\t"
    "          access - sets the method used to access the firmware or tests it.  sub commands:\n\t"
    "            pcie - (default) Use the device's PCIe connection for access\n\t"
    "            i2c_bde - Use the device's BDE I2C functions for access\n\t"
    "            i2c <I2C bus number> <slave device ID> - Use I2C using the give bus and device IDs\n\t"
    "            read <AXI address> - read four bytes using the current access method\n\t"
    "            write <AXI address> <value> - write four bytes using the current access method\n"
    ;
#endif

/*
 * Function:    cmd_pciephy
 * Purpose:     Show/configure PCIe phy registers.
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns:     CMD_OK/CMD_FAIL/
 */
cmd_result_t cmd_pciephy(int u, args_t *a)
{
    char *c;
    uint16_t address, data;
    srds_access_t sa;
    int rv = 0;

    if (SOC_IS_HELIX5(u) && (soc_cm_get_bus_type(u) & SOC_AXI_DEV_TYPE)) {
        /* HX5 supports PCIe EP mode only and PCIe diag support is not 
           required when running on internal processor */
        cli_out("pciephy command NOT supported on this device\n");
        ARG_DISCARD(a);
        return CMD_OK;
    }

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    c = ARG_GET(a);

    if (c != NULL && sal_strcasecmp(c, "fw") == 0) {
        return _pcie_phy_fw(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "diag") == 0) {
        return _pcie_phy_diag(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "getreg") == 0) {
        if ((c= ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0);
        sa.unit = u;
        sa.bus = &pcie_srds_bus;
        rv = pcie_phy_diag_reg_read(&sa, address, &data);
        if(rv != 0) {
            cli_out("pcie_phy_diag_reg_read failed: %d\n", rv);
        }
        else {
            cli_out("\naddess: 0x%x,  data = 0x%x\n", address, data);
        }
        return CMD_OK;
    }
    if (c != NULL && sal_strcasecmp(c, "setreg") == 0) {
        sa.unit = u;
        sa.bus = &pcie_srds_bus;
        if ((c= ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0);
        if ((c= ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        data = sal_ctoi(c, 0);
        rv = pcie_phy_diag_reg_write(&sa, address, data);
        if(rv != 0) {
            cli_out("ERROR: pcie_diag_reg_write failed: %d\n", rv);
        }
        else {
            cli_out("\naddess: 0x%x,  data = 0x%x\n", address, data);
        }
        return CMD_OK;
    }
    if (c != NULL && sal_strcasecmp(c, "getepreg") == 0) {
        return _pcie_get_reg(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "setepreg") == 0) {
        return _pcie_set_reg(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "modepreg") == 0) {
        return _pcie_rmw_reg(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "pramread") == 0) {
        return _pcie_serdes_pram_read(u, a);
    }
    if (c == NULL) {
        return (CMD_USAGE);
    }
    else {
        cli_out("Command NOT supported\n");
    }
    return CMD_OK;
}

#else
/* To avoid empty file warning in not supported architectures */
int appl_diag_diag_anchor;
#endif /* #if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_ESW_SUPPORT) */

