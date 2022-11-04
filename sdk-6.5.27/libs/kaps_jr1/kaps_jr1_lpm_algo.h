

#ifndef __KAPS_JR1_LPM_ALGO_H
#define __KAPS_JR1_LPM_ALGO_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_resource.h"
#include "kaps_jr1_db_internal.h"
#include "kaps_jr1_pool.h"
#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_algo_common.h"
#include "kaps_jr1_hw_limits.h"

#ifdef __cplusplus
extern "C"
{
#endif



    struct kaps_jr1_pfx_bundle;

#define MAX_COPIES_PER_LPM_DB   (2)



#define DBLIST_TO_LPM_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_jr1_lpm_entry, db_node)



    enum lpm_entry_status
    {
        PENDING = 0,     
        COMMITTED = 1    
    };



    struct kaps_jr1_lpm_entry
    {
        KAPS_JR1_ENTRY_STRUCT_MEMBERS uint32_t meta_priority:2;
                              
        struct kaps_jr1_pfx_bundle *pfx_bundle;
                                        
    };

    struct kaps_jr1_lpm_cr_entry_info
    {
        volatile uint8_t marked_for_delete;  
        volatile uint8_t status;  
        volatile uint8_t seq_num; 
        volatile uint32_t new_index;
                                  
        uint32_t entry_user_handle;  
        uint32_t ad_user_handle;  
        uint32_t hb_user_handle;  
        uint32_t hb_bit_no;       
    };


    struct kaps_jr1_lpm_db
    {

        
        struct kaps_jr1_db db_info;        
        struct kaps_jr1_acl_db_common *common;
                                       
        struct range_mgr *range_mgr;  

        
        struct kaps_jr1_fib_tbl *fib_tbl;
        struct kaps_jr1_fib_tbl *core0_fib_tbl;
        struct kaps_jr1_fib_tbl *core1_fib_tbl;
        struct kaps_jr1_db **resolution_dbs;
                                     
        struct kaps_jr1_c_list pending;     
        struct kaps_jr1_c_list tmp_list;     

        uint16_t core0_is_full;      
        uint16_t core1_is_full;      
        uint16_t current_core;                    
        uint16_t num_used_ab;      
        uint16_t num_released_ab;  
        uint16_t num_used_ait;     
        uint32_t used_uda_kb;      

        uint16_t num_ab_no_compression;      
        uint16_t num_ab_compression_1;       
        uint16_t num_ab_compression_2;       
        uint8_t num_resolution_db;           
        uint8_t meta_priority_map;           
        uint8_t is_entry_delete_in_progress; 
        uint8_t multiple_meta_priority;      
            POOL_DEF(
    kaps_jr1_lpm_entry) lpm_entry_pool;             
    };

    struct kaps_jr1_small_bb_stats
    {
        uint32_t numBricksForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
        uint32_t numEmptyBricksForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
                                                                  
        uint32_t numPfxForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
        uint32_t numHolesForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
                                                             
        uint32_t numLsnForEachSize[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
                                                              
        uint32_t numHolesForEachLsnSize[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
                                                                   
        uint32_t totalNumEntriesInLsns;
                                      
        uint32_t totalNumHolesInLsns;
                                    
        uint32_t maxLsnWidth;       
        uint32_t maxBricks;
                        
        uint32_t numUsedBricks;
                            
        uint32_t numUsedButEmptyBricks;
                                    
        uint32_t numFreeBricks;
                            
    };


    struct NlmFibStats
    {
        uint32_t numRPTWrites;   
        uint32_t numRITWrites;   
        uint32_t numIITWrites;   
        uint32_t numIPTWrites;   
        uint32_t numAPTWrites;   
        uint32_t numAITWrites;   
        uint32_t numHitBitReads; 
        uint32_t numHitBitWrites;
        uint32_t numHitBitCopy;  
        uint32_t numRptSplits;   
        uint32_t numRptPushes;   
        uint32_t numRptMoves;    
        uint32_t numLsnGiveOuts; 
        uint32_t numLsnPush;     
        uint32_t numLsnMerges;   
        uint32_t numRelatedLsnMerges;
                                 
        uint32_t numLsnShrinks;  
        uint32_t numIXCompactionCalls;
                                   
        uint32_t numUDACompactionCalls;
                                    
        uint32_t numUDADynamicAllocCalls;
                                      
        uint32_t numUDAExpansionCalls;
                                   
        uint32_t numLSNSbc;     
        uint32_t numLSNRmw;     
        uint32_t numLSNClr;     
        uint32_t numMetaDataWrite;
                                
        uint32_t numPIOWrites;  
        uint32_t numADWrites;   
        uint32_t numOfIPTShuffle;
                                
        uint32_t numOfAPTShuffle;
                                
        uint32_t numAptEntriesAdded;
        uint32_t numAptEntriesDeleted;
                                    
        uint32_t numLmpsofarPfx;    
        uint32_t numAptLongPfx;     
        uint32_t numLmpsofarHitInSearch;
                                       
        uint32_t numMissesInSearch;     
        uint32_t totalRptBytesLoppedOff;
                                       
        uint32_t maxNumRptSplitsInOneOperation;
                                            
        uint32_t maxNumIptSplitsInOneOperation;
                                            
        uint32_t numRecursiveRptSplits;     
        uint32_t numRecursiveIptSplits;     

        uint32_t numBricksForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
        uint32_t numEmptyBricksForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
                                                                  
        uint32_t numPfxForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
        uint32_t numHolesForEachGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
                                                             
        uint32_t numLsnForEachSize[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
                                                              
        uint32_t numHolesForEachLsnSize[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
                                                                   
        uint32_t totalNumHolesInLsns;
                                    

        uint32_t numHolesInLastBrick; 
        uint32_t numHolesInNonLastBricks; 

        uint32_t numLsnsWithPrefixMatchingDepth; 
        uint32_t numLsnsThatCanbePushedDeeper; 

        uint32_t maxLsnWidth;       
        uint32_t initialLsnWidth;

        uint32_t numRegions;
        uint32_t numUsedBricks;
                            
        uint32_t numUsedButEmptyBricks;
                                    
        uint32_t numPartiallyOccupiedBricks;
                                         
        uint32_t numLastBricksThatAreEmpty; 
        uint32_t numNonLastBricksThatAreEmpty; 
                                         
        uint32_t numFreeBricks;
                            

        uint32_t numUsedUitBanks;
                              

        uint32_t numTrieNodes;
                            
        uint32_t numRPTEntries;
                            
        uint32_t numIPTEntries;
                            
        uint32_t numAPTLmpsofarEntries;
                                    

        uint32_t numUsedAds2Slots; 

        uint32_t numFreeAds2Slots; 

        uint32_t rmw_lpus[KAPS_JR1_HW_MAX_LPUS_PER_LSN * 8];
                                                     
        uint32_t sbc_lpus[KAPS_JR1_HW_MAX_LPUS_PER_LSN]; 
        uint32_t sbc_clr_lpus[KAPS_JR1_HW_MAX_LPUS_PER_LSN]; 
        uint32_t clr_lpus[KAPS_JR1_HW_MAX_LPUS_PER_LSN]; 

        uint32_t rpt_lopoff_info[KAPS_JR1_LPM_KEY_MAX_WIDTH_8 + 1];
                                                          
        struct kaps_jr1_small_bb_stats kaps_jr1_small_bb_stats;

        uint32_t rxcTotalNumABAssigned;

        uint32_t numTotalIptAB;
        uint32_t numTotal80bIptAB;
        uint32_t numTotal160bIptAB;
        uint32_t numTotal80bSmallIptAB;
        uint32_t numTotal160bSmallIptAB;
        uint32_t numDupIptAB;

        uint32_t numTotalAptAB;
        uint32_t numTotal80bAptAB;
        uint32_t numTotal160bAptAB;
        uint32_t numTotal80bSmallAptAB;
        uint32_t numTotal160bSmallAptAB;
        uint32_t numDupAptAB;

        uint32_t num40bTriggers;
        uint32_t num80bTriggers;
        uint32_t num160bTriggers;
        uint32_t avgTrigLen;

        uint32_t maxNumRptSplitsInANode;
        uint32_t depthofNodeWithMaxRptSplits;
        uint32_t idOfNodeWithMaxRptSplits;

        uint32_t numTriggersInAB[KAPS_JR1_MAX_NUM_POOLS];
        uint32_t widthOfAB[KAPS_JR1_MAX_NUM_POOLS];
        uint32_t isABUsed[KAPS_JR1_MAX_NUM_POOLS];
        uint32_t isIPTAB[KAPS_JR1_MAX_NUM_POOLS];
        uint32_t numColsInAB[KAPS_JR1_MAX_NUM_POOLS];
    };


    struct kaps_jr1_lpm_mgr
    {
        struct kaps_jr1_fib_tbl_mgr *fib_tbl_mgr;
                                         
        struct kaps_jr1_db *resource_db;         
        struct kaps_jr1_db *curr_db;        
        kaps_jr1_db_index_callback callback_fn;
                                        
        void *callback_handle;         
        uint32_t is_config_locked;     
    };


    kaps_jr1_status kaps_jr1_lpm_db_get_fib_stats(
    struct kaps_jr1_lpm_db *db,
    struct NlmFibStats *fib_stats_pp);

    kaps_jr1_status kaps_jr1_lpm_db_print_fib_stats(
    struct kaps_jr1_db *db,
    struct NlmFibStats *fib_stats_p,
    struct kaps_jr1_db_stats *user_stats_p);

    struct kaps_jr1_wb_prefix_info
    {
        uint8_t pfx_len;
        uint8_t pfx_copy;
        uint8_t ad_seq_num;
        uint8_t meta_priority;
        uint8_t core_id;
        uint32_t index;
        uintptr_t user_handle;
    };

    struct kaps_jr1_wb_prefix_data
    {
        struct kaps_jr1_wb_prefix_info info;
        uint32_t ad_user_handle;
        uint32_t hb_no;
        uint32_t hb_user_handle;
        uint8_t *data;
        uint8_t *ad;
        struct kaps_jr1_list_node kaps_jr1_list_node;
    };

#define WBPFXLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_jr1_wb_prefix_data, kaps_jr1_list_node);








    kaps_jr1_status kaps_jr1_lpm_cr_remap_prefix(
    struct kaps_jr1_fib_tbl *fib_tbl,
    kaps_jr1_pfx_bundle * newPfxBundle,
    uint32_t * is_pending);



    void kaps_jr1_print_detailed_stats_html(
    FILE * f,
    struct kaps_jr1_device *dev,
    struct kaps_jr1_db *db);

    void kaps_jr1_lpm_replace_device(
    struct kaps_jr1_db *db,
    int32_t core);


    kaps_jr1_status kaps_jr1_db_is_reduced_algo_levels(
    struct kaps_jr1_db *db);



#ifdef __cplusplus
}
#endif
#endif                          
