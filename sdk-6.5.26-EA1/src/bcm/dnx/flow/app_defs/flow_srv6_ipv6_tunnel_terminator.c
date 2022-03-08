#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <include/bcm_int/dnx/algo/swstate/auto_generated/access/algo_srv6_access.h>
#include <src/bcm/dnx/flow/app_defs/flow_srv6_ipv6_tunnel_terminator.h>

