/*! \file bcmltx_ctr_mib_mem_idx.h
 *
 * MIB_MEM.__INDEX/__INSTANCE transform handler
 *
 * This file contains field transform information for
 * MIB_MEM.__INDEX/__INSTANCE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_CTR_MIB_MEM_IDX_H
#define BCMLTX_CTR_MIB_MEM_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief MIB_MEM.__INDEX/__INSTANCE forward transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            PORT_ID.
 * \param [out] out           __INDEX/__INSTANCE.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_ctr_mib_mem_idx_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_CTR_MIB_MEM_IDX_H */
