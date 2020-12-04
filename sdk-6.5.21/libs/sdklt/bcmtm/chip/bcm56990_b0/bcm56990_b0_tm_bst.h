/*! \file bcm56990_b0_tm_bst.h
 *
 * File containing BST PT parameters for
 * bcm56990_b0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_TM_BST_H
#define BCM56990_B0_TM_BST_H

#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmltd/bcmltd_types.h>

/*
 * BST Ingress status type. Order should not be changed. This order is
 * as per bit alignment in the hardware for the BST interrups types.
 */
typedef enum
bcmtm_bst_ing_stat_e {
    SERVICE_POOL,
    PORT_SERVICE_POOL,
    PG_HEADROOM,
    PG_SHARED,
    HEADROOM_POOL,
    MAX_BST_ING_STATUS
} bcmtm_bst_ing_stat_t;

/*
 * BST Egress status type. Order should not be changed. This order is
 * as per bit alignment in the hardware for the BST interrups types.
 */
typedef enum
bcmtm_bst_egr_state_e {
    QUEUE,
    UC_PORT_SERVICE_POOL,
    UC_SERVICE_POOL,
    MC_PORT_SERVICE_POOL,
    MC_SERVICE_POOL
} bcmtm_bst_egr_stat_t;

/*!
 * \brief Write the BST event control information to the PT
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table to be written
 * \param [in] info Information to be written to corresponding PT
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56990_b0_tm_bst_event_control_pt_set(int unit,
                                        bcmltd_sid_t ltid,
                                        bcmtm_bst_event_cfg_t *info);

/*!\ BST driver get function.
 *
 * \param [in] unit Unit number.
 * \param [out] bst_drv BST driver.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56990_b0_tm_bst_drv_get(int unit, void *bst_drv);

/*!
 * \brief BST interrupt notification.
 *
 * Reads the interrupt from hardware and add the information to message queue
 * for software processing.
 *
 * \param [in] unit Logical unit number.
 * \param [in] intr_num Interrupt source number.
 */
extern void
bcm56990_b0_tm_bst_intr_notify(int unit, uint32_t intr_param);
#endif /* BCM56990_B0_TM_BST_H */
