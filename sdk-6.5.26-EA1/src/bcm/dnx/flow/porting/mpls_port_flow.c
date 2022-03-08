/** \file mpls_port_flow.c
 *  * mpls port APIs to flows APIs conversion.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm/flow.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include "tunnel_init_flow.h"
#include <bcm_int/dnx/mpls_l2vpn/mpls_l2vpn.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <src/bcm/dnx/mpls_l2vpn/mpls_l2vpn_ingress.h>
#include <src/bcm/dnx/mpls_port/mpls_port.h>
#include <bcm_int/dnx/failover/failover.h>
#include <src/bcm/dnx/flow/app_defs/mpls/flow_mpls_utils.h>
