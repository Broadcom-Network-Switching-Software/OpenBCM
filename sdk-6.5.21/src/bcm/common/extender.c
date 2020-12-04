/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * extender structure initializers
 */
#if defined(INCLUDE_L3)

#include <sal/core/libc.h>
 
#include <bcm/extender.h>
#include <bcm/stat.h>

/* 
 * Function:
 *      bcm_extender_port_t_init
 * Purpose:
 *      Initialize the extender port struct
 * Parameters: 
 *      extender_port - Pointer to the struct to be init'ed
 */
void
bcm_extender_port_t_init(bcm_extender_port_t *extender_port)
{   
    if (extender_port != NULL) {
        sal_memset(extender_port, 0, sizeof(*extender_port));
        extender_port->pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
        extender_port->inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        extender_port->outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
    }
    return;
}

/* 
 * Function:
 *      bcm_extender_forward_t_init
 * Purpose:
 *      Initialize the extender forwarding entry struct
 * Parameters: 
 *      extender_forward_entry - Pointer to the struct to be init'ed
 */
void
bcm_extender_forward_t_init(bcm_extender_forward_t *extender_forward_entry)
{   
    if (extender_forward_entry != NULL) {
        sal_memset(extender_forward_entry, 0, sizeof(*extender_forward_entry));
    }
    return;
}

/* 
 * Function:
 *      bcm_extender_encap_t_init
 * Purpose:
 *      Initialize the extender encapsulation entry struct
 * Parameters: 
 *      extender_encap - Pointer to the struct to be init'ed
 */
void
bcm_extender_encap_t_init(bcm_extender_encap_t *extender_encap)
{   
    if (extender_encap != NULL) {
        sal_memset(extender_encap, 0, sizeof(*extender_encap));
    }
    return;
}

/*
 * Function:
 *      bcm_extender_egress_t_init
 * Purpose:
 *      Initialize the EXTENDER egress object struct
 * Parameters:
 *      extender_egress - Pointer to the struct to be init'ed
 */
void
bcm_extender_egress_t_init(bcm_extender_egress_t *extender_egress)
{
    if (extender_egress != NULL) {
        sal_memset(extender_egress, 0, sizeof(*extender_egress));
        extender_egress->pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
    }
    return;
}

#else
typedef int _bcm_extender_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

