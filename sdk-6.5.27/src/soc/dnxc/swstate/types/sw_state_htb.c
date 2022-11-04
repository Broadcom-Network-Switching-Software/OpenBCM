
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
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_hash_table_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>

#if (SW_STATE_LL_INVALID != 0)
#error "SW_STATE_LL_INVALID VALUE WAS CHANGED. REVIEW AND UPDATE THE CODE."
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/dnx_sw_state_journal.h>
#endif

#define SW_STATE_HTB_RH_MAX_PSL_SIZE               127
#define SW_STATE_HTB_RH_PSL_MASK_DECODE            0x7F
#define SW_STATE_HTB_RH_ACTIVE_SHIFT               7


static inline uint8
_sw_state_htb_decode_psl(
    uint8 active_psl)
{
    return active_psl & SW_STATE_HTB_RH_PSL_MASK_DECODE;
}


static inline uint8
_sw_state_htb_decode_active(
    uint8 active_psl)
{
    return active_psl >> SW_STATE_HTB_RH_ACTIVE_SHIFT;
}


static inline uint8
_sw_state_htb_active_psl_encode(
    uint8 active,
    uint8 psl)
{
    return ((_sw_state_htb_decode_psl(psl)) | (active << SW_STATE_HTB_RH_ACTIVE_SHIFT));
}


static uint8 _sw_state_htb_compare_key(
    int unit,
    uint8 table_idx,
    void *const key,
    uint32 entry_offset,
    sw_state_htb_t htb)
{
    uint8 not_equal;

    if(_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE)){
        
        DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->table_idx_offset)), &table_idx, 0, htb->table_idx_size, &not_equal, 0, "swstate htb compare table idx");
        if(not_equal != FALSE)
        {
            return not_equal;
        }
    }
    
    DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->key_offset)), key, 0, htb->key_size, &not_equal, 0, "swstate htb compare keys");
    return not_equal;
}

static uint8 isPrime(uint32 max_nof_elements)
{
    uint32 index;
    
    if (max_nof_elements <= 1)
    {
        return FALSE;
    }
    if (max_nof_elements <= 3)
    {
        return TRUE;
    }
    
    if (max_nof_elements % 2 == 0 || max_nof_elements % 3 == 0)
    {
        return FALSE;
    }
    for (index = 5; index*index <= max_nof_elements; index = index + 6)
    {
        if (max_nof_elements % index == 0 || max_nof_elements % (index + 2) == 0)
        {
           return FALSE;
        }
    }
    return TRUE;
}


static uint32 nextPrime(uint32 max_nof_elements)
{
    uint8 found = FALSE;

    
    if (max_nof_elements <= 1)
    {
        return 2;
    }
    if (isPrime(max_nof_elements))
    {
        found = TRUE;
    }
    
    while (!found) {
        max_nof_elements++;
        if (isPrime(max_nof_elements))
        {
            found = TRUE;
        }
    }

    return max_nof_elements;
}

static uint32 prevPrime(uint32 max_nof_elements)
{
    uint8 found = FALSE;

    
    if (max_nof_elements <= 2)
    {
        return max_nof_elements;
    }
    if (isPrime(max_nof_elements))
    {
        found = TRUE;
    }
    
    while (!found)
    {
        max_nof_elements--;
        if (isPrime(max_nof_elements))
        {
            found = TRUE;
        }
    }
    return max_nof_elements;
}


static uint32
_sw_state_htb_simple_large_hash(
    int unit,
    sw_state_htb_t htb,
    uint8 *const key)
{
    uint32 hash = 5381;
    uint32 idx;

    for(idx = 0; idx < htb->key_size; idx++)
    {
        hash += key[idx];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % htb->max_nof_elements;
}


static uint32
_sw_state_htb_simple_large_hash_uint32(
    int unit,
    sw_state_htb_t htb,
    uint8 *const key)
{
    uint32 tmp = 5381;

    tmp = ((tmp << 5) + tmp) ^ (*(uint32*)key);

    return tmp % htb->max_nof_elements;
}


static uint32
_sw_state_htb_simple_small_hash(
    int unit,
    sw_state_htb_t htb,
    uint8 *const key)
{
    uint32 hash = 0xAAAAAAAA;
    uint32 indx;

    for(indx = 0; indx < htb->key_size; indx++)
    {
        hash ^= ((indx & 1) == 0) ? ( (hash << 7) ^ (key[indx]) * (hash >> 3)) : (~((hash << 11) + ((key[indx]) ^ (hash >> 5))));
    }

    return hash % htb->max_nof_elements;
}



static uint32
_sw_state_htb_simple_small_hash_uint32(
    int unit,
    sw_state_htb_t htb,
    uint8 *const key)
{
    uint32 hash = 0xAAAAAAAA;

    hash ^= (~((hash << 11) + ((*(uint32*)key) ^ (hash >> 5))));

    return hash % htb->max_nof_elements;
}


const sw_state_htb_hash_algorithm_cb_t htb_callbacks_hashing[SWSTATE_HTB_HASH_FUNCS_COUNT] = {
    {"SWSTATE_HTB_DEFAULT_HASH",
     _sw_state_htb_simple_small_hash},
    {"SWSTATE_HTB_SMALL_HASH",
     _sw_state_htb_simple_small_hash},
    {"SWSTATE_HTB_LARGE_HASH",
     _sw_state_htb_simple_large_hash},
    {"SWSTATE_HTB_LARGE_HASH_UINT32",
     _sw_state_htb_simple_large_hash_uint32},
    {"SWSTATE_HTB_SMALL_HASH_UINT32",
     _sw_state_htb_simple_small_hash_uint32},
};



static shr_error_e
_sw_state_htb_write_metadata(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *const data,
    uint8 psl,
    uint32 offset)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    if(_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
    {
        
        DNX_SW_STATE_MEMWRITE(unit, htb->node_id, &table_idx, htb->metadata, offset + htb->table_idx_offset, htb->table_idx_size, 0, "swstate htb add table_idx");
    }
    
    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, key, htb->metadata, offset + htb->key_offset, htb->key_size, 0, "swstate htb add key");
    
    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, data, htb->metadata, offset + htb->data_offset, htb->data_size, 0, "swstate htb add data");
    
    psl = _sw_state_htb_active_psl_encode(1, psl);
    
    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, &psl, htb->metadata, offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb add psl and active");

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}

static shr_error_e _sw_state_htb_create_info_verify(
    int unit,
    uint32 key_size,
    uint32 data_size,
    sw_state_htb_create_info_t *create_info)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if(key_size != 0 && create_info->key_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - key_size. Only the argument value or Create_info value should be in use for key_size\n");
    }
    
    if(data_size != 0 && create_info->data_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - data_size. Only argument value or Create_info value should be in use for data_size\n");
    }
    
    if((data_size == 0 && create_info->data_size == 0) || (key_size == 0 && create_info->key_size == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameters. Please provide sizes for key/data in the arguments or Create_info.\n");
    }
    if(create_info->max_nof_elements == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameters. max_nof_elements. Please provide maximum nof elements.\n");
    }
    if (!(create_info->load_factor >= 80 && create_info->load_factor <= 100) && !(create_info->load_factor == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - Load Factor. Must be between 80-100 or 0 per default.\n");
    }
    if((create_info->hash_func < SWSTATE_HTB_DEFAULT_HASH) || (create_info->hash_func >= SWSTATE_HTB_HASH_FUNCS_COUNT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - hash func. Please provide valid ENUM.\n");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
_sw_state_htb_copy_metadata_fields(
    int unit,
    void *dst_ll_node,
    void *const src_ll_node,
    uint32 node_offset,
    uint8 node_size,
    void *dst_table_idx,
    void *const src_table_idx,
    uint32 table_idx_offset,
    uint8 table_idx_size,
    void *dst_key,
    void *const src_key,
    uint32 key_offset,
    uint32 key_size,
    void *dst_data,
    void *const src_data,
    uint32 data_offset,
    uint32 data_size,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    
    if (_SHR_IS_FLAG_SET(flags, (SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE)))
    {
        DNX_SW_STATE_MEMREAD(unit, dst_ll_node, src_ll_node, node_offset, node_size, 0, "swstate htb get node id");
    }
    
    if(_SHR_IS_FLAG_SET(flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
    {
        DNX_SW_STATE_MEMREAD(unit, dst_table_idx, src_table_idx, table_idx_offset,  table_idx_size, 0, "swstate htb get table_idx");
    }
    
    DNX_SW_STATE_MEMREAD(unit, dst_key, src_key, key_offset, key_size, 0, "swstate htb get key");
    DNX_SW_STATE_MEMREAD(unit, dst_data, src_data, data_offset, data_size, 0, "swstate htb get data");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_create(
    int unit,
    uint32 node_id,
    sw_state_htb_t *htb,
    sw_state_htb_create_info_t * create_info,
    uint32 key_size,
    uint32 data_size,
    uint32 alloc_flags)
{
    sw_state_ll_init_info_t ll_init_info;
    uint32 max_nof_elements_temp;
    SHR_FUNC_INIT_VARS(unit);

    if (*htb != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state htb create ERROR: htb already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_ALLOC(unit, node_id, (*htb), **htb,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state hash table");

    
    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(_sw_state_htb_create_info_verify(unit, key_size, data_size, create_info)));

    
    (*htb)->nof_htbs = 1;
    (*htb)->node_size = 0;
    (*htb)->node_offset = 0;
    (*htb)->table_idx_size = 0;
    (*htb)->table_idx_offset = 0;
    (*htb)->node_id = node_id;
    (*htb)->metadata_size = 0;
    (*htb)->max_allowed_nof_elements = create_info->max_nof_elements;
    
    (*htb)->load_factor = (create_info->load_factor == 0) ? 90 : create_info->load_factor;
    (*htb)->hash_func = create_info->hash_func;
    (*htb)->flags = create_info->flags;
    if(_SHR_IS_FLAG_SET((*htb)->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        (*htb)->node_size = sizeof(sw_state_ll_node_t);
    }
    if(_SHR_IS_FLAG_SET((*htb)->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
    {
        (*htb)->table_idx_size = sizeof(uint8);
        (*htb)->nof_htbs = create_info->nof_htbs;
    }
    (*htb)->table_idx_offset = (*htb)->node_size + (*htb)->node_offset;
    (*htb)->key_size = key_size + create_info->key_size;
    (*htb)->data_size = data_size + create_info->data_size;
    (*htb)->key_offset = (*htb)->table_idx_size + (*htb)->table_idx_offset;
    (*htb)->data_offset = (*htb)->key_size + (*htb)->key_offset;
    (*htb)->psl_active_size = sizeof(uint8);
    (*htb)->psl_active_offset = (*htb)->data_size + (*htb)->data_offset;
    
    (*htb)->metadata_size += ((*htb)->psl_active_offset + (*htb)->psl_active_size);

    
    (*htb)->max_nof_elements = 1 + (100 * create_info->max_nof_elements) / (*htb)->load_factor;
    max_nof_elements_temp = prevPrime((*htb)->max_nof_elements);
    if(max_nof_elements_temp <= (*htb)->max_allowed_nof_elements)
    {
        (*htb)->max_nof_elements = nextPrime((*htb)->max_nof_elements);
    }
    else
    {
        (*htb)->max_nof_elements = max_nof_elements_temp;
    }
    if(((*htb)->max_nof_elements > SW_STATE_HTB_CREATE_INFO_MAX_SMALL_HASH_PERFORMANCE) && ((*htb)->hash_func == SWSTATE_HTB_SMALL_HASH || (*htb)->hash_func == SWSTATE_HTB_SMALL_HASH_UINT32 || (*htb)->hash_func == SWSTATE_HTB_DEFAULT_HASH))
    {
        (*htb)->hash_func = SWSTATE_HTB_LARGE_HASH;
    }
    
    if (((*htb)->key_size == sizeof(uint32)) && (create_info->hash_func == SWSTATE_HTB_DEFAULT_HASH))
    {
        if ((*htb)->hash_func == SWSTATE_HTB_LARGE_HASH)
        {
            (*htb)->hash_func = SWSTATE_HTB_LARGE_HASH_UINT32;
        }
        else if ((*htb)->hash_func == SWSTATE_HTB_SMALL_HASH)
        {
            (*htb)->hash_func = SWSTATE_HTB_SMALL_HASH_UINT32;
        }
    }
    
    
    DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->metadata, uint8, ((*htb)->max_nof_elements * (*htb)->metadata_size),
            alloc_flags, "sw_state htb metadata");
    
    DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.key, uint8, (*htb)->key_size,
            alloc_flags, "sw_state htb sandbox key");
    DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.key_swap, uint8, (*htb)->key_size,
            alloc_flags, "sw_state htb sandbox key swap");
    DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.get_next_key, uint8, (*htb)->key_size,
            alloc_flags, "sw_state htb sandbox get_next_key");
    DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.data, uint8, (*htb)->data_size,
            alloc_flags, "sw_state htb sandbox data");
    DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.data_swap, uint8, (*htb)->data_size,
            alloc_flags, "sw_state htb sandbox data swap");

    if (_SHR_IS_FLAG_SET((*htb)->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        
        DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.ll_node, uint8, (*htb)->node_size,
                alloc_flags, "sw_state htb sandbox ll_node");
        DNX_SW_STATE_ALLOC(unit, (*htb)->node_id, (*htb)->sandbox.ll_node_swap, uint8, (*htb)->node_size,
                  alloc_flags, "sw_state htb sandbox ll_node");

        sal_memset(&ll_init_info, 0, sizeof(sw_state_ll_init_info_t));
        ll_init_info.nof_heads = (*htb)->nof_htbs;
        ll_init_info.max_nof_elements = (*htb)->max_allowed_nof_elements;
        SHR_IF_ERR_EXIT(sw_state_ll_create_empty(unit, (*htb)->node_id, &ll_init_info, SW_STATE_LL_MULTIHEAD, 0,
                sizeof(uint32), &((*htb)->traverse_ll), 0, alloc_flags));
    }
    if (create_info->print_cb_name != NULL) {
        sw_state_string_strncpy(
            unit,
            (*htb)->node_id,
            (*htb)->print_cb_name,
            sizeof(char)*(SW_STATE_CB_DB_NAME_STR_SIZE-1),
            create_info->print_cb_name);
    }
    if (create_info->print_cb_name != NULL)
    {
        SW_STATE_CB_DB_REGISTER_CB((*htb)->node_id,
                (*htb)->print_cb_db,
                create_info->print_cb_name,
                dnx_sw_state_hash_table_print_cb_get_cb);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_insert(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *const data)
{
    uint32 hash_index, entry_offset;
    uint8 flag_swap = FALSE;
    uint8 is_suppressed = FALSE;
    uint8 active, not_equal;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnx_sw_state_journal_log_rh_htbl(unit, htb->node_id, htb, table_idx, key));
#endif 

    if(htb->nof_used_elements == htb->max_allowed_nof_elements)
    {
        SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_htb_rh_find(unit, htb, table_idx, key, data, NULL), _SHR_E_NOT_FOUND);
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_FULL);
        }
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EXISTS);
    }
    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
    {
        if(table_idx >= htb->nof_htbs)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_FULL);
        }
    }
    hash_index = htb_callbacks_hashing[htb->hash_func].hash_cb(unit, htb, key);
    htb->sandbox.curr_psl = 0;
    entry_offset = htb->metadata_size * hash_index;
    while(1)
    {
        
        DNX_SW_STATE_MEMREAD(unit, &active, htb->metadata, entry_offset + htb->psl_active_offset,  htb->psl_active_size, 0, "swstate htb active get");
        if(_sw_state_htb_decode_active(active) == 0)
        {
            if(flag_swap)
            {
                SHR_IF_ERR_EXIT(_sw_state_htb_write_metadata(unit, htb, table_idx, htb->sandbox.key, htb->sandbox.data, htb->sandbox.curr_psl, entry_offset));
                if (_SHR_IS_FLAG_SET(htb->flags, (SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE)))
                {
                    SW_STATE_MULTIHEAD_LL_NODE_UPDATE(htb->node_id, htb->traverse_ll, *(sw_state_ll_node_t *)htb->sandbox.ll_node, &entry_offset);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(_sw_state_htb_write_metadata(unit, htb, table_idx, key, data, htb->sandbox.curr_psl, entry_offset));
                DNX_SW_STATE_COUNTER_INC(unit, htb->node_id, htb->nof_used_elements, 1, uint32, 0, "swstate htb increment nof used elements")
                if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
                {
                    
                    SW_STATE_MULTIHEAD_LL_ADD_FIRST(htb->node_id, htb->traverse_ll, table_idx, NULL, &entry_offset);
                    
                    SW_STATE_MULTIHEAD_LL_GET_FIRST(htb->node_id, htb->traverse_ll, table_idx, (sw_state_ll_node_t *)htb->sandbox.ll_node);
                    
                    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, htb->sandbox.ll_node, htb->metadata, entry_offset + htb->node_offset, htb->node_size, 0, "swstate htb add ll node");

                }
            }
            break;
        }
        
        not_equal = _sw_state_htb_compare_key(unit, table_idx, key, entry_offset, htb);
        if(not_equal == FALSE)
        {
            
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EXISTS);
        }
        
        DNX_SW_STATE_MEMREAD(unit, &htb->sandbox.psl, htb->metadata, entry_offset + htb->psl_active_offset,  htb->psl_active_size, 0, "swstate htb get psl");
        if (htb->sandbox.curr_psl <= _sw_state_htb_decode_psl(htb->sandbox.psl))
        {
            entry_offset = entry_offset + htb->metadata_size;
            if(htb->sandbox.curr_psl == SW_STATE_HTB_RH_MAX_PSL_SIZE)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_FULL);
            }
            htb->sandbox.curr_psl++;
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
            
            SHR_IF_ERR_EXIT(_sw_state_htb_copy_metadata_fields(unit, htb->sandbox.ll_node_swap, htb->metadata, entry_offset + htb->node_offset, htb->node_size,
                            &htb->sandbox.table_idx_swap, htb->metadata, entry_offset + htb->table_idx_offset, htb->table_idx_size,
                            htb->sandbox.key_swap, htb->metadata, entry_offset + htb->key_offset, htb->key_size,
                            htb->sandbox.data_swap, htb->metadata, entry_offset + htb->data_offset, htb->data_size, htb->flags));
            
            SHR_IF_ERR_EXIT(_sw_state_htb_write_metadata(unit, htb, table_idx, htb->sandbox.key, htb->sandbox.data, htb->sandbox.curr_psl, entry_offset));
            if (_SHR_IS_FLAG_SET(htb->flags, (SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE)))
            {
                SW_STATE_MULTIHEAD_LL_NODE_UPDATE(htb->node_id, htb->traverse_ll, *(sw_state_ll_node_t *)htb->sandbox.ll_node, &entry_offset);
            }
            
            SHR_IF_ERR_EXIT(_sw_state_htb_copy_metadata_fields(unit, htb->sandbox.ll_node, htb->sandbox.ll_node_swap, 0, htb->node_size,
                            &htb->sandbox.table_idx, &htb->sandbox.table_idx_swap, 0, htb->table_idx_size,
                            htb->sandbox.key, htb->sandbox.key_swap, 0, htb->key_size,
                            htb->sandbox.data, htb->sandbox.data_swap, 0, htb->data_size, htb->flags));
        }
        else
        {
            
            SHR_IF_ERR_EXIT(_sw_state_htb_copy_metadata_fields(unit, htb->sandbox.ll_node, htb->metadata, entry_offset + htb->node_offset, htb->node_size,
                            &htb->sandbox.table_idx, htb->metadata, entry_offset + htb->table_idx_offset, htb->table_idx_size,
                            htb->sandbox.key, htb->metadata, entry_offset + htb->key_offset, htb->key_size,
                            htb->sandbox.data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, htb->flags));
            
            SHR_IF_ERR_EXIT(_sw_state_htb_write_metadata(unit, htb, table_idx, key, data, htb->sandbox.curr_psl, entry_offset));
            if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
            {
                
                SW_STATE_MULTIHEAD_LL_ADD_FIRST(htb->node_id, htb->traverse_ll, table_idx, NULL, &entry_offset);
                
                SW_STATE_MULTIHEAD_LL_GET_FIRST(htb->node_id, htb->traverse_ll, table_idx, (sw_state_ll_node_t *)htb->sandbox.ll_node_swap);
                
                DNX_SW_STATE_MEMWRITE(unit, htb->node_id, htb->sandbox.ll_node_swap, htb->metadata, entry_offset + htb->node_offset, htb->node_size, 0, "swstate htb add ll node");
            }
            DNX_SW_STATE_COUNTER_INC(unit, htb->node_id, htb->nof_used_elements, 1, uint32, 0, "swstate htb increment nof used elements")
            flag_swap = TRUE;
        }
        
        htb->sandbox.curr_psl = ++htb->sandbox.psl;
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
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_find(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *data,
    uint32 *offset)
{
    uint32 hash_index, entry_offset;
    uint8 not_equal;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    hash_index = htb_callbacks_hashing[htb->hash_func].hash_cb(unit, htb, key);
    htb->sandbox.curr_psl = 0;
    htb->sandbox.psl = 0;
    entry_offset = htb->metadata_size * hash_index;
    while(1)
    {
        
        DNX_SW_STATE_MEMREAD(unit, &htb->sandbox.psl, htb->metadata, entry_offset + htb->psl_active_offset,  htb->psl_active_size, 0, "swstate htb get psl");
        if((_sw_state_htb_decode_active(htb->sandbox.psl) == 0) || (htb->sandbox.curr_psl > _sw_state_htb_decode_psl(htb->sandbox.psl)))
        {
            
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
        }
        
        not_equal = _sw_state_htb_compare_key(unit, table_idx, key, entry_offset, htb);
        if(not_equal == FALSE)
        {
            
            if(data != NULL)
            {
                DNX_SW_STATE_MEMREAD(unit, data, htb->metadata, entry_offset + htb->data_offset,  htb->data_size, 0, "swstate htb get data");
            }
            
            if(offset != NULL)
            {
                *offset = entry_offset;
            }
            SHR_EXIT();
        }
        entry_offset = entry_offset + htb->metadata_size;
        htb->sandbox.curr_psl++;
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
    uint8 table_idx,
    void *const key)
{
    uint32 entry_offset, entry_offset_shift, hash_index;
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_htb_rh_find(unit, htb, table_idx, key, NULL, &entry_offset), _SHR_E_NOT_FOUND);
    entry_offset_shift = entry_offset + htb->metadata_size;
    hash_index = entry_offset / htb->metadata_size;

#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnx_sw_state_journal_log_rh_htbl(unit, htb->node_id, htb, table_idx, key));
#endif 
    
    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        
        DNX_SW_STATE_MEMREAD(unit, htb->sandbox.ll_node, htb->metadata, entry_offset + htb->node_offset,  htb->node_size, 0, "swstate htb get ll node");
        SW_STATE_MULTIHEAD_LL_REMOVE_NODE(htb->node_id, htb->traverse_ll, table_idx, *(sw_state_ll_node_t*)(htb->sandbox.ll_node));
    }
    while(1){
        if(hash_index == (htb->max_nof_elements - 1))
        {
            
            entry_offset_shift = 0;
            hash_index = 0;
        }
        
        DNX_SW_STATE_MEMREAD(unit, &htb->sandbox.psl, htb->metadata, entry_offset_shift + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb get psl");
        if((_sw_state_htb_decode_active(htb->sandbox.psl) == 0 )|| (_sw_state_htb_decode_psl(htb->sandbox.psl) == 0))
        {
            
            htb->sandbox.psl = _sw_state_htb_active_psl_encode(0 , htb->sandbox.psl);
            
            DNX_SW_STATE_MEMWRITE(unit, htb->node_id, &htb->sandbox.psl, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb update psl");
            DNX_SW_STATE_COUNTER_DEC(unit, htb->node_id, htb->nof_used_elements, 1, uint32, 0, "swstate htb decrement nof used elements")
            SHR_EXIT();
        }

        htb->sandbox.psl--;
        
        DNX_SW_STATE_MEMWRITE(unit, htb->node_id, htb->metadata + entry_offset_shift, htb->metadata, entry_offset, htb->metadata_size - htb->psl_active_size, 0, "swstate htb delete entry");
        if (_SHR_IS_FLAG_SET(htb->flags, (SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE)))
        {
            DNX_SW_STATE_MEMREAD(unit, htb->sandbox.ll_node, htb->metadata, entry_offset, htb->node_size, 0, "swstate htb get node_id");
            SW_STATE_MULTIHEAD_LL_NODE_UPDATE(htb->node_id, htb->traverse_ll, *(sw_state_ll_node_t *)htb->sandbox.ll_node, &entry_offset);
        }
        
        htb->sandbox.psl = _sw_state_htb_active_psl_encode(1 , htb->sandbox.psl);
        
        DNX_SW_STATE_MEMWRITE(unit, htb->node_id, &htb->sandbox.psl, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb update psl");
        hash_index++;
        entry_offset = entry_offset_shift;
        entry_offset_shift += htb->metadata_size;
    }
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_replace(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    void *const key,
    void *const data)
{
    uint32 entry_offset;
    uint8 is_suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

#ifdef BCM_DNX_SUPPORT
	SHR_IF_ERR_EXIT(dnx_sw_state_journal_log_rh_htbl(unit, htb->node_id, htb, table_idx, key));
#endif 

    
    SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(sw_state_htb_rh_find(unit, htb, table_idx, key, NULL, &entry_offset), _SHR_E_NOT_FOUND);
    
    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb update data");

    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}
shr_error_e sw_state_htb_rh_traverse(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    sw_state_htb_traverse_cb traverse_func,
    void *user_data)
{
    
    uint32 iterator, entry_offset;
    uint8 active, not_equal;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        SW_STATE_MULTIHEAD_LL_GET_FIRST(htb->node_id, htb->traverse_ll, table_idx,(sw_state_ll_node_t *)htb->sandbox.ll_node);
        while (DNX_SW_STATE_LL_IS_NODE_VALID(*(sw_state_ll_node_t *)htb->sandbox.ll_node))
        {
            SW_STATE_MULTIHEAD_LL_NODE_VALUE(htb->node_id, htb->traverse_ll, *(sw_state_ll_node_t *)htb->sandbox.ll_node, &entry_offset);
            DNX_SW_STATE_MEMREAD(unit, htb->sandbox.key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key");
            DNX_SW_STATE_MEMREAD(unit, htb->sandbox.data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb get data");
            SW_STATE_MULTIHEAD_LL_NEXT_NODE(htb->node_id, htb->traverse_ll, *(sw_state_ll_node_t *)htb->sandbox.ll_node, (sw_state_ll_node_t *)htb->sandbox.ll_node);
            SHR_IF_ERR_EXIT(traverse_func(unit, htb->sandbox.key, htb->sandbox.data, user_data));
        }
    }
    else
    {
        entry_offset = 0;
        for(iterator = 0; iterator < htb->max_nof_elements; iterator++)
        {
            
            DNX_SW_STATE_MEMREAD(unit, &active, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb active get");
            if(_sw_state_htb_decode_active(active))
            {
                if(_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE)){
                    
                    DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->table_idx_offset)), &table_idx, 0, htb->table_idx_size, &not_equal, 0, "swstate htb compare table idx");
                    if(not_equal)
                    {
                        entry_offset += htb->metadata_size;
                        continue;
                    }
                }

                DNX_SW_STATE_MEMREAD(unit, htb->sandbox.key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key");
                DNX_SW_STATE_MEMREAD(unit, htb->sandbox.data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb get data");
                SHR_IF_ERR_EXIT(traverse_func(unit, htb->sandbox.key, htb->sandbox.data, user_data));
            }
            entry_offset += htb->metadata_size;
        }
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

    uint8 active, table_idx;
    dnx_sw_state_hash_table_print_cb print_cb;

    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    DNX_SW_STATE_PRINT(unit, "\nKey size: %u Bytes\n", htb->key_size);
    DNX_SW_STATE_PRINT(unit, "Data size: %u Bytes\n", htb->data_size);
    DNX_SW_STATE_PRINT(unit, "Active entries: %u\n", htb->nof_used_elements);
    DNX_SW_STATE_PRINT(unit, "Max number of entries: %u\n", htb->max_allowed_nof_elements);
    DNX_SW_STATE_PRINT(unit, "List of all key and data pairs:\n");

    entry_offset = 0;
    for(iterator = 0; iterator < htb->max_nof_elements; iterator++)
    {
        DNX_SW_STATE_MEMREAD(unit, &active, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb active get");
        if(_sw_state_htb_decode_active(active))
        {
            DNX_SW_STATE_MEMREAD(unit, htb->sandbox.key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key");
            DNX_SW_STATE_MEMREAD(unit, htb->sandbox.data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb get data");

            if (sal_strncmp(htb->print_cb_name, "", SW_STATE_CB_DB_NAME_STR_SIZE-1) != 0) {
                SW_STATE_CB_DB_GET_CB(htb->node_id,
                        htb->print_cb_db,
                        &print_cb,
                        dnx_sw_state_hash_table_print_cb_get_cb);

                print_cb(unit, htb->sandbox.key, htb->sandbox.data);
            }
            else
            {
                if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
                {
                    DNX_SW_STATE_MEMREAD(unit, &table_idx, htb->metadata, entry_offset + htb->table_idx_offset, htb->table_idx_size, 0, "swstate htb get table_idx");
                    
                    DNX_SW_STATE_PRINT(unit, "\n Table idx: %u", table_idx);
                }
                
                DNX_SW_STATE_PRINT(unit, "\nKey: 0x");
                for (print_indx = 0; print_indx < htb->key_size; ++print_indx)
                {
                    DNX_SW_STATE_PRINT(unit, "%02x", htb->sandbox.key[htb->key_size - print_indx - 1]);
                }
                
                DNX_SW_STATE_PRINT(unit, "\nData: 0x");
                for (print_indx = 0; print_indx < htb->data_size; ++print_indx)
                {
                    DNX_SW_STATE_PRINT(unit, "%02x", htb->sandbox.data[htb->data_size - print_indx - 1]);
                }
            }
            DNX_SW_STATE_PRINT(unit, "\n");
        }
        entry_offset += htb->metadata_size;
    }
    SHR_EXIT();
exit:

    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_delete_all(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx)
{
    uint32 entries = 0;
    uint32 entry_offset = 0;
    uint32 entry_offset_shift = htb->metadata_size;
    uint8 is_suppressed = FALSE;
    uint8 active, not_equal, active_shift;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        SHR_IF_ERR_EXIT(sw_state_ll_clear_all_at_index(unit, htb->node_id, htb->traverse_ll, (uint32)(table_idx)));
    }
    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
    {
        for(entries = 0; entries < htb->max_nof_elements; entries++)
        {
            entry_offset = entries * htb->metadata_size;
            entry_offset_shift = entry_offset + htb->metadata_size;
            
            DNX_SW_STATE_MEMREAD(unit, &active, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb active get");
            if(_sw_state_htb_decode_active(active))
            {
                
                DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->table_idx_offset)), &table_idx, 0, htb->table_idx_size, &not_equal, 0, "swstate htb compare table idx");
                if(not_equal)
                {
                    continue;
                }
                
                while(1)
                {
                    if(entries == (htb->max_nof_elements - 1))
                    {
                        entry_offset_shift = 0;
                    }
                    DNX_SW_STATE_MEMREAD(unit, &active_shift,htb->metadata, entry_offset_shift + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb get active shift");
                    if(_sw_state_htb_decode_active(active_shift) == 0 || (_sw_state_htb_decode_psl(active_shift)) == 0)
                    {
                        
                        active_shift = _sw_state_htb_active_psl_encode(0 , active_shift);
                        
                        DNX_SW_STATE_MEMWRITE(unit, htb->node_id, &active_shift, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb update psl");
                        DNX_SW_STATE_COUNTER_DEC(unit, htb->node_id, htb->nof_used_elements, 1, uint32, 0, "swstate htb decrement nof used elements")
                        break;
                    }
                    
                    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, htb->metadata + entry_offset_shift, htb->metadata, entry_offset, htb->metadata_size - htb->psl_active_size, 0, "swstate htb delete entry");
                    
                    htb->sandbox.psl = _sw_state_htb_decode_psl(active_shift);
                    htb->sandbox.psl--;
                    
                    htb->sandbox.psl = _sw_state_htb_active_psl_encode(1 , htb->sandbox.psl);
                    
                    DNX_SW_STATE_MEMWRITE(unit, htb->node_id, &htb->sandbox.psl, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb update psl");
                    entry_offset = entry_offset_shift;
                    entry_offset_shift +=  htb->metadata_size;
                }
            }
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(sw_state_htb_rh_delete_all_tables(unit, htb));
    }
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_delete_all_tables(
    int unit,
    sw_state_htb_t htb)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 

    is_suppressed = TRUE;

    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        SW_STATE_MULTIHEAD_LL_CLEAR_ALL(htb->node_id, htb->traverse_ll);
    }
    DNX_SW_STATE_MEMSET(unit, htb->node_id, htb->metadata, 0, 0x0, (htb->metadata_size * htb->max_nof_elements), 0, "swstate htb delete all entries");
    DNX_SW_STATE_MEMSET(unit, htb->node_id, &(htb->nof_used_elements), 0, 0x0, sizeof(uint32), 0, "swstate htb memset 0 nof used elements");
    SHR_EXIT();
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_htb_rh_get_next(
    int unit,
    sw_state_htb_t htb,
    uint8 table_idx,
    uint32 *iter,
    void *key,
    void *data)
{
    uint32 index, entry_offset;
    uint8 active;
    uint8 not_equal = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(htb);

    if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE))
    {
        if(*iter == 0)
        {
            
            SW_STATE_MULTIHEAD_LL_GET_FIRST(htb->node_id, htb->traverse_ll, table_idx, iter);
        }
        if(DNX_SW_STATE_LL_IS_NODE_VALID(*iter))
        {
            
            SW_STATE_MULTIHEAD_LL_NODE_VALUE(htb->node_id, htb->traverse_ll, *iter, &entry_offset);
            
            DNX_SW_STATE_MEMREAD(unit, key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key");
            DNX_SW_STATE_MEMREAD(unit, data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb get data");
            
            SW_STATE_MULTIHEAD_LL_NEXT_NODE(htb->node_id, htb->traverse_ll, *iter, iter);
            if(*iter  == SW_STATE_LL_INVALID)
            {
                *iter = UTILEX_U32_MAX;
            }
            SHR_EXIT();
        }
    }
    else
    {
        
        if(*iter != 0)
        {
            index = *iter - 1;
            entry_offset = index * htb->metadata_size;
            DNX_SW_STATE_MEMREAD(unit, &active, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb active get");
            if(_sw_state_htb_decode_active(active))
            {
                if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
                {
                    DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->table_idx_offset)), &table_idx, 0, htb->table_idx_size, &not_equal, 0, "swstate htb compare table idx");
                }
                if(not_equal == FALSE)
                {
                    
                    DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->key_offset)),htb->sandbox.get_next_key, 0, htb->key_size, &not_equal, 0, "swstate htb compare keys");
                    if(not_equal)
                    {
                        DNX_SW_STATE_MEMREAD(unit, key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key");
                        DNX_SW_STATE_MEMREAD(unit, htb->sandbox.get_next_key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key and save to get_next_key");
                        DNX_SW_STATE_MEMREAD(unit, data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb get data");
                        SHR_EXIT();
                    }
                }
            }
        }
        for (index = *iter, entry_offset = (*iter * htb->metadata_size); index < htb->max_nof_elements; index++)
        {
            DNX_SW_STATE_MEMREAD(unit, &active, htb->metadata, entry_offset + htb->psl_active_offset, htb->psl_active_size, 0, "swstate htb active get");
            if(_sw_state_htb_decode_active(active))
            {
                if (_SHR_IS_FLAG_SET(htb->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
                {
                    DNX_SW_STATE_MEMCMP(unit, (htb->metadata + (entry_offset + htb->table_idx_offset)), &table_idx, 0, htb->table_idx_size, &not_equal, 0, "swstate htb compare table idx");
                    if(not_equal)
                    {
                        continue;
                    }
                }
                
                DNX_SW_STATE_MEMREAD(unit, key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key");
                DNX_SW_STATE_MEMREAD(unit, htb->sandbox.get_next_key, htb->metadata, entry_offset + htb->key_offset, htb->key_size, 0, "swstate htb get key and save to get_next_key");
                DNX_SW_STATE_MEMREAD(unit, data, htb->metadata, entry_offset + htb->data_offset, htb->data_size, 0, "swstate htb get data");
                *iter = index + 1;
                SHR_EXIT();
            }
            entry_offset += htb->metadata_size;
        }
    }
    *iter = UTILEX_U32_MAX;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

