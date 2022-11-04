
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_HASHDNX

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_index_htb.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_hash_table_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/dnx_sw_state_journal.h>
#endif

static inline shr_error_e
_sw_state_index_htb_write_to_key_map(
    int unit,
    sw_state_index_htb_t index_htb,
    void *const key,
    uint32 data_idx)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    DNX_SW_STATE_MEMWRITE(unit, index_htb->htb_rh->node_id, key, index_htb->key_map, data_idx * index_htb->htb_rh->key_size, index_htb->htb_rh->key_size, 0, "swstate htb write key to key_map array at data_idx");

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

static inline shr_error_e
_sw_state_index_htb_read_key_map(
    int unit,
    sw_state_index_htb_t index_htb,
    uint32 data_idx,
    void *key)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    DNX_SW_STATE_MEMREAD(unit, key, index_htb->key_map, data_idx * index_htb->htb_rh->key_size, index_htb->htb_rh->key_size, 0, "swstate htb read key from key_map array at data_idx");
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

static shr_error_e _sw_state_index_htb_clear_occ_bit(
    int unit,
    void *key,
    void *data,
    void *user_data)
{
    sw_state_index_htb_t *index_htb;
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;
    index_htb = (sw_state_index_htb_t *)user_data;
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, (*index_htb)->htb_rh->node_id, (*index_htb)->data_indices_in_use, *(uint32 *)data, FALSE));
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


static shr_error_e _sw_state_index_htb_rh_verify_by_index_input(
    int unit,
    sw_state_index_htb_t index_htb,
    uint32 data_idx)
{
    uint8 index_is_ocupied;
    SHR_FUNC_INIT_VARS(unit);

    if(data_idx >= index_htb->htb_rh->max_allowed_nof_elements)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - data_idx(%u). Out of boundary access - Expected data_idx(%d) to be less than %u\n", data_idx, data_idx, index_htb->htb_rh->max_allowed_nof_elements);
    }
    SHR_IF_ERR_EXIT(sw_state_occ_bm_is_occupied(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use, data_idx, &index_is_ocupied));
    if(index_is_ocupied)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EXISTS);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e sw_state_index_htb_rh_create(
    int unit,
    uint32 node_id,
    sw_state_index_htb_t * index_htb,
    sw_state_htb_create_info_t *create_info,
    uint32 key_size,
    uint32 alloc_flags)
{
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    SHR_FUNC_INIT_VARS(unit);

    
    if(create_info->data_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter - create_info.data_size. Do not set data_size for Index HTB.\n");
    }
    DNX_SW_STATE_ALLOC(unit, node_id, (*index_htb), **index_htb,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state hash table");
    
    SHR_IF_ERR_EXIT(sw_state_htb_rh_create(unit, node_id, &((*index_htb)->htb_rh), create_info, key_size, sizeof(uint32), alloc_flags));

    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));
    btmp_init_info.size = (*index_htb)->htb_rh->max_nof_elements;
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_create(unit, node_id, &btmp_init_info, &((*index_htb)->data_indices_in_use), (*index_htb)->htb_rh->max_nof_elements, alloc_flags));
    
    DNX_SW_STATE_ALLOC(unit, node_id, (*index_htb)->key_map, uint8,((*index_htb)->htb_rh->key_size * (*index_htb)->htb_rh->max_allowed_nof_elements),
            alloc_flags, "sw_state index htb key_map alloc");
    DNX_SW_STATE_ALLOC(unit, node_id, (*index_htb)->sandbox.key, uint8, (*index_htb)->htb_rh->key_size,
            alloc_flags, "sw_state index htb sandbox key");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_index_htb_rh_insert(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key,
    uint32 *data_idx)
{
    uint8 is_suppressed = FALSE;
    uint8 found_free_position = FALSE;
    uint32 data_idx_bit;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_alloc_next(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use, &data_idx_bit, &found_free_position));
    if(!found_free_position)
    {
        SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_index_htb_rh_find(unit, index_htb, table_idx, key, data_idx), _SHR_E_NOT_FOUND);
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_FULL);
        }
        SHR_EXIT();
    }
    *data_idx = data_idx_bit;
    
    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_htb_rh_insert(unit, index_htb->htb_rh, table_idx, key, data_idx), _SHR_E_EXISTS);
    if (SHR_GET_CURRENT_ERR() == _SHR_E_EXISTS)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        
        if(found_free_position)
        {
            SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use, data_idx_bit, FALSE));
        }
        SW_STATE_MULTIHEAD_INDEX_HTB_RH_FIND(index_htb, table_idx, key, data_idx);
        SHR_EXIT();
    }
    
    SHR_IF_ERR_EXIT(_sw_state_index_htb_write_to_key_map(unit, index_htb, key, *(data_idx)));
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
shr_error_e sw_state_index_htb_rh_find(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key,
    uint32 *data_idx)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_htb_rh_find
                           (unit, index_htb->htb_rh, 0, (uint8 *)key, (uint8 *) data_idx, NULL), _SHR_E_NOT_FOUND);
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        *data_idx = UTILEX_U32_MAX;
        SHR_EXIT();
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

shr_error_e sw_state_index_htb_rh_delete(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    
    SW_STATE_MULTIHEAD_HTB_RH_FIND(index_htb->htb_rh, table_idx, key, &(index_htb->sandbox.data_idx));
    
    SW_STATE_MULTIHEAD_HTB_RH_DELETE(index_htb->htb_rh, table_idx, key);
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use, index_htb->sandbox.data_idx, FALSE));
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

shr_error_e sw_state_index_htb_rh_delete_all(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    SW_STATE_HTB_VERIFY_HTB_IS_ACTIVE(index_htb);

    if (_SHR_IS_FLAG_SET(index_htb->htb_rh->flags, SW_STATE_HTB_CREATE_INFO_MULTI_TABLE))
    {
        SW_STATE_MULTIHEAD_HTB_RH_TRAVERSE(index_htb->htb_rh, table_idx, _sw_state_index_htb_clear_occ_bit, index_htb);
        SW_STATE_MULTIHEAD_HTB_RH_DELETE_ALL(index_htb->htb_rh, table_idx);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(sw_state_index_htb_rh_delete_all_tables(unit, index_htb));
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

shr_error_e sw_state_index_htb_rh_delete_all_tables(
    int unit,
    sw_state_index_htb_t index_htb)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;
    SW_STATE_MULTIHEAD_HTB_RH_DELETE_ALL_TABLES(index_htb->htb_rh);
    
    DNX_SW_STATE_MEMSET(unit, index_htb->htb_rh->node_id, index_htb->key_map, 0, 0x0, (index_htb->htb_rh->key_size * index_htb->htb_rh->max_allowed_nof_elements), 0, "swstate htb key_map memset to 0");
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_clear(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use));
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

shr_error_e sw_state_index_htb_rh_insert_at_index(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    void *const key,
    uint32 data_idx)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(_sw_state_index_htb_rh_verify_by_index_input(unit, index_htb, data_idx), _SHR_E_EXISTS);
    if(SHR_GET_CURRENT_ERR() == _SHR_E_EXISTS)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        
        SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(sw_state_index_htb_rh_find(unit, index_htb, table_idx, key, &(index_htb->sandbox.data_idx)), _SHR_E_NOT_FOUND);
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
        {
            
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_FAIL);
        }
        if (index_htb->sandbox.data_idx != data_idx)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "\nThe key %u is attempted to be mapped to index %u, but it is already mapped to a different index %u.\n",
                    *(uint32 *)key, data_idx, index_htb->sandbox.data_idx);
        }
        SHR_EXIT();
    }
    
    SW_STATE_MULTIHEAD_HTB_RH_INSERT(index_htb->htb_rh, table_idx, key, &data_idx);

    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use, data_idx, TRUE));
    
    SHR_IF_ERR_EXIT(_sw_state_index_htb_write_to_key_map(unit, index_htb, key, data_idx));
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

shr_error_e sw_state_index_htb_rh_delete_by_index(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    uint32 data_idx)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(_sw_state_index_htb_rh_verify_by_index_input(unit, index_htb, data_idx), _SHR_E_EXISTS);
    if(SHR_GET_CURRENT_ERR() == _SHR_E_EXISTS)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        
        SHR_IF_ERR_EXIT(_sw_state_index_htb_read_key_map(unit, index_htb, data_idx, index_htb->sandbox.key));
        
        SW_STATE_MULTIHEAD_HTB_RH_DELETE(index_htb->htb_rh, table_idx, index_htb->sandbox.key);
        
        SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, index_htb->htb_rh->node_id, index_htb->data_indices_in_use, data_idx, FALSE));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}

shr_error_e sw_state_index_htb_rh_get_by_index(
    int unit,
    sw_state_index_htb_t index_htb,
    uint8 table_idx,
    uint32 data_idx,
    void *key)
{
    uint8 is_suppressed = FALSE;
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_COMPARISON_SUPPRESS(unit);
#endif 
    is_suppressed = TRUE;

    SHR_IF_ERR_EXIT_ELSE_SET_EXPECT(_sw_state_index_htb_rh_verify_by_index_input(unit, index_htb, data_idx), _SHR_E_EXISTS);
    if(SHR_GET_CURRENT_ERR() == _SHR_E_EXISTS)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        if(key != NULL)
        {
            SHR_IF_ERR_EXIT(_sw_state_index_htb_read_key_map(unit, index_htb, data_idx, key));
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
exit:
    if(is_suppressed)
    {
#ifdef BCM_DNX_SUPPORT
        DNX_SW_STATE_COMPARISON_UNSUPPRESS(unit);
#endif 
    }
    SHR_FUNC_EXIT;
}
