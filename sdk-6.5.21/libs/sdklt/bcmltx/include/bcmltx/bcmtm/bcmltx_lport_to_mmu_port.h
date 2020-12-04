/*! \file bcmltx_lport_to_mmu_port.h
 *
 * TM table's index Transform Handler
 *
 * This file contains field transform information for TM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_LPORT_TO_MMU_PORT_H
#define BCMLTX_LPORT_TO_MMU_PORT_H

#include <bcmltd/bcmltd_handler.h>

 /*!
 * \brief TM logical port number to MMU port number transform
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
bcmltx_lport_to_mmu_port_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_LPORT_TO_MMU_PORT_H */
