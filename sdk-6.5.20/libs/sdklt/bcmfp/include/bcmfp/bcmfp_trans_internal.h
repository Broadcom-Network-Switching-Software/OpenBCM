/*! \file bcmfp_trans_internal.h
 *
 * BCMFP component transaction management data structures and
 * functions are present in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_TRANS_INTERNAL_H
#define BCMFP_TRANS_INTERNAL_H

#include <bcmimm/bcmimm_int_comp.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_types_internal.h>

/*! Transaction commands for BCMFP */
typedef enum bcmfp_trans_cmd_s {
    /* To check if the stage operational information  is in
     * idle state before transaction is committed. If not means
     * something went wrong in previous transaction.
     */
    BCMFP_TRANS_CMD_IDLE_CHECK = 1,
    /* Free the back up operational state. */
    BCMFP_TRANS_CMD_COMMIT = 2,
    /* Copy back up operational information to active. */
    BCMFP_TRANS_CMD_ABORT = 3
} bcmfp_trans_cmd_t;

/*!
 * \brief BCMFP transaction commands execution for all stages.
 *
 * This function is called from commit and abort call back functions
 * (bcmfp_XXX_trans_commit and bcmfp_XXX_trans_abort) corresponding to
 * all BCMFP stages which are registered with in-memory component.
 * Based on the transaction command it will either free the back up
 * operational state or copy the back up to active operational state.
 *
 * \param [in] unit This is device unit number.
 * \param [in] stage_id This is one of the BCMFP stage id.
 * \param [in] trans_cmd This is one of the BCMFP transaction commands.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_trans_cmd(int unit,
                bcmfp_stage_id_t stage_id,
                bcmfp_trans_cmd_t trans_cmd);
/*!
 * \brief Transaction commit callback function for ingress stage
 * logical table(FP_ING_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * ingress stage logical table(FP_ING_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * ingress logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of ingress stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_ing_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for ingress
 * stage logical table(FP_ING_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every ingress logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of ingress stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_ing_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context);
/*!
 * \brief Transaction commit callback function for egress stage
 * logical table(FP_EGR_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * egress stage logical table(FP_EGR_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * egress logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of egress stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_egr_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for egress
 * stage logical table(FP_EGR_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every egress logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of egress stage operational information to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_egr_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context);
/*!
 * \brief Transaction commit callback function for vlan stage
 * logical table(FP_VLAN_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * vlan stage logical table(FP_VLAN_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * vlan logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of vlan stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_vlan_trans_commit(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for vlan
 * stage logical table(FP_VLAN_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every vlan logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of vlan stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_vlan_trans_abort(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context);
/*!
 * \brief Transaction commit callback function for exact match stage
 * logical table(FP_EM_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * exact match stage logical table(FP_EM_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * exact match logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of exact match stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_em_trans_commit(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for exact match
 * stage logical table(FP_EM_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every exact match logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of exact match stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_em_trans_abort(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     void *context);
/*!
 * \brief Transaction commit callback function for flow tracker stage
 * logical table(EM_FT_XXX) entries.
 *
 * This function is a callback function BCMFP component register with
 * the in-memory component to receive transaction commit event for all
 * exact match stage logical table(EM_FT_XXX) entries. The callback contains
 * the field values of the entry that was changed along with the key value
 * identify the entry. Though this function is called for each and every
 * exact match logical table entries that are part of the transaction, BCMFP
 * component does free the back up copies of exact match stage operational
 * information(bcmfp_stage_oper_info_t) on the first call. All other
 * follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context. Not used by BCMFP
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_em_ft_trans_commit(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         void *context);
/*!
 * \brief BCMFP transaction abort callback function.
 *
 * This function is a callback function that BCMFP register with the
 * in-memory component to receive transaction abort events for flow tracker
 * stage logical table(EM_FT_XXX) entries. This callback mechanism is
 * used to abort a transaction that was previously staged. Though this
 * function is called for each and every exact match logical table entries
 * that are part of the transaction, BCMFP component does copy the
 * back up copies of exact match stage operational state to active copies
 * on the first call. All other follow up calls to this function are mute.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID. Not used by BCMFP.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer to some context.Not used by BCMFP.
 *
 * \return None.
 */
extern void
bcmfp_em_ft_trans_abort(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        void *context);

/*!
 * \brief Set the transaction state of the HA block created either init/run
 *  time.Following are the different reasons for any HA block to be in any
 *  of the valid transaction states.
 *  ----------------------------------------------------------------------------
 *  |                | 1. When HA block is created(Either component init time) |
 *  |    IDLE        |    OR run time).                                        |
 *  |                | 2. When Transaction is commited/aborted successfully.   |
 *  ----------------------------------------------------------------------------
 *  |                | 1. If any filed value in HA block(that is created init  |
 *  |    UC_A        |    time) is changed in the transaction.                 |
 *  | (Uncommitted   | 2. When HA elements are "acquired from"/"release to"    |
 *  |  Active)       |    HA block(that is created run time).                  |
 *  ----------------------------------------------------------------------------
 *  |                | 1. Before started copying the active HA block to backup |
 *  |  COPY_A_TO_B   |    block. This is triggered when Transaction manager    |
 *  | (Copy Active   |    dispatches transaction commit command.               |
 *  |  to backup)    |                                                         |
 *  ----------------------------------------------------------------------------
 *  |                | 1. Before started copying the backup HA block to active |
 *  |  COPY_B_TO_A   |    block. This is triggered when Transaction manager    |
 *  | (Copy Backup   |    dispatches transaction abort command.                |
 *  |  to active)    |                                                         |
 *  ----------------------------------------------------------------------------
 *
 * \param [in] unit This is device unit number.
 * \param [in] blk_id HA block id(created at either init/run time).
 * \param [in] trans_state Transaction state(IDLE/UC_A/
 *                         COPY_A_TO_B/COPY_B_TO_A).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_trans_state_set(int unit,
                      uint8_t blk_id,
                      bcmfp_trans_state_t trans_state);
/*!
 * \brief Get the transaction state of the HA block created either
 *  FP component init/run time.
 *
 * \param [in] unit This is device unit number.
 * \param [in] blk_id HA block id(created at either init/run time).
 * \param [in] trans_state Transaction state(IDLE/UC_A/
 *                         COPY_A_TO_B/COPY_B_TO_A).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmfp_trans_state_get(int unit,
                      uint8_t blk_id,
                      bcmfp_trans_state_t *trans_state);

/*!
 * \brief Set the FP dynamic atomic transaction state to TRUE or FALSE.
 *
 * \param [in] unit This is device unit number.
 * \param [in] trans_id Transaction id.
 * \param [in] atomic_trans_state State(TRUE/FALSE) of atomic
 *                                transaction state.
 *
 * \return SHR_E_NONE on success
 */
extern int
bcmfp_trans_atomic_state_set(int unit,
                             uint32_t trans_id,
                             bool atomic_trans_state);
#endif /* BCMFP_TRANS_INTERNAL_H */
