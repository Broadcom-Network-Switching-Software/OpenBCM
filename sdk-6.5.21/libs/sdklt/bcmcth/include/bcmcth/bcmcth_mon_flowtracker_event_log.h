/*! \file bcmcth_mon_flowtracker_event_log.h
 *
 * BCMCTH Flowtracker LT MON_FLOWTRACKER_LEARN_EVENT_LOG
 * related definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_EVENT_LOG_H
#define BCMCTH_MON_FLOWTRACKER_EVENT_LOG_H
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>


/*!
 * The MON_FLOWTRACKER_LEARN_EVENT_LOG table is used for
 * reporting flow learn events. This structure holds the LT's
 * fields.
 */
typedef struct bcmcth_mon_flowtracker_event_log_entry_s {
    /*! Index into the LT */
    uint32_t mon_flowtracker_event_log_id;

    /*! Pipe on which the flow as learnt */
    uint8_t pipe;

    /*! Exact match index mode */
    bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode;

    /*! Exact match table index within the pipe */
    uint32_t exact_match_idx;

    /*! Index into the MON_FLOWTRACKER_GROUP table */
    uint32_t mon_flowtracker_group_id;
}bcmcth_mon_flowtracker_event_log_entry_t;

/*!
 * \brief Initialize the datastructures required for the event log library.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_event_log_init(int unit, bool warm);

/*!
 * \brief De-initialize the datastructures allocated for the event log library.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
extern void
bcmcth_mon_ft_learn_event_log_deinit(int unit);

/*!
 * \brief Dummy insert function for all entries since the interrupt handler
 *        will only be doing update.
 *        NOTE: Should not be called during Warmboot.
 *        NOTE: Should be called during module init and not dynamically.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_event_log_dummy_entries_insert(int unit);

/*!
 * \brief Dummy insert function for MON_FLOWTRACKER_LEARN_EVENT_LOG_STATUS entry since the
 *        bcmcth_mon_ft_learn_event_log_entry_update will only be doing update.
 *        NOTE: Should not be called during Warmboot.
 *        NOTE: Should be called during module init and not dynamically.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_event_log_status_dummy_entry_insert(int unit);

/*!
 * \brief Update the next entry with the given entry information.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_data Entry information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_event_log_entry_update(
                            int unit,
                            bcmcth_mon_flowtracker_event_log_entry_t *entry_data);

#endif /* BCMCTH_MON_FLOWTRACKER_EVENT_LOG_H */
