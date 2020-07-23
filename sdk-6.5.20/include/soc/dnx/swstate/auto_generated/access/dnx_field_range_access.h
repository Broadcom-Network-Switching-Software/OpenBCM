
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_RANGE_ACCESS_H__
#define __DNX_FIELD_RANGE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_range_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef int (*dnx_field_range_sw_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_range_sw_db_init_cb)(
    int unit);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_range_types_set_cb)(
    int unit, uint32 ext_l4_ops_range_types_idx_0, dnx_field_range_type_e ext_l4_ops_range_types);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_range_types_get_cb)(
    int unit, uint32 ext_l4_ops_range_types_idx_0, dnx_field_range_type_e *ext_l4_ops_range_types);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_set_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, CONST dnx_field_range_type_qual_info_t *ext_l4_ops_ffc_quals);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_get_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_range_type_qual_info_t *ext_l4_ops_ffc_quals);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_set_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_qual_t dnx_qual);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_get_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_qual_t *dnx_qual);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_set_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_input_type_e input_type);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_get_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, dnx_field_input_type_e *input_type);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_set_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int input_arg);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_get_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int *input_arg);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_set_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int offset);


typedef int (*dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_get_cb)(
    int unit, uint32 ext_l4_ops_ffc_quals_idx_0, int *offset);




typedef struct {
    dnx_field_range_sw_db_ext_l4_ops_range_types_set_cb set;
    dnx_field_range_sw_db_ext_l4_ops_range_types_get_cb get;
} dnx_field_range_sw_db_ext_l4_ops_range_types_cbs;


typedef struct {
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_set_cb set;
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_get_cb get;
} dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_cbs;


typedef struct {
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_set_cb set;
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_get_cb get;
} dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_cbs;


typedef struct {
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_set_cb set;
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_get_cb get;
} dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_cbs;


typedef struct {
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_set_cb set;
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_get_cb get;
} dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_cbs;


typedef struct {
    
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_cbs input_type;
    
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_cbs input_arg;
    
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_cbs offset;
} dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_cbs;


typedef struct {
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_set_cb set;
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_get_cb get;
    
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_cbs dnx_qual;
    
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_cbs qual_info;
} dnx_field_range_sw_db_ext_l4_ops_ffc_quals_cbs;


typedef struct {
    dnx_field_range_sw_db_is_init_cb is_init;
    dnx_field_range_sw_db_init_cb init;
    
    dnx_field_range_sw_db_ext_l4_ops_range_types_cbs ext_l4_ops_range_types;
    
    dnx_field_range_sw_db_ext_l4_ops_ffc_quals_cbs ext_l4_ops_ffc_quals;
} dnx_field_range_sw_db_cbs;






const char *
dnx_field_range_type_e_get_name(dnx_field_range_type_e value);




extern dnx_field_range_sw_db_cbs dnx_field_range_sw_db;

#endif 
