/*! \file trm_internal.h
 *
 * Transaction manager internal header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TRM_INTERNAL_H
#define TRM_INTERNAL_H

#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>
#include <shr/shr_thread.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmtrm/generated/bcmtrm_ha.h>

/*!
 *\brief Macros to control entry and transaction ID
 */
#define OP_ID_BITS  23  /*!< number of bits for the operation ID */
#define OP_ID_MASK ((1<<OP_ID_BITS)-1)  /*!< operation ID mask   */

#define INIT_OP_ID(op_id, unit,interactive)         \
    if (interactive) {                              \
        op_id = (unit << OP_ID_BITS) + (1 << 31);   \
    } else {                                        \
        op_id = unit << OP_ID_BITS;                 \
    }

#define INCREMENT_OP_ID(id)  \
    if (((id) & OP_ID_MASK) == OP_ID_MASK) {    \
        (id) = (id) & (0xFFFFFFFF-OP_ID_MASK);  \
    } else {                                    \
        (id)++;                                 \
    }


typedef struct {
    enum {ENTRY_CB, TRANS_CB, ENTRY_NOTIF, NOTIF_EXIT} type;
    bcmtrm_entry_t *ltm_entry;
    shr_error_t status;
} bcmtrm_hw_notif_struct_t;

extern shr_lmm_hdl_t bcmtrm_ltm_entry_hdl;   /*!< used to obtain ltm entries */

extern sal_msgq_t bcmtrm_hw_notif_q;

extern bool bcmltm_notify_units[];

typedef void (trans_state_set_cb)(int unit,
                                  uint32_t state,
                                  uint32_t trans_id,
                                  uint32_t *hdl);

typedef void (trans_state_update_cb)(int unit,
                                     uint32_t state,
                                     uint32_t trans_id,
                                     uint32_t hdl);

typedef void (trans_state_terminate_cb)(int unit,
                                        uint32_t trans_id,
                                        uint32_t hdl);

typedef struct bcmtrm_trans_state_s {
   trans_state_set_cb *set_f;
   trans_state_update_cb *update_f;
   trans_state_terminate_cb *cancel_f;
   trans_state_terminate_cb *done_f;
} bcmtrm_trans_state_t;

/* Forward declaration */
struct bcmtrm_unit_resources_s;

typedef int (bcmtrm_entry_req_f)(bcmtrm_entry_t *entry,
                                 struct bcmtrm_unit_resources_s *unit_res);
typedef int (bcmtrm_trans_req_f)(bcmtrm_trans_t *trans,
                                 struct bcmtrm_unit_resources_s *unit_res);

/*!
 * \brief Per unit control information
 */
typedef struct bcmtrm_unit_resources_s {
    /*! Modeled message Q */
    sal_msgq_t model_queue;
    /*! Interactive message Q */
    sal_msgq_t interact_queue;
    /*! mutex for the unit modeled operations */
    sal_mutex_t mtx;
    /*! mutex for the unit interactive operations */
    sal_mutex_t interact_mtx;
    /*! Thread handle for modeled thread. */
    shr_thread_t model_t_hdl;
    /*! Thread handle for interactive thread. */
    shr_thread_t inter_t_hdl;
    /*! true if the unit had been initialized */
    bool initialized;
    /*! the unit associated with this resource */
    int unit;
    /*! indicates that the unit is in stop state */
    bool stop_api;
    /*! Operation ID - increment number */
    uint32_t op_id;
    /*! Transaction ID */
    uint32_t trans_id;
    /*! HA memory pointer */
    trn_info_t *ha_mem;
    /*! HA memory for interactive tables */
    trn_interact_info_t *inter_ha_mem;
    /*! Counts the active synchronous transactions */
    uint32_t active_sync_trans;
    /*! Counts the interactive active synchronous transactions */
    uint32_t active_sync_trans_int;
    /*! The name of model thread instance */
    char *model_thread_name;
    /*! The name of interactive thread instance */
    char *inter_thread_name;
    /*! Assigned entry request function depends on atomic trans support */
    bcmtrm_entry_req_f *ent_req;
    /*! Assigned transaction request function depends on atomic trans support */
    bcmtrm_trans_req_f *trans_req;
    /*!
     * Synchronize sem used for synchronous operations to block until the
     * operation is completed.
     */
    sal_sem_t sync_sem;
} bcmtrm_unit_resources_t;



extern bcmtrm_trans_state_t bcmtrm_trans_ha_state;

/*!
 * \brief Process single modeled entry.
 *
 * This function processes a single modeled entry by calling the proper LTM
 * function according to the opcode of the entry. This function calls the
 * ltm action function followed by the ltm commit function. If needed, a
 * callback to the entity issuing the entry operation will be made.
 *
 * \param [in] entry Entry is the table entry to process.
 * \param [in,out] op_id Entry operation ID to use for this request. This
 * number will be incremented by this function after the operation.
 * \param [in,out] trans_id Transaction ID to use for this request. This
 * number will be incremented by this function after the operation.
 *
 * \retval SHR_E_NONE on success and error code for failure.
 */
extern int bcmtrm_process_entry_req(bcmtrm_entry_t *entry,
                                     uint32_t *op_id,
                                     uint32_t *trans_id);

/*!
 * \brief Process single interactive entry.
 *
 * This function processes a single interactive entry by calling the proper
 * LTM function according to the opcode of the entry. Calling the LTM with
 * interactive entry is a blocking call that will blocks until the hardware
 * had been updated. Therefore, there is no reason to call the ltm commit
 * function for this entry.
 *
 * \param [in] entry Entry is the table entry to process.
 * \param [in] op_id Entry operation ID to use for this request.
 * \param [in] ha_mem Is the HA memory to store the operation in case
 * of HA event.
 *
 * \retval SHR_E_NONE on success and error code for failure. Error codes
 * will reflect the return value from the LTM layer.
 */
extern int bcmtrm_process_interactive_entry_req(bcmtrm_entry_t *entry,
                                                uint32_t op_id,
                                                trn_interact_info_t *ha_mem);

/*!
 * \brief Memory allocator for entry field.
 *
 * This function allocates an entry field data structure. The function is
 * used by the LTM for lookup operations. The object is actually allocated
 * from the bcmlt pool via a function provided to the bcmtrm_init()
 * function.
 *
 * \retval pointer to entry field memory on success and NULL on failure.
 */
extern bcmltm_field_list_t* bcmtrm_local_field_alloc(void);


/*!
 * \brief Calls proper LTM staging function.
 *
 * This function allocates an ltm entry structure and call the proper ltm
 * action function based on the entry opcode.
 *
 * \param [in] entry Entry is the table entry to process.
 * \param [in] op_id Entry operation ID to use for this request.
 * \param [in] trans_id Transaction ID to use for this request.
 *
 * \retval The value as it returned from the ltm action function.
 */
int bcmtrm_stage_entry(bcmtrm_entry_t *entry,
                       uint32_t op_id,
                       uint32_t trans_id);

/*!
 * \brief Process a transaction.
 *
 * This function processes a transaction by processing the entries one by
 * one. Atomic transactions will be comitted once after all the entry
 * operations were successfully completed. When processing batch
 * transaction the function treat each entry individually.
 *
 * \param [in] trans Transaction to process by this function.
 * \param [in,out] op_id Entry operation ID to use for this request. During
 * batch processing every entry triggers the function to increment this number.
 * \param [in,out] trans_id Transaction ID to use for this request. During
 * batch processing every entry triggers the function to increment this number.
 * \param [in] user_data Is a synchronous object that synchronizes the caller
 * with the execution of the transaction.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_handle_transaction(bcmtrm_trans_t *trans,
                                     uint32_t *op_id,
                                     uint32_t *trans_id,
                                     void *user_data);

/*!
 * \brief Process interactive transaction.
 *
 * This function processes a transaction that is composed of interactive
 * (or PT) entries only. This transaction must be a batch transaction.
 *
 * \param [in] trans points to the transaction to process.
 * \param [in,out] op_id pointer to the initial entry operation ID.
 * Every entry increments this number.
 * \param [in] ha_mem Is the HA memory to store the operation in case
 * of HA event.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_handle_interactive_transaction(
                                    bcmtrm_trans_t *trans,
                                    uint32_t *op_id,
                                    trn_interact_info_t *ha_mem);

/*!
 * \brief Inform the caller about entry status change.
 *
 * This function informs the caller of the entry operation about a change
 * in the status of the operation. Informing the caller will be done if
 * the notification option matches the option requested by the caller,
 * unless the status indicates error which will always result in
 * informing the caller.
 *
 * \param [in] entry points to the entry to process.
 * \param [in] status is the current entry status.
 * \param [in] notify_opt is the reason for the notification.
 *
 * \retval none.
 */
extern void bcmtrm_appl_entry_inform(bcmtrm_entry_t *entry,
                        shr_error_t status,
                        bcmlt_notif_option_t notif_opt);

/*!
 * \brief Inform the caller about transaction status change.
 *
 * This function informs the caller of the entry operation about a change
 * in the status of the operation. Informing the caller will be done if
 * the notification option matches the option requested by the caller,
 * unless the status indicates error which will always result in
 * informing the caller.
 *
 * \param [in] trans points to the transaction to process.
 * \param [in] status is the current transaction status.
 * \param [in] notify_opt is the reason for the notification.
 *
 * \retval none.
 */
extern void bcmtrm_appl_trans_inform(bcmtrm_trans_t *trans,
                                  shr_error_t status,
                                  bcmlt_notif_option_t notif_opt);

/*!
 * \brief Process ltm results for modeled entry.
 *
 * This function process the ltm structure after the entry was processed by the
 * ltm. If there are output fields (in case of lookup) it moves them to the
 * entry fields list. It then release the ltm entry.
 *
 * \param [in] entry points to the entry to process.
 * \param [in] ltm_entry points to the ltm entry to process.
 *
 * \retval none.
 */
extern void bcmtrm_proc_entry_results(bcmtrm_entry_t *entry,
                             bcmltm_entry_t *ltm_entry);

/*!
 * \brief Converts LT opcodes to string.
 *
 * This function used for debug purposes only. It returns a string that is
 * corresponding to the opcode provided as input parameter.
 *
 * \param [in] opcode The opcode to convert.
 *
 * \retval The string associated with the given opcode.
 */
extern const char* bcmtrm_ltopcode_to_str(bcmlt_opcode_t opcode);

/*!
 * \brief Set the states of all entries in a transaction.
 *
 * This function sets the state of all the entries in the transaction to the
 * desired state.
 *
 * \param [in] trans point to the transaction to process.
 * \param [in] state is the desired state that the entries should be set with.
 *
 * \retval none.
 */
extern void bcmtrm_trans_entries_state_set(bcmtrm_trans_t *trans, int state);

/*!
 * \brief Queue interactive entry for processing the the interactive thread.
 *
 * This function being called when an interactive entry being processed in a
 * batch transaction. Since this action is blocking, its being transfered to be
 * processed by the interactive thread so the modeled thread can continue
 * without being blocked.
 *
 * \param [in] entry points to the entry to process.
 * \param [in] priority indicates the priority level of which this entry should
 * be processed. It is taken from the transaction priority.
 *
 * \retval none.
 */
extern void bcmtrm_q_interactive_entry(bcmtrm_entry_t *entry,
                                    bcmlt_priority_level_t priority);

/*!
 * \brief Process transaction operation complete.
 *
 * This function being called when the last entry of a transaction had been
 * processed. It makes sure that all the ltm entries had been released and than
 * it makes sure that the proper completion callback will be made. This
 * function is called only when the h/w operation was completed or if an error
 * had occur and the processing of the transaction had stopped.
 *
 * \param [in] trans points to the transaction to process.
 * \param [in] notify_opt is the reason for the notification.
 * \param [in] status is the current transaction status.
 *
 * \retval none.
 */
extern void bcmtrn_trans_cb_and_clean(bcmtrm_trans_t *trans,
                                    bcmlt_notif_option_t notif_opt,
                                    shr_error_t status);

/*!
 * \brief Process unit stop event.
 *
 * This function is called to handle a unit stop event. When the function
 * completes its action, the threads associated with the unit should have been
 * terminated.
 * Stopping the threads can be done gracefully or not, based on the graceful
 * input parameter.
 *
 * \param [in] unit is the unit nuber.
 * \param [in] graceful indicates the method to stop the threads. Graceful
 * methods will allow the threads to process everything they currently have
 * inside their queue while in non-graceful mode the thread only finishes
 * its current on-going operation.
 *
 * \retval SHR_E_NONE on success and SHR_E_TIMEOUT on failure.
 */
extern int bcmtrm_unit_stop(int unit, bool graceful);

/*!
 * \brief Free all transaciton resources.
 *
 * This function being called to free all the transaction resources once
 * the processing of the transaction had completed.
 *
 * \param [in] trans points to the transaction to free.
 *
 * \retval none.
 */
extern void bcmtrm_trans_done(bcmtrm_trans_t *trans);

/*!
 * \brief Notification thread.
 *
 * This threads handles the application notification for all units.
 * Notification can be made for asynchronous operation as well as table
 * change notifications.
 *
 * \param [in] arg Arg is used to communicate between the thread and the
 * thread creator. It is used as a counter to signal that the thread is
 * running or existing.
 *
 * \retval none.
 */
extern void bcmtrm_notify_thread(shr_thread_t t_hdl, void *arg);

/*!
 * \brief Allocates table event vector.
 *
 * This function allocates table event vector to store function callback
 * associated with the tables. This function being called during unit
 * initialization.
 *
 * \param [in] unit Unit associated with the table events.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_table_event_alloc(int unit);

/*!
 * \brief Free table event vector.
 *
 * This function frees table event vector, typically when a unit being
 * detached.
 *
 * \param [in] unit Unit associated with the table events.
 *
 * \retval none.
 */
extern void bcmtrm_table_event_free(int unit);

/*!
 * \brief Retrieve mutext associated with a unit.
 *
 * This function retrieves a mutex associated with a particular unit. The
 * purpose of the mutex is to prevent multiple staging operations on the
 * same unit simultaneously.
 *
 * \param [in] unit Unit to retrieve the mutex for.
 *
 * \retval Mutex handle. This function should not fail unless TRM init fails.
 */
extern sal_mutex_t bcmtrm_unit_mutex_get(int unit);

/*!
 * \brief Retrieve mutext protecting entry state change.
 *
 * This function retrieves the mutex that allows the state of the entry to
 * be updated. This function returns a mutext that is global as oppose
 * to per unit.
 *
 * \retval Mutex handle. This function should not fail unless TRM init fails.
 */
extern sal_mutex_t bcmtrm_entry_mutex_get(void);

/*!
 * \brief Free the entry and its resources.
 *
 * Calling to this function when all the operations associated with the entry
 * had been completed. This function frees (recycle) all the resources
 * associated with the entry.
 *
 * \param [in] entry Entry to recycle back to the free list.
 *
 * \retval none.
 */
extern void bcmtrm_entry_done(bcmtrm_entry_t *entry);

/*!
 * \brief The last thing to do in the processing of a transaction
 *
 * This function is only significant for asynch transactions. These types of
 * transactions can be freed asynchronously. This function therefore check
 * if the delete flag was set for this transaction. If it did it will free
 * the transaction. In any case, it will only mark the transaction state to
 * T_ACTIVE. Being in this state enables the bcmtrm_trans_free() to free the
 * transaction.
 *
 * \param [in] trans Transaction to handle for completion.
 *
 * \retval True if transaction was deleted. False otherwise.
 */
extern bool bcmtrm_trans_complete(bcmtrm_trans_t *trans);

/*!
 * \brief The last thing to do in the processing of an entry.
 *
 * This function is only significant for asynch entries. These types of
 * entries can be freed asynchronously. This function therefore check
 * if the delete flag was set for this entry. If it did it will free
 * the entry. In any case, it will only mark the entry state to E_ACTIVE.
 * Being in this state enables the bcmtrm_entry_free() to free the entry.
 *
 * \param [in] entry Entry to handle for completion.
 *
 * \retval none.
 */
extern void bcmtrm_entry_complete(bcmtrm_entry_t *entry);

/*!
 * \brief Recover all open transaction during warm boot.
 *
 * This function go through all the open transactions and decides, based on
 * their state, to abort or commit them. The function recovers the transactions
 * until nothing to verify or until no more
 *
 * \param [in] unit Is the recovered unit number.
 *
 * \retval SHR_E_NONE on success and error code when a particular transaction
 * failed to be recovered
 */
extern int bcmtrm_recover(int unit);

/*!
 * \brief Recovers specific transaction.
 *
 * This function recovered specific transaction of the given unit. This
 * function calls, based on the transaction state, to abort or commit the
 * transaction.
 *
 * \param [in] unit Is the unit where the recovery operation being performed.
 * \param [in] trn_state Is the transaction state structure. It contains the
 * transaction ID and transaction state.
 *
 * \retval SHR_E_NONE on success and SHR_E_BUSY when entry execution stalled.
 */
extern int bcmtrm_recover_trans(int unit, trn_state_t *trn_state);

/*!
 * \brief Recovers interactive operation
 *
 * This function releats the last interactive operation by issuing the
 * complete entry operation.
 *
 * \param [in] unit Is the unit where the recovery operation being performed.
 * \param [in] trn_state Is the transaction state structure. It contains the
 * transaction ID and transaction state.
 *
 * \retval none.
 */
extern void bcmtrm_recover_interact(int unit, trn_interact_info_t *ha_mem);


/*!
 * \brief South bound table change notification.
 *
 * This function receives an LTM entry change event and post it into the
 * notify thread.
 * This function only operates on logical tables.
 *
 * \param [in] unit is the unit number.
 * \param [in] high_pri Indicates if the event is high priority event.
 * \param [in] table_id is the logical table ID for the table
 * associated with the change.
 * \param [in] opcode is the operation code that was perform. Note that
 * the application needs to know if it was insert/update or delete operation.
 * \param [in] fields is a list of the entry fields. The fields will
 * be used by the TRM asynchronously and therefore the application
 * should not free the fields. The fields list should be allocated using
 * the shared field memory manager (shr_fmm) utility.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int bcmtrm_sb_table_notify(int unit,
                                  bool high_pri,
                                  uint32_t table_id,
                                  bcmlt_opcode_t opcode,
                                  bcmltm_field_list_t *fields);


/*!
 * \brief Send table changed event to registered application.
 *
 * This function checks if an application callback function was registered
 * to receive change notifications. If a function was found, it will be
 * called with the modified entry.
 *
 * \param [in] entry Is the changed entry in the table.
 *
 * \return None.
 */
extern void bcmtrm_tbl_chg_event(bcmtrm_entry_t *entry);

/*!
 * \brief Request to process entry synchronously in a single thread mode.
 *
 * This function process entry request in a single thread mode. The request will
 * be processed within the context of the calling thread. Furthermore, all
 * the notification that related to this entry operation will be done from
 * within the calling thread.
 * To operate in this mode the system must be configured to disable atomic
 * transaction (default configuration) by configuring the feature control not to
 * enable atomic transactions.
 * The commit operation will not provide a callback
 * function. Hence this function might return before the entry had been
 * installed into the h/w. The control of whether the function must return
 * only after the entry had been installed into the h/w or not is done by the
 * WAL bypass configuration flag. This flag is configurable parameter per unit.
 *
 * \param [in] entry Entry to be processed.
 * \param [in] unit_res Is the unit related resources. This function uses this
 * resource to lock the access to the unit (model or interactive path) among
 * other things.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_no_trd_entry_req(bcmtrm_entry_t *entry,
                                   bcmtrm_unit_resources_t *unit_res);

/*!
 * \brief Request to process transaction synchronously in a single thread mode.
 *
 * This function process transaction request in a single thread mode.
 * The request will be processed within the context of the calling thread.
 * Furthermore, all the notification that related to this transaction operation
 * (only batch transactions allowed) will be done from within the calling
 * thread.
 * To operate in this mode the system must be configured to disable atomic
 * transaction (default configuration) by configuring the feature control not to
 * enable atomic transactions.
 * The commit operation will not provide a callback
 * function. Hence this function might return before the entry had been
 * installed into the h/w. The control of whether the function must return
 * only after the entry had been installed into the h/w or not is done by the
 * WAL bypass configuration flag. This flag is configurable parameter per unit.
 *
 * \param [in] trans Transaction to be processed.
 * \param [in] unit_res Is the unit related resources. This function uses this
 * resource to lock the access to the unit (model or interactive path) among
 * other things.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_no_trd_trans_req(bcmtrm_trans_t *trans,
                                   bcmtrm_unit_resources_t *unit_res);

/*!
 * \brief Records an entry in non-volatile memory.
 *
 * This function records an entry and its operation within a non-volatile
 * memory so it can be played later.
 *
 * \param [in] entry The entry to record.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Write operation failed.
 */
extern int bcmtrm_entry_rec(bcmtrm_entry_t *entry);

/*!
 * \brief Records a transaction in non-volatile memory.
 *
 * This function records a transaction and its operation within a non-volatile
 * memory so it can be played later.
 *
 * \param [in] trans The transaction to record.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Write operation failed.
 */
extern int bcmtrm_trans_rec(bcmtrm_trans_t *trans);


#endif /* TRM_INTERNAL_H */
