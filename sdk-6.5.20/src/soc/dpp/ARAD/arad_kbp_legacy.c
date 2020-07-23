/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/scache.h>

extern genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];


STATIC void WriteValueToBitVector4( uint8 *data, uint32 value )
{
      data[ 0 ] = ( uint8 )( 0xFF & ( value >> 24 ) );
      data[ 1 ] = ( uint8 )( 0xFF & ( value >> 16 ) );
      data[ 2 ] = ( uint8 )( 0xFF & ( value >> 8 ) );
      data[ 3 ] = ( uint8 )( 0xFF & ( value >> 0 ) );
}

void FillRecordPatternFor_80B0_Table(uint8 *data_p, uint8 *mask_p, uint32 iter)
{
    

    sal_memset( data_p, 0, 10 );
    sal_memset( mask_p, 0, 10 );

    
    WriteValueToBitVector4(&data_p[2], iter);

    
    sal_memset(&mask_p[6], 0xFF, 4 );

    return;
}


STATIC void CreateRecordsInTables_nosmt(genericTblMgrAradAppData *refAppData_p,
    uint32 num_entries,
    uint32 *record_base_tbl,
    uint32 *ad_val_tbl)
{
    tableInfo *tblInfo_p;
    tableRecordInfo *tblRecordInfo_p;
    uint32 alloc_size;
    uint8  iter_tbl, tblWidth_inBytes;
    uint8  *rec_data, *rec_mask;
    uint16  start, end, iter_group, iter_priority;
    uint32 loop = 0, start_value = 0, advlaue = 0;

    
     for( iter_tbl = 0; iter_tbl < ARAD_KBP_FRWRD_IP_NOF_TABLES; iter_tbl++ )
     {
         if (NULL == refAppData_p->g_gtmInfo[iter_tbl].tblInfo.db_p) {
            
            continue;
        }

        tblInfo_p = &refAppData_p->g_gtmInfo[iter_tbl].tblInfo;

        start = tblInfo_p->groupId_start;
        end = tblInfo_p->groupId_end;

        tblInfo_p->tbl_size = num_entries;
        alloc_size = tblInfo_p->tbl_size;
        tblInfo_p->tblRecordInfo_p = arad_kbp_alloc(alloc_size, sizeof( tableRecordInfo ) );

        tblInfo_p->max_recCount    = alloc_size;
         tblWidth_inBytes = (uint8)(tblInfo_p->tbl_width / 8);

        rec_data = arad_kbp_alloc(1, tblWidth_inBytes );
        rec_mask = arad_kbp_alloc(1, tblWidth_inBytes );

        start_value = record_base_tbl[iter_tbl]; 
        advlaue = ad_val_tbl[iter_tbl];      

        
        iter_group = start;
        iter_priority = 0;
        tblRecordInfo_p = tblInfo_p->tblRecordInfo_p;

        for(loop = 0; loop < tblInfo_p->tbl_size; loop++)
        {
            tblRecordInfo_p->groupId  = iter_group;
            tblRecordInfo_p->priority = iter_priority;

            tblRecordInfo_p->record.m_data = arad_kbp_alloc(1, tblWidth_inBytes );
            tblRecordInfo_p->record.m_mask = arad_kbp_alloc(1, tblWidth_inBytes );
            tblRecordInfo_p->record.m_len  = tblInfo_p->tbl_width;

            
            FillRecordPatternFor_80B0_Table(rec_data, rec_mask, (start_value + loop));

            
            WriteValueToBitVector4(&tblRecordInfo_p->assoData[0], (advlaue + loop));

            if( tblInfo_p->tbl_id  == 0) 
                WriteValueToBitVector4(&tblRecordInfo_p->assoData[4], (advlaue + loop));

            sal_memcpy( tblRecordInfo_p->record.m_data, rec_data, tblWidth_inBytes );
            sal_memcpy( tblRecordInfo_p->record.m_mask, rec_mask, tblWidth_inBytes );

            tblRecordInfo_p++;

#ifdef WITH_INDEX_SHUFFLE
            iter_priority++;

            
            if( (iter_priority % end) == 0)
            {
                iter_priority = start;
                iter_group++;
            }

            
            if( iter_group && ((iter_group % end) == 0))
            {
                iter_group = start;
            }
#else
            iter_group++;
            iter_priority++;

            
            if( (iter_priority % end) == 0)
            {
                iter_priority = start;
            }

            
            if( (iter_group % end) == 0)
            {
                iter_group = start;
            }
#endif
        }

        
        arad_kbp_free(rec_data );
        arad_kbp_free(rec_mask );

     } 
}



STATIC int AddRecordsIntoTables(
                genericTblMgrAradAppData *refAppData_p,
                uint8 instance
                )
{
    int res;
    tableInfo *tblInfo_p;
    tableRecordInfo    *tblRecordInfo_p;
    uint32 iter_rec, num_recs;
    struct kbp_ad_db *ad_db_p;
    struct kbp_entry *dummy_entry;

    tblInfo_p = &refAppData_p->g_gtmInfo[instance].tblInfo;
    tblRecordInfo_p = tblInfo_p->tblRecordInfo_p;
    num_recs  = tblInfo_p->max_recCount;

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META("\n Adding records into table = %d\n"),instance));
    for(iter_rec = 0; iter_rec < num_recs; iter_rec++ )
    {
        struct kbp_ad *ad_entry;

        
        res = kbp_db_add_ace(tblInfo_p->db_p, tblRecordInfo_p->record.m_data, tblRecordInfo_p->record.m_mask,
                             tblRecordInfo_p->priority, &dummy_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META("Error in %s(): kbp_db_add_ace failed: %s\n"), FUNCTION_NAME(), kbp_get_status_string(res)));
        }

        
        if(tblInfo_p->tbl_assoWidth){
            ad_db_p = refAppData_p->g_gtmInfo[instance].tblInfo.ad_db_p;
            res = kbp_ad_db_add_entry(ad_db_p,tblRecordInfo_p->assoData,&ad_entry);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META("Error in %s(): kbp_ad_db_add_entry failed: %s\n"), FUNCTION_NAME(), kbp_get_status_string(res)));
            }
            res = kbp_entry_add_ad(tblInfo_p->db_p,dummy_entry,ad_entry);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META("Error in %s(): kbp_entry_add_ad failed: %s\n"), FUNCTION_NAME(), kbp_get_status_string(res)));
            }
        }

        
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
soc_sand_ll_timer_set("ARAD_KBP_TIMERS_ADD_RECORDS_INTO_TABLES", ARAD_KBP_TIMERS_ADD_RECORDS_INTO_TABLES);
#endif 
        res = kbp_db_install(tblInfo_p->db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META("Error in %s(): kbp_db_install failed: %s\n"), FUNCTION_NAME(), kbp_get_status_string(res)));
        }
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_ADD_RECORDS_INTO_TABLES);
#endif 


        
        tblRecordInfo_p++;
        tblInfo_p->rec_count++;

        if( (tblInfo_p->rec_count % 500) == 0 ) {
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META("\n\t    - %u records added to table \n"),tblInfo_p->rec_count));
        }

    } 

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META("\t   Total number of records added [%u] to table\n"), tblInfo_p->rec_count));

    return SOC_SAND_OK;
}

#define SRCH_ENABLE

#ifdef SRCH_ENABLE

STATIC uint32 Perform_LTR_Searches(int unit,
                                genericTblMgrAradAppData *refAppData_p,
                                uint32 nof_searches,
                                uint32 tbl_id,
                                uint32 db_type,
                                uint8 ltr_num)
{
    tableInfo   *tblInfo_p[4];
    tableRecordInfo   *tblRecordInfo_p[4];
    alg_kbp_rec *tblRecord_p[4];
    uint32 iter, numRecs, res;
    uint8  i=0;
    int32_t index1 = -1,priority1 = -1,index2 = -1,priority2 = -1;
    tableRecordInfo *tblRecHoldInfo_p[4]; 
    struct kbp_entry *entry_in_db1 = NULL,*entry_in_db2 = NULL;
    uint8_t db_key[80] = {0,};
    arad_kbp_rslt exp_result;

    
    struct kbp_search_result cmp_rslt;
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};
    uint8_t mkey[80] = { 0, };
	
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(tblInfo_p, 0x0, sizeof(tableInfo*) * 4);
    sal_memset(tblRecordInfo_p, 0x0, sizeof(tableRecordInfo*) * 4);
    sal_memset(tblRecord_p, 0x0, sizeof(alg_kbp_rec*) * 4);
    sal_memset(tblRecHoldInfo_p, 0x0, sizeof(tableRecordInfo*) * 4);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (i=0; i < nof_searches; i++) {

        tblInfo_p[i] = &refAppData_p->g_gtmInfo[tbl_id+i].tblInfo;

        
        tblRecHoldInfo_p[i] = tblRecordInfo_p[i] = tblInfo_p[i]->tblRecordInfo_p;
    }

    numRecs = tblInfo_p[0]->max_recCount; 

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "\n\tPerforming LTR#%d searches\n"), ltr_num));

    iter = 0;

    
    for( iter = 0; iter < numRecs; iter++ )
    {
        
        sal_memset(&cmp_rslt,0,sizeof(struct kbp_search_result));
        sal_memset(&exp_result,0,sizeof(arad_kbp_rslt));
        sal_memset(mkey,0,80);

        
        for(i = 0; i < nof_searches; i++)
        {
            exp_result.m_resultValid[i] = 1;
            exp_result.m_hitOrMiss[i]   = 1;
            exp_result.m_hitDevId[i]    = 0;
            exp_result.m_hitIndex[i]    = tblRecordInfo_p[i]->index;
        }

        
        switch (ltr_num) {
        case ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF:

            exp_result.m_respType[0] = 0x2;
            exp_result.m_respType[1] = 0x1;

            
            sal_memcpy(&exp_result.m_AssocData[0][2], &tblRecordInfo_p[0]->assoData[2], 6);

            
            sal_memcpy(&exp_result.m_AssocData[1][2], &tblRecordInfo_p[1]->assoData[2], 2);

            tblRecord_p[0] = &(tblRecordInfo_p[0]->record);
            sal_memcpy(&mkey[0],tblRecord_p[0]->m_data, 10);

            tblRecord_p[1] = &(tblRecordInfo_p[1]->record);
            sal_memcpy(&mkey[6],tblRecord_p[1]->m_data, 4);
            break;

        case ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF:

            exp_result.m_respType[0] = 0x1;

            
            sal_memcpy(&exp_result.m_AssocData[0][1], &tblRecordInfo_p[0]->assoData[1], 3);

            tblRecord_p[0] = &(tblRecordInfo_p[0]->record);
            sal_memcpy(&mkey[0],tblRecord_p[0]->m_data, 9);
            break;

        default:
            break;
        }

        
         {
             i = 0;
             sal_memcpy(db_key,tblRecord_p[i]->m_data,10);
             res = kbp_db_search(refAppData_p->g_gtmInfo[i].tblInfo.db_p,db_key, &entry_in_db1, &index1, &priority1);
             if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                 LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                 "Error in %s(): kbp_db_search failed index %d: %s\n"), FUNCTION_NAME(), i+1, kbp_get_status_string(res)));
             }
             LOG_INFO(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "\n\t- Search result for db = %d,index = %x\n"), i,index1));

             ++i;
             sal_memset(db_key,0,10);
             sal_memcpy(db_key,tblRecord_p[i]->m_data,10);
             res = kbp_db_search(refAppData_p->g_gtmInfo[i].tblInfo.db_p,db_key, &entry_in_db2, &index2, &priority2);
             if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                 LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                 "Error in %s(): kbp_db_search failed index %d: %s\n"), FUNCTION_NAME(), i+1, kbp_get_status_string(res)));
             }

             LOG_INFO(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "\n\t- Search result for db = %d,index = %x\n"), i,index2));
         }

        
        KBP_STRY(kbp_instruction_search(ltr_config_info.inst_p, mkey, 0, &cmp_rslt));

        for(i = 0; i < nof_searches; i++) {
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n Hit/miss of search = %d is %x, index  = %x\n"),i,cmp_rslt.hit_or_miss[i],cmp_rslt.hit_index[i]));
        }

        for(i = 0; i<nof_searches;i++)
        {
            uint16_t j = 0;
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n Actual AD for Search = %d\n"),i));
            for(j = 0;j<16;j++)
            {
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "%d_"),cmp_rslt.assoc_data[i][j]));
            }
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n")));
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n Expected AD for Search = %d\n"),i));
            for(j = 0;j<16;j++)
            {
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "%d_"),exp_result.m_AssocData[i][j]));
            }
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n")));
        }

        if( iter && ((iter % 500) == 0 ) )
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n\t     - %u keys are searched "), iter));


        
    }

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "\n\t   Total number of keys searched [%u]\n"), iter));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in Perform_LTR_Searches()", 0, 0);
}
#endif

uint32 arad_kbp_test_ip4_rpf_NlmGenericTableManager(
    int unit,
    uint32 num_entries,
    uint32 *record_base_tbl,
    uint32 *ad_val_tbl
    )
{
    uint8  iter_tbl;
    genericTblMgrAradAppData *refAppData_p = AradAppData[unit];

    
    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "\n Creating the records to add to table\n")));
    CreateRecordsInTables_nosmt(
        refAppData_p,
        num_entries,
        record_base_tbl,
        ad_val_tbl);

    
    for( iter_tbl = 0; iter_tbl < ARAD_KBP_FRWRD_IP_NOF_TABLES; iter_tbl++ )
    {
        
        if (NULL != refAppData_p->g_gtmInfo[iter_tbl].tblInfo.db_p) {
           AddRecordsIntoTables( refAppData_p,iter_tbl);
        }
    }

    
#ifdef  SRCH_ENABLE
    if (NULL != refAppData_p->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0].tblInfo.db_p)
    {
        Perform_LTR_Searches(
           unit,
           refAppData_p,
           2,
           ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0,
           ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_RPF ,
           ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF
        );
    }

    if (NULL != refAppData_p->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].tblInfo.db_p)
    {
        Perform_LTR_Searches(
           unit,
           refAppData_p, 
           1, 
           ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, 
           ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_RPF,
           ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF
        );
    }
#endif
    return 0x0;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif 

