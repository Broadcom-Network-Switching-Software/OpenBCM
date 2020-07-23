/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane4.h
 */

#ifndef _SOC_HURRICANE4_H_
#define _SOC_HURRICANE4_H_

#include <soc/drv.h>
#include <soc/helix5.h>
#include <soc/hurricane4_tdm.h>

#define SOC_HR4_PORT_BLOCK_BASE_PORT(port)                             \
    ((SOC_INFO(unit).port_l2p_mapping[port] >= 57) ?                   \
     (57 + ((SOC_INFO(unit).port_l2p_mapping[port] - 57) & ~0x3)) :    \
     (SOC_INFO(unit).port_l2p_mapping[port] >= 25) ?                   \
     (25 + ((SOC_INFO(unit).port_l2p_mapping[port] - 25) & ~0xF)) :    \
     (SOC_INFO(unit).port_l2p_mapping[port] >= 1) ?                    \
     (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3)) : -1)

#define HR4_PHY_PORT_SERDERS(phy_port)                                 \
        ((phy_port <= 24) ? ((phy_port - 1) / 12) :                    \
         (phy_port <= 56) ?                                            \
         (2 + ((phy_port - 25) / _HR4_PORTS_PER_PMQ_PBLK)) :           \
         (4 + ((phy_port - 57) / _HR4_PORTS_PER_PBLK)))

/* **************** TDM-related Macors and structs adopted from Tomahawk ---start************ */
/* Device port */
/* 64 General device port + 1 CPU + 1 Loopback  + 1 FAE + 4 MACSEC*/
#define _HR4_TDM_GDEV_PORTS_PER_PIPE        64
#define _HR4_TDM_DEV_PORTS_PER_PIPE         (_HR4_TDM_GDEV_PORTS_PER_PIPE + 7)
#define _HR4_TDM_DEV_PORTS_PER_DEV          \
    (_HR4_TDM_DEV_PORTS_PER_PIPE * HURRICANE4_TDM_PIPES_PER_DEV)
/* MMU port */
#define _HR4_TDM_MMU_PORTS_OFFSET_PER_PIPE  72
#define _HR4_TDM_MMU_PORTS_PER_DEV          (72 * 1)
/* ------------------------------------------------- */
#define _HR4_TDM_LENGTH 512
#define _HR4_PKT_SCH_LENGTH 160
#define _HR4_TDM_SPEED_CLASS_MAX                5

#define HR4_CLK_495MHZ 495
#define HR4_CLK_496MHZ 496
#define HR4_CLK_445MHZ 445
#define HR4_CLK_446MHZ 446
#define HR4_CLK_415MHZ 415
#define HR4_CLK_325MHZ 325
#define HR4_CLK_245MHZ 245
#define HR4_CLK_265MHZ 265
#define HR4_CLK_266MHZ 266
#define HR4_CLK_246MHZ 246
#define HR4_CLK_135MHZ 135
#define HR4_CLK_200MHZ 200
#define HR4_CLK_100MHZ 100
#define HR4_CLK_66MHZ   66
#define HR4_CLK_DEFAULT HR4_CLK_496MHZ

typedef struct _soc_hurricane4_tdm_pblk_info_s {
	int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
}_soc_hurricane4_tdm_pblk_info_t;

typedef struct _soc_hurricane4_tdm_pipe_s {
    int idb_tdm[_HR4_TDM_LENGTH];
    int mmu_tdm[_HR4_TDM_LENGTH];
    int ovs_tdm[HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE][HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE][HURRICANE4_TDM_OVS_GROUP_LENGTH];
    int pkt_shaper_tdm[HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE][_HR4_PKT_SCH_LENGTH];
} _soc_hurricane4_tdm_pipe_t;

typedef struct _soc_hurricane4_tdm_temp_s {
    _soc_hurricane4_tdm_pipe_t tdm_pipe[HURRICANE4_TDM_PIPES_PER_DEV];
    _soc_hurricane4_tdm_pblk_info_t pblk_info[HURRICANE4_TDM_PHY_PORTS_PER_DEV];
    int port_ratio[HURRICANE4_TDM_PBLKS_PER_DEV];
    int ovs_ratio_x1000[HURRICANE4_TDM_PIPES_PER_DEV][HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE];
} _soc_hurricane4_tdm_temp_t;


/* **************** TDM-related Macors and structs adopted from Tomahawk ---end************ */


/********************** SER begin ******************************/

/* 2 dedicated banks for L2
 * 2 dedicated banks for L3
 * 8 banks for IUAT
 * 4 banks for EUAT
 *
 * For L2, since there is only single wide view, the LP is only available for single wide view.
 * For L3 and UAT banks, since they can be assigned to any logical table which supports different views,
 * LP is available for different views - single, double and quad.
 */

/* Dedicated L2 entries - 32k per bank */
#define SOC_HR4_NUM_ENTRIES_L2                   65536
#define SOC_HR4_NUM_BANKS_L2                     2
/* Dedicated L2 LP entries per bank */
#define SOC_HR4_NUM_ENTRIES_L2_LP_BANK           8192 /* 1W */

/* Dedicated L3 entries - 16k per bank */
#define SOC_HR4_NUM_ENTRIES_L3                   32768
#define SOC_HR4_NUM_BANKS_L3                     2
/* Dedicated L3 LP entries per bank */
#define SOC_HR4_NUM_ENTRIES_L3_LP_BANK           7168 /* 4096(1W) + 2048(2W) + 1024(4W) */

/* Same for ingress and egress UAT */
#define SOC_HR4_NUM_ENTRIES_UAT_LP_BANK          192  /* 128(1W) + 64(2W) */

extern int
soc_hurricane4_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                             _soc_ser_sram_info_t *sram_info);

extern int soc_hr4_ser_enable_all(int unit, int enable);
extern void soc_hr4_ser_error(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int soc_hr4_mem_ser_control(int unit, soc_mem_t mem,
                                   int copyno, int enable);
extern void soc_hr4_ser_register(int unit);

extern void soc_hr4_ser_test_register(int unit);

extern int soc_hr4_ser_lp_mem_info_get(int unit, soc_mem_t mem, int index,
                                       soc_mem_t *hash_mem, int *hash_index);
/********************* SER end *********************************/


extern int soc_hr4_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
                                    int thdo_db_shd, int thdo_qe_shd,
                                    int post_update);

extern soc_functions_t soc_hurricane4_drv_funs;
extern int soc_hurricane4_mem_config(int unit);
extern int soc_hurricane4_num_cosq_init(int unit);
extern int soc_hurricane4_num_cosq_init_port(int unit, int port);

extern int soc_hurricane4_port_config_init(int unit, uint16 dev_id);
extern int soc_hurricane4_chip_reset(int unit);
extern int soc_hurricane4_port_reset(int unit);
extern uint32 _soc_hr4_piped_mem_index(int unit, soc_port_t port,
                                      soc_mem_t mem, int arr_off);

extern int soc_counter_hurricane4_non_dma_init(int unit, int nports,
                                    int non_dma_start_index,
                                    int *non_dma_entries);
extern int soc_counter_hurricane4_get_info(int unit, soc_port_t port,
                                soc_reg_t id, int *base_index,
                                int *num_entries);
extern int soc_counter_hurricane4_generic_get_info(int unit, soc_ctr_control_info_t ctrl_info,
                                        soc_reg_t id, int *base_index, int *num_entries);

typedef int (*_soc_hr4_bst_hw_cb)(int unit);
extern int  _bcm_hr4_pfc_reg_index_get(int unit, soc_port_t port, soc_port_t *mport, int *index);


enum soc_hr4_port_ratio_e {
    SOC_HR4_PORT_RATIO_SINGLE,
    SOC_HR4_PORT_RATIO_DUAL_1_1,
    SOC_HR4_PORT_RATIO_DUAL_2_1,
    SOC_HR4_PORT_RATIO_DUAL_1_2,
    SOC_HR4_PORT_RATIO_TRI_023_2_1_1,
    SOC_HR4_PORT_RATIO_TRI_023_4_1_1,
    SOC_HR4_PORT_RATIO_TRI_012_1_1_2,
    SOC_HR4_PORT_RATIO_TRI_012_1_1_4,
    SOC_HR4_PORT_RATIO_QUAD,
    SOC_HR4_PORT_RATIO_QSGMII0,
    SOC_HR4_PORT_RATIO_QSGMII1,
    SOC_HR4_PORT_RATIO_COUNT
};

/***********************************************************************************/
extern int soc_hurricane4_bond_info_init(int unit);
extern int soc_hr4_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_hr4_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern soc_error_t
soc_hurricane4_idb_buf_reset(int unit, soc_port_t port, int reset);

typedef enum {
    SOC_HR4_SCHED_MODE_UNKNOWN = 0,
    SOC_HR4_SCHED_MODE_STRICT,
    SOC_HR4_SCHED_MODE_WRR,
    SOC_HR4_SCHED_MODE_WERR
} soc_hr4_sched_mode_e;

typedef enum {
    SOC_HR4_NODE_LVL_ROOT = 0,
    SOC_HR4_NODE_LVL_L0,
    SOC_HR4_NODE_LVL_L1,
    SOC_HR4_NODE_LVL_L2,
    SOC_HR4_NODE_LVL_L3,
    SOC_HR4_NODE_LVL_MAX
} soc_hr4_node_lvl_e;

typedef enum {
    _SOC_HR4_INDEX_STYLE_BUCKET,
    _SOC_HR4_INDEX_STYLE_BUCKET_MODE,
    _SOC_HR4_INDEX_STYLE_WRED,
    _SOC_HR4_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_HR4_INDEX_STYLE_SCHEDULER,
    _SOC_HR4_INDEX_STYLE_PERQ_XMT,
    _SOC_HR4_INDEX_STYLE_UCAST_DROP,
    _SOC_HR4_INDEX_STYLE_UCAST_QUEUE,
    _SOC_HR4_INDEX_STYLE_MCAST_QUEUE,
    _SOC_HR4_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_HR4_INDEX_STYLE_EGR_POOL,
    _SOC_HR4_INDEX_STYLE_COUNT
} soc_hr4_index_style_t;

typedef enum soc_hr4_drop_limit_alpha_value_e {
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_HR4_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_hr4_drop_limit_alpha_value_t;


/* Following Macro would be used by *port_config_init  and needs to be re-verified*/
#define _HR4_PORTS_PER_PBLK             4
#define _HR4_PORTS_PER_PMQ_PBLK         16
#define _HR4_PBLKS_PER_PIPE             16
#define _HR4_PIPES_PER_DEV              1
#define _HR4_XPES_PER_DEV               1

#define _HR4_PBLKS_PER_DEV(unit)        (_HR4_PBLKS_PER_PIPE * NUM_PIPE(unit))

#define _HR4_PORTS_PER_PIPE             (_HR4_PORTS_PER_PBLK * _HR4_PBLKS_PER_PIPE)
#define _HR4_PORTS_PER_DEV(unit)        (_HR4_PORTS_PER_PIPE * NUM_PIPE(unit))


#define _HR4_MMU_MAX_PACKET_BYTES        9416  /* bytes */
#define _HR4_MMU_PACKET_HEADER_BYTES     64    /* bytes */
#define _HR4_MMU_JUMBO_FRAME_BYTES       9216  /* bytes */
#define _HR4_MMU_DEFAULT_MTU_BYTES       1536  /* bytes */

#define _HR4_MMU_PHYSICAL_CELLS_PER_XPE  18 * 1024 /* Total Physical cells per XPE */
#define _HR4_MMU_TOTAL_CELLS_PER_XPE     16384 /* 32MBE */
#define _HR4_MMU_RSVD_CELLS_CFAP_PER_XPE 211    /* Reserved CFAP cells per XPE */
#define _HR4_MMU_CFAP_BANK_FULL_LIMIT    1023   /* CFAP Bank Full limit */

#define _HR4_MMU_TOTAL_CELLS             (_HR4_MMU_TOTAL_CELLS_PER_XPE * _HR4_XPES_PER_DEV)
#define _HR4_MMU_PHYSICAL_CELLS          (_HR4_MMU_PHYSICAL_CELLS_PER_XPE * _HR4_XPES_PER_DEV)

#define _HR4_MMU_BYTES_PER_CELL          256   /* bytes (2048 bits) */
#define _HR4_MMU_NUM_PG                  8
#define _HR4_MMU_NUM_POOL                4
#define _HR4_MMU_NUM_RQE_QUEUES          11
#define _HR4_MMU_NUM_LBK_QUEUES          10
#define _HR4_MMU_NUM_INT_PRI             16


/* Nuber of queue group should be same as number of ports */ 
#define SOC_HR4_MMU_CFG_QGROUP_MAX       34

#define SOC_HR4_MMU_PORT_STRIDE          0x7f     /* MMU Port number space per pipe */

#define SOC_HR4_NUM_CPU_QUEUES           48          /* CPU port has 48 MC Queues */
#define SOC_HR4_NUM_UC_QUEUES_PER_PIPE   1152        /* (62 Downlink Ports * 16  + (4 Uplink + 4 Stacking) * 26) */
#define SOC_HR4_NUM_MC_QUEUES_PER_PIPE   1210        /* (62 Downlink Ports * 16  + (4 Uplink + 4 Stacking) * 26)  + 1 CPU Port +*/
#define SOC_HR4_NUM_MAX_UC_QUEUES_PER_PIPE   1280    /* (62 Downlink Ports * 16  + (4 Uplink + 4 Stacking) * 26) */
                                                     /* 1 Loop-back Port) * 10 */
#define _BCM_HR4_MMU_L3_MC_CPU_QBASE     2570
#define _BCM_HR4_MMU_L3_MC_LB_QBASE      2560

#define SOC_HR4_CPU_MCQ_BASE             1290        /* CPU port MC queue base index */
#define SOC_HR4_LB_MCQ_BASE              1280        /* LB ports MC queue base index */
#define SOC_HR4_DOWNLINK_PORTQ_BASE      (26 * 16)   /* same as helix5 */

#define SOC_HR4_NUM_MCAST_QUEUE_PER_LB_PORT 10
#define SOC_HR4_NUM_MACSEC_UC_QUEUE         32       /* 8 queues per macsec port * 4 macsec port */
#define SOC_HR4_NUM_MACSEC_MC_QUEUE         32       /* 8 queues per macsec port * 4 macsec port */
/* Number of Unicast Queues per Downlink Port
 Queues 0 - 7 for wireless traffic and 8 - 15 for wired traffic*/
#define SOC_HR4_NUM_UCAST_QUEUE_PER_PORT 16
/* Number of Multicast Queues per Downlink  Port
 Queues 0 - 7 for wireless traffic and 8 - 15 for wired traffic*/
#define SOC_HR4_NUM_MCAST_QUEUE_PER_PORT 16
/* Number of Scheduler Nodes per Downlink Port */
#define SOC_HR4_NUM_SCHEDULER_PER_PORT   19
/* Number of Unicast Queues per HG/Stacking Port
 Queues 0 - 7 for transit  traffic and 8 - 15 for wireless traffic, 16 - 23 is for wired traffic */
#define SOC_HR4_NUM_UCAST_QUEUE_PER_ST_PORT 26
/* Number of Multicast Queues per HG/Stacking  Port
 Queues 0 - 7 for transit  traffic and 8 - 15 for wireless traffic, 16 - 23 is for wired traffic */
#define SOC_HR4_NUM_MCAST_QUEUE_PER_ST_PORT 26
/* Number of Scheduler Nodes per HG/Stacking Port */
#define SOC_HR4_NUM_SCHEDULER_PER_ST_PORT   33
#define SOC_HR4_NUM_SCHEDULER_PER_CPU_PORT  12

#define SOC_HR4_MMU_MCQ_ENTRY_PER_XPE    12288
#define SOC_HR4_MMU_RQE_ENTRY_PER_XPE    2048
#define _HR4_MMU_RQE_ENTRY_RSVD_PER_XPE  242   /* Reserved RQE QE per XPE */
#define _HR4_MAX_MMU_MCQE_PER_XPE        532  /* MAX MMU MC QE */

#define _HR4_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_HR4_MMU_MCQ_ENTRY_PER_XPE
#define _HR4_MMU_TOTAL_RQE_ENTRY(unit)   SOC_HR4_MMU_RQE_ENTRY_PER_XPE

#define _HR4_TDM_MCQE_RSVD_OVERSHOOT     12   /* Reserved MC egress queue entry
                                                 for TDM overshoot */
#define _HR4_MCQE_RSVD_PER_MCQ           6    /* Reserved MC egress queue entry
                                                 per MC queue */
/* MMU pktstats control flags */
#define _HR4_MMU_PSTATS_ENABLE          0x0001 /* Packetized/OOB Stat mode is enabled */
#define _HR4_MMU_PSTATS_PKT_MOD         0x0002 /* Packetized mode is enabled. OOB Stat mode is disabled */
#define _HR4_MMU_PSTATS_HWM_MOD         0x0004 /* Report High Watermark values */
#define _HR4_MMU_PSTATS_RESET_ON_READ   0x0008 /* Enable Hardware clear-on read */
#define _HR4_MMU_PSTATS_MOR_EN          0x0010 /* Enable MOR capability */
#define _HR4_MMU_PSTATS_SYNC            0x8000 /* Read from device else the buffer */

/* Per pipe WRED config memory base index */
#define _HR4_WRED_PER_PIPE_UC_QUEUES_BASE  0
#define _HR4_WRED_PER_PIPE_PORT_SP_BASE    1280  /* (16 * 26) UPlink + (54 * 16) Dw link */
#define _HR4_WRED_PER_PIPE_GLOBAL_SP_BASE  1560  /* (4 global service pools) */

#define _HR4_IS_NON_FP_PORT(_p_)            (_p_ > 64)
#define _HR4_PG_HEADROOM_FOR_MACSEC_PORTS   (4 * 117)

extern void soc_hurricane4_mmu_pstats_tbl_config(int unit);
extern int  soc_hurricane4_mmu_pstats_mode_set(int unit, uint32 flags);
extern int  soc_hurricane4_mmu_pstats_mode_get(int unit, uint32 *flags);
extern int  soc_hurricane4_mmu_pstats_mor_enable(int unit);
extern int  soc_hurricane4_mmu_pstats_mor_disable(int unit);

typedef struct _soc_hr4_mmu_sw_config_info_s {
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
} _soc_hr4_mmu_sw_config_info_t;

#define SOC_HR4_STAT_ING_FLEX_POOL_MAX   20

/* Num of evict memories is 26, 1 is added for reserved mem */
#define SOC_HR4_NUM_EVICT_MEM            27

/* SUBPORT */
#define HR4_SUBPORT_COE_GROUP_MAX             (512)
#define HR4_SUBPORT_COE_PORT_MAX              (512)
#define HR4_MAX_SUBPORT_COE_PORT_PER_CASPORT  (80)
#define HR4_MAX_SUBPORT_COE_PORT_PER_MOD      (128)
#define HR4_MAX_COE_MODULES                   (5)

extern int
soc_hurricane4_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count);
extern void
soc_hurricane4_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int
soc_hurricane4_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout);

extern void soc_hurricane4_port_qsgmii_mode_get(int unit, soc_port_t port, int* qsgmii_mode);
extern void soc_hurricane4_sbus_ring_map_config(int unit);

/************ FlexPort Start ********************/
extern int soc_hr4_mmu_config_init_port(int unit, int port);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_hr4_flexport_scache_allocate(int unit);
extern int soc_hr4_flexport_scache_sync(int unit);
extern int soc_hr4_flexport_scache_recovery(int unit);
extern int soc_hr4_tdm_scache_allocate(int unit);
extern int soc_hr4_tdm_scache_sync(int unit);
extern int soc_hr4_tdm_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
soc_hr4_support_speeds(int unit, int port, uint32 *speed_mask);

/************ FlexPort End ********************/

extern int
soc_hr4_lpm_flex_ip4entry0_to_1(
    int u,
    void *src,
    void *dst,
    uint32 key_type,
    int copy_hit);

extern int
soc_hr4_lpm_flex_prefix_length_get(
    int u,
    void *entry,
    int *ipv6,
    int *vrf_id,
    int *pfx,
    uint32 key_type);

extern int soc_hr4_reg_blk_index_get(int unit, soc_reg_t reg, soc_port_t port,
                                          pbmp_t *bm, int *block, int *index, int invalid_blk_check);
extern int
soc_hr4_mpls_hash_control_set(int *hash_entry);

extern int
soc_hurricane4_show_voltage(int unit);
/*************************************************************************************************/
extern int
_soc_hr4_dual_parser_parity_bit_enable(int unit, soc_mem_t mem, int enable);

#ifdef BCM_UTT_SUPPORT
extern int soc_hr4_utt_init(int unit);
#endif /* BCM_UTT_SUPPORT */

int _soc_hr4_mem_skip(int unit, soc_mem_t mem);

int _soc_hr4_reg_skip(int unit, soc_reg_t reg);

#define HR4_WAR_L2X_SOBMH_KEY_TYPE           15
#define HR4_WAR_L2X_SOBMH_DATA_TYPE          16
#define HR4_WAR_VXLT2_SOBMH_KEY_TYPE         22
#define HR4_WAR_VXLT2_SOBMH_DATA_TYPE        21
#define HR4_WAR_L2X_SOBMH_EDIT_CTRL_ID       0x70
int soc_hr4_sobmh_war_l2_vxlate_entry_update(int unit, int modid, int old_modid);

#endif /* _SOC_HURRICANE4_H_ */
