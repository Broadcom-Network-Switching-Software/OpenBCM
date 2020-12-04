/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     BCM layer function driver
 * Notes:	Mostly empty for Hercules (does not have L2 table)
 */

#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>

int
bcm_hercules_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_l2_addr_get(int unit, sal_mac_addr_t mac,
			 bcm_vlan_t vid, bcm_l2_addr_t *l2addr)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *	bcm_hercules_l2_init
 * Description:
 *	Initialize chip-dependent parts of L2 module
 * Parameters:
 *	unit        - StrataSwitch unit number.
 */

int
bcm_hercules_l2_init(int unit)
{
    COMPILER_REFERENCE(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_l2_term
 * Description:
 *	Finalize chip-dependent parts of L2 module
 * Parameters:
 *	unit        - StrataSwitch unit number.
 */

int
bcm_hercules_l2_term(int unit)
{
    COMPILER_REFERENCE(unit);

    return SOC_E_NONE;
}

int
bcm_hercules_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                          bcm_l2_addr_t *cf_array, int cf_max,
                          int *cf_count)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(addr);
    COMPILER_REFERENCE(cf_array);
    COMPILER_REFERENCE(cf_max);
    COMPILER_REFERENCE(cf_count);

    return BCM_E_UNAVAIL;
}

