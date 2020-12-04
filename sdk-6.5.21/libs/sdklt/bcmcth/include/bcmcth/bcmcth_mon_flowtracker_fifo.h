/*! \file bcmcth_mon_flowtracker_fifo.h
 *
 * BCMCTH Flowtracker FT_LEARN_NOTIFY_FIFO driver
 * related definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMCTH_MON_FLOWTRACKER_FIFO_H
#define BCMCTH_MON_FLOWTRACKER_FIFO_H
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>

/*! Flow learn notification FIFO behavior flags */
#define BCMCTH_MON_FT_LEARN_FIFO_FULL_STOP_LEARNING (1 << 0)

/*! Utility macro for iterating the FIFO SW cache. */
#define BCMCTH_MON_FT_LEARN_FIFO_SW_CACHE_ITER(_fifo_sw_cache_,_entry_, _count_)\
    for((_entry_) = _fifo_sw_cache_;                                            \
        (_entry_) < (_fifo_sw_cache_ + _count_);                                \
        (_entry_)++)

/*!
 * \brief Initialize the FIFO handling driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Not enough memory.
 */
extern int bcmcth_mon_ft_fifo_drv_init (int unit);

/*!
 * \brief De-Initialize the FIFO handling driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
extern void bcmcth_mon_ft_fifo_drv_deinit (int unit);

/*!
 * \brief Pop the FIFO entries from HW and refresh the FIFO SW cache.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] count Number of unpopped entries in FIFO.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Driver uninitialized.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_fifo_sw_cache_refresh (int unit, int pipe,
                                        uint32_t count);

/*!
 * \brief Get the number of unpopped entries in FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [out] count FIFO's current entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Driver uninitialized.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_fifo_entry_count_get (int unit, int pipe,
                                        uint32_t *count);

/*!
 * \brief Set the behavior of learn notification FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] behavior Behavior flags. Refer BCMCTH_MON_FT_LEARN_FIFO_XXX.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Driver uninitialized.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_fifo_behavior_set (int unit, int pipe,
                                        uint32_t behavior);

/*!
 * \brief Traverse the SW cache and report the FIFO entries to MON_FLOWTRACKER_LEARN_EVENT_LOG LT.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] count Number of FIFO entries to report.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT Driver uninitialized.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_fifo_sw_cache_traverse_and_report (int unit, int pipe,
                                        uint32_t count);

#endif /* BCMCTH_MON_FLOWTRACKER_FIFO_H */
