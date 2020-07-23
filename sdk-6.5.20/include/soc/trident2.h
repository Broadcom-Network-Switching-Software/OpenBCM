/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident2.h
 */

#ifndef _SOC_TRIDENT2_H_
#define _SOC_TRIDENT2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/mmu_config.h>

#define SOC_TD2_NUM_EL_L2                             4
#define SOC_TD2_NUM_EL_SHARED                         8
#define SOC_TD2_NUM_EL_ALPM                           4
#define SOC_TD2_NUM_EL_VLAN_XLATE                     4
#define SOC_TD2_NUM_EL_EGR_VLAN_XLATE                 4
#define SOC_TD2_NUM_EL_ING_L3_NEXT_HOP                6
#define SOC_TD2_NUM_EL_L3_IPMC                        4

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#define SOC_TD2P_NUM_EL_SHARED                       16
#define SOC_TD2P_ALPM_MODE0_BKT_MASK            0x70003
#define SOC_TD2P_NUM_EL_L3                            3
#define SOC_TD2P_NUM_EL_L3_IPMC                       8
#endif

#define SOC_TD2_RAM_OFFSET_L2_BANK                 4096
#define SOC_TD2_RAM_OFFSET_L2MC                    4096  /* size of every ram */
#define SOC_TD2_NUM_EL_L2MC(unit)  \
((soc_mem_index_max(unit, L2MCm) + 1) / SOC_TD2_RAM_OFFSET_L2MC) /* Number of L2MC RAM*/

#define SOC_TD2_RAM_OFFSET_L2_SHARED_BANK          8192

#define SOC_TD2_RAM_OFFSET_L3_NARROW               8192
#define SOC_TD2_RAM_OFFSET_L3_WIDE                 4096
#define SOC_TD2_RAM_OFFSET_L3_DOUBLE_WIDE          2048

#define SOC_TD2_RAM_OFFSET_ING_L3_NEXT_HOP         8192
#define SOC_TD2_RAM_OFFSET_L3_IPMC                 2048

#define SOC_TD2_L3_DEFIP_ALPM_PER_BKT_COUNT           1
#define SOC_TD2_L3_DEFIP_ALPM_IPV6_128_PER_BKT_COUNT  2
#define SOC_TD2_L3_DEFIP_ALPM_IPV6_64_1_PER_BKT_COUNT 3
#define SOC_TD2_L3_DEFIP_ALPM_IPV6_64_PER_BKT_COUNT   4
#define SOC_TD2_L3_DEFIP_ALPM_IPV4_1_PER_BKT_COUNT    4
#define SOC_TD2_L3_DEFIP_ALPM_IPV4_PER_BKT_COUNT      6

#define SOC_TD2_NUM_ENTRIES_PER_L2_BANK           16384
#define SOC_TD2_NUM_ENTRIES_L2_BANK               32768
#define SOC_TD2_NUM_ENTRIES_L3_BANK               16384
#define SOC_TD2_NUM_ENTRIES_L3_BANK_WIDE           8192
#define SOC_TD2_NUM_ENTRIES_L3_BANK_DOUBLE_WIDE    4096

#define SOC_TD2_ALPM_MAX_BKTS                     16384
#define SOC_TD2_ALPM_BKT_OFFFSET                   2048
#define SOC_TD2_ALPM_BKT_MASK                    0x3FFF

#define _SOC_TD2_L2LU_INTR_MASK                 0x01000
#define _SOC_TD2_FUNC_INTR_MASK _SOC_TD2_L2LU_INTR_MASK

#define SOC_TD2_PORT_CT_SPEED_NONE                  0x0
#define SOC_TD2_PORT_CT_SPEED_10M_HALF              0x2
#define SOC_TD2_PORT_CT_SPEED_10M_FULL              0x3
#define SOC_TD2_PORT_CT_SPEED_100M_HALF             0x4
#define SOC_TD2_PORT_CT_SPEED_100M_FULL             0x5
#define SOC_TD2_PORT_CT_SPEED_1000M_HALF            0x6
#define SOC_TD2_PORT_CT_SPEED_1000M_FULL            0x7
#define SOC_TD2_PORT_CT_SPEED_2500M_HALF            0x8
#define SOC_TD2_PORT_CT_SPEED_2500M_FULL            0x9
#define SOC_TD2_PORT_CT_SPEED_5000M_FULL            0xa
#define SOC_TD2_PORT_CT_SPEED_10000M_FULL           0xc
#define SOC_TD2_PORT_CT_SPEED_11000M_FULL           0xb
#define SOC_TD2_PORT_CT_SPEED_12000M_FULL           0xd
#define SOC_TD2_PORT_CT_SPEED_13000M_FULL           0xe 
#define SOC_TD2_PORT_CT_SPEED_15000M_FULL           0xf
#define SOC_TD2_PORT_CT_SPEED_16000M_FULL          0x10
#define SOC_TD2_PORT_CT_SPEED_20000M_FULL          0x12
#define SOC_TD2_PORT_CT_SPEED_21000M_FULL          0x13
#define SOC_TD2_PORT_CT_SPEED_24000M_FULL          0x14
#define SOC_TD2_PORT_CT_SPEED_30000M_FULL          0x18
#define SOC_TD2_PORT_CT_SPEED_32000M_FULL          0x19
#define SOC_TD2_PORT_CT_SPEED_40000M_FULL          0x1c
#define SOC_TD2_PORT_CT_SPEED_42000M_FULL          0x1d
#define SOC_TD2_PORT_CT_SPEED_100000M_FULL         0x1e
#define SOC_TD2_PORT_CT_SPEED_120000M_FULL         0x1f

#define SOC_TD2_NUM_MMU_PORTS_PER_PIPE               53

#define SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE         16

#define _TD2_COSQ_NUM_CPU_L0_NODES          4
#define _TD2PLUS_COSQ_NUM_CPU_L0_NODES      6

#define SOC_TD2_COSQ_NUM_CPU_L0_NODES(unit)       \
    (SOC_IS_TD2P_TT2P(unit) ? _TD2PLUS_COSQ_NUM_CPU_L0_NODES : _TD2_COSQ_NUM_CPU_L0_NODES)
#define SOC_TD2_COSQ_CPU_RESERVED_L0_NUM(unit)    \
    (SOC_IS_TD2P_TT2P(unit) ? 0 : _TD2_COSQ_NUM_CPU_L0_NODES)

#define SOC_TD2_COSQ_CPU_RESERVED_L0_BASE(unit)   \
    (SOC_IS_TD2P_TT2P(unit) ? 259 : 261)

#define _TD2_PORTS_PER_XLP         4
#define _TD2_XLPS_PER_PGW          4
#define _TD2_PGWS_PER_QUAD         2
#define _TD2_QUADS_PER_PIPE        2
#define _TD2_PIPES_PER_DEV         2

#define _TD2_QUADS_PER_DEV         (_TD2_QUADS_PER_PIPE * _TD2_PIPES_PER_DEV)

#define _TD2_PGWS_PER_PIPE         (_TD2_PGWS_PER_QUAD * _TD2_QUADS_PER_PIPE)
#define _TD2_PGWS_PER_DEV          (_TD2_PGWS_PER_PIPE * _TD2_PIPES_PER_DEV)

#define _TD2_PORTS_PER_PGW         (_TD2_PORTS_PER_XLP * _TD2_XLPS_PER_PGW)
#define _TD2_PORTS_PER_QUAD        (_TD2_PORTS_PER_PGW * _TD2_PGWS_PER_QUAD)
#define _TD2_PORTS_PER_PIPE        (_TD2_PORTS_PER_QUAD * _TD2_QUADS_PER_PIPE)

#define _TD2_BYTES_PER_OBM_CELL    16

#define _TD2_LOG_PORTS_PER_PIPE    53
#define _TD2_MMU_PORTS_PER_PIPE    53
#define _TD2_NUM_LOG_PORTS_PER_DEV (_TD2_LOG_PORTS_PER_PIPE * _TD2_PIPES_PER_DEV)

#define SOC_FIELD_RANGE_CHECK(val, min, max) \
    (((val) > (min)) && ((val) <= (max)))

typedef enum {
    SOC_TD2_SCHED_MODE_UNKNOWN = 0,
    SOC_TD2_SCHED_MODE_STRICT,
    SOC_TD2_SCHED_MODE_WRR,
    SOC_TD2_SCHED_MODE_WDRR
} soc_td2_sched_mode_e;

typedef enum {
    SOC_TD2_NODE_LVL_ROOT = 0,
    SOC_TD2_NODE_LVL_L0,
    SOC_TD2_NODE_LVL_L1,
    SOC_TD2_NODE_LVL_L2,
    SOC_TD2_NODE_LVL_MAX
} soc_td2_node_lvl_e;

typedef enum {
    _SOC_TD2_INDEX_STYLE_BUCKET,
    _SOC_TD2_INDEX_STYLE_BUCKET_MODE,
    _SOC_TD2_INDEX_STYLE_WRED,
    _SOC_TD2_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_TD2_INDEX_STYLE_SCHEDULER,
    _SOC_TD2_INDEX_STYLE_PERQ_XMT,
    _SOC_TD2_INDEX_STYLE_UCAST_DROP,
    _SOC_TD2_INDEX_STYLE_UCAST_QUEUE,
    _SOC_TD2_INDEX_STYLE_MCAST_QUEUE,
    _SOC_TD2_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_TD2_INDEX_STYLE_EGR_POOL,
    _SOC_TD2_INDEX_STYLE_COUNT
} soc_td2_index_style_t;

typedef enum soc_td2_drop_limit_alpha_value_e {
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */                                          
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_TD2_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_td2_drop_limit_alpha_value_t;

extern uint8 _soc_alpm_mode[SOC_MAX_NUM_DEVICES];

extern int _soc_aacl_en[SOC_MAX_NUM_DEVICES];

extern uint32* soc_td2_mmu_params_arr_get (uint16 dev_id, uint8 rev_id);

extern soc_functions_t soc_trident2_drv_funs;
extern int _soc_trident2_mem_ser_control(int unit, soc_mem_t mem,
                                         int copyno, int enable);
extern int _soc_trident2_mem_cpu_write_control(int unit, soc_mem_t mem,
                                               int copyno, int enable,
                                               int *orig_enable);
extern int soc_trident2_ser_mem_clear(int unit, soc_mem_t mem);
extern int soc_trident2_alpm_scrub(int unit);
extern void _soc_trident2_alpm_bkt_view_set(int unit, int index, soc_mem_t view);
extern soc_mem_t _soc_trident2_alpm_bkt_view_get(int unit, int index);
extern int soc_trident2_mem_config(int unit);
extern int soc_trident2_alpm_mode_get(int unit);
extern int soc_trident2_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_trident2_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_trident2_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem);
extern int _soc_trident2_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                                           _soc_ser_sram_info_t *sram_info);
extern int soc_trident2_pipe_select(int unit, int egress, int pipe);
extern int soc_trident2_num_cosq_init(int unit);
extern int soc_trident2_tsc_map_get(int unit, uint32 *tsc_map);
extern int soc_trident2_port_config_init(int unit, uint16 dev_id, uint8 rev_id);
extern int soc_trident2_lls_bmap_alloc(int unit);
extern int soc_trident2_lls_bmap_alloc_port(int unit, int port, int ets_mode);
extern int soc_trident2_chip_reset(int unit);
extern int soc_trident2_tsc_reset(int unit);
extern void trident2p_ovstb_toggle(int unit);
extern int soc_trident2_l3_memwr_parity_check(int unit, soc_mem_t mem, int disable);

int soc_td2_mmu_traffic_ctrl_size(int unit, int *req_scache_size);

int soc_td2_mmu_traffic_ctrl_wb_sync(int unit, uint8 **scache_ptr);

int soc_td2_mmu_traffic_ctrl_wb_restore(int unit, uint8 **scache_ptr);


typedef struct soc_td2_port_lanes_s {
    int port_base;      /* Base port number */
    int lanes;          /* New number of lanes */
    int cur_lanes;      /* Current number of lanes */
    int mode;           /* New mode */
    int cur_mode;       /* Current mode */
    int weight;         /* Weight for new group */
    int cur_weight;     /* Weight for current group */
    int group;          /* New MMU oversub group number */
    int cur_group;      /* Current MMU oversub group number */
    int slot;           /* New MMU oversub group slot number */
    int cur_slot;       /* Current MMU oversub group slot number */
    int cur_empty;      /* Current MMU oversub group become empty after flex */
    int bindex;         /* Base port subport index */
    int oversub;        /* Oversub status for the ports */
    int phy_ports_len;  /* Length of valid entries in phy_ports array */
    int phy_ports[3];   /* Array of added/removed physical port number */
} soc_td2_port_lanes_t;
extern int soc_trident2_port_lanes_validate(int unit,
                                            soc_td2_port_lanes_t *lanes_ctrl);
extern int soc_trident2_port_lanes_set(int unit,
                                       soc_td2_port_lanes_t *lanes_ctrl);
extern int soc_trident2_port_lanes_get(int unit, soc_port_t port_base,
                                       int *cur_lanes);
extern int soc_trident2_temperature_monitor_get(int unit,
               int temperature_max,
               soc_switch_temperature_monitor_t *temperature_array,
               int *temperature_count);
extern int soc_trident2_show_material_process(int unit);
extern int soc_trident2_show_ring_osc(int unit);

enum soc_td2_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_TD2_PORT_MODE_QUAD = 0,
    SOC_TD2_PORT_MODE_TRI_012 = 1,
    SOC_TD2_PORT_MODE_TRI_023 = 2,
    SOC_TD2_PORT_MODE_DUAL = 3,
    SOC_TD2_PORT_MODE_SINGLE = 4
};

/*
 * Miscellaneous SOC port data used for validation
 *
 * Most fields are equivalent to the fields in the SOC_INFO data
 * structure.  Use same field names.
 */
typedef struct soc_info_misc_s {
    int              port_l2p_mapping[SOC_MAX_NUM_PORTS]; /* Logic to phys */
    int              port_p2l_mapping[SOC_MAX_NUM_PORTS]; /* Phys to logic */
    int              port_speed_max[SOC_MAX_NUM_PORTS];   /* Max speed */
    int              port_speed[SOC_MAX_NUM_PORTS];       /* Current speed */
    int              port_num_lanes[SOC_MAX_NUM_PORTS];   /* Number of lanes */
    soc_encap_mode_t port_encap[SOC_MAX_NUM_PORTS];       /* Encap mode */
    int              bandwidth;                           /* Bandwidth  */
    pbmp_t           oversub_pbm;                         /* Oversubscription */
    pbmp_t           disabled_bitmap;                     /* Disabled ports */
    pbmp_t           management_pbm;                      /* Management ports */
} soc_info_misc_t;

typedef struct soc_pmap_info_s {
    int port;
    int phy_port;
    int port_bandwidth;
    int lane_config;
    int fallback_lane;
    int pipe;
    char *option;
    soc_pbmp_t oversub_pbm;
} soc_pmap_info_t;

extern void
soc_trident2_soc_info_misc_t_init(int unit, soc_info_misc_t *si_m);
extern int
soc_trident2_port_bandwidth_validate(int unit, soc_info_misc_t *si_m);
extern int
soc_trident2_port_portmap_parse(int unit, soc_port_t port,
                                soc_pmap_info_t *pmap);

enum td2_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2_L2_HASH_KEY_TYPE_BRIDGE = 0,
    TD2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    TD2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    TD2_L2_HASH_KEY_TYPE_VFI = 3,
    TD2_L2_HASH_KEY_TYPE_VIF = 4,
    TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS = 5,
    TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG = 6,
    TD2_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT = 7,
    TD2_L2_HASH_KEY_TYPE_BFD = 8,
    TD2_L2_HASH_KEY_TYPE_PE_VID = 9,
    TD2_L2_HASH_KEY_TYPE_FCOE_ZONE = 10,
    TD2_L2_HASH_KEY_TYPE_COUNT
};

enum td2_l3_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2_L3_HASH_KEY_TYPE_V4UC = 0,
    TD2_L3_HASH_KEY_TYPE_V4UC_EXT = 1,
    TD2_L3_HASH_KEY_TYPE_V6UC = 2,
    TD2_L3_HASH_KEY_TYPE_V6UC_EXT = 3,
    TD2_L3_HASH_KEY_TYPE_V4MC = 4,
    TD2_L3_HASH_KEY_TYPE_V6MC = 5,
    /* 6,7 are not used */
    TD2_L3_HASH_KEY_TYPE_TRILL = 8,
    /* 9,10,11 is not used */
    TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN = 12,
    TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT = 13,
    TD2_L3_HASH_KEY_TYPE_FCOE_HOST = 14,
    TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT = 15,
    TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP = 16,
    TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT = 17,
    TD2_L3_HASH_KEY_TYPE_DST_NAT = 18,
    TD2_L3_HASH_KEY_TYPE_DST_NAPT = 19,
    TD2_L3_HASH_KEY_TYPE_V4L2MC = 20,
    TD2_L3_HASH_KEY_TYPE_V4L2VPMC = 21,
    TD2_L3_HASH_KEY_TYPE_V6L2MC = 22,
    TD2_L3_HASH_KEY_TYPE_V6L2VPMC = 23,
    TD2_L3_HASH_KEY_TYPE_COUNT
};

enum td2_mpls_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2_MPLS_HASH_KEY_TYPE_MPLS = 0,
    TD2_MPLS_HASH_KEY_TYPE_MIM_NVP = 1,
    TD2_MPLS_HASH_KEY_TYPE_MIM_ISID = 2,
    TD2_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP = 3,
    TD2_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP = 4,
    TD2_MPLS_HASH_KEY_TYPE_TRILL = 5,
    TD2_MPLS_HASH_KEY_TYPE_L2GRE_SIP = 6,
    TD2_MPLS_HASH_KEY_TYPE_L2GRE_VPNID = 7,
    TD2_MPLS_HASH_KEY_TYPE_VXLAN_SIP = 8,
    TD2_MPLS_HASH_KEY_TYPE_VXLAN_VPNID = 9,
    TD2_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP = 10,
    TD2_MPLS_HASH_KEY_TYPE_COUNT
};

enum td2_vlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    TD2_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    TD2_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    TD2_VLXLT_HASH_KEY_TYPE_OVID = 4,
    TD2_VLXLT_HASH_KEY_TYPE_IVID = 5,
    TD2_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    TD2_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    TD2_VLXLT_HASH_KEY_TYPE_VIF = 8,
    TD2_VLXLT_HASH_KEY_TYPE_VIF_VLAN = 9,
    TD2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN = 10,
    TD2_VLXLT_HASH_KEY_TYPE_VIF_OTAG = 11,
    TD2_VLXLT_HASH_KEY_TYPE_VIF_ITAG = 12,
    TD2_VLXLT_HASH_KEY_TYPE_L2GRE_DIP = 13,
    TD2_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT = 14,
    TD2_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN = 15,
    TD2_VLXLT_HASH_KEY_TYPE_IVID_VSAN = 16,
    TD2_VLXLT_HASH_KEY_TYPE_OVID_VSAN = 17,
    TD2_VLXLT_HASH_KEY_TYPE_VXLAN_DIP = 18,
    TD2_VLXLT_HASH_KEY_TYPE_COUNT
};

enum td2_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE = 0,
    TD2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP = 1,
    TD2_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE = 2,
    TD2_EVLXLT_HASH_KEY_TYPE_ISID_XLATE = 3,
    TD2_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE = 4,
    TD2_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI = 5,
    TD2_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP = 6,
    TD2_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP = 7,
    TD2_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI = 8,
    TD2_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP = 9,
    TD2_VELXLT_HASH_KEY_TYPE_COUNT
};

extern void
_soc_trident2_quad_bandwidth_calculate(int unit, int quad,
                                       int *max_quad_core_bandwidth,
                                       int *quad_linerate_bandwidth,
                                       int *quad_oversub_bandwidth);
extern int
soc_trident2_get_prio_map(int unit, int port, uint16 *pri_mask);

extern int      soc_trident2_hash_bank_count_get(int unit, soc_mem_t mem,
                                                 int *num_banks);
extern int      soc_trident2_hash_bank_bitmap_get(int unit, soc_mem_t mem,
                                                  uint32 *bitmap);
extern int      soc_trident2_hash_bank_number_get(int unit, soc_mem_t mem,
                                                  int seq_num, int *bank_num);
extern int      soc_trident2_hash_seq_number_get(int unit, soc_mem_t mem, 
                                                  int bank_num, int *seq_num);
extern int      soc_trident2_hash_bank_info_get(int unit, soc_mem_t mem,
                                                int bank,
                                                int *entries_per_bank,
                                                int *entries_per_row,
                                                int *entries_per_bucket,
                                                int *bank_base,
                                                int *bucket_offset);
extern int      soc_td2_hash_offset_get(int unit, soc_mem_t mem, int bank,
                                        int *hash_offset, int *use_lsb);
extern uint32   soc_td2_l2x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key);
extern int      soc_td2_l2x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_td2_l2x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_td2_l2x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_td2_l3x_hash(int unit, int bank, int hash_offset,
                                 int use_lsb, int key_nbits, void *base_entry,
                                 uint8 *key);
extern int      soc_td2_l3x_base_entry_to_key(int unit, int bank, uint32 *entry,
                                              uint8 *key);
extern uint32   soc_td2_l3x_entry_hash(int unit, int bank, int hash_offset,
                                       int use_lsb, uint32 *entry);
extern uint32   soc_td2_l3x_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_td2_mpls_hash(int unit, int hash_sel, int key_nbits,
                                  void *base_entry, uint8 *key);
extern int      soc_td2_mpls_base_entry_to_key(int unit, int bank, void *entry,
                                               uint8 *key);
extern uint32   soc_td2_mpls_entry_hash(int unit, int hash_sel,
                                        int bank, uint32 *entry);
extern uint32   soc_td2_mpls_bank_entry_hash(int unit, int bank, uint32 *entry);
extern uint32   soc_td2_vlan_xlate_hash(int unit, int hash_sel, int key_nbits,
                                        void *base_entry, uint8 *key);
extern int      soc_td2_vlan_xlate_base_entry_to_key(int unit, int bank,
                                                     void *entry,
                                                     uint8 *key);
extern uint32   soc_td2_vlan_xlate_entry_hash(int unit, int hash_sel, int bank,
                                              uint32 *entry);
extern uint32   soc_td2_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                   uint32 *entry);
extern uint32   soc_td2_egr_vlan_xlate_hash(int unit, int hash_sel,
                                            int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_td2_egr_vlan_xlate_base_entry_to_key(int unit, int bank,
                                                         void *entry,
                                                         uint8 *key);
extern uint32   soc_td2_egr_vlan_xlate_entry_hash(int unit,
                                                  int hash_sel,
                                                  int bank,
                                                  uint32 *entry);
extern uint32   soc_td2_egr_vlan_xlate_bank_entry_hash(int unit, int bank,
                                                       uint32 *entry);
extern uint32   soc_td2_ing_vp_vlan_member_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry, uint8 *key);
extern int      soc_td2_ing_vp_vlan_member_base_entry_to_key(int unit, int bank,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_td2_ing_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                      int bank,
                                                      uint32 *entry);
extern uint32   soc_td2_egr_vp_vlan_member_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry, uint8 *key);
extern int      soc_td2_egr_vp_vlan_member_base_entry_to_key(int unit, int bank,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_td2_egr_vp_vlan_member_entry_hash(int unit, int hash_sel,
                                                      int bank,
                                                      uint32 *entry);
extern uint32   soc_td2_ing_dnat_address_type_hash(int unit, int hash_sel,
                                                   int key_nbits,
                                                   void *base_entry,
                                                   uint8 *key);
extern int      soc_td2_ing_dnat_address_type_base_entry_to_key(int unit,
                                                                int bank,
                                                                void *entry,
                                                                uint8 *key);
extern uint32   soc_td2_ing_dnat_address_type_entry_hash(int unit,
                                                         int hash_sel,
                                                         int bank,
                                                         uint32 *entry);
extern uint32   soc_td2_l2_endpoint_id_hash(int unit, int hash_sel,
                                            int key_nbits, void *base_entry,
                                            uint8 *key);
extern int      soc_td2_l2_endpoint_id_base_entry_to_key(int unit, int bank,
                                                         void *entry,
                                                         uint8 *key);
extern uint32   soc_td2_l2_endpoint_id_entry_hash(int unit,
                                                  int hash_sel,
                                                  int bank,
                                                  uint32 *entry);
extern uint32   soc_td2_endpoint_queue_map_hash(int unit, int hash_sel,
                                                int key_nbits,
                                                void *base_entry,
                                                uint8 *key);
extern int      soc_td2_endpoint_queue_map_base_entry_to_key(int unit,
                                                             int bank,
                                                             void *entry,
                                                             uint8 *key);
extern uint32   soc_td2_endpoint_queue_map_entry_hash(int unit,
                                                      int hash_sel,
                                                      int bank,
                                                      uint32 *entry);
extern int      soc_hash_bucket_get(int unit, int mem, int bank,
                                    uint32 *entry, uint32 *bucket);
extern int      soc_hash_index_get(int unit, int mem, int bank, int bucket);

extern void
soc_cm_get_id_otp(int unit, uint16 *dev_id, uint8 *rev_id);

/* TDM algorithm */
extern int set_tdm_tbl
    (int speed[130], int tdm_bw,
     int pgw_tdm_tbl_x0[32], int ovs_tdm_tbl_x0[32], int ovs_spacing_x0[32],
     int pgw_tdm_tbl_x1[32], int ovs_tdm_tbl_x1[32], int ovs_spacing_x1[32],
     int pgw_tdm_tbl_y0[32], int ovs_tdm_tbl_y0[32], int ovs_spacing_y0[32],
     int pgw_tdm_tbl_y1[32], int ovs_tdm_tbl_y1[32], int ovs_spacing_y1[32],
     int mmu_tdm_tbl_x[256], int mmu_tdm_ovs_x_1[16], int mmu_tdm_ovs_x_2[16],
     int mmu_tdm_ovs_x_3[16], int mmu_tdm_ovs_x_4[16],
     int mmu_tdm_tbl_y[256], int mmu_tdm_ovs_y_1[16], int mmu_tdm_ovs_y_2[16],
     int mmu_tdm_ovs_y_3[16], int mmu_tdm_ovs_y_4[16],
     int port_state_map[128],
     int iarb_tdm_tbl_x[512], int iarb_tdm_tbl_y[512]);
extern int set_iarb_tdm_table(int core_bw, int is_x_ovs, int is_y_ovs,
                              int mgm4x1, int mgm4x2p5, int mgm1x10,
                              int *iarb_tdm_wrap_ptr_x,
                              int *iarb_tdm_wrap_ptr_y,
                              int iarb_tdm_tbl_x[512],
                              int iarb_tdm_tbl_y[512]);
extern int 
soc_td2_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                       int *pbase, int *pnumq);

#define SOC_TD2_PORT_PIPE(u, p) \
            (SOC_PBMP_MEMBER(SOC_INFO(u).xpipe_pbm,(p)) ? 0 : 1)

extern soc_mem_t
_soc_trident2_pmem(int unit, int port, soc_mem_t memx, soc_mem_t memy);

#define SOC_TD2_PMEM(u,p,m1,m2)   _soc_trident2_pmem((u),(p),(m1),(m2))

extern soc_reg_t
_soc_trident2_preg(int unit, int port, soc_reg_t reg_x, soc_reg_t reg_y);

#define SOC_TD2_PREG(u,p,m1,m2)   _soc_trident2_preg((u),(p),(m1),(m2))

extern uint32 _soc_trident2_mmu_port(int unit, int port);

#define SOC_TD2_MMU_PORT(u, p)  _soc_trident2_mmu_port((u),(p))

extern uint32 _soc_trident2_piped_mem_index(int unit, soc_port_t port,
                                 soc_mem_t mem, int arr_off);

#define SOC_TD2_MMU_PIPED_MEM_INDEX(u,p,m,off) \
                    _soc_trident2_piped_mem_index((u),(p),(m),(off))

#define _SOC_TD2_NODE_CONFIG_MEM(u,p,n)                                 \
    (((n)==SOC_TD2_NODE_LVL_ROOT) ? \
        SOC_TD2_PMEM((u),(p),ES_PIPE0_LLS_PORT_MEMA_CONFIGm,    \
                             ES_PIPE1_LLS_PORT_MEMA_CONFIGm) : \
     (((n) == SOC_TD2_NODE_LVL_L0) ? \
        SOC_TD2_PMEM((u),(p),ES_PIPE0_LLS_L0_MEMA_CONFIGm, \
                             ES_PIPE1_LLS_L0_MEMA_CONFIGm) : \
      ((n) == SOC_TD2_NODE_LVL_L1 ? \
        SOC_TD2_PMEM((u),(p),ES_PIPE0_LLS_L1_MEMA_CONFIGm, \
                             ES_PIPE1_LLS_L1_MEMA_CONFIGm) : -1  )))

#define _SOC_TD2_NODE_CONFIG_MEM2(u,p,n)                                 \
    (((n)==SOC_TD2_NODE_LVL_ROOT) ? \
        SOC_TD2_PMEM((u),(p),ES_PIPE0_LLS_PORT_MEMB_CONFIGm,    \
                             ES_PIPE1_LLS_PORT_MEMB_CONFIGm) : \
     (((n) == SOC_TD2_NODE_LVL_L0) ? \
        SOC_TD2_PMEM((u),(p),ES_PIPE0_LLS_L0_MEMB_CONFIGm, \
                             ES_PIPE1_LLS_L0_MEMB_CONFIGm) : \
      ((n) == SOC_TD2_NODE_LVL_L1 ? \
        SOC_TD2_PMEM((u),(p),ES_PIPE0_LLS_L1_MEMB_CONFIGm, \
                             ES_PIPE1_LLS_L1_MEMB_CONFIGm) : -1  )))

#define _SOC_TD2_NODE_PARENT_MEM(u,p,n)                                 \
    (((n)==SOC_TD2_NODE_LVL_L2) ? \
        SOC_TD2_PMEM((u),(p), ES_PIPE0_LLS_L2_PARENTm, \
                              ES_PIPE1_LLS_L2_PARENTm) : \
     (((n) == SOC_TD2_NODE_LVL_L1) ? \
        SOC_TD2_PMEM((u),(p), ES_PIPE0_LLS_L1_PARENTm, \
                              ES_PIPE1_LLS_L1_PARENTm) : \
      (((n) == SOC_TD2_NODE_LVL_L0) ? \
        SOC_TD2_PMEM((u),(p), ES_PIPE0_LLS_L0_PARENTm, \
                              ES_PIPE1_LLS_L0_PARENTm) : -1  )))

#define _SOC_TD2_PIPED_NODE_PARENT_MEM(u,p,n)                                 \
    (((n) == SOC_TD2_NODE_LVL_L2) ? \
        (p ? ES_PIPE1_LLS_L2_PARENTm:  \
             ES_PIPE0_LLS_L2_PARENTm) :  \
    (((n) == SOC_TD2_NODE_LVL_L1) ? \
        (p ? ES_PIPE1_LLS_L1_PARENTm:  \
             ES_PIPE0_LLS_L1_PARENTm)   :  \
    (((n) == SOC_TD2_NODE_LVL_L0) ? \
        (p ? ES_PIPE1_LLS_L0_PARENTm:  \
             ES_PIPE0_LLS_L0_PARENTm)   :  -1 )))  \

#define _SOC_TD2_NODE_WIEGHT_MEM(u,p,n)                                 \
    (((n)==SOC_TD2_NODE_LVL_L0) ? \
        SOC_TD2_PMEM((u),(p), ES_PIPE0_LLS_L0_CHILD_WEIGHT_CFGm , \
                              ES_PIPE1_LLS_L0_CHILD_WEIGHT_CFGm) : \
     (((n) == SOC_TD2_NODE_LVL_L1) ? \
        SOC_TD2_PMEM((u),(p), ES_PIPE0_LLS_L1_CHILD_WEIGHT_CFGm , \
                              ES_PIPE1_LLS_L1_CHILD_WEIGHT_CFGm) : \
      (((n) == SOC_TD2_NODE_LVL_L2) ? \
        SOC_TD2_PMEM((u),(p), ES_PIPE0_LLS_L2_CHILD_WEIGHT_CFGm , \
                              ES_PIPE1_LLS_L2_CHILD_WEIGHT_CFGm) : -1 )))


typedef enum {
    SOC_TD2_SCHED_UNKNOWN   = 0,
    SOC_TD2_SCHED_LLS,
    SOC_TD2_SCHED_HSP
} soc_trident2_sched_type_t;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#define MAX_MMU_OVS_GROUP_COUNT         8
#else
#define MAX_MMU_OVS_GROUP_COUNT         4
#endif /* BCM_TRIDENT2PUS_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
typedef soc_reg_t   (*mmu_ovs_group_wt_select_regs_t);
typedef soc_reg_t   (*pgw_ovs_tdm_regs_t);
typedef const soc_reg_t   (*mmu_ovs_group_regs_t)[MAX_MMU_OVS_GROUP_COUNT];
typedef const soc_reg_t   (*pgw_ovs_spacing_regs_t);
typedef const soc_reg_t   (*pgw_obm_threshold_regs_t);
typedef const soc_field_t (*pgw_tdm_fields_t);
typedef const soc_field_t (*pgw_spacing_fields_t);
#endif /* BCM_TRIDENT2PLUS_SUPPORT */


#if defined BCM_TRIDENT2PLUS_SUPPORT
/* logical to IFP port mapping structure */
typedef struct soc_logical_to_ifp_port_mapping_s {
    int ifp_x_port[_TD2_NUM_LOG_PORTS_PER_DEV];
    int ifp_y_port[_TD2_NUM_LOG_PORTS_PER_DEV];
    char valid_x[_TD2_NUM_LOG_PORTS_PER_DEV];
    char valid_y[_TD2_NUM_LOG_PORTS_PER_DEV];
    int last_valid_x_port;
    int last_valid_y_port;
} soc_logical_to_ifp_port_mapping_t;

extern soc_logical_to_ifp_port_mapping_t soc_td2p_log_to_ifp_port[SOC_MAX_NUM_DEVICES];
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

extern soc_trident2_sched_type_t
_soc_trident2_port_sched_type_get(int unit, soc_port_t port);

extern  int
soc_td2_port_common_attributes_get(int unit, int port, int *pipe,
                                                int *mmu_port, int *phy_port);
extern int
_soc_td2_invalid_parent_index(int unit, int level);

extern int
soc_td2_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                            int *hw_index);

extern int soc_td2_l2_hw_index(int unit, int qnum, int uc);
extern int soc_td2_hw_index_logical_num(int unit, int port,
                                                int index, int uc);
extern int
soc_td2_cosq_set_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int num_spri, int first_sp_child, 
                              int first_sp_mc_child, uint32 ucmap, uint32 spmap);

extern int
soc_td2_cosq_set_sched_mode(int unit, soc_port_t port, int level, int index,
                            soc_td2_sched_mode_e mode, int weight);

extern int
soc_td2_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_psri, int first_sp_child,
                              int first_sp_mc_child, uint32 ucmap, uint32 spmap, 
                              soc_td2_sched_mode_e mode, int weight);

extern int
soc_td2_cosq_get_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int *pnum_spri, int *pfirst_sp_child, 
                              int *pfirst_sp_mc_child, uint32 *pucmap,
                              uint32 *pspmap);

extern int
soc_td2_cosq_get_sched_mode(int unit, soc_port_t port, int level, 
                            int index, soc_td2_sched_mode_e *pmode, int *weight);

extern int
soc_td2_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *num_spri, int *first_sp_child, 
                              int *first_sp_mc_child, uint32 *ucmap,
                              uint32 *pspmap,
                              soc_td2_sched_mode_e *mode, int *weight);

extern int 
soc_td2_get_child_type(int unit, soc_port_t port, int level, int *child_type);

extern int 
soc_td2_sched_weight_get(int unit, int port, int level, int index, int *weight);

extern int 
soc_td2_sched_weight_set(int unit, int port, int level, int index, int weight);

extern int soc_td2_get_pipe_queue_count(int unit, int uc, int xpipe);

extern int soc_td2_dump_port_lls(int unit, int port);

extern int soc_td2_dump_port_hsp(int unit, int port);

extern int soc_td2_lls_init(int unit);

extern int soc_td2_lls_port_uninit(int unit, soc_port_t port);

extern int soc_td2_lls_reset(int unit);

extern int soc_td2_lls_reset_port(int unit, int port);

extern int soc_td2_cosq_set_sched_parent(int unit, soc_port_t port,
                              int level, int hw_index,
                              int parent_hw_idx);

extern int soc_td2_logical_qnum_hw_qnum(int unit, int port, int lqnum, int uc);

extern int soc_td2_mmu_get_shared_size(int unit, int *shared_size);
extern int soc_td2_mmu_config_shared_buf_recalc(int unit,
                                                uint32 spid,
                                                int shared_size,
                                                int flags);
extern int soc_td2_mmu_config_res_limits_update(int unit,
                                                uint32 spid,
                                                uint32 shared_size,
                                                int flags);

extern void soc_td2_process_func_intr(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);

typedef int (*_soc_td2_bst_hw_cb)(int unit);
extern int soc_td2_set_bst_callback(int unit, _soc_td2_bst_hw_cb cb);
typedef int (*soc_bst_intr_check_cb)(int unit);
extern int soc_td2_set_bst_intr_check_callback(int unit, soc_bst_intr_check_cb cb);

extern int soc_td2_port_asf_para_set(int unit, soc_port_t port, int speed,
                                     int enable);
extern int soc_trident2_port_speed_update(int unit, soc_port_t port,
                                          int speed);
extern int soc_td2_port_asf_set(int unit, soc_port_t port,
                                          int speed);
extern int soc_trident2_port_thdo_rx_enable_set(int unit, soc_port_t port,
                                                int enable);

extern int soc_trident2_fc_map_shadow_create(int unit);
extern void soc_trident2_fc_map_shadow_free(int unit);
extern int
soc_trident2_fc_map_shadow_entry_set (int unit, soc_mem_t mem, int index,
                                      void* entry_data);
extern int
soc_trident2_fc_map_shadow_entry_get (int unit, soc_mem_t mem, int index,
                                      void* entry_data);           
extern int soc_trident2_fc_map_shadow_clear (int unit, soc_mem_t mem);
extern int soc_td2_port_traffic_egr_enable_set(int unit, int port, int enable);
extern int soc_td2_port_mmu_tx_enable_set(int unit, int port, int enable);
extern int soc_td2_is_skip_default_lls_creation(int unit);

extern int soc_td2_mmu_rx_enable_set(int unit, soc_port_t port, int enable);

extern int soc_td2_qcn_counter_hw_index_get(int unit, soc_port_t port,
                                            int index, int *qindex);
extern void soc_td2p_subport_init(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_trident2_fc_map_shadow_size_get (int unit, uint32* size);
extern int soc_trident2_fc_map_shadow_sync(int unit, uint32 **sync_addr);
extern int soc_trident2_fc_map_shadow_load(int unit, uint32 **load_addr);
#endif

#if defined(SER_TR_TEST_SUPPORT)
extern soc_error_t soc_td2_ser_inject_error(int unit, uint32 flags,
                                            soc_mem_t mem, int pipe_target,
                                            int block, int index);
extern soc_error_t soc_td2_ser_test_mem(int unit, soc_mem_t mem,
                                        _soc_ser_test_t test_type, int cmd);
extern soc_error_t soc_td2_ser_test(int unit, _soc_ser_test_t test_type);
extern soc_error_t soc_td2_ser_error_injection_support(int unit,
                                                       soc_mem_t mem, int pipe);
extern int soc_td2_ser_test_overlay (int unit, _soc_ser_test_t test_type);
#endif /*#if defined(SER_TR_TEST_SUPPORT)*/
extern int soc_td2_reg_cpu_write_control(int unit, int enable);
extern int soc_td2_is_blk_valid(int unit, int blk);
extern int
soc_trident2_mem_parity_control(int unit, soc_mem_t mem, int copyno, 
                                            int enable);

typedef int (*_soc_td2_lls_traverse_cb)(int unit, int port, int level, 
        int hw_index, void *cookie);
extern int soc_td2_port_lls_traverse(int unit, soc_port_t port, 
        _soc_td2_lls_traverse_cb cb, void *cookie);
int
extern soc_td2_cosq_set_sched_child_config_dynamic(int unit, soc_port_t port,
         int level, int index, 
         int num_spri, int first_child,
         int first_mc_child, uint32 ucmap,
         uint32 spmap, int child_index);

extern int
soc_trident2_port_sched_set(int unit, soc_port_t port);

extern int
soc_td2_obm_ctrl_reg_default_set (int unit, int xlp, int port_index, int obm_inst,
                                  int oversub, int default_flag);
extern int
soc_td2_obm_use_counter_reg_default_set (int unit, int xlp, int port_index,
                                         int obm_inst);
extern int
soc_td2_obm_shared_config_reg_default_set (int unit, int xlp, int port_index,
                                           int obm_inst, int default_flag);
extern int
soc_td2_obm_max_usage_reg_default_set (int unit, int xlp, int port_index,
                                       int obm_inst);
extern int
soc_td2_obm_fc_config_reg_default_set (int unit, int xlp, int port_index,
                                       int obm_inst, int logical_port,
                                       int oversub, int default_flag);
extern int
soc_td2_obm_counters_reset(int unit, soc_port_t port);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
extern mmu_ovs_group_regs_t 
soc_trident2_mmu_ovs_group_regs_get (int unit);

extern mmu_ovs_group_wt_select_regs_t
soc_trident2_mmu_ovs_group_wt_select_regs_get (int unit);

extern pgw_ovs_tdm_regs_t
soc_trident2_pgw_ovs_tdm_regs_get (int unit);

extern pgw_ovs_spacing_regs_t 
soc_trident2_pgw_ovs_spacing_regs_get (int unit);

extern pgw_obm_threshold_regs_t 
soc_trident2_pgw_obm_threshold_regs_get (int unit);

extern pgw_tdm_fields_t 
soc_trident2_pgw_tdm_fields_get (int unit);

extern pgw_spacing_fields_t
soc_trident2_pgw_spacing_fields_get (int unit);

extern int
ser_test_trident2p_mem_index_remap(int unit, ser_test_data_t *test_data,
                                  int *mem_has_ecc);
extern int
soc_trident2p_mem_is_eligible_for_scan(int unit, soc_mem_t mem);
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

extern int
soc_trident2_port_icc_width_set(int unit, soc_port_t port);

extern int
soc_trident2_ser_test_skip_check(int unit, soc_mem_t mem,
                                   soc_acc_type_t acc_type);
extern int
soc_td2x_ser_single_bit_error_enable_set(int unit, int enable);

extern int
soc_td2x_ser_single_bit_error_enable_get(int unit, int *enable);

extern int
soc_td2x_ser_single_bit_report_for_bus_buf_set(int unit, int enable);

extern int
soc_td2p_ledup_port_update(int unit, soc_port_t port, uint8 attach);
#endif	/* !_SOC_TRIDENT2_H_ */
