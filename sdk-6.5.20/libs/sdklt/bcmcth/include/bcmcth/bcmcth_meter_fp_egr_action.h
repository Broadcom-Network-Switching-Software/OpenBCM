/*! \file bcmcth_meter_fp_egr_action.h
 *
 * This file contains EFP Meter Action Custom handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_EGR_ACTION_H
#define BCMCTH_METER_FP_EGR_ACTION_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_meter_action_internal.h>

/*! Supported actions for EFP meter. */
typedef enum bcmcth_meter_egr_action_e {
    METER_EGR_ACTION_INT_PRI,
    METER_EGR_ACTION_INT_ECN_CNG,
    METER_EGR_ACTION_DROP,
    METER_EGR_ACTION_DO_NOT_DROP,
} bcmcth_meter_egr_action_t;

/*! Supported colored actions for EFP meter. */
typedef enum bcmcth_meter_egr_color_action_e {
    METER_EGR_G_ACTION_INT_PRI,
    METER_EGR_G_ACTION_INT_ECN_CNG,
    METER_EGR_G_ACTION_DROP,
    METER_EGR_G_ACTION_DO_NOT_DROP,
    METER_EGR_Y_ACTION_INT_PRI,
    METER_EGR_Y_ACTION_INT_ECN_CNG,
    METER_EGR_Y_ACTION_DROP,
    METER_EGR_Y_ACTION_DO_NOT_DROP,
    METER_EGR_R_ACTION_INT_PRI,
    METER_EGR_R_ACTION_INT_ECN_CNG,
    METER_EGR_R_ACTION_DROP,
    METER_EGR_R_ACTION_DO_NOT_DROP,
} bcmcth_meter_egr_color_action_t;

/*! Macro for EFP meter action sw state. */
#define METER_FP_EGR_ACTION_STATE(unit)      (bcmcth_meter_fp_egr_action_state[unit])
/*! Macro for EFP meter action backup sw state. */
#define METER_FP_EGR_ACTION_STATE_HA(unit)   (bcmcth_meter_fp_egr_action_state_ha[unit])
/*! Macro for EFP meter action PDD sw state. */
#define METER_FP_EGR_PDD_STATE(unit)         (meter_fp_egr_pdd_state[unit])
/*! Macro for EFP meter action PDD backup sw state. */
#define METER_FP_EGR_PDD_STATE_HA(unit)      (meter_fp_egr_pdd_state_ha[unit])
/*! Macro for EFP meter action sw driver. */
#define METER_FP_EGR_ACTION_DRV(unit)        (meter_fp_egr_action_drv[unit])
/*! Macro for EFP meter action PDD information. */
#define METER_FP_EGR_PDD_INFO(unit)          (METER_FP_EGR_ACTION_DRV(unit)->pdd_info)
/*! Macro for EFP meter action offset information. */
#define METER_FP_EGR_ACTION_INFO(unit)       (METER_FP_EGR_ACTION_DRV(unit)->action_info)
/*! Macro for EFP meter action container information. */
#define METER_FP_EGR_CONT_INFO(unit, action) (METER_FP_EGR_PDD_INFO(unit)->cont_info[action])

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_meter_action_drv_t *_bc##_cth_meter_egr_action_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the meter tbls init functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_di_cth_meter_egr_action_variant_init(int unit, bcmcth_meter_action_drv_t *drv);
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_meter_fp_egr_action_drv_init(int unit);

/*!
 * \brief Cleanup device driver.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_meter_fp_egr_action_drv_cleanup(int unit);

/*!
 * \brief EFP meter init
 *
 * Initialize EFP Meter related data structures
 * for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_egr_action_init (int unit, bool warm);


/*!
 * \brief EFP meter cleanup
 *
 * Cleanup EFP Meter related data structures
 * allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_egr_action_cleanup(int unit);

/*!
 * \brief FP meter action insert
 *
 * Insert an FP Meter action in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [out] out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_action_entry_insert(int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data,
                                     bcmltd_fields_t *out);

/*!
 * \brief FP meter action update.
 *
 * Update Insert an FP Meter action in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [out] out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_action_entry_update(int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data,
                                     bcmltd_fields_t *out);
/*!
 * \brief FP meter action delete
 *
 * Delete an FP Meter action from the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_action_entry_delete(int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data);
/*!
 * \brief FP meter PDD insert
 *
 * Insert an FP Meter PDD in the hardware PDD table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  reason        IMM event reason.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_pdd_entry_insert(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  bcmimm_entry_event_t reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);

/*!
 * \brief FP meter PDD delete
 *
 * Delete an FP Meter PDD from the hardware PDD table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_pdd_entry_delete(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);
/*!
 * \brief FP meter SBR insert
 *
 * Insert an FP Meter SBR in the hardware SBR table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_sbr_entry_insert(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);

/*!
 * \brief FP meter SBR delete
 *
 * Delete an FP Meter SBR from the hardware SBR table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_egr_sbr_entry_delete(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data);

/*!
 * \brief EFP meter action transaction commit
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  context       Context pointer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_egr_action_commit (int unit,
                               bcmltd_sid_t sid,
                               uint32_t trans_id,
                               void *context);

/*!
 * \brief EFP meter action transaction abort
 *
 * Rollback the Meter entry from the backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  context       Context pointer.
 */
extern void
bcmcth_meter_fp_egr_action_abort (int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              void *context);
/*!
 * \brief Get pointer to EFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_fp_egr_action_drv_get (int unit,
                                    bcmcth_meter_action_drv_t **drv);
/*!
 * \brief EFP meter PDD HW entry prepare.
 *
 * Convert EFP meter action LT entry to HW entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  pt_id         Physical table id.
 * \param [in]  data          Input field list.
 * \param [out] buf           HW entry buffer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_egr_sbr_hw_entry_prepare(int unit,
                                     uint32_t pt_id,
                                     const bcmltd_field_t *data,
                                     uint8_t *buf);

/*!
 * \brief Get pointer to EFP PDD sw state structure.
 *
 * \param [in] unit Unit number.
 * \param [out] ptr Software state structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_fp_egr_pdd_state_get(int unit,
                                 bcmcth_meter_action_pdd_sw_state_t **ptr);

/*!
 * \brief Get operational mode of EFP meters.
 *
 * \param [in] unit Unit number.
 * \param [out] mode Operating mode.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_egr_oper_mode_get(int unit,
                                  uint8_t *mode);


#endif /* BCMCTH_METER_FP_EGR_ACTION_H */
