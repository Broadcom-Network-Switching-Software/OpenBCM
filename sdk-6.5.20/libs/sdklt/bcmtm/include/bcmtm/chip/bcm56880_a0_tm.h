/*! \file bcm56880_a0_tm.h
 *
 * File containing chip related defines and internal functions for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_H
#define BCM56880_A0_TM_H

#include <bcmpc/bcmpc_tm.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>

#define TD4_NUM_PHYS_PORTS           267
#define TD4_PORTS_PER_PBLK           8
#define TD4_PBLKS_PER_PIPE           4
#define TD4_PIPES_PER_DEV            8

#define TD4_GPHY_PORTS_PER_PIPE      (TD4_PORTS_PER_PBLK * TD4_PBLKS_PER_PIPE)
#define TD4_GPHY_PORTS_PER_DEV       (TD4_GPHY_PORTS_PER_PIPE * TD4_PIPES_PER_DEV)
#define TD4_PHY_PORTS_PER_PIPE       (TD4_GPHY_PORTS_PER_PIPE + 2)
#define TD4_ITMS_PER_DEV             2

#define TD4_PBLKS_PER_DEV            (TD4_PBLKS_PER_PIPE * TD4_PIPES_PER_DEV)
#define TD4_PORTS_PER_PIPE           (TD4_PORTS_PER_PBLK * TD4_PBLKS_PER_PIPE)
#define TD4_PORTS_PER_DEV            (TD4_PORTS_PER_PIPE * TD4_PIPES_PER_DEV)
#define TD4_PHY_PORTS_PER_DEV        (TD4_PHY_PORTS_PER_PIPE * TD4_PIPES_PER_DEV)
#define TD4_NUM_CPU_PORTS            1
#define TD4_NUM_LB_PORTS             4
#define TD4_NUM_SPARE_PORTS          7
#define TD4_NUM_MGMT_PORTS           4
#define TD4_PHYS_PORTS_PER_PIPE      32
#define TD4_DEV_PORTS_PER_PIPE       20
#define TD4_DEV_PORTS_PER_DEV        (TD4_DEV_PORTS_PER_PIPE * TD4_PIPES_PER_DEV)
#define TD4_MMU_PORTS_PER_PIPE       32
#define TD4_MGMT_LOCAL_PHYS_PORT     32
#define TD4_FP_PORTS_PER_PIPE        18
#define TD4_LB_LOCAL_PHYS_PORT       33
#define PIPE_RESET_HW_TIMEOUT_MSEC   50
#define PIPE_RESET_EMUL_TIMEOUT_MSEC 10000
#define EMULATION_FACTOR             10000
#define TD4_NUM_GP_QUEUES  12
#define TD4_TM_CHIP_QUEUES 1920
#define TD4_MAX_NUM_FP_PORTS 144
#define TD4_MAX_NUM_FP_PORTS_PIPE 18
#define TD4_NUM_LANES_AUX_PORT 1
#define TD4_MAX_SERVICE_POOLS 4

/*!
 * \brief Chip-specific initialization
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Hardware table not found or DPR frequency range check failed.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56880_a0_tm_chip_init(int unit, bool warm);

/*!
 * \Brief Initialize mmu thresholds for THDI/THDO.
 * Setup mmu threshold related mapping profile.
 * Setup mmu threshold for THDI/THDO.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56880_a0_tm_mmu_config_init_thresholds(int unit);

/*!
 * \brief Initialize mmu thresholds for THDR.
 * Setup mmu threshold for THDR.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56880_a0_tm_rqe_threshold_init(int unit);

/*!
 * \brief number of unicast queue for front panel port for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56880_a0_tm_num_mc_q_non_cpu_port_get(int unit);

/*!

 * \brief number of multicast queue for front panel ports for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56880_a0_tm_num_uc_q_non_cpu_port_get(int unit);

/*!
 * \brief check if the specified ITM is valid for the current chip mode.
 *
 * In half-chip mode, one of the ITMs can be invalid. This check needs to be
 * performed whenever per-ITM reg/mem is programmed.
 *
 * \param [in] unit unit number.
 * \param [in] itm ITM number.
 *
 * \retval SHR_E_NONE itm id valid.
 * \retval SHR_E_INTERNAL itm is invalid.
 */
extern int
bcm56880_a0_tm_check_valid_itm(int unit, int itm);

/*!
 * \brief IMM Initialization functions for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56880_a0_tm_imm_init(int unit);

/*!
 * \brief IMM pre-population functions for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56880_a0_tm_imm_populate(int unit);

/*!
 * \brief IMM update functions for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56880_a0_tm_imm_update(int unit, int pport, int up);

/*!
 * \brief Chip-level IDB init function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56880_a0_tm_idb_general_init(int unit);

/*!
 * \brief Port-level IDB flexport port-down function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56880_a0_tm_port_idb_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level IDB flexport port-up function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56880_a0_tm_port_idb_up(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level MMU flexport port-down function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_tm_port_mmu_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level MMU flexport port-up function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_tm_port_mmu_up(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level EDB flexport port-down function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56880_a0_tm_port_edb_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level EDB flexport port-up function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56880_a0_tm_port_edb_up(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level MMU TDM init function for bcm56880_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_tm_tdm_mmu_init(int unit);


/*!
 * \brief Get the port, mmu_q number from chip queue number.
 *
 * \param [in] unit unit number.
 * \param [in] chip_q Chip queue number.
 * \param [out] lport Logical port ID.
 * \param [out] mmu_q MMU queue ID.
 * \param [out] q_type Queue type.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56880_a0_tm_mmu_q_from_chip_q_get(int unit, int chip_q,
                        int *lport, uint32_t *mmu_q,
                        bcmtm_q_type_t *q_type);

/*!

 * \brief Get the chip queue number from port and mmu queue.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical port ID.
 * \param [in] mmu_q MMU queue ID.
 * \param [out] chip_q Chip queue number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
int
bcm56880_a0_tm_chip_q_get(int unit, uint32_t lport,
                          uint32_t mmu_q, uint32_t *chip_q);

/*!
 * \brief New port add/configure in MMU for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_port_mmu_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief New port add/configure in IDB for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_port_idb_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief New port add/configure in EDB for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_port_edb_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port delete in MMU for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_port_mmu_delete(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port delete in IDB for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_port_idb_delete(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port delete in EDB for bcm56880_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56880_a0_tm_port_edb_delete(int unit, bcmtm_pport_t pport);

/*!
 * \brief Enable IP portbitmap for link up port.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_ip_port_forwarding_bitmap_enable(int unit, bcmtm_pport_t pport);

/*!
 * \brief Disable IP portbitmap for link up port.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_ip_port_forwarding_bitmap_disable(int unit, bcmtm_pport_t pport);

/*!
 * \brief Flexport configuration validate.
 *
 * \param [in] unit unit number.
 * \param [in] num_ports Array depth.
 * \param [in] old_cfg Old MMU port config.
 * \param [in] new_cfg New MMU port config.
 *
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56880_a0_tm_validate (int unit,
                            size_t num_ports,
                            const bcmpc_mmu_port_cfg_t *old_cfg,
                            const bcmpc_mmu_port_cfg_t *new_cfg);


/*!
 * \brief Clear EDB credit counter before port up.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_tm_edb_credit_reset(int unit, bcmtm_pport_t pport);


/*!
 * \brief Function to fill port_sched_state_resource structure
 *
 * Retrieve all information contained in bcmpc_mmu_port_cfg_t and fill it into
 * the port_schedule_state field in tm_drv_info. The bcmpc_mmu_port_cfg_t is
 * generated by BCMPC and contains all the necessary port config info.
 *
 * \param [in] unit Logical unit number.
 * \param [in] num_ports Number of ports
 * \param [in] old_cfg Old port configuration indexed by physical ports.
 * \param [in] new_cfg New port configuration indexed by physical ports.
 */
extern void
bcm56880_a0_tm_drv_info_portmap_fill(int unit,
                                     size_t num_ports,
                                     const bcmpc_mmu_port_cfg_t *old_cfg,
                                     const bcmpc_mmu_port_cfg_t *new_cfg);


/*!
 * \brief Add a port in TM.
 *
 * \param [in] unit unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_tm_port_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief Delete a port in TM.
 *
 * \param [in] unit unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56880_a0_tm_port_delete(int unit, bcmtm_pport_t pport);

/*!
 * \breif  Update unicast drop config in IP.

 * @param [in] unit Device number
 * @param [in] pport Physical port number
 * @param [in] lport Logical port number
 * @param [in] add_flag Flag indicating port add or delete sequence.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56880_a0_tm_update_unicast_drop_config(int unit,
                                          bcmtm_pport_t pport,
                                          int lport,
                                          bool flag);
/*!
 * \brief Check credit loop between EP and MMU.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical Port Number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PORT Invalid logical port.
 * \retval SHR_E_FAIL Request credit mismatch observed.
 */
extern int
bcm56880_a0_tm_check_ep_mmu_credit(int unit, int lport);

/*!
 * \brief Check credit loop between Port and EP.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical Port Number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PORT Invalid logical port.
 * \retval SHR_E_FAIL Request credit mismatch observed of function called for
 *                    internal port.
 */
extern int
bcm56880_a0_tm_check_port_ep_credit(int unit, int lport);


/*!
 * \brief Get logical port from mmu chip port.
 *
 * \param [in] unit Unit number.
 * \param [in] mchip_port MMU chip port number.
 * \param [out] lport Logical port ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL port mapping unavailable.
 */
extern int
bcm56880_a0_tm_mchip_port_lport_get(int unit, int mchip_port, int *lport);

#endif /* BCM56880_A0_TM_H */
