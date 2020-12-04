/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hash table calculation routines
 */

#ifndef _SOC_HASH_H
#define _SOC_HASH_H

#include <soc/mem.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)
#ifdef BCM_XGS3_SWITCH_SUPPORT
typedef struct soc_xgs3_ecmp_hash_s{
    ip_addr_t  dip;            /* IPv4 destination IP.      */
    ip_addr_t  sip;            /* IPv4 source IP.           */
    ip6_addr_t dip6;           /* IPv6 destination IP.      */
    ip6_addr_t sip6;           /* IPv6 source IP.           */
    uint16     l4_src_port;    /* TCP/UDP source port.      */
    uint16     l4_dst_port;    /* TCP/UDP destination port. */
    uint8      v6;             /* IPv6 flag.                */
    int        ecmp_count;     /* Ecmp group size.          */
} soc_xgs3_ecmp_hash_t;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

extern uint32   soc_draco_crc32(uint8 *data, int data_size);
extern uint16   soc_draco_crc16(uint8 *data, int data_size);
#ifdef BCM_XGS_SWITCH_SUPPORT /* DPPCOMPILEENABLE */
extern uint32   soc_crc32b(uint8 *data, int data_nbits);
extern uint16   soc_crc16b(uint8 *data, int data_nbits);

extern void     soc_draco_l2x_base_entry_to_key(int unit, l2x_entry_t *entry,
                                                uint8 *key);
#endif                                                
extern void     soc_draco_l2x_param_to_key(sal_mac_addr_t mac, int vid,
                                           uint8 *key);
#ifdef BCM_XGS3_SWITCH_SUPPORT
extern uint32   soc_xgs3_l3_ecmp_hash(int unit, soc_xgs3_ecmp_hash_t *data);
#endif /* BCM_XGS3_SWITCH_SUPPORT */
extern uint32   soc_draco_trunk_hash(sal_mac_addr_t da,
                                     sal_mac_addr_t sa, int tgsize);
extern uint32   soc_fb_l2_hash(int unit, int hash_sel, uint8 *key);

extern uint32   soc_fb_l3_hash(int unit, int hash_sel,
                               int key_nbits, uint8 *key);
extern int      soc_fb_l3x_base_entry_to_key(int unit,
                                             uint32 *entry,
                                             uint8 *key);
extern uint32	soc_fb_l3x2_entry_hash(int unit, uint32 *entry);
extern uint32	soc_fb_vlan_mac_hash(int unit, int hash_sel, uint8 *key);
extern int      soc_fb_rv_vlanmac_hash_sel_get(int unit, int dual, int *hash_sel);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAPTOR_SUPPORT)
extern int      soc_fb_l2x_entry_bank_hash_sel_get(int unit, int bank,
                                                   int *hash_sel);
extern uint32   soc_fb_l2x_entry_hash(int unit, int hash_sel, uint32 *entry);
extern int      soc_fb_l3x_entry_bank_hash_sel_get(int unit, int bank,
                                                   int *hash_sel);
extern uint32   soc_fb_l3x_entry_hash(int unit, int hash_sel, uint32 *entry);
extern int      soc_fb_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */

#define FB2_HASH_VRF_BITS       6
#define FB2_HASH_VRF_MASK       0x3f

/* Triumph functions */
extern int      soc_tr_hash_sel_get(int unit, soc_mem_t mem, int bank,
                                    int *hash_sel);
extern uint32   soc_tr_l2x_hash(int unit, int hash_sel, int key_nbits,
                                void *base_entry, uint8 *key);
extern int      soc_tr_l2x_base_entry_to_key(int unit, void *entry,
                                             uint8 *key);
extern uint32   soc_tr_l2x_entry_hash(int unit, int hash_sel, uint32 *entry);
extern uint32   soc_tr_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);

extern uint32   soc_tr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                                       void *base_entry, uint8 *key);
extern int      soc_tr_vlan_xlate_base_entry_to_key(int unit, void *entry,
                                                    uint8 *key);
extern uint32   soc_tr_vlan_xlate_entry_hash(int unit, int hash_sel, 
                                             uint32 *entry);
extern uint32   soc_tr_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                  uint32 *entry);

extern uint32   soc_tr_egr_vlan_xlate_hash(int unit, int hash_sel,
                                           int key_nbits, void *base_entry,
                                           uint8 *key);
extern int      soc_tr_egr_vlan_xlate_base_entry_to_key(int unit, void *entry,
                                                        uint8 *key);
extern uint32   soc_tr_egr_vlan_xlate_entry_hash(int unit, int hash_sel, 
                                                 uint32 *entry);
extern uint32   soc_tr_egr_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                      uint32 *entry);
extern uint32   soc_tr_mpls_hash(int unit, int hash_sel, int key_nbits,
                                 void *base_entry, uint8 *key);
extern int      soc_tr_mpls_base_entry_to_key(int unit, void *entry,
                                              uint8 *key);
extern uint32   soc_tr_mpls_entry_hash(int unit, int hash_sel, uint32 *entry);
extern uint32   soc_tr_mpls_bank_entry_hash(int unit, int bank, uint32 *entry);

extern uint32   soc_tr3_ft_session_entry_hash(int unit, soc_mem_t, int hash_sel,
                                        uint32 *entry);
extern int      soc_tr3_ft_session_entry_to_key(int unit, soc_mem_t mem,
                                               void *entry, uint8 *key);
extern uint32   soc_tr3_wlan_entry_hash(int unit, soc_mem_t, int hash_sel, 
                                        uint32 *entry);
extern int      soc_tr3_wlan_base_entry_to_key(int unit, soc_mem_t mem, 
                                               void *entry, uint8 *key);
extern int      soc_dual_hash_recurse_depth_get(int unit, soc_mem_t mem);
extern int      soc_multi_hash_recurse_depth_get(int unit, soc_mem_t mem);

extern int      soc_td2_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                        int hash_offset, int use_lsb);

#if defined(BCM_TRIDENT2_SUPPORT)
extern int
soc_td2_hash_sel_get(int unit, soc_mem_t mem, int bank, int *hash_sel);
#endif
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
extern int soc_td2x_th_l2x_hash(int unit, uint32 *base_entry, int *num_banks,
                                int *bucket_array, int *index_array,
                                uint8 *hash_key);
extern int
soc_td2x_th_l3x_hash(int unit, soc_mem_t mem, uint32 *base_entry, int *num_banks,
                     int *bucket_array, int *index_array, int *phy_bank_array,
                     int *entry_num_array);
#endif
         
#define TRX_HASH_VRF_BITS       11 
#define TRX_HASH_VRF_MASK       0x7ff

enum XGS_HASH {
    /* WARNING: values given match hardware register; do not modify */
    XGS_HASH_CRC16_UPPER = 0,
    XGS_HASH_CRC16_LOWER = 1,
    XGS_HASH_LSB = 2,
    XGS_HASH_ZERO = 3,
    XGS_HASH_CRC32_UPPER = 4,
    XGS_HASH_CRC32_LOWER = 5,
    XGS_HASH_COUNT
};

enum FB_HASH {
    /* WARNING: values given match hardware register; do not modify */
    FB_HASH_ZERO = 0,
    FB_HASH_CRC32_UPPER = 1,
    FB_HASH_CRC32_LOWER = 2,
    FB_HASH_LSB = 3,
    FB_HASH_CRC16_LOWER = 4,
    FB_HASH_CRC16_UPPER = 5,
    FB_HASH_COUNT
};

enum XGS_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    XGS_HASH_KEY_TYPE_L2 = 0,
    XGS_HASH_KEY_TYPE_L3UC = 1,
    XGS_HASH_KEY_TYPE_L3MC = 2,
    XGS_HASH_KEY_TYPE_COUNT
};

enum TR_L3_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    TR_L3_HASH_KEY_TYPE_V4UC = 0,
    TR_L3_HASH_KEY_TYPE_V4MC = 1,
    TR_L3_HASH_KEY_TYPE_V6UC = 2,
    TR_L3_HASH_KEY_TYPE_V6MC = 3,
    TR_L3_HASH_KEY_TYPE_LMEP = 4,
    TR_L3_HASH_KEY_TYPE_RMEP = 5,
    TR_L3_HASH_KEY_TYPE_TRILL = 6,
    TR_L3_HASH_KEY_TYPE_COUNT
};

enum TR_L2_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    TR_L2_HASH_KEY_TYPE_BRIDGE = 0,
    TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    TR_L2_HASH_KEY_TYPE_VFI = 3,
    TR_L2_HASH_KEY_TYPE_VIF = 4,
    TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS = 5,
    TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG = 6,
    TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT = 7,
    TR_L2_HASH_KEY_TYPE_PE_VID = 9,
    TR_L2_HASH_KEY_TYPE_COUNT
};

enum KT_L2_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    KT_L2_HASH_KEY_TYPE_BRIDGE = 0,
    KT_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    KT_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    KT_L2_HASH_KEY_TYPE_VFI = 3,
    KT_L2_HASH_KEY_TYPE_BFD = 4,
    KT_L2_HASH_KEY_TYPE_COUNT
};

/*  Vlan Xlate common key types  */
enum VLXLT_HASH_KEY_TYPE {
    VLXLT_HASH_KEY_TYPE_IVID_OVID,
    VLXLT_HASH_KEY_TYPE_OTAG,
    VLXLT_HASH_KEY_TYPE_ITAG,
    VLXLT_HASH_KEY_TYPE_VLAN_MAC,
    VLXLT_HASH_KEY_TYPE_OVID,
    VLXLT_HASH_KEY_TYPE_IVID,
    VLXLT_HASH_KEY_TYPE_PRI_CFI,
    VLXLT_HASH_KEY_TYPE_HPAE,    
    VLXLT_HASH_KEY_TYPE_VIF,
    VLXLT_HASH_KEY_TYPE_VIF_VLAN,
    VLXLT_HASH_KEY_TYPE_VIF_CVLAN,
    VLXLT_HASH_KEY_TYPE_VIF_OTAG,
    VLXLT_HASH_KEY_TYPE_VIF_ITAG,
    VLXLT_HASH_KEY_TYPE_L2GRE_DIP,
    VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT,
    VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN,
    VLXLT_HASH_KEY_TYPE_IVID_VSAN,
    VLXLT_HASH_KEY_TYPE_OVID_VSAN,
    VLXLT_HASH_KEY_TYPE_VXLAN_DIP,
    VLXLT_HASH_KEY_TYPE_LLVID,
    VLXLT_HASH_KEY_TYPE_LLVID_OVID,
    VLXLT_HASK_KEY_TYPE_LLVID_IVID,
    VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID,
    VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG,
    VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG,
    VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID,
    VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID,
    VLXLT_HASH_KEY_TYPE_VNID,
    VLXLT_HASH_KEY_TYPE_COUNT
};

/* Egress Vlan Xlate common key types */
enum EGR_VLXLT_HASH_KEY_TYPE {
    EGR_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN,
    EGR_VLXLT_HASH_KEY_TYPE_OVID_VSAN,
    EGR_VLXLT_HASH_KEY_TYPE_IVID_VSAN,
    EGR_VLXLT_HASH_KEY_TYPE_IVID_OVID,
    EGR_VLXLT_HASH_KEY_TYPE_IVID,
    EGR_VLXLT_HASH_KEY_TYPE_OVID,
    EGR_VLXLT_HASH_KEY_TYPE_VSAN
};

/* Triumph key type values */
enum TR_VLXL_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    TR_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    TR_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    TR_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    TR_VLXLT_HASH_KEY_TYPE_OVID = 4,
    TR_VLXLT_HASH_KEY_TYPE_IVID = 5,
    TR_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    TR_VLXLT_HASH_KEY_TYPE_HPAE = 7,    /* Not supported until TR2 */
    TR_VLXLT_HASH_KEY_TYPE_VIF = 8,
    TR_VLXLT_HASH_KEY_TYPE_VIF_VLAN = 9,
    TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT = 14,
    TR_VLXLT_HASH_KEY_TYPE_COUNT
};

/* Triumph3 key type values */
enum TR3_VLXL_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    TR3_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID = 1,
    TR3_VLXLT_HASH_KEY_TYPE_OVID = 2,
    TR3_VLXLT_X_HASH_KEY_TYPE_OVID = 3,
    TR3_VLXLT_HASH_KEY_TYPE_IVID = 4,
    TR3_VLXLT_X_HASH_KEY_TYPE_IVID = 5,
    TR3_VLXLT_HASH_KEY_TYPE_OTAG = 6,
    TR3_VLXLT_X_HASH_KEY_TYPE_OTAG = 7,
    TR3_VLXLT_HASH_KEY_TYPE_ITAG = 8,
    TR3_VLXLT_X_HASH_KEY_TYPE_ITAG = 9,
    TR3_VLXLT_HASH_KEY_TYPE_PRI_CFI = 10,
    TR3_VLXLT_X_HASH_KEY_TYPE_PRI_CFI = 11,
    TR3_VLXLT_HASH_KEY_TYPE_IVID_OVID_SVP = 12,
    TR3_VLXLT_X_HASH_KEY_TYPE_IVID_OVID_SVP = 13,
    TR3_VLXLT_HASH_KEY_TYPE_OVID_SVP = 14,
    TR3_VLXLT_X_HASH_KEY_TYPE_OVID_SVP = 15,
    TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 20,
    TR3_VLXLT_HASH_KEY_TYPE_VIF = 21,
    TR3_VLXLT_HASH_KEY_TYPE_VIF_VLAN = 22,
    TR3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN = 23,
    TR3_VLXLT_HASH_KEY_TYPE_VIF_OTAG = 24,
    TR3_VLXLT_HASH_KEY_TYPE_VIF_ITAG = 25,
    TR3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP = 26,
    TR3_VLXLT_HASH_KEY_TYPE_HPAE = 27,
    TR3_VLXLT_HASH_KEY_TYPE_COUNT
};

/* Triumph3 PORT_TABLE key type values in VT_KEY_TYPE(_2) differs from
 * VLAN_XLATE(_2).KEY_TYPE values
 */
enum TR3_PT_VLXL_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    TR3_PT_XLT_KEY_TYPE_IVID_OVID = 0,
    TR3_PT_XLT_KEY_TYPE_OTAG = 1,
    TR3_PT_XLT_KEY_TYPE_ITAG = 2,
    TR3_PT_XLT_KEY_TYPE_VLAN_MAC = 3,
    TR3_PT_XLT_KEY_TYPE_OVID = 4,
    TR3_PT_XLT_KEY_TYPE_IVID = 5,
    TR3_PT_XLT_KEY_TYPE_PRI_CFI = 6,
    TR3_PT_XLT_KEY_TYPE_HPAE = 7,    
    TR3_PT_XLT_KEY_TYPE_VIF = 8,
    TR3_PT_XLT_KEY_TYPE_VIF_VLAN = 9,
    TR3_PT_XLT_KEY_TYPE_VIF_CVLAN = 10,
    TR3_PT_XLT_KEY_TYPE_VIF_OTAG = 11,
    TR3_PT_XLT_KEY_TYPE_VIF_ITAG = 12,
    TR3_PT_XLT_KEY_TYPE_COUNT
};

/* KT2 PORT_TABLE and VLAN_XLATE key type values
 */
enum KT2_PT_VLXL_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    KT2_PT_XLT_KEY_TYPE_IVID_OVID = 0,
    KT2_PT_XLT_KEY_TYPE_OTAG = 1,
    KT2_PT_XLT_KEY_TYPE_ITAG = 2,
    KT2_PT_XLT_KEY_TYPE_VLAN_MAC = 3, /* Reserved in Kt2, not used */
    KT2_PT_XLT_KEY_TYPE_OVID = 4,
    KT2_PT_XLT_KEY_TYPE_IVID = 5,
    KT2_PT_XLT_KEY_TYPE_PRI_CFI = 6,
    KT2_PT_XLT_KEY_TYPE_HPAE = 7,    /* Reserved in KT2, not used */
    KT2_PT_XLT_KEY_TYPE_VIF = 8,
    KT2_PT_XLT_KEY_TYPE_VIF_VLAN = 9,
    KT2_PT_XLT_KEY_TYPE_VIF_CVLAN = 10,
    KT2_PT_XLT_KEY_TYPE_VIF_OTAG = 11,
    KT2_PT_XLT_KEY_TYPE_VIF_ITAG = 12,
    KT2_PT_XLT_KEY_TYPE_LLVID = 13, /* LLTAG VLAN ID */
    KT2_PT_XLT_KEY_TYPE_LLVID_IVID = 14, /* LLTAG VLAN ID plus inner VLAN ID */
    KT2_PT_XLT_KEY_TYPE_LLVID_OVID = 15, /* LLTAG VLAN ID plus inner VLAN ID */
    KT2_PT_XLT_KEY_TYPE_COUNT
};

#ifdef SOC_ROBUST_HASH
extern int
soc_robust_hash_get(int unit, soc_robust_hash_config_t *rbh_cfg, int bank,
                    uint8 *key, int n_bits);
extern int
soc_robust_hash_get2(int unit, soc_robust_hash_config_t *rbh_cfg, int bank,
                    uint8 *key, int n_bits);
extern int
soc_robust_hash_table_set(int unit, soc_robust_hash_config_t *rbh_cfg,
                          int seed);

#define ROBUSTHASH(_u)  ((SOC_CONTROL(_u))->soc_robust_hash_config)
#endif /* SOC_ROBUST_HASH */

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
/* Maximum hash key size is 415b (L3) + 16b (Robust Hash) */
#define	XGS_HASH_KEY_SIZE	(52 + 2)
#else
#define	XGS_HASH_KEY_SIZE	10
#endif
#define SOC_LYNX_DMUX_MASK      0x3f    /* Lynx Mux/Demux only uses 6 bits */

#define SOC_L3_TUNNEL_BUCKET_SIZE      8
#define SOC_MPLS_ENTRY_BUCKET_SIZE     8
#define SOC_VLAN_XLATE_BUCKET_SIZE     8
#define SOC_EGR_VLAN_XLATE_BUCKET_SIZE 8
#define SOC_VLAN_MAC_BUCKET_SIZE       8
#define SOC_WLAN_BUCKET_SIZE           8
#define SOC_ING_VP_VLAN_MEMBER_BUCKET_SIZE     8
#define SOC_EGR_VP_VLAN_MEMBER_BUCKET_SIZE     8
#define SOC_ING_DNAT_ADDRESS_TYPE_BUCKET_SIZE  8
#define SOC_L2_ENDPOINT_ID_BUCKET_SIZE         8
#define SOC_ENDPOINT_QUEUE_MAP_BUCKET_SIZE     8
#define SOC_EGR_MP_GROUP_BUCKET_SIZE        8
#define SOC_FT_SESSION_BUCKET_SIZE      8
#define SOC_SUBPORT_ID_TO_SGPP_MAP_BUCKET_SIZE 8
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT || PORTMOD_SUPPORT*/

#endif  /* !_SOC_HASH_H */
