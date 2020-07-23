/*! \file ser_lt_ser_log_status.h
 *
 * NGSDK
 *
 * Interface functions for SER Cache and IMM LTs
 *
 * SER cache data comes from IMM LTs, and can be used by SER component
 * SER component also can update data to SER cache and IMM LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_LOG_STATUS_H
#define SER_LT_SER_LOG_STATUS_H

#include <sal/sal_types.h>

/*!
 * \brief Incease ID of SER_LOG, and update new ID to IMM SER_LOG_STATUS.
 *
 * \param [in] unit        Unit number.
 * \param [in] update_imm Update SER_LOG_STATUS.SER_LOG_ID.
 * \param [in] log_en SER logging is enabled controlled by SER_CONTROL.SER_LOGGING.
 *
 * \retval NONE
 */
extern void
bcmptm_ser_log_id_incr(int unit, bool update_imm, bool log_en);

/*!
 * \brief Get ID of SER_LOG from cache of SER_LOG_STATUS.
 * The latest SER log is saved into ID entry.
 *
 *
 * \param [in] unit           Unit number.
 *
 * \retval ID of SER_LOG.
 */
extern uint32_t
bcmptm_ser_log_id_get(int unit);

/*!
 * \brief Check whether SER_LOG is full or not.
 *
 * \param [in] unit           Unit number.
 *
 * \retval TRUE: SER_LOG is full, FALSE: SER_LOG is not full.
 */
extern bool
bcmptm_ser_log_is_full(int unit);

/*!
 * \brief Initialize IMM SER_LOG_STATUS and its cache.
 *
 * Called during Coldboot.
 *
 * \param [in] unit        Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_log_status_imm_init(int unit);

/*!
 * \brief Synchronize data of SER_LOG_STATUS to its cache.
 *
 * Called during Warmboot.
 *
 * \param [in] unit        Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_log_status_cache_sync(int unit);

#endif /* SER_LT_SER_LOG_STATUS_H */
