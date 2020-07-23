/*! \file bcma_bcmlt.h
 *
 * Functions related to bcmlt module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLT_H
#define BCMA_BCMLT_H

#include <shr/shr_error.h>
#include <shr/shr_pb.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmlt/bcmlt.h>

/*! Format to display shr_errmsg with error code. */
#define BCMA_BCMLT_ERRMSG_FMT             "%s (error code %d)"

/*!
 * Macro used in print data function combined with the format
 * \ref BCMA_BCMLT_ERRMSG_FMT to display shr_errmsg with error code.
 */
#define BCMA_BCMLT_ERRMSG(_err)           shr_errmsg(_err), _err

/*! Match mode option in \ref bcma_bcmlt_tables_search function. */
typedef enum bcma_bcmlt_match_mode_e {
    BCMA_BCMLT_MATCH_EXACT = 1,
    BCMA_BCMLT_MATCH_SUBSTR = 2
} bcma_bcmlt_match_mode_t;

/*! Flag to identify table search scope including logical table. */
#define BCMA_BCMLT_SCOPE_F_LTBL           (1 << 0)

/*! Flag to identify table search scope including physical table. */
#define BCMA_BCMLT_SCOPE_F_PTBL           (1 << 1)

/*!
 * \name Entry operation options
 * \anchor BCMA_BCMLT_TBL_LIST_OPT_xxx
 */

/*! \{ */

/*! Flag to display only field names of logical tables. */
#define  BCMA_BCMLT_TBL_LIST_OPT_FIELD_NAME_ONLY     (1 << 0)

/*! Flag to display logical info of fields in logical tables. */
#define  BCMA_BCMLT_TBL_LIST_OPT_FIELD_INFO          (1 << 1)

/*! Flag to display physical info of fields in logical tables. */
#define  BCMA_BCMLT_TBL_LIST_OPT_FIELD_PT_INFO       (1 << 2)

/*! Flag to display description of logical tables and fields. */
#define  BCMA_BCMLT_TBL_LIST_OPT_DESC                (1 << 3)

/*! \} */

/*!
 * \name Entry operation options
 * \anchor BCMA_BCMLT_ENTRY_OP_OPT_xxx
 */

/*! \{ */

/*! Perform INSERT operation after an entry not found UPDATE. */
#define BCMA_BCMLT_ENTRY_OP_OPT_INS_UPDT  (1 << 0)

/*! Automatically assign unspecified key fields with default values. */
#define BCMA_BCMLT_ENTRY_OP_OPT_AUTO_KFS  (1 << 1)

/*! Perform LOOKUP operation for each traversed entry. */
#define BCMA_BCMLT_ENTRY_OP_OPT_TRVS_LKUP (1 << 2)

/*! Perform UPDATE operation for each traversed entry. */
#define BCMA_BCMLT_ENTRY_OP_OPT_TRVS_UPD  (1 << 3)

/*! Perform DELETE operation for each traversed entry. */
#define BCMA_BCMLT_ENTRY_OP_OPT_TRVS_DEL  (1 << 4)

/*! Force unspecified fields to reset to default value in UPDATE operation. */
#define BCMA_BCMLT_ENTRY_OP_OPT_FLD_RESET (1 << 5)

/*! Only display fields with non-default value. */
#define BCMA_BCMLT_ENTRY_OP_OPT_FLD_DIFF  (1 << 6)

/*! Display all fields of the entry with non-default value. */
#define BCMA_BCMLT_ENTRY_OP_OPT_ENT_DIFF  (1 << 7)

/*! \} */


/*!
 * \name Field flags
 * \anchor BCMA_BCMLT_FIELD_F_xxx
 */

/*! \{ */

/*! Field is a key of table. */
#define BCMA_BCMLT_FIELD_F_KEY            (1 << 0)

/*! Field value can be represented by a symbol string. */
#define BCMA_BCMLT_FIELD_F_SYMBOL         (1 << 1)

/*! Field contents are represented by an array. */
#define BCMA_BCMLT_FIELD_F_ARRAY          (1 << 2)

/*! Field width is greater than 64-bit length. */
#define BCMA_BCMLT_FIELD_F_WIDE           (1 << 3)

/*! \} */

/*!
 * Macro to indicate that union u.array is to be used in
 * \ref bcma_bcmlt_field_t.
 */
#define BCMA_BCMLT_FIELD_IS_ARRAY(_f) \
    (((_f)->flags & (BCMA_BCMLT_FIELD_F_ARRAY | BCMA_BCMLT_FIELD_F_WIDE)) != 0)


/*!
 * \name Qualifiers for field value comparison
 * \anchor BCMA_BCMLT_FIELD_QLFR_xxx
 */

/*! \{ */

/*! Equal to */
#define BCMA_BCMLT_FIELD_QLFR_EQ          (1 << 0)

/*! Not equal to */
#define BCMA_BCMLT_FIELD_QLFR_NE          (1 << 1)

/*! Greater than or equal to */
#define BCMA_BCMLT_FIELD_QLFR_GE          (1 << 2)

/*! Less than or equal to */
#define BCMA_BCMLT_FIELD_QLFR_LE          (1 << 3)

/*! Greater than */
#define BCMA_BCMLT_FIELD_QLFR_GT          (1 << 4)

/*! Less than */
#define BCMA_BCMLT_FIELD_QLFR_LT          (1 << 5)

/*! \} */


/*! Table entry operation result display mode for fields. */
typedef enum bcma_bcmlt_field_display_mode_e {
    BCMA_BCMLT_DISP_MODE_HEX_DEC = 0,
    BCMA_BCMLT_DISP_MODE_HEX = 1,
    BCMA_BCMLT_DISP_MODE_DEC = 2
} bcma_bcmlt_field_display_mode_t;

/*! Table entry operation result display mode. */
typedef struct bcma_bcmlt_display_mode_s {

    /*!
     * Display mode for scalar fields (\ref bcma_bcmlt_field_display_mode_t).
     */
    int scalar: 8;

    /*!
     * Display mode for array fields (\ref bcma_bcmlt_field_display_mode_t).
     */
    int array: 8;

    /*!
     * Delimiter between each group of array fields.
     * Typical value is ',' , '\n' or ' '.
     */
    int array_delim: 8;

    /*!
     * Delimiter between fields.
     * Typical value is ',' , '\n' or ' '.
     */
    int field_delim: 8;

} bcma_bcmlt_display_mode_t;

/*!
 * \brief LT Field information structure used for array field.
 *
 * This structure defines the information needed for array field.
 */
typedef struct bcma_bcmlt_field_array_s {

    /*! Union of array field types. */
    union {

        /*!
         * Field values array which is dynamically allocated.
         * Might be used for field which is a scalar array or
         * wide-field (field width > 64-bit).
         */
        uint64_t *vals;

        /*! Field symbol strings array which is dynamically allocated. */
        const char **strs;

    } u;

    /*!
     * Dynamically allocated bool array to indicate if an element
     * in scalar array or symbol strings array is valid or not.
     */
    bool *elements;

    /*! Field array size allocated for \c vals or \c strs. */
    int size;

} bcma_bcmlt_field_array_t;

/*!
 * \brief LT Field information structure.
 *
 * This structure defines a single field within a table.
 */
typedef struct bcma_bcmlt_field_s {

    /*! Field name. */
    const char *name;

    /*! Field width. */
    uint32_t width;

    /*! Default value. */
    bcmlt_field_def_t dflt;

    /*! Union of entry field types. */
    union {

        /*! Field value used for non-array field. */
        uint64_t val;

        /*! Field symbol string used for non-array field. */
        const char *str;

        /*!
         * Dynamically allocated structure used for field
         * which data is represented in an array.
         */
        bcma_bcmlt_field_array_t *array;

    } u;

    /*! Field attribute (\ref BCMA_BCMLT_FIELD_F_xxx). */
    uint32_t flags;

    /*!
     * Qualifier information. Not NULL if the field stands for a qualifier
     * for data comparison.
     */
    const bcma_cli_parse_enum_t *qlfr;

} bcma_bcmlt_field_t;

/*! Parameters used in entry or transaction commit. */
typedef struct bcma_bcmlt_commit_params_s {

    /*! True if commit in asynchronous mode. */
    bool async;

    /*! Update notification that is used in asynchronous mode. */
    bcmlt_notif_option_t notify;

    /*! Priority levels for entry and transaction operations. */
    bcmlt_priority_level_t priority;

} bcma_bcmlt_commit_params_t;

/*!
 * \brief Entry information.
 *
 * This structure is used to store entry information while
 * performing logical or physical table operation.
 *
 * This structure should be initialized and cleaned up by functions
 * \ref bcma_bcmlt_entry_info_create and \ref bcma_bcmlt_entry_info_destroy
 * respectively.
 */
typedef struct bcma_bcmlt_entry_info_s {

    /*! Unit number the entry performs on. */
    int unit;

    /*! Logical or physical table name the entry operates on. */
    const char *name;

    /*! Entry handle used in bcmlt functions. */
    bcmlt_entry_handle_t eh;

    /*! Default fields information array. */
    bcmlt_field_def_t *def_fields;

    /*! Default number of fields information array. */
    int num_def_fields;

    /*! Number of key fields for this table. */
    int num_key_fields;

    /*! Fields information array. */
    bcma_bcmlt_field_t *fields;

    /*! Fields information array size. */
    int szfields;

    /*! Actual number of valid elements used in the fields information array. */
    int num_fields;

    /*!
     * Number of fields added to the entry before commit for LT.
     * Queried from bcmlt_entry_field_count() in <bcmlt/bcmlt.h>.
     * Default value is -1 if not used.
     */
    int num_commit_fields;

    /*!
     * Qualifier fields information array is supported in LT traverse
     * operations for field value comparison to filter the qualified entries
     * from the traversing result. The field qualifier information is set
     * through \c qlfr in \ref bcma_bcmlt_field_t.
     */
    bcma_bcmlt_field_t *qlfr_fields;

    /*!
     * The allocated qualifier fields information array size which supported
     * the utmost number of field qualifiers.
     */
    int szqlfr_fields;

    /*!
     * Actual number of valid elements used in the qualifier fields
     * information array.
     */
    int num_qlfr_fields;

    /*!
     * Tokens for matching entry fields to set as environment variables
     * after an LT LOOKUP or INSERT (index with allocate) operation.
     */
    bcma_cli_tokens_t *ev_ctoks;

    /*!
     * Array index range information via tokens in \c ev_ctoks.
     */
    int *ev_arrayinfo;

    /*! Cookie related to entry field environment variable functions. */
    void *ev_cookie;

    /*!
     * Attributes of the entry table.
     * Refer to BCMLT_ENT_ATTR_xxx in <bcmlt/bcmlt.h>.
     */
    uint32_t attr;

    /*! Options of the entry operation. */
    uint32_t opts;

    /*! Identify the entry is logical or physical table. */
    bool pt;

     /*! LT entry operation code (valid if field \c pt is false). */
    bcmlt_opcode_t opcode;

     /*! PT entry operation code (valid if field \c pt is true). */
    bcmlt_pt_opcode_t pt_opcode;

    /*! Parameters for entry commit. */
    bcma_bcmlt_commit_params_t commit_params;

    /*!
     * Pointer to the next entry information.
     * Mainly used in transaction commit.
     */
    struct bcma_bcmlt_entry_info_s *next;

} bcma_bcmlt_entry_info_t;

/*!
 * \brief Transaction information.
 *
 * This structure is used to store the transaction information while
 * performing logical or physical table operation in transaction mode.
 */
typedef struct bcma_bcmlt_transaction_info_s {

    /*! Unit number the transaction performs on. */
    int unit;

    /*! Transaction handle used in bcmlt functions. */
    bcmlt_transaction_hdl_t th;

    /*! Transaction type. */
    bcmlt_trans_type_t type;

    /*! Parameters for transaction commit. */
    bcma_bcmlt_commit_params_t commit_params;

    /*! The first entry of the entries list in the transaction. */
    bcma_bcmlt_entry_info_t *ei_head;

    /*! The last entry of the entries list in the transaction. */
    bcma_bcmlt_entry_info_t *ei_tail;

} bcma_bcmlt_transaction_info_t;

/*!
 * \brief Callback function in \ref bcma_bcmlt_tables_search.
 *
 * The callback function passes table name and default fields information,
 * including fields name, data type, access type, etc for advanced usage.
 *
 * \param [in] cookie User data through \ref bcma_bcmlt_tables_search.
 * \param [in] table_name Table name.
 * \param [in] fields Default fields array of table.
 * \param [in] num_fields Number of default fields array of table.
 *
 * \return Always 0.
 */
typedef int (*bcma_bcmlt_table_info_f)(void *cookie,
                                       const char *table_name,
                                       bcmlt_field_def_t *fields,
                                       uint32_t num_fields);

/*!
 * \brief Tables search function.
 *
 * Search tables on specific device by search_name. If the search matches,
 * The matched table and default fields information will be passed through
 * callback function.
 *
 * Two match modes for search are supported.
 * If \c match_mode is BCMA_BCMLT_MATCH_EXACT, the table
 * will be matched if search_name is exactly the same to table name.
 * If \c match_mode is BCMA_BCMLT_MATCH_SUBSTR, the
 * table will be matched if table_name contains search_name where case is
 * ignored.
 *
 * Special characters starting in \c search_name can identify different match
 * cases in BCMA_BCMLT_MATCH_SUBSTR.
 * 1. @: Exact match
 * 2. ^: Match from start
 * 3. *: Match in string
 *
 * If \ref BCMA_BCMLT_SCOPE_F_LTBL is set in \c search_scope,
 * logical tables will be included in the table search scope.
 * If \ref BCMA_BCMLT_SCOPE_F_PTBL is set in \c search_scope,
 * physical tables will be included in the table search scope.
 * If none of these two flags are set in \c search_scope, the search scope will
 * be equivalent to both flags are set. i.e. The seach scope is in both
 * logical and physical tables.
 *
 * \param [in] unit Unit number.
 * \param [in] search_name Table name or sub-string to be searched.
 * \param [in] match_mode Match mode in search.
 * \param [in] search_scope Search in logical or/and physical tables.
 * \param [in] cb Callback function when \c search_name matches.
 * \param [in] cookie User data for callback function.
 *
 * \return Number of tables matching the search criteria.
 */
extern int
bcma_bcmlt_tables_search(int unit, const char *search_name,
                         bcma_bcmlt_match_mode_t match_mode,
                         uint32_t search_scope,
                         bcma_bcmlt_table_info_f cb, void *cookie);

/*!
 * \brief Initialization function for tables search.
 *
 * This function allocates the required resources to ensure that
 * \ref bcma_bcmlt_tables_search will be thread-safe.
 *
 * \return 0 on success otherwise -1.
 */
extern int
bcma_bcmlt_tables_search_init(void);

/*!
 * \brief Cleanup function for tables search.
 *
 * This function frees the sources allocated in
 * \ref bcma_bcmlt_tables_search_init.
 *
 * \return Always 0.
 */
extern int
bcma_bcmlt_tables_search_cleanup(void);

/*!
 * \brief Callback function in \ref bcma_bcmlt_table_traverse.
 *
 * The traversed entry infromation and the index number the entry is
 * traversed are passed to the traverse callback function.
 *
 * \param [in] cookie User data through \ref bcma_bcmlt_table_traverse.
 * \param [in] ei The traversed entry information.
 * \param [in] entry_no The index number of the traversed entry.
 *
 * \return 0 on success otherwise -1.
 */
typedef int (*bcma_bcmlt_traverse_f)(void *cookie,
                                     bcma_bcmlt_entry_info_t *ei,
                                     int entry_no);

/*!
 * \brief Call a user-defined function for all entries in a table.
 *
 * This function traverses a logical table \c lt_name and
 * calls back to \ref bcma_bcmlt_traverse_f \c cb on every
 * traversed entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name Logical table name to be traversed.
 * \param [in] attr Entry attributes.
 * \param [in] fields Default fields array of table.
 * \param [in] num_fields Number of default fields array of table.
 * \param [in] cb Callback function for each traversed entry.
 * \param [in] cookie User data for callback function.
 *
 * \return Total number of entries traversed if > 0, otherwise
 *         SHR_E_xxx error codes.
 */
extern int
bcma_bcmlt_table_traverse(int unit, const char *lt_name, uint32_t attr,
                          bcmlt_field_def_t *fields, uint32_t num_fields,
                          bcma_bcmlt_traverse_f cb, void *cookie);

/*!
 * Create and initialize transaction information object.
 *
 * This function allocates \ref bcma_bcmlt_transaction_info_t instance and
 * allocates a transaction with transaction type \c type from bcmlt module.
 *
 * The \c params are the parameters used when committing the created
 * transaction. If \c params is NULL, default values will be used to commit
 * the transaction.
 *
 * \param [in] unit Unit number.
 * \param [in] type Transaction type (atomic or batch).
 * \param [in] params Parameters used in transaction commit.
 *
 * \return Pointer to allocated \ref bcma_bcmlt_transaction_info_t instance,
 *         otherwise NULL.
 */
extern bcma_bcmlt_transaction_info_t *
bcma_bcmlt_transaction_info_create(int unit, bcmlt_trans_type_t type,
                                   bcma_bcmlt_commit_params_t *params);

/*!
 * \brief Destroy transaction information object.
 *
 * This function frees the specified \ref bcma_bcmlt_transaction_info_t
 * instance and frees the transaction in bcmlt module. The entries associate
 * to the transaction will be freed as well.
 *
 * \param [in] ti Transaction information to be freed.
 *
 * \return 0 No errors, otherwise failure on invalid argument \c ti.
 */
extern int
bcma_bcmlt_transaction_info_destroy(bcma_bcmlt_transaction_info_t *ti);

/*!
 * \brief Add an entry to a transaction.
 *
 * A transaction can contain multiple entries and commit at one time.
 * This function is used to add an entry to a transaction to commit later.
 *
 * \param [in] ti Transaction information.
 * \param [in] ei Entry information.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_transaction_entry_add(bcma_bcmlt_transaction_info_t *ti,
                                 bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Create and initialize LT or PT entry information object.
 *
 * This function allocates \ref bcma_bcmlt_entry_info_t instance and
 * allocates entry from bcmlt module with \c table_name
 * for the specified device.
 *
 * If the default fields information \c fields is NULL, this function will
 * search the given table name among all tables through \ref
 * bcma_bcmlt_tables_search to verify the given table name and
 * retrieve the default fields information of the table. If this function
 * is called within a callback function of \ref bcma_bcmlt_tables_search,
 * the caller should specify the default fields information \c fields and
 * \c num_fields to avoid recursive calls to \ref bcma_bcmlt_tables_search.
 *
 * \param [in] unit Unit number.
 * \param [in] table_name Name of table of the allocated entry.
 * \param [in] physical Specify if the entry is in a logical table
 *                      or a physical table. True for physical table.
 * \param [in] attr Entry attributes (BCMLT_ENT_ATTR_xxx).
 * \param [in] params Parameters used in entry commit.
 * \param [in] fields Default fields information of the given table.
 * \param [in] num_fields Number of Default fields in the given table.
 *
 * \return Pointer to allocated \ref bcma_bcmlt_entry_info_t instance,
 *         otherwise NULL.
 */
extern bcma_bcmlt_entry_info_t *
bcma_bcmlt_entry_info_create(int unit, const char *table_name,
                             bool physical, uint32_t attr,
                             bcma_bcmlt_commit_params_t *params,
                             bcmlt_field_def_t *fields, uint32_t num_fields);

/*!
 * \brief Create and initialize LT or PT entry information object.
 *
 * This function is similar to \ref bcma_bcmlt_entry_info_create except that
 * the \c table_name is case sensitive and will not be verified through
 * \ref bcma_bcmlt_tables_search.
 *
 * \param [in] unit Unit number.
 * \param [in] table_name Name of table of the allocated entry.
 * \param [in] physical Specify if the entry is in a logical table
 *                      or a physical table. True for physical table.
 * \param [in] attr Entry attributes (BCMLT_ENT_ATTR_xxx).
 * \param [in] params Parameters used in entry commit.
 * \param [in] fields Default fields information of the given table.
 * \param [in] num_fields Number of Default fields in the given table.
 *
 * \return Pointer to allocated \ref bcma_bcmlt_entry_info_t instance,
 *         otherwise NULL.
 */
bcma_bcmlt_entry_info_t *
bcma_bcmlt_entry_info_create_exact_name(int unit, const char *table_name,
                                        bool physical, uint32_t attr,
                                        bcma_bcmlt_commit_params_t *params,
                                        bcmlt_field_def_t *fields,
                                        uint32_t num_fields);

/*!
 * \brief Destroy entry information object.
 *
 * This function frees entry in bcmlt module and frees
 * \ref bcma_bcmlt_entry_info_t instance for specified device.
 *
 * \param [in] ei Entry information to be freed.
 *
 * \return 0 No errors, otherwise failure on invalid argument \c ei or
 *         fail to free entry in bcmlt.
 */
extern int
bcma_bcmlt_entry_info_destroy(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief List entry information.
 *
 * If \c key_only is TRUE, this function lists the values of key fields
 * in the specified table entry. Otherwise all fields data will be listed.
 *
 * \param [in] ei Specified entry information.
 * \param [in] prefix Prefix string to be added for each field if not NULL.
 * \param [in] key_only List key fields only if non-zero.
 * \param [in] disp_mode Output display mode of field value.
 * \param [out] pb Output print buffer. When NULL, print to CLI output.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_list(bcma_bcmlt_entry_info_t *ei, const char *prefix,
                      bool key_only, bcma_bcmlt_display_mode_t disp_mode,
                      shr_pb_t *pb);

/*!
 * \brief List entry information in YAML format.
 *
 * If \c key_only is TRUE, this function lists the values of key fields
 * in the specified table entry. Otherwise all fields data will be listed.
 *
 * \param [in] ei Specified entry information.
 * \param [in] prefix Prefix string to be added for each field if not NULL.
 * \param [in] key_only List key fields only if non-zero.
 * \param [in] disp_mode Output display mode of field value.
 * \param [out] pb Output print buffer. When NULL, print to CLI output.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_list_yaml(bcma_bcmlt_entry_info_t *ei, const char *prefix,
                           bool key_only, bcma_bcmlt_display_mode_t disp_mode,
                           shr_pb_t *pb);

/*!
 * \brief Create entry fields in entry information object.
 *
 * This function creates \c num_fields array size for \ref bcma_bcmlt_field_t
 * as the member \c fields in \ref bcma_bcmlt_entry_info_t.
 * The minimum array size is the total table fields number this entry
 * presents for.
 *
 * If this function is called multiple times, the previous created \c fields
 * will be destroyed and a clean \c fields array will be allocated.
 *
 * \param [in] ei Entry information.
 * \param [in] num_fields Array count to be allocated for \c fields.
 *
 * \return 0 No errors, otherwise failure.
 */
extern int
bcma_bcmlt_entry_fields_create(bcma_bcmlt_entry_info_t *ei,
                               uint32_t num_fields);

/*!
 * \brief Destroy entry fields in entry information object.
 *
 * This function frees the member \c fields (array of \ref bcma_bcmlt_field_t)
 * in the specified entry information \ref bcma_bcmlt_entry_info_t.
 *
 * \param [in] ei Entry information.
 *
 * \return 0 No errors, otherwise failure.
 */
extern int
bcma_bcmlt_entry_fields_destroy(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Create entry qualifier fields in entry information object.
 *
 * This function creates \c num_fields array size for \ref bcma_bcmlt_field_t
 * as the member \c fields in \ref bcma_bcmlt_entry_info_t.
 *
 * If this function is called multiple times, the previous created \c fields
 * will be destroyed and a clean \c fields array will be allocated.
 *
 * \param [in] ei Entry information.
 * \param [in] num_fields Array count to be allocated for \c qualifier fields.
 *
 * \return 0 No errors, otherwise failure.
 */
extern int
bcma_bcmlt_entry_qlfr_fields_create(bcma_bcmlt_entry_info_t *ei,
                                    uint32_t num_fields);

/*!
 * \brief Destroy qualifier entry fields in entry information object.
 *
 * This function frees the member \c qlfr_fields
 * (array of \ref bcma_bcmlt_field_t)
 * in the specified entry information \ref bcma_bcmlt_entry_info_t.
 *
 * \param [in] ei Entry information.
 *
 * \return 0 No errors, otherwise failure.
 */
extern int
bcma_bcmlt_entry_qlfr_fields_destroy(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Get the supported qualifier enum array.
 *
 * \return The pointer to the supported qualifier enum array.
 */
extern const bcma_cli_parse_enum_t *
bcma_bcmlt_field_qualifier_enum_get(void);

/*!
 * \brief Check whether a specified field matches the qualifier constraint.
 *
 * \param [in] field The field for comparison.
 * \param [in] qlfr_field The qualifier field.
 *
 * \return true if the field matches the qualifier, otherwise false.
 */
extern bool
bcma_bcmlt_field_qualifier(bcma_bcmlt_field_t *field,
                           bcma_bcmlt_field_t *qlfr_field);

/*!
 * \brief Parse qualifier operator from a field assignment string.
 *
 * The qualifier operator is only valid in LT traverse operation to filter
 * out the matching entries through the constraint of the qualifiers.
 *
 * \param [in] ei Specified entry information.
 * \param [in] field_arg Field assignment string to be parsed.
 *
 * \return \ref BCMA_BCMLT_FIELD_QLFR_xxx if qualifier operator is found.
 *         0 No qualifier operator is found, otherwise negative value.
 */
extern int
bcma_bcmlt_entry_field_parse_qualifier(bcma_bcmlt_entry_info_t *ei,
                                       char *field_arg);

/*!
 * \brief Parse a field assignment string into \ref bcma_bcmlt_field_t.
 *
 * Find the matching field name and parse the field assigned value
 * from the field assignment string.
 *
 * \param [in,out] ei Specified entry information.
 * \param [in,out] field Specified field information.
 * \param [in] field_arg Field assignment string to be parsed.
 *
 * \return 0 No errors, otherwise negative value.
 */
extern int
bcma_bcmlt_entry_field_parse(bcma_bcmlt_entry_info_t *ei,
                             bcma_bcmlt_field_t *field, char *field_arg);

/*!
 * \brief Add a field to a table entry.
 *
 * Add specified field value to entry. Note that the field name and
 * field flags should be properly assigned first.
 *
 * \param [in] ei Entry information.
 * \param [in] field Field information to be added to entry.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_field_add(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field);


/*!
 * \brief Get a field value from a table entry.
 *
 * Get specified field value from entry. Note that the field name and
 * field flags should be properly assigned first.
 *
 * If \c ignore_not_found is TRUE, this function will return no errors
 * when the BCMLT API returned value is SHR_E_NOT_FOUND. The input \c field
 * information will be cleaned up in this case.
 *
 * \param [in] ei Entry information.
 * \param [in,out] field Field information.
 * \param [in] ignore_not_found Ignore E_NOT_FOUND if TRUE.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_field_get(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field, bool ignore_not_found);

/*!
 * \brief Get all fields in a table entry.
 *
 * All fields in a table entry will be traversed and added to the \c fields
 * in the \c ei \ref bcma_bcmlt_entry_info_t structure. The \c fields in \c ei
 * will be refreshed and might contain holes after calling this function.
 * The caller must add sanity check (e.g. check field name is not NULL) when
 * iterating the \c fields array to get all the fields in a table entry.
 *
 * \param [in,out] ei Specified entry information contains the returned fields
 *                    array.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_fields_get(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Set field name and value to environment variable name and value.
 *
 * This function sets the specified field \c field to CLI local variable.
 * For array fields, the variable will be set per array element index.
 * The array element index is specified through \c idx. All elements will be
 * set to variables if \c idx is negative. The variable name for element n of
 * array field is name field-name__n. The variable name is the field name
 * for non-array field.
 *
 * \param [in] ei Entry information.
 * \param [in] field Field information.
 * \param [in] idx Array field element index to be set.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_field_var_set(bcma_bcmlt_entry_info_t *ei,
                               bcma_bcmlt_field_t *field, int idx);

/*!
 * \brief Set field values to environment variable.
 *
 * This function sets the specified field \c field to CLI local variable.
 * If the field is with array attribute, it will return failure.
 *
 * If the variable already exists, the value will be appended to
 * current variable content concatenated by a comma.
 *
 * For example, after a sequece of this function calls for n times for \c field
 * with field name \<field-name\>, the CLI local variable will be:
 *
 *     <field-name>=<val1>,<val2>,...<valn>
 *
 *     <valn> is the field value at the n'th call.
 *
 * \param [in] ei Entry information.
 * \param [in] field Field information.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_field_var_append(bcma_bcmlt_entry_info_t *ei,
                                  bcma_bcmlt_field_t *field);

/*!
 * \brief Set a group of fields values to environment variable.
 *
 * This function sets the specified fields group \c fields to CLI local
 * variable. If any field is with array attribute, it will return failure.
 *
 * A set of fields values will be grouped in braces separated by colons.
 * If the variable already exists, the value will be appended to
 * current variable content concatenated by a comma.
 *
 * For example, after a sequece of this function calls for n times
 * with fields f1, f2,..., and fm, the CLI local variable will be:
 *
 *     <var_name>={<f1-val1>:<f2-val1>:...:<fm-val1>},
 *                {<f1-val2>:<f2-val2>:...:<fm-val2>},
 *                ...
 *                {<f1-valn>:<f2-valn>:...:<fm-valn>}
 *
 *     <fm-valn> is the field value of fm at the n'th call.
 *
 * \param [in] ei Entry information.
 * \param [in] fields Fields information array.
 * \param [in] num_fields Number of fields information array.
 * \param [in] var_name CLI environment variable name.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_entry_field_var_append_group(bcma_bcmlt_entry_info_t *ei,
                                        bcma_bcmlt_field_t **fields,
                                        int num_fields,
                                        const char *var_name);

/*!
 * \brief Unset all variables set through \ref bcma_bcmlt_entry_field_var_set.
 *
 * This function will delete all entry field environment variables previously
 * set via \ref bcma_bcmlt_entry_field_var_set.
 *
 * \param [in] ei Entry information.
 */
extern int
bcma_bcmlt_entry_field_vars_unset(bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Initialize field information from the matching default field data.
 *
 * \param [in] field Field information.
 * \param [in] df Default field information.
 *
 * \return 0 No errors, otherwise negative value.
 */
extern int
bcma_bcmlt_field_init(bcma_bcmlt_field_t *field, bcmlt_field_def_t *df);

/*!
 * \brief Free the memory allocated dynamically for array-type field.
 *
 * This function can be called safely for all types of field. It checks
 * the type of the input field, then frees the memory allocated dynamically
 * for array-type one or does nothing for non-array-type one.
 *
 * \param [in] field Field information.
 *
 * \retval 0 No errors.
 */
extern int
bcma_bcmlt_field_array_free(bcma_bcmlt_field_t *field);

/*!
 * \brief Return the field attribute flags.
 *
 * \param [in] df Default field information.
 *
 * \return Field attribute (\ref BCMA_BCMLT_FIELD_F_xxx).
 */
extern uint32_t
bcma_bcmlt_field_flags(bcmlt_field_def_t *df);

/*!
 * \brief Set default value to the field structure \ref bcma_bcmlt_field_t.
 *
 * Set the field default value to the field information structure \c field
 * according to the specified field type.
 *
 * \param [in,out] field Specified field information.
 *
 * \return 0 No errors, otherwise negative value.
 */
extern int
bcma_bcmlt_field_default_val_set(bcma_bcmlt_field_t *field);

/*!
 * \brief Compare field value to its default value.
 *
 * \param [in] field Specified field information.
 *
 * \retval 0 The field value is same as default.
 * \retval -1 The field value is different from default.
 */
extern int
bcma_bcmlt_field_default_val_cmp(bcma_bcmlt_field_t *field);

/*!
 * \brief Parse field value from string.
 *
 * Parse field value from string \c field_arg to field data.
 * The string \c field_arg is in one of the following formats:
 * - field=val
 * - field[r1]=v1{,field[r2]=v2,...}
 *
 * \param [in] field_arg String of a field assignment.
 * \param [out] field Field information to hold the parsed result.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_field_parse(char *field_arg, bcma_bcmlt_field_t *field);

/*!
 * \brief Parse array field indices range from string.
 *
 * This function will check whether the input string is specified in array
 * field format of index or indices range in token[m] or token[m-n].
 * If \c str is specified in array field format, \c str will be updated to
 * token, and \c sidx, \c edix will be returned as m or m, n for [m] or [m-n]
 * respectively. Otherwise \c str will remained no change and \c sidx, \c edix
 * will be return in -1.
 *
 * \param [in,out] str String to be parsed for array field string.
 * \param [out] sidx Parsed start index result of array field indices range.
 * \param [out] eidx Parsed end index result of array field indices range.
 *
 * \return 0 No errors, otherwise -1.
 */
extern int
bcma_bcmlt_field_parse_array_index(char *str, int *sidx, int *eidx);

/*!
 * \brief Format field value to string.
 *
 * Format field value in \c field to print buffer \c pb.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] prefix Prefix string to be added if not NULL.
 * \param [in] field Specified field information containing field value to
 *                   be formatted.
 * \param [in] disp_mode CLI output display mode of field value.
 *
 * \return Pointer to the current string.
 */
extern const char *
bcma_bcmlt_field_format(shr_pb_t *pb, const char *prefix,
                        bcma_bcmlt_field_t *field,
                        bcma_bcmlt_display_mode_t disp_mode);

/*!
 * \brief Format field value to string in YAML format.
 *
 * Format field value in \c field to print buffer \c pb.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] prefix Prefix string to be added if not NULL.
 * \param [in] field Specified field information containing field value to
 *                   be formatted.
 * \param [in] disp_mode CLI output display mode of field value.
 *
 * \return Pointer to the current string.
 */
extern const char *
bcma_bcmlt_field_format_yaml(shr_pb_t *pb, const char *prefix,
                             bcma_bcmlt_field_t *field,
                             bcma_bcmlt_display_mode_t disp_mode);

/*!
 * \brief Format field default information to string.
 *
 * Format a given field default information in table \c tbl_name to
 * print buffer \c pb. A prefix string can be specified through \c pf_str
 * to customize the output format.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] pf_str Prefix string of the output format.
 * \param [in] unit Unit number.
 * \param [in] tbl_name Table name of the field.
 * \param [in] df Default field information.
 *
 * \return Pointer to the current string.
 */
extern const char *
bcma_bcmlt_entry_field_default_info_format(shr_pb_t *pb, const char *pf_str,
                                           int unit, const char *tbl_name,
                                           bcmlt_field_def_t *df);

/*!
 * \brief Get symbol strings of a specified symbol field.
 *
 * This function will return the symbol strings array which is valid for
 * symbol field \c field_name in table \c tbl_name.
 *
 * Note the caller is responsible for freeing the returned array
 * by sal_free().
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_name Table name of the symbol field.
 * \param [in] field_name The specified symbol field name.
 * \param [out] elems The returned array size.
 *
 * \return Pointer to the symbol strings array or NULL on failure.
 */
extern const char **
bcma_bcmlt_entry_field_symbol_strs_get(int unit, const char *tbl_name,
                                       const char *field_name,
                                       uint32_t *elems);

/*!
 * \brief Format the output in row x column matrix.
 *
 * Format the strings in array \c strs in row x column matrix.
 * The columns are calculated dynamically according to the current
 * window width and the maximum string length in the array. If the
 * array size is too large, the calculation will be skipped and the
 * strings will be listed in one column per line.
 *
 * \param [out] pb Print buffer object which is already created.
 * \param [in] prefix Prefix for the output to be displayed.
 * \param [in] strs Array of string that needs to be displayed.
 * \param [in] cnt Number of elements in the array.
 *
 * \return NONE.
 */
extern void
bcma_bcmlt_format_strs_array(shr_pb_t *pb, const char *prefix,
                             const char **strs, int cnt);

#endif /* BCMA_BCMLT_H */
