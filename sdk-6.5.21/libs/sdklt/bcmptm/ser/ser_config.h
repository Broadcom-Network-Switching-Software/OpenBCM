/*! \file ser_config.h
 *
 * Interfaces can be used to configure SER controlling registers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_CONFIG_H
#define SER_CONFIG_H

#include <bcmltd/chip/bcmltd_id.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/generated/bcmptm_ser_local.h>

/*!
 * \brief Get SER data saved in HA memory space.
 *
 * \param [in] unit Unit number
 *
 * \retval Address of SER HA data.
 */
extern bcmptm_ser_sinfo_t *
bcmptm_ser_ha_info_get(int unit);

/*!
 * \brief  Start or stop H/W engine
 * (not CMIC SER engine) to scan TCAMs.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: enable, 0: disable.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_hw_scan_init(int unit, int enable);

/*!
 * \brief Enable or disable SER functions of one buffer or bus.
 *
 * The functions contain ECC checking, ECC-1bit reported,
 * ECC error injected.
 *
 * \param [in] unit Unit number.
 * \param [in] rid SID of buffer or bus.
 * \param [in] ctrl_type SER enable type.
 * \param [in] enable 1: enable, 0: disable.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_buf_bus_enable(int unit, bcmdrd_ser_rmr_id_t rid,
                          bcmdrd_ser_en_type_t ctrl_type, int enable);

/*!
 * \brief Get callback routine in cth/ser.
 *
 * \retval CTH SER driver structure.
 */
extern bcmptm_ser_cth_drv_t *
bcmptm_ser_cth_drv_get(void);

#endif /* SER_CONFIG_H */
