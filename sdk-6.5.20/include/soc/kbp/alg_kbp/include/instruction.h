/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __INSTRUCTION_H
#define __INSTRUCTION_H



#include <stdint.h>

#include "errors.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif




#define KBP_INSTRUCTION_MAX_RESULTS    (8)


#define KBP_INSTRUCTION_MAX_AD_BYTES    (32)



struct kbp_instruction;
struct kbp_db;
struct kbp_key;



enum kbp_result_valid
{
    KBP_RESULT_IS_INVALID,    
    KBP_RESULT_IS_VALID       
};



enum kbp_hit_or_miss
{
    KBP_MISS,               
    KBP_HIT                 
};



enum kbp_instruction_properties
{
    KBP_INST_PROP_RESULT_DATA_LEN, 
    KBP_INST_PROP_INVALID
};




enum kbp_search_resp_type
{
    
    KBP_INDEX_AND_NO_AD = 0,    

    
    KBP_INDEX_AND_32B_AD  = 4,   
    KBP_INDEX_AND_64B_AD  = 8,   
    KBP_INDEX_AND_96B_AD  = 12,  
    KBP_INDEX_AND_128B_AD = 16,  
    KBP_INDEX_AND_160B_AD = 20,  
    KBP_INDEX_AND_192B_AD = 24,  
    KBP_INDEX_AND_256B_AD = 31,  

    
    KBP_ONLY_AD_24B  =  3 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_32B  =  4 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_64B  =  8 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_96B  = 12 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_128B = 16 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_160B = 20 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_192B = 24 + KBP_INDEX_AND_256B_AD, 
    KBP_ONLY_AD_256B = 31 + KBP_INDEX_AND_256B_AD  
};



struct kbp_search_result
{
    enum kbp_result_valid result_valid[KBP_INSTRUCTION_MAX_RESULTS];   
    enum kbp_hit_or_miss  hit_or_miss[KBP_INSTRUCTION_MAX_RESULTS];    
    enum kbp_search_resp_type resp_type[KBP_INSTRUCTION_MAX_RESULTS];  
    uint8_t hit_dev_id[KBP_INSTRUCTION_MAX_RESULTS];  
    uint32_t hit_index[KBP_INSTRUCTION_MAX_RESULTS];  
    
    uint8_t assoc_data[KBP_INSTRUCTION_MAX_RESULTS][KBP_INSTRUCTION_MAX_AD_BYTES];
};



kbp_status kbp_instruction_init(struct kbp_device *device, uint32_t id, uint32_t ltr,
                                struct kbp_instruction **instruction);



kbp_status kbp_instruction_destroy(struct kbp_instruction *instruction);



kbp_status kbp_instruction_add_db(struct kbp_instruction *instruction, struct kbp_db *db, uint32_t result);



kbp_status kbp_instruction_set_key(struct kbp_instruction *instruction, struct kbp_key *master_key);



kbp_status kbp_instruction_install(struct kbp_instruction *instruction);



kbp_status kbp_instruction_print(struct kbp_instruction *instruction, FILE *fp);



kbp_status kbp_instruction_set_property(struct kbp_instruction *inst, enum kbp_instruction_properties property, ...);



kbp_status kbp_instruction_search(struct kbp_instruction *instruction, uint8_t *master_key, uint32_t cb_addrs, struct kbp_search_result *result);







kbp_status kbp_instruction_refresh_handle(struct kbp_device *device, struct kbp_instruction *stale_ptr, struct kbp_instruction **instruction);



kbp_status kbp_instruction_master_key_from_db_key(struct kbp_instruction *instruction, struct kbp_db * db, uint8_t *db_key, uint8_t *search_key);


kbp_status kbp_instruction_set_opcode(struct kbp_instruction *instruction, int32_t opcode);




#ifdef __cplusplus
}
#endif
#endif 
