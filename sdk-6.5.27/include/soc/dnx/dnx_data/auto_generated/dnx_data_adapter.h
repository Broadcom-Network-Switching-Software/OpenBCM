
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_ADAPTER_H_

#define _DNX_DATA_ADAPTER_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/access.h>
#include <include/soc/dnxc/dnxc_adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_adapter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_adapter_init(
    int unit);







typedef enum
{

    
    _dnx_data_adapter_tx_feature_nof
} dnx_data_adapter_tx_feature_e;



typedef int(
    *dnx_data_adapter_tx_feature_get_f) (
    int unit,
    dnx_data_adapter_tx_feature_e feature);


typedef uint32(
    *dnx_data_adapter_tx_loopback_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_tx_feature_get_f feature_get;
    
    dnx_data_adapter_tx_loopback_enable_get_f loopback_enable_get;
} dnx_data_if_adapter_tx_t;







typedef enum
{

    
    _dnx_data_adapter_rx_feature_nof
} dnx_data_adapter_rx_feature_e;



typedef int(
    *dnx_data_adapter_rx_feature_get_f) (
    int unit,
    dnx_data_adapter_rx_feature_e feature);


typedef uint32(
    *dnx_data_adapter_rx_constant_header_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_rx_feature_get_f feature_get;
    
    dnx_data_adapter_rx_constant_header_size_get_f constant_header_size_get;
} dnx_data_if_adapter_rx_t;






typedef struct
{
    char *Injected_packet_name;
} dnx_data_adapter_general_Injection_t;



typedef enum
{

    
    _dnx_data_adapter_general_feature_nof
} dnx_data_adapter_general_feature_e;



typedef int(
    *dnx_data_adapter_general_feature_get_f) (
    int unit,
    dnx_data_adapter_general_feature_e feature);


typedef uint32(
    *dnx_data_adapter_general_lib_ver_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_oamp_ms_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_sat_ms_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_transaction_type_code_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_transaction_type_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_input_oam_packet_code_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_input_oam_packet_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_input_packet_size_code_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_input_packet_size_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_header_signal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_tag_swap_res_signal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_recycle_command_signal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_recycle_context_signal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_valid_bytes_signal_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_header_signal_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_tag_swap_res_signal_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_recycle_command_signal_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_recycle_context_signal_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_general_tx_packet_valid_bytes_signal_width_get_f) (
    int unit);


typedef const dnx_data_adapter_general_Injection_t *(
    *dnx_data_adapter_general_Injection_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_adapter_general_feature_get_f feature_get;
    
    dnx_data_adapter_general_lib_ver_get_f lib_ver_get;
    
    dnx_data_adapter_general_oamp_ms_id_get_f oamp_ms_id_get;
    
    dnx_data_adapter_general_sat_ms_id_get_f sat_ms_id_get;
    
    dnx_data_adapter_general_transaction_type_code_get_f transaction_type_code_get;
    
    dnx_data_adapter_general_transaction_type_width_get_f transaction_type_width_get;
    
    dnx_data_adapter_general_input_oam_packet_code_get_f input_oam_packet_code_get;
    
    dnx_data_adapter_general_input_oam_packet_width_get_f input_oam_packet_width_get;
    
    dnx_data_adapter_general_input_packet_size_code_get_f input_packet_size_code_get;
    
    dnx_data_adapter_general_input_packet_size_width_get_f input_packet_size_width_get;
    
    dnx_data_adapter_general_tx_packet_header_signal_id_get_f tx_packet_header_signal_id_get;
    
    dnx_data_adapter_general_tx_packet_tag_swap_res_signal_id_get_f tx_packet_tag_swap_res_signal_id_get;
    
    dnx_data_adapter_general_tx_packet_recycle_command_signal_id_get_f tx_packet_recycle_command_signal_id_get;
    
    dnx_data_adapter_general_tx_packet_recycle_context_signal_id_get_f tx_packet_recycle_context_signal_id_get;
    
    dnx_data_adapter_general_tx_packet_valid_bytes_signal_id_get_f tx_packet_valid_bytes_signal_id_get;
    
    dnx_data_adapter_general_tx_packet_header_signal_width_get_f tx_packet_header_signal_width_get;
    
    dnx_data_adapter_general_tx_packet_tag_swap_res_signal_width_get_f tx_packet_tag_swap_res_signal_width_get;
    
    dnx_data_adapter_general_tx_packet_recycle_command_signal_width_get_f tx_packet_recycle_command_signal_width_get;
    
    dnx_data_adapter_general_tx_packet_recycle_context_signal_width_get_f tx_packet_recycle_context_signal_width_get;
    
    dnx_data_adapter_general_tx_packet_valid_bytes_signal_width_get_f tx_packet_valid_bytes_signal_width_get;
    
    dnx_data_adapter_general_Injection_get_f Injection_get;
    
    dnxc_data_table_info_get_f Injection_info_get;
} dnx_data_if_adapter_general_t;






typedef struct
{
    int ignore;
} dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t;


typedef struct
{
    int indication;
} dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t;


typedef struct
{
    int block_type;
    int regmem_id;
    int type;
    int arg;
} dnx_data_adapter_reg_mem_access_special_swap_core_table_t;


typedef struct
{
    soc_mem_t mem;
} dnx_data_adapter_reg_mem_access_clear_on_read_mems_t;


typedef struct
{
    soc_reg_t reg;
} dnx_data_adapter_reg_mem_access_clear_on_read_regs_t;


typedef struct
{
    int regmem;
} dnx_data_adapter_reg_mem_access_clear_on_read_regmems_t;



typedef enum
{

    
    _dnx_data_adapter_reg_mem_access_feature_nof
} dnx_data_adapter_reg_mem_access_feature_e;



typedef int(
    *dnx_data_adapter_reg_mem_access_feature_get_f) (
    int unit,
    dnx_data_adapter_reg_mem_access_feature_e feature);


typedef uint32(
    *dnx_data_adapter_reg_mem_access_do_collect_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_reg_mem_access_cmic_block_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_reg_mem_access_iproc_block_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_get_f) (
    int unit);


typedef const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t *(
    *dnx_data_adapter_reg_mem_access_swap_core_ignore_map_get_f) (
    int unit,
    int block_type);


typedef const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t *(
    *dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_get_f) (
    int unit,
    int block_type);


typedef const dnx_data_adapter_reg_mem_access_special_swap_core_table_t *(
    *dnx_data_adapter_reg_mem_access_special_swap_core_table_get_f) (
    int unit,
    int index);


typedef const dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *(
    *dnx_data_adapter_reg_mem_access_clear_on_read_mems_get_f) (
    int unit,
    int index);


typedef const dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *(
    *dnx_data_adapter_reg_mem_access_clear_on_read_regs_get_f) (
    int unit,
    int index);


typedef const dnx_data_adapter_reg_mem_access_clear_on_read_regmems_t *(
    *dnx_data_adapter_reg_mem_access_clear_on_read_regmems_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_adapter_reg_mem_access_feature_get_f feature_get;
    
    dnx_data_adapter_reg_mem_access_do_collect_enable_get_f do_collect_enable_get;
    
    dnx_data_adapter_reg_mem_access_cmic_block_index_get_f cmic_block_index_get;
    
    dnx_data_adapter_reg_mem_access_iproc_block_index_get_f iproc_block_index_get;
    
    dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_get_f swap_core_index_zero_with_core_index_get;
    
    dnx_data_adapter_reg_mem_access_swap_core_ignore_map_get_f swap_core_ignore_map_get;
    
    dnxc_data_table_info_get_f swap_core_ignore_map_info_get;
    
    dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_get_f swap_core_ignore_map_by_global_block_ids_enum_get;
    
    dnxc_data_table_info_get_f swap_core_ignore_map_by_global_block_ids_enum_info_get;
    
    dnx_data_adapter_reg_mem_access_special_swap_core_table_get_f special_swap_core_table_get;
    
    dnxc_data_table_info_get_f special_swap_core_table_info_get;
    
    dnx_data_adapter_reg_mem_access_clear_on_read_mems_get_f clear_on_read_mems_get;
    
    dnxc_data_table_info_get_f clear_on_read_mems_info_get;
    
    dnx_data_adapter_reg_mem_access_clear_on_read_regs_get_f clear_on_read_regs_get;
    
    dnxc_data_table_info_get_f clear_on_read_regs_info_get;
    
    dnx_data_adapter_reg_mem_access_clear_on_read_regmems_get_f clear_on_read_regmems_get;
    
    dnxc_data_table_info_get_f clear_on_read_regmems_info_get;
} dnx_data_if_adapter_reg_mem_access_t;






typedef struct
{
    int stage;
    int mdb;
    char *interface;
} dnx_data_adapter_mdb_lookup_caller_id_mapping_t;


typedef struct
{
    char *key_signal_name;
    char *signal_name;
    char *from;
    char *to;
} dnx_data_adapter_mdb_debug_signals_dest_mapping_t;



typedef enum
{

    
    _dnx_data_adapter_mdb_feature_nof
} dnx_data_adapter_mdb_feature_e;



typedef int(
    *dnx_data_adapter_mdb_feature_get_f) (
    int unit,
    dnx_data_adapter_mdb_feature_e feature);


typedef const dnx_data_adapter_mdb_lookup_caller_id_mapping_t *(
    *dnx_data_adapter_mdb_lookup_caller_id_mapping_get_f) (
    int unit,
    int caller_id,
    int stage_mdb_key);


typedef const dnx_data_adapter_mdb_debug_signals_dest_mapping_t *(
    *dnx_data_adapter_mdb_debug_signals_dest_mapping_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_adapter_mdb_feature_get_f feature_get;
    
    dnx_data_adapter_mdb_lookup_caller_id_mapping_get_f lookup_caller_id_mapping_get;
    
    dnxc_data_table_info_get_f lookup_caller_id_mapping_info_get;
    
    dnx_data_adapter_mdb_debug_signals_dest_mapping_get_f debug_signals_dest_mapping_get;
    
    dnxc_data_table_info_get_f debug_signals_dest_mapping_info_get;
} dnx_data_if_adapter_mdb_t;





typedef struct
{
    
    dnx_data_if_adapter_tx_t tx;
    
    dnx_data_if_adapter_rx_t rx;
    
    dnx_data_if_adapter_general_t general;
    
    dnx_data_if_adapter_reg_mem_access_t reg_mem_access;
    
    dnx_data_if_adapter_mdb_t mdb;
} dnx_data_if_adapter_t;




extern dnx_data_if_adapter_t dnx_data_adapter;


#endif 

