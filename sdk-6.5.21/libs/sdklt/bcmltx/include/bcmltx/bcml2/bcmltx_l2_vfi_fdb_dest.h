/*! \file bcmltx_l2_vfi_fdb_dest.h
 *
 * This file contains field transform information for
 * L2_VFI_FDB.DEST_TYPE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_VFI_FDB_DEST_H
#define BCMLTX_L2_VFI_FDB_DEST_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcml2/bcmltx_l2_common.h>

/*!
 * \brief L2_VFI_FDB destination type forward transform.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  in_key  Input key fields.
 * \param [in]  in      Input value fields.
 * \param [out] out     Output fields.
 * \param [in]  arg     Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_l2_vfi_fdb_dest_transform(int unit,
                                 const bcmltd_fields_t *in_key,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

/*!
 * \brief L2_VFI_FDB destination type reverse transform.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  in_key  Input key fields.
 * \param [in]  in      Input value fields.
 * \param [out] out     Output fields.
 * \param [in]  arg     Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_l2_vfi_fdb_dest_rev_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_L2_VFI_FDB_DEST_H */
