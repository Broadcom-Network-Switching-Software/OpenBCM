/*! \file bcmpc_pmgr_internal.h
 *
 * BCMPC PMGR Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PMGR_INTERNAL_H
#define BCMPC_PMGR_INTERNAL_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <phymod/phymod.h>

/*!
 * \brief Enumeration of PHY line side interface.
 */
typedef enum bcmpc_phy_intf_e {

    /*! No PHY */
    BCMPC_PHY_INTF_NULL,

    /*! SerDes Interface. */
    BCMPC_PHY_INTF_SERDES,

    /*! Media-Independent Interface. */
    BCMPC_PHY_INTF_MII,

    /*! Gigabit MII. */
    BCMPC_PHY_INTF_GMII,

    /*! Serial Gigabit MII. */
    BCMPC_PHY_INTF_SGMII,

    /*! Ten Bit Interface. */
    BCMPC_PHY_INTF_TBI,

    /*! 10-Gigabit MII. */
    BCMPC_PHY_INTF_XGMII,

    /*! Reduced Gigabit MII. */
    BCMPC_PHY_INTF_RGMII,

    /*! Reverse MII. */
    BCMPC_PHY_INTF_RVMII,

    /*! 10-gigabit Attachment Unit Interface. */
    BCMPC_PHY_INTF_XAUI,

    /*! Reduced XAUI. */
    BCMPC_PHY_INTF_RXAUI,

    /*! 40-gigabit AUI. */
    BCMPC_PHY_INTF_XLAUI,

    /*! 100-gigabit AUI. */
    BCMPC_PHY_INTF_CAUI,

    /*! XFP Interface. */
    BCMPC_PHY_INTF_XFI,

    /*! Serdes Framer Interface. */
    BCMPC_PHY_INTF_SFI,

    /*! Coper Interface. */
    BCMPC_PHY_INTF_CX,

    /*! Coper Interface. */
    BCMPC_PHY_INTF_CR,

    /*! Backplane Interface with 64B/66B PCS. */
    BCMPC_PHY_INTF_KR,

    /*! Backplane Interface with 8B/10B PCS. */
    BCMPC_PHY_INTF_KX,

    /*! Short Reach Interface. */
    BCMPC_PHY_INTF_SR,

    /*! Long Reach Interface. */
    BCMPC_PHY_INTF_LR,

    /*! Must be the last. */
    BCMPC_PHY_INTF_COUNT

} bcmpc_phy_intf_t;

/*!
 * \brief Port operational status enumeration.
 *
 * This enumeration defines the port operational status and
 * should mimic PC_PORT_STATE_T
 */
typedef enum bcmpc_port_oper_state_e {

    /*! Both RX and TX are disabled. */
    BCMPC_MAC_TX_RX_OFF,

    /*! Both RX and TX are enable. */
    BCMPC_MAC_TX_RX_ON,

    /*! Only RX is enabled and TX is disabled. */
    BCMPC_MAC_RX_ON,

    /*! Only TX is enabled and RX is disabled. */
    BCMPC_MAC_TX_ON

} bcmpc_port_oper_state_t;

/*!
 * \brief Port status structure
 *
 * This data structure defines the attributes for a logical port status.
 */
typedef struct bcmpc_pmgr_port_status_s {

    /*! MAC enabled. */
    uint32_t mac_enabled;

    /*! PHY enabled. */
    uint32_t phy_enabled;

    /*! Encapsulation mode. */
    bcmpc_port_encap_t encap_mode;

    /*! Loopback mode. */
    bcmpc_port_lpbk_t lpbk_mode;

    /*! Operational FEC mode. */
    bcmpc_fec_t fec_mode;

    /*! Port operating speed. */
    uint32_t speed;

    /*! Number of lanes on operating port.  */
    int num_lanes;

    /*! Link training status. */
    int link_training;

    /*! Auto-negotiation status. */
    int autoneg;

    /*! Link training status . */
    int link_training_done;

    /*! Auto-negotiation status. */
    int an_done;

    /*! Local fault. */
    uint32_t local_fault;

    /*! Remote fault. */
    uint32_t remote_fault;

    /*! Operational status. */
    bcmpc_port_oper_state_t oper_state;

} bcmpc_pmgr_port_status_t;

/*!
 * \brief Link status type.
 *
 * The link status is not a singal bit for up or down, but also including some
 * link related information, such as remote/local faults, etc.
 *
 * This enum defines the events for the port link status.
 */
typedef uint32_t bcmpc_pmgr_link_status_t;

/*!
 * \brief Link status bit definition.
 *
 * This enum defines the events for the port link status,
 * \ref bcmpc_pmgr_link_status_t.
 * The bit will be set when the corresponding event occurs.
 */
typedef enum bcmpc_pmgr_link_event_e {

    /*! Port is fully operational. */
    BCMPC_PMGR_LINK_EVENT_LINK = 0,

    /*! Local fault. */
    BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT,

    /*! Remote fault. */
    BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT,

    /*! PLL lock error. */
    BCMPC_PMGR_LINK_EVENT_PMD_RX_LOCK,

    /*! PCS link error. */
    BCMPC_PMGR_LINK_EVENT_PCS_LINK_ERR,

    /*! PMD link error. */
    BCMPC_PMGR_LINK_EVENT_PMD_LINK_ERR,

    /*! No signal detected. */
    BCMPC_PMGR_LINK_EVENT_NO_SIGNAL,

    /*! Lane synchronization error. */
    BCMPC_PMGR_LINK_EVENT_LANE_SYNC_ERR

} bcmpc_pmgr_link_event_t;

/*!
 * \brief Port config entry structure.
 *
 * The port configurations are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is a physical port number.
 */
typedef struct bcmpc_pmgr_db_entry_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Port configuration. */
    bcmpc_port_cfg_t pcfg;

} bcmpc_pmgr_db_entry_t;


/*!
 * \brief Port manager driver data for a port
 *
 * This structure is used to store the PMAC and PHY driver information for a
 * port. The information is gotten via the methods of \ref bcmpc_drv_t.
 *
 * In brief, \ref bcmpc_drv_t provides the "methods" to retrive driver
 * information for each port. And \ref bcmpc_pmgr_drv_data_t is the "storage"
 * to save the information.
 */
typedef struct bcmpc_pmgr_drv_data_s {

    /*! Physical device port number. */
    bcmpc_pport_t pport;

    /*! Max spped of the port. */
    int max_speed;

    /*! PMAC Driver. */
    bcmpmac_drv_t *pmac_drv;

    /*! PMAC Access. */
    bcmpmac_access_t pmac_acc;

    /*! PMAC Access data. */
    bcmpc_pmac_access_data_t pmac_data;

    /*! PMAC Access block port. */
    bcmpmac_pport_t pmac_blkport;

    /*! PHY Access. */
    phymod_phy_access_t phy_acc;

    /*! PHY Core Access. */
    phymod_core_access_t phy_core_acc;

    /*! PHY Access Driver. */
    bcmpc_phy_access_data_t phy_data;

    /*! PM Id. */
    int pm_id;

} bcmpc_pmgr_drv_data_t;

/*!
 * \brief Phy status structure
 *
 * This data structure defines the attributes for Phy status associated with
 * a logical port.
 */
typedef struct bcmpc_pmgr_phy_status_s {

    /*! TX sqeulch enabled. */
    bool tx_squelch;

    /*! rx sqeulch enabled. */
    bool rx_squelch;

    /*! RX AFE VGA set. */
    uint32_t rx_afe_vga;

    /*! RX AFE peaking filter set. */
    uint32_t rx_afe_pk_flt;

    /*! RX AFE low freq peaking filter set. */
    uint32_t rx_afe_low_fre_pk_flt;

    /*! RX AFE high freq peaking filter set. */
    uint32_t rx_afe_hi_fre_pk_flt;

    /*! RX AFE DFE TAP sign array set. */
    bool rx_afe_dfe_tap_sign[BCMPC_NUM_RX_DFE_TAPS_MAX];

    /*! RX AFE DFE TAP ARRAY set. */
    uint32_t rx_afe_dfe_tap[BCMPC_NUM_RX_DFE_TAPS_MAX];

    /*! RX Signal detect. */
    uint32_t  rx_signal_detect;

    /*! PAM4 TX pattern enabled. */
    bcmpc_pam4_tx_pattern_t pam4_tx_pattern;

    /*! PAM4 TX precoder enabled. */
    bool pam4_tx_precoder_enabled;

    /*! ECC interrupts enabled. */
    bool phy_ecc_intr_enabled;

    /*! TX pi voerride value */
    uint32_t  tx_pi;

    /*! TX pi voerride value sign. */
    bool  tx_pi_sign;

    /*! pmd debug lane event log level */
    uint8_t pmd_debug_lane_event_log_level;

    /*! Phy fec bypass indication. */
    uint32_t phy_fec_bypass_ind;

} bcmpc_pmgr_phy_status_t;

/*!
 * \brief Port ability type enumeration.
 *
 * This enumeration defines the ability types associated
 * with a logical port.
 */
typedef pm_port_ability_type_t bcmpc_pmgr_port_ability_type_t;

/*!
 * \brief Port ability structure
 *
 * This data structure defines the attributes for port abilities associated
 * with a logical port.
 */
typedef pm_port_ability_t bcmpc_pmgr_port_ability_t;

/*!
 * Tx taps configuration status.
 */
typedef bcmpc_tx_taps_config_t bcmpc_pmgr_tx_taps_status_t;

/*!
 * PMD firmware configuration status.
 */
typedef bcmpc_pmd_firmware_config_t bcmpc_pmgr_pmd_firmware_status_t;

/*!
 * PFC configuration status.
 */
typedef bcmpc_pfc_control_t bcmpc_pmgr_pfc_status_t;

/*!
 * This data structure contains the diagnostic statistics of a logical port.
 */
typedef struct bcmpc_pmgr_port_diag_stats_s {

    /*! FEC corrected codeword counter */
    uint64_t fec_corrected_cws;

    /*! FEC uncorrected codeword counter */
    uint64_t fec_uncorrected_cws;

    /*! FEC symbol error counter */
    uint64_t fec_symbol_errors;

} bcmpc_pmgr_port_diag_stats_t;


/*! Phy control flags. */
/*! Enable Tx sqeulch. */
#define PHY_CTRL_F_TX_SQUELCH            PM_PHY_CTRL_F_TX_SQUELCH

/*! Enable Rx sqeulch. */
#define PHY_CTRL_F_RX_SQUELCH            PM_PHY_CTRL_F_RX_SQUELCH

/*! Enable Rx adaptation resume. */
#define PHY_CTRL_F_RX_ADPT_RESUME        PM_PHY_CTRL_F_RX_ADPT_RESUME

/*! Set Rx AFE VGA. */
#define PHY_CTRL_F_RX_AFE_VGA            PM_PHY_CTRL_F_RX_AFE_VGA

/*! Set Rx AFE peaking filter. */
#define PHY_CTRL_F_RX_AFE_PK_FLT         PM_PHY_CTRL_F_RX_AFE_PK_FLT

/*! Set AFE low frequency peaking filter. */
#define PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT PM_PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT

/*! Set AFE high frequency peaking filter. */
#define PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT  PM_PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT

/*! Set Rx AFE DFE taps. */
#define PHY_CTRL_F_RX_AFE_DFE_TAP        PM_PHY_CTRL_F_RX_AFE_DFE_TAP

/*! Enable PAM4 TX pattern. */
#define PHY_CTRL_F_PAM4_TX_PATTERN       PM_PHY_CTRL_F_PAM4_TX_PATTERN

/*! Enable PAM4 TX precoder. */
#define PHY_CTRL_F_PAM4_TX_PRECODER      PM_PHY_CTRL_F_PAM4_TX_PRECODER

/*! Enable ECC interrupt. */
#define PHY_CTRL_F_ECC_INTR_ENABLE       PM_PHY_CTRL_F_ECC_INTR_ENABLE

/*! Set TX PI override */
#define PHY_CTRL_F_TX_PI_OVERRIDE        PM_PHY_CTRL_F_TX_PI_OVERRIDE


/*!
 * \brief Port number of abilties structure.
 *
 * This data structure defines the number of local, advert abilities
 * of a logical port.
 */
typedef struct bcmpc_pmgr_port_abilities_s {

    /*! Number of port abilities supported. */
    uint16_t  num_ability;

    /*! Port abilities. */
    bcmpc_pmgr_port_ability_t port_abilities[BCMPC_NUM_PORT_ABILITIES_MAX];

} bcmpc_pmgr_port_abilities_t;

/*!
 * \brief Port Reduce Lane Mode (RLM) status.
 *
 * This enum defines the status of RLM for a logical port.
 */
typedef enum bcmpc_pmgr_port_rlm_status_e {

    /*! RLM is disabled. */
    BCMPC_PMGR_PORT_RLM_STATUS_DISABLED = 0,

    /*! RLM is busy. */
    BCMPC_PMGR_PORT_RLM_STATUS_BUSY,

    /*! RLM is done. */
    BCMPC_PMGR_PORT_RLM_STATUS_DONE,

    /*! RLM is failed. */
    BCMPC_PMGR_PORT_RLM_STATUS_FAILED

} bcmpc_pmgr_port_rlm_status_t;

/*******************************************************************************
 * Port config DB functions
 */

/*!
 * \brief Initalize port status data structure.
 *
 * \param [in] st Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_port_status_t_init(bcmpc_pmgr_port_status_t *st);

/*!
 * \brief Enable the PM port of the given physical port.
 *
 * This function is used to enable/disable a port in port-block level.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] enable 1 to enalbe, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pm_port_enable(int unit, bcmpc_pport_t pport,
                          int enable);

/*!
 * \brief Set the port configuration of the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] lport The associated logical port.
 * \param [in] cfg Port configuration to set.
 * \param [in] is_new The port is a newly created port or not.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_cfg_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                        bcmpc_port_cfg_t *cfg, bool is_new);

/*!
 * \brief Get the port status of the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Port status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_port_status_t *st);

/*!
 * \brief Get the link status of the given physical port.
 *
 * This API is intended for use by the PC_PORT_STATUS LT.
 *
 * It will always return the complete set of link parameters e.g. link
 * status, fault state, PLL lock errors, etc.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_link_status_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Save the port configuration to DB.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_db_pcfg_save(int unit, bcmpc_pport_t pport,
                        bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Get the port configuration from DB.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 * \param [out] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_db_pcfg_get(int unit, bcmpc_pport_t pport,
                       bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Destroy the port configuration from DB.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_db_entry_destroy(int unit, bcmpc_pport_t pport);

/*!
 * \brief Initalize phy status data structure.
 *
 * \param [in] pst Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_phy_status_t_init(bcmpc_pmgr_phy_status_t *pst);

/*!
 * \brief Get the phy status of the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane combination of logical port and lane index.
 * \param [out] pst phy status buffer for the given physical port.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_phy_status_get(int unit, bcmpc_port_lane_t *port_lane,
                          bcmpc_pmgr_phy_status_t *pst);

/*!
 * \brief Initalize tx taps data structure.
 *
 * \param [in] taps_st Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_tx_taps_status_t_init(bcmpc_pmgr_tx_taps_status_t *taps_st);

/*!
 * \brief Get the phy status of the given port and lane.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane combination of logical port and lane index.
 * \param [out] taps_st TX taps configuration state.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_tx_taps_status_get(int unit, bcmpc_port_lane_t *port_lane,
                              bcmpc_pmgr_tx_taps_status_t *taps_st);

/*!
 * \brief Initalize pmd firmware data structure.
 *
 * \param [in] fm_st Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_pmd_firmware_status_t_init(bcmpc_pmgr_pmd_firmware_status_t *fm_st);

/*!
 * \brief Get the pmd firmware status of the given port and lane.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] fm_st PMD firmware configuration status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pmd_firmware_status_get(int unit, bcmpc_lport_t lport,
                                   bcmpc_pmgr_pmd_firmware_status_t *fm_st);

/*!
 * \brief Set Tx taps configuration of the given port and lane.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane combination of logical port and lane index.
 * \param [out] tx_cfg Tx taps configuration to be set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_tx_taps_config_set(int unit, bcmpc_port_lane_t *port_lane,
                              bcmpc_tx_taps_config_t *tx_cfg);

/*!
 * \brief Set PMD firmware configuration of the given port and lane.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] fm_cfg PMD firmware configuration to be set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pmd_firmware_config_set(int unit, bcmpc_lport_t lport,
                                   bcmpc_pmd_firmware_config_t *fm_cfg);

/*!
 * \brief Initalize pfc data structure.
 *
 * \param [in] taps_st Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_pfc_status_t_init(bcmpc_pmgr_pfc_status_t *taps_st);

/*!
 * \brief Get the pfc status of the given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device logical port.
 * \param [out] pst pfc status buffer for the given logical port.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pfc_status_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                          bcmpc_pmgr_pfc_status_t *pst);

/*!
 * \brief Get the port abilities of the given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [in] ability_type  Port abililty type.
 * \param [out] port_abilities Port local or advert abilities array.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_abilities_get(int unit, bcmpc_lport_t lport,
                                  bcmpc_pport_t pport,
                                  bcmpc_pmgr_port_ability_type_t ability_type,
                                  bcmpc_pmgr_port_abilities_t *port_abilities);

/*!
 * \brief Detach PM logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical Port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_port_detach(int unit, bcmpc_lport_t lport);

/*!
 * \brief  Power on PM TSC.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_tsc_power_on(int unit, bcmpc_pport_t pport);

/*!
 * \brief Set timesync configuration for the given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [in] config Timesync configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_timesync_set(int unit, bcmpc_lport_t lport,
                        bcmpc_pport_t pport,
                        bcmpc_port_timesync_t *config);

/*******************************************************************************
 * Type converter functions
 */

/*!
 * \brief Convert PMGR encap mode to PMAC.
 *
 * \param [in] pmgr_mode PMGR encap mode.
 * \param [out] pmac_mode PMAC encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_encap_mode_to_pmac(bcmpc_port_encap_t pmgr_mode,
                              bcmpmac_encap_mode_t *pmac_mode);

/*!
 * \brief Convert PMGR encap mode from PMAC.
 *
 * \param [in] pmac_mode PMAC encap mode.
 * \param [out] pmgr_mode PMGR encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_encap_mode_from_pmac(bcmpmac_encap_mode_t pmac_mode,
                                bcmpc_port_encap_t *pmgr_mode);

/*!
 * \brief Convert PM operating mode to PMAC.
 *
 * \param [in] pm_mode PM operating mode.
 * \param [out] pmac_mode PMAC core port mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_mode_to_pmac(bcmpc_pm_mode_t *pm_mode,
                           bcmpmac_core_port_mode_t *pmac_mode);

/*!
 * \brief Convert PMGR loopback mode to PHYMOD.
 *
 * \param [in] pmgr_mode PMGR loopback mode.
 * \param [out] phy_mode PHYMOD loopback mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_lpbk_mode_to_phy(bcmpc_port_lpbk_t pmgr_mode,
                            phymod_loopback_mode_t *phy_mode);

/*!
 * \brief Convert PMGR loopback mode from PHYMOD.
 *
 * \param [in] phy_mode PHYMOD loopback mode.
 * \param [out] pmgr_mode PMGR loopback mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_lpbk_mode_from_phy(phymod_loopback_mode_t phy_mode,
                              bcmpc_port_lpbk_t *pmgr_mode);

/*!
 * \brief Get the default PHYMOD interface.
 *
 * The default interface type is derived from port speed, number of lanes and
 * encapsulation mode.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] num_lanes Number of lanes.
 * \param [in] is_hg Using Higig/Higig2 mode or not.
 * \param [in] fiber_pref Prefer fiber interface or not.
 * \param [out] intf PHYMOD interface type.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_default_phymod_intf_get(int speed, int num_lanes, int is_hg,
                                   int fiber_pref, phymod_interface_t *intf);

/*!
 * \brief Convert core clock value to PHYMOD reference clock type.
 *
 * \param [in] ref_clk Core reference clock in KHz.
 * \param [out] phymod_clk PHYMOD reference clock value.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_ref_clk_to_phy(int ref_clk, phymod_ref_clk_t *phymod_clk);

/*!
 * \brief Convert the PLL value to PHYMOD PLL type.
 *
 * \param [in] pll PLL divider value multiplied by 1000,
 *                 e.g. 72000 means 72.000, and 73600 means 73.600.
 * \param [out] phymod_pll PHYMOD PLL value.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pll_to_phy(uint32_t pll, uint32_t *phymod_pll);

/*!
 * \brief Convert the PLL value to PHYMOD PLL type.
 *
 * \param [in] phymod_pll PHYMOD PLL value.
 * \param [out] pll PLL divider value multiplied by 1000,
 *                  e.g. 72000 means 72.000, and 73600 means 73.600.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pll_from_phy(uint32_t phymod_pll, uint32_t *pll);

/*!
 * \brief Convert PC FEC type to PHYMOD.
 *
 * \param [in] fec PC FEC type, \ref bcmpc_fec_t.
 * \param [out] phymod_fec PHYMOD FEC type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_fec_to_phy(bcmpc_fec_t fec, phymod_fec_type_t *phymod_fec);

/*!
 * \brief Convert PC FEC type from PHYMOD.
 *
 * \param [in] phymod_fec PHYMOD FEC type.
 * \param [out] fec PC FEC type, \ref bcmpc_fec_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_fec_from_phy(phymod_fec_type_t phymod_fec, bcmpc_fec_t *fec);

/*!
 * \brief Convert core configuration to PHYMOD.
 *
 * \param [in] unit Unit number
 * \param [in] ccfg PHYMOD interface config.
 * \param [in] num_lanes Number of lanes for the core.
 * \param [out] phymod_ccfg Phymod core configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_core_cfg_to_phy(int unit, bcmpc_pm_core_cfg_t *ccfg,
                              int num_lanes,
                              phymod_core_init_config_t *phymod_ccfg);

/*!
 * \brief Convert autoneg profile to phymod_autoneg_ability_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_lanes Number of lanes.
 * \param [in] is_hg Is Higig/Higig2 mode.
 * \param [in] prof Autoneg profile.
 * \param [out] phy_an_ability PHYMOD autoneg ability.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_autoneg_prof_to_phy_adv(int unit, int num_lanes, int is_hg,
                              bcmpc_autoneg_prof_t *prof,
                              phymod_autoneg_ability_t *phy_an_ability);

/*!
 * \brief Get the signal mode for the given lane speed.
 *
 * \param [in] lane_spd Lane speed in Mbps, i.e. port_speed / port_num_lanes.
 *
 * \return phymod_phy_signalling_method_t.
 */
extern phymod_phy_signalling_method_t
bcmpc_phy_signal_mode_get(uint32_t lane_spd);


/*!
 * \brief Convert bcmpc core configuration to PM port configuration.
 *
 * \param [in] unit           - Unit number.
 * \param [in] ccfg           - bcmpc core configuration.
 * \param [in] num_lanes      - Number of lanes.
 * \param [out] port_add_info - PM port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_core_cfg_to_pm_cfg(int unit, bcmpc_pm_core_cfg_t *ccfg,
                                 int num_lanes,
                                 pm_port_add_info_t  *port_add_info);

/*!
 * \brief Convert bcmpc port configuration to PM port configuration.
 *
 * \param [in] unit      - Unit number.
 * \param [in] pcfg      - bcmpc port configuration.
 * \param [out] add_info  - PM port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_port_cfg_to_pm_cfg(int unit, bcmpc_port_cfg_t *pcfg,
                              pm_port_add_info_t *add_info);


/*!
 * \brief Convert bcmpc port configuration to PM port speed configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pcfg bcmpc port configuration.
 * \param [out] speed_config PM port speed configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_port_speed_config_to_pm_cfg(int unit, bcmpc_port_cfg_t *pcfg,
                                       pm_speed_config_t *speed_config);

/*!
 * \brief Convert bcmpc port configuration to PM port speed configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] speed_config PM port speed configuration.
 * \param [out] pcfg bcmpc port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_port_pm_cfg_to_speed_config(int unit,
                                       pm_speed_config_t *speed_config,
                                       bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Convert bcmpc auto-negotiation profiles to PM port ability.
 *
 * \param [in] unit Unit number.
 * \param [in] an_prof BCMPC auto-negotiation profile.
 * \param [out] ability PM port ability.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_port_ability_to_pm_ability(int unit, bcmpc_autoneg_prof_t *an_prof,
                                      pm_port_ability_t *ability);

/*!
 * \brief Convert Tx lane taps status from PHYMOD Tx config.
 *
 * \param [in] unit Unit number
 * \param [in] phymod_tx PHYMOD Tx config.
 * \param [out] tx_st Tx taps setting.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_tx_lane_status_from_phy(int unit, phymod_tx_t *phymod_tx,
                                   bcmpc_tx_taps_config_t *tx_st);

/*!
 * \brief Convert Tx lane taps status to PHYMOD Tx config.
 *
 * \param [in] unit Unit number
 * \param [in] tx_st Tx taps setting.
 * \param [out] phymod_tx PHYMOD Tx config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_tx_taps_cfg_to_phy(int unit, bcmpc_tx_taps_config_t *tx_st,
                              phymod_tx_t *phymod_tx);

/*!
 * \brief Convert PHYMOD pmd lane config to pmd firmware lane config.
 *
 * \param [in] unit Unit number
 * \param [in] pmd_lane_cfg PHYMOD pmd lane config.
 * \param [out] fm_cfg PMD firmware lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pmd_lane_config_from_phy(int unit, phymod_firmware_lane_config_t *pmd_lane_cfg,
                                    bcmpc_pmd_firmware_config_t *fm_cfg);

/*!
 * \brief Convert PHYMOD pmd lane config to pmd firmware lane config.
 *
 * \param [in] unit Unit number
 * \param [in] fm_cfg PMD firmware lane config.
 * \param [out] pmd_lane_cfg PHYMOD pmd lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pmd_lane_config_to_phy(int unit, bcmpc_pmd_firmware_config_t *fm_cfg,
                                  phymod_firmware_lane_config_t *pmd_lane_cfg);

/*!
 * \brief Convert BCMPC pfc config to PMAC pfc config.
 *
 * \param [in] unit Unit number
 * \param [in] pctrl BCMPC pfc config.
 * \param [out] pfc_cfg PMAC pfc config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_pfc_cfg_to_pm_cfg(int unit, bcmpc_pfc_control_t *pctrl,
                                bcmpmac_pfc_t *pfc_cfg);

/*!
 * \brief Convert PMAC pfc config to BCMPC pfc config.
 *
 * \param [in] unit Unit number
 * \param [in] pfc_cfg PMAC pfc config.
 * \param [out] pctrl BCMPC pfc config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_pfc_cfg_from_pm_cfg(int unit, bcmpmac_pfc_t *pfc_cfg,
                                bcmpc_pfc_control_t *pctrl);

/*!
 * \brief Convert PM phy status to BCMPC phy status.
 *
 * \param [in] unit Unit number
 * \param [in] pm_phy_st PM phy status.
 * \param [out] pmgr_phy_st bcmpc phy status.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_phy_status_from_pm_phy_status(int unit, pm_phy_status_t *pm_phy_st,
                                         bcmpc_pmgr_phy_status_t *pmgr_phy_st);

/*!
 * \brief Convert BCMPM phy status to PM phy status.
 *
 * \param [in] unit Unit number
 * \param [in] pctrl bcmpc phy control.
 * \param [out] pm_pctrl PM phy control.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_phy_ctrl_to_pm_phy_ctrl(int unit, bcmpc_phy_control_t *pctrl,
                                   pm_phy_control_t *pm_pctrl);

/*!
 * \brief Convert PM port ability to BCMPC.
 *
 * \param [in] unit Unit number.
 * \param [in] num_abilities Number of abilties.
 * \param [in] port_abilities PM port abilities.
 * \param [out] pabilities BCMPC port abilities.
 *
 * \return Nothing.
 */
extern void
pm_port_ability_to_bcmpc_pmgr_ability(int unit, uint32_t num_abilities,
                                      pm_port_ability_t *port_abilities,
                                      bcmpc_pmgr_port_abilities_t *pabilities);

/*!
 * \brief Convert PM vco rate to BCMPC.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_vco_rate PM vco rate.
 * \param [out] vco_rates BCMPC vco rate.
 *
 * \return Nothing.
 */
extern void
pm_vco_rate_to_bcmpc_vco_rate(int unit, pm_vco_t *pm_vco_rate,
                              bcmpc_vco_type_t *vco_rates);

/*!
 * \brief Convert PM Reduce Lane Mode (RLM) status to BCMPC.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_status PM RLM status.
 * \param [out] status BCMPC RLM status.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_FAIL Failure.
 */
extern int
pm_port_rlm_status_to_bcmpc_pmgr_port_rlm_status(int unit,
                                          pm_port_rlm_status_t *pm_status,
                                          bcmpc_pmgr_port_rlm_status_t *status);
/*******************************************************************************
 * Hardware configuration functions for other PC components.
 */

/*!
 * \brief Set the port mode of a PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The first device physical port of the PM.
 * \param [in] mode PM operating mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_core_mode_set(int unit, bcmpc_pport_t pport,
                         bcmpc_pm_mode_t *mode);

/*!
 * \brief Initialize the PHY of the given physical port.
 *
 * This function is used to initialize the PHY of the given physical port.
 *
 * When \c do_core_init is set, this function will initialize the PHY of the
 * port, and also the core where the port belong to.
 *
 * Usually we need do core initialization when the first port of a core is
 * created.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 * \param [in] ccfg Core configuration.
 * \param [in] do_core_init True to do core initialization.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_phy_init(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg,
                    bcmpc_pm_core_cfg_t *ccfg, bool do_core_init);

/*!
 * \brief Get LAG failover status.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] failover_loopback 1 when in lag failover loopback state,
 *                                otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_loopback_get(int unit, bcmpc_pport_t pport,
                                 uint32_t *failover_loopback);

/*!
 * \brief Get LAG failover enable state.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical Port number.
 * \param [in] pport Device physical port.
 * \param [out] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_enable_get(int unit, bcmpc_lport_t lport,
                               bcmpc_pport_t pport,
                               uint32_t *enable);

/*!
 * \brief Enable/Disable LAG failover mode.
 *
 * If LAG failover mode is enabled, the MAC will route all TX packets to the RX
 * path when the link is down. The state is called failover loopback.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical Port number.
 * \param [in] pport Device physical port.
 * \param [in] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_enable_set(int unit, bcmpc_lport_t lport,
                               bcmpc_pport_t pport,
                               uint32_t enable);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * Change the port back to normal operation no matter the port is in failover
 * loopback state or not.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_failover_loopback_remove(int unit, bcmpc_pport_t pport);

/*!
 * \brief Recover the forced link state.
 *
 * This function is used to recover the forced link state when doing
 * warm boot.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_forced_link_state_recover(int unit, bcmpc_pport_t pport,
                                     bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Enable MAC and init phymod core.
 *
 * See \ref bcmpc_pmgr_pm_port_attach.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical Port number.
 * \param [in] pcfg Port configuration.
 * \param [in] ccfg Core configuration.
 * \param [in] enable Value 1 to enable, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_port_attach(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                          bcmpc_pm_core_cfg_t *ccfg, int enable);

/*!
 * \brief PM port configuration validate function.
 *
 * See \ref bcmpc_pmgr_pm_port_config_validate.
 *
 * \param [in] unit Unit number
 * \param [in] pcfg Port configuration information.
 * \param [in] is_first The port is the first port created or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_port_config_validate(int unit, bcmpc_port_cfg_t *pcfg,
                                   int is_first);

/*!
 * \brief PM port auto-negotiation abilities set function.
 *
 * See \ref bcmpc_pmgr_pm_port_autoneg_advert_set.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] an_cfg Autoneg profile configuration array.
 * \param [in] pcfg Port configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_port_autoneg_advert_set(int unit, bcmpc_lport_t lport,
                                      bcmpc_autoneg_prof_t *an_cfg,
                                      bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Mac control set function.
 *
 * See \ref bcmpc_pmgr_mac_control_set.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] mctrl Mac control information.
 * \param [in] default_mctrl Default Mac control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_mac_control_set(int unit, bcmpc_lport_t lport,
                           bcmpc_pport_t pport,
                           bcmpc_mac_control_t *mctrl,
                           bcmpc_mac_control_t default_mctrl);

/*!
 * \brief MAC control get function.
 *
 * See \ref bcmpc_pmgr_mac_control_get.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] mctrl Mac control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_mac_control_get(int unit, bcmpc_lport_t lport,
                          bcmpc_pport_t pport, bcmpc_mac_control_t *mctrl);

/*!
 * \brief Phy control set function.
 *
 * See \ref bcmpc_pmgr_phy_control_set.
 *
 * \param [in] unit Unit number
 * \param [in] port_lane combination of port and lane.
 * \param [in] pport Physical port number.
 * \param [in] pctrl Phy control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_phy_control_set(int unit, bcmpc_port_lane_t *port_lane,
                           bcmpc_pport_t pport, bcmpc_phy_control_t *pctrl);

/*!
 * \brief PFC control set function.
 *
 * See \ref bcmpc_pmgr_mac_control_set.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] pctrl PFC control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pfc_control_set(int unit, bcmpc_lport_t lport,
                          bcmpc_pport_t pport, bcmpc_pfc_control_t *pctrl);

/*!
 * \brief Suspend or resume a port.
 *
 * This function suspends or resumes the traffic on a given port.
 *
 * Port suspend means disabling the port without bringing down the PHY
 * link.
 *
 * If the port is already in the requested suspend/resume state, the
 * function will do nothing and return SHR_E_NONE.
 *
 * If suspend/resume is not supported by the port, the function will
 * return SHR_E_NONE.
 *
 * \param [in] unit Unit number
 * \param [in] pport Physical port number.
 * \param [in] enable Set to true to suspend traffic on this port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_port_suspend_set(int unit, bcmpc_pport_t pport, bool enable);

/*!
 * \brief Initalize port ability data structure.
 *
 * \param [in] pability Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_port_abilities_t_init(bcmpc_pmgr_port_abilities_t *pability);

/*!
 * \brief Get current PM vco rate associated PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] vco_rate Current PM vco rates.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_pm_vco_rate_get(int unit, bcmpc_pport_t pport,
                           bcmpc_vco_type_t *vco_rate);

/*!
 * \brief Convert BCMPC timesync configuration to PM timesync configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] cfg BCMPC timesync configuration.
 * \param [out] ts_cfg PM timesync configuration.
 *
 * \return SHR_E_NONE No errors.
 *         SHR_E_PARAM Invalid parameter.
 */
extern int
bcmpc_pmgr_timesync_to_pm_ts_config(int unit, bcmpc_port_timesync_t *cfg,
                                    pm_phy_timesync_config_t *ts_cfg);

/*!
 * \brief Convert BCMPC timesync configuration to PM SyncE configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] cfg BCMPC timesync configuration.
 * \param [out] synce_cfg PM SyncE configuration.
 *
 * \return SHR_E_NONE No errors.
 *         SHR_E_PARAM Invalid parameter.
 */
extern int
bcmpc_pmgr_timesync_to_pm_synce(int unit, bcmpc_port_timesync_t *cfg,
                                pm_port_synce_clk_ctrl_t *synce_cfg);

/*!
 * \brief PM port sw state update function.
 *
 * \param [in]  unit                 Unit number.
 * \param [in]  pport                physical port
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
extern  int
bcmpc_pmgr_port_sw_state_update(int unit,  bcmpc_pport_t pport);

/*!
 * \brief PM init.
 *
 * \param [in]  unit                 Unit number.
 * \param [in]  pport                physical port
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
extern  int
bcmpc_pmgr_pm_init(int unit,  bcmpc_pport_t pport);

/*!
 * \brief Get Reduce Lane Mode (RLM) status for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [out] status RLM status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
extern int
bcmpc_pmgr_port_rlm_status_get(int unit, bcmpc_pport_t pport,
                               bcmpc_pmgr_port_rlm_status_t *status);

/*!
 * \brief PM SERDES Core initialization.
 *
 * This function invokes the Port Macro driver to initialize and
 * configure the SERDES Core.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical Port number.
 * \param [in] vco_cfg VCO configuration.
 * \param [in] pcfg Logical port configuration.
 * \param [in] ccfg Core configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_pm_core_attach(int unit, bcmpc_pport_t pport,
                          pm_vco_setting_t *vco_cfg,
                          bcmpc_port_cfg_t *pcfg, bcmpc_pm_core_cfg_t *ccfg);

/*!
 * \brief This function determines the VCO rate for the configured ports.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [in] vco_select Port configuration (speed, fec, number of lanes).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
extern int
bcmpc_pmgr_pm_vco_rate_calculate(int unit, bcmpc_pport_t pport,
                                 pm_vco_setting_t *vco_select);

/*!
 * \brief This function populate the PC_PORT_INFOt LT with thei
 * operation status errors.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] op_st Operation status returned for PM driver call.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_oper_status_table_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_pport_t pport,
                                    bcmpc_port_oper_status_t *op_st);

/*!
 * \brief This function enable/disable the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] lport Logical port number.
 * \param [in] enable 1 - Enable, else Disable.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_port_enable_set(int unit, bcmpc_pport_t pport,
                           bcmpc_lport_t lport, uint32_t enable);

/*!
 * \brief This function is used to get the TSC core settings.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [out] ccfg Core configuration data structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
extern int
bcmpc_pmgr_pm_core_config_get(int unit, bcmpc_pport_t pport,
                              bcmpc_pm_core_cfg_t *ccfg);

/*!
 * Set HA link state
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_ha_link_state_set(int unit);

/*!
 * Recover from HA link state
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_link_state_recover(int unit);

/*!
 * Create HA link state buffer
 *
 * \param [in] unit Unit number.
 * \param [in] warm warmboot flag.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_ha_link_state_create(int unit, bool warm);

/*!
 * \brief Initalize port diag stats data structure.
 *
 * \param [in] diag_stats Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pmgr_port_diag_stats_t_init(bcmpc_pmgr_port_diag_stats_t *diag_stats);

/*!
 * \brief Get the diagnostic statistis for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport logical port.
 * \param [out] diag_stats Diagnostic statistics of port
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_diag_stats_get(int unit, bcmpc_lport_t lport,
                              bcmpc_pmgr_port_diag_stats_t *diag_stats);

/*!
 * \brief Set interrupts on port to enable/disable
 *
 * \param [in] unit Unit number.
 * \param [in] lport logical port.
 * \param [in] enable 1 - Enable ECC interrupts on port
 *                    0 - Disable ECC interrupts on port
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_interrupt_set(int unit, bcmpc_lport_t lport,
                         bool enable);


/*!
 * \brief Process interrupts on port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport logical port.
 * \param [in] intr_type Interrupt type to be processed.
 * \param [out] is_handled 1 - Interrupt processing successful.
 *                         0 - Interrupt processing failed.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_pmgr_interrupt_process(int unit, bcmpc_lport_t lport,
                             bcmpc_intr_type_t intr_type,
                             uint32_t *is_handled);

#endif /* BCMPC_PMGR_INTERNAL_H */
