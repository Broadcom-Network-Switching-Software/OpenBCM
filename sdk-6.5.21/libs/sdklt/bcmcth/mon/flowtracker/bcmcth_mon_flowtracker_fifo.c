/*! \file bcmcth_mon_flowtracker_fifo.c
 *
 * Flowtracker learn FIFO driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_fifo.h>

/******************************************************************************
* Defines
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

#define BCMCTH_MON_FT_FIFO_DRV_FN_ENTRY_COMMON(unit)\
    bcmcth_mon_flowtracker_fifo_drv_t *fifo_drv = NULL;\
    SHR_FUNC_ENTER(unit);\
    fifo_drv = bcmcth_mon_ft_fifo_drv_get(unit);\
    SHR_NULL_CHECK(fifo_drv, SHR_E_INIT);

#define BCMCTH_MON_FT_FIFO_DRV_VOID_FN_ENTRY_COMMON(unit)\
    bcmcth_mon_flowtracker_fifo_drv_t *fifo_drv = NULL;\
    fifo_drv = bcmcth_mon_ft_fifo_drv_get(unit);\
    if (fifo_drv == NULL) {\
        return;\
    }


/******************************************************************************
* Public functions
 */
/*!
 * \brief Initialize the FIFO handling driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Not enough memory.
 */
int bcmcth_mon_ft_fifo_drv_init (int unit)
{
    BCMCTH_MON_FT_FIFO_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(fifo_drv->drv_init(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-Initialize the FIFO handling driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
void bcmcth_mon_ft_fifo_drv_deinit (int unit)
{
    BCMCTH_MON_FT_FIFO_DRV_VOID_FN_ENTRY_COMMON(unit);
    fifo_drv->drv_deinit(unit);
}

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
int bcmcth_mon_ft_fifo_sw_cache_refresh (int unit, int pipe,
                                        uint32_t count)
{
    BCMCTH_MON_FT_FIFO_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(fifo_drv->sw_cache_refresh(unit, pipe, count));
exit:
    SHR_FUNC_EXIT();
}

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
int bcmcth_mon_ft_fifo_entry_count_get (int unit, int pipe,
                                        uint32_t *count)
{
    BCMCTH_MON_FT_FIFO_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(fifo_drv->entry_count_get(unit, pipe, count));
exit:
    SHR_FUNC_EXIT();
}

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
int bcmcth_mon_ft_fifo_behavior_set (int unit, int pipe,
                                        uint32_t behavior)
{
    BCMCTH_MON_FT_FIFO_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(fifo_drv->fifo_behavior_set(unit, pipe, behavior));
exit:
    SHR_FUNC_EXIT();
}

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
int bcmcth_mon_ft_fifo_sw_cache_traverse_and_report (int unit, int pipe,
                                        uint32_t count)
{
    BCMCTH_MON_FT_FIFO_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(fifo_drv->sw_cache_traverse_and_report(unit, pipe, count));
exit:
    SHR_FUNC_EXIT();
}

