

#ifndef __KAPS_XPT_H
#define __KAPS_XPT_H



#include <stdint.h>
#include "kaps_errors.h"


#ifdef __cplusplus
extern "C" {
#endif

#define KAPS_AD_WIDTH_8 (3)



enum kaps_cmd
{
    KAPS_CMD_READ,     
    KAPS_CMD_WRITE,    
    KAPS_CMD_EXTENDED, 
    KAPS_CMD_ACK       
};




enum kaps_func
{
    KAPS_FUNC0,       
    KAPS_FUNC1,       
    KAPS_FUNC2,       
    KAPS_FUNC3,       
    KAPS_FUNC4,       
    KAPS_FUNC5,       
    KAPS_FUNC6,       
    KAPS_FUNC7,       
    KAPS_FUNC8,       
    KAPS_FUNC9,       
    KAPS_FUNC10,      
    KAPS_FUNC11,      
    KAPS_FUNC12,      
    KAPS_FUNC13,      
    KAPS_FUNC14,      
    KAPS_FUNC15,      
    KAPS_FUNC16       
};



enum kaps_search_interface
{
    KAPS_SEARCH_0_A,    
    KAPS_SEARCH_0_B,    
    KAPS_SEARCH_1_A,    
    KAPS_SEARCH_1_B     
};



struct kaps_search_result
{
    uint8_t ad_value[KAPS_AD_WIDTH_8];  
    uint32_t match_len; 
};









struct kaps_xpt
{
    
    uint32_t device_type;


    
    kaps_status (*kaps_search) (void *xpt, uint8_t *key, enum kaps_search_interface search_interface, struct kaps_search_result *kaps_result);


    
    kaps_status (*kaps_command)(void *xpt, enum kaps_cmd cmd, enum kaps_func func, uint32_t blk_nr, uint32_t row_nr, uint32_t nbytes, uint8_t *bytes);

    
    kaps_status (*kaps_register_read)(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);

    
    kaps_status (*kaps_register_write)(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);


    
    kaps_status (*kaps_hb_read)(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes);

    
    kaps_status (*kaps_hb_write)(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes);




    
    kaps_status (*kaps_hb_dump)(void *xpt, uint32_t start_blk_num, uint32_t start_row_num, uint32_t end_blk_num,
                                uint32_t end_row_num, uint8_t clear_on_read, uint8_t *data);

    
    kaps_status (*kaps_hb_copy)(void *xpt, uint32_t src_blk_num, uint32_t src_row_num, uint32_t dest_blk_num,
                                uint32_t dest_row_num, uint16_t source_mask, uint8_t rotate_right, uint8_t perform_clear);


};




#ifdef __cplusplus
}
#endif


#endif 

