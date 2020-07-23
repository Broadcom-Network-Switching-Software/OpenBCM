/*! \file switch.h
 *
 * BCM switch control module interfaces for LTSW internal.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_SWITCH_H
#define BCMI_LTSW_SWITCH_H

#include <bcm/error.h>
#include <bcm/switch.h>
#include <sal/sal_types.h>

/*!
 * \brief Initializes the switch module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_switch_init(int unit);

/*!
 * \brief De-initializes the switch module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_switch_detach(int unit);

/*!
 * \brief Generate switch event to application.
 *
 * When the function is called, all callback functions are executed.
 *
 * \param [in] unit Unit number.
 * \param [in] event Switch event.
 * \param [in] arg1 The first callback function argument.
 * \param [in] arg2 The second callback function argument.
 * \param [in] arg3 The third callback function argument.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_switch_event_generate(int unit,  bcm_switch_event_t event,
                                uint32 arg1, uint32 arg2, uint32 arg3);

/*!
 * \brief Covert a drop event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid drop event enum value.
 * \retval SHR_E_NOT_FOUND The symbol string not found for the specified drop event.
 */
extern int
bcmi_ltsw_switch_drop_event_to_symbol(int unit, bcm_pkt_drop_event_t drop_event,
                                      const char **table, const char **symbol);

/*!
 * \brief Covert a trace event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid trace event enum value.
 * \retval SHR_E_NOT_FOUND The symbol string not found for the specified trace event.
 */
extern int
bcmi_ltsw_switch_trace_event_to_symbol(int unit, bcm_pkt_trace_event_t trace_event,
                                       const char **table, const char **symbol);

/*!
 * \brief Set a mirror instanced id to an EP recirculate profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id EP Recirculate profile id.
 * \param [in] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_set(int unit,
                                                       int profile_id,
                                                       int mirror_instance_id);

/*!
 * \brief Get the mirror instance id from EP recirculate profile entries.
 *
 * \param [in] unit Unit number.
 * \param [out] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_get(int unit,
                                                       int *mirror_instance_id);

#endif /*! BCMI_LTSW_SWITCH_H */
