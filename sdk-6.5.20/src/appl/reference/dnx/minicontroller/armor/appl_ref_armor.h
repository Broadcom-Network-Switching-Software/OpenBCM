/** \file appl_ref_oam_appl_init.h
 * General OAM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_MINICON_ARMOR_H_INCLUDED
/*
 * {
 */
#define APPL_REF_MINICON_ARMOR_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_debug.h>

#define MINICON_ARMOR_NAME_STR "ARMOR"

shr_error_e minicon_armor_init(
    int unit,
    bcm_field_context_t tcams_context_id,
    char *config_file);
int minicon_armor_is_initialized(
    int unit);
shr_error_e armor_convert_to_dbal_readable(
    int unit,
    char *in_file,
    char *out_file);
shr_error_e minicon_armor_load_config(
    int unit,
    char *filename);
int minicon_armor_get_num_of_tables(
    int unit);

char *minicon_armor_get_table_name_by_index(
    int unit,
    uint32 index);

shr_error_e minicon_armor_counter_get(
    int unit,
    uint32 core_id,
    uint32 counter_id,
    uint64 *val);
/*
 * }
 */
#endif /* APPL_REF_MINICON_ARMOR_H_INCLUDED */
