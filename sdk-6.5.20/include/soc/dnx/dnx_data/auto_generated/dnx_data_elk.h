

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_ELK_H_

#define _DNX_DATA_ELK_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_elk_init(
    int unit);







typedef enum
{
    
    dnx_data_elk_general_is_egw_support_rr,
    
    dnx_data_elk_general_is_kbp_traffic_supported,

    
    _dnx_data_elk_general_feature_nof
} dnx_data_elk_general_feature_e;



typedef int(
    *dnx_data_elk_general_feature_get_f) (
    int unit,
    dnx_data_elk_general_feature_e feature);


typedef uint32(
    *dnx_data_elk_general_max_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_rop_fifo_depth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_flp_full_treshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_max_lane_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_egw_max_opcode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_max_kbp_instance_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_lookup_timeout_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_trans_timer_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_general_device_type_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_elk_general_feature_get_f feature_get;
    
    dnx_data_elk_general_max_ports_get_f max_ports_get;
    
    dnx_data_elk_general_rop_fifo_depth_get_f rop_fifo_depth_get;
    
    dnx_data_elk_general_flp_full_treshold_get_f flp_full_treshold_get;
    
    dnx_data_elk_general_max_lane_id_get_f max_lane_id_get;
    
    dnx_data_elk_general_egw_max_opcode_get_f egw_max_opcode_get;
    
    dnx_data_elk_general_max_kbp_instance_get_f max_kbp_instance_get;
    
    dnx_data_elk_general_lookup_timeout_delay_get_f lookup_timeout_delay_get;
    
    dnx_data_elk_general_trans_timer_delay_get_f trans_timer_delay_get;
    
    dnx_data_elk_general_device_type_get_f device_type_get;
} dnx_data_if_elk_general_t;






typedef struct
{
    
    int initial_regular_capacity;
    
    int initial_optimized_capacity;
    
    int xl_db;
    
    int locality;
    
    int zero_size_ad;
    
    int dbal_none_direct_optimized;
} dnx_data_elk_application_db_property_t;



typedef enum
{
    
    dnx_data_elk_application_ipv4,
    
    dnx_data_elk_application_ipv6,
    
    dnx_data_elk_application_nat,
    
    dnx_data_elk_application_sllb,

    
    _dnx_data_elk_application_feature_nof
} dnx_data_elk_application_feature_e;



typedef int(
    *dnx_data_elk_application_feature_get_f) (
    int unit,
    dnx_data_elk_application_feature_e feature);


typedef uint32(
    *dnx_data_elk_application_optimized_result_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_split_rpf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_thread_safety_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_ipv4_large_mc_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_ipv6_large_mc_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_ipv4_mc_hash_table_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_ipv6_mc_hash_table_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_ipv4_mc_table_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_application_ipv6_mc_table_size_get_f) (
    int unit);


typedef const dnx_data_elk_application_db_property_t *(
    *dnx_data_elk_application_db_property_get_f) (
    int unit,
    int db);



typedef struct
{
    
    dnx_data_elk_application_feature_get_f feature_get;
    
    dnx_data_elk_application_optimized_result_get_f optimized_result_get;
    
    dnx_data_elk_application_split_rpf_get_f split_rpf_get;
    
    dnx_data_elk_application_thread_safety_get_f thread_safety_get;
    
    dnx_data_elk_application_ipv4_large_mc_enable_get_f ipv4_large_mc_enable_get;
    
    dnx_data_elk_application_ipv6_large_mc_enable_get_f ipv6_large_mc_enable_get;
    
    dnx_data_elk_application_ipv4_mc_hash_table_size_get_f ipv4_mc_hash_table_size_get;
    
    dnx_data_elk_application_ipv6_mc_hash_table_size_get_f ipv6_mc_hash_table_size_get;
    
    dnx_data_elk_application_ipv4_mc_table_size_get_f ipv4_mc_table_size_get;
    
    dnx_data_elk_application_ipv6_mc_table_size_get_f ipv6_mc_table_size_get;
    
    dnx_data_elk_application_db_property_get_f db_property_get;
    
    dnxc_data_table_info_get_f db_property_info_get;
} dnx_data_if_elk_application_t;






typedef struct
{
    
    uint32 mdio_id;
} dnx_data_elk_connectivity_mdio_t;


typedef struct
{
    
    uint32 reverse;
} dnx_data_elk_connectivity_ilkn_reverse_t;


typedef struct
{
    
    uint32 start_lane[DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS];
    
    uint32 port_core_mapping[DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS];
} dnx_data_elk_connectivity_topology_t;


typedef struct
{
    
    int pre;
    
    int main;
    
    int post;
    
    int pre2;
    
    int post2;
    
    int post3;
    
    enum kbp_blackhawk_tsc_txfir_tap_enable_enum txfir_tap_enable;
} dnx_data_elk_connectivity_kbp_serdes_tx_taps_t;


typedef struct
{
    
    int pre;
    
    int main;
    
    int post;
    
    int pre2;
    
    int post2;
    
    int post3;
    
    enum kbp_blackhawk_tsc_txfir_tap_enable_enum txfir_tap_enable;
} dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t;



typedef enum
{

    
    _dnx_data_elk_connectivity_feature_nof
} dnx_data_elk_connectivity_feature_e;



typedef int(
    *dnx_data_elk_connectivity_feature_get_f) (
    int unit,
    dnx_data_elk_connectivity_feature_e feature);


typedef uint32(
    *dnx_data_elk_connectivity_connect_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_connectivity_blackhole_get_f) (
    int unit);


typedef const dnx_data_elk_connectivity_mdio_t *(
    *dnx_data_elk_connectivity_mdio_get_f) (
    int unit,
    int kbp_inst);


typedef const dnx_data_elk_connectivity_ilkn_reverse_t *(
    *dnx_data_elk_connectivity_ilkn_reverse_get_f) (
    int unit,
    int ilkn_id);


typedef const dnx_data_elk_connectivity_topology_t *(
    *dnx_data_elk_connectivity_topology_get_f) (
    int unit);


typedef const dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *(
    *dnx_data_elk_connectivity_kbp_serdes_tx_taps_get_f) (
    int unit,
    int kbp_inst);


typedef const dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *(
    *dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_get_f) (
    int unit,
    int lane_id);



typedef struct
{
    
    dnx_data_elk_connectivity_feature_get_f feature_get;
    
    dnx_data_elk_connectivity_connect_mode_get_f connect_mode_get;
    
    dnx_data_elk_connectivity_blackhole_get_f blackhole_get;
    
    dnx_data_elk_connectivity_mdio_get_f mdio_get;
    
    dnxc_data_table_info_get_f mdio_info_get;
    
    dnx_data_elk_connectivity_ilkn_reverse_get_f ilkn_reverse_get;
    
    dnxc_data_table_info_get_f ilkn_reverse_info_get;
    
    dnx_data_elk_connectivity_topology_get_f topology_get;
    
    dnxc_data_table_info_get_f topology_info_get;
    
    dnx_data_elk_connectivity_kbp_serdes_tx_taps_get_f kbp_serdes_tx_taps_get;
    
    dnxc_data_table_info_get_f kbp_serdes_tx_taps_info_get;
    
    dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_get_f kbp_lane_serdes_tx_taps_get;
    
    dnxc_data_table_info_get_f kbp_lane_serdes_tx_taps_info_get;
} dnx_data_if_elk_connectivity_t;







typedef enum
{

    
    _dnx_data_elk_recovery_feature_nof
} dnx_data_elk_recovery_feature_e;



typedef int(
    *dnx_data_elk_recovery_feature_get_f) (
    int unit,
    dnx_data_elk_recovery_feature_e feature);


typedef uint32(
    *dnx_data_elk_recovery_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_elk_recovery_iteration_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_elk_recovery_feature_get_f feature_get;
    
    dnx_data_elk_recovery_enable_get_f enable_get;
    
    dnx_data_elk_recovery_iteration_get_f iteration_get;
} dnx_data_if_elk_recovery_t;





typedef struct
{
    
    dnx_data_if_elk_general_t general;
    
    dnx_data_if_elk_application_t application;
    
    dnx_data_if_elk_connectivity_t connectivity;
    
    dnx_data_if_elk_recovery_t recovery;
} dnx_data_if_elk_t;




extern dnx_data_if_elk_t dnx_data_elk;


#endif 

