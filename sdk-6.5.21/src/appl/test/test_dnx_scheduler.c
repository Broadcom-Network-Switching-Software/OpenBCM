/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_APPLDNX_TESTS

#include <sal/types.h>

#if defined (BCM_DNX_SUPPORT)

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <sal/appl/config.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>

#include "testlist.h"

/**
 * \brief - Check if we have shared shapers and if its parents color groups matches
 */
static int
test_dnx_sch_shared_shaper_check_all_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    int rv = 0;
    bcm_gport_t parent;
    bcm_cosq_scheduler_gport_params_t params;
    bcm_cosq_gport_info_t gport_info;
    int nof_elements_of_shared_shaper = 0;
    int first_parent_color, next_parent_color, cos = 0;


    if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        /* Check if we have a shared shaper */
        rv = bcm_cosq_scheduler_gport_get(unit, 0, gport, &params);
        if (rv != 0)
        {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Error - couldn't get the params for shared shaper.\n")));
            return BCM_E_FAIL;
        }
        if (params.mode == bcmCosqSchedulerGportModeSharedDual ||
            params.mode == bcmCosqSchedulerGportModeSharedQuad || params.mode == bcmCosqSchedulerGportModeSharedOcta)
        {
            /* Check what type is the shaper*/
            switch (params.mode)
            {
                case bcmCosqSchedulerGportModeSharedDual:
                    nof_elements_of_shared_shaper = 2;
                    break;
                case bcmCosqSchedulerGportModeSharedQuad:
                    nof_elements_of_shared_shaper = 4;
                    break;
                case bcmCosqSchedulerGportModeSharedOcta:
                    nof_elements_of_shared_shaper = 8;
                    break;
                default:
                    break;
            }
            /* Get the color group of the parent of the first element in the shaper*/
            gport_info.in_gport = gport;
            gport_info.cosq = 0;
            rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0, &gport_info);
            if (rv != 0)
            {
                LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Error - couldn't get handle for the first element of shared shaper.\n")));
                return BCM_E_FAIL;
            }
            rv = bcm_cosq_gport_parent_get(unit, gport_info.out_gport, -1, &parent);
            if (rv != 0)
            {
                LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Error - couldn't get the parent of the first element of shared shaper.\n")));
                return BCM_E_FAIL;
            }
            rv = bcm_cosq_control_get(unit, parent, -1, bcmCosqControlGroup, &first_parent_color);
            if (rv != 0)
            {
                LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Error - couldn't get the color group of the first parent element.\n")));
                return BCM_E_FAIL;
            }
            for (cos = 1; cos < nof_elements_of_shared_shaper; cos++) {
                /* Get next parent element from the shared shaper */
                rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSchedSharedShaper0 + cos, &gport_info);
                if (rv != 0)
                {
                    LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Error - couldn't get handle for the %d element of shared shaper.\n"), cos + 1));
                    return BCM_E_FAIL;
                }
                rv = bcm_cosq_gport_parent_get(unit, gport_info.out_gport,-1, &parent);
                if (rv != 0)
                {
                    LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Error - couldn't get the parent of the %d element of shared shaper.\n"), cos + 1));
                    return BCM_E_FAIL;
                }
                /* Get the color group of the parent */
                rv = bcm_cosq_control_get(unit, parent, -1, bcmCosqControlGroup, &next_parent_color);
                if (rv != 0)
                {
                    LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Error - couldn't get the color group of the %d parent element.\n"), cos + 1));
                    return BCM_E_FAIL;
                }
                /* Compare if the color group matches*/
                if (first_parent_color != next_parent_color)
                {
                    LOG_ERROR(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "Shared shaper parents groups mismatches. Index 0 - group %d , Index %d - group %d\n"),
                            first_parent_color, cos, next_parent_color));
                    return BCM_E_FAIL;
                }
            }
        }
    }

    return rv;
}

/* The main function for TR test if scheduling scheme is created correct */
int
test_dnx_sch_scheduling_scheme_test(int unit, args_t *a, void *pa)
{
    int rv = 0;

    rv = bcm_cosq_gport_traverse(unit, test_dnx_sch_shared_shaper_check_all_cb, 0);

    return rv;
}

#endif

#undef _ERR_MSG_MODULE_NAME
