/*! \file bcmlu_ngbde.h
 *
 * Linux user mode connector for kernel mode BDE.
 *
 * This module provides a number of APIs for communicating with the
 * Linux kernel BDE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLU_NGBDE_H
#define BCMLU_NGBDE_H

#include <inttypes.h>

/*!
 * \name Type of device.
 * \anchor BCMLU_DEV_TYPE_xxx
 *
 * The BDE is able to discover multiple types of devices, and since
 * the different types are managed by different software drivers, we
 * need to be able to distinguish them.
 */

/*! \{ */
#define BCMLU_DEV_TYPE_SWITCH   0       /*!< Network switch device. */
#define BCMLU_DEV_TYPE_ETH      1       /*!< Ethernet controller. */
#define BCMLU_DEV_TYPE_CPU      2       /*!< Embedded CPU device. */
/*! \} */

/*!
 * \name Bus type.
 * \anchor BCMLU_DEV_BUS_xxx
 *
 * Type of bus by which the device is connected to the host CPU.
 */

/*! \{ */
#define BCMLU_DEV_BUS_PCI       0       /*!< PCI bus. */
#define BCMLU_DEV_BUS_AXI       1       /*!< AXI bus (ARM system bus). */
#define BCMLU_DEV_BUS_RCPU      2       /*!< CMIC RCPU (network) interface. */
/*! \} */

/*!
 * \name Device flags.
 * \anchor BCMLU_DEV_F_xxx
 *
 * The device flags are use to inform the user mode software driver
 * about any special handling, which may be required in the current
 * device configuration.
 */

/*! \{ */

/*! PCI interrupts are operating in MSI mode. */
#define BCMLU_DEV_F_MSI         (1 << 0)

/*! \} */

/*! Maximum number if I/O windows per device. */
#define BCMLU_NUM_IOWIN_MAX     3

/*! Memory-mapped I/O window. */
typedef struct bcmlu_dev_iowin_s {

    /*! Physical I/O address */
    uint64_t addr;

    /*! I/O window size */
    uint32_t size;

} bcmlu_dev_iowin_t;

/*! Device information retrieved during probing. */
typedef struct bcmlu_dev_info_s {

    /*! Device type (BCMLU_DEV_TYPE_xxx) */
    uint8_t type;

    /*! Device bus (BCMLU_DEV_BUS_xxx) */
    uint8_t bus;

    /*! Device flags (BCMLU_DEV_F_xxx) */
    uint16_t flags;

    /*! Vendor ID (typically the PCI vendor ID) */
    uint16_t vendor_id;

    /*! Device ID (typically the PCI vendor ID) */
    uint16_t device_id;

    /*! Device revision (typically the PCI device revision) */
    uint16_t revision;

    /*! Device model (device-identification beyond PCI generic ID) */
    uint16_t model;

    /*! Device I/O window(s) */
    bcmlu_dev_iowin_t iowin[BCMLU_NUM_IOWIN_MAX];

} bcmlu_dev_info_t;

/*! Descriptor for contiguous DMA memory blocks. */
typedef struct bcmlu_dma_info_s {

    /*! Physical DMA memory address */
    uint64_t addr;

    /*! Bus DMA memory address (same as physical if no IOMMU) */
    uint64_t baddr;

    /*! DMA memory block size */
    uint32_t size;

} bcmlu_dma_info_t;

/*!
 * \brief Initialize BDE user mode library.
 *
 * Verify the connection to the Linux kernel module.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to open compatible device.
 */
extern int
bcmlu_ngbde_init(void);

/*!
 * \brief Clean up resources allocated during initialization.
 *
 * \retval 0 Always.
 */
extern int
bcmlu_ngbde_cleanup(void);

/*!
 * \brief Get number of probed switch devices.
 *
 * \param [out] ndev
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve information.
 */
extern int
bcmlu_ngbde_num_dev_get(int *ndev);

/*!
 * \brief Get device information.
 *
 * Please refer to \ref bcmlu_dev_info_t for details about the
 * information provided.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] di Pointer to device information structure to be filled.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve information.
 */
extern int
bcmlu_ngbde_dev_info_get(int devid, bcmlu_dev_info_t *di);

/*!
 * \brief Get device information.
 *
 * Please refer to \ref bcmlu_dma_info_t for details about the
 * information provided.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] di Pointer to DMA information structure to be filled.
 *
 * \retval 0 No errors.
 * \retval -1 Unable to retrieve information.
 */
extern int
bcmlu_ngbde_dma_info_get(int devid, bcmlu_dma_info_t *di);

/*!
 * \brief Connect to hardware interrupt handler.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Unable to connect to hardware interrupt handler.
 */
extern int
bcmlu_ngbde_intr_connect(int devid, int irq_num);

/*!
 * \brief Disconnect from hardware interrupt handler.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Unable to disconnect from hardware interrupt handler.
 */
extern int
bcmlu_ngbde_intr_disconnect(int devid, int irq_num);

/*!
 * \brief Wait for hardware interrupt.
 *
 * A user mode thread will call this function and sleep until a
 * hardware interrupt occurs.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_wait(int devid, int irq_num);

/*!
 * \brief Wake up sleeping interrupt thread.
 *
 * Wake up interrupt thread even if no interrupt has occurred.
 *
 * Intended for graceful shut-down procedure.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_stop(int devid, int irq_num);

/*!
 * \brief Clear list of interrupt status/mask registers.
 *
 * This function is typically called before new interrupt register
 * information is added.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_regs_clr(int devid, int irq_num);

/*!
 * \brief Add interrupt status/mask register to monitor.
 *
 * This function adds a new interrupt status/mask register set to the
 * list of registers monitored by the user-mode interrupt handler.
 *
 * The register list is used to determine whether a user-mode
 * interrupt has occurred.
 *
 * See also \ref bcmlu_ngbde_intr_regs_clr.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 * \param [in] status_reg Interrupt status register offset.
 * \param [in] mask_reg Interrupt mask register offset.
 * \param [in] kmask Mask of interrupts handled by secondary ISR.
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_reg_add(int devid, int irq_num, uint32_t status_reg,
                         uint32_t mask_reg, uint32_t kmask);

/*!
 * \name Interrupt ACK register access flags.
 * \anchor BCMLU_INTR_ACK_F_xxx
 */

/*! \{ */

/*! Interrupt ACK registers resides in PCI bridge window. */
#define BCMLU_INTR_ACK_F_PAXB (1 << 0)

/*! \} */

/*!
 * \brief Add interrupt ACK register and mask value.
 *
 * This function adds a new interrupt ACK register and mask value
 * to clear hardware interrupt.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num Interrupt number (MSI vector).
 * \param [in] ack_reg Interrupt ACK register offset.
 * \param [in] ack_val Interrupt ACK value.
 * \param [in] flags Base address window for ack_reg.
 *
 * \retval 0 No errors
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_ack_reg_add(int devid, int irq_num, uint32_t ack_reg,
                             uint32_t ack_val, uint32_t flags);

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * Since the register is shared with another (kernel mode) interrupt
 * handler, access must be protected by a lock.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt mask register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
extern int
bcmlu_ngbde_intr_mask_write(int devid, int irq_num,
                            uint32_t offs, uint32_t val);

/*!
 * \brief Map device I/O memory in kernel driver.
 *
 * This function is used to provide device I/O access for a kernel
 * mode driver.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] addr Physical address of I/O memory.
 * \param [in] size Size of I/O memory window.
 *
 * \return Pointer to mapped I/O memory, or NULL on error.
 */
extern int
bcmlu_ngbde_pio_map(int devid, uint64_t addr, uint32_t size);

/*!
 * \brief Map interrupt controller I/O memory in kernel driver.
 *
 * This function is used to provide interrupt controller I/O access
 * for a kernel mode driver.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] addr Physical address of I/O memory.
 * \param [in] size Size of I/O memory window.
 *
 * \return Pointer to mapped I/O memory, or NULL on error.
 */
extern int
bcmlu_ngbde_iio_map(int devid, uint64_t addr, uint32_t size);

/*!
 * \brief Map PCI bridge I/O memory in kernel driver.
 *
 * This function is used to provide interrupt controller I/O access
 * for a kernel mode driver.
 *
 * \param [in] devid Device ID (first device is 0).
 * \param [in] addr Physical address of I/O memory.
 * \param [in] size Size of I/O memory window.
 *
 * \return Pointer to mapped I/O memory, or NULL on error.
 */
extern int
bcmlu_ngbde_paxb_map(int devid, uint64_t addr, uint32_t size);

/*!
 * \brief Perform mmap through BDE kernel module.
 *
 * This function is provided to get around common kernel restrictions
 * with regards to mapping kernel RAM memory via the /dev/mem device.
 *
 * \param [in] offset Physical memory address.
 * \param [in] size Size of memory block to map.
 *
 * \return Logical address of mapped memory or NULL on error.
 */
extern void *
bcmlu_ngbde_mmap(uint64_t offset, size_t size);

/*!
 * \brief Unmap kernel memory.
 *
 * Unmap memory previously mapped using \ref bcmlu_ngbde_mmap.
 *
 * \param [in] addr Logical address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
extern int
bcmlu_ngbde_munmap(void *addr, size_t size);

#endif /* BCMLU_NGBDE_H */
