/*! \file bcmcth_meter_sc.h
 *
 * This file contains Storm Control Meter Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_SC_H
#define BCMCTH_METER_SC_H

#include <bcmltd/bcmltd_handler.h>
#include <shr/shr_bitop.h>
#include <bcmdrd_config.h>

/*!
 * \brief SC Meter validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_validate(int unit,
                         bcmlt_opcode_t opcode,
                         const bcmltd_fields_t *in,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter insert
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_insert(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter lookup
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_lookup(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter delete
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_delete(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter update
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field value (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_update(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief SC Meter init
 *
 * Initialize Storm Control Meter related data structures
 * for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_init(int unit,
                     bool warm);

/*!
 * \brief SC Meter cleanup
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_cleanup(int unit);

/*!
 * \brief Storm control meter traverse get first.
 *
 * Get the first Storm control meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_first(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg);
/*!
 * \brief Storm control meter traverse get next.
 *
 * Get the next Storm control meter LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_next(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief Storm control meter transaction commit
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_commit(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief Storm control meter transaction abort
 *
 * Rollback the Meter entry from the backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_sc_abort(int unit,
                      uint32_t trans_id,
                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Set pointer to storm control meter driver device structure.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE
 */
extern int
bcmcth_meter_sc_drv_init(int unit);

/*!
 * \brief Clear pointer to storm control meter driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE
 */
extern int
bcmcth_meter_sc_drv_cleanup(int unit);

#endif /* BCMCTH_METER_SC_H */
