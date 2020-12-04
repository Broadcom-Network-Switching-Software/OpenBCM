/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <bcm/module.h>
#include <bcm/init.h>
#include <bcm/error.h>

#include <soc/mem.h>
#include <soc/drv.h>

/* Warm boot state. */
static int bcm_wb_state[BCM_MAX_NUM_UNITS] = {0};

/* Protection mutexes for each unit. */
sal_mutex_t _bcm_lock[BCM_MAX_NUM_UNITS];

int inside_bcm_shutdown[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *    bcm_module_name
 * Purpose:
 *    Return the name of a module given its number
 * Parameters:
 *    unit - placeholder
 *    module_num - One of BCM_MODULE_xxx
 * Returns:
 *    Pointer to static char string
 * Notes:
 *    This function to be moved to its own module when it makes sense.
 */

STATIC char *_bcm_module_names[] = BCM_MODULE_NAMES_INITIALIZER;

char *
bcm_module_name(int unit, int module_num)
{
    if (sizeof(_bcm_module_names) / sizeof(_bcm_module_names[0])
                                                    != BCM_MODULE__COUNT) {
        int i;

        i = sizeof(_bcm_module_names) / sizeof(_bcm_module_names[0]) - 1;

        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "bcm_module_name: BCM_MODULE_NAMES_INITIALIZER(%d) and BCM_MODULE__COUNT(%d) mis-match\n"), i, BCM_MODULE__COUNT));
        for(;i >= 0;i--) {
            LOG_ERROR(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "%2d. %s\n"), i, _bcm_module_names[i]));
        }
    }
    if (module_num < 0 || module_num >= BCM_MODULE__COUNT) {
        return "unknown";
    }

    return _bcm_module_names[module_num];
}

/*
 * Function:
 *      bcm_info_t_init
 * Purpose:
 *      Initialize the bcm_info_t structure.
 * Parameters:
 *      info - Pointer to bcm_info_t structure.
 * Returns:
 *      NONE
 */
void
bcm_info_t_init(bcm_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof (*info));
    }
    return;
}

/*
 * Function:
 *      bcm_attach_info_t_init
 * Purpose:
 *      Initialize the bcm_attach_info_t structure.
 * Parameters:
 *      info - (IN/OUT) Pointer to structure to initialize.
 * Returns:
 *      NONE
 */
void
bcm_attach_info_t_init(bcm_attach_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof (*info));
    }
    return;
}

/*
 * Function:
 *      bcm_detach_retry_t_init
 * Purpose:
 *      Initialize the bcm_detach_retry_t structure.
 * Parameters:
 *      retry - Pointer to bcm_detach_retry_t structure.
 * Returns:
 *      NONE
 */
void
bcm_detach_retry_t_init(bcm_detach_retry_t *retry)
{
    if (retry != NULL) {
        sal_memset(retry, 0, sizeof (*retry));
    }
    return;
}

/*
 * Function:
 *      bcm_warmboot_set
 * Purpose:
 *      Set warm boot state
 * Parameters:
 *      unit - Unit number
 *      warmboot - Warmboot state: !0 for warmboot, 0 for cold boot.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_warmboot_set(int unit, int warmboot)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        return BCM_E_UNIT;
    }

    bcm_wb_state[unit] = warmboot ? TRUE : FALSE;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

/*
 * Function:
 *      bcmi_warmboot_get
 * Purpose:
 *      Get warm boot state
 * Parameters:
 *      unit - Unit number
 * Returns:
 *      warmboot - Warmboot state: !0 for warmboot, 0 for cold boot.
 */
int
bcmi_warmboot_get(int unit)
{
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        return FALSE;
    } else {
        return bcm_wb_state[unit];
    }
}

/*
 * Function:
 *      bcm_warmboot_get
 * Purpose:
 *      Get warm boot state
 * Parameters:
 *      unit - Unit number
 *      warmboot - Warmboot state: !0 for warmboot, 0 for cold boot.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_warmboot_get(int unit, int *warmboot)
{
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        return BCM_E_UNIT;
    }

    *warmboot = bcmi_warmboot_get(unit);

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_shutdown
 * Purpose:
 *      Free up resources without touching hardware
 * Parameters:
 *      unit    - switch device
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_shutdown(int unit)
{
    int warm_boot;
    int rv;

    /* Since this API is used for warm-boot, we need to enable
       warm boot here even if it hasn't been already, and restore
       it afterward if necessary. */

    warm_boot = SOC_WARM_BOOT(unit);

    if (!warm_boot)
    {
        SOC_WARM_BOOT_START(unit);
        bcm_warmboot_set(unit, 1);
    }

    inside_bcm_shutdown[unit] = TRUE;

    rv = bcm_detach(unit);

    if (!warm_boot)
    {
        SOC_WARM_BOOT_DONE(unit);
        bcm_warmboot_set(unit, 0);
    }

    inside_bcm_shutdown[unit] = FALSE;

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
