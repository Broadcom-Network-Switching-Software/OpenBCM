/**
 * \file mpls_tunnel_term.c
 * MPLS functionality for DNX tunnel termination.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm/flow.h>
#include "src/bcm/dnx/flow/flow_def.h"
#include <bcm_int/dnx/flow/flow.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "mpls_flow.h"
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <src/bcm/dnx/flow/app_defs/mpls/flow_mpls_utils.h>

#include <bcm_int/dnx_dispatch.h>
