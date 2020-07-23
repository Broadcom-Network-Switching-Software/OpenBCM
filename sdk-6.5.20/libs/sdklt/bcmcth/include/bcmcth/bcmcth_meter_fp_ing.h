/*! \file bcmcth_meter_fp_ing.h
 *
 * This file contains IFP Meter Custom handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_ING_H
#define BCMCTH_METER_FP_ING_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmcth/bcmcth_meter_fp_sw_state.h>
#include <bcmcth/bcmcth_meter_internal.h>
#include <bcmcth/bcmcth_meter_util.h>

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_meter_fp_drv_t *_bc##_cth_meter_fp_ing_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief IFP Meter init
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warmboot flag
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_ing_init(int unit,
                         bool warm);

/*!
 * \brief IFP Meter cleanup
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_ing_cleanup(int unit);

/*!
 * \brief Insert meter LT entry.
 *
 * Find a free index and write this meter into hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID
 * \param [in] trans_id Transaction ID.
 * \param [in] key Key fields list.
 * \param [in] data fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmcth_meter_ing_template_insert(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data);

/*!
 * \brief Delete meter LT entry.
 *
 * Delete the meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID
 * \param [in] trans_id Transaction ID.
 * \param [in] key Key fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmcth_meter_ing_template_delete(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key);
/*!
 * \brief Update the meter LT entry.
 *
 * Update the meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID
 * \param [in] trans_id Transaction ID.
 * \param [in] key Key fields list.
 * \param [in] data fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmcth_meter_ing_template_update(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data);
/*!
 * \brief Lookup the meter LT entry.
 *
 * Lookup the meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID
 * \param [in] trans_id Transaction ID.
 * \param [in] context Context pointer.
 * \param [in] lkup_type Lookup type.
 * \param [in] in Input fields list.
 * \param [in] out Output fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmcth_meter_ing_template_lookup(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 void *context,
                                 bcmimm_lookup_type_t lkup_type,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out);
/*!
 * \brief IFP meter transaction commit
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  context       Context.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_ing_template_commit(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 void *context);

/*!
 * \brief IFP meter transaction abort
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  context       Context.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern void
bcmcth_meter_ing_template_abort(int unit,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                void *context);

/*!
 * \brief IFP Meter config
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_fp_ing_dev_config (int unit);

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_meter_fp_ing_drv_init(int unit);

/*!
 * \brief Cleanup device driver.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_meter_fp_ing_drv_cleanup(int unit);

/*!
 * \brief Get pointer to IFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_fp_ing_drv_get(int unit,
                            bcmcth_meter_fp_drv_t **drv);

/*!
 * \brief Get pointer to IFP meter software state.
 *
 * \param [in] unit Unit number.
 * \param [out] ptr Software state structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_fp_ing_sw_state_get(int unit,
                                 bcmcth_meter_fp_sw_state_t **ptr);

/*!
 * \brief Get FP ingress meter attributes.
 *
 * \param [in] unit         Unit number.
 * \param [out] attr        FP meter attributes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Not supported.
 */
int
bcmcth_meter_fp_ing_attrib_get(int unit,
                               bcmcth_meter_fp_attrib_t **attr);

#endif /* BCMCTH_METER_FP_ING_H */
