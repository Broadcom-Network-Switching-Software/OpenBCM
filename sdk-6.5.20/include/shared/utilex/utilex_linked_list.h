/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file utilex_linked_list.h
 *
 * Definitions and prototypes for all common utilities related
 * to linked list.
 */
#ifndef UTILEX_LINKED_LIST_H_INCLUDED
/* { */
#define UTILEX_LINKED_LIST_H_INCLUDED

/*************
* INCLUDES  *
 */
/* { */
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_occupation_bitmap.h>
#include <shared/utilex/utilex_common.h>
#include <soc/dnx/swstate/types/utilex_linked_list_types.h>
/* } */
/*************
 * DEFINES   *
 */
/* { */
/**
 * \brief
 */
#define MAX_NUBER_OF_LISTS 100

/* } */

/*************
 * MACROS    *
 */
/* { */
/* } */
/*************
 * TYPE DEFS *
 */
/* { */

/**
 * \brief
 *   Typedef of procedure used to allocate data for a linked list.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in]  ll_id -
 *     Identifier of the linked list.
 *   \param [in]  size -
 *     Number of bytes on linked list.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * None
 */
typedef int (
    *utilex_ll_allocate_data_array) (
    int unit,
    int ll_id,
    uint32 size);

/**
 * \brief
 *   Typedef of procedure used to free data for a linked list.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in]  ll_id -
 *     Identifier of the linked list.
 *   \param [in]  size -
 *     Number of bytes on linked list.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * None
 */
typedef int (
    *utilex_ll_free_data_array) (
    int unit,
    int ll_id,
    uint32 size);

/**
 * \brief
 *   Typedef of procedure used to set data in a linked list.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in]  ll_id -
 *     Identifier of the linked list.
 *   \param [in]  index -
 *     Identifier of the linked list node.
 *   \param [in]  data -
 *     Pointer to a data which will be stored in the linked list node.
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * None
 */
typedef int (
    *utilex_ll_data_array_set) (
    int unit,
    int ll_id,
    uint32 index,
    void *data);

/**
 * \brief
 *   Typedef of procedure used to get data in a linked list.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in]  ll_id -
 *     Identifier of the linked list.
 *   \param [in]  index -
 *     Identifier of the linked list entry.
 *   \param [in]  data -
 *     Pointer to a variable in which will be stored the linked list entry data .
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * None
 */
typedef int (
    *utilex_ll_data_array_get) (
    int unit,
    int ll_id,
    uint32 index,
    void **data);

typedef struct
{
    utilex_ll_allocate_data_array allocate_data_array;
    utilex_ll_free_data_array free_data_array;
    utilex_ll_data_array_set array_set;
    utilex_ll_data_array_get array_get;
} linked_list_callbacks;
/* } */

/*************
* GLOBALS   *
 */
/* { */
/* } */

/*************
* FUNCTIONS *
 */
/* { */

/**
 * \brief
 *   Initialize control structure for ALL linked list instances expected.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 */
shr_error_e utilex_linked_list_init(
    int unit);
/**
 * \brief
 *   Creates a new Linked List pool instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] nof_elements -
 *     Number of linked lists elements which can be sustained simultaneously..
 *   \param [in] output_pool_id -
 *     Pointer to the pool index of the linked list pool.
 *   \param [in] cb_array_idx -
 *     Index of the call back procedure.
 * \par INDIRECT INPUT
 *   SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'output_pool_id' on DIRECT INPUT
 */
shr_error_e utilex_linked_list_pool_create(
    int unit,
    uint32 nof_elements,
    uint32 *output_pool_id,
    int cb_array_idx);

/**
 * \brief
 *   Creates a new Linked List instance
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] nof_elements -
 *     Number of linked lists elements which can be sustained simultaneously.
 *   \param [in] cb_array_idx -
 *     Index of the call back procedure.
 *   \param [in] handle -
 *     Pointer to the handle of the linked list.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   Initialized sw_state_legacy_access[unit].shareddnx.sand.linked_list: \n
 *   * See \b 'handle' on DIRECT INPUT
 */
shr_error_e utilex_linked_list_create(
    int unit,
    uint32 nof_elements,
    int cb_array_idx,
    uint32 *handle);

/**
 * \brief
 *   Get the data plus index of the head of the list
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] handle -
 *     Handle of the Linked List to remove. Sorted list is assumed to have been
 *     created.
 *   \param [in] data -
 *     Pointer to void. \n
 *     \b As \b output - \n
 *     This procedure loads '*data' by the data corresponding to the head of the Linked List.
 *     If 'index' indicates 'end of list' then nothing is loaded.
 *   \param [in] index -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*index' by the index corresponding head
 *     of the Linked List. If 'index' indicates 'end of list' then nothing is loaded.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'data' on DIRECT INPUT
 *   * See \b 'index' on DIRECT INPUT
 */
shr_error_e utilex_linked_list_head_get(
    int unit,
    uint32 handle,
    void *data,
    uint32 *index);

/**
 * \brief
 *   Remove an existing entry from the linked list. If entry does
 *   not exist then the operation returns an error.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] handle -
 *     Handle of the Linked List to remove. Sorted list is assumed to have been
 *     created.
 *   \param [in] index -
 *     Identifier of entry, on specified Linked List, to remove.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   *None
 */
shr_error_e utilex_linked_list_remove(
    int unit,
    uint32 handle,
    uint32 index);

/**
 * \brief
 *   Insert a new entry into the linked list.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] handle -
 *     Handle of the Linked List to add. Linked list is assumed to have been
 *     created.
 *   \param [in] idx_to_add_after -
 *     Index to entry to add after it into the linked list. The size of
 *     this array is per setup info specified upon creation of the
 *     Linked List.
 *   \param [in] data -
 *     Pointer to data to add into the linked list.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * See \b 'data' on DIRECT INPUT
 */
shr_error_e utilex_linked_list_add(
    int unit,
    uint32 handle,
    uint32 idx_to_add_after,
    void *data);

/**
 * \brief
 *   Get 'index plus data' of the entry just next to the one specified by index.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] handle -
 *     Handle of the Linked List to add. Linked list is assumed to have been
 *     created.
 *   \param [in] index -
 *     Index of an entry on linked list to get the 'next' of.
 *   \param [in] data -
 *     Pointer to void. \n
 *     \b As \b output - \n
 *     This procedure loads '*data' by the data corresponding to the 'next' (higher)
 *     iterator on Linked List. If 'index' indicates 'end of list' then nothing is loaded.
 *   \param [in] next_index -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*next_index' by the index corresponding to the 'next' (higher)
 *     iterator on Linked List. If 'index' indicates 'end of list' then nothing is loaded.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * See \b 'data' on DIRECT INPUT
 *   * See \b 'next_index' on DIRECT INPUT
 */
shr_error_e utilex_linked_list_get_next(
    int unit,
    uint32 handle,
    uint32 index,
    void *data,
    uint32 *next_index);

/**
 * \brief
 *   Get 'index plus data' of the entry just before of the one specified by index.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] handle -
 *     Handle of the Linked List to add. Linked list is assumed to have been
 *     created.
 *   \param [in] index -
 *     Index to entry of an entry on linked list to get the 'next' of.
 *   \param [in] data -
 *     Pointer to void. \n
 *     \b As \b output - \n
 *     This procedure loads '*data' by the data corresponding to the 'previous' (lower)
 *     iterator on Linked List. If 'index' indicates 'beginning of list' then nothing is loaded.
 *   \param [in] prev_index -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*prev_index' by the index corresponding to the 'previous' (lower)
 *     iterator on Linked List. If 'index' indicates 'beginning of list' then nothing is loaded.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \par INDIRECT OUTPUT
 *   * See \b 'data' on DIRECT INPUT
 *   * See \b 'prev_index' on DIRECT INPUT
 */
shr_error_e utilex_linked_list_get_prev(
    int unit,
    uint32 handle,
    uint32 index,
    void *data,
    uint32 *prev_index);
/* } */
#endif
