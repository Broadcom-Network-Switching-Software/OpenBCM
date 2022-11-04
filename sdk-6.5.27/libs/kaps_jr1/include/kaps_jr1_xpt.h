

#ifndef __KAPS_JR1_XPT_H
#define __KAPS_JR1_XPT_H



#include <stdint.h>
#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"


#ifdef __cplusplus
extern "C" {
#endif


#define KAPS_JR1_DEFAULT_AD_WIDTH_1 (20)
#define KAPS_JR1_DEFAULT_AD_WIDTH_8 (3)

#define KAPS_JR1_MAX_AD_WIDTH_8 (3) 




enum kaps_jr1_cmd
{
    KAPS_JR1_CMD_READ,     
    KAPS_JR1_CMD_WRITE,    
    KAPS_JR1_CMD_EXTENDED, 
    KAPS_JR1_CMD_ACK       
};




enum kaps_jr1_func
{
    KAPS_JR1_FUNC0,       
    KAPS_JR1_FUNC1,       
    KAPS_JR1_FUNC2,       
    KAPS_JR1_FUNC3,       
    KAPS_JR1_FUNC4,       
    KAPS_JR1_FUNC5,       
    KAPS_JR1_FUNC6,       
    KAPS_JR1_FUNC7,       
    KAPS_JR1_FUNC8,       
    KAPS_JR1_FUNC9,       
    KAPS_JR1_FUNC10,      
    KAPS_JR1_FUNC11,      
    KAPS_JR1_FUNC12,      
    KAPS_JR1_FUNC13,      
    KAPS_JR1_FUNC14,      
    KAPS_JR1_FUNC15,      
    KAPS_JR1_FUNC16,      
};



enum kaps_jr1_search_interface
{
    KAPS_JR1_SEARCH_0_A,    
    KAPS_JR1_SEARCH_0_B,    
    KAPS_JR1_SEARCH_1_A,    
    KAPS_JR1_SEARCH_1_B     
};



struct kaps_jr1_search_result
{
    uint32_t match_flag; 
    uint8_t ad_value[KAPS_JR1_MAX_AD_WIDTH_8];  
    uint32_t match_len; 
    uint32_t is_bdata; 
    uint32_t res_ad_width_1; 
};






struct kaps_jr1_xpt
{
    
    uint32_t device_type;

    
    kaps_jr1_status (*kaps_jr1_search) (void *xpt, uint8_t *key, enum kaps_jr1_search_interface search_interface, struct kaps_jr1_search_result *kaps_jr1_result);


    
    kaps_jr1_status (*kaps_jr1_command)(void *xpt, enum kaps_jr1_cmd cmd, enum kaps_jr1_func func, uint32_t blk_nr, uint32_t row_nr, uint32_t nbytes, uint8_t *bytes);

    
    kaps_jr1_status (*kaps_jr1_register_read)(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);

    
    kaps_jr1_status (*kaps_jr1_register_write)(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);


    
    kaps_jr1_status (*kaps_jr1_hb_read)(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes);

    
    kaps_jr1_status (*kaps_jr1_hb_write)(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes);




    
    kaps_jr1_status (*kaps_jr1_hb_dump)(void *xpt, uint32_t start_blk_num, uint32_t start_row_num, uint32_t end_blk_num,
                                uint32_t end_row_num, uint8_t clear_on_read, uint8_t *data);

    
    kaps_jr1_status (*kaps_jr1_hb_copy)(void *xpt, uint32_t src_blk_num, uint32_t src_row_num, uint32_t dest_blk_num,
                                uint32_t dest_row_num, uint16_t source_mask, uint8_t rotate_right, uint8_t perform_clear);




};




#ifdef __cplusplus
}
#endif


#endif 

