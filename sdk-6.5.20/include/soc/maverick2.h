/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        maverick2.h
 */

#ifndef _SOC_MAVERICK2_H_
#define _SOC_MAVERICK2_H_

#include <soc/drv.h>
#include <soc/trident3.h>
#include <soc/maverick2_tdm.h>

/* **************** TDM-related Macors and structs adopted from Tomahawk ---start************ */
/* Device port */
/* 64 General device port + 1 CPU/Mng + 1 Loopback*/
#define _MV2_TDM_GDEV_PORTS_PER_PIPE        64
#define _MV2_TDM_DEV_PORTS_PER_PIPE         (_MV2_TDM_GDEV_PORTS_PER_PIPE + 2)
#define _MV2_TDM_DEV_PORTS_PER_DEV          \
    (_MV2_TDM_DEV_PORTS_PER_PIPE * MAVERICK2_TDM_PIPES_PER_DEV)
/* MMU port */
#define _MV2_TDM_MMU_PORTS_OFFSET_PER_PIPE  64
#define _MV2_TDM_MMU_PORTS_PER_DEV          (64 * 1)
/* ------------------------------------------------- */
#define _MV2_TDM_LENGTH 512
#define _MV2_PKT_SCH_LENGTH 200
#define _MV2_TDM_SPEED_CLASS_MAX                6

typedef struct _soc_maverick2_tdm_pblk_info_s {
	int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
}_soc_maverick2_tdm_pblk_info_t;

typedef struct _soc_maverick2_tdm_pipe_s {
    int idb_tdm[_MV2_TDM_LENGTH];
    int mmu_tdm[_MV2_TDM_LENGTH];
    int ovs_tdm[MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE][MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE][MAVERICK2_TDM_OVS_GROUP_LENGTH];
    int pkt_shaper_tdm[MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE][_MV2_PKT_SCH_LENGTH];
} _soc_maverick2_tdm_pipe_t;

typedef struct _soc_maverick2_tdm_temp_s {
    _soc_maverick2_tdm_pipe_t tdm_pipe[MAVERICK2_TDM_PIPES_PER_DEV];
    _soc_maverick2_tdm_pblk_info_t pblk_info[MAVERICK2_TDM_PHY_PORTS_PER_DEV];
    int port_ratio[MAVERICK2_TDM_PBLKS_PER_DEV];
    int ovs_ratio_x1000[MAVERICK2_TDM_PIPES_PER_DEV][MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE];

} _soc_maverick2_tdm_temp_t;

/* **************** TDM-related Macors and structs adopted from Tomahawk ---end************ */




/********************** SER begin ******************************/
/* 2 dedicated banks for L2
 * 2 dedicated banks for L3
 * 8 banks for UFT
 * 8 banks for IUAT (4 16k banks + 4 8k banks)
 * 4 banks for EUAT (4 8k banks)
 *
 * For L2, since there is only single wide view, the LP is only available for single wide view.
 * For L3 and UFT/UAT banks, since they can be assigned to any logical table which supports different views,
 * LP is available for different views - single, double and quad.
 */

#define SOC_MV2_NUM_BANKS_L2                     2
#define SOC_MV2_NUM_BANKS_L3                     2
#define SOC_MV2_NUM_BANKS_UAT_16K                4

/* Dedicated L2 LP entries per bank */
#define SOC_MV2_NUM_ENTRIES_L2_LP_BANK           4096 /* 1W */
/* Dedicated L3 LP entries per bank */
#define SOC_MV2_NUM_ENTRIES_L3_LP_BANK           3584 /* 2048(1W) + 1024(2W) + 512(4W) */

#define SOC_MV2_NUM_ENTRIES_UAT_16K_LP_BANK      6144 /* 4096(1W) + 2048(2W) */
#define SOC_MV2_NUM_ENTRIES_UAT_8K_LP_BANK       3072 /* 2048(1W) + 1024(2W) */

/* UFT LP entries per bank */
#define SOC_MV2_NUM_ENTRIES_UFT_LP_BANK          14336 /* 8192(1W) + 4096(2W) + 2048(4W) */

extern int soc_mv2_ser_enable_all(int unit, int enable);
extern void soc_mv2_ser_error(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4);
extern int soc_mv2_mem_ser_control(int unit, soc_mem_t mem,
                                   int copyno, int enable);
extern void soc_mv2_ser_register(int unit);

extern void soc_mv2_ser_test_register(int unit);

extern int soc_mv2_ser_lp_mem_info_get(int unit, soc_mem_t mem, int index,
                                       soc_mem_t *hash_mem, int *hash_index);

/********************* SER end *********************************/




extern int soc_mv2_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
                                    int thdo_db_shd, int thdo_qe_shd,
                                    int post_update);

#define CCLK_FREQ_1700MHZ    1700
#define CCLK_FREQ_1525MHZ    1525
#define CCLK_FREQ_1012MHZ    1012

extern soc_functions_t soc_maverick2_drv_funs;
extern int soc_maverick2_bond_info_init(int unit);
extern int soc_maverick2_mem_config(int unit);
extern int soc_maverick2_num_cosq_init(int unit);
extern int soc_maverick2_num_cosq_init_port(int unit, int port);
extern int soc_maverick2_port_config_init(int unit, uint16 dev_id);
extern int soc_maverick2_chip_reset(int unit);
extern int soc_maverick2_port_reset(int unit);

extern int soc_mv2_port_lanes_set(int unit, int phy_port,
                                  int *lanes, int *port_bandwidth);

enum soc_mv2_port_ratio_e {
    SOC_MV2_PORT_RATIO_SINGLE,
    SOC_MV2_PORT_RATIO_DUAL_1_1,
    SOC_MV2_PORT_RATIO_DUAL_2_1,
    SOC_MV2_PORT_RATIO_DUAL_1_2,
    SOC_MV2_PORT_RATIO_TRI_023_2_1_1,
    SOC_MV2_PORT_RATIO_TRI_023_4_1_1,
    SOC_MV2_PORT_RATIO_TRI_012_1_1_2,
    SOC_MV2_PORT_RATIO_TRI_012_1_1_4,
    SOC_MV2_PORT_RATIO_QUAD,
    SOC_MV2_PORT_RATIO_COUNT
};

extern int soc_mv2_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_mv2_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);

#define _MV2_PORTS_PER_PBLK             4
#define _MV2_PBLKS_PER_PIPE             20
#define _MV2_PIPES_PER_DEV              1
#define _MV2_XPES_PER_DEV               1

#define _MV2_PBLKS_PER_DEV(unit)        (_MV2_PBLKS_PER_PIPE * NUM_PIPE(unit))

#define _MV2_PORTS_PER_PIPE             (_MV2_PORTS_PER_PBLK * _MV2_PBLKS_PER_PIPE)
#define _MV2_PORTS_PER_DEV(unit)        (_MV2_PORTS_PER_PIPE * NUM_PIPE(unit))

#define _MV2_SECTION_SIZE                32768

#define _MV2_MMU_MAX_PACKET_BYTES        9416  /* bytes */
#define _MV2_MMU_PACKET_HEADER_BYTES     64    /* bytes */
#define _MV2_MMU_JUMBO_FRAME_BYTES       9216  /* bytes */
#define _MV2_MMU_DEFAULT_MTU_BYTES       1536  /* bytes */

#define _MV2_MMU_PHYSICAL_CELLS_PER_XPE  131072 /* Total Physical cells per XPE */
#define _MV2_MMU_TOTAL_CELLS_PER_XPE     129024 /* 31.5MB/XPE = 129024 cells/XPE */
#define _MV2_MMU_RSVD_CELLS_CFAP_PER_XPE 556    /* Reserved CFAP cells per XPE */
#define _MV2_MMU_CFAP_BANK_FULL_LIMIT    2047   /* CFAP Bank Full limit */

#define _MV2_MMU_TOTAL_CELLS             (_MV2_MMU_TOTAL_CELLS_PER_XPE * _MV2_XPES_PER_DEV)
#define _MV2_MMU_PHYSICAL_CELLS          (_MV2_MMU_PHYSICAL_CELLS_PER_XPE * _MV2_XPES_PER_DEV)

#define _MV2_MMU_BYTES_PER_CELL          256   /* bytes (2048 bits) */
#define _MV2_MMU_NUM_PG                  8
#define _MV2_MMU_NUM_POOL                4
#define _MV2_MMU_NUM_RQE_QUEUES          11
#define _MV2_MMU_NUM_INT_PRI             16

#define SOC_MV2_MMU_CFG_QGROUP_MAX       34

#define SOC_MV2_MMU_PORT_STRIDE          0x7f  /* MMU Port number space per pipe */

#define SOC_MV2_NUM_CPU_QUEUES           48  /* CPU port has 48 MC Queues */
#define SOC_MV2_NUM_UC_QUEUES_PER_PIPE   650 /* (64 General Ports + 1 Management Port) * 10 */
#define SOC_MV2_NUM_MC_QUEUES_PER_PIPE   698 /* (64 General Ports + 1 CPU Port/Management Port +*/
                                             /* 1 Loop-back Port) * 10 */
#define SOC_MV2_CPU_MCQ_BASE             650 /* CPU port MC queue base index */
#define SOC_MV2_LB_MCQ_BASE              640 /* LB ports MC queue base index */
#define SOC_MV2_MGMT_UCQ_BASE            630 /* Mgmt ports UC queue base index */
#define SOC_MV2_MGMT_MCQ_BASE            630 /* Mgmt ports MC queue base index */

/* Number of Unicast Queues per Port */
#define SOC_MV2_NUM_UCAST_QUEUE_PER_PORT 10
/* Number of Multicast Queues per Port */
#define SOC_MV2_NUM_MCAST_QUEUE_PER_PORT 10
/* Number of Scheduler Nodes(L0) per Port */
#define SOC_MV2_NUM_SCHEDULER_PER_PORT   10

#define SOC_MV2_MMU_MCQ_ENTRY_PER_XPE    24576
#define SOC_MV2_MMU_RQE_ENTRY_PER_XPE    2048
#define _MV2_MMU_RQE_ENTRY_RSVD_PER_XPE  242   /* Reserved RQE QE per XPE */

#define _MV2_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_MV2_MMU_MCQ_ENTRY_PER_XPE
#define _MV2_MMU_TOTAL_RQE_ENTRY(unit)   SOC_MV2_MMU_RQE_ENTRY_PER_XPE

#define _MV2_MMU_CFAP_BANK_FULL_LIMIT    2047 /* CFAP Bank Full limit */
#define _MV2_TDM_MCQE_RSVD_OVERSHOOT     12   /* Reserved MC egress queue entry
                                                 for TDM overshoot */
#define _MV2_MCQE_RSVD_PER_MCQ           6    /* Reserved MC egress queue entry
                                                 per MC queue */

typedef struct _soc_mv2_mmu_sw_config_info_s {
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
} _soc_mv2_mmu_sw_config_info_t;

extern void soc_maverick2_sbus_ring_map_config(int unit);

/************ FlexPort Start ********************/
#define MV2_TDM_SPEED_ADJUST(speed)                     \
        SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed) < 10000 ? 10000 : \
        SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_mv2_flexport_scache_allocate(int unit);
extern int soc_mv2_flexport_scache_sync(int unit);
extern int soc_mv2_flexport_scache_recovery(int unit);
extern int soc_mv2_tdm_scache_allocate(int unit);
extern int soc_mv2_tdm_scache_sync(int unit);
extern int soc_mv2_tdm_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int soc_mv2_mmu_config_init_port(int unit, int port);
/************ FlexPort End ********************/

#if !defined(BCM_MV2_ASF_EXCLUDE)
/*
  =============================
   MV2 ASF (cut-thru forwarding)
  =============================
*/
#define OUT
#define IN_OUT

/* ASF Modes */
typedef enum {
    _SOC_MV2_ASF_MODE_SAF          = 0,  /* store-and-forward */
    _SOC_MV2_ASF_MODE_SAME_SPEED   = 1,  /* same speed CT  */
    _SOC_MV2_ASF_MODE_SLOW_TO_FAST = 2,  /* slow to fast CT   */
    _SOC_MV2_ASF_MODE_FAST_TO_SLOW = 3,  /* fast to slow CT   */
    _SOC_MV2_ASF_MODE_CFG_UPDATE   = 4,  /* internal cfg updates */
    _SOC_MV2_ASF_MODE_INIT         = 5,  /* init */
    _SOC_MV2_ASF_MODE_UNSUPPORTED  = 6
} soc_mv2_asf_mode_e;

/* ASF MMU Cell Buffer Allocation Profiles */
typedef enum {
    _SOC_MV2_ASF_MEM_PROFILE_NONE      = 0,  /* No cut-through support */
    _SOC_MV2_ASF_MEM_PROFILE_SIMILAR   = 1,  /* Similar Speed Alloc Profile  */
    _SOC_MV2_ASF_MEM_PROFILE_EXTREME   = 2   /* Extreme Speed Alloc Profile */
} soc_mv2_asf_mem_profile_e;

/* Latency modes */
typedef enum {
    _SOC_MV2_L2_LATENCY   = 0,
    _SOC_MV2_L3_LATENCY   = 1,
    _SOC_MV2_FULL_LATENCY = 2
} _soc_mv2_latency_mode_e;

extern int
soc_mv2_asf_init_start(int unit);

extern int
soc_mv2_asf_init_done(int unit);

extern int
soc_mv2_port_asf_init(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_mv2_asf_mode_e  mode);

extern int
soc_mv2_asf_deinit(int unit);

extern int
soc_mv2_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_mv2_asf_mode_e   mode,
    uint8                extra_cells);

extern int
soc_mv2_port_asf_mode_get(
    int                            unit,
    soc_port_t                     port,
    int                            port_speed,
    OUT soc_mv2_asf_mode_e* const  mode);

extern int
soc_mv2_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_mv2_asf_mode_e  mode);

extern int
soc_mv2_port_asf_params_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_mv2_asf_mode_e  mode,
    uint8               bubble_mop_disable,
    uint8               ca_thresh,
    int                 speed_encoding);

extern int
soc_mv2_port_asf_speed_to_mmu_cell_credit(
    int        unit,
    soc_port_t port,
    int        port_speed,
    OUT uint32 *mmu_cell_credit);

extern int
soc_mv2_port_asf_mmu_cell_credit_to_speed(
    int        unit,
    soc_port_t port,
    uint8      mmu_cell_credit,
    OUT int    *port_speed);

extern pbmp_t *
soc_mv2_port_asf_pause_bpmp_get(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
soc_mv2_asf_wb_memsz_get(
    int unit,
    OUT uint32* const wb_mem_sz,
    uint16            scache_ver);

extern int
soc_mv2_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data);

extern int
soc_mv2_asf_wb_recover(
    int unit,
    uint8* const wb_data,
    uint16       scache_ver);
#endif

extern int
soc_mv2_asf_pbmp_get(int unit);

extern int
soc_mv2_port_asf_show(
    int          unit,
    soc_port_t   port,
    int          port_speed);

extern int
soc_mv2_asf_config_dump(int unit);

extern int
soc_mv2_port_asf_config_dump(
	int          unit,
	soc_port_t   port,
	int          port_speed);

extern int
soc_mv2_port_asf_detach(
    int          unit,
    soc_port_t   port);
#endif

extern int
soc_mv2_lpm_flex_ip4entry0_to_1(
    int u,
    void *src,
    void *dst,
    uint32 key_type,
    int copy_hit);

extern int
soc_mv2_lpm_flex_prefix_length_get(
    int u,
    void *entry,
    int *ipv6,
    int *vrf_id,
    int *pfx,
    uint32 key_type);
extern int soc_mv2_post_mmu_init_update(int unit);
#endif /* !_SOC_MAVERICK2_H_ */
