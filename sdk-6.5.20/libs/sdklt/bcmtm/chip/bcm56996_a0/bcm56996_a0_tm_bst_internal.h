/*! \file bcm56996_a0_tm_bst_internal.h
 *
 * File containing BST PT parameters for
 * bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_TM_BST_INTERNAL_H
#define BCM56996_A0_TM_BST_INTERNAL_H

#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmltd/bcmltd_types.h>

/* Structure to pass ING interrupt info to handler function. */
typedef struct {
    SHR_BITDCLNAME(fbmp,
        TM_BST_EVENT_SOURCE_INGt_FIELD_COUNT);
    int pipe;
    int sp_index;
    int headroom_pool_index;
    int pg_port;
    int pg_index;
    int pg_headroom_port;
    int pg_headroom_index;
    int port_sp_port;
    int port_sp_index;
} bcmtm_bst_ing_source_t;

/* Structure to pass EGR interrupt info to handler function. */
typedef struct {
    SHR_BITDCLNAME(fbmp,
        TM_BST_EVENT_SOURCE_EGRt_FIELD_COUNT);
    int itm;
    int q_uc_port;
    int q_uc_id;
    int q_mc_port;
    int q_mc_id;
    int portsp_port;
    int portsp_sp;
    int sp_index;
    int mc_portsp_port;
    int mc_portsp_sp;
    int mc_sp_index;
} bcmtm_bst_egr_source_t;

/*!
 * \brief BST init time routine
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int
bcm56996_a0_bst_init(int unit, bool warm);


/*!
 * \brief BST cleanup functionality
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE  Error code.
 */
extern int
bcm56996_a0_bst_cleanup(int unit);

/*!
 * \brief Update the BST Event state in the IMM
 *
 * \param [in] unit Unit number.
 * \param [in] info Details of new BST state
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failure
 */
extern int
bcm56996_a0_tm_bst_event_imm_notify(int unit,
                bst_event_cfg_t *info);

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
bcm56996_a0_tm_bst_event_control_pt_set(int unit,
                bcmltd_sid_t ltid,
                bst_event_cfg_t *info);

/*!
 * \brief Enable/disable BST interrupt for a class.
 *
 * \param [in] unit Unit number.
 * \param [in] class BST class.
 * \param [in] enable Enable or disable interrupt.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56996_a0_bst_intr_enable(int unit,
                            bst_event_class_t class,
                            bool enable);
/*!\ BST driver get function.
 *
 * \param [in] unit Unit number.
 * \param [out] bst_drv BST driver.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56996_a0_tm_bst_drv_get(int unit, void *bst_drv);

#endif /* BCM56996_A0_TM_BST_INTERNAL_H */
