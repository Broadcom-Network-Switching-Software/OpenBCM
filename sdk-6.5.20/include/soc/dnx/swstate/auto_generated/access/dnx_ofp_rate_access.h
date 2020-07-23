
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_OFP_RATE_ACCESS_H__
#define __DNX_OFP_RATE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ofp_rate_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>



typedef int (*dnx_ofp_rate_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_ofp_rate_db_init_cb)(
    int unit);


typedef int (*dnx_ofp_rate_db_otm_alloc_cb)(
    int unit, uint32 otm_idx_0);


typedef int (*dnx_ofp_rate_db_otm_cal_instance_set_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 cal_instance);


typedef int (*dnx_ofp_rate_db_otm_cal_instance_get_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *cal_instance);


typedef int (*dnx_ofp_rate_db_otm_shaping_rate_set_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 rate);


typedef int (*dnx_ofp_rate_db_otm_shaping_rate_get_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *rate);


typedef int (*dnx_ofp_rate_db_otm_shaping_burst_set_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 burst);


typedef int (*dnx_ofp_rate_db_otm_shaping_burst_get_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *burst);


typedef int (*dnx_ofp_rate_db_otm_shaping_valid_set_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 valid);


typedef int (*dnx_ofp_rate_db_otm_shaping_valid_get_cb)(
    int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *valid);


typedef int (*dnx_ofp_rate_db_qpair_rate_set_cb)(
    int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 rate);


typedef int (*dnx_ofp_rate_db_qpair_rate_get_cb)(
    int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 *rate);


typedef int (*dnx_ofp_rate_db_qpair_valid_set_cb)(
    int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 valid);


typedef int (*dnx_ofp_rate_db_qpair_valid_get_cb)(
    int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 *valid);


typedef int (*dnx_ofp_rate_db_tcg_rate_set_cb)(
    int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 rate);


typedef int (*dnx_ofp_rate_db_tcg_rate_get_cb)(
    int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 *rate);


typedef int (*dnx_ofp_rate_db_tcg_valid_set_cb)(
    int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 valid);


typedef int (*dnx_ofp_rate_db_tcg_valid_get_cb)(
    int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 *valid);


typedef int (*dnx_ofp_rate_db_otm_cal_rate_set_cb)(
    int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 rate);


typedef int (*dnx_ofp_rate_db_otm_cal_rate_get_cb)(
    int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 *rate);


typedef int (*dnx_ofp_rate_db_otm_cal_modified_set_cb)(
    int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 modified);


typedef int (*dnx_ofp_rate_db_otm_cal_modified_get_cb)(
    int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 *modified);


typedef int (*dnx_ofp_rate_db_otm_cal_nof_calcal_instances_set_cb)(
    int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 nof_calcal_instances);


typedef int (*dnx_ofp_rate_db_otm_cal_nof_calcal_instances_get_cb)(
    int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 *nof_calcal_instances);


typedef int (*dnx_ofp_rate_db_calcal_is_modified_set_cb)(
    int unit, uint32 calcal_is_modified_idx_0, uint32 calcal_is_modified);


typedef int (*dnx_ofp_rate_db_calcal_is_modified_get_cb)(
    int unit, uint32 calcal_is_modified_idx_0, uint32 *calcal_is_modified);


typedef int (*dnx_ofp_rate_db_calcal_len_set_cb)(
    int unit, uint32 calcal_len_idx_0, uint32 calcal_len);


typedef int (*dnx_ofp_rate_db_calcal_len_get_cb)(
    int unit, uint32 calcal_len_idx_0, uint32 *calcal_len);




typedef struct {
    dnx_ofp_rate_db_otm_cal_instance_set_cb set;
    dnx_ofp_rate_db_otm_cal_instance_get_cb get;
} dnx_ofp_rate_db_otm_cal_instance_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_shaping_rate_set_cb set;
    dnx_ofp_rate_db_otm_shaping_rate_get_cb get;
} dnx_ofp_rate_db_otm_shaping_rate_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_shaping_burst_set_cb set;
    dnx_ofp_rate_db_otm_shaping_burst_get_cb get;
} dnx_ofp_rate_db_otm_shaping_burst_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_shaping_valid_set_cb set;
    dnx_ofp_rate_db_otm_shaping_valid_get_cb get;
} dnx_ofp_rate_db_otm_shaping_valid_cbs;


typedef struct {
    
    dnx_ofp_rate_db_otm_shaping_rate_cbs rate;
    
    dnx_ofp_rate_db_otm_shaping_burst_cbs burst;
    
    dnx_ofp_rate_db_otm_shaping_valid_cbs valid;
} dnx_ofp_rate_db_otm_shaping_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_alloc_cb alloc;
    
    dnx_ofp_rate_db_otm_cal_instance_cbs cal_instance;
    
    dnx_ofp_rate_db_otm_shaping_cbs shaping;
} dnx_ofp_rate_db_otm_cbs;


typedef struct {
    dnx_ofp_rate_db_qpair_rate_set_cb set;
    dnx_ofp_rate_db_qpair_rate_get_cb get;
} dnx_ofp_rate_db_qpair_rate_cbs;


typedef struct {
    dnx_ofp_rate_db_qpair_valid_set_cb set;
    dnx_ofp_rate_db_qpair_valid_get_cb get;
} dnx_ofp_rate_db_qpair_valid_cbs;


typedef struct {
    
    dnx_ofp_rate_db_qpair_rate_cbs rate;
    
    dnx_ofp_rate_db_qpair_valid_cbs valid;
} dnx_ofp_rate_db_qpair_cbs;


typedef struct {
    dnx_ofp_rate_db_tcg_rate_set_cb set;
    dnx_ofp_rate_db_tcg_rate_get_cb get;
} dnx_ofp_rate_db_tcg_rate_cbs;


typedef struct {
    dnx_ofp_rate_db_tcg_valid_set_cb set;
    dnx_ofp_rate_db_tcg_valid_get_cb get;
} dnx_ofp_rate_db_tcg_valid_cbs;


typedef struct {
    
    dnx_ofp_rate_db_tcg_rate_cbs rate;
    
    dnx_ofp_rate_db_tcg_valid_cbs valid;
} dnx_ofp_rate_db_tcg_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_cal_rate_set_cb set;
    dnx_ofp_rate_db_otm_cal_rate_get_cb get;
} dnx_ofp_rate_db_otm_cal_rate_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_cal_modified_set_cb set;
    dnx_ofp_rate_db_otm_cal_modified_get_cb get;
} dnx_ofp_rate_db_otm_cal_modified_cbs;


typedef struct {
    dnx_ofp_rate_db_otm_cal_nof_calcal_instances_set_cb set;
    dnx_ofp_rate_db_otm_cal_nof_calcal_instances_get_cb get;
} dnx_ofp_rate_db_otm_cal_nof_calcal_instances_cbs;


typedef struct {
    
    dnx_ofp_rate_db_otm_cal_rate_cbs rate;
    
    dnx_ofp_rate_db_otm_cal_modified_cbs modified;
    
    dnx_ofp_rate_db_otm_cal_nof_calcal_instances_cbs nof_calcal_instances;
} dnx_ofp_rate_db_otm_cal_cbs;


typedef struct {
    dnx_ofp_rate_db_calcal_is_modified_set_cb set;
    dnx_ofp_rate_db_calcal_is_modified_get_cb get;
} dnx_ofp_rate_db_calcal_is_modified_cbs;


typedef struct {
    dnx_ofp_rate_db_calcal_len_set_cb set;
    dnx_ofp_rate_db_calcal_len_get_cb get;
} dnx_ofp_rate_db_calcal_len_cbs;


typedef struct {
    dnx_ofp_rate_db_is_init_cb is_init;
    dnx_ofp_rate_db_init_cb init;
    
    dnx_ofp_rate_db_otm_cbs otm;
    
    dnx_ofp_rate_db_qpair_cbs qpair;
    
    dnx_ofp_rate_db_tcg_cbs tcg;
    
    dnx_ofp_rate_db_otm_cal_cbs otm_cal;
    
    dnx_ofp_rate_db_calcal_is_modified_cbs calcal_is_modified;
    
    dnx_ofp_rate_db_calcal_len_cbs calcal_len;
} dnx_ofp_rate_db_cbs;





extern dnx_ofp_rate_db_cbs dnx_ofp_rate_db;

#endif 
