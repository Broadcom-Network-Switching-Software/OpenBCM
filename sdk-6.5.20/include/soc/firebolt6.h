/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt6.h
 */

#ifndef _SOC_FIREBOLT6_H_
#define _SOC_FIREBOLT6_H_

#include <soc/drv.h>
#include <soc/trident3.h>
#include <soc/firebolt6_tdm.h>

#define SOC_FB6_PORT_BLOCK_BASE_PORT(port)                              \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

/* ****** TDM-related Macors and structs ******/

/* Device port */
/* 69 General device port + 1 CPU + 1 Loopback */
#define _FB6_TDM_GDEV_PORTS_PER_PIPE        69
#define _FB6_TDM_DEV_PORTS_PER_PIPE         (_FB6_TDM_GDEV_PORTS_PER_PIPE + 2)
#define _FB6_TDM_DEV_PORTS_PER_DEV          \
    (_FB6_TDM_DEV_PORTS_PER_PIPE * FIREBOLT6_TDM_PIPES_PER_DEV)
/* MMU port */
#define _FB6_TDM_MMU_PORTS_OFFSET_PER_PIPE  69
#define _FB6_TDM_MMU_PORTS_PER_DEV          (69 * 1)
/* MMU channelized port. */
#define _FB6_MMU_REPL_PORTS_PER_DEV         (456)
/* ------------------------------------------------- */
#define _FB6_TDM_LENGTH 512
#define _FB6_PKT_SCH_LENGTH 160
#define _FB6_TDM_SPEED_CLASS_MAX                6

#define FB6_NUM_PORT                   (72)
#define FB6_NUM_MMU_PORT               (71)
#define FB6_MGMT_PHY_PORT_START        (65)
#define FB6_MGMT_PHY_PORT_END          (68)
#define FB6_NUM_PHY_PORT               (68)
#define FB6_MAX_PORT_MAP               (5)
#define FB6_NUM_PORT_PER_LC            (48)
#define FB6_NUM_LC                     (8)
#define FB6_TOTAL_NUM_PORT             (FB6_NUM_PORT)
#define FB6_TOTAL_NUM_SUBPORT          (512)
#define FB6_PORTS_PER_TSC              (4)

/* Speed limit for each PM Blocks. */
extern int bcmi_otp_pm_max_speed[SOC_MAX_NUM_DEVICES][FIREBOLT6_TDM_PBLKS_PER_DEV];

/* Flexport TDM Info. */
typedef struct _soc_firebolt6_tdm_pblk_info_s {
    int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
}_soc_firebolt6_tdm_pblk_info_t;


typedef struct _soc_firebolt6_tdm_pipe_s {
    int idb_tdm[_FB6_TDM_LENGTH];
    int mmu_tdm[_FB6_TDM_LENGTH];
    int ovs_tdm[FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE][FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE][FIREBOLT6_TDM_OVS_GROUP_LENGTH];
    int pkt_shaper_tdm[FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE][_FB6_PKT_SCH_LENGTH];
} _soc_firebolt6_tdm_pipe_t;

typedef struct _soc_firebolt6_tdm_temp_s {
    _soc_firebolt6_tdm_pipe_t tdm_pipe[FIREBOLT6_TDM_PIPES_PER_DEV];
    _soc_firebolt6_tdm_pblk_info_t pblk_info[FIREBOLT6_TDM_PHY_PORTS_PER_DEV];
    int port_ratio[FIREBOLT6_TDM_PBLKS_PER_DEV];
    int ovs_ratio_x1000[FIREBOLT6_TDM_PIPES_PER_DEV][FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE];

} _soc_firebolt6_tdm_temp_t;

/* UTT info */
#define FB6_DEFIP_MAX_TCAMS            (64)
#define FB6_UTT_TCAM_DEPTH             (512)
#define FB6_UTT_MAX_TCAMS              (96)

#define FB6_UTT_IFP_ID                 (0)
#define FB6_UTT_IFP_MIN_TCAMS          (24)
#define FB6_UTT_IFP_MAX_TCAMS          (72)
#define FB6_UTT_IFP_MAX_TCAMS_PER_LKUP (4)
#define FB6_UTT_IFP_TCAM_START         (0)
#define FB6_UTT_IFP_LKUP_BASE          (0)

#define FB6_UTT_LPM_ID                 (1)
#define FB6_UTT_LPM_MIN_TCAMS          (24)
#define FB6_UTT_LPM_MAX_TCAMS          (64)
#define FB6_UTT_LPM_MAX_TCAMS_PER_LKUP (16)
#define FB6_UTT_LPM_LKUP_BASE          (18)

#ifdef BCM_UTT_SUPPORT
extern int soc_fb6_utt_init(int unit);
#endif

/********************** SER begin ******************************/

/* 2 dedicated banks for L2
 * 2 dedicated banks for L3
 * 8 banks for UFT
 * 8 banks for IUAT
 * 4 banks for EUAT
 *
 * For L2, since there is only single wide view, the LP is only available for single wide view.
 * For L3 and UFT/UAT banks, since they can be assigned to any logical table which supports different views,
 * LP is available for different views - single, double and quad.
 */

/* Dedicated L2 and L3 entries - 16k per bank */
#define SOC_FB6_NUM_ENTRIES_L2                   32768
#define SOC_FB6_NUM_ENTRIES_L3                   32768
#define SOC_FB6_NUM_BANKS_L2                     2
#define SOC_FB6_NUM_BANKS_L3                     2

/* Dedicated L2 LP entries per bank */
#define SOC_FB6_NUM_ENTRIES_L2_LP_BANK           4096 /* 1W */
/* Dedicated L3 LP entries per bank */
#define SOC_FB6_NUM_ENTRIES_L3_LP_BANK           7168 /* 4096(1W) + 2048(2W) + 1024(4W) */

/* Same for ingress and egress UAT */
#define SOC_FB6_NUM_ENTRIES_UAT_LP_BANK          3072 /* 2048(1W) + 1024(2W) */

/* UFT entries per bank */
#define SOC_FB6_NUM_ENTRIES_UFT_BANK             32768
/* UFT LP entries per bank */
#define SOC_FB6_NUM_ENTRIES_UFT_LP_BANK          14336 /* 8192(1W) + 4096(2W) + 2048(4W) */

extern int
soc_firebolt6_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                                _soc_ser_sram_info_t *sram_info);

extern int soc_fb6_ser_enable_all(int unit, int enable);
extern void soc_fb6_ser_error(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int soc_fb6_mem_ser_control(int unit, soc_mem_t mem,
                                   int copyno, int enable);
extern void soc_fb6_ser_register(int unit);

extern void soc_fb6_ser_test_register(int unit);

extern int soc_fb6_ser_lp_mem_info_get(int unit, soc_mem_t mem, int index,
                                       soc_mem_t *hash_mem, int *hash_index);
/********************* SER end *********************************/

/* SUBPORT */
#define FB6_SUBPORT_COE_GROUP_MAX             (512)
#define FB6_SUBPORT_COE_PORT_MAX              (512)
#define FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX  (48)
#define FB6_SUBPORT_COE_PORT_PER_MOD_MAX      (128)
#define FB6_SUBPORT_COE_MODULES_MAX           (5)

typedef struct _soc_fb6_mmu_sw_config_info_s {
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
} _soc_fb6_mmu_sw_config_info_t;


extern int 
soc_firebolt6_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array,
    int *temperature_count);

extern void
soc_firebolt6_temperature_intr(void *unit_vp, void *d1, void *d2,
    void *d3, void *d4);

extern soc_functions_t soc_firebolt6_drv_funs;
extern int soc_firebolt6_mem_config(int unit);
extern int soc_firebolt6_num_cosq_init_port(int unit, int port);
extern int soc_firebolt6_num_cosq_init(int unit);
extern int soc_fb6_port_config_init(int unit, uint16 dev_id);
extern int soc_firebolt6_chip_reset(int unit);
extern int soc_firebolt6_port_reset(int unit);

extern int soc_fb6_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
                                     int thdo_db_shd, int thdo_qe_shd,
                                     int post_update);

extern int soc_fb6_port_cosq_get(int unit, int index, int *gport, int *cosq, int mc);
extern int soc_fb6_mmu_config_port_init(int unit, int port);
extern int soc_fb6_num_cosq_port_init(int unit, int port);
extern int _soc_fb6_mmu_port_l0_mapping_set(int unit, bcm_port_t port);
/* Subport based Methods. */
extern int soc_fb6_subport_init(int unit);

extern int soc_fb6_port_obm_info_get(int unit, soc_port_t port,
                                          int *obm_id, int *lane);
extern int soc_firebolt6_bond_info_init(int unit);
extern int soc_fb6_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_fb6_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);

#define _FB6_PORTS_PER_PBLK             4
#define _FB6_PBLKS_PER_PIPE             17
#define _FB6_PIPES_PER_DEV              1
#define _FB6_XPES_PER_DEV               1
#define _FB6_OBMS_PER_PIPE              17

#define _FB6_PBLKS_PER_DEV(unit)        (_FB6_PBLKS_PER_PIPE * NUM_PIPE(unit))

#define _FB6_PORTS_PER_PIPE             (_FB6_PORTS_PER_PBLK * _FB6_PBLKS_PER_PIPE)
#define _FB6_PORTS_PER_DEV(unit)        (_FB6_PORTS_PER_PIPE * NUM_PIPE(unit))

enum soc_fb6_port_ratio_e {
    SOC_FB6_PORT_RATIO_SINGLE,
    SOC_FB6_PORT_RATIO_DUAL_1_1,
    SOC_FB6_PORT_RATIO_DUAL_2_1,
    SOC_FB6_PORT_RATIO_DUAL_1_2,
    SOC_FB6_PORT_RATIO_TRI_023_2_1_1,
    SOC_FB6_PORT_RATIO_TRI_023_4_1_1,
    SOC_FB6_PORT_RATIO_TRI_012_1_1_2,
    SOC_FB6_PORT_RATIO_TRI_012_1_1_4,
    SOC_FB6_PORT_RATIO_QUAD,
    SOC_FB6_PORT_RATIO_COUNT
};

typedef enum {
    _SOC_FB6_INDEX_STYLE_BUCKET,
    _SOC_FB6_INDEX_STYLE_BUCKET_MODE,
    _SOC_FB6_INDEX_STYLE_WRED,
    _SOC_FB6_INDEX_STYLE_WRED_DROP_THRESH,
    _SOC_FB6_INDEX_STYLE_SCHEDULER,
    _SOC_FB6_INDEX_STYLE_PERQ_XMT,
    _SOC_FB6_INDEX_STYLE_UCAST_DROP,
    _SOC_FB6_INDEX_STYLE_UCAST_QUEUE,
    _SOC_FB6_INDEX_STYLE_MCAST_QUEUE,
    _SOC_FB6_INDEX_STYLE_EXT_UCAST_QUEUE,
    _SOC_FB6_INDEX_STYLE_EGR_POOL,
    _SOC_FB6_INDEX_STYLE_COUNT
} soc_fb6_index_style_t;

enum soc_fb6_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_FB6_PORT_MODE_QUAD = 0,
    SOC_FB6_PORT_MODE_TRI_012 = 1,
    SOC_FB6_PORT_MODE_TRI_023 = 2,
    SOC_FB6_PORT_MODE_DUAL = 3,
    SOC_FB6_PORT_MODE_SINGLE = 4
};

typedef enum soc_fb6_drop_limit_alpha_value_e {
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128, /* Use 1/128 as the alpha value for
                                            dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                          dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                          dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                          dynamic threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                         threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                         threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                         threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                         threshold */
    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_fb6_drop_limit_alpha_value_t;



#define _FB6_MMU_MAX_PACKET_BYTES        12288  /* bytes */
#define _FB6_MMU_PACKET_HEADER_BYTES     64     /* bytes */
#define _FB6_MMU_JUMBO_FRAME_BYTES       12288  /* bytes */
#define _FB6_MMU_DEFAULT_MTU_BYTES       1536   /* bytes */

#define _FB6_MMU_PHYSICAL_CELLS_PER_XPE  132*1024  /* Total Physical cells per XPE */
#define _FB6_MMU_TOTAL_CELLS_PER_XPE     130*1024  /* 130K cells */
#define _FB6_MMU_RSVD_CELLS_CFAP_PER_XPE 211       /* Reserved CFAP cells per XPE */
#define _FB6_MMU_CFAP_BANK_FULL_LIMIT    2047      /* CFAP Bank Full limit */

#define _FB6_MMU_TOTAL_CELLS             (_FB6_MMU_TOTAL_CELLS_PER_XPE * _FB6_XPES_PER_DEV)
#define _FB6_MMU_PHYSICAL_CELLS          (_FB6_MMU_PHYSICAL_CELLS_PER_XPE * _FB6_XPES_PER_DEV)

#define _FB6_MMU_BYTES_PER_CELL          256   /* bytes (2048 bits) */
#define _FB6_MMU_NUM_PG                  8
#define _FB6_MMU_NUM_POOL                4
#define _FB6_MMU_NUM_RQE_QUEUES          11
#define _FB6_MMU_NUM_LBK_QUEUES          10
#define _FB6_MMU_NUM_INT_PRI             16

#define SOC_FB6_MMU_CFG_QGROUP_MAX       34

#define SOC_FB6_MMU_PORT_STRIDE          0x7f     /* MMU Port number space per pipe */

#define SOC_FB6_NUM_CPU_QUEUES           48          /* CPU port has 48 MC Queues */
#define SOC_FB6_NUM_UC_QUEUES_PER_PIPE   3680
#define SOC_FB6_NUM_MC_QUEUES_PER_PIPE   3690
                                                     /* 1 Loop-back Port) * 10 */
#define _BCM_FB6_MMU_L3_MC_CPU_QBASE     2570
#define _BCM_FB6_MMU_L3_MC_LB_QBASE      2560

#define SOC_FB6_CPU_MCQ_BASE             3690        /* CPU port MC queue base index */
#define SOC_FB6_LB_MCQ_BASE              3680        /* LB ports MC queue base index */



#define SOC_FB6_MMU_MCQ_ENTRY_PER_XPE    32768 /* C108  */
#define SOC_FB6_MMU_RQE_ENTRY_PER_XPE    4096
#define _FB6_MMU_RQE_ENTRY_RSVD_PER_XPE  242   /* Reserved RQE QE per XPE */
#define _FB6_MAX_MMU_MCQE_PER_XPE        1338  /* MAX MMU MC QE */

#define _FB6_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_FB6_MMU_MCQ_ENTRY_PER_XPE
#define _FB6_MMU_TOTAL_RQE_ENTRY(unit)   SOC_FB6_MMU_RQE_ENTRY_PER_XPE

#define _FB6_TDM_MCQE_RSVD_OVERSHOOT     12   /* Reserved MC egress queue entry
                                                 for TDM overshoot */
#define _FB6_MCQE_RSVD_PER_MCQ           6    /* Reserved MC egress queue entry */


#define SOC_FB6_MMU_NUM_QUEUES_PER_PORT              8
#define SOC_FB6_MMU_NUM_QUEUES_PER_ST_PORT           18
#define SOC_FB6_MMU_NUM_QUEUES_PER_HG_PORT           32


#define SOC_FB6_MMU_NUM_QUEUES_PER_ST_PORT           18
#define SOC_FB6_MMU_NUM_QUEUES_PER_HG_PORT           32
#define SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT           11
#define SOC_FB6_NUM_CPU_QUEUES                       48          /* CPU port has 48 MC Queues */



/* Number of Unicast Queues per Downlink Port queues 0 - 7 */
#define SOC_FB6_NUM_UCAST_QUEUE_PER_PORT 8
/* Number of Multicast Queues per Downlink  Port queues 0 - 7 */
#define SOC_FB6_NUM_MCAST_QUEUE_PER_PORT 8
/* Number of Scheduler Nodes per Downlink Port */
#define SOC_FB6_NUM_SCHEDULER_PER_PORT   9

/* Number of Unicast Queues per Stacking Port queues 0 - 17 */
#define SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT 18
/* Number of Multicast Queues per HG/Stacking  Port queues 0 - 17 */
#define SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT 18
/* Number of Scheduler Nodes per HG/Stacking Port */
#define SOC_FB6_NUM_SCHEDULER_PER_ST_PORT   21

/* Number of Unicast Queues per Stacking Port queues 0 - 32 */
#define SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT 32
/* Number of Multicast Queues per HG/Stacking  Port queues 0 - 32 */
#define SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT 32
/* Number of Scheduler Nodes per HG/Stacking Port */
#define SOC_FB6_NUM_SCHEDULER_PER_HG_PORT   36

#define SOC_FB6_CPU_MCQ_BASE                         3690        /* CPU port MC queue base index */

#define SOC_FB6_MAX_MMU_PORT_NUM                     48
#define SOC_FB6_HG_MAX_MMU_PORT_NUM                  56
#define SOC_FB6_ST_MAX_MMU_PORT_NUM                  64

/* Per pipe WRED config memory base index */
#define _FB6_WRED_PER_PIPE_UC_QUEUES_BASE  0
#define _FB6_WRED_PER_PIPE_PORT_SP_BASE    3680 
#define _FB6_WRED_PER_PIPE_GLOBAL_SP_BASE  3960  /* (4 global service pools) */

#define CCLK_FREQ_1250MHZ    1250
#define CCLK_FREQ_1118MHZ    1118
#define CCLK_FREQ_1012MHZ    1012


typedef enum {
    SOC_FB6_SCHED_MODE_UNKNOWN = 0,
    SOC_FB6_SCHED_MODE_STRICT,
    SOC_FB6_SCHED_MODE_WRR,
    SOC_FB6_SCHED_MODE_WERR
} soc_fb6_sched_mode_e;

typedef enum {
    SOC_FB6_NODE_LVL_ROOT = 0,
    SOC_FB6_NODE_LVL_L0,
    SOC_FB6_NODE_LVL_L1,
    SOC_FB6_NODE_LVL_L2,
    SOC_FB6_NODE_LVL_MAX
} soc_fb6_node_lvl_e;

extern int
_soc_fb6_port_base_l0_mapping_get(int unit, int port, int *base_l0, int *num_l0);

/* Counter begin */
extern int
soc_counter_firebolt6_non_dma_init(int unit, int nports,
                                   int non_dma_start_index, int *non_dma_entries);
extern int
soc_counter_firebolt6_get_info(int unit, soc_port_t port, soc_reg_t id,
                               int *base_index, int *num_entries);
extern int
soc_counter_firebolt6_generic_get_info(int unit,  soc_ctr_control_info_t ctrl_info,
                                       soc_reg_t id, int *base_index, int *num_entries);

/* Counter end */

extern int soc_fb6_show_subport_map(int unit);

extern int _soc_fb6_mmu_idb_ports_assign(int unit);

extern int _soc_fb6_mem_skip(int unit, soc_mem_t mem);

extern int _soc_fb6_reg_skip(int unit, soc_reg_t reg);

extern int soc_firebolt6_show_voltage(int unit);

extern int
soc_fb6_port_oversub_get(int unit, int phy_port, int logical_port);

extern int
_soc_fb6_port_speed_validate(int unit, bcm_port_t lp, bcm_port_t pp, int l, int e, int s);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_fb6_flexport_scache_allocate(int unit);
extern int soc_fb6_flexport_scache_sync(int unit);
extern int soc_fb6_flexport_scache_recovery(int unit);
extern int soc_fb6_tdm_scache_allocate(int unit);
extern int soc_fb6_tdm_scache_sync(int unit);
extern int soc_fb6_tdm_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern void soc_firebolt6_sbus_ring_map_config(int unit);

extern int
soc_firebolt6_port_mode_get(int unit, int logical_port, int *mode);

extern int
soc_fb6_support_speeds(int unit, int lanes, uint32 *speed_mask);

#ifdef INCLUDE_AVS
extern soc_error_t
soc_fb6_avs_init(int unit);
#endif /* INCLUDE_AVS */

#endif /* !_SOC_FIREBOLT6_H_ */
