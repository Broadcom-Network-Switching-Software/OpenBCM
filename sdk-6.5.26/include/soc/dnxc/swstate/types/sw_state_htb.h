/* \file sw_state_htb.h
 *
 * DNX hash table create APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_HTB_RB_H_
#define INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_HTB_RB_H_

#include <soc/dnxc/swstate/sw_state_features.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <shared/bitop.h>
#include <include/soc/dnxc/swstate/types/sw_state_htb_create.h>

typedef shr_error_e(
    *sw_state_htb_traverse_cb) (
    int unit,
    void *key,
    void *data);

typedef struct sw_state_htb_sandbox_s
{
    SHR_BITDCL *key;

    SHR_BITDCL *encoded_key;
    SHR_BITDCL *data;
} sw_state_htb_sandbox_t;

typedef struct sw_state_htb_s
{

    uint32 node_id;

    SHR_BITDCL *metadata;

    uint32 key_size;

    uint32 data_size;

    uint32 psl_size;

    uint32 metadata_size;

    uint32 max_nof_elements;

    uint32 nof_used_elements;

    uint32 load_factor;

    sw_state_htb_hash_func_e hash_func;

    uint32 flags;

    char print_cb_name[SW_STATE_CB_DB_NAME_STR_SIZE];

    struct sw_state_htb_s reverse_map;

    sw_state_htb_sandbox_t sandbox;
} sw_state_htb_t;

shr_error_e sw_state_htb_create(
    int unit,
    sw_state_htb_create_info_t * create_info);

shr_error_e sw_state_htb_insert(
    int unit,
    sw_state_htb_t * htb,
    uint32 table_idx,
    void *const key,
    void *const data);

shr_error_e sw_state_htb_find(
    int unit,
    sw_state_htb_t * htb,
    uint32 table_idx,
    void *const key,
    void *data,
    uint32 *offset);

shr_error_e sw_state_htb_delete(
    int unit,
    sw_state_htb_t * htb,
    uint32 table_idx,
    void *const key);

shr_error_e sw_state_htb_traverse(
    int unit,
    sw_state_htb_t * htb,
    sw_state_htb_traverse_cb traverse_func);

shr_error_e sw_state_htb_replace(
    int unit,
    sw_state_htb_t * htb,
    uint32 table_idx,
    void *const key,
    void *const data);

shr_error_e sw_state_htb_find_reverse(
    int unit,
    sw_state_htb_t * htb,
    uint32 table_idx,
    void *const data,
    void *key);

shr_error_e sw_state_htb_print(
    int unit,
    sw_state_htb_t * htb);

#endif
