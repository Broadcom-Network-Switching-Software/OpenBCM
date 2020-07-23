/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mcast.c
 * Purpose:     Tracks and manages L2 Multicast tables.
 */

#include <soc/defs.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/l2.h>

/*
 * Function:
 *      bcm_humv_mcast_init
 * Purpose:
 *      Initialize multicast api components
 * Returns:
 *      BCM_E_XXX on error
 *      number of mcast entries supported on success
 */

int
bcm_humv_mcast_init(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        uint32 mc_ctrl;
        soc_control_t       *soc = SOC_CONTROL(unit);

        /* Recover the mcast size settings */
        SOC_IF_ERROR_RETURN
            (READ_MC_CONTROL_1r(unit, &mc_ctrl));
        soc->higig2_bcast_size = 
            soc_reg_field_get(unit, MC_CONTROL_1r, mc_ctrl,
                              HIGIG2_BC_SIZEf);

        SOC_IF_ERROR_RETURN
            (READ_MC_CONTROL_2r(unit, &mc_ctrl));
        soc->higig2_mcast_size = 
            soc_reg_field_get(unit, MC_CONTROL_2r, mc_ctrl,
                              HIGIG2_L2MC_SIZEf);

        SOC_IF_ERROR_RETURN
            (READ_MC_CONTROL_3r(unit, &mc_ctrl));
        soc->higig2_ipmc_size = 
            soc_reg_field_get(unit, MC_CONTROL_3r, mc_ctrl,
                              HIGIG2_IPMC_SIZEf);

        SOC_IF_ERROR_RETURN
            (READ_MC_CONTROL_5r(unit, &mc_ctrl));
        soc->mcast_size = 
            soc_reg_field_get(unit, MC_CONTROL_5r, mc_ctrl,
                              SHARED_TABLE_L2MC_SIZEf);
        soc->ipmc_size = 
            soc_reg_field_get(unit, MC_CONTROL_5r, mc_ctrl,
                              SHARED_TABLE_IPMC_SIZEf);


    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Other initialization relocated to soc_misc_init */
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_humv_mcast_detach
 * Purpose:
 *      De-initialize multicast api components
 * Returns:
 *      BCM_E_XXX on error
 *      number of mcast entries supported on success
 */

int
_bcm_humv_mcast_detach(int unit)
{
    return BCM_E_NONE;
}
#else /* BCM_BRADLEY_SUPPORT */
typedef int _humv_mcast_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_BRADLEY_SUPPORT */
