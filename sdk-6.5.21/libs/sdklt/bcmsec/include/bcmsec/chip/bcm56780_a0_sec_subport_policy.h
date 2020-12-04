/*! \file bcm56780_a0_sec_subport_policy.h
 *
 * File containing chip related defines and internal functions for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_SUBPORT_POLICY_H
#define BCM56780_A0_SEC_SUBPORT_POLICY_H

#include <bcmsec/bcmsec_drv.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmsec/bcmsec_pt_internal.h>

/*!
 * \brief Subport policy set
 *
 * \param [in] unit unit number.
 * \param [in] ltid Logical table id
 * \param [in] pt_dyn_info PT info
 * \param [in] in Field list
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56780_a0_subport_policy_macsec_decrypt_set(int unit, bcmltd_sid_t ltid,
                                   bcmsec_pt_info_t *pt_dyn_info,
                                   bcmltd_field_t *in);
#endif /* BCM56780_A0_SEC_SUBPORT_POLICY_H */
