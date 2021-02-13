/** \file dnx/swstate/auto_generated/access/dnx_field_apptype_access.h
 *
 * sw state functions declarations
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_APPTYPE_ACCESS_H__
#define __DNX_FIELD_APPTYPE_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_field_apptype_types.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_field_apptype_sw_is_init
 */
typedef int (*dnx_field_apptype_sw_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_field_apptype_sw_init
 */
typedef int (*dnx_field_apptype_sw_init_cb)(
    int unit);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_set
 */
typedef int (*dnx_field_apptype_sw_predef_info_set_cb)(
    int unit, uint32 predef_info_idx_0, CONST dnx_field_predef_apptype_info_t *predef_info);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_get
 */
typedef int (*dnx_field_apptype_sw_predef_info_get_cb)(
    int unit, uint32 predef_info_idx_0, dnx_field_predef_apptype_info_t *predef_info);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_nof_context_ids_set
 */
typedef int (*dnx_field_apptype_sw_predef_info_nof_context_ids_set_cb)(
    int unit, uint32 predef_info_idx_0, uint8 nof_context_ids);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_nof_context_ids_get
 */
typedef int (*dnx_field_apptype_sw_predef_info_nof_context_ids_get_cb)(
    int unit, uint32 predef_info_idx_0, uint8 *nof_context_ids);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_context_ids_set
 */
typedef int (*dnx_field_apptype_sw_predef_info_context_ids_set_cb)(
    int unit, uint32 predef_info_idx_0, uint32 context_ids_idx_0, uint8 context_ids);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_context_ids_get
 */
typedef int (*dnx_field_apptype_sw_predef_info_context_ids_get_cb)(
    int unit, uint32 predef_info_idx_0, uint32 context_ids_idx_0, uint8 *context_ids);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_valid_for_kbp_set
 */
typedef int (*dnx_field_apptype_sw_predef_info_valid_for_kbp_set_cb)(
    int unit, uint32 predef_info_idx_0, uint8 valid_for_kbp);

/**
 * implemented by: dnx_field_apptype_sw_predef_info_valid_for_kbp_get
 */
typedef int (*dnx_field_apptype_sw_predef_info_valid_for_kbp_get_cb)(
    int unit, uint32 predef_info_idx_0, uint8 *valid_for_kbp);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_set
 */
typedef int (*dnx_field_apptype_sw_user_def_info_set_cb)(
    int unit, uint32 user_def_info_idx_0, CONST dnx_field_user_apptype_info_t *user_def_info);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_get
 */
typedef int (*dnx_field_apptype_sw_user_def_info_get_cb)(
    int unit, uint32 user_def_info_idx_0, dnx_field_user_apptype_info_t *user_def_info);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_opcode_id_set
 */
typedef int (*dnx_field_apptype_sw_user_def_info_opcode_id_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 opcode_id);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_opcode_id_get
 */
typedef int (*dnx_field_apptype_sw_user_def_info_opcode_id_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *opcode_id);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_acl_context_set
 */
typedef int (*dnx_field_apptype_sw_user_def_info_acl_context_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 acl_context);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_acl_context_get
 */
typedef int (*dnx_field_apptype_sw_user_def_info_acl_context_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *acl_context);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_base_apptype_set
 */
typedef int (*dnx_field_apptype_sw_user_def_info_base_apptype_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 base_apptype);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_base_apptype_get
 */
typedef int (*dnx_field_apptype_sw_user_def_info_base_apptype_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *base_apptype);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_flags_set
 */
typedef int (*dnx_field_apptype_sw_user_def_info_flags_set_cb)(
    int unit, uint32 user_def_info_idx_0, dnx_field_apptype_flags_e flags);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_flags_get
 */
typedef int (*dnx_field_apptype_sw_user_def_info_flags_get_cb)(
    int unit, uint32 user_def_info_idx_0, dnx_field_apptype_flags_e *flags);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_name_stringncat
 */
typedef int (*dnx_field_apptype_sw_user_def_info_name_stringncat_cb)(
    int unit, uint32 user_def_info_idx_0, char *src);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_name_stringncmp
 */
typedef int (*dnx_field_apptype_sw_user_def_info_name_stringncmp_cb)(
    int unit, uint32 user_def_info_idx_0, char *cmp_string, int *result);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_name_stringncpy
 */
typedef int (*dnx_field_apptype_sw_user_def_info_name_stringncpy_cb)(
    int unit, uint32 user_def_info_idx_0, char *src);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_name_stringlen
 */
typedef int (*dnx_field_apptype_sw_user_def_info_name_stringlen_cb)(
    int unit, uint32 user_def_info_idx_0, uint32 *size);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_name_stringget
 */
typedef int (*dnx_field_apptype_sw_user_def_info_name_stringget_cb)(
    int unit, uint32 user_def_info_idx_0, char *o_string);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_profile_id_set
 */
typedef int (*dnx_field_apptype_sw_user_def_info_profile_id_set_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 profile_id);

/**
 * implemented by: dnx_field_apptype_sw_user_def_info_profile_id_get
 */
typedef int (*dnx_field_apptype_sw_user_def_info_profile_id_get_cb)(
    int unit, uint32 user_def_info_idx_0, uint8 *profile_id);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable nof_context_ids
 */
typedef struct {
    dnx_field_apptype_sw_predef_info_nof_context_ids_set_cb set;
    dnx_field_apptype_sw_predef_info_nof_context_ids_get_cb get;
} dnx_field_apptype_sw_predef_info_nof_context_ids_cbs;

/**
 * This structure holds the access functions for the variable context_ids
 */
typedef struct {
    dnx_field_apptype_sw_predef_info_context_ids_set_cb set;
    dnx_field_apptype_sw_predef_info_context_ids_get_cb get;
} dnx_field_apptype_sw_predef_info_context_ids_cbs;

/**
 * This structure holds the access functions for the variable valid_for_kbp
 */
typedef struct {
    dnx_field_apptype_sw_predef_info_valid_for_kbp_set_cb set;
    dnx_field_apptype_sw_predef_info_valid_for_kbp_get_cb get;
} dnx_field_apptype_sw_predef_info_valid_for_kbp_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_predef_apptype_info_t
 */
typedef struct {
    dnx_field_apptype_sw_predef_info_set_cb set;
    dnx_field_apptype_sw_predef_info_get_cb get;
    /**
     * Access struct for nof_context_ids
     */
    dnx_field_apptype_sw_predef_info_nof_context_ids_cbs nof_context_ids;
    /**
     * Access struct for context_ids
     */
    dnx_field_apptype_sw_predef_info_context_ids_cbs context_ids;
    /**
     * Access struct for valid_for_kbp
     */
    dnx_field_apptype_sw_predef_info_valid_for_kbp_cbs valid_for_kbp;
} dnx_field_apptype_sw_predef_info_cbs;

/**
 * This structure holds the access functions for the variable opcode_id
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_opcode_id_set_cb set;
    dnx_field_apptype_sw_user_def_info_opcode_id_get_cb get;
} dnx_field_apptype_sw_user_def_info_opcode_id_cbs;

/**
 * This structure holds the access functions for the variable acl_context
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_acl_context_set_cb set;
    dnx_field_apptype_sw_user_def_info_acl_context_get_cb get;
} dnx_field_apptype_sw_user_def_info_acl_context_cbs;

/**
 * This structure holds the access functions for the variable base_apptype
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_base_apptype_set_cb set;
    dnx_field_apptype_sw_user_def_info_base_apptype_get_cb get;
} dnx_field_apptype_sw_user_def_info_base_apptype_cbs;

/**
 * This structure holds the access functions for the variable flags
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_flags_set_cb set;
    dnx_field_apptype_sw_user_def_info_flags_get_cb get;
} dnx_field_apptype_sw_user_def_info_flags_cbs;

/**
 * This structure holds the access functions for the variable name
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_name_stringncat_cb stringncat;
    dnx_field_apptype_sw_user_def_info_name_stringncmp_cb stringncmp;
    dnx_field_apptype_sw_user_def_info_name_stringncpy_cb stringncpy;
    dnx_field_apptype_sw_user_def_info_name_stringlen_cb stringlen;
    dnx_field_apptype_sw_user_def_info_name_stringget_cb stringget;
} dnx_field_apptype_sw_user_def_info_name_cbs;

/**
 * This structure holds the access functions for the variable profile_id
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_profile_id_set_cb set;
    dnx_field_apptype_sw_user_def_info_profile_id_get_cb get;
} dnx_field_apptype_sw_user_def_info_profile_id_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_user_apptype_info_t
 */
typedef struct {
    dnx_field_apptype_sw_user_def_info_set_cb set;
    dnx_field_apptype_sw_user_def_info_get_cb get;
    /**
     * Access struct for opcode_id
     */
    dnx_field_apptype_sw_user_def_info_opcode_id_cbs opcode_id;
    /**
     * Access struct for acl_context
     */
    dnx_field_apptype_sw_user_def_info_acl_context_cbs acl_context;
    /**
     * Access struct for base_apptype
     */
    dnx_field_apptype_sw_user_def_info_base_apptype_cbs base_apptype;
    /**
     * Access struct for flags
     */
    dnx_field_apptype_sw_user_def_info_flags_cbs flags;
    /**
     * Access struct for name
     */
    dnx_field_apptype_sw_user_def_info_name_cbs name;
    /**
     * Access struct for profile_id
     */
    dnx_field_apptype_sw_user_def_info_profile_id_cbs profile_id;
} dnx_field_apptype_sw_user_def_info_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_apptype_sw_t
 */
typedef struct {
    dnx_field_apptype_sw_is_init_cb is_init;
    dnx_field_apptype_sw_init_cb init;
    /**
     * Access struct for predef_info
     */
    dnx_field_apptype_sw_predef_info_cbs predef_info;
    /**
     * Access struct for user_def_info
     */
    dnx_field_apptype_sw_user_def_info_cbs user_def_info;
} dnx_field_apptype_sw_cbs;

/*
 * Global Variables
 */

/*
 * FUNCTIONs
 */

/*
 *
 * get_name function for enum type dnx_field_apptype_flags_e
 * AUTO-GENERATED - DO NOT MODIFY
 */
const char *
dnx_field_apptype_flags_e_get_name(dnx_field_apptype_flags_e value);


/*
 * Global Variables
 */

extern dnx_field_apptype_sw_cbs dnx_field_apptype_sw;

#endif /* __DNX_FIELD_APPTYPE_ACCESS_H__ */
