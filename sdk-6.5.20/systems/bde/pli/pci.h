/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PCI Local Bus constants and structures.
 */

#ifndef __PCI_H__
#define __PCI_H__

#include <sal/types.h>

extern int
pci_dma_putw(pci_dev_t *dev, unsigned long addr, uint32 data);
extern int
pci_dma_puth(pci_dev_t *dev, unsigned long addr, uint16 data);
extern int
pci_dma_putb(pci_dev_t *dev, unsigned long addr, uint8 data);
extern uint32
pci_dma_getw(pci_dev_t *dev, unsigned long addr);
extern uint16
pci_dma_geth(pci_dev_t *dev, unsigned long addr);
extern uint8
pci_dma_getb(pci_dev_t *dev, unsigned long addr);
extern int
pci_dma_offset_set(int dev_no, unsigned long offs);
extern int
pci_reset(void);

#endif /* __PCI_H__ */
