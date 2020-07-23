/*! \file port.c
 *
 * Port Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/port_int.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/mbcm/port.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/link.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/pfc.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/issu.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/core/boot.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_crc.h>
#include <shr/shr_util.h>
#include <shr/shr_types.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_pc_constants.h>
#include <bcmbd/bcmbd_miim.h>
#include <bcmpc/bcmpc_phy.h>
#include <bcmpc/bcmpc_pmgr.h>


#define BCMI_QOS_INT_PRIO_MAX(unit)     15
#define BCMI_QOS_DSCP_MAX(unit)         63

#define BCMPC_FIELD_OFFSETOF(_st, _f) \
            ((size_t)((char *)&((_st *)(0))->_f - (char *)0))

#define BSL_LOG_MODULE BSL_LS_BCM_PORT

#define DEFAULT_MAX_FRAME_SIZE(unit)      ltsw_port_info[unit]->max_frame_size
#define MIN_LANES_PER_PPORT(unit)         ltsw_port_info[unit]->min_lanes_per_pport

#define MAX_NUM_LANES_PER_PM 16

#define LTSW_PORT_CFG_PER_PORT   0
#define LTSW_PORT_CFG_PER_LANE   1

/* parse phy_id to bus, phy, and internal indication
*   phy_id encoding:
*       bit7, 1: internal MDIO bus, 0: external MDIO bus
*       bit9,8,6,5, mdio bus number
*       bit4-0,   mdio addresses
*/
#define LTSW_PHY_BUS_ID_UPPER_MASK      0x300
#define LTSW_PHY_BUS_ID_LOWER_MASK      0x60
#define LTSW_PHY_BUS_ID_UPPER_SHIFT     6
#define LTSW_PHY_BUS_ID_LOWER_SHIFT     5
#define LTSW_PHY_INT_SET_MASK           0x80
#define LTSW_PHY_ID_MASK                0x1F
#define LTSW_PHY_BUS_ID(_mdio)   ((((_mdio) & LTSW_PHY_BUS_ID_UPPER_MASK) >> \
        LTSW_PHY_BUS_ID_UPPER_SHIFT) | (((_mdio) & LTSW_PHY_BUS_ID_LOWER_MASK) >> \
        LTSW_PHY_BUS_ID_LOWER_SHIFT))
#define LTSW_PHY_IS_INTERNAL(_mdio)  ((_mdio & LTSW_PHY_INT_SET_MASK) ? 1 : 0)
#define LTSW_PHY_ID(_mdio)      (_mdio & LTSW_PHY_ID_MASK)

#define LTSW_PORT_PHY_REG_INDIRECT  _SHR_PORT_PHY_REG_INDIRECT


ltsw_port_ctrl_info_t *ltsw_port_info[BCM_MAX_NUM_UNITS];

typedef struct ltsw_pc_port_info_s {
    int pm_id;
    int pport;
    int speed;
    int lane;
    int fec_mode;
    int enable;
} ltsw_pc_port_info_t;

/* PC_PORT table field. */
#define PC_PORT_ENABLE                      (1 << 1)
#define PC_PORT_MAX_FRAME_SIZE              (1 << 2)
#define PC_PORT_LPBK_MODE                   (1 << 3)
#define PC_PORT_AUTONEG_PROFILE_ID          (1 << 4)
#define PC_PORT_FEC_MODE                    (1 << 5)
#define PC_PORT_NUM_LANES                   (1 << 6)
#define PC_PORT_AUTONEG                     (1 << 7)
#define PC_PORT_LINK_TRAINING               (1 << 8)
#define PC_PORT_SPEED                       (1 << 9)
#define PC_PORT_VALID_ABILITY_NUM           (1 << 10)
#define PC_PORT_ENCAP                       (1 << 11)
#define PC_PORT_LAG_FAILOVER                (1 << 12)
#define PC_PORT_CUSTOM_SPEED_VCO            (1 << 13)
#define PC_PORT_RLM                         (1 << 14)
#define PC_PORT_INITIATOR                   (1 << 15)
#define PC_PORT_ACTIVE_LANE_MASK            (1 << 16)
#define PC_PORT_SUSPEND                     (1 << 17)

#define PC_PORT_FIELD_RESTORE   (PC_PORT_ENABLE|PC_PORT_MAX_FRAME_SIZE|PC_PORT_LPBK_MODE|PC_PORT_FEC_MODE    \
                                 |PC_PORT_NUM_LANES|PC_PORT_AUTONEG|PC_PORT_LINK_TRAINING|PC_PORT_SPEED)

#define PC_AN_PROFILE_ID(ptr_port_data, idx)  (((pc_port_data_t*)ptr_port_data)->pc_an_profile_id.profile_id[idx])
#define PC_AN_PROFILE_NUM(ptr_port_data)      (((pc_port_data_t*)ptr_port_data)->pc_an_profile_id.profile_num)
#define PC_AN_PROFILE_ID_DEFAULT (0xff)

typedef struct pc_port_profile_id_s {
    uint32 profile_num;
    uint64_t profile_id[MAX_NUM_AUTONEG_PROFILES];
} pc_port_profile_id_t;

typedef struct pc_port_data_s {
    uint64_t enable;
    uint64_t max_frame_size;
    uint64_t num_lanes;
    pc_port_profile_id_t pc_an_profile_id;
    uint64_t autoneg;
    uint64_t link_training;
    uint64_t speed;
    uint64_t valid_ability_num;
    uint64_t lag_failover;
    uint64_t rlm;
    uint64_t initiator;
    uint64_t active_lane_mask;
    uint64_t suspend;
    const char *lpbk_mode;
    const char *fec_mode;
    const char *encap;
    const char *custom_speed_vco;
} pc_port_data_t;

typedef struct pc_port_field_s {
    uint64_t lport;
    uint32_t set_field_bmp;
    pc_port_data_t *lport_data;
} pc_port_field_t;

/* PC_PORT_PHYS_MAP table field. */
#define PC_PORT_PHYS_MAP_PPORT (1 << 0)

typedef struct pc_port_phys_map_data_s {
    uint64_t pport;
} pc_port_phys_map_data_t;

typedef struct pc_port_phys_map_field_s {
    uint64_t lport;
    uint32_t set_field_bmp;
    pc_port_phys_map_data_t *lport_map_data;
} pc_port_phys_map_field_t;

/* PC_PORT_STATUS table field. */
typedef struct pc_port_status_data_s {
    uint64_t phy_disabled;
    uint64_t mac_disabled;
    uint64_t autoneg;
    uint64_t link_training;
    uint64_t link_training_done;
    uint64_t speed;
    uint64_t num_lanes;
    uint64_t pmd_rx_lock;
    uint64_t failover_lb;
    uint64_t autoneg_done;
    uint64_t local_fault;
    uint64_t remote_fault;
    const char *loopback;
    const char *fec_mode;
    const char *encap;
    const char *rlm_status;
} pc_port_status_data_t;

typedef struct pc_port_status_field_s {
    uint64_t lport;
    pc_port_status_data_t *lport_status_data;
} pc_port_status_field_t;

/* PC_PHYS_PORT table field. */
typedef struct pc_phys_port_data_s {
    uint64_t pm_id;
    uint64_t pm_pport_id;
} pc_phys_port_data_t;

typedef struct pc_phys_port_field_s {
    uint64_t pport;
    pc_phys_port_data_t *pport_data;
} pc_phys_port_field_t;

/* PC_AUTONEG_PROFILE table field. */
typedef struct pc_autoneg_profile_data_s {
    uint64_t long_ch;
    uint64_t num_lanes;
    uint64_t link_training_off;
    uint64_t advert_speed;
    const char* autoneg_mode;
    const char* medium_type;
    const char* fec_mode;
    const char* pause_type;
} pc_autoneg_profile_data_t;

/*PC_TX_TAPS fields*/
#define PC_TX_FIR_PRE                (1 << 0)
#define PC_TX_FIR_PRE2               (1 << 1)
#define PC_TX_FIR_MAIN               (1 << 2)
#define PC_TX_FIR_POST               (1 << 3)
#define PC_TX_FIR_POST2              (1 << 4)
#define PC_TX_FIR_POST3              (1 << 5)
#define PC_TX_FIR_RPARA              (1 << 6)
#define PC_TX_FIR_AMP                (1 << 7)
#define PC_TX_FIR_DRV_MODE           (1 << 8)
#define PC_TX_FIR_TAP_MODE           (1 << 9)
#define PC_TX_FIR_SIG_MODE           (1 << 10)
#define PC_TX_FIR_PRE_AUTO           (1 << 11)
#define PC_TX_FIR_PRE2_AUTO          (1 << 12)
#define PC_TX_FIR_MAIN_AUTO          (1 << 13)
#define PC_TX_FIR_POST_AUTO          (1 << 14)
#define PC_TX_FIR_POST2_AUTO         (1 << 15)
#define PC_TX_FIR_POST3_AUTO         (1 << 16)
#define PC_TX_FIR_RPARA_AUTO         (1 << 17)
#define PC_TX_FIR_AMP_AUTO           (1 << 18)
#define PC_TX_FIR_DRV_MODE_AUTO      (1 << 19)
#define PC_TX_FIR_TAP_MODE_AUTO      (1 << 20)
#define PC_TX_FIR_SIG_MODE_AUTO      (1 << 21)

typedef struct pc_tx_taps_data_s {
    uint64_t tx_pre_auto;
    uint64_t tx_pre;
    uint64_t tx_pre_sign;
    uint64_t tx_pre2_auto;
    uint64_t tx_pre2;
    uint64_t tx_pre2_sign;
    uint64_t tx_main_auto;
    uint64_t tx_main;
    uint64_t tx_main_sign;
    uint64_t tx_post_auto;
    uint64_t tx_post;
    uint64_t tx_post_sign;
    uint64_t tx_post2_atuo;
    uint64_t tx_post2;
    uint64_t tx_post2_sign;
    uint64_t tx_post3_auto;
    uint64_t tx_post3;
    uint64_t tx_post3_sign;
    uint64_t tx_rpara_auto;
    uint64_t tx_rpara;
    uint64_t tx_rpara_sign;
    uint64_t tx_amp_auto;
    uint64_t tx_amp;
    uint64_t tx_amp_sign;
    uint64_t tx_drv_mode_auto;
    uint64_t tx_drv_mode;
    uint64_t tx_drv_mode_sign;
    uint64_t tx_tap_mode_auto;
    uint64_t tx_sig_mode_auto;
    const char *tx_tap_mode;
    const char *tx_sig_mode;
} pc_tx_taps_data_t;

typedef struct pc_tx_taps_field_s {
    uint64_t lport;
    uint64_t lane_idx;
    uint32_t field_bmp;
    pc_tx_taps_data_t* tx_taps_data;
} pc_tx_taps_field_t;

/*PC_PHY_CONTROL_CONFIG fields*/
#define PC_TX_SQUELCH_AUTO                          (1ULL << 0)
#define PC_TX_SQUELCH                               (1ULL << 1)
#define PC_RX_SQUELCH_AUTO                          (1ULL << 2)
#define PC_RX_SQUELCH                               (1ULL << 3)
#define PC_RX_ADAPTATION_RESUME_AUTO                (1ULL << 4)
#define PC_RX_ADAPTATION_RESUME                     (1ULL << 5)
#define PC_RX_AFE_VGA_AUTO                          (1ULL << 6)
#define PC_RX_AFE_VGA                               (1ULL << 7)
#define PC_RX_AFE_PEAKING_FILTER_AUTO               (1ULL << 8)
#define PC_RX_AFE_PEAKING_FILTER                    (1ULL << 9)
#define PC_RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTO      (1ULL << 10)
#define PC_RX_AFE_LOW_FREQ_PEAKING_FILTER           (1ULL << 11)
#define PC_RX_AFE_HIGH_FREQ_PEAKING_FILTER_AUTO     (1ULL << 12)
#define PC_RX_AFE_HIGH_FREQ_PEAKING_FILTER          (1ULL << 13)
#define PC_RX_AFE_DFE_TAP_AUTO                      (1ULL << 14)
#define PC_RX_AFE_DFE_TAPS                          (1ULL << 15)
#define PC_RX_AFE_DFE_TAP1                          (1ULL << 16)
#define PC_RX_AFE_DFE_TAP2                          (1ULL << 17)
#define PC_RX_AFE_DFE_TAP3                          (1ULL << 18)
#define PC_RX_AFE_DFE_TAP4                          (1ULL << 19)
#define PC_RX_AFE_DFE_TAP5                          (1ULL << 20)
#define PC_PAM4_TX_PATTERN_AUTO                     (1ULL << 21)
#define PC_PAM4_TX_PATTERN                          (1ULL << 22)
#define PC_PAM4_TX_PRECODER_AUTO                    (1ULL << 23)
#define PC_PAM4_TX_PRECODER                         (1ULL << 24)
#define PC_PHY_ECC_INTR_ENABLE_AUTO                 (1ULL << 25)
#define PC_PHY_ECC_INTR_ENABLE                      (1ULL << 26)
#define PC_TX_PI_FREQ_OVERRIDE_AUTO                 (1ULL << 27)
#define PC_TX_PI_FREQ_OVERRIDE                      (1ULL << 28)
#define PC_FEC_BYPASS_INDICATION_AUTO               (1ULL << 29)
#define PC_FEC_BYPASS_INDICATION                    (1ULL << 30)
#define PC_PMD_DEBUG_LANE_EVENT_LOG_LEVEL           (1ULL << 31)

typedef struct pc_phy_control_data_s {
    uint64_t tx_squelch_auto;
    uint64_t tx_squelch;
    uint64_t rx_squelch_auto;
    uint64_t rx_squelch;
    uint64_t rx_adaption_resume_auto;
    uint64_t rx_adaption_resume;
    uint64_t rx_afe_vga_auto;
    uint64_t rx_afe_vga;
    uint64_t rx_afe_pf_auto;
    uint64_t rx_afe_pf;
    uint64_t rx_afe_lf_pf_auto;
    uint64_t rx_afe_lf_pf;
    uint64_t rx_afe_hf_pf_auto;
    uint64_t rx_afe_hf_pf;
    uint64_t rx_afe_dfe_tap_sign[NUM_RX_DFE_TAPS];
    uint64_t rx_afe_dfe_tap_auto;
    uint64_t rx_afe_dfe_tap[NUM_RX_DFE_TAPS];
    uint32_t rx_afe_dfe_num;
    uint64_t rx_signal_detect;
    uint64_t pam4_tx_precoder_auto;
    uint64_t pam4_tx_precoder;
    uint64_t phy_ecc_intr_enable_auto;
    uint64_t phy_ecc_intr_enable;
    uint64_t tx_pi_freq_override_sign;
    uint64_t tx_pi_freq_override_auto;
    uint64_t tx_pi_freq_override;
    uint64_t fec_bypass_indication_auto;
    uint64_t fec_bypass_indication;
    uint64_t pam4_tx_pattern_auto;
    uint64_t pmd_debug_lane_event_log_level;
    const char *pam4_tx_pattern;
} pc_phy_control_data_t;

typedef struct pc_phy_control_field_s {
    uint64_t lport;
    uint64_t lane_idx;
    uint64_t field_bmp;
    pc_phy_control_data_t* phy_ctrl_data;
} pc_phy_control_field_t;


/*PC_PMD_FIRMWARE fields*/
#define PC_DFE_AUTO                     (1 << 0)
#define PC_DFE_ON                       (1 << 1)
#define PC_LP_DFE_AUTO                  (1 << 2)
#define PC_LP_DFE_ON                    (1 << 3)
#define PC_MEDIUM_TYPE_AUTO             (1 << 4)
#define PC_PMD_LANE_MEDIUM_TYPE         (1 << 5)
#define PC_CL72_RESTART_TO_EN_AUTO      (1 << 6)
#define PC_Cl72_RESTART_TO_EN           (1 << 7)
#define PC_LP_TX_PRECODER_ON_AUTO       (1 << 8)
#define PC_LP_TX_PRECODER_ON            (1 << 9)
#define PC_UNRELIABLE_LOS_AUTO          (1 << 10)
#define PC_UNRELIABLE_LOS               (1 << 11)
#define PC_SCRAMBLING_ENABLE_AUTO       (1 << 12)
#define PC_SCRAMBLING_ENABLE            (1 << 13)
#define PC_NS_PAM4_AUTO                 (1 << 14)
#define PC_NS_PAM4                      (1 << 15)
#define PC_ES_PAM4_AUTO                 (1 << 16)
#define PC_ES_PAM4                      (1 << 17)

#define PC_PMD_FW_CFG_FLD_PBMP          (PC_DFE_ON|PC_LP_DFE_ON|PC_PMD_LANE_MEDIUM_TYPE|PC_Cl72_RESTART_TO_EN   \
                                         |PC_LP_TX_PRECODER_ON|PC_UNRELIABLE_LOS|PC_SCRAMBLING_ENABLE|PC_NS_PAM4|PC_ES_PAM4)
#define PC_PMD_FW_AUTO_FLD_PBMP         (PC_DFE_AUTO|PC_LP_DFE_AUTO|PC_MEDIUM_TYPE_AUTO|PC_CL72_RESTART_TO_EN_AUTO|PC_LP_TX_PRECODER_ON_AUTO    \
                                         |PC_UNRELIABLE_LOS_AUTO|PC_SCRAMBLING_ENABLE_AUTO|PC_NS_PAM4_AUTO|PC_ES_PAM4_AUTO)

typedef struct pc_pmd_fw_data_s {
    uint64_t dfe_auto;
    uint64_t dfe_on;
    uint64_t lp_dfe_auto;
    uint64_t lp_dfe_on;
    uint64_t cl72_RestTO_EN_auto;
    uint64_t cl72_RestTO_EN;
    uint64_t lp_tx_precoder_on_auto;
    uint64_t lp_tx_precoder_on;
    uint64_t unreliable_los_auto;
    uint64_t unreliable_los;
    uint64_t scrambling_en_auto;
    uint64_t scrambling_en;
    uint64_t ns_pam4_auto;
    uint64_t ns_pam4;
    uint64_t es_pam4_auto;
    uint64_t es_pam4;
    uint64_t medium_type_auto;
    const char *medium_type;
} pc_pmd_fw_data_t;

typedef struct pc_pmd_fw_field_s {
    uint64_t lport;
    uint32_t field_bmp;
    pc_pmd_fw_data_t* pmd_fw_data;
} pc_pmd_fw_field_t;

/*PC_MAC_CONTROL table field*/
#define PC_OVERSIZE_PKT                         (1ULL << 0)
#define PC_RX_ENABLE_AUTO                       (1ULL << 1)
#define PC_RX_ENABLE                            (1ULL << 2)
#define PC_TX_ENABLE_AUTO                       (1ULL << 3)
#define PC_TX_ENABLE                            (1ULL << 4)
#define PC_LOCAL_FAULT_DISABLE                  (1ULL << 5)
#define PC_REMOTE_FAULT_DISABLE                 (1ULL << 6)
#define PC_INTER_FRAME_GAP_AUTO                 (1ULL << 7)
#define PC_INTER_FRAME_GAP                      (1ULL << 8)
#define PC_PAUSE_TX                             (1ULL << 9)
#define PC_PAUSE_RX                             (1ULL << 10)
#define PC_PAUSE_ADDR                           (1ULL << 11)
#define PC_PAUSE_PASS                           (1ULL << 12)
#define PC_CONTROL_PASS                         (1ULL << 13)
#define PC_RUNT_THRESHOLD_AUTO                  (1ULL << 14)
#define PC_RUNT_THRESHOLD                       (1ULL << 15)
#define PC_MAC_ECC_INTR_ENABLE                  (1ULL << 16)
#define PC_RX_ENABLE_OPER                       (1ULL << 17)
#define PC_TX_ENABLE_OPER                       (1ULL << 18)
#define PC_INTER_FRAME_GAP_OPER                 (1ULL << 19)
#define PC_RUNT_THRESHOLD_OPER                  (1ULL << 20)
#define PC_PAUSE_RX_OPER                        (1ULL << 21)
#define PC_PAUSE_TX_OPER                        (1ULL << 22)
#define PC_OPERATIONAL_STATE                    (1ULL << 23)
#define PC_STALL_TX                             (1ULL << 24)
#define PC_STALL_TX_OPER                        (1ULL << 25)
#define PC_PURGE_DRIBBLE_NIBBLE_ERROR_FRAMES    (1ULL << 26)
#define PC_PURGE_MACSEC_FRAMES                  (1ULL << 27)
#define PC_PURGE_SCH_CRC_ERROR                  (1ULL << 28)
#define PC_PURGE_PFC_FRAMES                     (1ULL << 29)
#define PC_PURGE_RUNT_FRAMES                    (1ULL << 30)
#define PC_RX_FIFO_FULL                         (1ULL << 31)
#define PC_PURGE_UNICAST_FRAMES                 (1ULL << 32)
#define PC_PURGE_VLAN_TAGGED_FRAMES             (1ULL << 33)
#define PC_PURGE_BAD_OPCODE_FRAMES              (1ULL << 34)
#define PC_PURGE_PAUSE_FRAMES                   (1ULL << 35)
#define PC_PURGE_CONTROL_FRAMES                 (1ULL << 36)
#define PC_PURGE_PROMISCUOUS_FRAMES             (1ULL << 37)
#define PC_PURGE_BROADCAST_FRAMES               (1ULL << 38)
#define PC_PURGE_MULTICAST_FRAMES               (1ULL << 39)
#define PC_PURGE_GOOD_FRAMES                    (1ULL << 40)
#define PC_PURGE_TRUNCATED_FRAMES               (1ULL << 41)
#define PC_PURGE_LENGTH_CHECK_FAIL_FRAMES       (1ULL << 42)
#define PC_PURGE_CRC_ERROR_FRAMES               (1ULL << 43)
#define PC_PURGE_RX_CODE_ERROR_FRAMES           (1ULL << 44)
#define PC_PURGE_UNSUPPORTED_PAUSE_PFC_DA       (1ULL << 45)
#define PC_PURGE_STACK_VLAN_FRAMES              (1ULL << 46)
#define PC_PURGE_WRONG_SA                       (1ULL << 47)

typedef struct pc_mac_control_data_s {
    uint64_t oversize_pkt;
    uint64_t rx_enable_auto;
    uint64_t rx_enable;
    uint64_t tx_enable_auto;
    uint64_t tx_enable;
    uint64_t local_fault_disable;
    uint64_t remote_fault_disable;
    uint64_t inter_frame_gap_auto;
    uint64_t inter_frame_gap;
    uint64_t pause_tx;
    uint64_t pause_rx;
    uint64_t pause_addr;
    uint64_t pause_pass;
    uint64_t control_pass;
    uint64_t runt_threshold_auto;
    uint64_t runt_threshold;
    uint64_t mac_ecc_intr_enable;
    uint64_t rx_enable_oper;
    uint64_t tx_enable_oper;
    uint64_t inter_frame_gap_oper;
    uint64_t runt_threshold_oper;
    uint64_t pause_rx_oper;
    uint64_t pause_tx_oper;
    uint64_t oper_state;
    uint64_t stall_tx;
    uint64_t stall_tx_oper;
    uint64_t purge_dribble_nibble_error;
    uint64_t purge_macsec_frames;
    uint64_t purge_sch_crc_frames;
    uint64_t purge_pfc_frames;
    uint64_t purge_runt_frames;
    uint64_t rx_fifo_full;
    uint64_t purge_unicast_frames;
    uint64_t purge_vlan_tagged_frames;
    uint64_t purge_bad_opcode_frames;
    uint64_t purge_pause_frames;
    uint64_t purge_control_frames;
    uint64_t purge_promiscuous_frames;
    uint64_t purge_broadcast_frames;
    uint64_t purge_multicast_frames;
    uint64_t purge_good_frames;
    uint64_t purge_truncated_frames;
    uint64_t purge_length_check_fail;
    uint64_t purge_crc_code_error;
    uint64_t purge_rx_code_error;
    uint64_t purge_unsupported_pause_pfc_da;
    uint64_t purge_statck_vlan_frames;
    uint64_t purge_wrong_sa;
} pc_mac_control_data_t;

typedef struct pc_mac_control_field_s {
    uint64_t lport;
    uint64_t set_field_bmp;
    pc_mac_control_data_t *mac_control_data;
} pc_mac_control_field_t;

/* PC_PFC table field. */
#define PC_PFC_XOFF_TIMER       (1 << 0)
#define PC_PFC_OPCODE           (1 << 1)
#define PC_PFC_ETH_TYPE         (1 << 2)
#define PC_PFC_DEST_ADDR        (1 << 3)
#define PC_PFC_REFRESH_TIMER    (1 << 4)
#define PC_PFC_ENABLE_STATS     (1 << 5)
#define PC_PFC_ENABLE_TX        (1 << 6)
#define PC_PFC_ENABLE_RX        (1 << 7)
#define PC_PFC_PASS             (1 << 8)

typedef struct pc_pfc_data_s {
    uint64_t xoff_timer;
    uint64_t opcode;
    uint64_t eth_type;
    uint64_t dest_addr;
    uint64_t refresh_timer;
    uint64_t enable_stats;
    uint64_t enable_tx;
    uint64_t enable_rx;
    uint64_t pass;
} pc_pfc_data_t;

typedef struct pc_pfc_field_s {
    uint64_t lport;
    uint32_t set_field_bmp;
    pc_pfc_data_t *pfc_data;
} pc_pfc_field_t;

/* PM_CORE table field. */
#define PC_PMD_COM_CLK              (1 << 0)
#define PC_TX_LANE_MAP_AUTO         (1 << 1)
#define PC_TX_LANE_MAP              (1 << 2)
#define PC_TX_LANE_MAP_OPER         (1 << 3)
#define PC_RX_LANE_MAP_AUTO         (1 << 4)
#define PC_RX_LANE_MAP              (1 << 5)
#define PC_RX_LANE_MAP_OPER         (1 << 6)
#define PC_TX_POLARITY_FLIP_AUTO    (1 << 7)
#define PC_TX_POLARITY_FLIP         (1 << 8)
#define PC_TX_POLARITY_FLIP_OPER    (1 << 9)
#define PC_RX_POLARITY_FLIP_AUTO    (1 << 10)
#define PC_RX_POLARITY_FLIP         (1 << 11)
#define PC_RX_POLARITY_FLIP_OPER    (1 << 12)
#define PC_PM_MODE                  (1 << 13)

typedef struct pc_pm_core_data_s {
    uint64_t pmd_com_clk;
    uint64_t tx_lane_map_auto;
    uint64_t tx_lane_map;
    uint64_t tx_lane_map_oper;
    uint64_t rx_lane_map_auto;
    uint64_t rx_lane_map;
    uint64_t rx_lane_map_oper;
    uint64_t tx_polarity_filp_auto;
    uint64_t tx_polarity_filp;
    uint64_t tx_polarity_filp_oper;
    uint64_t rx_polarity_filp_auto;
    uint64_t rx_polarity_filp;
    uint64_t rx_polarity_filp_oper;
    const char *pm_core;
} pc_pm_core_data_t;

typedef struct pc_pm_core_field_s {
    uint64_t pm_id;
    uint64_t core_index;
    uint32_t field_bmp;
    pc_pm_core_data_t *pm_core_data;
} pc_pm_core_field_t;

typedef struct pc_profile_field_s {
    uint64_t profile_id;
    void* profile_data;
} pc_profile_field_t;

#define PC_PORT_ABILITY_NUM_MAX  PC_MAX_ABILITIES

typedef struct pc_port_ability_data_s {
    uint64_t num_abilities;
    uint64_t speed[PC_PORT_ABILITY_NUM_MAX];
    uint64_t num_lane[PC_PORT_ABILITY_NUM_MAX];
    const char* fec_mode[PC_PORT_ABILITY_NUM_MAX];
    const char* autoneg_mode[PC_PORT_ABILITY_NUM_MAX];
    const char* medium_type[PC_PORT_ABILITY_NUM_MAX];
    const char* pause_type[PC_PORT_ABILITY_NUM_MAX];
    const char* channel_type[PC_PORT_ABILITY_NUM_MAX];
} pc_port_ability_data_t;

typedef struct pc_port_ability_field_s {
    uint64_t lport;
    const char* ability_type;
    pc_port_ability_data_t* port_ability_data;
} pc_port_ability_field_t;

typedef struct pc_pm_prop_data_s {
    uint64_t num_lanes;
    uint64_t num_ports;
    uint64_t num_aggr;
    uint64_t num_pll;
    uint64_t tvco_index;
    const char* pm_type;
    const char* vco_rate[LTSW_PM_MAX_PLL_NUM];
} pc_pm_prop_data_t;

typedef struct pc_pm_prop_field_s {
    uint64_t pm_id;
    pc_pm_prop_data_t *pm_prop_data;
} pc_pm_prop_field_t;

/*PC_PORT_TIMESYNC table field*/
#define PC_IEEE_1588                (1 << 0)
#define PC_ONE_STEP_TIMESTAMP       (1 << 1)
#define PC_IS_SOP                   (1 << 2)
#define PC_TS_COMP_MODE             (1 << 3)
#define PC_STAGE_0_MODE             (1 << 4)
#define PC_STAGE_1_MODE             (1 << 5)
#define PC_FRACTIONAL_DIVISOR       (1 << 6)

typedef struct pc_port_timesync_data_s {
    uint64_t ieee_1588;
    uint64_t one_step_timestamp;
    uint64_t is_sop;
    uint64_t fractional_divisor;
    const char* ts_comp_mode;
    const char* stage_0_mode;
    const char* stage_1_mode;
} pc_port_timesync_data_t;

typedef struct pc_port_timesync_field_s {
    uint64_t lport;
    uint32_t field_bmp;
    pc_port_timesync_data_t* timesync_data;
} pc_port_timesync_field_t;

typedef struct pc_port_diag_stats_data_s {
    uint64_t fec_correct_cws;
    uint64_t fec_uncorrect_cws;
    uint64_t fec_symb_errs;
} pc_port_diag_stats_data_t;

typedef struct pc_port_diag_stats_field_s {
    uint64_t lport;
    pc_port_diag_stats_data_t* port_diag_stats_data;
} pc_port_diag_stats_field_t;

/* PC_FDR_CONTROL table field */
#define PC_FDR_ENABLE                    (1 << 0)
#define PC_FDR_SYMBOL_ERROR_WINDOW       (1 << 1)
#define PC_FDR_SYMBOL_ERROR_THRESHOLD    (1 << 2)
#define PC_FDR_SYMBOL_ERROR_START_VALUE  (1 << 3)
#define PC_FDR_INTERRUPT_ENABLE          (1 << 4)

typedef struct pc_fdr_control_data_s {
    uint64_t fdr_enable;
    uint64_t symb_err_thres;
    uint64_t symb_err_start_val;
    uint64_t inter_enable;
    const char* symb_err_win;
} pc_fdr_control_data_t;

typedef struct pc_fdr_control_field_s {
    uint64_t lport;
    uint32_t field_bmp;
    pc_fdr_control_data_t *fdr_ctrl_data;
} pc_fdr_control_field_t;

typedef struct pc_fdr_stats_data_s {
    uint64_t start_time;
    uint64_t end_time;
    uint64_t num_uncorrect_cws;
    uint64_t num_cws;
    uint64_t num_symb_errs;
    uint64_t cw_s0_errs;
    uint64_t cw_s1_errs;
    uint64_t cw_s2_errs;
    uint64_t cw_s3_errs;
    uint64_t cw_s4_errs;
    uint64_t cw_s5_errs;
    uint64_t cw_s6_errs;
    uint64_t cw_s7_errs;
    uint64_t cw_s8_errs;
} pc_fdr_stats_data_t;

typedef struct pc_fdr_stats_field_s {
    uint64_t lport;
    uint32_t field_bmp;
    pc_fdr_stats_data_t* fdr_stats_data;
} pc_fdr_stats_field_t;

static shr_enum_map_t vco_rate_map[] = {
    {(char *)PC_PM_PLL_VCO_RATE_NONEs,      ltswPortVCONone},
    {(char *)PC_PM_PLL_VCO_RATE_12P5Gs,     ltswPortVCO12P5},
    {(char *)PC_PM_PLL_VCO_RATE_20P625Gs,   ltswPortVCO20P625G},
    {(char *)PC_PM_PLL_VCO_RATE_25P781Gs,   ltswPortVCO25P781G},
    {(char *)PC_PM_PLL_VCO_RATE_26P562Gs,   ltswPortVCO26P562G},
    {(char *)PC_PM_PLL_VCO_RATE_10P3125Gs,  ltswPortVCO10P3125},
    {(char *)PC_PM_PLL_VCO_RATE_41P25Gs, ltswPortVCO41P25G},
    {(char *)PC_PM_PLL_VCO_RATE_51P562Gs, ltswPortVCO51P562G},
    {(char *)PC_PM_PLL_VCO_RATE_53P125Gs, ltswPortVCO53P125G}
};

static shr_enum_map_t pm_type_map[] = {
    {(char *)PC_PM_TYPE_NONEs,         ltswPmTypeNone},
    {(char *)PC_PM_TYPE_CPUs,          ltswPmTypeCpu},
    {(char *)PC_PM_TYPE_LOOPBACKs,     ltswPmTypeLoopback},
    {(char *)PC_PM_TYPE_PM4X10s,       ltswPmTypePm4x10},
    {(char *)PC_PM_TYPE_PM4X25s,       ltswPmTypePm4x25},
    {(char *)PC_PM_TYPE_PM8X50s,       ltswPmTypePm8x50},
    {(char *)PC_PM_TYPE_PM8X50_GEN2s,  ltswPmTypePm8x50Gen2},
    {(char *)PC_PM_TYPE_PM8X50_GEN3s,  ltswPmTypePm8x50Gen3},
    {(char *)PC_PM_TYPE_PMQTCs,        ltswPmTypePmqtc},
    {(char *)PC_PM_TYPE_PMSGMII4PX2s,  ltswPmTypePmsgmii4px2},
    {(char *)PC_PM_TYPE_PM8X100s,      ltswPmTypePm8x100}
};

static shr_enum_map_t custom_speed_vco_map[] = {
    {(char *)PC_SPEED_VCO_NONEs,            ltswSpeedVcoNone},
    {(char *)PC_SPEED_1G_AT_6P25G_VCOs,     ltswSpeed1gAt6p25g},
    {(char *)PC_SPEED_1G_AT_12P5G_VCOs,     ltswSpeed1gAt12p5g},
    {(char *)PC_SPEED_1G_AT_25P781G_VCOs,   ltswSpeed1gAt25p781g},
    {(char *)PC_SPEED_2P5G_AT_12P5G_VCOs,   ltswSpeed2p5gAt12p5g},
    {(char *)PC_SPEED_5G_AT_12P5G_VCOs,     ltswSpeed5gAt12p5g}
};

static shr_enum_map_t lpbk_mode_map[] = {
    {(char *)PC_LPBK_MACs, BCM_PORT_LOOPBACK_MAC},
    {(char *)PC_LPBK_PMDs, BCM_PORT_LOOPBACK_PHY},
    {(char *)PC_LPBK_PCSs, BCM_PORT_LOOPBACK_PHY},
    {(char *)PC_LPBK_REMOTE_PMDs, BCM_PORT_LOOPBACK_PHY_REMOTE},
    {(char *)PC_LPBK_NONEs, BCM_PORT_LOOPBACK_NONE}
};

static shr_enum_map_t fec_mode_map[] = {
    {(char *)PC_FEC_NONEs, bcmPortPhyFecNone},
    {(char *)PC_FEC_BASE_Rs, bcmPortPhyFecBaseR},
    {(char *)PC_FEC_RS528s, bcmPortPhyFecRsFec},
    {(char *)PC_FEC_RS544s, bcmPortPhyFecRs544},
    {(char *)PC_FEC_RS272s, bcmPortPhyFecRs272},
    {(char *)PC_FEC_RS544_2XNs, bcmPortPhyFecRs544_2xN},
    {(char *)PC_FEC_RS272_2XNs, bcmPortPhyFecRs272_2xN}
};

static shr_enum_map_t medium_type_map[] = {
    {(char *)PC_PHY_MEDIUM_BACKPLANEs, BCM_PORT_MEDIUM_BACKPLANE},
    {(char *)PC_PHY_MEDIUM_COPPERs,    BCM_PORT_MEDIUM_COPPER},
    {(char *)PC_PHY_MEDIUM_OPTICALs,   BCM_PORT_MEDIUM_FIBER},
    {(char *)PC_PHY_MEDIUM_ALLs, BCM_PORT_MEDIUM_ALL},
};

static shr_enum_map_t lane_config_medium_type_map[] = {
    {(char *)PC_PHY_MEDIUM_BACKPLANEs, BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE },
    {(char *)PC_PHY_MEDIUM_COPPERs,    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE},
    {(char *)PC_PHY_MEDIUM_OPTICALs,   BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS},
};


static shr_enum_map_t pause_type_map[] = {
    {(char *)PC_PAUSE_NONEs,  bcmPortPhyPauseNone},
    {(char *)PC_PAUSE_TXs,    bcmPortPhyPauseTx},
    {(char *)PC_PAUSE_RXs,    bcmPortPhyPauseRx},
    {(char *)PC_PAUSE_SYMMs,  bcmPortPhyPauseSymm},
    {(char *)PC_PAUSE_ALLs,   bcmPortPhyPauseALL}
};

static shr_enum_map_t tx_tap_mode_map[] = {
    {(char *)PC_TXFIR_NRZ_LP_TAPS_3s,  bcmPortPhyTxTapMode3Tap},
    {(char *)PC_TXFIR_NRZ_TAPS_6s,     bcmPortPhyTxTapMode6Tap},
    {(char *)PC_TXFIR_PAM4_LP_TAPS_3s, bcmPortPhyTxTapMode3Tap},
    {(char *)PC_TXFIR_PAM4_TAPS_6s,    bcmPortPhyTxTapMode6Tap},
};

static shr_enum_map_t phy_sig_mode_map[] = {
    {(char *)PC_SIG_MODE_NRZs,   bcmPortPhySignallingModeNRZ},
    {(char *)PC_SIG_MODE_PAM4s,  bcmPortPhySignallingModePAM4},
};

static shr_enum_map_t phy_an_mode_map[] = {
    {(char *)PC_PHY_AUTONEG_MODE_NONEs,   bcmPortAnModeNone},
    {(char *)PC_PHY_AUTONEG_MODE_CL37s,   bcmPortAnModeCL37},
    {(char *)PC_PHY_AUTONEG_MODE_SGMIIs,  bcmPortAnModeCL37},
    {(char *)PC_PHY_AUTONEG_MODE_CL37_BAMs,   bcmPortAnModeCL37BAM},
    {(char *)PC_PHY_AUTONEG_MODE_CL73s,   bcmPortAnModeCL73},
    {(char *)PC_PHY_AUTONEG_MODE_CL73_BAMs,   bcmPortAnModeCL73BAM},
    {(char *)PC_PHY_AUTONEG_MODE_MSAs,   bcmPortAnModeCL73MSA},
};

static shr_enum_map_t encap_type_map[] = {
    {(char *)PC_ENCAP_IEEEs, BCM_PORT_ENCAP_IEEE},
    {(char *)PC_ENCAP_HIGIGs, BCM_PORT_ENCAP_HIGIG},
    {(char *)PC_ENCAP_HIGIG3s, BCM_PORT_ENCAP_HIGIG3},
};

static shr_enum_map_t channel_type_map[] = {
    {(char *)PC_PHY_CHANNEL_SHORTs, bcmPortPhyChannelShort},
    {(char *)PC_PHY_CHANNEL_LONGs, bcmPortPhyChannelLong},
    {(char *)PC_PHY_CHANNEL_ALLs, bcmPortPhyChannelAll},
};

static shr_enum_map_t rlm_status_type_map[] = {
    {(char *)PC_RLM_DISABLEs, bcmPortRlmDisabled},
    {(char *)PC_RLM_BUSYs, bcmPortRlmBusy},
    {(char *)PC_RLM_DONEs, bcmPortRlmDone},
    {(char *)PC_RLM_FAILEDs, bcmPortRlmFailed},
};

static shr_enum_map_t ts_comp_mode_map[] = {
    {(char *)PC_TIMESYNC_COMP_NONEs, 0},
    {(char *)PC_TIMESYNC_COMP_EARLIEST_LANEs, 1},
    {(char *)PC_TIMESYNC_COMP_LATEST_LANEs, 2},
};

static shr_enum_map_t fdr_symb_err_window_size_map[] = {
    {(char *)SYMBOL_ERROR_WINDOW_CW_128s, bcmPortFdrSymbErrWindowCw128},
    {(char *)SYMBOL_ERROR_WINDOW_CW_1Ks, bcmPortFdrSymbErrWindowCw1K},
    {(char *)SYMBOL_ERROR_WINDOW_CW_8Ks, bcmPortFdrSymbErrWindowCw8K},
    {(char *)SYMBOL_ERROR_WINDOW_CW_64Ks, bcmPortFdrSymbErrWindowCw64K},
    {(char *)SYMBOL_ERROR_WINDOW_CW_512Ks, bcmPortFdrSymbErrWindowCw512K},
    {(char *)SYMBOL_ERROR_WINDOW_CW_4Ms, bcmPortFdrSymbErrWindowCw4M},
    {(char *)SYMBOL_ERROR_WINDOW_CW_32Ms, bcmPortFdrSymbErrWindowCw32M},
    {(char *)SYMBOL_ERROR_WINDOW_CW_256Ms, bcmPortFdrSymbErrWindowCw256M},
};

typedef enum pc_port_ability_type_e {
    PC_PORT_ABILITY_LOCAL = 0,
    PC_PORT_ABILITY_ADVERT,
} pc_port_ability_type_t;

static shr_enum_map_t pc_ability_type_map[] = {
    {(char *)PC_ABILITY_LOCALs,  PC_PORT_ABILITY_LOCAL},
    {(char *)PC_ABILITY_ADVERTs, PC_PORT_ABILITY_ADVERT},
};

static int ltsw_pc_port_status_get(int unit, int port,
                                   pc_port_status_data_t *status_data);

static int ltsw_pc_port_get(int unit, int port,
                                   pc_port_data_t * lport_data);

/*!
 * \brief Convert ID to name.
 *
 * \param [in] name_id_map Map of name and ID.
 * \param [in] map_len Length of name_id_map array.
 * \param [in] id ID which to find its name.
 *
 * \return The pointer to name and NULL otherwise.
 */
static const char*
id2name(const shr_enum_map_t *name_id_map, int map_len, int id)
{
    int idx;

    for (idx = 0; idx < map_len; idx++) {
        if (name_id_map[idx].val == id) {
            return name_id_map[idx].name;
        }
    }

    return NULL;
}

/*!
 * \brief Convert name to ID.
 *
 * \param [in] name_id_map Map of name and ID.
 * \param [in] map_len Length of name_id_map array.
 * \param [in] name Name which to find its ID.
 *
 * \return valid ID and -1 otherwise.
 */
static int
name2id(const shr_enum_map_t *name_id_map, int map_len, const char *name)
{
    int idx;

    for (idx = 0; idx < map_len; idx++) {
        if (sal_strcmp(name_id_map[idx].name, name) == 0) {
            return name_id_map[idx].val;
        }
    }

    return -1;
}

/*!
 * \brief Find the first matched port.
 *
 * \param [in] unit Unit number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 *
 * \return The first matched port, or -1 if no match.
 */
static int
first_port(int unit, uint32_t port_type)
{
    int port;
    for (port = 0; port < PORT_NUM_MAX(unit); port ++) {
        if (PORT(unit, port).port_type & port_type)
            return port;
    }
    return -1;
}

/*!
 * \brief Convert port speed to port type bitmap.
 *
 * \param [in] speed Port speed.
 * \param [in] encap Port encapsulation mode.
 * \param [out] port_type Port type bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
speed_to_port_type(int speed, int encap, uint32_t *port_type)
{
    *port_type &= ~(BCMI_LTSW_PORT_TYPE_ETH | BCMI_LTSW_PORT_TYPE_HG |
                    BCMI_LTSW_PORT_TYPE_GE | BCMI_LTSW_PORT_TYPE_XE |
                    BCMI_LTSW_PORT_TYPE_CE | BCMI_LTSW_PORT_TYPE_CDE);

    switch (encap) {
        case BCM_PORT_ENCAP_IEEE:
            *port_type |= BCMI_LTSW_PORT_TYPE_ETH;
            if (speed >= 200000) {
                *port_type |= BCMI_LTSW_PORT_TYPE_CDE;
            } else if (speed >= 100000) {
                *port_type |= BCMI_LTSW_PORT_TYPE_CE;
            } else if (speed >= 10000) {
                *port_type |= BCMI_LTSW_PORT_TYPE_XE;
            } else {
                *port_type |= BCMI_LTSW_PORT_TYPE_GE;
            }
            break;

        case BCM_PORT_ENCAP_HIGIG:
        case BCM_PORT_ENCAP_HIGIG3:
            *port_type |= BCMI_LTSW_PORT_TYPE_HG;
            break;

        default:
            return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}

static int
ltsw_pc_fdr_stats_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *frd_stats_field)
{
    pc_fdr_stats_field_t *field = (pc_fdr_stats_field_t*)frd_stats_field;
    pc_fdr_stats_data_t *fdr_stats_data = (pc_fdr_stats_data_t*)field->fdr_stats_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FDR_START_TIMEs, &fdr_stats_data->start_time));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FDR_END_TIMEs, &fdr_stats_data->end_time));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, UNCORRECTABLE_CODE_WORDSs, &fdr_stats_data->num_uncorrect_cws));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, CODE_WORDSs, &fdr_stats_data->num_cws));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SYMBOL_ERRORSs, &fdr_stats_data->num_symb_errs));

    if (field->field_bmp & PC_FDR_CW_S0_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S0_ERRORSs, &fdr_stats_data->cw_s0_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S1_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S1_ERRORSs, &fdr_stats_data->cw_s1_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S2_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S2_ERRORSs, &fdr_stats_data->cw_s2_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S3_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S3_ERRORSs, &fdr_stats_data->cw_s3_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S4_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S4_ERRORSs, &fdr_stats_data->cw_s4_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S5_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S5_ERRORSs, &fdr_stats_data->cw_s5_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S6_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S6_ERRORSs, &fdr_stats_data->cw_s6_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S7_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S7_ERRORSs, &fdr_stats_data->cw_s7_errs));
    }

    if (field->field_bmp & PC_FDR_CW_S8_ERR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CODE_WORD_S8_ERRORSs, &fdr_stats_data->cw_s8_errs));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_stats_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *fdr_stats_field)
{
    pc_fdr_stats_field_t *field = (pc_fdr_stats_field_t*)fdr_stats_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_stats_get(int unit, bcm_port_t lport, uint32_t field_bmp, pc_fdr_stats_data_t *fdr_stats_data)
{
    pc_fdr_stats_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = field_bmp;
    field.fdr_stats_data = fdr_stats_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_FDR_STATSs, &field,
                ltsw_pc_fdr_stats_field_entry_set,
                ltsw_pc_fdr_stats_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_pc_fdr_control_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *fdr_control_field)
{
    pc_fdr_control_field_t *field = (pc_fdr_control_field_t*)fdr_control_field;
    pc_fdr_control_data_t *fdr_control_data = (pc_fdr_control_data_t*)field->fdr_ctrl_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ENABLEs, &fdr_control_data->fdr_enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, SYMBOL_ERROR_WINDOWs, &fdr_control_data->symb_err_win));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SYMBOL_ERROR_THRESHOLDs, &fdr_control_data->symb_err_thres));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SYMBOL_ERROR_START_VALUEs, &fdr_control_data->symb_err_start_val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, INTERRUPT_ENABLEs, &fdr_control_data->inter_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_control_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *fdr_control_field)
{
    pc_fdr_control_field_t *field = (pc_fdr_control_field_t*)fdr_control_field;
    pc_fdr_control_data_t *fdr_control_data = (pc_fdr_control_data_t*)field->fdr_ctrl_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (fdr_control_data == NULL) {
        SHR_EXIT();
    }

    if (field->field_bmp & PC_FDR_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ENABLEs, fdr_control_data->fdr_enable));
    }

    if (field->field_bmp & PC_FDR_SYMBOL_ERROR_WINDOW) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, SYMBOL_ERROR_WINDOWs, fdr_control_data->symb_err_win));
    }

    if (field->field_bmp & PC_FDR_SYMBOL_ERROR_THRESHOLD) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SYMBOL_ERROR_THRESHOLDs, fdr_control_data->symb_err_thres));
    }

    if (field->field_bmp & PC_FDR_SYMBOL_ERROR_START_VALUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SYMBOL_ERROR_START_VALUEs, fdr_control_data->symb_err_start_val));
    }

    if (field->field_bmp & PC_FDR_INTERRUPT_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, INTERRUPT_ENABLEs, fdr_control_data->inter_enable));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_control_lookup(int unit, int lport)
{
    pc_fdr_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = 0;
    field.fdr_ctrl_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_lookup(unit, PC_FDR_CONTROLs, &field,
                          ltsw_pc_fdr_control_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_control_insert(int unit, int lport, pc_fdr_control_data_t *fdr_control_data, uint32_t set_field_bmp)
{
    pc_fdr_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = set_field_bmp;
    field.fdr_ctrl_data = fdr_control_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_FDR_CONTROLs, &field,
                             ltsw_pc_fdr_control_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_control_update(int unit, int lport, pc_fdr_control_data_t *fdr_control_data, uint32_t set_field_bmp)
{
    pc_fdr_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = set_field_bmp;
    field.fdr_ctrl_data = fdr_control_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PC_FDR_CONTROLs, &field,
                            ltsw_pc_fdr_control_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_control_set(int unit, int lport, pc_fdr_control_data_t *fdr_control_data, uint32_t set_field_bmp)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_fdr_control_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_fdr_control_insert(unit, lport, fdr_control_data, set_field_bmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_fdr_control_update(unit, lport, fdr_control_data, set_field_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_fdr_control_get(int unit, int lport, pc_fdr_control_data_t *fdr_control_data)
{
    pc_fdr_control_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_fdr_control_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_fdr_control_insert(unit, lport, fdr_control_data, 0));
    }

    field.lport = lport;
    field.field_bmp = 0;
    field.fdr_ctrl_data = fdr_control_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_FDR_CONTROLs, &field,
                           ltsw_pc_fdr_control_field_entry_set,
                           ltsw_pc_fdr_control_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_diag_stats_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *port_diag_stats_field)
{
    pc_port_diag_stats_field_t *field = (pc_port_diag_stats_field_t*)port_diag_stats_field;
    pc_port_diag_stats_data_t *port_diag_stats_data = (pc_port_diag_stats_data_t*)field->port_diag_stats_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FEC_CORRECTED_CODEWORDSs, &port_diag_stats_data->fec_correct_cws));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FEC_UNCORRECTED_CODEWORDSs, &port_diag_stats_data->fec_uncorrect_cws));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FEC_SYMBOL_ERRORSs, &port_diag_stats_data->fec_symb_errs));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_diag_stats_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *port_diag_stats_field)
{
    pc_port_diag_stats_field_t *field = (pc_port_diag_stats_field_t*)port_diag_stats_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_diag_stats_get(int unit, int port, pc_port_diag_stats_data_t *diag_stats_data)
{
    pc_port_diag_stats_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.port_diag_stats_data = diag_stats_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PORT_DIAG_STATSs, &field,
                           ltsw_pc_port_diag_stats_field_entry_set,
                           ltsw_pc_port_diag_stats_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_pc_port_timesync_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *port_timesync_field)
{
    pc_port_timesync_field_t *field = (pc_port_timesync_field_t*)port_timesync_field;
    pc_port_timesync_data_t *port_timesync_data = (pc_port_timesync_data_t*)field->timesync_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, IEEE_1588s, &port_timesync_data->ieee_1588));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ONE_STEP_TIMESTAMPs, &port_timesync_data->one_step_timestamp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, IS_SOPs, &port_timesync_data->is_sop));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FRACTIONAL_DIVISORs, &port_timesync_data->fractional_divisor));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, TS_COMP_MODEs, &port_timesync_data->ts_comp_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, STAGE_0_MODEs, &port_timesync_data->stage_0_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, STAGE_1_MODEs, &port_timesync_data->stage_1_mode));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_timesync_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *port_timesync_field)
{
    pc_port_timesync_field_t *field = (pc_port_timesync_field_t*)port_timesync_field;
    pc_port_timesync_data_t *port_timesync_data = (pc_port_timesync_data_t*)field->timesync_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (port_timesync_data == NULL) {
        SHR_EXIT();
    }

    if (field->field_bmp & PC_IEEE_1588) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, IEEE_1588s, port_timesync_data->ieee_1588));
    }

    if (field->field_bmp & PC_ONE_STEP_TIMESTAMP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ONE_STEP_TIMESTAMPs, port_timesync_data->one_step_timestamp));
    }

    if (field->field_bmp & PC_IS_SOP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, IS_SOPs, port_timesync_data->is_sop));
    }

    if (field->field_bmp & PC_FRACTIONAL_DIVISOR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, FRACTIONAL_DIVISORs, port_timesync_data->fractional_divisor));
    }

    if (field->field_bmp & PC_TS_COMP_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, TS_COMP_MODEs, port_timesync_data->ts_comp_mode));
    }

    if (field->field_bmp & PC_STAGE_0_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, STAGE_0_MODEs, port_timesync_data->stage_0_mode));
    }

    if (field->field_bmp & PC_STAGE_1_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, STAGE_1_MODEs, port_timesync_data->stage_1_mode));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_timesync_lookup(int unit, int lport)
{
    pc_port_timesync_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = 0;
    field.timesync_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_lookup(unit, PC_PORT_TIMESYNCs, &field,
                          ltsw_pc_port_timesync_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_timesync_insert(int unit, int lport, pc_port_timesync_data_t *timesync_data, uint32_t set_field_bmp)
{
    pc_port_timesync_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = set_field_bmp;
    field.timesync_data = timesync_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_PORT_TIMESYNCs, &field,
                             ltsw_pc_port_timesync_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_timesync_update(int unit, int lport, pc_port_timesync_data_t *timesync_data, uint32_t set_field_bmp)
{
    pc_port_timesync_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.field_bmp = set_field_bmp;
    field.timesync_data = timesync_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PC_PORT_TIMESYNCs, &field,
                            ltsw_pc_port_timesync_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_timesync_set(int unit, int lport, pc_port_timesync_data_t *timesync_data, uint32_t set_field_bmp)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_port_timesync_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_timesync_insert(unit, lport, timesync_data, set_field_bmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_timesync_update(unit, lport, timesync_data, set_field_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_timesync_get(int unit, int lport, pc_port_timesync_data_t *timesync_data)
{
    pc_port_timesync_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_port_timesync_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_timesync_insert(unit, lport, timesync_data, 0));
    }

    field.lport = lport;
    field.field_bmp = 0;
    field.timesync_data = timesync_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PORT_TIMESYNCs, &field,
                           ltsw_pc_port_timesync_field_entry_set,
                           ltsw_pc_port_timesync_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_ability_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *port_ability_field)
{
    pc_port_ability_field_t *field = (pc_port_ability_field_t*)port_ability_field;
    pc_port_ability_data_t *port_ability_data = (pc_port_ability_data_t*)field->port_ability_data;
    uint32_t vaild_num;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_ABILITIESs, &port_ability_data->num_abilities));

    if (port_ability_data->num_abilities == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(eh, SPEEDs, 0,
            &port_ability_data->speed[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(eh, NUM_LANESs, 0,
            &port_ability_data->num_lane[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, FEC_MODEs, 0,
                                       &port_ability_data->fec_mode[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, AUTONEG_MODEs, 0,
                                       &port_ability_data->autoneg_mode[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, MEDIUM_TYPEs, 0,
                                       &port_ability_data->medium_type[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, PAUSE_TYPEs, 0,
                                       &port_ability_data->pause_type[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, CHANNEL_TYPEs, 0,
                                       &port_ability_data->channel_type[0], PC_PORT_ABILITY_NUM_MAX, &vaild_num));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_ability_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *port_ability_field)
{
    pc_port_ability_field_t *field = (pc_port_ability_field_t*)port_ability_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, ABILITY_TYPEs, field->ability_type));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_ability_get(int unit, bcm_port_t lport, pc_port_ability_type_t type,
                                pc_port_ability_data_t *port_ability_data)
{
   pc_port_ability_field_t field;

   SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.ability_type = id2name(pc_ability_type_map, COUNTOF(pc_ability_type_map), type);
    SHR_NULL_CHECK(field.ability_type, SHR_E_PARAM);
    field.port_ability_data = port_ability_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PORT_ABILITIESs, &field,
                           ltsw_pc_port_ability_field_entry_set,
                           ltsw_pc_port_ability_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PFC entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PFC table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pfc_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                             void *pfc_field)
{
    pc_pfc_field_t *field = (pc_pfc_field_t*)pfc_field;
    pc_pfc_data_t *pfc_data = (pc_pfc_data_t*)field->pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, XOFF_TIMERs, &pfc_data->xoff_timer));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, OPCODEs, &pfc_data->opcode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ETH_TYPEs, &pfc_data->eth_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, DEST_ADDRs, &pfc_data->dest_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, REFRESH_TIMERs, &pfc_data->refresh_timer));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ENABLE_STATSs, &pfc_data->enable_stats));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ENABLE_TXs, &pfc_data->enable_tx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ENABLE_RXs, &pfc_data->enable_rx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PFC_PASSs, &pfc_data->pass));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PFC entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PFC table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pfc_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pfc_field)
{
    pc_pfc_field_t *field = (pc_pfc_field_t*)pfc_field;
    pc_pfc_data_t *pfc_data = (pc_pfc_data_t*)field->pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (pfc_data == NULL) {
        SHR_EXIT();
    }

    if (field->set_field_bmp & PC_PFC_XOFF_TIMER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, XOFF_TIMERs, pfc_data->xoff_timer));
    }

    if (field->set_field_bmp & PC_PFC_OPCODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, OPCODEs, pfc_data->opcode));
    }

    if (field->set_field_bmp & PC_PFC_ETH_TYPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ETH_TYPEs, pfc_data->eth_type));
    }

    if (field->set_field_bmp & PC_PFC_DEST_ADDR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DEST_ADDRs, pfc_data->dest_addr));
    }

    if (field->set_field_bmp & PC_PFC_REFRESH_TIMER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, REFRESH_TIMERs, pfc_data->refresh_timer));
    }

    if (field->set_field_bmp & PC_PFC_ENABLE_STATS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ENABLE_STATSs, pfc_data->enable_stats));
    }

    if (field->set_field_bmp & PC_PFC_ENABLE_TX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ENABLE_TXs, pfc_data->enable_tx));
    }

    if (field->set_field_bmp & PC_PFC_ENABLE_RX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ENABLE_RXs, pfc_data->enable_rx));
    }

    if (field->set_field_bmp & PC_PFC_PASS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PFC_PASSs, pfc_data->pass));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pfc_lookup(int unit, int lport)
{
    pc_pfc_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.set_field_bmp = 0;
    field.pfc_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_lookup(unit, PC_PFCs, &field,
                          ltsw_pc_pfc_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pfc_insert(int unit, int lport, pc_pfc_data_t *pfc_data, uint32_t set_field_bmp)
{
    pc_pfc_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.set_field_bmp = set_field_bmp;
    field.pfc_data = pfc_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_PFCs, &field,
                             ltsw_pc_pfc_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pfc_update(int unit, int lport, pc_pfc_data_t *pfc_data, uint32_t set_field_bmp)
{
    pc_pfc_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.set_field_bmp = set_field_bmp;
    field.pfc_data = pfc_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PC_PFCs, &field,
                            ltsw_pc_pfc_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pfc_set(int unit, int lport, pc_pfc_data_t *pfc_data, uint32_t set_field_bmp)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_pfc_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_insert(unit, lport, pfc_data, set_field_bmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_update(unit, lport, pfc_data, set_field_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pfc_get(int unit, int lport, pc_pfc_data_t *pfc_data)
{
    pc_pfc_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_pfc_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_insert(unit, lport, pfc_data, 0));
    }

    field.lport = lport;
    field.set_field_bmp = 0;
    field.pfc_data = pfc_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PFCs, &field,
                           ltsw_pc_pfc_field_entry_set,
                           ltsw_pc_pfc_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_MAC_CONTROL entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_MAC_CONTROL table name.
 * \param [in] eh Entry handle.
 * \param [in] mac_control_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_mac_control_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *mac_control_field)
{
    pc_mac_control_field_t *field = (pc_mac_control_field_t*)mac_control_field;
    pc_mac_control_data_t *mac_control_data = (pc_mac_control_data_t*)field->mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (mac_control_data == NULL) {
        SHR_EXIT();
    }

    if (field->set_field_bmp & PC_OVERSIZE_PKT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, OVERSIZE_PKTs, mac_control_data->oversize_pkt));
    }

    if (field->set_field_bmp & PC_RX_ENABLE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_ENABLE_AUTOs, mac_control_data->rx_enable_auto));
    }

    if (field->set_field_bmp & PC_RX_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_ENABLEs, mac_control_data->rx_enable));
    }

    if (field->set_field_bmp & PC_TX_ENABLE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_ENABLE_AUTOs, mac_control_data->tx_enable_auto));
    }

    if (field->set_field_bmp & PC_TX_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_ENABLEs, mac_control_data->tx_enable));
    }

    if (field->set_field_bmp & PC_LOCAL_FAULT_DISABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LOCAL_FAULT_DISABLEs, mac_control_data->local_fault_disable));
    }

    if (field->set_field_bmp & PC_REMOTE_FAULT_DISABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, REMOTE_FAULT_DISABLEs, mac_control_data->remote_fault_disable));
    }

    if (field->set_field_bmp & PC_INTER_FRAME_GAP_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, INTER_FRAME_GAP_AUTOs, mac_control_data->inter_frame_gap_auto));
    }

    if (field->set_field_bmp & PC_INTER_FRAME_GAP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, INTER_FRAME_GAPs, mac_control_data->inter_frame_gap));
    }

    if (field->set_field_bmp & PC_PAUSE_TX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAUSE_TXs, mac_control_data->pause_tx));
    }

    if (field->set_field_bmp & PC_PAUSE_RX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAUSE_RXs, mac_control_data->pause_rx));
    }

    if (field->set_field_bmp & PC_PAUSE_ADDR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAUSE_ADDRs, mac_control_data->pause_addr));
    }

    if (field->set_field_bmp & PC_PAUSE_PASS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAUSE_PASSs, mac_control_data->pause_pass));
    }

    if (field->set_field_bmp & PC_CONTROL_PASS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, CONTROL_PASSs, mac_control_data->control_pass));
    }

    if (field->set_field_bmp & PC_RUNT_THRESHOLD_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RUNT_THRESHOLD_AUTOs, mac_control_data->runt_threshold_auto));
    }

    if (field->set_field_bmp & PC_RUNT_THRESHOLD) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RUNT_THRESHOLDs, mac_control_data->runt_threshold));
    }

    if (field->set_field_bmp & PC_MAC_ECC_INTR_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MAC_ECC_INTR_ENABLEs, mac_control_data->mac_ecc_intr_enable));
    }

    if (field->set_field_bmp & PC_STALL_TX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, STALL_TXs, mac_control_data->stall_tx));
    }

    if (field->set_field_bmp & PC_PURGE_DRIBBLE_NIBBLE_ERROR_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_DRIBBLE_NIBBLE_ERROR_FRAMESs, mac_control_data->purge_dribble_nibble_error));
    }

    if (field->set_field_bmp & PC_PURGE_MACSEC_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_MACSEC_FRAMESs, mac_control_data->purge_macsec_frames));
    }

    if (field->set_field_bmp & PC_PURGE_SCH_CRC_ERROR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_SCH_CRC_ERRORs, mac_control_data->purge_sch_crc_frames));
    }

    if (field->set_field_bmp & PC_PURGE_PFC_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_PFC_FRAMESs, mac_control_data->purge_pfc_frames));
    }

    if (field->set_field_bmp & PC_PURGE_RUNT_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_RUNT_FRAMESs, mac_control_data->purge_runt_frames));
    }

    if (field->set_field_bmp & PC_RX_FIFO_FULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_FIFO_FULLs, mac_control_data->rx_fifo_full));
    }

    if (field->set_field_bmp & PC_PURGE_UNICAST_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_UNICAST_FRAMESs, mac_control_data->purge_unicast_frames));
    }

    if (field->set_field_bmp & PC_PURGE_VLAN_TAGGED_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_VLAN_TAGGED_FRAMESs, mac_control_data->purge_vlan_tagged_frames));
    }

    if (field->set_field_bmp & PC_PURGE_BAD_OPCODE_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_BAD_OPCODE_FRAMESs, mac_control_data->purge_bad_opcode_frames));
    }

    if (field->set_field_bmp & PC_PURGE_PAUSE_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_PAUSE_FRAMESs, mac_control_data->purge_pause_frames));
    }

    if (field->set_field_bmp & PC_PURGE_CONTROL_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_CONTROL_FRAMESs, mac_control_data->purge_control_frames));
    }

    if (field->set_field_bmp & PC_PURGE_PROMISCUOUS_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_PROMISCUOUS_FRAMESs, mac_control_data->purge_promiscuous_frames));
    }

    if (field->set_field_bmp & PC_PURGE_BROADCAST_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_BROADCAST_FRAMESs, mac_control_data->purge_broadcast_frames));
    }

    if (field->set_field_bmp & PC_PURGE_MULTICAST_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_MULTICAST_FRAMESs, mac_control_data->purge_multicast_frames));
    }

    if (field->set_field_bmp & PC_PURGE_GOOD_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_GOOD_FRAMESs, mac_control_data->purge_good_frames));
    }

    if (field->set_field_bmp & PC_PURGE_TRUNCATED_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_TRUNCATED_FRAMESs, mac_control_data->purge_truncated_frames));
    }

    if (field->set_field_bmp & PC_PURGE_LENGTH_CHECK_FAIL_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_LENGTH_CHECK_FAIL_FRAMESs, mac_control_data->purge_length_check_fail));
    }

    if (field->set_field_bmp & PC_PURGE_CRC_ERROR_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_CRC_ERROR_FRAMESs, mac_control_data->purge_crc_code_error));
    }

    if (field->set_field_bmp & PC_PURGE_RX_CODE_ERROR_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_RX_CODE_ERROR_FRAMESs, mac_control_data->purge_rx_code_error));
    }

    if (field->set_field_bmp & PC_PURGE_UNSUPPORTED_PAUSE_PFC_DA) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_UNSUPPORTED_PAUSE_PFC_DAs, mac_control_data->purge_unsupported_pause_pfc_da));
    }

    if (field->set_field_bmp & PC_PURGE_STACK_VLAN_FRAMES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_STACK_VLAN_FRAMESs, mac_control_data->purge_statck_vlan_frames));
    }

    if (field->set_field_bmp & PC_PURGE_WRONG_SA) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PURGE_WRONG_SAs, mac_control_data->purge_wrong_sa));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_MAC_CONTROL entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_MAC_CONTROL table name.
 * \param [in] eh Entry handle.
 * \param [in] mac_control_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_mac_control_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *mac_control_field)
{
    pc_mac_control_field_t *field = (pc_mac_control_field_t*)mac_control_field;
    pc_mac_control_data_t *mac_control_data = (pc_mac_control_data_t*)field->mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, OVERSIZE_PKTs, &mac_control_data->oversize_pkt));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_ENABLE_AUTOs, &mac_control_data->rx_enable_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_ENABLEs, &mac_control_data->rx_enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_ENABLE_AUTOs, &mac_control_data->tx_enable_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_ENABLEs, &mac_control_data->tx_enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LOCAL_FAULT_DISABLEs, &mac_control_data->local_fault_disable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, REMOTE_FAULT_DISABLEs, &mac_control_data->remote_fault_disable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, INTER_FRAME_GAP_AUTOs, &mac_control_data->inter_frame_gap_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, INTER_FRAME_GAPs, &mac_control_data->inter_frame_gap));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAUSE_TXs, &mac_control_data->pause_tx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAUSE_RXs, &mac_control_data->pause_rx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAUSE_ADDRs, &mac_control_data->pause_addr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAUSE_PASSs, &mac_control_data->pause_pass));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, CONTROL_PASSs, &mac_control_data->control_pass));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RUNT_THRESHOLD_AUTOs, &mac_control_data->runt_threshold_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RUNT_THRESHOLDs, &mac_control_data->runt_threshold));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, MAC_ECC_INTR_ENABLEs, &mac_control_data->mac_ecc_intr_enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_ENABLE_OPERs, &mac_control_data->rx_enable_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_ENABLE_OPERs, &mac_control_data->tx_enable_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, INTER_FRAME_GAP_OPERs, &mac_control_data->inter_frame_gap_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RUNT_THRESHOLD_OPERs, &mac_control_data->runt_threshold_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAUSE_RX_OPERs, &mac_control_data->pause_rx_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAUSE_TX_OPERs, &mac_control_data->pause_tx_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, OPERATIONAL_STATEs, &mac_control_data->oper_state));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, STALL_TXs, &mac_control_data->stall_tx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, STALL_TX_OPERs, &mac_control_data->stall_tx_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_DRIBBLE_NIBBLE_ERROR_FRAMESs, &mac_control_data->purge_dribble_nibble_error));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_MACSEC_FRAMESs, &mac_control_data->purge_macsec_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_SCH_CRC_ERRORs, &mac_control_data->purge_sch_crc_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_PFC_FRAMESs, &mac_control_data->purge_pfc_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_RUNT_FRAMESs, &mac_control_data->purge_runt_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_FIFO_FULLs, &mac_control_data->rx_fifo_full));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_UNICAST_FRAMESs, &mac_control_data->purge_unicast_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_VLAN_TAGGED_FRAMESs, &mac_control_data->purge_vlan_tagged_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_BAD_OPCODE_FRAMESs, &mac_control_data->purge_bad_opcode_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_PAUSE_FRAMESs, &mac_control_data->purge_pause_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_CONTROL_FRAMESs, &mac_control_data->purge_control_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_PROMISCUOUS_FRAMESs, &mac_control_data->purge_promiscuous_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_BROADCAST_FRAMESs, &mac_control_data->purge_broadcast_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_MULTICAST_FRAMESs, &mac_control_data->purge_multicast_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_GOOD_FRAMESs, &mac_control_data->purge_good_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_TRUNCATED_FRAMESs, &mac_control_data->purge_truncated_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_LENGTH_CHECK_FAIL_FRAMESs, &mac_control_data->purge_length_check_fail));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_CRC_ERROR_FRAMESs, &mac_control_data->purge_crc_code_error));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_RX_CODE_ERROR_FRAMESs, &mac_control_data->purge_rx_code_error));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_UNSUPPORTED_PAUSE_PFC_DAs, &mac_control_data->purge_unsupported_pause_pfc_da));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_STACK_VLAN_FRAMESs, &mac_control_data->purge_statck_vlan_frames));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PURGE_WRONG_SAs, &mac_control_data->purge_wrong_sa));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_mac_control_lookup(int unit, bcm_port_t lport)
{
    pc_mac_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.set_field_bmp = 0;
    field.mac_control_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_lookup(unit, PC_MAC_CONTROLs, &field,
                        ltsw_pc_mac_control_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_mac_control_insert(int unit, int lport, pc_mac_control_data_t *mac_control_data, uint64_t set_field_bmp)
{
    pc_mac_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.set_field_bmp = set_field_bmp;
    field.mac_control_data = mac_control_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_MAC_CONTROLs, &field,
            ltsw_pc_mac_control_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_mac_control_update(int unit, int lport, pc_mac_control_data_t *mac_control_data, uint64_t set_field_bmp)
{
    pc_mac_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.set_field_bmp = set_field_bmp;
    field.mac_control_data = mac_control_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PC_MAC_CONTROLs, &field,
            ltsw_pc_mac_control_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_mac_control_set(int unit, int lport, pc_mac_control_data_t *mac_control_data, uint64_t set_field_bmp)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_mac_control_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_insert(unit, lport, mac_control_data, set_field_bmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_update(unit, lport, mac_control_data, set_field_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_mac_control_get(int unit, int lport, pc_mac_control_data_t *mac_control_data)
{
    int rv;
    pc_mac_control_field_t field;

    SHR_FUNC_ENTER(unit);

    rv = ltsw_pc_mac_control_lookup(unit, lport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_insert(unit, lport, mac_control_data, 0));
    }

    field.lport = lport;
    field.set_field_bmp = 0;
    field.mac_control_data = mac_control_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_MAC_CONTROLs, &field,
                           ltsw_pc_mac_control_field_entry_set,
                           ltsw_pc_mac_control_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PHY_CONTROL entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PHY_CONTROL table name.
 * \param [in] eh Entry handle.
 * \param [in] pc_phy_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_phy_control_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pc_phy_ctrl_field)
{
    pc_phy_control_field_t *field = (pc_phy_control_field_t *)pc_phy_ctrl_field;
    pc_phy_control_data_t *phy_ctrl_data = (pc_phy_control_data_t*)field->phy_ctrl_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, LANE_INDEXs, field->lane_idx));

    if (phy_ctrl_data == NULL) {
        SHR_EXIT();
    }

    if (field->field_bmp & PC_TX_SQUELCH_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_SQUELCH_AUTOs, phy_ctrl_data->tx_squelch_auto));
    }

    if (field->field_bmp & PC_TX_SQUELCH) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_SQUELCHs, phy_ctrl_data->tx_squelch));
    }

    if (field->field_bmp & PC_RX_SQUELCH_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_SQUELCH_AUTOs, phy_ctrl_data->rx_squelch_auto));
    }

    if (field->field_bmp & PC_RX_SQUELCH) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_SQUELCHs, phy_ctrl_data->rx_squelch));
    }

    if (field->field_bmp & PC_RX_ADAPTATION_RESUME_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_ADAPTATION_RESUME_AUTOs, phy_ctrl_data->rx_adaption_resume_auto));
    }

    if (field->field_bmp & PC_RX_ADAPTATION_RESUME) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_ADAPTION_RESUMEs, phy_ctrl_data->rx_adaption_resume));
    }

    if (field->field_bmp & PC_RX_AFE_VGA_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_VGA_AUTOs, phy_ctrl_data->rx_afe_vga_auto));
    }

    if (field->field_bmp & PC_RX_AFE_VGA) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_VGAs, phy_ctrl_data->rx_afe_vga));
    }

    if (field->field_bmp & PC_RX_AFE_PEAKING_FILTER_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_PEAKING_FILTER_AUTOs, phy_ctrl_data->rx_afe_pf_auto));
    }

    if (field->field_bmp & PC_RX_AFE_PEAKING_FILTER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_PEAKING_FILTERs, phy_ctrl_data->rx_afe_pf));
    }

    if (field->field_bmp & PC_RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTOs, phy_ctrl_data->rx_afe_lf_pf_auto));
    }

    if (field->field_bmp & PC_RX_AFE_LOW_FREQ_PEAKING_FILTER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_LOW_FREQ_PEAKING_FILTERs, phy_ctrl_data->rx_afe_lf_pf));
    }

    if (field->field_bmp & PC_RX_AFE_HIGH_FREQ_PEAKING_FILTER_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_HIGH_FREQ_PEAKING_FILTER_AUTOs, phy_ctrl_data->rx_afe_hf_pf_auto));
    }

    if (field->field_bmp & PC_RX_AFE_HIGH_FREQ_PEAKING_FILTER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_HIGH_FREQ_PEAKING_FILTERs, phy_ctrl_data->rx_afe_hf_pf));
    }

    if (field->field_bmp & PC_PAM4_TX_PATTERN_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAM4_TX_PATTERN_AUTOs, phy_ctrl_data->pam4_tx_pattern_auto));
    }

    if (field->field_bmp & PC_PAM4_TX_PATTERN) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, PAM4_TX_PATTERNs, phy_ctrl_data->pam4_tx_pattern));
    }

    if (field->field_bmp & PC_PAM4_TX_PRECODER_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAM4_TX_PRECODER_AUTOs, phy_ctrl_data->pam4_tx_precoder_auto));
    }

    if (field->field_bmp & PC_PAM4_TX_PRECODER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PAM4_TX_PRECODERs, phy_ctrl_data->pam4_tx_precoder));
    }

    if (field->field_bmp & PC_PHY_ECC_INTR_ENABLE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PHY_ECC_INTR_ENABLE_AUTOs, phy_ctrl_data->phy_ecc_intr_enable_auto));
    }

    if (field->field_bmp & PC_PHY_ECC_INTR_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PHY_ECC_INTR_ENABLEs, phy_ctrl_data->phy_ecc_intr_enable));
    }

    if (field->field_bmp & PC_TX_PI_FREQ_OVERRIDE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PI_FREQ_OVERRIDE_AUTOs, phy_ctrl_data->tx_pi_freq_override_auto));
    }

    if (field->field_bmp & PC_TX_PI_FREQ_OVERRIDE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PI_FREQ_OVERRIDE_SIGNs, phy_ctrl_data->tx_pi_freq_override_sign));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PI_FREQ_OVERRIDEs, phy_ctrl_data->tx_pi_freq_override));
    }

    if (field->field_bmp & PC_FEC_BYPASS_INDICATION_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, FEC_BYPASS_INDICATION_AUTOs, phy_ctrl_data->fec_bypass_indication_auto));
    }

    if (field->field_bmp & PC_FEC_BYPASS_INDICATION) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, FEC_BYPASS_INDICATIONs, phy_ctrl_data->fec_bypass_indication));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAP_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_AFE_DFE_TAP_AUTOs, phy_ctrl_data->rx_afe_dfe_tap_auto));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAP1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAP_SIGNs, 0, &phy_ctrl_data->rx_afe_dfe_tap_sign[0], 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAPs, 0, &phy_ctrl_data->rx_afe_dfe_tap[0], 1));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAP2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAP_SIGNs, 1, &phy_ctrl_data->rx_afe_dfe_tap_sign[1], 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAPs, 1, &phy_ctrl_data->rx_afe_dfe_tap[1], 1));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAP3) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAP_SIGNs, 2, &phy_ctrl_data->rx_afe_dfe_tap_sign[2], 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAPs, 2, &phy_ctrl_data->rx_afe_dfe_tap[2], 1));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAP4) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAP_SIGNs, 3, &phy_ctrl_data->rx_afe_dfe_tap_sign[3], 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAPs, 3, &phy_ctrl_data->rx_afe_dfe_tap[3], 1));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAP5) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAP_SIGNs, 4, &phy_ctrl_data->rx_afe_dfe_tap_sign[4], 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAPs, 4, &phy_ctrl_data->rx_afe_dfe_tap[4], 1));
    }

    if (field->field_bmp & PC_RX_AFE_DFE_TAPS) {
        if (phy_ctrl_data->rx_afe_dfe_num > NUM_RX_DFE_TAPS) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAP_SIGNs, 0, phy_ctrl_data->rx_afe_dfe_tap_sign, phy_ctrl_data->rx_afe_dfe_num));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, RX_AFE_DFE_TAPs, 0, phy_ctrl_data->rx_afe_dfe_tap, phy_ctrl_data->rx_afe_dfe_num));
    }

    if (field->field_bmp & PC_PMD_DEBUG_LANE_EVENT_LOG_LEVEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PMD_DEBUG_LANE_EVENT_LOG_LEVELs, phy_ctrl_data->pmd_debug_lane_event_log_level));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PHY_CONTROL entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PHY_CONTROL table name.
 * \param [in] eh Entry handle.
 * \param [in] pc_phy_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_phy_control_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pc_phy_ctrl_field)
{
    pc_phy_control_field_t *field = (pc_phy_control_field_t *)pc_phy_ctrl_field;
    pc_phy_control_data_t *phy_ctrl_data = (pc_phy_control_data_t*)field->phy_ctrl_data;

    SHR_FUNC_ENTER(unit);
    /*
     * There is no PMD_DEBUG_LANE_EVENT_LOG_LEVEL in PC_PHY_STATUS,
     * so use PMD_DEBUG_LANE_EVENT_LOG_LEVEL in PC_PHY_CONTROL as the get value.
    */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PMD_DEBUG_LANE_EVENT_LOG_LEVELs, &phy_ctrl_data->pmd_debug_lane_event_log_level));

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_pc_phy_control_set(int unit, int port, int lane_idx, pc_phy_control_data_t *phy_ctrl_data, uint64_t set_field_bmp)
{
    pc_phy_control_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.lane_idx = lane_idx;
    field.field_bmp = set_field_bmp;
    field.phy_ctrl_data = phy_ctrl_data;

    rv = bcmi_lt_entry_lookup(unit, PC_PHY_CONTROLs, &field,
                              ltsw_pc_phy_control_field_entry_set);

    if (SHR_E_NOT_FOUND == rv) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, PC_PHY_CONTROLs, &field,
                                  ltsw_pc_phy_control_field_entry_set));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PC_PHY_CONTROLs, &field,
                                  ltsw_pc_phy_control_field_entry_set));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_phy_control_get(int unit, int lport, int lane_idx, pc_phy_control_data_t *phy_ctrl_data)
{
    int rv;
    pc_phy_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = lport;
    field.lane_idx = lane_idx;
    field.field_bmp = 0;
    field.phy_ctrl_data = phy_ctrl_data;

    rv = bcmi_lt_entry_lookup(unit, PC_PHY_CONTROLs, &field,
                              ltsw_pc_phy_control_field_entry_set);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, PC_PHY_CONTROLs, &field,
                                  ltsw_pc_phy_control_field_entry_set));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PHY_CONTROLs, &field,
                           ltsw_pc_phy_control_field_entry_set,
                           ltsw_pc_phy_control_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Callback function to set fields of PC_PM_TX_LANE_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PM_TX_LANE_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_profile_field PC_PM_TX_LANE_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_tx_taps_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *tx_taps_field)
{
    pc_tx_taps_field_t *field = (pc_tx_taps_field_t *)tx_taps_field;
    pc_tx_taps_data_t *tx_taps = (pc_tx_taps_data_t*)field->tx_taps_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, LANE_INDEXs, field->lane_idx));

    if (tx_taps == NULL) {
        SHR_EXIT();
    }

    if (field->field_bmp & PC_TX_FIR_PRE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PRE_AUTOs, tx_taps->tx_pre_auto));
    }

    if (field->field_bmp & PC_TX_FIR_PRE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PREs, tx_taps->tx_pre));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PRE_SIGNs, tx_taps->tx_pre_sign));
    }

    if (field->field_bmp & PC_TX_FIR_PRE2_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PRE2_AUTOs, tx_taps->tx_pre2_auto));
    }

    if (field->field_bmp & PC_TX_FIR_PRE2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PRE2s, tx_taps->tx_pre2));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_PRE2_SIGNs, tx_taps->tx_pre2_sign));
    }

    if (field->field_bmp & PC_TX_FIR_MAIN_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_MAIN_AUTOs, tx_taps->tx_main_auto));
    }

    if (field->field_bmp & PC_TX_FIR_MAIN) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_MAINs, tx_taps->tx_main));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_MAIN_SIGNs, tx_taps->tx_main_sign));
    }

    if (field->field_bmp & PC_TX_FIR_POST_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST_AUTOs, tx_taps->tx_post_auto));
    }

    if (field->field_bmp & PC_TX_FIR_POST) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POSTs, tx_taps->tx_post));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST_SIGNs, tx_taps->tx_post_sign));
    }

    if (field->field_bmp & PC_TX_FIR_POST2_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST2_AUTOs, tx_taps->tx_post2_atuo));
    }

    if (field->field_bmp & PC_TX_FIR_POST2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST2s, tx_taps->tx_post2));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST2_SIGNs, tx_taps->tx_post2_sign));
    }

    if (field->field_bmp & PC_TX_FIR_POST3_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST3_AUTOs, tx_taps->tx_post3_auto));
    }

    if (field->field_bmp & PC_TX_FIR_POST3) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST3s, tx_taps->tx_post3));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POST3_SIGNs, tx_taps->tx_post3_sign));
    }

    if (field->field_bmp & PC_TX_FIR_RPARA_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_RPARA_AUTOs, tx_taps->tx_rpara_auto));
    }

    if (field->field_bmp & PC_TX_FIR_RPARA) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_RPARAs, tx_taps->tx_rpara));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_RPARA_SIGNs, tx_taps->tx_rpara_sign));
    }

    if (field->field_bmp & PC_TX_FIR_AMP_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_AMP_AUTOs, tx_taps->tx_amp_auto));
    }

    if (field->field_bmp & PC_TX_FIR_AMP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_AMPs, tx_taps->tx_amp));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_AMP_SIGNs, tx_taps->tx_amp_sign));
    }

    if (field->field_bmp & PC_TX_FIR_DRV_MODE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_DRV_MODE_AUTOs, tx_taps->tx_drv_mode_auto));
    }

    if (field->field_bmp & PC_TX_FIR_DRV_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_DRV_MODEs, tx_taps->tx_drv_mode));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_DRV_MODE_SIGNs, tx_taps->tx_drv_mode_sign));
    }

    if (field->field_bmp & PC_TX_FIR_TAP_MODE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TXFIR_TAP_MODE_AUTOs, tx_taps->tx_tap_mode_auto));
    }

    if (field->field_bmp & PC_TX_FIR_TAP_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, TXFIR_TAP_MODEs, tx_taps->tx_tap_mode));
    }

    if (field->field_bmp & PC_TX_FIR_SIG_MODE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_SIG_MODE_AUTOs, tx_taps->tx_sig_mode_auto));
    }

    if (field->field_bmp & PC_TX_FIR_SIG_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, TX_SIG_MODEs, tx_taps->tx_sig_mode));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_tx_taps_set(int unit, int port, int lane_idx, pc_tx_taps_data_t *tx_taps_data, uint32_t set_field_bmp)
{
    pc_tx_taps_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.lane_idx = lane_idx;
    field.field_bmp = set_field_bmp;
    field.tx_taps_data = tx_taps_data;

    rv = bcmi_lt_entry_lookup(unit, PC_TX_TAPSs, &field,
                              ltsw_pc_tx_taps_field_entry_set);

    if (SHR_E_NOT_FOUND == rv) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, PC_TX_TAPSs, &field,
                                  ltsw_pc_tx_taps_field_entry_set));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PC_TX_TAPSs, &field,
                                  ltsw_pc_tx_taps_field_entry_set));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PMD_LANE_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PMD_LANE_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in]pc_profile_field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pmd_fw_field_entry_set(int unit, const char *lt_name,
                                         bcmlt_entry_handle_t eh, void *pc_pmd_fw_field)
{
    pc_pmd_fw_field_t *field = (pc_pmd_fw_field_t*)pc_pmd_fw_field ;
    pc_pmd_fw_data_t *pmd_fw_data = (pc_pmd_fw_data_t*)field->pmd_fw_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (pmd_fw_data == NULL) {
        SHR_EXIT();
    }

    if(field->field_bmp & PC_DFE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DFE_AUTOs, pmd_fw_data->dfe_auto));
    }

    if(field->field_bmp & PC_DFE_ON) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DFEs, pmd_fw_data->dfe_on));
    }

    if(field->field_bmp & PC_LP_DFE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LP_DFE_AUTOs, pmd_fw_data->lp_dfe_auto));
    }

    if(field->field_bmp & PC_LP_DFE_ON) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LP_DFEs, pmd_fw_data->lp_dfe_on));
    }

    if(field->field_bmp & PC_CL72_RESTART_TO_EN_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, CL72_RESTART_TIMEOUT_EN_AUTOs, pmd_fw_data->cl72_RestTO_EN_auto));
    }

    if(field->field_bmp & PC_Cl72_RESTART_TO_EN) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, CL72_RESTART_TIMEOUT_ENs, pmd_fw_data->cl72_RestTO_EN));
    }

    if(field->field_bmp & PC_LP_TX_PRECODER_ON_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LP_TX_PRECODER_ON_AUTOs, pmd_fw_data->lp_tx_precoder_on_auto));
    }

    if(field->field_bmp & PC_LP_TX_PRECODER_ON) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LP_TX_PRECODER_ONs, pmd_fw_data->lp_tx_precoder_on));
    }

    if(field->field_bmp & PC_UNRELIABLE_LOS_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, UNRELIABLE_LOS_AUTOs, pmd_fw_data->unreliable_los_auto));
    }

    if(field->field_bmp & PC_UNRELIABLE_LOS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, UNRELIABLE_LOSs, pmd_fw_data->unreliable_los));
    }

    if(field->field_bmp & PC_SCRAMBLING_ENABLE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SCRAMBLING_ENABLE_AUTOs, pmd_fw_data->scrambling_en_auto));
    }

    if(field->field_bmp & PC_SCRAMBLING_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SCRAMBLING_ENABLEs, pmd_fw_data->scrambling_en));
    }

    if(field->field_bmp & PC_NS_PAM4_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, NORMAL_REACH_PAM4_AUTOs, pmd_fw_data->ns_pam4_auto));
    }

    if(field->field_bmp & PC_NS_PAM4) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, NORMAL_REACH_PAM4s, pmd_fw_data->ns_pam4));
    }

    if(field->field_bmp & PC_ES_PAM4_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, EXTENDED_REACH_PAM4_AUTOs, pmd_fw_data->es_pam4_auto));
    }

    if(field->field_bmp & PC_ES_PAM4) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, EXTENDED_REACH_PAM4s, pmd_fw_data->es_pam4));
    }

    if(field->field_bmp & PC_MEDIUM_TYPE_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MEDIUM_TYPE_AUTOs, pmd_fw_data->medium_type_auto));
    }

    if(field->field_bmp & PC_PMD_LANE_MEDIUM_TYPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, MEDIUM_TYPEs, pmd_fw_data->medium_type));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
ltsw_pc_pmd_fw_set(int unit, int port, pc_pmd_fw_data_t *pmd_fw_data, uint32_t set_field_bmp)
{
    pc_pmd_fw_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.field_bmp = set_field_bmp;
    field.pmd_fw_data = pmd_fw_data;

    rv = bcmi_lt_entry_lookup(unit, PC_PMD_FIRMWAREs, &field,
                              ltsw_pc_pmd_fw_field_entry_set);
    if (SHR_E_NOT_FOUND == rv) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, PC_PMD_FIRMWAREs, &field,
                                  ltsw_pc_pmd_fw_field_entry_set));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PC_PMD_FIRMWAREs, &field,
                                  ltsw_pc_pmd_fw_field_entry_set));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Calculate the lane bitmask per port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] bitmask The value of lane bitmask per port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_reach_mode_lane_bitmask_calculate(int unit, bcm_port_t port, uint32_t *bitmask)
{
    int num_lane;
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    num_lane = port_data.num_lanes;

    if (num_lane == 0x1) {
        *bitmask = 0x1;
    } else if (num_lane == 0x2) {
        *bitmask = 0x3;
    } else if (num_lane == 0x4) {
        *bitmask = 0xf;
    } else if (num_lane == 0x8) {
        *bitmask = 0xff;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_lane_config_decode(int unit, bcm_port_t port, int phy_lane_config, pc_pmd_fw_data_t *pmd_fw_data)
{
    int medium_type;
    uint32_t bitmask;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_reach_mode_lane_bitmask_calculate(unit, port, &bitmask));

    pmd_fw_data->dfe_on            = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(phy_lane_config);
    pmd_fw_data->lp_dfe_on         = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(phy_lane_config);
    pmd_fw_data->cl72_RestTO_EN    = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_GET(phy_lane_config);
    pmd_fw_data->lp_tx_precoder_on = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_GET(phy_lane_config);
    pmd_fw_data->unreliable_los    = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_GET(phy_lane_config);
    pmd_fw_data->scrambling_en     = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_GET(phy_lane_config) ? 0 : 1;
    if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(phy_lane_config)) {
        pmd_fw_data->ns_pam4 = bitmask;
    } else {
        pmd_fw_data->ns_pam4 = 0;
    }
    if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(phy_lane_config)) {
        pmd_fw_data->es_pam4 = bitmask;
    } else {
        pmd_fw_data->es_pam4 = 0;
    }
    medium_type = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(phy_lane_config);
    pmd_fw_data->medium_type = id2name(lane_config_medium_type_map, COUNTOF(lane_config_medium_type_map), medium_type);
    if (pmd_fw_data->medium_type == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * portmod_common_pmd_lane_config_encode
 *
 * @brief encode the portmod lane_config passed into a 32-bit encoded value
 *
 * @param [in]  unit     - portmod_lane_config
 * @param [out] name     - lane_config
 */
static int
ltsw_port_lane_config_encode(int unit, int port, pc_pmd_fw_data_t *pmd_fw_data, int* lane_config)
{
    int medium_type;
    uint32_t bitmask;

    SHR_FUNC_ENTER(unit);

    if (NULL == pmd_fw_data) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_reach_mode_lane_bitmask_calculate(unit, port, &bitmask));

    *lane_config = 0;

    medium_type = name2id(lane_config_medium_type_map, COUNTOF(lane_config_medium_type_map),
                          pmd_fw_data->medium_type);
    if (medium_type == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    if (pmd_fw_data->dfe_on) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(*lane_config);
    }

    if (pmd_fw_data->lp_dfe_on) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(*lane_config);
    }

    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(*lane_config, medium_type);

    if (pmd_fw_data->cl72_RestTO_EN) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(*lane_config);
    }

    if (pmd_fw_data->lp_tx_precoder_on) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_SET(*lane_config);
    }

    if (pmd_fw_data->unreliable_los) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(*lane_config);
    }

    if (!pmd_fw_data->scrambling_en) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_SET(*lane_config);
    }

    if(pmd_fw_data->ns_pam4 == bitmask) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(*lane_config);
    }

    if (pmd_fw_data->es_pam4 == bitmask) {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_SET(*lane_config);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PORT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT table name.
 * \param [in] eh Entry handle.
 * \param [in] port_field PC_PORT field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                             void *port_field)
{
    pc_port_field_t *field = (pc_port_field_t *)port_field;
    pc_port_data_t *lport_data = field->lport_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (lport_data == NULL) {
        SHR_EXIT();
    }

    if (field->set_field_bmp & PC_PORT_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ENABLEs, lport_data->enable));
    }

    if (field->set_field_bmp & PC_PORT_MAX_FRAME_SIZE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MAX_FRAME_SIZEs, lport_data->max_frame_size));
    }

    if (field->set_field_bmp & PC_PORT_LPBK_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, LOOPBACK_MODEs, lport_data->lpbk_mode));
    }

    if (field->set_field_bmp & PC_PORT_NUM_LANES) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, NUM_LANESs,lport_data->num_lanes));
    }

    if (field->set_field_bmp & PC_PORT_AUTONEG_PROFILE_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, PC_AUTONEG_PROFILE_IDs, 0,
                    &PC_AN_PROFILE_ID(lport_data, 0),
                    PC_AN_PROFILE_NUM(lport_data)));
    }

    if (field->set_field_bmp & PC_PORT_AUTONEG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, AUTONEGs, lport_data->autoneg));
    }

    if (field->set_field_bmp & PC_PORT_LINK_TRAINING) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LINK_TRAININGs, lport_data->link_training));
    }

    if (field->set_field_bmp & PC_PORT_SPEED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SPEEDs, lport_data->speed));
    }

    if (field->set_field_bmp & PC_PORT_VALID_ABILITY_NUM) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, NUM_VALID_AN_PROFILESs, lport_data->valid_ability_num));
    }

    if (field->set_field_bmp & PC_PORT_ENCAP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, ENCAPs, lport_data->encap));
    }

    if (field->set_field_bmp & PC_PORT_FEC_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, FEC_MODEs, lport_data->fec_mode));
    }

    if (field->set_field_bmp & PC_PORT_LAG_FAILOVER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, LAG_FAILOVERs, lport_data->lag_failover));
    }

    if (field->set_field_bmp & PC_PORT_CUSTOM_SPEED_VCO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, SPEED_VCO_FREQs, lport_data->custom_speed_vco));
    }

    if (field->set_field_bmp & PC_PORT_RLM) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RLMs, lport_data->rlm));
    }

    if (field->set_field_bmp & PC_PORT_INITIATOR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, INITIATORs, lport_data->initiator));
    }

    if (field->set_field_bmp & PC_PORT_ACTIVE_LANE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ACTIVE_LANE_MASKs, lport_data->active_lane_mask));
    }

    if (field->set_field_bmp & PC_PORT_SUSPEND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SUSPENDs, lport_data->suspend));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PORT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT table name.
 * \param [in] eh Entry handle.
 * \param [in] port_field PC_PORT field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                             void *port_field)
{
    pc_port_field_t *field = (pc_port_field_t *)port_field;
    pc_port_data_t *lport_data = field->lport_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PORT_IDs, &field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ENABLEs, &lport_data->enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, MAX_FRAME_SIZEs, &lport_data->max_frame_size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, LOOPBACK_MODEs, &lport_data->lpbk_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get
             (eh, PC_AUTONEG_PROFILE_IDs, 0,
                &PC_AN_PROFILE_ID(lport_data, 0), MAX_NUM_AUTONEG_PROFILES,
                &PC_AN_PROFILE_NUM(lport_data)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_LANESs, &lport_data->num_lanes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, AUTONEGs, &lport_data->autoneg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LINK_TRAININGs, &lport_data->link_training));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SPEEDs, &lport_data->speed));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_VALID_AN_PROFILESs, &lport_data->valid_ability_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, ENCAPs, &lport_data->encap));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, FEC_MODEs, &lport_data->fec_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LAG_FAILOVERs, &lport_data->lag_failover));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, SPEED_VCO_FREQs, &lport_data->custom_speed_vco));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RLMs, &lport_data->rlm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, INITIATORs, &lport_data->initiator));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ACTIVE_LANE_MASKs, &lport_data->active_lane_mask));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SUSPENDs, &lport_data->suspend));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PORT_PHYS_MAP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT_PHYS_MAP table name.
 * \param [in] eh Entry handle.
 * \param [in] port_field PC_PORT_PHYS_MAP field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_phys_map_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                      void *port_field)
{
    pc_port_phys_map_field_t *field = (pc_port_phys_map_field_t *)port_field;
    pc_port_phys_map_data_t *lport_map_data = field->lport_map_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    if (lport_map_data == NULL) {
        SHR_EXIT();
    }

    if (field->set_field_bmp & PC_PORT_PHYS_MAP_PPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PC_PHYS_PORT_IDs, lport_map_data->pport));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PORT_PHYS_MAP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT_PHYS_MAP table name.
 * \param [in] eh Entry handle.
 * \param [in] port_field PC_PORT_PHYS_MAP field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_phys_map_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                      void *port_field)
{
    pc_port_phys_map_field_t *field = (pc_port_phys_map_field_t *)port_field;
    pc_port_phys_map_data_t *lport_map_data = field->lport_map_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PORT_IDs, &field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PC_PHYS_PORT_IDs, &lport_map_data->pport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PORT_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_PORT_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_status_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                    void *status_field)
{
    pc_port_status_field_t *field = (pc_port_status_field_t *)status_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PORT_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_PORT_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_status_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                    void *status_field)
{
    pc_port_status_field_t *field = (pc_port_status_field_t *)status_field;
    pc_port_status_data_t *status_data = field->lport_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SPEEDs, &status_data->speed));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_LANESs, &status_data->num_lanes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, MAC_DISABLEDs, &status_data->mac_disabled));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PHY_DISABLEDs, &status_data->phy_disabled));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FAILOVER_LOOPBACKs, &status_data->failover_lb));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PMD_RX_LOCKs, &status_data->pmd_rx_lock));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, FEC_MODEs, &status_data->fec_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, LOOPBACKs, &status_data->loopback));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, AUTONEGs, &status_data->autoneg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LINK_TRAININGs, &status_data->link_training));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LINK_TRAINING_DONEs, &status_data->link_training_done));
#if 0
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, AUTONEG_DONEs, &status_data->autoneg_done));
#endif
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, ENCAPs, &status_data->encap));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LOCAL_FAULTs, &status_data->local_fault));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, REMOTE_FAULTs, &status_data->remote_fault));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, RLM_STATUSs, &status_data->rlm_status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PHYS_PORT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PHYS_PORT table name.
 * \param [in] eh Entry handle.
 * \param [in] pport_field PC_PHYS_PORT field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_phys_port_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                  void *pport_field)
{
    pc_phys_port_field_t *field = (pc_phys_port_field_t *)pport_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PC_PHYS_PORT_IDs, field->pport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PHYS_PORT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PHYS_PORT table name.
 * \param [in] eh Entry handle.
 * \param [in] pport_field PC_PHYS_PORT field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_phys_port_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                  void *pport_field)
{
    pc_phys_port_field_t *field = (pc_phys_port_field_t *)pport_field;
    pc_phys_port_data_t *pport_data = field->pport_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PC_PHYS_PORT_IDs, &field->pport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PC_PM_IDs, &pport_data->pm_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PM_PHYS_PORTs, &pport_data->pm_pport_id));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_insert(int unit, int port, pc_port_data_t *lport_data, uint32_t set_field_bmp)
{
    pc_port_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = set_field_bmp;
    field.lport_data = lport_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_PORTs, &field,
                              ltsw_pc_port_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_delete(int unit, int port)
{
    pc_port_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = 0;
    field.lport_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_delete(unit, PC_PORTs, &field,
                              ltsw_pc_port_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_set(int unit, int port, pc_port_data_t *lport_data, uint32_t set_field_bmp)
{
    pc_port_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = set_field_bmp;
    field.lport_data = lport_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PC_PORTs, &field,
                              ltsw_pc_port_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_get(int unit, int port, pc_port_data_t *lport_data)
{
    pc_port_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = 0;
    field.lport_data = lport_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PORTs, &field,
                           ltsw_pc_port_field_entry_set,
                           ltsw_pc_port_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_phys_map_insert(int unit, int port, pc_port_phys_map_data_t *lport_map_data, uint32_t set_field_bmp)
{
    pc_port_phys_map_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = set_field_bmp;
    field.lport_map_data = lport_map_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_PORT_PHYS_MAPs, &field,
                              ltsw_pc_port_phys_map_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_phys_map_delete(int unit, int port)
{
    pc_port_phys_map_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = 0;
    field.lport_map_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_delete(unit, PC_PORT_PHYS_MAPs, &field,
                              ltsw_pc_port_phys_map_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

int
ltsw_pc_port_phys_map_set(int unit, int port, pc_port_phys_map_data_t *lport_map_data, uint32_t set_field_bmp)
{
    pc_port_phys_map_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = set_field_bmp;
    field.lport_map_data = lport_map_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PC_PORT_PHYS_MAPs, &field,
                              ltsw_pc_port_phys_map_field_entry_set));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_phys_map_get(int unit, int port, pc_port_phys_map_data_t *lport_map_data)
{
    pc_port_phys_map_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.set_field_bmp = 0;
    field.lport_map_data = lport_map_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PORT_PHYS_MAPs, &field,
                           ltsw_pc_port_phys_map_field_entry_set,
                           ltsw_pc_port_phys_map_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_status_get(int unit, int port, pc_port_status_data_t *status_data)
{
    pc_port_status_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.lport_status_data = status_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PORT_STATUSs, &field,
                           ltsw_pc_port_status_field_entry_set,
                           ltsw_pc_port_status_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_phys_port_get(int unit, int pport, pc_phys_port_data_t *pport_data)
{
    pc_phys_port_field_t field;

    SHR_FUNC_ENTER(unit);

    field.pport = pport;
    field.pport_data = pport_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PHYS_PORTs, &field,
                           ltsw_pc_phys_port_field_entry_set,
                           ltsw_pc_phys_port_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_TX_TAPS_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_TX_TAPS_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_TX_TAPS_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_tx_taps_status_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                   void *status_field)
{
    pc_tx_taps_field_t *field = (pc_tx_taps_field_t *)status_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, LANE_INDEXs, field->lane_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_TX_TAPS_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_TX_TAPS_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_TX_TAPS_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_tx_taps_status_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                   void *status_field)
{
    pc_tx_taps_field_t *field = (pc_tx_taps_field_t *)status_field;
    pc_tx_taps_data_t *status_data = field->tx_taps_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_PREs, &status_data->tx_pre));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_PRE_SIGNs, &status_data->tx_pre_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_PRE2s, &status_data->tx_pre2));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_PRE2_SIGNs, &status_data->tx_pre2_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_MAINs, &status_data->tx_main));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_MAIN_SIGNs, &status_data->tx_main_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POSTs, &status_data->tx_post));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POST_SIGNs, &status_data->tx_post_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POST2s, &status_data->tx_post2));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POST2_SIGNs, &status_data->tx_post2_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POST3s, &status_data->tx_post3));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POST3_SIGNs, &status_data->tx_post3_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_RPARAs, &status_data->tx_rpara));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_RPARAs, &status_data->tx_rpara_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_AMPs, &status_data->tx_amp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_AMP_SIGNs, &status_data->tx_amp_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_DRV_MODEs, &status_data->tx_drv_mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_DRV_MODE_SIGNs, &status_data->tx_drv_mode_sign));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, TXFIR_TAP_MODEs, &status_data->tx_tap_mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, TX_SIG_MODEs, &status_data->tx_sig_mode));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_tx_taps_status_get(int unit, int port, int lane,
                       pc_tx_taps_data_t *status_data)
{
    pc_tx_taps_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.lane_idx = lane;
    field.tx_taps_data = status_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_TX_TAPS_STATUSs, &field,
                           ltsw_pc_tx_taps_status_field_entry_set,
                           ltsw_pc_tx_taps_status_field_entry_get));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PHY_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PHY_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_PHY_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_phy_status_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                   void *status_field)
{
    pc_phy_control_field_t *field = (pc_phy_control_field_t *)status_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, LANE_INDEXs, field->lane_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PHY_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PHY_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_PHY_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_phy_status_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                   void *status_field)
{
    pc_phy_control_field_t *field = (pc_phy_control_field_t *)status_field;
    pc_phy_control_data_t *status_data = field->phy_ctrl_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_SQUELCHs, &status_data->tx_squelch));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_SQUELCHs, &status_data->rx_squelch));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_AFE_VGAs, &status_data->rx_afe_vga));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_AFE_PEAKING_FILTERs, &status_data->rx_afe_pf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_AFE_LOW_FREQ_PEAKING_FILTERs, &status_data->rx_afe_lf_pf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_AFE_HIGH_FREQ_PEAKING_FILTERs, &status_data->rx_afe_hf_pf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get
            (eh, RX_AFE_DFE_TAPs, 0, &status_data->rx_afe_dfe_tap[0], NUM_RX_DFE_TAPS, &status_data->rx_afe_dfe_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get
            (eh, RX_AFE_DFE_TAP_SIGNs, 0, &status_data->rx_afe_dfe_tap_sign[0], NUM_RX_DFE_TAPS, &status_data->rx_afe_dfe_num));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_SIGNAL_DETECTs, &status_data->rx_signal_detect));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, PAM4_TX_PATTERNs, &status_data->pam4_tx_pattern));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PAM4_TX_PRECODERs, &status_data->pam4_tx_precoder));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PHY_ECC_INTR_ENABLEs, &status_data->phy_ecc_intr_enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_PI_FREQ_OVERRIDEs, &status_data->tx_pi_freq_override));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_PI_FREQ_OVERRIDE_SIGNs, &status_data->tx_pi_freq_override_sign));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, FEC_BYPASS_INDICATIONs, &status_data->fec_bypass_indication));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_phy_status_get(int unit, int port, int lane,
                       pc_phy_control_data_t *status_data)
{
    pc_phy_control_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.lane_idx = lane;
    field.phy_ctrl_data = status_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PHY_STATUSs, &field,
                           ltsw_pc_phy_status_field_entry_set,
                           ltsw_pc_phy_status_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PMD_FIRMWARE_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PMD_FIRMWARE_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_PMD_FIRMWARE_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pmd_fw_status_field_entry_set(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                   void *status_field)
{
    pc_pmd_fw_field_t *field = (pc_pmd_fw_field_t *)status_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, field->lport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_PMD_FIRMWARE_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PMD_FIRMWARE_STATUS table name.
 * \param [in] eh Entry handle.
 * \param [in] status_field PC_PMD_FIRMWARE_STATUS field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pmd_fw_status_field_entry_get(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                                   void *status_field)
{
    pc_pmd_fw_field_t *field = (pc_pmd_fw_field_t *)status_field;
    pc_pmd_fw_data_t *status_data = field->pmd_fw_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, DFEs, &status_data->dfe_on));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LP_DFEs, &status_data->lp_dfe_on));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, MEDIUM_TYPEs, &status_data->medium_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, CL72_RESTART_TIMEOUT_ENs, &status_data->cl72_RestTO_EN));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LP_TX_PRECODER_ONs, &status_data->lp_tx_precoder_on));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, UNRELIABLE_LOSs, &status_data->unreliable_los));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, SCRAMBLING_ENABLEs, &status_data->scrambling_en));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NORMAL_REACH_PAM4s, &status_data->ns_pam4));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, EXTENDED_REACH_PAM4s, &status_data->es_pam4));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pmd_fw_status_get(int unit, int port, pc_pmd_fw_data_t *status_data)
{
    pc_pmd_fw_field_t field;

    SHR_FUNC_ENTER(unit);

    field.lport = port;
    field.pmd_fw_data = status_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PMD_FIRMWARE_STATUSs, &field,
                           ltsw_pc_pmd_fw_status_field_entry_set,
                           ltsw_pc_pmd_fw_status_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PM_TX_LANE_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PM_TX_LANE_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_profile_field PC_PM_TX_LANE_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pm_core_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pm_core_field)
{
    pc_pm_core_field_t *field = (pc_pm_core_field_t *)pm_core_field;
    pc_pm_core_data_t *pm_core = (pc_pm_core_data_t*)field->pm_core_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PC_PM_IDs, field->pm_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, CORE_INDEXs, field->core_index));

    if (pm_core == NULL) {
        SHR_EXIT();
    }

    if (field->field_bmp & PC_PMD_COM_CLK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PMD_COM_CLKs, pm_core->pmd_com_clk));
    }

    if (field->field_bmp & PC_TX_LANE_MAP_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_LANE_MAP_AUTOs, pm_core->tx_lane_map_auto));
    }

    if (field->field_bmp & PC_TX_LANE_MAP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_LANE_MAPs, pm_core->tx_lane_map));
    }

    if (field->field_bmp & PC_RX_LANE_MAP_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_LANE_MAP_AUTOs, pm_core->rx_lane_map_auto));
    }

    if (field->field_bmp & PC_RX_LANE_MAP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RX_LANE_MAPs, pm_core->rx_lane_map));
    }

    if (field->field_bmp & PC_TX_POLARITY_FLIP_AUTO) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POLARITY_FLIP_AUTOs, pm_core->tx_polarity_filp_auto));
    }

    if (field->field_bmp & PC_TX_POLARITY_FLIP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TX_POLARITY_FLIPs, pm_core->tx_polarity_filp));
    }

    if (field->field_bmp & PC_RX_POLARITY_FLIP_AUTO) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, RX_POLARITY_FLIP_AUTOs, pm_core->rx_polarity_filp_auto));
    }

    if (field->field_bmp & PC_RX_POLARITY_FLIP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, RX_POLARITY_FLIPs, pm_core->rx_polarity_filp));
    }

    if (field->field_bmp & PC_PM_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, PM_MODEs, pm_core->pm_core));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PM_TX_LANE_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PFC_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_profile_field PC_PFC_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pm_core_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pm_core_field)
{
    pc_pm_core_field_t *field = (pc_pm_core_field_t *)pm_core_field;
    pc_pm_core_data_t *pm_core = (pc_pm_core_data_t*)field->pm_core_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, PMD_COM_CLKs, &pm_core->pmd_com_clk));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_LANE_MAP_AUTOs, &pm_core->tx_lane_map_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_LANE_MAPs, &pm_core->tx_lane_map));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_LANE_MAP_OPERs, &pm_core->tx_lane_map_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_LANE_MAP_AUTOs, &pm_core->rx_lane_map_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_LANE_MAPs, &pm_core->rx_lane_map));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_LANE_MAP_OPERs, &pm_core->rx_lane_map_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POLARITY_FLIP_AUTOs, &pm_core->tx_polarity_filp_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POLARITY_FLIPs, &pm_core->tx_polarity_filp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TX_POLARITY_FLIP_OPERs, &pm_core->tx_polarity_filp_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_POLARITY_FLIP_AUTOs, &pm_core->rx_polarity_filp_auto));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_POLARITY_FLIPs, &pm_core->rx_polarity_filp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, RX_POLARITY_FLIP_OPERs, &pm_core->rx_polarity_filp_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, PM_MODEs, &pm_core->pm_core));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_pm_core_set(int unit, int pm_id, int core_idx, pc_pm_core_data_t *pm_core_data, uint32_t set_field_bmp)
{
    pc_pm_core_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    field.pm_id = pm_id;
    field.core_index = core_idx;
    field.field_bmp = set_field_bmp;
    field.pm_core_data = pm_core_data;

    rv = bcmi_lt_entry_lookup(unit, PC_PM_COREs, &field,
                              ltsw_pc_pm_core_field_entry_set);

    if (SHR_E_NOT_FOUND == rv) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, PC_PM_COREs, &field,
                                  ltsw_pc_pm_core_field_entry_set));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PC_PM_COREs, &field,
                                  ltsw_pc_pm_core_field_entry_set));
    }

exit:
    SHR_FUNC_EXIT();
}

int
ltsw_pc_pm_core_get(int unit, int pm_id, int core_idx, pc_pm_core_data_t *pm_core_data)
{
    pc_pm_core_field_t field;
    int rv;

    SHR_FUNC_ENTER(unit);

    field.pm_id = pm_id;
    field.core_index = core_idx;
    field.field_bmp = 0;
    field.pm_core_data = pm_core_data;

    rv = bcmi_lt_entry_lookup(unit, PC_PM_COREs, &field,
                              ltsw_pc_pm_core_field_entry_set);

    if (SHR_E_NOT_FOUND == rv) {
        if (pm_core_data == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        sal_memset(pm_core_data, 0, sizeof(pc_pm_core_data_t));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PC_PM_COREs, &field,
                               ltsw_pc_pm_core_field_entry_set,
                               ltsw_pc_pm_core_field_entry_get));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_PM_TX_LANE_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PM_TX_LANE_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_profile_field PC_PM_TX_LANE_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pm_prop_field_entry_set(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pm_prop_field)
{
    pc_pm_prop_field_t *field = (pc_pm_prop_field_t *)pm_prop_field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PC_PM_IDs, field->pm_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PM_TX_LANE_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PFC_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_profile_field PC_PFC_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_pm_prop_field_entry_get(int unit, const char *lt_name,
                                    bcmlt_entry_handle_t eh, void *pm_prop_field)
{
    pc_pm_prop_field_t *field = (pc_pm_prop_field_t *)pm_prop_field;
    pc_pm_prop_data_t *pm_prop = (pc_pm_prop_data_t*)field->pm_prop_data;
    uint32_t actual_pll_num;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_LANESs, &pm_prop->num_lanes));

     SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_PORTSs, &pm_prop->num_ports));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_AGGRs, &pm_prop->num_aggr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_PLLs, &pm_prop->num_pll));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TVCO_SOURCE_INDEXs, &pm_prop->tvco_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, PM_TYPEs, &pm_prop->pm_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, VCO_RATEs, 0,
                                       &pm_prop->vco_rate[0], LTSW_PM_MAX_PLL_NUM, &actual_pll_num));

exit:
    SHR_FUNC_EXIT();
}

int
ltsw_pc_pm_prop_get(int unit, int pm_id, pc_pm_prop_data_t *pm_prop_data)
{
    pc_pm_prop_field_t field;

    SHR_FUNC_ENTER(unit);

    field.pm_id = pm_id;
    field.pm_prop_data = pm_prop_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_PM_PROPs, &field,
                           ltsw_pc_pm_prop_field_entry_set,
                           ltsw_pc_pm_prop_field_entry_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to set fields of PC_AUTONEG_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_AUTONEG_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] pfc_profile_field PC_AUTONEG_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_autoneg_profile_field_entry_set(int unit, const char *lt_name,
                                bcmlt_entry_handle_t eh, void *pc_profile_field)
{
    pc_profile_field_t *field = (pc_profile_field_t *)pc_profile_field;
    pc_autoneg_profile_data_t *profile_data = (pc_autoneg_profile_data_t*)field->profile_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PC_AUTONEG_PROFILE_IDs, field->profile_id));

    if (profile_data == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, ADVERT_SPEEDs, profile_data->advert_speed));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, LONG_CHs, profile_data->long_ch));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, NUM_LANESs, profile_data->num_lanes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, LINK_TRAINING_OFFs, profile_data->link_training_off));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, AUTONEG_MODEs, profile_data->autoneg_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MEDIUM_TYPEs, profile_data->medium_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, FEC_MODEs, profile_data->fec_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, PAUSE_TYPEs, profile_data->pause_type));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get fields of PC_AUTONEG_PROFILE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_AUTONEG_PROFILE table name.
 * \param [in] eh Entry handle.
 * \param [in] autoneg_profile_field PC_AUTONEG_PROFILE field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_autoneg_profile_field_entry_get(int unit, const char *lt_name,
                                bcmlt_entry_handle_t eh, void *pc_profile_field)
{
    pc_profile_field_t *field = (pc_profile_field_t *)pc_profile_field;
    pc_autoneg_profile_data_t *profile_data = (pc_autoneg_profile_data_t*)field->profile_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ADVERT_SPEEDs, &profile_data->advert_speed));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LONG_CHs, &profile_data->long_ch));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, NUM_LANESs, &profile_data->num_lanes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, LINK_TRAINING_OFFs, &profile_data->link_training_off));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, AUTONEG_MODEs, &profile_data->autoneg_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, MEDIUM_TYPEs, &profile_data->medium_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, FEC_MODEs, &profile_data->fec_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, PAUSE_TYPEs, &profile_data->pause_type));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_autoneg_profile_data_init(int unit, void* profile)
{
    pc_autoneg_profile_data_t* profile_data = (pc_autoneg_profile_data_t*)profile;

    SHR_FUNC_ENTER(unit);

    profile_data->long_ch = 0;
    profile_data->num_lanes = 0;
    profile_data->link_training_off = 0;
    profile_data->advert_speed = 0;

    profile_data->autoneg_mode = (char*)PC_PHY_AUTONEG_MODE_NONEs;
    profile_data->fec_mode = (char*)PC_FEC_NONEs;
    profile_data->medium_type = (char*)PC_PHY_MEDIUM_BACKPLANEs;
    profile_data->pause_type = (char*)PC_PAUSE_NONEs;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_autoneg_profile_cmp(int unit, void *entries, int entries_per_set, int index, int *cmp)
{
    pc_autoneg_profile_data_t *an_profile_in = (pc_autoneg_profile_data_t*)entries;
    pc_autoneg_profile_data_t profile_data;
    ltsw_pc_profile_info_t *pinfo;

    SHR_FUNC_ENTER(unit);

    pinfo = PROFILE_PC_AUTONEG(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_autoneg_profile_data_init(unit, &profile_data));

    /* read LT entries starting from index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_profile_get(unit, index, pinfo, &profile_data));

    *cmp = sal_memcmp(an_profile_in, &profile_data, BCMPC_FIELD_OFFSETOF(pc_autoneg_profile_data_t, autoneg_mode));

    if (*cmp != 0) {
        SHR_EXIT();
    }

    if (sal_strcmp(profile_data.autoneg_mode, an_profile_in->autoneg_mode)) {
        *cmp = 1;
        SHR_EXIT();
    }

    if (sal_strcmp(profile_data.medium_type, an_profile_in->medium_type)) {
        *cmp = 1;
        SHR_EXIT();
    }

    if (sal_strcmp(profile_data.fec_mode, an_profile_in->fec_mode)) {
        *cmp = 1;
        SHR_EXIT();
    }

    if (sal_strcmp(profile_data.pause_type, an_profile_in->pause_type)) {
        *cmp = 1;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_autoneg_profile_hash(int unit, void *entries, int entries_per_set, uint32_t *hash)
{
    *hash = shr_crc32(0, entries, entries_per_set * BCMPC_FIELD_OFFSETOF(pc_autoneg_profile_data_t, autoneg_mode));
    return SHR_E_NONE;
}

static int
ltsw_pc_port_init_data_restore(int unit, bcm_port_t port, pc_port_data_t *pc_port_data)
{
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (PC_PORT_INIT_DATA(unit, port)) {
        pc_port_data->autoneg = PC_PORT_INIT_DATA(unit, port)->autoneg;
        pc_port_data->enable = PC_PORT_INIT_DATA(unit, port)->enable;
        pc_port_data->link_training = PC_PORT_INIT_DATA(unit, port)->link_training;
        pc_port_data->max_frame_size = PC_PORT_INIT_DATA(unit, port)->max_frame_size;
        pc_port_data->num_lanes =  PC_PORT_INIT_DATA(unit, port)->num_lanes;
        pc_port_data->speed = PC_PORT_INIT_DATA(unit, port)->speed;
        pc_port_data->fec_mode =
            id2name(fec_mode_map, COUNTOF(fec_mode_map), PC_PORT_INIT_DATA(unit, port)->fec_mode);
        SHR_NULL_CHECK(pc_port_data->fec_mode, SHR_E_PARAM);

        if (PC_PORT_INIT_DATA(unit, port)->lpbk_mode == BCM_PORT_LOOPBACK_PHY) {
            if (pm_type == ltswPmTypePm4x10) {
                pc_port_data->lpbk_mode = PC_LPBK_PCSs;
            } else {
                pc_port_data->lpbk_mode = PC_LPBK_PMDs;
            }
        } else {
            pc_port_data->lpbk_mode =
                id2name(lpbk_mode_map, COUNTOF(lpbk_mode_map), PC_PORT_INIT_DATA(unit, port)->lpbk_mode);
            SHR_NULL_CHECK( pc_port_data->lpbk_mode, SHR_E_PARAM);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_init_data_store(int unit, bcm_port_t port, pc_port_data_t *pc_port_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pc_port_data, SHR_E_PARAM);

    if (PC_PORT_INIT_DATA(unit, port)) {
        PC_PORT_INIT_DATA(unit, port)->autoneg = pc_port_data->autoneg;
        PC_PORT_INIT_DATA(unit, port)->enable = pc_port_data->enable;
        PC_PORT_INIT_DATA(unit, port)->link_training = pc_port_data->link_training;
        PC_PORT_INIT_DATA(unit, port)->max_frame_size = pc_port_data->max_frame_size;
        PC_PORT_INIT_DATA(unit, port)->num_lanes = pc_port_data->num_lanes;
        PC_PORT_INIT_DATA(unit, port)->speed = pc_port_data->speed;
        PC_PORT_INIT_DATA(unit, port)->lpbk_mode =
            name2id(lpbk_mode_map, COUNTOF(lpbk_mode_map), pc_port_data->lpbk_mode);
        PC_PORT_INIT_DATA(unit, port)->fec_mode =
            name2id(fec_mode_map, COUNTOF(fec_mode_map), pc_port_data->fec_mode);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port init information.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT table name.
 * \param [in] eh Entry handle.
 * \param [in] port_info A array contains all port information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_info_init(int unit, const char *lt_name, bcmlt_entry_handle_t eh, void *port_info)
{
    ltsw_port_info_t *port_info_p = (ltsw_port_info_t *)port_info;
    pc_port_field_t port_field;
    pc_port_data_t port_data;
    int port, speed, encap;

    SHR_FUNC_ENTER(unit);

    port_field.lport_data = &port_data;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_field_entry_get(unit, lt_name, eh, &port_field));

    port = port_field.lport;
    speed = port_field.lport_data->speed;
    encap = name2id(encap_type_map, COUNTOF(encap_type_map),
                    port_field.lport_data->encap);
    if (encap < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!(port_info_p[port].port_type &
         (BCMI_LTSW_PORT_TYPE_CPU | BCMI_LTSW_PORT_TYPE_LB)))
    {
        SHR_IF_ERR_VERBOSE_EXIT
            (speed_to_port_type(speed, encap, &port_info_p[port].port_type));
    }
    if (port_info_p[port].port_type & BCMI_LTSW_PORT_TYPE_MA) {
        port_info_p[port].port_type |= BCMI_LTSW_PORT_TYPE_MGMT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_init_data_store(unit, port, &port_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port init information.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name PC_PORT_PHYS_MAP table name.
 * \param [in] eh Entry handle.
 * \param [in] port_info A array contains all port information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_pc_port_phys_map_info_init(int unit, const char *lt_name, bcmlt_entry_handle_t eh, void *port_info)
{
    ltsw_port_ctrl_info_t *port_ctrl_p = (ltsw_port_ctrl_info_t *)port_info;
    pc_port_phys_map_field_t port_field;
    pc_port_phys_map_data_t port_data;
    int port;

    SHR_FUNC_ENTER(unit);

    port_field.lport_map_data = &port_data;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_phys_map_field_entry_get(unit, lt_name, eh, &port_field));

    port = port_field.lport;
    port_ctrl_p->port[port].pport = port_data.pport;
    port_ctrl_p->phy_port[port_data.pport].port = port;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_valid_check(int unit)
{
    int lport, port;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    /* Check if PC_PORT is existed but PC_PORT_PHYS_MAP not existed */
    PORT_FP_ITER(unit, port) {
        if (PORT(unit, port).pport == -1) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                    "Error Port(%d): PC_PORT entry is existed but PC_PORT_PHYS_MAP entry is not existed!\n"), port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
        if (bcmi_ltsw_dev_logic_port_pipe(unit, port) !=
            bcmi_ltsw_dev_phys_port_pipe(unit, PORT(unit, port).pport)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Physical port %d and Logical port %d "
                                  "are not in same pipe\n"),
                       PORT(unit, port).pport, port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

    /* Check if PC_PORT_PHYS_MAP is existed but PC_PORT not existed */
    PHY_PORT_ITER(unit, port) {
        lport = PHY_PORT(unit, port).port;
        if (IS_CPU_PORT(unit, lport)
            || IS_LB_PORT(unit, lport)) {
            continue;
        }
        if (!PORT_IS_VALID(unit, lport)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                    "Error Port(%d): PC_PORT_PHYS_MAP entry is existed but PC_PORT entry is not existed!\n"), lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert Enum to int.
 *
 * \param [in] list List of int.
 * \param [in] e Enum which to find its int.
 *
 * \return Valid int or -1 otherwise.
 */
static int
ltsw_port_tab_etoi(const ltsw_port_tab_int_list_t *list, int e)
{
    if ((e < 0) || (e >= list->len)) {
        return -1;
    } else {
        return list->vals[e];
    }
}

/*!
 * \brief Convert int to Enum.
 *
 * \param [in] list List of int.
 * \param [in] s Int which to find its Enum.
 *
 * \return valid Enum or -1 otherwise.
 */
static int
ltsw_port_tab_itoe(const ltsw_port_tab_int_list_t *list, int i)
{
    int k;

    for (k = 0; k < list->len; k ++) {
        if (list->vals[k] == i) {
            return k;
        }
    }

    return -1;
}


/*!
 * \brief Convert Enum to string.
 *
 * \param [in] list List of strings.
 * \param [in] e Enum which to find its string.
 *
 * \return The pointer to the string or NULL otherwise.
 */
static const char*
ltsw_port_tab_etos(const ltsw_port_tab_sym_list_t *list, int e)
{
    if ((e < 0) || (e >= list->len)) {
        return NULL;
    } else {
        return list->syms[e];
    }
}

/*!
 * \brief Convert string to Enum.
 *
 * \param [in] list List of strings.
 * \param [in] s String which to find its Enum.
 *
 * \return valid Enum or -1 otherwise.
 */
static int
ltsw_port_tab_stoe(const ltsw_port_tab_sym_list_t *list, const char *s)
{
    int i;

    for (i = 0; i < list->len; i ++) {
        if ((sal_strcmp(s, list->syms[i])) == 0) {
            return i;
        }
    }

    return -1;
}


/*!
 * \brief Helper function to set multiple fields of PORT table.
 *
 * \param [in] unit Unit number.
 * \param [in] fields Array of fields.
 *  Fields belong to same LT must be grouped together.
 * \param [in] fields_cnt Number of fields.
 * \param [in] op Operation flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_tab_multi_fields_set(int unit,
                               ltsw_port_tab_field_t *fields, int fields_cnt,
                               uint32_t op)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i, j, fld_type;
    uint64_t u64;
    const char *s, *tbl;
    ltsw_port_tab_field_t *pf;
    const ltsw_port_tab_com_fld_list_t *fld_list;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0, pf = fields, tbl = NULL; i < fields_cnt; i ++, pf ++) {
        if ((pf->op & op) == 0) {
            continue;
        }

        /* Table changed or first table */
        if (tbl != pf->info->table->name) {
            /* Commit old table */
            if (eh != BCMLT_INVALID_HDL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
                bcmlt_entry_free(eh);
                eh = BCMLT_INVALID_HDL;
            }

            /* Allocate entry container for new table */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, pf->info->table->name, &eh));
            /* Set key field */
            if (pf->info->table->key) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, pf->info->table->key, pf->index));
            }

            tbl = pf->info->table->name;
        }

        /* Set data fields */
        fld_type = pf->info->flags
                 & (FLD_IS_ENUM | FLD_IS_SYMBOL | FLD_IS_ARRAY | FLD_IS_COMBINED);
        if (fld_type == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, pf->info->field, pf->data.n));
        } else if (fld_type == FLD_IS_ENUM) {
            u64 = ltsw_port_tab_etoi(pf->info->list, pf->data.n);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, pf->info->field, u64));
        } else if (fld_type == FLD_IS_SYMBOL) {
            s = ltsw_port_tab_etos(pf->info->list, pf->data.n);
            SHR_NULL_CHECK(s, SHR_E_FAIL);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, pf->info->field, s));
        } else if (fld_type == FLD_IS_ARRAY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add
                    (eh, pf->info->field, pf->data.a.idx,
                     pf->data.a.buf, pf->data.a.size));
        } else if (fld_type == FLD_IS_COMBINED) {
            fld_list = (const ltsw_port_tab_com_fld_list_t*)pf->info->list;
            for (j = 0; j < fld_list->len; j++) {
                u64 = (pf->data.n >> fld_list->flds[j].offset)
                    & ((1 << fld_list->flds[j].width) - 1);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, fld_list->flds[j].field, u64));
            }
        }
    }

    /* Commit last table */
    if (eh != BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Helper function to get multiple fields of PORT table.
 *
 * \param [in] unit Unit number.
 * \param [inout] fields Array of fields.
 *  Fields belong to same LT must be grouped together.
 * \param [in] fields_cnt Number of fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_tab_multi_fields_get(int unit,
                               ltsw_port_tab_field_t *fields, int fields_cnt,
                               uint32_t op)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i, j, fld_type, rv;
    uint32_t cnt;
    uint64_t u64;
    const char *s, *tbl;
    ltsw_port_tab_field_t *pf;
    const ltsw_port_tab_com_fld_list_t *fld_list;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0, pf = fields, tbl = NULL; i < fields_cnt; i ++, pf ++) {
        if ((pf->op & op) == 0) {
            continue;
        }

        /* Table changed or first table */
        if (tbl != pf->info->table->name) {
            /* Free entry container for old table */
            if (eh != BCMLT_INVALID_HDL) {
                bcmlt_entry_free(eh);
                eh = BCMLT_INVALID_HDL;
            }

            /* Allocate entry container */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, pf->info->table->name, &eh));
            /* Set key field */
            if (pf->info->table->key) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, pf->info->table->key, pf->index));
            }
            /* Get LT entry */
            rv = bcmi_lt_entry_commit
                     (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
            if (rv == SHR_E_NOT_FOUND) {
                /* Add a new entry with default value if entry not existing */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit
                        (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit
                        (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
            } else if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT(rv);
            }

            tbl = pf->info->table->name;
        }

        /* Get data fields */
        fld_type = pf->info->flags
                 & (FLD_IS_ENUM | FLD_IS_SYMBOL | FLD_IS_ARRAY | FLD_IS_COMBINED);
        if (fld_type == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, pf->info->field, &pf->data.n));
        } else if (fld_type == FLD_IS_ENUM) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, pf->info->field, &u64));
            pf->data.n = ltsw_port_tab_itoe(pf->info->list, u64);
        } else if (fld_type == FLD_IS_SYMBOL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, pf->info->field, &s));
            pf->data.n = ltsw_port_tab_stoe(pf->info->list, s);
            if (pf->data.n == -1) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        } else if (fld_type == FLD_IS_ARRAY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get
                    (eh, pf->info->field, pf->data.a.idx,
                     pf->data.a.buf, pf->data.a.size, &cnt));
        } else if (fld_type == FLD_IS_COMBINED) {
            fld_list = (const ltsw_port_tab_com_fld_list_t*)pf->info->list;
            pf->data.n = 0;
            for (j = 0; j < fld_list->len; j ++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, fld_list->flds[j].field, &u64));
                pf->data.n |=
                    ((u64 & ((1 << fld_list->flds[j].width) - 1))
                     << fld_list->flds[j].offset);
            }
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Update port names.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_name_update(int unit)
{
    int port, i;

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_CPU, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "cpu%d", i++);
    }

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_LB, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "lb%d", i++);
    }

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_GE, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "ge%d", i++);
    }

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_XE, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "xe%d", i++);
    }

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_CE, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "ce%d", i++);
    }

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_CDE, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "cd%d", i++);
    }

    i = 0;
    PORT_ITER(unit, BCMI_LTSW_PORT_TYPE_HG, port) {
        sal_snprintf(PORT(unit, port).port_name, BCMI_LTSW_PORT_NAME_MAX,
                     "hg%d", i++);
    }

    return SHR_E_NONE;
}


/*!
 * \brief Intialize dport structures.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_dport_init(int unit)
{
    int i, dport;

    ltsw_port_info[unit]->dport.enable =
        bcmi_ltsw_property_get(unit, BCMI_CPN_DPORT_MAP_ENABLE, 1);
    ltsw_port_info[unit]->dport.indexed =
        bcmi_ltsw_property_get(unit, BCMI_CPN_DPORT_MAP_INDEXED, 1);
    ltsw_port_info[unit]->dport.direct =
        bcmi_ltsw_property_get(unit, BCMI_CPN_DPORT_MAP_DIRECT, 0);

    /* Default map. Dport num no less than port num. */
    for (i = 0; i < PORT_NUM_MAX(unit); i ++) {
        ltsw_port_info[unit]->dport.l2d[i] = i;
        ltsw_port_info[unit]->dport.d2l[i] = i;
    }
    for (; i < ltsw_port_info[unit]->dport.dport_num; i ++) {
        ltsw_port_info[unit]->dport.d2l[i] = -1;
    }

    /* Read map from config */
    for (i = 0; i < PORT_NUM_MAX(unit); i ++) {
        dport = bcmi_ltsw_property_port_get(unit, i, BCMI_CPN_DPORT_MAP_PORT, -1);
        if (dport >= 0) {
            (void)bcmi_ltsw_port_dport_map(unit, i, dport);
        }
    }

    return SHR_E_NONE;
}


/*!
 * \brief De-intialize SW structures and profile tables.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_software_deinit(int unit)
{
    int port_num_max = 0, phy_port_num_max = 0, size;

    SHR_FUNC_ENTER(unit);

    /* Clear device specific info */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_info_deinit(unit));

    /* Destroy lock */
    sal_mutex_destroy(ltsw_port_info[unit]->lock);

    /* Clear and free memory for ltsw_port_info */
    port_num_max = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    phy_port_num_max = bcmi_ltsw_dev_phys_port_max(unit) + 1;
    size = sizeof(ltsw_port_ctrl_info_t)
           + sizeof(ltsw_port_info_t) * port_num_max
           + sizeof(ltsw_phy_port_info_t) * phy_port_num_max
           + sizeof(ltsw_pc_port_init_data_t) * port_num_max
           + sizeof(int) * port_num_max
           + sizeof(int) * port_num_max;
    sal_memset(ltsw_port_info[unit], 0, size);
    SHR_FREE(ltsw_port_info[unit]);
    ltsw_port_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Intialize SW structures and profile tables.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_software_init(int unit)
{
    int port_num_max = 0, phy_port_num_max = 0, alloc_size, i;
    int warm = bcmi_warmboot_get(unit);
    uint32_t ha_req_size, ha_alloc_size;
    void *ptr;

    SHR_FUNC_ENTER(unit);

    port_num_max = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    phy_port_num_max = bcmi_ltsw_dev_phys_port_max(unit) + 1;

    /* Allocate and initialize memory for ltsw_port_info */
    SHR_FREE(ltsw_port_info[unit]);
    alloc_size = sizeof(ltsw_port_ctrl_info_t)
               + sizeof(ltsw_port_info_t) * port_num_max
               + sizeof(ltsw_phy_port_info_t) * phy_port_num_max
               + sizeof(ltsw_pc_port_init_data_t) * port_num_max
               + sizeof(int) * port_num_max
               + sizeof(int) * port_num_max;
    SHR_ALLOC(ltsw_port_info[unit], alloc_size, "ltsw_port_info");
    SHR_NULL_CHECK(ltsw_port_info[unit], SHR_E_MEMORY);
    sal_memset(ltsw_port_info[unit], 0, alloc_size);
    ltsw_port_info[unit]->lock = sal_mutex_create("port_lock");
    ltsw_port_info[unit]->port_num_max = port_num_max;
    ltsw_port_info[unit]->port_addr_max = bcmi_ltsw_dev_max_port_addr(unit);
    ltsw_port_info[unit]->phy_port_num_max = phy_port_num_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_max_pm_size_get(unit, &ltsw_port_info[unit]->pm_num_max));
    ltsw_port_info[unit]->port = (ltsw_port_info_t*)(ltsw_port_info[unit] + 1);
    ltsw_port_info[unit]->phy_port =
        (ltsw_phy_port_info_t*)(ltsw_port_info[unit]->port + port_num_max);
    ltsw_port_info[unit]->pc_port_init_data =
        (ltsw_pc_port_init_data_t*)(ltsw_port_info[unit]->phy_port + phy_port_num_max);
    ltsw_port_info[unit]->modid_max = bcmi_ltsw_dev_max_modid(unit);
    ltsw_port_info[unit]->modid_count = bcmi_ltsw_dev_modid_count(unit);
    for (i = 0; i < phy_port_num_max; i ++) {
        ltsw_port_info[unit]->phy_port[i].port = -1;
    }
    for (i = 0; i < port_num_max; i++) {
        ltsw_port_info[unit]->port[i].pport = -1;
    }

    /* Allocate and initialize HA memory */
    ha_req_size = sizeof(bcmint_port_ha_info_t) * port_num_max;
    ha_alloc_size = ha_req_size;
    ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                 BCMI_HA_COMP_ID_PORT,
                                 BCMINT_PORT_SUB_COMP_ID_HA,
                                 "bcmPortInfo",
                                 &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(ptr, 0, ha_alloc_size);
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmi_ltsw_issu_struct_info_report
            (unit, BCMI_HA_COMP_ID_PORT, BCMINT_PORT_SUB_COMP_ID_HA,
             0, sizeof(bcmint_port_ha_info_t), port_num_max,
             BCMINT_PORT_HA_INFO_T_ID),
             SHR_E_EXISTS);

    ltsw_port_info[unit]->port_ha = (bcmint_port_ha_info_t*)ptr;

    ltsw_port_info[unit]->dport.dport_num = port_num_max;
    ltsw_port_info[unit]->dport.l2d =
        (int*)(ltsw_port_info[unit]->pc_port_init_data + port_num_max);
    ltsw_port_info[unit]->dport.d2l =
        (int*)(ltsw_port_info[unit]->dport.l2d + port_num_max);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_dport_init(unit));

    /* device specific info */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_info_init(unit));

    /* Get port init configuration from PC_PORT table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_traverse
            (unit, PC_PORTs, &PORT(unit, 0), ltsw_pc_port_info_init));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_traverse
            (unit, PC_PORT_PHYS_MAPs, ltsw_port_info[unit],
             ltsw_pc_port_phys_map_info_init));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_valid_check(unit));

    /* device specific info recover */
    if (warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_info_recover(unit));
    }

exit:
    if (SHR_FUNC_ERR() && (!warm)) {
        (void)bcmi_ltsw_ha_mem_free(unit, ltsw_port_info[unit]->port_ha);
        (void)ltsw_port_software_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable mac length check on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] enable Enable/Disable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_mac_len_check_enable(int unit, bcm_port_t port, int enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    mac_control_data.purge_length_check_fail = enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_PURGE_LENGTH_CHECK_FAIL_FRAMES));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize port settings if they are to be different from default.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_settings_init(int unit, bcm_port_t port)
{
    bcm_port_info_t info;
    int length_check;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "ltsw_port_settings_init: u=%d p=%d\n"),unit, port));

    bcm_port_info_t_init(&info);

    if (IS_ETH_PORT(unit, port)) {
        info.pause_tx = TRUE;
        info.pause_rx = TRUE;
        info.action_mask |= (BCM_PORT_ATTR_PAUSE_TX_MASK |
                             BCM_PORT_ATTR_PAUSE_RX_MASK);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_selective_set(unit, port, &info));

    /* Disable 802.3 frame mac length check as default */
    length_check = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_mac_len_check_enable(unit, port, length_check));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize port PHY and MAC.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_pm_init(int unit)
{
    bcm_port_t  p;

    SHR_FUNC_ENTER(unit);

    PORT_FP_ITER(unit, p) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "bcm_port_init: unit %d port %s\n"),
                                unit, bcmi_ltsw_port_name(unit, p)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_init_per_port(unit, p));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_learn_get(unit, port, flags));
        SHR_EXIT();
    } else if (BCM_GPORT_IS_TRUNK(port)){
        if (ltsw_feature(unit, LTSW_FT_VPLAG)) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_trunk_vp_lag_vp_learn_get(unit,
                                                     BCM_GPORT_TRUNK_GET(port),
                                                     flags));
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_learn_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_learn_set(unit, port, flags));
        SHR_EXIT();
    } else if (BCM_GPORT_IS_TRUNK(port)){
        if (ltsw_feature(unit, LTSW_FT_VPLAG)) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_trunk_vp_lag_vp_learn_set(unit,
                                                     BCM_GPORT_TRUNK_GET(port),
                                                     flags));
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (flags & BCM_PORT_LEARN_PENDING) {
        if (!IS_CPU_PORT(unit, port) && (!(flags & BCM_PORT_LEARN_ARL))) {
            /* When the PENDING flag is set, the ARL must also be set */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_learn_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the loopback internal(pcs) mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_phy_control_loopback_internal_get(int unit, bcm_port_t port, uint32_t* value)
{
    pc_port_status_data_t port_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    if (sal_strcmp(port_status_data.loopback, PC_LPBK_PCSs) == 0) {
        *value = 1;
    } else {
        *value = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the loopback internal(pcs) mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_phy_control_loopback_internal_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (value) {
        port_data.lpbk_mode = (char*)PC_LPBK_PCSs;
    } else {
        port_data.lpbk_mode = (char*)PC_LPBK_NONEs;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_LPBK_MODE));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the loopback pmd mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_phy_control_loopback_pmd_get(int unit, bcm_port_t port, uint32_t* value)
{
    pc_port_status_data_t port_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    if (sal_strcmp(port_status_data.loopback, PC_LPBK_PMDs) == 0) {
        *value = 1;
    } else {
        *value = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the loopback pmd mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_phy_control_loopback_pmd_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (value) {
        port_data.lpbk_mode = (char*)PC_LPBK_PMDs;
    } else {
        port_data.lpbk_mode = (char*)PC_LPBK_NONEs;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_LPBK_MODE));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the loopback remote pmd mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_phy_control_loopback_remote_get(int unit, bcm_port_t port, uint32_t* value)
{
    pc_port_status_data_t port_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    if (sal_strcmp(port_status_data.loopback, PC_LPBK_REMOTE_PMDs) == 0) {
        *value = 1;
    } else {
        *value = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the loopback remote pmd mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_phy_control_loopback_remote_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (value) {
        port_data.lpbk_mode = (char*)PC_LPBK_REMOTE_PMDs;
    } else {
        port_data.lpbk_mode = (char*)PC_LPBK_NONEs;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_LPBK_MODE));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the link training.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] value value for the configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_an_link_training_get(int unit, bcm_port_t port, int* value)
{
    pc_port_data_t port_data;
    uint64_t auto_idx;
    pc_autoneg_profile_data_t autoneg_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    if (port_data.valid_ability_num != 0) {
        auto_idx = PC_AN_PROFILE_ID(&port_data, 0);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_autoneg_profile_data_init(unit, &autoneg_data));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_pc_profile_get(unit, auto_idx, PROFILE_PC_AUTONEG(unit), &autoneg_data));
        *value = !autoneg_data.link_training_off;
    } else {
        *value = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set HG3 enable on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable HG3 enable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_hg3_enable_set(int unit, bcm_port_t port, int enable)
{
    pc_port_data_t port_data;
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    mac_control_data.inter_frame_gap = enable ? 8 : 12;
    mac_control_data.inter_frame_gap_auto = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_INTER_FRAME_GAP|PC_INTER_FRAME_GAP_AUTO));

    port_data.encap = enable ? PC_ENCAP_HIGIG3s : PC_ENCAP_IEEEs;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_ENCAP));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_port_hg3_enable_set(unit, port, enable),
         SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get HG3 enable on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] enable HG3 enable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_hg3_enable_get(int unit, bcm_port_t port, int *enable)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    *enable = sal_strcmp(port_data.encap, PC_ENCAP_HIGIG3s) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set encapsulation mode on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode Encapsulation mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_encap_set(int unit, bcm_port_t port, int mode)
{
    pc_port_status_data_t port_status_data;
    pc_port_data_t port_data;
    int pause, speed;

    SHR_FUNC_ENTER(unit);

    switch (mode) {
        case BCM_PORT_ENCAP_IEEE:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_hg3_enable_set(unit, port, 0));
            break;

        case BCM_PORT_ENCAP_HIGIG3:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_hg3_enable_set(unit, port, 1));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    if (port_data.enable) {
        speed = port_status_data.speed;
    } else {
        speed = port_data.speed;
    }

    /* disable pause for HG */
    pause = (mode == BCM_PORT_ENCAP_IEEE);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_set(unit, port, pause, pause));

    SHR_IF_ERR_VERBOSE_EXIT
        (speed_to_port_type(speed, mode, &PORT(unit, port).port_type));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get encapsulation mode on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode Encapsulation mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_port_encap_get(int unit, bcm_port_t port, int *mode)
{
    int hg3 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_hg3_enable_get(unit, port, &hg3));

    if (hg3) {
        *mode = BCM_PORT_ENCAP_HIGIG3;
    } else {
        *mode = BCM_PORT_ENCAP_IEEE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize internal flexport structure.
 *
 * \param [in] unit Unit number.
 * \param [in] port_resource User input flexport operations.
 * \param [out] resource Internal flexport structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_resource_init(int unit, bcm_port_resource_t *port_resource,
                            ltsw_flexport_resource_t *resource)
{
    int i, lport, num_op = resource->num_op, num_port = PORT_NUM_MAX(unit);
    bcm_port_resource_t *pr;
    ltsw_flexport_port_cfg_t *po;
    ltsw_flexport_port_info_t *pc, *pn;
    ltsw_flexport_pm_cfg_t *pm;
    pc_phys_port_data_t pport_data;
    pc_port_data_t port_data;
    pc_pmd_fw_data_t pmd_data;

    SHR_FUNC_ENTER(unit);

    /* current config */
    PORT_ALL_ITER(unit, lport) {
        if (!(PORT(unit, lport).flags & FLEXIBLE)) {
            continue;
        }

        pc = resource->current + lport;
        pc->valid = 1;
        pc->pport = PORT(unit, lport).pport;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_phys_port_get(unit, pc->pport, &pport_data));
        pc->pmid = pport_data.pm_id;
        pc->pmport = pport_data.pm_pport_id;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_get(unit, lport, &port_data));
        pc->speed = port_data.speed;
        pc->lanes = port_data.num_lanes;
        pc->fec =
            name2id(fec_mode_map, COUNTOF(fec_mode_map), port_data.fec_mode);
        if (pc->fec < 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        pc->training = port_data.link_training;
        pc->encap =
            name2id(encap_type_map, COUNTOF(encap_type_map), port_data.encap);
        if (pc->encap < 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pmd_fw_status_get(unit, lport, &pmd_data));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_lane_config_encode(unit, lport, &pmd_data, &pc->pmd));

        /* pm */
        pm = resource->pm + pc->pmid;
        BCM_PBMP_PORT_ADD(pm->pbm, lport);
    }

    /* copy to new config */
    sal_memcpy(resource->new, resource->current,
               sizeof(ltsw_flexport_port_info_t) * num_port);

    /* flexport operation */
    pr = port_resource;
    po = resource->operation;
    for (i = 0; i < num_op; i ++, pr ++, po ++) {
        po->flags = pr->flags;
        /* convert gport */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_addressable_local_get(unit, pr->port, &po->lport));
        pc = resource->current + po->lport;
        pn = resource->new + po->lport;
        po->valid = pc->valid;
        if (pr->physical_port == -1) {
            po->op = FLEXPORT_OP_DETACH;
            po->pport = pc->pport;
            po->pmid = pc->pmid;
            po->pmport = pc->pmport;
            po->lanes = pc->lanes;
            po->speed = pc->speed;
            po->fec = pc->fec;
            po->pmd = pc->pmd;
            po->training = pc->training;
            po->encap = pc->encap;
            /* update new config */
            pn->valid = 0;
        } else {
            po->op = FLEXPORT_OP_NONE;
            po->pport = pr->physical_port;
            /* could be invalid pport, but the error should be reported by
             * validation functions */
            (void)ltsw_pc_phys_port_get(unit, po->pport, &pport_data);
            po->pmid = pport_data.pm_id;
            po->pmport = pport_data.pm_pport_id;
            po->lanes = pr->lanes;
            po->speed = pr->speed;
            po->fec = pr->fec_type;
            po->pmd = pr->phy_lane_config;
            po->training = pr->link_training;
            po->encap = pr->encap;
            /* update new config */
            pn->valid = 1;
            pn->pport = po->pport;
            pn->pmid = po->pmid;
            pn->pmport = po->pmport;
            pn->lanes = po->lanes;
            pn->speed = po->speed;
            pn->fec = po->fec;
            pn->pmd = po->pmd;
            pn->training = po->training;
            pn->encap = po->encap;
        }

        /* pm */
        pm = resource->pm + po->pmid;
        pm->valid = 1;
        if (pm->num_op >= MAX_PORT_PER_PM * 2) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Too many operations in PM %d\n"),
                       po->pmid));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        pm->operation[pm->num_op ++] = po;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the type of FlexPort operations/changes:
 * none, add, delete remap, lanes, speed, encap.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_op_set(int unit, ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    int lport, pport, size;
    bcm_pbmp_t att_pbm, remap_pbm, lane_pbm, speed_pbm, fec_pbm, pmd_pbm, train_pbm, encap_pbm;
    ltsw_flexport_port_info_t *pc, *pn;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(att_pbm);
    BCM_PBMP_CLEAR(remap_pbm);
    BCM_PBMP_CLEAR(lane_pbm);
    BCM_PBMP_CLEAR(speed_pbm);
    BCM_PBMP_CLEAR(fec_pbm);
    BCM_PBMP_CLEAR(pmd_pbm);
    BCM_PBMP_CLEAR(train_pbm);
    BCM_PBMP_CLEAR(encap_pbm);

    /* decide op flags for each operation */
    for (i = num_op - 1, po = resource->operation + num_op - 1;
         i >= 0; i --, po --) {
        lport = po->lport;
        pport = po->pport;
        pc = resource->current + lport;

        if (!(po->op & FLEXPORT_OP_DETACH)) {
            BCM_PBMP_PORT_ADD(att_pbm, lport);
            if (!pc->valid) {
                /* new port added */
                po->op |= FLEXPORT_OP_ADD;
            } else if (pport != pc->pport) {
                /* portmap changed */
                po->op |= FLEXPORT_OP_REMAP;
                BCM_PBMP_PORT_ADD(remap_pbm, lport);
            } else {
                if (po->lanes != pc->lanes) {
                    /* lanes num changed */
                    po->op |= FLEXPORT_OP_LANES;
                    BCM_PBMP_PORT_ADD(lane_pbm, lport);
                }
                if (po->speed != pc->speed) {
                    /* speed changed */
                    po->op |= FLEXPORT_OP_SPEED;
                    BCM_PBMP_PORT_ADD(speed_pbm, lport);
                }
                if (po->fec != pc->fec) {
                    /* fec type changed */
                    po->op |= FLEXPORT_OP_FEC;
                    BCM_PBMP_PORT_ADD(fec_pbm, lport);
                }
                if (po->pmd != pc->pmd) {
                    /* phy lane cfg changed */
                    po->op |= FLEXPORT_OP_PMD;
                    BCM_PBMP_PORT_ADD(pmd_pbm, lport);
                }
                if (po->training != pc->training) {
                    /* link training changed */
                    po->op |= FLEXPORT_OP_TRAIN;
                    BCM_PBMP_PORT_ADD(train_pbm, lport);
                }
                if (po->encap != pc->encap) {
                    /* encap changed */
                    po->op |= FLEXPORT_OP_ENCAP;
                    BCM_PBMP_PORT_ADD(encap_pbm, lport);
                }

                /* Clear both NS and ES config on the phy lane config
                if link training enable is set */
                if (po->op & (FLEXPORT_OP_PMD | FLEXPORT_OP_TRAIN)) {
                    if (po->training) {
                        if (po->pmd != BCM_PORT_RESOURCE_DEFAULT_REQUEST) {
                            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(po->pmd);
                            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(po->pmd);
                            po->op |= FLEXPORT_OP_PMD;
                        }
                    }
                }
            }
        } else {
            if (!BCM_PBMP_MEMBER(att_pbm, lport)) {
                po->op |= FLEXPORT_OP_DEL;
            } else if (BCM_PBMP_MEMBER(remap_pbm, lport)) {
                po->op |= FLEXPORT_OP_REMAP;
            } else {
                if (BCM_PBMP_MEMBER(lane_pbm, lport)) {
                    po->op |= FLEXPORT_OP_LANES;
                }
                if (BCM_PBMP_MEMBER(speed_pbm, lport)) {
                    po->op |= FLEXPORT_OP_SPEED;
                }
                if (BCM_PBMP_MEMBER(fec_pbm, lport)) {
                    po->op |= FLEXPORT_OP_FEC;
                }
                if (BCM_PBMP_MEMBER(pmd_pbm, lport)) {
                    po->op |= FLEXPORT_OP_PMD;
                }
                if (BCM_PBMP_MEMBER(train_pbm, lport)) {
                    po->op |= FLEXPORT_OP_TRAIN;
                }
                if (BCM_PBMP_MEMBER(encap_pbm, lport)) {
                    po->op |= FLEXPORT_OP_ENCAP;
                }
            }
        }

        if ((po->op & (FLEXPORT_OP_ADD | FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP |
                       FLEXPORT_OP_LANES))
            && (po->flags & FLEXPORT_FLG_NO_REMAP)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Port %d shouldn't add/del/remap\n"),
                       po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    /* decide port config after flexport */
    size = sizeof(ltsw_flexport_port_info_t);
    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        lport = po->lport;
        pn = resource->new + lport;
        if (po->op & FLEXPORT_OP_DETACH) {
            sal_memset(pn, 0, size);
        } else {
            pn->valid = 1;
            pn->pport = po->pport;
            pn->pmid = po->pmid;
            pn->pmport = po->pmport;
            pn->lanes = po->lanes;
            pn->speed = po->speed;
            pn->encap = po->encap;
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the type of FlexPort operations on port macros.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_pm_op_set(int unit, ltsw_flexport_resource_t *resource)
{
    /* reserved for possible pm op flags */
    return SHR_E_NONE;
}


/*!
 * \brief Set PMD config for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] lanes Number of lanes in the port.
 * \param [in] phy_lane_config Serdes pmd lane config.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_pmd_set(int unit, int port, int phy_lane_config)
{
    uint32_t set_field_bmp = 0;
    pc_pmd_fw_data_t pmd_fw_data;

    SHR_FUNC_ENTER(unit);

    if (phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST) {
        /*Insert the default lane config*/
        pmd_fw_data.dfe_auto = 1;
        pmd_fw_data.lp_dfe_auto = 1;
        pmd_fw_data.cl72_RestTO_EN_auto = 1;
        pmd_fw_data.lp_tx_precoder_on_auto = 1;
        pmd_fw_data.unreliable_los_auto = 1;
        pmd_fw_data.scrambling_en_auto = 1;
        pmd_fw_data.ns_pam4_auto = 1;
        pmd_fw_data.es_pam4_auto = 1;
        pmd_fw_data.medium_type_auto = 1;
        set_field_bmp = PC_PMD_FW_AUTO_FLD_PBMP;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_lane_config_decode(unit, port, phy_lane_config, &pmd_fw_data));
        pmd_fw_data.dfe_auto = 0;
        pmd_fw_data.lp_dfe_auto = 0;
        pmd_fw_data.cl72_RestTO_EN_auto = 0;
        pmd_fw_data.lp_tx_precoder_on_auto = 0;
        pmd_fw_data.unreliable_los_auto = 0;
        pmd_fw_data.scrambling_en_auto = 0;
        pmd_fw_data.ns_pam4_auto = 0;
        pmd_fw_data.es_pam4_auto = 0;
        pmd_fw_data.medium_type_auto = 0;
        set_field_bmp = PC_PMD_FW_CFG_FLD_PBMP|PC_PMD_FW_AUTO_FLD_PBMP;
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pmd_fw_set(unit, port, &pmd_fw_data, set_field_bmp));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete ports on PC level.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_pc_del(int unit, ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, op, lport;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (!(po->op & FLEXPORT_OP_DETACH)) {
            continue;
        }

        op = po->op;
        lport = po->lport;

        /* PC_PORT */
        if (op & (FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP | FLEXPORT_OP_LANES |
                  FLEXPORT_OP_PC_RESET)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_delete(unit, lport));
        }
        /* PC_PORT_PHYS_MAP */
        if (op & (FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_phys_map_delete(unit, lport));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add ports on PC level.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_pc_add(int unit, ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, op, lport, lanes;
    uint32_t bmp;
    pc_port_data_t port_data;
    pc_port_phys_map_data_t port_map_data;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        op = po->op;
        lport = po->lport;
        lanes = po->lanes;
        if (op & (FLEXPORT_OP_ADD | FLEXPORT_OP_REMAP | FLEXPORT_OP_LANES |
                  FLEXPORT_OP_PC_RESET)) {
            /* PC_PORT_PHYS_MAP */
            if (op & (FLEXPORT_OP_ADD | FLEXPORT_OP_REMAP)) {
                port_map_data.pport = po->pport;
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_pc_port_phys_map_insert
                        (unit, lport, &port_map_data, PC_PORT_PHYS_MAP_PPORT));
            }

            /* PC_PORT */
            bmp = PC_PORT_SPEED | PC_PORT_NUM_LANES | PC_PORT_FEC_MODE
                |PC_PORT_MAX_FRAME_SIZE;
            port_data.max_frame_size = DEFAULT_MAX_FRAME_SIZE(unit);
            port_data.speed = po->speed;
            port_data.num_lanes = lanes;
            port_data.fec_mode =
                id2name(fec_mode_map, COUNTOF(fec_mode_map), po->fec);
            if (!port_data.fec_mode) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_insert(unit, lport, &port_data, bmp));

            /* PC_PMD_FIRMWARE */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexport_pmd_set(unit, lport, po->pmd));

            /* link training depends on PMD settings */
            port_data.link_training = po->training ? 1 : 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_set(unit, lport, &port_data, PC_PORT_LINK_TRAINING));
        } else {
            /* PC_PORT */
            bmp = 0;
            if (op & FLEXPORT_OP_SPEED) {
                bmp |= PC_PORT_SPEED;
                port_data.speed = po->speed;
            }
            if (op & FLEXPORT_OP_FEC) {
                bmp |= PC_PORT_FEC_MODE;
                port_data.fec_mode =
                    id2name(fec_mode_map, COUNTOF(fec_mode_map), po->fec);
                if (!port_data.fec_mode) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            if (bmp) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_pc_port_set(unit, lport, &port_data, bmp));
            }

            /* PC_PMD_FIRMWARE */
            if (op & FLEXPORT_OP_PMD) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexport_pmd_set(unit, lport, po->pmd));
            }

            /* link training depends on PMD settings */
            if (op & FLEXPORT_OP_TRAIN) {
                port_data.link_training = po->training ? 1 : 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_pc_port_set(unit, lport, &port_data, PC_PORT_LINK_TRAINING));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set ports encapsulation mode.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_encap_set(int unit, ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        if (po->op & (FLEXPORT_OP_ADD | FLEXPORT_OP_REMAP | FLEXPORT_OP_LANES |
                      FLEXPORT_OP_PC_RESET | FLEXPORT_OP_ENCAP)) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (ltsw_port_encap_set(unit, po->lport, po->encap));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Disable ports.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexport_port_disable(int unit, ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if ((po->op & FLEXPORT_OP_DETACH) || (po->flags & FLEXPORT_FLG_NO_REMAP)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_enable_set(unit, po->lport, 0));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Intialize port-based SW data.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_software_init_per_port(int unit, int port)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* device specific info */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_info_init_per_port(unit, port));

    /* Get port init configuration from PC_PORT table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PC_PORTs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_info_init(unit, PC_PORTs, eh, &PORT(unit, 0)));
    (void) bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PC_PORT_PHYS_MAPs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_phys_map_info_init
            (unit, PC_PORT_PHYS_MAPs, eh, ltsw_port_info[unit]));

exit:
    if (BCMLT_INVALID_HDL != eh) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-intialize port-based SW data.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_software_deinit_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    /* Reset device specific info */
    ltsw_port_info[unit]->phy_port[PORT(unit, port).pport].port = -1;
    sal_memset(&PORT(unit, port), 0, sizeof(ltsw_port_info_t));
    PORT(unit, port).pport = -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_info_init_per_port(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize PHY and MAC for port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_pm_init_per_port(int unit, int port)
{
    int rv;

    if ((rv = ltsw_port_settings_init(unit, port)) < 0) {
        LOG_WARN(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "Warning: Port %s: "
                             "Failed to configure initial settings: %s\n"),
                  bcmi_ltsw_port_name(unit, port), bcm_errmsg(rv)));
    }

    return SHR_E_NONE;
}

/*!
 * \brief Set multiple PORT/LPORT fields.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] fields Array of LT fields to modify.
 * \param [in] fields_cnt Number of fields to modify.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_tab_multi_set(int unit, bcm_port_t port,
                               ltsw_port_tab_field_t *fields,
                               int fields_cnt)
{
    uint32_t op;

    SHR_FUNC_ENTER(unit);

    /* Get basic info of each port tab */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tabs_info_get(unit, fields, fields_cnt));

    /* Decide operation of each port tab */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tabs_op_get(unit, port, fields, fields_cnt, &op));

    /* local config */
    if (op & PORT_CFG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_tabs_para_get
                (unit, port, fields, fields_cnt, PORT_CFG));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_tab_multi_fields_set
                (unit, fields, fields_cnt, PORT_CFG));
    }

    /* trunk config */
    if (op & TRUNK_CFG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_tabs_para_get
                (unit, port, fields, fields_cnt, TRUNK_CFG));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_tab_multi_fields_set
                (unit, fields, fields_cnt, TRUNK_CFG));
    }

    /* LPORT config */
    if (op & LPORT_CFG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_profile_lport_tab_update(unit, fields, fields_cnt));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get multiple PORT/LPORT fields.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] fields Array of LT fields to get.
 * \param [in] fields_cnt Number of fields to get.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_tab_multi_get(int unit, bcm_port_t port,
                               ltsw_port_tab_field_t *fields,
                               int fields_cnt)
{
    uint32_t op;

    SHR_FUNC_ENTER(unit);

    /* Get basic info of each port tab */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tabs_info_get(unit, fields, fields_cnt));

    /* Decide operation of each port tab */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tabs_op_get(unit, port, fields, fields_cnt, &op));

    /* local config */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tabs_para_get
            (unit, port, fields, fields_cnt, PORT_CFG));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_tab_multi_fields_get
            (unit, fields, fields_cnt, PORT_CFG));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get profile entries from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index First profile entry index.
 * \param [in] pinfo Profile information.
 * \param [out] profile Profile entries.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_profile_get(int unit, int index,
                             ltsw_port_profile_info_t *pinfo, void *profile)
{
    int offset;
    int inst, logic_index;

    SHR_FUNC_ENTER(unit);

        inst = pinfo->inst;
        logic_index = LOGIC_INDEX_GET(index, inst);
    for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pinfo->lt_get(unit, logic_index + offset,
                          (char*)profile + pinfo->entry_size * offset));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add profile.
 *
 * If same entries existed, just increament reference count.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Profile to be added.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_profile_add(int unit, void *profile, int ref_count,
                             ltsw_port_profile_info_t *pinfo, int *index)
{
    int entry_idx, offset, rv, rvt;
    int inst, logic_index;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    rv = bcmi_ltsw_profile_index_allocate
            (unit, pinfo->phd, profile, 0, pinfo->entries_per_set, &entry_idx);
    if (rv == SHR_E_NONE) {
        inst = pinfo->inst;
        logic_index = LOGIC_INDEX_GET(entry_idx, inst);
        /* write profile to LT entries starting from entry_idx */
        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            rvt = pinfo->lt_ins(unit, logic_index + offset,
                                (char*)profile + pinfo->entry_size * offset);
            if (SHR_FAILURE(rvt)) {
                /* free allocated index */
                (void)bcmi_ltsw_profile_index_free(unit, pinfo->phd, entry_idx);
                SHR_ERR_EXIT(rvt);
            }
        }
    } else if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(rv);
    }

    if (ref_count > 1) {
        /* increment reference count if more than one ref_count */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase
                (unit, pinfo->phd, pinfo->entries_per_set, entry_idx,
                (ref_count - 1)));
    }

    *index = entry_idx;

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add profile reference.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the added profile entry.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_profile_add_ref(int unit, int index, int ref_count,
                                 ltsw_port_profile_info_t *pinfo)
{
    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase
            (unit, pinfo->phd, pinfo->entries_per_set, index, ref_count));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete profile.
 *
 * If the entry is referred by more than one soure, just decreament ref count.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the first entry to be deleted.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_profile_delete(int unit, int index,
                                ltsw_port_profile_info_t *pinfo)
{
    int offset, rv;
    int inst, logic_index;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    rv = bcmi_ltsw_profile_index_free(unit, pinfo->phd, index);
    if (rv == SHR_E_NONE) {
        inst = pinfo->inst;
        logic_index = LOGIC_INDEX_GET(index, inst);
        /* delete LT entries starting from index */
        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pinfo->lt_del(unit, logic_index + offset));
        }
    } else if (rv != SHR_E_BUSY) {
        SHR_ERR_EXIT(rv);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count for profile.
 *
 * Hash is recovered from HW. Reference count is always increamented.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile index to be recovered.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_profile_recover(int unit, int index,
                                 ltsw_port_profile_info_t *pinfo)
{
    int profile_size;
    void *profile = NULL;
    uint32_t ref_count;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, pinfo->phd, index, &ref_count));
    /* the profile entry is not allocated */
    if (ref_count == 0) {
        profile_size = pinfo->entry_size * pinfo->entries_per_set;
        SHR_ALLOC(profile, profile_size, "profile");
        SHR_NULL_CHECK(profile, SHR_E_MEMORY);
        sal_memset(profile, 0, profile_size);
        /* read profile LT entries */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_profile_get(unit, index, pinfo, profile));
        /* recover profile hash */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, pinfo->phd, profile, pinfo->entries_per_set, index));
    }

    /* increment reference count */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase
            (unit, pinfo->phd, pinfo->entries_per_set, index, 1));

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given profile entries equals to the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] index Profile table index to be compared.
 * \param [in] pinfo Profile information.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_profile_cmp(int unit, void *entries, int index,
                             ltsw_port_profile_info_t *pinfo, int *cmp)
{
    int profile_size;
    void *profile = NULL;

    SHR_FUNC_ENTER(unit);

    profile_size = pinfo->entry_size * pinfo->entries_per_set;
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);

    /* read LT entries starting from index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_get(unit, index, pinfo, profile));

    *cmp = sal_memcmp(entries, profile, profile_size);

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_profile_get(int unit, int index, ltsw_pc_profile_info_t* pinfo, void* profile_data)
{
    pc_profile_field_t field;

    SHR_FUNC_ENTER(unit);

    field.profile_id = index;
    field.profile_data = profile_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PC_AUTONEG_PROFILEs, &field,
                           pinfo->pc_profile_field_set,
                           pinfo->pc_profile_field_get));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get profile entries from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index First profile entry index.
 * \param [in] pinfo Profile information.
 * \param [out] profile Profile entries.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pc_profile_get(int unit, int index,
                             ltsw_pc_profile_info_t *pinfo, void *profile)
{
    int offset;

    SHR_FUNC_ENTER(unit);

    for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_profile_get(unit, index + offset, pinfo,
                          (char*)profile + pinfo->entry_size * offset));
    }

exit:
    SHR_FUNC_EXIT();
}

static int ltsw_pc_profile_delete(int unit, int index, ltsw_pc_profile_info_t* pinfo)
{
    pc_profile_field_t field;

    SHR_FUNC_ENTER(unit);

    field.profile_id = index;
    field.profile_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_delete(unit, PC_AUTONEG_PROFILEs, &field,
                              pinfo->pc_profile_field_set));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete profile.
 *
 * If the entry is referred by more than one soure, just decreament ref count.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the first entry to be deleted.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pc_profile_delete(int unit, int index,
                                ltsw_pc_profile_info_t *pinfo)
{
    int offset, rv;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    rv = bcmi_ltsw_profile_index_free(unit, pinfo->phd, index);
    if (rv == SHR_E_NONE) {
        /* delete LT entries starting from index */
        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            SHR_IF_ERR_VERBOSE_EXIT(ltsw_pc_profile_delete(unit, index + offset, pinfo));
        }
    } else if (rv != SHR_E_BUSY) {
        SHR_ERR_EXIT(rv);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_profile_add(int unit, int index, ltsw_pc_profile_info_t* pinfo, void* profile_data)
{
    pc_profile_field_t field;

    SHR_FUNC_ENTER(unit);

    field.profile_id = index;
    field.profile_data = profile_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PC_AUTONEG_PROFILEs, &field,
                           pinfo->pc_profile_field_set));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add profile.
 *
 * If same entries existed, just increament reference count.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Profile to be added.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pc_profile_add(int unit, void *profile, int ref_count,
                             ltsw_pc_profile_info_t *pinfo, int *index)
{
    int entry_idx, offset, rv, rvt;

    SHR_FUNC_ENTER(unit);

    /*PORT_LOCK(unit);*/

    rv = bcmi_ltsw_profile_index_allocate
            (unit, pinfo->phd, profile, 0, pinfo->entries_per_set, &entry_idx);

    if (rv == SHR_E_NONE) {
        /* write profile to LT entries starting from entry_idx */
        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            rvt = ltsw_pc_profile_add(unit, entry_idx + offset, pinfo,
                                    (char*)profile + pinfo->entry_size * offset);
            if (SHR_FAILURE(rvt)) {
                /* free allocated index */
                (void)bcmi_ltsw_profile_index_free(unit, pinfo->phd, entry_idx);
                SHR_ERR_EXIT(rvt);
            }
        }
    } else if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(rv);
    }

    if (ref_count > 1) {
        /* increment reference count if more than one ref_count */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase
                (unit, pinfo->phd, pinfo->entries_per_set, entry_idx,
                (ref_count - 1)));
    }

    *index = entry_idx;
exit:
    /*PORT_UNLOCK(unit);*/
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given profile entries equals to the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] index Profile table index to be compared.
 * \param [in] pinfo Profile information.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pc_profile_cmp(int unit, void *entries, int index,
                             ltsw_pc_profile_info_t *pinfo, int *cmp)
{
    int profile_size;
    void *profile = NULL;

    SHR_FUNC_ENTER(unit);

    profile_size = pinfo->entry_size * pinfo->entries_per_set;
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);

    /* read LT entries starting from index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_profile_get(unit, index, pinfo, profile));

    *cmp = sal_memcmp(entries, profile, profile_size);

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_pc_profile_id_init_per_port(int unit, bcm_port_t port)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    port_data.valid_ability_num = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_VALID_ABILITY_NUM));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count for profile.
 *
 * Hash is recovered from HW. Reference count is always increamented.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile index to be recovered.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pc_profile_recover(int unit, int index,
                                 ltsw_pc_profile_info_t *pinfo)
{
    int profile_size, offset;
    void *profile = NULL;
    uint32_t ref_count;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, pinfo->phd, index, &ref_count));
    /* the profile entry is not allocated */
    if (ref_count == 0) {
        profile_size = pinfo->entry_size * pinfo->entries_per_set;
        SHR_ALLOC(profile, profile_size, "profile");
        SHR_NULL_CHECK(profile, SHR_E_MEMORY);

        for (offset = 0; offset < pinfo->entries_per_set; offset ++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_autoneg_profile_data_init(unit, (char*)profile + pinfo->entry_size * offset));
        }
        /* read profile LT entries */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_pc_profile_get(unit, index, pinfo, profile));
        /* recover profile hash */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, pinfo->phd, profile, pinfo->entries_per_set, index));
    }

    /* increment reference count */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase
            (unit, pinfo->phd, pinfo->entries_per_set, index, 1));

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_pc_profile_recover_per_port(int unit, bcm_port_t port)
{
    pc_port_data_t port_data;
    int i;
    ltsw_pc_profile_info_t *pinfo = NULL;
    int valid_profile_num;
    int auto_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    pinfo = &(ltsw_port_info[unit]->pc_an_profile_info);
    valid_profile_num = port_data.valid_ability_num;

    for (i = 0; i < valid_profile_num; i++) {
        auto_idx = PC_AN_PROFILE_ID(&port_data, i);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_pc_profile_recover(unit, auto_idx, pinfo));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_pc_profile_clear_per_port(int unit, bcm_port_t port)
{
    pc_port_data_t port_data;
    int i;
    int auto_idx = 0;
    ltsw_pc_profile_info_t *pinfo = NULL;
    int valid_profile_num;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    pinfo = &(ltsw_port_info[unit]->pc_an_profile_info);
    valid_profile_num = port_data.valid_ability_num;
    if (valid_profile_num == 0) {
        SHR_EXIT();
    }
    /*Delete PC Profile*/
    for(i = 0; i < valid_profile_num; i++) {
       auto_idx = PC_AN_PROFILE_ID(&port_data, i);
       SHR_IF_ERR_VERBOSE_EXIT
           ( bcmint_ltsw_pc_profile_delete(unit, auto_idx, pinfo));
    }
    port_data.valid_ability_num = 0;
    for(i = 0; i < valid_profile_num; i++) {
        PC_AN_PROFILE_ID(&port_data, i) = PC_AN_PROFILE_ID_DEFAULT;
    }
    PC_AN_PROFILE_NUM(&port_data) = valid_profile_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data,
                          PC_PORT_VALID_ABILITY_NUM | PC_PORT_AUTONEG_PROFILE_ID));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pc_profile_init (int unit)
{
    ltsw_pc_profile_info_t *pinfo;
    int port;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    pinfo = &(ltsw_port_info[unit]->pc_an_profile_info);
    pinfo->phd = BCMI_LTSW_PROFILE_PC_AUTONEG;
    pinfo->entry_size = sizeof(pc_autoneg_profile_data_t);
    pinfo->pc_profile_field_set = ltsw_pc_autoneg_profile_field_entry_set;
    pinfo->pc_profile_field_get = ltsw_pc_autoneg_profile_field_entry_get;

    rv = bcmi_ltsw_profile_unregister(unit, pinfo->phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &pinfo->phd, &pinfo->entry_idx_min,
                                    &pinfo->entry_idx_max, 1,
                                    ltsw_pc_autoneg_profile_hash,
                                    ltsw_pc_autoneg_profile_cmp));

    if (!bcmi_warmboot_get(unit)) {
        /* clear profile table first */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, PC_AUTONEG_PROFILEs));
    }

    PORT_FP_ITER(unit, port) {
         if (!bcmi_warmboot_get(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pc_profile_id_init_per_port(unit, port));
         } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pc_profile_recover_per_port(unit, port));
         }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pc_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* unregister profile */
    rv = bcmi_ltsw_profile_unregister(unit, BCMI_LTSW_PROFILE_PC_AUTONEG);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, PC_AUTONEG_PROFILEs));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_pc_max_pm_size_get(int unit, int *max_pm_num)
{
    uint32_t entry_num;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, PC_PM_PROPs, &entry_num));

    *max_pm_num = entry_num;

exit:
    SHR_FUNC_EXIT();
}

/*Get the lane mask in the core*/
int
bcmint_ltsw_port_lane_mask_get(int unit, bcm_port_t port, uint32_t *lane_mask)
{
    int pport, num_lane;
    pc_phys_port_data_t pport_data;
    pc_port_status_data_t status_data;

    int pm_pport_id;

    SHR_FUNC_ENTER(unit);
    pport = PORT(unit,port).pport;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phys_port_get(unit, pport, &pport_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));

    pm_pport_id = (int)pport_data.pm_pport_id * MIN_LANES_PER_PPORT(unit);
    num_lane = status_data.num_lanes;

    if (num_lane == 0x1) {
        *lane_mask = 1 << pm_pport_id;
    } else if (num_lane == 0x2) {
        *lane_mask = 0x3 << pm_pport_id;
    } else if (num_lane == 0x4) {
        *lane_mask = 0xf << pm_pport_id;
    } else if (num_lane == 0x8) {
        *lane_mask = 0xff;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_num_lane_get(int unit, bcm_port_t port, uint32_t *num_lane)
{
    pc_port_status_data_t status_data;
    uint32 lanes;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));

    lanes = status_data.num_lanes;

    if (lanes > MAX_NUM_LANES_PER_PM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *num_lane = lanes;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_tx_get(int unit, bcm_port_t port, int lane_idx, bcm_port_phy_tx_t *tx)
{
    pc_tx_taps_data_t tx_taps_status;
    uint32_t num_lane;
    uint32_t flag;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (lane_idx < 0) {
        flag = LTSW_PORT_CFG_PER_PORT;
    } else {
        if (lane_idx >= num_lane) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        flag = LTSW_PORT_CFG_PER_LANE;
    }

    if (flag == LTSW_PORT_CFG_PER_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
           (ltsw_pc_tx_taps_status_get(unit, port, 0, &tx_taps_status));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
           (ltsw_pc_tx_taps_status_get(unit, port, lane_idx, &tx_taps_status));
    }

    if (tx_taps_status.tx_pre_sign) {
        tx->pre = 0 - tx_taps_status.tx_pre;
    } else {
        tx->pre = tx_taps_status.tx_pre;
    }
    if (tx_taps_status.tx_pre2_sign) {
        tx->pre2 = 0 - tx_taps_status.tx_pre2;
    } else {
        tx->pre2 = tx_taps_status.tx_pre2;
    }
    if (tx_taps_status.tx_main_sign) {
        tx->main = 0 - tx_taps_status.tx_main;
    } else {
        tx->main = tx_taps_status.tx_main;
    }
    if (tx_taps_status.tx_post_sign) {
        tx->post = 0 - tx_taps_status.tx_post;
    } else {
        tx->post = tx_taps_status.tx_post;
    }
    if (tx_taps_status.tx_post2_sign) {
        tx->post2 = 0 - tx_taps_status.tx_post2;
    } else {
        tx->post2 = tx_taps_status.tx_post2;
    }
    if (tx_taps_status.tx_post3_sign) {
        tx->post3 = 0 - tx_taps_status.tx_post3;
    } else {
        tx->post3 = tx_taps_status.tx_post3;
    }

    if (tx_taps_status.tx_amp_sign) {
        tx->amp = 0 - tx_taps_status.tx_amp;
    } else {
        tx->amp = tx_taps_status.tx_amp;
    }

    if (tx_taps_status.tx_drv_mode_sign) {
        tx->drv_mode = 0 - tx_taps_status.tx_drv_mode;
    } else {
        tx->drv_mode = tx_taps_status.tx_drv_mode;
    }

    tx->tx_tap_mode = name2id(tx_tap_mode_map, COUNTOF(tx_tap_mode_map),
                              tx_taps_status.tx_tap_mode);
    tx->signalling_mode = name2id(phy_sig_mode_map, COUNTOF(phy_sig_mode_map),
                                  tx_taps_status.tx_sig_mode);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t *tx)
{
    bcm_port_t local_port = -1;
    int phyn = -1, phy_lane = -1, sys_side = 0;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(tx, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_phyn_validate
            (unit, port, &local_port, &phyn, &phy_lane, &sys_side));

    if (local_port != -1) {
        port = local_port;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_phy_tx_get(unit, port, phy_lane, tx));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_tx_set(int unit, bcm_port_t port, int lane_idx, bcm_port_phy_tx_t *tx)
{
    pc_tx_taps_data_t tx_fir;
    uint32_t num_lane, field_pbm;
    int idx;
    uint32_t flag;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (lane_idx < 0) {
        flag = LTSW_PORT_CFG_PER_PORT;
    } else {
        if (lane_idx >= num_lane) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        flag = LTSW_PORT_CFG_PER_LANE;
    }

    tx_fir.tx_pre = LTSW_PORT_ABS(tx->pre);
    tx_fir.tx_pre_sign = 0;
    if (tx->pre < 0) {
        tx_fir.tx_pre_sign = 1;
    }
    tx_fir.tx_pre_auto = 0;
    field_pbm = PC_TX_FIR_PRE|PC_TX_FIR_PRE_AUTO;

    tx_fir.tx_pre2 = LTSW_PORT_ABS(tx->pre2);
    tx_fir.tx_pre2_sign = 0;
    if (tx->pre2 < 0) {
        tx_fir.tx_pre2_sign = 1;
    }
    tx_fir.tx_pre2_auto = 0;
    field_pbm |= PC_TX_FIR_PRE2|PC_TX_FIR_PRE2_AUTO;

    tx_fir.tx_main = LTSW_PORT_ABS(tx->main);
    tx_fir.tx_main_sign = 0;
    if (tx->main < 0) {
        tx_fir.tx_main_sign = 1;
    }
    tx_fir.tx_main_auto = 0;
    field_pbm |= PC_TX_FIR_MAIN|PC_TX_FIR_MAIN_AUTO;

    tx_fir.tx_post = LTSW_PORT_ABS(tx->post);
    tx_fir.tx_post_sign = 0;
    if (tx->post < 0) {
        tx_fir.tx_post_sign = 1;
    }
    tx_fir.tx_post_auto = 0;
    field_pbm |= PC_TX_FIR_POST|PC_TX_FIR_POST_AUTO;

    tx_fir.tx_post2 = LTSW_PORT_ABS(tx->post2);
    tx_fir.tx_post2_sign = 0;
    if (tx->post2 < 0) {
        tx_fir.tx_post2_sign = 1;
    }
    tx_fir.tx_post2_atuo = 0;
    field_pbm |= PC_TX_FIR_POST2|PC_TX_FIR_POST2_AUTO;

    tx_fir.tx_post3 = LTSW_PORT_ABS(tx->post3);
    tx_fir.tx_post3_sign = 0;
    if (tx->post3 < 0) {
        tx_fir.tx_post3_sign = 1;
    }
    tx_fir.tx_post3_auto = 0;
    field_pbm |= PC_TX_FIR_POST3|PC_TX_FIR_POST3_AUTO;

    tx_fir.tx_amp = LTSW_PORT_ABS(tx->amp);
    tx_fir.tx_amp_sign = 0;
    if (tx->amp < 0) {
        tx_fir.tx_amp_sign = 1;
    }
    tx_fir.tx_amp_auto = 0;
    field_pbm |= PC_TX_FIR_AMP|PC_TX_FIR_AMP_AUTO;

    tx_fir.tx_drv_mode = LTSW_PORT_ABS(tx->drv_mode);
    tx_fir.tx_drv_mode_sign = 0;
    if (tx->drv_mode < 0) {
        tx_fir.tx_drv_mode_sign = 1;
    }
    tx_fir.tx_drv_mode_auto = 0;
    field_pbm |= PC_TX_FIR_DRV_MODE|PC_TX_FIR_DRV_MODE_AUTO;

    if (tx->tx_tap_mode == bcmPortPhyTxTapMode3Tap) {
        if (tx->signalling_mode == bcmPortPhySignallingModeNRZ) {
            tx_fir.tx_tap_mode = (char *)PC_TXFIR_NRZ_LP_TAPS_3s;
        } else {
            tx_fir.tx_tap_mode = (char *)PC_TXFIR_PAM4_LP_TAPS_3s;
        }
    } else {
        if (tx->signalling_mode == bcmPortPhySignallingModeNRZ) {
            tx_fir.tx_tap_mode = (char *)PC_TXFIR_NRZ_TAPS_6s;
        } else {
            tx_fir.tx_tap_mode = (char *)PC_TXFIR_PAM4_TAPS_6s;
        }
    }
    tx_fir.tx_tap_mode_auto = 0;
    field_pbm |= PC_TX_FIR_TAP_MODE|PC_TX_FIR_TAP_MODE_AUTO;

    switch(tx->signalling_mode) {
        case bcmPortPhySignallingModeNRZ:
            tx_fir.tx_sig_mode = (char *)PC_SIG_MODE_NRZs;
            break;
        case bcmPortPhySignallingModePAM4:
            tx_fir.tx_sig_mode = (char *)PC_SIG_MODE_PAM4s;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    tx_fir.tx_sig_mode_auto = 0;
    field_pbm |= PC_TX_FIR_SIG_MODE|PC_TX_FIR_SIG_MODE_AUTO;

    if (flag == LTSW_PORT_CFG_PER_PORT) {
        for (idx = 0; idx < num_lane; idx++) {
            SHR_IF_ERR_VERBOSE_EXIT(
                ltsw_pc_tx_taps_set(unit, port, idx, &tx_fir, field_pbm));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_tx_taps_set(unit, port, lane_idx, &tx_fir, field_pbm));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_phy_tx_set(int unit, bcm_port_t port, bcm_port_phy_tx_t *tx)
{
    bcm_port_t local_port = -1;
    int phyn = -1, phy_lane = -1, sys_side = 0;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(tx, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_phyn_validate
            (unit, port, &local_port, &phyn, &phy_lane, &sys_side));

    if (local_port != -1) {
        port = local_port;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_phy_tx_set(unit, port, phy_lane, tx));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
ltsw_port_local_fault_status_get(int unit, int port, int *local_fault)
{
    pc_port_status_data_t port_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    *local_fault = port_status_data.local_fault;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_remote_fault_status_get(int unit, int port, int *remote_fault)
{
    pc_port_status_data_t port_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    *remote_fault = port_status_data.remote_fault;

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_port_local_fault_control_set(int unit, bcm_port_t port, int local_control)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    if(local_control) {
        mac_control_data.local_fault_disable = 0;
    } else {
        mac_control_data.local_fault_disable = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_LOCAL_FAULT_DISABLE));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_remote_fault_control_set(int unit, bcm_port_t port, int remote_control)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    if(remote_control) {
        mac_control_data.remote_fault_disable = 0;
    } else {
        mac_control_data.remote_fault_disable = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_REMOTE_FAULT_DISABLE));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_remote_fault_control_get(int unit, bcm_port_t port, int *remote_control)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    if (mac_control_data.remote_fault_disable) {
        *remote_control = 0;
    }  else {
        *remote_control = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_local_fault_control_get(int unit, bcm_port_t port, int *local_control)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    if (mac_control_data.local_fault_disable) {
        *local_control = 0;
    }  else {
        *local_control = 1;
    }

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_port_control_rx_enable_set(int unit, bcm_port_t port, int rx_enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    if (rx_enable) {
        mac_control_data.rx_enable = 1;
    } else {
        mac_control_data.rx_enable = 0;
    }

    mac_control_data.rx_enable_auto = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_RX_ENABLE|PC_RX_ENABLE_AUTO));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_control_rx_enable_get(int unit, bcm_port_t port, int *rx_enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    *rx_enable = mac_control_data.rx_enable_oper;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_control_tx_enable_set(int unit, bcm_port_t port, int tx_enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    if (tx_enable) {
        mac_control_data.tx_enable = 1;
    } else {
        mac_control_data.tx_enable = 0;
    }

    mac_control_data.tx_enable_auto = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_TX_ENABLE|PC_TX_ENABLE_AUTO));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_control_tx_enable_get(int unit, bcm_port_t port, int *tx_enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    *tx_enable = mac_control_data.tx_enable_oper;

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_port_control_pkt_oversize_set(int unit, bcm_port_t port, int oversize)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    mac_control_data.oversize_pkt = oversize;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_OVERSIZE_PKT));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_control_pkt_oversize_get(int unit, bcm_port_t port, int *oversize)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    *oversize = mac_control_data.oversize_pkt;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_ethertype_get(int unit, int port, int *ethertype)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *ethertype = pfc_data.eth_type;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_opcode_get(int unit, int port, int *opcode)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *opcode = pfc_data.opcode;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_rx_get(int unit, int port, int *rx)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *rx = pfc_data.enable_rx;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_tx_get(int unit, int port, int *tx)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *tx = pfc_data.enable_tx;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_refresh_time_get(int unit, int port, int *refresh_time)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *refresh_time = pfc_data.refresh_timer;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_xoff_time_get(int unit, int port, int *xoff_time)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *xoff_time = pfc_data.xoff_timer;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_destmac_oui_get(int unit, int port, int *addr)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *addr = (pfc_data.dest_addr >> 24) & 0xffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_destmac_nonoui_get(int unit, int port, int *addr)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *addr = pfc_data.dest_addr & 0xffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_ethertype_set(int unit, int port, int value)
{
    pc_pfc_data_t pfc_data;
    int ethertype;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_pfc_ethertype_get(unit, port, &ethertype));

    if (ethertype != value) {
        pfc_data.eth_type = value;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_ETH_TYPE));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_opcode_set(int unit, int port, int value)
{
    pc_pfc_data_t pfc_data;
    int opcode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_pfc_opcode_get(unit, port, &opcode));

    if (opcode != value) {
        pfc_data.opcode = value;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_OPCODE));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_rx_set(int unit, int port, int enable)
{
    pc_pfc_data_t pfc_data;
    int pfc_tx, pfc_enable;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        pfc_data.enable_rx = 1;
    } else {
        pfc_data.enable_rx = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_ENABLE_RX));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_pfc_tx_get(unit, port, &pfc_tx));

    if (enable) {
        pfc_enable = 1;
    } else {
        pfc_enable = pfc_tx ? 1 : 0;
    }

    pfc_data.enable_stats = pfc_enable ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_ENABLE_STATS));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_pfc_rx_config_set(unit, port, pfc_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_tx_set(int unit, int port, int enable)
{
    pc_pfc_data_t pfc_data;
    int pfc_rx, pfc_enable;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        pfc_data.enable_tx = 1;
    } else {
        pfc_data.enable_tx = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_ENABLE_TX));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_pfc_rx_get(unit, port, &pfc_rx));

    if (enable) {
        pfc_enable = 1;
    } else {
        pfc_enable = pfc_rx ? 1 : 0;
    }

    pfc_data.enable_stats = pfc_enable ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_ENABLE_STATS));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_pfc_tx_config_set(unit, port, pfc_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_refresh_time_set(int unit, int port, int value)
{
    pc_pfc_data_t pfc_data;
    int refresh_time;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_pfc_refresh_time_get(unit, port, &refresh_time));

    if (refresh_time != value) {
        pfc_data.refresh_timer = value;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_REFRESH_TIMER));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_xoff_time_set(int unit, int port, int value)
{
    pc_pfc_data_t pfc_data;
    int xoff_time;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_pfc_xoff_time_get(unit, port, &xoff_time));

    if (xoff_time != value) {
        pfc_data.xoff_timer = value;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_XOFF_TIMER));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_destmac_oui_set(int unit, int port, int value)
{
    pc_pfc_data_t pfc_data;
    uint64_t addr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    addr = value & 0xffffff;
    addr = (addr << 24) | (pfc_data.dest_addr & 0xffffff);

    if (addr != pfc_data.dest_addr) {
        pfc_data.dest_addr = addr;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_DEST_ADDR));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_destmac_nonoui_set(int unit, int port, int value)
{
    pc_pfc_data_t pfc_data;
    uint64_t addr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    addr = value & 0xffffff;
    addr |= (pfc_data.dest_addr & 0xffffff000000ULL);

    if (addr != pfc_data.dest_addr) {
        pfc_data.dest_addr = addr;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_DEST_ADDR));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_pass_frame_get(int unit, int port, int *value)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_get(unit, port, &pfc_data));

    *value = pfc_data.pass;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pfc_pass_frame_set(int unit, int port, int enable)
{
    pc_pfc_data_t pfc_data;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        pfc_data.pass = 1;
    } else {
        pfc_data.pass = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pfc_set(unit, port, &pfc_data, PC_PFC_PASS));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pass_control_frame_set(int unit, int port, int enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        mac_control_data.control_pass = 1;
    } else {
        mac_control_data.control_pass = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_CONTROL_PASS));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_pass_control_frame_get(int unit, int port, int *value)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    *value = mac_control_data.control_pass;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_runt_threshold_get(int unit, int port, int *value)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    *value = mac_control_data.runt_threshold_oper;

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_port_runt_threshold_set(int unit, int port, int value)
{
    pc_mac_control_data_t mac_control_data;
    int runt_threshold;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_runt_threshold_get(unit, port, &runt_threshold));

    if (runt_threshold != value) {
        mac_control_data.runt_threshold = value;
        mac_control_data.runt_threshold_auto = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_RUNT_THRESHOLD|PC_RUNT_THRESHOLD_AUTO));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_stall_tx_get(int unit, int port, int *enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    
    *enable = mac_control_data.stall_tx ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_port_stall_tx_set(int unit, int port, int enable)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    mac_control_data.stall_tx = enable ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_STALL_TX));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_suspend_set(int unit, int port, int value)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    port_data.suspend = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_SUSPEND));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_pc_port_suspend_get(int unit, int port, int* value)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    *value = port_data.suspend;

    exit:
        SHR_FUNC_EXIT();
}

static int
ltsw_port_enqueue_set(int unit, int port, int value)
{
    int link;
    int is_suspend;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_link_status_get(unit, port, &link));

    if (link == BCM_PORT_LINK_STATUS_UP) {
        if (value) {
            is_suspend = 0;
        } else {
            is_suspend = 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_suspend_set(unit, port, is_suspend));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_port_enqueue_get(int unit, int port, int* value)
{
    int is_suspend;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_suspend_get(unit, port, &is_suspend));

    if (is_suspend) {
        *value = 0;
    } else {
        *value = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_control_gport_validate(int unit, bcm_port_t port_in,
                                        bcm_port_t *port_out)
{
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_PROXY(port_in)) {
        if (MODID_VALID_RANGE(unit, BCM_GPORT_PROXY_MODID_GET(port_in)) &&
            PORT_VALID_RANGE(unit, BCM_GPORT_PROXY_PORT_GET(port_in))) {
            *port_out = port_in;
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port_in, port_out));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Handle Port Macro related port control type, including MAC and SerDes configuration
 */
int
bcmint_ltsw_port_control_pm_get(int unit, bcm_port_t port,
                                bcm_port_control_t type,
                                int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    switch (type) {
        case bcmPortControlLinkFaultLocal:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_local_fault_status_get(unit, port, value));
            break;
        case bcmPortControlLinkFaultRemote:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_remote_fault_status_get(unit, port, value));
            break;
        case bcmPortControlLinkFaultLocalEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_local_fault_control_get(unit, port, value));
            break;
        case bcmPortControlLinkFaultRemoteEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_remote_fault_control_get(unit, port, value));
            break;
        case bcmPortControlRxEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_control_rx_enable_get(unit, port, value));
            break;
        case bcmPortControlTxEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_control_tx_enable_get(unit, port, value));
            break;
        case bcmPortControlStatOversize:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_control_pkt_oversize_get(unit, port, value));
            break;
        case bcmPortControlPFCEthertype:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_ethertype_get(unit, port, value));
            break;
        case bcmPortControlPFCOpcode:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_opcode_get(unit, port, value));
            break;
        case bcmPortControlPFCReceive:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_rx_get(unit, port, value));
            break;
        case bcmPortControlPFCTransmit:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_tx_get(unit, port, value));
            break;
        case bcmPortControlPFCRefreshTime:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_refresh_time_get(unit, port, value));
            break;
        case bcmPortControlPFCXOffTime:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_xoff_time_get(unit, port, value));
            break;
        case bcmPortControlPFCDestMacOui:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_destmac_oui_get(unit, port, value));
            break;
        case bcmPortControlPFCDestMacNonOui:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_destmac_nonoui_get(unit, port, value));
            break;
        case bcmPortControlPFCPassFrames:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_pass_frame_get(unit, port, value));
            break;
        case bcmPortControlPassControlFrames:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pass_control_frame_get(unit, port, value));
            break;
        case bcmPortControlRuntThreshold:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_runt_threshold_get(unit, port, value));
            break;
        case bcmPortControlMmuTrafficEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_enqueue_get(unit, port, value));
            break;
        case bcmPortControlTxStall:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_stall_tx_get(unit, port, value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_control_pm_set(int unit, bcm_port_t port,
                                bcm_port_control_t type,
                                int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    switch (type) {
        case bcmPortControlLinkFaultLocalEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_local_fault_control_set(unit, port, value));
            break;
        case bcmPortControlLinkFaultRemoteEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_remote_fault_control_set(unit, port, value));
            break;
        case bcmPortControlRxEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_control_rx_enable_set(unit, port, value));
            break;
        case bcmPortControlTxEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_control_tx_enable_set(unit, port, value));
            break;
        case bcmPortControlStatOversize:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_control_pkt_oversize_set(unit, port, value));
            break;
        case bcmPortControlPFCEthertype:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_ethertype_set(unit, port, value));
            break;
        case bcmPortControlPFCOpcode:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_opcode_set(unit, port, value));
            break;
        case bcmPortControlPFCReceive:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_rx_set(unit, port, value));
            break;
        case bcmPortControlPFCTransmit:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_tx_set(unit, port, value));
            break;
        case bcmPortControlPFCRefreshTime:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_refresh_time_set(unit, port, value));
            break;
        case bcmPortControlPFCXOffTime:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_xoff_time_set(unit, port, value));
            break;
        case bcmPortControlPFCDestMacOui:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_destmac_oui_set(unit, port, value));
            break;
        case bcmPortControlPFCDestMacNonOui:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_destmac_nonoui_set(unit, port, value));
            break;
        case bcmPortControlPFCPassFrames:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pfc_pass_frame_set(unit, port, value));
            break;
        case bcmPortControlPassControlFrames:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_pass_control_frame_set(unit, port, value));
            break;
        case bcmPortControlRuntThreshold:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_runt_threshold_set(unit, port, value));
            break;
        case bcmPortControlMmuTrafficEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_enqueue_set(unit, port, value));
            break;
        case bcmPortControlTxStall:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_stall_tx_set(unit, port, value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check array order. Delete operation must be first in array.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_operation_order_validate(
    int unit,
    ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, delete = 1;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (!(po->op & FLEXPORT_OP_DETACH)) {  /* First non-delete found */
            delete = 0;
        } else if (po->op & FLEXPORT_OP_DETACH) {
            if (!delete) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Delete operations must be "
                                      "first in array\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Convert gport, validate logical and physical ports are in range and flexible.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_port_eligible_validate(
    int unit,
    ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, lport, pport;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i++, po++) {
        lport = po->lport;
        if (!PORT_VALID_RANGE(unit, lport)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Invalid logical port %d\n"),
                       lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (PORT(unit, lport).port_type & BCMI_LTSW_PORT_TYPE_SPARE) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Inactive logical port %d\n"),
                       lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (!(PORT(unit, lport).flags & FLEXIBLE)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "FlexPort operation is not allowable on "
                                  "logical port %d\n"),
                       lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if ((PORT(unit, lport).flags & NO_REMAP) &&
            !(po->flags & FLEXPORT_FLG_NO_REMAP)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Remap operation is not enabled on "
                                  "logical port %d\n"),
                       lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        pport = po->pport;
        if (!(po->op & FLEXPORT_OP_DETACH)) {
            if (!PHY_PORT_VALID_RANGE(unit, pport)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit, "Invalid physical port %d\n"),
                           pport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if (ltsw_port_info[unit]->phy_port[pport].flags & INACTIVE) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Inactive physical port %d\n"),
                           pport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if (!(ltsw_port_info[unit]->phy_port[pport].flags & FLEXIBLE)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "FlexPort operation is not allowable on "
                                      "physical port %d\n"),
                           pport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if ((ltsw_port_info[unit]->phy_port[pport].flags & NO_REMAP) &&
                !(po->flags & FLEXPORT_FLG_NO_REMAP)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Remap operation is not enabled on "
                                      "physical port %d\n"),
                           pport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else if (po->valid) {
            if (ltsw_port_info[unit]->phy_port[pport].flags & INACTIVE) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Physical port %d of logical port %d "
                                      "is inactive\n"),
                           pport, lport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if (!(ltsw_port_info[unit]->phy_port[pport].flags & FLEXIBLE)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "FlexPort operation is not allowable on "
                                      "physical port %d of logical port %d\n"),
                           pport, lport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if ((ltsw_port_info[unit]->phy_port[pport].flags & NO_REMAP) &&
                !(po->flags & FLEXPORT_FLG_NO_REMAP)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Remap operation is not enabled on "
                                      "physical port %d of logical port %d\n"),
                           pport, lport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Delete port %d doesn't exist\n"),
                       lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check that existed ports are disabled.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_port_status_validate(int unit,
                                          ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, en;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (!po->valid) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_enable_get(unit, po->lport, &en));
        if (en) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Port %d needs to be disabled\n"),
                       po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check ports to be 'detached' do not have linkscan mode set.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_linkscan_validate(int unit,
                                       ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, rv;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (!(po->op & (FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP))) {
            continue;
        }

        rv = bcm_ltsw_linkscan_enable_port_get(unit, po->lport);
        if (rv == SHR_E_PORT) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        if (rv != SHR_E_DISABLED) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Linkscan mode needs to be "
                                  "disabled on port %d to be detached\n"),
                       po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check logical port and physical port are in same pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_pipe_validate(int unit,
                                   ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        if (bcmi_ltsw_dev_logic_port_pipe(unit, po->lport) !=
            bcmi_ltsw_dev_phys_port_pipe(unit, po->pport)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Physical port %d and Logical port %d "
                                  "are not in same pipe\n"),
                       po->pport, po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check port numbers are available and mappings are not replicated.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_port_mapping_validate(int unit,
                                           ltsw_flexport_resource_t *resource)
{
    int i, j, lport, pport, lport_max, pport_max, lanes;
    int num_op = resource->num_op;
    uint8_t *lports = NULL, *pports = NULL;
    ltsw_flexport_port_info_t *pc;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    lport_max = resource->num_port;
    pport_max = ltsw_port_info[unit]->phy_port_num_max;
    SHR_ALLOC(lports, lport_max, "occupied logical ports");
    SHR_ALLOC(pports, pport_max, "occupied physical ports");
    SHR_NULL_CHECK(lports, SHR_E_MEMORY);
    SHR_NULL_CHECK(pports, SHR_E_MEMORY);
    sal_memset(lports, 0, lport_max);
    sal_memset(pports, 0, pport_max);

    /* lport and pport map */
    for (lport = 0, pc = resource->current; lport < lport_max; lport++, pc++) {
        if (pc->valid) {
            lports[lport] = 1;
            pport = pc->pport;
            lanes = pc->lanes;
            for (j = 0; j < lanes; ) {
                pports[pport ++] = 1;
                j += MIN_LANES_PER_PPORT(unit);
            }
        }
    }

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            /* delete: lport must in map */
            lport = po->lport;
            if (!lports[lport]) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit, "Delete port %d doesn't exist\n"),
                           lport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            lports[lport] = 0;

            /* delete: pport always in map if lport in, no check */
            pport = po->pport;
            lanes = po->lanes;
            for (j = 0; j < lanes; ) {
                pports[pport ++] = 0;
                j += MIN_LANES_PER_PPORT(unit);
            }
        } else {
            lport = po->lport;
            pport = po->pport;
            lanes = po->lanes;

            /* if pport and lanes don't change, allow not delete */
            pc = resource->current + lport;
            if ((pport == pc->pport) && (lanes == pc->lanes)) {
                continue;
            }

            /* add: lport must not in map */
            if (lports[lport]) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit, "Add port %d is already mapped\n"),
                           lport));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
            }
            lports[lport] = 1;

            /* add: pport must not in map */
            for (j = 0; j < lanes; ) {
                if (pports[pport]) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "Add physical port %d is already "
                                          "taken by other port\n"),
                               pport));
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
                }
                pports[pport ++] = 1;
                j += MIN_LANES_PER_PPORT(unit);
            }
        }
    }

exit:
    SHR_FREE(lports);
    SHR_FREE(pports);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check pmd lane configurations are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_lane_config_validate(int unit,
                                          ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    ltsw_flexport_port_cfg_t *po;
    const char *medium;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if ((po->op & FLEXPORT_OP_DETACH) ||
            (po->pmd == BCM_PORT_RESOURCE_DEFAULT_REQUEST)) {
            continue;
        }

        medium = id2name(lane_config_medium_type_map,
                         COUNTOF(lane_config_medium_type_map),
                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(po->pmd));
        if (medium == NULL) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Wrong lane config 0x%x on port %d\n"),
                       po->pmd, po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check link training configurations are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_link_training_validate(int unit,
                                            ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        if ((po->training > 1) || (po->training < 0)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Link Training must be 0 or 1 on port %d\n"),
                       po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_ltsw_port_pm_id_get(int unit, bcm_port_t port, int *pm_id)
{
    pc_phys_port_data_t phy_port_data;
    int pport;

    SHR_FUNC_ENTER(unit);

    pport = PORT(unit, port).pport;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phys_port_get(unit, pport, &phy_port_data));

    *pm_id = phy_port_data.pm_id;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pm_type_get(int unit, int pm_id, int *pm_type)
{
    pc_pm_prop_data_t pm_prop_data;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pm_type, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pm_prop_get(unit, pm_id, &pm_prop_data));
    *pm_type = name2id(pm_type_map, COUNTOF(pm_type_map), pm_prop_data.pm_type);
    if (*pm_type == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_enable_set(int unit, bcm_port_t port, int enable)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    port_data.enable = enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_ENABLE));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    pc_port_data_t port_data;
    pc_port_status_data_t status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));

    *enable = 0;
    if (port_data.enable && !status_data.phy_disabled) {
        *enable = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_autoneg_set(int unit, bcm_port_t port, int autoneg)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    if (autoneg) {
        port_data.autoneg = 1;
    } else {
        port_data.autoneg = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_AUTONEG));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_autoneg_get(int unit, bcm_port_t port, int *autoneg)
{
    pc_port_status_data_t port_status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    if (port_status_data.autoneg) {
        *autoneg = 1;
    } else {
        *autoneg = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_loopback_get(int unit, bcm_port_t port, int *loopback)
{
    pc_port_status_data_t port_status_data;
    int lpbk_mode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));
    lpbk_mode = name2id(lpbk_mode_map, COUNTOF(lpbk_mode_map), port_status_data.loopback);

    if (lpbk_mode == -1) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    *loopback = lpbk_mode;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    pc_port_data_t port_data;
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (loopback == BCM_PORT_LOOPBACK_PHY) {
        if (pm_type == ltswPmTypePm4x10) {
            port_data.lpbk_mode = PC_LPBK_PCSs;
        } else {
            port_data.lpbk_mode = PC_LPBK_PMDs;
        }
    } else {
        port_data.lpbk_mode = id2name(lpbk_mode_map, COUNTOF(lpbk_mode_map), loopback);
        SHR_NULL_CHECK(port_data.lpbk_mode, SHR_E_FAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_LPBK_MODE));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_link_status_get(int unit, bcm_port_t port, int *status)
{
    int cur_link;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_link_state_get(unit, port, &cur_link));

    *status = cur_link;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_speed_max(int unit, bcm_port_t port, int *speed)
{
    int max_speed = 0;
    int i;
    pc_port_ability_data_t port_ability_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_ability_get(unit, port, PC_PORT_ABILITY_LOCAL, &port_ability_data));

    for (i = 0; i < port_ability_data.num_abilities; i++) {
        if (port_ability_data.speed[i] > max_speed) {
            max_speed = port_ability_data.speed[i];
        }
    }

    *speed = max_speed;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_duplex_get(int unit, bcm_port_t port, int *duplex)
{
    SHR_FUNC_ENTER(unit);

    /* Always full duplex. */
    *duplex = BCM_PORT_DUPLEX_FULL;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_duplex_set(int unit, bcm_port_t port, int duplex)
{
    SHR_FUNC_ENTER(unit);

    /* Always full duplex. */
    if (duplex != BCM_PORT_DUPLEX_FULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fault_get(int unit, bcm_port_t port, uint32_t *flags)
{
    int local_fault, remote_fault;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_local_fault_status_get(unit, port, &local_fault));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_remote_fault_status_get(unit, port, &remote_fault));

    *flags = 0;

    if (remote_fault) {
        *flags |= BCM_PORT_FAULT_REMOTE;
    }
    if (local_fault) {
        *flags |= BCM_PORT_FAULT_LOCAL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_frame_max_get(int unit, bcm_port_t port, int *size)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    /* Adjust for VLAN tagged packets. */
    *size = port_data.max_frame_size - 4;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_frame_max_set(int unit, bcm_port_t port, int size)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    if (size < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Adjust for VLAN tagged packets. */
    size += 4;

    if (size > DEFAULT_MAX_FRAME_SIZE(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    port_data.max_frame_size = size;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_MAX_FRAME_SIZE));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pause_get(int unit, bcm_port_t port, int *pause_tx, int *pause_rx)
{
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    *pause_tx = mac_control_data.pause_tx_oper;
    *pause_rx = mac_control_data.pause_rx_oper;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pause_set(int unit, bcm_port_t port, int pause_tx, int pause_rx)
{
    int ct_mode = 0;
    pc_mac_control_data_t mac_control_data;

    SHR_FUNC_ENTER(unit);

    mac_control_data.pause_tx = pause_tx;
    /* record but not enable rx when cut through enabled */
    if (PORT(unit, port).flags & CT_VALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_ct_mode_get(unit, port, &ct_mode));
    }

    mac_control_data.pause_rx = (ct_mode ? 0 : pause_rx);

    if (pause_rx) {
        ltsw_port_info[unit]->port_ha[port].flags |= HA_PAUSE_RX_EN;
    } else {
        ltsw_port_info[unit]->port_ha[port].flags &= ~HA_PAUSE_RX_EN;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_PAUSE_TX|PC_PAUSE_RX));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pause_addr_get(int unit, bcm_port_t port, bcm_mac_t mac)
{
    pc_mac_control_data_t mac_control_data;
    uint64_t addr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_get(unit, port, &mac_control_data));

    addr = mac_control_data.pause_addr;

    bcmi_ltsw_util_uint64_to_mac(mac, &addr);

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_pause_addr_set(int unit, bcm_port_t port, bcm_mac_t mac)
{
    pc_mac_control_data_t mac_control_data;
    uint64_t addr = 0;

    SHR_FUNC_ENTER(unit);

    bcmi_ltsw_util_mac_to_uint64(&addr, mac);

    mac_control_data.pause_addr = addr;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_PAUSE_ADDR));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_ifg_set(int unit, bcm_port_t port, int speed, bcm_port_duplex_t duplex, int ifg)
{
    pc_port_status_data_t port_status_data;
    pc_mac_control_data_t mac_control_data;
    pc_port_data_t port_data;
    int cur_duplex, cur_speed;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_duplex_get(unit, port, &cur_duplex));

    if (port_data.enable) {
        cur_speed = port_status_data.speed;
    } else {
        cur_speed = port_data.speed;
    }

    if ((speed == cur_speed)
        && (cur_duplex == duplex)) {
        mac_control_data.inter_frame_gap = ifg/8;
        mac_control_data.inter_frame_gap_auto = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_INTER_FRAME_GAP|PC_INTER_FRAME_GAP_AUTO));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_ifg_get(int unit, bcm_port_t port, int speed, bcm_port_duplex_t duplex, int *ifg)
{
    pc_mac_control_data_t mac_control_data;
    pc_port_status_data_t port_status_data;
    pc_port_data_t port_data;
    int cur_duplex, cur_speed;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_duplex_get(unit, port, &cur_duplex));

    if (port_data.enable) {
        cur_speed = port_status_data.speed;
    } else {
        cur_speed = port_data.speed;
    }

    if ((speed == cur_speed)
        && (cur_duplex == duplex)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_get(unit, port, &mac_control_data));
        *ifg = mac_control_data.inter_frame_gap_oper * 8;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_autoneg_ability_advert_set(int unit,
                                            bcm_port_t port,
                                            int num_ability,
                                            bcm_port_speed_ability_t *abilities)
{
    pc_port_data_t port_data;
    int max_speed, i, auto_new_idx, rv = SHR_E_NONE;
    pc_autoneg_profile_data_t an_ability;
    uint32 cur_num_ability, insert_num_ability = 0;
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    if (num_ability > MAX_NUM_AUTONEG_PROFILES) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_speed_max(unit, port, &max_speed));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    /*Autoneg abilities validation*/
    for (i = 0; i < num_ability; i++) {
        if (abilities[i].parallel_detect_en) {
            LOG_WARN(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                    "Warning: Port %s: "
                    "Parallel Detect is NOT supported!\n"),
                        bcmi_ltsw_port_name(unit, port)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (abilities[i].speed > max_speed) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (abilities[i].resolved_num_lanes > port_data.num_lanes) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_NULL_CHECK(id2name(fec_mode_map, COUNTOF(fec_mode_map),
                       abilities[i].fec_type), SHR_E_PARAM);

        SHR_NULL_CHECK(id2name(medium_type_map, COUNTOF(medium_type_map),
                       abilities[i].medium), SHR_E_PARAM);

        SHR_NULL_CHECK(id2name(pause_type_map, COUNTOF(pause_type_map),
                        abilities[i].pause), SHR_E_PARAM);

        SHR_NULL_CHECK(id2name(phy_an_mode_map, COUNTOF(phy_an_mode_map),
                        abilities[i].an_mode), SHR_E_PARAM);

        SHR_NULL_CHECK(id2name(channel_type_map, COUNTOF(channel_type_map),
                abilities[i].channel), SHR_E_PARAM);

        if (abilities[i].medium != abilities[0].medium) {
            LOG_WARN(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                    "Warning: Port %s: "
                    "Medium conflicts among abilities!\n"),
                        bcmi_ltsw_port_name(unit, port)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (abilities[i].pause != abilities[0].pause) {
            LOG_WARN(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                    "Warning: Port %s: "
                    "Pause conflicts among abilities!\n"),
                        bcmi_ltsw_port_name(unit, port)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /*An mode and link training should not conflict among abilities on PM4x10*/
        if (pm_type == ltswPmTypePm4x10) {
            if (abilities[i].an_mode != abilities[0].an_mode) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (abilities[i].link_training != abilities[0].link_training) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /*25G with short channel doesn't support RS FEC in CL73 AnMode*/
        if (abilities[i].speed == 25000) {
            if ((abilities[i].channel == bcmPortPhyChannelShort)
                && (abilities[i].fec_type == bcmPortPhyFecRsFec)
                && (abilities[i].an_mode == bcmPortAnModeCL73)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

    /*Clear old autoneg abilities*/
    cur_num_ability = port_data.valid_ability_num;
    if (cur_num_ability != 0) {
        for (i= 0; i < cur_num_ability; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pc_profile_delete(unit, PC_AN_PROFILE_ID(&port_data, i), PROFILE_PC_AUTONEG(unit)));
            PC_AN_PROFILE_ID(&port_data, i) = PC_AN_PROFILE_ID_DEFAULT;
        }

        PC_AN_PROFILE_NUM(&port_data) = cur_num_ability;
        port_data.valid_ability_num = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_set(unit, port, &port_data,
                              PC_PORT_VALID_ABILITY_NUM | PC_PORT_AUTONEG_PROFILE_ID));
    }

    /*Insert new autoneg abilities*/
    for (i = 0; i < num_ability; i++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_autoneg_profile_data_init(unit, &an_ability));

        an_ability.advert_speed = abilities[i].speed;
        an_ability.num_lanes = abilities[i].resolved_num_lanes;

        if (pm_type == ltswPmTypePm4x10) {
            an_ability.link_training_off = !abilities[i].link_training;
        } else {
            an_ability.link_training_off = 0;
        }

        an_ability.fec_mode = id2name(fec_mode_map, COUNTOF(fec_mode_map),
                                      abilities[i].fec_type);
        an_ability.medium_type = id2name(medium_type_map, COUNTOF(medium_type_map),
                                         abilities[i].medium);
        an_ability.pause_type = id2name(pause_type_map, COUNTOF(pause_type_map),
                                         abilities[i].pause);

        if ((abilities[i].an_mode == bcmPortAnModeCL37)
            && (abilities[i].medium == BCM_PORT_MEDIUM_BACKPLANE)) {
            an_ability.autoneg_mode = PC_PHY_AUTONEG_MODE_SGMIIs;
        } else {
            an_ability.autoneg_mode = id2name(phy_an_mode_map, COUNTOF(phy_an_mode_map),
                                            abilities[i].an_mode);
        }

        if (abilities[i].channel == bcmPortPhyChannelLong) {
            an_ability.long_ch = 1;
        } else {
            an_ability.long_ch = 0;
        }

        rv = bcmint_ltsw_pc_profile_add(unit, &an_ability, 1,
                                        PROFILE_PC_AUTONEG(unit), &auto_new_idx);
        if (SHR_E_NONE != rv) {
            break;
        }

        PC_AN_PROFILE_ID(&port_data, i) = auto_new_idx;
        insert_num_ability++;
    }

    /*Update pc_port even though error returned in the abilities inserting.
      In case the enties are residual in the profile management tales and LTs.*/
    PC_AN_PROFILE_NUM(&port_data) = insert_num_ability;
    port_data.valid_ability_num = insert_num_ability;
    rv = ltsw_pc_port_set(unit, port, &port_data,
                        PC_PORT_AUTONEG_PROFILE_ID | PC_PORT_VALID_ABILITY_NUM);
    if (rv != SHR_E_NONE) {
        /*Delete PC_AUTONEG_PROFILE entries which aren't configured in PC_PORT*/
        for (i = 0; i < insert_num_ability; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pc_profile_delete(unit, PC_AN_PROFILE_ID(&port_data, i), PROFILE_PC_AUTONEG(unit)));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_autoneg_ability_advert_get(int unit,
                                     bcm_port_t port,
                                     int max_num_ability,
                                     bcm_port_speed_ability_t *abilities,
                                     int *actual_num_ability)
{
    pc_port_ability_data_t port_ability_data;
    int i;
    int valid = 0;
    int speed, lane, fec, autoneg, medium, pause, channel, link_training;
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_ability_get(unit, port, PC_PORT_ABILITY_ADVERT, &port_ability_data));

    if (port_ability_data.num_abilities > max_num_ability) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (port_ability_data.num_abilities > PC_PORT_ABILITY_NUM_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *actual_num_ability = 0;

    if (pm_type == ltswPmTypePm4x10) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_an_link_training_get(unit, port, &link_training));
    } else {
        link_training = 0;
    }

    for (i = 0; i < port_ability_data.num_abilities; i++) {

        speed = port_ability_data.speed[i];
        lane = port_ability_data.num_lane[i];

        fec = name2id(fec_mode_map, COUNTOF(fec_mode_map), port_ability_data.fec_mode[i]);
        if (fec == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        autoneg = name2id(phy_an_mode_map, COUNTOF(phy_an_mode_map), port_ability_data.autoneg_mode[i]);
        if (autoneg == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        medium = name2id(medium_type_map, COUNTOF(medium_type_map), port_ability_data.medium_type[i]);
        if (medium == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        pause = name2id(pause_type_map, COUNTOF(pause_type_map), port_ability_data.pause_type[i]);
        if (pause == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        channel = name2id(channel_type_map, COUNTOF(channel_type_map), port_ability_data.channel_type[i]);
        if (channel == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        abilities[i].speed = speed;
        abilities[i].resolved_num_lanes = lane;
        abilities[i].fec_type = fec;
        abilities[i].an_mode = autoneg;
        abilities[i].medium = medium;
        abilities[i].pause = pause;
        abilities[i].channel = channel;
        abilities[i].link_training = link_training;

        valid++;
    }

    *actual_num_ability = valid;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_speed_ability_local_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_port_speed_ability_t *abilities,
                                     int *actual_num_ability)
{
    pc_port_ability_data_t port_ability_data;
    int i;
    int valid = 0;
    int speed, lane, fec, autoneg, medium, pause, channel;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_ability_get(unit, port, PC_PORT_ABILITY_LOCAL, &port_ability_data));

    if (port_ability_data.num_abilities > max_num_ability) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (port_ability_data.num_abilities > PC_PORT_ABILITY_NUM_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *actual_num_ability = 0;

    for (i = 0; i < port_ability_data.num_abilities; i++) {

        speed = port_ability_data.speed[i];
        lane = port_ability_data.num_lane[i];

        fec = name2id(fec_mode_map, COUNTOF(fec_mode_map), port_ability_data.fec_mode[i]);
        if (fec == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        autoneg = name2id(phy_an_mode_map, COUNTOF(phy_an_mode_map), port_ability_data.autoneg_mode[i]);
        if (autoneg == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        medium = name2id(medium_type_map, COUNTOF(medium_type_map), port_ability_data.medium_type[i]);
        if (medium == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        pause = name2id(pause_type_map, COUNTOF(pause_type_map), port_ability_data.pause_type[i]);
        if (pause == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        channel = name2id(channel_type_map, COUNTOF(channel_type_map), port_ability_data.channel_type[i]);
        if (channel == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        abilities[i].speed = speed;
        abilities[i].resolved_num_lanes = lane;
        abilities[i].fec_type = fec;
        abilities[i].an_mode = autoneg;
        abilities[i].medium = medium;
        abilities[i].pause = pause;
        abilities[i].channel = channel;

        valid++;
    }

    *actual_num_ability = valid;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_encap_set(int unit, bcm_port_t port, int mode)
{
    int old_mode, en;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_encap_get(unit, port, &old_mode));

    if (mode != old_mode) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_enable_get(unit, port, &en));
        if (en) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_enable_set(unit, port, 0));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_encap_set(unit, port, mode));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_name_update(unit));

        if (en) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_enable_set(unit, port, 1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_ltsw_port_encap_get(int unit, bcm_port_t port, int *mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_encap_get(unit, port, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_encap_init(int unit)
{
    int port, mode, skip;
    char *s;

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    PORT_FP_ITER(unit, port) {
        s = bcmi_ltsw_property_port_str_get(unit, port, BCMI_CPN_ENCAP_MODE);
        skip = 0;
        if (!s) {
            mode = BCM_PORT_ENCAP_IEEE;
        } else if (sal_strcasecmp(s, "IEEE") == 0) {
            mode = BCM_PORT_ENCAP_IEEE;
        } else if (sal_strcasecmp(s, "HIGIG3") == 0) {
            mode = BCM_PORT_ENCAP_HIGIG3;
            skip = !(PORT(unit, port).flags & HG_VALID);
        } else {
            skip = 1;
        }

        if (!skip) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_encap_set(unit, port, mode));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_ltsw_port_resource_get(int unit,
                           bcm_gport_t port, bcm_port_resource_t *resource)
{
    pc_port_phys_map_data_t port_map_data;
    pc_port_status_data_t port_status_data;
    pc_port_data_t port_data;
    pc_pmd_fw_data_t status_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_phys_map_get(unit, port, &port_map_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    bcm_port_resource_t_init(resource);

    resource->flags = 0;
    resource->port = port;
    resource->physical_port = port_map_data.pport;
    resource->link_training = port_status_data.link_training;
    resource->lanes = port_status_data.num_lanes;

    if (port_data.enable) {
        resource->speed = port_status_data.speed;
        resource->fec_type = name2id(fec_mode_map, COUNTOF(fec_mode_map),
                                     port_status_data.fec_mode);
    } else {
        resource->speed = port_data.speed;
        resource->fec_type = name2id(fec_mode_map, COUNTOF(fec_mode_map),
                                     port_data.fec_mode);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_encap_get(unit, port, (int*)&resource->encap));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pmd_fw_status_get(unit, port, &status_data));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_lane_config_encode(unit, port, &status_data, &resource->phy_lane_config));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_resource_multi_set(int unit,
                                 int nport, bcm_port_resource_t *resource)
{
    int size, num_port = PORT_NUM_MAX(unit);
    int num_pm = ltsw_port_info[unit]->pm_num_max;
    ltsw_flexport_resource_t *rsc = NULL;

    SHR_FUNC_ENTER(unit);

    /* allocate memory for flexport resource structure */
    size = sizeof(ltsw_flexport_resource_t)
         + sizeof(ltsw_flexport_port_info_t) * (2 * num_port)
         + sizeof(ltsw_flexport_port_cfg_t) * nport
         + sizeof(ltsw_flexport_pm_cfg_t) * num_pm;
    SHR_ALLOC(rsc, size, "flexport resource");
    SHR_NULL_CHECK(rsc, SHR_E_MEMORY);
    sal_memset(rsc, 0, size);
    rsc->num_port = num_port;
    rsc->num_pm = num_pm;
    rsc->num_op = nport;
    rsc->current = (ltsw_flexport_port_info_t*)(rsc + 1);
    rsc->operation = (ltsw_flexport_port_cfg_t*)(rsc->current + num_port);
    rsc->new = (ltsw_flexport_port_info_t*)(rsc->operation + nport);
    rsc->pm = (ltsw_flexport_pm_cfg_t*)(rsc->new + num_port);

    /* initialize port config structure */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_resource_init(unit, resource, rsc));

    /* input validation */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexport_input_validate(unit, rsc));

    /* needn't pause linkscan as LT can handle it */

    /* get type of flexPort operations */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_op_set(unit, rsc));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_pm_op_set(unit, rsc));

    /* resource validation */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexport_resource_validate(unit, rsc));

    /* detach BCM modules */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexport_resource_detach(unit, rsc));

    /* delete PC_PORT and other port-based PC LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_pc_del(unit, rsc));

    /* add PC_PORT and other port-based PC LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_pc_add(unit, rsc));

    /* attach BCM modules */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexport_resource_attach(unit, rsc));

    /* misc config */

    /* set encap */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_encap_set(unit, rsc));

    /* bring attached ports down */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexport_port_disable(unit, rsc));

    /* update port names */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_name_update(unit));

    /* update depedent modules */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexport_resource_status_update(unit, rsc));

exit:
    SHR_FREE(rsc);
    SHR_FUNC_EXIT();
}


int
bcmint_ltsw_port_resource_speed_multi_set(int unit,
                                       int nport, bcm_port_resource_t *resource)
{
    int idx, max_speed;
    int *loopback = NULL;
    int *enable = NULL;
    int *lm_mode = NULL;
    bcm_port_resource_t *local_resource = NULL, *prs, *prd;

    SHR_FUNC_ENTER(unit);

    if (nport < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_ALLOC(loopback, nport * sizeof(int), "bcmLtswPortLoopBack");
    SHR_NULL_CHECK(loopback, SHR_E_MEMORY);

    SHR_ALLOC(enable, nport * sizeof(int), "bcmLtswPortEnable");
    SHR_NULL_CHECK(enable, SHR_E_MEMORY);

    SHR_ALLOC(lm_mode, nport * sizeof(int), "bcmLtswPortLinkscanMode");
    SHR_NULL_CHECK(lm_mode, SHR_E_MEMORY);

    SHR_ALLOC(local_resource, 2 * nport * sizeof(bcm_port_resource_t),
              "local resource");
    SHR_NULL_CHECK(local_resource, SHR_E_MEMORY);

    for (idx = 0; idx < nport; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, resource[idx].port,
                                           &resource[idx].port));
        SHR_IF_ERR_VERBOSE_EXIT
            (port_fp_validate(unit, resource[idx].port));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_speed_max(unit, resource[idx].port, &max_speed));

        if (resource[idx].speed < 0 || resource[idx].speed > max_speed) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else if (resource[idx].speed == 0) {
            resource[idx].speed = max_speed;
        }
    }

    /*Clear enable/loopback, disable AN, set Linkscan mode None*/
    for (idx = 0; idx < nport; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_loopback_get(unit, resource[idx].port, &loopback[idx]));

        if (loopback[idx] != BCM_PORT_LOOPBACK_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_loopback_set(unit, resource[idx].port, BCM_PORT_LOOPBACK_NONE));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_enable_get(unit, resource[idx].port, &enable[idx]));
        if (enable[idx] == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_enable_set(unit, resource[idx].port, 0));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_autoneg_set(unit,resource[idx].port,0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_linkscan_mode_get(unit, resource[idx].port, &lm_mode[idx]));
        if (lm_mode[idx] != BCM_LINKSCAN_MODE_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_linkscan_mode_set(unit, resource[idx].port, BCM_LINKSCAN_MODE_NONE));
        }
    }

    /* call flexport to change speed */
    prs = resource;
    prd = local_resource;
    for (idx = 0; idx < nport; idx++, prs ++, prd ++) {
        bcm_port_resource_t_init(prd);
        prd->flags = prs->flags | FLEXPORT_FLG_NO_REMAP;
        prd->port = prs->port;
        prd->physical_port = -1;
    }
    prs = resource;
    for (idx = 0; idx < nport; idx++, prs ++, prd ++) {
        bcm_port_resource_t_init(prd);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_resource_get(unit, prs->port, prd));
        prd->flags = prs->flags | FLEXPORT_FLG_NO_REMAP;
        prd->port = prs->port;
        prd->speed = prs->speed;
        prd->fec_type = prs->fec_type;
        prd->phy_lane_config = prs->phy_lane_config;
        prd->link_training = prs->link_training;
    }

    SHR_IF_ERR_CONT
        (bcmint_ltsw_port_resource_multi_set(unit, nport * 2, local_resource));
    if (SHR_FUNC_ERR()) {
        /* Restore LM mode */
        for (idx = 0; idx < nport; idx++) {
            (void) bcm_ltsw_linkscan_mode_set(unit, resource[idx].port, lm_mode[idx]);
        }

        SHR_EXIT();
    }

    /*Restore LM mode, loopback and enable*/
    for (idx = 0; idx < nport; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_linkscan_mode_set(unit, resource[idx].port, lm_mode[idx]));

        if (loopback[idx] != BCM_PORT_LOOPBACK_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_loopback_set(unit, resource[idx].port, loopback[idx]));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_enable_set(unit, resource[idx].port, enable[idx]));
     }

exit:
    if (NULL != loopback) {
        SHR_FREE(loopback);
    }
    if (NULL != enable) {
        SHR_FREE(enable);
    }
    if (NULL != local_resource) {
        SHR_FREE(local_resource);
    }
    if (NULL != lm_mode) {
        SHR_FREE(lm_mode);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_rlm_config_set(int unit, bcm_port_t port,
                            bcm_port_rlm_config_t *rlm_config, int enable)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        port_data.rlm = enable;
        port_data.initiator = rlm_config->is_initiator;
        port_data.active_lane_mask = rlm_config->active_lane_bit_map;
    } else {
        port_data.rlm = 0;
        port_data.initiator = 0;
        port_data.active_lane_mask = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_RLM|PC_PORT_INITIATOR|PC_PORT_ACTIVE_LANE_MASK));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_rlm_config_get(int unit, bcm_port_t port,
                            bcm_port_rlm_config_t *rlm_config, int *enable)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    *enable = port_data.rlm;
    rlm_config->is_initiator = port_data.initiator;
    rlm_config->active_lane_bit_map = port_data.active_lane_mask;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_rlm_status_get(int unit, bcm_port_t port,
                            bcm_port_rlm_status_t *rlm_status)
{
    pc_port_status_data_t port_status_data;
    bcm_port_rlm_status_t current_rlm_status = bcmPortRlmCount;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    current_rlm_status = name2id(rlm_status_type_map, COUNTOF(rlm_status_type_map),
                             port_status_data.rlm_status);
    if (current_rlm_status < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *rlm_status = current_rlm_status;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_timesync_config_set(int unit, bcm_port_t port,
                                     bcm_port_phy_timesync_config_t *conf)
{
    pc_port_timesync_data_t timesync_data;
    int timesync_enable, is_one_step;

    SHR_FUNC_ENTER(unit);

    timesync_enable = (conf->flags & BCM_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0;
    is_one_step = (conf->flags & BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE) ? 1 : 0;

    timesync_data.ieee_1588 = timesync_enable;
    timesync_data.one_step_timestamp = is_one_step;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_timesync_set(unit, port, &timesync_data, PC_IEEE_1588 | PC_ONE_STEP_TIMESTAMP));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_timesync_config_get(int unit, bcm_port_t port,
                                     bcm_port_phy_timesync_config_t *conf)
{
    pc_port_timesync_data_t timesync_data;
    int timesync_enable, is_one_step;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_timesync_get(unit, port, &timesync_data));

    timesync_enable = timesync_data.ieee_1588;
    is_one_step = timesync_data.one_step_timestamp;

    conf->flags = 0;

    conf->flags |= timesync_enable ? BCM_PORT_PHY_TIMESYNC_ENABLE : 0;
    conf->flags |= is_one_step ? BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE : 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_control_phy_timesync_set(int unit, bcm_port_t port,
                                      bcm_port_control_phy_timesync_t type,
                                      uint64 value)
{
    pc_port_timesync_data_t timesync_data;

    SHR_FUNC_ENTER(unit);

    if (type == bcmPortControlPhyTimesyncTimestampAdjust) {
        timesync_data.ts_comp_mode = id2name(ts_comp_mode_map, COUNTOF(ts_comp_mode_map), value);
        SHR_NULL_CHECK(timesync_data.ts_comp_mode, SHR_E_PARAM);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_port_timesync_set(unit, port, &timesync_data, PC_TS_COMP_MODE));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_timesync_tx_info_get(int unit, bcm_port_t port,
                                  bcm_port_timesync_tx_info_t *tx_info)
{
    bcmpc_pmgr_port_timestamp_t pc_port_timestamp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_tx_timestamp_info_get(unit, port, &pc_port_timestamp));

    tx_info->sequence_id = pc_port_timestamp.sequence_id;
    tx_info->sub_ns = pc_port_timestamp.timestamp_sub_nanosec;
    COMPILER_64_SET(tx_info->timestamp, pc_port_timestamp.timestamp_hi, pc_port_timestamp.timestamp_lo);

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fdr_config_set(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config)
{
    pc_fdr_control_data_t fdr_ctrl_data;
    uint32_t field_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (fdr_config->fdr_enable) {
        fdr_ctrl_data.fdr_enable = 1;
    } else {
        fdr_ctrl_data.fdr_enable = 0;
    }
    field_bmp |= PC_FDR_ENABLE;

    fdr_ctrl_data.symb_err_thres = fdr_config->symb_err_threshold;
    field_bmp |= PC_FDR_SYMBOL_ERROR_THRESHOLD;

    fdr_ctrl_data.symb_err_start_val = fdr_config->symb_err_stats_sel;
    field_bmp |= PC_FDR_SYMBOL_ERROR_START_VALUE;

    if (fdr_config->intr_enable) {
        fdr_ctrl_data.inter_enable = 1;
    } else {
        fdr_ctrl_data.inter_enable = 0;
    }
    field_bmp |= PC_FDR_INTERRUPT_ENABLE;

    fdr_ctrl_data.symb_err_win =
        id2name(fdr_symb_err_window_size_map, COUNTOF(fdr_symb_err_window_size_map), fdr_config->window_size);
    SHR_NULL_CHECK(fdr_ctrl_data.symb_err_win, SHR_E_PARAM);
    field_bmp |= PC_FDR_SYMBOL_ERROR_WINDOW;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_fdr_control_set(unit, port, &fdr_ctrl_data, field_bmp));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fdr_config_get(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config)
{
    pc_fdr_control_data_t fdr_ctrl_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_fdr_control_get(unit, port, &fdr_ctrl_data));

    fdr_config->fdr_enable = fdr_ctrl_data.fdr_enable;
    fdr_config->intr_enable = fdr_ctrl_data.inter_enable;
    fdr_config->symb_err_stats_sel = fdr_ctrl_data.symb_err_start_val;
    fdr_config->symb_err_threshold = fdr_ctrl_data.symb_err_thres;
    fdr_config->window_size =
        name2id(fdr_symb_err_window_size_map, COUNTOF(fdr_symb_err_window_size_map), fdr_ctrl_data.symb_err_win);
    if (fdr_config->window_size < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fdr_stats_get(int unit, int port, uint32_t field_bmp, bcm_port_fdr_stats_t *fdr_stats)
{
    pc_fdr_stats_data_t fdr_stats_data;

    SHR_FUNC_ENTER(unit);

    sal_memset(&fdr_stats_data, 0, sizeof(pc_fdr_stats_data_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_fdr_stats_get(unit, port, field_bmp, &fdr_stats_data));

    fdr_stats->start_time = fdr_stats_data.start_time;
    fdr_stats->end_time = fdr_stats_data.end_time;
    fdr_stats->num_uncorrect_cws = fdr_stats_data.num_uncorrect_cws;
    fdr_stats->num_cws = fdr_stats_data.num_cws;
    fdr_stats->num_symb_errs = fdr_stats_data.num_symb_errs;
    fdr_stats->cw_s0_errs = fdr_stats_data.cw_s0_errs;
    fdr_stats->cw_s1_errs = fdr_stats_data.cw_s1_errs;
    fdr_stats->cw_s2_errs = fdr_stats_data.cw_s2_errs;
    fdr_stats->cw_s3_errs = fdr_stats_data.cw_s3_errs;
    fdr_stats->cw_s4_errs = fdr_stats_data.cw_s4_errs;
    fdr_stats->cw_s5_errs = fdr_stats_data.cw_s5_errs;
    fdr_stats->cw_s6_errs = fdr_stats_data.cw_s6_errs;
    fdr_stats->cw_s7_errs = fdr_stats_data.cw_s7_errs;
    fdr_stats->cw_s8_errs = fdr_stats_data.cw_s8_errs;

exit:
    SHR_FUNC_EXIT();
}

void
ltsw_pc_fdr_stats_parser(int unit, const char *lt_name, uint32_t entry_hdl,
                     void *notif_info, bcmi_ltsw_event_status_t *status)
{
    bcmlt_entry_handle_t eh = entry_hdl;
    bcmlt_unified_opcode_t op;
    port_fdr_stats_notif_t *pfsn = (port_fdr_stats_notif_t *)notif_info;
    uint64_t fval;
    int rv;

    sal_memset(pfsn, 0, sizeof(port_fdr_stats_notif_t));

    rv = bcmlt_entry_oper_get(eh, &op);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse opcode of %s event (%d).\n"),
                  lt_name, rv));
        return;
    }
    pfsn->op = op.opcode.lt;

    rv = bcmlt_entry_field_get(eh, PORT_IDs, &fval);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, PORT_IDs, rv));
        return;
    }
    pfsn->port = fval;

    if ((pfsn->op == BCMLT_OPCODE_DELETE) || (pfsn->op == BCMLT_OPCODE_INSERT)) {
        *status = bcmiLtswEventStatusDismissNotif;
        return;
    }

    rv = bcmlt_entry_field_get(eh, FDR_START_TIMEs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->start_time = fval;
    }

    rv = bcmlt_entry_field_get(eh, FDR_END_TIMEs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->end_time = fval;
    }

    rv = bcmlt_entry_field_get(eh, UNCORRECTABLE_CODE_WORDSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->num_uncorrect_cws = fval;

    }

    rv = bcmlt_entry_field_get(eh, CODE_WORDSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->num_cws = fval;
    }

    rv = bcmlt_entry_field_get(eh, SYMBOL_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->num_symb_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S0_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s0_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S1_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s1_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S2_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s2_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S3_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s3_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S4_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s4_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S5_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s5_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S6_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s6_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S7_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s7_errs = fval;
    }

    rv = bcmlt_entry_field_get(eh, CODE_WORD_S8_ERRORSs, &fval);
    if (SHR_SUCCESS(rv)) {
        pfsn->cw_s8_errs = fval;
    }

}

void
ltsw_port_fdr_states_ev_cb(int unit, const char *event, void *notif_info, void *data)
{
    port_fdr_stats_notif_t *pfsn = (port_fdr_stats_notif_t *)notif_info;
    bcm_port_fdr_stats_t fdr_stats;
    port_fdr_cb_t *item, *next;

    fdr_stats.start_time = pfsn->start_time;
    fdr_stats.end_time = pfsn->end_time;
    fdr_stats.num_uncorrect_cws = pfsn->num_uncorrect_cws;
    fdr_stats.num_cws = pfsn->num_cws;
    fdr_stats.num_symb_errs = pfsn->num_symb_errs;
    fdr_stats.cw_s0_errs = pfsn->cw_s0_errs;
    fdr_stats.cw_s1_errs = pfsn->cw_s1_errs;
    fdr_stats.cw_s2_errs = pfsn->cw_s2_errs;
    fdr_stats.cw_s3_errs = pfsn->cw_s3_errs;
    fdr_stats.cw_s4_errs = pfsn->cw_s4_errs;
    fdr_stats.cw_s5_errs = pfsn->cw_s5_errs;
    fdr_stats.cw_s6_errs = pfsn->cw_s6_errs;
    fdr_stats.cw_s7_errs = pfsn->cw_s7_errs;
    fdr_stats.cw_s8_errs = pfsn->cw_s8_errs;

    item = PORT_FDR_CBS(unit);
    while (item) {
        next = item->next;
        if (item->cb) {
            item->cb(unit, pfsn->port, &fdr_stats, item->user_data);
        }
        item = next;
    }
}

int
bcmint_ltsw_port_fdr_callback_register(int unit, bcm_port_fdr_handler_t f, void* user_data)
{
    port_fdr_cb_t *item;
    bool locked = false;
    bool first_cb = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    PORT_INIT(unit);

    PORT_LOCK(unit);
    locked = true;

    item = PORT_FDR_CBS(unit);
    if (item) {
        while (item) {
            if (item->cb == f) {
                break;
            }
            item = item->next;
        }
        if (item) {
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        first_cb = true;
    }

    item = sal_alloc(sizeof(port_fdr_cb_t), "ltswPortFdrCallback");
    SHR_NULL_CHECK(item, SHR_E_MEMORY);
    sal_memset(item, 0, sizeof(port_fdr_cb_t));

    item->cb = f;
    item->user_data = user_data;

    item->next = PORT_FDR_CBS(unit);
    PORT_FDR_CBS(unit) = item;

    PORT_UNLOCK(unit);
    locked = false;

    if (first_cb) {

        SHR_IF_ERR_EXIT
            (bcmi_lt_table_parser_set(unit, PC_FDR_STATSs,
                                      ltsw_pc_fdr_stats_parser,
                                      sizeof(port_fdr_stats_notif_t)));
        SHR_IF_ERR_EXIT
            (bcmi_lt_table_attrib_set(unit, PC_FDR_STATSs,
                                      (BCMI_LTSW_EVENT_HIGH_PRIORITY |
                                       BCMI_LTSW_EVENT_APPL_CALLBACK)));
        SHR_IF_ERR_EXIT
            (bcmi_lt_table_subscribe(unit, PC_FDR_STATSs, ltsw_port_fdr_states_ev_cb,
                                     NULL));
    }

exit:
    if (locked) {
        PORT_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fdr_callback_unregister(int unit, bcm_port_fdr_handler_t f, void* user_data)
{
    port_fdr_cb_t *item, *pre;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    PORT_INIT(unit);

    PORT_LOCK(unit);
    locked = true;

    item = PORT_FDR_CBS(unit);
    while (item) {
        if (item->cb == f) {
            break;
        }
        pre = item;
        item = item->next;
    }
    if (!item) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    } else if (item == PORT_FDR_CBS(unit)) {
        PORT_FDR_CBS(unit) = item->next;
    } else {
        pre->next = item->next;
    }
    sal_free(item);

    PORT_UNLOCK(unit);
    locked = false;

    if (!PORT_FDR_CBS(unit)) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_table_unsubscribe(unit, PC_FDR_STATSs, ltsw_port_fdr_states_ev_cb));
    }

exit:
    if (locked) {
        PORT_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_port_fec_mode_get(int unit, bcm_port_t port, bcm_port_phy_fec_t* fec_mode)
{
    pc_port_status_data_t port_status_data;
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    if (port_data.enable) {
        *fec_mode = name2id(fec_mode_map, COUNTOF(fec_mode_map), port_status_data.fec_mode);
    } else {
        *fec_mode = name2id(fec_mode_map, COUNTOF(fec_mode_map), port_data.fec_mode);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_port_fec_get(int unit, bcm_port_t port, bcm_port_phy_fec_t* fec)
{
    bcm_port_phy_fec_t fec_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fec, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_fec_mode_get(unit, port, &fec_mode));

   *fec = fec_mode;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_port_pm_id_get(int unit, bcm_port_t port, int *pm_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pm_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, pm_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Identifies if given modid is local on a given unit.
 *
 * \param [in] unit Unit number.
 * \param [in] modid Module ID.
 * \param [out] result TRUE if modid is local, FALSE otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_modid_is_local(int unit, bcm_module_t modid, int *result)
{
    bcm_module_t    mymodid;
    int             rv;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(result, SHR_E_PARAM);

    /* Get local module id. */
    rv = bcm_ltsw_stk_my_modid_get(unit, &mymodid);
    if (SHR_E_UNAVAIL == rv) {
        if (BCM_MODID_INVALID == modid) {
            *result = TRUE;
        } else {
            *result = FALSE;
        }
        SHR_EXIT();
    }

    if ((modid >= mymodid) && (modid < mymodid + NUM_MODID(unit))) {
        *result = TRUE;
    } else {
        *result = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize gport_info structure.
 *
 * \param [inout] gport_info Structure to initialize.
 *
 * \return None.
 */
void
bcmi_ltsw_gport_info_t_init(bcmi_ltsw_gport_info_t *gport_info)
{
    sal_memset(gport_info, 0, sizeof (bcmi_ltsw_gport_info_t));
}


/*!
 * \brief Helper funtion to Remaps module and port from HW space to API space.
 *
 * If mod_out == NULL then port_out will be local logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] mod_in Module ID to map.
 * \param [in] port_in Port number to map.
 * \param [out] mod_out Module ID after mapping.
 * \param [out] port_out Port number after mapping.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_modport_hw2api_map(int unit, bcm_module_t mod_in, bcm_port_t port_in,
                             bcm_module_t *mod_out, bcm_port_t *port_out)
{
    int             isLocal = 0;
    bcm_module_t    mymodid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(port_out, SHR_E_PARAM);

    if (NUM_MODID(unit) == 1) { /* single modid */
        if (mod_out) {
            *mod_out = mod_in;
        }
        *port_out = port_in;
    } else if (mod_out) { /* multiple modid */
        *port_out = port_in % (bcmi_ltsw_dev_max_modport(unit) + 1);
        *mod_out = mod_in + port_in / (bcmi_ltsw_dev_max_modport(unit) + 1);
    } else { /* local logical port requested */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_modid_is_local(unit, mod_in, &isLocal));
        if (!isLocal) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &mymodid));
        *port_out = port_in +
                    (mod_in - mymodid) * (bcmi_ltsw_dev_max_modport(unit) + 1);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Helper funtion to construct a gport from given information.
 *
 * The modid and port are translated from the local space to application space.
 *
 * \param [in] unit Unit number.
 * \param [in] gport_info Structure that contains information to encode gport.
 * \param [out] gport Global port identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_gport_construct(int unit, bcmi_ltsw_gport_info_t *gport_info,
                               bcm_gport_t *gport)
{
    bcm_gport_t     l_gport = 0;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gport_info, SHR_E_PARAM);
    SHR_NULL_CHECK(gport, SHR_E_PARAM);

    switch (gport_info->gport_type) {
        case _SHR_GPORT_TYPE_TRUNK:
            BCM_GPORT_TRUNK_SET(l_gport, gport_info->tgid);
            break;
        case _SHR_GPORT_TYPE_LOCAL_CPU:
            l_gport = BCM_GPORT_LOCAL_CPU;
            break;
        case _SHR_GPORT_TYPE_BLACK_HOLE:
            l_gport = BCM_GPORT_BLACK_HOLE;
            break;
        case _SHR_GPORT_TYPE_LOCAL:
            BCM_GPORT_LOCAL_SET(l_gport, gport_info->port);
            break;
        case _SHR_GPORT_TYPE_SUBPORT_GROUP:
            BCM_GPORT_SUBPORT_GROUP_SET(l_gport, gport_info->subport_id);
            break;
        case _SHR_GPORT_TYPE_MPLS_PORT:
            BCM_GPORT_MPLS_PORT_ID_SET(l_gport, gport_info->mpls_id);
            break;
        case _SHR_GPORT_TYPE_MIM_PORT:
            BCM_GPORT_MIM_PORT_ID_SET(l_gport, gport_info->mim_id);
            break;
        case _SHR_GPORT_TYPE_WLAN_PORT:
            BCM_GPORT_WLAN_PORT_ID_SET(l_gport, gport_info->wlan_id);
            break;
        case _SHR_GPORT_TYPE_TRILL_PORT:
            BCM_GPORT_TRILL_PORT_ID_SET(l_gport, gport_info->trill_id);
            break;
        case _SHR_GPORT_TYPE_NIV_PORT:
            BCM_GPORT_NIV_PORT_ID_SET(l_gport, gport_info->niv_id);
            break;
        case _SHR_GPORT_TYPE_L2GRE_PORT:
            BCM_GPORT_L2GRE_PORT_ID_SET(l_gport, gport_info->l2gre_id);
            break;
        case _SHR_GPORT_TYPE_VXLAN_PORT:
            BCM_GPORT_VXLAN_PORT_ID_SET(l_gport, gport_info->vxlan_id);
            break;
        case _SHR_GPORT_TYPE_FLOW_PORT:
            BCM_GPORT_FLOW_PORT_ID_SET(l_gport, gport_info->flow_id);
            break;
        case _SHR_GPORT_TYPE_VLAN_PORT:
            BCM_GPORT_VLAN_PORT_ID_SET(l_gport, gport_info->vlan_vp_id);
            break;
        case _SHR_GPORT_TYPE_EXTENDER_PORT:
            BCM_GPORT_EXTENDER_PORT_ID_SET(l_gport, gport_info->extender_id);
            break;
        case _SHR_GPORT_TYPE_DEVPORT:
            BCM_GPORT_DEVPORT_SET(l_gport, unit, gport_info->port);
            break;
        case _SHR_GPORT_TYPE_MODPORT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_modport_hw2api_map
                    (unit, gport_info->modid, gport_info->port,
                     &mod_out, &port_out));
            BCM_GPORT_MODPORT_SET(l_gport, mod_out, port_out);
            break;
        default:
            return BCM_E_PARAM;
    }

    *gport = l_gport;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Helper funtion to validate port/gport parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] port_in Port / Gport to validate.
 * \param [out] port_out Port number if valid.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_gport_validate(int unit, bcm_port_t port_in, bcm_port_t *port_out)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_SET(port_in)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, port_in, port_out));
    } else if (PORT_IS_VALID(unit, port_in)) {
        *port_out = port_in;
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Helper funtion to validate PHY specific gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port / Gport to validate.
 * \param [out] local_port Local Port number.
 * \param [out] phyn Local Phy number.
 * \param [out] phy_lane Lane number.
 * \param [out] sys_side System side(1) or line side(0).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_gport_phyn_validate(int unit, bcm_port_t port,
                                   bcm_port_t *local_port, int *phyn,
                                   int *phy_lane, int *sys_side)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(local_port, SHR_E_PARAM);
    SHR_NULL_CHECK(phy_lane, SHR_E_PARAM);
    SHR_NULL_CHECK(phyn, SHR_E_PARAM);
    SHR_NULL_CHECK(sys_side, SHR_E_PARAM);

    if (BCM_PHY_GPORT_IS_PHYN(port)) {
        *local_port = BCM_PHY_GPORT_PHYN_PORT_PORT_GET(port);
        *phyn = BCM_PHY_GPORT_PHYN_PORT_PHYN_GET(port);
        *phy_lane = -1;
    } else if(BCM_PHY_GPORT_IS_PHYN_LANE(port)) {
        *local_port = BCM_PHY_GPORT_PHYN_LANE_PORT_PORT_GET(port);
        *phyn = BCM_PHY_GPORT_PHYN_LANE_PORT_PHYN_GET(port);
        *phy_lane = BCM_PHY_GPORT_PHYN_LANE_PORT_LANE_GET(port);
    } else if(BCM_PHY_GPORT_IS_PHYN_SYS_SIDE(port)) {
        *local_port = BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_GET(port);
        *phyn = BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_GET(port);
        *sys_side = TRUE;
        *phy_lane = -1;
    } else if(BCM_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(port)) {
        *local_port = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_GET(port);
        *phyn = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_GET(port);
        *phy_lane = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_GET(port);
        *sys_side = TRUE;
    } else if (BCM_PHY_GPORT_IS_LANE(port)) {
        *local_port = BCM_PHY_GPORT_LANE_PORT_PORT_GET(port);
        *phy_lane = BCM_PHY_GPORT_LANE_PORT_LANE_GET(port);
    } else {
        *local_port = -1;
        SHR_EXIT();
    }

    if (!PORT_IS_VALID(unit, *local_port)) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Helper funtion to get modid, port, and trunk_id from a bcm_gport_t.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Global port identifier.
 * \param [out] modid Module ID.
 * \param [out] port Port number.
 * \param [out] trunk_id Trunk ID.
 * \param [out] id HW ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_gport_resolve(int unit, bcm_gport_t gport,
                             bcm_module_t *modid, bcm_port_t *port,
                             bcm_trunk_t *trunk_id, int *id)
{
    int             local_id;
    bcm_module_t    mod_in, local_modid;
    bcm_port_t      port_in, local_port;
    bcm_trunk_t     local_tgid;

    SHR_FUNC_ENTER(unit);

    local_modid = -1;
    local_port = -1;
    local_id = -1;
    local_tgid = BCM_TRUNK_INVALID;

    if (BCM_GPORT_IS_TRUNK(gport)) {
        local_tgid = BCM_GPORT_TRUNK_GET(gport);
    } else if (BCM_GPORT_IS_LOCAL_CPU(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &local_modid));
        local_port = first_port(unit, BCMI_LTSW_PORT_TYPE_CPU);
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &local_modid));
        local_port = BCM_GPORT_LOCAL_GET(gport);
        if (!PORT_IS_VALID(unit, local_port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_DEVPORT(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &local_modid));
        if (unit != BCM_GPORT_DEVPORT_DEVID_GET(gport)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
        local_port = BCM_GPORT_DEVPORT_PORT_GET(gport);

        if (!PORT_IS_VALID(unit, local_port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        mod_in = BCM_GPORT_MODPORT_MODID_GET(gport);
        port_in = BCM_GPORT_MODPORT_PORT_GET(gport);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, port_in,
                                      &local_modid, &local_port));

        if (!MODID_VALID_RANGE(unit, local_modid)) {
            SHR_ERR_EXIT(SHR_E_BADID);
        }
        if (!PORT_VALID_RANGE(unit, local_port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_PROXY(gport)) {
        mod_in = BCM_GPORT_PROXY_MODID_GET(gport);
        port_in = BCM_GPORT_PROXY_PORT_GET(gport);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, port_in,
                                      &local_modid, &local_port));

        if (!MODID_VALID_RANGE(unit, local_modid)) {
            SHR_ERR_EXIT(SHR_E_BADID);
        }
        if (!PORT_VALID_RANGE(unit, local_port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_SCHEDULER(gport) ||
               BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
               BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &local_modid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &local_id));
    } else {
         /* BCM_GPORT_INVALID should return an error */
         SHR_ERR_EXIT(SHR_E_PORT);
    }

    *modid = local_modid;
    *port = local_port;
    *trunk_id = local_tgid;
    *id = local_id;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Traverse over all local ports and gports.
 *
 * \param [in] unit Unit number.
 * \param [in] gport_type Gport type to traverse (BCM_GPORT_TYPE_XXX).
 * \param [in] cb Callback function.
 * \param [in] user_data User data to a call back.
 *
 * \return Number of matched ports.
 */
int
bcmi_ltsw_port_gport_traverse(int unit, uint32_t gport_type,
                              bcmi_ltsw_port_traverse_cb cb, void *user_data)
{
    SHR_FUNC_ENTER(unit);

    switch (gport_type) {
        case BCM_GPORT_TYPE_NONE:
            /* local ports */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_traverse(unit, cb, user_data));
            break;

        case BCM_GPORT_TYPE_MODPORT:
            /* remote ports */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stk_modport_traverse(unit, cb, user_data));
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Traverse over all local ports.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] user_data User data to a call back.
 *
 * \return Number of matched ports.
 */
int
bcmi_ltsw_port_traverse(int unit, bcmi_ltsw_port_traverse_cb cb,
                        void *user_data)
{
    int rv, port;

    SHR_FUNC_ENTER(unit);

    PORT_ALL_ITER(unit, port) {
        rv = cb(unit, port, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the addressable local port from the given Gport.
 *
 * Gport must be types that deal with logical BCM ports (MODPORT, LOCAL, ...).
 * Logical port does not need to be a valid port, only addressable.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Global port identifier.
 * \param [out] local_port Local port encoded in gport.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_addressable_local_get(int unit,
                                     bcm_gport_t gport, bcm_port_t *local_port)
{
    bcm_module_t my_mod, encap_mod;
    bcm_port_t encap_port;
    int isLocal = 0;

    SHR_FUNC_ENTER(unit);

    if ((NUM_MODID(unit) == 0) && !BCM_GPORT_IS_DEVPORT(gport) ){
        /* Only devport gport can be supported on device with no modid */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!BCM_GPORT_IS_SET(gport)) {
        *local_port = gport;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
         *local_port = BCM_GPORT_LOCAL_GET(gport);
    } else if (BCM_GPORT_IS_LOCAL_CPU(gport)) {
        *local_port = first_port(unit, BCMI_LTSW_PORT_TYPE_CPU);
    } else if (BCM_GPORT_IS_DEVPORT(gport)) {
        *local_port = BCM_GPORT_DEVPORT_PORT_GET(gport);
        if (unit != BCM_GPORT_DEVPORT_DEVID_GET(gport)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_MODPORT(gport) || BCM_GPORT_IS_PROXY(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_mod));

        if (BCM_GPORT_IS_MODPORT(gport)) {
            encap_mod = BCM_GPORT_MODPORT_MODID_GET(gport);
            encap_port = BCM_GPORT_MODPORT_PORT_GET(gport);
        } else {
            encap_mod = BCM_GPORT_PROXY_MODID_GET(gport);
            encap_port = BCM_GPORT_PROXY_PORT_GET(gport);
        }

        if (encap_mod == my_mod){
            *local_port = encap_port;
        } else if (NUM_MODID(unit) > 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_modid_is_local(unit, encap_mod, &isLocal));
            if (isLocal) {
                *local_port = encap_port +
                  (encap_mod - my_mod) * (bcmi_ltsw_dev_max_modport(unit) + 1);
            } else {
                SHR_ERR_EXIT(SHR_E_PORT);
            }
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (!PORT_VALID_RANGE(unit, *local_port)) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Count ports which match identified port type(s).
 *
 * \param [in] unit Unit number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 *
 * \return Number of matched ports.
 */
int
bcmi_ltsw_port_number_count(int unit, uint32_t port_type)
{
    int port, count;
    for (port = 0, count = 0; port < PORT_NUM_MAX(unit); port ++) {
        if (PORT(unit, port).port_type & port_type) {
            count ++;
        }
    }
    return count;
}


/*!
 * \brief Find out ports which match identified port type(s).
 *
 * \param [in] unit Unit number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 * \param [out] pbmp Port bitmap of matched ports.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_bitmap(int unit, uint32_t port_type, bcm_pbmp_t *pbmp)
{
    int port;
    PORT_INIT(unit);

    BCM_PBMP_CLEAR(*pbmp);
    for (port = 0; port < PORT_NUM_MAX(unit); port ++) {
        if (PORT(unit, port).port_type & port_type)
            BCM_PBMP_PORT_ADD(*pbmp, port);
    }

    return SHR_E_NONE;
}


/*!
 * \brief Check if port is identified type(s).
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 *
 * \return TRUE if port is identified type(s).
 */
int
bcmi_ltsw_port_is_type(int unit, bcm_port_t port, uint32_t port_type)
{
    if (!PORT_IS_VALID(unit, port)) {
        return FALSE;
    }

    return ((PORT(unit, port).port_type & port_type) ? TRUE : FALSE);
}


/*!
 * \brief Helper functiom to return CPU port.
 *
 * \param [in] unit Unit number.
 *
 * \return CPU port.
 */
int
bcmi_ltsw_port_cpu(int unit)
{
    return first_port(unit, BCMI_LTSW_PORT_TYPE_CPU);
}


/*!
 * \brief Get pipe id of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] pipe Pipe of a port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_pipe_get(int unit, int port, int *pipe)
{
    if (!PORT_IS_VALID(unit, port)) {
        *pipe = -1;
        return SHR_E_PARAM;
    }
    *pipe = bcmi_ltsw_dev_logic_port_pipe(unit, port);
    return SHR_E_NONE;
}


/*!
 * \brief Get physical port of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return Physical port of the port.
 */
int
bcmi_ltsw_port_to_pport(int unit, int port)
{
    return PORT_IS_VALID(unit, port) ? PORT(unit, port).pport : -1;
}


/*!
 * \brief Get port of a physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return Port of the physical port.
 */
int
bcmi_ltsw_pport_to_port(int unit, int pport)
{
    return PHY_PORT_IS_VALID(unit, pport) ?
           ltsw_port_info[unit]->phy_port[pport].port : -1;
}


/*!
 * \brief Get port name.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return Port name.
 */
char *
bcmi_ltsw_port_name(int unit, bcm_port_t port)
{
    char *name = NULL;

    name = PORT(unit, port).port_name;
    if (name == NULL) {
        name = "<unknown>";
    }

    return name;
}


/*!
 * \brief Allocate an ID from L2_IF pool.
 *
 * \param [in] unit Unit number.
 * \param [out] l2_if Allocated ID.
 *
 * \retval SHR_E_EMPTY No available ID to allocacte.
 */
int
bcmi_ltsw_l2_if_alloc(int unit, int *l2_if)
{
    return mbcm_ltsw_l2_if_alloc(unit, l2_if);
}


/*!
 * \brief Free an ID to L2_IF pool.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if ID to free.
 *
 * \retval SHR_E_FULL The ID is already free.
 */
int
bcmi_ltsw_l2_if_free(int unit, int l2_if)
{
    return mbcm_ltsw_l2_if_free(unit, l2_if);
}


/*!
 * \brief Dump L2_IF pool.
 *
 * \param [in] unit Unit Number.
 *
 * \return no return.
 */
void
bcmi_ltsw_l2_if_dump_sw(int unit)
{
    mbcm_ltsw_l2_if_dump(unit);
}


/*!
 * \brief Add specified entry to L2_IF based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_l2_if_add(int unit, int l2_if)
{
    return mbcm_ltsw_l2_if_add(unit, l2_if);
}


/*!
 * \brief Delete specified entry of L2_IF based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_l2_if_del(int unit, int l2_if)
{
    return mbcm_ltsw_l2_if_del(unit, l2_if);
}


/*!
 * \brief Get gport from L2 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if L2 interface.
 * \param [out] port Gport.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_l2_if_to_port(int unit, int l2_if, int *port)
{
    return mbcm_ltsw_l2_if_to_port(unit, l2_if, port);
}


/*!
 * \brief Get L2 interface from port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [out] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_port_to_l2_if(int unit, int port, int *l2_if)
{
    return mbcm_ltsw_port_to_l2_if(unit, port, l2_if);
}


/*!
 * \brief Get port mask from L2 interface mask.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if_mask L2 interface mask.
 * \param [out] port_mask Local logical port mask.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_l2_if_mask_to_port_mask(int unit, int l2_if_mask, int *port_mask)
{
    return mbcm_ltsw_l2_if_mask_to_port_mask(unit, l2_if_mask, port_mask);
}


/*!
 * \brief Get L2 interface mask from port mask.
 *
 * \param [in] unit Unit number.
 * \param [in] port_mask Local logical port mask.
 * \param [out] l2_if_mask L2 interface mask.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_port_mask_to_l2_if_mask(int unit, int port_mask, int *l2_if_mask)
{
    return mbcm_ltsw_port_mask_to_l2_if_mask(unit, port_mask, l2_if_mask);
}


/*!
 * \brief Get which pipes are using identified  L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 * \param [out] pipe_bmp Pipe bitmap.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_l2_if_to_pipes(int unit, int l2_if, uint32_t *pipe_bmp)
{
    return mbcm_ltsw_l2_if_to_pipes(unit, l2_if, pipe_bmp);
}


/*!
 * \brief Add specified entry to System Port based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] flags Operation flags BCMI_LTSW_PORT_XXX.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_sys_port_add(int unit, int sys_port, int flags)
{
    return mbcm_ltsw_sys_port_add(unit, sys_port, flags);
}


/*!
 * \brief Delete specified entry of System Port based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] flags Operation flags BCMI_LTSW_PORT_XXX.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_sys_port_del(int unit, int sys_port, int flags)
{
    return mbcm_ltsw_sys_port_del(unit, sys_port, flags);
}


/*!
 * \brief Change the system port of a port.
 * The settings of the old system port is moved to the new system port.
 * The old system port is deleted then.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] sys_port New system port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_sys_port_move(int unit, bcm_port_t port, int sys_port)
{
    return mbcm_ltsw_port_sys_port_move(unit, port, sys_port);
}


/*!
 * \brief Get gport from system port.
 *
 * \param [in] unit Unit number.
 * \param [in] sys_port System port.
 * \param [out] port Gport.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_sys_port_to_port(int unit, int sys_port, bcm_port_t *port)
{
    return mbcm_ltsw_sys_port_to_port(unit, sys_port, port);
}


/*!
 * \brief Get system port from port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [out] sys_port System port.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_port_to_sys_port(int unit, bcm_port_t port, int *sys_port)
{
    return mbcm_ltsw_port_to_sys_port(unit, port, sys_port);
}


/*!
 * \brief Get system ports from port or gport.
 *
 * If gport is a trunk, get system ports from its member ports.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [in] array_size Size of system port array.
 * \param [out] sys_port_array System port array.
 * \param [out] count Real count of system ports.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmi_ltsw_port_to_sys_ports(int unit, bcm_port_t port,
                            int array_size, int *sys_port_array, int *count)
{
    return mbcm_ltsw_port_to_sys_ports
                (unit, port, array_size, sys_port_array, count);
}


/*!
 * \brief Map System Port with L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_sys_port_map_l2_if(int unit, int sys_port, int l2_if)
{
    return mbcm_ltsw_sys_port_map_l2_if(unit, sys_port, l2_if);
}


/*!
 * \brief Demap System Port and L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_sys_port_demap_l2_if(int unit, int sys_port, int l2_if)
{
    return mbcm_ltsw_sys_port_demap_l2_if(unit, sys_port, l2_if);
}


/*!
 * \brief Traverse over all local ports and gports.
 *
 * \param [in] unit Unit number.
 * \param [in] tbmp Bmp of gport types to traverse (BCM_GPORT_TYPE_NONE_XXX).
 * \param [in] cb Callback function.
 * \param [in] user_data User data to a call back.
 *
 * \return Number of matched ports.
 */
int
bcmi_ltsw_sys_port_traverse(int unit, bcmi_ltsw_port_traverse_cb cb,
                                 void *user_data)
{
    return mbcm_ltsw_sys_port_traverse(unit, cb, user_data);
}

/*!
 * \brief Set single PORT/LPORT field.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Value to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_set(int unit, bcm_port_t port,
                       bcmi_ltsw_port_tab_t type, int data)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.data.n = data;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_set(unit, port, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    return rv;
}


/*!
 * \brief Get single PORT/LPORT field.
 *
 * Helper funtion for port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_get(int unit, bcm_port_t port,
                       bcmi_ltsw_port_tab_t type, int *data)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_get(unit, port, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    *data = port_tab_field.data.n;

    return rv;
}


/*!
 * \brief Set multiple PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerators of port tab types.
 * \param [in] data Values to be set.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_multi_set(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t *type, int *data,
                             int cnt)
{
    int i, size;
    ltsw_port_tab_field_t *port_tab_fields = NULL;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    size = sizeof(ltsw_port_tab_field_t) * cnt;
    SHR_ALLOC(port_tab_fields, size, "port_tab_fields");
    sal_memset(port_tab_fields, 0x0, size);
    for (i = 0; i < cnt; i ++) {
        port_tab_fields[i].type = type[i];
        port_tab_fields[i].data.n = data[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_fields, cnt));

exit:
    SHR_FREE(port_tab_fields);
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get multiple PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerators of port tab types.
 * \param [out] data The gotten values of each port tab types.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_multi_get(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t *type, int *data,
                             int cnt)
{
    int i, size;
    ltsw_port_tab_field_t *port_tab_fields = NULL;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    size = sizeof(ltsw_port_tab_field_t) * cnt;
    SHR_ALLOC(port_tab_fields, size, "port_tab_fields");
    sal_memset(port_tab_fields, 0x0, size);
    for (i = 0; i < cnt; i ++) {
        port_tab_fields[i].type = type[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, port_tab_fields, cnt));

    for (i = 0; i < cnt; i ++) {
        data[i] = port_tab_fields[i].data.n;
    }

exit:
    SHR_FREE(port_tab_fields);
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set single PORT/LPORT array field.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Array to be set.
 * \param [in] size Array size.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_array_set(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t type,
                             uint64_t *data, int size)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.data.a.idx = 0;
    port_tab_field.data.a.buf = data;
    port_tab_field.data.a.size = size;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_set(unit, port, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    return rv;
}


/*!
 * \brief Get single PORT/LPORT array field.
 *
 * Helper funtion for port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten array.
 * \param [in] size The number of elements to get.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_array_get(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t type,
                             uint64_t *data, int size)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.data.a.idx = 0;
    port_tab_field.data.a.buf = data;
    port_tab_field.data.a.size = size;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_get(unit, port, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    return rv;
}


/*!
 * \brief Set the identified bit of PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] bit Bit position.
 * \param [in] data Value to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_bit_set(int unit, bcm_port_t port,
                           bcmi_ltsw_port_tab_t type, int bit, int data)
{
    ltsw_port_tab_field_t port_tab_field;

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;

    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, &port_tab_field, 1));

    port_tab_field.data.n = (port_tab_field.data.n & ~(1 << bit))
                          | ((data & 1) << bit);
    
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, &port_tab_field, 1));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the identified bit of PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] bit Bit position.
 * \param [out] data The gotten value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_bit_get(int unit, bcm_port_t port,
                           bcmi_ltsw_port_tab_t type, int bit, int *data)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_get(unit, port, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    *data = (port_tab_field.data.n >> bit) & 1;

    return rv;
}


/*!
 * \brief Set single PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Value to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_set(int unit, int index,
                           bcmi_ltsw_port_tab_t type, int data)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.index = index;
    port_tab_field.op = ID_KNOWN;
    port_tab_field.data.n = data;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_set(unit, index, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    return rv;
}


/*!
 * \brief Get single PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index.
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_get(int unit, int index,
                           bcmi_ltsw_port_tab_t type, int *data)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.index = index;
    port_tab_field.op = ID_KNOWN;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_get(unit, index, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    *data = port_tab_field.data.n;

    return rv;
}

/*!
 * \brief Set multiple PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerators of port tab types.
 * \param [in] data Values to be set.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_multi_set(int unit, int index,
                                 bcmi_ltsw_port_tab_t *type, int *data,
                                 int cnt)
{
    int i, size;
    ltsw_port_tab_field_t *port_tab_fields = NULL;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    size = sizeof(ltsw_port_tab_field_t) * cnt;
    SHR_ALLOC(port_tab_fields, size, "port_tab_fields");
    sal_memset(port_tab_fields, 0x0, size);
    for (i = 0; i < cnt; i ++) {
        port_tab_fields[i].type = type[i];
        port_tab_fields[i].index = index;
        port_tab_fields[i].op = ID_KNOWN;
        port_tab_fields[i].data.n = data[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, index, port_tab_fields, cnt));

exit:
    SHR_FREE(port_tab_fields);
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multiple PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index.
 * \param [in] type Enumerators of port tab types.
 * \param [out] data The gotten values of each port tab types.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_multi_get(int unit, int index,
                                 bcmi_ltsw_port_tab_t *type, int *data,
                                 int cnt)
{
    int i, size;
    ltsw_port_tab_field_t *port_tab_fields = NULL;

    SHR_FUNC_ENTER(unit);

    PORT_LOCK(unit);

    size = sizeof(ltsw_port_tab_field_t) * cnt;
    SHR_ALLOC(port_tab_fields, size, "port_tab_fields");
    sal_memset(port_tab_fields, 0x0, size);
    for (i = 0; i < cnt; i ++) {
        port_tab_fields[i].type = type[i];
        port_tab_fields[i].index = index;
        port_tab_fields[i].op = ID_KNOWN;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, index, port_tab_fields, cnt));

    for (i = 0; i < cnt; i ++) {
        data[i] = port_tab_fields[i].data.n;
    }

exit:
    SHR_FREE(port_tab_fields);
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set single PORT/LPORT array field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Array to be set.
 * \param [in] size Array size.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_array_set(int unit, int index,
                                 bcmi_ltsw_port_tab_t type,
                                 uint64_t *data, int size)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.index = index;
    port_tab_field.op = ID_KNOWN;
    port_tab_field.data.a.idx = 0;
    port_tab_field.data.a.buf = data;
    port_tab_field.data.a.size = size;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_set(unit, index, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    return rv;
}


/*!
 * \brief Get single PORT/LPORT array field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten array.
 * \param [in] size The number of elements to get.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_array_get(int unit, int index,
                                 bcmi_ltsw_port_tab_t type,
                                 uint64_t *data, int size)
{
    ltsw_port_tab_field_t port_tab_field;
    int rv = SHR_E_UNAVAIL;

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));
    port_tab_field.type = type;
    port_tab_field.index = index;
    port_tab_field.op = ID_KNOWN;
    port_tab_field.data.a.idx = 0;
    port_tab_field.data.a.buf = data;
    port_tab_field.data.a.size = size;

    PORT_LOCK(unit);
    rv = bcmint_ltsw_port_tab_multi_get(unit, index, &port_tab_field, 1);
    PORT_UNLOCK(unit);

    return rv;
}


int
bcmi_ltsw_port_profile_port_pkt_ctrl_add(int unit,
                                         bcmi_ltsw_port_profile_port_pkt_ctrl_t *profile,
                                         int *index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_profile_port_pkt_ctrl_add(unit, profile, index));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_port_profile_port_pkt_ctrl_get(int unit, int index,
                                         bcmi_ltsw_port_profile_port_pkt_ctrl_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_profile_port_pkt_ctrl_get(unit, index, profile));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_port_profile_port_pkt_ctrl_delete(int unit, int index)
{
    PORT_INIT(unit);
    return mbcm_ltsw_port_profile_port_pkt_ctrl_delete(unit, index);
}


int
bcmi_ltsw_port_profile_port_pkt_ctrl_recover(int unit, int index)
{
    PORT_INIT(unit);
    return mbcm_ltsw_port_profile_port_pkt_ctrl_recover(unit, index);
}

int
bcmi_ltsw_port_profile_port_pkt_ctrl_update(int unit, int port,
                                            const char *field_name,
                                            int field_value)
{
    PORT_INIT(unit);
    return mbcm_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                       field_name,
                                                       field_value);
}

int
bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(int unit, int port,
                                            const char *field_name,
                                            int *field_value)
{
    PORT_INIT(unit);
    return mbcm_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                          field_name,
                                                          field_value);
}

int
bcmi_ltsw_port_ing_vlan_action_set(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int profile_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ing_vlan_action_set(unit, port, action, profile_index));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_port_ing_vlan_action_get(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int *profile_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ing_vlan_action_get(unit, port, action, profile_index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_port_egr_vlan_action_set(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int profile_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_vlan_action_set(unit, port, action, profile_index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_port_egr_vlan_action_get(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int *profile_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_vlan_action_get(unit, port, action, profile_index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_port_ing_vlan_action_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ing_vlan_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_port_egr_vlan_xlate_enable_set(int unit, bcm_port_t port, int enable)

{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_vlan_xlate_enable_set(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_port_egr_vlan_xlate_enable_get(int unit, bcm_port_t port, int *enable)

{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_vlan_xlate_enable_get(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Configure ASF mode on the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in]  mode ASF enable or ASF mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_asf_mode_set(int unit, bcm_port_t port, int mode)
{
    pc_mac_control_data_t mac_control_data;
    int rxp = -1;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    if (!(PORT(unit, port).flags & CT_VALID)) {
        if (mode == bcmPortAsfModeStoreAndForward) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }

    /* Disable pause rx if CT enable */
    if (mode && (ltsw_port_info[unit]->port_ha[port].flags & HA_PAUSE_RX_EN)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_get(unit, port, &mac_control_data));
        rxp = mac_control_data.pause_rx;

        mac_control_data.pause_rx = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_PAUSE_RX));
    }

    SHR_IF_ERR_CONT
        (bcmi_ltsw_port_ct_mode_set(unit, port, mode));
    /* roll back if fail to set CT */
    if (SHR_FUNC_ERR() && (rxp >= 0)) {
        mac_control_data.pause_rx = rxp;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_PAUSE_RX));
        SHR_EXIT();
    }

    /* Restore pause rx if CT disable */
    if ((!mode) && (ltsw_port_info[unit]->port_ha[port].flags & HA_PAUSE_RX_EN)) {
        mac_control_data.pause_rx = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_mac_control_set(unit, port, &mac_control_data, PC_PAUSE_RX));
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*
 * \brief Returns the ASF mode configured on the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in]  mode ASF enable or ASF mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_asf_mode_get(int unit, bcm_port_t port, int *mode)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (!(PORT(unit, port).flags & CT_VALID)) {
        *mode = bcmPortAsfModeStoreAndForward;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_ct_mode_get(unit, port, mode));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set HG3 Ethernet Type.
 *
 * \param [in] unit Unit Number.
 * \param [in] type HG3 Ethernet Type.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_hg3_eth_type_set(int unit, int type)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_hg3_eth_type_set(unit, type));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get HG3 Ethernet Type.
 *
 * \param [in] unit Unit Number.
 * \param [in] type HG3 Ethernet Type.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_hg3_eth_type_get(int unit, int *type)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(type, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_hg3_eth_type_get(unit, type));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get which pipes are using identified port group.
 *
 * \param [in] unit Unit Number.
 * \param [in] port_group Port group.
 * \param [in] flags Operation flags BCMI_LTSW_PORT_XXX.
 * \param [out] pipe_bmp Pipe bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_group_to_pipes(int unit, int port_group, int flags,
                              uint32_t *pipe_bmp)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(pipe_bmp, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_group_to_pipes(unit, port_group, flags, pipe_bmp));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the spec of port module resources.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Resource type.
 * \param [out] spec Spec of identified resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_resource_spec_get(int unit, bcmi_ltsw_port_resource_type_t type,
                                 const bcmi_ltsw_port_resource_spec_t **spec)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(spec, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_resource_spec_get(unit, type, spec));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Enable egress VLAN membership check for IFP redirection.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_ifp_egr_vlan_check_enable_set(int unit, uint32_t enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ifp_egr_vlan_check_enable_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get egress VLAN membership check enable for IFP redirection.
 *
 * \param [in] unit Unit Number.
 * \param [out] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_ifp_egr_vlan_check_enable_get(int unit, uint32_t *enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ifp_egr_vlan_check_enable_get(unit, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get flags to disable VLAN membership check.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] flags Flags to disable VLAN membership check.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_vlan_check_disable_get(int unit, bcm_port_t port, uint16_t *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_check_disable_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get flags to disable STG check.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] flags Flags to disable STG check.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_stg_check_disable_get(int unit, bcm_port_t port, uint16_t *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_stg_check_disable_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get port number form dport number.
 *
 * \param [in] unit Unit Number.
 * \param [in] dport Dport number.
 *
 * \retval Port number.
 */
int
bcmi_ltsw_dport_to_port(int unit, int dport)
{
    if (!ltsw_port_info[unit]->dport.enable) {
        if (PORT_IS_VALID(unit, dport)) {
            return dport;
        }
        return -1;
    }

    if ((dport >= 0) && (dport < ltsw_port_info[unit]->dport.dport_num)) {
        return ltsw_port_info[unit]->dport.d2l[dport];
    }

    return -1;
}


/*!
 * \brief Get dport number form port number.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 *
 * \retval Dport number.
 */
int
bcmi_ltsw_port_to_dport(int unit, int port)
{
    if (!ltsw_port_info[unit]->dport.enable) {
        if (PORT_IS_VALID(unit, port)) {
            return port;
        }
        return -1;
    }

    if (PORT_VALID_RANGE(unit, port)) {
        return ltsw_port_info[unit]->dport.l2d[port];
    }

    return -1;
}


/*!
 * \brief Map port number to dport number.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] dport Dport number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_dport_map(int unit, int port, int dport)
{
    int port_old, dport_old;

    SHR_FUNC_ENTER(unit);

    if ((dport < 0) || (dport >= ltsw_port_info[unit]->dport.dport_num)
        || (!PORT_VALID_RANGE(unit, port))) {
        LOG_WARN(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Warning: Invalid mapping port %d <-> dport %d!\n"),
                 port, dport));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    port_old = ltsw_port_info[unit]->dport.d2l[dport];
    dport_old = ltsw_port_info[unit]->dport.l2d[port];
    if (port_old >= 0) {
        ltsw_port_info[unit]->dport.l2d[port_old] = -1;
    }
    if (dport_old >= 0) {
        ltsw_port_info[unit]->dport.d2l[dport_old] = -1;
    }
    ltsw_port_info[unit]->dport.d2l[dport] = port;
    ltsw_port_info[unit]->dport.l2d[port] = dport;

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Dump per-port asf configurations to diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_port_asf_diag(int unit, bcm_port_t port)
{
    pc_port_status_data_t port_status_data;
    pc_port_data_t port_data;
    int speed;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    if (port_data.enable) {
        speed = port_status_data.speed;
    } else {
        speed = port_data.speed;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_asf_config_dump(unit, port, speed));

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Print per-port traffic forwarding mode in diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_port_asf_show(int unit, bcm_port_t port)
{
    pc_port_status_data_t port_status_data;
    pc_port_data_t port_data;
    int speed;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &port_status_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    if (port_data.enable) {
        speed = port_status_data.speed;
    } else {
        speed = port_data.speed;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_asf_config_show(unit, port, speed));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-initialize the PORT interface layer for the specified SOC device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (bcmi_warmboot_get(unit) == 0) {
        /* De-initialize profiles */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_profile_deinit(unit));

        /* De-initialize chip specified features */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_misc_deinit(unit));

        /* De-initialize pipeline */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_service_deinit(unit));
    }

    /* De-initialize software information */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_software_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump sw infomation of port module.
 *
 * \param [in] unit Unit number.
 *
 * \return no return.
 */
void
bcmi_ltsw_port_dump_sw(int unit)
{
    ltsw_port_info_t *pInfo;
    ltsw_phy_port_info_t *ppInfo;
    bcmint_port_ha_info_t *ha;
    int port, pport, pport_max, dport, port_tmp;
    ltsw_pc_profile_info_t *pc_prof;

    LOG_CLI((BSL_META_U(unit, "\nSW Information PORT - Unit %d\n\n"),
             unit));
    LOG_CLI((BSL_META_U(unit, "  Max number of ports [%3d]\n"),
             PORT_NUM_MAX(unit)));
    LOG_CLI((BSL_META_U(unit, "  Max number of mods  [%3d]\n"),
             NUM_MODID(unit)));

    LOG_CLI((BSL_META_U(unit, "\n  ============== Profile info =============\n\n")));
    LOG_CLI((BSL_META_U(unit, "                   hdl  min  max  size  set\n")));
    pc_prof = PROFILE_PC_AUTONEG(unit);
    if (pc_prof->phd) {
        LOG_CLI((BSL_META_U(unit, "  pc_autoneg       %2d    %d %5d %5d %3d\n"),
                 pc_prof->phd, pc_prof->entry_idx_min, pc_prof->entry_idx_max,
                 pc_prof->entry_size, pc_prof->entries_per_set));
    }

    LOG_CLI((BSL_META_U(unit, "\n  ================= Port info ==================\n\n")));
    LOG_CLI((BSL_META_U(unit, "              port_type     flags   pport\n")));
    PORT_ALL_ITER(unit, port) {
        pInfo = &PORT(unit, port);
        LOG_CLI((BSL_META_U(unit, "  %-5s %3d  0x%08X  0x%08X  %3d\n"),
                 pInfo->port_name, port,
                 pInfo->port_type, pInfo->flags, pInfo->pport));
    }

    LOG_CLI((BSL_META_U(unit, "\n  ================= Port HA info ==================\n\n")));
    LOG_CLI((BSL_META_U(unit, "                flags \n")));
    PORT_ALL_ITER(unit, port) {
        ha = &ltsw_port_info[unit]->port_ha[port];
        pInfo = &PORT(unit, port);
        LOG_CLI((BSL_META_U(unit, "  %-5s %3d  0x%08X\n"),
                 pInfo->port_name, port,
                 ha->flags));
    }

    LOG_CLI((BSL_META_U(unit, "\n  ================= PhyPort info ==================\n\n")));
    LOG_CLI((BSL_META_U(unit, "          flags   port\n")));
    for (pport = 0, ppInfo = &ltsw_port_info[unit]->phy_port[pport],
         pport_max = ltsw_port_info[unit]->phy_port_num_max;
         pport < pport_max; pport ++, ppInfo ++) {
        if (ppInfo->port >= 0) {
            LOG_CLI((BSL_META_U(unit, "  %3d  0x%08X  %3d\n"),
                     pport, ppInfo->flags, ppInfo->port));
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n  ================= Dport info ==================\n\n")));
    LOG_CLI((BSL_META_U(unit, "  Enable  [%d]\n"),
             ltsw_port_info[unit]->dport.enable));
    LOG_CLI((BSL_META_U(unit, "  Indexed [%d]\n"),
             ltsw_port_info[unit]->dport.indexed));
    LOG_CLI((BSL_META_U(unit, "  Direct  [%d]\n"),
             ltsw_port_info[unit]->dport.direct));
    LOG_CLI((BSL_META_U(unit, "  Number  [%d]\n\n"),
             ltsw_port_info[unit]->dport.dport_num));
    LOG_CLI((BSL_META_U(unit, "         port    dport\n")));
    PORT_ALL_ITER(unit, port) {
        dport = ltsw_port_info[unit]->dport.l2d[port];
        port_tmp = ltsw_port_info[unit]->dport.d2l[dport];
        pInfo = &PORT(unit, port);
        if (port == port_tmp) {
            LOG_CLI((BSL_META_U(unit, "  %-5s  %3d <--> %-3d\n"),
                     pInfo->port_name, port, dport));
        } else {
            LOG_CLI((BSL_META_U(unit, "  %-5s  %3d  --> %-3d\n"),
                     pInfo->port_name, port, dport));
            LOG_CLI((BSL_META_U(unit, "         %3d <--  %-3d\n"),
                     port_tmp, dport));
        }
    }

    mbcm_ltsw_port_dump_sw(unit);
}


/*!
 * \brief Initialize the PORT interface layer for the specified SOC device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_init(int unit)
{
    int         rv;

    SHR_FUNC_ENTER(unit);

    if ((unit < 0) ||(unit >= BCM_MAX_NUM_UNITS)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Deinit if already initialized */
    if (ltsw_port_info[unit]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_deinit(unit));
    }

    if ((rv = ltsw_port_software_init(unit)) != SHR_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Error unit %d:  Failed software port init: %s\n"),
                   unit, bcm_errmsg(rv)));
        SHR_ERR_EXIT(rv);
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "bcm_port_init: unit %d\n"), unit));

    if (bcmi_ltsw_property_get(unit, BCMI_CPN_SKIP_PORT_HW_INIT, 0)
        && SAL_BOOT_SIMULATION) {
        SHR_EXIT();
    }

    /* Initialize profiles */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_profile_init(unit));

    if (!bcmi_warmboot_get(unit)) {
        /* Initialize pipeline */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_service_init(unit));
    }

    if (!bcmi_warmboot_get(unit)) {
        /* Initialize MAC and PHY */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_port_pm_init(unit));
    }

    /* Initialize chip specified features */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_misc_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_name_update(unit));

    if (ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Probe the port to determine the proper MAC and PHY drivers.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Bitmap of ports to probe.
 * \param [out] okay_pbmp Ports which were successfully probed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_probe(int unit, bcm_pbmp_t pbmp, bcm_pbmp_t *okay_pbmp)
{
    pc_port_data_t port_data;
    pc_port_phys_map_data_t port_map_data;
    ltsw_flexport_port_cfg_t* ltsw_port_resource = NULL;
    uint32_t set_field_bmp;
    int port, pport, pm_id, pm_type, lport;
    int pc_pm_max_size, alloc_size;
    int i, j, nport;
    int tvco, ovco, pll_num;
    int* original_tvco = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(okay_pbmp, SHR_E_PARAM);
    BCM_PBMP_CLEAR(*okay_pbmp);

    alloc_size = sizeof(ltsw_flexport_port_cfg_t) * MAX_PORT_PER_PM;
    SHR_FREE(ltsw_port_resource);
    SHR_ALLOC(ltsw_port_resource, alloc_size, "ltsw_flexport_resource_t");
    SHR_NULL_CHECK(ltsw_port_resource, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_max_pm_size_get(unit, &pc_pm_max_size));

    SHR_FREE(original_tvco);
    SHR_ALLOC(original_tvco, pc_pm_max_size * sizeof(int), "int");
    SHR_NULL_CHECK(original_tvco, SHR_E_MEMORY);
    for(i = 0; i < pc_pm_max_size; i++) {
        original_tvco[i] = ltswPortVCOInvalid;
    }

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (port_fp_validate(unit, port));

        if (IS_MGMT_PORT(unit, port)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Port: %d is management port, which is NOT supported port probe!\n"), port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    /*Make sure the port detach*/
    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
        if (original_tvco[pm_id] == ltswPortVCOInvalid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_vco_get(unit, pm_id, &pll_num, &tvco, &ovco));
            original_tvco[pm_id] = tvco;
        }

        rv = bcmint_ltsw_port_enable_set(unit, port, 0);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }

        rv = bcmi_ltsw_link_port_control_detach(unit, port);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }

        rv = ltsw_pc_port_delete(unit, port);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
        rv = ltsw_pc_port_phys_map_delete(unit, port);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    for (i = 0; i < pc_pm_max_size; i++) {
        nport = 0;
        pm_type = ltswPmTypeNone;
        sal_memset(ltsw_port_resource, 0, alloc_size);

        BCM_PBMP_ITER(pbmp, port) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));

            if (pm_id != i) {
                continue;
            } else {
                ltsw_port_resource[nport].pmid = pm_id;
                ltsw_port_resource[nport].lport = port;
                ltsw_port_resource[nport].pport = PORT(unit, port).pport;
                ltsw_port_resource[nport].speed = PC_PORT_INIT_DATA(unit, port)->speed;
                ltsw_port_resource[nport].lanes = PC_PORT_INIT_DATA(unit, port)->num_lanes;
                ltsw_port_resource[nport].fec = PC_PORT_INIT_DATA(unit, port)->fec_mode;
                nport++;
            }

            if (nport > MAX_PORT_PER_PM) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }

        if (nport == 0) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_type_get(unit, i, &pm_type));

        if ((pm_type == ltswPmTypePm8x50)
            || (pm_type == ltswPmTypePm8x50Gen2)
            || (pm_type == ltswPmTypePm8x50Gen3)
            || (pm_type == ltswPmTypePm8x100)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_pm_port_init_adjust(unit, nport, ltsw_port_resource, original_tvco[i]));
        }

        for (j = 0; j < nport; j++) {
            lport = ltsw_port_resource[j].lport;
            pport = ltsw_port_resource[j].pport;

            port_map_data.pport = pport;
            set_field_bmp = PC_PORT_FIELD_RESTORE;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_init_data_restore(unit, lport, &port_data));

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_phys_map_insert
                    (unit, lport, &port_map_data, PC_PORT_PHYS_MAP_PPORT));

            /* Port should be disable after port probe */
            port_data.enable = 0;

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_pc_port_insert(unit, lport, &port_data, set_field_bmp));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pc_profile_id_init_per_port(unit, lport));

            SHR_IF_ERR_EXIT
                (bcmi_ltsw_link_port_control_attach(unit, lport));

            if (IS_ETH_PORT(unit, lport)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_port_pause_set(unit, lport, 1, 1));
            }

            BCM_PBMP_PORT_ADD(*okay_pbmp, lport);
        }
    }

exit:
    SHR_FREE(ltsw_port_resource);
    SHR_FREE(original_tvco);
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_detach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    /* Disable STP */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_stp_set(unit, port, BCM_STG_STP_DISABLE));

    /* Delete PC Profile per port */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_profile_clear_per_port(unit, port));

    /* Disable port */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_enable_set(unit, port, 0));

    /* Detach port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_delete(unit, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_phys_map_delete(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Bitmap of ports to detach.
 * \param [out] okay_pbmp Ports which were successfully detached.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_detach(int unit, bcm_pbmp_t pbmp, bcm_pbmp_t *okay_pbmp)
{
    int port;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(okay_pbmp, SHR_E_PARAM);
    BCM_PBMP_CLEAR(*okay_pbmp);

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (port_fp_validate(unit, port));

        if (IS_MGMT_PORT(unit, port)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Port: %d is management port, which is NOT supported port detach!\n"), port));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_detach(unit, port));

        BCM_PBMP_PORT_ADD(*okay_pbmp, port);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Physically enable/disable the MAC/PHY on this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable TRUE for port is enabled, FALSE for port is disabled.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int lpk_mode;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_enable_set(unit, port, enable));

    if (bcm_ltsw_linkscan_enable_port_get(unit, port) == SHR_E_DISABLED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_loopback_get(unit, port, &lpk_mode));

        if (!enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_link_port_update(unit, port, BCM_PORT_LINK_STATUS_DOWN));
        } else {
            if (lpk_mode == BCM_PORT_LOOPBACK_MAC
                || lpk_mode == BCM_PORT_LOOPBACK_PHY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_link_port_update(unit, port, BCM_PORT_LINK_STATUS_UP));
            }
        }
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the enable state as defined by bcm_port_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] enable TRUE for port is enabled, FALSE for port is disabled.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_enable_get(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return current Link up/down status.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] status FALSE for link down and TRUE for link up.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_link_status_get(int unit, bcm_port_t port, int *status)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(status, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_link_status_get(unit, port, status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port characteristics from PHY and program MAC to match.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link TRUE for process as link up, FALSE for process as link down.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_update(int unit, bcm_port_t port, int link)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_link_port_update(unit, port, link));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the current loopback mode of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] loopback One of:
 *              BCM_PORT_LOOPBACK_NONE
 *              BCM_PORT_LOOPBACK_MAC
 *              BCM_PORT_LOOPBACK_PHY
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_loopback_get(int unit, bcm_port_t port, int *loopback)
{

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(loopback, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_loopback_get(unit, port, loopback));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the loopback mode of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] loopback One of:
 *             BCM_PORT_LOOPBACK_NONE
 *             BCM_PORT_LOOPBACK_MAC
 *             BCM_PORT_LOOPBACK_PHY
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    int port_enable;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_loopback_set(unit, port, loopback));

    if (bcm_ltsw_linkscan_enable_port_get(unit, port) == SHR_E_DISABLED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_enable_get(unit, port, &port_enable));

        if (port_enable) {
            if (loopback == BCM_PORT_LOOPBACK_MAC
                || loopback == BCM_PORT_LOOPBACK_PHY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_link_port_update(unit, port, BCM_PORT_LINK_STATUS_UP));
            } else if (loopback == BCM_PORT_LOOPBACK_NONE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_link_port_update(unit, port, BCM_PORT_LINK_STATUS_DOWN));
            }
        }
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the autonegotiation state of the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] autoneg Boolean value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_autoneg_get(int unit, bcm_port_t port, int *autoneg)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(autoneg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_autoneg_get(unit, port, autoneg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the autonegotiation state for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] autoneg Boolean value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_autoneg_set(int unit, bcm_port_t port, int autoneg)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_autoneg_set(unit, port, autoneg));
exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the local port advertisement for autonegotiation.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] ability_mask Local advertisement.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ability_advert_set(int unit, bcm_port_t port,
                                bcm_port_ability_t *ability_mask)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Retrieve the local port advertisement for autonegotiation.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] ability_mask Local advertisement.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ability_advert_get(int unit, bcm_port_t port,
                                bcm_port_ability_t *ability_mask)
{
    return SHR_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_autoneg_ability_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      num_ability - number of ability the port will advertise
 *      ability - Local advertisement for each ability.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call WILL NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */
int
bcm_ltsw_port_autoneg_ability_advert_set(int unit,
                                            bcm_port_t port,
                                            int num_ability,
                                            bcm_port_speed_ability_t *abilities)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(abilities, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_autoneg_ability_advert_set(unit, port, num_ability, abilities));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the local port abilities.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] the max num of autoneg ability this port can advertise.
 * \param [out] values indicating the ability this port advertise.
 * \param [out] the actual num of ability that this port advertise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_autoneg_ability_advert_get(int unit,
                                     bcm_port_t port,
                                     int max_num_ability,
                                     bcm_port_speed_ability_t *abilities,
                                     int *actual_num_ability)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(abilities, SHR_E_PARAM);
    SHR_NULL_CHECK(actual_num_ability, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

   SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, max_num_ability,
                                                abilities, actual_num_ability));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the local port advertisement for autonegotiation.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] ability_mask Local advertisement.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_advert_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask)
{
    /*Will not be supported in HSDK*/
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Set the local port advertisement for autonegotiation.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] ability_mask Local advertisement.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_advert_set(int unit, bcm_port_t port, bcm_port_abil_t ability_mask)
{
    /*will not be supported in HSDK*/
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Retrieve the local port abilities.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] the max num of speed ability this port support.
 * \param [out] values indicating the ability of the MAC/PHY.
 * \param [out] the actual num of ability that this port can support.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_speed_ability_local_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_port_speed_ability_t *abilities,
                                     int *actual_num_ability)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(abilities, SHR_E_PARAM);
    SHR_NULL_CHECK(actual_num_ability, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_speed_ability_local_get(unit, port, max_num_ability,
                                                abilities, actual_num_ability));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_port_autoneg_ability_remote_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      max_num_ability  -- the max num of speed ability remote partner can advertise
 *      ability - (OUT) values indicating the ability of the MAC/PHY
 *      actual_num_ability - (OUT) the actual num of ability that remote partner advertise
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_port_autoneg_ability_remote_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_port_speed_ability_t *abilities,
                                     int *actual_num_ability)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Retrieve the local port abilities.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] Mask of BCM_PORT_ABIL_ values indicating the
 *              ability of the MAC/PHY.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ability_local_get(int unit, bcm_port_t port,
                               bcm_port_ability_t *ability_mask)
{
    return SHR_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_ability_remote_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      ability_mask - (OUT) Remote advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_ltsw_port_ability_remote_get(int unit, bcm_port_t port,
                           bcm_port_ability_t *ability_mask)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_port_ability_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask)
{
    return SHR_E_UNAVAIL;
}

int
bcm_ltsw_port_advert_remote_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Get the medium type of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out]medium type*.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_medium_get(int unit, bcm_port_t port, bcm_port_medium_t *medium)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Get the interface type of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] intf BCM_PORT_IF_*.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_interface_get(int unit, bcm_port_t port, bcm_port_if_t *intf)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Set the interface type of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] intf BCM_PORT_IF_*.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_interface_set(int unit, bcm_port_t port, bcm_port_if_t intf)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Get the speed of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] speed Value in megabits/sec.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_speed_get(int unit, bcm_port_t port, int *speed)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Get the maximum speed of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] speed Value in megabits/sec.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_speed_max(int unit, bcm_port_t port, int *speed)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(speed, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_speed_max(unit, port, speed));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the speed of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] speed Value in megabits/sec.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_speed_set(int unit, bcm_port_t port, int speed)
{
    return SHR_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_ltsw_port_resource_speed_config_validate
 * Purpose:
 *      Validate the resource configuration
 * Parameters:
 *      unit      - (IN) Unit number.
 *      resource  - (IN) Port resource information.
 *      pbmp      - (OUT) Returns the affected sister ports
 * Returns:
 *      BCM_E_NONE - Config is honred without affecting sister ports
 *      BCM_E_PARAM - Config will affect sister ports in pbmp
 */
int
bcm_ltsw_port_resource_speed_config_validate(int unit,
                                bcm_port_resource_t *resource, bcm_pbmp_t *pbmp)
{
    bcm_port_t port;

    SHR_FUNC_ENTER(unit);
    PORT_INIT(unit);

    port = resource->port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_speed_config_validate(unit, resource, pbmp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the speed port resource configuration for the specified logical
 * port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] resource Port resource information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_speed_get(int unit,
                                 bcm_gport_t port, bcm_port_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    if ((!is_fp_port(unit, port)) && (!IS_CPU_PORT(unit, port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmint_ltsw_port_resource_get(unit, port, resource),
         SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Modify the following port resources: speed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] resource Port resource information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_speed_set(int unit,
                                 bcm_gport_t port, bcm_port_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    resource->port = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_resource_speed_multi_set(unit, 1, resource));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Modify the following port resources: speed.
 *
 * \param [in] unit Unit number.
 * \param [in] nport Number of elements in array resource.
 * \param [in] resource Port resource information array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_speed_multi_set(int unit,
                                       int nport, bcm_port_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_resource_speed_multi_set(unit, nport, resource));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the port resource configuration for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] resource Port resource configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_get(int unit,
                           bcm_gport_t port, bcm_port_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_resource_get(unit, port, resource));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Modify the following port resources:
 *          - Logical to physical port mapping
 *          - Speed
 *          - Number of PHY lanes
 *          - Encapsulation mode
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] resource Port resource information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_set(int unit,
                           bcm_gport_t port, bcm_port_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_resource_multi_set(unit, 1, resource));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Modify the following port resources:
 *          - Logical to physical port mapping
 *          - Speed
 *          - Number of PHY lanes
 *          - Encapsulation mode
 *
 * \param [in] unit Unit number.
 * \param [in] nport Number of elements in array resource.
 * \param [in] resource Port resource information array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_multi_set(int unit,
                                 int nport, bcm_port_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_resource_multi_set(unit, nport, resource));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Iterates over the port resource configurations on a given
 *        unit and calls user-provided callback for every entry.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Callback function to execute.
 * \param [in] user_data Pointer to any user data.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_resource_traverse(int unit,
                                bcm_port_resource_traverse_cb trav_fn,
                                void *user_data)
{
    int port, rv;
    bcm_port_resource_t resource;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    PORT_FP_ITER(unit, port) {
        rv = bcmint_ltsw_port_resource_get(unit, port, &resource);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        rv = trav_fn(unit, &resource, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the port duplex settings.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] duplex Duplex settings.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_duplex_get(int unit, bcm_port_t port, int *duplex)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(duplex, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_duplex_get(unit, port, duplex));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the port duplex settings.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] duplex Duplex settings.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_duplex_set(int unit, bcm_port_t port, int duplex)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_duplex_set(unit, port, duplex));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get link fault type.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Flags to indicate fault type.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_fault_get(int unit, bcm_port_t port, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_fault_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum receive frame size for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] size Maximum frame size in bytes.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_frame_max_get(int unit, bcm_port_t port, int *size)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_frame_max_get(unit, port, size));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the maximum receive frame size for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] size Maximum frame size in bytes.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_frame_max_set(int unit, bcm_port_t port, int size)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_frame_max_set(unit, port, size));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_l3_mtu_set(unit, port, size));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pause state for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] pause_tx Boolean value.
 * \param [out] pause_rx Boolean value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_pause_get(int unit, bcm_port_t port, int *pause_tx, int *pause_rx)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(pause_tx, SHR_E_PARAM);
    SHR_NULL_CHECK(pause_rx, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_get(unit, port, pause_tx, pause_rx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the pause state for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pause_tx Boolean value.
 * \param [in] pause_rx Boolean value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_pause_set(int unit, bcm_port_t port, int pause_tx, int pause_rx)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_set(unit, port, pause_tx, pause_rx));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pause state for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] pause bcm_port_pause_e enum value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_pause_sym_get(int unit, bcm_port_t port, int *pause)
{
    int pause_tx, pause_rx;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(pause, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_get(unit, port, &pause_tx, &pause_rx));

    if (pause_tx && pause_rx) {
        *pause = BCM_PORT_PAUSE_SYM;
    } else if (pause_tx) {
        *pause = BCM_PORT_PAUSE_ASYM_TX;
    } else if (pause_rx) {
        *pause = BCM_PORT_PAUSE_ASYM_RX;
    } else {
        *pause = BCM_PORT_PAUSE_NONE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the pause state for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pause bcm_port_pause_e enum value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_pause_sym_set(int unit, bcm_port_t port, int pause)
{
    int pause_tx = 0, pause_rx = 0;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    switch (pause) {
        case BCM_PORT_PAUSE_SYM:
            pause_tx = pause_rx = 1;
            break;
        case BCM_PORT_PAUSE_ASYM_TX:
            pause_tx = 1;
            break;
        case BCM_PORT_PAUSE_ASYM_RX:
            pause_rx = 1;
            break;
        case BCM_PORT_PAUSE_NONE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_set(unit, port, pause_tx, pause_rx));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the source address for transmitted PAUSE frames.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mac MAC address sent with pause frames.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_pause_addr_get(int unit, bcm_port_t port, bcm_mac_t mac)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_addr_get(unit, port, mac));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the source address for transmitted PAUSE frames.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mac MAC address sent with pause frames.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_pause_addr_set(int unit, bcm_port_t port, bcm_mac_t mac)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pause_addr_set(unit, port, mac));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the source address for transmitted PAUSE frames.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] speed Port speed.
 * \param [in] duplex Port duplex.
 * \param [in] ifg Port ifg.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ifg_set(int unit, bcm_port_t port, int speed, bcm_port_duplex_t duplex, int ifg)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_ifg_set(unit, port, speed, duplex, ifg));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Gets the new ifg (Inter-frame gap) value.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] speed Port speed.
 * \param [in] duplex Port duplex.
 * \param [out] ifg Port ifg.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ifg_get(int unit, bcm_port_t port, int speed, bcm_port_duplex_t duplex, int *ifg)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(ifg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_ifg_get(unit, port, speed, duplex, ifg));

exit:
    SHR_FUNC_EXIT();
}

 /*!
 * \brief  Set port rlm config properties.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] rlm_config RLM Properties.
 * \param [in] enable enable or disable rlm.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_rlm_config_set(int unit, bcm_port_t port,
                            bcm_port_rlm_config_t *rlm_config, int enable)
{
    int pm_id, pm_type;
    int port_enabled = 0;
    int link = 0;
    int autoneg = 0;
    bcm_port_phy_fec_t fec_mode;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(rlm_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm8x50 ||
        pm_type == ltswPmTypePm8x50Gen2 ||
        pm_type == ltswPmTypePm8x50Gen3 ||
        pm_type == ltswPmTypePm8x100) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_enable_get(unit, port, &port_enabled));

        if (!enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_enable_set(unit, port, 0));
        } else {
            if (!port_enabled) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d is not enabled \n"), port));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_fec_mode_get(unit, port,&fec_mode));
            if (fec_mode == bcmPortPhyFecRs272 || fec_mode == bcmPortPhyFecRs272_2xN) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d can NOT be set RLM, because RLM is not supported with the RS272 FEC\n"), port));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_link_status_get(unit, port, &link));
            if (!link) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d link is not up \n"), port));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_autoneg_get(unit, port, &autoneg));
            if (autoneg) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d autoneg is on \n"), port));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_rlm_config_set(unit, port, rlm_config, enable));

        if (!enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_enable_set(unit, port, port_enabled));
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port rlm config properties.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] rlm_config RLM config properties.
 * \param [out] enable enable/disable rlm.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_rlm_config_get(int unit, bcm_port_t port,
                            bcm_port_rlm_config_t *rlm_config, int *enable)
{
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(rlm_config, SHR_E_PARAM);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm8x50 ||
        pm_type == ltswPmTypePm8x50Gen2 ||
        pm_type == ltswPmTypePm8x50Gen3 ||
        pm_type == ltswPmTypePm8x100) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_rlm_config_get(unit, port, rlm_config, enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

 /*!
 * \brief Get port rlm status.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] rlm_status RLM status properties.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_rlm_status_get(int unit, bcm_port_t port,
                            bcm_port_rlm_status_t *rlm_status)
{
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(rlm_status, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm8x50 ||
        pm_type == ltswPmTypePm8x50Gen2 ||
        pm_type == ltswPmTypePm8x50Gen3 ||
        pm_type == ltswPmTypePm8x100) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_rlm_status_get(unit, port, rlm_status));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

 /*!
 * \brief Set phy timesync configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] conf Phy timesync configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_phy_timesync_config_set(int unit, bcm_port_t port,
                                     bcm_port_phy_timesync_config_t *conf)
{
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(conf, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm8x50 ||
        pm_type == ltswPmTypePm8x50Gen2 ||
        pm_type == ltswPmTypePm8x50Gen3 ||
        pm_type == ltswPmTypePm8x100) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_phy_timesync_config_set(unit, port, conf));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
* \brief Get phy timesync configuration.
*
* \param [in] unit Unit number.
* \param [in] port Port number.
* \param [out] conf Phy timesync configuration.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_phy_timesync_config_get(int unit, bcm_port_t port,
                                     bcm_port_phy_timesync_config_t *conf)
{
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(conf, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm8x50 ||
        pm_type == ltswPmTypePm8x50Gen2 ||
        pm_type == ltswPmTypePm8x50Gen3 ||
        pm_type == ltswPmTypePm8x100) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_phy_timesync_config_get(unit, port, conf));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Set port control phy timesync configuration.
*
* \param [in] unit Unit number.
* \param [in] port Port number.
* \param [int] type Operation.
* \param [int] value Arg to operation.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_control_phy_timesync_set(int unit, bcm_port_t port,
                                      bcm_port_control_phy_timesync_t type,
                                      uint64 value)
{
    int pm_id, pm_type;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm8x50 ||
        pm_type == ltswPmTypePm8x50Gen2 ||
        pm_type == ltswPmTypePm8x50Gen3 ||
        pm_type == ltswPmTypePm8x100) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_control_phy_timesync_set(unit, port, type, value));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
* \brief Get port control phy timesync configuration.
*
* \param [in] unit Unit number.
* \param [in] port Port number.
* \param [int] type Operation.
* \param [out] value Pointer to arg for operation.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_control_phy_timesync_get(int unit, bcm_port_t port,
                                      bcm_port_control_phy_timesync_t type,
                                      uint64 *value)
{
    return SHR_E_UNAVAIL;
}

/*!
* \brief Retrives two-step PTP timestamp and seqid from MAC/PHY FIFO after packet TX
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [out]  tx_info Two-step timestamp.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_timesync_tx_info_get(int unit, bcm_port_t port,
                                  bcm_port_timesync_tx_info_t *tx_info)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(tx_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_timesync_tx_info_get(unit, port, tx_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Set the configuration for Flight Data Recorder (FDR) on a port
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [in]  fdr_config   Flight Data Recorder (FDR) configuration.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_fdr_config_set(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config)

{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(fdr_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    if (IS_MGMT_PORT(unit, port)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fdr_config->window_size >= bcmPortFdrSymbErrWindowCwCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fdr_config->symb_err_stats_sel != 0
        && fdr_config->symb_err_stats_sel != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_fdr_config_set(unit, port, fdr_config));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
* \brief Get the configuration for Flight Data Recorder (FDR) on a port
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [out]  fdr_config   Flight Data Recorder (FDR) configuration.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_fdr_config_get(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(fdr_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    if (IS_MGMT_PORT(unit, port)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_fdr_config_get(unit, port, fdr_config));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Get the statistics of Flight Data Recorder (FDR) on a port
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [out]  fdr_stats   Flight Data Recorder (FDR) statistics.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_fdr_stats_get(int unit, bcm_port_t port, bcm_port_fdr_stats_t *fdr_stats)
{
    bcm_port_fdr_config_t fdr_config;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(fdr_stats, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    if (IS_MGMT_PORT(unit, port)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_fdr_config_get(unit, port, &fdr_config));

    if (fdr_config.intr_enable) {
        LOG_WARN(BSL_LS_BCM_PORT, (BSL_META_U(unit, "Port: %d is in FDR Interrupt Mode!\n"), port));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_fdr_stats_get(unit, port, fdr_stats));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Register port Flight Data Recorder (FDR) interrupt notification callbacks
*
* \param [in]   unit       Unit number.
* \param [in]   f          Flight Data Recorder (FDR) callback.
* \param [in]   user_data  user data pointer.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_fdr_callback_register(int unit, bcm_port_fdr_handler_t f, void* user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_fdr_callback_register(unit, f, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Unregister port Flight Data Recorder (FDR) interrupt notification callbacks
*
* \param [in]   unit       Unit number.
* \param [in]   f          Flight Data Recorder (FDR) callback.
* \param [in]   user_data  user data pointer.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
int
bcm_ltsw_port_fdr_callback_unregister(int unit, bcm_port_fdr_handler_t f, void* user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_fdr_callback_unregister(unit, f, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    uint32 phy = 0, reg = 0;
    uint8 phy_id, is_internal, phy_bus;
    uint16 mdio_addr;
    uint32 dev_addr, reg_addr;
    int pport;
    uint32 reg_flag;

    SHR_FUNC_ENTER(unit);

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (port_fp_validate(unit, port));
    }

    LOG_INFO(BSL_LS_BCM_PORT,
         (BSL_META_U(unit,
                     "ltsw_port_phy_set: u=%d p=%d flags=0x%08x "
                     "phy_reg=0x%08x phy_data=0x%08x\n"),
                     unit, port, flags, phy_reg_addr, phy_data));

    reg_flag = BCM_PORT_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & LTSW_PORT_PHY_REG_INDIRECT) {
        flags |= BCM_PORT_PHY_INTERNAL;
        phy_reg_addr &= ~LTSW_PORT_PHY_REG_INDIRECT;
    }

    if (flags & BCM_PORT_PHY_INTERNAL) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        pport = bcmi_ltsw_port_to_pport(unit, port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_phy_reg_write(unit, pport, phy_reg_addr, phy_data, NULL));
    } else if (flags & BCM_PORT_PHY_NOMAP) {
        mdio_addr = (uint16) port;

        is_internal = LTSW_PHY_IS_INTERNAL(mdio_addr);
        phy_bus = LTSW_PHY_BUS_ID(mdio_addr);
        phy_id = LTSW_PHY_ID(mdio_addr);

        BCMBD_MIIM_PHY_INT_SET(phy, is_internal);
        BCMBD_MIIM_PHY_BUS_SET(phy, phy_bus);
        BCMBD_MIIM_PHY_ID_SET(phy, phy_id);

        if (flags & BCM_PORT_PHY_CLAUSE45) {
            BCMBD_MIIM_PHY_C45_SET(phy, 1);
            dev_addr = BCM_PORT_PHY_CLAUSE45_DEVAD(phy_reg_addr);
            reg_addr = BCM_PORT_PHY_CLAUSE45_REGAD(phy_reg_addr);
            BCMBD_MIIM_DEVAD_SET(reg, dev_addr);
            BCMBD_MIIM_REGAD_SET(reg, reg_addr);
        } else {
             reg = phy_reg_addr;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_miim_write(unit, phy, reg, phy_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32 phy = 0, reg = 0;
    uint8 phy_id, is_internal, phy_bus;
    uint16 mdio_addr;
    uint32 dev_addr, reg_addr;
    uint32 phy_rd_data;
    int pport;
    uint32 reg_flag;

    SHR_FUNC_ENTER(unit);

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (port_fp_validate(unit, port));
    }

    reg_flag = BCM_PORT_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & LTSW_PORT_PHY_REG_INDIRECT) {
        flags |= BCM_PORT_PHY_INTERNAL;
        phy_reg_addr &= ~LTSW_PORT_PHY_REG_INDIRECT;
    }

    if (flags & BCM_PORT_PHY_INTERNAL) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        pport = bcmi_ltsw_port_to_pport(unit, port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_phy_reg_read(unit, pport, phy_reg_addr, &phy_rd_data, NULL));
        *phy_data = phy_rd_data;
    } else if (flags & BCM_PORT_PHY_NOMAP) {
        mdio_addr = (uint16) port;

        is_internal = LTSW_PHY_IS_INTERNAL(mdio_addr);
        phy_bus = LTSW_PHY_BUS_ID(mdio_addr);
        phy_id = LTSW_PHY_ID(mdio_addr);

        BCMBD_MIIM_PHY_INT_SET(phy, is_internal);
        BCMBD_MIIM_PHY_BUS_SET(phy, phy_bus);
        BCMBD_MIIM_PHY_ID_SET(phy, phy_id);

        if (flags & BCM_PORT_PHY_CLAUSE45) {
            BCMBD_MIIM_PHY_C45_SET(phy, 1);
            dev_addr = BCM_PORT_PHY_CLAUSE45_DEVAD(phy_reg_addr);
            reg_addr = BCM_PORT_PHY_CLAUSE45_REGAD(phy_reg_addr);
            BCMBD_MIIM_DEVAD_SET(reg, dev_addr);
            BCMBD_MIIM_REGAD_SET(reg, reg_addr);
        } else {
            reg = phy_reg_addr;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_miim_read(unit, phy, reg, &phy_rd_data));

        *phy_data = phy_rd_data;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief General PHY register write.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number or PHY MDIO address (refer BCM_PORT_PHY_NOMAP).
 * \param [in] flags Logical OR of one or more of the following flags:
 *                   BCM_PORT_PHY_NOMAP
 *                           Instead of mapping port to PHY MDIO address,
 *                           treat port parameter as actual PHY MDIO address.
 *                   BCM_PORT_PHY_CLAUSE45
 *                           Assume Clause 45 device instead of Clause 22.
 *\param [in] phy_reg_addr PHY internal register address
 *\param [out] phy_data Data to write.
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_phy_set(unit, port, flags, phy_reg_addr, phy_data));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief General PHY register read.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number or PHY MDIO address (refer BCM_PORT_PHY_NOMAP).
 * \param [in] flags Logical OR of one or more of the following flags:
 *                   BCM_PORT_PHY_NOMAP
 *                           Instead of mapping port to PHY MDIO address,
 *                           treat port parameter as actual PHY MDIO address.
 *                   BCM_PORT_PHY_CLAUSE45
 *                           Assume Clause 45 device instead of Clause 22.
 *\param [in] phy_reg_addr PHY internal register address
 *\param [out] phy_data Data that was read.
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(phy_data,SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_phy_get(unit, port, flags, phy_reg_addr, phy_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the local port from the given GPORT ID.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Global port identifier.
 * \param [out] local_port Local port encoded in gport.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_local_get(int unit, bcm_gport_t gport, bcm_port_t *local_port)
{
    bcm_module_t my_mod, encap_mod;
    bcm_port_t encap_port;
    int isLocal = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    if ((NUM_MODID(unit) == 0) && !BCM_GPORT_IS_DEVPORT(gport) ){
        /* Only devport gport can be supported on device with no modid */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (BCM_GPORT_IS_LOCAL(gport)) {
         *local_port = BCM_GPORT_LOCAL_GET(gport);
         if (!PORT_IS_VALID(unit, *local_port)) {
             SHR_ERR_EXIT(SHR_E_PORT);
         }
    } else if (BCM_GPORT_IS_LOCAL_CPU(gport)) {
        *local_port = first_port(unit, BCMI_LTSW_PORT_TYPE_CPU);
    } else if (BCM_GPORT_IS_DEVPORT(gport)) {
        *local_port = BCM_GPORT_DEVPORT_PORT_GET(gport);
        if (unit != BCM_GPORT_DEVPORT_DEVID_GET(gport)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
        if (!PORT_IS_VALID(unit, *local_port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_MODPORT(gport) || BCM_GPORT_IS_PROXY(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &my_mod));

        if (BCM_GPORT_IS_MODPORT(gport)) {
            encap_mod = BCM_GPORT_MODPORT_MODID_GET(gport);
            encap_port = BCM_GPORT_MODPORT_PORT_GET(gport);
        } else {
            encap_mod = BCM_GPORT_PROXY_MODID_GET(gport);
            encap_port = BCM_GPORT_PROXY_PORT_GET(gport);
        }

        if (encap_mod == my_mod){
            *local_port = encap_port;
        } else if (NUM_MODID(unit) > 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_modid_is_local(unit, encap_mod, &isLocal));
            if (isLocal) {
                *local_port = encap_port +
                  (encap_mod - my_mod) * (bcmi_ltsw_dev_max_modport(unit) + 1);
            } else {
                SHR_ERR_EXIT(SHR_E_PORT);
            }
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
        if (!PORT_IS_VALID(unit, *local_port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        return BCM_E_PORT;
    } else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        *local_port = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        *local_port = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        *local_port = BCM_GPORT_NIV_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        *local_port = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_FLOW_PORT(gport)) {
        *local_port = BCM_GPORT_FLOW_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        *local_port = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        *local_port = BCM_GPORT_MIM_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        *local_port = BCM_GPORT_MPLS_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_TRUNK(gport)) {
        rv = bcmi_ltsw_trunk_tid_to_vp_lag_vp(unit, BCM_GPORT_TRUNK_GET(gport),
                                              local_port);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (BCM_GPORT_IS_VP_GROUP(gport)) {
        *local_port = BCM_GPORT_VP_GROUP_GET(gport);
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the GPORT ID for the specified port.
 *
 * Always returns a MODPORT gport or an error.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] gport GPORT ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_gport_get(int unit, bcm_port_t port, bcm_gport_t *gport)
{
    bcmi_ltsw_gport_info_t info;
    int rv;

    SHR_FUNC_ENTER(unit);

    bcmi_ltsw_gport_info_t_init(&info);
    rv= bcm_ltsw_stk_my_modid_get(unit, &info.modid);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    info.port = port;
    info.gport_type = _SHR_GPORT_TYPE_MODPORT;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_construct(unit, &info, gport));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get all the extra GPORT ID for the specified physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] size Size of gport array.
 * \param [out] gport GPORT IDs.
 * \param [out] gport GPORT count.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_gport_get_all(int unit, bcm_port_t local_port, int size,
                            bcm_gport_t *gport_array, int *count)
{
    return SHR_E_UNAVAIL;
}

int
bcmint_ltsw_port_config_get(int unit, bcm_port_config_t *config)
{
    int pipe, num_pipe;
    bcm_pbmp_t pbm_pipe;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &config->e));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_GE, &config->ge));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_XE, &config->xe));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_CE, &config->ce));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_CDE, &config->cd));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_HG, &config->hg));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_HG, &config->stack_ext));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &config->port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_CPU, &config->cpu));
    /* bitmap "all" doesn't include LB and RDB port for backward compatibility */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit,
                              (BCMI_LTSW_PORT_TYPE_PORT |
                               BCMI_LTSW_PORT_TYPE_CPU |
                               BCMI_LTSW_PORT_TYPE_MGMT),
                              &config->all));

    /* per-pipe pbm */
    num_pipe = bcmi_ltsw_dev_max_pipe_num(unit);
    for (pipe = 0; pipe < num_pipe && pipe < BCM_PIPES_MAX; pipe ++) {
        BCM_PBMP_CLEAR(pbm_pipe);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dev_pipe_logic_pbmp(unit, pipe, &pbm_pipe));
        BCM_PBMP_ASSIGN(config->per_pipe[pipe], config->all);
        BCM_PBMP_AND(config->per_pipe[pipe], pbm_pipe);
    }

    /* per-pp-pipe pbm */
    num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    for (pipe = 0; pipe < num_pipe; pipe ++) {
        BCM_PBMP_CLEAR(pbm_pipe);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dev_pp_pipe_logic_pbmp(unit, pipe, &pbm_pipe));
        BCM_PBMP_ASSIGN(config->per_pp_pipe[pipe], config->all);
        BCM_PBMP_AND(config->per_pp_pipe[pipe], pbm_pipe);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port configuration of a device.
 *
 * \param [in] unit Unit number.
 * \param [out] config Structure returning configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_config_get(int unit, bcm_port_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_config_get(unit, config));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set requested port parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] info Port information structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_selective_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int pause_tx, pause_rx;
    bcm_port_encap_config_t encap;
    uint32_t mask;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENCAP_MASK) {
        encap.encap = info->encap_mode;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_encap_config_set(unit, port, &encap));
    }

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_enable_set(unit, port, info->enable));
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_autoneg_set(unit, port, info->autoneg));
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        bcm_port_resource_t pr;
        /* When AN=on, speed will be auto-detected, hence don't set */
        if ((mask & BCM_PORT_ATTR_AUTONEG_MASK) && info->autoneg) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "Autoneg:On. SPEED will be auto-detected\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_resource_get(unit, port, &pr));
        if (pr.speed != info->speed) {
            LOG_INFO(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "Warning: bcm_port_info speed (%d) does NOT match "
                      " current speed (%d). May have fec/link training/"
                      "phy lane config mismatches!\n"),
                      pr.speed, info->speed));
        }
        pr.speed = info->speed;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_resource_speed_set(unit, port, &pr));
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK) {
        /* When AN=on, duplex will be auto-detected, hence don't set*/
        if ((mask & BCM_PORT_ATTR_AUTONEG_MASK) && info->autoneg) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "Autoneg:On. DUPLEX will be auto-detected\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_duplex_set(unit, port, info->duplex));
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_linkscan_set(unit, port, info->linkscan));
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_learn_set(unit, port, info->learn));
    }

    if (mask & BCM_PORT_ATTR_DISCARD_MASK) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm_ltsw_port_discard_set(unit, port, info->discard),
             SHR_E_UNAVAIL);
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm_ltsw_port_vlan_member_set(unit, port, info->vlanfilter),
             SHR_E_UNAVAIL);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_untagged_priority_set
                (unit, port, info->untagged_priority));
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm_ltsw_port_untagged_vlan_set(unit, port, info->untagged_vlan),
             SHR_E_UNAVAIL);
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_stp_set(unit, port, info->stp_state));
    }

    if (mask & BCM_PORT_ATTR_PFM_MASK) {
        
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_loopback_set(unit, port, info->loopback));
    }

    if (mask & (BCM_PORT_ATTR_PAUSE_TX_MASK |
                BCM_PORT_ATTR_PAUSE_RX_MASK)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_pause_get(unit, port, &pause_tx, &pause_rx));
        if (mask & BCM_PORT_ATTR_PAUSE_TX_MASK) {
            pause_tx = info->pause_tx;
        }
        if (mask & BCM_PORT_ATTR_PAUSE_RX_MASK) {
            pause_rx = info->pause_rx;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_pause_set(unit, port, pause_tx, pause_rx));
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_pause_addr_set(unit, port, info->pause_mac));
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_frame_max_set(unit, port, info->frame_max));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get requested port parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [inout] info Port information structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_selective_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    bcm_port_resource_t pr;
    bcm_port_encap_config_t encap;
    uint32_t mask;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_resource_speed_get(unit, port, &pr));

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENCAP_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_encap_config_get(unit, port, &encap));
        info->encap_mode = encap.encap;
    }

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_enable_get(unit, port, &info->enable));
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_link_status_get(unit, port, &info->linkstatus));
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_autoneg_get(unit, port, &info->autoneg));
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        info->speed = pr.speed;
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_duplex_get(unit, port, &info->duplex));
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_linkscan_get(unit, port, &info->linkscan));
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_learn_get(unit, port, &info->learn););
    }

    if (mask & BCM_PORT_ATTR_DISCARD_MASK) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm_ltsw_port_discard_get(unit, port, &info->discard),
             SHR_E_UNAVAIL);
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm_ltsw_port_vlan_member_get(unit, port, &info->vlanfilter),
             SHR_E_UNAVAIL);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_untagged_priority_get
                (unit, port, &info->untagged_priority));
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm_ltsw_port_untagged_vlan_get(unit, port, &info->untagged_vlan),
             SHR_E_UNAVAIL);
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_stp_get(unit, port, &info->stp_state));
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_loopback_get(unit, port, &info->loopback));
    }

    if (mask & (BCM_PORT_ATTR_PAUSE_TX_MASK |
                BCM_PORT_ATTR_PAUSE_RX_MASK)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_pause_get
                (unit, port, &info->pause_tx, &info->pause_rx));
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_pause_addr_get(unit, port, info->pause_mac));
    }

    if (mask & BCM_PORT_ATTR_SPEED_MAX_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_speed_max(unit, port, &info->speed_max));
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_frame_max_get(unit, port, &info->frame_max));
    }

    if (mask & BCM_PORT_ATTR_MEDIUM_MASK) {
        int medium;
        medium = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_GET(pr.phy_lane_config);
        if (medium == BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE) {
            info->medium = BCM_PORT_MEDIUM_BACKPLANE;
        } else if (medium == BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE) {
            info->medium = BCM_PORT_MEDIUM_COPPER;
        } else if (medium == BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS) {
            info->medium = BCM_PORT_MEDIUM_FIBER;
        } else {
            info->medium = BCM_PORT_MEDIUM_NONE;
        }
    }

    if (mask & BCM_PORT_ATTR_FAULT_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_fault_get(unit, port, &info->fault));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set all information on the port.
 *
 * Checks if AN is on, and if so, clears the proper bits in the action mask.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] info Port information structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_info_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    if (info->autoneg) {
        info->action_mask &= ~BCM_PORT_AN_ATTRS;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_selective_set(unit, port, info));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get all information on the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] info Port information structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_info_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    info->action_mask = BCM_PORT_ATTR_ALL_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_selective_get(unit, port, info));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Save the current settings of a port.
 *
 * The action_mask will be adjusted so that the proper values will be set
 * when a restore is made. This mask should not be altered between these calls.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] info Port information structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_info_save(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    info->action_mask2 = BCM_PORT_ATTR2_PORT_ABILITY;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_selective_get(unit, port, info));

    if (info->autoneg) {
        info->action_mask &= ~BCM_PORT_AN_ATTRS;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Restore port settings saved by info_save.
 *
 * Checks if AN is on, and if so, clears the proper bits in the action mask.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] info Port information structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_info_restore(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    PORT_INIT(unit);
    return bcm_ltsw_port_selective_set(unit, port, info);
}

/*!
 * \brief Get the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    PORT_LOCK(unit);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_learn_get(unit, port, flags));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the ARL hardware learning options for this port.
 *
 * This function defines what the hardware will do when a packet
 * is seen with an unknown address.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    PORT_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_learn_set(unit, port, flags));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Modify the port learn flags, adding add and removing remove flags.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] add Flags to set.
 * \param [in] remove Flags to clear.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_learn_modify(int unit, bcm_port_t port, uint32_t add, uint32_t remove)
{
    uint32_t flags;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    PORT_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_learn_get(unit, port, &flags));
    flags |= add;
    flags &= ~remove;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_port_learn_set(unit, port, flags));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the linkscan state for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] linkscan Linkscan value (None, S/W, H/W).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_linkscan_set(int unit, bcm_port_t port, int linkscan)
{
    PORT_INIT(unit);
    return bcm_ltsw_linkscan_mode_set(unit, port, linkscan);
}


/*!
 * \brief Get the linkscan state for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] linkscan Linkscan value (None, S/W, H/W).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_linkscan_get(int unit, bcm_port_t port, int *linkscan)
{
    PORT_INIT(unit);
    return bcm_ltsw_linkscan_mode_get(unit, port, linkscan);
}

/*!
 * \brief Get the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [out] value Current value of the port feature.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_control_get(int unit, bcm_port_t port,
                          bcm_port_control_t type, int *value)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_control_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [in] value Value of the port feature to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_control_set(int unit, bcm_port_t port,
                          bcm_port_control_t type, int value)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_control_set(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Control the sampling of packets ingressing or egressing a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] ingress_rate Ingress direction rate.
 * \param [in] egress_rate Egress direction rate.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_port_sample_rate_set(int unit, bcm_port_t port,
                              int ingress_rate, int egress_rate)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_sample_rate_set(unit, port, ingress_rate, egress_rate));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the sampling rate of packets ingressing or egressing a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] ingress_rate Ingress direction rate.
 * \param [out] egress_rate Egress direction rate.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_port_sample_rate_get(int unit, bcm_port_t port,
                              int *ingress_rate, int *egress_rate)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_sample_rate_get(unit, port, ingress_rate, egress_rate));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return whether L3 switching is enabled on the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] enable TRUE if enabled, FALSE if disabled.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l3_enable_get(int unit, bcm_port_t port, int *enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_l3_enable_get(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Enable/Disable L3 switching on the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable TRUE to enable, FALSE to disable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l3_enable_set(int unit, bcm_port_t port, int enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_l3_enable_set(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the maximum L3 frame size for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] size Maximum frame size in bytes.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l3_mtu_get(int unit, bcm_port_t port, int *size)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_l3_mtu_get(unit, port, size));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the maximum L3 frame size for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] size Maximum frame size in bytes.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l3_mtu_set(int unit, bcm_port_t port, int size)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_l3_mtu_set(unit, port, size));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the L3 multicast options for the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] flags Logical OR of BCM_PORT_IPMC_xxx flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ipmc_modify_get(int unit, bcm_port_t port, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ipmc_modify_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the L3 multicast options for the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] flags Logical OR of BCM_PORT_IPMC_xxx flags.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_ipmc_modify_set(int unit, bcm_port_t port, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_ipmc_modify_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the default Tag Protocol ID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tpid_set(int unit, bcm_port_t port, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_set(unit, port, OUTER_TPID, tpid));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_set
            (unit, port, OUTER_TPID | OBM_OUTER_TPID, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add allowed TPID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 * \param [in] color_select Color mode of the TPID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tpid_add(int unit, bcm_port_t port, uint16_t tpid,
                       int color_select)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((color_select != BCM_COLOR_PRIORITY) &&
        (color_select != BCM_COLOR_OUTER_CFI) &&
        (color_select != BCM_COLOR_INNER_CFI)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_add(unit, port, OUTER_TPID, tpid, color_select));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_add
            (unit, port, OUTER_TPID | OBM_OUTER_TPID, tpid, color_select));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the default Tag Protocol ID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tpid_get(int unit, bcm_port_t port, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_get(unit, port, OUTER_TPID, tpid));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_get(unit, port, OUTER_TPID, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get a list of tpids and associated color selections for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] size Number of elements of the next two array parameters.
 * \param [out] tpid_array Buffer array to hold a list of retrieved tpids.
 * \param [out] color_array Buffer array to hold a list of retrieved color selections.
 * \param [out] count Actual number of tpids retrieved.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tpid_get_all(int unit, bcm_port_t port,
                           int size, uint16_t *tpid_array,
                           int *color_array, int *count)
{
    SHR_FUNC_ENTER(unit);

    if (size < 0 || count == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ((size > 0) && (tpid_array == NULL || color_array == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_get_all
                (unit, port, OUTER_TPID, size, tpid_array, color_array, count));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_get_all
            (unit, port, OUTER_TPID, size, tpid_array, color_array, count));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete allowed TPID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tpid_delete(int unit, bcm_port_t port, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_delete(unit, port, OUTER_TPID, tpid));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_delete
            (unit, port, OUTER_TPID | OBM_OUTER_TPID, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete all allowed TPID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tpid_delete_all(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_delete_all(unit, port, OUTER_TPID));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_delete_all
            (unit, port, OUTER_TPID | OBM_OUTER_TPID));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the default L2 tunnel payload Tag Protocol ID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l2_tunnel_payload_tpid_set(int unit, bcm_port_t port, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_set(unit, port, PAYLOAD_OUTER_TPID, tpid));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_set(unit, port, PAYLOAD_OUTER_TPID, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add allowed L2 tunnel payload TPID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 * \param [in] color_select Color mode of the TPID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l2_tunnel_payload_tpid_add(int unit, bcm_port_t port, uint16_t tpid,
                                         int color_select)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((color_select != BCM_COLOR_PRIORITY) &&
        (color_select != BCM_COLOR_OUTER_CFI) &&
        (color_select != BCM_COLOR_INNER_CFI)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_add
                (unit, port, PAYLOAD_OUTER_TPID, tpid, color_select));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_add
            (unit, port, PAYLOAD_OUTER_TPID, tpid, color_select));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the default L2 tunnel payload Tag Protocol ID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l2_tunnel_payload_tpid_get(int unit, bcm_port_t port, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_get(unit, port, PAYLOAD_OUTER_TPID, tpid));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_get(unit, port, PAYLOAD_OUTER_TPID, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get a list of L2 tunnel payload tpids and associated color selections for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] size Number of elements of the next two array parameters.
 * \param [out] tpid_array Buffer array to hold a list of retrieved tpids.
 * \param [out] color_array Buffer array to hold a list of retrieved color selections.
 * \param [out] count Actual number of tpids retrieved.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l2_tunnel_payload_tpid_get_all(int unit, bcm_port_t port,
                                             int size, uint16_t *tpid_array,
                                             int *color_array, int *count)
{
    SHR_FUNC_ENTER(unit);

    if (size < 0 || count == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ((size > 0) && (tpid_array == NULL || color_array == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_get_all
                (unit, port, PAYLOAD_OUTER_TPID,
                 size, tpid_array, color_array, count));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_get_all
            (unit, port, PAYLOAD_OUTER_TPID,
             size, tpid_array, color_array, count));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete allowed L2 tunnel payload TPID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l2_tunnel_payload_tpid_delete(int unit, bcm_port_t port, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_delete(unit, port, PAYLOAD_OUTER_TPID, tpid));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_delete(unit, port, PAYLOAD_OUTER_TPID, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete all allowed L2 tunnel payload TPID for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_l2_tunnel_payload_tpid_delete_all(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_port_tpid_delete_all(unit, port, PAYLOAD_OUTER_TPID));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tpid_delete_all(unit, port, PAYLOAD_OUTER_TPID));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the default VLAN ID for received untagged packets on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] vid VLAN ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_untagged_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(vid, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_untagged_vlan_get(unit, port, vid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the default VLAN ID for received untagged packets on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] vid VLAN ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (!BCM_VLAN_VALID(vid)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_untagged_vlan_set(unit, port, vid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve the 802.1p priority for received untagged packets on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] priority 802.1p priority.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(priority, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_untagged_priority_get(unit, port, priority));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the 802.1p priority for received untagged packets on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] priority 802.1p priority.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((priority < BCM_PRIO_MIN) || (priority > BCM_PRIO_MAX)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_untagged_priority_set(unit, port, priority));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the expected TPID for the inner tag in double-tagging mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_inner_tpid_set(int unit, bcm_port_t port, uint16 tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_inner_tpid_set(unit, port, tpid));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the expected TPID for the inner tag in double-tagging mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tpid Tag Protocol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_inner_tpid_get(int unit, bcm_port_t port, uint16 *tpid)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(tpid, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_inner_tpid_get(unit, port, tpid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the trunk group for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] tid trunk ID.
 * \param [out] psc trunk selection criterion.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tgid_get(int unit, bcm_port_t port, int *tid, int *psc)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(tid, SHR_E_PARAM);
    SHR_NULL_CHECK(psc, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tgid_get(unit, port, tid, psc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the trunk group for a given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] tid trunk ID.
 * \param [in] psc trunk selection criterion.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_tgid_set(int unit, bcm_port_t port, int tid, int psc)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_tgid_set(unit, port, tid, psc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the private VLAN attribute of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] vlan VLAN identifier.
 * \param [in] pkt_prio Egress packet priority. Negative priority disables priority overriding.
 * \param [in] flags BCM_PORT_FORCE_VLAN_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                             int pkt_prio, uint32 flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (!BCM_VLAN_VALID(vlan)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (pkt_prio > BCM_PRIO_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_force_vlan_set(unit, port, vlan, pkt_prio, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the private VLAN attribute of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] vlan VLAN identifier.
 * \param [out] pkt_prio Egress packet priority.
 * \param [out] flags BCM_PORT_FORCE_VLAN_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_force_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vlan,
                             int *pkt_prio, uint32 *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(vlan, SHR_E_PARAM);
    SHR_NULL_CHECK(pkt_prio, SHR_E_PARAM);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_force_vlan_get(unit, port, vlan, pkt_prio, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get filter mode for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] flags BCM_PORT_VLAN_MEMBER_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_vlan_member_get(int unit, bcm_port_t port, uint32 *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    if (BCM_GPORT_IS_FLOW_PORT(port)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_member_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port default vlan tag actions.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] flags BCM_PORT_VLAN_MEMBER_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_vlan_member_set(int unit, bcm_port_t port, uint32 flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_FLOW_PORT(port)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_member_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the ports class ID.
 *
 * Ports with the same class ID can be treated as a group in field processing
 * and VLAN translation.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] class Classification type.
 * \param [in] class_id Class ID of the port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_class_set(int unit, bcm_port_t port,
                        bcm_port_class_t class, uint32 class_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_class_set(unit, port, class, class_id));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_port_class_validate(unit, &port, class, class_id),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_class_set(unit, port, class, class_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the ports class ID.
 *
 * Ports with the same class ID can be treated as a group in field processing
 * and VLAN translation.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] class Classification type.
 * \param [out] class_id Class ID of the port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_class_get(int unit, bcm_port_t port,
                        bcm_port_class_t class, uint32 *class_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(class_id, SHR_E_PARAM);

    if (BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_class_get(unit, port, class, class_id));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_port_class_validate(unit, &port, class, CLASS_ID_INVALID),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_class_get(unit, port, class, class_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port discard attributes for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode Port discard mode, one of BCM_PORT_DISCARD_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_discard_get(int unit, bcm_port_t port, int *mode)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_discard_get(unit, port, mode));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Set port discard attributes for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode Port discard mode, one of BCM_PORT_DISCARD_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_discard_set(int unit, bcm_port_t port, int mode)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_discard_set(unit, port, mode));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Specify the color selection for the given priority.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pri Packet 802.1p priority.
 * \param [in] color Color assigned to packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_priority_color_set(int unit, bcm_port_t port, int pri,
                                 bcm_color_t color)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((pri < BCM_PRIO_MIN) || (pri > BCM_PRIO_MAX)
        || (color < 0) || (color >= bcmColorCount)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(color);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_phb_map_set(unit, port, pri, -1, -1, cng));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the color selection for the given priority.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pri Packet 802.1p priority.
 * \param [out] color Color assigned to packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_priority_color_get(int unit, bcm_port_t port, int pri,
                                 bcm_color_t *color)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(color, SHR_E_PARAM);
    if ((pri < BCM_PRIO_MIN) || (pri > BCM_PRIO_MAX)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_phb_map_get(unit, port, pri, 0, NULL, &cng));
    *color = BCMI_LTSW_QOS_COLOR_DECODING(cng);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Specify the color selection for the given CFI.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] cfi Packet CFI.
 * \param [in] color Color assigned to packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_cfi_color_set(int unit, bcm_port_t port, int cfi,
                            bcm_color_t color)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((cfi < 0) || (cfi > 1)
        || (color < 0) || (color >= bcmColorCount)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(color);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_phb_map_set(unit, port, -1, cfi, -1, cng));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the color selection for the given CFI.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] cfi Packet CFI.
 * \param [out] color Color assigned to packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_cfi_color_get(int unit, bcm_port_t port, int cfi,
                            bcm_color_t *color)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(color, SHR_E_PARAM);
    if ((cfi < 0) || (cfi > 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_phb_map_get(unit, port, 0, cfi, NULL, &cng));
    *color = BCMI_LTSW_QOS_COLOR_DECODING(cng);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set vlan priority and cfi to internal priority and color mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pri Packet 802.1p priority.
 * \param [in] cfi Packet CFI.
 * \param [in] int_pri Internal priority assigned to packet.
 * \param [in] color Color assigned to packet. -1 means not to change it.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_vlan_priority_map_set(int unit, bcm_port_t port, int pri, int cfi,
                                    int int_pri, bcm_color_t color)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((pri < BCM_PRIO_MIN) || (pri > BCM_PRIO_MAX)
        || (cfi < 0) || (cfi > 1)
        || (int_pri < 0) || (int_pri > BCMI_QOS_INT_PRIO_MAX(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    cng = (color >= 0) ? BCMI_LTSW_QOS_COLOR_ENCODING(color) : -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_phb_map_set(unit, port, pri, cfi, int_pri, cng));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get vlan priority and cfi to internal priority and color mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pri Packet 802.1p priority.
 * \param [in] cfi Packet CFI.
 * \param [out] int_pri Internal priority assigned to packet.
 * \param [out] color Color assigned to packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_vlan_priority_map_get(int unit, bcm_port_t port, int pri, int cfi,
                                    int *int_pri, bcm_color_t *color)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(int_pri, SHR_E_PARAM);
    SHR_NULL_CHECK(color, SHR_E_PARAM);
    if ((pri < BCM_PRIO_MIN) || (pri > BCM_PRIO_MAX)
        || (cfi < 0) || (cfi > 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_vlan_phb_map_get(unit, port, pri, cfi, int_pri, &cng));
    *color = BCMI_LTSW_QOS_COLOR_DECODING(cng);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Define the mapping of outgoing port, internal priority, and color to
 *        outgoing packet priority and cfi bit.
 *        This API programs only the mapping table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          Port number.
 * \param [in]  internal_pri  Internal priority.
 * \param [in]  color         Color.
 * \param [in]  pkt_pri       Packet priority.
 * \param [in]  cfi           Packet CFI.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_port_vlan_priority_unmap_set(int unit, bcm_port_t port,
                                      int internal_pri, bcm_color_t color,
                                      int pkt_pri, int cfi)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((internal_pri < 0) || (internal_pri > BCMI_QOS_INT_PRIO_MAX(unit))
        || ((color != bcmColorGreen) && (color != bcmColorYellow) &&
            (color != bcmColorRed))
        || (pkt_pri < BCM_PRIO_MIN) || (pkt_pri > BCM_PRIO_MAX)
        || (cfi < 0) || (cfi > 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(color);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_phb_vlan_map_set
            (unit, port, internal_pri, cng, pkt_pri, cfi));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the mapping of outgoing port, internal priority, and color to
 *        outgoing packet priority and cfi bit.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          Port number.
 * \param [in]  internal_pri  Internal priority.
 * \param [in]  color         Color.
 * \param [out] pkt_pri       Packet priority.
 * \param [out] cfi           Packet CFI.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_port_vlan_priority_unmap_get(int unit, bcm_port_t port,
                                      int internal_pri, bcm_color_t color,
                                      int *pkt_pri, int *cfi)
{
    int cng;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(pkt_pri, SHR_E_PARAM);
    SHR_NULL_CHECK(cfi, SHR_E_PARAM);
    if ((internal_pri < 0) || (internal_pri > BCMI_QOS_INT_PRIO_MAX(unit))
        || ((color != bcmColorGreen) && (color != bcmColorYellow) &&
            (color != bcmColorRed))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(color);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_phb_vlan_map_get
            (unit, port, internal_pri, cng, pkt_pri, cfi));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number. -1 to apply on all available ports.
 * \param [in] mode DSCP mapping mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if (BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_dscp_map_mode_set(unit, port, mode));
        SHR_EXIT();
    }

    if (port == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (!IS_ETH_PORT(unit, port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
    }

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_dscp_phb_map_mode_set(unit, port, mode));
    }


exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number. -1 to get from first available port.
 * \param [out] mode DSCP mapping mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    if (BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_dscp_map_mode_get(unit, port, mode));
        SHR_EXIT();
    }

    if (port == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
        BCM_PBMP_ITER(pbmp, port) {
            break;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (!IS_ETH_PORT(unit, port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_dscp_phb_map_mode_get(unit, port, mode));


exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set DSCP remapping, and DSCP to priority and color mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number. -1 to apply on all available ports.
 * \param [in] srccp Source DSCP. -1 to apply on all DSCP.
 * \param [in] mapcp Remapped DSCP.
 * \param [in] pri Priority and color for mapped DSCP.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_dscp_map_set(int unit, bcm_port_t port, int srccp, int mapcp,
                           int pri)
{
    int int_pri, cng;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((srccp < -1) || (srccp > BCMI_QOS_DSCP_MAX(unit))
        || (mapcp < 0) || (mapcp > BCMI_QOS_DSCP_MAX(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    int_pri = pri & BCM_PRIO_MASK;
    if ((int_pri < 0) || (int_pri > BCMI_QOS_INT_PRIO_MAX(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    cng = pri & ~BCM_PRIO_MASK;
    if (cng == BCM_PRIO_RED) {
        cng = BCMI_LTSW_QOS_COLOR_RED;
    } else if (cng == BCM_PRIO_YELLOW) {
        cng = BCMI_LTSW_QOS_COLOR_YELLOW;
    } else if ((cng == BCM_PRIO_GREEN) || (cng == 0)) {
        cng = BCMI_LTSW_QOS_COLOR_GREEN;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (port == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (!IS_ETH_PORT(unit, port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
    }

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_dscp_phb_map_set
                (unit, port, srccp, mapcp, int_pri, cng));
    }


exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get DSCP remapping, and DSCP to priority and color mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number. -1 to get from first available port.
 * \param [in] srccp Source DSCP. -1 to get from first DSCP.
 * \param [out] mapcp Remapped DSCP.
 * \param [out] pri Priority and color for mapped DSCP.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_dscp_map_get(int unit, bcm_port_t port, int srccp, int *mapcp,
                           int *pri)
{
    int int_pri, cng;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(mapcp, SHR_E_PARAM);
    SHR_NULL_CHECK(pri, SHR_E_PARAM);

    if ((srccp < -1) || (srccp > BCMI_QOS_DSCP_MAX(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (port == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
        BCM_PBMP_ITER(pbmp, port) {
            break;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (!IS_ETH_PORT(unit, port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_dscp_phb_map_get
            (unit, port, srccp, mapcp, &int_pri, &cng));

    *pri = int_pri;
    if (cng == BCMI_LTSW_QOS_COLOR_RED) {
        *pri |= BCM_PRIO_RED;
    } else if (cng == BCMI_LTSW_QOS_COLOR_YELLOW) {
        *pri |= BCM_PRIO_YELLOW;
    } else if (cng != BCMI_LTSW_QOS_COLOR_GREEN) {
        *pri |= BCM_PRIO_DROP_FIRST;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set internal priority and congestion to packet DSCP mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] int_pri Internal priority.
 * \param [in] cng Congestion.
 * \param [in] dscp DSCP marking on outgoing packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_dscp_unmap_set(int unit, bcm_port_t port, int internal_pri,
                             bcm_color_t color, int pkt_dscp)
{
    int cng;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((internal_pri < 0) || (internal_pri > BCMI_QOS_INT_PRIO_MAX(unit))
        || ((color != bcmColorGreen) && (color != bcmColorYellow) &&
            (color != bcmColorRed))
        || (pkt_dscp < 0) || (pkt_dscp > BCMI_QOS_DSCP_MAX(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(color);

    if (port == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (!IS_ETH_PORT(unit, port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
    }

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_phb_dscp_map_set
                (unit, port, internal_pri, cng, pkt_dscp));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get internal priority and congestion to packet DSCP mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] int_pri Internal priority.
 * \param [in] cng Congestion.
 * \param [out] dscp DSCP marking on outgoing packet.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_dscp_unmap_get(int unit, bcm_port_t port, int internal_pri,
                             bcm_color_t color, int *pkt_dscp)
{
    int cng;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(pkt_dscp, SHR_E_PARAM);
    if ((internal_pri < 0) || (internal_pri > BCMI_QOS_INT_PRIO_MAX(unit))
        || ((color != bcmColorGreen) && (color != bcmColorYellow) &&
            (color != bcmColorRed))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(color);

    if (port == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
        BCM_PBMP_ITER(pbmp, port) {
            break;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        if (!IS_ETH_PORT(unit, port)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_phb_dscp_map_get
            (unit, port, internal_pri, cng, pkt_dscp));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the spanning tree state for a port.
 * All STGs containing all VLANs containing the port are updated.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] stp_state State to place port in, one of BCM_PORT_STP_xxx.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_stp_set(int unit, bcm_port_t port, int stp_state)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stg_stp_set_all(unit, port, stp_state));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the spanning tree state for a port in the default STG.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] stp_state STP state.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_stp_get(int unit, bcm_port_t port, int *stp_state)
{
    int stg_default, rv;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(stp_state, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    rv = bcm_ltsw_stg_default_get(unit, &stg_default);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stg_stp_get(unit, stg_default, port, stp_state));
    } else if (rv == BCM_E_UNAVAIL) {   /* FABRIC switches, etc */
        *stp_state = BCM_STG_STP_FORWARD;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Enable/Disable BPDU reception on the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable TRUE to enable, FALSE to disable (reject bpdu).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_bpdu_enable_set(int unit, bcm_port_t port, int enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_bpdu_enable_set(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Return whether BPDU reception is enabled on the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] enable TRUE if enabled, FALSE if disabled
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_port_bpdu_enable_get(int unit, bcm_port_t port, int *enable)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_bpdu_enable_get(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_rate_egress_set(int unit, bcm_port_t port,
                              uint32 kbits_sec, uint32 kbits_burst)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_shaper_validate
            (unit, port, kbits_sec, kbits_burst, SHAPER_MODE_BYTE));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_shaper_set
            (unit, port, kbits_sec, kbits_burst, SHAPER_MODE_BYTE));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_rate_egress_get(int unit, bcm_port_t port,
                              uint32 *kbits_sec, uint32 *kbits_burst)
{
    bcmi_ltsw_port_shaper_mode_t mode;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(kbits_sec, SHR_E_PARAM);
    SHR_NULL_CHECK(kbits_burst, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_shaper_get
            (unit, port, kbits_sec, kbits_burst, &mode));

    if (mode != SHAPER_MODE_BYTE) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_rate_egress_pps_set(int unit, bcm_port_t port,
                                  uint32 kbits_sec, uint32 kbits_burst)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_shaper_validate
            (unit, port, kbits_sec, kbits_burst, SHAPER_MODE_PKT));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_shaper_set
            (unit, port, kbits_sec, kbits_burst, SHAPER_MODE_PKT));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_rate_egress_pps_get(int unit, bcm_port_t port,
                                  uint32 *kbits_sec, uint32 *kbits_burst)
{
    bcmi_ltsw_port_shaper_mode_t mode;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(kbits_sec, SHR_E_PARAM);
    SHR_NULL_CHECK(kbits_burst, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egr_shaper_get
            (unit, port, kbits_sec, kbits_burst, &mode));

    if (mode != SHAPER_MODE_PKT) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the count of packets (or cells) currently buffered for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] count Count of packets (or cells) currently buffered.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_port_queued_count_get(int unit, bcm_port_t port, uint32_t *count)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_queue_count_get(unit, port, count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_port_stat_attach(int unit, bcm_port_t port, uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_stat_attach(unit, port, stat_counter_id));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Detach counter entries to given port with a given stat counter id.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_port_stat_detach_with_id(int unit, bcm_port_t port,
                                  uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_stat_detach(unit, port, stat_counter_id));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex state action to the given port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] action_id Flex state ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_port_flexstate_attach(int unit, bcm_port_t port, uint32_t action_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_flexstate_attach(unit, port, action_id));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Detach flex state action to given port with a given stat counter id.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] action_id Flex state ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_port_flexstate_detach_with_id(int unit, bcm_port_t port,
                                       uint32_t action_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_flexstate_detach(unit, port, action_id));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_flood_block_set(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                              uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, ing_port, &ing_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, egr_port, &egr_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_flood_block_set(unit, ing_port, egr_port, flags));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_flood_block_get(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                              uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, ing_port, &ing_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, egr_port, &egr_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_flood_block_get(unit, ing_port, egr_port, flags));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egress_set(unit, port, modid, pbmp));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_egress_get(unit, port, modid, pbmp));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_match_add(int unit, bcm_gport_t port, bcm_port_match_info_t *match)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_add(unit, port, match));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_match_delete(int unit, bcm_gport_t port, bcm_port_match_info_t *match)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_delete(unit, port, match));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_match_delete_all(int unit, bcm_gport_t port)
{
    int size, i;
    bcm_port_match_info_t *array = NULL;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_multi_get(unit, port, 0, NULL, &size));
    SHR_ALLOC(array, sizeof(bcm_port_match_info_t) * size, "match_array");
    sal_memset(array, 0, sizeof(bcm_port_match_info_t) * size);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_multi_get(unit, port, size, array, &size));

    for (i = 0; i < size; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_match_delete(unit, port, array + i));
    }

exit:
    SHR_FREE(array);
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_match_multi_get(int unit, bcm_gport_t port, int size,
                              bcm_port_match_info_t *array, int *count)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((count == NULL) ||
        (size < 0) ||
        ((size > 0) && (array == NULL))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_multi_get(unit, port, size, array, count));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_match_replace(int unit, bcm_gport_t port,
                            bcm_port_match_info_t *old_match,
                            bcm_port_match_info_t *new_match)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(old_match, SHR_E_PARAM);
    SHR_NULL_CHECK(new_match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_delete(unit, port, old_match));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_match_add(unit, port, new_match));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_match_set(int unit, bcm_gport_t port,
                        int size, bcm_port_match_info_t *array)
{
    int i;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    if ((size <= 0) || (array == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < size; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_port_match_add(unit, port, array + i));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_subsidiary_ports_get(int unit, bcm_port_t port, bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_subsidiary_ports_get(unit, port, pbmp));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_encap_config_set(int unit, bcm_port_t port,
                               bcm_port_encap_config_t *encap_config)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_NULL_CHECK(encap_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    switch (encap_config->encap) {
        case BCM_PORT_ENCAP_HIGIG3:
            if (!(PORT(unit, port).flags & HG_VALID)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            /* no break */
        case BCM_PORT_ENCAP_IEEE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_encap_set(unit, port, encap_config->encap));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_encap_config_get(int unit, bcm_port_t port,
                               bcm_port_encap_config_t *encap_config)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(encap_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_encap_get(unit, port, (int *)&encap_config->encap));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_priority_group_config_set(
    int unit, bcm_gport_t gport, int priority_group,
    bcm_port_priority_group_config_t *prigrp_config)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(prigrp_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_priority_group_config_set
            (unit, gport, priority_group, prigrp_config));

exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_port_priority_group_config_get(
    int unit, bcm_gport_t gport, int priority_group,
    bcm_port_priority_group_config_t *prigrp_config)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(prigrp_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_priority_group_config_get
            (unit, gport, priority_group, prigrp_config));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_l2_interface_id_get(int unit, bcm_gport_t port, int *interface_id)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(interface_id, SHR_E_PARAM);

    if ((!BCM_GPORT_IS_SET(port)) && (!PORT_IS_VALID(unit, port))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_to_l2_if(unit, port, interface_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_l2_interface_gport_get(int unit, int interface_id, bcm_gport_t *port)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(port, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_to_port(unit, interface_id, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_pipe_pbmp_get(int unit, int pipe, bcm_pbmp_t *pbmp)
{
    bcm_pbmp_t pbmp_all;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(*pbmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pipe_logic_pbmp(unit, pipe, pbmp));

    BCM_PBMP_CLEAR(pbmp_all);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit,
                              (BCMI_LTSW_PORT_TYPE_PORT |
                               BCMI_LTSW_PORT_TYPE_CPU |
                               BCMI_LTSW_PORT_TYPE_MGMT),
                              &pbmp_all));
    BCM_PBMP_AND(*pbmp, pbmp_all);

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_buffer_pbmp_get(int unit, int buffer_id, bcm_pbmp_t *pbmp)
{
    bcm_pbmp_t pbmp_all;
    int port, itm;
    bcm_info_t info;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_info_get(unit, &info));

    if (buffer_id >= info.num_buffers) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    BCM_PBMP_CLEAR(*pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit,
                              (BCMI_LTSW_PORT_TYPE_PORT |
                               BCMI_LTSW_PORT_TYPE_CPU |
                               BCMI_LTSW_PORT_TYPE_MGMT),
                              &pbmp_all));

    BCM_PBMP_ITER(pbmp_all, port) {
        itm = BCM_COSQ_BUFFER_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_itm_validate(unit, port, &itm));
        if (itm == buffer_id) {
            BCM_PBMP_PORT_ADD(*pbmp, port);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_port_group_range_get(int unit, bcm_port_group_type_t type, bcm_port_group_range_t *range)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_NULL_CHECK(range, SHR_E_PARAM);

    if ((type < 0) || (type >= bcmPortGroupCount)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_group_range_get(unit, type, range));

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_ltsw_port_phy_control_rx_get(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32_t *value)
{
    pc_phy_control_data_t phy_status;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phy_status_get(unit, port, 0, &phy_status));

    switch (type) {
        case BCM_PORT_PHY_CONTROL_RX_TAP1:
            *value = phy_status.rx_afe_dfe_tap[0];
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP2:
            *value = phy_status.rx_afe_dfe_tap[1];
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP3:
            *value = phy_status.rx_afe_dfe_tap[2];
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP4:
            *value = phy_status.rx_afe_dfe_tap[3];
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP5:
            *value = phy_status.rx_afe_dfe_tap[4];
            break;
        case BCM_PORT_PHY_CONTROL_RX_PEAK_FILTER:
            *value = phy_status.rx_afe_pf;
            break;
        case BCM_PORT_PHY_CONTROL_RX_VGA:
            *value = phy_status.rx_afe_vga;
            break;
        case BCM_PORT_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
            *value = phy_status.rx_afe_lf_pf;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_signal_detect_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_phy_control_data_t phy_status_data;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    /*Get the start lane of the port*/

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phy_status_get(unit, port, 0, &phy_status_data));

    *value = phy_status_data.rx_signal_detect;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_rx_seq_done_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_status_data_t status_data;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));
    *value = status_data.pmd_rx_lock;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_squelch_get(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32_t *value)
{
    pc_phy_control_data_t phy_status;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phy_status_get(unit, port, 0, &phy_status));

    switch (type) {
        case BCM_PORT_PHY_CONTROL_TX_LANE_SQUELCH:
            *value = phy_status.tx_squelch;
            break;
        case BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH:
            *value = phy_status.rx_squelch;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_tx_polarity_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_pm_core_data_t pm_core;
    pc_phys_port_data_t phys_port;
    int pport, pm_id, core_index, pm_pport_id;
    uint32 num_lane;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    pport = PORT(unit,port).pport;;
    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_phys_port_get(unit, pport, &phys_port));
    pm_id = phys_port.pm_id;
    pm_pport_id = phys_port.pm_pport_id;

    core_index = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pm_core_get(unit, pm_id, core_index, &pm_core));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (num_lane == 0x1) {
        *value = (pm_core.tx_polarity_filp_oper >> pm_pport_id) & 0x1;
    } else if (num_lane == 0x2) {
        *value = (pm_core.tx_polarity_filp_oper >> pm_pport_id) & 0x3;
    } else if (num_lane == 0x4) {
        *value = (pm_core.tx_polarity_filp_oper >> pm_pport_id) & 0xf;
    } else if (num_lane == 0x8) {
        *value = (pm_core.tx_polarity_filp_oper >> pm_pport_id) & 0xff;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_rx_polarity_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_pm_core_data_t pm_core;
    pc_phys_port_data_t phys_port;
    int pport, pm_id, core_index, pm_pport_id;
    uint32 num_lane;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    pport = PORT(unit,port).pport;
    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_phys_port_get(unit, pport, &phys_port));
    pm_id = phys_port.pm_id;
    pm_pport_id = phys_port.pm_pport_id;

    core_index = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pm_core_get(unit, pm_id, core_index, &pm_core));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (num_lane == 0x1) {
        *value = (pm_core.rx_polarity_filp_oper >> pm_pport_id) & 0x1;
    } else if (num_lane == 0x2) {
        *value = (pm_core.rx_polarity_filp_oper >> pm_pport_id) & 0x3;
    } else if (num_lane == 0x4) {
        *value = (pm_core.rx_polarity_filp_oper >> pm_pport_id) & 0xf;
    } else if (num_lane == 0x8) {
        *value = (pm_core.rx_polarity_filp_oper >> pm_pport_id) & 0xff;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_tx_pi_override_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_phy_control_data_t phy_status;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phy_status_get(unit, port, 0, &phy_status));

    *value = phy_status.tx_pi_freq_override;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_link_training_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_status_data_t status_data;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));
    *value = status_data.link_training;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_cl72_status_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_status_data_t status_data;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));
    *value = status_data.link_training_done;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_tx_pam4_preocder_enable_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_phy_control_data_t phy_status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phy_status_get(unit, port, 0, &phy_status));

    *value = phy_status.pam4_tx_precoder;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_lp_tx_precoder_enable_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_pmd_fw_data_t pmd_fw_status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pmd_fw_status_get(unit, port, &pmd_fw_status));

    *value = pmd_fw_status.lp_tx_precoder_on;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_pmd_debug_lane_event_log_level_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_phy_control_data_t control_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phy_control_get(unit, port, 0, &control_data));

    *value = control_data.pmd_debug_lane_event_log_level;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fec_corrected_codeword_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_diag_stats_data_t diag_stats_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_diag_stats_get(unit, port, &diag_stats_data));

    *value = diag_stats_data.fec_correct_cws;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_fec_uncorrected_codeword_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_diag_stats_data_t diag_stats_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_diag_stats_get(unit, port, &diag_stats_data));

    *value = diag_stats_data.fec_uncorrect_cws;

exit:
    SHR_FUNC_EXIT();
}

int bcmint_ltsw_port_fec_symbol_error_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_diag_stats_data_t diag_stats_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_diag_stats_get(unit, port, &diag_stats_data));

    *value = diag_stats_data.fec_symb_errs;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get PHY specific properties.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          Port number.
 * \param [in]  type          configuration type.
 * \param [out] value         value for the configuration.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */

int
bcm_ltsw_port_phy_control_get(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    switch (type) {
        case BCM_PORT_PHY_CONTROL_LOOPBACK_INTERNAL:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_phy_control_loopback_internal_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_LOOPBACK_PMD:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_phy_control_loopback_pmd_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_LOOPBACK_REMOTE:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_phy_control_loopback_remote_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_RX_SIGNAL_DETECT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_signal_detect_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_RX_SEQ_DONE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_rx_seq_done_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_TX_LANE_SQUELCH:
        case BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_phy_squelch_get(unit, port, type, value));
            break;
        case BCM_PORT_PHY_CONTROL_TX_POLARITY:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_tx_polarity_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_RX_POLARITY:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_rx_polarity_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_PHASE_INTERP:
        case BCM_PORT_PHY_CONTROL_TX_PPM_ADJUST:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_phy_tx_pi_override_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_CL72:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_link_training_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_CL72_STATUS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_cl72_status_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_tx_pam4_preocder_enable_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_lp_tx_precoder_enable_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_phy_pmd_debug_lane_event_log_level_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_fec_corrected_codeword_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_fec_uncorrected_codeword_get(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_FEC_SYMBOL_ERROR_COUNT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_fec_symbol_error_get(unit, port, value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_control_rx_set(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32_t value)
{
    pc_phy_control_data_t control_data;
    uint32 num_lane, phy_ctrl_field_bmp;
    int lane;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    switch (type) {
        case BCM_PORT_PHY_CONTROL_RX_TAP1:
            phy_ctrl_field_bmp = PC_RX_AFE_DFE_TAP1|PC_RX_AFE_DFE_TAP_AUTO;
            control_data.rx_afe_dfe_tap[0] = value;
            control_data.rx_afe_dfe_tap_auto = 0;
            control_data.rx_afe_dfe_tap_sign[0] = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP2:
            phy_ctrl_field_bmp = PC_RX_AFE_DFE_TAP2|PC_RX_AFE_DFE_TAP_AUTO;
            control_data.rx_afe_dfe_tap[1] = value;
            control_data.rx_afe_dfe_tap_auto = 0;
            control_data.rx_afe_dfe_tap_sign[1] = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP3:
            phy_ctrl_field_bmp = PC_RX_AFE_DFE_TAP3|PC_RX_AFE_DFE_TAP_AUTO;
            control_data.rx_afe_dfe_tap[2] = value;
            control_data.rx_afe_dfe_tap_auto = 0;
            control_data.rx_afe_dfe_tap_sign[2] = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP4:
            phy_ctrl_field_bmp = PC_RX_AFE_DFE_TAP4|PC_RX_AFE_DFE_TAP_AUTO;
            control_data.rx_afe_dfe_tap[3] = value;
            control_data.rx_afe_dfe_tap_auto = 0;
            control_data.rx_afe_dfe_tap_sign[3] = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_TAP5:
            phy_ctrl_field_bmp = PC_RX_AFE_DFE_TAP5|PC_RX_AFE_DFE_TAP_AUTO;
            control_data.rx_afe_dfe_tap[4] = value;
            control_data.rx_afe_dfe_tap_auto = 0;
            control_data.rx_afe_dfe_tap_sign[4] = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_PEAK_FILTER:
            phy_ctrl_field_bmp = PC_RX_AFE_PEAKING_FILTER|PC_RX_AFE_PEAKING_FILTER_AUTO;
            control_data.rx_afe_pf = value;
            control_data.rx_afe_pf_auto = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_VGA:
            phy_ctrl_field_bmp = PC_RX_AFE_VGA|PC_RX_AFE_VGA_AUTO;
            control_data.rx_afe_vga = value;
            control_data.rx_afe_vga_auto = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
            phy_ctrl_field_bmp = PC_RX_AFE_LOW_FREQ_PEAKING_FILTER|PC_RX_AFE_LOW_FREQ_PEAKING_FILTER_AUTO;
            control_data.rx_afe_lf_pf = value;
            control_data.rx_afe_lf_pf_auto = 0;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (lane = 0; lane < num_lane; lane++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_phy_control_set(unit, port, lane, &control_data, phy_ctrl_field_bmp));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_squelch_set(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32_t value)
{
    pc_phy_control_data_t control_data;
    uint32 num_lane, phy_ctrl_field_bmp;
    int lane;
    int squelch_ctrl;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (value) {
        squelch_ctrl = 1;
    } else {
        squelch_ctrl = 0;
    }

    switch (type) {
        case BCM_PORT_PHY_CONTROL_TX_LANE_SQUELCH:
            phy_ctrl_field_bmp = PC_TX_SQUELCH|PC_TX_SQUELCH_AUTO;
            control_data.tx_squelch = squelch_ctrl;
            control_data.tx_squelch_auto = 0;
            break;
        case BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH:
            phy_ctrl_field_bmp = PC_RX_SQUELCH|PC_RX_SQUELCH_AUTO;
            control_data.rx_squelch = squelch_ctrl;
            control_data.rx_squelch_auto = 0;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (lane = 0; lane < num_lane; lane++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_phy_control_set(unit, port, lane, &control_data, phy_ctrl_field_bmp));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_tx_pi_override_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_phy_control_data_t control_data;
    uint32 num_lane;
    int lane;
    uint64 phy_ctrl_field_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    phy_ctrl_field_bmp = PC_TX_PI_FREQ_OVERRIDE|PC_TX_PI_FREQ_OVERRIDE_AUTO;

    control_data.tx_pi_freq_override = value;
    control_data.tx_pi_freq_override_auto = 0;
    control_data.tx_pi_freq_override_sign = 0;

    for (lane = 0; lane < num_lane; lane++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_phy_control_set(unit, port, lane, &control_data, phy_ctrl_field_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_phy_pmd_debug_lane_event_log_level_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_phy_control_data_t control_data;
    uint32 num_lane;
    int lane;
    uint64 phy_ctrl_field_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    phy_ctrl_field_bmp = PC_PMD_DEBUG_LANE_EVENT_LOG_LEVEL;

    control_data.pmd_debug_lane_event_log_level = value;

    for (lane = 0; lane < num_lane; lane++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_phy_control_set(unit, port, lane, &control_data, phy_ctrl_field_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_tx_polarity_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_pm_core_data_t pm_core;
    pc_phys_port_data_t phys_port;
    int pport, pm_id, core_index, pm_pport_id;
    uint32 num_lane;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    pport = PORT(unit,port).pport;
    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_phys_port_get(unit, pport, &phys_port));
    pm_id = phys_port.pm_id;
    pm_pport_id = phys_port.pm_pport_id;

    core_index = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pm_core_get(unit, pm_id, core_index, &pm_core));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (num_lane == 0x1) {
        value &= 0x1;
        pm_core.tx_polarity_filp &= ~(0x1 << pm_pport_id);
    } else if (num_lane == 0x2) {
        value &= 0x3;
        pm_core.tx_polarity_filp &= ~(0x3 << pm_pport_id);
    } else if (num_lane == 0x4) {
        value &= 0xf;
        pm_core.tx_polarity_filp &= ~(0xf << pm_pport_id);
    } else if (num_lane == 0x8) {
        value &= 0xff;
        pm_core.tx_polarity_filp &= ~(0xff << pm_pport_id);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    pm_core.tx_polarity_filp |= value << pm_pport_id;
    pm_core.tx_polarity_filp_auto = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pm_core_set(unit, pm_id, core_index, &pm_core, PC_TX_POLARITY_FLIP|PC_TX_POLARITY_FLIP_AUTO));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_link_training_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_port_data_t port_data;
    uint32_t set_field_bmp = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    if (value) {
        port_data.link_training = 1;
    } else {
        port_data.link_training = 0;
    }
    set_field_bmp |= PC_PORT_LINK_TRAINING;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, set_field_bmp));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_rx_polarity_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_pm_core_data_t pm_core;
    pc_phys_port_data_t phys_port;
    int pport, pm_id, core_index, pm_pport_id;
    uint32 num_lane;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    pport = PORT(unit,port).pport;
    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_phys_port_get(unit, pport, &phys_port));
    pm_id = phys_port.pm_id;
    pm_pport_id = phys_port.pm_pport_id;

    core_index = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pm_core_get(unit, pm_id, core_index, &pm_core));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (num_lane == 0x1) {
        value &= 0x1;
        pm_core.rx_polarity_filp &= ~(0x1 << pm_pport_id);
    } else if (num_lane == 0x2) {
        value &= 0x3;
        pm_core.rx_polarity_filp &= ~(0x3 << pm_pport_id);
    } else if (num_lane == 0x4) {
        value &= 0xf;
        pm_core.rx_polarity_filp &= ~(0xf << pm_pport_id);
    } else if (num_lane == 0x8) {
        value &= 0xff;
        pm_core.rx_polarity_filp &= ~(0xff << pm_pport_id);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    pm_core.rx_polarity_filp |= value << pm_pport_id;
    pm_core.rx_polarity_filp_auto = 0;

    SHR_IF_ERR_VERBOSE_EXIT(
        ltsw_pc_pm_core_set(unit, pm_id, core_index, &pm_core, PC_RX_POLARITY_FLIP|PC_RX_POLARITY_FLIP_AUTO));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_tx_pam4_preocder_enable_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_phy_control_data_t control_data;
    uint32_t cl72_en, num_lane;
    int lane;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_link_training_get(unit, port, &cl72_en));
    if (cl72_en) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                 "ERROR: feature not supported with link training on \n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, port, &num_lane));

    if (value) {
        control_data.pam4_tx_precoder = 1;
    } else {
        control_data.pam4_tx_precoder = 0;
    }

    control_data.pam4_tx_precoder_auto = 0;

    for (lane = 0; lane < num_lane; lane++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ltsw_pc_phy_control_set(unit, port, lane, &control_data, PC_PAM4_TX_PRECODER|PC_PAM4_TX_PRECODER_AUTO));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_port_lp_tx_precoder_enable_set(int unit, bcm_port_t port, uint32_t value)
{
    pc_pmd_fw_data_t pmd_fw_data;
    uint32_t cl72_en;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_link_training_get(unit, port, &cl72_en));
    if (cl72_en) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                 "ERROR: feature not supported with link training on \n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    if (value) {
        pmd_fw_data.lp_tx_precoder_on = 1;
    } else {
        pmd_fw_data.lp_tx_precoder_on = 0;
    }

    pmd_fw_data.lp_tx_precoder_on_auto = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pmd_fw_set(unit, port, &pmd_fw_data, PC_LP_TX_PRECODER_ON|PC_LP_TX_PRECODER_ON_AUTO));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set PHY specific properties.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          Port number.
 * \param [in]  type          configuration type.
 * \param [in]  value         value for the configuration.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */

int
bcm_ltsw_port_phy_control_set(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    switch (type) {
        case BCM_PORT_PHY_CONTROL_LOOPBACK_INTERNAL:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_phy_control_loopback_internal_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_LOOPBACK_PMD:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_phy_control_loopback_pmd_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_LOOPBACK_REMOTE:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_port_phy_control_loopback_remote_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_TX_LANE_SQUELCH:
        case BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_phy_squelch_set(unit, port, type, value));
            break;
        case BCM_PORT_PHY_CONTROL_PHASE_INTERP:
        case BCM_PORT_PHY_CONTROL_TX_PPM_ADJUST:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_phy_tx_pi_override_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_TX_POLARITY:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_tx_polarity_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_RX_POLARITY:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_rx_polarity_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_CL72:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_link_training_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_tx_pam4_preocder_enable_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_lp_tx_precoder_enable_set(unit, port, value));
            break;
        case BCM_PORT_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_phy_pmd_debug_lane_event_log_level_set(unit, port, value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief lag failover enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable Enable or Disable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_lag_failover_enable_set(int unit, bcm_port_t port, uint32_t enable)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    port_data.lag_failover = enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_set(unit, port, &port_data, PC_PORT_LAG_FAILOVER));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief lag failover enable get.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [OUT] enable Enable or Disable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_lag_failover_enable_get(int unit, bcm_port_t port, uint32_t *enable)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    *enable = port_data.lag_failover;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief lag failover loopback get.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [OUT] value loopback or not.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_lag_failover_loopback_get(int unit, bcm_port_t port, uint32_t *value)
{
    pc_port_status_data_t status_data;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);
    PORT_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_status_get(unit, port, &status_data));
    *value = status_data.failover_lb;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief pm id get.
 *
 * \param [in] unit     Unit number.
 * \param [in] pport    physical port.
 * \param [OUT] pm_id   pm_id.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pport_pm_id_get(int unit, int pport, int *pm_id)
{
    pc_phys_port_data_t phy_port_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pm_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phys_port_get(unit, pport, &phy_port_data));

    *pm_id = phy_port_data.pm_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VCO get.
 *
 * \param [in] unit     Unit number.
 * \param [in] pm_id    pm id.
 * \param [OUT] pll_num PLL number in the PM core.
 * \param [OUT] tvco.
 * \param [OUT] ovco ovco is invalid if it`s single PLL.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_vco_get(int unit, int pm_id, int *pll_num, int *tvco, int *ovco)
{
    pc_pm_prop_data_t pm_prop_data;
    int pll0, pll1;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_pm_prop_get(unit, pm_id, &pm_prop_data));

    *pll_num = pm_prop_data.num_pll;
    if (pm_prop_data.num_pll == 1) {
        /*If the PM core only supports single PLL, return the VCO via tvco*/
        *tvco = name2id(vco_rate_map, COUNTOF(vco_rate_map), pm_prop_data.vco_rate[0]);
        if (*tvco == -1) {
            *tvco = ltswPortVCOInvalid;
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (pm_prop_data.num_pll == 2) {
        pll0 = name2id(vco_rate_map, COUNTOF(vco_rate_map), pm_prop_data.vco_rate[0]);
        if (pll0 == -1) {
            pll0 = ltswPortVCOInvalid;
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        pll1 = name2id(vco_rate_map, COUNTOF(vco_rate_map), pm_prop_data.vco_rate[1]);
        if (pll1 == -1) {
            pll1 = ltswPortVCOInvalid;
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (pm_prop_data.tvco_index == 0) {
            *tvco = pll0;
            *ovco = pll1;
        } else if (pm_prop_data.tvco_index == 1) {
            *tvco = pll1;
            *ovco = pll0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief lane mask get by physical port.
 *
 * \param [in] unit     Unit number.
 * \param [in] pport    physical Port number.
 * \param [in] num_lane lane Port number.
 * \param [OUT] lane_mask .
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pport_lane_mask_get(int unit, int pport, int num_lane, uint32_t *lane_mask)
{
    pc_phys_port_data_t pport_data;
    int pm_pport_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_phys_port_get(unit, pport, &pport_data));

    pm_pport_id = (int)pport_data.pm_pport_id * MIN_LANES_PER_PPORT(unit);

    if (num_lane == 0x1) {
        *lane_mask = 1 << pm_pport_id;
    } else if (num_lane == 0x2) {
        *lane_mask = 0x3 << pm_pport_id;
    } else if (num_lane == 0x4) {
        *lane_mask = 0xf << pm_pport_id;
    } else if (num_lane == 0x8) {
        *lane_mask = 0xff;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief port pbmp get by the PM_ID.
 *
 * \param [in] unit     Unit number.
 * \param [in] pm_id    pm id.
 * \param [OUT] port_pbmp .
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_pm_ports_get(int unit, int pm_id, bcm_pbmp_t *port_pbmp)
{
    int lport, pport;
    pc_phys_port_data_t pport_data;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(*port_pbmp);

    PORT_ALL_ITER(unit, lport) {

        pport = PORT(unit, lport).pport;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_pc_phys_port_get(unit, pport, &pport_data));

        if (pm_id == pport_data.pm_id) {
            BCM_PBMP_PORT_ADD(*port_pbmp, lport);
        }
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Get the customer vco for the speed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] custom_speed_vco.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_custom_speed_vco_get(int unit, bcm_port_t port, int *custom_speed_vco)
{
    pc_port_data_t port_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_fp_validate(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_pc_port_get(unit, port, &port_data));

    *custom_speed_vco = name2id(custom_speed_vco_map,
                                COUNTOF(custom_speed_vco_map), port_data.custom_speed_vco);
    if (*custom_speed_vco == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_port_clear
 * Purpose:
 *      Clear the PORT without resetting stacking ports.
 * Parameters:
 *      unit - unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 * Notes:
 *      By default ports come up enabled. They can be made to come up disabled
 *      at startup by a compile-time application policy flag in your Make.local
 *      A call to bcm_port_clear should exhibit similar behavior for
 *      non-stacking ethernet ports
 *      PTABLE initialized.
 */
int
bcm_ltsw_port_clear(int unit)
{
    return SHR_E_UNAVAIL;
}
