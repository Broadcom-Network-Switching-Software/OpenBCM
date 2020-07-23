/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_CPRI_H__
#define __BCM_CPRI_H__

#include <bcm/types.h>
#include <shared/cpri.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the CPRI subsystem. */
extern int bcm_cpri_init(
    int unit);

/* De-initialize the CPRI subsystem. */
extern int bcm_cpri_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Port types */
typedef enum bcm_cpri_port_type_e {
    bcmCpriPortTypeCpri = 0,    /*  CPRI port */
    bcmCpriPortTypeRsvd4 = 1    /* RSVD4 port */
} bcm_cpri_port_type_t;

/* bcm_cpri_port_speed_t */
typedef _shr_cpri_port_speed_t bcm_cpri_port_speed_t;

#define BCM_CPRI_PORT_SPEED_1228P8      _shrCpriPortSpeed1228p8 
#define BCM_CPRI_PORT_SPEED_2457P6      _shrCpriPortSpeed2457p6 
#define BCM_CPRI_PORT_SPEED_3072P0      _shrCpriPortSpeed3072p0 
#define BCM_CPRI_PORT_SPEED_4915P2      _shrCpriPortSpeed4915p2 
#define BCM_CPRI_PORT_SPEED_6144P0      _shrCpriPortSpeed6144p0 
#define BCM_CPRI_PORT_SPEED_9830P4      _shrCpriPortSpeed9830p4 
#define BCM_CPRI_PORT_SPEED_10137P6     _shrCpriPortSpeed10137p6 
#define BCM_CPRI_PORT_SPEED_12165P12    _shrCpriPortSpeed12165p12 
#define BCM_CPRI_PORT_SPEED_24330P24    _shrCpriPortSpeed24330p24 

#ifndef BCM_HIDE_DISPATCHABLE

/* Configures the Rx speed of the specified CPRI port. */
extern int bcm_cpri_port_rx_speed_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_port_type_t port_type, 
    bcm_cpri_port_speed_t speed);

/* Retrieves the Rx speed of the specified CPRI port. */
extern int bcm_cpri_port_rx_speed_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_port_type_t *port_type, 
    bcm_cpri_port_speed_t *speed);

/* Configures the Tx speed of the specified CPRI port. */
extern int bcm_cpri_port_tx_speed_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_port_type_t port_type, 
    bcm_cpri_port_speed_t speed);

/* Retrieves the Tx speed of the specified CPRI port. */
extern int bcm_cpri_port_tx_speed_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_port_type_t *port_type, 
    bcm_cpri_port_speed_t *speed);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_cpri_frame_sync_mode_t */
typedef _shr_cpri_frame_sync_mode_t bcm_cpri_frame_sync_mode_t;

/* CPRI Frame Sync Modes */
#define BCM_CPRI_FRAME_SYNC_MODE_HYPER  _shrCpriFrameSyncModeHyper 
#define BCM_CPRI_FRAME_SYNC_MODE_RADIO  _shrCpriFrameSyncModeRadio 
#define BCM_CPRI_FRAME_SYNC_MODE_BASIC  _shrCpriFrameSyncModeBasic 

/* CPRI Frame truncation modes */
typedef enum bcm_cpri_frame_trunc_mode_e {
    bcmCpriFrameTruncMode15_to_16 = 0,  /*  Truncation 15 to 16 bits */
    bcmCpriFrameTruncModeAdd_0 = 1,     /*  Truncation Add 0. */
    bcmCpriFrameTruncMode16_to_15 = 2,  /*  Truncation of 16 to 15 bits */
    bcmCpriFrameTruncModeAdd_1 = 3      /*  Truncation of adding 1. */
} bcm_cpri_frame_trunc_mode_t;

/* CPRI Frame compression modes */
typedef enum bcm_cpri_frame_compress_mode_e {
    bcmCpriFrameCompressModeCompress = 0, /*  Compress Mode */
    bcmCpriFrameCompressModeDecompress = 1, /* Decompress Mode */
    bcmCpriFrameCompressModeNone = 2    /*  No Compression */
} bcm_cpri_frame_compress_mode_t;

/* CPRI Framer stages */
typedef enum bcm_cpri_stage_e {
    bcmCpriStageRxParser = 0,   /*  Rx Parser */
    bcmCpriStageTxAssembler = 1 /*  Tx Assembler */
} bcm_cpri_stage_t;

/* CPRI Axc Framer Configuration */
typedef struct bcm_cpri_axc_frame_info_s {
    int axc_id;                         /* AxC ID - output for get_all only */
    int start_bit;                      /* Start bit position of basic frame */
    int num_bits;                       /* Number of bits in AxC */
    int map_method;                     /* Mapping method  0 - Method 1, 1 -
                                           Method 2 (not Supported) 2 - Method 3 */
    int stuffing_count;                 /* Nst for method#1 bits */
    int K;                              /* Container block count */
    int sample_size_M;                  /* Sample size */
    int Nv;                             /* Nv value for method 3 */
    int Na;                             /* Na value for method 3 */
    int Nc;                             /* Numnber of containers in a basic
                                           frame - method 3 only */
    bcm_cpri_frame_sync_mode_t frame_sync_mode; /* CPRI Frame synchronization modes */
    int basic_frame_offset;             /* Frame number offset of basic frame */
    int radio_frame_offset;             /* Frame number offset of radio frame */
    int hyper_frame_offset;             /* Frame number offset of hyper frame */
    int num_samples_per_roe_pkt;        /* Number of samples in RoE payload */
    int truncation_enable;              /* True/False */
    bcm_cpri_frame_trunc_mode_t truncation_mode; /*  */
    bcm_cpri_frame_compress_mode_t compress_mode; /*  */
    uint8 priority;                     /* Priority 0-3 */
    uint16 rx_queue_size;               /* Queue size in bytes */
    uint16 cycle_size;                  /* Only for Tx, Jitter buffer size in
                                           number of packets. */
} bcm_cpri_axc_frame_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set/Get up the AxC(method 1) or AxC group(method 3) on a CPRI port in
 * the Rx datapath.
 */
extern int bcm_cpri_port_rx_axc_frame_add(
    int unit, 
    bcm_gport_t port, 
    int axc_id, 
    bcm_cpri_axc_frame_info_t *axc_info);

/* 
 * Retrieves the AxC (method 1) or AxC group (method 3) in the Rx datapth
 * on a CPRI port.
 */
extern int bcm_cpri_port_rx_axc_frame_get(
    int unit, 
    bcm_gport_t port, 
    int axc_id, 
    bcm_cpri_axc_frame_info_t *axc_info);

/* 
 * Retrieves the configured list of AxC (method 1) or AxC group (method
 * 3) on a CPRI port.
 */
extern int bcm_cpri_port_rx_axc_frame_get_all(
    int unit, 
    bcm_gport_t port, 
    int max_count, 
    bcm_cpri_axc_frame_info_t *axc_info_list, 
    int *axc_count);

/* 
 * Remove the specified AxC (method 1) or AxC group (method 3) on a CPRI
 * port.
 */
extern int bcm_cpri_port_rx_axc_frame_delete(
    int unit, 
    bcm_gport_t port, 
    int axc_id);

/* 
 * Sets up the AxC (method 1) or AxC group (method 3) in the Tx datapath
 * on a CPRI port.
 */
extern int bcm_cpri_port_tx_axc_frame_add(
    int unit, 
    bcm_gport_t port, 
    int axc_id, 
    bcm_cpri_axc_frame_info_t *axc_info);

/* 
 * Retrieves the AxC (method 1) or AxC group (method 3) in the Tx
 * datapath
 * on a CPRI port.
 */
extern int bcm_cpri_port_tx_axc_frame_get(
    int unit, 
    bcm_gport_t port, 
    int axc_id, 
    bcm_cpri_axc_frame_info_t *axc_info);

/* 
 * Retrieves the configured list of AxC (method 1) or AxC group (method
 * 3) in the Tx datapath on a CPRI port.
 */
extern int bcm_cpri_port_tx_axc_frame_get_all(
    int unit, 
    bcm_gport_t port, 
    int max_count, 
    bcm_cpri_axc_frame_info_t *axc_info_list, 
    int *axc_count);

/* 
 * Remove the specified AxC (method 1) or AxC group (method 3) on a CPRI
 * port.
 */
extern int bcm_cpri_port_tx_axc_frame_delete(
    int unit, 
    bcm_gport_t port, 
    int axc_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Encap flags */
#define BCM_CPRI_ENCAP_USE_TAG_ID_FOR_VLAN  0x1        
#define BCM_CPRI_ENCAP_USE_TAG_ID_FOR_FLOWID 0x2        
#define BCM_CPRI_ENCAP_USE_OPCODE           0x4        

/* CPRI VLAN Types */
typedef enum bcm_cpri_vlan_type_e {
    bcmCpriVlanTypeNone = 0,        /* No VLAN */
    bcmCpriVlanTypeSingle = 1,      /* Single Tagged VLAN */
    bcmCpriVlanTypeQinQ = 2,        /*  VLAN Q in Q */
    bcmCpriVlanTypeSingleTable1 = 3 /* Single Tagged VLAN using table 1 */
} bcm_cpri_vlan_type_t;

/* CPRI RoE Subtypes */
typedef enum bcm_cpri_roe_subtype_e {
    bcmCpriRoESubtypeControl = 0,   /* RoE subtype Control */
    bcmCpriRoESubtypeResv = 1,      /* RoE subtype Reserve */
    bcmCpriRoESubtypeAgnostic = 2,  /* RoE subtype Agnostic */
    bcmCpriRoESubtypeCpriData = 3,  /* Roe subtype Data */
    bcmCpriRoESubtypeNative = 4,    /* RoE Subtype Native */
    bcmCpriRoESubtypeSlowCM = 5     /* RoE subtype Slow C&M */
} bcm_cpri_roe_subtype_t;

/* CPRI RoE Opcodes */
typedef enum bcm_cpri_roe_opcode_e {
    bcmCpriRoEOpcodeOamTlv = 0,     /*  OAM TLV */
    bcmCpriRoEOpcodeCtrlAxC = 1,    /*  Control AxC */
    bcmCpriRoEOpcodeVSD = 2,        /*  VSD */
    bcmCpriRoEOpcodeTiming = 3      /*  Timing */
} bcm_cpri_roe_opcode_t;

/* CPRI Header Encapsulation types */
typedef enum bcm_cpri_hdr_encap_type_e {
    bcmCpriHdrEncapTypeRoe = 0,     /* ROE encapsulation type */
    bcmCpriHdrEncapTypeNone = 1,    /* no ROE encapsulation */
    bcmCpriHdrEncapTypeCount = 2    /* should be last */
} bcm_cpri_hdr_encap_type_t;

/* bcm_cpri_roe_frame_format_t */
typedef _shr_cpri_roe_frame_format_t bcm_cpri_roe_frame_format_t;

/* CPRI RoE Frame Format. */
#define BCM_CPRI_ROE_FRAME_FORMAT_IEEE1914  _shrCpriRoeFrameFormatIEEE1914 /*  Selection of IEEE1914
                                                          frame format */
#define BCM_CPRI_ROE_FRAME_FORMAT_RSVD3     _shrCpriRoeFrameFormatRsvd3 /*  Selection of RSVD3
                                                          frame format */
#define BCM_CPRI_ROE_FRAME_FORMAT_ECPRI8    _shrCpriRoeFrameFormatEcpri8 /*  Selection of 8 bytes
                                                          eCPRI frame format */
#define BCM_CPRI_ROE_FRAME_FORMAT_ECPRI12   _shrCpriRoeFrameFormatEcpri12 /*  Selection of 12 bytes
                                                          eCPRI frame format */

/* CPRI Encap Information */
typedef struct bcm_cpri_encap_info_s {
    uint32 flags;                       /* CPRI Encap Info flags */
    uint16 flow_id;                     /*  Flow ID */
    bcm_cpri_roe_subtype_t subtype;     /* RoE subtype */
    bcm_cpri_hdr_encap_type_t hdr_type; /* CPRI header encapsulation type */
    int roe_ordering_info_index;        /* RoE ordering Info index */
    int macda_index;                    /* Index to MAC DA address */
    int macsa_index;                    /* Index to MAC SA address */
    bcm_cpri_vlan_type_t vlan_type;     /* CPRI Vlan type */
    int vlan0_idx;                      /* VLAN 0 Table index */
    int vlan1_idx;                      /*  VLAN 1 table index */
    int vlan0_priority;                 /*  VLAN 0 priority */
    int vlan1_priority;                 /*  VLAN 1 priority */
    uint8 vlan_ethertype_idx;           /*  VLAN EtherType index */
    bcm_cpri_roe_opcode_t opcode;       /* RoE opcode */
    bcm_cpri_roe_frame_format_t type;   /*  RoE Frame Header Formatt */
    uint8 version;                      /*  Header Version used in RSVD3 */
    uint32 ecpri_pc_id;                 /*  PC_ID of eCPRI header, 16 bits for
                                           eCPRI8 and 8 bits for eCPRI12 */
    uint32 ecpri_msg_type;              /*  Message Type eCPRI header, 8 bits
                                           only */
    uint8 ecpri12_header_byte0;         /* Only for eCPRI12, Protocol Rev, Rsvd
                                           and C bits. */
} bcm_cpri_encap_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure/Retrieve the encapsulation header info for a given AxC ID. */
extern int bcm_cpri_port_encap_set(
    int unit, 
    bcm_gport_t port, 
    int queue_num, 
    bcm_cpri_encap_info_t *encap_config);

/* Configure/Retrieve the encapsulation header info for a given AxC ID. */
extern int bcm_cpri_port_encap_get(
    int unit, 
    bcm_gport_t port, 
    int queue_num, 
    bcm_cpri_encap_info_t *encap_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Encapsulation VLAN configuration */
typedef struct bcm_cpri_encap_vlan_config_s {
    uint32 ethertype_vlan_tagged;   /* Tagged VLAN */
    uint32 ethertype_vlan_qinq;     /*  QinQ VLAN */
    uint32 roe_ethertype_0;         /*  RoE ether type 0 */
    uint32 roe_ethertype_1;         /*  RoE ether type 1 */
} bcm_cpri_encap_vlan_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the VLAN EtherType configuration for encapsulation header. */
extern int bcm_cpri_port_encap_vlan_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_encap_vlan_config_t *vlan_config);

/* Set/Get the VLAN EtherType configuration for encapsulation header. */
extern int bcm_cpri_port_encap_vlan_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_encap_vlan_config_t *vlan_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Encap RoE Ordering Info Info types */
typedef enum bcm_cpri_roe_info_type_e {
    bcmCpriRoEInfoTypeUseSeq = 0,       /* Use Sequene number */
    bcmCpriRoEInfoTypeBfnForQcnt = 1,   /*  Use Bfn for Qcnt */
    bcmCpriRoEInfoTypePInfo = 2         /*  P info */
} bcm_cpri_roe_info_type_t;

/* CPRI Encap RoE Ordering Info Increment types */
typedef enum bcm_cpri_roe_incr_type_e {
    bcmCpriRoEIncrTypeNone = 0,         /*  No increment. */
    bcmCpriRoEIncrTypeConstant = 1,     /*  Increment by a Contant */
    bcmCpriRoEIncrTypePayloadSize = 2   /*  Increment by payload size */
} bcm_cpri_roe_incr_type_t;

/* CPRI Encap RoE Ordering Information */
typedef struct bcm_cpri_encap_roe_oi_s {
    bcm_cpri_roe_info_type_t info_type; /*  ordering info type */
    bcm_cpri_roe_incr_type_t p_seq_incr; /* seqNumPIncProp */
    bcm_cpri_roe_incr_type_t q_seq_incr; /* seqNumQIncProp */
    uint32 pcnt_size;                   /* Number in the range 1 to 32 */
    uint32 pcnt_increment;              /* seqNumPInc */
    uint32 pcnt_max;                    /* seqNumPMax */
    uint32 qcnt_size;                   /* Number in the range 0 to (32-pcnt) */
    uint32 qcnt_increment;              /* seqNumQInc */
    uint32 qcnt_max;                    /* seqNumQMax */
    uint32 seq_rsvd;                    /* seqNumRsvd Value */
} bcm_cpri_encap_roe_oi_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the ordering info for encapsulation header. */
extern int bcm_cpri_port_encap_roe_ordering_info_set(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_cpri_encap_roe_oi_t *ordering_info);

/* Set/Get the ordering info for encapsulation header. */
extern int bcm_cpri_port_encap_roe_ordering_info_get(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_cpri_encap_roe_oi_t *ordering_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Encap RoE Ordering Info Sequence Offset configuration */
typedef struct bcm_cpri_roe_oi_seq_offset_s {
    uint32 pcnt_size;   /*  Pcnt Size */
    uint32 pcnt_offset; /*  Pcnt Offset */
    uint32 qcnt_size;   /*  Qcnt Size */
    uint32 qcnt_offset; /*  Qcnt Offset */
} bcm_cpri_roe_oi_seq_offset_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get sequence offset information associated with the ordering info. */
extern int bcm_cpri_port_encap_roe_ordering_sequence_offset_set(
    int unit, 
    bcm_gport_t port, 
    int queue, 
    int control, 
    bcm_cpri_roe_oi_seq_offset_t *seq_offset);

/* Set/Get sequence offset information associated with the ordering info. */
extern int bcm_cpri_port_encap_roe_ordering_sequence_offset_get(
    int unit, 
    bcm_gport_t port, 
    int queue, 
    int control, 
    bcm_cpri_roe_oi_seq_offset_t *seq_offset);

/* Set/Get the encapsulation entry MAC DA for lookup. */
extern int bcm_cpri_port_encap_roe_entry_macda_set(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_mac_t macaddr);

/* Set/Get the encapsulation entry MAC SA for lookup. */
extern int bcm_cpri_port_encap_roe_entry_macsa_set(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_mac_t macaddr);

/* Set/Get the encapsulation entry MAC DA for lookup. */
extern int bcm_cpri_port_encap_roe_entry_macda_get(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_mac_t *macaddr);

/* Set/Get the encapsulation entry MAC SA for lookup. */
extern int bcm_cpri_port_encap_roe_entry_macsa_get(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_mac_t *macaddr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Vlan flags */
#define BCM_CPRI_VLAN_FLAG_VLAN_0   0x0        
#define BCM_CPRI_VLAN_FLAG_VLAN_1   0x1        

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get VLAN ID for the given encap entry. */
extern int bcm_cpri_port_encap_roe_entry_vlan_set(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    uint32 index, 
    bcm_vlan_t vlan_id);

/* Set/Get VLAN ID for the given encap entry. */
extern int bcm_cpri_port_encap_roe_entry_vlan_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    uint32 index, 
    bcm_vlan_t *vlan_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Packet EtherTypes */
typedef enum bcm_cpri_ethertype_e {
    bcmCpriEthertypeRoe = 0,    /*  EtherType RoE */
    bcmCpriEthertypeFast = 1,   /*  EtherType Fast Ethernet */
    bcmCpriEthertypeVlan = 2,   /*  EtherType VLAN */
    bcmCpriEthertypeQinQ = 3    /*  EtherType QinQ */
} bcm_cpri_ethertype_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the EtherType for CPRI decap. */
extern int bcm_cpri_port_decap_ethertype_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_ethertype_t type, 
    uint16 tpid);

/* Set/Get the EtherType for CPRI decap. */
extern int bcm_cpri_port_decap_ethertype_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_ethertype_t type, 
    uint16 *tpid);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Decap Flow types */
typedef enum bcm_cpri_decap_flow_type_e {
    bcmCpriDecapFlowTypeIQData = 0,     /*  Flow Type IQ Data */
    bcmCpriDecapFlowTypeIQDataWithTimestamp = 1, /*  Flow Type IQ Data with Timestamp */
    bcmCpriDecapFlowTypeControlWithOpcode = 2, /*  Flow Type Control with Opcode */
    bcmCpriDecapFlowTypeControl = 3     /*  Flow Type Control, without opcode */
} bcm_cpri_decap_flow_type_t;

/* CPRI Decap Flow lookup options */
typedef enum bcm_cpri_decap_flow_lookup_e {
    bcmCpriDecapFlowLookupQueueNum = 0, /*  Use Queue Number */
    bcmCpriDecapFlowLookupFlowID = 1,   /*  Use Flow ID to look up Queue number */
    bcmCpriDecapFlowLookupOpcode = 2    /*  User Opcode to look up Queue number */
} bcm_cpri_decap_flow_lookup_t;

/* CPRI Decap Flow classification information */
typedef struct bcm_cpri_decap_flow_config_s {
    bcm_cpri_decap_flow_type_t flow_type; /*  Flow Type */
    bcm_cpri_decap_flow_lookup_t lookup_option; /*  Look Up option */
    int queue_num;                      /*  Queue Number */
} bcm_cpri_decap_flow_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure flow classification table in CPRI decap. */
extern int bcm_cpri_port_decap_flow_config_set(
    int unit, 
    bcm_gport_t port, 
    uint8 roe_subtype, 
    bcm_cpri_decap_flow_config_t *config);

/* Configure flow classification table in CPRI decap. */
extern int bcm_cpri_port_decap_flow_config_get(
    int unit, 
    bcm_gport_t port, 
    uint8 roe_subtype, 
    bcm_cpri_decap_flow_config_t *config);

/* Configure Subtype to mapped to invalid queue to drop. */
extern int bcm_cpri_port_decap_flow_config_clear(
    int unit, 
    bcm_gport_t port, 
    uint8 roe_subtype);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Decapsulation RoE Ordering Information */
typedef struct bcm_cpri_decap_roe_oi_s {
    bcm_cpri_roe_info_type_t info_type; /*  */
    uint32 pcnt_size;                   /* Number in the range 1 to 32 */
    uint32 pcnt_increment;              /* seqNumPInc */
    uint32 pcnt_max;                    /* seqNumPMax */
    uint32 qcnt_size;                   /* Number in the range 0 to (32-pcnt) */
    uint32 qcnt_increment;              /* seqNumQInc */
    uint32 qcnt_max;                    /* seqNumQMax */
    uint8 p_ext;                        /* Extended P Counter */
    uint8 gsm_tsn_bitmap;               /* TSN bitmap */
    uint32 pcnt_pkt_count;              /* Maximum pkt count described by P
                                           counter */
    uint32 modulo_2;                    /* Queue Cycle Size is modulo 2 */
} bcm_cpri_decap_roe_oi_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the ordering info for Decapsulation. */
extern int bcm_cpri_port_decap_roe_ordering_info_set(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_cpri_decap_roe_oi_t *ordering_info);

/* Set/Get the ordering info for Decapsulation. */
extern int bcm_cpri_port_decap_roe_ordering_info_get(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_cpri_decap_roe_oi_t *ordering_info);

/* Set/Get sequence offset information associated with the ordering info. */
extern int bcm_cpri_port_decap_roe_ordering_sequence_offset_set(
    int unit, 
    bcm_gport_t port, 
    int queue, 
    bcm_cpri_roe_oi_seq_offset_t *seq_offset);

/* Set/Get sequence offset information associated with the ordering info. */
extern int bcm_cpri_port_decap_roe_ordering_sequence_offset_get(
    int unit, 
    bcm_gport_t port, 
    int queue, 
    bcm_cpri_roe_oi_seq_offset_t *seq_offset);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Tx TGEN configuration */
typedef struct bcm_cpri_tx_frame_tgen_config_s {
    uint32 tx_tgen_hfn;         /* new hyperframe hfn number on next hyperframe */
    uint32 tx_tgen_bfn;         /* new radio frame bfn number on next radio
                                   frame */
    uint32 tx_tgen_bfn_sel;     /* use cfg_tgen_bfn on next hyperframe or radio
                                   frame */
    uint64 tx_tgen_ts_offset;   /* start frame event after matching this offset */
} bcm_cpri_tx_frame_tgen_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure tgen for port */
extern int bcm_cpri_port_tx_frame_tgen_config_set(
    int unit, 
    bcm_gport_t port, 
    int enable, 
    bcm_cpri_tx_frame_tgen_config_t *tgen_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Rx frame configuration */
typedef struct bcm_rsvd4_rx_frame_config_s {
    int speed_multiple; /* Only values 4 and 8 are supported */
} bcm_rsvd4_rx_frame_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set RSVD4 frame configuration */
extern int bcm_cpri_port_rsvd4_rx_frame_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_frame_config_t *config);

/* Get RSVD4 frame configuration */
extern int bcm_cpri_port_rsvd4_rx_frame_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_frame_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Rx Config fields */
typedef enum bcm_rsvd4_rx_config_e {
    bcmRsvd4RxConfigDescrambleEnable = 0, /*  Descrambling Enable */
    bcmRsvd4RxConfigSyncThreshold = 1,  /*  Sync Threshold */
    bcmRsvd4RxConfigUnsyncThreshold = 2, /*  Unsync Threshold */
    bcmRsvd4RxConfigFrameSyncThreshold = 3, /*  Frame Sync Threshold */
    bcmRsvd4RxConfigFrameUnsyncThreshold = 4, /*  Frame Unsync Threshold */
    bcmRsvd4RxConfigBlockSize = 5       /*  Block Size */
} bcm_rsvd4_rx_config_t;

/* bcm_rsvd4_rx_fsm_control_t */
typedef _shr_rsvd4_rx_config_t bcm_rsvd4_rx_fsm_control_t;

/* RSVD4 RX FSM Control. */
#define BCM_RSVD4_RX_FSM_CONTROL_DESCRAMBLE_ENABLE _shrRsvd4RxConfigDescrambleEnable /*  Descramble Enable */
#define BCM_RSVD4_RX_FSM_CONTROL_SYNC_T     _shrRsvd4RxConfigSyncThreshold /* Sync Threshold. */
#define BCM_RSVD4_RX_FSM_CONTROL_UNSYNC_T   _shrRsvd4RxConfigUnsyncThreshold /* UnSync Threashold. */
#define BCM_RSVD4_RX_FSM_CONTROL_FRAME_SYNC_T _shrRsvd4RxConfigFrameSyncThreshold /* Frame Sync Threadhold. */
#define BCM_RSVD4_RX_FSM_CONTROL_FRAME_UNSYNC_T _shrRsvd4RxConfigFrameUnsyncThreshold /* Frame UnSync Threshold */
#define BCM_RSVD4_RX_FSM_CONTROL_COUNT      _shrRsvd4RxConfigCount /*  Last Entry */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set RSVD4 Rx FSM Control and/or Configuration. */
extern int bcm_cpri_port_rsvd4_rx_fsm_control_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_fsm_control_t control_type, 
    uint32 value);

/* Get RSVD4 Rx FSM Control and/or Configuration. */
extern int bcm_cpri_port_rsvd4_rx_fsm_control_get(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_fsm_control_t control_type, 
    uint32 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_cpri_debug_attr_t */
typedef _shr_cpri_deubg_attr_t bcm_cpri_debug_attr_t;

/* CPRI DEBUG ATTRIBUTE. */
#define BCM_CPRI_DEBUG_DECAP_QUEUE_STATUS   _shrCpriDeubgAttrDecapQueueStatus /* Decap Queue Status. */
#define BCM_CPRI_DEBUG_ENCAP_QUEUE_STATUS   _shrCpriDeubgAttrEncapQueueStatus /* Encap Queue Status. */
#define BCM_CPRI_DEBUG_DECAP_QUEUE_COUNTER  _shrCpriDeubgAttrEncapQueueCounter /* Decap Queue Counter. */
#define BCM_CPRI_DEBUG_ENCAP_QUEUE_COUNTER  _shrCpriDeubgAttrDecapQueueCounter /* Encap Queue Counter. */
#define BCM_CPRI_DEBUG_DECAP_DROPPED_COUNTER _shrCpriDeubgAttrDecapDroppedCounter /* Decap Dropped Counter. */
#define BCM_CPRI_DEBUG_COUNT                _shrCpriDeubgAttrCount /* Last Entry */

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the value for the debug attribute. */
extern int bcm_cpri_port_debug_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_debug_attr_t type, 
    int index, 
    int buffer_size, 
    uint32 *data);

/* Set the value for the debug attribute. */
extern int bcm_cpri_port_debug_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_debug_attr_t type, 
    int index, 
    int buffer_size, 
    uint32 *data);

/* Clear the value for the debug attribute. */
extern int bcm_cpri_port_debug_clear(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_debug_attr_t type, 
    int index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_rsvd4_tx_fsm_control_t */
typedef _shr_rsvd4_tx_config_t bcm_rsvd4_tx_fsm_control_t;

/* RSVD4 TX FSM Control. */
#define BCM_RSVD4_TX_FSM_CONTROL_FORCE_OFF_STATE _shrRsvd4TxConfigForceOffState /* Force Off State. */
#define BCM_RSVD4_TX_FSM_CONTROL_ACK_T      _shrRsvd4TxConfigAckT /* Ack Threhold */
#define BCM_RSVD4_TX_FSM_CONTROL_FORCE_IDLE_ACK _shrRsvd4TxConfigForcedIdleAck /* Force Idle Ack. */
#define BCM_RSVD4_TX_FSM_CONTROL_LOS_ENABLE _shrRsvd4TxConfigLosEnable /*  Set LOS_ENABLE config */
#define BCM_RSVD4_TX_FSM_CONTROL_SCRAMBLE_ENABLE _shrRsvd4TxConfigScrambleEnable /* Scramble enable */
#define BCM_RSVD4_TX_FSM_CONTROL_SCRAMBLE_SEED _shrRsvd4TxConfigScrambleSeed /* Scramble Seed */
#define BCM_RSVD4_TX_FSM_CONTROL_TRANSMITTER_EN _shrRsvd4TxConfigTransmitterEnable /* TRANSMITTER_EN set */
#define BCM_RSVD4_TX_FSM_CONTROL_COUNT      _shrRsvd4TxConfigCount /* Last Entry */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set RSVD4 Tx FSM Control and/or Configuration. */
extern int bcm_cpri_port_rsvd4_tx_fsm_control_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_fsm_control_t control_type, 
    uint32 value);

/* Get RSVD4 Tx FSM Control and/or Configuration. */
extern int bcm_cpri_port_rsvd4_tx_fsm_control_get(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_fsm_control_t control_type, 
    uint32 *value);

/* Set RSVD4 frame configuration for debug purpose */
extern int bcm_cpri_port_rsvd4_rx_frame_config_debug_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_config_t config_type, 
    uint32 value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Rx FSM States */
typedef enum bcm_rsvd4_rx_fsm_state_e {
    bcmRsvd4RxFsmStateUnsync = 0,       /*  State Unsync */
    bcmRsvd4RxFsmStateWaitForSeed = 1,  /*  State Wait For Seed */
    bcmRsvd4RxFsmStateWaitForAck = 2,   /*  State Wait For Seed */
    bcmRsvd4RxFsmStateWaitForK28p7Idles = 3, /*  State Wait for K28.7 Idles */
    bcmRsvd4RxFsmStateWaitForFrameSync = 4, /*  State Wait for Frame Sync */
    bcmRsvd4RxFsmStateFrameSync = 5     /*  State Frame Sync */
} bcm_rsvd4_rx_fsm_state_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* To set the RSVD4 state machine to a particular state for debug purpose */
extern int bcm_cpri_port_rsvd4_rx_fsm_state_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_fsm_state_t state);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Rx FSM Override parameters */
typedef enum bcm_rsvd4_rx_params_e {
    bcmRsvd4RxParams_Frame_Unsync_T_Invalid_MG_Rcvd = 0, /*  Frame Unsync Threshold */
    bcmRsvd4RxParams_Frame_Sync_T_Valid_MG_Rcvd = 1, /*  Frame Sync Threshold */
    bcmRsvd4RxParams_K_MG_Idles_Rcvd = 2, /*  K Frame Received */
    bcmRsvd4RxParams_Idle_Req_Rcvd = 3, /*  Idle Request Received */
    bcmRsvd4RxParams_Idle_Ack_Rcvd = 4, /*  Idle Ack Received */
    bcmRsvd4RxParams_SeedCapAndVerifyDone = 5 /*  Seed Capture and Verify Done */
} bcm_rsvd4_rx_params_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* To set the RSVD4 state machine to a particular state for debug purpose */
extern int bcm_cpri_port_rsvd4_rx_override_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_rx_params_t parameter, 
    int enable, 
    int value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_rsvd4_msg_ts_mode_t */
typedef _shr_rsvd4_msg_ts_mode_t bcm_rsvd4_msg_ts_mode_t;

/* RSVD4 TS MSG Modes. */
#define BCM_RSVD4_MSG_TSMODE_WCDMA  _shrRsvd4MsgTsModeWCDMA /* Msg Timestamp WCDMA mode */
#define BCM_RSVD4_MSG_TSMODE_LTE    _shrRsvd4MsgTsModeLTE /* Msg Timestamp LTE mode */
#define BCM_RSVD4_MSG_TSMODE_GSM_DL _shrRsvd4MsgTsModeGSMDL /* Msg Timestamp GSM DL mode */
#define BCM_RSVD4_MSG_TSMODE_GSM_UL _shrRsvd4MsgTsModeGSMUL /* Msg Timestamp GSM UL  mode */

/* bcm_rsvd4_sync_mode_t */
typedef _shr_rsvd4_sync_mode_t bcm_rsvd4_sync_mode_t;

/* RSVD4 Frame Sync Modes. */
#define BCM_RSVD4_SYNC_MODE_NO_SYNC         _shrRsvd4SyncModeNoSync /* Sync On any message */
#define BCM_RSVD4_SYNC_MODE_MSG_AND_MASTER_FRAME_OFFSET _shrRsvd4SyncModeMsgAndMasterFrameOffset /* Sync On Master Frame
                                                          and Msg Offset */
#define BCM_RSVD4_SYNC_MODE_TS_NO_SYNC      _shrRsvd4SyncModeTsNoSync /* Sync On any message
                                                          timestamp, GSM Only */
#define BCM_RSVD4_SYNC_MODE_TS_MSG_AND_MASTER_FRAME_OFFSET _shrRsvd4SyncModeTsMsgAndMasterFrameOffset /* Sync On Master Frame
                                                          And Msg Offset
                                                          timestamp, GSM Only */

/* RSVD4 Rx Flow configuration */
typedef struct bcm_rsvd4_rx_flow_info_s {
    uint32 mask;                        /*  header mask */
    uint32 match_data;                  /* header data to match */
    uint16 payload_size;                /*   payload size in bits */
    uint16 last_payload_index;          /*  last payload index */
    bcm_rsvd4_msg_ts_mode_t msg_ts_mode; /*  Msg timestamp mode */
    bcm_rsvd4_sync_mode_t sync_mode;    /*  Sync Mode */
    uint16 stuffing_count;              /*  Stuffing Count */
    uint32 message_number_offset;       /*  Message number Offset for Sync */
    uint16 master_frame_offset;         /*  Master Frame Offset for Sync */
    uint16 msg_ts_count;                /*  Msg timestamp Count */
    uint16 container_blk_count;         /*  Container Block Count */
    uint8 priority;                     /*  Priority */
    uint32 queue_size;                  /*  Queue Size in bytes. */
    uint8 gsm_pad_size;                 /*  GSM Pad Size */
    uint8 gsm_extra_pad_size;           /*  GSM Extra Pad size */
    uint8 gsm_pad_en;                   /*  GSM Pad Enable */
    uint8 gsm_ctrl_loc;                 /*  GSM control location */
    uint32 axc_id;                      /*  AxC ID */
    uint32 last_payload_size;           /*  last Payload size */
    uint8 gsm_tsn_bitmap;               /*  GSM TSN bitmap */
} bcm_rsvd4_rx_flow_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add RSVD4 Rx Flow data AxC */
extern int bcm_cpri_port_rsvd4_rx_flow_add(
    int unit, 
    bcm_gport_t port, 
    int flow_id, 
    bcm_rsvd4_rx_flow_info_t *flow_info);

/* Delete RSVD4 Rx Flow data AxC */
extern int bcm_cpri_port_rsvd4_rx_flow_delete(
    int unit, 
    bcm_gport_t port, 
    int flow_id);

/* Set RSVD4 Rx master frame sync information */
extern int bcm_cpri_port_rsvd4_rx_bfn_sync_config_set(
    int unit, 
    bcm_gport_t port, 
    uint16 master_frame_count, 
    uint64 master_frame_start);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Frame Synchronization Info. */
typedef struct bcm_rsvd4_frame_sync_info_s {
    uint32 master_frame_number; /*  RSVD4 Master Frame Number */
} bcm_rsvd4_frame_sync_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get RSVD4 Rx current frame sync info */
extern int bcm_cpri_port_rsvd4_rx_current_frame_sync_info_get(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_frame_sync_info_t *frame_sync_info);

/* Get RSVD4 Tx current frame sync info */
extern int bcm_cpri_port_rsvd4_tx_current_frame_sync_info_get(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_frame_sync_info_t *frame_sync_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Tx frame configuration */
typedef struct bcm_rsvd4_tx_frame_config_s {
    uint8 los_enable;           /*  LOS enable */
    uint8 rsvd4_scrambler_seed; /*  RSVD4 scrambler seed */
    int speed_multiple;         /* Only values 4 and 8 are supported */
} bcm_rsvd4_tx_frame_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* RSVD4 Tx frame configuration */
extern int bcm_cpri_port_rsvd4_tx_frame_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_frame_config_t *config);

/* RSVD4 Tx frame configuration get */
extern int bcm_cpri_port_rsvd4_tx_frame_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_frame_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Tx Config fields */
typedef enum bcm_rsvd4_tx_config_e {
    bcmRsvd4TxConfigForceOffState = 0,  /*  Force Off State */
    bcmRsvd4TxConfigAckT = 1,           /*  Ack T */
    bcmRsvd4TxConfigForceIdleAck = 2,   /*  Force Idle Ack */
    bcmRsvd4TxConfigLosEnable = 3,      /*  LOS Enable */
    bcmRsvd4TxConfigScrambleEnable = 4, /*  Scrambling Enable */
    bcmRsvd4TxConfigScrambleSeed = 5,   /*  Scrambling Seed */
    bcmRsvd4TxConfigTransmitterEnable = 6 /*  Transmitter Enable */
} bcm_rsvd4_tx_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set RSVD4 Tx frame configuration for debug purpose */
extern int bcm_cpri_port_rsvd4_tx_frame_config_debug_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_config_t config_type, 
    uint32 value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Tx FSM States */
typedef enum bcm_rsvd4_tx_fsm_state_e {
    bcmRsvd4TxFsmStateOff = 0,      /*  State Off */
    bcmRsvd4TxFsmStateIdle = 1,     /*  State Idle */
    bcmRsvd4TxFsmStateIdleReq = 2,  /*  State Idle request */
    bcmRsvd4TxFsmStateIdleAck = 3,  /*  State Idle Acknowledge */
    bcmRsvd4TxFsmStateFrameTx = 4   /*  State frame Tx */
} bcm_rsvd4_tx_fsm_state_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* To set the RSVD4 state machine to a particular state for debug purpose */
extern int bcm_cpri_port_rsvd4_tx_fsm_state_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_fsm_state_t state);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Tx override parameteres for debug purpose */
typedef enum bcm_rsvd4_tx_params_e {
    bcmRsvd4TxParams_StartTx = 0,       /*  Start Tx */
    bcmRsvd4TxParams_PCS_Ack_CAP = 1,   /*  PCS Ack */
    bcmRsvd4TxParams_PCS_Idle_REQ = 2,  /*  PCS Idle Req */
    bcmRsvd4TxParams_PCS_SCR_Lock = 3,  /*  PCS SCR locked */
    bcmRsvd4TxParams_LOS_Status = 4     /*  LOS Status */
} bcm_rsvd4_tx_params_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* To set the RSVD4 state machine to a particular state for debug purpose */
extern int bcm_cpri_port_rsvd4_tx_override_set(
    int unit, 
    bcm_gport_t port, 
    bcm_rsvd4_tx_params_t parameter, 
    int enable, 
    int value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Tx Flow configuration */
typedef struct bcm_rsvd4_tx_flow_info_s {
    int axc_id;                         /*  axc_id */
    uint16 stuffing_count;              /*  Stuffing Count */
    uint32 message_number_offset;       /*  Msg Number Offset for Sync */
    uint16 master_frame_offset;         /*  Master Frame number offset for Sync */
    uint32 container_blk_count;         /*  Container Block Count */
    uint16 message_addr;                /*  Message Address */
    uint32 msg_ts_offset;               /*   Msg Timestamp Offset */
    bcm_rsvd4_msg_ts_mode_t msg_ts_mode; /*   Msg Timestamp Mode */
    uint8 message_type;                 /*  Message Type */
    bcm_rsvd4_sync_mode_t sync_mode;    /*  Sync Mode */
    uint16 msg_ts_count;                /*   Msg Timestamp Count */
    uint8 index_to_secondary_dbm;       /*  index to secondary DBM */
    uint8 secondary_dbm_enable;         /*  Secondary DBM enable */
    uint8 num_active_slot;              /*  number of active slot */
    uint16 payload_size;                /*  payload size in bits */
    uint16 last_payload_size;           /*  last payload size */
    uint8 last_index;                   /*  last payload index */
    uint8 pad_enable; 
    uint8 pad_size;                     /*  pad size */
    uint8 extra_pad_size;               /*  extra pad size */
    uint8 cycle_size;                   /*  Jitter buffer size in number of
                                           packets */
    uint32 buffer_size;                 /*  Packet size in bytes */
    uint8 gsm_ctrl_loc;                 /*  GSM control location */
} bcm_rsvd4_tx_flow_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add RSVD4 Tx Flow data AxC */
extern int bcm_cpri_port_rsvd4_tx_flow_add(
    int unit, 
    bcm_gport_t port, 
    int flow_id, 
    bcm_rsvd4_tx_flow_info_t *flow_info);

/* Delete a RSVD4 Tx Flow data AxC */
extern int bcm_cpri_port_rsvd4_tx_flow_delete(
    int unit, 
    bcm_gport_t port, 
    int flow_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 dual bitmap entry */
typedef struct bcm_rsvd4_dbm_entry_s {
    uint8 bm1_multiple; /*  Bit Map 1 Multiple */
    uint32 bm1[3];      /*  Bit Map 1 */
    uint8 bm1_size;     /*  Bit Map 1 size */
    uint32 bm2[2];      /*  Bit Map 2 */
    uint8 bm2_size;     /*  Bit map 2 size */
    uint8 num_slots;    /*  number of slot for Each bit map */
    uint8 pos_index;    /*  Position Index */
    uint8 pos_entries;  /*  Number of Position Table Entries */
} bcm_rsvd4_dbm_entry_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get up a DBM entry */
extern int bcm_cpri_port_rsvd4_dbm_entry_set(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_rsvd4_dbm_entry_t *entry);

/* Set/Get up a DBM entry */
extern int bcm_cpri_port_rsvd4_dbm_entry_get(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_rsvd4_dbm_entry_t *entry);

/* Clear a DBM entry */
extern int bcm_cpri_port_rsvd4_dbm_entry_clear(
    int unit, 
    bcm_gport_t port, 
    int index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Secondary dual bitmap entry */
typedef struct bcm_rsvd4_secondary_dbm_entry_s {
    uint8 bm1_multiple; /*  Bit Map 1 multiple */
    uint8 bm1;          /*  Bit Map 1 */
    uint8 bm1_size;     /*  Bit Map 1 size */
    uint8 bm2;          /*  Bit Map 2 */
    uint8 bm2_size;     /*  Bit Map 2 Size */
    uint8 num_slots;    /*  Number of Slots for each Bit */
} bcm_rsvd4_secondary_dbm_entry_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get up a secondary DBM entry */
extern int bcm_cpri_port_rsvd4_secondary_dbm_entry_set(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_rsvd4_secondary_dbm_entry_t *entry);

/* Set/Get up a secondary DBM entry */
extern int bcm_cpri_port_rsvd4_secondary_dbm_entry_get(
    int unit, 
    bcm_gport_t port, 
    int index, 
    bcm_rsvd4_secondary_dbm_entry_t *entry);

/* Clear a Secondary DBM entry */
extern int bcm_cpri_port_rsvd4_secondary_dbm_entry_clear(
    int unit, 
    bcm_gport_t port, 
    int index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Flow types */
typedef enum bcm_cpri_flow_type_e {
    bcmCpriFlowTypeData = 0,    /*  Data Flow Type */
    bcmCpriFlowTypeCtrl = 1,    /*  Control Flow Type */
    bcmCpriFlowTypeCount = 2    /* Should be last */
} bcm_cpri_flow_type_t;

/* CPRI dbm position table entry */
typedef struct bcm_cpri_dbm_position_entry_s {
    uint8 valid;                    /* Valid */
    uint8 flow_id;                  /* Flow id */
    bcm_cpri_flow_type_t flow_type; /* Flow type - Data or Ctrl */
    uint8 index;                    /* X slot position , 0 to (Num_slot-1) */
} bcm_cpri_dbm_position_entry_t;

/* CPRI transmission rule types */
typedef enum bcm_cpri_tx_rule_type_e {
    bcmCpriTxRuleTypeData = 0,  /*  Tx Rule type Data */
    bcmCpriTxRuleTypeCtrl = 1,  /*  Tx Rule Type Control */
    bcmCpriTxRuleTypeCount = 2  /* Should be last */
} bcm_cpri_tx_rule_type_t;

/* CPRI modulo rule types */
typedef enum bcm_cpri_modulo_rule_mod_e {
    bcmCpriModuloRuleMod1 = 0,      /*  Modulo 1 */
    bcmCpriModuloRuleMod2 = 1,      /*  Modulo 2 */
    bcmCpriModuloRuleMod4 = 2,      /*  Module 4 */
    bcmCpriModuloRuleMod8 = 3,      /*  Modulo 8 */
    bcmCpriModuloRuleMod16 = 4,     /*  Modulo 16 */
    bcmCpriModuloRuleMod32 = 5,     /*  Modulo 32 */
    bcmCpriModuloRuleMod64 = 6,     /*  Modulo 64 */
    bcmCpriModuloRuleModCount = 7   /* Should be last */
} bcm_cpri_modulo_rule_mod_t;

/* CPRI modulo rule entry */
typedef struct bcm_cpri_modulo_rule_entry_s {
    uint8 active;                       /* Entry is active */
    bcm_cpri_modulo_rule_mod_t modulo_value; /* Modulo value */
    uint8 modulo_index;                 /* Modulo index */
    uint8 dbm_enable;                   /* Dual bitmap rule is enabled */
    uint8 dbm_dbm_id;                   /* Flow Id or DBM profile ID if dbm is
                                           enabled */
    bcm_cpri_flow_type_t flow_type;     /* Flow type - Data or Ctrl */
} bcm_cpri_modulo_rule_entry_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get the tx rsvd4 modulo rule entry. */
extern int bcm_cpri_port_rsvd4_tx_modulo_rule_entry_set(
    int unit, 
    bcm_gport_t port, 
    uint32 modulo_rule_num, 
    bcm_cpri_tx_rule_type_t modulo_rule_type, 
    bcm_cpri_modulo_rule_entry_t *modulo_rule);

/* Set/Get the tx rsvd4 modulo rule entry. */
extern int bcm_cpri_port_rsvd4_tx_modulo_rule_entry_get(
    int unit, 
    bcm_gport_t port, 
    uint32 modulo_rule_num, 
    bcm_cpri_tx_rule_type_t modulo_rule_type, 
    bcm_cpri_modulo_rule_entry_t *modulo_rule);

/* Set/Get the tx rsvd4 position entry. */
extern int bcm_cpri_port_rsvd4_tx_position_entry_set(
    int unit, 
    bcm_gport_t port, 
    uint32 pos_index, 
    bcm_cpri_dbm_position_entry_t *pos_entry);

/* Set/Get the tx rsvd4 position entry. */
extern int bcm_cpri_port_rsvd4_tx_position_entry_get(
    int unit, 
    bcm_gport_t port, 
    uint32 pos_index, 
    bcm_cpri_dbm_position_entry_t *pos_entry);

/* Set/Get decap flow to queue mapping. */
extern int bcm_cpri_port_decap_flow_to_queue_mapping_set(
    int unit, 
    bcm_gport_t port, 
    uint32 flow_id, 
    uint32 queue_num);

/* Set/Get decap flow to queue mapping. */
extern int bcm_cpri_port_decap_flow_to_queue_mapping_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flow_id, 
    uint32 *queue_num);

/* Remapped the flow to  invalid queue. */
extern int bcm_cpri_port_decap_flow_to_queue_mapping_clear(
    int unit, 
    bcm_gport_t port, 
    uint32 flow_id);

/* Set/Get decap queue to ordering info index. */
extern int bcm_cpri_port_decap_queue_to_ordering_info_index_set(
    int unit, 
    bcm_gport_t port, 
    uint32 queue_num, 
    uint32 ordering_info_index);

/* Set/Get decap queue to ordering info index. */
extern int bcm_cpri_port_decap_queue_to_ordering_info_index_get(
    int unit, 
    bcm_gport_t port, 
    uint32 queue_num, 
    uint32 *ordering_info_index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_rsvd4_control_msg_proc_type_t */
typedef _shr_rsvd4_control_msg_proc_type_t bcm_rsvd4_control_msg_proc_type_t;

/* RSVD4 Control Message Processing Type. */
#define BCM_RSVD4_CONTROL_MSG_PROC_ETH      _shrRsvd4CtrlMsgProcFE /* Multi Message Fast
                                                          Ethernet. */
#define BCM_RSVD4_CONTROL_MSG_PROC_FCB      _shrRsvd4CtrlMsgProcFE /* FCB. */
#define BCM_RSVD4_CONTROL_MSG_PROC_PAYLOAD  _shrRsvd4CtrlMsgProcPayload /* Message Payload
                                                          Extraction. */
#define BCM_RSVD4_CONTROL_MSG_PROC_FULL     _shrRsvd4CtrlMsgProcFull /* Full Message. */
#define BCM_RSVD4_CONTROL_MSG_PROC_FULL_WITH_TAG _shrRsvd4CtrlMsgProcFullWithTag /* Full Message with Tag. */

/*  RSVD4 control flow configuration */
typedef struct bcm_rsvd4_control_flow_config_s {
    uint32 match_mask;                  /* Header Mask */
    uint32 match_data;                  /* Header Match Data */
    uint8 queue_num;                    /*  Queue number */
    bcm_rsvd4_control_msg_proc_type_t type; /*  Message process type */
    int sync_en;                        /*  sync profile enable */
    uint8 sync_profile;                 /*  sync profile number */
    uint8 priority;                     /* 0-3 */
    uint32 queue_size;                  /* in bytes */
} bcm_rsvd4_control_flow_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add/Delete control message flow. */
extern int bcm_cpri_port_rsvd4_rx_control_flow_add(
    int unit, 
    int port, 
    int control_flow_id, 
    bcm_rsvd4_control_flow_config_t *config);

/* Add/Delete control message flow. */
extern int bcm_cpri_port_rsvd4_rx_control_flow_delete(
    int unit, 
    int port, 
    int control_flow_id, 
    bcm_rsvd4_control_flow_config_t *control_flow_cfg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_rsvd4_fast_eth_config_info_t */
typedef struct bcm_rsvd4_fast_eth_config_info_s {
    uint32 queue_num;       /* Queue Number */
    int no_fcs_err_check;   /* Ignore FCS Error */
    uint32 min_packet_size; /*  Minimum packet size */
    uint32 max_packet_size; /*  Maximum Packet Size */
    int min_packet_drop;    /*  Enable drop of Packet size below min packet size */
    int max_packet_drop;    /*  Enable drop of Packet size above  max packet
                               size */
    int strip_crc;          /* Strip CRC */
} bcm_rsvd4_fast_eth_config_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Setup the fast Ethernet control flow. */
extern int bcm_cpri_port_rsvd4_cm_fast_eth_config_set(
    int unit, 
    int port, 
    int control_flow_id, 
    bcm_rsvd4_fast_eth_config_info_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_rsvd4_sync_count_cycle_t */
typedef _shr_rsvd4_sync_count_cycle_t bcm_rsvd4_sync_count_cycle_t;

/* RSVD4 Sync Count Cycle. */
#define BCM_RSVD4_SYNC_COUNT_EVERY_MF       _shrRsvd4SyncCountCycleEveryMF 
#define BCM_RSVD4_SYNC_COUNT_UPTO_SIX_MF    _shrRsvd4SyncCountCycleUpToSixMF 

/*  */
typedef struct bcm_rsvd4_sync_profile_entry_s {
    uint32 master_frame_offset;         /*  Master frame Offset */
    uint32 message_offset;              /*  Message Number Offset */
    bcm_rsvd4_sync_count_cycle_t count_cycle; /*  Count Cycle */
} bcm_rsvd4_sync_profile_entry_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure/Retrieve sync profile entry. */
extern int bcm_cpri_port_rsvd4_sync_profile_entry_set(
    int unit, 
    int port, 
    int index, 
    bcm_rsvd4_sync_profile_entry_t *config);

/* Configure/Retrieve sync profile entry. */
extern int bcm_cpri_port_rsvd4_sync_profile_entry_get(
    int unit, 
    int port, 
    int index, 
    bcm_rsvd4_sync_profile_entry_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Tag Generation Entry information. */
typedef struct bcm_cpri_tag_gen_entry_s {
    uint32 tag_id;      /* Tag ID */
    uint32 mask;        /* Mask */
    uint32 header;      /* Header Info */
    uint32 word_count;  /* Word Count, Only for RTWP` */
} bcm_cpri_tag_gen_entry_t;

/*  RSVD4 Control flow group */
typedef enum bcm_rsvd4_control_group_id_e {
    bcmRsvd4ControlGroupId0 = 0,    /*  RSVD4 Control Flow Group 0 */
    bcmRsvd4ControlGroupId1 = 1     /*  RSVD4 Control Flow Group 1 */
} bcm_rsvd4_control_group_id_t;

/*  RSVD4 Slot Type */
typedef enum bcm_rsvd4_slot_type_e {
    bcmRsvd4SlotData = 0,   /*  RSVD4 Slot Type Data */
    bcmRsvd4SlotControl = 1 /*  RSVD4 Slot Type Control */
} bcm_rsvd4_slot_type_t;

/*  RSVD4 Priority */
typedef enum bcm_rsvd4_priority_e {
    bcmRsvd4Priority0 = 0,  /*  RSVD4 Priority 0 */
    bcmRsvd4Priority1 = 1,  /*  RSVD4 Priority 1 */
    bcmRsvd4Priority2 = 2,  /*  RSVD4 Priority 2 */
    bcmRsvd4Priority3 = 3   /*  RSVD4 Priority 3 */
} bcm_rsvd4_priority_t;

/*  RSVD4 Control flow configuration */
typedef struct bcm_rsvd4_tx_control_flow_group_config_s {
    uint8 queue_num;                    /*  Queue Number */
    bcm_rsvd4_control_msg_proc_type_t type; /*  Process type */
    bcm_rsvd4_priority_t priority;      /*  Queue priority */
} bcm_rsvd4_tx_control_flow_group_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add/Delete flow to/from rsvd4 tx control group. */
extern int bcm_cpri_port_rsvd4_tx_control_flow_group_member_add(
    int unit, 
    int port, 
    bcm_rsvd4_control_group_id_t group_num, 
    bcm_rsvd4_tx_control_flow_group_config_t *config);

/* Add/Delete flow to/from rsvd4 tx control group. */
extern int bcm_cpri_port_rsvd4_tx_control_flow_group_member_delete(
    int unit, 
    int port, 
    bcm_rsvd4_control_group_id_t group_num, 
    bcm_rsvd4_priority_t priority);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  */
typedef struct bcm_rsvd4_tx_cm_eth_config_s {
    uint8 msg_node;     /*  Message Node */
    uint8 msg_subnode;  /*  Message Subnode */
    uint8 msg_type;     /*  Message Type */
    uint8 msg_padding;  /*  Message Padding */
} bcm_rsvd4_tx_cm_eth_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure the rsvd4 tx fast Ethernet control. */
extern int bcm_cpri_port_rsvd4_tx_cm_eth_config_set(
    int unit, 
    int port, 
    bcm_rsvd4_tx_cm_eth_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  RSVD4 Tx control message raw configuration */
typedef struct bcm_rsvd4_tx_cm_raw_config_s {
    uint8 msg_id;   /*  Message ID */
    uint8 msg_type; /*  Message Type */
} bcm_rsvd4_tx_cm_raw_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure the rsvd4 tx raw control message. */
extern int bcm_cpri_port_rsvd4_tx_cm_raw_config_set(
    int unit, 
    int port, 
    bcm_rsvd4_tx_cm_raw_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  RSVD4 tx control flow configuration */
typedef struct bcm_rsvd4_tx_control_flow_config_s {
    uint8 queue_num;                    /*  Queue Number */
    uint8 crc_mode;                     /*  crc mode for multi eth control axc */
    uint32 cycle_size;                  /*  cycle size of control axc for buffer
                                           allocation */
    uint32 queue_size;                  /*  queue size of control axc */
    bcm_rsvd4_control_msg_proc_type_t type; /*  Message processing type */
} bcm_rsvd4_tx_control_flow_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add tx rsvd4 control flow. */
extern int bcm_cpri_port_rsvd4_tx_control_flow_add(
    int unit, 
    int port, 
    int control_flow_id, 
    bcm_rsvd4_tx_control_flow_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD4 Control Message  tunnel CRC options */
typedef _shr_rsvd4_crc_option_t bcm_rsvd4_cm_tunnel_crc_option_t;

/* RSVD4 Control Message  tunnel CRC options. */
#define BCM_RSVD4_CM_TUNNEL_CRC_USE_ROE     _shrRsvd4CrcOptionUseRoe /* Use CRC from ROE
                                                          packet. */
#define BCM_RSVD4_CM_TUNNEL_CRC_REGENERATE  _shrRsvd4CrcOptionRegenerate /* Regenerate New CRC */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set tx rsvd4 tunnel control message crc option. */
extern int bcm_cpri_port_rsvd4_tx_cm_tunnel_crc_config_set(
    int unit, 
    int port, 
    bcm_rsvd4_cm_tunnel_crc_option_t crc_option);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  RSVD4 control message header config */
typedef struct bcm_rsvd4_tx_cm_hdr_entry_s {
    uint8 rsvd4_header_node;            /*  Message Header Node */
    uint8 rsvd4_header_subnode;         /*  Message Header subnode */
    uint8 rsvd4_control_payload_node;   /*  Control Payload node */
} bcm_rsvd4_tx_cm_hdr_entry_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure rsvd4 tx control message header. */
extern int bcm_cpri_port_rsvd4_tx_cm_hdr_config_entry_set(
    int unit, 
    int port, 
    int index, 
    bcm_rsvd4_tx_cm_hdr_entry_t *entry);

/* Configure rsvd4 tx control message header look up set. */
extern int bcm_cpri_port_rsvd4_tx_cm_hdr_lkup_entry_set(
    int unit, 
    int port, 
    int flow_id, 
    int header_index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_cpri_crc_mode_t */
typedef _shr_cpri_hdlc_crc_mode_t bcm_cpri_crc_mode_t;

/* bcm_cpri_hdlc_flag_size_t */
typedef _shr_cpri_hdlc_flag_size_t bcm_cpri_hdlc_flag_size_t;

/* cpri_cw_filter_mode_t */
typedef _shr_cpri_cw_filter_mode_t bcm_cpri_cw_filter_mode_t;

/* bcm_cpri_hdlc_filling_pattern_t */
typedef _shr_cpri_hdlc_filling_flag_pattern_t bcm_cpri_hdlc_filling_pattern_t;

/* bcm_cpri_gcw_mask_t */
typedef _shr_cpri_gcw_mask_t bcm_cpri_gcw_mask_t;

/* bcm_cpri_l1_signal_prot_map_t */
typedef _shr_cpri_l1_signal_prot_map_t bcm_cpri_l1_signal_prot_map_t;

/* Use on l1 signal protection API.. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOF  _SHR_L1_SIGNAL_PROT_MAP_LOF /*  Select/Add LOF bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOS  _SHR_L1_SIGNAL_PROT_MAP_LOS /*  Select/Add LOS bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_SDI  _SHR_L1_SIGNAL_PROT_MAP_SDI /*  Select/Add SDI bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RAI  _SHR_L1_SIGNAL_PROT_MAP_RAI /*  Select/Add RAI bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RESET _SHR_L1_SIGNAL_PROT_MAP_RESET /*  Select/Add RESET bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_ETH_PTR _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR /*  Select/Add ETH_PTR
                                                          bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_HDLC_RATE _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE /*  Select/Add HDLC Rate 
                                                          bit. */
#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_PROTOCOL_VER _SHR_L1_SIGNAL_PROT_MAP_VER /*  Select/Add Version
                                                          bit. */

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOF_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_LOF_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOS_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_LOS_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_SDI_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_SDI_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RAI_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_RAI_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RESET_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_RESET_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_ETH_PTR_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_HDLC_RATE_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_VER_SET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_VER_SET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOF_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_LOF_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOS_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_LOS_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_SDI_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_SDI_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RAI_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_RAI_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RESET_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_RESET_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_ETH_PTR_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_HDLC_RATE_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_VER_CLR(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_VER_CLR(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOF_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_LOF_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_LOS_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_LOS_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_SDI_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_SDI_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RAI_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_RAI_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_RESET_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_RESET_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_ETH_PTR_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_HDLC_RATE_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE_GET(flags) 

#define BCM_CPRI_CW_L1_SIGNAL_PROT_MAP_VER_GET(flags)  \
    _SHR_L1_SIGNAL_PROT_MAP_VER_GET(flags) 

#define BCM_CPRI_CRC_APPEND     _shrCpriHdlcCrcAppend 
#define BCM_CPRI_CRC_REPLACE    _shrCpriHdlcCrcReplace 
#define BCM_CPRI_CRC_NO_UPDATE  _shrCpriHdlcCrcNoUpdate 

#define BCM_CPRI_HDLC_FLAG_SIZE_1_BYTE  _shrCpriHdlcNumFlagSizeOneByte 
#define BCM_CPRI_HDLC_FLAG_SIZE_2_BYTE  _shrCpriHdlcNumFlagSizeTwoByte 

#define BCM_CPRI_HDLC_FILLING_PATTERN_7E    _shrCpriHdlcFillingFlagPattern7E 
#define BCM_CPRI_HDLC_FILLING_PATTERN_7F    _shrCpriHdlcFillingFlagPattern7F 
#define BCM_CPRI_HDLC_FILLING_PATTERN_FF    _shrCpriHdlcFillingFlagPatternFF 

#define BCM_CPRI_HDLC_FCS_SIZE_8_BITS   _shrCpriHdlcFcsSize8Bits 
#define BCM_CPRI_HDLC_FCS_SIZE_16_BITS  _shrCpriHdlcFcsSize16Bits 
#define BCM_CPRI_HDLC_FCS_SIZE_32_BITS  _shrCpriHdlcFcsSize32Bits 

#define BCM_CPRI_FILTER_DISABLE     _shrCpriFilterDisable 
#define BCM_CPRI_FILTER_NON_ZERO    _shrCpriFilterNonZero 
#define BCM_CPRI_FILTER_PERIODIC    _shrCpriFilterPeriodic 
#define BCM_CPRI_FILTER_CHANGE      _shrCpriFilterChange 
#define BCM_CPRI_FILTER_PATTERN     _shrCpriFilterPatternMatch 

#define BCM_CPRI_GCW_MASK_NO_BYTE   _shrCpriGcwMaskNone 
#define BCM_CPRI_GCW_MASK_LSB_BYTE  _shrCpriGcwMaskLSB 
#define BCM_CPRI_GCW_MASK_MSB_BYTE  _shrCpriGcwMaskMSB 
#define BCM_CPRI_GCW_MASK_BOTH_BYTE _shrCpriGcwMaskBOTH 

/* bcm_cpri_l1_function_t */
typedef _shr_cpri_l1_function_t bcm_cpri_l1_function_t;

/* Z.130.0 L1 Function Bits. */
#define BCM_CPRI_CW_L1_FUNCTION_RESET   _SHR_L1_FUNCTION_RESET /* L1 function reset bit. */
#define BCM_CPRI_CW_L1_FUNCTION_RAI     _SHR_L1_FUNCTION_RAI /* L1 function Remote Alarm
                                                      Indication (RAI) bit. */
#define BCM_CPRI_CW_L1_FUNCTION_SDI     _SHR_L1_FUNCTION_SDI /* L1 function SAP Defect
                                                      Indication (SDI) bit. */
#define BCM_CPRI_CW_L1_FUNCTION_LOS     _SHR_L1_FUNCTION_LOS /* L1 function Loss of Signal
                                                      (LOS) bit. */
#define BCM_CPRI_CW_L1_FUNCTION_LOF     _SHR_L1_FUNCTION_LOF /* L1 function Loss of Frame
                                                      (LOF) bit. */

#define BCM_CPRI_CW_L1_FUNCTION_RESET_SET(flags)  \
    _SHR_L1_FUNCTION_RESET_SET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_RAI_SET(flags)  \
    _SHR_L1_FUNCTION_RAI_SET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_SDI_SET(flags)  \
    _SHR_L1_FUNCTION_SDI_SET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_LOS_SET(flags)  \
    _SHR_L1_FUNCTION_LOS_SET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_LOF_SET(flags)  \
    _SHR_L1_FUNCTION_LOF_SET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_RESET_GET(flags)  \
    _SHR_L1_FUNCTION_RESET_GET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_RAI_GET(flags)  \
    _SHR_L1_FUNCTION_RAI_GET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_SDI_GET(flags)  \
    _SHR_L1_FUNCTION_SDI_GET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_LOS_GET(flags)  \
    _SHR_L1_FUNCTION_LOS_GET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_LOF_GET(flags)  \
    _SHR_L1_FUNCTION_LOF_GET(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_RESET_CLR(flags)  \
    _SHR_L1_FUNCTION_RESET_CLR(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_RAI_CLR(flags)  \
    _SHR_L1_FUNCTION_RAI_CLR(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_SDI_CLR(flags)  \
    _SHR_L1_FUNCTION_SDI_CLR(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_LOS_CLR(flags)  \
    _SHR_L1_FUNCTION_LOS_CLR(flags) 

#define BCM_CPRI_CW_L1_FUNCTION_LOF_CLR(flags)  \
    _SHR_L1_FUNCTION_LOF_CLR(flags) 

/* Z.66.0 HDLC Rate. */
#define BCM_CPRI_HDLC_RATE_NO_HDLC      0x00       /*  No HDLC . */
#define BCM_CPRI_HDLC_RATE_240KBPS      0x01       /*  240 Kbps . */
#define BCM_CPRI_HDLC_RATE_480KBPS      0x02       /*  480 Kbps . */
#define BCM_CPRI_HDLC_RATE_960KBPS      0x03       /*  960 Kbps . */
#define BCM_CPRI_HDLC_RATE_1920KBPS     0x04       /*  1920 Kbps . */
#define BCM_CPRI_HDLC_RATE_2400KBPS     0x05       /*  2400 Kbps . */
#define BCM_CPRI_HDLC_RATE_HIGHEST      0x06       /*  Highest Rate . */
#define BCM_CPRI_HDLC_RATE_NEGOTIATED   0x07       /* Negotiated on higher
                                                      layer. */

/* CPRI Control Word Layer 1 inband  information. */
typedef struct bcm_cpri_cw_l1_inband_info_s {
    uint32 eth_ptr;         /* Z.194.0 Ethernet pointer p. */
    uint32 layer1_function; /* Z.130.0 l1 function */
    uint32 hdlc_rate;       /* Z.66.0 HDLC rate */
    uint32 protocol_ver;    /* Z.2.0 version 1 or 2 */
} bcm_cpri_cw_l1_inband_info_t;

typedef enum bcm_cpri_crc_init_value_e {
    bcmCpriHdlcCrcInitValAll0 = _shrCpriHdlcCrcInitValAll0, /* All 0s */
    bcmCpriHdlcCrcInitValAll1 = _shrCpriHdlcCrcInitValAll1, /*  All 1s */
    bcmCpriHdlcCrcInitValCount = _shrCpriHdlcCrcInitValCount /* Should be last */
} bcm_cpri_crc_init_value_t;

typedef enum bcm_cpri_hdlc_fcs_size_e {
    bcmCpriHdlcFcsSize8Bits = _shrCpriHdlcFcsSize8Bits, /*  8 bits */
    bcmCpriHdlcFcsSize16Bits = _shrCpriHdlcFcsSize16Bits, /*  16 bits */
    bcmCpriHdlcFcsSize32Bits = _shrCpriHdlcFcsSize32Bits, /*  32 bits */
    bcmCpriHdlcFcsSizeCount = _shrCpriHdlcFcsSizeCount /* Should be last */
} bcm_cpri_hdlc_fcs_size_t;

/*  */
typedef struct bcm_cpri_slow_hdlc_config_info_s {
    bcm_cpri_crc_mode_t hdlc_crc_mode;  /*  HDLC CRC mode */
    bcm_cpri_hdlc_flag_size_t tx_flag_size; /*  Flag Size */
    bcm_cpri_hdlc_filling_pattern_t tx_filling_pattern; /*  Fill Pattern */
    bcm_cpri_crc_init_value_t crc_init_val; /* all 0's or all 1's */
    int use_fe_mac;                     /*  True/False */
    int crc_byte_swap;                  /*  True/False */
    int no_fcs_err_check;               /*  True/False */
    uint32 hdlc_cw_sel;                 /*  Control Word Select */
    uint32 hdlc_cw_size;                /*  Control Word Size */
    bcm_cpri_hdlc_fcs_size_t hdlc_fcs_size; /*  FCS size */
    int hdlc_run_drop;                  /*  Runt Drop Threshold */
    int hdlc_max_drop;                  /*  Max Drop Threshold */
    uint32 hdlc_min_size;               /*  Min Size Thereshold */
    uint32 hdlc_max_size;               /*  Max Size Threshold */
    uint32 hdlc_queue_num;              /*  Queue number */
    uint32 priority;                    /*  0-3 */
    uint32 queue_size;                  /*  in bytes */
    uint32 cycle_size;                  /*  Jitter Buffer Size in number of
                                           packets */
    uint32 buffer_size;                 /* pkt size */
} bcm_cpri_slow_hdlc_config_info_t;

/*  */
typedef struct bcm_cpri_fast_eth_config_info_s {
    uint32 sub_channel_start;       /*  The first sub-channel allocated for fast
                                       c&m */
    uint32 sub_channel_size;        /*  sub channel size */
    uint32 cw_sel;                  /*  Select sub-channel words */
    uint32 cw_size;                 /*  Number of bytes per word to use. Valid
                                       values are 0 to 16. */
    uint32 queue_num;               /*  queue number */
    int ignore_fcs_err;             /*   True or False */
    uint32 min_pkt_size;            /*  min packet size */
    uint32 max_pkt_size;            /*  max packet size */
    int drop_undersize_pkt;         /*  Enable drop of Packet size below min
                                       packet size */
    int drop_oversize_pkt;          /*  Enable drop of Packet size above max
                                       packet size */
    int strip_crc;                  /*   True or False */
    uint32 min_ipg;                 /*  Min Inter Packet Gap */
    bcm_cpri_crc_mode_t crc_mode;   /*  0-append 1-replace 2-No update */
    uint32 priority;                /*  0-3 */
    uint32 queue_size;              /*  in bytes */
    uint32 cycle_size;              /*  Jitter Buffer size, in number of packets */
    uint32 buffer_size;             /*  pkt size */
} bcm_cpri_fast_eth_config_info_t;

/* bcm_cpri_vsd_control_subchan_num_bytes_t */
typedef _shr_cpri_vsd_control_subchan_num_bytes_t bcm_cpri_vsd_control_subchan_num_bytes_t;

#define BCM_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_1 _shrVsdCtrlSubchanNumBytes1 
#define BCM_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_2 _shrVsdCtrlSubchanNumBytes2 

/* bcm_cpri_vsd_control_subchan_size_t */
typedef _shr_cpri_vsd_control_subchan_size_t bcm_cpri_vsd_control_subchan_size_t;

#define BCM_CPRI_VSD_CONTROL_SUBCHAN_SIZE_2 _shrVsdCtrlSubchanSize2 
#define BCM_CPRI_VSD_CONTROL_SUBCHAN_SIZE_4 _shrVsdCtrlSubchanSize4 

/* bcm_cpri_vsd_control_flow_num_bytes_t */
typedef _shr_cpri_vsd_control_flow_num_bytes_t bcm_cpri_vsd_control_flow_num_bytes_t;

#define BCM_CPRI_CONTROL_FLOW_NUM_BYTES_2   _shrVsdCtrlFlowNumBytes2 
#define BCM_CPRI_CONTROL_FLOW_NUM_BYTES_4   _shrVsdCtrlFlowNumBytes4 

/*  */
typedef struct bcm_cpri_rx_vsd_config_info_s {
    uint32 subchan_start;               /*  starting control subchannel number */
    bcm_cpri_vsd_control_subchan_size_t subchan_size; /*  2 or 4 sub channels. */
    bcm_cpri_vsd_control_subchan_num_bytes_t subchan_bytes; /* Sub Channel Number of Bytes 1,2 or 4. */
    int subchan_step;                   /*  0 - No Skip    1- Skip One Sub Chan. */
    uint32 queue_num;                   /*  Queue Number */
    uint32 vsd_valid_sector_mask[4];    /*  total of 128 possible sectors */
    uint32 priority;                    /* 0-3 */
    uint32 queue_size;                  /* in bytes */
    bcm_cpri_vsd_control_flow_num_bytes_t flow_bytes; /* number of bytes per flow 2 or 4. */
} bcm_cpri_rx_vsd_config_info_t;

/*  VSD Flow Information */
typedef struct bcm_cpri_rx_vsd_flow_info_s {
    uint32 hyper_frame_number;  /*  Hyper Frame Number */
    uint32 hyper_frame_modulo;  /* Hyper Frame Modulo */
    int filter_zero_data;       /* Filer all 0's Data. */
    uint32 section_num[4];      /*  Section numbers of the group, 0 .. 127 . */
    uint32 num_sector;          /*  Number of Sectors */
    uint32 tag_id;              /*  Tag ID */
} bcm_cpri_rx_vsd_flow_info_t;

/*  */
typedef struct bcm_cpri_rx_vsd_raw_config_info_s {
    uint32 schan_start;                 /* starting control subchannel number */
    uint32 schan_size;                  /*  Number of sub channels */
    uint32 cw_sel;                      /*  4 bits bit map. which Xs bytes to
                                           use */
    uint32 cw_size;                     /*  number of bytes per word to extract
                                           0 to Tcw/8. Max 16 bytes */
    uint32 queue_num;                   /* The queue number to be used for the
                                           payload data in the encapsulation
                                           memory. */
    uint32 match_mask;                  /* Mask to apply to the data before
                                           comparing to the match_value. */
    uint32 match_value;                 /* Value to match. */
    uint32 match_offset;                /* The offset of the first byte in the
                                           flow to match. */
    uint32 hfn_modulo;                  /* Modulo value. Valid values are 1 to
                                           150. */
    uint32 hfn_offset;                  /* Programmable offset. Valid values are
                                           0 to 149. */
    bcm_cpri_cw_filter_mode_t filter_mode; /*   Filter Mode */
    uint32 priority;                    /*  0-3 */
    uint32 queue_size;                  /*  in_bytes */
} bcm_cpri_rx_vsd_raw_config_info_t;

/*  */
typedef struct bcm_cpri_rx_rsvd5_config_info_s {
    uint32 schan_start;         /* The first sub-channel allocated for the RSVD5
                                   parser. Valid values are 16 to 63 */
    uint32 schan_size;          /*  Number of sub channels 1, 2 or 4 for CPRI
                                   4x, 8x and 16x.2 or 4 for CPRI 2x and
                                   10x.Setting the field to 0 disables the
                                   engine. Values other than 1, 2, 4 not
                                   supported. */
    int disable_parity_check;   /*  True/False */
    uint32 queue_num;           /*  Queue Number */
    uint32 priority;            /*  0-3 */
    uint32 queue_size;          /*  in_bytes */
} bcm_cpri_rx_rsvd5_config_info_t;

/*  Tag configuration */
typedef struct bcm_cpri_control_tag_config_s {
    uint32 default_tag;     /*  Default tag ID */
    uint32 no_match_tag;    /*  No Match Tag ID */
} bcm_cpri_control_tag_config_t;

/*  RX GCW Configuration */
typedef struct bcm_cpri_rx_gcw_config_info_s {
    uint32 Ns;                          /*  sub channel # */
    uint32 Xs;                          /*  word number */
    uint32 Y;                           /*  first byte number valid value 0-7
                                           for 0,2,4,..14) */
    bcm_cpri_gcw_mask_t mask;           /*  Mask */
    bcm_cpri_cw_filter_mode_t filter_mode; /*  Filter Mode */
    uint32 hfn_index;                   /*  Programmable offset. Valid values
                                           are 0 to 149. */
    uint32 hfn_modulo;                  /*  Modulo value. Valid values are 1 to
                                           150. */
    uint32 match_value;                 /*  Match Value */
    uint32 match_mask;                  /*  Mask to apply to the data before
                                           comparing to the match_value. */
} bcm_cpri_rx_gcw_config_info_t;

/*  Control Word Sync Info */
typedef struct bcm_cpri_cw_sync_info_s {
    uint32 hfn; /*  current hyperframe number Z.64.0 */
    uint32 bfn; /*  cpri radio frame number Z.192.0 Z.128.0 */
} bcm_cpri_cw_sync_info_t;

/*  VSD Config informations. */
typedef struct bcm_cpri_tx_vsd_config_info_s {
    uint32 subchan_start;               /*  starting control subchannel number */
    bcm_cpri_vsd_control_subchan_size_t subchan_size; /*  2 or 4 sub channels. */
    bcm_cpri_vsd_control_subchan_num_bytes_t subchan_bytes; /* Sub Channel Number of Bytes 1,2 or 4. */
    int subchan_step;                   /*  0 - No Skip    1- Skip One Sub Chan. */
    uint32 queue;                       /*  0x44 (68) default */
    uint32 vsd_valid_sector_mask[4];    /*  total of 128 possible sectors */
    uint32 cycle_size;                  /*  hdlc rate */
    uint32 buffer_size;                 /*  pkt size */
    bcm_cpri_vsd_control_flow_num_bytes_t flow_bytes; /* number of bytes per flow 2 or 4. */
    int byte_order_swap;                /* Byte Order Swap 0:[15:8]=byte0
                                           [7:0]=byte1 1: [15:8]=byte1
                                           [7:0]=byte0 */
} bcm_cpri_tx_vsd_config_info_t;

/*  VSD Flow Information */
typedef struct bcm_cpri_tx_vsd_flow_info_s {
    uint32 hyper_frame_number;  /*  Hyper Frame Number */
    uint32 hyper_frame_modulo;  /* Hyper Frame Modulo */
    int repeat_mode;            /* Repeat Mode. */
    uint32 section_num[4];      /*  Section numbers of the group, 0 .. 127 . */
    uint32 num_sector;          /*  Number of Sectors */
    uint32 roe_flow_id;         /*  RoE Flow ID */
} bcm_cpri_tx_vsd_flow_info_t;

/*  VSD Raw configuration */
typedef struct bcm_cpri_tx_vsd_raw_config_info_s {
    uint32 schan_start;     /* starting control subchannel number */
    uint32 schan_size;      /*  Number of sub channels */
    uint32 cw_sel;          /*  4 bits bit map. which Xs bytes to use */
    uint32 cw_size;         /*  number of bytes per word to extract 0 to Tcw/8.
                               Max 16 bytes */
    uint32 queue_num;       /* The decap queue number to be used for VSD raw
                               data. Valid values are 64-79. Should be different
                               than other control queue numbers.. */
    uint32 map_mode;        /*  Map Mode */
    int repeat_mode;        /*  True/False */
    int bfn0_filter_enable; /*  True/False */
    int bfn1_filter_enable; /*  True/False */
    uint32 hfn_index;       /* hfn number */
    uint32 hfn_modulo;      /*  hfn modulo */
    uint32 idle_value;      /*  Idle value */
    uint32 cycle_size;      /*  jitter buffer size in number of pakcets */
    uint32 buffer_size;     /*  packet size size */
} bcm_cpri_tx_vsd_raw_config_info_t;

/*  VSD raw fiter information */
typedef struct bcm_cpri_tx_cw_vsd_raw_filter_info_s {
    uint32 bfn0_value;  /*  BFN value for filter */
    uint32 bfn0_mask;   /*  BFN mask for filter */
    uint32 bfn1_value;  /*  BFN value for filter */
    uint32 bfn1_mask;   /*  BFN mask for filter */
} bcm_cpri_tx_cw_vsd_raw_filter_info_t;

/*  RSVD5 configuration */
typedef struct bcm_cpri_tx_cw_rsvd5_config_info_s {
    uint32 schan_start;     /* The first sub-channel allocated for the RSVD5
                               parser. Valid values are 16 to 63 */
    uint32 schan_size;      /*  Number of sub channels 1, 2 or 4 for CPRI 4x, 8x
                               and 16x.2 or 4 for CPRI 2x and 10x.Setting the
                               field to 0 disables the engine. Values other than
                               1, 2, 4 not supported. */
    int crc_check_enable;   /*  True/False */
    uint32 queue_num;       /*   Queue number */
    uint32 cycle_size;      /*  Jitter buffer size in number of packets */
    uint32 buffer_size;     /*  packet size */
} bcm_cpri_tx_cw_rsvd5_config_info_t;

/*  GCW config information */
typedef struct bcm_cpri_tx_gcw_config_info_s {
    uint32 Ns;                  /*  sub channel # */
    uint32 Xs;                  /*  word number */
    uint32 Y;                   /*  first byte number valid value 0-7 for
                                   0,2,4,..14) */
    bcm_cpri_gcw_mask_t mask;   /*  Mask */
    int repeat_mode;            /*  True/False */
    int bfn0_filter_enable;     /*  Insert only when BFN matches values
                                   specified by the BFN configuration register. */
    int bfn1_filter_enable;     /*  Insert only when BFN matches values
                                   specified by the BFN configuration register. */
    uint32 hfn_index;           /*  Programmable offset. Valid values are 0 to
                                   149. */
    uint32 hfn_modulo;          /*  Modulo value. Valid values are 1 to 150. */
} bcm_cpri_tx_gcw_config_info_t;

/*  GCW Filter information */
typedef struct bcm_cpri_tx_gcw_tx_filter_info_s {
    uint32 bfn0_value;  /*  BFN value for filter */
    uint32 bfn0_mask;   /*  BFN mask for filter */
    uint32 bfn1_value;  /*  BFN value for filter */
    uint32 bfn1_mask;   /*  BFN mask for filter */
} bcm_cpri_tx_gcw_tx_filter_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_cpri_port_rx_cw_sync_info_get(
    int unit, 
    int port, 
    uint32 *hyper_frame_num, 
    uint32 *radio_frame_num);

/* Get the control word L1 Inband Protocol information. */
extern int bcm_cpri_port_rx_cw_l1_inband_info_get(
    int unit, 
    int port, 
    bcm_cpri_cw_l1_inband_info_t *l1_inband_info);

/* Set/Get the control word signal protection. */
extern int bcm_cpri_port_rx_cw_l1_signal_protection_set(
    int unit, 
    int port, 
    uint32 signal_map, 
    uint32 enable);

/* Set/Get the control word signal protection. */
extern int bcm_cpri_port_rx_cw_l1_signal_protection_get(
    int unit, 
    int port, 
    uint32 signal_map, 
    uint32 *enable);

/* Sets/Gets the hdlc control flow. */
extern int bcm_cpri_port_cw_slow_hdlc_config_set(
    int unit, 
    int port, 
    bcm_cpri_slow_hdlc_config_info_t *config);

/* Sets/Gets the hdlc control flow. */
extern int bcm_cpri_port_cw_slow_hdlc_config_get(
    int unit, 
    int port, 
    bcm_cpri_slow_hdlc_config_info_t *config);

/* Sets/Gets the fast Ethernet control flow. */
extern int bcm_cpri_port_cw_fast_eth_config_set(
    int unit, 
    int port, 
    bcm_cpri_fast_eth_config_info_t *config);

/* Sets/Gets the fast Ethernet control flow. */
extern int bcm_cpri_port_cw_fast_eth_config_get(
    int unit, 
    int port, 
    bcm_cpri_fast_eth_config_info_t *config);

/* 
 * This API configures/retrieves the VSD (Vendor Specific Data) control
 * stream information.
 */
extern int bcm_cpri_port_rx_cw_vsd_config_set(
    int unit, 
    int port, 
    bcm_cpri_rx_vsd_config_info_t *config);

/* 
 * This API configures/retrieves the VSD (Vendor Specific Data) control
 * stream information.
 */
extern int bcm_cpri_port_rx_cw_vsd_config_get(
    int unit, 
    int port, 
    bcm_cpri_rx_vsd_config_info_t *config);

/* Configure/Retrieve the VSD(Vendor Specific Data) control flow */
extern int bcm_cpri_port_rx_cw_vsd_ctrl_flow_add(
    int unit, 
    int port, 
    uint16 group_id, 
    bcm_cpri_rx_vsd_flow_info_t *config);

/* Configure/Retrieve the VSD(Vendor Specific Data) control flow */
extern int bcm_cpri_port_rx_cw_vsd_ctrl_flow_delete(
    int unit, 
    int port, 
    uint16 group_id);

/* Configure/Retrieve the VSD(Vendor Specific Data) control flow */
extern int bcm_cpri_port_rx_cw_vsd_ctrl_flow_get(
    int unit, 
    int port, 
    uint16 group_id, 
    bcm_cpri_rx_vsd_flow_info_t *config);

/* Configure the tag to flow mapping. */
extern int bcm_cpri_encap_control_queue_tag_to_flow_id_map_set(
    int unit, 
    int port, 
    uint32 tag_id, 
    uint32 flow_id);

extern int bcm_cpri_port_rx_cw_vsd_raw_config_set(
    int unit, 
    int port, 
    uint8 vsd_raw_id, 
    bcm_cpri_rx_vsd_raw_config_info_t *config);

extern int bcm_cpri_port_rx_cw_vsd_raw_config_get(
    int unit, 
    int port, 
    uint8 vsd_raw_id, 
    bcm_cpri_rx_vsd_raw_config_info_t *config);

/* Set/Get rsvd5 configuration on a port. */
extern int bcm_cpri_port_rx_cw_rsvd5_config_set(
    int unit, 
    int port, 
    bcm_cpri_rx_rsvd5_config_info_t *config);

/* Set/Get rsvd5 configuration on a port. */
extern int bcm_cpri_port_rx_cw_rsvd5_config_get(
    int unit, 
    int port, 
    bcm_cpri_rx_rsvd5_config_info_t *config);

/* Configure rx default and no match tag. */
extern int bcm_cpri_port_rx_tag_config_set(
    int unit, 
    int port, 
    bcm_cpri_control_tag_config_t *config);

/* Add/Delete entry from tag generation table. */
extern int bcm_cpri_port_rx_tag_entry_add(
    int unit, 
    int port, 
    bcm_cpri_tag_gen_entry_t *config);

/* Add/Delete entry from tag generation table. */
extern int bcm_cpri_port_rx_tag_entry_delete(
    int unit, 
    int port, 
    bcm_cpri_tag_gen_entry_t *config);

/* Set/Get CPRI generic control word configuration. */
extern int bcm_cpri_port_rx_cw_gcw_config_set(
    int unit, 
    int port, 
    uint8 index, 
    bcm_cpri_rx_gcw_config_info_t *config);

/* Set/Get CPRI generic control word configuration. */
extern int bcm_cpri_port_rx_cw_gcw_config_get(
    int unit, 
    int port, 
    uint8 index, 
    bcm_cpri_rx_gcw_config_info_t *config);

/* Configure tx control word sync information. */
extern int bcm_cpri_port_tx_cw_sync_info_set(
    int unit, 
    int port, 
    bcm_cpri_cw_sync_info_t *entry);

/* Configure tx control word sync information. */
extern int bcm_cpri_port_tx_cw_sync_info_get(
    int unit, 
    int port, 
    bcm_cpri_cw_sync_info_t *entry);

/* Set/Get CPRI Tx control word layer 1 inband information. */
extern int bcm_cpri_port_tx_cw_l1_inband_info_set(
    int unit, 
    int port, 
    bcm_cpri_cw_l1_inband_info_t *inband_info);

/* Set/Get CPRI Tx control word layer 1 inband information. */
extern int bcm_cpri_port_tx_cw_l1_inband_info_get(
    int unit, 
    int port, 
    bcm_cpri_cw_l1_inband_info_t *inband_info);

/* Set/Get CPRI tx vendor specific control word. */
extern int bcm_cpri_port_tx_cw_vsd_config_set(
    int unit, 
    int port, 
    bcm_cpri_tx_vsd_config_info_t *entry);

/* Set/Get CPRI tx vendor specific control word. */
extern int bcm_cpri_port_tx_cw_vsd_config_get(
    int unit, 
    int port, 
    bcm_cpri_tx_vsd_config_info_t *entry);

/* Set CPRI tx vendor specific control flow. */
extern int bcm_cpri_port_tx_cw_vsd_ctrl_flow_add(
    int unit, 
    int port, 
    uint16 group_id, 
    bcm_cpri_tx_vsd_flow_info_t *config);

/* Set CPRI tx vendor specific control flow. */
extern int bcm_cpri_port_tx_cw_vsd_ctrl_flow_delete(
    int unit, 
    int port, 
    uint16 group_id);

/* Set CPRI tx vendor specific control flow. */
extern int bcm_cpri_port_tx_cw_vsd_ctrl_flow_get(
    int unit, 
    int port, 
    uint16 group_id, 
    bcm_cpri_tx_vsd_flow_info_t *config);

/* Configure/Retrieve CPRI tx vendor specific raw flow configuration. */
extern int bcm_cpri_port_tx_cw_vsd_raw_flow_config_set(
    int unit, 
    int port, 
    uint8 index, 
    bcm_cpri_tx_vsd_raw_config_info_t *config);

/* Configure/Retrieve CPRI tx vendor specific raw flow configuration. */
extern int bcm_cpri_port_tx_cw_vsd_raw_flow_config_get(
    int unit, 
    int port, 
    uint8 index, 
    bcm_cpri_tx_vsd_raw_config_info_t *config);

/* Set/Get tx vendor specific raw control filter configuration. */
extern int bcm_cpri_port_tx_cw_vsd_raw_filter_set(
    int unit, 
    int port, 
    bcm_cpri_tx_cw_vsd_raw_filter_info_t *config);

/* Set/Get tx vendor specific raw control filter configuration. */
extern int bcm_cpri_port_tx_cw_vsd_raw_filter_get(
    int unit, 
    int port, 
    bcm_cpri_tx_cw_vsd_raw_filter_info_t *config);

/* Set/Get CPRI Tx RSVD5 control configuration. */
extern int bcm_cpri_port_tx_cw_rsvd5_config_set(
    int unit, 
    int port, 
    bcm_cpri_tx_cw_rsvd5_config_info_t *config);

/* Set/Get CPRI Tx RSVD5 control configuration. */
extern int bcm_cpri_port_tx_cw_rsvd5_config_get(
    int unit, 
    int port, 
    bcm_cpri_tx_cw_rsvd5_config_info_t *config);

/* Set/GetCPRI Tx generic control word configuration. */
extern int bcm_cpri_port_tx_cw_gcw_config_set(
    int unit, 
    int port, 
    uint8 index, 
    bcm_cpri_tx_gcw_config_info_t *config);

/* Set/GetCPRI Tx generic control word configuration. */
extern int bcm_cpri_port_tx_cw_gcw_config_get(
    int unit, 
    int port, 
    uint8 index, 
    bcm_cpri_tx_gcw_config_info_t *config);

/* Set/Get CPRI Tx generic control word filter configuration. */
extern int bcm_cpri_port_tx_cw_gcw_filter_set(
    int unit, 
    int port, 
    bcm_cpri_tx_gcw_tx_filter_info_t *config);

/* Set/Get CPRI Tx generic control word filter configuration. */
extern int bcm_cpri_port_tx_cw_gcw_filter_get(
    int unit, 
    int port, 
    bcm_cpri_tx_gcw_tx_filter_info_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI interrupt types */
typedef _shr_cpri_interrupt_type_t bcm_cpri_interrupt_type_t;

/* CPRI interrupt types */
#define BCM_CPRI_INTR_RX_PCS_64B66B_BLK_LCK_LL _shrCpriIntrRxPcs64B66BBlkLckLl /* RX PCS Block lock
                                                          status - latch low
                                                          version. */
#define BCM_CPRI_INTR_RX_PCS_64B66B_BLK_LCK_LH _shrCpriIntrRxPcs64B66BBlkLckLh /*  RX PCS Block lock
                                                          status - latch high
                                                          version. */
#define BCM_CPRI_INTR_RX_PCS_64B66B_HI_BER_LL _shrCpriIntrRxPcs64B66BHiBerLl /* RX PCS BER - latch low
                                                          version. */
#define BCM_CPRI_INTR_RX_PCS_64B66B_HI_BER_LH _shrCpriIntrRxPcs64B66BHiBerLh /* RX PCS BER - latch
                                                          high version. */
#define BCM_CPRI_INTR_RX_PCS_LOS_LL         _shrCpriIntrRxPcsLosLl /* RX PCS LOS - CPRI port
                                                          Link UP. */
#define BCM_CPRI_INTR_RX_PCS_LOS_LH         _shrCpriIntrRxPcsLosLh /* RX PCS LOS - CPRI port
                                                          Link DOWN. */
#define BCM_CPRI_INTR_RX_PCS_LINK_STATUS_LL _shrCpriIntrRxPcsLinkStatusLl /* RX PCS link status -
                                                          latch low version. */
#define BCM_CPRI_INTR_RX_PCS_LINK_STATUS_LH _shrCpriIntrRxPcsLinkStatusLh /* RX PCS link status -
                                                          latch high version. */
#define BCM_CPRI_INTR_RX_PCS_EXTRACT_FIFO_OVRFLOW _shrCpriIntrRxPcsExtractFifoOvrFlow /* RX PCS extract FIFO
                                                          overflow interrupt. */
#define BCM_CPRI_INTR_RX_GCW                _shrCpriIntrRxGcw /* Interrupt for RX GCW
                                                          Word(0-15) is loaded. */
#define BCM_CPRI_INTR_RX_1588_TS_FIFO       _shrCpriIntrRx1588TsFifo /* RX 1588 FIFO intr */
#define BCM_CPRI_INTR_RX_1588_CAPTURED_TS   _shrCpriIntrRx1588CapturedTs /* RX 1588 Sw capture
                                                          interrupt. */
#define BCM_CPRI_INTR_RX_ENCAP_DATA_Q_OVERFLOW _shrCpriIntrRxEncapDataQOverflow /* Encap Data Queue FIFO
                                                          Over-Flow */
#define BCM_CPRI_INTR_RX_ENCAP_CTRL_Q_OVERFLOW _shrCpriIntrRxEncapCtrlQOverflow /* Encap Ctrl Queue FIFO
                                                          Over-Flow */
#define BCM_CPRI_INTR_RX_ENCAP_GSM_TS_Q_ERR _shrCpriIntrRxEncapGsmTsQErr /* Encap Per Queue GSM
                                                          Time-Slot Error */
#define BCM_CPRI_INTR_RX_ENCAP_WORKQ_FIFO_OVERFLOW _shrCpriIntrRxEncapWqFiFoOverflow /* Encap Per Work-Queue
                                                          FIFO Over Error */
#define BCM_CPRI_INTR_RX_ENCAP_RXFRM_IN_FIFO_OVERFLOW _shrCpriIntrRxEncapRxFrmInFifoOverflow /* Encap RX-framer
                                                          Input-buffer FIFO Over
                                                          Error */
#define BCM_CPRI_INTR_RX_ENCAP_IPSM_CDC_FIFO_ERR _shrCpriIntrRxEncapIpsmCdcFifoErr /* Encap IPSM CDC FIFO
                                                          Error */
#define BCM_CPRI_INTR_RX_FRM_RSVD4_TS       _shrCpriIntrRxFrmRsvd4Ts /* Per Axc(0-63)
                                                          Timestamp interrupt in
                                                          RSVD4 mode */
#define BCM_CPRI_INTR_RX_FRM_RSVD4_SINGLE_TS_ERR _shrCpriIntrRxFrmRsvd4SingleTsErr /*  single timestamp
                                                          error in RSVD4 mode */
#define BCM_CPRI_INTR_RX_FRM_L1_SIGNAL_CHANGE _shrCpriIntrRxFrmL1SigChange /* L1 signal change */
#define BCM_CPRI_INTR_RX_PCS_FEC_CS_CW_BAD  _shrCpriIntrRxPcsFecCsCwBad /*  cw_bad during CW_SYNC
                                                          state */
#define BCM_CPRI_INTR_TX_DECAP_DATA_Q_OVFL_ERR _shrCpriIntrTxDecapDataQOvflErr /* Decap Per Queue Decap
                                                          Data Queue FIFO
                                                          Over-Flow Error */
#define BCM_CPRI_INTR_TX_DECAP_DATA_Q_UDFL_ERR _shrCpriIntrTxDecapDataQUdflErr /* Decap Per Queue Decap
                                                          Data Queue FIFO
                                                          underflow Error */
#define BCM_CPRI_INTR_TX_DECAP_DATA_BUFSIZE_ERR _shrCpriIntrTxDecapDataQBufSizeErr /* Decap Per Queue Decap
                                                          Data Queue BUFF SIZE
                                                          Error */
#define BCM_CPRI_INTR_TX_DECAP_AG_MODE_PKT_MISS _shrCpriIntrTxDecapAgModePktMiss /* Agnostic mode missing
                                                          packet */
#define BCM_CPRI_INTR_TX_1588_CAPTURED_TS   _shrCpriIntrTx1588CapturedTs /* TX 1588 Sw capture
                                                          interrupt. */
#define BCM_CPRI_INTR_TX_1588_TS_FIFO       _shrCpriIntrTx1588TsFifo /* TX 1588 FIFO capture
                                                          interrupt. */
#define BCM_CPRI_INTR_TX_FRM_CWA_CTRL_PKT_FLOW_ID_ERR _shrCpriIntrTxFrmCwaCtrlPktFlowIdErr /* CWA ctrl flow packet
                                                          flow id error */
#define BCM_CPRI_INTR_TX_FRM_BFA_HF_START_MISALIGN _shrCpriIntrTxFrmBfaHfStartMisalign /* BFA hf start misalign */
#define BCM_CPRI_INTR_TX_FRM_DR_MULTI_ETH_PSIZE_UNDERSIZE _shrCpriIntrTxFrmDrMultiEthPsizeUndersize /* RSVD4 control proc
                                                          decap req multi eth -
                                                          packet undersize */
#define BCM_CPRI_INTR_TX_FRM_DR_ACK_MISALIGN _shrCpriIntrTxFrmDrAckMisalign /* RSVD4 control proc
                                                          decap req multi eth -
                                                          packet undersize */
#define BCM_CPRI_INTR_TX_FRM_CWA_MULTIPLE_VALIDS _shrCpriIntrTxFrmCwaMultipleValids /* CPRI control asm
                                                          valids from multiple
                                                          engines are asserted
                                                          at same time */
#define BCM_CPRI_INTR_TX_FRM_CWA_SDVM_PROC_OVERFLOW _shrCpriIntrTxFrmCwaSdvmProcOverflow /* CPRI control asm RSVD5
                                                          proc buffer overflow */
#define BCM_CPRI_INTR_TX_FRM_ARB_PSIZE_ZERO _shrCpriIntrTxFrmArbPsizeZero /* decap sop control word
                                                          packet size = 0 */
#define BCM_CPRI_INTR_TX_FRM_ARB_CPRI_ACK_MISMATCH _shrCpriIntrTxFrmArbCpriAckMismatch /* arbiter cpri ack
                                                          mismatch */
#define BCM_CPRI_INTR_TX_FRM_ARB_RSVD4_REQ_CONFLICT _shrCpriIntrTxFrmArbRsvd4ReqConflict /* arbiter RSVD4 req
                                                          conflict */
#define BCM_CPRI_INTR_TX_FRM_ARB_RSVD4_ACK_MISALIGN _shrCpriIntrTxFrmArbRsvd4AckMisalign /* arbiter RSVD4 ack
                                                          misaligned */
#define BCM_CPRI_INTR_TX_FRM_CWA_HDLC_BUF_OVERFLOW _shrCpriIntrTxFrmCwaHdlcBufferOverflow /* CPRI control asm hdlc
                                                          (slow cm) buffer
                                                          overflow */
#define BCM_CPRI_INTR_TX_FRM_CWA_FAST_ETH_BUF_OVERFLOW _shrCpriIntrTxFrmCwaFastEthBufferOverflow /* CPRI control asm fast
                                                          Ethernet (fast cm)
                                                          buffer overflow */
#define BCM_CPRI_INTR_TX_FRM_CWA_VSD_RAW_BUF_OVERFLOW _shrCpriIntrTxFrmCwaVsdRawBufferOverflow /* CPRI control asm vsd
                                                          raw buffer overflow */
#define BCM_CPRI_INTR_TX_FRM_CWA_SDVM_BUF_OVERFLOW _shrCpriIntrTxFrmCwaSdvmBufferOverflow /* CPRI control asm Rsvd5
                                                          buffer overflow */
#define BCM_CPRI_INTR_TX_FRM_MULTI_ETH_BUF_OVERFLOW _shrCpriIntrTxFrmMultiEthBufferOverflow /* RSVD4 control
                                                          multi-msg eth buffer
                                                          overflow */
#define BCM_CPRI_INTR_TX_FRM_MULTI_ETH_BUF_UNDERFLOW _shrCpriIntrTxFrmMultiEthBufferUnderflow /* RSVD4 control
                                                          multi-msg eth buffer
                                                          underflow */
#define BCM_CPRI_INTR_TX_GCW                _shrCpriIntrTxGcw /* RSVD4 control
                                                          multi-msg eth buffer
                                                          underflow */
#define BCM_CPRI_INTR_TX_PCS_GBOX_OVERFLOW  _shrCpriIntrTxPcsGboxOverflow /* gearbox buffer
                                                          overflow */
#define BCM_CPRI_INTR_TX_PCS_GBOX_UNDERFLOW _shrCpriIntrTxPcsGboxUnderflow /* gearbox buffer
                                                          underflow */
#define BCM_CPRI_INTR_TX_PCS_FIFO_OVERFLOW  _shrCpriIntrTxPcsFifoOverflow /* TX assembly FIFO
                                                          overflow */
#define BCM_CPRI_INTR_TX_PCS_FIFO_UNDERFLOW _shrCpriIntrTxPcsFifoUnderflow /* TX assembly FIFO
                                                          underflow */

/* CPRI port interrupt info */
typedef _shr_cpri_intr_info_t bcm_cpri_interrupt_info_t;

#define BCM_CPRI_INTR_DATA_NUM_OF_DATA64    _shrIntrDataNumOfData64 

/* CPRI interrupt data, ths is sent to the user registered callback */
typedef bcm_cpri_interrupt_info_t bcm_cpri_interrupt_data_t;

/* 
 * Callback function used for receiving notification about CPRI port RX,
 * TX, 1588 interrupts.
 */
typedef void (*bcm_cpri_port_interrupt_callback_t)(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_interrupt_data_t *data, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register/Unregsiter callback function for CPRI port interrupts. */
extern int bcm_cpri_port_interrupt_callback_register(
    int unit, 
    bcm_cpri_interrupt_type_t cpri_intr_type, 
    bcm_cpri_port_interrupt_callback_t callback, 
    void *user_data);

/* Register/Unregsiter callback function for CPRI port interrupts. */
extern int bcm_cpri_port_interrupt_callback_unregister(
    int unit, 
    bcm_cpri_interrupt_type_t cpri_intr_type, 
    bcm_cpri_port_interrupt_callback_t callback);

/* Set Enable/Disable CPRI port interrupts base on type and index.. */
extern int bcm_cpri_port_interrupt_enable_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_interrupt_type_t cpri_intr_type, 
    int data, 
    int enable);

/* Get Enable/Disable  CPRI port interrupts base on type and index. */
extern int bcm_cpri_port_interrupt_enable_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_interrupt_type_t cpri_intr_type, 
    int data, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri direction info */
typedef _shr_cpri_dir_t bcm_cpri_dir_t;

/* CPRI directions flags */
#define BCM_CPRI_DIR_TX         _shrCpriDirTx 
#define BCM_CPRI_DIR_RX         _shrCpriDirRx 

/* Cpri capture mode info */
typedef _shr_cpri_1588_capture_mode_t bcm_cpri_1588_capture_mode_t;

/* CPRI port 1588 timestamp capture modes */
#define BCM_CPRI_1588_MATCH_BFN_AND_HFN _shrCpri1588CaptureModeMatchBfnHfn /*  */
#define BCM_CPRI_1588_MATCH_HFN_ONLY    _shrCpri1588CaptureModeMatchHfnOnly /*  */

/* CPRI 1588 Timestamp capture config information. */
typedef struct bcm_cpri_1588_capture_config_s {
    bcm_cpri_1588_capture_mode_t mode;  /* Timestamp Capture type */
    uint32 capture_bfn_num;             /* if RSVD4, upper 12 bits of msg grp
                                           count */
    uint32 capture_hfn_num;             /* if RSVD4, lower 8 bits of msg grp
                                           count */
} bcm_cpri_1588_capture_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure  port 1588 timestamp capture once. */
extern int bcm_cpri_port_1588_timestamp_capture_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_capture_config_t *config);

/* Configure  port 1588 timestamp capture once. */
extern int bcm_cpri_port_1588_timestamp_capture_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_capture_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI 1588 Timestamp config information. */
typedef struct bcm_cpri_1588_config_s {
    uint64 nano_sec;    /* 48 bits only */
    uint8 sub_nano_sec; /* 4 bits only */
    uint8 sign_bit;     /* 1 bit only */
} bcm_cpri_1588_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure CPRI port 1588 timestamp adjust value. */
extern int bcm_cpri_port_1588_timestamp_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_config_t *config);

/* Configure CPRI port 1588 timestamp adjust value. */
extern int bcm_cpri_port_1588_timestamp_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri timestamp capture type info */
typedef _shr_cpri_1588_ts_type_t bcm_cpri_1588_ts_type_t;

/* CPRI port 1588 timestamp capture types */
#define BCM_CPRI_1588_TS_TYPE_FIFO  _shrCpri1588TsTypeFifo /*  */
#define BCM_CPRI_1588_TS_TYPE_CMIC  _shrCpri1588TsTypeCMIC /*  */
#define BCM_CPRI_1588_TS_TYPE_SW    _shrCpri1588TsTypeSw /*  */

/* Cpri timestamp ts config info */
typedef struct bcm_cpri_1588_timestamp_ts_config_s {
    int enable;             /* enable/disable */
    uint32 modulo_count;    /* modulo count */
} bcm_cpri_1588_timestamp_ts_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure CPRI port 1588 timestamp for repeating timestamp capture. */
extern int bcm_cpri_port_1588_timestamp_ts_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_ts_type_t type, 
    bcm_cpri_1588_timestamp_ts_config_t *config);

/* Configure CPRI port 1588 timestamp for repeating timestamp capture. */
extern int bcm_cpri_port_1588_timestamp_ts_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_ts_type_t type, 
    bcm_cpri_1588_timestamp_ts_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri timestamp value */
typedef uint64 bcm_cpri_1588_timestamp_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get CPRI port 1588 captured timestamp. */
extern int bcm_cpri_port_1588_timestamp_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    bcm_cpri_1588_ts_type_t type, 
    int max_count, 
    int *count, 
    bcm_cpri_1588_timestamp_t *ts_list);

#endif /* BCM_HIDE_DISPATCHABLE */

/* multiples of basic frame length */
typedef _shr_cpri_roe_payload_multiple_t bcm_cpri_roe_payload_multiple_t;

#define BCM_CPRI_PAYLOAD_MULTIPLE_BASIC_FRAME_1X _shrCpriPayloadMultiple1X 
#define BCM_CPRI_PAYLOAD_MULTIPLE_BASIC_FRAME_1P5X _shrCpriPayloadMultiple1p5X 
#define BCM_CPRI_PAYLOAD_MULTIPLE_BASIC_FRAME_2X _shrCpriPayloadMultiple2X 
#define BCM_CPRI_PAYLOAD_MULTIPLE_BASIC_FRAME_4X _shrCpriPayloadMultiple4X 
#define BCM_CPRI_PAYLOAD_MULTIPLE_BASIC_FRAME_8X _shrCpriPayloadMultiple8X 

/* CPRI Order Info Type .orderInfoType. */
typedef _shr_cpri_roe_ordering_info_option_t bcm_cpri_ordering_info_option_t;

/* CPRI Order Info Type .orderInfoType */
#define BCM_CPRI_ORDERING_INFO_OPTION_SEQ_NUM _shrCpriRoeOrderingInfoOptionSeqNum /*  Sequnece number in
                                                          ordering info. */
#define BCM_CPRI_ORDERING_INFO_OPTION_TIMESTAMP _shrCpriRoeOrderingInfoOptionTimestamp /*  Timestamp in ordering
                                                          info */

/* CPRI agnostic configuration */
typedef struct bcm_cpri_port_agnostic_config_s {
    bcm_cpri_frame_sync_mode_t frame_sync_mode; /* Radio, Hyper or Basic */
    uint32 basic_frame_offset;          /* Basic frame offset */
    uint32 radio_frame_offset;          /* Radio frame offset */
    uint32 hyper_frame_offset;          /* Hyper frame offset */
    bcm_cpri_roe_payload_multiple_t payload_multiple; /* ROE payload size */
    uint32 presentation_mod_count;      /* timestamp filtering */
    uint32 presentation_mod_offset;     /* tx only */
    uint32 tx_queue_mod_cnt;            /* jitter buffer cycle size tx only */
    bcm_cpri_ordering_info_option_t ordering_info_option; /* Ordering info option. */
} bcm_cpri_port_agnostic_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API to set/get the agnostic mode and its configuration. */
extern int bcm_cpri_port_agnostic_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t dir, 
    int enable, 
    bcm_cpri_port_agnostic_config_t *config);

/* API to set/get the agnostic mode and its configuration. */
extern int bcm_cpri_port_agnostic_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t dir, 
    int *enable, 
    bcm_cpri_port_agnostic_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri encapsulation presentation timestamp attribute */
typedef _shr_cpri_presentation_ts_attribute_t bcm_cpri_presentation_ts_attribute_t;

#define BCM_CPRI_ENCAP_PRESENTATION_TS_ADJUST _shrCpriEncapPresentationTsAdjust 
#define BCM_CPRI_ENCAP_PRESENTATION_APPROX_INC _shrCpriEncapPresentationApproxInc 

/* Cpri presentation time */
typedef _shr_cpri_presentation_time_t bcm_cpri_presentation_time_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API to set/get the presentation attribute. */
extern int bcm_cpri_encap_presentation_time_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_presentation_ts_attribute_t attribute, 
    bcm_cpri_presentation_time_t *time);

/* API to set/get the presentation attribute. */
extern int bcm_cpri_encap_presentation_time_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_presentation_ts_attribute_t attribute, 
    bcm_cpri_presentation_time_t *time_offset);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri decap presentation config */
typedef _shr_cpri_decap_presentation_config_t bcm_cpri_decap_presentation_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API to set/get the decap presentation time. */
extern int bcm_cpri_decap_presentation_time_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_decap_presentation_config_t *config);

/* API to set/get the decap presentation time. */
extern int bcm_cpri_decap_presentation_time_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_decap_presentation_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Port stats */
typedef enum bcm_cpri_stat_type_e {
    bcmCpriStatRxSymbolErr = 0,         /*  Rx Bytes Symbol Error Count. */
    bcmCpriStatRxSeedMismatch = 1,      /*  Rx Seed Mismatch Count. */
    bcmCpriStatRxStateErrTransition = 2, /*  Rx number of transuition into Err
                                           state. */
    bcmCpriStatTxVsdCtrlFlowIdErr = 3,  /*  Tx Vsd Control Flow ID out of range
                                           (0..127) error Count. */
    bcmCpriStatTxVsdCtrlPktSizeErr = 4, /*  Tx Vsd Control Packet size error
                                           count. >16 bytes. */
    bcmCpriStatTxVsdRawPktSizeErr = 5,  /*  Tx Vsd Raw Packet size error count. */
    bcmCpriStatTxRsvd5CrcErr = 6,       /*  Tx RSVD5 Dropped Packet Count due to
                                           CRC error. */
    bcmCpriStatRxRsvd4SingleMsgCrcErr = 7, /* Rx RSVD4 Control Single Message
                                           Dropped Packet Count due to CRC
                                           error. */
    bcmCpriStatRxTagLookupFailed = 8,   /*  Rx Tag Look up failed Count. */
    bcmCpriStatRxRsvd4SingleMsgCnt = 9, /* Rx RSVD4 Single Message Count . */
    bcmCpriStatRxRsvd4MultiMsgCnt = 10, /*  Rx RSVD4 Multi Message Count. */
    bcmCpriStatRxRsvd4MultiMsgSeqErr = 11, /* Rx RSVD4 Multi Message Sequence Error
                                           Count. */
    bcmCpriStatRxDataByteErr = 12,      /* Rx Data Bytes in Error. */
    bcmCpriStatRxRsvd4HeaderErr = 13,   /* Rx RSVD4 Dropped Messages Due to
                                           Header Error. */
    bcmCpriStatRxRsvd4HeaderNoMatch = 14, /*  Rx RSVD4 Messags Header No Match
                                           Found. */
    bcmCpriStatRxRsvd4MsgCnt = 15,      /* Rx RSVD4 Total Message Count. */
    bcmCpriStatRxHfnErr = 16,           /*  Rx incorrect Hyper frame
                                           number(HFN). */
    bcmCpriStatRxBfnErr = 17,           /*  Rx incorrect Radio frame
                                           number(BFN). */
    bcmCpriStatRxCtrlWordErr = 18,      /*  Rx Control Word Error. */
    bcmCpriStatRxGcwErr = 19,           /* Rx GCW Dropped Words due to Byte
                                           Error. */
    bcmCpriStatRxVsdRawErr = 20,        /*  Rx VSD Raw Dropped packets Due to
                                           byte error. */
    bcmCpriStatRxVsdCtrlErr = 21,       /*  Rx VSD Control Dropped packets Due
                                           to byte error. */
    bcmCpriStatTxRsvd4DataMsgCnt = 22,  /*  Tx Rsvd4 Data Message Count. */
    bcmCpriStatTxRsvd4DataEmptyMsgCnt = 23, /*  Tx Rsvd4 Data Empty Message Count. */
    bcmCpriStatTxRsvd4CtrlMsgCnt = 24,  /*  Tx Rsvd4 Control Message Count. */
    bcmCpriStatTxRsvd4CtrlEmptyMsgCnt = 25, /*  Tx Rsvd4 Control Empty Message
                                           Count. */
    bcmCpriStatRxRsvd5ParityErr = 26,   /* Rx RSVD5 Parity Errored Message
                                           Count. */
    bcmCpriStatRxHdlcErrFrame = 27,     /* Rx HDLC Errored frame Count. */
    bcmCpriStatRxHdlcGoodFrame = 28,    /* Rx HDLC Good Frame Count. */
    bcmCpriStatRxFastEthDroppedPkt = 29, /*  Rx Fast Ethernet Dropped Packet. */
    bcmCpriStatRxFastEthGoodPkt = 30,   /*  Rx Fast Ethernet Good Packet. */
    bcmCpriStatMax = 31                 /*  Last One. */
} bcm_cpri_stat_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* API to retrieve the CPRI port stats. */
extern int bcm_cpri_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_stat_type_t type, 
    uint64 *val);

/* Enable/Disable FEC. */
extern int bcm_cpri_port_fec_enable_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_dir_t dir, 
    int enable);

/* Enable/Disable FEC. */
extern int bcm_cpri_port_fec_enable_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_dir_t dir, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri FEC timestamp source */
typedef _shr_cpri_fec_ts_source_t bcm_cpri_fec_ts_source_t;

#define BCM_CPRI_FEC_TS_SOURCE_66b  _shrCpriFecTsSource66b 
#define BCM_CPRI_FEC_TS_SOURCE_257b _shrCpriFecTsSource257b 

/* Cpri FEC timestamp config info */
typedef struct bcm_cpri_fec_timestamp_config_s {
    int enable;                         /* Enable/Disable. */
    bcm_cpri_fec_ts_source_t fec_fifo_ts_source_select; /* Source 66 bit  or 257 bit. */
    bcm_cpri_fec_ts_source_t fec_cmic_ts_source_select; /* Source 66 bit  or 257 bit. */
    uint32 control_bit_offset_0;        /* Control Bit Offset. */
    uint32 control_bit_offset_1;        /* Control Bit Offset. */
    uint32 control_bit_offset_2;        /* Control Bit Offset. */
    uint32 control_bit_offset_3;        /* Control Bit Offset. */
} bcm_cpri_fec_timestamp_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Setup FEC related timestamp configuration. */
extern int bcm_cpri_port_fec_timestamp_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_dir_t dir, 
    bcm_cpri_fec_timestamp_config_t *config);

/* Setup FEC related timestamp configuration. */
extern int bcm_cpri_port_fec_timestamp_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_dir_t dir, 
    bcm_cpri_fec_timestamp_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri FEC aux config para */
typedef _shr_cpri_fec_aux_config_t bcm_cpri_fec_aux_config_t;

#define BCM_CPRI_FEC_AUX_CONFIG_SCRAMBLE_BYPASS_TX _shrCpriFecAuxConfigScrambleBypassTx 
#define BCM_CPRI_FEC_AUX_CONFIG_DESCRAMBLE_BYPASS_RX _shrCpriFecAuxConfigDescrambleBypassRx 
#define BCM_CPRI_FEC_AUX_CONFIG_BYPASS_INDICATION_ENABLE _shrCpriFecAuxConfigBypassIndicationEnable 
#define BCM_CPRI_FEC_AUX_CONFIG_BYPASS_CORRECTION_ENABLE _shrCpriFecAuxConfigBypassCorrectionEnable 
#define BCM_CPRI_FEC_AUX_CONFIG_CORRUPT_ALTERNATE_BLOCKS_ENABLE _shrCpriFecAuxConfigCorruptAlternateBlocksEnable 
#define BCM_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_CORRUPT_ENABLE _shrCpriFecAuxConfigSymbolErrorCorruptEnable 
#define BCM_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_TMR_PERIOD _shrCpriFecAuxConfigSymbolErrorTmrPeriod 
#define BCM_CPRI_FEC_AUX_CONFIG_SYMBOL_ERR_CNT_THRESHOLD _shrCpriFecAuxConfigSymbolErrCntThreshold 
#define BCM_CPRI_FEC_AUX_CONFIG_SET_SYMB_ERR_WINDOW_128 _shrCpriFecAuxConfigSetSymbErrWindow128 
#define BCM_CPRI_FEC_AUX_CONFIG_GBOX_TICK_DENOMINATOR _shrCpriFecAuxConfigGboxTickDenominator 
#define BCM_CPRI_FEC_AUX_CONFIG_GBOX_TICK_NUMERATOR _shrCpriFecAuxConfigGboxTickNumerator 
#define BCM_CPRI_FEC_AUX_CONFIG_SOFT_RESET  _shrCpriFecAuxConfigSoftReset 

#ifndef BCM_HIDE_DISPATCHABLE

/* Setup Auxiliary configuration for FEC. */
extern int bcm_cpri_port_fec_aux_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_fec_aux_config_t param_id, 
    uint32 value);

/* Setup Auxiliary configuration for FEC. */
extern int bcm_cpri_port_fec_aux_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_fec_aux_config_t param_id, 
    uint32 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Cpri FEC Counter type. */
typedef _shr_cpri_fec_stat_type_t bcm_cpri_fec_stat_type_t;

#define BCM_CPRI_FEC_STAT_CORRECTED_CODEWORD _shrCpriFecStatTypeCorrectedCodeword 
#define BCM_CPRI_FEC_STAT_UNCORRECTED_CODEWORD _shrCpriFecStatTypeUncorrectedCodeword 
#define BCM_CPRI_FEC_STAT_BIT_ERROR         _shrCpriFecStatTypeBitError 
#define BCM_CPRI_FEC_STAT_SYMBOL_ERROR      _shrCpriFecStatTypeSymbolError 
#define BCM_CPRI_FEC_STAT_INVALID           _shrCpriFecStatTypeInvalid 

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the FEC Statistic Counter */
extern int bcm_cpri_port_fec_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_cpri_fec_stat_type_t type, 
    uint32 *counter_value);

/* Set and Program Compression Table. */
extern int bcm_cpri_port_compression_lookup_table_set(
    int unit, 
    bcm_gport_t port, 
    int table_depth, 
    uint32 *comp_table);

/* Set and Program Compression Table. */
extern int bcm_cpri_port_compression_lookup_table_get(
    int unit, 
    bcm_gport_t port, 
    int max_depth, 
    int *table_depth, 
    uint32 *comp_table);

/* Set and Program DeCompression Table. */
extern int bcm_cpri_port_decompression_lookup_table_set(
    int unit, 
    bcm_gport_t port, 
    int table_depth, 
    uint32 *comp_table);

/* Set and Program DeCompression Table. */
extern int bcm_cpri_port_decompression_lookup_table_get(
    int unit, 
    bcm_gport_t port, 
    int max_depth, 
    int *table_depth, 
    uint32 *comp_table);

/* Set Decapsulation Queue Flow Control configuration. */
extern int bcm_cpri_port_decap_queue_flow_control_set(
    int unit, 
    bcm_gport_t port, 
    uint32 queue_num, 
    uint32 xon_threshold, 
    uint32 xoff_threshold, 
    int enable);

/* Get Decapsulation Queue Flow Control configuration. */
extern int bcm_cpri_port_decap_queue_flow_control_get(
    int unit, 
    bcm_gport_t port, 
    uint32 queue_num, 
    uint32 *xon_threshold, 
    uint32 *xoff_threshold, 
    int *enable);

/* Get Rx GCW word. */
extern int bcm_cpri_port_rx_cw_gcw_word_get(
    int unit, 
    bcm_port_t port, 
    int group_index, 
    uint16 *gcw_word);

/* Set Tx GCW word. */
extern int bcm_cpri_port_tx_cw_gcw_word_set(
    int unit, 
    bcm_port_t port, 
    int group_index, 
    uint16 gcw_word);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Tunnel Mode Configuration. */
typedef struct bcm_cpri_port_tunnel_config_s {
    uint32 payload_size;                /* RoE payload size. In bytes. */
    bcm_cpri_ordering_info_option_t ordering_info_option; /* Ordering info option. */
    uint32 presentation_mod_count;      /* Mod Count for Presentation time. */
    uint32 presentation_mod_offset;     /* Mod Offset for Presentation time. */
    uint32 tx_queue_mod_count;          /* Jitter buffer cycle size, Tx Only. */
} bcm_cpri_port_tunnel_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set tunnel Mode configuration. */
extern int bcm_cpri_port_tunnel_mode_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    int enable, 
    bcm_cpri_port_tunnel_config_t *config);

/* Set tunnel Mode configuration. */
extern int bcm_cpri_port_tunnel_mode_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    int *enable, 
    bcm_cpri_port_tunnel_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI RSVD4 Agnostic Mode Configuration. */
typedef struct bcm_cpri_port_rsvd4_agnostic_config_s {
    bcm_cpri_ordering_info_option_t ordering_info_option; /* Ordering info option. */
    bcm_rsvd4_sync_mode_t sync_mode;    /*  Sync Mode */
    uint32 payload_multiple;            /* Multiple of Message group 1-3. */
    uint32 master_frame_offset;         /*  Master Frame Offset for Sync */
    uint32 message_group_offset;        /*  Message Group Offset for Sync */
    uint32 presentation_mod_count;      /* Mod Count for Presentation time. */
    uint32 presentation_mod_offset;     /* Mod Offset for Presentation time. */
    uint32 tx_queue_mod_count;          /* Jitter buffer cycle size, Tx Only. */
} bcm_cpri_port_rsvd4_agnostic_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set RSVD4 Agnostic  Mode configuration. */
extern int bcm_cpri_port_rsvd4_agnostic_mode_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    int enable, 
    bcm_cpri_port_rsvd4_agnostic_config_t *config);

/* Set RSVD4 Agnostic  Mode configuration. */
extern int bcm_cpri_port_rsvd4_agnostic_mode_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_dir_t direction, 
    int *enable, 
    bcm_cpri_port_rsvd4_agnostic_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI RoE Header Field, */
typedef _shr_cpri_roe_header_field_t bcm_cpri_roe_header_field_t;

#define BCM_CPRI_ROE_HEADER_FIELD_SUBTYPE   _shrCpriRoeHeaderFieldSubType 
#define BCM_CPRI_ROE_HEADER_FIELD_FLOW_ID   _shrCpriRoeHeaderFieldFlowID 
#define BCM_CPRI_ROE_HEADER_FIELD_LENGTH_15_8 _shrCpriRoeHeaderFieldLength_15_8 
#define BCM_CPRI_ROE_HEADER_FIELD_LENGTH_7_0 _shrCpriRoeHeaderFieldLength_7_0 
#define BCM_CPRI_ROE_HEADER_FIELD_ORDER_INFO_32_34 _shrCpriRoeHeaderFieldOrderInfo_32_24 
#define BCM_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16 _shrCpriRoeHeaderFieldOrderInfo_23_16 
#define BCM_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8 _shrCpriRoeHeaderFieldOrderInfo_15_8 
#define BCM_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0 _shrCpriRoeHeaderFieldOrderInfo_7_0 
#define BCM_CPRI_ROE_HEADER_FIELD_OPCODE    _shrCpriRoeHeaderFieldOpcode 
#define BCM_CPRI_ROE_HEADER_FIELD_UNMAPPED  _shrCpriRoeHeaderFieldUnmapped 

/* CPRI eCPRI Header Field, */
typedef _shr_cpri_ecpri_header_field_t bcm_cpri_ecpri_header_field_t;

#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_0  _shrCpriEcpriHeaderFieldByte0 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_1  _shrCpriEcpriHeaderFieldByte1 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_2  _shrCpriEcpriHeaderFieldByte2 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_3  _shrCpriEcpriHeaderFieldByte3 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_4  _shrCpriEcpriHeaderFieldByte4 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_5  _shrCpriEcpriHeaderFieldByte5 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_6  _shrCpriEcpriHeaderFieldByte6 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_7  _shrCpriEcpriHeaderFieldByte7 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_8  _shrCpriEcpriHeaderFieldByte8 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_9  _shrCpriEcpriHeaderFieldByte9 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_10 _shrCpriEcpriHeaderFieldByte10 
#define BCM_CPRI_ECPRI_HEADER_FIELD_BYTE_11 _shrCpriEcpriHeaderFieldByte11 
#define BCM_CPRI_ECPRI_HEADER_FIELD_VLAN_0_BYTE_0 _shrCpriEcpriHeaderFieldVlan0Byte0 
#define BCM_CPRI_ECPRI_HEADER_FIELD_VLAN_0_BYTE_1 _shrCpriEcpriHeaderFieldVlan0Byte1 
#define BCM_CPRI_ECPRI_HEADER_FIELD_VLAN_1_BYTE_0 _shrCpriEcpriHeaderFieldVlan1Byte0 
#define BCM_CPRI_ECPRI_HEADER_FIELD_VLAN_1_BYTE_1 _shrCpriEcpriHeaderFieldVlan1Byte1 

/* CPRI RoE Frame Configuration. */
typedef struct bcm_cpri_port_frame_config_s {
    bcm_cpri_roe_frame_format_t port_frame_format; /* Port Frame Format. */
    bcm_cpri_ecpri_header_field_t decap_primary_key; /* Decap Primary Key */
    bcm_cpri_ecpri_header_field_t decap_secondary_key; /* Decap Secondary Key. */
} bcm_cpri_port_frame_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set RoE frame format for the port. */
extern int bcm_cpri_port_frame_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_port_frame_config_t *config);

/* Configure decap secondary key to queue mapping. */
extern int bcm_cpri_port_decap_secondary_key_to_queue_mapping_set(
    int unit, 
    bcm_port_t port, 
    uint32 secondary_key, 
    uint32 queue_num);

/* Configure decap secondary key to queue mapping. */
extern int bcm_cpri_port_decap_secondary_key_to_queue_mapping_get(
    int unit, 
    bcm_port_t port, 
    uint32 secondary_key, 
    uint32 *queue_num);

/* Set Compression Saturation Threshold Value. */
extern int bcm_cpri_port_compression_saturation_config_set(
    int unit, 
    bcm_port_t port, 
    uint32 threshold_value);

/* Set Compression Saturation Threshold Value. */
extern int bcm_cpri_port_compression_saturation_config_get(
    int unit, 
    bcm_port_t port, 
    uint32 *threshold_value);

/* Set/Get  DeCompression Saturation Value. */
extern int bcm_cpri_port_decompression_saturation_config_set(
    int unit, 
    bcm_port_t port, 
    uint32 sat_value);

/* Set/Get  DeCompression Saturation Value. */
extern int bcm_cpri_port_decompression_saturation_config_get(
    int unit, 
    bcm_port_t port, 
    uint32 *sat_value);

/* Clear Rx Data Path in Framer and Encapsulation. */
extern int bcm_cpri_port_rx_pipeline_clear(
    int unit, 
    bcm_gport_t port);

/* Clear Tx Data Path in Framer and Decapsulation. */
extern int bcm_cpri_port_tx_pipeline_clear(
    int unit, 
    bcm_gport_t port);

/* Get tx enable status */
extern int bcm_cpri_port_tx_enable_get(
    int unit, 
    bcm_gport_t port, 
    int *enable);

/* Set tx enable */
extern int bcm_cpri_port_tx_enable_set(
    int unit, 
    bcm_gport_t port, 
    int enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* CPRI Speed Scan Port Information. */
typedef struct bcm_cpri_speed_scan_port_info_s {
    int link_status;                /* Link Status. */
    bcm_cpri_port_speed_t speed;    /* CPRI port speed. */
} bcm_cpri_speed_scan_port_info_t;

/* Callback function used for speed scan to notify link status change.. */
typedef void (*bcm_cpri_speed_scan_handler_t)(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_speed_scan_port_info_t *port_info, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the CPRI Speed Scan. */
extern int bcm_cpri_speed_scan_init(
    int unit);

/* CPRI Speed Scan unit level configuration. */
extern int bcm_cpri_speed_scan_config_set(
    int unit, 
    int interval_in_us, 
    int enable);

/* CPRI Speed Scan unit level configuration. */
extern int bcm_cpri_speed_scan_config_get(
    int unit, 
    int *interval_in_us, 
    int *enable);

/* CPRI Adding port to speed scan list. */
extern int bcm_cpri_speed_scan_port_config_add(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_port_speed_t *speed_list, 
    int num_speed);

/* CPRI Delete port from speed scan list. */
extern int bcm_cpri_speed_scan_port_config_delete(
    int unit, 
    bcm_port_t port);

/* CPRI Speed Scan Port configuration get. */
extern int bcm_cpri_speed_scan_port_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_cpri_port_speed_t *speed_list, 
    int num_element, 
    int *num_speed);

/* Register handler function for CPRI speed scanning. */
extern int bcm_cpri_speed_scan_callback_register(
    int unit, 
    bcm_cpri_speed_scan_handler_t callback, 
    void *user_data);

/* UnRegister handler function for CPRI speed scanning. */
extern int bcm_cpri_speed_scan_callback_unregister(
    int unit, 
    bcm_cpri_speed_scan_handler_t callback);

#endif /* BCM_HIDE_DISPATCHABLE */

/* RSVD1 msg information */
typedef struct bcm_cpri_rsvd1_msg_s {
    uint32 rsvd1_msg0;  /* msg0 */
    uint32 rsvd1_msg1;  /* msg1 */
    uint32 rsvd1_msg2;  /* msg2 */
} bcm_cpri_rsvd1_msg_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Function to retrieve rsvd1 message. */
extern int bcm_cpri_rsvd1_msg_get(
    int unit, 
    bcm_cpri_rsvd1_msg_t *rsvd1_msg);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_CPRI_H__ */
