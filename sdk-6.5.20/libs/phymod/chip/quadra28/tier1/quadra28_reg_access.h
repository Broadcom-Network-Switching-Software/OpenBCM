/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef QUADRA28_REG_ACCESS_H
#define QUADRA28_REG_ACCESS_H

#define QUADRA28_CLAUSE45_ADDR(_devad, _regad)     \
            ((((_devad) & 0x3F) << 16) |        \
             ((_regad) & 0xFFFF))




int phymod_raw_iblk_read(const phymod_access_t *pa, uint32_t reg_addr, uint32_t *data);
int phymod_raw_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);
int quadra28_reg_modify(const phymod_access_t *pa,
                     uint32_t addr,
                     uint16_t reg_data,
                     uint16_t reg_mask);

#endif

