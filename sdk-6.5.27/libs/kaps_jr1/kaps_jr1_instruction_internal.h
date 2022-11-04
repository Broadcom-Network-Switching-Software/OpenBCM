

#ifndef __KAPS_JR1_INSTRUCTION_INTERNAL_H
#define __KAPS_JR1_INSTRUCTION_INTERNAL_H

#include "kaps_jr1_instruction.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif



    struct kaps_jr1_device;
    struct kaps_jr1_ab_info;



    enum kaps_jr1_entry_status
    {
        PC_OVERFLOW_0 = 0,
                         
        PC_OVERFLOW_1 = 1,
                         
        DECISION_TRIE_0 = 2,
                         
        DECISION_TRIE_1 = 3,
                         
        OVERFLOW = 4,    
        UNHANDLED = 5    
    };



#define KAPS_JR1_INSTLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_jr1_instruction, node)



    enum instruction_type
    {
        INSTR_UNKNOWN,
        INSTR_CBWLPM,       
        INSTR_COMPARE1,     
        INSTR_COMPARE2,     
        INSTR_COMPARE3      
    };


    struct instruction_desc
    {
        struct kaps_jr1_db *db;              
        uint32_t dba_res_map:16;        
        uint8_t sram_dt_num[8];         
        uint32_t kpu_id:3;              
        uint32_t result_id:3;           
        uint32_t user_result_id:3;      
        uint32_t is_dup_required:1;     
        uint32_t num_segments:4;        
        uint32_t num_ad_bytes:6;        
        uint32_t last_segment_width:8;  
        uint32_t last_segment_start_byte:8;
                                        
        uint32_t user_specified:1;      
    };


    struct kaps_jr1_instruction
    {
        struct kaps_jr1_list_node node;      
        struct kaps_jr1_device *device;  
        struct kaps_jr1_key *master_key; 
        uintptr_t stale_ptr;        
        uintptr_t cascaded_inst_stale_ptr;        
        struct instruction_desc desc[KAPS_JR1_INSTRUCTION_MAX_RESULTS];
                                                                
        uint32_t ltr:8;             
        uint32_t num_searches:4;    
        uint32_t type:4;            
        uint32_t is_installed:1;    
        uint32_t is_cmp3_pair:1;    
        uint32_t has_resolution:1;  
        uint32_t is_cmp3:1;         
        uint32_t is_cascading_src:1;
        uint32_t is_cascading_dst:1;
        struct kaps_jr1_instruction *cascaded_inst;
                                            
        uint32_t id;                
        uint32_t lut_rec_length;    
        uint32_t lut_index;         
        uint8_t is_prop_set[KAPS_JR1_INSTRUCTION_MAX_RESULTS];     
        uint8_t is_prop_set_xml_dump[KAPS_JR1_INSTRUCTION_MAX_RESULTS];
                                                                  
        uint32_t prop_num_bytes[KAPS_JR1_INSTRUCTION_MAX_RESULTS];    
        uint32_t prop_byte_offset[KAPS_JR1_INSTRUCTION_MAX_RESULTS];  
        uint8_t dba_dt_bit_map;     
        uint8_t cmp3_result_port_shift;
                                    
        uint32_t conflict_bits;     

                                    
    };



    kaps_jr1_status kaps_jr1_instruction_clocks(
    struct kaps_jr1_device *device,
    enum instruction_type instr,
    uint32_t * clocks);



    kaps_jr1_status kaps_jr1_instruction_name(
    struct kaps_jr1_device *device,
    enum instruction_type instr,
    char **name);


    kaps_jr1_status kaps_jr1_instruction_finalize_12k(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status kaps_jr1_instruction_finalize_op(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status kaps_jr1_instruction_finalize_advanced(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status kaps_jr1_instruction_determine_type(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status kaps_jr1_instruction_install_12k(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status kaps_jr1_instruction_install_op(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status write_op_conflict_bits(
    struct kaps_jr1_device *device,
    struct kaps_jr1_instruction *instr);



    kaps_jr1_status kaps_jr1_instruction_wb_save(
    struct kaps_jr1_instruction *instruction,
    struct kaps_jr1_wb_cb_functions *wb_fun);



    kaps_jr1_status kaps_jr1_instruction_wb_restore(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);



    kaps_jr1_status kaps_jr1_instruction_enable_ab_for_db(
    struct kaps_jr1_db *db,
    struct kaps_jr1_ab_info *ab_info,
    enum kaps_jr1_entry_status type,
    int32_t force_res_port);



    kaps_jr1_status kaps_jr1_instruction_disable_ab_for_db(
    struct kaps_jr1_db *db,
    struct kaps_jr1_ab_info *ab_info,
    enum kaps_jr1_entry_status type,
    int32_t is_blk_empty);


    struct kaps_jr1_range_extraction
    {
        uint16_t encoding;    
        uint16_t extraction_byte;
                              
    };



    kaps_jr1_status kaps_jr1_instruction_op_write_range_extraction(
    struct kaps_jr1_instruction *instruction,
    struct kaps_jr1_range_extraction *ext_info);



    kaps_jr1_status kaps_jr1_instruction_12k_write_range_extraction(
    struct kaps_jr1_instruction *instruction,
    struct kaps_jr1_range_extraction *ext_info);



    kaps_jr1_status kaps_jr1_instruction_configure_range_extraction(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status kaps_jr1_instruction_wb_print(
    FILE * bin_fp,
    FILE * txt_fp,
    uint32_t * nv_offset);



    kaps_jr1_status kaps_jr1_instruction_finalize_kpus(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_instruction_calc_key_segments(
    struct kaps_jr1_instruction *instruction);



    kaps_jr1_status kaps_jr1_instruction_check_interface_width_op(
    struct kaps_jr1_instruction *instruction);



    kaps_jr1_status kaps_jr1_device_inst_calculate_conflict_bits(
    struct kaps_jr1_device *main_device,
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status cascade_instruction(
    struct kaps_jr1_instruction *instr,
    struct kaps_jr1_device *from_device,
    struct kaps_jr1_device *to_device,
    struct kaps_jr1_c_list_iter *it);


    int32_t get_highest_ad_width(
    struct kaps_jr1_instruction *instr,
    int32_t result_id);



#ifdef __cplusplus
}
#endif
#endif  
