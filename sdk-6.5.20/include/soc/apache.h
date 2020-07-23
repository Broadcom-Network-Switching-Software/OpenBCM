/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        apache.h
 */

#ifndef _SOC_APACHE_H_
#define _SOC_APACHE_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/esw/port.h>

#define SOC_APACHE_NUM_EL_L2                             4
#define SOC_APACHE_NUM_EL_SHARED                         4
#define SOC_APACHE_NUM_EL_L3                             4
#define SOC_APACHE_NUM_EL_ALPM                           4
#define SOC_APACHE_NUM_EL_VLAN_XLATE                     4
#define SOC_APACHE_NUM_EL_EGR_VLAN_XLATE                 4
#define SOC_APACHE_NUM_EL_ING_L3_NEXT_HOP(unit) \
        SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureL3NHop8k) ? 2 : 8
#define SOC_APACHE_NUM_EL_L3_IPMC(unit) \
        SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureIpmc4k) ? 2 : 4

#define SOC_APACHE_NUM_EL_L2MC                           6

#define SOC_APACHE_RAM_OFFSET_L3_NARROW               8192
#define SOC_APACHE_RAM_OFFSET_L3_WIDE                 4096
#define SOC_APACHE_RAM_OFFSET_L3_DOUBLE_WIDE          2048

#define SOC_APACHE_RAM_OFFSET_ING_L3_NEXT_HOP         4096
#define SOC_APACHE_RAM_OFFSET_L3_IPMC                 2048
#define SOC_APACHE_RAM_OFFSET_L2MC                    4096

#define SOC_APACHE_L3_DEFIP_ALPM_PER_BKT_COUNT           1
#define SOC_APACHE_L3_DEFIP_ALPM_IPV6_128_PER_BKT_COUNT  2
#define SOC_APACHE_L3_DEFIP_ALPM_IPV6_64_1_PER_BKT_COUNT 3
#define SOC_APACHE_L3_DEFIP_ALPM_IPV6_64_PER_BKT_COUNT   4
#define SOC_APACHE_L3_DEFIP_ALPM_IPV4_1_PER_BKT_COUNT    4
#define SOC_APACHE_L3_DEFIP_ALPM_IPV4_PER_BKT_COUNT      6

#define SOC_APACHE_NUM_BKTS_PER_L2_BANK               2048
#define SOC_APACHE_NUM_BKTS_PER_L3_BANK                256
#define SOC_APACHE_NUM_BKTS_PER_SHARED_BANK          16384

#define SOC_APACHE_NUM_ENTRIES_L2_BANK               16384
#define SOC_APACHE_NUM_ENTRIES_L3_BANK                4096
#define SOC_APACHE_NUM_ENTRIES_L3_BANK_WIDE           8192
#define SOC_APACHE_NUM_ENTRIES_L3_BANK_DOUBLE_WIDE    4096

#define SOC_APACHE_ALPM_MAX_BKTS                     16384
#define SOC_APACHE_ALPM_BKT_OFFFSET                   2048
#define SOC_APACHE_ALPM_BKT_MASK                    0x3FFF

#define _SOC_APACHE_L2LU_INTR_MASK                 0x01000
#define _SOC_APACHE_FUNC_INTR_MASK _SOC_APACHE_L2LU_INTR_MASK

typedef enum soc_apache_port_ct_speed_e {
    SOC_APACHE_PORT_CT_SPEED_NONE = 0, /* Always First */
    SOC_APACHE_PORT_CT_SPEED_10M_FULL,
    SOC_APACHE_PORT_CT_SPEED_100M_FULL,
    SOC_APACHE_PORT_CT_SPEED_1000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_2500M_FULL,
    SOC_APACHE_PORT_CT_SPEED_5000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_10000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_11000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_20000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_21000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_25000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_27000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_40000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_42000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_50000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_53000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_100000M_FULL,
    SOC_APACHE_PORT_CT_SPEED_106000M_FULL
} soc_apache_port_ct_speed_t;

#define SOC_APACHE_NUM_MMU_PORTS_PER_PIPE               75

#define _APACHE_MMU_NUM_POOL                            4
#define _APACHE_MMU_NUM_RQE_QUEUES                     11



#define _AP_PORTS_PER_TSC         4
#define _AP_TSCS_PER_PGW          9
#define _AP_NUM_XLP_PER_PGW       7
#define _AP_NUM_CLP_PER_PGW       2
#define _AP_PGWS_PER_DEV          2

#define _AP_PORTS_PER_PGW         (_AP_PORTS_PER_TSC * _AP_TSCS_PER_PGW)
#define _AP_PORTS_PER_PIPE        (_AP_PORTS_PER_PGW * _AP_PGWS_PER_DEV)

/*
 * TDM algo does not support all speeds. SDK needs to adjust
 * the speeds to  what is supported in the algorithm.
 *
 * For FB5 LR ports:
 * Anything < 1G will be passed as 1G.
 * 5G needs to be passed as 10G.
 * 2.5G is supported in the TDM. We can pass as is.
 *
 * For OS and MV LR ports:
 * Anything < 10G will be passed as 10G
 */
#define APACHE_TDM_SPEED_ADJUST(unit, port, speed, oversub) \
    do {                                                    \
        speed = SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed);     \
        if ((speed > 0) && (speed < 10000)) {               \
            if (SOC_INFO(unit).oversub_mode != 2) {         \
                speed = 10000;                              \
            } else if (oversub) {                           \
                speed = 10000;                              \
            } else if (speed < 1000) {                      \
                speed = 1000;                               \
            } else if (speed == 5000) {                     \
                speed = 10000;                              \
            }                                               \
        }                                                   \
    } while (0)

#define AP_PHY_PORT_CPU           0
#define AP_PHY_PORT_LB            73
#define AP_PHY_PORT_RDB           74

typedef enum {
    SOC_APACHE_SCHED_MODE_UNKNOWN = 0,
    SOC_APACHE_SCHED_MODE_STRICT,
    SOC_APACHE_SCHED_MODE_WRR,
    SOC_APACHE_SCHED_MODE_WDRR
} soc_apache_sched_mode_e;

typedef enum {
    SOC_APACHE_NODE_LVL_ROOT = 0,
    SOC_APACHE_NODE_LVL_S1,
    SOC_APACHE_NODE_LVL_L0,
    SOC_APACHE_NODE_LVL_L1,
    SOC_APACHE_NODE_LVL_L2,
    SOC_APACHE_NODE_LVL_MAX
} soc_apache_node_lvl_e;

typedef enum {
    _SOC_APACHE_INDEX_STYLE_BUCKET,
    _SOC_APACHE_INDEX_STYLE_BUCKET_MODE,
    _SOC_APACHE_INDEX_STYLE_WRED,
    _SOC_APACHE_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_APACHE_INDEX_STYLE_SCHEDULER,
    _SOC_APACHE_INDEX_STYLE_PERQ_XMT,
    _SOC_APACHE_INDEX_STYLE_UCAST_DROP,
    _SOC_APACHE_INDEX_STYLE_UCAST_QUEUE,
    _SOC_APACHE_INDEX_STYLE_MCAST_QUEUE,
    _SOC_APACHE_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_APACHE_INDEX_STYLE_EGR_POOL,
    _SOC_APACHE_INDEX_STYLE_COUNT
} soc_apache_index_style_t;

typedef enum soc_apache_drop_limit_alpha_value_e {
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */                                          
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_apache_drop_limit_alpha_value_t;

extern soc_functions_t soc_apache_drv_funs;
extern soc_functions_t soc_monterey_drv_funs;
extern void soc_apache_ser_set_long_delay(int delay);
extern int _soc_apache_mem_ser_control(int unit, soc_mem_t mem,
                                         int copyno, int enable);
extern int _soc_apache_mem_cpu_write_control(int unit, soc_mem_t mem,
                                               int copyno, int enable,
                                               int *orig_enable);
extern int soc_apache_ser_mem_clear(int unit, soc_mem_t mem);
extern void _soc_apache_alpm_bkt_view_set(int unit, int index, soc_mem_t view);
extern soc_mem_t _soc_apache_alpm_bkt_view_get(int unit, int index);
extern void soc_apache_sbus_ring_map_config(int unit);
extern int soc_apache_mem_config(int unit);
extern int _soc_apache_mem_sram_info_get(int unit, soc_mem_t mem, int index, 
                                           _soc_ser_sram_info_t *sram_info);
extern int soc_apache_pipe_select(int unit, int egress, int pipe);
extern int soc_apache_num_cosq_init(int unit);
extern int soc_apache_tsc_map_get(int unit, uint32 *tsc_map, uint32 *force_hg);
extern int soc_apache_port_config_init(int unit, uint16 dev_id, uint8 rev_id);
extern int soc_apache_port_config_bandwidth_check(int unit);
extern soc_error_t
soc_apache_port_reg_blk_index_get(int unit ,int port, soc_block_type_t blktype, int *block);
extern int soc_apache_lls_bmap_alloc(int unit);
extern int soc_apache_chip_reset(int unit);
extern int soc_apache_tsc_reset(int unit);
extern int soc_apache_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_apache_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_apache_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem);
extern int soc_apache_mem_is_eligible_for_scan(int unit, soc_mem_t mem);
extern int soc_apache_ser_test_skip_check(int unit, soc_mem_t mem);
extern int soc_ap_is_skip_default_lls_creation(int unit);
extern void _soc_apache_ser_process_rdb(int unit);
extern void
soc_apache_top_intr(void *unit_vp, void *d1, void *d2,
                                   void *d3, void *d4);
extern int soc_apache_temperature_monitor_get(int unit,
               int temperature_max,
               soc_switch_temperature_monitor_t *temperature_array,
               int *temperature_count);
extern int soc_apache_show_material_process(int unit);
extern int soc_apache_show_ring_osc(int unit);
extern int soc_apache_show_voltage(int unit);
extern int soc_apache_bond_info_init(int unit);

enum soc_apache_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_APACHE_PORT_MODE_QUAD = 0,
    SOC_APACHE_PORT_MODE_TRI_012 = 1,
    SOC_APACHE_PORT_MODE_TRI_023 = 2,
    SOC_APACHE_PORT_MODE_DUAL = 3,
    SOC_APACHE_PORT_MODE_SINGLE = 4
};

extern int      soc_apache_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_apache_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_apache_hash_bank_number_get(int unit, soc_mem_t mem,
                                                  int seq_num, int *bank_num);
extern int      soc_apache_hash_seq_number_get(int unit, soc_mem_t mem, 
                                                  int bank_num, int *seq_num);
extern int      soc_apache_hash_bank_info_get(int unit, soc_mem_t mem,
                                                int bank,
                                                int *entries_per_bank,
                                                int *entries_per_row,
                                                int *entries_per_bucket,
                                                int *bank_base,
                                                int *bucket_offset);
extern int      soc_ap_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                        int *hash_offset, int *use_lsb);
extern int      soc_ap_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                       int hash_offset, int use_lsb);
extern uint32   soc_ap_l2x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key);
extern int      soc_ap_l2x_base_entry_to_key(int unit, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_ap_l2x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_ap_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_ap_l3x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key);
extern int      soc_ap_l3x_base_entry_to_key(int unit, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_ap_l3x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_ap_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_ap_mpls_hash(int unit, int hash_sel, int key_nbits,
                                  void *base_entry, uint8 *key);
extern int      soc_ap_mpls_base_entry_to_key(int unit, void *entry,
                                               uint8 *key);
extern uint32   soc_ap_mpls_entry_hash(int unit, int hash_sel, int bank,
                                       uint32 *entry);
extern uint32   soc_ap_mpls_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_ap_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                                        void *base_entry, uint8 *key);
extern int      soc_ap_vlan_xlate_base_entry_to_key(int unit, void *entry,
                                                     uint8 *key);
extern uint32   soc_ap_vlan_xlate_entry_hash(int unit, int hash_sel, int bank,
                                              uint32 *entry);
extern uint32   soc_ap_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32   soc_ap_egr_vlan_xlate_hash(int unit, int hash_sel,
                                            int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_ap_egr_vlan_xlate_base_entry_to_key(int unit, void *entry,
                                                         uint8 *key);
extern uint32   soc_ap_egr_vlan_xlate_entry_hash(int unit, int hash_sel, 
                                                 int bank, uint32 *entry);
extern uint32   soc_ap_egr_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                       uint32 *entry);
extern uint32   soc_ap_ing_vp_vlan_member_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry, uint8 *key);
extern int      soc_ap_ing_vp_vlan_member_base_entry_to_key(int unit,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_ap_ing_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                     int bank, uint32 *entry);
extern uint32   soc_ap_egr_vp_vlan_member_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry, uint8 *key);
extern int      soc_ap_egr_vp_vlan_member_base_entry_to_key(int unit,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_ap_egr_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                     int bank, uint32 *entry);
extern uint32   soc_ap_ing_dnat_address_type_hash(int unit, int hash_sel,
                                                   int key_nbits,
                                                   void *base_entry,
                                                   uint8 *key);
extern int      soc_ap_ing_dnat_address_type_base_entry_to_key(int unit,
                                                                void *entry,
                                                                uint8 *key);
extern uint32   soc_ap_ing_dnat_address_type_entry_hash(int unit,
                                                         int hash_sel,
                                                         int bank,
                                                         uint32 *entry);
extern uint32   soc_ap_l2_endpoint_id_hash(int unit, int hash_sel,
                                            int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_ap_l2_endpoint_id_base_entry_to_key(int unit, void *entry,
                                                         uint8 *key);
extern uint32   soc_ap_l2_endpoint_id_entry_hash(int unit, int hash_sel,
                                                 int bank,
                                                  uint32 *entry);
extern uint32   soc_ap_endpoint_queue_map_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry,
                                                uint8 *key);
extern int      soc_ap_endpoint_queue_map_base_entry_to_key(int unit,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_ap_endpoint_queue_map_entry_hash(int unit, int hash_sel,
                                                     int bank,
                                                      uint32 *entry);
extern int      soc_ap_hash_bucket_get(int unit, int mem, int bank,
                                    uint32 *entry, uint32 *bucket);
extern int      soc_ap_hash_index_get(int unit, int mem, int bank, int bucket);

extern int 
soc_apache_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                       int *pbase, int *pnumq);

#define SOC_APACHE_PORT_PIPE(u, p) 0

extern uint32 _soc_apache_mmu_port(int unit, int port);

#define SOC_APACHE_MMU_PORT(u, p)  _soc_apache_mmu_port((u),(p))

extern uint32 _soc_apache_piped_mem_index(int unit, soc_port_t port,
                                 soc_mem_t mem, int arr_off);

#define SOC_APACHE_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    _soc_apache_piped_mem_index((u),(p),(m),(off))

#define _SOC_APACHE_NODE_CONFIG_MEM(u,p,n)                                 \
    (((n) == SOC_APACHE_NODE_LVL_S1) ? LLS_S1_CONFIGm :                    \
     (((n) == SOC_APACHE_NODE_LVL_L0) ? LLS_L0_CONFIGm :                   \
      ((n) == SOC_APACHE_NODE_LVL_L1 ? LLS_L1_CONFIGm : -1  )))

#define _SOC_APACHE_NODE_CONFIG_MEM2(u,p,n) _SOC_APACHE_NODE_CONFIG_MEM(u, p, n)

#define _SOC_APACHE_NODE_PARENT_MEM(u,p,n)                                 \
    (((n)==SOC_APACHE_NODE_LVL_L2) ? LLS_L2_PARENTm :                      \
     (((n) == SOC_APACHE_NODE_LVL_L1) ? LLS_L1_PARENTm :                   \
      (((n) == SOC_APACHE_NODE_LVL_L0) ? LLS_L0_PARENTm :                  \
       (((n) == SOC_APACHE_NODE_LVL_S1) ? LLS_S1_PARENTm : -1  ))))

#define _SOC_APACHE_NODE_WIEGHT_MEM(u,p,n)                                 \
    (((n)==SOC_APACHE_NODE_LVL_S1) ? LLS_S1_CHILD_WEIGHT_CFG_CNTm :        \
     (((n)==SOC_APACHE_NODE_LVL_L0) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm :       \
     (((n) == SOC_APACHE_NODE_LVL_L1) ? LLS_L1_CHILD_WEIGHT_CFG_CNTm :     \
      (((n) == SOC_APACHE_NODE_LVL_L2) ? LLS_L2_CHILD_WEIGHT_CFG_CNTm : -1 ))))


typedef enum {
    SOC_APACHE_SCHED_UNKNOWN   = 0,
    SOC_APACHE_SCHED_LLS,
    SOC_APACHE_SCHED_HSP
} soc_apache_sched_type_t;

extern soc_apache_sched_type_t
_soc_apache_port_sched_type_get(int unit, soc_port_t port);

extern int
_soc_apache_invalid_parent_index(int unit, int level);

extern int
soc_apache_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                            int *hw_index);

extern int soc_apache_l2_hw_index(int unit, int qnum, int uc);
extern int soc_apache_hw_index_logical_num(int unit, int port,
                                                int index, int uc);
extern int
soc_apache_cosq_set_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int num_spri, int first_sp_child, 
                              int first_sp_mc_child, uint32 ucmap);

extern int
soc_apache_cosq_set_sched_mode(int unit, soc_port_t port, 
        int level, int index,
        soc_apache_sched_mode_e mode, int weight);

extern int
soc_apache_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_psri, int first_sp_child, 
                              int first_sp_mc_child, uint32 ucmap, uint32 spmap,
                              soc_apache_sched_mode_e mode, int weight);

extern int
soc_apache_cosq_get_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int *pnum_spri, int *pfirst_sp_child, 
                              int *pfirst_sp_mc_child, uint32 *pucmap, uint32 *spmap);

extern int
soc_apache_cosq_get_sched_mode(int unit, soc_port_t port, int level, 
                            int index, soc_apache_sched_mode_e *pmode, int *weight);

extern int _soc_apache_root_scheduler_index(int unit, int port);
extern int
soc_apache_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *num_spri, int *first_sp_child, 
                              int *first_sp_mc_child, uint32 *ucmap, uint32 *spmap,
                              soc_apache_sched_mode_e *mode, int *weight);

extern int 
soc_apache_get_child_type(int unit, soc_port_t port, int level, int *child_type);

extern int 
soc_apache_sched_weight_get(int unit, int port, int level, int index, int *weight);

extern int 
soc_apache_sched_weight_set(int unit, int port, int level, int index, int weight);

extern int soc_apache_get_pipe_queue_count(int unit, int uc, int xpipe);

extern int soc_apache_dump_port_lls(int unit, int port);

extern int soc_apache_dump_port_hsp(int unit, int port);

extern int soc_apache_lls_init(int unit);

extern int soc_apache_lb_lls_init(int unit);

extern int soc_apache_cosq_set_sched_parent(int unit, soc_port_t port,
                              int level, int hw_index,
                              int parent_hw_idx, int add);

extern int soc_apache_logical_qnum_hw_qnum(int unit, int port, int lqnum, int uc);

extern int soc_apache_mmu_get_shared_size(int unit, int *shared_size);
extern int soc_apache_mmu_config_shared_buf_recalc(int unit,
                                                uint32 spid,
                                                int shared_size,
                                                int flags);
extern int soc_apache_mmu_config_res_limits_update(int unit,
                                                uint32 spid,
                                                uint32 shared_size,
                                                int flags);

extern void soc_apache_process_func_intr(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);

typedef int (*_soc_apache_bst_hw_cb)(int unit);
extern int soc_apache_set_bst_callback(int unit, _soc_apache_bst_hw_cb cb);
extern int soc_apache_port_asf_para_set(int unit, soc_port_t port, int speed,
                                     int enable);
extern int soc_apache_port_speed_update(int unit, soc_port_t port, int speed);
extern int soc_apache_port_thdo_rx_enable_set(int unit, soc_port_t port,
                                                int enable);
extern int
soc_apache_idb_buf_reset(int unit, soc_port_t port, int reset);

extern int
soc_apache_edb_buf_reset(int unit, soc_port_t port, int reset);

extern int soc_apache_fc_map_shadow_create(int unit);
extern void soc_apache_fc_map_shadow_free(int unit);
extern int
soc_apache_fc_map_shadow_entry_set (int unit, soc_mem_t mem, int index,
                                      void* entry_data);
extern int
soc_apache_fc_map_shadow_entry_get (int unit, soc_mem_t mem, int index,
                                      void* entry_data);
int soc_apache_fc_map_shadow_clear (int unit, soc_mem_t mem);
extern int soc_apache_port_traffic_egr_enable_set(int unit, int port, int enable);
extern int soc_apache_port_mmu_tx_enable_set(int unit, int port, int enable);
extern int soc_apache_qcn_counter_hw_index_get(int unit, soc_port_t port,
                                            int index, int *qindex);
extern int
soc_apache_setup_hsp_port(int unit, int port);
extern int
soc_apache_delete_hsp_port(int unit, int port);
extern int
soc_apache_mmu_config_init(int unit, int test_only, soc_port_t port);
extern int
soc_apache_cosq_enable_hsp_sched(int unit, soc_port_t port);
extern soc_error_t
soc_apache_cosq_disable_hsp_sched(int unit, soc_port_t port);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_apache_fc_map_shadow_size_get (int unit, uint32* size);
extern int soc_apache_fc_map_shadow_sync(int unit, uint32 **sync_addr);
extern int soc_apache_fc_map_shadow_load(int unit, uint32 **load_addr);
#endif

#if defined(SER_TR_TEST_SUPPORT)
extern soc_error_t soc_apache_ser_inject_error(int unit, uint32 flags,
                                            soc_mem_t mem, int pipe_target,
                                            int block, int index);
extern soc_error_t soc_apache_ser_test_mem(int unit, soc_mem_t mem,
                                        _soc_ser_test_t test_type, int cmd);
extern soc_error_t soc_apache_ser_test(int unit, _soc_ser_test_t test_type);
extern soc_error_t soc_apache_ser_error_injection_support(int unit,
                                                       soc_mem_t mem, int pipe);
extern int soc_apache_ser_test_overlay (int unit, _soc_ser_test_t test_type);
extern int soc_apache_cosq_max_bucket_set(int unit, int port,
                                        int index, uint32 level);
extern int ser_test_apache_mem_index_remap(int unit,
                                           ser_test_data_t *test_data,
                                           int *mem_has_ecc);
#endif /*#if defined(SER_TR_TEST_SUPPORT)*/

/*
 * Data structure use for storing part of the SOC data information
 * which is needed during later phases of a FlexPort operation.
 */
typedef struct soc_ap_info_s {
    int port_l2p_mapping[SOC_MAX_NUM_PORTS];      /* Logic to physical */
    int port_p2l_mapping[SOC_MAX_NUM_PORTS];      /* Physical to logic */
    int port_p2m_mapping[SOC_MAX_NUM_PORTS];      /* Physical to MMU */
    int port_m2p_mapping[SOC_MAX_NUM_MMU_PORTS];  /* MMU to physical */
    int port_group[SOC_MAX_NUM_PORTS];            /* Group number */
    int port_speed_max[SOC_MAX_NUM_PORTS];        /* Max port speed */
    int port_init_speed[SOC_MAX_NUM_PORTS];       /* Init port speed */
    int port_num_lanes[SOC_MAX_NUM_PORTS];        /* Number of lanes */
    pbmp_t xpipe_pbm;                             /* Ports in X-pipe */
    pbmp_t ypipe_pbm;                             /* Ports in Y-pipe */
    pbmp_t oversub_pbm;                           /* Oversubscription ports */
} soc_ap_info_t;

extern int
soc_apache_port_mode_get(int unit, int logical_port, int *mode);

extern int
soc_apache_is_cut_thru_enabled(int unit, soc_port_t port, int *enable);

extern int
soc_ap_cut_thru_enable_disable(int unit, soc_port_t port, int enable);

extern int
soc_ap_phy_info_init(int unit);
extern int
soc_ap_phy_info_deinit(int unit);

extern int
soc_apache_port_cl91_status_get(int unit, soc_port_t port, uint32 *cl91_enabled);
extern int
soc_ap_port_lanes_valid(int unit, soc_port_t port, int lanes);

extern int
soc_ap_port_resource_speed_max_get(int unit, int *speed);

extern soc_error_t
_soc_apache_mmu_ovs_speed_class_map_get(int unit, int *speed, int *spg, uint32 *sp_spacing);
extern int
soc_ap_port_ovs_speed_group_validate(int unit);
extern int
soc_ap_port_mixed_sister_speed_validate(int unit);

extern int
soc_ap_port_addressable(int unit, soc_port_t port);

extern int
soc_ap_phy_port_addressable(int unit, int phy_port);

extern int
soc_ap_edb_buf_reset(int unit, soc_port_t port, int reset);

extern int
soc_ap_port_resource_validate(int unit, int nport,
                              soc_port_resource_t *resource);
extern int
soc_ap_port_resource_configure(int unit, int nport,
                               soc_port_resource_t *resource, int flag);

extern int
soc_ap_port_resource_tdm_set(int unit, int curr_port_info_size,
                             soc_port_resource_t *curr_port_info,
                             int new_port_info_size,
                             soc_port_resource_t *new_port_info,
                             soc_ap_info_t *si, int lossless);

extern int
soc_ap_port_resource_pgw_set(int unit,
                             int pre_num_res,  soc_port_resource_t *pre_res,
                             int post_num_res, soc_port_resource_t *post_res,
                             int lossless);

extern int
soc_ap_port_resource_ip_set(int unit, int curr_port_info_size,
                            soc_port_resource_t *curr_port_info,
                            int new_port_info_size,
                            soc_port_resource_t *new_port_info,
                            soc_ap_info_t *si);

extern int
soc_ap_port_resource_ep_set(int unit, int curr_port_info_size,
                            soc_port_resource_t *curr_port_info,
                            int new_port_info_size,
                            soc_port_resource_t *new_port_info,
                            soc_ap_info_t *si);
extern int
soc_ap_port_resource_mmu_mapping_set(int unit, int nport,
                                     soc_port_resource_t *resource);

extern int
soc_ap_port_resource_mmu_set(int unit, int nport,
                             soc_port_resource_t * flexport_data);

extern int
soc_apache_port_asf_set(int unit, soc_port_t port, int speed);
extern int
soc_apache_port_asf_speed_set(int unit, soc_port_t port, int speed);

extern int
soc_apache_port_lane_config_get(int unit, int phy_port, int *mode_lane,
                              int *found);
extern int
soc_apache_port_autoneg_core_get(int unit, int phy_port, int *an_core,
                               int *found);
extern int
soc_apache_mmu_hsp_port_reserve(int unit, int port, int speed_max);

extern int
soc_apache_tsc_disable(int unit);

/* 
 * FlexPort Functions
 */

extern int soc_apache_is_flex_enable(int unit);
extern soc_error_t
soc_apache_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp);

extern int
soc_apache_mmu_port_resource_set(int unit, soc_port_t port, int speed);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_apache_flexport_scache_allocate(int unit);
extern int soc_apache_flexport_scache_sync(int unit);
extern int soc_apache_flexport_scache_recovery(int unit);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void soc_apache_flexport_sw_dump(int unit);
#endif

#ifdef PORTMOD_SUPPORT
extern int soc_ap_portctrl_pm_portmod_init(int unit);
extern int soc_ap_portctrl_pm_portmod_deinit(int unit);
extern int soc_ap_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config_info);
extern int soc_ap_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
extern int soc_ap_portctrl_port_ability_update(int unit, soc_port_t port, soc_port_ability_t *ability);
extern int soc_ap_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);
#endif

extern int soc_apache_alpm_mode_get(int unit);
extern int soc_apache_get_alpm_banks(int unit);
extern int soc_apache_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_apache_l3_defip_index_remap(int unit, soc_mem_t mem, int index);

typedef int (*_soc_apache_lls_traverse_cb)(int unit, int port, int level, 
        int hw_index, void *cookie);

#define PORT_IS_PM4x25_PORT_PHY(unit, phy_port) \
            (((phy_port <= 36) && (phy_port >= 29)) || \
             ((phy_port <= 72) && (phy_port >= 65)))

#define PORT_IS_PM4x25_PORT(unit, lport) \
    PORT_IS_PM4x25_PORT_PHY(unit, (SOC_INFO(unit).port_l2p_mapping[lport]))

#define _SOC_AP_L2LU_INTR_MASK                 0x2000

#endif  /* !_SOC_APACHE_H_ */

/*test*/
