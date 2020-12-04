/*! \file ngbde_kapi.h
 *
 * NGBDE kernel API.
 *
 * This file is intended for use by other kernel modules relying on the BDE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGBDE_KAPI_H
#define NGBDE_KAPI_H

#include <linux/types.h>

/*! Maximum number of switch devices supported. */
#ifndef NGBDE_NUM_SWDEV_MAX
#define NGBDE_NUM_SWDEV_MAX     16
#endif

/*!
 * \brief Get Linux PCI device handle for a switch device.
 *
 * \param [in] kdev Device number.
 *
 * \return Linux PCI device handle or NULL if unavailable.
 */
extern struct pci_dev *
ngbde_kapi_pci_dev_get(int kdev);

/*!
 * \brief Get Linux kernel device handle for a switch device.
 *
 * \param [in] kdev Device number.
 *
 * \return Linux kernel device handle or NULL if unavailable.
 */
extern struct device *
ngbde_kapi_dma_dev_get(int kdev);

/*!
 * \brief Convert DMA bus address to virtual address.
 *
 * This API will convert a physical DMA bus address to a kernel
 * virtual address for a memory location that belongs to one of the
 * DMA memory pools allocated by the BDE module.
 *
 * \param [in] kdev Device number.
 * \param [in] baddr Physical DMA bus address for this device.
 *
 * \return Virtual kernel address or NULL on error.
 */
void *
ngbde_kapi_dma_bus_to_virt(int kdev, dma_addr_t baddr);

/*!
 * \brief Convert virtual address to DMA bus address.
 *
 * This API will convert a kernel virtual address to a physical DMA
 * bus address for a memory location that belongs to one of the DMA
 * memory pools allocated by the BDE module.
 *
 * \param [in] kdev Device number.
 * \param [in] vaddr Virtual kernel address.
 *
 * \return Physical DMA bus address for this device or 0 on error.
 */
dma_addr_t
ngbde_kapi_dma_virt_to_bus(int kdev, void *vaddr);

/*!
 * \brief Write a memory-mapped register from kernel driver.
 *
 * \param [in] kdev Device number.
 * \param [in] offs Register address offset.
 * \param [in] val Value to write to register.
 *
 * \return Nothing.
 */
extern void
ngbde_kapi_pio_write32(int kdev, uint32_t offs, uint32_t val);

/*!
 * \brief Read a memory-mapped register from kernel driver.
 *
 * \param [in] kdev Device number.
 * \param [in] offs Register address offset.
 *
 * \return Value read from register.
 */
extern uint32_t
ngbde_kapi_pio_read32(int kdev, uint32_t offs);

/*!
 * \brief Get base address fo memory-mapped I/O memory.
 *
 * The lgical base address returned can be used with ioread32, etc.
 *
 * \param [in] kdev Device number.
 *
 * \return Logical base address or NULL if unavailable.
 */
extern void *
ngbde_kapi_pio_membase(int kdev);

/*!
 * \brief Install kernel mode interrupt handler.
 *
 * \param [in] kdev Device number.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] isr_func Interrupt handler function.
 * \param [in] isr_data Interrupt handler context.
 *
 * \retval 0 No errors
 */
extern int
ngbde_kapi_intr_connect(int kdev, unsigned int irq_num,
                        int (*isr_func)(void *), void *isr_data);

/*!
 * \brief Uninstall kernel mode interrupt handler.
 *
 * \param [in] kdev Device number.
 * \param [in] irq_num MSI interrupt number.
 *
 * \retval 0 No errors
 */
extern int
ngbde_kapi_intr_disconnect(int kdev, unsigned int irq_num);

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by an interrupt handler when a shared
 * interrupt mask register needs to be updated.
 *
 * Note that the mask register to access is referenced by the
 * corrsponding status register. This is because the mask register may
 * be different depending on the host CPU interface being used
 * (e.g. PCI vs. AXI). On the other hand, the status register is the
 * same irrespective of the host CPU interface.
 *
 * \param [in] kdev Device number.
 * \param [in] irq_num Interrupt number (MSI vector).
 * \param [in] status_reg Corresponding interrupt status register offset.
 * \param [in] mask_val New value to write to mask register.
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
ngbde_kapi_intr_mask_write(int kdev, unsigned int irq_num,
                           uint32_t status_reg, uint32_t mask_val);

#endif /* NGBDE_KAPI_H */
