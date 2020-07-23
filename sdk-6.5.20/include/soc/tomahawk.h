/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk.h
 */

#ifndef _SOC_TOMAHAWK_H_
#define _SOC_TOMAHAWK_H_

#include <soc/drv.h>
#include <soc/trident2.h>

#define UPDATE_ALL_PIPES -1

#define SOC_TH_MMU_BASE_TYPE_IPORT 0
#define SOC_TH_MMU_BASE_TYPE_EPORT 1
#define SOC_TH_MMU_BASE_TYPE_IPIPE 2
#define SOC_TH_MMU_BASE_TYPE_EPIPE 3
#define SOC_TH_MMU_BASE_TYPE_CHIP 4
#define SOC_TH_MMU_BASE_TYPE_XPE 5
#define SOC_TH_MMU_BASE_TYPE_SLICE 6
#define SOC_TH_MMU_BASE_TYPE_LAYER 7

#define SOC_TH_MEM_CHK_IFP_TCAM(mem) \
        (mem == IFP_TCAMm || \
         mem == IFP_TCAM_PIPE0m || mem == IFP_TCAM_PIPE1m || \
         mem == IFP_TCAM_PIPE2m || mem == IFP_TCAM_PIPE3m)
#define SOC_TH_MEM_CHK_IFP_TCAM_WIDE(mem) \
        (mem == IFP_TCAM_WIDEm || \
         mem == IFP_TCAM_WIDE_PIPE0m || mem == IFP_TCAM_WIDE_PIPE1m || \
         mem == IFP_TCAM_WIDE_PIPE2m || mem == IFP_TCAM_WIDE_PIPE3m)
#define SOC_TH_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem) \
        (mem == SRC_COMPRESSIONm || mem == DST_COMPRESSIONm || \
         mem == FP_UDF_TCAMm || mem == VFP_TCAMm || \
         mem == IFP_LOGICAL_TABLE_SELECTm || \
         mem == EXACT_MATCH_LOGICAL_TABLE_SELECTm || \
         mem == EFP_TCAMm || mem == IFP_TCAMm || mem == IFP_TCAM_WIDEm) \

#define IS_HG2_SPEED(speed) ((106000 == speed) ||  (53000 == speed) || \
                             (42000 == speed)  ||  (27000 == speed) || \
                             (21000 == speed)  ||  (11000 == speed))

#define CCLK_FREQ_850MHZ    850

#define SOC_TH_MMU_LOSSLESS_DEFAULT_ENABLE    1
#define SOC_TH_MMU_LOSSLESS_DEFAULT_DISABLE   0

extern soc_pbmp_t loopback_disable[SOC_MAX_NUM_DEVICES]; 

extern int soc_tomahawk_clear_mmu_memory(int unit);

extern int soc_th_mem_parity_control(int unit, soc_mem_t mem, int copyno,
                                     int en, int en_1b);

extern int soc_th_ser_reg_field32_modify(int unit, soc_reg_t reg,
                                         soc_port_t port, soc_field_t field,
                                         uint32 value, int index,
                                         int mmu_base_index);
extern int soc_th_ser_reg_field_modify(int unit, soc_reg_t reg,
                                       soc_port_t port, soc_field_t field,
                                       uint32 value, int index,
                                       int mmu_base_index);
extern int
soc_tomahawk_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout);

extern int
soc_th_lp_to_fv_index_remap(int unit, soc_mem_t mem, int *rpt_index);

extern int
soc_th_scan_idb_mem_ecc_status(int unit);

extern int
soc_th_check_scrub_info(int unit, soc_mem_t mem, int blk, int copyno,
                        int *num_inst_to_scrub, int *acc_type_list);
extern int soc_th_check_scrub_skip(int unit, soc_mem_t mem,
                                   int check_hw_global_mode);
extern int soc_th_mem_is_eligible_for_scan(int unit, soc_mem_t mem);
extern void soc_th_mem_scan_info_get(int unit, soc_mem_t mem, int block,
                         int *num_pipe, int *ser_flag);
extern int soc_th_check_cache_skip(int unit, soc_mem_t mem);
extern int soc_th_check_hw_global_mode(int unit, soc_mem_t mem, int *mem_mode);
extern int
soc_th_ifp_tcam_dma_read(int unit, soc_mem_t mem, int blk, uint32 *table,
                         int en_per_pipe_read, uint8 *vmap);

#if defined(SER_TR_TEST_SUPPORT)
extern int ser_test_th_mem_index_remap(int unit, ser_test_data_t *test_data,
                                       int *mem_has_ecc);
#endif /*defined(SER_TR_TEST_SUPPORT*/

extern int soc_th_mem_index_invalid(int unit, soc_mem_t mem, int index,
                                    uint8 dual_mode, soc_mem_t in_mem,
                                    int in_pipe);

extern int soc_th_mem_is_dual_mode(int unit, soc_mem_t mem,
                                   soc_mem_t *base_mem, int *pipe);
extern int soc_th_ifp_tcam_range_dma_read(int unit, soc_mem_t mem, int blk,
                                          int start_addr, uint32 num_entry, uint32 *table);
extern int soc_th_dual_tcam_index_valid(int unit, soc_mem_t mem, int index);
extern int
soc_tomahawk_show_ring_osc(int unit);

extern int
soc_th_ifp_slice_mode_hw_get(int unit, int pipe, int slice_num, int *slice_type,
                             int *slice_enabled);
extern int
soc_th_ifp_slice_mode_get(int unit, int pipe, int slice_num, int *slice_type,
                             int *slice_enabled);
extern int
soc_th_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                int mmu_base_index,
                                uint32 rval_intr_status_reg);
extern int soc_th_mmu_get_shared_size(int unit, int *ing_shd,
                                      int *egr_db_shd, int *egr_qe_shd);
extern int soc_th_mmu_config_shared_buf_recalc(int unit, int res, int *ing_shd,
                                               int *egr_db_shd, int *egr_qe_shd,
                                               int flags);
extern int soc_th_mmu_config_res_limits_update(int unit, int res, int *ing_shd,
                                               int *egr_db_shd, int *egr_qe_shd,
                                               int flags);
extern int soc_th_mmu_additional_buffer_reserve(int unit, int pipe, int flex,
                                                int lossless, int asf_profile,
                                                _soc_mmu_rsvd_buffer_t *rsvd_buffer);

extern int soc_th_mmu_get_egr_rsvd(int unit, int pipe, int *egr_rsvd);
extern int soc_th_recalc_new_egress_shared_limit(int unit, int pipe,
        int *new_shared_limit);

extern soc_functions_t soc_tomahawk_drv_funs;
extern int soc_tomahawk_tcam_ser_init(int unit);
extern int soc_tomahawk_ser_enable_all(int unit, int enable);
extern void soc_tomahawk_ser_fail(int unit);
extern void soc_tomahawk_ser_error(void *unit_vp, void *d1, void *d2,
                                   void *d3, void *d4);
extern int soc_tomahawk_mem_ser_control(int unit, soc_mem_t mem,
                                        int copyno, int enable);
#if defined(INCLUDE_FLOWTRACKER)
typedef int (*soc_th_ft_em_ser_handler_t)(int unit, int pipe, soc_mem_t mem, int index);
void soc_th_ft_em_ser_handler_register(int unit, soc_th_ft_em_ser_handler_t handler);
int soc_th_ft_em_ser_process(int unit, int pipe, soc_mem_t mem, int index);
#endif /* INCLUDE_FLOWTRACKER */

#ifdef BCM_TOMAHAWK2_SUPPORT
extern int soc_th2_tcam_engine_enable(int unit, int enable);
#endif

extern int soc_tomahawk_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                              int enable, int *orig_enable);
extern int soc_tomahawk_reg_cpu_write_control(int unit, int enable);
extern int soc_tomahawk_ser_mem_clear(int unit, soc_mem_t mem);
extern void soc_tomahawk_ser_register(int unit);
extern int soc_tomahawk_process_ser_fifo(int unit, soc_block_t blk, int pipe,
                                         char *prefix_str, int l2_mgmt_ser_fifo,
                                         int reg_type, int cmic_bit);
extern int soc_tomahawk_alpm_scrub(int unit);
extern int soc_tomahawk_mem_config(int unit);
extern int soc_tomahawk_num_cosq_init_port(int unit, int port);
extern int soc_tomahawk_num_cosq_init(int unit);
extern void soc_tomahawk_pipe_map_get(int unit, uint32 *pipe_map);
extern void soc_tomahawk_port_ratio_get(int unit, int initTime, int clport, int *mode);

extern int soc_tomahawk_port_config_init(int unit, uint16 dev_id);
extern int soc_tomahawk_chip_reset(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_tomahawk_chip_warmboot_reset(int unit);
#endif
extern int soc_tomahawk_port_reset(int unit);
extern int soc_tomahawkplus_port_reset(int unit);
extern int soc_tomahawk_xpe_base_index_check(int unit, int base_type, int xpe,
                                             int base_index, char *msg);
extern int soc_tomahawk_mmu_layer_get(int unit, int pipe, int *layer);
extern int soc_tomahawk_xpe_reg32_set(int unit, soc_reg_t reg, int xpe,
                                      int base_index, int index, uint32 data);
extern int soc_tomahawk_xpe_reg32_get(int unit, soc_reg_t reg, int xpe,
                                      int base_index, int index, uint32 *data);
extern int soc_tomahawk_xpe_reg_set(int unit, soc_reg_t reg, int xpe,
                                    int base_index, int index, uint64 data);
extern int soc_tomahawk_xpe_reg_get(int unit, soc_reg_t reg, int xpe,
                                    int base_index, int index, uint64 *data);
extern int soc_tomahawk_xpe_mem_write(int unit, soc_mem_t mem, int xpe,
                                      int base_index, int copyno,
                                      int offset_in_section, void *entry_data);
extern int soc_tomahawk_xpe_mem_read(int unit, soc_mem_t mem, int xpe,
                                     int base_index, int copyno,
                                     int offset_in_section, void *entry_data);
extern int soc_tomahawk_mem_xpe_pipe_check(int unit, soc_mem_t mem,
                                           int xpe, int pipe);
extern int soc_tomahawk_mem_basetype_get(int unit, soc_mem_t mem);
extern int soc_tomahawk_mem_is_xpe(int unit, soc_mem_t mem);
extern int soc_tomahawk_sc_base_index_check(int unit, int base_type, int sc,
                                            int base_index, char *msg);
extern int soc_tomahawk_sc_reg32_set(int unit, soc_reg_t reg, int sc,
                                     int base_index, int index, uint32 data);
extern int soc_tomahawk_sc_reg32_get(int unit, soc_reg_t reg, int sc,
                                     int base_index, int index, uint32 *data);
extern int soc_tomahawk_sc_reg_set(int unit, soc_reg_t reg, int sc,
                                   int base_index, int index, uint64 data);
extern int soc_tomahawk_sc_reg_get(int unit, soc_reg_t reg, int sc,
                                   int base_index, int index, uint64 *data);
extern int soc_tomahawk_port_obm_info_get(int unit, soc_port_t port,
                                          int *obm_id, int *lane);

extern int soc_tomahawk_port_traffic_egr_enable_set(int unit, int port, int enable);

#if 0
extern int soc_tomahawk_sc_mem_write(int unit, soc_mem_t mem, int sc,
                                     int base_index, int copyno,
                                     int offset_in_section, void *entry_data);
extern int soc_tomahawk_sc_mem_read(int unit, soc_mem_t mem, int sc,
                                     int base_index, int copyno,
                                     int offset_in_section, void *entry_data);
#endif
extern uint32 _soc_th_piped_mem_index(int unit, soc_port_t port,
                                      soc_mem_t mem, int arr_off);

extern int soc_th_l2_bulk_age_start(int unit, int interval);
extern int soc_th_l2_bulk_age_stop(int unit);

/* Counter */
extern int soc_counter_tomahawk_status_enable(int unit, int enable);
extern int soc_counter_tomahawk_non_dma_init(int unit, int nports,
                                             int non_dma_start_index,
                                             int *non_dma_entries);
extern int soc_counter_tomahawk_get_info(int unit, soc_port_t port,
                                         soc_reg_t id, int *base_index,
                                         int *num_entries);
extern int soc_counter_tomahawk_eviction_enable(int unit, int enable);
extern void soc_counter_tomahawk_eviction_flags_update(int unit,
                                                       uint32 flags,
                                                       int enable);
extern int soc_counter_tomahawk_generic_get_info(int unit,  soc_ctr_control_info_t ctrl_info,
                                            soc_reg_t id, int *base_index, int *num_entries);
extern int soc_counter_tomahawk_eviction_init(int unit);
extern int soc_counter_tomahawk_get_child_dma(int unit, soc_reg_t id,
                                    soc_ctr_control_info_t ctrl_info,
                                    soc_counter_non_dma_t **child_dma);
extern int soc_counter_tomahawk_dma_flags_update(int unit,
                                    soc_counter_non_dma_t *ctr_dma,
                                    uint32 flags, int val);

extern int
soc_counter_th_extra_sflow_ctrs_init(int unit,
                                     soc_counter_non_dma_t *non_dma_parent,
                                     soc_counter_non_dma_t *non_dma_extra,
                                     uint32 extra_ctr_ct,
                                     int *total_entries);
extern int
soc_counter_th_extra_ctrs_init(int unit, soc_reg_t id,
                               soc_counter_non_dma_t *non_dma_parent,
                               soc_counter_non_dma_t *non_dma_extra,
                               uint32 extra_ctr_ct,
                               int *total_entries);
extern int
soc_counter_th_extra_obm_ctrs_init(int unit,
                                   soc_counter_non_dma_t *non_dma_parent,
                                   soc_counter_non_dma_t *non_dma_extra,
                                   uint32 extra_ctr_ct,
                                   int *total_entries);
extern int soc_counter_tomahawk_ctr_dma_post_init(int unit);

#if defined(INCLUDE_TELEMETRY) || defined(INCLUDE_IFA) || defined(INCLUDE_FLOWTRACKER)
extern int soc_counter_egr_perq_xmt_ctr_evict_disable(int unit);

extern int soc_counter_egr_perq_xmt_ctr_evict_enable(int unit);
#endif /* INCLUDE_TELEMETRY || INCLUDE_IFA || INCLUDE_FLOWTRACKER */

extern soc_error_t
soc_th_granular_speed_get(int unit, soc_port_t port, int *speed);

typedef enum {
    /* WARNING: values given match hardware register; do not modify */
    SOC_TH_SPEED_CLASS_0 = 0,
    SOC_TH_SPEED_CLASS_10G = 1,
    SOC_TH_SPEED_CLASS_20G = 2,
    SOC_TH_SPEED_CLASS_25G = 3,
    SOC_TH_SPEED_CLASS_40G = 4,
    SOC_TH_SPEED_CLASS_50G = 5,
    SOC_TH_SPEED_CLASS_100G = 6
} soc_th_speed_classes_e;

typedef enum {
    /* WARNING: values given match hardware register; do not modify */
    SOC_TH_PORT_MODE_QUAD = 0,
    SOC_TH_PORT_MODE_TRI_012 = 1,
    SOC_TH_PORT_MODE_TRI_023 = 2,
    SOC_TH_PORT_MODE_DUAL = 3,
    SOC_TH_PORT_MODE_SINGLE = 4
} soc_th_port_mode_e;

#define TH_FLEXPORT_SPEED_SET_NUM_LANES -1

typedef struct soc_th_port_lanes_s {
    int port_base;      /* Base port number */
    int lanes;          /* New number of lanes */
    int cur_lanes;      /* Current number of lanes */
    int mode;           /* New mode */
    int cur_mode;       /* Current mode */
    int ovs_class;      /* Speed class for new group */
    int cur_ovs_class;  /* Speed class for current group */
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
    int os_mixed_sister; /* enable or disable mixed sister port */
    int mixed_port_mode; /* mixed sister port mode */

    int action_id;
    int pipe;
    int speed_id;
    int port_mode;
    int src_idb_group;
    int dst_idb_group;
    int src_mmu_group;
    int dst_mmu_group;
} soc_th_port_lanes_t;

typedef struct soc_th_speed_group_slot_s {
    int slot;
    int pm_id;
    int port_id;
} soc_th_speed_group_slot_t;

typedef struct soc_th_speed_group_count_s {
    int group_id;
    int count_ports;
} soc_th_speed_group_count_t;

typedef struct soc_speed_group_info_s {
    soc_th_speed_classes_e  speed_id;
    soc_th_port_mode_e      port_mode;
    int   p_pm;                          /* the maximum number of ports of that port speed in one PM */
    int   n_group;                       /* the maximum number of PMs of that speed group */
    char *des;
} soc_speed_group_info_t;

extern int soc_tomahawk_port_lanes_consolidation(int unit,
                                                 soc_th_port_lanes_t *lanes_ctrl);
extern int soc_tomahawk_port_lanes_validate(int unit,
                                            soc_th_port_lanes_t *lanes_ctrl);
extern int soc_tomahawk_port_lanes_set(int unit,
                                       soc_th_port_lanes_t *lanes_ctrl);
extern int soc_tomahawk_port_lanes_get(int unit, soc_port_t port_base,
                                       int *cur_lanes);
extern int soc_tomahawk_port_speed_update(int unit, soc_port_t port, int speed);

extern int soc_th_port_ie_speed_get(int unit, soc_port_t port, int *speed);
extern int soc_th_port_hg_speed_get(int unit, soc_port_t port, int *speed);

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void soc_th_flexport_sw_dump(int unit);
extern void soc_th_flexport_sw_pipe_speed_group_dump(int unit,
                                                     int pipe,
                                                     int speed_id);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

extern soc_error_t
soc_th_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp);

soc_error_t
soc_th_port_speed_recover(int unit, soc_port_t port);

extern soc_error_t
soc_th_flexport_recover(int unit, soc_port_t port);

extern soc_error_t
soc_th_port_speed_supported(int unit, soc_port_t port, int speed);
extern soc_error_t
soc_th_speed_set_init_ctrl(int unit, soc_th_port_lanes_t *lanes_ctrl);

extern soc_error_t
soc_tomahawk_edb_buf_reset(int unit, soc_port_t port, int reset);

extern soc_error_t
soc_tomahawk_idb_buf_reset(int unit, soc_port_t port, int reset);
typedef int (*_soc_th_bst_hw_cb)(int unit);
extern int soc_th_set_bst_callback(int unit, _soc_th_bst_hw_cb cb);
extern int _soc_th_process_mmu_bst(int unit, int xpe, soc_field_t field);

extern int soc_th_ports_per_pm_get(int unit, int pm_id);

typedef void (*soc_th_mmu_config_buf_default_f)(int unit, 
              _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg, 
              int lossless);

extern int
soc_th_port_oversub_ratio_get(
    int          unit,
    soc_port_t   port,
    int* const   ratio);

#define SOC_TH_MMU_PORT(u, p)  _soc_th_mmu_port((u),(p))
#define SOC_TH_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    _soc_th_piped_mem_index((u),(p),(m),(off))
#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

enum soc_th_port_ratio_e {
    SOC_TH_PORT_RATIO_SINGLE,
    SOC_TH_PORT_RATIO_DUAL_1_1,
    SOC_TH_PORT_RATIO_DUAL_2_1,
    SOC_TH_PORT_RATIO_DUAL_1_2,
    SOC_TH_PORT_RATIO_TRI_023_2_1_1,
    SOC_TH_PORT_RATIO_TRI_023_4_1_1,
    SOC_TH_PORT_RATIO_TRI_012_1_1_2,
    SOC_TH_PORT_RATIO_TRI_012_1_1_4,
    SOC_TH_PORT_RATIO_QUAD,
    SOC_TH_PORT_RATIO_COUNT
};

enum soc_th_mixed_port_mode_e {
    MIXED_DUAL_MODE_0 = 0,
    MIXED_DUAL_MODE_20_0_20_0 = 1,
    MIXED_DUAL_MODE_40_0_40_0 = 2,
    MIXED_DUAL_MODE_50_0_50_0 = 3,
    MIXED_DUAL_MODE_25_0_50_0 = 4,
    MIXED_DUAL_MODE_50_0_25_0 = 5,
    MIXED_DUAL_MODE_25_0_25_0 = 6
};

enum soc_th_port_lanes_action_e {
    ACT_NONE,
    ACT_CONSOLIDATE,
    ACT_MIGRATE
};

enum th_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH_L2_HASH_KEY_TYPE_BRIDGE = 0,
    TH_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    TH_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    TH_L2_HASH_KEY_TYPE_VFI = 3,
    TH_L2_HASH_KEY_TYPE_VIF = 4,
    TH_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS = 5,
    TH_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG = 6,
    TH_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT = 7,
    TH_L2_HASH_KEY_TYPE_BFD = 8,
    TH_L2_HASH_KEY_TYPE_PE_VID = 9,
    TH_L2_HASH_KEY_TYPE_COUNT
};

enum th_l3_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH_L3_HASH_KEY_TYPE_V4UC = 0,
    TH_L3_HASH_KEY_TYPE_V4UC_EXT = 1,
    TH_L3_HASH_KEY_TYPE_V6UC = 2,
    TH_L3_HASH_KEY_TYPE_V6UC_EXT = 3,
    TH_L3_HASH_KEY_TYPE_V4MC = 4,
    TH_L3_HASH_KEY_TYPE_V6MC = 5,
    /* 6 to 7 are not used */
    TH_L3_HASH_KEY_TYPE_TRILL = 8,
    /* 9 to 17 are not used */
    TH_L3_HASH_KEY_TYPE_DST_NAT = 18,
    TH_L3_HASH_KEY_TYPE_DST_NAPT = 19,
    TH_L3_HASH_KEY_TYPE_COUNT
};

enum th_fpem_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH_FPEM_HASH_KEY_TYPE_128B = 0,
    TH_FPEM_HASH_KEY_TYPE_160B = 1,
    TH_FPEM_HASH_KEY_TYPE_320B = 2,
    TH_FPEM_HASH_KEY_TYPE_COUNT
};

enum th_mpls_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH_MPLS_HASH_KEY_TYPE_MPLS = 0,
    TH_MPLS_HASH_KEY_TYPE_MIM_NVP = 1,
    TH_MPLS_HASH_KEY_TYPE_MIM_ISID = 2,
    TH_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP = 3,
    TH_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP = 4,
    TH_MPLS_HASH_KEY_TYPE_TRILL = 5,
    TH_MPLS_HASH_KEY_TYPE_L2GRE_SIP = 6,
    TH_MPLS_HASH_KEY_TYPE_L2GRE_VPNID = 7,
    TH_MPLS_HASH_KEY_TYPE_VXLAN_SIP = 8,
    TH_MPLS_HASH_KEY_TYPE_VXLAN_VPNID = 9,
    TH_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP = 10,
    TH_MPLS_HASH_KEY_TYPE_COUNT
};

enum th_vlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    TH_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    TH_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    TH_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    TH_VLXLT_HASH_KEY_TYPE_OVID = 4,
    TH_VLXLT_HASH_KEY_TYPE_IVID = 5,
    TH_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    TH_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    TH_VLXLT_HASH_KEY_TYPE_VIF = 8,
    TH_VLXLT_HASH_KEY_TYPE_VIF_VLAN = 9,
    TH_VLXLT_HASH_KEY_TYPE_VIF_CVLAN = 10,
    TH_VLXLT_HASH_KEY_TYPE_VIF_OTAG = 11,
    TH_VLXLT_HASH_KEY_TYPE_VIF_ITAG = 12,
    TH_VLXLT_HASH_KEY_TYPE_L2GRE_DIP = 13,
    TH_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT = 14,
    /* 15 to 17 are not used */
    TH_VLXLT_HASH_KEY_TYPE_VXLAN_DIP = 18,
    TH_VLXLT_HASH_KEY_TYPE_COUNT
};

enum th_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TH_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE = 0,
    TH_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP = 1,
    /* 2 is not used */
    TH_EVLXLT_HASH_KEY_TYPE_ISID_XLATE = 3,
    TH_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE = 4,
    TH_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI = 5,
    TH_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP = 6,
    /* 7 is not used */
    TH_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI = 8,
    TH_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP = 9,
    TH_VELXLT_HASH_KEY_TYPE_COUNT
};

extern int      soc_tomahawk_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_tomahawk_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_tomahawk_hash_bank_number_get(int unit, soc_mem_t mem,
                                                  int seq_num, int *bank_num);
extern int      soc_tomahawk_hash_seq_number_get(int unit, soc_mem_t mem,
                                                 int bank_num, int *seq_num);
extern int      soc_tomahawk_hash_bank_info_get(int unit, soc_mem_t mem,
                                                int bank,
                                                int *entries_per_bank,
                                                int *entries_per_row,
                                                int *entries_per_bucket,
                                                int *bank_base,
                                                int *bucket_offset);
extern int      soc_th_hash_bucket_get(int unit, int mem, int bank,
                                       uint32 *entry, uint32 *bucket);
extern int      soc_th_hash_index_get(int unit, int mem, int bank, int bucket);
extern int      soc_th_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                       int *hash_offset, int *use_lsb);
extern int      soc_th_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                       int hash_offset, int use_lsb);
extern uint32   soc_th_l2x_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key);
extern int      soc_th_l2x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                             uint8 *key);
extern uint32   soc_th_l2x_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_th_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_th_l3x_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key);
extern int      soc_th_l3x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                             uint8 *key);
extern uint32   soc_th_l3x_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_th_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_th_exact_match_hash(int unit, int bank, int hash_offset,
                                        int use_lsb, int key_nbits,
                                        void *base_entry, uint8 *key);
extern int      soc_th_exact_match_base_entry_to_key(int unit, int bank,
                                                     uint32 *entry,
                                                     uint8 *key);
extern uint32   soc_th_exact_match_entry_hash(int unit, int bank,
                                              int hash_offset, int use_lsb,
                                              uint32 *entry);
extern uint32   soc_th_exact_match_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32   soc_th_mpls_hash(int unit, int hash_sel, int key_nbits,
                                 void *base_entry, uint8 *key);
extern int      soc_th_mpls_base_entry_to_key(int unit, int bank, void *entry,
                                              uint8 *key);
extern uint32   soc_th_mpls_entry_hash(int unit, int hash_sel, int bank,
                                       uint32 *entry);
extern uint32   soc_th_mpls_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_th_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                                       void *base_entry, uint8 *key);
extern int      soc_th_vlan_xlate_base_entry_to_key(int unit, int bank,
                                                    void *entry,
                                                    uint8 *key);
extern uint32   soc_th_vlan_xlate_entry_hash(int unit, int hash_sel, int bank,
                                             uint32 *entry);
extern uint32   soc_th_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                  uint32 *entry);
extern uint32   soc_th_egr_vlan_xlate_hash(int unit, int hash_sel,
                                           int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_th_egr_vlan_xlate_base_entry_to_key(int unit, int bank,
                                                        void *entry,
                                                        uint8 *key);
extern uint32   soc_th_egr_vlan_xlate_entry_hash(int unit, int hash_sel,
                                                 int bank, uint32 *entry);
extern uint32   soc_th_egr_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                      uint32 *entry);
extern uint32   soc_th_ing_vp_vlan_member_hash(int unit, int hash_sel,
                                               int key_nbits, void *base_entry,
                                               uint8 *key);
extern int      soc_th_ing_vp_vlan_member_base_entry_to_key(int unit, int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_th_ing_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                     int bank,
                                                     uint32 *entry);
extern uint32   soc_th_ing_vp_vlan_member_bank_entry_hash(int unit,
                                                     int bank, 
                                                     uint32 *entry);
extern uint32   soc_th_egr_vp_vlan_member_hash(int unit, int hash_sel,
                                               int key_nbits, void *base_entry,
                                               uint8 *key);
extern int      soc_th_egr_vp_vlan_member_base_entry_to_key(int unit, int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_th_egr_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                     int bank,
                                                     uint32 *entry);
extern uint32   soc_th_egr_vp_vlan_member_bank_entry_hash(int unit,
                                                     int bank,
                                                     uint32 *entry);
extern uint32   soc_th_ing_dnat_address_type_hash(int unit, int hash_sel,
                                                  int key_nbits,
                                                  void *base_entry,
                                                  uint8 *key);
extern int      soc_th_ing_dnat_address_type_base_entry_to_key(int unit,
                                                               int bank,
                                                               void *entry,
                                                               uint8 *key);
extern uint32   soc_th_ing_dnat_address_type_entry_hash(int unit,
                                                        int hash_sel,
                                                        int bank,
                                                        uint32 *entry);
extern uint32   soc_th_ing_dnat_address_type_bank_entry_hash(int unit,
                                                        int bank,
                                                        uint32 *entry);

/* Special port number used by H/W */
#define _SOC_TH_TDM_OVERSUB_TOKEN    0x22
#define _SOC_TH_TDM_NULL_TOKEN       0x23
#define _SOC_TH_TDM_IDL1_TOKEN       0x24
#define _SOC_TH_TDM_IDL2_TOKEN       0x25
#define _SOC_TH_TDM_UNUSED_TOKEN     0x3f

typedef enum {
    SOC_TH_SCHED_MODE_UNKNOWN = 0,
    SOC_TH_SCHED_MODE_STRICT,
    SOC_TH_SCHED_MODE_WRR,
    SOC_TH_SCHED_MODE_WERR
} soc_th_sched_mode_e;

typedef enum {
    SOC_TH_NODE_LVL_ROOT = 0,
    SOC_TH_NODE_LVL_L0,
    SOC_TH_NODE_LVL_L1,
    SOC_TH_NODE_LVL_L2,
    SOC_TH_NODE_LVL_MAX
} soc_th_node_lvl_e;

typedef enum {
    _SOC_TH_INDEX_STYLE_BUCKET,
    _SOC_TH_INDEX_STYLE_BUCKET_MODE,
    _SOC_TH_INDEX_STYLE_WRED,
    _SOC_TH_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_TH_INDEX_STYLE_SCHEDULER,
    _SOC_TH_INDEX_STYLE_PERQ_XMT,
    _SOC_TH_INDEX_STYLE_UCAST_DROP,
    _SOC_TH_INDEX_STYLE_UCAST_QUEUE,
    _SOC_TH_INDEX_STYLE_MCAST_QUEUE,
    _SOC_TH_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_TH_INDEX_STYLE_EGR_POOL,
    _SOC_TH_INDEX_STYLE_COUNT
} soc_th_index_style_t;

typedef enum soc_th_drop_limit_alpha_value_e {
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_TH_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_th_drop_limit_alpha_value_t;

#define _TH_PORTS_PER_PBLK             4
#define _TH_PBLKS_PER_PIPE             8
#define _TH_PIPES_PER_DEV              4
#define _TH_XPES_PER_DEV               4
#define _TH_MMU_PORTS_PER_PIPE         34

#define _TH_PBLKS_PER_DEV              (_TH_PBLKS_PER_PIPE * _TH_PIPES_PER_DEV)

#define _TH_PORTS_PER_PIPE             (_TH_PORTS_PER_PBLK * _TH_PBLKS_PER_PIPE)
#define _TH_PORTS_PER_DEV              (_TH_PORTS_PER_PIPE * _TH_PIPES_PER_DEV)
/* Device port */
/* 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH_GDEV_PORTS_PER_PIPE        32
#define _TH_DEV_PORTS_PER_PIPE         (_TH_GDEV_PORTS_PER_PIPE + 2)

#define _TH_SECTION_SIZE                32768

#define _TH_MMU_MAX_PACKET_BYTES        9416  /* bytes */
#define _TH_MMU_PACKET_HEADER_BYTES     64    /* bytes */
#define _TH_MMU_JUMBO_FRAME_BYTES       9216  /* bytes */
#define _TH_MMU_DEFAULT_MTU_BYTES       1536  /* bytes */

#define _TH_MMU_PHYSICAL_CELLS_PER_XPE  23040 /* Total Physical cells per XPE */
#define _THP_MMU_PHYSICAL_CELLS_PER_XPE 29184 /* Total Physical cells per XPE for TH+*/

#define _TH_MMU_TOTAL_CELLS_PER_XPE     20165 /* 4MB/XPE = 20165 cells/XPE */
#define _THP_MMU_TOTAL_CELLS_PER_XPE    27727 /* 5.5MB/XPE = 27727 cells/XPE for TH+*/

#define _TH_MMU_RSVD_CELLS_CFAP_PER_XPE 720   /* Reserved CFAP cells per XPE */
#define _THP_MMU_RSVD_CELLS_CFAP_PER_XPE 912  /* Reserved CFAP cells per XPE for TH+*/

#define _TH_MMU_TOTAL_CELLS             (_TH_MMU_TOTAL_CELLS_PER_XPE * _TH_XPES_PER_DEV)
#define _THP_MMU_TOTAL_CELLS            (_THP_MMU_TOTAL_CELLS_PER_XPE * _TH_XPES_PER_DEV)

#define _TH_MMU_PHYSICAL_CELLS          (_TH_MMU_PHYSICAL_CELLS_PER_XPE * _TH_XPES_PER_DEV)
#define _THP_MMU_PHYSICAL_CELLS         (_THP_MMU_PHYSICAL_CELLS_PER_XPE * _TH_XPES_PER_DEV)

#define _TH_MMU_BYTES_PER_CELL          208   /* bytes (1664 bits) */
#define _TH_MMU_NUM_PG                  8
#define _TH_MMU_NUM_POOL                4
#define _TH_MMU_NUM_RQE_QUEUES          11
#define _TH_MMU_NUM_INT_PRI             16

#define _TH_THDI_BUFFER_CELL_LIMIT_SP_MAX           19968
#define _THP_THDI_BUFFER_CELL_LIMIT_SP_MAX          27727  /* Higher for TH+ as per MMU settings 2.0 from Arch */
#define _TH_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX      19968
#define _THP_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX     27727  /* Higher for TH+ as per MMU settings 2.0 from Arch */

#define SOC_TH_MMU_CFG_QGROUP_MAX       36

#define SOC_TH_MMU_PORT_STRIDE          64    /* MMU Port number space per pipe */

#define SOC_TH_MMU_GP_UCQ_NUM           320   /* Generl ports UC Queue Num in per Pipe Queue Space */
#define SOC_TH_MMU_MGMT_UCQ_NUM         10    /* Management port UC Queues Num in Pipe Queue  */
#define SOC_TH_MMU_GP_MCQ_NUM           320   /* Generl ports MC Queue Num in per Pipe Queue Space */
#define SOC_TH_MMU_LB_MCQ_NUM           10    /* Loopback port MC Queue Num in per Pipe Queue Space */
#define SOC_TH_NUM_CPU_QUEUES           48    /* CPU port has 48 MC Queues */
#define SOC_TH_NUM_UC_QUEUES_PER_PIPE   ((SOC_TH_MMU_GP_UCQ_NUM) + \
                                         (SOC_TH_MMU_MGMT_UCQ_NUM)) /* Num UC Q's Per pipe 33*10 */
#define SOC_TH_NUM_MC_QUEUES_PER_PIPE   ((SOC_TH_MMU_GP_MCQ_NUM) + (SOC_TH_MMU_LB_MCQ_NUM) + \
                                         (SOC_TH_NUM_CPU_QUEUES))   /* Num MC Q's Per pipe (33*10)+48 */
#define SOC_TH_MMU_CPU_MCQ_OFFSET       ((SOC_TH_MMU_GP_UCQ_NUM) + (SOC_TH_MMU_MGMT_UCQ_NUM) + \
                                         (SOC_TH_MMU_GP_MCQ_NUM) + (SOC_TH_MMU_LB_MCQ_NUM))
#define SOC_TH_MMU_MGMT_UCQ_OFFSET      (SOC_TH_MMU_GP_UCQ_NUM)
#define SOC_TH_MMU_MGMT_MCQ_OFFSET      ((SOC_TH_MMU_GP_UCQ_NUM) + (SOC_TH_MMU_MGMT_UCQ_NUM) + \
                                         (SOC_TH_MMU_GP_MCQ_NUM) + (SOC_TH_MMU_LB_MCQ_NUM))

#define SOC_TH_MMU_MCQ_ENTRY_PER_XPE    8192
#define SOC_TH_MMU_RQE_ENTRY_PER_XPE    1024
#define SOC_TH_MMU_RQE_ENTRY_RSVD_PER_XPE 160   /* Reserved RQE QE per XPE */

#define _TH_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_TH_MMU_MCQ_ENTRY_PER_XPE
#define _TH_MMU_TOTAL_RQE_ENTRY(unit)   SOC_TH_MMU_RQE_ENTRY_PER_XPE

#define SOC_TH_STAT_ING_FLEX_POOL_MAX   20

#define _TH_ARRAY_MIN(arr, len, bmp, min)                   \
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


/* Function to Retrieve IFP memory mode. */
extern int
soc_th_ser_mem_mode_get(int unit, soc_mem_t mem, int *mode);

extern int
soc_th_cosq_sched_mode_set(int unit, soc_port_t port, int level, int index,
                           soc_th_sched_mode_e mode, int weight, int mc);
extern int
soc_th_cosq_sched_mode_get(int unit, soc_port_t port, int level, int index,
                           soc_th_sched_mode_e *mode, int *weight, int mc);
extern int
soc_th_cosq_cpu_parent_set(int unit, int child_index, int child_level,
                           int parent_index);
extern int
soc_th_cosq_cpu_parent_get(int unit, int child_index, int child_level,
                           int *parent_index);

extern int
soc_tomahawk_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                               _soc_ser_sram_info_t *sram_info);

#if defined(SER_TR_TEST_SUPPORT)
extern void soc_th_ser_test_register(int unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/

extern int soc_tomahawk_clear_all_port_data(int unit);

#define SOC_TH_NUM_EL_VLAN_XLATE                     4

#define SOC_TH_NUM_EL_EGR_VLAN_XLATE                 4

#define SOC_TH_NUM_EL_ING_L3_NEXT_HOP                4
#define SOC_TH_RAM_OFFSET_ING_L3_NEXT_HOP            8192

#define SOC_TH_NUM_ENTRIES_L2_BANK                   8192
#define SOC_TH_NUM_ENTRIES_L3_BANK(u)                (SOC_IS_TOMAHAWK3(u) ? 16384 : 8192)

#define SOC_TH_NUM_EL_SHARED                         4
#define SOC_TH_NUM_ENTRIES_XOR_BANK                  8192
#define SOC_TH_LP_ENTRIES_IN_XOR_BANK                2048
#define SOC_TH_LP_ENTRIES_IN_UFT_BANK                8192

#define SOC_TH_ALPM_MAX_BKTS(u)                      (SOC_IS_TOMAHAWK(u) ? 8192 : 16384)
#define SOC_TH_ALPM_BKT_MASK(u)                      (SOC_IS_TOMAHAWK(u) ? 0x1FFF : 0x3FFF)
#define SOC_TH_ALPM_BKT_OFFFSET                      2048
#define SOC_TH_ALPM_MODE0_BKT_MASK                   0x38003
#define SOC_TH_ALPM_MODE1_BKT_MASK                   SOC_TH_ALPM_MODE0_BKT_MASK >> 1

#define _SOC_TH_L2_MGMT_INTR_MASK                    0x2
#define _SOC_TH_FUNC_INTR_MASK                       _SOC_TH_L2_MGMT_INTR_MASK

extern void soc_th_set_alpm_banks(int unit, int alpm_banks);
extern int soc_th_get_alpm_banks(int unit);
extern int soc_tomahawk_alpm_mode_get(int unit);

extern int
soc_tomahawk_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count);
extern void
soc_tomahawk_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int
soc_tomahawk_show_voltage(int unit);
extern void
soc_tomahawk_process_func_intr(void *unit_vp, void *d1, void *d2, void *d3, void *d4);

extern void
_soc_tomahawk_alpm_bkt_view_set(int unit, int index, soc_mem_t view);
extern soc_mem_t
_soc_tomahawk_alpm_bkt_view_get(int unit, int index);

#define _SOC_TH_UNIT_VALIDATE(unit)                                \
        if ( !(((unit) >= 0) && ((unit) <= SOC_MAX_NUM_DEVICES)) ) \
            return SOC_E_UNIT;

#define OUT
#define IN_OUT

/*
  =============================
   TH ASF (cut-thru forwarding)
  =============================
*/

/* ASF Modes */
typedef enum {
    _SOC_TH_ASF_MODE_SAF          = 0,  /* store-and-forward */
    _SOC_TH_ASF_MODE_SAME_SPEED   = 1,  /* same speed CT  */
    _SOC_TH_ASF_MODE_SLOW_TO_FAST = 2,  /* slow to fast CT   */
    _SOC_TH_ASF_MODE_FAST_TO_SLOW = 3,  /* fast to slow CT   */
    _SOC_TH_ASF_MODE_CFG_UPDATE   = 4,  /* internal cfg updates */
    _SOC_TH_ASF_MODE_INIT         = 5,  /* init */
    _SOC_TH_ASF_MODE_UNSUPPORTED  = 6
} soc_th_asf_mode_e;

/* ASF MMU Cell Buffer Allocation Profiles */
typedef enum {
    _SOC_TH_ASF_MEM_PROFILE_NONE      = 0,  /* No cut-through support */
    _SOC_TH_ASF_MEM_PROFILE_SIMILAR   = 1,  /* Similar Speed Alloc Profile  */
    _SOC_TH_ASF_MEM_PROFILE_EXTREME   = 2   /* Extreme Speed Alloc Profile */
} soc_th_asf_mem_profile_e;

extern int
soc_th_asf_init_start(int unit);

extern int
soc_th_asf_init_done(int unit);

extern int
soc_th_port_asf_init(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode);

extern int
soc_th_asf_deinit(int unit);

extern int
soc_th_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_th_asf_mode_e    mode,
    uint8                extra_cells);

extern int
soc_th_port_asf_mode_get(
    int                            unit,
    soc_port_t                     port,
    int                            port_speed,
    OUT soc_th_asf_mode_e* const   mode);

extern int
soc_th_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode);

extern int
soc_th_port_asf_params_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode,
    uint8               bubble_mop_disable,
    uint8               ca_thresh,
    int                 speed_encoding);

extern int
soc_th_port_asf_speed_to_mmu_cell_credit(
    int        unit,
    soc_port_t port,
    int        port_speed,
    OUT uint32 *mmu_cell_credit);
extern int
soc_th_port_asf_mmu_cell_credit_to_speed(
    int        unit,
    soc_port_t port,
    uint8      mmu_cell_credit,
    OUT int    *port_speed);

extern pbmp_t *
soc_th_port_asf_pause_bpmp_get(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_th_asf_wb_memsz_get(
    int               unit,
    OUT uint32* const wb_mem_sz,
    uint16            scache_ver);

extern int
soc_th_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data);

extern int
soc_th_asf_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver);
#endif

extern int
soc_th_asf_pbmp_get(int unit);

extern int
soc_th_port_asf_show(
    int          unit,
    soc_port_t   port,
    int          port_speed);

extern int
soc_th_asf_config_dump(int unit);

extern int
soc_th_port_asf_config_dump(
	int          unit,
	soc_port_t   port,
	int          port_speed);

extern int
soc_th_port_asf_detach(
    int          unit,
    soc_port_t   port);

/*
  ==========================
   TH Switch Latency Bypass
  ==========================
*/

/* resource type */
typedef enum {
  _SOC_TH_REG = 0,
  _SOC_TH_MEM = 1
} soc_th_resource_e;

/* protocol table configs */
typedef struct _soc_th_latency_table_cfg_s {
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
} soc_th_latency_table_cfg_t;


extern int
soc_th_latency_init(int unit);

extern int
soc_th_latency_deinit(int unit);

extern int
soc_th_latency_get(
    int unit,
    OUT int* const latency);

extern int
soc_th_latency_bypassed(
    int             unit,
    soc_feature_t   feature);

extern int
soc_th_latency_mem_avail(
    int         unit,
    soc_mem_t   mem,
    int         latency);

extern int
soc_th_latency_reg_avail(
    int         unit,
    soc_reg_t   reg,
    int         latency);

extern int
soc_th_latency_mem_filter(int unit);

extern int
soc_th_latency_reg_filter(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_th_latency_wb_memsz_get(
    int               unit,
    OUT uint32* const wb_mem_sz,
    uint16            scache_ver);

extern int
soc_th_latency_wb_sync(
    int                 unit,
    IN_OUT uint8* const wb_data);

extern int
soc_th_latency_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver);

extern int
soc_th_latency_wb_reinit(int unit, int level);
#endif

extern int
soc_th_latency_show(int unit);

extern int
soc_th_latency_diag(int unit);


extern uint32 _soc_th_temp_mon_mask[SOC_MAX_NUM_DEVICES];

#ifdef INCLUDE_AVS
extern int soc_th_avs_init(int unit);
extern soc_error_t soc_th_avs_vcore_get(int unit, uint32 *voltage);
extern soc_error_t soc_th_avs_vcore_set(int unit, uint32 voltage);
extern soc_error_t soc_th_avs_vcore_init(int unit);
extern soc_error_t soc_th_avs_vcore_deinit(int unit);
#endif /* INCLUDE_AVS */

extern void _soc_tomahawk_speed_to_ovs_class_mapping(int unit, int speed, int *ovs_class);
extern void _soc_tomahawk_speed_to_slot_mapping(int unit, int speed, int *slot);
extern int _soc_tomahawk_tdm_idb_opportunistic_set(int unit, int enable);
extern int _soc_tomahawk_tdm_idb_hsp_set(int unit, int all_pipes);
extern int _soc_tomahawk_tdm_mmu_opportunistic_set(int unit, int enable);
extern int _soc_tomahawk_tdm_mmu_hsp_set(int unit, int all_pipes);

#ifdef PORTMOD_SUPPORT
extern int soc_th_portctrl_pm_portmod_init(int unit);
extern int soc_th_portctrl_pm_portmod_deinit(int unit);
extern int soc_th_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
#endif

extern int soc_th_field_mem_mode_get(int unit, soc_mem_t mem, int *mode);
extern int soc_mem_field_nw_tcam_prio_order_index_get(int unit, soc_mem_t mem,
                                                      int *tcam_idx);
extern int soc_th_ifp_mem_write(int unit, soc_mem_t mem, int copyno, int index,
                                void *entry_data);
extern int soc_th_ifp_mem_read(int unit, soc_mem_t mem, int copyno, int index,
                               void *entry_data);

extern int soc_th_iss_log_to_phy_bank_map_shadow_get(int unit, uint32 *shadow_val);
extern int soc_th_iss_log_to_phy_bank_map_shadow_set(int unit, uint32 shadow_val);

extern int soc_th_ser_single_bit_error_enable_set(int unit, int enable);

extern int soc_th_ser_single_bit_error_enable_get(int unit, int *enable);

extern int soc_th_ser_single_bit_report_for_bus_buf_set(int unit, int enable);

extern int soc_asf_register_linkscan_cb(int unit);

extern int soc_th_reset_serdes(int unit, int port, int reset_delay_us); 

extern void _soc_th_lpm_view_set(int unit, int index, soc_mem_t view, int pair_lpm);
extern soc_mem_t _soc_th_lpm_view_get(int unit, int index, int pair_lpm);
extern int soc_th_cosq_threshold_dynamic_change_check(
    int unit,
    int new_threshold,
    soc_reg_t ctr_reg,
    soc_ctr_control_info_t ctrl_info,
    int ar_idx,
    uint8 sync_mode,
    uint8 *allowed);
#endif /* !_SOC_TOMAHAWK_H_ */
