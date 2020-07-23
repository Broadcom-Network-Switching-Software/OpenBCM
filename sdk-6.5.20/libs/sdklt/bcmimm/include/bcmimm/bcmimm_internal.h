/*! \file bcmimm_internal.h
 *
 *  In-memory internal header file for front end.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMIMM_INTERNAL_H
#define BCMIMM_INTERNAL_H

#include <sal/sal_mutex.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/front_e_ha.h>

/*!
 * \brief Header space for every data block.
 */
#define BUFF_HEADER_SIZE sizeof(uint64_t)

/*!
 * \brief The maximal size (in bytes) of the table key
 */
#define BCMIMM_MAX_KEY_SIZE     32

/*!
 * \brief In-memory data pointer construction.
 */
#define BCMIMM_BUF_PTR(ptr,sub,offset) (ptr) = (sub << 24) | offset

/*!
 * \brief Obtain sub module information from IMM pointer
 */
#define BCMIMM_PTR_SUB_INFO(ptr) (ptr) >> 24

/*!
 * \brief Obtain offset information from IMM pointer
 */
#define BCMIMM_PTR_OFFSET_INFO(ptr) (ptr) & 0xFFFFFF

/*!
 * \brief In-memory pre-defined block sub-module IDs
 */
#define GENERIC_TBL_COMP_ID     0  /*!< Generic table component ID */
#define GENERIC_DATA_COMP_ID    1  /*!< Generic data component ID */
#define GENERIC_KEY_COMP_ID     2  /*!< Generic key component ID  */
#define TRANS_TBL_COMP_ID       3  /*!< Transaction table component ID */
#define AUX_TBL_COMP_ID         4  /*!< Auxiliary table component ID */

/*!
 * \brief In-memory dynamic sub-module IDs minimal value
 */
#define MIN_SUB_ID              (AUX_TBL_COMP_ID+1)

/*!
 * \brief In-memory convert bits size to byte size (round up)
 */
#define BCMIMM_BITS_TO_BYTES(x)  (((x) + 7) / 8)

/*!
 * \brief Enum describes the API (NB or SB) that manipulates the table.
 */
typedef enum bcmimm_tbl_owner_u {
    BCMIMM_TABLE_FREE, /*!< No ownership to the table */
    BCMIMM_TABLE_NB,   /*!< Northbound API owns the table */
    BCMIMM_TABLE_SB    /*!< Southbound API owns the table */
} bcmimm_tbl_owner_t;
/*!
 * \brief In-Memory HA table list.
 *
 * This structure contains the pointers to all the LT tables managed by this
 * module. This structure contains an array of pointers to LT table
 * information for every unit. It also contains pointers to the generic buffer
 * blocks. These buffer blocks are used by every logical table.
 */
extern imm_ha_blk_t *bcmimm_tables_list[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief In-Memory LT event structure.
 * This structure conatins all the callback functions and context that are
 * corresponding to specific table.
 */
typedef struct bcmimm_lt_event_s {
    bcmimm_lt_cb_t cb; /*!< The callback function */
    void *context;     /*!< Opaque context to pass to the callback function */
} bcmimm_lt_event_t;

/*!
 * \brief In-Memory table change event callback functions.
 *
 * This structure contains the pointers to all the components callback
 * functions. These functions will be invoke when the table associated
 * with them is changing.
 */
extern bcmimm_lt_event_t *bcmimm_tables_cb[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief In-Memory LT dynamic information.
 *
 * This structure conatins dynamic information about the LT that can be
 * obtained from the LRD.
 */
typedef struct {
    field_info_t *key_fields;  /*!< An array of all the key fields. */
    field_info_t *data_fields; /*!< An array of all the data fields. */
    bool update_only;/*!< Indicates if the table access type is update only. */
    bool index_alloc; /*!< If true index can be allocated by the component. */
    uint32_t min_idx;   /*!< The minimal index of the table. */
    const char *tbl_name; /*!< The LT name */
} bcmimm_table_dynamic_info_t;

/*!
 * \brief Global variable contains dynamic information of IMM tables.
 */
extern bcmimm_table_dynamic_info_t
    *bcmimm_dynamic_info[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Dynamic buffer management.
 *
 * This structure is being used to manage a dynamic buffer.
 */
typedef struct {
    sal_mutex_t sync_obj;    /*!< Mutex to protect the block (blk) integrity */
    uint8_t *blk;            /*!< Pointer to HA block content. */
} bcmimm_dynamic_blk_mgmt_t;

/*!
 * \brief Dynamic blocks per unit.
 *
 * This structure is being used to track the dynamic buffers that are used
 * for key and data storage as well as auxilary block. The purpose of this
 * structure is to enable the ability to store the entry information so abort
 * operation can be performed. The first entry will occupy the data and key
 * blocks where following entries will consume the auxiliary buffer. The
 * transaction buffer contains information about the transaction itself.
 */
typedef struct {
    /*! Block to store the data of an entry */
    bcmimm_dynamic_blk_mgmt_t data_blk;
    /*! Block to store the key fields of an entry. */
    bcmimm_dynamic_blk_mgmt_t key_blk;
    /*! Auxiliary buffer used to handle overflow in the data buffer. */
    bcmimm_dynamic_blk_mgmt_t aux_blk;
    bcmimm_dynamic_blk_mgmt_t trans_blk; /*!< Transaction information buffer. */
} bcmimm_dynamic_blk_t;

/*!
 * \brief Pointer to dynamic (non-HA) memories used in the operations.
 */
extern bcmimm_dynamic_blk_t bcmimm_dynamic_buffs[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Common information structure required for entry operation.
 */
typedef struct {
    table_info_t *tbl; /*!< LT information. */
    uint32_t trans_id; /*!< Transaction ID of this operation. */
    const bcmltd_fields_t *in; /*!< Input fields. */
    bcmltd_fields_t *extra_in; /*!< Extra fields from the component. */
    uint8_t *data_buf; /*!< Data buffer to save current entry state. */
    bool enable_ha; /*!< Indicate if state should be recovered. */
    bool aux;  /*!< Indicates if the auxilury buffer is used for data_buf. */
    uint32_t d_ptr; /*!< HA data pointer. */
    uint32_t k_ptr; /*!< HA key pointer. */
} bcmimm_common_info_t;
/*!
 * \brief In-Memory big endian indicator.
 *
 * This variable indicates if the environment is big or little endian.
 * The value of this variable is important when trying to compress uint64_t
 * type variables into smaller size.
 */
extern bool bcmimm_big_endian;

/*!
 *\brief Find a table within the LT served by this module.
 *
 * \param [in] unit Is the unit to search the table in.
 * \param [in] sid Is the table ID to find.
 * \param [out] idx Is the index in the tables list array of where the
 * table was found. If the caller doesn't care about the index this parameter
 * can be NULL.
 *
 * \return A pointer to the table information on success and NULL otherwise.
 */
extern table_info_t *bcmimm_tbl_find(int unit, uint32_t sid, uint32_t *idx);

/*!
 * \brief Call registered stage callback function.
 *
 * This function notifies a component that is registered to receive staging
 * events for a table.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] in The field list of the entry that is being changed.
 * \param [in] tbl The table being changed.
 * \param [in] idx The index is the index of the table in the master
 * \c table_list[]. This is the same index in \c bcmimm_tables_cb[] array where
 * the stage callback function can be found.
 * \param [in] op_arg Operation argument associated with the staging command.
 * \param [in] reason Value maps to one of the opcodes (insert/delete/update).
 * \param [out] returned_fields If the component wants to provide an extra list
 * of fields (typically read only fields) it should provide them via this
 * parameter.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_notify_stage(int unit,
                               const bcmltd_fields_t *in,
                               table_info_t *tbl,
                               uint32_t idx,
                               const bcmltd_op_arg_t *op_arg,
                               bcmimm_entry_event_t reason,
                               bcmltd_fields_t *returned_fields);

/*!
 * \brief Call registered lookup callback function.
 *
 * This function calls a component registered lookup function. The
 * lookup callback function will be called in the case of north bound lookup or
 * traverse operations. See bcmimm_lt_lookup_cb for more information.
 *
 * \param [in] unit The unit associated with the LT.
 * \param [in] in The key field list of the entry that is being looked up.
 * \param [in] tbl The table being looked up.
 * \param [in] idx The index is the index of the table in the master
 * \c table_list[]. This is the same index in \c bcmimm_tables_cb[] array where
 * the lookup callback function can be found.
 * \param [in] op_arg Operation arguments associated with the lookp command.
 * \param [in] lkup_type Requested type of lookup
 * (lookup/traverse/from hw).
 * \param [out] out List of fields that are expected to be returned. The
 * callback function may update the data of the fields in this variable.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_notify_lookup(int unit,
                                const bcmltd_fields_t *in,
                                table_info_t *tbl,
                                uint32_t idx,
                                const bcmltd_op_arg_t *op_arg,
                                bcmimm_lookup_type_t lkup_type,
                                bcmltd_fields_t *out);

/*!
 *\brief Extract key field from the key buffer.
 *
 * \param [in] tbl Pointer to the logical table information.
 * \param [in] dyn_info LT dynamic information. This object contains
 * information that can not be stored in HA memory such as real pointers.
 * \param [in] key_buf Key buffer where the key value should be
 * extracted from.
 * \param [in] idx The index of the key field in the table.
 * \param [in] fld Pointer to the field of which its value should be
 * extracted.
 *
 * \return None
 */
extern void bcmimm_extract_key_fld(table_info_t *tbl,
                                   bcmimm_table_dynamic_info_t *dyn_info,
                                   uint8_t *key_buf,
                                   int idx,
                                   bcmltd_field_t *fld);

/*!
 *\brief Construct the key from the key fields.
 *
 * \param [in] tbl Pointer to the logical table information.
 * \param [in] dyn_info LT dynamic information. This object contains
 * information that can not be stored in HA memory such as real pointers.
 * \param [in] in The fields of the entry.
 * \param [in] key_buf Key buffer where the key value should be placed.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_SHR_E_PARAM Missing key fields.
 * \retval SHR_E_INTERNAL Too many key fields in the LT definition.
 */
extern int bcmimm_key_build(table_info_t *tbl,
                            bcmimm_table_dynamic_info_t *dyn_info,
                            const bcmltd_fields_t *in,
                            uint8_t *key_buf);

/*!
 * \brief Initializes the in-memory logical tables associated for this unit.
 *
 * This function called by the system manager initializes tables associated
 * with this unit.
 *
 * \param [in] unit Is the unit to initialize.
 * \param [in] comp_id Is the component ID of this component.
 * \param [in] warm Indicates if the init is cold or warm.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_unit_init(int unit, uint8_t comp_id, bool warm);

/*!
 * \brief Shutdown the in-memory logical tables associated with this unit.
 *
 * This function shutdown all the in-memory LT associated with this unit.
 * It frees all associated resources. HA tables are not freed.
 *
 * \param [in] unit Is the unit to shutdown.
 *
 * \retval SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_unit_shutdown(int unit);

/*!
 * \brief Find table dimensions.
 *
 * This function calculates the max field size, the key length as well as
 * the max number of array elements (for field arrays). It does it using the
 * information provided by the LRD for this table.
 *
 * \param [in] unit Is the unit associated with the blocks.
 * \param [in] t_info Pointer to the table structure.
 * \param [in] num_fid The number of fields in the array \c f_info.
 * \param [in] f_info Is an array containing the information on all the fields
 * in the logical table.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_table_dimension_find(int unit,
                                       table_info_t *t_info,
                                       size_t num_fid,
                                       bcmlrd_client_field_info_t *f_info);

/*!
 *\brief Allocating working, non HA buffer.
 *
 * This function allocates a working buffer that can be used to stage an entry
 * into the back-end.
 *
 * \param [in] unit Is the unit associated with the operation.
 *
 * \return Pointer to the working buffer.
 */
extern uint8_t *bcmimm_working_buff_alloc(int unit);

/*!
 *\brief Free the working buffer.
 *
 * This function returned the working buffer to the general poll. Any threads
 * that were waiting to allocate the working buffer might get a change to
 * complete the allocation.
 *
 * \param [in] unit Is the unit associated with the operation.
 *
 * \return None.
 */
extern void bcmimm_working_buff_free(int unit);

/*!
 * \brief Determine if a given unit was initialized for IMM.
 *
 * \param [in] unit The unit to check.
 *
 * \return true The IMM was successfully initialized for this unit.
 * \return false The IMM was not successfully initialized for this unit.
 */
extern bool bcmimm_unit_is_active(int unit);

/*!
 * \brief Allocate dynamic buffers to support in-memory logical table.
 *
 * This function allocates all the dynamic buffers associated with an in-memory
 * logical table. The dynamic tables being allocated from HA memory (key and
 * data buffers) as well as regular memory (data and key field information).
 * The HA memory being allocated from known data and key buffer blocks.
 * See ha_buff_blk_alloc() for HA block allocations.
 *
 * \param [in] unit Is the unit associated with the table.
 * \param [in] t_info Pointer to the table structure.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_table_buffers_init(int unit, table_info_t *t_info);

/*!
 *\brief Extract entry fields from output BE buffers.
 *
 * \param [in] be_out Is the backend output after calling the BE lookup.
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [out] out Is the output field array where the fields will be
 * extracted to.
 *
 * \return The number of different fields (based on their IDs) that were
 * extracted. Array fields will only be counted once.
 */
extern int bcmimm_extract_buf_into_flds(bcmimm_be_fields_t *be_out,
                                        table_info_t *tbl,
                                        bcmltd_fields_t *out);

/*!
 *\brief Fills in default values.
 *
 * This function being call to complete the field list with fields that
 * has the default values. These fields were not explicitly set but in
 * some cases might be required by the application. This functionality
 * is only applicable for lookup operations.
 *
 * \param [in] unit The unit associated with the table.
 * \param [in,out] out Is the current output list of non-default fields.
 * \param [in] start Is the index in the \c out structure where fields with
 * default values can be placed.
 * \param [in] tbl Is the logical table associated with this entry.
 *
 * \return The number of fields that were added to the output parameter.
 */
extern uint32_t bcmimm_def_vals_fill(int unit,
                                     bcmltd_fields_t *out,
                                     uint32_t start,
                                     table_info_t *tbl);

/*!
 *\brief Initialize internal structure.
 *
 * This function being call once during init to create some resources
 * for the internal front end implementation.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_internal_init(void);

/*!
 *\brief Release internal resources before shutdown.
 *
 * This function being call once to release all the resources that were
 * internally used.
 *
 * \return None.
 */
extern void bcmimm_internal_shutdown(void);

/*!
 *\brief Allocate internal resources per unit.
 *
 * This function is called by the \ref bcmimm_unit_init function to allocate
 * some internal resources.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_MEMORY on failure to allocate resources.
 */
extern int bcmimm_internal_unit_init(int unit);

/*!
 *\brief Release per unit internal resources.
 *
 * This function being call during the shutdown of a unit to free the IMM
 * internal resources.
 *
 * \return None.
 */
extern void bcmimm_internal_unit_shutdown(int unit);

/*!
 *\brief Entry lookup function.
 *
 * This function being call to perform lookup of a particular entry in a
 * specified LT and unit. The function can be called by north or south bound
 * API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters, typically the key fields.
 * \param [out] out Is the output structure where the entry that was looked up
 * will be placed. Note that the output will not contain the key fields that
 * were part of the input.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_lookup(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Entry delete function.
 *
 * This function being call to delete a particular entry in a
 * specified LT and unit. The function can be called by north or south bound
 * API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters, typically the key fields.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_delete(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Entry update function.
 *
 * This function being call to perform an update of a particular entry in a
 * specified LT and unit. The function can be called by north or south bound
 * API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters. The input must include the key
 * fields and one or more data field.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_update(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Free field lists
 *
 * This function frees the elements of the key and data field lists back
 * into the field memory manager.
 *
 * \param [in] k_fields This is a list feels with key fields.
 * \param [in] d_fields This is a list feels with data fields.
 *
 * \return None
 */
extern void bcmimm_internal_free_lists(bcmltd_field_t *k_fields,
                                     bcmltd_field_t *d_fields);
/*!
 *\brief Entry insert function.
 *
 * This function is being called to perform entry insert into specified LT and
 * unit. The function can be called by north or south bound API.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] sid Is the LT ID
 * \param [in] in Is the input parameters. The input must include the key
 * fields and one or more data field.
 * \param [out] out Used for insert that resulted in an output parameters
 * such as key allocated.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_insert(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmltd_sid_t sid,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      bcmimm_tbl_owner_t owner);

/*!
 *\brief Compress input fields into back-end format.
 *
 * This function being called to compress the fields from the input \c in
 * into the back-end format. The back-end format has few buffers:
 * a. key_buf is the key buffer containing only the key information
 * b. f_data contains the fields data only
 * c. fid_buf contains information of the field IDs and index.
 *
 * \param [in] in Is the input parameters. The input must include the key
 * fields and one or more data field.
 * \param [in] extra_in Extra input fields generated by the component during
 * the stage callback.
 * \param [in] tbl Is the relevant table structure.
 * \param [in] dyn_info The dynamic information of the logical table. This
 * information retrieved during init.
 * \param [out] key_buf Is the buffer to place the key fields data into.
 * \param [out] f_data Is the buffer where the data field values will be
 * placed into.
 * \param [out] f_del Is a boolean buffer indicating true for a field that
 * should be deleted (unset).
 * \param [out] fid_buf Is the buffer where the data field's ID will be placed
 * into.
 *
 * \return Return the total number of data fields that were written into the
 * output buffers.
 */
extern uint32_t bcmimm_compress_fields_into_buffer
    (const bcmltd_fields_t *in,
     const bcmltd_fields_t *extra_in,
     table_info_t *tbl,
     bcmimm_table_dynamic_info_t *dyn_info,
     uint8_t *key_buf,
     uint8_t *f_data,
     bool *f_del,
     uint32_t *fid_buf);

/*!
 *\brief Entry get first (traverse start).
 *
 * This function finds the first entry of the table with matching sid.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] sid Is the LT ID of the traversed table.
 * \param [out] out Is the output where the first entry fields will be placed.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_get_first(int unit,
                                         const bcmltd_op_arg_t *op_arg,
                                         bcmltd_sid_t sid,
                                         bcmltd_fields_t *out,
                                         bcmimm_tbl_owner_t owner);

/*!
 *\brief Entry get next (traverse continue).
 *
 * This function finds the entry based on the input parameter \c in. It then
 * search for the entry followed the current entry.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] sid Is the LT ID of the traversed table.
 * \param [in] in Is the input containing the key value of the last entry that
 * was traversed.
 * \param [out] out Is the output where the next entry fields will be placed.
 * \param [in] owner Indicates if the caller is north or south bound API.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_intern_entry_get_next(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmltd_sid_t sid,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        bcmimm_tbl_owner_t owner);


/*!
 *\brief Enable/Disable HA support
 *
 * This function sets the support of HA feature within the IMM. Supporting HA
 * means that all the operations that can change LT should be reversable in
 * the case of abort.
 *
 * \param [in] enable Indicates if the feature is enabled (=true) or disabled.
 *
 * \return None.
 */
extern void bcmimm_ha_enable_set(bool enable);

/*!
 *\brief Indicating ISSU event on a particular unit.
 *
 * This function called by the ISSU DLL to indicate that there is an ISSU
 * event.
 *
 * \param [in] unit Is the unit where an ISSU event is taking place.
 *
 * \return None.
 */
extern void bcmimm_set_issu_event(int unit);

/*!
 *\brief Invokes validate function callback.
 *
 * This function construct the key and data field lists as input parameters
 * for the validation callback function.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] sid Is the LT ID.
 * \param [in] action Is the action associated with this entry.
 * \param [in] idx Is the index of the in-mem logical table.
 * \param [in] in Is the input fields to validate.
 * \param [in] tbl Is the LT information structure.
 *
 * \return SHR_E_NONE if successful and error code otherwise.
 */
extern int bcmimm_invoke_validate_cb(int unit,
                                     bcmltd_sid_t sid,
                                     bcmimm_entry_event_t action,
                                     uint32_t idx,
                                     const bcmltd_fields_t *in,
                                     table_info_t *tbl);

/*!
 *\brief Determines if a given field is a key field.
 *
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [in] dyn_info Dynamic info contains static information gathered
 * from the LRD regarding the LT fields attributes.
 * \param [in] fid Is the field ID to check.
 *
 * \return The index of the field if the field is a key and -1 otherwise.
 */
extern int bcmimm_is_key_field(table_info_t *tbl,
                               bcmimm_table_dynamic_info_t *dyn_info,
                               uint32_t fid);
/*!
 *\brief Construct the key value for direct table.
 *
 * This function construct the key value of a direct table from the entry input.
 * Direct table has limited key value (less than 32 bits) as the key is the
 * direct index of the entry.
 *
 * \param [in] tbl Table associated with the key construction.
 * \param [in] in Input entry fields.
 * \param [in] dyn_info Dynamic information associated with the table.
 * \param [out] key Key value.
 * \param [out] key_idx_ar Array of indexes of the key fields within the input
 * entry.
 *
 * \return None.
 */
extern void bcmimm_compress_direct_key(table_info_t *tbl,
                                       const bcmltd_fields_t *in,
                                       bcmimm_table_dynamic_info_t *dyn_info,
                                       uint32_t *key,
                                       uint32_t *key_idx_ar);

/*!
 *\brief Compress fields structures of direct tables.
 *
 * This function compressed field information for direct tables type.
 * The compression is being placed into the working buffer. The index of the
 * field that should be deleted will be placed into the f_del array where the
 * field ID will be placed into the working buffer (similar to regular fields).
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] tbl IMM Table information associated with this operation.
 * \param [in] in The input fields.
 * \param [in] extra_in More input fields obtained from the component
 * that is associated with the LT.
 * \param [in] key_idx_ar Key index array indicates the indexes of the fields
 * in in->field that are key fields.
 * \param [out] working_buf The buffer to place the compressed output.
 * \param [out] f_del Array indicating the index of the fields that should be
 * deleted.
 *
 * \return The number of fields that were compressed.
 */
extern uint32_t bcmimm_direct_compress_fields(int unit,
                                              table_info_t *tbl,
                                              const bcmltd_fields_t *in,
                                              const bcmltd_fields_t *extra_in,
                                              uint32_t *key_idx_ar,
                                              uint8_t *working_buf,
                                              bool *f_del);

/*!
 *\brief Extract compress fields structures of direct tables.
 *
 * This function extracts compressed field information for direct tables type.
 * Its operation is inverse to the function
 * \ref bcmimm_direct_compress_fields().
 *
 * \param [in] be_out The compressed buffer as it comes from the back end.
 * \param [in] tbl The relevant table of which the entry belongs to.
 * \param [out] out Structure to place the uncompressed fields information.
 *
 * \retval SHR_E_NONE
 */
extern int bcmimm_direct_extract_buf_into_flds(bcmimm_be_fields_t *be_out,
                                               table_info_t *tbl,
                                               bcmltd_fields_t *out);

/*!
 *\brief Insert an entry into direct table.
 *
 * This function inserts an entry into direct table type. The entry is first
 * been compressed by calling \ref bcmimm_direct_compress_fields(). The
 * compressed fields being handed to the back end which saves the fields values
 * in a HA memory.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_direct_entry_insert(int unit, bcmimm_common_info_t *info);

/*!
 *\brief Update an entry of a direct table.
 *
 * This function updates an entry of a direct table type. The entry input is
 * first been compressed by calling \ref bcmimm_direct_compress_fields(). The
 * compressed fields along with an array of fields that should be deleted are
 * being handed to the back end which updates the entry information in the HA
 * memory.
 * If atomic transaction is supported this function also saves the current
 * content of the entry in HA buffer. If the transaction is aborted the content
 * of this entry will be restored.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation.
 * \param [in] working_buf A buffer to use for field compression.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_direct_entry_update(int unit,
                                      bcmimm_common_info_t *info,
                                      uint8_t *working_buf);

/*!
 *\brief Delete an entry from a direct table.
 *
 * This function deletes an entry of a direct table type by calling the back-end
 * function.
 * If atomic transaction is supported this function also saves the current
 * content of the entry in HA buffer. If the transaction is aborted the content
 * of this entry will be restored.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_direct_entry_delete(int unit, bcmimm_common_info_t *info);

/*!
 *\brief Lookup an entry of a direct table.
 *
 * This function look up an entry of a direct table type by calling the back-end
 * lookup function.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation. In
 * particular this structure contains a pointer to the out buffer where the
 * entry content will be written into.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_direct_entry_lookup(int unit, bcmimm_common_info_t *info);

/*!
 *\brief Updates the buffers with final length information.
 *
 * This function will be called after a call to
 * \ref bcmimm_working_buff_alloc().
 * The function updates the first word of the data buffer to contain the
 * opcode and the length of the data buffer. If the buffers were allocated
 * from the auxiliary block, the function also updates the actual length
 * that was consumed and free the lock from the auxiliary block to be used
 * for another entries.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] data_buf is a pointer to the data buffer.
 * \param [in] d_len This is the actual used data length.
 * \param [in] k_len This is the actual key length.
 * \param [in] d_elem This is the number of data elements.
 * \param [in] opcode This is the opcode associated with this entry.
 * \param [in] aux This indicates if the auxiliary block was used.
 *
 * \return None.
 */
extern void bcmimm_buf_hdr_update(int unit,
                                  uint8_t *data_buf,
                                  uint32_t d_len,
                                  uint32_t k_len,
                                  uint32_t d_elem,
                                  uint8_t opcode,
                                  bool aux);

/*!
 *\brief Updates the transaction block information.
 *
 * This function updates the HA dedicated to transaction information with
 * the new transaction information. This HA block tracks the table, the
 * key and data pointers for every element in the transaction. It is assumed
 * that all the elements in the same transaction are of the same unit.
 * This function should be called for every successful operation other than
 * the lookup/traverse operations. The reason for that is that this function
 * sets up a record for the transaction. When the transaction is committed
 * or aborted the IMM must call the component commit/abort for modeled LTs.
 * By saving the transaction information the commit/abort functions can call
 * the component with the proper information. Furthermore, when HA is enabled
 * the abort operation performs a recovery operation to restore the backend
 * to its previous state.
 * Do not use this function for lookup or traverse operations.
 *
 * \param [in] unit Is the unit associated with this transaction entry.
 * \param [in] tbl Is the table associated with this transaction entry.
 * \param [in] d_ptr This is HA pointer pointing to the data portion of
 * the entry.
 * \param [in] k_ptr This is HA pointer pointing to the key portion of
 * the entry.
 * \param [in] trans_id Is the transaction identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_update_trans_info(int unit,
                                    table_info_t *tbl,
                                    uint32_t d_ptr,
                                    uint32_t k_ptr,
                                    uint32_t trans_id);

/*!
 * \brief Indicates if the IMM boot was warm or cold.
 *
 * \retval true if in warm boot, false otherwise.
 */
extern bool bcmimm_is_warm(void);

/*!
 * \brief Perform all IMM upgrade functions.
 *
 * This function performs all the required for ISSU with respect to IMM.
 * In particular this function does the following:
 * a. Adjust LT back end to changes in LT definitions
 * b. Upgrade field IDs to their new values.
 * c. Upgrade LT IDs to their new values.
 * d. Upgrade enum field values to their new enum value.
 *
 * \param [in] unit The unit performing the ISSU.
 * \param [in] tables Array of all current IMM LT IDs
 * \param [in] imm_lts The number of IMM LTs.
 * \param [in] imm_direct The number of direct IMM LT. Note that
 * imm_lt >= imm_direct.
 * \param [in] fe_sub_id Tracking the maximal front end sub-id value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int bcmimm_issu_upgrade(int unit,
                               bcmlrd_sid_t *tables,
                               size_t imm_lts,
                               size_t imm_direct,
                               shr_ha_sub_id *fe_sub_id);

#endif /* BCMIMM_INTERNAL_H */
