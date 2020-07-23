/*! \file bcmlt_ltable_field_array.c
 *
 * This file implements all the functionality associated with array fields.
 * Array fields can be scalar or symbol. The main functionality is the field
 * add and delete. Note that for arrays, the implementation keeps the array
 * fields sorted based on their index.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlt/bcmlt.h>
#include <bcmtrm/trm_api.h>
#include "bcmlt_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLT_ENTRY

/*!
 * \brief Data assignment function type.
 *
 * This is a function signature for value assignment function. The function
 * may assign uint64 or symbol to/from the field.
 *
 * \param [in] entry The entry containing the field.
 * \param [in,out] fld The field to assign data to/from it.
 * \param [in,out] val_arry Array of values to assign value to/from
 * \param [in] to_field Indicates the assignment direction. Either into the
 * field or from the field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*assign_f)(bcmtrm_entry_t *entry,
                        shr_fmm_t *fld,
                        void **val_arry,
                        bool to_field);

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Assign u64 value to a field or from field to u64 value.
 *
 * This function is used to assign a u64 value from the u64
 * array pointed by val_arry to the input field or vice versa. The direction
 * of assignment dictated by the value of to_field. After assigning the value
 * the function increment the pointer to point the next element in the array.
 *
 * \param [in,out] fld points to the field at assign the value to it.
 * \param [in,out] val_arry points to a pointer pointing to an array of boolean
 * values. This pointer is also incremented to point to the next element of the
 * array.
 * \param [in] to_field is true if the copy is from the array into the field
 * and false otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int assign_u64(bcmtrm_entry_t *entry,
                       shr_fmm_t *fld,
                       void **val_arry,
                       bool to_field)
{
    uint64_t **val = (uint64_t **)val_arry;
    if (to_field) {
        fld->data = **val;
    } else {
        **val = fld->data;
    }
    (*val)++;
    *val_arry = *val;
    return SHR_E_NONE;
}

/*!
 * \brief Assign value to or from symbol.
 *
 * This function assigned symbol value from enumeration value or enumeration
 * value from symbol. The operation is based on the value of the parameter
 * \c to_field.
 *
 * \param [in] entry Is a pointer to the entry structure.
 * \param [in,out] fld Is the field structure to assign the data to/from it.
 * \param [in] val_arry Is an array of values that should be used as source or
 * destination of the assignment. The function increments this pointer to
 * point to the next data element.
 * \param [in] to_field Indicates if the value should be assigned to or from
 * the field structure.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int assign_symbol(bcmtrm_entry_t *entry,
                          shr_fmm_t *fld,
                          void **val_arry,
                          bool to_field)
{
    /*
     * For assign_u64 the value is uint64_t, however, here the value is
     * a pointer. So we are at three pointers deep.
     */
    const char ***val = (const char ***)val_arry;
    int rv;

    if (to_field) {
        uint32_t enum_val = 0;
        rv = bcmlrd_field_symbol_to_value(entry->info.unit,
                                 entry->table_id,
                                 fld->id,
                                 **val,
                                 &enum_val);
        fld->data = enum_val;
    } else {
        rv = bcmlrd_field_value_to_symbol(entry->info.unit,
                                      entry->table_id,
                                      fld->id,
                                      (uint32_t)(fld->data & 0xFFFFFFFF),
                                      *val);
    }
    if (rv != SHR_E_NONE) {
        return rv;
    }
    (*val)++;
    *val_arry = (void *)*val;
    return rv;
}

/*!
 * \brief Verify symbol array values.
 *
 * This function verifies that all the symbol values, provided as input
 * to an array symbol add function are valid.
 *
 * \param [in] entry is a pointer to the entry structure.
 * \param [in] field_name is the field identifier associated with the symbol
 * values.
 * \param [in] data Is an array of symbol values.
 * \param [in] num_of_elem Indicates how many symbols present in the array
 * \c data
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int verify_sym(bcmtrm_entry_t *entry,
                      const char *field_name,
                      const char **data,
                      uint32_t num_of_elem)
{
    uint32_t f_id;
    bcmlt_field_def_t *f_def;
    uint32_t j;
    uint32_t enum_val = 0;

    SHR_FUNC_ENTER(entry->info.unit);

    if (num_of_elem == 0 || !field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                                            field_name,
                                            entry->db_hdl,
                                            &f_def,
                                            &f_id));

    if (!f_def->symbol) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (j = 0; j < num_of_elem; j++) {
        if (bcmlrd_field_symbol_to_value(entry->info.unit,
                                         entry->table_id,
                                         f_id,
                                         data[j],
                                         &enum_val) != SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Verify symbol array values based on field ID.
 *
 * This function verifies that all the symbol values expected to be assigned
 * into a field array.
 *
 * \param [in] entry is a pointer to the entry structure.
 * \param [in] fid is the field identifier associated with the symbol values.
 * \param [in] data Is an array of symbol values.
 * \param [in] num_of_elem Indicates how many symbols present in the array
 * \c data
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int verify_sym_by_id(bcmtrm_entry_t *entry,
                            uint32_t fid,
                            const char **data,
                            uint32_t num_of_elem)
{
    uint32_t j;
    uint32_t enum_val = 0;

    SHR_FUNC_ENTER(entry->info.unit);
    for (j = 0; j < num_of_elem; j++) {
        if (bcmlrd_field_symbol_to_value(entry->info.unit,
                                         entry->table_id,
                                         fid,
                                         data[j],
                                         &enum_val) != SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*! \brief Allocates field array.
 *
 *  This function allocates all the field elements of a field array.
 *  It returns a pointer to the start of the field array list.
 *
 * \param [in] start_idx is the first array index to add new elements.
 * \param [in] num_of_elem indicates the array size.
 * \param [in] field_id is the field array ID.
 * \param [in] dtag is the data tag of this field array.
 *
 * \return A pointer to the start of the list on success and NULL otherwise.
 */
static shr_fmm_t *field_array_alloc(bcmtrm_entry_t *entry,
                                    uint32_t start_idx,
                                    uint32_t num_of_elem,
                                    uint32_t field_id,
                                    shr_fmm_t **last_field)
{
    shr_fmm_t *start;
    shr_fmm_t *current;
    uint32_t j;

    if (entry->fld_mem_hdl) {
        if (!entry->free_fld) {
            sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
            entry->free_fld = shr_lmm_alloc_bulk(entry->fld_mem_hdl,
                                                 num_of_elem);
            sal_mutex_give(entry->entry_free_sync);
        }
        start = entry->free_fld;
        if (!start) {
            return NULL;
        }
        entry->free_fld = start->next;
    } else {
        start = shr_fmm_alloc();
        if (!start) {
            return NULL;
        }
    }
    start->id = field_id;
    start->idx = start_idx++;
    start->flags = 0;
    start->next = NULL;
    current = start;
    for (j = 1; j < num_of_elem; j++) {
        if (entry->fld_mem_hdl) {
            if (!entry->free_fld) {
                sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                entry->free_fld = shr_lmm_alloc_bulk(entry->fld_mem_hdl,
                                                     num_of_elem - 1);
                sal_mutex_give(entry->entry_free_sync);
            }
            current->next = entry->free_fld;
            if (!current->next) {
                entry->free_fld = start;
                return NULL;
            }
            entry->free_fld = current->next->next;
        } else {
            current->next = shr_fmm_alloc();
            if (!current->next) {
                while (start) {  /* Free whatever was allocated so far */
                    current = start;
                    start = start->next;
                    shr_fmm_free(current);
                }
                return NULL;
            }
        }
        current = current->next;
        current->id = field_id;
        current->idx = start_idx++;
        current->flags = 0;
        current->next = NULL;
    }
    *last_field = current;
    return start;
}

/*!
 * \brief Validate array field.
 *
 * This function validates that a given field is actually an array and that
 * other parameters associated with the array (such as start index) do not
 * conflict with the field attributes.
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] symbol indicates if the field is a symbol
 * \param [in] s_idx is the index where the current entries should start from
 * (within the array).
 * \param [in] n_of_elem indicates the number of elements to add to the array.
 * \param [out] f_id is the internal field ID of the field.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_validate(bcmtrm_entry_t *entry,
                                const char *field_name,
                                bool symbol,
                                uint32_t s_idx,
                                uint32_t n_of_elem,
                                uint32_t *f_id)
{
    bcmlt_field_def_t *f_def;
    uint32_t max_idx = n_of_elem + s_idx;

    SHR_FUNC_ENTER(entry->info.unit);
    if (!field_name || (n_of_elem == 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                                            field_name,
                                            entry->db_hdl,
                                            &f_def,
                                            f_id));

    if (f_def->depth < max_idx && f_def->elements < max_idx) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                              "Field %s is not array or operation " \
                              "exceeding array boundary\n"),
                     field_name));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (symbol != f_def->symbol) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Creates and inserts field array in an entry.
 *
 * This function creates and inserts a field array into an entry.
 * The function is data type agnostic and it uses
 * input assignment function to assign the proper data type.
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] s_idx is the index where the current entries should start from
 * (within the array).
 * \param [in] n_of_elem indicates the number of elements to add. It is assumed
 * that this number is larger than 0.
 * \param [in] data points to array of values.
 * \param [in] assign_func is a function to transfer values between field
 * presentation and user presentation.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_insert(bcmtrm_entry_t *entry,
                              uint32_t f_id,
                              uint32_t s_idx,
                              uint32_t n_of_elem,
                              void *data,
                              assign_f assign_func)
{
    int rv;
    shr_fmm_t *prev_fld;
    shr_fmm_t *field;
    shr_fmm_t *s_list;   /* Start of the list */
    shr_fmm_t *elem;
    void *local_data = data;
    uint32_t actual_start_idx;
    uint32_t actual_end_idx;
    shr_fmm_t *last_field;
    bool end_of_list;

    actual_start_idx = s_idx;
    actual_end_idx = s_idx + n_of_elem - 1;

    /* Search for the field with the same ID */
    if (entry->fld_arr) {
        field = entry->fld_arr[f_id];
    } else {
        field = bcmlt_find_field_in_entry(entry, f_id, &prev_fld);
    }
    prev_fld = NULL;
    if (field) {
        if (field->idx > actual_start_idx) {  /* If need to find the previous */
            bcmlt_find_field_in_entry(entry, f_id, &prev_fld);
        }

        /* This is the domain to focus the work on */
        end_of_list = false;
        while (actual_start_idx <= actual_end_idx) {
            /* Check if need to add the entry before the first one */
            if (field->idx > actual_start_idx) {
                if (entry->fld_mem_hdl) {
                    if (!entry->free_fld) {
                        sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                        elem = shr_lmm_alloc(entry->fld_mem_hdl);
                        sal_mutex_give(entry->entry_free_sync);
                    } else {
                        elem = entry->free_fld;
                        entry->free_fld = elem->next;
                    }
                } else {
                    elem = shr_fmm_alloc();
                }
                if (!elem) {
                    return SHR_E_MEMORY;
                }
                elem->id = f_id;
                elem->idx = actual_start_idx++;
                elem->flags = 0;
                rv = assign_func(entry, elem, &local_data, true);
                if (rv != SHR_E_NONE) {
                    return SHR_E_PARAM;
                }
                elem->next = field;
                if (prev_fld) {
                    prev_fld->next = elem;
                } else {
                    entry->l_field = elem;
                }
                prev_fld = elem;
                /* Update the array if needed */
                if (entry->fld_arr &&
                    entry->fld_arr[f_id]->idx >= actual_start_idx) {
                    entry->fld_arr[f_id] = elem;
                }
                continue;
            }
            if (field->idx == actual_start_idx) {
                rv = assign_func(entry, field, &local_data, true);
                if (rv != SHR_E_NONE) {
                    return SHR_E_PARAM;
                }
                actual_start_idx++;
                prev_fld = field;

                if (field->next && field->next->id == f_id) {
                    field = field->next;
                } else {
                    end_of_list = true;
                }
                continue;
            }
            if (field->idx < actual_start_idx) {
                if (end_of_list) {
                    if (entry->fld_mem_hdl) {
                        if (!entry->free_fld) {
                            sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                            elem = shr_lmm_alloc(entry->fld_mem_hdl);
                            sal_mutex_give(entry->entry_free_sync);
                       } else {
                            elem = entry->free_fld;
                            entry->free_fld = elem->next;
                        }
                    } else {
                        elem = shr_fmm_alloc();
                    }
                    if (!elem) {
                        return SHR_E_MEMORY;
                    }
                    elem->id = f_id;
                    elem->idx = actual_start_idx++;
                    elem->flags = 0;
                    rv = assign_func(entry, elem, &local_data, true);
                    if (rv != SHR_E_NONE) {
                        return SHR_E_PARAM;
                    }
                    if (prev_fld) {
                        elem->next = prev_fld->next;
                        prev_fld->next = elem;
                    } else {
                        elem->next = NULL;
                        entry->l_field = elem;
                    }
                    prev_fld = elem;
                    continue;
                } else {
                    prev_fld = field;
                    if (field->next && field->next->id == f_id) {
                        field = field->next;
                    } else {
                        end_of_list = true;
                    }
                }
            }
        }
    } else {  /* Didn't find a field with matching ID. */
        /* Add the entire list into the array */
        s_list = field_array_alloc(entry,
                                   actual_start_idx,
                                   actual_end_idx - actual_start_idx + 1,
                                   f_id,
                                   &last_field);
        if (!s_list) {
            return SHR_E_MEMORY;
        }

        /* Assign the data */
        for (field = s_list; field; field = field->next) {
            rv = assign_func(entry, field, &local_data, true);
            if (rv != SHR_E_NONE) {
                return SHR_E_PARAM;
            }
        }

        /* Add the array to the begining of the list */
        last_field->next = entry->l_field;
        entry->l_field = s_list;
        if (entry->fld_arr) {
            entry->fld_arr[f_id] = s_list;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Validates, creates and inserts field array in an entry.
 *
 * This function performs the actual validation of the field array insertion.
 * If the field is valid it calls a function to create and insert the
 * field array into an entry.
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] symbol indicates if the field is a symbol.
 * \param [in] s_idx is the index where the current entries should start from
 * (within the array).
 * \param [in] n_of_elem indicates the number of elements to add.
 * \param [in] data points to array of values.
 * \param [in] assign_func is a function to transfer values between field
 * presentation and user presentation.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_create(bcmtrm_entry_t *entry,
                              const char *field_name,
                              bool symbol,
                              uint32_t s_idx,
                              uint32_t n_of_elem,
                              void *data,
                              assign_f assign_func)
{
    int rv;
    uint32_t f_id;

    rv = field_array_validate(entry, field_name, symbol,
                              s_idx, n_of_elem, &f_id);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    return field_array_insert(entry, f_id, s_idx,
                              n_of_elem, data, assign_func);
}

/*!
 * \brief Obtains the values from an array elements.
 *
 * This function get the values of \c n_of_elem array elements starting from
 * index \c s_idx.
 *
 * \param [in] entry The entry to add the field array to.
 * \param [in] fid The ID of the field array.
 * \param [in] s_idx The starting point in the array where data should be read.
 * \param [in] n_of_elem The number of elements to get their values.
 * \param [out] data Array of output values. This array should be able to
 * contain \c n_of_elem data elements
 * \param [in] assign_func is a function to transfer values between field
 * presentation and user presentation.
 * \parqm [in] f_def Field definitions used to determine the default value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_get_by_id(bcmtrm_entry_t *entry,
                                uint32_t fid,
                                uint32_t s_idx,
                                uint32_t n_of_elem,
                                void *data,
                                uint32_t *r_elem_cnt,
                                assign_f assign_func,
                                bcmlt_field_def_t *f_def)
{
    shr_fmm_t *field, tmp_fld;
    void *local_data = data;
    uint32_t max_idx = n_of_elem + s_idx;

    SHR_FUNC_ENTER(entry->info.unit);
    if (n_of_elem == 0 || !data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Find the field array */
    if (entry->fld_arr) {
        field = entry->fld_arr[fid];
    } else {
        field = bcmlt_find_field_in_entry(entry, fid, NULL);
    }
    if (!field) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Field was not found in the entry\n")));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    /* Find the start index in the array */
    while (field && (field->id == fid) && (field->idx < s_idx)) {
        field = field->next;
    }

    /* Verify that indeed there is a good field */
    if (!field || (field->id != fid) || (field->idx < s_idx) ||
        (field->idx >= max_idx)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Field index was not found in the entry\n")));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    /* Copy the data */
    *r_elem_cnt = 0;
    tmp_fld.id = fid;
    while (field && (field->id == fid) && (field->idx < max_idx)) {
        /* If the index present in the fields copy the real data */
        if (*r_elem_cnt + s_idx == field->idx) {
            SHR_IF_ERR_EXIT(assign_func(entry, field, &local_data, false));
            field = field->next;
        } else {
            if (f_def) {
                tmp_fld.data = f_def->def;
            } else {
                tmp_fld.data = 0;
            }
            SHR_IF_ERR_EXIT(assign_func(entry, &tmp_fld, &local_data, false));
        }
        (*r_elem_cnt)++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get values from field array.
 *
 * This function reads the values from a field array and copy them
 * into supplied array
 *
 * \param [in] entry is the entry to add the field array to.
 * \param [in] field_name is the name of the field array.
 * \param [in] symbol indicates if the field is a symbol.
 * \param [in] s_idx is the index in the field array where the entries
 * should be read from.
 * \param [in] n_of_elem indicates the number of elements to get.
 * \param [in] data points to empty array where the values should be copied to.
 * \param [out] r_elem_cnt contains the number of elements that were written
 * into the data array.
 * \param [in] assign_func is a function to transfer values between field
 * presentation and user presentation.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int field_array_get(bcmtrm_entry_t *entry,
                           const char *field_name,
                           bool symbol,
                           uint32_t s_idx,
                           uint32_t n_of_elem,
                           void *data,
                           uint32_t *r_elem_cnt,
                           assign_f assign_func)
{
    uint32_t f_id;
    bcmlt_field_def_t *f_def;

    SHR_FUNC_ENTER(entry->info.unit);
    if (!field_name || (n_of_elem == 0) || !data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_db_field_info_get(entry->info.unit,
                                 field_name,
                                 entry->db_hdl,
                                 &f_def,
                                 &f_id));

    if (symbol != f_def->symbol) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(entry->info.unit,
                                "Symbol mismatch for field %s\n"), field_name));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
            field_array_get_by_id(entry, f_id, s_idx, n_of_elem,
                                  data, r_elem_cnt, assign_func, f_def));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions - Local to BCMLT
 */
shr_fmm_t *bcmlt_find_field_in_entry(bcmtrm_entry_t *entry,
                                     uint32_t field_id,
                                     shr_fmm_t **prev)
{
    shr_fmm_t *field = entry->l_field;

    if (prev) {
        *prev = NULL;
    }
    while (field) {
        if (field->id == field_id) {
            return field;
        }
        if (prev) {
            *prev = field;
        }
        field = field->next;
    }
    return NULL;
}


/*******************************************************************************
 * Public functions
 */
int bcmlt_entry_field_array_add(bcmlt_entry_handle_t entry_hdl,
                                    const char *field_name,
                                    uint32_t start_idx,
                                    uint64_t *data,
                                    uint32_t num_of_elem)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(field_array_create(entry, field_name, false,
                                               start_idx, num_of_elem,
                                               data, assign_u64));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       const char **data,
                                       uint32_t num_of_elem)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!data) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Verify the symbol input */
    SHR_IF_ERR_VERBOSE_EXIT(verify_sym(entry, field_name, data, num_of_elem));

    SHR_IF_ERR_EXIT(field_array_create(entry, field_name, true,
                                       start_idx, num_of_elem,
                                       (void *)data, assign_symbol));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_unset(bcmlt_entry_handle_t entry_hdl,
                                  const char *field_name,
                                  uint32_t start_idx,
                                  uint32_t num_of_elem)
{
    shr_fmm_t *field;
    shr_fmm_t *prev_field;
    shr_fmm_t *tmp_field;
    uint32_t field_id;
    uint32_t j = 0;
    bcmlt_field_def_t *fld_attr;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    uint32_t max_idx = start_idx + num_of_elem;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmlt_db_field_info_get(entry->info.unit,
                                field_name,
                                entry->db_hdl,
                                &fld_attr,
                                &field_id));
    if (fld_attr->depth < max_idx && fld_attr->elements < max_idx) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    field = bcmlt_find_field_in_entry(entry, field_id, &prev_field);

    while (j < num_of_elem) {
        if (field && field->id == field_id) {
            if (field->idx == j + start_idx) {
                field->flags |= SHR_FMM_FIELD_DEL;
                prev_field = field;
                field = field->next;
                j++;
            } else if (field->idx < j + start_idx) {
                prev_field = field;
                field = field->next;
            } else { /* Field idx is missing so create the field */
                if (entry->fld_mem_hdl) {
                    if (!entry->free_fld) {
                        sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                        tmp_field = shr_lmm_alloc(entry->fld_mem_hdl);
                        sal_mutex_give(entry->entry_free_sync);
                    } else {
                        tmp_field = entry->free_fld;
                        entry->free_fld = tmp_field->next;
                    }
                } else {
                    tmp_field = shr_fmm_alloc();
                }
                SHR_NULL_CHECK(tmp_field, SHR_E_MEMORY);
                tmp_field->id = field_id;
                tmp_field->flags = SHR_FMM_FIELD_DEL;
                tmp_field->idx = j + start_idx;
                j++;
                tmp_field->data = 0;

                /* Now add the field to the list */
                if (prev_field) {
                    tmp_field->next = prev_field->next;
                    prev_field->next = tmp_field;
                    prev_field = tmp_field;
                } else {
                    tmp_field->next = entry->l_field;
                    entry->l_field = tmp_field;
                    prev_field = tmp_field;
                    if (entry->fld_arr && !entry->fld_arr[field_id]) {
                        entry->fld_arr[field_id] = tmp_field;
                    }
                }
            }
        } else { /* No match, so the field is missing */
            if (entry->fld_mem_hdl) {
                if (!entry->free_fld) {
                    sal_mutex_take(entry->entry_free_sync, SAL_MUTEX_FOREVER);
                    tmp_field = shr_lmm_alloc(entry->fld_mem_hdl);
                    sal_mutex_give(entry->entry_free_sync);
                } else {
                    tmp_field = entry->free_fld;
                    entry->free_fld = tmp_field->next;
                }
            } else {
                tmp_field = shr_fmm_alloc();
            }
            SHR_NULL_CHECK(tmp_field, SHR_E_MEMORY);
            tmp_field->id = field_id;
            tmp_field->flags = SHR_FMM_FIELD_DEL;
            tmp_field->idx = j + start_idx;
            j++;
            tmp_field->data = 0;

            /* Now add the field to the list */
            if (prev_field) {
                tmp_field->next = prev_field->next;
                prev_field->next = tmp_field;
                prev_field = tmp_field;
            } else {
                tmp_field->next = entry->l_field;
                entry->l_field = tmp_field;
                prev_field = tmp_field;
                if (entry->fld_arr && !entry->fld_arr[field_id]) {
                    entry->fld_arr[field_id] = tmp_field;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_remove(bcmlt_entry_handle_t entry_hdl,
                                   const char *field_name,
                                   uint32_t start_idx,
                                   uint32_t num_of_elem)
{
    shr_fmm_t *field;
    shr_fmm_t *tmp_field;
    shr_fmm_t *prev_field;
    int rv;
    uint32_t field_id;
    bcmlt_field_def_t *fld_attr;
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);
    uint32_t max_idx = start_idx + num_of_elem;

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    if (!field_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    rv = bcmlt_db_field_info_get(entry->info.unit,
                                 field_name,
                                 entry->db_hdl,
                                 &fld_attr,
                                 &field_id);
    if (rv != 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (fld_attr->depth < max_idx && fld_attr->elements < max_idx) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!entry->l_field) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    field = bcmlt_find_field_in_entry(entry, field_id, &prev_field);

    if (!field) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    while (field &&
           field->id == field_id && field->idx < max_idx) {
        if (field->idx >= start_idx) {
            if (prev_field) {
                prev_field->next = field->next;
            } else {
                entry->l_field = field->next;
            }
            if (entry->fld_arr && entry->fld_arr[field_id] == field) {
                if (field->next && field->next->id == field_id) {
                    entry->fld_arr[field_id] = field->next;
                } else {
                    entry->fld_arr[field_id] = NULL;
                }
            }
            tmp_field = field;
            field = field->next;
            if (entry->fld_mem_hdl) {
                tmp_field->next = entry->free_fld;
                entry->free_fld = tmp_field;
            } else {
                shr_fmm_free(tmp_field);
            }
        } else {
            prev_field = field;
            field = field->next;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_get(bcmlt_entry_handle_t entry_hdl,
                                const char *field_name,
                                uint32_t start_idx,
                                uint64_t *data,
                                uint32_t num_of_elem,
                                uint32_t *r_elem_cnt)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    SHR_IF_ERR_VERBOSE_EXIT(field_array_get(entry, field_name, false,
                                            start_idx, num_of_elem, data,
                                            r_elem_cnt, assign_u64));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       const char **data,
                                       uint32_t num_of_elem,
                                       uint32_t *r_elem_cnt)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    SHR_IF_ERR_VERBOSE_EXIT(field_array_get(entry, field_name, true,
                                            start_idx, num_of_elem, data,
                                            r_elem_cnt, assign_symbol));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                      uint32_t fid,
                                      uint32_t start_idx,
                                      uint64_t *data,
                                      uint32_t num_of_elem)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    VALIDATE_FID(entry, fid);
    if (num_of_elem == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(field_array_insert(entry, fid,
                                               start_idx, num_of_elem,
                                               data, assign_u64));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_sym_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                          uint32_t fid,
                                          uint32_t start_idx,
                                          const char **data,
                                          uint32_t num_of_elem)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_VALIDATE(entry);
    VALIDATE_FID(entry, fid);
    if (num_of_elem == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Verify the symbol input */
    SHR_IF_ERR_VERBOSE_EXIT(verify_sym_by_id(entry, fid, data, num_of_elem));

    SHR_IF_ERR_EXIT(field_array_insert(entry, fid,
                                       start_idx, num_of_elem,
                                       (void *)data, assign_symbol));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                      uint32_t fid,
                                      uint32_t start_idx,
                                      uint64_t *data,
                                      uint32_t num_of_elem,
                                      uint32_t *r_elem_cnt)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    VALIDATE_FID(entry, fid);
    SHR_IF_ERR_VERBOSE_EXIT(field_array_get_by_id(entry, fid,
                                                  start_idx, num_of_elem, data,
                                                  r_elem_cnt, assign_u64,
                                                  NULL));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_entry_field_array_sym_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                          uint32_t fid,
                                          uint32_t start_idx,
                                          const char **data,
                                          uint32_t num_of_elem,
                                          uint32_t *r_elem_cnt)
{
    bcmtrm_entry_t *entry = bcmlt_hdl_data_get(entry_hdl);

    SHR_FUNC_ENTER(entry ? entry->info.unit : BSL_UNIT_UNKNOWN);
    ENTRY_GET_VALIDATE(entry);
    VALIDATE_FID(entry, fid);
    SHR_IF_ERR_VERBOSE_EXIT(field_array_get_by_id(entry, fid,
                                                 start_idx, num_of_elem, data,
                                                 r_elem_cnt, assign_symbol,
                                                 NULL));
exit:
    SHR_FUNC_EXIT();
}


