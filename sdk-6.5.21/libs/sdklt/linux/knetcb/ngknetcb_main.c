/*! \file ngknetcb_main.c
 *
 * NGKNET Callback module entry.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <lkm/lkm.h>
#include <ngknet_callback.h>

/*! \cond */
MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("NGKNET Callback Module");
MODULE_LICENSE("GPL");
/*! \endcond */

/*! Module information */
#define NGKNETCB_MODULE_NAME    "linux_ngknetcb"
#define NGKNETCB_MODULE_MAJOR   122

static void
show_pmd(uint8_t *pmd, int len)
{
    int i;

    printk("PMD (%d bytes):\n", len);
    for (i = 0; i < len; i++) {
        printk("%02X ", pmd[i]);
        if ((i + 1) % 32 == 0) {
            printk("\n");
        }
    }
    if (len % 32) {
        printk("\n");
    }
}

static void
show_mac(uint8_t *pkt)
{
    printk("DMAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
           pkt[0], pkt[1], pkt[2], pkt[3], pkt[4], pkt[5]);
}

static struct sk_buff *
test_rx_cb(struct sk_buff *skb)
{
    struct ngknet_callback_desc *cbd = NGKNET_SKB_CB(skb);

    printk("rx_cb for dev %d: %s\n", cbd->dev_no, cbd->type_str);
    show_pmd(cbd->pmd, cbd->pmd_len);
    show_mac(cbd->pmd + cbd->pmd_len);
    printk("netif user data: 0x%08x\n", *(uint32_t *)cbd->priv->user_data);
    if (cbd->filt) {
        printk("filter user data: 0x%08x\n", *(uint32_t *)cbd->filt->user_data);
    }

    return skb;
}

static struct sk_buff *
test_tx_cb(struct sk_buff *skb)
{
    struct ngknet_callback_desc *cbd = NGKNET_SKB_CB(skb);

    printk("tx_cb for dev %d: %s\n", cbd->dev_no, cbd->type_str);
    show_pmd(cbd->pmd, cbd->pmd_len);
    show_mac(cbd->pmd + cbd->pmd_len);

    return skb;
}

/*!
 * Generic module functions
 */

static int
ngknetcb_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int
ngknetcb_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static long
ngknetcb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static int
ngknetcb_mmap(struct file *filp, struct vm_area_struct *vma)
{
    return 0;
}

static struct file_operations ngknetcb_fops = {
    .open = ngknetcb_open,
    .release = ngknetcb_release,
    .unlocked_ioctl = ngknetcb_ioctl,
    .compat_ioctl = ngknetcb_ioctl,
    .mmap = ngknetcb_mmap,
};

static int __init
ngknetcb_init_module(void)
{
    int rv;

    rv = register_chrdev(NGKNETCB_MODULE_MAJOR, NGKNETCB_MODULE_NAME, &ngknetcb_fops);
    if (rv < 0) {
        printk(KERN_WARNING "%s: can't get major %d\n",
               NGKNETCB_MODULE_NAME, NGKNETCB_MODULE_MAJOR);
        return rv;
    }

    ngknet_rx_cb_register(test_rx_cb);
    ngknet_tx_cb_register(test_tx_cb);

    return 0;
}

static void __exit
ngknetcb_exit_module(void)
{
    ngknet_rx_cb_unregister(test_rx_cb);
    ngknet_tx_cb_unregister(test_tx_cb);

    unregister_chrdev(NGKNETCB_MODULE_MAJOR, NGKNETCB_MODULE_NAME);
}

module_init(ngknetcb_init_module);
module_exit(ngknetcb_exit_module);
