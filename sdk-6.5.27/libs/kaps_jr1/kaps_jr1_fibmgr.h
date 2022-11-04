

 

#ifndef INCLUDED_KAPS_JR1_FIBTBLMGR_H
#define INCLUDED_KAPS_JR1_FIBTBLMGR_H

#include "kaps_jr1_device.h"
#include "kaps_jr1_fib_cmn_prefix.h"
#include "kaps_jr1_arena.h"
#include "kaps_jr1_db.h"
#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_lpm_algo.h"

#include "kaps_jr1_externcstart.h"

#define KAPS_JR1_FIB_INVALID_INDEX               (0xFFFFFFFF)

typedef struct kaps_jr1_fib_resource_usage
{
    uint16_t m_numOfABsUsed;    
    uint16_t m_uitUsedInMb;     
    uint32_t m_udaUsedInKb;     
    uint32_t m_estCapacity;     
    uint32_t m_numABsReleased;  

} kaps_jr1_fib_resource_usage;

struct kaps_jr1_fib_tbl;


typedef void (
    *kaps_jr1_fib_prefix_index_changed_app_cb_t) (
    void *client_p,
    void *fibTbl_p,             
    kaps_jr1_pfx_bundle * b,
    uint32_t oldIndex,          
    uint32_t newIndex           
    );


typedef struct kaps_jr1_fib_tbl_mgr
{
    kaps_jr1_nlm_allocator *m_alloc_p;      

    struct kaps_jr1_device *m_devMgr_p;

    void *m_shadowDevice_p;

    struct kaps_jr1_fib_tbl_list *m_fibTblList_p;   

    struct kaps_jr1_fib_tbl *m_curFibTbl;

    uint8_t m_numOfDevices;     

    uint16_t m_fibTblCount;     

    int32_t m_IsConfigLocked;   

    struct kaps_jr1_trie_global *m_trieGlobal;

    struct kaps_jr1_trie_global *m_core0trieGlobal;

    struct kaps_jr1_trie_global *m_core1trieGlobal;

    kaps_jr1_fib_prefix_index_changed_app_cb_t m_indexChangeCallBackFn;     

    struct kaps_jr1_lpm_mgr *m_lpm_mgr;           

    struct kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *m_devWriteCallBackFns;

    uint32_t m_arenaSizeInBytes;

    uint32_t m_maxTblWidth_8;   

    uint32_t m_bufferKapsABWrites;      

    kaps_jr1_fib_resource_usage m_resourceUsage;    

    uint8_t m_trigDataBuffer[KAPS_JR1_HW_LPM_MAX_GRAN / 8];
} kaps_jr1_fib_tbl_mgr;

typedef struct kaps_jr1_fib_tbl_index_range
{
    uint32_t m_indexLowValue;   
    uint32_t m_indexHighValue;  
} kaps_jr1_fib_tbl_index_range;     

typedef struct kaps_jr1_fib_tbl
{
    kaps_jr1_fib_tbl_mgr *m_fibTblMgr_p;    

    kaps_jr1_nlm_allocator *m_alloc_p;      

    uint8_t m_tblId;            

    uint8_t m_coreId;           

    uint16_t m_width;           

    uint32_t m_numPrefixes;     

    uint32_t m_cascaded_fibtbl;

    kaps_jr1_fib_tbl_index_range m_tblIndexRange;   
    kaps_jr1_device_issu_read_fn wb_read_fn;
    kaps_jr1_device_issu_write_fn wb_write_fn;

    void *m_dependentTbls_p;
    

    struct kaps_jr1_lpm_trie *m_trie;

    uint16_t m_startBit;
    uint16_t m_endBit;
    struct NlmFibStats m_fibStats;      
    struct kaps_jr1_db *m_db;

    struct kaps_jr1_c_list wb_prefixes;     
    struct kaps_jr1_c_list wb_apt_prefixes; 
    struct kaps_jr1_c_list wb_apt_pfx_copies;       
    uint32_t ad_handle_zero_size_wb;

    uint32_t *cr_user_handles;  

} kaps_jr1_fib_tbl;

typedef struct kaps_jr1_fib_tbl_list
{
#include "kaps_jr1_dbllinklistdata.h"
    kaps_jr1_fib_tbl *m_fibTbl_p;
} kaps_jr1_fib_tbl_list;

extern kaps_jr1_fib_tbl_mgr *kaps_jr1_ftm_init(
    kaps_jr1_nlm_allocator * alloc_p,
    struct kaps_jr1_device *devMgr_p,
    void *client_p,
    kaps_jr1_fib_prefix_index_changed_app_cb_t indexChangedAppCb,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_destroy(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason);

extern kaps_jr1_fib_tbl *kaps_jr1_ftm_create_table(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t fibTblId,           
    kaps_jr1_fib_tbl_index_range * fibTblIndexRange,
    uint16_t fibMaxPrefixLength,
    struct kaps_jr1_db *db,
    uint32_t is_cascaded,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_destroy_table(
    kaps_jr1_fib_tbl * fibTbl,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_add_prefix(
    struct kaps_jr1_lpm_db *,       
    struct kaps_jr1_lpm_entry *entry,       
    NlmReasonCode * o_reason);
extern NlmErrNum_t kaps_jr1_ftm_wb_add_prefix(
    kaps_jr1_fib_tbl * fibTbl,      
    struct kaps_jr1_lpm_entry *entry,       
    uint32_t index,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_wb_add_apt_prefix(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_update_ad(
    kaps_jr1_fib_tbl * fibTbl,      
    struct kaps_jr1_lpm_entry *entry,       
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_update_ad_address(
    kaps_jr1_fib_tbl * fibTbl,      
    struct kaps_jr1_lpm_entry *entry,       
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_delete_prefix(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_get_index(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry *entry,
    int32_t * nindices,
    int32_t ** indices,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_locate_lpm(
    kaps_jr1_fib_tbl * fibTbl,
    uint8_t * key,
    struct kaps_jr1_lpm_entry **entry_pp,
    uint32_t * index,
    uint32_t * length,
    NlmReasonCode * o_reason);

extern NlmErrNum_t kaps_jr1_ftm_get_resource_usage(
    kaps_jr1_fib_tbl * fibTbl,
    kaps_jr1_fib_resource_usage * rxcUsage,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_ftm_get_prefix_handle(
    kaps_jr1_fib_tbl * fibTbl,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_jr1_lpm_entry **entry);

NlmErrNum_t kaps_jr1_ftm_update_hash_table(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry *entry,
    uint32_t is_del);

kaps_jr1_status kaps_jr1_ftm_wb_process_rpt(
    kaps_jr1_fib_tbl * fib_tbl);
kaps_jr1_status kaps_jr1_ftm_wb_save(
    kaps_jr1_fib_tbl * fib_tbl,
    uint32_t * nv_offset);
kaps_jr1_status kaps_jr1_ftm_wb_restore(
    kaps_jr1_fib_tbl * fib_tbl,
    uint32_t * nv_offset);

kaps_jr1_status kaps_jr1_ftm_cr_restore(
    kaps_jr1_fib_tbl * fib_tbl,
    struct kaps_jr1_wb_cb_functions *cb_fun);

NlmErrNum_t kaps_jr1_ftm_process_hit_bits(
    kaps_jr1_fib_tbl * fibTbl,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_ftm_calc_stats(
    kaps_jr1_fib_tbl * fibTbl);

NlmErrNum_t kaps_jr1_ftm_get_prefix_location(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry *lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_ftm_get_algo_hit_bit_value(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value,
    NlmReasonCode * o_reason);

#include "kaps_jr1_externcend.h"

#endif
