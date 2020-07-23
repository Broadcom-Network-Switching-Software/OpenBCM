



#ifndef INCLUDED_KAPS_LEGACY_H
#define INCLUDED_KAPS_LEGACY_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uint32_t NlmErrNum_t;
#define NLMCMOK 0


#define NLMERR_OK           (0)
#define NLMERR_FAIL         (1)
#define NLMERR_NULL_PTR     (2)
#define NLMERR_OPERATION_NO_SUPPORT (4)


#define NLM_STRY(A)                                                     \
    do                                                                  \
    {                                                                   \
        NlmErrNum_t __tmp_status = A;                                    \
        if (__tmp_status != NLMERR_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    } while(0)





enum
{
    NlmFalse = 0,  
    NlmTrue  = 1   
};



typedef enum NlmReasonCode_t {
    NLMRSC_REASON_OK = 0,
    NLMRSC_LOW_MEMORY,
    NLMRSC_INVALID_MEMALLOC_PTR,
    NLMRSC_INTERNAL_ERROR,

    
    NLMRSC_INVALID_XPT_PTR = 21,
    NLMRSC_INVALID_XPT_RQT_PTR,
    NLMRSC_INVALID_XPT_RSLT_PTR,
    NLMRSC_NOFREE_RQST,
    NLMRSC_NORQST_AVBL,
    NLMRSC_NORSLT_AVBL,
    NLMRSC_OPR_FAILURE,

    
    NLMRSC_INVALID_DEV_PTR = 41,
    NLMRSC_INVALID_DEVMGR_PTR,
    NLMRSC_INVALID_SHADOWDEV_PTR,
    NLMRSC_INVALID_KEY_PTR,
    NLMRSC_INVALID_SRCH_RSLT_PTR = 45,
    NLMRSC_INVALID_POINTER,
    NLMRSC_INVALID_OUTPUT,
    NLMRSC_INVALID_INPUT,
    NLMRSC_INVALID_REG_ADDRESS,
    NLMRSC_INVALID_DB_ADDRESS = 50,
    NLMRSC_INVALID_CB_ADDRESS,
    NLMRSC_INVALID_RANGE_ADDRESS,
    NLMRSC_INVALID_DATA,
    NLMRSC_INVALID_LTR_NUM,
    NLMRSC_INVALID_AB_NUM = 55,
    NLMRSC_INVALID_BLK_WIDTH,
    NLMRSC_INVALID_AB_INDEX,
    NLMRSC_DUPLICATE_DEVICE,
    NLMRSC_INVALID_PARAM,
    NLMRSC_DEV_MGR_CONFIG_LOCKED = 60,
    NLMRSC_DEV_MGR_CONFIG_NOT_LOCKED,
    NLMRSC_CASCADE_NOT_EXIST,
    NLMRSC_INVALID_PARENT,
    NLMRSC_INVALID_DEVICE_TYPE,
    NLMRSC_INVALID_NUM_OF_DEVICES = 65,
    NLMRSC_INVALID_DATA_LENGTH,
    NLMRSC_DATA_LENGTH_ADDRESS_MISMATCH,
    NLMRSC_READONLY_REGISTER,
    NLMRSC_WRITEONLY_REGISTER,
    NLMRSC_INVALID_DEVICE_ID,
    NLMRSC_INVALID_OPER_MODE,
    NLMRSC_INVALID_BANK,

    
    NLMRSC_INVALID_GENERIC_TM = 81,
    NLMRSC_INVALID_9K_TM,
    NLMRSC_INVALID_GENERIC_TABLE,
    NLMRSC_INVALID_GTM_BLKS_RANGE,
    NLMRSC_INVALID_APP_CALLBACK = 85,
    NLMRSC_INVALID_TABLEID,
    NLMRSC_INVALID_SEARCH_ATTRIBUTES,
    NLMRSC_INVALID_PS_DEPEND_ATTRIBUTES,
    NLMRSC_CONFIGURATION_LOCKED,
    NLMRSC_EXCESSIVE_NUM_TABLE_PAR_LTR = 90,
    NLMRSC_NO_LTR_CONFIGURED,
    NLMRSC_INVALID_BMR,
    NLMRSC_INVALID_RECORD,
    NLMRSC_DEVICE_DATABASE_FULL,
    NLMRSC_TABLE_FULL = 95,     
    NLMRSC_TABLE_LIMIT_EXCEEDED,        
    NLMRSC_RECORD_NOTFOUND,
    NLMRSC_INVALID_RANGE_ATTRIBUTES,
    NLMRSC_INVALID_KEY_NUM,
    NLMRSC_INVALID_RESULT_SEQ_NUM = 100,
    NLMRSC_INVALID_TABLE_WIDTH,
    NLMRSC_INVALID_TABLE_ASSO_WIDTH,
    NLMRSC_SEARCHATTRIB_NOT_DELETED,
    
    NLMRSC_INVALID_BANK_NUM,
    NLMRSC_INVALID_GTM_SB_BLKS_RANGE,

    
    NLMRSC_INVALID_RANGE_MGR_ATTR = 121,        
    NLMRSC_INVALID_RANGE_MGR,   
    NLMRSC_INVALID_DATABASE,    
    NLMRSC_DUPLICATE_DATABASE_ID,
    NLMRSC_INVALID_RANGE_DB_ATTR = 125, 
    NLMRSC_INVALID_RANGE,       
    NLMRSC_INVALID_MCOR_CONFIG,
    NLMRSC_INVALID_OUTPUT_RNG_PTR,      
    NLMRSC_INVALID_OUTPUT_NUM_PTR,      
    NLMRSC_DUPLICATE_RANGE_ID = 130,
    NLMRSC_INVALID_RANGE_SELECTION,
    NLMRSC_RANGE_DATABASE_FULL,
    NLMRSC_NO_SUPPORT_FOR_THIS_COMB,
    NLMRSC_NO_SUPPORT_FOR_SEARCHDB,

    
    NLMRSC_INVALID_TBLID_LEN = 161,
    NLMRSC_INVALID_FIB_MGR,
    NLMRSC_INVALID_FIB_TBL,
    NLMRSC_DUPLICATE_FIB_TBL_ID,
    NLMRSC_INVALID_FIB_BLKS_RANGE = 165,
    NLMRSC_INVALID_FIB_TBL_INDEX_RANGE,
    NLMRSC_INVALID_FIB_MAX_PREFIX_LENGTH,
    NLMRSC_INVALID_FIB_START_BIT_IN_KEY,
    NLMRSC_INVALID_PREFIX,
    NLMRSC_INVALID_IDX_SPACE_MGR = 170,
    NLMRSC_INVALID_IDX_RANGE_PTR,
    NLMRSC_IDX_RANGE_FULL,
    NLMRSC_INVALID_IDX_RANGE_GROW_FAILED,
    NLMRSC_MLP_GROW_FAILED,
    NLMRSC_LOCATE_NODE_FAILED = 175,
    NLMRSC_DUPLICATE_PREFIX,
    NLMRSC_PREFIX_NOT_FOUND,    
    NLMRSC_INVALID_PREFIX_LEN,
    NLMRSC_UDA_ALLOC_FAILED,
    NLMRSC_DBA_ALLOC_FAILED,
    NLMRSC_PCM_ALLOC_FAILED,
    NLMRSC_IT_ALLOC_FAILED,
    NLMRSC_TMAX_EXCEEDED,       
    NLMRSC_OUT_OF_AD,

    
    NLMRSC_INVALID_IBF_LOCATION = 201,
    NLMRSC_INVALID_CLK_CYCLE,
    NLMRSC_INVALID_OPERATION,

    NLMRSC_INCORRECT_INST_TYPE = 205,
    NLMRSC_INSTRUCTION_OVERFLOW,
    NLMRSC_INSTRUCTION_ALREADY_SCHEDULED,
    NLMRSC_INVALID_NIP_VALUE,
    NLMRSC_INVALID_RETURN_AD_VALUE,

    NLMRSC_INVALID_RBF_LOC = 210,
    NLMRSC_INVALID_CAB_LOC,
    NLMRSC_INVALID_SRAM_SHIFT_VALUE,
    NLMRSC_INVALID_SRAM_SHIFT_DIR,
    NLMRSC_INVALID_JUMP_ON_AD,

    NLMRSC_INVALID_KEY_SUPERBLK_MAPPING = 215,
    NLMRSC_INVALID_BLK_RESULT_MAPPING,
    NLMRSC_COUNTER_WIDTH_NOT_SUPPORTED,
    NLMRSC_INVALID_COUNTER_CONFIG,
    NLMRSC_INVALID_COUNTER_COMMAND,

    NLMRSC_INVALID_DUA_ADDRESS,
    NLMRSC_INVALID_DUA_COMMAND,

    NLMRSC_INVALID_AD_FIELD_SELECT = 222,
    NLMRSC_INVALID_BRANCH_WAY,
    NLMRSC_INVALID_LATENCY_CONFIGURATION,

    NLMRSC_INVALID_ADDRESS,

    
    NLMRSC_MT_SPINLOCK_INIT_FAILED = 226,
    NLMRSC_MT_MUTEX_INIT_FAILED,
    NLMRSC_MT_SEM_INIT_FAILED,
    NLMRSC_MT_SPINLOCK_DESTROY_FAILED,
    NLMRSC_MT_MUTEX_DESTROY_FAILED,
    NLMRSC_MT_SEM_DESTROY_FAILED,
    NLMRSC_MT_TRIENODE_DOESNOT_EXIST,

    NLMRSC_MT_INVALID_THREAD_ID,
    NLMRSC_MT_THREAD_ALREADY_REGISTERED,

    
    NLMRSC_INVALID_MAC_TM,
    NLMRSC_INVALID_MAC_SB_BLKS_RANGE,
    NLMRSC_INVALID_MAC_TBL,
    NLMRSC_INVALID_MAC_TBL_TYPE,
    NLMRSC_INVALID_MAC_START_BYTE_IN_KEY,
    NLMRSC_INVALID_MAC_AD_WIDTH,
    NLMRSC_INVALID_MAC_AD,

    
    NLMRSC_INVALID_PORT_NUM,
    NLMRSC_INVALID_SMT_NUM,
    NLMRSC_INVALID_INST_TYPE,

    
    NLMRSC_INVALID_AC2BANK_MAPPING,
    NLMRSC_INVALID_P1CTXID_SHIFT_VALUE,

    
    NLMRSC_INVALID_UDA_SB_BLKS_RANGE,

    NLMRSC_XPT_FAILED,

    
    NLMRSC_REASON_UNKNOWN,
    NLMRSC_API_NOT_SUPPORTED,
    NLMRSC_NO_RECORDS_FOUND,
    NLMRSC_RETRY_WITH_RPT_SPLIT,
    NLMRSC_REASON_END
} NlmReasonCode;

typedef struct kaps_nlm_allocator kaps_nlm_allocator;




typedef void *(*kaps_nlm_allocator_alloc_t) (kaps_nlm_allocator *, size_t);

typedef void *(*kaps_nlm_allocator_sys_malloc_t) (size_t);


typedef void *(*kaps_nlm_allocator_calloc_t)
 (kaps_nlm_allocator *, size_t, size_t);


typedef void *(*kaps_nlm_allocator_resize_t)
 (kaps_nlm_allocator *, void *, size_t, size_t);


typedef void (*kaps_nlm_allocator_free_t)
 (kaps_nlm_allocator *, void *);

typedef void (*kaps_nlm_allocator_sys_free_t)
 (void *);


typedef void (*kaps_nlm_allocator_dtor_t)
 (kaps_nlm_allocator *);


typedef struct kaps_nlm_allocator_vtbl {
    const char *className;      
    kaps_nlm_allocator_alloc_t m_malloc;   
    kaps_nlm_allocator_calloc_t m_calloc;  
    kaps_nlm_allocator_resize_t m_resize;  
    kaps_nlm_allocator_free_t m_free;      
    kaps_nlm_allocator_dtor_t m_dtor;      
    const char *className1;     
    kaps_nlm_allocator_sys_malloc_t m_sysMalloc;    
    kaps_nlm_allocator_sys_free_t m_sysFree;        
} kaps_nlm_allocator_vtbl;


struct kaps_nlm_allocator {
    kaps_nlm_allocator_vtbl m_vtbl;        
    kaps_nlm_allocator_vtbl *m_vtbl_p;     
    void *m_clientData_p;       
    void *m_arena_info;              

};


extern kaps_nlm_allocator *kaps_nlm_allocator_ctor(kaps_nlm_allocator * self);


extern void kaps_nlm_allocator_config(kaps_nlm_allocator * self, kaps_nlm_allocator_vtbl * vtbl);




extern void kaps_nlm_allocator_dtor_body(kaps_nlm_allocator *);


void *kaps_nlm_allocator_malloc(kaps_nlm_allocator * self, size_t size);


void *kaps_nlm_allocator_calloc(kaps_nlm_allocator * self, size_t cnt, size_t size);


void *kaps_nlm_allocator_resize(kaps_nlm_allocator * self, void *memblk, size_t newSize, size_t oldSize);


void kaps_nlm_allocator_free(kaps_nlm_allocator * self, void *memblk);




#ifdef __cplusplus
}
#endif

#endif

