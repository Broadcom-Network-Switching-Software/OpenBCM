/*! \file bcm56780_a0_sec_sa_policy.h
 *
 * File containing chip related defines and internal functions for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_SA_POLICY_H
#define BCM56780_A0_SEC_SA_POLICY_H

#include <bcmsec/bcmsec_drv.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmsec/bcmsec_pt_internal.h>

/*!
 * \brief SA policy set
 *
 * \param [in] unit unit number.
 * \param [in] ltid Logical table id
 * \param [in] pt_dyn_info PT info
 * \param [in] in Field list
 * \param [in] encrypt encrypt
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56780_a0_sa_policy_set (int unit, bcmltd_sid_t ltid,
                           bcmsec_pt_info_t *pt_dyn_info,
                           bcmltd_field_t *in, int encrypt);

/*!
 * \brief SA policy get
 *
 * \param [in] unit unit number.
 * \param [in] ltid Logical table id
 * \param [in] pt_dyn_info PT info
 * \param [in] sa_policy sa_policy
 * \param [in] encrypt encrypt
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56780_a0_sa_policy_get (int unit, bcmltd_sid_t ltid,
                           bcmsec_pt_info_t *pt_dyn_info,
                           sa_policy_info_t *sa_policy,
                           int encrypt);
/*!
 * \brief SA expire handler
 *
 * \param [in] unit unit number.
 * \param [in] blk_id Block
 * \param [in] direction Encrypt/Decrypt
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56780_a0_sec_sa_expire_handler(int unit, int blk_id, int direction);
#endif /* BCM56780_A0_SEC_SA_POLICY_H */
