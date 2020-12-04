/*! \file pci.h
 *
 * PCI Local Bus constants and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PCI_H
#define PCI_H

#include <sal/sal_libc.h>

/*
 * PCI Configuration Header offsets
 */
#define PCI_CONF_ID             0x00
#define PCI_CONF_REV            0x08
#define PCI_CONF_BAR0           0x10
#define PCI_CONF_BAR1           0x14
#define PCI_CONF_ILINE          0x3c

typedef void (*pci_isr_t)(void *isr_data);

typedef struct pci_dev_s {
    int bus_no;
    int dev_no;
    int func_no;
} pci_dev_t;

extern int pci_dma_putw(pci_dev_t *dev, unsigned long addr, uint32_t data);
extern int pci_dma_puth(pci_dev_t *dev, unsigned long addr, uint16_t data);
extern int pci_dma_putb(pci_dev_t *dev, unsigned long addr, uint8_t data);
extern uint32_t pci_dma_getw(pci_dev_t *dev, unsigned long addr);
extern uint16_t pci_dma_geth(pci_dev_t *dev, unsigned long addr);
extern uint8_t pci_dma_getb(pci_dev_t *dev, unsigned long addr);

extern int pci_config_putw(pci_dev_t *dev, uint32_t addr, uint32_t data);
extern uint32_t pci_config_getw(pci_dev_t *dev, uint32_t addr);

extern int pci_memory_putw(pci_dev_t *dev, uint32_t addr, uint32_t data);
extern uint32_t pci_memory_getw(pci_dev_t *dev, uint32_t addr);

extern int pci_device_iter(int (*rtn)(pci_dev_t *dev,
                                      uint16_t pci_ven_id,
                                      uint16_t pci_dev_id,
                                      uint8_t pci_rev_id));

extern int pci_dma_offset_set(int dev_no, unsigned long offs);

extern int pci_int_connect(int int_line, pci_isr_t isr, void *isr_data);

extern int pci_reset(void);

#endif /* PCI_H */
