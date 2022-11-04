

#ifndef __KAPS_JR1_KEY_INTERNAL_H
#define __KAPS_JR1_KEY_INTERNAL_H

#include "kaps_jr1_key.h"
#include "kaps_jr1_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif





#define KEY_TERNARY_SIZE_RATIO (0.6)

    struct range_info;
    struct kaps_jr1_db;
    struct kaps_jr1_device;
    struct kaps_jr1_instruction;


    struct kaps_jr1_key_field
    {
        uint16_t offset_1;        
        uint16_t width_1;         
        uint16_t orig_offset_1;   
        uint16_t field_prio;      
        uint32_t do_not_bmr:1;    
        uint32_t visited:1;       
        uint32_t is_usr_bmr:1;    
        uint32_t is_padding_field:1;
                                  
        struct range_info *rinfo; 
        struct kaps_jr1_key_field *p_field;
                                    
        enum kaps_jr1_key_field_type type;
                                   
        struct kaps_jr1_key_field *next;
                                   
        struct kaps_jr1_key_field *prev;
                                   
        char *name;               
    };



    struct kaps_jr1_key
    {
        uint32_t width_1:16;                
        uint32_t nranges:3;                 
        uint32_t has_user_bmr:1;            
        uint32_t ref_count:8;               
        uint32_t has_dummy_fill_field:1;    
        uint16_t critical_field_count;      
        struct kaps_jr1_device *device;          
        struct kaps_jr1_key_field *first_field;  
        struct kaps_jr1_key_field *last_field;   
        struct kaps_jr1_key_field *first_overlay_field;
                                                
        struct kaps_jr1_key_field *last_overlay_field;
                                                
    };



    struct kaps_jr1_print_key
    {
        char *fname;                    
        uint32_t width;                 
        enum kaps_jr1_key_field_type type;   
        struct kaps_jr1_print_key *next;     
    };



    void kaps_jr1_key_destroy_internal(
    struct kaps_jr1_key *key);



    char *kaps_jr1_key_get_type_internal(
    enum kaps_jr1_key_field_type type);



    void kaps_jr1_key_move_internal(
    struct kaps_jr1_key *key,
    struct kaps_jr1_key_field *field,
    uint32_t offset_1);



    void kaps_jr1_key_adjust_offsets(
    struct kaps_jr1_key *key);


    kaps_jr1_status kaps_jr1_key_configure_key_construction(
    struct kaps_jr1_instruction *instruction);



    kaps_jr1_status kaps_jr1_key_clone(
    struct kaps_jr1_device *device,
    struct kaps_jr1_key *key,
    struct kaps_jr1_key **result);



    kaps_jr1_status kaps_jr1_key_validate_internal(
    struct kaps_jr1_db *db,
    struct kaps_jr1_key *key);


    kaps_jr1_status kaps_jr1_key_match_clone_parent(
    struct kaps_jr1_db *clone,
    struct kaps_jr1_db *parent);


    kaps_jr1_status kaps_jr1_key_wb_save(
    struct kaps_jr1_key *key,
    struct kaps_jr1_wb_cb_functions *wb_fun);


    kaps_jr1_status kaps_jr1_key_wb_restore(
    struct kaps_jr1_key *key,
    struct kaps_jr1_wb_cb_functions *wb_fun);




    void kaps_jr1_key_wb_print(
    struct kaps_jr1_print_key *key,
    FILE * txt_fp);


    kaps_jr1_status kaps_jr1_key_print_internal(
    struct kaps_jr1_key *key,
    FILE * fp);


    void kaps_jr1_key_wb_free(
    struct kaps_jr1_print_key *key);


    kaps_jr1_status kaps_jr1_key_zero_kpu_key(
    struct kaps_jr1_device *device,
    uint32_t ltr_num,
    uint32_t kpu_no);



    kaps_jr1_status kaps_jr1_key_add_field_internal(
    struct kaps_jr1_key *key,
    char *name,
    uint32_t width_1,
    enum kaps_jr1_key_field_type type,
    uint32_t is_user_bmr);



    kaps_jr1_status kaps_jr1_key_get_number_of_segments(
    struct kaps_jr1_instruction *instruction,
    struct kaps_jr1_db *db,
    uint8_t * nsegments,
    uint8_t * last_segment_width,
    uint8_t * last_segment_start_byte);



    kaps_jr1_status kaps_jr1_key_configure_range_insertion(
    struct kaps_jr1_db *db);


#ifdef __cplusplus
}
#endif

#endif 
