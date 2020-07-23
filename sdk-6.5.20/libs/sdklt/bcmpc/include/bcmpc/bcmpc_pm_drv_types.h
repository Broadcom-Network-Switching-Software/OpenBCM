/*! \file bcmpc_pm_drv_types.h
 *
 * BCMPC PortMacro driver public data types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_TYPES_H
#define BCMPC_PM_DRV_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <phymod/phymod.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpmac/bcmpmac_acc.h>
#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpc/generated/bcmpc_pm_ha_internal.h>

/*! Bitmap of ports of a particular type or properties. */
typedef bcmdrd_pbmp_t pm_pbmp_t;

/*!
 * Maximum number of PMs per switch device.
 * that can be supported. To be only used for
 * HA validations.
 */
#define BCMPC_MAX_NUM_PMS_PER_SWITCH 256

/*! HA Port Link state sub-module ID. */
#define BCMPC_HA_PORT_LINK_STATE_MODULE_ID (BCMPC_MAX_NUM_PMS_PER_SWITCH + (1))


/*! The maximum number of physical ports per PM. */
#define BCMPM_NUM_PORTS_PER_PM_MAX 16

/*!
 * \brief PM physical port number type.
 *
 * The PM physical port number. It means the physical port
 * number within a port macro which is equivalent to the
 * "block-port number" or the "port index".
 */
typedef shr_port_t pm_pport_t;

/*!
 * \brief Port Macro (PM) types supported.
 */
typedef enum pm_type_s {
    /*! Used to indicate Invalid PM type. */
    PM_TYPE_INVALID = 0,

    /*! PM4X10. */
    PM_TYPE_PM4X10,

    /*! PM4X10Q. */
    PM_TYPE_PM4X10Q,

    /*! PM4X10QTC. */
    PM_TYPE_PM4X10QTC,

    /*! PM4X25. */
    PM_TYPE_PM4X25,

    /*! PM8X50. */
    PM_TYPE_PM8X50,

    /*! PM8X50_GEN2. */
    PM_TYPE_PM8X50_GEN2,

    /*! PM8X50_GEN3. */
    PM_TYPE_PM8X50_GEN3,

    /*! PM8X100 */
    PM_TYPE_PM8X100,

    /*! PM8X100_GEN2. */
    PM_TYPE_PM8X100_GEN2,

    /*! Must be the last. */
    PM_TYPE_COUNT

} pm_type_t;

/*! PortMacro version information as stored in the hardware. */
typedef struct pm_version_info_s {

    /*! PortMacro Revision Letter A, B, C, or D. */
    int8_t pm_rev_letter;

    /*! PortMacro Revision number - 0 to 7. */
    int8_t pm_rev_number;

    /*! PortMacro technology process, 0 - 28nm, 1 - 16nm. */
    int8_t pm_tech_process;

    /*! PortMacro type, PM4x10, PM4x25, PM4x10Q, or PM8x50. */
    pm_type_t pm_type;
} pm_version_info_t;

/*!
 * \brief Port Modes for MAC/PHY.
 */
typedef enum pm_core_port_mode_e {
    /*! Quad Port Mode. All four ports are enabled. */
    PM_PORT_MODE_QUAD = 0,

    /*!
     * Tri Port Mode. Ports 0, 1, and 2 are enabled.
     * Port 2 is dual lane.
     */
    PM_PORT_MODE_TRI012 = 1,

    /*!
     * Tri Port Mode. Ports 0, 2, and 3 are enabled.
     * Port 0 is dual lane.
     */
    PM_PORT_MODE_TRI023 = 2,

    /*!
     * Dual Port Mode. Ports 0, and 2 are enabled.
     * Each port is dual lane.
     */
    PM_PORT_MODE_DUAL = 3,

    /*! Single Port Mode. Each port is single lane XLGMII. */
    PM_PORT_MODE_SINGLE = 4

} pm_core_port_mode_t;

/*! PortMacro core mode information. */
typedef struct pm_port_mode_info_s {

    /*! Core Port mode, Quad, Tri, Dual, or Single mode. */
    pm_core_port_mode_t cur_mode;

    /*! lane mask indicating the lanes of the port. */
    int lanes;

    /*! PortMacro Physical port number. */
    int port_index;
} pm_port_mode_info_t;


/*! PortMacro global MAC controls. */
typedef enum pm_mac_control_info_e {
    /*! Reset MAC instance 0 of the PortMacro. */
    PM_CONTROL_MAC0_RESET = 0,

    /*! Reset MAC instance 1 of the PortMacro. */
    PM_CONTROL_MAC1_RESET,

    /*! MAC based Timestamping disable. */
    PM_CONTROL_MAC_TS_DISABLE,

    /*!
     * Enable MAC 16-byte interface mode.
     * Supported on specific MAC.
     */
    PM_CONTROL_MAC_16B_INTERFACE_MODE,

    /*! Must be the last. */
    PM_CONTROL_MAC_COUNT

} pm_mac_control_info_t;

/*! PortMacro TSC PLL lock status information. */
typedef struct pm_tsc_pll_lock_status_s {
    /*! PLL0 unlock, indicates if PLL lock was lost since last read. */
    int8_t pll0_unlock;

    /*! PLL0 live lock status. */
    int8_t pll0_live_lock_status;

    /*! PLL1 unlock, indicates if PLL lock was lost since last read. */
    int8_t pll1_unlock;

    /*! PLL1 live lock status. */
    int8_t pll1_live_lock_status;
} pm_tsc_pll_lock_status_t;

/*!
 * \brief pm_tc_controls flags.
 */
typedef enum pm_tsc_clock_select_pll_index_e {
    /*! Select PLL0 as TSC VCO clock. */
    PM_TSC_CORE_CLOCK_PLL0 = 0,

    /*! Select PLL1 as TSC VCO clock. */
    PM_TSC_CORE_CLOCK_PLL1,


    /*! Must be the last. */
    PM_TSC_CORE_CLOCK_COUNT

} pm_tsc_clock_select_pll_index_t;

/*!
 * \brief TSC clock select flag.
 */
#define PM_TSC_CORE_CLK_SELECT   0x1

/*!
 * \brief TSC Core power down select flag.
 */
#define PM_TSC_CORE_POWER_DOWN   0x2

/*!
 * \brief TSC Core reset select flag.
 */
#define PM_TSC_CORE_RESET        0x4

/*!
 * \brief Structure for TSC core control.
 *
 * This structure is used to select and perform an
 *  operation on the TSC core in a PortMacro.
 */
typedef struct pm_tsc_control_s {
    /*! PM_TSC_CORE_XXX flags. */
    uint32_t flags;

    /*! PLL0 live lock status. */
    pm_tsc_clock_select_pll_index_t vco_select;

    /*! TSC Core power down. */
    int8_t tsc_power_down;

    /*! TSC Core hard reset. */
    int8_t tsc_hard_reset;

} pm_tsc_control_t;

/*! TSC lane 0 status flags. */
#define  PM_TSC_LANE0_STATUS    0x1

/*! TSC lane 1 status flags. */
#define  PM_TSC_LANE1_STATUS    0x2

/*! TSC lane 2 status flags. */
#define  PM_TSC_LANE2_STATUS    0x4

/*! TSC lane 3 status flags. */
#define  PM_TSC_LANE3_STATUS    0x8

/*! TSC lane 4 status flags. */
#define  PM_TSC_LANE4_STATUS    0x10

/*! TSC lane 5 status flags. */
#define  PM_TSC_LANE5_STATUS    0x20

/*! TSC lane 6 status flags. */
#define  PM_TSC_LANE6_STATUS    0x40

/*! TSC lane 7 status flags. */
#define  PM_TSC_LANE7_STATUS    0x80

/*! Structure for TSC lane status. */
typedef struct pm_tsc_lane_status_info_s {

    /*! PM_TSC_LANE_XXX status flags. */
    uint32_t flags;

    /*! TSC lane number, PM_TSC_LANEX_STATUS flags. */
    uint32_t tsc_lane;

    /*! Per-lane status. */
    int status[PHYMOD_MAX_LANES_PER_CORE];

} pm_tsc_lane_status_info_t;

/*! PortMacro Port fault status flags. */

/*! PortMacro Port remote fault status. */
#define PM_PORT_REMOTE_FAULT_STATUS     0x1

/*! PortMacro Port local fault status. */
#define PM_PORT_LOCAL_FAULT_STATUS      0x2

/*!
 * Supported port physical medium types.
 */
typedef enum pm_port_medium_s {

    /*! Medium type backplane (KR). */
    PM_PORT_MEDIUM_BACKPLANE,

    /*! Medium type copper (CR). */
    PM_PORT_MEDIUM_COPPER,

    /*! Medium type optical. */
    PM_PORT_MEDIUM_FIBER,

    /*! ALl medium type. */
    PM_PORT_MEDIUM_ALL,

} pm_port_medium_t;

/*!
 * \brief Speed Config Flags
 */
/*! Do not set FEC configuration. */
#define PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION 0x1

/*! Set request that skip FEC configuration to flags in speed_config. */
#define PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_SET(speed_config) \
           ((speed_config)->flags |= PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION)

/*! Clear request that skip FEC configuration from flags in speed_config. */
#define PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_CLR(speed_config) \
           ((speed_config)->flags &= ~PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION)

/*! Get request that skip FEC configuration from flags in speed_config. */
#define PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_GET(speed_config) \
           ((speed_config)->flags &                                \
                 PM_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION ? 1 : 0)

/*! Port speed configuration structure. */
typedef struct pm_speed_config_s {

    /*! Port data rate in Mbits/sec. */
    uint32_t speed;

    /*! Number of lanes of the port. */
    uint8_t  num_lanes;

    /*! Medium type, eg. backplane, copper, fiber. */
    int medium_type;

    /*! FEC type. */
    pm_port_fec_t fec;

    /*! Enable or disable link training. */
    bool link_training;

    /*! PMD lane configuration. */
    phymod_firmware_lane_config_t lane_config;

    /*! Flags. */
    uint32_t flags;

} pm_speed_config_t;

/*!
 * Port macro driver return reason codes used to indicate
 * operational errors.
 */
typedef enum pm_oper_status_type_e {

    /*! Successful operation. */
    PM_OPER_SUCCESS = 0,

    /*! Invalid port. */
    PM_PORT_INVALID,

    /*! Invalid speed configuration. */
    PM_SPEED_INVALID,

    /*!
     * Number of VCO?s required to support the operation is
     * more than what the hardware supports.
     */
    PM_VCO_UNAVAIL,

    /*! Port deletion failed. */
    PM_PORT_ACTIVE,

    /*!
     * PAM4 encoding not supported with the current
     * primary VCO setting.
     */
    PM_PRIMARY_VCO_CFG_INVALID,

    /*! The lane number is not valid for this port. */
    PM_LANE_MASK_INVALID,

    /*! Unsupported loopback type. */
    PM_LOOPBACK_TYPE_INVALID,

    /*! Port ability configuration not supported. */
    PM_ABILITY_PORT_CFG_INVALID,

    /*! Pause setting conflicts in the port abilities. */
    PM_ABILITY_PAUSE_CONFLICT,

    /*!
     * The current port media type conflicts the media
     * setting in the abilities.
     */
    PM_ABILITY_MEDIA_TYPE_CONFLICT,

    /*! FEC configuration conflicts in the abilities configuration. */
    PM_ABILITY_FEC_CONFLICT,

    /*! Channel configuration conflicts in the ability configuration. */
    PM_ABILITY_CHANNEL_CONFLICT,

    /*! Invalid advertisement configuration. */
    PM_ABILITY_ADVERT_CFG_INVALID,

    /*! Conflict in auto-negotiation configuration in abilities. */
    PM_ABILITY_AN_CONFLICT_CFG,

    /*! Invalid auto-negotiation mode configuration. */
    PM_AN_MODE_INVALID,

    /*! MAC operation error. */
    PM_MAC_OPER_ERROR,

    /*! PHY operation error. */
    PM_PHY_OPER_ERROR,

    /*! Port is in suspended state, no switching. */
    PM_PORT_SUSPENDED

} pm_oper_status_type_t;

/*!
 * Structure definition to store the last LT operation
 * status (on failure) on a logical port.
 */
typedef struct pm_oper_status_s {

    /*! pc_oper_status is valid. */
    bool    valid;

    /*! Operational status of the last operation. */
    pm_oper_status_type_t  status;

    /*! Valid port macro lane mask.  */
    uint32_t         lane_mask;

    /*! Number of valid elements in port_list. */
    uint8_t          num_port;

    /*! List of ports, valid for only a subset of operational status. */
    pm_pport_t        port_list[BCMPM_NUM_PORTS_PER_PM_MAX];

} pm_oper_status_t;


/*! Port macro MAC and PHY access structure. */
typedef struct pm_access_s {
    /*! Device physical port. */
    shr_port_t pport;

    /*! Phymod PHY access information. */
    phymod_phy_access_t phy_acc;

    /*! Phymode core access information. */
    phymod_core_access_t core_acc;

    /*! PMAC access. */
    bcmpmac_access_t pmac_acc;

    /*! PMAC access block port. */
    bcmpmac_pport_t pmac_blkport;

    /*! pc_oper_status, error status of the last LT operation. */
    pm_oper_status_t    oper_status;

} pm_access_t;

/*!
 * Structure definition for auto-negotiation status.
 */
typedef struct pm_autoneg_status_s {

    /*! Auto-Negotiation is enabled or not. */
    uint32_t an_enabled;

    /*! Auto-Negotiation is done. */
    uint32_t an_done;

} pm_autoneg_status_t;


/*! TX  FIFO Timestamp and sequence ID value. */
typedef struct pm_tx_timestamp_info_s {
    /*! Low 32-bits of Timestamp in FIFO. */
    uint32_t timestamp_lo;

    /*! High 32-bits of timestamp in FIFO. */
    uint32_t timestamp_hi;

    /*! Sequence ID of sent ptp packet. */
    uint32_t sequence_id;

    /*! Sub-nanosecond of 1588 transmit packet. */
    uint32_t timestamp_sub_nanosec;
} pm_tx_timestamp_info_t;

/*! Port Macro egress timestamping mode enumerations. */
typedef enum pm_egr_timestamp_mode_s {
    /*! 32-bit egress 1588 timestamp. */
    PM_32_BIT_EGRESS_TIMESTAMP = 0,

    /*! 48-bit egress 1588 timestamp. */
    PM_48_BIT_EGRESS_TIMESTAMP = 1
} pm_egr_timestamp_mode_t;

#endif /* BCMPC_PM_DRV_TYPES_H */
