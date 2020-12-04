/*! \file event_mgr.h
 *
 * Event manager module definitions and APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_EVENT_MGR_H
#define BCMI_LTSW_EVENT_MGR_H

#include <sal/sal_types.h>

/*!
 * \brief Initialize the Event Management Module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY Out of memory.
 */
extern int
bcmi_event_mgr_init(int unit);

/*!
 * \brief De-initialize the Event Management Module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_INIT Not initialized.
 * \retval SHR_E_TIMEOUT Failed to stop assist threads.
 */
extern int
bcmi_event_mgr_deinit(int unit);

/*!
 * \brief Callback definition for event notification process.
 *
 * The signature of the callback function that processes notifications for
 * subscribed event or table updates.
 *
 * The client can pass a user_data context which will be passed back as input
 * parameter to the callback notification function.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event/table name.
 * \param [in] notif_info The table/event notification info.
 * \param [in] user_data The application context provided at the time of
 * table subscription (see bcmi_lt_table_subscribe()).
 *
 * \return none
 */
typedef void (*bcmi_ltsw_event_cb)(int unit,
                                   const char *event,
                                   void *notif_info,
                                   void *user_data);

/*!
 * \brief Register callback function to receive event.
 *
 * This function can be used to receive notifications of the specified event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] callback Callback function to call when the event being posted.
 * \param [in] user_data Opaque context for notifications.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int bcmi_ltsw_event_register(int unit,
                                    const char *event,
                                    bcmi_ltsw_event_cb callback,
                                    void *user_data);
/*!
 * \name Event attributes.
 * \anchor BCMI_LTSW_EVENT_xxx
 */

/*! \{ */
/*! Application callback is used to process the event. */
#define BCMI_LTSW_EVENT_APPL_CALLBACK (1 << 0)

/*! The specificed event has high priority. */
#define BCMI_LTSW_EVENT_HIGH_PRIORITY (1 << 1)

/*! \} */

/*!
 * \brief Set the attributes of event.
 *
 * This function sets attributes of an event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] attrib The attribute to set.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmi_ltsw_event_attrib_set(int unit, const char *event, uint32_t attrib);

/*!
 * \brief Deregister a callback to an event.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] event Event name.
 * \param [in] callback The callback function to be deregistered.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
extern int
bcmi_ltsw_event_unregister(int unit,
                           const char *event,
                           bcmi_ltsw_event_cb callback);

/*!
 * \brief Notify of an event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] notif_info Data associated with the event.
 * \param [in] notif_info_len The length of data associated with the event.
 *
 * \return None.
 */
extern void
bcmi_ltsw_event_notify(int unit,
                       const char *event,
                       void *notif_info,
                       int notif_info_len);

/*!
 * \name Event status returned by parser callback.
 * \anchor bcmiLtswEventStatusxxx
 */
/*! \{ */
typedef enum bcmi_ltsw_event_status_e {

    /*! Pass the notification. */
    bcmiLtswEventStatusPassNotif = 0,

    /*! Dismiss the notification.*/
    bcmiLtswEventStatusDismissNotif = 1,

    /*! The last one, not valid.*/
    bcmiLtswEventStatusCount = 2

} bcmi_ltsw_event_status_t;

/*! \} */

/*!
 * \brief Callback definition for table notification parser.
 *
 * The signature of the callback function that parses notifications for
 * subscribed table updates.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name Table name.
 * \param [in] entry_hdl Table entry handle.
 * \param [out] notif_info Parsed notification info.
 * \param [out] status Parser callback returned status, NULL for not care.
 *
 * The notif_info is allocated and freed inside the lt_event_mgr module.
 * The size of notif_info is given by user when subscribes the table changes
 * via \ref bcmi_lt_table_subscribe.
 * The registered notification parser will populate the notif_info, which will
 * be passed as input parameter to notification process callback
 * (\ref bcmi_lt_event_cb).
 *
 * \return none
 */
typedef void (*bcmi_lt_parser)(int unit,
                               const char *lt_name,
                               uint32_t entry_hdl,
                               void *notif_info,
                               bcmi_ltsw_event_status_t *status);


/*!
 * \brief Subscribe for table updates.
 *
 * This function registers a callback to get notified on any changes of a
 * particular table within a unit.
 * The user may provide a pointer to an context that can be used during the
 * callback event.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] lt_name The table name for subscribing to updates.
 * \param [in] callback Notification handler to be invoked.
 * \param [in] user_data Opaque context for notifications.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
int
bcmi_lt_table_subscribe(int unit,
                        const char *lt_name,
                        bcmi_ltsw_event_cb callback,
                        void *user_data);

/*!
 * \brief Set the parser of table updates.
 *
 * This function registers a callback to parse the table updates.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] lt_name The table name for subscribing to updates.
 * \param [in] parser Notification parser to be invoked.
 * \param [in] notif_info_len Length of notification info.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
int
bcmi_lt_table_parser_set(int unit,
                         const char *lt_name,
                         bcmi_lt_parser parser,
                         uint32_t notif_info_len);

/*!
 * \brief Set the attributes of table change event.
 *
 * This function sets attributes of table change event.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name Table name.
 * \param [in] attrib The attribute to set.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmi_lt_table_attrib_set(int unit, const char *lt_name, uint32_t attrib);

/*!
 * \brief Unsubscribe for table updates.
 *
 * This function deregisters a callback to get notified on any table updates.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] lt_name The table name.
 * \param [in] callback The callback function to be unregister.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
int
bcmi_lt_table_unsubscribe(int unit,
                          const char *lt_name,
                          bcmi_ltsw_event_cb callback);

/*!
 * \brief Get number of message pending in queue for given event.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] event Event name.
 * \param [out] num Number of pending message.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_NOT_FOUND No handle for given event.
 */
int
bcmi_ltsw_event_msg_num_get(int unit, const char *event, uint32_t *num);

/*!
 * \brief Set the threshold of message pending in the queue.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name Table name.
 * \param [in] threshold Threshold.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
extern int
bcmi_ltsw_event_msg_threshold_set(int unit, const char *event,
                                  uint32_t threshold);

/*!
 * \name Event string ID.
 * \anchor bcmiltswEv<xxx>
 */

/*! \{ */

/*! Link state update event for application. */
#define BCMI_LTSW_EV_LS_APPL "evLinkStateAppl"

/*! L2 event string used for L2 notification */
#define BCMI_LTSW_EV_L2_NOTIF "evL2Notif"

/*! PFC deadlock recovery event. */
#define BCMI_LTSW_EV_PFC_DEADLOCK "evPfcDeadlock"

/*! \} */

#endif /* BCMI_LT_EVENT_MGR_H */
