/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        katana2.h
 */

#ifndef _SOC_KATANA2_H_
#define _SOC_KATANA2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>

typedef struct kt2_pbmp_s {
    soc_pbmp_t *pbmp_gport_stack;
    soc_pbmp_t *pbmp_mxq;
    soc_pbmp_t *pbmp_mxq1g;
    soc_pbmp_t *pbmp_mxq2p5g;
    soc_pbmp_t *pbmp_mxq10g;
    soc_pbmp_t *pbmp_mxq13g;
    soc_pbmp_t *pbmp_mxq21g;
    soc_pbmp_t *pbmp_xport_xe;
    soc_pbmp_t *pbmp_valid;
    soc_pbmp_t *pbmp_pp;
    soc_pbmp_t *pbmp_linkphy;
}kt2_pbmp_t;

typedef enum bcmMxqConnection_s {
    bcmMqxConnectionUniCore=0, 
    bcmMqxConnectionWarpCore=1
}bcmMxqConnection_t;

typedef enum bcmMxqCorePortMode_s {
    bcmMxqCorePortModeSingle=0, 
    bcmMxqCorePortModeDual=1, 
    bcmMxqCorePortModeQuad=2 
}bcmMxqCorePortMode_t;

typedef enum bcmMxqPhyPortMode_s {
    bcmMxqPhyPortModeSingle=0, 
    bcmMxqPhyPortModeDual=1,  /* Suspicious */
    bcmMxqPhyPortModeQuad=2
}bcmMxqPhyPortMode_t;

enum kt2_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    KT2_L2_HASH_KEY_TYPE_BRIDGE = 0,
    KT2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    KT2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    KT2_L2_HASH_KEY_TYPE_VFI = 3,
    KT2_L2_HASH_KEY_TYPE_BFD = 4,
    KT2_L2_HASH_KEY_TYPE_VIF = 5,
    KT2_L2_HASH_KEY_TYPE_PE_VID = 6,
    KT2_L2_HASH_KEY_TYPE_COUNT
};

enum kt2_l3_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    KT2_L3_HASH_KEY_TYPE_V4UC = 0,
    KT2_L3_HASH_KEY_TYPE_V4MC = 1,
    KT2_L3_HASH_KEY_TYPE_V6UC = 2,
    KT2_L3_HASH_KEY_TYPE_V6MC = 3,
    KT2_L3_HASH_KEY_TYPE_LMEP = 4,
    KT2_L3_HASH_KEY_TYPE_RMEP = 5,
/*    KT2_L3_HASH_KEY_TYPE_TRILL = 6, UNSUPPORTED */
    KT2_L3_HASH_KEY_TYPE_COUNT
};

enum kt2_vlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    KT2_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    KT2_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    KT2_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    KT2_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    KT2_VLXLT_HASH_KEY_TYPE_OVID = 4,
    KT2_VLXLT_HASH_KEY_TYPE_IVID = 5,
    KT2_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    KT2_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    KT2_VLXLT_HASH_KEY_TYPE_VIF = 8,
    KT2_VLXLT_HASH_KEY_TYPE_VIF_VLAN = 9,
    KT2_VLXLT_HASH_KEY_TYPE_VIF_CVLAN = 10,
    KT2_VLXLT_HASH_KEY_TYPE_VIF_OTAG = 11,
    KT2_VLXLT_HASH_KEY_TYPE_VIF_ITAG = 12,
    KT2_VLXLT_HASH_KEY_TYPE_LLTAG_VID = 13,
    KT2_VLXLT_HASH_KEY_TYPE_LLVID_IVID = 14,
    KT2_VLXLT_HASH_KEY_TYPE_LLVID_OVID = 15,
    KT2_VLXLT_HASH_KEY_TYPE_COUNT
};

enum kt2_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    KT2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE = 0,
    KT2_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP = 1,
    KT2_EVLXLT_HASH_KEY_TYPE_ISID_XLATE = 3,
    KT2_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE = 4,
    KT2_EVLXLT_HASH_KEY_TYPE_COUNT
};

enum kt2_mpls_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    KT2_MPLS_HASH_KEY_TYPE_MPLS = 0,
    KT2_MPLS_HASH_KEY_TYPE_MIM_NVP = 1,
    KT2_MPLS_HASH_KEY_TYPE_MIM_ISID = 2,
    KT2_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP = 3,
    KT2_MPLS_HASH_KEY_TYPE_COUNT
};

typedef enum bcm_cosq_recalc_type_e {
    BCM_KT2_RECALC_SHARED_INT_DECREMENT,
    BCM_KT2_RECALC_SHARED_INT_INCREMENT,
    BCM_KT2_RECALC_SHARED_EXT_DECREMENT,
    BCM_KT2_RECALC_SHARED_EXT_INCREMENT
} bcm_kt2_cosq_recalc_type_t;

#define KT2_MAX_TDM_FREQUENCY         11 /* 80,110,155,185:80,185:90
                                           205:108 205:88 205:99, 95:16, 205:88*/
#define KT2_MAX_SPEEDS                7 /* 1G,2G,2.5G,10G,13G,20G,21G */

#define KT2_MAX_MXQBLOCKS             11
#define KT2_MAX_MXQPORTS_PER_BLOCK    4
#define KT2_LPBK_PORT                 41
#define KT2_CMIC_PORT                 0
#define KT2_OLP_PORT                  40
#define KT2_IDLE1                     62 /* Consecutive IDLE slots */
#define KT2_IDLE                      63
/* 0:CMIC Port  1-40:Physical Ports  41:Loopback Port */
#define KT2_MAX_LOGICAL_PORTS         42
#define KT2_MAX_PHYSICAL_PORTS        40


#define SOC_KT2_MIN_SUBPORT_INDEX                 42
#define SOC_KT2_MAX_SUBPORT_INDEX                 169
#define SOC_KT2_MAX_SUBPORTS                      128
#define SOC_KT2_MAX_LINKPHY_SUBPORTS_PER_PORT     64
#define SOC_KT2_MAX_TDM_SLOTS_PER_PORT            48
#define SOC_KT2_MAX_STREAMS_PER_SUBPORT           BCM_SUBPORT_CONFIG_MAX_STREAMS

#define SOC_KT2_SUBPORT_GROUP_MAX        (128)
#define SOC_KT2_SUBPORT_PORT_MAX         (128)

#define SOC_KT2_SUBPORT_PP_PORT_INDEX_MIN    (42) 
#define SOC_KT2_SUBPORT_PP_PORT_INDEX_MAX    (\
            SOC_KT2_SUBPORT_PP_PORT_INDEX_MIN + SOC_KT2_SUBPORT_PORT_MAX - 1) 

#define SOC_KT2_LINKPHY_TX_DATA_BUF_START_ADDR_MAX  3399
#define SOC_KT2_LINKPHY_TX_DATA_BUF_END_ADDR_MIN    23
#define SOC_KT2_LINKPHY_TX_DATA_BUF_END_ADDR_MAX    3423

#define SOC_KT2_LINKPHY_BLOCK_MAX                   2
#define SOC_KT2_LINKPHY_BLOCK_PORT_IDX_MAX          4
#define SOC_KT2_MAX_LINKPHY_PORTS                   8

#define SOC_KT2_LINKPHY_TX_STREAM_START_ADDR_OFFSET 0x60

#define KT2_PORT_TYPE_ETHERNET 0
#define KT2_PORT_TYPE_HIGIG    1
#define KT2_PORT_TYPE_LOOPBACK 2
#define KT2_PORT_TYPE_CASCADED 4
#define KT2_PORT_TYPE_GPON_EPON 5
#define KT2_CASCADED_TYPE_LINKPHY 0
#define KT2_CASCADED_TYPE_SUBTAG  1

typedef struct tdm_cycles_info_s {
        uint32 worse_tdm_slot_spacing;
        uint32 optimal_tdm_slot_spacing;
        uint32 min_tdm_cycles;
}tdm_cycles_info_t;

typedef struct  kt2_tdm_pos_info_s {
        uint16 total_slots;
        int16  pos[SOC_KT2_MAX_TDM_SLOTS_PER_PORT];
} kt2_tdm_pos_info_t;

typedef struct bcm56450_tdm_info_s {
        uint8  tdm_freq;
        uint8  tdm_size;
        /* Display purpose only */
        uint8  row;
        uint8  col;
}bcm56450_tdm_info_t;

typedef struct soc_kt2_sku_config_info_s {
    /* Config values */
    int config;
    int bcm56450_cfg_offset;
    int bcm56452_cfg_offset;
    int bcm56454_cfg_offset;
    int bcm56455_cfg_offset;
    int bcm56248_cfg_offset;
    int olp_port;
    int xfi_0;
    int xfi_1;
} soc_kt2_sku_config_info_t;

extern int flex_io_operation_busy[SOC_MAX_NUM_DEVICES];

typedef uint32 mxqspeeds_t[KT2_MAX_MXQBLOCKS][KT2_MAX_MXQPORTS_PER_BLOCK];
typedef uint32 kt2_mxqblock_ports_t[KT2_MAX_MXQBLOCKS]
                                   [KT2_MAX_MXQPORTS_PER_BLOCK];

typedef uint32 kt2_speed_t[KT2_MAX_PHYSICAL_PORTS];
typedef uint32 kt2_port_to_mxqblock_t[KT2_MAX_PHYSICAL_PORTS];
typedef uint32 kt2_port_to_mxqblock_subports_t[KT2_MAX_PHYSICAL_PORTS];


extern uint32 kt2_current_tdm[256];
extern uint32 kt2_current_tdm_size;

extern kt2_port_to_mxqblock_subports_t 
       *kt2_port_to_mxqblock_subports[SOC_MAX_NUM_DEVICES];
extern kt2_tdm_pos_info_t kt2_tdm_pos_info[KT2_MAX_MXQBLOCKS];
extern soc_error_t soc_katana2_reconfigure_tdm(int unit,uint32 new_tdm_size,
                                               uint32 *new_tdm);

extern kt2_mxqblock_ports_t *kt2_mxqblock_ports[SOC_MAX_NUM_DEVICES];
extern mxqspeeds_t *mxqspeeds[SOC_MAX_NUM_DEVICES];
extern uint8 kt2_tdm_update_flag;
extern tdm_cycles_info_t kt2_current_tdm_cycles_info[KT2_MAX_SPEEDS];


extern int _soc_kt2_config_get(int unit, soc_kt2_sku_config_info_t *conf);
extern int soc_flex_io_operation_status_set(int unit, int value);
extern int soc_kt2_vlan_xlate_base_entry_to_key(int unit, void *entry, uint8 *key);
extern int soc_kt2_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key);
extern int soc_kt2_l3x_base_entry_to_key(int unit, uint32 *entry, uint8 *key);
extern int soc_kt2_egr_vlan_xlate_base_entry_to_key(int unit, void *entry, uint8 *key);
extern int soc_kt2_mpls_base_entry_to_key(int unit, void *entry, uint8 *key);
extern uint32 soc_kt2_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, void *base_entry, uint8 *key);
extern uint32 soc_kt2_egr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, void *base_entry, uint8 *key);

extern soc_functions_t soc_katana2_drv_funs;

extern int soc_katana2_pipe_mem_clear(int unit);
extern int soc_katana2_linkphy_mem_clear(int unit);


extern int soc_kt2_ser_mem_clear(int unit, soc_mem_t mem);
extern void soc_kt2_ser_fail(int unit);

extern int soc_kt2_mem_config(int unit, int dev_id);
extern int soc_kt2_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);
extern int soc_kt2_show_ring_osc(int unit);
extern int soc_kt2_show_material_process(int unit);
extern int soc_kt2_show_voltage(int unit);

extern int soc_kt2_linkphy_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_kt2_linkphy_port_blk_idx_get(
    int unit, int port, int *block, int *index);
extern int soc_kt2_linkphy_get_portid(int unit, int block, int index);
extern void soc_katana2_pbmp_init(int unit, kt2_pbmp_t kt2_pbmp);
extern void soc_katana2_subport_init(int unit);
extern soc_error_t 
       soc_katana2_tdm_feasibility_check(int unit, soc_port_t port, int speed);
extern soc_error_t
       soc_katana2_update_tdm(int unit, soc_port_t port, int speed);
extern soc_error_t 
       soc_katana2_get_core_port_mode( 
       int unit,soc_port_t port,bcmMxqCorePortMode_t *mode);
extern soc_error_t 
       soc_katana2_get_phy_port_mode(
       int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode);
extern soc_error_t soc_katana2_get_port_mxqblock(
       int unit, soc_port_t port,uint8 *mxqblock);
extern soc_error_t soc_katana2_get_phy_connection_mode(
       int unit,soc_port_t port,int mxqblock, bcmMxqConnection_t *connection);
extern soc_error_t soc_katana2_port_lanes_get(
       int unit, soc_port_t port, int *lanes);
extern soc_error_t soc_katana2_port_lanes_set(
       int unit, soc_port_t port, int lanes);
extern void _katana2_phy_addr_default(
       int unit, int port, uint16 *phy_addr, uint16 *phy_addr_int);
extern soc_error_t soc_katana2_port_enable_set(
       int unit, soc_port_t port, int enable);
extern void soc_katana2_mxqblock_reset(int unit, uint8 mxqblock,int active_low);
extern int _soc_katana2_mmu_reconfigure(int unit, int port);
extern int soc_katana2_num_cosq_init(int unit);
extern void soc_kt2_blk_counter_config(int unit);
extern soc_error_t katana2_get_wc_phy_info(int unit,
                                           soc_port_t port,
                                           uint8 *lane_num,
                                           uint8 *phy_mode,
                                           uint8 *chip_num);

/* KT2 OAM */
typedef int (*soc_kt2_oam_handler_t)(int unit, soc_field_t fault_field);
typedef int (*soc_kt2_linkphy_handler_t) (int unit, soc_port_t port, uint32 val);
typedef int (*soc_kt2_oam_ser_handler_t)(int unit, soc_mem_t mem, int index);
extern void soc_kt2_oam_handler_register(int unit, soc_kt2_oam_handler_t handler);
extern void soc_kt2_linkphy_handler_register(int unit, soc_kt2_linkphy_handler_t handler);
extern void soc_kt2_rxlp_interrupt_process(int unit, int port, uint32 rval);

extern void soc_kt2_parity_error(void *unit_vp, void *d1, void *d2,
                         void *d3, void *d4);
extern void soc_kt2_oam_ser_handler_register(int unit, 
                                soc_kt2_oam_ser_handler_t handler);
extern int soc_kt2_oam_ser_process(int unit, soc_mem_t mem, int index);

/* KT2 cosq soc functions */
extern int soc_kt2_cosq_stream_mapping_set(int unit);
extern int soc_kt2_cosq_port_coe_linkphy_status_set(int unit, soc_port_t port,
                                                    int enable);
extern int soc_kt2_cosq_s1_range_set(int unit, soc_port_t port,
                                     int start_s1, int end_s1, int type);
extern int soc_kt2_cosq_repl_map_set(int unit, soc_port_t port,
                                     int start, int end, int enable);
extern int soc_kt2_cosq_lp_repl_map_set(int unit, int stream, uint32 pp_port,
                                        int enable);
extern int soc_kt2_cosq_s0_sched_set(int unit, int hw_index, int mode);
/* KT2 TDM related functions */
extern void kt2_tdm_display(int unit, uint32 *tdm, uint32 total_tdm_slots,
                            uint32 row, uint32 col);
extern void kt2_tdm_replace(uint32 *tdm, uint32 total_tdm_slots,
                            uint32  src, uint32 dst,uint32 one_time);
extern void kt2_tdm_swap(uint32 *tdm, uint32 total_tdm_slots,
                        uint32  src, uint32 dst);


/**********************************************************************
 * MMU, COSQ related prototypes.
 */
typedef enum {
    SOC_KT2_SCHED_MODE_UNKNOWN = 0,
    SOC_KT2_SCHED_MODE_STRICT,
    SOC_KT2_SCHED_MODE_WRR,
    SOC_KT2_SCHED_MODE_WDRR
} soc_kt2_sched_mode_e;

typedef enum {
    SOC_KT2_NODE_LVL_ROOT = 0,
    SOC_KT2_NODE_LVL_L0,
    SOC_KT2_NODE_LVL_L1,
    SOC_KT2_NODE_LVL_L2,
    SOC_KT2_NODE_LVL_MAX
} soc_kt2_node_lvl_e;

typedef enum {
    _SOC_KT2_COSQ_NODE_LVL_ROOT = 0,
    _SOC_KT2_COSQ_NODE_LVL_S0,
    _SOC_KT2_COSQ_NODE_LVL_S1,
    _SOC_KT2_COSQ_NODE_LVL_L0,
    _SOC_KT2_COSQ_NODE_LVL_L1,
    _SOC_KT2_COSQ_NODE_LVL_L2,
    _SOC_KT2_COSQ_NODE_LVL_MAX
} soc_kt2_cosq_node_lvl_e;

typedef enum soc_kt2_drop_limit_alpha_value_e {
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */                                          
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_KT2_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_kt2_drop_limit_alpha_value_t;

/* Worst case time = 256k * 768 * 8 @ 1Gbps = 1.6 sec */
#define _SOC_COSQ_QUEUE_FLUSH_TIMEOUT_PACKING_DEFAULT 5900000

/* Worst case time = 256k * 2880 *8 @ 1Gbps = 5.9 sec */
#define _SOC_COSQ_QUEUE_FLUSH_TIMEOUT_DEFAULT 1600000
#define _SOC_KT2_NODE_CONFIG_MEM(n)                                 \
    (((n)==SOC_KT2_NODE_LVL_ROOT) ? LLS_PORT_CONFIGm :              \
     (((n) == SOC_KT2_NODE_LVL_L0) ? LLS_L0_CONFIGm :               \
      ((n) == SOC_KT2_NODE_LVL_L1 ? LLS_L1_CONFIGm :  -1  )))
#define _SOC_KT2_NODE_CONFIG_LINKPHY_MEM(n)                                 \
    (((n)==_SOC_KT2_COSQ_NODE_LVL_ROOT) ? LLS_PORT_CONFIGm :              \
     (((n) == _SOC_KT2_COSQ_NODE_LVL_S0) ? LLS_L0_CONFIGm :               \
     (((n) == _SOC_KT2_COSQ_NODE_LVL_S1) ? LLS_S1_CONFIGm :               \
     (((n) == _SOC_KT2_COSQ_NODE_LVL_L0) ? LLS_L0_CONFIGm :               \
      ((n) == _SOC_KT2_COSQ_NODE_LVL_L1 ? LLS_L1_CONFIGm :  -1  )))))


#define _SOC_KT2_NODE_PARENT_MEM(n)                                 \
    (((n)==SOC_KT2_NODE_LVL_L2) ? LLS_L2_PARENTm :                  \
     (((n) == SOC_KT2_NODE_LVL_L1) ? LLS_L1_PARENTm :               \
      (((n) == SOC_KT2_NODE_LVL_L0) ? LLS_L0_PARENTm :  -1  )))

#define _SOC_KT2_NODE_WIEGHT_MEM(n)                                 \
    (((n)==SOC_KT2_NODE_LVL_L0) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm  : \
     (((n) == SOC_KT2_NODE_LVL_L1) ? LLS_L1_CHILD_WEIGHT_CFG_CNTm : \
      (((n) == SOC_KT2_NODE_LVL_L2) ?  LLS_L2_CHILD_WEIGHT_CFG_CNTm : -1 )))

#define _SOC_KT2_NODE_WIEGHT_LINKPHY_MEM(n)                                 \
     (((n)==_SOC_KT2_COSQ_NODE_LVL_S0) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm  : \
     (((n)==_SOC_KT2_COSQ_NODE_LVL_S1) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm  : \
     (((n)==_SOC_KT2_COSQ_NODE_LVL_L0) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm  : \
     (((n) == _SOC_KT2_COSQ_NODE_LVL_L1) ? LLS_L1_CHILD_WEIGHT_CFG_CNTm : \
      (((n) == _SOC_KT2_COSQ_NODE_LVL_L2) ?  LLS_L2_CHILD_WEIGHT_CFG_CNTm : -1 )))))

#define _SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, port)  \
        (SOC_PBMP_MEMBER(SOC_INFO(unit).linkphy_pp_port_pbm, port) || \
         SOC_PBMP_MEMBER(SOC_INFO(unit).subtag_pp_port_pbm, port))

#define _SOC_IS_PORT_LINKPHY_SUBTAG(unit, port)  \
        (SOC_PBMP_MEMBER(SOC_INFO(unit).linkphy_allowed_pbm, port) || \
         SOC_PBMP_MEMBER(SOC_INFO(unit).subtag_allowed_pbm, port))

extern int soc_kt2_dump_port_lls(int unit, int port, int subport);
extern int _soc_katana2_get_cfg_num(int unit, int *new_cfg_num);
extern int _soc_katana2_flexio_scache_allocate(int unit);
extern int _soc_katana2_flexio_scache_retrieve(int unit);
extern int _soc_katana2_flexio_scache_sync(int unit);
extern int soc_kt2_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_kt2_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_kt2_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem);
extern int _soc_katana2_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable);
#if defined(SER_TR_TEST_SUPPORT)
extern int soc_katana2_ser_inject_error(int unit, uint32 flags, soc_mem_t mem, int pipe_target, int block, int index);
extern int _soc_katana2_mem_nack_error_test(int unit, _soc_ser_test_t test_type, int *test_errors);
extern int soc_katana2_ser_mem_test(int unit, soc_mem_t mem, _soc_ser_test_t test_type, int cmd);
extern int soc_katana2_ser_test(int unit, _soc_ser_test_t test_type);
extern soc_error_t soc_katana2_ser_error_injection_support(int unit, soc_mem_t mem, int pipe);
#endif
extern int soc_kt2_lls_bmap_alloc(int unit);
extern int soc_kt2_cosq_max_bucket_set(int unit, int port,
                                   int index, uint32 level);
extern int soc_kt2_cosq_min_bucket_set(int unit, int port,
                                   int index, uint32 level);
extern int soc_kt2_cosq_min_clear(int unit, int port,
                                   int index, uint32 level);
extern int
_soc_kt2_cosq_min_bucket_get(int unit, int port, int index, int lvl,
                             uint32 *min, uint32 *matissa,
                             uint32 *exp, uint32 *cycle);
extern int _soc_kt2_cosq_begin_port_flush(int unit, int port,
                                         int hw_index);
extern int _soc_kt2_cosq_end_port_flush(int unit,
                                        int hw_index);
extern int soc_kt2_cosq_queue_enable(int unit,
                                        int hw_index, int enable);
extern int
soc_kt2_mmu_config_shared_buf_recalc(int unit, int delta_size, bcm_kt2_cosq_recalc_type_t recalc_type);

extern int soc_kt2_mmu_get_shared_size(int unit, int *shared_size, int *shared_size_external);
extern int
soc_kt2_sched_get_node_config(int unit, soc_port_t port, int level, int index,
                              int *pnum_spri, int *pfirst_child,
                              int *pfirst_mc_child, uint32 *pucmap,
                              uint32 *pspmap);
extern int
soc_kt2_cosq_get_sched_mode(int unit, soc_port_t port, int level, int index,
                              soc_kt2_sched_mode_e *pmode, int *weight);
extern int 
soc_kt2_cosq_s0_sched_get(int unit, int hw_index, soc_kt2_sched_mode_e *mode);
extern int
soc_kt2_cosq_two_s1s_in_s0_config_set(int unit, soc_port_t port, int enable);
extern uint32
soc_kt2_cosq_two_s1s_in_s0_config_get(int unit, soc_port_t port);
extern void soc_kt2_xport_type_update(int unit, soc_port_t port, int mode);
extern int
_bcm_kt2_is_pp_port_linkphy_subtag(int unit, int pp_port);
extern int
_soc_katana2_perq_flex_counters_init(int unit, uint32 drop_mask);
extern int
_soc_kt2_dfc_status_clear(int unit, int port, int stream_id);
extern int
soc_kt2_get_max_buffer_size(int unit, int external, int value);
#endif    /* !_SOC_KATANA2_H_ */
