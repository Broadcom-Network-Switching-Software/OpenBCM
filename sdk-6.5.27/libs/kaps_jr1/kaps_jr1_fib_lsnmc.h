

#ifndef INCLUDED_KAPS_JR1_LSNMC_H
#define INCLUDED_KAPS_JR1_LSNMC_H

#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_fib_cmn_nstblrqt.h"
#include "kaps_jr1_fib_triedata.h"
#include "kaps_jr1_ix_mgr.h"
#include "kaps_jr1_fib_poolmgr.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_fib_hw.h"
#include "kaps_jr1_hb_internal.h"

#include "kaps_jr1_externcstart.h"

#define KAPS_JR1_GIVEOUT_BUF_SIZE (KAPS_JR1_MAX_TMAX_VAL + 1)

#define KAPS_JR1_MAX_NUM_STORED_LSN_INFO (3)

#define KAPS_JR1_KEEP (1)
#define KAPS_JR1_GIVE (2)

typedef struct kaps_jr1_lpm_lpu_brick
{
    kaps_jr1_pfx_bundle **m_pfxes;  
    struct kaps_jr1_ix_chunk *m_ixInfo;     
    struct kaps_jr1_ad_db *ad_db;   
    struct kaps_jr1_lpm_lpu_brick *m_next_p;        
    uint16_t m_gran;            
    uint16_t m_numPfx;          
    uint32_t m_granIx:8;        
    uint32_t meta_priority:2;   
    uint32_t m_maxCapacity:16;  
    uint32_t m_hasReservedSlot:1;       
    uint32_t m_underAllocatedIx:1;      
} kaps_jr1_lpm_lpu_brick;



typedef struct kaps_jr1_lsn_mc_settings
{
    kaps_jr1_nlm_allocator *m_pAlloc;       
    kaps_jr1_fib_tbl *m_fibTbl;

    struct kaps_jr1_uda_mgr **m_pMlpMemMgr;
    struct uda_mem_chunk *m_pTmpMlpMemInfoBuf[KAPS_JR1_MAX_NUM_STORED_LSN_INFO];

    struct uda_mem_chunk *m_oldMlpMemInfoPtr;
    struct kaps_jr1_ix_chunk *m_oldIxChunkForLsn;

    struct kaps_jr1_ix_chunk *m_oldLpuIxChunk[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
    kaps_jr1_lpm_lpu_brick *m_oldLpuBrick[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_numBackupLpuBrickResources;

    uint8_t m_isPlaced[KAPS_JR1_MAX_NUM_PFX_IN_BUFFER];

    kaps_jr1_fib_prefix_index_changed_app_cb_t m_pAppCB;    
    void *m_pAppData;           

    hb_ix_changed_callback m_pHitBitCB;
    void *m_pHitBitCBData;

    uint8_t zero_value_array[KAPS_JR1_BKT_WIDTH_8];

    uint8_t common_bits_array[KAPS_JR1_LPM_KEY_MAX_WIDTH_8];

    struct kaps_jr1_device *m_device;

    uint32_t m_cmpLookupTable[256];

    uint16_t lpm_num_gran; 

    uint16_t m_lengthToGran[KAPS_JR1_HW_LPM_MAX_GRAN + 1];  
    uint16_t m_lengthToGranIx[KAPS_JR1_HW_LPM_MAX_GRAN + 1];

    uint16_t m_middleLevelLengthToGran[KAPS_JR1_HW_LPM_MAX_GRAN + 1];       
    uint16_t m_middleLevelLengthToGranIx[KAPS_JR1_HW_LPM_MAX_GRAN + 1];

    uint16_t m_max20bInPlacePfxInBrickForGranIx[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
    uint32_t m_maxTriggersInBrickForGranIx[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];

    uint16_t m_granIxToGran[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
    uint16_t m_numHolesForGranIx[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];

    uint16_t m_granIxToFormatValue[KAPS_JR1_MAX_NUM_GRANULARITIES];
    uint16_t m_middleLevelGranIxToFormatValue[KAPS_JR1_MAX_NUM_GRANULARITIES];

    uint16_t m_maxNumPfxInAnyBrick;

    uint32_t m_isGrowEnhEnable:1;
    uint32_t m_isPerLpuGran:1;
    uint32_t m_isAptPresent:1;
    uint32_t dynamic_alloc_enable:1;
    uint32_t m_isHardwareMappedIx:1;
    uint32_t m_isUnderAllocateLpuIx:1;
    uint32_t m_numUnderallocateLpus:4;
    uint32_t m_isShrinkEnabled:1;
    uint32_t m_isFullWidthLsn:1;
    uint32_t m_isMultiBrickAlloc:1;
    uint32_t m_areHitBitsPresent:1;
    uint32_t m_areIPTHitBitsPresent:1;
    uint32_t m_areRPTHitBitsPresent:1;
    uint32_t m_strictlyStoreLmpsofarInAds:1;

    uint32_t m_bdata_ads_size_1;

    uint32_t m_maxTriggersPerBktRow;

    
    uint32_t m_onlyPfxCopyInReservedSlot:1;
    uint32_t m_recomputeLmpOnDelete:1;
    uint32_t m_isJoinedUdc:1;
    uint32_t m_splitIfLowUda:1;
    uint32_t m_isAncestorLsnMergeEnabled:1;
    uint32_t m_isRelatedLsnMergeEnabled:1;
    uint32_t m_treat_20b_ad_as_inplace:1;
    uint32_t m_lpu_brick_width_1;
    uint32_t m_numMultiBricks;
    uint32_t m_num_extra_holes_per_lsn;
    uint32_t m_bottom_up_caution_height;        

    uint32_t m_tryLsnPush;

    uint32_t m_maxLpuPerLsn;
    uint32_t m_initialMaxLpuPerLsn;     

    uint32_t m_numJoinedSets;
    uint32_t m_numJoinedBricksInOneSet;

    uint8_t m_give_or_keep[KAPS_JR1_GIVEOUT_BUF_SIZE];
    uint32_t m_lsnid;
    uint8_t lsn_data_buffer[KAPS_JR1_HW_LPU_WIDTH_8 * KAPS_JR1_HW_MAX_LPUS_PER_LSN * 2];        

    uint8_t m_isLsnInfoValid[KAPS_JR1_MAX_NUM_STORED_LSN_INFO];

    uint32_t m_maxCapacityOfStoredLsnInfo[KAPS_JR1_MAX_NUM_STORED_LSN_INFO][KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_numBricksInStoredLsnInfo[KAPS_JR1_MAX_NUM_STORED_LSN_INFO];
    uint32_t m_startIxOfStoredLsnInfo[KAPS_JR1_MAX_NUM_STORED_LSN_INFO];
    uint32_t m_regionIdOfStoredLsnInfo[KAPS_JR1_MAX_NUM_STORED_LSN_INFO];

    int32_t m_numAllocatedBricksForAllLsns;
    uint32_t m_numJoinedSetsProcessed;

} kaps_jr1_lsn_mc_settings;


typedef struct kaps_jr1_lsn_mc
{

    kaps_jr1_lsn_mc_settings *m_pSettings;  
    struct kaps_jr1_ix_chunk *m_ixInfo;
    struct uda_mem_chunk *m_mlpMemInfo;
    struct kaps_jr1_lpm_trie *m_pTrie;      

    kaps_jr1_fib_tbl *m_pTbl;       

    
    kaps_jr1_pfx_bundle *m_pParentHandle;

    kaps_jr1_lpm_lpu_brick *m_lpuList;

    uint8_t m_numLpuBricks;

    
    uint16_t m_nNumPrefixes;

    
    uint16_t m_nLsnCapacity;

    
    uint32_t m_nAllocBase;

    
    uint16_t m_nNumIxAlloced;

    
    uint16_t m_numDeletes;

    uint32_t m_lsnId;

    
    uint32_t m_devid:4;
    uint32_t m_nDepth:12;
    uint32_t m_nLopoff:12;

    
    
    uint32_t m_bIsNewLsn:1;

    uint32_t m_bAllocedResource:1;
    uint32_t m_bIsCompacted:1;
    uint32_t m_bDoGiveout:1;

} kaps_jr1_lsn_mc;

typedef struct kaps_jr1_flat_lsn_data
{
    kaps_jr1_prefix *m_commonPfx;
    kaps_jr1_pfx_bundle *m_pfxesInLsn[KAPS_JR1_GIVEOUT_BUF_SIZE];
    struct kaps_jr1_ad_db *ad_colors[16];
    uint16_t meta_priority_colors[16];
    uint16_t pfx_color[KAPS_JR1_GIVEOUT_BUF_SIZE];
    uint16_t num_colors;
    uint16_t m_numPfxInLsn;
    uint16_t m_maxPfxLenInBits;
    kaps_jr1_pfx_bundle *m_iitLmpsofarPfx;
    uint8_t isGiveLsn;          
    kaps_jr1_lsn_mc *lsn_cookie;
    struct uda_mem_chunk *temp_uda_chunk;
} kaps_jr1_flat_lsn_data;

#define kaps_jr1_lsn_mc_get_prefix_count(self)               ((self)->m_nNumPrefixes)


kaps_jr1_lsn_mc_settings *kaps_jr1_lsn_mc_settings_create(
    kaps_jr1_nlm_allocator * pAlloc,
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_device *device,
    struct kaps_jr1_uda_mgr **pMlpMemMgr,
    kaps_jr1_fib_prefix_index_changed_app_cb_t pAppCB,
    void *pAppData,
    uint32_t numLpuPerRow,
    uint32_t numJoinedSets,
    uint32_t numJoinedBricksInOneSet);

void kaps_jr1_lsn_mc_settings_destroy(
    kaps_jr1_lsn_mc_settings * self);

uint32_t kaps_jr1_lsn_mc_get_ix_mgr_index(
    kaps_jr1_lsn_mc * self,
    uint32_t relative_index);

uint32_t kaps_jr1_lsn_mc_get_rel_index_in_lsn(
    kaps_jr1_lsn_mc * self,
    uint32_t actual_index);

kaps_jr1_lsn_mc *kaps_jr1_lsn_mc_create(
    kaps_jr1_lsn_mc_settings * settings,
    struct kaps_jr1_lpm_trie *pTrie,
    uint32_t depth);

void kaps_jr1_lsn_mc_destroy(
    kaps_jr1_lsn_mc * self);

void kaps_jr1_lsn_mc_free_single_lpu_brick(
    kaps_jr1_lpm_lpu_brick * curLpuBrick,
    kaps_jr1_nlm_allocator * alloc);

void kaps_jr1_lsn_mc_free_lpu_bricks(
    kaps_jr1_lpm_lpu_brick * curLpuBrick,
    kaps_jr1_nlm_allocator * alloc);

void kaps_jr1_lsn_mc_free_resources(
    kaps_jr1_lsn_mc * self);

void kaps_jr1_lsn_mc_free_resources_per_lpu(
    kaps_jr1_lsn_mc * self);

NlmErrNum_t kaps_jr1_lsn_mc_undo_acquire_resources(
    kaps_jr1_lsn_mc * self);

NlmErrNum_t kaps_jr1_lsn_mc_free_old_resources(
    kaps_jr1_lsn_mc * self);

NlmErrNum_t kaps_jr1_lsn_mc_backup_resources_per_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * brick,
    struct kaps_jr1_ix_chunk *oldIxInfo);

NlmErrNum_t kaps_jr1_lsn_mc_undo_acquire_resources_per_lpu(
    kaps_jr1_lsn_mc * self);

NlmErrNum_t kaps_jr1_lsn_mc_free_old_resources_per_lpu(
    kaps_jr1_lsn_mc * self);

NlmErrNum_t kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_submit_rqt(
    kaps_jr1_lsn_mc * self,
    NlmTblRqtCode rqtCode,
    struct kaps_jr1_lpm_entry *entry,
    struct kaps_jr1_lpm_entry **entrySlotInHash,
    NlmReasonCode * o_reason);

uint8_t *kaps_jr1_lsn_mc_calc_common_bits(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    uint32_t * retlen);

uint16_t kaps_jr1_lsn_mc_compute_gran(
    kaps_jr1_lsn_mc_settings * settings,
    uint32_t lengthAfterLopoff,
    uint16_t * o_granIx);

uint16_t kaps_jr1_lsn_mc_get_next_max_gran(
    kaps_jr1_lsn_mc_settings * settings,
    uint16_t gran,
    uint16_t next);

uint16_t kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(
    kaps_jr1_lsn_mc_settings * settings,
    struct kaps_jr1_ad_db *ad_db,
    uint32_t has_reservedSlot,
    uint16_t meta_priority,
    uint16_t gran);

uint32_t
kaps_jr1_lsn_mc_calc_max_triggers_in_brick(
    kaps_jr1_lsn_mc_settings * settings,
    uint16_t gran);


kaps_jr1_pfx_bundle *kaps_jr1_lsn_mc_locate_exact(
    kaps_jr1_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    uint32_t * match_brick_num);

kaps_jr1_pfx_bundle *kaps_jr1_lsn_mc_locate_lpm(
    kaps_jr1_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    struct kaps_jr1_lpm_entry *skipEntry,
    uint32_t * matchBrickIter,
    uint32_t * matchPos);

void kaps_jr1_lsn_mc_free_prefixes_safe(
    kaps_jr1_lsn_mc * self);

void kaps_jr1_lsn_mc_print(
    kaps_jr1_lsn_mc * self,
    FILE * fp);

void kaps_jr1_lsn_mc_printPrefix(
    kaps_jr1_lsn_mc * self,
    FILE * fp);

uint16_t kaps_jr1_lsn_mc_get_first_lpu_granIx(
    kaps_jr1_lsn_mc * self);

uint16_t kaps_jr1_lsn_mc_get_first_lpu_gran(
    kaps_jr1_lsn_mc * self);

void kaps_jr1_lsn_mc_update_prefix(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_pfx_bundle * b,
    uint32_t oldix,
    uint32_t nextix);

NlmErrNum_t kaps_jr1_lsn_mc_giveout(
    kaps_jr1_lsn_mc * originalLsn,
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_fib_tbl *fibTbl,
    kaps_jr1_flat_lsn_data * curGiveoutData_p,
    kaps_jr1_flat_lsn_data * keepData_p,
    kaps_jr1_flat_lsn_data * giveData_p,
    kaps_jr1_pfx_bundle * pfxBundleToInsert,
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t tryOnlyLsnPush,
    NlmReasonCode * o_reason);

kaps_jr1_flat_lsn_data *kaps_jr1_lsn_mc_create_flat_lsn_data(
    kaps_jr1_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_destroy_flat_lsn_data(
    kaps_jr1_nlm_allocator * alloc_p,
    kaps_jr1_flat_lsn_data * flatLsnData_p);

NlmErrNum_t kaps_jr1_lsn_mc_convert_lsn_to_flat_data(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_pfx_bundle * extraPfxArray[],
    uint32_t numExtraPfx,
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_convert_flat_data_to_lsn(
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    kaps_jr1_lsn_mc * self,
    uint32_t * doesLsnFit_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_acquire_resources(
    kaps_jr1_lsn_mc * self,
    uint32_t ixRqtSize,
    uint8_t numLpuBricks,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_acquire_resources_per_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * brick,
    uint32_t brickIter,
    uint32_t ixRqtSize,
    struct kaps_jr1_ad_db *ad_db,
    NlmReasonCode * o_reason);

void kaps_jr1_lsn_mc_assign_flat_data_colors(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * flatLsnData_p);

NlmErrNum_t kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * brick,
    uint32_t brickIter,
    struct kaps_jr1_ad_db *ad_db,
    struct kaps_jr1_ix_chunk **ixInfo_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lsn(
    kaps_jr1_lsn_mc * self,
    struct uda_mem_chunk *mlpMemInfo,
    struct kaps_jr1_ad_db *ad_db,
    struct kaps_jr1_ix_chunk **ixInfo_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_add_empty_bricks(
    kaps_jr1_lsn_mc * self,
    uint32_t pfxGran,
    uint32_t pfxGranIx,
    uint32_t totalNumBricks,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_store_old_lsn_info(
    kaps_jr1_lsn_mc * curLsn,
    uint32_t lsnInfoIndex);

NlmErrNum_t kaps_jr1_lsn_mc_sort_and_resize_per_lpu_gran(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(
    kaps_jr1_lsn_mc * curLsn,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_shrink_per_lsn_gran(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_shrink_per_lpu_gran(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason);

uint32_t kaps_jr1_lsn_mc_find_longest_prefix_length(
    kaps_jr1_lsn_mc * curLsn);

NlmErrNum_t kaps_jr1_lsn_mc_find_prefixes_in_path(
    kaps_jr1_lsn_mc * curLsn,
    uint8_t * pathDataToCheck,
    uint32_t pathLengthToCheck,
    uint32_t excludeExactLengthMatch,
    kaps_jr1_pfx_bundle * pfxArray[],
    uint32_t pfxLocationsToMove_p[],
    uint32_t * numPfxInPath,
    uint32_t *isExactMatchLenPfxPresent);

uint32_t
kaps_jr1_lsn_mc_find_if_next_brick_is_joined(
    kaps_jr1_lsn_mc * curLsn,
    uint32_t brick_iter);


struct kaps_jr1_ad_db *
kaps_jr1_lsn_mc_get_ad_db(
    kaps_jr1_lsn_mc *curLsn,
    kaps_jr1_lpm_lpu_brick *curBrick);


struct kaps_jr1_ix_mgr*
kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_ad_db *ad_db,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum);




NlmErrNum_t kaps_jr1_lsn_mc_verify(
    kaps_jr1_lsn_mc * self);


uint32_t kaps_jr1_lsn_mc_wb_create_lsn(
    kaps_jr1_lsn_mc * lsn_p,
    struct kaps_jr1_wb_lsn_info *lsn_info,
    NlmReasonCode * o_reason);


NlmErrNum_t kaps_jr1_lsn_mc_wb_insert_pfx(
    kaps_jr1_lsn_mc * lsn_p,
    uint32_t insert_index,
    struct kaps_jr1_lpm_entry *entry);


NlmErrNum_t kaps_jr1_lsn_mc_wb_store_prefixes(
    kaps_jr1_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle);


NlmErrNum_t kaps_jr1_lsn_mc_wb_restore_pfx_data(
    kaps_jr1_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_jr1_device_issu_read_fn read_fn,
    void *handle);





NlmErrNum_t kaps_jr1_lsn_mc_wb_prepare_lsn_info(
    kaps_jr1_lsn_mc * lsn_p,
    struct kaps_jr1_wb_lsn_info *lsn_info);


NlmErrNum_t
kaps_jr1_lsn_mc_chunk_move_per_lpu_gran(
    struct uda_mem_chunk *uda_chunk,
    int32_t to_region_id,
    int32_t to_offset,
    int32_t *size);



NlmErrNum_t
kaps_jr1_lsn_mc_handle_under_alloc(
    kaps_jr1_lsn_mc *Lsn, 
    kaps_jr1_lpm_lpu_brick *cuLpuBrick,
    uint32_t curBrickIter,
    uint32_t *ixRqtSize);

kaps_jr1_status
kaps_jr1_lsn_mc_ad_chunk_move(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_compaction_chunk *cur_playback_chunk);

#include <kaps_jr1_externcend.h>

#endif
