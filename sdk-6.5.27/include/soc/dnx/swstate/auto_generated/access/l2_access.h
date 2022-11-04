
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __L2_ACCESS_H__
#define __L2_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/l2_types.h>
#include <bcm/switch.h>


typedef int (*l2_db_context_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*l2_db_context_init_cb)(
    int unit);

typedef int (*l2_db_context_fid_profiles_changed_set_cb)(
    int unit, uint8 fid_profiles_changed);

typedef int (*l2_db_context_fid_profiles_changed_get_cb)(
    int unit, uint8 *fid_profiles_changed);

typedef int (*l2_db_context_routing_learning_set_cb)(
    int unit, uint32 routing_learning);

typedef int (*l2_db_context_routing_learning_get_cb)(
    int unit, uint32 *routing_learning);



typedef struct {
    l2_db_context_fid_profiles_changed_set_cb set;
    l2_db_context_fid_profiles_changed_get_cb get;
} l2_db_context_fid_profiles_changed_cbs;

typedef struct {
    l2_db_context_routing_learning_set_cb set;
    l2_db_context_routing_learning_get_cb get;
} l2_db_context_routing_learning_cbs;

typedef struct {
    l2_db_context_is_init_cb is_init;
    l2_db_context_init_cb init;
    l2_db_context_fid_profiles_changed_cbs fid_profiles_changed;
    l2_db_context_routing_learning_cbs routing_learning;
} l2_db_context_cbs;





extern l2_db_context_cbs l2_db_context;

#endif 
