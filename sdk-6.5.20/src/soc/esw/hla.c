/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/defs.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_HLA_SUPPORT)
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/esw/hla.h>
#include <shared/hla/hla.h>

int
soc_hla_license_load(int unit, const uint8 *data, int len, uint32 *status)
{
    hla_status_t hla_status;

    /* Check feature */
    if (!soc_feature(unit, soc_feature_hla)) {
        LOG_CLI((BSL_META_U(unit, "HLA Licensing not supported...\n")));
        return SOC_E_NONE;
    }

    /* Skip when warmboot is in progress */
    if (SOC_WARM_BOOT(unit)) {
        LOG_CLI((BSL_META_U(unit, "Skipped License file loading...\n")));
        return SOC_E_NONE;
    }

    (void) soc_hla_ccf_load(unit, data, len);

    sal_memset(&hla_status, 0, sizeof(hla_status));
    (void) soc_hla_status(unit, &hla_status);

    *status = hla_status.error;

    return SOC_E_NONE;
}

#endif /* BCM_ESW_SUPPORT && BCM_HLA_SUPPORT */
