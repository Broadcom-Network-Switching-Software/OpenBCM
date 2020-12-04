/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Arm Processor Subsystem flash utility
 */

#ifndef SPIFLASH_H
#define SPIFLASH_H

#if defined(BCM_PLATFORM_STRING) || defined(STAND_ALONE)
#include <appl/diag/aps/spiflash_toc.h>
#else
#include "spiflash_toc.h"
#endif

#define SPIFLASH_PAGE_SIZE          256

typedef struct flash_info_s {
    char        name[16];
    uint32      rdid;
    uint32      page_size;
    uint32      erase_page_size;        /* MGMT_FLASH_ERASE_PAGE erase size */
    uint32      erase_sector_size;      /* MGMT_FLASH_ERASE_SECTOR erase size */
    uint32      flash_size;
} flash_info_t;

int spiflash_calculate_header(flash_info_t *flash_info, flash_header_t *header,
                              uint32 dmu_config, uint32 spi_speed);

flash_info_t *spiflash_info_by_name(char *name);
flash_info_t *spiflash_info_by_rdid(uint32 rdid);

#endif
