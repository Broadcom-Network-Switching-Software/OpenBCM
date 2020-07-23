/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef INCLUDE_L3

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/ipmc.h>
#include <bcm/stat.h>
#include <bcm/vlan.h>
#include <bcm/debug.h>

/*
 * Function:
 *	bcm_ipmc_addr_t_init
 * Description:
 *	Initialize a bcm_ipmc_addr_t to a specified source IP address,
 *	destination IP address and VLAN, while zeroing all other fields.
 * Parameters:
 *	data - Pointer to bcm_ipmc_addr_t.
 * Returns:
 *	Nothing.
 */

void
bcm_ipmc_addr_t_init(bcm_ipmc_addr_t *data)
{
    if (NULL != data) {
        sal_memset(data, 0, sizeof(bcm_ipmc_addr_t));
        data->rp_id = BCM_IPMC_RP_ID_INVALID;
        data->ing_intf = BCM_IF_INVALID;
    }
}

/*
 * Function:
 *      bcm_ipmc_counters_t_init
 * Description:
 *      Initialize a ipmc counters object struct.
 * Parameters:
 *      ipmc_counter - Pointer to IPMC counters object struct.
 * Returns:
 *      Nothing.
 */
void
bcm_ipmc_counters_t_init(bcm_ipmc_counters_t *ipmc_counter)
{
    if (ipmc_counter != NULL) {
        sal_memset(ipmc_counter, 0, sizeof(*ipmc_counter));
    }
    return;
}

/*
 * Function:
 *	bcm_ipmc_range_t_init
 * Description:
 *	Initialize a bcm_ipmc_range_t to all zeroes.
 * Parameters:
 *	data - Pointer to bcm_ipmc_range_t.
 * Returns:
 *	Nothing.
 */
void
bcm_ipmc_range_t_init(bcm_ipmc_range_t *range)
{
    if (NULL != range) {
        sal_memset(range, 0, sizeof(bcm_ipmc_range_t));
    }
}

#else 
typedef int _bcm_ipmc_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

