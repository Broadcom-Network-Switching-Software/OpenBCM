/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Cancun load functions
 *
 * cancun files are required to be loaded TD3 onwards.
 */
#include<soc/defs.h>

#if defined(BCM_ESW_SUPPORT) && defined(CANCUN_SUPPORT)

#include<soc/esw/cancun.h>

int
soc_cancun_pre_misc_init_load(int unit)
{
#if !defined(NO_SAL_APPL)
    if (soc_feature(unit, soc_feature_cancun)) {
        if (soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CMH) < 0) {
            return (-1);
        }
        if (soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CCH) < 0) {
            return (-1);
        }
        if (soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CEH) < 0) {
            return (-1);
        }
    }
#endif

    return 0;
}

int
soc_cancun_post_misc_init_load(int unit)
{
#if !defined(NO_SAL_APPL)
    if (soc_feature(unit, soc_feature_cancun)) {
        if (soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CIH) < 0) {
            return (-1);
        }
        if (soc_feature(unit, soc_feature_flex_flow)) {
            if (soc_cancun_generic_load(unit, CANCUN_SOC_FILE_TYPE_CFH) < 0) {
                return (-1);
            }
        }
    }
#endif

    return 0;
}

#else /* !BCM_ESW_SUPPORT && !CANCUN_SUPPORT */
int _test_cancun_no_empty;
#endif /* BCM_ESW_SUPPORT && CANCUN_SUPPORT */
