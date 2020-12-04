

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_H_

#define _DNX_DATA_INTERNAL_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_dyn_loader.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_module_port,
    dnx_data_module_access,
    dnx_data_module_adapter,
    dnx_data_module_bfd,
    dnx_data_module_bier,
    dnx_data_module_consistent_hashing,
    dnx_data_module_ingr_congestion,
    dnx_data_module_crps,
    dnx_data_module_dbal,
    dnx_data_module_debug,
    dnx_data_module_dev_init,
    dnx_data_module_device,
    dnx_data_module_dram,
    dnx_data_module_ecgm,
    dnx_data_module_egr_queuing,
    dnx_data_module_elk,
    dnx_data_module_esb,
    dnx_data_module_esem,
    dnx_data_module_fabric,
    dnx_data_module_failover,
    dnx_data_module_fc,
    dnx_data_module_field,
    dnx_data_module_fifodma,
    dnx_data_module_graphical,
    dnx_data_module_gtimer,
    dnx_data_module_headers,
    dnx_data_module_ingress_cs,
    dnx_data_module_instru,
    dnx_data_module_intr,
    dnx_data_module_ipq,
    dnx_data_module_iqs,
    dnx_data_module_l2,
    dnx_data_module_l3,
    dnx_data_module_lane_map,
    dnx_data_module_latency,
    dnx_data_module_lif,
    dnx_data_module_linkscan,
    dnx_data_module_macsec,
    dnx_data_module_mdb,
    dnx_data_module_meter,
    dnx_data_module_mib,
    dnx_data_module_module_testing,
    dnx_data_module_mpls,
    dnx_data_module_multicast,
    dnx_data_module_nif,
    dnx_data_module_oam,
    dnx_data_module_pll,
    dnx_data_module_ingr_reassembly,
    dnx_data_module_port_pp,
    dnx_data_module_pp,
    dnx_data_module_ppmc,
    dnx_data_module_ptp,
    dnx_data_module_pvt,
    dnx_data_module_qos,
    dnx_data_module_regression,
    dnx_data_module_sat,
    dnx_data_module_sbusdma_desc,
    dnx_data_module_sch,
    dnx_data_module_snif,
    dnx_data_module_spb,
    dnx_data_module_srv6,
    dnx_data_module_stack,
    dnx_data_module_stat,
    dnx_data_module_stg,
    dnx_data_module_stif,
    dnx_data_module_switch,
    dnx_data_module_synce,
    dnx_data_module_system_red,
    dnx_data_module_tdm,
    dnx_data_module_time,
    dnx_data_module_trap,
    dnx_data_module_trunk,
    dnx_data_module_tune,
    dnx_data_module_tunnel,
    dnx_data_module_utilex,
    dnx_data_module_vlan,
    dnx_data_module_aod,
    dnx_data_module_aod_sizes,
    dnx_data_module_arr,
    dnx_data_module_mdb_app_db,
    dnx_data_module_pemla_soc,

    
    _dnx_data_module_nof
} dnx_data_module_e;




#endif 

