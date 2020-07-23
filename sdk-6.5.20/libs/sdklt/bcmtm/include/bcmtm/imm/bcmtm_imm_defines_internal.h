/*! \file bcmtm_imm_defines_internal.h
 *
 * Function defintions for imm implementations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_IMM_DEFINES_INTERNAL_H
#define BCMTM_IMM_DEFINES_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/*! IMM entry valid. */
#define VALID                       0

/*! Logical to physical port mapping is missing. */
#define PORT_INFO_UNAVAIL           1

/*! Configuration is not applicable to the current port encapsulation. */
#define PORT_ENCAP_MISMATCH         2

/*! Invalid scheduler profile. */
#define SCHED_PROFILE_INVALID       2

/*! Scheduler profile incomplete. All the 12 queues has not been configured. */
#define SCHED_PROFILE_INCOMPLETE    3

/*! Number of unicast queues is more than permissible limit. */
#define UC_Q_LIMIT_EXCEEDS          4

/*! Number of multicast queues is mpre than permissible limit. */
#define MC_Q_LIMIT_EXCEEDS          5

/*! Invalid scheduler  node ID. */
#define SCHED_NODE_ID_INVALID       6

/*! Marks the entry in IMM is invalid. */
#define ENTRY_INVALID             255

/*! Unicast queue ID is exceeding allowed unicast queue number. */
#define UC_Q_INVALID                2

/*! Multicast queue ID is exceeding allowed multicast queue number. */
#define MC_Q_INVALID                2

/*! Invalid profile. */
#define PROFILE_INVALID             BCMLTD_COMMON_TM_ENTRY_STATE_T_T##_PROFILE_INVALID
/*! Incorrect PFC Rx pri to Cos optimization. */
#define INCORRECT_PFC_OPTIMIZATION  BCMLTD_COMMON_TM_ENTRY_STATE_T_T##_INCORRECT_PFC_OPTIMIZATION


/*! IMM actions */

/*! Profile delete by the application. */
#define ACTION_PROFILE_DELETE 1
/*! Profile delete internal. */
#define ACTION_PROFILE_DELETE_INT 2
/*! Profile update internal. */
#define ACTION_PROFILE_UPDATE_INT 3
/*! Port delete internal. */
#define ACTION_PORT_DELETE_INT 4
/*! Port update internal. */
#define ACTION_PORT_UPDATE_INT 5

/*! Action insert to IMM. */
#define ACTION_INSERT 6
/*! Action delete to IMM. */
#define ACTION_DELETE 7
/*! Action update. */
#define ACTION_UPDATE 8
/*! Action profile update. */
#define ACTION_PROFILE_UPDATE 9

/*! Repl list resource not available. */
#define REPLICATION_RESOURCE_UNAVAILABLE        1

/*! \brief IMM callback register function.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_cb_reg(int unit);

/*! \brief IMM callback register function for scheduler shapers.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_scheduler_shaper_cb_reg(int unit);

/*! \brief IMM callback register function for port shapers.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_shaper_port_imm_register(int unit);

/*! \brief IMM callback register function for node shapers.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_shaper_node_imm_register(int unit);

/*! \brief IMM callback register function for cut through port.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_ct_port_imm_register(int unit);

/*! \brief IMM callback register function for wred.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmtm_imm_wred_cb_reg(int unit);

/*! \brief IMM callback register function for wred unicast queue.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmtm_wred_ucq_imm_register(int unit);

/*! \brief IMM callback register function for wred port servicepool.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmtm_wred_portsp_imm_register(int unit);

/*! \brief IMM callback register function for wred congestion profile.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmtm_wred_cng_profile_imm_register(int unit);

/*! \brief IMM callback register function for node schedulers.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_scheduler_node_imm_register(int unit);

/*! \brief IMM callback register function for CPU node scheduler shapers.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_scheduler_shaper_cpu_node_imm_register(int unit);

/*! \brief IMM callback register function for non-CPU node scheduler shapers.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
int
bcmtm_scheduler_shaper_node_imm_register(int unit);

/*! \brief IMM callback register function for scheduler profile.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_scheduler_profile_imm_register(int unit);

/*! \brief IMM callback register function for scheduler ports.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_scheduler_port_imm_register(int unit);

/*! \brief IMM callback register function for obm port controls.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_obm_port_pkt_parse_imm_register(int unit);

/*! \brief IMM callback register function for obm policy config.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_obm_pc_pm_pkt_parse_imm_register(int unit);

/*! \brief IMM callback register function for obm port flow controls.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_obm_port_fc_imm_register(int unit);

/*! \brief IMM callback register function for mapping obm port internal
 * priority with traffic class.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_obm_port_pkt_pri_tc_imm_register(int unit);

/*! \brief IMM callback register function for OBM.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_obm_cb_reg(int unit);

/*! \brief IMM init function for Scheduler shaper.
 *
 * \param unit [in] Unit number.
 * \param warm [in] Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_scheduler_profile_imm_init(int unit, bool warm);

/*! \brief IMM init function.
 *
 * \param unit [in] Unit number.
 * \param warm [in] Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_init(int unit, bool warm);

/*!
 * \brief BST control LT imm callback register.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int
bcmtm_imm_bst_control_register(int unit);

/*!
 * \brief BST IMM event control LT callback register.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int
bcmtm_imm_bst_event_register(int unit);

/*! \brief Multicast IMM MC Group init function.
 *
 * \param unit [in] Unit number.
 * \param warm [in] Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_mc_group_init(int unit, bool warm);

/*! \brief Multicast IMM Port Agg init function.
 *
 * \param unit [in] Unit number.
 * \param warm [in] Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_mc_port_agg_list_init(int unit, bool warm);

/*! \brief Multicast Port Agg IMM cleanup function.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_mc_port_agg_list_cleanup(int unit);
/*!
 * \brief Multicast IMM IMM callback registration routine
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int
bcmtm_imm_mc_group_register(int unit);

/*!
 * \brief Multicast IMM IMM callback registration routine
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int
bcmtm_imm_mc_port_list_register(int unit);

/*! \brief IMM callback function for Scheduler strict priority profile.
 *
 * \param unit [in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_scheduler_sp_profile_imm_register(int unit);

/*! \brief IMM callback function for EBST port.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_ebst_port_register(int unit);

/*! \brief IMM callback function for EBST queue.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_ebst_q_register(int unit);

/*! \brief IMM callback function for EBST port service pool.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_ebst_portsp_register(int unit);

/*! \brief IMM callback function for EBST service pool.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_ebst_sp_register(int unit);

/*! \brief IMM callback function for EBST control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_imm_ebst_control_register(int unit);


/*! \brief IMM callback register function for PFC LT's.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_pfc_imm_register(int unit);

/*! \brief IMM callback register function for PFC deadlock LT's.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_pfc_deadlock_imm_register(int unit);

/*! \brief IMM callback register function for MoD logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_mod_imm_register(int unit);

/*! \brief  IMM update updates the TM cconfiguration based on port updates.
 *
 * \param  [in] unit Unit number.
 * \param  [in] lport Logical port number.
 * \param  [in] action LT update/delete action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_imm_update(int unit, int lport, uint8_t action);

/*!
 * \brief Egress service pool IMM register.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_egr_sp_imm_register(int unit);


/*! \brief IMM register function for queue assignment map information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Table not found.
 */
extern int
bcmtm_q_assignment_info_imm_register(int unit);

/*! \brief IMM register function for cosq cpu map information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Table not found.
 */
extern int
bcmtm_cos_q_cpu_map_info_imm_register(int unit);

#endif /* BCMTM_IMM_DEFINES_INTERNAL_H */
