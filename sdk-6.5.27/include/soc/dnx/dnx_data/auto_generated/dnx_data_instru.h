
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INSTRU_H_

#define _DNX_DATA_INSTRU_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_instru.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_instru_init(
    int unit);






typedef struct
{
    bcm_instru_ipt_node_type_t type;
} dnx_data_instru_ipt_profile_info_t;


typedef struct
{
    uint32 bit;
    uint32 size;
} dnx_data_instru_ipt_metadata_t;



typedef enum
{
    dnx_data_instru_ipt_advanced_ipt,
    dnx_data_instru_ipt_ipt_mapping_v2,
    dnx_data_instru_ipt_estimated_encap_size,

    
    _dnx_data_instru_ipt_feature_nof
} dnx_data_instru_ipt_feature_e;



typedef int(
    *dnx_data_instru_ipt_feature_get_f) (
    int unit,
    dnx_data_instru_ipt_feature_e feature);


typedef uint32(
    *dnx_data_instru_ipt_profile_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ipt_metadata_bitmap_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ipt_metadata_edit_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ipt_node_id_padding_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ipt_telemetry_queue_occ_check_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ipt_telemetry_queue_occ_max_get_f) (
    int unit);


typedef const dnx_data_instru_ipt_profile_info_t *(
    *dnx_data_instru_ipt_profile_info_get_f) (
    int unit,
    int profile_id);


typedef const dnx_data_instru_ipt_metadata_t *(
    *dnx_data_instru_ipt_metadata_get_f) (
    int unit,
    int flag_id);



typedef struct
{
    
    dnx_data_instru_ipt_feature_get_f feature_get;
    
    dnx_data_instru_ipt_profile_size_get_f profile_size_get;
    
    dnx_data_instru_ipt_metadata_bitmap_size_get_f metadata_bitmap_size_get;
    
    dnx_data_instru_ipt_metadata_edit_size_get_f metadata_edit_size_get;
    
    dnx_data_instru_ipt_node_id_padding_size_get_f node_id_padding_size_get;
    
    dnx_data_instru_ipt_telemetry_queue_occ_check_get_f telemetry_queue_occ_check_get;
    
    dnx_data_instru_ipt_telemetry_queue_occ_max_get_f telemetry_queue_occ_max_get;
    
    dnx_data_instru_ipt_profile_info_get_f profile_info_get;
    
    dnxc_data_table_info_get_f profile_info_info_get;
    
    dnx_data_instru_ipt_metadata_get_f metadata_get;
    
    dnxc_data_table_info_get_f metadata_info_get;
} dnx_data_if_instru_ipt_t;







typedef enum
{
    dnx_data_instru_sflow_hard_logic_supported,

    
    _dnx_data_instru_sflow_feature_nof
} dnx_data_instru_sflow_feature_e;



typedef int(
    *dnx_data_instru_sflow_feature_get_f) (
    int unit,
    dnx_data_instru_sflow_feature_e feature);


typedef uint32(
    *dnx_data_instru_sflow_max_nof_sflow_encaps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_sflow_feature_get_f feature_get;
    
    dnx_data_instru_sflow_max_nof_sflow_encaps_get_f max_nof_sflow_encaps_get;
    
    dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_get_f nof_sflow_raw_entries_per_stack_get;
} dnx_data_if_instru_sflow_t;







typedef enum
{

    
    _dnx_data_instru_ifa_feature_nof
} dnx_data_instru_ifa_feature_e;



typedef int(
    *dnx_data_instru_ifa_feature_get_f) (
    int unit,
    dnx_data_instru_ifa_feature_e feature);


typedef uint32(
    *dnx_data_instru_ifa_ingress_tod_feature_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ifa_max_lenght_check_for_ifa2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ifa_ifa2_is_in_hl_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_ifa_ifa2_is_supported_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_ifa_feature_get_f feature_get;
    
    dnx_data_instru_ifa_ingress_tod_feature_get_f ingress_tod_feature_get;
    
    dnx_data_instru_ifa_max_lenght_check_for_ifa2_get_f max_lenght_check_for_ifa2_get;
    
    dnx_data_instru_ifa_ifa2_is_in_hl_get_f ifa2_is_in_hl_get;
    
    dnx_data_instru_ifa_ifa2_is_supported_get_f ifa2_is_supported_get;
} dnx_data_if_instru_ifa_t;







typedef enum
{

    
    _dnx_data_instru_ipfix_feature_nof
} dnx_data_instru_ipfix_feature_e;



typedef int(
    *dnx_data_instru_ipfix_feature_get_f) (
    int unit,
    dnx_data_instru_ipfix_feature_e feature);


typedef uint32(
    *dnx_data_instru_ipfix_ipfix_v2_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_ipfix_feature_get_f feature_get;
    
    dnx_data_instru_ipfix_ipfix_v2_get_f ipfix_v2_get;
} dnx_data_if_instru_ipfix_t;







typedef enum
{

    
    _dnx_data_instru_hbh_pt_feature_nof
} dnx_data_instru_hbh_pt_feature_e;



typedef int(
    *dnx_data_instru_hbh_pt_feature_get_f) (
    int unit,
    dnx_data_instru_hbh_pt_feature_e feature);


typedef uint32(
    *dnx_data_instru_hbh_pt_Ingress_1588_ToD_LSB_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_hbh_pt_feature_get_f feature_get;
    
    dnx_data_instru_hbh_pt_Ingress_1588_ToD_LSB_size_get_f Ingress_1588_ToD_LSB_size_get;
} dnx_data_if_instru_hbh_pt_t;







typedef enum
{
    dnx_data_instru_eventor_is_supported,
    dnx_data_instru_eventor_is_periodic_events_supported,

    
    _dnx_data_instru_eventor_feature_nof
} dnx_data_instru_eventor_feature_e;



typedef int(
    *dnx_data_instru_eventor_feature_get_f) (
    int unit,
    dnx_data_instru_eventor_feature_e feature);


typedef uint32(
    *dnx_data_instru_eventor_rx_evpck_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_tx_builder_max_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_nof_contexts_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_nof_builders_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_nof_builders_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_nof_sram_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_sram_bank_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_sram_banks_full_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_sram_banks_net_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_builder_max_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_rx_max_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_axi_sram_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_is_extra_words_copy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_is_rx_en_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_tx_builder_limitation_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_builder_max_timeout_ms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_eventor_sram_bank_mask_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_eventor_feature_get_f feature_get;
    
    dnx_data_instru_eventor_rx_evpck_header_size_get_f rx_evpck_header_size_get;
    
    dnx_data_instru_eventor_tx_builder_max_header_size_get_f tx_builder_max_header_size_get;
    
    dnx_data_instru_eventor_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_instru_eventor_nof_contexts_bits_get_f nof_contexts_bits_get;
    
    dnx_data_instru_eventor_nof_builders_get_f nof_builders_get;
    
    dnx_data_instru_eventor_nof_builders_bits_get_f nof_builders_bits_get;
    
    dnx_data_instru_eventor_nof_sram_banks_get_f nof_sram_banks_get;
    
    dnx_data_instru_eventor_sram_bank_bits_get_f sram_bank_bits_get;
    
    dnx_data_instru_eventor_sram_banks_full_size_get_f sram_banks_full_size_get;
    
    dnx_data_instru_eventor_sram_banks_net_size_get_f sram_banks_net_size_get;
    
    dnx_data_instru_eventor_builder_max_buffer_size_get_f builder_max_buffer_size_get;
    
    dnx_data_instru_eventor_rx_max_buffer_size_get_f rx_max_buffer_size_get;
    
    dnx_data_instru_eventor_axi_sram_offset_get_f axi_sram_offset_get;
    
    dnx_data_instru_eventor_is_extra_words_copy_get_f is_extra_words_copy_get;
    
    dnx_data_instru_eventor_is_rx_en_get_f is_rx_en_get;
    
    dnx_data_instru_eventor_tx_builder_limitation_1_get_f tx_builder_limitation_1_get;
    
    dnx_data_instru_eventor_builder_max_timeout_ms_get_f builder_max_timeout_ms_get;
    
    dnx_data_instru_eventor_sram_bank_mask_get_f sram_bank_mask_get;
} dnx_data_if_instru_eventor_t;







typedef enum
{

    
    _dnx_data_instru_synced_triggers_feature_nof
} dnx_data_instru_synced_triggers_feature_e;



typedef int(
    *dnx_data_instru_synced_triggers_feature_get_f) (
    int unit,
    dnx_data_instru_synced_triggers_feature_e feature);


typedef uint32(
    *dnx_data_instru_synced_triggers_synced_trigger_version_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_synced_triggers_feature_get_f feature_get;
    
    dnx_data_instru_synced_triggers_synced_trigger_version_get_f synced_trigger_version_get;
} dnx_data_if_instru_synced_triggers_t;






typedef struct
{
    int valid;
    int bcm_counter_type;
    int counter_index;
    int dbal_field;
    int words_to_bytes_conv_needed;
} dnx_data_instru_synced_counters_counters_info_t;



typedef enum
{
    dnx_data_instru_synced_counters_limited_ports_for_pfc,
    dnx_data_instru_synced_counters_is_implemented,

    
    _dnx_data_instru_synced_counters_feature_nof
} dnx_data_instru_synced_counters_feature_e;



typedef int(
    *dnx_data_instru_synced_counters_feature_get_f) (
    int unit,
    dnx_data_instru_synced_counters_feature_e feature);


typedef uint32(
    *dnx_data_instru_synced_counters_icgm_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nif_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nof_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_period_adjusted_to_ns_through_time_ratio_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_period_adjusting_clk_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_period_adjusting_ns_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_interval_period_size_hw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_max_interval_period_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_cgm_max_nof_intervals_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nif_max_nof_intervals_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nif_write_counter_wrap_around_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_bytes_in_word_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nof_pms_for_pfc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nof_ports_for_pfc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_instru_synced_counters_nof_tcs_per_port_get_f) (
    int unit);


typedef const dnx_data_instru_synced_counters_counters_info_t *(
    *dnx_data_instru_synced_counters_counters_info_get_f) (
    int unit,
    int entry_index,
    int source_type);



typedef struct
{
    
    dnx_data_instru_synced_counters_feature_get_f feature_get;
    
    dnx_data_instru_synced_counters_icgm_type_get_f icgm_type_get;
    
    dnx_data_instru_synced_counters_nif_type_get_f nif_type_get;
    
    dnx_data_instru_synced_counters_nof_types_get_f nof_types_get;
    
    dnx_data_instru_synced_counters_period_adjusted_to_ns_through_time_ratio_get_f period_adjusted_to_ns_through_time_ratio_get;
    
    dnx_data_instru_synced_counters_period_adjusting_clk_bits_get_f period_adjusting_clk_bits_get;
    
    dnx_data_instru_synced_counters_period_adjusting_ns_bits_get_f period_adjusting_ns_bits_get;
    
    dnx_data_instru_synced_counters_interval_period_size_hw_get_f interval_period_size_hw_get;
    
    dnx_data_instru_synced_counters_max_interval_period_size_get_f max_interval_period_size_get;
    
    dnx_data_instru_synced_counters_cgm_max_nof_intervals_get_f cgm_max_nof_intervals_get;
    
    dnx_data_instru_synced_counters_nif_max_nof_intervals_get_f nif_max_nof_intervals_get;
    
    dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_get_f cgm_write_counter_wrap_around_get;
    
    dnx_data_instru_synced_counters_nif_write_counter_wrap_around_get_f nif_write_counter_wrap_around_get;
    
    dnx_data_instru_synced_counters_bytes_in_word_get_f bytes_in_word_get;
    
    dnx_data_instru_synced_counters_nof_pms_for_pfc_get_f nof_pms_for_pfc_get;
    
    dnx_data_instru_synced_counters_nof_ports_for_pfc_get_f nof_ports_for_pfc_get;
    
    dnx_data_instru_synced_counters_nof_tcs_per_port_get_f nof_tcs_per_port_get;
    
    dnx_data_instru_synced_counters_counters_info_get_f counters_info_get;
    
    dnxc_data_table_info_get_f counters_info_info_get;
} dnx_data_if_instru_synced_counters_t;







typedef enum
{
    dnx_data_instru_alternate_marking_L_bit_profile_mapping,

    
    _dnx_data_instru_alternate_marking_feature_nof
} dnx_data_instru_alternate_marking_feature_e;



typedef int(
    *dnx_data_instru_alternate_marking_feature_get_f) (
    int unit,
    dnx_data_instru_alternate_marking_feature_e feature);


typedef uint32(
    *dnx_data_instru_alternate_marking_nof_L_bit_period_profile_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_instru_alternate_marking_feature_get_f feature_get;
    
    dnx_data_instru_alternate_marking_nof_L_bit_period_profile_get_f nof_L_bit_period_profile_get;
} dnx_data_if_instru_alternate_marking_t;





typedef struct
{
    
    dnx_data_if_instru_ipt_t ipt;
    
    dnx_data_if_instru_sflow_t sflow;
    
    dnx_data_if_instru_ifa_t ifa;
    
    dnx_data_if_instru_ipfix_t ipfix;
    
    dnx_data_if_instru_hbh_pt_t hbh_pt;
    
    dnx_data_if_instru_eventor_t eventor;
    
    dnx_data_if_instru_synced_triggers_t synced_triggers;
    
    dnx_data_if_instru_synced_counters_t synced_counters;
    
    dnx_data_if_instru_alternate_marking_t alternate_marking;
} dnx_data_if_instru_t;




extern dnx_data_if_instru_t dnx_data_instru;


#endif 

