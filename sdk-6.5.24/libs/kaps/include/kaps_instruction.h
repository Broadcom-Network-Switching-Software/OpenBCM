

#ifndef __KAPS_INSTRUCTION_H
#define __KAPS_INSTRUCTION_H



#include <stdint.h>

#include "kaps_errors.h"
#include "kaps_device.h"

#ifdef __cplusplus
extern "C" {
#endif




#define KAPS_INSTRUCTION_MAX_RESULTS    (8)


#define KAPS_INSTRUCTION_MAX_AD_BYTES    (32)



struct kaps_instruction;
struct kaps_db;
struct kaps_key;



enum kaps_result_valid
{
    KAPS_RESULT_IS_INVALID,    
    KAPS_RESULT_IS_VALID       
};



enum kaps_hit_or_miss
{
    KAPS_MISS,               
    KAPS_HIT                 
};



enum kaps_instruction_properties
{
    KAPS_INST_PROP_RESULT_DATA_LEN, 
    KAPS_INST_PROP_INVALID
};




enum kaps_search_resp_type
{
    
    KAPS_INDEX_AND_NO_AD = 0,    

    
    KAPS_INDEX_AND_32B_AD  = 4,   
    KAPS_INDEX_AND_64B_AD  = 8,   
    KAPS_INDEX_AND_96B_AD  = 12,  
    KAPS_INDEX_AND_128B_AD = 16,  
    KAPS_INDEX_AND_160B_AD = 20,  
    KAPS_INDEX_AND_192B_AD = 24,  
    KAPS_INDEX_AND_256B_AD = 31,  

    
    KAPS_ONLY_AD_24B  =  3 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_32B  =  4 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_64B  =  8 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_96B  = 12 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_128B = 16 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_160B = 20 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_192B = 24 + KAPS_INDEX_AND_256B_AD, 
    KAPS_ONLY_AD_256B = 31 + KAPS_INDEX_AND_256B_AD  
};



struct kaps_complete_search_result
{
    enum kaps_result_valid result_valid[KAPS_INSTRUCTION_MAX_RESULTS];   
    enum kaps_hit_or_miss  hit_or_miss[KAPS_INSTRUCTION_MAX_RESULTS];    
    enum kaps_search_resp_type resp_type[KAPS_INSTRUCTION_MAX_RESULTS];  
    uint8_t hit_dev_id[KAPS_INSTRUCTION_MAX_RESULTS];  
    uint32_t hit_index[KAPS_INSTRUCTION_MAX_RESULTS];  
    
    uint8_t assoc_data[KAPS_INSTRUCTION_MAX_RESULTS][KAPS_INSTRUCTION_MAX_AD_BYTES];
};



kaps_status kaps_instruction_init(struct kaps_device *device, uint32_t id, uint32_t ltr,
                                struct kaps_instruction **instruction);



kaps_status kaps_instruction_destroy(struct kaps_instruction *instruction);



kaps_status kaps_instruction_add_db(struct kaps_instruction *instruction, struct kaps_db *db, uint32_t result);



kaps_status kaps_instruction_set_key(struct kaps_instruction *instruction, struct kaps_key *master_key);



kaps_status kaps_instruction_install(struct kaps_instruction *instruction);



kaps_status kaps_instruction_print(struct kaps_instruction *instruction, FILE *fp);



kaps_status kaps_instruction_set_property(struct kaps_instruction *inst, enum kaps_instruction_properties property, ...);



kaps_status kaps_instruction_search(struct kaps_instruction *instruction, uint8_t *master_key, uint32_t cb_addrs, struct kaps_complete_search_result *result);







kaps_status kaps_instruction_refresh_handle(struct kaps_device *device, struct kaps_instruction *stale_ptr, struct kaps_instruction **instruction);



kaps_status kaps_instruction_master_key_from_db_key(struct kaps_instruction *instruction, struct kaps_db * db, uint8_t *db_key, uint8_t *search_key);


kaps_status kaps_instruction_set_opcode(struct kaps_instruction *instruction, int32_t opcode);




#ifdef __cplusplus
}
#endif
#endif 
