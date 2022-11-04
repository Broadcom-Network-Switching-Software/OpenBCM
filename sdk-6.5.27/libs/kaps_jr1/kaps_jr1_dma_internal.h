

#ifndef __KAPS_JR1_DMA_INTERNAL_H
#define __KAPS_JR1_DMA_INTERNAL_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_dma.h"
#include "kaps_jr1_db_internal.h"

#ifdef __cplusplus
extern "C" {
#endif


struct kaps_jr1_dma_db {
    struct kaps_jr1_db db_info;          
    struct kaps_jr1_fast_bitmap *alloc_fbmap;
    uint32_t dma_tag;               
    uint16_t user_width_1;          
};

struct kaps_jr1_dma_db_wb_info {
    uintptr_t stale_ptr;              
    uint32_t tid;                     
    uint32_t capacity;                
    uint16_t desc;                    
    uint16_t user_width_1;            
    uint32_t dma_tag;                 
    uint32_t bb_alloc;                
    char description[4];              
};


kaps_jr1_status kaps_jr1_dma_db_wb_save_info(struct kaps_jr1_db *db,
                                   struct kaps_jr1_wb_cb_functions *wb_fun);


kaps_jr1_status kaps_jr1_dma_db_wb_restore_info(struct kaps_jr1_device *device,
                                      struct kaps_jr1_wb_cb_functions *wb_fun);

#ifdef __cplusplus
}
#endif

#endif


