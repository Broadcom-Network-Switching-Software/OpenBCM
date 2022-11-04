

#ifndef __KAPS_JR1_MODEL_ADS_H
#define __KAPS_JR1_MODEL_ADS_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_device_alg.h"




#ifdef __cplusplus
extern "C" {
#endif

struct kaps_jr1_c_model;





struct kaps_jr1_c_model_ads_block
{
    struct kaps_jr1_c_model *m;   
    uint32_t ads_id; 
    struct kaps_jr1_ads rows[KAPS_JR1_ADS_MAX_NUM_ROWS];    
};


kaps_jr1_status kaps_jr1_c_model_ads_create(struct kaps_jr1_c_model *m, uint32_t ads_id, struct kaps_jr1_c_model_ads_block **ads_blk_pp);


kaps_jr1_status kaps_jr1_c_model_ads_destroy(struct kaps_jr1_c_model_ads_block * ads_blk);


kaps_jr1_status kaps_jr1_c_model_ads_write(struct kaps_jr1_c_model_ads_block * ads_blk, uint32_t row_nr, struct kaps_jr1_ads *ads_data);


kaps_jr1_status kaps_jr1_c_model_ads_read(struct kaps_jr1_c_model_ads_block * ads_blk, uint32_t row_nr, struct kaps_jr1_ads *ads_data);


#ifdef __cplusplus
}
#endif


#endif


