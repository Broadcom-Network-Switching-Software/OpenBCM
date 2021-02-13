/** \file dnx/swstate/auto_generated/access/kbp_access.h
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

#ifndef __KBP_ACCESS_H__
#define __KBP_ACCESS_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/kbp_hb.h>
#include <soc/kbp/alg_kbp/include/key.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: kbp_sw_state_is_init
 */
typedef int (*kbp_sw_state_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: kbp_sw_state_init
 */
typedef int (*kbp_sw_state_init_cb)(
    int unit);

/**
 * implemented by: kbp_sw_state_fwd_caching_enabled_set
 */
typedef int (*kbp_sw_state_fwd_caching_enabled_set_cb)(
    int unit, int fwd_caching_enabled);

/**
 * implemented by: kbp_sw_state_fwd_caching_enabled_get
 */
typedef int (*kbp_sw_state_fwd_caching_enabled_get_cb)(
    int unit, int *fwd_caching_enabled);

/**
 * implemented by: kbp_sw_state_is_device_locked_set
 */
typedef int (*kbp_sw_state_is_device_locked_set_cb)(
    int unit, int is_device_locked);

/**
 * implemented by: kbp_sw_state_is_device_locked_get
 */
typedef int (*kbp_sw_state_is_device_locked_get_cb)(
    int unit, int *is_device_locked);

/**
 * implemented by: kbp_sw_state_db_handles_info_set
 */
typedef int (*kbp_sw_state_db_handles_info_set_cb)(
    int unit, uint32 db_handles_info_idx_0, CONST kbp_db_handles_t *db_handles_info);

/**
 * implemented by: kbp_sw_state_db_handles_info_get
 */
typedef int (*kbp_sw_state_db_handles_info_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_db_handles_t *db_handles_info);

/**
 * implemented by: kbp_sw_state_db_handles_info_caching_bmp_set
 */
typedef int (*kbp_sw_state_db_handles_info_caching_bmp_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int caching_bmp);

/**
 * implemented by: kbp_sw_state_db_handles_info_caching_bmp_get
 */
typedef int (*kbp_sw_state_db_handles_info_caching_bmp_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *caching_bmp);

/**
 * implemented by: kbp_sw_state_db_handles_info_caching_enabled_set
 */
typedef int (*kbp_sw_state_db_handles_info_caching_enabled_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int caching_enabled);

/**
 * implemented by: kbp_sw_state_db_handles_info_caching_enabled_get
 */
typedef int (*kbp_sw_state_db_handles_info_caching_enabled_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *caching_enabled);

/**
 * implemented by: kbp_sw_state_db_handles_info_db_p_set
 */
typedef int (*kbp_sw_state_db_handles_info_db_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_db_t_p db_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_db_p_get
 */
typedef int (*kbp_sw_state_db_handles_info_db_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_db_t_p *db_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_ad_db_zero_size_p_set
 */
typedef int (*kbp_sw_state_db_handles_info_ad_db_zero_size_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p ad_db_zero_size_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_ad_db_zero_size_p_get
 */
typedef int (*kbp_sw_state_db_handles_info_ad_db_zero_size_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_db_t_p *ad_db_zero_size_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set
 */
typedef int (*kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p ad_entry_zero_size_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get
 */
typedef int (*kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, kbp_ad_entry_t_p *ad_entry_zero_size_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_ad_db_p_set
 */
typedef int (*kbp_sw_state_db_handles_info_ad_db_p_set_cb)(
    int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p ad_db_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_ad_db_p_get
 */
typedef int (*kbp_sw_state_db_handles_info_ad_db_p_get_cb)(
    int unit, uint32 db_handles_info_idx_0, uint32 ad_db_p_idx_0, kbp_ad_db_t_p *ad_db_p);

/**
 * implemented by: kbp_sw_state_db_handles_info_opt_result_size_set
 */
typedef int (*kbp_sw_state_db_handles_info_opt_result_size_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int opt_result_size);

/**
 * implemented by: kbp_sw_state_db_handles_info_opt_result_size_get
 */
typedef int (*kbp_sw_state_db_handles_info_opt_result_size_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *opt_result_size);

/**
 * implemented by: kbp_sw_state_db_handles_info_large_opt_result_size_set
 */
typedef int (*kbp_sw_state_db_handles_info_large_opt_result_size_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int large_opt_result_size);

/**
 * implemented by: kbp_sw_state_db_handles_info_large_opt_result_size_get
 */
typedef int (*kbp_sw_state_db_handles_info_large_opt_result_size_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *large_opt_result_size);

/**
 * implemented by: kbp_sw_state_db_handles_info_cloned_db_id_set
 */
typedef int (*kbp_sw_state_db_handles_info_cloned_db_id_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int cloned_db_id);

/**
 * implemented by: kbp_sw_state_db_handles_info_cloned_db_id_get
 */
typedef int (*kbp_sw_state_db_handles_info_cloned_db_id_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *cloned_db_id);

/**
 * implemented by: kbp_sw_state_db_handles_info_associated_dbal_table_id_set
 */
typedef int (*kbp_sw_state_db_handles_info_associated_dbal_table_id_set_cb)(
    int unit, uint32 db_handles_info_idx_0, int associated_dbal_table_id);

/**
 * implemented by: kbp_sw_state_db_handles_info_associated_dbal_table_id_get
 */
typedef int (*kbp_sw_state_db_handles_info_associated_dbal_table_id_get_cb)(
    int unit, uint32 db_handles_info_idx_0, int *associated_dbal_table_id);

/**
 * implemented by: kbp_sw_state_instruction_info_set
 */
typedef int (*kbp_sw_state_instruction_info_set_cb)(
    int unit, uint32 instruction_info_idx_0, CONST kbp_instruction_handles_t *instruction_info);

/**
 * implemented by: kbp_sw_state_instruction_info_get
 */
typedef int (*kbp_sw_state_instruction_info_get_cb)(
    int unit, uint32 instruction_info_idx_0, kbp_instruction_handles_t *instruction_info);

/**
 * implemented by: kbp_sw_state_instruction_info_inst_p_set
 */
typedef int (*kbp_sw_state_instruction_info_inst_p_set_cb)(
    int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p inst_p);

/**
 * implemented by: kbp_sw_state_instruction_info_inst_p_get
 */
typedef int (*kbp_sw_state_instruction_info_inst_p_get_cb)(
    int unit, uint32 instruction_info_idx_0, uint32 inst_p_idx_0, kbp_instruction_t_p *inst_p);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable fwd_caching_enabled
 */
typedef struct {
    kbp_sw_state_fwd_caching_enabled_set_cb set;
    kbp_sw_state_fwd_caching_enabled_get_cb get;
} kbp_sw_state_fwd_caching_enabled_cbs;

/**
 * This structure holds the access functions for the variable is_device_locked
 */
typedef struct {
    kbp_sw_state_is_device_locked_set_cb set;
    kbp_sw_state_is_device_locked_get_cb get;
} kbp_sw_state_is_device_locked_cbs;

/**
 * This structure holds the access functions for the variable caching_bmp
 */
typedef struct {
    kbp_sw_state_db_handles_info_caching_bmp_set_cb set;
    kbp_sw_state_db_handles_info_caching_bmp_get_cb get;
} kbp_sw_state_db_handles_info_caching_bmp_cbs;

/**
 * This structure holds the access functions for the variable caching_enabled
 */
typedef struct {
    kbp_sw_state_db_handles_info_caching_enabled_set_cb set;
    kbp_sw_state_db_handles_info_caching_enabled_get_cb get;
} kbp_sw_state_db_handles_info_caching_enabled_cbs;

/**
 * This structure holds the access functions for the variable db_p
 */
typedef struct {
    kbp_sw_state_db_handles_info_db_p_set_cb set;
    kbp_sw_state_db_handles_info_db_p_get_cb get;
} kbp_sw_state_db_handles_info_db_p_cbs;

/**
 * This structure holds the access functions for the variable ad_db_zero_size_p
 */
typedef struct {
    kbp_sw_state_db_handles_info_ad_db_zero_size_p_set_cb set;
    kbp_sw_state_db_handles_info_ad_db_zero_size_p_get_cb get;
} kbp_sw_state_db_handles_info_ad_db_zero_size_p_cbs;

/**
 * This structure holds the access functions for the variable ad_entry_zero_size_p
 */
typedef struct {
    kbp_sw_state_db_handles_info_ad_entry_zero_size_p_set_cb set;
    kbp_sw_state_db_handles_info_ad_entry_zero_size_p_get_cb get;
} kbp_sw_state_db_handles_info_ad_entry_zero_size_p_cbs;

/**
 * This structure holds the access functions for the variable ad_db_p
 */
typedef struct {
    kbp_sw_state_db_handles_info_ad_db_p_set_cb set;
    kbp_sw_state_db_handles_info_ad_db_p_get_cb get;
} kbp_sw_state_db_handles_info_ad_db_p_cbs;

/**
 * This structure holds the access functions for the variable opt_result_size
 */
typedef struct {
    kbp_sw_state_db_handles_info_opt_result_size_set_cb set;
    kbp_sw_state_db_handles_info_opt_result_size_get_cb get;
} kbp_sw_state_db_handles_info_opt_result_size_cbs;

/**
 * This structure holds the access functions for the variable large_opt_result_size
 */
typedef struct {
    kbp_sw_state_db_handles_info_large_opt_result_size_set_cb set;
    kbp_sw_state_db_handles_info_large_opt_result_size_get_cb get;
} kbp_sw_state_db_handles_info_large_opt_result_size_cbs;

/**
 * This structure holds the access functions for the variable cloned_db_id
 */
typedef struct {
    kbp_sw_state_db_handles_info_cloned_db_id_set_cb set;
    kbp_sw_state_db_handles_info_cloned_db_id_get_cb get;
} kbp_sw_state_db_handles_info_cloned_db_id_cbs;

/**
 * This structure holds the access functions for the variable associated_dbal_table_id
 */
typedef struct {
    kbp_sw_state_db_handles_info_associated_dbal_table_id_set_cb set;
    kbp_sw_state_db_handles_info_associated_dbal_table_id_get_cb get;
} kbp_sw_state_db_handles_info_associated_dbal_table_id_cbs;

/**
 * This structure holds the access functions for the variable kbp_db_handles_t
 */
typedef struct {
    kbp_sw_state_db_handles_info_set_cb set;
    kbp_sw_state_db_handles_info_get_cb get;
    /**
     * Access struct for caching_bmp
     */
    kbp_sw_state_db_handles_info_caching_bmp_cbs caching_bmp;
    /**
     * Access struct for caching_enabled
     */
    kbp_sw_state_db_handles_info_caching_enabled_cbs caching_enabled;
    /**
     * Access struct for db_p
     */
    kbp_sw_state_db_handles_info_db_p_cbs db_p;
    /**
     * Access struct for ad_db_zero_size_p
     */
    kbp_sw_state_db_handles_info_ad_db_zero_size_p_cbs ad_db_zero_size_p;
    /**
     * Access struct for ad_entry_zero_size_p
     */
    kbp_sw_state_db_handles_info_ad_entry_zero_size_p_cbs ad_entry_zero_size_p;
    /**
     * Access struct for ad_db_p
     */
    kbp_sw_state_db_handles_info_ad_db_p_cbs ad_db_p;
    /**
     * Access struct for opt_result_size
     */
    kbp_sw_state_db_handles_info_opt_result_size_cbs opt_result_size;
    /**
     * Access struct for large_opt_result_size
     */
    kbp_sw_state_db_handles_info_large_opt_result_size_cbs large_opt_result_size;
    /**
     * Access struct for cloned_db_id
     */
    kbp_sw_state_db_handles_info_cloned_db_id_cbs cloned_db_id;
    /**
     * Access struct for associated_dbal_table_id
     */
    kbp_sw_state_db_handles_info_associated_dbal_table_id_cbs associated_dbal_table_id;
} kbp_sw_state_db_handles_info_cbs;

/**
 * This structure holds the access functions for the variable inst_p
 */
typedef struct {
    kbp_sw_state_instruction_info_inst_p_set_cb set;
    kbp_sw_state_instruction_info_inst_p_get_cb get;
} kbp_sw_state_instruction_info_inst_p_cbs;

/**
 * This structure holds the access functions for the variable kbp_instruction_handles_t
 */
typedef struct {
    kbp_sw_state_instruction_info_set_cb set;
    kbp_sw_state_instruction_info_get_cb get;
    /**
     * Access struct for inst_p
     */
    kbp_sw_state_instruction_info_inst_p_cbs inst_p;
} kbp_sw_state_instruction_info_cbs;

/**
 * This structure holds the access functions for the variable kbp_sw_state_t
 */
typedef struct {
    kbp_sw_state_is_init_cb is_init;
    kbp_sw_state_init_cb init;
    /**
     * Access struct for fwd_caching_enabled
     */
    kbp_sw_state_fwd_caching_enabled_cbs fwd_caching_enabled;
    /**
     * Access struct for is_device_locked
     */
    kbp_sw_state_is_device_locked_cbs is_device_locked;
    /**
     * Access struct for db_handles_info
     */
    kbp_sw_state_db_handles_info_cbs db_handles_info;
    /**
     * Access struct for instruction_info
     */
    kbp_sw_state_instruction_info_cbs instruction_info;
} kbp_sw_state_cbs;

/*
 * Global Variables
 */

/*
 * FUNCTIONs
 */

/*
 *
 * get_name function for enum type dnx_kbp_ad_db_index_e
 * AUTO-GENERATED - DO NOT MODIFY
 */
const char *
dnx_kbp_ad_db_index_e_get_name(dnx_kbp_ad_db_index_e value);


/*
 * Global Variables
 */

extern kbp_sw_state_cbs kbp_sw_state;
#endif /* defined(INCLUDE_KBP)*/ 
#endif /* BCM_DNX_SUPPORT*/ 

#endif /* __KBP_ACCESS_H__ */
