/*! \file bcmcth_ctr_eflex_pkt_resolution_info.h
 *
 * This file contains Enhanced flex counter packet resolution
 * information handler function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EFLEX_PKT_RESOLUTION_INFO_H
#define BCMCTH_CTR_EFLEX_PKT_RESOLUTION_INFO_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Enhanced flex counter packet resolution info validation
 *
 * Validate Enhanced flex counter packet resolution info parameters.
 *
 * \param [in] unit Unit Number.
 * \param [in] opcode LT opcode.
 * \param [in] in Input field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_validate(int unit,
                                              bcmlt_opcode_t opcode,
                                              const bcmltd_fields_t *in,
                                              const bcmltd_generic_arg_t *arg);
/*!
 * \brief Enhanced flex counter packet resolution info insert.
 *
 * Insert into Enhanced flex counter packet resolution  info LT.
 * Invalid operation on read-only LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_insert(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg);
/*!
 * \brief Enhanced flex counter packet resolution info lookup.
 *
 * Lookup Enhanced flex counter packet resolution info LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_lookup(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg);
/*!
 * \brief Enhanced flex counter packet resolution info delete.
 *
 * Delete Enhanced flex counter packet resolution info.
 * Invalid operation on read-only LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_delete(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg);
/*!
 * \brief Enhanced flex counter packet resolution info update.
 *
 * Update Enhanced flex counter packet resolution info.
 * Invalid operation on read-only LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_update(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_generic_arg_t *arg);

/*!
 * \brief Enhanced flex counter packet resolution info traverse.
 *
 * Traverse first Enhanced flex counter packet resolution info LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values (not applicable).
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_first(int unit,
                                           const bcmltd_op_arg_t *op_arg,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out,
                                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief Enhanced flex counter packet resolution info traverse.
 *
 * Traverse next Enhanced flex counter packet resolution info LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_eflex_pkt_resolution_info_next(int unit,
                                          const bcmltd_op_arg_t *op_arg,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_CTR_EFLEX_PKT_RESOLUTION_INFO_H */
