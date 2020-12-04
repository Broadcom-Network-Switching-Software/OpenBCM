/*! \file bcmltx_port_oper_state.h
 *
 * SEC table's index Transform Handler
 *
 * This file contains field transform for port indexed
 * SEC tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_OPER_STATE_H
#define BCMLTX_PORT_OPER_STATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Port operational state reverse transform lookup.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              LTD key input logical field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical from physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_sec_port_oper_state_rev_transform(int unit,
                                  const bcmltd_fields_t *in_key,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_OPER_STATE_H */
