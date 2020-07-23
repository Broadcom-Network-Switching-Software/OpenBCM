
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_QUALIFIER_ACCESS_H__
#define __DNX_FIELD_QUALIFIER_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_qualifier_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>



typedef int (*dnx_field_qual_sw_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_qual_sw_db_init_cb)(
    int unit);


typedef int (*dnx_field_qual_sw_db_info_set_cb)(
    int unit, uint32 info_idx_0, CONST dnx_field_user_qual_info_t *info);


typedef int (*dnx_field_qual_sw_db_info_get_cb)(
    int unit, uint32 info_idx_0, dnx_field_user_qual_info_t *info);


typedef int (*dnx_field_qual_sw_db_info_valid_set_cb)(
    int unit, uint32 info_idx_0, uint32 valid);


typedef int (*dnx_field_qual_sw_db_info_valid_get_cb)(
    int unit, uint32 info_idx_0, uint32 *valid);


typedef int (*dnx_field_qual_sw_db_info_field_id_set_cb)(
    int unit, uint32 info_idx_0, dbal_fields_e field_id);


typedef int (*dnx_field_qual_sw_db_info_field_id_get_cb)(
    int unit, uint32 info_idx_0, dbal_fields_e *field_id);


typedef int (*dnx_field_qual_sw_db_info_size_set_cb)(
    int unit, uint32 info_idx_0, uint32 size);


typedef int (*dnx_field_qual_sw_db_info_size_get_cb)(
    int unit, uint32 info_idx_0, uint32 *size);


typedef int (*dnx_field_qual_sw_db_info_bcm_id_set_cb)(
    int unit, uint32 info_idx_0, int bcm_id);


typedef int (*dnx_field_qual_sw_db_info_bcm_id_get_cb)(
    int unit, uint32 info_idx_0, int *bcm_id);


typedef int (*dnx_field_qual_sw_db_info_ref_count_set_cb)(
    int unit, uint32 info_idx_0, uint32 ref_count);


typedef int (*dnx_field_qual_sw_db_info_ref_count_get_cb)(
    int unit, uint32 info_idx_0, uint32 *ref_count);


typedef int (*dnx_field_qual_sw_db_info_flags_set_cb)(
    int unit, uint32 info_idx_0, dnx_field_qual_flags_e flags);


typedef int (*dnx_field_qual_sw_db_info_flags_get_cb)(
    int unit, uint32 info_idx_0, dnx_field_qual_flags_e *flags);




typedef struct {
    dnx_field_qual_sw_db_info_valid_set_cb set;
    dnx_field_qual_sw_db_info_valid_get_cb get;
} dnx_field_qual_sw_db_info_valid_cbs;


typedef struct {
    dnx_field_qual_sw_db_info_field_id_set_cb set;
    dnx_field_qual_sw_db_info_field_id_get_cb get;
} dnx_field_qual_sw_db_info_field_id_cbs;


typedef struct {
    dnx_field_qual_sw_db_info_size_set_cb set;
    dnx_field_qual_sw_db_info_size_get_cb get;
} dnx_field_qual_sw_db_info_size_cbs;


typedef struct {
    dnx_field_qual_sw_db_info_bcm_id_set_cb set;
    dnx_field_qual_sw_db_info_bcm_id_get_cb get;
} dnx_field_qual_sw_db_info_bcm_id_cbs;


typedef struct {
    dnx_field_qual_sw_db_info_ref_count_set_cb set;
    dnx_field_qual_sw_db_info_ref_count_get_cb get;
} dnx_field_qual_sw_db_info_ref_count_cbs;


typedef struct {
    dnx_field_qual_sw_db_info_flags_set_cb set;
    dnx_field_qual_sw_db_info_flags_get_cb get;
} dnx_field_qual_sw_db_info_flags_cbs;


typedef struct {
    dnx_field_qual_sw_db_info_set_cb set;
    dnx_field_qual_sw_db_info_get_cb get;
    
    dnx_field_qual_sw_db_info_valid_cbs valid;
    
    dnx_field_qual_sw_db_info_field_id_cbs field_id;
    
    dnx_field_qual_sw_db_info_size_cbs size;
    
    dnx_field_qual_sw_db_info_bcm_id_cbs bcm_id;
    
    dnx_field_qual_sw_db_info_ref_count_cbs ref_count;
    
    dnx_field_qual_sw_db_info_flags_cbs flags;
} dnx_field_qual_sw_db_info_cbs;


typedef struct {
    dnx_field_qual_sw_db_is_init_cb is_init;
    dnx_field_qual_sw_db_init_cb init;
    
    dnx_field_qual_sw_db_info_cbs info;
} dnx_field_qual_sw_db_cbs;






const char *
dnx_field_qual_flags_e_get_name(dnx_field_qual_flags_e value);




extern dnx_field_qual_sw_db_cbs dnx_field_qual_sw_db;

#endif 
