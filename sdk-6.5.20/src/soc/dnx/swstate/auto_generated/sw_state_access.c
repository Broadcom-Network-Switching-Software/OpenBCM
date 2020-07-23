
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

typedef int make_iso_compilers_happy_swstatednx_sw_state_access;

#include <src/soc/dnx/swstate/auto_generated/access/adapter_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/algo_lif_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dbal_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_algo_field_action_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_algo_lane_map_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_algo_synce_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_cosq_ingress_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_dram_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_ecgm_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_egr_db_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_fabric_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_apptype_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_context_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_group_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_range_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_cache_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_field_tcam_prefix_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_fifodma_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_hard_reset_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_ipq_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_iqs_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_ire_packet_generator_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_rx_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.c>
#ifdef BCM_DNX_SUPPORT
#include <src/soc/dnx/swstate/auto_generated/access/dnx_stk_sys_access.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/access/dnx_time_sw_state_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_traps_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/dnx_visibility_access.c>
#if defined(INCLUDE_CTEST)
#include <src/soc/dnx/swstate/auto_generated/access/example_access.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/access/flush_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/instru_counters_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/interrupt_access.c>
#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/access/kbp_access.c>
#endif  
#endif  
#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/access/kbp_common_access.c>
#endif  
#endif  
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/access/kbp_fwd_tcam_access_mapper_access.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/access/l2_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/mdb_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/mdb_em_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/mdb_kaps_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/oam_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/reflector_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/srv6_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/stif_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/switch_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/tdm_access.c>
#include <src/soc/dnx/swstate/auto_generated/access/trunk_access.c>
