
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_APPTYPE_ACCESS_H__
#define __DNX_FIELD_APPTYPE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_apptype_types.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>



typedef int (*dnx_field_apptype_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_apptype_sw_init_cb)(
    int unit);


typedef int (*dnx_field_apptype_sw_predef_info_set_cb)(
    int unit, uint32 predef_info_idx_0, CONST dnx_field_predef_apptype_info_t *predef_info);


typedef int (*dnx_field_apptype_sw_predef_info_get_cb)(
    int unit, uint32 predef_info_idx_0, dnx_field_predef_apptype_info_t *predef_info);


typedef int (*dnx_field_apptype_sw_predef_info_nof_context_ids_set_cb)(
    int unit, uint32 predef_info_idx_0, uint8 nof_context_ids);


typedef int (*dnx_field_apptype_sw_predef_info_nof_context_ids_get_cb)(
    int unit, uint32 predef_info_idx_0, uint8 *nof_context_ids);


typedef int (*dnx_field_apptype_sw_predef_info_context_ids_set_cb)(
    int unit, uint32 predef_info_idx_0, uint32 context_ids_idx_0, uint8 context_ids);


typedef int (*dnx_field_apptype_sw_predef_info_context_ids_get_cb)(
    int unit, uint32 predef_info_idx_0, uint32 context_ids_idx_0, uint8 *context_ids);


typedef int (*dnx_field_apptype_sw_predef_info_valid_for_kbp_set_cb)(
    int unit, uint32 predef_info_idx_0, uint8 valid_for_kbp);


typedef int (*dnx_field_apptype_sw_predef_info_valid_for_kbp_get_cb)(
    int unit, uint32 predef_info_idx_0, uint8 *valid_for_kbp);


typedef int (*dnx_field_apptype_sw_user_def_info_set_cb)(
    int unit, uint32 user_def_info_idx_0, CONST dnx_field_user_apptype_info_t *user_def_info);


typedef int (*dnx_field_apptype_sw_user_def_info_get_cb)(
    int unit, uint32 user_def_info_idx_0, dnx_field_user_apptype_info_t *user_def_info);


typedef int (*dnx_field_apptype_sw_user_def_info_opcode_id_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 opcode_id);


typedef int (*dnx_field_apptype_sw_user_def_info_opcode_id_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *opcode_id);


typedef int (*dnx_field_apptype_sw_user_def_info_acl_context_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 acl_context);


typedef int (*dnx_field_apptype_sw_user_def_info_acl_context_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *acl_context);


typedef int (*dnx_field_apptype_sw_user_def_info_base_apptype_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 base_apptype);


typedef int (*dnx_field_apptype_sw_user_def_info_base_apptype_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *base_apptype);


typedef int (*dnx_field_apptype_sw_user_def_info_flags_set_cb)(
    int unit, uint32 user_def_info_idx_0, dnx_field_apptype_flags_e flags);


typedef int (*dnx_field_apptype_sw_user_def_info_flags_get_cb)(
    int unit, uint32 user_def_info_idx_0, dnx_field_apptype_flags_e *flags);


typedef int (*dnx_field_apptype_sw_user_def_info_name_stringncat_cb)(
    int unit, uint32 user_def_info_idx_0, char *src);


typedef int (*dnx_field_apptype_sw_user_def_info_name_stringncmp_cb)(
    int unit, uint32 user_def_info_idx_0, char *cmp_string, int *result);


typedef int (*dnx_field_apptype_sw_user_def_info_name_stringncpy_cb)(
    int unit, uint32 user_def_info_idx_0, char *src);


typedef int (*dnx_field_apptype_sw_user_def_info_name_stringlen_cb)(
    int unit, uint32 user_def_info_idx_0, uint32 *size);


typedef int (*dnx_field_apptype_sw_user_def_info_name_stringget_cb)(
    int unit, uint32 user_def_info_idx_0, char *o_string);


typedef int (*dnx_field_apptype_sw_user_def_info_profile_id_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 profile_id);


typedef int (*dnx_field_apptype_sw_user_def_info_profile_id_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *profile_id);




typedef struct {
    dnx_field_apptype_sw_predef_info_nof_context_ids_set_cb set;
    dnx_field_apptype_sw_predef_info_nof_context_ids_get_cb get;
} dnx_field_apptype_sw_predef_info_nof_context_ids_cbs;


typedef struct {
    dnx_field_apptype_sw_predef_info_context_ids_set_cb set;
    dnx_field_apptype_sw_predef_info_context_ids_get_cb get;
} dnx_field_apptype_sw_predef_info_context_ids_cbs;


typedef struct {
    dnx_field_apptype_sw_predef_info_valid_for_kbp_set_cb set;
    dnx_field_apptype_sw_predef_info_valid_for_kbp_get_cb get;
} dnx_field_apptype_sw_predef_info_valid_for_kbp_cbs;


typedef struct {
    dnx_field_apptype_sw_predef_info_set_cb set;
    dnx_field_apptype_sw_predef_info_get_cb get;
    
    dnx_field_apptype_sw_predef_info_nof_context_ids_cbs nof_context_ids;
    
    dnx_field_apptype_sw_predef_info_context_ids_cbs context_ids;
    
    dnx_field_apptype_sw_predef_info_valid_for_kbp_cbs valid_for_kbp;
} dnx_field_apptype_sw_predef_info_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_opcode_id_set_cb set;
    dnx_field_apptype_sw_user_def_info_opcode_id_get_cb get;
} dnx_field_apptype_sw_user_def_info_opcode_id_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_acl_context_set_cb set;
    dnx_field_apptype_sw_user_def_info_acl_context_get_cb get;
} dnx_field_apptype_sw_user_def_info_acl_context_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_base_apptype_set_cb set;
    dnx_field_apptype_sw_user_def_info_base_apptype_get_cb get;
} dnx_field_apptype_sw_user_def_info_base_apptype_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_flags_set_cb set;
    dnx_field_apptype_sw_user_def_info_flags_get_cb get;
} dnx_field_apptype_sw_user_def_info_flags_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_name_stringncat_cb stringncat;
    dnx_field_apptype_sw_user_def_info_name_stringncmp_cb stringncmp;
    dnx_field_apptype_sw_user_def_info_name_stringncpy_cb stringncpy;
    dnx_field_apptype_sw_user_def_info_name_stringlen_cb stringlen;
    dnx_field_apptype_sw_user_def_info_name_stringget_cb stringget;
} dnx_field_apptype_sw_user_def_info_name_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_profile_id_set_cb set;
    dnx_field_apptype_sw_user_def_info_profile_id_get_cb get;
} dnx_field_apptype_sw_user_def_info_profile_id_cbs;


typedef struct {
    dnx_field_apptype_sw_user_def_info_set_cb set;
    dnx_field_apptype_sw_user_def_info_get_cb get;
    
    dnx_field_apptype_sw_user_def_info_opcode_id_cbs opcode_id;
    
    dnx_field_apptype_sw_user_def_info_acl_context_cbs acl_context;
    
    dnx_field_apptype_sw_user_def_info_base_apptype_cbs base_apptype;
    
    dnx_field_apptype_sw_user_def_info_flags_cbs flags;
    
    dnx_field_apptype_sw_user_def_info_name_cbs name;
    
    dnx_field_apptype_sw_user_def_info_profile_id_cbs profile_id;
} dnx_field_apptype_sw_user_def_info_cbs;


typedef struct {
    dnx_field_apptype_sw_is_init_cb is_init;
    dnx_field_apptype_sw_init_cb init;
    
    dnx_field_apptype_sw_predef_info_cbs predef_info;
    
    dnx_field_apptype_sw_user_def_info_cbs user_def_info;
} dnx_field_apptype_sw_cbs;






const char *
dnx_field_apptype_flags_e_get_name(dnx_field_apptype_flags_e value);




extern dnx_field_apptype_sw_cbs dnx_field_apptype_sw;

#endif 
