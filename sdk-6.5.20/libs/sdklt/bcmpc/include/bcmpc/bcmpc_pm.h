/*! \file bcmpc_pm.h
 *
 * PortMacro Manager (PM) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the PortMacro Manager.
 *
 * The PortMacro library provides the functions to configure the whole PM, the
 * core within a PM or the lane within a PM. The library provides a common
 * interface for all the PM types. The PM specific implementation is internal
 * to the related PM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_H
#define BCMPC_PM_H

#include <bcmpc/bcmpc_pm_drv_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <bcmpmac/bcmpmac_acc.h>
#include <bcmpmac/bcmpmac_drv.h>

/*! Autonegotiation status. */
typedef phymod_autoneg_status_t pm_port_autoneg_status_t;

/*! Structure for storing PM specific information. */
typedef struct pm_db_s {
    /*! physical port bitmap. */
    pm_pbmp_t phys;

    /*!
     * TSC reference clock - device specific.
     * Most of the cases it is 156.25MHz.
     */
    phymod_ref_clk_t ref_clk;

    /*! Firmware load method. */
    phymod_firmware_load_method_t fw_load_method;

    /*! Function pointer for firmware downloading. */
    phymod_firmware_loader_f external_fw_loader;

    /*! PMAC driver. */
    bcmpmac_drv_t *pmacd;

    /*!
     * PM specific SW state information used for HA.
     */
    pm_ha_info_t *pm_ha_info;

    /*!
     * PMD firmware information.
     */
    char *pmd_fw_info;

    /*! CMIC to Port Macro delay in nanosecond. */
    uint32_t pm_offset;

} pm_db_t;

/*! This structure contains specific PM state. */
typedef struct pm_info_s {
    /*! PM type (used mainly for dispatching). */
    pm_type_t type;

    /*! Lock for portmacro database structure */
    sal_mutex_t pm_lock;

    /*!
     * PM internal state. This information is internal,
     * and maintained in the specific PM driver code.
     */
    pm_db_t pm_data;
} pm_info_t;

/*! Auxiliary info for assigning port mode. */
typedef enum pm_port_mode_aux_info_e {

    /*! No info. */
    PM_MODE_INFO_NONE = 0,

    /*! There are three ports in a core. */
    PM_MODE_INFO_THREE_PORTS = 1,

    /*! There are two ports in a core and each lane is in dual port mode. */
    PM_MODE_INFO_TWO_DUAL_MODE_PORTS = 2,

    /*! Should be last. */
    PM_MODE_INFO_COUNT
} pm_port_mode_aux_info_t;


/*! Supported mode on QTC core. */
typedef enum pm_qtc_mode_e {
    /*! Invalid mode. */
    PM_QTC_MODE_INVALID = 0,

    /*! PM core in Ethernet mode. */
    PM_QTC_MODE_ETHERNET = 1,

    /*! PM core in QSGMII mode. */
    PM_QTC_MODE_QSGMII = 2,

    /*! PM core in USXGMII mode. */
    PM_QTC_MODE_USXGMII = 3,

    /*! Must be the last one. */
    PM_QTC_MODE_COUNT
} pm_qtc_mode_t;

/*! Store time stamp adjust from configuration for MAC. */
typedef struct pm_port_ts_adjust_s {
    /*! Speed of the port. */
    int speed;

    /*! MAC pipeline delay in OSTS. */
    uint32_t osts_adjust;

    /*! Delay of TS timer from TS clk to TSC_Clk domain. */
    uint32_t tsts_adjust;
} pm_port_ts_adjust_t;

/*! port config information. */
typedef struct pm_port_config_s {
    /*! Enable parallel detection. */
    int pdetect;

    /*!
     * Specify the AN mode.
     * CL73, CL37, CL37BAM, CL73BAM, MSA etc..
     */
    phymod_an_mode_type_t an_mode;

    /*! Specify to enable link-training. */
    int an_cl72;

    /*! Specify to enable link-training in Forced speed mode. */
    int fs_cl72;

    /*! Specify FEC type to be enabled in AN mode.  */
    pm_port_fec_t an_fec;

    /*! TX FIR per-lane configuration parameters.  */
    phymod_tx_t tx_params[PHYMOD_MAX_LANES_PER_CORE];

    /*! Specify the set the port as HIGIG. */
    int is_hg;

    /*! Per-lane polarity configuration. */
    phymod_polarity_t polarity;

    /*! Specify the lane swap information per-core. */
    phymod_lane_map_t lane_map;

    /*! PHY firmware load method - NONE, INTERNAL, EXTERNAL. */
    phymod_firmware_load_method_t fw_load_method;

    /*! PHY firmware loader function if loader method is EXTERNAL. */
    phymod_firmware_loader_f fw_loader;

    /*!
     * TSC reference clock - device specific.
     * Mostly it is 156.25MHz.
     */
    phymod_ref_clk_t ref_clk;

    /*! Auxiliary info for assigning the core port mode. */
    pm_port_mode_aux_info_t port_mode_aux_info;

    /*! Time stamp adjust for MAC. */
    #define PM_TS_ADJUST_NUM  8

    /*! Configure time stamp adjust for MAC. */
    pm_port_ts_adjust_t port_ts_adjust[PM_TS_ADJUST_NUM];

    /*!
     * PMD TXPI mode.
     * 0 - disable, 1- Enable with external PD (Phase detector).
     */
    uint32_t txpi_mode;

    /*!
     * TXPI SDM scheme type.
     * 0 - 1st order scheme with floor,
     * 1 - 1st order scheme with rounding,
     * 2 - 2nd order scheme with rounding.
     */
    uint32_t txpi_sdm_scheme;

    /*! PM Core mode, Valid only for PM4x10QTC. */
    pm_qtc_mode_t qtc_mode;

    /*! Enable ECC interrupts. */
    int parity_enable;
} pm_port_config_t;

/*!
 * brief List of available VCOs for PM
 */
typedef enum pm_vco_e {
    /*! VCO rate is NONE. */
    PM_VCO_INVALID,

    /*! VCO rate is 6.25G. */
    PM_VCO_6P25G,

    /*! VCO rate is 10.3125G. */
    PM_VCO_10P3125G,

    /*! VCO rate is 12.5G. */
    PM_VCO_12P5G,

    /*! VCO rate is 20.625G. */
    PM_VCO_20P625G,

    /*! VCO rate is 25.781G. */
    PM_VCO_25P781G,

    /*! VCO rate is 26.562G. */
    PM_VCO_26P562G,

    /*! VCO rate is 41.25G. */
    PM_VCO_41P25G,

    /*! VCO rate is 51.5625G. */
    PM_VCO_51P562G,

    /*! VCO rate is 53.125G. */
    PM_VCO_53P125G,

    /*! Must be the last one. */
    PM_VCO_COUNT

} pm_vco_t;

/*! PM add flags */
/*! Phymod serdes core probing. */
#define PM_PORT_ADD_F_INIT_PROBE 0x1
/*! Phymod serdes core init pass1. */
#define PM_PORT_ADD_F_INIT_PASS1 0x2
/*! Phymod serdes core init pass2. */
#define PM_PORT_ADD_F_INIT_PASS2 0x4
/*! Phymod serdes core init firmware load verify. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY 0x8
/*! Phymod serdes core init firmware CRC verify. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY 0x10
/*! Phymod serdes core init pass2 bypass PMD config */
#define PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG 0x20
/*! Phymod serdes core init top device revision ID. */
#define PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0 0x40


/*! Set core probing request to flags in info. */
#define PM_PORT_ADD_F_INIT_PROBE_SET(info) ((info)->flags |= \
                                            PM_PORT_ADD_F_INIT_PROBE)
/*! Set core init pass1 request to flags in info. */
#define PM_PORT_ADD_F_INIT_PASS1_SET(info) ((info)->flags |= \
                                            PM_PORT_ADD_F_INIT_PASS1)
/*! Set core init pass2 request to flags in info. */
#define PM_PORT_ADD_F_INIT_PASS2_SET(info) ((info)->flags |= \
                                            PM_PORT_ADD_F_INIT_PASS2)
/*! Set core init firmware load verify request to flags in info. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY_SET(info) ((info)->flags |= \
                                       PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY)
/*! Set core init firmware crc verify request to flags in info. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY_SET(info) ((info)->flags |= \
                                       PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY)
/*! Set core init pass2 bypass PMD config */
#define PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_SET(info) ((info)->flags |= \
                                            PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG)
/*! Set core init top device revision ID */
#define PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0_SET(info) ((info)->flags |= \
                                            PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0)

/*! Clear core probing request from flags in info. */
#define PM_PORT_ADD_F_INIT_PROBE_CLR(info) ((info)->flags &= \
                                            ~PM_PORT_ADD_F_INIT_PROBE)
/*! Clear core init pass1 request from flags in info. */
#define PM_PORT_ADD_F_INIT_PASS1_CLR(info) ((info)->flags &= \
                                            ~PM_PORT_ADD_F_INIT_PASS1)
/*! Clear core init pass2 request from flags in info. */
#define PM_PORT_ADD_F_INIT_PASS2_CLR(info) ((info)->flags &= \
                                            ~PM_PORT_ADD_F_INIT_PASS2)
/*! Clear core init firmware load verify request to flags in info. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY_CLR(info) ((info)->flags |= \
                                       ~PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY)
/*! Clear core init firmware crc verify request to flags in info. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY_CLR(info) ((info)->flags |= \
                                       ~PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY)
/*! Clear to use the core VCO rate from the information provided. */
#define PM_PORT_ADD_F_USE_TVCO_CLR(info) ((info)->flags &= \
                                          ~PM_PORT_ADD_F_USE_TVCO)
/*! Clear to use the core VCO rate from the information provided. */
#define PM_PORT_ADD_F_USE_OVCO_CLR(info) ((info)->flags &= \
                                          ~PM_PORT_ADD_F_USE_OVCO)
/*! Clear to use the core VCO rate from the information provided. */
#define PM_PORT_ADD_F_USE_VCO_CLR(info) ((info)->flags &= \
                                          ~PM_PORT_ADD_F_USE_VCO)
/*! clear core init pass2 bypass PMD config */
#define PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_CLEAR(info) ((info)->flags &= \
                                            ~PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG)
/*! clear core init top device revision ID */
#define PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0_CLEAR(info) ((info)->flags &= \
                                            ~PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0)


/*! Get core probing request from flags in info. */
#define PM_PORT_ADD_F_INIT_PROBE_GET(info) ((info)->flags & \
                                            PM_PORT_ADD_F_INIT_PROBE ? 1 : 0)
/*! Get core init pass 1 request from flags in info. */
#define PM_PORT_ADD_F_INIT_PASS1_GET(info) ((info)->flags & \
                                            PM_PORT_ADD_F_INIT_PASS1 ? 1 : 0)
/*! Get core init pass2 request from flags in info. */
#define PM_PORT_ADD_F_INIT_PASS2_GET(info) ((info)->flags & \
                                            PM_PORT_ADD_F_INIT_PASS2 ? 1 : 0)
/*! Get core init firmware load verify request from flags in info. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY_GET(info) ((info)->flags & \
                                PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY ? 1 : 0)
/*! Get core init firmware crc verify request from flags in info. */
#define PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY_GET(info) ((info)->flags & \
                                 PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY ? 1 : 0)
/*! Get the method to derive core VCO rate. */
#define PM_PORT_ADD_F_USE_TVCO_GET(info) ((info)->flags & \
                                          PM_PORT_ADD_F_USE_TVCO? 1: 0)
/*! Get the method to derive core VCO rate. */
#define PM_PORT_ADD_F_USE_OVCO_GET(info) ((info)->flags & \
                                          PM_PORT_ADD_F_USE_OVCO? 1: 0)
/*! Get the method to derive core VCO rate. */
#define PM_PORT_ADD_F_USE_VCO_GET(info) ((info)->flags & \
                                          PM_PORT_ADD_F_USE_VCO? 1: 0)
/*! Get core init pass2 bypass PMD config */
#define PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_GET(info) ((info)->flags & \
                                          PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG? 1: 0)
/*! Get core init top device revision ID */
#define PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0_GET(info) ((info)->flags & \
                                          PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0? 1: 0)


/*!
 * PMD firmware lane configuration flag.
 * Lane configuration default settings in microcode when speed is configured.
 */
#define PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG 0x1

/*!
 * Serdes tx equalization FIR tap configuration flag.
 * TXFIR tap default settings when speed is configured.
 */
#define PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG 0x2

/*! Set default PMD firmware lane setting flag. */
#define PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG_SET(flags) \
                        ((flags) |= PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG)

/*! Set default tx taps setting flag. */
#define PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG_SET(flags) \
                        ((flags) |= PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG)

/*! Clear default PMD firmware lane setting flag. */
#define PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG_CLR(flags) \
                        ((flags) &= ~PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG)

/*! Clear default tx taps setting flag. */
#define PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG_CLR(flags) \
                        ((flags) &= ~PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG)

/*! Get default PMD firmware lane setting flag. */
#define PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG_GET(flags) \
                  ((flags) & PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG ? 1 : 0)

/*! Get default tx taps setting flag. */
#define PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG_GET(flags) \
                  ((flags) & PM_PORT_USER_F_DEFAULT_TX_TAPS_CONFIG ? 1 : 0)

/*! Validate pm_vco_t. */
int pm_vco_t_validate(int unit, pm_vco_t pm_vco_type);

/*! Infomation for adding a port to portmacro. */
typedef struct pm_port_add_info_s {
    /*! Initial configuration. */
    pm_port_config_t init_config;

    /*! Speed configuration. */
    pm_speed_config_t speed_config;

    /*! TVCO value. */
    pm_vco_t tvco;

    /*! OVCO value. */
    pm_vco_t ovco;

    /*! Phyical lanes belong to this portmacro. */
    pm_pbmp_t phys;

    /*! qsgmii only; Sub PHY index. */
    int sub_phy;

    /*! Port add flags. */
    uint32_t flags;

} pm_port_add_info_t;

/*! Structure for phy status. */
typedef struct pm_phy_status_s {

    /*! Tx lane control state. */
    phymod_phy_tx_lane_control_t tx_control;

    /*! Rx lane control state. */
    phymod_phy_rx_lane_control_t rx_control;

    /*! Rx afe setting. */
    phymod_rx_t rx_afe;

    /*! rx signal detect. */
    uint32_t rx_signal_detect;

    /*! pam4 precoder enable state. */
    int pam4_tx_precoder_enabled;

    /*! pam4 tx pattern*/
    phymod_PAM4_tx_pattern_t pam4_tx_pat;

    /*! Phy Ecc interrupt enable state. */
    uint32_t phy_ecc_intr_enabled;

    /*! Phy tx PI status */
    int32_t tx_pi;

    /*! pmd debug lane event log level */
    uint8_t pmd_debug_lane_event_log_level;

    /*! Phy fec bypass indication. */
    uint32_t phy_fec_bypass_ind;

} pm_phy_status_t;

/*! Structure for phy control. */
typedef struct pm_phy_control_s {

    /*! Tx lane control state. */
    phymod_phy_tx_lane_control_t tx_control;

    /*! Rx lane control state. */
    phymod_phy_rx_lane_control_t rx_control;

    /*! Rx afe setting. */
    phymod_rx_t rx_afe;

    /*! pam4 precoder enable state. */
    int pam4_tx_precoder_enable;

    /*! PAM4 tx pattern*/
    phymod_PAM4_tx_pattern_t pam4_tx_pat;

    /*! PAM4 tx pattern enable. */
    int pam4_tx_pat_enable;

    /*! Phy Ecc interrupt enable state. */
    int phy_ecc_intr_enable;

    /*! Phy tx pi override */
    int32_t tx_pi;

    /*! pmd debug lane event log level */
    uint8_t pmd_debug_lane_event_log_level;

    /*! Phy fec bypass indication. */
    int phy_fec_bypass_ind;

    /*! Phy fec detect only enable. */
    int phy_fec_detect_only;

} pm_phy_control_t;

/*! Firmware download method. */
typedef enum pm_fw_load_method_e {

    /*! PM firmware download via DMA. */
    PM_SERDES_FW_LOAD_FAST,

    /*! PM firmware slow download. */
    PM_SERDES_FW_LOAD_SLOW

} pm_fw_load_method_t;

/*!
 * \brief PM serdes firmware information.
 *
 * This structure stores serdes firmware download methods
 * and verify options.
 */
typedef struct bcmpc_pm_fw_cfg_s {

    /*! Enable firmware CRC verify. */
    bool fw_crc_verify;

    /*! Enable firmware load verify. */
    bool fw_load_verify;

    /*! Firmware load method. */
    pm_fw_load_method_t fw_load;

} bcmpc_pm_fw_cfg_t;

/*!
 * \brief This function is used to attach a logical port to the
 * Port Macro.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  add_info Port configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_attach(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                   pm_port_add_info_t *add_info);

/*!
 * PM APIs to enable/disable a port.
 */

/*! Enable/disable the MAC(RX/TX according to the flags). */
#define PM_PORT_ENABLE_MAC 0x1

/*! Enable/disable the Serdes (RX/TX according to the flags). */
#define PM_PORT_ENABLE_PHY 0x2

/*! Enable/disable TX. */
#define PM_PORT_ENABLE_TX 0x4

/*! Enable/disable RX. */
#define PM_PORT_ENABLE_RX 0x8

/*! Enable Tx sqeulch. */
#define PM_PHY_CTRL_F_TX_SQUELCH             (1 << 0)

/*! Enable Rx sqeulch. */
#define PM_PHY_CTRL_F_RX_SQUELCH             (1 << 1)

/*! Enable Rx adaptation resume. */
#define PM_PHY_CTRL_F_RX_ADPT_RESUME         (1 << 2)

/*! Set Rx AFE VGA. */
#define PM_PHY_CTRL_F_RX_AFE_VGA             (1 << 3)

/*! Set Rx AFE peaking filter. */
#define PM_PHY_CTRL_F_RX_AFE_PK_FLT          (1 << 4)

/*! Set AFE low frequency peaking filter. */
#define PM_PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT  (1 << 5)

/*! Set AFE high frequency peaking filter. */
#define PM_PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT   (1 << 6)

/*! Set Rx AFE DFE taps. */
#define PM_PHY_CTRL_F_RX_AFE_DFE_TAP         (1 << 7)

/*! Enable PAM4 TX pattern. */
#define PM_PHY_CTRL_F_PAM4_TX_PATTERN        (1 << 8)

/*! Enable PAM4 TX precoder. */
#define PM_PHY_CTRL_F_PAM4_TX_PRECODER       (1 << 9)

/*! Enable ECC interrupt. */
#define PM_PHY_CTRL_F_ECC_INTR_ENABLE        (1 << 10)

/*! Set TX PI OVERRIDE */
#define PM_PHY_CTRL_F_TX_PI_OVERRIDE         (1 << 11)

/*! Set PMD DEBUG LANE EVENT LOG LEVEL */
#define PM_PHY_CTRL_F_PMD_DEBUG_LANE_EVENT_LOG_LEVEL         (1 << 12)

/*! Set FEC BYPASS INDICATION. */
#define PM_PHY_CTRL_F_FEC_BYPASS_IND         (1 << 13)

/*! Set FEC DETECT ONLY. */
#define PM_PHY_CTRL_F_FEC_DETECT_ONLY        (1 << 14)

/*!
 * \brief Enable or Disable Link training on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  enable 1 (enable)/ 0 (Disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_cl72_set(int unit, pm_access_t *pm_acc,
                     pm_info_t *pm_info, int enable);

/*!
 * \brief Get Link training configuration on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] enable 1 (enable)/ 0 (Disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_cl72_get(int unit, pm_access_t *pm_acc,
                     pm_info_t *pm_info, int *enable);

/*!
 * \brief Get Link training status on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] status Link training status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_cl72_status_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                            phymod_cl72_status_t *status);


/*!
 * PM APIs to set/get Loopback state.
 */
typedef enum pm_loopback_mode_e {

    /*! No loopback. */
    PM_PORT_LPBK_NONE = 0,

    /*! MAC loopback. */
    PM_PORT_LPBK_MAC,

    /*! PCS local loopback. */
    PM_PORT_LPBK_PHY_PCS,

    /*! PMD local lopback. */
    PM_PORT_LPBK_PHY_PMD,

    /*! PCS remote loopback. */
    PM_PORT_LPBK_REMOTE_PCS,

    /*! PMD remote loopback. */
    PM_PORT_LPBK_REMOTE_PMD,

    /*! EDB loopback. */
    PM_PORT_LPBK_EDB,

    /*! Number of Loopback types. */
    PM_PORT_LPBK_COUNT

} pm_loopback_mode_t;

/*! MAC address. */
typedef shr_mac_t pm_mac_t;

/*!
 * \brief Set the MAC source address that will be sent in case of Pause/LLFC.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  mac_sa MAC address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_tx_mac_sa_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                          pm_mac_t mac_sa);

/*!
 * \brief Get the MAC source address that will be sent in case of Pause/LLFC.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] mac_sa MAC address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_tx_mac_sa_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                          pm_mac_t mac_sa);

/*!
 * \brief Set Average inter-packet gap.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  value value in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_tx_average_ipg_set(int unit, pm_access_t *pm_acc,
                               pm_info_t *pm_info, int value);

/*!
 * \brief Get Average inter-packet gap.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] value value in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_tx_average_ipg_get(int unit, pm_access_t *pm_acc,
                               pm_info_t *pm_info, int *value);

/*!
 * \brief Get local fault status on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] value i (indicates fault set)/ 0 (no fault).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_local_fault_status_get(int unit, pm_access_t *pm_acc,
                                   pm_info_t *pm_info, int *value);

/*!
 * \brief Get remote fault status on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] value i (indicates fault set)/ 0 (no fault).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
/*remote fault status get*/
int pm_port_remote_fault_status_get(int unit, pm_access_t *pm_acc,
                                    pm_info_t *pm_info, int* value);

/*! Pause control information. */
typedef bcmpmac_pause_ctrl_t  pm_pause_ctrl_t;

/*! Priority-based flow control configuration. */
typedef bcmpmac_pfc_t pm_pfc_t;

/*!
 * \brief Set PFC control configuration on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  control PFC control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_pfc_control_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                            pm_pfc_t *control);

/*!
 * \brief Get PFC control configuration on a port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] control PFC control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_pfc_control_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                            pm_pfc_t *control);

/*!
 * \brief Set Port duplex mode.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  enable 1 (enable)/ 0 (disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_duplex_set(int unit, pm_access_t *pm_acc,
                       pm_info_t *pm_info, int enable);

/*!
 * \brief Get Port duplex mode.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] enable 1 (enable)/ 0 (disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_duplex_get(int unit, pm_access_t *pm_acc,
                       pm_info_t *pm_info, int *enable);

/*!
 * \brief Set port FEC enable according to local/remote FEC ability.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  enable 1 (enable)/ 0 (disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_fec_enable_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                           uint32_t enable);
/*!
 * \brief Get port FEC enable according to local/remote FEC ability.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] enable 1 (enable)/ 0 (disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_fec_enable_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                           uint32_t *enable);

/*! Supported port pause. */
typedef enum pm_port_pause_e {
    /*! No pause. */
    PM_PORT_PAUSE_NONE = 0,

    /*! TX pause. */
    PM_PORT_PAUSE_TX,

    /*! RX pause. */
    PM_PORT_PAUSE_RX,

    /*! Symmetric pause. */
    PM_PORT_PAUSE_SYMM,

    /*! All supported pause type. */
    PM_PORT_PAUSE_ALL,

    /*! Must be the last one. */
    PM_PORT_PAUSE_COUNT
} pm_port_pause_t;

/*! Supported port phy channel type. */
typedef enum pm_port_channel_e {
    /*! Short channel. */
    PM_PORT_CHANNEL_SHORT = 0,

    /*! Long channel. */
    PM_PORT_CHANNEL_LONG,

    /*! All channel type. */
    PM_PORT_CHANNEL_ALL,

    /*! Must be the last one. */
    PM_PORT_CHANNEL_COUNT
} pm_port_channel_t;

/*! Port ability type. */
typedef enum pm_port_ability_type_s {
    /*! Port ability type - local. */
    PM_PORT_ABILITY_LOCAL,

    /*! Port ability type - advertised. */
    PM_PORT_ABILITY_ADVERT
}pm_port_ability_type_t;

/*! Port ability. */
typedef struct pm_port_ability_s {
    /*! Half duplex speed bit map. */
    uint32_t speed_half_duplex;

    /*! Full duplex speed bit map. */
    uint32_t speed_full_duplex;

    /*! Loopback type. */
    uint32_t loopback;

    /*! Energy efficient Ethernet ability. */
    uint32_t eee;

    /*! Encapsulation ability. */
    uint32_t encap;

    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec_type;

    /*! Medium type. */
    pm_port_medium_t medium;

    /*! Pause ability. */
    pm_port_pause_t pause;

    /*! Channel type. */
    pm_port_channel_t channel;

    /*! Auto-negotiation mode such as cl73, BAM or MSA. */
    pm_port_autoneg_mode_t an_mode;

} pm_port_ability_t;

/*! Rx Control Pass control frames. */
typedef struct pm_rx_control_s {
    /*! Bitmap for RX Frames. */
    uint32_t flags;

    /*! Pass control frames. */
    uint32_t pass_control_frames;

    /*! Pass PFC frames. */
    uint32_t pass_pfc_frames;

    /*! Pass pause frames. */
    uint32_t pass_pause_frames;
} pm_rx_control_t;

/*!
 * \brief Configure RX control frames handling.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  rx_ctrl RX contol configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_rx_control_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                           pm_rx_control_t *rx_ctrl);

/*!
 * \brief Get RX control frames handling configuration.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] rx_ctrl RX contol configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_rx_control_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                           pm_rx_control_t *rx_ctrl);

#if 0
/*!
 * @struct pm_eee_s
 * \brief EEE Timer Information.
 */
typedef struct pm_eee_s {
    /*! Enable EEE. */
    uint32_t enable;

    /*! EEE TX idle time. */
    uint32_t tx_idle_time;

    /*! EEE TX wake time . */
    uint32_t tx_wake_time;
} pm_eee_t;

/*! EEE clock gate Information. */
typedef struct pm_eee_clock_s {
    /*! EEE clock gate. */
    uint32_t clock_gate;

    /*! EEE clock counter. */
    uint32_t clock_count;

    /*! EEE timer pulse. */
    uint32_t timer_pulse;
} pm_eee_clock_t;

/*set EEE Config.*/
int pm_port_eee_set(int unit, pm_access_t *pm_acc,
                     pm_info_t *pm_info, pm_eee_t* eee);
int pm_port_eee_get(int unit, pm_access_t *pm_acc,
                     pm_info_t *pm_info, pm_eee_t* eee);

/*set EEE Config.*/
int pm_port_eee_clock_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                          pm_eee_clock_t* eee_clk);
int pm_port_eee_clock_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                          pm_eee_clock_t* eee_clk);

/*! Vlan Tag Inner/Outer Values. */
typedef struct pm_vlan_tag_s {
    /*! Inner Vlan tag. */
    int inner_vlan_tag;

    /*! Outer Vlan tag. */
    int outer_vlan_tag;
} pm_vlan_tag_t;

/*set Vlan Inner/Outer tag.*/
int pm_port_vlan_tag_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                         pm_vlan_tag_t* vlan_tag);
int pm_port_vlan_tag_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                         pm_vlan_tag_t* vlan_tag);

/*set modid field.*/
/* valid for PM4x10 only */
int pm_port_modid_set(int unit, pm_access_t *pm_acc,
                      pm_info_t *pm_info, int value);

/*set modid field.*/
int pm_port_led_chain_config(int unit, pm_access_t *pm_acc,
                             pm_info_t *pm_info, int value);

/*set modid field.*/
int pm_port_clear_rx_lss_status_set(int unit, pm_access_t *pm_acc,
                                    pm_info_t *pm_info,
                                    int lcl_fault, int rmt_fault);
int pm_port_clear_rx_lss_status_get(int unit, pm_access_t *pm_acc,
                                    pm_info_t *pm_info,
                                    int *lcl_fault, int *rmt_fault);

#endif

/*!
 * \brief Toggle Lag Failover Status.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_lag_failover_status_toggle(int unit, pm_access_t *pm_acc,
                                       pm_info_t *pm_info);

/*!
 * \brief Set Lag Failover loopback.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  value 1 (enable) / 0 (disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_lag_failover_loopback_set(int unit, pm_access_t *pm_acc,
                                      pm_info_t *pm_info, uint32_t value);

/*!
 * \brief Set Port Macro port mode.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  mode Single, Dual, Tri or Quad mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_mode_set(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                     pm_port_mode_info_t *mode);

/*!
 * \brief Get Port Macro port mode.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_acc MAC and PHY access information.
 * \param [in]  pm_info Port Macro database information.
 * \param [out] mode Single, Dual, Tri or Quad mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int pm_port_mode_get(int unit, pm_access_t *pm_acc, pm_info_t *pm_info,
                     pm_port_mode_info_t *mode);

/*! IEEE. */
#define PM_ENCAP_IEEE BCMPMAC_ENCAP_IEEE

/*! HiGig3 encapsulation. */
#define PM_ENCAP_HG3 BCMPMAC_ENCAP_HG3

/*! HiGIG encapsulation. */
#define PM_ENCAP_HIGIG BCMPMAC_ENCAP_HIGIG

/*! Encapsulation modes. */
typedef bcmpmac_encap_mode_t pm_encap_t;

/*! Timesync enable flags. Used in en/decoding HA timesync enable info. */
/*! Enable 1588. */
#define PM_PORT_TIMESYNC_F_1588_ENABLE 0x1
/*! Use SOP Timestamping. If this flag is not set, means SFD Timestamping. */
#define PM_PORT_TIMESYNC_F_SOP_TS  0x10
/*! Compensation mode - Earliest lane mode. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST 0x100
/*! Compensation mode - Latest lane mode. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_LATEST 0x200

/*! Set 1588 enable flag in flags. */
#define PM_PORT_TIMESYNC_F_1588_ENABLE_SET(flags) \
                              (flags |= PM_PORT_TIMESYNC_F_1588_ENABLE)
/*! Set SOP timestmap flag in flags. */
#define PM_PORT_TIMESYNC_F_SOP_TS_SET(flags) \
                              (flags |= PM_PORT_TIMESYNC_F_SOP_TS)
/*! Set earliest compensation mode flag in flags. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST_SET(flags) \
                              (flags |= PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST)
/*! Set latest compensation mode flag in flags. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_LATEST_SET(flags) \
                              (flags |= PM_PORT_TIMESYNC_F_COMP_MODE_LATEST)

/*! Clear 1588 enable flag in flags. */
#define PM_PORT_TIMESYNC_F_1588_ENABLE_CLR(flags) \
                              (flags &= ~PM_PORT_TIMESYNC_F_1588_ENABLE)
/*! Clear SOP timestamp flag in flags. */
#define PM_PORT_TIMESYNC_F_SOP_TS_CLR(flags) \
                              (flags &= ~PM_PORT_TIMESYNC_F_SOP_TS)
/*! Clear earliest compensation mode flag in flags. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST_CLR(flags) \
                              (flags &= ~PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST)
/*! Clear latest compensation mode flag in flags. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_LATEST_CLR(flags) \
                              (flags &= ~PM_PORT_TIMESYNC_F_COMP_MODE_LATEST)

/*! Get 1588 enable flag in flags. */
#define PM_PORT_TIMESYNC_F_1588_ENABLE_GET(flags) \
                         (flags & PM_PORT_TIMESYNC_F_1588_ENABLE ? 1 : 0)
/*! Get SOP timestamp flag in flags. */
#define PM_PORT_TIMESYNC_F_SOP_TS_GET(flags) \
                         (flags & PM_PORT_TIMESYNC_F_SOP_TS ? 1 : 0)
/*! Get earliest compensation mode flag in flags. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST_GET(flags) \
                         (flags & PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST ? 1 : 0)
/*! Get latest compensation mode flag in flags. */
#define PM_PORT_TIMESYNC_F_COMP_MODE_LATEST_GET(flags) \
                         (flags & PM_PORT_TIMESYNC_F_COMP_MODE_LATEST ? 1 : 0)

/*! Timesync deskew compensation mode. */
typedef enum pm_timesync_comp_mode_e {
    /* No deskew compensation. */
    PM_TIMESYNC_COMP_MODE_NONE,

    /* Timesync compensation earlist lane mode. */
    PM_TIMESYNC_COMP_MODE_EARLIEST_LANE,

    /* Timesync compensation latest lane mode. */
    PM_TIMESYNC_COMP_LATEST_LANE,

    /*! Must be the last one. */
    PM_TIMESYNC_COMP_COUNT
} pm_timesync_comp_mode_t;

/*! Base timesync configuration type. */
typedef struct pm_phy_timesync_config_s {
    /*! Enable/Disable 1588. */
    bool timesync_en;

    /*! One-step ot two-step timestamping. */
    bool is_one_step;

    /*! SOP or SFD timestamping. */
    bool is_sop;

    /*! Deskew compensation mode. */
    pm_timesync_comp_mode_t comp_mode;

} pm_phy_timesync_config_t;

/*! VCO settings. Used for VCO validation to get correct VCO settings. */
typedef struct pm_vco_setting_s {
    /*! Number of speed ids that need to be supported. */
    int num_speeds;

    /*! List of speed configs. */
    pm_speed_config_t* speed_config_list;

    /*!
     * List of starting_lanes correspond to speed
     * configs in speed_config_list.
     */
    int* port_starting_lane_list;

    /*! If speed is for PCS bypass port. */
    int* speed_for_pcs_bypass_port;

    /*! Higher VCO value. */
    pm_vco_t tvco;

    /*! Lower VCO value. */
    pm_vco_t ovco;

    /*! Tvco change is needed. */
    int is_tvco_new;

    /*! Ovco change is needed. */
    int is_ovco_new;
} pm_vco_setting_t;

/*! Enum definition for SyncE stage 0 divider mode. */
typedef enum pm_port_synce_ctrl_stg0_mode_e {
    /*! SyncE stage 0 - no divider. */
    PM_SYNCE_STG0_NO_DIV,

    /*! SyncE stage 0 divider - gap clock 4 over 5. */
    PM_SYNCE_STG0_DIV_GAP_CLK_4_OVER_5,

    /*! SyncE stage 0 divier - SDM fractional divider. */
    PM_SYNCE_STG0_SDM_FRAC_DIV,

    /*! SyncE stage 0 mode count. */
    PM_SYNCE_STG0_COUNT
} pm_port_synce_ctrl_stg0_mode_t;

/*! Enum definition for SyncE stage 1 divier mode. */
typedef enum pm_port_synce_ctrl_stg1_mode_e {
    /*! SyncE stage 1 - no divider. */
    PM_SYNCE_STG1_NO_DIV,

    /*! SyncE stage 1 divider 7. */
    PM_SYNCE_STG1_DIV_7,

    /*! SyncE stage 1 divider 11. */
    PM_SYNCE_STG1_DIV_11,

    /*!SyncE stage 1 mode count. */
    PM_SYNCE_STG1_COUNT

} pm_port_synce_ctrl_stg1_mode_t;

/*! Port SyncE configuration infomation. */
typedef struct pm_port_synce_clk_ctrl_s {
    /*! Stage 0 divider value. */
    pm_port_synce_ctrl_stg0_mode_t stg0_mode;

    /*! Stage 1 divider value. */
    pm_port_synce_ctrl_stg1_mode_t stg1_mode;

    /*! Modulator value */
    uint32_t sdm_val;
} pm_port_synce_clk_ctrl_t;

/*! RLM configuration. */
typedef struct pm_port_rlm_config_s {
    /*! Is initiator or not. */
    int is_initiator;

    /*! enable.disable for RLM */
    bool rlm_enable;

    /*! RLM active bit map. */
    uint32_t active_lane_bit_map;
} pm_port_rlm_config_t;

/*! RLM status. */
typedef struct pm_port_rlm_status_s {
    /*! RLM is not enabled. */
    int rlm_disabled;

    /*! RLM is not enabled. */
    int rlm_busy;

    /*! RLM is complete. */
    int rlm_done;

    /*! RLM failed. */
    int rlm_failed;

    /*! RLM status count. */
    int rlm_status_count;
} pm_port_rlm_status_t;

/*!
 * \brief Port interrupt types.
 *
 * This enum defines the interrupt types supported.
 */
typedef enum pm_intr_type_s {
    /*! FDR interrupt. */
    PM_INTR_FDR,

    /*! Should be last. */
    PM__INTR_COUNT
} pm_intr_type_t;

/*!
 * \brief Initialize the major data of \c pm_speed_config_t used in PM.
 *
 * \param [in] unit Unit number.
 * \param [out] speed_config Pointer to \c pm_speed_config_t structure.
 *
 * \retval NONE.
 */
int
pm_speed_config_t_init(int unit, pm_speed_config_t *speed_config);

/*!
 * \brief Initialize structure of bcmpc_pw_fw_cfg_t.
 *
 * \param [in] fw_cfg Firmwareconfiguration structure.
 *
 * \retval NONE.
 */
void
bcmpc_pm_fw_cfg_t_init(bcmpc_pm_fw_cfg_t *fw_cfg);

/*!
 * \brief Get serdes firmware downlad and verify options.
 *
 * \param [in] unit Unit number.
 * \param [out] fw_cfg Firmware configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_pm_fw_load_config_get(int unit, bcmpc_pm_fw_cfg_t *fw_cfg);

/*!
 * \brief Pass serdes firmware downlad and verify options to phymod.
 *
 * \param [in] unit Unit number.
 * \param [in] fw_cfg Firmware configuration.
 * \param [out] core_init_config phymod core init configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_phymod_fw_load_config_set(int unit, bcmpc_pm_fw_cfg_t *fw_cfg,
                            phymod_core_init_config_t *core_init_config);

#endif /* BCMPC_PM_H */
