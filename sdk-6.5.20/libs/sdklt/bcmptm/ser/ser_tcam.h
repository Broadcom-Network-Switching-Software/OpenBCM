/*! \file ser_tcam.h
 *
 * Interface functions for SER of TCAMs
 *
 * SER TCAM H/W initialization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_TCAM_H
#define SER_TCAM_H

#include "ser_correct.h"
#include <bcmptm/bcmptm_ser_chip_internal.h>

/*!
 * \brief Initialize, configure and start CMIC SER engine.
 *
 * Some chips need not to start CMIC SER engine.
 *
 * \param [in] unit   Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_tcam_cmic_scan_init(int unit, bool warm);

/*!
 * \brief Get SER correction information from SER_RESULT_0m or SER_RESULT_1m.
 *
 * \param [in] unit         Unit number.
 * \param [in] serc_info  Should be NULL always, not used.
 * \param [out] crt_info    hw_idx bit offset of SER_RANGE_ENABLEr.
 *                          sbus_addr  SBUS address of memory entry owning SER error.
 *                          acc_type   Access type of tables. If access type is 0, 1, 2, 3,
 *                                     it also can be used as table instance.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_tcam_fifo_pop(int unit,
                         bcmptm_pre_serc_info_t *serc_info,
                         bcmptm_ser_raw_info_t *crt_info);

/*!
 * \brief Parse SER correction information from SER_RESULT_0m or SER_RESULT_1m.
 *
 * \param [in] unit         Unit number.
 * \param [in] crt_info     hw_idx bit offset of SER_RANGE_ENABLEr.
 *                          sbus_addr SBUS address of memory entry owning SER error.
 *                          acc_type Access type of tables. If access type is 0, 1, 2, 3,
 *                                   it also can be used as table instance.
 *
 * \param [out] spci        Information can be used for SER correction directly.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_tcam_fifo_entry_parse(int unit,
                                 bcmptm_ser_raw_info_t *crt_info,
                                 bcmptm_ser_correct_info_t *spci);

#endif /* SER_TCAM_H */
