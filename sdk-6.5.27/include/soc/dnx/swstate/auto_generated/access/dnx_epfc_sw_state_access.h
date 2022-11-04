
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EPFC_SW_STATE_ACCESS_H__
#define __DNX_EPFC_SW_STATE_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_epfc_sw_state_types.h>


typedef int (*dnx_epfc_interface_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_epfc_interface_db_init_cb)(
    int unit);

typedef int (*dnx_epfc_interface_db_is_initialized_set_cb)(
    int unit, uint32 is_initialized);

typedef int (*dnx_epfc_interface_db_is_initialized_get_cb)(
    int unit, uint32 *is_initialized);

typedef int (*dnx_epfc_interface_db_uc_num_set_cb)(
    int unit, uint32 uc_num);

typedef int (*dnx_epfc_interface_db_uc_num_get_cb)(
    int unit, uint32 *uc_num);



typedef struct {
    dnx_epfc_interface_db_is_initialized_set_cb set;
    dnx_epfc_interface_db_is_initialized_get_cb get;
} dnx_epfc_interface_db_is_initialized_cbs;

typedef struct {
    dnx_epfc_interface_db_uc_num_set_cb set;
    dnx_epfc_interface_db_uc_num_get_cb get;
} dnx_epfc_interface_db_uc_num_cbs;

typedef struct {
    dnx_epfc_interface_db_is_init_cb is_init;
    dnx_epfc_interface_db_init_cb init;
    dnx_epfc_interface_db_is_initialized_cbs is_initialized;
    dnx_epfc_interface_db_uc_num_cbs uc_num;
} dnx_epfc_interface_db_cbs;





extern dnx_epfc_interface_db_cbs dnx_epfc_interface_db;

#endif 
