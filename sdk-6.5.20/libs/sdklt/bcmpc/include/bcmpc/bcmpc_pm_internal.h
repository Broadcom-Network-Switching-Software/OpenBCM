/*! \file bcmpc_pm_internal.h
 *
 * BCMPC PM Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_INTERNAL_H
#define BCMPC_PM_INTERNAL_H

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv_types.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_lt_cfg.h>


/*******************************************************************************
 * Typedefs
 */

/*!
 * \name Lane Configuration Encoding
 *
 * Some lane configurations are encoded together for a core, such as tx/rx
 * swap and polarity swap setting.
 */

/*! \{ */

/*!
 * \brief Get the configuration value of the given lane.
 *
 * This macro is used to get the specific lane configuration from the whole core
 * setting.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 * \param [in] _bits The bits of data per lane.
 *
 * \return The lane configuration value for \c _lane.
 */
#define BCMPC_LANE_CFG_GET(_core, _lane, _bits) \
            ((_core >> (_lane * _bits)) & ((_bits << 1) - 1))

/*!
 * \brief Set the configuration value of the given lane.
 *
 * This macro is used to set the specific lane configuration to the whole core
 * setting.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 * \param [in] _bits The bits of data per lane.
 * \param [in] _val The value of the lane configuration to set.
 *
 * \return The lane configuration value for \c _lane.
 */
#define BCMPC_LANE_CFG_SET(_core, _lane, _bits, _val) \
            (_core |= ((_val & ((_bits << 1) - 1)) << (_lane * _bits)))

/*! \} */

/*!
 * \name Lane Map Encoding
 *
 * Macro for lane map encoding/decoding.
 */

/*! \{ */

/*!
 * \brief The bits count for eahc lane in tx/rx lane map,
 * \ref bcmpc_pm_core_cfg_t.
 */
#define BCMPC_LANE_MAP_BITS_PER_LANE 4

/*!
 * \brief Get the lane map value of the given lane.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 *
 * \return The lane map value for \c _lane.
 */
#define BCMPC_LANE_MAP_GET(_core, _lane) \
            BCMPC_LANE_CFG_GET(_core, _lane, BCMPC_LANE_MAP_BITS_PER_LANE)

/*!
 * \brief Set the lane map value of the given lane.
 *
 * \param [in] _core Core configuration value
 * \param [in] _lane Lane number/index.
 * \param [in] _val The value of the lane map to set.
 */
#define BCMPC_LANE_MAP_SET(_core, _lane, _val) \
            BCMPC_LANE_CFG_SET(_core, _lane, BCMPC_LANE_MAP_BITS_PER_LANE, _val)

/*! \} */


/*!
 * \brief Port macro core identity.
 *
 * This structure is used to specifiy a port macro (PM) core.
 *
 * The PM core is constructed by the PM ID, the core index within the PM.
 */
typedef struct bcmpc_pm_core_s {

    /*! Port macro ID. */
    int pm_id;

    /*! Core index within port macro. */
    int core_idx;

} bcmpc_pm_core_t;

/*!
 * \brief Logical PM port resource.
 *
 * The structure describes the resource of a logical PM port which is derived by
 * lookup PM mode table.
 */
typedef struct bcmpc_pm_lport_rsrc_s {

    /*! PM core location. */
    bcmpc_pm_core_t pm_core;

    /*! Lane bitmap of the logical PM port. */
    uint32_t lbmp;

    /*! Max speed. */
    uint32_t max_speed;

    /*! Operating mode */
    bcmpc_pm_opmode_t op_mode;

} bcmpc_pm_lport_rsrc_t;

/*!
 * PC_PM_CORE LT fields enumeration.
 */
typedef enum bcmpc_pm_core_lt_flds_e {
    BCMPC_PM_CORE_LT_FLD_TX_LANE_MAP_AUTO = 0,
    BCMPC_PM_CORE_LT_FLD_TX_LANE_MAP,
    BCMPC_PM_CORE_LT_FLD_RX_LANE_MAP_AUTO,
    BCMPC_PM_CORE_LT_FLD_RX_LANE_MAP,
    BCMPC_PM_CORE_LT_FLD_TX_POLARITY_FLIP_AUTO,
    BCMPC_PM_CORE_LT_FLD_TX_POLARITY_FLIP,
    BCMPC_PM_CORE_LT_FLD_RX_POLARITY_FLIP_AUTO,
    BCMPC_PM_CORE_LT_FLD_RX_POLARITY_FLIP,
    BCMPC_PM_CORE_LT_FLD_PMD_CLOCK,
    BCMPC_PM_CORE_LT_FLD_PM_MODE
}bcmpc_pm_core_lt_flds_t;

/*! Maximum number of LT fields in PC_PM_CORE. */
#define PC_PM_CORE_FIELD_ID_MAX    10

/*!
 * \brief Port macro core configuration.
 *
 * This structure is used to configure the board-related properties of a
 * port macro core.
 */
typedef struct bcmpc_pm_core_cfg_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_PM_CORE_FIELD_ID_MAX);

    /*! TX lane map configuration is calculated internally when set. */
    bool  tx_lane_map_auto;

    /*! Bits[3:0] is lane 0 mapping, bits[7:4] is lane 1 mapping, etc. */
    uint64_t tx_lane_map;

    /*!
     * Bits[3:0] is lane 0 mapping, bits[7:4] is lane 1 mapping, etc.
     * Operational value of the TX lane map setting in the hardware.
     */
    uint64_t tx_lane_map_oper;

    /*! RX lane map configuration is calculated internally when set. */
    bool  rx_lane_map_auto;

    /*! Bits[3:0] is lane 0 mapping, bits[7:4] is lane 1 mapping, etc. */
    uint64_t rx_lane_map;

    /*!
     * Bits[3:0] is lane 0 mapping, bits[7:4] is lane 1 mapping, etc.
     * Operational value of the RX lane map setting in the hardware.
     */
    uint64_t rx_lane_map_oper;

    /*! TX polarity flip configuration is calculated internally when set. */
    bool  tx_polarity_flip_auto;

    /*! Bit 0 flips lane 0, bit 1 flips lane 1, etc. */
    uint16_t tx_polarity_flip;

    /*!
     * Bit 0 flips lane 0, bit 1 flips lane 1, etc.
     * Operation value of the TX polarity flip setting in the hardware.
     */
    uint16_t tx_polarity_flip_oper;

    /*! RX polarity flip configuration is calculated internally when set. */
    bool  rx_polarity_flip_auto;

    /*! Bit 0 flips lane 0, bit 1 flips lane 1, etc. */
    uint16_t rx_polarity_flip;

    /*!
     * Bit 0 flips lane 0, bit 1 flips lane 1, etc.
     * Operation value of the RX polarity flip setting in the hardware.
     */
    uint16_t rx_polarity_flip_oper;

    /*! Digital common clock that PMD uses. */
    uint32_t pmd_com_clk;

    /*! Port macro firmware download config. */
    bcmpc_pm_fw_cfg_t pm_fw_cfg;

    /*! Port macro operation mode - unsed for present. */
    bcmpc_pm_opmode_t pm_mode;

    /*! Operational state of the PC_PM_CORE entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_pm_core_cfg_t;

/*!
 * \brief Port macro lane identity.
 *
 * This structure is used to specifiy a port macro (PM) lane.
 *
 * The PM lane is constructed by the PM ID, the core index within the PM and the
 * lane number within the core.
 */
typedef struct bcmpc_pm_lane_s {

    /*! Port macro ID. */
    int pm_id;

    /*! Core index within port macro. */
    int core_idx;

    /*! Lane number within core. */
    int core_lane;

} bcmpc_pm_lane_t;

/*!
 * \brief Port abilities identity.
 *
 * This structure is used to specifiy port abilities on a given logical port.
 */
typedef struct bcmpc_pm_port_ability_s {

    /*! Logical port ID. */
    bcmpc_lport_t lport;

    /*! Type of port abilities. */
    pm_port_ability_type_t ability_type;

} bcmpc_pm_port_ability_t;

/*!
 * \brief Port medium type.
 */
typedef pm_port_medium_t bcmpc_phy_medium_t;

/*! Medium type backplane (KR). */
#define BCMPC_PHY_MEDIUM_BACKPLANE  PM_PORT_MEDIUM_BACKPLANE

/*! Medium type copper (CR). */
#define BCMPC_PHY_MEDIUM_COPPER     PM_PORT_MEDIUM_COPPER

/*! Medium type optical. */
#define BCMPC_PHY_MEDIUM_OPTICAL    PM_PORT_MEDIUM_FIBER

/*! All medium type. */
#define BCMPC_PHY_MEDIUM_ALL        PM_PORT_MEDIUM_ALL

/*!
 * \brief PC_PMD_FIRMWARE LT fields information.
 */
typedef enum bcmpc_pmd_firmware_lt_fields_e {
    BCMPC_PMD_FW_LT_FLD_DFE = 0,
    BCMPC_PMD_FW_LT_FLD_LP_DFE,
    BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE,
    BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN,
    BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON,
    BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS,
    BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE,
    BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4,
    BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4,
    BCMPC_PMD_FW_LT_FLD_DFE_AUTO,
    BCMPC_PMD_FW_LT_FLD_LP_DFE_AUTO,
    BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE_AUTO,
    BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN_AUTO,
    BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON_AUTO,
    BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS_AUTO,
    BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE_AUTO,
    BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4_AUTO,
    BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4_AUTO
} bcmpc_pmd_firmware_lt_fields;

/*! Maximum field ID of PC_PMD_FIRMWARE LTs. */
#define PC_PMD_FIRMWARE_FIELD_ID_MAX        18

/*!
 * \brief PMD firmware configuratione.
 *
 * Shareable pmd lane configuration for receiver.
 *
 * These settings are normally controlled by the SerDes firmware.
 */
typedef struct bcmpc_pmd_firmware_config_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_PMD_FIRMWARE_FIELD_ID_MAX);

    /*! Auto configure the DFE setting. */
    bool dfe_auto;

    /*! Set to enable DFE. */
    bool dfe;

    /*! Auto configure the low-powerDFE setting. */
    bool lp_dfe_auto;

    /*! Set to enable low power DFE. */
    bool lp_dfe;

    /*! Auto configure the medium type of the port. */
    bool medium_auto;

    /*! Set medium type. */
    bcmpc_phy_medium_t medium;

    /*!  Auto configure CL72 restart timeout setting. */
    bool cl72_res_to_auto;

    /*!
     * Enable a pmd rx restart after 600ms if the link has
     * failed to complete training.
     */
    bool cl72_res_to;

    /*! Auto configure the TX precoder on a port. */
    bool lp_tx_precoder_on_auto;

    /*! Link partner TX has precoder enabled.  */
    bool lp_tx_precoder_on;

    /*! Auto configure the unreliable LOS setting on a port. */
    bool unreliable_los_auto;

    /*!
     * Loss of signal (LOS) cannot be trusted when unreliable_los == TRUE
     * This is meant for optical modules with non-functional or missing optical
     * LOS.
     */
    bool unreliable_los;

    /*! Auto configure the scrambling enable setting on a port. */
    bool scrambling_en_auto;

    /*!
     * Set to enable scrambling a port
     * This is supported on lower speeds (1G, 2.5G, 10G).
     */
    bool scrambling_en;

    /*! Auto configure the PAM4 forced normal reach mode. */
    bool norm_reach_pam4_auto;

    /*! PAM4 forced normal reach mode. */
    uint32_t norm_reach_pam4_bmask;

    /*! PAM4 forced extended reach mode. */
    bool ext_reach_pam4_auto;

    /*! Auto configure the PAM4 forced extended reach mode. */
    uint32_t ext_reach_pam4_bmask;

    /*! Operational state of the PMD FW entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_pmd_firmware_config_t;

/*!
 * \brief Logical PM port information.
 *
 * The structure describes the a logical PM port information
 * generated from PC_PORTt and PC_PHYS_PORTt
 */
typedef struct bcmpc_pm_lport_info_s {

    /*! Lane bitmap of the logical PM port. */
    uint32_t lbmp;

    /*! port speed. */
    uint32_t max_speed;

} bcmpc_pm_lport_info_t;

/*!
 * \brief PC_STALL_TX_STATUS LT enumeration.
 */
typedef enum bcmpc_stall_tx_status_e {
    BCMPC_STALL_TX_DISABLE = 0,
    BCMPC_STALL_TX_ENABLE,
    BCMPC_STALL_TX_NO_SUPPORT
} bcmpc_stall_tx_status_t;

/*!
 * \brief PC_MAC_CONTROL LT RSV (Receive Statistic Vector)
 * control fields information.
 */
typedef enum bcmpc_mac_rsv_fields_e {
    BCMPC_MAC_RSV_WRONG_SA_PKTS = 0,
    BCMPC_MAC_RSV_STACK_VLAN_PKTS = 1,
    BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS = 2,
    BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS = 3,
    BCMPC_MAC_RSV_CRC_ERROR_PKTS = 4,
    BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS = 5,
    BCMPC_MAC_RSV_TRUNCATED_PKTS = 6,
    BCMPC_MAC_RSV_GOOD_PKTS = 7,
    BCMPC_MAC_RSV_MULTICAST_PKTS = 8,
    BCMPC_MAC_RSV_BROADCAST_PKTS = 9,
    BCMPC_MAC_RSV_PROMISCUOUS_PKTS = 10,
    BCMPC_MAC_RSV_CONTROL_PKTS = 11,
    BCMPC_MAC_RSV_PAUSE_PKTS = 12,
    BCMPC_MAC_RSV_BAD_OPCODE_PKTS = 13,
    BCMPC_MAC_RSV_VLAN_TAGGED_PKTS = 14,
    BCMPC_MAC_RSV_UNICAST_PKTS = 15,
    BCMPC_MAC_RSV_RX_FIFO_FULL = 16,
    BCMPC_MAC_RSV_RUNT_PKTS = 17,
    BCMPC_MAC_RSV_PFC_PKTS = 18,
    BCMPC_MAC_RSV_SCH_CRC_ERROR = 19,
    BCMPC_MAC_RSV_MACSEC_PKTS = 20,
    BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR = 21
} bcmpc_mac_rsv_fields_t;

/*! Maximum field ID of PC_MAC_CONTROL RSV control fields LTs. */
#define PC_MAC_RSV_FIELD_ID_MAX        22

/*!
 * \brief PC_MAC_CONTROL LT fields information.
 */
typedef enum bcmpc_mac_ctrl_lt_fields_e {
    BCMPC_MAC_CTRL_LT_FLD_OVERSIZE_PKT = 0,
    BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE_AUTO,
    BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE,
    BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE_AUTO,
    BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE,
    BCMPC_MAC_CTRL_LT_FLD_LOCAL_FAULT_DISABLE,
    BCMPC_MAC_CTRL_LT_FLD_REMOTE_FAULT_DISABLE,
    BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP_AUTO,
    BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP,
    BCMPC_MAC_CTRL_LT_FLD_PAUSE_TX,
    BCMPC_MAC_CTRL_LT_FLD_PAUSE_RX,
    BCMPC_MAC_CTRL_LT_FLD_PAUSE_ADDR,
    BCMPC_MAC_CTRL_LT_FLD_PAUSE_PASS,
    BCMPC_MAC_CTRL_LT_FLD_CONTROL_PASS,
    BCMPC_MAC_CTRL_LT_FLD_MAC_ECC_INTR_ENABLE,
    BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD_AUTO,
    BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD,
    BCMPC_MAC_CTRL_LT_FLD_STALL_TX,
    BCMPC_MAC_CTRL_LT_FLD_MAC_RSV_CONTROL
} bcmpc_mac_ctrl_lt_fields;

/*! Maximum field ID of PC_PORT LTs. */
#define PC_MAC_CTRL_FIELD_ID_MAX        19

/*!
 * \brief mac control configuration.
 *
 * This structure is used to configure the mac control settings.
 */
typedef struct bcmpc_mac_control_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_MAC_CTRL_FIELD_ID_MAX);

    /*! Bitmap of MAC RSV fields to be operated */
    SHR_BITDCLNAME(rsv_fbmp, PC_MAC_RSV_FIELD_ID_MAX);

    /*! Set oversize packet size. */
    uint32_t oversize_pkt;

    /*! Auto configure the MAC RX enable. */
    bool rx_enable_auto;

    /*! Enable RX on the port. */
    bool rx_enable;

    /*! Auto configure the MAC TX enable. */
    bool tx_enable_auto;

    /*! Enable TX on the port. */
    bool tx_enable;

    /*! Enable to support hardware lag failove. */
    bool  hw_lag_failover;

    /*! Disable local fault. */
    bool local_fault_dis;

    /*! Disable remote fault. */
    bool rmt_fault_dis;

    /*! Auto configure inter-frame gap (IFG) in bits. */
    uint8_t ifg_auto;

    /*! Specify inter-frame gap (IFG) in bits. */
    uint8_t ifg;

    /*! Pause MAC address. */
    bcmpc_mac_t pause_addr;

    /*! Tx pause ability. */
    int pause_tx;

    /*! Rx pause ability. */
    int pause_rx;

    /*!
     * Pass the PAUSE frames into the switch device.
     * Do not drop the frames at MAC.
     */
    int pause_pass;

    /*!
     * Pass all the control frames (including the PFC frames
     * with ethertype 0x8808) except PAUSE frames into the
     * switch device.
     */
    int control_pass;

    /*! Auto configure the Runt threshold value. */
    int runt_threshold_auto;

    /*! Runt threshold value. */
    int runt_threshold;

    /*! Enable ECC parity interrupt. */
    bool mac_ecc_intr;

    /*! Operational state of stall tx feature. */
    bcmpc_stall_tx_status_t stall_tx_status;

    /*! Stall TX - transmission disabled on the port. */
    bool stall_tx;

    /*! Set MAC RSV (Receive Statistic Vector) event control. */
    uint32_t mac_rsv_ctrl;

    /*! Operational state of the MAC entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_mac_control_t;

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
} bcmpc_phy_control_lt_fields_t;

/*! Maximum field ID of PC_PHY_CONTROL LT. */
#define PC_PHY_CTRL_FIELD_ID_MAX     28

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

    /*! Operational state of the PHY control entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_phy_control_t;

/*!
 * \brief PC_PFC LT fields information.
 */
typedef enum bcmpc_pfc_lt_fields_e {
    BCMPC_PFC_LT_FLD_ENABLE_RX = 0,
    BCMPC_PFC_LT_FLD_ENABLE_TX,
    BCMPC_PFC_LT_FLD_ENABLE_STATS,
    BCMPC_PFC_LT_FLD_REFRESH_TIMER,
    BCMPC_PFC_LT_FLD_DEST_ADDR,
    BCMPC_PFC_LT_FLD_ETH_TYPE,
    BCMPC_PFC_LT_FLD_OPCODE,
    BCMPC_PFC_LT_FLD_XOFF_TIMER,
    BCMPC_PFC_LT_FLD_PFC_PASS
} bcmpc_pfc_lt_fields_t;

/*! Maximum field ID of PC_PFC LT. */
#define PC_PFC_FIELD_ID_MAX        9

/*!
 * \brief Priority-based flow control (PFC) configuration.
 *
 * This structure is used to configure the PFC settings.
 */
typedef struct bcmpc_pfc_control_s {

    /*
     * PFC behavior control.
     */

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_PFC_FIELD_ID_MAX);


    /*! Enable PFC transmit. */
    bool tx_enable;

    /*! Enable PFC receive. */
    bool rx_enable;

    /*! Enable PFC counters. */
    bool stats_enable;

    /*!
     * Threshold for pause timer to cause XOFF to be resent. Set to 0 to
     * disable this feature.
     */
    uint32_t refresh_timer;

   /*
    * PFC frame configure.
    */

    /*! Destination MAC address to use for the PFC frame. */
    bcmpc_mac_t da;

    /*! EtherType to use for PFC frame. */
    uint32_t eth_type;

    /*! Opcode to use for PFC frame. */
    uint32_t opcode;

    /*! Time value sent in the Timer Field for classes in XOFF state. */
    uint32_t xoff_timer;

    /*! Enable to allow PFC packets into switch device. */
    uint32_t pfc_pass;

    /*! Operational state of the PFC entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_pfc_control_t;


/*******************************************************************************
 * Function declarations
 */

/*!
 * \brief Initalize PM mode data structure.
 *
 * \param [in] mode Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_mode_t_init(bcmpc_pm_mode_t *mode);

/*!
 * \brief Initalize PM core configuration structure.
 *
 * \param [in] ccfg Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_core_cfg_t_init(bcmpc_pm_core_cfg_t *ccfg);

/*!
 * \brief Initalize logical PM port resource data structure.
 *
 * \param [in] prsrc Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_lport_rsrc_t_init(bcmpc_pm_lport_rsrc_t *prsrc);

/*!
 * \brief Get the logical PM port resource by a physical port.
 *
 * This function would lookup topology database to get the logical PM port
 * resource by the physical port number of a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The physical port number of a logical port.
 * \param [out] prsrc Logical PM port resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Failed to get the logical PM port resource with \c pport.
 */
extern int
bcmpc_pm_lport_rsrc_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pm_lport_rsrc_t *prsrc);

/*!
 * \brief Get the core resource by a physical port.
 *
 * The core resource is derived from topology database and the current PM mode.
 *
 * For non-aggregated PM or run an aggregated PM in aggregation mode, the output
 * \c core_info is same as the output of \ref bcmpc_pm_info_get().
 *
 * When run an aggregated PM as several independent sub-PMs, this function will
 * return the specific sub-PM info, \c core_info, where the given pport belongs
 * to.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The physical port number of a logical port.
 * \param [out] core_info PM core resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Failed to get the logical PM port resource with \c pport.
 */
extern int
bcmpc_pm_core_rsrc_get(int unit, bcmpc_pport_t pport,
                       bcmpc_pm_info_t *core_info);

/*!
 * \brief Set the PM mode configuration.
 *
 * Set the PM mode configuration to the given PM.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [in] mode PM mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_pm_mode_set(int unit, int pm_id, bcmpc_pm_mode_t *mode);

/*!
 * \brief Delete the PM mode configuration.
 *
 * Delete the PM mode configuration of the given PM.
 *
 * This function will only do the internal state and/or hardware change, but
 * will not update the software database.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_internal_pm_mode_delete(int unit, int pm_id);


/*!
 * \brief Staging the PM mode configuration.
 *
 * Stage the new PM configurations to make the PC internal sub-components
 * know we are currently updating the PM mode and be able to get the
 * the port attribues e.g. the number of lanes and max speed accroding to
 * the new PM mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [in] pm_mode PM mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_mode_stage_set(int unit, int pm_id, bcmpc_pm_mode_t *pm_mode);

/*!
 * \brief Get the staged PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [out] pm_mode PM mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_mode_stage_get(int unit, int pm_id, bcmpc_pm_mode_t **pm_mode);

/*!
 * \brief Delete the staged PM mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pm_mode_stage_delete(int unit, int pm_id);

/*!
 * \brief Initalize PM logical port data structure.
 *
 * \param [in] lp_info Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_lport_info_t_init(bcmpc_pm_lport_info_t *lp_info);

/*!
 * \brief Get the port bitmap of the given port on the PM.
 *
 * This function will return bitmap for the given port on the PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [in] pcfg port configuration.
 * \param [out] lp_info port bitmap on the PM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the port bitmap.
 */
extern int
bcmpc_pm_lport_info_get(int unit,  bcmpc_pport_t pport,
                        bcmpc_port_cfg_t *pcfg,
                        bcmpc_pm_lport_info_t *lp_info);

/*!
 * \brief Initalize PM mac configuration structure.
 *
 * \param [in] mac_ctrl Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_mac_control_t_init(bcmpc_mac_control_t *mac_ctrl);

/*!
 * \brief Insert Mac configuration.
 *
 * This function is called when we INSERT mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] mac_cfg Mac configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_mac_control_insert(int unit, bcmpc_lport_t lport,
                                  bcmpc_mac_control_t *mac_cfg);

/*!
 * \brief Update Mac configuration.
 *
 * This function is called when we UPDATE mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] mac_cfg Mac configurations.
 * \param [in] default_cfg Default MAC Configuration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_mac_control_update(int unit, bcmpc_lport_t lport,
                                  bcmpc_mac_control_t *mac_cfg,
                                  bcmpc_mac_control_t default_cfg);

/*!
 * \brief Initalize PM phy configuration structure.
 *
 * \param [in] phy_ctrl Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_phy_control_t_init(bcmpc_phy_control_t *phy_ctrl);

/*!
 * \brief Insert Phy configuration.
 *
 * This function is called when we INSERT mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane Combination of port and lane.
 * \param [in] phy_cfg Phy configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_phy_control_insert(int unit, bcmpc_port_lane_t *port_lane,
                                  bcmpc_phy_control_t *phy_cfg);

/*!
 * \brief Update Phy configuration.
 *
 * This function is called when we UPDATE mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane Combinaton of port and lane.
 * \param [in] phy_cfg Phy configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_phy_control_update(int unit, bcmpc_port_lane_t *port_lane,
                                  bcmpc_phy_control_t *phy_cfg);

/*!
 * \brief Initalize PMD firmware configuration structure.
 *
 * \param [in] fm_ctrl Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmd_firmware_config_t_init(bcmpc_pmd_firmware_config_t *fm_ctrl);

/*!
 * \brief Insert pmd firmware configuration.
 *
 * This function is called when we INSERT mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] fm_cfg PMD firmware configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_pmd_firmware_config_insert(int unit,
                                          bcmpc_lport_t lport,
                                          bcmpc_pmd_firmware_config_t *fm_cfg);

/*!
 * \brief Update Phy configuration.
 *
 * This function is called when we UPDATE mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] fm_cfg PMD firmware configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_pmd_firmware_config_update(int unit,
                                          bcmpc_lport_t lport,
                                          bcmpc_pmd_firmware_config_t *fm_cfg);

/*!
 * \brief Initalize Tx taps configuration structure.
 *
 * \param [in] taps_ctrl Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_tx_taps_config_t_init(bcmpc_tx_taps_config_t *taps_ctrl);

/*!
 * \brief Insert Tx taps configuration.
 *
 * This function is called when we INSERT mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane Combination of port and lane.
 * \param [in] taps_cfg Tx taps configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_tx_taps_config_insert(int unit,
                                          bcmpc_port_lane_t *port_lane,
                                          bcmpc_tx_taps_config_t *taps_cfg);

/*!
 * \brief Update Tx taps configuration.
 *
 * This function is called when we UPDATE mac configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane Combination of port and lane.
 * \param [in] taps_cfg Tx taps configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_tx_taps_config_update(int unit,
                                          bcmpc_port_lane_t *port_lane,
                                          bcmpc_tx_taps_config_t *taps_cfg);

/*!
 * \brief Initalize PM pfc configuration structure.
 *
 * \param [in] pctrl Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pfc_ctrl_t_init(bcmpc_pfc_control_t *pctrl);

/*!
 * \brief Insert pfc configuration.
 *
 * This function is called when we INSERT pfc configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] pctrl PFC configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_pfc_ctrl_insert(int unit, bcmpc_lport_t lport,
                              bcmpc_pfc_control_t *pctrl);

/*!
 * \brief Update pfc configuration.
 *
 * This function is called when we UPDATE pfc configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] pctrl PFC configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_internal_pfc_ctrl_update(int unit, bcmpc_lport_t lport,
                             bcmpc_pfc_control_t *pctrl);

/*!
 * \brief Get PortMacro TSC Core configuration.
 *
 * This function is called during TSC Core initialization.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_core PM TSC core information.
 * \param [out] core_config PM TSC core configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pm_core_config_get(int unit, bcmpc_pm_core_t pm_core,
                         bcmpc_pm_core_cfg_t *core_config);

/*!
 * \brief Get PortMacro TSC Core configuration from hardware.
 *
 * This function is called during PC_PM_CORE LT lookup.
 * TX/RX polarity flip and TX/RX Lane map settings are
 * returned.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_core PM TSC core information.
 * \param [out] core_config PM TSC core configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pm_hw_core_config_get(int unit, bcmpc_pm_core_t pm_core,
                            bcmpc_pm_core_cfg_t *core_config);

/*!
 * \brief Get the PortMacro power status.
 *
 * This function is called during PC_PM_CORE LT lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_core PM TSC core information.
 * \param [out] is_active 1 if PortMacro is active.
 *                        0 if PortMacro is powered off.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pm_core_is_active(int unit, bcmpc_pm_core_t pm_core, bool *is_active);
#endif /* BCMPC_PM_INTERNAL_H */
