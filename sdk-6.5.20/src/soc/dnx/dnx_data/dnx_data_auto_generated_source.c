

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#define DNX_DATA_INTERNAL

#include <src/soc/dnx/dnx_data/auto_generated/dnx_data.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_port.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_access.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_adapter.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_bier.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_consistent_hashing.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_crps.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_dbal.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_debug.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_device.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_dram.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_elk.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_esb.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_esem.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_fabric.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_failover.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_fc.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_field.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_fifodma.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_graphical.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_gtimer.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_headers.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_instru.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_intr.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ipq.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_iqs.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_l2.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_l3.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_latency.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_lif.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_linkscan.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_macsec.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_mdb.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_meter.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_mib.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_mpls.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_multicast.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_nif.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_oam.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_pll.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_pp.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ppmc.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_ptp.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_pvt.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_qos.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_regression.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_sat.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_sch.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_snif.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_spb.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_srv6.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_stack.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_stat.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_stg.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_stif.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_switch.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_synce.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_system_red.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_tdm.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_time.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_trap.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_trunk.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_tune.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_utilex.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_vlan.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_aod.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_aod_sizes.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_arr.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_mdb_app_db.c>
#include <src/soc/dnx/dnx_data/auto_generated/dnx_data_pemla_soc.c>



