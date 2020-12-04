/*! \file bcmcth_mon_flowtracker_intr.h
 *
 * BCMCTH Flowtracker learn notification interrupt related prototypes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_INTR_H
#define BCMCTH_MON_FLOWTRACKER_INTR_H
/*!
 * \brief Enable/disable the top level interrupt for HW flow learn.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable/disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_learn_top_lvl_intr_enable (int unit, bool enable);

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
extern int bcmcth_mon_ft_learn_intr_enable (int unit, int pipe,
                                        bool enable);

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
extern int bcmcth_mon_ft_learn_fail_intr_enable (int unit, int pipe,
                                        bool enable);

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
extern int bcmcth_mon_ft_learn_intr_threshold_set (int unit, int pipe,
                                        uint16_t threshold);

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
extern int bcmcth_mon_ft_learn_fail_intr_threshold_set (int unit, int pipe,
                                        uint16_t threshold);

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
extern int bcmcth_mon_ft_learn_intr_status_get (int unit, int pipe,
                                        bool *lrn_intr_set,
                                        bool *lrn_fail_intr_set);

/*!
 * \brief Get the interrupt status for HW flow learn failure.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [out] intr_set Interrupt is set/not set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_learn_fail_intr_status_get (int unit, int pipe,
                                        bool *intr_set);

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
extern int bcmcth_mon_ft_learn_intr_handler_set (int unit,
                                          bcmbd_intr_f intr_hdlr,
                                          uint32_t intr_param);
/*!
 * \brief Clear the interrupt status for HW flow learn.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_learn_intr_status_clear (int unit, int pipe);

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
extern int bcmcth_mon_ft_learn_intr_enable_get (int unit, int pipe,
                                         bool *lrn_intr_ena,
                                         bool *lrn_fail_intr_ena);

/*!
 * \brief Clear the HW flow learn failure counter.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO IO access failed.
 */
extern int bcmcth_mon_ft_learn_fail_counter_clear (int unit, int pipe);
#endif /* BCMCTH_MON_FLOWTRACKER_INTR_H */
