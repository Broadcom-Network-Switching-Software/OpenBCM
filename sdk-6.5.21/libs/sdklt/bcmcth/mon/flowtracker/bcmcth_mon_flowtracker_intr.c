/*! \file bcmcth_mon_flowtracker_intr.c
 *
 * Flowtracker learn INTR driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_intr.h>

/******************************************************************************
* Defines
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

#define BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit)\
    bcmcth_mon_flowtracker_intr_drv_t *intr_drv = NULL;\
    SHR_FUNC_ENTER(unit);\
    intr_drv = bcmcth_mon_ft_intr_drv_get(unit);\
    SHR_NULL_CHECK(intr_drv, SHR_E_INIT);

/******************************************************************************
* Public functions
 */
/*!
 * \brief Enable/disable the top level interrupt for HW flow learn.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable/disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_top_lvl_intr_enable (int unit, bool enable)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(intr_drv->ft_learn_top_lvl_intr_enable(unit, enable));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable the interrupt for HW flow learn.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] enable Enable/disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_intr_enable (int unit, int pipe,
                                        bool enable)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(intr_drv->ft_learn_intr_enable(unit, pipe, enable));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable the interrupt for HW flow learn failure.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] enable Enable/disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_fail_intr_enable (int unit, int pipe,
                                        bool enable)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(intr_drv->ft_learn_fail_intr_enable(unit, pipe, enable));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the interrupt threshold for HW flow learn FIFO entries.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] threshold Threshold value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_intr_threshold_set (int unit, int pipe,
                                        uint16_t threshold)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(intr_drv->ft_learn_intr_threshold_set(unit, pipe, threshold));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the interrupt threshold for HW flow learn failure count.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in] threshold Threshold value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_fail_intr_threshold_set (int unit, int pipe,
                                        uint16_t threshold)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(intr_drv->ft_learn_fail_intr_threshold_set(unit, pipe, threshold));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the interrupt status for HW flow learn.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [out] lrn_intr_set Learn interrupt is set/not set.
 * \param [out] lrn_fail_intr_set Learn fail interrupt is set/not set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_intr_status_get (int unit, int pipe,
                                        bool *lrn_intr_set,
                                        bool *lrn_fail_intr_set)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(
        intr_drv->ft_learn_intr_status_get(unit, pipe,
                                            lrn_intr_set, lrn_fail_intr_set));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the interrupt handler for HW flow learn top level interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_hdlr  Interrupt handler function.
 * \param [in] intr_param Interrupt handler context value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_intr_handler_set (int unit,
                                          bcmbd_intr_f intr_hdlr,
                                          uint32_t intr_param)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(
        intr_drv->ft_learn_intr_handler_set(unit, intr_hdlr, intr_param));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the interrupt status for HW flow learn.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_intr_status_clear (int unit, int pipe)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(
        intr_drv->ft_learn_intr_status_clear(unit, pipe));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the interrupt enable status for HW flow learn success and failure.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [out] lrn_intr_ena Learn interrupt is enable or not.
 * \param [out] lrn_fail_intr_ena Learn fail interrupt is enabled or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_intr_enable_get (int unit, int pipe, bool *lrn_intr_ena,
                                         bool *lrn_fail_intr_ena)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(
        intr_drv->ft_learn_intr_enable_get(unit, pipe, lrn_intr_ena,
                                            lrn_fail_intr_ena));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the HW flow learn failure counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
int bcmcth_mon_ft_learn_fail_counter_clear (int unit, int pipe)
{
    BCMCTH_MON_FT_INTR_DRV_FN_ENTRY_COMMON(unit);

    SHR_IF_ERR_EXIT(
        intr_drv->ft_learn_fail_counter_clear(unit, pipe));
exit:
    SHR_FUNC_EXIT();
}
