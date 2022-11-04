

#ifndef __KAPS_JR1_DEVICE_H
#define __KAPS_JR1_DEVICE_H



#include <stdint.h>
#include <stdio.h>

#include "kaps_jr1_errors.h"
#include "kaps_jr1_init.h"
#include "kaps_jr1_hw_limits.h"



#ifdef __cplusplus
extern "C" {
#endif







struct kaps_jr1_device;



struct kaps_jr1_allocator;



struct kaps_jr1_db;



struct kaps_jr1_entry;




#define KAPS_JR1_SDK_VERSION "KAPS_JR1 SDK TRUNK"

#define KAPS_JR1_ERROR_LIST_VERSION (2)






enum kaps_jr1_db_type {
    KAPS_JR1_DB_INVALID, 
    KAPS_JR1_DB_LPM,     
    KAPS_JR1_DB_EM,      
    KAPS_JR1_DB_AD,      
    KAPS_JR1_DB_DMA,     
    KAPS_JR1_DB_HB,      
    KAPS_JR1_DB_END      
};



enum kaps_jr1_hw_resource {
    KAPS_JR1_HW_RESOURCE_DBA,  
    KAPS_JR1_HW_RESOURCE_UDA,  
    KAPS_JR1_HW_RESOURCE_UDA_CORE,  
    KAPS_JR1_HW_RESOURCE_INVALID      
};








kaps_jr1_status kaps_jr1_device_init(enum kaps_jr1_device_pair_type pair_type,
    uint32_t flags,
    void *xpt,
    struct kaps_jr1_device **device);



kaps_jr1_status kaps_jr1_device_destroy(struct kaps_jr1_device *device);



kaps_jr1_status kaps_jr1_device_lock(struct kaps_jr1_device *device);


enum kaps_jr1_restore_status {
    KAPS_JR1_RESTORE_NO_CHANGE,            
    KAPS_JR1_RESTORE_CHANGES_COMMITTED,    
    KAPS_JR1_RESTORE_CHANGES_ABORTED       
};






enum kaps_jr1_device_properties {
    KAPS_JR1_DEVICE_PROP_DESCRIPTION,                
    KAPS_JR1_DEVICE_PROP_TOTAL_NUM_BB,               
    KAPS_JR1_DEVICE_PROP_DEBUG_LEVEL,                
    KAPS_JR1_DEVICE_PROP_CRASH_RECOVERY,             
    KAPS_JR1_DEVICE_PROP_DUMP_ON_ASSERT,             
    KAPS_JR1_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS,    
    KAPS_JR1_DEVICE_PROP_PRINT_HEAP_INFO,                 
    KAPS_JR1_DEVICE_PROP_INVALID                     
    
};




#define KAPS_JR1_SEARCH_INTERFACE_WIDTH_1 (160)

#define KAPS_JR1_SEARCH_INTERFACE_WIDTH_8 (20)

#define KAPS_JR1_DBA_WIDTH_8 (20)

#define KAPS_JR1_DBA_WIDTH_1 (160)

#define KAPS_JR1_REGISTER_WIDTH_1 (32)

#define KAPS_JR1_REGISTER_WIDTH_8 (4)

#define KAPS_JR1_HB_ROW_WIDTH_8 (2)

#define KAPS_JR1_HB_ROW_WIDTH_1 (16)







#define KAPS_JR1_DEFAULT_DEVICE_ID (0x0001)

#define KAPS_JR1_QUMRAN_DEVICE_ID (0x0002) 

#define KAPS_JR1_JERICHO_PLUS_DEVICE_ID (0x0003)

#define KAPS_JR1_QUX_DEVICE_ID  (0x0004)




#define KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM0 (0)

#define KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4 (1)

#define KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0 (2)

#define KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4 (3)






#define KAPS_JR1_REVISION_REGISTER_ADDR (0x0)

#define KAPS_JR1_JERICHO_REVISION_REG_VALUE (0x00010000)

#define KAPS_JR1_QUMRAN_AX_REVISION_REG_VALUE (0x00020000)

#define KAPS_JR1_JERICHO_PLUS_FM0_REVISION_REG_VALUE (0x00030000)

#define KAPS_JR1_JERICHO_PLUS_FM4_REVISION_REG_VALUE (0x00030001)

#define KAPS_JR1_JERICHO_PLUS_JER_MODE_FM0_REVISION_REG_VALUE (0x00030002)

#define KAPS_JR1_JERICHO_PLUS_JER_MODE_FM4_REVISION_REG_VALUE (0x00030003)


#define KAPS_JR1_QUX_REVISION_REG_VALUE (0x00040000)

#define KAPS_JR1_JERICHO_2_REVISION_REG_VALUE (0x00050000)






struct kaps_jr1_dba_entry
{
    uint8_t key[KAPS_JR1_SEARCH_INTERFACE_WIDTH_8]; 
    uint32_t resv:4;       
    uint32_t is_valid:2;   
    uint32_t pad:2;       
};


struct kaps_jr1_ad_entry
{

    uint32_t ad:20;   
    uint32_t resv:12; 
    uint32_t resv1;
    uint32_t resv2;
    uint32_t resv3;
};



typedef enum
{
    KAPS_JR1_MDB_LPM_DB_CONFIG_2_STAGE,
    KAPS_JR1_MDB_LPM_DB_CONFIG_3_STAGE,
    KAPS_JR1_MDB_LPM_NOF_DB_CONFIGS
} kaps_jr1_mdb_lpm_db_config_e;





kaps_jr1_status kaps_jr1_device_set_property(struct kaps_jr1_device *device, enum kaps_jr1_device_properties property, ...);



kaps_jr1_status kaps_jr1_device_get_property(struct kaps_jr1_device *device, enum kaps_jr1_device_properties property, ...);



kaps_jr1_status kaps_jr1_device_thread_init(struct kaps_jr1_device *device, uint32_t tid, struct kaps_jr1_device **thr_device);



kaps_jr1_status kaps_jr1_device_print_html(struct kaps_jr1_device *device, FILE *fp);



kaps_jr1_status kaps_jr1_device_print_sw_state(struct kaps_jr1_device * device, FILE * fp);




kaps_jr1_status kaps_jr1_device_print(struct kaps_jr1_device *device, FILE *fp);



kaps_jr1_status kaps_jr1_device_get_ser_data(struct kaps_jr1_device *device, uint32_t blk_nr,
            uint32_t row_nr, uint32_t func_num, uint32_t nbytes, uint8_t *bytes);



const char * kaps_jr1_device_get_sdk_version(void);











typedef int32_t (*kaps_jr1_device_issu_read_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);


typedef int32_t (*kaps_jr1_device_issu_write_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);



kaps_jr1_status kaps_jr1_device_save_state(struct kaps_jr1_device *device, kaps_jr1_device_issu_read_fn read_fn, kaps_jr1_device_issu_write_fn write_fn, void *handle);



kaps_jr1_status kaps_jr1_device_save_state_and_continue(struct kaps_jr1_device *device, kaps_jr1_device_issu_read_fn read_fn, kaps_jr1_device_issu_write_fn write_fn, void *handle);




kaps_jr1_status kaps_jr1_device_restore_state(struct kaps_jr1_device *device, kaps_jr1_device_issu_read_fn read_fn, kaps_jr1_device_issu_write_fn write_fn, void *handle);


kaps_jr1_status kaps_jr1_device_reconcile_start(struct kaps_jr1_device *device);


kaps_jr1_status kaps_jr1_device_reconcile_end(struct kaps_jr1_device *device);





kaps_jr1_status kaps_jr1_device_lookup_index(struct kaps_jr1_device *device, uint32_t hw_index, struct kaps_jr1_db **db, struct kaps_jr1_entry **entry);



kaps_jr1_status kaps_jr1_device_dump(struct kaps_jr1_device * device, uint32_t dump_xml_data_with_entries, FILE *fp);





kaps_jr1_status kaps_jr1_device_start_transaction(struct kaps_jr1_device *device);



kaps_jr1_status kaps_jr1_device_end_transaction(struct kaps_jr1_device *device);


kaps_jr1_status kaps_jr1_device_query_restore_status(struct kaps_jr1_device *device, enum kaps_jr1_restore_status *status);


kaps_jr1_status kaps_jr1_device_clear_restore_status(struct kaps_jr1_device *device);









const char *kaps_jr1_device_get_last_error(struct kaps_jr1_device *device);





#ifdef __cplusplus
}
#endif
#endif 



