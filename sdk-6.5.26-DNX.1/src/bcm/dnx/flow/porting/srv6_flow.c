/** \file srv6_flow.c
 *  * srv6 sid initiator APIs to flows APIs conversion.
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
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm/srv6.h>
#include <bcm_int/dnx/srv6/srv6.h>
#include <soc/dnx/dbal/dbal_external_defines.h>
#include <shared/utilex/utilex_bitstream.h>
#include <src/bcm/dnx/tunnel/tunnel_term_srv6_utils.h>
#include <src/bcm/dnx/tunnel/tunnel_term_srv6.h>
#include <include/bcm_int/dnx/algo/swstate/auto_generated/access/algo_srv6_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
