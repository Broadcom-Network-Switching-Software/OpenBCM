#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/lif/cs_in_lif_profile_based_on_stages.h>

