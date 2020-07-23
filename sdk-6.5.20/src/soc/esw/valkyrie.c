/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        valkyrie.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>

#include <soc/triumph.h>
#include <soc/valkyrie.h>
#include <soc/drv.h>

#ifdef BCM_VALKYRIE_SUPPORT

/*
 * Valkyrie chip driver functions.  
 */
soc_functions_t soc_valkyrie_drv_funs = {
    soc_triumph_misc_init,
    soc_triumph_mmu_init,
    soc_triumph_age_timer_get,
    soc_triumph_age_timer_max_get,
    soc_triumph_age_timer_set,
};
#endif /* BCM_VALKYRIE_SUPPORT */
