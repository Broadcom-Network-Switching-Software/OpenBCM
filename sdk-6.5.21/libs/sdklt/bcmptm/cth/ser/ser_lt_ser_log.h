/*! \file ser_lt_ser_log.h
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

#ifndef SER_LT_SER_LOG_H
#define SER_LT_SER_LOG_H

#include <sal/sal_types.h>

typedef void (*ser_control_cache_update_f)(int unit, uint32_t fid_lt, uint32_t data);

/*!
 * \brief Clear cache of SER_LOG, when re-enable SER_PT_CONTROL.SER_LOGGING.
 *
 * \param [in] unit Unit number.
 * \param [in] updating_cb Update cache value of SER_PT_CONTROL.SER_LOGGING
 * \param [in] data data updated to cache of SER_PT_CONTROL.SER_LOGGING
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_log_cache_clear(int unit, ser_control_cache_update_f updating_cb,
                           uint32_t data);

/*!
 * \brief Get depth of SER_LOG.
 *
 * This value can be defined in the SER_CONFIG table.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval Depth of SER_LOG
 */
extern uint32_t
bcmptm_ser_log_depth_get(int unit);

/*!
 * \brief Get cache address of SER_LOG.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern bcmptm_ser_log_flds_t *
bcmptm_ser_log_cache_get(int unit);

/*!
 * \brief Synchronize data from cache of SER_LOG to IMM.
 * And then, clear the cache.
 *
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_log_imm_update(int unit);

/*!
 * \brief Get SID which is recorded in latest SER_LOG entry.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SID
 */
extern bcmdrd_sid_t
bcmptm_ser_log_cache_pt_id_get(int unit);

/*!
 * \brief Free data cache of SER_LOG.
 *
 * \param [in] unit        Unit number.
 *
 * \retval NONE
 */
extern void
bcmptm_ser_log_cache_free(int unit);

/*!
 * \brief Allocate data cache for SER_LOG.
 *
 * \param [in] unit        Unit number.
 *
 * \retval NONE
 */
extern void
bcmptm_ser_log_cache_allocate(int unit);

/*!
 * \brief Check whether the latest SER event needs to be recorded to
 * SER_LOG and SER_STATS.
 *
 * If the same SER event had been recorded, the latest SER event
 * will be squashed.
 *
 * \param [in] unit        Unit number.
 *
 * \retval TRUE Not record.
 * \retval FALSE record.
 */
extern bool
bcmptm_ser_event_squash(int unit);

/*!
 * \brief Set INVALID_PT.
 *
 * Value of INVALID_PT is defined per device.
 *
 * \param [in] unit        Unit number.
 *
 * \retval NONE.
 */
extern void
bcmptm_ser_log_invalid_pt_id_set(int unit, bcmdrd_sid_t sid);

#endif /* SER_LT_SER_LOG_H */
