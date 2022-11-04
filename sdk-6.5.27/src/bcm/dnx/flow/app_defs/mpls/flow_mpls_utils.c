#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mpls_access.h>
#include <src/bcm/dnx/mpls_port/mpls_port.h>
#include <bcm_int/dnx/switch/switch.h>
#include "flow_mpls_utils.h"
#include <bcm_int/dnx/algo/algo_gpm.h>
