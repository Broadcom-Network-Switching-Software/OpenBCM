/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        monterey.h
 */

#ifndef _SOC_MONTEREY_H_
#define _SOC_MONTEREY_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/esw/port.h>
#include <include/bcm/cosq.h> 
#define MONTEREY_PORTS_PER_TSC         4
#define MONTEREY_TSCS_PER_PGW          8
#define MONTEREY_NUM_XLP_PER_PGW       3
#define MONTEREY_NUM_CLP_PER_PGW       5

#define MONTEREY_PGWS_PER_DEV          2
#define MONTEREY_PGW0                  0
#define MONTEREY_PGW1                  1

#define MONTEREY_PORTS_PER_PGW         (MONTEREY_PORTS_PER_TSC * MONTEREY_TSCS_PER_PGW)
#define MONTEREY_PORTS_PER_PIPE        (MONTEREY_PORTS_PER_PGW * MONTEREY_PGWS_PER_DEV)

#define SOC_MONTEREY_NUM_EL_L2                             1
#define SOC_MONTEREY_NUM_EL_SHARED                         0
#define SOC_MONTEREY_NUM_EL_ALPM                           0
#define SOC_MONTEREY_NUM_EL_VLAN_XLATE                     4
#define SOC_MONTEREY_NUM_EL_EGR_VLAN_XLATE                 4
#define SOC_MONTEREY_NUM_EL_ING_L3_NEXT_HOP                1
#define SOC_MONTEREY_NUM_EL_L3_IPMC                        4
#define SOC_MONTEREY_NUM_EL_L2MC                           4

#define SOC_MONTEREY_RAM_OFFSET_L2_BANK                 32768                 
#define SOC_MONTEREY_RAM_OFFSET_L2_SHARED_BANK           0

#define SOC_MONTEREY_RAM_OFFSET_L3_NARROW               8192
#define SOC_MONTEREY_RAM_OFFSET_L3_WIDE                 4096
#define SOC_MONTEREY_RAM_OFFSET_L3_DOUBLE_WIDE          2048

#define SOC_MONTEREY_RAM_OFFSET_ING_L3_NEXT_HOP         5120
#define SOC_MONTEREY_RAM_OFFSET_L3_IPMC                 1024
#define SOC_MONTEREY_RAM_OFFSET_L2MC                    1024

#define SOC_MONTEREY_L3_DEFIP_ALPM_PER_BKT_COUNT           1
#define SOC_MONTEREY_L3_DEFIP_ALPM_IPV6_128_PER_BKT_COUNT  2
#define SOC_MONTEREY_L3_DEFIP_ALPM_IPV6_64_1_PER_BKT_COUNT 3
#define SOC_MONTEREY_L3_DEFIP_ALPM_IPV6_64_PER_BKT_COUNT   4
#define SOC_MONTEREY_L3_DEFIP_ALPM_IPV4_1_PER_BKT_COUNT    4
#define SOC_MONTEREY_L3_DEFIP_ALPM_IPV4_PER_BKT_COUNT      6

#define SOC_MONTEREY_NUM_ENTRIES_PER_L2_BANK           32768
#define SOC_MONTEREY_NUM_ENTRIES_L2_BANK               32768
#define SOC_MONTEREY_NUM_ENTRIES_L3_BANK               8192
#define SOC_MONTEREY_NUM_ENTRIES_L3_BANK_WIDE          4096
#define SOC_MONTEREY_NUM_ENTRIES_L3_BANK_DOUBLE_WIDE   2048

#define SOC_MONTEREY_ALPM_MAX_BKTS                     16384
#define SOC_MONTEREY_ALPM_BKT_OFFFSET                   2048
#define SOC_MONTEREY_ALPM_BKT_MASK                    0x3FFF

#define _SOC_MONTEREY_L2LU_INTR_MASK                 0x01000
#define _SOC_MONTEREY_FUNC_INTR_MASK _SOC_MONTEREY_L2LU_INTR_MASK
#define SOC_MONTEREY_TS_PLL_N1 768
#define SOC_MONTEREY_TS_PLL_N2 768 
#define SOC_MONTEREY_TS_PLL_M1 6544
#define SOC_MONTEREY_TS_PLL_M2 6545 
#define SOC_MONTEREY_TS_PLL_PMAX1 768
#define SOC_MONTEREY_TS_PLL_PMAX2 768   

typedef enum soc_monterey_port_ct_speed_e {
    SOC_MONTEREY_PORT_CT_SPEED_NONE = 0, /* Always First */
    SOC_MONTEREY_PORT_CT_SPEED_10M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_100M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_1000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_2500M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_5000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_10000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_11000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_20000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_21000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_25000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_27000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_40000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_42000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_50000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_53000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_100000M_FULL,
    SOC_MONTEREY_PORT_CT_SPEED_106000M_FULL
} soc_monterey_port_ct_speed_t;

#define SOC_MONTEREY_NUM_MMU_PORTS_PER_PIPE              67 

#define _MONTEREY_MMU_NUM_POOL                            4
#define _MONTEREY_MMU_NUM_RQE_QUEUES                     11

#define _MN_PORTS_PER_TSC         4
#define _MN_TSCS_PER_PGW          8
#define _MN_PORTS_PER_PGW         (_MN_PORTS_PER_TSC * _MN_TSCS_PER_PGW)

#define MN_PHY_PORT_CPU           0
#define MN_PHY_PORT_LB            65 
#define MN_PHY_PORT_MACSEC          66 
typedef enum {
    SOC_MONTEREY_SCHED_MODE_UNKNOWN = 0,
    SOC_MONTEREY_SCHED_MODE_STRICT,
    SOC_MONTEREY_SCHED_MODE_WRR,
    SOC_MONTEREY_SCHED_MODE_WDRR
} soc_monterey_sched_mode_e;

typedef enum {
    SOC_MONTEREY_NODE_LVL_ROOT = 0,
    SOC_MONTEREY_NODE_LVL_S1,
    SOC_MONTEREY_NODE_LVL_L0,
    SOC_MONTEREY_NODE_LVL_L1,
    SOC_MONTEREY_NODE_LVL_L2,
    SOC_MONTEREY_NODE_LVL_MAX
} soc_monterey_node_lvl_e;

typedef enum {
    _SOC_MONTEREY_INDEX_STYLE_BUCKET,
    _SOC_MONTEREY_INDEX_STYLE_BUCKET_MODE,
    _SOC_MONTEREY_INDEX_STYLE_WRED,
    _SOC_MONTEREY_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_MONTEREY_INDEX_STYLE_SCHEDULER,
    _SOC_MONTEREY_INDEX_STYLE_PERQ_XMT,
    _SOC_MONTEREY_INDEX_STYLE_UCAST_DROP,
    _SOC_MONTEREY_INDEX_STYLE_UCAST_QUEUE,
    _SOC_MONTEREY_INDEX_STYLE_MCAST_QUEUE,
    _SOC_MONTEREY_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_MONTEREY_INDEX_STYLE_EGR_POOL,
    _SOC_MONTEREY_INDEX_STYLE_COUNT
} soc_monterey_index_style_t;

typedef enum soc_monterey_drop_limit_alpha_value_e {
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */                                          
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_MONTEREY_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_monterey_drop_limit_alpha_value_t;
int _soc_monterey_obm_interrupt_cb(int unit);
int
_soc_monterey_obm_cb_unregister(int unit , bcm_obm_callback_fn  fn , void *fn_data);
int
_soc_monterey_obm_cb_register(int unit , bcm_obm_callback_fn fn , void *fn_data);
extern int 
soc_mn_port_ovs_speed_group_validate(int unit);
extern soc_functions_t soc_monterey_drv_funs;
extern soc_functions_t soc_monterey_drv_funs;
extern void soc_monterey_ser_set_long_delay(int delay);
extern int _soc_monterey_mem_ser_control(int unit, soc_mem_t mem,
                                         int copyno, int enable);
extern int _soc_monterey_mem_cpu_write_control(int unit, soc_mem_t mem,
                                               int copyno, int enable,
                                               int *orig_enable);
extern int soc_monterey_ser_mem_clear(int unit, soc_mem_t mem);
extern void _soc_monterey_alpm_bkt_view_set(int unit, int index, soc_mem_t view);
extern soc_mem_t _soc_monterey_alpm_bkt_view_get(int unit, int index);
extern void soc_monterey_sbus_ring_map_config(int unit);
extern int soc_monterey_mem_config(int unit);
extern int _soc_monterey_mem_sram_info_get(int unit, soc_mem_t mem, int index, 
                                           _soc_ser_sram_info_t *sram_info);
extern int soc_monterey_pipe_select(int unit, int egress, int pipe);
extern int soc_monterey_num_cosq_init(int unit);
extern int soc_monterey_tsc_map_get(int unit, uint32 *tsc_map, uint32 *force_hg);
extern int soc_monterey_port_config_init(int unit, uint16 dev_id, uint8 rev_id);
extern int soc_monterey_port_config_bandwidth_check(int unit);
extern soc_error_t
soc_monterey_port_reg_blk_index_get(int unit ,int port, soc_block_type_t blktype, int *block);
extern int soc_monterey_lls_bmap_alloc(int unit);
extern int soc_monterey_chip_reset(int unit);
extern int soc_monterey_tsc_reset(int unit);
extern int soc_monterey_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_monterey_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_monterey_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem);
extern int soc_monterey_mem_is_eligible_for_scan(int unit, soc_mem_t mem);
extern int soc_monterey_ser_test_skip_check(int unit, soc_mem_t mem);
extern int soc_mn_is_skip_default_lls_creation(int unit);
extern soc_error_t soc_monterey_gatingconfig_init(int unit) ;

extern void
soc_monterey_top_intr(void *unit_vp, void *d1, void *d2,
                                   void *d3, void *d4);
#ifdef INCLUDE_GDPLL
extern void soc_nanosync_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern void soc_nanosync_debug_intr(void *unit_vp, void *d1, void *d2,
                                    void *d3, void *d4);
#endif

extern int soc_monterey_temperature_monitor_get(int unit,
               int temperature_max,
               soc_switch_temperature_monitor_t *temperature_array,
               int *temperature_count);
extern int soc_monterey_show_material_process(int unit);
extern int soc_monterey_show_ring_osc(int unit);
extern int soc_monterey_show_voltage(int unit);

extern int soc_monterey_bond_info_init(int unit);

enum soc_monterey_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_MONTEREY_PORT_MODE_QUAD = 0,
    SOC_MONTEREY_PORT_MODE_TRI_012 = 1,
    SOC_MONTEREY_PORT_MODE_TRI_023 = 2,
    SOC_MONTEREY_PORT_MODE_DUAL = 3,
    SOC_MONTEREY_PORT_MODE_SINGLE = 4
};

extern int      soc_monterey_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_monterey_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_monterey_hash_bank_number_get(int unit, soc_mem_t mem,
                                                  int seq_num, int *bank_num);
extern int      soc_monterey_hash_seq_number_get(int unit, soc_mem_t mem, 
                                                  int bank_num, int *seq_num);
extern int      soc_monterey_hash_bank_info_get(int unit, soc_mem_t mem,
                                                int bank,
                                                int *entries_per_bank,
                                                int *entries_per_row,
                                                int *entries_per_bucket,
                                                int *bank_base,
                                                int *bucket_offset);
extern int      soc_mn_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                        int *hash_offset, int *use_lsb);
extern int      soc_mn_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                       int hash_offset, int use_lsb);
extern uint32   soc_mn_l2x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key);
extern int      soc_mn_l2x_base_entry_to_key(int unit, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_mn_l2x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_mn_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_mn_l3x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key);
extern int      soc_mn_l3x_base_entry_to_key(int unit, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_mn_l3x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_mn_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_mn_mpls_hash(int unit, int hash_sel, int key_nbits,
                                  void *base_entry, uint8 *key);
extern int      soc_mn_mpls_base_entry_to_key(int unit, void *entry,
                                               uint8 *key);
extern uint32   soc_mn_mpls_entry_hash(int unit, int hash_sel, int bank,
                                       uint32 *entry);
extern uint32   soc_mn_mpls_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_mn_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                                        void *base_entry, uint8 *key);
extern int      soc_mn_vlan_xlate_base_entry_to_key(int unit, void *entry,
                                                     uint8 *key);
extern uint32   soc_mn_vlan_xlate_entry_hash(int unit, int hash_sel, int bank,
                                              uint32 *entry);
extern uint32   soc_mn_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32   soc_mn_egr_vlan_xlate_hash(int unit, int hash_sel,
                                            int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_mn_egr_vlan_xlate_base_entry_to_key(int unit, void *entry,
                                                         uint8 *key);
extern uint32   soc_mn_egr_vlan_xlate_entry_hash(int unit, int hash_sel, 
                                                 int bank, uint32 *entry);
extern uint32   soc_mn_egr_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                       uint32 *entry);
extern uint32   soc_mn_ing_vp_vlan_member_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry, uint8 *key);
extern int      soc_mn_ing_vp_vlan_member_base_entry_to_key(int unit,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_mn_ing_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                     int bank, uint32 *entry);
extern uint32   soc_mn_egr_vp_vlan_member_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry, uint8 *key);
extern int      soc_mn_egr_vp_vlan_member_base_entry_to_key(int unit,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_mn_egr_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                     int bank, uint32 *entry);
extern uint32   soc_mn_ing_dnat_address_type_hash(int unit, int hash_sel,
                                                   int key_nbits,
                                                   void *base_entry,
                                                   uint8 *key);
extern int      soc_mn_ing_dnat_address_type_base_entry_to_key(int unit,
                                                                void *entry,
                                                                uint8 *key);
extern uint32   soc_mn_ing_dnat_address_type_entry_hash(int unit,
                                                         int hash_sel,
                                                         int bank,
                                                         uint32 *entry);
extern uint32   soc_mn_l2_endpoint_id_hash(int unit, int hash_sel,
                                            int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_mn_l2_endpoint_id_base_entry_to_key(int unit, void *entry,
                                                         uint8 *key);
extern uint32   soc_mn_l2_endpoint_id_entry_hash(int unit, int hash_sel,
                                                 int bank,
                                                  uint32 *entry);
extern uint32   soc_mn_endpoint_queue_map_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry,
                                                uint8 *key);
extern int      soc_mn_endpoint_queue_map_base_entry_to_key(int unit,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_mn_endpoint_queue_map_entry_hash(int unit, int hash_sel,
                                                     int bank,
                                                      uint32 *entry);
extern int      soc_mn_hash_bucket_get(int unit, int mem, int bank,
                                    uint32 *entry, uint32 *bucket);
extern int      soc_mn_hash_index_get(int unit, int mem, int bank, int bucket);

extern int 
soc_monterey_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                       int *pbase, int *pnumq);

#define SOC_MONTEREY_PORT_PIPE(u, p) 0

extern uint32 _soc_monterey_mmu_port(int unit, int port);

#define SOC_MONTEREY_MMU_PORT(u, p)  _soc_monterey_mmu_port((u),(p))

extern uint32 _soc_monterey_piped_mem_index(int unit, soc_port_t port,
                                 soc_mem_t mem, int arr_off);

#define SOC_MONTEREY_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    _soc_monterey_piped_mem_index((u),(p),(m),(off))

#define _SOC_MONTEREY_NODE_CONFIG_MEM(u,p,n)                                 \
    (((n) == SOC_MONTEREY_NODE_LVL_S1) ? LLS_S1_CONFIGm :                    \
     (((n) == SOC_MONTEREY_NODE_LVL_L0) ? LLS_L0_CONFIGm :                   \
      ((n) == SOC_MONTEREY_NODE_LVL_L1 ? LLS_L1_CONFIGm : -1  )))

#define _SOC_MONTEREY_NODE_CONFIG_MEM2(u,p,n) _SOC_MONTEREY_NODE_CONFIG_MEM(u, p, n)

#define _SOC_MONTEREY_NODE_PARENT_MEM(u,p,n)                                 \
    (((n)==SOC_MONTEREY_NODE_LVL_L2) ? LLS_L2_PARENTm :                      \
     (((n) == SOC_MONTEREY_NODE_LVL_L1) ? LLS_L1_PARENTm :                   \
      (((n) == SOC_MONTEREY_NODE_LVL_L0) ? LLS_L0_PARENTm :                  \
       (((n) == SOC_MONTEREY_NODE_LVL_S1) ? LLS_S1_PARENTm : -1  ))))

#define _SOC_MONTEREY_NODE_WIEGHT_MEM(u,p,n)                                 \
    (((n)==SOC_MONTEREY_NODE_LVL_S1) ? LLS_S1_CHILD_WEIGHT_CFG_CNTm :        \
     (((n)==SOC_MONTEREY_NODE_LVL_L0) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm :       \
     (((n) == SOC_MONTEREY_NODE_LVL_L1) ? LLS_L1_CHILD_WEIGHT_CFG_CNTm :     \
      (((n) == SOC_MONTEREY_NODE_LVL_L2) ? LLS_L2_CHILD_WEIGHT_CFG_CNTm : -1 ))))


typedef enum {
    SOC_MONTEREY_SCHED_UNKNOWN   = 0,
    SOC_MONTEREY_SCHED_LLS,
    SOC_MONTEREY_SCHED_HSP
} soc_monterey_sched_type_t;

extern soc_monterey_sched_type_t
_soc_monterey_port_sched_type_get(int unit, soc_port_t port);

extern int
_soc_monterey_invalid_parent_index(int unit, int level);

extern int
soc_monterey_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                            int *hw_index);

extern int soc_monterey_l2_hw_index(int unit, int qnum, int uc);
extern int soc_monterey_hw_index_logical_num(int unit, int port,
                                                int index, int uc);
extern int
soc_monterey_cosq_set_sched_mode(int unit, soc_port_t port, 
        int level, int index,
        soc_monterey_sched_mode_e mode, int weight);

extern int
soc_monterey_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_psri, int first_sp_child, 
                              int first_sp_mc_child, uint32 ucmap, uint32 spmap,
                              soc_monterey_sched_mode_e mode, int weight);

extern int
soc_monterey_cosq_get_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int *pnum_spri, int *pfirst_sp_child, 
                              int *pfirst_sp_mc_child, uint32 *pucmap, uint32 *spmap);

extern int
soc_monterey_cosq_get_sched_mode(int unit, soc_port_t port, int level, 
                            int index, soc_monterey_sched_mode_e *pmode, int *weight);

extern int _soc_monterey_root_scheduler_index(int unit, int port);
extern int
soc_monterey_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *num_spri, int *first_sp_child, 
                              int *first_sp_mc_child, uint32 *ucmap, uint32 *spmap,
                              soc_monterey_sched_mode_e *mode, int *weight);

extern int 
soc_monterey_get_child_type(int unit, soc_port_t port, int level, int *child_type);

extern int soc_monterey_get_pipe_queue_count(int unit, int uc, int xpipe);

extern int soc_monterey_dump_port_lls(int unit, int port);

extern int soc_monterey_dump_port_hsp(int unit, int port);

extern int soc_monterey_lls_init(int unit);

extern int soc_monterey_lb_lls_init(int unit);

extern int soc_monterey_cosq_set_sched_parent(int unit, soc_port_t port,
                              int level, int hw_index,
                              int parent_hw_idx, int add);

extern int soc_monterey_logical_qnum_hw_qnum(int unit, int port, int lqnum, int uc);

extern int soc_monterey_mmu_get_shared_size(int unit, int *shared_size);
extern int soc_monterey_mmu_config_shared_buf_recalc(int unit,
                                                uint32 spid,
                                                int shared_size,
                                                int flags);
extern int soc_monterey_mmu_config_res_limits_update(int unit,
                                                uint32 spid,
                                                uint32 shared_size,
                                                int flags);

extern void soc_monterey_process_func_intr(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);

extern
void soc_monterey_process_pm_intr(void *unit_vp, void *d1, void *d2,
                                  void *d3, void *d4);
#ifdef INCLUDE_XFLOW_MACSEC
extern
void soc_monterey_process_macsec_intr(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);
#endif
typedef int (*_soc_monterey_bst_hw_cb)(int unit);
extern int soc_monterey_set_bst_callback(int unit, _soc_monterey_bst_hw_cb cb);
extern int soc_monterey_port_asf_para_set(int unit, soc_port_t port, int speed,
                                     int enable);
extern int soc_monterey_port_speed_update(int unit, soc_port_t port,
                                          int speed);
extern int soc_monterey_port_thdo_rx_enable_set(int unit, soc_port_t port,
                                                int enable);
extern int
soc_monterey_idb_buf_reset(int unit, soc_port_t port, int reset);

extern int
soc_monterey_edb_buf_reset(int unit, soc_port_t port, int reset);

extern int soc_monterey_shadow_create(int unit);
extern void soc_monterey_shadow_free(int unit);
extern int
soc_monterey_shadow_entry_set (int unit, soc_mem_t mem, int index,
                                      void* entry_data);
extern int
soc_monterey_shadow_entry_get (int unit, soc_mem_t mem, int index,
                                      void* entry_data);
int soc_monterey_shadow_clear (int unit, soc_mem_t mem);
extern int soc_monterey_port_traffic_egr_enable_set(int unit, int port, int enable);
extern int soc_monterey_port_mmu_tx_enable_set(int unit, int port, int enable);
extern int soc_monterey_qcn_counter_hw_index_get(int unit, soc_port_t port,
                                            int index, int *qindex);
extern int soc_monterey_port_edb_reset(int unit, int port );
extern int
soc_monterey_setup_hsp_port(int unit, int port);
extern int
soc_monterey_hsp_sp_node_init(int unit, soc_port_t port);
extern int
soc_monterey_delete_hsp_port(int unit, int port);
extern int
soc_monterey_mmu_config_init(int unit, int test_only, soc_port_t port);
extern int
soc_monterey_cosq_enable_hsp_sched(int unit, soc_port_t port);
extern soc_error_t
soc_monterey_cosq_disable_hsp_sched(int unit, soc_port_t port);
extern int
bcm_monterey_send_packetport(int unit , int port , int cos , int num );
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_monterey_shadow_size_get (int unit, uint32* size);
extern int soc_monterey_shadow_sync(int unit, uint32 **sync_addr);
extern int soc_monterey_shadow_load(int unit, uint32 **load_addr);
#endif

#if defined(SER_TR_TEST_SUPPORT)
extern soc_error_t soc_monterey_ser_inject_error(int unit, uint32 flags,
                                            soc_mem_t mem, int pipe_target,
                                            int block, int index);
extern soc_error_t soc_monterey_ser_test_mem(int unit, soc_mem_t mem,
                                        _soc_ser_test_t test_type, int cmd);
extern soc_error_t soc_monterey_ser_test(int unit, _soc_ser_test_t test_type);
extern soc_error_t soc_monterey_ser_error_injection_support(int unit,
                                                       soc_mem_t mem, int pipe);
extern int soc_monterey_ser_test_overlay (int unit, _soc_ser_test_t test_type);
extern int soc_monterey_cosq_max_bucket_set(int unit, int port,
                                        int index, uint32 level);
extern int ser_test_monterey_mem_index_remap(int unit,
                                           ser_test_data_t *test_data,
                                           int *mem_has_ecc);
#endif /*#if defined(SER_TR_TEST_SUPPORT)*/

/*
 * Data structure use for storing part of the SOC data information
 * which is needed during later phases of a FlexPort operation.
 */
typedef struct soc_mn_info_s {
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
} soc_mn_info_t;

extern int
soc_monterey_port_mode_get(int unit, int logical_port, int *mode);

extern int
soc_monterey_is_cut_thru_enabled(int unit, soc_port_t port, int *enable);

extern int
soc_mn_cut_thru_enable_disable(int unit, soc_port_t port, int enable);


extern int
soc_mn_phy_info_init(int unit);
extern int
soc_mn_phy_info_deinit(int unit);

extern int
soc_mn_port_lanes_valid(int unit, soc_port_t port, int lanes);

extern int
soc_mn_port_resource_speed_max_get(int unit, int *speed);

extern int
soc_mn_port_addressable(int unit, soc_port_t port);

extern int
soc_mn_phy_port_addressable(int unit, int phy_port);

extern int
soc_mn_edb_buf_reset(int unit, soc_port_t port, int reset);

extern int
soc_mn_port_resource_validate(int unit, int nport,
                              soc_port_resource_t *resource);
extern int
soc_mn_port_resource_configure(int unit, int nport,
                               soc_port_resource_t *resource, int flag);

extern int
soc_mn_port_resource_tdm_set(int unit, int curr_port_info_size,
                             soc_port_resource_t *curr_port_info,
                             int new_port_info_size,
                             soc_port_resource_t *new_port_info,
                             soc_mn_info_t *si, int lossless);

extern int
soc_mn_port_resource_pgw_set(int unit,
                             int pre_num_res,  soc_port_resource_t *pre_res,
                             int post_num_res, soc_port_resource_t *post_res,
                             int lossless);

extern int
soc_mn_port_resource_ip_set(int unit, int curr_port_info_size,
                            soc_port_resource_t *curr_port_info,
                            int new_port_info_size,
                            soc_port_resource_t *new_port_info,
                            soc_mn_info_t *si);

extern int
soc_mn_port_resource_ep_set(int unit, int curr_port_info_size,
                            soc_port_resource_t *curr_port_info,
                            int new_port_info_size,
                            soc_port_resource_t *new_port_info,
                            soc_mn_info_t *si);
extern int
soc_mn_port_resource_mmu_mapping_set(int unit, int nport,
                                     soc_port_resource_t *resource);

extern int
soc_mn_port_resource_mmu_set(int unit, int nport,
                             soc_port_resource_t * flexport_data);

extern int
soc_monterey_port_asf_set(int unit, soc_port_t port, int speed);
extern int
soc_monterey_port_asf_speed_set(int unit, soc_port_t port, int speed);

extern int
soc_monterey_port_lane_config_get(int unit, int phy_port, int *mode_lane,
                              int *found);
extern int
soc_monterey_port_autoneg_core_get(int unit, int phy_port, int *an_core,
                               int *found);
extern int
soc_monterey_mmu_hsp_port_reserve(int unit, int port, int speed_max);
extern soc_error_t
_soc_monterey_mmu_ovs_speed_class_map_get(int unit, int *speed, int *spg, uint32 *sp_spacing);
extern int
soc_mn_port_mixed_sister_speed_validate(int unit);
extern int
soc_monterey_tsc_disable(int unit);

/* 
 * FlexPort Functions
 */

extern int soc_monterey_is_flex_enable(int unit);
extern soc_error_t
soc_monterey_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp);

extern int
soc_monterey_mmu_port_resource_set(int unit, soc_port_t port, int speed);
int
bcm_mn_obm_interrupt_port_control_set(int unit,
                                       bcm_port_t port,
                                       bcm_port_control_t type,
                                       int value);
int
bcm_mn_obm_interrupt_port_control_get(int unit,
                                       bcm_port_t port,
                                       bcm_port_control_t type,
                                       int *value);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_monterey_flexport_scache_allocate(int unit);
extern int soc_monterey_flexport_scache_sync(int unit);
extern int soc_monterey_flexport_scache_recovery(int unit);
#endif



#ifdef PORTMOD_SUPPORT
extern int soc_mn_portctrl_pm_portmod_init(int unit);
extern int soc_mn_portctrl_pm_portmod_deinit(int unit);
extern int soc_mn_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config_info);
extern int soc_mn_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
extern int soc_mn_portctrl_port_ability_update(int unit, soc_port_t port, soc_port_ability_t *ability);
extern int soc_mn_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);
#endif

extern int soc_monterey_alpm_mode_get(int unit);
extern int soc_monterey_get_alpm_banks(int unit);
extern int soc_monterey_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_monterey_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_monterey_mmu_pfc_tx_config_set(int unit, bcm_port_t port, int pfc_enable);
extern int
_soc_monterey_idb_port_init(int unit, int reset, int lossless, soc_port_t port);

typedef int (*_soc_monterey_lls_traverse_cb)(int unit, int port, int level, 
        int hw_index, void *cookie);

#define MONTEREY_PORT_IS_PM4x25_PORT_PHY(unit, phy_port) \
            (((phy_port <=20 ) && (phy_port >= 13)) || \
             ((phy_port <= 52 ) && (phy_port >= 45)))

#define MONTEREY_PORT_IS_CPM4x25_PORT_PHY(unit, phy_port) \
            (((phy_port <= 12 ) && (phy_port >= 1)) || \
             ((phy_port <= 64 ) && (phy_port >= 53)))
#define MONTEREY_PORT_IS_PM4x25_PORT(unit, lport) \
    MONTEREY_PORT_IS_PM4x25_PORT_PHY(unit, (SOC_INFO(unit).port_l2p_mapping[lport]))

#define MONTEREY_PORT_IS_CPM4x25_PORT(unit, lport) \
    MONTEREY_PORT_IS_CPM4x25_PORT_PHY(unit, (SOC_INFO(unit).port_l2p_mapping[lport]))
#endif  /* !_SOC_MONTEREY_H_ */

/*test*/
