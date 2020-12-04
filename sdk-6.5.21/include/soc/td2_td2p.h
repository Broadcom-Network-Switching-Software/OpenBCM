/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        td2_td2p.h
 * Purpose:
 * Requires:
 */
#ifndef _TD2_TD2P_H_
#define _TD2_TD2P_H_

#include <soc/types.h>
#include <soc/mmu_config.h>
#include <soc/esw/port.h>
#include <soc/portmod/portmod_defs.h>

/* TDM algorithm */
#define _TD2P_NUM_PORT_MODULES 33
#define OVSB_TOKEN 250
#define IDL1_TOKEN 251
#define IDL2_TOKEN 252
#define NUM_EXT_PORTS 130
#define OS_GROUP_LEN 16
#define NUM_TSC 32
#define PGW_LR_UNUSED_SLOT 129


#define _TD2_OVS_TOKEN                  (NUM_EXT_PORTS+1)
#define _TD2_IDL_TOKEN                  (NUM_EXT_PORTS+2)

#define _TD2_PGW_TDM_LENGTH             32
#define _TD2_IARB_TDM_LENGTH            512
#define _TD2_TD2_CELLS_PER_OBM          1020
#define _MMU_OVS_GROUP_COUNT            4
#define _TD2_MMU_OVS_GROUP_COUNT        _MMU_OVS_GROUP_COUNT


#ifdef BCM_TRIDENT2PLUS_SUPPORT

extern int get_mmu_mode (int unit);
extern int soc_td2p_if_full_chip (int unit);

#define _TD2P_PGW_TDM_LENGTH            64
#define MAX_PGW_TDM_LENGTH              64

#define _TD2P_TD2_CELLS_PER_OBM         2044


#define _TD2P_MMU_OVS_GROUP_COUNT       8
#define MAX_MMU_OVS_GROUP_COUNT         8

#define GET_PGW_TDM_LENGTH(unit)\
    ((SOC_IS_TD2P_TT2P(unit)) ? _TD2P_PGW_TDM_LENGTH : _TD2_PGW_TDM_LENGTH )

#define GET_MMU_OVS_GCOUNT(unit)\
    ((SOC_IS_TD2P_TT2P(unit)) ? _TD2P_MMU_OVS_GROUP_COUNT : _TD2_MMU_OVS_GROUP_COUNT )

#define GET_NUM_CELLS_PER_OBM(unit)\
    ((SOC_IS_TD2P_TT2P(unit)) ? _TD2P_TD2_CELLS_PER_OBM : _TD2_TD2_CELLS_PER_OBM )

#define _TD2P_OVS_TOKEN                 OVSB_TOKEN
#define _TD2P_IDL_TOKEN                 IDL1_TOKEN

#define GET_OVS_TOKEN(unit)\
    ((SOC_IS_TD2P_TT2P(unit)) ? _TD2P_OVS_TOKEN : _TD2_OVS_TOKEN )
#define GET_IDL_TOKEN(unit)\
    ((SOC_IS_TD2P_TT2P(unit)) ? _TD2P_IDL_TOKEN : _TD2_IDL_TOKEN )

#else

#define GET_PGW_TDM_LENGTH(unit)         _TD2_PGW_TDM_LENGTH
#define GET_MMU_OVS_GCOUNT(unit)         _MMU_OVS_GROUP_COUNT
#define GET_NUM_CELLS_PER_OBM(unit)     _TD2_TD2_CELLS_PER_OBM

#define MAX_PGW_TDM_LENGTH              32
#define MAX_MMU_OVS_GROUP_COUNT         4

#define GET_OVS_TOKEN(unit)         _TD2_OVS_TOKEN
#define GET_IDL_TOKEN(unit)         _TD2_IDL_TOKEN

#endif

#define _PGW_TDM_OVS_SIZE               32

#define _PGW_MASTER_COUNT               4
#define _PGW_TDM_SLOTS_PER_REG          4
#define _MMU_TDM_LENGTH                 256
#define _MMU_OVS_GROUP_TDM_LENGTH       16
#define _IARB_TDM_LENGTH                512
#define _MMU_OVS_WT_GROUP_COUNT         4

#define PGW_OBM_INIT_HW_DEFAULT (0)
#define PGW_OBM_INIT_SW_DEFAULT (1)

#define SOC_TD2P_MAX_MMU_PORTS_PER_PIPE 52

#define _SOC_TD2_MAX_PORT          (_TD2_LOG_PORTS_PER_PIPE * \
                                    _TD2_PIPES_PER_DEV)

#define _TD2_MMU_PACKET_HEADER_BYTES    64    /* bytes */
#define _TD2_MMU_JUMBO_FRAME_BYTES      9216  /* bytes */
#define _TD2_MMU_DEFAULT_MTU_BYTES      1536  /* bytes */

#define _TD2_MMU_BYTES_PER_CELL      208   /* bytes (1664 bits) */
#define _TD2_MMU_NUM_PG              8
#define _TD2_MMU_NUM_POOL            4
#define _TD2_MMU_NUM_RQE_QUEUES      11
#define _TD2P_MMU_RSVD_BUF_FLEX_PGMIN_LOSSLESS 8

#define SOC_TD2_MMU_CFG_QGROUP_MAX   128

/* quantization loss + TOQ reserved
 * quantization loss = MC queue count * 6
 * TOQ reserved = TOQ_MC_CFG0.MCQE_FULL_THRESHOLD * 4
 */
#define SOC_TD2_MMU_MCQ_RSVD_ENTRY   ((52 * 10 * 2 + 48 + 10) * 6 + (164 * 4))
#define SOC_TD2_MMU_RQE_TOTAL_RSVD_ENTRY (SOC_IS_TD2P_TT2P(unit) ? 76 : 88)
#define SOC_TD2_MMU_RQE_USER_RSVD_ENTRY (SOC_IS_TD2P_TT2P(unit) ? (11*8) : 0)
#define SOC_TD2_MMU_MCQ_ENTRY        49152
#define SOC_TD2_MMU_RQE_ENTRY        4096

#define _TD2P_MMU_GLOBAL_HEADROOM 50 /* per pipe */
#define _TD2P_MMU_CPU_HEADROOM    50 /* per device */
#define SOC_TD2_MMU_QGROUP_MIN_RSVD (SOC_IS_TD2P_TT2P(unit) ? 16 : 8)
#define SOC_TD2_MMU_Q_MIN_RSVD      (SOC_IS_TD2P_TT2P(unit) ? 0 : 8)

#define _TD2_MMU_TOTAL_MCQ_ENTRY(unit)  SOC_TD2_MMU_MCQ_ENTRY

#define _TD2P_MMU_NUM_MCQ_PORT_MACRO 10
#define _TD2P_MMU_PER_COSQ_EGRESS_QENTRY_RSVD 4

#define _TD2_MMU_TOTAL_RQE_ENTRY(unit)  SOC_TD2_MMU_RQE_ENTRY

#define _TD2_MMU_PORT_CREDIT_100G (SOC_IS_TD2P_TT2P(unit) ? 96 : 144)
#define _TD2_MMU_PORT_CREDIT_40G  48
#define _TD2_MMU_PORT_CREDIT_20G  24
#define _TD2_MMU_PORT_CREDIT_10G  12

#define _TD2_MMU_PORT_CREDIT(speed) _TD2_MMU_PORT_CREDIT_##speed

#define _TD2_MMU_ASF_CREDIT_THRESH_LO_100G (SOC_IS_TD2P_TT2P(unit) ? 30 : 64)
#define _TD2_MMU_ASF_CREDIT_THRESH_HI_100G (SOC_IS_TD2P_TT2P(unit) ? 30 : 108)
#define _TD2_MMU_ASF_CREDIT_THRESH_LO_40G  18
#define _TD2_MMU_ASF_CREDIT_THRESH_HI_40G  (SOC_IS_TD2P_TT2P(unit) ? 18 : 36)
#define _TD2_MMU_ASF_CREDIT_THRESH_LO_20G  8
#define _TD2_MMU_ASF_CREDIT_THRESH_HI_20G  (SOC_IS_TD2P_TT2P(unit) ? 8 : 18)
#define _TD2_MMU_ASF_CREDIT_THRESH_LO_10G  4
#define _TD2_MMU_ASF_CREDIT_THRESH_HI_10G  (SOC_IS_TD2P_TT2P(unit) ? 4 : 9)

#define _TD2_MMU_ASF_CREDIT_THRESH(param, speed) _TD2_MMU_ASF_CREDIT_THRESH_##param##_##speed

#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_LO_100G 10
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_HI_100G 10
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_LO_40G  5
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_HI_40G  5
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_LO_20G  4
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_HI_20G  4
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_LO_10G  3
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_HI_10G  3
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_OVERSUB_LO_40G  8
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_OVERSUB_HI_40G  8

#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED(param, speed) \
        _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_##param##_##speed
#define _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_OVERSUB(param, speed) \
        _TD2P_MMU_ASF_CREDIT_THRESH_OPTIMIZED_OVERSUB_##param##_##speed

/* The actual limits are 43, 22, 15, and 11, but EGR_MMU_CREDIT_REQUEST uses
   <= for credit comparison, so we set the credit limit to 1 less
   than the expected value.*/
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE_100G    42 /* 43 */
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE_40G     21 /* 22 */
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE_20G     14 /* 15 */
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE_10G     10 /* 11 */

#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(speed) \
            _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE_##speed

/* The actual limits are 23, 11, and 8, but EGR_MMU_CREDIT_REQUEST uses
   <= for credit comparison, so we set the credit limit to 1 less
   than the expected value.*/
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB_40G  22 /* 23 */
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB_20G  10 /* 11 */
#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB_10G  7 /* 8 */

#define _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(speed) \
            _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB_##speed

#define _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE_100G    61
#define _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE_40G     31
#define _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE_20G     19
#define _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE_10G     13

#define _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE(speed) \
            _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE_##speed

#define _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB_40G     42
#define _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB_20G     19
#define _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB_10G     13
                                       
#define _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB(speed) \
            _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB_##speed

#define _TD2_MMU_ASF_FIFO_THRESHOLD 3
#define _TD2_MMU_ASF_HS_FIFO_THRESHOLD 13

#define _TD2_TD2_MMU_XMIT_START_CNT_OVERSUB (si->frequency >= 608 ? 9 :\
                                            (si->frequency >= 500 ? 11 : 12))

#define _TD2P_TD2_MMU_XMIT_START_CNT_OVERSUB ((si->frequency >= 760 ? 11 :\
                                             (si->frequency >= 608 ? 12 : \
                                             (si->frequency >= 500 ? 14 : 15))))

#define XMIT_START_CNT_OVERSUB(param)\
    ((SOC_IS_TD2P_TT2P(unit))? _TD2P##param : _TD2##param)

#define _TD2_MMU_XMIT_START_CNT_OVERSUB XMIT_START_CNT_OVERSUB(_TD2_MMU_XMIT_START_CNT_OVERSUB)


#define _TD2_TD2_MMU_XMIT_START_CNT_LINERATE(speed) \
        (speed > 42000 ? 3 : ((si->frequency <= 415) && (speed > 11000) ? 8 : 7))

#define _TD2P_TD2_MMU_XMIT_START_CNT_LINERATE(speed) \
        (speed > 42000 ? 10 : ((si->frequency <= 415) && (speed > 21000) ? 8 : 7))

#define XMIT_START_CNT_LINERATE(param,speed)\
    ((SOC_IS_TD2P_TT2P(unit))? _TD2P##param(speed) : _TD2##param(speed))

#define _TD2_MMU_XMIT_START_CNT_LINERATE(speed) XMIT_START_CNT_LINERATE(_TD2_MMU_XMIT_START_CNT_LINERATE, speed)

#define _TD2_PGW_CELL_ASM_CUT_THRU_THRESHOLD (SOC_IS_TD2P_TT2P(unit) ? 4 : 6)

/*
 * Define:
 *      TD2P_PHY_PORT_ADDRESSABLE
 * Purpose:
 *      Checks that physical port is addressable (within valid range).
 */
#define TD2P_PHY_PORT_ADDRESSABLE(_u, _phy_port)                  \
    (soc_td2p_phy_port_addressable((_u), (_phy_port)) == SOC_E_NONE)


#define _SOC_TD2P_DEFIP_REDUCED_TCAMS   2
#define _SOC_TD2_DEFIP_MAX_TCAMS   8
#define _SOC_TD2_DEFIP_TCAM_DEPTH 1024

#define _SOC_TD2_MAX_INTERNAL_PRIORITY 15
#define _SOC_TD2_MAX_PRIORITY           7

#define PORT_STATE_UNUSED              0
#define PORT_STATE_LINERATE            1
#define PORT_STATE_OVERSUB             2
#define PORT_STATE_SUBPORT             3 /* part of other port */

typedef struct _soc_trident2_tdm_s {
    int speed[NUM_EXT_PORTS];
    int tdm_bw;
    int port_state[NUM_EXT_PORTS]; /* 0-unused, 1-line rate, 2-oversub,
                                    * 3-part of other port */
    int pipe_ovs_state[2];
    int manage_port_type; /* 0-none, 1-4x1g, 2-4x2.5g, 3-1x10g */

    int pgw_tdm[_PGW_MASTER_COUNT][MAX_PGW_TDM_LENGTH];
    int pgw_ovs_tdm[_PGW_MASTER_COUNT][_PGW_TDM_OVS_SIZE];
    int pgw_ovs_spacing[_PGW_MASTER_COUNT][_PGW_TDM_OVS_SIZE];
    int mmu_tdm[2][_MMU_TDM_LENGTH + 1];
    int mmu_ovs_group_tdm[2][MAX_MMU_OVS_GROUP_COUNT][_MMU_OVS_GROUP_TDM_LENGTH];
    int iarb_tdm_wrap_ptr[2];
    int iarb_tdm[2][_IARB_TDM_LENGTH];
#ifdef BCM_TRIDENT2PLUS_SUPPORT
	int encap_type[_TD2P_NUM_PORT_MODULES];
#endif
    int ancillary_bandwidth_mode;/* 0-none, 1-CPU, 2-LOOPBACK, 3-CPU_and_LOOPBACK */
} _soc_trident2_tdm_t;

typedef soc_reg_t(*mmu_ovs_group_wt_regs_t)[_MMU_OVS_WT_GROUP_COUNT];


#ifdef BCM_TRIDENT2PLUS_SUPPORT

#define TD2P_SUBPORT_COE_GROUP_MAX             (512)
#define TD2P_SUBPORT_COE_PORT_MAX              (512)
#define TD2P_MAX_SUBPORT_COE_PORT_PER_CASPORT  (80)
#define TD2P_MAX_SUBPORT_COE_PORT_PER_MOD      (128)
#define TD2P_MAX_COE_MODULES                   (5)


#define TD2P_X_PIPE                0
#define TD2P_Y_PIPE                1

#define TD2P_PIPES_PER_DEV         2
#define TD2P_PGWS_PER_QUAD         2
#define TD2P_QUADS_PER_PIPE        2

#define TD2P_PGWS_PER_PIPE         (TD2P_PGWS_PER_QUAD * TD2P_QUADS_PER_PIPE)
#define TD2P_PGWS_PER_DEV          (TD2P_PGWS_PER_PIPE * TD2P_PIPES_PER_DEV)

#define TD2P_XLPS_PER_PGW          4
#define TD2P_PORTS_PER_XLP         4

#define TD2P_PHY_PORTS_PER_PIPE    65

#define TD2P_PHY_PORT_CPU          0    /* CPU physical port */
#define TD2P_PHY_PORT_LB           129  /* Loopback physical port */

#define SOC_TD2P_PORT_CT_SPEED_NONE                0x0
#define SOC_TD2P_PORT_CT_SPEED_10M_FULL            0x1
#define SOC_TD2P_PORT_CT_SPEED_100M_FULL           0x2
#define SOC_TD2P_PORT_CT_SPEED_1000M_FULL          0x3
#define SOC_TD2P_PORT_CT_SPEED_2500M_FULL          0x4
#define SOC_TD2P_PORT_CT_SPEED_10000M_FULL         0x5
#define SOC_TD2P_PORT_CT_SPEED_11000M_FULL         0x6
#define SOC_TD2P_PORT_CT_SPEED_12000M_FULL         0x7
#define SOC_TD2P_PORT_CT_SPEED_13000M_FULL         0x8
#define SOC_TD2P_PORT_CT_SPEED_15000M_FULL         0x9
#define SOC_TD2P_PORT_CT_SPEED_16000M_FULL         0xa
#define SOC_TD2P_PORT_CT_SPEED_20000M_FULL         0xb
#define SOC_TD2P_PORT_CT_SPEED_21000M_FULL         0xc
#define SOC_TD2P_PORT_CT_SPEED_25000M_FULL         0xd
#define SOC_TD2P_PORT_CT_SPEED_30000M_FULL         0xe
#define SOC_TD2P_PORT_CT_SPEED_40000M_FULL         0xf
#define SOC_TD2P_PORT_CT_SPEED_42000M_FULL         0x10
#define SOC_TD2P_PORT_CT_SPEED_100000M_FULL        0x11
#define SOC_TD2P_PORT_CT_SPEED_106000M_FULL        0x12
#define SOC_TD2P_PORT_CT_SPEED_120000M_FULL        0x13
#define SOC_TD2P_PORT_CT_SPEED_127000M_FULL        0x14

enum td2plus_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE = 0,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP = 1,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE = 2,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_ISID_XLATE = 3,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE = 4,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI = 5,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP = 6,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP = 7,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI = 8,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP = 9,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VFI_DVP_GROUP = 10,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VRF_MAPPING = 11,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI =12,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_COUNT
};

enum td2plus_dvp_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_NOOP = 0,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_VFI = 1,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_VFI = 2,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_GID_VFI = 3,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_OL_VLAN = 4,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_PORT_UL_VLAN = 5,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_COUNT
};

/*
 * Data structure use for storing part of the SOC data information
 * which is needed during later phases of a FlexPort operation.
 */
typedef struct soc_td2p_info_s {
    int port_l2p_mapping[SOC_MAX_NUM_PORTS];      /* Logic to physical */
    int port_p2l_mapping[SOC_MAX_NUM_PORTS];      /* Physical to logic */
    int port_p2m_mapping[SOC_MAX_NUM_PORTS];      /* Physical to MMU */
    int port_m2p_mapping[SOC_MAX_NUM_MMU_PORTS];  /* MMU to physical */
    int port_group[SOC_MAX_NUM_PORTS];            /* Group number */
    int port_speed_max[SOC_MAX_NUM_PORTS];        /* Max port speed */
    int port_num_lanes[SOC_MAX_NUM_PORTS];        /* Number of lanes */
    pbmp_t xpipe_pbm;                             /* Ports in X-pipe */
    pbmp_t ypipe_pbm;                             /* Ports in Y-pipe */
    pbmp_t oversub_pbm;                           /* Oversubscription ports */
} soc_td2p_info_t;

extern _soc_trident2_tdm_t *
soc_td2_td2p_tdm_sched_info_get(int unit, int create);

extern mmu_ovs_group_wt_regs_t 
get_mmu_ovs_group_wt_regs(int unit, mmu_ovs_group_wt_regs_t old);

extern int
soc_td2p_set_obm_registers(int unit, soc_reg_t reg, int speed,
                           int index, int obm_inst, int lossless, int
                           default_flag );
extern int
soc_td2p_obm_prob_drop_default_set(int unit, int xlp, int index,
                                   int obm_inst, int default_flag);
extern
int soc_td2p_obm_cut_through_threshold_set (int unit, int xlp, int index,
                                            int obm_inst, int default_flag);
extern int
soc_td2p_mmu_delay_insertion_set(int unit, int port, int speed); 

extern int
soc_td2p_mem_config(int unit, uint16 dev_id, uint8 rev_id);

extern int
soc_td2p_get_shared_bank_size(int unit, uint16 dev_id, uint8 rev_id);

extern uint32*
soc_td2p_mmu_params_arr_get(uint16 dev_id, uint8 rev_id);

extern int
tdm_td2p_set_iarb_tdm_table(int core_bw, int is_x_ovs, int is_y_ovs,
                            int mgm4x1, int mgm4x2p5, int mgm1x10,
                            int *iarb_tdm_wrap_ptr_x, int *iarb_tdm_wrap_ptr_y,
                            int *iarb_tdm_tbl_x, int *iarb_tdm_tbl_y);

extern int
soc_td2p_show_voltage(int unit);

extern int
soc_td2p_idb_buf_reset(int unit, soc_port_t port, int reset);

extern int 
soc_td2p_set_mmu_credit_limit(int unit, soc_port_t port);

extern int
soc_td2p_use_pfc_optimized_settings(int unit);

extern int
soc_td2p_set_edb_pfc_watermark_threshold(int unit, soc_port_t port);

extern int
soc_td2p_edb_buf_reset(int unit, soc_port_t port, int reset);

extern int
soc_td2p_is_cut_thru_enabled(int unit, soc_port_t port, int *enable);

extern int
soc_td2p_egr_edb_xmit_ctrl_set(int unit, int phy_port, int speed,
                               int oversub, int cut_thru_enable);

extern int
soc_td2p_phy_info_init(int unit);

extern int
soc_td2p_port_mixed_sister_speed_validate(int unit);

extern int
soc_td2p_port_oversub_get(int unit, int phy_port, soc_port_t logical_port,
                          int *oversub);

extern int
soc_td2p_port_lanes_valid(int unit, soc_port_t port, int lanes);

extern int
soc_td2p_port_resource_speed_max_get(int unit, int *speed);

extern int
soc_td2p_port_resource_speed_max_x_get(int unit, int *speed);

extern int
soc_td2p_port_resource_speed_max_y_get(int unit, int *speed);

extern int
soc_td2p_port_addressable(int unit, soc_port_t port);

extern int
soc_td2p_phy_port_addressable(int unit, int phy_port);

extern int
soc_td2p_phy_port_pgw_info_get(int unit, int phy_port,
                               int *pgw, int *xlp, int *port_index);

extern int
soc_td2p_port_macro_first_phy_port_get(int unit, int phy_port,
                                       int *first_phy_port);

extern int
soc_td2p_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp);

extern int
soc_td2p_port_resource_validate(int unit, int nport,
                                soc_port_resource_t *resource);

extern int
soc_td2p_port_resource_configure(int unit, int nport,
                                 soc_port_resource_t *resource, int flag);

extern int
soc_td2p_pgw_obm_set(int unit,
                     int pre_num_res,  soc_port_resource_t *pre_res,
                     int post_num_res, soc_port_resource_t *post_res);

extern int
soc_td2p_port_resource_tdm_set(int unit, int curr_port_info_size,
                               soc_port_resource_t *curr_port_info,
                               int new_port_info_size,
                               soc_port_resource_t *new_port_info,
                               soc_td2p_info_t *si,
                               int lossless);

extern int
soc_td2p_port_resource_pgw_set(int unit,
                               int pre_num_res,  soc_port_resource_t *pre_res,
                               int post_num_res, soc_port_resource_t *post_res,
                               int lossless);

extern int
soc_td2p_port_resource_ip_set(int unit, int curr_port_info_size,
                              soc_port_resource_t *curr_port_info,
                              int new_port_info_size,
                              soc_port_resource_t *new_port_info,
                              soc_td2p_info_t *si);

extern int
soc_td2p_port_resource_ep_set(int unit, int curr_port_info_size,
                              soc_port_resource_t *curr_port_info,
                              int new_port_info_size,
                              soc_port_resource_t *new_port_info,
                              soc_td2p_info_t *si);

extern int
soc_td2p_port_resource_mmu_mapping_set(int unit, int nport,
                                       soc_port_resource_t *resource);

extern int
soc_td2p_port_resource_mmu_set(int unit, int nport, 
                               soc_port_resource_t * flexport_data);
extern int
soc_td2p_port_lane_config_get(int unit, int phy_port, int *mode_lane,
                              int *found);

extern int
soc_td2p_port_autoneg_core_get(int unit, int phy_port, int *an_core,
                               int *found);

extern int
soc_td2p_mmu_rqe_phy_port_mapping_set(int unit,
                               int pre_num_res,  soc_port_resource_t *pre_res);

extern int
soc_td2p_lls_reinit_invalid_pointers(int unit);
extern int
soc_td2p_lls_reset_flex(int unit);

extern int
soc_td2p_mmu_additional_buffer_reserve (int unit, int pipe, int flex,
                                        _soc_mmu_rsvd_buffer_t *rsvd_buffer);
extern int
soc_td2p_port_asf_set (int unit, soc_port_t port, int enable);
extern int
soc_td2_port_asf_speed_set(int unit, soc_port_t port, int speed);

extern int
_soc_td2_default_lossless_pg_headroom(int unit, soc_port_t port);
extern int
_soc_td2_min_cell_rsvd_per_mcq(int unit, int port, int default_val);
extern void
_soc_td2_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
         int lossless);
extern void
_soc_td2_mmu_config_buf_default(int unit, _soc_mmu_cfg_buf_t *buf,
                                _soc_mmu_device_info_t *devcfg, int lossless);
extern void
_soc_td2_mmu_config_buf_default_global(int unit, _soc_mmu_cfg_buf_t *buf,
        _soc_mmu_device_info_t *devcfg, int lossless);
extern void
_soc_td2_mmu_config_buf_default_port(int unit, int in_port, _soc_mmu_cfg_buf_t *buf,
        _soc_mmu_device_info_t *devcfg, int lossless);
extern int
_soc_td2_mmu_config_buf_set_hw_port(int unit, int port, _soc_mmu_cfg_buf_t *buf,
        _soc_mmu_device_info_t *devcfg, int lossless);
extern int
soc_td2p_mcq_entries_rsvd_port(int unit, int port);
extern int
soc_td2p_egr_buf_rsvd_port(int unit, int port, int default_mtu_cells);


/* 
 * FlexPort Functions
 */

extern int
soc_td2p_logic_ports_max_validate(int unit, portmod_pbmp_t phy_pbmp);

extern int 
soc_td2p_num_hsp_reserved_per_pipe(int unit, int pipe, int *num_hsp);

extern void
soc_td2p_num_hsp_reserved_reset(int unit);


extern int
soc_td2p_port_macro_flex_enabled(int unit, int port_macro, int *flex_enabled);

extern int 
soc_td2p_mmu_port_resource_set(int unit, soc_port_t port, int speed, int legacyFlex);
extern int 
soc_td2p_mmu_port_resource_clear(int unit, soc_port_t port);

/* interfaces to reconfigure mmu data structures */
extern int soc_td2p_mmu_flexport_map_validate(int unit, int nport, 
        soc_port_resource_t * flex_port_data);
extern int soc_td2p_mmu_flexport_map_set(int unit, int nport, 
        soc_port_resource_t * flexport_data);

/* called by interfaces to reconfigure mmu data structures */
extern int soc_td2p_mmu_flexport_allocate_deallocate_ports(int unit,
        int nport, soc_port_resource_t * flexport_data);

extern int soc_td2p_is_any_port_flex_enable(int unit);
extern int soc_td2p_is_port_flex_enable(int unit, int phy_port);
extern int soc_td2p_port_mode_get(int unit, int port, int *mode);

#ifdef PORTMOD_SUPPORT
extern int soc_td2p_portctrl_pm_portmod_init(int unit);
extern int soc_td2p_portctrl_pm_portmod_deinit(int unit);
extern int soc_td2p_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config_info);
extern int soc_td2p_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
extern int soc_td2p_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);
#endif

typedef int (*_soc_td2p_flexport_port_q_cb)(int unit, int nports, int * ports);
typedef int (*_soc_td2p_ets_mode_q_cb)(int unit, int port);
extern int soc_td2p_set_flexport_port_q_callback(int unit, 
                                             _soc_td2p_flexport_port_q_cb cb);
extern int soc_td2p_set_ets_mode_q_callback(int unit, 
                                             _soc_td2p_ets_mode_q_cb cb);

extern int soc_td2p_port_speed_set(int unit, soc_port_t port, int speed);
extern int soc_td2p_port_speed_get(int unit, soc_port_t port, int *speed);

extern int soc_td2p_port_sched_hsp_set(int unit, soc_port_t port, int sched_hsp);
extern int soc_td2p_port_sched_hsp_get(int unit, soc_port_t port, int *sched_hsp);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_td2p_flexport_scache_allocate(int unit);
extern int soc_td2p_flexport_scache_sync(int unit);
extern int soc_td2p_flexport_scache_recovery(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void soc_flexport_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

extern int soc_td2p_is_flex_enable(int unit);

#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#endif /* _td2_td2p_h */
