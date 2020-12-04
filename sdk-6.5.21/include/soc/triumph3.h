/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        triumph3.h
 */

#ifndef _SOC_TRIUMPH3_H_
#define _SOC_TRIUMPH3_H_

#include <soc/drv.h>

/*#define MMU_RESET_DEBUG_SUPPORT   1*/
#define _SOC_TR3_MAX_NUM_LPORTS                            64
#define _SOC_TR3_IBOD_INTR_MASK                        0x01000
#define _SOC_TR3_FLOWTRACKER_INTR_MASK                 0x08000
#define _SOC_TR3_ISM_INTR_MASK                         0x10000
#define _SOC_TR3_ESM_INTR_MASK                         0x20000
#define _SOC_TR3_AXP_INTR_MASK                         0x40000
#define _SOC_TR3_FUNC_PARITY_INTR_MASK (_SOC_TR3_IBOD_INTR_MASK | \
        _SOC_TR3_ISM_INTR_MASK | _SOC_TR3_AXP_INTR_MASK | \
        _SOC_TR3_FLOWTRACKER_INTR_MASK)
#define _SOC_TR3_FUNC_INTR_MASK _SOC_TR3_FUNC_PARITY_INTR_MASK

#define _SOC_TR3_L2_BULK_BURST_MIN                     0
#define _SOC_TR3_L2_BULK_BURST_MAX                     7

#define _SOC_TR3_PHY_PER_PORT_BLOCK                    3
#define _SOC_TR3_LANES_PER_PHY                         4
#define _SOC_TR3_MAX_HG_PORT_COUNT                     24

/* sample esm config-related defines */
#define F11_HWTL                                       7
#define F11_DA_ETHTYPE_SA_VID_VFI_SEL_VID_VFI_DATA_SRC 5
#define F11_SA_OTAG_DA_ETHTYPE                         4
#define F11_NSA_OTAG_NDA_ETHTYPE                       3
#define F11_UDF1                                       2
#define F11_UDF2                                       1

#define F10_HWTL                                       7
#define F10_DIPV6                                      5
#define F10_N_DIPV6                                    4
#define F10_SIPV6_U64_DIPV6_U64                        3
#define F10_UDF1                                       2
#define F10_UDF2                                       1

#define F9_HWTL                                        7
#define F9_SIPV6                                       5
#define F9_N_SIPV6                                     4
#define F9_SIPV6_L64_DIPV6_L64                         3
#define F9_UDF1                                        2
#define F9_UDF2                                        1

#define F8_HWTL                                        31
#define F8_SA_3210_DA_3210                             13
#define F8_SA_DA_3210                                  13
#define F8_OTAG_DA                                     12
#define F8_OTAG_SA                                     11
#define F8_SIPV4_DIPV4                                 10
#define F8_N_SIPV4_N_DIPV4                             9
#define F8_SIPV6_U64                                   8
#define F8_DIPV6_U64                                   7
#define F8_IP_FLOW_V6_BITS_FLAGS_IPV4                  6
#define F8_AUX_TAG_1_AUX_TAG_2                         5
#define F8_UDF1_3210                                   4
#define F8_UDF1_7654                                   3
#define F8_UDF2_3210                                   2
#define F8_UDF2_7654                                   1

#define F7_HWTL                                        31
#define F7_INT_RANGE_CHECK_L3_IIF                      20
#define F7_SA_54_DA_54                                 19
#define F7_SA_54_ETHTYPE                               18
#define F7_DA_54_ETHTYPE                               17
#define F7_OTAG_ITAG                                   16
#define F7_OTAG_ETHTYPE                                15
#define F7_SIPV4                                       14
#define F7_DIPV4                                       13
#define F7_L4_SRC_L4_DST                               12
#define F7_N_L4_SRC_N_L4_DST                           11
#define F7_IP_FLOW_FLAGS_IP_2                          10
#define F7_SVP_DVP                                     9
#define F7_SGLP_DGLP                                   8
#define F7_AUX_TAG_1                                   7
#define F7_AUX_TAG_2                                   6
#define F7_CLASSID_ALL                                 5
#define F7_UDF2_10                                     4
#define F7_UDF2_32                                     3
#define F7_UDF2_54                                     2
#define F7_UDF2_76                                     1

#define F6_HWTL                                        31
#define F6_SA_3210                                     19
#define F6_DA_3210                                     18
#define F6_SA_54_DA_54                                 17
#define F6_OTAG_ITAG                                   16
#define F6_OTAG_ETHERTYPE                              15
#define F6_SIPV4                                       14
#define F6_DIPV4                                       13
#define F6_L4_SRC_L4_DST                               12
#define F6_N_L4_SRC_N_L4_DST                           11
#define F6_IP_FLOW_FLAGS_IP_2                          10
#define F6_SVP_DVP                                     9
#define F6_SGLP_DGLP                                   8
#define F6_AUX_TAG_1                                   7
#define F6_AUX_TAG_2                                   6
#define F6_CLASSID_ALL                                 5
#define F6_UDF1_10                                     4
#define F6_UDF1_32                                     3
#define F6_UDF1_54                                     2
#define F6_UDF1_76                                     1

#define F5_HWTL                                        31
#define F5_ETHTYPE                                     22
#define F5_SA_54                                       21
#define F5_OTAG                                        20
#define F5_ITAG                                        19
#define F5_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST       18
#define F5_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC       17
#define F5_FLAGS_IP_1_VRF                              16
#define F5_L3_PROTOCOL_TOS                             15
#define F5_FLAGS_IP_3                                  14
#define F5_TOS_TTL                                     13
#define F5_L4_SRC                                      12
#define F5_L4_DST                                      11
#define F5_L3_IIF                                      10
#define F5_INT_RANGE_CHECK                             9
#define F5_SVP                                         8
#define F5_DVP                                         7
#define F5_SGLP                                        6
#define F5_DGLP                                        5
#define F5_CLASSID_INTERFACE                           4
#define F5_CLASSID_SRC                                 3
#define F5_CLASSID_DST                                 2
#define F5_UDF_1_AND_2_VALID                           1

#define F4_HWTL                                        31
#define F4_ETHTYPE                                     29
#define F4_DA_54                                       28
#define F4_OTAG                                        27
#define F4_ITAG                                        26
#define F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST       25
#define F4_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC       24
#define F4_FLAGS_IP_1_VRF                              23
#define F4_L3_PROTOCOL_TOS                             22
#define F4_FLAGS_IP_3                                  21
#define F4_TOS_TTL                                     20
#define F4_L4_SRC                                      19
#define F4_L4_DST                                      18
#define F4_L3_IIF                                      17
#define F4_INT_RANGE_CHECK                             16
#define F4_SVP                                         15
#define F4_DVP                                         14
#define F4_SGLP                                        13
#define F4_DGLP                                        12
#define CLASSID_INTERFACE                              11
#define F4_CLASSID_SRC                                 10
#define F4_CLASSID_DST                                 9
#define F4_UDF2_0                                      8
#define F4_UDF2_1                                      7
#define F4_UDF2_2                                      6
#define F4_UDF2_3                                      5
#define F4_UDF2_4                                      4
#define F4_UDF2_5                                      3
#define F4_UDF2_6                                      2
#define F4_UDF2_7                                      1

#define F3_HWTL                                        31
#define F3_ETHTYPE                                     29
#define F3_OTAG                                        28
#define F3_ITAG                                        27
#define F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_DST       26
#define F3_FREE_BIT_VID_VFI_SEL_VID_VFI_DATA_SRC       25
#define F3_FLAGS_IP_1_VRF                              24
#define F3_L3_PROTOCOL_TOS                             23
#define F3_FLAGS_IP_3                                  22
#define F3_TOS_TTL                                     21
#define F3_L4_SRC                                      20
#define F3_L4_DST                                      19
#define F3_L3_IIF                                      18
#define F3_INT_RANGE_CHECK                             17
#define F3_SVP                                         16
#define F3_DVP                                         15
#define F3_SGLP                                        14
#define F3_DGLP                                        13
#define F3_CLASSID_INTERFACE                           12
#define F3_CLASSID_SRC                                 11
#define F3_CLASSID_DST                                 10
#define F3_UDF_1_AND_2_VALID                           9
#define F3_UDF1_0                                      8
#define F3_UDF1_1                                      7
#define F3_UDF1_2                                      6
#define F3_UDF1_3                                      5
#define F3_UDF1_4                                      4
#define F3_UDF1_5                                      3
#define F3_UDF1_6                                      2
#define F3_UDF1_7                                      1

#define F1_HWTL                                        7
#define F1_DIPV6                                       5
#define F1_N_DIPV6                                     4
#define F1_SIPV6_U64_DIPV6_U64                         3
#define F1_SIPV6_L64_DIPV6_L64                         2
#define F1_UDF2                                        1

#define F2_FLAG_TOS_7                                  123
#define F2_FLAG_TOS_6                                  122
#define F2_FLAG_TOS_5                                  121
#define F2_FLAG_TOS_4                                  120
#define F2_FLAG_TOS_3                                  119
#define F2_FLAG_TOS_2                                  118
#define F2_FLAG_TOS_1                                  117
#define F2_FLAG_TOS_0                                  116
#define F2_FLAG_INCOMING_TAG_STATUS_1                  115
#define F2_FLAG_INCOMING_TAG_STATUS_0                  114
#define F2_FLAG_MY_STATION_HIT                         95
#define F2_FLAG_MH_MIRROR_ONLY                         94
#define F2_FLAG_HG                                     93
#define F2_FLAG_L3_IIF_VALID                           86
#define F2_FLAG_L3_PROTOCOL_FN_3                       61
#define F2_FLAG_L3_PROTOCOL_FN_2                       60
#define F2_FLAG_L3_PROTOCOL_FN_1                       59
#define F2_FLAG_L3_PROTOCOL_FN_0                       58
#define F2_FLAG_L3_TYPE_3                              39
#define F2_FLAG_L3_TYPE_2                              38
#define F2_FLAG_L3_TYPE_1                              37
#define F2_FLAG_L3_TYPE_0                              36
#define F2_FLAG_FORWARDING_TYPE_2                      34
#define F2_FLAG_FORWARDING_TYPE_1                      33
#define F2_FLAG_FORWARDING_TYPE_0                      32
#define F2_FLAG_LOOKUP_STATUS_VECTOR_17                29
#define F2_FLAG_LOOKUP_STATUS_VECTOR_16                28
#define F2_FLAG_LOOKUP_STATUS_VECTOR_15                27
#define F2_FLAG_LOOKUP_STATUS_VECTOR_14                26
#define F2_FLAG_LOOKUP_STATUS_VECTOR_13                25
#define F2_FLAG_LOOKUP_STATUS_VECTOR_12                24
#define F2_FLAG_LOOKUP_STATUS_VECTOR_11                23
#define F2_FLAG_LOOKUP_STATUS_VECTOR_10                22
#define F2_FLAG_LOOKUP_STATUS_VECTOR_9                 21
#define F2_FLAG_LOOKUP_STATUS_VECTOR_8                 20
#define F2_FLAG_LOOKUP_STATUS_VECTOR_7                 19
#define F2_FLAG_LOOKUP_STATUS_VECTOR_6                 18
#define F2_FLAG_LOOKUP_STATUS_VECTOR_5                 17
#define F2_FLAG_LOOKUP_STATUS_VECTOR_4                 16
#define F2_FLAG_LOOKUP_STATUS_VECTOR_3                 15
#define F2_FLAG_LOOKUP_STATUS_VECTOR_2                 14
#define F2_FLAG_LOOKUP_STATUS_VECTOR_1                 13
#define F2_FLAG_LOOKUP_STATUS_VECTOR_0                 12
#define F2_FLAG_TUNNEL_AND_LPBK_TYPE_4                 11
#define F2_FLAG_TUNNEL_AND_LPBK_TYPE_3                 10
#define F2_FLAG_TUNNEL_AND_LPBK_TYPE_2                 9
#define F2_FLAG_TUNNEL_AND_LPBK_TYPE_1                 8
#define F2_FLAG_TUNNEL_AND_LPBK_TYPE_0                 7
#define F2_FLAG_ESM_KEY_SUBTYPE_0                      0
#define F2_FLAG_RESERVED_0                             109

#define FLAGS_FIELD_ABSENT                             1

#define KEY_GEN_RSP_TYPE_L2_SA                         1
#define KEY_GEN_RSP_TYPE_L2_DA                         2
#define KEY_GEN_RSP_TYPE_SIP                           3 
#define KEY_GEN_RSP_TYPE_DIP                           4
#define KEY_GEN_RSP_TYPE_ACL                           5
#define KEY_GEN_RSP_TYPE_SA_OPT                        6

#define KEY_GEN_OFFSET_L2                              1
#define KEY_GEN_OFFSET_IPV4_ONLY                       2
#define KEY_GEN_OFFSET_IPV6_64_ONLY                    3
#define KEY_GEN_OFFSET_IPV6_128_ONLY                   4
#define KEY_GEN_OFFSET_IPV4_SA                         5
#define KEY_GEN_OFFSET_IPV6_128_SA                     6
#define KEY_GEN_OFFSET_IPV6_64_SA                      7            
#define KEY_GEN_OFFSET_AFLSS0                          8
#define KEY_GEN_OFFSET_AFLSS1                          9
#define KEY_GEN_OFFSET_AFLSS2                          10
#define KEY_GEN_OFFSET_AFLSS3                          11
#define KEY_GEN_OFFSET_AFLSS0A                         12
#define KEY_GEN_OFFSET_AFLSS1A                         13
#define KEY_GEN_OFFSET_AFLSS2A                         14
#define KEY_GEN_OFFSET_AFLSS1MP                        15
#define KEY_GEN_OFFSET_FLSS0                           16
#define KEY_GEN_OFFSET_TR2_ACL_L2C                     17
#define KEY_GEN_OFFSET_TR2_ACL_L2C_IP4                 18
#define KEY_GEN_OFFSET_TR2_ACL_L2C_IP6U                19
#define KEY_GEN_OFFSET_TR2_ACL_L2C_IP6                 20
#define KEY_GEN_OFFSET_TR2_ACL_L2C_ONLY                21
#define KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SGLP           22
#define KEY_GEN_OFFSET_TR2_ACL_L2_NON7K_SVP            23
#define KEY_GEN_OFFSET_TR2_ACL_L2_7K_SGLP              24
#define KEY_GEN_OFFSET_TR2_ACL_L2_7K_SVP               25
#define KEY_GEN_OFFSET_TR2_ACL_L2_V6U_NON7K_SGLP       26
#define KEY_GEN_OFFSET_TR2_ACL_L2_ONLY                 27
#define KEY_GEN_OFFSET_TR2_ACL_IP4C                    28
#define KEY_GEN_OFFSET_TR2_ACL_IP4C_L2                 29
#define KEY_GEN_OFFSET_TR2_ACL_IP4C_ONLY               30
#define KEY_GEN_OFFSET_TR2_ACL_IP4                     31
#define KEY_GEN_OFFSET_TR2_ACL_IP4_L2                  32
#define KEY_GEN_OFFSET_TR2_ACL_IP4_ONLY                33
#define KEY_GEN_OFFSET_TR2_ACL_L2IP4                   34
#define KEY_GEN_OFFSET_TR2_ACL_L2IP4_L2                35
#define KEY_GEN_OFFSET_TR2_ACL_L2IP4_ONLY              36
#define KEY_GEN_OFFSET_TR2_ACL_IP6C                    37
#define KEY_GEN_OFFSET_TR2_ACL_IP6C_IP6U_FWD           38
#define KEY_GEN_OFFSET_TR2_ACL_IP6C_L2                 39
#define KEY_GEN_OFFSET_TR2_ACL_IP6C_ONLY               40
#define KEY_GEN_OFFSET_TR2_ACL_IP6S                    41
#define KEY_GEN_OFFSET_TR2_ACL_IP6S_IP6U_FWD           42
#define KEY_GEN_OFFSET_TR2_ACL_IP6S_L2                 43
#define KEY_GEN_OFFSET_TR2_ACL_IP6S_ONLY               44
#define KEY_GEN_OFFSET_TR2_ACL_IP6F                    45
#define KEY_GEN_OFFSET_TR2_ACL_IP6F_IP6U_FWD           46
#define KEY_GEN_OFFSET_TR2_ACL_IP6F_L2                 47
#define KEY_GEN_OFFSET_TR2_ACL_IP6F_ONLY               48
#define KEY_GEN_OFFSET_TR2_ACL_L2IP6                   49
#define KEY_GEN_OFFSET_TR2_ACL_L2IP6_L2                50
#define KEY_GEN_OFFSET_TR2_ACL_L2IP6_ONLY              51
/*
 * This macro is introduced for Source Mac address based
 * ESM lookup through VFP_POLICY_TABLE WAR purpose
 */
#define KEY_GEN_OFFSET_TR3_WAR_L2SA_ONLY               52
#define KEY_GEN_OFFSET_MAX                             52

#define PKT_TYPE_L2                                    1
#define PKT_TYPE_IPV4                                  2
#define PKT_TYPE_IPV6                                  3
                                                       
/* tcam forwarding behaviors */                        
#define FWD_TYPE_L2_VLAN                               1
#define FWD_TYPE_L2_VFI                                2
#define FWD_TYPE_V4_BRIDGE                             3
#define FWD_TYPE_V6_BRIDGE                             4
#define FWD_TYPE_V4_ROUTE                              5
#define FWD_TYPE_V6_ROUTE                              6
                                                       
#define SRCH_PROFILE_SRC_L3IIF_BASE                    0
#define SRCH_PROFILE_SRC_SRC_VP_BASE                   256
#define SRCH_PROFILE_SRC_VLAN_BASE                     512
#define SRCH_PROFILE_SRC_VFP_BASE                      768
#define SRCH_PROFILE_SRC_VLAN_XLAT_BASE                1024
#define SRCH_PROFILE_SRC_TRUNK_MAP_BASE                1280

#define SRCH_PROFILE_SRC_TRUNK_MAP_0    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 0)
#define SRCH_PROFILE_SRC_TRUNK_MAP_1    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 32)
#define SRCH_PROFILE_SRC_TRUNK_MAP_2    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 2*32)
#define SRCH_PROFILE_SRC_TRUNK_MAP_3    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 3*32)
#define SRCH_PROFILE_SRC_TRUNK_MAP_4    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 4*32)
#define SRCH_PROFILE_SRC_TRUNK_MAP_5    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 5*32)
#define SRCH_PROFILE_SRC_TRUNK_MAP_6    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 6*32)
#define SRCH_PROFILE_SRC_TRUNK_MAP_7    (SRCH_PROFILE_SRC_TRUNK_MAP_BASE + 7*32)

#define ET_PA_AD_L2_ENTRY                              0
#define ET_PA_AD_L3_ENTRY_DEFIP                        0
#define ET_PA_AD_L3_ENTRY_UCAST                        1
                                                       
#define SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE           0
#define SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI              1
#define SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE           0
#define SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI              1
#define SOC_MEM_TR3_EXT_L2_MAX_ENTRIES                 1048576

/* HSP Hierarchy related constants */
#define _SOC_HSP_MAX_L0_NODES         10
#define _SOC_HSP_VALID_L0_START       1
#define _SOC_HSP_VALID_L0_END         9
#define _SOC_HSP_MIN_QUEUE_OFFSET     0
#define _SOC_HSP_MAX_QUEUE_OFFSET     3
#define _SOC_HSP_LAST_L0_UC_START     8
#define _SOC_HSP_LAST_L0_MC_START     18
#define _SOC_HSP_LAST_L0_MAX_UC_QUEUE 2
#define _SOC_HSP_L0_BASE_HW_INDEX     232
#define _SOC_HSP_L0_PER_PORT_OFFSET   12
#define _SOC_HSP_NUM_UC_QUEUE         10
#define _SOC_HSP_NUM_MC_QUEUE         10
#define _SOC_HSP_LAST_L0_NUM_UC_QUEUE 2
#define _SOC_HSP_LAST_L0_NUM_MC_QUEUE 2
typedef struct _tr3_worst_fme {
    uint8 cost0;
    uint8 cost1;
    uint8 cost2;
    uint8 idx;
} tr3_worst_fme_t;

typedef struct soc_et_pa_xlat_ {
    uint32 et_width;
    uint32 ad_width;
    uint32 entry_type; 
    uint32 l2_bulk_enable;
    uint32 hit_bit_enable;
    uint32 hit_bit_pa_base;
    uint32 ad_ba;
    uint32 ad_ra_base;
    uint32 ad_ca;
} soc_et_pa_xlat_t;

extern int soc_tr3_esm_get_worst_fme(int unit);
extern soc_functions_t soc_triumph3_drv_funs;

#define SOC_TR3_CORE_CLOCK_UNKNOWN                     0
#define SOC_TR3_CORE_CLOCK_315                         315
#define SOC_TR3_CORE_CLOCK_415                         415
#define SOC_TR3_CORE_CLOCK_450                         450

extern int soc_tr3_core_clock_speed(int unit);
extern int soc_tr3_mem_config(int unit, int dev_id, int rev_id);
extern void soc_tr3_chip_func_ft_intr(void *unit_vp, void *d1,
                                      void *d2, void *d3, void *d4);

extern int _soc_triumph3_mem_parity_control(int unit, soc_mem_t mem,
                                            int copyno, int enable);
extern void soc_tr3_process_func_intr(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);
extern int soc_tr3_ser_mem_clear(int unit, soc_mem_t mem);

extern void soc_tr3_blk_counter_config(int unit);
extern int soc_triumph3_get_egr_perq_xmt_counters_size(int unit,
                                                       int *num_entries);
extern int soc_tr3_port_config_init(int unit, uint16 dev_id);
extern int soc_tr3_port_lanes_set(int unit, soc_port_t port, int lanes, 
                                  int *old_lanes, int *mode_change);
extern int soc_tr3_port_lanes_get(int unit, soc_port_t port, int *lanes);
extern int soc_tr3_port_lanes_gen(int unit, soc_port_t port, int *lanes, 
                                  soc_block_type_t *pbt);
extern int soc_tr3_port_is_flex_port(int unit, soc_port_t port);
extern int soc_tr3_phy_addr_get(int unit, soc_port_t port, uint16 *int_addr);
extern int soc_triumph3_get_port_mapping(int unit, uint16 dev_id);
extern int soc_triumph3_init_port_mapping(int unit);
extern int soc_triumph3_init_num_cosq(int unit);

extern int soc_tr3_l2x_start(int unit, uint32 flags, sal_usecs_t interval);
extern int soc_tr3_l2x_stop(int unit);
extern int soc_tr3_l2x_running(int unit, uint32 *flags, sal_usecs_t *interval);
extern int soc_tr3_l2_entry_dump(void *user_data, shr_avl_datum_t *datum, 
                                 void *extra_data);
extern int soc_tr3_ext_l2_1_entry_dump(void *user_data, shr_avl_datum_t *datum, 
                                       void *extra_data);
extern int soc_tr3_ext_l2_2_entry_dump(void *user_data, shr_avl_datum_t *datum, 
                                       void *extra_data);
extern int soc_tr3_l2_attach(int unit);
extern int soc_tr3_l2_detach(int unit);
extern int soc_tr3_l2_freeze(int unit);
extern int soc_tr3_l2_thaw(int unit);
extern int soc_tr3_l2_is_frozen(int unit, int *frozen);
extern int soc_tr3_l2_port_age(int unit, soc_reg_t reg0, soc_reg_t reg1);
extern int soc_tr3_l2_hw_index(int unit, int qnum, int uc);
extern int soc_tr3_l2_bulk_age_start(int unit, int interval);
extern int soc_tr3_l2_bulk_age_stop(int unit);
extern void soc_tr3_l2mod_fifo_process(int unit, uint32 flags, 
                                       l2_mod_fifo_entry_t *entry);
extern int soc_tr3_l2_entry_limit_count_update(int unit);
extern int _soc_triumph3_esm_init_adjust_mem_size(int unit);
extern int soc_tr3_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_tr3_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_tr3_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem);
extern int _soc_tr3_port_mem_blk_enabled(int unit, int blk);
extern int soc_triumph3_chip_reset(int unit);
extern int soc_tr3_lls_bmap_alloc(int unit);

/**********************************************************************
 * MMU, COSQ related prototypes.
 */
typedef enum {
    SOC_TR3_SCHED_MODE_UNKNOWN = 0,
    SOC_TR3_SCHED_MODE_STRICT,
    SOC_TR3_SCHED_MODE_WRR,
    SOC_TR3_SCHED_MODE_WDRR
} soc_tr3_sched_mode_e;

typedef enum {
    SOC_TR3_NODE_LVL_ROOT = 0,
    SOC_TR3_NODE_LVL_L0,
    SOC_TR3_NODE_LVL_L1,
    SOC_TR3_NODE_LVL_L2,
    SOC_TR3_NODE_LVL_MAX
} soc_tr3_node_lvl_e;

typedef enum {
    _SOC_TR3_INDEX_STYLE_BUCKET,
    _SOC_TR3_INDEX_STYLE_BUCKET_MODE,
    _SOC_TR3_INDEX_STYLE_WRED,
    _SOC_TR3_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_TR3_INDEX_STYLE_SCHEDULER,
    _SOC_TR3_INDEX_STYLE_PERQ_XMT,
    _SOC_TR3_INDEX_STYLE_UCAST_DROP,
    _SOC_TR3_INDEX_STYLE_UCAST_QUEUE,
    _SOC_TR3_INDEX_STYLE_MCAST_QUEUE,
    _SOC_TR3_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_TR3_INDEX_STYLE_EGR_POOL,
    _SOC_TR3_INDEX_STYLE_COUNT
} soc_tr3_index_style_t;

typedef struct _soc_tr3_axp_lls_config_
{
    int local_port;
    /* Index 0 - UC; MC only or AXP3
     * Index 1 - MC
     * Index 2 - MC only for AXP3
     */
    int base[3];
    int numq[3];
} _soc_tr3_axp_lls_config;

extern _soc_tr3_axp_lls_config _tr3_axp_lls_config[4];

#define _SOC_TR3_NODE_CONFIG_MEM(n)                                 \
    (((n)==SOC_TR3_NODE_LVL_ROOT) ? LLS_PORT_CONFIGm :              \
     (((n) == SOC_TR3_NODE_LVL_L0) ? LLS_L0_CONFIGm :               \
      ((n) == SOC_TR3_NODE_LVL_L1 ? LLS_L1_CONFIGm :  -1  )))

#define _SOC_TR3_NODE_PARENT_MEM(n)                                 \
    (((n)==SOC_TR3_NODE_LVL_L2) ? LLS_L2_PARENTm :                  \
     (((n) == SOC_TR3_NODE_LVL_L1) ? LLS_L1_PARENTm :               \
      (((n) == SOC_TR3_NODE_LVL_L0) ? LLS_L0_PARENTm :  -1  )))

#define _SOC_TR3_NODE_WIEGHT_MEM(n)                                 \
    (((n)==SOC_TR3_NODE_LVL_L0) ? LLS_L0_CHILD_WEIGHT_CFGm  : \
     (((n) == SOC_TR3_NODE_LVL_L1) ? LLS_L1_CHILD_WEIGHT_CFGm : \
      (((n) == SOC_TR3_NODE_LVL_L2) ?  LLS_L2_CHILD_WEIGHT_CFGm : -1 )))

typedef int (*_soc_tr3_bst_hw_cb)(int unit);
extern int soc_tr3_set_bst_callback(int unit, _soc_tr3_bst_hw_cb cb);

typedef int (*soc_tr3_oam_handler_t)(int unit, soc_field_t fault_field);
typedef int (*soc_tr3_oam_ser_handler_t)(int unit, soc_mem_t mem, int index);
extern void soc_tr3_oam_handler_register(int unit, soc_tr3_oam_handler_t handler);
extern void soc_tr3_oam_ser_handler_register(int unit, 
                                            soc_tr3_oam_ser_handler_t handler);
extern int soc_tr3_oam_ser_process(int unit, soc_mem_t mem, int index);

extern int soc_tr3_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);

extern int soc_tr3_lls_init(int unit);
extern int soc_tr3_lls_port_uninit(int unit, soc_port_t port);
extern int soc_tr3_lls_l2_disable(int unit);
extern int soc_tr3_lls_reset(int unit);
extern int soc_tr3_lb_lls_init(int unit);
extern int soc_tr3_lb_lls_port_uninit(int unit, int port);

typedef int (*_soc_tr3_lls_traverse_cb)(int unit, int port, int level, 
                                        int hw_index, void *cookie);
extern int soc_tr3_port_lls_traverse(int unit, soc_port_t port, 
                                     _soc_tr3_lls_traverse_cb cb, void *cookie);
extern int _soc_tr3_invalid_parent_index(int unit, int level);
extern int soc_tr3_sched_weight_get(int unit, int level, int hw_index, int *weight);
extern int soc_tr3_sched_weight_set(int unit, int level, int index, int weight);
extern int soc_tr3_cosq_set_sched_parent(int unit, soc_port_t port,
                                         int node_type, int hw_index,
                                         int parent_hw_idx);
extern int
soc_tr3_cosq_set_sched_child_config(int unit, soc_port_t port, int level, 
                                    int index, int num_spri, int first_child,
                                    int first_mc_child, uint32 ucmap, uint32 spmap);

extern int
soc_tr3_cosq_set_sched_mode(int unit, soc_port_t port, int level, int index,
                            soc_tr3_sched_mode_e mode, int weight);

extern int
soc_tr3_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_spri, int first_child,
                              int first_mc_child, uint32 ucmap, uint32 spmap,
                              soc_tr3_sched_mode_e mode, int weight);

extern int
soc_tr3_cosq_get_sched_child_config(int unit, soc_port_t port, int level, int index,
                                    int *pnum_spri, int *pfirst_child,
                                    int *pfirst_mc_child, uint32 *pucmap,
                                    uint32 *pspmap);

extern int
soc_tr3_cosq_get_sched_mode(int unit, soc_port_t port, int level, int index,
                            soc_tr3_sched_mode_e *pmode, int *weight);

extern int
soc_tr3_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *pnum_spri, int *pfirst_child,
                              int *pfirst_mc_child, uint32 *pucmap,
                              uint32 *pspmap, soc_tr3_sched_mode_e *pmode, 
                              int *weight);

extern int soc_port_hg_capable(int unit, int port);
extern int soc_tr3_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                                 int *pbase, int *pnumq);
extern int soc_tr3_get_mcq_count(int unit);
extern int soc_tr3_get_child_type(int unit, soc_port_t port, int level, 
                                  int *child_type);
extern int soc_tr3_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                                      int *hw_index);
extern int soc_tr3_hsp_sched_hw_index_get(int unit, int port, int lvl, int index,
                                          int offset, int *hw_index);
extern int soc_tr3_mmu_config_init(int unit, int test_only);
extern int soc_tr3_dump_port_lls(int unit, int port);
extern int soc_tr3_dump_port_hsp(int unit, int port);
extern int soc_tr3_hsp_sched_weight_set(int unit, int port, int cosq, int weight, int adjust_cos);
extern int soc_tr3_hsp_sched_weight_get(int unit, int port, int cosq, int *weight);
extern int soc_tr3_hsp_set_sched_config(int unit, soc_port_t port, int cosq,
                                        soc_tr3_sched_mode_e mode, int adjust_cos);
extern int soc_tr3_hsp_get_sched_config(int unit, soc_port_t port, int cosq,
                                        soc_tr3_sched_mode_e *mode);
extern int _soc_tr3_hsp_sched_weight_set(int unit, int port, int index, int cosq, int weight);
extern int _soc_tr3_hsp_sched_weight_get(int unit, int port, int index, int cosq, int *weight);
extern int _soc_tr3_hsp_set_sched_config(int unit, soc_port_t port, int index, int cosq,
                                        soc_tr3_sched_mode_e mode);
extern int _soc_tr3_hsp_get_sched_config(int unit, soc_port_t port, int index, int cosq,
                                        soc_tr3_sched_mode_e *mode);
/**********************************************************************
 * ESM related prototypes.
 */
extern int soc_tr3_esm_init_read_config(int unit);
extern int soc_triumph3_tcam_access(int unit, int op_type, int num_inst,
                                    int num_pre_nop, int num_post_nop,
                                    uint32 *dbus, int *ibus, int num_bits,
                                    int num_searches);
extern void soc_triumph3_esm_init_mem_config(int unit);
extern int soc_tr3_esm_get_ism_mem_size(int unit, uint32 mem_type, uint32 *size);
extern int soc_triumph3_esm_init(int unit);
extern int soc_tr3_search_ext_mem(int unit, soc_mem_t *mem, void *key, 
                                  int *index_ptr);
extern int soc_tr3_get_vbit(int unit, soc_mem_t mem, int index, int *val);
extern int soc_tr3_set_vbit(int unit, soc_mem_t mem, int index, int val);
extern int soc_tr3_nl11k_init(int unit);
extern int soc_tr3_esm_pkt_type_to_slice_map_set(int unit, int pkt_type, 
                                                 int slice);
extern int nl_mdio_prbs_chk(int unit, uint16 mdio_portid, uint16 prbs, 
                            uint32 seed, uint16 crx, uint32 enable);
extern int nl_mdio_prbs_gen(int unit, uint16 mdio_portid, uint16 prbs, 
                            uint32 seed, uint16 crx, uint32 enable);
extern int nl_mdio_prbs_chk_err(int unit, uint16 mdio_portid, uint16 crx);
extern unsigned mdio_portid_get(int unit, unsigned dev_id);
extern int hwtl_find_idle_req_rsp_latency(int unit, int ptr_fme_req, 
                               int test_runtime_seconds, 
                               unsigned *latency_ns);

#define SET_TCAM_PARTITION_AD_INFO(part, et_w, ad_w, e_type, l2_bulk, hit_bit, \
                                   ad_flags) \
do {\
    if (partitions[part].num_entries) {\
        partitions[part].ad_info.et_width = (et_w);\
        partitions[part].ad_info.ad_width = (ad_w);\
        partitions[part].ad_info.entry_type = (e_type);\
        partitions[part].ad_info.l2_bulk_enable = (l2_bulk);\
        partitions[part].ad_info.hit_bit_enable = (hit_bit);\
        partitions[part].flags |= (ad_flags);\
    }\
} while (0)

#define SOC_TR3_INCR_AD_ADDR(cur_row, cur_col, cur_bank, col_decr, err_code) \
do {\
    (cur_col) -= (col_decr);\
    if ((cur_col) < 0) {\
        /* advance to next row */\
        (cur_row) += (8 >> partitions[part].ad_info.et_width);\
        /* check if end of bank */\
        if (((cur_row) << 9) >= ism_bank_size[(cur_bank)]) {\
            (cur_bank)++;\
            if ((cur_bank) >= ism_bank_count) {\
                (err_code) = SOC_E_MEMORY;\
                break;\
            }\
            (cur_row) = 0;\
        }\
        (cur_col) = 11;\
    }\
    (err_code) = SOC_E_NONE;\
} while (0)

#define SOC_TR3_SET_POLICY_TABLE_SIZE(table1, table2, table3, table4, table6, \
                                      policy_width, num_entries)\
do {\
    switch ((policy_width)) {\
    case 1:\
        memState[(table1)].index_max = (num_entries);\
        break;\
    case 2:\
        memState[(table2)].index_max = (num_entries);\
        break;\
    case 3:\
        memState[(table3)].index_max = (num_entries);\
        break;\
    case 4:\
        memState[(table4)].index_max = (num_entries);\
        break;\
    case 6:\
        memState[(table6)].index_max = (num_entries);\
        break;\
    default: \
        /* nothing. All are set to 0 anyway at init */\
        break;\
    }\
} while (0)

#define ROUND_UP_M(x, y) ((x)%(y) ? (x) + (y) - (x)%(y) : (x))

extern int wcmod_esm_serdes_control_set(int unit, int lane, soc_phy_control_t type, 
                                        uint32 *value);
extern int wcmod_esm_serdes_control_get(int unit, int lane, soc_phy_control_t type, 
                                        uint32 *value);
extern int wcmod_esm_serdes_init(int unit);
extern int wcmod_esm_serdes_fifo_reset(int unit);
extern int wcmod_esm_serdes_decouple_prbs_set(int unit, int lane, int tx, int rx, 
                                              int poly, int invert);
extern int soc_tr3_esmif_init(int unit);
extern uint32 tr3_tcam_partition_order[];
extern uint32 tr3_tcam_partition_order_num_entries; 
extern uint32 tr3_tcam_dup_part_order[];
extern uint32 tr3_dup_part_count;

/*
 * block to first physical port array - used in HX4 and TR3
 */
extern int _soc_tr3_b2pp[SOC_MAX_NUM_BLKS];
extern int _soc_tr3_mmu_buffer_cells[SOC_MAX_NUM_DEVICES];
extern uint32 _soc_tr3_tdm_arr[];
extern int _soc_tr3_tdm_size;
/* extern _soc_tr3_flex_tdm_inf_t _soc_tr3_flex_tdm; */

extern int16 _tr3_wc_flex_port_speeds_1[];
extern int16 _tr3_wc_flex_port_lanes_1[];
extern int16 _tr3_wc_def_flex_port_speeds_1[];
extern int16 _tr3_wc_flex_port_speeds_2[];
extern int16 _tr3_wc_flex_port_lanes_2[];
extern int16 _tr3_wc_def_flex_port_speeds_2[];
extern int16 _tr3_wc_flex_port_speeds_3[];
extern int16 _tr3_wc_flex_port_lanes_3[];
extern int16 _tr3_wc_def_flex_port_speeds_3[];
extern int16 _tr3_xc_flex_port_speeds[];
extern int16 _tr3_xc_flex_port_lanes[];
extern int16 _tr3_xc_def_flex_port_speeds[];

extern int _tr3_port_config_id[SOC_MAX_NUM_DEVICES];
extern int _tr3_tdm_opp_spacing[SOC_MAX_NUM_DEVICES];
#define _MAX_TR3_FLEX_PORT_GROUPS    20 /* Total num of XC, WC -1 */
typedef struct {
    uint16 dev_id;
    int16 port_groups;
    int16 ports[_MAX_TR3_FLEX_PORT_GROUPS];
} _soc_tr3_flex_port_data_t;
extern _soc_tr3_flex_port_data_t _tr3_flex_port_data[SOC_MAX_NUM_DEVICES];

extern int _soc_tr3_mmu_init(int unit);
extern int _soc_tr3_misc_init(int unit);

typedef struct {
    uint8 valid;
    int16 flex_port_groups[_MAX_TR3_FLEX_PORT_GROUPS];
    int16 *flex_port_speeds[_MAX_TR3_FLEX_PORT_GROUPS];
    int16 *def_flex_port_speeds[_MAX_TR3_FLEX_PORT_GROUPS];
    int16 *flex_port_lanes[_MAX_TR3_FLEX_PORT_GROUPS];
} _soc_tr3_flex_port_inf_t;

typedef struct {
    uint8 valid;
    uint16 cells_per_row[_MAX_TR3_FLEX_PORT_GROUPS]; /* Represents one cycle (of repetitions) */
    uint16 base_slot[_MAX_TR3_FLEX_PORT_GROUPS]; /* First slot's location in a cycle */
    uint16 cell_offset[_MAX_TR3_FLEX_PORT_GROUPS]; /* Offset between slots in a cycle */
} _soc_tr3_flex_tdm_inf_t;

extern _soc_tr3_flex_tdm_inf_t _soc_tr3_flex_tdm;
extern _soc_tr3_flex_port_inf_t _tr3_flex_port_inf[SOC_MAX_NUM_DEVICES];
extern int _soc_tr3_port_speed_max[];

extern char *_soc_hx4_ser_hwmem_base_info[];

/* defines for bits in the ppa_info data word */
#define _SOC_TR3_L2E_VP            0x80000000 /* Used for virtual-ports */
#define _SOC_TR3_L2E_VALID         0x40000000
#define _SOC_TR3_L2E_STATIC        0x20000000
#define _SOC_TR3_L2E_T             0x10000000

#define _SOC_TR3_L2E_VLAN_MASK   0xfff
#define _SOC_TR3_L2E_VLAN_SHIFT  16
#define _SOC_TR3_L2E_TRUNK_MASK  0xffff
#define _SOC_TR3_L2E_TRUNK_SHIFT 0
#define _SOC_TR3_L2E_MOD_MASK    0xff
#define _SOC_TR3_L2E_MOD_SHIFT   8
#define _SOC_TR3_L2E_PORT_MASK   0xff
#define _SOC_TR3_L2E_PORT_SHIFT  0
#define _SOC_TR3_L2E_DESTINATION_MASK   0xff /* Used for virtual-ports */
#define _SOC_TR3_L2E_DESTINATION_SHIFT  0 /* Used for virtual-ports */


typedef struct _soc_tr3_l2e_ppa_info_s {
    uint32      data;
    sal_mac_addr_t mac;
} _soc_tr3_l2e_ppa_info_t;

typedef struct _soc_tr3_l2e_ppa_vlan_s {
    int         vlan_min[VLAN_ID_MAX + 1];
    int         vlan_max[VLAN_ID_MAX + 1];
} _soc_tr3_l2e_ppa_vlan_t;

typedef enum soc_tr3_drop_limit_alpha_value_e {
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */                                          
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_TR3_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_tr3_drop_limit_alpha_value_t;

extern int soc_triumph3_ext_l2_entry_update(int unit, soc_mem_t mem, int index, 
                                            void *entry);

#define _SOC_TR3_MGMT_PHY_PORT        37
#define _SOC_HX4_MGMT_PHY_PORT        49
#define _SOC_HX4_XMAC_FIFO_CHK_PKT_SZ 9216
#define _SOC_HX4_FIFO_CHK_MAX_PORTS   86

extern 
int _soc_tr3_ur_port_reset(int unit, int num_ports, soc_port_t port,
                           soc_block_type_t pbt);
extern 
int _soc_hx4_xmac_cclk_config(int unit);

extern int soc_tr3_esm_recovery_start(int unit);
extern int soc_tr3_esm_recovery_stop(int unit);
extern int
soc_tr3_mmu_config_shared_buf_recalc(int unit, uint32 spid, int shared_size,
                                     int flags);
int soc_tr3_mmu_get_shared_size(int unit, int *shared_size);

#if defined(SER_TR_TEST_SUPPORT)
extern soc_error_t
soc_tr3_ser_error_injection_support (int unit, soc_mem_t mem,
                                     int pipe);
extern soc_error_t
soc_tr3_ser_inject_error (int unit, uint32 flags, soc_mem_t mem,
                          int pipe_target, int block, int index); 
extern soc_error_t
soc_tr3_ser_test_mem(int unit, soc_mem_t mem, _soc_ser_test_t test_type,
                     int cmd);
extern soc_error_t
soc_tr3_ser_test (int unit, _soc_ser_test_t test_type);

#endif /*defined(SER_TR_TEST_SUPPORT*/

extern int soc_tr3_lls_sched_init(int unit);
extern int soc_tr3_mmu_register_store(int unit);
extern int soc_tr3_mmu_register_restore(int unit);
extern int soc_tr3_mmu_store_alloc(int unit);
extern void soc_tr3_mmu_store_free(int unit);
extern int soc_tr3_mmu_memory_store(int unit);
extern int soc_tr3_mmu_memory_restore(int unit);
extern int soc_tr3_mmu_init_restore(int unit);

extern soc_pbmp_t
soc_tr3_get_control_port_bitmap(int unit);
#endif /* !_SOC_TRIUMPH3_H_ */
