

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ELK_H_

#define _DNX_DATA_INTERNAL_ELK_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_elk_submodule_general,
    dnx_data_elk_submodule_application,
    dnx_data_elk_submodule_connectivity,
    dnx_data_elk_submodule_recovery,

    
    _dnx_data_elk_submodule_nof
} dnx_data_elk_submodule_e;








int dnx_data_elk_general_feature_get(
    int unit,
    dnx_data_elk_general_feature_e feature);



typedef enum
{
    dnx_data_elk_general_define_max_ports,
    dnx_data_elk_general_define_rop_fifo_depth,
    dnx_data_elk_general_define_flp_full_treshold,
    dnx_data_elk_general_define_max_lane_id,
    dnx_data_elk_general_define_egw_max_opcode,
    dnx_data_elk_general_define_max_kbp_instance,
    dnx_data_elk_general_define_lookup_timeout_delay,
    dnx_data_elk_general_define_trans_timer_delay,
    dnx_data_elk_general_define_device_type,

    
    _dnx_data_elk_general_define_nof
} dnx_data_elk_general_define_e;



uint32 dnx_data_elk_general_max_ports_get(
    int unit);


uint32 dnx_data_elk_general_rop_fifo_depth_get(
    int unit);


uint32 dnx_data_elk_general_flp_full_treshold_get(
    int unit);


uint32 dnx_data_elk_general_max_lane_id_get(
    int unit);


uint32 dnx_data_elk_general_egw_max_opcode_get(
    int unit);


uint32 dnx_data_elk_general_max_kbp_instance_get(
    int unit);


uint32 dnx_data_elk_general_lookup_timeout_delay_get(
    int unit);


uint32 dnx_data_elk_general_trans_timer_delay_get(
    int unit);


uint32 dnx_data_elk_general_device_type_get(
    int unit);



typedef enum
{

    
    _dnx_data_elk_general_table_nof
} dnx_data_elk_general_table_e;









int dnx_data_elk_application_feature_get(
    int unit,
    dnx_data_elk_application_feature_e feature);



typedef enum
{
    dnx_data_elk_application_define_optimized_result,
    dnx_data_elk_application_define_split_rpf,
    dnx_data_elk_application_define_thread_safety,
    dnx_data_elk_application_define_ipv4_large_mc_enable,
    dnx_data_elk_application_define_ipv6_large_mc_enable,
    dnx_data_elk_application_define_ipv4_mc_hash_table_size,
    dnx_data_elk_application_define_ipv6_mc_hash_table_size,
    dnx_data_elk_application_define_ipv4_mc_table_size,
    dnx_data_elk_application_define_ipv6_mc_table_size,

    
    _dnx_data_elk_application_define_nof
} dnx_data_elk_application_define_e;



uint32 dnx_data_elk_application_optimized_result_get(
    int unit);


uint32 dnx_data_elk_application_split_rpf_get(
    int unit);


uint32 dnx_data_elk_application_thread_safety_get(
    int unit);


uint32 dnx_data_elk_application_ipv4_large_mc_enable_get(
    int unit);


uint32 dnx_data_elk_application_ipv6_large_mc_enable_get(
    int unit);


uint32 dnx_data_elk_application_ipv4_mc_hash_table_size_get(
    int unit);


uint32 dnx_data_elk_application_ipv6_mc_hash_table_size_get(
    int unit);


uint32 dnx_data_elk_application_ipv4_mc_table_size_get(
    int unit);


uint32 dnx_data_elk_application_ipv6_mc_table_size_get(
    int unit);



typedef enum
{
    dnx_data_elk_application_table_db_property,

    
    _dnx_data_elk_application_table_nof
} dnx_data_elk_application_table_e;



const dnx_data_elk_application_db_property_t * dnx_data_elk_application_db_property_get(
    int unit,
    int db);



shr_error_e dnx_data_elk_application_db_property_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_elk_application_db_property_info_get(
    int unit);






int dnx_data_elk_connectivity_feature_get(
    int unit,
    dnx_data_elk_connectivity_feature_e feature);



typedef enum
{
    dnx_data_elk_connectivity_define_connect_mode,
    dnx_data_elk_connectivity_define_blackhole,

    
    _dnx_data_elk_connectivity_define_nof
} dnx_data_elk_connectivity_define_e;



uint32 dnx_data_elk_connectivity_connect_mode_get(
    int unit);


uint32 dnx_data_elk_connectivity_blackhole_get(
    int unit);



typedef enum
{
    dnx_data_elk_connectivity_table_mdio,
    dnx_data_elk_connectivity_table_ilkn_reverse,
    dnx_data_elk_connectivity_table_topology,
    dnx_data_elk_connectivity_table_kbp_serdes_tx_taps,
    dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps,

    
    _dnx_data_elk_connectivity_table_nof
} dnx_data_elk_connectivity_table_e;



const dnx_data_elk_connectivity_mdio_t * dnx_data_elk_connectivity_mdio_get(
    int unit,
    int kbp_inst);


const dnx_data_elk_connectivity_ilkn_reverse_t * dnx_data_elk_connectivity_ilkn_reverse_get(
    int unit,
    int ilkn_id);


const dnx_data_elk_connectivity_topology_t * dnx_data_elk_connectivity_topology_get(
    int unit);


const dnx_data_elk_connectivity_kbp_serdes_tx_taps_t * dnx_data_elk_connectivity_kbp_serdes_tx_taps_get(
    int unit,
    int kbp_inst);


const dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t * dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_get(
    int unit,
    int lane_id);



shr_error_e dnx_data_elk_connectivity_mdio_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_elk_connectivity_ilkn_reverse_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_elk_connectivity_topology_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_elk_connectivity_kbp_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_elk_connectivity_mdio_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_elk_connectivity_ilkn_reverse_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_elk_connectivity_topology_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_elk_connectivity_kbp_serdes_tx_taps_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_info_get(
    int unit);






int dnx_data_elk_recovery_feature_get(
    int unit,
    dnx_data_elk_recovery_feature_e feature);



typedef enum
{
    dnx_data_elk_recovery_define_enable,
    dnx_data_elk_recovery_define_iteration,

    
    _dnx_data_elk_recovery_define_nof
} dnx_data_elk_recovery_define_e;



uint32 dnx_data_elk_recovery_enable_get(
    int unit);


uint32 dnx_data_elk_recovery_iteration_get(
    int unit);



typedef enum
{

    
    _dnx_data_elk_recovery_table_nof
} dnx_data_elk_recovery_table_e;






shr_error_e dnx_data_elk_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

