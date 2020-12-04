/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        qos.c
 * Purpose:     Katana2 QoS functions
 */

#include <soc/mem.h>
#include <bcm/error.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/katana2.h>

#ifdef BCM_KATANA2_SUPPORT

#undef DSCP_CODE_POINT_CNT
#define DSCP_CODE_POINT_CNT 64
#undef DSCP_CODE_POINT_MAX
#define DSCP_CODE_POINT_MAX (DSCP_CODE_POINT_CNT - 1)
#define _BCM_QOS_MAP_CHUNK_DSCP  64

 /* Function:
 *      bcm_kt2_port_dscp_map_set
 * Purpose:
 *      Internal implementation for bcm_kt2_port_dscp_map_set
 * Parameters:
 *      unit - switch device
 *      port - switch port or -1 for global table
 *      srccp - src code point or -1
 *      mapcp - mapped code point or -1
 *      prio - priority value for mapped code point
 *              -1 to use port default untagged priority
 *              BCM_PRIO_RED    can be or'ed into the priority
 *              BCM_PRIO_YELLOW can be or'ed into the priority
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_kt2_port_dscp_map_set(int unit, bcm_port_t port, int srccp,
                            int mapcp, int prio, int cng)
{
    port_tab_entry_t         pent;
    void                     *entries[2];
    uint32                   old_profile_index = 0;
    int                      index = 0;
    int                      rv = BCM_E_NONE;
    dscp_table_entry_t       dscp_table[_BCM_QOS_MAP_CHUNK_DSCP];
    int                      offset = 0,i = 0;
    void                     *entry;

    /* Lock the port table */
    soc_mem_lock(unit, PORT_TABm); 

    /* Get profile index from port table. */
    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, port, &pent);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_TABm);
        return rv;
    }

    old_profile_index =
        soc_mem_field32_get(unit, PORT_TABm, &pent, TRUST_DSCP_PTRf)
            * DSCP_CODE_POINT_CNT;

    sal_memset(dscp_table, 0, sizeof(dscp_table));

    /* Base index of table in hardware */

    entries[0] = &dscp_table;

    rv =  _bcm_dscp_table_entry_get(unit, old_profile_index,
            _BCM_QOS_MAP_CHUNK_DSCP, entries);
    offset = srccp;
    /* Modify what's needed */

    if (offset < 0) {
        for (i = 0; i <= DSCP_CODE_POINT_MAX; i++) {
             entry = &dscp_table[i];
             soc_DSCP_TABLEm_field32_set(unit, entry, DSCPf, mapcp);
             soc_DSCP_TABLEm_field32_set(unit, entry, PRIf, prio);
             soc_DSCP_TABLEm_field32_set(unit, entry, CNGf, cng);
        }
    } else {
        entry = &dscp_table[offset];
        soc_DSCP_TABLEm_field32_set(unit, entry, DSCPf, mapcp);
        soc_DSCP_TABLEm_field32_set(unit, entry, PRIf, prio);
        soc_DSCP_TABLEm_field32_set(unit, entry, CNGf, cng);
    }

    /* Add new chunk and store new HW index */

    rv = _bcm_dscp_table_entry_add(unit, entries, _BCM_QOS_MAP_CHUNK_DSCP, 
                                           (uint32 *)&index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_TABm);
        return rv;
    }

    if (index != old_profile_index) {
        soc_mem_field32_set(unit, PORT_TABm, &pent, TRUST_DSCP_PTRf,
                            index / _BCM_QOS_MAP_CHUNK_DSCP);
        rv = soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, port,
                            &pent);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_TABm);
            return rv;
        }
    }

    rv = _bcm_dscp_table_entry_delete(unit, old_profile_index);
    /* Release port table lock */
    soc_mem_unlock(unit, PORT_TABm);
    return rv;
}

#endif /* BCM_KATANA2_SUPPORT */

