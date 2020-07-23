/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include "eth_drv.h"

#define ETH_DRV_MAX_BUF_LEN     2048

#if defined(VXWORKS) && (VX_VERSION > 54) && !defined(ICS)

#ifdef ETH_DRV_SUPPORTED
#error "ethernet driver redefined for VxWorks"
#endif /* ETH_DRV_SUPPORTED */
#define ETH_DRV_SUPPORTED       1

#define ETH_DRV_THREAD_POLLING  0

#include <sys/types.h>
#if VX_VERSION == 69
#include <types/vxWind.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <vxWorks.h>
#include <taskLib.h>
#include <tickLib.h>
#include <stdlib.h>
#include <stdio.h>
#include "mux_drv.h"

typedef struct eth_unit_info_s {
    int         inited;
    rx_cb       rx_cb;
    void        *cookie;
    int         started;
} eth_unit_info_t;

#ifdef MBZ
#define ETH_TX_DEVICE_NAME "et"
#else
#ifdef BMW
#define ETH_TX_DEVICE_NAME "bc"
#else
#ifdef MPC8548
#define ETH_TX_DEVICE_NAME "motetsec"
#else
#define ETH_TX_DEVICE_NAME "unknown"
#endif
#endif
#endif

static eth_unit_info_t  *eth_control[ETH_MAX_UNITS];

static void _eth_drv_handle_rx(int unit, void *pkt, int pkt_len);

static int _eth_drv_init(int unit)
{
    if (eth_control[unit]) {
        return 0;
    }

    eth_control[unit] = malloc(sizeof(eth_unit_info_t));
    memset(eth_control[unit], 0, sizeof(eth_unit_info_t));

    if (et_drvTxRxInit(ETH_TX_DEVICE_NAME, unit)) {
        free(eth_control[unit]);
        eth_control[unit] = NULL;
        return -1;
    }
    eth_control[unit]->inited++;
    return 0;
}

static int
_eth_drv_rx_thread(void *p)
{
    int    unit = (int)p, plen;
    unsigned char * rx_pkt;

    if ((rx_pkt = malloc(ETH_DRV_MAX_BUF_LEN)) == NULL) {
        return -1;
    }
    et_drv_start(unit);
    while (eth_control[unit]->started) {
        et_drv_mii_rx(unit, rx_pkt, &plen);
        if (plen) {
            _eth_drv_handle_rx(unit, rx_pkt, plen);
        } else {
            taskDelay(10);
        }
    }

    if (rx_pkt) {
        free(rx_pkt);
    }
    return 0;
}

static int
_eth_drv_start(int unit)
{
    sigset_t	new_mask, orig_mask;

    if (eth_control[unit] == NULL) {
        printf("ETH DRV : Driver not initialized!!\n");
        return -1;
    }

    if (eth_control[unit]->started == 0) {
        eth_control[unit]->started++;
        if (ETH_DRV_THREAD_POLLING) {
            /* Start RX poll task */
            sigemptyset(&new_mask);
            sigaddset(&new_mask, SIGINT);
            sigprocmask(SIG_BLOCK, &new_mask, &orig_mask);
        
            if (taskSpawn("eth_drv_rx_thread", 50, 0,
                       16386, _eth_drv_rx_thread,
                       unit, 0, 0, 0, 0, 0, 0, 0, 0, 0) == ERROR) {
                eth_control[unit]->started = 0;
                printf("eth_drv : Failed to start RX task!!\n");
                return -1;
            }
            sigprocmask(SIG_SETMASK, &orig_mask, NULL);
        }
    } else {
        eth_control[unit]->started++;
    }

    return 0;

}

static int
_eth_drv_stop(int unit)
{
    if ((eth_control[unit] == NULL) || (!eth_control[unit]->started)) {
        return 0;
    }

    if (--eth_control[unit]->started == 0) {
        if (ETH_DRV_THREAD_POLLING) {
            et_drv_stop(unit);
        }
    }
    return 0;
}

static int
_eth_drv_tx(int unit, unsigned char *tx_pkt, int pkt_len, void *cookie)
{
    return et_drvTx(unit, tx_pkt, pkt_len, cookie);
}

static int 
_eth_drv_get_mac(int unit, unsigned char *mac)
{
    return et_drv_mac_addr_get(unit, mac);
}

#endif /* defined(VXWORKS) && (VX_VERSION > 54) && !defined(ICS) */

#if defined(LINUX) && !defined(KERNEL) && !defined(ICS)

#ifdef ETH_DRV_SUPPORTED
#error "ethernet driver redefined for Linux"
#endif /* ETH_DRV_SUPPORTED */
#define ETH_DRV_SUPPORTED       1

#include <sys/socket.h>
#include <sys/types.h>
#include <features.h>    /* for the glibc version number */
#include <netpacket/packet.h>
#include <net/ethernet.h>     /* the L2 protocols */
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <sal/appl/sal.h>

typedef struct eth_unit_info_s {
    int         sock_fd;
    struct      sockaddr_ll sl;
    struct ifreq ifr;

    int         unit;
    int         inited;
    rx_cb       rx_cb;
    void        *cookie;
    int         started;
} eth_unit_info_t;

#define ETH_MAX_UNITS   1
static eth_unit_info_t  *eth_control[ETH_MAX_UNITS];
static void _eth_drv_handle_rx(int unit, void *pkt, int pkt_len);

static void
_eth_drv_rx_thread(void *p)
{
    struct sockaddr_ll sl;
    int    unit, pkt_len;
    uint32 fromlen = sizeof(struct sockaddr_ll);
    int    rv, sock_fd;
    char * rx_pkt;
    fd_set fds;
    struct timeval timeout;

    unit = *(int *)p;
    sock_fd = eth_control[unit]->sock_fd;

    if ((rx_pkt = malloc(ETH_DRV_MAX_BUF_LEN)) == NULL) {
        return;
    }

    while (1) {
        if (!eth_control[unit]->started) {
            break;
        }
        FD_ZERO(&fds);
        FD_SET(sock_fd, &fds);

        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        rv = select(sock_fd + 1, &fds, NULL, NULL, &timeout);

        if (rv < 0) {
            /* error case */
            break;
        } else if (rv == 0) {
            /* timeout */
            continue;
        } else {
            if (FD_ISSET(sock_fd, &fds)) {
                pkt_len = recvfrom(sock_fd, rx_pkt, ETH_DRV_MAX_BUF_LEN, 0,
                                  (struct sockaddr *)&sl, &fromlen);
                _eth_drv_handle_rx(unit, rx_pkt, pkt_len);
            }
        }
    }

    if (rx_pkt) {
        free(rx_pkt);
    }
}

static int _eth_drv_init(int unit)
{
    eth_unit_info_t     *channel;
    int                  result;

    if (eth_control[unit]) {
        return 0;
    }

    eth_control[unit] = malloc(sizeof(eth_unit_info_t));
    memset(eth_control[unit], 0, sizeof(eth_unit_info_t));
    channel = eth_control[unit];

    channel->sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (channel->sock_fd == 0) {
        goto error;
    }

    sprintf(channel->ifr.ifr_name, "eth%d", unit);
    result = ioctl(channel->sock_fd, SIOCGIFINDEX, &channel->ifr);
    if (result) {
        goto error;
    }

    channel->sl.sll_family = AF_PACKET;
    channel->sl.sll_ifindex = channel->ifr.ifr_ifindex;
    channel->sl.sll_protocol = htons(ETH_P_ALL);
    channel->sl.sll_halen = 6;

    if (bind(channel->sock_fd, (struct sockaddr *)&channel->sl, 
         sizeof(struct sockaddr_ll))) {
        goto error;
    }

    eth_control[unit]->inited++;
    return 0;
error:
    free(eth_control[unit]);
    eth_control[unit] = NULL;
    return -1;
}

static int
_eth_drv_start(int unit)
{
    void *ptr;
    
    if ((eth_control[unit] == NULL) || (!eth_control[unit]->rx_cb)) {
        return -1;
    }

    if (eth_control[unit]->started == 0) {

        /*
         * Start an RX thread to poll for packets over socket interface.
         */
        eth_control[unit]->started++;
        eth_control[unit]->unit = unit;
        ptr = (void *)&eth_control[unit]->unit;
        sal_thread_create("eth_drv_rx_thread", 
                              SAL_THREAD_STKSZ,
                              10,
                              _eth_drv_rx_thread,
                              ptr);
    } else {
        eth_control[unit]->started++;
    }
    return 0;
}

static int
_eth_drv_stop(int unit)
{
    if ((eth_control[unit] == NULL) || (!eth_control[unit]->started)) {
        return 0;
    }

    --eth_control[unit]->started;
    return 0;
}

static int
_eth_drv_tx(int unit, unsigned char *tx_pkt, int pkt_len, void *cookie)
{
    eth_unit_info_t * p_chan = eth_control[unit];

    return (send(p_chan->sock_fd, tx_pkt, pkt_len, 0)
            == pkt_len) ? 0 : -1;
 
}

static int 
_eth_drv_get_mac(int unit, unsigned char *mac)
{
    eth_unit_info_t     *channel;
    int                  result;

    if (!eth_control[unit]) {
        return -1;
    }

    channel = eth_control[unit];

    result = ioctl(channel->sock_fd, SIOCGIFHWADDR, &channel->ifr);
    if (result) {
        return -1;
    }

    memcpy(mac, channel->ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    return 0;
}

#endif /* defined(LINUX) && !defined(KERNEL) && !defined(ICS) */

/********* APIs **********/

#ifdef ETH_DRV_SUPPORTED
static void 
_eth_drv_handle_rx(int unit, void *pkt, int pkt_len)
{
    if (eth_control[unit] && eth_control[unit]->rx_cb) {
        eth_control[unit]->rx_cb(unit,
                                 (unsigned char*) pkt, pkt_len, 
                                 eth_control[unit]->cookie);
    }
    return;
}

int eth_drv_init(int unit)
{
    if (unit >= ETH_MAX_UNITS) {
        return -1;
    }
    return _eth_drv_init(unit);
}

int eth_drv_register(int unit, rx_cb _rx, void *cookie)
{
    int rv = 0;

    if (unit >= ETH_MAX_UNITS) {
        return -1;
    }

    if (eth_control[unit] == NULL) {
        rv =  _eth_drv_init(unit);      
        if (rv < 0) {
            printf("ETH_DRV : error (driver initialize failed!)\n");
            return -1;
        }
    }

    eth_control[unit]->rx_cb = _rx;
    eth_control[unit]->cookie = cookie;
#if defined(VXWORKS)
    et_drvMIIRxHandlerRegister(unit, (cb_f)_rx, NULL, NULL, cookie);
#endif

    if (eth_control[unit]->started == 0) {
        rv = _eth_drv_start(unit);
    }

    return rv;
}

int eth_drv_unregister(int unit, rx_cb _rx)
{
    if (unit >= ETH_MAX_UNITS) {
        return -1;
    }
    if (eth_control[unit] == NULL) {
        printf("ETH_DRV : error (driver not initialized !!)\n");
        return -1;
    }
    eth_control[unit]->rx_cb = NULL;
#if defined(VXWORKS)
    et_drvMIIRxHandlerUnRegister(unit, (cb_f)_rx);
#endif

    if (eth_control[unit]->started) {
        _eth_drv_stop(unit);
    }

    return 0;
}

int eth_drv_start(int unit)
{
    return _eth_drv_start(unit);
}

int eth_drv_stop(int unit)
{
    return _eth_drv_stop(unit);
}


int eth_drv_tx(int unit, unsigned char * pkt, int len, void *cookie)
{
    return _eth_drv_tx(unit, pkt, len, cookie);
}


int eth_drv_get_mac(int unit, unsigned char *mac)
{
    return _eth_drv_get_mac(unit, mac);
}

#else

int eth_drv_init(int unit)
{
    return -1;
}

int eth_drv_register(int unit, rx_cb _rx, void *cookie)
{
    return -1;
}

int eth_drv_start(int unit)
{
    return -1;
}

int eth_drv_stop(int unit)
{
    return -1;
}


int eth_drv_tx(int unit, unsigned char * pkt, int len, void *cookie)
{
    return -1;
}


int eth_drv_get_mac(int unit, unsigned char *mac)
{
    return -1;
}

#endif /* ETH_DRV_SUPPORTED */

