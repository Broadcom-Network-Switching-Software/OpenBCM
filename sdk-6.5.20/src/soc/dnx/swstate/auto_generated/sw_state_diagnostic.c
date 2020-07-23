
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

typedef int make_iso_compilers_happy_swstatednx_sw_state_diagnostic;

#include <src/soc/dnx/swstate/auto_generated/diagnostic/adapter_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/algo_lif_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dbal_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_field_action_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_lane_map_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_synce_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_bier_db_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_cosq_ingress_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_dram_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_ecgm_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_egr_db_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_fabric_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_actions_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_apptype_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_context_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_group_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_qualifier_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_range_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_profile_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_bank_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_cache_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_hit_indication_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_location_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_manager_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_prefix_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_fifodma_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_hard_reset_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_ipq_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_iqs_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_ire_packet_generator_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_ofp_rate_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_port_imb_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_rx_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_sch_config_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_scheduler_diagnostic.c>
#ifdef BCM_DNX_SUPPORT
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_stk_sys_diagnostic.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_time_sw_state_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_traps_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/dnx_visibility_diagnostic.c>
#if defined(INCLUDE_CTEST)
#include <src/soc/dnx/swstate/auto_generated/diagnostic/example_diagnostic.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/diagnostic/flush_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/instru_counters_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/interrupt_diagnostic.c>
#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/diagnostic/kbp_common_diagnostic.c>
#endif  
#endif  
#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/diagnostic/kbp_diagnostic.c>
#endif  
#endif  
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/diagnostic/kbp_fwd_tcam_access_mapper_diagnostic.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/diagnostic/l2_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/mdb_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/mdb_em_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/mdb_kaps_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/oam_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/reflector_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/srv6_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/stif_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/switch_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/tdm_diagnostic.c>
#include <src/soc/dnx/swstate/auto_generated/diagnostic/trunk_diagnostic.c>
