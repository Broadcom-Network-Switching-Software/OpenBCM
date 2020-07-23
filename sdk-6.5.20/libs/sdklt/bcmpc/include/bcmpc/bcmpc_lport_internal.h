/*! \file bcmpc_lport_internal.h
 *
 * BCMPC LPORT Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_LPORT_INTERNAL_H
#define BCMPC_LPORT_INTERNAL_H

#include <shr/shr_bitop.h>
#include <shr/shr_thread.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_lt_cfg.h>

/*! Maximum length of thread name. */
#define PC_THREAD_NAME_MAX_LEN           16

/*!
 * \brief Globally available packet encapsulations.
 *
 * Note that this enum should be same as PC_ENCAP_T.
 */
typedef enum bcmpc_port_encap_e {

    /*! Standard Ethernet encapsulation. */
    BCMPC_PORT_ENCAP_IEEE = 0,

    /*! Broadcom HiGig+ encapsulation. */
    BCMPC_PORT_ENCAP_HIGIG,

    /*! Broadcom HiGig3 encapsulation. */
    BCMPC_PORT_ENCAP_HG3,

    /*! Broadcom cell mode. */
    BCMPC_PORT_ENCAP_CELL,

    /*! Must be the last. */
    BCMPC_PORT_ENCAP_COUNT

} bcmpc_port_encap_t;

/*! bcmpc port manager thread structure. */
typedef struct bcmpc_thread_ctrl_s {

    /*! port manager thread name. */
    char         name[PC_THREAD_NAME_MAX_LEN];

    /*! PFC DDR thread PID. */
    shr_thread_t pid;

    /*! Time between scans. */
    int          interval_us;

    /*! If to disable the thread. */
    int          pc_thread_disable;

} bcmpc_thread_ctrl_t;


/*!
 * \brief Port Control loopback types.
 *
 * PHY loopback modes must squelch the unused Tx and Rx paths if
 * possible.
 *
 * Outermost is closer to the line side of the PHY.
 *
 * Innermost is closer to the system (MAC) side of the PHY.
 *
 * Note that this enum should be same as PC_LOOPBACK_T.
 */
typedef pm_loopback_mode_t bcmpc_port_lpbk_t;

/*! No loopback. */
#define BCMPC_PORT_LPBK_NONE PM_PORT_LPBK_NONE

/*! MAC loopback. */
#define BCMPC_PORT_LPBK_MAC PM_PORT_LPBK_MAC

/*! PCS loopback. */
#define BCMPC_PORT_LPBK_PHY_PCS PM_PORT_LPBK_PHY_PCS

/*! PMD loopback. */
#define BCMPC_PORT_LPBK_PHY_PMD PM_PORT_LPBK_PHY_PMD

/*! PCS remote loopback. */
#define BCMPC_PORT_LPBK_REMOTE_PCS PM_PORT_LPBK_REMOTE_PCS

/*! PMD remote loopback. */
#define BCMPC_PORT_LPBK_REMOTE_PMD PM_PORT_LPBK_REMOTE_PMD

/*! Must be the last. */
#define BCMPC_PORT_LPBK_COUNT PM_PORT_LPBK_COUNT

/*!
 * \brief Port FEC types
 *
 * Note that this enum should be same as PC_FEC_T
 */
typedef pm_port_fec_t  bcmpc_fec_t;

/*! Disable FEC. */
#define BCMPC_FEC_NONE          PM_PORT_FEC_NONE
/*! BASE-R FEC (IEEE 802.3 clause 74). */
#define BCMPC_FEC_BASE_R        PM_PORT_FEC_BASE_R
/*! RS 528 FEC. */
#define BCMPC_FEC_RS_528        PM_PORT_FEC_RS_528
/*! RS 544 FEC, using 1xN RS FEC architecture. */
#define BCMPC_FEC_RS_544        PM_PORT_FEC_RS_544
/*! RS 272 FEC, using 1xN RS FEC architecture. */
#define BCMPC_FEC_RS_272        PM_PORT_FEC_RS_272
/*! RS 544 FEC, using 2xN RS FEC architecture. */
#define BCMPC_FEC_RS_544_2XN    PM_PORT_FEC_RS_544_2XN
/*! RS 272 FEC, using 2xN RS FEC architecture. */
#define BCMPC_FEC_RS_272_2XN    PM_PORT_FEC_RS_272_2XN
/*! Number of FEC types. */
#define BCMPC_FEC_COUNT         PM_PORT_FEC_COUNT

/*!
 * \brief Bit definition for auto-negotiation protocols.
 *
 * This enum defines the bits of \c an_protocol in \ref bcmpc_autoneg_prof_t.
 */
typedef enum bcmpc_an_protocol_e {

    /*! Parallel detection (no exchange of information). */
    BCMPC_AN_PROT_PARALLEL_DETECT,

    /*! Enable IEEE 802.3 clause 28 auto-negotiation. */
    BCMPC_AN_PROT_CL28,

    /*! Enable IEEE 802.3 clause 37 auto-negotiation. */
    BCMPC_AN_PROT_CL37,

    /*! Enable IEEE 802.3 clause 73 auto-negotiation. */
    BCMPC_AN_PROT_CL73,

    /*! Enable SGMII auto-negotiation. */
    BCMPC_AN_PROT_SGMII,

    /*! Enable Broadcom clause 37 auto-negotiation. */
    BCMPC_AN_PROT_BAM37,

    /*! Enable Broadcom clause 73 auto-negotiation. */
    BCMPC_AN_PROT_BAM73,

    /*! Must be the last. */
    BCMPC_AN_PROT_COUNT

} bcmpc_an_protocol_t;

/*!
 * \brief Bit definition for Auto-negotiation advertised misc abilities.
 *
 * This enum defines the bits of \c adv_ability in \ref bcmpc_autoneg_prof_t.
 */
typedef enum bcmpc_an_adv_ability_e {

    /*! Advertise asymmetric Tx PAUSE (symmetric if ADV_PAUSE_RX=1). */
    BCMPC_AN_ADV_PAUSE_TX,

    /*! Advertise symmetric if ADV_PAUSE_TX=1. */
    BCMPC_AN_ADV_PAUSE_RX,

    /*! Advertise 100 Mbps Energy Efficient Ethernet support. */
    BCMPC_AN_ADV_EEE_100M,

    /*! Advertise 1000 Mbps Energy Efficient Ethernet support. */
    BCMPC_AN_ADV_EEE_1G,

    /*! Must be the last. */
    BCMPC_AN_ADV_COUNT

} bcmpc_an_adv_ability_t;

/*!
 * \brief Autoneg Clauses.
 *
 * This enum defines Autoneg Clauses in \ref bcmpc_autoneg_prof_t.
 */
typedef enum bcmpc_phy_autoneg_mode_e {

    /*! NONE autoneg mode. */
    BCMPC_PHY_AUTONEG_MODE_NONE,

    /*! CLAUSE 37. */
    BCMPC_PHY_AUTONEG_MODE_CL37,

    /*! CLAUSE 37 BAM. */
    BCMPC_PHY_AUTONEG_MODE_CL37_BAM,

    /*! CLAUSE 73. */
    BCMPC_PHY_AUTONEG_MODE_CL73,

    /*! CLAUSE 73 BAM. */
    BCMPC_PHY_AUTONEG_MODE_CL73_BAM,

    /*! MSA (Multi-Source agreement) mode. */
    BCMPC_PHY_AUTONEG_MODE_MSA,

    /*! SGMII mode. */
    BCMPC_PHY_AUTONEG_MODE_SGMII,

    /*! Must be the last one. */
    BCMPC_PHY_AUTONEG_MODE_COUNT

} bcmpc_phy_autoneg_mode_t;

/*!
 * \brief PHY pause types.
 *
 * This enum defines phy pause type in \ref bcmpc_autoneg_prof_t.
 */
typedef enum bcmpc_phy_pause_type_e {

     /*! No PAUSE. */
    BCMPC_PHY_PAUSE_NONE,

    /*! TX pause. */
    BCMPC_PHY_PAUSE_TX,

    /*! RX pause. */
    BCMPC_PHY_PAUSE_RX,

    /*! Symmetric pause. */
    BCMPC_PHY_PAUSE_SYMM,

    /*! All pause type. */
    BCMPC_PHY_PAUSE_ALL,

    /*! Must be the last one. */
    BCMPC_PHY_PAUSE_COUNT

} bcmpc_phy_pause_type_t;

/*!
 * \brief Custom VCO frequency for the port speed.
 *
 * Note that this enum should be same as PC_SPEED_VCO_T.
 */
typedef enum bcmpc_speed_vco_e {

    /* No specific VCO setting. */
    BCMPC_SPEED_VCO_NONE,

    /* Speed 1G running at 6.25G VCO. */
    BCMPC_SPEED_1G_AT_6P25G_VCO,

    /* Speed 1G running at 12.5G VCO. */
    BCMPC_SPEED_1G_AT_12P5G_VCO,

    /* Speed 1G running at 25.781G VCO. */
    BCMPC_SPEED_1G_AT_25P781G_VCO,

    /* Speed 2.5G running at 12.5G VCO. */
    BCMPC_SPEED_2P5G_AT_12P5G_VCO,

    /* Speed 5G running at 12.5G VCO. */
    BCMPC_SPEED_5G_AT_12P5G_VCO

} bcmpc_speed_vco_t;

/*!
 * \brief PHY channel type.
 *
 * Note that this enum should be same as PC_PHY_CHANNEL_TYPE_T.
 */
typedef enum bcmpc_phy_channel_e {

     /*! Short channel. */
     BCMPC_PHY_CHANNEL_SHORT,

     /*! Long channel. */
     BCMPC_PHY_CHANNEL_LONG,

     /*! All channel type. */
     BCMPC_PHY_CHANNEL_ALL,

     /*! Must be the last one. */
     BCMPC_PHY_CHANNEL_COUNT
} bcmpc_phy_channel_t;

/*!
 * \brief PC_PORT LT fields information.
 */
typedef enum bcmpc_port_lt_fields_e {
    BCMPC_PORT_LT_FLD_ENABLE = 0,
    BCMPC_PORT_LT_FLD_SPEED,
    BCMPC_PORT_LT_FLD_NUM_LANES,
    BCMPC_PORT_LT_FLD_FEC_MODE,
    BCMPC_PORT_LT_FLD_ENCAP,
    BCMPC_PORT_LT_FLD_LINK_TRAINING,
    BCMPC_PORT_LT_FLD_LOOPBACK_MODE,
    BCMPC_PORT_LT_FLD_MAX_FRAME_SIZE,
    BCMPC_PORT_LT_FLD_LAG_FAILOVER,
    BCMPC_PORT_LT_FLD_NUM_VALID_AN_PROFILES,
    BCMPC_PORT_LT_FLD_AUTONEG_PROFILE_ID,
    BCMPC_PORT_LT_FLD_RLM,
    BCMPC_PORT_LT_FLD_INITIATOR,
    BCMPC_PORT_LT_FLD_ACTIVE_LANE_MASK,
    BCMPC_PORT_LT_FLD_SPEED_VCO_FREQ,
    BCMPC_PORT_LT_FLD_SUSPEND
} bcmpc_port_lt_fields_t;

/*! Maximum field ID of PC_PORT LTs. */
#define PC_PORT_FIELD_ID_MAX        16

/*!
 * \brief Port config structure
 *
 * This data structure defines the attributes for a logical port configuration.
 */
typedef struct bcmpc_port_cfg_s {

    /*! Physical device port number. */
    bcmpc_pport_t pport;

    /*! Administrative enable. */
    int enable;

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_PORT_FIELD_ID_MAX);

    /*! Port speed */
    uint32_t speed;

    /*! Number of lanes */
    uint8_t num_lanes;

    /*! FEC mode. */
    bcmpc_fec_t fec_mode;

    /*! Encapsulation mode. */
    bcmpc_port_encap_t encap_mode;

    /*! Link training. */
    bool link_training;

    /*! Loopback mode. */
    bcmpc_port_lpbk_t lpbk_mode;

    /*! Max frame size. */
    uint32_t max_frame_size;

    /*! Autoneg. */
    bool autoneg;

    /*! Valid number of autoneg profiles */
    uint8_t num_an_profs;

    /*! Autoneg profile ID. */
    uint32_t autoneg_profile_ids[NUM_AUTONEG_PROFILES_MAX];

    /*! RLM mode. */
    bool rlm_mode;

    /*! RLM initiator. */
    bool initiator;

    /*!
     * Active lane mask to be used for a logical port
     * when RLM_MODE is enabled and INITIATOR is true.
     */
    uint32_t active_lane_mask;

    /*! Enable to support LAG failover. */
    bool lag_failover;

    /*! Custom VCO frequency setting. */
    bcmpc_speed_vco_t  speed_vco_freq;

    /*!
     * Enable MMU cells drain on the port.
     * Do not enque packets to the port.
     */
    bool suspend;

    /*!
     * Restore normal operation of the port.
     * Enque packets to the port.
     */
    bool resume;

    /*! indicates if the port is in suspended state. */
    bool is_suspended;

    /*! Logical port number. */
    bcmpc_lport_t lport;

    /*! Indicates if Logical port is newly being created. */
    bool is_new;

    /*! Operational state of the PC_PORT entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_port_cfg_t;

/*!
 * \brief Auto-negotiation configuration profile.
 *
 * This structure is used to configure the auto-negotiation settings.
 * There are two fields in the structure; the first one describes the supported
 * auto-negotition methods, the second one describes the advertised abilities.
 *
 * The settings are in bit arrays. A bit is set when the corresponding protocol
 * or ability is enabled.
 * The bit definitions could be found by \ref bcmpc_an_protocol_t and
 * \ref bcmpc_an_adv_ability_t.
 */
typedef struct bcmpc_autoneg_prof_s {

    /*! Autoneg modes. */
    bcmpc_phy_autoneg_mode_t autoneg_mode;

    /*! Autoneg advertise speeds. */
    uint32_t an_advert_speed;

    /*! Phy medium type. */
    pm_port_medium_t phy_medium;

    /*! FEC mode. */
    bcmpc_fec_t fec_mode;

    /*! Pause type. */
    bcmpc_phy_pause_type_t  pause_type;

    /*! Enable long channel. */
    bool long_ch;

    /*! Number of lanes used after Autoneg. */
    uint8_t num_lanes;

    /*! Enable to turn link training off. Default link training is on */
    bool link_training_off;

} bcmpc_autoneg_prof_t;

/*!
 * \name Autoneg profile access.
 *
 * Macro for configuring bcmpc_autoneg_prof_t fields.
 */

/*! \{ */

/*! \} */

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
 * \brief Port timesync compensation mode.
 *
 * This enum defines the port_ts_comp_mode.
 */
typedef enum bcmpc_port_ts_comp_mode_e {

    /*! None Timesync compensation. */
    BCMPC_TIMESYNC_COMP_NONE,

    /*! Timesync compensation earlist lane mode. */
    BCMPC_TIMESYNC_COMP_EARLIEST_LANE,

    /*! Timesync compensation earlist lane mode. */
    BCMPC_TIMESYNC_COMP_LATEST_LANE,

    /*! Must be the last one. */
    BCMPC_TIMESYNC_COMP_COUNT

} bcmpc_port_ts_comp_mode_t;

/*!
 * \brief Port SYNCE stage0 mode.
 *
 * This enum defines the synce stage0 mode in \ref bcmpc_port_timesync_t.
 */
typedef enum bcmpc_port_synce_stg0_mode_e {

    /*! SYNCE stage no divider. Applicable to both stage 0. */
    BCMPC_PORT_SYNCE_STG0_MODE_DIV_NONE,

    /*! SYNCE stage 0 divider - gap clock 4 over 5. */
    BCMPC_PORT_SYNCE_STG0_MODE_DIV_GAP_CLK_4_OVER_5,

    /*! SYNCE stage 0 divider - SDM fractional divider. */
    BCMPC_PORT_SYNCE_STG0_MODE_SDM_FRAC_DIV,

    /*! SYNCE stage mode count. */
    BCMPC_PORT_SYNCE_STG0_MODE_COUNT

} bcmpc_port_synce_stg0_mode_t;

/*!
 * \brief Port SYNCE stage1 mode.
 *
 * This enum defines the synce stage mode in \ref bcmpc_port_timesync_t.
 */
typedef enum bcmpc_port_synce_stg1_mode_e {

    /*! SYNCE stage no divider. Applicable to both stage 1. */
    BCMPC_PORT_SYNCE_STG1_MODE_DIV_NONE,

    /*! SYNCE stage 1 divider 7. */
    BCMPC_PORT_SYNCE_STG1_MODE_DIV_7,

    /*! SYNCE stage 1 divider 11. */
    BCMPC_PORT_SYNCE_STG1_MODE_DIV_11,

    /*! SYNCE stage mode count. */
    BCMPC_PORT_SYNCE_STG1_MODE_COUNT

} bcmpc_port_synce_stg1_mode_t;

/*!
 * \brief PC_PORT_TIMESYNC LT fields information.
 */
typedef enum bcmpc_port_timesync_lt_fields_e {
    BCMPC_TIMESYNC_LT_FLD_IEEE_1588_ENABLE = 0,
    BCMPC_TIMESYNC_LT_FLD_ONE_STEP_TIMESTAMP,
    BCMPC_TIMESYNC_LT_FLD_IS_SOP,
    BCMPC_TIMESYNC_LT_FLD_TS_COMP_MODE,
    BCMPC_TIMESYNC_LT_FLD_STG0_MODE,
    BCMPC_TIMESYNC_LT_FLD_STG1_MODE,
    BCMPC_TIMESYNC_LT_FLD_SDM_VAL,
    BCMPC_TIMESYNC_LT_FLD_MAX
} bcmpc_port_timesync_lt_fields_t;

/*!
 * \brief timesync configuration structure.
 *
 * This structure is used to configure the port's timesync settings.
 */
typedef struct bcmpc_port_timesync_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, BCMPC_TIMESYNC_LT_FLD_MAX);

    /*! Set to enable IEEE 1588 time stamping. */
    bool     ieee_1588;

    /*! Set to enable one-step time stamping. */
    bool one_step_timestamp;

    /*! Set to enable 1588 SOP (start of packet). */
    bool is_sop;

    /*! Set timesync compensation mode. */
    bcmpc_port_ts_comp_mode_t ts_comp_mode;

    /*! Stage 0 divider value, valid for SYNCE only. */
    bcmpc_port_synce_stg0_mode_t stg0_mode;

    /*! Stage 1 divider value, valid for SYNCE only. */
    bcmpc_port_synce_stg1_mode_t stg1_mode;

    /*! Modulator value, valid for SYNCE only. */
    uint32_t fractional_divisor;

    /*! Operational state of the TIMESYNC entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_port_timesync_t;

/*!
 * \brief preemption configuration profile.
 *
 * This structure is used to configure the preemption settings.
 */
typedef struct bcmpc_preemption_s {

    /*! Enable to indicate preemption verify operation on a port. */
    bool verify_enable;

    /*! Specify the number of verify attampts on a port. */
    uint32_t num_verify_atts;

    /*! Specify the verify operation timeout value on a port. */
    uint32_t verify_timeout;

    /*! Set the value of RX non-final fragment size a port. */
    uint32_t rx_non_final_frag_size;

    /*! Set the value of RX final fragment size a port. */
    uint32_t rx_final_frag_size;

} bcmpc_preemption_t;

/*!
 * Structure definition to store the last LT operation
 * status (on failure) on a logical port.
 */
typedef pm_oper_status_t bcmpc_port_oper_status_t;

/*!
 * Structure definition to store the entry for
 * last LT operation status (on failure) on a
 * logical port.
 */
typedef struct bcmpc_port_oper_status_entry_s {

    /*! Physical port. */
    bcmpc_pport_t pport;

    /*! Operation status for the PM driver call. */
    bcmpc_port_oper_status_t oper_status;

} bcmpc_port_oper_status_entry_t;

/*!
 * pm thread define
 */
extern bcmpc_thread_ctrl_t *bcmpc_pm_thread_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Initalize port configuration data structure.
 *
 * \param [in] cfg Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_port_cfg_t_init(bcmpc_port_cfg_t *cfg);

/*!
 * \brief Initialize the non-HA cache data.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set for warm boot, otherwise for cold boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_cache_init(int unit, bool warm);

/*!
 * \brief Add a logical port.
 *
 * Add a logical port with the given port configuration.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_port_cfg_insert(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Delete a logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_cfg_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Update logical port configuration.
 *
 * Update the port configuration of the given logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_cfg_update(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Add a logical to physical port mapping.
 *
 * Add a logical to physical port mapping.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_port_map_insert(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Delete a logical to physical port mapping.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_map_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Update logical to physical port mapping.
 *
 * Update the associated physical port of the given logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_map_update(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Check the physical ports are free.
 *
 * Check the continuous \c pcnt physical ports from \c pport are free, i.e.
 * those physical ports are not occupied by any logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The base physical port number.
 * \param [out] pcnt The count of physical ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Some or all of the ports are in use.
 */
extern int
bcmpc_pport_free_check(int unit, bcmpc_pport_t pport, size_t pcnt);

/*!
 * \brief Deactivate a range of ports.
 *
 * Deactivate the logical ports which are using the physical ports in the given
 * range.
 *
 * \param [in] unit Unit number.
 * \param [in] pstart Starting physical port.
 * \param [in] pcnt Number of the physical ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_ports_deactivate(int unit, bcmpc_pport_t pstart, int pcnt);

/*!
 * \brief Activate a range of ports.
 *
 * Activate the logical ports which are using the physical ports in the given
 * range when the port configurations are vaild based on the current PM mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pstart Starting physical port.
 * \param [in] pcnt Number of the physical ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_ports_activate(int unit, bcmpc_pport_t pstart, int pcnt);

/*!
 * \brief Initalize auto-negotiation profile data structure.
 *
 * \param [in] prof Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_autoneg_prof_t_init(bcmpc_autoneg_prof_t *prof);

/*!
 * \brief Update the auto-negotiation profile.
 *
 * This function is called when we INSERT or UPDATE a auto-negotiation profile.
 *
 * When a profile is updated, we have to update the hardware for all ports
 * that are actively using the profile.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] prof_id auto-negotiation profile ID.
 * \param [in] prof auto-negotiation profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_autoneg_prof_internal_update(int unit, int prof_id,
                                   bcmpc_autoneg_prof_t *prof);

/*!
 * \brief Initalize port timesync data structure.
 *
 * \param [in] config Data to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_port_timesync_t_init(bcmpc_port_timesync_t *config);

/*!
 * \brief Insert the timesync configuration.
 *
 * This function is called when we INSERT PC_PORT_TIMESYNC table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] config Timesync configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failuser.
 */
extern int
bcmpc_internal_port_timesync_insert(int unit, bcmpc_lport_t lport,
                                    bcmpc_port_timesync_t *config);

/*!
 * \brief Update the timesync configuration.
 *
 * This function is called when we UPDATE a PC_PORT_TIMESYNC table.
 *
 * When a table is updated, we have to update the hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] config Timesync configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_port_timesync_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_port_timesync_t *config);

/*!
 * \brief Delete a timesync configuration for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] config Default timesync configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_port_timesync_delete(int unit, bcmpc_lport_t lport,
                                    bcmpc_port_timesync_t *config);

/*!
 * \brief inital setup of the port scan thread.
 *
 * This function is called during TOPO.
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_thread_init(int unit);

/*!
 * \brief Function to enable the port macro polling thread.
 *
 * \param [in] unit Unit number.
 * \param [in] enable thread enable/disable
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_thread_enable(int unit, bool enable);

/*!
 * \brief de-inital setup of the port scan thread.
 *
 * This function is called during system shut down.
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_thread_deinit(int unit);

/*!
 * \brief Function to config if to disable the pc thread.
 *
 * \param [in] unit Unit number.
 * \param [in] disable if to disable PM thread.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_thread_disable_config(int unit, bool disable);

/*!
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PMD_FIRMWARE LT
 * with the default values for the port based on the speed,
 * num-lanes and fec configuration in PC_PORT LT.
 * The logical to physical port map must be available when
 * calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 * \param [in] null_entry Prepopulate with null entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pmd_firmware_config_prepopulate(int unit, bcmpc_lport_t lport,
                                      bcmpc_port_cfg_t *pcfg, bool null_entry);

/*!
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_MAC_CONTROL LT
 * with the default values for the port based on the speed,
 * num-lanes and fec configuration in PC_PORT LT.
 * The logical to physical port map must be available when
 * calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 * \param [in] null_entry Prepopulate with null entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_mac_control_config_prepopulate(int unit, bcmpc_lport_t lport,
                                     bcmpc_port_cfg_t *pcfg, bool null_entry);

/*!
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_TX_TAPS LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 * \param [in] null_entry Prepopulate with null entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_tx_taps_config_prepopulate(int unit, bcmpc_lport_t lport,
                                 bcmpc_port_cfg_t *pcfg, bool null_entry);

/*!
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PHY_CONTROL LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 * \param [in] null_entry Prepopulate with null entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_phy_control_config_prepopulate(int unit, bcmpc_lport_t lport,
                                     bcmpc_port_cfg_t *pcfg, bool null_entry);

/*!
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PFC LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 * \param [in] null_entry Prepopulate with null entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pfc_config_prepopulate(int unit, bcmpc_lport_t lport,
                             bcmpc_port_cfg_t *pcfg, bool null_entry);

/*!
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_PORT_TIMESYNC LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 * \param [in] null_entry Prepopulate with null entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_timesync_config_prepopulate(int unit, bcmpc_lport_t lport,
                                  bcmpc_port_cfg_t *pcfg, bool null_entry);

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated
 * PC_PMD_FIRMWARE LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pmd_firmware_config_delete(int unit, bcmpc_lport_t lport);

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated
 * PC_MAC_CONTROL LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_mac_control_config_delete(int unit, bcmpc_lport_t lport);

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated
 * PC_TX_TAPS LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_tx_taps_config_delete(int unit, bcmpc_lport_t lport);

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated
 * PC_PY_CONTROL LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_phy_control_config_delete(int unit, bcmpc_lport_t lport);

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated PC_PFC LT
 * entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pfc_config_delete(int unit, bcmpc_lport_t lport);

/*!
 * This function should be invoked when a logical port is
 * deleted from the system to delete the associated PC_PFC LT
 * PC_PORT_TIMESYNC LT entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_timesync_config_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Create rw lock for accessing the l2p_map buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_l2p_lock_init(int unit);

/*!
 * \brief Destroy rw lock for accessing the l2p_map buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_l2p_lock_cleanup(int unit);

/*!
 * \brief Allocate memory for logical to physical port.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_l2p_map_create(int unit);

/*!
 * \brief Destroy l2p_map buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval Nothing.
 */
void
bcmpc_l2p_map_destroy(int unit);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the TX_TAPS LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_tx_taps_prepopulate_all(int unit);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_MAC_CONTROL LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_mac_control_prepopulate_all(int unit);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PFC LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pfc_prepopulate_all(int unit);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PORT_TIMESYNC LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_port_timesync_prepopulate_all(int unit);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PMD_FIRMWARE LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pmd_firmware_prepopulate_all(int unit);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PHY_CONTROL LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_phy_control_prepopulate_all(int unit);

/*!
 * This function should be invoked when the PC_PORT_PHYS_MAP
 * entry is added/updated to the system to update th
 * COPERATIONAL_STATE of the PC_PORT LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_port_lt_entry_update(int unit, bcmpc_lport_t lport);

/*!
 * This function is invoked during the pre_config
 * stage to pre-populate all the PC_PM_CORE LT entries
 * with null entries.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pm_core_prepopulate_all(int unit);

/*!
 * \brief Add a FDR configuration for a logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] fdr_cfg Logical port FDR config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_port_fdr_cfg_insert(int unit, bcmpc_lport_t lport,
                                   bcmpc_fdr_port_control_t *fdr_cfg);

/*!
 * \brief Delete FDR configuration on a logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_fdr_cfg_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Update logical port FDR configuration.
 *
 * Update the port configuration of the given logical port.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] fdr_cfg Logical port FDR config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_fdr_cfg_update(int unit, bcmpc_lport_t lport,
                                   bcmpc_fdr_port_control_t *fdr_cfg);

/*!
 * \brief Get FDR statistics.
 *
 * This function retrieves the FDR statistics from the hardware
 * and updates the FDR_STATUS LT with the values. This will
 * overwrite the existing values in the LT. It is the applications
 * responsibility to read the FDR status periodically when enabled
 * or notified.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] fdr_intr 1 - Indicates this the fdr_stats update
 *                          is due to FDR interrupt.
 *                      0 - Indciates a FDR_STATUS lookup operations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_port_fdr_stats_update(int unit, bcmpc_lport_t lport,
                                     bool fdr_intr);

/*!
 * \brief Update the operational state of MAC_CONTROL LT>
 *
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_MAC_CONTROL LT
 * entry OPERATIONAL_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] oper_state Operational state of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmpc_mac_control_oper_state_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_pport_t pport,
                                    bcmpc_entry_state_t oper_state);

/*!
 * \brief Update the operational state of PMD_FIRMWARE LT>
 *
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PMD_FIRMWARE LT
 * entry OPERATIONAL_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] oper_state Operational state of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmpc_pmd_firmware_oper_state_update(int unit, bcmpc_lport_t lport,
                                     bcmpc_pport_t pport,
                                     bcmpc_entry_state_t oper_state);

/*!
 * \brief Update the operational state of PC_PFC LT>
 *
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PFC LT
 * entry OPERATIONAL_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] oper_state Operational state of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmpc_pfc_oper_state_update(int unit, bcmpc_lport_t lport,
                            bcmpc_pport_t pport,
                            bcmpc_entry_state_t oper_state);

/*!
 * \brief Update the operational state of PC_PORT_TIMESYNC LT>
 *
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PORT_TIMESYNC LT
 * entry OPERATIONAL_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] oper_state Operational state of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmpc_timesync_oper_state_update(int unit, bcmpc_lport_t lport,
                                 bcmpc_pport_t pport,
                                 bcmpc_entry_state_t oper_state);

/*!
 * \brief Update the operational state of TX_TAPS LT.
 *
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_TX_TAPS LT
 * entry OPERATIONAL_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] num_lanes Number of lanes of the logical port.
 * \param [in] oper_state Operational state of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmpc_tx_taps_oper_state_update(int unit, bcmpc_lport_t lport,
                                bcmpc_pport_t pport, int num_lanes,
                                bcmpc_entry_state_t oper_state);

/*!
 * \brief Update the operational state of PHY_CONTROL LT.
 *
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_PHY_CONTROL LT
 * entry OPERATIONAL_STATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] num_lanes Number of lanes of the logical port.
 * \param [in] oper_state Operational state of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmpc_phy_control_oper_state_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_pport_t pport, int num_lanes,
                                    bcmpc_entry_state_t oper_state);

/*!
 * \brief Update the operational state of PC_PORT_MONITOR LT.
 *
 * This function is invoked during the pre_config
 * stage to pre-populate the PC_PORT_MONITORt LT entry.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_port_monitor_prepopulate(int unit);

#endif /* BCMPC_LPORT_INTERNAL_H */
