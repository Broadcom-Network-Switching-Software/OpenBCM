
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __FLOW_ACCESS_H__
#define __FLOW_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <include/bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_flow.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>


typedef int (*flow_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*flow_db_init_cb)(
    int unit);

typedef int (*flow_db_flow_application_info_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*flow_db_flow_application_info_is_verify_disabled_set_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 is_verify_disabled);

typedef int (*flow_db_flow_application_info_is_verify_disabled_get_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 *is_verify_disabled);

typedef int (*flow_db_flow_application_info_is_error_recovery_disabled_set_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 is_error_recovery_disabled);

typedef int (*flow_db_flow_application_info_is_error_recovery_disabled_get_cb)(
    int unit, uint32 flow_application_info_idx_0, uint8 *is_error_recovery_disabled);

typedef int (*flow_db_valid_phases_per_dbal_table_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);

typedef int (*flow_db_valid_phases_per_dbal_table_find_cb)(
    int unit, const dbal_tables_e *key, uint32 *value, uint8 *found);

typedef int (*flow_db_valid_phases_per_dbal_table_insert_cb)(
    int unit, const dbal_tables_e *key, const uint32 *value, uint8 *success);

typedef int (*flow_db_valid_phases_per_dbal_table_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const dbal_tables_e *key, const uint32 *value);

typedef int (*flow_db_valid_phases_per_dbal_table_clear_cb)(
    int unit);

typedef int (*flow_db_valid_phases_per_dbal_table_delete_cb)(
    int unit, const dbal_tables_e *key);

typedef int (*flow_db_dbal_table_to_valid_result_types_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);

typedef int (*flow_db_dbal_table_to_valid_result_types_find_cb)(
    int unit, const flow_app_encap_info_t *key, uint32 *value, uint8 *found);

typedef int (*flow_db_dbal_table_to_valid_result_types_insert_cb)(
    int unit, const flow_app_encap_info_t *key, const uint32 *value, uint8 *success);

typedef int (*flow_db_dbal_table_to_valid_result_types_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const flow_app_encap_info_t *key, const uint32 *value);

typedef int (*flow_db_dbal_table_to_valid_result_types_clear_cb)(
    int unit);

typedef int (*flow_db_dbal_table_to_valid_result_types_delete_cb)(
    int unit, const flow_app_encap_info_t *key);

typedef int (*flow_db_ingress_gport_to_match_info_htb_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);

typedef int (*flow_db_ingress_gport_to_match_info_htb_find_cb)(
    int unit, const int *key, ingress_gport_sw_info_t *value, uint8 *found);

typedef int (*flow_db_ingress_gport_to_match_info_htb_insert_cb)(
    int unit, const int *key, const ingress_gport_sw_info_t *value, uint8 *success);

typedef int (*flow_db_ingress_gport_to_match_info_htb_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const ingress_gport_sw_info_t *value);

typedef int (*flow_db_ingress_gport_to_match_info_htb_clear_cb)(
    int unit);

typedef int (*flow_db_ingress_gport_to_match_info_htb_delete_cb)(
    int unit, const int *key);



typedef struct {
    flow_db_flow_application_info_is_verify_disabled_set_cb set;
    flow_db_flow_application_info_is_verify_disabled_get_cb get;
} flow_db_flow_application_info_is_verify_disabled_cbs;

typedef struct {
    flow_db_flow_application_info_is_error_recovery_disabled_set_cb set;
    flow_db_flow_application_info_is_error_recovery_disabled_get_cb get;
} flow_db_flow_application_info_is_error_recovery_disabled_cbs;

typedef struct {
    flow_db_flow_application_info_alloc_cb alloc;
    flow_db_flow_application_info_is_verify_disabled_cbs is_verify_disabled;
    flow_db_flow_application_info_is_error_recovery_disabled_cbs is_error_recovery_disabled;
} flow_db_flow_application_info_cbs;

typedef struct {
    flow_db_valid_phases_per_dbal_table_create_cb create;
    flow_db_valid_phases_per_dbal_table_find_cb find;
    flow_db_valid_phases_per_dbal_table_insert_cb insert;
    flow_db_valid_phases_per_dbal_table_get_next_cb get_next;
    flow_db_valid_phases_per_dbal_table_clear_cb clear;
    flow_db_valid_phases_per_dbal_table_delete_cb delete;
} flow_db_valid_phases_per_dbal_table_cbs;

typedef struct {
    flow_db_dbal_table_to_valid_result_types_create_cb create;
    flow_db_dbal_table_to_valid_result_types_find_cb find;
    flow_db_dbal_table_to_valid_result_types_insert_cb insert;
    flow_db_dbal_table_to_valid_result_types_get_next_cb get_next;
    flow_db_dbal_table_to_valid_result_types_clear_cb clear;
    flow_db_dbal_table_to_valid_result_types_delete_cb delete;
} flow_db_dbal_table_to_valid_result_types_cbs;

typedef struct {
    flow_db_ingress_gport_to_match_info_htb_create_cb create;
    flow_db_ingress_gport_to_match_info_htb_find_cb find;
    flow_db_ingress_gport_to_match_info_htb_insert_cb insert;
    flow_db_ingress_gport_to_match_info_htb_get_next_cb get_next;
    flow_db_ingress_gport_to_match_info_htb_clear_cb clear;
    flow_db_ingress_gport_to_match_info_htb_delete_cb delete;
} flow_db_ingress_gport_to_match_info_htb_cbs;

typedef struct {
    flow_db_is_init_cb is_init;
    flow_db_init_cb init;
    flow_db_flow_application_info_cbs flow_application_info;
    flow_db_valid_phases_per_dbal_table_cbs valid_phases_per_dbal_table;
    flow_db_dbal_table_to_valid_result_types_cbs dbal_table_to_valid_result_types;
    flow_db_ingress_gport_to_match_info_htb_cbs ingress_gport_to_match_info_htb;
} flow_db_cbs;





extern flow_db_cbs flow_db;

#endif 
