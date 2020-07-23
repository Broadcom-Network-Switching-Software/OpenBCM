/*! \file bcmltx_ctr_port_q_oper_state.h
 *
 * CTR port queue operational state transform handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_CTR_PORT_Q_OPER_STATE_H
#define BCMLTX_CTR_PORT_Q_OPER_STATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Component extended transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in_key          LTD key input logical field set.
 * \param [in]  in_value        LTD value input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_ctr_port_q_oper_state_rev_transform(int unit,
                                           const bcmltd_fields_t *in_key,
                                           const bcmltd_fields_t *in_value,
                                           bcmltd_fields_t *out,
                                           const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_CTR_PORT_Q_OPER_STATE_H */


