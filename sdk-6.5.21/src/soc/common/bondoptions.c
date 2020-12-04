/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for Bond Options Init & De-Init
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/bondoptions.h>

soc_bond_info_t *soc_bond_info[SOC_MAX_NUM_DEVICES];



/*
 * Function:
 *      soc_bond_info_init
 * Purpose:
 *      Initialize SOC bond options information cache.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_bond_info_init(int unit)
{
    int rv = SOC_E_NONE;
    uint16 dev_id, driver_dev_id, driver_rev_id;
    uint8 rev_id;


    if (SOC_BOND_INFO(unit) == NULL) {
        SOC_BOND_INFO(unit) = sal_alloc(sizeof(soc_bond_info_t), "SOC Bond Info Structure");
        if (SOC_BOND_INFO(unit) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(SOC_BOND_INFO(unit), 0, (sizeof(soc_bond_info_t)));

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    /* Call bond_options_init function only if defined */
    if (SOC_FUNCTIONS(unit)->soc_bond_options_init) {
        rv = SOC_FUNCTIONS(unit)->soc_bond_options_init(unit);
    }

    return rv;
}


/*
 * Function:
 *      soc_bond_info_deinit
 * Purpose:
 *      UnInitialize/Deallocate SOC bond options information cache.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_bond_info_deinit(int unit)
{
    sal_free(SOC_BOND_INFO(unit));
    SOC_BOND_INFO(unit) = NULL;

    return SOC_E_NONE;
}

#else
typedef int _soc_common_bondoptionc_c_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */

