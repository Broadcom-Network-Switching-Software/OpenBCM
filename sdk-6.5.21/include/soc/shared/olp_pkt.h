/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        olp_pkt.h
 * Purpose:     olp Packet Format definitions
 *              common to SDK and uKernel.
 */

#ifndef   _SOC_SHARED_OLP_PKT_H_
#define   _SOC_SHARED_OLP_PKT_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif
#include <shared/pack.h>

/*************************************
 * OLP Packet Format
 */

/*
 * OLP TX HDR
 */

typedef struct soc_olp_tx_hdr {

#ifdef LE_HOST
    union {
        uint32 h1;
        struct {
            uint32 mod_id:8,
                   int_pri:4,
                   hdr_subtype:8,
                   hdr_type:8,
                   ver:4;
        }s1;
    }u1;
    union {
        uint32 h2;
        struct {
            uint32 _ctr2_id1:5,
                   ctr2_location:1,
                   ctr1_action:2,
                   ctr1_id:15,
                   ctr1_location:1,
                   port_id:8;
        }s2;
    }u2;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined (BCM_SABER2_SUPPORT)
    /* There are differences between OLP header formats used in TD2+ and SB2.
     * sX1 (where X is 3 or 4) are overlay structures used for TD2+
     * sX2 (where X is 3 or 4) are overlay structures used for SB2.
     */
    union {
        uint32 h3;
        struct {
            uint32 _class_id1:10,
                   oam_replacement_offset:8,
                   timestamp_action:2,
                   ctr2_action:2,
                   _ctr2_id2:10;
        }s31;
        struct {
            uint32 _ctr3_id1:4,
                   ctr3_location:1,
                   lm_replacement_off_oper:1,
                   rel_lm_replacement_off:4,
                   oam_replacement_offset:8,
                   timestamp_action:2,
                   ctr2_action:2,
                   _ctr2_id2:10;
        }s32; /* Overlay that is used for Saber2 */
    }u3;
    union {
        uint32 h4;
        struct {
            uint32 _rsvd1:26,
                   _class_id2:6;
        }s41;
        struct {
            uint32 
                _rsvd1:1,
                ctr3_off_to_lm:6,
                ctr2_off_to_lm:6,
                ctr1_off_to_lm:6,
                ctr3_action:2,
                _ctr3_id2:11;
        }s42; /* Overlay that is used for Saber2 */
    }u4;
#else
    union {
        uint32 h3; 
    }u3;
    union {
        uint32 h4; 
    }u4;
#endif   /* !(Td2+ or Sb2) */

#else  /* !LE_HOST*/
    union {
        uint32 h1;
        struct {
            uint32 ver:4,
                   hdr_type:8,
                   hdr_subtype:8,
                   int_pri:4,
                   mod_id:8;
        }s1;
    }u1;
    union {
        uint32 h2;
        struct {
            uint32 port_id:8,
                   ctr1_location:1,
                   ctr1_id:15,
                   ctr1_action:2,
                   ctr2_location:1,
                   _ctr2_id1:5;
        }s2;
    }u2;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined (BCM_SABER2_SUPPORT) 
    /* There are differences between OLP header formats used in TD2+ and SB2.
     * sX1 (where X is 3 or 4) are overlay structures used for TD2+
     * sX2 (where X is 3 or 4) are overlay structures used for SB2.
     */
    union {
        uint32 h3;
        struct {
            uint32 _ctr2_id2:10,
                   ctr2_action:2,
                   timestamp_action:2,
                   oam_replacement_offset:8,
                   _class_id1:10;
        }s31;
        struct {
            uint32 
                _ctr2_id2:10,
                ctr2_action:2,
                timestamp_action:2,
                oam_replacement_offset:8,
                rel_lm_replacement_off:4,
                lm_replacement_off_oper:1,
                ctr3_location:1,
                _ctr3_id1:4;
        }s32; /* Overlay that is used for Saber2 */
    }u3;
    union {
        uint32 h4;
        struct {
            uint32 _class_id2:6,
                   :26;
        }s41;
        struct {
            uint32 
                _ctr3_id2:11,
                ctr3_action:2,
                ctr1_off_to_lm:6,
                ctr2_off_to_lm:6,
                ctr3_off_to_lm:6,
                _rsvd1:1;
        }s42; /* Overlay that is used for Saber2 */
    }u4;
#else
    union {
        uint32 h3; 
    }u3;
    union {
        uint32 h4; 
    }u4;
#endif /* !(Td2+ or Sb2) */
#endif  /* LE_HOST */
 
} soc_olp_tx_hdr_t;

/*
 * OLP RX HDR
 */

typedef struct soc_olp_rx_hdr {

#ifdef LE_HOST
    union {
        uint32 h1;
        struct {
            uint32 _port_id1:4,
                   mod_id:8,
                   hdr_subtype:8,
                   hdr_type:8,
                   ver:4;
        }s1;
    }u1;
    union {
        uint32 h2;
        struct {
            uint32 _rx_timestamp_upper1:10,
                   sample_type:2,
                   session_id:16,
                   _port_id2:4;
        }s2;
    }u2;
    union {
        uint32 h3;
        struct {
            uint32 _rx_timestamp1:10,
                   _rx_timestamp_upper2:22;
        }s3;
    }u3;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined (BCM_SABER2_SUPPORT)
    /* There are differences between OLP header formats used in TD2+ and SB2.
     * s41 is overlay structure used for TD2+
     * s42 is overlay structure used for SB2.
     * s5 is overlay structure used for SB2 only.
     */
    union {
        uint32 h4;
        struct {
            uint32 
                :2,
                oam_pdu:8,
                _rx_timestamp2:22;
        }s41;
        struct {
            uint32 _sample_value_ext1:1,
                   sample_type_ext:1,
                   oam_pdu:8,
                   _rx_timestamp2:22;
        }s42;/* Overlay that is used for Saber2 */
    }u4;
    union {
        uint32 h5;
        struct {
            uint32 
                    :1,
                   _sample_value_ext2:31;
        }s5; /* Overlay that is used only for Saber2 */
    }u5;
#else
    union {
        uint32 h4; 
    }u4;
    union {
        uint32 h5; 
    }u5;

#endif /* !(Td2+ or sb2) */

#else /* ! LE_HOST*/

    union {
        uint32 h1;
        struct {
            uint32 ver:4,
                   hdr_type:8,
                   hdr_subtype:8,
                   mod_id:8,
                   _port_id1:4;
        }s1;
    }u1;
    union {
        uint32 h2;
        struct {
            uint32 _port_id2:4,
                   session_id:16,
                   sample_type:2,
                   _rx_timestamp_upper1:10;
        }s2;
    }u2;
    union {
        uint32 h3;
        struct {
            uint32 _rx_timestamp_upper2:22,
                   _rx_timestamp1:10;
        }s3;
    }u3;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined (BCM_SABER2_SUPPORT)
    /* There are differences between OLP header formats used in TD2+ and SB2.
     * s41 is overlay structure used for TD2+
     * s42 is overlay structure used for SB2.
     * s5 is overlay structure used for SB2 only.
     */
    union {
        uint32 h4;
        struct {
            uint32 _rx_timestamp2:22,
                   oam_pdu:8,
                   :2;
        }s41;
        struct {
            uint32 _rx_timestamp2:22,
                   oam_pdu:8,
                   sample_type_ext:1,
                   _sample_value_ext1:1;
        }s42; /* Overlay For saber2 */
    }u4;
    union {
        uint32 h5;
        struct {
            uint32 _sample_value_ext2:31,
                   :1;
        }s5;/* overlay used only for Saber2 */
    }u5;
#else
    union {
        uint32 h4;
    }u4;
    union {
        uint32 h5;
    }u5;
#endif  /* !(Td2+ or Sb2) */

#endif /*LE HOST*/

} soc_olp_rx_hdr_t;

typedef struct __attribute__ ((__packed__)) soc_olp_l2_hdr_s {
    uint8 dst_mac[6];
    uint8 src_mac[6];
    uint16 tpid;
    uint16 vlan;
    uint16 etherType;
} soc_olp_l2_hdr_t;

typedef struct __attribute__ ((__packed__)) soc_olp_rx_pkt_s {
    soc_olp_l2_hdr_t l2_hdr; 
    soc_olp_rx_hdr_t rx_hdr;
} soc_olp_rx_pkt_t;


typedef struct __attribute__ ((__packed__)) soc_olp_tx_pkt_s {
    soc_olp_l2_hdr_t l2_hdr;
    soc_olp_tx_hdr_t tx_hdr;
} soc_olp_tx_pkt_t;

/*******************************************
 * Extraction Macros
 */

#define SOC_OLP_RX_VER_GET(_orh)     \
    ((_orh)->u1.s1.ver )
#define SOC_OLP_RX_HDR_TYPE_GET(_orh)     \
    ((_orh)->u1.s1.hdr_type )
#define SOC_OLP_RX_HDR_STYPE_GET(_orh)     \
    ((_orh)->u1.s1.hdr_subtype )
#define SOC_OLP_RX_MODID_GET(_orh)     \
    ((_orh)->u1.s1.mod_id )
#define SOC_OLP_RX_PORT_GET(_orh)     \
    (((_orh)->u1.s1._port_id1 << 4) | ((_orh)->u2.s2._port_id2))
#define SOC_OLP_RX_SESSION_ID_GET(_orh)     \
    ((_orh)->u2.s2.session_id )
#define SOC_OLP_RX_SAMPLE_TYPE_GET(_orh)     \
    ((_orh)->u2.s2.sample_type )
#define SOC_OLP_RX_TIMESTAMP_UPPER_GET(_orh)     \
    (((_orh)->u2.s2._rx_timestamp_upper1 << 22) | ((_orh)->u3.s3._rx_timestamp_upper2 ))
#define SOC_OLP_RX_TIMESTAMP_GET(_orh)     \
    (((_orh)->u3.s3._rx_timestamp1 << 22) | ((_orh)->u4.s41._rx_timestamp2 ))
#define SOC_OLP_RX_OAM_PDU_OFFSET_GET(_orh)     \
    ((_orh)->u4.s41.oam_pdu )

#ifdef BCM_SABER2_SUPPORT
#define SOC_OLP_RX_OAM_SAMPLE_TYPE_EXT_GET(_orh)     \
    ((_orh)->u4.s42.sample_type_ext )
#define SOC_OLP_RX_SAMPLE_VALUE_EXT_GET(_orh)     \
    (((_orh)->u4.s42._sample_value_ext1 << 31) | ((_orh)->u5.s5._sample_value_ext2 ))

#endif /* BCM_SABER2_SUPPORT */

#define SOC_OLP_TX_VER_GET(_oth)     \
    ((_oth)->u1.s1.ver )
#define SOC_OLP_TX_HDR_TYPE_GET(_oth)     \
    ((_oth)->u1.s1.hdr_type )
#define SOC_OLP_TX_HDR_STYPE_GET(_oth)     \
    ((_oth)->u1.s1.hdr_subtype )
#define SOC_OLP_TX_PRI_GET(_oth)     \
    ((_oth)->u1.s1.int_pri )
#define SOC_OLP_TX_MODID_GET(_oth)     \
    ((_oth)->u1.s1.mod_id )
#define SOC_OLP_TX_PORT_GET(_oth)     \
    ((_oth)->u2.s2.port_id)
#define SOC_OLP_TX_CTR1_LOCATION_GET(_oth)     \
    ((_oth)->u2.s2.ctr1_location )
#define SOC_OLP_TX_CTR1_ID_GET(_oth)     \
    ((_oth)->u2.s2.ctr1_id )
#define SOC_OLP_TX_CTR1_ACTION_GET(_oth)     \
    ((_oth)->u2.s2.ctr1_action )
#define SOC_OLP_TX_CTR2_LOCATION_GET(_oth)     \
    ((_oth)->u2.s2.ctr2_location )
#define SOC_OLP_TX_CTR2_ID_GET(_oth)     \
    (((_oth)->u2.s2._ctr2_id1 << 10) | ((_oth)->u3.s31._ctr2_id2))
#define SOC_OLP_TX_CTR2_ACTION_GET(_oth)     \
    ((_oth)->u3.s31.ctr2_action )
#define SOC_OLP_TX_TIMESTAMP_ACTION_GET(_oth)     \
    ((_oth)->u3.s31.timestamp_action )
#define SOC_OLP_TX_OAM_OFFSET_GET(_oth)     \
    ((_oth)->u3.s31.oam_replacement_offset )

#define SOC_OLP_TX_VER(_oth)                SOC_OLP_TX_VER_GET(_oth) 
#define SOC_OLP_TX_HDR_TYPE(_oth)           SOC_OLP_TX_HDR_TYPE_GET(_oth) 
#define SOC_OLP_TX_HDR_STYPE(_oth)          SOC_OLP_TX_HDR_STYPE_GET(_oth) 
#define SOC_OLP_TX_PRI(_oth)                SOC_OLP_TX_PRI_GET(_oth)
#define SOC_OLP_TX_MODID(_oth)              SOC_OLP_TX_MODID_GET(_oth) 
#define SOC_OLP_TX_PORT(_oth)               SOC_OLP_TX_PORT_GET(_oth)
#define SOC_OLP_TX_CTR1_LOCATION(_oth)      SOC_OLP_TX_CTR1_LOCATION_GET(_oth)   
#define SOC_OLP_TX_CTR1_ID(_oth)            SOC_OLP_TX_CTR1_ID_GET(_oth)
#define SOC_OLP_TX_CTR1_ACTION(_oth)        SOC_OLP_TX_CTR1_ACTION_GET(_oth) 
#define SOC_OLP_TX_CTR2_LOCATION(_oth)      SOC_OLP_TX_CTR2_LOCATION_GET(_oth)   
#define SOC_OLP_TX_CTR2_ACTION(_oth)        SOC_OLP_TX_CTR2_ACTION_GET(_oth) 
#define SOC_OLP_TX_TIMESTAMP_ACTION(_oth)   SOC_OLP_TX_TIMESTAMP_ACTION_GET(_oth)  
#define SOC_OLP_TX_OAM_OFFSET(_oth)         SOC_OLP_TX_OAM_OFFSET_GET(_oth)

#define SOC_OLP_TX_CTR2_ID_SET(_oth,val)    {           \
    (_oth)->u2.s2._ctr2_id1 = ((val) & 0x7C00) >> 10;   \
    (_oth)->u3.s31._ctr2_id2 = (val) & (0x3ff);        \
    }

#ifdef BCM_TRIDENT2PLUS_SUPPORT     
#define SOC_OLP_TX_CLASSID_GET(_oth)     \
    (((_oth)->u3.s31._class_id1 << 6) | ((_oth)->u4.s41._class_id2))

#define SOC_OLP_TX_CLASSID(_oth)    SOC_OLP_TX_CLASSID_GET(_oth) 
#endif

#if defined(BCM_SABER2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
#define SOC_OLP_TX_REL_LM_REP_OFF_GET(_oth)     \
    ((_oth)->u3.s32.rel_lm_replacement_off )
#define SOC_OLP_TX_REL_LM_REP_OP_GET(_oth)     \
    ((_oth)->u3.s32.lm_replacement_off_oper )
#define SOC_OLP_TX_CTR3_LOCATION_GET(_oth)     \
    ((_oth)->u3.s32.ctr3_location )
#define SOC_OLP_TX_CTR3_ID_GET(_oth)     \
    (((_oth)->u3.s32._ctr3_id1 << 11) | ((_oth)->u4.s42._ctr3_id2))
#define SOC_OLP_TX_CTR3_ACTION_GET(_oth)     \
    ((_oth)->u4.s42.ctr3_action )
#define SOC_OLP_TX_CTR1_OFF_LM_GET(_oth)     \
    ((_oth)->u4.s42.ctr1_off_to_lm )
#define SOC_OLP_TX_CTR2_OFF_LM_GET(_oth)     \
    ((_oth)->u4.s42.ctr2_off_to_lm )
#define SOC_OLP_TX_CTR3_OFF_LM_GET(_oth)     \
    ((_oth)->u4.s42.ctr3_off_to_lm )

#define SOC_OLP_TX_REL_LM_REP_OFF(_oth)     SOC_OLP_TX_REL_LM_REP_OFF_GET(_oth)    
#define SOC_OLP_TX_REL_LM_REP_OP(_oth)      SOC_OLP_TX_REL_LM_REP_OP_GET(_oth)
#define SOC_OLP_TX_CTR3_LOCATION(_oth)      SOC_OLP_TX_CTR3_LOCATION_GET(_oth)
#define SOC_OLP_TX_CTR3_ID_SET(_oth,val)    {           \
    (_oth)->u3.s32._ctr3_id1 = ((val) & 0x7800) >> 11;   \
    (_oth)->u4.s42._ctr3_id2 = (val) & (0x7ff);        \
    }
#define SOC_OLP_TX_CTR3_ACTION(_oth)        SOC_OLP_TX_CTR3_ACTION_GET(_oth)
#define SOC_OLP_TX_CTR1_OFF_LM(_oth)        SOC_OLP_TX_CTR1_OFF_LM_GET(_oth)
#define SOC_OLP_TX_CTR2_OFF_LM(_oth)        SOC_OLP_TX_CTR2_OFF_LM_GET(_oth)
#define SOC_OLP_TX_CTR3_OFF_LM(_oth)        SOC_OLP_TX_CTR3_OFF_LM_GET(_oth)

#endif /* BCM_SABER2_SUPPORT || BCM_APACHE_SUPPORT */

#define SOC_OLP_TX_CTR(pool,idx)    \
        ((((pool) << 12) & 0x7000) | ((idx) & 0xfff))

#endif /* _SOC_SHARED_OLP_PKT_H_ */

