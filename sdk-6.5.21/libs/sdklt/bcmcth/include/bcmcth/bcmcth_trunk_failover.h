/*! \file bcmcth_trunk_failover.h
 *
 * TRUNK_FAILOVER LT handler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_FAILOVER_H
#define BCMCTH_TRUNK_FAILOVER_H

/*******************************************************************************
 * Includes
 */
#include <bcmltd/bcmltd_handler.h>

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief TRUNK_FAILOVER table validation handler function.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [out] in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_trunk_failover_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK_FAILOVER table insert handler function.
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
bcmcth_trunk_failover_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK_FAILOVER table lookup handler function.
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
bcmcth_trunk_failover_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK_FAILOVER table delete handler function.
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
bcmcth_trunk_failover_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK_FAILOVER table update handler function.
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
bcmcth_trunk_failover_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK_FAILOVER traverse get first.
 *
 * Get the first TRUNK_FAILOVER LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_failover_first(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK_FAILOVER traverse get next.
 *
 * Get the next TRUNK_FAILOVER LT entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Component arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_failover_next(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK FAILOVER LT Commit function.
 *
 * Commit software structures for TRUNK failover.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_trunk_failover_commit(int unit,
                             uint32_t trans_id,
                             const bcmltd_generic_arg_t *arg);

/*!
 * \brief TRUNK FAILOVER LT abort function.
 *
 * Abort software structures for TRUNK failover.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_trunk_failover_abort(int unit,
                            uint32_t trans_id,
                            const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_TRUNK_FAILOVER_H */

