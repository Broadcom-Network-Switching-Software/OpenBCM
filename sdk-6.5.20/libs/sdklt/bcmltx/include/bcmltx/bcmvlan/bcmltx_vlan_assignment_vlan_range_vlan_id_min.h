/*! \file bcmltx_vlan_assignment_vlan_range_vlan_id_min.h
 *
 * VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN Transform Handler Header File
 *
 * This file contains field transform information for
 * VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_VLAN_ASSIGNMENT_VLAN_RANGE_VLAN_ID_MIN_H
#define BCMLTX_VLAN_ASSIGNMENT_VLAN_RANGE_VLAN_ID_MIN_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN transform
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
bcmltx_vlan_assignment_vlan_range_vlan_id_min_transform(int unit,
                                                        const bcmltd_fields_t *in,
                                                        bcmltd_fields_t *out,
                                                        const bcmltd_transform_arg_t *arg);

/*!
 * \brief VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN rev transform
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
bcmltx_vlan_assignment_vlan_range_vlan_id_min_rev_transform(int unit,
                                                            const bcmltd_fields_t *in,
                                                            bcmltd_fields_t *out,
                                                            const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_VLAN_ASSIGNMENT_VLAN_RANGE_VLAN_ID_MIN_H */
