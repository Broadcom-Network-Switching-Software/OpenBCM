/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     QSPI Flash Driver 
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/dpc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/soc_flash.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx_qspi.h>
#endif
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
#include <sal/core/time.h>
#endif

/* Common commands */
#define CMD_READ_ID             0x9f

#define CMD_READ_ARRAY_SLOW     0x03
#define CMD_READ_ARRAY_FAST     0x0b

#define CMD_WRITE_STATUS        0x01
#define CMD_PAGE_PROGRAM        0x02
#define CMD_WRITE_DISABLE       0x04
#define CMD_READ_STATUS         0x05
#define CMD_WRITE_ENABLE        0x06
#define CMD_ERASE_4K            0x20
#define CMD_ERASE_32K           0x52
#define CMD_ERASE_64K           0xd8
#define CMD_ERASE_CHIP          0xc7
#define CMD_READ_FLAG_STATUS    0x70
#define CMD_READ_FLASH_DISCOVERY    0x5A
#define CMD_READ_CONFIG         0x35

#define SPI_FLASH_PROG_TIMEOUT          (2 * 200U * 1000)
#define SPI_FLASH_PAGE_ERASE_TIMEOUT    (5 * 200U * 1000)
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT  (10 * 200U * 1000)

#define SPI_FLASH_ID_CYPRESS   (0x01)
#define SPI_FLASH_ID_MICRON    (0x20)
#define SPI_FLASH_ID_ISSI      (0x9d)
#define SPI_FLASH_ID_MACRONIX  (0xc2)
#define SPI_FLASH_ID_WINBOND   (0xef)

/* SPI transfer flags */
#define SPI_RW_BEGIN 0x01    /* Assert CS before transfer */
#define SPI_RW_END   0x02    /* Deassert CS after transfer */


/* Common status */
#define STATUS_WIP              0x01
#ifndef __KERNEL__
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#define ID_LEN 11


/* Configuration register */
#define CYPRESS_CFG_TBPARM   (1 << 2)

typedef struct soc_flash_s {
        uint8            init;
        soc_flash_conf_t conf;
        int (*qspi_init)(int unit, uint32 max_hz, uint32 mode);
        int (*qspi_cleanup)(int unit);
        int (*qspi_claim_bus)(int unit);
        int (*qspi_release_bus)(int unit);
        int (*qspi_rw)(int unit, uint32 len, const void *dout,
                            void *din, uint32 flags);
} soc_flash_t;

/* soc property values for flash chip parameters */
#define SOC_PROP_MANF_ID_VALUE_INDEX 0
#define SOC_PROP_SECT_SIZE_VALUE_INDEX 1
#define SOC_PROP_PAGE_SIZE_VALUE_INDEX 2
#define SOC_PROP_NOF_VALUES 3
/* User provided flash chip parameters */
typedef struct soc_flash_user_params_s {
        uint8      manf_id;
        uint32     sector_size;
        uint32     page_size;
} soc_flash_user_params_t;

static const soc_flash_conf_t flash_table[] = {
    /* id                    page_size  sector    sector  nr_sectors  size
                             size       size      type                      */
    /* Micron MT25QL128/MT25QL256 family */
    { SPI_FLASH_ID_MICRON,   0x100,     0x10000,   0,       0x1000,    0   },
    /* Cypress/Spansion S25FL128S/S25FL256S family */
    { SPI_FLASH_ID_CYPRESS,  0x100,     0x1000,    0,       0x1000,    0   },
    /* Macronix  MX25L256/MX25L512 family */
    { SPI_FLASH_ID_MACRONIX, 0x100,     0x1000,    0,       0x2000,    0   },
    /* Winbond 25Q256FV */
    { SPI_FLASH_ID_WINBOND,  0x100,     0x10000,   0,       0x1000,    0   },
    /* ISSI IS25LP256D/IS25WP256D family */
    { SPI_FLASH_ID_ISSI,     0x100,     0x1000,    0,       0x10000,   0   },
};

STATIC soc_flash_t soc_flash[SOC_MAX_NUM_DEVICES];

static void
soc_flash_addr(uint32 addr, uint8 *cmd)
{
    /* cmd[0] is actual command */
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = (addr) & 0xFF;
}

static int
soc_flash_read_write(int unit,
                     const uint8 *cmd,
                     size_t cmd_len,
                     const uint8 *data_out,
                     uint8 *data_in,
                     size_t data_len)
{
    soc_flash_t *flash = &soc_flash[unit];
    uint32 flags = SPI_RW_BEGIN;
    int rv = SOC_E_NONE;
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    sal_usecs_t diff;
#define PCIEFW_MEASURE_START diff = sal_time_usecs();
#define PCIEFW_MEASURE_END(testname) \
    diff = sal_time_usecs() - diff; \
    cli_out("  %s: %uus\n", #testname, diff);
#else
#define PCIEFW_MEASURE_START
#define PCIEFW_MEASURE_END(testname)
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */

    if (data_len == 0) {
        flags |= SPI_RW_END;
    }

    PCIEFW_MEASURE_START;
    rv = flash->qspi_rw(unit, cmd_len, cmd, NULL, flags);
    PCIEFW_MEASURE_END(soc_flash_read_write_cmd);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Error to send command : %d\n"), rv));
    } else if (data_len != 0) {
        PCIEFW_MEASURE_START;
        rv = flash->qspi_rw(unit, data_len, data_out,
                       data_in, SPI_RW_END);
        PCIEFW_MEASURE_END(soc_flash_read_write_data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Error to transfer data: %d"), rv));
        }
    }

    return rv;
}

static int
soc_flash_cmd_read(int unit,
                   const uint8 *cmd,
                   size_t cmd_len,
                   void *data,
                   size_t data_len)
{
    int rv = SOC_E_NONE;

    rv = soc_flash_read_write(unit, cmd, cmd_len, NULL, data, data_len);

    return rv;
}

static int
soc_flash_cmd(int unit,
              uint8 cmd,
              void *response,
              size_t len)
{
    int rv = SOC_E_NONE;

    rv = soc_flash_cmd_read(unit, &cmd, 1, response, len);

    return rv;
}

static int
soc_flash_wait_cmd_complete(int unit, uint32 timeout)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint8 status;
    uint8 cmd;
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    sal_usecs_t diff;
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */

    cmd = CMD_READ_STATUS;

    soc_timeout_init(&to, timeout, 1);
    while(1) {
        PCIEFW_MEASURE_START;
        rv = soc_flash_cmd_read(unit, &cmd, sizeof(cmd), &status, 1);
        PCIEFW_MEASURE_END(soc_flash_wait_cmd_complete_read);
        if (SOC_FAILURE(rv)) {
            break;
        }

        if ((status & STATUS_WIP) == 0){
            break;
        }
        if (soc_timeout_check(&to)) {
            /* Timed out */
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Time out:\n")));
            rv = SOC_E_TIMEOUT;
            break;
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Status Register = 0x%x\n"), status));
    return rv;
}

static int
soc_flash_cmd_write(int unit,
                    const uint8 *cmd,
                    size_t cmd_len,
                    const void *data,
                    size_t data_len,
                    uint32 timeout)
{
    int rv = SOC_E_NONE;

    rv =  soc_flash_read_write(unit, cmd, cmd_len, data, NULL, data_len);
    if (SOC_SUCCESS(rv)) {
        rv = soc_flash_wait_cmd_complete(unit, timeout);
    }

    return rv;
}

/*
 * Enable writing on the SPI flash.
 */
static int 
soc_flash_cmd_write_enable(int unit)
{
    int rv = SOC_E_NONE;

    rv = soc_flash_cmd(unit, CMD_WRITE_ENABLE, NULL, 0);

    return rv;
}
#if 0
/*
 * Disable writing on the SPI flash.
 */
static int
soc_flash_cmd_write_disable(int unit)
{
    int rv = SOC_E_NONE;

    rv = soc_flash_cmd(unit, CMD_WRITE_DISABLE, NULL, 0);

    return rv;
}
#endif

static int
soc_flash_read_common(int unit,
                      const uint8 *cmd,
                      size_t cmd_len,
                      void *data,
                      size_t data_len)
{
    int rv = SOC_E_NONE;
    soc_flash_t *flash = &soc_flash[unit];

    rv = flash->qspi_claim_bus(unit);
    if (SOC_SUCCESS(rv)) {
        rv = soc_flash_cmd_read(unit, cmd, cmd_len, data, data_len);
        rv = flash->qspi_release_bus(unit);
    }

return rv;
}

int
soc_flash_cmd_write_status(int unit, uint8 sr)
{
    uint8 cmd;
    int rv = SOC_E_NONE;

    rv = soc_flash_cmd_write_enable(unit);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "SF: enabling write failed\n")));
        return rv;
    }

    cmd = CMD_WRITE_STATUS;
    rv = soc_flash_cmd_write(unit, &cmd, 1, &sr, 1, SPI_FLASH_PROG_TIMEOUT);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "SF: fail to write status register\n")));
        return rv;
    }

    return 0;
}

static int
qspi_init(int unit, uint32 max_hz, uint32 mode)
{
    return SOC_E_UNAVAIL;
}
static int
qspi_cleanup(int unit)
{
    return SOC_E_UNAVAIL;
}
static int
qspi_claim_bus(int unit)
{
    return SOC_E_UNAVAIL;
}
static int
qspi_release_bus(int unit)
{
    return SOC_E_UNAVAIL;
}
static int
qspi_rw(int unit, uint32 len, const void *dout, void *din, uint32 flags)
{
    return SOC_E_UNAVAIL;
}

/* read the flash chip user parameters from soc properties */
static int
soc_flsah_params_soc_prop_get(int unit, soc_flash_user_params_t *params)
{
    int vals[SOC_PROP_NOF_VALUES];

    if (soc_property_get_csv(unit, spn_PCIE_FLASH_MEM_PARAMS, SOC_PROP_NOF_VALUES, vals) != SOC_PROP_NOF_VALUES) {
        return SOC_E_UNAVAIL;
    } else if (vals[SOC_PROP_MANF_ID_VALUE_INDEX] < 0 || (vals[SOC_PROP_MANF_ID_VALUE_INDEX] > 255) ||
      vals[SOC_PROP_SECT_SIZE_VALUE_INDEX] < 0 || vals[SOC_PROP_PAGE_SIZE_VALUE_INDEX] < 0 ||
      vals[SOC_PROP_PAGE_SIZE_VALUE_INDEX] > vals[SOC_PROP_SECT_SIZE_VALUE_INDEX]) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Invalid flash chip parameters\n")));
        return SOC_E_PARAM;
    } else if (vals[SOC_PROP_SECT_SIZE_VALUE_INDEX] % vals[SOC_PROP_PAGE_SIZE_VALUE_INDEX]) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Warning: flash chip sector size %d is not a multiple of the page size %d\n"),
          vals[SOC_PROP_PAGE_SIZE_VALUE_INDEX], vals[SOC_PROP_SECT_SIZE_VALUE_INDEX]));
    }
    params->manf_id = vals[SOC_PROP_MANF_ID_VALUE_INDEX];
    params->sector_size = vals[SOC_PROP_SECT_SIZE_VALUE_INDEX];
    params->page_size = vals[SOC_PROP_PAGE_SIZE_VALUE_INDEX];
    return SOC_E_NONE;
}

int
soc_flash_discovery_read(int unit,
                         size_t len,
                         void *data)
{
    int rv = SOC_E_NONE;
    uint8 cmd[5];

    cmd[0] = CMD_READ_FLASH_DISCOVERY;
    soc_flash_addr(0, cmd);
    cmd[4] = 0;
    rv = soc_flash_read_common(unit, cmd, sizeof(cmd), data, len);

    return rv;
}


int
soc_flash_init(int unit,
                   uint32 max_hz,
                   uint32 mode,
                   soc_flash_conf_t *flash_conf)
{
    soc_flash_t *flash =  &soc_flash[unit];
    int i;
    uint8 id[ID_LEN];
    uint8 param[128];
    int rv = SOC_E_NONE;
    uint8 status;
    soc_flash_user_params_t flash_user_params;

    sal_memset(flash, 0 , sizeof(soc_flash_t));

    flash->qspi_init = qspi_init;
    flash->qspi_cleanup = qspi_cleanup;
    flash->qspi_rw = qspi_rw;
    flash->qspi_claim_bus = qspi_claim_bus;
    flash->qspi_release_bus = qspi_release_bus;

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        flash->qspi_init = cmicx_qspi_init;
        flash->qspi_cleanup = cmicx_qspi_cleanup;
        flash->qspi_rw = cmicx_qspi_rw;
        flash->qspi_claim_bus = cmicx_qspi_claim_bus;
        flash->qspi_release_bus = cmicx_qspi_release_bus;
    }
#endif
    rv = flash->qspi_init(unit, max_hz, mode);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Error %d in SPI bus init.\n"), rv));
        goto error_init;
    }
    rv = flash->qspi_claim_bus(unit);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Error %d in SPI bus claim.\n"), rv));
        goto err_claim_bus;
    }
    /* Read the ID codes */
    rv = soc_flash_cmd(unit, CMD_READ_ID, id, sizeof(id));
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Error %d in reading flash ID."), rv));
        goto err_read_id;
    }

    /* If the flash chip matches user supplied parameters, use them */
    if (SOC_SUCCESS(soc_flsah_params_soc_prop_get(unit, &flash_user_params)) &&
      flash_user_params.manf_id == id[0]) {
        flash->conf.id = id[0];
        flash->conf.size = 1 << id[2];
        flash->conf.page_size = flash_user_params.page_size;
        flash->conf.sector_type = SOC_FLASH_SECTOR_TYPE_UNIFORM;
        flash->conf.sector_size = flash_user_params.sector_size;
        flash->conf.nr_sectors = flash->conf.size / flash_user_params.sector_size;

    } else { /* Otherwise search the table for matches in shift and id */
        for (i = 0; i < sizeof(flash_table) / sizeof(flash_table[0]) && flash_table[i].id != id[0]; ++i);
        if (i < sizeof(flash_table) / sizeof(flash_table[0])) {
            /* we have a match */
            flash->conf.id = flash_table[i].id;
            flash->conf.size = 1 << id[2];
            if (id[0] == SPI_FLASH_ID_CYPRESS) {
                /* Cypress/Spansion */
                if (id[4]) {
                    /* Hybrid sector size, 32x4KB at top or bottom,
                       rest of them 64KB size */
                    flash->conf.sector_type = SOC_FLASH_SECTOR_TYPE_HYBRID;
                    flash->conf.sector_size = 0x10000; /* Set to 64K */
                    flash->conf.nr_sectors = flash->conf.size / 0x10000 + 30;
                    flash->conf.page_size = 256;
                }
                else {
                    /* Uniform 256KB sectors */
                    flash->conf.sector_type = SOC_FLASH_SECTOR_TYPE_UNIFORM;
                    flash->conf.sector_size = 0x40000;
                    flash->conf.nr_sectors = flash->conf.size /
                                             flash->conf.sector_size;
                    flash->conf.page_size = 512;
                }
            }
            else {
                flash->conf.page_size = flash_table[i].page_size;
                flash->conf.sector_type = SOC_FLASH_SECTOR_TYPE_UNIFORM;
                flash->conf.sector_size = flash_table[i].sector_size;
                flash->conf.nr_sectors = flash->conf.size /
                                         flash->conf.sector_size;
            }
        } else {
            rv = SOC_E_UNAVAIL;
            LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Flash id 0x%x not supported\n"), id[0]));
            goto err_read_id;
        }
    }

    LOG_INFO(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Supported Flash ID = 0x%x\n"), id[0]));

    /* Read the Discovery parameter */
    rv = soc_flash_discovery_read(unit, sizeof(param), param);
    /* Read Flag Status Register */
    rv = soc_flash_cmd(unit, CMD_READ_FLAG_STATUS, &status, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Error %d reading Flag Status\n"), rv));
        goto err_read_id;
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit, "Flag Status = 0x%x\n"), status));
    }
    flash->init = 1;
    sal_memcpy(flash_conf, &flash->conf, sizeof(soc_flash_conf_t));
    flash->qspi_release_bus(unit);

    return rv;

err_read_id:
    flash->qspi_release_bus(unit);
err_claim_bus:
    flash->qspi_cleanup(unit);
error_init:
    return rv;
}

int
soc_flash_cleanup(int unit)
{
    soc_flash_t *flash = &soc_flash[unit];
    int rv = SOC_E_NONE;

    if (!flash->init) {
        return SOC_E_INIT;
    }

    rv = flash->qspi_cleanup(unit);
    flash->init = 0;

    return rv;
}

int
soc_flash_read(int unit,
                   uint32 offset,
                   size_t len,
                   void *data)
{
    int rv = SOC_E_NONE;
    soc_flash_t *flash = &soc_flash[unit];
    uint8 cmd[5];

    if (!flash->init) {
        return SOC_E_INIT;
    }
    cmd[0] = CMD_READ_ARRAY_FAST;
    soc_flash_addr(offset, cmd);
    cmd[4] = 0;
    rv = soc_flash_read_common(unit, cmd, sizeof(cmd), data, len);

    return rv;
}

int
soc_flash_write(int unit,
                    uint32 offset,
                    size_t len,
                    const void *buf)
{
    soc_flash_t *flash =  &soc_flash[unit];
    uint32 page_size, data_offset;
    int rv = SOC_E_NONE;
    uint8 cmd[5];
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    sal_usecs_t diff;
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */

    if (!flash->init) {
        return SOC_E_INIT;
    }

    page_size = flash->conf.page_size;
    rv = flash->qspi_claim_bus(unit);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Error to claim SPI bus\n")));
    return rv;
    }

    cmd[0] = CMD_PAGE_PROGRAM;
    cmd[4] = 0;

    /* Distance from the page boundary */
    data_offset = (page_size - (offset & 0xFF) ) & 0xFF;
    if (data_offset > len) {
        data_offset = len;
    }
    /* first page if non aligned */
    if (data_offset > 0) {
        PCIEFW_MEASURE_START;
        rv = soc_flash_cmd_write_enable(unit);
        PCIEFW_MEASURE_END(soc_flash_cmd_write_enable);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Write enable failed")));
            goto error;
        }
        soc_flash_addr(offset, cmd);
        PCIEFW_MEASURE_START;
        rv = soc_flash_cmd_write(unit, cmd, 4,
                                 buf, data_offset, SPI_FLASH_PROG_TIMEOUT);
        PCIEFW_MEASURE_END(soc_flash_cmd_write);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Write failed")));
            goto error;
        }
        LOG_CLI(("\n."));
    }

    for ( ; (data_offset + page_size) < len; data_offset += page_size) {
        PCIEFW_MEASURE_START;
        rv = soc_flash_cmd_write_enable(unit);
        PCIEFW_MEASURE_END(soc_flash_cmd_write_enable);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Write enable failed")));
            goto error;
        }

        soc_flash_addr(offset + data_offset, cmd);
        rv = soc_flash_cmd_write(unit, cmd, 4, ((uint8*)buf) + data_offset,
                                 page_size, SPI_FLASH_PROG_TIMEOUT);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Write failed")));
            goto error;
        }
        LOG_CLI(("."));
    }
   /* last partial page */
    if (len > data_offset) {
        PCIEFW_MEASURE_START;
        rv = soc_flash_cmd_write_enable(unit);
        PCIEFW_MEASURE_END(soc_flash_cmd_write_enable);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                       "Write enable failed")));
            goto error;
        }

        soc_flash_addr(offset + data_offset, cmd);
        PCIEFW_MEASURE_START;
        rv = soc_flash_cmd_write(unit, cmd, 4, ((uint8*)buf) + data_offset,
                                 (len - data_offset), SPI_FLASH_PROG_TIMEOUT);
        PCIEFW_MEASURE_END(soc_flash_cmd_write);
    }
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Write failed")));
        goto error;
    }
    LOG_CLI((". Done\n"));

error:

    rv = flash->qspi_release_bus(unit);
    return rv;
}

static int
_soc_flash_erase_hybrid(int unit,
                    uint32 offset,
                    size_t len)
{
    uint32 start, end, erase_size, start_hybrid, end_hybrid;
    int rv = SOC_E_NONE;
    soc_flash_t *flash =  &soc_flash[unit];
    uint8 cmd[4], cfg;

    rv = soc_flash_cmd(unit, CMD_READ_CONFIG, &cfg, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Configuration register read failed\n")));
        return rv;
    }
    if (cfg & CYPRESS_CFG_TBPARM) {
        /* 4KB sectors at high address */
        end_hybrid = flash->conf.size;
        start_hybrid = flash->conf.size - (32 * 0x1000);
    }
    else {
        /* 4KB sectors at low address */
        end_hybrid = 32 * 0x1000;
        start_hybrid = 0;
    }

    if (offset >= start_hybrid && offset < end_hybrid) {
        erase_size = 0x1000; /* 4KB */
    }
    else {
        erase_size = flash->conf.sector_size;
    }
    if ((offset > 0) && (offset % erase_size)){
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Erase offset is not sector boudary\n")));
        return SOC_E_PARAM;
    }

    rv = flash->qspi_claim_bus(unit);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Unable to claim SPI bus\n")));
        return rv;
    }

    start = offset;
    end = start + len;

    while (offset < end) {
        if (offset >= start_hybrid && offset < end_hybrid) {
            erase_size = 0x1000; /* 4KB */
            cmd[0] = CMD_ERASE_4K;
        }
        else {
            erase_size = flash->conf.sector_size;
            cmd[0] = CMD_ERASE_64K;
        }
        soc_flash_addr(offset, cmd);
        offset += erase_size;
        rv = soc_flash_cmd_write_enable(unit);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
        rv = soc_flash_cmd_write(unit, cmd, sizeof(cmd),
                                  NULL, 0, SPI_FLASH_PAGE_ERASE_TIMEOUT);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }
error:
    flash->qspi_release_bus(unit);
    return rv;
}

int
soc_flash_erase(int unit,
                    uint32 offset,
                    size_t len)
{
    uint32 start, end, erase_size;
    int rv = SOC_E_NONE;
    soc_flash_t *flash =  &soc_flash[unit];
    uint8 cmd[4];

    if (!flash->init) {
        return SOC_E_INIT;
    }

    if (flash->conf.sector_type == SOC_FLASH_SECTOR_TYPE_HYBRID) {
        return(_soc_flash_erase_hybrid(unit, offset, len));
    }
    erase_size = flash->conf.sector_size;
    if ((offset > 0) && (offset % erase_size)){
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Erase offset is not sector boundary\n")));
        return SOC_E_PARAM;
    }
    /* make erase length at sector boundary */
    len = (len % erase_size) ? (len / erase_size + 1) : (len / erase_size);
    len *= erase_size;

    rv = flash->qspi_claim_bus(unit);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                   "Unable to claim SPI bus\n")));
        return rv;
    }
    if (erase_size == 4096) {
        cmd[0] = CMD_ERASE_4K;
    } else {
        cmd[0] = CMD_ERASE_64K;
    }
    start = offset;
    end = start + len;

    while (offset < end) {
        soc_flash_addr(offset, cmd);
        offset += erase_size;
        rv = soc_flash_cmd_write_enable(unit);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
        rv = soc_flash_cmd_write(unit, cmd, sizeof(cmd),
                                  NULL, 0, SPI_FLASH_PAGE_ERASE_TIMEOUT);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }
error:
    flash->qspi_release_bus(unit);
    return rv;
}

