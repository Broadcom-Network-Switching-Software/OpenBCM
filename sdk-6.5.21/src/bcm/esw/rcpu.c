/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Remote CPU module for XGS devices.
 */

/* We need to call top-level APIs on other units */
#ifdef BCM_HIDE_DISPATCHABLE
#undef BCM_HIDE_DISPATCHABLE
#endif

#include <shared/bsl.h>

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/mem.h>
#include <soc/pbsmh.h>
#include <soc/higig.h>

#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/tx.h>

#include <bcm_int/control.h>
#include <bcm_int/esw/rcpu.h>
#include <bcm_int/esw/rx.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/tx.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)

#include <bcm_int/esw/port.h>

#ifdef BCM_TRX_SUPPORT
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_OOB_RCPU_SUPPORT)
#include <drv/eth/eth_drv.h>
#endif /* BCM_OOB_RCPU_SUPPORT */

/* Number of module header bytes inserted after CMIC header for FromCPU packet */
#define FROM_CPU_DCB_SIZE    (32)

/* Number of packet retransmission for S-channel oprations */
#define MAX_RETRY_ATTEMPTS    (3)

/* 
 * Define this if you want to know the time take to prcess RCPU s-channel request.
 */
#define BCM_RCPU_BENCHMARK    (1)

#define RCPU_HDR_F_SOBMH     0x01
#define RCPU_HDR_F_HIGIG     0x02

#define RCPU_COPY_HDR(b, s, l, inc_buf_ptr)     \
           sal_memcpy((uint8*)(b), (s), (l));   \
           if ((inc_buf_ptr)) {                 \
                (b) += (l);             \
            }

#define RCPU_CLR_HDR(b, l, inc_buf_ptr)         \
           sal_memset((uint8*)(b), 0, (l));     \
           if ((inc_buf_ptr)) {                 \
               (b) += (l);              \
            }

STATIC int _bcm_rcpu_data_alloc(int unit, int size, void **pkt_buf);
STATIC int _bcm_rcpu_data_free(int unit, void *pkt_buf);
STATIC int _bcm_rcpu_rx_register(int unit, rcpu_rx_cb_f rx, void *cookie);
STATIC int _bcm_rcpu_rx_unregister(int unit, rcpu_rx_cb_f rx);
STATIC int _bcm_rcpu_tp_tx(int unit, uint8 *pkt_buf, int len, void *cookie);
STATIC void _rcpu_tx_callback(int unit, bcm_pkt_t *rcpu_pkt, void *cookie);
STATIC bcm_rx_t _bcm_rcpu_pkt_rx(int unit, bcm_pkt_t * rx_pkt, void * cookie);
#if defined(BCM_OOB_RCPU_SUPPORT)
STATIC int _bcm_rcpu_oob_tp_tx(int unit, uint8 *pkt_buf, int len, void *cookie);
#endif /* BCM_OOB_RCPU_SUPPORT */

/********** global variables ****************/

_bcm_rcpu_control_t      *_bcm_rcpu_control[BCM_RCPU_MAX_UNITS];

/* Standard BCM transport pointer structure */
rcpu_trans_ptr_t rcpu_trans_ptr = {
    _bcm_rcpu_rx_register,
    _bcm_rcpu_rx_unregister,
    _bcm_rcpu_tp_tx,
    _bcm_rcpu_data_alloc,
    _bcm_rcpu_data_free,
    0,  /* Default unit */
    0
};

#if defined(BCM_OOB_RCPU_SUPPORT)
/* Standard OOB transport pointer structure */
rcpu_trans_ptr_t rcpu_oob_trans_ptr = {
    eth_drv_register,
    eth_drv_unregister,
    _bcm_rcpu_oob_tp_tx,
    _bcm_rcpu_data_alloc,
    _bcm_rcpu_data_free,
    0,  /* Default unit */
    0
};
#endif /* BCM_OOB_RCPU_SUPPORT */

/************** local functions *****************************/

#define RCPU_TRANS_PTR_SET(unit) _rcpu_trans_ptr = CMVEC(unit).rcpu_tp;

STATIC int
_bcm_rcpu_data_alloc(int unit, int size, void **pkt_buf)
{
    COMPILER_REFERENCE(unit);
    *pkt_buf = (void *)sal_dma_alloc(size, "RCPU");
    return (*pkt_buf == NULL)? BCM_E_MEMORY:BCM_E_NONE;
}

STATIC int
_bcm_rcpu_data_free(int unit, void *pkt_buf)
{
    COMPILER_REFERENCE(unit);
    if (pkt_buf) {
        sal_dma_free(pkt_buf);
    }
    return BCM_E_NONE;
}

typedef struct rcpu_rx_callback_s {
    void                *rcb_cookie;    /* Cookie for callback */
    rcpu_rx_cb_f         rcb_function;  /* Callback function */
} rcpu_rx_callback_t;

STATIC rcpu_rx_callback_t rcpu_cb[BCM_RCPU_MAX_UNITS];

STATIC int
_bcm_rcpu_rx_register(int unit, rcpu_rx_cb_f rx, void *cookie)
{
    int rv;
    bcm_rx_cfg_t rx_cfg;
    uint32 flags = BCM_RCO_F_ALL_COS;
#ifdef BCM_RX_REFILL_FROM_INTR
    /* 
     * Define BCM_RX_REFILL_FROM_INTR to allocate common rx pool for ToCPU packets
     * in interrupt context.
     */
    flags |= BCM_RCO_F_INTR;
#endif
    BCM_IF_ERROR_RETURN(bcm_esw_rx_register(unit,
                        "RCPU RX", _bcm_rcpu_pkt_rx,
                        BCM_RCPU_RX_PRIO, NULL, flags));
    bcm_esw_rx_cfg_get(unit, &rx_cfg);
    rv = bcm_esw_rx_start(unit, &rx_cfg);
    if (rv == BCM_E_BUSY) {
        rv = BCM_E_NONE;
    }

    rcpu_cb[unit].rcb_function = rx;
    rcpu_cb[unit].rcb_cookie = cookie;

    return rv;
}

STATIC int
_bcm_rcpu_rx_unregister(int unit, rcpu_rx_cb_f rx)
{
    rcpu_cb[unit].rcb_function = NULL;
    return bcm_esw_rx_unregister(unit, _bcm_rcpu_pkt_rx, BCM_RCPU_RX_PRIO);
}

STATIC int
_bcm_rcpu_tp_tx(int unit, uint8 *pkt_buf, int len, void *cookie)
{
    int         rv;
    bcm_pkt_t   *pkt = (bcm_pkt_t *)cookie;
    bcm_pkt_t   *new_pkt = NULL;

    new_pkt = sal_alloc(sizeof(bcm_pkt_t), "_bcm_rcpu_tp_tx");
    if (!new_pkt) {
        return BCM_E_MEMORY;
    }
    sal_memset(new_pkt, 0, sizeof(bcm_pkt_t));
    new_pkt->unit = unit;
    new_pkt->pkt_data = &new_pkt->_pkt_data;
    new_pkt->blk_count = 1;
    new_pkt->pkt_data[0].data = pkt_buf;
    new_pkt->pkt_data[0].len = len;

    /* Use plain ether mode */
    new_pkt->flags = BCM_TX_ETHER | BCM_TX_CRC_REGEN;

    if (pkt) {
        new_pkt->cookie = pkt->_dv;
    } else {
        new_pkt->flags |= BCM_TX_CRC_ALLOC;
    }

    /* Use asynchronous tx */
    new_pkt->call_back = _rcpu_tx_callback;

    if (pkt && pkt->call_back != NULL) {
        /* coverity[overrun-buffer-val : FALSE] */
        rv = bcm_esw_tx(unit, new_pkt, pkt);
    } else {
        /* coverity[overrun-buffer-val : FALSE] */
        rv = bcm_esw_tx(unit, new_pkt, NULL);
    }

    /* Handle error case */

    return rv;
}

#if defined(BCM_OOB_RCPU_SUPPORT)
STATIC int
_bcm_rcpu_oob_tp_tx(int unit, uint8 *pkt_buf, int len, void *cookie)
{
    int rv;

    rv = eth_drv_tx(unit, pkt_buf, len, cookie);

    sal_dma_free(pkt_buf);

    return rv;
}
#endif /* BCM_OOB_RCPU_SUPPORT */

STATIC uint8
_bcm_rcpu_ctoi(const char *s)
{
    uint32 n = 0, base = 16;

    if (*s == '0') {
        s++;
    }

    for (n = 0; ((*s >= 'a' && *s <= 'z') ||
                 (*s >= 'A' && *s <= 'Z') ||
                 (*s >= '0' && *s <= '9')); s++) {
         n = n * base +
	        (*s >= 'a' ? *s - 'a' + 10 :
	         *s >= 'A' ? *s - 'A' + 10 :
	         *s - '0');
    }
    return (uint8)n;
}

STATIC int
_bcm_rcpu_parse_macaddr(char *s, bcm_mac_t mac)
{
    char *p = s;
    int i;

    if (NULL == s) {
        return -1;
    }

    /* Must be of the form 00:01:02:03:AA:BB */
    if (17 != sal_strlen(s)) {
        return -1;
    }

    for (i = 0; i < 6; i++) {
        mac[i] = _bcm_rcpu_ctoi(p);
        p += 3;
    }
    return 0;
}

STATIC int
_bcm_rcpu_get_config(int unit)
{
    soc_pbmp_t  pbmp;
    char *mac_str;

    /* Get MAC address of slave unit */
    mac_str = soc_property_get_str(unit, spn_RCPU_LMAC);
    if (mac_str != NULL) {
		if (_bcm_rcpu_parse_macaddr(mac_str, BCM_RCPU_CFG_LMAC(unit)) < 0 ) { 
       	 	return BCM_E_INTERNAL;
    	}
    }else{
        mac_str = DEFAULT_RCPU_DST_MAC;
		if (_bcm_rcpu_parse_macaddr(mac_str, BCM_RCPU_CFG_LMAC(unit)) < 0 ) { 
       	 	return BCM_E_INTERNAL;
    	}
        BCM_RCPU_CFG(unit)->rcpu_cfg.dst_mac[5] = unit;

    }


    /* Get MAC address used by master unit */
    mac_str = soc_property_get_str(unit, spn_RCPU_SRC_MAC);
    if (mac_str == NULL) {
        mac_str = DEFAULT_RCPU_SRC_MAC;
    }

    if (_bcm_rcpu_parse_macaddr(mac_str, BCM_RCPU_CFG_SRC_MAC(unit)) < 0 ) { 
        return BCM_E_INTERNAL;
    }

    /* Get the default VLAN and TPID */
    BCM_RCPU_CFG(unit)->rcpu_cfg.vlan = soc_property_get(unit, spn_RCPU_VLAN,
                                                BCM_VLAN_DEFAULT);
    BCM_RCPU_CFG(unit)->rcpu_cfg.tpid = DEFAULT_RCPU_TPID;

    /* 
     * Get the valid PBM for remote MAC. Valid PBM is the ports
     * on which the RCPU packets can be received by the remote CMIC. 
     */
    pbmp = soc_property_get_pbmp(unit, spn_RCPU_RX_PBMP, 0);
    if (SOC_PBMP_IS_NULL(pbmp)) {
        pbmp = PBMP_PORT_ALL(unit);
    }
    SOC_PBMP_ASSIGN(BCM_RCPU_CFG(unit)->pbmp, pbmp);

    /* Get the Signature and Ethertype to be used for remote CMIC packets. */
    BCM_RCPU_CFG(unit)->rcpu_cfg.signature = DEFAULT_RCPU_SIGNATURE;
    BCM_RCPU_CFG(unit)->rcpu_cfg.ether_type = DEFAULT_RCPU_ETHER_TYPE;

    /* Get the various RCPU packet priorities */
    if (soc_feature(unit, soc_feature_rcpu_priority)) {
        BCM_RCPU_CFG(unit)->dot1pri[0] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS0, 0);
        BCM_RCPU_CFG(unit)->mh_tc[0] = soc_property_get(unit, spn_RCPU_MH_TC_COS0, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[0] = soc_property_get(unit, spn_RCPU_CPU_TC_COS0, 0);
        BCM_RCPU_CFG(unit)->dot1pri[1] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS1, 0);
        BCM_RCPU_CFG(unit)->mh_tc[1] = soc_property_get(unit, spn_RCPU_MH_TC_COS1, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[1] = soc_property_get(unit, spn_RCPU_CPU_TC_COS1, 0);
        BCM_RCPU_CFG(unit)->dot1pri[2] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS2, 0);
        BCM_RCPU_CFG(unit)->mh_tc[2] = soc_property_get(unit, spn_RCPU_MH_TC_COS2, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[2] = soc_property_get(unit, spn_RCPU_CPU_TC_COS2, 0);
        BCM_RCPU_CFG(unit)->dot1pri[3] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS3, 0);
        BCM_RCPU_CFG(unit)->mh_tc[3] = soc_property_get(unit, spn_RCPU_MH_TC_COS3, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[3] = soc_property_get(unit, spn_RCPU_CPU_TC_COS3, 0);
        BCM_RCPU_CFG(unit)->dot1pri[4] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS4, 0);
        BCM_RCPU_CFG(unit)->mh_tc[4] = soc_property_get(unit, spn_RCPU_MH_TC_COS4, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[4] = soc_property_get(unit, spn_RCPU_CPU_TC_COS4, 0);
        BCM_RCPU_CFG(unit)->dot1pri[5] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS5, 0);
        BCM_RCPU_CFG(unit)->mh_tc[5] = soc_property_get(unit, spn_RCPU_MH_TC_COS5, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[5] = soc_property_get(unit, spn_RCPU_CPU_TC_COS5, 0);
        BCM_RCPU_CFG(unit)->dot1pri[6] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS6, 0);
        BCM_RCPU_CFG(unit)->mh_tc[6] = soc_property_get(unit, spn_RCPU_MH_TC_COS6, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[6] = soc_property_get(unit, spn_RCPU_CPU_TC_COS6, 0);
        BCM_RCPU_CFG(unit)->dot1pri[7] = soc_property_get(unit, spn_RCPU_DOT1PRI_COS7, 0);
        BCM_RCPU_CFG(unit)->mh_tc[7] = soc_property_get(unit, spn_RCPU_MH_TC_COS7, 0);
        BCM_RCPU_CFG(unit)->cpu_tc[7] = soc_property_get(unit, spn_RCPU_CPU_TC_COS7, 0);
    }
    return BCM_E_NONE;
}

STATIC void
_rcpu_tx_callback(int unit, bcm_pkt_t *rcpu_pkt, void *cookie)
{
    bcm_pkt_t   *pkt;

    if (cookie) {
        pkt = (bcm_pkt_t *) cookie;
        if (pkt->unit != unit && pkt->call_back) {
            pkt->call_back(pkt->unit, pkt, rcpu_pkt->cookie);
        }
    }
    sal_dma_free(rcpu_pkt->pkt_data[0].data);
    sal_free(rcpu_pkt);
}

/* tx_list callback data */
typedef struct _rcpu_tx_list_callback_data_s {
    bcm_pkt_cb_f      call_back;    /* Packet callback function */
    bcm_pkt_cb_f      all_done;     /* List callback function */
    void              *cookie;      /* Original pkt->cookie */
    bcm_pkt_t         *pkt_list;    /* head of packte list */
} _rcpu_tx_list_callback_data_t;

STATIC void
_rcpu_tx_list_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    _rcpu_tx_list_callback_data_t *data =
        (_rcpu_tx_list_callback_data_t *)pkt->cookie;

    /* Restore pkt data that was used */
    pkt->cookie = data->cookie;
    pkt->call_back = data->call_back;

    /* Packet callback, if any */
    if (pkt->call_back) {
        pkt->call_back(unit, pkt, cookie);
    }

    /* There's should always be a list callback, otherwise this
       function would not have been called */
    data->all_done(unit, data->pkt_list, cookie);

    sal_free(data);
    
}

STATIC INLINE void
_rcpu_get_mac_vlan_ptrs(bcm_pkt_t *pkt, uint8 **src_mac,
                   uint8 **vlan_ptr, int *block_offset, int *byte_offset)
{
    /* Assume everything is in block 0 */
    *src_mac = &pkt->pkt_data[0].data[sizeof(bcm_mac_t)];
    *block_offset = 0;

    if (BCM_PKT_NO_VLAN_TAG(pkt)) { 
        /* Get VLAN from _vtag pkt member */
        *byte_offset = 2 * sizeof(bcm_mac_t);
        *vlan_ptr = pkt->_vtag;

        if ((uint32)pkt->pkt_data[0].len < 2 * sizeof(bcm_mac_t)) {
            if (pkt->blk_count > 1) {
                /* Src MAC in block 1 */
                *src_mac = pkt->pkt_data[1].data;
                *block_offset = 1;
                *byte_offset = sizeof(bcm_mac_t);
            } else {
                /* This else case is added in conjunction with the
                 * pkt->blk_count check above to fix coverity warning
                 * regarding out-of-bounds access. This is needed to convey
                 * an error condition to the caller. The caller enforces
                 * a check on block_offset to be <= pkt->blk_count.
                 */
                *block_offset = pkt->blk_count + 1;
            }
        }
    } else { /* Packet has VLAN tag */
        *byte_offset = 2 * sizeof(bcm_mac_t) + sizeof(uint32);
        *vlan_ptr = &pkt->pkt_data[0].data[2 * sizeof(bcm_mac_t)];

        if ((uint32)pkt->pkt_data[0].len < 2 * sizeof(bcm_mac_t)) {
            if (pkt->blk_count > 1) {
                /* Src MAC in block 1; assume VLAN there too at first */
                *src_mac = pkt->pkt_data[1].data;
                *vlan_ptr = &pkt->pkt_data[1].data[sizeof(bcm_mac_t)];
                *block_offset = 1;
                *byte_offset = sizeof(bcm_mac_t) + sizeof(uint32);
                if ((uint32)pkt->pkt_data[1].len < sizeof(bcm_mac_t) + sizeof(uint32)) {
                    /* Oops, VLAN in block 2 */
                    if (pkt->blk_count > 2) {
                        *vlan_ptr = pkt->pkt_data[2].data;
                        *block_offset = 2;
                        *byte_offset = sizeof(uint32);
                    } else {
                        /* This else case is added in conjunction with the
                         * pkt->blk_count check above to fix coverity warning
                         * regarding out-of-bounds access. This is needed to convey
                         * an error condition to the caller. The caller enforces
                         * a check on block_offset to be <= pkt->blk_count.
                         */
                        *block_offset = pkt->blk_count + 1;
                    }
                }
            } else {
                /* This else case is added in conjunction with the
                 * pkt->blk_count check above to fix coverity warning
                 * regarding out-of-bounds access. This is needed to convey
                 * an error condition to the caller. The caller enforces
                 * a check on block_offset to be <= pkt->blk_count.
                 */
                *block_offset = pkt->blk_count + 1;
            }
        } else if ((uint32)pkt->pkt_data[0].len <
                   2 * sizeof(bcm_mac_t) + sizeof(uint32)) {
            if (pkt->blk_count > 1) {
                /* VLAN in block 1 */
                *block_offset = 1;
                *byte_offset = sizeof(uint32);
                *vlan_ptr = pkt->pkt_data[1].data;
            } else {
                 /* This else case is added in conjunction with the
                 * pkt->blk_count check above to fix coverity warning
                 * regarding out-of-bounds access. This is needed to convey
                 * an error condition to the caller. The caller enforces
                 * a check on block_offset to be <= pkt->blk_count.
                 */
                *block_offset = pkt->blk_count + 1;
           }
        }
    }
}

STATIC int
_tx_rcpu_higig_hdr_setup(int unit, bcm_pkt_t *pkt, uint8 *hdr_buf,
                         int *hdr_size, int *blk_ofst, int *byte_ofst)
{
    int i, byte_offset = 0;
    uint8 *src_mac, *vlan_ptr, *buf_start = hdr_buf;
    int block_offset = 0;
    soc_pbsmh_hdr_t *pbh = NULL;

    if ((uint32)pkt->pkt_data[0].len < sizeof(bcm_mac_t)) {
        return BCM_E_PARAM;
    }

    /* Get pointers to srcmac and vlan; check if bad block count */
    _rcpu_get_mac_vlan_ptrs(pkt, &src_mac, &vlan_ptr, &block_offset,
                       &byte_offset);

    if (block_offset >= pkt->blk_count) {
        return BCM_E_PARAM;
    }

    if (byte_offset >= pkt->pkt_data[block_offset].len) {
        byte_offset = 0;
        block_offset++;
    }

    if (!BCM_PKT_TX_ETHER(pkt)) {
        /* Raw packet steered mode (PB header in descriptor) */
        pbh = (soc_pbsmh_hdr_t *)pkt->_pb_hdr;

        /* copy PBH to header buffer */
        RCPU_COPY_HDR(hdr_buf, pbh, sizeof(soc_pbsmh_hdr_t), TRUE);
        RCPU_CLR_HDR(hdr_buf, FROM_CPU_DCB_SIZE - sizeof(soc_pbsmh_hdr_t), TRUE);

        /*
         * BCM_PKT_F_HGHDR flag indicates higig header is part of
         * packet data stream.
         */
        if (BCM_PKT_HAS_HGHDR(pkt)) {
            uint8 *tmp_pkt_hg_hdr = BCM_PKT_HG_HDR(pkt);
            int hg_hdr_len = SOC_HIGIG_HDR_SIZE;
            /*
             *      XGS3: Raw Higig packet steered mode.
             *            Make higig header part of packet stream and PB
             *            header part of descriptor
             */
#ifdef BCM_HIGIG2_SUPPORT
            if (tmp_pkt_hg_hdr[0] == SOC_HIGIG2_START) {
                hg_hdr_len = SOC_HIGIG2_HDR_SIZE;
            }
#endif /* BCM_HIGIG2_SUPPORT */
            /* copy HiGig header to buffer */
            RCPU_COPY_HDR(hdr_buf, tmp_pkt_hg_hdr, hg_hdr_len, TRUE);
        }
    } else {
        /* Non-SOBMH HiGig hdr */
        if (BCM_PKT_TX_HG_READY(pkt)) {
            uint8 *tmp_pkt_hg_hdr = BCM_PKT_HG_HDR(pkt);
            uint8 pad[FROM_CPU_DCB_SIZE];
            int hg_hdr_len = SOC_HIGIG_HDR_SIZE;
#ifdef BCM_HIGIG2_SUPPORT
            if (tmp_pkt_hg_hdr[0] == SOC_HIGIG2_START) {
                hg_hdr_len = SOC_HIGIG2_HDR_SIZE;
            }
#endif /* BCM_HIGIG2_SUPPORT */
            for (i = 0; i < (FROM_CPU_DCB_SIZE - hg_hdr_len); i++) {
                pad[i] = 0;
            }
            /* copy HiGig header to buffer */
            RCPU_COPY_HDR(hdr_buf, tmp_pkt_hg_hdr, hg_hdr_len, TRUE);
            RCPU_COPY_HDR(hdr_buf, pad, (FROM_CPU_DCB_SIZE - hg_hdr_len), TRUE);
        }
    }

    /* Dest mac */
    RCPU_COPY_HDR(hdr_buf, pkt->pkt_data[0].data, sizeof(bcm_mac_t), TRUE);
    
    /* Source mac */
    RCPU_COPY_HDR(hdr_buf, src_mac, sizeof(bcm_mac_t), TRUE);

    /* VLAN tag */
    /* No VLAN tag for Fabric mapped Higig TX as well */
    if (!BCM_PKT_HAS_HGHDR(pkt) &&
        !(pkt->flags & BCM_PKT_F_TX_UNTAG) &&
        !BCM_PKT_TX_FABRIC_MAPPED(pkt)) { /* No HG Hdr, so add VLAN tag */
        RCPU_COPY_HDR(hdr_buf, vlan_ptr, sizeof(uint32), TRUE);
    }

    /* SL TAG */
    if (pkt->flags & BCM_PKT_F_SLTAG) {
        RCPU_COPY_HDR(hdr_buf, pkt->_sltag, sizeof(uint32), TRUE);
    }

    *blk_ofst = block_offset;
    *byte_ofst = byte_offset;
    *hdr_size = hdr_buf - buf_start;

    LOG_INFO(BSL_LS_BCM_TX,
             (BSL_META_U(unit,
                         "_tx_rcpu_higig_hdr_setup: flags 0x%08x\n"), pkt->flags));

    return BCM_E_NONE;
}


extern void
rx_higig_info_decode(int unit, bcm_pkt_t *pkt);

STATIC bcm_pkt_t *
_bcm_rcpu_rx_process_dcb(int unit, dcb_t *dcb, bcm_pkt_t *pkt)
{
    pkt->rx_untagged = SOC_DCB_RX_UNTAGGED_GET(unit, dcb);
    pkt->rx_port = SOC_DCB_RX_INGPORT_GET(unit, dcb);

    pkt->rx_reason = SOC_DCB_RX_REASON_GET(unit, dcb);
    SOC_DCB_RX_REASONS_GET(unit, dcb, &pkt->rx_reasons);

    if (SOC_IS_XGS_SWITCH(unit)) { /* Get XGS HiGig info from DMA desc */
        int src_port_tgid;
        pkt->opcode = SOC_DCB_RX_OPCODE_GET(unit, dcb);
        pkt->dest_mod = SOC_DCB_RX_DESTMOD_GET(unit, dcb);
        pkt->dest_port = SOC_DCB_RX_DESTPORT_GET(unit, dcb);
        pkt->src_mod = SOC_DCB_RX_SRCMOD_GET(unit, dcb);
        src_port_tgid = SOC_DCB_RX_SRCPORT_GET(unit, dcb);
        if (!soc_feature(unit, soc_feature_trunk_group_overlay) &&
            (src_port_tgid & BCM_TGID_TRUNK_INDICATOR(unit))) {
            pkt->src_trunk = src_port_tgid & BCM_TGID_PORT_TRUNK_MASK(unit);
            pkt->flags |= BCM_PKT_F_TRUNK;
            pkt->src_port = -1;
        } else {
            pkt->src_port = src_port_tgid;
            pkt->src_trunk = -1;
            if (soc_feature(unit, soc_feature_trunk_extended)) {
                int rv = BCM_E_NONE;
                bcm_trunk_t tid;

                tid = -1;
                rv = _bcm_esw_trunk_port_property_get(unit, pkt->src_mod,
                                                      src_port_tgid, &tid);
                if (BCM_SUCCESS(rv) && (tid != -1)) {
                    pkt->src_trunk = tid;
                    pkt->flags |= BCM_PKT_F_TRUNK;
                }
            }
        }
        pkt->src_gport = BCM_GPORT_INVALID;
        pkt->dst_gport = BCM_GPORT_INVALID;
        pkt->cos = SOC_DCB_RX_COS_GET(unit, dcb);

        pkt->prio_int = BCM_PKT_VLAN_PRI(pkt);
        pkt->vlan = BCM_PKT_VLAN_ID(pkt);

        if (SOC_IS_XGS3_SWITCH(unit)) {
            bcm_vlan_t outer_vid;
            int hg_hdr_size;
            int rv = BCM_E_NONE;
            _bcm_gport_dest_t dest;

            if (SOC_DCB_RX_MIRROR_GET(unit, dcb)) {
                pkt->flags |= BCM_RX_MIRRORED;
            } 
            pkt->rx_classification_tag = SOC_DCB_RX_CLASSTAG_GET(unit, dcb);
            outer_vid = SOC_DCB_RX_OUTER_VID_GET(unit, dcb);
            if (outer_vid) {
                /* Don't overwrite if we don't have info */
                pkt->vlan = outer_vid;
            }
            pkt->vlan_pri = SOC_DCB_RX_OUTER_PRI_GET(unit, dcb);
            pkt->vlan_cfi = SOC_DCB_RX_OUTER_CFI_GET(unit, dcb);
            pkt->inner_vlan = SOC_DCB_RX_INNER_VID_GET(unit, dcb);
            pkt->inner_vlan_pri = SOC_DCB_RX_INNER_PRI_GET(unit, dcb);
            pkt->inner_vlan_cfi = SOC_DCB_RX_INNER_CFI_GET(unit, dcb);
#ifdef BCM_TRX_SUPPORT
            pkt->rx_outer_tag_action =
                _BCM_TRX_TAG_ACTION_DECODE(SOC_DCB_RX_OUTER_TAG_ACTION_GET(unit, dcb));
            pkt->rx_inner_tag_action =
                _BCM_TRX_TAG_ACTION_DECODE(SOC_DCB_RX_INNER_TAG_ACTION_GET(unit, dcb));
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_HIGIG2_SUPPORT
            if (soc_feature(unit, soc_feature_higig2)) {
                hg_hdr_size = sizeof(soc_higig2_hdr_t);
            } else
#endif /* BCM_HIGIG2_SUPPORT */
            {
                hg_hdr_size = sizeof(soc_higig_hdr_t);
            }
            /* Put XGS hdr into pkt->_higig */
            sal_memcpy(pkt->_higig, SOC_DCB_MHP_GET(unit, dcb),
                       hg_hdr_size);
#ifdef  LE_HOST
            {
                /* Higig field accessors expect network byte ordering,
                 * so we must reverse the bytes on LE hosts */
                int word;
                uint32 *hg_data = (uint32 *) pkt->_higig;
                for (word = 0; word < BYTES2WORDS(hg_hdr_size); word++) {
                    hg_data[word] = _shr_swap32(hg_data[word]);
                }
            }
#endif /* LE_HOST */
            rx_higig_info_decode(unit, pkt);

            if (BCM_GPORT_INVALID == pkt->src_gport) {
                /* Not set by the Higig2 logic */
                _bcm_gport_dest_t_init(&dest);
                if (-1 != pkt->src_trunk) {
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    dest.tgid = pkt->src_trunk;
                } else {
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    dest.modid = pkt->src_mod;
                    dest.port = pkt->src_port;
                }
                rv = _bcm_esw_gport_construct(unit, &dest, &(pkt->src_gport));
                if (BCM_FAILURE(rv)) {
                    pkt->src_gport = BCM_GPORT_INVALID;
                }
            }

            if (BCM_GPORT_INVALID == pkt->dst_gport) {
                /* Not set by the Higig2 logic */
                _bcm_gport_dest_t_init(&dest);
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                dest.modid = pkt->dest_mod;
                dest.port = pkt->dest_port;
                rv = _bcm_esw_gport_construct(unit, &dest,
                                              &(pkt->dst_gport));
                if (BCM_FAILURE(rv)) {
                    pkt->dst_gport = BCM_GPORT_INVALID;
                }
            }
        }
        pkt->rx_matched = SOC_DCB_RX_MATCHRULE_GET(unit, dcb);

        if (soc_feature(unit, soc_feature_rx_timestamp)) {
           /* Get time stamp value for TS protocol packets or OAM DM */
            pkt->rx_timestamp = SOC_DCB_RX_TIMESTAMP_GET(unit, dcb); 
        }

        if (soc_feature(unit, soc_feature_rx_timestamp_upper)) {
            /* Get upper 32-bit of time stamp value for OAM DM */
            pkt->rx_timestamp_upper = SOC_DCB_RX_TIMESTAMP_UPPER_GET(unit, dcb);
        }
    }
    return 0;
}

/* Enable VLAN translation for the vlan on given unit and port. */
STATIC int
rcpu_vlan_translate(int unit, int vlan, int port)
{
    BCM_IF_ERROR_RETURN
        (bcm_vlan_translate_add(unit, port, vlan, vlan, -1));
    BCM_IF_ERROR_RETURN
        (bcm_vlan_translate_egress_add(unit, port, vlan, vlan, -1));
    BCM_IF_ERROR_RETURN
        (bcm_vlan_control_port_set(unit, port,
                                   bcmVlanTranslateIngressEnable, TRUE));
    BCM_IF_ERROR_RETURN
        (bcm_vlan_control_port_set(unit, port,
                                   bcmVlanTranslateEgressEnable, TRUE));
    BCM_IF_ERROR_RETURN
        (bcm_vlan_control_port_set(unit, port,
                                   bcmVlanTranslateEgressMissUntag, FALSE));
    return BCM_E_NONE;
}

STATIC int
_bcm_rcpu_init(int unit)
{
    uint32              rval, MacOui, MacNonOui;
    uint8               *mac;
    bcm_l2_addr_t	    l2addr;
    bcm_pbmp_t		    pbm;
    bcm_pbmp_t		    upbm;
    bcm_pbmp_t		pbmcpu;
    int                 masterunit;
    bcm_module_t        mymodid;
    int                 rcpu_vlan;
    int                 rcpu_encap;
    int                 rcpu_port, port;

    masterunit = soc_property_get(unit, spn_RCPU_MASTER_UNIT, -1);
    if (masterunit == unit) {
        return BCM_E_CONFIG;
    }

    rcpu_vlan = BCM_RCPU_CFG(unit)->rcpu_cfg.vlan & 0xfff;
    rcpu_port = soc_property_get(unit, spn_RCPU_PORT, 3);
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, rcpu_port, &rcpu_encap));

    rval = 0;
    SOC_PBMP_ITER(BCM_RCPU_CFG(unit)->pbmp, port) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_control_set(unit, port, 
                                     bcmPortControlRemoteCpuEnable, 1));
    }

    /* enable all COS */
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, 
                               bcmSwitchRemoteCpuToCpuDestPortAllReasons, 0)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, 
                               bcmSwitchRemoteCpuToCpuDestMacAllReasons, 1)); 

    rval = 0;
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)){
        soc_reg_field_set(unit, CMIC_PKT_REASON_MINI_0_TYPEr, &rval, REASONSf, 0);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_PKT_REASON_MINI_0_TYPEr(unit, rval));
    }else
#endif /* BCM_CMICM_SUPPORT */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)){
        soc_reg_field_set(unit, CMIC_RPE_PKT_REASON_MINI_0_TYPEr, &rval, REASONSf, 0);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RPE_PKT_REASON_MINI_0_TYPEr(unit, 0, rval));
    }else
#endif /* BCM_CMICX_SUPPORT */
    {
        soc_reg_field_set(unit, CMIC_PKT_REASON_MINIr, &rval, REASONSf, 0);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_PKT_REASON_MINIr(unit, rval));
    }

    /* enable all COS */
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuForceScheduling, 1)); 

    /* Set dest mac address */
    mac = BCM_RCPU_CFG_SRC_MAC(unit);
    MacOui = (((uint32)mac[0]) << 16 | ((uint32)mac[1]) << 8 | ((uint32)mac[2]));
    MacNonOui = (((uint32)mac[3]) << 16 | ((uint32)mac[4]) << 8 | ((uint32)mac[5]));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuDestMacOui, 
                                   MacOui));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuDestMacNonOui, 
                                   MacNonOui));

    /* Set local mac address */
    mac = BCM_RCPU_CFG_LMAC(unit);
    MacOui = (((uint32)mac[0]) << 16 | ((uint32)mac[1]) << 8 | ((uint32)mac[2]));
    MacNonOui = (((uint32)mac[3]) << 16 | ((uint32)mac[4]) << 8 | ((uint32)mac[5]));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuLocalMacOui, 
                                   MacOui));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuLocalMacNonOui, 
                                   MacNonOui));
    /* set CMIC EtherType and Signature */
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuEthertype, 
                                   BCM_RCPU_CFG(unit)->rcpu_cfg.ether_type));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuSignature, 
                                   BCM_RCPU_CFG(unit)->rcpu_cfg.signature));

    /* Set the vlan and tpid values */
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuTpid, 
                                   BCM_RCPU_CFG(unit)->rcpu_cfg.tpid));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuVlan, 
                                   BCM_RCPU_CFG(unit)->rcpu_cfg.vlan));

    /* Enable vlan/lmac1 checking and Schan op */
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuSchanEnable, 1));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuMatchLocalMac, 1));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuMatchVlan, 1));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuToCpuEnable, 1));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuFromCpuEnable, 1));

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, bcmSwitchRemoteCpuCmicEnable, 1));
    }
#endif /* BCM_CMICM_SUPPORT */

#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_rcpu_priority)) {
        uint8 property;

#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN(READ_CMIC_RPE_PKT_CTRLr(unit, &rval));
            property = soc_property_get(unit, spn_RCPU_MH_SRC_PID_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_RPE_PKT_CTRLr, &rval, MH_SRC_PID_ENABLEf, property);
            property = soc_property_get(unit, spn_RCPU_MH_CPU_COS_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_RPE_PKT_CTRLr, &rval, CPU_TC_ENABLEf, property);
            property = soc_property_get(unit, spn_RCPU_MH_TC_MAP_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_RPE_PKT_CTRLr, &rval, MH_TC_MAP_ENABLEf, property);
            property = soc_property_get(unit, spn_RCPU_DOT1PRI_MAP_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_RPE_PKT_CTRLr, &rval, IEEE_802_1_PRI_MAP_ENABLEf, property);
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN(WRITE_CMIC_RPE_PKT_CTRLr(unit, rval));
        } else
#endif /* BCM_CMICX_SUPPORT */
        {
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN(READ_CMIC_PKT_CTRLr(unit, &rval));
            property = soc_property_get(unit, spn_RCPU_MH_SRC_PID_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval, MH_SRC_PID_ENABLEf, property);
            property = soc_property_get(unit, spn_RCPU_MH_CPU_COS_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval, CPU_TC_ENABLEf, property);
            property = soc_property_get(unit, spn_RCPU_MH_TC_MAP_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval, MH_TC_MAP_ENABLEf, property);
            property = soc_property_get(unit, spn_RCPU_DOT1PRI_MAP_ENABLE, 0);
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval, IEEE_802_1_PRI_MAP_ENABLEf, property);
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN(WRITE_CMIC_PKT_CTRLr(unit, rval));
        }
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    
    /*
     * Insert L2 entry on remote unit and local unit.
     */
    bcm_l2_addr_t_init(&l2addr, 
                       BCM_RCPU_CFG_LMAC(unit), 
                       rcpu_vlan);
    l2addr.flags |= BCM_L2_STATIC | BCM_L2_COPY_TO_CPU;
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &mymodid));
    l2addr.modid = soc_property_get(unit, spn_RCPU_SLAVE_MODID, mymodid);
    bcm_esw_l2_addr_add(unit, &l2addr);

    if ( masterunit >= 0 ) {
      l2addr.flags &= ~(BCM_L2_COPY_TO_CPU);
      BCM_IF_ERROR_RETURN(
          bcm_esw_stk_my_modid_get(masterunit, &mymodid));
      if ( rcpu_encap != BCM_PORT_ENCAP_IEEE ) {
          l2addr.modid = soc_property_get(unit, spn_RCPU_SLAVE_MODID, mymodid);
          l2addr.port = CMIC_PORT(unit);
      } else {
          l2addr.modid = soc_property_get(masterunit, spn_RCPU_MASTER_MODID, mymodid);
          l2addr.port = rcpu_port;
      }
      bcm_esw_l2_addr_add(masterunit, &l2addr);
    }

    bcm_l2_addr_t_init(&l2addr, 
                       BCM_RCPU_CFG_SRC_MAC(unit), 
                       rcpu_vlan);

    l2addr.flags = BCM_L2_STATIC;
    if ( masterunit >= 0 ) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(masterunit, &mymodid));
        l2addr.modid = soc_property_get(masterunit, spn_RCPU_MASTER_MODID, mymodid);
        bcm_esw_l2_addr_add(masterunit, &l2addr);
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &mymodid));

    if ( rcpu_encap != BCM_PORT_ENCAP_IEEE ) {
        /* for higig, we want to do it on the higig port */
        l2addr.modid = soc_property_get(masterunit, spn_RCPU_MASTER_MODID, mymodid);
        l2addr.port = CMIC_PORT(masterunit);  /* destination should be CMIC port */
    } else {
        l2addr.modid = soc_property_get(unit, spn_RCPU_SLAVE_MODID, mymodid);
        l2addr.port = rcpu_port;
    }
    bcm_esw_l2_addr_add(unit, &l2addr);

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_CLEAR(upbm);
    BCM_PBMP_CLEAR(pbmcpu);
    BCM_PBMP_PORT_SET(pbm, l2addr.port);
    BCM_PBMP_PORT_SET(pbmcpu, l2addr.port);
    BCM_PBMP_PORT_ADD(pbmcpu, CMIC_PORT(unit));


    /* If the RCPU vlan is not the default, create the vlan */
    if (BCM_SUCCESS(bcm_esw_vlan_create(unit, rcpu_vlan))) {
        if (rcpu_vlan != BCM_VLAN_DEFAULT &&
            rcpu_encap == BCM_PORT_ENCAP_IEEE) {
            /* 
             * Remove RCPU port from VLAN 1 if RCPU VLAN is not 1,
             * and the RCPU port encapsulation is IEEE 
             */
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_port_remove(unit, 1, pbm));

            /* 
             * Prepare VLAN translation on the RCPU port and CPU port if
             * double tagging mode gets enabled. This forces the service
             * vlan to always be the RCPU vlan for RCPU protocol packets. 
             */

            if (soc_feature(unit, soc_feature_vlan_translation) &&
                !soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                BCM_IF_ERROR_RETURN(rcpu_vlan_translate(unit, rcpu_vlan, 
                                                        l2addr.port));
                BCM_IF_ERROR_RETURN(rcpu_vlan_translate(unit, rcpu_vlan,
                                                        CMIC_PORT(unit)));
            }
        }
    }

    BCM_IF_ERROR_RETURN
	(bcm_esw_vlan_port_remove(unit, rcpu_vlan, pbmcpu));

    BCM_IF_ERROR_RETURN
	(bcm_esw_vlan_port_add(unit, rcpu_vlan, pbmcpu, upbm));

    if ( masterunit >= 0 ) {
        /* Error code here is ignored in purpose */
        (void)bcm_esw_vlan_create(masterunit, rcpu_vlan);

        BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_remove(masterunit, rcpu_vlan, pbmcpu));
        BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_add(masterunit, rcpu_vlan, pbmcpu, upbm));
        if (rcpu_vlan != BCM_VLAN_DEFAULT &&
            rcpu_encap == BCM_PORT_ENCAP_IEEE) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_vlan_port_remove(masterunit, 1, pbm));
        
            /* 
             * Prepare VLAN translation on the RCPU port and CPU port
             * if double tagging mode gets enabled for the master
             * unit. This forces the service vlan to always be the
             * RCPU vlan for RCPU protocol packets. 
             */

            if (soc_feature(unit, soc_feature_vlan_translation)) {
                BCM_IF_ERROR_RETURN(rcpu_vlan_translate(masterunit, rcpu_vlan,
                                                        l2addr.port));
                BCM_IF_ERROR_RETURN(rcpu_vlan_translate(masterunit, rcpu_vlan,
                                                        CMIC_PORT(masterunit)));
            }
        }
    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    /* Set the various packet priorities */
    if (soc_feature(unit, soc_feature_rcpu_priority)) {
        int i;

        for (i = 0; i < NUM_COS(unit); i++) {
            uint32  flags = 0;
            int     val = 0;

            if (BCM_RCPU_CFG(unit)->dot1pri[i]) {
                flags |= BCM_SWITCH_REMOTE_CPU_ENCAP_IEEE;
                val = BCM_RCPU_CFG(unit)->dot1pri[i];
            } else if (BCM_RCPU_CFG(unit)->mh_tc[i]) {
                flags |= BCM_SWITCH_REMOTE_CPU_ENCAP_HIGIG2;
                val = BCM_RCPU_CFG(unit)->mh_tc[i];
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_rcpu_encap_priority_map_set(unit, flags, i, val));
        }
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_TRX_SUPPORT)

#define BCM_TRX_CPU_QUEUE_MAX      31

    /* Set the traffic class to CPU queue mapping */
    if (soc_feature(unit, soc_feature_rcpu_tc_mapping)) {
        int queue, tc, tc_total = soc_mem_index_count(unit, CPU_TS_MAPm);
        cpu_ts_map_entry_t tc_map;

        for (tc = 0; tc < tc_total; tc++) {
            sal_memset(&tc_map, 0, sizeof(tc_map));
            queue = soc_property_suffix_num_get(unit, tc, spn_RCPU_CPU_QUEUE,
                                                "tc", 0);
            if (queue > BCM_TRX_CPU_QUEUE_MAX) {
                queue = BCM_TRX_CPU_QUEUE_MAX;
            }
            soc_mem_field32_set(unit, CPU_TS_MAPm, &tc_map,
                                CPU_QUEUE_IDf, queue);
            SOC_IF_ERROR_RETURN
                (WRITE_CPU_TS_MAPm(unit, MEM_BLOCK_ALL, tc, &tc_map));
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_NONE;
}

int
_bcm_rcpu_tx(int unit, bcm_pkt_t *pkt, void * cookie)
{
    int         rcpu_pkt_len = 0, pkt_len;
#ifdef BCM_HIGIG2_SUPPORT
    uint8       *tmp_pkt_hg_hdr = BCM_PKT_HG_HDR(pkt);
#endif /* BCM_HIGIG2_SUPPORT */
    int         hg_hdr_len = SOC_HIGIG_HDR_SIZE;
    int         i, hdr_size = 0, blk_ofst = 0, byte_ofst= 0, rv = BCM_E_NONE;
    uint32      mh_flags = 0, tmp_len, payload_len;
    uint8       *tx_buf, *buf_ptr;
    rcpu_encap_info_t info;
    rcpu_trans_ptr_t *_rcpu_trans_ptr;

    if (!SOC_IS_RCPU_UNIT(unit)) {
        return BCM_E_UNIT;
    }

    pkt->unit = unit;

    BCM_PKT_BLK_BYTES_CALC(pkt, pkt_len);
    if (pkt_len <= 0) {
        return BCM_E_PARAM;
    }

    /* 
     * Allocate pkt/buffer big enough to accommodate the packet 
     * after all the header manipulation is done.
     */
    rcpu_pkt_len = pkt_len + sizeof(rcpu1_cmic_pkt_hdr_t);
    if (!BCM_PKT_TX_ETHER(pkt)) {
        /* SOBMH header needs to be appended to the packet. */
        rcpu_pkt_len += FROM_CPU_DCB_SIZE;
        if (BCM_PKT_HAS_HGHDR(pkt)) {
#ifdef BCM_HIGIG2_SUPPORT
            if (tmp_pkt_hg_hdr[0] == SOC_HIGIG2_START) {
                hg_hdr_len = SOC_HIGIG2_HDR_SIZE;
            }
#endif /* BCM_HIGIG2_SUPPORT */
            rcpu_pkt_len += hg_hdr_len ; 
        }
        mh_flags = 1;
    } else {
        /* Non-SOBMH HiGig hdr */
        if (BCM_PKT_TX_HG_READY(pkt)) {
            rcpu_pkt_len += FROM_CPU_DCB_SIZE;
            mh_flags = 1;
        }
    }

    if (!BCM_PKT_NO_VLAN_TAG(pkt)) {
        if (BCM_PKT_HAS_HGHDR(pkt) ||
            (pkt->flags & BCM_PKT_F_TX_UNTAG)||
            BCM_PKT_TX_FABRIC_MAPPED(pkt)) {
            /* No VLAN tag */
            rcpu_pkt_len -= sizeof(uint32);
        }
    }

    if (pkt->flags & BCM_PKT_F_SLTAG) {
        /* SL TAG */
        rcpu_pkt_len += sizeof(uint32);
    }

    if ((BCM_PKT_NO_VLAN_TAG(pkt) && rcpu_pkt_len < 60) ||
        (!BCM_PKT_NO_VLAN_TAG(pkt) && BCM_PKT_HAS_HGHDR(pkt) && 
         BCM_PKT_TX_FABRIC_MAPPED(pkt) && rcpu_pkt_len < 60) ||
        (!BCM_PKT_NO_VLAN_TAG(pkt) && !BCM_PKT_HAS_HGHDR(pkt) && 
         !BCM_PKT_TX_FABRIC_MAPPED(pkt) && rcpu_pkt_len < 64)) {
        LOG_ERROR(BSL_LS_BCM_RCPU,
                  (BSL_META_U(unit,
                              "_bcm_rcpu_tx: Discarding %s runt packet %s higig header %d\n"), 
                              BCM_PKT_NO_VLAN_TAG(pkt) ? "untagged" : "tagged", 
                   BCM_PKT_HAS_HGHDR(pkt) ? "with" : "without", rcpu_pkt_len));
        return BCM_E_PARAM; 
    }

    if (pkt->flags & BCM_TX_CRC_ALLOC) {
        rcpu_pkt_len += ENET_FCS_SIZE;
    }

    RCPU_TRANS_PTR_SET(unit);
    _rcpu_trans_ptr->rcpu_tp_data_alloc(_rcpu_trans_ptr->tp_unit,
                                        rcpu_pkt_len,
                                        (void*)&buf_ptr);
    if (NULL == buf_ptr) {
        return BCM_E_MEMORY;
    }

    tx_buf = buf_ptr;

    _tx_rcpu_higig_hdr_setup(unit, pkt, 
                             buf_ptr + sizeof(rcpu1_cmic_pkt_hdr_t), 
                             &hdr_size, &blk_ofst, &byte_ofst);

    payload_len = hdr_size;
    buf_ptr = tx_buf + sizeof(rcpu1_cmic_pkt_hdr_t) + hdr_size;
    for (i = blk_ofst; i < pkt->blk_count; i++) {
        tmp_len = pkt->pkt_data[i].len - byte_ofst;
        sal_memcpy(buf_ptr, 
                   &pkt->pkt_data[i].data[byte_ofst],
                   tmp_len);
        buf_ptr += tmp_len;
        payload_len += tmp_len;
        byte_ofst = 0;
    }

    if (pkt->flags & BCM_TX_CRC_ALLOC) {
        payload_len += ENET_FCS_SIZE;
    }

    /* Now construct the CMIC header */
    info.datalen = (uint16)payload_len;
    info.flags = (mh_flags == 1)? SOC_RCPU_FLAG_MODHDR : 0;
    info.operation = SOC_RCPU_OP_FROMCPU_PKT;
    info.transid = 0;
    info.replen = 0;
    soc_rcpu_encap(unit, tx_buf, &info);

    /* This is for in-band driver tx */
    pkt->_dv = cookie;

    if (bsl_check(bslLayerSoc, bslSourceTx, bslSeverityNormal, unit)) {
    	int j;
        LOG_INFO(BSL_LS_BCM_TX,
                 (BSL_META_U(unit,
                             "_bcm_rcpu_tx: packet: ")));
    	for (j = 0; j < payload_len + sizeof(rcpu1_cmic_pkt_hdr_t); j++) {
            LOG_INFO(BSL_LS_BCM_TX,
                     (BSL_META_U(unit,
                                 "%.2x"), tx_buf[j]));
    	}
    	LOG_INFO(BSL_LS_BCM_TX,
                 (BSL_META_U(unit,
                             "\n")));
    }

    rv = _rcpu_trans_ptr->rcpu_tp_tx(_rcpu_trans_ptr->tp_unit,
                                    (void*)tx_buf,
                                    payload_len + sizeof(rcpu1_cmic_pkt_hdr_t),
                                    (void *)pkt);
    return rv;
}

extern int rcpu_rx_pkt_enqueue(int unit, bcm_pkt_t *ptk);

/*
 * Function:
 *      _bcm_rcpu_tx_list
 * Purpose:
 *      Transmit a linked list of packets via RCPU mechanism
 * Parameters:
 *      unit - transmission unit
 *      pkt - Pointer to linked list of packets
 *      all_done_cb - callback when complete
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This mimics the behavior of the XGS implementation of
 *      of bcm_tx_list().
 */
int 
_bcm_rcpu_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb, void *cookie)
{
    bcm_pkt_t *cur_pkt, *next_pkt;

    for (cur_pkt = pkt; cur_pkt; cur_pkt = next_pkt) {
        next_pkt = cur_pkt->next;
        if (!cur_pkt->next && all_done_cb) {
            _rcpu_tx_list_callback_data_t *data;

            /* If this is the last packet and there's a completion
               callback, then chain _rcpu_tx_list_callback to this
               packet's callback. */
            data = sal_alloc(sizeof(_rcpu_tx_list_callback_data_t),
                             "_bcm_rcpu_tx_list");
            if (!data) {
                return BCM_E_MEMORY;
            }

            data->call_back = cur_pkt->call_back;
            data->cookie = cur_pkt->cookie;
            data->all_done = all_done_cb;
            data->pkt_list = pkt;
            cur_pkt->call_back = _rcpu_tx_list_callback;
            cur_pkt->cookie  = (void *)data;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_rcpu_tx(unit, cur_pkt, cookie));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_rcpu_tx_array
 * Purpose:
 *      Transmit am array of packets via RCPU mechanism
 * Parameters:
 *      unit - transmission unit
 *      pkt - Pointer to an array of packets
 *      count - number of packets in the array
 *      all_done_cb - callback when complete
 *      cookie - Callback cookie.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This mimics the behavior of the XGS implementation of
 *      of bcm_tx_array().
 */
int 
_bcm_rcpu_tx_array(int unit, bcm_pkt_t **pkt, int count, 
                   bcm_pkt_cb_f all_done_cb, void *cookie)
{
    int       i;

    for (i = 0; i < count; i++) {
        if ((i == count - 1) && all_done_cb) {
            _rcpu_tx_list_callback_data_t *data;

            /* If this is the last packet and there's a completion
               callback, then chain _rcpu_tx_list_callback to this
               packet's callback. */
            data = sal_alloc(sizeof(_rcpu_tx_list_callback_data_t),
                             "_bcm_rcpu_tx_list");
            if (!data) {
                return BCM_E_MEMORY;
            }

            data->call_back = pkt[i]->call_back;
            data->cookie = pkt[i]->cookie;
            data->all_done = all_done_cb;
            data->pkt_list = pkt[0];
            pkt[i]->call_back = _rcpu_tx_list_callback;
            pkt[i]->cookie  = (void *)data;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_rcpu_tx(unit, pkt[i], cookie));
    }

    return BCM_E_NONE;
}

int 
_bcm_esw_rcpu_tocpu_rx(int unit, uint8 *pkt, int pkt_len, rcpu_encap_info_t *info)
{
    int               len, rv;
    bcm_pkt_t         *rx_pkt = NULL;
    dcb_t             *dcb;

    /* 
     * CMIC header is followed by dcb information.
     */
    len = pkt_len - info->cmic_hdr_size - info->dcb_size;
    /* Allocate pkt. */
    rv = bcm_rx_remote_pkt_alloc(len, &rx_pkt);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    sal_memcpy(rx_pkt->pkt_data[0].data, pkt + info->cmic_hdr_size + info->dcb_size , len);
    rx_pkt->pkt_len = len;
    rx_pkt->unit = unit;
    /* 
     * The dcb in ToCPU packets starts with module header, so make it aligned
     * with dcb structure.
     */
    dcb = (dcb_t *) (pkt + info->cmic_hdr_size - 8); 
    _bcm_rcpu_rx_process_dcb(unit, dcb, rx_pkt);

    rv = rcpu_rx_pkt_enqueue(unit, rx_pkt);
    if (BCM_FAILURE(rv)) {
        bcm_rx_remote_pkt_free(rx_pkt);
    }

    return rv;
}


int rx_rcpu_base_info_parse(bcm_pkt_t *rx_pkt)
{
    int                     unit, len, pkt_len;
    dcb_t                   *dcb;
    uint8                   *pkt, *pkt_start_p, *pkt_data_p;
    rcpu_encap_info_t info;

    pkt = pkt_start_p = rx_pkt->pkt_data[0].data;
    pkt_len = rx_pkt->pkt_len;
    
	unit = rx_pkt->unit;
    SOC_IF_ERROR_RETURN(soc_rcpu_decap(pkt, &unit, &info));

    /* Accept only ToCPU packets */
    if (SOC_RCPU_OP_TOCPU_PKT != info.operation) {
        return BCM_E_PARAM;
    }
    
    
    len = pkt_len - info.cmic_hdr_size - info.dcb_size;
    dcb = (dcb_t *) ((uint8*) pkt + info.cmic_hdr_size - 8); 
        /* Move the data to the head of the packet */
    pkt_data_p = pkt + info.cmic_hdr_size + info.dcb_size;

    sal_memcpy(pkt_start_p, pkt_data_p, len);

    rx_pkt->pkt_data[0].len = len;
    rx_pkt->pkt_len = len;
    rx_pkt->tot_len = len;
    rx_pkt->blk_count = 1;
    rx_pkt->alloc_ptr = rx_pkt->_pkt_data.data;
    
    rx_pkt->rx_unit = unit;
    rx_pkt->_dcb = (void *)dcb;
    return BCM_E_NONE;
}

int rx_rcpu_intr_process_packet(bcm_pkt_t *rx_pkt, int *runit)
{
    int                     unit;
    dcb_t                   *dcb;
    bcm_port_t              src_port_tgid;

    if (rx_pkt->rx_unit > 0) {
    	/*base info has been parsed*/
    } else {
    	SOC_IF_ERROR_RETURN(rx_rcpu_base_info_parse(rx_pkt));
    } 
    
    *runit = unit = rx_pkt->rx_unit; 
    dcb = (dcb_t *)(rx_pkt->_dcb);
    
    rx_pkt->opcode = SOC_DCB_RX_OPCODE_GET(unit, dcb);
    rx_pkt->dest_mod = SOC_DCB_RX_DESTMOD_GET(unit, dcb);
    rx_pkt->dest_port = SOC_DCB_RX_DESTPORT_GET(unit, dcb);
    rx_pkt->src_mod = SOC_DCB_RX_SRCMOD_GET(unit, dcb);
    src_port_tgid = SOC_DCB_RX_SRCPORT_GET(unit, dcb);
    if (!soc_feature(unit, soc_feature_trunk_group_overlay) &&
        (src_port_tgid & BCM_TGID_TRUNK_INDICATOR(unit))) {
        rx_pkt->src_trunk = src_port_tgid & BCM_TGID_PORT_TRUNK_MASK(unit);
        rx_pkt->flags |= BCM_PKT_F_TRUNK;
        rx_pkt->src_port = -1;
    } else {
        rx_pkt->src_port = src_port_tgid;
        rx_pkt->src_trunk = -1;
    }
    rx_pkt->cos = SOC_DCB_RX_COS_GET(unit, dcb);
    rx_pkt->prio_int = BCM_PKT_VLAN_PRI(rx_pkt);
    rx_pkt->vlan = BCM_PKT_VLAN_ID(rx_pkt);
    rx_pkt->rx_untagged = SOC_DCB_RX_UNTAGGED_GET(unit, dcb);
    
    if (SOC_DCB_RX_MIRROR_GET(unit, dcb)) {
        rx_pkt->flags |= BCM_RX_MIRRORED;
    } 
    rx_pkt->rx_classification_tag = SOC_DCB_RX_CLASSTAG_GET(unit, dcb);
    rx_pkt->rx_matched = SOC_DCB_RX_MATCHRULE_GET(unit, dcb);

    if (soc_feature(unit, soc_feature_rx_timestamp)) {
        /* Get time stamp value for TS protocol packets */
        rx_pkt->rx_timestamp = SOC_DCB_RX_TIMESTAMP_GET(unit, dcb); 
    }
    
    rx_pkt->rx_reason = SOC_DCB_RX_REASON_GET(unit, dcb);
    SOC_DCB_RX_REASONS_GET(unit, dcb, &rx_pkt->rx_reasons);
    
    rx_pkt->rx_decap_tunnel = 
        (bcm_rx_decap_tunnel_t)SOC_DCB_RX_DECAP_TUNNEL_GET(unit, dcb);

    return BCM_E_NONE;
}

STATIC bcm_rx_t
_bcm_rcpu_pkt_rx(int unit, bcm_pkt_t * rx_pkt, void * cookie)
{
    int rv;

    if (rx_pkt->blk_count == 1) {
        if (rcpu_cb[unit].rcb_function) {
            rv = (*rcpu_cb[unit].rcb_function)(unit, rx_pkt->pkt_data[0].data,
                                               rx_pkt->pkt_len,
                                               rcpu_cb[unit].rcb_cookie);                                      
            if (rv == BCM_E_NONE) {
                return BCM_RX_HANDLED;
            }
        }
    }

    return BCM_RX_NOT_HANDLED;
}

STATIC void 
_rcpu_modid_chg_cb(int unit, soc_switch_event_t  event, uint32 arg1, 
                   uint32 arg2, uint32 arg3, void* userdata)
{
    switch (event) {
        case SOC_SWITCH_EVENT_MODID_CHANGE:
            _bcm_rcpu_init(unit);
            break;
        default:
            break;
    }

    return;
}
/*
 * Function:
 *      _bcm_esw_rcpu_init
 * Purpose:
 *      Initialize RCPU configuration for below configurations:
 *      (1)Dual-Rapotr/Raven
 *      (2)Stand alone unit ready to be accessed by RCPU
 * Parameters:
 *      unit - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_rcpu_init(int unit)
{
    int                 rv = BCM_E_NONE;
    soc_pbmp_t          pbmp;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_rcpu_1) || SOC_IS_RCPU_ONLY(unit)) {
        /* Do not need this if this is RCPU master unit. */
        return BCM_E_NONE;
    }

    if (!SOC_IS_RCPU_UNIT(unit)) {
        /* 
         * Use rcpu_rx_pbmp to indicate this is stand alone unit and needs to 
         * be accessed by RCPU. 
         */
        pbmp = soc_property_get_pbmp(unit, spn_RCPU_RX_PBMP, 0);
        if (SOC_PBMP_IS_NULL(pbmp)) {
            return BCM_E_NONE;
        }
    }

    if (BCM_RCPU_CONTROL(unit) && 
        (BCM_RCPU_CONTROL(unit)->flags & BCM_RCPU_F_INITED)) {
        return BCM_E_NONE;
    }

    if (BCM_RCPU_CONTROL(unit) == NULL) {
        BCM_RCPU_CONTROL(unit) = sal_alloc(sizeof(_bcm_rcpu_control_t), 
                                           "rcpu_control");
        if (BCM_RCPU_CONTROL(unit) == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(BCM_RCPU_CONTROL(unit), 0, sizeof(_bcm_rcpu_control_t));

        BCM_RCPU_CONTROL(unit)->lock = sal_mutex_create("rcpu_unit_mutex");
        if (BCM_RCPU_CONTROL(unit)->lock == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /*
     * Get the config for the device.
     */
    rv = _bcm_rcpu_get_config(unit);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RCPU,
                  (BSL_META_U(unit,
                              "_bcm_esw_rcpu_init: Failed to get default config.\n")));
        goto err_cfg;        
    }

    rv = _bcm_rcpu_init(unit);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_RCPU,
                  (BSL_META_U(unit,
                              "_bcm_esw_rcpu_init: Failed to init.\n")));
        goto err_cfg;        
    }

    if (SOC_IS_RCPU_UNIT(unit)) {
        /* 
         * Assign soc_rcpu_cfg_t and transport vectors to make it compatible to 
         * RCPU_ONLY devices.
         */
        soc_cm_set_rcpu_cfg(unit, &BCM_RCPU_CFG(unit)->rcpu_cfg);
#if defined(BCM_OOB_RCPU_SUPPORT)
        if (soc_property_get(unit, spn_RCPU_USE_OOB, 0)) {
            soc_cm_set_rcpu_trans_tpr(unit, &rcpu_oob_trans_ptr);
        } else 
#endif /* BCM_OOB_RCPU_SUPPORT */
        {
            soc_cm_set_rcpu_trans_tpr(unit, &rcpu_trans_ptr);
        }

        /* Use the same routine of RCPU_ONLY device to receive ToCPU packets. */
        rv = soc_rcpu_init(unit);
    }

    /* Register a call back function to be notified upon modid changes */
    BCM_IF_ERROR_RETURN(soc_event_register(unit, _rcpu_modid_chg_cb, NULL));

    BCM_RCPU_CONTROL(unit)->flags |= BCM_RCPU_F_INITED;

    return rv;

err_cfg:

    if (BCM_RCPU_CONTROL(unit)) {
        sal_free(BCM_RCPU_CONTROL(unit));
        BCM_RCPU_CONTROL(unit) = NULL;
    }

    return rv;
}

int
_bcm_esw_rcpu_deinit(int unit)
{
    if ((!SOC_UNIT_VALID(unit))){
        return BCM_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_rcpu_1) || SOC_IS_RCPU_ONLY(unit)) {
        /* Do not need this if this is RCPU master unit. */
        return BCM_E_NONE;
    }

    if (BCM_RCPU_CONTROL(unit)) {

        if (BCM_RCPU_CONTROL(unit)->lock) {
            sal_mutex_destroy(BCM_RCPU_CONTROL(unit)->lock);
        }

        sal_free(BCM_RCPU_CONTROL(unit));
        BCM_RCPU_CONTROL(unit) = NULL;

        soc_rcpu_deinit(unit);
    }

    (void)soc_event_unregister(unit, _rcpu_modid_chg_cb, NULL);

    return BCM_E_NONE;
}
#else
int
_bcm_esw_rcpu_init(int unit)
{
    COMPILER_REFERENCE(unit);
    return BCM_E_UNAVAIL;
}

int
_bcm_esw_rcpu_deinit(int unit)
{
    COMPILER_REFERENCE(unit);
    return BCM_E_UNAVAIL;
}

int 
_bcm_rcpu_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(pkt);
    COMPILER_REFERENCE(cookie);
    return BCM_E_UNAVAIL;
}

int 
_bcm_rcpu_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb, void *cookie)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(pkt);
    COMPILER_REFERENCE(all_done_cb);
    COMPILER_REFERENCE(cookie);
    return BCM_E_UNAVAIL;
}

int 
_bcm_rcpu_tx_array(int unit, bcm_pkt_t **pkt, int count, 
                   bcm_pkt_cb_f all_done_cb, void *cookie)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(pkt);
    COMPILER_REFERENCE(count);
    COMPILER_REFERENCE(all_done_cb);
    COMPILER_REFERENCE(cookie);
    return BCM_E_UNAVAIL;
}

int rx_rcpu_intr_process_packet(bcm_pkt_t *rx_pkt, int *runit)
{
    COMPILER_REFERENCE(rx_pkt);
    COMPILER_REFERENCE(runit);
    return BCM_E_UNAVAIL;
}

#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */
