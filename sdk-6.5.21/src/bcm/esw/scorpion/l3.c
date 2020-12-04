/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     Scorpion L3 functions implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_SCORPION_SUPPORT) && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/scorpion.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>

/*                                                                                                
 * Function:                                                                                      
 *      _bcm_sc_defip_init                                                                  
 * Purpose:                                                                                       
 *      Initialize L3 DEFIP table for scorpion devices.                                                
 * Parameters:                                                                                    
 *      unit    - (IN)  SOC unit number.                                                          
 * Returns:                                                                                       
 *      BCM_E_XXX
 */                                                                                               
int                                                                                               
_bcm_sc_defip_init(int unit)
{
     /* Initialize  prefixes offsets, hash/avl  */
     /*   - lookup engine.                      */
     BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));

     /* Initialize IPv6 greater than 64 bit prefixes offsets */
     /* hash lookup engine.                                  */
     BCM_IF_ERROR_RETURN(_bcm_trx_defip_128_init(unit));
    
     return (BCM_E_NONE);
}                                         

/*                                                                                                
 * Function:                                                                                      
 *      _bcm_sc_defip_deinit 
 * Purpose:                                                                                       
 *      De-initialize L3 DEFIP table for scorpion devices.                                                
 * Parameters:                                                                                    
 *      unit    - (IN)  SOC unit number.                                                          
 * Returns:                                                                                       
 *      BCM_E_XXX
 */                                                                                               
int                                                                                               
_bcm_sc_defip_deinit(int unit)
{
     /* De-initialize  prefixes offsets, hash/avl  */
     /*   - lookup engine.                      */
     BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));

     /* De-initialize IPv6 greater than 64 bit prefixes offsets */
     /* hash lookup engine.                                  */
     BCM_IF_ERROR_RETURN(_bcm_trx_defip_128_deinit(unit));
    
     return (BCM_E_NONE);
}                                         
#else /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_scorpion_l3_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_SCORPION_SUPPORT && INCLUDE_L3 */
