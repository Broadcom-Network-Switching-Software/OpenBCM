/*! \file bcmptm_spm.h
 *
 * SPM resource info CTH implementation functions.
 *
 * This file contains APIs to the implementation of Strength Profile
 * resource info tables employed by the CTH wrappers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SPM_H
#define BCMPTM_SPM_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*******************************************************************************
 * Defines
 */

/*******************************************************************************
 * Typedef
 */

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief SPM resource info logical table CTH validate function.
 *
 * Validate the field values of a strength profile resource info
 * logical table entry.
 *
 * The CTH interface is located in the bcmcth/spm directory.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmptm_spm_resource_info_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg);

/*!
 * \brief Perform LOOKUP call for SP resource LTs.
 *
 * Strength Profile LTs have a corresponding resource LT to allow
 * inspection of the current entry usage state.  This function
 * provides the CTH interface for LOOKUP.
 *
 * The CTH interface is located in the bcmcth/spm directory.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmptm_spm_resource_info_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Perform TRAVERSE first call for SP resource LTs.
 *
 * Strength Profile LTs have a corresponding resource LT to allow
 * inspection of the current entry usage state.  This function
 * provides the CTH interface for TRAVERSE first.
 *
 * The CTH interface is located in the bcmcth/spm directory.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmptm_spm_resource_info_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Perform TRAVERSE next call for SP resource LTs.
 *
 * Strength Profile LTs have a corresponding resource LT to allow
 * inspection of the current entry usage state.  This function
 * provides the CTH inmplemetation for TRAVERSE next.
 *
 * The CTH interface is located in the bcmcth/spm directory.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmptm_spm_resource_info_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

#endif /* BCMPTM_SPM_H */
