
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

typedef int make_iso_compilers_happy_swstatednx_sw_state_layout;

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <src/soc/dnx/swstate/auto_generated/layout/algo_lif_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dbal_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_algo_field_action_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_algo_lane_map_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_algo_mib_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_algo_synce_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_bier_db_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_cosq_ingress_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_dram_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_ecgm_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_egr_db_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_epfc_sw_state_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_fabric_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_actions_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_apptype_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_context_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_flush_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_group_layout.c>
#ifdef INCLUDE_KBP
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_kbp_layout.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_qualifier_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_range_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_mapper_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_profile_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_bank_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_cache_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_hit_indication_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_location_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_manager_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_prefix_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_fifodma_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_hard_reset_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_ipq_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_iqs_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_ire_packet_generator_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_ofp_rate_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_port_imb_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_port_nif_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_rx_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_sch_config_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_scheduler_layout.c>
#ifdef BCM_DNX_SUPPORT
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_stk_sys_layout.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_time_sw_state_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/dnx_visibility_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/edk_layout.c>
#if defined(INCLUDE_CTEST)
#include <src/soc/dnx/swstate/auto_generated/layout/example_layout.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/layout/flow_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/flush_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/instru_counters_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/interrupt_layout.c>
#if defined(INCLUDE_KBP)
#include <src/soc/dnx/swstate/auto_generated/layout/kbp_fwd_tcam_access_mapper_layout.c>
#endif  
#ifdef INCLUDE_KBP
#include <src/soc/dnx/swstate/auto_generated/layout/kbp_layout.c>
#endif  
#include <src/soc/dnx/swstate/auto_generated/layout/l2_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/mdb_em_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/mdb_kaps_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/mdb_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/oam_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/pp_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/reflector_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/rx_trap_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/sat_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/srv6_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/stif_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/switch_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/tdm_layout.c>
#include <src/soc/dnx/swstate/auto_generated/layout/trunk_layout.c>
