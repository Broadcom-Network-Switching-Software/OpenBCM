
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_adapter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_consistent_hashing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_consistent_hashing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_cosq_aqm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_cosq_aqm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_aod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_aod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_aod_sizes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_aod_sizes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_mdb_app_db.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_mdb_app_db.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_pemla_soc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_pemla_soc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing_v1.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_egr_queuing_v1.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_graphical.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_gtimer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_gtimer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_gtp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_gtp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_headers.h>
#ifdef INCLUDE_CTEST
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>
#endif  
#ifdef INCLUDE_CTEST
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_infra_tests.h>
#endif  
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_kaps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_kaps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_kleap_stage_info.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_kleap_stage_info.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_latency.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_latency.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_led.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_led.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_linkscan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_linkscan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_macsec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_module_testing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_onesync.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_onesync.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_parser.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_parser.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ppmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ppmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pvt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_regression.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_regression.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_spb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_spb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_techsupport.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_techsupport.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_time.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_time.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tsn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tune.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_vlan.h>




dnx_data_if_access_t dnx_data_access;
dnx_data_if_adapter_t dnx_data_adapter;
dnx_data_if_bfd_t dnx_data_bfd;
dnx_data_if_bier_t dnx_data_bier;
dnx_data_if_consistent_hashing_t dnx_data_consistent_hashing;
dnx_data_if_cosq_aqm_t dnx_data_cosq_aqm;
dnx_data_if_crps_t dnx_data_crps;
dnx_data_if_dbal_t dnx_data_dbal;
dnx_data_if_debug_t dnx_data_debug;
dnx_data_if_dev_init_t dnx_data_dev_init;
dnx_data_if_device_t dnx_data_device;

dnx_data_if_dnx2_aod_t dnx_data_dnx2_aod;


dnx_data_if_dnx2_aod_sizes_t dnx_data_dnx2_aod_sizes;


dnx_data_if_dnx2_arr_t dnx_data_dnx2_arr;


dnx_data_if_dnx2_mdb_app_db_t dnx_data_dnx2_mdb_app_db;


dnx_data_if_dnx2_pemla_soc_t dnx_data_dnx2_pemla_soc;

dnx_data_if_dram_t dnx_data_dram;
dnx_data_if_ecgm_t dnx_data_ecgm;
dnx_data_if_egr_queuing_t dnx_data_egr_queuing;
dnx_data_if_egr_queuing_v1_t dnx_data_egr_queuing_v1;
dnx_data_if_elk_t dnx_data_elk;
dnx_data_if_esb_t dnx_data_esb;
dnx_data_if_esem_t dnx_data_esem;
dnx_data_if_fabric_t dnx_data_fabric;
dnx_data_if_failover_t dnx_data_failover;
dnx_data_if_fc_t dnx_data_fc;
dnx_data_if_field_t dnx_data_field;
dnx_data_if_field_map_t dnx_data_field_map;
dnx_data_if_fifodma_t dnx_data_fifodma;
dnx_data_if_flow_t dnx_data_flow;
dnx_data_if_graphical_t dnx_data_graphical;
dnx_data_if_gtimer_t dnx_data_gtimer;
dnx_data_if_gtp_t dnx_data_gtp;
dnx_data_if_headers_t dnx_data_headers;
#ifdef INCLUDE_CTEST

dnx_data_if_infra_tests_t dnx_data_infra_tests;

#endif 
dnx_data_if_ingr_congestion_t dnx_data_ingr_congestion;
dnx_data_if_ingr_reassembly_t dnx_data_ingr_reassembly;
dnx_data_if_ingress_cs_t dnx_data_ingress_cs;
dnx_data_if_instru_t dnx_data_instru;
dnx_data_if_intr_t dnx_data_intr;
dnx_data_if_ipq_t dnx_data_ipq;
dnx_data_if_iqs_t dnx_data_iqs;
dnx_data_if_kaps_t dnx_data_kaps;
dnx_data_if_kleap_stage_info_t dnx_data_kleap_stage_info;
dnx_data_if_l2_t dnx_data_l2;
dnx_data_if_l3_t dnx_data_l3;
dnx_data_if_lane_map_t dnx_data_lane_map;
dnx_data_if_latency_t dnx_data_latency;
dnx_data_if_led_t dnx_data_led;
dnx_data_if_lif_t dnx_data_lif;
dnx_data_if_linkscan_t dnx_data_linkscan;
dnx_data_if_macsec_t dnx_data_macsec;
dnx_data_if_mdb_t dnx_data_mdb;
dnx_data_if_meter_t dnx_data_meter;
dnx_data_if_mib_t dnx_data_mib;
dnx_data_if_module_testing_t dnx_data_module_testing;
dnx_data_if_mpls_t dnx_data_mpls;
dnx_data_if_multicast_t dnx_data_multicast;
dnx_data_if_nif_t dnx_data_nif;
dnx_data_if_oam_t dnx_data_oam;
dnx_data_if_onesync_t dnx_data_onesync;
dnx_data_if_parser_t dnx_data_parser;
dnx_data_if_pll_t dnx_data_pll;
dnx_data_if_port_t dnx_data_port;
dnx_data_if_port_pp_t dnx_data_port_pp;
dnx_data_if_pp_t dnx_data_pp;
dnx_data_if_ppmc_t dnx_data_ppmc;
dnx_data_if_ptp_t dnx_data_ptp;
dnx_data_if_pvt_t dnx_data_pvt;
dnx_data_if_qos_t dnx_data_qos;
dnx_data_if_regression_t dnx_data_regression;
dnx_data_if_sat_t dnx_data_sat;
dnx_data_if_sbusdma_desc_t dnx_data_sbusdma_desc;
dnx_data_if_sch_t dnx_data_sch;
dnx_data_if_snif_t dnx_data_snif;
dnx_data_if_spb_t dnx_data_spb;
dnx_data_if_srv6_t dnx_data_srv6;
dnx_data_if_stack_t dnx_data_stack;
dnx_data_if_stat_t dnx_data_stat;
dnx_data_if_stg_t dnx_data_stg;
dnx_data_if_stif_t dnx_data_stif;
dnx_data_if_switch_t dnx_data_switch;
dnx_data_if_synce_t dnx_data_synce;
dnx_data_if_system_red_t dnx_data_system_red;
dnx_data_if_tdm_t dnx_data_tdm;
dnx_data_if_techsupport_t dnx_data_techsupport;
dnx_data_if_time_t dnx_data_time;
dnx_data_if_trap_t dnx_data_trap;
dnx_data_if_trunk_t dnx_data_trunk;
dnx_data_if_tsn_t dnx_data_tsn;
dnx_data_if_tune_t dnx_data_tune;
dnx_data_if_tunnel_t dnx_data_tunnel;
dnx_data_if_vlan_t dnx_data_vlan;
static int dnx_data_valid;



shr_error_e
dnx_data_if_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_data_valid != 0)
    {
        SHR_EXIT();
    }

    
    
    
    dnx_data_access.mdio.feature_get = dnx_data_access_mdio_feature_get;

    
    dnx_data_access.mdio.nof_pms_per_ring_get = dnx_data_access_mdio_nof_pms_per_ring_get;
    dnx_data_access.mdio.ring_offset_get = dnx_data_access_mdio_ring_offset_get;
    dnx_data_access.mdio.offset_in_ring_get = dnx_data_access_mdio_offset_in_ring_get;
    dnx_data_access.mdio.ext_divisor_get = dnx_data_access_mdio_ext_divisor_get;
    dnx_data_access.mdio.int_divisor_get = dnx_data_access_mdio_int_divisor_get;
    dnx_data_access.mdio.int_div_out_delay_get = dnx_data_access_mdio_int_div_out_delay_get;
    dnx_data_access.mdio.ext_div_out_delay_get = dnx_data_access_mdio_ext_div_out_delay_get;

    
    
    
    dnx_data_access.rcpu.feature_get = dnx_data_access_rcpu_feature_get;

    

    
    dnx_data_access.rcpu.rx_get = dnx_data_access_rcpu_rx_get;
    dnx_data_access.rcpu.rx_info_get = dnx_data_access_rcpu_rx_info_get;
    
    
    dnx_data_access.blocks.feature_get = dnx_data_access_blocks_feature_get;

    

    
    dnx_data_access.blocks.override_get = dnx_data_access_blocks_override_get;
    dnx_data_access.blocks.override_info_get = dnx_data_access_blocks_override_info_get;
    
    
    
    dnx_data_adapter.tx.feature_get = dnx_data_adapter_tx_feature_get;

    
    dnx_data_adapter.tx.loopback_enable_get = dnx_data_adapter_tx_loopback_enable_get;

    
    
    
    dnx_data_adapter.rx.feature_get = dnx_data_adapter_rx_feature_get;

    
    dnx_data_adapter.rx.constant_header_size_get = dnx_data_adapter_rx_constant_header_size_get;

    
    
    
    dnx_data_adapter.general.feature_get = dnx_data_adapter_general_feature_get;

    
    dnx_data_adapter.general.lib_ver_get = dnx_data_adapter_general_lib_ver_get;
    dnx_data_adapter.general.oamp_ms_id_get = dnx_data_adapter_general_oamp_ms_id_get;
    dnx_data_adapter.general.sat_ms_id_get = dnx_data_adapter_general_sat_ms_id_get;
    dnx_data_adapter.general.transaction_type_code_get = dnx_data_adapter_general_transaction_type_code_get;
    dnx_data_adapter.general.transaction_type_width_get = dnx_data_adapter_general_transaction_type_width_get;
    dnx_data_adapter.general.input_oam_packet_code_get = dnx_data_adapter_general_input_oam_packet_code_get;
    dnx_data_adapter.general.input_oam_packet_width_get = dnx_data_adapter_general_input_oam_packet_width_get;
    dnx_data_adapter.general.input_packet_size_code_get = dnx_data_adapter_general_input_packet_size_code_get;
    dnx_data_adapter.general.input_packet_size_width_get = dnx_data_adapter_general_input_packet_size_width_get;
    dnx_data_adapter.general.tx_packet_header_signal_id_get = dnx_data_adapter_general_tx_packet_header_signal_id_get;
    dnx_data_adapter.general.tx_packet_tag_swap_res_signal_id_get = dnx_data_adapter_general_tx_packet_tag_swap_res_signal_id_get;
    dnx_data_adapter.general.tx_packet_recycle_command_signal_id_get = dnx_data_adapter_general_tx_packet_recycle_command_signal_id_get;
    dnx_data_adapter.general.tx_packet_recycle_context_signal_id_get = dnx_data_adapter_general_tx_packet_recycle_context_signal_id_get;
    dnx_data_adapter.general.tx_packet_valid_bytes_signal_id_get = dnx_data_adapter_general_tx_packet_valid_bytes_signal_id_get;
    dnx_data_adapter.general.tx_packet_header_signal_width_get = dnx_data_adapter_general_tx_packet_header_signal_width_get;
    dnx_data_adapter.general.tx_packet_tag_swap_res_signal_width_get = dnx_data_adapter_general_tx_packet_tag_swap_res_signal_width_get;
    dnx_data_adapter.general.tx_packet_recycle_command_signal_width_get = dnx_data_adapter_general_tx_packet_recycle_command_signal_width_get;
    dnx_data_adapter.general.tx_packet_recycle_context_signal_width_get = dnx_data_adapter_general_tx_packet_recycle_context_signal_width_get;
    dnx_data_adapter.general.tx_packet_valid_bytes_signal_width_get = dnx_data_adapter_general_tx_packet_valid_bytes_signal_width_get;

    
    dnx_data_adapter.general.Injection_get = dnx_data_adapter_general_Injection_get;
    dnx_data_adapter.general.Injection_info_get = dnx_data_adapter_general_Injection_info_get;
    
    
    dnx_data_adapter.reg_mem_access.feature_get = dnx_data_adapter_reg_mem_access_feature_get;

    
    dnx_data_adapter.reg_mem_access.do_collect_enable_get = dnx_data_adapter_reg_mem_access_do_collect_enable_get;
    dnx_data_adapter.reg_mem_access.cmic_block_index_get = dnx_data_adapter_reg_mem_access_cmic_block_index_get;
    dnx_data_adapter.reg_mem_access.iproc_block_index_get = dnx_data_adapter_reg_mem_access_iproc_block_index_get;
    dnx_data_adapter.reg_mem_access.swap_core_index_zero_with_core_index_get = dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_get;

    
    dnx_data_adapter.reg_mem_access.swap_core_ignore_map_get = dnx_data_adapter_reg_mem_access_swap_core_ignore_map_get;
    dnx_data_adapter.reg_mem_access.swap_core_ignore_map_info_get = dnx_data_adapter_reg_mem_access_swap_core_ignore_map_info_get;
    dnx_data_adapter.reg_mem_access.swap_core_ignore_map_by_global_block_ids_enum_get = dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_get;
    dnx_data_adapter.reg_mem_access.swap_core_ignore_map_by_global_block_ids_enum_info_get = dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_info_get;
    dnx_data_adapter.reg_mem_access.special_swap_core_table_get = dnx_data_adapter_reg_mem_access_special_swap_core_table_get;
    dnx_data_adapter.reg_mem_access.special_swap_core_table_info_get = dnx_data_adapter_reg_mem_access_special_swap_core_table_info_get;
    dnx_data_adapter.reg_mem_access.clear_on_read_mems_get = dnx_data_adapter_reg_mem_access_clear_on_read_mems_get;
    dnx_data_adapter.reg_mem_access.clear_on_read_mems_info_get = dnx_data_adapter_reg_mem_access_clear_on_read_mems_info_get;
    dnx_data_adapter.reg_mem_access.clear_on_read_regs_get = dnx_data_adapter_reg_mem_access_clear_on_read_regs_get;
    dnx_data_adapter.reg_mem_access.clear_on_read_regs_info_get = dnx_data_adapter_reg_mem_access_clear_on_read_regs_info_get;
    dnx_data_adapter.reg_mem_access.clear_on_read_regmems_get = dnx_data_adapter_reg_mem_access_clear_on_read_regmems_get;
    dnx_data_adapter.reg_mem_access.clear_on_read_regmems_info_get = dnx_data_adapter_reg_mem_access_clear_on_read_regmems_info_get;
    
    
    dnx_data_adapter.mdb.feature_get = dnx_data_adapter_mdb_feature_get;

    

    
    dnx_data_adapter.mdb.lookup_caller_id_mapping_get = dnx_data_adapter_mdb_lookup_caller_id_mapping_get;
    dnx_data_adapter.mdb.lookup_caller_id_mapping_info_get = dnx_data_adapter_mdb_lookup_caller_id_mapping_info_get;
    dnx_data_adapter.mdb.debug_signals_dest_mapping_get = dnx_data_adapter_mdb_debug_signals_dest_mapping_get;
    dnx_data_adapter.mdb.debug_signals_dest_mapping_info_get = dnx_data_adapter_mdb_debug_signals_dest_mapping_info_get;
    
    
    
    dnx_data_bfd.general.feature_get = dnx_data_bfd_general_feature_get;

    
    dnx_data_bfd.general.nof_bits_bfd_endpoints_get = dnx_data_bfd_general_nof_bits_bfd_endpoints_get;
    dnx_data_bfd.general.nof_endpoints_get = dnx_data_bfd_general_nof_endpoints_get;
    dnx_data_bfd.general.nof_dips_for_bfd_multihop_get = dnx_data_bfd_general_nof_dips_for_bfd_multihop_get;
    dnx_data_bfd.general.nof_bits_dips_for_bfd_multihop_get = dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_get;
    dnx_data_bfd.general.nof_bfd_server_trap_codes_get = dnx_data_bfd_general_nof_bfd_server_trap_codes_get;
    dnx_data_bfd.general.nof_profiles_for_tx_rate_get = dnx_data_bfd_general_nof_profiles_for_tx_rate_get;
    dnx_data_bfd.general.nof_bits_bfd_server_trap_codes_get = dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_get;
    dnx_data_bfd.general.rmep_index_db_ipv4_const_get = dnx_data_bfd_general_rmep_index_db_ipv4_const_get;
    dnx_data_bfd.general.rmep_index_db_mpls_const_get = dnx_data_bfd_general_rmep_index_db_mpls_const_get;
    dnx_data_bfd.general.rmep_index_db_pwe_const_get = dnx_data_bfd_general_rmep_index_db_pwe_const_get;
    dnx_data_bfd.general.nof_req_int_profiles_get = dnx_data_bfd_general_nof_req_int_profiles_get;
    dnx_data_bfd.general.nof_bits_your_discr_to_lif_get = dnx_data_bfd_general_nof_bits_your_discr_to_lif_get;
    dnx_data_bfd.general.oamp_bfd_cw_valid_dis_supp_get = dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_get;
    dnx_data_bfd.general.bfd_over_php_oam_tcam_oam_offset_get = dnx_data_bfd_general_bfd_over_php_oam_tcam_oam_offset_get;
    dnx_data_bfd.general.bfd_echo_support_in_parser_get = dnx_data_bfd_general_bfd_echo_support_in_parser_get;
    dnx_data_bfd.general.bfd_nof_server_device_profile_get = dnx_data_bfd_general_bfd_nof_server_device_profile_get;

    
    
    
    dnx_data_bfd.feature.feature_get = dnx_data_bfd_feature_feature_get;

    

    
    
    
    
    dnx_data_bier.params.feature_get = dnx_data_bier_params_feature_get;

    
    dnx_data_bier.params.bitstring_size_get = dnx_data_bier_params_bitstring_size_get;
    dnx_data_bier.params.bfr_entry_size_get = dnx_data_bier_params_bfr_entry_size_get;
    dnx_data_bier.params.bank_entries_get = dnx_data_bier_params_bank_entries_get;
    dnx_data_bier.params.bundle_nof_banks_get = dnx_data_bier_params_bundle_nof_banks_get;
    dnx_data_bier.params.bundle_nof_banks_net_get = dnx_data_bier_params_bundle_nof_banks_net_get;
    dnx_data_bier.params.nof_egr_table_copies_get = dnx_data_bier_params_nof_egr_table_copies_get;
    dnx_data_bier.params.mcdb_copies_per_entry_get = dnx_data_bier_params_mcdb_copies_per_entry_get;
    dnx_data_bier.params.nof_bfr_entries_get = dnx_data_bier_params_nof_bfr_entries_get;
    dnx_data_bier.params.bier_mcdb_size_get = dnx_data_bier_params_bier_mcdb_size_get;

    
    
    
    
    dnx_data_consistent_hashing.calendar.feature_get = dnx_data_consistent_hashing_calendar_feature_get;

    
    dnx_data_consistent_hashing.calendar.max_nof_entries_in_calendar_get = dnx_data_consistent_hashing_calendar_max_nof_entries_in_calendar_get;

    
    
    
    
    dnx_data_cosq_aqm.general.feature_get = dnx_data_cosq_aqm_general_feature_get;

    
    dnx_data_cosq_aqm.general.profile_nof_get = dnx_data_cosq_aqm_general_profile_nof_get;
    dnx_data_cosq_aqm.general.flows_nof_get = dnx_data_cosq_aqm_general_flows_nof_get;
    dnx_data_cosq_aqm.general.reserved_flows_nof_get = dnx_data_cosq_aqm_general_reserved_flows_nof_get;

    
    
    
    
    dnx_data_crps.engine.feature_get = dnx_data_crps_engine_feature_get;

    
    dnx_data_crps.engine.nof_engines_get = dnx_data_crps_engine_nof_engines_get;
    dnx_data_crps.engine.nof_mid_engines_get = dnx_data_crps_engine_nof_mid_engines_get;
    dnx_data_crps.engine.nof_big_engines_get = dnx_data_crps_engine_nof_big_engines_get;
    dnx_data_crps.engine.nof_databases_get = dnx_data_crps_engine_nof_databases_get;
    dnx_data_crps.engine.data_mapping_table_size_get = dnx_data_crps_engine_data_mapping_table_size_get;
    dnx_data_crps.engine.max_counter_set_size_get = dnx_data_crps_engine_max_counter_set_size_get;
    dnx_data_crps.engine.oam_counter_set_size_get = dnx_data_crps_engine_oam_counter_set_size_get;
    dnx_data_crps.engine.max_nof_counter_sets_get = dnx_data_crps_engine_max_nof_counter_sets_get;
    dnx_data_crps.engine.last_address_nof_bits_get = dnx_data_crps_engine_last_address_nof_bits_get;
    dnx_data_crps.engine.counter_format_nof_bits_get = dnx_data_crps_engine_counter_format_nof_bits_get;
    dnx_data_crps.engine.counter_mode_nof_bits_get = dnx_data_crps_engine_counter_mode_nof_bits_get;
    dnx_data_crps.engine.nof_small_meter_shared_engines_get = dnx_data_crps_engine_nof_small_meter_shared_engines_get;
    dnx_data_crps.engine.nof_mid_meter_shared_engines_get = dnx_data_crps_engine_nof_mid_meter_shared_engines_get;
    dnx_data_crps.engine.nof_big_meter_shared_engines_get = dnx_data_crps_engine_nof_big_meter_shared_engines_get;

    
    dnx_data_crps.engine.engines_info_get = dnx_data_crps_engine_engines_info_get;
    dnx_data_crps.engine.engines_info_info_get = dnx_data_crps_engine_engines_info_info_get;
    dnx_data_crps.engine.source_base_get = dnx_data_crps_engine_source_base_get;
    dnx_data_crps.engine.source_base_info_get = dnx_data_crps_engine_source_base_info_get;
    dnx_data_crps.engine.counter_format_types_get = dnx_data_crps_engine_counter_format_types_get;
    dnx_data_crps.engine.counter_format_types_info_get = dnx_data_crps_engine_counter_format_types_info_get;
    
    
    dnx_data_crps.eviction.feature_get = dnx_data_crps_eviction_feature_get;

    
    dnx_data_crps.eviction.counters_fifo_depth_get = dnx_data_crps_eviction_counters_fifo_depth_get;
    dnx_data_crps.eviction.nof_counters_fifo_get = dnx_data_crps_eviction_nof_counters_fifo_get;
    dnx_data_crps.eviction.dma_record_entry_nof_bits_get = dnx_data_crps_eviction_dma_record_entry_nof_bits_get;
    dnx_data_crps.eviction.bubble_before_request_timeout_get = dnx_data_crps_eviction_bubble_before_request_timeout_get;
    dnx_data_crps.eviction.bubble_after_request_timeout_get = dnx_data_crps_eviction_bubble_after_request_timeout_get;
    dnx_data_crps.eviction.seq_address_nof_bits_get = dnx_data_crps_eviction_seq_address_nof_bits_get;
    dnx_data_crps.eviction.nof_eviction_destination_types_get = dnx_data_crps_eviction_nof_eviction_destination_types_get;
    dnx_data_crps.eviction.nof_crps_network_channels_get = dnx_data_crps_eviction_nof_crps_network_channels_get;
    dnx_data_crps.eviction.bg_thread_enable_get = dnx_data_crps_eviction_bg_thread_enable_get;

    
    dnx_data_crps.eviction.phy_record_format_get = dnx_data_crps_eviction_phy_record_format_get;
    dnx_data_crps.eviction.phy_record_format_info_get = dnx_data_crps_eviction_phy_record_format_info_get;
    dnx_data_crps.eviction.condional_action_valid_get = dnx_data_crps_eviction_condional_action_valid_get;
    dnx_data_crps.eviction.condional_action_valid_info_get = dnx_data_crps_eviction_condional_action_valid_info_get;
    dnx_data_crps.eviction.supported_eviction_destination_get = dnx_data_crps_eviction_supported_eviction_destination_get;
    dnx_data_crps.eviction.supported_eviction_destination_info_get = dnx_data_crps_eviction_supported_eviction_destination_info_get;
    
    
    dnx_data_crps.expansion.feature_get = dnx_data_crps_expansion_feature_get;

    
    dnx_data_crps.expansion.expansion_size_get = dnx_data_crps_expansion_expansion_size_get;

    
    dnx_data_crps.expansion.source_metadata_types_get = dnx_data_crps_expansion_source_metadata_types_get;
    dnx_data_crps.expansion.source_metadata_types_info_get = dnx_data_crps_expansion_source_metadata_types_info_get;
    dnx_data_crps.expansion.source_expansion_info_get = dnx_data_crps_expansion_source_expansion_info_get;
    dnx_data_crps.expansion.source_expansion_info_info_get = dnx_data_crps_expansion_source_expansion_info_info_get;
    
    
    dnx_data_crps.src_interface.feature_get = dnx_data_crps_src_interface_feature_get;

    

    
    dnx_data_crps.src_interface.command_id_get = dnx_data_crps_src_interface_command_id_get;
    dnx_data_crps.src_interface.command_id_info_get = dnx_data_crps_src_interface_command_id_info_get;
    dnx_data_crps.src_interface.crps_eviction_get = dnx_data_crps_src_interface_crps_eviction_get;
    dnx_data_crps.src_interface.crps_eviction_info_get = dnx_data_crps_src_interface_crps_eviction_info_get;
    dnx_data_crps.src_interface.crps_counting_get = dnx_data_crps_src_interface_crps_counting_get;
    dnx_data_crps.src_interface.crps_counting_info_get = dnx_data_crps_src_interface_crps_counting_info_get;
    
    
    dnx_data_crps.latency.feature_get = dnx_data_crps_latency_feature_get;

    
    dnx_data_crps.latency.etpp_command_id_get = dnx_data_crps_latency_etpp_command_id_get;
    dnx_data_crps.latency.epp_command_id_get = dnx_data_crps_latency_epp_command_id_get;
    dnx_data_crps.latency.stat_id_port_offset_get = dnx_data_crps_latency_stat_id_port_offset_get;
    dnx_data_crps.latency.stat_id_tc_offset_get = dnx_data_crps_latency_stat_id_tc_offset_get;
    dnx_data_crps.latency.stat_id_multicast_offset_get = dnx_data_crps_latency_stat_id_multicast_offset_get;

    
    
    
    dnx_data_crps.state_counters.feature_get = dnx_data_crps_state_counters_feature_get;

    
    dnx_data_crps.state_counters.max_wide_state_counters_size_get = dnx_data_crps_state_counters_max_wide_state_counters_size_get;
    dnx_data_crps.state_counters.max_narrow_state_counters_size_get = dnx_data_crps_state_counters_max_narrow_state_counters_size_get;
    dnx_data_crps.state_counters.state_counter_data_size_per_full_counter_get = dnx_data_crps_state_counters_state_counter_data_size_per_full_counter_get;

    
    dnx_data_crps.state_counters.counter_size_per_format_type_get = dnx_data_crps_state_counters_counter_size_per_format_type_get;
    dnx_data_crps.state_counters.counter_size_per_format_type_info_get = dnx_data_crps_state_counters_counter_size_per_format_type_info_get;
    
    
    dnx_data_crps.general.feature_get = dnx_data_crps_general_feature_get;

    

    
    
    
    
    dnx_data_dbal.diag.feature_get = dnx_data_dbal_diag_feature_get;

    

    
    dnx_data_dbal.diag.loggerInfo_get = dnx_data_dbal_diag_loggerInfo_get;
    dnx_data_dbal.diag.loggerInfo_info_get = dnx_data_dbal_diag_loggerInfo_info_get;
    dnx_data_dbal.diag.example_1_key_get = dnx_data_dbal_diag_example_1_key_get;
    dnx_data_dbal.diag.example_1_key_info_get = dnx_data_dbal_diag_example_1_key_info_get;
    dnx_data_dbal.diag.example_2_keys_get = dnx_data_dbal_diag_example_2_keys_get;
    dnx_data_dbal.diag.example_2_keys_info_get = dnx_data_dbal_diag_example_2_keys_info_get;
    
    
    dnx_data_dbal.db_init.feature_get = dnx_data_dbal_db_init_feature_get;

    
    dnx_data_dbal.db_init.dbal_device_state_get = dnx_data_dbal_db_init_dbal_device_state_get;
    dnx_data_dbal.db_init.run_ltt_after_wm_test_get = dnx_data_dbal_db_init_run_ltt_after_wm_test_get;

    
    dnx_data_dbal.db_init.default_std_get = dnx_data_dbal_db_init_default_std_get;
    dnx_data_dbal.db_init.default_std_info_get = dnx_data_dbal_db_init_default_std_info_get;
    
    
    dnx_data_dbal.table.feature_get = dnx_data_dbal_table_feature_get;

    
    dnx_data_dbal.table.nof_dynamic_tables_get = dnx_data_dbal_table_nof_dynamic_tables_get;
    dnx_data_dbal.table.nof_dynamic_xml_tables_get = dnx_data_dbal_table_nof_dynamic_xml_tables_get;
    dnx_data_dbal.table.nof_dynamic_tables_labels_get = dnx_data_dbal_table_nof_dynamic_tables_labels_get;
    dnx_data_dbal.table.nof_dynamic_tables_key_fields_get = dnx_data_dbal_table_nof_dynamic_tables_key_fields_get;
    dnx_data_dbal.table.nof_dynamic_tables_result_fields_get = dnx_data_dbal_table_nof_dynamic_tables_result_fields_get;
    dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get = dnx_data_dbal_table_nof_dynamic_tables_multi_result_types_get;
    dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get = dnx_data_dbal_table_nof_dynamic_tables_ppmc_multi_result_types_get;
    dnx_data_dbal.table.nof_dynamic_and_static_tables_get = dnx_data_dbal_table_nof_dynamic_and_static_tables_get;

    
    
    
    dnx_data_dbal.hw_ent.feature_get = dnx_data_dbal_hw_ent_feature_get;

    
    dnx_data_dbal.hw_ent.nof_direct_maps_get = dnx_data_dbal_hw_ent_nof_direct_maps_get;
    dnx_data_dbal.hw_ent.nof_groups_maps_get = dnx_data_dbal_hw_ent_nof_groups_maps_get;

    
    
    
    
    dnx_data_debug.mem.feature_get = dnx_data_debug_mem_feature_get;

    

    
    dnx_data_debug.mem.params_get = dnx_data_debug_mem_params_get;
    dnx_data_debug.mem.params_info_get = dnx_data_debug_mem_params_info_get;
    dnx_data_debug.mem.block_map_get = dnx_data_debug_mem_block_map_get;
    dnx_data_debug.mem.block_map_info_get = dnx_data_debug_mem_block_map_info_get;
    dnx_data_debug.mem.mem_array_get = dnx_data_debug_mem_mem_array_get;
    dnx_data_debug.mem.mem_array_info_get = dnx_data_debug_mem_mem_array_info_get;
    
    
    dnx_data_debug.feature.feature_get = dnx_data_debug_feature_feature_get;

    

    
    
    
    dnx_data_debug.kleap.feature_get = dnx_data_debug_kleap_feature_get;

    
    dnx_data_debug.kleap.fwd1_gen_data_size_get = dnx_data_debug_kleap_fwd1_gen_data_size_get;
    dnx_data_debug.kleap.fwd2_gen_data_size_get = dnx_data_debug_kleap_fwd2_gen_data_size_get;

    
    
    
    dnx_data_debug.general.feature_get = dnx_data_debug_general_feature_get;

    
    dnx_data_debug.general.global_visibility_get = dnx_data_debug_general_global_visibility_get;
    dnx_data_debug.general.has_etpp_visibility_enabler_get = dnx_data_debug_general_has_etpp_visibility_enabler_get;
    dnx_data_debug.general.has_periodic_visibility_get = dnx_data_debug_general_has_periodic_visibility_get;
    dnx_data_debug.general.etpp_eop_and_sop_dec_above_threshold_get = dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get;
    dnx_data_debug.general.egress_port_mc_visibility_get = dnx_data_debug_general_egress_port_mc_visibility_get;
    dnx_data_debug.general.pmf_debug_signals_get = dnx_data_debug_general_pmf_debug_signals_get;
    dnx_data_debug.general.fer_debug_signals_get = dnx_data_debug_general_fer_debug_signals_get;
    dnx_data_debug.general.ees_data_device_debug_signals_get = dnx_data_debug_general_ees_data_device_debug_signals_get;
    dnx_data_debug.general.lbp_debug_signals_register_based_get = dnx_data_debug_general_lbp_debug_signals_register_based_get;
    dnx_data_debug.general.signal_version_get = dnx_data_debug_general_signal_version_get;
    dnx_data_debug.general.mem_array_index_get = dnx_data_debug_general_mem_array_index_get;

    
    
    
    dnx_data_debug.nif.feature_get = dnx_data_debug_nif_feature_get;

    
    dnx_data_debug.nif.extension_header_get = dnx_data_debug_nif_extension_header_get;

    
    
    
    
    dnx_data_dev_init.time.feature_get = dnx_data_dev_init_time_feature_get;

    
    dnx_data_dev_init.time.analyze_get = dnx_data_dev_init_time_analyze_get;
    dnx_data_dev_init.time.init_total_time_get = dnx_data_dev_init_time_init_total_time_get;
    dnx_data_dev_init.time.appl_init_total_time_get = dnx_data_dev_init_time_appl_init_total_time_get;
    dnx_data_dev_init.time.down_deviation_total_get = dnx_data_dev_init_time_down_deviation_total_get;
    dnx_data_dev_init.time.up_deviation_total_get = dnx_data_dev_init_time_up_deviation_total_get;
    dnx_data_dev_init.time.test_sensitivity_get = dnx_data_dev_init_time_test_sensitivity_get;

    
    dnx_data_dev_init.time.step_time_get = dnx_data_dev_init_time_step_time_get;
    dnx_data_dev_init.time.step_time_info_get = dnx_data_dev_init_time_step_time_info_get;
    dnx_data_dev_init.time.appl_step_time_get = dnx_data_dev_init_time_appl_step_time_get;
    dnx_data_dev_init.time.appl_step_time_info_get = dnx_data_dev_init_time_appl_step_time_info_get;
    
    
    dnx_data_dev_init.mem.feature_get = dnx_data_dev_init_mem_feature_get;

    
    dnx_data_dev_init.mem.force_zeros_get = dnx_data_dev_init_mem_force_zeros_get;
    dnx_data_dev_init.mem.reset_mode_get = dnx_data_dev_init_mem_reset_mode_get;
    dnx_data_dev_init.mem.defaults_verify_get = dnx_data_dev_init_mem_defaults_verify_get;
    dnx_data_dev_init.mem.emul_ext_init_get = dnx_data_dev_init_mem_emul_ext_init_get;

    
    dnx_data_dev_init.mem.default_get = dnx_data_dev_init_mem_default_get;
    dnx_data_dev_init.mem.default_info_get = dnx_data_dev_init_mem_default_info_get;
#ifdef BCM_ACCESS_SUPPORT

    dnx_data_dev_init.mem.sw_mem_init_get = dnx_data_dev_init_mem_sw_mem_init_get;
    dnx_data_dev_init.mem.sw_mem_init_info_get = dnx_data_dev_init_mem_sw_mem_init_info_get;

#endif 
    dnx_data_dev_init.mem.emul_ext_init_path_get = dnx_data_dev_init_mem_emul_ext_init_path_get;
    dnx_data_dev_init.mem.emul_ext_init_path_info_get = dnx_data_dev_init_mem_emul_ext_init_path_info_get;
    
    
    dnx_data_dev_init.properties.feature_get = dnx_data_dev_init_properties_feature_get;

    
    dnx_data_dev_init.properties.name_max_get = dnx_data_dev_init_properties_name_max_get;

    
    dnx_data_dev_init.properties.unsupported_get = dnx_data_dev_init_properties_unsupported_get;
    dnx_data_dev_init.properties.unsupported_info_get = dnx_data_dev_init_properties_unsupported_info_get;
    
    
    dnx_data_dev_init.general.feature_get = dnx_data_dev_init_general_feature_get;

    
    dnx_data_dev_init.general.access_only_get = dnx_data_dev_init_general_access_only_get;
    dnx_data_dev_init.general.heat_up_get = dnx_data_dev_init_general_heat_up_get;
    dnx_data_dev_init.general.flexe_core_drv_select_get = dnx_data_dev_init_general_flexe_core_drv_select_get;

    
    dnx_data_dev_init.general.clock_power_down_get = dnx_data_dev_init_general_clock_power_down_get;
    dnx_data_dev_init.general.clock_power_down_info_get = dnx_data_dev_init_general_clock_power_down_info_get;
    
    
    dnx_data_dev_init.context.feature_get = dnx_data_dev_init_context_feature_get;

    
    dnx_data_dev_init.context.forwarding_context_selection_mask_offset_get = dnx_data_dev_init_context_forwarding_context_selection_mask_offset_get;
    dnx_data_dev_init.context.forwarding_context_selection_result_offset_get = dnx_data_dev_init_context_forwarding_context_selection_result_offset_get;
    dnx_data_dev_init.context.termination_context_selection_mask_offset_get = dnx_data_dev_init_context_termination_context_selection_mask_offset_get;
    dnx_data_dev_init.context.termination_context_selection_result_offset_get = dnx_data_dev_init_context_termination_context_selection_result_offset_get;
    dnx_data_dev_init.context.trap_context_selection_mask_offset_get = dnx_data_dev_init_context_trap_context_selection_mask_offset_get;
    dnx_data_dev_init.context.trap_context_selection_result_offset_get = dnx_data_dev_init_context_trap_context_selection_result_offset_get;
    dnx_data_dev_init.context.prp_pem_context_selection_mask_offset_get = dnx_data_dev_init_context_prp_pem_context_selection_mask_offset_get;
    dnx_data_dev_init.context.prp_context_selection_result_offset_get = dnx_data_dev_init_context_prp_context_selection_result_offset_get;
    dnx_data_dev_init.context.fwd_reycle_priority_size_get = dnx_data_dev_init_context_fwd_reycle_priority_size_get;

    
    
    
    dnx_data_dev_init.ha.feature_get = dnx_data_dev_init_ha_feature_get;

    
    dnx_data_dev_init.ha.warmboot_support_get = dnx_data_dev_init_ha_warmboot_support_get;
    dnx_data_dev_init.ha.sw_state_max_size_get = dnx_data_dev_init_ha_sw_state_max_size_get;
    dnx_data_dev_init.ha.stable_location_get = dnx_data_dev_init_ha_stable_location_get;
    dnx_data_dev_init.ha.stable_size_get = dnx_data_dev_init_ha_stable_size_get;
    dnx_data_dev_init.ha.error_recovery_support_get = dnx_data_dev_init_ha_error_recovery_support_get;

    
    dnx_data_dev_init.ha.stable_filename_get = dnx_data_dev_init_ha_stable_filename_get;
    dnx_data_dev_init.ha.stable_filename_info_get = dnx_data_dev_init_ha_stable_filename_info_get;
    
    
    dnx_data_dev_init.shadow.feature_get = dnx_data_dev_init_shadow_feature_get;

    
    dnx_data_dev_init.shadow.cache_enable_all_get = dnx_data_dev_init_shadow_cache_enable_all_get;
    dnx_data_dev_init.shadow.cache_enable_ecc_get = dnx_data_dev_init_shadow_cache_enable_ecc_get;
    dnx_data_dev_init.shadow.cache_enable_parity_get = dnx_data_dev_init_shadow_cache_enable_parity_get;
    dnx_data_dev_init.shadow.cache_enable_specific_get = dnx_data_dev_init_shadow_cache_enable_specific_get;
    dnx_data_dev_init.shadow.cache_disable_specific_get = dnx_data_dev_init_shadow_cache_disable_specific_get;

    
    dnx_data_dev_init.shadow.uncacheable_mem_get = dnx_data_dev_init_shadow_uncacheable_mem_get;
    dnx_data_dev_init.shadow.uncacheable_mem_info_get = dnx_data_dev_init_shadow_uncacheable_mem_info_get;
#ifdef BCM_ACCESS_SUPPORT

    dnx_data_dev_init.shadow.uncacheable_regmem_get = dnx_data_dev_init_shadow_uncacheable_regmem_get;
    dnx_data_dev_init.shadow.uncacheable_regmem_info_get = dnx_data_dev_init_shadow_uncacheable_regmem_info_get;

#endif 
    
    
    dnx_data_dev_init.cmc.feature_get = dnx_data_dev_init_cmc_feature_get;

    
    dnx_data_dev_init.cmc.cmc_num_get = dnx_data_dev_init_cmc_cmc_num_get;
    dnx_data_dev_init.cmc.num_cpu_cosq_get = dnx_data_dev_init_cmc_num_cpu_cosq_get;
    dnx_data_dev_init.cmc.cmc_pci_get = dnx_data_dev_init_cmc_cmc_pci_get;
    dnx_data_dev_init.cmc.pci_cmc_num_get = dnx_data_dev_init_cmc_pci_cmc_num_get;
    dnx_data_dev_init.cmc.cmc_uc0_get = dnx_data_dev_init_cmc_cmc_uc0_get;
    dnx_data_dev_init.cmc.cmc_uc1_get = dnx_data_dev_init_cmc_cmc_uc1_get;
    dnx_data_dev_init.cmc.num_queues_pci_get = dnx_data_dev_init_cmc_num_queues_pci_get;
    dnx_data_dev_init.cmc.num_queues_uc0_get = dnx_data_dev_init_cmc_num_queues_uc0_get;
    dnx_data_dev_init.cmc.num_queues_uc1_get = dnx_data_dev_init_cmc_num_queues_uc1_get;

    
    
    
    dnx_data_dev_init.rx.feature_get = dnx_data_dev_init_rx_feature_get;

    
    dnx_data_dev_init.rx.rx_pool_nof_pkts_get = dnx_data_dev_init_rx_rx_pool_nof_pkts_get;

    
    
    
    
    dnx_data_device.general.feature_get = dnx_data_device_general_feature_get;

    
    dnx_data_device.general.max_nof_system_ports_get = dnx_data_device_general_max_nof_system_ports_get;
    dnx_data_device.general.invalid_system_port_get = dnx_data_device_general_invalid_system_port_get;
    dnx_data_device.general.fmq_system_port_get = dnx_data_device_general_fmq_system_port_get;
    dnx_data_device.general.invalid_fap_id_get = dnx_data_device_general_invalid_fap_id_get;
    dnx_data_device.general.max_nof_faps_get = dnx_data_device_general_max_nof_faps_get;
    dnx_data_device.general.fap_ids_per_core_bits_get = dnx_data_device_general_fap_ids_per_core_bits_get;
    dnx_data_device.general.nof_cores_get = dnx_data_device_general_nof_cores_get;
    dnx_data_device.general.valid_cores_bitmap_get = dnx_data_device_general_valid_cores_bitmap_get;
    dnx_data_device.general.nof_cores_for_traffic_get = dnx_data_device_general_nof_cores_for_traffic_get;
    dnx_data_device.general.max_nof_layer_protocols_get = dnx_data_device_general_max_nof_layer_protocols_get;
    dnx_data_device.general.nof_ipcs_get = dnx_data_device_general_nof_ipcs_get;
    dnx_data_device.general.nof_core_groups_get = dnx_data_device_general_nof_core_groups_get;
    dnx_data_device.general.nof_cores_per_core_group_get = dnx_data_device_general_nof_cores_per_core_group_get;
    dnx_data_device.general.nof_avs_status_bits_get = dnx_data_device_general_nof_avs_status_bits_get;
    dnx_data_device.general.core_max_nof_bits_get = dnx_data_device_general_core_max_nof_bits_get;
    dnx_data_device.general.max_nof_fap_ids_per_core_get = dnx_data_device_general_max_nof_fap_ids_per_core_get;
    dnx_data_device.general.max_nof_fap_ids_per_device_get = dnx_data_device_general_max_nof_fap_ids_per_device_get;
    dnx_data_device.general.core_clock_khz_get = dnx_data_device_general_core_clock_khz_get;
    dnx_data_device.general.system_ref_core_clock_khz_get = dnx_data_device_general_system_ref_core_clock_khz_get;
    dnx_data_device.general.ref_core_clock_mhz_get = dnx_data_device_general_ref_core_clock_mhz_get;
    dnx_data_device.general.core_clock_ndiv_size_get = dnx_data_device_general_core_clock_ndiv_size_get;
    dnx_data_device.general.core_clock_mdiv_size_get = dnx_data_device_general_core_clock_mdiv_size_get;
    dnx_data_device.general.core_clock_ndiv_offset_get = dnx_data_device_general_core_clock_ndiv_offset_get;
    dnx_data_device.general.core_clock_mdiv_offset_get = dnx_data_device_general_core_clock_mdiv_offset_get;
    dnx_data_device.general.bus_size_in_bits_get = dnx_data_device_general_bus_size_in_bits_get;
    dnx_data_device.general.maximal_core_bandwidth_mbps_get = dnx_data_device_general_maximal_core_bandwidth_mbps_get;
    dnx_data_device.general.enforce_maximal_core_bandwidth_get = dnx_data_device_general_enforce_maximal_core_bandwidth_get;
    dnx_data_device.general.max_mpps_get = dnx_data_device_general_max_mpps_get;
    dnx_data_device.general.packet_per_clock_get = dnx_data_device_general_packet_per_clock_get;
    dnx_data_device.general.nof_faps_get = dnx_data_device_general_nof_faps_get;
    dnx_data_device.general.device_id_get = dnx_data_device_general_device_id_get;
    dnx_data_device.general.nof_sku_bits_get = dnx_data_device_general_nof_sku_bits_get;
    dnx_data_device.general.bist_enable_get = dnx_data_device_general_bist_enable_get;
    dnx_data_device.general.nof_bits_for_nof_cores_get = dnx_data_device_general_nof_bits_for_nof_cores_get;

    
    dnx_data_device.general.ecc_err_masking_get = dnx_data_device_general_ecc_err_masking_get;
    dnx_data_device.general.ecc_err_masking_info_get = dnx_data_device_general_ecc_err_masking_info_get;
    dnx_data_device.general.ctest_full_file_indentifier_get = dnx_data_device_general_ctest_full_file_indentifier_get;
    dnx_data_device.general.ctest_full_file_indentifier_info_get = dnx_data_device_general_ctest_full_file_indentifier_info_get;
    
    
    dnx_data_device.regression.feature_get = dnx_data_device_regression_feature_get;

    

    
    dnx_data_device.regression.regression_parms_get = dnx_data_device_regression_regression_parms_get;
    dnx_data_device.regression.regression_parms_info_get = dnx_data_device_regression_regression_parms_info_get;
    
    
    dnx_data_device.emulation.feature_get = dnx_data_device_emulation_feature_get;

    
    dnx_data_device.emulation.emulation_system_get = dnx_data_device_emulation_emulation_system_get;
    dnx_data_device.emulation.framer_init_mode_get = dnx_data_device_emulation_framer_init_mode_get;

    
    
    
    dnx_data_device.otp.feature_get = dnx_data_device_otp_feature_get;

    
    dnx_data_device.otp.nof_words_in_row_get = dnx_data_device_otp_nof_words_in_row_get;
    dnx_data_device.otp.read_timeout_get = dnx_data_device_otp_read_timeout_get;

    
    dnx_data_device.otp.otp_instance_attributes_get = dnx_data_device_otp_otp_instance_attributes_get;
    dnx_data_device.otp.otp_instance_attributes_info_get = dnx_data_device_otp_otp_instance_attributes_info_get;
    dnx_data_device.otp.reg_addresses_get = dnx_data_device_otp_reg_addresses_get;
    dnx_data_device.otp.reg_addresses_info_get = dnx_data_device_otp_reg_addresses_info_get;
    
    
    dnx_data_device.docs.feature_get = dnx_data_device_docs_feature_get;

    

    

    
    
    
    dnx_data_dnx2_aod.tables.feature_get = dnx_data_dnx2_aod_tables_feature_get;

    

    
    dnx_data_dnx2_aod.tables.ERPP_FWD_CONTEXT_FILTER___image_get = dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_get;
    dnx_data_dnx2_aod.tables.ERPP_FWD_CONTEXT_FILTER___image_info_get = dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_info_get;
    dnx_data_dnx2_aod.tables.ERPP_FWD_CONTEXT_FILTER_get = dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER_get;
    dnx_data_dnx2_aod.tables.ERPP_FWD_CONTEXT_FILTER_info_get = dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER_info_get;
    dnx_data_dnx2_aod.tables.EGRESS_PER_FORWARD_CODE___image_get = dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE___image_get;
    dnx_data_dnx2_aod.tables.EGRESS_PER_FORWARD_CODE___image_info_get = dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE___image_info_get;
    dnx_data_dnx2_aod.tables.EGRESS_PER_FORWARD_CODE_get = dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE_get;
    dnx_data_dnx2_aod.tables.EGRESS_PER_FORWARD_CODE_info_get = dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE_info_get;
    dnx_data_dnx2_aod.tables.ERPP_FORWARD_CODE_SELECTION___image_get = dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_get;
    dnx_data_dnx2_aod.tables.ERPP_FORWARD_CODE_SELECTION___image_info_get = dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_info_get;
    dnx_data_dnx2_aod.tables.ERPP_FORWARD_CODE_SELECTION_get = dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION_get;
    dnx_data_dnx2_aod.tables.ERPP_FORWARD_CODE_SELECTION_info_get = dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION_info_get;
    dnx_data_dnx2_aod.tables.ETPP_FORWARD_CODE_SELECTION___image_get = dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_get;
    dnx_data_dnx2_aod.tables.ETPP_FORWARD_CODE_SELECTION___image_info_get = dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_info_get;
    dnx_data_dnx2_aod.tables.ETPP_FORWARD_CODE_SELECTION_get = dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION_get;
    dnx_data_dnx2_aod.tables.ETPP_FORWARD_CODE_SELECTION_info_get = dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION_info_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_START_TYPE_CONTEXT___image_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_START_TYPE_CONTEXT___image_info_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_info_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_START_TYPE_CONTEXT_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_START_TYPE_CONTEXT_info_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_info_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_info_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_info_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get;
    dnx_data_dnx2_aod.tables.EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_info_get = dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_info_get;
    dnx_data_dnx2_aod.tables.FER_FILTERS_PER_FWD_CONTEXT___image_get = dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT___image_get;
    dnx_data_dnx2_aod.tables.FER_FILTERS_PER_FWD_CONTEXT___image_info_get = dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT___image_info_get;
    dnx_data_dnx2_aod.tables.FER_FILTERS_PER_FWD_CONTEXT_get = dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT_get;
    dnx_data_dnx2_aod.tables.FER_FILTERS_PER_FWD_CONTEXT_info_get = dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT_info_get;
    dnx_data_dnx2_aod.tables.INGRESS_PRT_INFO___image_get = dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO___image_get;
    dnx_data_dnx2_aod.tables.INGRESS_PRT_INFO___image_info_get = dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO___image_info_get;
    dnx_data_dnx2_aod.tables.INGRESS_PRT_INFO_get = dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO_get;
    dnx_data_dnx2_aod.tables.INGRESS_PRT_INFO_info_get = dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO_info_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_MAP___image_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP___image_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_MAP___image_info_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP___image_info_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_MAP_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_MAP_info_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP_info_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_info_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_info_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get;
    dnx_data_dnx2_aod.tables.MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_info_get = dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_info_get;


    
    
    
    dnx_data_dnx2_aod_sizes.AOD.feature_get = dnx_data_dnx2_aod_sizes_AOD_feature_get;

    
    dnx_data_dnx2_aod_sizes.AOD.ETPP_TRAP_CONTEXT_PORT_PROFILE_get = dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_get;
    dnx_data_dnx2_aod_sizes.AOD.TDM_TRAP_CANDIDATE_HEADER_get = dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_get;
    dnx_data_dnx2_aod_sizes.AOD.TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get = dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get;

    
    
    
    dnx_data_dnx2_aod_sizes.dynamic_AOD.feature_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_get;

    
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_0_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_0_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_1_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_1_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_2_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_2_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_3_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_3_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_4_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_4_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_5_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_5_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_6_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_6_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_7_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_7_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_8_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_8_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_9_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_9_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_10_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_10_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_11_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_11_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_12_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_12_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_13_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_13_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_14_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_14_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_15_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_15_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_16_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_16_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_17_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_17_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_18_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_18_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_19_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_19_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_20_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_20_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_21_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_21_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_22_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_22_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_23_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_23_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_24_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_24_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_25_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_25_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_26_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_26_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_27_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_27_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_28_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_28_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_29_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_29_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_30_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_30_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_31_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_31_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_32_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_32_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_33_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_33_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_34_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_34_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_35_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_35_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_36_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_36_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_37_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_37_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_38_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_38_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_39_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_39_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_40_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_40_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_41_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_41_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_42_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_42_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_43_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_43_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_44_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_44_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_45_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_45_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_46_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_46_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_47_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_47_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_48_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_48_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_49_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_49_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_50_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_50_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_51_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_51_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_52_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_52_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_53_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_53_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_54_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_54_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_55_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_55_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_56_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_56_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_57_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_57_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_58_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_58_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_59_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_59_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_60_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_60_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_61_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_61_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_62_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_62_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_63_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_63_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_64_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_64_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_65_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_65_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_66_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_66_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_67_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_67_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_68_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_68_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_69_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_69_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_70_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_70_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_71_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_71_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_72_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_72_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_73_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_73_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_74_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_74_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_75_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_75_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_76_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_76_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_77_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_77_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_78_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_78_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_79_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_79_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_80_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_80_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_81_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_81_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_82_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_82_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_83_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_83_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_84_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_84_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_85_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_85_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_86_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_86_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_87_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_87_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_88_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_88_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_89_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_89_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_90_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_90_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_91_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_91_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_92_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_92_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_93_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_93_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_94_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_94_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_95_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_95_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_96_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_96_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_97_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_97_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_98_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_98_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_99_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_99_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_100_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_100_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_101_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_101_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_102_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_102_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_103_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_103_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_104_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_104_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_105_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_105_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_106_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_106_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_107_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_107_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_108_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_108_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_109_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_109_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_110_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_110_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_111_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_111_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_112_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_112_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_113_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_113_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_114_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_114_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_115_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_115_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_116_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_116_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_117_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_117_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_118_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_118_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_119_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_119_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_120_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_120_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_121_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_121_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_122_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_122_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_123_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_123_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_124_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_124_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_125_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_125_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_126_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_126_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_127_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_127_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_128_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_128_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_129_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_129_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_130_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_130_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_131_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_131_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_132_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_132_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_133_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_133_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_134_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_134_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_135_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_135_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_136_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_136_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_137_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_137_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_138_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_138_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_139_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_139_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_140_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_140_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_141_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_141_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_142_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_142_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_143_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_143_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_144_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_144_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_145_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_145_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_146_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_146_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_147_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_147_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_148_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_148_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_149_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_149_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_150_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_150_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_151_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_151_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_152_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_152_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_153_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_153_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_154_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_154_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_155_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_155_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_156_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_156_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_157_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_157_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_158_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_158_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_159_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_159_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_160_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_160_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_161_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_161_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_162_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_162_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_163_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_163_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_164_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_164_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_165_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_165_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_166_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_166_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_167_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_167_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_168_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_168_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_169_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_169_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_170_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_170_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_171_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_171_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_172_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_172_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_173_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_173_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_174_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_174_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_175_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_175_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_176_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_176_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_177_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_177_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_178_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_178_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_179_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_179_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_180_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_180_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_181_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_181_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_182_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_182_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_183_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_183_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_184_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_184_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_185_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_185_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_186_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_186_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_187_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_187_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_188_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_188_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_189_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_189_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_190_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_190_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_191_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_191_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_192_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_192_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_193_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_193_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_194_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_194_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_195_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_195_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_196_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_196_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_197_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_197_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_198_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_198_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_199_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_199_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_200_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_200_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_201_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_201_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_202_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_202_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_203_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_203_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_204_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_204_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_205_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_205_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_206_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_206_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_207_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_207_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_208_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_208_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_209_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_209_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_210_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_210_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_211_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_211_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_212_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_212_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_213_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_213_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_214_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_214_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_215_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_215_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_216_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_216_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_217_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_217_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_218_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_218_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_219_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_219_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_220_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_220_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_221_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_221_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_222_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_222_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_223_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_223_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_224_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_224_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_225_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_225_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_226_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_226_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_227_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_227_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_228_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_228_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_229_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_229_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_230_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_230_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_231_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_231_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_232_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_232_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_233_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_233_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_234_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_234_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_235_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_235_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_236_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_236_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_237_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_237_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_238_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_238_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_239_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_239_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_240_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_240_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_241_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_241_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_242_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_242_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_243_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_243_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_244_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_244_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_245_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_245_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_246_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_246_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_247_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_247_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_248_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_248_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_249_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_249_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_250_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_250_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_251_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_251_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_252_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_252_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_253_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_253_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_254_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_254_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_255_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_255_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_256_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_256_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_257_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_257_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_258_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_258_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_259_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_259_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_260_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_260_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_261_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_261_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_262_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_262_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_263_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_263_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_264_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_264_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_265_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_265_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_266_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_266_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_267_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_267_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_268_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_268_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_269_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_269_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_270_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_270_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_271_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_271_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_272_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_272_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_273_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_273_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_274_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_274_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_275_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_275_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_276_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_276_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_277_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_277_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_278_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_278_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_279_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_279_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_280_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_280_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_281_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_281_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_282_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_282_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_283_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_283_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_284_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_284_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_285_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_285_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_286_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_286_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_287_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_287_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_288_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_288_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_289_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_289_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_290_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_290_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_291_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_291_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_292_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_292_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_293_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_293_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_294_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_294_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_295_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_295_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_296_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_296_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_297_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_297_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_298_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_298_get;
    dnx_data_dnx2_aod_sizes.dynamic_AOD.aod_299_get = dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_299_get;

    


    
    
    
    dnx_data_dnx2_arr.prefix.feature_get = dnx_data_dnx2_arr_prefix_feature_get;

    
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_0_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_1_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_2_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_3_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_4_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_5_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_6_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_7_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_8_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_9_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_10_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_11_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_12_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_13_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_14_get;
    dnx_data_dnx2_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get = dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_15_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_0_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_1_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_1_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_2_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_3_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_4_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_5_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_6_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_7_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_7_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_8_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_8_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_9_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_9_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_10_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_10_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_11_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_11_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_12_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_12_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_13_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_13_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_14_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_14_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_15_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_15_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_1_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_1_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_2_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_2_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_3_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_3_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_4_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_4_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_5_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_5_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_6_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_6_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_7_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_7_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_8_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_8_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_9_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_9_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_10_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_10_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_11_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_11_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_12_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_12_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_13_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_13_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_14_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_14_get;
    dnx_data_dnx2_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_15_get = dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_15_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_0_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_0_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_1_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_1_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_2_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_2_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_3_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_3_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_4_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_4_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_5_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_5_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_6_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_6_get;
    dnx_data_dnx2_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_7_get = dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_7_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_0_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_0_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_1_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_1_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_2_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_2_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_3_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_3_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_4_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_4_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_5_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_5_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_6_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_6_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_7_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_7_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_8_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_8_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_9_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_9_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_10_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_10_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_11_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_11_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_12_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_12_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_13_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_13_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_14_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_14_get;
    dnx_data_dnx2_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_15_get = dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_15_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_0_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_0_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_1_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_1_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_2_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_2_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_3_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_3_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_4_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_4_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_5_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_5_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_6_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_6_get;
    dnx_data_dnx2_arr.prefix.IRPP_VSI_FORMATS___prefix_7_get = dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_7_get;

    
    
    
    dnx_data_dnx2_arr.max_nof_entries.feature_get = dnx_data_dnx2_arr_max_nof_entries_feature_get;

    
    dnx_data_dnx2_arr.max_nof_entries.AGE_PROFILE_ARR_get = dnx_data_dnx2_arr_max_nof_entries_AGE_PROFILE_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_ENCAP_1_ARR_ToS_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_1_ARR_ToS_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_ENCAP_2_ARR_ToS_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_2_ARR_ToS_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_ENCAP_3_ARR_ToS_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_3_ARR_ToS_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_ENCAP_4_ARR_ToS_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_4_ARR_ToS_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_ENCAP_5_ARR_ToS_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_5_ARR_ToS_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_ARR_ToS_plus_2_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_FWD_ARR_ToS_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_FWD_ARR_ToS_plus_1_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_plus_1_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_Preprocessing_ARR_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_Preprocessing_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_Termination_1_ARR_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_1_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.ETPP_Termination_2_ARR_get = dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_2_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IPPB_EXISTING_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IPPB_EXISTING_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IPPB_EXISTING_INCOMING_STRENGTH_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IPPB_EXISTING_INCOMING_STRENGTH_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IPPB_INCOMING_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IPPB_INCOMING_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IRPP_FEC_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IRPP_FEC_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IRPP_FWD1_FWD_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_FWD_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IRPP_FWD1_RPF_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_RPF_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IRPP_SUPER_FEC_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IRPP_VSI_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IRPP_VSI_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.IRPP_VTT_ARR_get = dnx_data_dnx2_arr_max_nof_entries_IRPP_VTT_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get = dnx_data_dnx2_arr_max_nof_entries_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.PPMC_ERPP_ARR_get = dnx_data_dnx2_arr_max_nof_entries_PPMC_ERPP_ARR_get;
    dnx_data_dnx2_arr.max_nof_entries.USER_ALLOCATION_get = dnx_data_dnx2_arr_max_nof_entries_USER_ALLOCATION_get;

    
    
    
    dnx_data_dnx2_arr.formats.feature_get = dnx_data_dnx2_arr_formats_feature_get;

    

    
    dnx_data_dnx2_arr.formats.files_get = dnx_data_dnx2_arr_formats_files_get;
    dnx_data_dnx2_arr.formats.files_info_get = dnx_data_dnx2_arr_formats_files_info_get;
    dnx_data_dnx2_arr.formats.MACT_AGE_PROFILE_ARR_CFG_get = dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_get;
    dnx_data_dnx2_arr.formats.MACT_AGE_PROFILE_ARR_CFG_info_get = dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get = dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get;
    dnx_data_dnx2_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_info_get = dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get = dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get;
    dnx_data_dnx2_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_info_get = dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_info_get;
    dnx_data_dnx2_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION_get = dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_get;
    dnx_data_dnx2_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION_info_get = dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_info_get;
    dnx_data_dnx2_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION___array_get = dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_get;
    dnx_data_dnx2_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION___array_info_get = dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get;
    dnx_data_dnx2_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_info_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get;
    dnx_data_dnx2_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_info_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_info_get;
    dnx_data_dnx2_arr.formats.ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get;
    dnx_data_dnx2_arr.formats.ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_info_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_info_get;
    dnx_data_dnx2_arr.formats.IPPB_EXPECTED_ARR_FORMAT_CONFIG_get = dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_get;
    dnx_data_dnx2_arr.formats.IPPB_EXPECTED_ARR_FORMAT_CONFIG_info_get = dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_info_get;
    dnx_data_dnx2_arr.formats.MACT_MRQ_ARR_CFG_get = dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_get;
    dnx_data_dnx2_arr.formats.MACT_MRQ_ARR_CFG_info_get = dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_info_get;
    dnx_data_dnx2_arr.formats.IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get = dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get;
    dnx_data_dnx2_arr.formats.IPPB_ACCEPTED_ARR_FORMAT_CONFIG_info_get = dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_info_get;
    dnx_data_dnx2_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get = dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get;
    dnx_data_dnx2_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_info_get = dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_info_get;
    dnx_data_dnx2_arr.formats.IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get = dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get;
    dnx_data_dnx2_arr.formats.IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_info_get = dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_info_get;
    dnx_data_dnx2_arr.formats.IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get = dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get;
    dnx_data_dnx2_arr.formats.IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_info_get = dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_info_get;
    dnx_data_dnx2_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get = dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get;
    dnx_data_dnx2_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_info_get = dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_info_get;
    dnx_data_dnx2_arr.formats.IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get = dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get;
    dnx_data_dnx2_arr.formats.IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_info_get = dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_info_get;
    dnx_data_dnx2_arr.formats.IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.IPPF_VSI_FORMAT_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get = dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get;
    dnx_data_dnx2_arr.formats.IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get = dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get;
    dnx_data_dnx2_arr.formats.IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get = dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get;
    dnx_data_dnx2_arr.formats.IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get = dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get;
    dnx_data_dnx2_arr.formats.IPPD_LEARN_ARR_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.IPPD_LEARN_ARR_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.OLP_LEARN_ARR_CONFIGURATION_TABLE_get = dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_get;
    dnx_data_dnx2_arr.formats.OLP_LEARN_ARR_CONFIGURATION_TABLE_info_get = dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_info_get;
    dnx_data_dnx2_arr.formats.ERPP_EGRESS_MCDB_FORMAT_CFG_get = dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_get;
    dnx_data_dnx2_arr.formats.ERPP_EGRESS_MCDB_FORMAT_CFG_info_get = dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_info_get;


    
    
    
    dnx_data_dnx2_mdb_app_db.fields.feature_get = dnx_data_dnx2_mdb_app_db_fields_feature_get;

    
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___0_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___0_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___0___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___0___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___1_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___1_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___1___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___1___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___2_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___2_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___2___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___2___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___3_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___3_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___3___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___3___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___4_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___4_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___4___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___4___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___5_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___5_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___5___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___5___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___6_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___6_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___6___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___6___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___7_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___7_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___7___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___7___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___8_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___8_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___8___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___8___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___9_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___9_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___9___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___9___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___10_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___10_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___10___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___10___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___11_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___11_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___11___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___11___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___12_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___12_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___12___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___12___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___13_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___13_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___13___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___13___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___14_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___14_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___14___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___14___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___15_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___15_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___15___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___15___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___16_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___16_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___16___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___16___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___17_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___17_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___17___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___17___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___18_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___18_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___18___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___18___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___19_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___19_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___19___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___19___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___20_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___20_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___20___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___20___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___21_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___21_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___21___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___21___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___22_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___22_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___22___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___22___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___23_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___23_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___23___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___23___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___24_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___24_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___24___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___24___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___25_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___25_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___25___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___25___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___26_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___26_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___26___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___26___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___27_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___27_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___27___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___27___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___28_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___28_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___28___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___28___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___29_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___29_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___29___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___29___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___30_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___30_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___30___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___30___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___31_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___31_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___31___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___31___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___32_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___32_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___32___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___32___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___33_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___33_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___33___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___33___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___34_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___34_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___34___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___34___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___35_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___35_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___35___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___35___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___36_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___36_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___36___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___36___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___37_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___37_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___37___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___37___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___38_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___38_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___38___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___38___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___39_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___39_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___39___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___39___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___40_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___40_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___40___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___40___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___41_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___41_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___41___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___41___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___42_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___42_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___42___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___42___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___43_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___43_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___43___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___43___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___44_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___44_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___44___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___44___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___45_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___45_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___45___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___45___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___46_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___46_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___46___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___46___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___47_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___47_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___47___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___47___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___48_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___48_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___48___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___48___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___49_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___49_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___49___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___49___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___50_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___50_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___50___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___50___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___51_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___51_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___51___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___51___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___52_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___52_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___52___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___52___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___53_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___53_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___53___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___53___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___54_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___54_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___54___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___54___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___55_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___55_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___55___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___55___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___56_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___56_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___56___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___56___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___57_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___57_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___57___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___57___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___58_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___58_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___58___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___58___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___59_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___59_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___59___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___59___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___60_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___60_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___60___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___60___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___61_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___61_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___61___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___61___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___62_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___62_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___62___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___62___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___63_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___63_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___63___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___63___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___64_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___64_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___64___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___64___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___65_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___65_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___65___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___65___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___66_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___66_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___66___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___66___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___67_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___67_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___67___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___67___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___68_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___68_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___68___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___68___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___69_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___69_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___69___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___69___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___70_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___70_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___70___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___70___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___71_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___71_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___71___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___71___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___72_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___72_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___72___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___72___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___73_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___73_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___73___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___73___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___74_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___74_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___74___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___74___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___75_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___75_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___75___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___75___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___76_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___76_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___76___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___76___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___77_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___77_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___77___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___77___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___78_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___78_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___78___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___78___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___79_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___79_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___79___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___79___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___80_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___80_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___80___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___80___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___81_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___81_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___81___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___81___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___82_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___82_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___82___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___82___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___83_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___83_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___83___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___83___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___84_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___84_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___84___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___84___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___85_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___85_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___85___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___85___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___86_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___86_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___86___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___86___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___87_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___87_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___87___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___87___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___88_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___88_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___88___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___88___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___89_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___89_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___89___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___89___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___90_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___90_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___90___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___90___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___91_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___91_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___91___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___91___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___92_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___92_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___92___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___92___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___93_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___93_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___93___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___93___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___94_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___94_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___94___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___94___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___95_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___95_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___95___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___95___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___96_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___96_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___96___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___96___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___97_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___97_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___97___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___97___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___98_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___98_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___98___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___98___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___99_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___99_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___99___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___99___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___100_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___100_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___100___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___100___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___101_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___101_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___101___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___101___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___102_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___102_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___102___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___102___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___103_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___103_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___103___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___103___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___104_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___104_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___104___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___104___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___105_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___105_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___105___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___105___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___106_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___106_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___106___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___106___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___107_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___107_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___107___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___107___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___108_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___108_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___108___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___108___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___109_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___109_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___109___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___109___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___110_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___110_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___110___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___110___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___111_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___111_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___111___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___111___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___112_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___112_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___112___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___112___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___113_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___113_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___113___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___113___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___114_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___114_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___114___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___114___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___115_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___115_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___115___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___115___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___116_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___116_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___116___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___116___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___117_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___117_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___117___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___117___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___118_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___118_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___118___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___118___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___119_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___119_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___119___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___119___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___120_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___120_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___120___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___120___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___121_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___121_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___121___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___121___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___122_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___122_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___122___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___122___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___123_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___123_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___123___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___123___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___124_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___124_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___124___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___124___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___125_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___125_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___125___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___125___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___126_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___126_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___126___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___126___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___127_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___127_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___127___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___127___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___128_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___128_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___128___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___128___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___129_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___129_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___129___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___129___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___130_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___130_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___130___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___130___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___131_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___131_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___131___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___131___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___132_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___132_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___132___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___132___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___133_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___133_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___133___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___133___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___134_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___134_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___134___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___134___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___135_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___135_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___135___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___135___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___136_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___136_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___136___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___136___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___137_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___137_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___137___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___137___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___138_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___138_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___138___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___138___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___139_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___139_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___139___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___139___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___140_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___140_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___140___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___140___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___141_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___141_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___141___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___141___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___142_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___142_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___142___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___142___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___143_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___143_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___143___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___143___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___144_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___144_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___144___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___144___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___145_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___145_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___145___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___145___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___146_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___146_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___146___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___146___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___147_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___147_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___147___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___147___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___148_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___148_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___148___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___148___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___149_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___149_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___149___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___149___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___150_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___150_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___150___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___150___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___151_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___151_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___151___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___151___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___152_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___152_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___152___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___152___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___153_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___153_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___153___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___153___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___154_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___154_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___154___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___154___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___155_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___155_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___155___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___155___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___156_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___156_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___156___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___156___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___157_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___157_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___157___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___157___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___158_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___158_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___158___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___158___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___159_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___159_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___159___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___159___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___160_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___160_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___160___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___160___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___161_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___161_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___161___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___161___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___162_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___162_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___162___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___162___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___163_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___163_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___163___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___163___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___164_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___164_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___164___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___164___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___165_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___165_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___165___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___165___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___166_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___166_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___166___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___166___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___167_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___167_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___167___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___167___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___168_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___168_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___168___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___168___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___169_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___169_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___169___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___169___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___170_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___170_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___170___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___170___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___171_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___171_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___171___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___171___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___172_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___172_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___172___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___172___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___173_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___173_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___173___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___173___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___174_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___174_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___174___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___174___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___175_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___175_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___175___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___175___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___176_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___176_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___176___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___176___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___177_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___177_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___177___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___177___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___178_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___178_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___178___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___178___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___179_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___179_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___179___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___179___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___180_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___180_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___180___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___180___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___181_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___181_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___181___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___181___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___182_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___182_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___182___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___182___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___183_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___183_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___183___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___183___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___184_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___184_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___184___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___184___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___185_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___185_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___185___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___185___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___186_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___186_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___186___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___186___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___187_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___187_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___187___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___187___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___188_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___188_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___188___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___188___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___189_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___189_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___189___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___189___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___190_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___190_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___190___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___190___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___191_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___191_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___191___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___191___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___192_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___192_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___192___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___192___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___193_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___193_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___193___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___193___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___194_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___194_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___194___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___194___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___195_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___195_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___195___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___195___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___196_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___196_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___196___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___196___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___197_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___197_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___197___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___197___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___198_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___198_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___198___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___198___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___199_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___199_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___199___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___199___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___200_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___200_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___200___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___200___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___201_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___201_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___201___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___201___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___202_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___202_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___202___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___202___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___203_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___203_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___203___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___203___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___204_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___204_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___204___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___204___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___205_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___205_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___205___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___205___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___206_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___206_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___206___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___206___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___207_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___207_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___207___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___207___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___208_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___208_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___208___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___208___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___209_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___209_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___209___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___209___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___210_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___210_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___210___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___210___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___211_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___211_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___211___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___211___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___212_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___212_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___212___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___212___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___213_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___213_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___213___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___213___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___214_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___214_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___214___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___214___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___215_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___215_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___215___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___215___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___216_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___216_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___216___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___216___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___217_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___217_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___217___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___217___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___218_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___218_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___218___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___218___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___219_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___219_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___219___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___219___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___220_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___220_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___220___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___220___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___221_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___221_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___221___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___221___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___222_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___222_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___222___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___222___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___223_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___223_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___223___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___223___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___224_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___224_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___224___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___224___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___225_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___225_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___225___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___225___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___226_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___226_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___226___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___226___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___227_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___227_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___227___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___227___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___228_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___228_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___228___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___228___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___229_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___229_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___229___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___229___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___230_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___230_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___230___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___230___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___231_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___231_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___231___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___231___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___232_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___232_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___232___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___232___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___233_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___233_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___233___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___233___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___234_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___234_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___234___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___234___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___235_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___235_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___235___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___235___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___236_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___236_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___236___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___236___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___237_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___237_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___237___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___237___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___238_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___238_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___238___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___238___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___239_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___239_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___239___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___239___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___240_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___240_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___240___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___240___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___241_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___241_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___241___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___241___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___242_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___242_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___242___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___242___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___243_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___243_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___243___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___243___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___244_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___244_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___244___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___244___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___245_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___245_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___245___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___245___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___246_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___246_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___246___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___246___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___247_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___247_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___247___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___247___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___248_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___248_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___248___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___248___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___249_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___249_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___249___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___249___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___250_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___250_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___250___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___250___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___251_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___251_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___251___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___251___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___252_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___252_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___252___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___252___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___253_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___253_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___253___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___253___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___254_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___254_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___254___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___254___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___255_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___255_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___255___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___255___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___256_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___256_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___256___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___256___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___257_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___257_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___257___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___257___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___258_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___258_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___258___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___258___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___259_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___259_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___259___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___259___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___260_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___260_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___260___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___260___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___261_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___261_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___261___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___261___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___262_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___262_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___262___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___262___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___263_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___263_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___263___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___263___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___264_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___264_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___264___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___264___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___265_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___265_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___265___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___265___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___266_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___266_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___266___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___266___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___267_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___267_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___267___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___267___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___268_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___268_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___268___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___268___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___269_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___269_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___269___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___269___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___270_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___270_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___270___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___270___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___271_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___271_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___271___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___271___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___272_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___272_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___272___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___272___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___273_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___273_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___273___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___273___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___274_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___274_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___274___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___274___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___275_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___275_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___275___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___275___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___276_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___276_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___276___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___276___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___277_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___277_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___277___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___277___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___278_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___278_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___278___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___278___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___279_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___279_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___279___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___279___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___280_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___280_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___280___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___280___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___281_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___281_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___281___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___281___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___282_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___282_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___282___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___282___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___283_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___283_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___283___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___283___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___284_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___284_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___284___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___284___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___285_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___285_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___285___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___285___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___286_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___286_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___286___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___286___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___287_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___287_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___287___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___287___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___288_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___288_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___288___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___288___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___289_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___289_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___289___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___289___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___290_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___290_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___290___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___290___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___291_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___291_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___291___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___291___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___292_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___292_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___292___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___292___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___293_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___293_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___293___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___293___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___294_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___294_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___294___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___294___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___295_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___295_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___295___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___295___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___296_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___296_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___296___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___296___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___297_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___297_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___297___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___297___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___298_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___298_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___298___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___298___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___299_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___299_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___299___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___299___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___300_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___300_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___300___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___300___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___301_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___301_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___301___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___301___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___302_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___302_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___302___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___302___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___303_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___303_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___303___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___303___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___304_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___304_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___304___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___304___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___305_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___305_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___305___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___305___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___306_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___306_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___306___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___306___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___307_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___307_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___307___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___307___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___308_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___308_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___308___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___308___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___309_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___309_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___309___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___309___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___310_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___310_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___310___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___310___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___311_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___311_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___311___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___311___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___312_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___312_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___312___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___312___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___313_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___313_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___313___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___313___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___314_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___314_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___314___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___314___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___315_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___315_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___315___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___315___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___316_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___316_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___316___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___316___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___317_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___317_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___317___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___317___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___318_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___318_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___318___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___318___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___319_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___319_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___319___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___319___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___320_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___320_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___320___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___320___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___321_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___321_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___321___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___321___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___322_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___322_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___322___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___322___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___323_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___323_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___323___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___323___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___324_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___324_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___324___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___324___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___325_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___325_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___325___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___325___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___326_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___326_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___326___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___326___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___327_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___327_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___327___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___327___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___328_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___328_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___328___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___328___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___329_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___329_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___329___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___329___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___330_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___330_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___330___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___330___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___331_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___331_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___331___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___331___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___332_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___332_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___332___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___332___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___333_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___333_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___333___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___333___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___334_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___334_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___334___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___334___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___335_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___335_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___335___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___335___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___336_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___336_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___336___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___336___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___337_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___337_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___337___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___337___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___338_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___338_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___338___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___338___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___339_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___339_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___339___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___339___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___340_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___340_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___340___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___340___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___341_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___341_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___341___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___341___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___342_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___342_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___342___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___342___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___343_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___343_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___343___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___343___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___344_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___344_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___344___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___344___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___345_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___345_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___345___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___345___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___346_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___346_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___346___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___346___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___347_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___347_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___347___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___347___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___348_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___348_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___348___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___348___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___349_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___349_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___349___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___349___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___350_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___350_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___350___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___350___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___351_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___351_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___351___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___351___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___352_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___352_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___352___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___352___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___353_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___353_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___353___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___353___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___354_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___354_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___354___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___354___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___355_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___355_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___355___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___355___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___356_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___356_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___356___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___356___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___357_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___357_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___357___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___357___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___358_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___358_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___358___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___358___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___359_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___359_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___359___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___359___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___360_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___360_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___360___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___360___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___361_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___361_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___361___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___361___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___362_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___362_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___362___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___362___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___363_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___363_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___363___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___363___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___364_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___364_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___364___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___364___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___365_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___365_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___365___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___365___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___366_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___366_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___366___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___366___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___367_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___367_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___367___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___367___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___368_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___368_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___368___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___368___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___369_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___369_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___369___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___369___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___370_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___370_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___370___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___370___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___371_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___371_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___371___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___371___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___372_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___372_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___372___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___372___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___373_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___373_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___373___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___373___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___374_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___374_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___374___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___374___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___375_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___375_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___375___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___375___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___376_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___376_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___376___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___376___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___377_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___377_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___377___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___377___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___378_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___378_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___378___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___378___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___379_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___379_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___379___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___379___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___380_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___380_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___380___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___380___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___381_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___381_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___381___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___381___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___382_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___382_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___382___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___382___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___383_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___383_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___383___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___383___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___384_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___384_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___384___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___384___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___385_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___385_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___385___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___385___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___386_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___386_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___386___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___386___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___387_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___387_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___387___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___387___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___388_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___388_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___388___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___388___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___389_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___389_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___389___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___389___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___390_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___390_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___390___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___390___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___391_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___391_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___391___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___391___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___392_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___392_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___392___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___392___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___393_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___393_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___393___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___393___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___394_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___394_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___394___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___394___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___395_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___395_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___395___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___395___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___396_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___396_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___396___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___396___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___397_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___397_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___397___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___397___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___398_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___398_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___398___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___398___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___399_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___399_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___399___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___399___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___400_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___400_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___400___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___400___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___401_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___401_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___401___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___401___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___402_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___402_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___402___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___402___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___403_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___403_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___403___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___403___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___404_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___404_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___404___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___404___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___405_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___405_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___405___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___405___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___406_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___406_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___406___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___406___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___407_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___407_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___407___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___407___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___408_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___408_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___408___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___408___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___409_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___409_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___409___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___409___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___410_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___410_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___410___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___410___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___411_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___411_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___411___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___411___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___412_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___412_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___412___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___412___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___413_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___413_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___413___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___413___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___414_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___414_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___414___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___414___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___415_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___415_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___415___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___415___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___416_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___416_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___416___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___416___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___417_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___417_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___417___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___417___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___418_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___418_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___418___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___418___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___419_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___419_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___419___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___419___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___420_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___420_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___420___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___420___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___421_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___421_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___421___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___421___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___422_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___422_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___422___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___422___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___423_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___423_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___423___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___423___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___424_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___424_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___424___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___424___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___425_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___425_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___425___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___425___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___426_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___426_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___426___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___426___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___427_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___427_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___427___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___427___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___428_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___428_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___428___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___428___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___429_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___429_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___429___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___429___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___430_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___430_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___430___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___430___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___431_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___431_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___431___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___431___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___432_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___432_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___432___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___432___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___433_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___433_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___433___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___433___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___434_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___434_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___434___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___434___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___435_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___435_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___435___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___435___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___436_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___436_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___436___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___436___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___437_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___437_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___437___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___437___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___438_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___438_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___438___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___438___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___439_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___439_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___439___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___439___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___440_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___440_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___440___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___440___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___441_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___441_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___441___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___441___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___442_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___442_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___442___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___442___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___443_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___443_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___443___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___443___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___444_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___444_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___444___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___444___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___445_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___445_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___445___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___445___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___446_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___446_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___446___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___446___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___447_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___447_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___447___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___447___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___448_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___448_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___448___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___448___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___449_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___449_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___449___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___449___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___450_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___450_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___450___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___450___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___451_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___451_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___451___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___451___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___452_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___452_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___452___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___452___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___453_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___453_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___453___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___453___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___454_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___454_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___454___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___454___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___455_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___455_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___455___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___455___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___456_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___456_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___456___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___456___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___457_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___457_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___457___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___457___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___458_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___458_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___458___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___458___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___459_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___459_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___459___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___459___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___460_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___460_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___460___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___460___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___461_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___461_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___461___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___461___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___462_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___462_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___462___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___462___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___463_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___463_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___463___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___463___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___464_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___464_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___464___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___464___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___465_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___465_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___465___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___465___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___466_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___466_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___466___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___466___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___467_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___467_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___467___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___467___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___468_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___468_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___468___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___468___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___469_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___469_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___469___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___469___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___470_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___470_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___470___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___470___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___471_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___471_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___471___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___471___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___472_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___472_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___472___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___472___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___473_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___473_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___473___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___473___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___474_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___474_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___474___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___474___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___475_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___475_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___475___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___475___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___476_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___476_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___476___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___476___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___477_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___477_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___477___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___477___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___478_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___478_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___478___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___478___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___479_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___479_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___479___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___479___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___480_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___480_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___480___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___480___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___481_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___481_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___481___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___481___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___482_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___482_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___482___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___482___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___483_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___483_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___483___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___483___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___484_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___484_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___484___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___484___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___485_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___485_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___485___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___485___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___486_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___486_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___486___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___486___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___487_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___487_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___487___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___487___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___488_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___488_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___488___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___488___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___489_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___489_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___489___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___489___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___490_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___490_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___490___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___490___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___491_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___491_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___491___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___491___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___492_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___492_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___492___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___492___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___493_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___493_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___493___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___493___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___494_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___494_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___494___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___494___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___495_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___495_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___495___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___495___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___496_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___496_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___496___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___496___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___497_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___497_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___497___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___497___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___498_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___498_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___498___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___498___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___499_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___499_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___499___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___499___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___500_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___500_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___500___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___500___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___501_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___501_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___501___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___501___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___502_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___502_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___502___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___502___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___503_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___503_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___503___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___503___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___504_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___504_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___504___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___504___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___505_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___505_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___505___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___505___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___506_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___506_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___506___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___506___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___507_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___507_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___507___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___507___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___508_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___508_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___508___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___508___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___509_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___509_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___509___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___509___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___510_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___510_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___510___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___510___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___511_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___511_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___511___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___511___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___512_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___512_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___512___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___512___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___513_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___513_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___513___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___513___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___514_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___514_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___514___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___514___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___515_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___515_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___515___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___515___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___516_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___516_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___516___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___516___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___517_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___517_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___517___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___517___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___518_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___518_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___518___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___518___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___519_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___519_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___519___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___519___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___520_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___520_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___520___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___520___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___521_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___521_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___521___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___521___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___522_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___522_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___522___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___522___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___523_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___523_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___523___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___523___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___524_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___524_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___524___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___524___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___525_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___525_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___525___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___525___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___526_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___526_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___526___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___526___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___527_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___527_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___527___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___527___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___528_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___528_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___528___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___528___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___529_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___529_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___529___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___529___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___530_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___530_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___530___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___530___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___531_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___531_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___531___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___531___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___532_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___532_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___532___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___532___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___533_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___533_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___533___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___533___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___534_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___534_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___534___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___534___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___535_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___535_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___535___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___535___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___536_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___536_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___536___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___536___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___537_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___537_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___537___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___537___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___538_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___538_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___538___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___538___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___539_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___539_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___539___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___539___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___540_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___540_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___540___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___540___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___541_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___541_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___541___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___541___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___542_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___542_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___542___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___542___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___543_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___543_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___543___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___543___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___544_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___544_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___544___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___544___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___545_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___545_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___545___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___545___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___546_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___546_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___546___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___546___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___547_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___547_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___547___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___547___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___548_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___548_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___548___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___548___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___549_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___549_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___549___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___549___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___550_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___550_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___550___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___550___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___551_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___551_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___551___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___551___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___552_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___552_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___552___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___552___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___553_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___553_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___553___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___553___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___554_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___554_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___554___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___554___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___555_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___555_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___555___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___555___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___556_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___556_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___556___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___556___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___557_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___557_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___557___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___557___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___558_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___558_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___558___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___558___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___559_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___559_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___559___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___559___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___560_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___560_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___560___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___560___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___561_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___561_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___561___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___561___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___562_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___562_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___562___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___562___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___563_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___563_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___563___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___563___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___564_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___564_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___564___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___564___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___565_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___565_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___565___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___565___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___566_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___566_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___566___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___566___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___567_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___567_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___567___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___567___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___568_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___568_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___568___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___568___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___569_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___569_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___569___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___569___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___570_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___570_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___570___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___570___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___571_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___571_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___571___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___571___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___572_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___572_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___572___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___572___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___573_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___573_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___573___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___573___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___574_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___574_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___574___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___574___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___575_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___575_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___575___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___575___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___576_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___576_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___576___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___576___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___577_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___577_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___577___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___577___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___578_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___578_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___578___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___578___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___579_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___579_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___579___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___579___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___580_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___580_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___580___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___580___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___581_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___581_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___581___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___581___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___582_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___582_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___582___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___582___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___583_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___583_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___583___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___583___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___584_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___584_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___584___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___584___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___585_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___585_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___585___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___585___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___586_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___586_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___586___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___586___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___587_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___587_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___587___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___587___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___588_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___588_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___588___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___588___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___589_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___589_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___589___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___589___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___590_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___590_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___590___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___590___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___591_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___591_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___591___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___591___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___592_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___592_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___592___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___592___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___593_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___593_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___593___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___593___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___594_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___594_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___594___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___594___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___595_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___595_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___595___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___595___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___596_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___596_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___596___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___596___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___597_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___597_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___597___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___597___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___598_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___598_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___598___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___598___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___599_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___599_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___599___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___599___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___600_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___600_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___600___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___600___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___601_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___601_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___601___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___601___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___602_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___602_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___602___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___602___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___603_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___603_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___603___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___603___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___604_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___604_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___604___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___604___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___605_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___605_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___605___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___605___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___606_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___606_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___606___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___606___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___607_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___607_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___607___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___607___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___608_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___608_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___608___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___608___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___609_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___609_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___609___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___609___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___610_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___610_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___610___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___610___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___611_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___611_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___611___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___611___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___612_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___612_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___612___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___612___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___613_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___613_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___613___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___613___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___614_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___614_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___614___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___614___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___615_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___615_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___615___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___615___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___616_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___616_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___616___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___616___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___617_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___617_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___617___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___617___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___618_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___618_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___618___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___618___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___619_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___619_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___619___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___619___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___620_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___620_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___620___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___620___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___621_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___621_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___621___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___621___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___622_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___622_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___622___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___622___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___623_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___623_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___623___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___623___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___624_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___624_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___624___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___624___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___625_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___625_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___625___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___625___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___626_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___626_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___626___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___626___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___627_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___627_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___627___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___627___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___628_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___628_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___628___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___628___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___629_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___629_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___629___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___629___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___630_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___630_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___630___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___630___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___631_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___631_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___631___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___631___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___632_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___632_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___632___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___632___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___633_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___633_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___633___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___633___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___634_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___634_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___634___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___634___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___635_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___635_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___635___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___635___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___636_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___636_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___636___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___636___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___637_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___637_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___637___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___637___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___638_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___638_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___638___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___638___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___639_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___639_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___639___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___639___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___640_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___640_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___640___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___640___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___641_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___641_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___641___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___641___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___642_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___642_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___642___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___642___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___643_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___643_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___643___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___643___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___644_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___644_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___644___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___644___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___645_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___645_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___645___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___645___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___646_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___646_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___646___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___646___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___647_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___647_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___647___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___647___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___648_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___648_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___648___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___648___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___649_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___649_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___649___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___649___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___650_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___650_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___650___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___650___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___651_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___651_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___651___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___651___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___652_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___652_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___652___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___652___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___653_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___653_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___653___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___653___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___654_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___654_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___654___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___654___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___655_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___655_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___655___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___655___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___656_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___656_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___656___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___656___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___657_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___657_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___657___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___657___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___658_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___658_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___658___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___658___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___659_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___659_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___659___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___659___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___660_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___660_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___660___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___660___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___661_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___661_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___661___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___661___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___662_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___662_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___662___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___662___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___663_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___663_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___663___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___663___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___664_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___664_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___664___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___664___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___665_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___665_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___665___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___665___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___666_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___666_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___666___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___666___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___667_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___667_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___667___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___667___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___668_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___668_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___668___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___668___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___669_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___669_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___669___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___669___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___670_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___670_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___670___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___670___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___671_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___671_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___671___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___671___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___672_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___672_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___672___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___672___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___673_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___673_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___673___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___673___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___674_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___674_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___674___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___674___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___675_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___675_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___675___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___675___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___676_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___676_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___676___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___676___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___677_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___677_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___677___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___677___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___678_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___678_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___678___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___678___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___679_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___679_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___679___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___679___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___680_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___680_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___680___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___680___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___681_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___681_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___681___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___681___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___682_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___682_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___682___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___682___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___683_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___683_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___683___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___683___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___684_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___684_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___684___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___684___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___685_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___685_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___685___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___685___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___686_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___686_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___686___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___686___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___687_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___687_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___687___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___687___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___688_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___688_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___688___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___688___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___689_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___689_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___689___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___689___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___690_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___690_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___690___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___690___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___691_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___691_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___691___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___691___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___692_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___692_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___692___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___692___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___693_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___693_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___693___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___693___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___694_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___694_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___694___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___694___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___695_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___695_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___695___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___695___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___696_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___696_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___696___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___696___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___697_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___697_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___697___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___697___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___698_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___698_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___698___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___698___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___699_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___699_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___699___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___699___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___700_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___700_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___700___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___700___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___701_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___701_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___701___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___701___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___702_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___702_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___702___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___702___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___703_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___703_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___703___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___703___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___704_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___704_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___704___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___704___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___705_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___705_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___705___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___705___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___706_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___706_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___706___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___706___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___707_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___707_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___707___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___707___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___708_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___708_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___708___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___708___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___709_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___709_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___709___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___709___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___710_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___710_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___710___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___710___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___711_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___711_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___711___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___711___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___712_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___712_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___712___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___712___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___713_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___713_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___713___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___713___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___714_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___714_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___714___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___714___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___715_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___715_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___715___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___715___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___716_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___716_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___716___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___716___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___717_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___717_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___717___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___717___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___718_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___718_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___718___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___718___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___719_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___719_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___719___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___719___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___720_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___720_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___720___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___720___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___721_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___721_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___721___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___721___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___722_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___722_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___722___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___722___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___723_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___723_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___723___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___723___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___724_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___724_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___724___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___724___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___725_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___725_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___725___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___725___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___726_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___726_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___726___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___726___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___727_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___727_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___727___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___727___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___728_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___728_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___728___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___728___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___729_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___729_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___729___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___729___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___730_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___730_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___730___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___730___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___731_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___731_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___731___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___731___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___732_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___732_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___732___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___732___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___733_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___733_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___733___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___733___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___734_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___734_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___734___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___734___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___735_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___735_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___735___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___735___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___736_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___736_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___736___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___736___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___737_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___737_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___737___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___737___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___738_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___738_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___738___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___738___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___739_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___739_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___739___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___739___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___740_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___740_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___740___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___740___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___741_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___741_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___741___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___741___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___742_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___742_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___742___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___742___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___743_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___743_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___743___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___743___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___744_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___744_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___744___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___744___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___745_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___745_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___745___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___745___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___746_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___746_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___746___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___746___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___747_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___747_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___747___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___747___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___748_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___748_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___748___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___748___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___749_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___749_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___749___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___749___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___750_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___750_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___750___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___750___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___751_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___751_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___751___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___751___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___752_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___752_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___752___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___752___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___753_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___753_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___753___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___753___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___754_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___754_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___754___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___754___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___755_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___755_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___755___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___755___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___756_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___756_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___756___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___756___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___757_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___757_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___757___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___757___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___758_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___758_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___758___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___758___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___759_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___759_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___759___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___759___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___760_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___760_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___760___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___760___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___761_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___761_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___761___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___761___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___762_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___762_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___762___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___762___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___763_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___763_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___763___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___763___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___764_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___764_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___764___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___764___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___765_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___765_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___765___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___765___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___766_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___766_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___766___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___766___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___767_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___767_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___767___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___767___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___768_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___768_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___768___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___768___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___769_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___769_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___769___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___769___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___770_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___770_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___770___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___770___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___771_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___771_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___771___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___771___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___772_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___772_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___772___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___772___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___773_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___773_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___773___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___773___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___774_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___774_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___774___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___774___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___775_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___775_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___775___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___775___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___776_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___776_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___776___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___776___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___777_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___777_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___777___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___777___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___778_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___778_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___778___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___778___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___779_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___779_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___779___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___779___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___780_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___780_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___780___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___780___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___781_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___781_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___781___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___781___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___782_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___782_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___782___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___782___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___783_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___783_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___783___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___783___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___784_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___784_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___784___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___784___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___785_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___785_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___785___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___785___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___786_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___786_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___786___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___786___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___787_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___787_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___787___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___787___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___788_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___788_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___788___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___788___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___789_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___789_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___789___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___789___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___790_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___790_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___790___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___790___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___791_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___791_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___791___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___791___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___792_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___792_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___792___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___792___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___793_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___793_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___793___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___793___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___794_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___794_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___794___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___794___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___795_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___795_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___795___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___795___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___796_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___796_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___796___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___796___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___797_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___797_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___797___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___797___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___798_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___798_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___798___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___798___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___799_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___799_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___799___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___799___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___800_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___800_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___800___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___800___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___801_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___801_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___801___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___801___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___802_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___802_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___802___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___802___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___803_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___803_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___803___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___803___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___804_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___804_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___804___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___804___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___805_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___805_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___805___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___805___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___806_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___806_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___806___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___806___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___807_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___807_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___807___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___807___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___808_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___808_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___808___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___808___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___809_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___809_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___809___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___809___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___810_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___810_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___810___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___810___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___811_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___811_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___811___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___811___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___812_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___812_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___812___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___812___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___813_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___813_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___813___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___813___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___814_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___814_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___814___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___814___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___815_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___815_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___815___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___815___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___816_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___816_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___816___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___816___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___817_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___817_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___817___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___817___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___818_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___818_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___818___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___818___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___819_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___819_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___819___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___819___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___820_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___820_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___820___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___820___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___821_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___821_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___821___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___821___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___822_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___822_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___822___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___822___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___823_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___823_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___823___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___823___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___824_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___824_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___824___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___824___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___825_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___825_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___825___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___825___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___826_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___826_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___826___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___826___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___827_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___827_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___827___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___827___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___828_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___828_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___828___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___828___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___829_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___829_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___829___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___829___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___830_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___830_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___830___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___830___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___831_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___831_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___831___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___831___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___832_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___832_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___832___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___832___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___833_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___833_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___833___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___833___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___834_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___834_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___834___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___834___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___835_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___835_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___835___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___835___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___836_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___836_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___836___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___836___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___837_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___837_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___837___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___837___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___838_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___838_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___838___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___838___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___839_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___839_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___839___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___839___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___840_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___840_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___840___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___840___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___841_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___841_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___841___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___841___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___842_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___842_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___842___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___842___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___843_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___843_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___843___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___843___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___844_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___844_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___844___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___844___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___845_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___845_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___845___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___845___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___846_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___846_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___846___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___846___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___847_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___847_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___847___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___847___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___848_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___848_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___848___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___848___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___849_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___849_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___849___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___849___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___850_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___850_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___850___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___850___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___851_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___851_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___851___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___851___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___852_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___852_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___852___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___852___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___853_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___853_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___853___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___853___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___854_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___854_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___854___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___854___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___855_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___855_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___855___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___855___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___856_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___856_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___856___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___856___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___857_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___857_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___857___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___857___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___858_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___858_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___858___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___858___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___859_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___859_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___859___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___859___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___860_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___860_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___860___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___860___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___861_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___861_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___861___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___861___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___862_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___862_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___862___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___862___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___863_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___863_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___863___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___863___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___864_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___864_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___864___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___864___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___865_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___865_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___865___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___865___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___866_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___866_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___866___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___866___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___867_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___867_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___867___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___867___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___868_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___868_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___868___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___868___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___869_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___869_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___869___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___869___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___870_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___870_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___870___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___870___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___871_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___871_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___871___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___871___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___872_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___872_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___872___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___872___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___873_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___873_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___873___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___873___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___874_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___874_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___874___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___874___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___875_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___875_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___875___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___875___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___876_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___876_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___876___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___876___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___877_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___877_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___877___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___877___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___878_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___878_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___878___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___878___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___879_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___879_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___879___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___879___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___880_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___880_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___880___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___880___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___881_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___881_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___881___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___881___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___882_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___882_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___882___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___882___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___883_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___883_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___883___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___883___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___884_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___884_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___884___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___884___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___885_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___885_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___885___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___885___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___886_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___886_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___886___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___886___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___887_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___887_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___887___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___887___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___888_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___888_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___888___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___888___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___889_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___889_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___889___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___889___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___890_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___890_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___890___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___890___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___891_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___891_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___891___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___891___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___892_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___892_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___892___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___892___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___893_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___893_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___893___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___893___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___894_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___894_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___894___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___894___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___895_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___895_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___895___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___895___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___896_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___896_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___896___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___896___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___897_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___897_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___897___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___897___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___898_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___898_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___898___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___898___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___899_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___899_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___899___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___899___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___900_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___900_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___900___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___900___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___901_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___901_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___901___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___901___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___902_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___902_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___902___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___902___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___903_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___903_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___903___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___903___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___904_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___904_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___904___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___904___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___905_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___905_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___905___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___905___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___906_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___906_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___906___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___906___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___907_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___907_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___907___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___907___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___908_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___908_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___908___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___908___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___909_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___909_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___909___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___909___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___910_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___910_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___910___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___910___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___911_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___911_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___911___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___911___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___912_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___912_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___912___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___912___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___913_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___913_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___913___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___913___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___914_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___914_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___914___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___914___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___915_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___915_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___915___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___915___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___916_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___916_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___916___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___916___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___917_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___917_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___917___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___917___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___918_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___918_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___918___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___918___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___919_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___919_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___919___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___919___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___920_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___920_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___920___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___920___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___921_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___921_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___921___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___921___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___922_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___922_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___922___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___922___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___923_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___923_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___923___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___923___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___924_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___924_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___924___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___924___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___925_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___925_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___925___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___925___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___926_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___926_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___926___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___926___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___927_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___927_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___927___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___927___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___928_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___928_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___928___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___928___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___929_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___929_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___929___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___929___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___930_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___930_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___930___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___930___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___931_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___931_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___931___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___931___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___932_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___932_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___932___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___932___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___933_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___933_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___933___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___933___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___934_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___934_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___934___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___934___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___935_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___935_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___935___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___935___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___936_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___936_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___936___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___936___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___937_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___937_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___937___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___937___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___938_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___938_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___938___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___938___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___939_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___939_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___939___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___939___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___940_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___940_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___940___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___940___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___941_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___941_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___941___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___941___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___942_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___942_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___942___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___942___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___943_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___943_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___943___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___943___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___944_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___944_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___944___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___944___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___945_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___945_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___945___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___945___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___946_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___946_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___946___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___946___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___947_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___947_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___947___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___947___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___948_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___948_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___948___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___948___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___949_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___949_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___949___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___949___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___950_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___950_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___950___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___950___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___951_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___951_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___951___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___951___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___952_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___952_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___952___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___952___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___953_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___953_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___953___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___953___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___954_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___954_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___954___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___954___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___955_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___955_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___955___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___955___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___956_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___956_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___956___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___956___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___957_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___957_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___957___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___957___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___958_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___958_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___958___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___958___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___959_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___959_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___959___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___959___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___960_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___960_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___960___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___960___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___961_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___961_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___961___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___961___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___962_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___962_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___962___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___962___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___963_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___963_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___963___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___963___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___964_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___964_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___964___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___964___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___965_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___965_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___965___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___965___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___966_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___966_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___966___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___966___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___967_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___967_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___967___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___967___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___968_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___968_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___968___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___968___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___969_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___969_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___969___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___969___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___970_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___970_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___970___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___970___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___971_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___971_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___971___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___971___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___972_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___972_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___972___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___972___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___973_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___973_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___973___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___973___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___974_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___974_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___974___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___974___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___975_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___975_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___975___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___975___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___976_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___976_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___976___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___976___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___977_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___977_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___977___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___977___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___978_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___978_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___978___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___978___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___979_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___979_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___979___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___979___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___980_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___980_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___980___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___980___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___981_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___981_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___981___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___981___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___982_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___982_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___982___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___982___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___983_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___983_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___983___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___983___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___984_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___984_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___984___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___984___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___985_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___985_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___985___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___985___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___986_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___986_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___986___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___986___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___987_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___987_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___987___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___987___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___988_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___988_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___988___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___988___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___989_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___989_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___989___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___989___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___990_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___990_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___990___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___990___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___991_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___991_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___991___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___991___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___992_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___992_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___992___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___992___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___993_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___993_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___993___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___993___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___994_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___994_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___994___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___994___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___995_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___995_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___995___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___995___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___996_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___996_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___996___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___996___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___997_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___997_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___997___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___997___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___998_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___998_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___998___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___998___valid_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___999_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___999_get;
    dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___999___valid_get = dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___999___valid_get;

    


    
    
    
    dnx_data_dnx2_pemla_soc.soc.feature_get = dnx_data_dnx2_pemla_soc_soc_feature_get;

    
    dnx_data_dnx2_pemla_soc.soc.JR1_MODE_get = dnx_data_dnx2_pemla_soc_soc_JR1_MODE_get;
    dnx_data_dnx2_pemla_soc.soc.JR2_MODE_get = dnx_data_dnx2_pemla_soc_soc_JR2_MODE_get;
    dnx_data_dnx2_pemla_soc.soc.system_headers_mode_get = dnx_data_dnx2_pemla_soc_soc_system_headers_mode_get;

    

    
    
    
    dnx_data_dram.hbm.feature_get = dnx_data_dram_hbm_feature_get;

    
    dnx_data_dram.hbm.burst_length_get = dnx_data_dram_hbm_burst_length_get;
    dnx_data_dram.hbm.stop_traffic_temp_threshold_get = dnx_data_dram_hbm_stop_traffic_temp_threshold_get;
    dnx_data_dram.hbm.stop_traffic_low_temp_threshold_get = dnx_data_dram_hbm_stop_traffic_low_temp_threshold_get;
    dnx_data_dram.hbm.restore_traffic_temp_threshold_get = dnx_data_dram_hbm_restore_traffic_temp_threshold_get;
    dnx_data_dram.hbm.restore_traffic_low_temp_threshold_get = dnx_data_dram_hbm_restore_traffic_low_temp_threshold_get;
    dnx_data_dram.hbm.usec_between_temp_samples_get = dnx_data_dram_hbm_usec_between_temp_samples_get;
    dnx_data_dram.hbm.power_down_temp_threshold_get = dnx_data_dram_hbm_power_down_temp_threshold_get;
    dnx_data_dram.hbm.power_down_low_temp_threshold_get = dnx_data_dram_hbm_power_down_low_temp_threshold_get;
    dnx_data_dram.hbm.dram_temp_monitor_enable_get = dnx_data_dram_hbm_dram_temp_monitor_enable_get;
    dnx_data_dram.hbm.start_disabled_get = dnx_data_dram_hbm_start_disabled_get;
    dnx_data_dram.hbm.output_enable_length_get = dnx_data_dram_hbm_output_enable_length_get;
    dnx_data_dram.hbm.output_enable_delay_get = dnx_data_dram_hbm_output_enable_delay_get;
    dnx_data_dram.hbm.driver_strength_get = dnx_data_dram_hbm_driver_strength_get;
    dnx_data_dram.hbm.t_rdlat_offset_get = dnx_data_dram_hbm_t_rdlat_offset_get;
    dnx_data_dram.hbm.sbus_execute_delay_default_get = dnx_data_dram_hbm_sbus_execute_delay_default_get;
    dnx_data_dram.hbm.enable_dram_bist_on_init_get = dnx_data_dram_hbm_enable_dram_bist_on_init_get;
    dnx_data_dram.hbm.default_model_part_num_get = dnx_data_dram_hbm_default_model_part_num_get;
    dnx_data_dram.hbm.nof_channel_dwords_get = dnx_data_dram_hbm_nof_channel_dwords_get;
    dnx_data_dram.hbm.wds_size_get = dnx_data_dram_hbm_wds_size_get;

    
    dnx_data_dram.hbm.channel_symmetric_regs_get = dnx_data_dram_hbm_channel_symmetric_regs_get;
    dnx_data_dram.hbm.channel_symmetric_regs_info_get = dnx_data_dram_hbm_channel_symmetric_regs_info_get;
    dnx_data_dram.hbm.channel_not_symmetric_regs_get = dnx_data_dram_hbm_channel_not_symmetric_regs_get;
    dnx_data_dram.hbm.channel_not_symmetric_regs_info_get = dnx_data_dram_hbm_channel_not_symmetric_regs_info_get;
    dnx_data_dram.hbm.controller_symmetric_regs_get = dnx_data_dram_hbm_controller_symmetric_regs_get;
    dnx_data_dram.hbm.controller_symmetric_regs_info_get = dnx_data_dram_hbm_controller_symmetric_regs_info_get;
    dnx_data_dram.hbm.controller_not_symmetric_regs_get = dnx_data_dram_hbm_controller_not_symmetric_regs_get;
    dnx_data_dram.hbm.controller_not_symmetric_regs_info_get = dnx_data_dram_hbm_controller_not_symmetric_regs_info_get;
    dnx_data_dram.hbm.channel_interrupt_regs_get = dnx_data_dram_hbm_channel_interrupt_regs_get;
    dnx_data_dram.hbm.channel_interrupt_regs_info_get = dnx_data_dram_hbm_channel_interrupt_regs_info_get;
    dnx_data_dram.hbm.controller_interrupt_regs_get = dnx_data_dram_hbm_controller_interrupt_regs_get;
    dnx_data_dram.hbm.controller_interrupt_regs_info_get = dnx_data_dram_hbm_controller_interrupt_regs_info_get;
    dnx_data_dram.hbm.channel_debug_regs_get = dnx_data_dram_hbm_channel_debug_regs_get;
    dnx_data_dram.hbm.channel_debug_regs_info_get = dnx_data_dram_hbm_channel_debug_regs_info_get;
    dnx_data_dram.hbm.channel_counter_regs_get = dnx_data_dram_hbm_channel_counter_regs_get;
    dnx_data_dram.hbm.channel_counter_regs_info_get = dnx_data_dram_hbm_channel_counter_regs_info_get;
    dnx_data_dram.hbm.channel_type_regs_get = dnx_data_dram_hbm_channel_type_regs_get;
    dnx_data_dram.hbm.channel_type_regs_info_get = dnx_data_dram_hbm_channel_type_regs_info_get;
    dnx_data_dram.hbm.controller_info_regs_get = dnx_data_dram_hbm_controller_info_regs_get;
    dnx_data_dram.hbm.controller_info_regs_info_get = dnx_data_dram_hbm_controller_info_regs_info_get;
    dnx_data_dram.hbm.bist_get = dnx_data_dram_hbm_bist_get;
    dnx_data_dram.hbm.bist_info_get = dnx_data_dram_hbm_bist_info_get;
    dnx_data_dram.hbm.hbc_last_in_chain_get = dnx_data_dram_hbm_hbc_last_in_chain_get;
    dnx_data_dram.hbm.hbc_last_in_chain_info_get = dnx_data_dram_hbm_hbc_last_in_chain_info_get;
    dnx_data_dram.hbm.hbc_sbus_chain_get = dnx_data_dram_hbm_hbc_sbus_chain_get;
    dnx_data_dram.hbm.hbc_sbus_chain_info_get = dnx_data_dram_hbm_hbc_sbus_chain_info_get;
    dnx_data_dram.hbm.dll_and_qc_conf_vals_per_freq_range_get = dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_get;
    dnx_data_dram.hbm.dll_and_qc_conf_vals_per_freq_range_info_get = dnx_data_dram_hbm_dll_and_qc_conf_vals_per_freq_range_info_get;
    
    
    dnx_data_dram.gddr6.feature_get = dnx_data_dram_gddr6_feature_get;

    
    dnx_data_dram.gddr6.nof_ca_bits_get = dnx_data_dram_gddr6_nof_ca_bits_get;
    dnx_data_dram.gddr6.bytes_per_channel_get = dnx_data_dram_gddr6_bytes_per_channel_get;
    dnx_data_dram.gddr6.training_fifo_depth_get = dnx_data_dram_gddr6_training_fifo_depth_get;
    dnx_data_dram.gddr6.readout_to_readout_prd_get = dnx_data_dram_gddr6_readout_to_readout_prd_get;
    dnx_data_dram.gddr6.refresh_to_readout_prd_get = dnx_data_dram_gddr6_refresh_to_readout_prd_get;
    dnx_data_dram.gddr6.readout_done_to_done_prd_get = dnx_data_dram_gddr6_readout_done_to_done_prd_get;
    dnx_data_dram.gddr6.refresh_mechanism_enable_get = dnx_data_dram_gddr6_refresh_mechanism_enable_get;
    dnx_data_dram.gddr6.bist_enable_get = dnx_data_dram_gddr6_bist_enable_get;
    dnx_data_dram.gddr6.dynamic_calibration_enable_get = dnx_data_dram_gddr6_dynamic_calibration_enable_get;
    dnx_data_dram.gddr6.cdr_enable_get = dnx_data_dram_gddr6_cdr_enable_get;
    dnx_data_dram.gddr6.write_recovery_get = dnx_data_dram_gddr6_write_recovery_get;
    dnx_data_dram.gddr6.cabi_get = dnx_data_dram_gddr6_cabi_get;
    dnx_data_dram.gddr6.dram_mode_get = dnx_data_dram_gddr6_dram_mode_get;
    dnx_data_dram.gddr6.command_pipe_extra_delay_get = dnx_data_dram_gddr6_command_pipe_extra_delay_get;
    dnx_data_dram.gddr6.use_11bits_ca_get = dnx_data_dram_gddr6_use_11bits_ca_get;
    dnx_data_dram.gddr6.ck_odt_get = dnx_data_dram_gddr6_ck_odt_get;
    dnx_data_dram.gddr6.dynamic_calibration_period_get = dnx_data_dram_gddr6_dynamic_calibration_period_get;
    dnx_data_dram.gddr6.ck_termination_mr_conf_val_get = dnx_data_dram_gddr6_ck_termination_mr_conf_val_get;
    dnx_data_dram.gddr6.ck_termination_ca_conf_val_get = dnx_data_dram_gddr6_ck_termination_ca_conf_val_get;
    dnx_data_dram.gddr6.wck_granularity_get = dnx_data_dram_gddr6_wck_granularity_get;
    dnx_data_dram.gddr6.ref_clk_bitmap_get = dnx_data_dram_gddr6_ref_clk_bitmap_get;
    dnx_data_dram.gddr6.ca_termination_conf_val_get = dnx_data_dram_gddr6_ca_termination_conf_val_get;

    
    dnx_data_dram.gddr6.refresh_intervals_get = dnx_data_dram_gddr6_refresh_intervals_get;
    dnx_data_dram.gddr6.refresh_intervals_info_get = dnx_data_dram_gddr6_refresh_intervals_info_get;
    dnx_data_dram.gddr6.dq_map_get = dnx_data_dram_gddr6_dq_map_get;
    dnx_data_dram.gddr6.dq_map_info_get = dnx_data_dram_gddr6_dq_map_info_get;
    dnx_data_dram.gddr6.dq_channel_swap_get = dnx_data_dram_gddr6_dq_channel_swap_get;
    dnx_data_dram.gddr6.dq_channel_swap_info_get = dnx_data_dram_gddr6_dq_channel_swap_info_get;
    dnx_data_dram.gddr6.dq_byte_map_get = dnx_data_dram_gddr6_dq_byte_map_get;
    dnx_data_dram.gddr6.dq_byte_map_info_get = dnx_data_dram_gddr6_dq_byte_map_info_get;
    dnx_data_dram.gddr6.ca_map_get = dnx_data_dram_gddr6_ca_map_get;
    dnx_data_dram.gddr6.ca_map_info_get = dnx_data_dram_gddr6_ca_map_info_get;
    dnx_data_dram.gddr6.cadt_byte_map_get = dnx_data_dram_gddr6_cadt_byte_map_get;
    dnx_data_dram.gddr6.cadt_byte_map_info_get = dnx_data_dram_gddr6_cadt_byte_map_info_get;
    dnx_data_dram.gddr6.channel_regs_get = dnx_data_dram_gddr6_channel_regs_get;
    dnx_data_dram.gddr6.channel_regs_info_get = dnx_data_dram_gddr6_channel_regs_info_get;
    dnx_data_dram.gddr6.controller_regs_get = dnx_data_dram_gddr6_controller_regs_get;
    dnx_data_dram.gddr6.controller_regs_info_get = dnx_data_dram_gddr6_controller_regs_info_get;
    dnx_data_dram.gddr6.channel_interrupt_regs_get = dnx_data_dram_gddr6_channel_interrupt_regs_get;
    dnx_data_dram.gddr6.channel_interrupt_regs_info_get = dnx_data_dram_gddr6_channel_interrupt_regs_info_get;
    dnx_data_dram.gddr6.controller_interrupt_regs_get = dnx_data_dram_gddr6_controller_interrupt_regs_get;
    dnx_data_dram.gddr6.controller_interrupt_regs_info_get = dnx_data_dram_gddr6_controller_interrupt_regs_info_get;
    dnx_data_dram.gddr6.channel_debug_regs_get = dnx_data_dram_gddr6_channel_debug_regs_get;
    dnx_data_dram.gddr6.channel_debug_regs_info_get = dnx_data_dram_gddr6_channel_debug_regs_info_get;
    dnx_data_dram.gddr6.channel_counter_regs_get = dnx_data_dram_gddr6_channel_counter_regs_get;
    dnx_data_dram.gddr6.channel_counter_regs_info_get = dnx_data_dram_gddr6_channel_counter_regs_info_get;
    dnx_data_dram.gddr6.channel_type_regs_get = dnx_data_dram_gddr6_channel_type_regs_get;
    dnx_data_dram.gddr6.channel_type_regs_info_get = dnx_data_dram_gddr6_channel_type_regs_info_get;
    dnx_data_dram.gddr6.controller_info_regs_get = dnx_data_dram_gddr6_controller_info_regs_get;
    dnx_data_dram.gddr6.controller_info_regs_info_get = dnx_data_dram_gddr6_controller_info_regs_info_get;
    dnx_data_dram.gddr6.master_phy_get = dnx_data_dram_gddr6_master_phy_get;
    dnx_data_dram.gddr6.master_phy_info_get = dnx_data_dram_gddr6_master_phy_info_get;
    dnx_data_dram.gddr6.ca_system_config_fields_get = dnx_data_dram_gddr6_ca_system_config_fields_get;
    dnx_data_dram.gddr6.ca_system_config_fields_info_get = dnx_data_dram_gddr6_ca_system_config_fields_info_get;
    
    
    dnx_data_dram.general_info.feature_get = dnx_data_dram_general_info_feature_get;

    
    dnx_data_dram.general_info.otp_restore_version_get = dnx_data_dram_general_info_otp_restore_version_get;
    dnx_data_dram.general_info.max_nof_drams_get = dnx_data_dram_general_info_max_nof_drams_get;
    dnx_data_dram.general_info.nof_channels_get = dnx_data_dram_general_info_nof_channels_get;
    dnx_data_dram.general_info.mr_mask_get = dnx_data_dram_general_info_mr_mask_get;
    dnx_data_dram.general_info.nof_mrs_get = dnx_data_dram_general_info_nof_mrs_get;
    dnx_data_dram.general_info.phy_address_mask_get = dnx_data_dram_general_info_phy_address_mask_get;
    dnx_data_dram.general_info.max_dram_index_get = dnx_data_dram_general_info_max_dram_index_get;
    dnx_data_dram.general_info.min_dram_index_get = dnx_data_dram_general_info_min_dram_index_get;
    dnx_data_dram.general_info.frequency_get = dnx_data_dram_general_info_frequency_get;
    dnx_data_dram.general_info.buffer_size_get = dnx_data_dram_general_info_buffer_size_get;
    dnx_data_dram.general_info.command_address_parity_get = dnx_data_dram_general_info_command_address_parity_get;
    dnx_data_dram.general_info.dq_write_parity_get = dnx_data_dram_general_info_dq_write_parity_get;
    dnx_data_dram.general_info.dq_read_parity_get = dnx_data_dram_general_info_dq_read_parity_get;
    dnx_data_dram.general_info.dbi_read_get = dnx_data_dram_general_info_dbi_read_get;
    dnx_data_dram.general_info.dbi_write_get = dnx_data_dram_general_info_dbi_write_get;
    dnx_data_dram.general_info.write_latency_get = dnx_data_dram_general_info_write_latency_get;
    dnx_data_dram.general_info.write_latency_hbm2e_get = dnx_data_dram_general_info_write_latency_hbm2e_get;
    dnx_data_dram.general_info.read_latency_get = dnx_data_dram_general_info_read_latency_get;
    dnx_data_dram.general_info.read_latency_hbm2e_get = dnx_data_dram_general_info_read_latency_hbm2e_get;
    dnx_data_dram.general_info.read_data_enable_delay_get = dnx_data_dram_general_info_read_data_enable_delay_get;
    dnx_data_dram.general_info.read_data_enable_length_get = dnx_data_dram_general_info_read_data_enable_length_get;
    dnx_data_dram.general_info.parity_latency_get = dnx_data_dram_general_info_parity_latency_get;
    dnx_data_dram.general_info.actual_parity_latency_get = dnx_data_dram_general_info_actual_parity_latency_get;
    dnx_data_dram.general_info.tune_mode_on_init_get = dnx_data_dram_general_info_tune_mode_on_init_get;
    dnx_data_dram.general_info.command_parity_latency_get = dnx_data_dram_general_info_command_parity_latency_get;
    dnx_data_dram.general_info.crc_write_latency_get = dnx_data_dram_general_info_crc_write_latency_get;
    dnx_data_dram.general_info.crc_read_latency_get = dnx_data_dram_general_info_crc_read_latency_get;
    dnx_data_dram.general_info.crc_write_get = dnx_data_dram_general_info_crc_write_get;
    dnx_data_dram.general_info.crc_read_get = dnx_data_dram_general_info_crc_read_get;
    dnx_data_dram.general_info.device_size_get = dnx_data_dram_general_info_device_size_get;
    dnx_data_dram.general_info.supported_dram_bitmap_get = dnx_data_dram_general_info_supported_dram_bitmap_get;
    dnx_data_dram.general_info.dram_bitmap_internal_get = dnx_data_dram_general_info_dram_bitmap_internal_get;
    dnx_data_dram.general_info.tertw_get = dnx_data_dram_general_info_tertw_get;
    dnx_data_dram.general_info.dram_blocks_in_power_down_get = dnx_data_dram_general_info_dram_blocks_in_power_down_get;

    
    dnx_data_dram.general_info.mr_defaults_get = dnx_data_dram_general_info_mr_defaults_get;
    dnx_data_dram.general_info.mr_defaults_info_get = dnx_data_dram_general_info_mr_defaults_info_get;
    dnx_data_dram.general_info.dram_info_get = dnx_data_dram_general_info_dram_info_get;
    dnx_data_dram.general_info.dram_info_info_get = dnx_data_dram_general_info_dram_info_info_get;
    dnx_data_dram.general_info.timing_params_get = dnx_data_dram_general_info_timing_params_get;
    dnx_data_dram.general_info.timing_params_info_get = dnx_data_dram_general_info_timing_params_info_get;
    dnx_data_dram.general_info.refresh_intervals_get = dnx_data_dram_general_info_refresh_intervals_get;
    dnx_data_dram.general_info.refresh_intervals_info_get = dnx_data_dram_general_info_refresh_intervals_info_get;
    
    
    dnx_data_dram.address_translation.feature_get = dnx_data_dram_address_translation_feature_get;

    
    dnx_data_dram.address_translation.matrix_column_size_get = dnx_data_dram_address_translation_matrix_column_size_get;
    dnx_data_dram.address_translation.physical_address_transaction_size_get = dnx_data_dram_address_translation_physical_address_transaction_size_get;
    dnx_data_dram.address_translation.nof_atms_get = dnx_data_dram_address_translation_nof_atms_get;
    dnx_data_dram.address_translation.nof_tdus_per_dram_get = dnx_data_dram_address_translation_nof_tdus_per_dram_get;
    dnx_data_dram.address_translation.max_tdu_index_get = dnx_data_dram_address_translation_max_tdu_index_get;

    
    dnx_data_dram.address_translation.tdu_map_get = dnx_data_dram_address_translation_tdu_map_get;
    dnx_data_dram.address_translation.tdu_map_info_get = dnx_data_dram_address_translation_tdu_map_info_get;
    dnx_data_dram.address_translation.matrix_configuration_get = dnx_data_dram_address_translation_matrix_configuration_get;
    dnx_data_dram.address_translation.matrix_configuration_info_get = dnx_data_dram_address_translation_matrix_configuration_info_get;
    dnx_data_dram.address_translation.interrupt_regs_get = dnx_data_dram_address_translation_interrupt_regs_get;
    dnx_data_dram.address_translation.interrupt_regs_info_get = dnx_data_dram_address_translation_interrupt_regs_info_get;
    dnx_data_dram.address_translation.counter_regs_get = dnx_data_dram_address_translation_counter_regs_get;
    dnx_data_dram.address_translation.counter_regs_info_get = dnx_data_dram_address_translation_counter_regs_info_get;
    
    
    dnx_data_dram.buffers.feature_get = dnx_data_dram_buffers_feature_get;

    
    dnx_data_dram.buffers.allowed_errors_get = dnx_data_dram_buffers_allowed_errors_get;
    dnx_data_dram.buffers.nof_bdbs_get = dnx_data_dram_buffers_nof_bdbs_get;
    dnx_data_dram.buffers.nof_fpc_banks_get = dnx_data_dram_buffers_nof_fpc_banks_get;

    
    dnx_data_dram.buffers.deleted_buffers_info_get = dnx_data_dram_buffers_deleted_buffers_info_get;
    dnx_data_dram.buffers.deleted_buffers_info_info_get = dnx_data_dram_buffers_deleted_buffers_info_info_get;
    
    
    dnx_data_dram.dram_block.feature_get = dnx_data_dram_dram_block_feature_get;

    
    dnx_data_dram.dram_block.leaky_bucket_window_size_get = dnx_data_dram_dram_block_leaky_bucket_window_size_get;
    dnx_data_dram.dram_block.wmr_decrement_thr_factor_get = dnx_data_dram_dram_block_wmr_decrement_thr_factor_get;
    dnx_data_dram.dram_block.wpr_increment_thr_factor_get = dnx_data_dram_dram_block_wpr_increment_thr_factor_get;

    
    dnx_data_dram.dram_block.dram_block_params_get = dnx_data_dram_dram_block_dram_block_params_get;
    dnx_data_dram.dram_block.dram_block_params_info_get = dnx_data_dram_dram_block_dram_block_params_info_get;
    
    
    dnx_data_dram.dbal.feature_get = dnx_data_dram_dbal_feature_get;

    
    dnx_data_dram.dbal.hbm_trc_nof_bits_get = dnx_data_dram_dbal_hbm_trc_nof_bits_get;
    dnx_data_dram.dbal.wpr_increment_threshold_nof_bits_get = dnx_data_dram_dbal_wpr_increment_threshold_nof_bits_get;
    dnx_data_dram.dbal.average_read_inflights_increment_threshold_nof_bits_get = dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_get;
    dnx_data_dram.dbal.average_read_inflights_decrement_threshold_nof_bits_get = dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_get;
    dnx_data_dram.dbal.hbm_pll_pdiv_nof_bits_get = dnx_data_dram_dbal_hbm_pll_pdiv_nof_bits_get;
    dnx_data_dram.dbal.hbm_pll_ch_mdiv_nof_bits_get = dnx_data_dram_dbal_hbm_pll_ch_mdiv_nof_bits_get;
    dnx_data_dram.dbal.hbm_pll_frefeff_info_nof_bits_get = dnx_data_dram_dbal_hbm_pll_frefeff_info_nof_bits_get;
    dnx_data_dram.dbal.hbm_pll_aux_post_enableb_nof_bits_get = dnx_data_dram_dbal_hbm_pll_aux_post_enableb_nof_bits_get;
    dnx_data_dram.dbal.hbm_pll_ch_enableb_nof_bits_get = dnx_data_dram_dbal_hbm_pll_ch_enableb_nof_bits_get;
    dnx_data_dram.dbal.hbm_pll_aux_post_diffcmos_en_nof_bits_get = dnx_data_dram_dbal_hbm_pll_aux_post_diffcmos_en_nof_bits_get;
    dnx_data_dram.dbal.dram_bist_mode_nof_bits_get = dnx_data_dram_dbal_dram_bist_mode_nof_bits_get;
    dnx_data_dram.dbal.hbm_write_latency_nof_bits_get = dnx_data_dram_dbal_hbm_write_latency_nof_bits_get;
    dnx_data_dram.dbal.hbm_output_enable_delay_nof_bits_get = dnx_data_dram_dbal_hbm_output_enable_delay_nof_bits_get;
    dnx_data_dram.dbal.hbm_rd_data_en_delay_nof_bits_get = dnx_data_dram_dbal_hbm_rd_data_en_delay_nof_bits_get;
    dnx_data_dram.dbal.hbm_c_u_nof_bits_get = dnx_data_dram_dbal_hbm_c_u_nof_bits_get;
    dnx_data_dram.dbal.hbm_r_u_nof_bits_get = dnx_data_dram_dbal_hbm_r_u_nof_bits_get;
    dnx_data_dram.dbal.hbmc_index_nof_bits_get = dnx_data_dram_dbal_hbmc_index_nof_bits_get;
    dnx_data_dram.dbal.hbmc_tdu_index_nof_bits_get = dnx_data_dram_dbal_hbmc_tdu_index_nof_bits_get;

    
    
    
    dnx_data_dram.firmware.feature_get = dnx_data_dram_firmware_feature_get;

    
    dnx_data_dram.firmware.nof_hbm_spare_data_results_get = dnx_data_dram_firmware_nof_hbm_spare_data_results_get;
    dnx_data_dram.firmware.sbus_clock_divider_get = dnx_data_dram_firmware_sbus_clock_divider_get;
    dnx_data_dram.firmware.snap_state_init_done_get = dnx_data_dram_firmware_snap_state_init_done_get;
    dnx_data_dram.firmware.max_timeout_get = dnx_data_dram_firmware_max_timeout_get;

    
    dnx_data_dram.firmware.rom_get = dnx_data_dram_firmware_rom_get;
    dnx_data_dram.firmware.rom_info_get = dnx_data_dram_firmware_rom_info_get;
    dnx_data_dram.firmware.operation_status_get = dnx_data_dram_firmware_operation_status_get;
    dnx_data_dram.firmware.operation_status_info_get = dnx_data_dram_firmware_operation_status_info_get;
    dnx_data_dram.firmware.operation_result_error_code_get = dnx_data_dram_firmware_operation_result_error_code_get;
    dnx_data_dram.firmware.operation_result_error_code_info_get = dnx_data_dram_firmware_operation_result_error_code_info_get;
    dnx_data_dram.firmware.operation_result_last_operation_get = dnx_data_dram_firmware_operation_result_last_operation_get;
    dnx_data_dram.firmware.operation_result_last_operation_info_get = dnx_data_dram_firmware_operation_result_last_operation_info_get;
    
    
    dnx_data_dram.apd_phy.feature_get = dnx_data_dram_apd_phy_feature_get;

    

    
    dnx_data_dram.apd_phy.pll_get = dnx_data_dram_apd_phy_pll_get;
    dnx_data_dram.apd_phy.pll_info_get = dnx_data_dram_apd_phy_pll_info_get;
    
    
    dnx_data_dram.ctests.feature_get = dnx_data_dram_ctests_feature_get;

    

    
    dnx_data_dram.ctests.rate_get = dnx_data_dram_ctests_rate_get;
    dnx_data_dram.ctests.rate_info_get = dnx_data_dram_ctests_rate_info_get;
    
    
    
    dnx_data_ecgm.general.feature_get = dnx_data_ecgm_general_feature_get;

    
    dnx_data_ecgm.general.ecgm_is_supported_get = dnx_data_ecgm_general_ecgm_is_supported_get;
    dnx_data_ecgm.general.max_fadt_references_get = dnx_data_ecgm_general_max_fadt_references_get;

    
    
    
    dnx_data_ecgm.core_resources.feature_get = dnx_data_ecgm_core_resources_feature_get;

    
    dnx_data_ecgm.core_resources.nof_sp_get = dnx_data_ecgm_core_resources_nof_sp_get;
    dnx_data_ecgm.core_resources.nof_interface_profiles_get = dnx_data_ecgm_core_resources_nof_interface_profiles_get;
    dnx_data_ecgm.core_resources.nof_port_profiles_get = dnx_data_ecgm_core_resources_nof_port_profiles_get;
    dnx_data_ecgm.core_resources.total_pds_get = dnx_data_ecgm_core_resources_total_pds_get;
    dnx_data_ecgm.core_resources.total_pds_nof_bits_get = dnx_data_ecgm_core_resources_total_pds_nof_bits_get;
    dnx_data_ecgm.core_resources.total_dbs_get = dnx_data_ecgm_core_resources_total_dbs_get;
    dnx_data_ecgm.core_resources.total_dbs_nof_bits_get = dnx_data_ecgm_core_resources_total_dbs_nof_bits_get;
    dnx_data_ecgm.core_resources.reserved_dbs_per_sp_nof_bits_get = dnx_data_ecgm_core_resources_reserved_dbs_per_sp_nof_bits_get;
    dnx_data_ecgm.core_resources.max_alpha_value_get = dnx_data_ecgm_core_resources_max_alpha_value_get;
    dnx_data_ecgm.core_resources.min_alpha_value_get = dnx_data_ecgm_core_resources_min_alpha_value_get;
    dnx_data_ecgm.core_resources.max_core_bandwidth_Mbps_get = dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_get;
    dnx_data_ecgm.core_resources.mc_rcy_reserved_interface_th_ids_get = dnx_data_ecgm_core_resources_mc_rcy_reserved_interface_th_ids_get;
    dnx_data_ecgm.core_resources.mc_rcy_reserved_port_th_ids_get = dnx_data_ecgm_core_resources_mc_rcy_reserved_port_th_ids_get;
    dnx_data_ecgm.core_resources.nof_tc_dp_admission_profiles_get = dnx_data_ecgm_core_resources_nof_tc_dp_admission_profiles_get;

    
    
    
    dnx_data_ecgm.port_resources.feature_get = dnx_data_ecgm_port_resources_feature_get;

    
    dnx_data_ecgm.port_resources.max_nof_ports_get = dnx_data_ecgm_port_resources_max_nof_ports_get;

    
    
    
    dnx_data_ecgm.queue_resources.feature_get = dnx_data_ecgm_queue_resources_feature_get;

    

    
    
    
    dnx_data_ecgm.interface_resources.feature_get = dnx_data_ecgm_interface_resources_feature_get;

    

    
    
    
    dnx_data_ecgm.delete_fifo.feature_get = dnx_data_ecgm_delete_fifo_feature_get;

    

    
    dnx_data_ecgm.delete_fifo.thresholds_get = dnx_data_ecgm_delete_fifo_thresholds_get;
    dnx_data_ecgm.delete_fifo.thresholds_info_get = dnx_data_ecgm_delete_fifo_thresholds_info_get;
    
    
    dnx_data_ecgm.info.feature_get = dnx_data_ecgm_info_feature_get;

    
    dnx_data_ecgm.info.fc_q_pair_vector_select_nof_bits_get = dnx_data_ecgm_info_fc_q_pair_vector_select_nof_bits_get;
    dnx_data_ecgm.info.fc_q_pair_vector_nof_presented_get = dnx_data_ecgm_info_fc_q_pair_vector_nof_presented_get;
    dnx_data_ecgm.info.fc_egr_if_vector_select_nof_bits_get = dnx_data_ecgm_info_fc_egr_if_vector_select_nof_bits_get;
    dnx_data_ecgm.info.fc_egr_if_vector_nof_presented_get = dnx_data_ecgm_info_fc_egr_if_vector_nof_presented_get;
    dnx_data_ecgm.info.nof_dropped_reasons_rqp_get = dnx_data_ecgm_info_nof_dropped_reasons_rqp_get;
    dnx_data_ecgm.info.nof_dropped_reasons_pqp_get = dnx_data_ecgm_info_nof_dropped_reasons_pqp_get;
    dnx_data_ecgm.info.nof_bits_in_pd_count_get = dnx_data_ecgm_info_nof_bits_in_pd_count_get;

    
    dnx_data_ecgm.info.dropped_reason_rqp_get = dnx_data_ecgm_info_dropped_reason_rqp_get;
    dnx_data_ecgm.info.dropped_reason_rqp_info_get = dnx_data_ecgm_info_dropped_reason_rqp_info_get;
    dnx_data_ecgm.info.dropped_reason_pqp_get = dnx_data_ecgm_info_dropped_reason_pqp_get;
    dnx_data_ecgm.info.dropped_reason_pqp_info_get = dnx_data_ecgm_info_dropped_reason_pqp_info_get;
    
    
    
    dnx_data_egr_queuing.params.feature_get = dnx_data_egr_queuing_params_feature_get;

    
    dnx_data_egr_queuing.params.nof_bits_in_tcg_weight_get = dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_get;
    dnx_data_egr_queuing.params.nof_bits_in_nof_q_pairs_in_ps_get = dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_get;
    dnx_data_egr_queuing.params.nof_q_pairs_in_ps_get = dnx_data_egr_queuing_params_nof_q_pairs_in_ps_get;
    dnx_data_egr_queuing.params.nof_bits_in_q_pair_get = dnx_data_egr_queuing_params_nof_bits_in_q_pair_get;
    dnx_data_egr_queuing.params.nof_q_pairs_get = dnx_data_egr_queuing_params_nof_q_pairs_get;
    dnx_data_egr_queuing.params.nof_bits_in_nof_port_schedulers_get = dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_get;
    dnx_data_egr_queuing.params.nof_port_schedulers_get = dnx_data_egr_queuing_params_nof_port_schedulers_get;
    dnx_data_egr_queuing.params.nof_bits_in_channel_get = dnx_data_egr_queuing_params_nof_bits_in_channel_get;
    dnx_data_egr_queuing.params.nof_bits_in_chan_arb_calendar_size_get = dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_get;
    dnx_data_egr_queuing.params.chan_arb_calendar_size_get = dnx_data_egr_queuing_params_chan_arb_calendar_size_get;
    dnx_data_egr_queuing.params.nof_bits_in_port_prio_calendar_size_get = dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_get;
    dnx_data_egr_queuing.params.port_prio_calendar_size_get = dnx_data_egr_queuing_params_port_prio_calendar_size_get;
    dnx_data_egr_queuing.params.nof_bits_in_tcg_calendar_size_get = dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_get;
    dnx_data_egr_queuing.params.tcg_calendar_size_get = dnx_data_egr_queuing_params_tcg_calendar_size_get;
    dnx_data_egr_queuing.params.nof_bits_in_nof_calendars_get = dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_get;
    dnx_data_egr_queuing.params.nof_calendars_get = dnx_data_egr_queuing_params_nof_calendars_get;
    dnx_data_egr_queuing.params.nof_bits_in_egr_interface_get = dnx_data_egr_queuing_params_nof_bits_in_egr_interface_get;
    dnx_data_egr_queuing.params.nof_bits_mirror_prio_get = dnx_data_egr_queuing_params_nof_bits_mirror_prio_get;
    dnx_data_egr_queuing.params.nof_bits_in_egr_ch_interface_get = dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_get;
    dnx_data_egr_queuing.params.nof_egr_interfaces_get = dnx_data_egr_queuing_params_nof_egr_interfaces_get;
    dnx_data_egr_queuing.params.first_lp_egr_interface_get = dnx_data_egr_queuing_params_first_lp_egr_interface_get;
    dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get = dnx_data_egr_queuing_params_nof_egr_ch_interfaces_get;
    dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get = dnx_data_egr_queuing_params_nof_egr_rcy_interfaces_get;
    dnx_data_egr_queuing.params.nof_egr_olp_interfaces_get = dnx_data_egr_queuing_params_nof_egr_olp_interfaces_get;
    dnx_data_egr_queuing.params.reserved_if_get = dnx_data_egr_queuing_params_reserved_if_get;
    dnx_data_egr_queuing.params.egr_if_cpu_get = dnx_data_egr_queuing_params_egr_if_cpu_get;
    dnx_data_egr_queuing.params.egr_if_sat_get = dnx_data_egr_queuing_params_egr_if_sat_get;
    dnx_data_egr_queuing.params.egr_if_oam_get = dnx_data_egr_queuing_params_egr_if_oam_get;
    dnx_data_egr_queuing.params.egr_if_olp_get = dnx_data_egr_queuing_params_egr_if_olp_get;
    dnx_data_egr_queuing.params.egr_if_rcy_get = dnx_data_egr_queuing_params_egr_if_rcy_get;
    dnx_data_egr_queuing.params.egr_if_txi_rcy_get = dnx_data_egr_queuing_params_egr_if_txi_rcy_get;
    dnx_data_egr_queuing.params.egr_if_eventor_get = dnx_data_egr_queuing_params_egr_if_eventor_get;
    dnx_data_egr_queuing.params.egr_if_nif_base_get = dnx_data_egr_queuing_params_egr_if_nif_base_get;
    dnx_data_egr_queuing.params.cal_res_get = dnx_data_egr_queuing_params_cal_res_get;
    dnx_data_egr_queuing.params.cal_res_packet_mode_get = dnx_data_egr_queuing_params_cal_res_packet_mode_get;
    dnx_data_egr_queuing.params.cal_burst_res_get = dnx_data_egr_queuing_params_cal_burst_res_get;
    dnx_data_egr_queuing.params.nof_bits_in_cal_cal_len_get = dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_get;
    dnx_data_egr_queuing.params.cal_cal_len_get = dnx_data_egr_queuing_params_cal_cal_len_get;
    dnx_data_egr_queuing.params.nof_bits_in_nif_cal_len_pqp_get = dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_get;
    dnx_data_egr_queuing.params.nif_cal_len_pqp_get = dnx_data_egr_queuing_params_nif_cal_len_pqp_get;
    dnx_data_egr_queuing.params.nof_bits_in_nif_cal_len_fqp_get = dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_get;
    dnx_data_egr_queuing.params.nif_cal_len_fqp_get = dnx_data_egr_queuing_params_nif_cal_len_fqp_get;
    dnx_data_egr_queuing.params.nof_bits_in_nof_tcg_get = dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_get;
    dnx_data_egr_queuing.params.nof_tcg_get = dnx_data_egr_queuing_params_nof_tcg_get;
    dnx_data_egr_queuing.params.nof_bits_in_nof_egr_q_prio_get = dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_get;
    dnx_data_egr_queuing.params.nof_egr_q_prio_get = dnx_data_egr_queuing_params_nof_egr_q_prio_get;
    dnx_data_egr_queuing.params.tcg_min_priorities_get = dnx_data_egr_queuing_params_tcg_min_priorities_get;
    dnx_data_egr_queuing.params.max_credit_number_get = dnx_data_egr_queuing_params_max_credit_number_get;
    dnx_data_egr_queuing.params.max_gbps_rate_egq_get = dnx_data_egr_queuing_params_max_gbps_rate_egq_get;
    dnx_data_egr_queuing.params.txq_iready_th_get = dnx_data_egr_queuing_params_txq_iready_th_get;
    dnx_data_egr_queuing.params.txq_tdm_iready_th_get = dnx_data_egr_queuing_params_txq_tdm_iready_th_get;
    dnx_data_egr_queuing.params.initial_packet_mode_get = dnx_data_egr_queuing_params_initial_packet_mode_get;
    dnx_data_egr_queuing.params.sub_calendar_ifc_get = dnx_data_egr_queuing_params_sub_calendar_ifc_get;
    dnx_data_egr_queuing.params.txq_max_credits_get = dnx_data_egr_queuing_params_txq_max_credits_get;
    dnx_data_egr_queuing.params.txq_max_credits_reduce_factor_get = dnx_data_egr_queuing_params_txq_max_credits_reduce_factor_get;
    dnx_data_egr_queuing.params.pqp_ifc_almost_empty_th_get = dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_get;
    dnx_data_egr_queuing.params.pqp_port_almost_empty_th_get = dnx_data_egr_queuing_params_pqp_port_almost_empty_th_get;
    dnx_data_egr_queuing.params.pqp_queue_almost_empty_th_get = dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_get;
    dnx_data_egr_queuing.params.if_min_gap_priority_size_get = dnx_data_egr_queuing_params_if_min_gap_priority_size_get;
    dnx_data_egr_queuing.params.egr_if_flexe_base_get = dnx_data_egr_queuing_params_egr_if_flexe_base_get;
    dnx_data_egr_queuing.params.nof_dedicated_flexe_interfaces_get = dnx_data_egr_queuing_params_nof_dedicated_flexe_interfaces_get;
    dnx_data_egr_queuing.params.nof_egr_interfaces_wo_flexe_get = dnx_data_egr_queuing_params_nof_egr_interfaces_wo_flexe_get;
    dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get = dnx_data_egr_queuing_params_egr_if_nif_alloc_start_get;
    dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get = dnx_data_egr_queuing_params_egr_if_nif_alloc_end_get;
    dnx_data_egr_queuing.params.egr_if_ilkn_alloc_start_get = dnx_data_egr_queuing_params_egr_if_ilkn_alloc_start_get;
    dnx_data_egr_queuing.params.egr_if_ilkn_alloc_end_get = dnx_data_egr_queuing_params_egr_if_ilkn_alloc_end_get;
    dnx_data_egr_queuing.params.egq_shaper_max_mbps_get = dnx_data_egr_queuing_params_egq_shaper_max_mbps_get;
    dnx_data_egr_queuing.params.nof_fqp_profiles_get = dnx_data_egr_queuing_params_nof_fqp_profiles_get;
    dnx_data_egr_queuing.params.min_gap_bits_get = dnx_data_egr_queuing_params_min_gap_bits_get;
    dnx_data_egr_queuing.params.bubble_pulse_width_for_polling_get = dnx_data_egr_queuing_params_bubble_pulse_width_for_polling_get;
    dnx_data_egr_queuing.params.invalid_otm_port_get = dnx_data_egr_queuing_params_invalid_otm_port_get;
    dnx_data_egr_queuing.params.burst_size_below_mtu_get = dnx_data_egr_queuing_params_burst_size_below_mtu_get;
    dnx_data_egr_queuing.params.egress_bubble_delay_get = dnx_data_egr_queuing_params_egress_bubble_delay_get;

    
    dnx_data_egr_queuing.params.if_speed_params_get = dnx_data_egr_queuing_params_if_speed_params_get;
    dnx_data_egr_queuing.params.if_speed_params_info_get = dnx_data_egr_queuing_params_if_speed_params_info_get;
    dnx_data_egr_queuing.params.emr_fifo_get = dnx_data_egr_queuing_params_emr_fifo_get;
    dnx_data_egr_queuing.params.emr_fifo_info_get = dnx_data_egr_queuing_params_emr_fifo_info_get;
    dnx_data_egr_queuing.params.if_speed_params_clk_th_get = dnx_data_egr_queuing_params_if_speed_params_clk_th_get;
    dnx_data_egr_queuing.params.if_speed_params_clk_th_info_get = dnx_data_egr_queuing_params_if_speed_params_clk_th_info_get;
    dnx_data_egr_queuing.params.fqp_dbf_additional_latency_get = dnx_data_egr_queuing_params_fqp_dbf_additional_latency_get;
    dnx_data_egr_queuing.params.fqp_dbf_additional_latency_info_get = dnx_data_egr_queuing_params_fqp_dbf_additional_latency_info_get;
    
    
    dnx_data_egr_queuing.common_max_val.feature_get = dnx_data_egr_queuing_common_max_val_feature_get;

    
    dnx_data_egr_queuing.common_max_val.nof_bits_in_calendar_size_get = dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_get;
    dnx_data_egr_queuing.common_max_val.calendar_size_get = dnx_data_egr_queuing_common_max_val_calendar_size_get;
    dnx_data_egr_queuing.common_max_val.slow_port_speed_get = dnx_data_egr_queuing_common_max_val_slow_port_speed_get;
    dnx_data_egr_queuing.common_max_val.mux_calendar_resolution_get = dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_get;

    
    
    
    dnx_data_egr_queuing.rate_measurement.feature_get = dnx_data_egr_queuing_rate_measurement_feature_get;

    
    dnx_data_egr_queuing.rate_measurement.max_interval_get = dnx_data_egr_queuing_rate_measurement_max_interval_get;
    dnx_data_egr_queuing.rate_measurement.promile_factor_get = dnx_data_egr_queuing_rate_measurement_promile_factor_get;
    dnx_data_egr_queuing.rate_measurement.max_granularity_get = dnx_data_egr_queuing_rate_measurement_max_granularity_get;
    dnx_data_egr_queuing.rate_measurement.min_bytes_for_granularity_get = dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_get;

    
    
    
    dnx_data_egr_queuing.phantom_queues.feature_get = dnx_data_egr_queuing_phantom_queues_feature_get;

    
    dnx_data_egr_queuing.phantom_queues.nof_threshold_profiles_get = dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_get;
    dnx_data_egr_queuing.phantom_queues.max_threshold_get = dnx_data_egr_queuing_phantom_queues_max_threshold_get;

    
    
    
    dnx_data_egr_queuing.reassembly.feature_get = dnx_data_egr_queuing_reassembly_feature_get;

    

    
    
    
    dnx_data_egr_queuing.general.feature_get = dnx_data_egr_queuing_general_feature_get;

    

    
    
    
    
    dnx_data_egr_queuing_v1.general.feature_get = dnx_data_egr_queuing_v1_general_feature_get;

    

    
    
    
    dnx_data_egr_queuing_v1.egress_interfaces.feature_get = dnx_data_egr_queuing_v1_egress_interfaces_feature_get;

    
    dnx_data_egr_queuing_v1.egress_interfaces.nof_egr_interfaces_get = dnx_data_egr_queuing_v1_egress_interfaces_nof_egr_interfaces_get;
    dnx_data_egr_queuing_v1.egress_interfaces.nof_channelized_egr_interfaces_get = dnx_data_egr_queuing_v1_egress_interfaces_nof_channelized_egr_interfaces_get;
    dnx_data_egr_queuing_v1.egress_interfaces.cpu_interface_nof_channels_get = dnx_data_egr_queuing_v1_egress_interfaces_cpu_interface_nof_channels_get;
    dnx_data_egr_queuing_v1.egress_interfaces.cpu_nof_tcs_get = dnx_data_egr_queuing_v1_egress_interfaces_cpu_nof_tcs_get;
    dnx_data_egr_queuing_v1.egress_interfaces.rcy_interface_nof_channels_get = dnx_data_egr_queuing_v1_egress_interfaces_rcy_interface_nof_channels_get;
    dnx_data_egr_queuing_v1.egress_interfaces.nof_ipc_bus_interfaces_get = dnx_data_egr_queuing_v1_egress_interfaces_nof_ipc_bus_interfaces_get;

    
    dnx_data_egr_queuing_v1.egress_interfaces.mapping_data_get = dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_get;
    dnx_data_egr_queuing_v1.egress_interfaces.mapping_data_info_get = dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_info_get;
    dnx_data_egr_queuing_v1.egress_interfaces.per_speed_mapping_data_get = dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_get;
    dnx_data_egr_queuing_v1.egress_interfaces.per_speed_mapping_data_info_get = dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_info_get;
    
    
    dnx_data_egr_queuing_v1.egress_queues.feature_get = dnx_data_egr_queuing_v1_egress_queues_feature_get;

    
    dnx_data_egr_queuing_v1.egress_queues.nof_egr_queues_get = dnx_data_egr_queuing_v1_egress_queues_nof_egr_queues_get;
    dnx_data_egr_queuing_v1.egress_queues.nof_egr_priorities_get = dnx_data_egr_queuing_v1_egress_queues_nof_egr_priorities_get;
    dnx_data_egr_queuing_v1.egress_queues.ps_queue_resolution_get = dnx_data_egr_queuing_v1_egress_queues_ps_queue_resolution_get;
    dnx_data_egr_queuing_v1.egress_queues.rcy_reserved_queue_get = dnx_data_egr_queuing_v1_egress_queues_rcy_reserved_queue_get;
    dnx_data_egr_queuing_v1.egress_queues.nof_queue_pairs_get = dnx_data_egr_queuing_v1_egress_queues_nof_queue_pairs_get;
    dnx_data_egr_queuing_v1.egress_queues.total_nof_egr_queues_get = dnx_data_egr_queuing_v1_egress_queues_total_nof_egr_queues_get;

    
    dnx_data_egr_queuing_v1.egress_queues.supported_nof_priorities_get = dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_get;
    dnx_data_egr_queuing_v1.egress_queues.supported_nof_priorities_info_get = dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_info_get;
    
    
    dnx_data_egr_queuing_v1.scheduling.feature_get = dnx_data_egr_queuing_v1_scheduling_feature_get;

    
    dnx_data_egr_queuing_v1.scheduling.nof_interface_calendar_slots_get = dnx_data_egr_queuing_v1_scheduling_nof_interface_calendar_slots_get;

    
    
    
    dnx_data_egr_queuing_v1.shaping.feature_get = dnx_data_egr_queuing_v1_shaping_feature_get;

    
    dnx_data_egr_queuing_v1.shaping.default_max_burst_get = dnx_data_egr_queuing_v1_shaping_default_max_burst_get;
    dnx_data_egr_queuing_v1.shaping.max_burst_value_get = dnx_data_egr_queuing_v1_shaping_max_burst_value_get;
    dnx_data_egr_queuing_v1.shaping.queue_max_burst_value_get = dnx_data_egr_queuing_v1_shaping_queue_max_burst_value_get;
    dnx_data_egr_queuing_v1.shaping.max_interface_credits_value_get = dnx_data_egr_queuing_v1_shaping_max_interface_credits_value_get;
    dnx_data_egr_queuing_v1.shaping.max_port_credits_value_get = dnx_data_egr_queuing_v1_shaping_max_port_credits_value_get;
    dnx_data_egr_queuing_v1.shaping.slots_per_clock_get = dnx_data_egr_queuing_v1_shaping_slots_per_clock_get;
    dnx_data_egr_queuing_v1.shaping.credits_per_bit_get = dnx_data_egr_queuing_v1_shaping_credits_per_bit_get;
    dnx_data_egr_queuing_v1.shaping.credits_per_packet_get = dnx_data_egr_queuing_v1_shaping_credits_per_packet_get;
    dnx_data_egr_queuing_v1.shaping.calcal_channelized_entries_get = dnx_data_egr_queuing_v1_shaping_calcal_channelized_entries_get;
    dnx_data_egr_queuing_v1.shaping.calcal_non_channelized_entries_get = dnx_data_egr_queuing_v1_shaping_calcal_non_channelized_entries_get;
    dnx_data_egr_queuing_v1.shaping.calcal_length_get = dnx_data_egr_queuing_v1_shaping_calcal_length_get;
    dnx_data_egr_queuing_v1.shaping.nof_port_shaper_entries_get = dnx_data_egr_queuing_v1_shaping_nof_port_shaper_entries_get;
    dnx_data_egr_queuing_v1.shaping.port_cal_unused_entry_get = dnx_data_egr_queuing_v1_shaping_port_cal_unused_entry_get;
    dnx_data_egr_queuing_v1.shaping.port_shaper_start_entry_get = dnx_data_egr_queuing_v1_shaping_port_shaper_start_entry_get;
    dnx_data_egr_queuing_v1.shaping.nof_queue_shaper_entries_get = dnx_data_egr_queuing_v1_shaping_nof_queue_shaper_entries_get;
    dnx_data_egr_queuing_v1.shaping.nof_cir_shaper_entries_get = dnx_data_egr_queuing_v1_shaping_nof_cir_shaper_entries_get;
    dnx_data_egr_queuing_v1.shaping.rcy_mirror_multiplier_get = dnx_data_egr_queuing_v1_shaping_rcy_mirror_multiplier_get;
    dnx_data_egr_queuing_v1.shaping.rcy_mirror_max_burst_value_get = dnx_data_egr_queuing_v1_shaping_rcy_mirror_max_burst_value_get;
    dnx_data_egr_queuing_v1.shaping.max_rcy_mirror_credit_value_get = dnx_data_egr_queuing_v1_shaping_max_rcy_mirror_credit_value_get;

    
    
    
    
    dnx_data_elk.general.feature_get = dnx_data_elk_general_feature_get;

    
    dnx_data_elk.general.max_ports_get = dnx_data_elk_general_max_ports_get;
    dnx_data_elk.general.rop_fifo_depth_get = dnx_data_elk_general_rop_fifo_depth_get;
    dnx_data_elk.general.flp_full_treshold_get = dnx_data_elk_general_flp_full_treshold_get;
    dnx_data_elk.general.max_lane_id_get = dnx_data_elk_general_max_lane_id_get;
    dnx_data_elk.general.egw_max_opcode_get = dnx_data_elk_general_egw_max_opcode_get;
    dnx_data_elk.general.max_kbp_instance_get = dnx_data_elk_general_max_kbp_instance_get;
    dnx_data_elk.general.lookup_timeout_delay_get = dnx_data_elk_general_lookup_timeout_delay_get;
    dnx_data_elk.general.trans_timer_delay_get = dnx_data_elk_general_trans_timer_delay_get;
    dnx_data_elk.general.nof_dbs_per_core_get = dnx_data_elk_general_nof_dbs_per_core_get;
    dnx_data_elk.general.device_type_get = dnx_data_elk_general_device_type_get;

    
    
    
    dnx_data_elk.application.feature_get = dnx_data_elk_application_feature_get;

    
    dnx_data_elk.application.large_mc_mode1_get = dnx_data_elk_application_large_mc_mode1_get;
    dnx_data_elk.application.large_mc_mode2_get = dnx_data_elk_application_large_mc_mode2_get;
    dnx_data_elk.application.large_mc_mode3_get = dnx_data_elk_application_large_mc_mode3_get;
    dnx_data_elk.application.optimized_result_get = dnx_data_elk_application_optimized_result_get;
    dnx_data_elk.application.split_rpf_get = dnx_data_elk_application_split_rpf_get;
    dnx_data_elk.application.thread_safety_get = dnx_data_elk_application_thread_safety_get;
    dnx_data_elk.application.ipv4_large_mc_enable_get = dnx_data_elk_application_ipv4_large_mc_enable_get;
    dnx_data_elk.application.ipv6_large_mc_enable_get = dnx_data_elk_application_ipv6_large_mc_enable_get;
    dnx_data_elk.application.ipv4_mc_hash_table_size_get = dnx_data_elk_application_ipv4_mc_hash_table_size_get;
    dnx_data_elk.application.ipv6_mc_hash_table_size_get = dnx_data_elk_application_ipv6_mc_hash_table_size_get;
    dnx_data_elk.application.ipv4_mc_table_size_get = dnx_data_elk_application_ipv4_mc_table_size_get;
    dnx_data_elk.application.ipv6_mc_table_size_get = dnx_data_elk_application_ipv6_mc_table_size_get;

    
    dnx_data_elk.application.db_property_get = dnx_data_elk_application_db_property_get;
    dnx_data_elk.application.db_property_info_get = dnx_data_elk_application_db_property_info_get;
    
    
    dnx_data_elk.connectivity.feature_get = dnx_data_elk_connectivity_feature_get;

    
    dnx_data_elk.connectivity.connect_mode_get = dnx_data_elk_connectivity_connect_mode_get;
    dnx_data_elk.connectivity.blackhole_get = dnx_data_elk_connectivity_blackhole_get;

    
    dnx_data_elk.connectivity.mdio_get = dnx_data_elk_connectivity_mdio_get;
    dnx_data_elk.connectivity.mdio_info_get = dnx_data_elk_connectivity_mdio_info_get;
    dnx_data_elk.connectivity.ilkn_reverse_get = dnx_data_elk_connectivity_ilkn_reverse_get;
    dnx_data_elk.connectivity.ilkn_reverse_info_get = dnx_data_elk_connectivity_ilkn_reverse_info_get;
    dnx_data_elk.connectivity.topology_get = dnx_data_elk_connectivity_topology_get;
    dnx_data_elk.connectivity.topology_info_get = dnx_data_elk_connectivity_topology_info_get;
    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get = dnx_data_elk_connectivity_kbp_serdes_tx_taps_get;
    dnx_data_elk.connectivity.kbp_serdes_tx_taps_info_get = dnx_data_elk_connectivity_kbp_serdes_tx_taps_info_get;
    dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get = dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_get;
    dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_info_get = dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_info_get;
    
    
    dnx_data_elk.recovery.feature_get = dnx_data_elk_recovery_feature_get;

    
    dnx_data_elk.recovery.enable_get = dnx_data_elk_recovery_enable_get;
    dnx_data_elk.recovery.iteration_get = dnx_data_elk_recovery_iteration_get;

    
    
    
    
    dnx_data_esb.general.feature_get = dnx_data_esb_general_feature_get;

    
    dnx_data_esb.general.total_nof_esb_queues_get = dnx_data_esb_general_total_nof_esb_queues_get;
    dnx_data_esb.general.nof_nif_eth_queues_get = dnx_data_esb_general_nof_nif_eth_queues_get;
    dnx_data_esb.general.nof_eth_lp_queues_get = dnx_data_esb_general_nof_eth_lp_queues_get;
    dnx_data_esb.general.nof_nif_ilkn_queues_get = dnx_data_esb_general_nof_nif_ilkn_queues_get;
    dnx_data_esb.general.nof_ilkn_tdm_or_lp_queues_get = dnx_data_esb_general_nof_ilkn_tdm_or_lp_queues_get;
    dnx_data_esb.general.nof_flexe_queues_get = dnx_data_esb_general_nof_flexe_queues_get;
    dnx_data_esb.general.nof_special_queues_get = dnx_data_esb_general_nof_special_queues_get;
    dnx_data_esb.general.nof_reserved_queues_get = dnx_data_esb_general_nof_reserved_queues_get;
    dnx_data_esb.general.nof_fpc_get = dnx_data_esb_general_nof_fpc_get;
    dnx_data_esb.general.total_nof_buffers_get = dnx_data_esb_general_total_nof_buffers_get;
    dnx_data_esb.general.mux_cal_len_get = dnx_data_esb_general_mux_cal_len_get;
    dnx_data_esb.general.mux_cal_window_size_get = dnx_data_esb_general_mux_cal_window_size_get;
    dnx_data_esb.general.nif_credits_resolution_get = dnx_data_esb_general_nif_credits_resolution_get;
    dnx_data_esb.general.nif_credits_rate_resolution_get = dnx_data_esb_general_nif_credits_rate_resolution_get;
    dnx_data_esb.general.fpc_ptr_size_get = dnx_data_esb_general_fpc_ptr_size_get;
    dnx_data_esb.general.nof_buffers_per_fpc_get = dnx_data_esb_general_nof_buffers_per_fpc_get;
    dnx_data_esb.general.l1_mux_cal_res_get = dnx_data_esb_general_l1_mux_cal_res_get;
    dnx_data_esb.general.tm_egr_queuing_mux_cal_res_get = dnx_data_esb_general_tm_egr_queuing_mux_cal_res_get;
    dnx_data_esb.general.flexe_queue_id_base_get = dnx_data_esb_general_flexe_queue_id_base_get;
    dnx_data_esb.general.ilkn_queue_id_base_get = dnx_data_esb_general_ilkn_queue_id_base_get;
    dnx_data_esb.general.ilkn_tdm_or_lp_queue_id_base_get = dnx_data_esb_general_ilkn_tdm_or_lp_queue_id_base_get;
    dnx_data_esb.general.eth_lp_queue_id_base_get = dnx_data_esb_general_eth_lp_queue_id_base_get;
    dnx_data_esb.general.nof_egr_txi_interfaces_get = dnx_data_esb_general_nof_egr_txi_interfaces_get;
    dnx_data_esb.general.max_queue_to_map_to_nif_get = dnx_data_esb_general_max_queue_to_map_to_nif_get;
    dnx_data_esb.general.txi_irdy_th_factor_get = dnx_data_esb_general_txi_irdy_th_factor_get;
    dnx_data_esb.general.txi_irdy_th_divident_get = dnx_data_esb_general_txi_irdy_th_divident_get;
    dnx_data_esb.general.slow_port_dedicated_queue_get = dnx_data_esb_general_slow_port_dedicated_queue_get;
    dnx_data_esb.general.flexe_l1_dest_dedicated_queue_get = dnx_data_esb_general_flexe_l1_dest_dedicated_queue_get;

    
    dnx_data_esb.general.if_esb_params_get = dnx_data_esb_general_if_esb_params_get;
    dnx_data_esb.general.if_esb_params_info_get = dnx_data_esb_general_if_esb_params_info_get;
    
    
    dnx_data_esb.dbal.feature_get = dnx_data_esb_dbal_feature_get;

    
    dnx_data_esb.dbal.pm_internal_port_size_get = dnx_data_esb_dbal_pm_internal_port_size_get;
    dnx_data_esb.dbal.counter_config_mask_size_get = dnx_data_esb_dbal_counter_config_mask_size_get;
    dnx_data_esb.dbal.credit_size_bits_get = dnx_data_esb_dbal_credit_size_bits_get;
    dnx_data_esb.dbal.txi_irdy_thresh_size_get = dnx_data_esb_dbal_txi_irdy_thresh_size_get;

    
    
    
    
    dnx_data_esem.access_cmd.feature_get = dnx_data_esem_access_cmd_feature_get;

    
    dnx_data_esem.access_cmd.nof_esem_accesses_get = dnx_data_esem_access_cmd_nof_esem_accesses_get;
    dnx_data_esem.access_cmd.app_di_id_size_in_bits_get = dnx_data_esem_access_cmd_app_di_id_size_in_bits_get;
    dnx_data_esem.access_cmd.default_native_get = dnx_data_esem_access_cmd_default_native_get;
    dnx_data_esem.access_cmd.sflow_sample_interface_get = dnx_data_esem_access_cmd_sflow_sample_interface_get;
    dnx_data_esem.access_cmd.ip_tunnel_no_action_get = dnx_data_esem_access_cmd_ip_tunnel_no_action_get;
    dnx_data_esem.access_cmd.port_svtag_get = dnx_data_esem_access_cmd_port_svtag_get;
    dnx_data_esem.access_cmd.nof_esem_cmd_default_profiles_get = dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_get;
    dnx_data_esem.access_cmd.nof_cmds_size_in_bits_get = dnx_data_esem_access_cmd_nof_cmds_size_in_bits_get;
    dnx_data_esem.access_cmd.nof_cmds_get = dnx_data_esem_access_cmd_nof_cmds_get;
    dnx_data_esem.access_cmd.min_size_for_esem_cmd_get = dnx_data_esem_access_cmd_min_size_for_esem_cmd_get;
    dnx_data_esem.access_cmd.esem_cmd_arr_prefix_gre_any_ipv4_get = dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_get;
    dnx_data_esem.access_cmd.esem_cmd_arr_prefix_vxlan_gpe_get = dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_get;
    dnx_data_esem.access_cmd.esem_cmd_arr_prefix_ip_tunnel_no_action_get = dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_get;
    dnx_data_esem.access_cmd.esem_cmd_arr_prefix_arp_plus_ac_get = dnx_data_esem_access_cmd_esem_cmd_arr_prefix_arp_plus_ac_get;
    dnx_data_esem.access_cmd.esem_cmd_predefine_allocations_get = dnx_data_esem_access_cmd_esem_cmd_predefine_allocations_get;
    dnx_data_esem.access_cmd.no_action_get = dnx_data_esem_access_cmd_no_action_get;
    dnx_data_esem.access_cmd.default_ac_get = dnx_data_esem_access_cmd_default_ac_get;
    dnx_data_esem.access_cmd.allocation_mngr_default_get = dnx_data_esem_access_cmd_allocation_mngr_default_get;

    
    dnx_data_esem.access_cmd.esem_cmd_table_map_get = dnx_data_esem_access_cmd_esem_cmd_table_map_get;
    dnx_data_esem.access_cmd.esem_cmd_table_map_info_get = dnx_data_esem_access_cmd_esem_cmd_table_map_info_get;
    
    
    dnx_data_esem.default_result_profile.feature_get = dnx_data_esem_default_result_profile_feature_get;

    
    dnx_data_esem.default_result_profile.default_native_get = dnx_data_esem_default_result_profile_default_native_get;
    dnx_data_esem.default_result_profile.default_ac_get = dnx_data_esem_default_result_profile_default_ac_get;
    dnx_data_esem.default_result_profile.sflow_sample_interface_get = dnx_data_esem_default_result_profile_sflow_sample_interface_get;
    dnx_data_esem.default_result_profile.default_dual_homing_get = dnx_data_esem_default_result_profile_default_dual_homing_get;
    dnx_data_esem.default_result_profile.vxlan_get = dnx_data_esem_default_result_profile_vxlan_get;
    dnx_data_esem.default_result_profile.nof_profiles_get = dnx_data_esem_default_result_profile_nof_profiles_get;
    dnx_data_esem.default_result_profile.nof_allocable_profiles_get = dnx_data_esem_default_result_profile_nof_allocable_profiles_get;

    
    
    
    dnx_data_esem.feature.feature_get = dnx_data_esem_feature_feature_get;

    
    dnx_data_esem.feature.esem_name_space_nof_bits_get = dnx_data_esem_feature_esem_name_space_nof_bits_get;

    
    
    
    
    dnx_data_fabric.general.feature_get = dnx_data_fabric_general_feature_get;

    
    dnx_data_fabric.general.nof_lcplls_get = dnx_data_fabric_general_nof_lcplls_get;
    dnx_data_fabric.general.fmac_clock_khz_get = dnx_data_fabric_general_fmac_clock_khz_get;
    dnx_data_fabric.general.fmac_tx_padding_min_port_speed_get = dnx_data_fabric_general_fmac_tx_padding_min_port_speed_get;
    dnx_data_fabric.general.nof_ber_proj_error_analyzer_counters_get = dnx_data_fabric_general_nof_ber_proj_error_analyzer_counters_get;
    dnx_data_fabric.general.connect_mode_get = dnx_data_fabric_general_connect_mode_get;

    
    dnx_data_fabric.general.fmac_bus_size_get = dnx_data_fabric_general_fmac_bus_size_get;
    dnx_data_fabric.general.fmac_bus_size_info_get = dnx_data_fabric_general_fmac_bus_size_info_get;
    dnx_data_fabric.general.pll_phys_get = dnx_data_fabric_general_pll_phys_get;
    dnx_data_fabric.general.pll_phys_info_get = dnx_data_fabric_general_pll_phys_info_get;
    dnx_data_fabric.general.pm_properties_get = dnx_data_fabric_general_pm_properties_get;
    dnx_data_fabric.general.pm_properties_info_get = dnx_data_fabric_general_pm_properties_info_get;
    dnx_data_fabric.general.force_single_pll_get = dnx_data_fabric_general_force_single_pll_get;
    dnx_data_fabric.general.force_single_pll_info_get = dnx_data_fabric_general_force_single_pll_info_get;
    dnx_data_fabric.general.supported_fmac_tx_padding_sizes_get = dnx_data_fabric_general_supported_fmac_tx_padding_sizes_get;
    dnx_data_fabric.general.supported_fmac_tx_padding_sizes_info_get = dnx_data_fabric_general_supported_fmac_tx_padding_sizes_info_get;
    
    
    dnx_data_fabric.links.feature_get = dnx_data_fabric_links_feature_get;

    
    dnx_data_fabric.links.nof_links_get = dnx_data_fabric_links_nof_links_get;
    dnx_data_fabric.links.nof_links_per_core_get = dnx_data_fabric_links_nof_links_per_core_get;
    dnx_data_fabric.links.max_link_id_get = dnx_data_fabric_links_max_link_id_get;
    dnx_data_fabric.links.usec_between_load_balancing_events_get = dnx_data_fabric_links_usec_between_load_balancing_events_get;
    dnx_data_fabric.links.kr_fec_supported_get = dnx_data_fabric_links_kr_fec_supported_get;
    dnx_data_fabric.links.wfd_max_hw_weight_get = dnx_data_fabric_links_wfd_max_hw_weight_get;
    dnx_data_fabric.links.load_balancing_profile_id_min_get = dnx_data_fabric_links_load_balancing_profile_id_min_get;
    dnx_data_fabric.links.load_balancing_profile_id_max_get = dnx_data_fabric_links_load_balancing_profile_id_max_get;
    dnx_data_fabric.links.fabric_pm_type_get = dnx_data_fabric_links_fabric_pm_type_get;
    dnx_data_fabric.links.core_mapping_mode_get = dnx_data_fabric_links_core_mapping_mode_get;

    
    dnx_data_fabric.links.general_get = dnx_data_fabric_links_general_get;
    dnx_data_fabric.links.general_info_get = dnx_data_fabric_links_general_info_get;
    dnx_data_fabric.links.polarity_get = dnx_data_fabric_links_polarity_get;
    dnx_data_fabric.links.polarity_info_get = dnx_data_fabric_links_polarity_info_get;
    dnx_data_fabric.links.supported_interfaces_get = dnx_data_fabric_links_supported_interfaces_get;
    dnx_data_fabric.links.supported_interfaces_info_get = dnx_data_fabric_links_supported_interfaces_info_get;
    dnx_data_fabric.links.default_fec_get = dnx_data_fabric_links_default_fec_get;
    dnx_data_fabric.links.default_fec_info_get = dnx_data_fabric_links_default_fec_info_get;
    dnx_data_fabric.links.supported_fec_controls_get = dnx_data_fabric_links_supported_fec_controls_get;
    dnx_data_fabric.links.supported_fec_controls_info_get = dnx_data_fabric_links_supported_fec_controls_info_get;
    
    
    dnx_data_fabric.blocks.feature_get = dnx_data_fabric_blocks_feature_get;

    
    dnx_data_fabric.blocks.nof_instances_fmac_get = dnx_data_fabric_blocks_nof_instances_fmac_get;
    dnx_data_fabric.blocks.nof_links_in_fmac_get = dnx_data_fabric_blocks_nof_links_in_fmac_get;
    dnx_data_fabric.blocks.nof_instances_fsrd_get = dnx_data_fabric_blocks_nof_instances_fsrd_get;
    dnx_data_fabric.blocks.nof_links_in_fsrd_get = dnx_data_fabric_blocks_nof_links_in_fsrd_get;
    dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get = dnx_data_fabric_blocks_nof_fmacs_in_fsrd_get;
    dnx_data_fabric.blocks.nof_pms_get = dnx_data_fabric_blocks_nof_pms_get;
    dnx_data_fabric.blocks.nof_links_in_pm_get = dnx_data_fabric_blocks_nof_links_in_pm_get;
    dnx_data_fabric.blocks.nof_fmacs_in_pm_get = dnx_data_fabric_blocks_nof_fmacs_in_pm_get;
    dnx_data_fabric.blocks.nof_instances_fdtl_get = dnx_data_fabric_blocks_nof_instances_fdtl_get;
    dnx_data_fabric.blocks.nof_links_in_fdtl_get = dnx_data_fabric_blocks_nof_links_in_fdtl_get;
    dnx_data_fabric.blocks.nof_sbus_chains_get = dnx_data_fabric_blocks_nof_sbus_chains_get;

    
    dnx_data_fabric.blocks.fsrd_sbus_chain_get = dnx_data_fabric_blocks_fsrd_sbus_chain_get;
    dnx_data_fabric.blocks.fsrd_sbus_chain_info_get = dnx_data_fabric_blocks_fsrd_sbus_chain_info_get;
    dnx_data_fabric.blocks.fmac_sbus_chain_get = dnx_data_fabric_blocks_fmac_sbus_chain_get;
    dnx_data_fabric.blocks.fmac_sbus_chain_info_get = dnx_data_fabric_blocks_fmac_sbus_chain_info_get;
    dnx_data_fabric.blocks.fsrd_ref_clk_master_get = dnx_data_fabric_blocks_fsrd_ref_clk_master_get;
    dnx_data_fabric.blocks.fsrd_ref_clk_master_info_get = dnx_data_fabric_blocks_fsrd_ref_clk_master_info_get;
    
    
    dnx_data_fabric.reachability.feature_get = dnx_data_fabric_reachability_feature_get;

    
    dnx_data_fabric.reachability.resolution_get = dnx_data_fabric_reachability_resolution_get;
    dnx_data_fabric.reachability.gen_period_get = dnx_data_fabric_reachability_gen_period_get;
    dnx_data_fabric.reachability.full_cycle_period_nsec_get = dnx_data_fabric_reachability_full_cycle_period_nsec_get;
    dnx_data_fabric.reachability.watchdog_resolution_get = dnx_data_fabric_reachability_watchdog_resolution_get;
    dnx_data_fabric.reachability.watchdog_period_usec_get = dnx_data_fabric_reachability_watchdog_period_usec_get;

    
    
    
    dnx_data_fabric.mesh.feature_get = dnx_data_fabric_mesh_feature_get;

    
    dnx_data_fabric.mesh.max_mc_id_get = dnx_data_fabric_mesh_max_mc_id_get;
    dnx_data_fabric.mesh.max_nof_dest_get = dnx_data_fabric_mesh_max_nof_dest_get;
    dnx_data_fabric.mesh.multicast_enable_get = dnx_data_fabric_mesh_multicast_enable_get;
    dnx_data_fabric.mesh.nof_dest_get = dnx_data_fabric_mesh_nof_dest_get;

    
    
    
    dnx_data_fabric.cell.feature_get = dnx_data_fabric_cell_feature_get;

    
    dnx_data_fabric.cell.nof_priorities_get = dnx_data_fabric_cell_nof_priorities_get;
    dnx_data_fabric.cell.vsc256_max_size_get = dnx_data_fabric_cell_vsc256_max_size_get;
    dnx_data_fabric.cell.max_vsc_format_size_get = dnx_data_fabric_cell_max_vsc_format_size_get;
    dnx_data_fabric.cell.sr_cell_header_offset_get = dnx_data_fabric_cell_sr_cell_header_offset_get;
    dnx_data_fabric.cell.sr_cell_nof_instances_get = dnx_data_fabric_cell_sr_cell_nof_instances_get;
    dnx_data_fabric.cell.sr_cell_rx_fifo_size_get = dnx_data_fabric_cell_sr_cell_rx_fifo_size_get;
    dnx_data_fabric.cell.sr_cell_data_valid_offset_get = dnx_data_fabric_cell_sr_cell_data_valid_offset_get;
    dnx_data_fabric.cell.ctrl_cells_init_fifo_empty_iterations_get = dnx_data_fabric_cell_ctrl_cells_init_fifo_empty_iterations_get;
    dnx_data_fabric.cell.default_packing_mode_get = dnx_data_fabric_cell_default_packing_mode_get;
    dnx_data_fabric.cell.pcp_enable_get = dnx_data_fabric_cell_pcp_enable_get;
    dnx_data_fabric.cell.cell_format_get = dnx_data_fabric_cell_cell_format_get;

    
    dnx_data_fabric.cell.supported_pcp_modes_get = dnx_data_fabric_cell_supported_pcp_modes_get;
    dnx_data_fabric.cell.supported_pcp_modes_info_get = dnx_data_fabric_cell_supported_pcp_modes_info_get;
    dnx_data_fabric.cell.sr_cell_rx_parsing_table_get = dnx_data_fabric_cell_sr_cell_rx_parsing_table_get;
    dnx_data_fabric.cell.sr_cell_rx_parsing_table_info_get = dnx_data_fabric_cell_sr_cell_rx_parsing_table_info_get;
    
    
    dnx_data_fabric.pipes.feature_get = dnx_data_fabric_pipes_feature_get;

    
    dnx_data_fabric.pipes.max_nof_pipes_get = dnx_data_fabric_pipes_max_nof_pipes_get;
    dnx_data_fabric.pipes.max_nof_contexts_get = dnx_data_fabric_pipes_max_nof_contexts_get;
    dnx_data_fabric.pipes.max_nof_subcontexts_get = dnx_data_fabric_pipes_max_nof_subcontexts_get;
    dnx_data_fabric.pipes.nof_pipes_get = dnx_data_fabric_pipes_nof_pipes_get;
    dnx_data_fabric.pipes.multi_pipe_system_get = dnx_data_fabric_pipes_multi_pipe_system_get;

    
    dnx_data_fabric.pipes.map_get = dnx_data_fabric_pipes_map_get;
    dnx_data_fabric.pipes.map_info_get = dnx_data_fabric_pipes_map_info_get;
    dnx_data_fabric.pipes.valid_map_config_get = dnx_data_fabric_pipes_valid_map_config_get;
    dnx_data_fabric.pipes.valid_map_config_info_get = dnx_data_fabric_pipes_valid_map_config_info_get;
    dnx_data_fabric.pipes.dtq_subcontexts_get = dnx_data_fabric_pipes_dtq_subcontexts_get;
    dnx_data_fabric.pipes.dtq_subcontexts_info_get = dnx_data_fabric_pipes_dtq_subcontexts_info_get;
    
    
    dnx_data_fabric.debug.feature_get = dnx_data_fabric_debug_feature_get;

    
    dnx_data_fabric.debug.mesh_topology_size_get = dnx_data_fabric_debug_mesh_topology_size_get;
    dnx_data_fabric.debug.mesh_topology_fast_get = dnx_data_fabric_debug_mesh_topology_fast_get;

    
    
    
    dnx_data_fabric.mesh_topology.feature_get = dnx_data_fabric_mesh_topology_feature_get;

    
    dnx_data_fabric.mesh_topology.mesh_init_get = dnx_data_fabric_mesh_topology_mesh_init_get;
    dnx_data_fabric.mesh_topology.mesh_config_1_get = dnx_data_fabric_mesh_topology_mesh_config_1_get;

    
    
    
    dnx_data_fabric.tdm.feature_get = dnx_data_fabric_tdm_feature_get;

    
    dnx_data_fabric.tdm.priority_get = dnx_data_fabric_tdm_priority_get;

    
    
    
    dnx_data_fabric.cgm.feature_get = dnx_data_fabric_cgm_feature_get;

    
    dnx_data_fabric.cgm.nof_rci_levels_get = dnx_data_fabric_cgm_nof_rci_levels_get;
    dnx_data_fabric.cgm.nof_rci_threshold_levels_get = dnx_data_fabric_cgm_nof_rci_threshold_levels_get;
    dnx_data_fabric.cgm.egress_drop_th_clocks_resolution_get = dnx_data_fabric_cgm_egress_drop_th_clocks_resolution_get;
    dnx_data_fabric.cgm.egress_fifo_fc_threshold_get = dnx_data_fabric_cgm_egress_fifo_fc_threshold_get;
    dnx_data_fabric.cgm.static_shaper_credit_resolution_bytes_get = dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_get;
    dnx_data_fabric.cgm.cgm_adaptive_tfl_shaper_zone_nof_get = dnx_data_fabric_cgm_cgm_adaptive_tfl_shaper_zone_nof_get;
    dnx_data_fabric.cgm.cgm_global_cell_shaper_max_rate_get = dnx_data_fabric_cgm_cgm_global_cell_shaper_max_rate_get;
    dnx_data_fabric.cgm.rci_link_level_factor_max_value_get = dnx_data_fabric_cgm_rci_link_level_factor_max_value_get;
    dnx_data_fabric.cgm.local_unicast_dynamic_wfq_max_threshold_value_get = dnx_data_fabric_cgm_local_unicast_dynamic_wfq_max_threshold_value_get;
    dnx_data_fabric.cgm.local_unicast_dynamic_wfq_high_threshold_get = dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_threshold_get;
    dnx_data_fabric.cgm.local_unicast_dynamic_wfq_low_threshold_get = dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_threshold_get;
    dnx_data_fabric.cgm.local_unicast_dynamic_wfq_high_weight_get = dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_weight_get;
    dnx_data_fabric.cgm.local_unicast_dynamic_wfq_low_weight_get = dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_weight_get;
    dnx_data_fabric.cgm.llfc_type_default_get = dnx_data_fabric_cgm_llfc_type_default_get;

    
    dnx_data_fabric.cgm.access_map_get = dnx_data_fabric_cgm_access_map_get;
    dnx_data_fabric.cgm.access_map_info_get = dnx_data_fabric_cgm_access_map_info_get;
    dnx_data_fabric.cgm.ingress_dynamic_wfq_configs_get = dnx_data_fabric_cgm_ingress_dynamic_wfq_configs_get;
    dnx_data_fabric.cgm.ingress_dynamic_wfq_configs_info_get = dnx_data_fabric_cgm_ingress_dynamic_wfq_configs_info_get;
    
    
    dnx_data_fabric.dtqs.feature_get = dnx_data_fabric_dtqs_feature_get;

    
    dnx_data_fabric.dtqs.max_nof_subdtqs_get = dnx_data_fabric_dtqs_max_nof_subdtqs_get;
    dnx_data_fabric.dtqs.max_nof_dtqs_get = dnx_data_fabric_dtqs_max_nof_dtqs_get;
    dnx_data_fabric.dtqs.max_size_get = dnx_data_fabric_dtqs_max_size_get;
    dnx_data_fabric.dtqs.ocb_only_size_get = dnx_data_fabric_dtqs_ocb_only_size_get;
    dnx_data_fabric.dtqs.nof_active_dtqs_get = dnx_data_fabric_dtqs_nof_active_dtqs_get;

    
    
    
    dnx_data_fabric.ilkn.feature_get = dnx_data_fabric_ilkn_feature_get;

    

    
    dnx_data_fabric.ilkn.bypass_info_get = dnx_data_fabric_ilkn_bypass_info_get;
    dnx_data_fabric.ilkn.bypass_info_info_get = dnx_data_fabric_ilkn_bypass_info_info_get;
    
    
    dnx_data_fabric.dbal.feature_get = dnx_data_fabric_dbal_feature_get;

    
    dnx_data_fabric.dbal.link_rci_threshold_bits_nof_get = dnx_data_fabric_dbal_link_rci_threshold_bits_nof_get;
    dnx_data_fabric.dbal.cgm_llfc_threshold_bits_nof_get = dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_get;
    dnx_data_fabric.dbal.cgm_llfc_threshold_max_value_get = dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_get;
    dnx_data_fabric.dbal.cgm_llfc_ingress_ipt_ctx_mapping_bits_nof_get = dnx_data_fabric_dbal_cgm_llfc_ingress_ipt_ctx_mapping_bits_nof_get;
    dnx_data_fabric.dbal.egress_fifo_drop_threshold_bits_nof_get = dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_get;
    dnx_data_fabric.dbal.egress_fifo_drop_threshold_max_value_get = dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_get;
    dnx_data_fabric.dbal.cgm_link_fifo_base_address_bits_nof_get = dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_get;
    dnx_data_fabric.dbal.cgm_link_fifo_base_address_max_value_get = dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_base_address_bits_nof_get = dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_base_address_max_value_get = dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_fc_threshold_bits_nof_get = dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_fc_threshold_max_value_get = dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_fc_full_level_min_value_get = dnx_data_fabric_dbal_cgm_egress_fifo_fc_full_level_min_value_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_fc_full_level_max_value_get = dnx_data_fabric_dbal_cgm_egress_fifo_fc_full_level_max_value_get;
    dnx_data_fabric.dbal.cgm_egress_fifo_fc_full_level_bits_nof_get = dnx_data_fabric_dbal_cgm_egress_fifo_fc_full_level_bits_nof_get;
    dnx_data_fabric.dbal.cgm_egress_drop_mc_low_prio_th_bits_nof_get = dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_get;
    dnx_data_fabric.dbal.cgm_egress_dynamic_wfq_th_bits_nof_get = dnx_data_fabric_dbal_cgm_egress_dynamic_wfq_th_bits_nof_get;
    dnx_data_fabric.dbal.cgm_ingress_fc_threshold_max_value_get = dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_get;
    dnx_data_fabric.dbal.cgm_static_shaper_calendar_bits_nof_get = dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_get;
    dnx_data_fabric.dbal.fabric_transmit_wfq_index_bits_nof_get = dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_get;
    dnx_data_fabric.dbal.fabric_transmit_wfq_index_max_value_get = dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_get;
    dnx_data_fabric.dbal.mesh_topology_init_config_2_bits_nof_get = dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_get;
    dnx_data_fabric.dbal.fabric_transmit_dtq_fc_thresholds_bits_nof_get = dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_get;
    dnx_data_fabric.dbal.fabric_transmit_pdq_size_bits_nof_get = dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_get;
    dnx_data_fabric.dbal.adaptive_shaper_threshold_bits_nof_get = dnx_data_fabric_dbal_adaptive_shaper_threshold_bits_nof_get;
    dnx_data_fabric.dbal.adaptive_shaper_factor_bits_nof_get = dnx_data_fabric_dbal_adaptive_shaper_factor_bits_nof_get;
    dnx_data_fabric.dbal.adaptive_shaper_mnol_bits_nof_get = dnx_data_fabric_dbal_adaptive_shaper_mnol_bits_nof_get;
    dnx_data_fabric.dbal.global_cell_shaper_allowed_cells_bits_nof_get = dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_bits_nof_get;
    dnx_data_fabric.dbal.global_cell_shaper_allowed_cells_granularity_get = dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_granularity_get;
    dnx_data_fabric.dbal.global_cell_shaper_allowed_cells_max_value_get = dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_max_value_get;
    dnx_data_fabric.dbal.global_cell_shaper_time_frame_bits_nof_get = dnx_data_fabric_dbal_global_cell_shaper_time_frame_bits_nof_get;
    dnx_data_fabric.dbal.global_cell_shaper_time_frame_max_value_get = dnx_data_fabric_dbal_global_cell_shaper_time_frame_max_value_get;
    dnx_data_fabric.dbal.global_cell_shaper_cnt_bits_nof_get = dnx_data_fabric_dbal_global_cell_shaper_cnt_bits_nof_get;
    dnx_data_fabric.dbal.fdr_links_offset_get = dnx_data_fabric_dbal_fdr_links_offset_get;
    dnx_data_fabric.dbal.fabric_cgm_rci_high_severity_min_nof_links_bits_nof_get = dnx_data_fabric_dbal_fabric_cgm_rci_high_severity_min_nof_links_bits_nof_get;
    dnx_data_fabric.dbal.priority_bits_nof_get = dnx_data_fabric_dbal_priority_bits_nof_get;
    dnx_data_fabric.dbal.nof_links_bits_nof_get = dnx_data_fabric_dbal_nof_links_bits_nof_get;

    
    
    
    dnx_data_fabric.mesh_mc.feature_get = dnx_data_fabric_mesh_mc_feature_get;

    
    dnx_data_fabric.mesh_mc.nof_replication_get = dnx_data_fabric_mesh_mc_nof_replication_get;

    
    
    
    dnx_data_fabric.transmit.feature_get = dnx_data_fabric_transmit_feature_get;

    
    dnx_data_fabric.transmit.pdqs_total_size_get = dnx_data_fabric_transmit_pdqs_total_size_get;
    dnx_data_fabric.transmit.nof_pdqs_get = dnx_data_fabric_transmit_nof_pdqs_get;
    dnx_data_fabric.transmit.s2d_pdq_size_get = dnx_data_fabric_transmit_s2d_pdq_size_get;
    dnx_data_fabric.transmit.pdq_sram_fc_th_get = dnx_data_fabric_transmit_pdq_sram_fc_th_get;
    dnx_data_fabric.transmit.pdq_mixs_fc_th_get = dnx_data_fabric_transmit_pdq_mixs_fc_th_get;
    dnx_data_fabric.transmit.pdq_mixd_fc_th_get = dnx_data_fabric_transmit_pdq_mixd_fc_th_get;
    dnx_data_fabric.transmit.pdq_mix_mixs_fc_th_get = dnx_data_fabric_transmit_pdq_mix_mixs_fc_th_get;
    dnx_data_fabric.transmit.pdq_mix_mixd_fc_th_get = dnx_data_fabric_transmit_pdq_mix_mixd_fc_th_get;
    dnx_data_fabric.transmit.pdq_ocb_fc_th_get = dnx_data_fabric_transmit_pdq_ocb_fc_th_get;
    dnx_data_fabric.transmit.pdq_dram_delete_fc_th_get = dnx_data_fabric_transmit_pdq_dram_delete_fc_th_get;
    dnx_data_fabric.transmit.pdq_sram_delete_fc_th_get = dnx_data_fabric_transmit_pdq_sram_delete_fc_th_get;
    dnx_data_fabric.transmit.pdq_s2d_fc_th_get = dnx_data_fabric_transmit_pdq_s2d_fc_th_get;
    dnx_data_fabric.transmit.pdq_sram_eir_fc_th_get = dnx_data_fabric_transmit_pdq_sram_eir_fc_th_get;
    dnx_data_fabric.transmit.pdq_mixs_eir_fc_th_get = dnx_data_fabric_transmit_pdq_mixs_eir_fc_th_get;
    dnx_data_fabric.transmit.pdq_mixd_eir_fc_th_get = dnx_data_fabric_transmit_pdq_mixd_eir_fc_th_get;
    dnx_data_fabric.transmit.pdq_ocb_eir_fc_th_get = dnx_data_fabric_transmit_pdq_ocb_eir_fc_th_get;

    
    dnx_data_fabric.transmit.pdq_to_dqcq_fc_th_select_get = dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_get;
    dnx_data_fabric.transmit.pdq_to_dqcq_fc_th_select_info_get = dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_info_get;
    dnx_data_fabric.transmit.eir_fc_leaky_bucket_get = dnx_data_fabric_transmit_eir_fc_leaky_bucket_get;
    dnx_data_fabric.transmit.eir_fc_leaky_bucket_info_get = dnx_data_fabric_transmit_eir_fc_leaky_bucket_info_get;
    dnx_data_fabric.transmit.eir_fc_leaky_bucket_delta_select_get = dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_get;
    dnx_data_fabric.transmit.eir_fc_leaky_bucket_delta_select_info_get = dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_info_get;
    dnx_data_fabric.transmit.dtq_fc_local_thresholds_get = dnx_data_fabric_transmit_dtq_fc_local_thresholds_get;
    dnx_data_fabric.transmit.dtq_fc_local_thresholds_info_get = dnx_data_fabric_transmit_dtq_fc_local_thresholds_info_get;
    dnx_data_fabric.transmit.dtq_fc_fabric_thresholds_get = dnx_data_fabric_transmit_dtq_fc_fabric_thresholds_get;
    dnx_data_fabric.transmit.dtq_fc_fabric_thresholds_info_get = dnx_data_fabric_transmit_dtq_fc_fabric_thresholds_info_get;
    dnx_data_fabric.transmit.dtq_fc_ocb_thresholds_get = dnx_data_fabric_transmit_dtq_fc_ocb_thresholds_get;
    dnx_data_fabric.transmit.dtq_fc_ocb_thresholds_info_get = dnx_data_fabric_transmit_dtq_fc_ocb_thresholds_info_get;
    dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_get = dnx_data_fabric_transmit_dtq_to_pdq_ocb_thresholds_get;
    dnx_data_fabric.transmit.dtq_to_pdq_ocb_thresholds_info_get = dnx_data_fabric_transmit_dtq_to_pdq_ocb_thresholds_info_get;
    dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_get = dnx_data_fabric_transmit_dtq_to_pdq_non_ocb_thresholds_get;
    dnx_data_fabric.transmit.dtq_to_pdq_non_ocb_thresholds_info_get = dnx_data_fabric_transmit_dtq_to_pdq_non_ocb_thresholds_info_get;
    dnx_data_fabric.transmit.dtq_to_pdq_fc_mapping_get = dnx_data_fabric_transmit_dtq_to_pdq_fc_mapping_get;
    dnx_data_fabric.transmit.dtq_to_pdq_fc_mapping_info_get = dnx_data_fabric_transmit_dtq_to_pdq_fc_mapping_info_get;
    
    
    dnx_data_fabric.system_upgrade.feature_get = dnx_data_fabric_system_upgrade_feature_get;

    
    dnx_data_fabric.system_upgrade.max_master_id_get = dnx_data_fabric_system_upgrade_max_master_id_get;
    dnx_data_fabric.system_upgrade.param_0_get = dnx_data_fabric_system_upgrade_param_0_get;
    dnx_data_fabric.system_upgrade.param_1_get = dnx_data_fabric_system_upgrade_param_1_get;
    dnx_data_fabric.system_upgrade.param_2_get = dnx_data_fabric_system_upgrade_param_2_get;
    dnx_data_fabric.system_upgrade.param_3_get = dnx_data_fabric_system_upgrade_param_3_get;
    dnx_data_fabric.system_upgrade.param_4_get = dnx_data_fabric_system_upgrade_param_4_get;
    dnx_data_fabric.system_upgrade.param_5_get = dnx_data_fabric_system_upgrade_param_5_get;
    dnx_data_fabric.system_upgrade.param_7_get = dnx_data_fabric_system_upgrade_param_7_get;

    
    dnx_data_fabric.system_upgrade.target_get = dnx_data_fabric_system_upgrade_target_get;
    dnx_data_fabric.system_upgrade.target_info_get = dnx_data_fabric_system_upgrade_target_info_get;
    
    
    dnx_data_fabric.regression.feature_get = dnx_data_fabric_regression_feature_get;

    
    dnx_data_fabric.regression.minimum_system_ref_clk_get = dnx_data_fabric_regression_minimum_system_ref_clk_get;
    dnx_data_fabric.regression.soft_reset_with_fabric_max_exec_time_get = dnx_data_fabric_regression_soft_reset_with_fabric_max_exec_time_get;
    dnx_data_fabric.regression.soft_reset_with_fabric_avg_exec_time_get = dnx_data_fabric_regression_soft_reset_with_fabric_avg_exec_time_get;

    
    
    
    
    dnx_data_failover.path_select.feature_get = dnx_data_failover_path_select_feature_get;

    
    dnx_data_failover.path_select.egress_size_get = dnx_data_failover_path_select_egress_size_get;
    dnx_data_failover.path_select.ingress_size_get = dnx_data_failover_path_select_ingress_size_get;
    dnx_data_failover.path_select.ingress_mc_size_get = dnx_data_failover_path_select_ingress_mc_size_get;
    dnx_data_failover.path_select.fec_size_get = dnx_data_failover_path_select_fec_size_get;
    dnx_data_failover.path_select.egr_no_protection_get = dnx_data_failover_path_select_egr_no_protection_get;
    dnx_data_failover.path_select.ing_no_protection_get = dnx_data_failover_path_select_ing_no_protection_get;
    dnx_data_failover.path_select.ing_mc_no_protection_get = dnx_data_failover_path_select_ing_mc_no_protection_get;
    dnx_data_failover.path_select.fec_no_protection_get = dnx_data_failover_path_select_fec_no_protection_get;
    dnx_data_failover.path_select.fec_facility_protection_get = dnx_data_failover_path_select_fec_facility_protection_get;
    dnx_data_failover.path_select.nof_fec_path_select_banks_get = dnx_data_failover_path_select_nof_fec_path_select_banks_get;
    dnx_data_failover.path_select.nof_fec_path_select_banks_in_hw_get = dnx_data_failover_path_select_nof_fec_path_select_banks_in_hw_get;
    dnx_data_failover.path_select.fec_bank_size_get = dnx_data_failover_path_select_fec_bank_size_get;
    dnx_data_failover.path_select.in_lif_protection_pointer_nof_msb_get = dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_get;
    dnx_data_failover.path_select.in_lif_protection_pointer_nof_lsb_get = dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_get;

    
    
    
    dnx_data_failover.facility.feature_get = dnx_data_failover_facility_feature_get;

    

    
    
    
    
    dnx_data_fc.general.feature_get = dnx_data_fc_general_feature_get;

    
    dnx_data_fc.general.nof_glb_rsc_get = dnx_data_fc_general_nof_glb_rsc_get;
    dnx_data_fc.general.nof_gen_bmps_get = dnx_data_fc_general_nof_gen_bmps_get;
    dnx_data_fc.general.gen_bmp_first_id_get = dnx_data_fc_general_gen_bmp_first_id_get;
    dnx_data_fc.general.nof_af_nif_indications_get = dnx_data_fc_general_nof_af_nif_indications_get;
    dnx_data_fc.general.nof_cal_llfcs_get = dnx_data_fc_general_nof_cal_llfcs_get;
    dnx_data_fc.general.nof_entries_qmlf_get = dnx_data_fc_general_nof_entries_qmlf_get;
    dnx_data_fc.general.nof_bits_in_pfc_status_select_get = dnx_data_fc_general_nof_bits_in_pfc_status_select_get;
    dnx_data_fc.general.nof_rmc_thresholds_profiles_get = dnx_data_fc_general_nof_rmc_thresholds_profiles_get;
    dnx_data_fc.general.rmc_pfc_set_threshold_default_get = dnx_data_fc_general_rmc_pfc_set_threshold_default_get;
    dnx_data_fc.general.rmc_pfc_clear_threshold_default_get = dnx_data_fc_general_rmc_pfc_clear_threshold_default_get;
    dnx_data_fc.general.rmc_llfc_set_threshold_default_get = dnx_data_fc_general_rmc_llfc_set_threshold_default_get;
    dnx_data_fc.general.rmc_llfc_clear_threshold_default_get = dnx_data_fc_general_rmc_llfc_clear_threshold_default_get;
    dnx_data_fc.general.nof_bits_in_nif_eth_rmc_id_get = dnx_data_fc_general_nof_bits_in_nif_eth_rmc_id_get;
    dnx_data_fc.general.bcm_cos_count_get = dnx_data_fc_general_bcm_cos_count_get;

    
    
    
    dnx_data_fc.calendar_common.feature_get = dnx_data_fc_calendar_common_feature_get;

    
    dnx_data_fc.calendar_common.rx_dest_pfc_get = dnx_data_fc_calendar_common_rx_dest_pfc_get;
    dnx_data_fc.calendar_common.rx_dest_nif_llfc_get = dnx_data_fc_calendar_common_rx_dest_nif_llfc_get;
    dnx_data_fc.calendar_common.rx_dest_port_2_prio_get = dnx_data_fc_calendar_common_rx_dest_port_2_prio_get;
    dnx_data_fc.calendar_common.rx_dest_port_4_prio_get = dnx_data_fc_calendar_common_rx_dest_port_4_prio_get;
    dnx_data_fc.calendar_common.rx_dest_port_8_prio_get = dnx_data_fc_calendar_common_rx_dest_port_8_prio_get;
    dnx_data_fc.calendar_common.rx_dest_pfc_generic_bmp_get = dnx_data_fc_calendar_common_rx_dest_pfc_generic_bmp_get;
    dnx_data_fc.calendar_common.rx_dest_empty_get = dnx_data_fc_calendar_common_rx_dest_empty_get;
    dnx_data_fc.calendar_common.tx_src_vsq_a_b_c_get = dnx_data_fc_calendar_common_tx_src_vsq_a_b_c_get;
    dnx_data_fc.calendar_common.tx_src_vsq_d_get = dnx_data_fc_calendar_common_tx_src_vsq_d_get;
    dnx_data_fc.calendar_common.tx_src_llfc_vsq_get = dnx_data_fc_calendar_common_tx_src_llfc_vsq_get;
    dnx_data_fc.calendar_common.tx_src_pfc_vsq_get = dnx_data_fc_calendar_common_tx_src_pfc_vsq_get;
    dnx_data_fc.calendar_common.tx_src_glb_rsc_get = dnx_data_fc_calendar_common_tx_src_glb_rsc_get;
    dnx_data_fc.calendar_common.tx_src_nif_llfc_get = dnx_data_fc_calendar_common_tx_src_nif_llfc_get;
    dnx_data_fc.calendar_common.tx_src_empty_get = dnx_data_fc_calendar_common_tx_src_empty_get;

    
    
    
    dnx_data_fc.coe.feature_get = dnx_data_fc_coe_feature_get;

    
    dnx_data_fc.coe.nof_coe_cal_instances_get = dnx_data_fc_coe_nof_coe_cal_instances_get;
    dnx_data_fc.coe.nof_supported_chan_ids_get = dnx_data_fc_coe_nof_supported_chan_ids_get;
    dnx_data_fc.coe.nof_coe_vid_mask_width_get = dnx_data_fc_coe_nof_coe_vid_mask_width_get;
    dnx_data_fc.coe.nof_coe_vid_right_shift_width_get = dnx_data_fc_coe_nof_coe_vid_right_shift_width_get;
    dnx_data_fc.coe.nof_coe_ptc_mask_width_get = dnx_data_fc_coe_nof_coe_ptc_mask_width_get;
    dnx_data_fc.coe.nof_coe_ptc_left_shift_width_get = dnx_data_fc_coe_nof_coe_ptc_left_shift_width_get;
    dnx_data_fc.coe.max_coe_ticks_get = dnx_data_fc_coe_max_coe_ticks_get;
    dnx_data_fc.coe.fc_support_get = dnx_data_fc_coe_fc_support_get;
    dnx_data_fc.coe.map_value_size_in_bits_get = dnx_data_fc_coe_map_value_size_in_bits_get;
    dnx_data_fc.coe.calendar_pause_resolution_get = dnx_data_fc_coe_calendar_pause_resolution_get;
    dnx_data_fc.coe.data_offset_get = dnx_data_fc_coe_data_offset_get;
    dnx_data_fc.coe.ethertype_get = dnx_data_fc_coe_ethertype_get;
    dnx_data_fc.coe.control_opcode_get = dnx_data_fc_coe_control_opcode_get;
    dnx_data_fc.coe.coe_vid_offset_get = dnx_data_fc_coe_coe_vid_offset_get;
    dnx_data_fc.coe.coe_vid_mask_get = dnx_data_fc_coe_coe_vid_mask_get;
    dnx_data_fc.coe.coe_vid_right_shift_get = dnx_data_fc_coe_coe_vid_right_shift_get;
    dnx_data_fc.coe.coe_ptc_mask_get = dnx_data_fc_coe_coe_ptc_mask_get;
    dnx_data_fc.coe.coe_ptc_left_shift_get = dnx_data_fc_coe_coe_ptc_left_shift_get;

    
    dnx_data_fc.coe.calendar_mode_get = dnx_data_fc_coe_calendar_mode_get;
    dnx_data_fc.coe.calendar_mode_info_get = dnx_data_fc_coe_calendar_mode_info_get;
    dnx_data_fc.coe.mac_address_get = dnx_data_fc_coe_mac_address_get;
    dnx_data_fc.coe.mac_address_info_get = dnx_data_fc_coe_mac_address_info_get;
    dnx_data_fc.coe.calender_length_get = dnx_data_fc_coe_calender_length_get;
    dnx_data_fc.coe.calender_length_info_get = dnx_data_fc_coe_calender_length_info_get;
    
    
    dnx_data_fc.oob.feature_get = dnx_data_fc_oob_feature_get;

    
    dnx_data_fc.oob.nof_oob_ids_get = dnx_data_fc_oob_nof_oob_ids_get;

    
    dnx_data_fc.oob.calender_length_get = dnx_data_fc_oob_calender_length_get;
    dnx_data_fc.oob.calender_length_info_get = dnx_data_fc_oob_calender_length_info_get;
    dnx_data_fc.oob.calender_rep_count_get = dnx_data_fc_oob_calender_rep_count_get;
    dnx_data_fc.oob.calender_rep_count_info_get = dnx_data_fc_oob_calender_rep_count_info_get;
    dnx_data_fc.oob.tx_freq_get = dnx_data_fc_oob_tx_freq_get;
    dnx_data_fc.oob.tx_freq_info_get = dnx_data_fc_oob_tx_freq_info_get;
    dnx_data_fc.oob.port_get = dnx_data_fc_oob_port_get;
    dnx_data_fc.oob.port_info_get = dnx_data_fc_oob_port_info_get;
    dnx_data_fc.oob.indication_invert_get = dnx_data_fc_oob_indication_invert_get;
    dnx_data_fc.oob.indication_invert_info_get = dnx_data_fc_oob_indication_invert_info_get;
    
    
    dnx_data_fc.inband.feature_get = dnx_data_fc_inband_feature_get;

    
    dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get = dnx_data_fc_inband_nof_inband_intlkn_cal_instances_get;

    
    dnx_data_fc.inband.calender_length_get = dnx_data_fc_inband_calender_length_get;
    dnx_data_fc.inband.calender_length_info_get = dnx_data_fc_inband_calender_length_info_get;
    dnx_data_fc.inband.port_get = dnx_data_fc_inband_port_get;
    dnx_data_fc.inband.port_info_get = dnx_data_fc_inband_port_info_get;
    
    
    dnx_data_fc.pfc_deadlock.feature_get = dnx_data_fc_pfc_deadlock_feature_get;

    
    dnx_data_fc.pfc_deadlock.cdum_intr_id_get = dnx_data_fc_pfc_deadlock_cdum_intr_id_get;
    dnx_data_fc.pfc_deadlock.cdu_intr_id_get = dnx_data_fc_pfc_deadlock_cdu_intr_id_get;
    dnx_data_fc.pfc_deadlock.clu_intr_id_get = dnx_data_fc_pfc_deadlock_clu_intr_id_get;
    dnx_data_fc.pfc_deadlock.nbu_intr_id_get = dnx_data_fc_pfc_deadlock_nbu_intr_id_get;
    dnx_data_fc.pfc_deadlock.nof_bits_in_port_num_get = dnx_data_fc_pfc_deadlock_nof_bits_in_port_num_get;

    
    
    
    
    dnx_data_field.base_ipmf1.feature_get = dnx_data_field_base_ipmf1_feature_get;

    
    dnx_data_field.base_ipmf1.nof_ffc_get = dnx_data_field_base_ipmf1_nof_ffc_get;
    dnx_data_field.base_ipmf1.nof_keys_get = dnx_data_field_base_ipmf1_nof_keys_get;
    dnx_data_field.base_ipmf1.nof_keys_alloc_get = dnx_data_field_base_ipmf1_nof_keys_alloc_get;
    dnx_data_field.base_ipmf1.nof_keys_alloc_for_tcam_get = dnx_data_field_base_ipmf1_nof_keys_alloc_for_tcam_get;
    dnx_data_field.base_ipmf1.nof_keys_alloc_for_exem_get = dnx_data_field_base_ipmf1_nof_keys_alloc_for_exem_get;
    dnx_data_field.base_ipmf1.nof_keys_alloc_for_dir_ext_get = dnx_data_field_base_ipmf1_nof_keys_alloc_for_dir_ext_get;
    dnx_data_field.base_ipmf1.nof_keys_alloc_for_mdb_dt_get = dnx_data_field_base_ipmf1_nof_keys_alloc_for_mdb_dt_get;
    dnx_data_field.base_ipmf1.nof_masks_per_fes_get = dnx_data_field_base_ipmf1_nof_masks_per_fes_get;
    dnx_data_field.base_ipmf1.nof_fes_id_per_array_get = dnx_data_field_base_ipmf1_nof_fes_id_per_array_get;
    dnx_data_field.base_ipmf1.nof_fes_array_get = dnx_data_field_base_ipmf1_nof_fes_array_get;
    dnx_data_field.base_ipmf1.nof_fes_instruction_per_context_get = dnx_data_field_base_ipmf1_nof_fes_instruction_per_context_get;
    dnx_data_field.base_ipmf1.nof_fes_programs_get = dnx_data_field_base_ipmf1_nof_fes_programs_get;
    dnx_data_field.base_ipmf1.nof_prog_per_fes_get = dnx_data_field_base_ipmf1_nof_prog_per_fes_get;
    dnx_data_field.base_ipmf1.cs_container_5_selected_bits_size_get = dnx_data_field_base_ipmf1_cs_container_5_selected_bits_size_get;
    dnx_data_field.base_ipmf1.nof_contexts_get = dnx_data_field_base_ipmf1_nof_contexts_get;
    dnx_data_field.base_ipmf1.nof_link_profiles_get = dnx_data_field_base_ipmf1_nof_link_profiles_get;
    dnx_data_field.base_ipmf1.nof_cs_lines_get = dnx_data_field_base_ipmf1_nof_cs_lines_get;
    dnx_data_field.base_ipmf1.nof_actions_get = dnx_data_field_base_ipmf1_nof_actions_get;
    dnx_data_field.base_ipmf1.nof_qualifiers_get = dnx_data_field_base_ipmf1_nof_qualifiers_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get = dnx_data_field_base_ipmf1_nof_bits_in_fes_action_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get = dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_get;
    dnx_data_field.base_ipmf1.fes_key_select_resolution_in_bits_get = dnx_data_field_base_ipmf1_fes_key_select_resolution_in_bits_get;
    dnx_data_field.base_ipmf1.fem_condition_ms_bit_min_value_get = dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_get;
    dnx_data_field.base_ipmf1.nof_fem_programs_get = dnx_data_field_base_ipmf1_nof_fem_programs_get;
    dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get = dnx_data_field_base_ipmf1_log_nof_bits_in_fem_key_select_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fem_key_select_get = dnx_data_field_base_ipmf1_nof_bits_in_fem_key_select_get;
    dnx_data_field.base_ipmf1.fem_key_select_resolution_in_bits_get = dnx_data_field_base_ipmf1_fem_key_select_resolution_in_bits_get;
    dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get = dnx_data_field_base_ipmf1_log_nof_bits_in_fem_map_data_field_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fem_map_data_field_get = dnx_data_field_base_ipmf1_nof_bits_in_fem_map_data_field_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get = dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_2_15_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get = dnx_data_field_base_ipmf1_nof_bits_in_fem_action_fems_0_1_get;
    dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get = dnx_data_field_base_ipmf1_nof_bits_in_fem_action_get;
    dnx_data_field.base_ipmf1.nof_fem_condition_get = dnx_data_field_base_ipmf1_nof_fem_condition_get;
    dnx_data_field.base_ipmf1.nof_fem_map_index_get = dnx_data_field_base_ipmf1_nof_fem_map_index_get;
    dnx_data_field.base_ipmf1.nof_fem_id_get = dnx_data_field_base_ipmf1_nof_fem_id_get;
    dnx_data_field.base_ipmf1.num_fems_with_short_action_get = dnx_data_field_base_ipmf1_num_fems_with_short_action_get;
    dnx_data_field.base_ipmf1.num_bits_in_fem_field_select_get = dnx_data_field_base_ipmf1_num_bits_in_fem_field_select_get;
    dnx_data_field.base_ipmf1.nof_fem_id_per_array_get = dnx_data_field_base_ipmf1_nof_fem_id_per_array_get;
    dnx_data_field.base_ipmf1.nof_fem_array_get = dnx_data_field_base_ipmf1_nof_fem_array_get;
    dnx_data_field.base_ipmf1.nof_compare_pairs_in_compare_mode_get = dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_get;
    dnx_data_field.base_ipmf1.compare_key_size_get = dnx_data_field_base_ipmf1_compare_key_size_get;
    dnx_data_field.base_ipmf1.nof_l4_ops_ranges_legacy_get = dnx_data_field_base_ipmf1_nof_l4_ops_ranges_legacy_get;
    dnx_data_field.base_ipmf1.nof_ext_l4_ops_ranges_get = dnx_data_field_base_ipmf1_nof_ext_l4_ops_ranges_get;
    dnx_data_field.base_ipmf1.nof_pkt_hdr_ranges_get = dnx_data_field_base_ipmf1_nof_pkt_hdr_ranges_get;
    dnx_data_field.base_ipmf1.nof_out_lif_ranges_get = dnx_data_field_base_ipmf1_nof_out_lif_ranges_get;
    dnx_data_field.base_ipmf1.cmp_selection_get = dnx_data_field_base_ipmf1_cmp_selection_get;
    dnx_data_field.base_ipmf1.dir_ext_single_key_size_get = dnx_data_field_base_ipmf1_dir_ext_single_key_size_get;
    dnx_data_field.base_ipmf1.dir_ext_first_key_get = dnx_data_field_base_ipmf1_dir_ext_first_key_get;
    dnx_data_field.base_ipmf1.exem_max_result_size_get = dnx_data_field_base_ipmf1_exem_max_result_size_get;
    dnx_data_field.base_ipmf1.fes_instruction_size_get = dnx_data_field_base_ipmf1_fes_instruction_size_get;
    dnx_data_field.base_ipmf1.fes_pgm_id_offset_get = dnx_data_field_base_ipmf1_fes_pgm_id_offset_get;
    dnx_data_field.base_ipmf1.fes_key_select_for_zero_bit_get = dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_get;
    dnx_data_field.base_ipmf1.fes_shift_for_zero_bit_get = dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_get;
    dnx_data_field.base_ipmf1.exem_pdb_get = dnx_data_field_base_ipmf1_exem_pdb_get;

    
    
    
    dnx_data_field.base_ipmf2.feature_get = dnx_data_field_base_ipmf2_feature_get;

    
    dnx_data_field.base_ipmf2.nof_ffc_get = dnx_data_field_base_ipmf2_nof_ffc_get;
    dnx_data_field.base_ipmf2.nof_keys_get = dnx_data_field_base_ipmf2_nof_keys_get;
    dnx_data_field.base_ipmf2.nof_keys_alloc_get = dnx_data_field_base_ipmf2_nof_keys_alloc_get;
    dnx_data_field.base_ipmf2.nof_keys_alloc_for_tcam_get = dnx_data_field_base_ipmf2_nof_keys_alloc_for_tcam_get;
    dnx_data_field.base_ipmf2.nof_keys_alloc_for_exem_get = dnx_data_field_base_ipmf2_nof_keys_alloc_for_exem_get;
    dnx_data_field.base_ipmf2.nof_keys_alloc_for_dir_ext_get = dnx_data_field_base_ipmf2_nof_keys_alloc_for_dir_ext_get;
    dnx_data_field.base_ipmf2.nof_keys_alloc_for_mdb_dt_get = dnx_data_field_base_ipmf2_nof_keys_alloc_for_mdb_dt_get;
    dnx_data_field.base_ipmf2.nof_masks_per_fes_get = dnx_data_field_base_ipmf2_nof_masks_per_fes_get;
    dnx_data_field.base_ipmf2.nof_fes_id_per_array_get = dnx_data_field_base_ipmf2_nof_fes_id_per_array_get;
    dnx_data_field.base_ipmf2.nof_fes_array_get = dnx_data_field_base_ipmf2_nof_fes_array_get;
    dnx_data_field.base_ipmf2.nof_fes_instruction_per_context_get = dnx_data_field_base_ipmf2_nof_fes_instruction_per_context_get;
    dnx_data_field.base_ipmf2.nof_cs_lines_get = dnx_data_field_base_ipmf2_nof_cs_lines_get;
    dnx_data_field.base_ipmf2.nof_contexts_get = dnx_data_field_base_ipmf2_nof_contexts_get;
    dnx_data_field.base_ipmf2.nof_qualifiers_get = dnx_data_field_base_ipmf2_nof_qualifiers_get;
    dnx_data_field.base_ipmf2.dir_ext_single_key_size_get = dnx_data_field_base_ipmf2_dir_ext_single_key_size_get;
    dnx_data_field.base_ipmf2.dir_ext_first_key_get = dnx_data_field_base_ipmf2_dir_ext_first_key_get;
    dnx_data_field.base_ipmf2.exem_max_result_size_get = dnx_data_field_base_ipmf2_exem_max_result_size_get;
    dnx_data_field.base_ipmf2.fes_key_select_for_zero_bit_get = dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_get;
    dnx_data_field.base_ipmf2.fes_shift_for_zero_bit_get = dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_get;
    dnx_data_field.base_ipmf2.exem_pdb_get = dnx_data_field_base_ipmf2_exem_pdb_get;

    
    
    
    dnx_data_field.base_ipmf3.feature_get = dnx_data_field_base_ipmf3_feature_get;

    
    dnx_data_field.base_ipmf3.nof_ffc_get = dnx_data_field_base_ipmf3_nof_ffc_get;
    dnx_data_field.base_ipmf3.nof_keys_get = dnx_data_field_base_ipmf3_nof_keys_get;
    dnx_data_field.base_ipmf3.nof_keys_alloc_get = dnx_data_field_base_ipmf3_nof_keys_alloc_get;
    dnx_data_field.base_ipmf3.nof_keys_alloc_for_tcam_get = dnx_data_field_base_ipmf3_nof_keys_alloc_for_tcam_get;
    dnx_data_field.base_ipmf3.nof_keys_alloc_for_exem_get = dnx_data_field_base_ipmf3_nof_keys_alloc_for_exem_get;
    dnx_data_field.base_ipmf3.nof_keys_alloc_for_dir_ext_get = dnx_data_field_base_ipmf3_nof_keys_alloc_for_dir_ext_get;
    dnx_data_field.base_ipmf3.nof_keys_alloc_for_mdb_dt_get = dnx_data_field_base_ipmf3_nof_keys_alloc_for_mdb_dt_get;
    dnx_data_field.base_ipmf3.nof_masks_per_fes_get = dnx_data_field_base_ipmf3_nof_masks_per_fes_get;
    dnx_data_field.base_ipmf3.nof_fes_id_per_array_get = dnx_data_field_base_ipmf3_nof_fes_id_per_array_get;
    dnx_data_field.base_ipmf3.nof_fes_array_get = dnx_data_field_base_ipmf3_nof_fes_array_get;
    dnx_data_field.base_ipmf3.nof_fes_instruction_per_context_get = dnx_data_field_base_ipmf3_nof_fes_instruction_per_context_get;
    dnx_data_field.base_ipmf3.nof_fes_programs_get = dnx_data_field_base_ipmf3_nof_fes_programs_get;
    dnx_data_field.base_ipmf3.nof_prog_per_fes_get = dnx_data_field_base_ipmf3_nof_prog_per_fes_get;
    dnx_data_field.base_ipmf3.cs_scratch_pad_size_get = dnx_data_field_base_ipmf3_cs_scratch_pad_size_get;
    dnx_data_field.base_ipmf3.nof_contexts_get = dnx_data_field_base_ipmf3_nof_contexts_get;
    dnx_data_field.base_ipmf3.nof_cs_lines_get = dnx_data_field_base_ipmf3_nof_cs_lines_get;
    dnx_data_field.base_ipmf3.nof_actions_get = dnx_data_field_base_ipmf3_nof_actions_get;
    dnx_data_field.base_ipmf3.nof_qualifiers_get = dnx_data_field_base_ipmf3_nof_qualifiers_get;
    dnx_data_field.base_ipmf3.nof_bits_in_fes_action_get = dnx_data_field_base_ipmf3_nof_bits_in_fes_action_get;
    dnx_data_field.base_ipmf3.nof_bits_in_fes_key_select_get = dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_get;
    dnx_data_field.base_ipmf3.fes_key_select_resolution_in_bits_get = dnx_data_field_base_ipmf3_fes_key_select_resolution_in_bits_get;
    dnx_data_field.base_ipmf3.nof_out_lif_ranges_get = dnx_data_field_base_ipmf3_nof_out_lif_ranges_get;
    dnx_data_field.base_ipmf3.fes_instruction_size_get = dnx_data_field_base_ipmf3_fes_instruction_size_get;
    dnx_data_field.base_ipmf3.fes_pgm_id_offset_get = dnx_data_field_base_ipmf3_fes_pgm_id_offset_get;
    dnx_data_field.base_ipmf3.dir_ext_single_key_size_get = dnx_data_field_base_ipmf3_dir_ext_single_key_size_get;
    dnx_data_field.base_ipmf3.dir_ext_first_key_get = dnx_data_field_base_ipmf3_dir_ext_first_key_get;
    dnx_data_field.base_ipmf3.exem_max_result_size_get = dnx_data_field_base_ipmf3_exem_max_result_size_get;
    dnx_data_field.base_ipmf3.nof_fes_used_by_sdk_get = dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_get;
    dnx_data_field.base_ipmf3.fes_key_select_for_zero_bit_get = dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_get;
    dnx_data_field.base_ipmf3.fes_shift_for_zero_bit_get = dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_get;
    dnx_data_field.base_ipmf3.exem_pdb_get = dnx_data_field_base_ipmf3_exem_pdb_get;

    
    
    
    dnx_data_field.base_epmf.feature_get = dnx_data_field_base_epmf_feature_get;

    
    dnx_data_field.base_epmf.nof_ffc_get = dnx_data_field_base_epmf_nof_ffc_get;
    dnx_data_field.base_epmf.nof_keys_get = dnx_data_field_base_epmf_nof_keys_get;
    dnx_data_field.base_epmf.nof_keys_alloc_get = dnx_data_field_base_epmf_nof_keys_alloc_get;
    dnx_data_field.base_epmf.nof_keys_alloc_for_tcam_get = dnx_data_field_base_epmf_nof_keys_alloc_for_tcam_get;
    dnx_data_field.base_epmf.nof_keys_alloc_for_exem_get = dnx_data_field_base_epmf_nof_keys_alloc_for_exem_get;
    dnx_data_field.base_epmf.nof_keys_alloc_for_dir_ext_get = dnx_data_field_base_epmf_nof_keys_alloc_for_dir_ext_get;
    dnx_data_field.base_epmf.nof_masks_per_fes_get = dnx_data_field_base_epmf_nof_masks_per_fes_get;
    dnx_data_field.base_epmf.nof_fes_id_per_array_get = dnx_data_field_base_epmf_nof_fes_id_per_array_get;
    dnx_data_field.base_epmf.nof_fes_array_get = dnx_data_field_base_epmf_nof_fes_array_get;
    dnx_data_field.base_epmf.nof_fes_instruction_per_context_get = dnx_data_field_base_epmf_nof_fes_instruction_per_context_get;
    dnx_data_field.base_epmf.nof_fes_programs_get = dnx_data_field_base_epmf_nof_fes_programs_get;
    dnx_data_field.base_epmf.nof_prog_per_fes_get = dnx_data_field_base_epmf_nof_prog_per_fes_get;
    dnx_data_field.base_epmf.nof_cs_lines_get = dnx_data_field_base_epmf_nof_cs_lines_get;
    dnx_data_field.base_epmf.nof_contexts_get = dnx_data_field_base_epmf_nof_contexts_get;
    dnx_data_field.base_epmf.nof_actions_get = dnx_data_field_base_epmf_nof_actions_get;
    dnx_data_field.base_epmf.nof_qualifiers_get = dnx_data_field_base_epmf_nof_qualifiers_get;
    dnx_data_field.base_epmf.nof_bits_in_fes_action_get = dnx_data_field_base_epmf_nof_bits_in_fes_action_get;
    dnx_data_field.base_epmf.nof_bits_in_fes_key_select_get = dnx_data_field_base_epmf_nof_bits_in_fes_key_select_get;
    dnx_data_field.base_epmf.fes_key_select_resolution_in_bits_get = dnx_data_field_base_epmf_fes_key_select_resolution_in_bits_get;
    dnx_data_field.base_epmf.nof_l4_ops_ranges_legacy_get = dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_get;
    dnx_data_field.base_epmf.fes_instruction_size_get = dnx_data_field_base_epmf_fes_instruction_size_get;
    dnx_data_field.base_epmf.fes_pgm_id_offset_get = dnx_data_field_base_epmf_fes_pgm_id_offset_get;
    dnx_data_field.base_epmf.dir_ext_single_key_size_get = dnx_data_field_base_epmf_dir_ext_single_key_size_get;
    dnx_data_field.base_epmf.dir_ext_first_key_get = dnx_data_field_base_epmf_dir_ext_first_key_get;
    dnx_data_field.base_epmf.exem_max_result_size_get = dnx_data_field_base_epmf_exem_max_result_size_get;
    dnx_data_field.base_epmf.fes_key_select_for_zero_bit_get = dnx_data_field_base_epmf_fes_key_select_for_zero_bit_get;
    dnx_data_field.base_epmf.fes_shift_for_zero_bit_get = dnx_data_field_base_epmf_fes_shift_for_zero_bit_get;
    dnx_data_field.base_epmf.exem_pdb_get = dnx_data_field_base_epmf_exem_pdb_get;

    
    
    
    dnx_data_field.base_ifwd2.feature_get = dnx_data_field_base_ifwd2_feature_get;

    
    dnx_data_field.base_ifwd2.nof_ffc_get = dnx_data_field_base_ifwd2_nof_ffc_get;
    dnx_data_field.base_ifwd2.nof_keys_get = dnx_data_field_base_ifwd2_nof_keys_get;
    dnx_data_field.base_ifwd2.nof_contexts_get = dnx_data_field_base_ifwd2_nof_contexts_get;
    dnx_data_field.base_ifwd2.nof_cs_lines_get = dnx_data_field_base_ifwd2_nof_cs_lines_get;

    
    
    
    dnx_data_field.stage.feature_get = dnx_data_field_stage_feature_get;

    

    
    dnx_data_field.stage.stage_info_get = dnx_data_field_stage_stage_info_get;
    dnx_data_field.stage.stage_info_info_get = dnx_data_field_stage_stage_info_info_get;
    
    
    dnx_data_field.external_tcam.feature_get = dnx_data_field_external_tcam_feature_get;

    
    dnx_data_field.external_tcam.max_single_key_size_get = dnx_data_field_external_tcam_max_single_key_size_get;
    dnx_data_field.external_tcam.nof_acl_keys_master_max_get = dnx_data_field_external_tcam_nof_acl_keys_master_max_get;
    dnx_data_field.external_tcam.nof_acl_keys_fg_max_get = dnx_data_field_external_tcam_nof_acl_keys_fg_max_get;
    dnx_data_field.external_tcam.nof_keys_total_get = dnx_data_field_external_tcam_nof_keys_total_get;
    dnx_data_field.external_tcam.min_acl_nof_ffc_get = dnx_data_field_external_tcam_min_acl_nof_ffc_get;
    dnx_data_field.external_tcam.max_fwd_context_num_for_one_apptype_get = dnx_data_field_external_tcam_max_fwd_context_num_for_one_apptype_get;
    dnx_data_field.external_tcam.max_acl_context_num_get = dnx_data_field_external_tcam_max_acl_context_num_get;
    dnx_data_field.external_tcam.size_apptype_profile_id_get = dnx_data_field_external_tcam_size_apptype_profile_id_get;
    dnx_data_field.external_tcam.key_bmp_get = dnx_data_field_external_tcam_key_bmp_get;
    dnx_data_field.external_tcam.apptype_user_1st_get = dnx_data_field_external_tcam_apptype_user_1st_get;
    dnx_data_field.external_tcam.apptype_user_nof_get = dnx_data_field_external_tcam_apptype_user_nof_get;
    dnx_data_field.external_tcam.max_payload_size_per_opcode_get = dnx_data_field_external_tcam_max_payload_size_per_opcode_get;
    dnx_data_field.external_tcam.max_payload_size_per_opcode_epmf_get = dnx_data_field_external_tcam_max_payload_size_per_opcode_epmf_get;
    dnx_data_field.external_tcam.max_epmf_nof_lookups_get = dnx_data_field_external_tcam_max_epmf_nof_lookups_get;
    dnx_data_field.external_tcam.nof_total_lookups_get = dnx_data_field_external_tcam_nof_total_lookups_get;
    dnx_data_field.external_tcam.multicore_fg_supports_core_all_get = dnx_data_field_external_tcam_multicore_fg_supports_core_all_get;
    dnx_data_field.external_tcam.type_get = dnx_data_field_external_tcam_type_get;

    
    dnx_data_field.external_tcam.ffc_to_quad_and_group_map_get = dnx_data_field_external_tcam_ffc_to_quad_and_group_map_get;
    dnx_data_field.external_tcam.ffc_to_quad_and_group_map_info_get = dnx_data_field_external_tcam_ffc_to_quad_and_group_map_info_get;
    dnx_data_field.external_tcam.epmf_payload_sizes_get = dnx_data_field_external_tcam_epmf_payload_sizes_get;
    dnx_data_field.external_tcam.epmf_payload_sizes_info_get = dnx_data_field_external_tcam_epmf_payload_sizes_info_get;
    
    
    dnx_data_field.tcam.feature_get = dnx_data_field_tcam_feature_get;

    
    dnx_data_field.tcam.access_hw_mix_ratio_supported_get = dnx_data_field_tcam_access_hw_mix_ratio_supported_get;
    dnx_data_field.tcam.direct_supported_get = dnx_data_field_tcam_direct_supported_get;
    dnx_data_field.tcam.key_size_half_get = dnx_data_field_tcam_key_size_half_get;
    dnx_data_field.tcam.key_size_single_get = dnx_data_field_tcam_key_size_single_get;
    dnx_data_field.tcam.key_size_double_get = dnx_data_field_tcam_key_size_double_get;
    dnx_data_field.tcam.dt_max_key_size_get = dnx_data_field_tcam_dt_max_key_size_get;
    dnx_data_field.tcam.dt_max_key_size_small_bank_get = dnx_data_field_tcam_dt_max_key_size_small_bank_get;
    dnx_data_field.tcam.action_size_half_get = dnx_data_field_tcam_action_size_half_get;
    dnx_data_field.tcam.action_size_single_get = dnx_data_field_tcam_action_size_single_get;
    dnx_data_field.tcam.action_size_double_get = dnx_data_field_tcam_action_size_double_get;
    dnx_data_field.tcam.key_mode_size_get = dnx_data_field_tcam_key_mode_size_get;
    dnx_data_field.tcam.entry_size_single_key_hw_get = dnx_data_field_tcam_entry_size_single_key_hw_get;
    dnx_data_field.tcam.entry_size_single_key_shadow_get = dnx_data_field_tcam_entry_size_single_key_shadow_get;
    dnx_data_field.tcam.entry_size_single_valid_bits_hw_get = dnx_data_field_tcam_entry_size_single_valid_bits_hw_get;
    dnx_data_field.tcam.hw_bank_size_get = dnx_data_field_tcam_hw_bank_size_get;
    dnx_data_field.tcam.nof_big_bank_lines_get = dnx_data_field_tcam_nof_big_bank_lines_get;
    dnx_data_field.tcam.nof_small_bank_lines_get = dnx_data_field_tcam_nof_small_bank_lines_get;
    dnx_data_field.tcam.nof_big_banks_get = dnx_data_field_tcam_nof_big_banks_get;
    dnx_data_field.tcam.nof_small_banks_get = dnx_data_field_tcam_nof_small_banks_get;
    dnx_data_field.tcam.nof_banks_get = dnx_data_field_tcam_nof_banks_get;
    dnx_data_field.tcam.nof_payload_tables_get = dnx_data_field_tcam_nof_payload_tables_get;
    dnx_data_field.tcam.nof_access_profiles_get = dnx_data_field_tcam_nof_access_profiles_get;
    dnx_data_field.tcam.action_width_selector_size_get = dnx_data_field_tcam_action_width_selector_size_get;
    dnx_data_field.tcam.nof_entries_80_bits_get = dnx_data_field_tcam_nof_entries_80_bits_get;
    dnx_data_field.tcam.nof_entries_160_bits_get = dnx_data_field_tcam_nof_entries_160_bits_get;
    dnx_data_field.tcam.tcam_banks_size_get = dnx_data_field_tcam_tcam_banks_size_get;
    dnx_data_field.tcam.tcam_banks_last_index_get = dnx_data_field_tcam_tcam_banks_last_index_get;
    dnx_data_field.tcam.nof_tcam_handlers_get = dnx_data_field_tcam_nof_tcam_handlers_get;
    dnx_data_field.tcam.max_prefix_size_get = dnx_data_field_tcam_max_prefix_size_get;
    dnx_data_field.tcam.max_prefix_value_get = dnx_data_field_tcam_max_prefix_value_get;
    dnx_data_field.tcam.nof_keys_max_get = dnx_data_field_tcam_nof_keys_max_get;
    dnx_data_field.tcam.access_profile_half_key_mode_get = dnx_data_field_tcam_access_profile_half_key_mode_get;
    dnx_data_field.tcam.access_profile_single_key_mode_get = dnx_data_field_tcam_access_profile_single_key_mode_get;
    dnx_data_field.tcam.access_profile_double_key_mode_get = dnx_data_field_tcam_access_profile_double_key_mode_get;
    dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get = dnx_data_field_tcam_tcam_entries_per_hit_indication_entry_get;
    dnx_data_field.tcam.max_tcam_priority_get = dnx_data_field_tcam_max_tcam_priority_get;
    dnx_data_field.tcam.nof_banks_hitbit_units_get = dnx_data_field_tcam_nof_banks_hitbit_units_get;
    dnx_data_field.tcam.nof_big_bank_lines_per_hitbit_unit_get = dnx_data_field_tcam_nof_big_bank_lines_per_hitbit_unit_get;
    dnx_data_field.tcam.nof_small_bank_lines_per_hitbit_unit_get = dnx_data_field_tcam_nof_small_bank_lines_per_hitbit_unit_get;
    dnx_data_field.tcam.app_db_id_size_get = dnx_data_field_tcam_app_db_id_size_get;
    dnx_data_field.tcam.tcam_indirect_command_version_get = dnx_data_field_tcam_tcam_indirect_command_version_get;

    
    
    
    dnx_data_field.group.feature_get = dnx_data_field_group_feature_get;

    
    dnx_data_field.group.nof_fgs_get = dnx_data_field_group_nof_fgs_get;
    dnx_data_field.group.nof_action_per_fg_get = dnx_data_field_group_nof_action_per_fg_get;
    dnx_data_field.group.nof_quals_per_fg_get = dnx_data_field_group_nof_quals_per_fg_get;
    dnx_data_field.group.nof_keys_per_fg_max_get = dnx_data_field_group_nof_keys_per_fg_max_get;
    dnx_data_field.group.id_fec_get = dnx_data_field_group_id_fec_get;
    dnx_data_field.group.payload_max_size_get = dnx_data_field_group_payload_max_size_get;

    
    
    
    dnx_data_field.efes.feature_get = dnx_data_field_efes_feature_get;

    
    dnx_data_field.efes.max_nof_key_selects_per_field_io_get = dnx_data_field_efes_max_nof_key_selects_per_field_io_get;
    dnx_data_field.efes.max_nof_field_ios_get = dnx_data_field_efes_max_nof_field_ios_get;
    dnx_data_field.efes.epmf_exem_zero_padding_added_to_field_io_get = dnx_data_field_efes_epmf_exem_zero_padding_added_to_field_io_get;

    
    dnx_data_field.efes.key_select_properties_get = dnx_data_field_efes_key_select_properties_get;
    dnx_data_field.efes.key_select_properties_info_get = dnx_data_field_efes_key_select_properties_info_get;
    
    
    dnx_data_field.fem.feature_get = dnx_data_field_fem_feature_get;

    
    dnx_data_field.fem.max_nof_key_selects_per_field_io_get = dnx_data_field_fem_max_nof_key_selects_per_field_io_get;
    dnx_data_field.fem.nof_condition_bits_get = dnx_data_field_fem_nof_condition_bits_get;

    
    dnx_data_field.fem.key_select_properties_get = dnx_data_field_fem_key_select_properties_get;
    dnx_data_field.fem.key_select_properties_info_get = dnx_data_field_fem_key_select_properties_info_get;
    
    
    dnx_data_field.context.feature_get = dnx_data_field_context_feature_get;

    
    dnx_data_field.context.default_context_get = dnx_data_field_context_default_context_get;
    dnx_data_field.context.default_ftmh_mc_context_get = dnx_data_field_context_default_ftmh_mc_context_get;
    dnx_data_field.context.default_itmh_context_get = dnx_data_field_context_default_itmh_context_get;
    dnx_data_field.context.default_itmh_pph_context_get = dnx_data_field_context_default_itmh_pph_context_get;
    dnx_data_field.context.default_itmh_pph_fhei_context_get = dnx_data_field_context_default_itmh_pph_fhei_context_get;
    dnx_data_field.context.default_itmh_1588_context_get = dnx_data_field_context_default_itmh_1588_context_get;
    dnx_data_field.context.default_itmh_1588_pph_context_get = dnx_data_field_context_default_itmh_1588_pph_context_get;
    dnx_data_field.context.default_itmh_1588_pph_fhei_context_get = dnx_data_field_context_default_itmh_1588_pph_fhei_context_get;
    dnx_data_field.context.default_itmh_pph_routing_context_get = dnx_data_field_context_default_itmh_pph_routing_context_get;
    dnx_data_field.context.default_j1_itmh_context_get = dnx_data_field_context_default_j1_itmh_context_get;
    dnx_data_field.context.default_j1_itmh_pph_context_get = dnx_data_field_context_default_j1_itmh_pph_context_get;
    dnx_data_field.context.default_oam_context_get = dnx_data_field_context_default_oam_context_get;
    dnx_data_field.context.default_oam_reflector_context_get = dnx_data_field_context_default_oam_reflector_context_get;
    dnx_data_field.context.default_oam_upmep_context_get = dnx_data_field_context_default_oam_upmep_context_get;
    dnx_data_field.context.default_j1_learning_2ndpass_context_get = dnx_data_field_context_default_j1_learning_2ndpass_context_get;
    dnx_data_field.context.default_rch_remove_context_get = dnx_data_field_context_default_rch_remove_context_get;
    dnx_data_field.context.default_rch_extended_encap_context_get = dnx_data_field_context_default_rch_extended_encap_context_get;
    dnx_data_field.context.default_nat_context_get = dnx_data_field_context_default_nat_context_get;
    dnx_data_field.context.nof_hash_keys_get = dnx_data_field_context_nof_hash_keys_get;
    dnx_data_field.context.default_itmh_pph_unsupported_ext_ipmf2_context_get = dnx_data_field_context_default_itmh_pph_unsupported_ext_ipmf2_context_get;
    dnx_data_field.context.default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get = dnx_data_field_context_default_itmh_pph_fhei_unsupported_ext_ipmf2_context_get;
    dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get = dnx_data_field_context_default_itmh_pph_fhei_vlan_ipmf2_context_get;
    dnx_data_field.context.default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get = dnx_data_field_context_default_itmh_1588_pph_fhei_unsupported_ext_ipmf2_context_get;
    dnx_data_field.context.default_itmh_1588_pph_unsupported_ext_ipmf2_context_get = dnx_data_field_context_default_itmh_1588_pph_unsupported_ext_ipmf2_context_get;
    dnx_data_field.context.default_itmh_1588_pph_fhei_vlan_ipmf2_context_get = dnx_data_field_context_default_itmh_1588_pph_fhei_vlan_ipmf2_context_get;
    dnx_data_field.context.default_ftmh_mc_ipmf3_context_get = dnx_data_field_context_default_ftmh_mc_ipmf3_context_get;

    
    
    
    dnx_data_field.preselector.feature_get = dnx_data_field_preselector_feature_get;

    
    dnx_data_field.preselector.default_j1_itmh_presel_id_ipmf1_get = dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_j1_itmh_pph_presel_id_ipmf1_get = dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_j1_itmh_pph_oamp_presel_id_ipmf1_get = dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_j1_learn_presel_id_1st_pass_ipmf1_get = dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_get;
    dnx_data_field.preselector.default_j1_learn_presel_id_2nd_pass_ipmf1_get = dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_get;
    dnx_data_field.preselector.default_nat_presel_id_ipmf1_get = dnx_data_field_preselector_default_nat_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_ftmh_mc_presel_id_ipmf1_get = dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_fhei_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_pph_fhei_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_1588_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_1588_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_1588_pph_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_1588_pph_fhei_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_1588_pph_fhei_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_eth_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_pph_eth_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_ipv4_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_pph_ipv4_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_ipv6_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_pph_ipv6_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_mpls_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_pph_mpls_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_y1711_rcpu_presel_id_ipmf1_get = dnx_data_field_preselector_default_itmh_y1711_rcpu_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_psp_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get = dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf2_get;
    dnx_data_field.preselector.srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_trap_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_icmp_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usd_eth_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usd_eth_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usd_ipv4_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usd_ipv4_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usd_ipv6_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usd_ipv6_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usd_mpls_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usd_mpls_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_usid_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_usid_no_default_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_endpoint_gsid_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usp_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usp_trap_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usp_trap_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usp_fwd_trap_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usp_fwd_trap_presel_id_ipmf1_get;
    dnx_data_field.preselector.srv6_egress_usp_icmp_presel_id_ipmf1_get = dnx_data_field_preselector_srv6_egress_usp_icmp_presel_id_ipmf1_get;
    dnx_data_field.preselector.mpls_bier_presel_id_ipmf1_get = dnx_data_field_preselector_mpls_bier_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_rcy_mirror_sys_hdr_presel_id_ipmf1_get = dnx_data_field_preselector_default_rcy_mirror_sys_hdr_presel_id_ipmf1_get;
    dnx_data_field.preselector.default_itmh_pph_unsupported_ext_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_pph_unsupported_ext_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get = dnx_data_field_preselector_default_itmh_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get;
    dnx_data_field.preselector.default_itmh_pph_fhei_trap_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_pph_fhei_trap_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_pph_fhei_vlan_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_pph_fhei_vlan_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_1588_pph_unsupported_ext_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get = dnx_data_field_preselector_default_itmh_1588_pph_fhei_unsupported_ext_presel_id_2nd_ipmf2_get;
    dnx_data_field.preselector.default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_1588_pph_fhei_trap_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get = dnx_data_field_preselector_default_itmh_1588_pph_fhei_vlan_presel_id_ipmf2_get;
    dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_itmh_pph_trap_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_pph_trap_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_itmh_pph_vlan_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_pph_vlan_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_itmh_1588_pph_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_1588_pph_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_itmh_1588_pph_trap_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_1588_pph_trap_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_itmh_1588_pph_vlan_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_1588_pph_vlan_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_itmh_pph_routing_presel_id_ipmf3_get = dnx_data_field_preselector_default_itmh_pph_routing_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_j1_itmh_pph_presel_id_ipmf3_get = dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_ftmh_mc_presel_id_ipmf3_get = dnx_data_field_preselector_default_ftmh_mc_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_oam_roo_ipv4_presel_id_ipmf3_get = dnx_data_field_preselector_default_oam_roo_ipv4_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_oam_roo_ipv6_presel_id_ipmf3_get = dnx_data_field_preselector_default_oam_roo_ipv6_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get = dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get = dnx_data_field_preselector_default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get = dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get = dnx_data_field_preselector_default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get = dnx_data_field_preselector_default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get = dnx_data_field_preselector_default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_rch_remove_presel_id_ipmf3_get = dnx_data_field_preselector_default_rch_remove_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_rch_extended_encap_presel_id_ipmf3_get = dnx_data_field_preselector_default_rch_extended_encap_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_j1_php_presel_id_ipmf3_get = dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_get;
    dnx_data_field.preselector.default_j1_swap_presel_id_ipmf3_get = dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_partial_fwd_layer_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_partial_fwd_layer_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_endpoint_usid_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_endpoint_usid_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_endpoint_usid_no_default_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_endpoint_usid_no_default_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_endpoint_gsid_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_endpoint_gsid_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_endpoint_psp_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_endpoint_psp_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_psp_ext_to_endpoint_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_psp_ext_to_endpoint_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_egress_usp_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_egress_usp_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get;
    dnx_data_field.preselector.srv6_egress_usd_presel_id_ipmf3_get = dnx_data_field_preselector_srv6_egress_usd_presel_id_ipmf3_get;
    dnx_data_field.preselector.mpls_bier_presel_id_ipmf3_get = dnx_data_field_preselector_mpls_bier_presel_id_ipmf3_get;
    dnx_data_field.preselector.ebtr_const_fwd_layer_eth_epmf_get = dnx_data_field_preselector_ebtr_const_fwd_layer_eth_epmf_get;
    dnx_data_field.preselector.srv6_ext_termination_btr_epmf_get = dnx_data_field_preselector_srv6_ext_termination_btr_epmf_get;
    dnx_data_field.preselector.default_j1_same_port_presel_id_epmf_get = dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_get;
    dnx_data_field.preselector.default_learn_limit_presel_id_epmf_get = dnx_data_field_preselector_default_learn_limit_presel_id_epmf_get;
    dnx_data_field.preselector.srv6_endpoint_hbh_bta_presel_id_epmf_get = dnx_data_field_preselector_srv6_endpoint_hbh_bta_presel_id_epmf_get;
    dnx_data_field.preselector.srv6_transit_hbh_bta_presel_id_epmf_get = dnx_data_field_preselector_srv6_transit_hbh_bta_presel_id_epmf_get;
    dnx_data_field.preselector.srv6_endpoint_psp_bta_presel_id_epmf_get = dnx_data_field_preselector_srv6_endpoint_psp_bta_presel_id_epmf_get;
    dnx_data_field.preselector.default_mpls_8b_fhei_presel_id_epmf_get = dnx_data_field_preselector_default_mpls_8b_fhei_presel_id_epmf_get;
    dnx_data_field.preselector.default_non_pph_presel_id_epmf_get = dnx_data_field_preselector_default_non_pph_presel_id_epmf_get;
    dnx_data_field.preselector.default_nat_presel_id_epmf_get = dnx_data_field_preselector_default_nat_presel_id_epmf_get;
    dnx_data_field.preselector.num_cs_inlif_profile_entries_get = dnx_data_field_preselector_num_cs_inlif_profile_entries_get;
    dnx_data_field.preselector.nof_uint32_in_cs_qual_hw_get = dnx_data_field_preselector_nof_uint32_in_cs_qual_hw_get;

    
    
    
    dnx_data_field.qual.feature_get = dnx_data_field_qual_feature_get;

    
    dnx_data_field.qual.user_1st_get = dnx_data_field_qual_user_1st_get;
    dnx_data_field.qual.user_nof_get = dnx_data_field_qual_user_nof_get;
    dnx_data_field.qual.predefined_nof_get = dnx_data_field_qual_predefined_nof_get;
    dnx_data_field.qual.layer_record_nof_get = dnx_data_field_qual_layer_record_nof_get;
    dnx_data_field.qual.vw_1st_get = dnx_data_field_qual_vw_1st_get;
    dnx_data_field.qual.vw_nof_get = dnx_data_field_qual_vw_nof_get;
    dnx_data_field.qual.max_bits_in_qual_get = dnx_data_field_qual_max_bits_in_qual_get;
    dnx_data_field.qual.ingress_pbus_header_length_get = dnx_data_field_qual_ingress_pbus_header_length_get;
    dnx_data_field.qual.egress_pbus_header_length_get = dnx_data_field_qual_egress_pbus_header_length_get;
    dnx_data_field.qual.ifwd2_pbus_size_get = dnx_data_field_qual_ifwd2_pbus_size_get;
    dnx_data_field.qual.ipmf1_pbus_size_get = dnx_data_field_qual_ipmf1_pbus_size_get;
    dnx_data_field.qual.ipmf2_pbus_size_get = dnx_data_field_qual_ipmf2_pbus_size_get;
    dnx_data_field.qual.ipmf3_pbus_size_get = dnx_data_field_qual_ipmf3_pbus_size_get;
    dnx_data_field.qual.epmf_pbus_size_get = dnx_data_field_qual_epmf_pbus_size_get;
    dnx_data_field.qual.l4_ops_pbus_size_get = dnx_data_field_qual_l4_ops_pbus_size_get;
    dnx_data_field.qual.ingress_nof_layer_records_get = dnx_data_field_qual_ingress_nof_layer_records_get;
    dnx_data_field.qual.ingress_layer_record_size_get = dnx_data_field_qual_ingress_layer_record_size_get;
    dnx_data_field.qual.egress_nof_layer_records_get = dnx_data_field_qual_egress_nof_layer_records_get;
    dnx_data_field.qual.egress_layer_record_size_get = dnx_data_field_qual_egress_layer_record_size_get;
    dnx_data_field.qual.kbp_extra_offset_after_layer_record_offset_get = dnx_data_field_qual_kbp_extra_offset_after_layer_record_offset_get;
    dnx_data_field.qual.special_metadata_qual_max_parts_get = dnx_data_field_qual_special_metadata_qual_max_parts_get;

    
    
    
    dnx_data_field.action.feature_get = dnx_data_field_action_feature_get;

    
    dnx_data_field.action.user_1st_get = dnx_data_field_action_user_1st_get;
    dnx_data_field.action.user_nof_get = dnx_data_field_action_user_nof_get;
    dnx_data_field.action.predefined_nof_get = dnx_data_field_action_predefined_nof_get;
    dnx_data_field.action.vw_1st_get = dnx_data_field_action_vw_1st_get;
    dnx_data_field.action.vw_nof_get = dnx_data_field_action_vw_nof_get;

    
    dnx_data_field.action.params_get = dnx_data_field_action_params_get;
    dnx_data_field.action.params_info_get = dnx_data_field_action_params_info_get;
    
    
    dnx_data_field.virtual_wire.feature_get = dnx_data_field_virtual_wire_feature_get;

    
    dnx_data_field.virtual_wire.signals_nof_get = dnx_data_field_virtual_wire_signals_nof_get;
    dnx_data_field.virtual_wire.actions_per_signal_nof_get = dnx_data_field_virtual_wire_actions_per_signal_nof_get;
    dnx_data_field.virtual_wire.general_data_user_general_containers_size_get = dnx_data_field_virtual_wire_general_data_user_general_containers_size_get;
    dnx_data_field.virtual_wire.general_data_nof_containers_get = dnx_data_field_virtual_wire_general_data_nof_containers_get;
    dnx_data_field.virtual_wire.ipmf1_general_data_index_get = dnx_data_field_virtual_wire_ipmf1_general_data_index_get;

    
    dnx_data_field.virtual_wire.signal_mapping_get = dnx_data_field_virtual_wire_signal_mapping_get;
    dnx_data_field.virtual_wire.signal_mapping_info_get = dnx_data_field_virtual_wire_signal_mapping_info_get;
    
    
    dnx_data_field.profile_bits.feature_get = dnx_data_field_profile_bits_feature_get;

    
    dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get = dnx_data_field_profile_bits_ingress_pp_port_key_gen_var_size_get;
    dnx_data_field.profile_bits.max_port_profile_size_get = dnx_data_field_profile_bits_max_port_profile_size_get;
    dnx_data_field.profile_bits.nof_bits_in_port_profile_get = dnx_data_field_profile_bits_nof_bits_in_port_profile_get;
    dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get = dnx_data_field_profile_bits_nof_bits_in_ingress_pp_port_general_data_get;
    dnx_data_field.profile_bits.nof_ing_in_lif_get = dnx_data_field_profile_bits_nof_ing_in_lif_get;
    dnx_data_field.profile_bits.nof_ing_eth_rif_get = dnx_data_field_profile_bits_nof_ing_eth_rif_get;

    
    
    
    dnx_data_field.dir_ext.feature_get = dnx_data_field_dir_ext_feature_get;

    
    dnx_data_field.dir_ext.half_key_size_get = dnx_data_field_dir_ext_half_key_size_get;
    dnx_data_field.dir_ext.single_key_size_get = dnx_data_field_dir_ext_single_key_size_get;

    
    
    
    dnx_data_field.state_table.feature_get = dnx_data_field_state_table_feature_get;

    
    dnx_data_field.state_table.supported_get = dnx_data_field_state_table_supported_get;
    dnx_data_field.state_table.address_size_entry_max_get = dnx_data_field_state_table_address_size_entry_max_get;
    dnx_data_field.state_table.data_size_entry_max_get = dnx_data_field_state_table_data_size_entry_max_get;
    dnx_data_field.state_table.address_max_entry_max_get = dnx_data_field_state_table_address_max_entry_max_get;
    dnx_data_field.state_table.wr_bit_size_rw_get = dnx_data_field_state_table_wr_bit_size_rw_get;
    dnx_data_field.state_table.opcode_size_rmw_get = dnx_data_field_state_table_opcode_size_rmw_get;
    dnx_data_field.state_table.ipmf2_key_select_supported_get = dnx_data_field_state_table_ipmf2_key_select_supported_get;
    dnx_data_field.state_table.data_size_get = dnx_data_field_state_table_data_size_get;
    dnx_data_field.state_table.state_table_stage_key_get = dnx_data_field_state_table_state_table_stage_key_get;
    dnx_data_field.state_table.address_size_get = dnx_data_field_state_table_address_size_get;
    dnx_data_field.state_table.address_max_get = dnx_data_field_state_table_address_max_get;
    dnx_data_field.state_table.wr_bit_size_get = dnx_data_field_state_table_wr_bit_size_get;
    dnx_data_field.state_table.key_size_get = dnx_data_field_state_table_key_size_get;

    
    
    
    dnx_data_field.sce.feature_get = dnx_data_field_sce_feature_get;

    

    
    
    
    dnx_data_field.mdb_dt.feature_get = dnx_data_field_mdb_dt_feature_get;

    
    dnx_data_field.mdb_dt.supported_get = dnx_data_field_mdb_dt_supported_get;
    dnx_data_field.mdb_dt.key_size_get = dnx_data_field_mdb_dt_key_size_get;
    dnx_data_field.mdb_dt.action_size_large_get = dnx_data_field_mdb_dt_action_size_large_get;
    dnx_data_field.mdb_dt.ipmf1_key_select_supported_get = dnx_data_field_mdb_dt_ipmf1_key_select_supported_get;
    dnx_data_field.mdb_dt.ipmf1_key_select_get = dnx_data_field_mdb_dt_ipmf1_key_select_get;
    dnx_data_field.mdb_dt.ipmf2_key_select_supported_get = dnx_data_field_mdb_dt_ipmf2_key_select_supported_get;
    dnx_data_field.mdb_dt.ipmf2_key_select_get = dnx_data_field_mdb_dt_ipmf2_key_select_get;
    dnx_data_field.mdb_dt.ipmf3_key_select_supported_get = dnx_data_field_mdb_dt_ipmf3_key_select_supported_get;
    dnx_data_field.mdb_dt.ipmf3_key_select_get = dnx_data_field_mdb_dt_ipmf3_key_select_get;
    dnx_data_field.mdb_dt.ipmf1_ipmf2_concurrent_support_get = dnx_data_field_mdb_dt_ipmf1_ipmf2_concurrent_support_get;
    dnx_data_field.mdb_dt.signal_block_name_is_map_get = dnx_data_field_mdb_dt_signal_block_name_is_map_get;
    dnx_data_field.mdb_dt.signal_key_is_entire_key_get = dnx_data_field_mdb_dt_signal_key_is_entire_key_get;
    dnx_data_field.mdb_dt.pmf_map_stage_get = dnx_data_field_mdb_dt_pmf_map_stage_get;

    
    
    
    dnx_data_field.hash.feature_get = dnx_data_field_hash_feature_get;

    
    dnx_data_field.hash.max_key_size_get = dnx_data_field_hash_max_key_size_get;

    
    
    
    dnx_data_field.udh.feature_get = dnx_data_field_udh_feature_get;

    
    dnx_data_field.udh.ext_supported_get = dnx_data_field_udh_ext_supported_get;
    dnx_data_field.udh.type_count_get = dnx_data_field_udh_type_count_get;

    dnx_data_field.udh.type_0_length_get = dnx_data_field_udh_type_0_length_get;


    dnx_data_field.udh.type_1_length_get = dnx_data_field_udh_type_1_length_get;


    dnx_data_field.udh.type_2_length_get = dnx_data_field_udh_type_2_length_get;


    dnx_data_field.udh.type_3_length_get = dnx_data_field_udh_type_3_length_get;

    dnx_data_field.udh.field_class_id_size_0_get = dnx_data_field_udh_field_class_id_size_0_get;
    dnx_data_field.udh.field_class_id_size_1_get = dnx_data_field_udh_field_class_id_size_1_get;
    dnx_data_field.udh.field_class_id_size_2_get = dnx_data_field_udh_field_class_id_size_2_get;
    dnx_data_field.udh.field_class_id_size_3_get = dnx_data_field_udh_field_class_id_size_3_get;
    dnx_data_field.udh.field_class_id_total_size_get = dnx_data_field_udh_field_class_id_total_size_get;

    
    
    
    dnx_data_field.system_headers.feature_get = dnx_data_field_system_headers_feature_get;

    
    dnx_data_field.system_headers.nof_profiles_get = dnx_data_field_system_headers_nof_profiles_get;
    dnx_data_field.system_headers.pph_learn_ext_disable_get = dnx_data_field_system_headers_pph_learn_ext_disable_get;

    
    dnx_data_field.system_headers.system_header_profiles_get = dnx_data_field_system_headers_system_header_profiles_get;
    dnx_data_field.system_headers.system_header_profiles_info_get = dnx_data_field_system_headers_system_header_profiles_info_get;
    
    
    dnx_data_field.exem.feature_get = dnx_data_field_exem_feature_get;

    
    dnx_data_field.exem.small_app_db_id_size_get = dnx_data_field_exem_small_app_db_id_size_get;
    dnx_data_field.exem.large_app_db_id_size_get = dnx_data_field_exem_large_app_db_id_size_get;
    dnx_data_field.exem.small_max_key_size_get = dnx_data_field_exem_small_max_key_size_get;
    dnx_data_field.exem.large_max_key_size_get = dnx_data_field_exem_large_max_key_size_get;
    dnx_data_field.exem.max_result_size_all_stages_get = dnx_data_field_exem_max_result_size_all_stages_get;
    dnx_data_field.exem.small_max_container_size_get = dnx_data_field_exem_small_max_container_size_get;
    dnx_data_field.exem.large_max_container_size_get = dnx_data_field_exem_large_max_container_size_get;
    dnx_data_field.exem.vmv_hw_max_size_get = dnx_data_field_exem_vmv_hw_max_size_get;
    dnx_data_field.exem.small_min_app_db_id_range_get = dnx_data_field_exem_small_min_app_db_id_range_get;
    dnx_data_field.exem.large_min_app_db_id_range_get = dnx_data_field_exem_large_min_app_db_id_range_get;
    dnx_data_field.exem.small_max_app_db_id_range_get = dnx_data_field_exem_small_max_app_db_id_range_get;
    dnx_data_field.exem.large_max_app_db_id_range_get = dnx_data_field_exem_large_max_app_db_id_range_get;
    dnx_data_field.exem.small_ipmf2_key_get = dnx_data_field_exem_small_ipmf2_key_get;
    dnx_data_field.exem.small_ipmf2_key_part_get = dnx_data_field_exem_small_ipmf2_key_part_get;
    dnx_data_field.exem.small_ipmf2_key_hw_value_get = dnx_data_field_exem_small_ipmf2_key_hw_value_get;
    dnx_data_field.exem.small_ipmf2_key_hw_bits_get = dnx_data_field_exem_small_ipmf2_key_hw_bits_get;
    dnx_data_field.exem.ipmf1_key_configurable_get = dnx_data_field_exem_ipmf1_key_configurable_get;
    dnx_data_field.exem.ipmf2_key_configurable_get = dnx_data_field_exem_ipmf2_key_configurable_get;
    dnx_data_field.exem.ipmf1_key_get = dnx_data_field_exem_ipmf1_key_get;
    dnx_data_field.exem.debug_key_signal_includes_zero_padding_get = dnx_data_field_exem_debug_key_signal_includes_zero_padding_get;

    dnx_data_field.exem.pmf_sexem3_stage_get = dnx_data_field_exem_pmf_sexem3_stage_get;


    
    
    
    dnx_data_field.exem_learn_flush_machine.feature_get = dnx_data_field_exem_learn_flush_machine_feature_get;

    
    dnx_data_field.exem_learn_flush_machine.flush_machine_nof_entries_get = dnx_data_field_exem_learn_flush_machine_flush_machine_nof_entries_get;
    dnx_data_field.exem_learn_flush_machine.entry_extra_bits_for_hash_get = dnx_data_field_exem_learn_flush_machine_entry_extra_bits_for_hash_get;
    dnx_data_field.exem_learn_flush_machine.sexem_entry_max_size_get = dnx_data_field_exem_learn_flush_machine_sexem_entry_max_size_get;
    dnx_data_field.exem_learn_flush_machine.lexem_entry_max_size_get = dnx_data_field_exem_learn_flush_machine_lexem_entry_max_size_get;
    dnx_data_field.exem_learn_flush_machine.command_bit_transplant_get = dnx_data_field_exem_learn_flush_machine_command_bit_transplant_get;
    dnx_data_field.exem_learn_flush_machine.command_bit_delete_get = dnx_data_field_exem_learn_flush_machine_command_bit_delete_get;
    dnx_data_field.exem_learn_flush_machine.command_bit_clear_hit_bit_get = dnx_data_field_exem_learn_flush_machine_command_bit_clear_hit_bit_get;
    dnx_data_field.exem_learn_flush_machine.nof_bits_source_get = dnx_data_field_exem_learn_flush_machine_nof_bits_source_get;
    dnx_data_field.exem_learn_flush_machine.source_bit_for_scan_get = dnx_data_field_exem_learn_flush_machine_source_bit_for_scan_get;
    dnx_data_field.exem_learn_flush_machine.source_bit_for_pipe_get = dnx_data_field_exem_learn_flush_machine_source_bit_for_pipe_get;
    dnx_data_field.exem_learn_flush_machine.source_bit_for_mrq_get = dnx_data_field_exem_learn_flush_machine_source_bit_for_mrq_get;
    dnx_data_field.exem_learn_flush_machine.nof_bits_accessed_get = dnx_data_field_exem_learn_flush_machine_nof_bits_accessed_get;
    dnx_data_field.exem_learn_flush_machine.nof_flush_profiles_get = dnx_data_field_exem_learn_flush_machine_nof_flush_profiles_get;

    
    dnx_data_field.exem_learn_flush_machine.sexem_flush_tcam_tcam_memories_get = dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_get;
    dnx_data_field.exem_learn_flush_machine.sexem_flush_tcam_tcam_memories_info_get = dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_tcam_memories_info_get;
    dnx_data_field.exem_learn_flush_machine.lexem_flush_tcam_tcam_memories_get = dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_get;
    dnx_data_field.exem_learn_flush_machine.lexem_flush_tcam_tcam_memories_info_get = dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_tcam_memories_info_get;
    dnx_data_field.exem_learn_flush_machine.sexem_flush_tcam_data_rules_get = dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_get;
    dnx_data_field.exem_learn_flush_machine.sexem_flush_tcam_data_rules_info_get = dnx_data_field_exem_learn_flush_machine_sexem_flush_tcam_data_rules_info_get;
    dnx_data_field.exem_learn_flush_machine.lexem_flush_tcam_data_rules_get = dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_get;
    dnx_data_field.exem_learn_flush_machine.lexem_flush_tcam_data_rules_info_get = dnx_data_field_exem_learn_flush_machine_lexem_flush_tcam_data_rules_info_get;
    
    
    dnx_data_field.ace.feature_get = dnx_data_field_ace_feature_get;

    
    dnx_data_field.ace.supported_get = dnx_data_field_ace_supported_get;
    dnx_data_field.ace.ace_id_size_get = dnx_data_field_ace_ace_id_size_get;
    dnx_data_field.ace.key_size_get = dnx_data_field_ace_key_size_get;
    dnx_data_field.ace.app_db_id_size_get = dnx_data_field_ace_app_db_id_size_get;
    dnx_data_field.ace.payload_size_get = dnx_data_field_ace_payload_size_get;
    dnx_data_field.ace.min_key_range_pmf_get = dnx_data_field_ace_min_key_range_pmf_get;
    dnx_data_field.ace.max_key_range_pmf_get = dnx_data_field_ace_max_key_range_pmf_get;
    dnx_data_field.ace.min_ace_id_dynamic_range_get = dnx_data_field_ace_min_ace_id_dynamic_range_get;
    dnx_data_field.ace.nof_ace_id_get = dnx_data_field_ace_nof_ace_id_get;
    dnx_data_field.ace.nof_action_per_ace_format_get = dnx_data_field_ace_nof_action_per_ace_format_get;
    dnx_data_field.ace.row_size_get = dnx_data_field_ace_row_size_get;
    dnx_data_field.ace.min_entry_size_get = dnx_data_field_ace_min_entry_size_get;
    dnx_data_field.ace.ace_id_pmf_alloc_first_get = dnx_data_field_ace_ace_id_pmf_alloc_first_get;
    dnx_data_field.ace.ace_id_pmf_alloc_last_get = dnx_data_field_ace_ace_id_pmf_alloc_last_get;
    dnx_data_field.ace.nof_masks_per_fes_get = dnx_data_field_ace_nof_masks_per_fes_get;
    dnx_data_field.ace.nof_fes_id_per_array_get = dnx_data_field_ace_nof_fes_id_per_array_get;
    dnx_data_field.ace.nof_fes_array_get = dnx_data_field_ace_nof_fes_array_get;
    dnx_data_field.ace.nof_fes_instruction_per_context_get = dnx_data_field_ace_nof_fes_instruction_per_context_get;
    dnx_data_field.ace.nof_fes_programs_get = dnx_data_field_ace_nof_fes_programs_get;
    dnx_data_field.ace.nof_prog_per_fes_get = dnx_data_field_ace_nof_prog_per_fes_get;
    dnx_data_field.ace.nof_bits_in_fes_action_get = dnx_data_field_ace_nof_bits_in_fes_action_get;
    dnx_data_field.ace.fes_shift_for_zero_bit_get = dnx_data_field_ace_fes_shift_for_zero_bit_get;
    dnx_data_field.ace.fes_instruction_size_get = dnx_data_field_ace_fes_instruction_size_get;
    dnx_data_field.ace.fes_shift_offset_get = dnx_data_field_ace_fes_shift_offset_get;
    dnx_data_field.ace.fes_invalid_bits_offset_get = dnx_data_field_ace_fes_invalid_bits_offset_get;
    dnx_data_field.ace.fes_type_offset_get = dnx_data_field_ace_fes_type_offset_get;
    dnx_data_field.ace.fes_ace_action_offset_get = dnx_data_field_ace_fes_ace_action_offset_get;
    dnx_data_field.ace.fes_chosen_mask_offset_get = dnx_data_field_ace_fes_chosen_mask_offset_get;
    dnx_data_field.ace.hw_invalid_action_id_get = dnx_data_field_ace_hw_invalid_action_id_get;

    
    
    
    dnx_data_field.entry.feature_get = dnx_data_field_entry_feature_get;

    
    dnx_data_field.entry.dir_ext_nof_fields_get = dnx_data_field_entry_dir_ext_nof_fields_get;
    dnx_data_field.entry.nof_action_params_per_entry_get = dnx_data_field_entry_nof_action_params_per_entry_get;
    dnx_data_field.entry.nof_qual_params_per_entry_get = dnx_data_field_entry_nof_qual_params_per_entry_get;

    
    
    
    dnx_data_field.L4_Ops.feature_get = dnx_data_field_L4_Ops_feature_get;

    
    dnx_data_field.L4_Ops.udp_position_get = dnx_data_field_L4_Ops_udp_position_get;
    dnx_data_field.L4_Ops.tcp_position_get = dnx_data_field_L4_Ops_tcp_position_get;
    dnx_data_field.L4_Ops.nof_range_entries_get = dnx_data_field_L4_Ops_nof_range_entries_get;
    dnx_data_field.L4_Ops.nof_ext_encoders_get = dnx_data_field_L4_Ops_nof_ext_encoders_get;
    dnx_data_field.L4_Ops.nof_ext_types_get = dnx_data_field_L4_Ops_nof_ext_types_get;
    dnx_data_field.L4_Ops.nof_configurable_ranges_get = dnx_data_field_L4_Ops_nof_configurable_ranges_get;

    
    
    
    dnx_data_field.encoded_qual_actions_offset.feature_get = dnx_data_field_encoded_qual_actions_offset_feature_get;

    
    dnx_data_field.encoded_qual_actions_offset.trap_strength_offset_get = dnx_data_field_encoded_qual_actions_offset_trap_strength_offset_get;
    dnx_data_field.encoded_qual_actions_offset.trap_qualifier_offset_get = dnx_data_field_encoded_qual_actions_offset_trap_qualifier_offset_get;
    dnx_data_field.encoded_qual_actions_offset.sniff_qualifier_offset_get = dnx_data_field_encoded_qual_actions_offset_sniff_qualifier_offset_get;
    dnx_data_field.encoded_qual_actions_offset.mirror_qualifier_offset_get = dnx_data_field_encoded_qual_actions_offset_mirror_qualifier_offset_get;
    dnx_data_field.encoded_qual_actions_offset.forward_trap_qualifier_offset_get = dnx_data_field_encoded_qual_actions_offset_forward_trap_qualifier_offset_get;

    
    
    
    dnx_data_field.compare.feature_get = dnx_data_field_compare_feature_get;

    
    dnx_data_field.compare.equal_offset_get = dnx_data_field_compare_equal_offset_get;
    dnx_data_field.compare.not_equal_offset_get = dnx_data_field_compare_not_equal_offset_get;
    dnx_data_field.compare.smaller_offset_get = dnx_data_field_compare_smaller_offset_get;
    dnx_data_field.compare.not_smaller_offset_get = dnx_data_field_compare_not_smaller_offset_get;
    dnx_data_field.compare.bigger_offset_get = dnx_data_field_compare_bigger_offset_get;
    dnx_data_field.compare.not_bigger_offset_get = dnx_data_field_compare_not_bigger_offset_get;
    dnx_data_field.compare.nof_keys_get = dnx_data_field_compare_nof_keys_get;
    dnx_data_field.compare.nof_compare_pairs_get = dnx_data_field_compare_nof_compare_pairs_get;

    
    
    
    dnx_data_field.diag.feature_get = dnx_data_field_diag_feature_get;

    
    dnx_data_field.diag.bytes_to_remove_mask_get = dnx_data_field_diag_bytes_to_remove_mask_get;
    dnx_data_field.diag.layers_to_remove_size_in_bit_get = dnx_data_field_diag_layers_to_remove_size_in_bit_get;
    dnx_data_field.diag.layers_to_remove_mask_get = dnx_data_field_diag_layers_to_remove_mask_get;
    dnx_data_field.diag.nof_signals_per_action_get = dnx_data_field_diag_nof_signals_per_action_get;
    dnx_data_field.diag.nof_signals_per_qualifier_get = dnx_data_field_diag_nof_signals_per_qualifier_get;
    dnx_data_field.diag.key_signal_size_in_bytes_get = dnx_data_field_diag_key_signal_size_in_bytes_get;
    dnx_data_field.diag.result_signal_size_in_words_get = dnx_data_field_diag_result_signal_size_in_words_get;
    dnx_data_field.diag.dt_result_signal_size_in_words_get = dnx_data_field_diag_dt_result_signal_size_in_words_get;
    dnx_data_field.diag.hit_signal_size_in_words_get = dnx_data_field_diag_hit_signal_size_in_words_get;

    
    
    
    dnx_data_field.general_data_qualifiers.feature_get = dnx_data_field_general_data_qualifiers_feature_get;

    
    dnx_data_field.general_data_qualifiers.vw_supported_get = dnx_data_field_general_data_qualifiers_vw_supported_get;
    dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_nof_bits_get = dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_nof_bits_get;
    dnx_data_field.general_data_qualifiers.pwe_in_lif_wide_data_nof_bits_get = dnx_data_field_general_data_qualifiers_pwe_in_lif_wide_data_nof_bits_get;
    dnx_data_field.general_data_qualifiers.ip_tunnel_in_lif_wide_data_nof_bits_get = dnx_data_field_general_data_qualifiers_ip_tunnel_in_lif_wide_data_nof_bits_get;
    dnx_data_field.general_data_qualifiers.native_ac_in_lif_wide_data_nof_bits_get = dnx_data_field_general_data_qualifiers_native_ac_in_lif_wide_data_nof_bits_get;
    dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get = dnx_data_field_general_data_qualifiers_ac_in_lif_wide_data_extended_nof_bits_get;
    dnx_data_field.general_data_qualifiers.mact_entry_grouping_nof_bits_get = dnx_data_field_general_data_qualifiers_mact_entry_grouping_nof_bits_get;
    dnx_data_field.general_data_qualifiers.vw_vip_valid_nof_bits_get = dnx_data_field_general_data_qualifiers_vw_vip_valid_nof_bits_get;
    dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get = dnx_data_field_general_data_qualifiers_vw_vip_id_nof_bits_get;
    dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get = dnx_data_field_general_data_qualifiers_vw_member_reference_nof_bits_get;
    dnx_data_field.general_data_qualifiers.vw_pcc_hit_nof_bits_get = dnx_data_field_general_data_qualifiers_vw_pcc_hit_nof_bits_get;
    dnx_data_field.general_data_qualifiers.l3srcbind_hit_nof_bits_get = dnx_data_field_general_data_qualifiers_l3srcbind_hit_nof_bits_get;
    dnx_data_field.general_data_qualifiers.srv6_next_si_get = dnx_data_field_general_data_qualifiers_srv6_next_si_get;
    dnx_data_field.general_data_qualifiers.srv6_next_gsid_get = dnx_data_field_general_data_qualifiers_srv6_next_gsid_get;

    
    
    
    dnx_data_field.common_max_val.feature_get = dnx_data_field_common_max_val_feature_get;

    
    dnx_data_field.common_max_val.nof_ffc_get = dnx_data_field_common_max_val_nof_ffc_get;
    dnx_data_field.common_max_val.nof_masks_per_fes_get = dnx_data_field_common_max_val_nof_masks_per_fes_get;
    dnx_data_field.common_max_val.nof_fes_id_per_array_get = dnx_data_field_common_max_val_nof_fes_id_per_array_get;
    dnx_data_field.common_max_val.nof_fes_array_get = dnx_data_field_common_max_val_nof_fes_array_get;
    dnx_data_field.common_max_val.nof_fes_instruction_per_context_get = dnx_data_field_common_max_val_nof_fes_instruction_per_context_get;
    dnx_data_field.common_max_val.nof_fes_programs_get = dnx_data_field_common_max_val_nof_fes_programs_get;
    dnx_data_field.common_max_val.nof_prog_per_fes_get = dnx_data_field_common_max_val_nof_prog_per_fes_get;
    dnx_data_field.common_max_val.nof_cs_lines_get = dnx_data_field_common_max_val_nof_cs_lines_get;
    dnx_data_field.common_max_val.nof_contexts_get = dnx_data_field_common_max_val_nof_contexts_get;
    dnx_data_field.common_max_val.nof_actions_get = dnx_data_field_common_max_val_nof_actions_get;
    dnx_data_field.common_max_val.nof_qualifiers_get = dnx_data_field_common_max_val_nof_qualifiers_get;
    dnx_data_field.common_max_val.nof_bits_in_fes_action_get = dnx_data_field_common_max_val_nof_bits_in_fes_action_get;
    dnx_data_field.common_max_val.nof_bits_in_fes_key_select_get = dnx_data_field_common_max_val_nof_bits_in_fes_key_select_get;
    dnx_data_field.common_max_val.nof_bits_in_ffc_get = dnx_data_field_common_max_val_nof_bits_in_ffc_get;
    dnx_data_field.common_max_val.nof_ffc_in_uint32_get = dnx_data_field_common_max_val_nof_ffc_in_uint32_get;
    dnx_data_field.common_max_val.nof_action_per_group_get = dnx_data_field_common_max_val_nof_action_per_group_get;
    dnx_data_field.common_max_val.nof_layer_records_get = dnx_data_field_common_max_val_nof_layer_records_get;
    dnx_data_field.common_max_val.layer_record_size_get = dnx_data_field_common_max_val_layer_record_size_get;
    dnx_data_field.common_max_val.nof_l4_ops_ranges_legacy_get = dnx_data_field_common_max_val_nof_l4_ops_ranges_legacy_get;
    dnx_data_field.common_max_val.nof_pkt_hdr_ranges_get = dnx_data_field_common_max_val_nof_pkt_hdr_ranges_get;
    dnx_data_field.common_max_val.nof_out_lif_ranges_get = dnx_data_field_common_max_val_nof_out_lif_ranges_get;
    dnx_data_field.common_max_val.kbr_size_get = dnx_data_field_common_max_val_kbr_size_get;
    dnx_data_field.common_max_val.nof_compare_pairs_in_compare_mode_get = dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_get;
    dnx_data_field.common_max_val.nof_bits_in_fem_action_get = dnx_data_field_common_max_val_nof_bits_in_fem_action_get;
    dnx_data_field.common_max_val.nof_fem_condition_get = dnx_data_field_common_max_val_nof_fem_condition_get;
    dnx_data_field.common_max_val.nof_fem_map_index_get = dnx_data_field_common_max_val_nof_fem_map_index_get;
    dnx_data_field.common_max_val.nof_fem_programs_get = dnx_data_field_common_max_val_nof_fem_programs_get;
    dnx_data_field.common_max_val.nof_fem_id_get = dnx_data_field_common_max_val_nof_fem_id_get;
    dnx_data_field.common_max_val.nof_array_ids_get = dnx_data_field_common_max_val_nof_array_ids_get;
    dnx_data_field.common_max_val.dbal_pairs_get = dnx_data_field_common_max_val_dbal_pairs_get;

    
    dnx_data_field.common_max_val.array_id_type_get = dnx_data_field_common_max_val_array_id_type_get;
    dnx_data_field.common_max_val.array_id_type_info_get = dnx_data_field_common_max_val_array_id_type_info_get;
    
    
    dnx_data_field.init.feature_get = dnx_data_field_init_feature_get;

    
    dnx_data_field.init.fec_dest_get = dnx_data_field_init_fec_dest_get;
    dnx_data_field.init.l4_trap_get = dnx_data_field_init_l4_trap_get;
    dnx_data_field.init.oam_layer_index_get = dnx_data_field_init_oam_layer_index_get;
    dnx_data_field.init.oam_stat_get = dnx_data_field_init_oam_stat_get;
    dnx_data_field.init.roo_get = dnx_data_field_init_roo_get;
    dnx_data_field.init.disable_erpp_counters_get = dnx_data_field_init_disable_erpp_counters_get;
    dnx_data_field.init.jr1_ipmc_inlif_get = dnx_data_field_init_jr1_ipmc_inlif_get;
    dnx_data_field.init.j1_same_port_get = dnx_data_field_init_j1_same_port_get;
    dnx_data_field.init.j1_learning_get = dnx_data_field_init_j1_learning_get;
    dnx_data_field.init.learn_limit_get = dnx_data_field_init_learn_limit_get;
    dnx_data_field.init.j1_php_get = dnx_data_field_init_j1_php_get;
    dnx_data_field.init.ftmh_mc_get = dnx_data_field_init_ftmh_mc_get;
    dnx_data_field.init.flow_id_get = dnx_data_field_init_flow_id_get;

    
    
    
    dnx_data_field.features.feature_get = dnx_data_field_features_feature_get;

    
    dnx_data_field.features.tcam_cs_is_tcam_direct_access_get = dnx_data_field_features_tcam_cs_is_tcam_direct_access_get;
    dnx_data_field.features.ecc_enable_get = dnx_data_field_features_ecc_enable_get;
    dnx_data_field.features.switch_to_acl_context_get = dnx_data_field_features_switch_to_acl_context_get;
    dnx_data_field.features.per_pp_port_pmf_profile_cs_offset_get = dnx_data_field_features_per_pp_port_pmf_profile_cs_offset_get;
    dnx_data_field.features.tcam_result_flip_eco_get = dnx_data_field_features_tcam_result_flip_eco_get;
    dnx_data_field.features.tcam_result_half_payload_on_msb_get = dnx_data_field_features_tcam_result_half_payload_on_msb_get;
    dnx_data_field.features.parsing_start_offset_msb_meaningless_get = dnx_data_field_features_parsing_start_offset_msb_meaningless_get;
    dnx_data_field.features.kbp_opcode_in_ipmf1_cs_get = dnx_data_field_features_kbp_opcode_in_ipmf1_cs_get;
    dnx_data_field.features.kbp_hitbits_correct_in_ipmf1_cs_get = dnx_data_field_features_kbp_hitbits_correct_in_ipmf1_cs_get;
    dnx_data_field.features.exem_vmv_removable_from_payload_get = dnx_data_field_features_exem_vmv_removable_from_payload_get;
    dnx_data_field.features.multiple_dynamic_mem_enablers_get = dnx_data_field_features_multiple_dynamic_mem_enablers_get;
    dnx_data_field.features.aacl_super_group_handler_enable_get = dnx_data_field_features_aacl_super_group_handler_enable_get;
    dnx_data_field.features.aacl_tcam_swap_enable_get = dnx_data_field_features_aacl_tcam_swap_enable_get;
    dnx_data_field.features.extended_l4_ops_get = dnx_data_field_features_extended_l4_ops_get;
    dnx_data_field.features.state_table_ipmf1_key_select_get = dnx_data_field_features_state_table_ipmf1_key_select_get;
    dnx_data_field.features.state_table_acr_bus_get = dnx_data_field_features_state_table_acr_bus_get;
    dnx_data_field.features.state_table_atomic_rmw_get = dnx_data_field_features_state_table_atomic_rmw_get;
    dnx_data_field.features.hitbit_volatile_get = dnx_data_field_features_hitbit_volatile_get;
    dnx_data_field.features.fem_replace_msb_instead_lsb_get = dnx_data_field_features_fem_replace_msb_instead_lsb_get;
    dnx_data_field.features.dir_ext_epmf_get = dnx_data_field_features_dir_ext_epmf_get;
    dnx_data_field.features.dir_ext_epmf_key_has_debug_signal_get = dnx_data_field_features_dir_ext_epmf_key_has_debug_signal_get;
    dnx_data_field.features.hashing_process_lsb_to_msb_get = dnx_data_field_features_hashing_process_lsb_to_msb_get;
    dnx_data_field.features.exem_age_flush_scan_get = dnx_data_field_features_exem_age_flush_scan_get;
    dnx_data_field.features.epmf_has_lookup_enabler_get = dnx_data_field_features_epmf_has_lookup_enabler_get;
    dnx_data_field.features.epmf_outlif_profile_map_get = dnx_data_field_features_epmf_outlif_profile_map_get;
    dnx_data_field.features.tcam_full_key_half_payload_both_lsb_msb_get = dnx_data_field_features_tcam_full_key_half_payload_both_lsb_msb_get;
    dnx_data_field.features.epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get = dnx_data_field_features_epmf_exem_debug_signal_move_zero_padding_from_msb_to_lsb_get;
    dnx_data_field.features.ecmp_tunnel_priority_get = dnx_data_field_features_ecmp_tunnel_priority_get;
    dnx_data_field.features.key_result_hit_signals_exist_get = dnx_data_field_features_key_result_hit_signals_exist_get;
    dnx_data_field.features.external_cs_based_on_fwd2_get = dnx_data_field_features_external_cs_based_on_fwd2_get;
    dnx_data_field.features.udh_base_cs_is_mapped_get = dnx_data_field_features_udh_base_cs_is_mapped_get;
    dnx_data_field.features.state_collection_engine_get = dnx_data_field_features_state_collection_engine_get;
    dnx_data_field.features.cs_lif_profile_get = dnx_data_field_features_cs_lif_profile_get;
    dnx_data_field.features.empf_trap_action_is_strength_based_get = dnx_data_field_features_empf_trap_action_is_strength_based_get;
    dnx_data_field.features.has_last_keys_get = dnx_data_field_features_has_last_keys_get;
    dnx_data_field.features.ipmf3_has_last_keys_get = dnx_data_field_features_ipmf3_has_last_keys_get;
    dnx_data_field.features.ipmf3_context_signal_is_from_register_get = dnx_data_field_features_ipmf3_context_signal_is_from_register_get;
    dnx_data_field.features.epmf_context_signal_exists_get = dnx_data_field_features_epmf_context_signal_exists_get;
    dnx_data_field.features.epmf_context_signal_from_phl_block_get = dnx_data_field_features_epmf_context_signal_from_phl_block_get;
    dnx_data_field.features.epmf_is_on_etpp_get = dnx_data_field_features_epmf_is_on_etpp_get;
    dnx_data_field.features.de_has_debug_signals_get = dnx_data_field_features_de_has_debug_signals_get;
    dnx_data_field.features.result_hit_signal_names_always_include_key_get = dnx_data_field_features_result_hit_signal_names_always_include_key_get;
    dnx_data_field.features.ipmf3_context_has_oam_stateless_property_get = dnx_data_field_features_ipmf3_context_has_oam_stateless_property_get;

    
    
    
    dnx_data_field.signal_sizes.feature_get = dnx_data_field_signal_sizes_feature_get;

    
    dnx_data_field.signal_sizes.dual_queue_size_get = dnx_data_field_signal_sizes_dual_queue_size_get;
    dnx_data_field.signal_sizes.packet_header_size_get = dnx_data_field_signal_sizes_packet_header_size_get;
    dnx_data_field.signal_sizes.ecn_get = dnx_data_field_signal_sizes_ecn_get;
    dnx_data_field.signal_sizes.congestion_info_get = dnx_data_field_signal_sizes_congestion_info_get;
    dnx_data_field.signal_sizes.parsing_start_offset_size_get = dnx_data_field_signal_sizes_parsing_start_offset_size_get;

    
    
    
    dnx_data_field.dnx_data_internal.feature_get = dnx_data_field_dnx_data_internal_feature_get;

    
    dnx_data_field.dnx_data_internal.lr_eth_is_da_mac_valid_get = dnx_data_field_dnx_data_internal_lr_eth_is_da_mac_valid_get;

    
    
    
    dnx_data_field.tests.feature_get = dnx_data_field_tests_feature_get;

    
    dnx_data_field.tests.learn_info_actions_structure_change_get = dnx_data_field_tests_learn_info_actions_structure_change_get;
    dnx_data_field.tests.ingress_time_stamp_increased_get = dnx_data_field_tests_ingress_time_stamp_increased_get;
    dnx_data_field.tests.ace_debug_signals_exist_get = dnx_data_field_tests_ace_debug_signals_exist_get;
    dnx_data_field.tests.epmf_debug_rely_on_tcam0_get = dnx_data_field_tests_epmf_debug_rely_on_tcam0_get;
    dnx_data_field.tests.epmf_tm_port_cs_get = dnx_data_field_tests_epmf_tm_port_cs_get;
    dnx_data_field.tests.small_exem_addr_bits_get = dnx_data_field_tests_small_exem_addr_bits_get;
    dnx_data_field.tests.stub_adapter_supports_semantic_flush_get = dnx_data_field_tests_stub_adapter_supports_semantic_flush_get;
    dnx_data_field.tests.parsing_start_offset_exists_get = dnx_data_field_tests_parsing_start_offset_exists_get;
    dnx_data_field.tests.first_part_of_acr_zero_padded_by_32_get = dnx_data_field_tests_first_part_of_acr_zero_padded_by_32_get;
    dnx_data_field.tests.ext_stat_valid_can_be_read_in_tm_cmd_get = dnx_data_field_tests_ext_stat_valid_can_be_read_in_tm_cmd_get;

    
    
    
    
    dnx_data_field_map.action.feature_get = dnx_data_field_map_action_feature_get;

    

    
    dnx_data_field_map.action.params_get = dnx_data_field_map_action_params_get;
    dnx_data_field_map.action.params_info_get = dnx_data_field_map_action_params_info_get;
    
    
    dnx_data_field_map.qual.feature_get = dnx_data_field_map_qual_feature_get;

    

    
    dnx_data_field_map.qual.params_get = dnx_data_field_map_qual_params_get;
    dnx_data_field_map.qual.params_info_get = dnx_data_field_map_qual_params_info_get;
    dnx_data_field_map.qual.cs_get = dnx_data_field_map_qual_cs_get;
    dnx_data_field_map.qual.cs_info_get = dnx_data_field_map_qual_cs_info_get;
    dnx_data_field_map.qual.special_metadata_qual_get = dnx_data_field_map_qual_special_metadata_qual_get;
    dnx_data_field_map.qual.special_metadata_qual_info_get = dnx_data_field_map_qual_special_metadata_qual_info_get;
    dnx_data_field_map.qual.cs_special_general_data_qual_get = dnx_data_field_map_qual_cs_special_general_data_qual_get;
    dnx_data_field_map.qual.cs_special_general_data_qual_info_get = dnx_data_field_map_qual_cs_special_general_data_qual_info_get;
    dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_get = dnx_data_field_map_qual_cs_pem_scratch_pad_mapping_get;
    dnx_data_field_map.qual.cs_pem_scratch_pad_mapping_info_get = dnx_data_field_map_qual_cs_pem_scratch_pad_mapping_info_get;
    dnx_data_field_map.qual.layer_record_info_ingress_get = dnx_data_field_map_qual_layer_record_info_ingress_get;
    dnx_data_field_map.qual.layer_record_info_ingress_info_get = dnx_data_field_map_qual_layer_record_info_ingress_info_get;
    dnx_data_field_map.qual.layer_record_info_egress_get = dnx_data_field_map_qual_layer_record_info_egress_get;
    dnx_data_field_map.qual.layer_record_info_egress_info_get = dnx_data_field_map_qual_layer_record_info_egress_info_get;
    dnx_data_field_map.qual.header_get = dnx_data_field_map_qual_header_get;
    dnx_data_field_map.qual.header_info_get = dnx_data_field_map_qual_header_info_get;
    
    
    dnx_data_field_map.tcam.feature_get = dnx_data_field_map_tcam_feature_get;

    

    
    dnx_data_field_map.tcam.tcam_pp_app_get = dnx_data_field_map_tcam_tcam_pp_app_get;
    dnx_data_field_map.tcam.tcam_pp_app_info_get = dnx_data_field_map_tcam_tcam_pp_app_info_get;
    
    
    dnx_data_field_map.port_profile.feature_get = dnx_data_field_map_port_profile_feature_get;

    

    
    dnx_data_field_map.port_profile.port_class_profile_get = dnx_data_field_map_port_profile_port_class_profile_get;
    dnx_data_field_map.port_profile.port_class_profile_info_get = dnx_data_field_map_port_profile_port_class_profile_info_get;
    
    
    dnx_data_field_map.bcm_to_dnx.feature_get = dnx_data_field_map_bcm_to_dnx_feature_get;

    

    
    dnx_data_field_map.bcm_to_dnx.forward_context_get = dnx_data_field_map_bcm_to_dnx_forward_context_get;
    dnx_data_field_map.bcm_to_dnx.forward_context_info_get = dnx_data_field_map_bcm_to_dnx_forward_context_info_get;
    
    
    
    dnx_data_fifodma.general.feature_get = dnx_data_fifodma_general_feature_get;

    
    dnx_data_fifodma.general.nof_fifodma_channels_get = dnx_data_fifodma_general_nof_fifodma_channels_get;
    dnx_data_fifodma.general.min_nof_host_entries_get = dnx_data_fifodma_general_min_nof_host_entries_get;
    dnx_data_fifodma.general.max_nof_host_entries_get = dnx_data_fifodma_general_max_nof_host_entries_get;

    
    dnx_data_fifodma.general.fifodma_map_get = dnx_data_fifodma_general_fifodma_map_get;
    dnx_data_fifodma.general.fifodma_map_info_get = dnx_data_fifodma_general_fifodma_map_info_get;
    
    
    
    dnx_data_flow.general.feature_get = dnx_data_flow_general_feature_get;

    
    dnx_data_flow.general.match_per_gport_sw_nof_entries_get = dnx_data_flow_general_match_per_gport_sw_nof_entries_get;
    dnx_data_flow.general.match_entry_info_max_key_size_in_words_get = dnx_data_flow_general_match_entry_info_max_key_size_in_words_get;

    
    
    
    dnx_data_flow.srv6.feature_get = dnx_data_flow_srv6_feature_get;

    
    dnx_data_flow.srv6.use_flow_lif_init_get = dnx_data_flow_srv6_use_flow_lif_init_get;
    dnx_data_flow.srv6.use_flow_lif_term_get = dnx_data_flow_srv6_use_flow_lif_term_get;

    
    
    
    dnx_data_flow.twamp.feature_get = dnx_data_flow_twamp_feature_get;

    
    dnx_data_flow.twamp.twamp_reflector_supported_get = dnx_data_flow_twamp_twamp_reflector_supported_get;

    
    
    
    
    dnx_data_graphical.diag.feature_get = dnx_data_graphical_diag_feature_get;

    
    dnx_data_graphical.diag.num_of_rows_get = dnx_data_graphical_diag_num_of_rows_get;
    dnx_data_graphical.diag.num_of_index_get = dnx_data_graphical_diag_num_of_index_get;
    dnx_data_graphical.diag.num_of_block_entries_get = dnx_data_graphical_diag_num_of_block_entries_get;

    
    dnx_data_graphical.diag.blocks_left_get = dnx_data_graphical_diag_blocks_left_get;
    dnx_data_graphical.diag.blocks_left_info_get = dnx_data_graphical_diag_blocks_left_info_get;
    dnx_data_graphical.diag.blocks_right_get = dnx_data_graphical_diag_blocks_right_get;
    dnx_data_graphical.diag.blocks_right_info_get = dnx_data_graphical_diag_blocks_right_info_get;
    dnx_data_graphical.diag.counters_get = dnx_data_graphical_diag_counters_get;
    dnx_data_graphical.diag.counters_info_get = dnx_data_graphical_diag_counters_info_get;
    dnx_data_graphical.diag.drop_reason_get = dnx_data_graphical_diag_drop_reason_get;
    dnx_data_graphical.diag.drop_reason_info_get = dnx_data_graphical_diag_drop_reason_info_get;
    
    
    
    dnx_data_gtimer.rtp.feature_get = dnx_data_gtimer_rtp_feature_get;

    

    
    
    
    dnx_data_gtimer.fdt.feature_get = dnx_data_gtimer_fdt_feature_get;

    
    dnx_data_gtimer.fdt.block_index_size_get = dnx_data_gtimer_fdt_block_index_size_get;
    dnx_data_gtimer.fdt.block_index_max_value_get = dnx_data_gtimer_fdt_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.fdtl.feature_get = dnx_data_gtimer_fdtl_feature_get;

    
    dnx_data_gtimer.fdtl.block_index_size_get = dnx_data_gtimer_fdtl_block_index_size_get;
    dnx_data_gtimer.fdtl.block_index_max_value_get = dnx_data_gtimer_fdtl_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.pem.feature_get = dnx_data_gtimer_pem_feature_get;

    
    dnx_data_gtimer.pem.block_index_size_get = dnx_data_gtimer_pem_block_index_size_get;
    dnx_data_gtimer.pem.block_index_max_value_get = dnx_data_gtimer_pem_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.dhc.feature_get = dnx_data_gtimer_dhc_feature_get;

    
    dnx_data_gtimer.dhc.block_index_size_get = dnx_data_gtimer_dhc_block_index_size_get;
    dnx_data_gtimer.dhc.block_index_max_value_get = dnx_data_gtimer_dhc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cdu.feature_get = dnx_data_gtimer_cdu_feature_get;

    
    dnx_data_gtimer.cdu.block_index_size_get = dnx_data_gtimer_cdu_block_index_size_get;
    dnx_data_gtimer.cdu.block_index_max_value_get = dnx_data_gtimer_cdu_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.esb.feature_get = dnx_data_gtimer_esb_feature_get;

    
    dnx_data_gtimer.esb.block_index_size_get = dnx_data_gtimer_esb_block_index_size_get;
    dnx_data_gtimer.esb.block_index_max_value_get = dnx_data_gtimer_esb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cdum.feature_get = dnx_data_gtimer_cdum_feature_get;

    

    
    
    
    dnx_data_gtimer.mesh_topology.feature_get = dnx_data_gtimer_mesh_topology_feature_get;

    

    
    
    
    dnx_data_gtimer.bdm.feature_get = dnx_data_gtimer_bdm_feature_get;

    
    dnx_data_gtimer.bdm.block_index_size_get = dnx_data_gtimer_bdm_block_index_size_get;
    dnx_data_gtimer.bdm.block_index_min_value_get = dnx_data_gtimer_bdm_block_index_min_value_get;
    dnx_data_gtimer.bdm.block_index_max_value_get = dnx_data_gtimer_bdm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.hbc.feature_get = dnx_data_gtimer_hbc_feature_get;

    
    dnx_data_gtimer.hbc.block_index_size_get = dnx_data_gtimer_hbc_block_index_size_get;
    dnx_data_gtimer.hbc.block_index_max_value_get = dnx_data_gtimer_hbc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ecgm.feature_get = dnx_data_gtimer_ecgm_feature_get;

    
    dnx_data_gtimer.ecgm.block_index_size_get = dnx_data_gtimer_ecgm_block_index_size_get;
    dnx_data_gtimer.ecgm.block_index_max_value_get = dnx_data_gtimer_ecgm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cfc.feature_get = dnx_data_gtimer_cfc_feature_get;

    
    dnx_data_gtimer.cfc.block_index_size_get = dnx_data_gtimer_cfc_block_index_size_get;
    dnx_data_gtimer.cfc.block_index_max_value_get = dnx_data_gtimer_cfc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cgm.feature_get = dnx_data_gtimer_cgm_feature_get;

    
    dnx_data_gtimer.cgm.block_index_size_get = dnx_data_gtimer_cgm_block_index_size_get;
    dnx_data_gtimer.cgm.block_index_max_value_get = dnx_data_gtimer_cgm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.crps.feature_get = dnx_data_gtimer_crps_feature_get;

    
    dnx_data_gtimer.crps.block_index_size_get = dnx_data_gtimer_crps_block_index_size_get;
    dnx_data_gtimer.crps.block_index_max_value_get = dnx_data_gtimer_crps_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ddha.feature_get = dnx_data_gtimer_ddha_feature_get;

    
    dnx_data_gtimer.ddha.block_index_size_get = dnx_data_gtimer_ddha_block_index_size_get;
    dnx_data_gtimer.ddha.block_index_max_value_get = dnx_data_gtimer_ddha_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ddp.feature_get = dnx_data_gtimer_ddp_feature_get;

    
    dnx_data_gtimer.ddp.block_index_size_get = dnx_data_gtimer_ddp_block_index_size_get;
    dnx_data_gtimer.ddp.block_index_max_value_get = dnx_data_gtimer_ddp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.dqm.feature_get = dnx_data_gtimer_dqm_feature_get;

    
    dnx_data_gtimer.dqm.block_index_size_get = dnx_data_gtimer_dqm_block_index_size_get;
    dnx_data_gtimer.dqm.block_index_max_value_get = dnx_data_gtimer_dqm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.edb.feature_get = dnx_data_gtimer_edb_feature_get;

    
    dnx_data_gtimer.edb.block_index_size_get = dnx_data_gtimer_edb_block_index_size_get;
    dnx_data_gtimer.edb.block_index_max_value_get = dnx_data_gtimer_edb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.epni.feature_get = dnx_data_gtimer_epni_feature_get;

    
    dnx_data_gtimer.epni.block_index_size_get = dnx_data_gtimer_epni_block_index_size_get;
    dnx_data_gtimer.epni.block_index_max_value_get = dnx_data_gtimer_epni_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.epre.feature_get = dnx_data_gtimer_epre_feature_get;

    
    dnx_data_gtimer.epre.block_index_size_get = dnx_data_gtimer_epre_block_index_size_get;
    dnx_data_gtimer.epre.block_index_max_value_get = dnx_data_gtimer_epre_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.eps.feature_get = dnx_data_gtimer_eps_feature_get;

    
    dnx_data_gtimer.eps.block_index_size_get = dnx_data_gtimer_eps_block_index_size_get;
    dnx_data_gtimer.eps.block_index_max_value_get = dnx_data_gtimer_eps_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.erpp.feature_get = dnx_data_gtimer_erpp_feature_get;

    
    dnx_data_gtimer.erpp.block_index_size_get = dnx_data_gtimer_erpp_block_index_size_get;
    dnx_data_gtimer.erpp.block_index_max_value_get = dnx_data_gtimer_erpp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.etppa.feature_get = dnx_data_gtimer_etppa_feature_get;

    
    dnx_data_gtimer.etppa.block_index_size_get = dnx_data_gtimer_etppa_block_index_size_get;
    dnx_data_gtimer.etppa.block_index_max_value_get = dnx_data_gtimer_etppa_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.etppb.feature_get = dnx_data_gtimer_etppb_feature_get;

    
    dnx_data_gtimer.etppb.block_index_size_get = dnx_data_gtimer_etppb_block_index_size_get;
    dnx_data_gtimer.etppb.block_index_max_value_get = dnx_data_gtimer_etppb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.etppc.feature_get = dnx_data_gtimer_etppc_feature_get;

    
    dnx_data_gtimer.etppc.block_index_size_get = dnx_data_gtimer_etppc_block_index_size_get;
    dnx_data_gtimer.etppc.block_index_max_value_get = dnx_data_gtimer_etppc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.fqp.feature_get = dnx_data_gtimer_fqp_feature_get;

    
    dnx_data_gtimer.fqp.block_index_size_get = dnx_data_gtimer_fqp_block_index_size_get;
    dnx_data_gtimer.fqp.block_index_max_value_get = dnx_data_gtimer_fqp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ile.feature_get = dnx_data_gtimer_ile_feature_get;

    
    dnx_data_gtimer.ile.block_index_size_get = dnx_data_gtimer_ile_block_index_size_get;
    dnx_data_gtimer.ile.block_index_max_value_get = dnx_data_gtimer_ile_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ippa.feature_get = dnx_data_gtimer_ippa_feature_get;

    
    dnx_data_gtimer.ippa.block_index_size_get = dnx_data_gtimer_ippa_block_index_size_get;
    dnx_data_gtimer.ippa.block_index_max_value_get = dnx_data_gtimer_ippa_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ippb.feature_get = dnx_data_gtimer_ippb_feature_get;

    
    dnx_data_gtimer.ippb.block_index_size_get = dnx_data_gtimer_ippb_block_index_size_get;
    dnx_data_gtimer.ippb.block_index_max_value_get = dnx_data_gtimer_ippb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ippc.feature_get = dnx_data_gtimer_ippc_feature_get;

    
    dnx_data_gtimer.ippc.block_index_size_get = dnx_data_gtimer_ippc_block_index_size_get;
    dnx_data_gtimer.ippc.block_index_max_value_get = dnx_data_gtimer_ippc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ippd.feature_get = dnx_data_gtimer_ippd_feature_get;

    
    dnx_data_gtimer.ippd.block_index_size_get = dnx_data_gtimer_ippd_block_index_size_get;
    dnx_data_gtimer.ippd.block_index_max_value_get = dnx_data_gtimer_ippd_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ippe.feature_get = dnx_data_gtimer_ippe_feature_get;

    
    dnx_data_gtimer.ippe.block_index_size_get = dnx_data_gtimer_ippe_block_index_size_get;
    dnx_data_gtimer.ippe.block_index_max_value_get = dnx_data_gtimer_ippe_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ippf.feature_get = dnx_data_gtimer_ippf_feature_get;

    
    dnx_data_gtimer.ippf.block_index_size_get = dnx_data_gtimer_ippf_block_index_size_get;
    dnx_data_gtimer.ippf.block_index_max_value_get = dnx_data_gtimer_ippf_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ips.feature_get = dnx_data_gtimer_ips_feature_get;

    
    dnx_data_gtimer.ips.block_index_size_get = dnx_data_gtimer_ips_block_index_size_get;
    dnx_data_gtimer.ips.block_index_max_value_get = dnx_data_gtimer_ips_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ipt.feature_get = dnx_data_gtimer_ipt_feature_get;

    
    dnx_data_gtimer.ipt.block_index_size_get = dnx_data_gtimer_ipt_block_index_size_get;
    dnx_data_gtimer.ipt.block_index_max_value_get = dnx_data_gtimer_ipt_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.iqm.feature_get = dnx_data_gtimer_iqm_feature_get;

    
    dnx_data_gtimer.iqm.block_index_size_get = dnx_data_gtimer_iqm_block_index_size_get;
    dnx_data_gtimer.iqm.block_index_max_value_get = dnx_data_gtimer_iqm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ire.feature_get = dnx_data_gtimer_ire_feature_get;

    
    dnx_data_gtimer.ire.block_index_size_get = dnx_data_gtimer_ire_block_index_size_get;
    dnx_data_gtimer.ire.block_index_max_value_get = dnx_data_gtimer_ire_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.itpp.feature_get = dnx_data_gtimer_itpp_feature_get;

    
    dnx_data_gtimer.itpp.block_index_size_get = dnx_data_gtimer_itpp_block_index_size_get;
    dnx_data_gtimer.itpp.block_index_max_value_get = dnx_data_gtimer_itpp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.itppd.feature_get = dnx_data_gtimer_itppd_feature_get;

    
    dnx_data_gtimer.itppd.block_index_size_get = dnx_data_gtimer_itppd_block_index_size_get;
    dnx_data_gtimer.itppd.block_index_max_value_get = dnx_data_gtimer_itppd_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.kaps.feature_get = dnx_data_gtimer_kaps_feature_get;

    
    dnx_data_gtimer.kaps.block_index_size_get = dnx_data_gtimer_kaps_block_index_size_get;
    dnx_data_gtimer.kaps.block_index_max_value_get = dnx_data_gtimer_kaps_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.mcp.feature_get = dnx_data_gtimer_mcp_feature_get;

    
    dnx_data_gtimer.mcp.block_index_size_get = dnx_data_gtimer_mcp_block_index_size_get;
    dnx_data_gtimer.mcp.block_index_max_value_get = dnx_data_gtimer_mcp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.mrps.feature_get = dnx_data_gtimer_mrps_feature_get;

    
    dnx_data_gtimer.mrps.block_index_size_get = dnx_data_gtimer_mrps_block_index_size_get;
    dnx_data_gtimer.mrps.block_index_max_value_get = dnx_data_gtimer_mrps_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.nmg.feature_get = dnx_data_gtimer_nmg_feature_get;

    
    dnx_data_gtimer.nmg.block_index_size_get = dnx_data_gtimer_nmg_block_index_size_get;
    dnx_data_gtimer.nmg.block_index_max_value_get = dnx_data_gtimer_nmg_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ocb.feature_get = dnx_data_gtimer_ocb_feature_get;

    
    dnx_data_gtimer.ocb.block_index_size_get = dnx_data_gtimer_ocb_block_index_size_get;
    dnx_data_gtimer.ocb.block_index_max_value_get = dnx_data_gtimer_ocb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.pdm.feature_get = dnx_data_gtimer_pdm_feature_get;

    
    dnx_data_gtimer.pdm.block_index_size_get = dnx_data_gtimer_pdm_block_index_size_get;
    dnx_data_gtimer.pdm.block_index_max_value_get = dnx_data_gtimer_pdm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.pqp.feature_get = dnx_data_gtimer_pqp_feature_get;

    
    dnx_data_gtimer.pqp.block_index_size_get = dnx_data_gtimer_pqp_block_index_size_get;
    dnx_data_gtimer.pqp.block_index_max_value_get = dnx_data_gtimer_pqp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.rqp.feature_get = dnx_data_gtimer_rqp_feature_get;

    
    dnx_data_gtimer.rqp.block_index_size_get = dnx_data_gtimer_rqp_block_index_size_get;
    dnx_data_gtimer.rqp.block_index_max_value_get = dnx_data_gtimer_rqp_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.sch.feature_get = dnx_data_gtimer_sch_feature_get;

    
    dnx_data_gtimer.sch.block_index_size_get = dnx_data_gtimer_sch_block_index_size_get;
    dnx_data_gtimer.sch.block_index_max_value_get = dnx_data_gtimer_sch_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.sif.feature_get = dnx_data_gtimer_sif_feature_get;

    
    dnx_data_gtimer.sif.block_index_size_get = dnx_data_gtimer_sif_block_index_size_get;
    dnx_data_gtimer.sif.block_index_max_value_get = dnx_data_gtimer_sif_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.spb.feature_get = dnx_data_gtimer_spb_feature_get;

    
    dnx_data_gtimer.spb.block_index_size_get = dnx_data_gtimer_spb_block_index_size_get;
    dnx_data_gtimer.spb.block_index_max_value_get = dnx_data_gtimer_spb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.sqm.feature_get = dnx_data_gtimer_sqm_feature_get;

    
    dnx_data_gtimer.sqm.block_index_size_get = dnx_data_gtimer_sqm_block_index_size_get;
    dnx_data_gtimer.sqm.block_index_max_value_get = dnx_data_gtimer_sqm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.tcam.feature_get = dnx_data_gtimer_tcam_feature_get;

    
    dnx_data_gtimer.tcam.block_index_size_get = dnx_data_gtimer_tcam_block_index_size_get;
    dnx_data_gtimer.tcam.block_index_max_value_get = dnx_data_gtimer_tcam_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.tdu.feature_get = dnx_data_gtimer_tdu_feature_get;

    
    dnx_data_gtimer.tdu.block_index_size_get = dnx_data_gtimer_tdu_block_index_size_get;
    dnx_data_gtimer.tdu.block_index_max_value_get = dnx_data_gtimer_tdu_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.dcc.feature_get = dnx_data_gtimer_dcc_feature_get;

    
    dnx_data_gtimer.dcc.block_index_size_get = dnx_data_gtimer_dcc_block_index_size_get;
    dnx_data_gtimer.dcc.block_index_max_value_get = dnx_data_gtimer_dcc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cdb.feature_get = dnx_data_gtimer_cdb_feature_get;

    
    dnx_data_gtimer.cdb.block_index_size_get = dnx_data_gtimer_cdb_block_index_size_get;
    dnx_data_gtimer.cdb.block_index_max_value_get = dnx_data_gtimer_cdb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cdbm.feature_get = dnx_data_gtimer_cdbm_feature_get;

    
    dnx_data_gtimer.cdbm.block_index_size_get = dnx_data_gtimer_cdbm_block_index_size_get;
    dnx_data_gtimer.cdbm.block_index_max_value_get = dnx_data_gtimer_cdbm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.cdpm.feature_get = dnx_data_gtimer_cdpm_feature_get;

    
    dnx_data_gtimer.cdpm.block_index_size_get = dnx_data_gtimer_cdpm_block_index_size_get;
    dnx_data_gtimer.cdpm.block_index_max_value_get = dnx_data_gtimer_cdpm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ocbm.feature_get = dnx_data_gtimer_ocbm_feature_get;

    
    dnx_data_gtimer.ocbm.block_index_size_get = dnx_data_gtimer_ocbm_block_index_size_get;
    dnx_data_gtimer.ocbm.block_index_max_value_get = dnx_data_gtimer_ocbm_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.mss.feature_get = dnx_data_gtimer_mss_feature_get;

    
    dnx_data_gtimer.mss.block_index_size_get = dnx_data_gtimer_mss_block_index_size_get;
    dnx_data_gtimer.mss.block_index_max_value_get = dnx_data_gtimer_mss_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.msd.feature_get = dnx_data_gtimer_msd_feature_get;

    
    dnx_data_gtimer.msd.block_index_size_get = dnx_data_gtimer_msd_block_index_size_get;
    dnx_data_gtimer.msd.block_index_max_value_get = dnx_data_gtimer_msd_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.msw.feature_get = dnx_data_gtimer_msw_feature_get;

    
    dnx_data_gtimer.msw.block_index_size_get = dnx_data_gtimer_msw_block_index_size_get;
    dnx_data_gtimer.msw.block_index_max_value_get = dnx_data_gtimer_msw_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.arb.feature_get = dnx_data_gtimer_arb_feature_get;

    
    dnx_data_gtimer.arb.block_index_size_get = dnx_data_gtimer_arb_block_index_size_get;
    dnx_data_gtimer.arb.block_index_max_value_get = dnx_data_gtimer_arb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ofr.feature_get = dnx_data_gtimer_ofr_feature_get;

    
    dnx_data_gtimer.ofr.block_index_size_get = dnx_data_gtimer_ofr_block_index_size_get;
    dnx_data_gtimer.ofr.block_index_max_value_get = dnx_data_gtimer_ofr_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.oft.feature_get = dnx_data_gtimer_oft_feature_get;

    
    dnx_data_gtimer.oft.block_index_size_get = dnx_data_gtimer_oft_block_index_size_get;
    dnx_data_gtimer.oft.block_index_max_value_get = dnx_data_gtimer_oft_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.frwa.feature_get = dnx_data_gtimer_frwa_feature_get;

    
    dnx_data_gtimer.frwa.block_index_size_get = dnx_data_gtimer_frwa_block_index_size_get;
    dnx_data_gtimer.frwa.block_index_max_value_get = dnx_data_gtimer_frwa_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.frwbc.feature_get = dnx_data_gtimer_frwbc_feature_get;

    
    dnx_data_gtimer.frwbc.block_index_size_get = dnx_data_gtimer_frwbc_block_index_size_get;
    dnx_data_gtimer.frwbc.block_index_max_value_get = dnx_data_gtimer_frwbc_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.wb.feature_get = dnx_data_gtimer_wb_feature_get;

    
    dnx_data_gtimer.wb.block_index_size_get = dnx_data_gtimer_wb_block_index_size_get;
    dnx_data_gtimer.wb.block_index_max_value_get = dnx_data_gtimer_wb_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.w40.feature_get = dnx_data_gtimer_w40_feature_get;

    
    dnx_data_gtimer.w40.block_index_size_get = dnx_data_gtimer_w40_block_index_size_get;
    dnx_data_gtimer.w40.block_index_max_value_get = dnx_data_gtimer_w40_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.msu.feature_get = dnx_data_gtimer_msu_feature_get;

    
    dnx_data_gtimer.msu.block_index_size_get = dnx_data_gtimer_msu_block_index_size_get;
    dnx_data_gtimer.msu.block_index_max_value_get = dnx_data_gtimer_msu_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.pmu.feature_get = dnx_data_gtimer_pmu_feature_get;

    
    dnx_data_gtimer.pmu.block_index_size_get = dnx_data_gtimer_pmu_block_index_size_get;
    dnx_data_gtimer.pmu.block_index_max_value_get = dnx_data_gtimer_pmu_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.nbu.feature_get = dnx_data_gtimer_nbu_feature_get;

    
    dnx_data_gtimer.nbu.block_index_size_get = dnx_data_gtimer_nbu_block_index_size_get;
    dnx_data_gtimer.nbu.block_index_max_value_get = dnx_data_gtimer_nbu_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.mgu.feature_get = dnx_data_gtimer_mgu_feature_get;

    
    dnx_data_gtimer.mgu.block_index_size_get = dnx_data_gtimer_mgu_block_index_size_get;
    dnx_data_gtimer.mgu.block_index_max_value_get = dnx_data_gtimer_mgu_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ipw.feature_get = dnx_data_gtimer_ipw_feature_get;

    
    dnx_data_gtimer.ipw.block_index_size_get = dnx_data_gtimer_ipw_block_index_size_get;
    dnx_data_gtimer.ipw.block_index_max_value_get = dnx_data_gtimer_ipw_block_index_max_value_get;

    
    
    
    dnx_data_gtimer.ipc.feature_get = dnx_data_gtimer_ipc_feature_get;

    
    dnx_data_gtimer.ipc.block_index_size_get = dnx_data_gtimer_ipc_block_index_size_get;
    dnx_data_gtimer.ipc.block_index_max_value_get = dnx_data_gtimer_ipc_block_index_max_value_get;

    
    
    
    
    dnx_data_gtp.general.feature_get = dnx_data_gtp_general_feature_get;

    
    dnx_data_gtp.general.gtp_term_is_lif_get = dnx_data_gtp_general_gtp_term_is_lif_get;
    dnx_data_gtp.general.match_vrf_max_val_get = dnx_data_gtp_general_match_vrf_max_val_get;

    
    
    
    
    dnx_data_headers.ftmh.feature_get = dnx_data_headers_ftmh_feature_get;

    
    dnx_data_headers.ftmh.packet_size_offset_get = dnx_data_headers_ftmh_packet_size_offset_get;
    dnx_data_headers.ftmh.traffic_class_offset_get = dnx_data_headers_ftmh_traffic_class_offset_get;
    dnx_data_headers.ftmh.src_sys_port_aggregate_offset_get = dnx_data_headers_ftmh_src_sys_port_aggregate_offset_get;
    dnx_data_headers.ftmh.pp_dsp_offset_get = dnx_data_headers_ftmh_pp_dsp_offset_get;
    dnx_data_headers.ftmh.drop_precedence_offset_get = dnx_data_headers_ftmh_drop_precedence_offset_get;
    dnx_data_headers.ftmh.tm_action_type_offset_get = dnx_data_headers_ftmh_tm_action_type_offset_get;
    dnx_data_headers.ftmh.tm_action_is_mc_offset_get = dnx_data_headers_ftmh_tm_action_is_mc_offset_get;
    dnx_data_headers.ftmh.outlif_offset_get = dnx_data_headers_ftmh_outlif_offset_get;
    dnx_data_headers.ftmh.cni_offset_get = dnx_data_headers_ftmh_cni_offset_get;
    dnx_data_headers.ftmh.ecn_enable_offset_get = dnx_data_headers_ftmh_ecn_enable_offset_get;
    dnx_data_headers.ftmh.tm_profile_offset_get = dnx_data_headers_ftmh_tm_profile_offset_get;
    dnx_data_headers.ftmh.visibility_offset_get = dnx_data_headers_ftmh_visibility_offset_get;
    dnx_data_headers.ftmh.internal_header_type_offset_get = dnx_data_headers_ftmh_internal_header_type_offset_get;
    dnx_data_headers.ftmh.tsh_en_offset_get = dnx_data_headers_ftmh_tsh_en_offset_get;
    dnx_data_headers.ftmh.internal_header_en_offset_get = dnx_data_headers_ftmh_internal_header_en_offset_get;
    dnx_data_headers.ftmh.tm_dest_ext_present_offset_get = dnx_data_headers_ftmh_tm_dest_ext_present_offset_get;
    dnx_data_headers.ftmh.ase_present_offset_get = dnx_data_headers_ftmh_ase_present_offset_get;
    dnx_data_headers.ftmh.flow_id_ext_present_offset_get = dnx_data_headers_ftmh_flow_id_ext_present_offset_get;
    dnx_data_headers.ftmh.bier_bfr_ext_present_offset_get = dnx_data_headers_ftmh_bier_bfr_ext_present_offset_get;
    dnx_data_headers.ftmh.stack_route_history_bmp_offset_get = dnx_data_headers_ftmh_stack_route_history_bmp_offset_get;
    dnx_data_headers.ftmh.tm_dst_offset_get = dnx_data_headers_ftmh_tm_dst_offset_get;
    dnx_data_headers.ftmh.observation_info_offset_get = dnx_data_headers_ftmh_observation_info_offset_get;
    dnx_data_headers.ftmh.destination_offset_get = dnx_data_headers_ftmh_destination_offset_get;
    dnx_data_headers.ftmh.src_sysport_offset_get = dnx_data_headers_ftmh_src_sysport_offset_get;
    dnx_data_headers.ftmh.trajectory_trace_type_offset_get = dnx_data_headers_ftmh_trajectory_trace_type_offset_get;
    dnx_data_headers.ftmh.port_offset_get = dnx_data_headers_ftmh_port_offset_get;
    dnx_data_headers.ftmh.direction_offset_get = dnx_data_headers_ftmh_direction_offset_get;
    dnx_data_headers.ftmh.session_id_offset_get = dnx_data_headers_ftmh_session_id_offset_get;
    dnx_data_headers.ftmh.truncated_offset_get = dnx_data_headers_ftmh_truncated_offset_get;
    dnx_data_headers.ftmh.en_offset_get = dnx_data_headers_ftmh_en_offset_get;
    dnx_data_headers.ftmh.cos_offset_get = dnx_data_headers_ftmh_cos_offset_get;
    dnx_data_headers.ftmh.vlan_offset_get = dnx_data_headers_ftmh_vlan_offset_get;
    dnx_data_headers.ftmh.erspan_type_offset_get = dnx_data_headers_ftmh_erspan_type_offset_get;
    dnx_data_headers.ftmh.flow_id_offset_get = dnx_data_headers_ftmh_flow_id_offset_get;
    dnx_data_headers.ftmh.flow_profile_offset_get = dnx_data_headers_ftmh_flow_profile_offset_get;
    dnx_data_headers.ftmh.int_profile_offset_get = dnx_data_headers_ftmh_int_profile_offset_get;
    dnx_data_headers.ftmh.int_type_offset_get = dnx_data_headers_ftmh_int_type_offset_get;
    dnx_data_headers.ftmh.packet_size_bits_get = dnx_data_headers_ftmh_packet_size_bits_get;
    dnx_data_headers.ftmh.traffic_class_bits_get = dnx_data_headers_ftmh_traffic_class_bits_get;
    dnx_data_headers.ftmh.src_sys_port_aggregate_bits_get = dnx_data_headers_ftmh_src_sys_port_aggregate_bits_get;
    dnx_data_headers.ftmh.pp_dsp_bits_get = dnx_data_headers_ftmh_pp_dsp_bits_get;
    dnx_data_headers.ftmh.drop_precedence_bits_get = dnx_data_headers_ftmh_drop_precedence_bits_get;
    dnx_data_headers.ftmh.tm_action_type_bits_get = dnx_data_headers_ftmh_tm_action_type_bits_get;
    dnx_data_headers.ftmh.tm_action_is_mc_bits_get = dnx_data_headers_ftmh_tm_action_is_mc_bits_get;
    dnx_data_headers.ftmh.outlif_bits_get = dnx_data_headers_ftmh_outlif_bits_get;
    dnx_data_headers.ftmh.cni_bits_get = dnx_data_headers_ftmh_cni_bits_get;
    dnx_data_headers.ftmh.ecn_enable_bits_get = dnx_data_headers_ftmh_ecn_enable_bits_get;
    dnx_data_headers.ftmh.tm_profile_bits_get = dnx_data_headers_ftmh_tm_profile_bits_get;
    dnx_data_headers.ftmh.visibility_bits_get = dnx_data_headers_ftmh_visibility_bits_get;
    dnx_data_headers.ftmh.tsh_en_bits_get = dnx_data_headers_ftmh_tsh_en_bits_get;
    dnx_data_headers.ftmh.internal_header_en_bits_get = dnx_data_headers_ftmh_internal_header_en_bits_get;
    dnx_data_headers.ftmh.tm_dest_ext_present_bits_get = dnx_data_headers_ftmh_tm_dest_ext_present_bits_get;
    dnx_data_headers.ftmh.ase_present_bits_get = dnx_data_headers_ftmh_ase_present_bits_get;
    dnx_data_headers.ftmh.flow_id_ext_present_bits_get = dnx_data_headers_ftmh_flow_id_ext_present_bits_get;
    dnx_data_headers.ftmh.bier_bfr_ext_present_bits_get = dnx_data_headers_ftmh_bier_bfr_ext_present_bits_get;
    dnx_data_headers.ftmh.tm_dst_bits_get = dnx_data_headers_ftmh_tm_dst_bits_get;
    dnx_data_headers.ftmh.observation_info_bits_get = dnx_data_headers_ftmh_observation_info_bits_get;
    dnx_data_headers.ftmh.destination_bits_get = dnx_data_headers_ftmh_destination_bits_get;
    dnx_data_headers.ftmh.src_sysport_bits_get = dnx_data_headers_ftmh_src_sysport_bits_get;
    dnx_data_headers.ftmh.trajectory_trace_type_bits_get = dnx_data_headers_ftmh_trajectory_trace_type_bits_get;
    dnx_data_headers.ftmh.port_bits_get = dnx_data_headers_ftmh_port_bits_get;
    dnx_data_headers.ftmh.direction_bits_get = dnx_data_headers_ftmh_direction_bits_get;
    dnx_data_headers.ftmh.session_id_bits_get = dnx_data_headers_ftmh_session_id_bits_get;
    dnx_data_headers.ftmh.truncated_bits_get = dnx_data_headers_ftmh_truncated_bits_get;
    dnx_data_headers.ftmh.en_bits_get = dnx_data_headers_ftmh_en_bits_get;
    dnx_data_headers.ftmh.cos_bits_get = dnx_data_headers_ftmh_cos_bits_get;
    dnx_data_headers.ftmh.vlan_bits_get = dnx_data_headers_ftmh_vlan_bits_get;
    dnx_data_headers.ftmh.erspan_type_bits_get = dnx_data_headers_ftmh_erspan_type_bits_get;
    dnx_data_headers.ftmh.flow_id_bits_get = dnx_data_headers_ftmh_flow_id_bits_get;
    dnx_data_headers.ftmh.flow_profile_bits_get = dnx_data_headers_ftmh_flow_profile_bits_get;
    dnx_data_headers.ftmh.base_header_size_get = dnx_data_headers_ftmh_base_header_size_get;
    dnx_data_headers.ftmh.ase_header_size_get = dnx_data_headers_ftmh_ase_header_size_get;
    dnx_data_headers.ftmh.tm_dst_size_get = dnx_data_headers_ftmh_tm_dst_size_get;
    dnx_data_headers.ftmh.flow_id_header_size_get = dnx_data_headers_ftmh_flow_id_header_size_get;
    dnx_data_headers.ftmh.int_profile_bits_get = dnx_data_headers_ftmh_int_profile_bits_get;
    dnx_data_headers.ftmh.int_type_bits_get = dnx_data_headers_ftmh_int_type_bits_get;
    dnx_data_headers.ftmh.ase_oam_type_offset_get = dnx_data_headers_ftmh_ase_oam_type_offset_get;
    dnx_data_headers.ftmh.ase_oam_offset_offset_get = dnx_data_headers_ftmh_ase_oam_offset_offset_get;
    dnx_data_headers.ftmh.ase_oam_ts_data_offset_get = dnx_data_headers_ftmh_ase_oam_ts_data_offset_get;
    dnx_data_headers.ftmh.ase_oam_ts_data_upper_offset_get = dnx_data_headers_ftmh_ase_oam_ts_data_upper_offset_get;
    dnx_data_headers.ftmh.ase_oam_mep_type_offset_get = dnx_data_headers_ftmh_ase_oam_mep_type_offset_get;
    dnx_data_headers.ftmh.ase_oam_sub_type_offset_get = dnx_data_headers_ftmh_ase_oam_sub_type_offset_get;
    dnx_data_headers.ftmh.ase_oam_type_bits_get = dnx_data_headers_ftmh_ase_oam_type_bits_get;
    dnx_data_headers.ftmh.ase_oam_offset_bits_get = dnx_data_headers_ftmh_ase_oam_offset_bits_get;
    dnx_data_headers.ftmh.ase_oam_ts_data_bits_get = dnx_data_headers_ftmh_ase_oam_ts_data_bits_get;
    dnx_data_headers.ftmh.ase_oam_ts_data_upper_bits_get = dnx_data_headers_ftmh_ase_oam_ts_data_upper_bits_get;
    dnx_data_headers.ftmh.ase_oam_mep_type_bits_get = dnx_data_headers_ftmh_ase_oam_mep_type_bits_get;
    dnx_data_headers.ftmh.ase_oam_sub_type_bits_get = dnx_data_headers_ftmh_ase_oam_sub_type_bits_get;
    dnx_data_headers.ftmh.ase_type_offset_get = dnx_data_headers_ftmh_ase_type_offset_get;
    dnx_data_headers.ftmh.ase_1588_offset_offset_get = dnx_data_headers_ftmh_ase_1588_offset_offset_get;
    dnx_data_headers.ftmh.ase_1588_tp_command_offset_get = dnx_data_headers_ftmh_ase_1588_tp_command_offset_get;
    dnx_data_headers.ftmh.ase_1588_ts_encapsulation_offset_get = dnx_data_headers_ftmh_ase_1588_ts_encapsulation_offset_get;
    dnx_data_headers.ftmh.ase_type_bits_get = dnx_data_headers_ftmh_ase_type_bits_get;
    dnx_data_headers.ftmh.ase_1588_offset_bits_get = dnx_data_headers_ftmh_ase_1588_offset_bits_get;
    dnx_data_headers.ftmh.ase_1588_tp_command_bits_get = dnx_data_headers_ftmh_ase_1588_tp_command_bits_get;
    dnx_data_headers.ftmh.ase_1588_ts_encapsulation_bits_get = dnx_data_headers_ftmh_ase_1588_ts_encapsulation_bits_get;
    dnx_data_headers.ftmh.add_dsp_ext_get = dnx_data_headers_ftmh_add_dsp_ext_get;

    
    
    
    dnx_data_headers.tsh.feature_get = dnx_data_headers_tsh_feature_get;

    
    dnx_data_headers.tsh.base_header_size_get = dnx_data_headers_tsh_base_header_size_get;
    dnx_data_headers.tsh.time_stamp_offset_get = dnx_data_headers_tsh_time_stamp_offset_get;
    dnx_data_headers.tsh.time_stamp_bits_get = dnx_data_headers_tsh_time_stamp_bits_get;

    
    
    
    dnx_data_headers.otsh.feature_get = dnx_data_headers_otsh_feature_get;

    
    dnx_data_headers.otsh.base_header_size_get = dnx_data_headers_otsh_base_header_size_get;

    
    
    
    dnx_data_headers.optimized_ftmh.feature_get = dnx_data_headers_optimized_ftmh_feature_get;

    
    dnx_data_headers.optimized_ftmh.base_header_size_get = dnx_data_headers_optimized_ftmh_base_header_size_get;
    dnx_data_headers.optimized_ftmh.packet_size_7_0_offset_get = dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_get;
    dnx_data_headers.optimized_ftmh.tm_action_is_mc_offset_get = dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_get;
    dnx_data_headers.optimized_ftmh.packet_size_8_offset_get = dnx_data_headers_optimized_ftmh_packet_size_8_offset_get;
    dnx_data_headers.optimized_ftmh.user_defined_offset_get = dnx_data_headers_optimized_ftmh_user_defined_offset_get;
    dnx_data_headers.optimized_ftmh.fap_id_offset_get = dnx_data_headers_optimized_ftmh_fap_id_offset_get;
    dnx_data_headers.optimized_ftmh.pp_dsp_or_mc_id_offset_get = dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_get;

    
    
    
    dnx_data_headers.internal.feature_get = dnx_data_headers_internal_feature_get;

    
    dnx_data_headers.internal.parsing_start_type_offset_get = dnx_data_headers_internal_parsing_start_type_offset_get;
    dnx_data_headers.internal.parsing_start_type_bits_get = dnx_data_headers_internal_parsing_start_type_bits_get;
    dnx_data_headers.internal.lif_ext_type_offset_get = dnx_data_headers_internal_lif_ext_type_offset_get;
    dnx_data_headers.internal.lif_ext_type_bits_get = dnx_data_headers_internal_lif_ext_type_bits_get;
    dnx_data_headers.internal.fai_offset_get = dnx_data_headers_internal_fai_offset_get;
    dnx_data_headers.internal.fai_bits_get = dnx_data_headers_internal_fai_bits_get;
    dnx_data_headers.internal.tail_edit_offset_get = dnx_data_headers_internal_tail_edit_offset_get;
    dnx_data_headers.internal.tail_edit_bits_get = dnx_data_headers_internal_tail_edit_bits_get;
    dnx_data_headers.internal.forward_domain_offset_get = dnx_data_headers_internal_forward_domain_offset_get;
    dnx_data_headers.internal.forward_domain_bits_get = dnx_data_headers_internal_forward_domain_bits_get;
    dnx_data_headers.internal.inlif_offset_get = dnx_data_headers_internal_inlif_offset_get;
    dnx_data_headers.internal.inlif_bits_get = dnx_data_headers_internal_inlif_bits_get;
    dnx_data_headers.internal.fhei_size_get = dnx_data_headers_internal_fhei_size_get;
    dnx_data_headers.internal.fhei_size_sz0_get = dnx_data_headers_internal_fhei_size_sz0_get;
    dnx_data_headers.internal.fhei_size_sz1_get = dnx_data_headers_internal_fhei_size_sz1_get;
    dnx_data_headers.internal.fhei_size_sz2_get = dnx_data_headers_internal_fhei_size_sz2_get;
    dnx_data_headers.internal.fhei_size_offset_get = dnx_data_headers_internal_fhei_size_offset_get;
    dnx_data_headers.internal.fhei_size_bits_get = dnx_data_headers_internal_fhei_size_bits_get;
    dnx_data_headers.internal.learning_ext_present_offset_get = dnx_data_headers_internal_learning_ext_present_offset_get;
    dnx_data_headers.internal.learning_ext_present_bits_get = dnx_data_headers_internal_learning_ext_present_bits_get;
    dnx_data_headers.internal.learning_ext_size_get = dnx_data_headers_internal_learning_ext_size_get;
    dnx_data_headers.internal.ttl_offset_get = dnx_data_headers_internal_ttl_offset_get;
    dnx_data_headers.internal.ttl_bits_get = dnx_data_headers_internal_ttl_bits_get;
    dnx_data_headers.internal.fwd_layer_index_offset_get = dnx_data_headers_internal_fwd_layer_index_offset_get;
    dnx_data_headers.internal.fwd_layer_index_bits_get = dnx_data_headers_internal_fwd_layer_index_bits_get;
    dnx_data_headers.internal.eth_rm_fli_offset_get = dnx_data_headers_internal_eth_rm_fli_offset_get;
    dnx_data_headers.internal.eth_rm_fli_bits_get = dnx_data_headers_internal_eth_rm_fli_bits_get;
    dnx_data_headers.internal.eth_rm_pso_offset_get = dnx_data_headers_internal_eth_rm_pso_offset_get;
    dnx_data_headers.internal.eth_rm_pso_bits_get = dnx_data_headers_internal_eth_rm_pso_bits_get;

    
    
    
    dnx_data_headers.internal_legacy.feature_get = dnx_data_headers_internal_legacy_feature_get;

    
    dnx_data_headers.internal_legacy.fhei_size_get = dnx_data_headers_internal_legacy_fhei_size_get;
    dnx_data_headers.internal_legacy.fhei_size_sz0_get = dnx_data_headers_internal_legacy_fhei_size_sz0_get;
    dnx_data_headers.internal_legacy.fhei_size_sz1_get = dnx_data_headers_internal_legacy_fhei_size_sz1_get;
    dnx_data_headers.internal_legacy.fhei_size_sz2_get = dnx_data_headers_internal_legacy_fhei_size_sz2_get;
    dnx_data_headers.internal_legacy.inlif_inrif_offset_get = dnx_data_headers_internal_legacy_inlif_inrif_offset_get;
    dnx_data_headers.internal_legacy.inlif_inrif_bits_get = dnx_data_headers_internal_legacy_inlif_inrif_bits_get;
    dnx_data_headers.internal_legacy.unknown_address_offset_get = dnx_data_headers_internal_legacy_unknown_address_offset_get;
    dnx_data_headers.internal_legacy.unknown_address_bits_get = dnx_data_headers_internal_legacy_unknown_address_bits_get;
    dnx_data_headers.internal_legacy.forwarding_header_offset_offset_get = dnx_data_headers_internal_legacy_forwarding_header_offset_offset_get;
    dnx_data_headers.internal_legacy.forwarding_header_offset_bits_get = dnx_data_headers_internal_legacy_forwarding_header_offset_bits_get;
    dnx_data_headers.internal_legacy.forward_code_offset_get = dnx_data_headers_internal_legacy_forward_code_offset_get;
    dnx_data_headers.internal_legacy.forward_code_bits_get = dnx_data_headers_internal_legacy_forward_code_bits_get;
    dnx_data_headers.internal_legacy.fhei_size_offset_get = dnx_data_headers_internal_legacy_fhei_size_offset_get;
    dnx_data_headers.internal_legacy.fhei_size_bits_get = dnx_data_headers_internal_legacy_fhei_size_bits_get;
    dnx_data_headers.internal_legacy.learning_ext_present_offset_get = dnx_data_headers_internal_legacy_learning_ext_present_offset_get;
    dnx_data_headers.internal_legacy.learning_ext_present_bits_get = dnx_data_headers_internal_legacy_learning_ext_present_bits_get;
    dnx_data_headers.internal_legacy.learning_ext_size_get = dnx_data_headers_internal_legacy_learning_ext_size_get;
    dnx_data_headers.internal_legacy.eei_ext_present_offset_get = dnx_data_headers_internal_legacy_eei_ext_present_offset_get;
    dnx_data_headers.internal_legacy.eei_ext_present_bits_get = dnx_data_headers_internal_legacy_eei_ext_present_bits_get;
    dnx_data_headers.internal_legacy.vsi_outrif_offset_get = dnx_data_headers_internal_legacy_vsi_outrif_offset_get;
    dnx_data_headers.internal_legacy.vsi_outrif_bits_get = dnx_data_headers_internal_legacy_vsi_outrif_bits_get;

    
    
    
    dnx_data_headers.fhei_sz1_trap_sniff.feature_get = dnx_data_headers_fhei_sz1_trap_sniff_feature_get;

    
    dnx_data_headers.fhei_sz1_trap_sniff.fhei_type_offset_get = dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_get;
    dnx_data_headers.fhei_sz1_trap_sniff.fhei_type_bits_get = dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_get;
    dnx_data_headers.fhei_sz1_trap_sniff.code_offset_get = dnx_data_headers_fhei_sz1_trap_sniff_code_offset_get;
    dnx_data_headers.fhei_sz1_trap_sniff.code_bits_get = dnx_data_headers_fhei_sz1_trap_sniff_code_bits_get;
    dnx_data_headers.fhei_sz1_trap_sniff.qualifier_offset_get = dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_get;
    dnx_data_headers.fhei_sz1_trap_sniff.qualifier_bits_get = dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_get;

    
    
    
    dnx_data_headers.fhei_trap_sniff_legacy.feature_get = dnx_data_headers_fhei_trap_sniff_legacy_feature_get;

    
    dnx_data_headers.fhei_trap_sniff_legacy.code_offset_get = dnx_data_headers_fhei_trap_sniff_legacy_code_offset_get;
    dnx_data_headers.fhei_trap_sniff_legacy.code_bits_get = dnx_data_headers_fhei_trap_sniff_legacy_code_bits_get;
    dnx_data_headers.fhei_trap_sniff_legacy.qualifier_offset_get = dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_get;
    dnx_data_headers.fhei_trap_sniff_legacy.qualifier_bits_get = dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_get;

    
    
    
    dnx_data_headers.fhei_sz1_stamp_ive.feature_get = dnx_data_headers_fhei_sz1_stamp_ive_feature_get;

    
    dnx_data_headers.fhei_sz1_stamp_ive.fhei_ivec_offset_get = dnx_data_headers_fhei_sz1_stamp_ive_fhei_ivec_offset_get;
    dnx_data_headers.fhei_sz1_stamp_ive.fhei_ivec_bits_get = dnx_data_headers_fhei_sz1_stamp_ive_fhei_ivec_bits_get;
    dnx_data_headers.fhei_sz1_stamp_ive.fhei_ive_type_offset_get = dnx_data_headers_fhei_sz1_stamp_ive_fhei_ive_type_offset_get;
    dnx_data_headers.fhei_sz1_stamp_ive.fhei_ive_type_bits_get = dnx_data_headers_fhei_sz1_stamp_ive_fhei_ive_type_bits_get;

    
    
    
    dnx_data_headers.udh.feature_get = dnx_data_headers_udh_feature_get;

    
    dnx_data_headers.udh.data_type0_offset_get = dnx_data_headers_udh_data_type0_offset_get;
    dnx_data_headers.udh.data_type1_offset_get = dnx_data_headers_udh_data_type1_offset_get;
    dnx_data_headers.udh.data_type2_offset_get = dnx_data_headers_udh_data_type2_offset_get;
    dnx_data_headers.udh.data_type3_offset_get = dnx_data_headers_udh_data_type3_offset_get;
    dnx_data_headers.udh.data_offset_get = dnx_data_headers_udh_data_offset_get;
    dnx_data_headers.udh.data_type0_bits_get = dnx_data_headers_udh_data_type0_bits_get;
    dnx_data_headers.udh.data_type1_bits_get = dnx_data_headers_udh_data_type1_bits_get;
    dnx_data_headers.udh.data_type2_bits_get = dnx_data_headers_udh_data_type2_bits_get;
    dnx_data_headers.udh.data_type3_bits_get = dnx_data_headers_udh_data_type3_bits_get;
    dnx_data_headers.udh.data_type0_size_get = dnx_data_headers_udh_data_type0_size_get;
    dnx_data_headers.udh.data_type1_size_get = dnx_data_headers_udh_data_type1_size_get;
    dnx_data_headers.udh.data_type2_size_get = dnx_data_headers_udh_data_type2_size_get;
    dnx_data_headers.udh.data_type3_size_get = dnx_data_headers_udh_data_type3_size_get;
    dnx_data_headers.udh.data_size_get = dnx_data_headers_udh_data_size_get;
    dnx_data_headers.udh.base_size_get = dnx_data_headers_udh_base_size_get;

    
    
    
    dnx_data_headers.ftmh_legacy.feature_get = dnx_data_headers_ftmh_legacy_feature_get;

    
    dnx_data_headers.ftmh_legacy.packet_size_offset_get = dnx_data_headers_ftmh_legacy_packet_size_offset_get;
    dnx_data_headers.ftmh_legacy.traffic_class_offset_get = dnx_data_headers_ftmh_legacy_traffic_class_offset_get;
    dnx_data_headers.ftmh_legacy.src_sys_port_aggregate_offset_get = dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_get;
    dnx_data_headers.ftmh_legacy.pp_dsp_offset_get = dnx_data_headers_ftmh_legacy_pp_dsp_offset_get;
    dnx_data_headers.ftmh_legacy.drop_precedence_offset_get = dnx_data_headers_ftmh_legacy_drop_precedence_offset_get;
    dnx_data_headers.ftmh_legacy.tm_action_type_offset_get = dnx_data_headers_ftmh_legacy_tm_action_type_offset_get;
    dnx_data_headers.ftmh_legacy.otsh_en_offset_get = dnx_data_headers_ftmh_legacy_otsh_en_offset_get;
    dnx_data_headers.ftmh_legacy.internal_header_en_offset_get = dnx_data_headers_ftmh_legacy_internal_header_en_offset_get;
    dnx_data_headers.ftmh_legacy.outbound_mirr_disable_offset_get = dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_get;
    dnx_data_headers.ftmh_legacy.tm_action_is_mc_offset_get = dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_get;
    dnx_data_headers.ftmh_legacy.outlif_offset_get = dnx_data_headers_ftmh_legacy_outlif_offset_get;
    dnx_data_headers.ftmh_legacy.dsp_ext_present_offset_get = dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_get;
    dnx_data_headers.ftmh_legacy.cni_offset_get = dnx_data_headers_ftmh_legacy_cni_offset_get;
    dnx_data_headers.ftmh_legacy.ecn_enable_offset_get = dnx_data_headers_ftmh_legacy_ecn_enable_offset_get;
    dnx_data_headers.ftmh_legacy.exclude_source_offset_get = dnx_data_headers_ftmh_legacy_exclude_source_offset_get;
    dnx_data_headers.ftmh_legacy.stack_route_history_bmp_offset_get = dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_get;
    dnx_data_headers.ftmh_legacy.dsp_ext_header_offset_get = dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_get;
    dnx_data_headers.ftmh_legacy.packet_size_bits_get = dnx_data_headers_ftmh_legacy_packet_size_bits_get;
    dnx_data_headers.ftmh_legacy.traffic_class_bits_get = dnx_data_headers_ftmh_legacy_traffic_class_bits_get;
    dnx_data_headers.ftmh_legacy.src_sys_port_aggregate_bits_get = dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_get;
    dnx_data_headers.ftmh_legacy.pp_dsp_bits_get = dnx_data_headers_ftmh_legacy_pp_dsp_bits_get;
    dnx_data_headers.ftmh_legacy.drop_precedence_bits_get = dnx_data_headers_ftmh_legacy_drop_precedence_bits_get;
    dnx_data_headers.ftmh_legacy.tm_action_type_bits_get = dnx_data_headers_ftmh_legacy_tm_action_type_bits_get;
    dnx_data_headers.ftmh_legacy.otsh_en_bits_get = dnx_data_headers_ftmh_legacy_otsh_en_bits_get;
    dnx_data_headers.ftmh_legacy.internal_header_en_bits_get = dnx_data_headers_ftmh_legacy_internal_header_en_bits_get;
    dnx_data_headers.ftmh_legacy.outbound_mirr_disable_bits_get = dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_get;
    dnx_data_headers.ftmh_legacy.tm_action_is_mc_bits_get = dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_get;
    dnx_data_headers.ftmh_legacy.outlif_bits_get = dnx_data_headers_ftmh_legacy_outlif_bits_get;
    dnx_data_headers.ftmh_legacy.dsp_ext_present_bits_get = dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_get;
    dnx_data_headers.ftmh_legacy.cni_bits_get = dnx_data_headers_ftmh_legacy_cni_bits_get;
    dnx_data_headers.ftmh_legacy.ecn_enable_bits_get = dnx_data_headers_ftmh_legacy_ecn_enable_bits_get;
    dnx_data_headers.ftmh_legacy.exclude_source_bits_get = dnx_data_headers_ftmh_legacy_exclude_source_bits_get;
    dnx_data_headers.ftmh_legacy.dsp_ext_header_bits_get = dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_get;
    dnx_data_headers.ftmh_legacy.base_header_size_get = dnx_data_headers_ftmh_legacy_base_header_size_get;
    dnx_data_headers.ftmh_legacy.dsp_ext_header_size_get = dnx_data_headers_ftmh_legacy_dsp_ext_header_size_get;

    
    
    
    dnx_data_headers.otsh_legacy.feature_get = dnx_data_headers_otsh_legacy_feature_get;

    
    dnx_data_headers.otsh_legacy.type_offset_get = dnx_data_headers_otsh_legacy_type_offset_get;
    dnx_data_headers.otsh_legacy.type_bits_get = dnx_data_headers_otsh_legacy_type_bits_get;
    dnx_data_headers.otsh_legacy.oam_sub_type_offset_get = dnx_data_headers_otsh_legacy_oam_sub_type_offset_get;
    dnx_data_headers.otsh_legacy.oam_sub_type_bits_get = dnx_data_headers_otsh_legacy_oam_sub_type_bits_get;
    dnx_data_headers.otsh_legacy.mep_type_offset_get = dnx_data_headers_otsh_legacy_mep_type_offset_get;
    dnx_data_headers.otsh_legacy.mep_type_bits_get = dnx_data_headers_otsh_legacy_mep_type_bits_get;
    dnx_data_headers.otsh_legacy.tp_command_offset_get = dnx_data_headers_otsh_legacy_tp_command_offset_get;
    dnx_data_headers.otsh_legacy.tp_command_bits_get = dnx_data_headers_otsh_legacy_tp_command_bits_get;
    dnx_data_headers.otsh_legacy.ts_encapsulation_offset_get = dnx_data_headers_otsh_legacy_ts_encapsulation_offset_get;
    dnx_data_headers.otsh_legacy.ts_encapsulation_bits_get = dnx_data_headers_otsh_legacy_ts_encapsulation_bits_get;
    dnx_data_headers.otsh_legacy.oam_ts_data_0_offset_get = dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_get;
    dnx_data_headers.otsh_legacy.oam_ts_data_0_bits_get = dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_get;
    dnx_data_headers.otsh_legacy.oam_ts_data_1_offset_get = dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_get;
    dnx_data_headers.otsh_legacy.oam_ts_data_1_bits_get = dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_get;
    dnx_data_headers.otsh_legacy.offset_offset_get = dnx_data_headers_otsh_legacy_offset_offset_get;
    dnx_data_headers.otsh_legacy.offset_bits_get = dnx_data_headers_otsh_legacy_offset_bits_get;
    dnx_data_headers.otsh_legacy.latency_flow_id_offset_get = dnx_data_headers_otsh_legacy_latency_flow_id_offset_get;
    dnx_data_headers.otsh_legacy.latency_flow_id_bits_get = dnx_data_headers_otsh_legacy_latency_flow_id_bits_get;
    dnx_data_headers.otsh_legacy.latency_time_stamp_offset_get = dnx_data_headers_otsh_legacy_latency_time_stamp_offset_get;
    dnx_data_headers.otsh_legacy.latency_time_stamp_bits_get = dnx_data_headers_otsh_legacy_latency_time_stamp_bits_get;

    
    
    
    dnx_data_headers.system_headers.feature_get = dnx_data_headers_system_headers_feature_get;

    
    dnx_data_headers.system_headers.system_headers_mode_jericho_get = dnx_data_headers_system_headers_system_headers_mode_jericho_get;
    dnx_data_headers.system_headers.system_headers_mode_jericho2_get = dnx_data_headers_system_headers_system_headers_mode_jericho2_get;
    dnx_data_headers.system_headers.crc_size_counted_in_pkt_len_get = dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_get;
    dnx_data_headers.system_headers.system_headers_mode_get = dnx_data_headers_system_headers_system_headers_mode_get;
    dnx_data_headers.system_headers.jr_mode_ftmh_lb_key_ext_mode_get = dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_get;
    dnx_data_headers.system_headers.jr_mode_ftmh_stacking_ext_mode_get = dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_get;
    dnx_data_headers.system_headers.jr_mode_pph_learn_extension_disable_get = dnx_data_headers_system_headers_jr_mode_pph_learn_extension_disable_get;
    dnx_data_headers.system_headers.udh_base_size_get = dnx_data_headers_system_headers_udh_base_size_get;
    dnx_data_headers.system_headers.max_udh0_block_size_get = dnx_data_headers_system_headers_max_udh0_block_size_get;
    dnx_data_headers.system_headers.max_udh1_block_size_get = dnx_data_headers_system_headers_max_udh1_block_size_get;
    dnx_data_headers.system_headers.max_udh2_block_size_get = dnx_data_headers_system_headers_max_udh2_block_size_get;
    dnx_data_headers.system_headers.max_udh3_block_size_get = dnx_data_headers_system_headers_max_udh3_block_size_get;
    dnx_data_headers.system_headers.itmh_itpp_termination_get = dnx_data_headers_system_headers_itmh_itpp_termination_get;
    dnx_data_headers.system_headers.jr_mode_ftmh_oam_ts_right_shift_get = dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_get;
    dnx_data_headers.system_headers.jr2_mode_ftmh_stacking_ext_mode_get = dnx_data_headers_system_headers_jr2_mode_ftmh_stacking_ext_mode_get;

    
    
    
    dnx_data_headers.otmh.feature_get = dnx_data_headers_otmh_feature_get;

    
    dnx_data_headers.otmh.otmh_outlif_ext_mode_get = dnx_data_headers_otmh_otmh_outlif_ext_mode_get;
    dnx_data_headers.otmh.otmh_src_ext_enable_get = dnx_data_headers_otmh_otmh_src_ext_enable_get;

    
    
    
    dnx_data_headers.pph.feature_get = dnx_data_headers_pph_feature_get;

    
    dnx_data_headers.pph.pph_vsi_selection_size_get = dnx_data_headers_pph_pph_vsi_selection_size_get;

    
    
    
    dnx_data_headers.general.feature_get = dnx_data_headers_general_feature_get;

    

    
    
    
    dnx_data_headers.layer_records.feature_get = dnx_data_headers_layer_records_feature_get;

    

    
    
    
    dnx_data_headers.feature.feature_get = dnx_data_headers_feature_feature_get;

    
    dnx_data_headers.feature.fhei_mpls_swap_remark_size_get = dnx_data_headers_feature_fhei_mpls_swap_remark_size_get;

    
#ifdef INCLUDE_CTEST

    
    
    
    dnx_data_infra_tests.res_mngr_example.feature_get = dnx_data_infra_tests_res_mngr_example_feature_get;

    
    dnx_data_infra_tests.res_mngr_example.nof_tags_levels_get = dnx_data_infra_tests_res_mngr_example_nof_tags_levels_get;
    dnx_data_infra_tests.res_mngr_example.grain_size_1_get = dnx_data_infra_tests_res_mngr_example_grain_size_1_get;
    dnx_data_infra_tests.res_mngr_example.grain_size_2_get = dnx_data_infra_tests_res_mngr_example_grain_size_2_get;
    dnx_data_infra_tests.res_mngr_example.max_tag_value_1_get = dnx_data_infra_tests_res_mngr_example_max_tag_value_1_get;
    dnx_data_infra_tests.res_mngr_example.max_tag_value_2_get = dnx_data_infra_tests_res_mngr_example_max_tag_value_2_get;
    dnx_data_infra_tests.res_mngr_example.nof_elements_get = dnx_data_infra_tests_res_mngr_example_nof_elements_get;
    dnx_data_infra_tests.res_mngr_example.first_element_get = dnx_data_infra_tests_res_mngr_example_first_element_get;
    dnx_data_infra_tests.res_mngr_example.create_flags_get = dnx_data_infra_tests_res_mngr_example_create_flags_get;
    dnx_data_infra_tests.res_mngr_example.extra_arg_get = dnx_data_infra_tests_res_mngr_example_extra_arg_get;
    dnx_data_infra_tests.res_mngr_example.advanced_algorithm_get = dnx_data_infra_tests_res_mngr_example_advanced_algorithm_get;
    dnx_data_infra_tests.res_mngr_example.data_per_entry_size_get = dnx_data_infra_tests_res_mngr_example_data_per_entry_size_get;

    
    
    
    dnx_data_infra_tests.temp_mngr_example.feature_get = dnx_data_infra_tests_temp_mngr_example_feature_get;

    
    dnx_data_infra_tests.temp_mngr_example.create_flags_get = dnx_data_infra_tests_temp_mngr_example_create_flags_get;
    dnx_data_infra_tests.temp_mngr_example.first_profile_get = dnx_data_infra_tests_temp_mngr_example_first_profile_get;
    dnx_data_infra_tests.temp_mngr_example.nof_profiles_get = dnx_data_infra_tests_temp_mngr_example_nof_profiles_get;
    dnx_data_infra_tests.temp_mngr_example.max_references_get = dnx_data_infra_tests_temp_mngr_example_max_references_get;
    dnx_data_infra_tests.temp_mngr_example.default_profile_get = dnx_data_infra_tests_temp_mngr_example_default_profile_get;
    dnx_data_infra_tests.temp_mngr_example.advanced_algorithm_get = dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_get;
    dnx_data_infra_tests.temp_mngr_example.extra_arg_get = dnx_data_infra_tests_temp_mngr_example_extra_arg_get;
    dnx_data_infra_tests.temp_mngr_example.element_8_get = dnx_data_infra_tests_temp_mngr_example_element_8_get;
    dnx_data_infra_tests.temp_mngr_example.element_16_get = dnx_data_infra_tests_temp_mngr_example_element_16_get;
    dnx_data_infra_tests.temp_mngr_example.element_32_get = dnx_data_infra_tests_temp_mngr_example_element_32_get;
    dnx_data_infra_tests.temp_mngr_example.element_int_get = dnx_data_infra_tests_temp_mngr_example_element_int_get;

    
    
    
    dnx_data_infra_tests.linked_list_example.feature_get = dnx_data_infra_tests_linked_list_example_feature_get;

    

    
    
    
    dnx_data_infra_tests.sw_state.feature_get = dnx_data_infra_tests_sw_state_feature_get;

    
    dnx_data_infra_tests.sw_state.warmboot_backoff_rate_get = dnx_data_infra_tests_sw_state_warmboot_backoff_rate_get;
    dnx_data_infra_tests.sw_state.nof_guaranteed_reboots_get = dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_get;
    dnx_data_infra_tests.sw_state.htb_max_nof_elements_get = dnx_data_infra_tests_sw_state_htb_max_nof_elements_get;
    dnx_data_infra_tests.sw_state.htb_max_nof_elements_index_get = dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_get;
    dnx_data_infra_tests.sw_state.insert_nof_pairs_get = dnx_data_infra_tests_sw_state_insert_nof_pairs_get;
    dnx_data_infra_tests.sw_state.htb_rh_hashing_algorithm_get = dnx_data_infra_tests_sw_state_htb_rh_hashing_algorithm_get;
    dnx_data_infra_tests.sw_state.htb_rh_create_flags_get = dnx_data_infra_tests_sw_state_htb_rh_create_flags_get;

    

#endif 
    
    
    
    dnx_data_ingr_congestion.config.feature_get = dnx_data_ingr_congestion_config_feature_get;

    
    dnx_data_ingr_congestion.config.guarantee_mode_get = dnx_data_ingr_congestion_config_guarantee_mode_get;
    dnx_data_ingr_congestion.config.wred_packet_size_get = dnx_data_ingr_congestion_config_wred_packet_size_get;

    
    
    
    dnx_data_ingr_congestion.info.feature_get = dnx_data_ingr_congestion_info_feature_get;

    
    dnx_data_ingr_congestion.info.threshold_granularity_get = dnx_data_ingr_congestion_info_threshold_granularity_get;
    dnx_data_ingr_congestion.info.words_resolution_get = dnx_data_ingr_congestion_info_words_resolution_get;
    dnx_data_ingr_congestion.info.bytes_threshold_granularity_get = dnx_data_ingr_congestion_info_bytes_threshold_granularity_get;
    dnx_data_ingr_congestion.info.nof_dropped_reasons_cgm_get = dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_get;
    dnx_data_ingr_congestion.info.wred_max_gain_get = dnx_data_ingr_congestion_info_wred_max_gain_get;
    dnx_data_ingr_congestion.info.wred_granularity_get = dnx_data_ingr_congestion_info_wred_granularity_get;
    dnx_data_ingr_congestion.info.nof_pds_in_pdb_get = dnx_data_ingr_congestion_info_nof_pds_in_pdb_get;
    dnx_data_ingr_congestion.info.max_jumbo_packet_size_get = dnx_data_ingr_congestion_info_max_jumbo_packet_size_get;
    dnx_data_ingr_congestion.info.max_sram_pdbs_get = dnx_data_ingr_congestion_info_max_sram_pdbs_get;
    dnx_data_ingr_congestion.info.max_dram_bdbs_get = dnx_data_ingr_congestion_info_max_dram_bdbs_get;
    dnx_data_ingr_congestion.info.max_total_bytes_get = dnx_data_ingr_congestion_info_max_total_bytes_get;
    dnx_data_ingr_congestion.info.nof_active_drams_get = dnx_data_ingr_congestion_info_nof_active_drams_get;
    dnx_data_ingr_congestion.info.total_bytes_get = dnx_data_ingr_congestion_info_total_bytes_get;

    
    dnx_data_ingr_congestion.info.resource_get = dnx_data_ingr_congestion_info_resource_get;
    dnx_data_ingr_congestion.info.resource_info_get = dnx_data_ingr_congestion_info_resource_info_get;
    dnx_data_ingr_congestion.info.dp_free_res_presentage_drop_get = dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_get;
    dnx_data_ingr_congestion.info.dp_free_res_presentage_drop_info_get = dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_info_get;
    dnx_data_ingr_congestion.info.admission_preferences_get = dnx_data_ingr_congestion_info_admission_preferences_get;
    dnx_data_ingr_congestion.info.admission_preferences_info_get = dnx_data_ingr_congestion_info_admission_preferences_info_get;
    
    
    dnx_data_ingr_congestion.fadt_tail_drop.feature_get = dnx_data_ingr_congestion_fadt_tail_drop_feature_get;

    
    dnx_data_ingr_congestion.fadt_tail_drop.default_max_size_byte_threshold_for_ocb_only_get = dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_get;

    
    
    
    dnx_data_ingr_congestion.dram_bound.feature_get = dnx_data_ingr_congestion_dram_bound_feature_get;

    
    dnx_data_ingr_congestion.dram_bound.fadt_alpha_min_get = dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_get;
    dnx_data_ingr_congestion.dram_bound.fadt_alpha_max_get = dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_get;

    
    dnx_data_ingr_congestion.dram_bound.resource_get = dnx_data_ingr_congestion_dram_bound_resource_get;
    dnx_data_ingr_congestion.dram_bound.resource_info_get = dnx_data_ingr_congestion_dram_bound_resource_info_get;
    
    
    dnx_data_ingr_congestion.voq.feature_get = dnx_data_ingr_congestion_voq_feature_get;

    
    dnx_data_ingr_congestion.voq.nof_rate_class_get = dnx_data_ingr_congestion_voq_nof_rate_class_get;
    dnx_data_ingr_congestion.voq.rate_class_nof_bits_get = dnx_data_ingr_congestion_voq_rate_class_nof_bits_get;
    dnx_data_ingr_congestion.voq.nof_compensation_profiles_get = dnx_data_ingr_congestion_voq_nof_compensation_profiles_get;
    dnx_data_ingr_congestion.voq.default_compensation_get = dnx_data_ingr_congestion_voq_default_compensation_get;
    dnx_data_ingr_congestion.voq.voq_congestion_notification_fifo_size_get = dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_get;

    
    
    
    dnx_data_ingr_congestion.vsq.feature_get = dnx_data_ingr_congestion_vsq_feature_get;

    
    dnx_data_ingr_congestion.vsq.vsq_rate_class_nof_get = dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_a_rate_class_nof_get = dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_a_nof_get = dnx_data_ingr_congestion_vsq_vsq_a_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_b_nof_get = dnx_data_ingr_congestion_vsq_vsq_b_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_c_nof_get = dnx_data_ingr_congestion_vsq_vsq_c_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_d_nof_get = dnx_data_ingr_congestion_vsq_vsq_d_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_e_nof_get = dnx_data_ingr_congestion_vsq_vsq_e_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get = dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_f_nof_get = dnx_data_ingr_congestion_vsq_vsq_f_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get = dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_get;
    dnx_data_ingr_congestion.vsq.pool_nof_get = dnx_data_ingr_congestion_vsq_pool_nof_get;
    dnx_data_ingr_congestion.vsq.connection_class_nof_get = dnx_data_ingr_congestion_vsq_connection_class_nof_get;
    dnx_data_ingr_congestion.vsq.tc_pg_profile_nof_get = dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_get;
    dnx_data_ingr_congestion.vsq.vsq_e_congestion_notification_fifo_size_get = dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_get;
    dnx_data_ingr_congestion.vsq.vsq_f_congestion_notification_fifo_size_get = dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_get;
    dnx_data_ingr_congestion.vsq.oversubscription_bubble_width_get = dnx_data_ingr_congestion_vsq_oversubscription_bubble_width_get;
    dnx_data_ingr_congestion.vsq.vsq_e_default_get = dnx_data_ingr_congestion_vsq_vsq_e_default_get;
    dnx_data_ingr_congestion.vsq.vsq_f_default_get = dnx_data_ingr_congestion_vsq_vsq_f_default_get;

    
    dnx_data_ingr_congestion.vsq.info_get = dnx_data_ingr_congestion_vsq_info_get;
    dnx_data_ingr_congestion.vsq.info_info_get = dnx_data_ingr_congestion_vsq_info_info_get;
    
    
    dnx_data_ingr_congestion.init.feature_get = dnx_data_ingr_congestion_init_feature_get;

    
    dnx_data_ingr_congestion.init.fifo_size_get = dnx_data_ingr_congestion_init_fifo_size_get;

    
    dnx_data_ingr_congestion.init.vsq_words_rjct_map_get = dnx_data_ingr_congestion_init_vsq_words_rjct_map_get;
    dnx_data_ingr_congestion.init.vsq_words_rjct_map_info_get = dnx_data_ingr_congestion_init_vsq_words_rjct_map_info_get;
    dnx_data_ingr_congestion.init.vsq_sram_rjct_map_get = dnx_data_ingr_congestion_init_vsq_sram_rjct_map_get;
    dnx_data_ingr_congestion.init.vsq_sram_rjct_map_info_get = dnx_data_ingr_congestion_init_vsq_sram_rjct_map_info_get;
    dnx_data_ingr_congestion.init.dp_global_sram_buffer_drop_get = dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_get;
    dnx_data_ingr_congestion.init.dp_global_sram_buffer_drop_info_get = dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_info_get;
    dnx_data_ingr_congestion.init.dp_global_sram_pdb_drop_get = dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_get;
    dnx_data_ingr_congestion.init.dp_global_sram_pdb_drop_info_get = dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_info_get;
    dnx_data_ingr_congestion.init.dp_global_dram_bdb_drop_get = dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_get;
    dnx_data_ingr_congestion.init.dp_global_dram_bdb_drop_info_get = dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_info_get;
    dnx_data_ingr_congestion.init.equivalent_global_drop_get = dnx_data_ingr_congestion_init_equivalent_global_drop_get;
    dnx_data_ingr_congestion.init.equivalent_global_drop_info_get = dnx_data_ingr_congestion_init_equivalent_global_drop_info_get;
    
    
    dnx_data_ingr_congestion.dbal.feature_get = dnx_data_ingr_congestion_dbal_feature_get;

    
    dnx_data_ingr_congestion.dbal.admission_test_nof_get = dnx_data_ingr_congestion_dbal_admission_test_nof_get;
    dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get = dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get = dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.dram_bdbs_nof_bits_get = dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_pdbs_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffer_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sqm_debug_pkt_ctr_nof_bits_get = dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffer_per_pool_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_pds_per_pool_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_pds_per_pool_nof_bits_get;
    dnx_data_ingr_congestion.dbal.compensation_delta_nof_bits_get = dnx_data_ingr_congestion_dbal_compensation_delta_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffer_size_voq_occupancy_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffer_size_voq_occupancy_nof_bits_get;
    dnx_data_ingr_congestion.dbal.size_in_words_sram_voq_occupancy_nof_bits_get = dnx_data_ingr_congestion_dbal_size_in_words_sram_voq_occupancy_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_pds_size_occupancy_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_pds_size_occupancy_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffer_size_vsq_occupancy_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffer_size_vsq_occupancy_nof_bits_get;
    dnx_data_ingr_congestion.dbal.size_in_words_occupancy_nof_bits_get = dnx_data_ingr_congestion_dbal_size_in_words_occupancy_nof_bits_get;
    dnx_data_ingr_congestion.dbal.dram_bounds_sram_words_th_nof_bits_get = dnx_data_ingr_congestion_dbal_dram_bounds_sram_words_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_buffers_vsq_th_nof_bits_get;
    dnx_data_ingr_congestion.dbal.sram_vsq_pds_th_nof_bits_get = dnx_data_ingr_congestion_dbal_sram_vsq_pds_th_nof_bits_get;

    
    dnx_data_ingr_congestion.dbal.admission_bits_mapping_get = dnx_data_ingr_congestion_dbal_admission_bits_mapping_get;
    dnx_data_ingr_congestion.dbal.admission_bits_mapping_info_get = dnx_data_ingr_congestion_dbal_admission_bits_mapping_info_get;
    
    
    dnx_data_ingr_congestion.mirror_on_drop.feature_get = dnx_data_ingr_congestion_mirror_on_drop_feature_get;

    
    dnx_data_ingr_congestion.mirror_on_drop.nof_groups_get = dnx_data_ingr_congestion_mirror_on_drop_nof_groups_get;
    dnx_data_ingr_congestion.mirror_on_drop.aging_clocks_resolution_get = dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_get;

    
    
    
    dnx_data_ingr_congestion.sram_buffer.feature_get = dnx_data_ingr_congestion_sram_buffer_feature_get;

    
    dnx_data_ingr_congestion.sram_buffer.nof_drop_profiles_get = dnx_data_ingr_congestion_sram_buffer_nof_drop_profiles_get;
    dnx_data_ingr_congestion.sram_buffer.nof_drop_profiles_bits_get = dnx_data_ingr_congestion_sram_buffer_nof_drop_profiles_bits_get;

    
    dnx_data_ingr_congestion.sram_buffer.drop_tresholds_get = dnx_data_ingr_congestion_sram_buffer_drop_tresholds_get;
    dnx_data_ingr_congestion.sram_buffer.drop_tresholds_info_get = dnx_data_ingr_congestion_sram_buffer_drop_tresholds_info_get;
    
    
    
    dnx_data_ingr_reassembly.general.feature_get = dnx_data_ingr_reassembly_general_feature_get;

    

    
    
    
    dnx_data_ingr_reassembly.context.feature_get = dnx_data_ingr_reassembly_context_feature_get;

    
    dnx_data_ingr_reassembly.context.nof_contexts_get = dnx_data_ingr_reassembly_context_nof_contexts_get;
    dnx_data_ingr_reassembly.context.invalid_context_get = dnx_data_ingr_reassembly_context_invalid_context_get;
    dnx_data_ingr_reassembly.context.nof_cpu_channels_per_core_get = dnx_data_ingr_reassembly_context_nof_cpu_channels_per_core_get;
    dnx_data_ingr_reassembly.context.reassembly_timeout_get = dnx_data_ingr_reassembly_context_reassembly_timeout_get;
    dnx_data_ingr_reassembly.context.rcy_unique_context_get = dnx_data_ingr_reassembly_context_rcy_unique_context_get;

    
    dnx_data_ingr_reassembly.context.context_map_get = dnx_data_ingr_reassembly_context_context_map_get;
    dnx_data_ingr_reassembly.context.context_map_info_get = dnx_data_ingr_reassembly_context_context_map_info_get;
    
    
    dnx_data_ingr_reassembly.ilkn.feature_get = dnx_data_ingr_reassembly_ilkn_feature_get;

    

    
    
    
    dnx_data_ingr_reassembly.priority.feature_get = dnx_data_ingr_reassembly_priority_feature_get;

    
    dnx_data_ingr_reassembly.priority.nif_eth_priorities_nof_get = dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get;
    dnx_data_ingr_reassembly.priority.mirror_priorities_nof_get = dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get;
    dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get = dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get;
    dnx_data_ingr_reassembly.priority.priorities_nof_get = dnx_data_ingr_reassembly_priority_priorities_nof_get;
    dnx_data_ingr_reassembly.priority.cgm_priorities_nof_get = dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get;

    
    
    
    dnx_data_ingr_reassembly.dbal.feature_get = dnx_data_ingr_reassembly_dbal_feature_get;

    
    dnx_data_ingr_reassembly.dbal.reassembly_context_bits_get = dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get;
    dnx_data_ingr_reassembly.dbal.interface_bits_get = dnx_data_ingr_reassembly_dbal_interface_bits_get;
    dnx_data_ingr_reassembly.dbal.interface_max_get = dnx_data_ingr_reassembly_dbal_interface_max_get;
    dnx_data_ingr_reassembly.dbal.context_map_base_address_bits_get = dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get;
    dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get = dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get;
    dnx_data_ingr_reassembly.dbal.context_map_base_offest_ext_max_get = dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get;
    dnx_data_ingr_reassembly.dbal.interleaved_interface_bits_get = dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get;
    dnx_data_ingr_reassembly.dbal.interleaved_interface_max_get = dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get;

    
    
    
    
    dnx_data_ingress_cs.properties.feature_get = dnx_data_ingress_cs_properties_feature_get;

    
    dnx_data_ingress_cs.properties.cs_buffer_nof_bits_max_get = dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get;
    dnx_data_ingress_cs.properties.nof_bits_entry_size_prefix_get = dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get;

    
    dnx_data_ingress_cs.properties.per_stage_get = dnx_data_ingress_cs_properties_per_stage_get;
    dnx_data_ingress_cs.properties.per_stage_info_get = dnx_data_ingress_cs_properties_per_stage_info_get;
    
    
    dnx_data_ingress_cs.features.feature_get = dnx_data_ingress_cs_features_feature_get;

    

    
    
    
    dnx_data_ingress_cs.parser.feature_get = dnx_data_ingress_cs_parser_feature_get;

    

    
    
    
    
    dnx_data_instru.ipt.feature_get = dnx_data_instru_ipt_feature_get;

    
    dnx_data_instru.ipt.profile_size_get = dnx_data_instru_ipt_profile_size_get;
    dnx_data_instru.ipt.metadata_bitmap_size_get = dnx_data_instru_ipt_metadata_bitmap_size_get;
    dnx_data_instru.ipt.metadata_edit_size_get = dnx_data_instru_ipt_metadata_edit_size_get;
    dnx_data_instru.ipt.node_id_padding_size_get = dnx_data_instru_ipt_node_id_padding_size_get;
    dnx_data_instru.ipt.telemetry_queue_occ_check_get = dnx_data_instru_ipt_telemetry_queue_occ_check_get;
    dnx_data_instru.ipt.telemetry_queue_occ_max_get = dnx_data_instru_ipt_telemetry_queue_occ_max_get;

    
    dnx_data_instru.ipt.profile_info_get = dnx_data_instru_ipt_profile_info_get;
    dnx_data_instru.ipt.profile_info_info_get = dnx_data_instru_ipt_profile_info_info_get;
    dnx_data_instru.ipt.metadata_get = dnx_data_instru_ipt_metadata_get;
    dnx_data_instru.ipt.metadata_info_get = dnx_data_instru_ipt_metadata_info_get;
    
    
    dnx_data_instru.sflow.feature_get = dnx_data_instru_sflow_feature_get;

    
    dnx_data_instru.sflow.max_nof_sflow_encaps_get = dnx_data_instru_sflow_max_nof_sflow_encaps_get;
    dnx_data_instru.sflow.nof_sflow_raw_entries_per_stack_get = dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_get;

    
    
    
    dnx_data_instru.ifa.feature_get = dnx_data_instru_ifa_feature_get;

    
    dnx_data_instru.ifa.ingress_tod_feature_get = dnx_data_instru_ifa_ingress_tod_feature_get;
    dnx_data_instru.ifa.max_lenght_check_for_ifa2_get = dnx_data_instru_ifa_max_lenght_check_for_ifa2_get;
    dnx_data_instru.ifa.ifa2_is_in_hl_get = dnx_data_instru_ifa_ifa2_is_in_hl_get;
    dnx_data_instru.ifa.ifa2_is_supported_get = dnx_data_instru_ifa_ifa2_is_supported_get;

    
    
    
    dnx_data_instru.ipfix.feature_get = dnx_data_instru_ipfix_feature_get;

    
    dnx_data_instru.ipfix.ipfix_v2_get = dnx_data_instru_ipfix_ipfix_v2_get;

    
    
    
    dnx_data_instru.hbh_pt.feature_get = dnx_data_instru_hbh_pt_feature_get;

    
    dnx_data_instru.hbh_pt.Ingress_1588_ToD_LSB_size_get = dnx_data_instru_hbh_pt_Ingress_1588_ToD_LSB_size_get;

    
    
    
    dnx_data_instru.eventor.feature_get = dnx_data_instru_eventor_feature_get;

    
    dnx_data_instru.eventor.rx_evpck_header_size_get = dnx_data_instru_eventor_rx_evpck_header_size_get;
    dnx_data_instru.eventor.tx_builder_max_header_size_get = dnx_data_instru_eventor_tx_builder_max_header_size_get;
    dnx_data_instru.eventor.nof_contexts_get = dnx_data_instru_eventor_nof_contexts_get;
    dnx_data_instru.eventor.nof_contexts_bits_get = dnx_data_instru_eventor_nof_contexts_bits_get;
    dnx_data_instru.eventor.nof_builders_get = dnx_data_instru_eventor_nof_builders_get;
    dnx_data_instru.eventor.nof_builders_bits_get = dnx_data_instru_eventor_nof_builders_bits_get;
    dnx_data_instru.eventor.nof_sram_banks_get = dnx_data_instru_eventor_nof_sram_banks_get;
    dnx_data_instru.eventor.sram_bank_bits_get = dnx_data_instru_eventor_sram_bank_bits_get;
    dnx_data_instru.eventor.sram_banks_full_size_get = dnx_data_instru_eventor_sram_banks_full_size_get;
    dnx_data_instru.eventor.sram_banks_net_size_get = dnx_data_instru_eventor_sram_banks_net_size_get;
    dnx_data_instru.eventor.builder_max_buffer_size_get = dnx_data_instru_eventor_builder_max_buffer_size_get;
    dnx_data_instru.eventor.rx_max_buffer_size_get = dnx_data_instru_eventor_rx_max_buffer_size_get;
    dnx_data_instru.eventor.axi_sram_offset_get = dnx_data_instru_eventor_axi_sram_offset_get;
    dnx_data_instru.eventor.is_extra_words_copy_get = dnx_data_instru_eventor_is_extra_words_copy_get;
    dnx_data_instru.eventor.is_rx_en_get = dnx_data_instru_eventor_is_rx_en_get;
    dnx_data_instru.eventor.tx_builder_limitation_1_get = dnx_data_instru_eventor_tx_builder_limitation_1_get;
    dnx_data_instru.eventor.builder_max_timeout_ms_get = dnx_data_instru_eventor_builder_max_timeout_ms_get;
    dnx_data_instru.eventor.sram_bank_mask_get = dnx_data_instru_eventor_sram_bank_mask_get;

    
    
    
    dnx_data_instru.synced_triggers.feature_get = dnx_data_instru_synced_triggers_feature_get;

    
    dnx_data_instru.synced_triggers.synced_trigger_version_get = dnx_data_instru_synced_triggers_synced_trigger_version_get;

    
    
    
    dnx_data_instru.synced_counters.feature_get = dnx_data_instru_synced_counters_feature_get;

    
    dnx_data_instru.synced_counters.icgm_type_get = dnx_data_instru_synced_counters_icgm_type_get;
    dnx_data_instru.synced_counters.nif_type_get = dnx_data_instru_synced_counters_nif_type_get;
    dnx_data_instru.synced_counters.nof_types_get = dnx_data_instru_synced_counters_nof_types_get;
    dnx_data_instru.synced_counters.period_adjusted_to_ns_through_time_ratio_get = dnx_data_instru_synced_counters_period_adjusted_to_ns_through_time_ratio_get;
    dnx_data_instru.synced_counters.period_adjusting_clk_bits_get = dnx_data_instru_synced_counters_period_adjusting_clk_bits_get;
    dnx_data_instru.synced_counters.period_adjusting_ns_bits_get = dnx_data_instru_synced_counters_period_adjusting_ns_bits_get;
    dnx_data_instru.synced_counters.interval_period_size_hw_get = dnx_data_instru_synced_counters_interval_period_size_hw_get;
    dnx_data_instru.synced_counters.max_interval_period_size_get = dnx_data_instru_synced_counters_max_interval_period_size_get;
    dnx_data_instru.synced_counters.cgm_max_nof_intervals_get = dnx_data_instru_synced_counters_cgm_max_nof_intervals_get;
    dnx_data_instru.synced_counters.nif_max_nof_intervals_get = dnx_data_instru_synced_counters_nif_max_nof_intervals_get;
    dnx_data_instru.synced_counters.cgm_write_counter_wrap_around_get = dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_get;
    dnx_data_instru.synced_counters.nif_write_counter_wrap_around_get = dnx_data_instru_synced_counters_nif_write_counter_wrap_around_get;
    dnx_data_instru.synced_counters.bytes_in_word_get = dnx_data_instru_synced_counters_bytes_in_word_get;
    dnx_data_instru.synced_counters.nof_pms_for_pfc_get = dnx_data_instru_synced_counters_nof_pms_for_pfc_get;
    dnx_data_instru.synced_counters.nof_ports_for_pfc_get = dnx_data_instru_synced_counters_nof_ports_for_pfc_get;
    dnx_data_instru.synced_counters.nof_tcs_per_port_get = dnx_data_instru_synced_counters_nof_tcs_per_port_get;

    
    dnx_data_instru.synced_counters.counters_info_get = dnx_data_instru_synced_counters_counters_info_get;
    dnx_data_instru.synced_counters.counters_info_info_get = dnx_data_instru_synced_counters_counters_info_info_get;
    
    
    dnx_data_instru.alternate_marking.feature_get = dnx_data_instru_alternate_marking_feature_get;

    
    dnx_data_instru.alternate_marking.nof_L_bit_period_profile_get = dnx_data_instru_alternate_marking_nof_L_bit_period_profile_get;

    
    
    
    
    dnx_data_intr.general.feature_get = dnx_data_intr_general_feature_get;

    
    dnx_data_intr.general.nof_interrupts_get = dnx_data_intr_general_nof_interrupts_get;
    dnx_data_intr.general.polled_irq_mode_get = dnx_data_intr_general_polled_irq_mode_get;
    dnx_data_intr.general.polled_irq_priority_get = dnx_data_intr_general_polled_irq_priority_get;
    dnx_data_intr.general.polled_irq_delay_get = dnx_data_intr_general_polled_irq_delay_get;
    dnx_data_intr.general.epni_ebd_interrupt_assert_enable_get = dnx_data_intr_general_epni_ebd_interrupt_assert_enable_get;
    dnx_data_intr.general.table_dma_enable_get = dnx_data_intr_general_table_dma_enable_get;
    dnx_data_intr.general.tdma_timeout_usec_get = dnx_data_intr_general_tdma_timeout_usec_get;
    dnx_data_intr.general.tdma_intr_enable_get = dnx_data_intr_general_tdma_intr_enable_get;
    dnx_data_intr.general.tslam_dma_enable_get = dnx_data_intr_general_tslam_dma_enable_get;
    dnx_data_intr.general.tslam_timeout_usec_get = dnx_data_intr_general_tslam_timeout_usec_get;
    dnx_data_intr.general.tslam_intr_enable_get = dnx_data_intr_general_tslam_intr_enable_get;
    dnx_data_intr.general.schan_timeout_usec_get = dnx_data_intr_general_schan_timeout_usec_get;
    dnx_data_intr.general.schan_intr_enable_get = dnx_data_intr_general_schan_intr_enable_get;
    dnx_data_intr.general.miim_timeout_usec_get = dnx_data_intr_general_miim_timeout_usec_get;
    dnx_data_intr.general.miim_intr_enable_get = dnx_data_intr_general_miim_intr_enable_get;
    dnx_data_intr.general.sbus_dma_interval_get = dnx_data_intr_general_sbus_dma_interval_get;
    dnx_data_intr.general.sbus_dma_intr_enable_get = dnx_data_intr_general_sbus_dma_intr_enable_get;
    dnx_data_intr.general.mem_clear_chunk_size_get = dnx_data_intr_general_mem_clear_chunk_size_get;
    dnx_data_intr.general.ser_event_generate_support_get = dnx_data_intr_general_ser_event_generate_support_get;
    dnx_data_intr.general.ser_reset_cb_en_support_get = dnx_data_intr_general_ser_reset_cb_en_support_get;

    
    dnx_data_intr.general.active_on_init_get = dnx_data_intr_general_active_on_init_get;
    dnx_data_intr.general.active_on_init_info_get = dnx_data_intr_general_active_on_init_info_get;
    dnx_data_intr.general.disable_on_init_get = dnx_data_intr_general_disable_on_init_get;
    dnx_data_intr.general.disable_on_init_info_get = dnx_data_intr_general_disable_on_init_info_get;
    dnx_data_intr.general.disable_print_on_init_get = dnx_data_intr_general_disable_print_on_init_get;
    dnx_data_intr.general.disable_print_on_init_info_get = dnx_data_intr_general_disable_print_on_init_info_get;
    dnx_data_intr.general.block_intr_field_mapping_get = dnx_data_intr_general_block_intr_field_mapping_get;
    dnx_data_intr.general.block_intr_field_mapping_info_get = dnx_data_intr_general_block_intr_field_mapping_info_get;
    dnx_data_intr.general.skip_clear_on_init_get = dnx_data_intr_general_skip_clear_on_init_get;
    dnx_data_intr.general.skip_clear_on_init_info_get = dnx_data_intr_general_skip_clear_on_init_info_get;
    dnx_data_intr.general.force_unmask_on_init_get = dnx_data_intr_general_force_unmask_on_init_get;
    dnx_data_intr.general.force_unmask_on_init_info_get = dnx_data_intr_general_force_unmask_on_init_info_get;
    dnx_data_intr.general.skip_clear_on_regress_get = dnx_data_intr_general_skip_clear_on_regress_get;
    dnx_data_intr.general.skip_clear_on_regress_info_get = dnx_data_intr_general_skip_clear_on_regress_info_get;
    dnx_data_intr.general.framer_module_map_get = dnx_data_intr_general_framer_module_map_get;
    dnx_data_intr.general.framer_module_map_info_get = dnx_data_intr_general_framer_module_map_info_get;
    dnx_data_intr.general.chip_intr_remap_get = dnx_data_intr_general_chip_intr_remap_get;
    dnx_data_intr.general.chip_intr_remap_info_get = dnx_data_intr_general_chip_intr_remap_info_get;
    dnx_data_intr.general.chip_intr_enable_get = dnx_data_intr_general_chip_intr_enable_get;
    dnx_data_intr.general.chip_intr_enable_info_get = dnx_data_intr_general_chip_intr_enable_info_get;
    
    
    dnx_data_intr.ser.feature_get = dnx_data_intr_ser_feature_get;

    
    dnx_data_intr.ser.tcam_protection_issue_get = dnx_data_intr_ser_tcam_protection_issue_get;
    dnx_data_intr.ser.new_tcam_scan_mode_get = dnx_data_intr_ser_new_tcam_scan_mode_get;

    
    dnx_data_intr.ser.mem_mask_get = dnx_data_intr_ser_mem_mask_get;
    dnx_data_intr.ser.mem_mask_info_get = dnx_data_intr_ser_mem_mask_info_get;
    dnx_data_intr.ser.xor_mem_get = dnx_data_intr_ser_xor_mem_get;
    dnx_data_intr.ser.xor_mem_info_get = dnx_data_intr_ser_xor_mem_info_get;
    dnx_data_intr.ser.intr_exception_get = dnx_data_intr_ser_intr_exception_get;
    dnx_data_intr.ser.intr_exception_info_get = dnx_data_intr_ser_intr_exception_info_get;
    dnx_data_intr.ser.static_mem_get = dnx_data_intr_ser_static_mem_get;
    dnx_data_intr.ser.static_mem_info_get = dnx_data_intr_ser_static_mem_info_get;
    dnx_data_intr.ser.dynamic_mem_get = dnx_data_intr_ser_dynamic_mem_get;
    dnx_data_intr.ser.dynamic_mem_info_get = dnx_data_intr_ser_dynamic_mem_info_get;
    dnx_data_intr.ser.mdb_mem_map_get = dnx_data_intr_ser_mdb_mem_map_get;
    dnx_data_intr.ser.mdb_mem_map_info_get = dnx_data_intr_ser_mdb_mem_map_info_get;
    dnx_data_intr.ser.kaps_tcam_scan_get = dnx_data_intr_ser_kaps_tcam_scan_get;
    dnx_data_intr.ser.kaps_tcam_scan_info_get = dnx_data_intr_ser_kaps_tcam_scan_info_get;
    dnx_data_intr.ser.severity_mem_get = dnx_data_intr_ser_severity_mem_get;
    dnx_data_intr.ser.severity_mem_info_get = dnx_data_intr_ser_severity_mem_info_get;
    dnx_data_intr.ser.tcam_scan_get = dnx_data_intr_ser_tcam_scan_get;
    dnx_data_intr.ser.tcam_scan_info_get = dnx_data_intr_ser_tcam_scan_info_get;
    
    
    
    dnx_data_ipq.queues.feature_get = dnx_data_ipq_queues_feature_get;

    
    dnx_data_ipq.queues.nof_queues_get = dnx_data_ipq_queues_nof_queues_get;
    dnx_data_ipq.queues.min_bundle_size_get = dnx_data_ipq_queues_min_bundle_size_get;
    dnx_data_ipq.queues.fmq_bundle_size_get = dnx_data_ipq_queues_fmq_bundle_size_get;
    dnx_data_ipq.queues.max_fmq_id_get = dnx_data_ipq_queues_max_fmq_id_get;
    dnx_data_ipq.queues.queue_id_bits_get = dnx_data_ipq_queues_queue_id_bits_get;
    dnx_data_ipq.queues.voq_offset_bits_get = dnx_data_ipq_queues_voq_offset_bits_get;
    dnx_data_ipq.queues.min_bundle_size_bits_get = dnx_data_ipq_queues_min_bundle_size_bits_get;
    dnx_data_ipq.queues.tc_profile_from_tm_command_nof_bits_get = dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_get;
    dnx_data_ipq.queues.flow_quartet_nof_bits_get = dnx_data_ipq_queues_flow_quartet_nof_bits_get;
    dnx_data_ipq.queues.invalid_queue_get = dnx_data_ipq_queues_invalid_queue_get;

    
    
    
    dnx_data_ipq.regions.feature_get = dnx_data_ipq_regions_feature_get;

    
    dnx_data_ipq.regions.nof_queues_get = dnx_data_ipq_regions_nof_queues_get;

    
    
    
    dnx_data_ipq.tc_map.feature_get = dnx_data_ipq_tc_map_feature_get;

    
    dnx_data_ipq.tc_map.nof_voq_flow_profiles_get = dnx_data_ipq_tc_map_nof_voq_flow_profiles_get;
    dnx_data_ipq.tc_map.voq_flow_profiles_bits_get = dnx_data_ipq_tc_map_voq_flow_profiles_bits_get;
    dnx_data_ipq.tc_map.default_voq_flow_profile_idx_get = dnx_data_ipq_tc_map_default_voq_flow_profile_idx_get;
    dnx_data_ipq.tc_map.nof_sysport_profiles_get = dnx_data_ipq_tc_map_nof_sysport_profiles_get;
    dnx_data_ipq.tc_map.default_sysport_profile_idx_get = dnx_data_ipq_tc_map_default_sysport_profile_idx_get;
    dnx_data_ipq.tc_map.voq_resolution_type_max_value_get = dnx_data_ipq_tc_map_voq_resolution_type_max_value_get;

    
    
    
    dnx_data_ipq.general.feature_get = dnx_data_ipq_general_feature_get;

    

    
    
    
    
    dnx_data_iqs.credit.feature_get = dnx_data_iqs_credit_feature_get;

    
    dnx_data_iqs.credit.max_credit_balance_threshold_get = dnx_data_iqs_credit_max_credit_balance_threshold_get;
    dnx_data_iqs.credit.max_worth_get = dnx_data_iqs_credit_max_worth_get;
    dnx_data_iqs.credit.satisified_back_min_get = dnx_data_iqs_credit_satisified_back_min_get;
    dnx_data_iqs.credit.satisified_back_max_get = dnx_data_iqs_credit_satisified_back_max_get;
    dnx_data_iqs.credit.satisified_empty_min_get = dnx_data_iqs_credit_satisified_empty_min_get;
    dnx_data_iqs.credit.satisified_empty_max_get = dnx_data_iqs_credit_satisified_empty_max_get;
    dnx_data_iqs.credit.hungry_min_get = dnx_data_iqs_credit_hungry_min_get;
    dnx_data_iqs.credit.hungry_max_get = dnx_data_iqs_credit_hungry_max_get;
    dnx_data_iqs.credit.hungry_mult_min_get = dnx_data_iqs_credit_hungry_mult_min_get;
    dnx_data_iqs.credit.hungry_mult_max_get = dnx_data_iqs_credit_hungry_mult_max_get;
    dnx_data_iqs.credit.nof_bw_levels_get = dnx_data_iqs_credit_nof_bw_levels_get;
    dnx_data_iqs.credit.fmq_credit_fc_on_th_get = dnx_data_iqs_credit_fmq_credit_fc_on_th_get;
    dnx_data_iqs.credit.fmq_credit_fc_off_th_get = dnx_data_iqs_credit_fmq_credit_fc_off_th_get;
    dnx_data_iqs.credit.fmq_byte_fc_on_th_get = dnx_data_iqs_credit_fmq_byte_fc_on_th_get;
    dnx_data_iqs.credit.fmq_byte_fc_off_th_get = dnx_data_iqs_credit_fmq_byte_fc_off_th_get;
    dnx_data_iqs.credit.fmq_eir_credit_fc_th_get = dnx_data_iqs_credit_fmq_eir_credit_fc_th_get;
    dnx_data_iqs.credit.fmq_eir_byte_fc_th_get = dnx_data_iqs_credit_fmq_eir_byte_fc_th_get;
    dnx_data_iqs.credit.fmq_nof_be_classes_get = dnx_data_iqs_credit_fmq_nof_be_classes_get;
    dnx_data_iqs.credit.fmq_max_be_weight_get = dnx_data_iqs_credit_fmq_max_be_weight_get;
    dnx_data_iqs.credit.fmq_max_burst_max_get = dnx_data_iqs_credit_fmq_max_burst_max_get;
    dnx_data_iqs.credit.fmq_shp_crdt_rate_mltp_get = dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_get;
    dnx_data_iqs.credit.fmq_shp_crdt_rate_delta_get = dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_get;
    dnx_data_iqs.credit.max_rate_profile_preset_gbps_get = dnx_data_iqs_credit_max_rate_profile_preset_gbps_get;
    dnx_data_iqs.credit.nof_profiles_get = dnx_data_iqs_credit_nof_profiles_get;
    dnx_data_iqs.credit.profile_nof_bits_get = dnx_data_iqs_credit_profile_nof_bits_get;
    dnx_data_iqs.credit.worth_get = dnx_data_iqs_credit_worth_get;
    dnx_data_iqs.credit.fmq_shp_rate_max_get = dnx_data_iqs_credit_fmq_shp_rate_max_get;

    
    
    
    dnx_data_iqs.deq_default.feature_get = dnx_data_iqs_deq_default_feature_get;

    
    dnx_data_iqs.deq_default.low_delay_deq_bytes_get = dnx_data_iqs_deq_default_low_delay_deq_bytes_get;
    dnx_data_iqs.deq_default.credit_balance_max_get = dnx_data_iqs_deq_default_credit_balance_max_get;
    dnx_data_iqs.deq_default.credit_balance_resolution_get = dnx_data_iqs_deq_default_credit_balance_resolution_get;
    dnx_data_iqs.deq_default.s2d_credit_balance_max_get = dnx_data_iqs_deq_default_s2d_credit_balance_max_get;
    dnx_data_iqs.deq_default.s2d_credit_balance_resolution_get = dnx_data_iqs_deq_default_s2d_credit_balance_resolution_get;
    dnx_data_iqs.deq_default.sram_to_fabric_credit_lfsr_thr_get = dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_get;
    dnx_data_iqs.deq_default.sram_to_fabric_credit_lfsr_mask_get = dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_get;

    
    dnx_data_iqs.deq_default.sqm_read_weight_profiles_get = dnx_data_iqs_deq_default_sqm_read_weight_profiles_get;
    dnx_data_iqs.deq_default.sqm_read_weight_profiles_info_get = dnx_data_iqs_deq_default_sqm_read_weight_profiles_info_get;
    dnx_data_iqs.deq_default.dqm_read_weight_profiles_get = dnx_data_iqs_deq_default_dqm_read_weight_profiles_get;
    dnx_data_iqs.deq_default.dqm_read_weight_profiles_info_get = dnx_data_iqs_deq_default_dqm_read_weight_profiles_info_get;
    dnx_data_iqs.deq_default.params_get = dnx_data_iqs_deq_default_params_get;
    dnx_data_iqs.deq_default.params_info_get = dnx_data_iqs_deq_default_params_info_get;
    dnx_data_iqs.deq_default.dram_params_get = dnx_data_iqs_deq_default_dram_params_get;
    dnx_data_iqs.deq_default.dram_params_info_get = dnx_data_iqs_deq_default_dram_params_info_get;
    dnx_data_iqs.deq_default.max_deq_cmd_get = dnx_data_iqs_deq_default_max_deq_cmd_get;
    dnx_data_iqs.deq_default.max_deq_cmd_info_get = dnx_data_iqs_deq_default_max_deq_cmd_info_get;
    dnx_data_iqs.deq_default.s2d_max_deq_cmd_get = dnx_data_iqs_deq_default_s2d_max_deq_cmd_get;
    dnx_data_iqs.deq_default.s2d_max_deq_cmd_info_get = dnx_data_iqs_deq_default_s2d_max_deq_cmd_info_get;
    
    
    dnx_data_iqs.dqcq.feature_get = dnx_data_iqs_dqcq_feature_get;

    
    dnx_data_iqs.dqcq.max_nof_contexts_get = dnx_data_iqs_dqcq_max_nof_contexts_get;
    dnx_data_iqs.dqcq.nof_priorities_get = dnx_data_iqs_dqcq_nof_priorities_get;

    
    
    
    dnx_data_iqs.dbal.feature_get = dnx_data_iqs_dbal_feature_get;

    
    dnx_data_iqs.dbal.ipt_counters_nof_bits_get = dnx_data_iqs_dbal_ipt_counters_nof_bits_get;
    dnx_data_iqs.dbal.credit_worth_nof_bits_get = dnx_data_iqs_dbal_credit_worth_nof_bits_get;
    dnx_data_iqs.dbal.fsm_sn_mask_get = dnx_data_iqs_dbal_fsm_sn_mask_get;

    
    
    
    dnx_data_iqs.general.feature_get = dnx_data_iqs_general_feature_get;

    

    
    
    
    dnx_data_iqs.flush.feature_get = dnx_data_iqs_flush_feature_get;

    
    dnx_data_iqs.flush.bytes_units_to_send_get = dnx_data_iqs_flush_bytes_units_to_send_get;

    
    
    
    
    dnx_data_kaps.flist.feature_get = dnx_data_kaps_flist_feature_get;

    

    
    
    
    
    dnx_data_kleap_stage_info.kleap_stage_info.feature_get = dnx_data_kleap_stage_info_kleap_stage_info_feature_get;

    
    dnx_data_kleap_stage_info.kleap_stage_info.nof_kleap_stages_get = dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_get;

    
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_info_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_info_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_kbr_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_kbr_info_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_info_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_pd_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_pd_info_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_info_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_get;
    dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_sub_stage_info_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_info_get;
    
    
    
    dnx_data_l2.general.feature_get = dnx_data_l2_general_feature_get;

    
    dnx_data_l2.general.vsi_offset_shift_get = dnx_data_l2_general_vsi_offset_shift_get;
    dnx_data_l2.general.lif_offset_shift_get = dnx_data_l2_general_lif_offset_shift_get;
    dnx_data_l2.general.lem_nof_dbs_get = dnx_data_l2_general_lem_nof_dbs_get;
    dnx_data_l2.general.l2_learn_limit_mode_get = dnx_data_l2_general_l2_learn_limit_mode_get;
    dnx_data_l2.general.jr_mode_nof_fec_bits_get = dnx_data_l2_general_jr_mode_nof_fec_bits_get;
    dnx_data_l2.general.arad_plus_mode_nof_fec_bits_get = dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get;
    dnx_data_l2.general.l2_egress_max_extention_size_bytes_get = dnx_data_l2_general_l2_egress_max_extention_size_bytes_get;
    dnx_data_l2.general.l2_egress_max_additional_termination_size_bytes_get = dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get;
    dnx_data_l2.general.mact_mngmnt_fid_exceed_limit_int_get = dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_get;
    dnx_data_l2.general.mact_lela_fid_exceed_limit_int_get = dnx_data_l2_general_mact_lela_fid_exceed_limit_int_get;
    dnx_data_l2.general.l2_api_supported_flags2_get = dnx_data_l2_general_l2_api_supported_flags2_get;
    dnx_data_l2.general.l2_traverse_api_supported_flags_get = dnx_data_l2_general_l2_traverse_api_supported_flags_get;
    dnx_data_l2.general.l2_delete_api_supported_flags_get = dnx_data_l2_general_l2_delete_api_supported_flags_get;
    dnx_data_l2.general.l2_add_get_apis_supported_flags_get = dnx_data_l2_general_l2_add_get_apis_supported_flags_get;

    
    dnx_data_l2.general.mact_result_type_map_get = dnx_data_l2_general_mact_result_type_map_get;
    dnx_data_l2.general.mact_result_type_map_info_get = dnx_data_l2_general_mact_result_type_map_info_get;
    dnx_data_l2.general.scan_data_bases_info_get = dnx_data_l2_general_scan_data_bases_info_get;
    dnx_data_l2.general.scan_data_bases_info_info_get = dnx_data_l2_general_scan_data_bases_info_info_get;
    dnx_data_l2.general.fwd_mact_info_get = dnx_data_l2_general_fwd_mact_info_get;
    dnx_data_l2.general.fwd_mact_info_info_get = dnx_data_l2_general_fwd_mact_info_info_get;
    dnx_data_l2.general.lem_table_map_l2_action_get = dnx_data_l2_general_lem_table_map_l2_action_get;
    dnx_data_l2.general.lem_table_map_l2_action_info_get = dnx_data_l2_general_lem_table_map_l2_action_info_get;
    
    
    dnx_data_l2.feature.feature_get = dnx_data_l2_feature_feature_get;

    

    
    
    
    dnx_data_l2.vsi.feature_get = dnx_data_l2_vsi_feature_get;

    
    dnx_data_l2.vsi.nof_vsi_aging_profiles_get = dnx_data_l2_vsi_nof_vsi_aging_profiles_get;
    dnx_data_l2.vsi.nof_event_forwarding_profiles_get = dnx_data_l2_vsi_nof_event_forwarding_profiles_get;
    dnx_data_l2.vsi.nof_vsi_learning_profiles_get = dnx_data_l2_vsi_nof_vsi_learning_profiles_get;
    dnx_data_l2.vsi.vsi_table_get = dnx_data_l2_vsi_vsi_table_get;

    
    
    
    dnx_data_l2.vlan_domain.feature_get = dnx_data_l2_vlan_domain_feature_get;

    
    dnx_data_l2.vlan_domain.nof_vlan_domains_get = dnx_data_l2_vlan_domain_nof_vlan_domains_get;
    dnx_data_l2.vlan_domain.nof_bits_next_layer_network_domain_get = dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_get;

    
    
    
    dnx_data_l2.dma.feature_get = dnx_data_l2_dma_feature_get;

    
    dnx_data_l2.dma.flush_nof_dma_entries_get = dnx_data_l2_dma_flush_nof_dma_entries_get;
    dnx_data_l2.dma.flush_db_nof_dma_rules_get = dnx_data_l2_dma_flush_db_nof_dma_rules_get;
    dnx_data_l2.dma.flush_db_nof_dma_rules_per_table_get = dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_get;
    dnx_data_l2.dma.flush_db_rule_size_get = dnx_data_l2_dma_flush_db_rule_size_get;
    dnx_data_l2.dma.flush_db_data_size_get = dnx_data_l2_dma_flush_db_data_size_get;
    dnx_data_l2.dma.flush_group_size_get = dnx_data_l2_dma_flush_group_size_get;
    dnx_data_l2.dma.learning_fifo_dma_buffer_size_get = dnx_data_l2_dma_learning_fifo_dma_buffer_size_get;
    dnx_data_l2.dma.learning_fifo_dma_timeout_get = dnx_data_l2_dma_learning_fifo_dma_timeout_get;
    dnx_data_l2.dma.learning_fifo_dma_threshold_get = dnx_data_l2_dma_learning_fifo_dma_threshold_get;
    dnx_data_l2.dma.l2_dma_cpu_learn_thread_priority_get = dnx_data_l2_dma_l2_dma_cpu_learn_thread_priority_get;

    
    
    
    dnx_data_l2.age_and_flush_machine.feature_get = dnx_data_l2_age_and_flush_machine_feature_get;

    
    dnx_data_l2.age_and_flush_machine.max_age_states_get = dnx_data_l2_age_and_flush_machine_max_age_states_get;
    dnx_data_l2.age_and_flush_machine.flush_buffer_nof_entries_get = dnx_data_l2_age_and_flush_machine_flush_buffer_nof_entries_get;
    dnx_data_l2.age_and_flush_machine.traverse_thread_priority_get = dnx_data_l2_age_and_flush_machine_traverse_thread_priority_get;

    
    dnx_data_l2.age_and_flush_machine.flush_pulse_get = dnx_data_l2_age_and_flush_machine_flush_pulse_get;
    dnx_data_l2.age_and_flush_machine.flush_pulse_info_get = dnx_data_l2_age_and_flush_machine_flush_pulse_info_get;
    dnx_data_l2.age_and_flush_machine.age_get = dnx_data_l2_age_and_flush_machine_age_get;
    dnx_data_l2.age_and_flush_machine.age_info_get = dnx_data_l2_age_and_flush_machine_age_info_get;
    
    
    dnx_data_l2.olp.feature_get = dnx_data_l2_olp_feature_get;

    
    dnx_data_l2.olp.lpkgv_shift_get = dnx_data_l2_olp_lpkgv_shift_get;
    dnx_data_l2.olp.lpkgv_mask_get = dnx_data_l2_olp_lpkgv_mask_get;
    dnx_data_l2.olp.lpkgv_with_outlif_get = dnx_data_l2_olp_lpkgv_with_outlif_get;
    dnx_data_l2.olp.lpkgv_wo_outlif_get = dnx_data_l2_olp_lpkgv_wo_outlif_get;
    dnx_data_l2.olp.destination_offset_get = dnx_data_l2_olp_destination_offset_get;
    dnx_data_l2.olp.outlif_offset_get = dnx_data_l2_olp_outlif_offset_get;
    dnx_data_l2.olp.eei_offset_get = dnx_data_l2_olp_eei_offset_get;
    dnx_data_l2.olp.fec_offset_get = dnx_data_l2_olp_fec_offset_get;
    dnx_data_l2.olp.jr_mode_enhanced_performance_enable_get = dnx_data_l2_olp_jr_mode_enhanced_performance_enable_get;
    dnx_data_l2.olp.olp_learning_block_id_get = dnx_data_l2_olp_olp_learning_block_id_get;
    dnx_data_l2.olp.olp_fwd_block_id_get = dnx_data_l2_olp_olp_fwd_block_id_get;

    
    
    
    
    dnx_data_l3.egr_pointed.feature_get = dnx_data_l3_egr_pointed_feature_get;

    
    dnx_data_l3.egr_pointed.first_egr_pointed_id_get = dnx_data_l3_egr_pointed_first_egr_pointed_id_get;
    dnx_data_l3.egr_pointed.nof_egr_pointed_ids_get = dnx_data_l3_egr_pointed_nof_egr_pointed_ids_get;
    dnx_data_l3.egr_pointed.nof_bits_in_egr_pointed_id_get = dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_get;

    
    
    
    dnx_data_l3.fec.feature_get = dnx_data_l3_fec_feature_get;

    
    dnx_data_l3.fec.nof_fecs_get = dnx_data_l3_fec_nof_fecs_get;
    dnx_data_l3.fec.first_valid_fec_ecmp_id_get = dnx_data_l3_fec_first_valid_fec_ecmp_id_get;
    dnx_data_l3.fec.max_fec_id_for_single_dhb_cluster_pair_granularity_get = dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_get;
    dnx_data_l3.fec.max_fec_id_for_double_dhb_cluster_pair_granularity_get = dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_get;
    dnx_data_l3.fec.max_default_fec_get = dnx_data_l3_fec_max_default_fec_get;
    dnx_data_l3.fec.bank_size_get = dnx_data_l3_fec_bank_size_get;
    dnx_data_l3.fec.bank_size_round_up_get = dnx_data_l3_fec_bank_size_round_up_get;
    dnx_data_l3.fec.max_nof_super_fecs_per_bank_get = dnx_data_l3_fec_max_nof_super_fecs_per_bank_get;
    dnx_data_l3.fec.nof_physical_fecs_per_bank_get = dnx_data_l3_fec_nof_physical_fecs_per_bank_get;
    dnx_data_l3.fec.bank_nof_physical_rows_get = dnx_data_l3_fec_bank_nof_physical_rows_get;
    dnx_data_l3.fec.super_fec_size_get = dnx_data_l3_fec_super_fec_size_get;
    dnx_data_l3.fec.max_nof_banks_get = dnx_data_l3_fec_max_nof_banks_get;
    dnx_data_l3.fec.first_bank_without_id_alloc_get = dnx_data_l3_fec_first_bank_without_id_alloc_get;
    dnx_data_l3.fec.first_shared_bank_get = dnx_data_l3_fec_first_shared_bank_get;
    dnx_data_l3.fec.max_super_fec_id_get = dnx_data_l3_fec_max_super_fec_id_get;
    dnx_data_l3.fec.nof_fec_directions_get = dnx_data_l3_fec_nof_fec_directions_get;
    dnx_data_l3.fec.fec_z_max_fec_id_get = dnx_data_l3_fec_fec_z_max_fec_id_get;
    dnx_data_l3.fec.nof_fec_result_types_get = dnx_data_l3_fec_nof_fec_result_types_get;
    dnx_data_l3.fec.uneven_bank_sizes_get = dnx_data_l3_fec_uneven_bank_sizes_get;
    dnx_data_l3.fec.fer_hw_version_get = dnx_data_l3_fec_fer_hw_version_get;
    dnx_data_l3.fec.fec_property_supported_flags_get = dnx_data_l3_fec_fec_property_supported_flags_get;
    dnx_data_l3.fec.supported_fec_flags_get = dnx_data_l3_fec_supported_fec_flags_get;
    dnx_data_l3.fec.supported_fec_flags2_get = dnx_data_l3_fec_supported_fec_flags2_get;

    
    dnx_data_l3.fec.fec_resource_info_get = dnx_data_l3_fec_fec_resource_info_get;
    dnx_data_l3.fec.fec_resource_info_info_get = dnx_data_l3_fec_fec_resource_info_info_get;
    dnx_data_l3.fec.super_fec_result_types_map_get = dnx_data_l3_fec_super_fec_result_types_map_get;
    dnx_data_l3.fec.super_fec_result_types_map_info_get = dnx_data_l3_fec_super_fec_result_types_map_info_get;
    dnx_data_l3.fec.fec_result_type_fields_get = dnx_data_l3_fec_fec_result_type_fields_get;
    dnx_data_l3.fec.fec_result_type_fields_info_get = dnx_data_l3_fec_fec_result_type_fields_info_get;
    dnx_data_l3.fec.fec_physical_db_get = dnx_data_l3_fec_fec_physical_db_get;
    dnx_data_l3.fec.fec_physical_db_info_get = dnx_data_l3_fec_fec_physical_db_info_get;
    dnx_data_l3.fec.fec_tables_info_get = dnx_data_l3_fec_fec_tables_info_get;
    dnx_data_l3.fec.fec_tables_info_info_get = dnx_data_l3_fec_fec_tables_info_info_get;
    dnx_data_l3.fec.fec_hierachy_position_in_range_get = dnx_data_l3_fec_fec_hierachy_position_in_range_get;
    dnx_data_l3.fec.fec_hierachy_position_in_range_info_get = dnx_data_l3_fec_fec_hierachy_position_in_range_info_get;
    
    
    dnx_data_l3.fer.feature_get = dnx_data_l3_fer_feature_get;

    
    dnx_data_l3.fer.mdb_granularity_ratio_get = dnx_data_l3_fer_mdb_granularity_ratio_get;
    dnx_data_l3.fer.nof_fer_sort_to_mdb_table_muxes_get = dnx_data_l3_fer_nof_fer_sort_to_mdb_table_muxes_get;
    dnx_data_l3.fer.max_nof_sorting_blocks_per_fec_db_get = dnx_data_l3_fer_max_nof_sorting_blocks_per_fec_db_get;

    
    dnx_data_l3.fer.mux_configuration_get = dnx_data_l3_fer_mux_configuration_get;
    dnx_data_l3.fer.mux_configuration_info_get = dnx_data_l3_fer_mux_configuration_info_get;
    dnx_data_l3.fer.mux_connection_get = dnx_data_l3_fer_mux_connection_get;
    dnx_data_l3.fer.mux_connection_info_get = dnx_data_l3_fer_mux_connection_info_get;
    dnx_data_l3.fer.sorting_stages_info_get = dnx_data_l3_fer_sorting_stages_info_get;
    dnx_data_l3.fer.sorting_stages_info_info_get = dnx_data_l3_fer_sorting_stages_info_info_get;
    
    
    dnx_data_l3.source_address.feature_get = dnx_data_l3_source_address_feature_get;

    
    dnx_data_l3.source_address.source_address_table_size_get = dnx_data_l3_source_address_source_address_table_size_get;
    dnx_data_l3.source_address.nof_mymac_prefixes_get = dnx_data_l3_source_address_nof_mymac_prefixes_get;
    dnx_data_l3.source_address.max_nof_mac_addresses_used_by_arp_get = dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_get;
    dnx_data_l3.source_address.custom_sa_use_dual_homing_get = dnx_data_l3_source_address_custom_sa_use_dual_homing_get;
    dnx_data_l3.source_address.is_mymac_prefix_limitted_get = dnx_data_l3_source_address_is_mymac_prefix_limitted_get;

    
    dnx_data_l3.source_address.address_type_info_get = dnx_data_l3_source_address_address_type_info_get;
    dnx_data_l3.source_address.address_type_info_info_get = dnx_data_l3_source_address_address_type_info_info_get;
    
    
    dnx_data_l3.vrf.feature_get = dnx_data_l3_vrf_feature_get;

    
    dnx_data_l3.vrf.nof_vrf_get = dnx_data_l3_vrf_nof_vrf_get;
    dnx_data_l3.vrf.nof_vrf_ipv6_get = dnx_data_l3_vrf_nof_vrf_ipv6_get;

    
    
    
    dnx_data_l3.routing_enablers.feature_get = dnx_data_l3_routing_enablers_feature_get;

    
    dnx_data_l3.routing_enablers.routing_enable_vector_length_get = dnx_data_l3_routing_enablers_routing_enable_vector_length_get;
    dnx_data_l3.routing_enablers.routing_enable_action_support_get = dnx_data_l3_routing_enablers_routing_enable_action_support_get;
    dnx_data_l3.routing_enablers.routing_enable_action_profile_size_get = dnx_data_l3_routing_enablers_routing_enable_action_profile_size_get;
    dnx_data_l3.routing_enablers.routing_enable_indication_nof_bits_per_layer_get = dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_get;
    dnx_data_l3.routing_enablers.routing_enable_nof_profiles_per_layer_get = dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_get;
    dnx_data_l3.routing_enablers.routing_enabled_action_profile_id_get = dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_get;

    
    dnx_data_l3.routing_enablers.layer_enablers_by_id_get = dnx_data_l3_routing_enablers_layer_enablers_by_id_get;
    dnx_data_l3.routing_enablers.layer_enablers_by_id_info_get = dnx_data_l3_routing_enablers_layer_enablers_by_id_info_get;
    dnx_data_l3.routing_enablers.layer_enablers_get = dnx_data_l3_routing_enablers_layer_enablers_get;
    dnx_data_l3.routing_enablers.layer_enablers_info_get = dnx_data_l3_routing_enablers_layer_enablers_info_get;
    
    
    dnx_data_l3.rif.feature_get = dnx_data_l3_rif_feature_get;

    
    dnx_data_l3.rif.max_nof_rifs_get = dnx_data_l3_rif_max_nof_rifs_get;
    dnx_data_l3.rif.nof_rifs_get = dnx_data_l3_rif_nof_rifs_get;

    
    
    
    dnx_data_l3.fwd.feature_get = dnx_data_l3_fwd_feature_get;

    
    dnx_data_l3.fwd.flp_fragment_support_get = dnx_data_l3_fwd_flp_fragment_support_get;
    dnx_data_l3.fwd.max_mc_group_lpm_get = dnx_data_l3_fwd_max_mc_group_lpm_get;
    dnx_data_l3.fwd.max_mc_group_em_get = dnx_data_l3_fwd_max_mc_group_em_get;
    dnx_data_l3.fwd.compressed_sip_svl_size_get = dnx_data_l3_fwd_compressed_sip_svl_size_get;
    dnx_data_l3.fwd.nof_compressed_svl_sip_get = dnx_data_l3_fwd_nof_compressed_svl_sip_get;
    dnx_data_l3.fwd.lpm_strength_profiles_support_get = dnx_data_l3_fwd_lpm_strength_profiles_support_get;
    dnx_data_l3.fwd.nof_lpm_strength_profiles_get = dnx_data_l3_fwd_nof_lpm_strength_profiles_get;
    dnx_data_l3.fwd.compressed_sip_ivl_size_get = dnx_data_l3_fwd_compressed_sip_ivl_size_get;
    dnx_data_l3.fwd.nof_compressed_ivl_sip_get = dnx_data_l3_fwd_nof_compressed_ivl_sip_get;
    dnx_data_l3.fwd.uc_supported_route_flags_get = dnx_data_l3_fwd_uc_supported_route_flags_get;
    dnx_data_l3.fwd.uc_supported_route_flags2_get = dnx_data_l3_fwd_uc_supported_route_flags2_get;
    dnx_data_l3.fwd.uc_supported_kbp_route_flags_get = dnx_data_l3_fwd_uc_supported_kbp_route_flags_get;
    dnx_data_l3.fwd.uc_supported_kbp_route_flags2_get = dnx_data_l3_fwd_uc_supported_kbp_route_flags2_get;
    dnx_data_l3.fwd.ipmc_supported_flags_get = dnx_data_l3_fwd_ipmc_supported_flags_get;
    dnx_data_l3.fwd.ipmc_config_supported_flags_get = dnx_data_l3_fwd_ipmc_config_supported_flags_get;
    dnx_data_l3.fwd.ipmc_config_supported_traverse_flags_get = dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_get;
    dnx_data_l3.fwd.last_kaps_intf_id_get = dnx_data_l3_fwd_last_kaps_intf_id_get;

    
    dnx_data_l3.fwd.lpm_profile_to_entry_strength_get = dnx_data_l3_fwd_lpm_profile_to_entry_strength_get;
    dnx_data_l3.fwd.lpm_profile_to_entry_strength_info_get = dnx_data_l3_fwd_lpm_profile_to_entry_strength_info_get;
    dnx_data_l3.fwd.routing_tables_get = dnx_data_l3_fwd_routing_tables_get;
    dnx_data_l3.fwd.routing_tables_info_get = dnx_data_l3_fwd_routing_tables_info_get;
    dnx_data_l3.fwd.mc_tables_get = dnx_data_l3_fwd_mc_tables_get;
    dnx_data_l3.fwd.mc_tables_info_get = dnx_data_l3_fwd_mc_tables_info_get;
    dnx_data_l3.fwd.ipmc_compression_mode_get = dnx_data_l3_fwd_ipmc_compression_mode_get;
    dnx_data_l3.fwd.ipmc_compression_mode_info_get = dnx_data_l3_fwd_ipmc_compression_mode_info_get;
    dnx_data_l3.fwd.lpm_default_prefix_length_info_get = dnx_data_l3_fwd_lpm_default_prefix_length_info_get;
    dnx_data_l3.fwd.lpm_default_prefix_length_info_info_get = dnx_data_l3_fwd_lpm_default_prefix_length_info_info_get;
    dnx_data_l3.fwd.lpm_app_db_to_strength_profile_get = dnx_data_l3_fwd_lpm_app_db_to_strength_profile_get;
    dnx_data_l3.fwd.lpm_app_db_to_strength_profile_info_get = dnx_data_l3_fwd_lpm_app_db_to_strength_profile_info_get;
    
    
    dnx_data_l3.ecmp.feature_get = dnx_data_l3_ecmp_feature_get;

    
    dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get = dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_get;
    dnx_data_l3.ecmp.group_size_consistent_nof_bits_get = dnx_data_l3_ecmp_group_size_consistent_nof_bits_get;
    dnx_data_l3.ecmp.group_size_nof_bits_get = dnx_data_l3_ecmp_group_size_nof_bits_get;
    dnx_data_l3.ecmp.max_group_size_get = dnx_data_l3_ecmp_max_group_size_get;
    dnx_data_l3.ecmp.nof_ecmp_basic_mode_get = dnx_data_l3_ecmp_nof_ecmp_basic_mode_get;
    dnx_data_l3.ecmp.max_ecmp_basic_mode_get = dnx_data_l3_ecmp_max_ecmp_basic_mode_get;
    dnx_data_l3.ecmp.max_ecmp_extended_mode_get = dnx_data_l3_ecmp_max_ecmp_extended_mode_get;
    dnx_data_l3.ecmp.nof_extended_ecmp_per_failover_bank_get = dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_get;
    dnx_data_l3.ecmp.nof_failover_banks_per_extended_ecmp_bank_get = dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_get;
    dnx_data_l3.ecmp.nof_group_profiles_per_hierarchy_get = dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get;
    dnx_data_l3.ecmp.member_table_nof_rows_size_in_bits_get = dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get;
    dnx_data_l3.ecmp.member_table_address_size_in_bits_get = dnx_data_l3_ecmp_member_table_address_size_in_bits_get;
    dnx_data_l3.ecmp.ecmp_bank_size_get = dnx_data_l3_ecmp_ecmp_bank_size_get;
    dnx_data_l3.ecmp.nof_ecmp_banks_basic_get = dnx_data_l3_ecmp_nof_ecmp_banks_basic_get;
    dnx_data_l3.ecmp.nof_ecmp_banks_extended_get = dnx_data_l3_ecmp_nof_ecmp_banks_extended_get;
    dnx_data_l3.ecmp.nof_ecmp_get = dnx_data_l3_ecmp_nof_ecmp_get;
    dnx_data_l3.ecmp.profile_id_size_get = dnx_data_l3_ecmp_profile_id_size_get;
    dnx_data_l3.ecmp.profile_id_offset_get = dnx_data_l3_ecmp_profile_id_offset_get;
    dnx_data_l3.ecmp.tunnel_priority_support_get = dnx_data_l3_ecmp_tunnel_priority_support_get;
    dnx_data_l3.ecmp.nof_tunnel_priority_map_profiles_get = dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get;
    dnx_data_l3.ecmp.tunnel_priority_field_width_get = dnx_data_l3_ecmp_tunnel_priority_field_width_get;
    dnx_data_l3.ecmp.tunnel_priority_index_field_width_get = dnx_data_l3_ecmp_tunnel_priority_index_field_width_get;
    dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get = dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get;
    dnx_data_l3.ecmp.consistent_bank_size_in_bits_get = dnx_data_l3_ecmp_consistent_bank_size_in_bits_get;
    dnx_data_l3.ecmp.ecmp_small_consistent_group_nof_entries_get = dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_get;
    dnx_data_l3.ecmp.ecmp_small_consistent_group_entry_size_in_bits_get = dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_get;
    dnx_data_l3.ecmp.nof_consistent_resources_get = dnx_data_l3_ecmp_nof_consistent_resources_get;

    
    dnx_data_l3.ecmp.consistent_tables_info_get = dnx_data_l3_ecmp_consistent_tables_info_get;
    dnx_data_l3.ecmp.consistent_tables_info_info_get = dnx_data_l3_ecmp_consistent_tables_info_info_get;
    
    
    dnx_data_l3.vip_ecmp.feature_get = dnx_data_l3_vip_ecmp_feature_get;

    
    dnx_data_l3.vip_ecmp.nof_vip_ecmp_get = dnx_data_l3_vip_ecmp_nof_vip_ecmp_get;
    dnx_data_l3.vip_ecmp.max_vip_ecmp_table_size_get = dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_get;

    
    
    
    dnx_data_l3.wcmp.feature_get = dnx_data_l3_wcmp_feature_get;

    
    dnx_data_l3.wcmp.group_size_nof_bits_get = dnx_data_l3_wcmp_group_size_nof_bits_get;
    dnx_data_l3.wcmp.max_member_weight_get = dnx_data_l3_wcmp_max_member_weight_get;
    dnx_data_l3.wcmp.member_weight_quant_factor_get = dnx_data_l3_wcmp_member_weight_quant_factor_get;

    
    
    
    dnx_data_l3.vrrp.feature_get = dnx_data_l3_vrrp_feature_get;

    
    dnx_data_l3.vrrp.nof_protocol_groups_get = dnx_data_l3_vrrp_nof_protocol_groups_get;
    dnx_data_l3.vrrp.nof_tcam_entries_get = dnx_data_l3_vrrp_nof_tcam_entries_get;
    dnx_data_l3.vrrp.nof_vsi_tcam_entries_get = dnx_data_l3_vrrp_nof_vsi_tcam_entries_get;
    dnx_data_l3.vrrp.nof_tcam_entries_used_by_exem_get = dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_get;
    dnx_data_l3.vrrp.exem_vrid_ipv4_tcam_index_get = dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get;
    dnx_data_l3.vrrp.exem_vrid_ipv6_tcam_index_get = dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get;
    dnx_data_l3.vrrp.exem_default_tcam_index_get = dnx_data_l3_vrrp_exem_default_tcam_index_get;
    dnx_data_l3.vrrp.exem_default_tcam_index_msb_get = dnx_data_l3_vrrp_exem_default_tcam_index_msb_get;
    dnx_data_l3.vrrp.vrrp_default_num_entries_get = dnx_data_l3_vrrp_vrrp_default_num_entries_get;
    dnx_data_l3.vrrp.is_mapped_pp_port_used_get = dnx_data_l3_vrrp_is_mapped_pp_port_used_get;
    dnx_data_l3.vrrp.nof_dbs_get = dnx_data_l3_vrrp_nof_dbs_get;

    
    dnx_data_l3.vrrp.vrrp_tables_get = dnx_data_l3_vrrp_vrrp_tables_get;
    dnx_data_l3.vrrp.vrrp_tables_info_get = dnx_data_l3_vrrp_vrrp_tables_info_get;
    dnx_data_l3.vrrp.data_bases_get = dnx_data_l3_vrrp_data_bases_get;
    dnx_data_l3.vrrp.data_bases_info_get = dnx_data_l3_vrrp_data_bases_info_get;
    
    
    dnx_data_l3.feature.feature_get = dnx_data_l3_feature_feature_get;

    

    
    
    
    
    dnx_data_lane_map.nif.feature_get = dnx_data_lane_map_nif_feature_get;

    

    
    dnx_data_lane_map.nif.mapping_get = dnx_data_lane_map_nif_mapping_get;
    dnx_data_lane_map.nif.mapping_info_get = dnx_data_lane_map_nif_mapping_info_get;
    
    
    dnx_data_lane_map.fabric.feature_get = dnx_data_lane_map_fabric_feature_get;

    
    dnx_data_lane_map.fabric.resolution_get = dnx_data_lane_map_fabric_resolution_get;

    
    dnx_data_lane_map.fabric.mapping_get = dnx_data_lane_map_fabric_mapping_get;
    dnx_data_lane_map.fabric.mapping_info_get = dnx_data_lane_map_fabric_mapping_info_get;
    
    
    dnx_data_lane_map.ilkn.feature_get = dnx_data_lane_map_ilkn_feature_get;

    

    
    dnx_data_lane_map.ilkn.remapping_get = dnx_data_lane_map_ilkn_remapping_get;
    dnx_data_lane_map.ilkn.remapping_info_get = dnx_data_lane_map_ilkn_remapping_info_get;
    
    
    dnx_data_lane_map.otn.feature_get = dnx_data_lane_map_otn_feature_get;

    

    
    dnx_data_lane_map.otn.logical_lanes_get = dnx_data_lane_map_otn_logical_lanes_get;
    dnx_data_lane_map.otn.logical_lanes_info_get = dnx_data_lane_map_otn_logical_lanes_info_get;
    dnx_data_lane_map.otn.logical_otn_lane_get = dnx_data_lane_map_otn_logical_otn_lane_get;
    dnx_data_lane_map.otn.logical_otn_lane_info_get = dnx_data_lane_map_otn_logical_otn_lane_info_get;
    
    
    
    dnx_data_latency.features.feature_get = dnx_data_latency_features_feature_get;

    
    dnx_data_latency.features.latency_bins_get = dnx_data_latency_features_latency_bins_get;

    
    
    
    dnx_data_latency.profile.feature_get = dnx_data_latency_profile_feature_get;

    
    dnx_data_latency.profile.egress_nof_get = dnx_data_latency_profile_egress_nof_get;
    dnx_data_latency.profile.ingress_nof_get = dnx_data_latency_profile_ingress_nof_get;

    
    
    
    dnx_data_latency.based_admission.feature_get = dnx_data_latency_based_admission_feature_get;

    
    dnx_data_latency.based_admission.reject_flow_profile_value_get = dnx_data_latency_based_admission_reject_flow_profile_value_get;
    dnx_data_latency.based_admission.cgm_extended_profiles_enable_value_get = dnx_data_latency_based_admission_cgm_extended_profiles_enable_value_get;
    dnx_data_latency.based_admission.max_supported_profile_get = dnx_data_latency_based_admission_max_supported_profile_get;

    
    
    
    dnx_data_latency.ingress.feature_get = dnx_data_latency_ingress_feature_get;

    
    dnx_data_latency.ingress.jr_mode_latency_timestamp_left_shift_get = dnx_data_latency_ingress_jr_mode_latency_timestamp_left_shift_get;
    dnx_data_latency.ingress.nof_flow_profile_get = dnx_data_latency_ingress_nof_flow_profile_get;

    
    
    
    
    dnx_data_led.general.feature_get = dnx_data_led_general_feature_get;

    
    dnx_data_led.general.last_port_on_scan_chain_get = dnx_data_led_general_last_port_on_scan_chain_get;
    dnx_data_led.general.m0ss_freq_mhz_get = dnx_data_led_general_m0ss_freq_mhz_get;

    
    
    
    
    dnx_data_lif.global_lif.feature_get = dnx_data_lif_global_lif_feature_get;

    
    dnx_data_lif.global_lif.use_mdb_size_get = dnx_data_lif_global_lif_use_mdb_size_get;
    dnx_data_lif.global_lif.glem_rif_optimization_enabled_get = dnx_data_lif_global_lif_glem_rif_optimization_enabled_get;
    dnx_data_lif.global_lif.null_lif_get = dnx_data_lif_global_lif_null_lif_get;
    dnx_data_lif.global_lif.three_intf_glem_get = dnx_data_lif_global_lif_three_intf_glem_get;
    dnx_data_lif.global_lif.nof_global_in_lifs_get = dnx_data_lif_global_lif_nof_global_in_lifs_get;
    dnx_data_lif.global_lif.nof_global_out_lifs_get = dnx_data_lif_global_lif_nof_global_out_lifs_get;
    dnx_data_lif.global_lif.egress_in_lif_null_value_get = dnx_data_lif_global_lif_egress_in_lif_null_value_get;
    dnx_data_lif.global_lif.egress_out_lif_null_value_get = dnx_data_lif_global_lif_egress_out_lif_null_value_get;
    dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get = dnx_data_lif_global_lif_nof_global_l2_gport_lifs_get;
    dnx_data_lif.global_lif.global_lif_bta_max_val_get = dnx_data_lif_global_lif_global_lif_bta_max_val_get;
    dnx_data_lif.global_lif.global_lif_tag_nof_bta_bits_get = dnx_data_lif_global_lif_global_lif_tag_nof_bta_bits_get;
    dnx_data_lif.global_lif.global_lif_intf_max_val_get = dnx_data_lif_global_lif_global_lif_intf_max_val_get;
    dnx_data_lif.global_lif.global_lif_tag_nof_intf_bits_get = dnx_data_lif_global_lif_global_lif_tag_nof_intf_bits_get;
    dnx_data_lif.global_lif.global_lif_group_max_val_get = dnx_data_lif_global_lif_global_lif_group_max_val_get;
    dnx_data_lif.global_lif.global_lif_tag_nof_group_bits_get = dnx_data_lif_global_lif_global_lif_tag_nof_group_bits_get;
    dnx_data_lif.global_lif.global_lif_grain_size_get = dnx_data_lif_global_lif_global_lif_grain_size_get;

    
    
    
    dnx_data_lif.virtual_lif.feature_get = dnx_data_lif_virtual_lif_feature_get;

    
    dnx_data_lif.virtual_lif.flow_ingress_ids_get = dnx_data_lif_virtual_lif_flow_ingress_ids_get;
    dnx_data_lif.virtual_lif.flow_egress_ids_get = dnx_data_lif_virtual_lif_flow_egress_ids_get;

    
    
    
    dnx_data_lif.in_lif.feature_get = dnx_data_lif_in_lif_feature_get;

    
    dnx_data_lif.in_lif.inlif_index_bit_size_get = dnx_data_lif_in_lif_inlif_index_bit_size_get;
    dnx_data_lif.in_lif.inlif_minimum_index_size_get = dnx_data_lif_in_lif_inlif_minimum_index_size_get;
    dnx_data_lif.in_lif.inlif_resource_tag_size_get = dnx_data_lif_in_lif_inlif_resource_tag_size_get;
    dnx_data_lif.in_lif.nof_in_lif_profiles_get = dnx_data_lif_in_lif_nof_in_lif_profiles_get;
    dnx_data_lif.in_lif.nof_used_in_lif_profile_bits_get = dnx_data_lif_in_lif_nof_used_in_lif_profile_bits_get;
    dnx_data_lif.in_lif.nof_used_eth_rif_profile_bits_get = dnx_data_lif_in_lif_nof_used_eth_rif_profile_bits_get;
    dnx_data_lif.in_lif.nof_in_lif_incoming_orientation_get = dnx_data_lif_in_lif_nof_in_lif_incoming_orientation_get;
    dnx_data_lif.in_lif.nof_eth_rif_profiles_get = dnx_data_lif_in_lif_nof_eth_rif_profiles_get;
    dnx_data_lif.in_lif.nof_da_not_found_destination_profiles_get = dnx_data_lif_in_lif_nof_da_not_found_destination_profiles_get;
    dnx_data_lif.in_lif.default_da_not_found_destination_profile_get = dnx_data_lif_in_lif_default_da_not_found_destination_profile_get;
    dnx_data_lif.in_lif.drop_in_lif_get = dnx_data_lif_in_lif_drop_in_lif_get;
    dnx_data_lif.in_lif.mc_bridge_fallback_per_lif_profile_get = dnx_data_lif_in_lif_mc_bridge_fallback_per_lif_profile_get;
    dnx_data_lif.in_lif.max_number_of_orientation_bits_iop_mode_get = dnx_data_lif_in_lif_max_number_of_orientation_bits_iop_mode_get;
    dnx_data_lif.in_lif.max_number_of_egress_lif_profile_bits_iop_mode_get = dnx_data_lif_in_lif_max_number_of_egress_lif_profile_bits_iop_mode_get;
    dnx_data_lif.in_lif.algo_flow_inlif_bank_size_get = dnx_data_lif_in_lif_algo_flow_inlif_bank_size_get;
    dnx_data_lif.in_lif.nof_routing_enable_profiles_get = dnx_data_lif_in_lif_nof_routing_enable_profiles_get;
    dnx_data_lif.in_lif.nof_shift_bits_get = dnx_data_lif_in_lif_nof_shift_bits_get;
    dnx_data_lif.in_lif.max_nof_per_core_inlifs_ratio_get = dnx_data_lif_in_lif_max_nof_per_core_inlifs_ratio_get;
    dnx_data_lif.in_lif.generic_data_1_start_position_get = dnx_data_lif_in_lif_generic_data_1_start_position_get;
    dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get = dnx_data_lif_in_lif_in_lif_profile_pmf_reserved_start_bit_get;
    dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get = dnx_data_lif_in_lif_eth_rif_profile_pmf_reserved_start_bit_get;
    dnx_data_lif.in_lif.in_lif_profile_egress_reserved_start_bit_get = dnx_data_lif_in_lif_in_lif_profile_egress_reserved_start_bit_get;
    dnx_data_lif.in_lif.in_lif_profile_allocate_indexed_mode_get = dnx_data_lif_in_lif_in_lif_profile_allocate_indexed_mode_get;
    dnx_data_lif.in_lif.nof_in_lif_profile_allocate_orientation_bits_get = dnx_data_lif_in_lif_nof_in_lif_profile_allocate_orientation_bits_get;
    dnx_data_lif.in_lif.nof_in_lif_profile_allocate_policer_inner_dp_bits_get = dnx_data_lif_in_lif_nof_in_lif_profile_allocate_policer_inner_dp_bits_get;
    dnx_data_lif.in_lif.nof_in_lif_profile_allocate_policer_outer_dp_bits_get = dnx_data_lif_in_lif_nof_in_lif_profile_allocate_policer_outer_dp_bits_get;
    dnx_data_lif.in_lif.nof_in_lif_profile_allocate_same_interface_mode_bits_get = dnx_data_lif_in_lif_nof_in_lif_profile_allocate_same_interface_mode_bits_get;
    dnx_data_lif.in_lif.nof_in_lif_egress_reserved_bits_get = dnx_data_lif_in_lif_nof_in_lif_egress_reserved_bits_get;
    dnx_data_lif.in_lif.eth_rif_profile_egress_reserved_nof_bits_get = dnx_data_lif_in_lif_eth_rif_profile_egress_reserved_nof_bits_get;
    dnx_data_lif.in_lif.eth_rif_profile_egress_reserved_start_bit_get = dnx_data_lif_in_lif_eth_rif_profile_egress_reserved_start_bit_get;
    dnx_data_lif.in_lif.in_lif_profile_native_indexed_mode_start_bit_get = dnx_data_lif_in_lif_in_lif_profile_native_indexed_mode_start_bit_get;
    dnx_data_lif.in_lif.nof_in_lif_profile_allocate_indexed_mode_bits_get = dnx_data_lif_in_lif_nof_in_lif_profile_allocate_indexed_mode_bits_get;
    dnx_data_lif.in_lif.in_lif_profile_allocate_orientation_get = dnx_data_lif_in_lif_in_lif_profile_allocate_orientation_get;
    dnx_data_lif.in_lif.in_lif_profile_allocate_policer_inner_dp_get = dnx_data_lif_in_lif_in_lif_profile_allocate_policer_inner_dp_get;
    dnx_data_lif.in_lif.in_lif_profile_allocate_policer_outer_dp_get = dnx_data_lif_in_lif_in_lif_profile_allocate_policer_outer_dp_get;
    dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get = dnx_data_lif_in_lif_in_lif_profile_allocate_same_interface_mode_get;

    
    
    
    dnx_data_lif.out_lif.feature_get = dnx_data_lif_out_lif_feature_get;

    
    dnx_data_lif.out_lif.nof_out_lif_profiles_get = dnx_data_lif_out_lif_nof_out_lif_profiles_get;

    dnx_data_lif.out_lif.nof_erpp_out_lif_profiles_get = dnx_data_lif_out_lif_nof_erpp_out_lif_profiles_get;

    dnx_data_lif.out_lif.nof_out_lif_outgoing_orientation_get = dnx_data_lif_out_lif_nof_out_lif_outgoing_orientation_get;
    dnx_data_lif.out_lif.outrif_profile_width_get = dnx_data_lif_out_lif_outrif_profile_width_get;
    dnx_data_lif.out_lif.glem_result_get = dnx_data_lif_out_lif_glem_result_get;
    dnx_data_lif.out_lif.physical_bank_pointer_size_get = dnx_data_lif_out_lif_physical_bank_pointer_size_get;
    dnx_data_lif.out_lif.outlif_eedb_banks_pointer_size_get = dnx_data_lif_out_lif_outlif_eedb_banks_pointer_size_get;
    dnx_data_lif.out_lif.global_outlif_bta_sop_resolution_get = dnx_data_lif_out_lif_global_outlif_bta_sop_resolution_get;
    dnx_data_lif.out_lif.pmf_outlif_profile_width_get = dnx_data_lif_out_lif_pmf_outlif_profile_width_get;
    dnx_data_lif.out_lif.pmf_outrif_profile_width_get = dnx_data_lif_out_lif_pmf_outrif_profile_width_get;
    dnx_data_lif.out_lif.data_entries_size_get = dnx_data_lif_out_lif_data_entries_size_get;
    dnx_data_lif.out_lif.etpp_out_lif_profile_default_mpls_profile_get = dnx_data_lif_out_lif_etpp_out_lif_profile_default_mpls_profile_get;
    dnx_data_lif.out_lif.etpp_out_lif_profile_default_port_profile_get = dnx_data_lif_out_lif_etpp_out_lif_profile_default_port_profile_get;
    dnx_data_lif.out_lif.etpp_out_lif_profile_default_orientation_profile_get = dnx_data_lif_out_lif_etpp_out_lif_profile_default_orientation_profile_get;
    dnx_data_lif.out_lif.outlif_pointer_size_get = dnx_data_lif_out_lif_outlif_pointer_size_get;
    dnx_data_lif.out_lif.allocation_bank_size_get = dnx_data_lif_out_lif_allocation_bank_size_get;
    dnx_data_lif.out_lif.local_outlif_width_get = dnx_data_lif_out_lif_local_outlif_width_get;
    dnx_data_lif.out_lif.outlif_profile_width_get = dnx_data_lif_out_lif_outlif_profile_width_get;
    dnx_data_lif.out_lif.nof_local_out_lifs_get = dnx_data_lif_out_lif_nof_local_out_lifs_get;
    dnx_data_lif.out_lif.physical_bank_size_get = dnx_data_lif_out_lif_physical_bank_size_get;
    dnx_data_lif.out_lif.global_ipv6_outlif_bta_sop_get = dnx_data_lif_out_lif_global_ipv6_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_vxlan_outlif_bta_sop_get = dnx_data_lif_out_lif_global_vxlan_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_mpls_outlif_bta_sop_get = dnx_data_lif_out_lif_global_mpls_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_srv6_outlif_bta_sop_get = dnx_data_lif_out_lif_global_srv6_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_sflow_extended_gateway_two_outlif_bta_sop_get = dnx_data_lif_out_lif_global_sflow_extended_gateway_two_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_sflow_extended_gateway_one_outlif_bta_sop_get = dnx_data_lif_out_lif_global_sflow_extended_gateway_one_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_sflow_header_sampling_outlif_bta_sop_get = dnx_data_lif_out_lif_global_sflow_header_sampling_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_sflow_dp_outlif_bta_sop_get = dnx_data_lif_out_lif_global_sflow_dp_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_gtp_concat_ip_outlif_bta_sop_get = dnx_data_lif_out_lif_global_gtp_concat_ip_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_bier_outlif_bta_sop_get = dnx_data_lif_out_lif_global_bier_outlif_bta_sop_get;
    dnx_data_lif.out_lif.global_lif_allocation_estimated_encap_size_enable_get = dnx_data_lif_out_lif_global_lif_allocation_estimated_encap_size_enable_get;
    dnx_data_lif.out_lif.global_lif_allocation_outlif_intf_enable_get = dnx_data_lif_out_lif_global_lif_allocation_outlif_intf_enable_get;
    dnx_data_lif.out_lif.global_lif_allocation_outlif_group_enable_get = dnx_data_lif_out_lif_global_lif_allocation_outlif_group_enable_get;

    
    dnx_data_lif.out_lif.logical_to_physical_phase_map_get = dnx_data_lif_out_lif_logical_to_physical_phase_map_get;
    dnx_data_lif.out_lif.logical_to_physical_phase_map_info_get = dnx_data_lif_out_lif_logical_to_physical_phase_map_info_get;
    
    
    dnx_data_lif.lif_table_manager.feature_get = dnx_data_lif_lif_table_manager_feature_get;

    
    dnx_data_lif.lif_table_manager.max_nof_lif_tables_get = dnx_data_lif_lif_table_manager_max_nof_lif_tables_get;
    dnx_data_lif.lif_table_manager.max_nof_result_types_get = dnx_data_lif_lif_table_manager_max_nof_result_types_get;
    dnx_data_lif.lif_table_manager.max_fields_per_result_type_get = dnx_data_lif_lif_table_manager_max_fields_per_result_type_get;
    dnx_data_lif.lif_table_manager.max_fields_per_table_get = dnx_data_lif_lif_table_manager_max_fields_per_table_get;

    
    
    
    dnx_data_lif.feature.feature_get = dnx_data_lif_feature_feature_get;

    

    
    
    
    
    dnx_data_linkscan.general.feature_get = dnx_data_linkscan_general_feature_get;

    
    dnx_data_linkscan.general.error_delay_get = dnx_data_linkscan_general_error_delay_get;
    dnx_data_linkscan.general.max_error_get = dnx_data_linkscan_general_max_error_get;
    dnx_data_linkscan.general.thread_priority_get = dnx_data_linkscan_general_thread_priority_get;
    dnx_data_linkscan.general.m0_pause_enable_get = dnx_data_linkscan_general_m0_pause_enable_get;

    
    
    
    
    dnx_data_macsec.general.feature_get = dnx_data_macsec_general_feature_get;

    
    dnx_data_macsec.general.macsec_nof_get = dnx_data_macsec_general_macsec_nof_get;
    dnx_data_macsec.general.macsec_in_core_nof_get = dnx_data_macsec_general_macsec_in_core_nof_get;
    dnx_data_macsec.general.ports_in_macsec_nof_get = dnx_data_macsec_general_ports_in_macsec_nof_get;
    dnx_data_macsec.general.etype_nof_get = dnx_data_macsec_general_etype_nof_get;
    dnx_data_macsec.general.etype_not_zero_verify_get = dnx_data_macsec_general_etype_not_zero_verify_get;
    dnx_data_macsec.general.tx_threshold_get = dnx_data_macsec_general_tx_threshold_get;
    dnx_data_macsec.general.macsec_tdm_cal_max_depth_get = dnx_data_macsec_general_macsec_tdm_cal_max_depth_get;
    dnx_data_macsec.general.macsec_tdm_cal_hw_depth_get = dnx_data_macsec_general_macsec_tdm_cal_hw_depth_get;
    dnx_data_macsec.general.association_number_full_range_nof_get = dnx_data_macsec_general_association_number_full_range_nof_get;
    dnx_data_macsec.general.macsec_gen_get = dnx_data_macsec_general_macsec_gen_get;
    dnx_data_macsec.general.pfc_and_pause_timer_nof_bits_get = dnx_data_macsec_general_pfc_and_pause_timer_nof_bits_get;
    dnx_data_macsec.general.sectag_offset_min_value_get = dnx_data_macsec_general_sectag_offset_min_value_get;
    dnx_data_macsec.general.supported_entries_nof_get = dnx_data_macsec_general_supported_entries_nof_get;
    dnx_data_macsec.general.is_macsec_enabled_get = dnx_data_macsec_general_is_macsec_enabled_get;
    dnx_data_macsec.general.is_power_optimization_enabled_get = dnx_data_macsec_general_is_power_optimization_enabled_get;

    
    dnx_data_macsec.general.pm_to_macsec_get = dnx_data_macsec_general_pm_to_macsec_get;
    dnx_data_macsec.general.pm_to_macsec_info_get = dnx_data_macsec_general_pm_to_macsec_info_get;
    dnx_data_macsec.general.macsec_instances_get = dnx_data_macsec_general_macsec_instances_get;
    dnx_data_macsec.general.macsec_instances_info_get = dnx_data_macsec_general_macsec_instances_info_get;
    dnx_data_macsec.general.macsec_block_mapping_get = dnx_data_macsec_general_macsec_block_mapping_get;
    dnx_data_macsec.general.macsec_block_mapping_info_get = dnx_data_macsec_general_macsec_block_mapping_info_get;
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec.general.macsec_control_options_get = dnx_data_macsec_general_macsec_control_options_get;
    dnx_data_macsec.general.macsec_control_options_info_get = dnx_data_macsec_general_macsec_control_options_info_get;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec.general.stat_special_map_get = dnx_data_macsec_general_stat_special_map_get;
    dnx_data_macsec.general.stat_special_map_info_get = dnx_data_macsec_general_stat_special_map_info_get;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec.general.counter_map_get = dnx_data_macsec_general_counter_map_get;
    dnx_data_macsec.general.counter_map_info_get = dnx_data_macsec_general_counter_map_info_get;

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 

    dnx_data_macsec.general.access_counter_map_get = dnx_data_macsec_general_access_counter_map_get;
    dnx_data_macsec.general.access_counter_map_info_get = dnx_data_macsec_general_access_counter_map_info_get;

#endif 
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_ACCESS_SUPPORT) 

    dnx_data_macsec.general.access_stat_map_get = dnx_data_macsec_general_access_stat_map_get;
    dnx_data_macsec.general.access_stat_map_info_get = dnx_data_macsec_general_access_stat_map_info_get;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_macsec.general.stat_map_get = dnx_data_macsec_general_stat_map_get;
    dnx_data_macsec.general.stat_map_info_get = dnx_data_macsec_general_stat_map_info_get;

#endif 
    
    
    dnx_data_macsec.ingress.feature_get = dnx_data_macsec_ingress_feature_get;

    
    dnx_data_macsec.ingress.flow_nof_get = dnx_data_macsec_ingress_flow_nof_get;
    dnx_data_macsec.ingress.policy_nof_get = dnx_data_macsec_ingress_policy_nof_get;
    dnx_data_macsec.ingress.secure_assoc_nof_get = dnx_data_macsec_ingress_secure_assoc_nof_get;
    dnx_data_macsec.ingress.udf_nof_bits_get = dnx_data_macsec_ingress_udf_nof_bits_get;
    dnx_data_macsec.ingress.mgmt_rule_exact_nof_get = dnx_data_macsec_ingress_mgmt_rule_exact_nof_get;
    dnx_data_macsec.ingress.tpid_nof_get = dnx_data_macsec_ingress_tpid_nof_get;
    dnx_data_macsec.ingress.cpu_flex_map_nof_get = dnx_data_macsec_ingress_cpu_flex_map_nof_get;
    dnx_data_macsec.ingress.sc_tcam_nof_get = dnx_data_macsec_ingress_sc_tcam_nof_get;
    dnx_data_macsec.ingress.sa_per_sc_nof_get = dnx_data_macsec_ingress_sa_per_sc_nof_get;
    dnx_data_macsec.ingress.invalidate_sa_get = dnx_data_macsec_ingress_invalidate_sa_get;
    dnx_data_macsec.ingress.secure_channel_nof_get = dnx_data_macsec_ingress_secure_channel_nof_get;

    
    dnx_data_macsec.ingress.udf_get = dnx_data_macsec_ingress_udf_get;
    dnx_data_macsec.ingress.udf_info_get = dnx_data_macsec_ingress_udf_info_get;
    dnx_data_macsec.ingress.restricted_sc_tcam_id_get = dnx_data_macsec_ingress_restricted_sc_tcam_id_get;
    dnx_data_macsec.ingress.restricted_sc_tcam_id_info_get = dnx_data_macsec_ingress_restricted_sc_tcam_id_info_get;
    dnx_data_macsec.ingress.restricted_sp_tcam_id_get = dnx_data_macsec_ingress_restricted_sp_tcam_id_get;
    dnx_data_macsec.ingress.restricted_sp_tcam_id_info_get = dnx_data_macsec_ingress_restricted_sp_tcam_id_info_get;
    
    
    dnx_data_macsec.egress.feature_get = dnx_data_macsec_egress_feature_get;

    
    dnx_data_macsec.egress.secure_assoc_nof_get = dnx_data_macsec_egress_secure_assoc_nof_get;
    dnx_data_macsec.egress.soft_expiry_threshold_profiles_nof_get = dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_get;
    dnx_data_macsec.egress.an_bit_nof_get = dnx_data_macsec_egress_an_bit_nof_get;
    dnx_data_macsec.egress.nh_profiles_nof_get = dnx_data_macsec_egress_nh_profiles_nof_get;
    dnx_data_macsec.egress.association_number_actual_range_nof_get = dnx_data_macsec_egress_association_number_actual_range_nof_get;
    dnx_data_macsec.egress.sa_per_sc_nof_get = dnx_data_macsec_egress_sa_per_sc_nof_get;
    dnx_data_macsec.egress.secure_channel_nof_get = dnx_data_macsec_egress_secure_channel_nof_get;

    
    
    
    dnx_data_macsec.wrapper.feature_get = dnx_data_macsec_wrapper_feature_get;

    
    dnx_data_macsec.wrapper.macsec_arb_ports_nof_get = dnx_data_macsec_wrapper_macsec_arb_ports_nof_get;
    dnx_data_macsec.wrapper.per_port_speed_max_mbps_get = dnx_data_macsec_wrapper_per_port_speed_max_mbps_get;
    dnx_data_macsec.wrapper.wrapper_speed_max_mbps_get = dnx_data_macsec_wrapper_wrapper_speed_max_mbps_get;
    dnx_data_macsec.wrapper.framer_ports_offset_get = dnx_data_macsec_wrapper_framer_ports_offset_get;
    dnx_data_macsec.wrapper.tdm_calendar_sbus_slot_get = dnx_data_macsec_wrapper_tdm_calendar_sbus_slot_get;
    dnx_data_macsec.wrapper.tdm_calendar_nof_slots_per_line_get = dnx_data_macsec_wrapper_tdm_calendar_nof_slots_per_line_get;
    dnx_data_macsec.wrapper.tdm_calendar_speed_granularity_get = dnx_data_macsec_wrapper_tdm_calendar_speed_granularity_get;
    dnx_data_macsec.wrapper.nof_tags_get = dnx_data_macsec_wrapper_nof_tags_get;

    
    
    
    
    dnx_data_mdb.global.feature_get = dnx_data_mdb_global_feature_get;

    
    dnx_data_mdb.global.utilization_api_nof_dbs_get = dnx_data_mdb_global_utilization_api_nof_dbs_get;
    dnx_data_mdb.global.adapter_stub_enable_get = dnx_data_mdb_global_adapter_stub_enable_get;

    
    
    
    dnx_data_mdb.dh.feature_get = dnx_data_mdb_dh_feature_get;

    
    dnx_data_mdb.dh.total_nof_macroes_get = dnx_data_mdb_dh_total_nof_macroes_get;
    dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get = dnx_data_mdb_dh_total_nof_macroes_plus_em_ovf_or_eedb_bank_get;
    dnx_data_mdb.dh.total_nof_macroes_plus_data_width_get = dnx_data_mdb_dh_total_nof_macroes_plus_data_width_get;
    dnx_data_mdb.dh.max_nof_cluster_interfaces_get = dnx_data_mdb_dh_max_nof_cluster_interfaces_get;
    dnx_data_mdb.dh.max_nof_clusters_get = dnx_data_mdb_dh_max_nof_clusters_get;
    dnx_data_mdb.dh.max_nof_db_clusters_get = dnx_data_mdb_dh_max_nof_db_clusters_get;
    dnx_data_mdb.dh.nof_bucket_clusters_get = dnx_data_mdb_dh_nof_bucket_clusters_get;
    dnx_data_mdb.dh.nof_buckets_in_macro_get = dnx_data_mdb_dh_nof_buckets_in_macro_get;
    dnx_data_mdb.dh.nof_pair_clusters_get = dnx_data_mdb_dh_nof_pair_clusters_get;
    dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get = dnx_data_mdb_dh_nof_data_rows_per_hitbit_row_get;
    dnx_data_mdb.dh.nof_ddha_blocks_get = dnx_data_mdb_dh_nof_ddha_blocks_get;
    dnx_data_mdb.dh.nof_ddhb_blocks_get = dnx_data_mdb_dh_nof_ddhb_blocks_get;
    dnx_data_mdb.dh.nof_dhc_blocks_get = dnx_data_mdb_dh_nof_dhc_blocks_get;
    dnx_data_mdb.dh.nof_ddhc_blocks_get = dnx_data_mdb_dh_nof_ddhc_blocks_get;
    dnx_data_mdb.dh.nof_ddhx_blocks_get = dnx_data_mdb_dh_nof_ddhx_blocks_get;
    dnx_data_mdb.dh.cluster_row_width_bits_get = dnx_data_mdb_dh_cluster_row_width_bits_get;
    dnx_data_mdb.dh.cluster_row_width_uint32_get = dnx_data_mdb_dh_cluster_row_width_uint32_get;
    dnx_data_mdb.dh.data_out_granularity_get = dnx_data_mdb_dh_data_out_granularity_get;
    dnx_data_mdb.dh.bpu_setup_bb_connected_get = dnx_data_mdb_dh_bpu_setup_bb_connected_get;
    dnx_data_mdb.dh.bpu_setup_size_120_240_120_get = dnx_data_mdb_dh_bpu_setup_size_120_240_120_get;
    dnx_data_mdb.dh.bpu_setup_size_480_odd_get = dnx_data_mdb_dh_bpu_setup_size_480_odd_get;
    dnx_data_mdb.dh.bpu_setup_size_480_get = dnx_data_mdb_dh_bpu_setup_size_480_get;
    dnx_data_mdb.dh.bpu_setup_size_360_120_get = dnx_data_mdb_dh_bpu_setup_size_360_120_get;
    dnx_data_mdb.dh.bpu_setup_size_120_360_get = dnx_data_mdb_dh_bpu_setup_size_120_360_get;
    dnx_data_mdb.dh.bpu_setup_size_240_240_get = dnx_data_mdb_dh_bpu_setup_size_240_240_get;
    dnx_data_mdb.dh.bpu_setup_size_120_120_240_get = dnx_data_mdb_dh_bpu_setup_size_120_120_240_get;
    dnx_data_mdb.dh.bpu_setup_size_240_120_120_get = dnx_data_mdb_dh_bpu_setup_size_240_120_120_get;
    dnx_data_mdb.dh.bpu_setup_size_120_120_120_120_get = dnx_data_mdb_dh_bpu_setup_size_120_120_120_120_get;
    dnx_data_mdb.dh.nof_fec_macro_types_get = dnx_data_mdb_dh_nof_fec_macro_types_get;

    
    dnx_data_mdb.dh.dh_info_get = dnx_data_mdb_dh_dh_info_get;
    dnx_data_mdb.dh.dh_info_info_get = dnx_data_mdb_dh_dh_info_info_get;
    dnx_data_mdb.dh.block_info_get = dnx_data_mdb_dh_block_info_get;
    dnx_data_mdb.dh.block_info_info_get = dnx_data_mdb_dh_block_info_info_get;
    dnx_data_mdb.dh.logical_macro_info_get = dnx_data_mdb_dh_logical_macro_info_get;
    dnx_data_mdb.dh.logical_macro_info_info_get = dnx_data_mdb_dh_logical_macro_info_info_get;
    dnx_data_mdb.dh.macro_type_info_get = dnx_data_mdb_dh_macro_type_info_get;
    dnx_data_mdb.dh.macro_type_info_info_get = dnx_data_mdb_dh_macro_type_info_info_get;
    dnx_data_mdb.dh.mdb_75_macro_halved_get = dnx_data_mdb_dh_mdb_75_macro_halved_get;
    dnx_data_mdb.dh.mdb_75_macro_halved_info_get = dnx_data_mdb_dh_mdb_75_macro_halved_info_get;
    dnx_data_mdb.dh.entry_banks_info_get = dnx_data_mdb_dh_entry_banks_info_get;
    dnx_data_mdb.dh.entry_banks_info_info_get = dnx_data_mdb_dh_entry_banks_info_info_get;
    dnx_data_mdb.dh.mdb_1_info_get = dnx_data_mdb_dh_mdb_1_info_get;
    dnx_data_mdb.dh.mdb_1_info_info_get = dnx_data_mdb_dh_mdb_1_info_info_get;
    dnx_data_mdb.dh.mdb_2_info_get = dnx_data_mdb_dh_mdb_2_info_get;
    dnx_data_mdb.dh.mdb_2_info_info_get = dnx_data_mdb_dh_mdb_2_info_info_get;
    dnx_data_mdb.dh.mdb_3_info_get = dnx_data_mdb_dh_mdb_3_info_get;
    dnx_data_mdb.dh.mdb_3_info_info_get = dnx_data_mdb_dh_mdb_3_info_info_get;
    dnx_data_mdb.dh.mdb_fec_macro_types_info_get = dnx_data_mdb_dh_mdb_fec_macro_types_info_get;
    dnx_data_mdb.dh.mdb_fec_macro_types_info_info_get = dnx_data_mdb_dh_mdb_fec_macro_types_info_info_get;
    dnx_data_mdb.dh.macro_global_indexes_get = dnx_data_mdb_dh_macro_global_indexes_get;
    dnx_data_mdb.dh.macro_global_indexes_info_get = dnx_data_mdb_dh_macro_global_indexes_info_get;
    
    
    dnx_data_mdb.pdbs.feature_get = dnx_data_mdb_pdbs_feature_get;

    
    dnx_data_mdb.pdbs.max_nof_interface_dhs_get = dnx_data_mdb_pdbs_max_nof_interface_dhs_get;
    dnx_data_mdb.pdbs.mesh_mode_support_get = dnx_data_mdb_pdbs_mesh_mode_support_get;
    dnx_data_mdb.pdbs.max_key_size_get = dnx_data_mdb_pdbs_max_key_size_get;
    dnx_data_mdb.pdbs.max_payload_size_get = dnx_data_mdb_pdbs_max_payload_size_get;
    dnx_data_mdb.pdbs.table_mdb_9_mdb_item_1_array_size_get = dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_get;
    dnx_data_mdb.pdbs.table_mdb_9_mdb_item_2_array_size_get = dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_get;
    dnx_data_mdb.pdbs.table_mdb_9_mdb_item_3_array_size_get = dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_get;
    dnx_data_mdb.pdbs.table_mdb_40_mdb_item_0_array_size_get = dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_get;
    dnx_data_mdb.pdbs.table_mdb_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdb_mact_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.mdb_nof_profiles_get = dnx_data_mdb_pdbs_mdb_nof_profiles_get;
    dnx_data_mdb.pdbs.table_mdb_kaps_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdb_ddha_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_ddha_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_ddha_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_ddhab_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_ddhab_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdba_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdba_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdbb_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdbb_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.table_mdbk_dynamic_memory_access_memory_access_field_size_get = dnx_data_mdb_pdbs_table_mdbk_dynamic_memory_access_memory_access_field_size_get;
    dnx_data_mdb.pdbs.adapter_mapping_crps_id_get = dnx_data_mdb_pdbs_adapter_mapping_crps_id_get;

    
    dnx_data_mdb.pdbs.mdb_profile_get = dnx_data_mdb_pdbs_mdb_profile_get;
    dnx_data_mdb.pdbs.mdb_profile_info_get = dnx_data_mdb_pdbs_mdb_profile_info_get;
    dnx_data_mdb.pdbs.mdb_profiles_info_get = dnx_data_mdb_pdbs_mdb_profiles_info_get;
    dnx_data_mdb.pdbs.mdb_profiles_info_info_get = dnx_data_mdb_pdbs_mdb_profiles_info_info_get;
    dnx_data_mdb.pdbs.mdb_profile_kaps_cfg_get = dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_get;
    dnx_data_mdb.pdbs.mdb_profile_kaps_cfg_info_get = dnx_data_mdb_pdbs_mdb_profile_kaps_cfg_info_get;
    dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_get = dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_get;
    dnx_data_mdb.pdbs.mdb_profile_table_str_mapping_info_get = dnx_data_mdb_pdbs_mdb_profile_table_str_mapping_info_get;
    dnx_data_mdb.pdbs.dbal_vmv_str_mapping_get = dnx_data_mdb_pdbs_dbal_vmv_str_mapping_get;
    dnx_data_mdb.pdbs.dbal_vmv_str_mapping_info_get = dnx_data_mdb_pdbs_dbal_vmv_str_mapping_info_get;
    dnx_data_mdb.pdbs.mdb_pdb_info_get = dnx_data_mdb_pdbs_mdb_pdb_info_get;
    dnx_data_mdb.pdbs.mdb_pdb_info_info_get = dnx_data_mdb_pdbs_mdb_pdb_info_info_get;
    dnx_data_mdb.pdbs.dbal_pdb_info_get = dnx_data_mdb_pdbs_dbal_pdb_info_get;
    dnx_data_mdb.pdbs.dbal_pdb_info_info_get = dnx_data_mdb_pdbs_dbal_pdb_info_info_get;
    dnx_data_mdb.pdbs.mdb_adapter_mapping_get = dnx_data_mdb_pdbs_mdb_adapter_mapping_get;
    dnx_data_mdb.pdbs.mdb_adapter_mapping_info_get = dnx_data_mdb_pdbs_mdb_adapter_mapping_info_get;
    dnx_data_mdb.pdbs.fec_physical_dbs_alloc_info_get = dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_get;
    dnx_data_mdb.pdbs.fec_physical_dbs_alloc_info_info_get = dnx_data_mdb_pdbs_fec_physical_dbs_alloc_info_info_get;
    dnx_data_mdb.pdbs.mdb_51_info_get = dnx_data_mdb_pdbs_mdb_51_info_get;
    dnx_data_mdb.pdbs.mdb_51_info_info_get = dnx_data_mdb_pdbs_mdb_51_info_info_get;
    dnx_data_mdb.pdbs.mdb_53_info_get = dnx_data_mdb_pdbs_mdb_53_info_get;
    dnx_data_mdb.pdbs.mdb_53_info_info_get = dnx_data_mdb_pdbs_mdb_53_info_info_get;
    
    
    dnx_data_mdb.em.feature_get = dnx_data_mdb_em_feature_get;

    
    dnx_data_mdb.em.age_row_size_bits_get = dnx_data_mdb_em_age_row_size_bits_get;
    dnx_data_mdb.em.age_ovfcam_row_size_bits_get = dnx_data_mdb_em_age_ovfcam_row_size_bits_get;
    dnx_data_mdb.em.ovf_cam_max_size_get = dnx_data_mdb_em_ovf_cam_max_size_get;
    dnx_data_mdb.em.age_support_per_entry_size_ratio_get = dnx_data_mdb_em_age_support_per_entry_size_ratio_get;
    dnx_data_mdb.em.flush_tcam_rule_counter_support_get = dnx_data_mdb_em_flush_tcam_rule_counter_support_get;
    dnx_data_mdb.em.nof_aging_profiles_get = dnx_data_mdb_em_nof_aging_profiles_get;
    dnx_data_mdb.em.aging_profiles_size_in_bits_get = dnx_data_mdb_em_aging_profiles_size_in_bits_get;
    dnx_data_mdb.em.max_tid_size_get = dnx_data_mdb_em_max_tid_size_get;
    dnx_data_mdb.em.max_nof_tids_get = dnx_data_mdb_em_max_nof_tids_get;
    dnx_data_mdb.em.flush_support_tids_get = dnx_data_mdb_em_flush_support_tids_get;
    dnx_data_mdb.em.flush_max_supported_key_get = dnx_data_mdb_em_flush_max_supported_key_get;
    dnx_data_mdb.em.flush_max_supported_key_plus_payload_get = dnx_data_mdb_em_flush_max_supported_key_plus_payload_get;
    dnx_data_mdb.em.dh_120_entry_encoding_nof_bits_get = dnx_data_mdb_em_dh_120_entry_encoding_nof_bits_get;
    dnx_data_mdb.em.dh_240_entry_encoding_nof_bits_get = dnx_data_mdb_em_dh_240_entry_encoding_nof_bits_get;
    dnx_data_mdb.em.format_granularity_get = dnx_data_mdb_em_format_granularity_get;
    dnx_data_mdb.em.max_nof_vmv_size_get = dnx_data_mdb_em_max_nof_vmv_size_get;
    dnx_data_mdb.em.vmv_nof_values_get = dnx_data_mdb_em_vmv_nof_values_get;
    dnx_data_mdb.em.nof_vmv_size_nof_bits_get = dnx_data_mdb_em_nof_vmv_size_nof_bits_get;
    dnx_data_mdb.em.esem_nof_vmv_size_get = dnx_data_mdb_em_esem_nof_vmv_size_get;
    dnx_data_mdb.em.glem_nof_vmv_size_get = dnx_data_mdb_em_glem_nof_vmv_size_get;
    dnx_data_mdb.em.mact_nof_vmv_size_get = dnx_data_mdb_em_mact_nof_vmv_size_get;
    dnx_data_mdb.em.mact_max_payload_size_get = dnx_data_mdb_em_mact_max_payload_size_get;
    dnx_data_mdb.em.shift_vmv_max_size_get = dnx_data_mdb_em_shift_vmv_max_size_get;
    dnx_data_mdb.em.shift_vmv_max_regs_per_table_get = dnx_data_mdb_em_shift_vmv_max_regs_per_table_get;
    dnx_data_mdb.em.min_nof_app_id_bits_get = dnx_data_mdb_em_min_nof_app_id_bits_get;
    dnx_data_mdb.em.max_nof_spn_sizes_get = dnx_data_mdb_em_max_nof_spn_sizes_get;
    dnx_data_mdb.em.nof_lfsr_sizes_get = dnx_data_mdb_em_nof_lfsr_sizes_get;
    dnx_data_mdb.em.nof_formats_get = dnx_data_mdb_em_nof_formats_get;
    dnx_data_mdb.em.defragmentation_priority_supported_get = dnx_data_mdb_em_defragmentation_priority_supported_get;
    dnx_data_mdb.em.disable_cuckoo_loop_detection_support_get = dnx_data_mdb_em_disable_cuckoo_loop_detection_support_get;
    dnx_data_mdb.em.disable_cuckoo_hit_bit_sync_get = dnx_data_mdb_em_disable_cuckoo_hit_bit_sync_get;
    dnx_data_mdb.em.age_profile_per_ratio_support_get = dnx_data_mdb_em_age_profile_per_ratio_support_get;
    dnx_data_mdb.em.flex_mag_supported_get = dnx_data_mdb_em_flex_mag_supported_get;
    dnx_data_mdb.em.flex_fully_supported_get = dnx_data_mdb_em_flex_fully_supported_get;
    dnx_data_mdb.em.step_table_max_size_get = dnx_data_mdb_em_step_table_max_size_get;
    dnx_data_mdb.em.nof_encoding_values_get = dnx_data_mdb_em_nof_encoding_values_get;
    dnx_data_mdb.em.nof_encoding_types_get = dnx_data_mdb_em_nof_encoding_types_get;
    dnx_data_mdb.em.extra_vmv_shift_registers_get = dnx_data_mdb_em_extra_vmv_shift_registers_get;
    dnx_data_mdb.em.scan_bank_participate_in_cuckoo_support_get = dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_get;
    dnx_data_mdb.em.scan_bank_participate_in_cuckoo_partial_support_get = dnx_data_mdb_em_scan_bank_participate_in_cuckoo_partial_support_get;
    dnx_data_mdb.em.sbus_error_code_support_get = dnx_data_mdb_em_sbus_error_code_support_get;
    dnx_data_mdb.em.nof_emp_tables_get = dnx_data_mdb_em_nof_emp_tables_get;
    dnx_data_mdb.em.defrag_interrupt_support_get = dnx_data_mdb_em_defrag_interrupt_support_get;
    dnx_data_mdb.em.glem_bypass_option_support_get = dnx_data_mdb_em_glem_bypass_option_support_get;
    dnx_data_mdb.em.flush_max_supported_payload_get = dnx_data_mdb_em_flush_max_supported_payload_get;
    dnx_data_mdb.em.flush_payload_max_bus_get = dnx_data_mdb_em_flush_payload_max_bus_get;
    dnx_data_mdb.em.my_mac_ippa_em_dbal_id_get = dnx_data_mdb_em_my_mac_ippa_em_dbal_id_get;
    dnx_data_mdb.em.my_mac_ippf_em_dbal_id_get = dnx_data_mdb_em_my_mac_ippf_em_dbal_id_get;
    dnx_data_mdb.em.mact_dbal_id_get = dnx_data_mdb_em_mact_dbal_id_get;

    
    dnx_data_mdb.em.em_utilization_api_db_translation_get = dnx_data_mdb_em_em_utilization_api_db_translation_get;
    dnx_data_mdb.em.em_utilization_api_db_translation_info_get = dnx_data_mdb_em_em_utilization_api_db_translation_info_get;
    dnx_data_mdb.em.spn_get = dnx_data_mdb_em_spn_get;
    dnx_data_mdb.em.spn_info_get = dnx_data_mdb_em_spn_info_get;
    dnx_data_mdb.em.lfsr_get = dnx_data_mdb_em_lfsr_get;
    dnx_data_mdb.em.lfsr_info_get = dnx_data_mdb_em_lfsr_info_get;
    dnx_data_mdb.em.compare_size_get = dnx_data_mdb_em_compare_size_get;
    dnx_data_mdb.em.compare_size_info_get = dnx_data_mdb_em_compare_size_info_get;
    dnx_data_mdb.em.emp_index_table_mapping_get = dnx_data_mdb_em_emp_index_table_mapping_get;
    dnx_data_mdb.em.emp_index_table_mapping_info_get = dnx_data_mdb_em_emp_index_table_mapping_info_get;
    dnx_data_mdb.em.em_aging_info_get = dnx_data_mdb_em_em_aging_info_get;
    dnx_data_mdb.em.em_aging_info_info_get = dnx_data_mdb_em_em_aging_info_info_get;
    dnx_data_mdb.em.em_aging_cfg_get = dnx_data_mdb_em_em_aging_cfg_get;
    dnx_data_mdb.em.em_aging_cfg_info_get = dnx_data_mdb_em_em_aging_cfg_info_get;
    dnx_data_mdb.em.em_info_get = dnx_data_mdb_em_em_info_get;
    dnx_data_mdb.em.em_info_info_get = dnx_data_mdb_em_em_info_info_get;
    dnx_data_mdb.em.em_interrupt_get = dnx_data_mdb_em_em_interrupt_get;
    dnx_data_mdb.em.em_interrupt_info_get = dnx_data_mdb_em_em_interrupt_info_get;
    dnx_data_mdb.em.step_table_pdb_max_depth_get = dnx_data_mdb_em_step_table_pdb_max_depth_get;
    dnx_data_mdb.em.step_table_pdb_max_depth_info_get = dnx_data_mdb_em_step_table_pdb_max_depth_info_get;
    dnx_data_mdb.em.step_table_max_depth_possible_get = dnx_data_mdb_em_step_table_max_depth_possible_get;
    dnx_data_mdb.em.step_table_max_depth_possible_info_get = dnx_data_mdb_em_step_table_max_depth_possible_info_get;
    dnx_data_mdb.em.step_table_max_depth_nof_ways_ar_get = dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_get;
    dnx_data_mdb.em.step_table_max_depth_nof_ways_ar_info_get = dnx_data_mdb_em_step_table_max_depth_nof_ways_ar_info_get;
    dnx_data_mdb.em.mdb_15_info_get = dnx_data_mdb_em_mdb_15_info_get;
    dnx_data_mdb.em.mdb_15_info_info_get = dnx_data_mdb_em_mdb_15_info_info_get;
    dnx_data_mdb.em.mdb_16_info_get = dnx_data_mdb_em_mdb_16_info_get;
    dnx_data_mdb.em.mdb_16_info_info_get = dnx_data_mdb_em_mdb_16_info_info_get;
    dnx_data_mdb.em.mdb_21_info_get = dnx_data_mdb_em_mdb_21_info_get;
    dnx_data_mdb.em.mdb_21_info_info_get = dnx_data_mdb_em_mdb_21_info_info_get;
    dnx_data_mdb.em.mdb_23_info_get = dnx_data_mdb_em_mdb_23_info_get;
    dnx_data_mdb.em.mdb_23_info_info_get = dnx_data_mdb_em_mdb_23_info_info_get;
    dnx_data_mdb.em.mdb_24_info_get = dnx_data_mdb_em_mdb_24_info_get;
    dnx_data_mdb.em.mdb_24_info_info_get = dnx_data_mdb_em_mdb_24_info_info_get;
    dnx_data_mdb.em.mdb_29_info_get = dnx_data_mdb_em_mdb_29_info_get;
    dnx_data_mdb.em.mdb_29_info_info_get = dnx_data_mdb_em_mdb_29_info_info_get;
    dnx_data_mdb.em.mdb_31_info_get = dnx_data_mdb_em_mdb_31_info_get;
    dnx_data_mdb.em.mdb_31_info_info_get = dnx_data_mdb_em_mdb_31_info_info_get;
    dnx_data_mdb.em.mdb_32_info_get = dnx_data_mdb_em_mdb_32_info_get;
    dnx_data_mdb.em.mdb_32_info_info_get = dnx_data_mdb_em_mdb_32_info_info_get;
    dnx_data_mdb.em.mdb_41_info_get = dnx_data_mdb_em_mdb_41_info_get;
    dnx_data_mdb.em.mdb_41_info_info_get = dnx_data_mdb_em_mdb_41_info_info_get;
    dnx_data_mdb.em.mdb_45_info_get = dnx_data_mdb_em_mdb_45_info_get;
    dnx_data_mdb.em.mdb_45_info_info_get = dnx_data_mdb_em_mdb_45_info_info_get;
    dnx_data_mdb.em.mdb_emp_tables_mapping_get = dnx_data_mdb_em_mdb_emp_tables_mapping_get;
    dnx_data_mdb.em.mdb_emp_tables_mapping_info_get = dnx_data_mdb_em_mdb_emp_tables_mapping_info_get;
    dnx_data_mdb.em.mdb_em_shift_vmv_regs_get = dnx_data_mdb_em_mdb_em_shift_vmv_regs_get;
    dnx_data_mdb.em.mdb_em_shift_vmv_regs_info_get = dnx_data_mdb_em_mdb_em_shift_vmv_regs_info_get;
    dnx_data_mdb.em.em_emp_get = dnx_data_mdb_em_em_emp_get;
    dnx_data_mdb.em.em_emp_info_get = dnx_data_mdb_em_em_emp_info_get;
    dnx_data_mdb.em.mdb_step_table_map_get = dnx_data_mdb_em_mdb_step_table_map_get;
    dnx_data_mdb.em.mdb_step_table_map_info_get = dnx_data_mdb_em_mdb_step_table_map_info_get;
    dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get = dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_get;
    dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_info_get = dnx_data_mdb_em_mdb_em_dbal_phy_tables_info_info_get;
    dnx_data_mdb.em.filter_rules_get = dnx_data_mdb_em_filter_rules_get;
    dnx_data_mdb.em.filter_rules_info_get = dnx_data_mdb_em_filter_rules_info_get;
    dnx_data_mdb.em.data_rules_get = dnx_data_mdb_em_data_rules_get;
    dnx_data_mdb.em.data_rules_info_get = dnx_data_mdb_em_data_rules_info_get;
    
    
    dnx_data_mdb.direct.feature_get = dnx_data_mdb_direct_feature_get;

    
    dnx_data_mdb.direct.physical_address_max_bits_get = dnx_data_mdb_direct_physical_address_max_bits_get;
    dnx_data_mdb.direct.physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get = dnx_data_mdb_direct_physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get;
    dnx_data_mdb.direct.vsi_physical_address_max_bits_get = dnx_data_mdb_direct_vsi_physical_address_max_bits_get;
    dnx_data_mdb.direct.fec_row_width_get = dnx_data_mdb_direct_fec_row_width_get;
    dnx_data_mdb.direct.fec_address_mapping_bits_get = dnx_data_mdb_direct_fec_address_mapping_bits_get;
    dnx_data_mdb.direct.fec_address_mapping_bits_relevant_get = dnx_data_mdb_direct_fec_address_mapping_bits_relevant_get;
    dnx_data_mdb.direct.fec_max_cluster_pairs_get = dnx_data_mdb_direct_fec_max_cluster_pairs_get;
    dnx_data_mdb.direct.max_payload_size_get = dnx_data_mdb_direct_max_payload_size_get;
    dnx_data_mdb.direct.nof_fecs_in_super_fec_get = dnx_data_mdb_direct_nof_fecs_in_super_fec_get;
    dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get = dnx_data_mdb_direct_nof_fec_ecmp_hierarchies_get;
    dnx_data_mdb.direct.physical_address_ovf_enable_val_get = dnx_data_mdb_direct_physical_address_ovf_enable_val_get;
    dnx_data_mdb.direct.nof_fec_dbs_get = dnx_data_mdb_direct_nof_fec_dbs_get;
    dnx_data_mdb.direct.map_payload_size_enum_get = dnx_data_mdb_direct_map_payload_size_enum_get;
    dnx_data_mdb.direct.map_payload_size_get = dnx_data_mdb_direct_map_payload_size_get;
    dnx_data_mdb.direct.fec_allocation_base_macro_type_get = dnx_data_mdb_direct_fec_allocation_base_macro_type_get;

    
    dnx_data_mdb.direct.vsi_payload_type_per_range_get = dnx_data_mdb_direct_vsi_payload_type_per_range_get;
    dnx_data_mdb.direct.vsi_payload_type_per_range_info_get = dnx_data_mdb_direct_vsi_payload_type_per_range_info_get;
    dnx_data_mdb.direct.direct_utilization_api_db_translation_get = dnx_data_mdb_direct_direct_utilization_api_db_translation_get;
    dnx_data_mdb.direct.direct_utilization_api_db_translation_info_get = dnx_data_mdb_direct_direct_utilization_api_db_translation_info_get;
    dnx_data_mdb.direct.direct_info_get = dnx_data_mdb_direct_direct_info_get;
    dnx_data_mdb.direct.direct_info_info_get = dnx_data_mdb_direct_direct_info_info_get;
    dnx_data_mdb.direct.hit_bit_pos_in_abk_get = dnx_data_mdb_direct_hit_bit_pos_in_abk_get;
    dnx_data_mdb.direct.hit_bit_pos_in_abk_info_get = dnx_data_mdb_direct_hit_bit_pos_in_abk_info_get;
    dnx_data_mdb.direct.mdb_11_info_get = dnx_data_mdb_direct_mdb_11_info_get;
    dnx_data_mdb.direct.mdb_11_info_info_get = dnx_data_mdb_direct_mdb_11_info_info_get;
    dnx_data_mdb.direct.mdb_26_info_get = dnx_data_mdb_direct_mdb_26_info_get;
    dnx_data_mdb.direct.mdb_26_info_info_get = dnx_data_mdb_direct_mdb_26_info_info_get;
    dnx_data_mdb.direct.mdb_direct_mdb_phy_tables_info_get = dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_get;
    dnx_data_mdb.direct.mdb_direct_mdb_phy_tables_info_info_get = dnx_data_mdb_direct_mdb_direct_mdb_phy_tables_info_info_get;
    
    
    dnx_data_mdb.eedb.feature_get = dnx_data_mdb_eedb_feature_get;

    
    dnx_data_mdb.eedb.phase_map_bits_get = dnx_data_mdb_eedb_phase_map_bits_get;
    dnx_data_mdb.eedb.phase_map_array_size_get = dnx_data_mdb_eedb_phase_map_array_size_get;
    dnx_data_mdb.eedb.nof_phase_per_mag_get = dnx_data_mdb_eedb_nof_phase_per_mag_get;
    dnx_data_mdb.eedb.nof_phases_get = dnx_data_mdb_eedb_nof_phases_get;
    dnx_data_mdb.eedb.nof_eedb_mags_get = dnx_data_mdb_eedb_nof_eedb_mags_get;
    dnx_data_mdb.eedb.nof_eedb_banks_get = dnx_data_mdb_eedb_nof_eedb_banks_get;
    dnx_data_mdb.eedb.nof_phases_per_eedb_bank_get = dnx_data_mdb_eedb_nof_phases_per_eedb_bank_get;
    dnx_data_mdb.eedb.nof_phases_per_eedb_bank_size_get = dnx_data_mdb_eedb_nof_phases_per_eedb_bank_size_get;
    dnx_data_mdb.eedb.phase_bank_select_default_get = dnx_data_mdb_eedb_phase_bank_select_default_get;
    dnx_data_mdb.eedb.entry_format_bits_get = dnx_data_mdb_eedb_entry_format_bits_get;
    dnx_data_mdb.eedb.entry_format_encoding_bits_get = dnx_data_mdb_eedb_entry_format_encoding_bits_get;
    dnx_data_mdb.eedb.bank_id_bits_get = dnx_data_mdb_eedb_bank_id_bits_get;
    dnx_data_mdb.eedb.entry_bank_get = dnx_data_mdb_eedb_entry_bank_get;
    dnx_data_mdb.eedb.abk_bank_get = dnx_data_mdb_eedb_abk_bank_get;

    
    dnx_data_mdb.eedb.phase_info_get = dnx_data_mdb_eedb_phase_info_get;
    dnx_data_mdb.eedb.phase_info_info_get = dnx_data_mdb_eedb_phase_info_info_get;
    dnx_data_mdb.eedb.phase_ll_bucket_enable_get = dnx_data_mdb_eedb_phase_ll_bucket_enable_get;
    dnx_data_mdb.eedb.phase_ll_bucket_enable_info_get = dnx_data_mdb_eedb_phase_ll_bucket_enable_info_get;
    dnx_data_mdb.eedb.phase_to_table_get = dnx_data_mdb_eedb_phase_to_table_get;
    dnx_data_mdb.eedb.phase_to_table_info_get = dnx_data_mdb_eedb_phase_to_table_info_get;
    dnx_data_mdb.eedb.outlif_physical_phase_granularity_get = dnx_data_mdb_eedb_outlif_physical_phase_granularity_get;
    dnx_data_mdb.eedb.outlif_physical_phase_granularity_info_get = dnx_data_mdb_eedb_outlif_physical_phase_granularity_info_get;
    
    
    dnx_data_mdb.kaps.feature_get = dnx_data_mdb_kaps_feature_get;

    
    dnx_data_mdb.kaps.nof_dbs_get = dnx_data_mdb_kaps_nof_dbs_get;
    dnx_data_mdb.kaps.nof_small_bbs_get = dnx_data_mdb_kaps_nof_small_bbs_get;
    dnx_data_mdb.kaps.nof_rows_in_rpb_max_get = dnx_data_mdb_kaps_nof_rows_in_rpb_max_get;
    dnx_data_mdb.kaps.nof_rows_in_small_ads_max_get = dnx_data_mdb_kaps_nof_rows_in_small_ads_max_get;
    dnx_data_mdb.kaps.nof_rows_in_small_bb_max_get = dnx_data_mdb_kaps_nof_rows_in_small_bb_max_get;
    dnx_data_mdb.kaps.nof_rows_in_small_rpb_hitbits_max_get = dnx_data_mdb_kaps_nof_rows_in_small_rpb_hitbits_max_get;
    dnx_data_mdb.kaps.nof_big_bbs_blk_ids_get = dnx_data_mdb_kaps_nof_big_bbs_blk_ids_get;
    dnx_data_mdb.kaps.nof_bytes_in_hitbit_row_get = dnx_data_mdb_kaps_nof_bytes_in_hitbit_row_get;
    dnx_data_mdb.kaps.key_lsn_bits_get = dnx_data_mdb_kaps_key_lsn_bits_get;
    dnx_data_mdb.kaps.key_prefix_length_get = dnx_data_mdb_kaps_key_prefix_length_get;
    dnx_data_mdb.kaps.max_prefix_in_bb_row_get = dnx_data_mdb_kaps_max_prefix_in_bb_row_get;
    dnx_data_mdb.kaps.key_width_in_bits_get = dnx_data_mdb_kaps_key_width_in_bits_get;
    dnx_data_mdb.kaps.bb_byte_width_get = dnx_data_mdb_kaps_bb_byte_width_get;
    dnx_data_mdb.kaps.rpb_byte_width_get = dnx_data_mdb_kaps_rpb_byte_width_get;
    dnx_data_mdb.kaps.ads_byte_width_get = dnx_data_mdb_kaps_ads_byte_width_get;
    dnx_data_mdb.kaps.direct_byte_width_get = dnx_data_mdb_kaps_direct_byte_width_get;
    dnx_data_mdb.kaps.rpb_valid_bits_get = dnx_data_mdb_kaps_rpb_valid_bits_get;
    dnx_data_mdb.kaps.max_fmt_get = dnx_data_mdb_kaps_max_fmt_get;
    dnx_data_mdb.kaps.nof_app_ids_get = dnx_data_mdb_kaps_nof_app_ids_get;
    dnx_data_mdb.kaps.max_nof_result_types_get = dnx_data_mdb_kaps_max_nof_result_types_get;
    dnx_data_mdb.kaps.rpb_field_get = dnx_data_mdb_kaps_rpb_field_get;
    dnx_data_mdb.kaps.big_kaps_revision_val_get = dnx_data_mdb_kaps_big_kaps_revision_val_get;
    dnx_data_mdb.kaps.small_kaps_ad_size_e_get = dnx_data_mdb_kaps_small_kaps_ad_size_e_get;
    dnx_data_mdb.kaps.mdb_lpm_blackhole_mode_enable_get = dnx_data_mdb_kaps_mdb_lpm_blackhole_mode_enable_get;

    
    dnx_data_mdb.kaps.big_bb_blk_id_mapping_get = dnx_data_mdb_kaps_big_bb_blk_id_mapping_get;
    dnx_data_mdb.kaps.big_bb_blk_id_mapping_info_get = dnx_data_mdb_kaps_big_bb_blk_id_mapping_info_get;
    dnx_data_mdb.kaps.db_info_get = dnx_data_mdb_kaps_db_info_get;
    dnx_data_mdb.kaps.db_info_info_get = dnx_data_mdb_kaps_db_info_info_get;
    dnx_data_mdb.kaps.db_supported_ad_sizes_get = dnx_data_mdb_kaps_db_supported_ad_sizes_get;
    dnx_data_mdb.kaps.db_supported_ad_sizes_info_get = dnx_data_mdb_kaps_db_supported_ad_sizes_info_get;
    dnx_data_mdb.kaps.db_access_info_get = dnx_data_mdb_kaps_db_access_info_get;
    dnx_data_mdb.kaps.db_access_info_info_get = dnx_data_mdb_kaps_db_access_info_info_get;
    dnx_data_mdb.kaps.mdb_28_info_get = dnx_data_mdb_kaps_mdb_28_info_get;
    dnx_data_mdb.kaps.mdb_28_info_info_get = dnx_data_mdb_kaps_mdb_28_info_info_get;
    dnx_data_mdb.kaps.mdb_53_info_get = dnx_data_mdb_kaps_mdb_53_info_get;
    dnx_data_mdb.kaps.mdb_53_info_info_get = dnx_data_mdb_kaps_mdb_53_info_info_get;
    dnx_data_mdb.kaps.kaps_lookup_result_get = dnx_data_mdb_kaps_kaps_lookup_result_get;
    dnx_data_mdb.kaps.kaps_lookup_result_info_get = dnx_data_mdb_kaps_kaps_lookup_result_info_get;
    
    
    dnx_data_mdb.feature.feature_get = dnx_data_mdb_feature_feature_get;

    

    
    
    
    
    dnx_data_meter.general.feature_get = dnx_data_meter_general_feature_get;

    

    
    
    
    dnx_data_meter.profile.feature_get = dnx_data_meter_profile_feature_get;

    
    dnx_data_meter.profile.large_bucket_mode_exp_add_get = dnx_data_meter_profile_large_bucket_mode_exp_add_get;
    dnx_data_meter.profile.max_regular_bucket_mode_burst_get = dnx_data_meter_profile_max_regular_bucket_mode_burst_get;
    dnx_data_meter.profile.rev_exp_min_get = dnx_data_meter_profile_rev_exp_min_get;
    dnx_data_meter.profile.rev_exp_max_get = dnx_data_meter_profile_rev_exp_max_get;
    dnx_data_meter.profile.burst_size_min_get = dnx_data_meter_profile_burst_size_min_get;
    dnx_data_meter.profile.nof_valid_global_meters_get = dnx_data_meter_profile_nof_valid_global_meters_get;
    dnx_data_meter.profile.nof_dp_command_max_get = dnx_data_meter_profile_nof_dp_command_max_get;
    dnx_data_meter.profile.global_engine_hw_stat_database_id_get = dnx_data_meter_profile_global_engine_hw_stat_database_id_get;
    dnx_data_meter.profile.ir_mant_nof_bits_get = dnx_data_meter_profile_ir_mant_nof_bits_get;

    
    
    
    dnx_data_meter.diag.feature_get = dnx_data_meter_diag_feature_get;

    

    
    
    
    dnx_data_meter.meter_db.feature_get = dnx_data_meter_meter_db_feature_get;

    
    dnx_data_meter.meter_db.nof_ingress_db_get = dnx_data_meter_meter_db_nof_ingress_db_get;
    dnx_data_meter.meter_db.nof_egress_db_get = dnx_data_meter_meter_db_nof_egress_db_get;
    dnx_data_meter.meter_db.nof_meters_id_in_group_get = dnx_data_meter_meter_db_nof_meters_id_in_group_get;

    
    
    
    dnx_data_meter.mem_mgmt.feature_get = dnx_data_meter_mem_mgmt_feature_get;

    
    dnx_data_meter.mem_mgmt.adrress_range_resolution_get = dnx_data_meter_mem_mgmt_adrress_range_resolution_get;
    dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get = dnx_data_meter_mem_mgmt_object_stat_pointer_base_resolution_get;
    dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get = dnx_data_meter_mem_mgmt_base_big_engine_for_meter_get;
    dnx_data_meter.mem_mgmt.base_mid_engine_for_meter_get = dnx_data_meter_mem_mgmt_base_mid_engine_for_meter_get;
    dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get = dnx_data_meter_mem_mgmt_ingress_single_bucket_engine_get;
    dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get = dnx_data_meter_mem_mgmt_egress_single_bucket_engine_get;
    dnx_data_meter.mem_mgmt.meter_pointer_size_get = dnx_data_meter_mem_mgmt_meter_pointer_size_get;
    dnx_data_meter.mem_mgmt.meter_pointer_map_size_get = dnx_data_meter_mem_mgmt_meter_pointer_map_size_get;
    dnx_data_meter.mem_mgmt.invalid_bank_id_get = dnx_data_meter_mem_mgmt_invalid_bank_id_get;
    dnx_data_meter.mem_mgmt.default_bank_id_get = dnx_data_meter_mem_mgmt_default_bank_id_get;
    dnx_data_meter.mem_mgmt.ptr_map_table_resolution_get = dnx_data_meter_mem_mgmt_ptr_map_table_resolution_get;
    dnx_data_meter.mem_mgmt.bank_id_size_get = dnx_data_meter_mem_mgmt_bank_id_size_get;
    dnx_data_meter.mem_mgmt.nof_bank_ids_get = dnx_data_meter_mem_mgmt_nof_bank_ids_get;
    dnx_data_meter.mem_mgmt.sections_offset_get = dnx_data_meter_mem_mgmt_sections_offset_get;
    dnx_data_meter.mem_mgmt.nof_banks_in_big_engine_get = dnx_data_meter_mem_mgmt_nof_banks_in_big_engine_get;
    dnx_data_meter.mem_mgmt.nof_single_buckets_per_entry_get = dnx_data_meter_mem_mgmt_nof_single_buckets_per_entry_get;
    dnx_data_meter.mem_mgmt.nof_dual_buckets_per_entry_get = dnx_data_meter_mem_mgmt_nof_dual_buckets_per_entry_get;
    dnx_data_meter.mem_mgmt.nof_profiles_per_entry_get = dnx_data_meter_mem_mgmt_nof_profiles_per_entry_get;
    dnx_data_meter.mem_mgmt.nof_buckets_in_small_bank_get = dnx_data_meter_mem_mgmt_nof_buckets_in_small_bank_get;
    dnx_data_meter.mem_mgmt.nof_buckets_in_big_bank_get = dnx_data_meter_mem_mgmt_nof_buckets_in_big_bank_get;
    dnx_data_meter.mem_mgmt.bucket_size_nof_bits_get = dnx_data_meter_mem_mgmt_bucket_size_nof_bits_get;

    
    
    
    dnx_data_meter.compensation.feature_get = dnx_data_meter_compensation_feature_get;

    
    dnx_data_meter.compensation.nof_delta_compensation_profiles_get = dnx_data_meter_compensation_nof_delta_compensation_profiles_get;

    
    dnx_data_meter.compensation.per_ingress_pp_port_delta_range_get = dnx_data_meter_compensation_per_ingress_pp_port_delta_range_get;
    dnx_data_meter.compensation.per_ingress_pp_port_delta_range_info_get = dnx_data_meter_compensation_per_ingress_pp_port_delta_range_info_get;
    dnx_data_meter.compensation.per_meter_delta_range_get = dnx_data_meter_compensation_per_meter_delta_range_get;
    dnx_data_meter.compensation.per_meter_delta_range_info_get = dnx_data_meter_compensation_per_meter_delta_range_info_get;
    
    
    dnx_data_meter.expansion.feature_get = dnx_data_meter_expansion_feature_get;

    
    dnx_data_meter.expansion.max_size_get = dnx_data_meter_expansion_max_size_get;
    dnx_data_meter.expansion.nof_expansion_sets_get = dnx_data_meter_expansion_nof_expansion_sets_get;
    dnx_data_meter.expansion.max_meter_set_size_get = dnx_data_meter_expansion_max_meter_set_size_get;
    dnx_data_meter.expansion.max_meter_set_base_get = dnx_data_meter_expansion_max_meter_set_base_get;
    dnx_data_meter.expansion.ingress_metadata_size_get = dnx_data_meter_expansion_ingress_metadata_size_get;
    dnx_data_meter.expansion.egress_metadata_size_get = dnx_data_meter_expansion_egress_metadata_size_get;
    dnx_data_meter.expansion.expansion_size_get = dnx_data_meter_expansion_expansion_size_get;
    dnx_data_meter.expansion.expansion_offset_size_get = dnx_data_meter_expansion_expansion_offset_size_get;

    
    dnx_data_meter.expansion.source_metadata_types_get = dnx_data_meter_expansion_source_metadata_types_get;
    dnx_data_meter.expansion.source_metadata_types_info_get = dnx_data_meter_expansion_source_metadata_types_info_get;
    
    
    
    dnx_data_mib.general.feature_get = dnx_data_mib_general_feature_get;

    
    dnx_data_mib.general.nof_express_rows_per_cdmib_port_get = dnx_data_mib_general_nof_express_rows_per_cdmib_port_get;
    dnx_data_mib.general.nof_preempt_rows_per_cdmib_port_get = dnx_data_mib_general_nof_preempt_rows_per_cdmib_port_get;
    dnx_data_mib.general.nof_rows_per_cdmib_port_get = dnx_data_mib_general_nof_rows_per_cdmib_port_get;
    dnx_data_mib.general.nof_cdmib_port_per_pm_get = dnx_data_mib_general_nof_cdmib_port_per_pm_get;
    dnx_data_mib.general.nof_cdmib_port_get = dnx_data_mib_general_nof_cdmib_port_get;
    dnx_data_mib.general.stat_interval_get = dnx_data_mib_general_stat_interval_get;
    dnx_data_mib.general.stat_jumbo_get = dnx_data_mib_general_stat_jumbo_get;

    
    dnx_data_mib.general.stat_pbmp_get = dnx_data_mib_general_stat_pbmp_get;
    dnx_data_mib.general.stat_pbmp_info_get = dnx_data_mib_general_stat_pbmp_info_get;
    
    
    dnx_data_mib.controlled_counters.feature_get = dnx_data_mib_controlled_counters_feature_get;

    
    dnx_data_mib.controlled_counters.flexmac_mib_general_cnt_len_get = dnx_data_mib_controlled_counters_flexmac_mib_general_cnt_len_get;
    dnx_data_mib.controlled_counters.flexmac_mib_rbyt_cnt_len_get = dnx_data_mib_controlled_counters_flexmac_mib_rbyt_cnt_len_get;
    dnx_data_mib.controlled_counters.flexmac_mib_rrbyt_cnt_len_get = dnx_data_mib_controlled_counters_flexmac_mib_rrbyt_cnt_len_get;
    dnx_data_mib.controlled_counters.flexmac_mib_tbyt_cnt_len_get = dnx_data_mib_controlled_counters_flexmac_mib_tbyt_cnt_len_get;

    
    dnx_data_mib.controlled_counters.map_data_get = dnx_data_mib_controlled_counters_map_data_get;
    dnx_data_mib.controlled_counters.map_data_info_get = dnx_data_mib_controlled_counters_map_data_info_get;
    dnx_data_mib.controlled_counters.set_data_get = dnx_data_mib_controlled_counters_set_data_get;
    dnx_data_mib.controlled_counters.set_data_info_get = dnx_data_mib_controlled_counters_set_data_info_get;
    dnx_data_mib.controlled_counters.counter_data_get = dnx_data_mib_controlled_counters_counter_data_get;
    dnx_data_mib.controlled_counters.counter_data_info_get = dnx_data_mib_controlled_counters_counter_data_info_get;
    dnx_data_mib.controlled_counters.flexmac_counter_data_get = dnx_data_mib_controlled_counters_flexmac_counter_data_get;
    dnx_data_mib.controlled_counters.flexmac_counter_data_info_get = dnx_data_mib_controlled_counters_flexmac_counter_data_info_get;
    
    
    
    dnx_data_module_testing.unsupported.feature_get = dnx_data_module_testing_unsupported_feature_get;

    
    dnx_data_module_testing.unsupported.supported_def_get = dnx_data_module_testing_unsupported_supported_def_get;
    dnx_data_module_testing.unsupported.unsupported_def_get = dnx_data_module_testing_unsupported_unsupported_def_get;

    
    dnx_data_module_testing.unsupported.supported_table_get = dnx_data_module_testing_unsupported_supported_table_get;
    dnx_data_module_testing.unsupported.supported_table_info_get = dnx_data_module_testing_unsupported_supported_table_info_get;
    dnx_data_module_testing.unsupported.unsupported_table_get = dnx_data_module_testing_unsupported_unsupported_table_get;
    dnx_data_module_testing.unsupported.unsupported_table_info_get = dnx_data_module_testing_unsupported_unsupported_table_info_get;
    
    
    dnx_data_module_testing.standard_submodule.feature_get = dnx_data_module_testing_standard_submodule_feature_get;

    
    dnx_data_module_testing.standard_submodule.standard_define_get = dnx_data_module_testing_standard_submodule_standard_define_get;

    dnx_data_module_testing.standard_submodule.define_family_dnx2_get = dnx_data_module_testing_standard_submodule_define_family_dnx2_get;

    dnx_data_module_testing.standard_submodule.standard_numeric_get = dnx_data_module_testing_standard_submodule_standard_numeric_get;

    dnx_data_module_testing.standard_submodule.num_family_dnx2_get = dnx_data_module_testing_standard_submodule_num_family_dnx2_get;


    dnx_data_module_testing.standard_submodule.num_property_family_dnx2_get = dnx_data_module_testing_standard_submodule_num_property_family_dnx2_get;


    
    dnx_data_module_testing.standard_submodule.standard_table_get = dnx_data_module_testing_standard_submodule_standard_table_get;
    dnx_data_module_testing.standard_submodule.standard_table_info_get = dnx_data_module_testing_standard_submodule_standard_table_info_get;

    dnx_data_module_testing.standard_submodule.table_family_dnx2_get = dnx_data_module_testing_standard_submodule_table_family_dnx2_get;
    dnx_data_module_testing.standard_submodule.table_family_dnx2_info_get = dnx_data_module_testing_standard_submodule_table_family_dnx2_info_get;

    
    
    dnx_data_module_testing.types.feature_get = dnx_data_module_testing_types_feature_get;

    
    dnx_data_module_testing.types.def_signed_get = dnx_data_module_testing_types_def_signed_get;
    dnx_data_module_testing.types.def_unsigned_get = dnx_data_module_testing_types_def_unsigned_get;
    dnx_data_module_testing.types.num_signed_get = dnx_data_module_testing_types_num_signed_get;
    dnx_data_module_testing.types.num_unsigned_get = dnx_data_module_testing_types_num_unsigned_get;

    
    dnx_data_module_testing.types.all_get = dnx_data_module_testing_types_all_get;
    dnx_data_module_testing.types.all_info_get = dnx_data_module_testing_types_all_info_get;
    
    
    dnx_data_module_testing.property_methods.feature_get = dnx_data_module_testing_property_methods_feature_get;

    
    dnx_data_module_testing.property_methods.numeric_range_get = dnx_data_module_testing_property_methods_numeric_range_get;

    
    dnx_data_module_testing.property_methods.enable_get = dnx_data_module_testing_property_methods_enable_get;
    dnx_data_module_testing.property_methods.enable_info_get = dnx_data_module_testing_property_methods_enable_info_get;
    dnx_data_module_testing.property_methods.port_enable_get = dnx_data_module_testing_property_methods_port_enable_get;
    dnx_data_module_testing.property_methods.port_enable_info_get = dnx_data_module_testing_property_methods_port_enable_info_get;
    dnx_data_module_testing.property_methods.suffix_enable_get = dnx_data_module_testing_property_methods_suffix_enable_get;
    dnx_data_module_testing.property_methods.suffix_enable_info_get = dnx_data_module_testing_property_methods_suffix_enable_info_get;
    dnx_data_module_testing.property_methods.range_get = dnx_data_module_testing_property_methods_range_get;
    dnx_data_module_testing.property_methods.range_info_get = dnx_data_module_testing_property_methods_range_info_get;
    dnx_data_module_testing.property_methods.port_range_get = dnx_data_module_testing_property_methods_port_range_get;
    dnx_data_module_testing.property_methods.port_range_info_get = dnx_data_module_testing_property_methods_port_range_info_get;
    dnx_data_module_testing.property_methods.suffix_range_get = dnx_data_module_testing_property_methods_suffix_range_get;
    dnx_data_module_testing.property_methods.suffix_range_info_get = dnx_data_module_testing_property_methods_suffix_range_info_get;
    dnx_data_module_testing.property_methods.direct_map_get = dnx_data_module_testing_property_methods_direct_map_get;
    dnx_data_module_testing.property_methods.direct_map_info_get = dnx_data_module_testing_property_methods_direct_map_info_get;
    dnx_data_module_testing.property_methods.port_direct_map_get = dnx_data_module_testing_property_methods_port_direct_map_get;
    dnx_data_module_testing.property_methods.port_direct_map_info_get = dnx_data_module_testing_property_methods_port_direct_map_info_get;
    dnx_data_module_testing.property_methods.suffix_direct_map_get = dnx_data_module_testing_property_methods_suffix_direct_map_get;
    dnx_data_module_testing.property_methods.suffix_direct_map_info_get = dnx_data_module_testing_property_methods_suffix_direct_map_info_get;
    dnx_data_module_testing.property_methods.custom_get = dnx_data_module_testing_property_methods_custom_get;
    dnx_data_module_testing.property_methods.custom_info_get = dnx_data_module_testing_property_methods_custom_info_get;
    dnx_data_module_testing.property_methods.pbmp_get = dnx_data_module_testing_property_methods_pbmp_get;
    dnx_data_module_testing.property_methods.pbmp_info_get = dnx_data_module_testing_property_methods_pbmp_info_get;
    dnx_data_module_testing.property_methods.port_pbmp_get = dnx_data_module_testing_property_methods_port_pbmp_get;
    dnx_data_module_testing.property_methods.port_pbmp_info_get = dnx_data_module_testing_property_methods_port_pbmp_info_get;
    dnx_data_module_testing.property_methods.suffix_pbmp_get = dnx_data_module_testing_property_methods_suffix_pbmp_get;
    dnx_data_module_testing.property_methods.suffix_pbmp_info_get = dnx_data_module_testing_property_methods_suffix_pbmp_info_get;
    dnx_data_module_testing.property_methods.str_get = dnx_data_module_testing_property_methods_str_get;
    dnx_data_module_testing.property_methods.str_info_get = dnx_data_module_testing_property_methods_str_info_get;
    dnx_data_module_testing.property_methods.port_str_get = dnx_data_module_testing_property_methods_port_str_get;
    dnx_data_module_testing.property_methods.port_str_info_get = dnx_data_module_testing_property_methods_port_str_info_get;
    dnx_data_module_testing.property_methods.suffix_str_get = dnx_data_module_testing_property_methods_suffix_str_get;
    dnx_data_module_testing.property_methods.suffix_str_info_get = dnx_data_module_testing_property_methods_suffix_str_info_get;
    
    
    dnx_data_module_testing.data_pointers.feature_get = dnx_data_module_testing_data_pointers_feature_get;

    
    dnx_data_module_testing.data_pointers.cores_get = dnx_data_module_testing_data_pointers_cores_get;
    dnx_data_module_testing.data_pointers.add_cores_get = dnx_data_module_testing_data_pointers_add_cores_get;
    dnx_data_module_testing.data_pointers.mul_cores_get = dnx_data_module_testing_data_pointers_mul_cores_get;
    dnx_data_module_testing.data_pointers.div_cores_get = dnx_data_module_testing_data_pointers_div_cores_get;
    dnx_data_module_testing.data_pointers.op_cores_get = dnx_data_module_testing_data_pointers_op_cores_get;
    dnx_data_module_testing.data_pointers.num_cores_get = dnx_data_module_testing_data_pointers_num_cores_get;
    dnx_data_module_testing.data_pointers.num_add_cores_get = dnx_data_module_testing_data_pointers_num_add_cores_get;
    dnx_data_module_testing.data_pointers.num_mul_cores_get = dnx_data_module_testing_data_pointers_num_mul_cores_get;
    dnx_data_module_testing.data_pointers.num_div_cores_get = dnx_data_module_testing_data_pointers_num_div_cores_get;
    dnx_data_module_testing.data_pointers.num_op_cores_get = dnx_data_module_testing_data_pointers_num_op_cores_get;
    dnx_data_module_testing.data_pointers.num_last_cores_get = dnx_data_module_testing_data_pointers_num_last_cores_get;
    dnx_data_module_testing.data_pointers.num_table_cores_get = dnx_data_module_testing_data_pointers_num_table_cores_get;

    
    dnx_data_module_testing.data_pointers.table_pointers_get = dnx_data_module_testing_data_pointers_table_pointers_get;
    dnx_data_module_testing.data_pointers.table_pointers_info_get = dnx_data_module_testing_data_pointers_table_pointers_info_get;
    
    
    dnx_data_module_testing.dbal.feature_get = dnx_data_module_testing_dbal_feature_get;

    
    dnx_data_module_testing.dbal.vrf_field_size_get = dnx_data_module_testing_dbal_vrf_field_size_get;
    dnx_data_module_testing.dbal.field_define_example_get = dnx_data_module_testing_dbal_field_define_example_get;
    dnx_data_module_testing.dbal.res_max_val_get = dnx_data_module_testing_dbal_res_max_val_get;
    dnx_data_module_testing.dbal.res_field_size_get = dnx_data_module_testing_dbal_res_field_size_get;

    
    dnx_data_module_testing.dbal.all_get = dnx_data_module_testing_dbal_all_get;
    dnx_data_module_testing.dbal.all_info_get = dnx_data_module_testing_dbal_all_info_get;
    
    
    dnx_data_module_testing.example_tests.feature_get = dnx_data_module_testing_example_tests_feature_get;

    
    dnx_data_module_testing.example_tests.field_size_get = dnx_data_module_testing_example_tests_field_size_get;
    dnx_data_module_testing.example_tests.large_nof_elements_get = dnx_data_module_testing_example_tests_large_nof_elements_get;
    dnx_data_module_testing.example_tests.default_memory_size_get = dnx_data_module_testing_example_tests_default_memory_size_get;

    
    dnx_data_module_testing.example_tests.all_get = dnx_data_module_testing_example_tests_all_get;
    dnx_data_module_testing.example_tests.all_info_get = dnx_data_module_testing_example_tests_all_info_get;
    dnx_data_module_testing.example_tests.mem_consumption_get = dnx_data_module_testing_example_tests_mem_consumption_get;
    dnx_data_module_testing.example_tests.mem_consumption_info_get = dnx_data_module_testing_example_tests_mem_consumption_info_get;

    
    
    dnx_data_module_testing.submodule_family_dnx2.feature_get = dnx_data_module_testing_submodule_family_dnx2_feature_get;

    
    dnx_data_module_testing.submodule_family_dnx2.dnx2_submodule_define_get = dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_define_get;
    dnx_data_module_testing.submodule_family_dnx2.dnx2_submodule_numeric_get = dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_numeric_get;

    
    dnx_data_module_testing.submodule_family_dnx2.dnx2_submodule_table_get = dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_get;
    dnx_data_module_testing.submodule_family_dnx2.dnx2_submodule_table_info_get = dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_info_get;

    
    
    dnx_data_module_testing.compile_flags_submodule_standard.feature_get = dnx_data_module_testing_compile_flags_submodule_standard_feature_get;

    
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_module_testing.compile_flags_submodule_standard.define_ifdef_get = dnx_data_module_testing_compile_flags_submodule_standard_define_ifdef_get;

#endif 
#ifndef INCLUDE_XFLOW_MACSEC

    dnx_data_module_testing.compile_flags_submodule_standard.define_ifndef_get = dnx_data_module_testing_compile_flags_submodule_standard_define_ifndef_get;

#endif 
#if defined(INCLUDE_XFLOW_MACSEC)

    dnx_data_module_testing.compile_flags_submodule_standard.define_if_get = dnx_data_module_testing_compile_flags_submodule_standard_define_if_get;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_module_testing.compile_flags_submodule_standard.numeric_flags_get = dnx_data_module_testing_compile_flags_submodule_standard_numeric_flags_get;

#endif 

    
#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_module_testing.compile_flags_submodule_standard.table_flags_get = dnx_data_module_testing_compile_flags_submodule_standard_table_flags_get;
    dnx_data_module_testing.compile_flags_submodule_standard.table_flags_info_get = dnx_data_module_testing_compile_flags_submodule_standard_table_flags_info_get;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    
    
    dnx_data_module_testing.compile_flags_submodule_flags.feature_get = dnx_data_module_testing_compile_flags_submodule_flags_feature_get;

    
    dnx_data_module_testing.compile_flags_submodule_flags.define_no_flags_get = dnx_data_module_testing_compile_flags_submodule_flags_define_no_flags_get;
#ifdef BCM_WARM_BOOT_SUPPORT

    dnx_data_module_testing.compile_flags_submodule_flags.define_flags_get = dnx_data_module_testing_compile_flags_submodule_flags_define_flags_get;

#endif 
    dnx_data_module_testing.compile_flags_submodule_flags.numeric_no_flags_get = dnx_data_module_testing_compile_flags_submodule_flags_numeric_no_flags_get;
#ifdef BCM_WARM_BOOT_SUPPORT

    dnx_data_module_testing.compile_flags_submodule_flags.numeric_flags_get = dnx_data_module_testing_compile_flags_submodule_flags_numeric_flags_get;

#endif 

    
    dnx_data_module_testing.compile_flags_submodule_flags.table_no_flags_get = dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_get;
    dnx_data_module_testing.compile_flags_submodule_flags.table_no_flags_info_get = dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_info_get;
#ifdef BCM_WARM_BOOT_SUPPORT

    dnx_data_module_testing.compile_flags_submodule_flags.table_flags_get = dnx_data_module_testing_compile_flags_submodule_flags_table_flags_get;
    dnx_data_module_testing.compile_flags_submodule_flags.table_flags_info_get = dnx_data_module_testing_compile_flags_submodule_flags_table_flags_info_get;

#endif 

#endif 
    
    
    dnx_data_module_testing.max_test.feature_get = dnx_data_module_testing_max_test_feature_get;

    
    dnx_data_module_testing.max_test.greater_d3_get = dnx_data_module_testing_max_test_greater_d3_get;
    dnx_data_module_testing.max_test.greater_d2_get = dnx_data_module_testing_max_test_greater_d2_get;
    dnx_data_module_testing.max_test.same_value_get = dnx_data_module_testing_max_test_same_value_get;
    dnx_data_module_testing.max_test.d2_undefined_get = dnx_data_module_testing_max_test_d2_undefined_get;
    dnx_data_module_testing.max_test.d3_undefined_get = dnx_data_module_testing_max_test_d3_undefined_get;

    dnx_data_module_testing.max_test.dnx2_exclusive_get = dnx_data_module_testing_max_test_dnx2_exclusive_get;

#ifdef INCLUDE_XFLOW_MACSEC

    dnx_data_module_testing.max_test.compile_flags_get = dnx_data_module_testing_max_test_compile_flags_get;

#endif 

    
    
    
    
    dnx_data_mpls.general.feature_get = dnx_data_mpls_general_feature_get;

    
    dnx_data_mpls.general.nof_mpls_termination_profiles_get = dnx_data_mpls_general_nof_mpls_termination_profiles_get;
    dnx_data_mpls.general.nof_mpls_push_profiles_get = dnx_data_mpls_general_nof_mpls_push_profiles_get;
    dnx_data_mpls.general.nof_bits_mpls_label_get = dnx_data_mpls_general_nof_bits_mpls_label_get;
    dnx_data_mpls.general.log_nof_mpls_range_elements_get = dnx_data_mpls_general_log_nof_mpls_range_elements_get;
    dnx_data_mpls.general.nof_mpls_range_elements_get = dnx_data_mpls_general_nof_mpls_range_elements_get;
    dnx_data_mpls.general.mpls_special_label_encap_handling_get = dnx_data_mpls_general_mpls_special_label_encap_handling_get;
    dnx_data_mpls.general.mpls_control_word_supported_get = dnx_data_mpls_general_mpls_control_word_supported_get;
    dnx_data_mpls.general.mpls_second_stage_parser_handling_get = dnx_data_mpls_general_mpls_second_stage_parser_handling_get;
    dnx_data_mpls.general.mpls_control_word_flag_bits_lsb_get = dnx_data_mpls_general_mpls_control_word_flag_bits_lsb_get;
    dnx_data_mpls.general.fhei_mps_type_nof_bits_get = dnx_data_mpls_general_fhei_mps_type_nof_bits_get;
    dnx_data_mpls.general.mpls_speculative_learning_handling_get = dnx_data_mpls_general_mpls_speculative_learning_handling_get;
    dnx_data_mpls.general.gach_with_flow_label_supported_get = dnx_data_mpls_general_gach_with_flow_label_supported_get;
    dnx_data_mpls.general.max_nof_mpls_term_stages_get = dnx_data_mpls_general_max_nof_mpls_term_stages_get;
    dnx_data_mpls.general.nof_php_gport_lif_get = dnx_data_mpls_general_nof_php_gport_lif_get;

    
    
    
    
    dnx_data_multicast.general.feature_get = dnx_data_multicast_general_feature_get;

    

    
    
    
    dnx_data_multicast.params.feature_get = dnx_data_multicast_params_feature_get;

    
    dnx_data_multicast.params.max_ing_mc_groups_get = dnx_data_multicast_params_max_ing_mc_groups_get;
    dnx_data_multicast.params.max_egr_mc_groups_get = dnx_data_multicast_params_max_egr_mc_groups_get;
    dnx_data_multicast.params.nof_ing_mc_bitmaps_get = dnx_data_multicast_params_nof_ing_mc_bitmaps_get;
    dnx_data_multicast.params.nof_egr_mc_bitmaps_get = dnx_data_multicast_params_nof_egr_mc_bitmaps_get;
    dnx_data_multicast.params.mcdb_allocation_method_get = dnx_data_multicast_params_mcdb_allocation_method_get;
    dnx_data_multicast.params.nof_mcdb_entries_bits_get = dnx_data_multicast_params_nof_mcdb_entries_bits_get;
    dnx_data_multicast.params.ingr_dest_bits_get = dnx_data_multicast_params_ingr_dest_bits_get;
    dnx_data_multicast.params.max_static_element_id_get = dnx_data_multicast_params_max_static_element_id_get;
    dnx_data_multicast.params.mcdb_block_size_get = dnx_data_multicast_params_mcdb_block_size_get;
    dnx_data_multicast.params.nof_mcdb_entries_get = dnx_data_multicast_params_nof_mcdb_entries_get;
    dnx_data_multicast.params.max_nof_ingress_bmp_dest_get = dnx_data_multicast_params_max_nof_ingress_bmp_dest_get;

    
    dnx_data_multicast.params.olp_ing_multicast_id_get = dnx_data_multicast_params_olp_ing_multicast_id_get;
    dnx_data_multicast.params.olp_ing_multicast_id_info_get = dnx_data_multicast_params_olp_ing_multicast_id_info_get;
    
    
    dnx_data_multicast.mcdb_hash_table.feature_get = dnx_data_multicast_mcdb_hash_table_feature_get;

    
    dnx_data_multicast.mcdb_hash_table.max_key_size_in_words_get = dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_get;
    dnx_data_multicast.mcdb_hash_table.destination_start_bit_get = dnx_data_multicast_mcdb_hash_table_destination_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.destination_nof_bits_get = dnx_data_multicast_mcdb_hash_table_destination_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.reference_num_start_bit_get = dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.reference_num_nof_bits_get = dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.destination_type_start_bit_get = dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.destination_type_nof_bits_get = dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.info_start_bit_get = dnx_data_multicast_mcdb_hash_table_info_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.info_nof_bits_get = dnx_data_multicast_mcdb_hash_table_info_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.info_type_start_bit_get = dnx_data_multicast_mcdb_hash_table_info_type_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.info_type_nof_bits_get = dnx_data_multicast_mcdb_hash_table_info_type_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.group_id_start_bit_get = dnx_data_multicast_mcdb_hash_table_group_id_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.group_id_nof_bits_get = dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.is_ingress_start_bit_get = dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.is_ingress_nof_bits_get = dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.core_id_start_bit_get = dnx_data_multicast_mcdb_hash_table_core_id_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.core_id_nof_bits_get = dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.destination2_start_bit_get = dnx_data_multicast_mcdb_hash_table_destination2_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.destination2_nof_bits_get = dnx_data_multicast_mcdb_hash_table_destination2_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.destination2_type_start_bit_get = dnx_data_multicast_mcdb_hash_table_destination2_type_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.destination2_type_nof_bits_get = dnx_data_multicast_mcdb_hash_table_destination2_type_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.same_if_dspa_valid_start_bit_get = dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.same_if_dspa_valid_nof_bits_get = dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.same_if_dspa_start_bit_get = dnx_data_multicast_mcdb_hash_table_same_if_dspa_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.same_if_dspa_nof_bits_get = dnx_data_multicast_mcdb_hash_table_same_if_dspa_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.same_if_outlif_valid_start_bit_get = dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.same_if_outlif_valid_nof_bits_get = dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.same_if_outlif_start_bit_get = dnx_data_multicast_mcdb_hash_table_same_if_outlif_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.same_if_outlif_nof_bits_get = dnx_data_multicast_mcdb_hash_table_same_if_outlif_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.forward_group_valid_start_bit_get = dnx_data_multicast_mcdb_hash_table_forward_group_valid_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.forward_group_valid_nof_bits_get = dnx_data_multicast_mcdb_hash_table_forward_group_valid_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.forward_group_start_bit_get = dnx_data_multicast_mcdb_hash_table_forward_group_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.forward_group_nof_bits_get = dnx_data_multicast_mcdb_hash_table_forward_group_nof_bits_get;
    dnx_data_multicast.mcdb_hash_table.is_static_start_bit_get = dnx_data_multicast_mcdb_hash_table_is_static_start_bit_get;
    dnx_data_multicast.mcdb_hash_table.is_static_nof_bits_get = dnx_data_multicast_mcdb_hash_table_is_static_nof_bits_get;

    
    
    
    
    dnx_data_nif.global.feature_get = dnx_data_nif_global_feature_get;

    
    dnx_data_nif.global.nof_lcplls_get = dnx_data_nif_global_nof_lcplls_get;
    dnx_data_nif.global.nof_nif_interfaces_per_core_get = dnx_data_nif_global_nof_nif_interfaces_per_core_get;
    dnx_data_nif.global.nif_interface_id_to_unit_id_granularity_get = dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get;
    dnx_data_nif.global.nof_nif_units_per_core_get = dnx_data_nif_global_nof_nif_units_per_core_get;
    dnx_data_nif.global.reassembler_fifo_threshold_get = dnx_data_nif_global_reassembler_fifo_threshold_get;
    dnx_data_nif.global.total_data_capacity_get = dnx_data_nif_global_total_data_capacity_get;
    dnx_data_nif.global.single_mgmt_port_capacity_get = dnx_data_nif_global_single_mgmt_port_capacity_get;
    dnx_data_nif.global.max_nof_lanes_per_port_get = dnx_data_nif_global_max_nof_lanes_per_port_get;
    dnx_data_nif.global.ghao_speed_limit_get = dnx_data_nif_global_ghao_speed_limit_get;
    dnx_data_nif.global.start_tx_threshold_global_get = dnx_data_nif_global_start_tx_threshold_global_get;
    dnx_data_nif.global.l1_only_mode_get = dnx_data_nif_global_l1_only_mode_get;
    dnx_data_nif.global.otn_supported_get = dnx_data_nif_global_otn_supported_get;

    
    dnx_data_nif.global.pll_phys_get = dnx_data_nif_global_pll_phys_get;
    dnx_data_nif.global.pll_phys_info_get = dnx_data_nif_global_pll_phys_info_get;
    dnx_data_nif.global.nif_interface_id_to_unit_id_get = dnx_data_nif_global_nif_interface_id_to_unit_id_get;
    dnx_data_nif.global.nif_interface_id_to_unit_id_info_get = dnx_data_nif_global_nif_interface_id_to_unit_id_info_get;
    
    
    dnx_data_nif.phys.feature_get = dnx_data_nif_phys_feature_get;

    
    dnx_data_nif.phys.nof_phys_get = dnx_data_nif_phys_nof_phys_get;
    dnx_data_nif.phys.max_phys_in_core_get = dnx_data_nif_phys_max_phys_in_core_get;
    dnx_data_nif.phys.pm8x50_gen_get = dnx_data_nif_phys_pm8x50_gen_get;
    dnx_data_nif.phys.is_pam4_speed_supported_get = dnx_data_nif_phys_is_pam4_speed_supported_get;
    dnx_data_nif.phys.first_mgmt_phy_get = dnx_data_nif_phys_first_mgmt_phy_get;
    dnx_data_nif.phys.nof_mgmt_phys_get = dnx_data_nif_phys_nof_mgmt_phys_get;
    dnx_data_nif.phys.txpi_sdm_scheme_get = dnx_data_nif_phys_txpi_sdm_scheme_get;

    
    dnx_data_nif.phys.general_get = dnx_data_nif_phys_general_get;
    dnx_data_nif.phys.general_info_get = dnx_data_nif_phys_general_info_get;
    dnx_data_nif.phys.polarity_get = dnx_data_nif_phys_polarity_get;
    dnx_data_nif.phys.polarity_info_get = dnx_data_nif_phys_polarity_info_get;
    dnx_data_nif.phys.core_phys_map_get = dnx_data_nif_phys_core_phys_map_get;
    dnx_data_nif.phys.core_phys_map_info_get = dnx_data_nif_phys_core_phys_map_info_get;
    dnx_data_nif.phys.vco_div_get = dnx_data_nif_phys_vco_div_get;
    dnx_data_nif.phys.vco_div_info_get = dnx_data_nif_phys_vco_div_info_get;
    dnx_data_nif.phys.nof_phys_per_core_get = dnx_data_nif_phys_nof_phys_per_core_get;
    dnx_data_nif.phys.nof_phys_per_core_info_get = dnx_data_nif_phys_nof_phys_per_core_info_get;
    dnx_data_nif.phys.txpi_sdm_div_get = dnx_data_nif_phys_txpi_sdm_div_get;
    dnx_data_nif.phys.txpi_sdm_div_info_get = dnx_data_nif_phys_txpi_sdm_div_info_get;
    
    
    dnx_data_nif.ilkn.feature_get = dnx_data_nif_ilkn_feature_get;

    
    dnx_data_nif.ilkn.ilu_nof_get = dnx_data_nif_ilkn_ilu_nof_get;
    dnx_data_nif.ilkn.ilkn_unit_nof_get = dnx_data_nif_ilkn_ilkn_unit_nof_get;
    dnx_data_nif.ilkn.ilkn_unit_if_nof_get = dnx_data_nif_ilkn_ilkn_unit_if_nof_get;
    dnx_data_nif.ilkn.ilkn_if_nof_get = dnx_data_nif_ilkn_ilkn_if_nof_get;
    dnx_data_nif.ilkn.fec_units_per_core_nof_get = dnx_data_nif_ilkn_fec_units_per_core_nof_get;
    dnx_data_nif.ilkn.nof_lanes_per_fec_unit_get = dnx_data_nif_ilkn_nof_lanes_per_fec_unit_get;
    dnx_data_nif.ilkn.lanes_max_nof_get = dnx_data_nif_ilkn_lanes_max_nof_get;
    dnx_data_nif.ilkn.lanes_max_nof_using_fec_get = dnx_data_nif_ilkn_lanes_max_nof_using_fec_get;
    dnx_data_nif.ilkn.lanes_allowed_nof_get = dnx_data_nif_ilkn_lanes_allowed_nof_get;
    dnx_data_nif.ilkn.ilkn_over_eth_pms_max_get = dnx_data_nif_ilkn_ilkn_over_eth_pms_max_get;
    dnx_data_nif.ilkn.segments_max_nof_get = dnx_data_nif_ilkn_segments_max_nof_get;
    dnx_data_nif.ilkn.segments_half_nof_get = dnx_data_nif_ilkn_segments_half_nof_get;
    dnx_data_nif.ilkn.pms_nof_get = dnx_data_nif_ilkn_pms_nof_get;
    dnx_data_nif.ilkn.fmac_bus_size_get = dnx_data_nif_ilkn_fmac_bus_size_get;
    dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get = dnx_data_nif_ilkn_ilkn_rx_hrf_nof_get;
    dnx_data_nif.ilkn.ilkn_tx_hrf_nof_get = dnx_data_nif_ilkn_ilkn_tx_hrf_nof_get;
    dnx_data_nif.ilkn.data_rx_hrf_size_get = dnx_data_nif_ilkn_data_rx_hrf_size_get;
    dnx_data_nif.ilkn.tdm_rx_hrf_size_get = dnx_data_nif_ilkn_tdm_rx_hrf_size_get;
    dnx_data_nif.ilkn.tx_hrf_credits_get = dnx_data_nif_ilkn_tx_hrf_credits_get;
    dnx_data_nif.ilkn.nof_rx_hrf_per_port_get = dnx_data_nif_ilkn_nof_rx_hrf_per_port_get;
    dnx_data_nif.ilkn.watermark_high_elk_get = dnx_data_nif_ilkn_watermark_high_elk_get;
    dnx_data_nif.ilkn.watermark_low_elk_get = dnx_data_nif_ilkn_watermark_low_elk_get;
    dnx_data_nif.ilkn.watermark_high_data_get = dnx_data_nif_ilkn_watermark_high_data_get;
    dnx_data_nif.ilkn.watermark_low_data_get = dnx_data_nif_ilkn_watermark_low_data_get;
    dnx_data_nif.ilkn.pad_size_get = dnx_data_nif_ilkn_pad_size_get;
    dnx_data_nif.ilkn.burst_max_range_max_get = dnx_data_nif_ilkn_burst_max_range_max_get;
    dnx_data_nif.ilkn.burst_min_get = dnx_data_nif_ilkn_burst_min_get;
    dnx_data_nif.ilkn.burst_short_get = dnx_data_nif_ilkn_burst_short_get;
    dnx_data_nif.ilkn.burst_min_range_max_get = dnx_data_nif_ilkn_burst_min_range_max_get;
    dnx_data_nif.ilkn.max_nof_ifs_get = dnx_data_nif_ilkn_max_nof_ifs_get;
    dnx_data_nif.ilkn.max_nof_elk_ifs_get = dnx_data_nif_ilkn_max_nof_elk_ifs_get;
    dnx_data_nif.ilkn.port_0_status_intr_id_get = dnx_data_nif_ilkn_port_0_status_intr_id_get;
    dnx_data_nif.ilkn.port_1_status_intr_id_get = dnx_data_nif_ilkn_port_1_status_intr_id_get;
    dnx_data_nif.ilkn.link_stable_wait_get = dnx_data_nif_ilkn_link_stable_wait_get;
    dnx_data_nif.ilkn.interleaved_error_drop_single_context_get = dnx_data_nif_ilkn_interleaved_error_drop_single_context_get;

    
    dnx_data_nif.ilkn.phys_get = dnx_data_nif_ilkn_phys_get;
    dnx_data_nif.ilkn.phys_info_get = dnx_data_nif_ilkn_phys_info_get;
    dnx_data_nif.ilkn.supported_phys_get = dnx_data_nif_ilkn_supported_phys_get;
    dnx_data_nif.ilkn.supported_phys_info_get = dnx_data_nif_ilkn_supported_phys_info_get;
    dnx_data_nif.ilkn.supported_interfaces_get = dnx_data_nif_ilkn_supported_interfaces_get;
    dnx_data_nif.ilkn.supported_interfaces_info_get = dnx_data_nif_ilkn_supported_interfaces_info_get;
    dnx_data_nif.ilkn.ilkn_cores_get = dnx_data_nif_ilkn_ilkn_cores_get;
    dnx_data_nif.ilkn.ilkn_cores_info_get = dnx_data_nif_ilkn_ilkn_cores_info_get;
    dnx_data_nif.ilkn.ilkn_cores_clup_facing_get = dnx_data_nif_ilkn_ilkn_cores_clup_facing_get;
    dnx_data_nif.ilkn.ilkn_cores_clup_facing_info_get = dnx_data_nif_ilkn_ilkn_cores_clup_facing_info_get;
    dnx_data_nif.ilkn.supported_device_core_get = dnx_data_nif_ilkn_supported_device_core_get;
    dnx_data_nif.ilkn.supported_device_core_info_get = dnx_data_nif_ilkn_supported_device_core_info_get;
    dnx_data_nif.ilkn.properties_get = dnx_data_nif_ilkn_properties_get;
    dnx_data_nif.ilkn.properties_info_get = dnx_data_nif_ilkn_properties_info_get;
    dnx_data_nif.ilkn.nif_pms_get = dnx_data_nif_ilkn_nif_pms_get;
    dnx_data_nif.ilkn.nif_pms_info_get = dnx_data_nif_ilkn_nif_pms_info_get;
    dnx_data_nif.ilkn.fabric_pms_get = dnx_data_nif_ilkn_fabric_pms_get;
    dnx_data_nif.ilkn.fabric_pms_info_get = dnx_data_nif_ilkn_fabric_pms_info_get;
    dnx_data_nif.ilkn.nif_lanes_map_get = dnx_data_nif_ilkn_nif_lanes_map_get;
    dnx_data_nif.ilkn.nif_lanes_map_info_get = dnx_data_nif_ilkn_nif_lanes_map_info_get;
    dnx_data_nif.ilkn.fabric_lanes_map_get = dnx_data_nif_ilkn_fabric_lanes_map_get;
    dnx_data_nif.ilkn.fabric_lanes_map_info_get = dnx_data_nif_ilkn_fabric_lanes_map_info_get;
    dnx_data_nif.ilkn.start_tx_threshold_table_get = dnx_data_nif_ilkn_start_tx_threshold_table_get;
    dnx_data_nif.ilkn.start_tx_threshold_table_info_get = dnx_data_nif_ilkn_start_tx_threshold_table_info_get;
    dnx_data_nif.ilkn.connectivity_options_get = dnx_data_nif_ilkn_connectivity_options_get;
    dnx_data_nif.ilkn.connectivity_options_info_get = dnx_data_nif_ilkn_connectivity_options_info_get;
    
    
    dnx_data_nif.eth.feature_get = dnx_data_nif_eth_feature_get;

    
    dnx_data_nif.eth.cdu_nof_get = dnx_data_nif_eth_cdu_nof_get;
    dnx_data_nif.eth.clu_nof_get = dnx_data_nif_eth_clu_nof_get;
    dnx_data_nif.eth.nbu_nof_get = dnx_data_nif_eth_nbu_nof_get;
    dnx_data_nif.eth.mgu_nof_get = dnx_data_nif_eth_mgu_nof_get;
    dnx_data_nif.eth.cdum_nof_get = dnx_data_nif_eth_cdum_nof_get;
    dnx_data_nif.eth.nof_pms_in_cdu_get = dnx_data_nif_eth_nof_pms_in_cdu_get;
    dnx_data_nif.eth.nof_pms_in_clu_get = dnx_data_nif_eth_nof_pms_in_clu_get;
    dnx_data_nif.eth.nof_pms_in_nbu_get = dnx_data_nif_eth_nof_pms_in_nbu_get;
    dnx_data_nif.eth.nof_pms_in_mgu_get = dnx_data_nif_eth_nof_pms_in_mgu_get;
    dnx_data_nif.eth.total_nof_ethu_pms_in_device_get = dnx_data_nif_eth_total_nof_ethu_pms_in_device_get;
    dnx_data_nif.eth.nof_cdu_lanes_in_pm_get = dnx_data_nif_eth_nof_cdu_lanes_in_pm_get;
    dnx_data_nif.eth.nof_clu_lanes_in_pm_get = dnx_data_nif_eth_nof_clu_lanes_in_pm_get;
    dnx_data_nif.eth.nof_cdu_pms_get = dnx_data_nif_eth_nof_cdu_pms_get;
    dnx_data_nif.eth.nof_clu_pms_get = dnx_data_nif_eth_nof_clu_pms_get;
    dnx_data_nif.eth.nof_nbu_lanes_in_pm_get = dnx_data_nif_eth_nof_nbu_lanes_in_pm_get;
    dnx_data_nif.eth.nof_mgu_lanes_in_pm_get = dnx_data_nif_eth_nof_mgu_lanes_in_pm_get;
    dnx_data_nif.eth.nof_nbu_pms_get = dnx_data_nif_eth_nof_nbu_pms_get;
    dnx_data_nif.eth.nof_mgu_pms_get = dnx_data_nif_eth_nof_mgu_pms_get;
    dnx_data_nif.eth.ethu_nof_get = dnx_data_nif_eth_ethu_nof_get;
    dnx_data_nif.eth.cdu_nof_per_core_get = dnx_data_nif_eth_cdu_nof_per_core_get;
    dnx_data_nif.eth.clu_nof_per_core_get = dnx_data_nif_eth_clu_nof_per_core_get;
    dnx_data_nif.eth.max_ethu_nof_per_core_get = dnx_data_nif_eth_max_ethu_nof_per_core_get;
    dnx_data_nif.eth.nof_lanes_in_cdu_get = dnx_data_nif_eth_nof_lanes_in_cdu_get;
    dnx_data_nif.eth.nof_lanes_in_clu_get = dnx_data_nif_eth_nof_lanes_in_clu_get;
    dnx_data_nif.eth.nof_lanes_in_nbu_get = dnx_data_nif_eth_nof_lanes_in_nbu_get;
    dnx_data_nif.eth.nof_lanes_in_mgu_get = dnx_data_nif_eth_nof_lanes_in_mgu_get;
    dnx_data_nif.eth.max_nof_lanes_in_ethu_get = dnx_data_nif_eth_max_nof_lanes_in_ethu_get;
    dnx_data_nif.eth.total_nof_cdu_lanes_in_device_get = dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get;
    dnx_data_nif.eth.total_nof_nbu_lanes_in_device_get = dnx_data_nif_eth_total_nof_nbu_lanes_in_device_get;
    dnx_data_nif.eth.cdu_mac_mode_config_nof_get = dnx_data_nif_eth_cdu_mac_mode_config_nof_get;
    dnx_data_nif.eth.nbu_mac_mode_config_nof_get = dnx_data_nif_eth_nbu_mac_mode_config_nof_get;
    dnx_data_nif.eth.mac_mode_config_lanes_nof_get = dnx_data_nif_eth_mac_mode_config_lanes_nof_get;
    dnx_data_nif.eth.cdu_mac_nof_get = dnx_data_nif_eth_cdu_mac_nof_get;
    dnx_data_nif.eth.nbu_mac_nof_get = dnx_data_nif_eth_nbu_mac_nof_get;
    dnx_data_nif.eth.mac_lanes_nof_get = dnx_data_nif_eth_mac_lanes_nof_get;
    dnx_data_nif.eth.ethu_logical_fifo_nof_get = dnx_data_nif_eth_ethu_logical_fifo_nof_get;
    dnx_data_nif.eth.cdu_memory_entries_nof_get = dnx_data_nif_eth_cdu_memory_entries_nof_get;
    dnx_data_nif.eth.clu_memory_entries_nof_get = dnx_data_nif_eth_clu_memory_entries_nof_get;
    dnx_data_nif.eth.nbu_memory_entries_nof_get = dnx_data_nif_eth_nbu_memory_entries_nof_get;
    dnx_data_nif.eth.mgu_memory_entries_nof_get = dnx_data_nif_eth_mgu_memory_entries_nof_get;
    dnx_data_nif.eth.priority_group_nof_entries_min_get = dnx_data_nif_eth_priority_group_nof_entries_min_get;
    dnx_data_nif.eth.priority_groups_nof_get = dnx_data_nif_eth_priority_groups_nof_get;
    dnx_data_nif.eth.nof_rmcs_per_priority_group_get = dnx_data_nif_eth_nof_rmcs_per_priority_group_get;
    dnx_data_nif.eth.pad_size_min_get = dnx_data_nif_eth_pad_size_min_get;
    dnx_data_nif.eth.pad_size_max_get = dnx_data_nif_eth_pad_size_max_get;
    dnx_data_nif.eth.packet_size_max_get = dnx_data_nif_eth_packet_size_max_get;
    dnx_data_nif.eth.an_max_nof_abilities_get = dnx_data_nif_eth_an_max_nof_abilities_get;
    dnx_data_nif.eth.phy_map_granularity_get = dnx_data_nif_eth_phy_map_granularity_get;
    dnx_data_nif.eth.vco_factor_for_53p125_rate_get = dnx_data_nif_eth_vco_factor_for_53p125_rate_get;
    dnx_data_nif.eth.vco_factor_for_51p5625_rate_get = dnx_data_nif_eth_vco_factor_for_51p5625_rate_get;
    dnx_data_nif.eth.is_400G_supported_get = dnx_data_nif_eth_is_400G_supported_get;
    dnx_data_nif.eth.is_800G_supported_get = dnx_data_nif_eth_is_800G_supported_get;

    
    dnx_data_nif.eth.start_tx_threshold_table_get = dnx_data_nif_eth_start_tx_threshold_table_get;
    dnx_data_nif.eth.start_tx_threshold_table_info_get = dnx_data_nif_eth_start_tx_threshold_table_info_get;
    dnx_data_nif.eth.pm_properties_get = dnx_data_nif_eth_pm_properties_get;
    dnx_data_nif.eth.pm_properties_info_get = dnx_data_nif_eth_pm_properties_info_get;
    dnx_data_nif.eth.ethu_properties_get = dnx_data_nif_eth_ethu_properties_get;
    dnx_data_nif.eth.ethu_properties_info_get = dnx_data_nif_eth_ethu_properties_info_get;
    dnx_data_nif.eth.phy_map_get = dnx_data_nif_eth_phy_map_get;
    dnx_data_nif.eth.phy_map_info_get = dnx_data_nif_eth_phy_map_info_get;
    dnx_data_nif.eth.is_nif_loopback_supported_get = dnx_data_nif_eth_is_nif_loopback_supported_get;
    dnx_data_nif.eth.is_nif_loopback_supported_info_get = dnx_data_nif_eth_is_nif_loopback_supported_info_get;
    dnx_data_nif.eth.mac_padding_get = dnx_data_nif_eth_mac_padding_get;
    dnx_data_nif.eth.mac_padding_info_get = dnx_data_nif_eth_mac_padding_info_get;
    dnx_data_nif.eth.max_speed_get = dnx_data_nif_eth_max_speed_get;
    dnx_data_nif.eth.max_speed_info_get = dnx_data_nif_eth_max_speed_info_get;
    dnx_data_nif.eth.supported_interfaces_get = dnx_data_nif_eth_supported_interfaces_get;
    dnx_data_nif.eth.supported_interfaces_info_get = dnx_data_nif_eth_supported_interfaces_info_get;
    dnx_data_nif.eth.ethu_per_core_get = dnx_data_nif_eth_ethu_per_core_get;
    dnx_data_nif.eth.ethu_per_core_info_get = dnx_data_nif_eth_ethu_per_core_info_get;
    dnx_data_nif.eth.nif_cores_ethus_get = dnx_data_nif_eth_nif_cores_ethus_get;
    dnx_data_nif.eth.nif_cores_ethus_info_get = dnx_data_nif_eth_nif_cores_ethus_info_get;
    dnx_data_nif.eth.supported_clu_mgu_an_abilities_get = dnx_data_nif_eth_supported_clu_mgu_an_abilities_get;
    dnx_data_nif.eth.supported_clu_mgu_an_abilities_info_get = dnx_data_nif_eth_supported_clu_mgu_an_abilities_info_get;
    dnx_data_nif.eth.ingress_reassembly_context_mapping_get = dnx_data_nif_eth_ingress_reassembly_context_mapping_get;
    dnx_data_nif.eth.ingress_reassembly_context_mapping_info_get = dnx_data_nif_eth_ingress_reassembly_context_mapping_info_get;
    
    
    dnx_data_nif.simulator.feature_get = dnx_data_nif_simulator_feature_get;

    
    dnx_data_nif.simulator.cdu_type_get = dnx_data_nif_simulator_cdu_type_get;
    dnx_data_nif.simulator.clu_type_get = dnx_data_nif_simulator_clu_type_get;
    dnx_data_nif.simulator.nbu_type_get = dnx_data_nif_simulator_nbu_type_get;
    dnx_data_nif.simulator.mgu_type_get = dnx_data_nif_simulator_mgu_type_get;

    
    
    
    dnx_data_nif.framer.feature_get = dnx_data_nif_framer_feature_get;

    
    dnx_data_nif.framer.nof_units_get = dnx_data_nif_framer_nof_units_get;
    dnx_data_nif.framer.b66switch_backup_channels_get = dnx_data_nif_framer_b66switch_backup_channels_get;
    dnx_data_nif.framer.gen_get = dnx_data_nif_framer_gen_get;
    dnx_data_nif.framer.nof_mgmt_channels_get = dnx_data_nif_framer_nof_mgmt_channels_get;
    dnx_data_nif.framer.clock_mhz_get = dnx_data_nif_framer_clock_mhz_get;
    dnx_data_nif.framer.ptp_channel_max_bandwidth_get = dnx_data_nif_framer_ptp_channel_max_bandwidth_get;
    dnx_data_nif.framer.oam_channel_max_bandwidth_get = dnx_data_nif_framer_oam_channel_max_bandwidth_get;
    dnx_data_nif.framer.nof_mac1_channels_get = dnx_data_nif_framer_nof_mac1_channels_get;
    dnx_data_nif.framer.nof_mac2_channels_get = dnx_data_nif_framer_nof_mac2_channels_get;
    dnx_data_nif.framer.nof_pms_per_framer_get = dnx_data_nif_framer_nof_pms_per_framer_get;

    
    dnx_data_nif.framer.b66switch_get = dnx_data_nif_framer_b66switch_get;
    dnx_data_nif.framer.b66switch_info_get = dnx_data_nif_framer_b66switch_info_get;
    dnx_data_nif.framer.mac_info_get = dnx_data_nif_framer_mac_info_get;
    dnx_data_nif.framer.mac_info_info_get = dnx_data_nif_framer_mac_info_info_get;
    dnx_data_nif.framer.mgmt_channel_info_get = dnx_data_nif_framer_mgmt_channel_info_get;
    dnx_data_nif.framer.mgmt_channel_info_info_get = dnx_data_nif_framer_mgmt_channel_info_info_get;
    
    
    dnx_data_nif.mac_client.feature_get = dnx_data_nif_mac_client_feature_get;

    
    dnx_data_nif.mac_client.nof_normal_clients_get = dnx_data_nif_mac_client_nof_normal_clients_get;
    dnx_data_nif.mac_client.nof_special_clients_get = dnx_data_nif_mac_client_nof_special_clients_get;
    dnx_data_nif.mac_client.nof_clients_get = dnx_data_nif_mac_client_nof_clients_get;
    dnx_data_nif.mac_client.channel_base_get = dnx_data_nif_mac_client_channel_base_get;
    dnx_data_nif.mac_client.max_nof_slots_get = dnx_data_nif_mac_client_max_nof_slots_get;
    dnx_data_nif.mac_client.nof_logical_fifos_get = dnx_data_nif_mac_client_nof_logical_fifos_get;
    dnx_data_nif.mac_client.oam_client_channel_get = dnx_data_nif_mac_client_oam_client_channel_get;
    dnx_data_nif.mac_client.ptp_client_channel_get = dnx_data_nif_mac_client_ptp_client_channel_get;
    dnx_data_nif.mac_client.oam_tx_cal_slot_get = dnx_data_nif_mac_client_oam_tx_cal_slot_get;
    dnx_data_nif.mac_client.ptp_tx_cal_slot_get = dnx_data_nif_mac_client_ptp_tx_cal_slot_get;
    dnx_data_nif.mac_client.priority_groups_nof_get = dnx_data_nif_mac_client_priority_groups_nof_get;
    dnx_data_nif.mac_client.ptp_channel_max_bandwidth_get = dnx_data_nif_mac_client_ptp_channel_max_bandwidth_get;
    dnx_data_nif.mac_client.oam_channel_max_bandwidth_get = dnx_data_nif_mac_client_oam_channel_max_bandwidth_get;
    dnx_data_nif.mac_client.nof_mem_entries_per_slot_get = dnx_data_nif_mac_client_nof_mem_entries_per_slot_get;
    dnx_data_nif.mac_client.ptp_oam_fifo_entries_in_sb_rx_get = dnx_data_nif_mac_client_ptp_oam_fifo_entries_in_sb_rx_get;
    dnx_data_nif.mac_client.average_ipg_for_l1_get = dnx_data_nif_mac_client_average_ipg_for_l1_get;
    dnx_data_nif.mac_client.mac_tx_threshold_get = dnx_data_nif_mac_client_mac_tx_threshold_get;
    dnx_data_nif.mac_client.rmc_cal_nof_slots_get = dnx_data_nif_mac_client_rmc_cal_nof_slots_get;
    dnx_data_nif.mac_client.nof_mac_timeslots_get = dnx_data_nif_mac_client_nof_mac_timeslots_get;
    dnx_data_nif.mac_client.tx_average_ipg_max_get = dnx_data_nif_mac_client_tx_average_ipg_max_get;
    dnx_data_nif.mac_client.tx_threshold_max_get = dnx_data_nif_mac_client_tx_threshold_max_get;
    dnx_data_nif.mac_client.tx_fifo_cell_cnt_max_get = dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_get;
    dnx_data_nif.mac_client.sb_fifo_granularity_get = dnx_data_nif_mac_client_sb_fifo_granularity_get;
    dnx_data_nif.mac_client.sb_cal_granularity_get = dnx_data_nif_mac_client_sb_cal_granularity_get;
    dnx_data_nif.mac_client.nof_sb_fifos_get = dnx_data_nif_mac_client_nof_sb_fifos_get;
    dnx_data_nif.mac_client.nof_sb_cal_slots_get = dnx_data_nif_mac_client_nof_sb_cal_slots_get;
    dnx_data_nif.mac_client.nof_units_per_sb_tx_fifo_get = dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_get;
    dnx_data_nif.mac_client.nof_units_per_sb_rx_fifo_get = dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_get;
    dnx_data_nif.mac_client.tinymac_clock_mhz_get = dnx_data_nif_mac_client_tinymac_clock_mhz_get;
    dnx_data_nif.mac_client.sb_tx_init_credit_for_5g_get = dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_get;
    dnx_data_nif.mac_client.async_fifo_delay_cycles_get = dnx_data_nif_mac_client_async_fifo_delay_cycles_get;
    dnx_data_nif.mac_client.pipe_delay_cycles_get = dnx_data_nif_mac_client_pipe_delay_cycles_get;
    dnx_data_nif.mac_client.mac_pad_size_get = dnx_data_nif_mac_client_mac_pad_size_get;

    
    dnx_data_nif.mac_client.start_tx_threshold_table_get = dnx_data_nif_mac_client_start_tx_threshold_table_get;
    dnx_data_nif.mac_client.start_tx_threshold_table_info_get = dnx_data_nif_mac_client_start_tx_threshold_table_info_get;
    dnx_data_nif.mac_client.l1_mismatch_rate_table_get = dnx_data_nif_mac_client_l1_mismatch_rate_table_get;
    dnx_data_nif.mac_client.l1_mismatch_rate_table_info_get = dnx_data_nif_mac_client_l1_mismatch_rate_table_info_get;
    dnx_data_nif.mac_client.speed_tx_property_table_get = dnx_data_nif_mac_client_speed_tx_property_table_get;
    dnx_data_nif.mac_client.speed_tx_property_table_info_get = dnx_data_nif_mac_client_speed_tx_property_table_info_get;
    
    
    dnx_data_nif.sar_client.feature_get = dnx_data_nif_sar_client_feature_get;

    
    dnx_data_nif.sar_client.nof_clients_get = dnx_data_nif_sar_client_nof_clients_get;
    dnx_data_nif.sar_client.ilkn_burst_size_in_feu_get = dnx_data_nif_sar_client_ilkn_burst_size_in_feu_get;
    dnx_data_nif.sar_client.nof_sar_timeslots_get = dnx_data_nif_sar_client_nof_sar_timeslots_get;
    dnx_data_nif.sar_client.channel_base_get = dnx_data_nif_sar_client_channel_base_get;
    dnx_data_nif.sar_client.sar_jitter_tolerance_max_level_get = dnx_data_nif_sar_client_sar_jitter_tolerance_max_level_get;
    dnx_data_nif.sar_client.cal_granularity_get = dnx_data_nif_sar_client_cal_granularity_get;

    
    dnx_data_nif.sar_client.sar_cell_mode_to_size_get = dnx_data_nif_sar_client_sar_cell_mode_to_size_get;
    dnx_data_nif.sar_client.sar_cell_mode_to_size_info_get = dnx_data_nif_sar_client_sar_cell_mode_to_size_info_get;
    dnx_data_nif.sar_client.sar_jitter_fifo_levels_get = dnx_data_nif_sar_client_sar_jitter_fifo_levels_get;
    dnx_data_nif.sar_client.sar_jitter_fifo_levels_info_get = dnx_data_nif_sar_client_sar_jitter_fifo_levels_info_get;
    
    
    dnx_data_nif.flexe.feature_get = dnx_data_nif_flexe_feature_get;

    
    dnx_data_nif.flexe.feu_nof_get = dnx_data_nif_flexe_feu_nof_get;
    dnx_data_nif.flexe.nof_clients_get = dnx_data_nif_flexe_nof_clients_get;
    dnx_data_nif.flexe.nof_groups_get = dnx_data_nif_flexe_nof_groups_get;
    dnx_data_nif.flexe.max_group_id_get = dnx_data_nif_flexe_max_group_id_get;
    dnx_data_nif.flexe.min_group_id_get = dnx_data_nif_flexe_min_group_id_get;
    dnx_data_nif.flexe.nb_tx_delay_get = dnx_data_nif_flexe_nb_tx_delay_get;
    dnx_data_nif.flexe.nb_tx_fifo_th_for_first_read_get = dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_get;
    dnx_data_nif.flexe.nof_pcs_get = dnx_data_nif_flexe_nof_pcs_get;
    dnx_data_nif.flexe.nof_phy_speeds_get = dnx_data_nif_flexe_nof_phy_speeds_get;
    dnx_data_nif.flexe.phy_speed_granularity_get = dnx_data_nif_flexe_phy_speed_granularity_get;
    dnx_data_nif.flexe.max_flexe_core_speed_get = dnx_data_nif_flexe_max_flexe_core_speed_get;
    dnx_data_nif.flexe.flexe_core_clock_source_pm_get = dnx_data_nif_flexe_flexe_core_clock_source_pm_get;
    dnx_data_nif.flexe.max_nof_slots_get = dnx_data_nif_flexe_max_nof_slots_get;
    dnx_data_nif.flexe.min_client_id_get = dnx_data_nif_flexe_min_client_id_get;
    dnx_data_nif.flexe.max_client_id_get = dnx_data_nif_flexe_max_client_id_get;
    dnx_data_nif.flexe.nof_flexe_instances_get = dnx_data_nif_flexe_nof_flexe_instances_get;
    dnx_data_nif.flexe.nof_flexe_lphys_get = dnx_data_nif_flexe_nof_flexe_lphys_get;
    dnx_data_nif.flexe.distributed_ilu_id_get = dnx_data_nif_flexe_distributed_ilu_id_get;
    dnx_data_nif.flexe.max_nif_rate_centralized_get = dnx_data_nif_flexe_max_nif_rate_centralized_get;
    dnx_data_nif.flexe.max_nif_rate_distributed_get = dnx_data_nif_flexe_max_nif_rate_distributed_get;
    dnx_data_nif.flexe.max_ilu_rate_distributed_get = dnx_data_nif_flexe_max_ilu_rate_distributed_get;
    dnx_data_nif.flexe.nof_flexe_core_ports_get = dnx_data_nif_flexe_nof_flexe_core_ports_get;
    dnx_data_nif.flexe.nb_cal_nof_slots_get = dnx_data_nif_flexe_nb_cal_nof_slots_get;
    dnx_data_nif.flexe.nb_cal_granularity_get = dnx_data_nif_flexe_nb_cal_granularity_get;
    dnx_data_nif.flexe.non_5g_slot_bw_get = dnx_data_nif_flexe_non_5g_slot_bw_get;
    dnx_data_nif.flexe.nb_cal_slot_nof_bits_get = dnx_data_nif_flexe_nb_cal_slot_nof_bits_get;
    dnx_data_nif.flexe.flexe_mode_get = dnx_data_nif_flexe_flexe_mode_get;
    dnx_data_nif.flexe.nb_tdm_slot_allocation_mode_get = dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get;
    dnx_data_nif.flexe.device_slot_mode_get = dnx_data_nif_flexe_device_slot_mode_get;

    
    dnx_data_nif.flexe.phy_info_get = dnx_data_nif_flexe_phy_info_get;
    dnx_data_nif.flexe.phy_info_info_get = dnx_data_nif_flexe_phy_info_info_get;
    dnx_data_nif.flexe.supported_pms_get = dnx_data_nif_flexe_supported_pms_get;
    dnx_data_nif.flexe.supported_pms_info_get = dnx_data_nif_flexe_supported_pms_info_get;
    dnx_data_nif.flexe.client_speed_granularity_get = dnx_data_nif_flexe_client_speed_granularity_get;
    dnx_data_nif.flexe.client_speed_granularity_info_get = dnx_data_nif_flexe_client_speed_granularity_info_get;
    dnx_data_nif.flexe.nb_tx_delay_table_get = dnx_data_nif_flexe_nb_tx_delay_table_get;
    dnx_data_nif.flexe.nb_tx_delay_table_info_get = dnx_data_nif_flexe_nb_tx_delay_table_info_get;
    
    
    dnx_data_nif.wbc.feature_get = dnx_data_nif_wbc_feature_get;

    

    
    
    
    dnx_data_nif.prd.feature_get = dnx_data_nif_prd_feature_get;

    
    dnx_data_nif.prd.nof_control_frames_get = dnx_data_nif_prd_nof_control_frames_get;
    dnx_data_nif.prd.nof_ether_type_codes_get = dnx_data_nif_prd_nof_ether_type_codes_get;
    dnx_data_nif.prd.nof_tcam_entries_get = dnx_data_nif_prd_nof_tcam_entries_get;
    dnx_data_nif.prd.tcam_key_offset_size_get = dnx_data_nif_prd_tcam_key_offset_size_get;
    dnx_data_nif.prd.nof_mpls_special_labels_get = dnx_data_nif_prd_nof_mpls_special_labels_get;
    dnx_data_nif.prd.nof_priorities_get = dnx_data_nif_prd_nof_priorities_get;
    dnx_data_nif.prd.custom_ether_type_code_min_get = dnx_data_nif_prd_custom_ether_type_code_min_get;
    dnx_data_nif.prd.custom_ether_type_code_max_get = dnx_data_nif_prd_custom_ether_type_code_max_get;
    dnx_data_nif.prd.ether_type_code_max_get = dnx_data_nif_prd_ether_type_code_max_get;
    dnx_data_nif.prd.ether_type_max_get = dnx_data_nif_prd_ether_type_max_get;
    dnx_data_nif.prd.rmc_threshold_max_get = dnx_data_nif_prd_rmc_threshold_max_get;
    dnx_data_nif.prd.cdu_rmc_threshold_max_get = dnx_data_nif_prd_cdu_rmc_threshold_max_get;
    dnx_data_nif.prd.hrf_threshold_max_get = dnx_data_nif_prd_hrf_threshold_max_get;
    dnx_data_nif.prd.mpls_special_label_max_get = dnx_data_nif_prd_mpls_special_label_max_get;
    dnx_data_nif.prd.flex_key_offset_key_max_get = dnx_data_nif_prd_flex_key_offset_key_max_get;
    dnx_data_nif.prd.flex_key_offset_result_max_get = dnx_data_nif_prd_flex_key_offset_result_max_get;
    dnx_data_nif.prd.tpid_max_get = dnx_data_nif_prd_tpid_max_get;
    dnx_data_nif.prd.nof_clu_port_profiles_get = dnx_data_nif_prd_nof_clu_port_profiles_get;
    dnx_data_nif.prd.nof_framer_port_profiles_get = dnx_data_nif_prd_nof_framer_port_profiles_get;
    dnx_data_nif.prd.nof_ofr_port_profiles_get = dnx_data_nif_prd_nof_ofr_port_profiles_get;
    dnx_data_nif.prd.rmc_fifo_2_threshold_resolution_get = dnx_data_nif_prd_rmc_fifo_2_threshold_resolution_get;
    dnx_data_nif.prd.nof_ofr_nif_interfaces_get = dnx_data_nif_prd_nof_ofr_nif_interfaces_get;
    dnx_data_nif.prd.nof_prd_counters_get = dnx_data_nif_prd_nof_prd_counters_get;

    
    dnx_data_nif.prd.ether_type_get = dnx_data_nif_prd_ether_type_get;
    dnx_data_nif.prd.ether_type_info_get = dnx_data_nif_prd_ether_type_info_get;
    dnx_data_nif.prd.port_profile_map_get = dnx_data_nif_prd_port_profile_map_get;
    dnx_data_nif.prd.port_profile_map_info_get = dnx_data_nif_prd_port_profile_map_info_get;
    
    
    dnx_data_nif.portmod.feature_get = dnx_data_nif_portmod_feature_get;

    
    dnx_data_nif.portmod.pm_types_nof_get = dnx_data_nif_portmod_pm_types_nof_get;

    
    dnx_data_nif.portmod.pm_types_and_interfaces_get = dnx_data_nif_portmod_pm_types_and_interfaces_get;
    dnx_data_nif.portmod.pm_types_and_interfaces_info_get = dnx_data_nif_portmod_pm_types_and_interfaces_info_get;
    
    
    dnx_data_nif.scheduler.feature_get = dnx_data_nif_scheduler_feature_get;

    
    dnx_data_nif.scheduler.rate_per_ethu_bit_get = dnx_data_nif_scheduler_rate_per_ethu_bit_get;
    dnx_data_nif.scheduler.rate_per_ilu_bit_get = dnx_data_nif_scheduler_rate_per_ilu_bit_get;
    dnx_data_nif.scheduler.rate_per_cdu_rmc_bit_get = dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_get;
    dnx_data_nif.scheduler.rate_per_clu_rmc_bit_get = dnx_data_nif_scheduler_rate_per_clu_rmc_bit_get;
    dnx_data_nif.scheduler.nof_weight_bits_get = dnx_data_nif_scheduler_nof_weight_bits_get;
    dnx_data_nif.scheduler.nof_rmc_bits_get = dnx_data_nif_scheduler_nof_rmc_bits_get;
    dnx_data_nif.scheduler.dynamic_scheduling_rate_resolution_get = dnx_data_nif_scheduler_dynamic_scheduling_rate_resolution_get;
    dnx_data_nif.scheduler.rmc_map_table_nof_entries_get = dnx_data_nif_scheduler_rmc_map_table_nof_entries_get;
    dnx_data_nif.scheduler.rmc_map_table_entry_resolution_get = dnx_data_nif_scheduler_rmc_map_table_entry_resolution_get;

    
    dnx_data_nif.scheduler.rx_scheduler_rmc_entries_map_get = dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_get;
    dnx_data_nif.scheduler.rx_scheduler_rmc_entries_map_info_get = dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_info_get;
    
    
    dnx_data_nif.dbal.feature_get = dnx_data_nif_dbal_feature_get;

    
    dnx_data_nif.dbal.cdu_rx_rmc_entry_size_get = dnx_data_nif_dbal_cdu_rx_rmc_entry_size_get;
    dnx_data_nif.dbal.clu_rx_rmc_entry_size_get = dnx_data_nif_dbal_clu_rx_rmc_entry_size_get;
    dnx_data_nif.dbal.cdu_tx_start_threshold_entry_size_get = dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_get;
    dnx_data_nif.dbal.clu_tx_start_threshold_entry_size_get = dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_get;
    dnx_data_nif.dbal.nbu_tx_start_threshold_entry_size_get = dnx_data_nif_dbal_nbu_tx_start_threshold_entry_size_get;
    dnx_data_nif.dbal.mgu_tx_start_threshold_entry_size_get = dnx_data_nif_dbal_mgu_tx_start_threshold_entry_size_get;
    dnx_data_nif.dbal.nbu_tx_threshold_after_ovf_entry_size_get = dnx_data_nif_dbal_nbu_tx_threshold_after_ovf_entry_size_get;
    dnx_data_nif.dbal.mgu_tx_threshold_after_ovf_entry_size_get = dnx_data_nif_dbal_mgu_tx_threshold_after_ovf_entry_size_get;
    dnx_data_nif.dbal.nbu_rx_fifo_srstn_entry_size_get = dnx_data_nif_dbal_nbu_rx_fifo_srstn_entry_size_get;
    dnx_data_nif.dbal.mgu_rx_fifo_srstn_entry_size_get = dnx_data_nif_dbal_mgu_rx_fifo_srstn_entry_size_get;
    dnx_data_nif.dbal.ilu_burst_min_entry_size_get = dnx_data_nif_dbal_ilu_burst_min_entry_size_get;
    dnx_data_nif.dbal.ethu_rx_rmc_counter_entry_size_get = dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get;
    dnx_data_nif.dbal.cdu_lane_fifo_level_and_occupancy_entry_size_get = dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get;
    dnx_data_nif.dbal.clu_lane_fifo_level_and_occupancy_entry_size_get = dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get;
    dnx_data_nif.dbal.nbu_lane_fifo_level_and_occupancy_entry_size_get = dnx_data_nif_dbal_nbu_lane_fifo_level_and_occupancy_entry_size_get;
    dnx_data_nif.dbal.mgu_lane_fifo_level_and_occupancy_entry_size_get = dnx_data_nif_dbal_mgu_lane_fifo_level_and_occupancy_entry_size_get;
    dnx_data_nif.dbal.sch_cnt_dec_threshold_bit_size_get = dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get;
    dnx_data_nif.dbal.tx_credits_bits_get = dnx_data_nif_dbal_tx_credits_bits_get;
    dnx_data_nif.dbal.nof_rx_fifo_counters_get = dnx_data_nif_dbal_nof_rx_fifo_counters_get;
    dnx_data_nif.dbal.nof_tx_fifo_counters_get = dnx_data_nif_dbal_nof_tx_fifo_counters_get;

    
    
    
    dnx_data_nif.features.feature_get = dnx_data_nif_features_feature_get;

    

    
    
    
    dnx_data_nif.arb.feature_get = dnx_data_nif_arb_feature_get;

    
    dnx_data_nif.arb.nof_contexts_get = dnx_data_nif_arb_nof_contexts_get;
    dnx_data_nif.arb.nof_rx_qpms_get = dnx_data_nif_arb_nof_rx_qpms_get;
    dnx_data_nif.arb.nof_rx_ppms_in_qpm_get = dnx_data_nif_arb_nof_rx_ppms_in_qpm_get;
    dnx_data_nif.arb.nof_lanes_in_qpm_get = dnx_data_nif_arb_nof_lanes_in_qpm_get;
    dnx_data_nif.arb.nof_map_destinations_get = dnx_data_nif_arb_nof_map_destinations_get;
    dnx_data_nif.arb.max_calendar_length_get = dnx_data_nif_arb_max_calendar_length_get;
    dnx_data_nif.arb.nof_rx_sources_get = dnx_data_nif_arb_nof_rx_sources_get;
    dnx_data_nif.arb.rx_sch_calendar_nof_modes_get = dnx_data_nif_arb_rx_sch_calendar_nof_modes_get;
    dnx_data_nif.arb.rx_qpm_calendar_length_get = dnx_data_nif_arb_rx_qpm_calendar_length_get;
    dnx_data_nif.arb.rx_sch_calendar_length_get = dnx_data_nif_arb_rx_sch_calendar_length_get;
    dnx_data_nif.arb.cdpm_calendar_length_get = dnx_data_nif_arb_cdpm_calendar_length_get;
    dnx_data_nif.arb.tmac_calendar_length_get = dnx_data_nif_arb_tmac_calendar_length_get;
    dnx_data_nif.arb.rx_qpm_calendar_nof_clients_get = dnx_data_nif_arb_rx_qpm_calendar_nof_clients_get;
    dnx_data_nif.arb.rx_sch_calendar_nof_clients_get = dnx_data_nif_arb_rx_sch_calendar_nof_clients_get;
    dnx_data_nif.arb.rx_qpm_port_speed_granularity_get = dnx_data_nif_arb_rx_qpm_port_speed_granularity_get;
    dnx_data_nif.arb.rx_sch_port_speed_granularity_get = dnx_data_nif_arb_rx_sch_port_speed_granularity_get;
    dnx_data_nif.arb.tx_eth_port_speed_granularity_get = dnx_data_nif_arb_tx_eth_port_speed_granularity_get;
    dnx_data_nif.arb.tx_tmac_port_speed_granularity_get = dnx_data_nif_arb_tx_tmac_port_speed_granularity_get;
    dnx_data_nif.arb.tx_tmac_nof_sections_get = dnx_data_nif_arb_tx_tmac_nof_sections_get;
    dnx_data_nif.arb.tx_tmac_link_list_speed_granularity_get = dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_get;
    dnx_data_nif.arb.nof_entries_in_memory_row_for_rx_calendar_get = dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_get;
    dnx_data_nif.arb.nof_bit_per_entry_in_rx_calendar_get = dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_get;
    dnx_data_nif.arb.nof_entries_in_tmac_calendar_get = dnx_data_nif_arb_nof_entries_in_tmac_calendar_get;
    dnx_data_nif.arb.nof_bit_per_entry_in_tmac_calendar_get = dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_get;
    dnx_data_nif.arb.nof_bit_per_entry_in_ppm_calendar_get = dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_get;
    dnx_data_nif.arb.min_port_speed_for_link_list_section_get = dnx_data_nif_arb_min_port_speed_for_link_list_section_get;
    dnx_data_nif.arb.link_list_sections_denominator_get = dnx_data_nif_arb_link_list_sections_denominator_get;
    dnx_data_nif.arb.high_start_tx_speed_threshold_get = dnx_data_nif_arb_high_start_tx_speed_threshold_get;
    dnx_data_nif.arb.low_start_tx_speed_threshold_get = dnx_data_nif_arb_low_start_tx_speed_threshold_get;
    dnx_data_nif.arb.high_start_tx_speed_value_get = dnx_data_nif_arb_high_start_tx_speed_value_get;
    dnx_data_nif.arb.middle_start_tx_speed_value_get = dnx_data_nif_arb_middle_start_tx_speed_value_get;
    dnx_data_nif.arb.low_start_tx_speed_value_get = dnx_data_nif_arb_low_start_tx_speed_value_get;
    dnx_data_nif.arb.max_value_for_tx_start_threshold_get = dnx_data_nif_arb_max_value_for_tx_start_threshold_get;
    dnx_data_nif.arb.rx_pm_default_min_pkt_size_get = dnx_data_nif_arb_rx_pm_default_min_pkt_size_get;

    
    
    
    dnx_data_nif.ofr.feature_get = dnx_data_nif_ofr_feature_get;

    
    dnx_data_nif.ofr.nof_contexts_get = dnx_data_nif_ofr_nof_contexts_get;
    dnx_data_nif.ofr.nof_rmc_per_priority_group_get = dnx_data_nif_ofr_nof_rmc_per_priority_group_get;
    dnx_data_nif.ofr.nof_total_rmcs_get = dnx_data_nif_ofr_nof_total_rmcs_get;
    dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get = dnx_data_nif_ofr_nof_rx_mem_sections_per_group_get;
    dnx_data_nif.ofr.nof_rx_memory_groups_get = dnx_data_nif_ofr_nof_rx_memory_groups_get;
    dnx_data_nif.ofr.nof_rx_memory_sections_get = dnx_data_nif_ofr_nof_rx_memory_sections_get;
    dnx_data_nif.ofr.nof_rx_memory_entries_per_section_get = dnx_data_nif_ofr_nof_rx_memory_entries_per_section_get;
    dnx_data_nif.ofr.rx_memory_link_list_speed_granularity_get = dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_get;
    dnx_data_nif.ofr.rx_sch_granularity_get = dnx_data_nif_ofr_rx_sch_granularity_get;
    dnx_data_nif.ofr.nof_bytes_per_memory_section_get = dnx_data_nif_ofr_nof_bytes_per_memory_section_get;

    
    
    
    dnx_data_nif.oft.feature_get = dnx_data_nif_oft_feature_get;

    
    dnx_data_nif.oft.calendar_length_get = dnx_data_nif_oft_calendar_length_get;
    dnx_data_nif.oft.nof_sections_get = dnx_data_nif_oft_nof_sections_get;
    dnx_data_nif.oft.nof_internal_calendar_entries_get = dnx_data_nif_oft_nof_internal_calendar_entries_get;
    dnx_data_nif.oft.nof_bit_per_internal_entry_in_calendar_get = dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_get;
    dnx_data_nif.oft.nof_contexts_get = dnx_data_nif_oft_nof_contexts_get;
    dnx_data_nif.oft.port_speed_granularity_get = dnx_data_nif_oft_port_speed_granularity_get;
    dnx_data_nif.oft.calendar_speed_granularity_get = dnx_data_nif_oft_calendar_speed_granularity_get;

    
    
    
    dnx_data_nif.signal_quality.feature_get = dnx_data_nif_signal_quality_feature_get;

    
    dnx_data_nif.signal_quality.is_debug_enabled_get = dnx_data_nif_signal_quality_is_debug_enabled_get;
    dnx_data_nif.signal_quality.max_degrade_threshold_get = dnx_data_nif_signal_quality_max_degrade_threshold_get;
    dnx_data_nif.signal_quality.min_degrade_threshold_get = dnx_data_nif_signal_quality_min_degrade_threshold_get;
    dnx_data_nif.signal_quality.max_fail_threshold_get = dnx_data_nif_signal_quality_max_fail_threshold_get;
    dnx_data_nif.signal_quality.min_fail_threshold_get = dnx_data_nif_signal_quality_min_fail_threshold_get;
    dnx_data_nif.signal_quality.max_interval_get = dnx_data_nif_signal_quality_max_interval_get;
    dnx_data_nif.signal_quality.min_interval_get = dnx_data_nif_signal_quality_min_interval_get;

    
    dnx_data_nif.signal_quality.supported_fecs_get = dnx_data_nif_signal_quality_supported_fecs_get;
    dnx_data_nif.signal_quality.supported_fecs_info_get = dnx_data_nif_signal_quality_supported_fecs_info_get;
    dnx_data_nif.signal_quality.hw_based_solution_get = dnx_data_nif_signal_quality_hw_based_solution_get;
    dnx_data_nif.signal_quality.hw_based_solution_info_get = dnx_data_nif_signal_quality_hw_based_solution_info_get;
    
    
    dnx_data_nif.otn.feature_get = dnx_data_nif_otn_feature_get;

    
    dnx_data_nif.otn.nof_units_get = dnx_data_nif_otn_nof_units_get;
    dnx_data_nif.otn.nof_lanes_get = dnx_data_nif_otn_nof_lanes_get;
    dnx_data_nif.otn.nof_groups_get = dnx_data_nif_otn_nof_groups_get;
    dnx_data_nif.otn.nof_lanes_per_group_get = dnx_data_nif_otn_nof_lanes_per_group_get;

    
    dnx_data_nif.otn.supported_interfaces_get = dnx_data_nif_otn_supported_interfaces_get;
    dnx_data_nif.otn.supported_interfaces_info_get = dnx_data_nif_otn_supported_interfaces_info_get;
    
    
    dnx_data_nif.flr.feature_get = dnx_data_nif_flr_feature_get;

    
    dnx_data_nif.flr.max_tick_unit_get = dnx_data_nif_flr_max_tick_unit_get;
    dnx_data_nif.flr.max_tick_count_get = dnx_data_nif_flr_max_tick_count_get;
    dnx_data_nif.flr.timer_granularity_get = dnx_data_nif_flr_timer_granularity_get;

    
    
    
    
    dnx_data_oam.general.feature_get = dnx_data_oam_general_feature_get;

    
    dnx_data_oam.general.oam_nof_non_acc_action_profiles_get = dnx_data_oam_general_oam_nof_non_acc_action_profiles_get;
    dnx_data_oam.general.oam_nof_acc_action_profiles_get = dnx_data_oam_general_oam_nof_acc_action_profiles_get;
    dnx_data_oam.general.oam_nof_reserved_egr_acc_action_profiles_get = dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_get;
    dnx_data_oam.general.oam_egr_acc_action_profile_id_for_inject_mpls_get = dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_get;
    dnx_data_oam.general.oam_nof_bits_non_acc_action_profiles_get = dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_get;
    dnx_data_oam.general.oam_nof_bits_acc_action_profiles_get = dnx_data_oam_general_oam_nof_bits_acc_action_profiles_get;
    dnx_data_oam.general.oam_nof_bits_oam_lif_db_key_base_ingress_get = dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_get;
    dnx_data_oam.general.oam_nof_bits_oam_lif_db_key_base_egress_get = dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_get;
    dnx_data_oam.general.oam_nof_endpoints_get = dnx_data_oam_general_oam_nof_endpoints_get;
    dnx_data_oam.general.oam_nof_oamp_meps_get = dnx_data_oam_general_oam_nof_oamp_meps_get;
    dnx_data_oam.general.oam_nof_groups_get = dnx_data_oam_general_oam_nof_groups_get;
    dnx_data_oam.general.oam_nof_entries_oam_lif_db_get = dnx_data_oam_general_oam_nof_entries_oam_lif_db_get;
    dnx_data_oam.general.oam_nof_bits_counter_base_get = dnx_data_oam_general_oam_nof_bits_counter_base_get;
    dnx_data_oam.general.oam_nof_punt_error_codes_get = dnx_data_oam_general_oam_nof_punt_error_codes_get;
    dnx_data_oam.general.oam_nof_priority_map_profiles_get = dnx_data_oam_general_oam_nof_priority_map_profiles_get;
    dnx_data_oam.general.oam_force_interface_for_meps_without_counter_get = dnx_data_oam_general_oam_force_interface_for_meps_without_counter_get;
    dnx_data_oam.general.oam_my_cfm_mac_by_range_get = dnx_data_oam_general_oam_my_cfm_mac_by_range_get;
    dnx_data_oam.general.oam_nof_pemla_channel_types_get = dnx_data_oam_general_oam_nof_pemla_channel_types_get;
    dnx_data_oam.general.oam_mdb_rmep_db_profile_limitation_get = dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_get;
    dnx_data_oam.general.oam_lm_read_index_field_exists_get = dnx_data_oam_general_oam_lm_read_index_field_exists_get;
    dnx_data_oam.general.oam_lm_read_per_sub_enable_field_exists_get = dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_get;
    dnx_data_oam.general.oam_lm_read_per_sub_disable_field_exists_get = dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_get;
    dnx_data_oam.general.oam_identification_appdb_1_field_exists_get = dnx_data_oam_general_oam_identification_appdb_1_field_exists_get;
    dnx_data_oam.general.oam_ntp_freq_control_word_get = dnx_data_oam_general_oam_ntp_freq_control_word_get;
    dnx_data_oam.general.oam_ntp_time_freq_control_get = dnx_data_oam_general_oam_ntp_time_freq_control_get;
    dnx_data_oam.general.oam_IEEE_1588_time_freq_control_get = dnx_data_oam_general_oam_IEEE_1588_time_freq_control_get;
    dnx_data_oam.general.oam_nse_nco_freq_control_get = dnx_data_oam_general_oam_nse_nco_freq_control_get;
    dnx_data_oam.general.oam_nof_labels_for_sd_on_p_node_get = dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_get;
    dnx_data_oam.general.oam_tod_config_in_eci_get = dnx_data_oam_general_oam_tod_config_in_eci_get;
    dnx_data_oam.general.oam_tod_config_in_ns_get = dnx_data_oam_general_oam_tod_config_in_ns_get;
    dnx_data_oam.general.oam_tod_config_in_ns_version_get = dnx_data_oam_general_oam_tod_config_in_ns_version_get;
    dnx_data_oam.general.oam_tod_config_in_ns_nof_bits_in_load_enable_get = dnx_data_oam_general_oam_tod_config_in_ns_nof_bits_in_load_enable_get;
    dnx_data_oam.general.tod_init_in_oam_get = dnx_data_oam_general_tod_init_in_oam_get;
    dnx_data_oam.general.oam_ns_freq_control_upper_get = dnx_data_oam_general_oam_ns_freq_control_upper_get;
    dnx_data_oam.general.oam_local_outlif_msb_bit_index_get = dnx_data_oam_general_oam_local_outlif_msb_bit_index_get;
    dnx_data_oam.general.oam_mpls_tp_or_bfd_enable_map_exists_get = dnx_data_oam_general_oam_mpls_tp_or_bfd_enable_map_exists_get;
    dnx_data_oam.general.oam_nof_counter_interfaces_get = dnx_data_oam_general_oam_nof_counter_interfaces_get;
    dnx_data_oam.general.oam_sub_type_v2_get = dnx_data_oam_general_oam_sub_type_v2_get;
    dnx_data_oam.general.oam_nof_server_device_profile_get = dnx_data_oam_general_oam_nof_server_device_profile_get;
    dnx_data_oam.general.oam_app_id_init_get = dnx_data_oam_general_oam_app_id_init_get;
    dnx_data_oam.general.oam_slm_dedicated_profile_supported_get = dnx_data_oam_general_oam_slm_dedicated_profile_supported_get;
    dnx_data_oam.general.oam_tcam_key_pkt_data_at_fwd_header_get = dnx_data_oam_general_oam_tcam_key_pkt_data_at_fwd_header_get;
    dnx_data_oam.general.oam_trap_action_profile_nof_bits_get = dnx_data_oam_general_oam_trap_action_profile_nof_bits_get;
    dnx_data_oam.general.bfd_nof_cluster_profile_get = dnx_data_oam_general_bfd_nof_cluster_profile_get;

    
    
    
    dnx_data_oam.oamp.feature_get = dnx_data_oam_oamp_feature_get;

    
    dnx_data_oam.oamp.nof_mep_db_entry_banks_get = dnx_data_oam_oamp_nof_mep_db_entry_banks_get;
    dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get = dnx_data_oam_oamp_nof_mep_db_entries_per_bank_get;
    dnx_data_oam.oamp.scan_rate_bank_exist_get = dnx_data_oam_oamp_scan_rate_bank_exist_get;
    dnx_data_oam.oamp.nof_mep_db_entries_get = dnx_data_oam_oamp_nof_mep_db_entries_get;
    dnx_data_oam.oamp.max_nof_endpoint_id_get = dnx_data_oam_oamp_max_nof_endpoint_id_get;
    dnx_data_oam.oamp.max_nof_mep_id_get = dnx_data_oam_oamp_max_nof_mep_id_get;
    dnx_data_oam.oamp.max_value_of_mep_id_get = dnx_data_oam_oamp_max_value_of_mep_id_get;
    dnx_data_oam.oamp.nof_umc_get = dnx_data_oam_oamp_nof_umc_get;
    dnx_data_oam.oamp.mep_id_nof_bits_get = dnx_data_oam_oamp_mep_id_nof_bits_get;
    dnx_data_oam.oamp.mep_id_in_rx_pkt_nof_bits_get = dnx_data_oam_oamp_mep_id_in_rx_pkt_nof_bits_get;
    dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get = dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_get;
    dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get = dnx_data_oam_oamp_nof_mep_db_endpoint_entries_get;
    dnx_data_oam.oamp.mep_db_da_is_mc_for_short_entries_support_get = dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_get;
    dnx_data_oam.oamp.nof_icc_profiles_get = dnx_data_oam_oamp_nof_icc_profiles_get;
    dnx_data_oam.oamp.local_port_2_system_port_size_get = dnx_data_oam_oamp_local_port_2_system_port_size_get;
    dnx_data_oam.oamp.oam_nof_sa_mac_profiles_get = dnx_data_oam_oamp_oam_nof_sa_mac_profiles_get;
    dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get = dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_get;
    dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get = dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_get;
    dnx_data_oam.oamp.oam_nof_tpid_profiles_get = dnx_data_oam_oamp_oam_nof_tpid_profiles_get;
    dnx_data_oam.oamp.oamp_pe_default_instruction_index_get = dnx_data_oam_oamp_oamp_pe_default_instruction_index_get;
    dnx_data_oam.oamp.oamp_pe_max_nof_instructions_get = dnx_data_oam_oamp_oamp_pe_max_nof_instructions_get;
    dnx_data_oam.oamp.oamp_pe_nof_fem_bits_get = dnx_data_oam_oamp_oamp_pe_nof_fem_bits_get;
    dnx_data_oam.oamp.mep_db_sub_entry_size_get = dnx_data_oam_oamp_mep_db_sub_entry_size_get;
    dnx_data_oam.oamp.mep_db_nof_sub_entries_get = dnx_data_oam_oamp_mep_db_nof_sub_entries_get;
    dnx_data_oam.oamp.mep_db_full_entry_size_get = dnx_data_oam_oamp_mep_db_full_entry_size_get;
    dnx_data_oam.oamp.mep_pe_profile_nof_bits_get = dnx_data_oam_oamp_mep_pe_profile_nof_bits_get;
    dnx_data_oam.oamp.rmep_db_entries_nof_bits_get = dnx_data_oam_oamp_rmep_db_entries_nof_bits_get;
    dnx_data_oam.oamp.rmep_db_rmep_index_nof_bits_get = dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_get;
    dnx_data_oam.oamp.rmep_max_self_contained_get = dnx_data_oam_oamp_rmep_max_self_contained_get;
    dnx_data_oam.oamp.rmep_pair_sys_clocks_get = dnx_data_oam_oamp_rmep_pair_sys_clocks_get;
    dnx_data_oam.oamp.mep_sys_clocks_get = dnx_data_oam_oamp_mep_sys_clocks_get;
    dnx_data_oam.oamp.num_clocks_scan_nof_bits_get = dnx_data_oam_oamp_num_clocks_scan_nof_bits_get;
    dnx_data_oam.oamp.oamp_scan_sycles_in_us_get = dnx_data_oam_oamp_oamp_scan_sycles_in_us_get;
    dnx_data_oam.oamp.oamp_scan_sycles_in_ms_get = dnx_data_oam_oamp_oamp_scan_sycles_in_ms_get;
    dnx_data_oam.oamp.mep_db_version_limitation_get = dnx_data_oam_oamp_mep_db_version_limitation_get;
    dnx_data_oam.oamp.oamp_mep_db_threshold_version_limitation_get = dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_get;
    dnx_data_oam.oamp.rmep_db_non_zero_lifetime_units_limitation_get = dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_get;
    dnx_data_oam.oamp.crc_calculation_supported_get = dnx_data_oam_oamp_crc_calculation_supported_get;
    dnx_data_oam.oamp.modified_mep_db_structure_get = dnx_data_oam_oamp_modified_mep_db_structure_get;
    dnx_data_oam.oamp.local_port_2_system_port_bits_get = dnx_data_oam_oamp_local_port_2_system_port_bits_get;
    dnx_data_oam.oamp.mdb_nof_access_ids_get = dnx_data_oam_oamp_mdb_nof_access_ids_get;
    dnx_data_oam.oamp.bfd_mpls_short_mdb_reserved_bits_get = dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_get;
    dnx_data_oam.oamp.bfd_m_hop_short_mdb_reserved_bits_get = dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_get;
    dnx_data_oam.oamp.bfd_pwe_short_mdb_reserved_bits_get = dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_get;
    dnx_data_oam.oamp.ccm_eth_offload_mdb_reserved_bits_get = dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_get;
    dnx_data_oam.oamp.bfd_1_hop_short_mdb_reserved_bits_get = dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_get;
    dnx_data_oam.oamp.ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get = dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get;
    dnx_data_oam.oamp.punt_packet_destination_size_get = dnx_data_oam_oamp_punt_packet_destination_size_get;
    dnx_data_oam.oamp.punt_packet_itmh_destination_full_encoding_supported_get = dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_get;
    dnx_data_oam.oamp.core_nof_bits_get = dnx_data_oam_oamp_core_nof_bits_get;
    dnx_data_oam.oamp.punt_packet_pph_ttl_supported_get = dnx_data_oam_oamp_punt_packet_pph_ttl_supported_get;
    dnx_data_oam.oamp.lmm_dmm_supported_get = dnx_data_oam_oamp_lmm_dmm_supported_get;
    dnx_data_oam.oamp.slm_supported_get = dnx_data_oam_oamp_slm_supported_get;
    dnx_data_oam.oamp.oamp_sat_rxi_enable_support_get = dnx_data_oam_oamp_oamp_sat_rxi_enable_support_get;
    dnx_data_oam.oamp.mdb_tx_rate_lsb_field_present_get = dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_get;
    dnx_data_oam.oamp.nof_jumbo_dm_tlv_entries_get = dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_get;
    dnx_data_oam.oamp.mdb_reserved_field_size_calculation_get = dnx_data_oam_oamp_mdb_reserved_field_size_calculation_get;
    dnx_data_oam.oamp.event_fifo_slicing_size_offset_get = dnx_data_oam_oamp_event_fifo_slicing_size_offset_get;
    dnx_data_oam.oamp.ccm_piggyback_per_priority_with_crc_supported_get = dnx_data_oam_oamp_ccm_piggyback_per_priority_with_crc_supported_get;
    dnx_data_oam.oamp.nof_write_complete_checks_get = dnx_data_oam_oamp_nof_write_complete_checks_get;
    dnx_data_oam.oamp.oam_nof_trap_code_tcam_get = dnx_data_oam_oamp_oam_nof_trap_code_tcam_get;
    dnx_data_oam.oamp.mep_rmep_db_access_regs_get = dnx_data_oam_oamp_mep_rmep_db_access_regs_get;
    dnx_data_oam.oamp.dev_type_for_server_tests_get = dnx_data_oam_oamp_dev_type_for_server_tests_get;
    dnx_data_oam.oamp.protection_packet_max_header_size_get = dnx_data_oam_oamp_protection_packet_max_header_size_get;
    dnx_data_oam.oamp.protection_packet_max_len_size_get = dnx_data_oam_oamp_protection_packet_max_len_size_get;
    dnx_data_oam.oamp.max_number_of_events_get = dnx_data_oam_oamp_max_number_of_events_get;
    dnx_data_oam.oamp.nof_generic_memory_entries_get = dnx_data_oam_oamp_nof_generic_memory_entries_get;
    dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get = dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_get;
    dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get = dnx_data_oam_oamp_oamp_mep_full_entry_threshold_get;
    dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get = dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_get;
    dnx_data_oam.oamp.mep_db_jr1_mode_get = dnx_data_oam_oamp_mep_db_jr1_mode_get;
    dnx_data_oam.oamp.oamp_fifo_dma_event_interface_enable_get = dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_get;
    dnx_data_oam.oamp.oamp_fifo_dma_event_interface_buffer_size_get = dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_get;
    dnx_data_oam.oamp.oamp_fifo_dma_event_interface_timeout_get = dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_get;
    dnx_data_oam.oamp.oamp_fifo_dma_event_interface_threshold_get = dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_get;
    dnx_data_oam.oamp.oamp_fifo_dma_event_thread_priority_get = dnx_data_oam_oamp_oamp_fifo_dma_event_thread_priority_get;
    dnx_data_oam.oamp.oamp_fifo_dma_report_interface_enable_get = dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_get;
    dnx_data_oam.oamp.oamp_fifo_dma_report_interface_buffer_size_get = dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_get;
    dnx_data_oam.oamp.oamp_fifo_dma_report_interface_timeout_get = dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_get;
    dnx_data_oam.oamp.oamp_fifo_dma_report_interface_threshold_get = dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_get;
    dnx_data_oam.oamp.oamp_dual_mode_get = dnx_data_oam_oamp_oamp_dual_mode_get;
    dnx_data_oam.oamp.up_mep_tod_recycle_port_core_0_get = dnx_data_oam_oamp_up_mep_tod_recycle_port_core_0_get;
    dnx_data_oam.oamp.up_mep_tod_recycle_port_core_1_get = dnx_data_oam_oamp_up_mep_tod_recycle_port_core_1_get;
    dnx_data_oam.oamp.server_destination_msb_bits_get = dnx_data_oam_oamp_server_destination_msb_bits_get;
    dnx_data_oam.oamp.server_destination_lsb_bits_get = dnx_data_oam_oamp_server_destination_lsb_bits_get;
    dnx_data_oam.oamp.intr_oamp_pending_event_get = dnx_data_oam_oamp_intr_oamp_pending_event_get;
    dnx_data_oam.oamp.intr_oamp_stat_pending_event_get = dnx_data_oam_oamp_intr_oamp_stat_pending_event_get;

    
    
    
    dnx_data_oam.property.feature_get = dnx_data_oam_property_feature_get;

    
    dnx_data_oam.property.oam_injected_over_lsp_cnt_get = dnx_data_oam_property_oam_injected_over_lsp_cnt_get;
    dnx_data_oam.property.lm_dm_per_priority_enable_get = dnx_data_oam_property_lm_dm_per_priority_enable_get;

    
    
    
    dnx_data_oam.feature.feature_get = dnx_data_oam_feature_feature_get;

    

    
    
    
    
    dnx_data_onesync.general.feature_get = dnx_data_onesync_general_feature_get;

    

    
    
    
    dnx_data_onesync.onesync_firmware.feature_get = dnx_data_onesync_onesync_firmware_feature_get;

    
    dnx_data_onesync.onesync_firmware.knetsync_cosq_port_get = dnx_data_onesync_onesync_firmware_knetsync_cosq_port_get;

    
    
    
    
    dnx_data_parser.layer.feature_get = dnx_data_parser_layer_feature_get;

    
    dnx_data_parser.layer.gre_collapsed_into_ip_get = dnx_data_parser_layer_gre_collapsed_into_ip_get;

    
    
    
    
    dnx_data_pll.general.feature_get = dnx_data_pll_general_feature_get;

    
    dnx_data_pll.general.ts_ptp_fref_get = dnx_data_pll_general_ts_ptp_fref_get;

    
    dnx_data_pll.general.nif_pll_get = dnx_data_pll_general_nif_pll_get;
    dnx_data_pll.general.nif_pll_info_get = dnx_data_pll_general_nif_pll_info_get;
    dnx_data_pll.general.fabric_pll_get = dnx_data_pll_general_fabric_pll_get;
    dnx_data_pll.general.fabric_pll_info_get = dnx_data_pll_general_fabric_pll_info_get;
    
    
    dnx_data_pll.pll1.feature_get = dnx_data_pll_pll1_feature_get;

    
    dnx_data_pll.pll1.nof_pll1_get = dnx_data_pll_pll1_nof_pll1_get;
    dnx_data_pll.pll1.ts_pll1_id_get = dnx_data_pll_pll1_ts_pll1_id_get;
    dnx_data_pll.pll1.bs_pll1_id_get = dnx_data_pll_pll1_bs_pll1_id_get;
    dnx_data_pll.pll1.fab_pll1_id_get = dnx_data_pll_pll1_fab_pll1_id_get;
    dnx_data_pll.pll1.nif_pll1_id_get = dnx_data_pll_pll1_nif_pll1_id_get;
    dnx_data_pll.pll1.flexe_pll1_id_get = dnx_data_pll_pll1_flexe_pll1_id_get;

    
    dnx_data_pll.pll1.config_get = dnx_data_pll_pll1_config_get;
    dnx_data_pll.pll1.config_info_get = dnx_data_pll_pll1_config_info_get;
    
    
    dnx_data_pll.pll3.feature_get = dnx_data_pll_pll3_feature_get;

    
    dnx_data_pll.pll3.nof_pll3_get = dnx_data_pll_pll3_nof_pll3_get;
    dnx_data_pll.pll3.vco_clock_get = dnx_data_pll_pll3_vco_clock_get;
    dnx_data_pll.pll3.kp_get = dnx_data_pll_pll3_kp_get;
    dnx_data_pll.pll3.ki_get = dnx_data_pll_pll3_ki_get;
    dnx_data_pll.pll3.en_ctrl_get = dnx_data_pll_pll3_en_ctrl_get;
    dnx_data_pll.pll3.en_ctrl_byp_get = dnx_data_pll_pll3_en_ctrl_byp_get;
    dnx_data_pll.pll3.route_ctr_byp_get = dnx_data_pll_pll3_route_ctr_byp_get;
    dnx_data_pll.pll3.ref_clock_125_get = dnx_data_pll_pll3_ref_clock_125_get;
    dnx_data_pll.pll3.ref_clock_156_25_get = dnx_data_pll_pll3_ref_clock_156_25_get;
    dnx_data_pll.pll3.ref_clock_312_5_get = dnx_data_pll_pll3_ref_clock_312_5_get;
    dnx_data_pll.pll3.nif0_pll3_id_get = dnx_data_pll_pll3_nif0_pll3_id_get;
    dnx_data_pll.pll3.nif1_pll3_id_get = dnx_data_pll_pll3_nif1_pll3_id_get;
    dnx_data_pll.pll3.mas0_pll3_id_get = dnx_data_pll_pll3_mas0_pll3_id_get;
    dnx_data_pll.pll3.mas1_pll3_id_get = dnx_data_pll_pll3_mas1_pll3_id_get;

    
    dnx_data_pll.pll3.config_get = dnx_data_pll_pll3_config_get;
    dnx_data_pll.pll3.config_info_get = dnx_data_pll_pll3_config_info_get;
    
    
    dnx_data_pll.pll4.feature_get = dnx_data_pll_pll4_feature_get;

    
    dnx_data_pll.pll4.nof_pll4_get = dnx_data_pll_pll4_nof_pll4_get;
    dnx_data_pll.pll4.ts_pll4_id_get = dnx_data_pll_pll4_ts_pll4_id_get;
    dnx_data_pll.pll4.bs_pll4_id_get = dnx_data_pll_pll4_bs_pll4_id_get;
    dnx_data_pll.pll4.nif_pll4_id_get = dnx_data_pll_pll4_nif_pll4_id_get;
    dnx_data_pll.pll4.fab_pll4_id_get = dnx_data_pll_pll4_fab_pll4_id_get;
    dnx_data_pll.pll4.nif_tsc_pll4_id_get = dnx_data_pll_pll4_nif_tsc_pll4_id_get;
    dnx_data_pll.pll4.framer_pll4_id_get = dnx_data_pll_pll4_framer_pll4_id_get;
    dnx_data_pll.pll4.sar_pll4_id_get = dnx_data_pll_pll4_sar_pll4_id_get;

    
    dnx_data_pll.pll4.config_get = dnx_data_pll_pll4_config_get;
    dnx_data_pll.pll4.config_info_get = dnx_data_pll_pll4_config_info_get;
    
    
    dnx_data_pll.synce_pll.feature_get = dnx_data_pll_synce_pll_feature_get;

    

    
    
    
    
    dnx_data_port.static_add.feature_get = dnx_data_port_static_add_feature_get;

    
    dnx_data_port.static_add.default_speed_for_special_if_get = dnx_data_port_static_add_default_speed_for_special_if_get;
    dnx_data_port.static_add.fabric_fw_load_method_get = dnx_data_port_static_add_fabric_fw_load_method_get;
    dnx_data_port.static_add.fabric_fw_crc_check_get = dnx_data_port_static_add_fabric_fw_crc_check_get;
    dnx_data_port.static_add.fabric_fw_load_verify_get = dnx_data_port_static_add_fabric_fw_load_verify_get;
    dnx_data_port.static_add.nif_fw_load_method_get = dnx_data_port_static_add_nif_fw_load_method_get;
    dnx_data_port.static_add.nif_fw_crc_check_get = dnx_data_port_static_add_nif_fw_crc_check_get;
    dnx_data_port.static_add.nif_fw_load_verify_get = dnx_data_port_static_add_nif_fw_load_verify_get;

    
    dnx_data_port.static_add.ucode_port_get = dnx_data_port_static_add_ucode_port_get;
    dnx_data_port.static_add.ucode_port_info_get = dnx_data_port_static_add_ucode_port_info_get;
    dnx_data_port.static_add.speed_get = dnx_data_port_static_add_speed_get;
    dnx_data_port.static_add.speed_info_get = dnx_data_port_static_add_speed_info_get;
    dnx_data_port.static_add.ext_stat_speed_get = dnx_data_port_static_add_ext_stat_speed_get;
    dnx_data_port.static_add.ext_stat_speed_info_get = dnx_data_port_static_add_ext_stat_speed_info_get;
    dnx_data_port.static_add.eth_padding_get = dnx_data_port_static_add_eth_padding_get;
    dnx_data_port.static_add.eth_padding_info_get = dnx_data_port_static_add_eth_padding_info_get;
    dnx_data_port.static_add.link_training_get = dnx_data_port_static_add_link_training_get;
    dnx_data_port.static_add.link_training_info_get = dnx_data_port_static_add_link_training_info_get;
    dnx_data_port.static_add.ext_stat_link_training_get = dnx_data_port_static_add_ext_stat_link_training_get;
    dnx_data_port.static_add.ext_stat_link_training_info_get = dnx_data_port_static_add_ext_stat_link_training_info_get;
    dnx_data_port.static_add.fec_type_get = dnx_data_port_static_add_fec_type_get;
    dnx_data_port.static_add.fec_type_info_get = dnx_data_port_static_add_fec_type_info_get;
    dnx_data_port.static_add.serdes_lane_config_get = dnx_data_port_static_add_serdes_lane_config_get;
    dnx_data_port.static_add.serdes_lane_config_info_get = dnx_data_port_static_add_serdes_lane_config_info_get;
    dnx_data_port.static_add.serdes_tx_taps_get = dnx_data_port_static_add_serdes_tx_taps_get;
    dnx_data_port.static_add.serdes_tx_taps_info_get = dnx_data_port_static_add_serdes_tx_taps_info_get;
    dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get = dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_get;
    dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_info_get = dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_info_get;
    dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get = dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_get;
    dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_info_get = dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_info_get;
    dnx_data_port.static_add.tx_pam4_precoder_get = dnx_data_port_static_add_tx_pam4_precoder_get;
    dnx_data_port.static_add.tx_pam4_precoder_info_get = dnx_data_port_static_add_tx_pam4_precoder_info_get;
    dnx_data_port.static_add.lp_tx_precoder_get = dnx_data_port_static_add_lp_tx_precoder_get;
    dnx_data_port.static_add.lp_tx_precoder_info_get = dnx_data_port_static_add_lp_tx_precoder_info_get;
    dnx_data_port.static_add.fabric_quad_info_get = dnx_data_port_static_add_fabric_quad_info_get;
    dnx_data_port.static_add.fabric_quad_info_info_get = dnx_data_port_static_add_fabric_quad_info_info_get;
    dnx_data_port.static_add.header_type_get = dnx_data_port_static_add_header_type_get;
    dnx_data_port.static_add.header_type_info_get = dnx_data_port_static_add_header_type_info_get;
    dnx_data_port.static_add.erp_exist_get = dnx_data_port_static_add_erp_exist_get;
    dnx_data_port.static_add.erp_exist_info_get = dnx_data_port_static_add_erp_exist_info_get;
    dnx_data_port.static_add.base_flexe_instance_get = dnx_data_port_static_add_base_flexe_instance_get;
    dnx_data_port.static_add.base_flexe_instance_info_get = dnx_data_port_static_add_base_flexe_instance_info_get;
    dnx_data_port.static_add.cable_swap_info_get = dnx_data_port_static_add_cable_swap_info_get;
    dnx_data_port.static_add.cable_swap_info_info_get = dnx_data_port_static_add_cable_swap_info_info_get;
    dnx_data_port.static_add.flr_config_get = dnx_data_port_static_add_flr_config_get;
    dnx_data_port.static_add.flr_config_info_get = dnx_data_port_static_add_flr_config_info_get;
    dnx_data_port.static_add.timestamp_phy_init_get = dnx_data_port_static_add_timestamp_phy_init_get;
    dnx_data_port.static_add.timestamp_phy_init_info_get = dnx_data_port_static_add_timestamp_phy_init_info_get;
    dnx_data_port.static_add.txpi_mode_get = dnx_data_port_static_add_txpi_mode_get;
    dnx_data_port.static_add.txpi_mode_info_get = dnx_data_port_static_add_txpi_mode_info_get;
    
    
    dnx_data_port.general.feature_get = dnx_data_port_general_feature_get;

    
    dnx_data_port.general.erp_pp_dsp_get = dnx_data_port_general_erp_pp_dsp_get;
    dnx_data_port.general.max_nof_channels_get = dnx_data_port_general_max_nof_channels_get;
    dnx_data_port.general.max_nof_mirror_channels_get = dnx_data_port_general_max_nof_mirror_channels_get;
    dnx_data_port.general.fabric_phys_offset_get = dnx_data_port_general_fabric_phys_offset_get;
    dnx_data_port.general.pp_dsp_size_get = dnx_data_port_general_pp_dsp_size_get;
    dnx_data_port.general.ingress_vlan_domain_size_get = dnx_data_port_general_ingress_vlan_domain_size_get;
    dnx_data_port.general.vlan_domain_size_get = dnx_data_port_general_vlan_domain_size_get;
    dnx_data_port.general.ffc_instruction_size_get = dnx_data_port_general_ffc_instruction_size_get;
    dnx_data_port.general.ffc_instruction_offset_size_get = dnx_data_port_general_ffc_instruction_offset_size_get;
    dnx_data_port.general.ffc_instruction_width_size_get = dnx_data_port_general_ffc_instruction_width_size_get;
    dnx_data_port.general.ffc_instruction_input_offset_size_get = dnx_data_port_general_ffc_instruction_input_offset_size_get;
    dnx_data_port.general.max_first_header_size_to_skip_get = dnx_data_port_general_max_first_header_size_to_skip_get;
    dnx_data_port.general.max_first_header_size_to_skip_before_get = dnx_data_port_general_max_first_header_size_to_skip_before_get;
    dnx_data_port.general.max_first_header_size_to_skip_after_get = dnx_data_port_general_max_first_header_size_to_skip_after_get;
    dnx_data_port.general.prt_ffc_width_start_offset_get = dnx_data_port_general_prt_ffc_width_start_offset_get;
    dnx_data_port.general.prt_ffc_start_input_offset_get = dnx_data_port_general_prt_ffc_start_input_offset_get;
    dnx_data_port.general.max_phys_per_pm_get = dnx_data_port_general_max_phys_per_pm_get;
    dnx_data_port.general.max_core_access_per_port_get = dnx_data_port_general_max_core_access_per_port_get;
    dnx_data_port.general.nof_pp_dsp_per_fap_id_get = dnx_data_port_general_nof_pp_dsp_per_fap_id_get;
    dnx_data_port.general.nof_ptc_get = dnx_data_port_general_nof_ptc_get;
    dnx_data_port.general.nof_in_tm_ports_get = dnx_data_port_general_nof_in_tm_ports_get;
    dnx_data_port.general.nof_out_tm_ports_get = dnx_data_port_general_nof_out_tm_ports_get;
    dnx_data_port.general.nof_pp_dsp_get = dnx_data_port_general_nof_pp_dsp_get;
    dnx_data_port.general.nof_pp_ports_get = dnx_data_port_general_nof_pp_ports_get;
    dnx_data_port.general.reserved_pp_dsp_get = dnx_data_port_general_reserved_pp_dsp_get;
    dnx_data_port.general.nof_vlan_membership_if_get = dnx_data_port_general_nof_vlan_membership_if_get;
    dnx_data_port.general.max_vlan_membership_if_get = dnx_data_port_general_max_vlan_membership_if_get;
    dnx_data_port.general.fabric_port_base_get = dnx_data_port_general_fabric_port_base_get;
    dnx_data_port.general.pp_port_bits_size_get = dnx_data_port_general_pp_port_bits_size_get;
    dnx_data_port.general.nof_port_bits_in_pp_bus_get = dnx_data_port_general_nof_port_bits_in_pp_bus_get;
    dnx_data_port.general.nof_bits_egress_vlan_membership_if_get = dnx_data_port_general_nof_bits_egress_vlan_membership_if_get;

    
    dnx_data_port.general.first_header_size_get = dnx_data_port_general_first_header_size_get;
    dnx_data_port.general.first_header_size_info_get = dnx_data_port_general_first_header_size_info_get;
    dnx_data_port.general.fec_compensation_info_get = dnx_data_port_general_fec_compensation_info_get;
    dnx_data_port.general.fec_compensation_info_info_get = dnx_data_port_general_fec_compensation_info_info_get;
    dnx_data_port.general.pm_info_get = dnx_data_port_general_pm_info_get;
    dnx_data_port.general.pm_info_info_get = dnx_data_port_general_pm_info_info_get;
    dnx_data_port.general.phy_controls_features_get = dnx_data_port_general_phy_controls_features_get;
    dnx_data_port.general.phy_controls_features_info_get = dnx_data_port_general_phy_controls_features_info_get;
    
    
    dnx_data_port.imb.feature_get = dnx_data_port_imb_feature_get;

    

    
    dnx_data_port.imb.imb_type_info_get = dnx_data_port_imb_imb_type_info_get;
    dnx_data_port.imb.imb_type_info_info_get = dnx_data_port_imb_imb_type_info_info_get;
    
    
    
    dnx_data_port_pp.general.feature_get = dnx_data_port_pp_general_feature_get;

    
    dnx_data_port_pp.general.nof_ingress_llvp_profiles_get = dnx_data_port_pp_general_nof_ingress_llvp_profiles_get;
    dnx_data_port_pp.general.nof_egress_llvp_profiles_get = dnx_data_port_pp_general_nof_egress_llvp_profiles_get;
    dnx_data_port_pp.general.default_ingress_native_ac_llvp_profile_id_get = dnx_data_port_pp_general_default_ingress_native_ac_llvp_profile_id_get;
    dnx_data_port_pp.general.default_egress_native_ac_llvp_profile_id_get = dnx_data_port_pp_general_default_egress_native_ac_llvp_profile_id_get;
    dnx_data_port_pp.general.nof_egress_sit_profile_get = dnx_data_port_pp_general_nof_egress_sit_profile_get;
    dnx_data_port_pp.general.nof_egress_acceptable_frame_type_profile_get = dnx_data_port_pp_general_nof_egress_acceptable_frame_type_profile_get;
    dnx_data_port_pp.general.sit_inner_eth_additional_tag_get = dnx_data_port_pp_general_sit_inner_eth_additional_tag_get;
    dnx_data_port_pp.general.nof_virtual_port_tcam_entries_get = dnx_data_port_pp_general_nof_virtual_port_tcam_entries_get;

    
    
    
    dnx_data_port_pp.filters.feature_get = dnx_data_port_pp_filters_feature_get;

    

    
    
    
    
    dnx_data_pp.stages.feature_get = dnx_data_pp_stages_feature_get;

    
    dnx_data_pp.stages.is_vtt5_supported_get = dnx_data_pp_stages_is_vtt5_supported_get;
    dnx_data_pp.stages.is_erpp_supported_get = dnx_data_pp_stages_is_erpp_supported_get;

    
    dnx_data_pp.stages.params_get = dnx_data_pp_stages_params_get;
    dnx_data_pp.stages.params_info_get = dnx_data_pp_stages_params_info_get;
    
    
    dnx_data_pp.PEM.feature_get = dnx_data_pp_PEM_feature_get;

    
    dnx_data_pp.PEM.nof_pem_bits_get = dnx_data_pp_PEM_nof_pem_bits_get;
    dnx_data_pp.PEM.nof_pem_blocks_per_core_get = dnx_data_pp_PEM_nof_pem_blocks_per_core_get;

    
    
    
    dnx_data_pp.application.feature_get = dnx_data_pp_application_feature_get;

    
    dnx_data_pp.application.vtt1_lookup_by_exem1_get = dnx_data_pp_application_vtt1_lookup_by_exem1_get;

    
    dnx_data_pp.application.device_image_get = dnx_data_pp_application_device_image_get;
    dnx_data_pp.application.device_image_info_get = dnx_data_pp_application_device_image_info_get;
    dnx_data_pp.application.ucode_get = dnx_data_pp_application_ucode_get;
    dnx_data_pp.application.ucode_info_get = dnx_data_pp_application_ucode_info_get;
    
    
    dnx_data_pp.ETPP.feature_get = dnx_data_pp_ETPP_feature_get;

    
    dnx_data_pp.ETPP.etps_type_size_bits_get = dnx_data_pp_ETPP_etps_type_size_bits_get;
    dnx_data_pp.ETPP.nof_additional_headers_profiles_get = dnx_data_pp_ETPP_nof_additional_headers_profiles_get;
    dnx_data_pp.ETPP.nof_additional_headers_profiles_maps_get = dnx_data_pp_ETPP_nof_additional_headers_profiles_maps_get;
    dnx_data_pp.ETPP.dynamic_additional_headers_profile_get = dnx_data_pp_ETPP_dynamic_additional_headers_profile_get;
    dnx_data_pp.ETPP.ingress_packet_header_size_get = dnx_data_pp_ETPP_ingress_packet_header_size_get;
    dnx_data_pp.ETPP.btc_max_header_to_add_in_bytes_get = dnx_data_pp_ETPP_btc_max_header_to_add_in_bytes_get;

    
    
    
    dnx_data_pp.debug.feature_get = dnx_data_pp_debug_feature_get;

    
    dnx_data_pp.debug.packet_header_data_get = dnx_data_pp_debug_packet_header_data_get;
    dnx_data_pp.debug.ptc_size_get = dnx_data_pp_debug_ptc_size_get;
    dnx_data_pp.debug.valid_bytes_get = dnx_data_pp_debug_valid_bytes_get;
    dnx_data_pp.debug.time_stamp_get = dnx_data_pp_debug_time_stamp_get;

    
    dnx_data_pp.debug.from_signal_str_get = dnx_data_pp_debug_from_signal_str_get;
    dnx_data_pp.debug.from_signal_str_info_get = dnx_data_pp_debug_from_signal_str_info_get;
    
    
    
    dnx_data_ppmc.param.feature_get = dnx_data_ppmc_param_feature_get;

    
    dnx_data_ppmc.param.max_mc_replication_id_get = dnx_data_ppmc_param_max_mc_replication_id_get;
    dnx_data_ppmc.param.min_mc_replication_id_get = dnx_data_ppmc_param_min_mc_replication_id_get;
    dnx_data_ppmc.param.mc_replication_id_offset_get = dnx_data_ppmc_param_mc_replication_id_offset_get;
    dnx_data_ppmc.param.ace_implementation_get = dnx_data_ppmc_param_ace_implementation_get;

    
    
    
    
    dnx_data_ptp.general.feature_get = dnx_data_ptp_general_feature_get;

    
    dnx_data_ptp.general.nof_port_profiles_get = dnx_data_ptp_general_nof_port_profiles_get;

    
    
    
    dnx_data_ptp.ptp_firmware.feature_get = dnx_data_ptp_ptp_firmware_feature_get;

    
    dnx_data_ptp.ptp_firmware.ptp_cosq_port_get = dnx_data_ptp_ptp_firmware_ptp_cosq_port_get;

    
    
    
    
    dnx_data_pvt.general.feature_get = dnx_data_pvt_general_feature_get;

    
    dnx_data_pvt.general.nof_pvt_monitors_get = dnx_data_pvt_general_nof_pvt_monitors_get;
    dnx_data_pvt.general.pvt_base_get = dnx_data_pvt_general_pvt_base_get;
    dnx_data_pvt.general.pvt_factor_get = dnx_data_pvt_general_pvt_factor_get;
    dnx_data_pvt.general.pvt_offset_get = dnx_data_pvt_general_pvt_offset_get;
    dnx_data_pvt.general.nof_main_monitors_get = dnx_data_pvt_general_nof_main_monitors_get;
    dnx_data_pvt.general.pvt_mon_control_sel_nof_bits_get = dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get;
    dnx_data_pvt.general.thermal_data_nof_bits_get = dnx_data_pvt_general_thermal_data_nof_bits_get;
    dnx_data_pvt.general.vol_sel_get = dnx_data_pvt_general_vol_sel_get;
    dnx_data_pvt.general.vol_factor_get = dnx_data_pvt_general_vol_factor_get;
    dnx_data_pvt.general.vol_multiple_get = dnx_data_pvt_general_vol_multiple_get;
    dnx_data_pvt.general.vol_divisor_get = dnx_data_pvt_general_vol_divisor_get;

    
    dnx_data_pvt.general.pvt_name_get = dnx_data_pvt_general_pvt_name_get;
    dnx_data_pvt.general.pvt_name_info_get = dnx_data_pvt_general_pvt_name_info_get;
    
    
    dnx_data_pvt.vtmon.feature_get = dnx_data_pvt_vtmon_feature_get;

    

    
    dnx_data_pvt.vtmon.vtmon_access_get = dnx_data_pvt_vtmon_vtmon_access_get;
    dnx_data_pvt.vtmon.vtmon_access_info_get = dnx_data_pvt_vtmon_vtmon_access_info_get;
    
    
    
    dnx_data_qos.qos.feature_get = dnx_data_qos_qos_feature_get;

    
    dnx_data_qos.qos.nof_ingress_profiles_get = dnx_data_qos_qos_nof_ingress_profiles_get;
    dnx_data_qos.qos.nof_ingress_opcode_profiles_get = dnx_data_qos_qos_nof_ingress_opcode_profiles_get;
    dnx_data_qos.qos.nof_ingress_ecn_profiles_get = dnx_data_qos_qos_nof_ingress_ecn_profiles_get;
    dnx_data_qos.qos.nof_ingress_policer_profiles_get = dnx_data_qos_qos_nof_ingress_policer_profiles_get;
    dnx_data_qos.qos.nof_ingress_vlan_pcp_profiles_get = dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_get;
    dnx_data_qos.qos.nof_egress_profiles_get = dnx_data_qos_qos_nof_egress_profiles_get;
    dnx_data_qos.qos.nof_egress_policer_profiles_get = dnx_data_qos_qos_nof_egress_policer_profiles_get;
    dnx_data_qos.qos.nof_egress_mpls_php_profiles_get = dnx_data_qos_qos_nof_egress_mpls_php_profiles_get;
    dnx_data_qos.qos.nof_egress_opcode_profiles_get = dnx_data_qos_qos_nof_egress_opcode_profiles_get;
    dnx_data_qos.qos.nof_egress_php_opcode_profiles_get = dnx_data_qos_qos_nof_egress_php_opcode_profiles_get;
    dnx_data_qos.qos.nof_egress_ttl_pipe_profiles_get = dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_get;
    dnx_data_qos.qos.nof_ingress_propagation_profiles_get = dnx_data_qos_qos_nof_ingress_propagation_profiles_get;
    dnx_data_qos.qos.nof_ingress_ecn_opcode_profiles_get = dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_get;
    dnx_data_qos.qos.nof_egress_network_qos_pipe_profiles_get = dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_get;
    dnx_data_qos.qos.nof_egress_cos_profiles_get = dnx_data_qos_qos_nof_egress_cos_profiles_get;
    dnx_data_qos.qos.packet_max_priority_get = dnx_data_qos_qos_packet_max_priority_get;
    dnx_data_qos.qos.packet_max_exp_get = dnx_data_qos_qos_packet_max_exp_get;
    dnx_data_qos.qos.packet_max_dscp_get = dnx_data_qos_qos_packet_max_dscp_get;
    dnx_data_qos.qos.packet_max_cfi_get = dnx_data_qos_qos_packet_max_cfi_get;
    dnx_data_qos.qos.packet_max_tc_get = dnx_data_qos_qos_packet_max_tc_get;
    dnx_data_qos.qos.packet_max_dp_get = dnx_data_qos_qos_packet_max_dp_get;
    dnx_data_qos.qos.packet_max_ecn_get = dnx_data_qos_qos_packet_max_ecn_get;
    dnx_data_qos.qos.packet_max_egress_policer_offset_get = dnx_data_qos_qos_packet_max_egress_policer_offset_get;
    dnx_data_qos.qos.max_int_map_id_get = dnx_data_qos_qos_max_int_map_id_get;
    dnx_data_qos.qos.max_egress_var_get = dnx_data_qos_qos_max_egress_var_get;
    dnx_data_qos.qos.ecn_bits_width_get = dnx_data_qos_qos_ecn_bits_width_get;
    dnx_data_qos.qos.eei_mpls_ttl_profile_get = dnx_data_qos_qos_eei_mpls_ttl_profile_get;
    dnx_data_qos.qos.routing_dscp_preserve_mode_get = dnx_data_qos_qos_routing_dscp_preserve_mode_get;

    
    
    
    dnx_data_qos.feature.feature_get = dnx_data_qos_feature_feature_get;

    
    dnx_data_qos.feature.phb_rm_ffc_bits_width_get = dnx_data_qos_feature_phb_rm_ffc_bits_width_get;

    
    
    
    
    dnx_data_regression.dvapi_topology.feature_get = dnx_data_regression_dvapi_topology_feature_get;

    
    dnx_data_regression.dvapi_topology.nof_pm_ext_loops_get = dnx_data_regression_dvapi_topology_nof_pm_ext_loops_get;
    dnx_data_regression.dvapi_topology.nof_pm_p2p_links_get = dnx_data_regression_dvapi_topology_nof_pm_p2p_links_get;
    dnx_data_regression.dvapi_topology.nof_pm_force_er_get = dnx_data_regression_dvapi_topology_nof_pm_force_er_get;

    
    dnx_data_regression.dvapi_topology.pm_ext_loops_get = dnx_data_regression_dvapi_topology_pm_ext_loops_get;
    dnx_data_regression.dvapi_topology.pm_ext_loops_info_get = dnx_data_regression_dvapi_topology_pm_ext_loops_info_get;
    dnx_data_regression.dvapi_topology.pm_p2p_links_get = dnx_data_regression_dvapi_topology_pm_p2p_links_get;
    dnx_data_regression.dvapi_topology.pm_p2p_links_info_get = dnx_data_regression_dvapi_topology_pm_p2p_links_info_get;
    dnx_data_regression.dvapi_topology.pm_force_er_get = dnx_data_regression_dvapi_topology_pm_force_er_get;
    dnx_data_regression.dvapi_topology.pm_force_er_info_get = dnx_data_regression_dvapi_topology_pm_force_er_info_get;
    dnx_data_regression.dvapi_topology.pm_test_limitations_get = dnx_data_regression_dvapi_topology_pm_test_limitations_get;
    dnx_data_regression.dvapi_topology.pm_test_limitations_info_get = dnx_data_regression_dvapi_topology_pm_test_limitations_info_get;
    
    
    dnx_data_regression.ser_parameters.feature_get = dnx_data_regression_ser_parameters_feature_get;

    
    dnx_data_regression.ser_parameters.cgm_voq_profiles_get = dnx_data_regression_ser_parameters_cgm_voq_profiles_get;
    dnx_data_regression.ser_parameters.ecgm_qqst_table_get = dnx_data_regression_ser_parameters_ecgm_qqst_table_get;
    dnx_data_regression.ser_parameters.ecgm_qdct_table_get = dnx_data_regression_ser_parameters_ecgm_qdct_table_get;
    dnx_data_regression.ser_parameters.erpp_per_pp_port_table_get = dnx_data_regression_ser_parameters_erpp_per_pp_port_table_get;
    dnx_data_regression.ser_parameters.erpp_per_pp_port_table_2_get = dnx_data_regression_ser_parameters_erpp_per_pp_port_table_2_get;
    dnx_data_regression.ser_parameters.mdb_ivsi_get = dnx_data_regression_ser_parameters_mdb_ivsi_get;
    dnx_data_regression.ser_parameters.mdb_evsi_get = dnx_data_regression_ser_parameters_mdb_evsi_get;
    dnx_data_regression.ser_parameters.mdb_glem_get = dnx_data_regression_ser_parameters_mdb_glem_get;
    dnx_data_regression.ser_parameters.mdb_exem_1_get = dnx_data_regression_ser_parameters_mdb_exem_1_get;
    dnx_data_regression.ser_parameters.mdb_fec_1_get = dnx_data_regression_ser_parameters_mdb_fec_1_get;

    
    dnx_data_regression.ser_parameters.cgm_ipp_map_get = dnx_data_regression_ser_parameters_cgm_ipp_map_get;
    dnx_data_regression.ser_parameters.cgm_ipp_map_info_get = dnx_data_regression_ser_parameters_cgm_ipp_map_info_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_bound_prms_get = dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_bound_prms_info_get = dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_info_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_bound_prms_2_get = dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_bound_prms_2_info_get = dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_info_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_recovery_prms_get = dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_recovery_prms_info_get = dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_info_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_recovery_prms_2_get = dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_get;
    dnx_data_regression.ser_parameters.cgm_voq_dram_recovery_prms_2_info_get = dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_info_get;
    
    
    dnx_data_regression.source_info.feature_get = dnx_data_regression_source_info_feature_get;

    

    
    dnx_data_regression.source_info.tlist_get = dnx_data_regression_source_info_tlist_get;
    dnx_data_regression.source_info.tlist_info_get = dnx_data_regression_source_info_tlist_info_get;
    
    
    dnx_data_regression.access.feature_get = dnx_data_regression_access_feature_get;

    
    dnx_data_regression.access.access_mode_get = dnx_data_regression_access_access_mode_get;
    dnx_data_regression.access.severity_miss_check_get = dnx_data_regression_access_severity_miss_check_get;

    
    
    
    
    dnx_data_sat.general.feature_get = dnx_data_sat_general_feature_get;

    
    dnx_data_sat.general.granularity_get = dnx_data_sat_general_granularity_get;
    dnx_data_sat.general.accum_count_not_collect_during_unavaliable_status_get = dnx_data_sat_general_accum_count_not_collect_during_unavaliable_status_get;

    
    
    
    dnx_data_sat.generator.feature_get = dnx_data_sat_generator_feature_get;

    
    dnx_data_sat.generator.nof_evc_ids_get = dnx_data_sat_generator_nof_evc_ids_get;
    dnx_data_sat.generator.nof_gtf_ids_get = dnx_data_sat_generator_nof_gtf_ids_get;
    dnx_data_sat.generator.gtf_pkt_header_max_length_get = dnx_data_sat_generator_gtf_pkt_header_max_length_get;
    dnx_data_sat.generator.gtf_pkt_length_max_patn_get = dnx_data_sat_generator_gtf_pkt_length_max_patn_get;
    dnx_data_sat.generator.gtf_max_seq_period_get = dnx_data_sat_generator_gtf_max_seq_period_get;
    dnx_data_sat.generator.gtf_stamp_max_inc_step_get = dnx_data_sat_generator_gtf_stamp_max_inc_step_get;
    dnx_data_sat.generator.gtf_stamp_max_inc_period_get = dnx_data_sat_generator_gtf_stamp_max_inc_period_get;
    dnx_data_sat.generator.gtf_bw_max_value_get = dnx_data_sat_generator_gtf_bw_max_value_get;
    dnx_data_sat.generator.gtf_rate_max_get = dnx_data_sat_generator_gtf_rate_max_get;
    dnx_data_sat.generator.gtf_pps_rate_max_get = dnx_data_sat_generator_gtf_pps_rate_max_get;
    dnx_data_sat.generator.gtf_pkt_header_entries_get = dnx_data_sat_generator_gtf_pkt_header_entries_get;
    dnx_data_sat.generator.gtf_pkt_header_length_get = dnx_data_sat_generator_gtf_pkt_header_length_get;
    dnx_data_sat.generator.gtf_pkt_len_patten_size_get = dnx_data_sat_generator_gtf_pkt_len_patten_size_get;
    dnx_data_sat.generator.gtf_min_pkt_len_get = dnx_data_sat_generator_gtf_min_pkt_len_get;
    dnx_data_sat.generator.max_bit_rate_in_64B_get = dnx_data_sat_generator_max_bit_rate_in_64B_get;
    dnx_data_sat.generator.max_bit_rate_in_128B_get = dnx_data_sat_generator_max_bit_rate_in_128B_get;
    dnx_data_sat.generator.max_bit_rate_in_256B_get = dnx_data_sat_generator_max_bit_rate_in_256B_get;
    dnx_data_sat.generator.max_bit_rate_in_512B_get = dnx_data_sat_generator_max_bit_rate_in_512B_get;
    dnx_data_sat.generator.max_bit_rate_in_1024B_get = dnx_data_sat_generator_max_bit_rate_in_1024B_get;
    dnx_data_sat.generator.max_bit_rate_in_1280B_get = dnx_data_sat_generator_max_bit_rate_in_1280B_get;
    dnx_data_sat.generator.max_bit_rate_in_1518B_get = dnx_data_sat_generator_max_bit_rate_in_1518B_get;
    dnx_data_sat.generator.max_bit_rate_in_9000B_get = dnx_data_sat_generator_max_bit_rate_in_9000B_get;
    dnx_data_sat.generator.max_packet_rate_in_64B_get = dnx_data_sat_generator_max_packet_rate_in_64B_get;
    dnx_data_sat.generator.max_packet_rate_in_128B_get = dnx_data_sat_generator_max_packet_rate_in_128B_get;
    dnx_data_sat.generator.max_packet_rate_in_256B_get = dnx_data_sat_generator_max_packet_rate_in_256B_get;
    dnx_data_sat.generator.max_packet_rate_in_512B_get = dnx_data_sat_generator_max_packet_rate_in_512B_get;
    dnx_data_sat.generator.max_packet_rate_in_1024B_get = dnx_data_sat_generator_max_packet_rate_in_1024B_get;
    dnx_data_sat.generator.max_packet_rate_in_1280B_get = dnx_data_sat_generator_max_packet_rate_in_1280B_get;
    dnx_data_sat.generator.max_packet_rate_in_1518B_get = dnx_data_sat_generator_max_packet_rate_in_1518B_get;
    dnx_data_sat.generator.max_packet_rate_in_9000B_get = dnx_data_sat_generator_max_packet_rate_in_9000B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_64B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_128B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_256B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_512B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_1024B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_1280B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_1518B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_get;
    dnx_data_sat.generator.max_ctf_recv_packet_rate_in_9000B_get = dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_get;

    
    
    
    dnx_data_sat.collector.feature_get = dnx_data_sat_collector_feature_get;

    
    dnx_data_sat.collector.nof_ctf_ids_get = dnx_data_sat_collector_nof_ctf_ids_get;
    dnx_data_sat.collector.nof_oam_ids_get = dnx_data_sat_collector_nof_oam_ids_get;
    dnx_data_sat.collector.mef_payload_offset_get = dnx_data_sat_collector_mef_payload_offset_get;
    dnx_data_sat.collector.mef_seq_num_offset_get = dnx_data_sat_collector_mef_seq_num_offset_get;
    dnx_data_sat.collector.mef_time_stamp_offset_get = dnx_data_sat_collector_mef_time_stamp_offset_get;
    dnx_data_sat.collector.mef_crc_byte_offset_get = dnx_data_sat_collector_mef_crc_byte_offset_get;
    dnx_data_sat.collector.y1731_payload_offset_get = dnx_data_sat_collector_y1731_payload_offset_get;
    dnx_data_sat.collector.y1731_seq_num_offset_get = dnx_data_sat_collector_y1731_seq_num_offset_get;
    dnx_data_sat.collector.y1731_time_stamp_offset_get = dnx_data_sat_collector_y1731_time_stamp_offset_get;
    dnx_data_sat.collector.y1731_crc_byte_offset_get = dnx_data_sat_collector_y1731_crc_byte_offset_get;
    dnx_data_sat.collector.ctf_bins_limit_entries_get = dnx_data_sat_collector_ctf_bins_limit_entries_get;
    dnx_data_sat.collector.trap_id_entries_get = dnx_data_sat_collector_trap_id_entries_get;
    dnx_data_sat.collector.trap_data_entries_get = dnx_data_sat_collector_trap_data_entries_get;

    
    
    
    dnx_data_sat.rx_flow_param.feature_get = dnx_data_sat_rx_flow_param_feature_get;

    

    
    
    
    dnx_data_sat.tx_flow_param.feature_get = dnx_data_sat_tx_flow_param_feature_get;

    

    
    
    
    
    dnx_data_sbusdma_desc.global.feature_get = dnx_data_sbusdma_desc_global_feature_get;

    
    dnx_data_sbusdma_desc.global.dma_desc_aggregator_chain_length_max_get = dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_get;
    dnx_data_sbusdma_desc.global.dma_desc_aggregator_buff_size_kb_get = dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_get;
    dnx_data_sbusdma_desc.global.dma_desc_aggregator_timeout_usec_get = dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_get;

    
    dnx_data_sbusdma_desc.global.enable_module_desc_dma_get = dnx_data_sbusdma_desc_global_enable_module_desc_dma_get;
    dnx_data_sbusdma_desc.global.enable_module_desc_dma_info_get = dnx_data_sbusdma_desc_global_enable_module_desc_dma_info_get;
    
    
    
    dnx_data_sch.general.feature_get = dnx_data_sch_general_feature_get;

    
    dnx_data_sch.general.nof_slow_profiles_get = dnx_data_sch_general_nof_slow_profiles_get;
    dnx_data_sch.general.nof_slow_levels_get = dnx_data_sch_general_nof_slow_levels_get;
    dnx_data_sch.general.cycles_per_credit_token_get = dnx_data_sch_general_cycles_per_credit_token_get;
    dnx_data_sch.general.nof_fmq_class_get = dnx_data_sch_general_nof_fmq_class_get;
    dnx_data_sch.general.port_priority_propagation_version_get = dnx_data_sch_general_port_priority_propagation_version_get;
    dnx_data_sch.general.port_priority_propagation_enable_get = dnx_data_sch_general_port_priority_propagation_enable_get;

    
    dnx_data_sch.general.low_rate_factor_to_dbal_enum_get = dnx_data_sch_general_low_rate_factor_to_dbal_enum_get;
    dnx_data_sch.general.low_rate_factor_to_dbal_enum_info_get = dnx_data_sch_general_low_rate_factor_to_dbal_enum_info_get;
    dnx_data_sch.general.dbal_enum_to_low_rate_factor_get = dnx_data_sch_general_dbal_enum_to_low_rate_factor_get;
    dnx_data_sch.general.dbal_enum_to_low_rate_factor_info_get = dnx_data_sch_general_dbal_enum_to_low_rate_factor_info_get;
    dnx_data_sch.general.slow_rate_max_bucket_get = dnx_data_sch_general_slow_rate_max_bucket_get;
    dnx_data_sch.general.slow_rate_max_bucket_info_get = dnx_data_sch_general_slow_rate_max_bucket_info_get;
    
    
    dnx_data_sch.ps.feature_get = dnx_data_sch_ps_feature_get;

    
    dnx_data_sch.ps.min_priority_for_tcg_get = dnx_data_sch_ps_min_priority_for_tcg_get;
    dnx_data_sch.ps.max_nof_tcg_get = dnx_data_sch_ps_max_nof_tcg_get;
    dnx_data_sch.ps.nof_hrs_in_ps_get = dnx_data_sch_ps_nof_hrs_in_ps_get;
    dnx_data_sch.ps.max_tcg_weight_get = dnx_data_sch_ps_max_tcg_weight_get;
    dnx_data_sch.ps.tc_tcg_calendar_access_period_get = dnx_data_sch_ps_tc_tcg_calendar_access_period_get;
    dnx_data_sch.ps.max_port_rate_mbps_get = dnx_data_sch_ps_max_port_rate_mbps_get;
    dnx_data_sch.ps.max_burst_get = dnx_data_sch_ps_max_burst_get;
    dnx_data_sch.ps.assigned_credit_worth_multiplier_get = dnx_data_sch_ps_assigned_credit_worth_multiplier_get;

    
    
    
    dnx_data_sch.flow.feature_get = dnx_data_sch_flow_feature_get;

    
    dnx_data_sch.flow.nof_hr_get = dnx_data_sch_flow_nof_hr_get;
    dnx_data_sch.flow.nof_cl_get = dnx_data_sch_flow_nof_cl_get;
    dnx_data_sch.flow.nof_se_get = dnx_data_sch_flow_nof_se_get;
    dnx_data_sch.flow.nof_flows_get = dnx_data_sch_flow_nof_flows_get;
    dnx_data_sch.flow.region_size_get = dnx_data_sch_flow_region_size_get;
    dnx_data_sch.flow.nof_regions_get = dnx_data_sch_flow_nof_regions_get;
    dnx_data_sch.flow.nof_hr_regions_get = dnx_data_sch_flow_nof_hr_regions_get;
    dnx_data_sch.flow.nof_connectors_only_regions_get = dnx_data_sch_flow_nof_connectors_only_regions_get;
    dnx_data_sch.flow.flow_in_region_mask_get = dnx_data_sch_flow_flow_in_region_mask_get;
    dnx_data_sch.flow.min_connector_bundle_size_get = dnx_data_sch_flow_min_connector_bundle_size_get;
    dnx_data_sch.flow.conn_idx_interdig_flow_get = dnx_data_sch_flow_conn_idx_interdig_flow_get;
    dnx_data_sch.flow.shaper_flipflops_nof_get = dnx_data_sch_flow_shaper_flipflops_nof_get;
    dnx_data_sch.flow.nof_flows_in_quartet_order_group_get = dnx_data_sch_flow_nof_flows_in_quartet_order_group_get;
    dnx_data_sch.flow.nof_regions_in_quartet_order_group_get = dnx_data_sch_flow_nof_regions_in_quartet_order_group_get;
    dnx_data_sch.flow.nof_quartet_order_groups_get = dnx_data_sch_flow_nof_quartet_order_groups_get;
    dnx_data_sch.flow.nof_low_rate_ranges_get = dnx_data_sch_flow_nof_low_rate_ranges_get;
    dnx_data_sch.flow.nof_low_rate_regions_get = dnx_data_sch_flow_nof_low_rate_regions_get;
    dnx_data_sch.flow.low_rate_pattern_mask_get = dnx_data_sch_flow_low_rate_pattern_mask_get;
    dnx_data_sch.flow.low_rate_flow_resolution_get = dnx_data_sch_flow_low_rate_flow_resolution_get;
    dnx_data_sch.flow.erp_hr_get = dnx_data_sch_flow_erp_hr_get;
    dnx_data_sch.flow.reserved_hr_get = dnx_data_sch_flow_reserved_hr_get;
    dnx_data_sch.flow.max_se_get = dnx_data_sch_flow_max_se_get;
    dnx_data_sch.flow.max_flow_get = dnx_data_sch_flow_max_flow_get;
    dnx_data_sch.flow.first_hr_region_get = dnx_data_sch_flow_first_hr_region_get;
    dnx_data_sch.flow.first_se_flow_id_get = dnx_data_sch_flow_first_se_flow_id_get;
    dnx_data_sch.flow.hr_se_id_min_get = dnx_data_sch_flow_hr_se_id_min_get;
    dnx_data_sch.flow.hr_se_id_max_get = dnx_data_sch_flow_hr_se_id_max_get;
    dnx_data_sch.flow.virtual_flow_mapped_queue_get = dnx_data_sch_flow_virtual_flow_mapped_queue_get;
    dnx_data_sch.flow.default_credit_source_se_id_get = dnx_data_sch_flow_default_credit_source_se_id_get;
    dnx_data_sch.flow.runtime_performance_optimize_enable_sched_allocation_get = dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_get;

    
    dnx_data_sch.flow.region_type_get = dnx_data_sch_flow_region_type_get;
    dnx_data_sch.flow.region_type_info_get = dnx_data_sch_flow_region_type_info_get;
    dnx_data_sch.flow.nof_remote_cores_get = dnx_data_sch_flow_nof_remote_cores_get;
    dnx_data_sch.flow.nof_remote_cores_info_get = dnx_data_sch_flow_nof_remote_cores_info_get;
    dnx_data_sch.flow.quartet_type_get = dnx_data_sch_flow_quartet_type_get;
    dnx_data_sch.flow.quartet_type_info_get = dnx_data_sch_flow_quartet_type_info_get;
    dnx_data_sch.flow.quartet_index_conversion_get = dnx_data_sch_flow_quartet_index_conversion_get;
    dnx_data_sch.flow.quartet_index_conversion_info_get = dnx_data_sch_flow_quartet_index_conversion_info_get;
    
    
    dnx_data_sch.se.feature_get = dnx_data_sch_se_feature_get;

    
    dnx_data_sch.se.default_cos_get = dnx_data_sch_se_default_cos_get;
    dnx_data_sch.se.nof_color_group_get = dnx_data_sch_se_nof_color_group_get;
    dnx_data_sch.se.max_hr_weight_get = dnx_data_sch_se_max_hr_weight_get;
    dnx_data_sch.se.cl_class_profile_nof_get = dnx_data_sch_se_cl_class_profile_nof_get;

    
    dnx_data_sch.se.quartet_offset_to_cl_num_get = dnx_data_sch_se_quartet_offset_to_cl_num_get;
    dnx_data_sch.se.quartet_offset_to_cl_num_info_get = dnx_data_sch_se_quartet_offset_to_cl_num_info_get;
    
    
    dnx_data_sch.interface.feature_get = dnx_data_sch_interface_feature_get;

    
    dnx_data_sch.interface.nof_big_calendars_get = dnx_data_sch_interface_nof_big_calendars_get;
    dnx_data_sch.interface.nof_channelized_calendars_get = dnx_data_sch_interface_nof_channelized_calendars_get;
    dnx_data_sch.interface.max_calendar_size_get = dnx_data_sch_interface_max_calendar_size_get;
    dnx_data_sch.interface.nof_sch_interfaces_get = dnx_data_sch_interface_nof_sch_interfaces_get;
    dnx_data_sch.interface.cal_speed_resolution_get = dnx_data_sch_interface_cal_speed_resolution_get;
    dnx_data_sch.interface.reserved_get = dnx_data_sch_interface_reserved_get;
    dnx_data_sch.interface.max_if_rate_mbps_get = dnx_data_sch_interface_max_if_rate_mbps_get;

    
    dnx_data_sch.interface.shaper_resolution_get = dnx_data_sch_interface_shaper_resolution_get;
    dnx_data_sch.interface.shaper_resolution_info_get = dnx_data_sch_interface_shaper_resolution_info_get;
    
    
    dnx_data_sch.device.feature_get = dnx_data_sch_device_feature_get;

    
    dnx_data_sch.device.drm_resolution_get = dnx_data_sch_device_drm_resolution_get;

    
    
    
    dnx_data_sch.sch_alloc.feature_get = dnx_data_sch_sch_alloc_feature_get;

    
    dnx_data_sch.sch_alloc.tag_aggregate_se_2_get = dnx_data_sch_sch_alloc_tag_aggregate_se_2_get;
    dnx_data_sch.sch_alloc.tag_aggregate_se_4_get = dnx_data_sch_sch_alloc_tag_aggregate_se_4_get;
    dnx_data_sch.sch_alloc.tag_aggregate_se_8_get = dnx_data_sch_sch_alloc_tag_aggregate_se_8_get;
    dnx_data_sch.sch_alloc.shared_shaper_max_tag_value_get = dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_get;
    dnx_data_sch.sch_alloc.tag_size_aggregate_se_get = dnx_data_sch_sch_alloc_tag_size_aggregate_se_get;
    dnx_data_sch.sch_alloc.tag_size_con_get = dnx_data_sch_sch_alloc_tag_size_con_get;
    dnx_data_sch.sch_alloc.type_con_reg_start_get = dnx_data_sch_sch_alloc_type_con_reg_start_get;
    dnx_data_sch.sch_alloc.alloc_invalid_flow_get = dnx_data_sch_sch_alloc_alloc_invalid_flow_get;
    dnx_data_sch.sch_alloc.dealloc_flow_ids_get = dnx_data_sch_sch_alloc_dealloc_flow_ids_get;
    dnx_data_sch.sch_alloc.type_con_reg_end_get = dnx_data_sch_sch_alloc_type_con_reg_end_get;
    dnx_data_sch.sch_alloc.type_hr_reg_start_get = dnx_data_sch_sch_alloc_type_hr_reg_start_get;

    
    dnx_data_sch.sch_alloc.region_get = dnx_data_sch_sch_alloc_region_get;
    dnx_data_sch.sch_alloc.region_info_get = dnx_data_sch_sch_alloc_region_info_get;
    dnx_data_sch.sch_alloc.connector_get = dnx_data_sch_sch_alloc_connector_get;
    dnx_data_sch.sch_alloc.connector_info_get = dnx_data_sch_sch_alloc_connector_info_get;
    dnx_data_sch.sch_alloc.se_per_region_type_get = dnx_data_sch_sch_alloc_se_per_region_type_get;
    dnx_data_sch.sch_alloc.se_per_region_type_info_get = dnx_data_sch_sch_alloc_se_per_region_type_info_get;
    dnx_data_sch.sch_alloc.se_get = dnx_data_sch_sch_alloc_se_get;
    dnx_data_sch.sch_alloc.se_info_get = dnx_data_sch_sch_alloc_se_info_get;
    dnx_data_sch.sch_alloc.composite_se_per_region_type_get = dnx_data_sch_sch_alloc_composite_se_per_region_type_get;
    dnx_data_sch.sch_alloc.composite_se_per_region_type_info_get = dnx_data_sch_sch_alloc_composite_se_per_region_type_info_get;
    dnx_data_sch.sch_alloc.composite_se_get = dnx_data_sch_sch_alloc_composite_se_get;
    dnx_data_sch.sch_alloc.composite_se_info_get = dnx_data_sch_sch_alloc_composite_se_info_get;
    
    
    dnx_data_sch.dbal.feature_get = dnx_data_sch_dbal_feature_get;

    
    dnx_data_sch.dbal.scheduler_enable_grouping_factor_get = dnx_data_sch_dbal_scheduler_enable_grouping_factor_get;
    dnx_data_sch.dbal.scheduler_enable_dbal_mult_factor_get = dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_get;
    dnx_data_sch.dbal.flow_bits_get = dnx_data_sch_dbal_flow_bits_get;
    dnx_data_sch.dbal.se_bits_get = dnx_data_sch_dbal_se_bits_get;
    dnx_data_sch.dbal.cl_bits_get = dnx_data_sch_dbal_cl_bits_get;
    dnx_data_sch.dbal.hr_bits_get = dnx_data_sch_dbal_hr_bits_get;
    dnx_data_sch.dbal.interface_bits_get = dnx_data_sch_dbal_interface_bits_get;
    dnx_data_sch.dbal.calendar_bits_get = dnx_data_sch_dbal_calendar_bits_get;
    dnx_data_sch.dbal.flow_shaper_mant_bits_get = dnx_data_sch_dbal_flow_shaper_mant_bits_get;
    dnx_data_sch.dbal.flow_shaper_max_burst_bits_get = dnx_data_sch_dbal_flow_shaper_max_burst_bits_get;
    dnx_data_sch.dbal.ps_shaper_quanta_bits_get = dnx_data_sch_dbal_ps_shaper_quanta_bits_get;
    dnx_data_sch.dbal.ps_shaper_max_burst_bits_get = dnx_data_sch_dbal_ps_shaper_max_burst_bits_get;
    dnx_data_sch.dbal.drm_nof_links_max_get = dnx_data_sch_dbal_drm_nof_links_max_get;
    dnx_data_sch.dbal.token_count_default_get = dnx_data_sch_dbal_token_count_default_get;
    dnx_data_sch.dbal.priority_propagation_if_group_max_get = dnx_data_sch_dbal_priority_propagation_if_group_max_get;
    dnx_data_sch.dbal.max_sch_interfaces_with_fc_get = dnx_data_sch_dbal_max_sch_interfaces_with_fc_get;
    dnx_data_sch.dbal.scheduler_enable_key_size_get = dnx_data_sch_dbal_scheduler_enable_key_size_get;
    dnx_data_sch.dbal.flow_id_pair_key_size_get = dnx_data_sch_dbal_flow_id_pair_key_size_get;
    dnx_data_sch.dbal.flow_id_pair_max_get = dnx_data_sch_dbal_flow_id_pair_max_get;
    dnx_data_sch.dbal.ps_bits_get = dnx_data_sch_dbal_ps_bits_get;
    dnx_data_sch.dbal.flow_shaper_descr_bits_get = dnx_data_sch_dbal_flow_shaper_descr_bits_get;
    dnx_data_sch.dbal.ps_shaper_bits_get = dnx_data_sch_dbal_ps_shaper_bits_get;
    dnx_data_sch.dbal.cl_profile_bits_get = dnx_data_sch_dbal_cl_profile_bits_get;
    dnx_data_sch.dbal.priority_propagation_if_group_nof_bits_get = dnx_data_sch_dbal_priority_propagation_if_group_nof_bits_get;

    
    
    
    dnx_data_sch.features.feature_get = dnx_data_sch_features_feature_get;

    

    
    
    
    dnx_data_sch.fc.feature_get = dnx_data_sch_fc_feature_get;

    

    
    
    
    
    dnx_data_snif.general.feature_get = dnx_data_snif_general_feature_get;

    
    dnx_data_snif.general.cud_type_standard_get = dnx_data_snif_general_cud_type_standard_get;
    dnx_data_snif.general.cud_type_sflow_get = dnx_data_snif_general_cud_type_sflow_get;
    dnx_data_snif.general.cud_type_mirror_on_drop_get = dnx_data_snif_general_cud_type_mirror_on_drop_get;

    
    
    
    dnx_data_snif.ingress.feature_get = dnx_data_snif_ingress_feature_get;

    
    dnx_data_snif.ingress.nof_profiles_get = dnx_data_snif_ingress_nof_profiles_get;
    dnx_data_snif.ingress.mirror_nof_profiles_get = dnx_data_snif_ingress_mirror_nof_profiles_get;
    dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get = dnx_data_snif_ingress_nof_mirror_on_drop_profiles_get;
    dnx_data_snif.ingress.mirror_nof_vlan_profiles_get = dnx_data_snif_ingress_mirror_nof_vlan_profiles_get;
    dnx_data_snif.ingress.mirror_default_tagged_profile_get = dnx_data_snif_ingress_mirror_default_tagged_profile_get;
    dnx_data_snif.ingress.mirror_untagged_profiles_get = dnx_data_snif_ingress_mirror_untagged_profiles_get;
    dnx_data_snif.ingress.prob_max_val_get = dnx_data_snif_ingress_prob_max_val_get;
    dnx_data_snif.ingress.crop_size_get = dnx_data_snif_ingress_crop_size_get;
    dnx_data_snif.ingress.keep_orig_dest_on_dsp_ext_get = dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_get;

    
    
    
    dnx_data_snif.egress.feature_get = dnx_data_snif_egress_feature_get;

    
    dnx_data_snif.egress.mirror_nof_profiles_get = dnx_data_snif_egress_mirror_nof_profiles_get;
    dnx_data_snif.egress.snoop_nof_profiles_get = dnx_data_snif_egress_snoop_nof_profiles_get;
    dnx_data_snif.egress.mirror_nof_vlan_profiles_get = dnx_data_snif_egress_mirror_nof_vlan_profiles_get;
    dnx_data_snif.egress.mirror_default_profile_get = dnx_data_snif_egress_mirror_default_profile_get;
    dnx_data_snif.egress.prob_max_val_get = dnx_data_snif_egress_prob_max_val_get;
    dnx_data_snif.egress.prob_nof_bits_get = dnx_data_snif_egress_prob_nof_bits_get;
    dnx_data_snif.egress.probability_nof_profiles_get = dnx_data_snif_egress_probability_nof_profiles_get;
    dnx_data_snif.egress.crop_size_nof_profiles_get = dnx_data_snif_egress_crop_size_nof_profiles_get;
    dnx_data_snif.egress.crop_size_granularity_get = dnx_data_snif_egress_crop_size_granularity_get;

    
    dnx_data_snif.egress.rcy_mirror_to_forward_port_map_get = dnx_data_snif_egress_rcy_mirror_to_forward_port_map_get;
    dnx_data_snif.egress.rcy_mirror_to_forward_port_map_info_get = dnx_data_snif_egress_rcy_mirror_to_forward_port_map_info_get;
    
    
    
    dnx_data_spb.ocb.feature_get = dnx_data_spb_ocb_feature_get;

    
    dnx_data_spb.ocb.nof_blocks_get = dnx_data_spb_ocb_nof_blocks_get;
    dnx_data_spb.ocb.nof_rings_get = dnx_data_spb_ocb_nof_rings_get;
    dnx_data_spb.ocb.nof_banks_per_ring_get = dnx_data_spb_ocb_nof_banks_per_ring_get;
    dnx_data_spb.ocb.nof_ptrs_per_bank_get = dnx_data_spb_ocb_nof_ptrs_per_bank_get;
    dnx_data_spb.ocb.otp_defective_bank_nof_bits_get = dnx_data_spb_ocb_otp_defective_bank_nof_bits_get;
    dnx_data_spb.ocb.otp_is_any_bank_defective_nof_bits_get = dnx_data_spb_ocb_otp_is_any_bank_defective_nof_bits_get;
    dnx_data_spb.ocb.nof_logical_banks_get = dnx_data_spb_ocb_nof_logical_banks_get;
    dnx_data_spb.ocb.bank_full_th_for_cfg_get = dnx_data_spb_ocb_bank_full_th_for_cfg_get;
    dnx_data_spb.ocb.buffer_ptr_nof_bits_get = dnx_data_spb_ocb_buffer_ptr_nof_bits_get;
    dnx_data_spb.ocb.sram_in_power_down_get = dnx_data_spb_ocb_sram_in_power_down_get;
    dnx_data_spb.ocb.bank_full_threshold_bits_get = dnx_data_spb_ocb_bank_full_threshold_bits_get;
    dnx_data_spb.ocb.bank_full_threshold_get = dnx_data_spb_ocb_bank_full_threshold_get;

    
    dnx_data_spb.ocb.last_in_chain_get = dnx_data_spb_ocb_last_in_chain_get;
    dnx_data_spb.ocb.last_in_chain_info_get = dnx_data_spb_ocb_last_in_chain_info_get;
    
    
    
    dnx_data_srv6.general.feature_get = dnx_data_srv6_general_feature_get;

    

    
    
    
    dnx_data_srv6.termination.feature_get = dnx_data_srv6_termination_feature_get;

    
    dnx_data_srv6.termination.max_nof_terminated_sids_psp_extended_get = dnx_data_srv6_termination_max_nof_terminated_sids_psp_extended_get;
    dnx_data_srv6.termination.max_nof_terminated_sids_psp_get = dnx_data_srv6_termination_max_nof_terminated_sids_psp_get;
    dnx_data_srv6.termination.max_nof_terminated_sids_usp_get = dnx_data_srv6_termination_max_nof_terminated_sids_usp_get;
    dnx_data_srv6.termination.max_nof_terminated_sids_usp_1pass_get = dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get;
    dnx_data_srv6.termination.max_nof_terminated_sids_usd_1pass_get = dnx_data_srv6_termination_max_nof_terminated_sids_usd_1pass_get;
    dnx_data_srv6.termination.max_nof_locators_get = dnx_data_srv6_termination_max_nof_locators_get;

    
    
    
    dnx_data_srv6.encapsulation.feature_get = dnx_data_srv6_encapsulation_feature_get;

    
    dnx_data_srv6.encapsulation.max_nof_encap_sids_main_pass_get = dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_get;
    dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get = dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_get;

    
    
    
    dnx_data_srv6.endpoint.feature_get = dnx_data_srv6_endpoint_feature_get;

    

    
    
    
    
    dnx_data_stack.general.feature_get = dnx_data_stack_general_feature_get;

    
    dnx_data_stack.general.nof_tm_domains_max_get = dnx_data_stack_general_nof_tm_domains_max_get;
    dnx_data_stack.general.dest_port_base_queue_min_get = dnx_data_stack_general_dest_port_base_queue_min_get;
    dnx_data_stack.general.dest_port_base_queue_max_get = dnx_data_stack_general_dest_port_base_queue_max_get;
    dnx_data_stack.general.lag_stack_trunk_resolve_entry_min_get = dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_get;
    dnx_data_stack.general.lag_stack_trunk_resolve_entry_max_get = dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_get;
    dnx_data_stack.general.lag_stack_trunk_resolve_entry_all_get = dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_get;
    dnx_data_stack.general.lag_stack_fec_resolve_entry_min_get = dnx_data_stack_general_lag_stack_fec_resolve_entry_min_get;
    dnx_data_stack.general.lag_stack_fec_resolve_entry_max_get = dnx_data_stack_general_lag_stack_fec_resolve_entry_max_get;
    dnx_data_stack.general.lag_stack_fec_resolve_entry_all_get = dnx_data_stack_general_lag_stack_fec_resolve_entry_all_get;
    dnx_data_stack.general.lag_domain_min_get = dnx_data_stack_general_lag_domain_min_get;
    dnx_data_stack.general.lag_domain_max_get = dnx_data_stack_general_lag_domain_max_get;
    dnx_data_stack.general.trunk_entry_per_tmd_bit_num_get = dnx_data_stack_general_trunk_entry_per_tmd_bit_num_get;
    dnx_data_stack.general.stacking_enable_get = dnx_data_stack_general_stacking_enable_get;

    
    
    
    
    dnx_data_stat.stat_pp.feature_get = dnx_data_stat_stat_pp_feature_get;

    
    dnx_data_stat.stat_pp.max_irpp_profile_value_get = dnx_data_stat_stat_pp_max_irpp_profile_value_get;
    dnx_data_stat.stat_pp.max_etpp_profile_value_get = dnx_data_stat_stat_pp_max_etpp_profile_value_get;
    dnx_data_stat.stat_pp.max_etpp_counting_profile_value_get = dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get;
    dnx_data_stat.stat_pp.max_etpp_metering_profile_value_get = dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get;
    dnx_data_stat.stat_pp.max_erpp_profile_value_get = dnx_data_stat_stat_pp_max_erpp_profile_value_get;
    dnx_data_stat.stat_pp.etpp_reversed_stat_cmd_get = dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get;
    dnx_data_stat.stat_pp.etpp_enc_stat_cmd_is_reversed_get = dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get;
    dnx_data_stat.stat_pp.etpp_egress_vsi_always_pushed_get = dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get;
    dnx_data_stat.stat_pp.irpp_start_profile_value_get = dnx_data_stat_stat_pp_irpp_start_profile_value_get;
    dnx_data_stat.stat_pp.erpp_start_profile_value_get = dnx_data_stat_stat_pp_erpp_start_profile_value_get;
    dnx_data_stat.stat_pp.etpp_start_profile_value_get = dnx_data_stat_stat_pp_etpp_start_profile_value_get;
    dnx_data_stat.stat_pp.etpp_counting_start_profile_value_get = dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get;
    dnx_data_stat.stat_pp.etpp_metering_start_profile_value_get = dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get;
    dnx_data_stat.stat_pp.supported_egress_interface_counter_enum_get = dnx_data_stat_stat_pp_supported_egress_interface_counter_enum_get;

    
    
    
    dnx_data_stat.diag_counter.feature_get = dnx_data_stat_diag_counter_feature_get;

    

    
    dnx_data_stat.diag_counter.ovf_info_get = dnx_data_stat_diag_counter_ovf_info_get;
    dnx_data_stat.diag_counter.ovf_info_info_get = dnx_data_stat_diag_counter_ovf_info_info_get;
    
    
    dnx_data_stat.drop_reasons.feature_get = dnx_data_stat_drop_reasons_feature_get;

    
    dnx_data_stat.drop_reasons.nof_drop_reasons_get = dnx_data_stat_drop_reasons_nof_drop_reasons_get;

    
    dnx_data_stat.drop_reasons.group_drop_reason_index_get = dnx_data_stat_drop_reasons_group_drop_reason_index_get;
    dnx_data_stat.drop_reasons.group_drop_reason_index_info_get = dnx_data_stat_drop_reasons_group_drop_reason_index_info_get;
    dnx_data_stat.drop_reasons.drop_reason_index_get = dnx_data_stat_drop_reasons_drop_reason_index_get;
    dnx_data_stat.drop_reasons.drop_reason_index_info_get = dnx_data_stat_drop_reasons_drop_reason_index_info_get;
    dnx_data_stat.drop_reasons.drop_reasons_groups_get = dnx_data_stat_drop_reasons_drop_reasons_groups_get;
    dnx_data_stat.drop_reasons.drop_reasons_groups_info_get = dnx_data_stat_drop_reasons_drop_reasons_groups_info_get;
    
    
    dnx_data_stat.feature.feature_get = dnx_data_stat_feature_feature_get;

    

    
    
    
    
    dnx_data_stg.stg.feature_get = dnx_data_stg_stg_feature_get;

    
    dnx_data_stg.stg.nof_topology_ids_get = dnx_data_stg_stg_nof_topology_ids_get;

    
    
    
    
    dnx_data_stif.general.feature_get = dnx_data_stif_general_feature_get;

    

    
    
    
    dnx_data_stif.config.feature_get = dnx_data_stif_config_feature_get;

    
    dnx_data_stif.config.sram_buffers_resolution_get = dnx_data_stif_config_sram_buffers_resolution_get;
    dnx_data_stif.config.sram_pdbs_resolution_get = dnx_data_stif_config_sram_pdbs_resolution_get;
    dnx_data_stif.config.dram_bdb_resolution_get = dnx_data_stif_config_dram_bdb_resolution_get;
    dnx_data_stif.config.stif_packet_size_get = dnx_data_stif_config_stif_packet_size_get;
    dnx_data_stif.config.stif_scrubber_queue_min_get = dnx_data_stif_config_stif_scrubber_queue_min_get;
    dnx_data_stif.config.stif_scrubber_queue_max_get = dnx_data_stif_config_stif_scrubber_queue_max_get;
    dnx_data_stif.config.stif_scrubber_rate_min_get = dnx_data_stif_config_stif_scrubber_rate_min_get;
    dnx_data_stif.config.stif_scrubber_rate_max_get = dnx_data_stif_config_stif_scrubber_rate_max_get;
    dnx_data_stif.config.stif_report_mode_get = dnx_data_stif_config_stif_report_mode_get;
    dnx_data_stif.config.stif_report_size_ingress_get = dnx_data_stif_config_stif_report_size_ingress_get;
    dnx_data_stif.config.stif_report_size_egress_get = dnx_data_stif_config_stif_report_size_egress_get;
    dnx_data_stif.config.stif_idle_report_get = dnx_data_stif_config_stif_idle_report_get;
    dnx_data_stif.config.nof_valid_sources_get = dnx_data_stif_config_nof_valid_sources_get;

    
    dnx_data_stif.config.scrubber_buffers_get = dnx_data_stif_config_scrubber_buffers_get;
    dnx_data_stif.config.scrubber_buffers_info_get = dnx_data_stif_config_scrubber_buffers_info_get;
    dnx_data_stif.config.selective_report_range_get = dnx_data_stif_config_selective_report_range_get;
    dnx_data_stif.config.selective_report_range_info_get = dnx_data_stif_config_selective_report_range_info_get;
    
    
    dnx_data_stif.port.feature_get = dnx_data_stif_port_feature_get;

    
    dnx_data_stif.port.max_nof_instances_get = dnx_data_stif_port_max_nof_instances_get;
    dnx_data_stif.port.stif_ethu_select_size_get = dnx_data_stif_port_stif_ethu_select_size_get;
    dnx_data_stif.port.stif_lane_mapping_size_get = dnx_data_stif_port_stif_lane_mapping_size_get;
    dnx_data_stif.port.ethu_id_get = dnx_data_stif_port_ethu_id_get;

    
    dnx_data_stif.port.ethu_id_stif_valid_get = dnx_data_stif_port_ethu_id_stif_valid_get;
    dnx_data_stif.port.ethu_id_stif_valid_info_get = dnx_data_stif_port_ethu_id_stif_valid_info_get;
    
    
    dnx_data_stif.report.feature_get = dnx_data_stif_report_feature_get;

    
    dnx_data_stif.report.qsize_idle_report_size_get = dnx_data_stif_report_qsize_idle_report_size_get;
    dnx_data_stif.report.qsize_idle_report_period_get = dnx_data_stif_report_qsize_idle_report_period_get;
    dnx_data_stif.report.qsize_idle_report_period_invalid_get = dnx_data_stif_report_qsize_idle_report_period_invalid_get;
    dnx_data_stif.report.qsize_scrubber_report_cycles_unit_get = dnx_data_stif_report_qsize_scrubber_report_cycles_unit_get;
    dnx_data_stif.report.max_thresholds_per_buffer_get = dnx_data_stif_report_max_thresholds_per_buffer_get;
    dnx_data_stif.report.incoming_tc_get = dnx_data_stif_report_incoming_tc_get;
    dnx_data_stif.report.record_two_msb_get = dnx_data_stif_report_record_two_msb_get;
    dnx_data_stif.report.nof_source_types_get = dnx_data_stif_report_nof_source_types_get;
    dnx_data_stif.report.stat_id_max_count_get = dnx_data_stif_report_stat_id_max_count_get;
    dnx_data_stif.report.stat_object_size_get = dnx_data_stif_report_stat_object_size_get;
    dnx_data_stif.report.billing_format_element_packet_size_get = dnx_data_stif_report_billing_format_element_packet_size_get;
    dnx_data_stif.report.billing_format_msb_opcode_size_get = dnx_data_stif_report_billing_format_msb_opcode_size_get;
    dnx_data_stif.report.billing_format_etpp_metadata_size_get = dnx_data_stif_report_billing_format_etpp_metadata_size_get;
    dnx_data_stif.report.billing_opsize_use_one_type_mask_get = dnx_data_stif_report_billing_opsize_use_one_type_mask_get;
    dnx_data_stif.report.billing_opsize_use_two_types_mask_get = dnx_data_stif_report_billing_opsize_use_two_types_mask_get;
    dnx_data_stif.report.billing_opsize_use_three_types_mask_get = dnx_data_stif_report_billing_opsize_use_three_types_mask_get;
    dnx_data_stif.report.billing_opsize_use_four_types_mask_get = dnx_data_stif_report_billing_opsize_use_four_types_mask_get;
    dnx_data_stif.report.billing_enq_ing_size_shift_get = dnx_data_stif_report_billing_enq_ing_size_shift_get;

    
    dnx_data_stif.report.etpp_metadata_get = dnx_data_stif_report_etpp_metadata_get;
    dnx_data_stif.report.etpp_metadata_info_get = dnx_data_stif_report_etpp_metadata_info_get;
    dnx_data_stif.report.report_size_get = dnx_data_stif_report_report_size_get;
    dnx_data_stif.report.report_size_info_get = dnx_data_stif_report_report_size_info_get;
    
    
    dnx_data_stif.flow_control.feature_get = dnx_data_stif_flow_control_feature_get;

    
    dnx_data_stif.flow_control.etpp_pipe_length_get = dnx_data_stif_flow_control_etpp_pipe_length_get;
    dnx_data_stif.flow_control.etpp_deq_size_of_fifo_get = dnx_data_stif_flow_control_etpp_deq_size_of_fifo_get;
    dnx_data_stif.flow_control.cgm_pipe_length_get = dnx_data_stif_flow_control_cgm_pipe_length_get;
    dnx_data_stif.flow_control.cgm_size_of_fifo_get = dnx_data_stif_flow_control_cgm_size_of_fifo_get;
    dnx_data_stif.flow_control.billing_level_sampling_get = dnx_data_stif_flow_control_billing_level_sampling_get;
    dnx_data_stif.flow_control.fc_disable_value_get = dnx_data_stif_flow_control_fc_disable_value_get;
    dnx_data_stif.flow_control.billing_etpp_flow_control_high_get = dnx_data_stif_flow_control_billing_etpp_flow_control_high_get;
    dnx_data_stif.flow_control.billing_etpp_flow_control_low_get = dnx_data_stif_flow_control_billing_etpp_flow_control_low_get;
    dnx_data_stif.flow_control.billing_cgm_flow_control_high_get = dnx_data_stif_flow_control_billing_cgm_flow_control_high_get;
    dnx_data_stif.flow_control.billing_cgm_flow_control_low_get = dnx_data_stif_flow_control_billing_cgm_flow_control_low_get;
    dnx_data_stif.flow_control.qsize_cgm_flow_control_high_get = dnx_data_stif_flow_control_qsize_cgm_flow_control_high_get;
    dnx_data_stif.flow_control.qsize_cgm_flow_control_low_get = dnx_data_stif_flow_control_qsize_cgm_flow_control_low_get;

    
    
    
    dnx_data_stif.regression.feature_get = dnx_data_stif_regression_feature_get;

    
    dnx_data_stif.regression.last_stif_data_length_get = dnx_data_stif_regression_last_stif_data_length_get;

    
    dnx_data_stif.regression.supported_modes_get = dnx_data_stif_regression_supported_modes_get;
    dnx_data_stif.regression.supported_modes_info_get = dnx_data_stif_regression_supported_modes_info_get;
    
    
    
    dnx_data_switch.load_balancing.feature_get = dnx_data_switch_load_balancing_feature_get;

    
    dnx_data_switch.load_balancing.nof_lb_clients_get = dnx_data_switch_load_balancing_nof_lb_clients_get;
    dnx_data_switch.load_balancing.nof_lb_crc_sel_tcam_entries_get = dnx_data_switch_load_balancing_nof_lb_crc_sel_tcam_entries_get;
    dnx_data_switch.load_balancing.nof_crc_functions_get = dnx_data_switch_load_balancing_nof_crc_functions_get;
    dnx_data_switch.load_balancing.initial_reserved_label_get = dnx_data_switch_load_balancing_initial_reserved_label_get;
    dnx_data_switch.load_balancing.initial_reserved_label_force_get = dnx_data_switch_load_balancing_initial_reserved_label_force_get;
    dnx_data_switch.load_balancing.initial_reserved_next_label_get = dnx_data_switch_load_balancing_initial_reserved_next_label_get;
    dnx_data_switch.load_balancing.initial_reserved_next_label_valid_get = dnx_data_switch_load_balancing_initial_reserved_next_label_valid_get;
    dnx_data_switch.load_balancing.reserved_next_label_valid_get = dnx_data_switch_load_balancing_reserved_next_label_valid_get;
    dnx_data_switch.load_balancing.num_valid_mpls_protocols_get = dnx_data_switch_load_balancing_num_valid_mpls_protocols_get;
    dnx_data_switch.load_balancing.nof_layer_records_from_parser_get = dnx_data_switch_load_balancing_nof_layer_records_from_parser_get;
    dnx_data_switch.load_balancing.nof_seeds_per_crc_function_get = dnx_data_switch_load_balancing_nof_seeds_per_crc_function_get;
    dnx_data_switch.load_balancing.hashing_selection_per_layer_get = dnx_data_switch_load_balancing_hashing_selection_per_layer_get;
    dnx_data_switch.load_balancing.mpls_split_stack_config_get = dnx_data_switch_load_balancing_mpls_split_stack_config_get;
    dnx_data_switch.load_balancing.mpls_cam_next_label_valid_field_exists_get = dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_get;
    dnx_data_switch.load_balancing.padded_layers_crc_buffer_get = dnx_data_switch_load_balancing_padded_layers_crc_buffer_get;
    dnx_data_switch.load_balancing.wide_hash_buffer_get = dnx_data_switch_load_balancing_wide_hash_buffer_get;
    dnx_data_switch.load_balancing.nof_lb_mpls_stack_get = dnx_data_switch_load_balancing_nof_lb_mpls_stack_get;
    dnx_data_switch.load_balancing.nof_bits_mpls_protocol_identifier_get = dnx_data_switch_load_balancing_nof_bits_mpls_protocol_identifier_get;
    dnx_data_switch.load_balancing.seed_configuration_per_pp_port_get = dnx_data_switch_load_balancing_seed_configuration_per_pp_port_get;
    dnx_data_switch.load_balancing.swig_base_offline_simulation_get = dnx_data_switch_load_balancing_swig_base_offline_simulation_get;
    dnx_data_switch.load_balancing.pmf_simulation_support_get = dnx_data_switch_load_balancing_pmf_simulation_support_get;

    
    dnx_data_switch.load_balancing.lb_clients_get = dnx_data_switch_load_balancing_lb_clients_get;
    dnx_data_switch.load_balancing.lb_clients_info_get = dnx_data_switch_load_balancing_lb_clients_info_get;
    dnx_data_switch.load_balancing.lb_client_crc_get = dnx_data_switch_load_balancing_lb_client_crc_get;
    dnx_data_switch.load_balancing.lb_client_crc_info_get = dnx_data_switch_load_balancing_lb_client_crc_info_get;
    dnx_data_switch.load_balancing.lb_field_enablers_get = dnx_data_switch_load_balancing_lb_field_enablers_get;
    dnx_data_switch.load_balancing.lb_field_enablers_info_get = dnx_data_switch_load_balancing_lb_field_enablers_info_get;
    
    
    dnx_data_switch.load_balancing_sim.feature_get = dnx_data_switch_load_balancing_sim_feature_get;

    
    dnx_data_switch.load_balancing_sim.device_enum_get = dnx_data_switch_load_balancing_sim_device_enum_get;

    
    
    
    dnx_data_switch.svtag.feature_get = dnx_data_switch_svtag_feature_get;

    
    dnx_data_switch.svtag.supported_get = dnx_data_switch_svtag_supported_get;
    dnx_data_switch.svtag.svtag_label_size_bits_get = dnx_data_switch_svtag_svtag_label_size_bits_get;
    dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get = dnx_data_switch_svtag_egress_svtag_offset_addr_size_bits_get;
    dnx_data_switch.svtag.egress_svtag_offset_addr_position_bits_get = dnx_data_switch_svtag_egress_svtag_offset_addr_position_bits_get;
    dnx_data_switch.svtag.egress_svtag_signature_size_bits_get = dnx_data_switch_svtag_egress_svtag_signature_size_bits_get;
    dnx_data_switch.svtag.egress_svtag_signature_position_bits_get = dnx_data_switch_svtag_egress_svtag_signature_position_bits_get;
    dnx_data_switch.svtag.egress_svtag_sci_size_bits_get = dnx_data_switch_svtag_egress_svtag_sci_size_bits_get;
    dnx_data_switch.svtag.egress_svtag_sci_position_bits_get = dnx_data_switch_svtag_egress_svtag_sci_position_bits_get;
    dnx_data_switch.svtag.egress_svtag_pkt_type_size_bits_get = dnx_data_switch_svtag_egress_svtag_pkt_type_size_bits_get;
    dnx_data_switch.svtag.egress_svtag_pkt_type_position_bits_get = dnx_data_switch_svtag_egress_svtag_pkt_type_position_bits_get;
    dnx_data_switch.svtag.egress_svtag_ipv6_indication_position_bits_get = dnx_data_switch_svtag_egress_svtag_ipv6_indication_position_bits_get;
    dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get = dnx_data_switch_svtag_egress_svtag_hw_field_position_bits_get;
    dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get = dnx_data_switch_svtag_egress_svtag_accumulation_indication_hw_field_position_bits_get;
    dnx_data_switch.svtag.UDP_dedicated_port_get = dnx_data_switch_svtag_UDP_dedicated_port_get;
    dnx_data_switch.svtag.ingress_svtag_position_bytes_get = dnx_data_switch_svtag_ingress_svtag_position_bytes_get;
    dnx_data_switch.svtag.svtag_physical_db_get = dnx_data_switch_svtag_svtag_physical_db_get;

    
    
    
    dnx_data_switch.feature.feature_get = dnx_data_switch_feature_feature_get;

    

    
    
    
    dnx_data_switch.wide_data.feature_get = dnx_data_switch_wide_data_feature_get;

    

    
    
    
    
    dnx_data_synce.general.feature_get = dnx_data_synce_general_feature_get;

    
    dnx_data_synce.general.nof_synce_type_get = dnx_data_synce_general_nof_synce_type_get;
    dnx_data_synce.general.nif_div_min_get = dnx_data_synce_general_nif_div_min_get;
    dnx_data_synce.general.nif_div_max_get = dnx_data_synce_general_nif_div_max_get;
    dnx_data_synce.general.fabric_div_min_get = dnx_data_synce_general_fabric_div_min_get;
    dnx_data_synce.general.fabric_div_max_get = dnx_data_synce_general_fabric_div_max_get;
    dnx_data_synce.general.nof_clk_blocks_get = dnx_data_synce_general_nof_clk_blocks_get;
    dnx_data_synce.general.nof_fabric_clk_blocks_get = dnx_data_synce_general_nof_fabric_clk_blocks_get;
    dnx_data_synce.general.nof_mas_get = dnx_data_synce_general_nof_mas_get;
    dnx_data_synce.general.output_clk_mode_get = dnx_data_synce_general_output_clk_mode_get;

    
    dnx_data_synce.general.cfg_get = dnx_data_synce_general_cfg_get;
    dnx_data_synce.general.cfg_info_get = dnx_data_synce_general_cfg_info_get;
    dnx_data_synce.general.nmg_chain_map_get = dnx_data_synce_general_nmg_chain_map_get;
    dnx_data_synce.general.nmg_chain_map_info_get = dnx_data_synce_general_nmg_chain_map_info_get;
    
    
    
    dnx_data_system_red.config.feature_get = dnx_data_system_red_config_feature_get;

    
    dnx_data_system_red.config.enable_get = dnx_data_system_red_config_enable_get;

    
    
    
    dnx_data_system_red.info.feature_get = dnx_data_system_red_info_feature_get;

    
    dnx_data_system_red.info.nof_red_q_size_get = dnx_data_system_red_info_nof_red_q_size_get;
    dnx_data_system_red.info.max_red_q_size_get = dnx_data_system_red_info_max_red_q_size_get;
    dnx_data_system_red.info.max_ing_aging_period_get = dnx_data_system_red_info_max_ing_aging_period_get;
    dnx_data_system_red.info.max_sch_aging_period_get = dnx_data_system_red_info_max_sch_aging_period_get;
    dnx_data_system_red.info.sch_aging_period_factor_get = dnx_data_system_red_info_sch_aging_period_factor_get;
    dnx_data_system_red.info.max_free_res_thr_range_get = dnx_data_system_red_info_max_free_res_thr_range_get;

    
    dnx_data_system_red.info.resource_get = dnx_data_system_red_info_resource_get;
    dnx_data_system_red.info.resource_info_get = dnx_data_system_red_info_resource_info_get;
    dnx_data_system_red.info.dbal_get = dnx_data_system_red_info_dbal_get;
    dnx_data_system_red.info.dbal_info_get = dnx_data_system_red_info_dbal_info_get;
    
    
    
    dnx_data_tdm.params.feature_get = dnx_data_tdm_params_feature_get;

    
    dnx_data_tdm.params.context_map_base_size_get = dnx_data_tdm_params_context_map_base_size_get;
    dnx_data_tdm.params.context_map_base_nof_get = dnx_data_tdm_params_context_map_base_nof_get;
    dnx_data_tdm.params.sid_min_size_get = dnx_data_tdm_params_sid_min_size_get;
    dnx_data_tdm.params.sid_size_get = dnx_data_tdm_params_sid_size_get;
    dnx_data_tdm.params.nof_stream_ids_get = dnx_data_tdm_params_nof_stream_ids_get;
    dnx_data_tdm.params.global_sid_offset_size_get = dnx_data_tdm_params_global_sid_offset_size_get;
    dnx_data_tdm.params.global_sid_offset_nof_get = dnx_data_tdm_params_global_sid_offset_nof_get;
    dnx_data_tdm.params.sid_offset_size_get = dnx_data_tdm_params_sid_offset_size_get;
    dnx_data_tdm.params.sid_offset_nof_get = dnx_data_tdm_params_sid_offset_nof_get;
    dnx_data_tdm.params.sid_offset_nof_in_bits_get = dnx_data_tdm_params_sid_offset_nof_in_bits_get;
    dnx_data_tdm.params.sid_max_size_get = dnx_data_tdm_params_sid_max_size_get;
    dnx_data_tdm.params.context_size_get = dnx_data_tdm_params_context_size_get;
    dnx_data_tdm.params.nof_contexts_get = dnx_data_tdm_params_nof_contexts_get;
    dnx_data_tdm.params.pkt_size_size_get = dnx_data_tdm_params_pkt_size_size_get;
    dnx_data_tdm.params.pkt_size_lower_limit_get = dnx_data_tdm_params_pkt_size_lower_limit_get;
    dnx_data_tdm.params.pkt_size_upper_limit_get = dnx_data_tdm_params_pkt_size_upper_limit_get;
    dnx_data_tdm.params.link_mask_size_get = dnx_data_tdm_params_link_mask_size_get;
    dnx_data_tdm.params.link_mask_nof_get = dnx_data_tdm_params_link_mask_nof_get;
    dnx_data_tdm.params.tdm_tc_get = dnx_data_tdm_params_tdm_tc_get;
    dnx_data_tdm.params.tdm_dp_get = dnx_data_tdm_params_tdm_dp_get;
    dnx_data_tdm.params.mode_get = dnx_data_tdm_params_mode_get;

    
    
    
    
    dnx_data_techsupport.tm.feature_get = dnx_data_techsupport_tm_feature_get;

    
    dnx_data_techsupport.tm.num_of_tables_per_block_get = dnx_data_techsupport_tm_num_of_tables_per_block_get;
    dnx_data_techsupport.tm.num_of_tm_command_get = dnx_data_techsupport_tm_num_of_tm_command_get;

    
    dnx_data_techsupport.tm.tm_block_get = dnx_data_techsupport_tm_tm_block_get;
    dnx_data_techsupport.tm.tm_block_info_get = dnx_data_techsupport_tm_tm_block_info_get;
    dnx_data_techsupport.tm.tm_command_get = dnx_data_techsupport_tm_tm_command_get;
    dnx_data_techsupport.tm.tm_command_info_get = dnx_data_techsupport_tm_tm_command_info_get;
    dnx_data_techsupport.tm.tm_mem_get = dnx_data_techsupport_tm_tm_mem_get;
    dnx_data_techsupport.tm.tm_mem_info_get = dnx_data_techsupport_tm_tm_mem_info_get;
    
    
    
    dnx_data_time.timesync.feature_get = dnx_data_time_timesync_feature_get;

    

    
    
    
    
    dnx_data_trap.general.feature_get = dnx_data_trap_general_feature_get;

    

    
    
    
    dnx_data_trap.ingress.feature_get = dnx_data_trap_ingress_feature_get;

    
    dnx_data_trap.ingress.protocol_traps_init_mode_get = dnx_data_trap_ingress_protocol_traps_init_mode_get;
    dnx_data_trap.ingress.protocol_traps_lif_profile_starting_offset_get = dnx_data_trap_ingress_protocol_traps_lif_profile_starting_offset_get;
    dnx_data_trap.ingress.max_legal_predefined_trap_id_get = dnx_data_trap_ingress_max_legal_predefined_trap_id_get;
    dnx_data_trap.ingress.nof_user_defined_traps_get = dnx_data_trap_ingress_nof_user_defined_traps_get;
    dnx_data_trap.ingress.nof_user_defined_traps_in_jr1_mode_get = dnx_data_trap_ingress_nof_user_defined_traps_in_jr1_mode_get;
    dnx_data_trap.ingress.nof_1588_traps_get = dnx_data_trap_ingress_nof_1588_traps_get;
    dnx_data_trap.ingress.nof_entries_action_table_get = dnx_data_trap_ingress_nof_entries_action_table_get;
    dnx_data_trap.ingress.nof_protocol_trap_profiles_get = dnx_data_trap_ingress_nof_protocol_trap_profiles_get;
    dnx_data_trap.ingress.nof_recycle_cmds_get = dnx_data_trap_ingress_nof_recycle_cmds_get;
    dnx_data_trap.ingress.nof_programmable_traps_get = dnx_data_trap_ingress_nof_programmable_traps_get;
    dnx_data_trap.ingress.nof_lif_traps_get = dnx_data_trap_ingress_nof_lif_traps_get;
    dnx_data_trap.ingress.nof_icmp_compressed_types_get = dnx_data_trap_ingress_nof_icmp_compressed_types_get;
    dnx_data_trap.ingress.nof_stat_objs_get = dnx_data_trap_ingress_nof_stat_objs_get;
    dnx_data_trap.ingress.max_nof_stat_objs_to_overwrite_get = dnx_data_trap_ingress_max_nof_stat_objs_to_overwrite_get;
    dnx_data_trap.ingress.nof_error_code_profiles_get = dnx_data_trap_ingress_nof_error_code_profiles_get;
    dnx_data_trap.ingress.nof_sci_profiles_get = dnx_data_trap_ingress_nof_sci_profiles_get;
    dnx_data_trap.ingress.nof_error_codes_get = dnx_data_trap_ingress_nof_error_codes_get;
    dnx_data_trap.ingress.nof_scis_get = dnx_data_trap_ingress_nof_scis_get;

    
    
    
    dnx_data_trap.erpp.feature_get = dnx_data_trap_erpp_feature_get;

    
    dnx_data_trap.erpp.nof_erpp_app_traps_get = dnx_data_trap_erpp_nof_erpp_app_traps_get;
    dnx_data_trap.erpp.nof_erpp_forward_profiles_get = dnx_data_trap_erpp_nof_erpp_forward_profiles_get;
    dnx_data_trap.erpp.nof_erpp_user_configurable_profiles_get = dnx_data_trap_erpp_nof_erpp_user_configurable_profiles_get;
    dnx_data_trap.erpp.nof_erpp_snif_profiles_get = dnx_data_trap_erpp_nof_erpp_snif_profiles_get;

    
    
    
    dnx_data_trap.etpp.feature_get = dnx_data_trap_etpp_feature_get;

    
    dnx_data_trap.etpp.nof_etpp_app_traps_get = dnx_data_trap_etpp_nof_etpp_app_traps_get;
    dnx_data_trap.etpp.nof_etpp_user_configurable_profiles_get = dnx_data_trap_etpp_nof_etpp_user_configurable_profiles_get;
    dnx_data_trap.etpp.first_etpp_user_configurable_profile_get = dnx_data_trap_etpp_first_etpp_user_configurable_profile_get;
    dnx_data_trap.etpp.nof_etpp_trap_profiles_get = dnx_data_trap_etpp_nof_etpp_trap_profiles_get;
    dnx_data_trap.etpp.nof_etpp_oam_traps_get = dnx_data_trap_etpp_nof_etpp_oam_traps_get;
    dnx_data_trap.etpp.nof_etpp_snif_profiles_get = dnx_data_trap_etpp_nof_etpp_snif_profiles_get;
    dnx_data_trap.etpp.nof_etpp_lif_traps_get = dnx_data_trap_etpp_nof_etpp_lif_traps_get;
    dnx_data_trap.etpp.trap_context_port_profile_4b_get = dnx_data_trap_etpp_trap_context_port_profile_4b_get;
    dnx_data_trap.etpp.nof_mtu_profiles_get = dnx_data_trap_etpp_nof_mtu_profiles_get;
    dnx_data_trap.etpp.max_mtu_value_get = dnx_data_trap_etpp_max_mtu_value_get;
    dnx_data_trap.etpp.nof_mtu_trap_action_profiles_get = dnx_data_trap_etpp_nof_mtu_trap_action_profiles_get;
    dnx_data_trap.etpp.nof_mtu_map_table_entries_get = dnx_data_trap_etpp_nof_mtu_map_table_entries_get;

    
    
    
    dnx_data_trap.strength.feature_get = dnx_data_trap_strength_feature_get;

    
    dnx_data_trap.strength.max_strength_get = dnx_data_trap_strength_max_strength_get;
    dnx_data_trap.strength.max_snp_strength_get = dnx_data_trap_strength_max_snp_strength_get;
    dnx_data_trap.strength.max_ingress_to_egress_compressed_strength_get = dnx_data_trap_strength_max_ingress_to_egress_compressed_strength_get;
    dnx_data_trap.strength.default_trap_strength_get = dnx_data_trap_strength_default_trap_strength_get;
    dnx_data_trap.strength.default_snoop_strength_get = dnx_data_trap_strength_default_snoop_strength_get;

    
    
    
    dnx_data_trap.register_name_changes.feature_get = dnx_data_trap_register_name_changes_feature_get;

    
    dnx_data_trap.register_name_changes.out_lif_mcdb_ptr_signal_get = dnx_data_trap_register_name_changes_out_lif_mcdb_ptr_signal_get;
    dnx_data_trap.register_name_changes.added_n_get = dnx_data_trap_register_name_changes_added_n_get;
    dnx_data_trap.register_name_changes.per_pp_port_change_get = dnx_data_trap_register_name_changes_per_pp_port_change_get;
    dnx_data_trap.register_name_changes.visibility_signal_0_get = dnx_data_trap_register_name_changes_visibility_signal_0_get;

    
    
    
    
    dnx_data_trunk.parameters.feature_get = dnx_data_trunk_parameters_feature_get;

    
    dnx_data_trunk.parameters.nof_pools_get = dnx_data_trunk_parameters_nof_pools_get;
    dnx_data_trunk.parameters.max_nof_members_in_pool_get = dnx_data_trunk_parameters_max_nof_members_in_pool_get;
    dnx_data_trunk.parameters.max_nof_groups_in_pool_get = dnx_data_trunk_parameters_max_nof_groups_in_pool_get;
    dnx_data_trunk.parameters.spa_pool_shift_get = dnx_data_trunk_parameters_spa_pool_shift_get;
    dnx_data_trunk.parameters.spa_type_shift_get = dnx_data_trunk_parameters_spa_type_shift_get;

    
    dnx_data_trunk.parameters.pool_info_get = dnx_data_trunk_parameters_pool_info_get;
    dnx_data_trunk.parameters.pool_info_info_get = dnx_data_trunk_parameters_pool_info_info_get;
    
    
    dnx_data_trunk.psc.feature_get = dnx_data_trunk_psc_feature_get;

    
    dnx_data_trunk.psc.consistant_hashing_nof_resources_get = dnx_data_trunk_psc_consistant_hashing_nof_resources_get;
    dnx_data_trunk.psc.consistant_hashing_resource_to_entries_ratio_get = dnx_data_trunk_psc_consistant_hashing_resource_to_entries_ratio_get;
    dnx_data_trunk.psc.consistant_hashing_small_group_size_in_bits_get = dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_get;
    dnx_data_trunk.psc.smooth_division_max_nof_member_get = dnx_data_trunk_psc_smooth_division_max_nof_member_get;
    dnx_data_trunk.psc.smooth_division_entries_per_profile_get = dnx_data_trunk_psc_smooth_division_entries_per_profile_get;

    
    
    
    dnx_data_trunk.egress_trunk.feature_get = dnx_data_trunk_egress_trunk_feature_get;

    
    dnx_data_trunk.egress_trunk.nof_get = dnx_data_trunk_egress_trunk_nof_get;
    dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get = dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_get;
    dnx_data_trunk.egress_trunk.dbal_egress_trunk_index_multiplier_get = dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_get;
    dnx_data_trunk.egress_trunk.dbal_entry_index_divider_get = dnx_data_trunk_egress_trunk_dbal_entry_index_divider_get;
    dnx_data_trunk.egress_trunk.dbal_entry_index_msb_multiplier_get = dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_get;
    dnx_data_trunk.egress_trunk.size_mode_get = dnx_data_trunk_egress_trunk_size_mode_get;
    dnx_data_trunk.egress_trunk.invalid_pp_dsp_get = dnx_data_trunk_egress_trunk_invalid_pp_dsp_get;

    
    
    
    dnx_data_trunk.graceful.feature_get = dnx_data_trunk_graceful_feature_get;

    
    dnx_data_trunk.graceful.first_hw_configuration_get = dnx_data_trunk_graceful_first_hw_configuration_get;
    dnx_data_trunk.graceful.second_hw_configuration_get = dnx_data_trunk_graceful_second_hw_configuration_get;

    
    
    
    dnx_data_trunk.flow_agg.feature_get = dnx_data_trunk_flow_agg_feature_get;

    
    dnx_data_trunk.flow_agg.flow_aggs_in_bundle_get = dnx_data_trunk_flow_agg_flow_aggs_in_bundle_get;
    dnx_data_trunk.flow_agg.nof_flow_aggs_get = dnx_data_trunk_flow_agg_nof_flow_aggs_get;
    dnx_data_trunk.flow_agg.nof_flow_agg_bundles_get = dnx_data_trunk_flow_agg_nof_flow_agg_bundles_get;
    dnx_data_trunk.flow_agg.first_flow_agg_id_get = dnx_data_trunk_flow_agg_first_flow_agg_id_get;

    
    
    
    dnx_data_trunk.protection.feature_get = dnx_data_trunk_protection_feature_get;

    

    
    
    
    
    dnx_data_tsn.general.feature_get = dnx_data_tsn_general_feature_get;

    

    
    
    
    dnx_data_tsn.preemption.feature_get = dnx_data_tsn_preemption_feature_get;

    
    dnx_data_tsn.preemption.verify_max_time_get = dnx_data_tsn_preemption_verify_max_time_get;
    dnx_data_tsn.preemption.verify_max_attempts_get = dnx_data_tsn_preemption_verify_max_attempts_get;
    dnx_data_tsn.preemption.non_final_frag_size_tx_get = dnx_data_tsn_preemption_non_final_frag_size_tx_get;
    dnx_data_tsn.preemption.final_frag_size_tx_get = dnx_data_tsn_preemption_final_frag_size_tx_get;
    dnx_data_tsn.preemption.non_final_frag_size_rx_get = dnx_data_tsn_preemption_non_final_frag_size_rx_get;
    dnx_data_tsn.preemption.final_frag_size_rx_get = dnx_data_tsn_preemption_final_frag_size_rx_get;
    dnx_data_tsn.preemption.frag_size_bytes_in_hw_unit_get = dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_get;

    
    
    
    dnx_data_tsn.time.feature_get = dnx_data_tsn_time_feature_get;

    
    dnx_data_tsn.time.tick_period_ns_get = dnx_data_tsn_time_tick_period_ns_get;
    dnx_data_tsn.time.tick_period_min_ns_get = dnx_data_tsn_time_tick_period_min_ns_get;
    dnx_data_tsn.time.tsn_counter_nof_bits_get = dnx_data_tsn_time_tsn_counter_nof_bits_get;
    dnx_data_tsn.time.tas_tod_nof_bits_get = dnx_data_tsn_time_tas_tod_nof_bits_get;
    dnx_data_tsn.time.tas_tod_source_get = dnx_data_tsn_time_tas_tod_source_get;
    dnx_data_tsn.time.tsn_counter_max_diff_get = dnx_data_tsn_time_tsn_counter_max_diff_get;
    dnx_data_tsn.time.tas_tod_update_interrupt_id_get = dnx_data_tsn_time_tas_tod_update_interrupt_id_get;

    
    
    
    dnx_data_tsn.taf.feature_get = dnx_data_tsn_taf_feature_get;

    
    dnx_data_tsn.taf.max_time_intervals_entries_get = dnx_data_tsn_taf_max_time_intervals_entries_get;
    dnx_data_tsn.taf.nof_gates_get = dnx_data_tsn_taf_nof_gates_get;
    dnx_data_tsn.taf.max_profiles_per_gate_get = dnx_data_tsn_taf_max_profiles_per_gate_get;
    dnx_data_tsn.taf.gate_control_list_nof_entries_get = dnx_data_tsn_taf_gate_control_list_nof_entries_get;
    dnx_data_tsn.taf.taf_admission_profile_get = dnx_data_tsn_taf_taf_admission_profile_get;

    
    
    
    dnx_data_tsn.tas.feature_get = dnx_data_tsn_tas_feature_get;

    
    dnx_data_tsn.tas.max_time_intervals_entries_get = dnx_data_tsn_tas_max_time_intervals_entries_get;
    dnx_data_tsn.tas.nof_tas_ports_get = dnx_data_tsn_tas_nof_tas_ports_get;
    dnx_data_tsn.tas.max_profiles_per_port_get = dnx_data_tsn_tas_max_profiles_per_port_get;

    
    
    
    dnx_data_tsn.cqf.feature_get = dnx_data_tsn_cqf_feature_get;

    
    dnx_data_tsn.cqf.max_time_intervals_entries_get = dnx_data_tsn_cqf_max_time_intervals_entries_get;
    dnx_data_tsn.cqf.nof_cqf_ports_get = dnx_data_tsn_cqf_nof_cqf_ports_get;
    dnx_data_tsn.cqf.time_margin_get = dnx_data_tsn_cqf_time_margin_get;

    
    dnx_data_tsn.cqf.port_control_list_init_get = dnx_data_tsn_cqf_port_control_list_init_get;
    dnx_data_tsn.cqf.port_control_list_init_info_get = dnx_data_tsn_cqf_port_control_list_init_info_get;
    
    
    dnx_data_tsn.tsn_thread.feature_get = dnx_data_tsn_tsn_thread_feature_get;

    
    dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get = dnx_data_tsn_tsn_thread_wakeup_interval_nsec_get;
    dnx_data_tsn.tsn_thread.max_process_time_nsec_get = dnx_data_tsn_tsn_thread_max_process_time_nsec_get;
    dnx_data_tsn.tsn_thread.group_window_length_nsec_get = dnx_data_tsn_tsn_thread_group_window_length_nsec_get;

    
    
    
    
    dnx_data_tune.ecgm.feature_get = dnx_data_tune_ecgm_feature_get;

    
    dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get = dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get;
    dnx_data_tune.ecgm.port_queue_uc_flow_control_min_thresh_get = dnx_data_tune_ecgm_port_queue_uc_flow_control_min_thresh_get;
    dnx_data_tune.ecgm.total_dbs_get = dnx_data_tune_ecgm_total_dbs_get;
    dnx_data_tune.ecgm.uc_pool_size_global_core_th_get = dnx_data_tune_ecgm_uc_pool_size_global_core_th_get;
    dnx_data_tune.ecgm.mc_pool_size_global_core_th_get = dnx_data_tune_ecgm_mc_pool_size_global_core_th_get;
    dnx_data_tune.ecgm.mc_drop_global_core_th_get = dnx_data_tune_ecgm_mc_drop_global_core_th_get;
    dnx_data_tune.ecgm.uc_fc_global_core_th_get = dnx_data_tune_ecgm_uc_fc_global_core_th_get;

    
    dnx_data_tune.ecgm.port_dp_ratio_get = dnx_data_tune_ecgm_port_dp_ratio_get;
    dnx_data_tune.ecgm.port_dp_ratio_info_get = dnx_data_tune_ecgm_port_dp_ratio_info_get;
    dnx_data_tune.ecgm.tc_dp_fadt_th_get = dnx_data_tune_ecgm_tc_dp_fadt_th_get;
    dnx_data_tune.ecgm.tc_dp_fadt_th_info_get = dnx_data_tune_ecgm_tc_dp_fadt_th_info_get;
    dnx_data_tune.ecgm.interface_general_fadt_th_get = dnx_data_tune_ecgm_interface_general_fadt_th_get;
    dnx_data_tune.ecgm.interface_general_fadt_th_info_get = dnx_data_tune_ecgm_interface_general_fadt_th_info_get;
    dnx_data_tune.ecgm.interface_rcy_fadt_th_get = dnx_data_tune_ecgm_interface_rcy_fadt_th_get;
    dnx_data_tune.ecgm.interface_rcy_fadt_th_info_get = dnx_data_tune_ecgm_interface_rcy_fadt_th_info_get;
    dnx_data_tune.ecgm.interface_cpu_fadt_th_get = dnx_data_tune_ecgm_interface_cpu_fadt_th_get;
    dnx_data_tune.ecgm.interface_cpu_fadt_th_info_get = dnx_data_tune_ecgm_interface_cpu_fadt_th_info_get;
    
    
    dnx_data_tune.iqs.feature_get = dnx_data_tune_iqs_feature_get;

    
    dnx_data_tune.iqs.credit_resolution_up_th_get = dnx_data_tune_iqs_credit_resolution_up_th_get;
    dnx_data_tune.iqs.fabric_delay_get = dnx_data_tune_iqs_fabric_delay_get;
    dnx_data_tune.iqs.fabric_multicast_delay_get = dnx_data_tune_iqs_fabric_multicast_delay_get;
    dnx_data_tune.iqs.fmq_max_rate_get = dnx_data_tune_iqs_fmq_max_rate_get;

    
    dnx_data_tune.iqs.bw_level_rate_get = dnx_data_tune_iqs_bw_level_rate_get;
    dnx_data_tune.iqs.bw_level_rate_info_get = dnx_data_tune_iqs_bw_level_rate_info_get;
    
    
    dnx_data_tune.fabric.feature_get = dnx_data_tune_fabric_feature_get;

    
    dnx_data_tune.fabric.cgm_llfc_pipe_th_get = dnx_data_tune_fabric_cgm_llfc_pipe_th_get;
    dnx_data_tune.fabric.cgm_drop_fabric_class_th_get = dnx_data_tune_fabric_cgm_drop_fabric_class_th_get;
    dnx_data_tune.fabric.cgm_rci_high_sev_min_links_param_get = dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get;
    dnx_data_tune.fabric.cgm_rci_egress_pipe_level_th_base_get = dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_base_get;
    dnx_data_tune.fabric.cgm_rci_egress_pipe_level_th_offset_get = dnx_data_tune_fabric_cgm_rci_egress_pipe_level_th_offset_get;
    dnx_data_tune.fabric.cgm_rci_total_egress_pipe_level_th_base_get = dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_base_get;
    dnx_data_tune.fabric.cgm_rci_total_egress_pipe_level_th_offset_get = dnx_data_tune_fabric_cgm_rci_total_egress_pipe_level_th_offset_get;

    
    dnx_data_tune.fabric.cgm_drop_fabric_multicast_th_get = dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_get;
    dnx_data_tune.fabric.cgm_drop_fabric_multicast_th_info_get = dnx_data_tune_fabric_cgm_drop_fabric_multicast_th_info_get;
    dnx_data_tune.fabric.cgm_rci_egress_level_factor_get = dnx_data_tune_fabric_cgm_rci_egress_level_factor_get;
    dnx_data_tune.fabric.cgm_rci_egress_level_factor_info_get = dnx_data_tune_fabric_cgm_rci_egress_level_factor_info_get;
    
    
    
    dnx_data_tunnel.udp.feature_get = dnx_data_tunnel_udp_feature_get;

    
    dnx_data_tunnel.udp.nof_udp_ports_profiles_get = dnx_data_tunnel_udp_nof_udp_ports_profiles_get;
    dnx_data_tunnel.udp.first_udp_ports_profile_get = dnx_data_tunnel_udp_first_udp_ports_profile_get;

    
    
    
    dnx_data_tunnel.tunnel_type.feature_get = dnx_data_tunnel_tunnel_type_feature_get;

    
    dnx_data_tunnel.tunnel_type.udp_tunnel_type_get = dnx_data_tunnel_tunnel_type_udp_tunnel_type_get;
    dnx_data_tunnel.tunnel_type.gre_tunnel_type_get = dnx_data_tunnel_tunnel_type_gre_tunnel_type_get;
    dnx_data_tunnel.tunnel_type.geneve_tunnel_type_supported_get = dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_get;

    
    
    
    dnx_data_tunnel.roo.feature_get = dnx_data_tunnel_roo_feature_get;

    
    dnx_data_tunnel.roo.terminate_only_8B_in_ip_at_term_block_using_pmf_get = dnx_data_tunnel_roo_terminate_only_8B_in_ip_at_term_block_using_pmf_get;

    
    
    
    dnx_data_tunnel.feature.feature_get = dnx_data_tunnel_feature_feature_get;

    
    dnx_data_tunnel.feature.tunnel_route_disable_get = dnx_data_tunnel_feature_tunnel_route_disable_get;
    dnx_data_tunnel.feature.dedicated_fodo_to_vni_table_get = dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_get;
    dnx_data_tunnel.feature.dip_idx_type_is_lif_field_get = dnx_data_tunnel_feature_dip_idx_type_is_lif_field_get;
    dnx_data_tunnel.feature.fodo_to_vni_entry_created_by_vlan_api_get = dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_get;
    dnx_data_tunnel.feature.has_flow_gport_to_tunnel_type_get = dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_get;
    dnx_data_tunnel.feature.support_learn_mode_async_lif_and_fec_get = dnx_data_tunnel_feature_support_learn_mode_async_lif_and_fec_get;
    dnx_data_tunnel.feature.l3_fodo_result_type_supported_get = dnx_data_tunnel_feature_l3_fodo_result_type_supported_get;
    dnx_data_tunnel.feature.udp_tunnel_nat_get = dnx_data_tunnel_feature_udp_tunnel_nat_get;
    dnx_data_tunnel.feature.short_network_domain_get = dnx_data_tunnel_feature_short_network_domain_get;

    
    
    
    
    dnx_data_vlan.vlan_translate.feature_get = dnx_data_vlan_vlan_translate_feature_get;

    
    dnx_data_vlan.vlan_translate.nof_vlan_tag_formats_get = dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get;
    dnx_data_vlan.vlan_translate.nof_ingress_vlan_edit_profiles_get = dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get;
    dnx_data_vlan.vlan_translate.nof_egress_vlan_edit_profiles_get = dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get;
    dnx_data_vlan.vlan_translate.nof_vlan_range_entries_get = dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get;
    dnx_data_vlan.vlan_translate.nof_vlan_ranges_per_entry_get = dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get;
    dnx_data_vlan.vlan_translate.egress_tpid_outer_packet_index_get = dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get;
    dnx_data_vlan.vlan_translate.egress_tpid_inner_packet_index_get = dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get;
    dnx_data_vlan.vlan_translate.nof_ingress_vlan_edit_action_ids_get = dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get;
    dnx_data_vlan.vlan_translate.nof_egress_vlan_edit_action_ids_get = dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get;

    
    
    
    dnx_data_vlan.llvp.feature_get = dnx_data_vlan_llvp_feature_get;

    
    dnx_data_vlan.llvp.ext_vcp_en_get = dnx_data_vlan_llvp_ext_vcp_en_get;

    
    
    dnx_data_valid = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_if_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    
    

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    _dnxc_data[unit].name = "dnx_data";
    _dnxc_data[unit].nof_modules = _dnx_data_module_nof;
    DNXC_DATA_ALLOC(_dnxc_data[unit].modules, dnxc_data_module_t,  _dnxc_data[unit].nof_modules, "_dnxc_data modules");

    
    SHR_IF_ERR_EXIT(dnx_data_if_init(unit));
    
    SHR_IF_ERR_EXIT(dnx_data_access_init(unit, &_dnxc_data[unit].modules[dnx_data_module_access]));
    SHR_IF_ERR_EXIT(dnx_data_adapter_init(unit, &_dnxc_data[unit].modules[dnx_data_module_adapter]));
    SHR_IF_ERR_EXIT(dnx_data_bfd_init(unit, &_dnxc_data[unit].modules[dnx_data_module_bfd]));
    SHR_IF_ERR_EXIT(dnx_data_bier_init(unit, &_dnxc_data[unit].modules[dnx_data_module_bier]));
    SHR_IF_ERR_EXIT(dnx_data_consistent_hashing_init(unit, &_dnxc_data[unit].modules[dnx_data_module_consistent_hashing]));
    SHR_IF_ERR_EXIT(dnx_data_cosq_aqm_init(unit, &_dnxc_data[unit].modules[dnx_data_module_cosq_aqm]));
    SHR_IF_ERR_EXIT(dnx_data_crps_init(unit, &_dnxc_data[unit].modules[dnx_data_module_crps]));
    SHR_IF_ERR_EXIT(dnx_data_dbal_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_debug_init(unit, &_dnxc_data[unit].modules[dnx_data_module_debug]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dev_init]));
    SHR_IF_ERR_EXIT(dnx_data_device_init(unit, &_dnxc_data[unit].modules[dnx_data_module_device]));

    SHR_IF_ERR_EXIT(dnx_data_dnx2_aod_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dnx2_aod]));


    SHR_IF_ERR_EXIT(dnx_data_dnx2_aod_sizes_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dnx2_aod_sizes]));


    SHR_IF_ERR_EXIT(dnx_data_dnx2_arr_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dnx2_arr]));


    SHR_IF_ERR_EXIT(dnx_data_dnx2_mdb_app_db_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dnx2_mdb_app_db]));


    SHR_IF_ERR_EXIT(dnx_data_dnx2_pemla_soc_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dnx2_pemla_soc]));

    SHR_IF_ERR_EXIT(dnx_data_dram_init(unit, &_dnxc_data[unit].modules[dnx_data_module_dram]));
    SHR_IF_ERR_EXIT(dnx_data_ecgm_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ecgm]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_init(unit, &_dnxc_data[unit].modules[dnx_data_module_egr_queuing]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_init(unit, &_dnxc_data[unit].modules[dnx_data_module_egr_queuing_v1]));
    SHR_IF_ERR_EXIT(dnx_data_elk_init(unit, &_dnxc_data[unit].modules[dnx_data_module_elk]));
    SHR_IF_ERR_EXIT(dnx_data_esb_init(unit, &_dnxc_data[unit].modules[dnx_data_module_esb]));
    SHR_IF_ERR_EXIT(dnx_data_esem_init(unit, &_dnxc_data[unit].modules[dnx_data_module_esem]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_init(unit, &_dnxc_data[unit].modules[dnx_data_module_fabric]));
    SHR_IF_ERR_EXIT(dnx_data_failover_init(unit, &_dnxc_data[unit].modules[dnx_data_module_failover]));
    SHR_IF_ERR_EXIT(dnx_data_fc_init(unit, &_dnxc_data[unit].modules[dnx_data_module_fc]));
    SHR_IF_ERR_EXIT(dnx_data_field_init(unit, &_dnxc_data[unit].modules[dnx_data_module_field]));
    SHR_IF_ERR_EXIT(dnx_data_field_map_init(unit, &_dnxc_data[unit].modules[dnx_data_module_field_map]));
    SHR_IF_ERR_EXIT(dnx_data_fifodma_init(unit, &_dnxc_data[unit].modules[dnx_data_module_fifodma]));
    SHR_IF_ERR_EXIT(dnx_data_flow_init(unit, &_dnxc_data[unit].modules[dnx_data_module_flow]));
    SHR_IF_ERR_EXIT(dnx_data_graphical_init(unit, &_dnxc_data[unit].modules[dnx_data_module_graphical]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_init(unit, &_dnxc_data[unit].modules[dnx_data_module_gtimer]));
    SHR_IF_ERR_EXIT(dnx_data_gtp_init(unit, &_dnxc_data[unit].modules[dnx_data_module_gtp]));
    SHR_IF_ERR_EXIT(dnx_data_headers_init(unit, &_dnxc_data[unit].modules[dnx_data_module_headers]));
#ifdef INCLUDE_CTEST

    SHR_IF_ERR_EXIT(dnx_data_infra_tests_init(unit, &_dnxc_data[unit].modules[dnx_data_module_infra_tests]));

#endif 
    SHR_IF_ERR_EXIT(dnx_data_ingr_congestion_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ingr_congestion]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ingr_reassembly]));
    SHR_IF_ERR_EXIT(dnx_data_ingress_cs_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ingress_cs]));
    SHR_IF_ERR_EXIT(dnx_data_instru_init(unit, &_dnxc_data[unit].modules[dnx_data_module_instru]));
    SHR_IF_ERR_EXIT(dnx_data_intr_init(unit, &_dnxc_data[unit].modules[dnx_data_module_intr]));
    SHR_IF_ERR_EXIT(dnx_data_ipq_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ipq]));
    SHR_IF_ERR_EXIT(dnx_data_iqs_init(unit, &_dnxc_data[unit].modules[dnx_data_module_iqs]));
    SHR_IF_ERR_EXIT(dnx_data_kaps_init(unit, &_dnxc_data[unit].modules[dnx_data_module_kaps]));
    SHR_IF_ERR_EXIT(dnx_data_kleap_stage_info_init(unit, &_dnxc_data[unit].modules[dnx_data_module_kleap_stage_info]));
    SHR_IF_ERR_EXIT(dnx_data_l2_init(unit, &_dnxc_data[unit].modules[dnx_data_module_l2]));
    SHR_IF_ERR_EXIT(dnx_data_l3_init(unit, &_dnxc_data[unit].modules[dnx_data_module_l3]));
    SHR_IF_ERR_EXIT(dnx_data_lane_map_init(unit, &_dnxc_data[unit].modules[dnx_data_module_lane_map]));
    SHR_IF_ERR_EXIT(dnx_data_latency_init(unit, &_dnxc_data[unit].modules[dnx_data_module_latency]));
    SHR_IF_ERR_EXIT(dnx_data_led_init(unit, &_dnxc_data[unit].modules[dnx_data_module_led]));
    SHR_IF_ERR_EXIT(dnx_data_lif_init(unit, &_dnxc_data[unit].modules[dnx_data_module_lif]));
    SHR_IF_ERR_EXIT(dnx_data_linkscan_init(unit, &_dnxc_data[unit].modules[dnx_data_module_linkscan]));
    SHR_IF_ERR_EXIT(dnx_data_macsec_init(unit, &_dnxc_data[unit].modules[dnx_data_module_macsec]));
    SHR_IF_ERR_EXIT(dnx_data_mdb_init(unit, &_dnxc_data[unit].modules[dnx_data_module_mdb]));
    SHR_IF_ERR_EXIT(dnx_data_meter_init(unit, &_dnxc_data[unit].modules[dnx_data_module_meter]));
    SHR_IF_ERR_EXIT(dnx_data_mib_init(unit, &_dnxc_data[unit].modules[dnx_data_module_mib]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_init(unit, &_dnxc_data[unit].modules[dnx_data_module_module_testing]));
    SHR_IF_ERR_EXIT(dnx_data_mpls_init(unit, &_dnxc_data[unit].modules[dnx_data_module_mpls]));
    SHR_IF_ERR_EXIT(dnx_data_multicast_init(unit, &_dnxc_data[unit].modules[dnx_data_module_multicast]));
    SHR_IF_ERR_EXIT(dnx_data_nif_init(unit, &_dnxc_data[unit].modules[dnx_data_module_nif]));
    SHR_IF_ERR_EXIT(dnx_data_oam_init(unit, &_dnxc_data[unit].modules[dnx_data_module_oam]));
    SHR_IF_ERR_EXIT(dnx_data_onesync_init(unit, &_dnxc_data[unit].modules[dnx_data_module_onesync]));
    SHR_IF_ERR_EXIT(dnx_data_parser_init(unit, &_dnxc_data[unit].modules[dnx_data_module_parser]));
    SHR_IF_ERR_EXIT(dnx_data_pll_init(unit, &_dnxc_data[unit].modules[dnx_data_module_pll]));
    SHR_IF_ERR_EXIT(dnx_data_port_init(unit, &_dnxc_data[unit].modules[dnx_data_module_port]));
    SHR_IF_ERR_EXIT(dnx_data_port_pp_init(unit, &_dnxc_data[unit].modules[dnx_data_module_port_pp]));
    SHR_IF_ERR_EXIT(dnx_data_pp_init(unit, &_dnxc_data[unit].modules[dnx_data_module_pp]));
    SHR_IF_ERR_EXIT(dnx_data_ppmc_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ppmc]));
    SHR_IF_ERR_EXIT(dnx_data_ptp_init(unit, &_dnxc_data[unit].modules[dnx_data_module_ptp]));
    SHR_IF_ERR_EXIT(dnx_data_pvt_init(unit, &_dnxc_data[unit].modules[dnx_data_module_pvt]));
    SHR_IF_ERR_EXIT(dnx_data_qos_init(unit, &_dnxc_data[unit].modules[dnx_data_module_qos]));
    SHR_IF_ERR_EXIT(dnx_data_regression_init(unit, &_dnxc_data[unit].modules[dnx_data_module_regression]));
    SHR_IF_ERR_EXIT(dnx_data_sat_init(unit, &_dnxc_data[unit].modules[dnx_data_module_sat]));
    SHR_IF_ERR_EXIT(dnx_data_sbusdma_desc_init(unit, &_dnxc_data[unit].modules[dnx_data_module_sbusdma_desc]));
    SHR_IF_ERR_EXIT(dnx_data_sch_init(unit, &_dnxc_data[unit].modules[dnx_data_module_sch]));
    SHR_IF_ERR_EXIT(dnx_data_snif_init(unit, &_dnxc_data[unit].modules[dnx_data_module_snif]));
    SHR_IF_ERR_EXIT(dnx_data_spb_init(unit, &_dnxc_data[unit].modules[dnx_data_module_spb]));
    SHR_IF_ERR_EXIT(dnx_data_srv6_init(unit, &_dnxc_data[unit].modules[dnx_data_module_srv6]));
    SHR_IF_ERR_EXIT(dnx_data_stack_init(unit, &_dnxc_data[unit].modules[dnx_data_module_stack]));
    SHR_IF_ERR_EXIT(dnx_data_stat_init(unit, &_dnxc_data[unit].modules[dnx_data_module_stat]));
    SHR_IF_ERR_EXIT(dnx_data_stg_init(unit, &_dnxc_data[unit].modules[dnx_data_module_stg]));
    SHR_IF_ERR_EXIT(dnx_data_stif_init(unit, &_dnxc_data[unit].modules[dnx_data_module_stif]));
    SHR_IF_ERR_EXIT(dnx_data_switch_init(unit, &_dnxc_data[unit].modules[dnx_data_module_switch]));
    SHR_IF_ERR_EXIT(dnx_data_synce_init(unit, &_dnxc_data[unit].modules[dnx_data_module_synce]));
    SHR_IF_ERR_EXIT(dnx_data_system_red_init(unit, &_dnxc_data[unit].modules[dnx_data_module_system_red]));
    SHR_IF_ERR_EXIT(dnx_data_tdm_init(unit, &_dnxc_data[unit].modules[dnx_data_module_tdm]));
    SHR_IF_ERR_EXIT(dnx_data_techsupport_init(unit, &_dnxc_data[unit].modules[dnx_data_module_techsupport]));
    SHR_IF_ERR_EXIT(dnx_data_time_init(unit, &_dnxc_data[unit].modules[dnx_data_module_time]));
    SHR_IF_ERR_EXIT(dnx_data_trap_init(unit, &_dnxc_data[unit].modules[dnx_data_module_trap]));
    SHR_IF_ERR_EXIT(dnx_data_trunk_init(unit, &_dnxc_data[unit].modules[dnx_data_module_trunk]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_init(unit, &_dnxc_data[unit].modules[dnx_data_module_tsn]));
    SHR_IF_ERR_EXIT(dnx_data_tune_init(unit, &_dnxc_data[unit].modules[dnx_data_module_tune]));
    SHR_IF_ERR_EXIT(dnx_data_tunnel_init(unit, &_dnxc_data[unit].modules[dnx_data_module_tunnel]));
    SHR_IF_ERR_EXIT(dnx_data_vlan_init(unit, &_dnxc_data[unit].modules[dnx_data_module_vlan]));

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_parse_init(unit));

    
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_INIT_DONE));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_values_set(unit));

    
    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnx_data_property_unsupported_verify(unit)));

    
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_DEV_DATA_DONE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_CONT(dnx_data_if_deinit(unit));

    
    SHR_IF_ERR_CONT(dnxc_data_mgmt_deinit(unit, &_dnxc_data[unit]));

    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE

