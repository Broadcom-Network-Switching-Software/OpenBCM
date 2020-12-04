/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * File:       udf_common.c
 * Purpose:    Contains initilization of UDF common resources
 *             and manage device specific driver functions.
 */

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/bitop.h>

#include <bcm/udf.h>
#include <bcm_int/esw/udf.h>

_bcm_udf_control_t *_udf_control[BCM_MAX_NUM_UNITS];

/*
 * Function to detach UDF common resources.
 */
int _bcm_udf_common_detach(int unit)
{
    _bcm_udf_control_t *udf_ctrl = NULL;

    udf_ctrl = UDF_CONTROL(unit);
    if (udf_ctrl == NULL) {
       return BCM_E_NONE;
    }

    if (udf_ctrl->functions.udf_detach != NULL) {
       (void) udf_ctrl->functions.udf_detach(unit);
    } else {
       return BCM_E_INTERNAL;
    }

    _BCM_UDF_FREE(udf_ctrl);
    UDF_CONTROL(unit) = NULL;
    return BCM_E_NONE;
}

/*
 * Function to initialize UDF common resources and
 * device specific routines.
 */
int _bcm_udf_common_init(int unit)
{
    int rv;
    _bcm_udf_control_t *udf_ctrl = NULL;

    udf_ctrl = UDF_CONTROL(unit);

    /* Detatch first if it's been previously initialized. */
    if (udf_ctrl != NULL) {
        rv = _bcm_udf_common_detach(unit);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
        udf_ctrl = NULL;
    }

    /* Allocate a bcm_udf_control */
    _BCM_UDF_ALLOC(udf_ctrl, sizeof (_bcm_udf_control_t), "UDF common control");
    if (NULL == udf_ctrl) {
        return (BCM_E_MEMORY);
    }

    UDF_CONTROL(unit) = udf_ctrl;

#if defined BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_udf_td3x_support)) {
       /* Initialize the function pointers */
       _bcm_udf_td3_functions_init(&udf_ctrl->functions);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    if (udf_ctrl->functions.udf_init != NULL) {
       rv = udf_ctrl->functions.udf_init(unit);
    } else {
       rv = BCM_E_INIT;
    }
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    return BCM_E_NONE;

cleanup:
    if (UDF_CONTROL(unit) != NULL) {
        _BCM_UDF_FREE(UDF_CONTROL(unit));
    }
    return rv;
}
