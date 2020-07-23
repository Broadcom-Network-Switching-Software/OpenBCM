/*! \file bcma_sys_probe.c
 *
 * Probe function for PLI simulators (BCMSIM/PCId).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_mutex.h>

#include <bsl/bsl.h>

#include <shr/shr_mpool.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <sim/pli/pci.h>
#include <sim/pli/pli.h>

#include <bcma/sys/bcma_sys_probe.h>

#include <bcmbd/bcmbd_simhook.h>

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#error Simulation environemnt does not support direct register access
#endif

/* DMA pool size in MB */
#ifndef PLI_DMA_SIZE
#define PLI_DMA_SIZE            16
#endif

/* Maximum DMA pool size in MB */
#define PLI_DMA_SIZE_MAX        128

/* Convenience macro */
#define ONE_MB                  (1024 * 1024)

#define PLI_PCI_ILINE(_sd)	((_sd)->iline)

/*!
 * \brief Tracked information about probed devices.
 */
typedef struct sys_dev_s {

    /*! PCI device used for PLI I/O. */
    pci_dev_t pci_dev;

    /*! CMIC base address in iProc AXI space. */
    uint32_t cmic_base;

    /*! ISR thread lock. */
    sal_mutex_t intr_lock;

    /*! Interrupt configuration. */
    bcmdrd_hal_intr_info_t intr_info;

    /*! Interrupt handler provided by the SDK. */
    bcmdrd_hal_isr_func_f isr_func;

    /*! Context for the interrupt handler. */
    void *isr_data;

    /*! Hardware interrupt line (always zero). */
    uint32_t iline;

    /*! Memory block for DMA pool. */
    void *dma_pool;

    /*! DMA memory manager (for alloc/free). */
    shr_mpool_handle_t mpool;

    /*! Override device ID. */
    bcmdrd_dev_id_t override_id;

} sys_dev_t;

/*! Actual devices in our system. */
static sys_dev_t sys_devs[BCMDRD_CONFIG_MAX_UNITS];

static int ndevs = 0;

static bool use_schan_rpc = true;

static int
io_read(void *devh, bcmdrd_hal_io_space_t io_space,
        uint32_t addr, void *data, size_t size)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    uint32_t *data32 = data;

    if (io_space == BCMDRD_IOS_DEFAULT && size == 4) {
        *data32 = pci_memory_getw(&sd->pci_dev, sd->cmic_base + addr);
        return 0;
    }
    if (io_space == BCMDRD_IOS_CPU && size == 4) {
        *data32 = pci_memory_getw(&sd->pci_dev, addr);
        return 0;
    }
    /* Unsupported I/O space */
    return -1;
}

static int
io_write(void *devh, bcmdrd_hal_io_space_t io_space,
         uint32_t addr, void *data, size_t size)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    uint32_t *data32 = data;

    if (io_space == BCMDRD_IOS_DEFAULT && size == 4) {
        pci_memory_putw(&sd->pci_dev, sd->cmic_base + addr, *data32);
        return 0;
    }
    if (io_space == BCMDRD_IOS_CPU && size == 4) {
        pci_memory_putw(&sd->pci_dev, addr, *data32);
        return 0;
    }
    /* Unsupported I/O space */
    return -1;
}

static void
isr(void *devh)
{
    sys_dev_t *sd = (sys_dev_t *)devh;

    assert(sd);

    if (!sd->isr_func) {
        /* This may happen if KNET is active */
        return;
    }

    sal_mutex_take(sd->intr_lock, SAL_MUTEX_FOREVER);

    /* Run SDK interrupt handler */
    sd->isr_func(sd->isr_data);

    sal_mutex_give(sd->intr_lock);
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

    sd->isr_func = isr_func;
    sd->isr_data = isr_data;

    return pci_int_connect(PLI_PCI_ILINE(sd), isr, devh);
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

    sd->isr_func = NULL;
    sd->isr_data = NULL;

    return pci_int_connect(PLI_PCI_ILINE(sd), NULL, NULL);
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
    int rv;

    if (!cb) {
        /* Silently ignore */
        return 0;
    }

    sal_mutex_take(sd->intr_lock, SAL_MUTEX_FOREVER);

    /* Perform callback */
    rv = cb(data);

    sal_mutex_give(sd->intr_lock);

    return rv;
}

/*!
 * \brief Configure the interrupt handler.
 *
 * This function is not really required in PLI mode, but we maintain
 * it for testing purposes.
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

    sal_memcpy(&sd->intr_info, intr_info, sizeof(sd->intr_info));

    return 0;
}

/*!
 * \brief Write shared interrupt mask register.
 *
 * This function is used by the user-mode interrupt handler when a
 * shared interrupt mask register needs to be updated.
 *
 * This function is not really required in PLI mode, but we maintain
 * it for testing purposes.
 *
 * \param [in] devh Device context.
 * \param [in] irq_num MSI interrupt number.
 * \param [in] offs Interrupt status register offset.
 * \param [in] val New value to write to register.
 *
 * \retval 0 No errors
 * \retval <0 Something went wrong.
 */
static int
intr_mask_write(void *devh, int irq_num, uint32_t offs, uint32_t val)
{
    sys_dev_t *sd = (sys_dev_t *)devh;
    uint32_t mask;
    int idx;

    /* Find mask register that matches status register */
    mask = offs;
    for (idx = 0; idx < sd->intr_info.num_irq_regs; idx++) {
        if (offs == sd->intr_info.irq_regs[idx].status_reg) {
            mask = sd->intr_info.irq_regs[idx].mask_reg;
            break;
        }
    }
    return io_write(devh, BCMDRD_IOS_DEFAULT, mask, &val, 4);
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
 * \return Logical address of DMA mmoery, or NULL if error.
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
        *dma_addr = (uint64_t)(unsigned long)dma_mem;
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
    /* All DMA memory is uncached */
    return (uint64_t)(unsigned long)dma_mem;
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

/*!
 * \brief Create DRD device from PCI device.
 *
 * \param [in] dev PCI device structure.
 * \param [in] pci_vendor_id PCI vendor ID.
 * \param [in] pci_device_id PCI device ID.
 * \param [in] pci_revision PCI device revision.
 *
 * \retval 0 No errors
 */
static int
io_setup(pci_dev_t *dev,
         uint16_t pci_vendor_id,
         uint16_t pci_device_id,
         uint8_t pci_revision)
{
    int unit = ndevs;
    sys_dev_t *sd;
    uint32_t tmp;
    char *envstr;
    size_t dma_size;
    unsigned long dma_offs;
    bcmdrd_dev_id_t id;
    bcmdrd_hal_io_t io;
    bcmdrd_hal_intr_t intr;
    bcmdrd_hal_dma_t dma;

    /* Check for resources */
    if (ndevs >= COUNTOF(sys_devs)) {
        return -1;
    }

    /* Convenience pointer */
    sd = &sys_devs[unit];

    /* Set up ID structure */
    sal_memset(&id, 0, sizeof(id));
    id.vendor_id = pci_vendor_id;
    id.device_id = pci_device_id;
    id.revision = pci_revision;

    /* Check for override ID */
    if (sd->override_id.vendor_id) {
        sal_memcpy(&id, &sd->override_id, sizeof(bcmdrd_dev_id_t));
    }

    /* Skip unsupported devices */
    if (!bcmdrd_dev_supported(&id)) {
        return -1;
    }

    if (sd->intr_lock == NULL) {
        sd->intr_lock = sal_mutex_create("bcmaSysIntr");
    }
    if (sd->intr_lock == NULL) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to create interrupt lock.\n")));
        return -1;
    }

    if (bcmdrd_dev_create(unit, &id) < 0) {
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META_U(unit,
                             "Failed to create device "
                             "(%04x:%04x:%02x)\n"),
                  id.vendor_id, id.device_id, id.revision));
        return -1;
    }

    /* Allow driver to account for simulation environment */
    bcmdrd_feature_enable(unit, BCMDRD_FT_ACTIVE_SIM);

    /* Initialize I/O structure */
    sal_memset(&io, 0, sizeof(io));

    /* Callback context is local system device */
    io.devh = sd;

    io.read = io_read;
    io.write = io_write;

    if (bcmdrd_dev_hal_io_init(unit, &io) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META_U(unit,
                              "Failed to initialize I/O\n")));
        return -1;
    }

    /* Determine DMA pool size */
    dma_size = PLI_DMA_SIZE;
    if ((envstr = getenv("SOC_DMA_SIZE")) != NULL) {
        dma_size = sal_atoi(envstr);
    }
    if (dma_size == 0) {
        dma_size = PLI_DMA_SIZE;
    }
    if (dma_size > PLI_DMA_SIZE_MAX) {
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META_U(unit,
                             "Configured DMA pool size (%d MB) too big"
                             " - using %d MB instead\n"),
                  (int)dma_size, (int)PLI_DMA_SIZE_MAX));
        dma_size = PLI_DMA_SIZE_MAX;
    }
    LOG_INFO(BSL_LS_SYS_PCI,
             (BSL_META_U(unit,
                         "Requested DMA pool size is %d MB\n"),
              (int)dma_size));
    dma_size = dma_size * ONE_MB;

    /* Initialize DMA resources */
    sd->dma_pool = malloc(dma_size);
    if (sd->dma_pool == NULL) {
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META_U(unit,
                             "Unable to allocate DMA mpool.\n")));
        return -1;
    }
    sd->mpool = shr_mpool_create(sd->dma_pool, dma_size, 0);
    if (sd->mpool == NULL) {
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META_U(unit,
                             "Unable to create DMA mpool.\n")));
        return -1;
    }
    dma_offs = (unsigned long)sd->dma_pool;
    dma_offs &= ~0xffffffffUL;
    pci_dma_offset_set(unit, dma_offs);

    /* Initialize DMA structure */
    sal_memset(&dma, 0, sizeof(dma));

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
        return -1;
    }

    /* Initialize interrupt structure */
    sal_memset(&intr, 0, sizeof(intr));

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
        return -1;
    }

    /* Configure PCI */

    /*
     * For iProc-based devices we expect to read the CMIC base (in
     * iProc AXI space) from PCI BAR1.
     *
     * All access to iProc register (including the CMIC) will go
     * through PCI BAR0, so we need to reset that to zero.
     */
    tmp = pci_config_getw(dev, PCI_CONF_BAR1);
    if (tmp != 0 && tmp != 0xffffffff) {
        sd->cmic_base = tmp;
    } else {
        sd->cmic_base = 0;
    }

    /* Write base address */
    pci_config_putw(dev, PCI_CONF_BAR0, 0);

    /* Write something to int line */
    tmp = pci_config_getw(dev, PCI_CONF_ILINE);
    tmp = (tmp & ~0xff) | PLI_PCI_ILINE(sd);
    pci_config_putw(dev, PCI_CONF_ILINE, tmp);

    ndevs++;

    return 0;
}

/*!
 * \brief Disable PLI S-channel RPC.
 */
static void
schan_disable(void)
{
    use_schan_rpc = false;
}

/* List of configuration options and associate configuration functions */
static struct {
    const char *name;
    void (*func)(void);
} cfg_opt[] = {
    { "noschanrpc", schan_disable },
    { 0 }
};

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
        if (sd->dma_pool) {
            free(sd->dma_pool);
            sd->dma_pool = NULL;
        }
        sal_memset(sd, 0, sizeof(*sd));
    }

    /* Reset PCI to allow re-probing */
    pci_reset();
    ndevs = 0;

    return 0;
}

int
bcma_sys_probe(void)
{
    if (ndevs) {
        /* Already probed */
        return -1;
    }

    /* Install PLI S-channel operation hook */
    if (use_schan_rpc) {
        bcmbd_simhook_schan_op = pli_schan;
    }

    if (pci_device_iter(io_setup) < 0) {
        return -1;
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
    int idx;
    const char *cfg_name;

    if (config) {
        for (idx = 0; idx < COUNTOF(cfg_opt); idx++) {
            cfg_name = cfg_opt[idx].name;
            if (cfg_name && sal_strcmp(cfg_name, config) == 0) {
                cfg_opt[idx].func();
                return 0;
            }
        }
    }
    return -1;
}

const char *
bcma_sys_probe_config_get(const char *prev)
{
    int idx;

    if (prev == NULL) {
        return cfg_opt[0].name;
    }
    for (idx = 0; idx < COUNTOF(cfg_opt); idx++) {
        if (sal_strcmp(cfg_opt[idx].name, prev) == 0) {
            return cfg_opt[idx+1].name;
        }
    }
    return NULL;
}
