/**
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ipmc.c
 * Purpose:     Triumph3 IPMC implementation.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) 
#if defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/types.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/firebolt.h>

#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/subport.h>
#include <soc/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
#include <soc/saber2.h>
#include <bcm_int/esw/saber2.h>
#endif
#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif
#ifdef BCM_KATANA_SUPPORT
#include <bcm_int/esw/katana.h>
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/common/multicast.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */

/* Structures for managing REPL_HEAD table resources */
int _bcm_kt_ipmc_subscriber_queues_get(int unit, int ipmc_id,
                                       SHR_BITDCL *q_vec,
                                       uint32 *q_repl_ptrs,
                                       int *q_num);

typedef struct _tr3_repl_head_free_block_s {
    int index; /* Starting index of a free block of REPL_HEAD table entries */
    int size;  /* Number of entries in the free block */
    struct _tr3_repl_head_free_block_s *next; /* Pointer to next free block */
} _tr3_repl_head_free_block_t;

typedef struct _tr3_repl_head_info_s {
    _tr3_repl_head_free_block_t **free_list_array;
                              /* Array of lists of free blocks */
    int array_size;           /* Number of lists in the array */
} _tr3_repl_head_info_t;

typedef struct _kt2_repl_grp_info_s {
   int mb_index;
   int lb_index;
   soc_mem_t mb_mem;
   soc_mem_t lb_mem;
   soc_field_t mbmp;
   soc_field_t lbmp;
} _kt2_repl_grp_info_t;

typedef struct _tr3_if_updated_s {
    bcm_if_t *if_array_del;   /* the interface array to be deleted */   
    int       if_count_del;   /* size of the interface array to be deleted */
    bcm_if_t *if_array_new;   /* the new interface array to be added */
    int       if_count_new;   /* size of the new interface array to be added */
} _tr3_if_updated_t;
static _tr3_repl_head_info_t *_tr3_repl_head_info[BCM_MAX_NUM_UNITS];

#define REPL_HEAD_FREE_LIST(_unit_, _index_) \
    _tr3_repl_head_info[_unit_]->free_list_array[_index_]

/* Structures for managing REPL_LIST table resources */

typedef struct _tr3_repl_list_entry_info_s {
    SHR_BITDCL *bitmap_entries_used; /* A bitmap indicating which REPL_LIST
                                        entries are used */
    int num_entries; /* Total number of entries in REPL_LIST table */
} _tr3_repl_list_entry_info_t;

static _tr3_repl_list_entry_info_t \
           *_tr3_repl_list_entry_info[BCM_MAX_NUM_UNITS];

static uint8 reserve_multicast_resources[BCM_MAX_NUM_UNITS] = {0};

#define REPL_LIST_ENTRY_USED_GET(_u_, _i_) \
        SHR_BITGET(_tr3_repl_list_entry_info[_u_]->bitmap_entries_used, _i_)
#define REPL_LIST_ENTRY_USED_SET(_u_, _i_) \
        SHR_BITSET(_tr3_repl_list_entry_info[_u_]->bitmap_entries_used, _i_)
#define REPL_LIST_ENTRY_USED_CLR(_u_, _i_) \
        SHR_BITCLR(_tr3_repl_list_entry_info[_u_]->bitmap_entries_used, _i_)

/* Structures containing replication info */

typedef struct _tr3_repl_port_info_s {
    int *intf_count; /* Array of interface counts, one per replication group */
} _tr3_repl_port_info_t;

typedef struct _tr3_repl_info_s {
    int num_repl_groups; /* Number of replication groups on this device */
    uint32 num_intf;     /* Number of interfaces on this device */
    _bcm_repl_list_info_t *repl_list_info;
                         /* Linked list of replication lists */
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    _tr3_repl_port_info_t *port_info[SOC_MAX_NUM_PP_PORTS];
#else
    _tr3_repl_port_info_t *port_info[SOC_MAX_NUM_PORTS];
#endif
                         /* Per port replication info */
    int *l3_intf_next_hop_ipmc; /* Array of next hop indices, one for each
                            L3 interface that's added to an IPMC group */ 
    int *l3_intf_next_hop_trill; /* Array of next hop indices, one for each
                            L3 interface that's added to a Trill group */ 
} _tr3_repl_info_t;

static _tr3_repl_info_t *_tr3_repl_info[BCM_MAX_NUM_UNITS];

#ifdef BCM_KATANA2_SUPPORT
typedef struct _kt2_rep_regs_s {
      soc_field_t last_ptr;
      soc_mem_t   mem;
}_kt2_rep_regs_t;

_kt2_rep_regs_t kt2_rqe_rep_regs[] = { 
   { INVALIDf,    INVALIDm},
    {PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT5_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT6_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT7_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT8_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT9_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT10_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT11_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT12_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT13_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT14_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT15_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT16_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT17_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT18_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT19_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT20_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT21_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT22_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT23_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT24_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT25_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT26_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT27_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT28_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT29_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT30_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT31_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT32_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT33_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT34_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT35_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT36_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT37_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT38_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT39_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT40_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT41_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT42_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT43_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT44_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT45_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT46_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT47_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT48_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT49_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT50_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT51_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT52_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT53_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT54_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT55_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT56_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT57_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT58_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT59_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT60_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT61_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT62_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT63_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT64_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT65_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT66_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT67_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT68_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT69_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT70_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT71_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT72_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT73_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT74_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT75_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT76_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT77_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT78_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT79_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT80_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT81_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT82_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT83_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT84_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT85_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT86_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT87_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT88_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT89_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT90_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT91_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT92_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT93_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT94_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT95_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT96_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT97_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT98_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT99_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT100_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT101_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT102_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT103_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT104_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT105_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT106_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT107_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT108_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT109_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT110_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT111_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT112_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT113_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT114_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT115_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT116_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT117_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT118_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT119_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT120_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT121_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT122_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT123_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT124_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT125_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT126_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT127_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT128_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT129_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT130_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT131_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT132_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT133_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT134_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT135_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT136_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT137_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT138_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT139_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT140_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT141_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT142_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT143_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT144_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT145_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT146_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT147_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT148_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT149_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT150_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT151_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT152_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT153_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT154_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT155_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT156_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT157_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT158_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT159_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT160_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT161_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT162_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT163_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT164_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT165_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT166_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT167_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT168_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT169_LASTf, MMU_IPMC_GROUP_TBL4m},
};
#endif 

#ifdef BCM_SABER2_SUPPORT
_kt2_rep_regs_t sb2_rqe_rep_regs[] = { 
   { INVALIDf,    INVALIDm},
    {PORT1_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT2_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT3_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT4_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT5_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT6_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT7_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT8_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT9_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT10_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT11_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT12_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT13_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT14_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT15_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT16_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT17_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT18_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT19_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT20_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT21_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT22_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT23_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT24_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT25_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT26_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT27_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT28_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT29_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT30_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT31_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT32_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT33_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT34_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT35_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT36_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT37_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT38_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT39_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT40_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT41_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT42_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT43_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT44_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT45_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT46_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT47_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT48_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT49_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT50_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT51_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT52_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT53_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT54_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT55_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT56_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT57_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT58_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT59_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT60_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT61_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT62_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT63_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT64_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT65_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT66_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT67_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT68_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT69_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT70_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT71_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT72_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT73_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT74_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT75_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT76_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT77_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT78_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT79_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT80_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT81_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT82_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT83_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT84_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT85_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT86_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT87_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT88_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT89_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT90_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT91_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT92_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT93_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT94_LASTf, MMU_IPMC_GROUP_TBLm},
    {PORT95_LASTf, MMU_IPMC_GROUP_TBLm},
};
typedef struct _sb2_repl_queue_info_s {
  int32 *queue_count;           /* # VLANs the queue repl to */
  int32 *queue_count_int;       /* # Count of internally buffered queues */
  int32 *queue_count_ext;       /* # Count of externally buffered queues */
}  _sb2_repl_queue_info_t;
typedef struct _sb2_extq_repl_info_s {
   int ipmc_size;
   uint32 queue_num;
   uint32 extq_list_total;
   uint32 extq_repl_max;
   uint32 *bitmap_entries_used; /* free entries of
   MMU_EXT_MC_QUEUE_LIST  table */
   _bcm_repl_list_info_t *repl_extq_list_info;
   _sb2_repl_queue_info_t *mcgroup_info;
   _sb2_repl_queue_info_t *port_info[SOC_MAX_NUM_PP_PORTS];
   _bcm_ext_q_ipmc_t *ext_ipmc_list;
} _sb2_extq_repl_info_t;
static _sb2_extq_repl_info_t *_sb2_extq_repl_info[BCM_MAX_NUM_UNITS];




#define IPMC_SB2_NHOP_ID_MIN 16384 
#define IPMC_SB2_EGRESS_ID_MIN 8192 
#define IPMC_SB2_REPLICATION_INDEX_MAX 32767 
#define IPMC_EXTQ_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _sb2_extq_repl_info[_u_]->queue_num)) \
            { return BCM_E_PARAM; }
#define IPMC_EXTQ_REPL_INIT(unit) \
        if (_sb2_extq_repl_info[unit] == NULL) { return BCM_E_INIT; }
#define IPMC_EXTQ_LIST_TOTAL(_u_) \
        _sb2_extq_repl_info[_u_]->extq_list_total
#define IPMC_EXTQ_REPL_MAX(_u_) \
        _sb2_extq_repl_info[_u_]->extq_repl_max
#define IPMC_EXTQ_TOTAL(_u_) \
        _sb2_extq_repl_info[_u_]->queue_num
#define IPMC_EXTQ_REPL_VE_USED_GET(_u_, _i_) \
        SHR_BITGET(_sb2_extq_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_EXTQ_REPL_VE_USED_SET(_u_, _i_) \
        SHR_BITSET(_sb2_extq_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_EXTQ_REPL_VE_USED_CLR(_u_, _i_) \
        SHR_BITCLR(_sb2_extq_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_EXTQ_REPL_LIST_INFO(_u_) \
    _sb2_extq_repl_info[_u_]->repl_extq_list_info
#define IPMC_EXTQ_GROUP_INFO(_u_) 		\
	_sb2_extq_repl_info[_u_]->mcgroup_info
#define IPMC_EXTQ_REPL_QUEUE_COUNT(_u_, _ipmc_id_) \
        _sb2_extq_repl_info[_u_]->mcgroup_info->queue_count[_ipmc_id_]
#define IPMC_EXTQ_REPL_QUEUE_COUNT_INT(_u_, _ipmc_id_) \
        _sb2_extq_repl_info[_u_]->mcgroup_info->queue_count_int[_ipmc_id_]
#define IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(_u_, _ipmc_id_) \
        _sb2_extq_repl_info[_u_]->mcgroup_info->queue_count_ext[_ipmc_id_]
#define IPMC_EXTQ_REPL_PORT_INFO(_u_, _p_) \
        _sb2_extq_repl_info[_u_]->port_info[_p_]
#define IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(_u_, _p_, _ipmc_id_) \
        _sb2_extq_repl_info[_u_]->port_info[_p_]->queue_count[_ipmc_id_]

int
_bcm_sb2_ipmc_subscriber_qvec_get(int unit, int ipmc_id, 
                                  SHR_BITDCL *q_vec);
int
_bcm_sb2_ipmc_subscriber_add_queue_node(int unit, int ipmc_id, int queue_id,  
                                    _bcm_ext_repl_q_list_t *queue_node) ;
#endif
int
bcm_tr3_ipmc_repl_detach(int unit);
#define REPL_LOCK(_u_)                         \
    {                                          \
        soc_mem_lock(_u_, MMU_REPL_LIST_TBLm); \
    }
#define REPL_UNLOCK(_u_)                         \
    {                                            \
        soc_mem_unlock(_u_, MMU_REPL_LIST_TBLm); \
    }

#define REPL_INIT(unit) \
        if (_tr3_repl_info[unit] == NULL) { return BCM_E_INIT; }

#define REPL_GROUP_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _tr3_repl_info[_u_]->num_repl_groups)) \
            { return BCM_E_PARAM; }

#define REPL_INTF_TOTAL(_u_) _tr3_repl_info[_u_]->num_intf

#define REPL_LIST_INFO(_u_) _tr3_repl_info[_u_]->repl_list_info

#define REPL_PORT_GROUP_INTF_COUNT(_u_, _p_, _group_id_) \
    _tr3_repl_info[_u_]->port_info[_p_]->intf_count[_group_id_]

#define REPL_L3_INTF_NEXT_HOP_IPMC(_u_, _intf_) \
    _tr3_repl_info[_u_]->l3_intf_next_hop_ipmc[_intf_]
#define REPL_L3_INTF_NEXT_HOP_TRILL(_u_, _intf_) \
    _tr3_repl_info[_u_]->l3_intf_next_hop_trill[_intf_]

#if defined(BCM_KATANA2_SUPPORT)
#define REPL_PORT_CHECK(unit, port) \
    { \
        if (SOC_IS_KATANA2(unit)) { \
            if (!IS_PORT(unit, port) && \
                !IS_AXP_PORT(unit, port) && \
                !(port >= SOC_INFO(unit).pp_port_index_min) && \
                !(port <= SOC_INFO(unit).pp_port_index_max)) { \
                return BCM_E_PARAM; \
            } \
        }  else { \
            if (!IS_PORT(unit, port) && !IS_AXP_PORT(unit, port)) { \
                return BCM_E_PARAM; \
            } \
        } \
    }
#else
#define REPL_PORT_CHECK(unit, port) \
    { \
        if (!IS_PORT(unit, port) && !IS_AXP_PORT(unit, port)) { \
            return BCM_E_PARAM; \
        } \
    }
#endif

#define _BCM_QOS_MAP_TYPE_MASK                  0x3ff
#define _BCM_QOS_MAP_SHIFT                      10
#define _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP  7

#define REPL_NH_INDEX_UNALLOCATED -1
#define REPL_NH_INDEX_L3_EGRESS_ALLOCATED -2

/* --------------------------------------------------------------
 * The following set of routines manage REPL_HEAD table resource.
 * --------------------------------------------------------------
 */

/*
 * Function:
 *      _bcm_tr3_repl_head_info_deinit
 * Purpose:
 *      De-initialize replication head info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void 
_bcm_tr3_repl_head_info_deinit(int unit)
{
    int i;
    _tr3_repl_head_free_block_t *block_ptr;
    _tr3_repl_head_free_block_t *next_block_ptr;

    if (NULL != _tr3_repl_head_info[unit]) {
        if (NULL != _tr3_repl_head_info[unit]->free_list_array) {
            for (i = 0; i < _tr3_repl_head_info[unit]->array_size; i++) {
                block_ptr = REPL_HEAD_FREE_LIST(unit, i);
                while (NULL != block_ptr) {
                    next_block_ptr = block_ptr->next;
                    sal_free(block_ptr);
                    block_ptr = next_block_ptr;
                }
                REPL_HEAD_FREE_LIST(unit, i) = NULL;
            }
            sal_free(_tr3_repl_head_info[unit]->free_list_array);
            _tr3_repl_head_info[unit]->free_list_array = NULL;
            _tr3_repl_head_info[unit]->array_size = 0;
        }
        sal_free(_tr3_repl_head_info[unit]);
        _tr3_repl_head_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_tr3_repl_head_info_init
 * Purpose:
 *      Initialize replication head info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_head_info_init(int unit)
{
    int max_array_index = 0;
    int num_of_ports = 0;

    if (NULL == _tr3_repl_head_info[unit]) {
        _tr3_repl_head_info[unit] = sal_alloc(sizeof(_tr3_repl_head_info_t),
                "repl_head_info");
        if (NULL == _tr3_repl_head_info[unit]) {
            _bcm_tr3_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_repl_head_info[unit], 0, sizeof(_tr3_repl_head_info_t));

    if (NULL == _tr3_repl_head_info[unit]->free_list_array) {
        /* Each element of the array is a linked list of free blocks.
         * Array element N is a linked list of free blocks of size N.
         * When allocating a block of REPL_HEAD table entries, the max number
         * of entries needed is equal to the max number of members in a
         * replication group. This will also be the max index of the array. 
         * Of course, the REPL_HEAD table may contain bigger blocks of free
         * entries. Array element 0 wil be a linked list of free blocks with 
         * size greater than the max number of members in a replication group.
         */
        if (SOC_IS_APACHE(unit)) {
            max_array_index = soc_mem_field_length(unit,
                                  MMU_REPL_GROUP_INFO0m, PIPE_MEMBER_BMPf);
        } else
        if (soc_mem_field_valid(unit, MMU_REPL_GROUP_INFO0m, PIPE_MEMBER_BMPf) &&
            soc_mem_field_valid(unit, MMU_REPL_GROUP_INFO1m, PIPE_MEMBER_BMPf)) {
            max_array_index =
                soc_mem_field_length(unit, MMU_REPL_GROUP_INFO0m,
                        PIPE_MEMBER_BMPf) +
                soc_mem_field_length(unit, MMU_REPL_GROUP_INFO1m,
                        PIPE_MEMBER_BMPf);
                
        } else if (soc_mem_field_valid(unit, MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTS_119_0f) &&
                soc_mem_field_valid(unit, MMU_REPL_GRP_TBL1m, MEMBER_BMP_PORTS_169_120f)) {
                max_array_index =
                (soc_mem_field_length(unit, MMU_REPL_GRP_TBL0m,
                                      MEMBER_BMP_PORTS_119_0f) +
                 soc_mem_field_length(unit, MMU_REPL_GRP_TBL1m,
                                       MEMBER_BMP_PORTS_169_120f));
        } else if (soc_mem_field_valid(unit, MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTSf)){       
            max_array_index = soc_mem_field_length(unit, MMU_REPL_GRP_TBL0m,
                    MEMBER_BMP_PORTSf);
	}else if (soc_mem_field_valid(unit, MMU_REPL_GRP_TBLm, MEMBER_BMP_PORTSf)){       
            max_array_index = soc_mem_field_length(unit, MMU_REPL_GRP_TBLm,
                    MEMBER_BMP_PORTSf);
	}else{  
                max_array_index = soc_mem_field_length(unit, MMU_REPL_GROUPm,
                        MEMBER_BITMAPf);
            }
        _tr3_repl_head_info[unit]->free_list_array =
            sal_alloc(sizeof(_tr3_repl_head_free_block_t *) *
                      (max_array_index + 1), "repl head free list array");
        if (NULL == _tr3_repl_head_info[unit]->free_list_array) {
            _bcm_tr3_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }
        _tr3_repl_head_info[unit]->array_size = max_array_index + 1;
    }
    sal_memset(_tr3_repl_head_info[unit]->free_list_array, 0,
            sizeof(_tr3_repl_head_free_block_t *) *
            _tr3_repl_head_info[unit]->array_size);

    if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {
        if (!SOC_WARM_BOOT(unit)) {
            /* Clear replication head table */
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_REPL_HEAD_TBLm, MEM_BLOCK_ALL, 0));
        }
    } else {
        REPL_HEAD_FREE_LIST(unit, 0) = 
            sal_alloc(sizeof(_tr3_repl_head_free_block_t), "repl head free block");
        if (NULL == REPL_HEAD_FREE_LIST(unit, 0)) {
            _bcm_tr3_repl_head_info_deinit(unit);
            return BCM_E_MEMORY;
        }
        
        /* Initially, there is only one free block, starting at entry 
         * (1 + max_array_index) of the REPL_HEAD table and ending 
         * at the last entry. Entry 0 is reserved.
         * Entries from (soc_mem_index_max(unit,MMU_REPL_HEAD_TBLm) - 
         * count + 1) to  soc_mem_index_max(unit,MMU_REPL_HEAD_TBLm) 
         * are reserved for swapping the REPL_HEADm of a replication group 
         * when there is no resource for deleting a port 
         * from the replication group.
         */
        reserve_multicast_resources[unit] =
            soc_property_get(unit, spn_RESERVE_MULTICAST_RESOURCES, 0) ? 1 : 0;
        if (reserve_multicast_resources[unit]) {
            BCM_PBMP_COUNT(PBMP_ALL(unit), num_of_ports);
        }
        REPL_HEAD_FREE_LIST(unit, 0)->index = 1;
        REPL_HEAD_FREE_LIST(unit, 0)->size = soc_mem_index_max(unit,
                MMU_REPL_HEAD_TBLm) - num_of_ports;
        REPL_HEAD_FREE_LIST(unit, 0)->next = NULL;

        /* Clear entry 0 of REPL_HEAD table */
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, MMU_REPL_HEAD_TBLm, MEM_BLOCK_ALL,
                    0, soc_mem_entry_null(unit, MMU_REPL_HEAD_TBLm)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_repl_head_block_free
 * Purpose:
 *      Free a block of REPL_HEAD table entries.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      index - (IN) Index of the first entry of the block.
 *      size  - (IN) Size of the block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_head_block_free(int unit, int index, int size)
{
    int i;
    int block_index, block_size;
    int coalesced_index, coalesced_size;
    _tr3_repl_head_free_block_t *block_ptr;
    _tr3_repl_head_free_block_t *prev_block_ptr;
    _tr3_repl_head_free_block_t *next_block_ptr;
    _tr3_repl_head_free_block_t *coalesced_block_ptr;

    if (size <= 0) {
        return BCM_E_INTERNAL;
    }

    /* First, coalesce the block with any existing free blocks
     * that are contiguous with the block.
     */ 
    coalesced_index = index;
    coalesced_size = size;
    for (i = 0; i < _tr3_repl_head_info[unit]->array_size; i++) {
        block_ptr = REPL_HEAD_FREE_LIST(unit, i);
        prev_block_ptr = NULL;
        while (NULL != block_ptr) {
            block_index = block_ptr->index;
            block_size = block_ptr->size;
            next_block_ptr = block_ptr->next;
            if ((block_index + block_size) == coalesced_index) {
                coalesced_index = block_index;
                coalesced_size += block_size;
                if (block_ptr == REPL_HEAD_FREE_LIST(unit, i)) {
                    REPL_HEAD_FREE_LIST(unit, i) = next_block_ptr;
                } else {
                    /* 
                     * In the following line of code, Coverity thinks the
                     * prev_block_ptr may still be NULL when dereferenced.
                     * This situation will never occur because 
                     * if block_ptr is not pointing to the head of the 
                     * linked list, prev_block_ptr would not be NULL.
                     */
                    /* coverity[var_deref_op : FALSE] */
                    prev_block_ptr->next = next_block_ptr;
                } 
                sal_free(block_ptr);
            } else if ((coalesced_index + coalesced_size) == block_index) {
                coalesced_size += block_size;
                if (block_ptr == REPL_HEAD_FREE_LIST(unit, i)) {
                    REPL_HEAD_FREE_LIST(unit, i) = next_block_ptr;
                } else {
                    /* 
                     * In the following line of code, Coverity thinks the
                     * prev_block_ptr may still be NULL when dereferenced.
                     * This situation will never occur because 
                     * if block_ptr is not pointing to the head of the 
                     * linked list, prev_block_ptr would not be NULL.
                     */
                    /* coverity[var_deref_op : FALSE] */
                    prev_block_ptr->next = next_block_ptr;
                } 
                sal_free(block_ptr);
            } else {
                prev_block_ptr = block_ptr;
            }
            block_ptr = next_block_ptr;
        }
    }

    /* Insert coalesced free block */
    coalesced_block_ptr = sal_alloc(sizeof(_tr3_repl_head_free_block_t),
            "coalesced repl head free block");
    if (NULL == coalesced_block_ptr) {
        return BCM_E_MEMORY;
    }
    coalesced_block_ptr->index = coalesced_index;
    coalesced_block_ptr->size = coalesced_size;
    if (coalesced_size > (_tr3_repl_head_info[unit]->array_size - 1)) {
        /* Insert into free list 0 */
        coalesced_block_ptr->next = REPL_HEAD_FREE_LIST(unit, 0);
        REPL_HEAD_FREE_LIST(unit, 0) = coalesced_block_ptr;
    } else {
        coalesced_block_ptr->next = REPL_HEAD_FREE_LIST(unit, coalesced_size);
        REPL_HEAD_FREE_LIST(unit, coalesced_size) = coalesced_block_ptr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_repl_head_block_alloc
 * Purpose:
 *      Allocate a free block of REPL_HEAD table entries.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      size - (IN) Size of free block requested.
 *      index - (OUT) Index of the first entry of the free block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_head_block_alloc(int unit, int size, int *index)
{
    int max_array_index;
    int i;
    int block_index, block_size;
    _tr3_repl_head_free_block_t *next_block_ptr;

    if (size == 0) {
        return BCM_E_PARAM;
    }
    if (NULL == index) {
        return BCM_E_PARAM;
    }

    max_array_index = _tr3_repl_head_info[unit]->array_size - 1;
    for (i = size; i <= max_array_index; i++) {
        if (NULL != REPL_HEAD_FREE_LIST(unit, i)) {
           block_index = REPL_HEAD_FREE_LIST(unit, i)->index; 
           block_size = REPL_HEAD_FREE_LIST(unit, i)->size; 
           next_block_ptr = REPL_HEAD_FREE_LIST(unit, i)->next; 
           sal_free(REPL_HEAD_FREE_LIST(unit, i));
           REPL_HEAD_FREE_LIST(unit, i) = next_block_ptr;

           /* If the obtained free block contains more entries
            * than requested, insert the remainder back into
            * the free list array.
            */
           if (block_size > size) {
               BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_block_free(unit,
                           block_index + size, block_size - size));
           }

           *index = block_index;
           return BCM_E_NONE;
        }
    }

    /* Get free block from free list 0 */
    if (NULL != REPL_HEAD_FREE_LIST(unit, 0)) {
        block_index = REPL_HEAD_FREE_LIST(unit, 0)->index;
        block_size = REPL_HEAD_FREE_LIST(unit, 0)->size;
        next_block_ptr = REPL_HEAD_FREE_LIST(unit, 0)->next;
        if (block_size < size) {
            /* Free blocks on list 0 should never be
             * smaller than requested size.
             */
            return BCM_E_INTERNAL;
        } 

        sal_free(REPL_HEAD_FREE_LIST(unit, 0));
        REPL_HEAD_FREE_LIST(unit, 0) = next_block_ptr;

        /* If the obtained free block contains more entries
         * than requested, insert the remainder back into
         * the free list array.
         */
        if (block_size > size) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_block_free(unit,
                        block_index + size, block_size - size));
        }

        *index = block_index;
        return BCM_E_NONE;
    }

    /* No free block of sufficient size can be found */
    return BCM_E_RESOURCE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_tr3_repl_head_block_insert
 * Purpose:
 *      Insert a free block of REPL_HEAD table entries into free array list.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      index - (IN) Index of the first entry of the block.
 *      size - (IN) Size of block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_head_block_insert(int unit, int index, int size)
{
    _tr3_repl_head_free_block_t *block_ptr;

    block_ptr = sal_alloc(sizeof(_tr3_repl_head_free_block_t),
            "repl head free block");
    if (NULL == block_ptr) {
        return BCM_E_MEMORY;
    }
    block_ptr->index = index;
    block_ptr->size = size;
    if (size > (_tr3_repl_head_info[unit]->array_size - 1)) {
        /* Insert into free list 0 */
        block_ptr->next = REPL_HEAD_FREE_LIST(unit, 0);
        REPL_HEAD_FREE_LIST(unit, 0) = block_ptr;
    } else {
        block_ptr->next = REPL_HEAD_FREE_LIST(unit, size);
        REPL_HEAD_FREE_LIST(unit, size) = block_ptr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_repl_head_block_used_set
 * Purpose:
 *      Mark a block of REPL_HEAD table entries as used.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      index - (IN) Index of the first entry of the block.
 *      size - (IN) Size of block.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_head_block_used_set(int unit, int index, int size)
{
    int i;
    _tr3_repl_head_free_block_t *prev_block_ptr;
    _tr3_repl_head_free_block_t *block_ptr;
    int block_index, block_size, sub_block_size;

    for (i = 0; i < _tr3_repl_head_info[unit]->array_size; i++) {
        block_ptr = REPL_HEAD_FREE_LIST(unit, i);
        prev_block_ptr = NULL;
        while (NULL != block_ptr) {
            block_index = block_ptr->index;
            block_size = block_ptr->size;
            if ((index >= block_index) &&
                    ((index + size) <= (block_index + block_size))) {

                /* This free block contains the block to be marked as used.
                 * Remove this free block from linked list.
                 */
                if (block_ptr == REPL_HEAD_FREE_LIST(unit, i)) {
                    REPL_HEAD_FREE_LIST(unit, i) = block_ptr->next;
                } else {
                    /* 
                     * In the following line of code, Coverity thinks the
                     * prev_block_ptr may still be NULL when dereferenced.
                     * This situation will never occur because 
                     * if block_ptr is not pointing to the head of the 
                     * linked list, prev_block_ptr would not be NULL.
                     */
                    /* coverity[var_deref_op : FALSE] */
                    prev_block_ptr->next = block_ptr->next;
                }
                sal_free(block_ptr);

                /* This free block contains up to 3 sub-blocks: the sub-block
                 * to be marked as used, and the sub-blocks before and after
                 * it. The sub-blocks before and after the used sub-block
                 * need to be inserted back into the free list array.
                 */

                /* Insert the sub-block before the used sub-block back into
                 * the free list array.
                 */
                sub_block_size = index - block_index;
                if (sub_block_size > 0) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr3_repl_head_block_insert(unit, block_index,
                                                         sub_block_size));
                }

                /* Insert the sub-block after the used sub-block back into
                 * the free list array.
                 */
                sub_block_size = (block_index + block_size) - (index + size);
                if (sub_block_size > 0) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr3_repl_head_block_insert(unit, index + size,
                                                         sub_block_size));
                }

                return BCM_E_NONE;
            }

            prev_block_ptr = block_ptr;
            block_ptr = block_ptr->next;
        }
    }

    /* The block to be marked used is not found among the free blocks. */
    return BCM_E_NOT_FOUND;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_tr3_repl_head_entry_info_get
 * Purpose:
 *      Get replication head table size and number of entries in the free block.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      size - (out) Replication head table size.
 *      free - (out) Number of entries in the Replication head table.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_tr3_repl_head_entry_info_get(int unit, int *free)
{
    int temp_free = 0;
    _tr3_repl_head_free_block_t *free_block;
    int i = 0;

    if (NULL == free) {
        return BCM_E_PARAM;
    } 

    if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {
        return BCM_E_UNAVAIL;
    }

    IPMC_LOCK(unit);  
    if (_tr3_repl_head_info[unit] != NULL) {
        if (_tr3_repl_head_info[unit]->free_list_array != NULL) {
            for (i = 0; i < _tr3_repl_head_info[unit]->array_size; i++) {
                free_block = REPL_HEAD_FREE_LIST(unit, i);
                while (NULL != free_block){
                    temp_free  = temp_free + free_block->size;
                    free_block = free_block->next;
                }
            }
        }
    }
    IPMC_UNLOCK(unit);

    if (0 > temp_free){        
        return BCM_E_INTERNAL;
    }

    *free = temp_free;
    
    return BCM_E_NONE;
}


/* --------------------------------------------------------------
 * The following set of routines manage REPL_LIST table resource.
 * --------------------------------------------------------------
 */

/*
 * Function:
 *      _bcm_tr3_repl_list_entry_info_deinit
 * Purpose:
 *      De-initialize replication list entry info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void 
_bcm_tr3_repl_list_entry_info_deinit(int unit)
{
    if (NULL != _tr3_repl_list_entry_info[unit]) {
        if (NULL != _tr3_repl_list_entry_info[unit]->bitmap_entries_used) {
            sal_free(_tr3_repl_list_entry_info[unit]->bitmap_entries_used);
            _tr3_repl_list_entry_info[unit]->bitmap_entries_used = NULL;
        }
        sal_free(_tr3_repl_list_entry_info[unit]);
        _tr3_repl_list_entry_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_tr3_repl_list_entry_info_init
 * Purpose:
 *      Initialize replication list entry info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_list_entry_info_init(int unit)
{
    int alloc_size;

    if (NULL == _tr3_repl_list_entry_info[unit]) {
        _tr3_repl_list_entry_info[unit] =
            sal_alloc(sizeof(_tr3_repl_list_entry_info_t),
                    "repl_list_entry_info");
        if (NULL == _tr3_repl_list_entry_info[unit]) {
            _bcm_tr3_repl_list_entry_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_repl_list_entry_info[unit], 0,
            sizeof(_tr3_repl_list_entry_info_t));

    _tr3_repl_list_entry_info[unit]->num_entries = soc_mem_index_count(unit,
                MMU_REPL_LIST_TBLm);
    alloc_size = SHR_BITALLOCSIZE(_tr3_repl_list_entry_info[unit]->num_entries);
    if (NULL == _tr3_repl_list_entry_info[unit]->bitmap_entries_used) {
        _tr3_repl_list_entry_info[unit]->bitmap_entries_used =
            sal_alloc(alloc_size, "repl list bitmap_entries_used");
        if (NULL == _tr3_repl_list_entry_info[unit]->bitmap_entries_used) {
            _bcm_tr3_repl_list_entry_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_repl_list_entry_info[unit]->bitmap_entries_used, 0,
            alloc_size);

    /* Reserve REPL_LIST table entry 0 */
    REPL_LIST_ENTRY_USED_SET(unit, 0);

    /* Clear entry 0 of REPL_LIST table */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, MMU_REPL_LIST_TBLm, MEM_BLOCK_ALL,
                0, soc_mem_entry_null(unit, MMU_REPL_LIST_TBLm)));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_repl_list_entry_free
 * Purpose:
 *      Free a REPL_LIST table entry.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      entry_index - (IN)Index of the entry to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_repl_list_entry_free(int unit, int entry_index)
{
    if (!REPL_LIST_ENTRY_USED_GET(unit, entry_index)) {
        return BCM_E_INTERNAL;
    }

    REPL_LIST_ENTRY_USED_CLR(unit, entry_index);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_repl_list_entry_alloc
 * Purpose:
 *      Allocate a free entry from REPL_LIST table.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      entry_index - (OUT)Index of a free entry.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_tr3_repl_list_entry_alloc(int unit, int *entry_index)
{
    int i;

    for (i = 0; i < _tr3_repl_list_entry_info[unit]->num_entries; i++) {
        if (!REPL_LIST_ENTRY_USED_GET(unit, i)) {
            *entry_index = i;
            REPL_LIST_ENTRY_USED_SET(unit, i);
            return BCM_E_NONE;
        }
    }

    *entry_index = -1;
    return BCM_E_RESOURCE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_tr3_repl_list_entry_used_set
 * Purpose:
 *      Mark the REPL_LIST table entry as used.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      entry_index - (IN)Index of the entry to be marked.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_repl_list_entry_used_set(int unit, int entry_index)
{
    REPL_LIST_ENTRY_USED_SET(unit, entry_index);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_repl_list_entry_used_get
 * Purpose:
 *      Get used status of a REPL_LIST table entry.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      entry_index - (IN)Index of the entry.
 *      used - (OUT)Used status.
 * Returns:
 *      TRUE if used, else FALSE.
 */
STATIC int
_bcm_tr3_repl_list_entry_used_get(int unit, int entry_index)
{
    if (REPL_LIST_ENTRY_USED_GET(unit, entry_index)) {
        return TRUE;
    }
    return FALSE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_METROLITE_SUPPORT
/* Function:
 *      _bcm_metorlite_repl_list_tbl_ptr_get
 * Purpose:
 *      Get the appropriate MMU replication tables and intended
 *      member bitmap last bitmap fields for  for port
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      port       - (IN) port.
 *      info       - (OUT) MMU group table info
*                     mem, pbmp, index corresponding to the port.
 * Returns:
 *      BCM_E_XXX
 */
    STATIC int
_bcm_metrolite_repl_list_tbl_ptr_get(int unit, bcm_port_t port,
        _kt2_repl_grp_info_t *info)
{
    if (port < 32) {
        info->mb_mem =  MMU_REPL_GRP_TBLm;
        info->mbmp = MEMBER_BMP_PORTSf;
        info->lb_mem = MMU_REPL_GRP_TBLm;
        info->lbmp =  LAST_BMP_PORTSf;
        info->mb_index = port;
        info->lb_index = port;
    } else {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit, "REPL_GRP_TBL invalid port\n")));
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif
#ifdef BCM_SABER2_SUPPORT
/* Function:
 *      _bcm_sb2_repl_list_tbl_ptr_get
 * Purpose:
 *      Get the appropriate MMU replication tables and intended 
 *      member bitmap last bitmap fields for  for port
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      port       - (IN) port. 
 *      info       - (OUT) MMU group table info 
*                     mem, pbmp, index corresponding to the port.
 * Returns:
 *      BCM_E_XXX
 */
    STATIC int
_bcm_sb2_repl_list_tbl_ptr_get(int unit, bcm_port_t port,
        _kt2_repl_grp_info_t *info)
{
    if ((port >= 0) && (port < 24)) {
        info->mb_mem =  MMU_REPL_GRP_TBL0m;
        info->mbmp = MEMBER_BMP_PORTSf;
        info->lb_mem = MMU_REPL_GRP_TBL0m;
        info->lbmp =  LAST_BMP_PORTS_23_0f;
        info->mb_index = port;
        info->lb_index = port;
    } else if ((port >= 24) && (port < 96)) {
        info->mb_mem =  MMU_REPL_GRP_TBL0m;
        info->mbmp = MEMBER_BMP_PORTSf;
        info->lb_mem = MMU_REPL_GRP_TBL1m;
        info->lbmp =  LAST_BMP_PORTS_95_24f;
        info->mb_index = port;
        info->lb_index = port - 24;
    } else {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "REPL_GRP_TBL invalid port\n")));
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#ifdef BCM_METROLITE_SUPPORT
/* Function:
 *      _bcm_metrolite_repl_group_info_set
 * Purpose:
 *      Set replication group info.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      repl_group - (IN) The replication group number.
 *      port       - (IN) port.
 *      mbmp       - (IN) Member bitmap.
 *      lbmp       - (IN) Last Member bitmap.
 *      head_index - (IN) Base pointer to REPL_HEAD table.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_metrolite_repl_group_info_set(int unit, int repl_group, bcm_port_t port,
        soc_pbmp_t mbmp, soc_pbmp_t lbmp, int head_index)
{
    mmu_repl_grp_tbl_entry_t repl_grp_tbl_entry;
    _kt2_repl_grp_info_t kt2_repl_grp_info;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;

    sal_memset(&repl_grp_tbl_entry, 0, sizeof(repl_grp_tbl_entry));
    sal_memset(&kt2_repl_grp_info, 0, sizeof(kt2_repl_grp_info));

    BCM_IF_ERROR_RETURN(_bcm_metrolite_repl_list_tbl_ptr_get (unit, port,
                &kt2_repl_grp_info));

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));

    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(lbmp, i);
    }

    soc_mem_field_set(unit, kt2_repl_grp_info.lb_mem,
                (uint32 *)&repl_grp_tbl_entry, kt2_repl_grp_info.lbmp, fldbuf);
    soc_MMU_REPL_GRP_TBLm_field32_set(unit, &repl_grp_tbl_entry,
            BASE_PTRf, head_index);

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(mbmp, i);
    }

    soc_MMU_REPL_GRP_TBLm_field_set(unit, &repl_grp_tbl_entry,
            MEMBER_BMP_PORTSf, fldbuf);
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GRP_TBLm(unit,
                MEM_BLOCK_ALL, repl_group, &repl_grp_tbl_entry));
    return BCM_E_NONE;
}
#endif
/* Function:
 *      _bcm_sb2_repl_group_info_set
 * Purpose:
 *      Set replication group info.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      repl_group - (IN) The replication group number.
 *      port       - (IN) port.
 *      mbmp       - (IN) Member bitmap.
 *      lbmp       - (IN) Last Member bitmap.
 *      head_index - (IN) Base pointer to REPL_HEAD table.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_sb2_repl_group_info_set(int unit, int repl_group, bcm_port_t port,
        soc_pbmp_t mbmp, soc_pbmp_t lbmp, int head_index)
{
    mmu_repl_grp_tbl0_entry_t repl_grp_tbl0_entry;
    mmu_repl_grp_tbl1_entry_t repl_grp_tbl1_entry;
    _kt2_repl_grp_info_t kt2_repl_grp_info;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;

    sal_memset(&repl_grp_tbl0_entry, 0, sizeof(repl_grp_tbl0_entry));
    sal_memset(&repl_grp_tbl1_entry, 0, sizeof(repl_grp_tbl1_entry));
    sal_memset(&kt2_repl_grp_info, 0, sizeof(kt2_repl_grp_info));

    BCM_IF_ERROR_RETURN(_bcm_sb2_repl_list_tbl_ptr_get (unit, port,
                &kt2_repl_grp_info));

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));

    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(lbmp, i);
    }

    if (port >=24) {
        soc_mem_field_set(unit, kt2_repl_grp_info.lb_mem,
                (uint32 *)&repl_grp_tbl1_entry, kt2_repl_grp_info.lbmp, fldbuf);
    } else {
        soc_mem_field_set(unit, kt2_repl_grp_info.lb_mem,
                (uint32 *)&repl_grp_tbl0_entry, kt2_repl_grp_info.lbmp, fldbuf);
    }
    soc_MMU_REPL_GRP_TBL1m_field32_set(unit, &repl_grp_tbl1_entry,
            BASE_PTRf, head_index);


    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(mbmp, i);
    }

    soc_MMU_REPL_GRP_TBL0m_field_set(unit, &repl_grp_tbl0_entry,
            MEMBER_BMP_PORTSf, fldbuf);
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GRP_TBL1m(unit,
                MEM_BLOCK_ALL, repl_group, &repl_grp_tbl1_entry));
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GRP_TBL0m(unit,
                MEM_BLOCK_ALL, repl_group, &repl_grp_tbl0_entry));
    return BCM_E_NONE;
}


/* Function:
 *     _bcm_sb2_ipmc_subscriber_node_max_get 
 * Purpose:
 *     calculate the maximum hardware qlist indexes
 *     needed based on the replication count . 
 *     Each hardware index can take two pair of (queueid,nexthopid)
 *     but the both the queue id cannot be same  in given index 
 *     This function tries to find the optimum index required given the
 *     above restriction.
 *     it tries to pair  (queueid,encapid) such that the two queues are 
 *     not same           
 *     eg if let there be 3 queues and 3 replication on each queue       
 *     queue 1 requires 3 indexes but there are 3 open positions in 
 *     these index  so queue 2 is paired with queue 1 and 3 indexes
 *     are filled and queue 3 will require futher 3 indexes.So the total is
 *     6. 
 *     calculation based on the code below 
 *     iinitialy we need 3 for queue1 so 
 *     index0 = 3 
 *     then we need 3 for queue 2 but we can use the index1 entry
 *     index1 = 3 
 *     check if index1 >= index0 (3 >= 3) 
 *     if true we have found count of index0 pairs and index0 valus is
 *     added to max_count and diffrence between index1 and index0 is kept
 *     in index0 as we have yet to find pairs for them . 
 *     this is continued till all the replications are taken in account 
 *     At the end whatever that remains in the index0  count does not have  
 *     pairs and will use up seperate index, hence added to max_count . 
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      ipmc_id    - (IN) ipmc_id
 *      node_max   -  (OUT) max indexes  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_ipmc_subscriber_node_max_get( int unit, int ipmc_id, int *node_max)
{

    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                          ext_ipmc_list[ipmc_id];  
    _bcm_ext_repl_q_list_t *startq;

    int index0, index1 ;
    index0 = index1 = 0;
    *node_max = 0; 
    startq = ipmc_node->first_q_ptr;
    if (!startq) {
        *node_max = 0 ;         
        return BCM_E_INTERNAL; 
    }
    index0 = startq->repl_count ;
    startq = startq->next ;
    for (; startq ; startq = startq->next) {
         index1 += startq->repl_count;
         if (index1 >= index0) {                  
             *node_max += index0 ; 
             index0 = (index1 - index0) ;
             index1 = 0; 
         } 
    }
    *node_max += index0;    
    return BCM_E_NONE;  
}

 /*
  * Function:
  *    _bcm_sb2_extq__repl_list_free
  * Description:
  *    Release the MMU_EXT_MC_QUEUE_LIST entries in the HW list starting
  *     at extq_ptr.
  */

STATIC int
_bcm_sb2_extq_repl_list_free(int unit, int extq_ptr)
{
     int     rv = BCM_E_NONE;
     int     next_ptr1, next_ptr2;
     mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
     mmu_ext_mc_queue_list0_entry_t     qlist0_entry;

     if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                     extq_ptr, &qlist4_entry)) < 0) {
          goto repl_list_free_done;
     }
     if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                     extq_ptr, &qlist0_entry)) < 0) {
          goto repl_list_free_done;
     }
     next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                                      &qlist4_entry, EXT_Q_NXT_PTR1f);
     next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                                      &qlist4_entry, EXT_Q_NXT_PTR2f);
     IPMC_EXTQ_REPL_VE_USED_CLR(unit, extq_ptr);

     sal_memset (&qlist0_entry, 0,
         sizeof(mmu_ext_mc_queue_list0_entry_t));
     sal_memset (&qlist4_entry, 0, 
                sizeof(mmu_ext_mc_queue_list4_entry_t));    

     rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                             extq_ptr, &qlist0_entry);
     rv =  WRITE_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                             extq_ptr, &qlist4_entry);

     if (next_ptr1 != extq_ptr) {
         if ((rv = _bcm_sb2_extq_repl_list_free(unit, next_ptr1)) < 0) {
              goto repl_list_free_done;
         }
     }

     if (next_ptr2 != extq_ptr) {
         if ((rv = _bcm_sb2_extq_repl_list_free(unit, next_ptr2)) < 0) {
              goto repl_list_free_done;
         }
     }

 repl_list_free_done:
     return rv;

}

#ifdef BCM_SABER2_SUPPORT
void
bcm_sb2_ipmc_repl_extq_port_free(int unit, bcm_port_t port) {
   if (_sb2_extq_repl_info[unit]->port_info[port] != NULL) {
        if (_sb2_extq_repl_info[unit]->port_info[port]->queue_count
                != NULL) {
            sal_free(
                    _sb2_extq_repl_info[unit]->port_info[port]->queue_count);
            _sb2_extq_repl_info[unit]->port_info[port]->queue_count = NULL;
        }
        sal_free(_sb2_extq_repl_info[unit]->port_info[port]);
    }
    _sb2_extq_repl_info[unit]->port_info[port] = NULL;
}
#endif

/*
 * Function:
 *      bcm_sb2_ipmc_repl_detach
 * Purpose:
 *      Cleanup routing to deinit ipmc 
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_sb2_ipmc_repl_detach(int unit)
{
    _bcm_repl_list_info_t *rli_current, *rli_free;
    bcm_port_t            port;
    bcm_pbmp_t            pbmp_pp;

    BCM_PBMP_CLEAR(pbmp_pp);
    BCM_PBMP_ASSIGN(pbmp_pp, PBMP_PP_ALL(unit));

    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_pp);
    }
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &pbmp_pp));
    }
    
    if (_sb2_extq_repl_info[unit] != NULL) {
        PBMP_ITER(pbmp_pp, port) {
            if (port >= SOC_MAX_NUM_PP_PORTS) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "bcm_sb2_ipmc_repl_detach invalid PP port %d\n"),port));
                break;
            }

            bcm_sb2_ipmc_repl_extq_port_free(unit, port); 
        }

        if (_sb2_extq_repl_info[unit]->mcgroup_info != NULL) {
            if (_sb2_extq_repl_info[unit]->mcgroup_info->queue_count 
                                            != NULL) {
                sal_free(_sb2_extq_repl_info[unit]->
                         mcgroup_info->queue_count);
            }
            if (_sb2_extq_repl_info[unit]->mcgroup_info->queue_count_int
                                            != NULL) {
                sal_free(_sb2_extq_repl_info[unit]->
                         mcgroup_info->queue_count_int);
            }
            if (_sb2_extq_repl_info[unit]->mcgroup_info->queue_count_ext
                                            != NULL) {
                sal_free(_sb2_extq_repl_info[unit]->
                         mcgroup_info->queue_count_ext);
            }
            sal_free(_sb2_extq_repl_info[unit]->mcgroup_info);
        }
        if (_sb2_extq_repl_info[unit]->ext_ipmc_list) {
            sal_free(_sb2_extq_repl_info[unit]->ext_ipmc_list);

        }
        if (_sb2_extq_repl_info[unit]->bitmap_entries_used != NULL) {
            sal_free(_sb2_extq_repl_info[unit]->bitmap_entries_used);
        }


        if (_sb2_extq_repl_info[unit]->repl_extq_list_info != NULL) {
            rli_current = IPMC_EXTQ_REPL_LIST_INFO(unit);
            while (rli_current != NULL) {
                   rli_free = rli_current;
                   rli_current = rli_current->next;
                   sal_free(rli_free);
            }
        }

        sal_free(_sb2_extq_repl_info[unit]);
        _sb2_extq_repl_info[unit] = NULL;
    }

    return BCM_E_NONE;
}



 STATIC int
_bcm_sb2_extq_repl_next_free_ptr(int unit)
 {
     int                 ix, bit;
     SHR_BITDCL          not_ptrs;

     for (ix = 0; ix < _SHR_BITDCLSIZE(IPMC_EXTQ_LIST_TOTAL(unit)); ix++) {
         not_ptrs = ~_sb2_extq_repl_info[unit]->bitmap_entries_used[ix];
         if (not_ptrs) {
             for (bit = 0; bit < SHR_BITWID; bit++) {
                  if (not_ptrs & (1 << bit)) {
                      return (ix * SHR_BITWID) + bit;
                  }
             }
         }
     }

     return -1;
}


 STATIC int
 _sb2_extq_mc_group_ptr(int unit,int ipmc_id,
                   int *extq_ptr, int last, int set)
{
   int         rv = BCM_E_NONE;
   mmu_ext_mc_group_map_entry_t mc_group_entry;
   soc_mem_t mem = MMU_EXT_MC_GROUP_MAPm;
   int remap_id;

   soc_mem_lock(unit, mem);
   if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                         ipmc_id, &mc_group_entry)) < 0) {
        soc_mem_unlock(unit, mem);
        return rv;
   }

   if (set) {
       soc_mem_field32_set(unit, mem, &mc_group_entry,
       EXT_Q_FIRST_PTRf, *extq_ptr);
       soc_mem_field32_set(unit, mem, &mc_group_entry,
       EXT_Q_LAST_LASTf, last ? 1 : 0);
 
       if ((rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, ipmc_id,
                                    &remap_id, FALSE)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
       }
       /* update the new chain in the remapped id */
       if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, remap_id,
                                   &mc_group_entry)) < 0) {
       soc_mem_unlock(unit, mem);
       return rv;
       }
  
   } else {
       *extq_ptr = soc_mem_field32_get(unit, mem, &mc_group_entry,
                                              EXT_Q_FIRST_PTRf);
   }
   soc_mem_unlock(unit, mem);
   return rv;
}


int 
_bcm_sb2_ipmc_set_remap_group(int unit, int ipmc_id, 
                            bcm_port_t port, int count)
{
    int rv = BCM_E_NONE;
    int int_mem = 0, ext_mem = 0;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    bcm_pbmp_t destination_pbmp;
    int port_ext_queue_count = 0;
    soc_mem_t dest_pbm_mem;




    
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                     ipmc_id, entry));

    /*
     * Get extended queue_count_int and extended queue_count_ext
     * If there is at least one extended queue replication that
     * requires external buffer, EXT_Q_EXTERNALf is set to 1.
     * If there is at least one extended queue replication that
     * requires internal buffer, EXT_Q_INTERNALf is set to 1.
     */
    int_mem = IPMC_EXTQ_REPL_QUEUE_COUNT_INT(unit, ipmc_id) ? 1 : 0;
    ext_mem = IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(unit, ipmc_id) ? 1 : 0;

       soc_mem_field32_set(unit, L3_IPMCm, entry, EXT_Q_INTERNALf, 
            int_mem);
        soc_mem_field32_set(unit, L3_IPMCm, entry, EXT_Q_EXTERNALf, 
            ext_mem);
    soc_mem_field32_set(unit, L3_IPMCm, entry, EXT_Q_NUM_COPIESf, 
                                                          count);
    
    soc_mem_field32_set(unit, L3_IPMCm, entry, VALIDf, 1);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                     ipmc_id, entry));

    BCM_PBMP_CLEAR(destination_pbmp);

    if (SOC_MEM_IS_VALID(unit ,L3_IPMC_2m)) {
        /* For Katana2 */
        dest_pbm_mem = L3_IPMC_2m ;
    } else {
        /* Saber2, Katana */
        dest_pbm_mem = L3_IPMCm;
    }

    if (SOC_MEM_FIELD_VALID(unit, dest_pbm_mem, DESTINATION_PBMf)) {

        port_ext_queue_count = IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit,
                                   port, ipmc_id);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, dest_pbm_mem, MEM_BLOCK_ANY,
                                     ipmc_id, entry));

        /* Extract Destination_pbm for egress mirror support on extended queues. */
        soc_mem_pbmp_field_get(unit,
            dest_pbm_mem, entry, DESTINATION_PBMf, &destination_pbmp);
        if (port_ext_queue_count) {
            BCM_PBMP_PORT_ADD(destination_pbmp, port);
        } else {
            if (BCM_PBMP_MEMBER(destination_pbmp, port)) {
                BCM_PBMP_PORT_REMOVE(destination_pbmp, port);
            }
        }
        soc_mem_pbmp_field_set(unit,
            dest_pbm_mem, entry, DESTINATION_PBMf, &destination_pbmp);

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, dest_pbm_mem, MEM_BLOCK_ANY,
                                     ipmc_id, entry));
    }


    return rv;
}


/*
 * Function:
 *     _bcm_sb2_ipmc_subscriber_find_ipmc 
 * Purpose:
 *      For the given hardware start index find a matching ipmc
 *      that uses that hardware start index .  
 * Parameters:
 *      unit     - SOC unit #
 *      start_index - (IN) hardware start index 
 *      ipmc_id      - (OUT) ipmc_id
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_ipmc_subscriber_find_ipmc(int unit ,int start_index, int *ipmc_id) 
{

  _bcm_ext_q_ipmc_t  *ipmc_node ; 
                                  
  int max_ipmc;
  int i = 0; 
  
  max_ipmc = soc_mem_index_count(unit, L3_IPMCm);   
  for (i = 0; i < max_ipmc; i++) {
       ipmc_node = &_sb2_extq_repl_info[unit]->
                                 ext_ipmc_list[i];  
       if(ipmc_node->list_start_index == start_index) {
          *ipmc_id = i;
          break;  
       }
  }
  return BCM_E_NONE; 
}

/*
 * Function:
 *     _bcm_sb2_extq_compare_nh_nodes 
 * Purpose:
 *     Compare all the nh_nodes for the given two queue nodes    
 *      
 * Parameters:
 *      unit     - SOC unit #
 *      startq   -  
 *      queue_node   - 
 * Returns:
 *      BCM_E_XXX
 */

int
 _bcm_sb2_extq_compare_nh_nodes(_bcm_ext_repl_q_list_t *startq,
                                  _bcm_ext_repl_q_list_t *queue_node)
{

    _bcm_ext_nh_list_t *nh_nodenew, *nh_node;
    int nh_node_found = 0;

     for (nh_nodenew = startq->first_nh_ptr ; nh_nodenew ;
                                 nh_nodenew = nh_nodenew->next) {
          nh_node_found = 0 ; 
          for (nh_node = queue_node->first_nh_ptr ; nh_node ;
                                 nh_node = nh_node->next) {
               if (nh_nodenew->encap_id == nh_node->encap_id) {
                   nh_node_found = 1 ;
                   break ; 
               }
          }
          if (!nh_node_found) {
              return BCM_E_NOT_FOUND; 
          }
     }
     return BCM_E_NONE; 
}


/*
 * Function:
 *     _bcm_sb2_extq_repl_list_compare 
 * Purpose:
 *     Compare the two ipmc tree and find if they are similar 
 * Parameters:
 *      unit     - SOC unit #
 *      startq   -  
 *      queue_node   - 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_extq_repl_list_compare(int unit , int new_ipmc_id ,int ipmc_id)
{

    _bcm_ext_q_ipmc_t  *ipmc_node = &(_sb2_extq_repl_info[unit]->
                                         ext_ipmc_list[ipmc_id]);  
    _bcm_ext_repl_q_list_t *startq,*queue_node;
    _bcm_ext_q_ipmc_t  *ipmc_nodenew = &(_sb2_extq_repl_info[unit]->
                                         ext_ipmc_list[new_ipmc_id]);  
    int rv = 0;
    if (ipmc_id == new_ipmc_id) {
        return BCM_E_NOT_FOUND; 
    }
    /*compare queue hash*/ 
    if ( ipmc_nodenew->queue_hash 
        != ipmc_node->queue_hash) {
         return BCM_E_NOT_FOUND; 
    }
    startq = ipmc_nodenew->first_q_ptr; 
    
    for (; startq; startq = startq->next) {

       for (queue_node = ipmc_node->first_q_ptr ;
            queue_node ; queue_node = queue_node->next) {
            if(startq->queue_id == queue_node->queue_id) {
               break; 
           }
       }
       if (!queue_node) {
           return BCM_E_NOT_FOUND;
       }
       if (startq->repl_hash != queue_node->repl_hash) {
          return BCM_E_NOT_FOUND; 
       }
       rv =  _bcm_sb2_extq_compare_nh_nodes(startq, queue_node);  
       if (rv < 0) { 
           break ; 
       }
    }
    return rv; 
}

/*
 * Function:
 *   _bcm_sb2_subscriber_ipmc_index_validate   
 * Purpose:
 *     verifies if we have suffcient hardware indexes
 * Parameters:
 *      unit     - SOC unit #
 *      ipmc_id   -  
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_sb2_subscriber_ipmc_index_validate(int unit, int ipmc_id)
{

     int node_max = 0; 
     int *index; 
     int rv = 0;
     int i = 0 ; 
     
     _bcm_sb2_ipmc_subscriber_node_max_get(unit, ipmc_id, &node_max);
     index = sal_alloc(node_max * sizeof(int),"list of index");
     for (i = 0; i < node_max; i++) {
          index[i] = _bcm_sb2_extq_repl_next_free_ptr(unit);
          if (index[i] < 0) {
              rv = BCM_E_RESOURCE;
              goto clean_up;
          }
          IPMC_EXTQ_REPL_VE_USED_SET(unit, index[i]);
    }
     
clean_up:
    i--;    
    for ( ; i >= 0; i--) {
         IPMC_EXTQ_REPL_VE_USED_CLR(unit,index[i]); 
    }
    sal_free(index); 
    return rv; 
}


/*
 * Function:
 *    _bcm_sb2_extq_repl_list_write   
 * Purpose:
 *    Writes the data in given ipmc tree in hardware 
 * Parameters:
 *      unit     - SOC unit #
 *      ipmc_id   -  
 *      extq_index - (OUT) start of index in hardware 
 *      count      -(OUT)  count of hardware index used 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_sb2_extq_repl_list_write(int unit, int ipmc_id, 
           int *extq_index, int *count)  
{
    int     node_max, alloc_size;
    int     next1, next2;
    int     i, repl_count = 0;
    int     rv = BCM_E_NONE;
    int     copy, last, buf_type, vlan_last;
    int     reset_val  = 1;
    int     encap_id = 0 ;
    int     repl_index = 0;
    int     next_ptr_allocated = 0 ;
    int     queue_id = 0;
    _bcm_repl_list_info_t *list_array = NULL;
    _bcm_repl_list_info_t *cur_node, *next, *fast_node, *start_node;
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                          ext_ipmc_list[ipmc_id];  
    _bcm_ext_repl_q_list_t *startq;
    mmu_ext_mc_queue_list0_entry_t     *qlist0_entry = NULL;
    mmu_ext_mc_queue_list4_entry_t     *qlist4_entry = NULL;
    soc_field_t        base_qid[] = { BASE_QID0f, BASE_QID1f };
    soc_field_t        profile_idx[] = { Q_OFFSET_PROF0f, Q_OFFSET_PROF1f };
    soc_field_t        repl_ptr[] = { L3_REPL_PTR0f, L3_REPL_PTR1f };    
    soc_field_t        l2_copy[]  = { L2_COPY0f, L2_COPY1f };
    soc_field_t        l3_last[] = { L3_LAST0f, L3_LAST1f };     
    soc_field_t        buff_type[] = { BUFF_TYPE0f, BUFF_TYPE1f };
    soc_field_t        reset_field[] = { L3_REPL_TYPE0f, L3_REPL_TYPE1f };   
    _bcm_ext_nh_list_t *startnh; 
    
    reset_val = 0x4000;
    for (i = 0; i < 2; i++) {
         reset_field[i] = repl_ptr[i];
    }
    node_max = 0;  
    _bcm_sb2_ipmc_subscriber_node_max_get(unit, ipmc_id, &node_max);

    alloc_size = node_max * sizeof(_bcm_repl_list_info_t);
    list_array = sal_alloc(alloc_size, "IPMC repl node array");

    if (list_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(list_array, 0, alloc_size);

    qlist0_entry = sal_alloc(node_max * 
                               sizeof(mmu_ext_mc_queue_list0_entry_t),
                                   "IPMC repl entry0 array ");
    if (qlist0_entry == NULL) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    qlist4_entry = sal_alloc(node_max *
                      sizeof(mmu_ext_mc_queue_list4_entry_t),
                                   "IPMC repl entry0 array ");
    if (qlist4_entry == NULL) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    /* create a link list of entries with free qlist pointers */    
    for (i = 0; i < node_max; i++) {
         sal_memset(&qlist0_entry[i], 0,
             sizeof(mmu_ext_mc_queue_list0_entry_t));
         sal_memset(&qlist4_entry[i], 0, 
                    sizeof(mmu_ext_mc_queue_list4_entry_t));    
         cur_node = &list_array[i];
         cur_node->index = _bcm_sb2_extq_repl_next_free_ptr(unit);
         if (cur_node->index < 0) {
             rv = BCM_E_RESOURCE;
             goto clean_up;
         }
         IPMC_EXTQ_REPL_VE_USED_SET(unit, cur_node->index);
         if (i < (node_max - 1)) { /* create a chain till penultimate node */
             cur_node->next = &list_array[i+1];
         }
    }    

    /* copy back the stored contents into qlist entries */
    fast_node = cur_node = &list_array[0];
    *extq_index = cur_node->index;

    *count = 0;
    repl_count = 0 ; 
    repl_index = 0 ; 
    start_node = cur_node; 
    for (startq = ipmc_node->first_q_ptr ; startq; startq = startq->next) {
         for (startnh = startq->first_nh_ptr; startnh ; startnh=startnh->next) {
              soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                                &qlist0_entry[repl_index], 
                                     base_qid[repl_count],startq->queue_id);
              soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                                      &qlist0_entry[repl_index],
                                                      profile_idx[repl_count],
                                                      startq->profile_id & 0x3);
              copy = startq->l2_copy; 
              buf_type = startq->buf_type;
              vlan_last = 1;
              soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                            &qlist0_entry[repl_index], 
                                             l2_copy[repl_count], copy);
              if (soc_feature(unit, soc_feature_ddr3)) {
                  soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                              &qlist0_entry[repl_index], 
                                        buff_type[repl_count], buf_type);
              }
              soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, 
                                                    &qlist0_entry[repl_index],
                                                     l3_last[repl_count],
                                                     vlan_last);
                                 
              if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, startnh->encap_id)) {
                  /* L3 interface is used */
                  encap_id = startnh->encap_id + IPMC_SB2_EGRESS_ID_MIN ;
              } else {
                 /* Next hop is used */
                  encap_id =   startnh->encap_id -
                           BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + IPMC_SB2_NHOP_ID_MIN;
              }
              soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, 
                                                   &qlist0_entry[repl_index],
                                                        repl_ptr[repl_count],
                                                        encap_id);
              if (!next_ptr_allocated)  {
                  last = 0;
                  if ((repl_index < 2) || (repl_index % 2) != 0) {
                      next = fast_node->next;
                      if (next != NULL) {
                          next1 = next->index;
                          fast_node = next;
                          if (fast_node->next != NULL) {
                              next2 = fast_node->next->index;
                              fast_node = fast_node->next;
       /* need to set the last flag now, if this is the last enrty */
                              if (fast_node->next == NULL) {
                                  last = 1;
                              }
                         } else {
                             next2 = cur_node->index;
                             last = 1;
                         }
                     } else {
                         next1 = cur_node->index;
                         next2 = next1;
                     }    
                 } else {
                    next1 = cur_node->index;
                    next2 = next1;
                 }
                 soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit,
                                                 &qlist4_entry[repl_index], 
                                                    EXT_Q_NXT_PTR1f, next1);
                 soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit, 
                                                  &qlist4_entry[repl_index], 
                                                    EXT_Q_NXT_PTR2f, next2);
                 soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit,  
                                                  &qlist4_entry[repl_index],
                                                    EXT_Q_LAST_LASTf, last);
                 cur_node = cur_node->next; 
             }
             repl_index++;  
             if (repl_index == node_max)  {
                 *count = repl_index ;
                 repl_index = 0; 
                 repl_count = 1 ; 
                 next_ptr_allocated = 1 ;  
             }
          }
      }
      cur_node = start_node; 
      for ( i = 0 ; i < node_max ; i++)   {
           if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, 
                                    &qlist0_entry[i], base_qid[1])) == 0) {
                soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, 
                                                   &qlist0_entry[i],
                                                   reset_field[1], 
                                                   reset_val);
           }
           if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                  cur_node->index, &qlist0_entry[i])) < 0) {
                goto clean_up;
           }
           if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                   cur_node->index, &qlist4_entry[i])) < 0) {
                goto clean_up;
           }

           cur_node = cur_node->next;
     }
            
clean_up:
    if (list_array != NULL) {
        sal_free(list_array);
    }
    if (qlist0_entry) {
        sal_free(qlist0_entry);
    }
    if (qlist4_entry) {
        sal_free(qlist4_entry);
    }
    return rv;
}

int
_bcm_sb2_ipmc_delete_extq_list(int unit ,int prev_start_ptr, int ipmc_id) 
{

     int new_ipmc_id = 0;
     _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;
     if (!prev_start_ptr ) {
         return BCM_E_INTERNAL; 

     } 
     rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);
     rli_prev = NULL;
     for (rli_current = rli_start; rli_current != NULL;
          rli_current = rli_current->next) {
          if (prev_start_ptr == rli_current->index) {
              (rli_current->refcount)--;
              if (rli_current->refcount &&
                     (rli_current->first_ipmc == ipmc_id)) {
                  _bcm_sb2_ipmc_subscriber_find_ipmc(unit, rli_current->index,
                                                                  &new_ipmc_id);
                   rli_current->first_ipmc = new_ipmc_id;
              }
              if (rli_current->refcount == 0) {
                  /* Free these linked list entries */
                  _bcm_sb2_extq_repl_list_free(unit, prev_start_ptr);
                  /* If we have an error, we'll fall out anyway */
                  if (rli_prev == NULL) {
                      IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current->next;
                  } else {
                      rli_prev->next = rli_current->next;
                  }
                  sal_free(rli_current);
              }
                    break;
          }
          rli_prev = rli_current;
     }
     return BCM_E_NONE;
}


int
_bcm_sb2_ipmc_subscriber_extq_update(int unit, int ipmc_id, int *qlist_deleted)
                                        
{
    int     rv = BCM_E_NONE;
    int     list_start_ptr, prev_start_ptr;
    int     alloc_size, extq_hash, last_flag;
    int     queue_count = 0;
    SHR_BITDCL *q_vec = NULL;
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
        ext_ipmc_list[ipmc_id];
    _bcm_repl_list_info_t *rli_start, *rli_current;

    /* Check previous group pointer */
    if ((rv = _sb2_extq_mc_group_ptr(unit, ipmc_id,
                    &prev_start_ptr, 0, 0)) < 0) {
        goto list_update_done;
    }
    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);
    if((rv = _bcm_sb2_ipmc_subscriber_qvec_get(unit, ipmc_id, q_vec)) < 0 ) {
        goto list_update_done;

    }
    /* Search for list already in table */
    rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);

    extq_hash =
        _shr_crc32b(0, (uint8 *)q_vec, IPMC_EXTQ_TOTAL(unit));

    for (rli_current = rli_start; rli_current != NULL;
            rli_current = rli_current->next) {
         if (extq_hash == rli_current->hash) {
             rv = _bcm_sb2_extq_repl_list_compare(unit, ipmc_id,
                    rli_current->first_ipmc);
             if (rv == BCM_E_NOT_FOUND) {
                 continue; /* Not a match */
             } else if (rv < 0) {
                 goto list_update_done; /* Access error */
             } else {
                 break; /* Match */
             }
         }
    }

    if (rli_current != NULL) {
        /* Found a match, point to here and increase reference count */
        if (prev_start_ptr == rli_current->index) {
            /* We're already pointing to this list, so done */
            rv = BCM_E_NONE;
            goto list_update_done;
        } else {
            list_start_ptr = rli_current->index;
            queue_count = rli_current->list_size;
        }
    } else {
        /* Not a match, make a new chain */
        rv = _bcm_sb2_subscriber_ipmc_index_validate(unit, ipmc_id); 
        if (rv < 0) {
            if (prev_start_ptr) {
                _bcm_sb2_ipmc_delete_extq_list(unit, prev_start_ptr, ipmc_id);
                *qlist_deleted = 1;   
            } 
            rv = _bcm_sb2_subscriber_ipmc_index_validate(unit, ipmc_id); 
            if ( rv < 0 ) {
                goto list_update_done; 
            }
        }
        if ((rv = _bcm_sb2_extq_repl_list_write(unit, ipmc_id , 
                        &list_start_ptr,
                        &queue_count)) < 0 ) {
            goto list_update_done;
        }

        if (queue_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC extq repl list info");
            if (rli_current == NULL) {
                /* Release list */
                _bcm_sb2_extq_repl_list_free(unit, list_start_ptr);
                rv = BCM_E_MEMORY;
                goto list_update_done;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = list_start_ptr;
            rli_current->hash = extq_hash;
            rli_current->next = rli_start;
            rli_current->list_size = queue_count;
            rli_current->first_ipmc = ipmc_id;
            IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
        }
    } 
    last_flag = (queue_count == 1);

    if (queue_count > 0) {
        if ((rv = _sb2_extq_mc_group_ptr(unit, ipmc_id, 
                        &list_start_ptr, last_flag, 1)) < 0) {
            if (rli_current->refcount == 0) {
                /* This was new */
                _bcm_sb2_extq_repl_list_free(unit, list_start_ptr);
                IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current->next;
                sal_free(rli_current);
            }
            goto list_update_done;
        }

        (rli_current->refcount)++;
        ipmc_node->list_start_index = list_start_ptr;            
        /* we don't need this rli_current anymore */
    } else if (prev_start_ptr != 0) {
        list_start_ptr = 0;
        if ((rv = _sb2_extq_mc_group_ptr(unit, ipmc_id,
                        &list_start_ptr, last_flag, 1)) < 0) {
            goto list_update_done;
        }
    }
    IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) = queue_count;
    if (!(*qlist_deleted)) { 
        _bcm_sb2_ipmc_delete_extq_list(unit, prev_start_ptr, ipmc_id);
    }

list_update_done:
    if (q_vec != NULL) {
        sal_free(q_vec);
    }
    return rv;

}

int
_bcm_sb2_ipmc_subscriber_qvec_get(int unit, int ipmc_id, 
                                  SHR_BITDCL *q_vec)
{
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                        ext_ipmc_list[ipmc_id];
    _bcm_ext_repl_q_list_t *startq; 
    
    for (startq = ipmc_node->first_q_ptr; startq;
                          startq = startq->next) {
         SHR_BITSET(q_vec, startq->queue_id);
    }
    return BCM_E_NONE ; 
}


int
_bcm_sb2_ipmc_update_queue_hash(int unit, int ipmc_id) 
{

    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                           ext_ipmc_list[ipmc_id];
    int alloc_size = 0;  
    int rv = 0 ; 
    SHR_BITDCL *q_vec = NULL;

    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);
    if ((rv = _bcm_sb2_ipmc_subscriber_qvec_get(unit, ipmc_id, q_vec)) < 0 ) {
         goto clean_up; 

    }
    ipmc_node->queue_hash =  
               _shr_crc32b(0, (uint8 *)q_vec, IPMC_EXTQ_TOTAL(unit));
clean_up :
    if (q_vec != NULL) {
        sal_free(q_vec);
    }
    return rv;
}

int
_bcm_sb2_ipmc_subscriber_queue_node_get(int unit, int ipmc_id, 
                                        int queue_id,
                                        _bcm_ext_repl_q_list_t **queue_node) 
{
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                         ext_ipmc_list[ipmc_id];
    _bcm_ext_repl_q_list_t *startq; 

    if (!queue_node) { 
        return BCM_E_INTERNAL;  
    }
    *queue_node = NULL ; 
    for (startq = ipmc_node->first_q_ptr ; startq;
                          startq = startq->next) {
         if (queue_id == startq->queue_id ) {
             *queue_node = startq ; 
              break ; 
         }
    }
    return BCM_E_NONE ;
}

int
_bcm_sb2_ipmc_subscriber_add_queue_node(int unit, int ipmc_id, int queue_id,  
                                    _bcm_ext_repl_q_list_t *queue_node) 

{
    _bcm_ext_q_ipmc_t  *ipmc_node  = &_sb2_extq_repl_info[unit]->
                                                  ext_ipmc_list[ipmc_id];
    
    if (!queue_node) {
        return BCM_E_INTERNAL; 
    }
    if (!ipmc_node->first_q_ptr) {
        ipmc_node->first_q_ptr = queue_node ;
        ipmc_node->last_q_ptr = queue_node ;
    } else {
        ipmc_node->last_q_ptr->next = queue_node ;
        ipmc_node->last_q_ptr = queue_node ;
    }
    queue_node->queue_id = queue_id; 
    _bcm_sb2_ipmc_update_queue_hash(unit, ipmc_id ) ;
    return BCM_E_NONE; 

}
int
_bcm_sb2_ipmc_subscriber_nexthop_node_get(int unit, int encap_id, 
                                          _bcm_ext_repl_q_list_t *queue_node,
                                          _bcm_ext_nh_list_t **nh_node) 
{
    _bcm_ext_nh_list_t *startnh; 

    if (!queue_node) {
        return BCM_E_INTERNAL; 

    }
    if(!nh_node) {  
       return BCM_E_INTERNAL; 
    }

    *nh_node = NULL; 
    for (startnh = queue_node->first_nh_ptr; startnh ; startnh=startnh->next) {
        if (startnh->encap_id == encap_id) {
            *nh_node = startnh;
            break;  
        }
    }
    return BCM_E_NONE;
}

int
 _bcm_sb2_ipmc_update_nh_hash(int unit, _bcm_ext_repl_q_list_t *queue_node) 

 {

    _bcm_ext_nh_list_t *startnh; 
    int partition = 0 ; 
    SHR_BITDCL *intf_vec = NULL;
    int alloc_size = 0; 
    int rv = 0; 

    if (!queue_node) {
        rv = BCM_E_INTERNAL;
        goto clean_up; 
    }
    alloc_size =  REPL_INTF_TOTAL(unit); 
    intf_vec = sal_alloc(alloc_size, "Repl interface vector");
    if (intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    sal_memset(intf_vec, 0, alloc_size);

    partition = soc_mem_index_count(unit, EGR_L3_INTFm);
    for (startnh = queue_node->first_nh_ptr; startnh ;
                               startnh = startnh->next) {
         if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, startnh->encap_id) &&
             (uint32)(startnh->encap_id) > partition) {
             rv = BCM_E_PARAM;
             goto clean_up;
         }
         if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, startnh->encap_id)) {
             /* L3 interface is used */
             SHR_BITSET(intf_vec,startnh->encap_id);
         } else {
             /* Next hop is used */
             SHR_BITSET(intf_vec, partition  + startnh->encap_id -
                              BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
         }
    }
    queue_node->repl_hash = 
           _shr_crc32b(0, (uint8 *)intf_vec, alloc_size); 
 clean_up: 
    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }
    return rv; 

}


int
 _bcm_ipmc_subscriber_update_nh_node(int unit, int ipmc_id, 
                                     _bcm_ext_repl_q_list_t *queue_node,
                                     _bcm_ext_nh_list_t *nh_node,
                                      int encap_id)
{

    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                       ext_ipmc_list[ipmc_id];  
    if (!queue_node || !nh_node) {
        return BCM_E_INTERNAL; 
    }
    if (queue_node->first_nh_ptr) { 
        queue_node->last_nh_ptr->next = nh_node ; 
        queue_node->last_nh_ptr =nh_node ;
    } else { 
        queue_node->first_nh_ptr = nh_node ; 
        queue_node->last_nh_ptr = nh_node ;  
    }
    queue_node->repl_count++;
    ipmc_node->repl_count++;
    nh_node->encap_id = encap_id; 
    _bcm_sb2_ipmc_update_nh_hash(unit, queue_node) ; 

     return BCM_E_NONE;
}


int
_bcm_sb2_ipmc_subscriber_remove_nh_node(int unit, int ipmc_id, 
                                        _bcm_ext_repl_q_list_t *queue_node,
                                       int id) 
{

    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                       ext_ipmc_list[ipmc_id];  
    _bcm_ext_nh_list_t *nh_node, *prev_node;
    
    prev_node = NULL;
    if (!queue_node) {
        return BCM_E_INTERNAL; 
    }
    
   for(nh_node = queue_node->first_nh_ptr; nh_node;
                            nh_node = prev_node->next) {
       if (nh_node->encap_id == id) {
           if(!prev_node) {
               queue_node->first_nh_ptr = nh_node->next;
               if (!queue_node->first_nh_ptr) {
                   queue_node->last_nh_ptr = 
                                queue_node->first_nh_ptr ;
               }
           } else {
             prev_node->next = nh_node->next ; 
             if (queue_node->last_nh_ptr == nh_node ) 
             {
                 queue_node->last_nh_ptr = prev_node;                    
             }
           }
           queue_node->repl_count--;
           ipmc_node->repl_count--;  
           _bcm_sb2_ipmc_update_nh_hash(unit, queue_node );  
           sal_free(nh_node);  
           break ;      
      }
      prev_node = nh_node ; 

   }
   return BCM_E_NONE; 
}

int
_bcm_sb2_ipmc_subscriber_remove_queue_node(int unit,
                         int ipmc_id, int queue_id) 

{

    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                     ext_ipmc_list[ipmc_id];  
    _bcm_ext_repl_q_list_t *queue_node, *prev_node; 
    
    prev_node = NULL;
    queue_node = ipmc_node->first_q_ptr; 

    for (; queue_node; queue_node = prev_node->next) {
         if (queue_node->queue_id == queue_id) {
             if (queue_node->repl_count) {
                 return BCM_E_INTERNAL; 
             }
             if (!prev_node) {
                 ipmc_node->first_q_ptr = queue_node->next;
                 if (!ipmc_node->first_q_ptr) {
                      ipmc_node->last_q_ptr = 
                                 ipmc_node->first_q_ptr ;
                 }
             } else {
               prev_node->next = queue_node->next; 
               if (ipmc_node->last_q_ptr == queue_node ) {
                   ipmc_node->last_q_ptr = prev_node ;                     
               }
             }
             _bcm_sb2_ipmc_update_queue_hash(unit, ipmc_id);
             sal_free(queue_node);
             break; 
         }
         prev_node = queue_node; 
    }
    return BCM_E_NONE; 
}


int
_bcm_sb2_ipmc_subscriber_egress_intf_get(int unit, int ipmc_id, int if_max,
                                         bcm_gport_t *port_array,
                                         bcm_if_t *if_array,
                                         bcm_gport_t *subs_array,
                                         int *count)
{


    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                           ext_ipmc_list[ipmc_id];  
    _bcm_ext_repl_q_list_t *queue_node; 
    _bcm_ext_nh_list_t *nh_node; 
    int repl_count = 0 ;
    int intf_max = IPMC_EXTQ_REPL_MAX(unit);;   
    BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, ipmc_id));
    if (if_max <= 0 ) {
        return BCM_E_PARAM;
    }

    for (queue_node = ipmc_node->first_q_ptr;
         queue_node; queue_node = queue_node->next) {
         for (nh_node = queue_node->first_nh_ptr ; 
              nh_node; nh_node = nh_node->next) {
              if_array[repl_count] = nh_node->encap_id;
              subs_array[repl_count] = queue_node->queue_id; 
              repl_count++; 
              if (repl_count == intf_max) {
                  goto intf_get_done;                         
              }
         }
    }
intf_get_done: 
    *count = repl_count ;  
    return BCM_E_NONE;     
}


/*
 * Function:
 *   _bcm_sb2_ipmc_subscriber_egress_intf_add     
 * Purpose:
 *     Adds the given encapid to the given queue in the given ipmc  
 * Parameters:
 *      unit     - SOC unit #
 *      ipmc_id   - ipmc_id 
 *      port     - physical port
 *      id       - next hop encap id 
 *      subscriber_queue - queue where replication need to happen
 *      is_l3    - set to true if its a l3 replication
 *      queue_count - replication count for the ipmc 
 *      qcount_increases - set to true if the replication count has changed
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_ipmc_subscriber_egress_intf_add(int unit, int ipmc_id,
                                        bcm_port_t port, 
                                        int id,
                                        bcm_gport_t subscriber_queue,
                                        int is_l3,
                                        int *queue_count,
                                        int *q_count_increased
                                        )
{
    _bcm_ext_repl_q_list_t *queue_node; 
    _bcm_ext_nh_list_t *nh_node ; 
    int queue_found = 0;
    int queue_id = 0;
    int qlist_deleted = 0; 
    int rv  = 0 ;
    int list_start_ptr = 0 ;
    int encap_id = 0 ;
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                           ext_ipmc_list[ipmc_id];  
    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, ipmc_id));
     
    if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, id)) {
        /* L3 interface is used */
         encap_id = id + IPMC_SB2_EGRESS_ID_MIN ;
    } else {
    /* Next hop is used */
         encap_id =   id  -
            BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + IPMC_SB2_NHOP_ID_MIN;
    }
    if (encap_id > IPMC_SB2_REPLICATION_INDEX_MAX) { 
        return BCM_E_PORT;  
    }
    if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                                _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                                NULL,  &queue_id, NULL)) < 0) {
          return rv;  
    }

    BCM_IF_ERROR_RETURN (
    _bcm_sb2_ipmc_subscriber_queue_node_get( unit, ipmc_id, 
                                                 queue_id, &queue_node));    
    if (queue_node)  { 
        queue_found = 1 ;
    } else { 
       queue_node = sal_alloc(sizeof(_bcm_ext_repl_q_list_t), 
                     "IPMC extended replication queue node ");
       if (!queue_node) { 
            return BCM_E_MEMORY; 
       }
       sal_memset(queue_node, 0, sizeof(_bcm_ext_repl_q_list_t));
    }
    if (!queue_found ) {
        _bcm_sb2_ipmc_subscriber_add_queue_node(unit, ipmc_id, queue_id, 
                                                             queue_node);
    }
    if (IS_EXT_MEM_PORT(unit, port)) {
        queue_node->buf_type = 1; 
    } 
    rv = _bcm_sb2_ipmc_subscriber_nexthop_node_get(unit, id, queue_node,
                                                           &nh_node);
    if (rv < 0 ) { 
        goto cleanup; 
    }
    if (nh_node ) {
        /* nhop is already present just return */ 
        return BCM_E_EXISTS ;   

    }
    else { 
       nh_node = sal_alloc(sizeof(_bcm_ext_nh_list_t),
                   "IPMC extended replication ");

       if (!nh_node) { 
          rv = BCM_E_MEMORY;
          goto cleanup ;  
       }
       sal_memset(nh_node, 0, sizeof(_bcm_ext_nh_list_t));
       _bcm_ipmc_subscriber_update_nh_node(unit, ipmc_id, queue_node, 
                                                       nh_node, id);
    }

    rv = _bcm_sb2_ipmc_subscriber_extq_update( unit, ipmc_id, 
                                              &qlist_deleted);
    if (rv < 0) {  
        goto cleanup; 
    } 
    *q_count_increased = 1;
    *queue_count = ipmc_node->repl_count;
    return BCM_E_NONE; 
cleanup :
   if (nh_node) { 
       _bcm_sb2_ipmc_subscriber_remove_nh_node(unit, ipmc_id, queue_node, id);  
   }
   if (!queue_found)
   {
       _bcm_sb2_ipmc_subscriber_remove_queue_node(unit, ipmc_id, queue_id);  
   }
   if (qlist_deleted) {
       list_start_ptr = 0;
       _sb2_extq_mc_group_ptr(unit, ipmc_id,
                        &list_start_ptr, 0, 1) ;
       qlist_deleted = 0 ; 
       _bcm_sb2_ipmc_subscriber_extq_update( unit, ipmc_id, 
                                              &qlist_deleted);
   }
   return rv ; 
}
STATIC int
_bcm_sb2_ipmc_subscriber_egress_cleanup(int unit, int ipmc_id, int id,
                                    int queue_removed, int nh_removed,
                                    _bcm_ext_repl_q_list_t *queue_node, 
                                    int queue_id ) 
{
    _bcm_ext_nh_list_t *nh_node ;
    bcm_port_t temp_port ; 
    if (queue_removed) { 
        queue_node = sal_alloc(sizeof(_bcm_ext_repl_q_list_t), 
                "IPMC extended replication queue node ");
        if (!queue_node) { 
            return BCM_E_MEMORY; 
        }
        sal_memset(queue_node, 0, sizeof(_bcm_ext_repl_q_list_t));
        _bcm_sb2_ipmc_subscriber_add_queue_node(unit, ipmc_id,queue_id, queue_node);
  
        BCM_IF_ERROR_RETURN(bcm_kt2_cosq_port_get(unit, queue_id, &temp_port));
        if (IS_EXT_MEM_PORT(unit, temp_port)) {
            queue_node->buf_type = 1; 
        } 
    }
    if (nh_removed) {
        nh_node = sal_alloc(sizeof(_bcm_ext_nh_list_t), "IPMC extended replication" 
                "replication node ");
        if(!nh_node ) { 
            return BCM_E_MEMORY;

        }
        sal_memset(nh_node, 0, sizeof(_bcm_ext_nh_list_t));
        _bcm_ipmc_subscriber_update_nh_node(unit, ipmc_id, queue_node, 
                nh_node, id);
    }
    return BCM_E_NONE;
} 

int 
_bcm_sb2_ipmc_subscriber_egress_intf_delete_all(int unit, int ipmc_id,
                                           bcm_port_t *port_array,
                                           int *encap_array,
                                           bcm_if_t *q_array,
                                           int *q_count_increased)
{
    _bcm_ext_repl_q_list_t *queue_node, *n_queue_node;
    int index = 0;
    int rv = 0;
    int nh_removed = 0;
    int queue_removed = 0;
    int qlist_deleted = 0 ;
    int list_start_ptr = 0;
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                        ext_ipmc_list[ipmc_id];  


    /*getting the ipmc_node and perform deletion task for all queues */
    queue_node = ipmc_node->first_q_ptr;
    while (queue_node) {
        n_queue_node = queue_node->next;
        rv = _bcm_sb2_ipmc_subscriber_remove_nh_node(unit, ipmc_id, 
                                        queue_node, encap_array[index]); 
        if (rv < 0) {
            if (rv == BCM_E_NOT_FOUND) { 
                rv = BCM_E_PARAM;
            }  
            return rv;
        }
        nh_removed = 1;

        if (!queue_node->repl_count) {
            rv =  _bcm_sb2_ipmc_subscriber_remove_queue_node
                (unit, ipmc_id, queue_node->queue_id);  
            if (BCM_FAILURE(rv)){
                _bcm_sb2_ipmc_subscriber_egress_cleanup(unit,ipmc_id,encap_array[index] ,
                        queue_removed, nh_removed,
                        queue_node, queue_node->queue_id);
                return rv;
            }
            queue_removed = 1 ; 
        }   
        rv = _bcm_sb2_ipmc_subscriber_extq_update(unit, ipmc_id, &qlist_deleted);
        if (BCM_FAILURE(rv)) {
            _bcm_sb2_ipmc_subscriber_egress_cleanup(unit,ipmc_id,encap_array[index] ,
                    queue_removed, nh_removed,
                    queue_node, queue_node->queue_id);
            if (qlist_deleted) {
                list_start_ptr = 0;
                _sb2_extq_mc_group_ptr(unit, ipmc_id,
                        &list_start_ptr, 0, 1) ;
                qlist_deleted = 0 ;        
                _bcm_sb2_ipmc_subscriber_extq_update(unit, ipmc_id, &qlist_deleted);
            }    
            return rv;
        }
        index++;
        queue_node = n_queue_node;
    }
    *q_count_increased = 1;
    return rv; 
}


/*
 * Function:
 *   _bcm_sb2_ipmc_subscriber_egress_intf_delete    
 * Purpose:
 *     Adds the given encapid to the given queue in the given ipmc  
 * Parameters:
 *      unit     - SOC unit #
 *      ipmc_id   - ipmc_id 
 *      port     - physical port
 *      id       - next hop encap id 
 *      subscriber_queue - queue where replication need to happen
 *      is_l3    - set to true if its a l3 replication
 *      queue_count - replication count for the ipmc 
 *      qcount_increases - set to true if the replication count has changed
 * Returns:
 *      BCM_E_XXX
 */ 
int
_bcm_sb2_ipmc_subscriber_egress_intf_delete(int unit, int ipmc_id,
                                        bcm_port_t port, 
                                        int id,
                                        bcm_gport_t subscriber_queue,
                                        int *queue_count,
                                        int *q_count_increased
                                        )
{

    _bcm_ext_repl_q_list_t *queue_node ; 
    _bcm_ext_nh_list_t *nh_node ; 
    int queue_id = 0;
    int rv = 0;
    int nh_removed = 0;
    int queue_removed = 0;
    int qlist_deleted = 0 ;
    int list_start_ptr = 0;
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                           ext_ipmc_list[ipmc_id];  
    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, ipmc_id));

    if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                                _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                                NULL,  &queue_id, NULL)) < 0) {
          return rv;  
    }

    BCM_IF_ERROR_RETURN (
    _bcm_sb2_ipmc_subscriber_queue_node_get( unit , ipmc_id, 
                                                 queue_id, &queue_node));    
    if (!queue_node)  { 
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_sb2_ipmc_subscriber_remove_nh_node(unit, ipmc_id, queue_node, id); 
    if (rv < 0) {
        if (rv == BCM_E_NOT_FOUND) { 
            rv = BCM_E_PARAM;
        }  
        return rv;
    }
    nh_removed = 1 ; 
    if (!queue_node->repl_count) {
        rv =  _bcm_sb2_ipmc_subscriber_remove_queue_node
                 (unit, ipmc_id, queue_id);  
        if (rv < 0) {
            goto clean_up;         
        }
        queue_removed = 1 ; 
    }
    rv = _bcm_sb2_ipmc_subscriber_extq_update(unit, ipmc_id, &qlist_deleted);
    if (rv < 0) {  
        goto clean_up; 
    }
    *q_count_increased = 1;
    *queue_count = ipmc_node->repl_count;
    return BCM_E_NONE; 

clean_up:
    if (queue_removed) { 
        queue_node = sal_alloc(sizeof(_bcm_ext_repl_q_list_t), 
                     "IPMC extended replication queue node ");
        if (!queue_node) { 
            return BCM_E_MEMORY; 
        }
        sal_memset(queue_node, 0, sizeof(_bcm_ext_repl_q_list_t));
        _bcm_sb2_ipmc_subscriber_add_queue_node(unit, ipmc_id,queue_id, queue_node);
        if (IS_EXT_MEM_PORT(unit, port)) {
            queue_node->buf_type = 1; 
        } 
    }
    if (nh_removed) {
        nh_node = sal_alloc(sizeof(_bcm_ext_nh_list_t), "IPMC extended replication" 
                                                              "replication node ");
        if(!nh_node ) { 
           return BCM_E_MEMORY;

        }
        sal_memset(nh_node, 0, sizeof(_bcm_ext_nh_list_t));
        _bcm_ipmc_subscriber_update_nh_node(unit, ipmc_id, queue_node, 
                                                       nh_node, id);
    }
    if (qlist_deleted)   {
        list_start_ptr = 0;
        _sb2_extq_mc_group_ptr(unit, ipmc_id,
                        &list_start_ptr, 0, 1) ;
        qlist_deleted = 0 ;        
        _bcm_sb2_ipmc_subscriber_extq_update(unit, ipmc_id, &qlist_deleted);

    }
    return rv; 
}


/*
 * Function:
 *   _bcm_sb2_ipmc_subscriber_egress_intf_delete    
 * Purpose:
 *      This is used to recover the software data from hardware
 *      in case of warmboot
 * Parameters:
 *      unit     - SOC unit #
 *      ipmc_id   - ipmc_id 
 *      extq_ptr  - first index in hardware
 *      q_vec     - q_vec for the given ipmc   
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_recover_subscriber_queue_entries(int unit, int extq_ptr, 
                                     SHR_BITDCL *q_vec, int ipmc_id,
                                     int *q_num)
{
    int     rv = BCM_E_NONE;
    int     i;
    int     next_ptr1, next_ptr2;
    int     queue_id;
    int     ptr, copy, buf_type=0; 
    mmu_ext_mc_queue_list0_entry_t     qlist0_entry;
    mmu_ext_mc_queue_list1_entry_t     qlist1_entry; 
    mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
    soc_field_t        base_qid[]  = { BASE_QID0f, BASE_QID1f }; 
    soc_field_t        repl_ptr[]  = { L3_REPL_PTR0f, L3_REPL_PTR1f };    
    soc_field_t        l2_copy[]   = { L2_COPY0f, L2_COPY1f };
    soc_field_t        buff_type[]  = { BUFF_TYPE0f, BUFF_TYPE1f };
    soc_field_t   profile_idx[] = { Q_OFFSET_PROF0f, Q_OFFSET_PROF1f };
    _bcm_ext_repl_q_list_t *queue_node = 0; 
    _bcm_ext_nh_list_t *nh_node = 0 ; 
    int queue_not_found = 0 ;

    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                          extq_ptr, &qlist0_entry)) < 0) {
        goto q_entries_done;
    }
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        if ((rv = READ_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                        extq_ptr, &qlist1_entry)) < 0) {
            goto q_entries_done;
        }
    } 
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                          extq_ptr, &qlist4_entry)) < 0) {
        goto q_entries_done;
    }

    /* store the content of queue list entries */
    for (i = 0; i < 2; i++) {
        queue_node = 0; 
        nh_node = 0 ;
        queue_not_found = 0; 
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, 
                                    &qlist0_entry, base_qid[i])) != 0) {    
            _bcm_sb2_ipmc_subscriber_queue_node_get( unit, ipmc_id,
                                 queue_id, &queue_node);
            if (!queue_node) {
                queue_not_found = 1;  
                queue_node = sal_alloc(sizeof(_bcm_ext_repl_q_list_t),
                             "IPMC extended replication queue node ");
                if (!queue_node) {
                    rv = BCM_E_MEMORY;  
                    goto q_entries_done;
                    
                }
                sal_memset(queue_node, 0, sizeof(_bcm_ext_repl_q_list_t));
                 _bcm_sb2_ipmc_subscriber_add_queue_node(unit, ipmc_id, queue_id,
                                                        queue_node); 
            }
            ptr = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry, 
                                                           repl_ptr[i]);
            if (ptr > IPMC_SB2_NHOP_ID_MIN) {
                 ptr =  (ptr - IPMC_SB2_NHOP_ID_MIN 
                              + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit)) ;
                 
            }
            else { 
                ptr = ptr - IPMC_SB2_EGRESS_ID_MIN ;                  
            }


            copy = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry, 
                                                           l2_copy[i]);
            if (soc_feature(unit, soc_feature_ddr3)) {
                buf_type = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry,
                                                            buff_type[i]);
            }
            nh_node = sal_alloc(sizeof(_bcm_ext_nh_list_t), "IPMC extended replication"
                                                                    "replication node ");
            if (!nh_node) {
                rv = BCM_E_MEMORY;
                goto q_entries_done ;
            }
            sal_memset(nh_node, 0, sizeof(_bcm_ext_nh_list_t));
            queue_node->l2_copy = copy ;  
            queue_node->buf_type = buf_type ;
            queue_node->profile_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry,
                                                                             profile_idx[i]);
            SHR_BITSET(q_vec, queue_id);
            if (queue_not_found) {
                (*q_num)++;
            }
            _bcm_ipmc_subscriber_update_nh_node(unit, ipmc_id,
                              queue_node, nh_node, ptr);

       }
    }
    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR2f);
    
    if (next_ptr1 != extq_ptr) {
        if ((rv = _bcm_sb2_recover_subscriber_queue_entries(unit, next_ptr1, q_vec, 
                                                  ipmc_id, q_num)) < 0) {
            goto q_entries_done;                                                                                                
        }                                                    
    } 
           
    if (next_ptr2 != extq_ptr) {
        if ((rv = _bcm_sb2_recover_subscriber_queue_entries(unit, next_ptr2, q_vec, 
                                                   ipmc_id, q_num)) < 0) {
            goto q_entries_done;                                                                                                        
        }                                                
    }    
    return BCM_E_NONE; 

q_entries_done:
    if (nh_node) {
       sal_free(nh_node);
    }
    if (queue_not_found && queue_node) {
        sal_free(nh_node); 
    }
    return rv;
}


#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_bcm_sb2_extq_repl_list_get(int unit, int extq_ptr,
                           int *list_ptr, 
                           int *list_num)
{
    int     rv = BCM_E_NONE;
    int     next_ptr1, next_ptr2;
    mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
    
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                    extq_ptr, &qlist4_entry)) < 0) {
        goto repl_list_get_done;
    }

    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR2f);
    IPMC_EXTQ_REPL_VE_USED_SET(unit, extq_ptr);
    list_ptr[*list_num] = extq_ptr;
    (*list_num)++;
    
    if (next_ptr1 != extq_ptr) {
        if ((rv = _bcm_sb2_extq_repl_list_get(unit, next_ptr1, list_ptr, 
             list_num)) < 0) {
            goto repl_list_get_done;   
        }                                                    
    } 
           
    if (next_ptr2 != extq_ptr) {
        if ((rv = _bcm_sb2_extq_repl_list_get(unit, next_ptr2, list_ptr,
             list_num)) < 0) {  
            goto repl_list_get_done; 
        }                                                
    }

repl_list_get_done:
    return rv;

}

STATIC int
_bcm_sb2_extq_repl_reload(int unit, int ipmc_id) 
{
    SHR_BITDCL *q_vec = NULL;
    int *extq_list_ptr;
    int q_max, q_num = 0;
    int list_max, list_num = 0;
    int alloc_size;
    int rv = BCM_E_NONE;
    int extq_ptr, extq_hash; 
    _bcm_repl_list_info_t *rli_start, *rli_current;
    int list_start_ptr, remap_id;
    int queue_count = 0;
    int queue_id, count = 0;
    bcm_port_t port;
    int list_size = 0;

    if ((rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, ipmc_id, 
                    &remap_id, FALSE)) < 0) {
         return rv;                
    }

    if ((rv = _sb2_extq_mc_group_ptr(unit, remap_id,
                                &extq_ptr, 0, 0)) < 0) {
        return rv;
    }

    if (extq_ptr == 0) {
        return rv;
    }
    q_max = IPMC_EXTQ_TOTAL(unit);
    rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);
    if (IPMC_EXTQ_REPL_VE_USED_GET(unit, extq_ptr)) {
        /* We've already traversed this list, just note it */
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
             if (rli_current->index == extq_ptr) {
                 (rli_current->refcount)++;
                 IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) =
                     rli_current->list_size;
                 break;
             }
        }
        if (rli_current == NULL) {
            /* Table out of sync.  Not good. */
            bcm_tr3_ipmc_repl_detach(unit);
            bcm_sb2_ipmc_repl_detach(unit);
            return BCM_E_INTERNAL;
        } else {
            return rv;
        }
    }


    list_max = IPMC_EXTQ_LIST_TOTAL(unit);
    alloc_size = list_max * sizeof(int);
    extq_list_ptr = sal_alloc(alloc_size, "IPMC extq list array");
    if (extq_list_ptr == NULL) {
        return BCM_E_MEMORY;
    }    
    sal_memset(extq_list_ptr, 0, alloc_size);
    if ((rv = _bcm_sb2_extq_repl_list_get(unit, extq_ptr, extq_list_ptr, 
                                         &list_num)) < 0 ) {
        goto reload_done;
    }    
        
    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        sal_free(extq_list_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);

    if ((rv = _bcm_sb2_recover_subscriber_queue_entries(unit, extq_ptr, q_vec, 
                                          ipmc_id, &q_num)) < 0 ) {
        goto reload_done;
    }
    
    if (q_num > q_max) {
        rv = BCM_E_PARAM;
        goto reload_done;
    }

    /* Search for list already in table */
    extq_hash =
        _shr_crc32b(0, (uint8 *)q_vec, IPMC_EXTQ_TOTAL(unit));

    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
         if (extq_hash == rli_current->hash) {
             rv = _bcm_sb2_extq_repl_list_compare(unit, ipmc_id,
                                            rli_current->first_ipmc);
             if (rv == BCM_E_NOT_FOUND) {
                 continue; /* Not a match */
             } else if (rv < 0) {
                 goto reload_done; /* Access error */
             } else {
                 break; /* Match */
             }
        }
    }

    if (rli_current != NULL) {
        /* Found a match, point to here and increase reference count */
        if (extq_ptr == rli_current->index) {
            /* We're already pointing to this list, so done */
            rv = BCM_E_NONE;
            goto reload_done;
        } else {
            list_start_ptr = rli_current->index;
            queue_count = rli_current->list_size;  
        }
    } else {
        /* Not a match, make a new chain */
        list_start_ptr = extq_ptr;
        list_size = (q_num % 2) ? ((q_num / 2) + 1) : (q_num / 2);
        queue_count = q_num;

        if (queue_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC extq repl list info");
            if (rli_current == NULL) {
                rv = BCM_E_MEMORY;
                goto reload_done;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = list_start_ptr;
            rli_current->hash = extq_hash;
            rli_current->next = rli_start;
            rli_current->list_size = list_size;
            rli_current->first_ipmc = ipmc_id ;
            IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
            (rli_current->refcount)++;
        }
    }

    IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) = list_size;

    count = 0;
    for (queue_id = 0; ((queue_id < q_max) && (count < queue_count));
        queue_id++) {
        if (SHR_BITGET(q_vec, queue_id)) {
            count++;
            if ((rv = bcm_kt2_cosq_port_get(unit, queue_id, &port)) < 0) {
                goto reload_done;
            }
            _bcm_sb2_ipmc_port_ext_queue_count_increment(unit, ipmc_id, port);
        }
    }

reload_done:
    if (extq_list_ptr != NULL) {
        sal_free(extq_list_ptr);
    }    
    if (q_vec != NULL) {
        sal_free(q_vec);
    }

    return rv; 
}
#endif 

int
_bcm_sb2_ipmc_subscriber_egress_intf_set(int unit, int ipmc_id,
                                        bcm_port_t port, int if_count, 
                                        int *id,
                                        bcm_gport_t subscriber_queue,
                                        int is_l3,
                                        int *queue_count,
                                        int *q_count_increased) 
{

   int i = 0 ; 
   int rv = 0 ; 
   for (i = 0; i < if_count; i++) 
   {
       rv =  _bcm_sb2_ipmc_subscriber_egress_intf_add(unit, ipmc_id, port,
                                         id[i], subscriber_queue, is_l3,
                                      queue_count, q_count_increased);
       if ( rv < 0) {
            return rv; 
       }
   }
   return BCM_E_NONE; 
}


void _bcm_sb2_ipmc_port_ext_queue_count_increment(int unit,
                                      int ipmc_id, bcm_port_t port)
{
     if ((IPMC_EXTQ_REPL_PORT_INFO(unit, port) != NULL) &&
         (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count != NULL)) {
     IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit, port, ipmc_id)++;
     }
     if (IS_EXT_MEM_PORT(unit, port)) {
        IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(unit, ipmc_id)++;
     } else {
        IPMC_EXTQ_REPL_QUEUE_COUNT_INT(unit, ipmc_id)++;
     }
     return;
}

void _bcm_sb2_ipmc_port_ext_queue_count_decrement(int unit,
                                     int ipmc_id, bcm_port_t port)
{
     if ((IPMC_EXTQ_REPL_PORT_INFO(unit, port) != NULL) &&
         (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count != NULL)) {
     IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit, port, ipmc_id)--;
     }
     if (IS_EXT_MEM_PORT(unit, port)) {
        IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(unit, ipmc_id)--;
     } else {
        IPMC_EXTQ_REPL_QUEUE_COUNT_INT(unit, ipmc_id)--;
     }
     return;
}

int _bcm_sb2_ipmc_port_ext_queue_count(int unit,
                                      int ipmc_id, bcm_port_t port)
{
     /* coverity[overrun-local : FALSE] */
     if (SOC_PORT_VALID(unit, port)) {
     return IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit, port, ipmc_id);
     } else {
         return BCM_E_PORT;
     }
}

#endif

#ifdef BCM_KATANA2_SUPPORT

/* Function:
 *      _bcm_kt2_repl_list_tbl_ptr_get
 * Purpose:
 *      Get the appropriate MMU replication tables and intended 
 *      member bitmap last bitmap fields for  for port
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      port       - (IN) port. 
 *      info       - (OUT) MMU group table info 
*                     mem, pbmp, index corresponding to the port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_repl_list_tbl_ptr_get(int unit, bcm_port_t port,
                               _kt2_repl_grp_info_t *info)
{
#ifdef BCM_SABER2_SUPPORT
    if (soc_mem_field_valid(unit, MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTSf)) {
        /* saber2 */
        return (_bcm_sb2_repl_list_tbl_ptr_get (unit, port, info));
    }
#endif
#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        return _bcm_metrolite_repl_list_tbl_ptr_get(unit, port, info);
    }
#endif
    if (info == NULL)
    {
       return BCM_E_PARAM;
    }
    if ((port >= 0) && (port < 70))
    {
        info->mb_mem =  MMU_REPL_GRP_TBL0m;
        info->mbmp = MEMBER_BMP_PORTS_119_0f;
        info->lb_mem = MMU_REPL_GRP_TBL1m;
        info->lbmp =  LAST_BMP_PORTS_69_0f;
        info->mb_index = port;
        info->lb_index = port;
    } else if ((port >= 70) && (port < 120)) {
        info->mb_mem =  MMU_REPL_GRP_TBL0m;
        info->mbmp = MEMBER_BMP_PORTS_119_0f;
        info->lb_mem = MMU_REPL_GRP_TBL2m;
        info->lbmp =  LAST_BMP_PORTS_169_70f;
        info->mb_index = port;
        info->lb_index = port - 70;
    } else if ((port >= 120) && (port < 170)) {
        info->mb_mem =  MMU_REPL_GRP_TBL1m;
        info->mbmp = MEMBER_BMP_PORTS_169_120f;
        info->lb_mem = MMU_REPL_GRP_TBL2m;
        info->lbmp =  LAST_BMP_PORTS_169_70f;
        info->mb_index = port - 120;
        info->lb_index = port - 70;
    }

    return BCM_E_NONE;
}

/* Function:
 *      _bcm_kt2_repl_group_info_set
 * Purpose:
 *      Set replication group info.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      repl_group - (IN) The replication group number.
 *      port       - (IN) port.
 *      mbmp       - (IN) Member bitmap.
 *      lbmp       - (IN) Last Member bitmap.
 *      head_index - (IN) Base pointer to REPL_HEAD table.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_repl_group_info_set(int unit, int repl_group, bcm_port_t port,
        soc_pbmp_t mbmp, soc_pbmp_t lbmp, int head_index)
{

    mmu_repl_grp_tbl0_entry_t repl_grp_tbl0_entry;
    mmu_repl_grp_tbl1_entry_t repl_grp_tbl1_entry;
    mmu_repl_grp_tbl2_entry_t repl_grp_tbl2_entry;
    _kt2_repl_grp_info_t kt2_repl_grp_info;
    int member_bitmap_width0;
    int member_bitmap_width1;
    soc_pbmp_t member_bitmap0, member_bitmap1;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int i;

#ifdef BCM_SABER2_SUPPORT
    if (soc_mem_field_valid(unit, MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTSf)) {
        /* saber2 */
        return (_bcm_sb2_repl_group_info_set(unit, 
                    repl_group, port, mbmp,
                    lbmp, head_index)); 
    }
#endif
#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        return (_bcm_metrolite_repl_group_info_set(unit,
                    repl_group, port, mbmp,
                    lbmp, head_index));
    }
#endif
    sal_memset(&repl_grp_tbl0_entry, 0, sizeof(repl_grp_tbl0_entry));
    sal_memset(&repl_grp_tbl1_entry, 0, sizeof(repl_grp_tbl1_entry));
    sal_memset(&repl_grp_tbl2_entry, 0, sizeof(repl_grp_tbl2_entry));
    sal_memset(&kt2_repl_grp_info, 0, sizeof(kt2_repl_grp_info));

    BCM_IF_ERROR_RETURN(_bcm_kt2_repl_list_tbl_ptr_get (unit, port,
               &kt2_repl_grp_info));
     
    member_bitmap_width0 =  soc_mem_field_length(unit,
                MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTS_119_0f);

    member_bitmap_width1 =  soc_mem_field_length(unit,
                MMU_REPL_GRP_TBL1m, MEMBER_BMP_PORTS_169_120f);

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));

    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(lbmp, i);
    }
    if (port >=70) {
        soc_mem_field_set(unit, kt2_repl_grp_info.lb_mem,
           (uint32 *)&repl_grp_tbl2_entry, kt2_repl_grp_info.lbmp, fldbuf);
    } else {
        soc_mem_field_set(unit, kt2_repl_grp_info.lb_mem,
            (uint32 *)&repl_grp_tbl1_entry, kt2_repl_grp_info.lbmp, fldbuf);
    }

    soc_MMU_REPL_GRP_TBL2m_field32_set(unit, &repl_grp_tbl2_entry,
                        BASE_PTRf, head_index);

    SOC_PBMP_CLEAR(member_bitmap0);
    for (i = 0; i < member_bitmap_width0; i++) {
        if (SOC_PBMP_MEMBER(mbmp, i)) {
            SOC_PBMP_PORT_ADD(member_bitmap0, i);
        }
    }

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(member_bitmap0, i);
    }

    soc_MMU_REPL_GRP_TBL0m_field_set(unit, &repl_grp_tbl0_entry,
            MEMBER_BMP_PORTS_119_0f, fldbuf);

    SOC_PBMP_CLEAR(member_bitmap1);
    for (i = member_bitmap_width0;
         i < (member_bitmap_width0 + member_bitmap_width1);
         i++) {
        if (SOC_PBMP_MEMBER(mbmp, i)) {
            SOC_PBMP_PORT_ADD(member_bitmap1, (i - member_bitmap_width0));
        }
    }

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(member_bitmap1, i);
    }

    soc_MMU_REPL_GRP_TBL1m_field_set(unit, &repl_grp_tbl1_entry,
            MEMBER_BMP_PORTS_169_120f, fldbuf);

    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GRP_TBL1m(unit,
                    MEM_BLOCK_ALL, repl_group, &repl_grp_tbl1_entry));
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GRP_TBL2m(unit,
                    MEM_BLOCK_ALL, repl_group, &repl_grp_tbl2_entry));
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GRP_TBL0m(unit,
                    MEM_BLOCK_ALL, repl_group, &repl_grp_tbl0_entry));

    return BCM_E_NONE;
}

#endif /* BCM_KATANA2_SUPPORT*/

/* --------------------------------------------------------------
 * The following set of routines manage replication lists.
 * --------------------------------------------------------------
 */
/*
 * Function:
 *      bcm_tr3_ipmc_repl_detach
 * Purpose:
 *      Deinitialize replication.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_detach(int unit)
{
    bcm_port_t          port;
    _bcm_repl_list_info_t *rli_current, *rli_free;
    bcm_pbmp_t            pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_all);
    }
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &pbmp_all));
        BCM_IF_ERROR_RETURN(bcm_kt_ipmc_repl_detach(unit));
    }
#endif

    if (_tr3_repl_info[unit] != NULL) {
        PBMP_ITER(pbmp_all, port) {
            /* Coverity issue ignored:
             * Maximum port number used in PBMP_PORT_ALL
             * should never exceed SOC_MAX_NUM_PP_PORTS */
            /* coverity[overrun-local : FALSE] */
            if (_tr3_repl_info[unit]->port_info[port] != NULL) {
                if (_tr3_repl_info[unit]->
                        port_info[port]->intf_count != NULL) {
                    sal_free(_tr3_repl_info[unit]->
                            port_info[port]->intf_count);
                }
                sal_free(_tr3_repl_info[unit]->port_info[port]);
            }
        }

        if (_tr3_repl_info[unit]->repl_list_info != NULL) {
            rli_current = _tr3_repl_info[unit]->repl_list_info;
            while (rli_current != NULL) {
                rli_free = rli_current;
                rli_current = rli_current->next;
                sal_free(rli_free);
            }
        }

        if (_tr3_repl_info[unit]->l3_intf_next_hop_ipmc != NULL) {
            sal_free(_tr3_repl_info[unit]->l3_intf_next_hop_ipmc);
        }
        if (_tr3_repl_info[unit]->l3_intf_next_hop_trill != NULL) {
            sal_free(_tr3_repl_info[unit]->l3_intf_next_hop_trill);
        }

        sal_free(_tr3_repl_info[unit]);
        _tr3_repl_info[unit] = NULL;
    }

    _bcm_tr3_repl_head_info_deinit(unit);
    _bcm_tr3_repl_list_entry_info_deinit(unit);

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        if (soc_property_get(unit, spn_MULTICAST_PER_TRUNK_REPLICATION, 0)) {
            BCM_IF_ERROR_RETURN(bcm_td2p_aggid_info_detach(unit));
        }
    }
#endif

    return BCM_E_NONE;
}

#ifdef BCM_TRIDENT2PLUS_SUPPORT
/*
 * Function:
 *      _bcm_td2p_repl_port_agg_map_init
 * Purpose:
 *      Initialize L3MC Port Agg Map.
 *      From Trident 2 Plus MMU uArch Spec Chapter 5. TD2PLUS MMU - L3MC Replication
 *      Software must initialize this table on boot time.
 *      Software should initialize every L3MC-Port-Agg-ID to be the same
 *      value as the MMU port to retain backward compatibility to Trident2 functionality.
 *      Since L3MC-Port-Agg-ID is used per egress pipe,
 *      the maximum L3MC-Port-Agg-IDs needed matches the
 *      maximum number of ports per pipe which is 53.
 *      Therefore, only the lower six bits are needed for L3MC-Port-Agg-ID by the hardware.
 *      But for registers in the ENQ stage used in cut-through mode,
 *      PIPE_NUM field must be programmed because L3MC-Port-Agg-ID is used for
 *      whole port space.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2p_repl_port_agg_map_init(int unit)
{
    soc_info_t *si;
    int phy_port, mmu_port;
    bcm_port_t port;
    uint32 regval;
    int max_ports = 0 ;
    si = &SOC_INFO(unit);
#ifdef BCM_APACHE_SUPPORT
    if(SOC_IS_APACHE(unit))
    {
        max_ports = 74 ;
    } else 
#endif 
    {
        max_ports = 64 ;
    }


    if (soc_property_get(unit, spn_MULTICAST_PER_TRUNK_REPLICATION, 0)) {
        BCM_MC_PER_TRUNK_REPL_MODE(unit) = TRUE;
        BCM_IF_ERROR_RETURN(bcm_td2p_aggid_info_init(unit));
    } else {
        BCM_MC_PER_TRUNK_REPL_MODE(unit) = FALSE;
    }

    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_NONE;
    }
    PBMP_ITER(PBMP_ALL(unit), port) {
        if (IS_RDB_PORT(unit, port)) {
            continue;
        }
        /* Coverity issue ignored:
         * Maximum port number used in PBMP_ALL
         * should never exceed SOC_MAX_NUM_PP_PORTS */
        /* coverity[value_overwrite] */
        /* coverity[overrun-local : FALSE] */

        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* configure mmu port to repl aggregateId map */
        regval = 0;
        soc_reg_field_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, &regval,
                L3MC_PORT_AGG_IDf,
                BCM_MC_PER_TRUNK_REPL_MODE(unit) ? TD2P_AGG_ID_HW_INVALID :
                (mmu_port % max_ports));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr,
                    port,0,regval));
        regval = 0;
        soc_reg_field_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, &regval,
                          L3MC_PORT_AGG_IDf,
                          BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                          TD2P_AGG_ID_HW_INVALID : mmu_port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr,
                            port, 0, regval));
        regval = 0;
        soc_reg_field_set(unit, MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr, &regval,
                          L3MC_PORT_AGG_IDf,
                          BCM_MC_PER_TRUNK_REPL_MODE(unit) ?
                          TD2P_AGG_ID_HW_INVALID : mmu_port);
        SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr(unit,
                            port, regval));

    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
int
bcm_sb2_ipmc_repl_extq_port_alloc(int unit, bcm_port_t port) {
    int alloc_size;

    alloc_size = sizeof(_sb2_repl_queue_info_t);
    /* Coverity issue ignored:
     * Maximum port number used in PBMP_PORT_ALL
     * should never exceed SOC_MAX_NUM_PP_PORTS */
    /* coverity[overrun-local : FALSE] */
    if (!IPMC_EXTQ_REPL_PORT_INFO(unit, port)) {      
        IPMC_EXTQ_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC Extq repl port info");
        if (IPMC_EXTQ_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_tr3_ipmc_repl_detach(unit); 
            bcm_sb2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    } 
    sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port), 0, alloc_size);

    alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
    if (!IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count) { 
        IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count =
            sal_alloc(alloc_size, "IPMC Extq repl port queue count");
        if (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count == NULL) {
            bcm_tr3_ipmc_repl_detach(unit); 
            bcm_sb2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count,
                                  0, alloc_size);
    return BCM_E_NONE; 
}
#endif

/*
 * Function:
 *      _bcm_tr3_ipmc_repl_init
 * Purpose:
 *      Initialize replication.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_init(int unit)
{
    soc_info_t *si;
    int member_count;
    int phy_port, mmu_port;
    bcm_port_t port;
    int alloc_size;
    int i;
    int rv = BCM_E_NONE;
    soc_pbmp_t member_bitmap;
    int member_bitmap_index;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    mmu_repl_group_entry_t repl_group_entry;
    uint32 regval;
    int num_lanes, count_width;
#ifdef BCM_KATANA2_SUPPORT 
    mmu_repl_map_tbl_entry_t map_entry;
    uint32 pp_port = 0;
    uint32 repl_map_idx_pp = 0;
    uint32 repl_map_port_ct = 0;
    uint32 toq_entry = 0;
#endif
    bcm_pbmp_t          pbmp_all;
#ifdef BCM_SABER2_SUPPORT    
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
#endif
    BCM_PBMP_CLEAR (pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_all);
    }
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &pbmp_all));
    }
#endif

    bcm_tr3_ipmc_repl_detach(unit);

    if (NULL == _tr3_repl_info[unit]) {
       _tr3_repl_info[unit] = sal_alloc(sizeof(_tr3_repl_info_t), "repl info");
       if (NULL == _tr3_repl_info[unit]) {
           bcm_tr3_ipmc_repl_detach(unit);
           return BCM_E_MEMORY;
       }
    }
    sal_memset(_tr3_repl_info[unit], 0, sizeof(_tr3_repl_info_t));

    _tr3_repl_info[unit]->num_repl_groups = soc_mem_index_count(unit, L3_IPMCm);

    if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {
        /* Each replication group is statically allocated a region
         * in REPL_HEAD table. The size of the region depends on the
         * maximum number of valid ports. Thus, the max number of
         * replication groups is limited to number of REPL_HEAD entries
         * divided by the max number of valid ports.
         */
        si = &SOC_INFO(unit);
        member_count = 0;
        PBMP_ITER(SOC_CONTROL(unit)->repl_eligible_pbmp, port) {
        if (SOC_IS_KATANA2(unit)) {
            phy_port = port;
            mmu_port = port;
        } else {
            /* Coverity issue ignored:
             * Maximum port number used in repl_eligible_pbmp
             * should never exceed SOC_MAX_NUM_PP_PORTS */
            /* coverity[value_overwrite] */
            /* coverity[overrun-local : FALSE] */
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
        }
            if ((!SOC_IS_KATANA2(unit)) && 
                ((mmu_port == 57) || (mmu_port == 59) || 
                 (mmu_port == 61) || (mmu_port == 62))) {
                /* No replication on MMU ports 57, 59, 61 and 62 */
                continue;
            }
            member_count++;
        }
        if (member_count > 0) {
            _tr3_repl_info[unit]->num_repl_groups =
                soc_mem_index_count(unit, MMU_REPL_HEAD_TBLm) / member_count;
            if (_tr3_repl_info[unit]->num_repl_groups > 
                    soc_mem_index_count(unit, L3_IPMCm)) {
                _tr3_repl_info[unit]->num_repl_groups =
                    soc_mem_index_count(unit, L3_IPMCm);
            }
        }
    }

    /* The total number of replication interfaces equals to the total
     * number of next hops. Unlike previous XGS3 devices, each L3 inteface
     * will be associated with a next hop index.
     */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit))
    {
        _tr3_repl_info[unit]->num_intf = 
            (soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) + 
             soc_mem_index_count(unit, EGR_L3_INTFm));
        if (!SOC_IS_SABER2(unit)) {
            rv = bcm_kt2_ipmc_repl_init(unit);
            if (BCM_FAILURE(rv)) {
                bcm_tr3_ipmc_repl_detach(unit);
                return rv;
             }  
        }
    } else 
#endif /* BCM_KATANA2_SUPPORT */
    {
        _tr3_repl_info[unit]->num_intf = soc_mem_index_count(unit,
            EGR_L3_NEXT_HOPm);

        if (NULL == _tr3_repl_info[unit]->l3_intf_next_hop_ipmc) {
            _tr3_repl_info[unit]->l3_intf_next_hop_ipmc =
                sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                        "l3_intf_next_hop_ipmc");
            if (NULL == _tr3_repl_info[unit]->l3_intf_next_hop_ipmc) {
                bcm_tr3_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
        }
        for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
            _tr3_repl_info[unit]->l3_intf_next_hop_ipmc[i] = REPL_NH_INDEX_UNALLOCATED;
        }

        if (NULL == _tr3_repl_info[unit]->l3_intf_next_hop_trill) {
            _tr3_repl_info[unit]->l3_intf_next_hop_trill =
                sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                        "l3_intf_next_hop_trill");
            if (NULL == _tr3_repl_info[unit]->l3_intf_next_hop_trill) {
                bcm_tr3_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
        }
        for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
            _tr3_repl_info[unit]->l3_intf_next_hop_trill[i] = -1;
        }

    }

    /* PBMP_ITER(PBMP_ALL(unit), port)  */
    SOC_PBMP_ITER(pbmp_all, port) { 
        /* Coverity issue ignored:
         * Maximum port number used in PBMP_PORT_ALL
         * should never exceed SOC_MAX_NUM_PP_PORTS */
        /* coverity[overrun-local : FALSE] */
        if (NULL == _tr3_repl_info[unit]->port_info[port]) {
            _tr3_repl_info[unit]->port_info[port] =
                sal_alloc(sizeof(_tr3_repl_port_info_t), "repl port info");
            if (NULL == _tr3_repl_info[unit]->port_info[port]) {
                bcm_tr3_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(_tr3_repl_info[unit]->port_info[port], 0,
                sizeof(_tr3_repl_port_info_t));

        alloc_size = sizeof(int) * _tr3_repl_info[unit]->num_repl_groups;
        if (NULL == _tr3_repl_info[unit]->port_info[port]->intf_count) {
            _tr3_repl_info[unit]->port_info[port]->intf_count = 
                sal_alloc(alloc_size, "repl port intf count");
            if (NULL == _tr3_repl_info[unit]->port_info[port]->intf_count) {
                bcm_tr3_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(_tr3_repl_info[unit]->port_info[port]->intf_count, 0,
                alloc_size);
    }


    rv = _bcm_tr3_repl_head_info_init(unit);
    if (BCM_FAILURE(rv)) {
        bcm_tr3_ipmc_repl_detach(unit);
        return rv;
    }

    rv = _bcm_tr3_repl_list_entry_info_init(unit);
    if (BCM_FAILURE(rv)) {
        bcm_tr3_ipmc_repl_detach(unit);
        return rv;
    }

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        rv = _bcm_td2p_repl_port_agg_map_init(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif

    if (!SOC_WARM_BOOT(unit)) {
        if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {

            /* Set MEMBER_BITMAP and LAST_BITMAP fields */
            si = &SOC_INFO(unit);
            SOC_PBMP_CLEAR(member_bitmap);
            member_count = 0;
            PBMP_ITER(SOC_CONTROL(unit)->repl_eligible_pbmp, port) {
                phy_port = si->port_l2p_mapping[port];
                mmu_port = si->port_p2m_mapping[phy_port];

                if ((mmu_port == 57) || (mmu_port == 59) ||
                    (mmu_port == 61) || (mmu_port == 62)) {
                    /* No replication on MMU ports 57, 59, 61, and 62 */
                    continue;
                } else if (mmu_port == 60) {
                    member_bitmap_index = 59;
                } else {
                    member_bitmap_index = mmu_port;
                }

                SOC_PBMP_PORT_ADD(member_bitmap, member_bitmap_index);
                member_count++;
            }

 
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                fldbuf[i] = SOC_PBMP_WORD_GET(member_bitmap, i);
            }
 
            sal_memset(&repl_group_entry, 0, sizeof(mmu_repl_group_entry_t));
            soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                      MEMBER_BITMAPf, fldbuf);
            soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                      LAST_BITMAPf, fldbuf);

            /* Set HEAD_INDEXf field */
            for (i = 0; i < _tr3_repl_info[unit]->num_repl_groups; i++) {
                soc_MMU_REPL_GROUPm_field32_set(unit, &repl_group_entry,
                          HEAD_INDEXf, i * member_count);
                SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUPm(unit, MEM_BLOCK_ALL,
                              i, &repl_group_entry));
            }

        } else {
            /* Clear replication group table */
            if (soc_mem_is_valid(unit, MMU_REPL_GROUP_INFO0m)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_REPL_GROUP_INFO0m, MEM_BLOCK_ALL,
                                   0));

                /* APACHE does not have MMU_REPL_GROUP_INFO1m */
                if (soc_mem_is_valid(unit, MMU_REPL_GROUP_INFO1m)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_REPL_GROUP_INFO1m, MEM_BLOCK_ALL,
                                   0));
                }
            } else {
#ifdef  BCM_METROLITE_SUPPORT 
                if (SOC_IS_METROLITE(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, MMU_REPL_GRP_TBLm, MEM_BLOCK_ALL, 0));
                    repl_map_idx_pp =  _BCM_ML_MAX_EXT_STREAMS_PER_LINKPHY_PORT;
                    repl_map_port_ct =  ML_MAX_LOGICAL_PORTS;
                    sal_memset(&map_entry, 0, sizeof(mmu_repl_map_tbl_entry_t));
                    for (i = repl_map_idx_pp;((i < repl_map_port_ct + repl_map_idx_pp) && 
                                             (i < soc_mem_index_count(unit,MMU_REPL_MAP_TBLm)));
                                              i++) {
                        pp_port = i - repl_map_idx_pp;
                        soc_MMU_REPL_MAP_TBLm_field_set(unit, &map_entry,
                                                       DEST_PPPf, &pp_port);
                        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_MAP_TBLm(unit,
                                   MEM_BLOCK_ALL, i, &map_entry));
                    }          
	        } else
#endif
#ifdef  BCM_KATANA2_SUPPORT 
                if (SOC_IS_KATANA2(unit)) {
                    SOC_IF_ERROR_RETURN(soc_mem_clear(unit,
                                        MMU_REPL_GRP_TBL1m, MEM_BLOCK_ALL, 0));
                    if (soc_mem_is_valid(unit, MMU_REPL_GRP_TBL2m)) {
                    SOC_IF_ERROR_RETURN(soc_mem_clear(unit, 
                                        MMU_REPL_GRP_TBL2m, MEM_BLOCK_ALL, 0));
                        repl_map_idx_pp =  (soc_mem_index_count(unit,
                               MMU_REPL_MAP_TBLm) - SOC_INFO(unit).pp_port_index_min);
                    } 
                    SOC_IF_ERROR_RETURN(soc_mem_clear(unit,
                                        MMU_REPL_GRP_TBL0m, MEM_BLOCK_ALL, 0));
                    sal_memset(&map_entry, 0, sizeof(mmu_repl_map_tbl_entry_t));
                    repl_map_port_ct = KT2_MAX_LOGICAL_PORTS;
                    READ_TOQ_GEN_CFGr(unit, &toq_entry);
                    soc_reg_field_set(unit, TOQ_GEN_CFGr,
                            &toq_entry, CFG_REPL_GRP_ATOMIC_ENf,1);
                    WRITE_TOQ_GEN_CFGr(unit, toq_entry);

                    /* MMU_REPL_MAP_TBL table is programmed to map 256 - 297 
                     * indices to pp ports 1-41 
                     */

#ifdef BCM_SABER2_SUPPORT
                    /* 0-63 are for Link phy ports */
                    if (SOC_IS_SABER2(unit)) {
                        repl_map_idx_pp =  SOC_INFO(unit).subport_port_max;
                        repl_map_port_ct =  SB2_MAX_LOGICAL_PORTS;
                    }
#endif

                    for (i = repl_map_idx_pp;((i < repl_map_port_ct + repl_map_idx_pp) && (i < soc_mem_index_count(unit,MMU_REPL_MAP_TBLm)));
                            i++) {
                        pp_port = i - repl_map_idx_pp;
                        soc_MMU_REPL_MAP_TBLm_field_set(unit, &map_entry, 
                                                       DEST_PPPf, &pp_port);
                        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_MAP_TBLm(unit,
                                   MEM_BLOCK_ALL, i, &map_entry));
                    }  
                } else 
#endif /* BCM_KATANA2_SUPPORT */
                { 
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, MMU_REPL_GROUPm, MEM_BLOCK_ALL, 0));

                }
            }
 
            /* Clear replication group initial count table */
            if (soc_mem_is_valid(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNTm)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
                                   MEM_BLOCK_ALL, 0));
            }

            /* Initialize PORT_INITIAL_COPY_COUNT_WIDTH registers */
            if (SOC_REG_IS_VALID(unit, PORT_INITIAL_COPY_COUNT_WIDTHr)) {
                PBMP_ITER(PBMP_ALL(unit), port) {
                    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                        count_width = 2;
                    } else if (IS_RDB_PORT(unit, port)) {
                        continue;
                    } else if (SOC_IS_APACHE(unit)) {
                        if (SOC_INFO(unit).port_speed_max[port] < 25000) {
                            count_width = 1; /* 1 ICC bit  */
                        } else if (SOC_INFO(unit).port_speed_max[port] <= 53000) {
                            count_width = 2; /* 2 ICC bits  */
                        } else {
                            count_width = 3; /* 3 ICC bits  */
                        }
                    } else if (SOC_IS_TRIDENT2PLUS(unit)) {
                        if (SOC_INFO(unit).port_speed_max[port] <= 11000) {
                            /* max speed could be 11G */
                            count_width = 1; /* 1 ICC bit  */
                        } else if (SOC_INFO(unit).port_speed_max[port] <= 42000) {
                            /* max speed could be 42G */
                            count_width = 2; /* 2 ICC bits  */
                        } else {
                            count_width = 3; /* 3 ICC bits  */
                        }
                    } else {
                        num_lanes = SOC_INFO(unit).port_num_lanes[port];
                        switch (num_lanes) {
                            case 1: count_width = 1;
                                    break;
                            case 2: count_width = 2;
                                    break;
                            case 4: count_width = 3;
                                    break;
                            default: count_width = 0;
                                     break;
                        }
                    }
                    regval = 0;
                    soc_reg_field_set(unit, PORT_INITIAL_COPY_COUNT_WIDTHr,
                            &regval, BIT_WIDTHf,
                            count_width ? (count_width - 1) : 0);
                    SOC_IF_ERROR_RETURN
                        (WRITE_PORT_INITIAL_COPY_COUNT_WIDTHr(unit, port,
                                                              regval));
                }
            }
        }
    } 
#ifdef BCM_SABER2_SUPPORT
     if (SOC_IS_SABER2(unit)) {

         /* Release SB2 ipmc structures for re-allocation */
         bcm_sb2_ipmc_repl_detach(unit);

         if (!_sb2_extq_repl_info[unit]) {  
             /* Allocate struct for IPMC Extq replication bookkeeping */
             alloc_size = sizeof(_sb2_extq_repl_info_t);
             _sb2_extq_repl_info[unit] = sal_alloc(alloc_size, "IPMC Extq repl info");
             if (_sb2_extq_repl_info[unit] == NULL) {
                 bcm_tr3_ipmc_repl_detach(unit); 
                 return BCM_E_MEMORY;
             }
             sal_memset(_sb2_extq_repl_info[unit], 0, alloc_size);
             _sb2_extq_repl_info[unit]->ipmc_size =
                           soc_mem_index_count(unit, L3_IPMCm);
         } 

         alloc_size = soc_mem_index_count(unit, L3_IPMCm); 
         if (!_sb2_extq_repl_info[unit]->ext_ipmc_list) {   
             _sb2_extq_repl_info[unit]->ext_ipmc_list = 
                  sal_alloc(alloc_size * sizeof(_bcm_ext_q_ipmc_t),
                                    "Extended queue ipmc nodes");
             if (!_sb2_extq_repl_info[unit]->ext_ipmc_list) {
                 bcm_tr3_ipmc_repl_detach(unit);
                 bcm_sb2_ipmc_repl_detach(unit);
                 return BCM_E_MEMORY;  
             }
         } 
         sal_memset(_sb2_extq_repl_info[unit]->ext_ipmc_list, 0,
                             alloc_size * sizeof(_bcm_ext_q_ipmc_t));

         IPMC_EXTQ_REPL_MAX(unit) = (1 << soc_mem_field_length(unit, L3_IPMCm,
                                     EXT_Q_NUM_COPIESf)) - 1;
         IPMC_EXTQ_TOTAL(unit) = 1024;
         IPMC_EXTQ_LIST_TOTAL(unit) = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST0m);

         alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_LIST_TOTAL(unit));
         if (!_sb2_extq_repl_info[unit]->bitmap_entries_used) {  
             _sb2_extq_repl_info[unit]->bitmap_entries_used =
             sal_alloc(alloc_size, "IPMC Extq repl entries used");
             if (_sb2_extq_repl_info[unit]->bitmap_entries_used == NULL) {
                 bcm_tr3_ipmc_repl_detach(unit); 
                 bcm_sb2_ipmc_repl_detach(unit);
                 return BCM_E_MEMORY;
             } 
         }
         sal_memset(_sb2_extq_repl_info[unit]->bitmap_entries_used, 0, alloc_size);
    
         alloc_size = sizeof(_sb2_repl_queue_info_t);
         /* Always reserve slot 0 */
         IPMC_EXTQ_REPL_VE_USED_SET(unit, 0);
         if (!_sb2_extq_repl_info[unit]->mcgroup_info) {
              _sb2_extq_repl_info[unit]->mcgroup_info =
             sal_alloc(alloc_size, "IPMC MC group info");
             if (_sb2_extq_repl_info[unit]->mcgroup_info == NULL) {
                  bcm_tr3_ipmc_repl_detach(unit) ;
                  bcm_sb2_ipmc_repl_detach(unit);
                  return BCM_E_MEMORY;
             }
         }
         sal_memset(_sb2_extq_repl_info[unit]->mcgroup_info, 0, alloc_size);
     
         alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
         if (!IPMC_EXTQ_GROUP_INFO(unit)->queue_count) {   
              IPMC_EXTQ_GROUP_INFO(unit)->queue_count =
                  sal_alloc(alloc_size, "IPMC Extq queue count");
              if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count == NULL) {
                  bcm_tr3_ipmc_repl_detach(unit);
                  bcm_sb2_ipmc_repl_detach(unit);
                  return BCM_E_MEMORY;
              }
         } 
         sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count,
                                  0, alloc_size);
         if (!IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int) {
              IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int=
                  sal_alloc(alloc_size, "IPMC Extq queue count");
              if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int== NULL) {
                  bcm_tr3_ipmc_repl_detach(unit);
                  bcm_sb2_ipmc_repl_detach(unit);
                  return BCM_E_MEMORY;
              }
         } 
         if (!IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext) {
              IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext=
                  sal_alloc(alloc_size, "IPMC Extq queue count");
              if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext== NULL) {
                  bcm_tr3_ipmc_repl_detach(unit);
                  bcm_sb2_ipmc_repl_detach(unit);
                  return BCM_E_MEMORY;
              }
         }
              
         SOC_PBMP_ITER(pbmp_all, port) { 
            /* Coverity issue ignored:
             * Maximum port number used in PBMP_PORT_ALL
             * should never exceed SOC_MAX_NUM_PP_PORTS */
            /* coverity[overrun-call : FALSE] */
            BCM_IF_ERROR_RETURN(bcm_sb2_ipmc_repl_extq_port_alloc(unit, port)); 
         }
         
           /* reserve ipmc index 0 & 1 for extended queue work around */
           BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 0));
           BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 1));
       
            /* Read L3 ipmc table. */
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                           0, entry));
            /* use 1 as initial redirection pointer */
            soc_mem_field32_set(unit, L3_IPMCm, entry,
                                     MMU_MC_REDIRECTION_PTRf, 1);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL,
                                         0, &entry));
     }
#endif 

    return rv;
}

/* Function:
 *	_bcm_tr3_repl_head_ptr_replace
 * Purpose:
 *      Replace REPL_HEAD pointers in MMU multicast queues.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	old_head_index    - Starting index of old REPL_HEAD block.
 *	old_member_bitmap - Old replication group member bitmap.
 *	new_head_index    - Starting index of new REPL_HEAD block.
 *	new_member_bitmap - New replication group member bitmap.
 *	new_last_bitmap   - New replication group last indication bitmap.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_head_ptr_replace(int unit, int old_head_index,
        soc_pbmp_t old_member_bitmap, int new_head_index,
        soc_pbmp_t new_member_bitmap, soc_pbmp_t new_last_bitmap)
{
    int member_bitmap_width;
    int old_member_id, new_member_id;
    int i;
    int old_repl_head_ptr, new_repl_head_ptr;
    int new_l3_last;
    int mmu_port;
    uint64 regval_64;
    int timeout_usec;
    soc_timeout_t to;
    int replace_done;
    soc_info_t *si;
    bcm_port_t phy_port, port;


    si = &SOC_INFO(unit);

    member_bitmap_width = soc_mem_field_length(unit, MMU_REPL_GROUPm,
            MEMBER_BITMAPf);

    old_member_id = 0;
    new_member_id = 0;
    for (i = 0; i < member_bitmap_width; i++) {
        if (SOC_PBMP_MEMBER(old_member_bitmap, i)) {

            /* Compute index to old block of REPL_HEAD entries */
            old_repl_head_ptr = old_head_index + old_member_id;

            if (SOC_PBMP_MEMBER(new_member_bitmap, i)) {
                /* Compute index to new block of REPL_HEAD entries */
                new_repl_head_ptr = new_head_index + new_member_id;
                new_l3_last = SOC_PBMP_MEMBER(new_last_bitmap, i) ? 1 : 0;
            } else {
                new_repl_head_ptr = 0;
                new_l3_last = 1;
            }

            /* Convert member bitmap index to MMU port */
            if (i == 59) {
                mmu_port = 60;
            } else {
                mmu_port = i;
            }

            /* Convert MMU port to logical port */
            phy_port = si->port_m2p_mapping[mmu_port];
            port = si->port_p2l_mapping[phy_port];

            /* Replace old_repl_head_ptr with new_repl_head_ptr
             * in multicast queues.
             */
            COMPILER_64_ZERO(regval_64);
            soc_reg64_field32_set(unit, REPL_HEAD_PTR_REPLACEr, &regval_64,
                    OLD_REPL_HEAD_PTRf, old_repl_head_ptr);
            soc_reg64_field32_set(unit, REPL_HEAD_PTR_REPLACEr, &regval_64,
                    NEW_REPL_HEAD_PTRf, new_repl_head_ptr);
            soc_reg64_field32_set(unit, REPL_HEAD_PTR_REPLACEr, &regval_64,
                    NEW_L3_LASTf, new_l3_last);
            soc_reg64_field32_set(unit, REPL_HEAD_PTR_REPLACEr, &regval_64,
                    REPLACE_ENf, 1);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, REPL_HEAD_PTR_REPLACEr,
                        port, 0, regval_64));

            /* Wait for replacement done */
            timeout_usec = 1000000;
            soc_timeout_init(&to, timeout_usec, 0);
            while (TRUE) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, REPL_HEAD_PTR_REPLACEr,
                            port, 0, &regval_64));
                replace_done = soc_reg64_field32_get(unit,
                        REPL_HEAD_PTR_REPLACEr, regval_64, REPLACE_DONEf);
                if (replace_done) {
                    break;
                }

                if (soc_timeout_check(&to)) {
                    return BCM_E_TIMEOUT;
                }
            }

            old_member_id++;
        }

        if (SOC_PBMP_MEMBER(new_member_bitmap, i)) {
            new_member_id++;
        }
    }

    return BCM_E_NONE;
}

#ifdef BCM_TRIDENT2_SUPPORT

/* Function:
 *	_bcm_td2_repl_icc_set
 * Purpose:
 *      Set replication initial copy count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_td2_repl_icc_set (int unit, int repl_group, bcm_port_t port,
                int intf_count, int count_width, int copy_count)
{
    soc_info_t *si;
    uint32 copy_count_buf[2];
    uint32 count_mask, count_shift;
    mmu_repl_group_initial_copy_count_entry_t initial_copy_count_entry;
    bcm_port_t phy_port;

    si = &SOC_INFO(unit);
    if (SOC_IS_KATANA2(unit)) {
        phy_port = port;
    } else {
        phy_port = si->port_l2p_mapping[port];
    }

    SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUP_INITIAL_COPY_COUNTm(unit,
                MEM_BLOCK_ANY, repl_group, &initial_copy_count_entry));
    if (IS_LB_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_131_130f, copy_count);
    } else if (IS_CPU_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_129_128f, copy_count);
    } else if ((phy_port <= 128) && (phy_port >= 65)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                &initial_copy_count_entry, ICC_BIT_127_64f, copy_count_buf);
        count_mask = (1 << count_width) - 1;
        count_shift = (phy_port - 65) % 32;
        copy_count_buf[(phy_port - 65) / 32] &= ~(count_mask << count_shift);
        copy_count_buf[(phy_port - 65) / 32] |= (copy_count << count_shift);
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                &initial_copy_count_entry, ICC_BIT_127_64f, copy_count_buf);
    } else if (phy_port <= 64) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                &initial_copy_count_entry, ICC_BIT_63_0f, copy_count_buf);
        count_mask = (1 << count_width) - 1;
        count_shift = (phy_port - 1) % 32;
        copy_count_buf[(phy_port - 1)/32] &= ~(count_mask << count_shift);
        copy_count_buf[(phy_port - 1)/32] |= (copy_count << count_shift);
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                &initial_copy_count_entry, ICC_BIT_63_0f, copy_count_buf);
    } else {
        return BCM_E_PORT;
    }
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUP_INITIAL_COPY_COUNTm(unit,
                MEM_BLOCK_ALL, repl_group, &initial_copy_count_entry));

    return BCM_E_NONE;

}

/* Function:
 *	_bcm_apache_repl_icc_set
 * Purpose:
 *      Set replication initial copy count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_apache_repl_icc_set (int unit, int repl_group, bcm_port_t port,
                int intf_count, int count_width, int copy_count)
{
    soc_info_t *si;
    uint32 copy_count_buf;
    uint32 count_mask, count_shift;
    mmu_repl_group_initial_copy_count_entry_t initial_copy_count_entry;
    bcm_port_t phy_port;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];

    SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUP_INITIAL_COPY_COUNTm(unit,
                MEM_BLOCK_ANY, repl_group, &initial_copy_count_entry));
    if (IS_LB_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_92_90f, copy_count);
    } else if (IS_CPU_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_89_88f, copy_count);
    } else if ((phy_port <= 72) && (phy_port >= 65)) {
        copy_count_buf =
            soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_get(unit,
                    &initial_copy_count_entry, ICC_BIT_87_72f);

        count_mask = (1 << count_width) - 1;
        count_shift = (phy_port - 65) * 2;
        copy_count_buf &= ~(count_mask << count_shift);
        copy_count_buf |= (copy_count << count_shift);

        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_87_72f, copy_count_buf);
    } else if ((phy_port <= 64) && (phy_port >= 37)) {
        copy_count_buf =
            soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_get(unit,
                    &initial_copy_count_entry, ICC_BIT_71_44f);

        count_mask = (1 << count_width) - 1;
        count_shift = (phy_port - 37);
        copy_count_buf &= ~(count_mask << count_shift);
        copy_count_buf |= (copy_count << count_shift);

        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_71_44f, copy_count_buf);
    } else if ((phy_port <= 36) && (phy_port >= 29)) {
        copy_count_buf =
            soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_get(unit,
                    &initial_copy_count_entry, ICC_BIT_43_28f);

        count_mask = (1 << count_width) - 1;
        count_shift = ((phy_port - 29) * 2);
        copy_count_buf &= ~(count_mask << count_shift);
        copy_count_buf |= (copy_count << count_shift);

        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_43_28f, copy_count_buf);
    } else if (phy_port <= 28) {
        copy_count_buf =
            soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_get(unit,
                    &initial_copy_count_entry, ICC_BIT_27_0f);

        count_mask = (1 << count_width) - 1;
        count_shift = (phy_port - 1);
        copy_count_buf &= ~(count_mask << count_shift);
        copy_count_buf |= (copy_count << count_shift);

        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_27_0f, copy_count_buf);
    } else {
        return BCM_E_PORT;
    }
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUP_INITIAL_COPY_COUNTm(unit,
                MEM_BLOCK_ALL, repl_group, &initial_copy_count_entry));


    return BCM_E_NONE;
}

/* Function:
 *     _bcm_monterey_repl_icc_set
 * Purpose:
 *      Set replication initial copy count.
 * Parameters:
 *     unit       - StrataSwitch PCI device unit number.
 *     repl_group - The replication group number.
 *     port       - Port.
 *     intf_count - Replication list's interface count.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_monterey_repl_icc_set (int unit, int repl_group, bcm_port_t port,
                int intf_count, int count_width, int copy_count)
{
    soc_info_t *si;
    uint32 copy_count_buf[4];
    uint32 count_mask, count_shift;
    mmu_repl_group_initial_copy_count_entry_t initial_copy_count_entry;
    bcm_port_t phy_port;
    uint32 bit_offset;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];

    SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUP_INITIAL_COPY_COUNTm(unit,
                MEM_BLOCK_ANY, repl_group, &initial_copy_count_entry));
    if (IS_MACSEC_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_134_132f, copy_count);
    } else if (IS_LB_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_131_130f, copy_count);
    } else if (IS_CPU_PORT(unit, port)) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field32_set(unit,
                &initial_copy_count_entry, ICC_BIT_129_128f, copy_count);
    } else if (phy_port <= 64) {
        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_get(unit,
                &initial_copy_count_entry, ICC_BIT_127_0f, copy_count_buf);

        count_mask = (1 << count_width) - 1;
        bit_offset = 2 * (phy_port - 1);
        count_shift = bit_offset % 32;
        copy_count_buf[(bit_offset)/32] &= ~(count_mask << count_shift);
        copy_count_buf[(bit_offset)/32] |= (copy_count << count_shift);

        soc_MMU_REPL_GROUP_INITIAL_COPY_COUNTm_field_set(unit,
                &initial_copy_count_entry, ICC_BIT_127_0f, copy_count_buf);
    } else {
        return BCM_E_PORT;
    }
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUP_INITIAL_COPY_COUNTm(unit,
                MEM_BLOCK_ALL, repl_group, &initial_copy_count_entry));


    return BCM_E_NONE;
}
/* Function:
 *	_bcm_td2_repl_initial_copy_count_set
 * Purpose:
 *      Set replication initial copy count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_td2_repl_initial_copy_count_set(int unit, int repl_group, bcm_port_t port,
                int intf_count)
{
    uint32 regval;
    int count_width, copy_count;
    int rv;
 
    SOC_IF_ERROR_RETURN(READ_PORT_INITIAL_COPY_COUNT_WIDTHr(unit,
                port, &regval));
    count_width = 1 + soc_reg_field_get(unit,
            PORT_INITIAL_COPY_COUNT_WIDTHr, regval, BIT_WIDTHf);
    copy_count = 0;
    switch (count_width) {
        case 1:
            if (intf_count <= 1) {
                copy_count = intf_count;
            } 
            break;
        case 2:
            if (intf_count <= 3) {
                copy_count = intf_count;
            }
            break;
        case 3:
            if (intf_count <= 5) {
                copy_count = intf_count;
            }
            break;
        default:
            return BCM_E_INTERNAL;
            break;
    }

    if (SOC_IS_MONTEREY(unit)) {
        rv = _bcm_monterey_repl_icc_set(unit, repl_group, port, intf_count,
                count_width, copy_count);
    } else if (SOC_IS_APACHE(unit)) {
        rv = _bcm_apache_repl_icc_set(unit, repl_group, port, intf_count,
                count_width, copy_count);
    } else {
        rv = _bcm_td2_repl_icc_set(unit, repl_group, port, intf_count,
                count_width, copy_count);
    }

    return rv;
}

/* Function:
 *	_bcm_td2_repl_group_info_set
 * Purpose:
 *      Set replication group info.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	member_bitmap - Member bitmap.
 *	head_index    - Base pointer to REPL_HEAD table.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_td2_repl_group_info_set(int unit, int repl_group,
        soc_pbmp_t member_bitmap, int head_index)
{
    int rv = BCM_E_NONE;
    soc_pbmp_t xpipe_member_bitmap, ypipe_member_bitmap;
    int xpipe_member_bitmap_width, ypipe_member_bitmap_width = 0;
    int xpipe_member_count, ypipe_member_count;
    int i;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    mmu_repl_group_info0_entry_t repl_group_info0_entry;
    mmu_repl_group_info1_entry_t repl_group_info1_entry;
    soc_info_t *si;
    soc_pbmp_t l3_pbmp;
    bcm_port_t mmu_port, phy_port, port;
    int cut_through_eligible;
    ipmc_entry_t l3_ipmc_entry;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    bcm_multicast_t  group = 0;
#endif

    /* Configure X-pipe REPL_GROUP table */

    SOC_PBMP_CLEAR(xpipe_member_bitmap);
    xpipe_member_bitmap_width = soc_mem_field_length(unit,
                MMU_REPL_GROUP_INFO0m, PIPE_MEMBER_BMPf);
    xpipe_member_count = 0;
    for (i = 0; i < xpipe_member_bitmap_width; i++) {
        if (SOC_PBMP_MEMBER(member_bitmap, i)) {
            SOC_PBMP_PORT_ADD(xpipe_member_bitmap, i);
            xpipe_member_count++;
        }
    }
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        fldbuf[i] = SOC_PBMP_WORD_GET(xpipe_member_bitmap, i);
    }
    sal_memset(&repl_group_info0_entry, 0, sizeof(repl_group_info0_entry));
    soc_MMU_REPL_GROUP_INFO0m_field_set(unit, &repl_group_info0_entry,
            PIPE_MEMBER_BMPf, fldbuf);
    soc_MMU_REPL_GROUP_INFO0m_field32_set(unit, &repl_group_info0_entry,
            PIPE_BASE_PTRf, head_index);
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUP_INFO0m(unit, MEM_BLOCK_ALL,
                repl_group, &repl_group_info0_entry));

    SOC_PBMP_CLEAR(ypipe_member_bitmap);
    if (soc_feature(unit, soc_feature_split_repl_group_table)) {
        /* Configure Y-pipe REPL_GROUP table */

        SOC_PBMP_CLEAR(ypipe_member_bitmap);
        ypipe_member_bitmap_width = soc_mem_field_length(unit,
                MMU_REPL_GROUP_INFO1m, PIPE_MEMBER_BMPf);
        ypipe_member_count = 0;
        for (i = xpipe_member_bitmap_width;
             i < (xpipe_member_bitmap_width + ypipe_member_bitmap_width);
             i++) {
             if (SOC_PBMP_MEMBER(member_bitmap, i)) {
                 SOC_PBMP_PORT_ADD(ypipe_member_bitmap,
                         (i - xpipe_member_bitmap_width));
                 ypipe_member_count++;
             }
        }
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
             fldbuf[i] = SOC_PBMP_WORD_GET(ypipe_member_bitmap, i);
        }
        sal_memset(&repl_group_info1_entry, 0, sizeof(repl_group_info1_entry));
        soc_MMU_REPL_GROUP_INFO1m_field_set(unit, &repl_group_info1_entry,
                PIPE_MEMBER_BMPf, fldbuf);
        if (ypipe_member_count > 0) {
            soc_MMU_REPL_GROUP_INFO1m_field32_set(unit, &repl_group_info1_entry,
                    PIPE_BASE_PTRf, (head_index + xpipe_member_count));
        } else {
            soc_MMU_REPL_GROUP_INFO1m_field32_set(unit, &repl_group_info1_entry,
                    PIPE_BASE_PTRf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUP_INFO1m(unit, MEM_BLOCK_ALL,
                    repl_group, &repl_group_info1_entry));
    }

    /* When MMU is in cut-through mode, REPL_GROUP table is not read.
     * Instead, MMU gets the REPL_HEAD base pointer and L3 bitmap from
     * the L3_IPMC table in ingress pipeline.
     */
    if (soc_mem_field_valid(unit, L3_IPMCm, REPL_HEAD_BASE_PTRf)) {
        /* Convert MMU member bitmap to logical port bitmap.
         * X-pipe contains mmu ports 0-52. Y-pipe contains mmu ports 64-116.
         */  
        si = &SOC_INFO(unit);
        SOC_PBMP_CLEAR(l3_pbmp);
        for (i = 0; i < xpipe_member_bitmap_width; i++) {
            if (SOC_PBMP_MEMBER(xpipe_member_bitmap, i)) {
                mmu_port = i;
                phy_port = si->port_m2p_mapping[mmu_port];
                port = si->port_p2l_mapping[phy_port];
                if (port != -1) {
                    SOC_PBMP_PORT_ADD(l3_pbmp, port);
                }
            }
        }
        if (soc_feature(unit, soc_feature_split_repl_group_table)) {
            for (i = 0; i < ypipe_member_bitmap_width; i++) {
                 if (SOC_PBMP_MEMBER(ypipe_member_bitmap, i)) {
                     mmu_port = i + 64;
                     phy_port = si->port_m2p_mapping[mmu_port];
                     port = si->port_p2l_mapping[phy_port];
                     SOC_PBMP_PORT_ADD(l3_pbmp, port);
                 }
            }
        }

        /* Determine cut-through eligibility. An IPMC group is eligible
         * for cut-through only if there is a single replication per
         * L3 port.
         */
        cut_through_eligible = TRUE;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        /* For VXLAN IPMC, cut-through is always disabled.
        *  The IP header for the tunnel needs the entire length of
        *  the IP payload. This implies that the MMU must store the
        *  entire packet, determining its length and then forward it
        *  to the EP.
        */
        (void)_bcm_tr_multicast_ipmc_group_type_get(unit, repl_group, &group);
        if ((SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) &&
            (_BCM_MULTICAST_TYPE_VXLAN == _BCM_MULTICAST_TYPE_GET( group))){
            cut_through_eligible = FALSE;
        } else
#endif
        {
            SOC_PBMP_ITER(l3_pbmp, port) {
                /* Coverity issue ignored:
                 * Maximum port number used in l3_pbmp
                 * should never exceed SOC_MAX_NUM_PP_PORTS */
                /* coverity[overrun-local : FALSE] */
                if (REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) > 1) {
                        cut_through_eligible = FALSE;
                    break;
                }
            }
        }

        /* Write to L3_IPMC table */
        soc_mem_lock(unit, L3_IPMCm);
        rv = READ_L3_IPMCm(unit, MEM_BLOCK_ANY, repl_group, &l3_ipmc_entry);
        if (BCM_SUCCESS(rv)) {
            if (!soc_L3_IPMCm_field32_get(unit, &l3_ipmc_entry, VALIDf)) {
                sal_memset(&l3_ipmc_entry, 0, sizeof(ipmc_entry_t));
                soc_L3_IPMCm_field32_set(unit, &l3_ipmc_entry, VALIDf, 1);
            }
            soc_L3_IPMCm_field32_set(unit, &l3_ipmc_entry, REPL_HEAD_BASE_PTRf,
                    head_index);
            soc_L3_IPMCm_field32_set(unit, &l3_ipmc_entry, DO_NOT_CUT_THROUGHf,
                    !cut_through_eligible);
            rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, repl_group, &l3_ipmc_entry);
        }
        soc_mem_unlock(unit, L3_IPMCm);
    }

    return rv;
}

#endif /* BCM_TRIDENT2_SUPPORT */

/* Function:
 *	_bcm_tr3_repl_list_start_ptr_set
 * Purpose:
 *      Set replication list start pointer for given (repl_group, port).
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	start_ptr  - Replication list's start pointer.
 *	intf_count - Replication list's interface count.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_list_start_ptr_set(int unit, int repl_group, bcm_port_t port,
                int start_ptr, int intf_count)
{
    int add_member;
    soc_info_t *si;
    bcm_port_t phy_port, mmu_port;
    int member_bitmap_index = 0;
    mmu_repl_group_entry_t repl_group_entry;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    soc_pbmp_t old_member_bitmap, old_last_bitmap;
    soc_pbmp_t new_member_bitmap, new_last_bitmap;
    int i;
    int old_member_count = 0, new_member_count = 0;
    int member_id, member, old_member_id, new_member_id;
    int old_head_index = 0 , new_head_index = 0;
    mmu_repl_head_tbl_entry_t repl_head_entry, old_repl_head_entry;
#ifdef BCM_KATANA2_SUPPORT
    mmu_repl_grp_tbl0_entry_t repl_grp_tbl0_entry;
    mmu_repl_grp_tbl1_entry_t repl_grp_tbl1_entry;
    mmu_repl_grp_tbl2_entry_t repl_grp_tbl2_entry;
    mmu_repl_grp_tbl_entry_t repl_grp_tbl_entry;
    _kt2_repl_grp_info_t kt2_repl_grp_info;    
    int last_member_bitmap_index = 0;
    soc_pbmp_t member_bitmap0, member_bitmap1;
    int member_bitmap_width0 = 0;
    int remap_grp_id = 0;
#endif
    int rv = BCM_E_NONE;
    int reserved_resources_used = 0, loop, num_of_ports = 0;

#ifdef BCM_TRIDENT2_SUPPORT
    int new_head_index_2 = 0, rv2 = BCM_E_NONE;
#endif
    if (start_ptr > 0) {
        add_member = TRUE;
    } else {
        add_member = FALSE;
    }
   
    si = &SOC_INFO(unit);
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {

        sal_memset(&repl_grp_tbl0_entry, 0, sizeof(mmu_repl_grp_tbl0_entry_t));
        sal_memset(&repl_grp_tbl1_entry, 0, sizeof(mmu_repl_grp_tbl1_entry_t));
        sal_memset(&repl_grp_tbl2_entry, 0, sizeof(mmu_repl_grp_tbl2_entry_t));
        sal_memset(&kt2_repl_grp_info, 0, sizeof(kt2_repl_grp_info));

        phy_port = port;
        mmu_port = port;
        /* Get the REMAP ID for IPMC index if any */
        BCM_IF_ERROR_RETURN
        (_bcm_kt_ipmc_mmu_mc_remap_ptr(unit, repl_group, &remap_grp_id, FALSE));


        BCM_IF_ERROR_RETURN(_bcm_kt2_repl_list_tbl_ptr_get (unit, mmu_port,
               &kt2_repl_grp_info));      

        member_bitmap_index = mmu_port;
        last_member_bitmap_index = kt2_repl_grp_info.lb_index;

        if (soc_mem_field_valid(unit, kt2_repl_grp_info.mb_mem, MEMBER_BMP_PORTSf)) {
            member_bitmap_width0 =  soc_mem_field_length(unit,
                    kt2_repl_grp_info.mb_mem, MEMBER_BMP_PORTSf);
        } else {
        member_bitmap_width0 =  soc_mem_field_length(unit,
                MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTS_119_0f);
        }

    } else 
#endif /*BCM_KATANA2_SUPPORT */
    {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
    }
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_split_repl_group_table)) {
        int xpipe_member_bitmap_width;
        mmu_repl_group_info0_entry_t repl_group_info0_entry;
        mmu_repl_group_info1_entry_t repl_group_info1_entry;
        soc_pbmp_t xpipe_member_bitmap, ypipe_member_bitmap;

        /* In Trident2, REPL_GROUP table is split into 2 halves,
         * one for each pipeline. The X-pipe REPL_GROUP table's
         * member bitmap contains MMU ports 0-52. The Y-pipe REPL_GROUP
         * table's member bitmap contains MMU ports 64-116.
         * These two member bitmaps will be concatenated to form a
         * single 106-bit member bitmap.
         */
        xpipe_member_bitmap_width = soc_mem_field_length(unit,
                MMU_REPL_GROUP_INFO0m, PIPE_MEMBER_BMPf);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            int aggid = TD2P_AGG_ID_INVALID;
            BCM_IF_ERROR_RETURN(bcm_td2p_port_to_aggid(unit, port, &aggid));
                if (mmu_port >= 64) {
                    member_bitmap_index = aggid + xpipe_member_bitmap_width;
                } else {
                    member_bitmap_index = aggid;
                }
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            if (mmu_port >= 64) {
                member_bitmap_index = mmu_port - 64 + xpipe_member_bitmap_width;
            } else {
                member_bitmap_index = mmu_port;
            }
        }

        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUP_INFO0m(unit, MEM_BLOCK_ANY,
                    repl_group, &repl_group_info0_entry));
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_MMU_REPL_GROUP_INFO0m_field_get(unit, &repl_group_info0_entry,
                PIPE_MEMBER_BMPf, fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(xpipe_member_bitmap, i, fldbuf[i]);
        }
        old_head_index = soc_MMU_REPL_GROUP_INFO0m_field32_get(unit,
                &repl_group_info0_entry, PIPE_BASE_PTRf);

        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUP_INFO1m(unit, MEM_BLOCK_ANY,
                    repl_group, &repl_group_info1_entry));
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_MMU_REPL_GROUP_INFO1m_field_get(unit, &repl_group_info1_entry,
                PIPE_MEMBER_BMPf, fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(ypipe_member_bitmap, i, fldbuf[i]);
        }
        SOC_PBMP_CLEAR(old_member_bitmap);
        SOC_PBMP_OR(old_member_bitmap, xpipe_member_bitmap);
        SOC_PBMP_ITER(ypipe_member_bitmap, member) {
            SOC_PBMP_PORT_ADD(old_member_bitmap,
                    (member + xpipe_member_bitmap_width));
        }

        SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
        SOC_PBMP_COUNT(old_member_bitmap, old_member_count);

    } else if (SOC_IS_APACHE(unit)) {
        mmu_repl_group_info0_entry_t repl_group_info0_entry;
        soc_pbmp_t                   xpipe_member_bitmap;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            int aggid = TD2P_AGG_ID_INVALID;
            BCM_IF_ERROR_RETURN(bcm_td2p_port_to_aggid(unit, port, &aggid));
            member_bitmap_index = aggid;
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            member_bitmap_index = mmu_port;
        }
        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUP_INFO0m(unit, MEM_BLOCK_ANY,
                                                       repl_group, 
                                                       &repl_group_info0_entry));
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_MMU_REPL_GROUP_INFO0m_field_get(unit, &repl_group_info0_entry,
                                            PIPE_MEMBER_BMPf, fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(xpipe_member_bitmap, i, fldbuf[i]);
        }
        old_head_index = soc_MMU_REPL_GROUP_INFO0m_field32_get(unit,
                          &repl_group_info0_entry, PIPE_BASE_PTRf);

        SOC_PBMP_CLEAR(old_member_bitmap);
        SOC_PBMP_OR(old_member_bitmap, xpipe_member_bitmap);
        
        SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
        SOC_PBMP_COUNT(old_member_bitmap, old_member_count);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {

        /* Read all the group Tables 
         * read all the member_bitmap and concatenate to get 
         *  the member_bitmap for all the ports
         */

        if (SOC_MEM_IS_VALID(unit, MMU_REPL_GRP_TBL0m) && (SOC_MEM_IS_VALID(unit, MMU_REPL_GRP_TBL1m))
                && (SOC_MEM_IS_VALID(unit, MMU_REPL_GRP_TBL2m))) {
        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBL0m(unit, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_grp_tbl0_entry));
       
        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBL1m(unit, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_grp_tbl1_entry));
 
        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBL2m(unit, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_grp_tbl2_entry));

        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));            
        soc_MMU_REPL_GRP_TBL0m_field_get(unit, &repl_grp_tbl0_entry,
                MEMBER_BMP_PORTS_119_0f, fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(member_bitmap0, i, fldbuf[i]);
        }

        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_MMU_REPL_GRP_TBL1m_field_get(unit, &repl_grp_tbl1_entry,
                MEMBER_BMP_PORTS_169_120f, fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(member_bitmap1, i, fldbuf[i]);
        }
   
        SOC_PBMP_CLEAR(old_member_bitmap);
        SOC_PBMP_OR(old_member_bitmap, member_bitmap0);
        SOC_PBMP_ITER(member_bitmap1, member) {
            SOC_PBMP_PORT_ADD(old_member_bitmap,
                    (member + member_bitmap_width0));
        }

        SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
        SOC_PBMP_COUNT(old_member_bitmap, old_member_count); 

        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        if (mmu_port >= 70)
        {
            soc_mem_field_get(unit, kt2_repl_grp_info.lb_mem, 
               (uint32 *)&repl_grp_tbl2_entry, kt2_repl_grp_info.lbmp, fldbuf);
        } else {
            soc_mem_field_get(unit, kt2_repl_grp_info.lb_mem,
               (uint32 *)&repl_grp_tbl1_entry, kt2_repl_grp_info.lbmp, fldbuf);
        }
    
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(old_last_bitmap, i, fldbuf[i]);
        }

        SOC_PBMP_ASSIGN(new_last_bitmap, old_last_bitmap);

        old_head_index = soc_MMU_REPL_GRP_TBL2m_field32_get(unit,
                    &repl_grp_tbl2_entry, BASE_PTRf);
        } else if (soc_mem_field_valid(unit, MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTSf)) {
            /* saber2 */
            SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBL0m(unit, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_grp_tbl0_entry));

            SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBL1m(unit, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_grp_tbl1_entry));

            sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
            soc_MMU_REPL_GRP_TBL0m_field_get(unit, &repl_grp_tbl0_entry,
                    MEMBER_BMP_PORTSf, fldbuf);
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                SOC_PBMP_WORD_SET(member_bitmap0, i, fldbuf[i]);
            }

            SOC_PBMP_CLEAR(old_member_bitmap);
            SOC_PBMP_OR(old_member_bitmap, member_bitmap0);
            SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
            SOC_PBMP_COUNT(old_member_bitmap, old_member_count); 

            sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
            if (mmu_port >= 24) {
                soc_mem_field_get(unit, kt2_repl_grp_info.lb_mem, 
                        (uint32 *)&repl_grp_tbl1_entry, kt2_repl_grp_info.lbmp, fldbuf);
            } else {
                soc_mem_field_get(unit, kt2_repl_grp_info.lb_mem,
                        (uint32 *)&repl_grp_tbl0_entry, kt2_repl_grp_info.lbmp, fldbuf);
            }
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                SOC_PBMP_WORD_SET(old_last_bitmap, i, fldbuf[i]);
            }
            SOC_PBMP_ASSIGN(new_last_bitmap, old_last_bitmap);
            old_head_index = soc_MMU_REPL_GRP_TBL1m_field32_get(unit,
                    &repl_grp_tbl1_entry, BASE_PTRf);

        } else if (SOC_IS_METROLITE(unit)) {

            SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBLm(unit, MEM_BLOCK_ANY,
                                          remap_grp_id, &repl_grp_tbl_entry));
            sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
            soc_MMU_REPL_GRP_TBLm_field_get(unit, &repl_grp_tbl_entry,
                    MEMBER_BMP_PORTSf, fldbuf);
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                SOC_PBMP_WORD_SET(member_bitmap0, i, fldbuf[i]);
            }

            SOC_PBMP_CLEAR(old_member_bitmap);
            SOC_PBMP_OR(old_member_bitmap, member_bitmap0);
            SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
            SOC_PBMP_COUNT(old_member_bitmap, old_member_count);

            sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
            soc_mem_field_get(unit, kt2_repl_grp_info.lb_mem,
                   (uint32 *)&repl_grp_tbl_entry, kt2_repl_grp_info.lbmp, fldbuf);
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                SOC_PBMP_WORD_SET(old_last_bitmap, i, fldbuf[i]);
            }
            SOC_PBMP_ASSIGN(new_last_bitmap, old_last_bitmap);
            old_head_index = soc_MMU_REPL_GRP_TBLm_field32_get(unit,
                    &repl_grp_tbl_entry, BASE_PTRf);

        } else {

            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "REPL_GRP_TBL invalid memory\n")));
            return BCM_E_INTERNAL;
        }
    } else
#endif /* BCM_KATANA2_SUPPORT*/
    {
         /* Triumph3 MMU does not support replication on
          * ports 57, 59, 61 and 62 */
        if ((mmu_port == 57) || (mmu_port == 59) || 
            (mmu_port == 61) || (mmu_port == 62)) {
            return BCM_E_PORT;
        } else if (mmu_port == 60) {
            member_bitmap_index = 59;
        } else {
            member_bitmap_index = mmu_port;
        } 
        SOC_IF_ERROR_RETURN(READ_MMU_REPL_GROUPm(unit, MEM_BLOCK_ANY, 
                        repl_group,&repl_group_entry));

        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_MMU_REPL_GROUPm_field_get(unit, &repl_group_entry, MEMBER_BITMAPf,
                    fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(old_member_bitmap, i, fldbuf[i]);
        }
        SOC_PBMP_ASSIGN(new_member_bitmap, old_member_bitmap);
        SOC_PBMP_COUNT(old_member_bitmap, old_member_count);

        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_MMU_REPL_GROUPm_field_get(unit, &repl_group_entry, LAST_BITMAPf,
                    fldbuf);
        for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
            SOC_PBMP_WORD_SET(old_last_bitmap, i, fldbuf[i]);
        }
        SOC_PBMP_ASSIGN(new_last_bitmap, old_last_bitmap);
        old_head_index = soc_MMU_REPL_GROUPm_field32_get(unit, 
                    &repl_group_entry, HEAD_INDEXf);
    }

    if (add_member) {

        /* Update REPL_HEAD table */

        sal_memset(&repl_head_entry, 0, sizeof(mmu_repl_head_tbl_entry_t));
        soc_MMU_REPL_HEAD_TBLm_field32_set(unit, &repl_head_entry,
                HEAD_PTRf, start_ptr);
        if (soc_mem_field_valid(unit, MMU_REPL_HEAD_TBLm, ADDL_REPSf)) {
            if (intf_count > 4) {
                soc_MMU_REPL_HEAD_TBLm_field32_set(unit, &repl_head_entry,
                        ADDL_REPSf, 3);
            } else if (intf_count > 1) {
                soc_MMU_REPL_HEAD_TBLm_field32_set(unit, &repl_head_entry,
                        ADDL_REPSf, intf_count - 2);
            } else {
                soc_MMU_REPL_HEAD_TBLm_field32_set(unit, &repl_head_entry,
                        ADDL_REPSf, 0);
            }
        }

        if (SOC_PBMP_MEMBER(old_member_bitmap, member_bitmap_index)) {
            /* Port is already a member of the group */
            member_id = 0; 
            SOC_PBMP_ITER(old_member_bitmap, member) {
                if (member == member_bitmap_index) {
                    break;
                }
                member_id++;
            }
            SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_HEAD_TBLm(unit, MEM_BLOCK_ALL,
                        old_head_index + member_id, &repl_head_entry));
            new_head_index = old_head_index;
        } else {
            if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {
                /* If REPL_HEAD entries are statically allocated to
                 * replication groups, the port should already be set
                 * in member_bitmap.
                 */
                return BCM_E_PORT;
            }
            SOC_PBMP_PORT_ADD(new_member_bitmap, member_bitmap_index);
            new_member_count = old_member_count + 1;
            BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_block_alloc(unit,
                        new_member_count, &new_head_index));
            old_member_id = 0;
            new_member_id = 0;
            SOC_PBMP_ITER(new_member_bitmap, member) {
                if (member == member_bitmap_index) {
                   SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ALL, new_head_index + new_member_id,
                                &repl_head_entry));
                } else {
                    SOC_IF_ERROR_RETURN(READ_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ANY, old_head_index + old_member_id,
                                &old_repl_head_entry));
                    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ALL, new_head_index + new_member_id,
                                &old_repl_head_entry));
                    old_member_id++;
                }
                new_member_id++;
            }           
        }
 
        /* Update REPL_GROUP entry */

#ifdef BCM_TRIDENT2_SUPPORT
        if ((soc_feature(unit, soc_feature_split_repl_group_table)) ||
            SOC_IS_APACHE(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_td2_repl_initial_copy_count_set(unit,
                        repl_group, port, intf_count));
            BCM_IF_ERROR_RETURN(_bcm_td2_repl_group_info_set(unit,
                        repl_group, new_member_bitmap, new_head_index));
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {

            if (intf_count == 1) {
                SOC_PBMP_PORT_ADD(new_last_bitmap, last_member_bitmap_index);
            } else {
                SOC_PBMP_PORT_REMOVE(new_last_bitmap, last_member_bitmap_index);
            }
            
            BCM_IF_ERROR_RETURN(_bcm_kt2_repl_group_info_set(unit, 
                         remap_grp_id, port, new_member_bitmap,
                         new_last_bitmap, new_head_index)); 

       } else 
#endif
        {
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                fldbuf[i] = SOC_PBMP_WORD_GET(new_member_bitmap, i);
            }
            soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                        MEMBER_BITMAPf, fldbuf);
           
            if (intf_count == 1) {
                SOC_PBMP_PORT_ADD(new_last_bitmap, member_bitmap_index);
            } else {
                SOC_PBMP_PORT_REMOVE(new_last_bitmap, member_bitmap_index);
            }

            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                fldbuf[i] = SOC_PBMP_WORD_GET(new_last_bitmap, i);
            }
            soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                        LAST_BITMAPf, fldbuf);

            soc_MMU_REPL_GROUPm_field32_set(unit, &repl_group_entry,
                        HEAD_INDEXf, new_head_index);

            SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUPm(unit, MEM_BLOCK_ALL,
                            repl_group, &repl_group_entry));
        }

        /* Release old block of REPL_HEAD entries */
        if (old_member_count > 0 && old_head_index != new_head_index) {
            if (soc_feature(unit, soc_feature_repl_head_ptr_replace)) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_ptr_replace(unit,
                            old_head_index, old_member_bitmap,
                            new_head_index, new_member_bitmap,
                            new_last_bitmap));
            }
            BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_block_free(unit,
                        old_head_index, old_member_count));
        }

#ifdef BCM_TRIDENT2_SUPPORT
        /*
         * If we are here then we have already allocated a new index for
         * head_block. But as the algorithm tries to add members in scaled
         * system, fragmentation occurs in table which results in
         * under utilization of replication tables.
         * To solve that problem, a quick solution is done here
         * to try to re-allocate an index from list after freeing
         * old indexes and if smaller index is found, then allocate
         * that index.
         */
        if (soc_feature(unit, soc_feature_mmu_repl_alloc_unfrag)) {
            if (!SOC_PBMP_MEMBER(old_member_bitmap, member_bitmap_index)) {
                rv2 = _bcm_tr3_repl_head_block_alloc(unit,
                        new_member_count, &new_head_index_2);

                if (BCM_SUCCESS(rv2)) {

                    if (new_head_index_2 < new_head_index) {
                        old_member_id = 0;
                        new_member_id = 0;

                        SOC_PBMP_ITER(new_member_bitmap, member) {
                            SOC_IF_ERROR_RETURN(READ_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ANY, new_head_index + old_member_id,
                                &old_repl_head_entry));
                            SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ALL, new_head_index_2 + new_member_id,
                                &old_repl_head_entry));
                            old_member_id++;
                            new_member_id++;
                        }


                        BCM_IF_ERROR_RETURN(_bcm_td2_repl_group_info_set(unit,
                            repl_group, new_member_bitmap, new_head_index_2));

                        _bcm_tr3_repl_head_block_free(unit,
                            new_head_index, new_member_count);

                    } else {
                        _bcm_tr3_repl_head_block_free(unit,
                            new_head_index_2, new_member_count);
                    }
                }
            }
        }
#endif
    } else {
        /* Remove member from replication group */

        if (soc_feature(unit, soc_feature_static_repl_head_alloc)) {
            /* Clear REPL_HEAD entry */
            member_id = 0; 
            SOC_PBMP_ITER(old_member_bitmap, member) {
                if (member == member_bitmap_index) {
                    break;
                }
                member_id++;
            }
            SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_HEAD_TBLm(unit,
                        MEM_BLOCK_ALL, old_head_index + member_id,
                        soc_mem_entry_null(unit, MMU_REPL_HEAD_TBLm)));

            /* Set bit in LAST_BITMAP */
            SOC_PBMP_PORT_ADD(new_last_bitmap, member_bitmap_index);
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                fldbuf[i] = SOC_PBMP_WORD_GET(new_last_bitmap, i);
            }

            soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                        LAST_BITMAPf, fldbuf);
            SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUPm(unit, MEM_BLOCK_ALL,
                            repl_group, &repl_group_entry));
            return BCM_E_NONE;
        }

        if (!SOC_PBMP_MEMBER(old_member_bitmap, member_bitmap_index)) {
            /* Port is not a member. Nothing more to do. */
            return BCM_E_NONE;
        }

        /* Update REPL_HEAD table */

        new_member_count = old_member_count - 1;
        for (loop = 0; loop < 2; loop++) {
            if (new_member_count > 0) {
                rv = _bcm_tr3_repl_head_block_alloc(unit,
                                                    new_member_count,
                                                    &new_head_index);
                if (BCM_FAILURE(rv)) {
                    if (!reserve_multicast_resources[unit]) {
                        return rv; 
                    }
                    BCM_PBMP_COUNT(PBMP_ALL(unit), num_of_ports);
                    new_head_index = 
                        soc_mem_index_max(unit, MMU_REPL_HEAD_TBLm)
                                     - num_of_ports + 1;
                    reserved_resources_used = 1;
                }
                old_member_id = 0;
                new_member_id = 0;
                SOC_PBMP_ITER(old_member_bitmap, member) {
                    if (member != member_bitmap_index) {
                        SOC_IF_ERROR_RETURN(READ_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ANY, old_head_index + old_member_id,
                                &old_repl_head_entry));
                        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_HEAD_TBLm(unit,
                                MEM_BLOCK_ALL, new_head_index + new_member_id,
                                &old_repl_head_entry));
                        new_member_id++;
                    }
                    old_member_id++;
                }
            } else {
                new_head_index = 0;
            }

            if (loop == 0) {
                SOC_PBMP_PORT_REMOVE(new_member_bitmap, member_bitmap_index);
            }

        /* Update REPL_GROUP entry */

#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_split_repl_group_table) ||
                (SOC_IS_APACHE(unit))) {
                BCM_IF_ERROR_RETURN(_bcm_td2_repl_initial_copy_count_set(unit,
                            repl_group, port, 0));
                BCM_IF_ERROR_RETURN(_bcm_td2_repl_group_info_set(unit,
                            repl_group, new_member_bitmap, new_head_index));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if (loop == 0) {
                    SOC_PBMP_PORT_REMOVE(new_last_bitmap, 
                                              last_member_bitmap_index);
                }
                
                BCM_IF_ERROR_RETURN(_bcm_kt2_repl_group_info_set(unit,
                             remap_grp_id, port, new_member_bitmap,
                             new_last_bitmap, new_head_index));
            } else 
#endif /*BCM_KATANA2_SUPPORT */
            {
                for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                    fldbuf[i] = SOC_PBMP_WORD_GET(new_member_bitmap, i);
                }
                soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                         MEMBER_BITMAPf, fldbuf);

                SOC_PBMP_PORT_REMOVE(new_last_bitmap, member_bitmap_index);
                for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                    fldbuf[i] = SOC_PBMP_WORD_GET(new_last_bitmap, i);
                }
                soc_MMU_REPL_GROUPm_field_set(unit, &repl_group_entry,
                            LAST_BITMAPf, fldbuf);

                soc_MMU_REPL_GROUPm_field32_set(unit, &repl_group_entry,
                            HEAD_INDEXf, new_head_index);

                SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_GROUPm(unit, MEM_BLOCK_ALL,
                                repl_group, &repl_group_entry));
            }

        /* Release old block of REPL_HEAD entries */
            if (soc_feature(unit, soc_feature_repl_head_ptr_replace)) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_ptr_replace(unit,
                            old_head_index, old_member_bitmap,
                            new_head_index, new_member_bitmap,
                            new_last_bitmap));
            }

            if (loop == 0) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_repl_head_block_free(unit,
                            old_head_index, old_member_count));
            }

            if (!reserved_resources_used) {
                break;
            }

            old_head_index = new_head_index;
            BCM_PBMP_ASSIGN(old_member_bitmap, new_member_bitmap);            
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_tr3_repl_list_start_ptr_get
 * Purpose:
 *      Get replication list start pointer for given (repl_group, port).
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port.
 *	start_ptr  - (OUT) Replication list's start pointer to
 *	             REPL_LIST table.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_list_start_ptr_get(int unit, int repl_group, bcm_port_t port,
        int *start_ptr)
{
    soc_info_t *si;
    bcm_port_t phy_port, mmu_port;
    int member_bitmap_index;
    soc_mem_t repl_group_table;
    soc_field_t member_bitmap_f, head_index_f; 
    uint32 repl_group_entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    soc_pbmp_t member_bitmap;
    int i;
    int member_id = 0;
    int member;
    int head_index;
    mmu_repl_head_tbl_entry_t repl_head_entry;
#ifdef BCM_KATANA2_SUPPORT
    _kt2_repl_grp_info_t kt2_repl_grp_info;
    mmu_repl_grp_tbl0_entry_t repl_grp_tbl0_entry;
    soc_pbmp_t member_bitmap0;
    mmu_repl_grp_tbl2_entry_t repl_group_base_ptr_entry;
    int remap_grp_id = 0;

    sal_memset(&repl_grp_tbl0_entry, 0, sizeof(mmu_repl_grp_tbl0_entry_t));
    sal_memset(&member_bitmap0, 0, sizeof(member_bitmap0));
#endif /* BCM_KATANA2_SUPPORT */

    si = &SOC_INFO(unit);
    sal_memset(&member_bitmap, 0, sizeof(member_bitmap));
    sal_memset(&repl_group_table, 0, sizeof(repl_group_table));

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        phy_port = port;
        mmu_port = port;
        BCM_IF_ERROR_RETURN
        (_bcm_kt_ipmc_mmu_mc_remap_ptr(unit, repl_group, &remap_grp_id, FALSE));
    } else 
#endif /* BCM_KATANA2_SUPPORT */
    {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
    }

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
        int aggid = TD2P_AGG_ID_INVALID;
        BCM_IF_ERROR_RETURN(bcm_td2p_port_to_aggid(unit, port, &aggid));
        member_bitmap_index = aggid;
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    {
        member_bitmap_index = mmu_port;
    }
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        repl_group_table = MMU_REPL_GROUP_INFO0m;
        member_bitmap_f = PIPE_MEMBER_BMPf;
        head_index_f = PIPE_BASE_PTRf;
    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_split_repl_group_table)) {
        /* Trident2 has 108 MMU ports: port 0-53 in X pipe, 64-117 in Y pipe.
         * MMU ports 53 and 117 are purge ports.
         */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
            if (mmu_port >= 64) {
                repl_group_table = MMU_REPL_GROUP_INFO1m;
            } else {
                repl_group_table = MMU_REPL_GROUP_INFO0m;
            }
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        {
            if (mmu_port >= 64) {
                member_bitmap_index = mmu_port - 64;
                repl_group_table = MMU_REPL_GROUP_INFO1m;
            } else {
                repl_group_table = MMU_REPL_GROUP_INFO0m;
            }
        }
        member_bitmap_f = PIPE_MEMBER_BMPf;
        head_index_f = PIPE_BASE_PTRf;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            sal_memset(&kt2_repl_grp_info, 0, sizeof(kt2_repl_grp_info));
            BCM_IF_ERROR_RETURN(_bcm_kt2_repl_list_tbl_ptr_get (unit, mmu_port,
               &kt2_repl_grp_info));
            repl_group_table  = kt2_repl_grp_info.mb_mem;
            member_bitmap_f = kt2_repl_grp_info.mbmp;
            head_index_f = BASE_PTRf;
        } else 
#endif /* BCM_KATANA2_SUPPORT */
        { 
            repl_group_table = MMU_REPL_GROUPm;
            member_bitmap_f = MEMBER_BITMAPf;
            head_index_f = HEAD_INDEXf;
 
            /* Triumph3 MMU does not support replication on ports 57, 59, 61 and 62 */
            if ((mmu_port == 57) || (mmu_port == 59) || (mmu_port == 61) || (mmu_port == 62)) {
                return BCM_E_PORT;
            } else if (mmu_port == 60) {
                member_bitmap_index = 59;
            }   
        }
    }

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        if (mmu_port >=120) {
            /* when port added to the replication list  >=120,
             * the IPMC_VLAN pointer is calculated based on number of
             * ports added to the multicast group, both the port members
             * in range 0-119 and 120-169
             */
            member_bitmap_index = mmu_port - 120;
            SOC_IF_ERROR_RETURN(READ_MMU_REPL_GRP_TBL0m(unit, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_grp_tbl0_entry));
            sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
            soc_MMU_REPL_GRP_TBL0m_field_get(unit, &repl_grp_tbl0_entry,
                   MEMBER_BMP_PORTS_119_0f, fldbuf);
            for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                SOC_PBMP_WORD_SET(member_bitmap0, i, fldbuf[i]);
            }

            SOC_PBMP_ITER(member_bitmap0, member) {
                member_id++;
            }
        }
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                  remap_grp_id, repl_group_entry));
    } else
#endif /* BCM_KATANA2_SUPPORT */
    {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, repl_group_table, MEM_BLOCK_ANY,
                repl_group, repl_group_entry));
    } 

    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
    soc_mem_field_get(unit, repl_group_table, repl_group_entry,
            member_bitmap_f, fldbuf);
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
        SOC_PBMP_WORD_SET(member_bitmap, i, fldbuf[i]);
    }
    if (!SOC_PBMP_MEMBER(member_bitmap, member_bitmap_index)) {
        /* Port is not set in member_bitmap */
        *start_ptr = 0;
        return BCM_E_NONE;
    }

    SOC_PBMP_ITER(member_bitmap, member) {
        if (member == member_bitmap_index) {
            break;
        }
        member_id++;
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        if (soc_mem_is_valid(unit, MMU_REPL_GRP_TBL2m)) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, MMU_REPL_GRP_TBL2m, MEM_BLOCK_ANY,
                    remap_grp_id, &repl_group_base_ptr_entry));
        head_index = member_id + soc_mem_field32_get(unit, MMU_REPL_GRP_TBL2m,
                &repl_group_base_ptr_entry, head_index_f);
        } else if (soc_mem_is_valid(unit, MMU_REPL_GRP_TBL1m)) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, MMU_REPL_GRP_TBL1m, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_group_base_ptr_entry));
            head_index = member_id + soc_mem_field32_get(unit, MMU_REPL_GRP_TBL1m,
                    &repl_group_base_ptr_entry, head_index_f);
        } else {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, MMU_REPL_GRP_TBLm, MEM_BLOCK_ANY,
                        remap_grp_id, &repl_group_base_ptr_entry));
            head_index = member_id + soc_mem_field32_get(unit, MMU_REPL_GRP_TBLm,
                    &repl_group_base_ptr_entry, head_index_f);
        }
    } else
#endif /* BCM_KATANA2_SUPPORT */
    {
        head_index = member_id + soc_mem_field32_get(unit, repl_group_table,
                repl_group_entry, head_index_f);
    }
    SOC_IF_ERROR_RETURN(READ_MMU_REPL_HEAD_TBLm(unit, MEM_BLOCK_ANY, head_index,
                &repl_head_entry));
    *start_ptr = soc_MMU_REPL_HEAD_TBLm_field32_get(unit, &repl_head_entry,
            HEAD_PTRf);

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_tr3_repl_list_free
 * Purpose:
 *	Free the REPL_LIST entries in the HW list starting at start_ptr.
 * Parameters:
 *	unit      - StrataSwitch PCI device unit number.
 *	start_ptr - Replication list's start pointer to
 *	            REPL_LIST table.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_list_free(int unit, int start_ptr)
{
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int	prev_repl_entry_ptr, repl_entry_ptr;

    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;

    while (repl_entry_ptr != prev_repl_entry_ptr) {
        SOC_IF_ERROR_RETURN
            (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                                     repl_entry_ptr, &repl_list_entry));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_repl_list_entry_free(unit, repl_entry_ptr)); 
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                &repl_list_entry, NEXTPTRf);
    }

    return BCM_E_NONE;
}

#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *      _bcm_kt2_repl_list_write
 * Purpose:
 *      Write the replication list contained in intf_vec into REPL_LIST table.
 *      Return the start_ptr and total interface count.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      intf_vec   - (IN) Vector of interfaces.
 *      nh_count   - (IN) Next hop count.
 *      start_ptr  - (OUT) Replication list's start pointer to
 *                   REPL_LIST table.
 *      intf_count - (OUT) Interface count.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_repl_list_write(int unit, SHR_BITDCL *intf_vec, 
                         uint32 nh_count, int *start_ptr, int *intf_count)
{
    int i;
    int remaining_count;
    int prev_repl_entry_ptr, repl_entry_ptr;
    uint32 msb_max, msb;
    uint32 ls_bits[2];
    int rv = BCM_E_NONE;
    int no_more_free_repl_entries;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int if_cnt, nh_cnt, offset;
    uint32 ms_bit;
    uint32 repl_type =0;

    if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
    nh_cnt = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    *intf_count = 0;

    for (i = 0; i < _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)); i++) {
        *intf_count += _shr_popcount(intf_vec[i]);
    }

    if (*intf_count == 0) {
        return BCM_E_NONE;
    }

    if (nh_count > 0) {
        repl_type = 1;
        msb_max = _SHR_BITDCLSIZE(nh_cnt) / 2; /* 32 -> 64 */
    } else {
        msb_max = _SHR_BITDCLSIZE(if_cnt) / 2; /* 32 -> 64 */
    }

    offset = (repl_type == 0)? 0 : _SHR_BITDCLSIZE(if_cnt);

    remaining_count = *intf_count;
    prev_repl_entry_ptr = -1;


    for (msb = 0; msb < msb_max; msb++) {

        ls_bits[0] = intf_vec[offset + 2 * msb + 0];
        ls_bits[1] = intf_vec[offset + 2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            rv = _bcm_tr3_repl_list_entry_alloc(unit, &repl_entry_ptr);
            if (rv == BCM_E_RESOURCE) {
                no_more_free_repl_entries = TRUE;
            } else if (BCM_FAILURE(rv)) {
                return rv;
            } else {
                no_more_free_repl_entries = FALSE;
            }
            if (prev_repl_entry_ptr == -1) {
                if (no_more_free_repl_entries) {
                    return BCM_E_RESOURCE;
                }
                *start_ptr = repl_entry_ptr;
            } else {
                if (no_more_free_repl_entries) {
                    /* Terminate replication list */
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXTPTRf, prev_repl_entry_ptr);
                } else {
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXTPTRf, repl_entry_ptr);
                }
                SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_LIST_TBLm(unit,
                            MEM_BLOCK_ALL, prev_repl_entry_ptr,
                            &repl_list_entry));
                if (no_more_free_repl_entries) {
                    /* Free the list already written */
                    _bcm_tr3_repl_list_free(unit, *start_ptr);
                    return BCM_E_RESOURCE;
                }
            }
            prev_repl_entry_ptr = repl_entry_ptr;

            sal_memset(&repl_list_entry, 0, sizeof(repl_list_entry));
            if (repl_type == 1) {
                /* Replication is over next hops */
                /* coverity[large_shift : FALSE] */
                ms_bit = msb + (1 << (soc_mem_field_length(unit, 
                                      MMU_REPL_LIST_TBLm, MSB_VLANf) - 1));
            } else {
                /* Replication is over interfaces */
                ms_bit = msb;
            }
            soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                    MSB_VLANf, ms_bit);
            soc_MMU_REPL_LIST_TBLm_field_set(unit, &repl_list_entry,
                    LSB_VLAN_BMf, ls_bits);

            remaining_count -= (_shr_popcount(ls_bits[0]) +
                    _shr_popcount(ls_bits[1]));
             if (remaining_count > 4) {
               /* Set the RMNG_REPS field to all ones */
                soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                        RMNG_REPSf, (1 << soc_mem_field_length(unit,
                        MMU_REPL_LIST_TBLm, RMNG_REPSf)) - 1);
             } else if (remaining_count > 0) {
                soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            RMNG_REPSf, remaining_count - 1);

             } else { /* No more intefaces left to be written */
                break;
             }
        }
    }

    if (prev_repl_entry_ptr > 0) {
        /* Write final entry */
        soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                          &repl_list_entry, NEXTPTRf, prev_repl_entry_ptr);
        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_LIST_TBLm(unit,
                   MEM_BLOCK_ALL, prev_repl_entry_ptr, &repl_list_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_repl_list_compare
 * Purpose:
 *      Compare replication list starting at given pointer to
 *      the interface list contained in intf_vec.
 * Parameters:
 *      unit      - StrataSwitch PCI device unit number.
 *      start_ptr - (IN) Replication list's start pointer to REPL_LIST table.
 *      intf_vec  - (IN) Vector of interfaces.
 *      nh_count  - (IN) next hops count
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_repl_list_compare(int unit, int start_ptr,
                          SHR_BITDCL *intf_vec, uint32 nh_count)
{
    uint32              msb, msb_val, hw_msb, msb_max;
    uint32              ls_bits[2], hw_ls_bits[2];
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int                 repl_entry_ptr, prev_repl_entry_ptr;
    uint32   offset, if_cnt, nh_cnt;
    uint32 repl_type =0;
    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;

    if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
    nh_cnt = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);


    if (nh_count > 0) {
        repl_type = 1;
        msb_max = _SHR_BITDCLSIZE(nh_cnt) / 2; /* 32 -> 64 */
    } else {
        msb_max = _SHR_BITDCLSIZE(if_cnt) / 2; /* 32 -> 64 */
    }

    offset = (repl_type == 0)? 0 : _SHR_BITDCLSIZE(if_cnt);

    for (msb = 0; msb < msb_max; msb++) {
        ls_bits[0] = intf_vec[offset + 2 * msb + 0];
        ls_bits[1] = intf_vec[offset + 2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            if (repl_entry_ptr == prev_repl_entry_ptr) { /* HW list end */
                return BCM_E_NOT_FOUND;
            }
            SOC_IF_ERROR_RETURN(READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                        repl_entry_ptr, &repl_list_entry));
            hw_msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                    &repl_list_entry, MSB_VLANf);
            soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                    LSB_VLAN_BMf, hw_ls_bits);

            if (repl_type == 1) {
                /* coverity[large_shift : FALSE] */
                msb_val = msb + (1 << (soc_mem_field_length(unit,
                                      MMU_REPL_LIST_TBLm, MSB_VLANf) - 1)); 
            } else {
                msb_val = msb;
            }
            if ((hw_msb != msb_val) || (ls_bits[0] != hw_ls_bits[0]) ||
                    (ls_bits[1] != hw_ls_bits[1])) {
                return BCM_E_NOT_FOUND;
            }
            prev_repl_entry_ptr = repl_entry_ptr;
            repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                    &repl_list_entry, NEXTPTRf);
        }
    }
    /*
     * Make sure that h/w has also reached to end.
     * only then we can say that this is a matched entry.
     */
    if (repl_entry_ptr != prev_repl_entry_ptr) {
        return BCM_E_NOT_FOUND;
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_repl_intf_vec_construct
 * Purpose:
 *      Construct replication interface vector.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      port       - (IN) Port.
 *      if_count   - (IN) Number of interfaces in replication list.
 *      if_array   - (IN) List of interface numbers.
 *      is_l3      - (IN) Indicates if interfaces are IPMC interfaces.
 *      check_port - (IN) If if_array consists of L3 interfaces, this parameter
 *                   controls whether to check the given port is a member
 *                   in each L3 interface's VLAN. This check should be
 *                   disabled when not needed, in order to improve
 *                   performance.
 *      intf_vec   - (OUT) Vector of interfaces.
 *      nh_count   - (OUT) Next hop Count
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_repl_intf_vec_construct(int unit, bcm_port_t port, int if_count,
        bcm_if_t *if_array, int is_l3, int check_port, SHR_BITDCL *intf_vec,
        uint32 *nh_count)
{
    int if_num;
    bcm_l3_intf_t l3_intf;
    uint32 partition;
    uint32 nextHop = 0;

    partition =  soc_mem_index_count(unit, EGR_L3_INTFm);

    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */

            if (if_array[if_num] > soc_mem_index_max(unit, EGR_L3_INTFm)) {
                return BCM_E_PARAM;
            }

            if (check_port) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = if_array[if_num];
                BCM_IF_ERROR_RETURN(bcm_esw_l3_intf_get(unit, &l3_intf));
            }

            /* Get a next hop index if the L3 inteface is not already
             * associated with one.
             */
            if (is_l3) { /* L3 interface is being added to IPMC group */
                SHR_BITSET(intf_vec, if_array[if_num]);
            }
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec, partition + if_array[if_num] -
                       BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            nextHop++;
        }
    }
    *nh_count = nextHop;

    return BCM_E_NONE;
}

#endif /* BCM_KATANA2_SUPPORT */
/*
 * Function:
 *	_bcm_tr3_repl_list_write
 * Purpose:
 *	Write the replication list contained in intf_vec into REPL_LIST table.
 *      Return the start_ptr and total interface count.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	start_ptr  - (OUT) Replication list's start pointer to
 *	             REPL_LIST table.
 *      intf_count - (OUT) Interface count.
 *	intf_vec   - (IN) Vector of interfaces.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_list_write(int unit, int *start_ptr, int *intf_count,
                        SHR_BITDCL *intf_vec)
{
    int i;
    int remaining_count;
    int prev_repl_entry_ptr, repl_entry_ptr;
    uint32 msb_max, msb;
    uint32 ls_bits[2];
    int rv = BCM_E_NONE;
    int no_more_free_repl_entries;
    mmu_repl_list_tbl_entry_t repl_list_entry;

    *intf_count = 0;
    for (i = 0; i < _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)); i++) {
        *intf_count += _shr_popcount(intf_vec[i]);
    }

    if (*intf_count == 0) {
        return BCM_E_NONE;
    }

    remaining_count = *intf_count;
    prev_repl_entry_ptr = -1;
    msb_max = _SHR_BITDCLSIZE(REPL_INTF_TOTAL(unit)) / 2; /* 32 -> 64 */
    for (msb = 0; msb < msb_max; msb++) {
        ls_bits[0] = intf_vec[2 * msb + 0];
        ls_bits[1] = intf_vec[2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            rv = _bcm_tr3_repl_list_entry_alloc(unit, &repl_entry_ptr);
            if (rv == BCM_E_RESOURCE) {
                no_more_free_repl_entries = TRUE;
            } else if (BCM_FAILURE(rv)) {
                return rv;
            } else {
                no_more_free_repl_entries = FALSE;
            }
            if (prev_repl_entry_ptr == -1) {
                if (no_more_free_repl_entries) {
                    return BCM_E_RESOURCE;
                }
                *start_ptr = repl_entry_ptr;
            } else {
                if (no_more_free_repl_entries) {
                    /* Terminate replication list */
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXTPTRf, prev_repl_entry_ptr);
                } else {
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXTPTRf, repl_entry_ptr);
                }
                SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_LIST_TBLm(unit,
                            MEM_BLOCK_ALL, prev_repl_entry_ptr,
                            &repl_list_entry));
                if (no_more_free_repl_entries) {
                    /* Free the list already written */
                    _bcm_tr3_repl_list_free(unit, *start_ptr);
                    return BCM_E_RESOURCE;
                }
            } 
            prev_repl_entry_ptr = repl_entry_ptr;

            sal_memset(&repl_list_entry, 0, sizeof(repl_list_entry));
            soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                    MSB_VLANf, msb);
            soc_MMU_REPL_LIST_TBLm_field_set(unit, &repl_list_entry,
                    LSB_VLAN_BMf, ls_bits);

            remaining_count -= (_shr_popcount(ls_bits[0]) +
                    _shr_popcount(ls_bits[1]));
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                if (remaining_count > 5) {
                    /* Set the RMNG_REPS field to all zeroes */
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            RMNG_REPSf, 0);
                } else if (remaining_count > 0) {
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            RMNG_REPSf, remaining_count);
                } else { /* No more intefaces left to be written */
                    break;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                if (remaining_count > 4) {
                    /* Set the RMNG_REPS field to all ones */
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            RMNG_REPSf, (1 << soc_mem_field_length(unit,
                                    MMU_REPL_LIST_TBLm, RMNG_REPSf)) - 1);
                } else if (remaining_count > 0) {
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            RMNG_REPSf, remaining_count - 1);

                } else { /* No more intefaces left to be written */
                    break;
                }
            }
        }
    }

    if (prev_repl_entry_ptr > 0) {
        /* Write final entry */
        soc_MMU_REPL_LIST_TBLm_field32_set(unit,
                          &repl_list_entry, NEXTPTRf, prev_repl_entry_ptr);
        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_LIST_TBLm(unit,
                   MEM_BLOCK_ALL, prev_repl_entry_ptr, &repl_list_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_tr3_repl_list_compare
 * Purpose:
 *	Compare replication list starting at given pointer to
 *	the interface list contained in intf_vec.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number.
 *	start_ptr - Replication list's start pointer to REPL_LIST table.
 *	intf_vec - Vector of interfaces.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_list_compare(int unit, int start_ptr,
                          SHR_BITDCL *intf_vec)
{
    uint32                    hw_msb;
    uint32                    hw_ls_bits[2];
    int                       repl_entry_ptr, prev_repl_entry_ptr;
    SHR_BITDCL                *hw_vec = NULL;
    int                       alloc_sz;
    int                       rv;
    mmu_repl_list_tbl_entry_t repl_list_entry;

    prev_repl_entry_ptr = -1;
    repl_entry_ptr = start_ptr;
    alloc_sz = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));

    hw_vec = sal_alloc(alloc_sz, "hw_vec");
    if (hw_vec == NULL ) {
        return SOC_E_MEMORY;
    }
    sal_memset(hw_vec, 0, alloc_sz);

    while (repl_entry_ptr != prev_repl_entry_ptr) {
        rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                                     repl_entry_ptr, &repl_list_entry);
        if(BCM_FAILURE(rv)) {
            sal_free(hw_vec);
            return rv;
        }

        hw_msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                                                    MSB_VLANf);
        soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry, LSB_VLAN_BMf,
                                         hw_ls_bits);
        hw_vec[2 * hw_msb + 0] = hw_ls_bits[0];
        hw_vec[2 * hw_msb + 1] = hw_ls_bits[1];
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                                                            &repl_list_entry,
                                                            NEXTPTRf);
    }

    if (SHR_BITEQ_RANGE(intf_vec, hw_vec, 0, REPL_INTF_TOTAL(unit))) {
        sal_free(hw_vec);
        return BCM_E_NONE;
    }

    sal_free(hw_vec);
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *	_bcm_tr3_repl_intf_vec_construct
 * Purpose:
 *	Construct replication interface vector.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	port       - Port.
 *	if_count   - Number of interfaces in replication list.
 *  if_array   - (IN) List of interface numbers.
 *  if_updated - (IN) interfaces info to be updated including
 *                    interfaces which is new to the group and port
 *                    and interfaces to be deleted.
 *  is_l3      - (IN) Indicates if interfaces are IPMC interfaces.
 *  check_port - If if_array consists of L3 interfaces, this parameter 
 *                   controls whether to check the given port is a member
 *                   in each L3 interface's VLAN. This check should be  
 *                   disabled when not needed, in order to improve
 *                   performance.
 *  intf_vec   - (OUT) Vector of interfaces.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_repl_intf_vec_construct(
     int unit, bcm_port_t port, int if_count, bcm_if_t *if_array,
     _tr3_if_updated_t *if_updated, int is_l3, int check_port,
     SHR_BITDCL *intf_vec)
{
    int if_num;
    bcm_l3_intf_t l3_intf;
    uint32 nh_flags;
    bcm_l3_egress_t nh_info;
    int nh_index;
    egr_l3_next_hop_entry_t egr_nh;
    int mac_oui, mac_non_oui;
    bcm_mac_t rbridge_mac;
#if defined(BCM_HGPROXY_COE_SUPPORT)
    bcm_port_t port_in = port;
#endif


#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &port));
    }
#endif

    if (is_l3) {
        bcm_if_t intf;
        int ref_count = 0;
        for (if_num = 0; if_num < if_updated->if_count_del; if_num++) {
            intf = if_updated->if_array_del[if_num];
            if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
                continue;
            }

            if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf) == 
                REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
                /* This next hop attached to the intf is allocated 
                 * by bcm_l3_egress_create. 
                 */
                continue;
            }
            
            if (intf > soc_mem_index_max(unit, EGR_L3_INTFm)) {
                return BCM_E_PARAM;
            }
            /* Decrease the NH ref count when the intf is deleted
             * from <group, port>. 
             */
            nh_index = REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf);
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_index));
            BCM_IF_ERROR_RETURN(
                bcm_xgs3_nh_ref_count_get(unit, nh_index, &ref_count));
            if (ref_count == 0) {
                REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf) = 
                    REPL_NH_INDEX_UNALLOCATED;
            }                
        }
    }
    
    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */

            if (if_array[if_num] > soc_mem_index_max(unit, EGR_L3_INTFm)) {
                return BCM_E_PARAM;
            }

            if (check_port) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = if_array[if_num];
                BCM_IF_ERROR_RETURN(bcm_esw_l3_intf_get(unit, &l3_intf));
            }

            if (is_l3) { /* L3 interface is being added to IPMC group */
                if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]) ==
                        REPL_NH_INDEX_UNALLOCATED) {
                    /* A next hop index has not been allocated for the L3
                     * inteface yet. Allocate a new next hop index.
                     */
                    bcm_l3_egress_t_init(&nh_info);
                    nh_flags = _BCM_L3_SHR_MATCH_DISABLE |
                        _BCM_L3_SHR_WRITE_DISABLE;
                    BCM_IF_ERROR_RETURN
                        (bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

                    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            ENTRY_TYPEf, 7);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__INTF_NUMf, if_array[if_num]);
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__L2_MC_DA_DISABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_DA_DISABLEf, 1);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__L2_MC_SA_DISABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_SA_DISABLEf, 1);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__L2_MC_VLAN_DISABLEf)) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L2_MC_VLAN_DISABLEf, 1);
                    }
#if defined(BCM_HGPROXY_COE_SUPPORT)
                 if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                     (soc_feature(unit, soc_feature_channelized_switching))) &&
                        BCM_GPORT_IS_SET(port_in) &&
                        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {
                        bcm_module_t mod_out;
                        bcm_port_t port_out;
                        bcm_trunk_t trunk_id;
                        int local_id;

                        BCM_IF_ERROR_RETURN(
                            _bcm_esw_gport_resolve(unit, port_in,
                                                   &mod_out, &port_out,
                                                   &trunk_id, &local_id));
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                L3MC__USE_MAC_DA_PROFILEf)) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                L3MC__USE_MAC_DA_PROFILEf,
                                                TRUE);
                        }
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                L3MC__HG_MC_DST_PORT_NUMf)) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                L3MC__HG_MC_DST_PORT_NUMf,
                                                port_out);
                        }
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                L3MC__HG_MC_DST_MODIDf)) {
                            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                                L3MC__HG_MC_DST_MODIDf,
                                                mod_out);
                        }
                    }
#endif
                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ALL, nh_index, &egr_nh));

                    REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]) =
                        nh_index;
                } else if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]) ==
                        REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
                    /* A next hop index has already been allocated for this
                     * L3 interface by the bcm_l3_egress_create API. Adding
                     * such L3 interface directly to the IPMC group is not
                     * allowed. To add the L3 egress object to an IPMC group,
                     * first call bcm_multicast_egress_object_encap_get to get
                     * an encap ID, then add the encap ID to the group.
                     */
                    return BCM_E_CONFIG;
                } else {
                    /* IPMC module had already silently allocated a next hop
                     * index for this L3 interface.
                     * But the next hop reference count should be increased
                     * if the intf is new to the <group, port>.
                     */
                    int if_num_new;
                    for (if_num_new = 0; if_num_new < if_updated->if_count_new;
                         if_num_new++) {
                        if (if_array[if_num] == 
                            if_updated->if_array_new[if_num_new]) {
                            nh_index = 
                                REPL_L3_INTF_NEXT_HOP_IPMC(unit,
                                                           if_array[if_num]);
                            _bcm_xgs3_nh_ref_cnt_incr(unit, nh_index);
                            break;
                        }
                    }
                }

                SHR_BITSET(intf_vec,
                    REPL_L3_INTF_NEXT_HOP_IPMC(unit, if_array[if_num]));

            } else { /* L3 interface is being added to Trill group */
                if (REPL_L3_INTF_NEXT_HOP_TRILL(unit, if_array[if_num]) == -1) {
                    bcm_l3_egress_t_init(&nh_info);
                    nh_flags = _BCM_L3_SHR_MATCH_DISABLE |
                        _BCM_L3_SHR_WRITE_DISABLE;
                    BCM_IF_ERROR_RETURN
                        (bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

                    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
#ifdef BCM_TRIDENT2_SUPPORT
                    if (SOC_IS_TD2_TT2(unit)) {
                        /*
                         * TD2 needs to set the entry_type to 0x7 (L3MC) 
                         * in order to use trill_all_rbridges_macda as the DA 
                         * in TRILL header 
                         */
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                ENTRY_TYPEf, 7);
                    } else
#endif /* BCM_TRIDENT2_SUPPORT */                        
                    {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                ENTRY_TYPEf, 0);
                    }
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__INTF_NUMf, if_array[if_num]);

                    /* Get and Set rbridge mac */
                    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                            bcmSwitchTrillBroadcastDestMacOui, &mac_oui)); 
                    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                            bcmSwitchTrillBroadcastDestMacNonOui, &mac_non_oui)); 
                    rbridge_mac[0] = (mac_oui >> 16) & 0xff;
                    rbridge_mac[1] = (mac_oui >> 8) & 0xff;
                    rbridge_mac[2] = mac_oui & 0xff;
                    rbridge_mac[3] = (mac_non_oui >> 16) & 0xff;
                    rbridge_mac[4] = (mac_non_oui >> 8) & 0xff;
                    rbridge_mac[5] = mac_non_oui & 0xff;
                    soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3__MAC_ADDRESSf, rbridge_mac);

                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ALL, nh_index, &egr_nh));

                    REPL_L3_INTF_NEXT_HOP_TRILL(unit, if_array[if_num]) =
                        nh_index;
                }
                SHR_BITSET(intf_vec,
                    REPL_L3_INTF_NEXT_HOP_TRILL(unit, if_array[if_num]));
            }
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec,
                    if_array[if_num] - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_tr3_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	replication group.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port to assign replication list.
 *	if_count   - Number of interfaces in replication list.
 *  if_array   - (IN) List of interface numbers.
 *  if_updated - (IN) interfaces info to be updated including
 *                    interfaces which is new to the group and port
 *                    and interfaces to be deleted.
 *  is_l3      - (IN) Indicates if multicast group is of type IPMC.
 *  check_port - (IN) If if_array consists of L3 interfaces, this parameter 
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be  
 *                        disabled when not needed, in order to improve
 *                        performance.
 *  prev_start_ptr - (IN) Start pointer of existing replication list.
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_tr3_ipmc_egress_intf_set(
    int unit, int repl_group, bcm_port_t port, int if_count,
    bcm_if_t *if_array, _tr3_if_updated_t *if_updated,
    int is_l3, int check_port, int prev_start_ptr)
{
    int rv = BCM_E_NONE;
    int set_repl_list;
    int list_start_ptr=0;
    int alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    int repl_hash;
    _bcm_repl_list_info_t *rli_current, *rli_prev;
    int intf_count;
    int new_repl_list = FALSE;
#ifdef BCM_KATANA2_SUPPORT
    soc_field_t  last_ptr = INVALIDf;
    soc_mem_t    rqe_mem = INVALIDm;
    mmu_ipmc_group_tbl0_entry_t rqe_entry;
    uint32 nh_count =0;
    _kt2_rep_regs_t     *rqe_arr_ptr;
    int ipmc_id = 0 ;
    int last = 0;
#endif /* BCM_KATANA2_SUPPORT */
    int repl_intf_total;
    int intf_cnt_old = 0;
    bcm_port_t port_in = port;


    REPL_INIT(unit);
    REPL_GROUP_ID(unit, repl_group);

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &port));
    }
#endif

    /* Replication is allowed on CPU ports, only on TD2 */
    if (IS_CPU_PORT(unit, port)) {
        if (!SOC_IS_TRIDENT2X(unit)) {
            return BCM_E_PARAM;
        }
    } else {   
        REPL_PORT_CHECK(unit, port);
    }

    repl_intf_total = REPL_INTF_TOTAL(unit);
    if (if_count > repl_intf_total) {
        return BCM_E_PARAM;
    } else if (if_count > 0) {
        set_repl_list = TRUE;
    } else {
        set_repl_list = FALSE;
    }

    intf_cnt_old = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    if (set_repl_list) {
        /* Set new replication list for given (repl_group, port) */

        alloc_size = SHR_BITALLOCSIZE(repl_intf_total);
        intf_vec = sal_alloc(alloc_size, "Repl interface vector");
        if (intf_vec == NULL) {
            rv = BCM_E_MEMORY;
            goto intf_set_done;
        }
        sal_memset(intf_vec, 0, alloc_size);

        /* Interface validation and vector construction */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            rv = _bcm_kt2_repl_intf_vec_construct(unit, port, if_count, 
                      if_array, is_l3, check_port, intf_vec, &nh_count);
        }
        else
#endif /* BCM_KATANA2_SUPPORT */
        {
            rv = _bcm_tr3_repl_intf_vec_construct(unit, port_in, if_count,
                                                  if_array, if_updated,
                                                  is_l3, check_port, intf_vec);
        }
        if (BCM_FAILURE(rv)) {
            goto intf_set_done;
        }

        /* Search for matching replication list */
        if ((_shr_popcount(*(uint32 *)intf_vec) == if_count) && 
            (repl_intf_total >= SHR_BITWID)) {
            repl_hash = _shr_crc32b(0, (uint8 *)intf_vec, SHR_BITWID);
        } else {
            repl_hash = _shr_crc32b(0, (uint8 *)intf_vec, repl_intf_total);
        }
        for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
                rli_current = rli_current->next) {
            if (repl_hash == rli_current->hash) {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    rv = _bcm_kt2_repl_list_compare(unit, rli_current->index,
                          intf_vec, nh_count);
                } else 
#endif /* BCM_KATANA2_SUPPORT */
                {    
                    rv = _bcm_tr3_repl_list_compare(unit, rli_current->index,
                        intf_vec);
                }
                if (rv == BCM_E_NOT_FOUND) {
                    continue; /* Not a match */
                } else if (BCM_FAILURE(rv)) {
                    goto intf_set_done; 
                } else {
                    break; /* Match */
                }
            }
        }

        if (rli_current != NULL) {
            /* Found a match */
            if (prev_start_ptr == rli_current->index) {
                /* (repl_group, port) already points to this list, so done */
                rv = BCM_E_NONE;
                goto intf_set_done;
            } else {
                list_start_ptr = rli_current->index;
                intf_count = rli_current->list_size;
            }
        } else {
            /* Not a match, make a new chain */
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
               rv = _bcm_kt2_repl_list_write(unit,
                     intf_vec, nh_count, &list_start_ptr, &intf_count);
            } else 
#endif /* BCM_KATANA2_SUPPORT */
            {
               rv = _bcm_tr3_repl_list_write(unit, &list_start_ptr,
                     &intf_count, intf_vec);
            }
            if (BCM_FAILURE(rv)) {
                goto intf_set_done;
            }

            if (intf_count > 0) {
                /* Update data structures */
                alloc_size = sizeof(_bcm_repl_list_info_t);
                rli_current = sal_alloc(alloc_size, "IPMC repl list info");
                if (rli_current == NULL) {
                    /* Release list */
                    _bcm_tr3_repl_list_free(unit, list_start_ptr);
                    rv = BCM_E_MEMORY;
                    goto intf_set_done;
                }
                sal_memset(rli_current, 0, alloc_size);
                rli_current->index = list_start_ptr;
                rli_current->hash = repl_hash;
                rli_current->list_size = intf_count;
                rli_current->next = REPL_LIST_INFO(unit);
                REPL_LIST_INFO(unit) = rli_current;
                new_repl_list = TRUE;
            } else {
                rv = BCM_E_INTERNAL;
                goto intf_set_done;
            }
        }

        /* Update the interface count before updating replication list
         * start pointer, since _bcm_tr3_repl_list_start_ptr_set
         * depends on correct interface count.
         */
        REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = intf_count;

#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_IS_SABER2(unit)) {
#ifdef BCM_SABER2_SUPPORT
                rqe_arr_ptr = sb2_rqe_rep_regs;
#endif
            } else { 
                rqe_arr_ptr = kt2_rqe_rep_regs;
            }
            rqe_mem = rqe_arr_ptr[port].mem;
            last_ptr = rqe_arr_ptr[port].last_ptr;
            last = ((intf_count == 1) ? 1 : 0);
            rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit,repl_group, 
                                               &ipmc_id, FALSE);
            if (rv < 0) { 
                goto intf_set_done;
            }
            soc_mem_lock(unit, rqe_mem);
            if ((rv = soc_mem_read(unit, rqe_mem, MEM_BLOCK_ALL,
                                    ipmc_id, &rqe_entry)) < 0) {
                 soc_mem_unlock(unit, rqe_mem);
                 goto intf_set_done;
            }
            soc_mem_field32_set(unit, rqe_mem, &rqe_entry, last_ptr, last ? 1 : 0); 
            if ((rv = soc_mem_write(unit, rqe_mem, MEM_BLOCK_ALL, ipmc_id,
                                                   &rqe_entry)) < 0) {
                 soc_mem_unlock(unit, rqe_mem);
                 goto intf_set_done;
            }
            soc_mem_unlock(unit, rqe_mem);
        }
#endif 
        /* Update replication list start pointer */
        rv = _bcm_tr3_repl_list_start_ptr_set(unit, repl_group, port,
                list_start_ptr, intf_count);
        if (BCM_FAILURE(rv)) {
            if (new_repl_list) {
                _bcm_tr3_repl_list_free(unit, list_start_ptr);
                REPL_LIST_INFO(unit) = rli_current->next;
                sal_free(rli_current);
            }
            REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = intf_cnt_old;
            goto intf_set_done;
        }
        (rli_current->refcount)++;
       

    } else { 
        if (!SOC_IS_KATANA2(unit)) {
            rv = _bcm_tr3_repl_intf_vec_construct(unit, port, if_count,
                                                  if_array, if_updated,
                                                  is_l3, check_port, NULL);
        }
        /* Delete replication list for given (repl_group, port) */
        REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = 0;
        if (prev_start_ptr != 0) {
            rv = _bcm_tr3_repl_list_start_ptr_set(unit, repl_group, port, 0, 0);
            if (BCM_FAILURE(rv)) {
                REPL_PORT_GROUP_INTF_COUNT(unit, port,
                                           repl_group) = intf_cnt_old;
                goto intf_set_done;
            }
        }        
    }

    /* Delete old replication list */
    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
                rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    _bcm_tr3_repl_list_free(unit, prev_start_ptr);
                    if (rli_prev == NULL) {
                        REPL_LIST_INFO(unit) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
                    sal_free(rli_current);
                }
                break;
            }
            rli_prev = rli_current;
        }
    }

intf_set_done:

    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }

    return rv;
}
#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *      _bcm_kt2_init_check
 * Purpose:
 *      Katana2 initialization 
 *      unit         - StrataSwitch PCI device unit number.
 *      ipmc_id      - The replication group number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_init_check(int unit, int ipmc_id)
{
   REPL_INIT(unit);
   REPL_GROUP_ID(unit, ipmc_id);
   return BCM_E_NONE;

}

/*
 * Function:
 *	_bcm_kt2_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	replication group.
 * Parameters:
 *	unit         - StrataSwitch PCI device unit number.
 *	repl_group   - The replication group number.
 *	port         - Port to assign replication list.
 *      ipmc_ptr     - 
 *	if_count     - Number of interfaces in replication list.
 *      if_array     - (IN) List of interface numbers.
 *      new_ipmc_ptr - (OUT) repl_ptr 
 *      last_ipmc_flag - (OUT) flag indicating last replication
 *      check_port - (IN) If if_array consists of L3 interfaces, this parameter 
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be  
 *                        disabled when not needed, in order to improve
 *                        performance.
 * Returns:
 *	BCM_E_XXX
 */
int
_bcm_kt2_ipmc_egress_intf_set(int unit, int repl_group, bcm_port_t port,
                             int ipmc_ptr, int if_count, bcm_if_t *if_array, 
                             int *new_ipmc_ptr, int *last_ipmc_flag,
                             int check_port)
{
    int rv = BCM_E_NONE;
    int prev_start_ptr, list_start_ptr=0;
    int alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    int repl_hash;
    _bcm_repl_list_info_t *rli_current, *rli_prev;
    int intf_count = 0;
    int last_flag;
    uint32 nh_count = 0;
    int if_num, partition;
    bcm_l3_intf_t       l3_intf;
    pbmp_t              pbmp, ubmp;
    int set_repl_list;


    REPL_INIT(unit);
    REPL_GROUP_ID(unit, repl_group);
    REPL_PORT_CHECK(unit, port);

    if (if_count > REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    } else if (if_count > 0) {
        set_repl_list = TRUE;
    } else {
        set_repl_list = FALSE;
    }

    REPL_LOCK(unit);

    prev_start_ptr = ipmc_ptr;

    if (set_repl_list == FALSE) {
        *new_ipmc_ptr = 0;
    } else {

        partition = soc_mem_index_count(unit, EGR_L3_INTFm);

        alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
        intf_vec = sal_alloc(alloc_size, "Repl interface vector");
        if (intf_vec == NULL) {
            rv = BCM_E_MEMORY;
            goto intf_set_done;
        }
        sal_memset(intf_vec, 0, alloc_size);

        /* Interface validation and vector construction */
        for (if_num = 0; if_num < if_count; if_num++) {
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num]) &&
                (uint32)(if_array[if_num]) > partition) {
                rv = BCM_E_PARAM;
                goto intf_set_done;
            }
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
                /* L3 interface is used */
                if (check_port) {
                    bcm_l3_intf_t_init(&l3_intf);
                    l3_intf.l3a_intf_id = if_array[if_num];
                    if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                        goto intf_set_done;
                    }
                    if ((rv = bcm_esw_vlan_port_get(unit, l3_intf.l3a_vid,
                                    &pbmp, &ubmp)) < 0) {
                        goto intf_set_done;
                    }
                    if (!BCM_PBMP_MEMBER(pbmp, port)) {
                        rv = BCM_E_PARAM;
                        goto intf_set_done;
                    }
                }
                SHR_BITSET(intf_vec, if_array[if_num]);
                intf_count++;
            } else {
                /* Next hop is used */
                SHR_BITSET(intf_vec, partition + if_array[if_num] -
                           BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
                nh_count++;
            }
        }

        /* Search for list already in table */
        repl_hash = _shr_crc32b(0, (uint8 *)intf_vec, REPL_INTF_TOTAL(unit));
        for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
            rli_current = rli_current->next) {
            if (repl_hash == rli_current->hash) {
                rv = _bcm_kt2_repl_list_compare(unit, rli_current->index,
                          intf_vec, nh_count);
                if (rv == BCM_E_NOT_FOUND) {
                    continue; /* Not a match */
                } else if (BCM_FAILURE(rv)) {
                    goto intf_set_done;
                } else {
                    break; /* Match */
                }
            }
        }

        if (rli_current != NULL) {
            /* Found a match */
            if (prev_start_ptr == rli_current->index) {
                /* (repl_group, port) already points to this list, so done */
                rv = BCM_E_NONE;
                goto intf_set_done;
            } else {
                list_start_ptr = rli_current->index;
                intf_count = rli_current->list_size;
            }
        } else {
            /* Not a match, make a new chain */
            rv = _bcm_kt2_repl_list_write(unit,
                     intf_vec, nh_count, &list_start_ptr, &intf_count);
            if (BCM_FAILURE(rv)) {
                goto intf_set_done;
            }

            if (intf_count > 0) {
                /* Update data structures */
                alloc_size = sizeof(_bcm_repl_list_info_t);
                rli_current = sal_alloc(alloc_size, "IPMC repl list info");
                if (rli_current == NULL) {
                    /* Release list */
                    _bcm_tr3_repl_list_free(unit, list_start_ptr);
                    rv = BCM_E_MEMORY;
                    goto intf_set_done;
                }
                sal_memset(rli_current, 0, alloc_size);
                rli_current->index = list_start_ptr;
                rli_current->hash = repl_hash;
                rli_current->list_size = intf_count;
                rli_current->next = REPL_LIST_INFO(unit);
                REPL_LIST_INFO(unit) = rli_current;
            } else {
                rv = BCM_E_INTERNAL;
                goto intf_set_done;
            }
        }

        last_flag = (intf_count == 1);
        *last_ipmc_flag = last_flag;
        if (intf_count > 0) {
            (rli_current->refcount)++;
        } else if (prev_start_ptr != 0) {
            list_start_ptr = 0;
        }
        *new_ipmc_ptr = list_start_ptr;

    }

    /* Delete old replication list */
    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
                rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    _bcm_tr3_repl_list_free(unit, prev_start_ptr);
                    if (rli_prev == NULL) {
                        REPL_LIST_INFO(unit) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
                    sal_free(rli_current);
                }
                break;
            }
            rli_prev = rli_current;
        }
    }

intf_set_done:

    REPL_UNLOCK(unit);

    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_kt2_ipmc_egress_intf_add
 * Purpose:
 *      Add L3 interface to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit         - StrataSwitch PCI device unit number.
 *      ipmc_id      - IPMC group number.
 *      port         - port to add.
 *      ipmc_ptr     - replication pointer.
 *      id           - encap_id.
 *      new_ipmc_ptr - (OUT) repl_ptr
 *      last_ipmc_flag - (OUT) flag indicating last replication
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                              int ipmc_ptr, int id, int  *new_ipmc_ptr,
                              int *last_flag)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    pbmp_t pbmp, ubmp;
    int                 partition;
    bcm_l3_intf_t       l3_intf;
    int i = 0;

    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, ipmc_id));

    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = _bcm_kt2_ipmc_egress_intf_get(unit, ipmc_id, ipmc_ptr,
                                       if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {

        for (i = 0 ;i < intf_num ; i++ ) {
             if (if_array[i] == id) { 
                 rv =  BCM_E_EXISTS;
                 goto intf_add_done;
             }
        }
        if (intf_num < intf_max) {
            if_array[intf_num++] = id;

            /* For IPMC, check port is a member of the L3 interface's VLAN.
             * Performing this check here is more efficient than performing
             * it in bcm_tr2_ipmc_egress_intf_set.
             */
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, id)) {
                /* L3 interface is used */
                partition = soc_mem_index_count(unit, EGR_L3_INTFm);
                if (id > partition) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }

                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = id;
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto intf_add_done;
                }
                if ((rv = bcm_esw_vlan_port_get(unit, l3_intf.l3a_vid,
                                &pbmp, &ubmp)) < 0) {
                    goto intf_add_done;
                }
                if (!BCM_PBMP_MEMBER(pbmp, port)) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }
            }

            rv = _bcm_kt2_ipmc_egress_intf_set(unit, ipmc_id, port, 
                                               ipmc_ptr, intf_num, if_array, 
                                               new_ipmc_ptr, last_flag, FALSE);
        } else {
            rv = BCM_E_FULL;
        }
    }

intf_add_done:
    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_kt2_ipmc_egress_intf_get
 * Purpose:
 *      Retrieve set of egress interfaces to port's replication list for chosen
 *      replication list.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      repl_group - The replication group number.
 *      ipmc_ptr   - Relication pointer
 *      if_array   - (OUT) List of interface numbers.
 *      if_count   - (OUT) Number of interfaces in replication list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter if_max = 0, return in the output parameter
 *      if_count the total number of interfaces in the specified multicast
 *      group's replication list.
 */
int 
_bcm_kt2_ipmc_egress_intf_get(int unit, int repl_group, 
                             int ipmc_ptr, bcm_if_t *if_array, 
                             int *if_count)
{
    int rv = BCM_E_NONE;
    int prev_repl_entry_ptr, repl_entry_ptr;
    int intf_count;
    int intf_base;
    int ls_pos;
    uint32 ls_bits[2];
    mmu_repl_list_tbl_entry_t repl_list_entry;
    uint32 nh_offset = 0;
    uint32  is_len = 0;
    int if_max =0;


    BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, repl_group));

    REPL_LOCK(unit);

    is_len = soc_mem_field_length(unit, MMU_REPL_LIST_TBLm, LSB_VLAN_BMf);
    /* coverity[large_shift : FALSE] */
    nh_offset = (1 << (soc_mem_field_length(unit, MMU_REPL_LIST_TBLm,
                                                MSB_VLANf) - 1)) * is_len;
    if_max = REPL_INTF_TOTAL(unit);

    if (if_max < 0) {
        return BCM_E_PARAM;
    } else if (if_max > 0 && NULL == if_array) {
        return BCM_E_PARAM;
    }

    if (NULL == if_count) {
        return BCM_E_PARAM;
    }

    prev_repl_entry_ptr = -1;
    intf_count = 0;
    repl_entry_ptr = ipmc_ptr;

    while (repl_entry_ptr != prev_repl_entry_ptr) {
        rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                repl_entry_ptr, &repl_list_entry);
        if (BCM_FAILURE(rv)) {
            goto intf_get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                MSB_VLANf) * 64;
        soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                LSB_VLAN_BMf, ls_bits);

        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (if_max == 0) {
                    intf_count++;
                } else {
                        /* check if L3 interface or nexthop */
                        if (intf_base >= nh_offset) {
                            /* Entry contains next hops */
                            if_array[intf_count] = intf_base - nh_offset +
                                ls_pos + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                        } else {
                            /* Entry contains interfaces */
                            if_array[intf_count] = intf_base + ls_pos;
                        }
                    intf_count++;
                    if (intf_count == if_max) {
                        *if_count = intf_count;
                        goto intf_get_done;
                    }
                }
            } 
        } 
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                &repl_list_entry, NEXTPTRf);
    }

    *if_count = intf_count;

intf_get_done:
    REPL_UNLOCK(unit);
    return rv;

}

#if defined(BCM_KATANA2_SUPPORT)
int
_bcm_kt2_ipmc_egress_intf_delete_all( int unit, int ipmc_id, int ipmc_ptr)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count, rv = BCM_E_NONE;
    SHR_BITDCL *intf_vec = NULL;
    int if_num, partition;
    int intf_count= 0, repl_hash;
    _bcm_repl_list_info_t *rli_start,*rli_current, *rli_prev = NULL;
    uint32 nh_count = 0;

    partition = soc_mem_index_count(unit, EGR_L3_INTFm);
    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max *sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "Repl interface vector");
    if (intf_vec == NULL) {
        sal_free(if_array);
        return BCM_E_MEMORY;
    }
    sal_memset(intf_vec, 0, alloc_size);
    REPL_LOCK(unit);
    rv = _bcm_kt2_ipmc_egress_intf_get (unit, ipmc_id, ipmc_ptr,
            if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        /*prepare the interface vector and find out the refrence count */
        for (if_num = 0; if_num < if_count; if_num++) {
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num]) &&
                    (uint32)(if_array[if_num]) > partition) {
                rv = BCM_E_PARAM;
                break;
            }
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
                /* L3 interface is used */
                SHR_BITSET(intf_vec, if_array[if_num]);
                intf_count++;
            } else {
                /* Next hop is used */
                SHR_BITSET(intf_vec, partition + if_array[if_num] -
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
                nh_count++;
            }
        }
        rli_start = REPL_LIST_INFO(unit);
        repl_hash =
            _shr_crc32b(0, (uint8 *)intf_vec, REPL_INTF_TOTAL(unit));

        for (rli_current = rli_start; rli_current != NULL;
                rli_current = rli_current->next) {
            if (repl_hash == rli_current->hash) {
                rv = _bcm_kt2_repl_list_compare(unit, rli_current->index,
                        intf_vec, nh_count);
                if (rv == BCM_E_NOT_FOUND) {
                    continue; /* Not a match */
                } else if (rv < 0) {
                    goto error; /* Access error */
                } else {
                    break; /* Match */
                }
            }   
            rli_prev = rli_current;
        }

        if (rli_current){
            rli_current->refcount-- ;
            if (rli_current->refcount == 0){
                rv = _bcm_tr3_repl_list_free(unit, rli_current->index);
                /* If we have an error, we'll fall out anyway */
                if (rli_prev == NULL) {
                    REPL_LIST_INFO(unit) = rli_current->next;
                } else {
                    rli_prev->next = rli_current->next;
                }
                sal_free(rli_current);
            }
        }
    }
error:
    sal_free(if_array);
    sal_free(intf_vec);
    REPL_UNLOCK(unit);
    return rv;
}
#endif

/*
 * Function:
 *      bcm_kt2_ipmc_egress_intf_delete
 * Purpose:
 *      Remove L3 interface from selected ports' replication list for chosen
 *      Relication list.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      ipmc_ptr     - replication pointer.
 *      id           - encap_id.
 *      new_ipmc_ptr - (OUT) repl_ptr
 *      last_ipmc_flag - (OUT) flag indicating last replication
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                int ipmc_ptr, int id, int  *new_ipmc_ptr,
                                int *last_flag)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = _bcm_kt2_ipmc_egress_intf_get(unit, ipmc_id, ipmc_ptr,
                                       if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = _bcm_kt2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             ipmc_ptr, if_count, if_array,
                                            new_ipmc_ptr, last_flag, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

#endif
/*
 * Function:
 *      _bcm_tr3_ipmc_egress_intf_get
 * Purpose:
 *	Retrieve set of egress interfaces to port's replication list for chosen
 *	replication group.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port from which to get replication list.
 *	if_max     - Maximum number of interfaces in replication list.
 *      if_array   - (OUT) List of interface numbers.
 *	if_count   - (OUT) Number of interfaces in replication list.
 * repl_start_ptr - (OUT) Start pointer of existing replication list.
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *      If the input parameter if_max = 0, return in the output parameter
 *      if_count the total number of interfaces in the specified multicast 
 *      group's replication list.
 */
int
_bcm_tr3_ipmc_egress_intf_get(int unit, int repl_group, bcm_port_t port,
                              int if_max, bcm_if_t *if_array, int *if_count,
                              int *repl_start_ptr)
{
    int rv = BCM_E_NONE;
    int prev_repl_entry_ptr, repl_entry_ptr;
    int intf_count;
    int l3_intf, intf_base;
    int ls_pos;
    uint32 ls_bits[2];
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int next_hop_index;
    int entry_type;
    egr_l3_next_hop_entry_t egr_nh;
#if defined (BCM_KATANA2_SUPPORT)
    uint32 nh_offset = 0;
    uint32  is_len = 0;
#endif


    REPL_INIT(unit);
    REPL_GROUP_ID(unit, repl_group);
    /* Replication is allowed on CPU ports, only on TD2 */
    if(IS_CPU_PORT(unit, port)) {
        if(!SOC_IS_TRIDENT2X(unit)) {
            return BCM_E_PARAM;
        }
    }
    else
    {   
        REPL_PORT_CHECK(unit, port);
    }

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        is_len = soc_mem_field_length(unit, MMU_REPL_LIST_TBLm, LSB_VLAN_BMf);
        /* coverity[negative_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_REPL_LIST_TBLm,
                                                MSB_VLANf) - 1)) * is_len;
    }
#endif

    if (if_max < 0) {
        return BCM_E_PARAM;
    } else if (if_max > 0 && NULL == if_array) {
        return BCM_E_PARAM;
    }

    if (NULL == if_count) {
        return BCM_E_PARAM;
    }

    if (REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) == 0) {
        *if_count = 0;
        return BCM_E_NONE;
    }

    rv = _bcm_tr3_repl_list_start_ptr_get(unit, repl_group, port,
            &repl_entry_ptr);
    if (BCM_FAILURE(rv)) {
        goto intf_get_done;
    } else {
        *repl_start_ptr = repl_entry_ptr;
    }

    prev_repl_entry_ptr = -1;
    intf_count = 0;
    while (repl_entry_ptr != prev_repl_entry_ptr) {
        rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                repl_entry_ptr, &repl_list_entry);
        if (BCM_FAILURE(rv)) {
            goto intf_get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                MSB_VLANf) * 64;
        soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                LSB_VLAN_BMf, ls_bits);

        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (if_max == 0) {
                    intf_count++;
                } else {
#if defined(BCM_KATANA2_SUPPORT)
                    if (SOC_IS_KATANA2(unit))
                    {
                        /* check if L3 interface or nexthop */
                        if (intf_base >= nh_offset) {
                            /* Entry contains next hops */
                            if_array[intf_count] = intf_base - nh_offset +
                                ls_pos + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                        } else {
                            /* Entry contains interfaces */
                            if_array[intf_count] = intf_base + ls_pos;
                        }
                    }
                    else
#endif
                    {

                        /* Check if L3 interfaces or next hop */
                    
                        next_hop_index = intf_base + ls_pos;
                        if_array[intf_count] = next_hop_index +
                         BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);                

                        /* Check if next hop index corresponds to
                         * an IPMC or Trill interface.
                         */
                        rv = READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                             next_hop_index, &egr_nh);
                        if (BCM_FAILURE(rv)) {
                            goto intf_get_done;
                        }
                        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                     &egr_nh, ENTRY_TYPEf);
                        if (entry_type == 0) {
                            l3_intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                &egr_nh, L3__INTF_NUMf);
                            if (REPL_L3_INTF_NEXT_HOP_TRILL(unit, l3_intf) ==
                                next_hop_index) {
                                if_array[intf_count] = l3_intf;
                            } 
                        }
                        else if (entry_type == 7) {
                            l3_intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                                  &egr_nh, L3MC__INTF_NUMf);
                            if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, l3_intf) ==
                                  next_hop_index) {
                               if_array[intf_count] = l3_intf;
                            } 
                        }
                    } /* end - if not KATANA2*/
                    intf_count++;
                    if (intf_count == if_max) {
                        *if_count = intf_count;
                        goto intf_get_done;
                    }
                }
            } 
        } 
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                &repl_list_entry, NEXTPTRf);
        if (intf_count >= REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group)) {
            break;
        }
    }

    *if_count = intf_count;

intf_get_done:
    return rv;
}

/*
 * Function:
 *      bcm_tr3_ipmc_egress_intf_get
 * Purpose:
 *      Retrieve set of egress interfaces to port's replication list for chosen
 *      replication group.
 * Parameters:
 *      unit       - StrataSwitch PCI device unit number.
 *      repl_group - The replication group number.
 *      port       - Port from which to get replication list.
 *      if_max     - Maximum number of interfaces in replication list.
 *      if_array   - (OUT) List of interface numbers.
 *      if_count   - (OUT) Number of interfaces in replication list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter if_max = 0, return in the output parameter
 *      if_count the total number of interfaces in the specified multicast
 *      group's replication list.
 */
int
bcm_tr3_ipmc_egress_intf_get(int unit, int repl_group, bcm_port_t port,
                             int if_max, bcm_if_t *if_array, int *if_count)
{
    int rv = BCM_E_NONE;
    int repl_start_ptr = 0;

    REPL_LOCK(unit);
    rv = _bcm_tr3_ipmc_egress_intf_get(unit, repl_group, port,
                                       if_max, if_array, if_count,
                                       &repl_start_ptr);
    REPL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_tr3_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	replication group.
 * Parameters:
 *	unit       - StrataSwitch PCI device unit number.
 *	repl_group - The replication group number.
 *	port       - Port to assign replication list.
 *	if_count   - Number of interfaces in replication list.
 *  if_array   - (IN) List of interface numbers.
 *  is_l3      - (IN) Indicates if multicast group is of type IPMC.
 *  check_port - (IN) If if_array consists of L3 interfaces, this parameter 
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be  
 *                        disabled when not needed, in order to improve
 *                        performance.
 * Returns:
 *	BCM_E_XXX
 */
int 
bcm_tr3_ipmc_egress_intf_set(
    int unit, int repl_group, bcm_port_t port,
    int if_count, bcm_if_t *if_array, int is_l3, int check_port)
{
    int rv = BCM_E_NONE;
    _tr3_if_updated_t  if_updated;
    bcm_if_t *if_array_cur = NULL;
    int intf_num_cur, intf_max, alloc_size;
    int if_index, if_cur_index;
    int repl_start_ptr = 0;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, repl_group);

    if (IS_CPU_PORT(unit, port)) {
        if (!SOC_IS_TRIDENT2X(unit)) {
            return BCM_E_PARAM;
        }
    } else {
        REPL_PORT_CHECK(unit, port);
    }

    if ((if_count == 0) &&
        (REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) == 0)) {
        return BCM_E_NONE;
    }

    sal_memset(&if_updated, 0 , sizeof(if_updated));
    
    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    
    if_array_cur = sal_alloc(alloc_size, "IPMC intf array current");
    if (if_array_cur == NULL) {
        rv = BCM_E_MEMORY;
        goto intf_cleanup;
    }

    if_updated.if_array_new = sal_alloc(if_count * sizeof(bcm_if_t),
                                        "IPMC intf array new");
    if (if_updated.if_array_new == NULL) {
        rv = BCM_E_MEMORY;   
        goto intf_cleanup;
    }

    REPL_LOCK(unit);
    rv = _bcm_tr3_ipmc_egress_intf_get(unit, repl_group, port,
                                       intf_max, if_array_cur, &intf_num_cur,
                                       &repl_start_ptr);
    if (BCM_FAILURE(rv)) {
        REPL_UNLOCK(unit);
        goto intf_cleanup;
    }
    
    if_updated.if_array_del = sal_alloc(intf_num_cur * sizeof(bcm_if_t),
                                        "IPMC intf array del");
    if (if_updated.if_array_del == NULL) {
        REPL_UNLOCK(unit);
        rv = BCM_E_MEMORY;
        goto intf_cleanup;
    }

    /* Get the new if array for <group, port> and 
     * the if array deleted from <group, port>.
     */
    for (if_index = 0; if_index < if_count; if_index++) {
        for (if_cur_index = 0; if_cur_index < intf_num_cur; if_cur_index++) {
            if (if_array[if_index] == if_array_cur[if_cur_index]) {
                /* The if_array[if_index] has been already
                 * attached to <group, port>. 
                 */ 
                if_array_cur[if_cur_index] = -1;
                break;
            }
        }
        
        if (if_cur_index == intf_num_cur) {
            /* The if_array[if_index] is new for <group, port>. */
            if_updated.if_array_new[if_updated.if_count_new++] = 
                if_array[if_index];
        }        
    }

    for (if_cur_index = 0; if_cur_index < intf_num_cur; if_cur_index++) {
        if (if_array_cur[if_cur_index] == -1) {
            continue;
        }
        if_updated.if_array_del[if_updated.if_count_del++] = 
            if_array_cur[if_cur_index];
    }

    if ((if_updated.if_count_new == 0) && (if_updated.if_count_del == 0)) {
        REPL_UNLOCK(unit);
        rv = BCM_E_NONE;
        goto intf_cleanup;
    }

    rv = _bcm_tr3_ipmc_egress_intf_set(unit, repl_group, port,
                                       if_count,if_array, &if_updated,
                                       is_l3, check_port, repl_start_ptr);
    REPL_UNLOCK(unit);
intf_cleanup:
    if (if_array_cur) {
        sal_free(if_array_cur);
    }
    if (if_updated.if_array_new) {
        sal_free(if_updated.if_array_new);
    }
    if (if_updated.if_array_del) {
        sal_free(if_updated.if_array_del);
    }
    return rv;
}


/*
 * Function:
 *      bcm_tr3_ipmc_egress_intf_add
 * Purpose:
 *      Add L3 interface to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to add.
 *      l3_intf  - L3 interface to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    pbmp_t pbmp, ubmp;
    bcm_if_t intf_add;
    _tr3_if_updated_t if_updated;
    int repl_start_ptr = 0;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, l3_intf->l3a_vid, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = _bcm_tr3_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                       if_array, &intf_num, &repl_start_ptr);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = l3_intf->l3a_intf_id;
            intf_add = l3_intf->l3a_intf_id;
            if_updated.if_array_new = &intf_add;
            if_updated.if_count_new = 1;            
            rv = _bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id,
                                               port,intf_num,if_array,
                                               &if_updated, TRUE, FALSE,
                                               repl_start_ptr);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_ipmc_egress_intf_delete
 * Purpose:
 *      Remove L3 interface from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to remove.
 *      l3_intf  - L3 interface to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;
    _tr3_if_updated_t if_updated;
    bcm_if_t intf_del;
    int repl_start_ptr;
    
    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    /* Replication is allowed on CPU ports, only on TD2 */
    if(IS_CPU_PORT(unit, port)) {
        if(!SOC_IS_TRIDENT2X(unit)) {
            return BCM_E_PARAM;
        }
    }
    else
    {   
        REPL_PORT_CHECK(unit, port);
    }

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = _bcm_tr3_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                       if_array, &if_count, &repl_start_ptr);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf->l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            intf_del = l3_intf->l3a_intf_id;
            if_updated.if_count_del = 1;
            if_updated.if_array_del = &intf_del;
            rv = _bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id, port,
                                               if_count, if_array,
                                               &if_updated,TRUE, FALSE,
                                               repl_start_ptr);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ipmc_repl_get
 * Purpose:
 *      Return set of VLANs selected for port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to list.
 *      vlan_vec - (OUT) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_get(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_vector_t vlan_vec)
{
    int rv = BCM_E_NONE;
    uint32 ls_bits[2];
    int prev_repl_entry_ptr, repl_entry_ptr;
    int intf_base;
    int ls_pos;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int next_hop_index;
    int entry_type;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t egress_object;
    bcm_if_t egr_if;
    int vlan_count;
    egr_l3_next_hop_entry_t egr_nh;
#ifdef BCM_KATANA2_SUPPORT
    uint32 nh_offset = 0;
    uint32 ls_len = 0;
#endif /* BCM_KATANA2_SUPPORT*/

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    BCM_VLAN_VEC_ZERO(vlan_vec);

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit))
    {
        ls_len  = soc_mem_field_length(unit, MMU_REPL_LIST_TBLm, LSB_VLAN_BMf);
        /* coverity[large_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_REPL_LIST_TBLm,
                                                MSB_VLANf) - 1)) * ls_len;
    }
#endif

    REPL_LOCK(unit);
    if (REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id) == 0) {
        REPL_UNLOCK(unit);
        return BCM_E_NONE;
    }

    rv = _bcm_tr3_repl_list_start_ptr_get(unit, ipmc_id, port,
            &repl_entry_ptr);
    if (BCM_FAILURE(rv)) {
        goto vlan_get_done;
    }

    prev_repl_entry_ptr = -1;
    vlan_count = 0;
    while (repl_entry_ptr != prev_repl_entry_ptr) {
        rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                repl_entry_ptr, &repl_list_entry);
        if (BCM_FAILURE(rv)) {
            goto vlan_get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                MSB_VLANf) * 64;
        soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                LSB_VLAN_BMf, ls_bits);
        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    bcm_l3_intf_t_init(&l3_intf);
                    if (intf_base >= nh_offset) {
                        /* Next hop */
                        next_hop_index = intf_base - nh_offset + ls_pos +
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                        /* Coverity: egress_object.intf is initialized above
                         * by bcm_esw_l3_egress_get. */
                        /* coverity[uninit_use : FALSE] */
                        rv = bcm_esw_l3_egress_get(unit, next_hop_index, 
                                                   &egress_object);
                        if (BCM_FAILURE(rv)) {
                            goto vlan_get_done;
                        }
                        l3_intf.l3a_intf_id = egress_object.intf;

                    } else {
                        /* L3 Interface */
                        l3_intf.l3a_intf_id  = intf_base + ls_pos;
                    }                    
                } else 
#endif
                {
                    next_hop_index = intf_base + ls_pos;
                    rv = READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                          next_hop_index, &egr_nh);
                    if (BCM_FAILURE(rv)) {
                        goto vlan_get_done;
                    }
                    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                          &egr_nh, ENTRY_TYPEf);
                    bcm_l3_intf_t_init(&l3_intf);
                    if (entry_type == 7) {
                        l3_intf.l3a_intf_id = 
                        soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                               &egr_nh, L3MC__INTF_NUMf);
                    } else {
                        egr_if = next_hop_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                        rv = bcm_esw_l3_egress_get(unit, 
                               egr_if, &egress_object);
                        if (BCM_FAILURE(rv)) {
                            goto vlan_get_done;
                        }
                        l3_intf.l3a_intf_id = egress_object.intf;
                    }
                } /* END of if not KATANA2 */
                rv = bcm_esw_l3_intf_get(unit, &l3_intf);
                if (BCM_FAILURE(rv)) {
                    goto vlan_get_done;
                }
                BCM_VLAN_VEC_SET(vlan_vec, l3_intf.l3a_vid);
                vlan_count++;
               
            }
        }
        prev_repl_entry_ptr = repl_entry_ptr;
        repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                &repl_list_entry, NEXTPTRf);
        if (vlan_count >= REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

vlan_get_done:
    REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ipmc_repl_add
 * Purpose:
 *      Add VLAN to selected ports' replication list for chosen
 *      IPMC group.
 *Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to add.
 *      vlan     - VLAN to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_add(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;
    bcm_if_t intf_add;
    _tr3_if_updated_t if_updated;
    int repl_start_ptr = 0;
    
    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }

    REPL_LOCK(unit);

    intf_max = 1 + REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    rv = _bcm_tr3_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                       if_array, &if_count, &repl_start_ptr);
    if (BCM_SUCCESS(rv)) {
        if (if_count < intf_max) {
            if_array[if_count++] = l3_intf.l3a_intf_id;
            intf_add = l3_intf.l3a_intf_id;
            if_updated.if_array_new = &intf_add;
            if_updated.if_count_new = 1;
            rv = _bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id, port,
                                               if_count, if_array,
                                               &if_updated,TRUE, FALSE,
                                               repl_start_ptr);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ipmc_repl_delete
 * Purpose:
 *      Remove VLAN from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to remove.
 *      vlan     - VLAN to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr3_ipmc_repl_delete(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t l3_intf;
    bcm_if_t intf_del;
    _tr3_if_updated_t if_updated;
    int repl_start_ptr = 0;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }

    REPL_LOCK(unit);
    intf_max = REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        REPL_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    rv = _bcm_tr3_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                       if_array, &if_count, &repl_start_ptr);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf.l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            intf_del = l3_intf.l3a_intf_id;
            if_updated.if_count_del = 1;
            if_updated.if_array_del = &intf_del;
            rv = _bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id, port,
                                               if_count, if_array,
                                               &if_updated, TRUE, FALSE,
                                               repl_start_ptr);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }
    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_delete_all
 * Purpose:
 *      Remove all VLANs from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The MC index number.
 *      port     - port from which to remove VLANs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_delete_all(int unit, int ipmc_id, bcm_port_t port)
{
    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    return bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id, port, 0, NULL,
                                        TRUE, FALSE);
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_set
 * Purpose:
 *      Assign set of VLANs provided to port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to list.
 *      vlan_vec - (IN) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_vector_t vlan_vec)
{
    int rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t l3_intf;
    pbmp_t pbmp, ubmp;
    int  intf_num, intf_max, alloc_size, vid;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = BCM_VLAN_MAX - BCM_VLAN_MIN + 1;
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(if_array, 0, alloc_size);
    intf_num = 0;
    for (vid = BCM_VLAN_MIN; vid < BCM_VLAN_MAX; vid++) {
        if (BCM_VLAN_VEC_GET(vlan_vec, vid)) {
            rv = bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp);
            if (BCM_FAILURE(rv)) {
                sal_free(if_array);
                return rv;
            }
            if (!BCM_PBMP_MEMBER(pbmp, port)) {
                sal_free(if_array);
                return BCM_E_PARAM;
            }
            bcm_l3_intf_t_init(&l3_intf);
            l3_intf.l3a_vid = vid;
            if ((rv = bcm_esw_l3_intf_find_vlan(unit, &l3_intf)) < 0) {
                sal_free(if_array);
                return rv;
            }
            if_array[intf_num++] = l3_intf.l3a_intf_id;
        }
    }

    rv = bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id, port,
                                     intf_num, if_array, TRUE, FALSE);

    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ipmc_egress_intf_add
 * Purpose:
 *      Add encap ID to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to add.
 *      encap_id - Encap ID.
 *      is_l3    - Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            int encap_id, int is_l3)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;
    _tr3_if_updated_t if_updated;
    bcm_if_t intf_add;
    bcm_port_t port_in = port;
    int repl_start_ptr = 0;

    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &port));
    }
#endif

    /* Replication is allowed on CPU ports, only on TD2 */
    if(IS_CPU_PORT(unit, port)) {
        if(!SOC_IS_TRIDENT2X(unit)) {
            return BCM_E_PARAM;
        }
    }
    else
    {   
        REPL_PORT_CHECK(unit, port);
    }

#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) &&
        encap_id == BCM_ENCAP_TRUNK_MEMBER) {
        /* For TH_MC_PER_TRUNK_REPL mode, indentical replication list
         * is set for all the members of a trunk.
         */
        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    intf_max = REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = _bcm_tr3_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                       if_array, &intf_num, &repl_start_ptr);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = encap_id;

            /* For IPMC and Trill, check port is a member of the L3 interface's 
             * VLAN. Performing this check here is more efficient than doing 
             * it in bcm_tr3_ipmc_egress_intf_set.
             */ 
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id)) {
                /* L3 interface is used */

                if (encap_id > soc_mem_index_max(unit, EGR_L3_INTFm)) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }

                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = encap_id;
                rv = bcm_esw_l3_intf_get(unit, &l3_intf);
                if (BCM_FAILURE(rv)) {
                    goto intf_add_done;
                }
            }
            
            sal_memset(&if_updated, 0, sizeof(if_updated));
            intf_add = encap_id;
            if_updated.if_array_new = &intf_add;
            if_updated.if_count_new = 1;
            rv = _bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id,
                                               port_in, intf_num, if_array,
                                               &if_updated, is_l3, FALSE,
                                               repl_start_ptr);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

intf_add_done:

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ipmc_egress_intf_delete
 * Purpose:
 *      Remove encap ID from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - IPMC group number.
 *      port     - port to remove.
 *      if_max   - Maximal interface.
 *      encap_id - Encap ID to delete from replication.
 *      is_l3    - Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               int if_max, int encap_id, int is_l3)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, if_count, if_cur, match, rv = BCM_E_NONE;
    _tr3_if_updated_t if_updated;
    bcm_if_t intf_del;
    int repl_start_ptr = 0;
    
    REPL_INIT(unit);
    REPL_GROUP_ID(unit, ipmc_id);

    /* Replication is allowed on CPU ports, only on TD2 */
    if(IS_CPU_PORT(unit, port)) {
        if(!SOC_IS_TRIDENT2X(unit)) {
            return BCM_E_PARAM;
        }
    }
    else
    {   
        REPL_PORT_CHECK(unit, port);
    }

#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    if ((BCM_MC_PER_TRUNK_REPL_MODE(unit)) &&
        encap_id == BCM_ENCAP_TRUNK_MEMBER) {
        /* For MC_PER_TRUNK_REPL mode, indentical replication list
         * is set for all the members of a trunk.
         */
        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    if (!REPL_PORT_GROUP_INTF_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    if ((if_max <= 0) || ((uint32)if_max > REPL_INTF_TOTAL(unit))) {
        return BCM_E_PARAM;
    }

    alloc_size = if_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    REPL_LOCK(unit);
    rv = _bcm_tr3_ipmc_egress_intf_get(unit, ipmc_id, port, if_max,
                                       if_array, &if_count, &repl_start_ptr);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == encap_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            sal_memset(&if_updated, 0, sizeof(if_updated));
            intf_del = encap_id;
            if_updated.if_count_del = 1;
            if_updated.if_array_del = &intf_del;
            rv = _bcm_tr3_ipmc_egress_intf_set(unit, ipmc_id, port, 
                                               if_count, if_array,
                                               &if_updated, is_l3, FALSE,
                                               repl_start_ptr);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

    REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_ipmc_trill_mac_update
 * Purpose:
 *      Update the Trill MAC address in next hop entries that are
 *      in Trill multicast replication list. 
 * Parameters:
 *      unit      - (IN) SOC unit #
 *      mac_field - (IN) MAC address
 *      flag      - (IN) 0 = Set the lower 24 bits of MAC address,
 *                       1 = Set the higher 24 bits of MAC address.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_trill_mac_update(int unit, uint32 mac_field, uint8 flag)
{
    int intf, nh_index;
    egr_l3_next_hop_entry_t egr_nh;
    uint32 entry_type;
    bcm_mac_t mac;

    for (intf = 0; intf < soc_mem_index_count(unit, EGR_L3_INTFm); intf++) {
        nh_index = REPL_L3_INTF_NEXT_HOP_TRILL(unit, intf);
        if (nh_index >= 0) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ANY, nh_index, &egr_nh));
            entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                    &egr_nh, ENTRY_TYPEf);
            if (entry_type == 0) { /* normal entry type */
                soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3__MAC_ADDRESSf, mac);
                if (!flag) {
                    /* Set the low-order bytes */
                    mac[3] = (uint8) (mac_field >> 16 & 0xff);
                    mac[4] = (uint8) (mac_field >> 8 & 0xff);
                    mac[5] = (uint8) (mac_field & 0xff);
                } else {
                    /* Set the High-order bytes */
                    mac[0] = (uint8) (mac_field >> 16 & 0xff);
                    mac[1] = (uint8) (mac_field >> 8 & 0xff);
                    mac[2] = (uint8) (mac_field & 0xff);
                }
                soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3__MAC_ADDRESSf, mac);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_l3_intf_next_hop_free
 * Purpose:
 *      Free the next hop index associated with the given L3 interface.
 * Parameters:
 *      unit - (IN) SOC unit #
 *      intf - (IN) L3 interface ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_l3_intf_next_hop_free(int unit, int intf)
{
    int nh_index;

    if (_tr3_repl_info[unit] == NULL) {
        /* IPMC replication is not initialized. There is no next hop
         * index to free.
         */
        return BCM_E_NONE;
    }

    nh_index = REPL_L3_INTF_NEXT_HOP_TRILL(unit, intf);
    if (nh_index >= 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index,
                                    soc_mem_entry_null(unit, EGR_L3_NEXT_HOPm)));
        BCM_IF_ERROR_RETURN
            (bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index));
        REPL_L3_INTF_NEXT_HOP_TRILL(unit, intf) = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_l3_intf_next_hop_get
 * Purpose:
 *      Get the next hop index that was silently allocated to the L3 interface.
 * Parameters:
 *      unit - (IN) SOC unit #
 *      intf - (IN) L3 interface ID
 *      nh_index - (OUT) Next hop index
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_l3_intf_next_hop_get(int unit, int intf, int *nh_index)
{
    *nh_index = REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf);

    if (*nh_index == REPL_NH_INDEX_UNALLOCATED ||
        *nh_index == REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_l3_intf_next_hop_l3_egress_set
 * Purpose:
 *      Inform the IPMC module that a next hop index has been allocated
 *      for the given L3 interface by the bcm_l3_egress_create API.
 * Parameters:
 *      unit - (IN) SOC unit #
 *      intf - (IN) L3 interface ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_l3_intf_next_hop_l3_egress_set(int unit, int intf)
{
    int nh_index;

    nh_index = REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf);

    if (nh_index == REPL_NH_INDEX_UNALLOCATED) {
        REPL_L3_INTF_NEXT_HOP_IPMC(unit, intf) =
            REPL_NH_INDEX_L3_EGRESS_ALLOCATED;
    } else if (nh_index == REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
        /* Do nothing. Users are allowed to create multiple L3
         * egress objects based on the same L3 interface.
         */
    } else {
        /* IPMC module had already silently allocated a next hop index
         * for this L3 interface.
         */
        return BCM_E_CONFIG;
    }
        
    return BCM_E_NONE;
}
#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_scache_size_get
 * Purpose:
 *      Get the required scache size for storing a bitmap
 *      of L3 interfaces.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_tr3_ipmc_repl_l3_intf_scache_size_get(
    int unit, 
    uint32 *size)
{
    int num_l3_intf;

    *size = 0;
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size += SHR_BITALLOCSIZE(num_l3_intf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_sync
 * Purpose:
 *      Record L3 interface bitmap into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_tr3_ipmc_repl_l3_intf_sync(
    int unit, 
    uint8 **scache_ptr)
{
    int num_l3_intf, i;
    SHR_BITDCL *l3_intf_bitmap = NULL;

    REPL_INIT(unit);

    /* Create a bitmap of L3 interfaces */
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    l3_intf_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_l3_intf),
            "L3 interface bitmap");
    if (NULL == l3_intf_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(l3_intf_bitmap, 0, SHR_BITALLOCSIZE(num_l3_intf));

    /* For the L3 interfaces that were allocated next hop indices by
     * bcm_l3_egress_create API, set the bit in the bitmap.
     */
    for (i = 0; i < num_l3_intf; i++) {
        if (REPL_L3_INTF_NEXT_HOP_IPMC(unit, i) ==
                REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
            SHR_BITSET(l3_intf_bitmap, i);
        }
    }

    /* Store the bitmap into scache */
    sal_memcpy((*scache_ptr), l3_intf_bitmap, SHR_BITALLOCSIZE(num_l3_intf));
    (*scache_ptr) += SHR_BITALLOCSIZE(num_l3_intf);

    sal_free(l3_intf_bitmap);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_scache_recover
 * Purpose:
 *      Recover L3 interface bitmap from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_l3_intf_scache_recover(int unit, uint8 **scache_ptr)
{
    int num_l3_intf, i;
    SHR_BITDCL *l3_intf_bitmap = NULL;

    /* Recover L3 interface bitmap */
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    l3_intf_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_l3_intf),
            "L3 interface bitmap");
    if (NULL == l3_intf_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memcpy(l3_intf_bitmap, (*scache_ptr), SHR_BITALLOCSIZE(num_l3_intf));
    (*scache_ptr) += SHR_BITALLOCSIZE(num_l3_intf);

    for (i = 0; i < num_l3_intf; i++) {
        if (SHR_BITGET(l3_intf_bitmap, i)) {
            REPL_L3_INTF_NEXT_HOP_IPMC(unit, i) =
                REPL_NH_INDEX_L3_EGRESS_ALLOCATED;
        }
    }

    sal_free(l3_intf_bitmap);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_size_get
 * Purpose:
 *      Get the required scache size for storing map from L3 interfaces
 *      to SDK managed next hop.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_size_get(
    int unit,
    uint32 *size)
{
    int num_l3_intf;

    *size = 0;
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size += (num_l3_intf * sizeof(int));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_nh_map_sync
 * Purpose:
 *      Record L3 interface map to SDK managed nexthop into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_ipmc_repl_l3_intf_nh_map_sync(
    int unit,
    uint8 **scache_ptr)
{
    int num_l3_intf, i;

    REPL_INIT(unit);

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        *(int *) (*scache_ptr) = REPL_L3_INTF_NEXT_HOP_IPMC(unit, i);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_recover
 * Purpose:
 *      Recover L3 interface nexthop map from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_recover(int unit, uint8 **scache_ptr)
{
    int  num_l3_intf, i;

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        REPL_L3_INTF_NEXT_HOP_IPMC(unit, i) = *(int *)(*scache_ptr);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_size_get
 * Purpose:
 *      Get the required scache size for storing map from L3 interfaces
 *      to SDK managed trill next hop.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(
    int unit,
    uint32 *size)
{
    int num_l3_intf;

    *size = 0;
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size += (num_l3_intf * sizeof(int));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_sync
 * Purpose:
 *      Record L3 interface map to SDK managed trill nexthop into the scache.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_sync(
    int unit,
    uint8 **scache_ptr)
{
    int num_l3_intf, i;

    REPL_INIT(unit);

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        *(int *) (*scache_ptr) = REPL_L3_INTF_NEXT_HOP_TRILL(unit, i);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_recover
 * Purpose:
 *      Recover L3 interface trill nexthop map from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_recover(int unit, uint8 **scache_ptr)
{
    int  num_l3_intf, i;

    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < num_l3_intf; i++) {
        REPL_L3_INTF_NEXT_HOP_TRILL(unit, i) = *(int *)(*scache_ptr);
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}



#ifdef BCM_TRIDENT2PLUS_SUPPORT
STATIC int
bcm_td2p_set_port_intf_cnt(int unit, int pipe, int aggid,
                           int repl_group, int intf_cnt)
{
    int port_iter, aggid_iter, pipe_iter;
    uint32 regval;
    bcm_pbmp_t            pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

    PBMP_ITER(pbmp_all, port_iter) {
        if (IS_RDB_PORT(unit, port_iter)) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
            READ_MMU_TOQ_REPL_PORT_AGG_MAPr(unit, port_iter, &regval));
        aggid_iter = soc_reg_field_get(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, regval,
                                       L3MC_PORT_AGG_IDf);
        /* coverity[overrun-local:FALSE] */
        pipe_iter = SOC_INFO(unit).port_pipe[port_iter];
        if ((aggid_iter == aggid) && (pipe_iter == pipe)) {
            /* coverity[overrun-local:FALSE] */
            REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group) = intf_cnt;
        }
    }

    return BCM_E_NONE;
}

STATIC int
bcm_td2p_add_port_intf_cnt(int unit, int pipe, int aggid,
                           int repl_group, int intf_cnt)
{
    int port_iter, aggid_iter, pipe_iter;
    uint32 regval;
    bcm_pbmp_t            pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

    PBMP_ITER(pbmp_all, port_iter) {
        if (IS_RDB_PORT(unit, port_iter)) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
            READ_MMU_TOQ_REPL_PORT_AGG_MAPr(unit, port_iter, &regval));
        aggid_iter = soc_reg_field_get(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, regval,
                                       L3MC_PORT_AGG_IDf);
        /* coverity[overrun-local:FALSE] */
        pipe_iter = SOC_INFO(unit).port_pipe[port_iter];
        if ((aggid_iter == aggid) && (pipe_iter == pipe)) {
            /* coverity[overrun-local:FALSE] */
            REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group) += intf_cnt;
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

/*
 * Function:
 *      _bcm_tr3_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize replication software to state consistent with
 *      hardware.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_ipmc_repl_reload(int unit)
{
    uint8 *repl_group_buf = NULL;
    uint8 *repl_group_buf2 = NULL;
    int index_min, index_max;
    int rv = BCM_E_NONE;
    int intf_vec_alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    int i, j;
    uint32 *repl_group_entry;
    int head_index;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int member_count;
    soc_info_t *si;
    int member_id, member;
    bcm_port_t mmu_port, phy_port, port = -1;
    mmu_repl_head_tbl_entry_t repl_head_entry;
    int start_ptr;
    _bcm_repl_list_info_t *rli_current;
    int prev_repl_entry_ptr, repl_entry_ptr;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int msb;
    uint32 ls_bits[2];
    uint8 flags;
    soc_pbmp_t member_bitmap;
    soc_mem_t repl_group_table;
    soc_field_t base_idx_f;
    soc_field_t member_bitmap_f;
#ifdef BCM_TRIDENT2_SUPPORT
    soc_mem_t repl_group_table2 = MMU_REPL_GROUP_INFO1m;
    int member_bitmap2_offset = 0;
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    int pipe = -1, aggid = -1, intf_cnt = 0;
#endif


    /* Initialize internal data structures */
    BCM_IF_ERROR_RETURN(bcm_tr3_ipmc_repl_init(unit));

    /* Recover internal state by traversing replication lists */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        repl_group_table = MMU_REPL_GROUP_INFO0m;
        base_idx_f = PIPE_BASE_PTRf;
        member_bitmap_f = PIPE_MEMBER_BMPf;

    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_split_repl_group_table)) {
        repl_group_table = MMU_REPL_GROUP_INFO0m;
        repl_group_table2 = MMU_REPL_GROUP_INFO1m;
        base_idx_f = PIPE_BASE_PTRf;
        member_bitmap_f = PIPE_MEMBER_BMPf;
        member_bitmap2_offset = soc_mem_field_length(unit,
                repl_group_table, member_bitmap_f);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        repl_group_table = MMU_REPL_GROUPm;
        base_idx_f = HEAD_INDEXf;
        member_bitmap_f = MEMBER_BITMAPf;
    }

    /* Read replication group table */
    repl_group_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, repl_group_table), "repl group buf");
    if (NULL == repl_group_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, repl_group_table);
    index_max = soc_mem_index_max(unit, repl_group_table);
    rv = soc_mem_read_range(unit, repl_group_table, MEM_BLOCK_ANY,
            index_min, index_max, repl_group_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }


#ifdef BCM_TRIDENT2_SUPPORT
    /* Read replication group table of Y-pipe */
    else if (soc_feature(unit, soc_feature_split_repl_group_table)) {
        repl_group_buf2 = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, repl_group_table2), "repl group buf2");
        if (NULL == repl_group_buf2) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, repl_group_table2);
        index_max = soc_mem_index_max(unit, repl_group_table2);
        rv = soc_mem_read_range(unit, repl_group_table2, MEM_BLOCK_ANY,
                index_min, index_max, repl_group_buf2);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    intf_vec_alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(intf_vec_alloc_size, "Repl interface vector");
    if (intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(intf_vec, 0, intf_vec_alloc_size);

    for (i = index_min; i <= index_max; i++) {
        repl_group_entry = soc_mem_table_idx_to_pointer(unit,
                repl_group_table, uint32 *, repl_group_buf, i);

        /* Get the head index */
        head_index = soc_mem_field32_get(unit, repl_group_table,
                    repl_group_entry, base_idx_f);
        if (0 == head_index &&
                !soc_feature(unit, soc_feature_static_repl_head_alloc)) {
            continue; /* with next replication group */
        }

        /* Get the member bitmap */
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_mem_field_get(unit, repl_group_table, repl_group_entry,
                    member_bitmap_f, fldbuf);
        for (j = 0; j < SOC_PBMP_WORD_MAX; j++) {
            SOC_PBMP_WORD_SET(member_bitmap, j, fldbuf[j]);
        }
#ifdef BCM_TRIDENT2_SUPPORT
        /* Get the member bitmap of Y-pipe */
        if (soc_feature(unit, soc_feature_split_repl_group_table)) {
            uint32 *repl_group_entry2;
            soc_pbmp_t member_bitmap2;

            if (NULL != repl_group_buf2) {
                repl_group_entry2 = soc_mem_table_idx_to_pointer(unit,
                        repl_group_table2, uint32 *, repl_group_buf2, i);
                soc_mem_field_get(unit, repl_group_table2, repl_group_entry2,
                        member_bitmap_f, fldbuf);
                for (j = 0; j < SOC_PBMP_WORD_MAX; j++) {
                    SOC_PBMP_WORD_SET(member_bitmap2, j, fldbuf[j]);
                }
                SOC_PBMP_ITER(member_bitmap2, member) {
                    SOC_PBMP_PORT_ADD(member_bitmap,
                            (member + member_bitmap2_offset));
                }
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */
        SOC_PBMP_COUNT(member_bitmap, member_count);
        if (0 == member_count) {
            continue; /* with next replication group */
        }

        member_id = 0;
        SOC_PBMP_ITER(member_bitmap, member) {
            /* Convert member to mmu port, then to physical port, and
             * finally to logical port */
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_split_repl_group_table)) {
                if (member_bitmap2_offset > 0 && member >= member_bitmap2_offset) {
                    mmu_port = member - member_bitmap2_offset + 64;
                } else {
                    mmu_port = member;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                /* In Triumph3, mmu port 60 is mapped to member 59 */
                if (59 == member && SOC_IS_TRIUMPH3(unit)) {
                    mmu_port = 60;
                } else {
                    mmu_port = member;
                }
            }
            /* Add code to check MMU port is less than SOC_MAX_NUM_PORTS,
             * in order to prevent false Coverity warning that the
             * si->port_m2p_mapping array may be overrun.
             */
            if (mmu_port >= SOC_MAX_NUM_PORTS) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                if (mmu_port >= 64) {
                    aggid = mmu_port - 64;
                    pipe = 1;
                } else {
                    aggid = mmu_port;
                    pipe = 0;
                }
            } else
#endif
            {
                si = &SOC_INFO(unit);
                phy_port = si->port_m2p_mapping[mmu_port];
                port = si->port_p2l_mapping[phy_port];
           }

            /* Get replication list start pointer */
            rv = READ_MMU_REPL_HEAD_TBLm(unit, MEM_BLOCK_ANY,
                    head_index + member_id, &repl_head_entry);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            member_id++;
            start_ptr = soc_MMU_REPL_HEAD_TBLm_field32_get(unit,
                    &repl_head_entry, HEAD_PTRf);
            if (0 == start_ptr) {
                continue; /* with next member */
            }

            if (_bcm_tr3_repl_list_entry_used_get(unit, start_ptr)) {
                /* Already traversed this replication list */
                for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
                        rli_current = rli_current->next) {
                    if (rli_current->index == start_ptr) {
                        (rli_current->refcount)++;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                        if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                            intf_cnt = rli_current->list_size;
                            bcm_td2p_set_port_intf_cnt(unit, pipe, aggid,
                                                       i, intf_cnt);
                        } else
#endif
                        {
                            REPL_PORT_GROUP_INTF_COUNT(unit, port, i) =
                                rli_current->list_size;
                        }
                        break;
                    }
                }
                if (rli_current == NULL) {
                    /* Unexpected */
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                } else {
                    continue; /* with next member */
                }
            }

            /* Traverse the replication list table */
            sal_memset(intf_vec, 0, intf_vec_alloc_size);
            prev_repl_entry_ptr = -1;
            repl_entry_ptr = start_ptr;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                intf_cnt = 0;
            }
#endif
            while (repl_entry_ptr != prev_repl_entry_ptr) {
                rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                        repl_entry_ptr, &repl_list_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, MSB_VLANf);
                soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                        LSB_VLAN_BMf, ls_bits);
                intf_vec[2 * msb + 0] = ls_bits[0];
                intf_vec[2 * msb + 1] = ls_bits[1];
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                    intf_cnt += (_shr_popcount(ls_bits[0]) +
                                 _shr_popcount(ls_bits[1]));
                } else
#endif
                {
                    REPL_PORT_GROUP_INTF_COUNT(unit, port, i) +=
                        _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                }
                rv = _bcm_tr3_repl_list_entry_used_set(unit, repl_entry_ptr);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                prev_repl_entry_ptr = repl_entry_ptr;
                repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, NEXTPTRf);
            }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                bcm_td2p_add_port_intf_cnt(unit, pipe, aggid, i, intf_cnt);
            }
#endif

            /* Insert a new replication list into linked list */
            rli_current = sal_alloc(sizeof(_bcm_repl_list_info_t),
                    "repl list info");
            if (rli_current == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            sal_memset(rli_current, 0, sizeof(_bcm_repl_list_info_t));
            rli_current->index = start_ptr;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
                if ((_shr_popcount(*(uint32 *)intf_vec) == intf_cnt) &&
                    (REPL_INTF_TOTAL(unit) >= SHR_BITWID)) {
                    rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec, SHR_BITWID);
                } else {
                    rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec,
                        REPL_INTF_TOTAL(unit));
                }
                rli_current->list_size = intf_cnt;
            } else
#endif
            {
                if ((_shr_popcount(*(uint32 *)intf_vec) ==
                         REPL_PORT_GROUP_INTF_COUNT(unit, port, i)) &&
                    (REPL_INTF_TOTAL(unit) >= SHR_BITWID)) {
                    rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec, SHR_BITWID);
                } else {
                    rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec,
                        REPL_INTF_TOTAL(unit));
                }
                rli_current->list_size = REPL_PORT_GROUP_INTF_COUNT(unit, port, i);
            }
            (rli_current->refcount)++;
            rli_current->next = REPL_LIST_INFO(unit);
            REPL_LIST_INFO(unit) = rli_current;
        }

        /* Update REPL_HEAD table usage */
        if (!soc_feature(unit, soc_feature_static_repl_head_alloc)) {
            rv = _bcm_tr3_repl_head_block_used_set(unit, head_index, member_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }
    soc_cm_sfree(unit, repl_group_buf);
    repl_group_buf = NULL;
    if (repl_group_buf2) {
        soc_cm_sfree(unit, repl_group_buf2);
        repl_group_buf2 = NULL;
    }
    sal_free(intf_vec);
    intf_vec = NULL;

    /* Recover REPL list mode from HW cache */
    rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                         _BCM_IPMC_WB_REPL_LIST, &flags);
    if (flags) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

cleanup:
    if (repl_group_buf) {
        soc_cm_sfree(unit, repl_group_buf);
    }
    if (repl_group_buf2) {
        soc_cm_sfree(unit, repl_group_buf2);
    }
    if (intf_vec) {
        sal_free(intf_vec);
    }

    if (BCM_FAILURE(rv)) {
        bcm_tr3_ipmc_repl_detach(unit);
    }

    return rv;
}
#ifdef BCM_SABER2_SUPPORT
/*
 * Function:
 *      _bcm_sb2_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize replication software to state consistent with
 *      hardware.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_ipmc_repl_reload(int unit)
{
    uint8 *repl_group_buf0 = NULL;
    uint8 *repl_group_base_ptr_buf = NULL;
    int index_min, index_max;
    int rv = BCM_E_NONE;
    int intf_vec_alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    int i, j;
    uint32 *repl_group_entry0;
    uint32 *repl_group_base_ptr_entry;
    int head_index;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int member_count;
    int member_id, member;
    bcm_port_t mmu_port, phy_port, port;
    mmu_repl_head_tbl_entry_t repl_head_entry;
    int start_ptr;
    _bcm_repl_list_info_t *rli_current;
    int prev_repl_entry_ptr, repl_entry_ptr;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int msb;
    uint32 ls_bits[2];
    uint8 flags;
    soc_pbmp_t member_bitmap;
    soc_mem_t repl_group_table0 = INVALIDm;
    soc_mem_t repl_group_base_ptr_table = INVALIDm;
    soc_field_t base_idx_f;
    uint32 nh_offset , is_len, offset;
    soc_field_t mbmp;
    int intf_base;

    /* Initialize internal data structures */
    BCM_IF_ERROR_RETURN(bcm_tr3_ipmc_repl_init(unit));

    is_len = soc_mem_field_length(unit, MMU_REPL_LIST_TBLm, LSB_VLAN_BMf);
    /* coverity[large_shift : FALSE] */
    nh_offset = (1 << (soc_mem_field_length(unit, MMU_REPL_LIST_TBLm,
                                                MSB_VLANf) - 1)) * is_len;
    /* Recover internal state by traversing replication lists */
#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        repl_group_table0 = MMU_REPL_GRP_TBLm;
        repl_group_base_ptr_table = MMU_REPL_GRP_TBLm;
    } else
#endif
    {
        repl_group_table0 = MMU_REPL_GRP_TBL0m;
        repl_group_base_ptr_table = MMU_REPL_GRP_TBL1m;
    }

    base_idx_f = BASE_PTRf;
    mbmp = MEMBER_BMP_PORTSf;

    /* Read replication group table MMU_REPL_GRP_TBL0 */
    repl_group_buf0 = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, repl_group_table0), "repl group buf");

    if (NULL == repl_group_buf0) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }


    repl_group_base_ptr_buf = soc_cm_salloc(unit,
          SOC_MEM_TABLE_BYTES(unit, repl_group_base_ptr_table), 
                              "repl group base ptr buf");

    if (NULL == repl_group_base_ptr_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, repl_group_table0);
    index_max = soc_mem_index_max(unit, repl_group_table0);
    rv = soc_mem_read_range(unit, repl_group_table0, MEM_BLOCK_ANY,
            index_min, index_max, repl_group_buf0);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }


    index_min = soc_mem_index_min(unit, repl_group_base_ptr_table);
    index_max = soc_mem_index_max(unit, repl_group_base_ptr_table);
    rv = soc_mem_read_range(unit, repl_group_base_ptr_table, MEM_BLOCK_ANY,
                index_min, index_max, repl_group_base_ptr_buf); 
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    intf_vec_alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(intf_vec_alloc_size * sizeof(SHR_BITDCL),
                         "Repl interface vector");
    if (intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(intf_vec, 0, intf_vec_alloc_size);

    for (i = index_min; i <= index_max; i++) {
        repl_group_entry0 = soc_mem_table_idx_to_pointer(unit,
                repl_group_table0, uint32 *, repl_group_buf0, i);

        /* Get the head index */
        repl_group_base_ptr_entry = soc_mem_table_idx_to_pointer(unit,
                    repl_group_base_ptr_table, uint32 *,
                    repl_group_base_ptr_buf, i);

        head_index = soc_mem_field32_get(unit, repl_group_base_ptr_table,
                    repl_group_base_ptr_entry, base_idx_f);

        /* Get the member bitmap */
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_mem_field_get(unit, repl_group_table0, repl_group_entry0,
                    mbmp, fldbuf);
         

        for (j = 0; j < SOC_PBMP_WORD_MAX; j++) {
            SOC_PBMP_WORD_SET(member_bitmap, j, fldbuf[j]);
        }

        SOC_PBMP_COUNT(member_bitmap, member_count);

        if (0 == member_count) {
            continue; /* with next replication group */
        }

        member_id = 0;
        SOC_PBMP_ITER(member_bitmap, member) {
            /* Convert member to mmu port, then to physical port, and
             * finally to logical port */

            mmu_port = member;
            phy_port = mmu_port;
            port = phy_port;

            /* Get replication list start pointer */
            rv = READ_MMU_REPL_HEAD_TBLm(unit, MEM_BLOCK_ANY,
                    head_index + member_id, &repl_head_entry);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            member_id++;

            start_ptr = soc_MMU_REPL_HEAD_TBLm_field32_get(unit,
                    &repl_head_entry, HEAD_PTRf);
            if (0 == start_ptr) {
                continue; /* with next member */
            }

            if (_bcm_tr3_repl_list_entry_used_get(unit, start_ptr)) {
                /* Already traversed this replication list */
                for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
                        rli_current = rli_current->next) {
                    if (rli_current->index == start_ptr) {
                        (rli_current->refcount)++;
                        REPL_PORT_GROUP_INTF_COUNT(unit, port, i) =
                            rli_current->list_size;
                        break;
                    }
                }
                if (rli_current == NULL) {
                    /* Unexpected */
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                } else {
                    continue; /* with next member */
                }
            }

            /* Traverse the replication list table */
            sal_memset(intf_vec, 0, intf_vec_alloc_size);
            prev_repl_entry_ptr = -1;
            repl_entry_ptr = start_ptr;
            while (repl_entry_ptr != prev_repl_entry_ptr) {
                rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                        repl_entry_ptr, &repl_list_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
       
                msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, MSB_VLANf);
                soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                        LSB_VLAN_BMf, ls_bits);

                intf_base = msb *64;

                if (intf_base >= nh_offset)
                {
                    /* replication on next hops  */
                    offset = _SHR_BITDCLSIZE(soc_mem_index_count(unit, 
                                             EGR_L3_INTFm)); 
                } else {
                    /* replication on L3 interface */
                    offset = 0;
                }
                intf_vec[offset + 2 * msb + 0] = ls_bits[0];
                intf_vec[offset + 2 * msb + 1] = ls_bits[1];

                REPL_PORT_GROUP_INTF_COUNT(unit, port, i) +=
                    _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                rv = _bcm_tr3_repl_list_entry_used_set(unit, repl_entry_ptr);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                prev_repl_entry_ptr = repl_entry_ptr;
                repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, NEXTPTRf);
            }

            /* Insert a new replication list into linked list */
            rli_current = sal_alloc(sizeof(_bcm_repl_list_info_t),
                    "repl list info");
            if (rli_current == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            sal_memset(rli_current, 0, sizeof(_bcm_repl_list_info_t));
            rli_current->index = start_ptr;
            rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec,
                    REPL_INTF_TOTAL(unit));
            rli_current->list_size = REPL_PORT_GROUP_INTF_COUNT(unit, port, i);
            (rli_current->refcount)++;
            rli_current->next = REPL_LIST_INFO(unit);
            REPL_LIST_INFO(unit) = rli_current;
        }
    }
    

    if (SOC_IS_SABER2(unit)) 
    { 
        for (i = soc_mem_index_min(unit, L3_IPMCm);
             i <= soc_mem_index_max(unit, L3_IPMCm); i++) {
             if (i > 1) {
                 rv = _bcm_sb2_extq_repl_reload(unit, i); 
                 if (rv < 0) 
                 {
                     goto cleanup; 
                 }
             }
       }
   } 

    soc_cm_sfree(unit, repl_group_buf0);
    repl_group_buf0 = NULL;
    
    soc_cm_sfree(unit, repl_group_base_ptr_buf);
    repl_group_base_ptr_buf = NULL;
    
    sal_free(intf_vec);
    intf_vec = NULL;

    /* Recover REPL list mode from HW cache */
    rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                         _BCM_IPMC_WB_REPL_LIST, &flags);
    if (flags) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

cleanup:
    if (repl_group_buf0) {
        soc_cm_sfree(unit, repl_group_buf0);
    }
    if (repl_group_base_ptr_buf) {
        soc_cm_sfree(unit, repl_group_base_ptr_buf);
    }
    if (intf_vec) {
        sal_free(intf_vec);
    }
    if (BCM_FAILURE(rv)) {
        bcm_tr3_ipmc_repl_detach(unit);
    }

    return rv;
}
#endif /* BCM_SABER2_SUPPORT*/


#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *      _bcm_kt2_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize replication software to state consistent with
 *      hardware.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_ipmc_repl_reload(int unit)
{
    uint8 *repl_group_buf0 = NULL;
    uint8 *repl_group_buf1 = NULL;
    uint8 *repl_group_base_ptr_buf = NULL;
    int index_min, index_max;
    int rv = BCM_E_NONE;
    int intf_vec_alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    int i, j;
    uint32 *repl_group_entry0;
    uint32 *repl_group_entry1;
    uint32 *repl_group_base_ptr_entry;
    int head_index;
    uint32 fldbuf[SOC_PBMP_WORD_MAX];
    int member_count;
    int member_id, member;
    bcm_port_t mmu_port, phy_port, port;
    mmu_repl_head_tbl_entry_t repl_head_entry;
    int start_ptr;
    _bcm_repl_list_info_t *rli_current;
    int prev_repl_entry_ptr, repl_entry_ptr;
    mmu_repl_list_tbl_entry_t repl_list_entry;
    int msb;
    uint32 ls_bits[2];
    uint8 flags;
    soc_pbmp_t member_bitmap;
    soc_pbmp_t member_bitmap0;
    soc_pbmp_t member_bitmap1;
    soc_mem_t repl_group_table0;
    soc_mem_t repl_group_table1;
    soc_mem_t repl_group_base_ptr_table = INVALIDm;
    soc_field_t base_idx_f;
    uint32 nh_offset , is_len, offset;
    soc_field_t mbmp0, mbmp1;
    int intf_base;
    int member_bitmap_width0;

    /* Initialize internal data structures */
    BCM_IF_ERROR_RETURN(bcm_tr3_ipmc_repl_init(unit));
 
    is_len = soc_mem_field_length(unit, MMU_REPL_LIST_TBLm, LSB_VLAN_BMf);
    /* coverity[large_shift : FALSE] */
    nh_offset = (1 << (soc_mem_field_length(unit, MMU_REPL_LIST_TBLm,
                                                MSB_VLANf) - 1)) * is_len;
    member_bitmap_width0 =  soc_mem_field_length(unit,
                MMU_REPL_GRP_TBL0m, MEMBER_BMP_PORTS_119_0f);
    /* Recover internal state by traversing replication lists */
    repl_group_table0 = MMU_REPL_GRP_TBL0m;
    repl_group_table1 = MMU_REPL_GRP_TBL1m;
    repl_group_base_ptr_table = MMU_REPL_GRP_TBL2m;
    base_idx_f = BASE_PTRf;
    mbmp0 = MEMBER_BMP_PORTS_119_0f;
    mbmp1 = MEMBER_BMP_PORTS_169_120f;

    /* Read replication group table MMU_REPL_GRP_TBL0 */
    repl_group_buf0 = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, repl_group_table0), "repl group buf");

    if (NULL == repl_group_buf0) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Read replication group table MMU_REPL_GRP_TBL1 */
    repl_group_buf1 = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, repl_group_table1), "repl group buf");

    if (NULL == repl_group_buf1) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    repl_group_base_ptr_buf = soc_cm_salloc(unit,
          SOC_MEM_TABLE_BYTES(unit, repl_group_base_ptr_table), 
                              "repl group base ptr buf");

    if (NULL == repl_group_base_ptr_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, repl_group_table0);
    index_max = soc_mem_index_max(unit, repl_group_table0);
    rv = soc_mem_read_range(unit, repl_group_table0, MEM_BLOCK_ANY,
            index_min, index_max, repl_group_buf0);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, repl_group_table1);
    index_max = soc_mem_index_max(unit, repl_group_table1);
    rv = soc_mem_read_range(unit, repl_group_table1, MEM_BLOCK_ANY,
            index_min, index_max, repl_group_buf1);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }


    index_min = soc_mem_index_min(unit, repl_group_base_ptr_table);
    index_max = soc_mem_index_max(unit, repl_group_base_ptr_table);
    rv = soc_mem_read_range(unit, repl_group_base_ptr_table, MEM_BLOCK_ANY,
                index_min, index_max, repl_group_base_ptr_buf); 
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    intf_vec_alloc_size = SHR_BITALLOCSIZE(REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(intf_vec_alloc_size * sizeof(SHR_BITDCL),
                         "Repl interface vector");
    if (intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(intf_vec, 0, intf_vec_alloc_size);

    for (i = index_min; i <= index_max; i++) {
        repl_group_entry0 = soc_mem_table_idx_to_pointer(unit,
                repl_group_table0, uint32 *, repl_group_buf0, i);

        repl_group_entry1 = soc_mem_table_idx_to_pointer(unit,
                repl_group_table1, uint32 *, repl_group_buf1, i);
 
        /* Get the head index */
        repl_group_base_ptr_entry = soc_mem_table_idx_to_pointer(unit,
                    repl_group_base_ptr_table, uint32 *,
                    repl_group_base_ptr_buf, i);

        head_index = soc_mem_field32_get(unit, repl_group_base_ptr_table,
                    repl_group_base_ptr_entry, base_idx_f);

        /* Get the member bitmap */
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_mem_field_get(unit, repl_group_table0, repl_group_entry0,
                    mbmp0, fldbuf);
        for (j = 0; j < SOC_PBMP_WORD_MAX; j++) {
            SOC_PBMP_WORD_SET(member_bitmap0, j, fldbuf[j]);
        }
         
        sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
        soc_mem_field_get(unit, repl_group_table1, repl_group_entry1,
                          mbmp1, fldbuf);
        for (j = 0; j < SOC_PBMP_WORD_MAX; j++) {
            SOC_PBMP_WORD_SET(member_bitmap1, j, fldbuf[j]);
        }

        SOC_PBMP_CLEAR(member_bitmap);
        SOC_PBMP_OR(member_bitmap, member_bitmap0);

        SOC_PBMP_ITER(member_bitmap1, member) {
           SOC_PBMP_PORT_ADD(member_bitmap,
                    (member + member_bitmap_width0));
        }

        SOC_PBMP_COUNT(member_bitmap, member_count);

        if (0 == member_count) {
            continue; /* with next replication group */
        }

        member_id = 0;
        SOC_PBMP_ITER(member_bitmap, member) {
            /* Convert member to mmu port, then to physical port, and
             * finally to logical port */

            mmu_port = member;
            phy_port = mmu_port;
            port = phy_port;

            /* Get replication list start pointer */
            rv = READ_MMU_REPL_HEAD_TBLm(unit, MEM_BLOCK_ANY,
                    head_index + member_id, &repl_head_entry);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            member_id++;

            start_ptr = soc_MMU_REPL_HEAD_TBLm_field32_get(unit,
                    &repl_head_entry, HEAD_PTRf);
            if (0 == start_ptr) {
                continue; /* with next member */
            }

            if (_bcm_tr3_repl_list_entry_used_get(unit, start_ptr)) {
                /* Already traversed this replication list */
                for (rli_current = REPL_LIST_INFO(unit); rli_current != NULL;
                        rli_current = rli_current->next) {
                    if (rli_current->index == start_ptr) {
                        (rli_current->refcount)++;
                        REPL_PORT_GROUP_INTF_COUNT(unit, port, i) =
                            rli_current->list_size;
                        break;
                    }
                }
                if (rli_current == NULL) {
                    /* Unexpected */
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                } else {
                    continue; /* with next member */
                }
            }

            /* Traverse the replication list table */
            sal_memset(intf_vec, 0, intf_vec_alloc_size);
            prev_repl_entry_ptr = -1;
            repl_entry_ptr = start_ptr;
            while (repl_entry_ptr != prev_repl_entry_ptr) {
                rv = READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY,
                        repl_entry_ptr, &repl_list_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
       
                msb = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, MSB_VLANf);
                soc_MMU_REPL_LIST_TBLm_field_get(unit, &repl_list_entry,
                        LSB_VLAN_BMf, ls_bits);

                intf_base = msb *64;

                if (intf_base >= nh_offset)
                {
                    /* replication on next hops  */
                    offset = _SHR_BITDCLSIZE(soc_mem_index_count(unit, 
                                             EGR_L3_INTFm)); 
                } else {
                    /* replication on L3 interface */
                    offset = 0;
                }
                intf_vec[offset + 2 * msb + 0] = ls_bits[0];
                intf_vec[offset + 2 * msb + 1] = ls_bits[1];

                REPL_PORT_GROUP_INTF_COUNT(unit, port, i) +=
                    _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                rv = _bcm_tr3_repl_list_entry_used_set(unit, repl_entry_ptr);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                prev_repl_entry_ptr = repl_entry_ptr;
                repl_entry_ptr = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, NEXTPTRf);
            }

            /* Insert a new replication list into linked list */
            rli_current = sal_alloc(sizeof(_bcm_repl_list_info_t),
                    "repl list info");
            if (rli_current == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            sal_memset(rli_current, 0, sizeof(_bcm_repl_list_info_t));
            rli_current->index = start_ptr;
            rli_current->hash = _shr_crc32b(0, (uint8 *)intf_vec,
                    REPL_INTF_TOTAL(unit));
            rli_current->list_size = REPL_PORT_GROUP_INTF_COUNT(unit, port, i);
            (rli_current->refcount)++;
            rli_current->next = REPL_LIST_INFO(unit);
            REPL_LIST_INFO(unit) = rli_current;
        }

    }
    soc_cm_sfree(unit, repl_group_buf0);
    repl_group_buf0 = NULL;
    
    soc_cm_sfree(unit, repl_group_buf1);
    repl_group_buf1 = NULL;
   
   
    soc_cm_sfree(unit, repl_group_base_ptr_buf);
    repl_group_base_ptr_buf = NULL;
    
    sal_free(intf_vec);
    intf_vec = NULL;

    /* Recover subscriber REPL from HW cache */
    rv = _bcm_kt2_extq_repl_reload(unit);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Recover REPL list mode from HW cache */
    rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                         _BCM_IPMC_WB_REPL_LIST, &flags);
    if (flags) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

cleanup:
    if (repl_group_buf0) {
        soc_cm_sfree(unit, repl_group_buf0);
    }
    if (repl_group_buf1) {
        soc_cm_sfree(unit, repl_group_buf1);
    }
    if (repl_group_base_ptr_buf) {
        soc_cm_sfree(unit, repl_group_base_ptr_buf);
    }
    if (intf_vec) {
        sal_free(intf_vec);
    }
    if (BCM_FAILURE(rv)) {
        bcm_tr3_ipmc_repl_detach(unit);
    }

    return rv;
}
#endif /* BCM_KATANA2_SUPPORT*/

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE

#ifdef BCM_SABER2_SUPPORT 

/*
 * Function:
 *     bcm_kt2_ipmc_subscriber_sw_dump
 * Purpose:
 *     Displays IPMC Subscriber replication information 
 *     maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void 
bcm_sb2_ipmc_subscriber_sw_dump(int unit)
{
    int                   i, j;
    uint32                *bitmap;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    _sb2_repl_queue_info_t  *group_info;
    _sb2_extq_repl_info_t   *extq_info;


    /*
     * _sb2_extq_repl_info
     */

    LOG_CLI((BSL_META_U(unit,
                        "  IPMC EXTQ REPL Info -\n")));
    extq_info = _sb2_extq_repl_info[unit];
    LOG_CLI((BSL_META_U(unit,
                        "    IPMC Size    : %d\n"), extq_info->ipmc_size));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue Num    : %d\n"), extq_info->queue_num));
    LOG_CLI((BSL_META_U(unit,
                        "    Extq total   : %d\n"), extq_info->extq_list_total));

    LOG_CLI((BSL_META_U(unit,
                        "    Bitmap (index:value-hex) :")));
    if (extq_info->bitmap_entries_used != NULL) {
        bitmap =  extq_info->bitmap_entries_used;
        for (i = 0, j = 0;
             i < _SHR_BITDCLSIZE(extq_info->extq_list_total); i++) {
            /* If zero, skip print */
            if (bitmap[i] == 0) {
                continue;
            }
            if (!(j % 4)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "  %5d:%8.8x"), i, bitmap[i]));
            j++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    group_info = extq_info->mcgroup_info;
    for (i = 0, j = 0; i < extq_info->ipmc_size; i++) {
        /* If zero, skip print */
        if (group_info->queue_count[i] == 0) {
            continue;
        }
        if ((j > 0) && !(j % 4)) {
            LOG_CLI((BSL_META_U(unit,
                                "\n         ")));
        }
        LOG_CLI((BSL_META_U(unit,
                            " %5d:%-4d"), i, group_info->queue_count[i]));
        j++;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    
    rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);
    LOG_CLI((BSL_META_U(unit,
                        "    List Info    -\n")));
    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
        LOG_CLI((BSL_META_U(unit,
                            "    Hash:  0x%08x\n"), rli_current->hash));
        LOG_CLI((BSL_META_U(unit,
                            "    Index: %4d\n"), rli_current->index));
        LOG_CLI((BSL_META_U(unit,
                            "    Size:  %4d\n"), rli_current->list_size));
        LOG_CLI((BSL_META_U(unit,
                            "    Refs:  %4d\n"), rli_current->refcount));
    }
}

#endif 


/*
 * Function:
 *     _bcm_tr3_ipmc_repl_sw_dump
 * Purpose:
 *     Displays IPMC replication information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_tr3_ipmc_repl_sw_dump(int unit)
{
    _tr3_repl_info_t *repl_info;
    bcm_port_t port;
    _tr3_repl_port_info_t *port_info;
    int i, j;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    SHR_BITDCL *bitmap;
    _tr3_repl_head_free_block_t *free_block;
    int num_of_ports          = 0;
    int repl_head_total_free  = 0;

    LOG_CLI((BSL_META_U(unit,
                        "  IPMC REPL Info -\n")));
    repl_info = _tr3_repl_info[unit];
    if (repl_info != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "    Replication Group Size : %d\n"),
                 repl_info->num_repl_groups));
        LOG_CLI((BSL_META_U(unit,
                            "    Replication Intf Size  : %d\n"),
                 repl_info->num_intf));

        LOG_CLI((BSL_META_U(unit,
                            "    Port Info    -\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    port (index:intf-count) :\n")));
        for (port = 0; port < ((SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS) ? 
            SOC_MAX_NUM_PP_PORTS : SOC_MAX_NUM_PORTS); port++) {    
            port_info = repl_info->port_info[port];
            LOG_CLI((BSL_META_U(unit,
                                "    %3d -"), port));
            if (port_info == NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    " null\n")));
                continue;
            }
            for (i = 0, j = 0; i < repl_info->num_repl_groups; i++) {
                /* If zero, skip print */
                if (port_info->intf_count[i] == 0) {
                    continue;
                }
                if ((j > 0) && !(j % 4)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n         ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %5d:%-5d"), i, port_info->intf_count[i]));
                j++;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }

        rli_start = REPL_LIST_INFO(unit);
        LOG_CLI((BSL_META_U(unit,
                            "    List Info    -\n")));
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
            LOG_CLI((BSL_META_U(unit,
                                "    Hash:  0x%08x\n"), rli_current->hash));
            LOG_CLI((BSL_META_U(unit,
                                "    Index: %4d\n"), rli_current->index));
            LOG_CLI((BSL_META_U(unit,
                                "    Size:  %4d\n"), rli_current->list_size));
            LOG_CLI((BSL_META_U(unit,
                                "    Refs:  %4d\n"), rli_current->refcount));
        }

      if (repl_info->l3_intf_next_hop_ipmc != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "    L3 Interface Next Hop IPMC Info -\n")));
        for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
           if (repl_info->l3_intf_next_hop_ipmc[i] ==
                   REPL_NH_INDEX_UNALLOCATED) {
              continue;
           }
           if (repl_info->l3_intf_next_hop_ipmc[i] ==
                   REPL_NH_INDEX_L3_EGRESS_ALLOCATED) {
              continue;
           }
          LOG_CLI((BSL_META_U(unit,
                              "      L3 Interface %4d: Next Hop Index %5d\n"), i,
                   repl_info->l3_intf_next_hop_ipmc[i]));
        }
      }

     if (repl_info->l3_intf_next_hop_trill != NULL) {

        LOG_CLI((BSL_META_U(unit,
                            "    L3 Interface Next Hop Trill Info -\n")));
        for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
           if (repl_info->l3_intf_next_hop_trill[i] == -1) {
              continue;
           }
          LOG_CLI((BSL_META_U(unit,
                              "      L3 Interface %4d: Next Hop Index %5d\n"), i,
                   repl_info->l3_intf_next_hop_trill[i]));
        }
     }
    }

    if (_tr3_repl_list_entry_info[unit] != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "    Replication List Table Size : %d\n"),
                 _tr3_repl_list_entry_info[unit]->num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "    Replication List Table Usage Bitmap (index:value-hex) :")));
        if (_tr3_repl_list_entry_info[unit]->bitmap_entries_used != NULL) {
            bitmap = _tr3_repl_list_entry_info[unit]->bitmap_entries_used;
            for (i = 0, j = 0;
                    i < _SHR_BITDCLSIZE(_tr3_repl_list_entry_info[unit]->num_entries);
                    i++) {
                /* If zero, skip print */
                if (bitmap[i] == 0) {
                    continue;
                }
                if (!(j % 4)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "  %5d:%8.8x"), i, bitmap[i]));
                j++;
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    if (_tr3_repl_head_info[unit] != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "    Replication Head Table Size : %d\n"),
                 soc_mem_index_count(unit, MMU_REPL_HEAD_TBLm)));
        if (soc_property_get(unit, spn_RESERVE_MULTICAST_RESOURCES, 0)) { 
            BCM_PBMP_COUNT(PBMP_ALL(unit), num_of_ports); 
            LOG_CLI(
                (BSL_META_U(unit,
                            " Unit %d Replication Head reserved entries: %d\n"),
                 unit, num_of_ports)); 
        } 
        
        LOG_CLI((BSL_META_U(unit,
                            "    Replication Head Free List Array:\n")));
        if (_tr3_repl_head_info[unit]->free_list_array != NULL) {
            for (i = 0; i < _tr3_repl_head_info[unit]->array_size; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    "      Free List %2d:"), i));
                free_block = REPL_HEAD_FREE_LIST(unit, i);
                j = 0;
                while (NULL != free_block) {
                    if (j > 0 && !(j % 4)) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n                   ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "  %7d:%-7d"), free_block->index,
                             free_block->size));
                    free_block = free_block->next;
                    j++;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
        }
    }
    (void)_bcm_tr3_repl_head_entry_info_get(unit, &repl_head_total_free); 
    LOG_CLI((BSL_META_U(unit,
                        " Unit %d Replication Head total available: %d\n"), 
             unit, repl_head_total_free)); 

#ifdef BCM_KATANA2_SUPPORT
    /*
     * _kt_extq_repl_info
     */
#ifdef BCM_SABER2_SUPPORT 

    if (SOC_IS_SABER2(unit) && (repl_info != NULL)) { 
        bcm_sb2_ipmc_subscriber_sw_dump( unit);
    } else
#endif
    { 
#ifndef BCM_SW_STATE_DUMP_DISABLE  
        if (SOC_IS_KATANA2(unit) && (repl_info != NULL)) { 
            bcm_kt2_ipmc_subscriber_sw_dump(unit);
        }
#endif /* BCM_SW_STATE_DUMP_DISABLE */
    } 
#endif

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      bcm_tr3_ipmc_port_attach_repl_init
 * Purpose:
 *      Initialize IPMC replication for the specified port. This function is
 *      called during Flexport operation on TD2+.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number on which ipmc replication has to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 * This function assumes that 'port' value has been validated
 * This function should not be called during Warmboot
 * Called during port attach operation
 */
int
bcm_tr3_ipmc_port_attach_repl_init(int unit, bcm_port_t port)
{
    int alloc_size;

    REPL_INIT(unit);

    if (NULL == _tr3_repl_info[unit]->port_info[port]) {

        _tr3_repl_info[unit]->port_info[port] =
            sal_alloc(sizeof(_tr3_repl_port_info_t), "repl port info");

        if (NULL == _tr3_repl_info[unit]->port_info[port]) {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                        "Port ipmc repl port info's alloc failed."
                        " unit=%d port=%d(%s)\n"),
                        unit, port, SOC_PORT_NAME(unit, port)));
            return BCM_E_MEMORY;
        }
    }

    sal_memset(_tr3_repl_info[unit]->port_info[port], 0,
                sizeof(_tr3_repl_port_info_t));

    alloc_size = sizeof(int) * _tr3_repl_info[unit]->num_repl_groups;

    if (NULL == _tr3_repl_info[unit]->port_info[port]->intf_count) {

        _tr3_repl_info[unit]->port_info[port]->intf_count =
            sal_alloc(alloc_size, "repl port intf count");

        if (NULL == _tr3_repl_info[unit]->port_info[port]->intf_count) {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                        "Port ipmc repl port intf count's alloc failed."
                        " unit=%d port=%d(%s)\n"),
                        unit, port, SOC_PORT_NAME(unit, port)));

            /* Free previously allocated memory */
            sal_free(_tr3_repl_info[unit]->port_info[port]);
            _tr3_repl_info[unit]->port_info[port] = NULL;

            return BCM_E_MEMORY;
        }
    }

    sal_memset(_tr3_repl_info[unit]->port_info[port]->intf_count, 0,
               alloc_size);

    /* Note: Programming of port agg map init. is done under mmu_set function,
     * by calling soc_td2p_repl_port_agg_map_init()
     * Also, programming of PORT_INITIAL_COPY_COUNT_WIDTHr is done under
     * mmu_set by calling soc_td2p_port_icc_width_set()
     */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ipmc_port_detach_repl_deinit
 * Purpose:
 *      Deinitialize replication data for a port. This function is called
 *      during TD2+ Flexport operation
 * Parameters:
 *      unit - SOC unit #
 *      port - Port for which replication data has to be cleared, or removed
 * Returns:
 *      BCM_E_XXX
 * Notes:
 * This function assumes that 'port' value has been validated
 * Called during TD2+ Flexport port detach operation
 * This function should not be called during Warmboot
 */
int
bcm_tr3_ipmc_port_detach_repl_deinit(int unit, bcm_port_t port)
{
    if (_tr3_repl_info[unit] != NULL) {
        if (_tr3_repl_info[unit]->port_info[port] != NULL) {
            if (_tr3_repl_info[unit]->port_info[port]->intf_count != NULL) {
                sal_free(_tr3_repl_info[unit]->port_info[port]->intf_count);
                _tr3_repl_info[unit]->port_info[port]->intf_count = NULL;
            }
             sal_free(_tr3_repl_info[unit]->port_info[port]);
            _tr3_repl_info[unit]->port_info[port] = NULL;
        }
    }

    return BCM_E_NONE;
}

#if defined(BCM_SABER2_SUPPORT)

int
_bcm_sb2_ipmc_subscriber_egress_queue_qos_map(int unit, int extq_ptr,
                                             int base_queue_id,
                                             int *map_id,
                                             int set)
{
    int rv = BCM_E_NOT_FOUND;
    int next_ptr1, next_ptr2;
    int queue_id;
    int i;
    int profile = 0;
    mmu_ext_mc_queue_list0_entry_t qlist0_entry;
    mmu_ext_mc_queue_list4_entry_t qlist4_entry;
    soc_field_t   base_qid[] = { BASE_QID0f, BASE_QID1f };
    soc_field_t   profile_idx[] = { Q_OFFSET_PROF0f, Q_OFFSET_PROF1f };


    if (map_id == NULL) {
        return BCM_E_INTERNAL;
    }

    if (set) {
        if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
            _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP) {
           return BCM_E_PARAM;
        }
        /* Extract the profile_offset from QOS MAP*/
        profile = *map_id & _BCM_QOS_MAP_TYPE_MASK;

        if ((profile < SB2_MIN_RQE_PROFILE_INDEX ) ||
            (profile > SB2_MAX_RQE_PROFILE_INDEX)) {
            return BCM_E_PARAM;
        }
    }

    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist0_entry)) < 0) {
        goto extq_list_done;
    }
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist4_entry)) < 0) {
        goto extq_list_done;
    }

    for (i = 0; i < 2; i++) {
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit,
                                    &qlist0_entry, base_qid[i])) != 0) {

            if (base_queue_id == queue_id) {
                if (set) {

                    /* set the profile index of the queue list entry */
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                        &qlist0_entry,
                                        profile_idx[i], profile);

                    rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit,
                                          MEM_BLOCK_ANY,
                                          extq_ptr, &qlist0_entry);
                } else {
                    profile = soc_mem_field32_get(unit,
                                    MMU_EXT_MC_QUEUE_LIST0m,
                                    &qlist0_entry, profile_idx[i]);
                    *map_id = profile | (_BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP
                                                     << _BCM_QOS_MAP_SHIFT);

                    rv = BCM_E_NONE;
                }
            }
        }
    }

    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                            &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                            &qlist4_entry, EXT_Q_NXT_PTR2f);

    if (next_ptr1 != extq_ptr) {
        rv = _bcm_sb2_ipmc_subscriber_egress_queue_qos_map(unit, next_ptr1,
                                                 base_queue_id, map_id, set);
    }

    if (next_ptr2 != extq_ptr) {
        rv = _bcm_sb2_ipmc_subscriber_egress_queue_qos_map(unit, next_ptr2,
                                                 base_queue_id, map_id, set);
    }

extq_list_done:
    return rv;

}

STATIC
int _bcm_sb2_ipmc_egress_qos_profile_update(int unit, int ipmc_id,
                                            int queue_id, int qos_map_id)
{
    _bcm_ext_repl_q_list_t *queue_node;
    _bcm_ext_q_ipmc_t  *ipmc_node = &_sb2_extq_repl_info[unit]->
                                          ext_ipmc_list[ipmc_id];

    queue_node = ipmc_node->first_q_ptr;
    while (queue_node != NULL) {
           if (queue_node->queue_id == queue_id) {
               queue_node->profile_id = qos_map_id;
               break;
           }
           queue_node = queue_node->next;
    }
    if (queue_node == NULL) {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_sb2_ipmc_subscriber_egress_intf_qos_map_set
 * Purpose:
 *    Assign the complete set of egress GPORTs in the
 *   replication list for the specified multicast index.
 * Parameters:
 *   unit              - (IN) Device Number
 *   group             - (IN) Multicast group ID
 *   port              - (IN) GPORT Identifier
 *   subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *   qos_map_id        - (IN) Qos Map ID
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_sb2_ipmc_subscriber_egress_intf_qos_map_set(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int qos_map_id)
{

    int         rv = BCM_E_NONE;
    int         extq_ptr;
    int         queue_id;

    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, ipmc_id));

    if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                           _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                           NULL,  &queue_id, NULL)) < 0) {
        goto clean_up;
    }

    REPL_LOCK(unit);
    if ((rv = _sb2_extq_mc_group_ptr(unit, ipmc_id,
                                &extq_ptr, 0, 0)) < 0) {
        goto clean_up;
    }

    if (extq_ptr == 0) {
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    rv = _bcm_sb2_ipmc_subscriber_egress_queue_qos_map(unit, extq_ptr,
                                                  queue_id, &qos_map_id, 1);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_COMMON,
          (BSL_META_U(unit,"Failure - setting subscriber queue %d Qos Map %d\n"),
                           subscriber_queue, qos_map_id));
    }
    /* Update profile id */
    rv = _bcm_sb2_ipmc_egress_qos_profile_update(unit, ipmc_id, queue_id, qos_map_id);

    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_COMMON,
          (BSL_META_U(unit,"Failure - queue %d not found for IPMC id %d\n"),
                           queue_id, ipmc_id));
    }

clean_up:
    REPL_UNLOCK(unit);
    return rv;

}

/*
 * Function:
 *    _bcm_sb2_ipmc_subscriber_egress_intf_qos_map_get
 * Purpose:
 *    Assign the complete set of egress GPORTs in the
 *   replication list for the specified multicast index.
 * Parameters:
 *   unit              - (IN) Device Number
 *   group             - (IN) Multicast group ID
 *   port              - (IN) GPORT Identifier
 *   subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *   qos_map_id        - (OUT) Qos Map ID
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_sb2_ipmc_subscriber_egress_intf_qos_map_get(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int *qos_map_id)
{
    int         rv = BCM_E_NONE;
    int         extq_ptr;
    int         queue_id;

    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt_init_check(unit, ipmc_id));

    if (qos_map_id == NULL) {
        return BCM_E_INTERNAL;
    }

    REPL_LOCK(unit);
    if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                           _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                           NULL,  &queue_id, NULL)) < 0) {
        goto clean_up;
    }

    if ((rv = _sb2_extq_mc_group_ptr(unit, ipmc_id,
                                &extq_ptr, 0, 0)) < 0) {
        goto clean_up;
    }

    if (extq_ptr == 0) {
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    rv = _bcm_sb2_ipmc_subscriber_egress_queue_qos_map(unit, extq_ptr,
                                                  queue_id, qos_map_id, 0);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_COMMON,
          (BSL_META_U(unit,"Failure - getting subscriber queue %d\n"),
                           subscriber_queue));
    }

clean_up:
    REPL_UNLOCK(unit);
    return rv;

}


#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
STATIC int
_bcm_td2p_ipmc_egress_intf_set_for_trunk_first_member(
    int unit, int repl_group, bcm_port_t port,
    int if_count, bcm_if_t *if_array, int is_l3,
    int check_port, bcm_trunk_t tgid)
{
    int old_intf_count, new_intf_count;
    int aggid;

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    if ((old_intf_count == 0) && (if_count > 0)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_port_aggid_add(unit, port, tgid, &aggid));
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_ipmc_egress_intf_set(
            unit, repl_group, port, if_count, if_array, is_l3, check_port));

    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    if ((old_intf_count > 0) && (new_intf_count == 0)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_port_aggid_del(unit, port));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
    int unit, int repl_group, bcm_port_t port,
    bcm_port_t first_port, bcm_trunk_t tgid)
{
    int aggid;
    int old_intf_count, new_intf_count;

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, first_port, repl_group);
    if (old_intf_count == new_intf_count) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_repl_initial_copy_count_set(unit, repl_group,
                                             port, new_intf_count));

    if ((old_intf_count == 0) && (new_intf_count > 0)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_port_aggid_add(unit, port, tgid, &aggid));
    } else if ((old_intf_count > 0) && (new_intf_count == 0)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_port_aggid_del(unit, port));
    }

    REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = new_intf_count;

    return BCM_E_NONE;
}

STATIC int
_bcm_td2p_ipmc_egress_intf_add_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int encap_id, int is_l3)
{
    int aggid;

    if (REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) == 0) {
        BCM_IF_ERROR_RETURN(
            bcm_td2p_port_aggid_add(unit, port, BCM_TRUNK_INVALID, &aggid));
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_ipmc_egress_intf_add(
            unit, repl_group, port, encap_id, is_l3));

    return BCM_E_NONE;
}

STATIC int
_bcm_td2p_ipmc_egress_intf_del_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int if_max, int encap_id, int is_l3)
{

    int old_intf_count, new_intf_count;

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    BCM_IF_ERROR_RETURN(
        _bcm_esw_ipmc_egress_intf_delete(
            unit, repl_group, port, if_max, encap_id, is_l3));
    new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    if (new_intf_count == 0 && old_intf_count > 0) {
        BCM_IF_ERROR_RETURN(bcm_td2p_port_aggid_del(unit, port));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_td2p_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int encap_id, int is_l3)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int idx;
    int member_count, port_iter;
    bcm_port_t trunk_pipe_first_port_array[BCM_PIPES_MAX];
    int aggid, pipe;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &member_count));

    for (idx = 0; idx < BCM_PIPES_MAX; idx++) {
        trunk_pipe_first_port_array[idx] = -1;
    }
    for (idx = 0; idx < member_count; idx++) {
        port_iter = trunk_local_member_ports[idx];
        pipe      = SOC_INFO(unit).port_pipe[port_iter];
        if (trunk_pipe_first_port_array[pipe] == -1) {
            if (REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group) == 0) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2p_port_aggid_add(unit, port_iter, tgid, &aggid));
            }
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_egress_intf_add(
                    unit, repl_group, port_iter, encap_id, is_l3));
            trunk_pipe_first_port_array[pipe] = port_iter;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
                    unit, repl_group, port_iter,
                    trunk_pipe_first_port_array[pipe], tgid));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2p_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int if_max, int encap_id, int is_l3)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int idx;
    int member_count, port_iter;
    bcm_port_t trunk_pipe_first_port_array[BCM_PIPES_MAX];
    int pipe;
    int old_intf_count, new_intf_count;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &member_count));

    for (idx = 0; idx < BCM_PIPES_MAX; idx++) {
        trunk_pipe_first_port_array[idx] = -1;
    }

    for (idx = 0; idx < member_count; idx++) {
        port_iter = trunk_local_member_ports[idx];
        pipe      = SOC_INFO(unit).port_pipe[port_iter];
        if (trunk_pipe_first_port_array[pipe] == -1) {
            old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_egress_intf_delete(
                     unit, repl_group, trunk_local_member_ports[idx],
                     if_max, encap_id, is_l3));
            new_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
            if (new_intf_count == 0 && old_intf_count > 0) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2p_port_aggid_del(unit, port_iter));
            }
            trunk_pipe_first_port_array[pipe] = port_iter;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
                    unit, repl_group, port_iter,
                    trunk_pipe_first_port_array[pipe], tgid));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2p_ipmc_egress_intf_add_trunk_member(int unit, int repl_group,
                                            bcm_port_t port)
{
#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PP_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PP_PORTS;
#else
    bcm_port_t      trunk_local_member_ports[SOC_MAX_NUM_PORTS] = {0};
    int             max_num_ports = SOC_MAX_NUM_PORTS;
#endif

    int local_modid;
    int trunk_local_ports;
    int intf_count, if_count, old_intf_count;
    int same_pipe_port = -1, diff_pipe_port = -1;
    int i, rv;
    bcm_port_t port_iter;
    bcm_if_t *if_array;
    bcm_trunk_t tgid;
    int aggid;

    /* Member port belongs to an existing trunk, it needs to share
     * identical replication list as other members of this trunk in
     * same pipe.
     */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));
    BCM_IF_ERROR_RETURN(
        bcm_esw_trunk_find(unit, local_modid, port, &tgid));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_local_members_get(unit, tgid, max_num_ports,
                                         trunk_local_member_ports,
                                         &trunk_local_ports));

    for (i = 0; i < trunk_local_ports; i++) {
        port_iter = trunk_local_member_ports[i];
        if (port != port_iter) {
            intf_count =
                REPL_PORT_GROUP_INTF_COUNT(unit, port_iter, repl_group);
            if (intf_count > 0) {
                if (SOC_INFO(unit).port_pipe[port] ==
                    SOC_INFO(unit).port_pipe[port_iter]) {
                    same_pipe_port = port_iter;
                    break;
                } else {
                    if (diff_pipe_port == -1) {
                        diff_pipe_port = port_iter;
                    }
                }
            }
        }
    }

    /* or return BCM_E_PARAM? */
    if (same_pipe_port == -1 && diff_pipe_port == -1) {
        return BCM_E_NONE;
    }

    if (same_pipe_port != -1) {
        BCM_IF_ERROR_RETURN(
            _bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
                unit, repl_group, port, same_pipe_port, tgid));
    } else {
        intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, diff_pipe_port,
                                                repl_group);
        if_array = sal_alloc(sizeof(bcm_if_t) * intf_count,
                             "if_array pointers");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        rv = bcm_tr3_ipmc_egress_intf_get(unit, repl_group, diff_pipe_port,
                                          intf_count, if_array, &if_count);
        if (BCM_FAILURE(rv)) {
            sal_free(if_array);
            return rv;
        }

        old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
        if ((old_intf_count == 0) && (intf_count > 0)) {
            rv = bcm_td2p_port_aggid_add(unit, port, tgid, &aggid);
            if (BCM_FAILURE(rv)) {
                sal_free(if_array);
                return rv;
            }
        }
        rv = _bcm_esw_ipmc_egress_intf_set(unit, repl_group, port,
                                           if_count, if_array, TRUE, FALSE);
        if (BCM_FAILURE(rv)) {
            sal_free(if_array);
            return rv;
        }
        sal_free(if_array);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2p_ipmc_egress_intf_del_trunk_member(int unit, int repl_group,
                                            bcm_port_t port)
{
    int is_last_member;
    int old_intf_count;

    BCM_IF_ERROR_RETURN(
        bcm_td2p_port_last_member_check(unit, port, &is_last_member));

    old_intf_count = REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group);
    if (is_last_member) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_ipmc_egress_intf_set(
                unit, repl_group, port, 0, NULL, TRUE, FALSE));
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_td2_repl_initial_copy_count_set(
                unit, repl_group, port, 0));
        REPL_PORT_GROUP_INTF_COUNT(unit, port, repl_group) = 0;
    }

    if (old_intf_count > 0) {
        BCM_IF_ERROR_RETURN(bcm_td2p_port_aggid_del(unit, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_set_for_trunk_first_member
 * Purpose:
 *      Assign set of egress interfaces to the first port of a trunk.
 *      or to the only one non-trunk port.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Port to assign replication list.
 *  if_count   - (IN) Number of interfaces in replication list.
 *  if_array   - (IN) List of interface numbers.
 *  if_updated - (IN) interfaces info to be updated including
 *                    interfaces which is new to the group and port
 *                    and interfaces to be deleted.
 *  is_l3      - (IN) Indicates if multicast group is of type IPMC.
 *  check_port - (IN) If if_array consists of L3 interfaces, this parameter
 *                    controls whether to check the given port is a member
 *                    in each L3 interface's VLAN. This check should be
 *                    disabled when not needed, in order to improve
 *                    performance.
 *  tgid       - (IN) trunk id or BCM_TRUNK_INVALID for non-trunk port.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_set_for_trunk_first_member(
    int unit, int repl_group, bcm_port_t port,
    int if_count, bcm_if_t *if_array, int is_l3,
    int check_port, bcm_trunk_t tgid)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_set_for_trunk_first_member(
             unit, repl_group, port, if_count, if_array,
             is_l3, check_port, tgid);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member
 * Purpose:
 *      Align egress intf of same pipe port with the first port of trunk.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Port to assign replication list.
 *  first_port - (IN) The first member port of the trunk.
 *  tgid       - (IN) trunk id.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
    int unit, int repl_group, bcm_port_t port,
    bcm_port_t first_port, bcm_trunk_t tgid)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
             unit, repl_group, port, first_port, tgid);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_add_in_per_trunk_mode
 * Purpose:
 *      Add encap ID to selected ports' replication list for chosen
 *      IPMC group in mc per trunk repl mode.
 *      It is used for non-trunk port.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Port to assign replication list.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_add_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_add_in_per_trunk_mode(
             unit, repl_group, port, encap_id, is_l3);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_del_in_per_trunk_mode
 * Purpose:
 *      Remove encap ID from selected ports' replication list for chosen
 *      IPMC group in mc per trunk repl mode.
 *      It is used for non-trunk port.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Port to assign replication list.
 *  if_max     - (IN) Maximal interface.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_del_in_per_trunk_mode(int unit, int repl_group,
    bcm_port_t port, int if_max, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_del_in_per_trunk_mode(
             unit, repl_group, port, if_max, encap_id, is_l3);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_add_for_trunk
 * Purpose:
 *      Add encap ID to a trunk.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  tgid       - (IN) Trunk ID.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_add_for_trunk(
             unit, repl_group, tgid, encap_id, is_l3);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_del_for_trunk
 * Purpose:
 *      Remove encap ID from a trunk.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  tgid       - (IN) Trunk ID.
 *  if_max     - (IN) Maximal interface.
 *  encap_id   - (IN) Encap ID.
 *  is_l3      - (IN) Indicates if multicast group type is IPMC.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
    bcm_trunk_t tgid, int if_max, int encap_id, int is_l3)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_del_for_trunk(
             unit, repl_group, tgid, if_max, encap_id, is_l3);
    IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_add_trunk_member
 * Purpose:
 *      Add a port member to a trunk for chosen IPMC group.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Trunk member port.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_add_trunk_member(int unit, int repl_group,
                                           bcm_port_t port)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_add_trunk_member(
             unit, repl_group, port);
    IPMC_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      bcm_td2p_ipmc_egress_intf_del_trunk_member
 * Purpose:
 *      Remove a port member from a trunk for chosen IPMC group.
 * Parameters:
 *  unit       - (IN) StrataSwitch PCI device unit number.
 *  repl_group - (IN) The replication group number.
 *  port       - (IN) Trunk member port.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ipmc_egress_intf_del_trunk_member(int unit, int repl_group,
                                           bcm_port_t port)
{
    int rv = BCM_E_NONE;

    IPMC_LOCK(unit);
    rv = _bcm_td2p_ipmc_egress_intf_del_trunk_member(
             unit, repl_group, port);
    IPMC_UNLOCK(unit);

    return rv;
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */
#endif
