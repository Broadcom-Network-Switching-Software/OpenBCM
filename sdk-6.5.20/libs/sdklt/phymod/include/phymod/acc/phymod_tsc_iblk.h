/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __PHYMOD_TSC_IBLK_H__
#define __PHYMOD_TSC_IBLK_H__

#include <phymod/phymod_acc.h>
#include <phymod/phymod_reg.h>

/* Special lane values for broadcast and dual-lane multicast */
#define PHYMOD_TSC_IBLK_MCAST01    4
#define PHYMOD_TSC_IBLK_MCAST23    5
#define PHYMOD_TSC_IBLK_BCAST      6

/* Special lane values for broadcast and dual-lane multicast for TSC blackhawk */
#define PHYMOD_TSCBH_IBLK_MCAST01       0x104
#define PHYMOD_TSCBH_IBLK_MCAST23       0x105
#define PHYMOD_TSCBH_IBLK_MCAST02       0x107
#define PHYMOD_TSCBH_IBLK_MCAST45       0x204
#define PHYMOD_TSCBH_IBLK_MCAST67       0x205
#define PHYMOD_TSCBH_IBLK_MCAST46       0x207
#define PHYMOD_TSCBH_IBLK_MCAST0123     0x106
#define PHYMOD_TSCBH_IBLK_MCAST4567     0x206
#define PHYMOD_TSCBH_IBLK_BCAST         0x306

#define PHYMOD_TSCBH_IBLK_MPP_0         0x1
#define PHYMOD_TSCBH_IBLK_MPP_1         0x2
#define PHYMOD_TSCBH_IBLK_MPP_0_1       0x3
#define PHYMOD_TSCBH_IBLK_MPP_SHIFT     8

/* Special lane values for broadcast and dual-lane multicast for blackhawk PMD register */
#define PHYMOD_BH_IBLK_MCAST01       0x20
#define PHYMOD_BH_IBLK_MCAST23       0x21
#define PHYMOD_BH_IBLK_MCAST45       0x22
#define PHYMOD_BH_IBLK_MCAST67       0x23
#define PHYMOD_BH_IBLK_MCAST0123     0x40
#define PHYMOD_BH_IBLK_MCAST4567     0x41
#define PHYMOD_BH_IBLK_BCAST         0xff

/* specail define for tsco 4 copy register access */
#define PHYMOD_TSCO_8_LANE_PORT_SHIFT 8


extern int
phymod_tsc_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tsc_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);

extern int
phymod_tscbh_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tscbh_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);

extern int
phymod_tsco1_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tsco1_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);

extern int
phymod_tsco2_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tsco2_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);

extern int
phymod_tsco4_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tsco4_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);


#endif /* __PHYMOD_TSC_IBLK_H__ */
