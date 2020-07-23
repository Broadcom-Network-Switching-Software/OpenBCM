/*! \file bcma_flash.c
 *
 * QSPI flash driver for MT25QL128/MT25QL256 and compatible parts.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bsl/bsl.h>

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_qspi.h>

#include <bcma/flash/bcma_flash.h>


/*******************************************************************************
 * Local definitions
 */

/*! BSL layer source. */
#define BSL_LOG_MODULE BSL_LS_APPL_QSPI

/*! Write enable. */
#define BCMA_QSPI_CMD_WREN        0x06

/*! Write disable. */
#define BCMA_QSPI_CMD_WRDI        0x04

/*! Write status register. */
#define BCMA_QSPI_CMD_WRSR        0x01

/*! Read status register. */
#define BCMA_QSPI_CMD_RDSR        0x05

/*! Read flag status register. */
#define BCMA_QSPI_CMD_RDFSR       0x70

/*! Read. */
#define BCMA_QSPI_CMD_READ        0x03

/*! Fast read. */
#define BCMA_QSPI_CMD_FAST_READ   0x0b

/*! Read manufacturer and product ID. */
#define BCMA_QSPI_CMD_RDID        0x9f

/*! Sector Erase. */
#define BCMA_QSPI_CMD_SE          0xd8

/*! Subsector Erase 4KB. */
#define BCMA_QSPI_CMD_SE_4K       0x20

/*! Bulk (chip) erase. */
#define BCMA_QSPI_CMD_BE          0xc7

/*! Page Program. */
#define BCMA_QSPI_CMD_PP          0x02

/*! Command opcode (instruction) length in bytes. */
#define BCMA_QSPI_CMD_OPCODE_LEN 1

/*! Command address length in bytes. */
#define BCMA_QSPI_CMD_ADDR_LEN   3

/*! Data length to read for RDID command. */
#define BCMA_QSPI_RDID_DATA_LEN  11

/*! Write-in-precess bit offset in status register. */
#define BCMA_QSPI_SR_WIP_BIT (1 << 0)

/*! Write-enabled bit offset in status register. */
#define BCMA_QSPI_SR_WEN_BIT (1 << 1)

/*! Cypress specific command for read configuration register. */
#define BCMA_CYPRESS_CMD_RCFG 0x35

/*! TBPARM bit offset in Cypress specific configuration register. */
#define BCMA_CYPRESS_CFG_TBPARM_BIT (1 << 2)

/*! Timeout for page program process. */
#define BCMA_QSPI_PP_TIMEOUT 400000

/*! Timeout for sector erase process. */
#define BCMA_QSPI_SE_TIMEOUT 1000000

/*! Adjust the length for sector alignment. */
#define BCMA_QSPI_SECTOR_ALIGN_LEN(_len, _sec_sz) \
        ((((_len) + (_sec_sz - 1)) / _sec_sz) * _sec_sz)


/*******************************************************************************
 * Builtin device list
 */

#define BCMA_FLASH_ID_CYPRESS   0x01
#define BCMA_FLASH_ID_MICRON    0x20
#define SPI_FLASH_ID_ISSI       0x9d
#define BCMA_FLASH_ID_MACRONIX  0xc2
#define BCMA_FLASH_ID_WINBOND   0xef

#define BCMA_FLASH_SECTOR_TYPE_UNIFORM 0
#define BCMA_FLASH_SECTOR_TYPE_HYBRID 1

#define BCMA_VAL_4K     0x1000
#define BCMA_VAL_8K     0x2000
#define BCMA_VAL_64K    0x10000
#define BCMA_VAL_256K   0x40000

static bcma_flash_dev_t flash_dev_table[] = {
    /* id, page_size, sector_size, num_sectors, sector_type, size */

    /* Micron MT25QL128/MT25QL256 family */
    { BCMA_FLASH_ID_MICRON, 256, BCMA_VAL_64K, BCMA_VAL_4K, 0, 0 },

    /* Cypress/Spansion S25FL128S/S25FL256S family */
    { BCMA_FLASH_ID_CYPRESS, 256, BCMA_VAL_4K, BCMA_VAL_4K, 0, 0 },

    /* Macronix MX25L256/MX25L512 family */
    { BCMA_FLASH_ID_MACRONIX, 256, BCMA_VAL_4K, BCMA_VAL_8K, 0, 0 },

    /* Winbond 25Q256FV */
    { BCMA_FLASH_ID_WINBOND, 256, BCMA_VAL_64K, BCMA_VAL_4K, 0, 0 },

    /* ISSI IS25LP256D/IS25WP256D family */
    { SPI_FLASH_ID_ISSI, 256, BCMA_VAL_4K, BCMA_VAL_64K, 0, 0},
};

static bcma_flash_dev_t custom_flash_dev[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

/*!
 * Set SPI command bytes.
 *
 * \param [in] opcode SPI instruction.
 * \param [in] addr Address.
 * \param [in] dummy_bytes Dummy bytes.
 * \param [in] len Length of command buffer.
 * \param [out] buf Command buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL The command buffer is to small.
 */
static int
qspi_flash_cmd_addr_set(uint8_t opcode, uint32_t addr, uint32_t dummy_bytes,
                        size_t len, uint8_t *buf)
{
    uint8_t idx, *cmd = buf;

    if (len < BCMA_QSPI_CMD_OPCODE_LEN + BCMA_QSPI_CMD_ADDR_LEN + dummy_bytes) {
        return SHR_E_INTERNAL;
    }

    *cmd = opcode;
    cmd++;
    *cmd = (addr >> 16) & 0xff;
    cmd++;
    *cmd = (addr >> 8) & 0xff;
    cmd++;
    *cmd = (addr) & 0xff;
    cmd++;
    for (idx = 0; idx < dummy_bytes; idx++) {
        *cmd = 0;
        cmd++;
    }

    return SHR_E_NONE;
}

static int
qspi_flash_write_enable(int unit, bool en)
{
    uint8_t cmd;

    SHR_FUNC_ENTER(unit);

    cmd = en ? BCMA_QSPI_CMD_WREN : BCMA_QSPI_CMD_WRDI;

    SHR_IF_ERR_EXIT
        (bcmbd_qspi_transfer_one(unit, sizeof(cmd), &cmd, 0, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
qspi_flash_write_wait(int unit, uint32_t to_us)
{
    shr_timeout_t to;
    uint8_t cmd, st;

    SHR_FUNC_ENTER(unit);

    cmd = BCMA_QSPI_CMD_RDSR;

    shr_timeout_init(&to, to_us, 1);
    do {
        SHR_IF_ERR_EXIT
            (bcmbd_qspi_transfer_one(unit, sizeof(cmd), &cmd, sizeof(st), &st));

        if (shr_timeout_check(&to)) {
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    } while (st & BCMA_QSPI_SR_WIP_BIT);

exit:
    SHR_FUNC_EXIT();
}

static int
qspi_flash_probe(int unit, bcma_flash_dev_t *fdev)
{
    uint8_t cmd, idx;
    uint8_t id[BCMA_QSPI_RDID_DATA_LEN];

    SHR_FUNC_ENTER(unit);

    /* Read flash ID. */
    cmd = BCMA_QSPI_CMD_RDID;
    SHR_IF_ERR_EXIT
        (bcmbd_qspi_transfer_one(unit, sizeof(cmd), &cmd, sizeof(id), id));

    if (id[0] == custom_flash_dev[unit].id) {
        sal_memcpy(fdev, &custom_flash_dev[unit], sizeof(*fdev));
        SHR_EXIT();
    }

    /* Check the builtin flash device list. */
    for (idx = 0; idx < COUNTOF(flash_dev_table); idx++) {
        if (id[0] == flash_dev_table[idx].id) {
            sal_memcpy(fdev, &flash_dev_table[idx], sizeof(*fdev));
            break;
        }
    }

    /* Return error if the flash is not supported. */
    if (idx == COUNTOF(flash_dev_table)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Complete the flash properties based on the response of command RDID. */
    fdev->size = 1 << id[2];
    if (id[0] == BCMA_FLASH_ID_CYPRESS) {
        /* Cypress/Spansion */
        if (id[4]) {
            /*
             * Hybrid sector size.
             * 4 KB * 32 sectors at top or bottom, rest of them are 64KB per
             * sector.
             */
            fdev->sector_type = BCMA_FLASH_SECTOR_TYPE_HYBRID;
            fdev->sector_size = BCMA_VAL_64K;
            fdev->num_sectors = fdev->size / BCMA_VAL_64K + 30;
            fdev->page_size = 256;
        } else {
            /* Uniform 256KB sectors */
            fdev->sector_type = BCMA_FLASH_SECTOR_TYPE_UNIFORM;
            fdev->sector_size = BCMA_VAL_256K;
            fdev->num_sectors = fdev->size / fdev->sector_size;
            fdev->page_size = 512;
        }
    } else {
        fdev->sector_type = BCMA_FLASH_SECTOR_TYPE_UNIFORM;
        fdev->num_sectors = fdev->size / fdev->sector_size;
    }

exit:
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Flash info: ID = 0x%x, page size = 0x%x, "
                                "sector size = 0x%x, size = 0x%x\n"),
                    fdev->id, fdev->page_size, fdev->sector_size, fdev->size));
    }

    SHR_FUNC_EXIT();
}

static int
qspi_flash_page_write(int unit, uint32_t addr, size_t len, uint8_t *data)
{
    uint8_t cmd[4];

    SHR_FUNC_ENTER(unit);

    /* Build the command bytes */
    SHR_IF_ERR_EXIT
        (qspi_flash_cmd_addr_set(BCMA_QSPI_CMD_PP, addr, 0,
                                 sizeof(cmd), cmd));

    /* Write enable before each PROGRAM command */
    SHR_IF_ERR_EXIT
        (qspi_flash_write_enable(unit, true));

    /* Send out the command bytes */
    SHR_IF_ERR_EXIT
        (bcmbd_qspi_transfer(unit, BCMBD_QSPI_TRANS_BEGIN,
                                 sizeof(cmd), cmd, 0, NULL));

    /* Send out the data bytes */
    SHR_IF_ERR_EXIT
        (bcmbd_qspi_transfer(unit, BCMBD_QSPI_TRANS_END,
                             len, data, 0, NULL));

    /* Wait command complete */
    SHR_IF_ERR_EXIT
        (qspi_flash_write_wait(unit, BCMA_QSPI_PP_TIMEOUT));

exit:
    SHR_FUNC_EXIT();
}

static int
qspi_flash_erase_uniform(int unit, bcma_flash_dev_t *fdev,
                         uint32_t offset, size_t len)
{
    uint8_t opcode, cmd[4];
    uint32_t addr, end;

    SHR_FUNC_ENTER(unit);

    if (offset % fdev->sector_size) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Erase offset is not sector boudary.\n")));
    }

    if (fdev->sector_size == BCMA_VAL_4K) {
        opcode = BCMA_QSPI_CMD_SE_4K;
    } else {
        opcode = BCMA_QSPI_CMD_SE;
    }

    addr = offset;
    end = offset + len;
    while (addr < end) {
        /* Write enable before each ERASE command */
        SHR_IF_ERR_EXIT
            (qspi_flash_write_enable(unit, true));

        /* Erase one sector */
        SHR_IF_ERR_EXIT
            (qspi_flash_cmd_addr_set(opcode, addr, 0, sizeof(cmd), cmd));
        SHR_IF_ERR_EXIT
            (bcmbd_qspi_transfer_one(unit, sizeof(cmd), cmd, 0, NULL));

        /* Wait command complete */
        SHR_IF_ERR_EXIT
            (qspi_flash_write_wait(unit, BCMA_QSPI_SE_TIMEOUT));

        addr += fdev->sector_size;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
qspi_flash_erase_hybrid(int unit, bcma_flash_dev_t *fdev,
                         uint32_t offset, size_t len)
{
    uint8_t opcode, cmd[4], cfgr;
    uint32_t addr, end, sec4k_start, sec4k_end, erase_size;

    SHR_FUNC_ENTER(unit);

    /* Determine the 4KB sectors at high or low address. */
    opcode = BCMA_CYPRESS_CMD_RCFG;
    SHR_IF_ERR_EXIT
        (bcmbd_qspi_transfer_one(unit, sizeof(opcode), &opcode,
                                 sizeof(cfgr), &cfgr));
    if (cfgr & BCMA_CYPRESS_CFG_TBPARM_BIT) {
        /* 4KB sectors at high address. */
        sec4k_start = fdev->size - (32 * BCMA_VAL_4K);
        sec4k_end = fdev->size;
    } else {
        /* 4KB sectors at low address. */
        sec4k_start = 0;
        sec4k_end = 32 * BCMA_VAL_4K;
    }

    /* Check the given offset is sector aligned. */
    if (offset >= sec4k_start && offset < sec4k_end) {
        erase_size = BCMA_VAL_4K;
    } else {
        erase_size = fdev->sector_size;
    }
    if (offset % erase_size) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit, "Erase offset is not sector boudary.\n")));
    }

    addr = offset;
    end = offset + len;
    while (addr < end) {
        if (offset >= sec4k_start && offset < sec4k_end) {
            erase_size = BCMA_VAL_4K;
        } else {
            erase_size = fdev->sector_size;
        }
        if (erase_size == BCMA_VAL_4K) {
            opcode = BCMA_QSPI_CMD_SE_4K;
        } else {
            opcode = BCMA_QSPI_CMD_SE;
        }

        /* Write enable before each ERASE command */
        SHR_IF_ERR_EXIT
            (qspi_flash_write_enable(unit, true));

        /* Erase one sector */
        SHR_IF_ERR_EXIT
            (qspi_flash_cmd_addr_set(opcode, addr, 0, sizeof(cmd), cmd));
        SHR_IF_ERR_EXIT
            (bcmbd_qspi_transfer_one(unit, sizeof(cmd), cmd, 0, NULL));

        /* Wait command complete */
        SHR_IF_ERR_EXIT
            (qspi_flash_write_wait(unit, BCMA_QSPI_SE_TIMEOUT));

        addr += erase_size;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcma_flash_dev_set(int unit, bcma_flash_dev_t *fdev)
{
    SHR_FUNC_ENTER(unit);

    if (!fdev) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM, (BSL_META_U(unit, "fdev is NULL.\n")));
    }

    sal_memcpy(&custom_flash_dev[unit], fdev, sizeof(*fdev));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_flash_dev_probe(int unit, bcma_flash_dev_t *fdev)
{
    SHR_FUNC_ENTER(unit);

    if (!fdev) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM, (BSL_META_U(unit, "fdev is NULL.\n")));
    }

    SHR_IF_ERR_EXIT
        (qspi_flash_probe(unit, fdev));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_flash_read(int unit, uint32_t offset, size_t len, uint8_t *data)
{
    bcma_flash_dev_t fdev;
    uint8_t cmd[5];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (qspi_flash_probe(unit, &fdev));

    SHR_IF_ERR_EXIT
        (qspi_flash_cmd_addr_set(BCMA_QSPI_CMD_FAST_READ, offset, 1,
                                 sizeof(cmd), cmd));
    SHR_IF_ERR_EXIT
        (bcmbd_qspi_transfer_one(unit, sizeof(cmd), cmd, len, data));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_flash_write(int unit, uint32_t offset, size_t len, uint8_t *data)
{
    bcma_flash_dev_t fdev;
    uint8_t *data_ptr, *data_end;
    uint32_t addr, page_mask, first_page_offset, wbytes;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (qspi_flash_probe(unit, &fdev));

    data_ptr = data;
    data_end = data + len;

    /* Distance from the page boundary */
    page_mask = fdev.page_size - 1;
    first_page_offset = offset & page_mask;

    /* Write the first partial page if the start address is not page aligned */
    if (first_page_offset) {
        addr = offset;
        wbytes = fdev.page_size - first_page_offset;
        if (wbytes > len) {
            wbytes = len;
        }

        SHR_IF_ERR_EXIT
            (qspi_flash_page_write(unit, addr, wbytes, data_ptr));

        data_ptr += wbytes;
    }

    /* Write full pages */
    while ((data_ptr + fdev.page_size) <= data_end) {
        addr = data_ptr - data + offset;
        wbytes = fdev.page_size;

        SHR_IF_ERR_EXIT
            (qspi_flash_page_write(unit, addr, wbytes, data_ptr));

        data_ptr += wbytes;
    }

    /* Write the last partial page if the ending address is not page aligned */
    if (data_ptr < data_end) {
        addr = data_ptr - data + offset;
        wbytes = data_end - data_ptr;

        SHR_IF_ERR_EXIT
            (qspi_flash_page_write(unit, addr, wbytes, data_ptr));

        data_ptr += wbytes;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_flash_erase(int unit, uint32_t offset, size_t len)
{
    bcma_flash_dev_t fdev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (qspi_flash_probe(unit, &fdev));

    if (fdev.sector_type == BCMA_FLASH_SECTOR_TYPE_HYBRID) {
        SHR_IF_ERR_EXIT
            (qspi_flash_erase_hybrid(unit, &fdev, offset, len));
    } else {
        SHR_IF_ERR_EXIT
            (qspi_flash_erase_uniform(unit, &fdev, offset, len));
    }

exit:
    SHR_FUNC_EXIT();
}
