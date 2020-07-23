
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_TYPES_H__
#define __KBP_TYPES_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/kbp_hb.h>
#include <soc/kbp/alg_kbp/include/key.h>



#define DNX_KBP_NOF_DBS 256


#define DNX_KBP_NOF_INSTRUCTIONS 128


#define DNX_KBP_USE_OPTIMIZED_RESULT dnx_data_elk.application.optimized_result_get(unit)


#define DNX_KBP_NOF_SMTS 2



typedef struct kbp_db * kbp_db_t_p;

typedef struct kbp_ad_db * kbp_ad_db_t_p;

typedef struct kbp_hb_db * kbp_hb_db_t_p;

typedef struct kbp_instruction * kbp_instruction_t_p;

typedef struct kbp_key * kbp_key_t_p;

typedef struct kbp_ad * kbp_ad_entry_t_p;




typedef enum {
    
    DNX_KBP_AD_DB_INDEX_REGULAR = 0,
    
    DNX_KBP_AD_DB_INDEX_OPTIMIZED = 1,
    DNX_KBP_NOF_AD_DB_INDEX = 2
} dnx_kbp_ad_db_index_e;




typedef struct {
    kbp_instruction_t_p inst_p[DNX_KBP_NOF_SMTS];
} kbp_instruction_handles_t;


typedef struct {
    int caching_bmp;
    kbp_db_t_p db_p;
    kbp_ad_db_t_p ad_db_zero_size_p;
    kbp_ad_entry_t_p ad_entry_zero_size_p;
    kbp_ad_db_t_p ad_db_p[DNX_KBP_NOF_AD_DB_INDEX];
    int opt_result_size;
    int cloned_db_id;
    int associated_dbal_table_id;
} kbp_db_handles_t;


typedef struct {
    int is_device_locked;
    kbp_db_handles_t db_handles_info[DNX_KBP_NOF_DBS];
    kbp_instruction_handles_t instruction_info[DNX_KBP_NOF_INSTRUCTIONS];
} kbp_sw_state_t;

#endif  
#endif  

#endif 
