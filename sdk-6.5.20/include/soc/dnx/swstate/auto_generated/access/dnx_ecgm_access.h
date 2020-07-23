
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ECGM_ACCESS_H__
#define __DNX_ECGM_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ecgm_types.h>
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>



typedef int (*dnx_ecgm_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_ecgm_db_init_cb)(
    int unit);


typedef int (*dnx_ecgm_db_interface_caching_port_set_cb)(
    int unit, bcm_port_t port);


typedef int (*dnx_ecgm_db_interface_caching_port_get_cb)(
    int unit, bcm_port_t *port);


typedef int (*dnx_ecgm_db_interface_caching_info_set_cb)(
    int unit, dnx_ecgm_interface_profile_info_t info);


typedef int (*dnx_ecgm_db_interface_caching_info_get_cb)(
    int unit, dnx_ecgm_interface_profile_info_t *info);


typedef int (*dnx_ecgm_db_interface_caching_profile_id_set_cb)(
    int unit, int profile_id);


typedef int (*dnx_ecgm_db_interface_caching_profile_id_get_cb)(
    int unit, int *profile_id);


typedef int (*dnx_ecgm_db_interface_caching_valid_set_cb)(
    int unit, int valid);


typedef int (*dnx_ecgm_db_interface_caching_valid_get_cb)(
    int unit, int *valid);


typedef int (*dnx_ecgm_db_port_caching_port_set_cb)(
    int unit, bcm_port_t port);


typedef int (*dnx_ecgm_db_port_caching_port_get_cb)(
    int unit, bcm_port_t *port);


typedef int (*dnx_ecgm_db_port_caching_info_set_cb)(
    int unit, CONST dnx_ecgm_port_profile_info_t *info);


typedef int (*dnx_ecgm_db_port_caching_info_get_cb)(
    int unit, CONST dnx_ecgm_port_profile_info_t **info);


typedef int (*dnx_ecgm_db_port_caching_profile_id_set_cb)(
    int unit, int profile_id);


typedef int (*dnx_ecgm_db_port_caching_profile_id_get_cb)(
    int unit, int *profile_id);


typedef int (*dnx_ecgm_db_port_caching_valid_set_cb)(
    int unit, int valid);


typedef int (*dnx_ecgm_db_port_caching_valid_get_cb)(
    int unit, int *valid);




typedef struct {
    dnx_ecgm_db_interface_caching_port_set_cb set;
    dnx_ecgm_db_interface_caching_port_get_cb get;
} dnx_ecgm_db_interface_caching_port_cbs;


typedef struct {
    dnx_ecgm_db_interface_caching_info_set_cb set;
    dnx_ecgm_db_interface_caching_info_get_cb get;
} dnx_ecgm_db_interface_caching_info_cbs;


typedef struct {
    dnx_ecgm_db_interface_caching_profile_id_set_cb set;
    dnx_ecgm_db_interface_caching_profile_id_get_cb get;
} dnx_ecgm_db_interface_caching_profile_id_cbs;


typedef struct {
    dnx_ecgm_db_interface_caching_valid_set_cb set;
    dnx_ecgm_db_interface_caching_valid_get_cb get;
} dnx_ecgm_db_interface_caching_valid_cbs;


typedef struct {
    
    dnx_ecgm_db_interface_caching_port_cbs port;
    
    dnx_ecgm_db_interface_caching_info_cbs info;
    
    dnx_ecgm_db_interface_caching_profile_id_cbs profile_id;
    
    dnx_ecgm_db_interface_caching_valid_cbs valid;
} dnx_ecgm_db_interface_caching_cbs;


typedef struct {
    dnx_ecgm_db_port_caching_port_set_cb set;
    dnx_ecgm_db_port_caching_port_get_cb get;
} dnx_ecgm_db_port_caching_port_cbs;


typedef struct {
    dnx_ecgm_db_port_caching_info_set_cb set;
    dnx_ecgm_db_port_caching_info_get_cb get;
} dnx_ecgm_db_port_caching_info_cbs;


typedef struct {
    dnx_ecgm_db_port_caching_profile_id_set_cb set;
    dnx_ecgm_db_port_caching_profile_id_get_cb get;
} dnx_ecgm_db_port_caching_profile_id_cbs;


typedef struct {
    dnx_ecgm_db_port_caching_valid_set_cb set;
    dnx_ecgm_db_port_caching_valid_get_cb get;
} dnx_ecgm_db_port_caching_valid_cbs;


typedef struct {
    
    dnx_ecgm_db_port_caching_port_cbs port;
    
    dnx_ecgm_db_port_caching_info_cbs info;
    
    dnx_ecgm_db_port_caching_profile_id_cbs profile_id;
    
    dnx_ecgm_db_port_caching_valid_cbs valid;
} dnx_ecgm_db_port_caching_cbs;


typedef struct {
    dnx_ecgm_db_is_init_cb is_init;
    dnx_ecgm_db_init_cb init;
    
    dnx_ecgm_db_interface_caching_cbs interface_caching;
    
    dnx_ecgm_db_port_caching_cbs port_caching;
} dnx_ecgm_db_cbs;





extern dnx_ecgm_db_cbs dnx_ecgm_db;

#endif 
