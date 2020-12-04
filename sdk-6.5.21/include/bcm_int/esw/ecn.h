/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ECN - Broadcom StrataSwitch ECN API internal definitions 
 */

#ifndef _BCM_INT_ECN_H
#define _BCM_INT_ECN_H

/* ECN max value */
#define _BCM_ECN_VALUE_MAX               (3)

/* Max Internal Congestion value */
#define _BCM_ECN_INT_CN_VALUE_MAX        (3)

/* Action dequeue memory index */
#define _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET          (0x5)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_GREEN_OFFSET        (0x4)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_YELLOW_OFFSET       (0x3)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_RED_OFFSET          (0x2)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET             (0x0)

#define _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET_TD3      (0x3)
#define _BCM_ECN_DEQUEUE_MEM_INDEX_CE_OFFSET_TD3          (0x2)

/* Action egress memory index*/
#define _BCM_ECN_EGRESS_MEM_INDEX_INT_CN_OFFSET           (0x2)
#define _BCM_ECN_EGRESS_MEM_INDEX_ECN_OFFSET              (0x0)

/* Internal Congestion Action Values */
#define _BCM_ECN_INT_CN_RESPONSIVE_DROP                   (0x0)
#define _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP               (0x1)
#define _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN           (0x2)
#define _BCM_ECN_INT_CN_CONGESTION_MARK_ECN               (0x3)

/* ECN */
extern int
_bcm_esw_ecn_init(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_ecn_sw_dump(int unit);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
extern int
bcmi_xgs5_ecn_scache_size_get(int unit, uint32 *scache_size);
 
#ifdef BCM_WARM_BOOT_SUPPORT
extern int 
bcmi_xgs5_ecn_sync(int unit, uint8 **scache_ptr);
extern int
bcmi_xgs5_ecn_reinit(int unit, uint8 **scache_ptr); 
#endif /* BCM_WARM_BOOT_SUPPORT */

#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK         0xC00
#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_EXP2ECN      0x400
#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_ECN2EXP      0x800
#define _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP    0xC00
#define _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK          0x0ff

#define _BCM_ECN_MAX_ENTRIES_PER_MAP             0x20
#define _BCM_ING_ECN_MAX_ENTRIES_PER_MAP         0x40

#define _BCM_XGS5_ECN_MAP_TYPE_MASK               0xC000
#define _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM        0x4000
#define _BCM_XGS5_ECN_MAP_NUM_MASK                0x0ff
#define _BCM_ECN_MAX_ENTRIES_PER_TUNNEL_TERM_MAP  0x10
#define _BCM_ECN_MAX_ENTRIES_PER_TUNNEL_TERM_NONIP_MAP  0x4

#define _BCM_ECN_ENCAP_ENTRIES   4

typedef enum _bcm_ecn_map_type_s {
    _bcmEcnmapTypeExp2Ecn,      /* Ingress_EXP_TO_ECN map */
    _bcmEcnmapTypeIpEcn2Exp,    /* Egress_IP_ECN_TO_EXP map */
    _bcmEcnmapTypeIntcn2Exp,    /* Egress_INT_CN_TO_EXP map */
    _bcmEcnmapTypeTunnelTerm   /* Ingress Tunnel termination ecn map */
} _bcm_ecn_map_type_e;
extern int
bcmi_xgs5_ecn_map_used_get(int unit, int map_index, _bcm_ecn_map_type_e type);
extern int
bcmi_ecn_map_id2hw_idx (int unit,int ecn_map_id,int *hw_idx);
extern int
bcmi_ecn_map_hw_idx2id (int unit, int hw_idx, int type, int *ecn_map_id);
extern void bcmi_ecn_map_clean_up(int unit);
#endif

#endif /* _BCM_INT_ECN_H */
