/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SHBDE_MDIO_H__
#define __SHBDE_MDIO_H__

#include <shbde.h>

typedef struct shbde_mdio_ctrl_s {

    /* Primary HAL*/
    shbde_hal_t *shbde;

    /* Context for iProc MDIO register access */
    void *regs;

    /* Base address for MDIO registers */
    unsigned int base_addr;

    /* iProc MDIO register access */
    unsigned int (*io32_read)(shbde_hal_t *shbde, void *iproc_regs, 
                              unsigned int addr);
    void (*io32_write)(shbde_hal_t *shbde, void *iproc_regs, 
                       unsigned int addr, unsigned int data);

} shbde_mdio_ctrl_t;


extern int
shbde_iproc_mdio_init(shbde_mdio_ctrl_t *smc);

extern int
shbde_iproc_mdio_read(shbde_mdio_ctrl_t *smc, unsigned int phy_addr,
                      unsigned int reg, unsigned int *val);

extern int
shbde_iproc_mdio_write(shbde_mdio_ctrl_t *smc, unsigned int phy_addr,
                       unsigned int reg, unsigned int val);

#endif /* __SHBDE_MDIO_H__ */
