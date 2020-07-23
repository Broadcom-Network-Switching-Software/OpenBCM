/*! \file bcma_sys_probe.c
 *
 * User mode interfaces for Linux BDE kernel module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_thread.h>

#include <bsl/bsl.h>

#include <shr/shr_mpool.h>
#include <shr/shr_timeout.h>
#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlu/bcmlu_mmap.h>
#include <bcmlu/bcmlu_ngbde.h>
#include <bcmlu/bcmpkt/bcmlu_knet.h>
#include <bcmlu/bcmpkt/bcmlu_socket.h>

#include <bcma/sys/bcma_sys_probe.h>

/*
 * The path to the driver device files
 */
#define DEV_FNAME       "/dev/mem"
#define DEV_FMODE       (O_RDWR | O_SYNC)

/*!
 * \brief Tracked information about probed devices.
 *
 * Note that we only support a single IRQ line per device. If this
 * needs to be exapnded, then the thread and isr data must be tracked
 * per IRQ line.
 */
typedef struct sys_dev_s {

    /*! Device number in kernel BDE. */
    int kdev;

    /*! IRQ line services by ISR thread. */
    int intr_thread_irq_num;

    /*! Run state for ISR thread. */
    bool intr_thread_running;

    /*! Stop signal for ISR thread. */
    bool intr_thread_stop;

    /*! ISR thread ID. */
    sal_thread_t intr_thread_id;

    /*! ISR thread lock. */
    sal_mutex_t intr_lock;

    /*! Interrupt handler provided by the SDK. */
    bcmdrd_hal_isr_func_f isr_func;

    /*! Context for the interrupt handler. */
    void *isr_data;

    /*! Physical address of DMA memory pool. */
    uint64_t dma_pbase;

    /*! Bus address of DMA memory pool. */
    uint64_t dma_bbase;

    /*! Size (in bytes) of DMA memory pool. */
    size_t dma_size;

    /*! Logical (mapped) address of DMA memory pool. */
    void *dma_lbase;

    /*! Physical address of HA DMA memory pool. */
    uint64_t ha_dma_pbase;

    /*! Bus address of HA DMA memory pool. */
    uint64_t ha_dma_bbase;

    /*! Size (in bytes) of HA DMA memory pool. */
    size_t ha_dma_size;

    /*! Logical (mapped) address of HA DMA memory pool. */
    void *ha_dma_lbase;

    /*! DMA memory manager (for alloc/free). */
    shr_mpool_handle_t mpool;

    /*! Override device ID. */
    bcmdrd_dev_id_t override_id;

} sys_dev_t;

/*! Actual devices in our system. */
static sys_dev_t sys_devs[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Map I/O memory into host memory space.
 *
 * <long-description>
 *
 * \param [in] offset Physical address start.
 * \param [in] size Size of region to map into host CPU memory space.
 *
 * \return Pointer to mapped region, or NULL on failure.
 */
void *
io_remap(uint64_t offset, size_t size)
{
    return bcmlu_ngbde_mmap(offset, size);
}

/*!
 * \brief Unmap I/O memory from host memory space.
 *
 * \param [in] iomem Mapped host CPU memory space.
 * \param [in] size Size of region to unmap.
 *
 * \return Nothing.
 */
void
io_unmap(void *iomem, size_t size)
{
    bcmlu_ngbde_munmap(iomem, size);
}

/*!
 * \brief Convert logical DMA address to physical DMA address.
 *
 * \param [in] sd Device handle.
 * \param [in] addr Logical DMA address.
 *
 * \return Physical DMA address.
 */
static uint64_t
dma_l2p(sys_dev_t *sd, void *addr)
{
    uint64_t bbase = sd->dma_bbase;
    unsigned long lbase = (unsigned long)sd->dma_lbase;
    unsigned long laddr = (unsigned long)addr;

    if (addr == NULL) {
        return 0;
    }

    /* DMA memory is one contiguous block */
    return bbase + (laddr - lbase);
}

/*!
 * \brief Interrupt handler thread.
 *
 * This thread is a high-priority thread which runs the user mode
 * interrupt handler.
 *
 * The thread will be sleeping in the kernel BDE until a hardware
 * interrupt occurs, upon which the thread is woken up by the real
 * kernel mode ISR.
 *
 * \param [in] data Interrupt handler context (\ref sys_dev_t).
 *
 * \return Nothing.
 */
static void
intr_thread(void *data)
{
    sys_dev_t *sd = (sys_dev_t *)data;

    sd->intr_thread_running = true;

    while (1) {
        bcmlu_ngbde_intr_wait(sd->kdev, sd->intr_thread_irq_num);
        if (sd->intr_thread_stop) {
            break;
        }
        if (sd->isr_func) {
            /* Run ISR with sync lock held */
            sal_mutex_take(sd->intr_lock, SAL_MUTEX_FOREVER);
            sd->isr_func(sd->isr_data);
            sal_mutex_give(sd->intr_lock);
        }
    }

    sd->intr_thread_running = false;
}

/*!
 * \brief Install user-mode interrupt handler.
 *
 * The user mode interrupt handler will be executed by a high-priority
 * thread, which is kicked off by the real hardware interrupt.
 *
 * See also \ref intr_thread.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] isr_func User mode interrupt handler.
 * \param [in] isr_data Interrupt handler context.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
intr_connect(void *devh, int irq_num,
             bcmdrd_hal_isr_func_f isr_func, void *isr_data)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    if (bcmlu_ngbde_intr_connect(sd->kdev, irq_num) < 0) {
        return -1;
    }

    sd->isr_func = isr_func;
    sd->isr_data = isr_data;

    sd->intr_thread_stop = false;
    sd->intr_thread_irq_num = irq_num;
    sd->intr_thread_id = sal_thread_create("bcmaSysIsr",
                                           4096, SAL_THREAD_PRIO_INTERRUPT,
                                           intr_thread, devh);
    if (sd->intr_thread_id == SAL_THREAD_ERROR) {
        sd->intr_thread_stop = true;
        bcmlu_ngbde_intr_disconnect(sd->kdev, irq_num);
        return -1;
    }

    return 0;
}

/*!
 * \brief Uninstall user-mode interrupt handler.
 *
 * Remove interrupt handler previously installed using \ref
 * intr_connect.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
intr_disconnect(void *devh, int irq_num)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = 5000000;
    int min_polls = 1;

    sd->intr_thread_stop = true;

    if (bcmlu_ngbde_intr_stop(sd->kdev, irq_num) < 0) {
        return -1;
    }

    shr_timeout_init(&to, timeout_usec, min_polls);
    while (1) {
        if (!sd->intr_thread_running) {
            break;
        }
        if (shr_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Failed to stop interrupt thread\n")));
            return -1;
        }
    }

    if (bcmlu_ngbde_intr_disconnect(sd->kdev, irq_num) < 0) {
        return -1;
    }

    /* Clear associated resource variables */
    sd->intr_thread_stop = false;
    sd->intr_thread_irq_num = 0;
    sd->isr_func = NULL;
    sd->isr_data = NULL;

    return 0;
}

/*!
 * \brief Serialize function with interrupt handler.
 *
 * Take interrupt lock and adjust scheduling priority to avoid
 * priority inversion.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] cb Function to be serialized with interrupt context.
 * \param [in] data Pass-through data for \c cb function.
 *
 * \return Return value of callback function.
 */
static int
intr_sync(void *devh, int irq_num,
          bcmdrd_intr_sync_cb_f cb, void *data)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    int prio;
    int rv;

    if (!cb) {
        /* Silently ignore */
        return 0;
    }

    /* Calling this function from the interrupt thread will cause a deadlock */
    if (sal_thread_self() == sd->intr_thread_id) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("intr_sync called from interrupt thread\n")));
        return 0;
    }

    if (sal_thread_prio_get(&prio) < 0) {
        /* Do not change priority (should never get here) */
        prio = SAL_THREAD_PRIO_INTERRUPT;
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META("intr_sync called with unknown thread priority\n")));
    } else {
        sal_thread_prio_set(SAL_THREAD_PRIO_INTERRUPT);
    }

    sal_mutex_take(sd->intr_lock, SAL_MUTEX_FOREVER);

    /* Perform callback */
    rv = cb(data);

    sal_mutex_give(sd->intr_lock);

    if (prio != SAL_THREAD_PRIO_INTERRUPT) {
        /* Restore scheduling policy if it was modified */
        sal_thread_prio_set(prio);
    }

    return rv;
}

/*!
 * \brief Configure the interrupt handler thread.
 *
 * The information provided allows the kernel mode ISR to access and
 * interpret the IRQ registers associated with this IRQ line.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] intr_info Interrupt configuration information.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
static int
intr_configure(void *devh, int irq_num, bcmdrd_hal_intr_info_t *intr_info)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    bcmdrd_hal_irq_reg_t *ir;
    uint32_t flags = 0;
    int rv, idx;

    if (intr_info == NULL) {
        return -1;
    }

    if (intr_info->piowin_size > 0) {
        rv = bcmlu_ngbde_pio_map(sd->kdev, intr_info->piowin_addr,
                                 intr_info->piowin_size);
        if (rv < 0) {
            return -1;
        }
    }

    if (intr_info->iiowin_size > 0) {
        rv = bcmlu_ngbde_iio_map(sd->kdev, intr_info->iiowin_addr,
                                 intr_info->iiowin_size);
        if (rv < 0) {
            return -1;
        }
    }

    if (bcmlu_ngbde_intr_regs_clr(sd->kdev, irq_num) < 0) {
        return -1;
    }

    for (idx = 0; idx < intr_info->num_irq_regs; idx++) {
        ir = &intr_info->irq_regs[idx];
        rv = bcmlu_ngbde_intr_reg_add(sd->kdev, irq_num,
                                      ir->status_reg, ir->mask_reg, ir->kmask);
        if (rv < 0) {
            return -1;
        }
    }

    if (intr_info->paxbwin_size > 0) {
        rv = bcmlu_ngbde_paxb_map(sd->kdev, intr_info->paxbwin_addr,
                                  intr_info->paxbwin_size);
        if (rv < 0) {
            return -1;
        }
        if (intr_info->intr_ack.flags & BCMDRD_HAL_INTR_ACK_F_PAXB) {
            flags |= BCMLU_INTR_ACK_F_PAXB;
        }
        rv = bcmlu_ngbde_intr_ack_reg_add(sd->kdev, irq_num,
                                          intr_info->intr_ack.ack_reg,
                                          intr_info->intr_ack.ack_val,
                                          flags);
        if (rv < 0) {
            return -1;
        }
    }

    return 0;
}

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * Since the register is shared with another (kernel mode) interrupt
 * handler, access must be protected by a lock.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt mask register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
static int
intr_mask_write(void *devh, int irq_num, uint32_t offs, uint32_t val)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    if (bcmlu_ngbde_intr_mask_write(sd->kdev, irq_num, offs, val) < 0) {
        return -1;
    }

    return 0;
}

/*!
 * \brief Allocate a block of DMA memory.
 *
 * This function is the system call-back function for dma_alloc.
 *
 * \param [in] devh Device context.
 * \param [in] size Size of DMA memory to allocate.
 * \param [out] dma_addr Physical address of DMA memory.
 *
 * \return Logical address of DMA memory, or NULL if error.
 */
static void *
dma_alloc(void *devh, size_t size, uint64_t *dma_addr)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    void *dma_mem;

    if (sd == NULL) {
        return NULL;
    }

    dma_mem = shr_mpool_alloc(sd->mpool, size);
    if (dma_addr) {
        *dma_addr = dma_l2p(sd, dma_mem);
    }

    return dma_mem;
}

/*!
 * \brief Free block of DMA memory.
 *
 * This function is the system call-back function for dma_alloc.
 *
 * \param [in] devh Device context.
 * \param [in] size Size of DMA memory to free.
 * \param [in] dma_mem Logical address of DMA memory.
 * \param [in] dma_addr Physical address of DMA memory.
 */
static void
dma_free(void *devh, size_t size, void *dma_mem, uint64_t dma_addr)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    if (sd) {
        shr_mpool_free(sd->mpool, dma_mem);
    }
}

/*!
 * \brief Map DMA memory to device.
 *
 * This function simply returns the physical bus address for the DMA
 * memory block.
 *
 * \param [in] devh Device context.
 * \param [in] dma_mem CPU address of memory block to map.
 * \param [in] size Size of memory block to map.
 * \param [in] to_dev True if transferring from host memory to device.
 *
 * \return Physical bus address or 0 if error.
 */
static uint64_t
dma_map(void *devh, void *dma_mem, size_t size, bool to_dev)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    uint64_t bbase;

    /* All DMA memory is uncached */
    bbase = dma_l2p(sd, dma_mem);

    /* Sanity check */
    if (bbase < sd->dma_bbase ||
        (bbase + size) >= (sd->dma_bbase + sd->dma_size)) {
        return 0;
    }

    return bbase;
}

/*!
 * \brief Unmap DMA memory for device.
 *
 * Empty function since all DMA memory is uncached.
 *
 * \param [in] devh Device context.
 * \param [in] dma_addr Physical bus address of mapped memory.
 * \param [in] size Size of memory block to unmap.
 * \param [in] to_dev True if transferring from host memory to device.
 */
static void
dma_unmap(void *devh, uint64_t dma_addr, size_t size, bool to_dev)
{
    /* All DMA memory is uncached */
}

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_probe_cleanup(void)
{
    int unit;
    sys_dev_t *sd;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        sd = &sys_devs[unit];
        if (bcmdrd_dev_exists(unit)) {
            bcmdrd_dev_destroy(unit);
        }
        if (sd->intr_lock) {
            sal_mutex_destroy(sd->intr_lock);
            sd->intr_lock = NULL;
        }
        if (sd->mpool) {
            shr_mpool_destroy(sd->mpool);
            sd->mpool = NULL;
        }
        if (sd->dma_lbase) {
            bcmlu_ngbde_munmap(sd->dma_lbase, sd->dma_size);
            sd->dma_lbase = NULL;
        }
        if (sd->ha_dma_lbase) {
            bcmlu_ngbde_munmap(sd->ha_dma_lbase, sd->ha_dma_size);
            sd->ha_dma_lbase = NULL;
        }
    }

    /* Detach KNET driver*/
    bcmlu_knet_detach();

    /* Close BDE device */
    bcmlu_ngbde_cleanup();

    return 0;
}

int
bcma_sys_probe(void)
{
    int ndevs = 0;
    int unit;
    int bdx;
    int idx;
    sys_dev_t *sd;
    bcmlu_dev_info_t dev_info, *di = &dev_info;
    bcmlu_dma_info_t dma_info, *dmi = &dma_info;
    bcmdrd_dev_id_t id;
    bcmdrd_hal_io_t io;
    bcmdrd_hal_intr_t intr;
    bcmdrd_hal_dma_t dma;
    int bde_devs;
    int rv = SHR_E_NONE;

    /* Open BDE device */
    if (bcmlu_ngbde_init() != 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to open BDE device\n")));
        return -1;
    }

    /* Attach KNET driver*/
    rv = bcmlu_knet_attach();
    if (rv < 0) {
        if (rv == SHR_E_UNAVAIL) {
            LOG_INFO(BSL_LS_SYS_PCI,
                     (BSL_META("KNET kernel module is not available.\n")));
        } else {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Unable to attach KNET driver\n")));
        }
    } else if (bcmlu_socket_attach() < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to attach SOCKET driver\n")));
    }

    /* Get all available devices */
    if (bcmlu_ngbde_num_dev_get(&bde_devs) < 0) {
        return -1;
    }

    /* For each device in the system */
    for (bdx = 0; bdx < bde_devs; bdx++) {

        /* Next unit number */
        unit = ndevs;

        /* Convenience pointer */
        sd = &sys_devs[unit];

        /* Kernel device handle */
        sd->kdev = bdx;

        /* Initialize ID structure */
        memset(&id, 0, sizeof(id));

        /* Get device details */
        if (bcmlu_ngbde_dev_info_get(bdx, di) < 0) {
            return -1;
        }

        if (sd->intr_lock == NULL) {
            sd->intr_lock = sal_mutex_create("bcmaSysIntr");
        }
        if (sd->intr_lock == NULL) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Unable to create interrupt lock\n")));
            return -1;
        }

        /* Add device info to the ID structure */
        id.vendor_id = di->vendor_id;
        id.device_id = di->device_id;
        id.revision = di->revision;

        /* Check for override ID */
        if (sd->override_id.vendor_id) {
            sal_memcpy(&id, &sd->override_id, sizeof(bcmdrd_dev_id_t));
        }

        /* Create device */
        if (bcmdrd_dev_create(unit, &id) < 0) {
            LOG_WARN(BSL_LS_SYS_PCI,
                     (BSL_META_U(unit,
                                 "Failed to create device "
                                 "(%04x:%04x:%02x:%08x:%08x)\n"),
                      di->vendor_id, di->device_id, di->revision,
                      (unsigned int)di->iowin[0].addr,
                      (unsigned int)di->iowin[1].addr));
            continue;
        }

        /* Initialize I/O structure */
        memset(&io, 0, sizeof(io));

        if (di->flags & BCMLU_DEV_F_MSI) {
            io.flags |= BCMDRD_HAL_IO_F_MSI;
        }

        
        io.bus_type = BCMDRD_BT_PCI;

        /* Configure host bus byte swapping. */
        io.byte_swap_pio = SYS_BE_PIO;
        io.byte_swap_packet_dma = SYS_BE_PACKET;
        io.byte_swap_non_packet_dma = SYS_BE_OTHER;

        /*! Physical address(es) for memory-mapped I/O. */
        io.phys_addr[0] = di->iowin[0].addr;
        io.phys_addr[1] = di->iowin[1].addr;

        /* Callback context is local system device */
        io.devh = sd;

        /*! Map I/O memory into CPU space. */
        io.ioremap = io_remap;
        io.iounmap = io_unmap;

        /* Optionally dump I/O memory resource information */
        for (idx = 0; idx < 2; idx++) {
            if (io.phys_addr[idx] == 0) {
                continue;
            }
            LOG_INFO(BSL_LS_SYS_PCI,
                     (BSL_META_U(unit,
                                 "I/O memory resource #%d at 0x%08"PRIx64"\n"),
                      idx, io.phys_addr[idx]));
        }
        LOG_INFO(BSL_LS_SYS_PCI,
                 (BSL_META_U(unit,
                             "I/O memory will be mapped using %s\n"),
                  bcmlu_mmap_type_str()));

        if (bcmdrd_dev_hal_io_init(unit, &io) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize I/O\n")));
            bcmdrd_dev_destroy(unit);
            continue;
        }

        /* Get DMA resources */
        if (bcmlu_ngbde_dma_info_get(bdx, dmi) == 0) {

            /* Optionally dump DMA information */
            LOG_INFO(BSL_LS_SYS_PCI,
                     (BSL_META_U(unit,
                                 "%d MB of kernel DMA memory at 0x%08"PRIx64" "
                                 "(bus address 0x%08"PRIx64")\n"),
                      dmi->size / (1024 * 1024), dmi->addr, dmi->baddr));

            /* Save physical properties of DMA memory */
            sd->dma_pbase = dmi->addr;
            sd->dma_bbase = dmi->baddr;
            sd->dma_size = dmi->size;

            /* Get required HA DMA memory size */
            (void)bcmdrd_dev_ha_dma_mem_get(unit, &sd->ha_dma_size, NULL);
            if (sd->ha_dma_size && dmi->size > sd->ha_dma_size) {
                /* Let HA DMA occupy lower part of kernel memory block */
                sd->ha_dma_pbase = sd->dma_pbase;
                sd->ha_dma_bbase = sd->dma_bbase;
                /* Let normal DMA occupy upper part of kernel memory block */
                sd->dma_pbase += sd->ha_dma_size;
                sd->dma_bbase += sd->ha_dma_size;
                sd->dma_size -= sd->ha_dma_size;
            } else {
                /* Insufficient DMA memory to allocate HA DMA memory */
                sd->ha_dma_size = 0;
                LOG_WARN(BSL_LS_SYS_PCI,
                         (BSL_META_U(unit,
                                     "Insufficient DMA memory for HA\n")));
            }

            /* Map DMA memory from kernel space */
            sd->dma_lbase = bcmlu_ngbde_mmap(sd->dma_pbase, sd->dma_size);
            if (sd->dma_lbase == NULL) {
                LOG_WARN(BSL_LS_SYS_PCI,
                         (BSL_META_U(unit,
                                     "Unable to map DMA memory\n")));
            }

            if (sd->dma_lbase) {
                /* Create memory pool for use by DMA memory allocator */
                sd->mpool = shr_mpool_create(sd->dma_lbase, sd->dma_size, 0);
                if (sd->mpool == NULL) {
                    LOG_WARN(BSL_LS_SYS_PCI,
                             (BSL_META_U(unit,
                                         "Unable to create DMA mpool\n")));
                }
            }

            if (sd->mpool) {
                /* Initialize DMA structure */
                memset(&dma, 0, sizeof(dma));

                /* Callback context is local system device */
                dma.devh = sd;

                /*! Configure function vectors */
                dma.dma_alloc = dma_alloc;
                dma.dma_free = dma_free;
                dma.dma_map = dma_map;
                dma.dma_unmap = dma_unmap;

                if (bcmdrd_dev_hal_dma_init(unit, &dma) < 0) {
                    LOG_ERROR(BSL_LS_SYS_PCI,
                              (BSL_META_U(unit,
                                          "Failed to initialize DMA\n")));
                    bcmdrd_dev_destroy(unit);
                    continue;
                }

            }

            /* Map HA DMA memory from kernel space */
            if (sd->ha_dma_size) {
                sd->ha_dma_lbase = bcmlu_ngbde_mmap(sd->ha_dma_pbase,
                                                    sd->ha_dma_size);
                if (sd->ha_dma_lbase == NULL) {
                    LOG_WARN(BSL_LS_SYS_PCI,
                             (BSL_META_U(unit,
                                         "Unable to map HA DMA memory\n")));
                }
            }

            if (sd->ha_dma_lbase) {
                if (bcmdrd_dev_ha_dma_mem_set(unit, sd->ha_dma_size,
                                              sd->ha_dma_lbase,
                                              sd->ha_dma_bbase) < 0) {
                    LOG_WARN(BSL_LS_SYS_PCI,
                             (BSL_META_U(unit,
                                         "Failed to initialize HA DMA\n")));
                }
            }
        }

        /* Initialize interrupt structure */
        memset(&intr, 0, sizeof(intr));

        /* Callback context is local system device */
        intr.devh = sd;

        /*! Connect/disconnect interrupt handler. */
        intr.intr_connect = intr_connect;
        intr.intr_disconnect = intr_disconnect;
        intr.intr_sync = intr_sync;
        intr.intr_configure = intr_configure;
        intr.intr_mask_write = intr_mask_write;

        if (bcmdrd_dev_hal_intr_init(unit, &intr) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize interrupt API\n")));
            bcmdrd_dev_destroy(unit);
            continue;
        }

        if (++ndevs >= COUNTOF(sys_devs)) {
            break;
        }
    }

    return ndevs;
}

int
bcma_sys_probe_override(int unit, const char *dev_name)
{
    bcmdrd_dev_id_t id;
    int idx;

    if (SHR_FAILURE(bcmdrd_dev_id_from_name(dev_name, &id))) {
        return -1;
    }

    for (idx = 0; idx < BCMDRD_CONFIG_MAX_UNITS; idx++) {
        if (unit >= 0 && unit != idx) {
            continue;
        }
        sal_memcpy(&sys_devs[idx].override_id, &id, sizeof(bcmdrd_dev_id_t));
    }
    return 0;
}

int
bcma_sys_probe_config_set(const char *config)
{
    return 0;
}

const char *
bcma_sys_probe_config_get(const char *prev)
{
    return NULL;
}
