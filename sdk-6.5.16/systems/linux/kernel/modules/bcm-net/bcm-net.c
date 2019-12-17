/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

/*
 * This version of the BCM Linux network driver is written
 * to run on top of the linux-bcm-core module.
 *
 * All TX packets are tagged and all RX packets are assumed to
 * include a tag (which is stripped unconditionally.)
 *
 * It is possible to segment the switch ports into multiple network
 * devices by specifying port masks for each network device.
 *
 * For a list of supported module parameters, please see below.
 */

#include <gmodule.h> /* Must be included first */
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/random.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>

#include <soc/types.h>
#include <soc/enet.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/tx.h>
#include <bcm/rx.h>
#include <bcm/l2.h>
#include <bcm/stat.h>
#include <bcm/port.h>

#include <ibde.h>
#include <linux-bde.h>
#include <bcm-core.h>


MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("Network Device Driver for BCM Core devices");
MODULE_LICENSE("Proprietary");

static int unit;
LKM_MOD_PARAM(unit, "i", int, 0);
MODULE_PARM_DESC(unit,
"SOC unit to operate on (default 0)");

static int vlan = 1;
LKM_MOD_PARAM(vlan, "i", int, 0);
MODULE_PARM_DESC(vlan,
"VLAN ID used for transmission (default 1)");

static char *mac_addr = NULL;
LKM_MOD_PARAM(mac_addr, "s", charp, 0);
MODULE_PARM_DESC(mac_addr,
"Ethernet MAC address (default 02:10:18:xx:xx:xx)");

static int tx_buffers = 32;
LKM_MOD_PARAM(tx_buffers, "i", int, 0);
MODULE_PARM_DESC(tx_buffers,
"Number of preallocated TX packet buffers (default 32)");


/* Module Information */
#define MODULE_MAJOR 123
#define MODULE_NAME "linux-bcm-net"

/* Compatibility */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
static inline void *netdev_priv(struct net_device *dev)
{
        return dev->priv;
}
#endif /* KERNEL_VERSION(2,4,27) */

/*
 * Currently we cannot send packets from interrupt context because
 * bcm_l2_addr_get will try to sleep while doing S-Channel messaging.
 * To ensure we only send packets from process context, we pass all
 * packets to an internal thread, which handles the actual packet
 * transmissions.
 */
#define BNET_TX_THREAD


/* Local packet info */
typedef struct pkt_info_s {
    struct list_head list;

    unsigned char *pkt;
    bcm_pkt_t bcm_pkt;

    int count;
} pkt_info_t;

typedef struct {
    uint32_t pmask;
    struct net_device *ndev;
    struct net_device_stats stats; 
    pbmp_t pbmp;
} bnet_dev_t;

/* Default random MAC address has Broadcom OUI with local admin bit set */
static sal_mac_addr_t bnet_dev_mac = { 0x02, 0x10, 0x18, 0x00, 0x00, 0x00 };

static spinlock_t packet_lock;

/* 
 * Driver net device
 * 
 * By default a separate network device will be assigned to port 0
 * while the remaining ports will all belong to another network
 * device.
 */
static bnet_dev_t bnet_dev[] = {
    { 0x00000001 },     /* Port bitmask for 1st interface (ethX) */
    { 0xFFFFFFFE },     /* Port bitmask for 2nd interface (ethX+1) */
    { 0 }               /* Last entry must be zero */
};

/* Driver Proc Entry root */
static struct proc_dir_entry *bnet_proc_root = NULL;

/* Driver Proc Entry switch stats root */
static struct proc_dir_entry *stats_root = NULL;

struct bnet_priv_t {
    sal_sem_t tx_sem;
    volatile int tx_thread_running;
    struct sk_buff_head tx_queue;
    bnet_dev_t *bdev;
};

extern ibde_t *bde;

LIST_HEAD(_free_list);

static void
bnet_init_packets(void)
{
    int i;

    spin_lock_init(&packet_lock);

    if (tx_buffers < 0 || tx_buffers > 4096) {
	tx_buffers = 32;
    }	

    for (i = 0; i < tx_buffers; i++) {
	pkt_info_t *pkt = (pkt_info_t*)kmalloc(sizeof(*pkt), GFP_KERNEL);
	memset(pkt, 0, sizeof(*pkt));
	pkt->pkt = (unsigned char*) bde->salloc(0, 1600, "");
	list_add(&pkt->list, &_free_list);
    }
}

static pkt_info_t *
bnet_alloc_packet(void)
{
    pkt_info_t *rc = NULL;
    unsigned long flags;

    spin_lock_irqsave(&packet_lock, flags);
    if (!list_empty(&_free_list)) {
	rc = list_entry(_free_list.next, pkt_info_t, list);
	list_del(&rc->list);
	rc->count++;
    }
    spin_unlock_irqrestore(&packet_lock, flags);
    return rc;
}

static void
bnet_free_packet(pkt_info_t *pkt)
{
    unsigned long flags;

    spin_lock_irqsave(&packet_lock, flags);
    list_add(&pkt->list, &_free_list);
    spin_unlock_irqrestore(&packet_lock, flags);
}

static void
bnet_cleanup_packets(void)
{
    while(!list_empty(&_free_list)) {
	pkt_info_t *p;
	p = list_entry(_free_list.next, pkt_info_t, list);	
	list_del(&p->list);
	bde->sfree(0, p->pkt);
	kfree(p);
    }
}

/* 
 * Network Interface 
 */
static void bnet_init_bdev_pbmp(bnet_dev_t *bdev)
{
    int i;
    pbmp_t pbmp_all;

    BCM_PBMP_ASSIGN(pbmp_all, PBMP_PORT_ALL(unit));
    BCM_PBMP_CLEAR(bdev->pbmp);
    for (i = 0; i < 8*sizeof(bdev->pmask); i++) {
        if (((1 << i) & bdev->pmask) && BCM_PBMP_MEMBER(pbmp_all, i)) {
            BCM_PBMP_PORT_ADD(bdev->pbmp, i);
        }
    }
}

static bnet_dev_t *bnet_bdev_from_port(int port)
{
    int i;

    for (i = 0; bnet_dev[i].ndev; i++) {
        if ((1 << port) & bnet_dev[i].pmask) {
            return &bnet_dev[i];
        }
    }

    return NULL;
}

static bnet_dev_t *bnet_bdev_from_ndev(struct net_device *dev)
{
    return ((struct bnet_priv_t *)netdev_priv(dev))->bdev;
}

static int 
bnet_open(struct net_device *dev)
{
    netif_start_queue(dev);
    return 0;
}

static int
bnet_stop(struct net_device *dev)
{
    netif_stop_queue(dev);
    return 0;
}	

/* 
 * Network Device Statistics. 
 * Cleared at init time.  
 */
static struct net_device_stats *
bnet_get_stats(struct net_device *dev)
{
    bnet_dev_t *bdev = bnet_bdev_from_ndev(dev);

    if (bdev == NULL) {
        /* Unknown device */
        return NULL;
    }

    return &bdev->stats;
}

/* fake multicast ability */
static void
bnet_set_multicast_list(struct net_device *dev)
{
}

/* bcmx_pkt_cb_f:  cookie param must be ignored (rx only) */
static void 
bnet_tx_done(int ignored, bcm_pkt_t *packet, void *cookie)
{
    bnet_free_packet((pkt_info_t*)packet->cookie);
}	

static int 
bnet_tx_deferred(struct sk_buff *skb, struct net_device *dev)
{
    pkt_info_t *tx;
    bcm_l2_addr_t l2addr;
    pbmp_t pbmp;
    int rv;
    bnet_dev_t *bdev = bnet_bdev_from_ndev(dev);

    if (bdev == NULL) {
        /* Unknown device */
        return -1;
    }

    bdev->stats.tx_packets++;
    bdev->stats.tx_bytes += skb->len+4;
    tx = bnet_alloc_packet();

    if (tx) {
        bcm_pkt_t *pkt = &tx->bcm_pkt;

        memcpy(tx->pkt, skb->data, 12);
        tx->pkt[12] = 0x81;
        tx->pkt[13] = 0x00;
        tx->pkt[14] = (vlan >> 8) & 0xF;
        tx->pkt[15] = vlan & 0xFF;
        memcpy(tx->pkt+16, skb->data+12, skb->len-12);   

        /* Set up bcm packet */
        pkt->pkt_data = &pkt->_pkt_data;
        pkt->blk_count = 1;
        pkt->_pkt_data.data = (uint8 *)tx->pkt;
        pkt->_pkt_data.len = skb->len + 4;
        pkt->flags = BCM_TX_CRC_APPEND;
        pkt->call_back = bnet_tx_done;
        pkt->cookie = (void *)tx;

        rv = BCM_E_NOT_FOUND;
        if (!in_interrupt() && (tx->pkt[0] & 0x1) == 0) {
            bcm_l2_addr_t_init(&l2addr, tx->pkt, vlan);
            rv = bcm_l2_addr_get(unit, tx->pkt, vlan, &l2addr);
        }
        if (rv == BCM_E_NOT_FOUND) {
            BCM_PBMP_ASSIGN(pbmp, bdev->pbmp);
        } else {
            BCM_PBMP_PORT_SET(pbmp, l2addr.port);
        }
        BCM_PBMP_ASSIGN(pkt->tx_pbmp, pbmp);
        BCM_PBMP_ASSIGN(pkt->tx_upbmp, pbmp);

        if (bcm_tx(unit, &tx->bcm_pkt, NULL) < 0) {
            bdev->stats.tx_errors++;
            bnet_free_packet(tx);
        }
    }
    else {
        bdev->stats.tx_dropped++;
    }

    kfree_skb(skb);
    return 0;
}

/*
 * Currently we cannot send packets from interrupt context because
 * bcm_l2_addr_get will try to sleep while doing S-Channel messaging.
 * To avoid out of order transmissions, all packets are sent through
 * the queue, even if we are called from process context.
 */
static int 
bnet_tx(struct sk_buff *skb, struct net_device *dev)
{
#ifdef BNET_TX_THREAD
    struct bnet_priv_t *pd = (struct bnet_priv_t *)netdev_priv(bnet_dev[0].ndev);

    skb_queue_tail(&pd->tx_queue, skb);
    sal_sem_give(pd->tx_sem);
#else
    bnet_tx_deferred(skb, dev);
#endif
    return 0;
}

#ifdef BNET_TX_THREAD
static void
bnet_tx_thread(void *p)
{
    struct bnet_priv_t *pd = (struct bnet_priv_t *)netdev_priv(bnet_dev[0].ndev);
    struct sk_buff *skb;

#ifndef CONFIG_TIMESYS
    current->rt_priority = 50;
    current->policy = SCHED_RR;
#endif

    while (pd->tx_thread_running) {
        while ((skb = skb_dequeue(&pd->tx_queue)) != NULL) {
            bnet_tx_deferred(skb, skb->dev);
        }
        sal_sem_take(pd->tx_sem, sal_sem_FOREVER);
    }
}

static int 
bnet_tx_init(struct net_device *dev)
{
    struct bnet_priv_t *pd = (struct bnet_priv_t *)netdev_priv(bnet_dev[0].ndev);

    pd->tx_sem = sal_sem_create("bnetTX", 0, 0);
    if (!pd->tx_sem) {
        gprintk("Error creating thread semaphore.\n");
        return -1;
    }
    skb_queue_head_init(&pd->tx_queue);
    pd->tx_thread_running = 1;
    sal_thread_create("bnetTX", 0x1000, 50, bnet_tx_thread, dev);
    return 0;
}

static int 
bnet_tx_cleanup(struct net_device *dev)
{
    struct bnet_priv_t *pd = (struct bnet_priv_t *)netdev_priv(bnet_dev[0].ndev);
    struct sk_buff *skb;

    pd->tx_thread_running = 0;
    sal_sem_give(pd->tx_sem);
    sal_usleep(500000);
    sal_sem_destroy(pd->tx_sem);
    while ((skb = skb_dequeue(&pd->tx_queue)) != NULL) {
        kfree_skb(skb);
    }
    return 0;
}
#endif

static void
bnet_rx_deferred(void *packet_int, int src_port,
                 int reason, int pkt_size)
{
    struct sk_buff *skb;    
    unsigned char *pkt = (unsigned char*) packet_int;
    int i;
    int rx_vlan;
    bnet_dev_t *bdev = bnet_bdev_from_port(src_port);

    if (bdev == NULL) {
        /* Unknown port */
        return;
    }

    bdev->stats.rx_packets++;
    bdev->stats.rx_bytes += pkt_size; 

    if (ENET_MACADDR_MULTICAST(pkt)) {
	bdev->stats.multicast++;
    }

    rx_vlan = ((pkt[14] & 0x0F) << 8) + pkt[15];

    if (rx_vlan == vlan && (skb = dev_alloc_skb(pkt_size + 2)) != NULL) {
        skb->dev = bdev->ndev;
        skb_reserve(skb, 2);	/* 16 byte align the IP fields. */

        /* Drop the VLAN tag */
        for (i = 11; i >= 0; i--) {
            pkt[i+4] = pkt[i];
	}
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24))
        skb_copy_to_linear_data(skb, pkt+4, pkt_size-4);
#else
        eth_copy_and_sum(skb, pkt+4, pkt_size-4, 0);
#endif
        skb_put(skb, pkt_size);

        skb->protocol = eth_type_trans(skb, skb->dev);
        netif_rx (skb);
    }
    else {
        bdev->stats.rx_dropped++;
    }
}

static bcm_rx_t
bnet_rx(int ignored, bcm_pkt_t *pkt, void *cookie)
{
    COMPILER_REFERENCE(ignored);

    bnet_rx_deferred(pkt->pkt_data->data, pkt->rx_port, 0, pkt->pkt_len);
    return BCM_RX_HANDLED;
}	


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29))
static const struct net_device_ops bkn_netdev_ops = {
	.ndo_open		= bnet_open,
	.ndo_stop		= bnet_stop,
	.ndo_start_xmit		= bnet_tx,
	.ndo_get_stats		= bnet_get_stats,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_rx_mode	= bnet_set_multicast_list,
	.ndo_set_mac_address	= NULL,
	.ndo_do_ioctl		= NULL,
	.ndo_tx_timeout		= NULL,
	.ndo_change_mtu		= NULL,
};
#endif

static struct net_device *
bnet_init(sal_mac_addr_t mac, bnet_dev_t *bnet_dev)
{
    struct net_device *dev;
    bcm_l2_addr_t l2addr;

    /* 
     * Create the kernel ethernet device. 
     */
    dev = alloc_etherdev(sizeof(struct bnet_priv_t));
#ifdef SET_MODULE_OWNER
    SET_MODULE_OWNER(dev);
#endif

    if (dev == NULL) {
        gprintk("Error initializing ethernet device.\n");
        return NULL;
    }

    ((struct bnet_priv_t *)netdev_priv(dev))->bdev = bnet_dev;

    /* Set the device MAC address */
    memcpy(dev->dev_addr, mac, 6);

    /* Device information -- not available right now */
    dev->irq = 0;
    dev->base_addr = 0;

    /* Device vectors */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29))
    dev->netdev_ops = &bkn_netdev_ops;
#else
    dev->open = bnet_open;
    dev->hard_start_xmit = bnet_tx;
    dev->stop = bnet_stop;
    dev->set_multicast_list = bnet_set_multicast_list;
    dev->do_ioctl = NULL;
    dev->get_stats = bnet_get_stats;
#endif

    /* Register the kernel ethernet device */
    if (register_netdev(dev)) {
        gprintk("Error registering ethernet device.\n");
        _free_netdev(dev);
        return NULL;
    }

    /* Add ARL entry */
    bcm_l2_addr_t_init(&l2addr, mac, vlan);
    l2addr.flags = BCM_L2_STATIC;	
    l2addr.port = CMIC_PORT(unit);
    bcm_l2_addr_add(unit, &l2addr);

    return dev;
}

static int
bnet_destroy(struct net_device *dev)
{
    /* Delete ARL entry */
    bcm_l2_addr_delete(unit, dev->dev_addr, vlan);

    unregister_netdev(dev);
    _free_netdev(dev);

    return 0;
}

#define STAT(p, s) { \
    int _rv; \
    uint64 _v; \
    _rv = bcm_stat_get(unit, (p), (s), &_v); \
    if (_rv >= 0) { \
        seq_printf(m, "%s=%d\n", #s, COMPILER_64_LO(_v)); \
    } \
}

/*
 * Port Statistics Proc Entries
 */
static int 
bnet_proc_stat_show(struct seq_file *m, void *v)
{
    bcm_port_t port = (bcm_port_t)PTR_TO_INT(m->private);

    /*** RFC 1213 ***/

    STAT(port, snmpIfInOctets); 
    STAT(port, snmpIfInUcastPkts);
    STAT(port, snmpIfInNUcastPkts);
    STAT(port, snmpIfInDiscards);
    STAT(port, snmpIfInErrors);
    STAT(port, snmpIfInUnknownProtos);
    STAT(port, snmpIfOutOctets);
    STAT(port, snmpIfOutUcastPkts);
    STAT(port, snmpIfOutNUcastPkts);
    STAT(port, snmpIfOutDiscards);
    STAT(port, snmpIfOutErrors);
    STAT(port, snmpIpForwDatagrams);
    STAT(port, snmpIpInReceives);
    STAT(port, snmpIpInDiscards);

    /*** RFC 1493 ***/

    STAT(port, snmpDot1dBasePortDelayExceededDiscards);
    STAT(port, snmpDot1dBasePortMtuExceededDiscards);
    STAT(port, snmpDot1dTpPortInFrames);
    STAT(port, snmpDot1dTpPortOutFrames);
    STAT(port, snmpDot1dPortInDiscards);

    /*** RFC 1757 (EtherStat) ***/

    STAT(port, snmpEtherStatsDropEvents);
    STAT(port, snmpEtherStatsMulticastPkts);
    STAT(port, snmpEtherStatsBroadcastPkts);
    STAT(port, snmpEtherStatsUndersizePkts);
    STAT(port, snmpEtherStatsFragments);
    STAT(port, snmpEtherStatsPkts64Octets);
    STAT(port, snmpEtherStatsPkts65to127Octets);
    STAT(port, snmpEtherStatsPkts128to255Octets);
    STAT(port, snmpEtherStatsPkts256to511Octets);
    STAT(port, snmpEtherStatsPkts512to1023Octets);
    STAT(port, snmpEtherStatsPkts1024to1518Octets);
    STAT(port, snmpEtherStatsOversizePkts);
    STAT(port, snmpEtherStatsJabbers);
    STAT(port, snmpEtherStatsOctets);
    STAT(port, snmpEtherStatsPkts);
    STAT(port, snmpEtherStatsCollisions);
    STAT(port, snmpEtherStatsCRCAlignErrors);
    STAT(port, snmpEtherStatsTXNoErrors);
    STAT(port, snmpEtherStatsRXNoErrors);

    /*** RFC 2665 ***/

    STAT(port, snmpDot3StatsAlignmentErrors);
    STAT(port, snmpDot3StatsFCSErrors);
    STAT(port, snmpDot3StatsSingleCollisionFrames);
    STAT(port, snmpDot3StatsMultipleCollisionFrames);
    STAT(port, snmpDot3StatsSQETTestErrors);
    STAT(port, snmpDot3StatsDeferredTransmissions);
    STAT(port, snmpDot3StatsLateCollisions);
    STAT(port, snmpDot3StatsExcessiveCollisions);
    STAT(port, snmpDot3StatsInternalMacTransmitErrors);
    STAT(port, snmpDot3StatsCarrierSenseErrors);
    STAT(port, snmpDot3StatsFrameTooLongs);
    STAT(port, snmpDot3StatsInternalMacReceiveErrors);
    STAT(port, snmpDot3StatsSymbolErrors);

    /*** RFC 2233 ***/

    STAT(port, snmpIfHCInOctets);
    STAT(port, snmpIfHCInUcastPkts);
    STAT(port, snmpIfHCInMulticastPkts);
    STAT(port, snmpIfHCInBroadcastPkts);
    STAT(port, snmpIfHCOutOctets);
    STAT(port, snmpIfHCOutUcastPkts);
    STAT(port, snmpIfHCOutMulticastPkts);
    STAT(port, snmpIfHCOutBroadcastPckts);

    /*** Additional Broadcom stats ***/

    STAT(port, snmpBcmIPMCBridgedPckts);
    STAT(port, snmpBcmIPMCRoutedPckts);
    STAT(port, snmpBcmIPMCInDroppedPckts);
    STAT(port, snmpBcmIPMCOutDroppedPckts);

    return 0;
}

static int bnet_proc_stat_open(struct inode * inode, struct file * file) {
    return single_open(file, bnet_proc_stat_show, PROC_PDE_DATA(inode));

}
struct file_operations bnet_proc_stat_fops = {
    owner:      THIS_MODULE,
    open:       bnet_proc_stat_open,
    read:       seq_read,
    llseek:     seq_lseek,
    release:    single_release,
};

static int
bnet_proc_init(void)
{
    char scratch[64];
    pbmp_t pbm;
    bcm_port_t port;
    struct proc_dir_entry* entry;

    stats_root = proc_mkdir("stats", bnet_proc_root); 

    BCM_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
    PBMP_ITER(pbm, port) {
        sprintf(scratch, "port%d", port);
        PROC_CREATE_DATA(entry, scratch, 0, stats_root, &bnet_proc_stat_fops,
                         INT_TO_PTR(port));
        if (entry == NULL) {
            return -1;
        }
    }

    return 0;  
}

static int
bnet_proc_cleanup(void)
{
    char scratch[64];
    pbmp_t pbm;
    bcm_port_t port;

    BCM_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
    PBMP_ITER(pbm, port) {
        sprintf(scratch, "port%d", port); 
        remove_proc_entry(scratch, stats_root); 
    }

    remove_proc_entry("stats", bnet_proc_root); 
    return 0;
}

/*
 * Generic module functions
 */

static int
_pprint(void)
{	
    struct list_head *list;
    int i=0;

    pprintf("Broadcom BCM Core Linux Network Driver\n");
    pprintf("SOC Unit: %d\n", unit);
    pprintf("VLAN ID: %d\n", vlan);
    pprintf("Packet Buffer Usage Statistics (%d buffers):\n", tx_buffers);
    list_for_each(list, &_free_list) {
	pkt_info_t *pkt = (pkt_info_t*)list;
	pprintf("%3d: %.8d\n", i++, pkt->count);
    }

    /* put some goodies here */
    return 0;
}

static int
_init(void)
{
    char scratch[64];
    int i;
    struct net_device *dev;

    if (!soc_attached(unit)) {
        gprintk("Error: SOC device not attached.\n");
        return -1;
    }

    /* Initialize ether device processing */
    bnet_init_packets();

    /* Randomize Lower 3 bytes of the MAC address (TESTING ONLY) */
    get_random_bytes(&bnet_dev_mac[3], 3);

    /* Check for user-supplied MAC address (recommended) */
    if (mac_addr != NULL && strlen(mac_addr) == 17) {
        for (i = 0; i < 6; i++) {
            bnet_dev_mac[i] = simple_strtoul(&mac_addr[i*3], NULL, 16) & 0xFF;
        }
        /* Do not allow multicast address */
        bnet_dev_mac[0] &= ~0x01;
    }

    /* Initialize BCM driver */
    system_init(unit);

    bnet_init_bdev_pbmp(&bnet_dev[0]);
    if ((dev = bnet_init(bnet_dev_mac, &bnet_dev[0])) == NULL) {
        return -1;
    }
    bnet_dev[0].ndev = dev;

    /* Create virtual devices as needed */
    for (i = 1; bnet_dev[i].pmask; i++) {
        bnet_dev_mac[5]++;
        bnet_init_bdev_pbmp(&bnet_dev[i]);
        if ((dev = bnet_init(bnet_dev_mac, &bnet_dev[i])) == NULL) {
            return -1;
        }
        bnet_dev[i].ndev = dev;
    }

    bcm_rx_register(unit, "bcm-net", bnet_rx, BCM_RX_PRIO_MAX, NULL, 
                    BCM_RCO_F_ALL_COS | BCM_RCO_F_INTR);
    bcm_rx_start(unit, NULL);

    /* Initialize TX */
#ifdef BNET_TX_THREAD
    bnet_tx_init(bnet_dev[0].ndev);
#endif

    /* Initialize proc files */
    proc_mkdir("bcm", NULL);
    sprintf(scratch, "bcm/%s", bnet_dev[0].ndev->name); 
    bnet_proc_root = proc_mkdir(scratch, NULL);

    bnet_proc_init();
    return 0;
}

static int
_cleanup(void)
{
    char scratch[64];
    int i;

    bcm_rx_stop(unit, NULL);
    bcm_rx_unregister(unit, bnet_rx, BCM_RX_PRIO_MAX);
    bnet_cleanup_packets();
#ifdef BNET_TX_THREAD
    bnet_tx_cleanup(bnet_dev[0].ndev);
#endif

    bnet_proc_cleanup();
    sprintf(scratch, "bcm/%s", bnet_dev[0].ndev->name);
    remove_proc_entry(scratch, NULL);
    remove_proc_entry("bcm", NULL);

    for (i = 0; bnet_dev[i].ndev; i++) {
        bnet_destroy(bnet_dev[i].ndev);
    }

    return 0;
}	

static gmodule_t _gmodule = {
    name: MODULE_NAME, 
    major: MODULE_MAJOR, 
    init: _init,
    cleanup: _cleanup, 
    pprint: _pprint, 
    ioctl: NULL,
    open: NULL, 
    close: NULL, 
}; 

gmodule_t*
gmodule_get(void)
{
    EXPORT_NO_SYMBOLS;
    return &_gmodule;
}
