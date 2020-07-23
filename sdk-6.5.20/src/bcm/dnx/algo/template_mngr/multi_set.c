/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file multi_set.c
 *
 * All common utilities related to multi-set of bits.
 *
 * A multi-set is essentially a hash table with control over
 * the number of duplications (reference count) both per member and
 * over the total number of duplications. 
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*************
* INCLUDES  *
 */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <assert.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>

/*************
* DEFINES   *
 */
#define MULTI_SET_HASH_WIDTH_FACTOR 1

/*************
*  MACROS   *
 */

/*************
* TYPE DEFS *
 */

/*************
* GLOBALS   *
 */

#ifdef BCM_DNX_SUPPORT
/*
 * { 
 */
 /*************
 * FUNCTIONS *
 */

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_member_add(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    uint32 flags,
    int nof_additions,
    void *key,
    void *extra_arguments,
    int *data_indx,
    uint8 *first_appear)
{
    uint8 tmp_cnt[sizeof(uint32)];
    uint32 ref_count[1], found_index, adjusted_additions;
    uint8 with_id = 0;
    uint8 success = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    SHR_NULL_CHECK(key, _SHR_E_INTERNAL, "key");
    SHR_NULL_CHECK(data_indx, _SHR_E_INTERNAL, "data_indx");
    SHR_NULL_CHECK(first_appear, _SHR_E_INTERNAL, "first_appear");

    /*
     * Init local variables
     */
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));
    ref_count[0] = 0;
    adjusted_additions = 0;

    /*
     * 'found_index' is loaded as indirect output below, so it does not need to be
     * initialized. However, Coverity requires initialization...
     */
    found_index = 0;
    with_id = (_SHR_IS_FLAG_SET(flags, MULTI_SET_ADD_WITH_ID));

    /*
     * Calibrate the ID because the multiset starts from 0 but the IDs the caller gives can start from "first_profile"
     * given in the init_info.
     */
    if (with_id)
    {
        *data_indx = *data_indx - multi_set->init_info.first_profile;
    }

    if (nof_additions == 0)
    {
        success = FALSE;
        goto exit;
    }

    /*
     * If index was given by caller, add this member at at specified index.
     * Otherwise, add using input key and get corresponding index (into 'found_index'). 
     */
    if (with_id)
    {
        found_index = *(uint32 *) data_indx;
        /*
         * Note: Procedure sw_state_hash_table_entry_add_at_index() reports 'success' on trying to add
         * an existing entry.
         */
        SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add_at_index
                        (unit, module_id, multi_set->hash_table, (MULTI_SET_KEY *) key, found_index, &success));
    }
    else
    {
        /*
         * Note: Procedure sw_state_hash_table_entry_add() reports 'success' on trying to add
         * an existing entry.
         */
        SHR_IF_ERR_EXIT(sw_state_hash_table_entry_add
                        (unit, module_id, multi_set->hash_table, (MULTI_SET_KEY *) key, NULL, &found_index, &success));
    }
    /*
     * At this point, member has been added.
     *
     * If it is a singleton, and we didn't find it in the lookup (sw_state_hash_table_entry_lookup),
     * then mark it as 'first appearance' and quit.
     * Note that *success is as loaded by 'sw_state_hash_table_entry_add()'
     */
    /*
     * Save the index. 
     */
    *data_indx = (int) found_index;
    /*
     * Apparently, if *success is FALSE then 'found_index' must be SW_STATE_HASH_TABLE_NULL
     * and the other way round.
     */
    if (!success)
    {
        /*
         * Enter if *success is FALSE
         *
         * If we were given the index, we assume the user allocated the 
         * entry before the call. If we weren't given the index, we
         * assume the operation failed because the table is full.
         */
        *first_appear = (with_id) ? FALSE : TRUE;
        goto exit;
    }
    /*
     * At this point, member is NOT a singleton and was successfully
     * added to the hashtable of the multiset.
     */

    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * '(*data_indx) * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, (*data_indx) * multi_set->counter_size,
                         multi_set->counter_size, 0, "multi_set ref_counter.");

    *ref_count = 0;
    /*
     * Call the _get_by_index with data_index that is offsetted with first_profile, because the user will do the same logic.
     */
    SHR_IF_ERR_EXIT(multi_set_get_by_index
                    (unit, module_id, multi_set, *data_indx + multi_set->init_info.first_profile, (int *) ref_count,
                     key));
    /*
     * Set if maximum entries is required. 
     */
    if (nof_additions == UTILEX_U32_MAX)
    {
        adjusted_additions = (multi_set->init_info.max_duplications - (*ref_count));
    }
    else
    {
        adjusted_additions = nof_additions;
    }
    /*
     * Same value referenced more than initialized value times, return
     * operation fail.
     */
    if (((*ref_count) + adjusted_additions) >
        UTILEX_BITS_MASK((UTILEX_NOF_BITS_IN_BYTE * multi_set->counter_size - 1), 0))
    {
        /*
         * Overflowing the counter. 
         */
        success = FALSE;
        goto exit;
    }
    if (((*ref_count) + adjusted_additions) > multi_set->init_info.max_duplications)
    {   /* Adding more entries than there are available */
        success = FALSE;
        goto exit;
    }

    if (*ref_count == 0)
    {
        *first_appear = TRUE;
    }
    else
    {
        *first_appear = FALSE;
    }
    *ref_count += adjusted_additions;

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(ref_count, multi_set->counter_size, tmp_cnt));

    /*
     * Copy 'counter_size' bytes from 'tmp_cnt' buffer into 'multi_set->ref_counter'
     * buffer at offset '(*data_indx) * counter_size'.
     */
    DNX_SW_STATE_MEMWRITE(unit, module_id, tmp_cnt, multi_set->ref_counter, (*data_indx) * multi_set->counter_size,
                          multi_set->counter_size, 0, "multi_set ref_counter.");

exit:
    *data_indx = *data_indx + multi_set->init_info.first_profile;

    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FULL);
    }

    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_create(
    int unit,
    uint32 module_id,
    multi_set_t * multi_set,
    dnx_algo_template_create_data_t * init_info,
    void *extra_arguments,
    uint32 alloc_flags)
{
    uint32 max_duplications;
    uint32 member_size;
    sw_state_htbl_init_info_t hash_table_init_info;
    multi_set_info_t multi_set_init_info;
    int rv = 0;
    uint32 tmp_counter_size;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(*multi_set);

    if (extra_arguments != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "extra_arguments are not used, so thay should be NULL.");
    }

    /*
     * Init local variables
     */
    max_duplications = 0;
    member_size = 0;
    sal_memset(&hash_table_init_info, 0, sizeof(sw_state_htbl_init_info_t));

    /*
     * Init multi set create data.
     */
    sal_memset(&multi_set_init_info, 0x0, sizeof(multi_set_info_t));
    multi_set_init_info.max_duplications = init_info->max_references;
    multi_set_init_info.member_size = init_info->data_size;
    multi_set_init_info.nof_members = init_info->nof_profiles;
    multi_set_init_info.first_profile = init_info->first_profile;
    multi_set_init_info.data_size = init_info->data_size;

    rv = SHR_GET_CURRENT_ERR();

    /*
     * return error if multiset was already created (based on rv from MULTI_SET_VERIFY_MULTISET_IS_ACTIVE) 
     */
    if (rv == _SHR_E_INTERNAL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state multiset create ERROR: multiset already exist.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    DNX_SW_STATE_ALLOC(unit, module_id, *multi_set, **multi_set, /* number of elements */ 1,
                       DNXC_SW_STATE_NONE, "sw_state multi set");

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &((*multi_set)->init_info), &multi_set_init_info,
                              sizeof(multi_set_info_t), 0, "sw_state multi_set create");

    member_size = multi_set_init_info.member_size;
    max_duplications = multi_set_init_info.max_duplications;

    /*
     * Load 'init' section of multi set as per input.
     * Make sure all callbacks are NULL - Currently, we only use defaults
     */
    if (multi_set_init_info.nof_members == 0 || member_size == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

    if (max_duplications == UTILEX_U32_MAX)
    {
        max_duplications = UTILEX_U32_MAX - 1;

    }

    /*
     * create hash-table.
     */
    hash_table_init_info.max_nof_elements = multi_set_init_info.nof_members;
    hash_table_init_info.expected_nof_elements = multi_set_init_info.nof_members;
    hash_table_init_info.print_cb_name = multi_set_init_info.print_cb_name;

    SHR_IF_ERR_EXIT(sw_state_hash_table_create(unit, module_id, &((*multi_set)->hash_table), &hash_table_init_info, member_size, member_size, TRUE, 0   /* nof_members 
                                                                                                                                                         */ , alloc_flags));

    /*
     * calculate the size of pointers (list head and next) according to table size.
     */
    tmp_counter_size =
        (utilex_log2_round_up(max_duplications + 1) + (UTILEX_NOF_BITS_IN_BYTE - 1)) / UTILEX_NOF_BITS_IN_BYTE;

    DNX_SW_STATE_MEMCPY_BASIC(unit,
                              module_id,
                              &((*multi_set)->counter_size),
                              &tmp_counter_size, sizeof(tmp_counter_size), DNXC_SW_STATE_NO_FLAGS, "counter_size");

    DNX_SW_STATE_ALLOC(unit, module_id, (*multi_set)->ref_counter, DNX_SW_STATE_BUFF,
                       multi_set_init_info.nof_members * (*multi_set)->counter_size, alloc_flags,
                       "sw_state multi set ref counter");

    DNX_SW_STATE_MEMSET(unit, module_id, (*multi_set)->ref_counter, 0, 0x00,
                        multi_set_init_info.nof_members * (*multi_set)->counter_size, 0,
                        "sw_state multi set ref counter");

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_destroy(
    int unit,
    uint32 module_id,
    multi_set_t * multi_set,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE((*multi_set));

    if (extra_arguments != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "extra_arguments are not used, so thay should be NULL.");
    }

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    SHR_IF_ERR_EXIT(sw_state_hash_table_destroy(unit, module_id, &((*multi_set)->hash_table)));

    DNX_SW_STATE_FREE(unit, module_id, (*multi_set)->ref_counter, "sw state multi_set ref_counter");

    DNX_SW_STATE_FREE(unit, module_id, (*multi_set), "sw state multi_set ref_counter");

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_member_remove_by_index_multiple(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    int data_indx,
    int remove_amount,
    uint8 *last_appear)
{
    uint32 adjusted_remove_amount = 0, ref_counter = 0;
    uint8 tmp_cnt[sizeof(uint32)];
    int _data_indx = 0;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    /*
     * calibrate the Data ID because the multiset starts from
     * ID 0 but the IDs the caller gives can start
     * from "first_profile" given in the init_info
     */
    _data_indx = data_indx - multi_set->init_info.first_profile;

    /*
     * Init local variables
     */
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));

    SHR_NULL_CHECK(last_appear, _SHR_E_INTERNAL, "last_appear");

    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * 'data_indx * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, _data_indx * multi_set->counter_size,
                         multi_set->counter_size, 0, "ref counter");

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &ref_counter));

    if (ref_counter == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trying to free a non existing profile.\n");
    }

    /*
     * Remove all entries
     */
    if ((remove_amount > ref_counter))
    {
        adjusted_remove_amount = ref_counter;
    }
    else
    {
        adjusted_remove_amount = remove_amount;
    }

    ref_counter -= adjusted_remove_amount;
    *last_appear = (ref_counter == 0) ? TRUE : FALSE;
    SHR_IF_ERR_EXIT(utilex_U32_to_U8(&ref_counter, multi_set->counter_size, tmp_cnt));

    /*
     * Copy 'counter_size' bytes from 'tmp_cnt' buffer into 'multi_set->ref_counter'
     * buffer at offset 'data_indx * counter_size'.
     */

    DNX_SW_STATE_MEMWRITE(unit, module_id, tmp_cnt, multi_set->ref_counter, (_data_indx) * multi_set->counter_size,
                          multi_set->counter_size, 0, "multi_set ref_counter.");

    if (*last_appear)
    {

        SHR_IF_ERR_EXIT(sw_state_hash_table_entry_remove_by_index(unit, module_id, multi_set->hash_table, _data_indx));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_member_lookup(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    const void *key,
    int *data_indx)
{
    uint8 tmp_cnt[sizeof(uint32)];
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    /*
     * Init local variables
     */
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));
    found = 0;

    SHR_NULL_CHECK(key, _SHR_E_INTERNAL, "key");
    SHR_NULL_CHECK(data_indx, _SHR_E_INTERNAL, "data_indx");

    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup
                    (unit, module_id, multi_set->hash_table, (MULTI_SET_KEY *) key, NULL, (uint32 *) data_indx,
                     &found));

    if (!found)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        goto exit;
    }

    *data_indx = *data_indx + multi_set->init_info.first_profile;

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_get_next(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    MULTI_SET_ITER * iter,
    MULTI_SET_KEY * key,
    uint32 *data_indx,
    uint32 *ref_count)
{
    uint32 tmp_ref_count;
    uint8 tmp_cnt[sizeof(uint32)];

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    /*
     * Init local variables
     */
    tmp_ref_count = 0;
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));

    /*
     * traverse the mutli-set and print
     */

    if (MULTI_SET_ITER_END(*iter))
    {
        goto exit;
    }

    SHR_IF_ERR_EXIT(sw_state_hash_table_get_next(unit, module_id, multi_set->hash_table, iter, key, NULL, data_indx));

    if (SW_STATE_HASH_TABLE_ITER_IS_END(iter))
    {
        goto exit;
    }

    tmp_ref_count = 0;

    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * '(*data_indx) * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, (*data_indx) * multi_set->counter_size,
                         multi_set->counter_size, 0, "ref counter");

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &tmp_ref_count));

    *ref_count = tmp_ref_count;
exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_get_by_index(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    int data_indx,
    int *ref_count,
    void *key)
{
    uint32 tmp_ref_count;
    uint8 found;
    uint8 tmp_cnt[sizeof(uint32)];
    uint32 _data_indx;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    /*
     * Init local variables
     */
    tmp_ref_count = 0;
    found = 0;
    _data_indx = 0;
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));

    /*
     * calibrate the Data ID because the multiset starts from
     * ID 0 but the IDs the caller gives can start
     * from "first_profile" given in the init_info
     */
    if (data_indx >= multi_set->init_info.first_profile)
    {
        _data_indx = (uint32) data_indx - multi_set->init_info.first_profile;
    }
    else
    {
        _data_indx = multi_set->init_info.first_profile;
    }

    SHR_IF_ERR_EXIT(sw_state_hash_table_get_by_index
                    (unit, module_id, multi_set->hash_table, _data_indx, (MULTI_SET_KEY *) key, &found));

    if (!found)
    {
        if (ref_count != NULL)
        {
            *ref_count = 0;
        }
        goto exit;
    }

    tmp_ref_count = 0;
    /*
     * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
     * '(*data_indx) * counter_size' into 'tmp_cnt' buffer.
     */
    DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, _data_indx * multi_set->counter_size,
                         multi_set->counter_size, 0, "ref counter");

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &tmp_ref_count));

    if (ref_count != NULL)
    {
        *ref_count = tmp_ref_count;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_clear(
    int unit,
    uint32 module_id,
    multi_set_t multi_set)
{
    uint32 counter_size;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());

    /*
     * Init local variables
     */
    counter_size = 0;

    /*
     * clear mutli-set.
     */
    SHR_IF_ERR_EXIT(sw_state_hash_table_clear(unit, module_id, multi_set->hash_table));

    /*
     * calculate the size of pointers (list head and next) according to table size.
     */
    counter_size =
        (utilex_log2_round_up(multi_set->init_info.max_duplications + 1) +
         (UTILEX_NOF_BITS_IN_BYTE - 1)) / UTILEX_NOF_BITS_IN_BYTE;
    DNX_SW_STATE_MEMSET(unit, module_id, multi_set->ref_counter, 0, 0x00,
                        multi_set->init_info.nof_members * counter_size, 0, "");

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See description in .h file.
 */
shr_error_e
multi_set_print(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    uint8 clear_on_print)
{
    uint32 data_indx;
    uint32 print_header;
    uint32 iter;
    uint8 key[100];
    uint32 ref_count, org_ref_count;
    uint8 tmp_cnt[sizeof(uint32)];
    uint8 exist;

    SHR_FUNC_INIT_VARS(unit);
    MULTI_SET_VERIFY_UNIT_IS_LEGAL(unit);
    MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(multi_set);

    if (SHR_GET_CURRENT_ERR())
    {
        SHR_EXIT();
    }

    /*
     * Init local variables
     */
    data_indx = 0;
    ref_count = 0;
    org_ref_count = 0;
    sal_memset(tmp_cnt, 0, sizeof(uint8) * sizeof(uint32));
    sal_memset(key, 0, sizeof(uint8) * 100);
    print_header = 1;

    /*
     * traverse the mutli-set and print
     */
    LOG_CLI((BSL_META_U(unit, "\r\n")));
    SW_STATE_HASH_TABLE_ITER_SET_BEGIN(&iter);

    SHR_IF_ERR_EXIT(sw_state_hash_table_entry_lookup
                    (unit, module_id, multi_set->hash_table, key, NULL, &data_indx, &exist));
    if (!exist)
    {
        DNX_SW_STATE_PRINT(unit, "Nothing to print\r\n");
        SHR_EXIT();
    }
    while (!SW_STATE_HASH_TABLE_ITER_IS_END(&iter))
    {
        SHR_IF_ERR_EXIT(sw_state_hash_table_get_next
                        (unit, module_id, multi_set->hash_table, &iter, key, NULL, &data_indx));

        if (SW_STATE_HASH_TABLE_ITER_IS_END(&iter))
        {
            goto exit;
        }
        org_ref_count = 1;

        ref_count = 0;
        /*
         * Copy 'counter_size' bytes from 'multi_set->ref_counter' buffer at offset
         * '(*data_indx) * counter_size' into 'tmp_cnt' buffer.
         */
        DNX_SW_STATE_MEMREAD(unit, tmp_cnt, multi_set->ref_counter, data_indx * multi_set->counter_size,
                             multi_set->counter_size, 0, "ref counter");

        SHR_IF_ERR_EXIT(utilex_U8_to_U32(tmp_cnt, multi_set->counter_size, &ref_count));

        org_ref_count = ref_count;
        if (clear_on_print)
        {
            ref_count = 0;
            SHR_IF_ERR_EXIT(utilex_U32_to_U8(&ref_count, multi_set->counter_size, tmp_cnt));

            /*
             * Copy 'counter_size' bytes from 'tmp_cnt' buffer into 'multi_set->ref_counter' buffer at offset
             * '(*data_indx) * counter_size .
             */
            DNX_SW_STATE_MEMWRITE(unit, module_id, tmp_cnt, multi_set->ref_counter,
                                  data_indx * multi_set->counter_size, multi_set->counter_size, 0,
                                  "multi_set ref_counter.");
        }

        if (org_ref_count)
        {
            uint32 member_index;

            if (print_header)
            {
                DNX_SW_STATE_PRINT(unit, "| %-8s|", "Index");
                DNX_SW_STATE_PRINT(unit, "  ");
                for (member_index = 0; member_index < multi_set->init_info.member_size; member_index++)
                {
                    DNX_SW_STATE_PRINT(unit, "%2s", "  ");
                }
                DNX_SW_STATE_PRINT(unit, "| %-8s|\n\r", "ref_cnt");
                print_header = 0;
            }
            DNX_SW_STATE_PRINT(unit, "| %-8u|", data_indx);
            DNX_SW_STATE_PRINT(unit, "0x");
            for (member_index = 0; member_index < multi_set->init_info.member_size; member_index++)
            {
                DNX_SW_STATE_PRINT(unit, "%02x", key[multi_set->init_info.member_size - member_index - 1]);
            }
            DNX_SW_STATE_PRINT(unit, "| %-8u|\n\r", org_ref_count);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * } 
 */
/*
 * FUNCTIONS 
 */
/*
 * } 
 */
#else
/*
 * {
 */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
multi_set_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/*
 * }
 */
#endif /* BCM_DNX_SUPPORT */
