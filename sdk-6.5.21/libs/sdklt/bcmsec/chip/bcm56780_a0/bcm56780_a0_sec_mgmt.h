/*! \file bcm56780_a0_sec_mgmt.h
 *
 * File containing decrypt management related changes for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_MGMT_H
#define BCM56780_A0_SEC_MGMT_H

#include <bcmsec/bcmsec_types.h>
#include <bcmlrd/bcmlrd_types.h>


/*!
 * \brief SEC PM enable for Macsec
 *
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID
 * \param [in] pm_id Port Macro ID
 * \param [in] enable Enable/Disable
 * \param [out] opcode operation status
 *
 * \retval SHR_E_NONE
 * \retval SHR_E_PARAM Parameter error
 */
extern int
bcm56780_a0_sec_decrypt_mgmt(int unit, bcmltd_sid_t ltid,
                             bcmsec_decrypt_mgmt_cfg_t *decrypt_mgmt_cfg);
#endif /* BCM56780_A0_SEC_MGMT_H */
