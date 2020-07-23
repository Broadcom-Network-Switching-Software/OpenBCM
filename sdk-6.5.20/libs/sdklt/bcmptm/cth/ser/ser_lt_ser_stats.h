/*! \file ser_lt_ser_stats.h
 *
 * Interface functions for SER Cache and IMM LT
 *
 * SER cache data comes from IMM LT, and can be used by SER component
 * SER component also can update data to SER cache and IMM LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_STATS_H
#define SER_LT_SER_STATS_H

#include <sal/sal_types.h>

/*!
 * \brief Sync data from IMM to cache of LT SER_STATS.
 *
 * Called during Warmboot.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_stats_cache_sync(int unit);

/*!
 * \brief Increase count fields of LT SER_STATS by SER component.
 *
 * \param [in] unit            Unit number.
 * \param [in] pt_id           PT ID.
 * \param [in] flags           Flags of SER event
 * \param [in] blk_type        Block type.
 * \param [in] recovery_type   Recovery type.
 *
 * \retval SHR_E_NONE Success
 */
extern void
bcmptm_ser_stats_update(int unit,
                        bcmdrd_sid_t pt_id, uint32_t flags,
                        int blk_type,
                        bcmptm_ser_recovery_type_t recovery_type);

/*!
 * \brief Get data from cache of SER_STATS,
 * and then update the data to IMM.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] insert         Execute an insert operation.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_stats_imm_update(int unit, int insert);

/*!
 * \brief Allocate data cache for SER_STATS,
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_stats_cache_allocate(int unit);

/*!
 * \brief Free data cache for SER_STATS.
 *
 * \param [in] unit           Unit number.
 *
 * \retval
 */
extern void
bcmptm_ser_stats_cache_destroy(int unit);

/*!
 * \brief Register IMM callback routines for LT SER_STATS.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_stats_callback_register(int unit);

#endif /* SER_LT_SER_STATS_H */
