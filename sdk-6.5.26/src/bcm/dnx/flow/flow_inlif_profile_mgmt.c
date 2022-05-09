#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/lif/cs_in_lif_profile_based_on_stages.h>
#include <src/bcm/dnx/tunnel/tunnel_term_srv6_utils.h>
#include <src/bcm/dnx/flow/app_defs/flow_srv6_ipv6_tunnel_terminator.h>

