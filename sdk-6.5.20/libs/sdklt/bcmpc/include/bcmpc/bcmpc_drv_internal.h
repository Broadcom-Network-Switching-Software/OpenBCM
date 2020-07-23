/*! \file bcmpc_drv_internal.h
 *
 * BCMPC Driver Object.
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BCMBD) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_DRV_INTERNAL_H
#define BCMPC_DRV_INTERNAL_H

#include <sal/sal_types.h>
#include <phymod/phymod.h>
#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_pm_drv_types.h>
#include <bcmpc/bcmpc_fdr.h>
#include <bcmpc/bcmpc_tm.h>

/*!
 * \brief BCMPC user access data for PMAC access bus.
 *
 * This structure is used as \c user_acc of \c bcmpmac_access_t.
 *
 * PMAC address only contains the information of register offset, register type
 * and access type. For CMICM/CMICD S-Channel operations, besides PMAC address,
 * it requires unit and block number information as well.
 *
 * This structure stores the extra data according to the host CPU interface to
 * construct the hardware dependent address.
 */
typedef struct bcmpc_pmac_access_data_s {

    /*! Unit number. */
    int unit;

    /*! Block number. */
    int blk;

} bcmpc_pmac_access_data_t;

/*!
 * \brief BCMPC configuration profile.
 *
 * Some port configurations e.g. port up/down are device-specific and are
 * complex in the sense that they involve access to the ingress/egress pipelines
 * , the MMU, the port block, the MAC and the SerDes.
 *
 * This structure is used to define the chip specific programming sequence for
 * some events e.g. port up/down.
 *
 * The entire programming sequence is defined as an array of operations \c ops.
 */
typedef struct bcmpc_cfg_profile_s {

    /*! The count of operations. */
    int op_cnt;

    /*! Operations array. */
    bcmpc_operation_t *ops;

} bcmpc_cfg_profile_t;

/*!
 * \brief BCMPC port abilities.
 *
 *  This structure is used to store port abilities.
 */
typedef struct bcmpc_dev_ability_entry_s {

    /*! Port speed in Mbps. */
    uint32_t speed;

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! FEC type. */
    pm_port_fec_t fec_type;

} bcmpc_dev_ability_entry_t;


/*******************************************************************************
 * PC driver functions
 */

/*!
 * \name PC driver functions
 *
 * The function prototypes for \ref bcmpc_drv_t.
 */

/*! \{ */

/*!
 * \brief Device-specific topology initialization.
 *
 * This function will be called when SYSM starts the PC component. It is used to
 * initialize the chip-specific topology, i.e. PM types and instances
 * population. It should be a pure software operation for writing the topology
 * information into the database.
 *
 * This function should enforce PM instance order according to the following:
 * 1. CPU ports (if any)
 * 2. Primary ports (front panel ports, e.g. the gigabit ports)
 * 3. Special ports (up-links)
 * 4. Management ports (if any)
 * 5. Internal ports (loopback ports, etc.)
 *
 * \param [in] unit Unit number.
 * \param [in] warm True for warm boot, otherwise cold boot.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_topo_init_f)(int unit, bool warm);

/*! Download firmware before ports being configured. */
#define BCMPC_PRE_FW_LOAD   1

/*!
 * \brief Device-specific port initialization.
 *
 * This function will be called when SYSM starts the PC component. It is used to
 * initialize the chip-specific port configurations. it could include software
 * and hardware operations, ex. internal ports population and PHY firmware
 * download.
 *
 * \param [in] unit Unit number.
 * \param [in] pre_load Indicates if download firmware before ports configured.
 * \param [in] warm Set for warm boot, otherwise cold boot.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_dev_init_f)(int unit, bool pre_load, bool warm);

/*!
 * \brief Device-specific cleanup.
 *
 * This function will be called when SYSM stops the PC component. It is used to
 * cleanup the chip-specific software resources.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_dev_cleanup_f)(int unit);

/*!
 * \brief PHY MDIO address retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PHY address of \c pport if no errors, otherwise -1.
 */
typedef int
(*bcmpc_phy_addr_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port block information retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] blk_num Returned block number.
 * \param [out] blk_port Returned block port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Failed to get the port-block information of \c pport.
 */
typedef int
(*bcmpc_pblk_get_f)(int unit, bcmpc_pport_t pport, int *blk_num, int *blk_port);

/*!
 * \brief PHY bus driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PHYMOD bus driver of \c pport if no errors, otherwise NULL.
 */
typedef phymod_bus_t *
(*bcmpc_phy_bus_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PMAC bus driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PMAC bus driver of \c pport if no errors, otherwise NULL.
 */
typedef bcmpmac_reg_access_t *
(*bcmpc_pmac_bus_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PHY device driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PHYMOD dispatch type of \c pport if no errors,
 *         otherwise \c phymodDispatchTypeCount.
 */
typedef phymod_dispatch_type_t
(*bcmpc_phy_drv_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PMAC device driver retrieval function pointer type.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The PMAC driver of \c pport if no errors, otherwise NULL.
 */
typedef bcmpmac_drv_t *
(*bcmpc_pmac_drv_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief PHY PLL types in PortMacro.
 */
typedef enum bcmpc_pll_type_e {
    BCMPC_PLL_TYPE_DEFAULT,
    BCMPC_PLL_TYPE_TVCO
} bcmpc_pll_type_t;

/*!
 * \brief PHY PLL index retrieval function pointer type.
 *
 * This function is used to get the PLL index of the given PLL type for
 * the multi-PLLs PortMacro.
 *
 * Take PM8x50 for example, A PM may have two PLLs, one is TVCO and the other one is
 * OVCO. The hardware design will decide whether PLL 0 or 1 is used as the TVCO
 * source.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pll_type PLL type.
 *
 * \return The PLL index for the \c pll_type.
 */
typedef uint8_t
(*bcmpc_phy_pll_idx_get_f)(int unit, bcmpc_pport_t pport,
                           bcmpc_pll_type_t pll_type);

/*!
 * \brief Port up programming sequence retrieval function pointer type.
 *
 * To get the port up programming sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The port up profile of \c pport if no errors, otherwise NULL.
 */
typedef bcmpc_cfg_profile_t *
(*bcmpc_port_up_profile_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port down programming sequence retrieval function pointer type.
 *
 * To get the port down programming sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The port down profile of \c pport if no errors, otherwise NULL.
 */
typedef bcmpc_cfg_profile_t *
(*bcmpc_port_down_profile_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port suspend programming sequence retrieval function pointer type.
 *
 * To get the port suspend programming sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The port suspend profile of \c pport if no errors, otherwise NULL.
 */
typedef bcmpc_cfg_profile_t *
(*bcmpc_port_suspend_profile_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Port resume programming sequence retrieval function pointer type.
 *
 * To get the port resume programming sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The port resume profile of \c pport if no errors, otherwise NULL.
 */
typedef bcmpc_cfg_profile_t *
(*bcmpc_port_resume_profile_get_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Execute operations of a port configuration operation.
 *
 * All operations except for e.g. the MMU operations are owned by the Port
 * Control framework (BCMPC + BCMPMAC + PHYMOD). The MMU operations are owned by
 * the Traffic Manager (BCMTM) eventually.
 *
 * This is a temporary function for execute the port configuration operation
 * which are not handled by PC. And will be removed when the other module are
 * ready.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op Operation.
 */
typedef int
(*bcmpc_op_exec_f)(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op);

/*!
 * \brief Oversub port bit map get function pointer type.
 *
 * This function is used to get the oversub port bit map.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] lport Logical port number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_oversub_pbmp_get_f)(int unit, bcmpc_pbmp_t *pbmp);

/*!
 * \brief Get PM physical port.
 *
 * Used for override the PM physical port from standard calculation.
 *
 * A typical use case is the device uses arbitrary map for the device physical
 * ports and the PM ports.
 *
 * For example, suppose that the starting physical port of a PM is 257, with
 * the standard caculation, we will treat the PM port of physical port 258 is 1
 * (= 258 - 257).
 * For some device, the physical port to the PM port map is arbitrary,
 * e.g. physical port 257 is PM port 0 but physical port 258 is PM port 2.
 *
 * When a devcie needs such special calculation, this function need to be
 * implemented. If the given \c pport does not require any special caculation
 * to get the PM port, \c pm_pport should not be modified inside the function.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port number.
 * \param [out] pm_pport Physical PM port number.
 */
typedef void
(*bcmpc_pm_pport_get_f)(int unit, bcmpc_pport_t pport, int *pm_pport);

/*
 * \brief Get lane bitmap for a logical port.
 *
 * Used for override the lane bitmap from standard calculation.
 *
 * A typical use case is the device uses arbitrary map for the device physical
 * ports and the PM ports.
 *
 * For example, suppose that the starting physical port of a PM is 5, with the
 * standard caculation, we will treat physical port 6 lane bitmap as 0x2 on the
 * PM.
 *
 * However, for some devices, one physical port can map to multiple physical
 * SerDes lanes. For those cases, the lane bitmap will be calculated
 * differently.
 *
 * For example, if the PM has 8 SerDes lanes. While
 * only 4 physical port is allocted to the PM. In this case, suppose the
 * starting physical port of a PM is 5, then the lane bitmap for physical port 6
 * is 0x4(one-lane port) or 0xc(two-lane port).
 * When a devcie needs such special calculation, this function need to be
 * implemented. If the device does not require any special caculation
 * to get the PM port, \c lbmp should not be modified inside the function.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port number.
 * \param [in] pm_phys_port Physical port offset within the PM.
 * \param [inout] lbmp Lane bitmap of the port. Lane bitmap input to the
 *                     function only set number of bits based on the SerDes
 *                     lanes of the port without any shift. This function will
 *                     shift the lane bitmap according to the port offset within
 *                     the PM.
 */
typedef void
(*bcmpc_port_lbmp_get_f)(int unit,
                         bcmpc_pport_t pport,
                         int pm_phys_port,
                         uint32_t *lbmp);

/*!
 * \brief Firmware download function get.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port number.
 * \param [in] fw_loader_req Request firmware firmware loader or not.
 * \param [out] fw_loader firmware download function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pm_firmware_loader_f)(int unit, bcmpc_pport_t pport,
                              uint32_t fw_loader_req,
                              phymod_firmware_loader_f *fw_loader);

/*!
 * \brief Raw phy register read.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Base port number of the PM.
 * \param [in] reg_addr TSC 32-bit register address.
 * \param [out] val The value read from the register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pm_phy_raw_read_f)(int unit, bcmpc_pport_t base_port,
                           uint32_t reg_addr, uint32_t *val);
/*!
 * \brief Raw phy register write.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Base port number of the PM.
 * \param [in] reg_addr TSC 32-bit register address.
 * \param [out] val The value to write to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pm_phy_raw_write_f)(int unit, bcmpc_pport_t base_port,
                            uint32_t reg_addr, uint32_t val);

/*!
 * \brief This function returns if a port is internal or not.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Base port number of the PM.
 * \param [out] is_internal True if internal port, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_is_internal_port_f)(int unit, bcmpc_pport_t base_port,
                            bool *is_internal);

/*!
 * \brief This function returns number of physical ports for
 * given configured port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port number.
 * \param [in] pmd_num_lane PMD number of lane on the port.
 * \param [out] physical_port_num Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_num_phys_ports_get_f)(int unit, bcmpc_pport_t port, int pmd_num_lane,
                              int *phyiscal_port_num);

/*!
 * \brief This function returns maximum number of logical and physical ports
 * supported on a device.
 *
 * \param [in] unit Unit number.
 * \param [out] max_pport Maximum number of physical ports supported
 * \param [out] max_lport Maximum number of logical ports supported
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmpc_max_num_ports_get_f)(int unit, uint32_t *max_pport,
                             uint32_t *max_lport);

/*!
 * \brief This function returns if a Port Macro is internal or not.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Port Macro id.
 * \param [out] is_internal True if internal Port Macro type, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_is_internal_pm_f)(int unit, int pm_id, bool *is_internal);

/*!
 * \brief This function returns port abilities that device doesn't support
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port number.
 * \param [out] size port unsupported ability entry size.
 *
 * \retval pointer to the struct of dev_ability_entry_t.
 */
typedef const bcmpc_dev_ability_entry_t *
(*bcmpc_dev_unsupported_abilities_get_f)(int unit, bcmpc_pport_t pport,
                                         int *size);


/*!
 * \brief This function returns if a port need to be skipped for.
 * PM instance creation
 *
 * \param [in] unit Unit number.
 * \param [in] pport Base port number of the PM.
 * \param [out] is_skip True if skip, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_skip_pm_instance_f)(int unit, bcmpc_pport_t base_port,
                            bool *is_skip);

/*!
 * \brief This function returns if phymod core init pass1 pport needs to
 * be adjusted
 *
 * \param [in] unit Unit number.
 * \param [in/out] pm_acc
 * \param [out] is_true True if need to run twice, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_phymod_core_init_pass1_pport_adjust_f) (int unit, pm_access_t *pm_acc,
                                         bool *is_true);


/*!
 * \brief This function returns if the Port Macro that the physical port
 * belongs to consists of two PMLs or not.
 *
 * \param [in] unit Unit number.
 * \param [in] pport of the PM.
 * \param [out] is_true True if support dual PMLs, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_is_dual_pml_supported_f) (int unit, bcmpc_pport_t pport, bool *is_true);


/*!
 * \brief This function returns if SEC module is supported on the device.
 *
 * \param [in] unit Unit number.
 * \param [out] True SEC module is supported on the device
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_is_sec_supported_f)(int unit);

/*!
 * \brief This function returns SEC is enabled on the PM
 *
 * \param [in] unit Unit number.
 * \param [out] True SEC is enabled on the PM
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_is_pm_sec_enabled_f)(int unit, int pm_id);


/*!
 * \brief This function returns if phymod core init pass2 pport needs to
 * be adjusted to using U1
 *
 * \param [in] unit Unit number.
 * \param [in/out] pm_acc
 * \param [out] is_true True if need to run twice, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_phymod_core_init_pass2_u1_adjust_f) (int unit, pm_access_t *pm_acc);

/*!
 * \brief This function returns if phymod core init pass2 pport needs to
 * be adjusted to using U0
 *
 * \param [in] unit Unit number.
 * \param [in/out] pm_acc
 * \param [out] is_true True if need to run twice, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_phymod_core_init_pass2_u0_adjust_f) (int unit, pm_access_t *pm_acc);

/*!
 * \brief This function returns the base physical port for the PML within a
 * PM based on pml_sel /c.
 *
 * \param [in] unit Unit number.
 * \param [in] base_pport Base physical port number of the PM.
 * \param [in] pml_sel PML number within the PM.
 * \param [out] base_pport_adjust Base physical port number of the PML.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pm_base_pport_adjust_f)(int unit, bcmpc_pport_t base_pport,
                                uint8_t pml_sel,
                                bcmpc_pport_t *base_pport_adjust);

/*!
 * \brief This Function to return the worst case timeout value to be
 * used during link UP/DOWN sequence execution.
 *
 * \param [in] unit Unit number.
 * \param [out] timeout_up Workst case timeout value in usecs for link up.
 * \param [out] timeout_down Workst case timeout value in usecs for link down.
 */
typedef int
(*bcmpc_link_sequence_exec_timeout_get_f) (int unit, uint32_t *to_up,
                                           uint32_t *to_down);

/*!
 * \brief This function returns top device revision ID A0.
 *
 * \param [in] unit Unit number.
 * \param [out] rev_id_a0 Top device revision ID A0.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_top_dev_revision_a0_get_f) (int unit,  bool *rev_id_a0);

/*!
 * \brief This Function returns if the port supports stall transmission
 * in MAC>
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] is_supported ! - indicates supported else, not supported.
 */
typedef int
(*bcmpc_stall_tx_ability_get_f) (int unit, bcmpc_pport_t pport,
                                 bool *is_supported);

/*!
 * \brief This function add PHYSIM instances for different PM type based on the
 * chip special requirement.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pm_physim_add_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief This Function translates the global port based Receive Statistics
 *        Vector (RSV) to MAC (CDMAC or XLMAC) RSV mask.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] in_flags Bitmask to indicate the port RSV mask control.
 * \param [in] in_value Bitmask to indicate the value of port RSV control.
 * \param [out] out_flags Bitmask to indicate the MAC RSV mask control.
 * \param [out] out_value Bitmask to indicate the value of MAC RSV control.
 */
typedef int
(*bcmpc_translate_port_rsv_to_mac_rsv_f) (int unit, bcmpc_pport_t pport,
                                    uint32_t in_flags, uint32_t in_value,
                                    uint32_t *out_flags, uint32_t *out_value);

/*!
 * \brief This Function translates the MAC based Receive Statistics
 *        Vector (RSV) to global port based RSV mask.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] in_flags Bitmask to indicate the MAC RSV mask control.
 * \param [in] in_value Bitmask to indicate the value of MAC RSV control.
 * \param [out] out_flags Bitmask to indicate the port RSV mask control.
 * \param [out] out_value Bitmask to indicate the value of port RSV control.
 */
typedef int
(*bcmpc_translate_mac_rsv_to_port_rsv_f) (int unit, bcmpc_pport_t pport,
                                    uint32_t in_flags, uint32_t in_value,
                                    uint32_t *out_flags, uint32_t *out_value);

/*!
 * \brief Device-specific imm initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_imm_init_f) (int unit);

/*!
 * \brief Device-specific port configuration.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_port_config_apply_f) (int unit);

/*!
 * \brief Get FDR enable status on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] fdr_ctrl FDR control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_port_fdr_enable_get_f)(int unit, bcmpc_pport_t pport,
                               bcmpc_fdr_port_control_t *fdr_ctrl);

/*!
 * \brief This function enables/disables FDR feature on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] fdr_ctrl FDR control information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_port_fdr_enable_set_f)(int unit, bcmpc_pport_t pport,
                               bcmpc_fdr_port_control_t fdr_ctrl);

/*!
 * \brief This function retrieves the FDR statistics.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] fdr_stats FDR statistics information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_port_fdr_stats_get_f)(int unit, bcmpc_pport_t pport,
                               bcmpc_fdr_port_stats_t *fdr_stats);

/*!
 * \brief Initialize auxiliary funtions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_aux_function_init_f) (int unit, bool warm);

/*!
 * \brief Cleanup auxiliary functions.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_aux_function_cleanup_f) (int unit);

typedef int
(*bcmpc_mmu_pcfg_array_complement_f)(int unit, size_t num_ports,
                                     bcmpc_mmu_port_cfg_t **mmu_cfg);


/*!
 * \brief Get CMIC to Port Macro staging delay.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Port macro(PM) ID.
 * \param [out] pm_offset Staging delay from CMIC to the PM in nanosecond.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pm_offset_get_f)(int unit, int pm_id, uint32_t *pm_offset);

/*!
 * \brief This function returns if a port is internal aux port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Base port number of the PM.
 * \param [out] is_aux_port True if internal aux port, else false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_is_internal_aux_port_f)(int unit, bcmpc_pport_t base_port,
                                bool *is_aux_port);
/*!
 * \brief This function enables/disables interrupts on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable Flag that tells whether to enable/disable interrupts
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_port_interrupt_set_f)(int unit, bcmpc_pport_t pport,
                              bool enable);


/*! \} */

/*******************************************************************************
 * PC driver object
 */

/*!
 * \brief Port Control driver object
 *
 * Port Control driver is used to provide the chip-specific information.
 *
 * The chip specific information includes e.g.
 * 1. The port block number and PHY address.
 * 2. The driver and access bus of PHY/PMAC.
 * 3. The programming sequence for complex port configuration e.g. port up/down.
 * 4. The linkscan mode.
 *
 * The chip-specific PC driver is attached during BCMPC initialization
 * \ref bcmpc_attach().
 */
typedef struct bcmpc_drv_s {

    /*! Initialize device-specific topology. */
    bcmpc_topo_init_f topology_init;

    /*! Initialize device-specific port settings. */
    bcmpc_dev_init_f dev_init;

    /*! Cleanup device-specific resources. */
    bcmpc_dev_cleanup_f dev_cleanup;

    /*! Get PHY MDIO address. */
    bcmpc_phy_addr_get_f phy_addr_get;

    /*! Get port block information. */
    bcmpc_pblk_get_f pblk_get;

    /*! Get PHY Bus Driver. */
    bcmpc_phy_bus_get_f phy_bus_get;

    /*! Get PMAC Bus Driver. */
    bcmpc_pmac_bus_get_f pmac_bus_get;

    /*! Get PHY Driver. */
    bcmpc_phy_drv_get_f phy_drv_get;

    /*! Get PMAC Driver. */
    bcmpc_pmac_drv_get_f pmac_drv_get;

    /*! Get the PLL index. */
    bcmpc_phy_pll_idx_get_f phy_pll_idx_get;

    /*! Get Port up programming sequence. */
    bcmpc_port_up_profile_get_f port_up_prof_get;

    /*! Get Port down programming sequence. */
    bcmpc_port_down_profile_get_f port_down_prof_get;

    /*! Get Port suspend programming sequence. */
    bcmpc_port_suspend_profile_get_f port_suspend_prof_get;

    /*! Get Port resume programming sequence. */
    bcmpc_port_resume_profile_get_f port_resume_prof_get;

    /*! Get the oversub port bit map. */
    bcmpc_oversub_pbmp_get_f oversub_pbmp_get;

    /*! Get the PM physical port. */
    bcmpc_pm_pport_get_f pm_pport_get;

    /*! Get lane bitmap for a port. */
    bcmpc_port_lbmp_get_f port_lbmp_get;

    /*! Serdes firmware download. */
    bcmpc_pm_firmware_loader_f pm_firmware_loader;

    /*! Phy raw register read. */
    bcmpc_pm_phy_raw_read_f pm_phy_raw_read;

    /*! Phy raw register read. */
    bcmpc_pm_phy_raw_write_f pm_phy_raw_write;

    /*! Function to determine if the physical port is internal. */
    bcmpc_is_internal_port_f is_internal_port;

    /*! Function to return number of physical ports for a given port. */
    bcmpc_num_phys_ports_get_f num_phys_ports_get;

    /*!
     * Function to return number of maximum number of logical
     * and physical ports.
     */
    bcmpc_max_num_ports_get_f max_num_ports_get;

    /*! Function to determine if the Port Macro type is internal. */
    bcmpc_is_internal_pm_f is_internal_pm;

    /*! Function to return unsupported port abilities. */
    bcmpc_dev_unsupported_abilities_get_f dev_unsupported_abilities_get;

    /*! Function to determine if the pm instance needs to be skipped. */
    bcmpc_skip_pm_instance_f skip_pm_instance;

    /*! Function to determine if the phymod core init pass1 needs to adjust the pport */
    bcmpc_phymod_core_init_pass1_pport_adjust_f phymod_core_init_pass1_pport_adjust;

    /*! Function to determine if the port macro consists of two PMLs. */
    bcmpc_is_dual_pml_supported_f is_dual_pml_supported;

    /*! Function to return */
    bcmpc_phymod_core_init_pass2_u1_adjust_f phymod_core_init_pass2_u1_adjust;

    /*! Function to return */
    bcmpc_phymod_core_init_pass2_u0_adjust_f phymod_core_init_pass2_u0_adjust;

    /*! Function to return base physical port for a given PML within the PM. */
    bcmpc_pm_base_pport_adjust_f pm_base_pport_adjust;

    /*!
     * Function to return the worst case timeout value to be used during
     * link UP/DOWN sequence execution.
     */
    bcmpc_link_sequence_exec_timeout_get_f link_sequence_exec_timeout_get;

    /*! Function to return device revision a0. */
    bcmpc_top_dev_revision_a0_get_f dev_revision_a0_get;

    /*! Function to if the pport supports stall transmission in MAC feature. */
    bcmpc_stall_tx_ability_get_f is_stall_tx_supported;

    /*! Function to determine if SEC module is valid on the device. */
    bcmpc_is_sec_supported_f is_sec_supported;

    /*! Function to determine if SEC is enabled on the Port Macro. */
    bcmpc_is_pm_sec_enabled_f is_pm_sec_enabled;

    /*! Function to add PHYSIM instance. */
    bcmpc_pm_physim_add_f pm_physim_add;

    /*! Function to translate the port RSV mask to MAC RSV mask. */
    bcmpc_translate_port_rsv_to_mac_rsv_f port_to_mac_rsv;

    /*! Function to translate the MAC RSV mask to port RSV mask. */
    bcmpc_translate_mac_rsv_to_port_rsv_f mac_to_port_rsv;

    /*! Function to initialize device-specific IMM interface. */
    bcmpc_imm_init_f imm_init;

    /*! Function to config ports. */
    bcmpc_port_config_apply_f port_config_apply;

    /*! Function to get FDR enable status on a port. */
    bcmpc_port_fdr_enable_get_f fdr_control_get;

    /*! Function to enable FDR on a port. */
    bcmpc_port_fdr_enable_set_f fdr_control_set;

    /*! Function to get FDR stats on a port. */
    bcmpc_port_fdr_stats_get_f fdr_stats_get;

    /*! Function to initialize auxiliary functions. */
    bcmpc_aux_function_init_f aux_func_init;

    /*! Cleanup auxiliary functions. */
    bcmpc_aux_function_cleanup_f aux_func_cleanup;

    /*! Function to complement mmu port configuration array. */
    bcmpc_mmu_pcfg_array_complement_f mmu_pcfg_array;

    /*! Function to get CMIC to port macro delay for a given port macro. */
    bcmpc_pm_offset_get_f pm_offset_get;


    /*! Function to determine if the physical port is internal aux port. */
    bcmpc_is_internal_aux_port_f is_internal_aux_port;

    /*! Function to enable/disable interrupts on a port */
    bcmpc_port_interrupt_set_f interrupt_set;


} bcmpc_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_pc_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize PC device driver.
 *
 * Install PC driver functions and initialize device features.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmpc_attach(int unit);

/*!
 * \brief Clean up PC driver.
 *
 * Release any resources allocated by \ref bcmpc_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmpc_detach(int unit);

#endif /* BCMPC_DRV_INTERNAL_H */
