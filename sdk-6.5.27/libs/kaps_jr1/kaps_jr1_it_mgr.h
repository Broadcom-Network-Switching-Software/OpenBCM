

#ifndef __KAPS_JR1_IT_MGR_H
#define __KAPS_JR1_IT_MGR_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_allocator.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_ab.h"
#include "kaps_jr1_instruction_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AD_IN_PLACE_IN_UIT (0x1000000)



    struct it_bank_info
    {
        uint8_t type;       
        uint8_t instance_id;
        uint8_t bank_no;    
        uint8_t nab;        
        uint8_t is_used_bmp;
        uint8_t ab_conf;     
        uint32_t ab_width_8;
        struct kaps_jr1_db *db;  
    };


    struct it_mgr_stats
    {
        uint32_t num_used_uit_banks;
    };

    enum it_instance_id
    {
        IT_INSTANCE_0,
        IT_INSTANCE_1
    };


    struct kaps_jr1_it_mgr
    {
        struct it_bank_info bank[KAPS_JR1_MAX_NUM_ADS_BLOCKS];
                                                        
    };


    struct it_mgr
    {
        struct kaps_jr1_allocator *alloc;
        struct kaps_jr1_device *device;  

        union
        {
            struct kaps_jr1_it_mgr kaps_jr1;
        } u;
    };


    kaps_jr1_status kaps_jr1_it_mgr_init(
    struct kaps_jr1_device *device,
    struct kaps_jr1_allocator *alloc,
    struct it_mgr **mgr_pp);


    kaps_jr1_status kaps_jr1_it_mgr_alloc(
    struct it_mgr *self,
    struct kaps_jr1_db *db,
    enum kaps_jr1_entry_status type,
    enum it_instance_id instance_id,
    uint32_t ab_width_in_bytes,
    struct kaps_jr1_ab_info *ab);


    kaps_jr1_status kaps_jr1_it_mgr_free(
    struct it_mgr *self,
    struct kaps_jr1_db *db,
    struct kaps_jr1_ab_info *ab,
    uint32_t ab_width_in_bytes,
    enum kaps_jr1_entry_status type);



    kaps_jr1_status kaps_jr1_it_mgr_destroy(
    struct it_mgr *self);


    kaps_jr1_status kaps_jr1_it_mgr_wb_alloc(
    struct it_mgr *self,
    struct kaps_jr1_db *db,
    enum kaps_jr1_entry_status type,
    uint32_t ab_width_in_bytes,
    struct kaps_jr1_ab_info *ab);

#ifdef __cplusplus
}
#endif

#endif 
