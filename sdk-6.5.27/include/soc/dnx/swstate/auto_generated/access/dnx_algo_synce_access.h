
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_SYNCE_ACCESS_H__
#define __DNX_ALGO_SYNCE_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_synce_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>


typedef int (*dnx_algo_synce_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_algo_synce_db_init_cb)(
    int unit);

typedef int (*dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set_cb)(
    int unit, int master_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_get_cb)(
    int unit, int *master_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set_cb)(
    int unit, int slave_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_get_cb)(
    int unit, int *slave_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_master_set1_synce_enabled_set_cb)(
    int unit, int master_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_master_set1_synce_enabled_get_cb)(
    int unit, int *master_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_slave_set1_synce_enabled_set_cb)(
    int unit, int slave_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_fabric_synce_status_slave_set1_synce_enabled_get_cb)(
    int unit, int *slave_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_master_synce_enabled_set_cb)(
    int unit, int master_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_master_synce_enabled_get_cb)(
    int unit, int *master_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_slave_synce_enabled_set_cb)(
    int unit, int slave_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_slave_synce_enabled_get_cb)(
    int unit, int *slave_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_master_set1_synce_enabled_set_cb)(
    int unit, int master_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_master_set1_synce_enabled_get_cb)(
    int unit, int *master_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_slave_set1_synce_enabled_set_cb)(
    int unit, int slave_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_nif_synce_status_slave_set1_synce_enabled_get_cb)(
    int unit, int *slave_set1_synce_enabled);

typedef int (*dnx_algo_synce_db_synce_type_core_group_id_set_cb)(
    int unit, uint32 synce_type_core_group_id_idx_0, uint32 synce_type_core_group_id);

typedef int (*dnx_algo_synce_db_synce_type_core_group_id_get_cb)(
    int unit, uint32 synce_type_core_group_id_idx_0, uint32 *synce_type_core_group_id);

typedef int (*dnx_algo_synce_db_synce_type_core_group_id_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);



typedef struct {
    dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set_cb set;
    dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_get_cb get;
} dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set_cb set;
    dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_get_cb get;
} dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_fabric_synce_status_master_set1_synce_enabled_set_cb set;
    dnx_algo_synce_db_fabric_synce_status_master_set1_synce_enabled_get_cb get;
} dnx_algo_synce_db_fabric_synce_status_master_set1_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_fabric_synce_status_slave_set1_synce_enabled_set_cb set;
    dnx_algo_synce_db_fabric_synce_status_slave_set1_synce_enabled_get_cb get;
} dnx_algo_synce_db_fabric_synce_status_slave_set1_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_cbs master_synce_enabled;
    dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_cbs slave_synce_enabled;
    dnx_algo_synce_db_fabric_synce_status_master_set1_synce_enabled_cbs master_set1_synce_enabled;
    dnx_algo_synce_db_fabric_synce_status_slave_set1_synce_enabled_cbs slave_set1_synce_enabled;
} dnx_algo_synce_db_fabric_synce_status_cbs;

typedef struct {
    dnx_algo_synce_db_nif_synce_status_master_synce_enabled_set_cb set;
    dnx_algo_synce_db_nif_synce_status_master_synce_enabled_get_cb get;
} dnx_algo_synce_db_nif_synce_status_master_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_nif_synce_status_slave_synce_enabled_set_cb set;
    dnx_algo_synce_db_nif_synce_status_slave_synce_enabled_get_cb get;
} dnx_algo_synce_db_nif_synce_status_slave_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_nif_synce_status_master_set1_synce_enabled_set_cb set;
    dnx_algo_synce_db_nif_synce_status_master_set1_synce_enabled_get_cb get;
} dnx_algo_synce_db_nif_synce_status_master_set1_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_nif_synce_status_slave_set1_synce_enabled_set_cb set;
    dnx_algo_synce_db_nif_synce_status_slave_set1_synce_enabled_get_cb get;
} dnx_algo_synce_db_nif_synce_status_slave_set1_synce_enabled_cbs;

typedef struct {
    dnx_algo_synce_db_nif_synce_status_master_synce_enabled_cbs master_synce_enabled;
    dnx_algo_synce_db_nif_synce_status_slave_synce_enabled_cbs slave_synce_enabled;
    dnx_algo_synce_db_nif_synce_status_master_set1_synce_enabled_cbs master_set1_synce_enabled;
    dnx_algo_synce_db_nif_synce_status_slave_set1_synce_enabled_cbs slave_set1_synce_enabled;
} dnx_algo_synce_db_nif_synce_status_cbs;

typedef struct {
    dnx_algo_synce_db_synce_type_core_group_id_set_cb set;
    dnx_algo_synce_db_synce_type_core_group_id_get_cb get;
    dnx_algo_synce_db_synce_type_core_group_id_alloc_cb alloc;
} dnx_algo_synce_db_synce_type_core_group_id_cbs;

typedef struct {
    dnx_algo_synce_db_is_init_cb is_init;
    dnx_algo_synce_db_init_cb init;
    dnx_algo_synce_db_fabric_synce_status_cbs fabric_synce_status;
    dnx_algo_synce_db_nif_synce_status_cbs nif_synce_status;
    dnx_algo_synce_db_synce_type_core_group_id_cbs synce_type_core_group_id;
} dnx_algo_synce_db_cbs;





extern dnx_algo_synce_db_cbs dnx_algo_synce_db;

#endif 
