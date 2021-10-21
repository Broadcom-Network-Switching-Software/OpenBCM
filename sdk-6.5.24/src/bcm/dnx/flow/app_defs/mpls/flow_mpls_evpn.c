#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include "flow_mpls_utils.h"

#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>

