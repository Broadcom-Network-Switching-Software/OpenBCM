

#ifndef __KAPS_DEVICE_H
#define __KAPS_DEVICE_H



#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"
#include "kaps_init.h"
#include "kaps_hw_limits.h"

#ifdef __cplusplus
extern "C" {
#endif







struct kaps_device;



struct kaps_allocator;



struct kaps_db;



struct kaps_entry;




#define KAPS_SDK_VERSION "KAPS SDK TRUNK"

#define KAPS_ERROR_LIST_VERSION (2)






enum kaps_db_type {
    KAPS_DB_INVALID, 
    KAPS_DB_ACL,     
    KAPS_DB_LPM,     
    KAPS_DB_EM,      
    KAPS_DB_AD,      
    KAPS_DB_DMA,     
    KAPS_DB_HB,      
    KAPS_DB_COUNTER, 
    KAPS_DB_TAP,     
    KAPS_DB_STATS,   
    KAPS_DB_END      
};



enum kaps_hw_resource {
    KAPS_HW_RESOURCE_DBA,  
    KAPS_HW_RESOURCE_UDA,  
    KAPS_HW_RESOURCE_DBA_CORE,  
    KAPS_HW_RESOURCE_UDA_CORE,  
    KAPS_HW_RESOURCE_RANGE_UNITS, 
    KAPS_HW_RESOURCE_INVALID      
};








kaps_status kaps_device_init(struct kaps_allocator *alloc, enum kaps_device_type type, uint32_t flags,
                           void *xpt, struct kaps_device_config *config, struct kaps_device **device);



kaps_status kaps_device_destroy(struct kaps_device *device);



kaps_status kaps_device_lock(struct kaps_device *device);


enum kaps_restore_status {
    KAPS_RESTORE_NO_CHANGE,            
    KAPS_RESTORE_CHANGES_COMMITTED,    
    KAPS_RESTORE_CHANGES_ABORTED       
};






enum kaps_device_properties {
    KAPS_DEVICE_PROP_DESCRIPTION,                
    KAPS_DEVICE_PROP_TOTAL_NUM_BB,               
    KAPS_DEVICE_PROP_RESOURCE_FN,                
    KAPS_DEVICE_PROP_LTR_SM_UPDATE,              
    KAPS_DEVICE_PROP_INTERRUPT,                  
    KAPS_DEVICE_PROP_IFSR_THRESHOLD,             
    KAPS_DEVICE_PROP_LAST_PARITY_ERRORS,         
    KAPS_DEVICE_PROP_LAST_PARITY_ERRORS_STATUS,  
    KAPS_DEVICE_PROP_INJECT_PARITY_ERRORS,       
    KAPS_DEVICE_PROP_NON_REPAIRABLE_ABS,         
    KAPS_DEVICE_PROP_READ_TYPE,                  
    KAPS_DEVICE_PROP_DEBUG_LEVEL,                
    KAPS_DEVICE_PROP_INST_LATENCY,               
    KAPS_DEVICE_PRE_CLEAR_ABS,                   
    KAPS_DEVICE_PROP_LAST_UDA_PARITY_ERRORS,     
    KAPS_DEVICE_ADD_BROADCAST,                   
    KAPS_DEVICE_ADD_SELECTIVE_BROADCAST,         
    KAPS_DEVICE_BROADCAST_AT_XPT,                
    KAPS_DEVICE_PROP_CRASH_RECOVERY,             
    KAPS_DEVICE_PROP_DEFER_LTR_WRITES,           
    KAPS_DEVICE_PROP_CLR_DEV_ERR_BITS,           
    KAPS_DEVICE_PROP_DUMP_ON_ASSERT,             
    KAPS_DEVICE_PROP_CLOCK_RATE,                 
    KAPS_DEVICE_PROP_ADV_UDA_WRITE,              
    KAPS_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS,    
    KAPS_DEVICE_PROP_COUNTER_COMPRESSION,        
    KAPS_DEVICE_PROP_STAT_RECORD_SIZE,           
    KAPS_DEVICE_PROP_TAP_NULL_REC_SIZE,          
    KAPS_DEVICE_PROP_TAP_INGRESS_REC_SIZE,       
    KAPS_DEVICE_PROP_TAP_EGRESS_REC_SIZE,        
    KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_START_LOC, 
    KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_START_LOC, 
    KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_LEN, 
    KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_LEN,  
    KAPS_DEVICE_PROP_MIN_RESOURCE,               
    KAPS_DEVICE_PROP_FORCE_INVALIDATE_LOC,       
    KAPS_DEVICE_PROP_HANDLE_INTERFACE_ERRORS,    
    KAPS_DEVICE_PROP_INVALID                     
};




#define KAPS_AD_WIDTH_1 (20)

#define KAPS_SEARCH_INTERFACE_WIDTH_1 (160)

#define KAPS_SEARCH_INTERFACE_WIDTH_8 (20)

#define KAPS_DBA_WIDTH_8 (20)

#define KAPS_DBA_WIDTH_1 (160)

#define KAPS_REGISTER_WIDTH_1 (32)

#define KAPS_REGISTER_WIDTH_8 (4)

#define KAPS_HB_ROW_WIDTH_8 (2)

#define KAPS_HB_ROW_WIDTH_1 (16)







#define KAPS_DEFAULT_DEVICE_ID (0x0001)

#define KAPS_QUMRAN_DEVICE_ID (0x0002) 

#define KAPS_JERICHO_PLUS_DEVICE_ID (0x0003)

#define KAPS_QUX_DEVICE_ID  (0x0004)

#define KAPS_JERICHO_2_DEVICE_ID      (0x0005)




#define KAPS_JERICHO_PLUS_SUB_TYPE_FM0 (0)

#define KAPS_JERICHO_PLUS_SUB_TYPE_FM4 (1)

#define KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0 (2)

#define KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4 (3)



#define KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL (0)

#define KAPS_JERICHO_2_SUB_TYPE_THREE_LEVEL (1)

#define KAPS_JERICHO_2_SUB_TYPE_Q2A_THREE_LEVEL (2)

#define KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL (3)






#define KAPS_REVISION_REGISTER_ADDR (0x0)

#define KAPS_JERICHO_REVISION_REG_VALUE (0x00010000)

#define KAPS_QUMRAN_AX_REVISION_REG_VALUE (0x00020000)

#define KAPS_JERICHO_PLUS_FM0_REVISION_REG_VALUE (0x00030000)

#define KAPS_JERICHO_PLUS_FM4_REVISION_REG_VALUE (0x00030001)

#define KAPS_JERICHO_PLUS_JER_MODE_FM0_REVISION_REG_VALUE (0x00030002)

#define KAPS_JERICHO_PLUS_JER_MODE_FM4_REVISION_REG_VALUE (0x00030003)


#define KAPS_QUX_REVISION_REG_VALUE (0x00040000)

#define KAPS_JERICHO_2_REVISION_REG_VALUE (0x00050000)










struct kaps_dba_entry
{
    uint8_t key[KAPS_SEARCH_INTERFACE_WIDTH_8]; 
    uint32_t resv:4;       
    uint32_t is_valid:2;   
    uint32_t pad:2;       
};


struct kaps_ad_entry
{

    uint32_t ad:20;   
    uint32_t resv:12; 
    uint32_t resv1;
    uint32_t resv2;
    uint32_t resv3;
};




kaps_status kaps_device_set_property(struct kaps_device *device, enum kaps_device_properties property, ...);



kaps_status kaps_device_get_property(struct kaps_device *device, enum kaps_device_properties property, ...);



kaps_status kaps_device_thread_init(struct kaps_device *device, uint32_t tid, struct kaps_device **thr_device);



kaps_status kaps_device_print_html(struct kaps_device *device, FILE *fp);



kaps_status kaps_device_print_sw_state(struct kaps_device * device, FILE * fp);




kaps_status kaps_device_print(struct kaps_device *device, FILE *fp);



kaps_status kaps_device_get_ser_data(struct kaps_device *device, uint32_t blk_nr,
            uint32_t row_nr, uint32_t func_num, uint32_t nbytes, uint8_t *bytes);



const char * kaps_device_get_sdk_version(void);











typedef int32_t (*kaps_device_issu_read_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);


typedef int32_t (*kaps_device_issu_write_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);



kaps_status kaps_device_save_state(struct kaps_device *device, kaps_device_issu_read_fn read_fn, kaps_device_issu_write_fn write_fn, void *handle);



kaps_status kaps_device_save_state_and_continue(struct kaps_device *device, kaps_device_issu_read_fn read_fn, kaps_device_issu_write_fn write_fn, void *handle);




kaps_status kaps_device_restore_state(struct kaps_device *device, kaps_device_issu_read_fn read_fn, kaps_device_issu_write_fn write_fn, void *handle);


kaps_status kaps_device_reconcile_start(struct kaps_device *device);


kaps_status kaps_device_reconcile_end(struct kaps_device *device);





kaps_status kaps_device_lookup_index(struct kaps_device *device, uint32_t hw_index, struct kaps_db **db, struct kaps_entry **entry);



kaps_status kaps_device_dump(struct kaps_device * device, uint32_t dump_xml_data_with_entries, FILE *fp);





kaps_status kaps_device_start_transaction(struct kaps_device *device);



kaps_status kaps_device_end_transaction(struct kaps_device *device);


kaps_status kaps_device_query_restore_status(struct kaps_device *device, enum kaps_restore_status *status);


kaps_status kaps_device_clear_restore_status(struct kaps_device *device);









const char *kaps_device_get_last_error(struct kaps_device *device);





#ifdef __cplusplus
}
#endif
#endif 

