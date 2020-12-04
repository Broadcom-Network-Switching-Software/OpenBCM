/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk2.h
 */

#ifndef _SOC_TOMAHAWK2_H_
#define _SOC_TOMAHAWK2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/mmu_config.h>
#include <soc/esw/port.h>

extern int soc_th2_post_mmu_init_update(int unit);
extern int soc_th2_vfp_ser_correction(int unit, soc_mem_t mem, int index,
                                      int copyno, int entry_dw);
extern int soc_tomahawk2_port_config_init(int unit, uint16 dev_id);
extern int _soc_tomahawk2_port_mapping_init(int unit);
extern void _soc_th2_mmu_init_dev_config(int unit,
                                         _soc_mmu_device_info_t *devcfg,
                                         int lossless);
extern int soc_th2_slot_pipeline_latency_get(int dpp_ratio_x10, int lmode);
extern int soc_tomahawk2_clear_mmu_memory(int unit);
extern int soc_tomahawk2_init_mmu_memory(int unit);
extern void soc_tomahawk2_sbus_ring_map_config(int unit);
extern int soc_tomahawk2_chip_reset(int unit);
extern int soc_th2_mmu_config_init(int unit, int test_only);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_tomahawk2_chip_warmboot_reset(int unit);
#endif

extern int soc_tomahawk2_sed_base_index_check(int unit, int base_type, int sed,
                                 int base_index, char *msg);
extern int soc_tomahawk2_sed_reg32_set(int unit, soc_reg_t reg, int sed,
                                     int base_index, int index, uint32 data);
extern int soc_tomahawk2_sed_reg32_get(int unit, soc_reg_t reg, int sed,
                                     int base_index, int index, uint32 *data);
extern int soc_tomahawk2_sed_reg_set(int unit, soc_reg_t reg, int sed,
                                   int base_index, int index, uint64 data);
extern int soc_tomahawk2_sed_reg_get(int unit, soc_reg_t reg, int sed,
                                   int base_index, int index, uint64 *data);
extern int _soc_tomahawk2_tdm_init(int unit);

extern void soc_tomahawk2_mmu_pstats_tbl_config(int unit);
extern int soc_tomahawk2_mmu_pstats_mode_set(int unit, uint32 flags);
extern int soc_tomahawk2_mmu_pstats_mode_get(int unit, uint32 *flags);
extern int soc_tomahawk2_mmu_pstats_mor_enable(int unit);
extern int soc_tomahawk2_mmu_pstats_mor_disable(int unit);
extern int soc_tomahawk2_mmu_splitter_reset(int unit);
extern int soc_tomahawk2_mdio_addr_to_port(uint32 phy_addr);
extern int soc_th2_ledup_init(int unit);
extern int soc_tomahawk2_bond_info_init(int unit);
extern int soc_tomahawk2_port_obm_info_get(int unit, soc_port_t port,
                                           int *obm_id, int *lane);
extern int soc_th2_latency_bypassed(int unit, soc_feature_t feature);
extern int
soc_th2_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                 int mmu_base_index,
                                 uint32 rval_intr_status_reg);
extern int
soc_tomahawk2_clear_enabled_port_data(int unit);

/* General Physical port */
#define _TH2_PORTS_PER_PBLK             4
#define _TH2_PBLKS_PER_PIPE             16
#define _TH2_PBLKS_PER_HPIPE            8
#define _TH2_PIPES_PER_DEV              4
#define _TH2_XPES_PER_DEV               4

#define _TH2_PBLKS_PER_DEV              \
    (_TH2_PBLKS_PER_PIPE * _TH2_PIPES_PER_DEV)

#define _TH2_PORTS_PER_PIPE             \
    (_TH2_PORTS_PER_PBLK * _TH2_PBLKS_PER_PIPE)
#define _TH2_PORTS_PER_DEV              \
    (_TH2_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)

/* 64 Falcon +  1 Eagle core*/
#define _TH2_PM_PER_DEV                 (_TH2_PBLKS_PER_DEV + 1)

#define _TH2_GPHY_PORTS_PER_PIPE        \
    (_TH2_PORTS_PER_PBLK * _TH2_PBLKS_PER_PIPE)
#define _TH2_GPHY_PORTS_PER_DEV         \
    (_TH2_GPHY_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)

#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

/* 64 General_phy_port + 1 CPU/Magagement + 1 Loopback
 * Note: phy port id range:
 *  General phy port 1-256
 *  CPU 0
 *  LB 260-263
 *  Mng 257,259
 */
#define _TH2_PHY_PORTS_PER_PIPE         (_TH2_GPHY_PORTS_PER_PIPE + 2)
#define _TH2_PHY_PORTS_PER_DEV          \
    (_TH2_PHY_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)
#define _TH2_PHY_PORT_MNG0              257
#define _TH2_PHY_PORT_MNG1              259

/* Device port */
/* 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH2_GDEV_PORTS_PER_PIPE        32
#define _TH2_DEV_PORTS_PER_PIPE         (_TH2_GDEV_PORTS_PER_PIPE + 2)
#define _TH2_DEV_PORTS_PER_DEV          \
    (_TH2_DEV_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)
#define _TH2_DEV_RSV_PORT               134 /* Reserved Port */
#define _TH2_DEV_PORT_MNG0              66
#define _TH2_DEV_PORT_MNG1              100


/* MMU port */
#define _TH2_MMU_PORTS_OFFSET_PER_PIPE  64
#define _TH2_MMU_PORTS_PER_DEV          (64 * 4)
#define _TH2_MMU_GPORTS_PER_PIPE        32 /* General Ethernet or Higig Ports*/
#define _TH2_MMU_PORT_MNG0              96
#define _TH2_MMU_PORT_MNG1              160

/* MMU spec */
#define _TH2_MMU_PHYSICAL_CELLS_PER_XPE  57344 /* Total Physical cells per XPE 14*4K */
#define _TH2_MMU_TOTAL_CELLS_PER_XPE     53248 /* usable cell pointer 13*4K cells */
#define _TH2_MMU_RSVD_CELLS_CFAP_PER_XPE 286   /* Reserved CFAP cells per XPE */

#define _TH2_THDI_BUFFER_CELL_LIMIT_SP_MAX      _TH2_MMU_TOTAL_CELLS_PER_XPE
#define _TH2_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX _TH2_MMU_TOTAL_CELLS_PER_XPE

#define _TH2_MMU_TOTAL_CELLS             (_TH2_MMU_TOTAL_CELLS_PER_XPE * _TH2_XPES_PER_DEV)
#define _TH2_MMU_PHYSICAL_CELLS          (_TH2_MMU_PHYSICAL_CELLS_PER_XPE * _TH2_XPES_PER_DEV)

#define SOC_TH2_NUM_UC_QUEUES_PER_PIPE   330 /* Num UC Q's Per pipe 33 * 10 */
#define SOC_TH2_MMU_MCQ_ENTRY_PER_XPE    8192
#define SOC_TH2_MMU_RQE_ENTRY_PER_XPE    2048
#define SOC_TH2_MMU_MCQ_ENTRY_RESERVED_TDM_OVERSHOOT    12

#define _TH2_MMU_TOTAL_MCQ_ENTRY(unit)   SOC_TH2_MMU_MCQ_ENTRY_PER_XPE
#define _TH2_MMU_TOTAL_RQE_ENTRY(unit)   SOC_TH2_MMU_RQE_ENTRY_PER_XPE

/* MMU pktstats control flags */
#define _TH2_MMU_PSTATS_ENABLE          0x0001 /* Packetized/OOB Stat mode is enabled */
#define _TH2_MMU_PSTATS_PKT_MOD         0x0002 /* Packetized mode is enabled. OOB Stat mode is disabled */
#define _TH2_MMU_PSTATS_HWM_MOD         0x0004 /* Report High Watermark values */
#define _TH2_MMU_PSTATS_RESET_ON_READ   0x0008 /* Enable Hardware clear-on read */
#define _TH2_MMU_PSTATS_MOR_EN          0x0010 /* Enable MOR capability */
#define _TH2_MMU_PSTATS_SYNC            0x8000 /* Read from device else the buffer */

/* IDB port */
#define _TH2_IDB_PORT_CPU_MNG           32

/**************** TDM start ***********************/
#define _TH2_TDM_LENGTH                 512
#define _TH2_OVS_GROUP_COUNT_PER_HPIPE  6
#define _TH2_OVS_GROUP_COUNT_PER_PIPE   12
#define _TH2_OVS_GROUP_TDM_LENGTH       12
#define _TH2_OVS_HPIPE_COUNT_PER_PIPE   2
#define _TH2_PKT_SCH_LENGTH             160

#define _TH2_TDM_CALENDAR_UNIVERSAL             0
#define _TH2_TDM_CALENDAR_ETHERNET_OPTIMIZED    1

#define _TH2_TDM_LR_SPEED_CLASS_MAX             4
#define _TH2_TDM_OS_SPEED_CLASS_MAX             5

/* Special port number used by TDM */
#define _SOC_TH2_TDM_CPU_MNG_TOKEN  0x20
#define _SOC_TH2_TDM_LB_TOKEN       0x21
#define _SOC_TH2_TDM_OVERSUB_TOKEN  0x22
#define _SOC_TH2_TDM_NULL_TOKEN     0x23
#define _SOC_TH2_TDM_IDL1_TOKEN     0x24
#define _SOC_TH2_TDM_IDL2_TOKEN     0x25
#define _SOC_TH2_TDM_UNUSED_TOKEN   0x3f

typedef struct _soc_tomahawk2_tdm_pblk_info_s {
    int pblk_hpipe_num; /* half pipeline info */
    int pblk_cal_idx;   /* index to 8 HPIPEx_PBLK_CALENDARr */
} _soc_tomahawk2_tdm_pblk_info_t;

typedef struct _soc_tomahawk2_tdm_pipe_s {
    int idb_tdm[_TH2_TDM_LENGTH];
    int mmu_tdm[_TH2_TDM_LENGTH];
    int ovs_tdm[_TH2_OVS_HPIPE_COUNT_PER_PIPE][_TH2_OVS_GROUP_COUNT_PER_HPIPE][_TH2_OVS_GROUP_TDM_LENGTH];
    int pkt_shaper_tdm[_TH2_OVS_HPIPE_COUNT_PER_PIPE][_TH2_PKT_SCH_LENGTH];
} _soc_tomahawk2_tdm_pipe_t;

typedef struct _soc_tomahawk2_tdm_s {
    _soc_tomahawk2_tdm_pipe_t tdm_pipe[_TH2_PIPES_PER_DEV];
    _soc_tomahawk2_tdm_pblk_info_t pblk_info[_TH2_PHY_PORTS_PER_DEV];
    int port_ratio[_TH2_PBLKS_PER_DEV];
    int ovs_ratio_x1000[_TH2_PIPES_PER_DEV][_TH2_OVS_HPIPE_COUNT_PER_PIPE];
} _soc_tomahawk2_tdm_t;

extern int _soc_tomahawk2_idb_init(int unit);
extern int _soc_tomahawk2_ing_fsaf_init(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_th2_tdm_scache_allocate(int unit);
extern int soc_th2_tdm_scache_sync(int unit);
extern int soc_th2_tdm_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/**************** TDM end ***********************/

/* Oversub XON/XOFF Threshold */
#define _TH2_XON_THRESHOLD_10G      1584
#define _TH2_XON_THRESHOLD_25G      1584
#define _TH2_XON_THRESHOLD_40G      4656
#define _TH2_XON_THRESHOLD_50G      4656
#define _TH2_XON_THRESHOLD_100G     11808
#define _TH2_XOFF_THRESHOLD_10G     2064
#define _TH2_XOFF_THRESHOLD_25G     2064
#define _TH2_XOFF_THRESHOLD_40G     5136
#define _TH2_XOFF_THRESHOLD_50G     5136
#define _TH2_XOFF_THRESHOLD_100G    12288

/*PG headroom*/
#define _TH2_PG_HEADROOM_LINERATE_10G   166
#define _TH2_PG_HEADROOM_LINERATE_25G   194
#define _TH2_PG_HEADROOM_LINERATE_40G   280
#define _TH2_PG_HEADROOM_LINERATE_50G   298
#define _TH2_PG_HEADROOM_LINERATE_100G  507
#define _TH2_PG_HEADROOM_OVERSUB_10G    \
    (_TH2_PG_HEADROOM_LINERATE_10G + CEIL(_TH2_XOFF_THRESHOLD_10G, 64))
#define _TH2_PG_HEADROOM_OVERSUB_25G    \
    (_TH2_PG_HEADROOM_LINERATE_25G + CEIL(_TH2_XOFF_THRESHOLD_25G, 64))
#define _TH2_PG_HEADROOM_OVERSUB_40G    \
    (_TH2_PG_HEADROOM_LINERATE_40G + CEIL(_TH2_XOFF_THRESHOLD_40G, 64))
#define _TH2_PG_HEADROOM_OVERSUB_50G    \
    (_TH2_PG_HEADROOM_LINERATE_50G + CEIL(_TH2_XOFF_THRESHOLD_50G, 64))
#define _TH2_PG_HEADROOM_OVERSUB_100G    \
    (_TH2_PG_HEADROOM_LINERATE_100G + CEIL(_TH2_XOFF_THRESHOLD_100G, 64))

/* CFAP */
#define _TH2_CFAP_BANK_FULL_LIMIT   2047

#define _TH2_POLL_SCALE_TO_LIMIT(size, scale)           \
    (((scale) > 0) && ((scale) < 8) ?                   \
        (((size) * (scale) * 125) / 1000) : (size))

extern int soc_tomahawk2_edb_buf_reset(int unit, soc_port_t port, int reset);
extern int soc_tomahawk2_idb_buf_reset(int unit, soc_port_t port, int reset);

enum soc_th2_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_TH2_PORT_MODE_QUAD = 0,
    SOC_TH2_PORT_MODE_TRI_012 = 1,
    SOC_TH2_PORT_MODE_TRI_023 = 2,
    SOC_TH2_PORT_MODE_DUAL = 3,
    SOC_TH2_PORT_MODE_SINGLE = 4
};

extern int soc_th2_port_mode_get(int unit, int logical_port, int *mode);

#ifdef PORTMOD_SUPPORT
extern int soc_th2_portctrl_pm_portmod_init(int unit);
extern int soc_th2_portctrl_pm_portmod_deinit(int unit);
extern int soc_th2_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config_info);
extern int soc_th2_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
extern int soc_th2_portctrl_port_ability_update(int unit, soc_port_t port,
                                                  soc_port_ability_t *ability);
extern int soc_th2_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);
#ifdef FW_BCAST_DOWNLOAD
extern int soc_th2_portctrl_sbus_ring_info_get(int unit, soc_port_t port, int* bcast_id, int* position);
extern int soc_th2_portctrl_sbus_bcast_id_get(int unit, int sbus_ch, int* bcast_id);
#endif
#endif

/************ FlexPort Start ********************/
extern int soc_th2_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_th2_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int soc_th2_mmu_config_init_port(int unit, int port);
extern int soc_th2_max_lr_bandwidth(int unit, uint32 *max_lr_bw);
extern int soc_th2_support_speeds(int unit, int lanes, uint32 *speed_mask);
extern int soc_th2_speed_mix_validate(int unit, uint32 speed_mask);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_th2_flexport_scache_allocate(int unit);
extern int soc_th2_flexport_scache_sync(int unit);
extern int soc_th2_flexport_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void soc_th2_flexport_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
/************ FlexPort End ********************/
typedef int (*_soc_th2_tcb_hw_cb)(int unit);
extern int soc_th2_set_tcb_callback(int unit, _soc_th2_tcb_hw_cb cb);
extern int soc_th2_process_mmu_tcb(int unit, int xpe, soc_field_t field);
extern soc_error_t soc_th2_port_speed_supported(int unit, soc_port_t port,
                                                int speed);

extern int soc_th2_mmu_config_shared_buf_set_hw(int unit, int res, 
                            int *thdi_shd, int *thdo_db_shd, int *thdo_qe_shd,
                            int post_update);
extern int soc_th2_mmu_config_shared_buf_set_hw_port(int unit, int port,
                                                     int *thdi_shd,
                                                     int *thdo_db_shd,
                                                     int *thdo_qe_shd);
extern int soc_th2_max_lr_bandwidth_validate(int unit);
extern int soc_th2_speed_class_validate(int unit);

/************ Untethered OTP Start *************/
typedef struct _soc_tomahawk2_tsc_otp_info_s{
    int disable;            /* TSC_DISABLE */
    int max_lane_speed;     /* TSC_MAX_LANE_SPEED */
    int force_loopback;     /* TSC_FORCE_LOOPBACK */
    int force_hg;           /* FORCE_HG */
} _soc_tomahawk2_tsc_otp_info_t;

extern int soc_tomahawk2_tsc_otp_info_get(int unit, int tsc_id,
                                          _soc_tomahawk2_tsc_otp_info_t *info);
extern void soc_th2_uft_otp_info_get(int unit, 
    int *num_share_banks_enabled, int *share_banks_bitmap_enabled);
extern int soc_th2_set_uft_bank_map(int unit);

/************* Untethered OTP End **************/

/************ SRAM info Start *************/
#define SOC_TH2_NUM_EL_SHARED                         8
#define SOC_TH2_LP_ENTRIES_IN_UFT_BANK                16384
#define SOC_TH2_NUM_EL_ING_L3_NEXT_HOP                6
/************* SRAM info End **************/

/************* AVS Start **************/
#ifdef INCLUDE_AVS
extern soc_error_t soc_th2_avs_init(int unit);
#endif /* INCLUDE_AVS */
/************* AVS End **************/

#endif	/* !_SOC_TOMAHAWK2_H_ */
