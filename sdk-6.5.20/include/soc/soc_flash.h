/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Header for QSPI Flash Driver
 */
#ifndef _SOC_FLASH_H_
#define _SOC_FLASH_H_

#define SOC_FLASH_SECTOR_TYPE_UNIFORM (0)
#define SOC_FLASH_SECTOR_TYPE_HYBRID  (1)
typedef struct soc_flash_conf_s {
    uint8      id;
    size_t     page_size;
    size_t     sector_size;
    uint32     sector_type;
    uint32     nr_sectors;
    size_t     size;
} soc_flash_conf_t;

extern int
soc_flash_init(int unit,
               uint32 max_hz,
               uint32 mode,
               soc_flash_conf_t *spi_flash);

extern int
soc_flash_cleanup(int unit);

extern int
soc_flash_read(int unit,
               uint32 offset,
               size_t len,
               void *buf);

extern int
soc_flash_write(int unit,
                uint32 offset,
                size_t len,
                const void *buf);

extern int
soc_flash_erase(int unit,
                uint32 offset,
                size_t len);

extern int
soc_flash_cmd_write_status(int unit,
                           uint8 sr);

#endif /* _SOC_FLASH_H_ */
