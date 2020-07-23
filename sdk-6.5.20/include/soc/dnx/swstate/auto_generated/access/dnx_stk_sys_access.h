
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_STK_SYS_ACCESS_H__
#define __DNX_STK_SYS_ACCESS_H__

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_stk_sys_types.h>



typedef int (*dnx_stk_sys_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_stk_sys_db_init_cb)(
    int unit);


typedef int (*dnx_stk_sys_db_nof_fap_ids_set_cb)(
    int unit, int nof_fap_ids);


typedef int (*dnx_stk_sys_db_nof_fap_ids_get_cb)(
    int unit, int *nof_fap_ids);


typedef int (*dnx_stk_sys_db_nof_fap_ids_inc_cb)(
    int unit, uint32 inc_value);


typedef int (*dnx_stk_sys_db_nof_fap_ids_dec_cb)(
    int unit, uint32 dec_value);


typedef int (*dnx_stk_sys_db_module_enable_done_set_cb)(
    int unit, int module_enable_done);


typedef int (*dnx_stk_sys_db_module_enable_done_get_cb)(
    int unit, int *module_enable_done);




typedef struct {
    dnx_stk_sys_db_nof_fap_ids_set_cb set;
    dnx_stk_sys_db_nof_fap_ids_get_cb get;
    dnx_stk_sys_db_nof_fap_ids_inc_cb inc;
    dnx_stk_sys_db_nof_fap_ids_dec_cb dec;
} dnx_stk_sys_db_nof_fap_ids_cbs;


typedef struct {
    dnx_stk_sys_db_module_enable_done_set_cb set;
    dnx_stk_sys_db_module_enable_done_get_cb get;
} dnx_stk_sys_db_module_enable_done_cbs;


typedef struct {
    dnx_stk_sys_db_is_init_cb is_init;
    dnx_stk_sys_db_init_cb init;
    
    dnx_stk_sys_db_nof_fap_ids_cbs nof_fap_ids;
    
    dnx_stk_sys_db_module_enable_done_cbs module_enable_done;
} dnx_stk_sys_db_cbs;





extern dnx_stk_sys_db_cbs dnx_stk_sys_db;
#endif  

#endif 
