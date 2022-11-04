

#ifndef INCLUDED_KAPS_JR1_POOLMGR_H
#define INCLUDED_KAPS_JR1_POOLMGR_H

#include "kaps_jr1_fib_trienode.h"
#include "kaps_jr1_fib_ipm.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_it_mgr.h"
#include "kaps_jr1_algo_common.h"
#include "kaps_jr1_lpm_algo.h"








#define KAPS_JR1_MAX_NUM_IPT_ENTRIES_PER_POOL    (2*1024)



#define KAPS_JR1_INVALID_POOL_INDEX  (0xFFFF)


#define KAPS_JR1_INVALID_BLK_NUM     (0xFFFF)


#define KAPS_JR1_CONSTRUCT_POOL_ID(iptPoolIndex) (iptPoolIndex)


#define KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId) (poolId & 0xFFFF)


typedef struct kaps_jr1_rpts_in_pool
{
    kaps_jr1_trie_node *m_rptTrieNode_p; 
    struct kaps_jr1_rpts_in_pool *m_next_p; 
} kaps_jr1_rpts_in_pool;


typedef enum kaps_jr1_pool_type
{
    KAPS_JR1_IPT_POOL,   
} kaps_jr1_pool_type;


typedef struct kaps_jr1_pool_info
{
    struct kaps_jr1_fib_dba *m_dba_mgr;  
    kaps_jr1_pool_type m_poolType;     
    kaps_jr1_rpts_in_pool *m_rptsInPool_p;  
    uint8_t m_rptIdsFree[MAX_NUM_RPT_ENTRIES_IN_POOL / KAPS_JR1_BITS_IN_BYTE]; 
    uint8_t m_isInUse;                
    uint32_t m_poolIndex;
    uint32_t m_curNumRptEntries;        
    uint32_t m_curNumDbaEntries;        
    uint32_t m_maxNumDbaEntries;        
    uint32_t m_maxAllowedNumDbaEntries; 
    uint32_t m_width_1;                 
    uint32_t m_isReservedPool;          
    uint32_t m_isWidthLocked;           
} kaps_jr1_pool_info;


typedef enum kaps_jr1_pool_blk_status
{
    BLK_UNAVAILABLE,    
    BLK_FREE,           
    BLK_ALLOCATED       
} kaps_jr1_pool_blk_status;


typedef struct kaps_jr1_pool_mgr
{
    kaps_jr1_pool_info m_ipmPoolInfo[KAPS_JR1_MAX_NUM_POOLS];   
    kaps_jr1_pool_info m_apmPoolInfo[KAPS_JR1_MAX_NUM_POOLS];   
    kaps_jr1_nlm_allocator *m_alloc_p;                      
    kaps_jr1_fib_tbl_mgr *fibTblMgr;                        
    struct it_mgr *m_itMgr;                         
    struct kaps_jr1_ab_info *abs_allocated[KAPS_JR1_MAX_NUM_POOLS];   
    kaps_jr1_pool_blk_status m_dbaBlkStatus[KAPS_JR1_MAX_NUM_POOLS]; 
    uint32_t m_numActiveIptPools;                   
    uint32_t m_numActiveAptPools;                   
    uint32_t m_curNumRptEntriesInAllPools;          
    uint32_t m_numFreeAB;                           
    uint32_t m_totalNumAB;                          
    uint32_t m_numABsReleased;                      
    uint32_t m_force_trigger_width;                 
    uint32_t m_cur_max_trigger_length_1;            
    uint32_t m_are_pools_updated;                   
} kaps_jr1_pool_mgr;


struct kaps_jr1_wb_pool_info
{
    kaps_jr1_pool_type pool_type;      
    uint32_t pool_ix;           
    uint32_t blk_num;           
    uint32_t m_width_1;         
    uint32_t m_start_offset_1;  
    uint32_t m_isReservedPool;  
    uint32_t m_isWidthLocked;   
};


kaps_jr1_pool_mgr *kaps_jr1_pool_mgr_create(
    kaps_jr1_fib_tbl_mgr * fibTblMgr_p,
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t is_cascaded,
    struct kaps_jr1_c_list *ab_list);


NlmErrNum_t kaps_jr1_pool_mgr_assign_pool_to_rpt_entry(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t numIptEntriesRequired,
    uint32_t numAptEntriesRequired,
    uint32_t numPools,
    uint32_t ab_width_1,
    int32_t * was_dba_dynamic_allocated,
    NlmReasonCode * o_reason);


NlmErrNum_t kaps_jr1_pool_mgr_release_pool_for_rpt_entry(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t oldPoolId,
    uint32_t oldRptId);


NlmErrNum_t kaps_jr1_pool_mgr_incr_num_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    uint32_t incr,
    kaps_jr1_pool_type poolType);


NlmErrNum_t kaps_jr1_pool_mgr_decr_num_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    uint32_t decr,
    kaps_jr1_pool_type poolType);


uint32_t kaps_jr1_pool_mgr_get_num_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType);


uint32_t kaps_jr1_pool_mgr_get_max_allowed_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType);


uint32_t kaps_jr1_pool_mgr_get_max_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType);


kaps_jr1_trie_node *kaps_jr1_pool_mgr_get_rpt_with_most_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType);


kaps_jr1_ipm *kaps_jr1_pool_mgr_get_ipm_for_pool(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId);


uint32_t kaps_jr1_pool_mgr_get_num_active_pools(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_type poolType);


uint32_t kaps_jr1_pool_mgr_get_num_rpt_entries_in_pool(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType);


void kaps_jr1_pool_mgr_destroy(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_nlm_allocator * alloc_p);


NlmErrNum_t kaps_jr1_pool_mgr_createEntryData(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t * is_xy,
    NlmReasonCode * o_reason);


void kaps_jr1_pool_mgr_verify(
    kaps_jr1_pool_mgr * poolMgr);


void kaps_jr1_pool_mgr_get_dba_stats(
    kaps_jr1_pool_mgr * poolMgr,
    struct kaps_jr1_db *parent_db,
    uint32_t avg_num_pfx_per_brick,
    uint32_t cur_num_entries_in_db,
    uint32_t * numOfABsUsed,
    uint32_t * is_dba_brimmed,
    uint32_t * num_abs_released,
    uint32_t * extra_entry_estimate,
    uint32_t log_info);


NlmErrNum_t kaps_jr1_pool_mgr_wb_save_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset);


NlmErrNum_t kaps_jr1_pool_mgr_wb_restore_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_device_issu_read_fn read_fn,
    void *handle,
    uint32_t * nv_offset);


NlmErrNum_t kaps_jr1_pool_mgr_wb_add_rpt_entry_to_pool(
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_trie_node * rpt_node);




void kaps_jr1_pool_mgr_cr_save_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    struct kaps_jr1_wb_cb_functions *wb_fun);


void kaps_jr1_pool_mgr_cr_restore_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    struct kaps_jr1_wb_cb_functions *wb_fun);

int kaps_jr1_pool_mgr_is_pool_in_use(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType);

void kaps_jr1_pool_mgr_get_pool_stats(
    kaps_jr1_pool_mgr * poolMgr,
    struct NlmFibStats *fibStats);





#endif
