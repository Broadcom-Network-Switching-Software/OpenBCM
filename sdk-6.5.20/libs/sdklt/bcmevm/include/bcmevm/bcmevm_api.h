/*! \file bcmevm_api.h
 *
 *  BCM component generated notification API
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMEVM_API_H
#define BCMEVM_API_H

#include <bcmltm/bcmltm_types.h>
#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief North bound table change notification.
 *
 * This function is a north bound function registered with the component
 * generation table change notification component. This callback function
 * records the request and post it with one of the transaction manager
 * notification thread. Therefore this function will return fast.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Logical table ID for the table
 *                      associated with the change.
 * \param [in] opcode Operation code that was perform. Note that the
 *                    application needs to know if it was insert/update
 *                    or delete operation.
 * \param [in] fields List of entry fields.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
typedef int (bcmevm_cb)(int unit,
                        bool high_pri,
                        uint32_t table_id,
                        bcmlt_opcode_t opcode,
                        bcmltm_field_list_t *fields);

/*!
 * \brief North bound registration for callback function.
 *
 * This function registers north bound callback function that will
 * be call whenever the notification function \ref bcmevm_table_notify()
 * being called.
 *
 * \param [in] cb Is a pointer to callback function.
 *
 * \return None.
 */
extern void bcmevm_register_cb(bcmevm_cb *cb);

/*!
 * \brief South bound table change notification.
 *
 * This function is being called by a component that can update an LT
 * entry, for example a port state change. The function provides the
 * SDK components with the ability to inform the application about
 * such asynchronous events. The application is required to register a
 * callback function with a particular LT to receive table changes.
 *
 * If an entry was changed the component should only provide the
 * modified fields along with the key fields. Fields that were
 * unchanged should not be required to be present. If the application
 * would like to obtain the complete entry it can do this at any time
 * using a lookup operation. For this reason it is important to
 * provide the key fields with the entry.
 *
 * This function only works for LOGICAL tables as the application can
 * not register itself for changes in physical tables.
 *
 * \param [in] unit Unit number.
 * \param [in] high_pri Indicates if the event is high priority event.
 * \param [in] table_id Logical table ID for the table associated with
 *                      the change.
 * \param [in] opcode Operation code that was performed. Note that the
 *                    application needs to know if it was insert/update
 *                    or delete operation.
 *
 * \param [in] fields List of entry fields. The fields will be used by
 *                    the TRM asynchronously and therefore the
 *                    application should not free the fields. The
 *                    fields list should be allocated using the shared
 *                    field memory manager (shr_fmm) utility.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmevm_table_notify(int unit,
                               bool high_pri,
                               uint32_t table_id,
                               bcmlt_opcode_t opcode,
                               bcmltm_field_list_t *fields);


/*!
 * \brief Register to receive table change events.
 *
 * This function being called by a component that is interested to receive
 * notification for LT changes that are made by a component (i.e. south bound
 * changes). Changes made to LT by the application (north bound changes) will
 * not be notified to the component via this mechanism.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Identifier of the table of interest.
 * \param [in] cb Caller provided callback function.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmevm_register_tbl_event(int unit,
                                     uint32_t table_id,
                                     bcmevm_cb *cb);

/*!
 * \brief Extended event data.
 * This structure can be used to send extended event data. It is the
 * responsibility of the event posting function to allocate and set this
 * structure.
 * The idea is to use this structure as the event data parameter for the
 * function \ref bcmevm_publish_event_notify(). In such case the user should
 * type cast the uint64_t parameter to a pointer for this data structure.
 * The caller will be able to allocate this structure on the stack as the
 * notification will occur within the context of the call to the function
 * \ref bcmevm_publish_event_notify().
 */
typedef struct {
    /*! Indicates the size (# of elements) of the data array. */
    uint32_t count;
    const uint64_t *data;      /*!< Array of elements. */
} bcmevm_extend_ev_data_t;

/*!
 * \brief Event callback function prototype.
 *
 * This function prototype used to carry event notifications for components
 * that were registered to receive such notifications.
 *
 * The callback function being called from the context of the event poster
 * thread. Therefore the action taken by the component should be minimal.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event for which the function had been called for.
 * \param [in] ev_data Data associated with the event.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
typedef void (bcmevm_event_cb)(int unit,
                               const char *event,
                               uint64_t ev_data);

/*!
 * \brief Register callback function to receive published event.
 *
 * This function can be used by components that wishes to receive notifications
 * of the specified event.
 *
 * It is expected that the call to this function will happen only after
 * the init state of the system manager.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] cb Callback function to call when the event being posted.
 *
 * \return SHR_E_NONE on success and error code on failure. Possible error codes
 * can be:
 * SHR_E_UNIT - for invalid unit.
 * SHR_E_PARAM - for invalid event or callback function.
 */
extern int bcmevm_register_published_event(int unit,
                                           const char *event,
                                           bcmevm_event_cb *cb);

/*!
 * \brief Unegister callback function from receiving published event.
 *
 * This function can be used by components that wishes to stop receiving
 * notifications of the specified event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] cb Registered callback function that was registered to
 * the receive the given event
 *
 * \return SHR_E_NONE on success and error code on failure. Possible error codes
 * can be:
 * SHR_E_UNIT - for invalid unit.
 * SHR_E_PARAM - for invalid event or callback function.
 */
extern int bcmevm_unregister_published_event(int unit,
                                            const char *event,
                                            bcmevm_event_cb *cb);

/*!
 * \brief Notify other components of an event.
 *
 * This function informs all the components that were interested in receiving
 * the posted event about the occurance of this event. The event can be
 * associated with 64 bit data field. This data will be past to every component
 * callback function.
 * The caller must understand that this function will block until all registered
 * components had been informed.
 * The caller should not be aware if any other component is registered to
 * receive this event or not.
 * If the event data exceeds the content of 64 bit the caller may use the
 * extended event data structure (see \ref bcmevm_extend_ev_data_t). To use
 * the extended event data the caller needs to allocate an extended event data.
 * Note that since the event will be processed within this function, the caller
 * may allocate the structure on the stack instead of dynamic allocation using
 * sal_malloc().
 * The receiver of each event must know if the event data is regular or
 * extended.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] ev_data Data associated with the event. There can be two classes
 * of data associated with the event. Regular event data of type uint64_t and
 * extended event data. The extended event data is a pointer to the type
 * \ref bcmevm_extend_ev_data_t. The caller may type cast the pointer to the
 * \ref bcmevm_extend_ev_data_t type to uint64_t. It is expected that the
 * receiver of the event would know the class of incoming event data.
 *
 * \return None.
 */
extern void bcmevm_publish_event_notify(int unit,
                                        const char *event,
                                        uint64_t ev_data);

#endif /* BCMEVM_API_H */
