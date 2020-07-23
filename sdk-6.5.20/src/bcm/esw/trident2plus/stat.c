/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port driver.
 */

#include <soc/defs.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/td2_td2p.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm/error.h>
#include <bcm_int/esw/stat.h>

/*
 * Function:
 *  bcmi_td2p_stat_port_attach
 * Description:
 *  Initializes the BCM stat module for the new port that has been flexed
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 *  port - port number
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_INTERNAL - Chip access failure.
 */

int
bcmi_td2p_stat_port_attach(int unit, bcm_port_t port)
{
    pbmp_t      pbmp;
    int         config_threshold;

    config_threshold = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if ((config_threshold < 1518) ||
        (config_threshold > 0x3fff) ) {
        config_threshold = 1518;
    }

    BCM_IF_ERROR_RETURN    
        (_bcm_esw_stat_ovr_threshold_set(unit, port, config_threshold));

    /* Adding the new port to bitmap and passing it to 
       counter_set32_by_port as it takes only bitmap as argument */
       SOC_PBMP_CLEAR(pbmp);
       SOC_PBMP_PORT_ADD(pbmp, port);
       SOC_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbmp, 0));
       
    /* Note: Assuming soc_counter_start would be called later after
             all the modules for the new port are initialised */
    

   return BCM_E_NONE;
}
/*
 * Function:
 *  bcmi_td2p_stat_port_detach
 * Description:
 *  Clear the port based statistics from the StrataSwitch port.
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 *  port - port number
 * Returns:
 *  BCM_E_NONE - Success.
 */

int
bcmi_td2p_stat_port_detach(int unit, bcm_port_t port)
{
    pbmp_t      pbm;
    
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    BCM_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbm, 0));
    
    return(BCM_E_NONE);
    
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
