/** \file mpls_evpn_flow.c
 *  * mpls evpn APIs to flows APIs conversion.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/flow.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/qos/qos.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <src/bcm/dnx/mpls_l2vpn/mpls_l2vpn_ingress.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <src/bcm/dnx/flow/app_defs/mpls/flow_mpls_utils.h>
#include "mpls_evpn_flow.h"
