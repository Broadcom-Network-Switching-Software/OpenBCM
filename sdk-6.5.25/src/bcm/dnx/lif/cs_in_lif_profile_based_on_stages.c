/*
 * cs_in_lif_profile_based_on_stages.c
 *
 *  Created on: Jul 8, 2021
 *      Author: sk889844
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/cs_in_lif_profile_based_on_stages.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>

