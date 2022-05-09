
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_HASHDNX




#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_index_hash_table.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_hash_table_access.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
















 

 
 

 

 
 
shr_error_e
sw_state_index_hash_table_info_clear(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table)
{

    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    DNX_SW_STATE_MEMSET(unit, node_id, &hash_table->init_info, 0, 0x0,
        sizeof(sw_state_htbl_init_info_t), 0, "sw_state_hashtable info clear");

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}







static shr_error_e
sw_state_index_hash_table_simple_hash(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    uint32 seed,
    uint32 *hash_val)
{
    uint32 expected_nof_elements, key_size;
    uint32 hash = 0xAAAAAAAA;
    uint32 indx = 0;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(hash_val, _SHR_E_PARAM, "hash_val");
    key_size = hash_table->key_size;
    expected_nof_elements = hash_table->init_info.expected_nof_elements;
    for (indx = 0; indx < key_size; ++indx)
    {
        hash ^= ((indx & 1) == 0) ? ( (hash << 7) ^ (key[indx]) * (hash >> 3)) : (~((hash << 11) + ((key[indx]) ^ (hash >> 5))));
    }

    *hash_val = hash % expected_nof_elements;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e sw_state_index_hash_table_find_entry(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    uint8 first_empty,
    uint8 alloc_by_index,
    uint32 *entry,
    uint8 *found,
    uint32 *prev,
    uint8 *first);






shr_error_e
sw_state_index_hash_table_create(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t* hash_table,
    sw_state_idx_htbl_init_info_t * init_info,
    uint32 key_size,
    uint32 data_size,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    uint32 max_nof_elements, expected_nof_elements, ptr_size_in_bits, null_ptr;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);

    if (*hash_table != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state htb create ERROR: htb already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    DNX_SW_STATE_ALLOC(unit, node_id, (*hash_table), **hash_table,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state hash table");

    sw_state_index_hash_table_info_clear(unit, node_id, *hash_table);

    DNX_SW_STATE_MEMCPY(unit, node_id, (*hash_table)->init_info, init_info, sw_state_htbl_init_info_t, 0, "swstate hashtable create");

    max_nof_elements = init_info->max_nof_elements;
    expected_nof_elements = init_info->expected_nof_elements;

    
    if (nof_elements != 0)
    {
        max_nof_elements = nof_elements;
    }

    if (max_nof_elements == 0 || expected_nof_elements == 0 || key_size == 0 || data_size == 0)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "sw state htb create ERROR: htb already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }
    
    ptr_size_in_bits = utilex_log2_round_up(max_nof_elements + 1);
    
    null_ptr = UTILEX_BITS_MASK((ptr_size_in_bits - 1), 0);
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            node_id,
            &(*hash_table)->ptr_size,
            &ptr_size_in_bits,
            sizeof(ptr_size_in_bits),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->ptr_size_in_bits");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            node_id,
            &(*hash_table)->null_ptr,
            &null_ptr,
            sizeof(null_ptr),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->null_ptr");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            node_id,
            &(*hash_table)->key_size,
            &key_size,
            sizeof(key_size),
            DNXC_SW_STATE_NO_FLAGS,
            "(*hash_table)->key_size");

    if (init_info->print_cb_name != NULL) {
        sw_state_string_strncpy(
            unit,
            node_id,
            (*hash_table)->print_cb_name,
            sizeof(char)*(SW_STATE_CB_DB_NAME_STR_SIZE-1),
            init_info->print_cb_name);
    }

    
    DNX_SW_STATE_ALLOC(unit, node_id, (*hash_table)->keys, uint8, max_nof_elements * key_size,
            alloc_flags, "sw_state hash table keys");


    
    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*hash_table)->next, (max_nof_elements * ptr_size_in_bits),
        NULL, 0, alloc_flags, "sw_state hash table next");
    DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET((*hash_table)->next, max_nof_elements * ptr_size_in_bits);

    
    DNX_SW_STATE_ALLOC_BITMAP(unit, node_id, (*hash_table)->lists_head, max_nof_elements * ptr_size_in_bits,
        NULL, 0, alloc_flags, "sw_state hash table lists head");

    DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET((*hash_table)->lists_head, max_nof_elements * ptr_size_in_bits);

    sw_state_occ_bm_init_info_clear(&btmp_init_info);
    btmp_init_info.size = max_nof_elements;
    
    sw_state_occ_bm_create(unit, node_id, &btmp_init_info, &((*hash_table)->memory_use), nof_elements, alloc_flags);
    if (init_info->print_cb_name != NULL)
    {
        SW_STATE_CB_DB_REGISTER_CB(node_id,
                (*hash_table)->print_cb_db,
                init_info->print_cb_name,
                dnx_sw_state_hash_table_print_cb_get_cb);
    }
exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_destroy(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t *hash_table_ptr)
{
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE((*hash_table_ptr));

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    DNX_SW_STATE_FREE(unit, node_id, (*hash_table_ptr)->lists_head, "sw state hashtable lists_head");
    DNX_SW_STATE_FREE(unit, node_id, (*hash_table_ptr)->next, "sw state hashtable next");
    DNX_SW_STATE_FREE(unit, node_id, (*hash_table_ptr)->keys, "sw state hashtable keys");
    DNX_SW_STATE_FREE(unit, node_id, (*hash_table_ptr), "sw state hashtable");

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_entry_add(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    SW_STATE_INDEX_HASH_TABLE_DATA * const data,
    uint32 *data_indx,
    uint8 *success)
{
    uint8 found, first;
    uint32 key_size, entry_offset = 0, prev_entry;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    *data_indx = SW_STATE_INDEX_HASH_TABLE_NULL;
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(data_indx, _SHR_E_PARAM, "data_indx");
    SHR_NULL_CHECK(success, _SHR_E_PARAM, "success");

    
#ifdef BCM_DNX_SUPPORT
#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_idx_htbl(unit, node_id, hash_table, key));
#endif 
#endif

    
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_find_entry
                    (unit, node_id, hash_table, key, TRUE, FALSE, &entry_offset, &found, &prev_entry, &first));

    if (entry_offset == SW_STATE_INDEX_HASH_TABLE_NULL)
    {
        *success = FALSE;
        *data_indx = SW_STATE_INDEX_HASH_TABLE_NULL;
        goto exit;
    }
    key_size = hash_table->key_size;

    
    DNX_SW_STATE_MEMWRITE(unit, node_id, key, hash_table->keys, key_size * entry_offset, key_size, 0, "swstate hashtable add key");

    *success = TRUE;
    *data_indx = entry_offset;

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 " Exit: Key[0,1] %d%d data_indx %d success %d\r\n",
                 (int) key[0], (int) key[1], (int) (entry_offset), (int) (*success));
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_entry_add_at_index(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    uint32 data_indx,
    uint8 *success)
{
    uint8 found, indx_in_use, first;
    uint32 entry_offset, prev_entry;
    uint32 old_index;
    uint32 key_size;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(success, _SHR_E_PARAM, "success");

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    key_size = hash_table->key_size;
    
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_entry_lookup(unit, node_id, hash_table, key, NULL, &old_index, &found));

    if (found && old_index != data_indx)
    {
        *success = FALSE;
        SHR_ERR_EXIT(_SHR_E_EXISTS, "The entry %d has already been added.\r\n", old_index);
    }
    if (found && old_index == data_indx)
    {
        
        *success = TRUE;
        goto exit;
    }
    SHR_IF_ERR_EXIT(sw_state_occ_bm_is_occupied(unit, node_id, hash_table->memory_use, data_indx, &indx_in_use));

    if (indx_in_use && !found)
    {
        
        *success = FALSE;
        goto exit;
    }

    
#ifdef BCM_DNX_SUPPORT
#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_idx_htbl(unit, node_id, hash_table, key));
#endif 
#endif

    entry_offset = data_indx;
    
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_find_entry
                    (unit, node_id, hash_table, key, TRUE, TRUE, &entry_offset, &found, &prev_entry, &first));
    if (entry_offset == SW_STATE_INDEX_HASH_TABLE_NULL)
    {
        *success = FALSE;
        goto exit;
    }
    
    DNX_SW_STATE_MEMWRITE(unit, node_id, key, hash_table->keys, key_size * entry_offset, key_size, 0, "swstate hashtable add key");

    *success = TRUE;
exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_entry_remove(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key)
{
    uint8 found, first;
    uint32 entry_offset, prev_entry;
    uint32 null_ptr[1], ptr_size;
    uint32 key_size = 0;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_find_entry
                    (unit, node_id, hash_table, key, FALSE, FALSE, &entry_offset, &found, &prev_entry, &first));
    
    if (!found)
    {
        goto exit;
    }

    
#ifdef BCM_DNX_SUPPORT
#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_idx_htbl(unit, node_id, hash_table, key));
#endif 
#endif

    
    
    ptr_size = hash_table->ptr_size;

    
    if (first)
    {
        

        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, hash_table->lists_head, ptr_size * prev_entry,
                                     ptr_size * entry_offset, ptr_size ,hash_table->next);
    }
    else
    {
        
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, hash_table->next, ptr_size * prev_entry,
                                     ptr_size * entry_offset, ptr_size, hash_table->next);
    }

    key_size = hash_table->key_size;

    
    DNX_SW_STATE_MEMSET(unit, node_id, hash_table->keys, key_size * entry_offset, 0x0, key_size, 0, "swstate hashtable remove key");


    
    sw_state_occ_bm_occup_status_set(unit, node_id, hash_table->memory_use, entry_offset, FALSE);
    null_ptr[0] = hash_table->null_ptr;

    
    DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, hash_table->next, entry_offset * ptr_size, 0, ptr_size, null_ptr);
exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_entry_remove_by_index(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    uint32 data_indx)
{
    uint8 *cur_key;
    uint8 in_use;
    uint32 key_size, max_nof_elements;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    max_nof_elements = hash_table->init_info.max_nof_elements;
    SHR_MAX_VERIFY(data_indx, max_nof_elements - 1, _SHR_E_PARAM,
        "data_index is greater than or equals to the number of elements in the hashtable");
    key_size = hash_table->key_size;
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_is_occupied(unit, node_id, hash_table->memory_use, data_indx, &in_use));
    if (!in_use)
    {
        
        goto exit;
    }

    cur_key = (uint8*)(hash_table->keys) + key_size * data_indx;
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_entry_remove(unit, node_id, hash_table, cur_key));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_entry_remove_all(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table)
{
    uint8 *cur_key = NULL;
    uint32 iter = 0;
    uint8 *data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);
    cur_key = sal_alloc(hash_table->key_size, "key buffer");
    data = sal_alloc(sizeof(uint32), "data buffer");
    do {
        
        SHR_IF_ERR_EXIT(sw_state_index_hash_table_get_next(unit, node_id, hash_table, &iter, (SW_STATE_INDEX_HASH_TABLE_KEY*) cur_key, NULL, (uint32*)data));
        if (iter == UTILEX_U32_MAX)
        {
            SHR_EXIT();
        }
        
        SHR_IF_ERR_EXIT(sw_state_index_hash_table_entry_remove(unit, node_id, hash_table, (SW_STATE_INDEX_HASH_TABLE_KEY*) cur_key));
    } while(iter != UTILEX_U32_MAX);

exit:
    SHR_FREE(cur_key);
    SHR_FREE(data);
    SHR_FUNC_EXIT;
}



static shr_error_e
sw_state_index_hash_table_find_entry(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    uint8 get_first_empty,
    uint8 alloc_by_index,
    uint32 *entry,
    uint8 *found,
    uint32 *prev_entry,
    uint8 *first)
{
    uint8 *cur_key, *next = NULL;
    uint32 key_size, ptr_size, null_ptr, next_node;
    SHR_BITDCL ptr_long[128], tmp_buf[1];
    uint32 hash_val = 0;
    uint8 not_found, found_new;
    SHR_BITDCL *tmp_shr_bitdcl_buf = NULL;
    uint8 *tmp_uint8_buf = NULL;
    uint32 tmp_buf_size = 0;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    SHR_NULL_CHECK(hash_table, _SHR_E_PARAM, "hash_table");
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(found, _SHR_E_PARAM, "found");

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    ptr_size = hash_table->ptr_size;
    key_size = hash_table->key_size;
    null_ptr = hash_table->null_ptr;

    *ptr_long = 0;
    *tmp_buf = 0;
    *prev_entry = SW_STATE_INDEX_HASH_TABLE_NULL;
    *first = TRUE;

    
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_simple_hash(unit, node_id, hash_table, key, 0, &hash_val));
    *prev_entry = hash_val;

    tmp_buf_size = (key_size > sizeof(uint32)) ? key_size : sizeof(uint32);

    tmp_shr_bitdcl_buf = sal_alloc(tmp_buf_size, "alloc temp buffer for sw_state hash table");
    tmp_uint8_buf = sal_alloc(tmp_buf_size, "alloc temp buffer for sw_state hash table");;
    next = sal_alloc(tmp_buf_size, "alloc current key for sw_state hash table");

    sal_memset(next, 0, tmp_buf_size);

    if (tmp_shr_bitdcl_buf == NULL || tmp_uint8_buf == NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY,
                                 "temp buff allocation for sw_state hash table failed! %s%s%s\n",
                                 EMPTY, EMPTY, EMPTY);
    }
    sal_memset(tmp_shr_bitdcl_buf, 0, tmp_buf_size);
    sal_memset(tmp_uint8_buf, 0, tmp_buf_size);

    
    *ptr_long = 0;
    DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, hash_table->lists_head, ptr_size * hash_val, 0, ptr_size, ptr_long);

    
    if (*ptr_long == null_ptr)
    {
        if (get_first_empty)
        {
            if (alloc_by_index)
            {
                
                sw_state_occ_bm_occup_status_set(unit, node_id, hash_table->memory_use, *entry, TRUE);

                found_new = TRUE;

                *ptr_long = *entry;
            }
            else
            {
                SHR_IF_ERR_EXIT(sw_state_occ_bm_alloc_next(unit, node_id, hash_table->memory_use, ptr_long, &found_new));
            }
            if (!found_new)
            {
                *ptr_long = SW_STATE_INDEX_HASH_TABLE_NULL;
                *entry = SW_STATE_INDEX_HASH_TABLE_NULL;
            }
            else
            {

                
                DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, hash_table->lists_head, ptr_size * hash_val, 0, ptr_size, ptr_long);
                *entry = *ptr_long;
            }
            *found = FALSE;
            goto exit;
        }
        *found = FALSE;
        *entry = SW_STATE_INDEX_HASH_TABLE_NULL;
        goto exit;
    }
    not_found = TRUE;

    while (*ptr_long != null_ptr)
    {
        sal_memset(tmp_uint8_buf, 0, tmp_buf_size);
        
        
        DNX_SW_STATE_MEMREAD(unit, tmp_uint8_buf, hash_table->keys, key_size * (*ptr_long), key_size, 0, "swstate hashtable find");
        cur_key = tmp_uint8_buf;
        not_found = (uint8) sal_memcmp(cur_key, key, (key_size * sizeof(SW_STATE_INDEX_HASH_TABLE_KEY)));
        if (not_found == FALSE)
        {
            
            *found = TRUE;
            *entry = *ptr_long;
            goto exit;
        }
        
        *first = FALSE;
        
        
        sal_memset(tmp_shr_bitdcl_buf, 0, tmp_buf_size);
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, hash_table->next, ptr_size * (*ptr_long), 0, ptr_size, (uint32*)tmp_shr_bitdcl_buf);
        SHR_IF_ERR_EXIT(utilex_U32_to_U8(tmp_shr_bitdcl_buf, tmp_buf_size, next));
        *prev_entry = *ptr_long;
        *ptr_long = 0;
        SHR_IF_ERR_EXIT(utilex_U8_to_U32(next, UTILEX_DIV_ROUND_UP(ptr_size, UTILEX_NOF_BITS_IN_BYTE), ptr_long));
    }
    
    *found = FALSE;
    if (get_first_empty)
    {
        if (alloc_by_index)
        {
            

            SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, node_id, hash_table->memory_use, *entry, TRUE));
            found_new = TRUE;
            next_node = *entry;
        }
        else
        {
            SHR_IF_ERR_EXIT(sw_state_occ_bm_alloc_next(unit, node_id, hash_table->memory_use, &next_node, &found_new));
        }
        if (!found_new)
        {
            *entry = SW_STATE_INDEX_HASH_TABLE_NULL;
        }
        else
        {
            
            ptr_long[0] = next_node;

            sal_memset(tmp_shr_bitdcl_buf, 0, tmp_buf_size);
            sal_memcpy(tmp_shr_bitdcl_buf, &(ptr_long[0]), sizeof(ptr_long[0]));
            
            DNX_SW_STATE_BIT_RANGE_WRITE(unit, node_id, 0, hash_table->next,
                ptr_size*(*prev_entry), 0, ptr_size, tmp_shr_bitdcl_buf);

            *entry = next_node;
        }
    }
    else
    {
        *entry = SW_STATE_INDEX_HASH_TABLE_NULL;
    }

exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    if (tmp_shr_bitdcl_buf != NULL) {
        sal_free(tmp_shr_bitdcl_buf);
    }
    if (tmp_uint8_buf != NULL) {
        sal_free(tmp_uint8_buf);
    }
    if (next != NULL) {
        sal_free(next);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_entry_lookup(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    SW_STATE_INDEX_HASH_TABLE_DATA * data,
    uint32 *data_indx,
    uint8 *found)
{
    uint8 is_found, first;
    uint32 entry_offset, prev_entry;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(data_indx, _SHR_E_PARAM, "data_indx");
    SHR_NULL_CHECK(found, _SHR_E_PARAM, "found");

    
    SHR_IF_ERR_EXIT(sw_state_index_hash_table_find_entry(unit,
                                                 node_id, hash_table, key, FALSE, FALSE, &entry_offset, &is_found, &prev_entry,
                                                 &first));

    
    if (!is_found)
    {
        *found = FALSE;
        *data_indx = SW_STATE_INDEX_HASH_TABLE_NULL;
        goto exit;
    }
    *found = TRUE;
    *data_indx = entry_offset;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_get_by_index(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    uint32 data_indx,
    SW_STATE_INDEX_HASH_TABLE_KEY * key,
    uint8 *found)
{
    uint32 max_nof_elements, key_size;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    max_nof_elements = hash_table->init_info.max_nof_elements;
    key_size = hash_table->key_size;
    if (data_indx > max_nof_elements)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
        SHR_EXIT();
    }
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_is_occupied(unit, node_id, hash_table->memory_use, data_indx, found));
    
    if (!*found)
    {
        goto exit;
    }
    
    if (key != NULL)
    {
        DNX_SW_STATE_MEMREAD(unit, key, hash_table->keys, key_size * data_indx, key_size, 0, "");
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_index_hash_table_clear(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table)
{
    uint32 ptr_size, max_nof_elements, key_size;
    int32 offset;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    DNXC_SW_STATE_COMPARISON_SUPPRESS(unit);

    is_suppressed = TRUE;

    max_nof_elements = hash_table->init_info.max_nof_elements;
    key_size = hash_table->key_size;
    ptr_size = hash_table->ptr_size;
    offset = 0;
    
    DNX_SW_STATE_MEMSET(unit, node_id, hash_table->keys, offset, 0x00, max_nof_elements * key_size, 0, "");

    
    DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET(hash_table->next + offset, max_nof_elements * ptr_size);
    
    DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET(hash_table->lists_head + offset, max_nof_elements * ptr_size);

    
    sw_state_occ_bm_clear(unit, node_id, hash_table->memory_use);
exit:
    if(is_suppressed)
    {
        DNXC_SW_STATE_COMPARISON_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_get_next(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    SW_STATE_INDEX_HASH_TABLE_ITER * iter,
    SW_STATE_INDEX_HASH_TABLE_KEY * const key,
    SW_STATE_INDEX_HASH_TABLE_DATA * const data,
    uint32 *data_indx)
{
    uint32 indx;
    uint32 max_nof_elements, key_size;
    uint8 occupied;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(iter, _SHR_E_PARAM, "iter");
    SHR_NULL_CHECK(data_indx, _SHR_E_PARAM, "data_indx");
    max_nof_elements = hash_table->init_info.max_nof_elements;
    key_size = hash_table->key_size;
    
    for (indx = *iter; indx < max_nof_elements; ++indx)
    {

        SHR_IF_ERR_EXIT(sw_state_occ_bm_is_occupied(unit, node_id, hash_table->memory_use, indx, &occupied));
        if (occupied)
        {
            *data_indx = indx;
            *iter = indx + 1;
            
            DNX_SW_STATE_MEMREAD(unit, key, hash_table->keys, key_size * indx, key_size, 0, "");
            goto exit;
        }
    }
    *iter = UTILEX_U32_MAX;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_htb_print(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table)
{
    uint32 indx;
    uint32 print_indx;
    uint32 ptr_long[1], ptr_long_tmp[1] = {0};
    uint8  *cur_key, *cur_data;
    uint32 expected_nof_elements, ptr_size, null_ptr, key_size;
    uint32 active_entries, max_nof_elements = 0;
    dnx_sw_state_hash_table_print_cb print_cb;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    key_size = hash_table->key_size;
    ptr_size = hash_table->ptr_size;
    expected_nof_elements = hash_table->init_info.expected_nof_elements;
    max_nof_elements = hash_table->init_info.max_nof_elements;
    null_ptr = hash_table->null_ptr;

    sw_state_index_hash_table_get_num_active(unit, node_id, hash_table, &active_entries);
    DNX_SW_STATE_PRINT(unit, "\nKey size: %u\n", key_size);
    DNX_SW_STATE_PRINT(unit, "Active entries: %u\n", active_entries);
    DNX_SW_STATE_PRINT(unit, "Max number of entries: %u\n", max_nof_elements);
    DNX_SW_STATE_PRINT(unit, "List of all key and data pairs:\n");

    
    for (indx = 0; indx < expected_nof_elements; ++indx)
    {
        
        *ptr_long = 0;
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, hash_table->lists_head, ptr_size * indx, 0, ptr_size, ptr_long);

        if (*ptr_long == null_ptr)
        {
            continue;
        }

        while (*ptr_long != null_ptr)
        {

            cur_key = (uint8*)(hash_table->keys) + key_size * ptr_long[0];
            cur_data = (uint8*)&ptr_long[0];
            if (sal_strncmp(hash_table->print_cb_name, "", SW_STATE_CB_DB_NAME_STR_SIZE-1) != 0) {
                SW_STATE_CB_DB_GET_CB(node_id,
                        hash_table->print_cb_db,
                        &print_cb,
                        dnx_sw_state_hash_table_print_cb_get_cb);

                print_cb(unit, cur_key, cur_data);

            } else {
                
                DNX_SW_STATE_PRINT(unit, "Key: 0x");
                for (print_indx = 0; print_indx < key_size; ++print_indx)
                {
                    DNX_SW_STATE_PRINT(unit, "%02x", cur_key[key_size - print_indx - 1]);
                }
            }
            DNX_SW_STATE_PRINT(unit, "\n");
            
            
            ptr_long_tmp[0] = 0;
            ptr_long_tmp[0] = ptr_long[0];
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, hash_table->next, ptr_size * ptr_long[0], 0, ptr_size, ptr_long_tmp);
            ptr_long[0] = 0;
            ptr_long[0] = ptr_long_tmp[0];

        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
sw_state_index_hash_table_size_get(
    sw_state_idx_htbl_init_info_t * init_info,
    uint32 key_size)
{
    uint32 ptr_size = 0;
    int size = 0;
    sw_state_occ_bitmap_init_info_t btmp_init_info;

    
    size += (sizeof(sw_state_idx_htbl_t));

    
    size += (sizeof(uint8) * (init_info->max_nof_elements * key_size));

    
    size += (sizeof(uint8) * (init_info->max_nof_elements * ptr_size));

    ptr_size = (utilex_log2_round_up(init_info->max_nof_elements + 1) + (UTILEX_NOF_BITS_IN_BYTE - 1)) / UTILEX_NOF_BITS_IN_BYTE;

    
    size += (sizeof(uint8) * (init_info->max_nof_elements * ptr_size));

    
    sw_state_occ_bm_init_info_clear(&btmp_init_info);
    btmp_init_info.size = init_info->max_nof_elements;

    size += sw_state_occ_bm_size_get(&btmp_init_info);

    return size;
}



shr_error_e
sw_state_index_hash_table_get_num_active(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table,
    uint32 *num_active_ptr)
{
    uint32 indx;
    uint32 expected_nof_elements, ptr_size, null_ptr;
    uint32 tmp_buf_ptr[1] = {0};
    uint32 num_active;

    SHR_FUNC_INIT_VARS(unit);
    num_active = 0;
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    ptr_size = hash_table->ptr_size;
    expected_nof_elements = hash_table->init_info.expected_nof_elements;
    null_ptr = hash_table->null_ptr;
    
    for (indx = 0; indx < expected_nof_elements; ++indx)
    {
        
        sal_memset(tmp_buf_ptr, 0, sizeof(tmp_buf_ptr[0]));
        DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, hash_table->lists_head, ptr_size * indx, 0, ptr_size, tmp_buf_ptr);
        if (*tmp_buf_ptr == null_ptr)
        {
            continue;
        }
        while (*tmp_buf_ptr != null_ptr)
        {
            num_active++;
            
            
            DNX_SW_STATE_BIT_RANGE_READ(unit, node_id, 0, hash_table->next, ptr_size * (*tmp_buf_ptr), 0, ptr_size, tmp_buf_ptr);
        }
    }
exit:
    *num_active_ptr = num_active;
    SHR_FUNC_EXIT;
}

#if SW_STATE_DEBUG


static shr_error_e
sw_state_next_and_key_table_print(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table)
{
    uint32 indx;
    uint32 ptr_long, print_indx;
    uint8 *cur_key, *next;
    uint32 expected_nof_elements, ptr_size, key_size;
    uint32 tmp_buf_ptr[1]={0};

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    key_size = hash_table->key_size;
    ptr_size = hash_table->ptr_size;
    expected_nof_elements = hash_table->init_info.expected_nof_elements;
    
    for (indx = 0; indx < expected_nof_elements; ++indx)
    {
        
        DNX_SW_STATE_MEMREAD(unit, tmp_buf_ptr, hash_table->next, ptr_size * indx, ptr_size, 0, "");
        next = tmp_buf_ptr;
        ptr_long = 0;
        SHR_IF_ERR_EXIT(utilex_U8_to_U32(next, ptr_size, &ptr_long));
        DNX_SW_STATE_PRINT(unit, " entry %2u: Pointer Value %3d ", indx, (int) ptr_long);

        cur_key = (uint8*)(hash_table->keys) + key_size * indx;
        DNX_SW_STATE_PRINT(unit, "Key 0x");
        for (print_indx = 0; print_indx < key_size; ++print_indx)
        {
            DNX_SW_STATE_PRINT(unit, "%02x", cur_key[key_size - print_indx - 1]);
        }
        DNX_SW_STATE_PRINT(unit, "\n");
    }
    DNX_SW_STATE_PRINT(unit, "\n");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_index_hash_table_info_print(
    int unit,
    uint32 node_id,
    sw_state_idx_htbl_t hash_table)
{
    uint32 key_size, max_nof_elements, expected_nof_elements;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_UNIT_IS_LEGAL(unit);
    SW_STATE_INDEX_HASH_TABLE_VERIFY_HASHTABLE_IS_ACTIVE(hash_table);

    key_size = hash_table->key_size;
    max_nof_elements = hash_table->init_info.max_nof_elements;
    expected_nof_elements = hash_table->init_info.expected_nof_elements;

    DNX_SW_STATE_PRINT(unit, "key_size   : %u\n", key_size);
    DNX_SW_STATE_PRINT(unit, "init_info.max_nof_elements : %u\n", max_nof_elements);
    DNX_SW_STATE_PRINT(unit, "init_info.expected_nof_elements: %u\n", expected_nof_elements);
exit:
    SHR_FUNC_EXIT;
}


#endif 

