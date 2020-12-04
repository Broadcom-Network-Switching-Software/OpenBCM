/*! \file bcmltx_port_id_set.h
 *
 * Mirror Port ID Set Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_ID_SET_H
#define BCMLTX_PORT_ID_SET_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Mirror Port ID fields transform
 *
 * This function transforms the Mirror Port ID
 * into individual HW fields.
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
bcmltx_port_id_set_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

/*!
 * \brief Mirror Port ID fields rev transform
 *
 * This function transforms the HW fields into
 * the Mirror Port ID.
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
bcmltx_port_id_set_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_ID_SET_H */
