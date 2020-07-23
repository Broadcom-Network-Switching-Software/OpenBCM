/*! \file bcm56996_a0_tm.h
 *
 * File containing chip related defines and internal functions for bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_TM_H
#define BCM56996_A0_TM_H

#include <bcmpc/bcmpc_tm.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>


/*! TH4G physical port number. */
#define TH4G_NUM_PHYS_PORTS              272
/*! TH4G per PBLK port number. */
#define TH4G_PORTS_PER_PBLK              4
/*! TH4G per pipe PBLK number. */
#define TH4G_PBLKS_PER_PIPE              4
/*! TH4G per device port number. */
#define TH4G_DEV_PORTS_PER_DEV           272
/*! TH4G per device pipe number. */
#define TH4G_PIPES_PER_DEV               16
/*! TH4G per device itm number. */
#define TH4G_ITMS_PER_DEV                2
/*! TH4G per device IDB number. */
#define TH4G_IDBS_PER_DEV                TH4G_PIPES_PER_DEV

/*! TH4G per device PBLK number. */
#define TH4G_PBLKS_PER_DEV               (TH4G_PBLKS_PER_PIPE * TH4G_PIPES_PER_DEV)
/*! TH4G cpu port number. */
#define TH4G_NUM_CPU_PORTS               1
/*! TH4G cpu port ID. */
#define TH4G_CPU_LPORT                   0
/*! TH4G loopback port number. */
#define TH4G_NUM_LB_PORTS                8
/*! TH4G management port number. */
#define TH4G_NUM_MGMT_PORTS              2
/*! TH4G spare port number. */
#define TH4G_NUM_SPARE_PORTS             5
/*! TH4G per pipe physical port number. */
#define TH4G_PHYS_PORTS_PER_PIPE         16
/*! TH4G per pipe device port number. */
#define TH4G_DEV_PORTS_PER_PIPE          17
/*! TH4G per pipe mmu port number. */
#define TH4G_MMU_PORTS_PER_PIPE          32
/*! TH4G itm number. */
#define TH4G_NUM_ITM                     2
/*! TH4G local management port physical port number. */
#define TH4G_MGMT_LOCAL_PHYS_PORT        16
/*! TH4G per pipe front panel port number. */
#define TH4G_FP_PORTS_PER_PIPE           16
/*! TH4G local loopback port physical port number. */
#define TH4G_LB_LOCAL_PHYS_PORT          16
/*! TH4G cpu low priority rqe queue number. */
#define TH4G_CPU_LO_RQE_Q_NUM            6
/*! TH4G cpu high priority rqe queue number. */
#define TH4G_CPU_HI_RQE_Q_NUM            7
/*! TH4G mirror rqe queue number. */
#define TH4G_MIRR_RQE_Q_NUM              8

/*! Pipe reset hardware timeout msec. */
#define PIPE_RESET_HW_TIMEOUT_MSEC      50
/*! Pipe reset emulator timeout msec. */
#define PIPE_RESET_EMUL_TIMEOUT_MSEC    10000
/*! Emulator factor. */
#define EMULATION_FACTOR                10000
/*! TH4G per port queue number. */
#define TH4G_NUM_GP_QUEUES               12

/*! TH4G front panel port max port ID. */
#define TH4G_MAX_NUM_FP_PORTS            256
/*! TH4G per pipe max front panel port number. */
#define TH4G_MAX_NUM_FP_PORTS_PIPE       16
/*! TH4G 10G management port lane number. */
#define TH4G_NUM_LANES_10G_MGMT_PORT     1
/*! TH4G 40G management port lane number. */
#define TH4G_NUM_LANES_40G_MGMT_PORT     4
/*! TH4G auxiliary port lane number. */
#define TH4G_NUM_LANES_AUX_PORT          1

#define TH4G_MAX_SERVICE_POOLS 4
#define TH4G_TM_CHIP_QUEUES 3264


/*!
 * \brief TM port level init/config function.
 *
 * Perform all port configuration dependent TM configurations.
 *
 * This function is bound to the port control(PC) driver and will only be invoked by PC during
 * initialization or after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Port number.
 * \param [in] old_cfg Old MMU port config.
 * \param [in] new_cfg New MMU port config.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56996_a0_tm_pc_configure(int unit,
                            size_t num_ports,
                            const bcmpc_mmu_port_cfg_t *old_cfg,
                            const bcmpc_mmu_port_cfg_t *new_cfg);

/*!
 * \brief Chip-specific initialization
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Hardware table not found or DPR frequency range check failed.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56996_a0_tm_chip_init(int unit, bool warm);

/*!
 * \brief Transfrom physcial table SID for DV code use.
 *
 * \param [in] unit Unit number.
 * \param [in] in_sid Input symbol ID.
 * \param [in] in_inst Input instance number.
 *
 * \param [out] out_sid Output symbol ID.
 * \param [out] instance Output instance number.
 *
 * \retval none
 */
extern void
bcm56996_a0_tm_pt_sid_rename(int unit,
                             bcmdrd_sid_t in_sid,
                             int in_inst,
                             bcmdrd_sid_t *out_sid,
                             int *instance);

/*!
 * \brief Initialize mmu thresholds for THDI/THDO.
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
bcm56996_a0_tm_mmu_config_init_thresholds(int unit);

/*!
 * \brief Initialize mmu thresholds for THDI/THDO
 *
 *Setup mmu per-port thresholds for THDI/THDO
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device Port number.
 *
 * \retval SHR_E_* all the possible shr error.
 */
int
bcm56996_a0_tm_port_thd_configure (int unit, int lport);

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
bcm56996_a0_tm_rqe_threshold_init(int unit);


/*!
 * \brief number of unicast queue for front panel port for bcm56996_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_num_mc_q_non_cpu_port_get(int unit);

/*!

 * \brief number of multicast queue for front panel ports for bcm56996_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_num_uc_q_non_cpu_port_get(int unit);

/*!
 * \brief IMM Initialization functions for bcm56996_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_imm_init(int unit);

/*!
 * \brief IMM pre-population functions for bcm56996_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_imm_populate(int unit);

/*!
 * \brief IMM update functions for bcm56996_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport Physical port ID.
 * \param [in] up Port up/down status.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_imm_update(int unit, int pport, int up);

/*!
 * \brief number of unicast queue for front panel port for bcm56996_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_num_mc_queues_per_port_get(int unit);

/*!

 * \brief number of multicast queue for front panel ports for bcm56996_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56996_a0_tm_num_uc_queues_per_port_get(int unit);

/*!

 * \brief Get the chip queue number from logical port, mmu queue number.
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
extern int
bcm56996_a0_tm_chip_q_get(int unit, uint32_t lport,
                          uint32_t mmu_q, uint32_t *chip_q);

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
bcm56996_a0_tm_mmu_q_from_chip_q_get(int unit, int chip_q,
                                     int *lport, uint32_t *mmu_q,
                                     bcmtm_q_type_t *q_type);

/*!
 * \brief Port-level IDB flexport port-down function for bcm56996_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56996_a0_tm_port_idb_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level IDB flexport port-up function for bcm56996_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56996_a0_tm_port_idb_up(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level MMU flexport port-down function for bcm56996_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56996_a0_tm_port_mmu_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level EDB flexport port-down function for bcm56996_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56996_a0_tm_port_edb_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level EDB flexport port-up function for bcm56996_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56996_a0_tm_port_edb_up(int unit, bcmtm_pport_t pport);

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
bcm56996_a0_ip_port_forwarding_bitmap_enable(int unit, bcmtm_pport_t pport);

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
bcm56996_a0_ip_port_forwarding_bitmap_disable(int unit, bcmtm_pport_t pport);

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
bcm56996_a0_tm_check_valid_itm(int unit, int itm);

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
bcm56996_a0_tm_validate (int unit,
                         size_t num_ports,
                         const bcmpc_mmu_port_cfg_t *old_cfg,
                         const bcmpc_mmu_port_cfg_t *new_cfg);

/*!
 * \brief Rset EP to MMU credit during port disable.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */

extern int
bcm56996_a0_tm_mmu_port_clear_edb_credit_counter(int unit,
                                                 bcmtm_pport_t pport);

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
bcm56996_a0_tm_drv_info_portmap_fill(int unit,
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
bcm56996_a0_tm_port_add(int unit, bcmtm_pport_t pport);

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
bcm56996_a0_tm_port_delete(int unit, bcmtm_pport_t pport);

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
bcm56996_a0_tm_check_ep_mmu_credit(int unit, int lport);

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
bcm56996_a0_tm_check_port_ep_credit(int unit, int lport);

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
bcm56996_a0_tm_mchip_port_lport_get(int unit, int mchip_port, int *lport);

#endif /* BCM56996_A0_TM_H */
