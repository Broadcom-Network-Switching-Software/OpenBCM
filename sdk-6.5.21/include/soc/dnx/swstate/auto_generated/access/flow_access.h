
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __FLOW_ACCESS_H__
#define __FLOW_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/flow_types.h>



typedef int (*flow_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*flow_db_init_cb)(
    int unit);


typedef int (*flow_db_flow_application_info_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*flow_db_flow_application_info_is_verify_disabled_set_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 is_verify_disabled);


typedef int (*flow_db_flow_application_info_is_verify_disabled_get_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 *is_verify_disabled);


typedef int (*flow_db_flow_application_info_is_error_recovery_disabled_set_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 is_error_recovery_disabled);


typedef int (*flow_db_flow_application_info_is_error_recovery_disabled_get_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 *is_error_recovery_disabled);




typedef struct {
    flow_db_flow_application_info_is_verify_disabled_set_cb set;
    flow_db_flow_application_info_is_verify_disabled_get_cb get;
} flow_db_flow_application_info_is_verify_disabled_cbs;


typedef struct {
    flow_db_flow_application_info_is_error_recovery_disabled_set_cb set;
    flow_db_flow_application_info_is_error_recovery_disabled_get_cb get;
} flow_db_flow_application_info_is_error_recovery_disabled_cbs;


typedef struct {
    flow_db_flow_application_info_alloc_cb alloc;
    
    flow_db_flow_application_info_is_verify_disabled_cbs is_verify_disabled;
    
    flow_db_flow_application_info_is_error_recovery_disabled_cbs is_error_recovery_disabled;
} flow_db_flow_application_info_cbs;


typedef struct {
    flow_db_is_init_cb is_init;
    flow_db_init_cb init;
    
    flow_db_flow_application_info_cbs flow_application_info;
} flow_db_cbs;





extern flow_db_cbs flow_db;

#endif 
