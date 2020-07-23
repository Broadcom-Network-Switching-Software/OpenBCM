/*! \file bcmltx_dlb_port_status.h
 *
 * DLB_TRUNK_PORT_STATUS transform
 * utilities for link status.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_DLB_PORT_STATUS_H
#define BCMLTX_DLB_PORT_STATUS_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief DLB port status reverse transform.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  in_key   Port ID.
 * \param [in]  in_val   Hardware port status.
 * \param [out] out      LINK_STATE field.
 * \param [in]  arg      Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_dlb_port_status_rev_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in_val,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_DLB_PORT_STATUS_H */
