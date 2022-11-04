/* \file sw_state_htb_create.h
 *
 * DNX hash table create APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifndef INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_HTB_CREATE_H_
#define INCLUDE_SOC_DNXC_SWSTATE_TYPES_SW_STATE_HTB_CREATE_H_

#define SW_STATE_HTB_CREATE_INFO_EFFICIENT_TRAVERSE     SAL_BIT(0)

#define SW_STATE_HTB_CREATE_INFO_MULTI_TABLE            SAL_BIT(1)

#define SW_STATE_HTB_CREATE_INFO_REVERSE_MAP            SAL_BIT(2)

#define SW_STATE_HTB_CREATE_INFO_MEAN_SEARCH            SAL_BIT(3)

#define SW_STATE_HTB_CREATE_INFO_CACHE_HASH_VALUE       SAL_BIT(4)

#define SW_STATE_HTB_CREATE_INFO_DATA_OPTIMIZATION      SAL_BIT(5)

#define SW_STATE_HTB_CREATE_INFO_MAX_SMALL_HASH_PERFORMANCE 10000

typedef enum
{
    SWSTATE_HTB_DEFAULT_HASH = 0,
    SWSTATE_HTB_SMALL_HASH,
    SWSTATE_HTB_LARGE_HASH,
    SWSTATE_HTB_LARGE_HASH_UINT32,
    SWSTATE_HTB_SMALL_HASH_UINT32,

    SWSTATE_HTB_HASH_FUNCS_COUNT
} sw_state_htb_hash_func_e;

typedef struct
{

    uint32 max_nof_elements;

    uint8 load_factor;

    uint32 key_size;

    uint32 data_size;

    uint32 flags;

    uint8 nof_htbs;

    sw_state_htb_hash_func_e hash_func;
    char *print_cb_name;
} sw_state_htb_create_info_t;

#endif
