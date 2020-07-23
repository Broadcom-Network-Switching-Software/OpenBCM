
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCH_CONFIG_ACCESS_H__
#define __DNX_SCH_CONFIG_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_sch_config_types.h>
#include <include/bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>



typedef int (*cosq_config_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*cosq_config_init_cb)(
    int unit);


typedef int (*cosq_config_hr_group_bw_set_cb)(
    int unit, uint32 hr_group_bw_idx_0, uint32 hr_group_bw_idx_1, int hr_group_bw);


typedef int (*cosq_config_hr_group_bw_get_cb)(
    int unit, uint32 hr_group_bw_idx_0, uint32 hr_group_bw_idx_1, int *hr_group_bw);


typedef int (*cosq_config_hr_group_bw_alloc_cb)(
    int unit, uint32 hr_group_bw_idx_0);


typedef int (*cosq_config_groups_bw_set_cb)(
    int unit, uint32 groups_bw_idx_0, uint32 groups_bw_idx_1, int groups_bw);


typedef int (*cosq_config_groups_bw_get_cb)(
    int unit, uint32 groups_bw_idx_0, uint32 groups_bw_idx_1, int *groups_bw);


typedef int (*cosq_config_ipf_config_mode_set_cb)(
    int unit, dnx_sch_ipf_config_mode_e ipf_config_mode);


typedef int (*cosq_config_ipf_config_mode_get_cb)(
    int unit, dnx_sch_ipf_config_mode_e *ipf_config_mode);


typedef int (*cosq_config_flow_alloc_cb)(
    int unit, uint32 flow_idx_0);


typedef int (*cosq_config_flow_credit_src_set_cb)(
    int unit, uint32 flow_idx_0, uint32 flow_idx_1, CONST dnx_sch_credit_src_info_t *credit_src);


typedef int (*cosq_config_flow_credit_src_get_cb)(
    int unit, uint32 flow_idx_0, uint32 flow_idx_1, dnx_sch_credit_src_info_t *credit_src);


typedef int (*cosq_config_flow_credit_src_weight_set_cb)(
    int unit, uint32 flow_idx_0, uint32 flow_idx_1, int weight);


typedef int (*cosq_config_flow_credit_src_weight_get_cb)(
    int unit, uint32 flow_idx_0, uint32 flow_idx_1, int *weight);


typedef int (*cosq_config_flow_credit_src_mode_set_cb)(
    int unit, uint32 flow_idx_0, uint32 flow_idx_1, int mode);


typedef int (*cosq_config_flow_credit_src_mode_get_cb)(
    int unit, uint32 flow_idx_0, uint32 flow_idx_1, int *mode);


typedef int (*cosq_config_connector_set_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, CONST dnx_sch_connector_swstate_info_t *connector);


typedef int (*cosq_config_connector_get_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, dnx_sch_connector_swstate_info_t *connector);


typedef int (*cosq_config_connector_alloc_cb)(
    int unit, uint32 connector_idx_0);


typedef int (*cosq_config_connector_num_cos_set_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 num_cos);


typedef int (*cosq_config_connector_num_cos_get_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 *num_cos);


typedef int (*cosq_config_connector_connection_valid_set_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 connection_valid);


typedef int (*cosq_config_connector_connection_valid_get_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 *connection_valid);


typedef int (*cosq_config_connector_src_modid_set_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 src_modid);


typedef int (*cosq_config_connector_src_modid_get_cb)(
    int unit, uint32 connector_idx_0, uint32 connector_idx_1, uint8 *src_modid);


typedef int (*cosq_config_se_alloc_cb)(
    int unit, uint32 se_idx_0);


typedef int (*cosq_config_se_child_count_set_cb)(
    int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 child_count);


typedef int (*cosq_config_se_child_count_get_cb)(
    int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 *child_count);


typedef int (*cosq_config_se_child_count_inc_cb)(
    int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 inc_value);


typedef int (*cosq_config_se_child_count_dec_cb)(
    int unit, uint32 se_idx_0, uint32 se_idx_1, uint32 dec_value);




typedef struct {
    cosq_config_hr_group_bw_set_cb set;
    cosq_config_hr_group_bw_get_cb get;
    cosq_config_hr_group_bw_alloc_cb alloc;
} cosq_config_hr_group_bw_cbs;


typedef struct {
    cosq_config_groups_bw_set_cb set;
    cosq_config_groups_bw_get_cb get;
} cosq_config_groups_bw_cbs;


typedef struct {
    cosq_config_ipf_config_mode_set_cb set;
    cosq_config_ipf_config_mode_get_cb get;
} cosq_config_ipf_config_mode_cbs;


typedef struct {
    cosq_config_flow_credit_src_weight_set_cb set;
    cosq_config_flow_credit_src_weight_get_cb get;
} cosq_config_flow_credit_src_weight_cbs;


typedef struct {
    cosq_config_flow_credit_src_mode_set_cb set;
    cosq_config_flow_credit_src_mode_get_cb get;
} cosq_config_flow_credit_src_mode_cbs;


typedef struct {
    cosq_config_flow_credit_src_set_cb set;
    cosq_config_flow_credit_src_get_cb get;
    
    cosq_config_flow_credit_src_weight_cbs weight;
    
    cosq_config_flow_credit_src_mode_cbs mode;
} cosq_config_flow_credit_src_cbs;


typedef struct {
    cosq_config_flow_alloc_cb alloc;
    
    cosq_config_flow_credit_src_cbs credit_src;
} cosq_config_flow_cbs;


typedef struct {
    cosq_config_connector_num_cos_set_cb set;
    cosq_config_connector_num_cos_get_cb get;
} cosq_config_connector_num_cos_cbs;


typedef struct {
    cosq_config_connector_connection_valid_set_cb set;
    cosq_config_connector_connection_valid_get_cb get;
} cosq_config_connector_connection_valid_cbs;


typedef struct {
    cosq_config_connector_src_modid_set_cb set;
    cosq_config_connector_src_modid_get_cb get;
} cosq_config_connector_src_modid_cbs;


typedef struct {
    cosq_config_connector_set_cb set;
    cosq_config_connector_get_cb get;
    cosq_config_connector_alloc_cb alloc;
    
    cosq_config_connector_num_cos_cbs num_cos;
    
    cosq_config_connector_connection_valid_cbs connection_valid;
    
    cosq_config_connector_src_modid_cbs src_modid;
} cosq_config_connector_cbs;


typedef struct {
    cosq_config_se_child_count_set_cb set;
    cosq_config_se_child_count_get_cb get;
    cosq_config_se_child_count_inc_cb inc;
    cosq_config_se_child_count_dec_cb dec;
} cosq_config_se_child_count_cbs;


typedef struct {
    cosq_config_se_alloc_cb alloc;
    
    cosq_config_se_child_count_cbs child_count;
} cosq_config_se_cbs;


typedef struct {
    cosq_config_is_init_cb is_init;
    cosq_config_init_cb init;
    
    cosq_config_hr_group_bw_cbs hr_group_bw;
    
    cosq_config_groups_bw_cbs groups_bw;
    
    cosq_config_ipf_config_mode_cbs ipf_config_mode;
    
    cosq_config_flow_cbs flow;
    
    cosq_config_connector_cbs connector;
    
    cosq_config_se_cbs se;
} cosq_config_cbs;






const char *
dnx_sch_ipf_config_mode_e_get_name(dnx_sch_ipf_config_mode_e value);




extern cosq_config_cbs cosq_config;

#endif 
