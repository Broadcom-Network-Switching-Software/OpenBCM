
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_ACCESS_H__
#define __KBP_ACCESS_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/kbp_hb.h>
#include <soc/kbp/alg_kbp/include/key.h>



typedef int (*kbp_sw_state_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*kbp_sw_state_init_cb)(
    int unit);


typedef int (*kbp_sw_state_is_device_locked_set_cb)(
    int unit, int is_device_locked);


typedef int (*kbp_sw_state_is_device_locked_get_cb)(
    int unit, int *is_device_locked);


typedef int (*kbp_sw_state_db_handles_info_set_cb)(
    int unit, uint32 db_handles_info_idx_0, CONST kbp_db_handles_t *db_handles_info);


typedef int (*kbp_sw_state_db_handles_info_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_db_handles_t *db_handles_info);


typedef int (*kbp_sw_state_db_handles_info_caching_bmp_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int caching_bmp);


typedef int (*kbp_sw_state_db_handles_info_caching_bmp_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *caching_bmp);


typedef int (*kbp_sw_state_db_handles_info_db_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_db_t_p db_p);


typedef int (*kbp_sw_state_db_handles_info_db_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_db_t_p *db_p);


typedef int (*kbp_sw_state_db_handles_info_ad_db_zero_size_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p ad_db_zero_size_p);


typedef int (*kbp_sw_state_db_handles_info_ad_db_zero_size_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p *ad_db_zero_size_p);


typedef int (*kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p ad_entry_zero_size_p);


typedef int (*kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p *ad_entry_zero_size_p);


typedef int (*kbp_sw_state_db_handles_info_ad_db_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p ad_db_p);


typedef int (*kbp_sw_state_db_handles_info_ad_db_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p *ad_db_p);


typedef int (*kbp_sw_state_db_handles_info_opt_result_size_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int opt_result_size);


typedef int (*kbp_sw_state_db_handles_info_opt_result_size_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *opt_result_size);


typedef int (*kbp_sw_state_db_handles_info_cloned_db_id_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int cloned_db_id);


typedef int (*kbp_sw_state_db_handles_info_cloned_db_id_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *cloned_db_id);


typedef int (*kbp_sw_state_db_handles_info_associated_dbal_table_id_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int associated_dbal_table_id);


typedef int (*kbp_sw_state_db_handles_info_associated_dbal_table_id_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *associated_dbal_table_id);


typedef int (*kbp_sw_state_instruction_info_set_cb)(
    int unit, uint32 instruction_info_idx_0, CONST kbp_instruction_handles_t *instruction_info);


typedef int (*kbp_sw_state_instruction_info_get_cb)(
    int unit, uint32 instruction_info_idx_0, kbp_instruction_handles_t *instruction_info);


typedef int (*kbp_sw_state_instruction_info_inst_p_set_cb)(
    int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p inst_p);


typedef int (*kbp_sw_state_instruction_info_inst_p_get_cb)(
    int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p *inst_p);




typedef struct {
    kbp_sw_state_is_device_locked_set_cb set;
    kbp_sw_state_is_device_locked_get_cb get;
} kbp_sw_state_is_device_locked_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_caching_bmp_set_cb set;
    kbp_sw_state_db_handles_info_caching_bmp_get_cb get;
} kbp_sw_state_db_handles_info_caching_bmp_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_db_p_set_cb set;
    kbp_sw_state_db_handles_info_db_p_get_cb get;
} kbp_sw_state_db_handles_info_db_p_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_ad_db_zero_size_p_set_cb set;
    kbp_sw_state_db_handles_info_ad_db_zero_size_p_get_cb get;
} kbp_sw_state_db_handles_info_ad_db_zero_size_p_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set_cb set;
    kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get_cb get;
} kbp_sw_state_db_handles_info_ad_entry_zero_size_p_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_ad_db_p_set_cb set;
    kbp_sw_state_db_handles_info_ad_db_p_get_cb get;
} kbp_sw_state_db_handles_info_ad_db_p_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_opt_result_size_set_cb set;
    kbp_sw_state_db_handles_info_opt_result_size_get_cb get;
} kbp_sw_state_db_handles_info_opt_result_size_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_cloned_db_id_set_cb set;
    kbp_sw_state_db_handles_info_cloned_db_id_get_cb get;
} kbp_sw_state_db_handles_info_cloned_db_id_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_associated_dbal_table_id_set_cb set;
    kbp_sw_state_db_handles_info_associated_dbal_table_id_get_cb get;
} kbp_sw_state_db_handles_info_associated_dbal_table_id_cbs;


typedef struct {
    kbp_sw_state_db_handles_info_set_cb set;
    kbp_sw_state_db_handles_info_get_cb get;
    
    kbp_sw_state_db_handles_info_caching_bmp_cbs caching_bmp;
    
    kbp_sw_state_db_handles_info_db_p_cbs db_p;
    
    kbp_sw_state_db_handles_info_ad_db_zero_size_p_cbs ad_db_zero_size_p;
    
    kbp_sw_state_db_handles_info_ad_entry_zero_size_p_cbs ad_entry_zero_size_p;
    
    kbp_sw_state_db_handles_info_ad_db_p_cbs ad_db_p;
    
    kbp_sw_state_db_handles_info_opt_result_size_cbs opt_result_size;
    
    kbp_sw_state_db_handles_info_cloned_db_id_cbs cloned_db_id;
    
    kbp_sw_state_db_handles_info_associated_dbal_table_id_cbs associated_dbal_table_id;
} kbp_sw_state_db_handles_info_cbs;


typedef struct {
    kbp_sw_state_instruction_info_inst_p_set_cb set;
    kbp_sw_state_instruction_info_inst_p_get_cb get;
} kbp_sw_state_instruction_info_inst_p_cbs;


typedef struct {
    kbp_sw_state_instruction_info_set_cb set;
    kbp_sw_state_instruction_info_get_cb get;
    
    kbp_sw_state_instruction_info_inst_p_cbs inst_p;
} kbp_sw_state_instruction_info_cbs;


typedef struct {
    kbp_sw_state_is_init_cb is_init;
    kbp_sw_state_init_cb init;
    
    kbp_sw_state_is_device_locked_cbs is_device_locked;
    
    kbp_sw_state_db_handles_info_cbs db_handles_info;
    
    kbp_sw_state_instruction_info_cbs instruction_info;
} kbp_sw_state_cbs;






const char *
dnx_kbp_ad_db_index_e_get_name(dnx_kbp_ad_db_index_e value);




extern kbp_sw_state_cbs kbp_sw_state;
#endif  
#endif  

#endif 
