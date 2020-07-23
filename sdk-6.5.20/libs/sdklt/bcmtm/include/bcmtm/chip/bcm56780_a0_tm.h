/*! \file bcm56780_a0_tm.h
 *
 * File containing chip related defines and internal functions for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TM_H
#define BCM56780_A0_TM_H

#include <bcmpc/bcmpc_tm.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>

/*! Number of physical ports 0 - 166.*/
#define TD4_X9_NUM_PHYS_PORTS 167

/*! Number of port in each port block. 8*50 PM */
#define TD4_X9_PORTS_PER_PBLK 8

/*! Number of port blocks per pipe. */
#define TD4_X9_PBLKS_PER_PIPE 5

/*! Number of data path pipe per device. */
#define TD4_X9_PIPES_PER_DEV 4

/*! Number of packet processing pipe per device. */
#define TD4_X9_PP_PIPE_PER_DEV 2

/*! Number of port blocks per device .*/
#define TD4_X9_PBLKS_PER_DEV (TD4_X9_PBLKS_PER_PIPE * TD4_X9_PIPES_PER_DEV)

/*! Number of CPU port. CPU port is available only on Pipe 0. */
#define TD4_X9_NUM_CPU_PORTS 1

/*! Number of loopback port. (Pipe 1 and Pipe 3)*/
#define TD4_X9_NUM_LB_PORTS 2

/*! Number of RDB ports. (Pipe 0 and Pipe 2) */
#define TD4_X9_NUM_RDB_PORTS 2

/*! Number of management port. (Pipe 1 and Pipe 3) */
#define TD4_X9_NUM_MGMT_PORTS 2

/*! Number of physical general ports per pipe. */
#define TD4_X9_PHYS_PORTS_PER_PIPE 40

/*! Number of device port per pipe. */
#define TD4_X9_DEV_PORTS_PER_PIPE 20

/*! Number of device port per device. */
#define TD4_X9_DEV_PORTS_PER_DEV (TD4_X9_DEV_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV)

/*! Number of MMU ports per device. */
#define TD4_X9_MMU_PORTS_PER_PIPE 32

/*! Number of device front panel ports */
#define TD4_X9_FP_PORTS_PER_PIPE 18

/*! Number of front panel ports per device. */
#define TD4_X9_FP_PORTS_PER_DEV (TD4_X9_FP_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV)

/*! Reset timers counts. */
#define PIPE_RESET_HW_TIMEOUT_MSEC 50

/*! Pipe reset timer for emulation setup. */
#define PIPE_RESET_EMUL_TIMEOUT_MSEC 10000

/*! Emulation factor. */
#define EMULATION_FACTOR 10000

/*! Number of queues for front panel ports, management and loopback ports. */
#define TD4_X9_NUM_QUEUES_NON_CPU_PORT 12

/*! Number of queues for CPU port. */
#define TD4_X9_NUM_QUEUES_CPU_PORT 48
/*! Number of lanes for auxillary port. */
#define TD4_X9_NUM_LANES_AUX_PORT 1


/*! Number of ITMS */
#define TD4_X9_ITMS_PER_DEV 1

/*! Number of chip queues. */
#define TD4_X9_TM_CHIP_QUEUES 972

/*! bcm56780 device settings. */
/*! Maximum packet size in bytes. */
#define TD4_X9_MMU_MAX_PACKET_BYTES 9416
/*! Packet header size in bytes. */
/*! Jumbo packet maximum frame size in bytes. */
#define TD4_X9_MMU_JUMBO_FRAME_BYTES 9216
/*! Default MTU for the device in bytes. */
#define TD4_X9_MMU_DEFAULT_MTU_BYTES 1536
/*! Cell size in bytes. */
/*! Number of priority groups */
#define TD4_X9_MMU_PRI_GROUP 8
/*! Number of service pools. */
#define TD4_X9_MMU_SERVICE_POOL 4
/*! Multicast CQE for admission. */
#define TD4_X9_MMU_MCQ_ENTRY 20480
/*! Number of cells. */
#define TD4_X9_MMU_TOTAL_CELLS 271620

/*!
 * \brief TM port level init/config function.
 *
 * Perform all port configuration dependent TM configurations.
 *
 * This function is bound to the port control(PC) driver and will only be invoked by PC during
 * initialization or after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports in old_cfg and new_cfg.
 * \param [in] old_cfg Old MMU port config.
 * \param [in] new_cfg New MMU port config.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56780_a0_tm_pc_configure(int unit,
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
bcm56780_a0_tm_chip_init(int unit, bool warm);

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
bcm56780_a0_tm_mmu_config_init_thresholds(int unit);

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
bcm56780_a0_tm_rqe_threshold_init(int unit);

/*!
 * \brief number of unicast queue for front panel port for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_tm_num_mc_q_non_cpu_port_get(int unit);

/*!

 * \brief number of multicast queue for front panel ports for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_tm_num_uc_q_non_cpu_port_get(int unit);


/*!
 * \brief IMM Initialization functions for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_tm_imm_init(int unit);

/*!
 * \brief IMM pre-population functions for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_tm_imm_populate(int unit);

/*!
 * \brief IMM update functions for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport Physical port number.
 * \param [in] up Port up/down status.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_tm_imm_update(int unit, int pport, int up);


/*!
 * \brief Chip-level IDB init function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56780_a0_tm_idb_init(int unit);

/*!
 * \brief Port-level IDB flexport port-down function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56780_a0_tm_port_idb_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level IDB flexport port-up function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56780_a0_tm_port_idb_up(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level MMU flexport port-down function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56780_a0_tm_port_mmu_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level EDB flexport port-down function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56780_a0_tm_port_edb_down(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level EDB flexport port-up function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport physical port number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 */
extern int
bcm56780_a0_tm_port_edb_up(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port-level MMU TDM init function for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM hardware programming errors.
 * \retval SHR_E_FAIL failure to allocate resources.
 */
extern int
bcm56780_a0_tm_tdm_mmu_init(int unit);


/*!

 * \brief Get the port, mmu_q number from chip queue number.
 *
 * \param [in] unit unit number.
 * \param [in] chip_q Chip queue number.
 * \param [out] lport Logical port ID.
 * \param [out] mmu_q MMU queue ID.
 * \param [out] q_type Unicast(0)/Multicast(1).
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_tm_mmu_q_from_chip_q_get(int unit, int chip_q,
                        int *lport, uint32_t *mmu_q, bcmtm_q_type_t *q_type);

/*!
 * \brief New port add/configure in MMU for bcm56780_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56780_a0_tm_port_mmu_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief New port add/configure in IDB for bcm56780_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56780_a0_tm_port_idb_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief New port add/configure in EDB for bcm56780_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56780_a0_tm_port_edb_add(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port delete in MMU for bcm56780_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56780_a0_tm_port_mmu_delete(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port delete in IDB for bcm56780_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56780_a0_tm_port_idb_delete(int unit, bcmtm_pport_t pport);

/*!
 * \brief Port delete in EDB for bcm56780_a0.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56780_a0_tm_port_edb_delete(int unit, bcmtm_pport_t pport);

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
bcm56780_a0_tm_ip_port_forwarding_enable(int unit, bcmtm_pport_t pport);

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
bcm56780_a0_tm_ip_port_forwarding_disable(int unit, bcmtm_pport_t pport);

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
bcm56780_a0_tm_validate (int unit,
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
bcm56780_a0_tm_edb_credit_reset(int unit, bcmtm_pport_t pport);

/*!
 * \brief Checks for  the logical port is spare port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE port is spare port.
 * \retval FALSE port is not spare port.
 */
extern int
bcm56780_a0_tm_is_port_spare(int unit, int lport);

/*!
 * \brief RDB interrupt handler.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_param Interrupt param.
 */
extern void
bcm56780_a0_rdb_intr_hdlr(int unit, uint32_t intr_param);

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
bcm56780_a0_tm_port_add(int unit, bcmtm_pport_t pport);

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
bcm56780_a0_tm_port_delete(int unit, bcmtm_pport_t pport);

/*!
 * \brief Function to fill port_sched_state_resource structure
 *
 * Retrieve all information contained in bcmpc_mmu_port_cfg_t and fill it into
 * the field in tm_drv_info. The bcmpc_mmu_port_cfg_t is
 * generated by BCMPC and contains all the necessary port config info.
 *
 * \param [in] unit Logical unit number.
 * \param [in] num_ports Number of ports.
 * \param [in] old_cfg Old port configurations.
 * \param [in] new_cfg New port configurations.
 */
extern void
bcm56780_a0_tm_drv_info_portmap_fill(int unit,
                                     size_t num_ports,
                                     const bcmpc_mmu_port_cfg_t *old_cfg,
                                     const bcmpc_mmu_port_cfg_t *new_cfg);


/*!
 * \brief Check validity of the ITM.
 *
 * \param [in] unit Logical unit number.
 * \param [in] buffer_pool Buffer pool.
 *
 * \retval SHR_E_NONE valid itm.
 * \retval SHR_E_PARAM Invalid itm.
 */
extern int
bcm56780_a0_tm_check_valid_itm(int unit, int buffer_pool);

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
bcm56780_a0_tm_mchip_port_lport_get(int unit, int mport, int *lport);

#endif /* BCM56780_A0_TM_H */
