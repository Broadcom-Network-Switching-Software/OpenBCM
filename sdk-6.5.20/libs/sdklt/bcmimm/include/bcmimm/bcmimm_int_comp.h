/*! \file bcmimm_int_comp.h
 *
 * In-memory Inter component API. This API should use by other components
 * (typically custom handlers) to access the in memory data.
 * This API has two main parts:
 * 1. An IMM API that can be used by the component. Any component may use
 * this API for every LT that is IMM backed.
 * 2. Component association with a specific LT. In this API the component
 * provides as set of callback functions that will be invoked by the IMM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMIMM_INT_COMP_H
#define BCMIMM_INT_COMP_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*****************************************************************************
 * First part of API where the component can set or query any LT that is IMM *
 * backed LT.                                                                *
 */

/*!
 * \brief Lookup for an entry.
 *
 * This function performs a lookup for an entry using the key fields included
 * in \c in structure. Once found, the function will construct the data fields
 * into the output array allocated at \c out.
 * The output list contains only fields that were inserted/updated.
 * Fields, belong to the table, that were never set will not be part of the
 * output.
 * Note - It is important that the caller will free every field in the output
 * list using the function shr_fmm_free().
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry
 * to be looked up. Data fields may be present.
 * \param [out] out This is an output structure. It should contain array of
 * field pointers that is sufficiently large to place all the data fields of the
 * entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_lookup(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out);

/*!
 * \brief Update an entry.
 *
 * This function updates an entry by providing the key fields along with the
 * data fields. Only the specified data fields will be modified. Existing
 * fields that are not part of the input will be unchanged.
 *
 * \param [in] unit This is device unit number.
 * \param [in] hi_pri Indicates if the operation should have high priority.
 * This parameter is mostly applicable to the notification that will be
 * generated in response to the change.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing all the fields of the entry
 * to be updated. Both key and data fields should be present.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_update(int unit,
                               bool hi_pri,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Delete an entry.
 *
 * This function deletes an entry using the entry key fields.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry.
 * Other fields might be present as well but will have no impact on the
 * operation.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_delete(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Insert a new entry.
 *
 * This function inserts a new entry into in-memory LT by providing the key
 * and data fields. All the key fields must be provided. Data fields might
 * be partially provide.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing all the fields of the entry
 * to be inserted. Both key and data fields should be present.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_insert(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Lock an entry of the table.
 *
 * This function locks an entry from the logical table specified via \c sid.
 * Locked entry can not be modified (or deleted). This functionality is
 * useful if the entry being referred by another field in a different logical
 * table. By default every entry is unlocked unless explicitly locked.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry
 * to be locked.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_lock(int unit,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in);

/*!
 * \brief Unlock an entry of the table.
 *
 * This function unlocks an entry that was previously locked. Once unlocked,
 * the entry can be modified or deleted.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in This is a structure containing the key fields of the entry
 * to be unlocked.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_unlock(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);

/*!
 * \brief Table traverse get first table entry.
 *
 * This function starts to traverse the table from its first entry.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [out] out This is a structure to place the entry content in. Note that
 * only fields that were set will be present in the output structure.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_get_first(int unit,
                                  bcmltd_sid_t sid,
                                  bcmltd_fields_t *out);

/*!
 * \brief Table traverse get next table entry.
 *
 * This function continues to traverse the table from its previously traversed
 * entry. The function \ref bcmimm_entry_get_first() should be called prior
 * of calling this function to initiate the traverse output. The output from
 * \ref bcmimm_entry_get_first() should be used at the first call to this
 * function. Otherwise only part of the table might be traversed as this
 * function will start where the previous call to this function was left off.
 * The order of the entries is based on internal oerder maintained by the
 * back-end module hence the caller should not assume anything about the order
 * of entries.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] in Is the input fields that must include the key fields. It is
 * initially being obtain via \ref bcmimm_entry_get_first() and later via
 * previous call to bcmimm_entry_get_next(). The input parameter for this
 * function is critical to find the next entry, that is the entry follows the
 * current entry of which its key being provided in this input parameter.
 * \param [out] out This is a structure to place the entry content in. Note that
 * only fields that were set will be present in the output structure.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_entry_get_next(int unit,
                                 bcmltd_sid_t sid,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out);

/*****************************************************************************
 * Second part of API where the component registers callback function with   *
 * a specific LT.                                                            *
 */
/*!
 * \brief In-memory notification reason.
 *
 * This enumerates the various reasons for notification.
 */
typedef enum bcmimm_entry_event_e {
    BCMIMM_ENTRY_INSERT,    /*!< Insert event. */
    BCMIMM_ENTRY_UPDATE,    /*!< Update event. */
    BCMIMM_ENTRY_DELETE,    /*!< Delete event. */
    BCMIMM_ENTRY_LOOKUP     /*!< Lookup event only used for verification */
} bcmimm_entry_event_t;

/*!
 * \brief In-memory lookup type.
 *
 * This enumerates the various types of lookup invocation.
 */
typedef enum bcmimm_lookup_type_e {
    BCMIMM_LOOKUP,      /*!< Normal lookup. */
    BCMIMM_TRAVERSE    /*!< The lookup is part of traverse. */
}bcmimm_lookup_type_t;

/*!
 * \brief In-memory logical table staging callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive table staging events for specific
 * logical table. The callback contains the field of the entry as they set
 * by the user along with the key value identifying the entry.
 * The IMM calls this function before updating its internal DB. From this
 * callback the component may call the IMM to retrieve any relevant entry.
 * The entry content will be the content prior to the current operation.
 * Therefore in a delete operation (\c event_reason == BCMIMM_ENTRY_DELETE)
 * the component may use the key(s) provided in the \c key_fields parameter
 * to retrieve the content of the entry.
 * If this function fails and the table is modeled (not interactive) then
 * the IMM will call the abort function \ref bcmimm_lt_abort_cb, if it was
 * registered.
 * Note: This function is being phased out, use \ref bcmimm_lt_stage_ex_cb()
 * instead.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Opaque parameter that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_stage_cb)(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key_fields,
                                 const bcmltd_field_t *data_fields,
                                 void *context,
                                 bcmltd_fields_t *output_fields);

/*!
 * \brief In-memory logical table extended staging callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive table staging events for specific
 * logical table. The callback contains the field values of the entry that
 * was changed along with the key value identify the entry. This is the
 * extended version of the stage callback function as it includes an extended
 * operation arguments (\c op_arg).
 * The IMM calls this function before updating its internal DB. From this
 * callback the component may call the IMM to retrieve any relevant entry.
 * The entry content will be the content prior to the current operation.
 * Therefore in a delete operation (\c event_reason == BCMIMM_ENTRY_DELETE)
 * the component may use the key(s) provided in the \c key_fields parameter
 * to retrieve the content of the entry.
 * If this function fails and the table is modeled (not interactive) then
 * the IMM will call the abort function \ref bcmimm_lt_abort_cb, if it was
 * registered.
 * Another duty of this function is to allocate a key (index) for LT's that
 * have the index_allocate attribute. For this type of LTs the component should
 * check if a key field exists (for event_reason = BCMIMM_ENTRY_INSERT) and if
 * not it should allocate the key field.
 * The key field information (when allocated by the component) should be added
 * to the \c output_fields parameter.
 *
 * \param [in] unit Device unit number.
 * \param [in] sid The logical table ID.
 * \param [in] op_arg The operation arguments. This parameter contains the
 * transaction ID as well as operation flags. These flags will have the prefix
 * BCMLT_ENT_ATTR_xxx.
 * \param [in] event_reason The reason for the entry event.
 * \param [in] key_fields Linked list of the key fields identifying the entry.
 * \param [in] data_field Linked list of the data fields in the modified entry.
 * \param [in] context Opaque parameter that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_stage_ex_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmimm_entry_event_t event_reason,
                                    const bcmltd_field_t *key_fields,
                                    const bcmltd_field_t *data_fields,
                                    void *context,
                                    bcmltd_fields_t *output_fields);

/*!
 * \brief In-memory transaction complete callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive transaction commit event
 * for specific logical table.
 * The callback parameter include the table ID and associated transaction ID to
 * commit.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_complete_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    uint32_t trans_id,
                                    void *context);

/*!
 * \brief In-memory transaction abort callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive transaction abort event
 * for specific logical table. This callback mechanism can
 * be used to abort a staged transaction.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return None.
 */
typedef void (bcmimm_lt_abort_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    uint32_t trans_id,
                                    void *context);

/*!
 * \brief In-memory entry validate callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to participate in the validation phase for specific
 * logical table. The callback contains the field values of the entry that
 * needed validation along with the key value identify the entry.
 * If the entry is valid this function should return no error (SHR_E_NONE),
 * otherwise it should return error code.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_validate_cb)(int unit,
                                    bcmltd_sid_t sid,
                                    bcmimm_entry_event_t action,
                                    const bcmltd_field_t *key_fields,
                                    const bcmltd_field_t *data_fields,
                                    void *context);

/*!
 * \brief In-memory entry lookup callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive an entry lookup event for a specific
 * logical table. The callback function will be called during normal lookup,
 * traverse or when required to read the content from hardware.
 * The purpose of this call is to allow the component to overwrite the values
 * of some data fields (in particular for dynamic fields) in addition to
 * informing the component that a lookup operation is taking place.
 *
 * The lookup operations defined by \ref bcmimm_lookup_type_t can be classified
 * into two operation categories, lookup and traverse.
 *
 * For the lookup category the key fields will be found in the \c in parameter
 * while for the traverse category the key fields will be found in the \c out
 * parameter.
 *
 * In all the operations the \c out field will contain all the data fields and
 * their associated values as maintained by the IMM or default values
 * (for fields that were not yet set).
 * The component may only change the data portions of the data fields.
 * For improved performance the fields will be sorted based on
 * their field ID. Array fields will be sorted based on their index.
 * Note: This function is being phased out, use \ref bcmimm_lt_lookup_ex_cb()
 * instead.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Is a structure which contains the key fields but may include
 * also other fields. The component should only focus on the key fields.
 * \param [in,out] out this structure contains all the fields of the
 * table, so that the component should not delete or add any field to this
 * structure. The data values of the field were set by the IMM so the component
 * may only overwrite the data section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_lookup_cb)(int unit,
                                   bcmltd_sid_t sid,
                                   uint32_t trans_id,
                                   void *context,
                                   bcmimm_lookup_type_t lkup_type,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out);

/*!
 * \brief In-memory entry lookup callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to receive an entry lookup event for a specific
 * logical table. The callback function will be called during normal lookup,
 * traverse or when required to read the content from hardware.
 * The purpose of this call is to allow the component to overwrite the values
 * of some data fields (in particular for dynamic fields) in addition to
 * informing the component that a lookup operation is taking place.
 *
 * The lookup operations defined by \ref bcmimm_lookup_type_t can be classified
 * into two operation categories, lookup and traverse.
 *
 * For the lookup category the key fields will be found in the \c in parameter
 * while for the traverse category the key fields will be found in the \c out
 * parameter.
 *
 * In all the operations the \c out field will contain all the data fields and
 * their associated values as maintained by the IMM or default values
 * (for fields that were not yet set).
 * The component may only change the data portions of the data fields. Key
 * fields should not be changed. Key fields will only be included in the \c out
 * parameter in the case were \c lkup_type is BCMIMM_TRAVERSE.
 * For improved performance the fields will be sorted based on
 * their field ID. Array fields will be sorted based on their index.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments. This parameter contains the
 * transaction ID as well as operation flags. These flags will have the prefix
 * BCMLT_ENT_ATTR_xxx. One flag of particular interest is
 * \c BCMLT_ENT_ATTR_GET_FROM_HW where it is expected that the values will be
 * read directly from the h/w.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * For BCMIMM_TRAVERSE the output key field is already in the \c out parameter.
 * For BCMIMM_LOOKUP only the \c in parameter contains the key field.
 * \param [in] in Is a structure which contains the key fields but may include
 * also other fields. The component should only focus on the key fields.
 * \param [in,out] out this structure contains all the fields of the
 * table, so that the component should not delete or add any field to this
 * structure. The data values of the field were set by the IMM so the component
 * may only overwrite the data section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_lookup_ex_cb)(int unit,
                                     bcmltd_sid_t sid,
                                     const bcmltd_op_arg_t *op_arg,
                                     void *context,
                                     bcmimm_lookup_type_t lkup_type,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out);

/*!
 * \brief In-memory max entry set.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to participate in setting the entry limit value.
 * This function will be called prior to setting the new entry limit value
 * of the IMM, which allows the application to reject the proposed value.
 *
 * \param [in] unit This is device unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] sid This is the logical table ID.
 * \param [out] count The value of entry limit.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_max_ent_set_cb)(int unit,
                                       uint32_t trans_id,
                                       bcmltd_sid_t sid,
                                       uint32_t count);

/*!
 * \brief In-memory entry limit get.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to participate in getting the entry limit value.
 * If the component registered this function with the IMM then the IMM will use
 * the component to provide the entry limit value. Otherwise, the IMM will
 * provide the entry limit value as it was read from the LRD.
 *
 * \param [in] unit This is device unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] sid This is the logical table ID.
 * \param [out] count The returned value of entry limit.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmimm_lt_ent_lmt_get_cb)(int unit,
                                       uint32_t trans_id,
                                       bcmltd_sid_t sid,
                                       uint32_t *count);

/*!
 * \brief In-memory event callback structure.
 *
 * This structure contains all possible callback functions
 * that will be corresponding to various entry commit stages.
 * For insert/update and delete operation the sequence of function callback
 * invocation is as followed:
 * i. Validate
 * ii. op_stage
 * iii. commit/abort. This call will only be made for modeled LTs. For
 * interactive LTs the entire operation will be done during the stage phase.
 *
 * Lookup and traverse operations will trigger the following sequence of
 * functions callback:
 * i. Validate (only for lookup operation)
 * ii. op_lookup
 */
typedef struct bcmimm_lt_cb_s {
    bcmimm_lt_validate_cb *validate;  /*!< Validate function */
    bcmimm_lt_stage_cb *stage;        /*!< Staging function */
    bcmimm_lt_stage_ex_cb *op_stage;  /*!< Extended staging function */
    bcmimm_lt_complete_cb *commit;    /*!< Commit function */
    bcmimm_lt_abort_cb *abort;        /*!< Abort function */
    bcmimm_lt_lookup_cb *lookup;      /*!< Lookup callback */
    bcmimm_lt_lookup_ex_cb *op_lookup; /*!< Extended lookup callback */
    bcmimm_lt_max_ent_set_cb *max_ent_set; /*!< Max entry set callback */
    bcmimm_lt_ent_lmt_get_cb *ent_limit_get; /*!< Entry limit get callback */
} bcmimm_lt_cb_t;
/*!
 * \brief Register to receive logical table change event.
 *
 * This function registers a callback function to be called whenever the
 * logical table identified by \c sid has changed. The caller can register a
 * functions for validation, staging, post commit and transaction abort.
 *
 *
 * \param [in] unit Is the device unit number.
 * \param [in] sid Is the logical table ID.
 * \param [in] cb Is the structure that contains the callback functions
 * required for this table.
 * \param [in] context Is a pointer that will be provided with the callback
 * functions. The caller can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_lt_event_reg(int unit,
                               bcmltd_sid_t sid,
                               bcmimm_lt_cb_t *cb,
                               void *context);

#endif /* BCMIMM_INT_COMP_H */

