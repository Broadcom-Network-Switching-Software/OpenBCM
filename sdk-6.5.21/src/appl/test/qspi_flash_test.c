/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/test.h>
#include <appl/diag/system.h>
#include <sal/appl/io.h>
#include <sal/compiler.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include "testlist.h"
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/soc_flash.h>

#ifdef BCM_CMICX_SUPPORT

# define SPI_SPEED    62500000 /* BSPI */
# define SPI_MODE     3 /* (CPOL|CPHA) */

#define SWAP_ENDIAN(x) ( \
    (((x) << 24) & 0xff000000) | \
    (((x) <<  8) & 0x00ff0000) | \
    (((x) >>  8) & 0x0000ff00) | \
    (((x) >> 24) & 0x000000ff))

soc_flash_conf_t testflash;

char *qspi_flash_magic_str = "QSPI_FLASH";

/* initialize the test */
int qspi_flash_test_init(int u, args_t *a, void *p)
{
    unsigned int speed = SPI_SPEED;
    unsigned int mode = SPI_MODE;
    soc_mem_test_t *parm = p;
    int rv = SOC_E_NONE;
    cli_out("qspi_flash_test_init...\n");
    parm->userdata = qspi_flash_magic_str;
    rv = soc_flash_init(u, speed, mode, &testflash);
    if (SOC_FAILURE(rv)) {
        cli_out("Error: QSPI flash init failed, speed = %u, mode =%u\n", speed, mode);
    }
    return rv;
}

static void fill_pattern(uint32 *buf, uint32 bufsize, uint32 pattern)
{
    int i;
    int size = bufsize/4; /* Number of words to fill */

    for(i = 0; i < size; i++) {
        buf[i] = pattern;
    }
    return;
}

/* run the test */
int qspi_flash_test_run(int u, args_t *a, void *p)
{
    int rv = SOC_E_NONE;
    uint32 *wbuf = NULL;
    uint32 *rbuf = NULL;
    uint32 *sbuf = NULL;
    uint32 numsect = testflash.nr_sectors;
    uint32 sector, offset=0;
    uint32 startsect, endsect;
    soc_mem_test_t *parm = p;

    if(parm->index_end) {
        /* Boundary check */
        if((parm->index_start >= testflash.size) ||
           (parm->index_end >= testflash.size) ||
           (parm->index_end < parm->index_start)) {
           cli_out("IndexStart and IndexEnd not in flash boundaries 0 and 0x%zx\n", testflash.size);
           return(SOC_E_PARAM);
        }
        startsect = parm->index_start/testflash.sector_size;
        endsect = parm->index_end/testflash.sector_size;
    }
    else { /* default - test last 16 sectors */
        startsect = numsect - 16;
        endsect = numsect - 1;
    }

    cli_out("Max sectors in flash: %d, sectsize: 0x%zx\n", numsect, testflash.sector_size);
    cli_out("Testing sectors: %d to %d\n", startsect, endsect);

    wbuf = sal_alloc(testflash.sector_size, "FLASH_W");
    if (!wbuf) {
        cli_out("sal_alloc failed\n");
        rv = SOC_E_MEMORY;
        goto cleanup;
    }

    rbuf = sal_alloc(testflash.sector_size, "FLASH_R");
    if (!rbuf) {
        cli_out("sal_alloc failed\n");
        rv = SOC_E_MEMORY;
        goto cleanup;
    }

    sbuf = sal_alloc(testflash.sector_size, "FLASH_S");
    if (!sbuf) {
        cli_out("sal_alloc failed\n");
        rv = SOC_E_MEMORY;
        goto cleanup;
    }

    /* To retain  original content, we are going to test sectors
       only. We are not goig to erase whole flash,
       instead, erase sector wise, and recover that sector with
       original data after testing the sector */

    /* Test last 16 sectors to reduce the test time */
    for(sector = startsect; sector <= endsect; sector++) {
        offset = sector * testflash.sector_size;
        cli_out("Testing sector: %d, offset=0x%08x, size: 0x%08zx\n", sector, offset,
                                                          testflash.sector_size);
        /* Save sector data */
        rv = soc_flash_read(u, offset, testflash.sector_size, sbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash read failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto cleanup;
        }

        /* Sector erase and blank check */
        fill_pattern(wbuf, testflash.sector_size, 0);

        rv = soc_flash_erase(u,offset, testflash.sector_size);
        if(rv != SOC_E_NONE) {
            cli_out("Flash erase failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto cleanup;
        }

        rv = soc_flash_write(u, offset, testflash.sector_size, wbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash write failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        rv = soc_flash_read(u, offset, testflash.sector_size, rbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash read  failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        rv = sal_memcmp(wbuf, rbuf,testflash.sector_size);
        if(rv != 0) {
            cli_out("Data integrity failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        /* Walking 0 and 1 pattern */
        fill_pattern(wbuf, testflash.sector_size, 0x55555555);
        rv = soc_flash_erase(u,offset, testflash.sector_size);
        if(rv != SOC_E_NONE) {
            cli_out("Flash erase failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }
        rv = soc_flash_write(u, offset, testflash.sector_size, wbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash write failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        rv = soc_flash_read(u, offset, testflash.sector_size, rbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash read  failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        rv = sal_memcmp(wbuf, rbuf,testflash.sector_size);
        if(rv != 0) {
            cli_out("Data integrity failed with walking 0s and 1s, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }


        /* Walking 1 and 0 pattern */
        fill_pattern(wbuf, testflash.sector_size, 0xAAAAAAAA);
        rv = soc_flash_erase(u,offset, testflash.sector_size);
        if(rv != SOC_E_NONE) {
            cli_out("Flash erase failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }
        rv = soc_flash_write(u, offset, testflash.sector_size, wbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash write failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        rv = soc_flash_read(u, offset, testflash.sector_size, rbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Flash read  failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

        rv = sal_memcmp(wbuf, rbuf,testflash.sector_size);
        if(rv != 0) {
            cli_out("Data integrity failed with walking 1s and 0s, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto restore;
        }

restore:
        rv = soc_flash_erase(u,offset, testflash.sector_size);
        if(rv != SOC_E_NONE) {
            cli_out("Flash erase failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto cleanup;
        }

        rv = soc_flash_write(u, offset, testflash.sector_size, sbuf);
        if(rv != SOC_E_NONE) {
            cli_out("Fatal: Flash restore failed, sector: %d, size: 0x%08zx\n", sector,
                                                          testflash.sector_size);
            goto cleanup;
        }
    } /* End for */
cleanup:
    if(wbuf) sal_free(wbuf);
    if(rbuf) sal_free(rbuf);
    if(sbuf) sal_free(sbuf);

    return rv;
}


/* end the test */
int qspi_flash_test_done(int u, void *p)
{
    soc_mem_test_t *parm = p;
    parm->userdata = NULL;

    cli_out("QSPI flash test Done, please power cycle the system\n");
    return 0;
}

#endif /* BCM_CMICX_SUPPORT */
