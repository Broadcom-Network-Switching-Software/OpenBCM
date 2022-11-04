/** \file diag_dnx_field_last.h
 *
 * Diagnostics procedures, for DNX, for 'last' (last packet) operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#ifndef DIAG_DNX_LAST_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_LAST_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include "diag_dnx_field_utils.h"
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * Indicates the size in uint32 of the key stored per field group.
 */
#define DIAG_DNX_LAST_SINGLE_KEY_SIZE_UINT32 (BYTES2WORDS(DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES))
/*
 * Indicates the size in uint32 of the key stored per field group.
 */
#define DIAG_DNX_LAST_FG_KEY_SIZE_UINT32 (DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX * \
                                          (BYTES2WORDS(DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES)))
/*
 * }
 */

#if defined(INCLUDE_KBP)
typedef struct
{
    /** Array to store the values of the used KEYs, per FG. */
    
    uint32 kbp_acl_key_value[DNX_KBP_MAX_ACL_KEY_SIZE_IN_WORDS];
    /** Array to store the values of the used KEYs, per FG. */
    uint32 quals_offset_on_master_key[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
} diag_dnx_field_last_kbp_fg_info_t;
#endif /* defined(INCLUDE_KBP) */

typedef struct
{
    /** Array to store the values of the used key. */
    uint32
        key_values[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX *
                   (BYTES2WORDS(DNX_DATA_MAX_FIELD_DIAG_KEY_SIGNAL_SIZE_IN_BYTES))];
    
    /** Array to store the values of the results, per FG. Used to store 128b double result. */
    uint32 result_values[BITS2WORDS(DNX_DATA_MAX_FIELD_GROUP_PAYLOAD_MAX_SIZE)];
    /** Array to store the values of the results, per FG. starts from the first action. */
    uint32 result_values_zero_aligned[BITS2WORDS(DNX_DATA_MAX_FIELD_GROUP_PAYLOAD_MAX_SIZE)];
    /** Stores if the entry got hit. */
    uint8 entry_hit;
    dnx_field_entry_t entry_info;
    /** Indication if the key was collected*/
    uint8 key_collected;
    /** Indication if the result was collected*/
    uint8 result_collected;
    /** Indication if the aligned result was collected*/
    uint8 result_aligned_collected;
    /** Indication if the hit was collected*/
    uint8 hit_collected;
#if defined(INCLUDE_KBP)
    /** Used to store kbp_info like acl_key_value and quals_offset_on_master_key, per FG. */
    diag_dnx_field_last_kbp_fg_info_t kbp_info;
#endif                          /* defined(INCLUDE_KBP) */
} diag_dnx_field_last_fg_info_t;

/**
 * \brief
 *  Init an array of structures of type diag_dnx_field_last_fg_info_t
 * \param [in] unit - The unit number.
 * \param [out] last_fg_info - pointer to an array of structures to init.
 */
shr_error_e diag_dnx_field_last_fg_info_t_init(
    int unit,
    diag_dnx_field_last_fg_info_t last_fg_info[]);

/**
 * \brief
 *   List of shell options for 'last' shell commands (list)
 * \remark
 */
extern sh_sand_cmd_t Sh_dnx_field_last_cmds[];

#endif /* DIAG_DNX_LAST_H_INCLUDED */
