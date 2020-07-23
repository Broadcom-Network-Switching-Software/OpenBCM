/*! \file bcm56990_b0_tm_imm_internal.h
 *
 * BCMTM in-memory table (IMM) utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_IMM_INTERNAL_H
#define BCM56990_A0_TM_IMM_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*
 * Operational state enums:
 * Defines must be consistent with the enum define in FLTG.
 */
/* TM_ENTRY_STATE_T (to be removed) */
#define VALID                         0
#define PORT_INFO_UNAVAILABLE         1

#define PORT_DELETE                   0
#define PORT_ADD                      1
#define UPDATE_ONLY                   2

extern int
bcm56990_b0_tm_thd_ing_port_pri_grp_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_headroom_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_port_service_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_service_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_uc_q_register(int unit);

extern int
bcm56990_b0_tm_thd_mc_q_register(int unit);

extern int
bcm56990_b0_tm_thd_egr_uc_port_service_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_egr_mc_port_service_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_egr_service_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_mc_egr_service_pool_register(int unit);

extern int
bcm56990_b0_tm_thd_q_grp_register(int unit);

int
bcm56990_b0_tm_thd_dynamic_margin_register(int unit);

int
bcm56990_b0_tm_thd_dynamic_margin_populate(int unit);

/*! \brief IMM callback register function for Device RX Q.
 *
 * \param unit[in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_device_imm_rx_q_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_port_pri_grp_populate(int unit);

extern int
bcm56990_b0_tm_port_uc_q_to_service_pool_register(int unit);

extern int
bcm56990_b0_tm_port_mc_q_to_service_pool_register(int unit);

extern int
bcm56990_b0_tm_ing_port_pri_grp_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_headroom_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_ing_port_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_ing_service_pool_populate(int unit);

/*! \brief IMM callback register function for LT TM_PFC_ING.
 *
 * \param unit[in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_pfc_ing_port_register(int unit);

/*! \brief IMM callback register function for LT TM_PFC_EGR.
 *
 * \param unit[in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_pfc_egr_port_register(int unit);

/*! \brief IMM callback register function for LT TM_PFC_PRI_PROFILE.
 *
 * \param unit[in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_pfc_pri_profile_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_port_pri_grp_populate(int unit);

extern int
bcm56990_b0_tm_thd_ing_headroom_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_ing_port_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_ing_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_uc_q_populate(int unit);

extern int
bcm56990_b0_tm_thd_mc_q_populate(int unit);

extern int
bcm56990_b0_tm_thd_egr_uc_port_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_egr_mc_port_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_egr_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_mc_egr_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_thd_q_grp_populate(int unit);

/*! \brief function to populate read-only LT TM_PORT_INFO.
 *
 * \param unit[in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_port_info_populate(int unit);

extern int
bcm56990_b0_tm_port_uc_q_to_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_port_mc_q_to_service_pool_populate(int unit);

extern int
bcm56990_b0_tm_ing_port_pri_grp_populate(int unit);

/*! \brief BST IMM callback register function.
 *
 *  \param unit[in] Unit number.
 *
 *  \retval SHR_E_NONE No errors.
 *  \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_imm_bst_register(int unit);

extern int
bcm56990_b0_tm_ing_port_register(int unit);

extern int
bcm56990_b0_tm_ing_port_populate(int unit);

/*! \brief IMM callback register function for PFC Deadlock LT.
 *
 *  \param unit[in] Unit number.
 *
 *  \retval SHR_E_NONE No errors.
 *  \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_pfc_deadlock_register(int unit);

extern int
bcm56990_b0_tm_thd_ing_port_pri_grp_update(int unit, int pport, int up);

/*! \brief function to populate update-only LT TM_PFC_DEADLOCK_RECOVERY_STATUS.
 *
 * \param unit[in] Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcm56990_b0_tm_pfc_deadlock_recovery_status_populate(int unit);

extern int
bcm56990_b0_tm_ing_port_pri_grp_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_thd_egr_mc_port_service_pool_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_thd_egr_uc_port_service_pool_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_ing_port_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_thd_ing_port_service_pool_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_port_mc_q_to_service_pool_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_port_uc_q_to_service_pool_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_thd_mc_q_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_thd_uc_q_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_thd_q_grp_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_pfc_deadlock_recovery_status_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_pfc_deadlock_recovery_update(int unit, int pport, int up);

extern int
bcm56990_b0_tm_port_info_update(int unit, int pport, int up);
#endif /* BCM56990_A0_TM_IMM_INTERNAL_H */
