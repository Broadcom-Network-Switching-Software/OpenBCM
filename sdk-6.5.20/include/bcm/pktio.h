/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PKTIO_H__
#define __BCM_PKTIO_H__

#include <shared/bitop.h>
#include <bcm/types.h>

#define BCM_PKTIO_PMD_SIZE_WORDS    48         /* Packet metadata size. */

/* Packet metadata information. */
typedef struct bcm_pktio_pmd_s {
    uint32 *rxpmd;                      /* RX Packet metadata handle. */
    uint32 *txpmd;                      /* TX Packet metadata handle. */
    uint32 *higig;                      /* HiGig handle. */
    uint32 *lbhdr;                      /* Loopback Header handle. */
    uint32 data[BCM_PKTIO_PMD_SIZE_WORDS]; /* Headers' data. */
} bcm_pktio_pmd_t;

extern void bcm_pktio_pmd_t_init(
    bcm_pktio_pmd_t *pmd);

typedef struct bcm_pktio_pkt_s bcm_pktio_pkt_t;

/* Packet structure. */
struct bcm_pktio_pkt_s { 
    bcm_pktio_pkt_t *next;  /* Point to next packet in the list. */
    bcm_pktio_pkt_t *prev;  /* Point to previous packet in the list. */
    int unit;               /* Switch unit number. */
    uint32 flags;           /* Internal used only. */
    uint32 type;            /* Packet forwarding type. */
    bcm_pktio_pmd_t pmd;    /* Packet metadata information. */
    void *data_buf;         /* Packet data buffer information. */
};

#define BCM_PKTIO_BUF_F_TX      (0x1 << 0) /* Reserve space for TX overhead. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Allocate a packet structure and packet data. */
extern int bcm_pktio_alloc(
    int unit, 
    uint32 max_size, 
    uint32 flags, 
    bcm_pktio_pkt_t **packet);

/* Release packet data buffer and destroy packet object. */
extern int bcm_pktio_free(
    int unit, 
    bcm_pktio_pkt_t *packet);

/* Claim the packet. */
extern int bcm_pktio_claim(
    int unit, 
    bcm_pktio_pkt_t *pkt, 
    bcm_pktio_pkt_t **new_pkt);

/* Retrieve packet data handle and length. */
extern int bcm_pktio_pkt_data_get(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    void **data, 
    uint32 *length);

/* Reserve buffer size in headroom. */
extern int bcm_pktio_reserve(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    uint32 len, 
    void **data);

/* Add extra size in the front of a packet data. */
extern int bcm_pktio_push(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    uint32 len, 
    void **data);

/* Add bytes in the end of a packet data. */
extern int bcm_pktio_put(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    uint32 len, 
    void **data);

/* Remove bytes from the start of packet data. */
extern int bcm_pktio_pull(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    uint32 len, 
    void **data);

/* Remove end from packet buffer. */
extern int bcm_pktio_trim(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    uint32 len);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_RXPMD_FLEX_FID_OFFSET     256        /* Offset to be added to
                                                          RXPMD FLEX field. */
#define BCM_PKTIO_RXPMD_FLEX_FID_START      (0 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_ING_TIMESTAMP_15_0 (0 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L3_IIF_13_0    (1 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_15_0 (2 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_NHOP_INDEX_1_14_0 (3 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_VFI_15_0       (4 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L2_OIF_10_0    (5 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_I2E_CLASS_ID_15_0 (6 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_PKT_MISC_CTRL_0_3_0 (7 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_DLB_ECMP_DESTINATION_15_0 (8 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_SVP_NETWORK_GROUP_BITMAP_3_0 (9 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_SYSTEM_DESTINATION_15_0 (10 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L2_IIF_10_0    (11 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_NHOP_2_OR_ECMP_GROUP_INDEX_1_14_0 (12 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L3_OIF_1_13_0  (13 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_DVP_15_0       (14 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_47_32 (15 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_ENTROPY_LABEL_LOW_15_0 (16 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_SYSTEM_OPCODE_3_0 (17 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_ING_TIMESTAMP_31_16 (18 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_SYSTEM_SOURCE_15_0 (19 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_ENTROPY_LABEL_HIGH_3_0 (20 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L2_IIF_QOS_REMARK_CTRL_3_0 (21 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_TIMESTAMP_CTRL_3_0 (22 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_31_16 (23 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_INGRESS_PP_PORT_7_0 (24 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_IFP_TS_CONTROL_ACTION_3_0 (25 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EFFECTIVE_TTL_7_0 (26 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_SVP_15_0       (27 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_DROP_CODE_15_0 (28 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_INT_PRI_3_0    (29 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_TUNNEL_PROCESSING_RESULTS_1_3_0 (30 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_PARSER_VHLEN_0_15_0 (31 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_VLAN_TAG_PRESERVE_CTRL_1_0 (32 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_IFP_IOAM_ACTION_3_0 (33 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_MPLS_LABEL_DECAP_COUNT_3_0 (34 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L3_DNAT_INDEX_15_0 (35 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_NAT_CTRL_3_0   (36 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_L3_SNAT_INDEX_15_0 (37 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0 (38 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_NHOP_2_OR_ECMP_GROUP_INDEX_1_15_0 (39 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_NHOP_INDEX_1_15_0 (40 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_INGRESS_QOS_REMAP_VALUE_15_0 (41 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_INGRESS_QOS_REMARK_CTRL_3_0 (42 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_IFP_TO_EP_MACSEC_INFO_15_0 (43 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_IFP_OPAQUE_OBJ0_15_0 (44 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_IFP_IOAM_GBP_ACTION_3_0 (45 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EVENT_TRACE_VECTOR_31_0 (46 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_ING_TIMESTAMP_31_0 (47 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EP_NIH_HDR_TIMESTAMP_15_0 (48 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EP_NIH_HDR_TIMESTAMP_31_16 (49 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EGR_MTOP_INDEX_LO_15_0 (50 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EGR_MTOP_INDEX_HI_3_0 (51 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_IPV4_GATEWAY_LOOKUP_HIT_VRF_STRENGTH_3_3 (52 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_EM_FT_OPAQUE_OBJ_OR_IFP_OPAQUE_OBJ_15_0 (53 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_INGRESS_QOS_REMAP_VALUE_OR_IFP_OPAQUE_OBJ_15_0 (54 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_FID_END        (54 + BCM_PKTIO_RXPMD_FLEX_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_FID_COUNT      55         /* <UNDEF> */

#define BCM_PKTIO_GENERIC_LOOPBACK_T_FID_START 0          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_INPUT_PRIORITY 0          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_RESERVED_1 1          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_RESERVED_0 2          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_INTERFACE_OBJ 3          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_PROCESSING_CTRL_1 4          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_PROCESSING_CTRL_0 5          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_DESTINATION_OBJ 6          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_SOURCE_SYSTEM_PORT 7          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_HEADER_TYPE 8          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_INTERFACE_CTRL 9          /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_FLAGS  10         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_ENTROPY_OBJ 11         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_DESTINATION_TYPE 12         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_RESERVED_2 13         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_QOS_OBJ 14         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_START_BYTE 15         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_FID_END 15         /* <UNDEF> */
#define BCM_PKTIO_GENERIC_LOOPBACK_T_FID_COUNT 16         /* <UNDEF> */

#define BCM_PKTIO_HG3_BASE_T_FID_START      0          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_SYSTEM_SOURCE  0          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_HG3_RESERVED   1          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_CN             2          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_EXT_HDR_PRESENT 3          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_MIRROR_COPY    4          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_L3_ROUTED      5          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_CNG            6          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_RESERVED_ETYPE 7          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_VERSION        8          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_SYSTEM_DESTINATION 9          /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_ENTROPY        10         /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_TC             11         /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_SYSTEM_DESTINATION_TYPE 12         /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_FID_END        12         /* <UNDEF> */
#define BCM_PKTIO_HG3_BASE_T_FID_COUNT      13         /* <UNDEF> */

#define BCM_PKTIO_HG3_EXT0_FID_OFFSET       32         /* Offset to be added to
                                                          higig3 ext0 fids. */
#define BCM_PKTIO_HG3_EXTENSION_0_T_FID_START (0 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_FORWARDING_DOMAIN (0 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_DVP_OR_L3_IIF (1 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_FLAGS   (2 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_CLASS_ID (3 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_SVP     (4 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_FID_END (4 + BCM_PKTIO_HG3_EXT0_FID_OFFSET) /* <UNDEF> */
#define BCM_PKTIO_HG3_EXTENSION_0_T_FID_COUNT 5          /* <UNDEF> */

#define BCM_PKTIO_BITMAP_WORD_SIZE  16         /* Bitmap array size. */

typedef struct bcm_pktio_bitmap_s {
    uint32 pbits[BCM_PKTIO_BITMAP_WORD_SIZE]; 
} bcm_pktio_bitmap_t;

#define BCM_PKTIO_FID_INVALID   -1         /* Invalid field id. */

#define BCM_PKTIO_FID_COUNT     BCM_PKTIO_BITMAP_WORD_SIZE * 32 /* Field id number. */

/* Packet metadata field ID supported bit array. */
typedef struct bcm_pktio_fid_support_s {
    bcm_pktio_bitmap_t txpmd_support;   /* TXPMD field ID supported bit array. */
    bcm_pktio_bitmap_t rxpmd_support;   /* RXPMD field ID supported bit array. */
    bcm_pktio_bitmap_t gih_support;     /* Generic loopback field ID supported
                                           bit array. */
    bcm_pktio_bitmap_t rxflex_support;  /* FLEXPMD field ID supported bit array. */
    bcm_pktio_bitmap_t higig3_support;  /* Higig3 field ID supported bit array. */
} bcm_pktio_fid_support_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the pmd support bitmap of different pmd types. */
extern int bcm_pktio_pmd_fid_support_get(
    int unit, 
    bcm_pktio_fid_support_t *support);

/* Get the pmd support bitmap of different pmd types. */
extern int bcm_pktio_raw_pmd_fid_support_get(
    int unit, 
    bcm_pktio_pmd_t *pmd, 
    bcm_pktio_fid_support_t *support);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Macro to get a field ID's supported status.
 * 
 * Returns:
 *   zero     => Not supported
 *   non-zero => Supported
 */
#define BCM_PKTIO_FID_SUPPORT_GET(_support, _fid)  \
    SHR_BITGET(((_support).pbits), (_fid)) 

/* Macro to iterate over all supported field IDs in the _support. */
#define BCM_PKTIO_FID_SUPPORT_ITER(_support, _fid)  \
    for ((_fid) = BCM_PKTIO_FID_INVALID + 1; \
         (_fid) < BCM_PKTIO_FID_COUNT; \
         (_fid)++) \
        if (BCM_PKTIO_FID_SUPPORT_GET(_support, _fid)) 

/* Packet Metadata Types. */
typedef enum bcm_pktio_pmd_type_e {
    bcmPktioPmdTypeTx = 0, 
    bcmPktioPmdTypeRx = 1, 
    bcmPktioPmdTypeGih = 2, 
    bcmPktioPmdTypeRxFlex = 3, 
    bcmPktioPmdTypeHigig3 = 4 
} bcm_pktio_pmd_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set value into a specific PMD field. */
extern int bcm_pktio_pmd_field_set(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    bcm_pktio_pmd_type_t type, 
    int fid, 
    uint32 val);

/* Get value from a specific PMD field. */
extern int bcm_pktio_pmd_field_get(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    bcm_pktio_pmd_type_t type, 
    int fid, 
    uint32 *val);

/* Set value into a specific PMD field. */
extern int bcm_pktio_raw_pmd_field_set(
    int unit, 
    bcm_pktio_pmd_t *pmd, 
    bcm_pktio_pmd_type_t type, 
    int fid, 
    uint32 val);

/* Get value from a specific PMD field. */
extern int bcm_pktio_raw_pmd_field_get(
    int unit, 
    bcm_pktio_pmd_t *pmd, 
    bcm_pktio_pmd_type_t type, 
    int fid, 
    uint32 *val);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_REASON_NONE   0          /* No reason. */

#define BCM_PKTIO_RXPMD_FLEX_REASON_CML_FLAGS 0          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_STATIC_MOVE 1          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_PKT_INTEGRITY_CHECK_FAILED 2          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_PROTOCOL_PKT 3          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L2_DST_LOOKUP_MISS 4          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L2_DST_LOOKUP 5          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L3_DST_LOOKUP_MISS 6          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L3_DST_LOOKUP 7          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L3_HDR_ERROR 8          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L3_TTL_ERROR 9          /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_IPMC_L3_IIF_OR_RPA_ID_CHECK_FAILED 10         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_LEARN_CACHE_FULL 11         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_VFP     12         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_IFP     13         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_IFP_METER 14         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_DST_FP  15         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_SVP     16         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_EM_FT   17         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_IVXLT   18         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_0 19         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_1 20         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_2 21         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_3 22         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_4 23         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_5 24         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_6 25         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MATCHED_RULE_BIT_7 26         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_NO_COPY_TO_CPU 27         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_DISCARD 28         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MACSA_MULTICAST 29         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MEMBERSHIP_CHECK_FAILED 30         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_SPANNING_TREE_CHECK_FAILED 31         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MIRROR_SAMPLER_SAMPLED 32         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MIRROR_SAMPLER_EGR_SAMPLED 33         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_SER_DROP 34         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_URPF_CHECK_FAILED 35         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L3_IIF_EQ_L3_OIF 36         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_DLB_ECMP_MONITOR_EN_OR_MEMBER_REASSINED 37         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_DLB_LAG_MONITOR_EN_OR_MEMBER_REASSINED 38         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MPLS_CTRL_PKT_TO_CPU 39         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_MTOP_IPV4_GATEWAY 40         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_DLB_ECMP_PKT_SAMPLED 41         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_DLB_LAG_PKT_SAMPLED 42         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_TRACE_DO_NOT_MIRROR 43         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_TRACE_DO_NOT_COPY_TO_CPU 44         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_TRACE_DOP 45         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_LEARN 46         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_L2_SRC_RELEARN 47         /* <UNDEF> */
#define BCM_PKTIO_RXPMD_FLEX_REASON_COUNT   48         /* <UNDEF> */

#define BCM_PKTIO_REASON_COUNTS BCM_PKTIO_BITMAP_WORD_SIZE * 32 /* Reason id number. */

/* Packet reasons bitmap. */
typedef struct bcm_pktio_reasons_s {
    bcm_pktio_bitmap_t rx_reasons;      /* Packet rx reasons bitmap. */
    bcm_pktio_bitmap_t flex_reasons;    /* Packet flex reasons bitmap. */
} bcm_pktio_reasons_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get RX reasons from RXPMD and RXFLEXPMD. */
extern int bcm_pktio_pmd_reasons_get(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    bcm_pktio_reasons_t *reasons);

/* Set RX reasons into pmd data. */
extern int bcm_pktio_raw_pmd_reasons_set(
    int unit, 
    bcm_pktio_pmd_t *pmd, 
    bcm_pktio_reasons_t *reasons);

/* Get RX reasons from pmd data. */
extern int bcm_pktio_raw_pmd_reasons_get(
    int unit, 
    bcm_pktio_pmd_t *pmd, 
    bcm_pktio_reasons_t *reasons);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Macro to check if a reason is included in a set of reasons.
 * 
 * Returns:
 *   zero     => reason is not included in the set
 *   non-zero => reason is included in the set
 */
#define BCM_PKTIO_REASON_GET(_reasons, _reason)  \
    SHR_BITGET(((_reasons).pbits), (_reason)) 

/* Macro to add a reason to a set of reasons. */
#define BCM_PKTIO_REASON_SET(_reasons, _reason)  \
    SHR_BITSET(((_reasons).pbits), (_reason)) 

/* Macro to add all reasons to a set of reasons. */
#define BCM_PKTIO_REASON_SET_ALL(_reasons)  \
    SHR_BITSET_RANGE(((_reasons).pbits), 0, BCM_PKTIO_REASON_COUNTS) 

/* Macro to clear a reason from a set of reasons. */
#define BCM_PKTIO_REASON_CLEAR(_reasons, _reason)  \
    SHR_BITCLR(((_reasons).pbits), (_reason)) 

/* Macro to clear a set of reasons. */
#define BCM_PKTIO_REASON_CLEAR_ALL(_reasons)  \
    SHR_BITCLR_RANGE(((_reasons).pbits), 0, BCM_PKTIO_REASON_COUNTS) 

/* Macro to check if a set of reasons is empty. */
#define BCM_PKTIO_REASON_IS_NULL(_reasons)  \
    SHR_BITNULL_RANGE(((_reasons).pbits), 0, BCM_PKTIO_REASON_COUNTS) 

/* Macro to itterate over a set of reasons. */
#define BCM_PKTIO_REASON_ITER(_reasons, _reason)  \
    for((_reason) = BCM_PKTIO_REASON_NONE; \
        (_reason) < BCM_PKTIO_REASON_COUNTS; \
        (_reason)++) \
        if(BCM_PKTIO_REASON_GET(_reasons, _reason)) 

/* Macro to count number of active reasons in a set of reasons. */
#define BCM_PKTIO_REASON_COUNT(_reasons, _count)  \
    SHR_BITCOUNT_RANGE(((_reasons).pbits), _count, 0, BCM_PKTIO_REASON_COUNTS) 

/* Macro to compare two sets of reasons. */
#define BCM_PKTIO_REASON_EQ(_reasons1, _reasons2)  \
    SHR_BITEQ_RANGE(((_reasons1).pbits), ((_reasons2).pbits), \
                    0, BCM_PKTIO_REASON_COUNTS) 

#define BCM_PKTIO_MATCH_ID_NONE 0          /* No match id. */

#define BCM_PKTIO_MATCH_ID_INNER_L2_HDR_ETAG 0          /* Inner l2 hdr etag */
#define BCM_PKTIO_MATCH_ID_INNER_L2_HDR_ITAG 1          /* Inner l2 hdr itag */
#define BCM_PKTIO_MATCH_ID_INNER_L2_HDR_L2  2          /* Inner l2 hdr l2 */
#define BCM_PKTIO_MATCH_ID_INNER_L2_HDR_OTAG 3          /* Inner l2 hdr otag */
#define BCM_PKTIO_MATCH_ID_INNER_L2_HDR_SNAP_OR_LLC 4          /* Inner l2 hdr snap or
                                                          llc */
#define BCM_PKTIO_MATCH_ID_INNER_L2_HDR_VNTAG 5          /* Inner l2 hdr vntag */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_ARP 6          /* Inner l3 l4 hdr arp */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_AUTH_EXT_1 7          /* Inner l3 l4 hdr auth
                                                          ext 1 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_AUTH_EXT_2 8          /* Inner l3 l4 hdr auth
                                                          ext 2 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_BFD 9          /* Inner l3 l4 hdr bfd */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_ESP_EXT 10         /* Inner l3 l4 hdr esp
                                                          ext */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_ETHERTYPE 11         /* Inner l3 l4 hdr
                                                          ethertype */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_FRAG_EXT_1 12         /* Inner l3 l4 hdr frag
                                                          ext 1 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_FRAG_EXT_2 13         /* Inner l3 l4 hdr frag
                                                          ext 2 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_ICMP 14         /* Inner l3 l4 hdr icmp */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_IGMP 15         /* Inner l3 l4 hdr igmp */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_IPV4 16         /* Inner l3 l4 hdr ipv4 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_IPV6 17         /* Inner l3 l4 hdr ipv6 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_P_1588 18         /* Inner l3 l4 hdr p 1588 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_RARP 19         /* Inner l3 l4 hdr rarp */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_TCP_FIRST_4BYTES 20         /* Inner l3 l4 hdr tcp
                                                          first 4bytes */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_TCP_LAST_16BYTES 21         /* Inner l3 l4 hdr tcp
                                                          last 16bytes */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_UDP 22         /* Inner l3 l4 hdr udp */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_UNKNOWN_L3 23         /* Inner l3 l4 hdr
                                                          unknown l3 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_UNKNOWN_L4 24         /* Inner l3 l4 hdr
                                                          unknown l4 */
#define BCM_PKTIO_MATCH_ID_INNER_L3_L4_HDR_UNKNOWN_L5 25         /* Inner l3 l4 hdr
                                                          unknown l5 */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_ETAG 26         /* Outer l2 hdr etag */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_HG3_BASE 27         /* Outer l2 hdr hg3 base */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_HG3_EXT_0 28         /* Outer l2 hdr hg3 ext 0 */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_ITAG 29         /* Outer l2 hdr itag */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_L2  30         /* Outer l2 hdr l2 */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_OTAG 31         /* Outer l2 hdr otag */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_SNAP_OR_LLC 32         /* Outer l2 hdr snap or
                                                          llc */
#define BCM_PKTIO_MATCH_ID_OUTER_L2_HDR_VNTAG 33         /* Outer l2 hdr vntag */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_ARP 34         /* Outer l3 l4 hdr arp */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_AUTH_EXT_1 35         /* Outer l3 l4 hdr auth
                                                          ext 1 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_AUTH_EXT_2 36         /* Outer l3 l4 hdr auth
                                                          ext 2 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_BFD 37         /* Outer l3 l4 hdr bfd */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_ESP_EXT 38         /* Outer l3 l4 hdr esp
                                                          ext */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_ETHERTYPE 39         /* Outer l3 l4 hdr
                                                          ethertype */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_FRAG_EXT_1 40         /* Outer l3 l4 hdr frag
                                                          ext 1 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_FRAG_EXT_2 41         /* Outer l3 l4 hdr frag
                                                          ext 2 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_GPE 42         /* Outer l3 l4 hdr gpe */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_GRE 43         /* Outer l3 l4 hdr gre */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_GRE_CHKSUM 44         /* Outer l3 l4 hdr gre
                                                          chksum */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_GRE_KEY 45         /* Outer l3 l4 hdr gre
                                                          key */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_GRE_ROUT 46         /* Outer l3 l4 hdr gre
                                                          rout */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_GRE_SEQ 47         /* Outer l3 l4 hdr gre
                                                          seq */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_ICMP 48         /* Outer l3 l4 hdr icmp */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_IFA_METADATA 49         /* Outer l3 l4 hdr ifa
                                                          metadata */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_IGMP 50         /* Outer l3 l4 hdr igmp */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_IPV4 51         /* Outer l3 l4 hdr ipv4 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_IPV6 52         /* Outer l3 l4 hdr ipv6 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS0 53         /* Outer l3 l4 hdr mpls0 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS1 54         /* Outer l3 l4 hdr mpls1 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS2 55         /* Outer l3 l4 hdr mpls2 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS3 56         /* Outer l3 l4 hdr mpls3 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS4 57         /* Outer l3 l4 hdr mpls4 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS5 58         /* Outer l3 l4 hdr mpls5 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS6 59         /* Outer l3 l4 hdr mpls6 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS_ACH 60         /* Outer l3 l4 hdr mpls
                                                          ach */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_MPLS_CW 61         /* Outer l3 l4 hdr mpls
                                                          cw */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_P_1588 62         /* Outer l3 l4 hdr p 1588 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_RARP 63         /* Outer l3 l4 hdr rarp */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_TCP_FIRST_4BYTES 64         /* Outer l3 l4 hdr tcp
                                                          first 4bytes */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_TCP_LAST_16BYTES 65         /* Outer l3 l4 hdr tcp
                                                          last 16bytes */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_UDP 66         /* Outer l3 l4 hdr udp */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_UNKNOWN_L3 67         /* Outer l3 l4 hdr
                                                          unknown l3 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_UNKNOWN_L4 68         /* Outer l3 l4 hdr
                                                          unknown l4 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_UNKNOWN_L5 69         /* Outer l3 l4 hdr
                                                          unknown l5 */
#define BCM_PKTIO_MATCH_ID_OUTER_L3_L4_HDR_VXLAN 70         /* Outer l3 l4 hdr vxlan */
#define BCM_PKTIO_MATCH_ID_SYS_HDR_LOOPBACK 71         /* Sys hdr loopback */
#define BCM_PKTIO_MATCH_ID_COUNT            72         /* Maximum match id
                                                          count, always last */

#ifndef BCM_HIDE_DISPATCHABLE

/* Get packet format from the match ID array. */
extern int bcm_pktio_pmd_pkt_format_get(
    int unit, 
    int array_size, 
    uint32 *match_id_array, 
    int *count, 
    bcm_pktio_bitmap_t *pkt_format);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Macro to check if a match id is included in a set of match ids.
 * 
 * Returns:
 *   zero     => match id is not included in the set
 *   non-zero => match id is included in the set
 */
#define BCM_PKTIO_MATCH_ID_GET(_match_ids, _match_id)  \
    SHR_BITGET(((_match_ids).pbits), (_match_id)) 

/* Macro to itterate over a set of match ids. */
#define BCM_PKTIO_MATCH_ID_ITER(_match_ids, _match_id)  \
    for((_match_id) = BCM_PKTIO_MATCH_ID_NONE; \
        (_match_id) < BCM_PKTIO_MATCH_ID_COUNT; \
        (_match_id)++) \
        if(BCM_PKTIO_MATCH_ID_GET(_match_ids, _match_id)) 

#define BCM_PKTIO_TX_MC_QUEUE   0x1        /* Indicates packet goes to MC queue. */

/* Packet metadata. */
typedef struct bcm_pktio_txpmd_s {
    bcm_port_t tx_port; /* TX port. */
    uint8 prio_int;     /* Internal priority of the packet. */
    uint8 cos;          /* The local COS queue to use. */
    uint32 flags;       /* BCM_PKTIO_TX_xxx flags. */
} bcm_pktio_txpmd_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Transmit one packet. */
extern int bcm_pktio_pmd_set(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    bcm_pktio_txpmd_t *txpmd);

/* Transmit one packet. */
extern int bcm_pktio_tx(
    int unit, 
    bcm_pktio_pkt_t *packet);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Return values from packet I/O RX callout routines. */
typedef enum bcm_pktio_rx_e {
    BCM_PKTIO_RX_NOT_HANDLED = 0,   /* Packet not processed. Dispatcher will
                                       deliver it to the next callback. */
    BCM_PKTIO_RX_HANDLED = 1        /* Packet handled. Dispatcher will skip next
                                       callback. */
} bcm_pktio_rx_t;

/* Callback function type for applications using packet IO facility. */
typedef bcm_pktio_rx_t (*bcm_pktio_rx_cb_f)(
    int unit, 
    bcm_pktio_pkt_t *packet, 
    void *cookie);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register receive callbacks for received packets. */
extern int bcm_pktio_rx_register(
    int unit, 
    const char *name, 
    bcm_pktio_rx_cb_f callback, 
    uint8 priority, 
    void *cookie, 
    uint32 flags);

/* Unregister receive callbacks for received packets. */
extern int bcm_pktio_rx_unregister(
    int unit, 
    bcm_pktio_rx_cb_f callback, 
    uint8 priority);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_NETIF_NAME_MAX        16         /* Maximum chars are allowed
                                                      in network interface name. */
#define BCM_PKTIO_NETIF_USER_DATA_SIZE  64         /* Network interface user
                                                      data size in bytes. */

#define BCM_PKTIO_NETIF_F_RCPU_ENCAP    (1 << 0)   /* RCPU header encapsulation
                                                      is used for delivering
                                                      packet metadata to/from
                                                      applications.
                                                      Applications should
                                                      encapsulate RCPU header
                                                      when transmitting packets
                                                      to the netif,
                                                      and may parser RCPU header
                                                      when receiving packets
                                                      from the netif. */
#define BCM_PKTIO_NETIF_F_BIND_RX_CH    (1 << 1)   /* Bind this netif with a RX
                                                      DMA channel. All packets
                                                      from
                                                      the DMA channel will be
                                                      forwarded to this netif. */
#define BCM_PKTIO_NETIF_F_BIND_TX_PORT  (1 << 2)   /* Bind this network
                                                      interface to a front-panel
                                                      switch port for
                                                      transmission.  All packets
                                                      from this network
                                                      interface will be
                                                      forwarded to the bound
                                                      switch port. */
#define BCM_PKTIO_NETIF_F_ADD_TAG       (1 << 3)   /* Add VLAN tag for packets
                                                      transmitted via this
                                                      network interface. */

/* Packet network interface structure. */
typedef struct bcm_pktio_netif_s {
    int id;                             /* Network interface ID. */
    uint32 flags;                       /* Creation flag. Refer to
                                           BCM_PKTIO_NETIF_F_XXX flags. */
    bcm_mac_t mac_addr;                 /* MAC address associated with this
                                           network interface. */
    uint16 vlan;                        /* Default VLAN ID associated with this
                                           network interface. */
    bcm_port_t port;                    /* Bind with a network interface for
                                           transmit destination port. */
    char port_encap[10];                /* port_encap works with port to tell
                                           the encapsulation type of the port. */
    int port_queue;                     /* port_queue works with port to tell
                                           egress queue number. */
    int dma_chan_id;                    /* Bind a Rx DMA channel with this
                                           netif. */
    uint32 max_frame_size;              /* Max packet size is for this netif.
                                           Any packet which size is bigger than
                                           this setting will be dropped. */
    char name[BCM_PKTIO_NETIF_NAME_MAX]; /* Network interface name. */
    uint8 user_data[BCM_PKTIO_NETIF_USER_DATA_SIZE]; /* Network interface user data. */
} bcm_pktio_netif_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a kernel network interface. */
extern int bcm_pktio_netif_create(
    int unit, 
    bcm_pktio_netif_t *netif);

/* Get a kernel network interface configuration. */
extern int bcm_pktio_netif_get(
    int unit, 
    int netif_id, 
    bcm_pktio_netif_t *netif);

/* Destroy a kernel network interface. */
extern int bcm_pktio_netif_destroy(
    int unit, 
    int netif_id);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_NETIF_LINK_DOWN   0          /* Network interface link down. */
#define BCM_PKTIO_NETIF_LINK_UP     1          /* Network interface link up. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set a kernel network interface link status. */
extern int bcm_pktio_netif_link_status_set(
    int unit, 
    int netif_id, 
    int status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Network interface traverse callback function */
typedef int (*bcm_pktio_netif_traverse_cb_f)(
    int unit, 
    bcm_pktio_netif_t *netif, 
    void *cb_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse kernel network interface objects */
extern int bcm_pktio_netif_traverse(
    int unit, 
    bcm_pktio_netif_traverse_cb_f cb_func, 
    void *cb_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_FILTER_T_RX_PKT   1          /* Packet filter for RX
                                                  direction. */

#define BCM_PKTIO_FILTER_F_STRIP_TAG    0x00000001 /* Strip VLAN tag from
                                                      packets sent to virtual
                                                      network interfaces. */
#define BCM_PKTIO_FILTER_F_ANY_DATA     0x00000002 /* Filter will match any
                                                      packet. */

#define BCM_PKTIO_DEST_T_NULL       0          /* Null destination (drop
                                                  packet). */
#define BCM_PKTIO_DEST_T_NETIF      1          /* Send packet to virtual network
                                                  interface. */
#define BCM_PKTIO_DEST_T_BCM_RX_API 2          /* Send packet to BCM Rx API. */
#define BCM_PKTIO_DEST_T_CALLBACK   3          /* Send packet to kernel 
                                                  call-back function. */

#define BCM_PKTIO_FILTER_M_RAW          0x00000001 /* Match raw packet data. */
#define BCM_PKTIO_FILTER_M_VLAN         0x00000002 /* Match VLAN ID. */
#define BCM_PKTIO_FILTER_M_INGPORT      0x00000004 /* Match local ingress port. */
#define BCM_PKTIO_FILTER_M_SRC_MODPORT  0x00000008 /* Match source module port. */
#define BCM_PKTIO_FILTER_M_SRC_MODID    0x00000010 /* Match source module ID. */
#define BCM_PKTIO_FILTER_M_REASON       0x00000020 /* Match copy-to-CPU reason
                                                      code */
#define BCM_PKTIO_FILTER_M_FP_RULE      0x00000040 /* Match filter processor
                                                      rule number. */
#define BCM_PKTIO_FILTER_M_ERROR        0x00000080 /* Match error bit. */
#define BCM_PKTIO_FILTER_M_CPU_QUEUE    0x00000100 /* Match CPU queue (rx
                                                      queue). */
#define BCM_PKTIO_FILTER_M_PMD          0x00000200 /* Match pmd data. */

#define BCM_PKTIO_FILTER_DESC_MAX       32         /* Filter description maximum
                                                      size. */
#define BCM_PKTIO_FILTER_SIZE_MAX       256        /* Packet raw data maximum
                                                      size. */
#define BCM_PKTIO_FILTER_USER_DATA_SIZE 64         /* Filter user data size in
                                                      bytes. */

/* Packet filter structure. */
typedef struct bcm_pktio_filter_s {
    int id;                             /* Filter ID. */
    uint32 type;                        /* Filter type. Refer to
                                           BCM_PKTIO_FILTER_T_XXX. */
    uint32 flags;                       /* Filter flags. Refer to
                                           BCM_PKTIO_FILTER_F_XXX. */
    uint32 priority;                    /* Filter priority (0 is highest). */
    uint32 dest_type;                   /* Destination type. Refer to
                                           BCM_PKTIO_DEST_T_XXX. */
    int dest_id;                        /* Filter destination ID. */
    uint16 dest_proto;                  /* If non-zero this value overrides the
                                           default protocol type when matching
                                           packet is passed to network stack. */
    uint32 mirror_type;                 /* Destination type. Refer to
                                           BCM_PKTIO_DEST_T_XXX. */
    int mirror_id;                      /* Mirror destination ID. */
    uint16 mirror_proto;                /* If non-zero this value overrides the
                                           default protocol type when matching
                                           packet is passed to network stack. */
    int dma_chan;                       /* Source RX DMA channel to match. */
    uint32 match_flags;                 /* Refert to BCM_PKTIO_FILTER_M_XXX. */
    char desc[BCM_PKTIO_FILTER_DESC_MAX]; /* Filter description (optional). */
    uint16 m_vlan;                      /* VLAN ID to match. */
    bcm_port_t m_ingport;               /* Local ingress port to match. */
    bcm_port_t m_src_modport;           /* Source module port to match. */
    int m_src_modid;                    /* Source module ID to match. */
    int m_cpu_queue;                    /* Source CPU port queue to match. */
    bcm_pktio_bitmap_t m_reason;        /* Copy-to-CPU reason to match. */
    bcm_pktio_bitmap_t m_flex_reason;   /* Copy-to-CPU flex reason to match. */
    uint32 m_fp_rule;                   /* Filter processor rule to match. */
    bcm_pktio_pmd_t m_pmd_data;         /* Pmd data to match. */
    bcm_pktio_pmd_t m_pmd_mask;         /* Pmd data mask to match. */
    uint32 raw_size;                    /* Size of valid raw data and mask. */
    uint8 m_raw_data[BCM_PKTIO_FILTER_SIZE_MAX]; /* Raw data to match. */
    uint8 m_raw_mask[BCM_PKTIO_FILTER_SIZE_MAX]; /* Raw data mask to match. */
    uint8 user_data[BCM_PKTIO_FILTER_USER_DATA_SIZE]; /* User data. */
} bcm_pktio_filter_t;

extern void bcm_pktio_filter_t_init(
    bcm_pktio_filter_t *filter);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a kernel packet filter. */
extern int bcm_pktio_filter_create(
    int unit, 
    bcm_pktio_filter_t *filter);

/* Get a kernel packet filter configuration. */
extern int bcm_pktio_filter_get(
    int unit, 
    int filter_id, 
    bcm_pktio_filter_t *filter);

/* Destroy a kernel packet filter. */
extern int bcm_pktio_filter_destroy(
    int unit, 
    int filter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Network interface traverse callback function */
typedef int (*bcm_pktio_filter_traverse_cb_f)(
    int unit, 
    bcm_pktio_filter_t *filter, 
    void *cb_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse kernel packet filter objects */
extern int bcm_pktio_filter_traverse(
    int unit, 
    bcm_pktio_filter_traverse_cb_f cb_func, 
    void *cb_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_TRACE_LEARN   0x1        /* Learning enable on trace packet. */
#define BCM_PKTIO_TRACE_NO_IFP  0x2        /* Disable ingress field processor
                                              lookup on trace packet. */
#define BCM_PKTIO_TRACE_DROP_TM 0x4        /* Drop trace packet to TM. */

/* Trace information used to construct trace packet. */
typedef struct bcm_pktio_trace_pkt_s {
    uint32 flags;       /* Trace flags refer to BCM_PKTIO_TRACE_XXX. */
    bcm_port_t port;    /* Local front panel port that acts as a masquerade
                           port. */
    int len;            /* Trace packet length. */
    uint8 *pkt;         /* Trace packet data. */
} bcm_pktio_trace_pkt_t;

#define BCM_PKTIO_TRACE_SIZE_BYTES  112        /* Maximum size of raw trace data
                                                  (in bytes). */

/* Raw trace data. */
typedef struct bcm_pktio_trace_data_s {
    uint32 len;                         /* Amount of available trace data. */
    uint8 buf[BCM_PKTIO_TRACE_SIZE_BYTES]; /* Trace data buffer. */
} bcm_pktio_trace_data_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Send out a trace packet and get trace raw data. */
extern int bcm_pktio_trace_data_collect(
    int unit, 
    bcm_pktio_trace_pkt_t *trace_pkt, 
    bcm_pktio_trace_data_t *trace_data);

/* Get value of an trace info field. */
extern int bcm_pktio_trace_field_get(
    int unit, 
    bcm_pktio_trace_data_t *trace_data, 
    int fid, 
    uint32 *val);

/* 
 * Get drop reason of trace info. Setting all bits 1 in trace_data can
 * get supported drop reason bitmap.
 */
extern int bcm_pktio_trace_drop_reasons_get(
    int unit, 
    bcm_pktio_trace_data_t *trace_data, 
    bcm_pktio_bitmap_t *bitmap);

/* 
 * Get counter of trace info. Setting all bits 1 in trace_data can get
 * supported trace counter bitmap.
 */
extern int bcm_pktio_trace_counter_get(
    int unit, 
    bcm_pktio_trace_data_t *trace_data, 
    bcm_pktio_bitmap_t *bitmap);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_PKTIO_TRACE_BITMAP_GET(_bitmap, _bit)  \
    SHR_BITGET(((_bitmap).pbits), (_bit)) 

#define BCM_PKTIO_TRACE_BITMAP_SET(_bitmap, _bit)  \
    SHR_BITSET(((_bitmap).pbits), (_bit)) 

#define BCM_PKTIO_TRACE_BITMAP_CLEAR_ALL(_bitmap)  \
    SHR_BITCLR_RANGE(((_bitmap).pbits), 0, BCM_PKTIO_BITMAP_WORD_SIZE * 32) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Send out a trace packet. */
extern int bcm_pktio_trace_tx(
    int unit, 
    bcm_pktio_trace_pkt_t *trace_pkt);

/* Collect DOP raw data. */
extern int bcm_pktio_trace_dop_data_collect(
    int unit, 
    bcm_port_t port, 
    uint32 dop_id, 
    bcm_pktio_trace_data_t *trace_data);

/* Initialize packet IO module. */
extern int bcm_pktio_init(
    int unit);

/* Cleanup packet IO module. */
extern int bcm_pktio_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_PKTIO_H__ */
