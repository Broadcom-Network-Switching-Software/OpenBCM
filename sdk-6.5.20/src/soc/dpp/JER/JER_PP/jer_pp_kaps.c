/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/mcm/memregs.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/diag/shell.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>

#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>

#include <soc/scache.h>

#ifdef USE_MODEL
#include <soc/kbp/alg_kbp/include/model.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/ha.h>
#endif







#define TABLE_ID_OFFSET             10


#define KAPS_HB_AGE_COUNT           4

#define JER_KAPS_DB_IS_PRIVATE(db_id) ((db_id == JER_KAPS_IP_CORE_0_PRIVATE_DB_ID) || (db_id == JER_KAPS_IP_CORE_1_PRIVATE_DB_ID))

#define KAPS_ACCESS sw_state_access[unit].dpp.soc.arad.pp.kaps_db

#define JER_KAPS_RPB_TCAM_SCAN_DEBUG0 0x25
#define JER_KAPS_RPB_TCAM_SCAN_DEBUG1 0x26

#ifdef CRASH_RECOVERY_SUPPORT

typedef enum {
    HA_KAPS_SUB_ID_0 = 0
} HA_KAPS_sub_id_tl;
uint8 *kaps_nv_mem_ptr;
#endif



















static
    JER_KAPS_DB_CONFIG
        jer_kaps_db_config_info_static[JER_KAPS_IP_NOF_DB] = {

             
            {   
                FALSE, 
                0, 
                JER_KAPS_IP_NOF_DB, 
                NULL, 
                NULL, 
                NULL, 
                NULL  
            },

             
            {   
                FALSE, 
                0, 
                JER_KAPS_IP_CORE_0_PRIVATE_DB_ID, 
                NULL, 
                NULL, 
                NULL, 
                NULL  
            }, 

             
            {   
                FALSE, 
                0, 
                JER_KAPS_IP_NOF_DB, 
                NULL, 
                NULL, 
                NULL, 
                NULL  
            },

             
            {   
                FALSE, 
                0, 
                JER_KAPS_IP_CORE_0_PUBLIC_DB_ID, 
                NULL, 
                NULL, 
                NULL, 
                NULL  
            },
        };  



static
    JER_KAPS_TABLE_CONFIG
        jer_kaps_table_config_info_static[JER_KAPS_IP_NOF_TABLES] = {

            
            {   
                JER_KAPS_IP_CORE_0_PRIVATE_DB_ID,  
                JER_KAPS_TABLE_USE_DB_HANDLE, 
                {3, {{"TBL_ID_0", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_0 + DIP_0", 14 + 32 + (112 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

            
            {   
                JER_KAPS_IP_CORE_1_PRIVATE_DB_ID,  
                JER_KAPS_TABLE_USE_DB_HANDLE, 
                {3, {{"TBL_ID_1", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_1 + SIP_1", 14 + 32 + (112 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            }, 

             
            {   
                JER_KAPS_IP_CORE_0_PRIVATE_DB_ID,  
                JER_KAPS_IP_NOF_TABLES, 
                {3, {{"TBL_ID_2", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_2", 14, KBP_KEY_FIELD_EM}, {"DIP_2", 128+ (16 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

             
            {   
                JER_KAPS_IP_CORE_1_PRIVATE_DB_ID,  
                JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID, 
                {3, {{"TBL_ID_3", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_3", 14, KBP_KEY_FIELD_EM}, {"SIP_3", 128+ (16 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            }, 

             
            {   
                JER_KAPS_IP_CORE_0_PRIVATE_DB_ID,  
                JER_KAPS_IP_NOF_TABLES, 
                {3, {{"TBL_ID_4", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_4", 14, KBP_KEY_FIELD_EM}, {"MC-G-SIP-INRIF_4", 28 + 32 + 15 + (69 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

             
            {   
                JER_KAPS_IP_CORE_0_PRIVATE_DB_ID,  
                JER_KAPS_IP_NOF_TABLES, 
                {4, {{"TBL_ID_5", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_5", 14, KBP_KEY_FIELD_EM}, {"MC-GROUP_5", 128, KBP_KEY_FIELD_EM}, {"INRIF_5", 15 + (1 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

            
            {   
                JER_KAPS_IP_CORE_0_PUBLIC_DB_ID,  
                JER_KAPS_TABLE_USE_DB_HANDLE, 
                {3, {{"TBL_ID_6", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_6 + DIP_6", 14 + 32 + (112 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

            
            {   
                JER_KAPS_IP_CORE_1_PUBLIC_DB_ID,  
                JER_KAPS_TABLE_USE_DB_HANDLE, 
                {3, {{"TBL_ID_7", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_7 + SIP_7", 14 + 32 + (112 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            }, 

             
            {   
                JER_KAPS_IP_CORE_0_PUBLIC_DB_ID,  
                JER_KAPS_IP_NOF_TABLES, 
                {3, {{"TBL_ID_8", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_8", 14, KBP_KEY_FIELD_EM}, {"DIP_8", 128 + (16 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

             
            {   
                JER_KAPS_IP_CORE_1_PUBLIC_DB_ID,  
                JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID, 
                {3, {{"TBL_ID_9", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_9", 14, KBP_KEY_FIELD_EM}, {"SIP_9", 128 + (16 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            }, 

             
            {   
                JER_KAPS_IP_CORE_0_PUBLIC_DB_ID,  
                JER_KAPS_IP_NOF_TABLES, 
                {3, {{"TBL_ID_10", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_10", 14, KBP_KEY_FIELD_EM}, {"MC-G-SIP-INRIF_10", 28 + 32 + 15 + (69 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },

             
            {   
                JER_KAPS_IP_CORE_0_PUBLIC_DB_ID,  
                JER_KAPS_IP_NOF_TABLES, 
                {4, {{"TBL_ID_11", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_11", 14, KBP_KEY_FIELD_EM}, {"MC-GROUP_11", 128, KBP_KEY_FIELD_EM}, {"INRIF_11", 15 + (1 ), KBP_KEY_FIELD_PREFIX}}}, 
                NULL 
            },
        };  

static 
    JER_KAPS_SEARCH_CONFIG
        jer_kaps_search_config_info_static[JER_KAPS_NOF_SEARCHES] = {

            
            {
                FALSE, 
                FALSE, 
                {JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID, JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID, JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID, JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID}, 

                NULL 
            }, 

            
            {
                FALSE, 
                FALSE, 
                {JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID, JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID, JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID, JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID}, 
                NULL 
            }, 

            
            {
                FALSE, 
                FALSE, 
                {JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID, JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID, JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID, JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID}, 
                NULL 
            },  

            
            {
                FALSE, 
                FALSE, 
                {JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID, JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID, JER_KAPS_IP_CORE_0_PRIVATE_IPV6_MC_TBL_ID, JER_KAPS_IP_CORE_0_PUBLIC_IPV6_MC_TBL_ID}, 
                NULL 
            }, 
        };

static
    JER_KAPS_DB_CONFIG
        jer_kaps_db_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_IP_NOF_DB];

static
    JER_KAPS_TABLE_CONFIG
        jer_kaps_table_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_IP_NOF_TABLES];

static 
    JER_KAPS_SEARCH_CONFIG
        jer_kaps_search_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_NOF_SEARCHES];

genericJerAppData 
    *JerAppData[SOC_SAND_MAX_DEVICE] = {NULL};

static
    JER_KAPS_DB_CONFIG
        jer_kaps_dma_db_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_NOF_MAX_DMA_DB];

static kbp_warmboot_t kaps_warmboot_data[SOC_SAND_MAX_DEVICE];


const char* JER_KAPS_DB_NAMES[] = {"PRIVATE_CORE_0", "PRIVATE_CORE_1", "PUBLIC_CORE_0", "PUBLIC_CORE_1"};







void jer_kaps_sw_init(int unit)
{
    uint32
        db_id,
        db_idx,
        tbl_id,
        tbl_idx,
        search_idx,
        db_size;
    uint8 
        valid_tables_num;
    soc_dpp_config_jer_pp_t *jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);

    
    sal_memcpy(&jer_kaps_db_config_info[unit][0], &jer_kaps_db_config_info_static[0], sizeof(JER_KAPS_DB_CONFIG)*JER_KAPS_IP_NOF_DB);
    sal_memcpy(&jer_kaps_table_config_info[unit][0], &jer_kaps_table_config_info_static[0], sizeof(JER_KAPS_TABLE_CONFIG)*JER_KAPS_IP_NOF_TABLES);
    sal_memcpy(&jer_kaps_search_config_info[unit][0], &jer_kaps_search_config_info_static[0], sizeof(JER_KAPS_SEARCH_CONFIG)*JER_KAPS_NOF_SEARCHES);
    sal_memset(&jer_kaps_dma_db_config_info[unit][0], 0x0, sizeof(JER_KAPS_DB_CONFIG)*JER_KAPS_NOF_MAX_DMA_DB);


    if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX(unit)) {
        
        for(search_idx = 0; search_idx < JER_KAPS_NOF_SEARCHES; search_idx++) {
            jer_kaps_search_config_info[unit][search_idx].tbl_id[1] = jer_kaps_search_config_info_static[search_idx].tbl_id[2];
            jer_kaps_search_config_info[unit][search_idx].tbl_id[2] = jer_kaps_search_config_info_static[search_idx].tbl_id[1];
        }
    }

    
    for ( db_idx = 0; 
          db_idx < JER_KAPS_IP_NOF_DB; 
          db_idx++ ) 
    {
        if (JER_KAPS_DB_IS_PRIVATE(db_idx))
        {
            db_size = jer_pp_config->kaps_private_ip_frwrd_table_size;
        }
        else
        {
            db_size = jer_pp_config->kaps_public_ip_frwrd_table_size;
        }

        if (db_size > 0) 
        {
            if (SOC_IS_QAX(unit) && jer_kaps_db_config_info[unit][db_idx].clone_of_db_id != JER_KAPS_IP_NOF_DB) {
                
                jer_kaps_db_config_info[unit][db_idx].valid = FALSE;
            } else {
                jer_kaps_db_config_info[unit][db_idx].valid = TRUE;
                jer_kaps_db_config_info[unit][db_idx].db_size = db_size;
            }
        }
    }

    
    for(search_idx = 0; search_idx < JER_KAPS_NOF_SEARCHES; search_idx++) 
    {
        valid_tables_num = 0;
        
        for ( tbl_idx = 0; tbl_idx < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; tbl_idx++) 
        {
            
            tbl_id = jer_kaps_search_config_info[unit][search_idx].tbl_id[tbl_idx];
            db_id = jer_kaps_table_config_info[unit][tbl_id].db_id;
            if (jer_kaps_db_config_info[unit][db_id].valid) {
                
                if (valid_tables_num != tbl_idx) {
                    jer_kaps_search_config_info[unit][search_idx].tbl_id[valid_tables_num] = jer_kaps_search_config_info[unit][search_idx].tbl_id[tbl_idx];
                }
                valid_tables_num++;
            }
        }
        jer_kaps_search_config_info[unit][search_idx].valid_tables_num = valid_tables_num;
        
        jer_kaps_search_config_info[unit][search_idx].max_tables_num = SOC_IS_JERICHO_PLUS(unit) ? jer_kaps_search_config_info[unit][search_idx].valid_tables_num : JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES;
    }

    
    if (jer_pp_config->kaps_large_db_size) {
        for ( db_idx = 0; 
              db_idx < ( jer_pp_config->kaps_large_db_size / JER_KAPS_DMA_DB_NOF_ENTRIES ); 
              db_idx++ ) 
        {
              jer_kaps_dma_db_config_info[unit][db_idx].valid = TRUE;
              jer_kaps_dma_db_config_info[unit][db_idx].db_size = JER_KAPS_DMA_DB_NOF_PMF_ENTRIES;
        }
    }
}


STATIC int jer_kaps_init_device(int unit)
{
    uint32 flags;
    kbp_warmboot_t *warmboot_data;
    int32 res;
    uint8 is_allocated;

#ifdef CRASH_RECOVERY_SUPPORT
    unsigned int nv_mem_size = JER_KAPS_NV_MEMORY_SIZE; 
#endif
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    JerAppData[unit]->kaps_xpt_p = NULL;
#ifdef USE_MODEL
    
    res = kbp_sw_model_init(JerAppData[unit]->dalloc_p, KBP_DEVICE_KAPS, KBP_DEVICE_DEFAULT,
                              NULL, &(JerAppData[unit]->kaps_xpt_p));
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_sw_model_init with failed: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
    }
#else
#ifndef BLACKHOLE_MODE
    res = jer_pp_xpt_init(unit, &(JerAppData[unit]->kaps_xpt_p));
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
#endif
#endif

    warmboot_data = &kaps_warmboot_data[unit];

    flags = KBP_DEVICE_DEFAULT | KBP_DEVICE_ISSU;
    if (SOC_WARM_BOOT(unit)) {
        flags |= KBP_DEVICE_SKIP_INIT;
#ifdef CRASH_RECOVERY_SUPPORT
        if (JER_KAPS_IS_CR_MODE(unit) == 1) {
            flags &= ~KBP_DEVICE_ISSU;
        }
#endif

    }
    KBP_TRY(kbp_device_init(JerAppData[unit]->dalloc_p, 
                            KBP_DEVICE_KAPS, 
                            flags, 
                            (struct kaps_xpt*)JerAppData[unit]->kaps_xpt_p,
                            NULL,
                            &JerAppData[unit]->kaps_device_p));

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        
        if(ha_mem_alloc(unit, HA_KAPS_Mem_Pool, HA_KAPS_SUB_ID_0, &nv_mem_size, (void**)&kaps_nv_mem_ptr) != SOC_E_NONE) {
            return SOC_E_INTERNAL;
        }
        SOC_SAND_CHECK_NULL_PTR(kaps_nv_mem_ptr,20,exit);

        if (SOC_WARM_BOOT(unit)){
            KBP_TRY(kbp_device_set_property(JerAppData[unit]->kaps_device_p, KBP_DEVICE_PROP_CRASH_RECOVERY, 0, kaps_nv_mem_ptr, nv_mem_size)); 
            KBP_TRY(kbp_device_restore_state(JerAppData[unit]->kaps_device_p, NULL, NULL, NULL));
        } else {
            KBP_TRY(kbp_device_set_property(JerAppData[unit]->kaps_device_p, KBP_DEVICE_PROP_CRASH_RECOVERY, 1, kaps_nv_mem_ptr, nv_mem_size)); 
            res = KAPS_ACCESS.is_allocated(unit, &is_allocated);
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
            if(!is_allocated) {
                res = KAPS_ACCESS.alloc(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            }
        }
    } else
#endif
    {
        if (SOC_WARM_BOOT(unit)) {
            res = kbp_device_restore_state(JerAppData[unit]->kaps_device_p, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit,
                           "Error in %s(): failed with error: %s!\n"), 
                           FUNCTION_NAME(),
                           kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
            }
        } else {
            res = KAPS_ACCESS.is_allocated(unit, &is_allocated);
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
            if(!is_allocated) {
                res = KAPS_ACCESS.alloc(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            }
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_nlm_dev_mgr()", 0, 0);
}


STATIC uint32 jer_kaps_db_init(int unit, uint32 db_id)
{
    uint32 
        res = SOC_SAND_OK;

    JER_KAPS_DB_CONFIG
        *db_config_info = &(jer_kaps_db_config_info[unit][db_id]);

    JER_KAPS_DB_HANDLES
        db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    memset(&db_handles_info, 0, sizeof(db_handles_info));

    
    if((NULL != db_config_info->db_p) || !(db_config_info->valid)) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    if (!SOC_WARM_BOOT(unit)) {
       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): DB [%d] : create DB with db-size=%d.\n"),
                               FUNCTION_NAME(),
                    db_id,
                    db_config_info->db_size));

        if (db_config_info->clone_of_db_id == JER_KAPS_IP_NOF_DB) {
            
            res = kbp_db_init(
                    JerAppData[unit]->kaps_device_p, 
                    KBP_DB_LPM, 
                    db_id,
                    db_config_info->db_size, 
                    &db_config_info->db_p
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_init failed with : %s!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            
            LOG_VERBOSE(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                    "%s(): DB [%d] : create AD DB with asso-data=%d.\n"),
                                    FUNCTION_NAME(),
                         db_id,
                         JER_KAPS_AD_WIDTH_IN_BITS));

            KBP_TRY(kbp_ad_db_init(
                        JerAppData[unit]->kaps_device_p, 
                        db_id,
                        db_config_info->db_size, 
                        JER_KAPS_AD_WIDTH_IN_BITS,
                        &db_config_info->ad_db_p)
                    );

            KBP_TRY(kbp_db_set_ad(db_config_info->db_p,
                                  db_config_info->ad_db_p));

            
            db_handles_info.ad_db_p = db_config_info->ad_db_p;

            if (SOC_IS_JERICHO_PLUS(unit)) {
                KBP_TRY(kbp_hb_db_init(
                   JerAppData[unit]->kaps_device_p,
                   db_id,
                   db_config_info->db_size,
                   &db_config_info->hb_db_p
                   ));

                KBP_TRY(kbp_hb_db_set_property(db_config_info->hb_db_p, KBP_PROP_AGE_COUNT, KAPS_HB_AGE_COUNT));

                KBP_TRY(kbp_db_set_hb(db_config_info->db_p,
                                      db_config_info->hb_db_p));
            }

            
            db_handles_info.hb_db_p = db_config_info->hb_db_p;
        }
        else {
            
            KBP_TRY(kbp_db_clone(jer_kaps_db_config_info[unit][db_config_info->clone_of_db_id].db_p,
                                 db_id,
                                 &(db_config_info->db_p))
                    );
        }

        

        
        db_handles_info.db_p    = db_config_info->db_p;
        db_handles_info.is_valid = db_config_info->valid;
        res = KAPS_ACCESS.db_info.set(unit, db_id, db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        ARAD_DO_NOTHING_AND_EXIT;
    }
    else {
        res = KAPS_ACCESS.db_info.get(unit, db_id, &db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        res = kbp_db_refresh_handle(JerAppData[unit]->kaps_device_p, db_handles_info.db_p, &db_handles_info.db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Table Key : kbp_db_refresh_handle failed: %s, db_id = %d!\n"),
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), db_id));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 31, exit);
        }

        if (db_config_info->clone_of_db_id == JER_KAPS_IP_NOF_DB) {
            res = kbp_ad_db_refresh_handle(JerAppData[unit]->kaps_device_p, db_handles_info.ad_db_p, &db_handles_info.ad_db_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): Table Key : kbp_ad_db_refresh_handle failed: %s, db_id = %d!\n"),
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res), db_id));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 31, exit);
            }
            jer_kaps_db_config_info[unit][db_id].ad_db_p = db_handles_info.ad_db_p;

            if (SOC_IS_JERICHO_PLUS(unit)) {
                db_handles_info.hb_db_p = (struct kbp_hb_db *) (((uintptr_t) db_handles_info.hb_db_p));
                res = kbp_hb_db_refresh_handle(JerAppData[unit]->kaps_device_p, db_handles_info.hb_db_p, &db_handles_info.hb_db_p);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                   LOG_ERROR(BSL_LS_SOC_TCAM,
                             (BSL_META_U(unit,
                                         "Error in %s(): kbp_hb_db_refresh_handle failed: %s, db_id = %d!\n"),
                                         FUNCTION_NAME(),
                              kbp_get_status_string(res), db_id));

                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 31, exit);
                }
                jer_kaps_db_config_info[unit][db_id].hb_db_p = db_handles_info.hb_db_p;
            }
        }

        jer_kaps_db_config_info[unit][db_id].db_p = db_handles_info.db_p;

        res = KAPS_ACCESS.db_info.set(unit, db_id, db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_db_init()", db_id, 0);
}


STATIC uint32 jer_kaps_dma_db_init(int unit, uint32 db_id)
{
    uint32 
        res = SOC_SAND_OK;

    JER_KAPS_DB_CONFIG
        *db_config_info = &(jer_kaps_dma_db_config_info[unit][db_id]);

    JER_KAPS_DMA_DB_HANDLES
        dma_db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    memset(&dma_db_handles_info, 0, sizeof(dma_db_handles_info));

    
    if(NULL != db_config_info->dma_db_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    if (!SOC_WARM_BOOT(unit)) {
        res = kbp_dma_db_init(
                JerAppData[unit]->kaps_device_p,
                db_id,
                db_config_info->db_size,
                JER_KAPS_DMA_DB_WIDTH(unit),
                &db_config_info->dma_db_p
              );
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): kbp_db_init failed with : %s!\n"),
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
        
        dma_db_handles_info.dma_db_p    = db_config_info->dma_db_p;
        dma_db_handles_info.is_valid    = db_config_info->valid;
        res = KAPS_ACCESS.dma_db_info.set(unit, db_id, dma_db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        ARAD_DO_NOTHING_AND_EXIT;
    } else {
        res = KAPS_ACCESS.dma_db_info.get(unit, db_id, &dma_db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        res = kbp_dma_db_refresh_handle(JerAppData[unit]->kaps_device_p, dma_db_handles_info.dma_db_p, &dma_db_handles_info.dma_db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Table Key : kbp_dma_db_refresh_handle failed: %s, db_id = %d!\n"),
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), db_id));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 31, exit);
        }

        jer_kaps_dma_db_config_info[unit][db_id].dma_db_p = dma_db_handles_info.dma_db_p;

        res = KAPS_ACCESS.dma_db_info.set(unit, db_id, dma_db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_db_init()", db_id, 0);
}


STATIC uint32 jer_kaps_table_init(int unit, uint32 tbl_id)
{
    uint32 
        key_ndx,
        db_id,
        res = SOC_SAND_OK;

    JER_KAPS_KEY *key;

    JER_KAPS_TABLE_CONFIG
        *tbl_config_info = &(jer_kaps_table_config_info[unit][tbl_id]);

    JER_KAPS_TABLE_HANDLES
        table_handle;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    memset(&table_handle, 0, sizeof(table_handle));

    
    if(NULL != tbl_config_info->tbl_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    if (!SOC_WARM_BOOT(unit)) 
    {
        
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): Table [%d] : create table.\n"),
                               FUNCTION_NAME(),
                    tbl_id));

        
        if (!jer_kaps_db_config_info[unit][tbl_config_info->db_id].valid) {
            return SOC_SAND_OK;
        }
        if (tbl_config_info->clone_of_tbl_id == JER_KAPS_TABLE_USE_DB_HANDLE) 
        {
            
            db_id = tbl_config_info->db_id;
            tbl_config_info->tbl_p = jer_kaps_db_config_info[unit][db_id].db_p;
        }
        else if (tbl_config_info->clone_of_tbl_id == JER_KAPS_IP_NOF_TABLES) 
        {
            
            db_id = tbl_config_info->db_id;
            res = kbp_db_add_table(
                    jer_kaps_db_config_info[unit][db_id].db_p, 
                    tbl_id + TABLE_ID_OFFSET, 
                    &(tbl_config_info->tbl_p)
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_add_table failed with : %s!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));
           
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }
        else
        {
            res = kbp_db_clone(jer_kaps_table_config_info[unit][tbl_config_info->clone_of_tbl_id].tbl_p,
                                 tbl_id + TABLE_ID_OFFSET,
                                 &(tbl_config_info->tbl_p));
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_clone failed with : %s!\n"),
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }

        
        KBP_TRY(kbp_key_init(JerAppData[unit]->kaps_device_p, &key));

        
        for(key_ndx = 0;
             key_ndx < tbl_config_info->key_fields.nof_key_segments;
             key_ndx++)
        {
            
            if(tbl_config_info->key_fields.key_segment[key_ndx].nof_bits){

                res = kbp_key_add_field(
                        key, 
                        tbl_config_info->key_fields.key_segment[key_ndx].name,
                        tbl_config_info->key_fields.key_segment[key_ndx].nof_bits, 
                        tbl_config_info->key_fields.key_segment[key_ndx].type
                      );

                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    if(res != KBP_DUPLICATE_KEY_FIELD) {
                       LOG_ERROR(BSL_LS_SOC_TCAM,
                                 (BSL_META_U(unit,
                                             "Error in %s(): Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                             FUNCTION_NAME(),
                                  res,
                                  kbp_get_status_string(res)));

                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                    }
                }
            }
        }

        res = kbp_db_set_key(
                tbl_config_info->tbl_p,
                key
              );
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Table Key : kbp_db_set_key with failed: %s, table_id = %d!\n"),
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), tbl_id));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }

        table_handle.tbl_p = tbl_config_info->tbl_p;
        res = KAPS_ACCESS.table_handles.set(unit, tbl_id, table_handle);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    } else {
        if (tbl_config_info->clone_of_tbl_id == JER_KAPS_TABLE_USE_DB_HANDLE)
        {
            
            db_id = tbl_config_info->db_id;
            table_handle.tbl_p = jer_kaps_db_config_info[unit][db_id].db_p;
        }
        else
        {
            
            res = KAPS_ACCESS.table_handles.get(unit, tbl_id, &table_handle);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            res = kbp_db_refresh_handle(JerAppData[unit]->kaps_device_p, table_handle.tbl_p, &(table_handle.tbl_p));
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): Table Key : kbp_db_refresh_handle failed: %s, table_id = %d!\n"),
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res), tbl_id));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 31, exit);
            }
        }
        tbl_config_info->tbl_p = table_handle.tbl_p;

        res = KAPS_ACCESS.table_handles.set(unit, tbl_id, table_handle);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_table_init()", tbl_id, 0);
}

STATIC uint32 jer_kaps_search_init(int unit, uint32 search_id)
{
    uint32
        key_ndx,
        tbl_idx,
        tbl_id,
        res = SOC_SAND_OK;

    JER_KAPS_KEY *master_key;

    JER_KAPS_TABLE_CONFIG
        *tbl_config_info = NULL;

    JER_KAPS_SEARCH_CONFIG
        *search_config_info = &(jer_kaps_search_config_info[unit][search_id]);

    JER_KAPS_INSTRUCTION_HANDLES
        instruction_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_WARM_BOOT(unit)) {
        
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s() : create a new instruction, search ID %d.\n"),
                                FUNCTION_NAME(),
                     search_id));

        KBP_TRY(kbp_instruction_init(
                    JerAppData[unit]->kaps_device_p, 
                    search_id,
                    0, 
                    &(search_config_info->inst_p))
                );

        
        KBP_TRY(kbp_key_init(JerAppData[unit]->kaps_device_p, &master_key));

        
        
        for (tbl_idx = 0; tbl_idx < search_config_info->max_tables_num; tbl_idx++)
        {
            tbl_id = search_config_info->tbl_id[tbl_idx];
            tbl_config_info = &(jer_kaps_table_config_info[unit][tbl_id]);

            
            for(key_ndx = 0;
                 key_ndx < tbl_config_info->key_fields.nof_key_segments;
                 key_ndx++)
            {
                
                if(tbl_config_info->key_fields.key_segment[key_ndx].nof_bits){

                    res = kbp_key_add_field(
                            master_key, 
                            tbl_config_info->key_fields.key_segment[key_ndx].name,
                            tbl_config_info->key_fields.key_segment[key_ndx].nof_bits, 
                            tbl_config_info->key_fields.key_segment[key_ndx].type
                          );

                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        if(res != KBP_DUPLICATE_KEY_FIELD) {
                           LOG_ERROR(BSL_LS_SOC_TCAM,
                                     (BSL_META_U(unit,
                                                 "Error in %s(): Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                                 FUNCTION_NAME(),
                                      res,
                                      kbp_get_status_string(res)));

                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                        }
                    }
                }
            }
        }

        KBP_TRY(kbp_instruction_set_key(search_config_info->inst_p, master_key));

        for (tbl_idx = 0; tbl_idx < search_config_info->valid_tables_num; tbl_idx++)
        {
            tbl_id = search_config_info->tbl_id[tbl_idx];
            tbl_config_info = &(jer_kaps_table_config_info[unit][tbl_id]);

            if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX(unit)) {
                res = kbp_instruction_add_db(search_config_info->inst_p,
                                             jer_kaps_table_config_info[unit][tbl_id].tbl_p,
                                             
                                             tbl_idx);
            } else {
                res = kbp_instruction_add_db(search_config_info->inst_p,
                                             jer_kaps_table_config_info[unit][tbl_id].tbl_p,
                                             
                                             
                                             tbl_idx * search_config_info->max_tables_num / search_config_info->valid_tables_num);
            }
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_instruction_add_db failed with : %s, tbl_id = %d result = %d!\n"),
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res), tbl_id, tbl_idx));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }

        res = kbp_instruction_install(search_config_info->inst_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): kbp_instruction_install failed with : %s, search_id = %d!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), search_id));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }

        instruction_handles_info.inst_p = search_config_info->inst_p;

        res = KAPS_ACCESS.search_instruction_info.set(unit, search_id, instruction_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    } else {
        res = KAPS_ACCESS.search_instruction_info.get(unit, search_id, &instruction_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        KBP_TRY(kbp_instruction_refresh_handle(JerAppData[unit]->kaps_device_p, instruction_handles_info.inst_p, &instruction_handles_info.inst_p));

        search_config_info->inst_p = instruction_handles_info.inst_p;

        res = KAPS_ACCESS.search_instruction_info.set(unit, search_id, instruction_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_search_init()", search_id, 0);
}

STATIC
    int jer_kaps_device_lock_config(
        SOC_SAND_IN  int unit
    )
{
    uint32 res = SOC_SAND_OK;

    res = kbp_device_lock(JerAppData[unit]->kaps_device_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_device_lock with failed: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
    }

    return res;
}

uint32 jer_kaps_init_db_set(int unit)
{
    uint32
        tbl_idx,
        search_idx,
        db_idx,
        db_id,
        res;

    uint8 is_valid;

    JER_KAPS_DB_HANDLES
        db_handles_info;
    JER_KAPS_DMA_DB_HANDLES
        dma_db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (JER_KAPS_ENABLE_PRIVATE_DB(unit) || JER_KAPS_ENABLE_PUBLIC_DB(unit)) {
        
        for (db_idx = 0; db_idx < JER_KAPS_IP_NOF_DB; db_idx++) 
        {
            if (SOC_WARM_BOOT(unit)) {
                res = KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

                is_valid = db_handles_info.is_valid;
            } 
            else 
            {
                is_valid = jer_kaps_db_config_info[unit][db_idx].valid; 
            }

            if (is_valid) 
            {
                res = jer_kaps_db_init(
                        unit,  
                        db_idx
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            }
        }

        
        for (tbl_idx = 0; tbl_idx < JER_KAPS_IP_NOF_TABLES; tbl_idx++) 
        {
            db_id = jer_kaps_table_config_info[unit][tbl_idx].db_id;
            is_valid = jer_kaps_db_config_info[unit][db_id].valid;
            if (is_valid)
            {
                res = jer_kaps_table_init(unit, tbl_idx);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            }
        }

        
        for(search_idx = 0; search_idx < JER_KAPS_NOF_SEARCHES; search_idx++)
        {
            if (jer_kaps_search_config_info[unit][search_idx].valid_tables_num)
            {
                
                res = jer_kaps_search_init(
                        unit, 
                        search_idx
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
            }
        }
    }

    if (JER_KAPS_ENABLE_DMA(unit)) {
        
        for (db_idx = 0; db_idx < JER_KAPS_NOF_MAX_DMA_DB; db_idx++) 
        {
            if (SOC_WARM_BOOT(unit))
            {
                res = KAPS_ACCESS.dma_db_info.get(unit, db_idx, &dma_db_handles_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

                is_valid = dma_db_handles_info.is_valid;
            }
            else
            {
                is_valid = jer_kaps_dma_db_config_info[unit][db_idx].valid;
            }

            if (is_valid)
            {
                res = jer_kaps_dma_db_init(
                        unit,
                        db_idx
                      );

                SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
            }
        }
    }

    
    res = jer_kaps_device_lock_config(unit);
    if(res != 0)
    {
        LOG_ERROR(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "Error in %s(): %s!\n"),
                         FUNCTION_NAME(),
              kbp_get_status_string(res)));
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    
    if (!SOC_WARM_BOOT(unit))
    {
        for (db_idx = 0; db_idx < JER_KAPS_NOF_MAX_DMA_DB; db_idx++)
        {
             is_valid = jer_kaps_dma_db_config_info[unit][db_idx].valid;
             if (is_valid)
             {
                res = jer_pp_kaps_dma_clear_db(
                       unit,
                        db_idx
                      );
               SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
             }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_db_set()", 0, 0);
}

uint32 jer_kaps_tcam_error_scan_enable(int unit)
{
    uint32 res, blk_id, reg_val;
    JER_KAPS_XPT xpt_p;
    uint8 data[4]={0};
        
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (!SOC_WARM_BOOT(unit)) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            
            sal_memset(&xpt_p, 0, sizeof(xpt_p));
            xpt_p.jer_data.unit = unit;
            
            for (blk_id = JER_KAPS_RPB_BLOCK_INDEX_START; blk_id <= JER_KAPS_RPB_BLOCK_INDEX_END; blk_id++) {
                sal_memset(data, 0, sizeof(uint32));
                data[3] = 5; 

                res = jer_pp_kaps_write_command(&xpt_p,
                                                blk_id,
                                                KAPS_CMD_WRITE,
                                                KAPS_FUNC0,
                                                JER_KAPS_RPB_TCAM_SCAN_DEBUG0,
                                                4,
                                                data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
                
                
                reg_val = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 10 / 4096;
                data[0] = (reg_val >> 24) & 0xFF; 
                data[1] = (reg_val >> 16) & 0xFF; 
                data[2] = (reg_val >> 8) & 0xFF; 
                data[3] = reg_val & 0xFF; 
                res = jer_pp_kaps_write_command(&xpt_p,
                                                blk_id,
                                                KAPS_CMD_WRITE,
                                                KAPS_FUNC0,
                                                JER_KAPS_RPB_TCAM_SCAN_DEBUG1,
                                                4,
                                                data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_db_set()", 0, 0);
}



uint32 jer_kaps_init_app(int unit)
{
    uint32 res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (JerAppData[unit] == NULL) {
        ARAD_ALLOC_ANY_SIZE(JerAppData[unit], genericJerAppData, 1,"JerAppData[unit]");
        sal_memset(JerAppData[unit], 0x0, sizeof(genericJerAppData));
    }
    else {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kaps is already initialized.\n"), FUNCTION_NAME()));
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    KBP_TRY(default_allocator_create(&JerAppData[unit]->dalloc_p));

    
    res = jer_kaps_init_device(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    jer_kaps_sw_init(unit);

    
    res = jer_kaps_init_db_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    
    res = jer_kaps_tcam_error_scan_enable(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_app()", 0, 0);
}

STATIC
uint32 jer_kaps_deinit_db_set(
    int unit)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    sal_memset(&jer_kaps_db_config_info[unit][0], 0, sizeof(JER_KAPS_DB_CONFIG)*JER_KAPS_IP_NOF_DB);
    sal_memset(&jer_kaps_table_config_info[unit][0], 0, sizeof(JER_KAPS_TABLE_CONFIG)*JER_KAPS_IP_NOF_TABLES);
    sal_memset(&jer_kaps_search_config_info[unit][0], 0, sizeof(JER_KAPS_SEARCH_CONFIG)*JER_KAPS_NOF_SEARCHES);

    jer_kaps_sw_init(unit);

    SOC_SAND_EXIT_AND_SEND_ERROR( "error in  jer_kaps_deinit_db_set()", 0, 0);

}

STATIC 
int
    jer_kaps_deinit_device(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int no_sync_flag
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!no_sync_flag)
    {
        KBP_TRY(kbp_device_destroy(JerAppData[unit]->kaps_device_p));
    }
    else
    {
        KBP_TRY(default_allocator_destroy(JerAppData[unit]->dalloc_p));
        KBP_TRY(default_allocator_create(&JerAppData[unit]->dalloc_p));
    }

#ifdef USE_MODEL
    
    if (!no_sync_flag)
    {
        KBP_TRY(kbp_sw_model_destroy(JerAppData[unit]->kaps_xpt_p));
    }
#else
#ifndef BLACKHOLE_MODE
    jer_pp_xpt_deinit(unit, JerAppData[unit]->kaps_xpt_p);
#endif
#endif

    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_deinit_device()", 0, 0);
}

uint32 
    jer_kaps_deinit_app(
        int unit,
        int no_sync_flag
    )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(NULL == JerAppData[unit])
    {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kaps not initialized.\n"), FUNCTION_NAME()));
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = jer_kaps_deinit_db_set(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef CRASH_RECOVERY_SUPPORT
    if (JER_KAPS_IS_CR_MODE(unit) == 1) {
        res = jer_kaps_deinit_device(unit, no_sync_flag);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
#endif
    {
        res = jer_kaps_deinit_device(unit, 0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    
    KBP_TRY(default_allocator_destroy(JerAppData[unit]->dalloc_p));

    ARAD_FREE(JerAppData[unit]);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_deinit_app()", unit, 0x0);
}

int
jer_kaps_autosync_set(int unit, int enable)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

int
 jer_kaps_sync(int unit)
{
    kbp_warmboot_t *warmboot_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


#ifdef CRASH_RECOVERY_SUPPORT
    if(!(JER_KAPS_IS_CR_MODE(unit)))
#endif
    {
        warmboot_data = &kaps_warmboot_data[unit];

        if (NULL == JerAppData[unit]) ARAD_DO_NOTHING_AND_EXIT;

        KBP_TRY(kbp_device_save_state_and_continue(JerAppData[unit]->kaps_device_p, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp));
    }
   
    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

#ifdef CRASH_RECOVERY_SUPPORT
uint32 jer_kaps_cr_transaction_cmd(int unit,
                                   uint8 is_start)
{
    if (JerAppData[unit]->kaps_device_p != NULL) {
        if (is_start) {
            KBP_TRY(kbp_device_start_transaction(JerAppData[unit]->kaps_device_p));
        } else { 
            KBP_TRY(kbp_device_end_transaction(JerAppData[unit]->kaps_device_p));
        }
    }
    return 0;
}

uint8 jer_kaps_cr_query_restore_status(int unit)
{
    uint8 result=TRUE;

    enum kbp_restore_status res_status;
    if(JerAppData[unit]->kaps_device_p != NULL){
        KBP_TRY(kbp_device_query_restore_status(JerAppData[unit]->kaps_device_p, &res_status));
    }

    if (res_status == KBP_RESTORE_CHANGES_ABORTED) {
        result = FALSE;
    }

    return result;
}

uint32 jer_kaps_cr_clear_restore_status(int unit)
{
    if(JerAppData[unit]->kaps_device_p != NULL){
        KBP_TRY(kbp_device_clear_restore_status(JerAppData[unit]->kaps_device_p));
    }
    return 0;
}

uint32 jer_kaps_cr_db_commit(int unit,
                             uint32 tbl_idx)
{
    struct kbp_db *db_p = NULL;
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_db_get(unit, tbl_idx, &db_p);

    if (db_p != NULL) {
        res = kbp_db_install(db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_db_commit()", 0, 0);
}
#endif

void 
  jer_kaps_warmboot_register(int unit,
                             FILE *file_fp,
                             kbp_device_issu_read_fn read_fn, 
                             kbp_device_issu_write_fn write_fn)
{
    kaps_warmboot_data[unit].kbp_file_fp = file_fp;
    kaps_warmboot_data[unit].kbp_file_read = read_fn;
    kaps_warmboot_data[unit].kbp_file_write = write_fn;
}


void jer_kaps_db_get(
   int unit,
   uint32 tbl_id,
   struct kbp_db **db_p)
{
    *db_p = jer_kaps_table_config_info[unit][tbl_id].tbl_p;
}

void jer_kaps_ad_db_get(
   int unit,
   uint32 tbl_id,
   struct kbp_ad_db **ad_db_p)
{
    uint32 db_id;

    db_id = jer_kaps_table_config_info[unit][tbl_id].db_id;
    *ad_db_p = jer_kaps_db_config_info[unit][db_id].ad_db_p;
}

void jer_kaps_hb_db_get(
   int unit,
   uint32 tbl_id,
   struct kbp_hb_db **ad_hb_p)
{
    uint32 db_id;

    db_id = jer_kaps_table_config_info[unit][tbl_id].db_id;
    *ad_hb_p = jer_kaps_db_config_info[unit][db_id].hb_db_p;
}

void jer_kaps_dma_db_get(
   int unit,
   uint32 db_id,
   struct kbp_dma_db **db_p)
{

    *db_p = jer_kaps_dma_db_config_info[unit][db_id].dma_db_p;
}


void jer_kaps_search_config_get(
   int unit,
   uint32 search_id,
   JER_KAPS_SEARCH_CONFIG *search_cfg_p)
{
    *search_cfg_p = jer_kaps_search_config_info[unit][search_id];
}

void jer_kaps_table_config_get(
   int unit,
   uint32 tbl_id,
   JER_KAPS_TABLE_CONFIG *table_cfg_p)
{
    *table_cfg_p = jer_kaps_table_config_info[unit][tbl_id];
}

void jer_kaps_ad_db_by_db_id_get(
    int unit,
    uint32 db_id,
    JER_KAPS_AD_DB **ad_db_p)
{
    *ad_db_p = jer_kaps_db_config_info[unit][db_id].ad_db_p;
}


uint8 jer_kaps_clone_of_db_id_get(
    int unit,
    uint32 db_id)
{
    return jer_kaps_db_config_info[unit][db_id].clone_of_db_id;
}

void* jer_kaps_kaps_xpt_p_get(
    int unit)
{
    return JerAppData[unit]->kaps_xpt_p;
}

void* jer_kaps_app_data_get(
    int unit)
{
    return JerAppData[unit];
}

uint32 jer_kaps_hb_timer(int unit)
{
    uint32
        db_idx,
        res;

    JER_KAPS_DB_HANDLES
        db_handles_info;

    JER_KAPS_DB_CONFIG
        *db_config_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (db_idx = 0; db_idx < JER_KAPS_IP_NOF_DB; db_idx++)
    {
        db_config_info = &(jer_kaps_db_config_info[unit][db_idx]);
        if (db_config_info->valid)
        {
            if (db_config_info->clone_of_db_id == JER_KAPS_IP_NOF_DB) {
                
                res = KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                KBP_TRY(kbp_hb_db_timer(db_handles_info.hb_db_p));
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_app()", 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
