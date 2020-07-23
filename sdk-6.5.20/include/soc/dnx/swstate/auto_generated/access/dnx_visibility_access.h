
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_VISIBILITY_ACCESS_H__
#define __DNX_VISIBILITY_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_visibility_types.h>
#include <bcm/instru.h>



typedef int (*visibility_info_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*visibility_info_db_init_cb)(
    int unit);


typedef int (*visibility_info_db_visib_mode_set_cb)(
    int unit, int visib_mode);


typedef int (*visibility_info_db_visib_mode_get_cb)(
    int unit, int *visib_mode);




typedef struct {
    visibility_info_db_visib_mode_set_cb set;
    visibility_info_db_visib_mode_get_cb get;
} visibility_info_db_visib_mode_cbs;


typedef struct {
    visibility_info_db_is_init_cb is_init;
    visibility_info_db_init_cb init;
    
    visibility_info_db_visib_mode_cbs visib_mode;
} visibility_info_db_cbs;





extern visibility_info_db_cbs visibility_info_db;

#endif 
