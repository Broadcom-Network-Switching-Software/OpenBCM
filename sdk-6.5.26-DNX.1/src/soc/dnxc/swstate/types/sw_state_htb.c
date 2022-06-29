
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_HASHDNX

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_htb.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>

#define SW_STATE_HTB_RH_PSL_SIZE_IN_BITS               8


static uint32
sw_state_htb_simple_large_hash(
    int unit,
    sw_state_htb_t htb,
    SHR_BITDCL *const key)
{
    uint32 tmp = 5381;

    tmp = ((tmp << 5) + tmp) ^ key[0];

    return tmp % htb->max_nof_elements;

}


static uint32
sw_state_htb_simple_small_hash(
    int unit,
    sw_state_htb_t htb,
    SHR_BITDCL *const key)
{
    uint32 hash = 0xAAAAAAAA;

    hash ^= (~((hash << 11) + ((key[0]) ^ (hash >> 5))));

    return hash % htb->max_nof_elements;
}

const sw_state_htb_hash_algorithm_cb_t htb_callbacks_hashing[SWSTATE_HTB_HASH_FUNCS_COUNT] = {

    {"SWSTATE_HTB_DEFAULT_HASH",
     sw_state_htb_simple_large_hash},
    {"SWSTATE_HTB_SMALL_HASH",
     sw_state_htb_simple_small_hash},
};

static shr_error_e
sw_state_htb_write_metadata(
    int unit,
    sw_state_htb_t htb,
    void *const key,
    void *const data,
    SHR_BITDCL *psl,
    uint32 offset)
{
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

    
    DNX_SW_STATE_BIT_SET(unit, htb->node_id, 0, htb->metadata, offset);
    
    DNX_SW_STATE_BIT_RANGE_WRITE(unit, htb->node_id, 0, htb->metadata, offset + htb->key_offset, 0, htb->key_size, key);
    
    DNX_SW_STATE_BIT_RANGE_WRITE(unit, htb->node_id, 0, htb->metadata, offset + htb->data_offset, 0, htb->data_size, data);
    
    DNX_SW_STATE_BIT_RANGE_WRITE(unit, htb->node_id, 0, htb->metadata, offset + htb->psl_offset, 0, htb->psl_size, psl);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_create(
    int unit,
    uint32 node_id,
    sw_state_htb_t *htb,
    sw_state_htb_create_info_t * create_info,
    uint32 alloc_flags)
{

    SHR_FUNC_INIT_VARS(unit);

    if (*htb != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state htb create ERROR: htb already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    DNX_SW_STATE_ALLOC(unit, node_id, (*htb), **htb,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state hash table");

    (*htb)->node_id = node_id;
    (*htb)->max_allowed_nof_elements = create_info->max_nof_elements;
    (*htb)->load_factor = create_info->load_factor;
    (*htb)->key_size = create_info->key_size_in_bits;
    (*htb)->data_size = create_info->data_size_in_bits;
    (*htb)->psl_size = SW_STATE_HTB_RH_PSL_SIZE_IN_BITS;
    (*htb)->nof_used_elements = 0;
    (*htb)->hash_func = create_info->hash_func;
    (*htb)->key_offset = 1;
    (*htb)->data_offset = (*htb)->key_size + (*htb)->key_offset;
    (*htb)->psl_offset = (*htb)->data_size + (*htb)->data_offset;
    
    (*htb)->metadata_size = (*htb)->key_size + (*htb)->data_size + (*htb)->psl_size + 1;
    (*htb)->flags = create_info->flags;


    if((*htb)->load_factor == 0)
    {
        (*htb)->load_factor = 80;
    }
    if (!((*htb)->load_factor >= 80 && (*htb)->load_factor <= 100))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - Load Factor. Must be between 80-100 or 0 per default.\n");
    }
    (*htb)->max_nof_elements = 1 + (100 * create_info->max_nof_elements) / (*htb)->load_factor;


    
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->metadata, ((*htb)->max_nof_elements * (*htb)->metadata_size),
        NULL, 0, alloc_flags, "sw_state htb metadata");
    
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.key, (*htb)->key_size,
        NULL, 0, alloc_flags, "sw_state htb sandbox key");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.key_compare, (*htb)->key_size,
        NULL, 0, alloc_flags, "sw_state htb sandbox key compare");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.key_swap, (*htb)->key_size,
        NULL, 0, alloc_flags, "sw_state htb sandbox key swap");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.encoded_key, (*htb)->key_size,
        NULL, 0, alloc_flags, "sw_state htb sandbox encoded key");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.data, (*htb)->data_size,
        NULL, 0, alloc_flags, "sw_state htb sandbox data");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.data_swap, (*htb)->data_size,
        NULL, 0, alloc_flags, "sw_state htb sandbox data swap");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.psl, (*htb)->psl_size,
    NULL, 0, alloc_flags, "sw_state htb sandbox psl");
    DNX_SW_STATE_ALLOC_BITMAP(unit, (*htb)->node_id, (*htb)->sandbox.curr_psl, (*htb)->psl_size,
    NULL, 0, alloc_flags, "sw_state htb sandbox curr psl");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_insert(
    int unit,
    sw_state_htb_t htb,
    uint32 table_idx,
    void *const key,
    void *const data)
{
    uint32 hash_index, entry_offset;
    uint8 flag_swap = FALSE;
    uint8 active, not_found;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    if(htb->nof_used_elements == htb->max_allowed_nof_elements)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Table full\n");
    }

    hash_index = htb_callbacks_hashing[htb->hash_func].hash_cb(unit, htb, key);

    entry_offset = htb->metadata_size * hash_index;
    while(1)
    {
        DNX_SW_STATE_BIT_GET(unit, htb->node_id, 0, htb->metadata, entry_offset, &active);
        if(active == 0)
        {
            if(flag_swap)
            {
                SHR_IF_ERR_EXIT(sw_state_htb_write_metadata(unit, htb, htb->sandbox.key, htb->sandbox.data, htb->sandbox.curr_psl, entry_offset));
            }
            else
            {
                SHR_IF_ERR_EXIT(sw_state_htb_write_metadata(unit, htb, key, data, htb->sandbox.curr_psl, entry_offset));
                htb->nof_used_elements++;
            }
            break;
        }
        DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->key_offset, 0, htb->key_size, htb->sandbox.key_compare);
        not_found = (uint8) sal_memcmp(htb->sandbox.key_compare, key, SHR_BITALLOCSIZE(htb->key_size));
        if(not_found == FALSE)
        {
            
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Key already exists in the table\n");
        }
        DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->psl_offset, 0, htb->psl_size, htb->sandbox.psl);
        if ((*htb->sandbox.curr_psl) <= (*htb->sandbox.psl))
        {
            entry_offset = entry_offset + htb->metadata_size;
            (*htb->sandbox.curr_psl)++;
            if((*htb->sandbox.curr_psl) >= (1 << SW_STATE_HTB_RH_PSL_SIZE_IN_BITS))
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "OVERFLOW - PSL value (%d) is greater than max allowed value by the type.\n", (*htb->sandbox.curr_psl));
            }
            hash_index++;
            if(hash_index == htb->max_nof_elements)
            {
                hash_index = 0;
                entry_offset = 0;
            }
            continue;
        }
        
        
        if(flag_swap)
        {
            
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->key_offset, 0, htb->key_size, htb->sandbox.key_swap);
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->data_offset, 0, htb->data_size, htb->sandbox.data_swap);
            
            SHR_IF_ERR_EXIT(sw_state_htb_write_metadata(unit, htb, htb->sandbox.key, htb->sandbox.data, htb->sandbox.curr_psl, entry_offset));

            
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->sandbox.key_swap, 0, 0, htb->key_size, htb->sandbox.key);
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->sandbox.data_swap, 0, 0, htb->data_size, htb->sandbox.data);
        }
        else
        {
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->key_offset, 0, htb->key_size, htb->sandbox.key);
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->data_offset, 0, htb->data_size, htb->sandbox.data);
            
            SHR_IF_ERR_EXIT(sw_state_htb_write_metadata(unit, htb, key, data, htb->sandbox.curr_psl, entry_offset));
            htb->nof_used_elements++;
            flag_swap = TRUE;
        }
        
        (*htb->sandbox.curr_psl) = ++(*htb->sandbox.psl);
        entry_offset = entry_offset + htb->metadata_size;
        hash_index++;
        if(hash_index == htb->max_nof_elements)
        {
            hash_index = 0;
            entry_offset = 0;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_find(
    int unit,
    sw_state_htb_t htb,
    uint32 table_idx,
    void *const key,
    void *data,
    uint32 *offset)
{
    uint32 hash_index, entry_offset;
    uint8 active;
    uint8 not_found;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    hash_index = htb_callbacks_hashing[htb->hash_func].hash_cb(unit, htb, key);

    entry_offset = htb->metadata_size * hash_index;
    while(1)
    {
        DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->psl_offset, 0, htb->psl_size, htb->sandbox.psl);
        DNX_SW_STATE_BIT_GET(unit, htb->node_id, 0, htb->metadata, entry_offset, &active);
        if(active == 0 || (*htb->sandbox.curr_psl) > (*htb->sandbox.psl))
        {
            
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Key not found\n");
        }
        DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->key_offset, 0, htb->key_size, htb->sandbox.key);
        not_found = (uint8) sal_memcmp(htb->sandbox.key, key, SHR_BITALLOCSIZE(htb->key_size));
        if(not_found == FALSE)
        {
            
            if(offset != NULL)
            {
                *offset = entry_offset;
                goto exit;
            }
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->data_offset, 0, htb->data_size, data);
            goto exit;
        }
        entry_offset = entry_offset + htb->metadata_size;
        (*htb->sandbox.curr_psl)++;
        hash_index++;
        if(hash_index == htb->max_nof_elements)
        {
            hash_index = 0;
            entry_offset = 0;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_delete(
    int unit,
    sw_state_htb_t htb,
    uint32 table_idx,
    void *const key)
{
    uint32 entry_offset, entry_offset_shift, hash_index;
    uint8 active;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    SHR_IF_ERR_EXIT(sw_state_htb_rh_find(unit, htb, 0, key, NULL, &entry_offset));
    entry_offset_shift = entry_offset + htb->metadata_size;
    hash_index = entry_offset / htb->metadata_size;
    
    while(1){
        if(hash_index == (htb->max_nof_elements - 1))
        {
            entry_offset_shift = 0;
            hash_index = 0;
        }
        DNX_SW_STATE_BIT_GET(unit, htb->node_id, 0, htb->metadata, entry_offset_shift, &active);
        DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset_shift + htb->psl_offset, 0, htb->psl_size, htb->sandbox.psl);
        if(active == 0 || (*htb->sandbox.psl) == 0)
        {
            
            DNX_SW_STATE_BIT_CLEAR(unit, htb->node_id, 0, htb->metadata, entry_offset);
            htb->nof_used_elements--;
            goto exit;
        }
        (*htb->sandbox.psl)--;
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, htb->node_id, 0, htb->metadata, entry_offset, entry_offset_shift , htb->metadata_size - htb->psl_size, htb->metadata);
        DNX_SW_STATE_BIT_RANGE_WRITE(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->psl_offset, 0, htb->psl_size, htb->sandbox.psl);
        hash_index++;
        entry_offset = entry_offset_shift;
        entry_offset_shift = entry_offset_shift + htb->metadata_size;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_replace(
    int unit,
    sw_state_htb_t htb,
    uint32 table_idx,
    void *const key,
    void *const data)
{
    uint32 entry_offset;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    
    SHR_IF_ERR_EXIT(sw_state_htb_rh_find(unit, htb, 0, key, NULL, &entry_offset));
    
    DNX_SW_STATE_BIT_RANGE_WRITE(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->data_offset, 0, htb->data_size, data);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e sw_state_htb_traverse(
    int unit,
    sw_state_htb_t htb,
    sw_state_htb_traverse_cb traverse_func)
{
    
    uint32 iterator, entry_offset;
    uint8 active;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    entry_offset = 0;
    for(iterator = 0; iterator < htb->max_nof_elements; iterator++)
    {
        DNX_SW_STATE_BIT_GET(unit, htb->node_id, 0, htb->metadata, entry_offset, &active);
        if(active)
        {
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->key_offset, 0, htb->key_size, htb->sandbox.key);
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->data_offset, 0, htb->data_size, htb->sandbox.data);
            traverse_func(unit, htb->sandbox.key, htb->sandbox.data);
        }
        entry_offset += htb->metadata_size;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_print(
    int unit,
    sw_state_htb_t htb)
{
    uint32 iterator, entry_offset;
    uint32 print_indx = 0;
    uint32 key_size =  SHR_BITALLOCSIZE(htb->key_size) / sizeof(uint32);
    uint32 data_size =  SHR_BITALLOCSIZE(htb->data_size) / sizeof(uint32);

    
    SHR_BITDCL *key_buff;
    SHR_BITDCL *data_buff;
    uint8 active;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    DNX_SW_STATE_PRINT(unit, "\nKey size: %u Bits\n", htb->key_size);
    DNX_SW_STATE_PRINT(unit, "Data size: %u Bits\n", htb->data_size);
    DNX_SW_STATE_PRINT(unit, "Active entries: %u\n", htb->nof_used_elements);
    DNX_SW_STATE_PRINT(unit, "Max number of entries: %u\n", htb->max_allowed_nof_elements);
    DNX_SW_STATE_PRINT(unit, "List of all key and data pairs:\n");

    key_buff = sal_alloc((sizeof(uint32) * key_size), "key buffer");
    data_buff = sal_alloc((sizeof(uint32) * data_size), "data buffer");
    sal_memset(key_buff, 0, (sizeof(uint32) * key_size));
    sal_memset(data_buff, 0, (sizeof(uint32) * data_size));
    entry_offset = 0;
    for(iterator = 0; iterator < htb->max_nof_elements; iterator++)
    {
        DNX_SW_STATE_BIT_GET(unit, htb->node_id, 0, htb->metadata, entry_offset, &active);
        if(active)
        {
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->key_offset, 0, htb->key_size, key_buff);
            DNX_SW_STATE_BIT_RANGE_READ(unit, htb->node_id, 0, htb->metadata, entry_offset + htb->data_offset, 0, htb->data_size, data_buff);
            
            
            DNX_SW_STATE_PRINT(unit, "\nKey: 0x");
            for (print_indx = 0; print_indx < key_size; ++print_indx)
            {
                DNX_SW_STATE_PRINT(unit, "%02x", key_buff[key_size - print_indx - 1]);
            }
            
            DNX_SW_STATE_PRINT(unit, "\nData: 0x");
            for (print_indx = 0; print_indx < data_size; ++print_indx)
            {
                DNX_SW_STATE_PRINT(unit, "%02x", data_buff[data_size - print_indx - 1]);
            }
            DNX_SW_STATE_PRINT(unit, "\n");
        }
        entry_offset += htb->metadata_size;
    }
    SHR_EXIT();
exit:
    SHR_FREE(key_buff);
    SHR_FREE(data_buff);
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_delete_all(
    int unit,
    sw_state_htb_t htb)
{

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    DNX_SW_STATE_BIT_RANGE_CLEAR(unit, htb->node_id, 0, htb->metadata, 0, (htb->metadata_size * htb->max_nof_elements));

    htb->nof_used_elements = 0;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
