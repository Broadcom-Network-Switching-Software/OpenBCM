/*! \file bcmpc_lt_cfg.h
 *
 * BCMPC Logical Table configuration interface defintions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module Logical Table (LT) interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_LT_CFG_H
#define BCMPC_LT_CFG_H


#include <shr/shr_bitop.h>

#include <bcmpc/bcmpc_types.h>

/******************************************************************************
 *
 * Global BCMPC LT related enumerations.
 */

/*!
 * \brief BCMPC LT entry state enumeration.
 *
 * This enumerations are used to indicate the operational
 * state of a PC LT entry.
 */
typedef enum bcmpc_entry_state_e {

    /*! Entry is valid. */
    BCMPC_VALID = 0,

    /*! Entry is invalid - unknown logical port. */
    BCMPC_PORT_UNKNOWN,

    /*! Unknown logical port to physical port mapping.  */
    BCMPC_PORT_MAP_UNKNOWN,

    /*! Unknown port speed. */
    BCMPC_PORT_SPEED_UNKNOWN,

    /*! Unknown Number of lanes configuration for the logical port. */
    BCMPC_PORT_NUM_LANES_UNKNOWN,

    /*! Profile incomplete. */
    BCMPC_PROFILE_INCOMPELETE,

    /*! Port configuration incomplete. */
    BCMPC_PORT_CFG_INCOMPELETE,

    /*! Physical Port invalid. */
    BCMPC_PHYSICAL_PORT_INVALID,

    /*! Physical Port invalid. */
    BCMPC_RLM_AUTONEG_CFG_CONFLICT,

    /*! RLM not supported on the logical port. */
    BCMPC_RLM_NO_SUPPORT,

    /*!
     * RLM not supported on the logical port
     * with the current speed setting.
     */
    BCMPC_RLM_CFG_PORT_SPEED_INVALID,

    /*!
     * RLM configuration invalid, due to
     * incorrect active lane mask selection
     * on the logical port.
     */
    BCMPC_RLM_CFG_ACTIVE_LANE_MASK_INVALID,

    /*!
     * Physical port conflict.
     * Indicates that the same physical port is mapped to
     * multiple logical ports. This could be due to incorrect
     * number of lanes configuration for a llgical port or
     * could be due to incorrect logical to physcial map
     * configuration.
     */
    BCMPC_PHYSICAL_PORT_CONFLICT,

    /*!
     * PM VCO violation, this indicates that the logical port
     * comfiguration resulted in VCO setting which cannot
     * be honored due to current PM VCO selection based on the
     * already configured logical ports withing the PM>
     */
    BCMPC_PORT_CFG_PM_VCO_VIOLATION,

    /*! Internal Port Macro, invalid configuration. */
    BCMPC_INTERNAL_PM,

    /*! Inactive PM, not powered up. */
    BCMPC_INACTIVE_PM,

    /*!
     * Indicates that Link Training (CLAUSE 72) is enabled on the
     * logical port.
     */
    BCMPC_LINK_TRAINING_ACTIVE,

    /*! Invalid configuration. */
    BCMPC_CONFIG_INVALID
} bcmpc_entry_state_t;

/*!
 * \brief Logical port and lane identity.
 *
 * The structure is used to specify a logical port and lane.
 */
typedef struct bcmpc_port_lane_s {

    /*! Logical port ID. */
    bcmpc_lport_t lport;

    /*! Lane index of the logical port. */
    int lane_index;

} bcmpc_port_lane_t;

/******************************************************************************
 * Structure and enumerations definition used for TX_TAPS configuration.
 */

/*!
 * \brief PHY tx lane signalling mode.
 */
typedef enum bcmpc_phy_sig_mode_e {

    /*! NRZ mode. */
    BCMPC_PHY_SIG_MODE_NRZ,

    /*! PAM4 mode. */
    BCMPC_PHY_SIG_MODE_PAM4,

    /*! 50G PAM4 mode. */
    BCMPC_PHY_SIG_MODE_50G_PAM4

} bcmpc_phy_sig_mode_t;

/*!
 * \brief PHY TXFIR tap mode.
 */
typedef enum bcmpc_phy_txfir_tap_mode_e {

    /*! DEFAULT TAP mode. */
    BCMPC_PHY_TXFIR_TAP_DEFAULT,

    /*! NRZ lower power 3 taps. */
    BCMPC_PHY_TXFIR_NRZ_LP_TAPS_3,

    /*! NRZ 6 taps. */
    BCMPC_PHY_TXFIR_NRZ_TAPS_6,

    /*! PAM4 lower power 3 taps. */
    BCMPC_PHY_TXFIR_PAM4_LP_TAPS_3,

    /*! PAM4 6 taps. */
    BCMPC_PHY_TXFIR_PAM4_TAPS_6,

} bcmpc_phy_txfir_tap_mode_t;

/*!
 * \brief PC_TX_TAPS LT fields information.
 */
typedef enum bcmpc_tx_taps_lt_fields_e {
    BCMPC_TX_TAPS_LT_FLD_TX_PRE,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE2,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE2_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE2_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_MAIN,
    BCMPC_TX_TAPS_LT_FLD_TX_MAIN_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_MAIN_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_POST,
    BCMPC_TX_TAPS_LT_FLD_TX_POST_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_POST_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_POST2,
    BCMPC_TX_TAPS_LT_FLD_TX_POST2_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_POST2_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_POST3,
    BCMPC_TX_TAPS_LT_FLD_TX_POST3_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_POST3_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_RPARA,
    BCMPC_TX_TAPS_LT_FLD_TX_RPARA_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_RPARA_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_AMP,
    BCMPC_TX_TAPS_LT_FLD_TX_AMP_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_AMP_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE,
    BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TXFIR_TAP_MODE,
    BCMPC_TX_TAPS_LT_FLD_TXFIR_TAP_MODE_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_SIG_MODE,
    BCMPC_TX_TAPS_LT_FLD_TX_SIG_MODE_AUTO,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE3,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE3_SIGN,
    BCMPC_TX_TAPS_LT_FLD_TX_PRE3_AUTO
} BCmpc_tx_taps_lt_fields;

/*! Maximum number of LT fields in PC_TX_TAPs. */
#define PC_TX_TAPS_FIELD_ID_MAX    34

/*!
 * \brief Tx taps configuration.
 *
 * This structure includes the Tx lane configurations e.g. preemphasis and
 * amplitude control.
 */
typedef struct bcmpc_tx_taps_config_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_TX_TAPS_FIELD_ID_MAX);

    /*! TX FIR pre cursor is calculated internally when it is set. */
    bool  pre_auto;

    /*! Tx FIR pre cursor tap value. */
    uint16_t  pre;

    /*! Tx FIR pre cursor sign */
    bool pre_sign;

    /*! TX FIR pre2 cursor is calculated internally when it is set. */
    bool  pre2_auto;

    /*! Tx FIR pre2 cursor tap value. */
    uint16_t pre2;

    /*! Tx FIR pre2 cursor sign */
    bool pre2_sign;

    /*! TX FIR pre3 cursor is calculated internally when it is set. */
    bool  pre3_auto;

    /*! Tx FIR pre3 cursor tap value. */
    uint16_t pre3;

    /*! Tx FIR pre3 cursor sign */
    bool pre3_sign;

    /*! TX FIR main cursor is calculated internally when it is set. */
    bool  main_auto;

    /*! Tx FIR main cursor tap value. */
    uint16_t main;

    /*! Tx main cursor sign */
    bool main_sign;

    /*! TX FIR post cursor is calculated internally when it is set. */
    bool  post_auto;

    /*! Tx FIR post cursor tap value. */
    uint16_t post;

    /*! Tx post cursor sign */
    bool post_sign;

    /*! TX FIR post2 cursor is calculated internally when it is set. */
    bool  post2_auto;

    /*! Tx FIR post2 cursor tap value. */
    uint16_t post2;

    /*! Tx post2 cursor sign */
    bool post2_sign;

    /*! TX FIR post3 cursor is calculated internally when it is set. */
    bool  post3_auto;

    /*! Tx FIR post3 cursor tap value. */
    uint16_t post3;

    /*! Tx  FIR post2 cursor sign. */
    bool post3_sign;

    /*! TX FIR shunt value is calculated internally when it is set. */
    bool  rpara_auto;

    /*! Tx FIR shunt resistance value. */
    uint16_t rpara;

    /*! Tx FIR shunt resistance value sign. */
    bool rpara_sign;

    /*! TX amplitude is set internally when it is set. */
    bool  amp_auto;

    /*! Tx amplitude control. */
    uint16_t amp;

    /*! Tx amplitude control sign. */
    bool amp_sign;

    /*! TX driver mode is set internally when it is set. */
    bool  drv_mode_auto;

    /*! Tx driver mode. */
    uint16_t drv_mode;

    /*! Tx driver mode sign. */
    bool drv_mode_sign;

    /*! TX signalling mode is set internally when it is set. */
    bool  sig_mode_auto;

    /*! Tx signalling mode. */
    bcmpc_phy_sig_mode_t sig_mode;

    /*! TXFIR mode is set internally when it is set. */
    bool  txfir_tap_mode_auto;

    /*! TXFIR tap mode. */
    bcmpc_phy_txfir_tap_mode_t txfir_tap_mode;

    /*! Operational state of the TX TAPS entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_tx_taps_config_t;

/******************************************************************************
 * Structure and enumerations definition used for PHY_CONTROL configuration.
 */

/*!
 * The maximum number of RX DEF TAP, the number of taps
 * differ on different devices. Cho0sing a large value.
 */
#define BCMPC_NUM_RX_DFE_TAPS_MAX (24)

/*!
 * \brief PC_PHY LT fields information.
 */
typedef enum bcmpc_phy_control_lt_fields_e {
    BCMPC_FLD_TX_SQUELCH = 0,
    BCMPC_FLD_RX_SQUELCH,
    BCMPC_FLD_RX_ADAPTION_RESUME,
    BCMPC_FLD_RX_AFE_VGA,
    BCMPC_FLD_RX_AFE_PEAKING_FILTER,
    BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER,
    BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER,
    BCMPC_FLD_RX_AFE_DFE_TAP_SIGN,
    BCMPC_FLD_RX_AFE_DFE_TAP,
    BCMPC_FLD_PAM4_TX_PATTERN,
    BCMPC_FLD_PAM4_TX_PRECODER,
    BCMPC_FLD_PHY_ECC_INTR_EN,
    BCMPC_FLD_TX_PI_FREQ_OVERRIDE_SIGN,
    BCMPC_FLD_TX_PI_FREQ_OVERRIDE,
    BCMPC_FLD_PMD_DEBUG_LANE_EVENT_LOG_LEVEL,
    BCMPC_FLD_FEC_BYPASS_INDICATION,
    BCMPC_FLD_AUTONEG_FEC_OVERRIDE,
    BCMPC_FLD_TX_SQUELCH_AUTO,
    BCMPC_FLD_RX_SQUELCH_AUTO,
    BCMPC_FLD_RX_ADAPTION_RESUME_AUTO,
    BCMPC_FLD_RX_AFE_VGA_AUTO,
    BCMPC_FLD_RX_AFE_PEAKING_FILTER_AUTO,
    BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER_AUTO,
    BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER_AUTO,
    BCMPC_FLD_RX_AFE_DFE_TAP_AUTO,
    BCMPC_FLD_PAM4_TX_PATTERN_AUTO,
    BCMPC_FLD_PAM4_TX_PRECODER_AUTO,
    BCMPC_FLD_PHY_ECC_INTR_EN_AUTO,
    BCMPC_FLD_TX_PI_FREQ_OVERRIDE_AUTO,
    BCMPC_FLD_FEC_BYPASS_INDICATION_AUTO,
    BCMPC_FLD_AUTONEG_FEC_OVERRIDE_AUTO,
} bcmpc_phy_control_lt_fields_t;

/*! Maximum field ID of PC_PHY_CONTROL LT. */
#define PC_PHY_CTRL_FIELD_ID_MAX     30

/*!
 * \brief PAM4 TX pattern types.
 *
 * This enum defines the pam4_tx_pattern \ref bcmpc_phy_control_t.
 */
typedef enum bcmpc_pam4_tx_pattern_e {

    /*! PAM4 tx pattern none. */
    BCMPC_PAM4_PATTERN_NONE,

    /*! PAM4 tx JP03B pattern. */
    BCMPC_PAM4_PATTERN_JP03B,

    /*! PAM4 tx lineartiy pattern. */
    BCMPC_PAM4_PATTERN_LINEAR,

    /*! Must be the last. */
    BCMPC_PAM4_PATTERN_COUNT

} bcmpc_pam4_tx_pattern_t;

/*!
 * \brief phy control configuration.
 *
 * This structure is used to configure the phy control settings.
 */
typedef struct bcmpc_phy_control_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_PHY_CTRL_FIELD_ID_MAX);

    /*!
     * When set TX squelch setting is
     * automatically configured internally.
     */
    bool tx_squelch_auto;

    /*! Enable tx sqeulch. */
    bool tx_squelch;

    /*!
     * When set RX squelch setting is
     * automatically configured internally.
     */
    bool rx_squelch_auto;

    /*! Enable rx sqeulch. */
    bool rx_squelch;

    /*!
     * When set TX PI override setting is
     * automatically configured internally.
     */
    bool tx_pi_ovride_auto;

    /*! TX PI override. */
    uint32_t tx_pi_ovride;

    /*! TX PI override sign */
    bool tx_pi_ovride_sign;

    /*!
     * When set rx adaptation resume is
     * automatically configured internally.
     */
    bool rx_adpt_resume_auto;

    /*! Enable rx adaptation resume. */
    bool rx_adpt_resume;

    /*!
     * When set RX AFE VGA is
     * automatically configured internally.
     */
    bool rx_afe_vga_auto;

    /*! RX AFE VGA set. */
    uint32_t rx_afe_vga;

    /*!
     * When set RX AFE peaking filter is
     * automatically configured internally.
     */
    bool rx_afe_pk_flt_auto;

    /*! RX AFE peaking filter set. */
    uint32_t rx_afe_pk_flt;

    /*!
     * When set RX AFE low freq peaking filter is
     * automatically configured internally.
     */
    bool rx_afe_low_fre_pk_flt_auto;

    /*! RX AFE low freq peaking filter set. */
    uint32_t rx_afe_low_fre_pk_flt;

    /*!
     * When set RX AFE hi freq peaking filter is
     * automatically configured internally.
     */
    bool rx_afe_hi_fre_pk_flt_auto;

    /*! RX AFE high freq peaking filter set. */
    uint32_t rx_afe_hi_fre_pk_flt;

    /*!
     * When set RX AFE DFE TAP is
     * automatically configured internally.
     */
    bool rx_afe_dfe_tap_auto;

    /*! RX AFE DFE TAP sign ARRAY set. */
    bool rx_afe_dfe_tap_sign[BCMPC_NUM_RX_DFE_TAPS_MAX];

    /*! RX AFE DFE TAP ARRAY set. */
    uint32_t rx_afe_dfe_tap[BCMPC_NUM_RX_DFE_TAPS_MAX];

    /*! RX PPM */
    uint32_t rx_ppm;

    /*!
     * When set PAM4 TX pattern setting is
     * automatically configured internally.
     */
    bool pam4_tx_pat_auto;

    /*! Enable PAM4 TX pattern. */
    bcmpc_pam4_tx_pattern_t pam4_tx_pat;

    /*!
     * When set PAM4 TX precoder setting is
     * automatically configured internally.
     */
    bool pam4_tx_precoder_auto;

    /*! Set to enable PAM4 TX precoder. */
    bool pam4_tx_precoder;

    /*!
     * When set PHY ECC interrupts are
     * automatically configured internally.
     */
    bool phy_ecc_intr_auto;

    /*! Set to enable ECC interrupts supported. */
    bool phy_ecc_intr;

    /*! PMD debug lane event log level */
    uint8_t pmd_debug_lane_event_log_level;

    /*!
     * When set FEC bypass indication setting is
     * automatically configured internally.
     */
    bool phy_fec_bypass_ind_auto;

    /*!
     * Specify FEC bypass indication,
     * else FEC indication is active.
     */
    bool phy_fec_bypass_ind;

    /*!
     * When set, AN FEC Override setting is configured internally
     */
    bool phy_autoneg_fec_override_auto;

    /*!
     * Set to allow CL73 100G with no FEC
     * This is a non IEEE compliant mode and needs to be enabled explicitly
     */
    bool phy_autoneg_fec_override;

    /*! Operational state of the PHY control entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_phy_control_t;

/*!
 * \brief PHY_CONTROL LT field map.
 *
 * This structure is used to map the LT field to
 * local enumeration associated with the LT field.
 */
typedef struct bcmpc_phy_ctrl_fld_map_s {
    /*! PHY_CONTROL LT Field map. */
    bcmpc_phy_control_lt_fields_t fld_map;

    /*! PHY_CONTROL LT field Id. */
    uint32_t fid;
} bcmpc_phy_ctrl_fld_map_t;
#endif /* BCMPC_LT_CFG_H */
