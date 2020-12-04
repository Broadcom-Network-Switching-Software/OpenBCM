/*! \file bcmlu_ngbde.c
 *
 * Linux user mode connector for kernel mode BDE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

/* Include sys/types.h before ngbde_ioctl.h to avoid type conflicts */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <lkm/ngbde_ioctl.h>
#include <bcmlu/bcmlu_mmap.h>
#include <bcmlu/bcmlu_ngbde.h>

#define DEV_FNAME       "/dev/linux_ngbde"
#define DEV_FMODE       (O_RDWR | O_SYNC)

#define MOD_FNAME       "linux_ngbde.ko"

static int devfd = -1;

static int
dev_open(void)
{
    char *shcmd;

    if (devfd >= 0) {
        /* Already open */
        return 0;
    }

    devfd = open(DEV_FNAME, DEV_FMODE);
    if (devfd >= 0) {
        /* Successfully opened */
        return 0;
    }

    /* If device file cannot be opened, the try to create it */
    shcmd = "f=" DEV_FNAME "; if [ ! -e $f ]; then mknod $f c 120 0; fi";
    if (system(shcmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to create device node %s.\n"),
                   DEV_FNAME));
        return -1;
    }
    /* If successfully created, try opening device file again */
    devfd = open(DEV_FNAME, DEV_FMODE);
    if (devfd >= 0) {
        /* Successfully opened */
        return 0;
    }

    /* If device file still cannot be opened, the try loading BDE module */
    shcmd = "/sbin/insmod " MOD_FNAME;
    if (system(shcmd) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to load kernel module %s.\n"),
                   MOD_FNAME));
        return -1;
    }

    /* If BDE module loaded successfully, try opening device file again */
    devfd = open(DEV_FNAME, DEV_FMODE);
    if (devfd >= 0) {
        /* Successfully opened */
        return 0;
    }

    /* Give up */
    LOG_ERROR(BSL_LS_SYS_PCI,
              (BSL_META("Unable to open BDE device.\n")));
    return -1;
}

static int
dev_close(void)
{
    if (devfd >= 0) {
        close(devfd);
        devfd = -1;
    }
    return 0;
}

static int
do_ioctl(int cmd, struct ngbde_ioc_cmd_s *ioc)
{
    if (ioctl(devfd, cmd, ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("BDE IOCTL command %d failed.\n"), cmd));
        return -1;
    }
    return 0;
}

int
bcmlu_ngbde_init(void)
{
    struct ngbde_ioc_cmd_s ioc;

    if (dev_open() < 0) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    if (do_ioctl(NGBDE_IOC_MOD_INFO, &ioc) < 0) {
        return -1;
    }
    if (ioc.op.mod_info.version != NGBDE_IOC_VERSION) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Kernel BDE version mismatch.\n")));
        dev_close();
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_cleanup(void)
{
    dev_close();
    return 0;
}

int
bcmlu_ngbde_num_dev_get(int *ndev)
{
    struct ngbde_ioc_cmd_s ioc;

    if (ndev == NULL) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    if (do_ioctl(NGBDE_IOC_PROBE_INFO, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    *ndev = ioc.op.probe_info.num_swdev;

    return ioc.rc;
}

int
bcmlu_ngbde_dev_info_get(int devid, bcmlu_dev_info_t *di)
{
    struct ngbde_ioc_cmd_s ioc;
    int idx;

    if (di == NULL) {
        return -1;
    }

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    if (do_ioctl(NGBDE_IOC_DEV_INFO, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    di->type = ioc.op.dev_info.type;
    di->vendor_id = ioc.op.dev_info.vendor_id;
    di->device_id = ioc.op.dev_info.device_id;
    di->revision = ioc.op.dev_info.revision;
    if (ioc.op.dev_info.flags & NGBDE_DEV_F_MSI) {
        di->flags |= BCMLU_DEV_F_MSI;
    }

    for (idx = 0; idx < 2; idx++) {
        memset(&ioc, 0, sizeof(ioc));
        ioc.devid = devid;
        ioc.op.rsrc_id.type = NGBDE_IO_RSRC_DEV_IO;
        ioc.op.rsrc_id.inst = idx;
        if (do_ioctl(NGBDE_IOC_PHYS_ADDR, &ioc) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META("Unable to connect to kernel BDE.\n")));
            return -1;
        }
        di->iowin[idx].addr = ioc.op.phys_addr.addr;
        di->iowin[idx].size = ioc.op.phys_addr.size;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_dma_info_get(int devid, bcmlu_dma_info_t *dmi)
{
    struct ngbde_ioc_cmd_s ioc;

    if (dmi == NULL) {
        return -1;
    }

    /* Get raw physical address */
    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.rsrc_id.type = NGBDE_IO_RSRC_DMA_MEM;
    if (do_ioctl(NGBDE_IOC_PHYS_ADDR, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    dmi->addr = ioc.op.phys_addr.addr;
    dmi->size = ioc.op.phys_addr.size;

    /* Get bus address (same as physical if no IOMMU) */
    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.rsrc_id.type = NGBDE_IO_RSRC_DMA_BUS;
    if (do_ioctl(NGBDE_IOC_PHYS_ADDR, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }
    if (ioc.rc != 0) {
        dmi->baddr = dmi->addr;
        LOG_WARN(BSL_LS_SYS_PCI,
                 (BSL_META("Kernel modules out-of-date. DMA may not work.\n")));
        return 0;
    }
    dmi->baddr = ioc.op.phys_addr.addr;
    if (dmi->size != ioc.op.phys_addr.size) {
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_connect(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_CONN;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_disconnect(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_DISC;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_wait(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_WAIT;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_stop(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_INTR_STOP;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_regs_clr(int devid, int irq_num)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ctrl.irq_num = irq_num;
    ioc.op.intr_ctrl.cmd = NGBDE_ICTL_REGS_CLR;
    if (do_ioctl(NGBDE_IOC_INTR_CTRL, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_reg_add(int devid, int irq_num, uint32_t status_reg,
                         uint32_t mask_reg, uint32_t kmask)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.irq_reg_add.irq_num = irq_num;
    ioc.op.irq_reg_add.status_reg = status_reg;
    ioc.op.irq_reg_add.mask_reg = mask_reg;
    ioc.op.irq_reg_add.kmask = kmask;
    if (do_ioctl(NGBDE_IOC_IRQ_REG_ADD, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_ack_reg_add(int devid, int irq_num, uint32_t ack_reg,
                             uint32_t ack_val, uint32_t flags)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.intr_ack_reg_add.irq_num = irq_num;
    ioc.op.intr_ack_reg_add.ack_reg = ack_reg;
    ioc.op.intr_ack_reg_add.ack_val = ack_val;
    if (flags & BCMLU_INTR_ACK_F_PAXB) {
        ioc.op.intr_ack_reg_add.flags |= NGBDE_DEV_INTR_ACK_F_PAXB;
    }

    if (do_ioctl(NGBDE_IOC_INTR_ACK_REG_ADD, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_intr_mask_write(int devid, int irq_num,
                            uint32_t offs, uint32_t val)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.irq_mask_wr.irq_num = irq_num;
    ioc.op.irq_mask_wr.offs = offs;
    ioc.op.irq_mask_wr.val = val;
    if (do_ioctl(NGBDE_IOC_IRQ_MASK_WR, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_pio_map(int devid, uint64_t addr, uint32_t size)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.pio_win.addr = addr;
    ioc.op.pio_win.size = size;
    if (do_ioctl(NGBDE_IOC_PIO_WIN_MAP, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_iio_map(int devid, uint64_t addr, uint32_t size)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.pio_win.addr = addr;
    ioc.op.pio_win.size = size;
    if (do_ioctl(NGBDE_IOC_IIO_WIN_MAP, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

int
bcmlu_ngbde_paxb_map(int devid, uint64_t addr, uint32_t size)
{
    struct ngbde_ioc_cmd_s ioc;

    memset(&ioc, 0, sizeof(ioc));
    ioc.devid = devid;
    ioc.op.pio_win.addr = addr;
    ioc.op.pio_win.size = size;
    if (do_ioctl(NGBDE_IOC_PAXB_WIN_MAP, &ioc) < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Unable to connect to kernel BDE.\n")));
        return -1;
    }

    return ioc.rc;
}

void *
bcmlu_ngbde_mmap(uint64_t offset, size_t size)
{
    dev_open();

    return bcmlu_mmap(devfd, offset, size);
}

int
bcmlu_ngbde_munmap(void *addr, size_t size)
{
    return bcmlu_munmap(addr, size);
}
