

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_H_

#define _DNX_DATA_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_adapter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_consistent_hashing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_consistent_hashing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_graphical.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_gtimer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_gtimer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_latency.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_latency.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_linkscan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_linkscan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_macsec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_module_testing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_module_testing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ppmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ppmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pvt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_regression.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_regression.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_spb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_spb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_time.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_time.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tune.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_utilex.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_utilex.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_aod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_aod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_aod_sizes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_aod_sizes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_arr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mdb_app_db.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb_app_db.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pemla_soc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pemla_soc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_init(
    int unit);




shr_error_e dnx_data_deinit(
    int unit);



shr_error_e dnx_data_if_init(
    int unit);


shr_error_e dnx_data_if_deinit(
    int unit);




#endif 

