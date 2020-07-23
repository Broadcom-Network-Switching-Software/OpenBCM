/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>

#include <soc/mem.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm/rate.h>
#include <bcm/error.h>
/*
* Function:
*      bcm_rate_limit_t_init
* Purpose:
*      Initialize the bcm_rate_limit_t structure
* Parameters:
*      rate_limit - Pointer to structure which should be initialized
* Returns:
*      NONE
*/
void 
bcm_rate_limit_t_init(bcm_rate_limit_t *rate_limit)
{ 
    if (NULL != rate_limit) {
        sal_memset(rate_limit, 0, sizeof (*rate_limit));
    }
    return;
}

/*
* Function:
*      bcm_rate_packet_t_init
* Purpose:
*      Initialize the bcm_rate_packet_t structure
* Parameters:
*      rate_limit - Pointer to structure which should be initialized
* Returns:
*      NONE
*/
void 
bcm_rate_packet_t_init(bcm_rate_packet_t *pkt_rate)
{ 
    if (NULL != pkt_rate) {
        sal_memset(pkt_rate, 0, sizeof (*pkt_rate));
        pkt_rate->kbits_burst = -1;
    }
    return;
}


