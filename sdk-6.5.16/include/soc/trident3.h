/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        trident3.h
 */

#ifndef _SOC_TRIDENT3_H_
#define _SOC_TRIDENT3_H_

#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <soc/trident3_tdm.h>

/* **************** TDM-related Macors and structs adopted from Tomahawk ---start************ */
/* Device port */
/* 64 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TD3_TDM_GDEV_PORTS_PER_PIPE        64
#define _TD3_TDM_DEV_PORTS_PER_PIPE         (_TD3_TDM_GDEV_PORTS_PER_PIPE + 2)
#define _TD3_TDM_DEV_PORTS_PER_DEV          \
    (_TD3_TDM_DEV_PORTS_PER_PIPE * TRIDENT3_TDM_PIPES_PER_DEV)
/* MMU port */
#define _TD3_TDM_MMU_PORTS_OFFSET_PER_PIPE  64
#define _TD3_TDM_MMU_PORTS_PER_DEV          (64 * 2)
/* ------------------------------------------------- */
#define _TD3_TDM_LENGTH 512	
#define _TD3_PKT_SCH_LENGTH 160
#define _TD3_TDM_LR_SPEED_CLASS_MAX             4
#define _TD3_TDM_OS_SPEED_CLASS_MAX             5

/* **************** TDM-related Macors and structs adopted from Tomahawk ---end************ */

extern int soc_td3_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
                                    int thdo_db_shd, int thdo_qe_shd,
                                    int post_update);
#define SOC_TD3_MMU_BASE_TYPE_IPORT 0
#define SOC_TD3_MMU_BASE_TYPE_EPORT 1
#define SOC_TD3_MMU_BASE_TYPE_IPIPE 2
#define SOC_TD3_MMU_BASE_TYPE_EPIPE 3
#define SOC_TD3_MMU_BASE_TYPE_CHIP 4
#define SOC_TD3_MMU_BASE_TYPE_XPE 5
#define SOC_TD3_MMU_BASE_TYPE_SLICE 6
#define SOC_TD3_MMU_BASE_TYPE_LAYER 7

#define SOC_TD3_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem) \
        (mem == SRC_COMPRESSIONm || mem == DST_COMPRESSIONm || \
         mem == FP_UDF_TCAMm || mem == VFP_TCAMm || \
         mem == IFP_LOGICAL_TABLE_SELECTm || \
         mem == EXACT_MATCH_LOGICAL_TABLE_SELECTm || \
         mem == EFP_TCAMm || mem == IFP_TCAMm || mem == IFP_TCAM_WIDEm) \

#define CCLK_FREQ_1700MHZ    1700
#define CCLK_FREQ_1525MHZ    1525
#define CCLK_FREQ_1012MHZ    1012

#define SOC_TD3_FP_SLICE_ENTRY_NARROW_VIEW 1536
#define SOC_TD3_FP_SLICE_ENTRY_WIDE_VIEW 768

extern int soc_td3_rx_etype_niv[];
extern int soc_td3_rx_etype_pe[];
extern uint32 soc_td3_ip_pipe_fifo_bmask[SOC_MAX_NUM_DEVICES][2];
extern uint32 soc_td3_temp_mon_mask[SOC_MAX_NUM_DEVICES];

extern soc_functions_t soc_trident3_drv_funs;
extern int soc_trident3_tcam_mode_set(int unit, int stage, int mode);
extern int soc_trident3_tcam_mode_get(int unit, soc_mem_t mem, int *mode);
extern int soc_td3_ifp_slice_mode_check(int unit, soc_mem_t mem, int slice, int *slice_skip);

extern void soc_td3_hgoe_feature_update(int unit);

extern int soc_trident3_check_cache_skip(int unit, soc_mem_t mem);
extern void soc_trident3_pipe_map_get(int unit, uint32 *pipe_map);
extern int soc_trident3_pipe_select(int unit, int egress, int pipe);
extern void soc_trident3_vlan_xlate_mem_remap(soc_mem_t *mem, int *index);
extern int soc_trident3_l3_memwr_parity_check(int unit, soc_mem_t mem,
                                              int disable);
extern int soc_td3_parity_bit_enable(int unit, soc_reg_t enreg,
                                     soc_mem_t enmem, soc_field_t enfld,
                                     int enfld_position, int enable);
extern int soc_trident3_check_cache_skip(int unit, soc_mem_t mem);
extern int soc_trident3_ser_enable_parity_table_all(int unit, int enable);
extern int
soc_trident3_ser_enable_parity_table(int unit, int type, void *info, int enable);
extern int soc_trident3_tcam_ser_init(int unit);
extern int soc_trident3_ser_enable_all(int unit, int enable);
extern void soc_trident3_ser_fail(int unit);
extern void soc_trident3_ser_error(void *unit_vp, void *d1, void *d2,
                                   void *d3, void *d4);
extern int soc_trident3_mem_ser_control(int unit, soc_mem_t mem,
                                        int copyno, int enable);
extern int soc_trident3_ifp_slice_mode_hw_get(int unit, int pipe, int slice_num,
                                         int *slice_type, int *slice_enabled);
extern int soc_trident3_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                              int enable, int *orig_enable);
extern int soc_trident3_reg_cpu_write_control(int unit, int enable);
extern int soc_trident3_ser_mem_clear(int unit, soc_mem_t mem);
extern void soc_trident3_ser_register(int unit);
extern int soc_trident3_mem_config(int unit);
extern int soc_trident3_num_cosq_init(int unit);
extern int soc_trident3_num_cosq_init_port(int unit, int port);
extern int soc_trident3_port_map_validity_check(int unit, int port, int phy_port);
extern int soc_trident3_port_config_init(int unit, uint16 dev_id);
extern int soc_trident3_chip_reset(int unit);
extern int soc_trident3_port_reset(int unit);
extern int soc_trident3_xpe_base_index_check(int unit, int base_type, int xpe,
                                             int base_index, char *msg);
extern int soc_trident3_xpe_reg32_set(int unit, soc_reg_t reg, int xpe,
                                      int base_index, int index, uint32 data);
extern int soc_trident3_xpe_reg32_get(int unit, soc_reg_t reg, int xpe,
                                      int base_index, int index, uint32 *data);
extern int soc_trident3_xpe_reg_set(int unit, soc_reg_t reg, int xpe,
                                    int base_index, int index, uint64 data);
extern int soc_trident3_xpe_reg_get(int unit, soc_reg_t reg, int xpe,
                                    int base_index, int index, uint64 *data);
#if 0
extern int soc_trident3_xpe_mem_write(int unit, soc_mem_t mem, int xpe,
                                      int base_index, int copyno,
                                      int offset_in_section, void *entry_data);
extern int soc_trident3_xpe_mem_read(int unit, soc_mem_t mem, int xpe,
                                     int base_index, int copyno,
                                     int offset_in_section, void *entry_data);
#endif
extern int soc_trident3_mem_xpe_pipe_check(int unit, soc_mem_t mem,
                                           int xpe, int pipe);
extern int soc_trident3_mem_basetype_get(int unit, soc_mem_t mem);
extern int soc_trident3_mem_is_xpe(int unit, soc_mem_t mem);
extern int soc_trident3_sc_base_index_check(int unit, int base_type, int sc,
                                            int base_index, char *msg);
extern int soc_trident3_sc_reg32_set(int unit, soc_reg_t reg, int sc,
                                     int base_index, int index, uint32 data);
extern int soc_trident3_sc_reg32_get(int unit, soc_reg_t reg, int sc,
                                     int base_index, int index, uint32 *data);
extern int soc_trident3_sc_reg_set(int unit, soc_reg_t reg, int sc,
                                   int base_index, int index, uint64 data);
extern int soc_trident3_sc_reg_get(int unit, soc_reg_t reg, int sc,
                                   int base_index, int index, uint64 *data);
extern int soc_trident3_sed_reg32_set(int unit, soc_reg_t reg, int sed,
                                     int base_index, int index, uint32 data);
extern int soc_trident3_sed_reg32_get(int unit, soc_reg_t reg, int sed,
                                     int base_index, int index, uint32 *data);
extern int soc_trident3_sed_reg_set(int unit, soc_reg_t reg, int sed,
                                   int base_index, int index, uint64 data);
extern int soc_trident3_sed_reg_get(int unit, soc_reg_t reg, int sed,
                                   int base_index, int index, uint64 *data);
#if 0
extern int soc_trident3_sc_mem_write(int unit, soc_mem_t mem, int sc,
                                     int base_index, int copyno,
                                     int offset_in_section, void *entry_data);
extern int soc_trident3_sc_mem_read(int unit, soc_mem_t mem, int sc,
                                     int base_index, int copyno,
                                     int offset_in_section, void *entry_data);
#endif
extern uint32 _soc_td3_piped_mem_index(int unit, soc_port_t port,
                                      soc_mem_t mem, int arr_off);
extern soc_mem_t
_soc_trident3_pmem(int unit, int port, soc_mem_t mema, soc_mem_t memb);

extern int soc_td3_l2_bulk_age_start(int unit, int interval);
extern int soc_td3_l2_bulk_age_stop(int unit);
extern int soc_trident3_port_obm_info_get(int unit, soc_port_t port,
                                          int *obm_id, int *lane);

extern int soc_trident3_show_ring_osc(int unit);

extern soc_error_t
soc_td3_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp);

/* Counter */
extern int soc_counter_trident3_status_enable(int unit, int enable);
extern int soc_counter_trident3_non_dma_init(int unit, int nports,
                                             int non_dma_start_index,
                                             int *non_dma_entries);
extern int soc_counter_trident3_get_info(int unit, soc_port_t port,
                                         soc_reg_t id, int *base_index,
                                         int *num_entries);
extern int soc_counter_trident3_eviction_init(int unit);
extern int soc_counter_trident3_eviction_enable(int unit, int enable);
extern void soc_counter_trident3_eviction_flags_update(int unit,
                                                       uint32 flags,
                                                       int enable);
extern int soc_counter_trident3_generic_get_info(int unit,  soc_ctr_control_info_t ctrl_info,
                                            soc_reg_t id, int *base_index, int *num_entries);
extern int
soc_counter_trident3_get_child_dma_by_idx(int unit,
        soc_counter_non_dma_t *parent, int idx, soc_counter_non_dma_t **child);
extern int
soc_counter_trident3_get_child_dma(int unit, soc_reg_t id,
                                   soc_ctr_control_info_t ctrl_info,
                                   soc_counter_non_dma_t **child_dma);
extern int
soc_counter_trident3_dma_flags_update(int unit, soc_counter_non_dma_t *ctr_dma,
                                      uint32 flags, int val);
extern int
soc_counter_td3_extra_sflow_ctrs_init(int unit,
                               soc_counter_non_dma_t *non_dma_parent,
                               soc_counter_non_dma_t *non_dma_extra,
                               uint32 extra_ctr_ct,
                               int *total_entries);
extern int
soc_counter_trident3_extra_ctrs_init(int unit, soc_reg_t id,
                                     soc_counter_non_dma_t *non_dma_parent,
                                     soc_counter_non_dma_t *non_dma_extra,
                                     uint32 extra_ctr_ct,
                                     int *total_entries);

extern int soc_td3_flow_based_udf_enable(int unit);

typedef struct _soc_trident3_tdm_pblk_info_s {
	int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
}_soc_trident3_tdm_pblk_info_t;

typedef struct _soc_trident3_tdm_pipe_s {
    int idb_tdm[_TD3_TDM_LENGTH];
    int mmu_tdm[_TD3_TDM_LENGTH];
    int ovs_tdm[TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE][TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE][TRIDENT3_TDM_OVS_GROUP_LENGTH];
    int pkt_shaper_tdm[TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE][_TD3_PKT_SCH_LENGTH];
} _soc_trident3_tdm_pipe_t;

typedef struct _soc_trident3_tdm_temp_s {
    _soc_trident3_tdm_pipe_t tdm_pipe[TRIDENT3_TDM_PIPES_PER_DEV];
    _soc_trident3_tdm_pblk_info_t pblk_info[TRIDENT3_TDM_PHY_PORTS_PER_DEV];
    int port_ratio[TRIDENT3_TDM_PBLKS_PER_DEV];
    int ovs_ratio_x1000[TRIDENT3_TDM_PIPES_PER_DEV][TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE];

} _soc_trident3_tdm_temp_t;

typedef struct soc_td3_port_lanes_s {
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
} soc_td3_port_lanes_t;

extern void soc_trident3_port_ratio_get(int unit, int clport, int *mode);

extern int soc_trident3_port_lanes_validate(int unit,
                                            soc_td3_port_lanes_t *lanes_ctrl);
extern int soc_trident3_port_lanes_set(int unit,
                                       soc_td3_port_lanes_t *lanes_ctrl);
extern int soc_trident3_port_lanes_get(int unit, soc_port_t port_base,
                                       int *cur_lanes);
extern int soc_trident3_port_speed_update(int unit, soc_port_t port, int speed);

typedef int (*_soc_td3_bst_hw_cb)(int unit);
extern int soc_td3_set_bst_callback(int unit, _soc_td3_bst_hw_cb cb);
extern int _soc_td3_process_mmu_bst(int unit, int xpe, soc_field_t field);


#define SOC_TD3_MMU_PORT(u, p)  _soc_td3_mmu_port((u),(p))
#define SOC_TD3_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    _soc_td3_piped_mem_index((u),(p),(m),(off))
#define SOC_TD3_PMEM(u, p, m1, m2) _soc_trident3_pmem((u), (p), (m1), (m2))

typedef enum {
    SOC_TD3_TS_PLL,
    SOC_TD3_BS_PLL0,
    SOC_TD3_BS_PLL1,
    SOC_TD3_PLL_INVALID
} soc_td3_plls_e;

enum soc_td3_port_ratio_e {
    SOC_TD3_PORT_RATIO_SINGLE,
    SOC_TD3_PORT_RATIO_DUAL_1_1,
    SOC_TD3_PORT_RATIO_DUAL_2_1,
    SOC_TD3_PORT_RATIO_DUAL_1_2,
    SOC_TD3_PORT_RATIO_TRI_023_2_1_1,
    SOC_TD3_PORT_RATIO_TRI_023_4_1_1,
    SOC_TD3_PORT_RATIO_TRI_012_1_1_2,
    SOC_TD3_PORT_RATIO_TRI_012_1_1_4,
    SOC_TD3_PORT_RATIO_QUAD,
    SOC_TD3_PORT_RATIO_COUNT
};

enum soc_td3_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_TD3_PORT_MODE_QUAD = 0,
    SOC_TD3_PORT_MODE_TRI_012 = 1,
    SOC_TD3_PORT_MODE_TRI_023 = 2,
    SOC_TD3_PORT_MODE_DUAL = 3,
    SOC_TD3_PORT_MODE_SINGLE = 4
};

extern int soc_trident3_hash_init(int unit);

enum td3_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_L2_HASH_KEY_TYPE_BRIDGE = 0,
    TD3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    TD3_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    TD3_L2_HASH_KEY_TYPE_VFI = 3,
    TD3_L2_HASH_KEY_TYPE_VIF = 4,
    TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS = 5,
    TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG = 6,
    TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT = 7,
    TD3_L2_HASH_KEY_TYPE_BFD = 8,
    TD3_L2_HASH_KEY_TYPE_PE_VID = 9,
    TD3_L2_HASH_KEY_TYPE_FCOE_ZONE = 10,
    TD3_L2_HASH_KEY_TYPE_COUNT
};

enum td3_l3_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_L3_HASH_KEY_TYPE_V4UC = 0,
    TD3_L3_HASH_KEY_TYPE_V4UC_EXT = 0,
    TD3_L3_HASH_KEY_TYPE_V6UC = 2,
    TD3_L3_HASH_KEY_TYPE_V6UC_EXT = 2,
    TD3_L3_HASH_KEY_TYPE_V4MC = 4,
    TD3_L3_HASH_KEY_TYPE_V6MC = 5,
    TD3_L3_HASH_KEY_TYPE_TRILL = 8,
    TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN = 12,
    TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT = 12,
    TD3_L3_HASH_KEY_TYPE_FCOE_HOST = 14,
    TD3_L3_HASH_KEY_TYPE_FCOE_HOST_EXT = 14,
    TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP = 16,
    TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP_EXT = 16,
    TD3_L3_HASH_KEY_TYPE_DST_NAT = 18,
    TD3_L3_HASH_KEY_TYPE_DST_NAPT = 19,
    TD3_L3_HASH_KEY_TYPE_L2_IPV4_MULTICAST = 20,
    TD3_L3_HASH_KEY_TYPE_L2VP_IPV4_MULTICAST = 21,
    TD3_L3_HASH_KEY_TYPE_L2_IPV6_MULTICAST = 22,
    TD3_L3_HASH_KEY_TYPE_L2VP_IPV6_MULTICAST = 23,
    TD3_L3_HASH_KEY_TYPE_COUNT
};

enum td3_fpem_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_FPEM_HASH_KEY_TYPE_128B = 0,
    TD3_FPEM_HASH_KEY_TYPE_160B = 1,
    TD3_FPEM_HASH_KEY_TYPE_320B = 2,
    TD3_FPEM_HASH_KEY_TYPE_COUNT
};

enum td3_mpls_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_MPLS_HASH_KEY_TYPE_MPLS = 0,
    TD3_MPLS_HASH_KEY_TYPE_MIM_NVP = 1,
    TD3_MPLS_HASH_KEY_TYPE_MIM_ISID = 2,
    TD3_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP = 3,
    TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP = 4,
    TD3_MPLS_HASH_KEY_TYPE_TRILL = 5,
    TD3_MPLS_HASH_KEY_TYPE_L2GRE_SIP = 6,
    TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID = 7,
    TD3_MPLS_HASH_KEY_TYPE_VXLAN_SIP = 8,
    TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID = 9,
    TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP = 10,
    TD3_MPLS_HASH_KEY_TYPE_VXLAN_FLEX_IPV4_DIP = 12,
    TD3_MPLS_HASH_KEY_TYPE_COUNT
};

enum td3_vlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    TD3_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    TD3_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    TD3_VLXLT_HASH_KEY_TYPE_OVID = 4,
    TD3_VLXLT_HASH_KEY_TYPE_IVID = 5,
    TD3_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    TD3_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    TD3_VLXLT_HASH_KEY_TYPE_VIF = 8,
    TD3_VLXLT_HASH_KEY_TYPE_VIF_VLAN = 9,
    TD3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN = 10,
    TD3_VLXLT_HASH_KEY_TYPE_VIF_OTAG = 11,
    TD3_VLXLT_HASH_KEY_TYPE_VIF_ITAG = 12,
    TD3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP = 13,
    TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT = 14,
    TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN = 15,
    TD3_VLXLT_HASH_KEY_TYPE_IVID_VSAN = 16,
    TD3_VLXLT_HASH_KEY_TYPE_OVID_VSAN = 17,
    TD3_VLXLT_HASH_KEY_TYPE_VXLAN_DIP = 18,
    TD3_VLXLT_HASH_KEY_TYPE_VXLAN_FLEX_VNID = 19,
    TD3_VLXLT_HASH_KEY_TYPE_VXLAN_FLEX_SIP_VNID = 28,
    TD3_VLXLT_HASH_KEY_TYPE_COUNT
};

enum td3_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE = 0,
    TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP = 1,
    TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE = 2,
    TD3_EVLXLT_HASH_KEY_TYPE_ISID_XLATE = 3,
    TD3_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE = 4,
    TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI = 5,
    TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP = 6,
    TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP = 7,
    TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI = 8,
    TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP = 9,
    TD3_ELVXLT_HASH_KEY_TYPE_VFI_DVP_GROUP = 10,
    TD3_ELVXLT_HASH_KEY_TYPE_VRF_MAPPING = 11,
    TD3_ELVXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI = 12,
    TD3_VELXLT_HASH_KEY_TYPE_COUNT
};

enum td3_l3_hash_data_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD3_L3_HASH_DATA_TYPE_V4UC = 0,
    TD3_L3_HASH_DATA_TYPE_V4UC_EXT = 1,
    TD3_L3_HASH_DATA_TYPE_V6UC = 2,
    TD3_L3_HASH_DATA_TYPE_V6UC_EXT = 3,
    TD3_L3_HASH_DATA_TYPE_V4MC = 4,
    TD3_L3_HASH_DATA_TYPE_V6MC = 5,
    TD3_L3_HASH_DATA_TYPE_TRILL = 8,
    TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN = 12,
    TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN_EXT = 13,
    TD3_L3_HASH_DATA_TYPE_FCOE_HOST = 14,
    TD3_L3_HASH_DATA_TYPE_FCOE_HOST_EXT = 15,
    TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP = 16,
    TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP_EXT = 17,
    TD3_L3_HASH_DATA_TYPE_DST_NAT = 18,
    TD3_L3_HASH_DATA_TYPE_DST_NAPT = 19,
    TD3_L3_HASH_DATA_TYPE_V4L2MC = 20,
    TD3_L3_HASH_DATA_TYPE_V4L2VPMC  = 21,
    TD3_L3_HASH_DATA_TYPE_V6L2MC = 22,
    TD3_L3_HASH_KEY_TYPE_V6L2VPMC = 23,
    TD3_L3_HASH_DATA_TYPE_COUNT
};

typedef int (*_soc_td3_tcb_hw_cb)(int unit);
extern int soc_td3_set_tcb_callback(int unit, _soc_td3_tcb_hw_cb cb);
extern int _soc_td3_process_mmu_tcb(int unit, int xpe, soc_field_t field);

/* Maximum Flex Key Width for each hash table in TD3 */

#define SOC_TD3_L2_MAX_KEY_WIDTH                        100
#define SOC_TD3_L3_MAX_KEY_WIDTH                        415
#define SOC_TD3_EXACT_MATCH_MAX_KEY_WIDTH               322
#define SOC_TD3_MPLS_MAX_KEY_WIDTH                      199
#define SOC_TD3_VLAN_XLATE_MAX_KEY_WIDTH                199
#define SOC_TD3_EGR_VLAN_XLATE_MAX_KEY_WIDTH            199
#define SOC_TD3_ING_DNAT_ADDRESS_TYPE_MAX_KEY_WIDTH      48
#define SOC_TD3_SUBPORT_ID_SGPP_MAP_MAX_KEY_WIDTH        24
#define SOC_TD3_ING_VP_VLAN_MEMBERSHIP_MAX_KEY_WIDTH     37
#define SOC_TD3_EGR_VP_VLAN_MEMBERSHIP_MAX_KEY_WIDTH     37

extern int soc_trident3_uft_uat_config(int unit);

extern int      soc_td3_shared_hash_mem_bucket_read(int unit, int ent,
                                        int key_type, int *width, 
                                        soc_mem_t base_mem, soc_mem_t *mem,
                                        void *bkt_entry, void *entry);
extern int      soc_td3_hash_mem_status_get(int unit, soc_mem_t mem,
                                            void* entry, uint32 *valid);
extern int      soc_trident3_iuat_sizing(int unit, int iuat_entries,
                                         int *iuat_16k, int *iuat_8k);
extern int      soc_trident3_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_trident3_hash_bank_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_td3_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem,
                                                 uint32 *bitmap);
extern int      soc_td3_hash_bank_number_get(int unit, soc_mem_t mem,
                                             int seq_num, int *bank_num);
extern int      soc_td3_hash_seq_number_get(int unit, soc_mem_t mem,
                                            int bank_num, int *seq_num);
extern int      soc_td3_hash_bank_info_get(int unit, soc_mem_t mem,
                                           int bank,
                                           int *entries_per_bank,
                                           int *entries_per_row,
                                           int *entries_per_bucket,
                                           int *bank_base,
                                           int *bucket_offset);
extern int      soc_td3_hash_bucket_get(int unit, int mem, int bank,
                                       uint32 *entry, uint32 *bucket);
extern int      soc_td3_hash_index_get(int unit, int mem, int bank, int bucket);
extern int      soc_td3_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                       int *hash_offset, int *use_lsb);
extern int      soc_td3_hash_offset_set(int unit, soc_mem_t mem, int bank,
                                       int hash_offset, int use_lsb);
extern uint32   soc_td3_l2x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key1, uint8 *key2);
extern int      soc_td3_l2x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_td3_l2x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_td3_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_td3_l3x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key1, uint8 *key2);
extern int      soc_td3_l3x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_td3_l3x_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry);
extern uint32   soc_td3_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_td3_exact_match_hash(int unit, int bank, int hash_offset,
                                         int use_lsb, int key_nbits,
                                         void *base_entry, uint8 *key1,
                                         uint8 *key2);
extern int      soc_td3_exact_match_base_entry_to_key(int unit, int bank, uint32 *entry,
                                                      uint8 *key);
extern uint32   soc_td3_exact_match_entry_hash(int unit, int bank,
                                              int hash_offset, int use_lsb,
                                              uint32 *entry);
extern uint32   soc_td3_exact_match_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32   soc_td3_mpls_hash(int unit, soc_mem_t mem, int bank, int hash_offset, int use_lsb,
                                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2);
extern int      soc_td3_mpls_base_entry_to_key(int unit, soc_mem_t mem, int bank, void *entry,
                                              uint8 *key);
extern uint32   soc_td3_mpls_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                        int use_lsb, uint32 *entry);
extern uint32   soc_td3_mpls_bank_entry_hash(int unit, soc_mem_t mem, int bank, uint32 *entry);
extern uint32   soc_td3_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                        int use_lsb, int key_nbits, void *base_entry,
                                        uint8 *key1, uint8 *key2);
extern int      soc_td3_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                                     void *entry, uint8 *key);
extern uint32   soc_td3_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                              int use_lsb, uint32 *entry);
extern uint32   soc_td3_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                                  uint32 *entry);
extern uint32   soc_td3_egr_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                            int use_lsb, int key_nbits, void *base_entry,
                                            uint8 *key1, uint8 *key2);
extern int      soc_td3_egr_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem,
                                                         int bank, void *entry, uint8 *key);
extern uint32   soc_td3_egr_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                                                  int use_lsb, uint32 *entry);
extern uint32   soc_td3_egr_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem,
                                                       int bank, uint32 *entry);
extern uint32   soc_td3_ing_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                                int use_lsb, int key_nbits, void *base_entry,
                                                uint8 *key1, uint8 *key2);
extern int      soc_td3_ing_vp_vlan_member_base_entry_to_key(int unit,
                                                            int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_td3_ing_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                                      int use_lsb, uint32 *entry);
extern uint32   soc_td3_ing_vp_vlan_member_bank_entry_hash(int unit, int bank,
                                                           uint32 *entry);
extern uint32   soc_td3_egr_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2);
extern int      soc_td3_egr_vp_vlan_member_base_entry_to_key(int unit,
                                                            int bank,
                                                            void *entry,
                                                            uint8 *key);
extern uint32   soc_td3_egr_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                                      int use_lsb, uint32 *entry);
extern uint32   soc_td3_egr_vp_vlan_member_bank_entry_hash(int unit, int bank,
                                                           uint32 *entry);
extern uint32   soc_td3_ing_dnat_address_type_hash(int unit, int bank, int hash_offset,
                                                   int use_lsb, int key_nbits,
                                                   void *base_entry, uint8 *key1, uint8 *key2);
extern int      soc_td3_ing_dnat_address_type_base_entry_to_key(int unit,
                                                               int bank,
                                                               void *entry,
                                                               uint8 *key);
extern uint32   soc_td3_ing_dnat_address_type_entry_hash(int unit, int bank, int hash_offset,
                                                         int use_lsb, uint32 *entry);
extern uint32   soc_td3_ing_dnat_address_type_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_td3_subport_id_to_sgpp_map_hash(int unit, int bank, int hash_offset,
                                                    int use_lsb, int key_nbits,
                                                    void *base_entry, uint8 *key1, uint8 *key2);
extern int      soc_td3_subport_id_to_sgpp_map_base_entry_to_key(int unit, int bank,
                                                                 void *entry, uint8 *key);
extern uint32   soc_td3_subport_id_to_sgpp_map_entry_hash(int unit, int bank, int hash_offset,
                                                          int use_lsb, uint32 *entry);
extern uint32   soc_td3_subport_id_to_sgpp_map_bank_entry_hash(int unit, int bank,
                                                               uint32 *entry);

extern int soc_td3_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_td3_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern soc_error_t
soc_trident3_edb_buf_reset(int unit, soc_port_t port, int reset);

extern soc_error_t
soc_trident3_idb_buf_reset(int unit, soc_port_t port, int reset);

extern int soc_trident3_bond_info_init(int unit);
extern int soc_trident3_init_idb_memory(int unit);

/* ALPM support */
extern int soc_trident3_get_alpm_banks(int unit);
extern int soc_trident3_set_alpm_banks(int unit, int banks);
extern int soc_trident3_alpm_mode_get(int unit);
extern void _soc_trident3_alpm_bkt_view_set(int unit, int index, soc_mem_t view);
extern soc_mem_t _soc_trident3_alpm_bkt_view_get(int unit, int index);

extern void _soc_trident3_alpm2_bnk_fmt_set(int unit, int index, int8 fmt);
extern int8 _soc_trident3_alpm2_bnk_fmt_get(int unit, int index);
extern int soc_trident3_init_alpm2_memory(int unit);

extern int _soc_trident3_init_hash_control_reset(int unit);


/* Special port number used by H/W */
#define _SOC_TD3_TDM_OVERSUB_TOKEN    0x22
#define _SOC_TD3_TDM_NULL_TOKEN       0x23
#define _SOC_TD3_TDM_IDL1_TOKEN       0x24
#define _SOC_TD3_TDM_IDL2_TOKEN       0x25
#define _SOC_TD3_TDM_UNUSED_TOKEN     0x3f

typedef enum {
    SOC_TD3_SCHED_MODE_UNKNOWN = 0,
    SOC_TD3_SCHED_MODE_STRICT,
    SOC_TD3_SCHED_MODE_WRR,
    SOC_TD3_SCHED_MODE_WERR
} soc_td3_sched_mode_e;

typedef enum {
    SOC_TD3_NODE_LVL_ROOT = 0,
    SOC_TD3_NODE_LVL_L0,
    SOC_TD3_NODE_LVL_L1,
    SOC_TD3_NODE_LVL_L2,
    SOC_TD3_NODE_LVL_MAX
} soc_td3_node_lvl_e;

typedef enum {
    _SOC_TD3_INDEX_STYLE_BUCKET,
    _SOC_TD3_INDEX_STYLE_BUCKET_MODE,
    _SOC_TD3_INDEX_STYLE_WRED,
    _SOC_TD3_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_TD3_INDEX_STYLE_SCHEDULER,
    _SOC_TD3_INDEX_STYLE_PERQ_XMT,
    _SOC_TD3_INDEX_STYLE_UCAST_DROP,
    _SOC_TD3_INDEX_STYLE_UCAST_QUEUE,
    _SOC_TD3_INDEX_STYLE_MCAST_QUEUE,
    _SOC_TD3_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_TD3_INDEX_STYLE_EGR_POOL,
    _SOC_TD3_INDEX_STYLE_COUNT
} soc_td3_index_style_t;

typedef enum soc_td3_drop_limit_alpha_value_e {
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_TD3_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_td3_drop_limit_alpha_value_t;

#define _TD3_PORTS_PER_PBLK             4
#define _TD3_PBLKS_PER_PIPE             16
#define _TD3_PIPES_PER_DEV              2
#define _TD3_XPES_PER_DEV               1

#define _TD3_PBLKS_PER_DEV(unit)        (_TD3_PBLKS_PER_PIPE * NUM_PIPE(unit))

#define _TD3_PORTS_PER_PIPE             (_TD3_PORTS_PER_PBLK * _TD3_PBLKS_PER_PIPE)
#define _TD3_PORTS_PER_DEV(unit)        (_TD3_PORTS_PER_PIPE * NUM_PIPE(unit))

#define _TD3_SECTION_SIZE                32768

#define _TD3_MMU_MAX_PACKET_BYTES        9416  /* bytes */
#define _TD3_MMU_PACKET_HEADER_BYTES     64    /* bytes */
#define _TD3_MMU_JUMBO_FRAME_BYTES       9216  /* bytes */
#define _TD3_MMU_DEFAULT_MTU_BYTES       1536  /* bytes */

#define _TD3_MMU_PHYSICAL_CELLS_PER_XPE  131072 /* Total Physical cells per XPE */
#define _TD3_MMU_TOTAL_CELLS_PER_XPE     129024 /* 31.5MB/XPE = 129024 cells/XPE */
#define _TD3_MMU_RSVD_CELLS_CFAP_PER_XPE 556    /* Reserved CFAP cells per XPE */
#define _TD3_MMU_CFAP_BANK_FULL_LIMIT    2047   /* CFAP Bank Full limit */

#define _TD3_MMU_TOTAL_CELLS             (_TD3_MMU_TOTAL_CELLS_PER_XPE * _TD3_XPES_PER_DEV)
#define _TD3_MMU_PHYSICAL_CELLS          (_TD3_MMU_PHYSICAL_CELLS_PER_XPE * _TD3_XPES_PER_DEV)

#define _TD3_MMU_BYTES_PER_CELL          256   /* bytes (2048 bits) */
#define _TD3_MMU_NUM_PG                  8
#define _TD3_MMU_NUM_POOL                4
#define _TD3_MMU_NUM_RQE_QUEUES          11
#define _TD3_MMU_NUM_INT_PRI             16
#define _TD3_MMU_PORTS_PER_PIPE          66

#define SOC_TD3_MMU_CFG_QGROUP_MAX       66

#define SOC_TD3_MMU_PORT_STRIDE          0x7f  /* MMU Port number space per pipe */
#define SOC_TD3_MMU_PORT_PER_PIPE        128

#define SOC_TD3_NUM_CPU_QUEUES           48  /* CPU port has 48 MC Queues */
#define SOC_TD3_NUM_UC_QUEUES_PER_PIPE   650 /* (64 General Ports + 1 Management Port) * 10 */
#define SOC_TD3_NUM_MC_QUEUES_PER_PIPE   698 /* (64 General Ports + 1 CPU Port/Management Port +*/
                                             /* 1 Loop-back Port) * 10 */
#define SOC_TD3_CPU_MCQ_BASE             650 /* CPU port MC queue base index */
#define SOC_TD3_LB_MCQ_BASE              640 /* LB ports MC queue base index */
#define SOC_TD3_MGMT_UCQ_BASE            640 /* Mgmt ports UC queue base index */
#define SOC_TD3_MGMT_MCQ_BASE            650 /* Mgmt ports MC queue base index */
#define SOC_TD3_SECOND_MGMT_BASE         630 /* Second Mgmt ports UC/MC queue base index */

/* Number of Unicast Queues per Port */
#define SOC_TD3_NUM_UCAST_QUEUE_PER_PORT 10
/* Number of Multicast Queues per Port */
#define SOC_TD3_NUM_MCAST_QUEUE_PER_PORT 10
/* Number of Scheduler Nodes(L0) per Port */
#define SOC_TD3_NUM_SCHEDULER_PER_PORT   10

#define SOC_TD3_MMU_MCQ_ENTRY_PER_XPE    24576
#define SOC_TD3_MMU_RQE_ENTRY_PER_XPE    2048
#define _TD3_MMU_RQE_ENTRY_RSVD_PER_XPE  242   /* Reserved RQE QE per XPE */

#define _TD3_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_TD3_MMU_MCQ_ENTRY_PER_XPE
#define _TD3_MMU_TOTAL_RQE_ENTRY(unit)   SOC_TD3_MMU_RQE_ENTRY_PER_XPE

#define _TD3_MMU_CFAP_BANK_FULL_LIMIT    2047 /* CFAP Bank Full limit */
#define _TD3_TDM_MCQE_RSVD_OVERSHOOT     12   /* Reserved MC egress queue entry
                                                 for TDM overshoot */
#define _TD3_MCQE_RSVD_PER_MCQ           6    /* Reserved MC egress queue entry
                                                 per MC queue */
/* MMU pktstats control flags */
#define _TD3_MMU_PSTATS_ENABLE          0x0001 /* Packetized/OOB Stat mode is enabled */
#define _TD3_MMU_PSTATS_PKT_MOD         0x0002 /* Packetized mode is enabled. OOB Stat mode is disabled */
#define _TD3_MMU_PSTATS_HWM_MOD         0x0004 /* Report High Watermark values */
#define _TD3_MMU_PSTATS_RESET_ON_READ   0x0008 /* Enable Hardware clear-on read */
#define _TD3_MMU_PSTATS_MOR_EN          0x0010 /* Enable MOR capability */
#define _TD3_MMU_PSTATS_SYNC            0x8000 /* Read from device else the buffer */

/* Per pipe WRED config memory base index */
#define _TD3_WRED_PER_PIPE_UC_QUEUES_BASE  0    /* (64 General Ports + 1 Management Port) * 10 */
#define _TD3_WRED_PER_PIPE_PORT_SP_BASE    650  /* (64 General Ports + 1 Management Port) * 4 */
#define _TD3_WRED_PER_PIPE_GLOBAL_SP_BASE  910  /* (4 global service pools) */

extern void soc_trident3_mmu_pstats_tbl_config(int unit);
extern int  soc_trident3_mmu_pstats_mode_set(int unit, uint32 flags);
extern int  soc_trident3_mmu_pstats_mode_get(int unit, uint32 *flags);
extern int  soc_trident3_mmu_pstats_mor_enable(int unit);
extern int  soc_trident3_mmu_pstats_mor_disable(int unit);
extern int soc_trident3_mmu_splitter_reset(int unit);

typedef struct _soc_td3_mmu_sw_config_info_s {
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
} _soc_td3_mmu_sw_config_info_t;

#define SOC_TD3_STAT_ING_FLEX_POOL_MAX   20

/* Num of evict memories is 26, 1 is added for reserved mem */
#define SOC_TD3_NUM_EVICT_MEM            27

extern int
soc_trident3_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                               _soc_ser_sram_info_t *sram_info);

#define SOC_TD3_NUM_EL_VLAN_XLATE                     4

#define SOC_TD3_NUM_EL_EGR_VLAN_XLATE                 4
#define SOC_TD3_NUM_EL_MPLS_ENTRY                     4

#define SOC_TD3_NUM_EL_ING_L3_NEXT_HOP                4
#define SOC_TD3_RAM_OFFSET_ING_L3_NEXT_HOP            8192

#define SOC_TD3_NUM_ENTRIES_L2_BANK                   32768
#define SOC_TD3_NUM_ENTRIES_L2_DEDICATE_RAM           1024
#define SOC_TD3_NUM_ENTRIES_L2_SHARE_RAM              2048
#define SOC_TD3_NUM_ENTRIES_L3_BANK                   16384

#define SOC_TD3_NUM_EL_SHARED                         4
#define SOC_TD3_NUM_ENTRIES_XOR_BANK                  8192
#define SOC_TD3_LP_ENTRIES_IN_XOR_BANK                2048
#define SOC_TD3_LP_ENTRIES_IN_UFT_BANK                8192

#define SOC_TD3_ALPM_MAX_BKTS(u)    (SOC_IS_TRIDENT3X(u) ? 8192 : 16384)
#define SOC_TD3_ALPM_BKT_MASK(u)    (SOC_IS_TRIDENT3X(u) ? 0x1FFF : 0x3FFF)
#define SOC_TD3_ALPM_BKT_OFFFSET                      2048
#define SOC_TD3_ALPM_MODE0_BKT_MASK                   0x70007
#define SOC_TD3_ALPM_MODE1_BKT_MASK                   SOC_TD3_ALPM_MODE0_BKT_MASK >> 1

/* SUBPORT */
#define TD3_SUBPORT_COE_GROUP_MAX             (512)
#define TD3_SUBPORT_COE_PORT_MAX              (512)
#define TD3_MAX_SUBPORT_COE_PORT_PER_CASPORT  (80)
#define TD3_MAX_SUBPORT_COE_PORT_PER_MOD      (128)
#define TD3_MAX_COE_MODULES                   (5)

extern int
soc_trident3_clear_all_memory(int unit);

extern int
soc_trident3_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count);
extern void
soc_trident3_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
#if defined(SER_TR_TEST_SUPPORT)
extern int ser_test_td3_mem_index_remap(int unit, ser_test_data_t *test_data,
                                       int *mem_has_ecc);
extern soc_error_t
soc_td3_ser_parity_control_reg_set(int unit, ser_test_data_t *test_data, int enable);

extern soc_error_t
soc_td3_ser_error_injection_support(int unit, soc_mem_t mem, int pipe_target);

extern soc_error_t
soc_td3_ser_inject_or_test_mem(int unit, soc_mem_t mem, int pipe_target,
                               int block, int index, _soc_ser_test_t test_type,
                               int inject_only, int cmd, uint32 flags);


extern soc_error_t
soc_td3_ser_correction_info_get(int unit, ser_correction_info_t *corr_info);

extern soc_error_t
soc_td3_ser_test(int unit, _soc_ser_test_t test_type);

#endif /*defined(SER_TR_TEST_SUPPORT*/

typedef enum {
    _SOC_TD3_PARITY_TYPE_NONE,
    _SOC_TD3_PARITY_TYPE_ECC,
    _SOC_TD3_PARITY_TYPE_PARITY,
    _SOC_TD3_PARITY_TYPE_MMU_SER,
    _SOC_TD3_PARITY_TYPE_MMU_GLB,
    _SOC_TD3_PARITY_TYPE_MMU_XPE,
    _SOC_TD3_PARITY_TYPE_MMU_SC,
    _SOC_TD3_PARITY_TYPE_MMU_SED,
    _SOC_TD3_PARITY_TYPE_CLMAC,
    _SOC_TD3_PARITY_TYPE_XLMAC,
    _SOC_TD3_PARITY_TYPE_SER
} _soc_td3_ser_info_type_t;

typedef struct _soc_td3_ser_reg_s {
    soc_reg_t reg;
    char      *mem_str;
} _soc_td3_ser_reg_t;

typedef struct _soc_td3_ser_info_s {
    _soc_td3_ser_info_type_t   type;
    struct _soc_td3_ser_info_s *info;
    int                       id;
    soc_field_t               group_reg_enable_field;
    soc_field_t               group_reg_status_field;
    soc_mem_t                 mem;
    char                      *mem_str;
    soc_reg_t                 enable_reg;
    soc_field_t               enable_field;
    soc_field_t               *enable_field_list;
    soc_reg_t                 intr_enable_reg;
    soc_field_t               intr_enable_field;
    soc_field_t               *intr_enable_field_list;
    soc_reg_t                 intr_status_reg;
    _soc_td3_ser_reg_t         *intr_status_reg_list;
    soc_reg_t                 intr_status_field;
    soc_field_t               *intr_status_field_list;
    soc_reg_t                 intr_clr_reg;
    soc_field_t               intr_clr_field;
    soc_field_t               *intr_clr_field_list;
} _soc_td3_ser_info_t;

extern int
soc_td3_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                        soc_reg_t group_reg, uint64 *group_rval,
                        _soc_td3_ser_info_t *info_list,
                        soc_mem_t mem, int enable);
extern int
soc_td3_ser_process(int unit, int block_info_idx, int inst, int pipe,
                    int port, soc_reg_t group_reg, uint64 group_rval,
                    const _soc_td3_ser_info_t *info_list,
                    soc_block_t blocktype,
                    char *prefix_str);
extern soc_error_t
soc_td3_ser_tcam_wrapper_enable(int unit, int enable);

extern void
soc_td3_ser_test_register(int unit);
extern int
soc_trident3_process_ser_fifo(int unit, soc_block_t blk, int pipe,
                              char *prefix_str, int l2_mgmt_ser_fifo);
extern int soc_td3_ser_reg_field32_modify(int unit, soc_reg_t reg,
                                         soc_port_t port, soc_field_t field,
                                         uint32 value, int index,
                                         int mmu_base_index);
extern int
soc_td3_mem_parity_control(int unit, soc_mem_t mem, int copyno,
                              int en, int en_1b);
extern void
soc_trident3_process_func_intr(void *unit_vp, void *d1, void *d2,
                                void *d3, void *d4);
extern int
soc_td3_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                int mmu_base_index,
                                uint32 rval_intr_status_reg);
extern int
soc_td3_ser_mem_mode_get(int unit, soc_mem_t mem, int *mode);
extern int
soc_trident3_clear_mmu_memory(int unit);
extern int
soc_td3_check_scrub_skip(int unit, soc_mem_t mem, int check_hw_global_mode);
extern int
soc_td3_check_scrub_info(int unit, soc_mem_t mem, int blk, int copyno,
                        int *num_inst_to_scrub, int *acc_type_list);
extern int
soc_td3_check_hw_global_mode(int unit, soc_mem_t mem, int *mem_mode);
extern int
soc_td3_get_alpm_banks(int unit);
extern int
soc_trident3_alpm_mode_get(int unit);
extern int
soc_td3_tcam_engine_enable(int unit, int enable);
extern void
soc_td3_mem_scan_info_get(int unit, soc_mem_t mem, int block,
                         int *num_pipe, int *ser_flags);
extern int
soc_td3_mmu_bmp_reg_pos_get(int unit, soc_port_t port,
                            int *pipe, int *split, int *pos);
extern int
soc_trident3_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout);
extern int
soc_td3_port_traffic_egr_enable_set(int unit, soc_port_t port, int enable);
extern int
soc_td3_port_mmu_tx_enable_set(int unit, soc_port_t port, int enable);
extern int
soc_td3_port_mmu_tx_enable_get(int unit, soc_port_t port, int *enable);
extern int
soc_td3_port_mmu_rx_enable_set(int unit, soc_port_t port, int enable);
extern int
soc_td3_port_mmu_rx_enable_get(int unit, soc_port_t port, int *enable);
extern int
soc_td3_mmu_traffic_ctrl_size(int unit, int *req_scache_size);
extern int
soc_td3_mmu_traffic_ctrl_wb_sync(int unit, uint8 **scache_ptr);
extern int
soc_td3_mmu_traffic_ctrl_wb_restore(int unit, uint8 **scache_ptr);
extern void soc_td3_subport_init(int unit);

/************ FlexPort Start ********************/
extern int soc_td3_mmu_config_init_port(int unit, int port);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_td3_flexport_scache_allocate(int unit);
extern int soc_td3_flexport_scache_sync(int unit);
extern int soc_td3_flexport_scache_recovery(int unit);
extern int soc_td3_tdm_scache_allocate(int unit);
extern int soc_td3_tdm_scache_sync(int unit);
extern int soc_td3_tdm_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
/************ FlexPort End ********************/

int
soc_td3_max_lr_bandwidth(int unit, uint32 *max_lr_bw);
int
soc_td3_max_lr_cal_len(int unit, uint32 *max_lr_cal, int eth);
int
soc_td3_support_speeds(int unit, int lanes, uint32 *speed_mask);
uint32 _soc_td3_mmu_port(int unit, int port);

extern int
soc_td3_speed_mix_validate(int unit, uint32 speed_mask);
extern int
soc_td3_port_mode_get(int unit, int logical_port, int *mode);


#if !defined(BCM_TD3_ASF_EXCLUDE)
/*
  =============================
   TD3 ASF (cut-thru forwarding)
  =============================
*/
#define OUT
#define IN_OUT

/* ASF Modes */
typedef enum {
    _SOC_TD3_ASF_MODE_SAF          = 0,  /* store-and-forward */
    _SOC_TD3_ASF_MODE_SAME_SPEED   = 1,  /* same speed CT  */
    _SOC_TD3_ASF_MODE_SLOW_TO_FAST = 2,  /* slow to fast CT   */
    _SOC_TD3_ASF_MODE_FAST_TO_SLOW = 3,  /* fast to slow CT   */
    _SOC_TD3_ASF_MODE_CFG_UPDATE   = 4,  /* internal cfg updates */
    _SOC_TD3_ASF_MODE_INIT         = 5,  /* init */
    _SOC_TD3_ASF_MODE_UNSUPPORTED  = 6
} soc_td3_asf_mode_e;

/* ASF MMU Cell Buffer Allocation Profiles */
typedef enum {
    _SOC_TD3_ASF_MEM_PROFILE_NONE      = 0,  /* No cut-through support */
    _SOC_TD3_ASF_MEM_PROFILE_SIMILAR   = 1,  /* Similar Speed Alloc Profile  */
    _SOC_TD3_ASF_MEM_PROFILE_EXTREME   = 2   /* Extreme Speed Alloc Profile */
} soc_td3_asf_mem_profile_e;

/* Latency modes */
typedef enum {
    _SOC_TD3_L2_LATENCY   = 0,
    _SOC_TD3_L3_LATENCY   = 1,
    _SOC_TD3_FULL_LATENCY = 2
} _soc_td3_latency_mode_e;

extern int
soc_td3_port_oversub_ratio_get(
    int          unit,
    soc_port_t   port,
    int* const   ratio);

extern int
soc_td3_asf_init_start(int unit);

extern int
soc_td3_asf_init_done(int unit);

extern int
soc_td3_port_asf_init(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_td3_asf_mode_e  mode);

extern int
soc_td3_asf_deinit(int unit);

extern int
soc_td3_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_td3_asf_mode_e   mode,
    uint8                extra_cells);

extern int
soc_td3_port_asf_mode_get(
    int                            unit,
    soc_port_t                     port,
    int                            port_speed,
    OUT soc_td3_asf_mode_e* const  mode);

extern int
soc_td3_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_td3_asf_mode_e  mode);

extern int
soc_td3_port_asf_params_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_td3_asf_mode_e  mode);

extern int
soc_td3_port_asf_speed_to_mmu_cell_credit(
    int        unit,
    soc_port_t port,
    int        port_speed,
    OUT uint32 *mmu_cell_credit);

extern int
soc_td3_port_asf_mmu_cell_credit_to_speed(
    int        unit,
    soc_port_t port,
    uint8      mmu_cell_credit,
    OUT int    *port_speed);

extern pbmp_t *
soc_td3_port_asf_pause_bpmp_get(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_td3_asf_wb_memsz_get(
    int unit,
    OUT uint32* const wb_mem_sz,
    uint16            scache_ver);

extern int
soc_td3_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data);

extern int
soc_td3_asf_wb_recover(
    int unit,
    uint8* const wb_data,
    uint16       scache_ver);
#endif

extern int
soc_td3_asf_pbmp_get(int unit);

extern int
soc_td3_port_asf_show(
    int          unit,
    soc_port_t   port,
    int          port_speed);

extern int
soc_td3_asf_config_dump(int unit);

extern int
soc_td3_port_asf_config_dump(
	int          unit,
	soc_port_t   port,
	int          port_speed);

extern int
soc_td3_port_asf_detach(
    int          unit,
    soc_port_t   port);
#endif

extern soc_error_t
soc_td3_granular_speed_get(int unit, soc_port_t port, int *speed);

extern int
soc_td3_lpm_flex_ip4entry0_to_1(
    int u,
    void *src,
    void *dst,
    uint32 key_type,
    int copy_hit);

extern int
soc_td3_lpm_flex_ip4entry0_to_0(
    int u,
    void *src,
    void *dst,
    uint32 key_type,
    int copy_hit);

extern int
soc_td3_lpm_flex_prefix_length_get(
    int u,
    void *entry,
    int *ipv6,
    int *vrf_id,
    int *pfx,
    uint32 key_type);
extern int soc_td3_post_mmu_init_update(int unit);

extern int
soc_td3_mpls_hash_control_set(int *hash_entry);

extern int
_soc_trident3_init_mmu_memory(int unit);
extern int soc_td3_frequency_init(int unit);

extern int
_soc_trident3_pll_dco_code_get(int unit, soc_td3_plls_e pll, uint32 *dco_code);

extern int
_soc_td3_pll_dco_normalize(uint32 *dco_code, uint32 ref_freq, uint32 ndiv);


extern int
soc_trident3_age_timer_get(int unit, int *age_seconds, int *enabled);

extern int
soc_trident3_age_timer_max_get(int unit, int *max_seconds);

extern int
soc_trident3_age_timer_set(int unit, int age_seconds, int enable);

extern int
soc_trident3_tscx_firmware_set(int unit, int port, uint8 *array, int datalen);

extern int
soc_trident3_latency_config(int unit);

extern int
soc_td3_set_idb_dpp_ctrl(int unit);

extern int
soc_flex_hash_mem_view_get(int unit, soc_mem_t mem,uint32 *entry,
                           soc_mem_t *mem_view, soc_field_t *field_list);

#ifdef INCLUDE_AVS
extern soc_error_t
soc_td3_avs_init(int unit);
#endif /* INCLUDE_AVS */

#endif /* !_SOC_TRIDENT3_H_ */
