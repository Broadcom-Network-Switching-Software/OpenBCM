/*! \file bcmltx_chip_port_sp_idx.h
 *
 * TM OBM port index and instance calculation.
 * Calculated the memory index and pipe instance for the OBM port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_CHIP_PORT_SP_IDX_H
#define BCMLTX_CHIP_PORT_SP_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TM Chip Port SP index transform.
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
bcmltx_chip_port_sp_idx_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_CHIP_PORT_SP_IDX_H */
