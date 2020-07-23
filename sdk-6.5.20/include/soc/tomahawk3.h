
/*
 * $Id: tomahawk3.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk.h
 */

#ifndef _soc_tomahawk3_H_
#define _soc_tomahawk3_H_

#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <soc/esw/port.h>

#define SOC_TH3_PM4X10_COUNT        1
#define SOC_TH3_PM8X50_COUNT        32

#define SOC_TH3_PM4X10_PORT_1       257

#define SOC_TH3_MMU_BASE_TYPE_IPORT 0
#define SOC_TH3_MMU_BASE_TYPE_EPORT 1
#define SOC_TH3_MMU_BASE_TYPE_IPIPE 2
#define SOC_TH3_MMU_BASE_TYPE_EPIPE 3
#define SOC_TH3_MMU_BASE_TYPE_CHIP 4
#define SOC_TH3_MMU_BASE_TYPE_ITM 5

#define soc_th3_MEM_CHK_IFP_TCAM(mem) \
        (mem == IFP_TCAMm || \
         mem == IFP_TCAM_PIPE0m || mem == IFP_TCAM_PIPE1m || \
         mem == IFP_TCAM_PIPE2m || mem == IFP_TCAM_PIPE3m)
#define soc_th3_MEM_CHK_IFP_TCAM_WIDE(mem) \
        (mem == IFP_TCAM_WIDEm || \
         mem == IFP_TCAM_WIDE_PIPE0m || mem == IFP_TCAM_WIDE_PIPE1m || \
         mem == IFP_TCAM_WIDE_PIPE2m || mem == IFP_TCAM_WIDE_PIPE3m)
#define soc_th3_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem) \
        (mem == SRC_COMPRESSIONm || mem == DST_COMPRESSIONm || \
         mem == FP_UDF_TCAMm || mem == VFP_TCAMm || \
         mem == IFP_LOGICAL_TABLE_SELECTm || \
         mem == EXACT_MATCH_LOGICAL_TABLE_SELECTm || \
         mem == EFP_TCAMm || mem == IFP_TCAMm || mem == IFP_TCAM_WIDEm) \
/* Field multi-size slices */
#define _BCM_FIELD_TH3_INGRESS_MAX_SMALL_SLICES 6
#define _BCM_FIELD_TH3_INGRESS_SMALL_SLICE_SZ 256
#define _BCM_FIELD_TH3_INGRESS_LARGE_SLICE_SZ 512

#define IS_HG2_SPEED(speed) ((106000 == speed) ||  (53000 == speed) || \
                             (42000 == speed)  ||  (27000 == speed) || \
                             (21000 == speed)  ||  (11000 == speed))

#define CCLK_FREQ_850MHZ    850

#define SOC_TH3_MMU_PORT_STRIDE         32 /* MMU Port number space per pipe */
#define SOC_TH3_NUM_COS_QUEUES_PER_PIPE 276 /* Num UC Q's Per pipe 19*12 + 48 */
#define SOC_TH3_NUM_CPU_QUEUES          48 /* CPU port has 48 MC Queues */
#define SOC_TH3_NUM_GP_QUEUES           12 /*GPs includingLB/mgmt has 12 Queues*/
#define SOC_TH3_MAX_DEV_PORTS_PER_PIPE  20 /*Max device/MMU ports per pipe*/
#define SOC_TH3_LB_PORT_LOCAL_PHY_NUM   33 /*Local phy port number for LB port*/
#define SOC_TH3_LB_PORT_LOCAL_DEV_NUM   19 /*Local dev port number for LB port*/
#define SOC_TH3_MGMT_PORT_LOCAL_PHY_NUM 32 /*Local phy port number for Mgmt port*/
#define SOC_TH3_MGMT_PORT_LOCAL_DEV_NUM 18 /*Local dev port number for Mgmt port*/
#define SOC_TH3_COS_MAX 12 /*Max number of cos*/

/* Device port
 * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH3_GDEV_PORTS_PER_PIPE        18
#define _TH3_DEV_PORTS_PER_PIPE         (_TH3_GDEV_PORTS_PER_PIPE + 2)
#define _TH3_DEV_PORTS_PER_DEV          \
    (_TH3_DEV_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)

/* L2 learn cache defaults. Refer to L2_LEARN_COPY_CACHE_CTRL's fields for range
 * of values that can be defined, and the meaning of 1 bit fields which will be
 * programmed by values below
 */
/* Default cache fill level is set to 50%; may be changed after testing
 * determines a more accurate value
 */
#define SOC_TH3_LRN_CACHE_THRESHOLD_DEFAULT 8
/* No clear-on-read of cache entries by default */
#define SOC_TH3_LRN_CACHE_CLR_ON_RD_DEFAULT 0
/* Interrupt only if cache fill level exceeds threshold value (0) */
#define SOC_TH3_LRN_CACHE_INTR_CTL_DEFAULT  0
/* Default priority of learn thread */
#define SOC_TH3_LRN_THREAD_PRI_DEFAULT 50

#define SOC_TH3_MAC_IS_MCAST(_mac_)       (_mac_[0] & 0x1)

/* Beginning interrupt number for low pri interrupts */
#define TH3_LP_INTR_START_NUM 128

/* Bits 8-15 */
#define SOC_TH3_L2_LEARN_CACHE_INTR_MASK 0x0000FF00
/* Bits 1,4-20 */
#define SOC_TH3_SER_REG0_INTR_MASK 0x001FFFF3
/* Bits 16-31 */
#define SOC_TH3_SER_REG1_INTR_MASK 0xFFFF0000
/* Bits 0-31 */
#define SOC_TH3_SER_REG2_INTR_MASK 0xFFFFFFFF
/* Bits 0-16 */
#define SOC_TH3_SER_REG3_INTR_MASK 0x0001FFFF

extern uint32 _soc_mem_entry_reset_value[1];

extern int
soc_th3_check_scrub_info(int unit, soc_mem_t mem, int blk, int copyno,
                         int *num_inst_to_scrub, int *acc_type_list);
extern int soc_tomahawk3_clear_mmu_counters(int unit);
extern int soc_tomahawk3_init_mmu_memory(int unit);
extern int soc_th3_mmu_mem_blk_remap(soc_mem_t mem, uint32 *block);
extern int soc_th3_tcam_engine_enable(int unit, int enable);
extern int
soc_tomahawk3_parity_bit_enable(int unit, soc_reg_t enreg,
                                soc_mem_t enmem, soc_field_t enfld,
                                int enfld_position, int enable);
extern int soc_tomahawk3_ser_enable_parity_table_all(int unit, int enable);
extern int soc_th3_mem_parity_control(int unit, soc_mem_t mem, int copyno,
                                     int en, int en_1b);
extern int soc_th3_ser_reg_field32_modify(int unit, soc_reg_t reg,
                                         soc_port_t port, soc_field_t field,
                                         uint32 value, int index,
                                         int mmu_base_index);
extern int soc_th3_ser_test_skip_by_at(int unit, soc_mem_t mem, soc_acc_type_t at);
extern int soc_th3_ser_pipe_skip(int unit, soc_pipe_select_t pipe);
extern int soc_th3_idb_enable_field_check(int unit, soc_reg_t reg,
                                          soc_field_t field,
                                          int *is_idb, int *match);
#ifdef BCM_CMICX_SUPPORT
extern void soc_th3_ser_intr_handler(int unit,uint32 cmic_val,int reg_num);
#endif
extern int
soc_th3_lp_to_fv_index_remap(int unit, soc_mem_t mem, int *rpt_index);

extern int
soc_th3_scan_idb_mem_ecc_status(int unit);

extern int soc_th3_check_scrub_skip(int unit, soc_mem_t mem,
                                   int check_hw_global_mode);
extern int soc_th3_mem_is_eligible_for_scan(int unit, soc_mem_t mem);
extern void soc_th3_mem_scan_info_get(int unit, soc_mem_t mem, int block,
                         int *num_pipe, int *ser_flag);
extern int soc_th3_check_cache_skip(int unit, soc_mem_t mem);
extern int soc_th3_check_hw_global_mode(int unit, soc_mem_t mem, int *mem_mode);
extern int
soc_th3_ifp_tcam_dma_read(int unit, soc_mem_t mem, int blk, uint32 *table,
                         int en_per_pipe_read);

#if defined(SER_TR_TEST_SUPPORT)
extern int ser_test_TH3_mem_index_remap(int unit, ser_test_data_t *test_data,
                                       int *mem_has_ecc);
#endif /*defined(SER_TR_TEST_SUPPORT*/

extern int soc_th3_mem_index_invalid(int unit, soc_mem_t mem, int index,
                                    uint8 dual_mode, soc_mem_t in_mem,
                                    int in_pipe);

extern int soc_th3_mem_is_dual_mode(int unit, soc_mem_t mem,
                                   soc_mem_t *base_mem, int *pipe);
extern int soc_th3_ifp_tcam_range_dma_read(int unit, soc_mem_t mem, int blk,
                                          int start_addr, uint32 num_entry, uint32 *table);
extern int soc_th3_dual_tcam_index_valid(int unit, soc_mem_t mem, int index);
extern int
soc_tomahawk3_show_ring_osc(int unit);

extern int
soc_th3_ifp_slice_mode_hw_get(int unit, int pipe, int slice_num, int *slice_type,
                             int *slice_enabled);
extern int
soc_th3_ifp_slice_mode_get(int unit, int pipe, int slice_num, int *slice_type,
                             int *slice_enabled);
extern int
soc_th3_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                int mmu_base_index,
                                uint32 rval_intr_status_reg);
extern int soc_th3_mmu_get_shared_size(int unit, int *thd_shared);
extern int soc_th3_mmu_config_shared_buf_recalc(int unit, int res, int ing_shd,
                                               int egr_db_shd, int egr_qe_shd,
                                               int flags);
extern int soc_th3_mmu_config_res_limits_update(int unit, int* delta,
                                                int pool, int post_update);
extern int soc_th3_cal_egress_rsvd_limit(int unit, int* total_egr_rsvd_limit);
extern int soc_th3_mmu_config_flex_thresholds(int unit, int port, int shared_limit);
extern int soc_th3_flex_reconf_thresholds(int unit, int port, int pipe);
extern int _soc_th3_mmu_config_shared_limit_chk_set(int unit,
        soc_reg_t reg, soc_mem_t mem, soc_field_t field, int index,
        int itm, uint32 val, int decrease, int chk_zero);


extern soc_functions_t soc_tomahawk3_drv_funs;
extern int soc_tomahawk3_tcam_ser_init(int unit);
extern int soc_tomahawk3_ser_enable_all(int unit, int enable);
extern void soc_tomahawk3_ser_fail(int unit);
extern void soc_tomahawk3_ser_error(void *unit_vp, void *d1, void *d2,
                                   void *d3, void *d4);
extern int soc_tomahawk3_mem_ser_control(int unit, soc_mem_t mem,
                                        int copyno, int enable);
extern int soc_tomahawk3_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                              int enable, int *orig_enable);
extern int soc_tomahawk3_reg_cpu_write_control(int unit, int enable);
extern int soc_tomahawk3_ser_mem_clear(int unit, soc_mem_t mem);
extern void soc_tomahawk3_ser_register(int unit);
extern int soc_tomahawk3_process_ser_fifo(int unit, soc_block_t blk, int pipe,
                                         char *prefix_str, int l2_mgmt_ser_fifo);
extern int soc_tomahawk3_alpm_scrub(int unit);
extern int soc_tomahawk3_mem_config(int unit);
extern int soc_tomahawk3_port_config_init(int unit, uint16 dev_id);

extern int soc_tomahawk3_misc_init_fast_reboot(int unit);

extern int soc_tomahawk3_chip_reset_fast_reboot(int unit);
extern int soc_tomahawk3_chip_reset(int unit);
extern int soc_tomahawk3_port_reset(int unit);
extern int soc_tomahawk3_mem_basetype_get(int unit, soc_mem_t mem);
extern int _soc_th3_ports_per_pm_get(int unit, int pm_id);
extern uint32 _soc_th3_piped_mem_index(int unit, soc_port_t port,
                                      soc_mem_t mem, int arr_off);

extern int soc_th3_l2_age_start(int unit, int interval);
extern int soc_th3_l2_age_stop(int unit);

/* Counter */
extern int soc_counter_tomahawk3_obm_drop_to_phy_port(int unit,int pipe,
                                                      soc_reg_t reg, int obm_port);
extern int soc_counter_tomahawk3_status_enable(int unit, int enable);
extern int soc_counter_tomahawk3_non_dma_init(int unit, int nports,
                                             int non_dma_start_index,
                                             int *non_dma_entries);
extern int soc_counter_tomahawk3_eviction_enable(int unit, int enable);
extern void soc_counter_tomahawk3_eviction_flags_update(int unit,
                                                       uint32 flags,
                                                       int enable);


extern soc_error_t
soc_th3_granular_speed_get(int unit, soc_port_t port, int *speed);

#define TH3_FLEXPORT_SPEED_SET_NUM_LANES -1
typedef struct soc_th3_port_lanes_s {
    int port_base;      /* Base port number */
    int lanes;          /* New number of lanes */
    int cur_lanes;      /* Current number of lanes */
    int mode;           /* New mode */
    int cur_mode;       /* Current mode */
    int class;          /* Speed class for new group */
    int cur_class;      /* Speed class for current group */
    int idb_group;      /* New IDB oversub group number */
    int cur_idb_group;  /* Current IDB oversub group number */
    int idb_slots[4];   /* New IDB oversub group slot number */
    int cur_idb_slots[4]; /* Current IDB oversub group slot number */
    int cur_idb_empty;  /* Current IDB oversub group become empty after flex */
    int mmu_group;      /* New MMU oversub group number */
    int cur_mmu_group;  /* Current MMU oversub group number */
    int mmu_slots[4];   /* New MMU oversub group slot number */
    int cur_mmu_slots[4]; /* Current MMU oversub group slot number */
    int cur_mmu_empty;  /* Current MMU oversub group become empty after flex */
    int bindex;         /* Base port subport index */
    int oversub;        /* Oversub status for the ports */
    int phy_ports_len;  /* Length of valid entries in phy_ports array */
    int phy_ports[3];   /* Array of added/removed physical port number */
    int speed;          /* Port speed to be used during speed set */
} soc_th3_port_lanes_t;

extern int soc_tomahawk3_port_speed_update(int unit, soc_port_t port, int speed);

extern int soc_th3_port_hg_speed_get(int unit, soc_port_t port, int *speed);

extern soc_error_t
soc_th3_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp);

extern soc_error_t
soc_th3_flexport_recover(int unit, soc_port_t port);

extern soc_error_t
soc_th3_port_speed_supported(int unit, soc_port_t port, int speed);
extern soc_error_t
soc_th3_speed_set_init_ctrl(int unit, soc_th_port_lanes_t *lanes_ctrl);

typedef int (*_soc_th3_bst_hw_cb)(int unit);
extern int soc_th3_set_bst_callback(int unit, _soc_th3_bst_hw_cb cb);
extern int  soc_th3_process_mmu_bst(int unit, int itm, soc_field_t field);

extern int
soc_th3_port_oversub_ratio_get(
    int          unit,
    soc_port_t   port,
    int* const   ratio);

extern void soc_nanosync_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);

#define SOC_TH3_MMU_PORT(u, p)  _soc_th3_mmu_port((u),(p))
#define SOC_TH3_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    _soc_th3_piped_mem_index((u),(p),(m),(off))

/* Port Block Base Port is the first physical port of each PM: 1, 9, 17, etc */
#define SOC_TH3_PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x7));

#define SOC_TH3_MMU_LOCAL_PORT(u,p) soc_th3_mmu_local_port_num((u),(p))
#define SOC_TH3_MMU_CHIP_PORT(u,p) soc_th3_mmu_chip_port_num((u),(p))
#define SOC_TH3_MMU_LOSSLESS_DEFAULT_DISABLE 0

enum soc_th3_port_ratio_e {
    SOC_TH3_PORT_RATIO_SINGLE,
    SOC_TH3_PORT_RATIO_DUAL_1_1,
    SOC_TH3_PORT_RATIO_DUAL_2_1,
    SOC_TH3_PORT_RATIO_DUAL_1_2,
    SOC_TH3_PORT_RATIO_TRI_023_2_1_1,
    SOC_TH3_PORT_RATIO_TRI_012_1_1_2,
    SOC_TH3_PORT_RATIO_QUAD,
    SOC_TH3_PORT_RATIO_COUNT
};

enum soc_th3_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    soc_th3_PORT_MODE_QUAD = 0,
    soc_th3_PORT_MODE_TRI_012 = 1,
    soc_th3_PORT_MODE_TRI_023 = 2,
    soc_th3_PORT_MODE_DUAL = 3,
    soc_th3_PORT_MODE_SINGLE = 4
};

enum TH3_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH3_L2_HASH_KEY_TYPE_BRIDGE = 0,
    TH3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    TH3_L2_HASH_KEY_TYPE_COUNT
};

enum TH3_l3_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH3_L3_HASH_KEY_TYPE_V4UC = 0,
    TH3_L3_HASH_KEY_TYPE_V6UC = 2,
    TH3_L3_HASH_KEY_TYPE_V4MC = 4,
    TH3_L3_HASH_KEY_TYPE_V6MC = 5,
    TH3_L3_HASH_KEY_TYPE_COUNT
};

enum TH3_tunnel_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH3_TUNNEL_HASH_KEY_TYPE_MPLS = 0,
    TH3_TUNNEL_HASH_KEY_TYPE_V4 = 1,
    TH3_TUNNEL_HASH_KEY_TYPE_V6 = 2,
    TH3_TUNNEL_HASH_KEY_TYPE_COUNT
};

enum TH3_fpem_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH3_FPEM_HASH_KEY_TYPE_128B = 0,
    TH3_FPEM_HASH_KEY_TYPE_160B = 1,
    TH3_FPEM_HASH_KEY_TYPE_320B = 2,
    TH3_FPEM_HASH_KEY_TYPE_COUNT
};

enum TH3_mpls_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH3_MPLS_HASH_KEY_TYPE_MPLS = 0,
    TH3_MPLS_HASH_KEY_TYPE_COUNT
};

/* Maximum Flex Key Width for hash table in TH3 */
#define SOC_TH3_L2_ENTRY_MAX_KEY_WIDTH          (64)
#define SOC_TH3_EXACT_MATCH_MAX_KEY_WIDTH       (322)
#define SOC_TH3_L3_TUNNEL_MAX_KEY_WIDTH         (311)
#define SOC_TH3_L3_MPLS_MAX_KEY_WIDTH           (32)
#define SOC_TH3_L3_MAX_KEY_WIDTH                (288)

extern int      soc_tomahawk3_hash_init(int unit);
extern int      soc_tomahawk3_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_tomahawk3_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_tomahawk3_hash_bank_number_get(int unit, soc_mem_t mem,
                                                  int seq_num, int *bank_num);
extern int      soc_tomahawk3_hash_seq_number_get(int unit, soc_mem_t mem,
                                                 int bank_num, int *seq_num);
extern int      soc_tomahawk3_hash_bank_info_get(int unit, soc_mem_t mem,
                                                int bank,
                                                int *entries_per_bank,
                                                int *entries_per_row,
                                                int *entries_per_bucket,
                                                int *bank_base,
                                                int *bucket_offset);
extern int      soc_tomahawk3_hash_bucket_get(int unit, int mem, int bank,
                                       uint32 *entry, uint32 *bucket);
extern int      soc_tomahawk3_hash_index_get(int unit, int mem, int bank, int bucket);
extern int      soc_tomahawk3_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                       int *hash_offset, int *use_lsb);
extern int      soc_tomahawk3_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                       int hash_offset, int use_lsb);
extern uint32   soc_tomahawk3_l2x_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_l2x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                             uint8 *key);
extern uint32   soc_tomahawk3_l2x_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_tomahawk3_l3x_hash(int unit, int bank, int hash_offset, int use_lsb,
                 int key_nbits, void *base_entry, uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_l3x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                             uint8 *key);
extern uint32   soc_tomahawk3_l3x_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern int      soc_tomahawk3_hash_l3_init(int unit);
extern uint32   soc_tomahawk3_exact_match_hash(int unit, int bank, int hash_offset,
                                        int use_lsb, int key_nbits,
                                        void *base_entry, uint8 *key1, 
                                        uint8 *key2);
extern int      soc_tomahawk3_exact_match_base_entry_to_key(int unit, int bank,
                                                     uint32 *entry,
                                                     uint8 *key);
extern uint32   soc_tomahawk3_exact_match_entry_hash(int unit, int bank,
                                              int hash_offset, int use_lsb,
                                              uint32 *entry);
extern uint32   soc_tomahawk3_exact_match_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32 soc_tomahawk3_mpls_hash(int unit, int bank, int hash_offset, 
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_mpls_base_entry_to_key(int unit, int bank, void *entry,
                                              uint8 *key);
extern uint32 soc_tomahawk3_mpls_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_mpls_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_tomahawk3_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                        int use_lsb, int key_nbits, void *base_entry,
                        uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                     void *entry, uint8 *key);
extern uint32   soc_tomahawk3_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                              int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                   uint32 *entry);
extern uint32   soc_tomahawk3_egr_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                            int use_lsb, int key_nbits, void *base_entry,
                            uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_egr_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                         void *entry, uint8 *key);
extern uint32   soc_tomahawk3_egr_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank,
                                  int hash_offset, int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_egr_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                       uint32 *entry);
extern uint32   soc_tomahawk3_ing_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_ing_vp_vlan_member_base_entry_to_key(int unit, int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_tomahawk3_ing_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_egr_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_egr_vp_vlan_member_base_entry_to_key(int unit, int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_tomahawk3_egr_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_ing_dnat_address_type_hash(int unit, int bank, int hash_offset,
                                   int use_lsb, int key_nbits,
                                   void *base_entry, uint8 *key1, uint8 *key2);
extern int      soc_tomahawk3_ing_dnat_address_type_base_entry_to_key(int unit,
                                                               int bank,
                                                               void *entry,
                                                               uint8 *key);
extern uint32   soc_tomahawk3_ing_dnat_address_type_entry_hash(int unit, int bank, int hash_offset,
                                         int use_lsb, uint32 *entry);
extern uint32   soc_tomahawk3_tunnel_hash(int unit, int bank, int hash_offset, int use_lsb,
                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2);

extern int      soc_tomahawk3_tunnel_base_entry_to_key(int unit,
                                                int bank,
                                                uint32 *entry,
                                                uint8 *key);

extern uint32 soc_tomahawk3_tunnel_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry);

extern uint32 soc_tomahawk3_tunnel_bank_entry_hash(int unit, int bank, uint32 *entry);

extern int      soc_tomahawk3_hash_mem_status_get(int unit, soc_mem_t mem, void* entry, uint32 *v);
extern int      soc_tomahawk3_shared_hash_mem_bucket_read(int unit, int ent, int entry_width, int *width,
                                    soc_mem_t base_mem, soc_mem_t *mem, void *bkt_entry, void *entry);

/* SCB */
extern int soc_tomahawk3_scb_init(int unit);

/* WRED */
extern int soc_tomahawk3_wred_init(int unit);

/* Read Launcher */
extern int soc_tomahawk3_rl_init(int unit);

/* MMU interrupts Enable */
extern int soc_th3_mmu_enable_non_func_intr(int unit);

/* MMU per-block non-func interrupt handlers */
extern int soc_th3_process_itm_cfap_int(int unit, int itm, soc_field_t
        itm_intr_field);
extern int soc_th3_process_itm_thdi_int(int unit, int itm, soc_field_t
        itm_intr_field);
extern int soc_th3_process_itm_thdo_int(int unit, int itm, soc_field_t
        itm_intr_field);
extern int soc_th3_process_itm_thdr_int(int unit, int itm, soc_field_t
        itm_intr_field);
extern int soc_th3_process_itm_crb_int(int unit, int itm, soc_field_t
        itm_intr_field);
extern int soc_th3_process_itm_rqe_int(int unit, int itm, soc_field_t
        itm_intr_field);

extern int soc_th3_process_eb_ebctm_int(int unit, int pipe, soc_field_t
        eb_intr_field);
extern int soc_th3_process_eb_ebcfp_int(int unit, int pipe, soc_field_t
        eb_intr_field);
extern int soc_th3_process_eb_mtro_int(int unit, int pipe, soc_field_t
        eb_intr_field);


/* Special port number used by H/W */
#define _soc_th3_TDM_OVERSUB_TOKEN    0x22
#define _soc_th3_TDM_NULL_TOKEN       0x23
#define _soc_th3_TDM_IDL1_TOKEN       0x24
#define _soc_th3_TDM_IDL2_TOKEN       0x25
#define _soc_th3_TDM_UNUSED_TOKEN     0x3f

/* scheduler internal data structure */
#define SOC_TH3_MAX_NUM_PORTS         160
#define SOC_TH3_MAX_NUM_SCHED_PROFILE  8
#define TH3_MMU_LB_MCQ_OFFSET 216
#define TH3_MMU_MGMT_Q_OFFSET 228
#define TH3_MMU_CPU_MCQ_OFFSET 228

/* Added for Queue scheduler mapping */
typedef struct _soc_mmu_cfg_scheduler_profile_s
{
    int num_unicast_queue[SOC_TH3_COS_MAX];
    int num_multicast_queue[SOC_TH3_COS_MAX];
    int strict_priority[SOC_TH3_COS_MAX];
    int flow_control_only_unicast[SOC_TH3_COS_MAX];
    int valid;
} _soc_mmu_cfg_scheduler_profile_t;

/* Fixed attributes for PG headroom calculation */
#define SOC_TH3_IPG_SIZE    20
#define SOC_TH3_PFC_FRAME_SIZE  64


extern int _soc_th3_get_num_ucq(int unit);
extern int _soc_th3_get_num_mcq(int unit);
extern int soc_th3_chip_queue_num_get(int unit, soc_port_t port, int qid, int type,
        int *hw_index);
extern _soc_mmu_cfg_scheduler_profile_t* soc_mmu_cfg_scheduler_profile_alloc(int unit);
extern void soc_mmu_cfg_scheduler_profile_free(int unit, _soc_mmu_cfg_scheduler_profile_t *cfg);
extern int _soc_mmu_cfg_scheduler_profile_read(int unit, _soc_mmu_cfg_scheduler_profile_t *sched_profile,
                                 int *scheduler_port_profile_map);
extern int _soc_mmu_tomahawk3_scheduler_profile_reset(int unit,
             _soc_mmu_cfg_scheduler_profile_t *sched_profile);
extern int _soc_mmu_tomahawk3_scheduler_profile_config_check(int unit, int idx,
             _soc_mmu_cfg_scheduler_profile_t *sched_profile);
extern int _soc_mmu_tomahawk3_scheduler_profile_check(int unit,
             _soc_mmu_cfg_scheduler_profile_t *sched_profile);
extern int soc_tomahawk3_cosq_port_info_init(int unit, int port_num);


extern int      soc_tomahawk3_scheduler_init(int unit);
extern int soc_tomahawk3_sched_update_init(int unit, int profile, int *L0, int *schedq,
        int *mmuq, int *cos, int *strict_priority, int* fc_is_us_only);
extern int
soc_profile_port_list_get(int unit, int profile, int port_in_profile[]);
extern int
soc_cosq_sched_policy_init(int unit, int profile);
extern int soc_tomahawk3_schduler_hier_show(int unit, int port);
extern int soc_tomahawk3_sched_update_flex_per_port(int unit, int port, int profile, int *L0, int *schedq,
        int *mmuq, int *cos);
extern int soc_tomahawk3_get_cos_for_mmu_queue(int unit, int profile, int mmuq);

extern int soc_tomahawk3_get_fc_only_uc_for_cos(int unit, int profile, int cos);
extern int soc_tomahawk3_get_sp_for_cos(int unit, int profile, int cos);
extern int soc_tomahawk3_get_sched_profile_max_cos(int unit, int profile);

/* Warmboot helper functions */
extern int soc_tomahawk3_sched_update_reinit(int unit, int profile, int *L0, int *schedq,
        int *mmuq, int *cos, int *strict_priority, int* fc_is_us_only);
extern int
soc_cosq_sched_policy_reinit(int unit, int profile);
extern int soc_tomahawk3_sched_profile_attach_reinit(int unit, soc_port_t port, int profile);

typedef enum {
    SOC_TH3_SCHED_MODE_UNKNOWN = 0,
    SOC_TH3_SCHED_MODE_STRICT,
    SOC_TH3_SCHED_MODE_WRR,
    SOC_TH3_SCHED_MODE_WERR
} soc_th3_sched_mode_e;

typedef enum {
    SOC_TH3_NODE_LVL_ROOT = 0,
    SOC_TH3_NODE_LVL_L0,
    SOC_TH3_NODE_LVL_L1,
    SOC_TH3_NODE_LVL_L2,
    SOC_TH3_NODE_LVL_MAX
} soc_th3_node_lvl_e;

typedef struct soc_th3_cosq_node_s {
    int level;
    int in_use;
    int fc_is_uc_only;
    soc_th3_sched_mode_e sched_policy;
    int weight;
    int hw_index;
    soc_cos_t cos;
    struct soc_th3_cosq_node_s *parent; /*only one parent*/
    struct soc_th3_cosq_node_s *children[2]; /*child*/
} soc_th3_cosq_node_t;

typedef struct soc_th3_cosq_cpu_node_s {
    int level;
    int in_use;
    soc_th3_sched_mode_e sched_policy;
    int weight;
    int hw_index;
    soc_cos_t cos;
    struct soc_th3_cosq_cpu_node_s *parent; /*only one parent*/
    struct soc_th3_cosq_cpu_node_s *children[SOC_TH3_NUM_CPU_QUEUES]; /*child*/
} soc_th3_cosq_cpu_node_t;

typedef struct soc_th3_cosq_port_info_s {
    soc_th3_cosq_node_t L0[SOC_TH3_NUM_GP_QUEUES];
    soc_th3_cosq_node_t L1[SOC_TH3_NUM_GP_QUEUES];
    soc_th3_cosq_node_t mmuq[SOC_TH3_NUM_GP_QUEUES];
} soc_th3_cosq_port_info_t;

typedef struct soc_th3_cosq_cpu_port_info_s {
    soc_th3_cosq_cpu_node_t L0[SOC_TH3_NUM_GP_QUEUES];
    soc_th3_cosq_cpu_node_t L1[SOC_TH3_NUM_CPU_QUEUES];
} soc_th3_cosq_cpu_port_info_t;

typedef struct soc_th3_mmu_info_s {
    soc_th3_cosq_port_info_t th3_port_info[SOC_TH3_MAX_NUM_PORTS];
    soc_th3_cosq_cpu_port_info_t th3_cpu_port_info;
} soc_th3_mmu_info_t;

typedef struct soc_th3_sched_profile_info_s {
    int mmuq[2];
    int strict_priority;
    int cos;
    int fc_is_uc_only;
} soc_th3_sched_profile_info_t;

extern soc_th3_mmu_info_t *th3_cosq_mmu_info[SOC_MAX_NUM_DEVICES];
extern soc_th3_sched_profile_info_t th3_sched_profile_info[SOC_MAX_NUM_DEVICES]
                      [SOC_TH3_MAX_NUM_SCHED_PROFILE][SOC_TH3_NUM_GP_QUEUES];
extern int L1_TO_L0_MAPPING[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE]
                                [SOC_TH3_NUM_GP_QUEUES];
extern int L0_TO_L1_MAPPING_NUM[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE]
                                        [SOC_TH3_NUM_GP_QUEUES];
extern int L0_TO_COSQ_MAPPING[SOC_MAX_NUM_DEVICES][SOC_TH3_MAX_NUM_SCHED_PROFILE]
                                        [SOC_TH3_NUM_GP_QUEUES];

typedef enum {
    _soc_th3_INDEX_STYLE_BUCKET,
    _soc_th3_INDEX_STYLE_BUCKET_MODE,
    _soc_th3_INDEX_STYLE_WRED,
    _soc_th3_INDEX_STYLE_WRED_DROP_THRESH,
    _soc_th3_INDEX_STYLE_SCHEDULER,
    _soc_th3_INDEX_STYLE_PERQ_XMT,
    _soc_th3_INDEX_STYLE_UCAST_DROP,
    _soc_th3_INDEX_STYLE_UCAST_QUEUE,
    _soc_th3_INDEX_STYLE_MCAST_QUEUE,
    _soc_th3_INDEX_STYLE_EXT_UCAST_QUEUE,
    _soc_th3_INDEX_STYLE_EGR_POOL,
    _soc_th3_INDEX_STYLE_COUNT
} soc_th3_index_style_t;

typedef enum soc_th3_drop_limit_alpha_value_e {
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_th3_drop_limit_alpha_value_t;

#define _TH3_PORTS_PER_PBLK             8
#define _TH3_PBLKS_PER_PIPE             4
#define _TH3_PIPES_PER_DEV              8

#define _TH3_GPHY_PORTS_PER_PIPE        \
    (_TH3_PORTS_PER_PBLK * _TH3_PBLKS_PER_PIPE)
#define _TH3_GPHY_PORTS_PER_DEV         \
    (_TH3_GPHY_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)
#define _TH3_PHY_PORTS_PER_PIPE         (_TH3_GPHY_PORTS_PER_PIPE + 2)
#define _TH3_ITMS_PER_DEV               2

#define _TH3_PBLKS_PER_DEV              (_TH3_PBLKS_PER_PIPE * _TH3_PIPES_PER_DEV)

#define _TH3_PORTS_PER_PIPE             (_TH3_PORTS_PER_PBLK * _TH3_PBLKS_PER_PIPE)
#define _TH3_PORTS_PER_DEV              (_TH3_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)
#define _TH3_PHY_PORTS_PER_DEV          \
    (_TH3_PHY_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)

#define _TH3_SBUS_BCAST_BLOCK_DEFAULT   0x7f
#define _TH3_SBUS_BCAST_BLOCK_SBUS4     56
#define _TH3_SBUS_BCAST_BLOCK_SBUS5     59

/* MMU port */
#define _TH3_MMU_PORTS_OFFSET_PER_PIPE  32
#define _TH3_MMU_PORTS_PER_DEV          (32 * 8)

#define _TH3_SECTION_SIZE                32768

#define _TH3_MMU_MAX_PACKET_BYTES        9416  /* bytes */
#define _TH3_MMU_PACKET_HEADER_BYTES     40    /* bytes */
#define _TH3_MMU_JUMBO_FRAME_BYTES       9216  /* bytes */
#define _TH3_MMU_DEFAULT_MTU_BYTES       1536  /* bytes */

#define _TH3_MMU_PHYSICAL_CELLS_PER_ITM  139264 /* Total Physical cells per ITM */
#define _TH3_MMU_TOTAL_CELLS_AVAIL_PER_ITM    134169 /* Total cells cells/ITM */
#define _TH3_MMU_RSVD_CELLS_CFAP_PER_ITM 408   /* Reserved CFAP cells/ITM */
#define SOC_TH3_MMU_MCQ_ENTRY_PER_ITM    13600
#define SOC_TH3_MMU_RQE_ENTRY_PER_ITM    2048
#define SOC_TH3_MMU_RQE_ENTRY_RSVD_PER_ITM 63   /* Reserved RQE QE per ITM */
#define _TH3_MMU_TOTAL_CELLS_PER_ITM     (_TH3_MMU_TOTAL_CELLS_AVAIL_PER_ITM - \
                                          SOC_TH3_MMU_RQE_ENTRY_PER_ITM)


#define _TH3_THDI_BUFFER_CELL_LIMIT_SP_MAX      _TH3_MMU_TOTAL_CELLS_PER_ITM
#define _TH3_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX      _TH3_MMU_TOTAL_CELLS_PER_ITM


#define _TH3_MMU_BYTES_PER_CELL          254   /* bytes (1664 bits) */
#define _TH3_MMU_NUM_PG                  8
#define _TH3_MMU_NUM_POOL                4
#define _TH3_MMU_NUM_INT_PRI             16
#define _TH3_MMU_NUM_MAX_PROFILES        8

#define _TH3_A0_MMU_NUM_RQE_QUEUES          3
#define _TH3_A0_CPU_HI_RQE_Q_NUM 0
#define _TH3_A0_CPU_LO_RQE_Q_NUM 1
#define _TH3_A0_MIRR_RQE_Q_NUM 2

#define _TH3_MMU_NUM_RQE_QUEUES          9
#define _TH3_CPU_HI_RQE_Q_NUM 6
#define _TH3_CPU_LO_RQE_Q_NUM 7
#define _TH3_MIRR_RQE_Q_NUM 8

#define SOC_TH3_MMU_CFG_QGROUP_MAX       160

#define _TH3_MMU_JUMBO_PACKET_SIZE _MMU_CFG_MMU_BYTES_TO_CELLS(_TH3_MMU_JUMBO_FRAME_BYTES + \
                                                               _TH3_MMU_PACKET_HEADER_BYTES, \
                                                               _TH3_MMU_BYTES_PER_CELL)

#define SOC_TH3_NUM_UC_QUEUES_PER_PIPE   330 /* Num UC Q's Per pipe 33 * 10 */
#define SOC_TH3_NUM_MC_QUEUES_PER_PIPE   378 /* Num MC Q's Per pipe (33 * 10)
                                               + 48 CPU queues */
#define SOC_TH3_MAX_PHY_PORTS_PER_PIPE 32
#define SOC_TH3_PHY_PORTS_PER_DEV          \
    (SOC_TH3_MAX_PHY_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)
#define SOC_TH3_DEV_PORTS_PER_DEV          \
    (SOC_TH3_MAX_DEV_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)
#define SOC_TH3_MMU_PORTS_PER_DEV          \
    ( SOC_TH3_MAX_DEV_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV)

#define _TH3_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_TH3_MMU_MCQ_ENTRY_PER_ITM
#define _TH3_MMU_TOTAL_RQE_ENTRY(unit)   SOC_TH3_MMU_RQE_ENTRY_PER_ITM

/* Default values for thresholds */
#define _TH3_CFAP_BANK_FULL_LIMIT 2044
#define _TH3_LAST_PKT_ACCEPT_MODE_DEFAULT 1
#define _TH3_LAST_PKT_ACCEPT_MODE_POOL_DEFAULT 15
#define _TH3_COLOR_AWARE_DEFAULT 0
#define _TH3_HDRM_LIMIT_DEFAULT 0
#define _TH3_SPID_OVERRIDE_ENABLE_DEFAULT 0
#define _TH3_SPAP_YELLOW_OFFSET_DEFAULT 0
#define _TH3_SPAP_RED_OFFSET_DEFAULT 0
#define _TH3_MC_Q_MODE_DEFAULT 2
#define _TH3_OUTPUT_PORT_EN_DEFAULT 1048575
#define _TH3_PAUSE_EN_DEFAULT 0
#define _TH3_SPID_DEFAULT 0

/*defines added for SDK-142748*/
#define _TH3_MMU_SPARE_SRC_PORT 98
#define _TH3_MMU_SPARE_SRC_PORT_PIPE 4
#define _TH3_MMU_SPARE_DST_PORT 158
#define _TH3_IFP_COS_MAP_RSVD_PROF 3

#define soc_th3_STAT_ING_FLEX_POOL_MAX   20

#define _TH3_ARRAY_MIN(arr, len, bmp, min)                   \
    do {                                                    \
        int i;                                              \
        min = 0x7fffffff;                                   \
        if (arr) {                                          \
            for (i = 0; i < len; i ++) {                    \
                if ((bmp & (1 << i)) && (arr[i] < min)) {   \
                    min = arr[i];                           \
                }                                           \
            }                                               \
        }                                                   \
    } while (0)

/* TDM */
#define _TH3_TDM_LENGTH                 33
#define _TH3_OVS_HPIPE_COUNT_PER_PIPE   1

typedef struct _soc_tomahawk3_tdm_pblk_info_s {
    int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
} _soc_tomahawk3_tdm_pblk_info_t;

typedef struct _soc_tomahawk3_tdm_pipe_s {
    int idb_tdm[_TH3_TDM_LENGTH];
    int mmu_tdm[_TH3_TDM_LENGTH];
} _soc_tomahawk3_tdm_pipe_t;

typedef struct _soc_tomahawk3_tdm_s {
    _soc_tomahawk3_tdm_pipe_t tdm_pipe[_TH3_PIPES_PER_DEV];
    _soc_tomahawk3_tdm_pblk_info_t pblk_info[_TH3_PHY_PORTS_PER_DEV];
    int port_ratio[_TH3_PBLKS_PER_DEV];
    int ovs_ratio_x1000[_TH3_PIPES_PER_DEV][_TH3_OVS_HPIPE_COUNT_PER_PIPE];
} _soc_tomahawk3_tdm_t;

/* Oversub XON/XOFF Threshold */
#define _TH3_XON_THRESHOLD_10G      4672
#define _TH3_XON_THRESHOLD_25G      4672
#define _TH3_XON_THRESHOLD_50G      4672
#define _TH3_XON_THRESHOLD_100G     11776
#define _TH3_XON_THRESHOLD_200G     24064
#define _TH3_XON_THRESHOLD_400G     48640

#define _TH3_XOFF_THRESHOLD_10G     5184
#define _TH3_XOFF_THRESHOLD_25G     5184
#define _TH3_XOFF_THRESHOLD_50G     5184
#define _TH3_XOFF_THRESHOLD_100G    12288
#define _TH3_XOFF_THRESHOLD_200G    24576
#define _TH3_XOFF_THRESHOLD_400G    49152


/*PG headroom*/
#define _TH3_PG_HEADROOM_LINERATE_10G   167
#define _TH3_PG_HEADROOM_LINERATE_25G   256
#define _TH3_PG_HEADROOM_LINERATE_50G   360
#define _TH3_PG_HEADROOM_LINERATE_100G  580
#define _TH3_PG_HEADROOM_LINERATE_200G  1061
#define _TH3_PG_HEADROOM_LINERATE_400G  1962

#define _TH3_PG_HEADROOM_OVERSUB_10G    \
    (_TH3_PG_HEADROOM_LINERATE_10G + CEIL(_TH3_XOFF_THRESHOLD_10G, 64))
#define _TH3_PG_HEADROOM_OVERSUB_25G    \
    (_TH3_PG_HEADROOM_LINERATE_25G + CEIL(_TH3_XOFF_THRESHOLD_25G, 64))
#define _TH3_PG_HEADROOM_OVERSUB_50G    \
    (_TH3_PG_HEADROOM_LINERATE_50G + CEIL(_TH3_XOFF_THRESHOLD_50G, 64))
#define _TH3_PG_HEADROOM_OVERSUB_100G    \
    (_TH3_PG_HEADROOM_LINERATE_100G + CEIL(_TH3_XOFF_THRESHOLD_100G, 64))
#define _TH3_PG_HEADROOM_OVERSUB_200G    \
    (_TH3_PG_HEADROOM_LINERATE_200G + CEIL(_TH3_XOFF_THRESHOLD_200G, 64))
#define _TH3_PG_HEADROOM_OVERSUB_400G    \
    (_TH3_PG_HEADROOM_LINERATE_400G + CEIL(_TH3_XOFF_THRESHOLD_400G, 64))

/* Function to Retrieve IFP memory mode. */
extern int
soc_th3_field_mem_mode_get(int unit, soc_mem_t mem, int *mode);

extern int
soc_th3_ser_mem_mode_get(int unit, soc_mem_t mem, int *mode);

extern int
soc_th3_cosq_sched_mode_set(int unit, soc_port_t port, int level, int index,
                           soc_th3_sched_mode_e mode, int weight, int mc);
extern int
soc_th3_cosq_sched_mode_get(int unit, soc_port_t port, int level, int index,
                           soc_th3_sched_mode_e *mode, int *weight, int mc);
extern int
soc_th3_cosq_cpu_parent_set(int unit, int child_index, int child_level,
                           int parent_index);
extern int
soc_th3_cosq_cpu_parent_get(int unit, int child_index, int child_level,
                           int *parent_index);

extern int
soc_th3_cosq_parent_get(int unit, int port, int child_index, int child_level,
                           int *parent_index);
extern int
soc_tomahawk3_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                               _soc_ser_sram_info_t *sram_info);

#if defined(SER_TR_TEST_SUPPORT)
extern void soc_th3_ser_test_register(int unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/

extern int soc_tomahawk3_clear_all_port_data(int unit);

#define SOC_TH3_NUM_EL_VLAN_XLATE                     4

#define SOC_TH3_NUM_EL_EGR_VLAN_XLATE                 4

#define SOC_TH3_NUM_EL_ING_L3_NEXT_HOP                4
#define SOC_TH3_RAM_OFFSET_ING_L3_NEXT_HOP            8192

#define SOC_TH3_NUM_ENTRIES_L2_BANK                   8192
#define SOC_TH3_NUM_ENTRIES_L3_BANK                   16384

#define SOC_TH3_NUM_EL_SHARED                         4
#define SOC_TH3_NUM_ENTRIES_XOR_BANK                  2048
#define SOC_TH3_LP_ENTRIES_IN_XOR_BANK                2048
#define SOC_TH3_LP_ENTRIES_IN_UFT_BANK                8192

#define SOC_TH3_ALPM_MAX_BKTS(u)                      8192
#define SOC_TH3_ALPM_BKT_MASK(u)                      0x1FFF
#define SOC_TH3_ALPM_BKT_OFFFSET                      2048
#define SOC_TH3_ALPM_MODE_BKT_MASK                   0x38003

#define _soc_th3_L2_MGMT_INTR_MASK                    0x2
#define _soc_th3_FUNC_INTR_MASK                       _soc_th3_L2_MGMT_INTR_MASK

extern int soc_th3_get_alpm_banks(int unit);
extern int soc_tomahawk3_alpm_mode_get(int unit);

extern void _soc_th3_lpm_view_set(int unit, int index, soc_mem_t view, int pair_lpm);
extern soc_mem_t _soc_th3_lpm_view_get(int unit, int index, int pair_lpm);

extern int
soc_tomahawk3_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count);
extern void
soc_tomahawk3_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern void
soc_tomahawk3_process_func_intr(void *unit_vp, void *d1, void *d2, void *d3, void *d4);

#define _soc_th3_UNIT_VALIDATE(unit)                                \
        if ( !(((unit) >= 0) && ((unit) <= SOC_MAX_NUM_DEVICES)) ) \
            return SOC_E_UNIT;

#define OUT
#define IN_OUT

/*
  =============================
   TH3 ASF (cut-thru forwarding)
  =============================
*/
/* ASF Modes */
typedef enum soc_th3_asf_mode_e{
    SOC_TH3_ASF_MODE_SAF = 0,
    SOC_TH3_ASF_MODE_CT = 1,
    SOC_TH3_ASF_MODE_CFG_UPDATE = 2,
    SOC_TH3_ASF_MODE_INIT = 3,
    SOC_TH3_ASF_MODE_MAX = 4
} soc_th3_asf_mode_t;

int
soc_th3_asf_init(int unit);

int
soc_th3_asf_deinit(int unit);

int
soc_th3_port_asf_mode_get(
    int unit,
    soc_port_t port,
    int port_speed,
    soc_th3_asf_mode_t *mode);

int
soc_th3_port_asf_mode_set(
    int unit,
    soc_port_t port,
    int port_speed,
    soc_th3_asf_mode_t mode);

int
soc_th3_port_asf_detach(int unit, soc_port_t port);

int
soc_th3_port_asf_valid(
    int unit,
    soc_port_t port);

/* diag shell debug functions */
int
soc_th3_asf_config_dump(int unit);

int
soc_th3_port_asf_show(int unit, soc_port_t port, int speed);

int
soc_th3_port_asf_config_dump(
    int unit,
    soc_port_t port,
    int port_speed);

/* EDB config*/
int
soc_th3_port_asf_xmit_start_count_set(
    int unit,
    soc_port_t port,
    int port_speed,
    soc_th3_asf_mode_t mode,
    uint8 extra_cells);

int
soc_th3_port_asf_xmit_start_count_get(
    int unit,
    soc_port_t port,
    int sc,
    int dc,
    soc_th3_asf_mode_t mode,
    uint8* xmit_cnt);

#ifdef BCM_WARM_BOOT_SUPPORT
/* TH3 ASF Warmboot functions */
extern int
soc_th3_asf_wb_memsz_get(
    int unit,
    OUT uint32* const wb_mem_sz,
    uint16            scache_ver);

extern int
soc_th3_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data);

extern int
soc_th3_asf_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver);
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
  ==========================
   TH Switch Latency Bypass
  ==========================
*/

/* resource type */
typedef enum {
  _soc_th3_REG = 0,
  _soc_th3_MEM = 1
} soc_th3_resource_e;

/* protocol table configs */
typedef struct _soc_th3_latency_table_cfg_s {
    int l2_entries;
    int fixed_l2_entries;
    int shared_l2_banks;
    int l3_entries;
    int fixed_l3_entries;
    int shared_l3_banks;
    int fpem_entries;
    int shared_fpem_banks;
    int alpm_enable;
    int max_tcams;
    int tcam_depth;
} soc_th3_latency_table_cfg_t;


extern int
soc_th3_latency_init(int unit);

extern int
soc_th3_latency_deinit(int unit);

extern int
soc_th3_latency_get(
    int unit,
    OUT int* const latency);

extern int
soc_th3_latency_bypassed(
    int             unit,
    soc_feature_t   feature);

extern int
soc_th3_latency_mem_avail(
    int         unit,
    soc_mem_t   mem);

extern int
soc_th3_latency_reg_avail(
    int         unit,
    soc_reg_t   reg);

extern int
soc_th3_latency_mem_filter(int unit);

extern int
soc_th3_latency_reg_filter(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_th3_latency_wb_memsz_get(
    int               unit,
    OUT uint32* const wb_mem_sz,
    uint16            scache_ver);

extern int
soc_th3_latency_wb_sync(
    int                 unit,
    IN_OUT uint8* const wb_data);

extern int
soc_th3_latency_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver);

extern int
soc_th3_latency_wb_reinit(int unit, int level);
#endif

extern int
soc_th3_latency_show(int unit);

extern int
soc_th3_latency_diag(int unit);


#ifdef INCLUDE_AVS
extern int soc_th3_avs_init(int unit);
extern soc_error_t soc_th3_avs_vcore_get(int unit, uint32 *voltage);
extern soc_error_t soc_th3_avs_vcore_set(int unit, uint32 voltage);
extern soc_error_t soc_th3_avs_vcore_init(int unit);
extern soc_error_t soc_th3_avs_vcore_deinit(int unit);
#endif /* INCLUDE_AVS */

#ifdef PORTMOD_SUPPORT
extern int soc_th3_portctrl_pm_portmod_init(int unit);
extern int soc_th3_portctrl_pm_portmod_deinit(int unit);
extern int soc_th3_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config_info);
extern int soc_th3_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
extern int soc_th3_portctrl_pm_type_get(int unit, soc_port_t phy_port, int * pm_type);
extern int soc_th3_portctrl_vco_soc_info_update(int unit);
extern int soc_th3_portctrl_pm_vco_store_clear(int unit);
extern int soc_th3_portctrl_pm_vco_store(int unit, int index, const void *pm_vco_info);
extern int soc_th3_portctrl_pm_vco_fetch(int unit, int index, void *pm_vco_info);
extern int soc_th3_portctrl_pm_vco_reconfigure(int unit);
extern int soc_th3_portctrl_pm_is_active(int unit, int phy_port, int *is_active);

#endif
extern int soc_th3_portctrl_sbus_broadcast_setup(int unit);

extern int soc_th3_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_th3_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_th3_support_speeds(int unit, int lanes, uint32 *speed_mask);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_th3_flexport_scache_allocate(int unit);
extern int soc_th3_flexport_scache_sync(int unit);
extern int soc_th3_flexport_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern void soc_th3_flexport_sw_dump(int unit);
extern int soc_th3_device_speed_check(int unit, int speed, int lanes);

extern int soc_th3_ifp_mem_write(int unit, soc_mem_t mem, int copyno, int index,
                                void *entry_data);
extern int soc_th3_ifp_mem_read(int unit, soc_mem_t mem, int copyno, int index,
                               void *entry_data);
/*
  ==========================
   TH3 TM Functions
  ==========================
*/
extern int soc_tomahawk3_num_cosq_init(int unit, int port);
extern int soc_tomahawk3_port_pg_headroom_update(int unit, soc_port_t port);
extern int soc_tomahawk3_cpu_tx_rqe_queues_for_flush(int unit, int itm);
/*
  ==========================
   TH3 MMU SBus Functions
  ==========================
*/
extern int soc_tomahawk3_itm_base_index_check(int unit, int base_type, int itm,
                                             int base_index, char *msg);
extern int soc_tomahawk3_mem_is_itm(int unit, soc_mem_t mem);
extern int soc_tomahawk3_reg_is_itm(int unit, soc_reg_t reg);
extern int soc_tomahawk3_itm_reg32_set(int unit, soc_reg_t reg, int itm,
                                      int base_index, int index, uint32 data);
extern int soc_tomahawk3_itm_reg32_get(int unit, soc_reg_t reg, int itm,
                                      int base_index, int index, uint32 *data);
extern int soc_tomahawk3_itm_reg_set(int unit, soc_reg_t reg, int itm,
                                    int base_index, int index, uint64 data);
extern int soc_tomahawk3_itm_reg_get(int unit, soc_reg_t reg, int itm,
                                    int base_index, int index, uint64 *data);
extern int soc_tomahawk3_eb_base_index_check(int unit, int base_type, int eb,
                                            int base_index, char *msg);
extern int soc_tomahawk3_eb_reg32_set(int unit, soc_reg_t reg, int eb,
                                     int base_index, int index, uint32 data);
extern int soc_tomahawk3_eb_reg32_get(int unit, soc_reg_t reg, int eb,
                                     int base_index, int index, uint32 *data);
extern int soc_tomahawk3_eb_reg_set(int unit, soc_reg_t reg, int eb,
                                   int base_index, int index, uint64 data);
extern int soc_tomahawk3_eb_reg_get(int unit, soc_reg_t reg, int eb,
                                   int base_index, int index, uint64 *data);
extern int soc_tomahawk3_itm_mem_write(int unit, soc_mem_t mem, int itm,
          int base_index, int copyno, int offset_in_section, void *entry_data);
extern int soc_tomahawk3_itm_mem_read(int unit, soc_mem_t mem, int itm,
          int base_index, int copyno, int offset_in_section, void *entry_data);
extern int
soc_tomahawk3_itm_mem_read_range(int unit, soc_mem_t mem, int itm, int base_index,
                          int copyno, int idx_min, int idx_max, void *entry_data);
extern int soc_tomahawk3_eb_mem_write(int unit, soc_mem_t mem, int eb,
          int base_index, int copyno, int offset_in_section, void *entry_data);
extern int soc_tomahawk3_eb_mem_read(int unit, soc_mem_t mem, int eb,
          int base_index, int copyno, int offset_in_section, void *entry_data);

/*
  ==========================
   TH3 TM Init Misc Functions
  ==========================
*/
extern int soc_tomahawk3_mmu_port_mapping_init (int unit);
extern int soc_tomahawk3_en_mmu_refresh (int unit);
extern int soc_th3_mmu_config_init_thresholds(int unit, int test_only);
extern int soc_tomahawk3_init_tm_memory(int unit);
extern int soc_tomahawk3_ep_top_init(int unit);

/*
  ==========================
   TH3 TM Init MMU Functions
  ==========================
*/
extern int soc_tomahawk3_tm_port_speed_encoding(int unit);
extern int soc_tomahawk3_cell_pointer_init(int unit);
extern int soc_tomahawk3_mmu_port_rx_enables(int unit);
extern int soc_th3_mmu_rqe_queue_number_init(int unit);

extern uint32 _soc_th3_mmu_port(int unit, int port);
extern uint32 soc_th3_mmu_chip_port_num (int unit, int port);
extern uint32 soc_th3_mmu_local_port_num (int unit, int port);
extern void soc_tomahawk3_pipe_map_get(int unit, uint32 *pipe_map);
extern int soc_tomahawk3_itm_valid(int unit, int itm);
extern int soc_th3_ports_per_pm_get(int unit, int pm_id);

extern int soc_th3_l2x_attach(int unit);
extern int soc_th3_l2x_detach(int unit);
extern int soc_th3_l2_learn_thread_running(int unit, sal_usecs_t* interval);
extern int soc_th3_l2_learn_thread_start(int unit, int interval);
extern int soc_th3_l2_learn_thread_stop(int unit);
extern int soc_th3_l2_learn_alloc_resources(int unit);
extern int soc_th3_lrn_shadow_insert(int unit, l2x_entry_t *entry);
extern int soc_th3_lrn_shadow_delete(int unit, l2x_entry_t *entry);
extern int soc_th3_lrn_shadow_show(void *user_data, shr_avl_datum_t *datum,
                                   void *extra_data);
extern void soc_th3_lrn_cache_intr_handler(int unit, void *data);
extern int soc_th3_l2x_appl_callback_enable(int unit);
extern int soc_th3_l2x_appl_callback_disable(int unit);
extern int soc_th3_l2x_appl_callback_config_state_get(int unit);
extern int soc_th3_l2x_appl_callback_ready(int unit);
extern int soc_th3_l2x_appl_callback_entry_add(int unit, sal_mac_addr_t mac,
                                               vlan_id_t vlan, int state);
extern int soc_th3_l2x_appl_callback_entry_delete(int unit, sal_mac_addr_t mac,
                                                  vlan_id_t vlan);
extern int soc_th3_l2x_appl_callback_entry_lookup(int unit, sal_mac_addr_t mac,
                                                  vlan_id_t vlan, int *found,
                                                  int *enable);
extern int soc_th3_l2x_appl_callback_entry_delete_all(int unit);
extern int soc_th3_l2x_appl_callback_resources_destroy(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_th3_l2x_appl_callback_info_sync(int unit, uint8 **scache_ptr);
extern int soc_th3_l2x_appl_callback_info_restore(int unit, uint8 **scache_ptr);
extern void soc_th3_l2_scache_appl_callback_info_dump(int unit,
                                                      uint8 *scache_bmap_ptr,
                                                      int bmap_size);
#endif /* BCM_WARM_BOOT_SUPPORT */
extern void soc_tomahawk3_port_ratio_get_schedule_state(int unit,
                               soc_port_schedule_state_t *port_schedule_state,
                               int cdport, int *mode,
                               int prev_or_new);

extern int soc_tomahawk3_idb_init_update(int unit);
extern int soc_tomahawk3_cdport_init(int unit);
extern int soc_tomahawk3_tsc_clock_init(int unit);
extern int soc_tomahawk3_slot_pipeline_config(int unit);
extern int soc_tomahawk3_enable_ingress_forwarding(int unit);

/*
  ==========================
   TH3 PSTATS MMU Functions
  ==========================
*/

#define _TH3_MMU_PSTATS_ENABLE_ING      0x0001 /* Packetized Stat ING is enabled */
#define _TH3_MMU_PSTATS_ENABLE_EGR      0x0002 /* Packetized Stat EGR is enabled */
#define _TH3_MMU_PSTATS_ENABLE          0x0003
#define _TH3_MMU_PSTATS_HWM_MOD_ING     0x0004 /* Report High Watermark values for INGRESS */
#define _TH3_MMU_PSTATS_HWM_MOD_EGR     0x0008 /* Report High Watermark values for EGRESS */
#define _TH3_MMU_PSTATS_HWM_MOD         0x000c
#define _TH3_MMU_PSTATS_RESET_ON_READ_ING   0x0010 /* Enable Hardware clear-on read for INGRESS*/
#define _TH3_MMU_PSTATS_RESET_ON_READ_EGR   0x0020 /* Enable Hardware clear-on read for EGRESS*/
#define _TH3_MMU_PSTATS_RESET_ON_READ      0x0030
#define _TH3_MMU_PSTATS_MOR_EN          0x0100 /* Enable MOR capability */
#define _TH3_MMU_PSTATS_SYNC            0x8000 /* Read from device else the buffer */

extern void soc_tomahawk3_mmu_pstats_tbl_config_all(int unit);
extern void soc_tomahawk3_mmu_pstats_tbl_config(int unit);
extern int soc_tomahawk3_mmu_pstats_mode_set(int unit, uint32 flags);
extern int soc_tomahawk3_mmu_pstats_mode_get(int unit, uint32 *flags);
extern int soc_tomahawk3_mmu_pstats_mor_enable(int unit);
extern int soc_tomahawk3_mmu_pstats_mor_disable(int unit);
extern int soc_tomahawk3_mmu_splitter_reset(int unit);

/**************** TDM start ***********************/
#define _TH3_PKT_SCH_LENGTH             160
#define _TH3_TDM_CALENDAR_UNIVERSAL             0
#define _TH3_TDM_CALENDAR_ETHERNET_OPTIMIZED    1
#define _TH3_TDM_SPEED_CLASS_MAX                7

extern int soc_tomahawk3_tdm_update(int unit);

extern int soc_th3_retrieve_mmu_mem_list_for_port(int unit, int port, soc_mem_t mem,
        int blk_num, soc_mem_t *mem_list, int max_mem_cnt, int *mem_cnt);
extern int soc_tomahawk3_dpr_frequency_range_check(int unit, int dpr_freq);
extern void soc_th3_check_idb_portmap(int unit);
extern void soc_th3_check_mmu_portmap(int unit);
extern void soc_th3_check_settings_from_file(int unit, char* file_name);

/* Thresholds */
extern void _soc_th3_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
        int lossless);

/* Scheduler */
extern int soc_tomahawk3_sched_profile_set(int unit, int profile, int L0, int schedq,
        int mmuq);
extern int _soc_scheduler_profile_mapping_setup(int unit, _soc_mmu_cfg_scheduler_profile_t *sched_profile,
                                         int profile_index, int *L0, int *schedq, int *mmuq,
                                         int *cos_list, int *strict_priority, int *fc_is_uc_only);

extern int soc_tomahawk3_profile_exists(int unit, int profile);
extern int soc_tomahawk3_sched_profile_attach(int unit, soc_port_t port, int profile);
extern int soc_tomahawk3_sched_profile_get(int unit, soc_port_t port, int *profile);
extern int soc_tomahawk3_l1_to_l0_profile_set_per_port(int unit, int port, int profile, int L0, int schedq);
extern int soc_tomahawk3_l1_to_l0_profile_update_per_port(int unit, int port, int profile);


extern int soc_tomahawk3_tbl_entries_per_sram_entry_get(int unit, soc_mem_t mem,
                       int *table_entries_per_sram_entry);

extern int soc_tomahawk3_hash_mutex_init (int unit);

/* Flowtracker (ETRAP) */
extern int _bcm_th3_ft_dump_stats(int unit, int clear);

extern int _bcm_th3_ft_dump_flow_table(int unit, int pipe, int start_idx, int end_idx, int valid);
extern int soc_th3_skip_obm_mem_if_pm_down(int unit, soc_mem_t mem, int *skip);

extern int soc_th3_from_soc_attach(int unit, int val);


extern int
soc_tomahawk3_bond_info_init(int unit);

#ifdef INCLUDE_AVS
extern soc_error_t
soc_tomahawk3_avs_init(int unit);
#endif /* INCLUDE_AVS */

typedef struct _soc_pkt_size_dist_s
{
    /* Packet size in bytes; distribution percentage */
    int pkt_size;
    int dist_perc;
} _soc_pkt_size_dist_t;

extern int
soc_th3_get_port_pg_headroom (int unit, int port_speed,
        int cable_length, int array_size, _soc_pkt_size_dist_t *pkt_size_dst,
        int *pg_hdrm);


#endif /* !_soc_tomahawk3_H_ */
