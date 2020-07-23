/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix5.h
 */

#ifndef _SOC_HELIX5_H_
#define _SOC_HELIX5_H_

#include <soc/drv.h>
#include <soc/trident3.h>
#include <soc/helix5_tdm.h>

#define SOC_HX5_PORT_BLOCK_BASE_PORT(port)                          \
        SOC_INFO(unit).port_l2p_mapping[port] <= 48 ?               \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0xF)) :\
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

/* **************** TDM-related Macors and structs adopted from Tomahawk ---start************ */
/* Device port */
/* 69 General device port + 1 CPU + 1 Loopback */
#define _HX5_TDM_GDEV_PORTS_PER_PIPE        69
#define _HX5_TDM_DEV_PORTS_PER_PIPE         (_HX5_TDM_GDEV_PORTS_PER_PIPE + 2)
#define _HX5_TDM_DEV_PORTS_PER_DEV          \
    (_HX5_TDM_DEV_PORTS_PER_PIPE * HELIX5_TDM_PIPES_PER_DEV)
/* MMU port */
#define _HX5_TDM_MMU_PORTS_OFFSET_PER_PIPE  69
#define _HX5_TDM_MMU_PORTS_PER_DEV          (69 * 1)
/* ------------------------------------------------- */
#define _HX5_TDM_LENGTH 512	
#define _HX5_PKT_SCH_LENGTH 160
#define _HX5_TDM_SPEED_CLASS_MAX                5

#define HX5_CLK_450MHZ 450
#define HX5_CLK_400MHZ 400
#define HX5_CLK_543MHZ 543
#define HX5_CLK_668MHZ 668
#define HX5_CLK_812MHZ 812
#define HX5_CLK_893MHZ 893
#define HX5_CLK_931MHZ 931

#define HX5_ICID_LENGTH_BYTES                   8 /* ICID length in bytes */

typedef struct _soc_helix5_tdm_pblk_info_s {
	int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
}_soc_helix5_tdm_pblk_info_t;

typedef struct _soc_helix5_tdm_pipe_s {
    int idb_tdm[_HX5_TDM_LENGTH];
    int mmu_tdm[_HX5_TDM_LENGTH];
    int ovs_tdm[HELIX5_OVS_HPIPE_COUNT_PER_PIPE][HELIX5_TDM_OVS_GROUPS_PER_HPIPE][HELIX5_TDM_OVS_GROUP_LENGTH];
    int pkt_shaper_tdm[HELIX5_OVS_HPIPE_COUNT_PER_PIPE][_HX5_PKT_SCH_LENGTH];
} _soc_helix5_tdm_pipe_t;

typedef struct _soc_helix5_tdm_temp_s {
    _soc_helix5_tdm_pipe_t tdm_pipe[HELIX5_TDM_PIPES_PER_DEV];
    _soc_helix5_tdm_pblk_info_t pblk_info[HELIX5_TDM_PHY_PORTS_PER_DEV];
    int port_ratio[HELIX5_TDM_PBLKS_PER_DEV];
    int ovs_ratio_x1000[HELIX5_TDM_PIPES_PER_DEV][HELIX5_OVS_HPIPE_COUNT_PER_PIPE];

} _soc_helix5_tdm_temp_t;

/* **************** TDM-related Macors and structs adopted from Tomahawk ---end************ */


/********************** SER begin ******************************/

/* 2 dedicated banks for L2
 * 2 dedicated banks for L3
 * 6 banks for UFT
 * 8 banks for IUAT
 * 4 banks for EUAT
 *
 * For L2, since there is only single wide view, the LP is only available for single wide view.
 * For L3 and UFT/UAT banks, since they can be assigned to any logical table which supports different views,
 * LP is available for different views - single, double and quad.
 */

/* Dedicated L2 and L3 entries - 8k per bank */
#define SOC_HX5_NUM_ENTRIES_L2                   16384
#define SOC_HX5_NUM_ENTRIES_L3                   16384
#define SOC_HX5_NUM_BANKS_L2                     2
#define SOC_HX5_NUM_BANKS_L3                     2

/* Dedicated L2 LP entries per bank */
#define SOC_HX5_NUM_ENTRIES_L2_LP_BANK           2048 /* 1W */
/* Dedicated L3 LP entries per bank */
#define SOC_HX5_NUM_ENTRIES_L3_LP_BANK           3584 /* 2048(1W) + 1024(2W) + 512(4W) */

/* Same for ingress and egress UAT */
#define SOC_HX5_NUM_ENTRIES_UAT_LP_BANK          1536 /* 1024(1W) + 512(2W) */

/* UFT entries per bank */
#define SOC_HX5_NUM_ENTRIES_UFT_BANK             16384
/* UFT LP entries per bank */
#define SOC_HX5_NUM_ENTRIES_UFT_LP_BANK          7168 /* 4096(1W) + 2048(2W) + 1024(4W) */

#define SOC_HX5_NUM_XOR_RAM_UFT                  2
#define SOC_HX5_NUM_ENTRIES_XOR_RAM              8192

extern int
soc_helix5_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                             _soc_ser_sram_info_t *sram_info);

extern int soc_hx5_ser_enable_all(int unit, int enable);
extern void soc_hx5_ser_error(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int soc_hx5_mem_ser_control(int unit, soc_mem_t mem,
                                   int copyno, int enable);
extern void soc_hx5_ser_register(int unit);

extern void soc_hx5_ser_test_register(int unit);

extern int soc_hx5_ser_lp_mem_info_get(int unit, soc_mem_t mem, int index,
                                       soc_mem_t *hash_mem, int *hash_index);
/********************* SER end *********************************/


extern int soc_hx5_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
                                    int thdo_db_shd, int thdo_qe_shd,
                                    int post_update);

extern soc_functions_t soc_helix5_drv_funs;
extern int soc_helix5_mem_config(int unit);
extern int soc_helix5_num_cosq_init(int unit);
extern int soc_helix5_num_cosq_init_port(int unit, int port);
extern int soc_hx5_port_oversub_get(int unit, int phy_port, int logical_port);
extern int soc_helix5_port_config_init(int unit, uint16 dev_id);
extern int soc_helix5_chip_reset(int unit);
extern int soc_helix5_port_reset(int unit);

extern int soc_helix5_port_obm_info_get(int unit, soc_port_t port,
                                          int *obm_id, int *lane);

/* Counter */
extern int soc_counter_helix5_status_enable(int unit, int enable);
extern int soc_counter_helix5_non_dma_init(int unit, int nports,
                                             int non_dma_start_index,
                                             int *non_dma_entries);
extern int soc_counter_helix5_get_info(int unit, soc_port_t port,
                                         soc_reg_t id, int *base_index,
                                         int *num_entries);
extern int soc_counter_helix5_eviction_enable(int unit, int enable);
extern void soc_counter_helix5_eviction_flags_update(int unit,
                                                       uint32 flags,
                                                       int enable);
extern int soc_counter_helix5_generic_get_info(int unit,  soc_ctr_control_info_t ctrl_info,
                                            soc_reg_t id, int *base_index, int *num_entries);
extern int  _bcm_hx5_pfc_reg_index_get(int unit, soc_port_t port, soc_port_t *mport, int *index);

#define IS_OVERSUB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))

enum soc_hx5_port_ratio_e {
    SOC_HX5_PORT_RATIO_SINGLE,
    SOC_HX5_PORT_RATIO_DUAL_1_1,
    SOC_HX5_PORT_RATIO_DUAL_2_1,
    SOC_HX5_PORT_RATIO_DUAL_1_2,
    SOC_HX5_PORT_RATIO_TRI_023_2_1_1,
    SOC_HX5_PORT_RATIO_TRI_023_4_1_1,
    SOC_HX5_PORT_RATIO_TRI_012_1_1_2,
    SOC_HX5_PORT_RATIO_TRI_012_1_1_4,
    SOC_HX5_PORT_RATIO_QUAD,
    SOC_HX5_PORT_RATIO_QSGMII0,
    SOC_HX5_PORT_RATIO_QSGMII1,
    SOC_HX5_PORT_RATIO_COUNT
};

typedef int (*_soc_hx5_tcb_hw_cb)(int unit);
extern int soc_hx5_set_tcb_callback(int unit, _soc_hx5_tcb_hw_cb cb);
extern int _soc_hx5_process_mmu_tcb(int unit, int xpe, soc_field_t field);

extern int      soc_hx5_shared_hash_mem_bucket_read(int unit, int ent,
                                        int key_type, int *width, 
                                        soc_mem_t base_mem, soc_mem_t *mem,
                                        void *bkt_entry, void *entry);
extern int      soc_hx5_hash_mem_status_get(int unit, soc_mem_t mem,
                                            void* entry, uint32 *valid);
extern int      soc_helix5_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_helix5_hash_bank_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_hx5_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem,
                                                 uint32 *bitmap);
extern int      soc_hx5_hash_bank_number_get(int unit, soc_mem_t mem,
                                             int seq_num, int *bank_num);
extern int      soc_hx5_hash_seq_number_get(int unit, soc_mem_t mem,
                                            int bank_num, int *seq_num);
extern int      soc_hx5_hash_bank_info_get(int unit, soc_mem_t mem,
                                           int bank,
                                           int *entries_per_bank,
                                           int *entries_per_row,
                                           int *entries_per_bucket,
                                           int *bank_base,
                                           int *bucket_offset);
extern int      soc_hx5_hash_bucket_get(int unit, int mem, int bank,
                                       uint32 *entry, uint32 *bucket);
extern int      soc_hx5_hash_index_get(int unit, int mem, int bank, int bucket);
extern int      soc_hx5_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                       int *hash_offset, int *use_lsb);
extern int      soc_hx5_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                       int hash_offset, int use_lsb);
extern uint32   soc_hx5_l2x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key1, uint8 *key2);
extern int      soc_hx5_l2x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_hx5_l2x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_hx5_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_hx5_l3x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key1, uint8 *key2);
extern int      soc_hx5_l3x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_hx5_l3x_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_hx5_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_hx5_exact_match_hash(int unit, int bank, int hash_offset,
                                         int use_lsb, int key_nbits,
                                         void *base_entry, uint8 *key1,
                                         uint8 *key2);
extern int      soc_hx5_exact_match_base_entry_to_key(int unit, int bank, uint32 *entry,
                                                      uint8 *key);
extern uint32   soc_hx5_exact_match_entry_hash(int unit, int bank,
                                              int hash_offset, int use_lsb,
                                              uint32 *entry);
extern uint32   soc_hx5_exact_match_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32   soc_hx5_mpls_hash(int unit, soc_mem_t mem, int bank, int hash_offset, int use_lsb,
                                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2);
extern int      soc_hx5_mpls_base_entry_to_key(int unit, soc_mem_t mem, int bank, void *entry,
                                              uint8 *key);
extern uint32   soc_hx5_mpls_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                        int use_lsb, uint32 *entry);
extern uint32   soc_hx5_mpls_bank_entry_hash(int unit, soc_mem_t mem, int bank, uint32 *entry);
extern uint32   soc_hx5_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                        int use_lsb, int key_nbits, void *base_entry,
                                        uint8 *key1, uint8 *key2);
extern int      soc_hx5_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                                     void *entry, uint8 *key);
extern uint32   soc_hx5_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                              int use_lsb, uint32 *entry);
extern uint32   soc_hx5_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                                  uint32 *entry);
extern uint32   soc_hx5_egr_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                            int use_lsb, int key_nbits, void *base_entry,
                                            uint8 *key1, uint8 *key2);
extern int      soc_hx5_egr_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem,
                                                         int bank, void *entry, uint8 *key);
extern uint32   soc_hx5_egr_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                                  int use_lsb, uint32 *entry);
extern uint32   soc_hx5_egr_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem,
                                                       int bank, uint32 *entry);
extern uint32   soc_hx5_ing_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                                int use_lsb, int key_nbits, void *base_entry,
                                                uint8 *key1, uint8 *key2);
extern int      soc_hx5_ing_vp_vlan_member_base_entry_to_key(int unit,
                                                            int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_hx5_ing_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                                      int use_lsb, uint32 *entry);
extern uint32   soc_hx5_ing_vp_vlan_member_bank_entry_hash(int unit, int bank,
                                                           uint32 *entry);
extern uint32   soc_hx5_egr_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2);
extern int      soc_hx5_egr_vp_vlan_member_base_entry_to_key(int unit,
                                                            int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_hx5_egr_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                                      int use_lsb, uint32 *entry);
extern uint32   soc_hx5_egr_vp_vlan_member_bank_entry_hash(int unit, int bank,
                                                           uint32 *entry);

#ifdef BCM_HURRICANE4_SUPPORT
extern
uint32 soc_hx5_subport_id_to_sgpp_map_hash(int unit, int bank, int hash_offset,
                                           int use_lsb, int key_nbits,
                                           void *base_entry, uint8 *key1, uint8 *key2);
extern
int soc_hx5_subport_id_to_sgpp_map_base_entry_to_key(int unit, int bank,
                                                     void *entry, uint8 *key);
extern
uint32 soc_hx5_subport_id_to_sgpp_map_entry_hash(int unit, int bank, int hash_offset,
                                                 int use_lsb, uint32 *entry);
extern
uint32 soc_hx5_subport_id_to_sgpp_map_bank_entry_hash(int unit, int bank,
                                                      uint32 *entry);
#endif

extern int soc_helix5_bond_info_init(int unit);
extern int soc_hx5_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_hx5_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);

typedef enum {
    SOC_HX5_SCHED_MODE_UNKNOWN = 0,
    SOC_HX5_SCHED_MODE_STRICT,
    SOC_HX5_SCHED_MODE_WRR,
    SOC_HX5_SCHED_MODE_WERR
} soc_hx5_sched_mode_e;

typedef enum {
    SOC_HX5_NODE_LVL_ROOT = 0,
    SOC_HX5_NODE_LVL_L0,
    SOC_HX5_NODE_LVL_L1,
    SOC_HX5_NODE_LVL_L2,
    SOC_HX5_NODE_LVL_L3,
    SOC_HX5_NODE_LVL_MAX
} soc_hx5_node_lvl_e;

typedef enum {
    _SOC_HX5_INDEX_STYLE_BUCKET,
    _SOC_HX5_INDEX_STYLE_BUCKET_MODE,
    _SOC_HX5_INDEX_STYLE_WRED,
    _SOC_HX5_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_HX5_INDEX_STYLE_SCHEDULER,
    _SOC_HX5_INDEX_STYLE_PERQ_XMT,
    _SOC_HX5_INDEX_STYLE_UCAST_DROP,
    _SOC_HX5_INDEX_STYLE_UCAST_QUEUE,
    _SOC_HX5_INDEX_STYLE_MCAST_QUEUE,
    _SOC_HX5_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_HX5_INDEX_STYLE_EGR_POOL,
    _SOC_HX5_INDEX_STYLE_COUNT
} soc_hx5_index_style_t;

typedef enum soc_hx5_drop_limit_alpha_value_e {
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_HX5_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_hx5_drop_limit_alpha_value_t;

#define _HX5_PORTS_PER_PBLK             4
#define _HX5_PORTS_PER_PMQ_PBLK         16
#define _HX5_PBLKS_PER_PIPE             19
#define _HX5_PIPES_PER_DEV              1
#define _HX5_XPES_PER_DEV               1
#define _HX5_OBMS_PER_PIPE              10

#define _HX5_PBLKS_PER_DEV(unit)        (_HX5_PBLKS_PER_PIPE * NUM_PIPE(unit))

#define _HX5_PORTS_PER_PIPE             (_HX5_PORTS_PER_PBLK * _HX5_PBLKS_PER_PIPE)
#define _HX5_PORTS_PER_DEV(unit)        (_HX5_PORTS_PER_PIPE * NUM_PIPE(unit))

#define _HX5_SECTION_SIZE                32768

#define _HX5_MMU_MAX_PACKET_BYTES        9416  /* bytes */
#define _HX5_MMU_PACKET_HEADER_BYTES     64    /* bytes */
#define _HX5_MMU_JUMBO_FRAME_BYTES       9216  /* bytes */
#define _HX5_MMU_DEFAULT_MTU_BYTES       1536  /* bytes */

#define _HX5_MMU_PHYSICAL_CELLS_PER_XPE  36*1024 /* Total Physical cells per XPE */
#define _HX5_MMU_TOTAL_CELLS_PER_XPE     32768 /* 32MBE */
#define _HX5_MMU_RSVD_CELLS_CFAP_PER_XPE 346    /* Reserved CFAP cells per XPE */
#define _HX5_MMU_CFAP_BANK_FULL_LIMIT    1023   /* CFAP Bank Full limit */

#define _HX5_MMU_TOTAL_CELLS             (_HX5_MMU_TOTAL_CELLS_PER_XPE * _HX5_XPES_PER_DEV)
#define _HX5_MMU_PHYSICAL_CELLS          (_HX5_MMU_PHYSICAL_CELLS_PER_XPE * _HX5_XPES_PER_DEV)

#define _HX5_MMU_BYTES_PER_CELL          256   /* bytes (2048 bits) */
#define _HX5_MMU_NUM_PG                  8
#define _HX5_MMU_NUM_POOL                4
#define _HX5_MMU_NUM_RQE_QUEUES          11
#define _HX5_MMU_NUM_LBK_QUEUES          10
#define _HX5_MMU_NUM_INT_PRI             16

#define SOC_HX5_MMU_CFG_QGROUP_MAX       34

#define SOC_HX5_MMU_PORT_STRIDE          0x7f     /* MMU Port number space per pipe */

#define SOC_HX5_NUM_CPU_QUEUES           48          /* CPU port has 48 MC Queues */
#define SOC_HX5_NUM_UC_QUEUES_PER_PIPE   1280        /* (54 Downlink Ports * 16  + (8 Uplink + 8 Stacking) * 26) */
#define SOC_HX5_NUM_MC_QUEUES_PER_PIPE   1338        /* (54 Downlink Ports * 16  + (8 Uplink + 8 Stacking) * 26)  + 1 CPU Port +*/
                                                     /* 1 Loop-back Port) * 10 */
#define _BCM_HX5_MMU_L3_MC_CPU_QBASE     2570
#define _BCM_HX5_MMU_L3_MC_LB_QBASE      2560

#define SOC_HX5_CPU_MCQ_BASE             1290        /* CPU port MC queue base index */
#define SOC_HX5_LB_MCQ_BASE              1280        /* LB ports MC queue base index */
#define SOC_HX5_DOWNLINK_PORTQ_BASE      (26 * 16)   /* 16 (8 UPlink + 8 Stacking) * 26 queues */


/* Number of Unicast Queues per Downlink Port 
 Queues 0 - 7 for wireless traffic and 8 - 15 for wired traffic*/
#define SOC_HX5_NUM_UCAST_QUEUE_PER_PORT 16
/* Number of Multicast Queues per Downlink  Port 
 Queues 0 - 7 for wireless traffic and 8 - 15 for wired traffic*/
#define SOC_HX5_NUM_MCAST_QUEUE_PER_PORT 16
/* Number of Scheduler Nodes per Downlink Port */
#define SOC_HX5_NUM_SCHEDULER_PER_PORT   19

/* Number of Unicast Queues per HG/Stacking Port 
 Queues 0 - 7 for transit  traffic and 8 - 15 for wireless traffic, 16 - 23 is for wired traffic */ 
#define SOC_HX5_NUM_UCAST_QUEUE_PER_ST_PORT 26
/* Number of Multicast Queues per HG/Stacking  Port 
 Queues 0 - 7 for transit  traffic and 8 - 15 for wireless traffic, 16 - 23 is for wired traffic */ 
#define SOC_HX5_NUM_MCAST_QUEUE_PER_ST_PORT 26
/* Number of Scheduler Nodes per HG/Stacking Port */
#define SOC_HX5_NUM_SCHEDULER_PER_ST_PORT   33
#define SOC_HX5_NUM_SCHEDULER_PER_CPU_PORT  12

#define SOC_HX5_MMU_MCQ_ENTRY_PER_XPE    24576
#define SOC_HX5_MMU_RQE_ENTRY_PER_XPE    2048
#define _HX5_MMU_RQE_ENTRY_RSVD_PER_XPE  242   /* Reserved RQE QE per XPE */
#define _HX5_MAX_MMU_MCQE_PER_XPE        1338  /* MAX MMU MC QE */

#define _HX5_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_HX5_MMU_MCQ_ENTRY_PER_XPE
#define _HX5_MMU_TOTAL_RQE_ENTRY(unit)   SOC_HX5_MMU_RQE_ENTRY_PER_XPE

#define _HX5_TDM_MCQE_RSVD_OVERSHOOT     12   /* Reserved MC egress queue entry
                                                 for TDM overshoot */
#define _HX5_MCQE_RSVD_PER_MCQ           6    /* Reserved MC egress queue entry
                                                 per MC queue */

/* Per pipe WRED config memory base index */
#define _HX5_WRED_PER_PIPE_UC_QUEUES_BASE  0
#define _HX5_WRED_PER_PIPE_PORT_SP_BASE    1280  /* (16 * 26) UPlink + (54 * 16) Dw link */
#define _HX5_WRED_PER_PIPE_GLOBAL_SP_BASE  1560  /* (4 global service pools) */

typedef struct _soc_hx5_mmu_sw_config_info_s {
    uint32  mmu_egr_queue_min;  /* For CPU and loopback port */
    uint32  mmu_egr_qgrp_min;
    uint32  mmu_total_pri_groups;
    uint32  mmu_active_pri_groups;
    uint32  mmu_pg_min;
    uint32  mmu_port_min;
    uint32  mmu_mc_egr_qentry_min;
    uint32  mmu_rqe_qentry_min;
    uint32  mmu_rqe_queue_min;
    int     ing_shared_total;
    int     egr_shared_total;
} _soc_hx5_mmu_sw_config_info_t;

extern int
soc_helix5_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count);
extern void
soc_helix5_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int
soc_helix5_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout);

extern void soc_helix5_port_qsgmii_mode_get(int unit, soc_port_t port, int* qsgmii_mode);
extern void soc_helix5_sbus_ring_map_config(int unit);

/************ FlexPort Start ********************/
extern int soc_hx5_mmu_config_init_port(int unit, int port);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_hx5_flexport_scache_allocate(int unit);
extern int soc_hx5_flexport_scache_sync(int unit);
extern int soc_hx5_flexport_scache_recovery(int unit);
extern int soc_hx5_tdm_scache_allocate(int unit);
extern int soc_hx5_tdm_scache_sync(int unit);
extern int soc_hx5_tdm_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
soc_hx5_support_speeds(int unit, int port, uint32 *speed_mask);

/************ FlexPort End ********************/

extern int
soc_hx5_lpm_flex_ip4entry0_to_1(
    int u,
    void *src,
    void *dst,
    uint32 key_type,
    int copy_hit);

extern int
soc_hx5_lpm_flex_prefix_length_get(
    int u,
    void *entry,
    int *ipv6,
    int *vrf_id,
    int *pfx,
    uint32 key_type);

extern int soc_hx5_reg_blk_index_get(int unit, soc_reg_t reg, soc_port_t port,
                                          pbmp_t *bm, int *block, int *index, int invalid_blk_check);
extern int
soc_hx5_mpls_hash_control_set(int *hash_entry);

extern int
soc_helix5_show_voltage(int unit);

extern int
soc_helix5_icid_get(int unit, int max_size, uint8 *buffer, int *actual_size);

extern int
soc_helix5_show_icid(int unit);

#ifdef INCLUDE_AVS
extern soc_error_t
soc_hx5_avs_init(int unit);
#endif /* INCLUDE_AVS */

#endif /* !_SOC_HELIX5_H_ */
