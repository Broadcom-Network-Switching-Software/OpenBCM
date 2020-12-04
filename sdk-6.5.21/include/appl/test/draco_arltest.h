/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Draco ARL Test definitions.
 */

#ifndef __DRACO_ARLTEST__H
#define __DRACO_ARLTEST__H

/*
 * Draco ARL Test Data Structure
 */

typedef struct draco_l2_testdata_s {
    int                 unit;
    int                 opt_count;
    int                 opt_verbose;
    int                 opt_reset;
    int                 opt_hash;
    int                 opt_base_vid;
    int                 opt_vid_inc;         /* VID increment */
    sal_mac_addr_t      opt_base_mac;
    int                 opt_mac_inc;         /* Mac increment */
    COMPILER_DOUBLE     tm;
    uint32              save_hash_control;
    int                 hash_count;          /* Hash Alg count */
    uint32              save_l2_aux_hash_control;
    uint32              save_l2_table_hash_control;
    uint32              save_shared_table_hash_control;
#ifdef BCM_TRIDENT3_SUPPORT
    l2_entry_hash_control_entry_t  save_l2_entry_hash_control;
    uft_shared_banks_control_entry_t save_shared_entry_hash_control;
#endif
} draco_l2_testdata_t;

#define DRACO_L2_VID_MAX        (0x7ff) 
#define DRACO_L3_VID_MAX        (0xfff) 
#define DRACO_ARL_DEFAULT_HASH  XGS_HASH_LSB

#define FB_L2_VID_MAX            (0xfff)
#define FB_L2_DEFAULT_HASH      FB_HASH_LSB
#define FB_L3_DEFAULT_HASH      FB_HASH_LSB

#define MAX_VRF_ID              (0x3f)
#define FB2_VRF_ID_MAX          MAX_VRF_ID
#define TR_VRF_ID_MAX           (0x7ff)

typedef struct draco_l3_testdata_s {
    int                 unit;
    int                 opt_count;
    int                 opt_verbose;
    int                 opt_reset;
    int                 opt_hash;
    int                 opt_dual_hash;
    int                 opt_ipmc_enable;
    int                 opt_key_src_ip;      /* Source IP in upper IPMC key */
    ip_addr_t           opt_base_ip;
    int                 opt_ip_inc;          /* IP addr increment */
    ip_addr_t           opt_src_ip;
    int                 opt_src_ip_inc;      /* Source IP addr increment */
    int                 opt_base_vid;
    int                 opt_vid_inc;         /* VID increment */
    sal_mac_addr_t      opt_base_mac;
    int                 opt_mac_inc;         /* Mac increment */
    ip6_addr_t          opt_base_ip6;
    ip6_addr_t          opt_src_ip6;
    int                 opt_ip6_inc;
    int                 opt_src_ip6_inc;
    int                 ipv6;                /* Test uses IPv6 args */
    COMPILER_DOUBLE     tm;
    int                 save_hash_control;
    int                 save_dual_hash_control;
    int                 save_l3_table_hash_control;
    int                 save_shared_table_hash_control;
    uint32              save_ipmc_config;
    uint32              set_ipmc_config;
    int                 hash_count;          /* Hash Alg count */
    int                 opt_base_vrf_id;     /* VRF ID */ 
    int                 opt_vrf_id_inc;      /* VRF ID Increment */ 
#ifdef BCM_TRIDENT3_SUPPORT
    l3_entry_hash_control_entry_t  save_l3_entry_hash_control;
    uft_shared_banks_control_entry_t save_shared_entry_hash_control;
#endif
} draco_l3_testdata_t;

/* L2/L3 test structure, to retain test settings between runs */
typedef struct draco_l2_test_s {
    int                 dlw_set_up;     /* TRUE if dl2_setup() done */
    draco_l2_testdata_t dlp_l2_hash;    /* L2 hash Parameters */
    draco_l2_testdata_t dlp_l2_ov;      /* L2 overflow Parameters */
    draco_l2_testdata_t dlp_l2_lu;      /* L2 lookup Parameters */
    draco_l2_testdata_t dlp_l2_dp;      /* L2 delete by port Parameters */
    draco_l2_testdata_t dlp_l2_dv;      /* L2 delete by VLAN Parameters */
    draco_l2_testdata_t *dlp_l2_cur;    /* Current L2 test parameters */
    int                 dlw_unit;       /* Unit # */
} draco_l2_test_t;

typedef struct draco_l3_test_s {
    int                 dlw_set_up;     /* TRUE if dl3_setup() done */
    draco_l3_testdata_t dlp_l3_hash;    /* L3 hash Parameters */
    draco_l3_testdata_t dlp_l3_ov;      /* L3 overflow Parameters */
    draco_l3_testdata_t *dlp_l3_cur;    /* Current L3 test parameters */
    int                 dlw_unit;       /* Unit # */
} draco_l3_test_t;

#endif /*!__DRACO_ARLTEST__H */
