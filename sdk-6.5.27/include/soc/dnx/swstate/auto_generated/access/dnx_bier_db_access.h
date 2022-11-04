
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_BIER_DB_ACCESS_H__
#define __DNX_BIER_DB_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_bier_db_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>


typedef int (*dnx_bier_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_bier_db_init_cb)(
    int unit);

typedef int (*dnx_bier_db_set_size_set_cb)(
    int unit, uint32 set_size_idx_0, int set_size);

typedef int (*dnx_bier_db_set_size_get_cb)(
    int unit, uint32 set_size_idx_0, int *set_size);

typedef int (*dnx_bier_db_ingress_active_set_cb)(
    int unit, uint32 active_idx_0, uint32 active);

typedef int (*dnx_bier_db_ingress_active_get_cb)(
    int unit, uint32 active_idx_0, uint32 *active);

typedef int (*dnx_bier_db_egress_active_set_cb)(
    int unit, uint32 active_idx_0, uint32 active);

typedef int (*dnx_bier_db_egress_active_get_cb)(
    int unit, uint32 active_idx_0, uint32 *active);



typedef struct {
    dnx_bier_db_set_size_set_cb set;
    dnx_bier_db_set_size_get_cb get;
} dnx_bier_db_set_size_cbs;

typedef struct {
    dnx_bier_db_ingress_active_set_cb set;
    dnx_bier_db_ingress_active_get_cb get;
} dnx_bier_db_ingress_active_cbs;

typedef struct {
    dnx_bier_db_ingress_active_cbs active;
} dnx_bier_db_ingress_cbs;

typedef struct {
    dnx_bier_db_egress_active_set_cb set;
    dnx_bier_db_egress_active_get_cb get;
} dnx_bier_db_egress_active_cbs;

typedef struct {
    dnx_bier_db_egress_active_cbs active;
} dnx_bier_db_egress_cbs;

typedef struct {
    dnx_bier_db_is_init_cb is_init;
    dnx_bier_db_init_cb init;
    dnx_bier_db_set_size_cbs set_size;
    dnx_bier_db_ingress_cbs ingress;
    dnx_bier_db_egress_cbs egress;
} dnx_bier_db_cbs;





extern dnx_bier_db_cbs dnx_bier_db;

#endif 
