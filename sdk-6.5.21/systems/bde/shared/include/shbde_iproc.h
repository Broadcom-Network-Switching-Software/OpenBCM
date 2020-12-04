/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SHBDE_IPROC_H__
#define __SHBDE_IPROC_H__

#include <shbde.h>

extern int
shbde_iproc_config_init(shbde_iproc_config_t *icfg,
                        unsigned int dev_id, unsigned int dev_rev);

extern int
shbde_iproc_paxb_init(shbde_hal_t *shbde, void *iproc_regs,
                      shbde_iproc_config_t *icfg);

extern unsigned int
shbde_iproc_pci_read(shbde_hal_t *shbde, void *iproc_regs,
                     unsigned int addr);

extern void
shbde_iproc_pci_write(shbde_hal_t *shbde, void *iproc_regs,
                      unsigned int addr, unsigned int data);

extern int
shbde_iproc_pcie_preemphasis_set(shbde_hal_t *shbde, void *iproc_regs,
                                 shbde_iproc_config_t *icfg, void *pci_dev);

#endif /* __SHBDE_IPROC_H__ */
