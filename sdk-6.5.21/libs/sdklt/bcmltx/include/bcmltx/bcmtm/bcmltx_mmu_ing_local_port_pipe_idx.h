/*! \file bcmltx_mmu_ing_local_port_pipe_idx.h
 *
 * TM mmu local port_pipe index and instance calculation.
 * Calculated the memory index and pipe instance for the port_pipe.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MMU_ING_LOCAL_PORT_PIPE_IDX_H
#define BCMLTX_MMU_ING_LOCAL_PORT_PIPE_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TM ING Port PIPE index transform.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_mmu_ing_local_port_pipe_idx_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief TM ING Port PIPE index reverse transform.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_mmu_ing_local_port_pipe_idx_rev_transform(int unit,
                                                 const bcmltd_fields_t *in,
                                                 bcmltd_fields_t *out,
                                                 const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_MMU_ING_LOCAL_PORT_PIPE_IDX_H */
