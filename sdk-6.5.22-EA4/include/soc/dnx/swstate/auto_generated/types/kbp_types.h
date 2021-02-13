/** \file dnx/swstate/auto_generated/types/kbp_types.h
 *
 * sw state types (structs/enums/typedefs)
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

#ifndef __KBP_TYPES_H__
#define __KBP_TYPES_H__

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/kbp_hb.h>
#include <soc/kbp/alg_kbp/include/key.h>
/*
 * MACROs
 */

/**
 * Replace with appropriate shared define
 */
#define DNX_KBP_NOF_DBS 256

/**
 * Only a single search instruction in KBP
 */
#define DNX_KBP_NOF_INSTRUCTIONS 128

/**
 * Indication for using optimized AD DB results
 */
#define DNX_KBP_USE_OPTIMIZED_RESULT dnx_data_elk.application.optimized_result_get(unit)

/**
 * The number of OP2 threads
 */
#define DNX_KBP_NOF_SMTS 2

/*
 * TYPEDEFs
 */

typedef struct kbp_db * kbp_db_t_p;

typedef struct kbp_ad_db * kbp_ad_db_t_p;

typedef struct kbp_hb_db * kbp_hb_db_t_p;

typedef struct kbp_instruction * kbp_instruction_t_p;

typedef struct kbp_key * kbp_key_t_p;

typedef struct kbp_ad * kbp_ad_entry_t_p;

/*
 * ENUMs
 */

/**
 * Represents the inices for regular and optimized KBP AD DBs. Optimized index should always be used with OPTIMIZED_RESULT_TYPE indication.
 */
typedef enum {
    /**
     * Index of the regular AD DB
     */
    DNX_KBP_AD_DB_INDEX_REGULAR = 0,
    /**
     * Index of the optimized AD DB
     */
    DNX_KBP_AD_DB_INDEX_OPTIMIZED = 1,
    /**
     * Index of the large optimized AD DB
     */
    DNX_KBP_AD_DB_INDEX_LARGE_OPTIMIZED = 2,
    DNX_KBP_NOF_AD_DB_INDEX = 3
} dnx_kbp_ad_db_index_e;

/*
 * STRUCTs
 */

/**
 * A struct used to hold the KBP instruction handle
 */
typedef struct {
    kbp_instruction_t_p inst_p[DNX_KBP_NOF_SMTS];
} kbp_instruction_handles_t;

/**
 * A struct used to hold the KBP database (DB) and associated data (AD) database handles
 */
typedef struct {
    int caching_bmp;
    /**
     * Indication showing that caching is currently enabled or disabled for the DB.
     */
    int caching_enabled;
    kbp_db_t_p db_p;
    kbp_ad_db_t_p ad_db_zero_size_p;
    kbp_ad_entry_t_p ad_entry_zero_size_p;
    kbp_ad_db_t_p ad_db_p[DNX_KBP_NOF_AD_DB_INDEX];
    int opt_result_size;
    int large_opt_result_size;
    int cloned_db_id;
    int associated_dbal_table_id;
} kbp_db_handles_t;

/**
 * KBP related sw state
 */
typedef struct {
    /**
     * Global indication showing that caching is enabled or disabled for all forwarding tables
     */
    int fwd_caching_enabled;
    int is_device_locked;
    kbp_db_handles_t db_handles_info[DNX_KBP_NOF_DBS];
    kbp_instruction_handles_t instruction_info[DNX_KBP_NOF_INSTRUCTIONS];
} kbp_sw_state_t;

#endif /* defined(INCLUDE_KBP)*/ 
#endif /* BCM_DNX_SUPPORT*/ 

#endif /* __KBP_TYPES_H__ */
