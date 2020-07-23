/*! \file bcma_bcmbd_qspi.c
 *
 * QSPI CLI shared functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_qspi.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/flash/bcma_flash.h>
#include <bcma/bcmbd/bcma_bcmbd_qspi.h>

#define BSL_LOG_MODULE BSL_LS_APPL_QSPI
#define BCMA_BD_QSPI_MODE BCMBD_QSPI_MODE_CPHA | BCMBD_QSPI_MODE_CPOL
#define BCMA_BD_QSPI_SPEED 62500000


/*******************************************************************************
 * Internal public functions
 */

int
bcma_bcmbd_qspi_init(int unit)
{
    bcmbd_qspi_conf_t qspi_conf;

    SHR_FUNC_ENTER(unit);

    qspi_conf.mode = BCMA_BD_QSPI_MODE;
    qspi_conf.max_hz = BCMA_BD_QSPI_SPEED;
    SHR_IF_ERR_EXIT
        (bcmbd_qspi_init(unit, &qspi_conf));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmbd_qspi_flash_read(int unit, uint32_t offset, uint32_t len,
                           uint8_t *data)
{
    SHR_FUNC_ENTER(unit);

    if (len == 0) {
        SHR_EXIT();
    }

    /* Initialize QSPI. */
    SHR_IF_ERR_MSG_EXIT
        (bcma_bcmbd_qspi_init(unit),
         (BSL_META_U(unit, "Failed to initialize QSPI.\n")));

    /* Read flash data. */
    SHR_IF_ERR_MSG_EXIT
        (bcma_flash_read(unit, offset, len, data),
         (BSL_META_U(unit, "Failed to execute QSPI flash read.\n")));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmbd_qspi_flash_load(int unit, uint32_t offset, const char *fname)
{
    int fsize;
    uint8_t *wdata = NULL, *rdata = NULL;
    bcma_io_file_handle_t fh = NULL;
    bcma_flash_dev_t flash_dev;

    SHR_FUNC_ENTER(unit);

    /* Initialize QSPI. */
    SHR_IF_ERR_MSG_EXIT
        (bcma_bcmbd_qspi_init(unit),
         (BSL_META_U(unit, "Failed to initialize QSPI.\n")));

    /* Get the device properities. */
    SHR_IF_ERR_MSG_EXIT
        (bcma_flash_dev_probe(unit, &flash_dev),
         (BSL_META_U(unit, "Failed to probe flash device.\n")));

    if (offset % flash_dev.sector_size) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "offset is not sector boundary.\n")));
    }

    /* Open file. */
    fh = bcma_io_file_open(fname, "r");
    if (!fh) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Failed to open file %s.\n"),
              fname));
    }

    fsize = bcma_io_file_size(fh);
    if (fsize <= 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "File %s is empty.\n"),
              fname));
    }

    SHR_ALLOC(wdata, flash_dev.sector_size, "bcmaBcmbdFlashLoadWdata");
    if (!wdata) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Failed to allocate memory for write.\n")));
    }
    SHR_ALLOC(rdata, flash_dev.sector_size, "bcmaBcmbdFlashLoadRdata");
    if (!rdata) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Failed to allocate memory for read.\n")));
    }

    /* Write data into flash one sector by one sector. */
    while (fsize) {
        uint32_t wlen;

        /* Get data from file */
        if (flash_dev.sector_size > (uint32_t)fsize) {
            wlen = (uint32_t)fsize;
        } else {
            wlen = flash_dev.sector_size;
        }
        if (bcma_io_file_read(fh, wdata, sizeof(*wdata), wlen) < 0) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(unit, "Failed to read file.\n")));
        }

        /* Erase one sector */
        SHR_IF_ERR_MSG_EXIT
            (bcma_flash_erase(unit, offset, wlen),
             (BSL_META_U(unit, "Failed to erase flash.\n")));

        /* Write data */
        SHR_IF_ERR_MSG_EXIT
            (bcma_flash_write(unit, offset, wlen, wdata),
             (BSL_META_U(unit, "Failed to write flash.\n")));

        /* Read flash data back to verify */
        SHR_IF_ERR_MSG_EXIT
            (bcma_flash_read(unit, offset, wlen, rdata),
             (BSL_META_U(unit, "Failed to read flash.\n")));

        if (sal_memcmp(wdata, rdata, wlen) != 0) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(unit, "Flash data mismatch.\n")));
        }

        fsize -= wlen;
        offset += wlen;
    }

exit:
    if (fh) {
        bcma_io_file_close(fh);
    }
    SHR_FREE(wdata);
    SHR_FREE(rdata);
    SHR_FUNC_EXIT();
}
