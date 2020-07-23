
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __TRUNK_ACCESS_H__
#define __TRUNK_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>



typedef int (*trunk_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*trunk_db_init_cb)(
    int unit);


typedef int (*trunk_db_pools_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*trunk_db_pools_groups_alloc_cb)(
    int unit, uint32 pools_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*trunk_db_pools_groups_psc_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int psc);


typedef int (*trunk_db_pools_groups_psc_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *psc);


typedef int (*trunk_db_pools_groups_psc_profile_id_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int psc_profile_id);


typedef int (*trunk_db_pools_groups_psc_profile_id_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *psc_profile_id);


typedef int (*trunk_db_pools_groups_members_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, CONST trunk_group_member_info_t *members);


typedef int (*trunk_db_pools_groups_members_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, trunk_group_member_info_t *members);


typedef int (*trunk_db_pools_groups_members_range_read_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, trunk_group_member_info_t *dest);


typedef int (*trunk_db_pools_groups_members_range_write_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, const trunk_group_member_info_t *source);


typedef int (*trunk_db_pools_groups_members_range_fill_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, trunk_group_member_info_t value);


typedef int (*trunk_db_pools_groups_members_alloc_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*trunk_db_pools_groups_members_system_port_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 system_port);


typedef int (*trunk_db_pools_groups_members_system_port_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *system_port);


typedef int (*trunk_db_pools_groups_members_flags_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 flags);


typedef int (*trunk_db_pools_groups_members_flags_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, uint32 *flags);


typedef int (*trunk_db_pools_groups_members_index_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int index);


typedef int (*trunk_db_pools_groups_members_index_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 members_idx_0, int *index);


typedef int (*trunk_db_pools_groups_nof_members_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int nof_members);


typedef int (*trunk_db_pools_groups_nof_members_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *nof_members);


typedef int (*trunk_db_pools_groups_last_member_added_index_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int last_member_added_index);


typedef int (*trunk_db_pools_groups_last_member_added_index_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *last_member_added_index);


typedef int (*trunk_db_pools_groups_in_use_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int in_use);


typedef int (*trunk_db_pools_groups_in_use_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *in_use);


typedef int (*trunk_db_pools_groups_pp_ports_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 pp_ports_idx_0, uint32 pp_ports);


typedef int (*trunk_db_pools_groups_pp_ports_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 pp_ports_idx_0, uint32 *pp_ports);


typedef int (*trunk_db_pools_groups_pp_ports_range_read_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, uint32 *dest);


typedef int (*trunk_db_pools_groups_pp_ports_range_write_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_elements, const uint32 *source);


typedef int (*trunk_db_pools_groups_pp_ports_range_fill_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 from_idx, uint32 nof_elements, uint32 value);


typedef int (*trunk_db_pools_groups_pp_ports_alloc_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*trunk_db_pools_groups_in_header_type_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 in_header_type);


typedef int (*trunk_db_pools_groups_in_header_type_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *in_header_type);


typedef int (*trunk_db_pools_groups_out_header_type_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 out_header_type);


typedef int (*trunk_db_pools_groups_out_header_type_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *out_header_type);


typedef int (*trunk_db_pools_groups_flags_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 flags);


typedef int (*trunk_db_pools_groups_flags_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, uint32 *flags);


typedef int (*trunk_db_pools_groups_master_trunk_id_set_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int master_trunk_id);


typedef int (*trunk_db_pools_groups_master_trunk_id_get_cb)(
    int unit, uint32 pools_idx_0, uint32 groups_idx_0, int *master_trunk_id);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_set_cb)(
    int unit, uint32 trunk_system_port_entries_idx_0, CONST trunk_system_port_entry_t *trunk_system_port_entries);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_get_cb)(
    int unit, uint32 trunk_system_port_entries_idx_0, trunk_system_port_entry_t *trunk_system_port_entries);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read_cb)(
    int unit, uint32 nof_elements, trunk_system_port_entry_t *dest);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write_cb)(
    int unit, uint32 nof_elements, const trunk_system_port_entry_t *source);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_range_fill_cb)(
    int unit, uint32 from_idx, uint32 nof_elements, trunk_system_port_entry_t value);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc_cb)(
    int unit);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set_cb)(
    int unit, uint32 trunk_system_port_entries_idx_0, int trunk_id);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_get_cb)(
    int unit, uint32 trunk_system_port_entries_idx_0, int *trunk_id);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set_cb)(
    int unit, uint32 trunk_system_port_entries_idx_0, uint32 flags);


typedef int (*trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_get_cb)(
    int unit, uint32 trunk_system_port_entries_idx_0, uint32 *flags);




typedef struct {
    trunk_db_pools_groups_psc_set_cb set;
    trunk_db_pools_groups_psc_get_cb get;
} trunk_db_pools_groups_psc_cbs;


typedef struct {
    trunk_db_pools_groups_psc_profile_id_set_cb set;
    trunk_db_pools_groups_psc_profile_id_get_cb get;
} trunk_db_pools_groups_psc_profile_id_cbs;


typedef struct {
    trunk_db_pools_groups_members_system_port_set_cb set;
    trunk_db_pools_groups_members_system_port_get_cb get;
} trunk_db_pools_groups_members_system_port_cbs;


typedef struct {
    trunk_db_pools_groups_members_flags_set_cb set;
    trunk_db_pools_groups_members_flags_get_cb get;
} trunk_db_pools_groups_members_flags_cbs;


typedef struct {
    trunk_db_pools_groups_members_index_set_cb set;
    trunk_db_pools_groups_members_index_get_cb get;
} trunk_db_pools_groups_members_index_cbs;


typedef struct {
    trunk_db_pools_groups_members_set_cb set;
    trunk_db_pools_groups_members_get_cb get;
    trunk_db_pools_groups_members_range_read_cb range_read;
    trunk_db_pools_groups_members_range_write_cb range_write;
    trunk_db_pools_groups_members_range_fill_cb range_fill;
    trunk_db_pools_groups_members_alloc_cb alloc;
    
    trunk_db_pools_groups_members_system_port_cbs system_port;
    
    trunk_db_pools_groups_members_flags_cbs flags;
    
    trunk_db_pools_groups_members_index_cbs index;
} trunk_db_pools_groups_members_cbs;


typedef struct {
    trunk_db_pools_groups_nof_members_set_cb set;
    trunk_db_pools_groups_nof_members_get_cb get;
} trunk_db_pools_groups_nof_members_cbs;


typedef struct {
    trunk_db_pools_groups_last_member_added_index_set_cb set;
    trunk_db_pools_groups_last_member_added_index_get_cb get;
} trunk_db_pools_groups_last_member_added_index_cbs;


typedef struct {
    trunk_db_pools_groups_in_use_set_cb set;
    trunk_db_pools_groups_in_use_get_cb get;
} trunk_db_pools_groups_in_use_cbs;


typedef struct {
    trunk_db_pools_groups_pp_ports_set_cb set;
    trunk_db_pools_groups_pp_ports_get_cb get;
    trunk_db_pools_groups_pp_ports_range_read_cb range_read;
    trunk_db_pools_groups_pp_ports_range_write_cb range_write;
    trunk_db_pools_groups_pp_ports_range_fill_cb range_fill;
    trunk_db_pools_groups_pp_ports_alloc_cb alloc;
} trunk_db_pools_groups_pp_ports_cbs;


typedef struct {
    trunk_db_pools_groups_in_header_type_set_cb set;
    trunk_db_pools_groups_in_header_type_get_cb get;
} trunk_db_pools_groups_in_header_type_cbs;


typedef struct {
    trunk_db_pools_groups_out_header_type_set_cb set;
    trunk_db_pools_groups_out_header_type_get_cb get;
} trunk_db_pools_groups_out_header_type_cbs;


typedef struct {
    trunk_db_pools_groups_flags_set_cb set;
    trunk_db_pools_groups_flags_get_cb get;
} trunk_db_pools_groups_flags_cbs;


typedef struct {
    trunk_db_pools_groups_master_trunk_id_set_cb set;
    trunk_db_pools_groups_master_trunk_id_get_cb get;
} trunk_db_pools_groups_master_trunk_id_cbs;


typedef struct {
    trunk_db_pools_groups_alloc_cb alloc;
    
    trunk_db_pools_groups_psc_cbs psc;
    
    trunk_db_pools_groups_psc_profile_id_cbs psc_profile_id;
    
    trunk_db_pools_groups_members_cbs members;
    
    trunk_db_pools_groups_nof_members_cbs nof_members;
    
    trunk_db_pools_groups_last_member_added_index_cbs last_member_added_index;
    
    trunk_db_pools_groups_in_use_cbs in_use;
    
    trunk_db_pools_groups_pp_ports_cbs pp_ports;
    
    trunk_db_pools_groups_in_header_type_cbs in_header_type;
    
    trunk_db_pools_groups_out_header_type_cbs out_header_type;
    
    trunk_db_pools_groups_flags_cbs flags;
    
    trunk_db_pools_groups_master_trunk_id_cbs master_trunk_id;
} trunk_db_pools_groups_cbs;


typedef struct {
    trunk_db_pools_alloc_cb alloc;
    
    trunk_db_pools_groups_cbs groups;
} trunk_db_pools_cbs;


typedef struct {
    trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_set_cb set;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_get_cb get;
} trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_cbs;


typedef struct {
    trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_set_cb set;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_get_cb get;
} trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_cbs;


typedef struct {
    trunk_db_trunk_system_port_db_trunk_system_port_entries_set_cb set;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_get_cb get;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_range_read_cb range_read;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_range_write_cb range_write;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_range_fill_cb range_fill;
    trunk_db_trunk_system_port_db_trunk_system_port_entries_alloc_cb alloc;
    
    trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_cbs trunk_id;
    
    trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_cbs flags;
} trunk_db_trunk_system_port_db_trunk_system_port_entries_cbs;


typedef struct {
    
    trunk_db_trunk_system_port_db_trunk_system_port_entries_cbs trunk_system_port_entries;
} trunk_db_trunk_system_port_db_cbs;


typedef struct {
    trunk_db_is_init_cb is_init;
    trunk_db_init_cb init;
    
    trunk_db_pools_cbs pools;
    
    trunk_db_trunk_system_port_db_cbs trunk_system_port_db;
} trunk_db_cbs;





extern trunk_db_cbs trunk_db;

#endif 
