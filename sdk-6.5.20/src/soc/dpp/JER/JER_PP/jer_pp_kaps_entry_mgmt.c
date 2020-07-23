/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#include <soc/mem.h>
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <soc/mem.h>



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>






#define UINT64_SIZE_IN_BITS 64
#define UINT32_SIZE_IN_BITS 32
#define UINT8_SIZE_IN_BITS 8






#define JER_KAPS_KEY_INFO_DECODE(_unit, _key, _db_id, _offset, _word_off) \
    do { \
        if (SOC_IS_JERICHO_PLUS_ONLY(unit)) { \
            SHR_BITCOPY_RANGE(&db_id, 0, &key, 16, 5); \
            SHR_BITCOPY_RANGE(&offset, 0, &key, 2, 14); \
        } else if (SOC_IS_QUX(unit)) { \
            SHR_BITCOPY_RANGE(&db_id, 0, &key, 14, 4); \
            SHR_BITCOPY_RANGE(&offset, 0, &key, 2, 10); \
        } else if (SOC_IS_QAX(unit)) { \
            SHR_BITCOPY_RANGE(&db_id, 0, &key, 14, 4); \
            SHR_BITCOPY_RANGE(&offset, 0, &key, 2, 12); \
        } else { \
            SHR_BITCOPY_RANGE(&db_id, 0, &key, 14, 5); \
            SHR_BITCOPY_RANGE(&offset, 0, &key, 2, 12); \
        } \
        SHR_BITCOPY_RANGE(&word_off, 0, &key, 0, 2); \
    } while(0)










#ifdef CRASH_RECOVERY_SUPPORT
extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];
#endif






STATIC uint32
  jer_pp_kaps_data_buffer_to_kaps_payload_buffer_encode(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     *asso_data_buffer,
    SOC_SAND_IN  uint8                      nof_bytes,
    SOC_SAND_IN  uint32                     is_for_dma,
    SOC_SAND_OUT uint8                      *asso_data
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        fld_val;
    uint32 aligned_data_buffer[2];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    sal_memcpy(aligned_data_buffer, asso_data_buffer, nof_bytes );

    if (nof_bytes == JER_KAPS_AD_BUFFER_NOF_BYTES && !is_for_dma) 
    {
        aligned_data_buffer[0] = asso_data_buffer[0] << (32 - JER_KAPS_AD_WIDTH_IN_BITS); 
    }

    sal_memset(asso_data, 0x0, sizeof(uint8) * nof_bytes);

    for (byte_ndx = 0; byte_ndx < nof_bytes; ++byte_ndx)
    {
        byte_array_idx = nof_bytes - byte_ndx - 1;
        
        fld_val = 0;
        SHR_BITCOPY_RANGE(&fld_val, 0, aligned_data_buffer, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), SOC_SAND_NOF_BITS_IN_BYTE);
        asso_data[byte_array_idx] = (uint8) (fld_val & 0xFF);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_data_buffer_to_kaps_payload_buffer_encode()",0,0);
}



  uint32
    jer_pp_kaps_entry_add_hw(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_IN  uint32                      hitbit_enabled,
       SOC_SAND_OUT  uint8                      *data,
       SOC_SAND_OUT  uint8                      *assoData,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    )
{
    int32
        res;
    struct kbp_db 
       *db_p = NULL;
    struct kbp_ad_db 
       *ad_db_p = NULL;
    struct kbp_hb_db
       *hb_db_p = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    struct kbp_hb
       *hb_entry = NULL;
    int is_update = 0;

    SOCDNX_INIT_FUNC_DEFS;

    *success = SOC_SAND_SUCCESS;
#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        SOCDNX_SAND_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,TRUE));
    } else {
        SOCDNX_SAND_IF_ERR_EXIT(soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_kaps_kbp));
    }
#endif
    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOCDNX_NULL_CHECK(db_p);

    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOCDNX_NULL_CHECK(ad_db_p);

    if (SOC_IS_JERICHO_PLUS(unit) && (hitbit_enabled != 0)) {
        jer_kaps_hb_db_get(unit,
                           table_id,
                           &hb_db_p
            );

        SOCDNX_NULL_CHECK(hb_db_p);
    }

    
    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if (db_entry != NULL) {
        is_update = 1;
    }

    if (!is_update) {
        res = kbp_db_add_prefix(
                db_p, 
                data, 
                prefix_len,
                &db_entry);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
        }

        res = kbp_ad_db_add_entry(
                ad_db_p,
                assoData,
                &ad_entry
              );

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            kbp_db_delete_entry(db_p, db_entry); 
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
        }

        res = kbp_entry_add_ad(
                db_p,
                db_entry,
                ad_entry
              );

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            kbp_db_delete_entry(db_p, db_entry); 
            kbp_ad_db_delete_entry(ad_db_p, ad_entry); 
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
        }

        if (SOC_IS_JERICHO_PLUS(unit) && (hitbit_enabled != 0)) {
            res = kbp_hb_db_add_entry(hb_db_p, &hb_entry);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                kbp_db_delete_entry(db_p, db_entry); 
                kbp_ad_db_delete_entry(ad_db_p, ad_entry); 
                *success = SOC_SAND_FAILURE_INTERNAL_ERR;
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
            }

            res = kbp_entry_add_hb(
                    db_p,
                    db_entry,
                    hb_entry
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                kbp_db_delete_entry(db_p, db_entry); 
                kbp_ad_db_delete_entry(ad_db_p, ad_entry); 
                kbp_hb_db_delete_entry(hb_db_p, hb_entry); 
                *success = SOC_SAND_FAILURE_INTERNAL_ERR;
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
            }
        }
    }
    else {
        res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
        }

        res = kbp_ad_db_update_entry(ad_db_p, ad_entry, assoData);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
        }
    }

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        dcmn_cr_info[unit]->kaps_dirty = 1;
        dcmn_cr_info[unit]->kaps_tbl_id = table_id;
    } else
#endif
    {
#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("jer_pp_kaps_entry_add_hw", 6);
#endif 
        res = kbp_db_install(db_p);
#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(6);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            kbp_db_delete_entry(db_p, db_entry); 
            kbp_ad_db_delete_entry(ad_db_p, ad_entry); 
            LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Entry add : kbp_db_install failed with: %s!\n"),
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
        if(res == KBP_OUT_OF_DBA || res == KBP_OUT_OF_UIT || res == KBP_OUT_OF_UDA || res == KBP_OUT_OF_AD){
                *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
                SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("KAPS is full")));
            }
            else{
                *success = SOC_SAND_FAILURE_INTERNAL_ERR;
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("internal error occured")));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

  uint32
    jer_pp_kaps_entry_remove_hw(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_IN  uint32                      hitbit_enabled,
       SOC_SAND_OUT uint8                       *data
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_hb_db
        *hb_db_p = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    struct kbp_hb
       *hb_entry = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        SOC_SAND_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,TRUE));
    } else {
        SOC_SAND_IF_ERR_EXIT(soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_kaps_kbp));
    }
#endif
    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    
    res = kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(db_entry, 40, exit);

    
    res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(ad_entry, 60, exit);

    if (SOC_IS_JERICHO_PLUS(unit) && (hitbit_enabled != 0)) {
        
        res = kbp_entry_get_hb(db_p, db_entry, &hb_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 63, exit);
        }
        SOC_SAND_CHECK_NULL_PTR(hb_entry, 66, exit);
    }

    res = kbp_db_delete_entry( 
            db_p,
            db_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
    }

    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    res = kbp_ad_db_delete_entry(
            ad_db_p, 
            ad_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_ad_db_delete_entry failed with error: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

    if (SOC_IS_JERICHO_PLUS(unit) && (hitbit_enabled != 0)) {
        jer_kaps_hb_db_get(unit,
                           table_id, 
                           &hb_db_p
            );

        SOC_SAND_CHECK_NULL_PTR(hb_db_p, 90, exit);

        res = kbp_hb_db_delete_entry(
                hb_db_p, 
                hb_entry
              );
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): kbp_hb_db_delete_entry failed with error: %s!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        }
    }

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        dcmn_cr_info[unit]->kaps_dirty = 1;
        dcmn_cr_info[unit]->kaps_tbl_id = table_id;
    } else
#endif
    {
#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("jer_pp_kaps_entry_delete_hw", 7);
#endif 
        res = kbp_db_install(db_p);
#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(7);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): kbp_ad_db_delete_entry failed with error: %s!\n"), 
                                  FUNCTION_NAME(),
                       kbp_get_status_string(res)));
             SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_remove_hw()",0,0);
}


uint32 jer_pp_kaps_get_hit_bit(
         SOC_SAND_IN    int                unit,
         SOC_SAND_IN    uint8              table_id,
         SOC_SAND_INOUT struct kbp_entry  *db_entry,
         SOC_SAND_IN    uint8              clear,
         SOC_SAND_OUT   uint8             *hit_bit
      )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_hb_db
       *hb_db_p = NULL;
    struct kbp_hb
       *hb_entry = NULL;
    uint32
        bit_value;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    jer_kaps_hb_db_get(unit,
                       table_id,
                       &hb_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(hb_db_p, 105, exit);

    res = kbp_entry_get_hb(db_p, db_entry, &hb_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
    }

    SOC_SAND_CHECK_NULL_PTR(hb_entry, 120, exit);

    
    res = kbp_hb_entry_get_bit_value(hb_db_p, hb_entry, &bit_value, clear);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 130, exit);
    }

    *hit_bit = bit_value & SOC_DPP_DBAL_HITBIT_FLAG_HIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_get_hit_bit()",0,0);
}

  uint32
    jer_pp_kaps_entry_get_hw(
       SOC_SAND_IN    int         unit,
       SOC_SAND_IN    uint8       table_id,
       SOC_SAND_IN    uint32      prefix_len,
       SOC_SAND_IN    uint32      hitbit_enabled,
       SOC_SAND_OUT   uint8       *data,
       SOC_SAND_OUT   uint8       *assoData,
       SOC_SAND_INOUT uint8       *hit_bit,
       SOC_SAND_OUT   uint8       *found
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_ad 
        *ad_entry = NULL;
    struct kbp_entry
        *db_entry = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = FALSE;

    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    
    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if (!db_entry)
        goto exit;

    
    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    SOC_SAND_CHECK_NULL_PTR(ad_entry, 90, exit);

    sal_memset(assoData, 0x0, sizeof(uint8) * JER_KAPS_AD_BUFFER_NOF_BYTES);

    res = kbp_ad_db_get(ad_db_p, ad_entry, assoData);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

    if (SOC_IS_JERICHO_PLUS(unit) && (hitbit_enabled != 0)) {
        uint8 clear_on_read;
        
        if (hit_bit == NULL) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 110, exit);
        }        
        clear_on_read = *hit_bit & SOC_DPP_DBAL_HITBIT_FLAG_CLEAR ? 1 : 0;

        res = jer_pp_kaps_get_hit_bit(unit, table_id, db_entry, clear_on_read, hit_bit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }

    *found = TRUE;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_get_hw()",0,0);
}

  uint32
    jer_pp_kaps_entry_get_block_hw(
       SOC_SAND_IN    int                             unit,
       SOC_SAND_IN    SOC_DPP_DBAL_SW_TABLE_IDS       dbal_table_id,
       SOC_SAND_INOUT uint32                          vrf_ndx,
       SOC_SAND_IN    SOC_DPP_DBAL_TABLE_INFO        *dbal_table,
       SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE *block_range_key,
       SOC_SAND_OUT   SOC_PPC_FP_QUAL_VAL            *qual_vals_array,
       SOC_SAND_OUT   void                           *payload,
       SOC_SAND_OUT   uint32                         *nof_entries
    )
{
    struct kbp_entry_iter *iter=NULL;
    struct kbp_entry *kpb_e = NULL;
    struct kbp_entry_info kpb_e_info;

    struct kbp_ad_db *ad_db_p = NULL;

    uint8   ad_bytes_array[JER_KAPS_AD_BUFFER_NOF_BYTES];

    uint32  entry_key[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint32  entry_mask[JER_KAPS_KEY_BUFFER_NOF_BYTES];


    int entry_index = -1;
    int vrf_qual_index;
    uint32 res;
    JER_KAPS_DB *tbl_p=NULL;
    JER_KAPS_IP_TBL_ID kaps_table_id;
    int vrf_mode = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if(vrf_ndx == -1){
        vrf_ndx = 1;         
    }else{
        vrf_mode = 1;
    }

    res = jer_pp_kaps_dbal_table_id_translate(unit, dbal_table_id, NULL , vrf_ndx, &kaps_table_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (kaps_table_id < JER_KAPS_IP_NOF_TABLES) {
        jer_kaps_db_get(unit, kaps_table_id, &tbl_p);
        jer_kaps_ad_db_get(unit, kaps_table_id, &ad_db_p);
    } else {
        tbl_p = NULL;
        ad_db_p = NULL;
    }

    KBP_TRY(kbp_db_entry_iter_init(tbl_p, &iter));

    do {
        ++entry_index;
        res = kbp_db_entry_iter_next(tbl_p, iter, &kpb_e);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
    }
    while ( kpb_e && (entry_index < block_range_key->start.payload.arr[0]));

    do {
        if (kpb_e == NULL)
            break;

        res = kbp_entry_get_info(tbl_p, kpb_e, &kpb_e_info);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
        }

        
        soc_sand_os_memset(entry_key, 0, sizeof(uint32) * (JER_KAPS_KEY_BUFFER_NOF_BYTES));

        
        soc_sand_os_memset(entry_mask, 0, sizeof(uint32) * (JER_KAPS_KEY_BUFFER_NOF_BYTES));

        
        DBAL_QUAL_VALS_CLEAR(qual_vals_array + (entry_index - block_range_key->start.payload.arr[0])*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        res = arad_pp_dbal_kbp_buffer_to_entry_key(unit, dbal_table, kpb_e_info.prio_len, kpb_e_info.data, qual_vals_array + (entry_index - block_range_key->start.payload.arr[0])*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

        vrf_qual_index = 0;
        while (qual_vals_array[(entry_index - block_range_key->start.payload.arr[0])*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + vrf_qual_index].type != SOC_PPC_FP_QUAL_IRPP_VRF) {
            vrf_qual_index++;
            if (vrf_qual_index == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                break;
            }
        }

        if ((vrf_mode) && (vrf_qual_index == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)){
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
        }

        
        if ((qual_vals_array[(entry_index - block_range_key->start.payload.arr[0])*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + vrf_qual_index].val.arr[0] == vrf_ndx) || (vrf_mode == 0))
        {
            res = kbp_ad_db_get(ad_db_p, kpb_e_info.ad_handle, ad_bytes_array);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
            }

            
            jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, ad_bytes_array, (uint32 *)payload + (entry_index - block_range_key->start.payload.arr[0]));

            (*nof_entries)++;
        }

        entry_index++;

        res = kbp_db_entry_iter_next(tbl_p, iter, &kpb_e);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }

    } while ((entry_index < block_range_key->start.payload.arr[0] + block_range_key->entries_to_act));

    if((!kpb_e) || ((*nof_entries) < block_range_key->entries_to_act))
    {
        block_range_key->start.payload.arr[0] = SOC_SAND_UINT_MAX;
        block_range_key->start.payload.arr[1] = SOC_SAND_UINT_MAX;
    }
    else
    {
        block_range_key->start.payload.arr[0] = entry_index;
        block_range_key->start.payload.arr[1] = vrf_ndx;
    }
exit:
    
    res = kbp_db_entry_iter_destroy(tbl_p, iter);
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_get_block_hw()",0,0);
}


STATIC
  uint32
    jer_pp_kaps_entry_table_clear_hw_long_prefix(
       SOC_SAND_IN  int         unit,
       SOC_SAND_IN  uint8       table_id,
       SOC_SAND_IN  uint32      prefix_len,
       SOC_SAND_IN  uint32      prefix,
       SOC_SAND_IN  uint32      hit_bit_enabled
    )
{
    uint32
        res;
    struct kbp_db
        *db_p = NULL;
    struct kbp_ad_db
        *ad_db_p = NULL;
    struct kbp_hb_db
        *hb_db_p = NULL;
    struct kbp_entry
        *db_entry = NULL;
    struct kbp_entry_iter
        *iter_p = NULL;
    struct kbp_entry_info
        kpb_e_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        SOC_SAND_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,TRUE));
    } else {
        SOC_SAND_IF_ERR_EXIT(soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_kaps_kbp));
    }
#endif
 
    jer_kaps_db_get(unit,
                    table_id,
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    
    jer_kaps_ad_db_get(unit,
                       table_id,
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    if (SOC_IS_JERICHO_PLUS(unit) && (hit_bit_enabled != 0)) {
        jer_kaps_hb_db_get(unit,
                           table_id, 
                           &hb_db_p
            );

        SOC_SAND_CHECK_NULL_PTR(hb_db_p, 35, exit);
    }

    res = kbp_db_entry_iter_init(db_p, &iter_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(iter_p, 90, exit);

    res = kbp_db_entry_iter_next(db_p, iter_p, &db_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    while (db_entry != NULL) {
        res = kbp_entry_get_info(db_p, db_entry, &kpb_e_info);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        }

        
        if (kpb_e_info.data[0] >> (SOC_SAND_NOF_BITS_IN_BYTE - prefix_len) == prefix) {
            
            res = jer_pp_kaps_entry_remove_hw(unit, table_id, kpb_e_info.prio_len, hit_bit_enabled, kpb_e_info.data);
            if (res != SOC_E_NONE) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 75, exit);
            }

            
            res = kbp_db_entry_iter_destroy(db_p, iter_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
            }

            res = kbp_db_entry_iter_init(db_p, &iter_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
            }
            SOC_SAND_CHECK_NULL_PTR(iter_p, 90, exit);
        }

        res = kbp_db_entry_iter_next(db_p, iter_p, &db_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
    }

    res = kbp_db_entry_iter_destroy(db_p, iter_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        dcmn_cr_info[unit]->kaps_dirty = 1;
        dcmn_cr_info[unit]->kaps_tbl_id = table_id;
    }else
#endif
    {
        res = kbp_db_install(db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): jer_pp_kaps_entry_table_clear_hw_long_prefix failed with error: %s!\n"), 
                                  FUNCTION_NAME(),
                       kbp_get_status_string(res)));
             SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear_hw_long_prefix()",0,0);
}

uint32
    jer_pp_kaps_dma_clear_db(
        SOC_SAND_IN int       unit,
        SOC_SAND_IN uint32    db_id
    )
{
    uint32 res = SOC_SAND_OK;
    struct kbp_dma_db 
       *db_dma_p = NULL;
    uint8
        entry_data[BITS2BYTES(SOC_DPP_IMP_DEFS_MAX(KAPS_DMA_LINE_WIDTH))];
    uint32 offset;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_dma_db_get(unit,
                        db_id, 
                        &db_dma_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

    sal_memset(entry_data, 0, sizeof(entry_data));

    for (offset = 0; offset < JER_KAPS_DMA_DB_NOF_PMF_ENTRIES; offset++) {
        res = jer_pp_xpt_wait_dma_done(unit);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }

        res = kbp_dma_db_add_entry(db_dma_p,
                                   entry_data,
                                   offset);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }

        res = jer_pp_xpt_wait_dma_done(unit);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_dma_clear_db()",0,0);
}

STATIC
  uint32
    jer_pp_kaps_entry_table_clear_hw(
       SOC_SAND_IN  int         unit,
       SOC_SAND_IN  uint8       table_id,
       SOC_SAND_IN  uint32      hit_bit_enabled
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        SOC_SAND_IF_ERR_EXIT(jer_kaps_cr_transaction_cmd(unit,TRUE));
    } else {
        SOC_SAND_IF_ERR_EXIT(soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_kaps_kbp));
    }
#endif

    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);
    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );
    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    res = kbp_db_delete_all_entries(db_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    res = kbp_ad_db_delete_unused_entries(ad_db_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

    

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        dcmn_cr_info[unit]->kaps_dirty = 1;
        dcmn_cr_info[unit]->kaps_tbl_id = table_id;
    }else
#endif
    {
        res = kbp_db_install(db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): jer_pp_kaps_entry_table_clear_hw failed with error: %s!\n"), 
                                  FUNCTION_NAME(),
                       kbp_get_status_string(res)));
             SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear_hw()",0,0);
}


void jer_pp_kaps_key_encode(SOC_SAND_IN    uint8 tbl_prefix,
                           SOC_SAND_IN    uint8 nof_quals,
                           SOC_SAND_IN    uint64 *qual_vals,
                           SOC_SAND_IN    uint8 *qual_nof_bits,
                           SOC_SAND_OUT   uint8 *data_bytes)
{
    int32 
        bit_offset,
        data_idx,
        qual_idx, i;
    uint64 val, data[3] = {0};

    
    COMPILER_64_SET(data[0], 0, tbl_prefix);
    COMPILER_64_SHL(data[0], UINT64_SIZE_IN_BITS - JER_KAPS_TBL_PREFIX_NOF_BITS);

    
    bit_offset = UINT64_SIZE_IN_BITS - JER_KAPS_TBL_PREFIX_NOF_BITS;
    data_idx = 0;
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {  
        if (qual_nof_bits[qual_idx] > bit_offset)
        {
            
            val = qual_vals[qual_idx];
            COMPILER_64_SHR(val,(qual_nof_bits[qual_idx] - bit_offset));
            data[data_idx] |= val; 
            data_idx++;
            bit_offset += UINT64_SIZE_IN_BITS; 
        }

        bit_offset -= qual_nof_bits[qual_idx]; 
        val = qual_vals[qual_idx];
        COMPILER_64_SHL(val, bit_offset);
        data[data_idx] |=  val; 
    }

    
    for (i = 0; i < JER_KAPS_KEY_BUFFER_NOF_BYTES; i++) {
        val = data[i/UINT8_SIZE_IN_BITS];
        COMPILER_64_SHR(val, (UINT64_SIZE_IN_BITS - ((i%8)+1)*8));
        data_bytes[i] = val;
    }
}

uint32
  jer_pp_kaps_route_to_kaps_payload_buffer_encode(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     *asso_data_buffer,
    SOC_SAND_OUT uint8                      *asso_data
  )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    res =  jer_pp_kaps_data_buffer_to_kaps_payload_buffer_encode(unit,asso_data_buffer, JER_KAPS_AD_BUFFER_NOF_BYTES, FALSE, asso_data);

    SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

#if 0 

    sal_memset(asso_data, 0x0, sizeof(uint8) * JER_KAPS_AD_BUFFER_NOF_BYTES);

    for (byte_ndx = 0; byte_ndx < JER_KAPS_AD_BUFFER_NOF_BYTES; ++byte_ndx)
    {
        byte_array_idx = JER_KAPS_AD_BUFFER_NOF_BYTES - byte_ndx - 1;
        
        fld_val = 0;
        SHR_BITCOPY_RANGE(&fld_val, 0, asso_data_buffer, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), SOC_SAND_NOF_BITS_IN_BYTE);
        asso_data[byte_array_idx] = (uint8) (fld_val & 0xFF);
    }
#endif 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_route_to_kaps_payload_buffer_encode()",0,0);
}


uint32
  jer_pp_kaps_dma_buffer_to_kaps_payload_buffer_encode(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     *asso_data_buffer,
    SOC_SAND_IN  uint32                     size,
    SOC_SAND_OUT uint8                      *asso_data
  )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    res =  jer_pp_kaps_data_buffer_to_kaps_payload_buffer_encode(unit, asso_data_buffer, size, TRUE, asso_data);

    SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_route_to_kaps_payload_buffer_encode()",0,0);
}

uint32
  jer_pp_kaps_payload_buffer_to_dma_buffer_decode(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                      *asso_data,
    SOC_SAND_IN  uint32                     size,
    SOC_SAND_OUT uint32                     *asso_data_buffer
  )
{
    uint32
        byte_ndx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(asso_data);
    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);

    for (byte_ndx = 0; byte_ndx < size; byte_ndx++) {
      asso_data_buffer[ byte_ndx / 4] |= asso_data[byte_ndx] << ((3 - byte_ndx) * 8);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_payload_buffer_to_dma_buffer_decode()",0,0);
}

uint32
  jer_pp_kaps_route_to_kaps_payload_buffer_decode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint8                      *asso_data,
    SOC_SAND_OUT uint32                     *asso_data_buffer
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    
    *asso_data_buffer = asso_data[0] << 12 | asso_data[1] << 4 | asso_data[2] >> 4;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_route_to_kaps_payload_buffer_decode()",0,0);
}

uint32
  jer_pp_kaps_payload_to_mc_dest_id(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                  payload,
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID              *dest_id
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    SOC_SAND_CHECK_NULL_INPUT(dest_id);

    if (payload & (1 << 17)) {
        dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
        dest_id->dest_val = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_FEC_DECODE(payload));
    }
    else if (payload & (1 << 18)) {
        dest_id->dest_type = SOC_SAND_PP_DEST_MULTICAST;
        dest_id->dest_val = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_MC_DECODE(payload));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_payload_to_mc_dest_id()",0,0);
}


uint32 
  jer_pp_kaps_dbal_table_id_translate(SOC_SAND_IN  int                       unit, 
                                      SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS table_id, 
                                      SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL       qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                      SOC_SAND_IN  uint8                     private_table, 
                                      SOC_SAND_OUT JER_KAPS_IP_TBL_ID       *kaps_table_id)
{
    uint32 vrf = 0;
    int i;
    uint32 inrif = 0;
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;
 
    if (qual_vals == NULL) {
        
        vrf = private_table;
        inrif = private_table;
    } else {
        
        for (i=0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
            if (qual_vals[i].type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                vrf = qual_vals[i].val.arr[0];
                break;
            }
        }

        
        if (i == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
            vrf = 1;
        }

        
        for (i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
            if (qual_vals[i].type == SOC_PPC_FP_QUAL_IRPP_IN_RIF) {
                inrif = qual_vals[i].val.arr[0];
                break;
            }
        }

        
        if (i == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
            inrif = 0;
        }
    }

    
    if(ARAD_KBP_IS_OP_OR_OP2) {
        switch(table_id) {
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS: 
            if(!ARAD_KBP_ENABLE_IPV4_UC_PUBLIC) {
                vrf = 1;
            }
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS: 
            if(!ARAD_KBP_ENABLE_IPV6_UC_PUBLIC) {
                vrf = 1;
            }
            break;
        default: 
            vrf = 1;
        }
    }
    
    else if(ARAD_KBP_IS_IN_USE) {
        vrf = 1;
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    if (((vrf == 0) && JER_KAPS_ENABLE_PUBLIC_DB(unit)) || !JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
        
        switch (table_id) {
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV6_MC_TBL_ID;
            break;
        default: 
            if((table.db_prefix >> (table.db_prefix_len - JER_KAPS_TABLE_PREFIX_LENGTH)) == JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX) {
                *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID;
            } else {
                *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID;
            }
        }
    } else {
        
        switch (table_id) {
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC:
            *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV6_MC_TBL_ID;
            break;
        default: 
            if((table.db_prefix >> (table.db_prefix_len - JER_KAPS_TABLE_PREFIX_LENGTH)) == JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX) {
                *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID;
            } else {
                *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID;
            }
        }
    }

    
    if ((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS)
         && (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1)){
        if ((JER_KAPS_ENABLE_PUBLIC_DB(unit) && (inrif == 0))
            || !JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
            *kaps_table_id = JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID;
         } else {
            *kaps_table_id = JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID;
         }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_pp_kaps_table_id_to_dbal_translate(SOC_SAND_IN  int                        unit,
                                         SOC_SAND_IN  JER_KAPS_IP_TBL_ID         kaps_table_id,
                                         SOC_SAND_OUT SOC_DPP_DBAL_SW_TABLE_IDS *table_id)
{
    SOCDNX_INIT_FUNC_DEFS;
 
    switch (kaps_table_id) {
    case JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        break;
    case JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
        break;
    case JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
        break;
    case JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
        break;
    case JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
        break;
    case JER_KAPS_IP_CORE_0_PUBLIC_IPV6_MC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC;
        break;
    case JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
        break;
    case JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS;
        break;
    case JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
        break;
    case JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE;
        break;
    case JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS;
        break;
    case JER_KAPS_IP_CORE_0_PRIVATE_IPV6_MC_TBL_ID:
        *table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("jer_pp_kaps_table_id_to_dbal_translate - invalid kaps table_id")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 jer_pp_kaps_entry_table_clear(SOC_SAND_IN    int unit, 
                                     SOC_SAND_IN    uint8 dbal_tbl_id)
{
    uint32 res;
    SOC_DPP_DBAL_TABLE_INFO dbal_table;
    JER_KAPS_IP_TBL_ID kaps_table_id;
    uint32 i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    
    if ((dbal_tbl_id != SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS) && (dbal_tbl_id != SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS)) {
        res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, dbal_tbl_id, &dbal_table);
        SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

        
        for (i=0; i<2; i++) {
            if (((i==1) && (JER_KAPS_ENABLE_PRIVATE_DB(unit))) || ((i==0) && (JER_KAPS_ENABLE_PUBLIC_DB(unit)))){
                res = jer_pp_kaps_dbal_table_id_translate(unit, dbal_tbl_id, NULL, i, &kaps_table_id);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

                if ((dbal_table.db_prefix_len > JER_KAPS_TABLE_PREFIX_LENGTH) || 
                    (dbal_table.additional_table_info != 0)) { 
                    res = jer_pp_kaps_entry_table_clear_hw_long_prefix(unit, kaps_table_id, dbal_table.db_prefix_len, dbal_table.db_prefix, dbal_table.additional_table_info);
                } else { 
                    res = jer_pp_kaps_entry_table_clear_hw(unit, kaps_table_id, dbal_table.additional_table_info);
                }
                SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear()",0,0);
}

uint32 jer_pp_kaps_db_enabled(SOC_SAND_IN    int              unit,
                              SOC_SAND_IN    SOC_PPC_VRF_ID   vrf_ndx)
{
    int32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    if (vrf_ndx == 0) {
        if (!JER_KAPS_ENABLE(unit)) {
            
            
            res = SOC_SAND_ERR;
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    else {
        if (!JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
            
            res = SOC_SAND_ERR;
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear()",0,0);
}

uint32 jer_pp_kaps_db_verify(SOC_SAND_IN    int              unit,
                             SOC_SAND_IN    SOC_PPC_RIF_ID    inrif)
{
    int32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    if (inrif == 0) {
        if (!JER_KAPS_ENABLE(unit)) {
            
            
            res = SOC_SAND_ERR;
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    } else {
        if (!JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
            
            res = SOC_SAND_ERR;
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_db_verify()",0,0);
}


uint32
    jer_pp_kaps_dma_entry_add(          
                   SOC_SAND_IN  int                         unit,
                   SOC_SAND_IN  uint32                      key,
                   SOC_SAND_IN  uint8                      *data,
                   SOC_SAND_IN  uint32                      size,
                   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success)
{
    int32
        res;
    struct kbp_dma_db 
       *db_p = NULL;
    uint8
        convert_flag = 0,
        entry_data[BITS2BYTES(JER_KAPS_DMA_DB_WIDTH_MAX)];
    uint32
        db_id=0, offset=0, word_off=0, mem_off=0, mem_len = size;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    *success = SOC_SAND_SUCCESS;

    JER_KAPS_KEY_INFO_DECODE(unit, key, db_id, offset, word_off);

    
    if (mem_len == JER_KAPS_DMA_BUFFER_NOF_BYTES) {
        
        if (word_off & 1) {
            LOG_WARN(BSL_LS_BCM_FP, (BSL_META_U(unit,"Trying to create KAPS DB with invalid LSB, must be 0 for double action size ! \n")));
            goto exit;
        }
    }

#ifdef CRASH_RECOVERY_SUPPORT
    soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_dma);
#endif

    
    jer_kaps_dma_db_get(unit,
                        db_id, 
                        &db_p);
    
    res = kbp_dma_db_read_entry(db_p, 
                               offset, 
                               entry_data);

    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
    }

    if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        
        int zone_size; 
        int nof_zones_per_entry; 
        int nof_words; 
        int bits_diff; 
        int shift; 
        uint8 mask; 
        int src_byte_idx; 
        int dst_byte_idx; 
        
        nof_words = 4;
        zone_size = SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width);
        bits_diff = 8 - zone_size % 8; 
        shift = bits_diff * (nof_words - word_off);
        mask = (1 << shift) - 1;
        
        dst_byte_idx = BITS2BYTES(JER_KAPS_DMA_DB_WIDTH(unit) - (word_off * zone_size) + 1) - 1;
        
        src_byte_idx = mem_len - 1;
        
        while(src_byte_idx) {
            
            entry_data[dst_byte_idx] &= mask;
            entry_data[dst_byte_idx] |= data[src_byte_idx] << shift;
            
            dst_byte_idx--;
            entry_data[dst_byte_idx] &= ~mask;
            entry_data[dst_byte_idx] |= data[src_byte_idx] >> (8 - shift);
            
            src_byte_idx--;
        } 
        
        entry_data[dst_byte_idx] &= mask;
        entry_data[dst_byte_idx] |= data[src_byte_idx] << shift;
        if(dst_byte_idx > 0) {
            dst_byte_idx--;
            
            nof_zones_per_entry = size/BITS2BYTES(zone_size);
            mask = (1 << (shift - nof_zones_per_entry*bits_diff)) - 1;
            entry_data[dst_byte_idx] &= ~mask;
            entry_data[dst_byte_idx] |= (data[src_byte_idx] >> (8 - shift)) & mask;
        }
    } else { 
        
        mem_off = (3 - word_off) * (JER_KAPS_DMA_BUFFER_NOF_BYTES/2);
        
        if (mem_len == JER_KAPS_DMA_BUFFER_NOF_BYTES) {
            mem_off -= (JER_KAPS_DMA_BUFFER_NOF_BYTES/2);
        }

        if (SOC_IS_QUX(unit)) {

            

            if (mem_off > 0) {
                mem_off -= 1;
            } else if (mem_off == 0x0) {
                convert_flag = 1;
            }
        }

        
        sal_memcpy(&(entry_data[mem_off]), data + convert_flag, mem_len - convert_flag);
    }

    
    res = kbp_dma_db_add_entry(db_p, 
                               entry_data, 
                               offset);

    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
    }

    
    res = jer_pp_xpt_wait_dma_done(unit);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_dma_entry_add()",0,0);
}


uint32
    jer_pp_kaps_dma_entry_remove(          
                   SOC_SAND_IN  int                         unit,
                   SOC_SAND_IN  uint32                      key,
                   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    int32
        res;
    struct kbp_dma_db 
       *db_p = NULL;
    uint32
        db_id = 0, offset = 0, word_off = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef CRASH_RECOVERY_SUPPORT
    soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_dma);
#endif

    *success = SOC_SAND_SUCCESS;

    JER_KAPS_KEY_INFO_DECODE(unit, key, db_id, offset, word_off);

    
    jer_kaps_dma_db_get(unit,
                        db_id, 
                        &db_p);

    
    res = kbp_dma_db_delete_entry(db_p,  
                               offset);


    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_dma_entry_remove()",0,0);
}

uint32
    jer_pp_kaps_dma_entry_get(
                   SOC_SAND_IN  int                         unit,
                   SOC_SAND_IN  uint32                      key,
                   SOC_SAND_IN  uint32                      size,
                   SOC_SAND_OUT uint8                       *data,
                   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success)
{
    int32
        res;
    struct kbp_dma_db
       *db_p = NULL;
    uint8
        convert_flag = 0,
        entry_data[BITS2BYTES(JER_KAPS_DMA_DB_WIDTH_MAX)];
    uint32
        db_id = 0, offset = 0, word_off = 0, mem_off = 0, mem_len = size;
    int
        zone_size, nof_words, bits_diff, shift, src_byte_idx, dst_byte_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef CRASH_RECOVERY_SUPPORT
    soc_dcmn_cr_suppress(unit,dcmn_cr_no_support_dma);
#endif

    *success = SOC_SAND_SUCCESS;

    sal_memset(data, 0, mem_len);
    JER_KAPS_KEY_INFO_DECODE(unit, key, db_id, offset, word_off);

    
    jer_kaps_dma_db_get(unit,
                        db_id,
                        &db_p);

    
    res = kbp_dma_db_read_entry(db_p,
                               offset,
                               entry_data);

    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
    }

    if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        nof_words = 4;
        zone_size = SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width);
        bits_diff = 8 - zone_size % 8;
        shift = bits_diff * (nof_words - word_off);
        dst_byte_idx = BITS2BYTES(JER_KAPS_DMA_DB_WIDTH(unit) - (word_off * zone_size) + 1) - 1;
        src_byte_idx = mem_len - 1;
        while(src_byte_idx) {
            data[src_byte_idx] |= entry_data[dst_byte_idx] >> shift;
            dst_byte_idx--;
            data[src_byte_idx] |= entry_data[dst_byte_idx] << (8 - shift);
            src_byte_idx--;
        }

        data[src_byte_idx] = entry_data[dst_byte_idx] >> shift;
        if(dst_byte_idx > 0) {
            dst_byte_idx--;
            data[src_byte_idx] |= (entry_data[dst_byte_idx] << (8 - shift));
        }
    } else {
        mem_off = (3 - word_off) * (JER_KAPS_DMA_BUFFER_NOF_BYTES/2);
        
        if ((size == JER_KAPS_DMA_BUFFER_NOF_BYTES) && (mem_off >= (JER_KAPS_DMA_BUFFER_NOF_BYTES/2))) {
            mem_off -= (JER_KAPS_DMA_BUFFER_NOF_BYTES/2);
        }

        if (SOC_IS_QUX(unit)) {
            if (mem_off > 0) {
                mem_off -= 1;
            } else if (mem_off == 0x0) {
                convert_flag = 1;
                if (mem_len > 3) {
                    mem_len = 3;
                }
            }
        }

        sal_memcpy(data, &(entry_data[mem_off + convert_flag]), mem_len);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_dma_entry_get()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

