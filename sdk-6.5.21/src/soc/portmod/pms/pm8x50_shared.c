/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#include <soc/portmod/portmod.h>
#include <soc/drv.h>

#if defined(PORTMOD_PM8X50_SUPPORT) || defined (PORTMOD_PM8X50_FABRIC_SUPPORT)

#include <soc/portmod/pm8x50_shared.h>

/*
 * Get the VCO rates from given PCS bypassed port speed
 */

int pm8x50_shared_pcs_bypassed_vco_get(int speed, portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    switch (speed) {
        case 56250:
        case 28125:
            *vco = portmodVCO28P125G;
            break;
        case 53125:
            *vco = portmodVCO26P562G;
            break;
        case 27343:
            *vco = portmodVCO27P343G;
            break;
        case 25781:
            *vco = portmodVCO25P781G;
            break;
        case 25000:
            *vco = portmodVCO25G;
            break;
        case 12500:
            *vco = portmodVCO25G;
            break;
        case 20625:
        case 10312:
            *vco = portmodVCO20P625G;
            break;
        default:
            *vco = portmodVCOInvalid;
            break;
    }

    SOC_FUNC_RETURN;
}

#endif /* #if defined(PORTMOD_PM8X50_SUPPORT) || defined (PORTMOD_PM8X50_FABRIC_SUPPORT) */
