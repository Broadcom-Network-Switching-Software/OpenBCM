/** \file diag_dnx_field_last.h
 *
 * Diagnostics procedures, for DNX, for 'last' (last packet) operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
    /** Array to store the KEY IDs, used by FGs. */
    dbal_enum_value_field_field_key_e key_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX];
    /** Array to store the values of the used KEYs, per FG. Used to store 160b per key. */
    uint32 key_values[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX][DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    /** Array to store the values of the results, per FG. Used to store 128b double result. */
    uint32 result_values[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY];
    /** Stores the Entry, which was hit, per FG. */
    uint32 entry_id;
    /** Stores if the entry got hit. */
    uint8 entry_hit;
#if defined(INCLUDE_KBP)
    /** Used to store kbp_info like acl_key_value and quals_offset_on_master_key, per FG. */
    diag_dnx_field_last_kbp_fg_info_t kbp_info;
#endif                          /* defined(INCLUDE_KBP) */
} diag_dnx_field_last_fg_info_t;

/**
 * \brief
 *  Init structure diag_dnx_field_last_fg_info_t
 * \param [in] unit - The unit number.
 * \param [out] last_fg_info_p - pointer to the structure to init.
 */
shr_error_e diag_dnx_field_last_fg_info_t_init(
    int unit,
    diag_dnx_field_last_fg_info_t * last_fg_info_p);

/**
 * \brief
 *   List of shell options for 'last' shell commands (list)
 * \remark
 */
extern sh_sand_cmd_t Sh_dnx_field_last_cmds[];

#endif /* DIAG_DNX_LAST_H_INCLUDED */
