
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_ACCESS_H__
#define __SWITCH_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/switch_types.h>



typedef int (*switch_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*switch_db_init_cb)(
    int unit);


typedef int (*switch_db_module_verification_set_cb)(
    int unit, uint32 module_verification_idx_0, uint8 module_verification);


typedef int (*switch_db_module_verification_get_cb)(
    int unit, uint32 module_verification_idx_0, uint8 *module_verification);


typedef int (*switch_db_module_error_recovery_set_cb)(
    int unit, uint32 module_error_recovery_idx_0, uint8 module_error_recovery);


typedef int (*switch_db_module_error_recovery_get_cb)(
    int unit, uint32 module_error_recovery_idx_0, uint8 *module_error_recovery);


typedef int (*switch_db_l3mcastl2_fwd_type_set_cb)(
    int unit, uint8 l3mcastl2_fwd_type);


typedef int (*switch_db_l3mcastl2_fwd_type_get_cb)(
    int unit, uint8 *l3mcastl2_fwd_type);


typedef int (*switch_db_header_enablers_hashing_set_cb)(
    int unit, uint32 header_enablers_hashing_idx_0, uint32 header_enablers_hashing);


typedef int (*switch_db_header_enablers_hashing_get_cb)(
    int unit, uint32 header_enablers_hashing_idx_0, uint32 *header_enablers_hashing);




typedef struct {
    switch_db_module_verification_set_cb set;
    switch_db_module_verification_get_cb get;
} switch_db_module_verification_cbs;


typedef struct {
    switch_db_module_error_recovery_set_cb set;
    switch_db_module_error_recovery_get_cb get;
} switch_db_module_error_recovery_cbs;


typedef struct {
    switch_db_l3mcastl2_fwd_type_set_cb set;
    switch_db_l3mcastl2_fwd_type_get_cb get;
} switch_db_l3mcastl2_fwd_type_cbs;


typedef struct {
    switch_db_header_enablers_hashing_set_cb set;
    switch_db_header_enablers_hashing_get_cb get;
} switch_db_header_enablers_hashing_cbs;


typedef struct {
    switch_db_is_init_cb is_init;
    switch_db_init_cb init;
    
    switch_db_module_verification_cbs module_verification;
    
    switch_db_module_error_recovery_cbs module_error_recovery;
    
    switch_db_l3mcastl2_fwd_type_cbs l3mcastl2_fwd_type;
    
    switch_db_header_enablers_hashing_cbs header_enablers_hashing;
} switch_db_cbs;





extern switch_db_cbs switch_db;

#endif 
