/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include "dbal_internal.h"
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/cmic.h>
#include <sal/types.h>

#if defined(INCLUDE_KBP)
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_fwd_tcam_access_mapper_access.h>


#define DNX_KBP_HANDLES_GET(_table_, _kbp_handles_)                                                                 \
    do                                                                                                              \
    {                                                                                                               \
        if (_table_->kbp_handles)                                                                                   \
        {                                                                                                           \
            if (((kbp_db_handles_t *) _table_->kbp_handles)->db_p == NULL)                                          \
            {                                                                                                       \
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP Device is not Synced or the table is not in use\n");               \
            }                                                                                                       \
            _kbp_handles_ = _table_->kbp_handles;                                                                   \
        }                                                                                                           \
        else                                                                                                        \
        {                                                                                                           \
            SHR_ERR_EXIT(_SHR_E_INTERNAL,                                                                           \
                    "kbp handles not initialized for table %s\n", _table_->table_name);                             \
        }                                                                                                           \
    }                                                                                                               \
    while(0)


#define DNX_KBP_ACCESS_PERFORM_CACHING(_is_acl_, _caching_bmp_) \
    ((_caching_bmp_ & DNX_KBP_CACHING_BMP_ALLOWED) && \
    (((_is_acl_ && (_caching_bmp_ & DNX_KBP_CACHING_BMP_ACL)) || \
    (!is_acl && (_caching_bmp_ & DNX_KBP_CACHING_BMP_FWD)))))


#define DNX_KBP_ACCESS_ACL_DB_ENTRY_GET(_unit_, _entry_handle_, _db_entry_) \
do { \
    shr_error_e rv = _SHR_E_NONE; \
    rv = dbal_kbp_entry_acl_exists(_unit_, _entry_handle_); \
    if (rv == _SHR_E_NOT_FOUND) { SHR_IF_ERR_EXIT_NO_MSG(rv); } \
    else { SHR_IF_ERR_EXIT(rv); } \
    _db_entry_ = (struct kbp_entry *) INT_TO_PTR(_entry_handle_->phy_entry.entry_hw_id); \
} while(0)


static shr_error_e
dbal_kbp_physical_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 print_only_key,
    char *action)
{
    int index, bsl_severity;
    int key_size_in_words = BITS2WORDS(entry_handle->phy_entry.key_size);
    int payload_size_in_words = BITS2WORDS(entry_handle->phy_entry.payload_size);
    uint8 is_acl = (entry_handle->table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID ? TRUE : FALSE);
    int logger_action = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE(bsl_severity);

    if (bsl_severity >= bslSeverityInfo)
    {
        if (dbal_logger_is_enable(unit, entry_handle->table_id))
        {
            if ((!sal_strcasecmp(action, "get from")) || (!sal_strcasecmp(action, "get next from"))
                || (!sal_strcasecmp(action, "Access ID get")))
            {
                logger_action = 1;
            }
            else if (!sal_strcasecmp(action, "delete from"))
            {
                logger_action = 2;
            }
            else
            {
                logger_action = 0;
            }

            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(TRUE, bslSeverityInfo, DBAL_ACCESS_PRINTS_KBP, logger_action);

            
            LOG_CLI((BSL_META("Entry %s db_p %p ad_db_p (regular) %p ad_db_p (optimized) %p\n"),
                     action,
                     ((void *) ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->db_p),
                     ((void *) ((kbp_db_handles_t *) entry_handle->table->
                                kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]),
                     ((void *) ((kbp_db_handles_t *) entry_handle->table->
                                kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED])));

            
            LOG_CLI((BSL_META("Phy. entry buffer:\n")));
            LOG_CLI((BSL_META("Key(%3d bits): 0x"), entry_handle->phy_entry.key_size));
            for (index = key_size_in_words - 1; index >= 0; index--)
            {
                LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.key[index]));
            }

            if (is_acl)
            {
                LOG_CLI((BSL_META("\n")));
                LOG_CLI((BSL_META("Key Mask     : 0x")));
                for (index = key_size_in_words - 1; index >= 0; index--)
                {
                    LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.k_mask[index]));
                }
            }

            LOG_CLI((BSL_META(" %s %d\n"), (is_acl ? "Priority" : "/"), entry_handle->phy_entry.prefix_length));

            if (!print_only_key)
            {
                
                LOG_CLI((BSL_META("Payload(%3d bits): 0x"), entry_handle->phy_entry.payload_size));
                for (index = payload_size_in_words - 1; index >= 0; index--)
                {
                    LOG_CLI((BSL_META("%08x"), entry_handle->phy_entry.payload[index]));
                }
                LOG_CLI((BSL_META("\n")));
            }
            DBAL_PRINT_FRAME_FOR_ACCESS_PRINTS(FALSE, bslSeverityInfo, DBAL_ACCESS_PRINTS_KBP, logger_action);
        }
    }

    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_res_type_resolution(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 payload_size)
{
    int index;
    int cur_res_type = 0;
    uint8 is_payload_opt = FALSE;
    
    uint32 result_type[1] = { 0 };
    dbal_table_field_info_t table_field_info;
    int res_type_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (payload_size == ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->opt_result_size)
    {
        is_payload_opt = TRUE;
    }

    while (cur_res_type < entry_handle->table->nof_result_types)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id,
                                                   DBAL_FIELD_RESULT_TYPE, 0, cur_res_type, 0, &table_field_info));

        
        table_field_info.bits_offset_in_buffer =
            entry_handle->table->max_payload_size - table_field_info.field_nof_bits;
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, DBAL_FIELD_RESULT_TYPE,
                                                   entry_handle->phy_entry.payload, result_type));

        for (index = 0; index < entry_handle->table->multi_res_info[cur_res_type].result_type_nof_hw_values; index++)
        {
            if (DNX_KBP_USE_OPTIMIZED_RESULT && (is_payload_opt == TRUE))
            {
                
                if (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[index] ==
                    DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE)
                {
                    result_type[0] = DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE;
                    entry_handle->cur_res_type = cur_res_type;
                    entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size = payload_size;
                    res_type_found = 1;
                    break;
                }
            }
            else
            {
                
                if (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[index] == result_type[0])
                {
                    entry_handle->cur_res_type = cur_res_type;
                    entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size = payload_size;
                    res_type_found = 1;
                    break;
                }
            }
        }
        if (res_type_found)
        {
            break;
        }
        cur_res_type++;
    }

    if (cur_res_type == entry_handle->table->nof_result_types)
    {
        if (!res_type_found)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    
    if ((payload_size != 0) &&
        (entry_handle->table->multi_res_info[cur_res_type].entry_payload_size < entry_handle->table->max_payload_size))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                        (unit,
                         entry_handle->table->max_payload_size -
                         entry_handle->table->multi_res_info[cur_res_type].entry_payload_size, TRUE, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_res_type_align(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 payload_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_IF_ERR_EXIT(dbal_kbp_res_type_resolution(unit, entry_handle, payload_size));
    }
    else
    {
        entry_handle->cur_res_type = 0;
        if (entry_handle->table->multi_res_info[0].entry_payload_size < entry_handle->table->max_payload_size)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_phy_entry_align
                            (unit,
                             entry_handle->table->max_payload_size -
                             entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size,
                             TRUE, entry_handle));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_access_id_hash_key_fill(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dnx_kbp_fwd_tcam_access_hash_key_t * hash_key,
    uint32 *table_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(hash_key, _SHR_E_PARAM, "hash_key");
    SHR_NULL_CHECK(table_index, _SHR_E_PARAM, "table_index");
    sal_memset(hash_key, 0, sizeof(dnx_kbp_fwd_tcam_access_hash_key_t));

    if (entry_handle->table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
    {
        *table_index = DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC;
    }
    else if (entry_handle->table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        *table_index = DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for KBP IPv4/6 MC TCAM tables\n");
    }

    SHR_BITCOPY_RANGE(hash_key->key, 0, entry_handle->phy_entry.key, 0, entry_handle->table->key_size);
    SHR_BITCOPY_RANGE(hash_key->key_mask, 0, entry_handle->phy_entry.k_mask, 0, entry_handle->table->key_size);
    SHR_BITAND_RANGE(hash_key->key, hash_key->key_mask, 0, entry_handle->table->key_size, hash_key->key);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_access_id_by_key_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 access_id = 0;
    uint32 table_index;
    uint8 found = FALSE;
    dnx_kbp_fwd_tcam_access_hash_key_t hash_key = { {0}, {0} };

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->table_type != DBAL_TABLE_TYPE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get_access_id supported only for TCAM\n");
    }

    SHR_IF_ERR_EXIT(dbal_kbp_access_id_hash_key_fill(unit, entry_handle, &hash_key, &table_index));
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "access ID get"));
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.find(unit, table_index, &hash_key, &access_id, &found));

    if (!found)
    {
        entry_handle->phy_entry.entry_hw_id = 0;
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
    else
    {
        entry_handle->phy_entry.entry_hw_id = (int) access_id;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_access_id_create(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 access_id)
{
    uint32 table_index;
    uint8 success = FALSE;
    dnx_kbp_fwd_tcam_access_hash_key_t hash_key = { {0}, {0} };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_kbp_access_id_hash_key_fill(unit, entry_handle, &hash_key, &table_index));
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "access ID create"));
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.insert(unit, table_index, &hash_key, &access_id, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Failed to create new access_id %d\n", access_id);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_access_id_destroy(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 table_index;
    dnx_kbp_fwd_tcam_access_hash_key_t hash_key = { {0}, {0} };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_kbp_access_id_hash_key_fill(unit, entry_handle, &hash_key, &table_index));
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "access ID destroy"));
    SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.delete(unit, table_index, &hash_key));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_get_ranges(
    int unit,
    dbal_entry_handle_t * entry_handle,
    struct kbp_entry_info *entry_info)
{
    int key_index;
    int range_id;
    uint16 lo[DNX_KBP_MAX_NOF_RANGES];
    uint16 hi[DNX_KBP_MAX_NOF_RANGES];

    SHR_FUNC_INIT_VARS(unit);

    
    for (range_id = 0; range_id < entry_info->nranges; range_id++)
    {
        lo[range_id] = entry_info->rinfo[range_id].lo;
        hi[range_id] = entry_info->rinfo[range_id].hi;
    }

    
    for (key_index = 0, range_id = 0; key_index < entry_handle->table->nof_key_fields; key_index++)
    {
        if (SHR_BITGET(entry_handle->table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
        {
            if (range_id >= entry_info->nranges)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected ranges are more than the retrieved ranges (%d)",
                             entry_info->nranges);
            }

            if (hi[range_id] != 0)
            {
                
                uint32 key_value[1] = { lo[range_id] };
                uint32 key_mask[1] = { 0xFFFFFFFF };
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(key_value,
                                                               entry_handle->table->
                                                               keys_info[key_index].bits_offset_in_buffer,
                                                               DNX_KBP_MAX_RANGE_SIZE_IN_BITS,
                                                               entry_handle->phy_entry.key));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (key_mask, entry_handle->table->keys_info[key_index].bits_offset_in_buffer,
                                 DNX_KBP_MAX_RANGE_SIZE_IN_BITS, entry_handle->phy_entry.k_mask));
            }
            entry_handle->key_field_ranges[key_index] = hi[range_id] - lo[range_id] + 1;
            entry_handle->nof_ranged_fields++;
            range_id++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_kbp_entry_add_ranges(
    int unit,
    dbal_entry_handle_t * entry_handle,
    kbp_db_t_p db_p,
    struct kbp_entry *db_entry)
{
    int key_index;
    int range_id = 0;
    int nof_ranges;
    uint16 lo[DNX_KBP_MAX_NOF_RANGES];
    uint16 hi[DNX_KBP_MAX_NOF_RANGES];

    SHR_FUNC_INIT_VARS(unit);

    
    for (key_index = 0; key_index < entry_handle->table->nof_key_fields; key_index++)
    {
        if (SHR_BITGET(entry_handle->table->keys_info[key_index].field_indication_bm, DBAL_FIELD_IND_IS_RANGED))
        {
            uint32 key_value[1] = { 0 };

            if (entry_handle->key_field_ids[key_index] == DBAL_FIELD_EMPTY)
            {
                lo[range_id] = 0;
                hi[range_id] = -1;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_key_field_get(unit, entry_handle, entry_handle->key_field_ids[key_index],
                                                         key_value, NULL, DBAL_POINTER_TYPE_ARR_UINT32));

                lo[range_id] = key_value[0];
                hi[range_id] = key_value[0];
                if (entry_handle->key_field_ranges[key_index])
                {
                    hi[range_id] += entry_handle->key_field_ranges[key_index] - 1;
                }
            }
            range_id++;
        }
    }
    nof_ranges = range_id;

    
    for (range_id = 0; range_id < nof_ranges; range_id++)
    {
        DNX_KBP_TRY(kbp_entry_add_range(db_p, db_entry, lo[range_id], hi[range_id], range_id));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_lpm_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *payload_size)
{
    int res;
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    
    ad_db_p = (DNX_KBP_USE_OPTIMIZED_RESULT || kbp_handles->ad_db_zero_size_p) ?
        NULL : kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];

    res = dnx_kbp_entry_lpm_get(unit,
                                kbp_handles->db_p,
                                ad_db_p,
                                NULL,
                                entry_handle->phy_entry.key,
                                entry_handle->table->key_size,
                                entry_handle->phy_entry.payload,
                                entry_handle->table->max_payload_size,
                                entry_handle->phy_entry.prefix_length,
                                &(entry_handle->phy_entry.hitbit), kbp_handles, payload_size);

    if (res == _SHR_E_NOT_FOUND)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get from"));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_lpm_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    int is_cache_enabled = FALSE;
    int cur_res_type = entry_handle->cur_res_type;
    uint32 access_bmp_indication = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);

    
    if (DNX_KBP_USE_OPTIMIZED_RESULT &&
        (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[0] ==
         DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE) &&
        SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        
        entry_handle->table->multi_res_info[cur_res_type].entry_payload_size = kbp_handles->opt_result_size;
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];

        if (kbp_handles->ad_db_zero_size_p)
        {
            uint8 all_zeros[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
            uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };
            uint32 payload_size = entry_handle->table->multi_res_info[cur_res_type].entry_payload_size;

            SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, payload_size, entry_handle->phy_entry.payload, asso_data));

            
            payload_size = (BITS2BYTES(payload_size));
            if (!sal_memcmp(all_zeros, asso_data, payload_size))
            {
                
                entry_handle->table->multi_res_info[cur_res_type].entry_payload_size = 0;
                ad_db_p = kbp_handles->ad_db_zero_size_p;
            }
        }
    }
    else
    {
        
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
    }

#if 0
    
    if (dbal_image_name_is_std_1(unit))
    {
        uint32 is_default = FALSE;
        
        dbal_table_field_info_t table_field_info;
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, entry_handle->table_id, DBAL_FIELD_IS_DEFAULT, 0, cur_res_type, 0, &table_field_info));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, DBAL_FIELD_IS_DEFAULT, entry_handle->phy_entry.payload, &is_default));
        if (is_default)
        {
            access_bmp_indication |= DNX_KBP_ACCESS_BMP_INDICATION_DEFAULT_ENTRY;
        }
    }
#endif

    
    SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_get(unit, FALSE, &is_cache_enabled));
    if (is_cache_enabled)
    {
        access_bmp_indication |= DNX_KBP_ACCESS_BMP_INDICATION_CACHING_ENABLED;
    }

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "add to"));

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_add(unit,
                                          kbp_handles->db_p,
                                          ad_db_p,
                                          NULL,
                                          entry_handle->phy_entry.key,
                                          entry_handle->table->key_size,
                                          entry_handle->phy_entry.payload,
                                          entry_handle->table->multi_res_info[cur_res_type].entry_payload_size,
                                          entry_handle->phy_entry.prefix_length,
                                          &(entry_handle->phy_entry.indirect_commit_mode),
                                          entry_handle->phy_entry.hitbit, access_bmp_indication, kbp_handles));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_lpm_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    int is_cache_enabled = FALSE;
    uint32 access_bmp_indication = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "delete from"));

    
    ad_db_p = (DNX_KBP_USE_OPTIMIZED_RESULT ? NULL :
               kbp_handles->ad_db_zero_size_p ? NULL : kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]);

    
    SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_get(unit, FALSE, &is_cache_enabled));
    if (is_cache_enabled)
    {
        access_bmp_indication |= DNX_KBP_ACCESS_BMP_INDICATION_CACHING_ENABLED;
    }

    SHR_IF_ERR_EXIT(dnx_kbp_entry_lpm_delete(unit,
                                             kbp_handles->db_p,
                                             ad_db_p,
                                             NULL,
                                             entry_handle->phy_entry.key,
                                             entry_handle->table->key_size,
                                             entry_handle->phy_entry.prefix_length,
                                             access_bmp_indication, kbp_handles, NULL));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_acl_exists(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table))
    {
        SHR_IF_ERR_EXIT_NO_MSG(dbal_kbp_access_id_by_key_get(unit, entry_handle));
    }
    else if ((entry_handle->access_id_set == 0))
    {
        
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_acl_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *payload_size)
{
    kbp_db_t_p db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    struct kbp_entry *db_entry = NULL;
    struct kbp_entry_info entry_info;
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES] = { 0 };
    uint32 inverted_mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    kbp_status kbp_rv;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_KBP_ACCESS_ACL_DB_ENTRY_GET(unit, entry_handle, db_entry);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    db_p = kbp_handles->db_p;

    kbp_rv = kbp_entry_get_info(db_p, db_entry, &entry_info);
    if (dnx_kbp_error_translation(kbp_rv) != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if (entry_info.active == 0)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    sal_memcpy(data, entry_info.data, sizeof(uint8) * (entry_info.width_8));
    sal_memcpy(mask, entry_info.mask, sizeof(uint8) * (entry_info.width_8));
    DNX_KBP_TRY(kbp_ad_db_get(entry_info.ad_db, entry_info.ad_handle, asso_data));

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, entry_handle->table->key_size, data, entry_handle->phy_entry.key));
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, entry_handle->table->key_size, mask, inverted_mask));

    
    {
        uint32 key_offset =
            entry_handle->table->keys_info[entry_handle->table->nof_key_fields - 1].bits_offset_in_buffer;
        int size = entry_handle->table->key_size - key_offset;

        SHR_BITNEGATE_RANGE(inverted_mask, key_offset, size, entry_handle->phy_entry.k_mask);
    }

    
    if (entry_handle->table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, entry_handle->table->max_payload_size, payload_size));
    }
    else
    {
        *payload_size = entry_handle->table->max_payload_size;
    }
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, *payload_size, asso_data, entry_handle->phy_entry.payload));

    
    if (entry_info.ad_db == kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED])
    {
        *payload_size = kbp_handles->opt_result_size;
    }

    entry_handle->phy_entry.prefix_length = entry_info.prio_len;

    
    SHR_IF_ERR_EXIT(dbal_kbp_entry_get_ranges(unit, entry_handle, &entry_info));

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get from"));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_acl_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_handles_t *kbp_handles = NULL;
    int is_cache_enabled = FALSE;

    struct kbp_entry *db_entry = (struct kbp_entry *) INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
    struct kbp_entry_info entry_info;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_KBP_ACCESS_ACL_DB_ENTRY_GET(unit, entry_handle, db_entry);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);

    SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "delete from"));

    DNX_KBP_TRY(kbp_entry_get_info(kbp_handles->db_p, db_entry, &entry_info));

    DNX_KBP_TRY(kbp_db_delete_entry(kbp_handles->db_p, db_entry));

    
    SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_get
                    (unit, !DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table), &is_cache_enabled));

    
    if (!is_cache_enabled)
    {
        DNX_KBP_TRY(kbp_db_install(kbp_handles->db_p));
        if (entry_info.ad_db != kbp_handles->ad_db_zero_size_p)
        {
            
            DNX_KBP_TRY(kbp_ad_db_delete_entry(entry_info.ad_db, entry_info.ad_handle));
        }
    }

    
    if (DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table))
    {
        SHR_IF_ERR_EXIT(dbal_kbp_entry_access_id_destroy(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_acl_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    uint32 priority,
    uint32 key_size,
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES],
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES],
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES],
    uint8 *add_entry)
{
    shr_error_e rv = _SHR_E_NONE;
    struct kbp_entry *db_entry = NULL;
    struct kbp_entry_info entry_info = { 0 };
    uint8 entry_found = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    

    rv = dbal_kbp_entry_acl_exists(unit, entry_handle);

    if (rv == _SHR_E_NOT_FOUND)
    {
        entry_found = FALSE;
        if (entry_handle->phy_entry.indirect_commit_mode == DBAL_INDIRECT_COMMIT_MODE_UPDATE)
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    db_entry = (struct kbp_entry *) INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);

    
    entry_handle->phy_entry.indirect_commit_mode = DBAL_INDIRECT_COMMIT_MODE_UPDATE;

    
    if (entry_found)
    {
        
        DNX_KBP_TRY(kbp_entry_get_info(db_p, db_entry, &entry_info));

        
        if (DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table))
        {
            
            if (ad_db_p == entry_info.ad_db)
            {
                
                DNX_KBP_TRY(kbp_ad_db_update_entry(ad_db_p, entry_info.ad_handle, asso_data));
                *add_entry = FALSE;
            }
            else
            {
                
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Attempting to update an entry payload located in different AD DB, which could cause traffic loss\n");
            }
        }
        
        else
        {
            
            if (priority != entry_info.prio_len)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different priority\n");
            }
            
            if (sal_memcmp(data, entry_info.data, BITS2BYTES(key_size)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different key\n");
            }
            
            if (sal_memcmp(mask, entry_info.mask, BITS2BYTES(key_size)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Attempting to update an entry using different key mask\n");
            }
            
            DNX_KBP_TRY(kbp_ad_db_update_entry(ad_db_p, entry_info.ad_handle, asso_data));
            *add_entry = FALSE;
        }
    }
    
    else
    {
        *add_entry = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_kbp_entry_acl_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int kbp_ret_val;
    int is_cache_enabled = FALSE;
    kbp_db_t_p db_p = NULL;
    kbp_ad_db_t_p ad_db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;
    struct kbp_entry *db_entry = NULL;
    struct kbp_ad *ad_entry = NULL;
    uint8 data[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 mask[DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES] = { 0 };
    uint8 asso_data[DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES] = { 0 };
    uint32 inverted_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 payload_size;
    int cur_res_type = entry_handle->cur_res_type;
    uint8 add_entry = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    db_p = kbp_handles->db_p;
    
    if (DNX_KBP_USE_OPTIMIZED_RESULT && DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table)
        && (entry_handle->table->multi_res_info[cur_res_type].result_type_hw_value[0] ==
            DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE))
    {
        
        entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size =
            kbp_handles->opt_result_size;
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED];
    }
    else
    {
        ad_db_p = kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR];
    }

    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, entry_handle->table->key_size, entry_handle->phy_entry.key, data));
    
    SHR_BITNEGATE_RANGE(entry_handle->phy_entry.k_mask, 0, entry_handle->table->key_size, inverted_mask);
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, entry_handle->table->key_size, inverted_mask, mask));

    
    if (entry_handle->table_id >= DBAL_NOF_TABLES)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, entry_handle->table->max_payload_size, &payload_size));
    }
    else
    {
        payload_size = entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;
    }
    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(unit, payload_size, entry_handle->phy_entry.payload, asso_data));

    
    if (entry_handle->phy_entry.indirect_commit_mode != DBAL_INDIRECT_COMMIT_MODE_NORMAL)
    {
        SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_update(unit, entry_handle, db_p, ad_db_p,
                                                  entry_handle->phy_entry.prefix_length,
                                                  entry_handle->table->key_size, data, mask, asso_data, &add_entry));
    }

    if (add_entry)
    {
        
        if (entry_handle->access_id_set == 0)
        {
            DNX_KBP_TRY(kbp_db_add_ace(db_p, data, mask, entry_handle->phy_entry.prefix_length, &db_entry));
        }
        else
        {
            
            DNX_KBP_TRY(kbp_db_add_ace_with_index
                        (db_p, data, mask, entry_handle->phy_entry.prefix_length, entry_handle->phy_entry.entry_hw_id));
            db_entry = INT_TO_PTR(entry_handle->phy_entry.entry_hw_id);
        }

        
        SHR_IF_ERR_EXIT(dbal_kbp_entry_add_ranges(unit, entry_handle, db_p, db_entry));

        SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "add to"));

        kbp_ret_val = kbp_ad_db_add_entry(ad_db_p, asso_data, &ad_entry);
        if (dnx_kbp_error_translation(kbp_ret_val) != _SHR_E_NONE)
        {
            
            kbp_db_delete_entry(db_p, db_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(kbp_ret_val));
        }

        kbp_ret_val = kbp_entry_add_ad(db_p, db_entry, ad_entry);
        if (dnx_kbp_error_translation(kbp_ret_val) != _SHR_E_NONE)
        {
            
            kbp_db_delete_entry(db_p, db_entry);
            kbp_ad_db_delete_entry(ad_db_p, ad_entry);
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_entry_add_ad failed: %s\n", kbp_get_status_string(kbp_ret_val));
        }

        
        SHR_IF_ERR_EXIT(dnx_kbp_caching_enabled_get
                        (unit, !DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table), &is_cache_enabled));

        
        if (!is_cache_enabled)
        {
            kbp_ret_val = kbp_db_install(db_p);
            if (dnx_kbp_error_translation(kbp_ret_val) != _SHR_E_NONE)
            {
                
                kbp_db_delete_entry(db_p, db_entry);
                kbp_ad_db_delete_entry(ad_db_p, ad_entry);

                if (dnx_kbp_error_translation(kbp_ret_val) == _SHR_E_FULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FULL, "Table is full, %s\n", kbp_get_status_string(kbp_ret_val));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "kbp_db_install failed: %s\n", kbp_get_status_string(kbp_ret_val));
                }
            }
        }

        if (entry_handle->access_id_set == 0)
        {
            void *tmp;
            
            tmp = (void *) (db_entry);
            entry_handle->phy_entry.entry_hw_id = PTR_TO_INT(tmp);
        }

        
        if (DBAL_ACTIONS_IS_TCAM_BY_KEY(entry_handle->table))
        {
            int rv;
            rv = dbal_kbp_entry_access_id_create(unit, entry_handle, entry_handle->phy_entry.entry_hw_id);
            if (rv != _SHR_E_NONE)
            {
                
                kbp_db_delete_entry(db_p, db_entry);
                kbp_ad_db_delete_entry(ad_db_p, ad_entry);
                SHR_SET_CURRENT_ERR(_SHR_E_FULL);
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    shr_error_e res;
    uint32 payload_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    KBP_COMPILATION_CHECK;

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        {
            
            entry_handle->phy_entry.prefix_length = entry_handle->phy_entry.key_size;
            res = dbal_kbp_entry_lpm_get(unit, entry_handle, &payload_size);
            break;
        }
        case DBAL_TABLE_TYPE_LPM:
        {
            res = dbal_kbp_entry_lpm_get(unit, entry_handle, &payload_size);
            break;
        }
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
        {
            res = dbal_kbp_entry_acl_get(unit, entry_handle, &payload_size);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table type %s not supported for KBP\n",
                         dbal_table_type_to_string(unit, entry_handle->table->table_type));
        }
    }

    if (res != _SHR_E_NONE)
    {
        if (res == _SHR_E_NOT_FOUND)
        {
            
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(res, "entry get failed internal error table %s\n", entry_handle->table->table_name);
        }
    }

    SHR_IF_ERR_EXIT(dbal_kbp_res_type_align(unit, entry_handle, payload_size));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_entry_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        {
            
            entry_handle->phy_entry.prefix_length = entry_handle->phy_entry.key_size;
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_add(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_LPM:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_add(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_add(unit, entry_handle));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table type %d not supported for KBP\n",
                         entry_handle->table->table_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_entry_delete(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        {
            
            entry_handle->phy_entry.prefix_length = entry_handle->phy_entry.key_size;
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_delete(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_LPM:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_lpm_delete(unit, entry_handle));
            break;
        }
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_BY_ID:
        {
            SHR_IF_ERR_EXIT(dbal_kbp_entry_acl_delete(unit, entry_handle));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table type %d not supported for KBP\n",
                         entry_handle->table->table_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_t_p db_p = NULL;
    kbp_db_handles_t *kbp_handles = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    db_p = kbp_handles->db_p;

    {
        DNX_KBP_TRY(kbp_db_delete_all_entries(db_p));
        if (kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR] != NULL)
        {
            DNX_KBP_TRY(kbp_ad_db_delete_all_entries(kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]));
        }
        if (DNX_KBP_USE_OPTIMIZED_RESULT && (kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED] != NULL))
        {
            DNX_KBP_TRY(kbp_ad_db_delete_all_entries(kbp_handles->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]));
        }
        DNX_KBP_TRY(kbp_db_install(db_p));
    }

    
    if (entry_handle->table_id == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.clear(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV4_MC));
    }
    else if (entry_handle->table_id == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD)
    {
        SHR_IF_ERR_EXIT(KBP_FWD_TCAM_ACCESS.key_2_entry_id_hash.clear(unit, DNX_KBP_TCAM_HASH_TABLE_INDEX_IPV6_MC));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_handles_t *kbp_handles = NULL;
    dbal_physical_entry_iterator_t *kbp_iterator;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    kbp_iterator = &iterator_info->mdb_iterator;

    sal_memset(kbp_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));
    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);

    kbp_iterator->kbp_lpm_db_p = kbp_handles->db_p;
    DNX_KBP_TRY(kbp_db_entry_iter_init(kbp_iterator->kbp_lpm_db_p, &(kbp_iterator->kbp_lpm_iter)));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    kbp_db_handles_t *kbp_handles = NULL;
    dbal_physical_entry_iterator_t *kbp_iterator;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    kbp_iterator = &(iterator_info->mdb_iterator);

    DNX_KBP_HANDLES_GET(entry_handle->table, kbp_handles);
    kbp_iterator->kbp_lpm_db_p = kbp_handles->db_p;
    DNX_KBP_TRY(kbp_db_entry_iter_destroy(kbp_iterator->kbp_lpm_db_p, kbp_iterator->kbp_lpm_iter));
    sal_memset(kbp_iterator, 0x0, sizeof(dbal_physical_entry_iterator_t));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_table_entry_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;
    struct kbp_entry *kpb_entry;
    struct kbp_entry_info entry_info;
    struct kbp_entry_iter *iterator_p;
    kbp_db_t_p db_p;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_physical_entry_t *phy_entry = &(entry_handle->phy_entry);
    uint32 inverted_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint8 payload_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    db_p = iterator_info->mdb_iterator.kbp_lpm_db_p;
    
    iterator_p = iterator_info->mdb_iterator.kbp_lpm_iter;

    DNX_KBP_TRY(kbp_db_entry_iter_next(db_p, iterator_p, &kpb_entry));

    if (kpb_entry == NULL)
    {
        
        iterator_info->is_end = TRUE;
    }
    else
    {
        DNX_KBP_TRY(kbp_entry_get_info(db_p, kpb_entry, &entry_info));

        if (kpb_entry != NULL)
        {
            if (entry_info.ad_handle != NULL)
            {
                uint8 asso_data[DBAL_PHYSICAL_RES_SIZE_IN_BYTES] = { 0 };

                DNX_KBP_TRY(kbp_ad_db_get(entry_info.ad_db, entry_info.ad_handle, asso_data));

                sal_memset(phy_entry, 0x0, sizeof(*phy_entry));

                
                phy_entry->key_size = table->key_size;
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, table->key_size, entry_info.data, phy_entry->key));
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal(unit, table->key_size, entry_info.mask, inverted_mask));
                
                SHR_BITNEGATE_RANGE(inverted_mask, 0, table->key_size, phy_entry->k_mask);

                
                phy_entry->payload_size = table->max_payload_size;
                SHR_IF_ERR_EXIT(dnx_kbp_buffer_kbp_to_dbal
                                (unit, table->max_payload_size, asso_data, phy_entry->payload));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(phy_entry->p_mask, 0, table->max_payload_size - 1));
                phy_entry->prefix_length = entry_info.prio_len;

                phy_entry->entry_hw_id = PTR_TO_INT(kpb_entry);

                SHR_IF_ERR_EXIT(dbal_kbp_entry_get_ranges(unit, entry_handle, &entry_info));

                
                payload_size = table->max_payload_size;
                if (entry_info.ad_db ==
                    ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED])
                {
                    payload_size = ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->opt_result_size;
                }
                
                else if (entry_info.ad_db == ((kbp_db_handles_t *) entry_handle->table->kbp_handles)->ad_db_zero_size_p)
                {
                    payload_size = 0;
                    entry_handle->access_id_set = TRUE;
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBPSDK entry AD handle is null.\n");
            }
        }
    }

    if (!iterator_info->is_end)
    {
        SHR_IF_ERR_EXIT(dbal_kbp_res_type_align(unit, entry_handle, payload_size));

        SHR_IF_ERR_EXIT(dbal_kbp_physical_entry_print(unit, entry_handle, TRUE, "get next from"));
        iterator_info->entries_counter++;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_kbp_access_cache_commit(
    int unit,
    int is_acl)
{
    int db_id;
    kbp_db_handles_t op2_kbp_handles;
    uint8 is_allocated = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(KBP_COMMON_ACCESS.is_init(unit, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP manager is not initialized; Caching cannot be performed.");
    }
    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP is not enabled; Caching cannot be performed.");
    }

    
    for (db_id = 0; db_id < DNX_KBP_MAX_NOF_DBS; db_id++)
    {
        
        SHR_IF_ERR_EXIT(KBP_ACCESS.db_handles_info.get(unit, db_id, &op2_kbp_handles));
        if (DNX_KBP_ACCESS_PERFORM_CACHING(is_acl, op2_kbp_handles.caching_bmp))
        {
            
            int res = kbp_db_install(op2_kbp_handles.db_p);
            if (res != KBP_OK)
            {
                
                kbp_db_delete_all_pending_entries(op2_kbp_handles.db_p);
                if (op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR] != NULL)
                {
                    kbp_ad_db_delete_unused_entries(op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]);
                }
                if (DNX_KBP_USE_OPTIMIZED_RESULT && (op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED] != NULL))
                {
                    kbp_ad_db_delete_unused_entries(op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]);
                }

                if (dnx_kbp_error_translation(res))
                {
                    SHR_ERR_EXIT(_SHR_E_FULL, "Error: Table is full during cache commit, %s.\n",
                                 kbp_get_status_string(res));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: kbp_db_install failed during cache commit with: %s.\n",
                                 kbp_get_status_string(res));
                }
            }

            
            if (op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR] != NULL)
            {
                DNX_KBP_TRY(kbp_ad_db_delete_unused_entries(op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_REGULAR]));
            }
            if (DNX_KBP_USE_OPTIMIZED_RESULT && (op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED] != NULL))
            {
                DNX_KBP_TRY(kbp_ad_db_delete_unused_entries(op2_kbp_handles.ad_db_p[DNX_KBP_AD_DB_INDEX_OPTIMIZED]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#else


shr_error_e dbal_kbp_entry_get(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_entry_add(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_entry_delete(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_clear(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_iterator_init(int unit, dbal_entry_handle_t * entry_handle){ SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_entry_get_next(int unit, dbal_entry_handle_t * entry_handle) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n"); exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_table_iterator_deinit(int unit,dbal_entry_handle_t * entry_handle){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_res_type_resolution(int unit, dbal_entry_handle_t * entry_handle, uint8 is_payload_opt){SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_access_id_by_key_get(int unit,dbal_entry_handle_t * entry_handle) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}

shr_error_e dbal_kbp_access_cache_commit(int unit, int is_acl) {SHR_FUNC_INIT_VARS(unit);SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP SDK not compiled\n");exit:SHR_FUNC_EXIT;}


#endif 
