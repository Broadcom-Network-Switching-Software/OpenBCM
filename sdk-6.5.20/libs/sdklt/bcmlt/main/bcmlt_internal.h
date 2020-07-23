/*! \file bcmlt_internal.h
 *
 *  This file contains data structure definition and definitions of functions
 *  that are internal to the BCMLT client API
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLT_INTERNAL_H
#define BCMLT_INTERNAL_H

#include <sal/sal_mutex.h>
#include <shr/shr_types.h>
#include <shr/shr_lmem_mgr.h>
#include <bsl/bsl.h>
#include <bcmtrm/trm_api.h>

/* Macros used for common parameter checking */
#define ENTRY_VALIDATE(entry) \
    if (!entry || (entry->state != E_ACTIVE)) {                             \
        if (!entry) {                                                       \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Invalid entry handle\n")));              \
        } else {                                                            \
            LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                        (BSL_META("Entry was committed\n")));               \
        }                                                                   \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

#define ENTRY_GET_VALIDATE(entry) \
    if (!entry) {                                                       \
        LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                    (BSL_META("Invalid entry handle\n")));              \
        SHR_ERR_EXIT(SHR_E_PARAM);                               \
    }

#define VALIDATE_FID(entry, fid) \
    if (entry->max_fid < fid) {                                         \
        LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                    (BSL_META("Invalid field ID\n")));                  \
        SHR_ERR_EXIT(SHR_E_PARAM);                               \
    }

#define UNIT_VALIDATION(unit)   \
    if (unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS) {                      \
        SHR_ERR_EXIT(SHR_E_UNIT);                                    \
    }                                                                       \
    if (!bcmlt_device_attached(unit)) {                                     \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit, "Unit not attached\n")));             \
        SHR_ERR_EXIT(SHR_E_UNIT);                                    \
    }

#define LT_OPCODE_VALIDATE(opcode, unit) \
    if ((opcode <= BCMLT_OPCODE_NOP) || (opcode >= BCMLT_OPCODE_NUM)) {     \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid opcode %d\n"), opcode));           \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

#define PT_OPCODE_VALIDATE(opcode, unit) \
    if ((opcode <= BCMLT_PT_OPCODE_NOP) || (opcode >= BCMLT_PT_OPCODE_NUM)) {\
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid opcode %d\n"), opcode));           \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

#define PRIORITY_VALIDATE(priority, unit) \
    if ((priority < BCMLT_PRIORITY_NORMAL) ||                               \
        (priority > BCMLT_PRIORITY_HIGH)) {                                 \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid priority %d\n"), priority));       \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

#define NOTIFICATION_VALIDATE(notif_opt, notif_fn, unit) \
    if ((notif_opt >= BCMLT_NOTIF_OPTION_NUM) ||                            \
        (notif_opt < BCMLT_NOTIF_OPTION_NO_NOTIF) ||                        \
        ((notif_opt > BCMLT_NOTIF_OPTION_NO_NOTIF) && !notif_fn)) {         \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid notification option %d\n"),        \
                     notif_opt));                                           \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

#define PT_NOTIFICATION_VALIDATE(notif_opt, notif_fn, unit) \
    if ((notif_opt >= BCMLT_NOTIF_OPTION_NUM) ||                            \
        (notif_opt < BCMLT_NOTIF_OPTION_NO_NOTIF) ||                        \
        (notif_opt == BCMLT_NOTIF_OPTION_COMMIT) ||                        \
        ((notif_opt > BCMLT_NOTIF_OPTION_NO_NOTIF) && !notif_fn)) {         \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(unit,                                       \
                                "Invalid notification option %d\n"),        \
                     notif_opt));                                           \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }


typedef enum bcmlt_table_acc_e {
    BCMLT_TABLE_ACCESS_READ_WRITE = 0,  /*!< Allow read and write access */
    BCMLT_TABLE_ACCESS_READ_ONLY        /*!< Read only access mode       */
} bcmlt_table_acc_t;

/*!
 * \brief Table attribute structure.
 *
 * This structure contains attributes required for the control path.
 */
typedef struct {
    uint32_t          table_id;     /*!< Table ID                      */
    const char        *name;        /*!< Table name                    */
    bool              interactive;  /*!< interactive table             */
    bcmlt_table_acc_t access;       /*!< Access attribute of the table */
    bool              pt;           /*!< Table is pass through?        */
    uint32_t          num_of_fields;/*!< Table number of fields        */
    uint32_t          max_fid;      /*!< Maximum field ID */
} bcmlt_table_attrib_t;

/*!
 * \brief Module init function.
 *
 * This function must be called during the s/w initialization. It should
 * be called only once. Before calling this function none of the
 * bcmlt_db_xxx functions can be called.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_init(void);

/*!
 * \brief Module delete function.
 *
 * This function must be called during the s/w shutdown. It should
 * be called only once. No other bcmlt_db_xxx function should be
 * called after this function.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_delete(void);

/*!
 * \brief Device init function.
 *
 * This function initializes the bcmlt_db for this unit. Only after calling
 * this function the application may call other function in this library
 * associated with this unit.
 *
 * \param [in] unit Is the device unit.
 *
 * \return SHR_E_NONE on success and error code otherwise. Reason for failure
 * can be:
 * a. Insufficient memory resources.
 * b. DRD was not initialized for this unit.
 * c. LRD was not initialized for this unit.
 */
extern int bcmlt_tables_db_init (int unit);

/*!
 * \brief Device delete function.
 *
 * This function deletes the _bcmlt_db for this unit. It deletes
 * all the tables associated with this unit and free all the
 * table resources. After calling
 * this function the application should not call other function in
 * this library associated with this unit (result in failure).
 *
 * \param [in] unit Is the device unit.
 *
 * \return 0 for success, otherwise failure.
 */
extern int bcmlt_tables_db_delete (int unit);

/*!
 * \brief Clean out all data fields
 *
 * This function filters out all the data fields and leaving the entry
 * only with key fields. This function is typically being used for lookup
 * or traverse operations where residue from previous operations is unwanted.
 *
 * \param [in] entry Is the entry to process its fields.
 *
 * \return SHR_E_NONE on success and SHR_E_UNAVAIL otherwise.
 */
extern int bcmlt_entry_clean_data_fields(bcmtrm_entry_t *entry);

/*!
 * \brief Fill out the field IDs of all key fields
 *
 * This function go through the table fields and copy the field IDs of all
 * key fields into the array \c keys. It also updated the parameter
 * \c num_of_keys with the number of key fields found.
 *
 * \param [in] unit Is the device unit.
 * \param [in] hdl Is the table database handle, typically obtained via the
 * function \ref bcmlt_db_table_info_get().
 * \param [in] array_size Is the number of element that can be contained in
 * the array \c keys.
 * \param [out] keys Is an array where the key fields ID will be placed.
 * \param [out] num_of_keys This parameter will contain the actual number of
 * key fields that were copied into the array \c keys.
 *
 * \return None.
 */
extern void bcmlt_db_table_keys_get(int unit,
                                    void *hdl,
                                    uint32_t array_size,
                                    uint32_t *keys,
                                    uint32_t *num_of_keys);
/*!
 * \brief Obtain specific table attributes based on table name.
 *
 * This function provides the attributes of a specific table on specific unit
 *
 * \param [in] unit Is the device unit.
 * \param [in] table_name Is the table name.
 * \param [out] attrib Is a double pointer pointing to returned attributes.
 * \param [out] fld_array_hdl Is the memory handle to use for field array
 * allocation.
 * \param [out] hdl Is a double pointer pointing to returned context. This
 *              context should be use in consecutive call to
 *              bcmlt_db_table_info_get().
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_table_info_get(int unit,
                                   const char *table_name,
                                   bcmlt_table_attrib_t **attrib,
                                   shr_lmm_hdl_t *fld_array_hdl,
                                   void **hdl);

/*!
 * \brief Obtain specific table attributes based on table ID.
 *
 * This function provides the attributes of a specific table on specific unit
 *
 * \param [in] unit Is the device unit.
 * \param [in] tid Is the table ID.
 * \param [out] attrib Is a double pointer pointing to returned attributes.
 * \param [out] fld_array_hdl Is the memory handle to use for field array
 * allocation.
 * \param [out] hdl Is a double pointer pointing to returned context. This
 *              context should be use in consecutive call to
 *              bcmlt_db_table_info_get().
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_id_table_info_get(int unit,
                                      uint32_t tid,
                                      bcmlt_table_attrib_t **attrib,
                                      shr_lmm_hdl_t *fld_array_hdl,
                                      void **hdl);

/*!
 * \brief Obtain specific table attributes.
 *
 * This function provides the attributes of a specific table on specific unit.
 *
 * \param [in] unit Is the device unit.
 * \param [in] name Is the field name.
 * \param [in] hdl Is the table database handle, typically obtained via the
 * function \ref bcmlt_db_table_info_get().
 * \param [out] f_attr Is a double pointer pointing to returned field
 *              attributes.
 * \param [out] field_id Is a pointer pointing to returned field ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_field_info_get(int unit,
                                   const char *name,
                                   void *hdl,
                                   bcmlt_field_def_t **f_attr,
                                   uint32_t *field_id);

/*!
 * \brief Start table enumeration
 *
 * This API starts the retrieval of all available logical table names
 * on a particular device. The function returns the name of one table.
 * Use the table search next to retrieve the rest of the tables.
 *
 * \param [in] unit Is the device number for the table of interest.
 * \param [in] flags Can be used to filter the output.
 * \param [out] name Pointer to the first table name.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_table_name_get_first(int unit,
                                         uint32_t flags,
                                         char **name);

/*!
 * \brief Continue table enumeration
 *
 * This API continue the retrieval of all available logical table names
 * on a particular device. The function returns the name of one table.
 * Use this function again to retrieve more names.
 *
 * \param [in] unit Is the device number for the table of interest.
 * \param [in] flags Can be used to filter the output.
 * \param [out] name Pointer to the table name that was found next.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmlt_db_table_name_get_next(int unit,
                                        uint32_t flags,
                                        char **name);

/*!
 * \brief Obtain table fields definition.
 *
 * This function obtains the attributes of the fields in a given table.
 * The attributes will be copied into caller provided buffer. The
 * caller must also provide the size of the output buffer. If the
 * caller doesn't know apriori the required size it can call with NULL
 * pointer for the output buffer and 0 for the fields_array_size. In
 * any case the require_array_size will contain the desired space
 * (number of elements) to obtain the fields information
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] table_name Is the table name
 * \param [in] fields_array_size Is the number of fields the output
 * array can hold.
 * \param [out] field_defs_array Pointer to caller provided memory
 * where the fields attributes should be copied to.
 * \param [out] require_array_size Is a pointer where the function
 * places the number of fields in the table.
 *
 * \return SHR_E_NONE on success, otherwise failure finding the table
 */
extern int bcmlt_db_table_field_defs_get(int unit,
                                         const char *table_name,
                                         uint32_t fields_array_size,
                                         bcmlt_field_def_t *field_defs_array,
                                         uint32_t *require_array_size);

/*!
 * \brief Obtain the maximal number of fields in an interactive table.
 *
 * This function obtains the maximal number of fields that can be in any
 * interactive table of this unit.
 *
 * \param [in] unit Is the device number the query refers to.
 *
 * \return The maximal number of fields at any interactive table.
 */
extern uint32_t bcmlt_db_interact_tbl_max_fields(int unit);

/*!
 * \brief Obtain the maximal number of fields in all LTs.
 *
 * This function obtains the maximal number of fields that can be in any
 * logical table of this unit.
 *
 * \param [in] unit Is the device number the query refers to.
 *
 * \return The maximal number of fields at any logical table.
 */
extern uint32_t bcmlt_db_lt_max_fields(int unit);

/*!
 * \brief Indicates if bcmlt component had been initialized.
 *
 * This function returns true if the bcmlt component was initialized and false
 * otherwise.
 *
 * \return true if bcmlt library was succesfully initialized and
 * false otherwise.
 */
extern bool bcmlt_is_initialized(void);

/*!
 * \brief Indicates if atomic transactions are enabled
 *
 * \return true if atomic transaction is enabled and false otherwise.
 * false otherwise.
 */
extern bool bcmlt_is_atomic_ena(void);

/*!
 * \brief Find field in a given entry.
 *
 * This function searches for a particular field, based on \c field_id in
 * a given entry. If found and if \c prev is not NULL, it also provide the
 * field that is predecessor to the desired field.
 *
 * \param [in] entry Is the entry to search for the field.
 * \param [in] field_id Is the search criteria.
 * \param [out] prev Can be NULL. If not NULL and the field was found, then
 * \c prev will contain the field that is previous to the desired field in
 * the linked list of fields.
 *
 * \return pointer to the desired field if the field was found and NULL
 * otherwise.
 */
extern shr_fmm_t *bcmlt_find_field_in_entry(bcmtrm_entry_t *entry,
                                            uint32_t field_id,
                                            shr_fmm_t **prev);

/*
 * Replay functions
*/

/*!
 * \brief Record entry operation.
 *
 * This function records the entry operation so it can be played back at a
 * later time. The function \c bcmlt_replay_record_start() must be called
 * prior to calling this function for this function to succeed.
 *
 * \param [in] entry Is the entry to record. The entry structure
 * contains all the information and action required to replay this instruction.
 *
 * \return SHR_E_NONE if the entry was recorded and error code otherwise.
 */
extern int bcmlt_replay_entry_record(bcmtrm_entry_t *entry);

/*!
 * \brief Record transaction operation.
 *
 * This function records the transaction operation so it can be played back
 * at a later time. The function \c bcmlt_replay_record_start() must be called
 * prior to calling this function for this function to succeed.
 *
 * \param [in] trans Is the transaction to record. The transaction
 * structure contains all the information and action required to replay this
 * transaction action.
 *
 * \return SHR_E_NONE if the transaction was recorded and error code otherwise.
 */
extern int bcmlt_replay_trans_record(bcmtrm_trans_t *trans);

/*!
 * \brief Initialize the handle structure.
 *
 * This function initializes the handle structure.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmlt_hdl_init(void);

/*!
 * \brief Shutdown the handle structure.
 *
 * This function shutdown the handle structure and free
 * all the associated resources.
 *
 * \return None.
 */
extern void bcmlt_hdl_shutdown(void);

/*!
 * \brief Allocates handle from the free handle poll.
 *
 * This function allocates a new handle and initializes
 * the handle content with a given pointer to the data. The caller can use
 * the returned handle to obtain the pointer to the data using the function
 * \ref bcmlt_hdl_data_get()
 *
 * \param [in] data Is a pointer for the caller that should be associated
 * with the new allocated handle.
 * \param [out] hdl Can be used later to obtain the pointer of \c data.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmlt_hdl_alloc(void *data, uint32_t *hdl);

/*!
 * \brief Free previously allocated handle.
 *
 * This function frees a previously allocated handle and returned the handle
 * to the free list to be reused.
 *
 * \param [in] hdl Is the handle to free.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmlt_hdl_free(uint32_t hdl);

/*!
 * \brief Obtain the data associated with a handle.
 *
 * This function initializes the handle structure for the given unit.
 *
 * \param [in] hdl Is the handle that was previously allocated.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern void *bcmlt_hdl_data_get(uint32_t hdl);

/*!
 * \brief BCMLT module Init.
 *
 * This is the first function to invoke from client application.
 *
 * A library init is only allowed once per lifecycle of the application process.
 * Subsequent inits following a successful library init call are rejected.
 *
 * Client library will allocate and initialize the data structure and resources
 * required by the library.
 *
 * \return SHR_E_NONE success, otherwise failure in allocating resources
 */
extern int bcmlt_init(void);

/*!
 * \brief BCMLT module Close
 *
 * This is the last function to invoke from client application.
 *
 * A library close will clean up all the resources used by the library
 * including recycle all the active transactions
 *
 * \return SHR_E_NONE success, otherwise failure in freeing resources
 */
extern int bcmlt_close(void);

/*!
 * \brief Check that no resources are allocated to this unit.
 *
 * This function checks that all the entry and transaction resources associated
 * with a given unit had been released.
 *
 * \param [in] unit The unit to use for resource check.
 *
 * \return SHR_E_NONE No resources are allocated.
 * \return SHR_E_BUSY Some resources are tied up.
 */
extern int bcmlt_allocated_resources(int unit);

/*!
 * \brief Debug log dump of an entry content.
 *
 * This function being called when BCMLT Detail log level is enabled. It writes
 * the entry content into the BSL log.
 *
 * \param [in] entry Is the entry to log its content.
 * \param [in] pb Print buffer handle. This should be NULL when dumping the
 * content of a single entry.
 *
 * \return None.
 */
extern void bcmlt_dump_entry(bcmtrm_entry_t *entry, void *pb);

/*!
 * \brief Debug log dump of a transaction content.
 *
 * This function being called when BCMLT Detail log level is enabled. It writes
 * the transaction content including its entries into the BSL log.
 *
 * \param [in] trans Is the transaction to log its content.
 *
 * \return None.
 */
extern void bcmlt_dump_trans(bcmtrm_trans_t *trans);

/*!
 * \brief Entry module initialization.
 *
 * This function initializes the entry module of the BCMLT. The
 * purpose of this function is to initialize the high performance API.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failure due to resources.
 */
extern int bcmlt_entry_module_init(void);

/*!
 * \brief Shutdown the entry module.
 *
 * This function shuts down the entry module by releasing resources that were
 * used for the high performance API.
 *
 * \return SHR_E_NONE Success.
 */
extern int bcmlt_entry_module_shutdown(void);

/*!
 * \brief Obtain the entry synchronous resource.
 *
 * This function obtain the synchronization resource for all the entries and
 * handles.
 *
 * \return Synchronous object on success.
 * \return NULL on failure.
 */
extern sal_mutex_t bcmlt_entry_sync_obj_get(void);
#endif /* BCMLT_INTERNAL_H */
