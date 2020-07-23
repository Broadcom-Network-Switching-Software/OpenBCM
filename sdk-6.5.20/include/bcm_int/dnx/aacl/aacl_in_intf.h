/*! \file bcm_int/dnx/aacl/aacl_in_intf.h
 *
 * Internal DNX AACL APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _AACL_IN_INTF_H_INCLUDED__

#define _AACL_IN_INTF_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif


#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm/types.h>








#define AACL_MAX_GROUP_ID 3










#define AACL_MAX_NUM_OF_FILEDS 25

typedef enum
{
    AACL_TCAM_COMPRESSED_ENTRY,
    AACL_TCAM_OVERFLOW_ENTRY
} aacl_tcam_entry_type;

typedef enum
{
    AACL_LPM_INDEX1_ENTRY,
    AACL_LPM_INDEX2_ENTRY
} aacl_lpm_entry_type;

typedef shr_error_e(
    *aacl_out_tcam_entry_add) (
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    int priority,
    uint32 *payload,
    int payload_size,
    uint32 *entry_id);

typedef int (
    *aacl_out_tcam_entry_delete) (
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 entry_id);

typedef int (
    *aacl_out_tcam_entry_payload_update) (
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 *payload,
    int payload_size,
    uint32 entry_id);

typedef int (
    *aacl_out_lpm_entry_add) (
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size);

typedef int (
    *aacl_out_lpm_entry_delete) (
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length);

typedef int (
    *aacl_out_lpm_entry_payload_update) (
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size);

typedef struct
{
    aacl_out_tcam_entry_add tcam_entry_add;
    aacl_out_lpm_entry_add lpm_entry_add;
    aacl_out_tcam_entry_delete tcam_entry_delete;
    aacl_out_lpm_entry_delete lpm_entry_delete;
    aacl_out_tcam_entry_payload_update tcam_entry_payload_update;
    aacl_out_lpm_entry_payload_update lpm_entry_payload_update;
} aacl_mngr_out_intf_cb;


typedef struct aacl_out_dbal_info
{
    dbal_tables_e compressed_dbal_id;
    dbal_tables_e overflow_dbal_id;
    dbal_tables_e index1_dbal_id;
    dbal_tables_e index2_dbal_id;
} aacl_out_intf_dbal_table_info_t;

aacl_out_intf_dbal_table_info_t aacl_group_to_dbal_info[BCM_MAX_NUM_UNITS][AACL_MAX_GROUP_ID];






shr_error_e dnx_aacl_init(
    int unit);


shr_error_e dnx_aacl_deinit(
    int unit);


shr_error_e dnx_aacl_is_enabled(
    int unit,
    uint8 *is_enabled);


shr_error_e dnx_aacl_group_create(
    int unit,
    uint32 *group_id,
    char *group_name,
    uint32 flags);


shr_error_e dnx_aacl_group_dbal_tables_connect(
    int unit,
    uint32 group_id,
    dbal_tables_e compressed_dbal_id,
    dbal_tables_e overflow_dbal_id,
    dbal_tables_e index1_dbal_id,
    dbal_tables_e index2_dbal_id);


shr_error_e dnx_aacl_entry_create(
    int unit,
    uint32 group_id,
    uint32 priority,
    uint32 *entry_id);


shr_error_e dnx_aacl_entry_key_field_masked_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 *field_value,
    uint32 *field_mask);


shr_error_e dnx_aacl_entry_key_field_range_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 min_value,
    uint32 max_value);


shr_error_e dnx_aacl_entry_result_field_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 *result_value);


shr_error_e dnx_aacl_entry_install(
    int unit,
    uint32 entry_id);


shr_error_e dnx_aacl_batch_collect(
    int unit,
    uint32 group_id);


shr_error_e dnx_aacl_batch_commit(
    int unit,
    uint32 group_id);

#endif
