/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
#ifndef _SHR_SW_STATE_INCLUDES_H
/* { */
#define _SHR_SW_STATE_INCLUDES_H

#include <shared/swstate/sw_state_hash_tbl.h>
#include <shared/swstate/sw_state_res_tag_bitmap.h>
#include <shared/swstate/sw_state_resmgr.h>
#include <shared/shr_template.h>

#if defined(BCM_PETRA_SUPPORT)
/* { */
#include <bcm_int/dpp/counters.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/QAX/qax_ipsec.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam_mep_db.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>
#include <soc/dpp/TMC/tmc_api_link_bonding.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <bcm_int/dpp/stg.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/l2.h>
#include <bcm_int/dpp/trunk.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/ipmc.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/lb.h>
#include <bcm_int/dpp/bfd_uc.h>
/* } */
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */
#if (defined(BCM_DNX_SUPPORT))
/* { */
#include <sal/core/alloc.h>
#include <shared/utilex/utilex_multi_set.h>
/* } */
#endif

/* } */
#endif /*_SHR_SW_STATE_INCLUDES_H*/
