



#ifndef INCLUDED_KAPS_JR1_TRIEDATA_H
#define INCLUDED_KAPS_JR1_TRIEDATA_H

#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_fib_trienode.h"
#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_fibmgr_xk.h"
#include "kaps_jr1_fib_hw.h"
#include "kaps_jr1_pfx_hash_table.h"

#include "kaps_jr1_externcstart.h"

#define KAPS_JR1_DEFAULT_RPT_DEPTH       (8)

#define KAPS_JR1_JERICHO_PLUS_FM4_MAX_KEY_WIDTH_1 (148)

#define KAPS_JR1_MAX_TMAX_VAL  ((uint16_t)(KAPS_JR1_HW_LPU_WIDTH_1 / KAPS_JR1_HW_LPM_SMALLEST_GRAN ) * KAPS_JR1_HW_MAX_LPUS_PER_DB)

#define KAPS_JR1_MIN_ENTRIES_NEEDED_FOR_SPLIT (3)

#define KAPS_JR1_TRIE_FIX_POOLS_THRESHOLD (1000)

#define KAPS_JR1_MAX_NUM_RPT_UUID (KAPS_JR1_HW_MAX_PCM_ENTRIES)

#define KAPS_JR1_MAX_NUM_TRIE_NODES_FOR_RELATED_MERGE (10)

#define KAPS_JR1_TRIE_NUMSTACK   3  

#define KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY (256)

#define KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY ((32 * 1024) + 1024)

#define KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY (32 * 1024)

#define KAPS_JR1_MAX_NUM_PFX_IN_BUFFER (4 * 1024)

typedef struct kaps_jr1_ipt_brick_scratchpad
{
    uint32_t m_numOccupiedEntries[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_maxNumEntries[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_gran[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];

    uint32_t m_numActiveBricks;

    uint32_t m_prefixLens[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB * KAPS_JR1_MAX_PFX_PER_BKT_ARRAY_SIZE];
    uint32_t m_numPfx;

} kaps_jr1_ipt_brick_scratchpad;

enum KAPS_JR1_RPT_OPERATION
{
    KAPS_JR1_RPT_MOVE,
    KAPS_JR1_RPT_SPLIT,
    KAPS_JR1_RPT_PUSH
};


typedef struct kaps_jr1_trie_global
{
    kaps_jr1_nlm_allocator *m_alloc_p;

    struct kaps_jr1_fib_tbl_mgr *fibtblmgr;
    struct kaps_jr1_uda_mgr *m_mlpmemmgr[KAPS_JR1_MAX_NUM_DEVICES];
    int8_t is_common_uda_mgr_used[KAPS_JR1_MAX_NUM_DEVICES];

    kaps_jr1_pfx_bundle *m_tempPfxBrick[KAPS_JR1_MAX_NUM_PFX_IN_BUFFER];
    kaps_jr1_pfx_bundle *m_bricksPfxBuffer[KAPS_JR1_MAX_NUM_PFX_IN_BUFFER];

    uint32_t m_oldPoolId;
    uint32_t m_oldRptId;
    uint32_t m_old_rpt_uuid;

    uint32_t m_newPoolId;
    uint32_t m_newRptId;
    uint32_t m_new_rpt_uuid;

    uint32_t m_oldRptDepth;
    uint32_t m_newRptDepth;

    enum KAPS_JR1_RPT_OPERATION m_rptOp;

    uint32_t m_indexBufferIter;
    uint32_t *m_indexBuffer_p;
    uint8_t *m_isReserved160bTrigBuffer;

    uint32_t m_deleteRptAptLmpsofar;

    uint32_t m_maxIptEntriesForRptMove;
    uint32_t m_maxAptEntriesForRptMove;

    uint32_t m_iptEntriesMoveDelta;

    uint32_t m_numDeletes;

    struct kaps_jr1_pool_mgr *poolMgr;

    kaps_jr1_trie_node *m_rptNodesToSplitOrMove[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_rpt_nodes_to_split_or_move;

    struct kaps_jr1_trie_node *m_curRptNodeToSplit_p;
    struct kaps_jr1_trie_node *m_curRptNodeToPush_p;

    kaps_jr1_trie_node *m_newRptNode_p;

    uint32_t m_areAllPoolsHealthy;

    uint32_t m_isNearestIptAncestorSplit:1;
    uint32_t m_isAptPresent:1;
    uint32_t m_isIITLmpsofar:1;
    uint32_t m_pfxLocationsToMove[KAPS_JR1_MAX_TMAX_VAL];
    uint32_t m_pfxLocationsToClear[KAPS_JR1_MAX_TMAX_VAL];

    kaps_jr1_pfx_bundle *m_extraPfxArray[KAPS_JR1_MAX_TMAX_VAL];        

    kaps_jr1_trie_node *m_trieNodesForRelatedMerge[KAPS_JR1_MAX_NUM_TRIE_NODES_FOR_RELATED_MERGE];

    uint32_t m_numTrieNodesForMerge;

    
    uint8_t ab_enable[KAPS_JR1_HW_MAX_AB / KAPS_JR1_BITS_IN_BYTE];

    
    uint8_t ab_enable2[KAPS_JR1_HW_MAX_AB / KAPS_JR1_BITS_IN_BYTE];

    
    struct kaps_jr1_wb_cb_functions *wb_fun;

    struct kaps_jr1_ad_db *cur_ad_db;
    struct kaps_jr1_lsn_mc *curLsnToSplit;  

    kaps_jr1_trie_node *m_rptNodeForCalcIptApt;

    uint32_t useTrigLenForCalcIptApt;

    uint32_t use_reserved_abs_for_160b_trig;

    uint32_t m_are_reserved_160b_abs_initialized;

    uint32_t m_max_reserved_160b_trig_per_rpt;

    uint32_t m_move_reserved_160b_trig_to_regular_pool;

    uint32_t poolIdsForReserved160bTrig[2];

    int32_t m_rpt_uuid_table[KAPS_JR1_MAX_NUM_RPT_UUID];    

    uint32_t m_numReserved160bThatBecameRegular;

    uint32_t m_numRegularThatBecameReserved160b;

} kaps_jr1_trie_global;


typedef struct kaps_jr1_lpm_trie
{
    kaps_jr1_trie_global *m_trie_global;
    kaps_jr1_fib_tbl *m_tbl_ptr;
    struct kaps_jr1_lsn_mc_settings *m_lsn_settings_p;
    struct kaps_jr1_trie_node *m_roots_trienode_p;

    struct kaps_jr1_trie_node *m_cache_trienode_p;

    kaps_jr1_pfx_bundle *m_toadd_ses[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];  
    uint32_t m_num_items_in_to_add_ses;

    kaps_jr1_pfx_bundle *m_todelete_ses[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];       
    uint32_t m_num_items_in_to_delete_ses;

    kaps_jr1_trie_node *m_toupdate_sit[KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY];       
    uint32_t m_num_items_in_to_update_sit;

    struct kaps_jr1_lsn_mc *m_toupdate_lsns[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];   
    uint32_t m_num_items_in_to_update_lsns;

    struct kaps_jr1_lsn_mc *m_todelete_lsns[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];   
    uint32_t m_num_items_in_to_delete_lsns;

    kaps_jr1_trie_node *m_todelete_nodes[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];      
    uint32_t m_num_items_in_to_delete_nodes;

    kaps_jr1_trie_node *m_toupdate_rit[KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY];
    uint32_t m_num_items_in_to_update_rit;

    void *m_pendingGiveouts[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_pending_giveouts;

    void *m_completedGiveouts[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_completed_giveouts;

    kaps_jr1_trie_node *m_delayedTrieNode[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_delayed_trie_node;

    struct kaps_jr1_lsn_mc *m_delayedLsn[KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_delayed_lsn;

    uint32_t m_node_processing_stack[1024];

    struct kaps_jr1_trie_node **m_recurse_stack[KAPS_JR1_TRIE_NUMSTACK];
    NlmNsTrieNode__TraverseArgs *m_recurse_args_stack[KAPS_JR1_TRIE_NUMSTACK];
    uint8_t m_recurse_stack_inuse[KAPS_JR1_TRIE_NUMSTACK];
    uint8_t m_recurse_args_stack_inuse[KAPS_JR1_TRIE_NUMSTACK];
    struct kaps_jr1_trie_node **m_ancestors_space;  
    struct pfx_hash_table *m_hashtable_p;
    uint8_t m_doverify;         
    uint8_t m_alloc_failed;     
    uint8_t m_min_lopoff;       
    uint8_t m_expansion_depth;  
    uint8_t m_isRptSplitDueToIpt;
    uint8_t m_isRptSplitDueToApt;
    uint8_t m_ancestors_space_inuse;
    uint32_t m_numOfMCHoles;
    uint32_t m_maxDepth;
    uint32_t m_index_in_inst;
    uint32_t m_maxAllowedIptEntriesForRpt;
    uint32_t m_isRecursiveSplit:1;
    uint32_t m_isCompactionRequired:1;
    uint32_t m_isCompactionEnabled:1;
    uint32_t m_isOverAllocEnabled:1;
    uint32_t m_isCopyIptAndAptToNewPoolInProgress:1;

    
    uint32_t m_rpt_move_failed_during_ipt_gran_change;

    uint32_t m_numExtraIptEntriesNeededDuringGiveout;

    struct kaps_jr1_instruction *m_instruction;

    uint8_t *m_iptHitBitBuffer;
    uint8_t *m_rptTcamHitBitBuffer;

    NlmReasonCode m_rptSplitReasonCode;

    kaps_jr1_pfx_bundle *m_defaultRptPfxBundle_p[KAPS_JR1_MAX_CASCADED_DEVICES];

    kaps_jr1_ipt_brick_scratchpad m_iptBrickScratchpad;

} kaps_jr1_lpm_trie;


struct kaps_jr1_wb_rpt_entry_info
{
    uint32_t rpt_entry_len_1;                                         
    uint8_t rpt_entry[KAPS_JR1_RPB_WIDTH_8];                              
    uint32_t rpt_entry_location;                                      
    uint16_t rpt_id;                                                  
    uint16_t rpt_uuid;                                                
    uint32_t pool_id;                                                 
    uint32_t db_id;                                                   
    uint32_t num_ipt_entries;                                         
    uint32_t num_apt_entries;                                         
    uint32_t num_reserved_160b_trig;                                  
};



struct kaps_jr1_wb_lpu_brick_info
{
    uint16_t gran;                 
    uint16_t gran_ix;              
    uint32_t base_address;         
    uint8_t meta_priority;      
    uint8_t m_hasReservedSlot;  
    uint8_t m_underAllocatedIx; 
    uint16_t brick_ix_allocated;   
    struct kaps_jr1_ad_db *ad_db;       
};



struct kaps_jr1_wb_lsn_info
{
    uint32_t lsn_id;
    uint8_t num_lpus;                                                    
    uint8_t region_id;                                                   
    uint32_t offset;                                                     
    uint32_t base_address;                                               
    uint16_t ix_allocated;                                               
    struct kaps_jr1_wb_lpu_brick_info brick_info[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];     
    struct kaps_jr1_wb_lpu_brick_info special_entry_info;
};


struct kaps_jr1_wb_ipt_entry_info
{
    uint8_t ipt_entry_len_1;                                   
    uint8_t is_reserved_160b_trig;                            
    uint8_t ipt_entry[KAPS_JR1_LPM_KEY_MAX_WIDTH_8];  
    uint16_t blk_num;                                          
    uint16_t addr;                                             
    struct kaps_jr1_wb_lsn_info lsn_info;                               
};

struct kaps_jr1_trie_node **kaps_jr1_trie_get_stack_space(
    kaps_jr1_lpm_trie * self);

void kaps_jr1_trie_return_stack_space(
    kaps_jr1_lpm_trie * self,
    struct kaps_jr1_trie_node **stk);

void kaps_jr1_trie_node_return_stack_space_args(
    kaps_jr1_lpm_trie * self,
    NlmNsTrieNode__TraverseArgs * stk);

NlmNsTrieNode__TraverseArgs *kaps_jr1_trie_node_get_stack_space_args(
    kaps_jr1_lpm_trie * self);

void kaps_jr1_trie_incr_num_ipt_entries_in_subtrie(
    kaps_jr1_trie_node * node,
    uint32_t value);

void kaps_jr1_trie_decr_num_ipt_entries_in_subtrie(
    kaps_jr1_trie_node * node,
    uint32_t value);

uint16_t kaps_jr1_trie_get_num_rpt_bits_lopped_off(
    struct kaps_jr1_device *device,
    uint32_t depth);

NlmErrNum_t kaps_jr1_trie_compute_rpt_parent(
    kaps_jr1_trie_node * trienode,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_trie_commit_rpt_and_rit(
    kaps_jr1_trie_node * rptNode_p,
    NlmReasonCode * o_reason);

void kaps_jr1_trie_propagate_rpt_attributes(
    kaps_jr1_trie_node * curNode_p,
    kaps_jr1_trie_node * rptNode_p);

NlmErrNum_t kaps_jr1_trie_process_rpt_split_or_move(
    kaps_jr1_lpm_trie * self_p,
    NlmReasonCode * o_reason_p,
    uint32_t force_rpt_split);

NlmErrNum_t kaps_jr1_trie_obtain_rpt_uuid(
    kaps_jr1_trie_node * rptTrieNode);

NlmErrNum_t kaps_jr1_trie_release_rpt_uuid(
    kaps_jr1_trie_node * rptTrieNode);

NlmErrNum_t kaps_jr1_trie_get_algo_hit_bit_value(
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_lpm_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value);

NlmErrNum_t kaps_jr1_trie_convert_status_to_err_num(
    kaps_jr1_status status,
    NlmReasonCode * o_reason_p);

#include <kaps_jr1_externcend.h>

#endif 
