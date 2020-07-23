/*! \file ngknet_main.h
 *
 * Data structure and macro definitions for NGKNET kernel module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGKNET_MAIN_H
#define NGKNET_MAIN_H

#include <linux/netdevice.h>
#include <lkm/lkm.h>
#include <lkm/ngknet_dev.h>
#include <bcmcnet/bcmcnet_core.h>

/*! Maximum number of PDMA devices supported */
#ifdef NGBDE_NUM_SWDEV_MAX
#define NUM_PDMA_DEV_MAX    NGBDE_NUM_SWDEV_MAX
#else
#define NUM_PDMA_DEV_MAX    16
#endif

/*!
 * Debug levels
 */
#define DBG_LVL_VERB        0x0001
#define DBG_LVL_PKT         0x0002
#define DBG_LVL_CMD         0x0004
#define DBG_LVL_IRQ         0x0008
#define DBG_LVL_NAPI        0x0010
#define DBG_LVL_NDEV        0x0020
#define DBG_LVL_FILT        0x0040
#define DBG_LVL_RCPU        0x0080
#define DBG_LVL_WARN        0x0100
#define DBG_LVL_PDMP        0x0200
#define DBG_LVL_RATE        0x0400
#define DBG_LVL_LINK        0x0800

#define DBG_VERB(_s)        do { if (debug & DBG_LVL_VERB) printk _s; } while (0)
#define DBG_PKT(_s)         do { if (debug & DBG_LVL_PKT)  printk _s; } while (0)
#define DBG_CMD(_s)         do { if (debug & DBG_LVL_CMD)  printk _s; } while (0)
#define DBG_IRQ(_s)         do { if (debug & DBG_LVL_IRQ)  printk _s; } while (0)
#define DBG_NAPI(_s)        do { if (debug & DBG_LVL_NAPI) printk _s; } while (0)
#define DBG_NDEV(_s)        do { if (debug & DBG_LVL_NDEV) printk _s; } while (0)
#define DBG_FILT(_s)        do { if (debug & DBG_LVL_FILT) printk _s; } while (0)
#define DBG_RCPU(_s)        do { if (debug & DBG_LVL_RCPU) printk _s; } while (0)
#define DBG_WARN(_s)        do { if (debug & DBG_LVL_WARN) printk _s; } while (0)
#define DBG_PDMP(_s)        do { if (debug & DBG_LVL_PDMP) printk _s; } while (0)
#define DBG_RATE(_s)        do { if (debug & DBG_LVL_RATE) printk _s; } while (0)
#define DBG_LINK(_s)        do { if (debug & DBG_LVL_LINK) printk _s; } while (0)

/*!
 * Device description
 */
struct ngknet_dev {
    /*! Base address for PCI register access */
    volatile void *base_addr;

    /*! Required for DMA memory control */
    struct device *dev;

    /*! Required for PCI memory control */
    struct pci_dev *pci_dev;

    /*! Base network device */
    struct net_device *net_dev;

    /*! PDMA device */
    struct pdma_dev pdma_dev;

    /*! Device number (from BDE) */
    int dev_no;

    /*! Vitual network devices, 0 is reserved */
    struct net_device *vdev[NUM_VDEV_MAX + 1];

    /*! Vitual network devices bound to queue */
    struct net_device *bdev[NUM_QUE_MAX];

    /*! Filter list */
    struct list_head filt_list;

    /*! Filter control, 0 is reserved */
    void *fc[NUM_FILTER_MAX + 1];

    /*! Callback control */
    struct ngknet_callback_ctrl *cbc;

    /*! RCPU control */
    struct ngknet_rcpu_hdr rcpu_ctrl;

    /*! NGKNET lock */
    spinlock_t lock;

    /*! NGKNET wait queue */
    wait_queue_head_t wq;

    /*! VNET wait queue */
    wait_queue_head_t vnet_wq;

    /*! VNET is active */
    atomic_t vnet_active;

    /*! HNET wait queue */
    wait_queue_head_t hnet_wq;

    /*! HNET is active */
    atomic_t hnet_active;

    /*! HNET deamon */
    struct task_struct *hnet_task;

    /*! HNET work */
    struct work_struct hnet_work;

    /*! Flags */
    int flags;
    /*! NGKNET device is active */
#define NGKNET_DEV_ACTIVE      (1 << 0)
};

/*!
 * Network interface specific private data
 */
struct ngknet_private {
    /*! Network device */
    struct net_device *net_dev;

    /*! Network stats */
    struct net_device_stats stats;

    /*! NGKNET device */
    struct ngknet_dev *bkn_dev;

    /*! Network interface ID */
    int id;

    /*! Network interface type */
    int type;

    /*! Network interface flags */
    uint32_t flags;

    /*! Network interface vlan */
    uint32_t vlan;

    /*! Network interface bound to */
    uint32_t chan;

    /*! Metadata offset from Ethernet header */
    uint32_t meta_off;

    /*! Metadata length */
    uint32_t meta_len;

    /*! Metadata used to send packets to physical port */
    uint8_t meta_data[NGKNET_NETIF_META_MAX];

    /*! User data gotten back through callbacks */
    uint8_t user_data[NGKNET_NETIF_USER_DATA];

    /*! Users of this network interface */
    int users;

    /*! Wait for this network interface free */
    int wait;
};

/*!
 * \brief Create network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_netif_create(struct ngknet_dev *dev, ngknet_netif_t *netif);

/*!
 * \brief Destroy network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] id Network interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_netif_destroy(struct ngknet_dev *dev, int id);

/*!
 * \brief Get network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [in] id Network interface ID.
 * \param [out] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_netif_get(struct ngknet_dev *dev, int id, ngknet_netif_t *netif);

/*!
 * \brief Get the next network interface.
 *
 * \param [in] dev NGKNET device structure point.
 * \param [out] netif Network interface structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
ngknet_netif_get_next(struct ngknet_dev *dev, ngknet_netif_t *netif);

/*!
 * \brief Get debug level.
 *
 * \retval Current debug level.
 */
extern int
ngknet_debug_level_get(void);

/*!
 * \brief Set debug level.
 *
 * \param [in] debug_level Debug level to be set.
 */
extern void
ngknet_debug_level_set(int debug_level);

/*!
 * \brief Get Rx rate limit.
 *
 * \retval Current Rx rate limit.
 */
extern int
ngknet_rx_rate_limit_get(void);

/*!
 * \brief Set Rx rate limit.
 *
 * \param [in] rate_limit Rx rate limit to be set.
 */
extern void
ngknet_rx_rate_limit_set(int rate_limit);

#endif /* NGKNET_MAIN_H */

