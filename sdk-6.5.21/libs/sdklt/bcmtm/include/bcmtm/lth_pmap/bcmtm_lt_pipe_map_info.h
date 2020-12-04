/*! \file bcmtm_lt_pipe_map_info.h
 *
 * APIs, definitions for Pipe map info handler functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_LT_PIPE_MAP_INFO_H
#define BCMTM_LT_PIPE_MAP_INFO_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Pipe map info table validation.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [out] in            Input field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_validate(int unit,
                                bcmlt_opcode_t opcode,
                                const bcmltd_fields_t *in,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Pipe map info table insert.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_insert(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Pipe map info table lookup.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_lookup(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Pipe map info table delete.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_delete(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Pipe map info table update.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field value (not applicable).
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_update(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Pipe map info table first.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_first(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg);
/*!
 * \brief Pipe map info table next.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_lt_pipe_map_info_next(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);

#endif /* BCMTM_LT_PIPE_MAP_INFO_H */
