

#ifndef __KAPS_JR1_INSTRUCTION_H
#define __KAPS_JR1_INSTRUCTION_H



#include <stdint.h>

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"

#ifdef __cplusplus
extern "C" {
#endif




#define KAPS_JR1_INSTRUCTION_MAX_RESULTS    (8)


#define KAPS_JR1_INSTRUCTION_MAX_AD_BYTES    (32)



struct kaps_jr1_instruction;
struct kaps_jr1_db;
struct kaps_jr1_key;



enum kaps_jr1_result_valid
{
    KAPS_JR1_RESULT_IS_INVALID,    
    KAPS_JR1_RESULT_IS_VALID       
};



enum kaps_jr1_hit_or_miss
{
    KAPS_JR1_MISS,               
    KAPS_JR1_HIT                 
};



enum kaps_jr1_instruction_properties
{
    KAPS_JR1_INST_PROP_RESULT_DATA_LEN, 
    KAPS_JR1_INST_PROP_INVALID
};





struct kaps_jr1_complete_search_result
{
    enum kaps_jr1_result_valid result_valid[KAPS_JR1_INSTRUCTION_MAX_RESULTS];   
    enum kaps_jr1_hit_or_miss  hit_or_miss[KAPS_JR1_INSTRUCTION_MAX_RESULTS];    
    uint32_t result_type[KAPS_JR1_INSTRUCTION_MAX_RESULTS];  
    uint8_t hit_dev_id[KAPS_JR1_INSTRUCTION_MAX_RESULTS];  
    uint32_t hit_index[KAPS_JR1_INSTRUCTION_MAX_RESULTS];  
    
    uint8_t assoc_data[KAPS_JR1_INSTRUCTION_MAX_RESULTS][KAPS_JR1_INSTRUCTION_MAX_AD_BYTES];
};



kaps_jr1_status kaps_jr1_instruction_init(struct kaps_jr1_device *device, uint32_t id, uint32_t ltr,
                                struct kaps_jr1_instruction **instruction);



kaps_jr1_status kaps_jr1_instruction_destroy(struct kaps_jr1_instruction *instruction);



kaps_jr1_status kaps_jr1_instruction_add_db(struct kaps_jr1_instruction *instruction, struct kaps_jr1_db *db, uint32_t result);



kaps_jr1_status kaps_jr1_instruction_set_key(struct kaps_jr1_instruction *instruction, struct kaps_jr1_key *master_key);



kaps_jr1_status kaps_jr1_instruction_install(struct kaps_jr1_instruction *instruction);



kaps_jr1_status kaps_jr1_instruction_print(struct kaps_jr1_instruction *instruction, FILE *fp);



kaps_jr1_status kaps_jr1_instruction_set_property(struct kaps_jr1_instruction *inst, enum kaps_jr1_instruction_properties property, ...);



kaps_jr1_status kaps_jr1_instruction_search(struct kaps_jr1_instruction *instruction, uint8_t *master_key, uint32_t cb_addrs, struct kaps_jr1_complete_search_result *result);







kaps_jr1_status kaps_jr1_instruction_refresh_handle(struct kaps_jr1_device *device, struct kaps_jr1_instruction *stale_ptr, struct kaps_jr1_instruction **instruction);



kaps_jr1_status kaps_jr1_instruction_master_key_from_db_key(struct kaps_jr1_instruction *instruction, struct kaps_jr1_db * db, uint8_t *db_key, uint8_t *search_key);


kaps_jr1_status kaps_jr1_instruction_set_opcode(struct kaps_jr1_instruction *instruction, int32_t opcode);




#ifdef __cplusplus
}
#endif
#endif 
