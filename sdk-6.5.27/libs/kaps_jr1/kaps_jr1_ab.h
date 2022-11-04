

#ifndef __KAPS_JR1_AB_H
#define __KAPS_JR1_AB_H

#include "kaps_jr1_device_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif







    enum kaps_jr1_ab_confs
    {
        KAPS_JR1_NORMAL_80 = 0,
                          
        KAPS_JR1_NORMAL_160 = 1
                         
    };



    struct kaps_jr1_ab_info
    {

        struct kaps_jr1_list_node ab_node;
                                    
        uint32_t ab_num;     
        uint32_t num_slots; 
        uint32_t conf:4;       
        uint32_t blk_cleared:1;
        uint32_t translate:1;  
        uint32_t base_addr;    
        uint32_t base_idx;     
        uint32_t ab_init_done:1; 
        uint32_t is_dup_ab:1;  
        uint32_t is_mapped_ab:1;
                               

        struct kaps_jr1_db *db;     
        struct kaps_jr1_ab_info *dup_ab;
                                 
        struct kaps_jr1_ab_info *parent;

        struct kaps_jr1_device *device;
                                

    };

#define KAPS_JR1_ABLIST_TO_ABINFO(ptr) CONTAINER_OF((ptr), struct kaps_jr1_ab_info, ab_node)





#ifdef __cplusplus
}
#endif
#endif
