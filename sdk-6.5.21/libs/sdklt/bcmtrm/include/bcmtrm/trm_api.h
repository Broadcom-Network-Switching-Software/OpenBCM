/*! \file trm_api.h
 *
 * This interface to the Logical Table Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TRM_API_H
#define TRM_API_H

#include <sal/sal_mutex.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmltd/bcmltd_lt_types.h>

/*!
 * \brief Default value for max active entries.
 *
 * The default number for maximal entries structures to create for all units.
 * This configurable parameter can be used to restrict the memory consumption
 * used by the entry containers.
 */
#define BCMTRM_DEFAULT_MAX_ACTIVE_ENTRIES  512

/*!
 * \brief Default value for max active transactions.
 *
 * The default number of the maximal transaction structures to create for all
 * units. This configurable parameter can be used to restrict the memory
 * consumption used by the transaction containers.
 */
#define BCMTRM_DEFAULT_MAX_ACTIVE_TRANSACTIONS 128

/* Forward decleration */
struct bcmtrm_trans_s;

/*!
 * \brief Combined entry callback functions.
 *
 * This data structure used to maintain the application callback function. The
 * same data structure used for PT and non PT entries callback.
 */
typedef union {
    bcmlt_entry_cb entry_cb;        /*!< Callback for non PT entry */
    bcmlt_entry_pt_cb entry_pt_cb;  /*!< Callback for PT entry     */
}bcmtrm_combined_entry_cb_t;

/*!
 * \brief Entry data structure.
 */

typedef struct bcmtrm_entry_s{
    bcmlt_entry_info_t       info;      /*!< Entry info provided by the user */
    enum {E_IDLE,                       /*!< Idle state - entry is free      */
        E_ACTIVE,                       /*!< Entry had been allocated        */
        E_COMMITTING,                   /*!< Entry during commit             */
        E_COMMITTED}         state;     /*!< Entry was committed             */
    uint32_t                 table_id;  /*!< Table ID for this entry         */
    bcmltm_combined_opcode_t opcode;    /*!< Entry opcode                    */
    bool                     pt;        /*!< Indicate passthrough or not     */
    bool                     interactive; /*!< True if interactive table     */
    bool                     asynch;    /*!< Indicate sync or async operation*/
    bool                     delete;    /*!< Delete the entry?               */
    bcmlt_priority_level_t   priority;  /*!< Entry priority                  */
    void                     *db_hdl;   /*!< Data base handle for this table */
    bcmtrm_combined_entry_cb_t cb;      /*!< Callback function               */
    void                     *usr_data; /*!< Application context             */
    struct bcmtrm_trans_s    *p_trans;  /*!< Pointer to mother transaction   */
    bcmltm_entry_t           *ltm_entry;/*!< Associated ltm transaction      */
    uint32_t                 attrib;    /*!< Attributes associated with entry*/
    uint32_t                 ha_trn_hdl;/*!< HA transaction handle           */
    /*! Memory handle to allocate field array */
    shr_lmm_hdl_t            fld_arr_mem_hdl;
    uint32_t                 max_fid;   /*!< Maximal field ID for the entry  */
    shr_fmm_t               **fld_arr;  /*!< Field array sorted base on FID  */
    shr_fmm_t                *l_field;  /*!< The list of fields (c++ list)   */
    /*! Memory handle to free field structure when operating in HP API */
    shr_lmm_hdl_t            fld_mem_hdl;
    /*! Sync object used to coordinate entry allocation&free */
    sal_mutex_t              entry_free_sync;
    shr_fmm_t                *free_fld; /*!< Free list of fields             */
    struct bcmtrm_entry_s    *next;     /*!< Next element on the free list   */
    struct bcmtrm_entry_s    *debug_next; /*!< Next element on the active list*/
    struct bcmtrm_entry_s    *debug_prev; /*!< Double link - previous element*/
} bcmtrm_entry_t;

/*!
 * \brief Data protection object.
 */
typedef struct bcmtrm_data_protect_object_s {
    sal_mutex_t   mutex;     /*!< Mutext for the protection                   */
    struct bcmtrm_data_protect_object_s *next;/*!< Link list for next element */
} bcmtrm_data_protect_object_t;

/*!
 * \brief Transaction data structure.
 */
typedef struct bcmtrm_trans_s {
    bcmlt_transaction_info_t info;         /*!< Infor provided by the user    */
    int                      unit;         /*!< The unit -                    */
    bool                     syncronous;   /*!< True if syncronous trans      */
    enum {T_IDLE,                        /*!< Idle state - trans is free      */
        T_ACTIVE,                        /*!< Trans had been allocated        */
        T_COMMITTING,                    /*!< Trans is process of committing  */
        T_COMMITTED}         state;      /*!< Transaction had been committed  */
    bool                     pt_trans; /*!< True - trans has only PT entries  */
    bool                     delete;       /*!< Delete the transaction?       */
    bcmlt_priority_level_t   priority;     /*!< Transaction priority          */
    bcmlt_trans_cb           cb;           /*!< Callback function             */
    void                     *usr_data;   /*!< Application context            */
    uint32_t                 processed_entries;/*!< Num of processed entries  */
    uint32_t                 committed_entries;/*!< Num of committed entries  */
    uint32_t                 commit_success;/*!< Num of successfully committed*/
    bcmtrm_entry_t           *l_entries;   /*!< List of entries (c++ list)    */
    bcmtrm_entry_t           *last_entry;   /*!< Last entry of the list       */
    /*!Protection for processed_entries*/
    bcmtrm_data_protect_object_t *lock_obj;
    /*! Sync object used to coordinate transaction allocate&free */
    sal_mutex_t              trans_free_sync;
    struct bcmtrm_trans_s    *next;     /*!< Required for object management   */
    struct bcmtrm_trans_s    *debug_next;  /*!< Required for debug            */
    struct bcmtrm_trans_s    *debug_prev; /*!< Double link - previous element */
} bcmtrm_trans_t;


/*!
 * \brief Field allocation callback function.
 *
 * Allocated memory for field.
 *
 * \return Pointer to allocated field data structure or NULL on failure.
 */
typedef bcmltm_field_list_t *(*bcmtrm_field_alloc)(void);

/*!
 * \brief Entry free callback function.
 *
 * Free entry and all its fields.
 *
 * \param [in] entry Points to the entry to be freed.
 *
 * \return none
 */
typedef void (*bcmtrm_entry_free_fn)(bcmtrm_entry_t *entry);

/*!
 * \brief Transaction free callback function.
 *
 * free transaction and all its entries.
 *
 * \param [in] trans points to the transaction to be freed.
 *
 * \return none.
 */
typedef void (*bcmtrm_trans_free_fn)(bcmtrm_trans_t *trans);

/*
 * TRM provides the following APIs.
 */

/*!
 * \brief initialize BCM-TRM component.
 *
 * This function should be called once during the system initialization. It
 * should be called prior to any unit attach function.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_init(void);

/*!
 * \brief delete the BCM-TRM component.
 *
 * This function should be called once during the system shutdown. It
 * should be called past unit dettach function for all units.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_delete(void);

/*!
 * \brief initialize a unit of BCM-TRM.
 *
 * This function should be called during the unit attached. It will allocate the
 * resources required by the instance. This function will be called by the
 * system manager in its init phase.
 *
 * \param [in] unit Is the unit number that was added to the system.
 * \param [in] warm Indicate if warm or cold start.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_unit_init(int unit, bool warm);

/*!
 * \brief Delete a unit of BCM-TRM.
 *
 * This function should be called by the system manager during the unit shutdown
 * procedure. This function will free all resources that were allocated by this
 * component.
 *
 * \param [in] unit Is the unit number that was added to the system.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_unit_delete(int unit);

/*!
 * \brief Retrieves the active entries and transactions lists.
 *
 * This function retrieves the entries and transactions active lists. These
 * list contains all the elements (entries or transactions) that were allocated
 * by the application.
 *
 * \param [out] ent_list The list of active entries.
 * \param [out] trans_list The list of active transactions.
 *
 * \return None.
 */
extern void bcmtrm_active_element_lists_get(bcmtrm_entry_t **ent_list,
                                            bcmtrm_trans_t **trans_list);

/*!
 * \brief Request for synchronous transaction.
 *
 * This function process a transaction in a synchronous way. The function posts
 * the transaction request onto a message queue to be processed by a dedicated
 * thread associated with the unit. Once that was done the function waits on a
 * synchronization object for the operation to be signaled as completed.
 *
 * \param [in] trans Is a pointer to the transaction to process.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_trans_req (bcmtrm_trans_t *trans);

/*!
 * \brief Request for asynchronous transaction.
 *
 * This function process a transaction in an asynchronous way. The function
 * posts the transaction request onto a message queue to be processed by a
 * dedicated thread associated with the unit. Once that was done the function
 * returns. Note that if the message queue of the thread is full this function
 * will be blocked until it was able to post the request.
 *
 * \param [in] trans Is a pointer to the transaction to process.
 *
 * \return SHR_E_NONE on success and error code otherwise
 */
extern int bcmtrm_asynch_trans_req (bcmtrm_trans_t *trans);

/*!
 * \brief Allocate a transaction.
 *
 * This function allocate a transaction data structure The only input for
 * this function is the transaction type (atomic or batch).
 *
 * \param [in] type Is the transaction type.
 *
 * \return pointer to a transaction structure on success and NULL otherwise.
 */
extern bcmtrm_trans_t *bcmtrm_trans_alloc(bcmlt_trans_type_t type);


/*!
 * \brief Request for transaction free.
 *
 * This function process transaction free request. The function handles both
 * synchronous and asynchronous transactions. For synchronous transaction it
 * verifies that the transaction state is T_ACTIVE before releasing the
 * resources associated with the transaction.
 * For asynchronous transactions, the function checks the state of the
 * transaction. If it is T_ACTIVE it will free its resources, otherwise,
 * the function mark the transaction for delete. The transaction will be
 * free by the notification thread once the transaction completed its
 * processing and notifications.
 *
 * \param [in] trans Is a pointer to the transaction to process.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_trans_free(bcmtrm_trans_t *trans);

/*!
 * \brief Allocate an entry.
 *
 * This function allocates and initializes a transaction entry.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_id Table ID.
 * \param [in] interact Indicate if the table is interactive.
 * \param [in] pt Indicates if this is a PT table.
 * \param [in] fld_array_hdl The memory handle to allocate field array.
 * \param [in] tbl_name The table name.
 *
 * \return pointer to an entry on success and NULL otherwise.
 */
extern bcmtrm_entry_t *bcmtrm_entry_alloc(int unit,
                                          uint32_t tbl_id,
                                          bool interact,
                                          bool pt,
                                          shr_lmm_hdl_t fld_array_hdl,
                                          const char *tbl_name);

/*!
 * \brief Free previously allocated entry.
 *
 * This function process entry free request. Synchronous operation can be
 * processed immediately if the operation had complete (state = E_ACTIVE).
 * Synchronous operations can not be freed while the entry operation is
 * on-going.For asynchronous operations, the function operates according to
 * the entry state. If the entry state is E_ACTIVE it means that the
 * entry processing is complete and therefore the entry resources can be
 * freed. Otherwise, it marks the entry for deletion. The entry resources
 * will be freed once it completed its processing.
 *
 * \param [in] entry Entry to be freed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_entry_free(bcmtrm_entry_t *entry);

/*!
 * \brief Request to process entry synchronously.
 *
 * This function process entry request. The function
 * posts the entry request onto a message queue to be processed by a
 * dedicated thread associated with the unit. Once that was done the function
 * blocks until the operation completed and the entry callback (TRM internal
 * function) was called.
 *
 * \param [in] entry Entry to be processed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_entry_req (bcmtrm_entry_t *entry);

/*!
 * \brief Request to process entry asynchronously.
 *
 * This function process entry request. The function
 * posts the entry request onto a message queue to be processed by a
 * dedicated thread associated with the unit. Once that was done the function
 * returns. Note that if the message queue of the thread is full this function
 * will be blocked until it was able to post the request.
 *
 * \param [in] entry Entry to be processed asynchronously.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_asynch_entry_req (bcmtrm_entry_t *entry);

/*!
 * \brief Initializes unit for interactive tables.
 *
 * This function being used to inform the TRM about the maximal number of
 * fields that can be in any interactive table of this unit.
 *
 * \param [in] unit Is the unit number of which this function referes too.
 * \param [in] warm Indicate if it is warm or cold boot.
 * \param [in] max_fields Indicates the maximal number of fields that can be
 * part of any interactive table for this unit.
 *
 * \return SHR_E_NONE on success. SHR_E_MEMORY when failed to allocate memory
 * or SHR_E_PARAM when input parameters are invalid.
 */
extern int bcmtrm_interactive_table_init(int unit,
                                         bool warm,
                                         uint32_t max_fields);

/*
 * Table callback functions
 */
/*!
 * \brief Subscribe/Unsubscribe application to table change event.
 *
 * This function associated application callback function with specific table
 * change event. Whenever, the desired table changes, the TRM will call the
 * registered function. The same function can be used to unsubscribing by
 * passing NULL as the application function.
 *
 * \param [in] unit Indicates the interested unit.
 * \param [in] table_id Indicates the table of interest. This ID related to the
 * unit as well.
 * \param [in] event_func The function to call when table changes. To
 * unsubscribe set event_func to NULL.
 * \param [in] user_data Context provided by the application that can assist the
 * application event function.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmtrm_table_event_subscribe(int unit,
                                        uint32_t table_id,
                                        bcmlt_table_sub_cb event_func,
                                        void *user_data);

/*!
 * \brief Callback to allocate entry.
 *
 * This function being called by the TRM notification thread to create
 * BCMLT entry which can later be handed to the application.
 *
 * \param [in] unit The unit number associated with the entry.
 * \param [in] tbl_id The LRD table ID associated with the entry.
 * \param [in] opcode The opcode associated with the entry.
 * \param [in] fields Input fields to associate with the entry.
 * \param [out] entry Newly allocated entry.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The table ID is invalid.
 * \retval SHR_E_INIT The BCMLT layer had not been initialized.
 */
typedef int (*bcmtrm_entry_alloc_assist_f)(int unit,
                                           uint32_t tbl_id,
                                           bcmlt_opcode_t opcode,
                                           shr_fmm_t *fields,
                                           bcmtrm_entry_t **entry);

/*!
 * \brief Callback to free an entry.
 *
 * This function type being called to free an entry.
 *
 * \param [in] entry The entry to free.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The entry is invalid.
 */
typedef int (*bcmtrm_entry_free_assist_f)(bcmtrm_entry_t *entry);

/*!
 * \brief Callback function to record an entry operation.
 *
 * This is a callback function that is register with the TRM module to record
 * an entry operation. The operation being recorded from the TRM to preserve
 * the original order of which the entries will be committed.
 *
 * \param [in] entry The entry to record.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Write operation failed.
 */
typedef int (*bcmlt_replay_entry_record_f)(bcmtrm_entry_t *entry);

/*!
 * \brief Callback function to record a transaction operation.
 *
 * This is a callback function that is register with the TRM module to record
 * a transaction operation. The operation being recorded from the TRM to
 * preserve the original order of which the transactions will be committed.
 *
 * \param [in] trans The transaction to record.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Write operation failed.
 */
typedef int (*bcmlt_replay_trans_record_f)(bcmtrm_trans_t *trans);

/*!
 * \brief Register entry assist function with the TRM.
 *
 * This function registers a callback functions that will enable the
 * notification thread to allocate and free BCMLT entry before informing the
 * application about table change event.
 *
 * \param [in] alloc_cb The callback function to allocate an entry.
 * \param [in] free_cb The callback function to free an entry.
 * \param [in] entry_rec The callback to record an entry operation.
 * \param [in] trans_rec The callback to record transaction operation.
 *
 * \return None.
 */
extern void bcmtrm_register_entry_assist(bcmtrm_entry_alloc_assist_f alloc_cb,
                                         bcmtrm_entry_free_assist_f free_cb,
                                         bcmlt_replay_entry_record_f entry_rec,
                                         bcmlt_replay_trans_record_f trans_rec);


/*!
 * \brief Provide statistics for the transaction handles.
 *
 * \param [out] stats Contains the statistics associated with the transaction
 * handlers.
 *
 * \return None.
 */
extern void bcmtrm_get_trans_hdl_stat(shr_lmm_stat_t *stats);

/*!
 * \brief Provide statistics for the entry handles.
 *
 * \param [out] stats Contains the statistics associated with the entry
 * handlers.
 *
 * \return None.
 */
extern void bcmtrm_get_entry_hdl_stat(shr_lmm_stat_t *stats);

/*
 * Debug functions
 */
/*!
 * \brief dump all active elements (transactions + entries) into log.
 *
 * This function dumps into log the content of all active transactions and
 * entries.
 *
 * \return none
 */
extern void bcmtrm_active_elements_dump(void);

#endif /* TRM_API_H */
