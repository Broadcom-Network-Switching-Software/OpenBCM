/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_RCPU_H
#define _BCM_INT_RCPU_H

#include <soc/rcpu.h>

#ifdef INCLUDE_RCPU

extern int _bcm_esw_rcpu_init(int unit);
extern int _bcm_esw_rcpu_deinit(int unit);
extern int _bcm_rcpu_tx(int unit, bcm_pkt_t *pkt, void *cookie);
extern int _bcm_rcpu_tx_list(int unit, bcm_pkt_t *pkt,
                              bcm_pkt_cb_f all_done_cb, void *cookie);
extern int _bcm_rcpu_tx_array(int unit, bcm_pkt_t **pkt, int count, 
                              bcm_pkt_cb_f all_done_cb, void *cookie);
extern int 
_bcm_esw_rcpu_tocpu_rx(int unit, uint8 *pkt, int pkt_len, rcpu_encap_info_t *info);

typedef struct bcm_rcpu_cfg_s {
    soc_rcpu_cfg_t rcpu_cfg;
    soc_pbmp_t     pbmp;       /* remote CMIC RX port bitmap       */
    uint32         dot1pri[8]; /* 802.1p priority to be used for each COS */
    uint32         mh_tc[8];   /* Module header traffic class to be used for each COS */
    uint32         cpu_tc[8];  /* CPU traffic class to be used for each COS */      
} bcm_rcpu_cfg_t;

#define RCPU_MAX_BUF_LEN        (RX_PKT_SIZE_DFLT + 32 + 64)

#define BCM_RCPU_CFG(_unit)    (&BCM_RCPU_CONTROL(_unit)->cfg)

#define BCM_RCPU_CFG_LMAC(_unit)       \
            BCM_RCPU_CONTROL(_unit)->cfg.rcpu_cfg.dst_mac
#define BCM_RCPU_CFG_SRC_MAC(_unit)       \
            BCM_RCPU_CONTROL(_unit)->cfg.rcpu_cfg.src_mac

#define BCM_RCPU_F_INITED       0x10000

typedef struct bcm_rcpu_control_s {
    bcm_rcpu_cfg_t      cfg;
    sal_mutex_t         lock;
    uint32              flags;
} _bcm_rcpu_control_t;

#define BCM_RCPU_MAX_UNITS       BCM_UNITS_MAX 
extern _bcm_rcpu_control_t      *_bcm_rcpu_control[BCM_RCPU_MAX_UNITS];

#define BCM_RCPU_CONTROL(_unit)     _bcm_rcpu_control[_unit]
#define BCM_RCPU_UNIT_VALID(_unit)  ((_unit) >= 0 && \
				     (_unit) < BCM_RCPU_MAX_UNITS && \
				     BCM_RCPU_CONTROL(_unit) != 0)

#define	RCPU_UNIT_LOCK(unit)   sal_mutex_take(BCM_RCPU_CONTROL((unit))->lock, \
                                               sal_mutex_FOREVER)
#define	RCPU_UNIT_UNLOCK(unit) sal_mutex_give(BCM_RCPU_CONTROL((unit))->lock)

#define BCM_RCPU_RX_PRIO            BCM_RX_PRIO_MAX

#endif /* INCLUDE_RCPU */

#endif /* _BCM_INT_RCPU_H */

