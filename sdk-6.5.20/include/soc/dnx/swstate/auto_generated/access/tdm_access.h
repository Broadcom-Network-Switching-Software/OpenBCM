
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __TDM_ACCESS_H__
#define __TDM_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/tdm_types.h>



typedef int (*tdm_ftmh_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*tdm_ftmh_info_init_cb)(
    int unit);


typedef int (*tdm_ftmh_info_tdm_header_is_ftmh_set_cb)(
    int unit, uint8 tdm_header_is_ftmh);


typedef int (*tdm_ftmh_info_tdm_header_is_ftmh_get_cb)(
    int unit, uint8 *tdm_header_is_ftmh);


typedef int (*tdm_ftmh_info_use_optimized_ftmh_set_cb)(
    int unit, uint8 use_optimized_ftmh);


typedef int (*tdm_ftmh_info_use_optimized_ftmh_get_cb)(
    int unit, uint8 *use_optimized_ftmh);




typedef struct {
    tdm_ftmh_info_tdm_header_is_ftmh_set_cb set;
    tdm_ftmh_info_tdm_header_is_ftmh_get_cb get;
} tdm_ftmh_info_tdm_header_is_ftmh_cbs;


typedef struct {
    tdm_ftmh_info_use_optimized_ftmh_set_cb set;
    tdm_ftmh_info_use_optimized_ftmh_get_cb get;
} tdm_ftmh_info_use_optimized_ftmh_cbs;


typedef struct {
    tdm_ftmh_info_is_init_cb is_init;
    tdm_ftmh_info_init_cb init;
    
    tdm_ftmh_info_tdm_header_is_ftmh_cbs tdm_header_is_ftmh;
    
    tdm_ftmh_info_use_optimized_ftmh_cbs use_optimized_ftmh;
} tdm_ftmh_info_cbs;





extern tdm_ftmh_info_cbs tdm_ftmh_info;

#endif 
