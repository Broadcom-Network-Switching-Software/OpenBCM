/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Manages untagged priority setting for port
 */

#include <shared/bsl.h>

#include <assert.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/xgs3.h>


/*
 * Function:
 *      bcm_port_untagged_priority_set
 * Purpose:
 *      Set the 802.1P priority for untagged packets coming in on a
 *      port.  This value will be written into the priority field of the
 *      tag that is added at the ingress.
 * Parameters:
 *      unit      - StrataSwitch Unit #.
 *      port      - StrataSwitch port #.
 *      priority  - Priority to be set in 802.1p priority tag, from 0 to 7.
 *                  A negative priority leaves the ingress port priority as
 *                  is, but disables it from overriding ARL-based priorities.
 *                  (on those devices that support ARL-based priority).
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_shadow_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    port_tab_entry_t ptab_entry;   /* Port table entry.             */
    int rv = BCM_E_NONE;

    if (priority > 7 || priority < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    soc_mem_lock(unit, PORT_TABm);

    rv = BCM_XGS3_MEM_READ(unit, PORT_TABm, port, &ptab_entry);
    if (BCM_FAILURE(rv)) {
       soc_mem_unlock(unit, PORT_TABm);
       return (rv);
    }

    soc_mem_field32_set(unit, PORT_TABm, &ptab_entry, PORT_INT_PRIf, 
                    priority);
    rv = BCM_XGS3_MEM_WRITE(unit, PORT_TABm, port, &ptab_entry);
    if (BCM_FAILURE(rv)) {
       soc_mem_unlock(unit, PORT_TABm);
       return (rv);
    }

    soc_mem_unlock(unit, PORT_TABm);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_shadow_port_untagged_priority_set: u=%d p=%d pri=%d rv=%d\n"),
              unit, port, priority, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_untagged_priority_get
 * Purpose:
 *      Returns priority being assigned to untagged receive packets
 * Parameters:
 *      unit      - StrataSwitch Unit #.
 *      port      - StrataSwitch port #.
 *      priority  - Pointer to an int in which priority value is returned.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_shadow_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
    port_tab_entry_t ptab_entry;   /* Port table entry.             */
    int rv = BCM_E_NONE;

    if (priority != NULL) {
        soc_mem_lock(unit, PORT_TABm);
        rv = BCM_XGS3_MEM_READ(unit, PORT_TABm, port, &ptab_entry);
        if (BCM_FAILURE(rv)) {
           soc_mem_unlock(unit, PORT_TABm);
           return (rv);
        }
        soc_mem_unlock(unit, PORT_TABm);

        *priority = soc_mem_field32_get(unit, PORT_TABm, &ptab_entry, PORT_INT_PRIf);
        LOG_INFO(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "bcm_shadow_port_untagged_priority_get: u=%d p=%d pri=%d\n"),
                  unit, port, *priority));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_shadow_port_dscp_map_set
 * Purpose:
 *      Internal implementation for bcm_port_dscp_map_set
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

STATIC int
_bcm_shadow_port_dscp_map_set(int unit, bcm_port_t port,
                       int srccp, int mapcp, int prio)
{
    int                  i, cng;
    dscp_table_entry_t   de;
    int                  max_index;   
    int                  min_index;

#define DSCP_CODE_POINT_CNT 256  /* for IPV6 */
#define DSCP_CODE_POINT_MAX (DSCP_CODE_POINT_CNT - 1)

    if (mapcp < -1 || mapcp > DSCP_CODE_POINT_MAX) {
        return BCM_E_PARAM;
    }


    if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX) {
        return BCM_E_PARAM;
    }
    /* Extract cng bits and check for valid priority. */
    
    cng = 0; /* Green */
    if (prio < 0) {
        return BCM_E_PARAM;
    }
    if (prio & BCM_PRIO_RED) {
        cng = 0x01;  /* Red */
        prio &= ~BCM_PRIO_RED;
    } else if (prio & BCM_PRIO_YELLOW) {
        cng = 0x03;  /* Yellow  */
        prio &= ~BCM_PRIO_YELLOW;
    }
    if ((prio & ~BCM_PRIO_MASK) != 0) {
        return BCM_E_PARAM;
    }
    if (srccp < 0 && mapcp < 0) {
        /* No mapping */
        return BCM_E_NONE;
    } else if (srccp < 0) {
        /* fill all entries in DSCP_TABLEm with mapcp */
        sal_memset(&de, 0, sizeof(de));
        soc_DSCP_TABLEm_field32_set(unit, &de, PRIf, prio);
        soc_DSCP_TABLEm_field32_set(unit, &de, CNGf, cng);
        min_index = port * 512;
        max_index = min_index + DSCP_CODE_POINT_MAX;
        for (i = min_index; i < max_index; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_DSCP_TABLEm(unit, MEM_BLOCK_ALL, i, &de));
        }
    } else {
        sal_memset(&de, 0, sizeof(de));
        /* fill specific srccp entry/entries in DSCP_TABLEm with mapcp */
        soc_DSCP_TABLEm_field32_set(unit, &de, PRIf, prio);
        soc_DSCP_TABLEm_field32_set(unit, &de, CNGf, cng);
        /* The table storage is, 256 for IPv4 + 256 for IPv6 per port */
        if (srccp < 64) {
            SOC_IF_ERROR_RETURN
              (WRITE_DSCP_TABLEm(unit, MEM_BLOCK_ALL, 
                                 (port * 512) + (srccp << 2), &de));  /* IPV4 */
        }
        SOC_IF_ERROR_RETURN
          (WRITE_DSCP_TABLEm(unit, MEM_BLOCK_ALL, 
                             (port * 512) + 256 + srccp, &de)); /* IPV6 */
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_dscp_map_set
 * Purpose:
 *      Define a mapping for diffserv code points
 *      The mapping enable/disable is controlled by a seperate API
 *      bcm_port_dscp_map_enable_set/get
 *      Also Enable/Disable control for DSCP mapping for tunnels
 *      are available in the respective tunnel create APIs.
 * Parameters:
 *      unit - switch device
 *      port - switch port      or -1 to setup global mapping table.
 *      srccp - src code point or -1
 *      mapcp - mapped code point or -1
 *      prio - priority value for mapped code point
 *              -1 to use port default untagged priority
 *              BCM_PRIO_DROP_FIRST can be or'ed into the priority
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      On most xgs switches there are a limited set
 *      of mappings that are possible:
 *              srccp -1, mapcp -1:     no mapping
 *              srccp -1, mapcp 0..63:  map all packets
 *              srccp 0, mapcp 0..63:   map packets with cp 0
 *      On Firebolt/Helix/Felix, there is no per port mapping table.
 *      Only a global mapping table is available.
 */

int
bcm_shadow_port_dscp_map_set(int unit, bcm_port_t port, int srccp, int mapcp,
                             int prio)
{
    int rv = BCM_E_NONE;

    if (port != -1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    soc_mem_lock(unit, PORT_TABm);

    if (port < 0) {
        for (port = 0; port <= 16; port++) {  /* for all ports */
            rv = _bcm_shadow_port_dscp_map_set(unit, port, srccp, mapcp, prio);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, PORT_TABm);
                return rv;
            }
        }
    } else {
        if ((port >= 1) && (port <= 16)) { /*Specific port */
            rv = _bcm_shadow_port_dscp_map_set(unit, port, srccp, mapcp, prio);
        } else {
            rv = BCM_E_PORT;
        }
    }
    soc_mem_unlock(unit, PORT_TABm);

    return rv;
}

/*
 * Function:
 *      _bcm_shadow_port_dscp_map_get
 * Purpose:
 *      Get a mapping for diffserv code points
 * Parameters:
 *      unit - switch device
 *      port - switch port
 *      srccp - src code point or -1
 *      mapcp - (OUT) pointer to returned mapped code point
 *      prio - (OUT) Priority value for mapped code point or -1
 *                      May have BCM_PRIO_DROP_FIRST or'ed into it
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_shadow_port_dscp_map_get(int unit, bcm_port_t port, int srccp, int *mapcp,
                      int *prio)
{
    int                     base;
    dscp_table_entry_t      de;

    if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX || 
        mapcp == NULL || prio == NULL) {
        return BCM_E_PARAM;
    }

    /* look up in DSCP_TABLEm */
    if (srccp < 0) {
        srccp = 0;
    }
    base = (port * 512) + 256; /* Always retrive IPV6 settings */
    /* IPv4 and IPv6 are both set to same value */
    SOC_IF_ERROR_RETURN
        (READ_DSCP_TABLEm(unit, MEM_BLOCK_ANY, base + srccp, &de));
    *prio = soc_DSCP_TABLEm_field32_get(unit, &de, PRIf);
    if (soc_DSCP_TABLEm_field32_get(unit, &de, CNGf)) {
        
        *prio |= BCM_PRIO_DROP_FIRST;
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_port_dscp_map_get
 * Purpose:
 *      Get a mapping for diffserv code points
 * Parameters:
 *      unit - switch device
 *      port - switch port
 *      srccp - src code point or -1
 *      mapcp - (OUT) pointer to returned mapped code point
 *      prio - (OUT) Priority value for mapped code point or -1
 *                      May have BCM_PRIO_DROP_FIRST or'ed into it
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_shadow_port_dscp_map_get(int unit, bcm_port_t port, int srccp, int *mapcp,
                      int *prio)
{
    int rv;

    if (port != -1) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    }

    soc_mem_lock(unit, PORT_TABm);

    /* Get device port configuration. */
    if ((port >= 1) && (port <=16)) {
        rv = _bcm_shadow_port_dscp_map_get(unit, port, srccp, mapcp, prio);
    } else {
        rv = BCM_E_PORT;
    }

    soc_mem_unlock(unit, PORT_TABm);
    return rv;
}


/*
 * Function:
 *      bcm_shadow_port_vlan_priority_map_set
 * Description:
 *      Define the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_shadow_port_vlan_priority_map_set(int unit, bcm_port_t port, int pkt_pri,
                                   int cfi, int internal_pri, bcm_color_t color)
{
    int                      index;
    ing_pri_cng_map_entry_t  pri_map;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (pkt_pri < 0 || pkt_pri > 7 || cfi < 0 || cfi > 1 || internal_pri < 0 ||
        internal_pri >=
        (1 << soc_mem_field_length(unit, ING_PRI_CNG_MAPm, PRIf))) {
        return BCM_E_PARAM;
    }

    /* ING_PRI_CNG_MAP table is indexed with
     * port[0:4] incoming priority[2:0] incoming CFI[0]
     */
    index = (port << 4) | (pkt_pri << 1) | cfi;

    memset(&pri_map, 0, sizeof(pri_map));
    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, PRIf, 
                        internal_pri);
    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf, 
                        _BCM_COLOR_ENCODING(unit, color));
    SOC_IF_ERROR_RETURN
        (WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL, index, &pri_map));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_shadow_port_vlan_priority_map_get
 * Description:
 *      Get the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (OUT) Internal priority
 *      color        - (OUT) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_shadow_port_vlan_priority_map_get(int unit, bcm_port_t port, int pkt_pri,
                                   int cfi, int *internal_pri, 
                                   bcm_color_t *color)
{

    int index, hw_color;
    ing_pri_cng_map_entry_t pri_map;

    /* Input parameters check. */
    if ((NULL == internal_pri) || (NULL == color)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (pkt_pri < 0 || pkt_pri > 7 || cfi < 0 || cfi > 1) {
        return BCM_E_PARAM;
    }

    /* ING_PRI_CNG_MAP table is indexed with
     * port[4:0] incoming priority[2:0] incoming CFI[0]
     */
    index = (port << 4) | (pkt_pri << 1) | cfi;
    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_shadow_port_vlan_priority_map_get: u=%d p=%d index=%d\n"),
              unit, port, index));

    SOC_IF_ERROR_RETURN
        (READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY, index, &pri_map));
    *internal_pri = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &pri_map, 
                                       PRIf);
    hw_color = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf);
    *color = _BCM_COLOR_DECODING(unit, hw_color);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_shadow_port_management_packet_config_set
 * Description:
 *      Set Management packet classification for Ethertype and/or Dest MAC
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      config      -  (IN) Pointer to bcm_port_management_packet_config_t
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs MGMT_FRAME_CTRL register
 */
int
bcm_shadow_port_management_packet_config_set(int unit, bcm_port_t port,
                                   bcm_port_management_packet_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_mac_t   dest_mac;
    soc_reg_t reg[5] = {CTRL_DA1r, CTRL_DA2r, CTRL_DA3r, CTRL_DA4r,
                       CTRL_DA5r};
    soc_field_t field[5] = {CTRL_DA1f, CTRL_DA2f, CTRL_DA3f, CTRL_DA4f,
                       CTRL_DA5f};
    uint64 mac_field;
    bcm_ethertype_t etype;
    uint32 val;
    uint64 val64;
    int enable, i, p, empty = 0;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    if (port < 0 || port > 16) {
        return BCM_E_PARAM;
    }
    soc_mem_lock(unit, PORT_TABm);
    if ((config->flags & 
                    (BCM_PORT_MANAGEMENT_PACKET_DEST_MAC |
                     BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE)) ==
                    (BCM_PORT_MANAGEMENT_PACKET_DEST_MAC |
                     BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE)) {
        if (config->flags & BCM_PORT_MANAGEMENT_PACKET_CLEAR) { 
            /* Clear the config */
            rv = READ_CTRL_ETHERTYPE2r(unit, &val);
            etype = soc_reg_field_get(unit, CTRL_ETHERTYPE2r, val, 
                                      ETHERTYPEf);
            if (BCM_SUCCESS(rv)) {
                rv = soc_reg_get(unit, CTRL_DA6r, port, 0, &mac_field);
                SAL_MAC_ADDR_FROM_UINT64(dest_mac, mac_field);
                if (BCM_SUCCESS(rv)) {
                    if ((ENET_CMP_MACADDR(dest_mac, config->dest_mac) == 0) &&
                                        (etype == config->ethertype)) {
                        rv = (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                           CTRL_ETHERTYPE2f, 0));
                        rv = (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                           CTRL_DA6f, 0));
                        for (i = 0; i <= 16; i++) { 
                            /* check if any other ports are still using ethtype */
                            rv = READ_MGMT_FRAME_ENABLEr(unit, i, &val);
                            if (BCM_SUCCESS(rv)) {
                                enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, CTRL_ETHERTYPE2f);
                                if (enable) {
                                    break;
                                }
                            }
                        }
                        if (BCM_SUCCESS(rv) && i >= 16) { /* No ports using ethertype, clear it */
                            rv = soc_reg_field32_modify(unit, CTRL_ETHERTYPE2r, port, 
                                                    ETHERTYPEf, 0);
                            COMPILER_64_ZERO(val64);
                            rv = soc_reg_set(unit, CTRL_DA6r, port, 0, val64);
                        }
                    } else {
                        rv = BCM_E_NOT_FOUND;
                    }
                }
            }
        } else {
            rv = READ_CTRL_ETHERTYPE2r(unit, &val);
            etype = soc_reg_field_get(unit, CTRL_ETHERTYPE2r, val, 
                                      ETHERTYPEf);
            if (BCM_SUCCESS(rv)) {
                rv = soc_reg_get(unit, CTRL_DA6r, port, 0, &mac_field);
                SAL_MAC_ADDR_FROM_UINT64(dest_mac, mac_field);
                if (BCM_SUCCESS(rv)) {
                   if ((ENET_CMP_MACADDR(dest_mac, config->dest_mac) == 0) &&
                       (etype == config->ethertype)) {
                        rv = (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                           CTRL_ETHERTYPE2f, 1));
                        rv = (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                           CTRL_DA6f, 1));
                    } else if ((etype == 0) && COMPILER_64_IS_ZERO(mac_field)) {
                        rv = (soc_reg_field32_modify(unit, CTRL_ETHERTYPE2r, port, 
                                           ETHERTYPEf, config->ethertype));
                        SAL_MAC_ADDR_TO_UINT64(config->dest_mac, mac_field);
                        rv = soc_reg_set(unit, CTRL_DA6r, port, 0, mac_field);

                        rv = (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                           CTRL_ETHERTYPE2f, 1));
                        rv = (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                           CTRL_DA6f, 1));
                    } else {
                        rv = BCM_E_RESOURCE;
                    }
                }
            }
        }
    } else if ((config->flags & BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE) ==
                         BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE) {
        if (config->flags & BCM_PORT_MANAGEMENT_PACKET_CLEAR) { 
            /* Clear the config */
            rv = READ_CTRL_ETHERTYPE1r(unit, &val);
            etype = soc_reg_field_get(unit, CTRL_ETHERTYPE1r, val, 
                                      ETHERTYPEf);
            if (BCM_SUCCESS(rv)) { 
                if (etype == config->ethertype) {
                    rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                                CTRL_ETHERTYPE1f, 0);
                    for (i = 0; i <= 16; i++) { 
                        /* check if any other ports are still using ethtype */
                        rv = READ_MGMT_FRAME_ENABLEr(unit, i, &val);
                        if (BCM_SUCCESS(rv)) {
                            enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, CTRL_ETHERTYPE1f);
                            if (enable) {
                                break;
                            }
                        }
                    }
                    if (BCM_SUCCESS(rv) && i >= 16) { /* No ports using ethertype, clear it */
                        rv = soc_reg_field32_modify(unit, CTRL_ETHERTYPE1r, port, 
                                                    ETHERTYPEf, 0);
                    }
                } else {
                    rv = BCM_E_NOT_FOUND;
                }
            }
        } else {
            /* Only Ether type is valid */
            rv = READ_MGMT_FRAME_ENABLEr(unit, port, &val);
            if (BCM_SUCCESS(rv)) {
                enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, CTRL_ETHERTYPE1f);
                rv = READ_CTRL_ETHERTYPE1r(unit, &val);
                etype = soc_reg_field_get(unit, CTRL_ETHERTYPE1r, val, ETHERTYPEf);
                if (BCM_SUCCESS(rv)) {
                    if ((etype == config->ethertype)) { 
                        /* Other ports have already added the ether type */
                        rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            CTRL_ETHERTYPE1f, 1);
                    } else if ((etype == 0x0)) {
                        BCM_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, CTRL_ETHERTYPE1r, port, 
                                                ETHERTYPEf, config->ethertype));
                        BCM_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                                CTRL_ETHERTYPE1f, 1));
                    } else {
                        rv = BCM_E_RESOURCE;
                    }
                }
            }
        }
    } else if ((config->flags & BCM_PORT_MANAGEMENT_PACKET_DEST_MAC) ==
                         BCM_PORT_MANAGEMENT_PACKET_DEST_MAC) {
        /* check if the DA is CDP multicast */
        bcm_mac_t neigh_disc_da = {0x33, 0x33, 0x00, 0x00, 0x00, 0x00};
        bcm_mac_t cdp_da = {0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCC};
        bcm_mac_t sstp_da = {0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCD};
        if (config->flags & BCM_PORT_MANAGEMENT_PACKET_CLEAR) { 

            if (ENET_CMP_MACADDR(cdp_da, config->dest_mac) == 0) {
                /* Clear the MGMT Frame classification */
                rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            C_MC_DMAC_Af, 0);
            } else if (ENET_CMP_MACADDR(sstp_da, config->dest_mac) == 0) {
                /* Clear the MGMT Frame classification */
                rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            C_MC_DMAC_Bf, 0);
            } else if (ENET_CMP_MACADDR(neigh_disc_da, config->dest_mac) == 0) {
                /* Clear the MGMT Frame classification */
                rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            NEIGHBOR_DISCf, 0);
            } else  {
                /* Clear the config */
                for (i = 0; i < 5; i++) {
                    rv = soc_reg_get(unit, reg[i], port, 0, &mac_field);
                    SAL_MAC_ADDR_FROM_UINT64(dest_mac, mac_field);
                    if (BCM_SUCCESS(rv) && (ENET_CMP_MACADDR(dest_mac,
                    config->dest_mac) == 0)) {
                        rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                                field[i], 0);
                        for (p = 0; p <= 16; p++) { 
                            /* check if any other ports are still using ethtype */
                            rv = READ_MGMT_FRAME_ENABLEr(unit, p, &val);
                            if (BCM_SUCCESS(rv)) {
                                enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, field[i]);
                                if (enable) {
                                    break;
                                }
                            }
                        }
                        if (BCM_SUCCESS(rv) && i >= 16) { /* No ports using dest-da, clear it */
                            COMPILER_64_ZERO(val64);
                            rv = soc_reg_set(unit, reg[i], port, 0, val64);
                        }
                        break;
                    }
                }
                if (i >= 5) {
                    rv = BCM_E_NOT_FOUND;
                }
            }
        } else {
            if (ENET_CMP_MACADDR(cdp_da, config->dest_mac) == 0) {
                /* Clear the MGMT Frame classification */
                rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            C_MC_DMAC_Af, 1);
            } else if (ENET_CMP_MACADDR(sstp_da, config->dest_mac) == 0) {
                /* Clear the MGMT Frame classification */
                rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            C_MC_DMAC_Bf, 1);
            } else if (ENET_CMP_MACADDR(neigh_disc_da, config->dest_mac) == 0) {
                /* Clear the MGMT Frame classification */
                rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                            NEIGHBOR_DISCf, 1);
            } else {
                empty = -1;
                for (i = 0; i < 5; i++) { /* find a matching Dest-DA */
                    rv = soc_reg_get(unit, reg[i], port, 0, &mac_field);
                    SAL_MAC_ADDR_FROM_UINT64(dest_mac, mac_field);
                    if (BCM_SUCCESS(rv) && (ENET_CMP_MACADDR(dest_mac, config->dest_mac) == 0)) {
                        rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                                    field[i], 1);
                        break;
                    }
                    if (BCM_SUCCESS(rv) && COMPILER_64_IS_ZERO(mac_field)) {
                        empty = i; /* found an empty slot */
                    }
                }

                if (i >= 5 && empty != -1) { /* no match found, but found an empty slot */
                    SAL_MAC_ADDR_TO_UINT64(config->dest_mac, mac_field);
                    rv = soc_reg64_set(unit, reg[empty], port, 0, mac_field);
                    if (BCM_SUCCESS(rv)) {
                        rv = soc_reg_field32_modify(unit, MGMT_FRAME_ENABLEr, port, 
                                                    field[empty], 1);
                    }
                } else if (i >= 5 && empty == -1) {   /* no empty slot found */
                    rv = BCM_E_RESOURCE;
                }
            } 
        }
    } else {
        rv = BCM_E_PARAM;
    }
    soc_mem_unlock(unit, PORT_TABm);

    return rv;
}

/*
 * Function:
 *      bcm_shadow_port_management_packet_config_get
 * Description:
 *      Get Management packet classification for Ethertype and/or Dest MAC
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      config_array -  (OUT) Pointer to bcm_port_management_packet_config_t
 *      max_config   - (IN) Max number of Management Packet classifications requested
 *      config_count  -  (OUT) Actual number of Management Packet classifcations
 * Return Value:
 *      BCM_E_XXX
 * Note:
 */
int
bcm_shadow_port_management_packet_config_get(int unit, bcm_port_t port,
                                   bcm_port_management_packet_config_t *config,
                                   int max_config, int *config_count)

{
    int rv = BCM_E_NONE;
    bcm_mac_t   dest_mac;
    soc_reg_t reg[5] = {CTRL_DA1r, CTRL_DA2r, CTRL_DA3r, CTRL_DA4r,
                       CTRL_DA5r};
    soc_field_t field[5] = {CTRL_DA1f, CTRL_DA2f, CTRL_DA3f, CTRL_DA4f,
                       CTRL_DA5f};
    uint64 mac_field;
    bcm_ethertype_t etype;
    uint32 val;
    int enable, i = 0, cnt = 0;
    bcm_mac_t neigh_disc_da = {0x33, 0x33, 0x00, 0x00, 0x00, 0x00};
    bcm_mac_t cdp_da = {0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCC};
    bcm_mac_t sstp_da = {0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCD};

    if ((config == NULL) || (max_config == 0)) {
        return BCM_E_PARAM;
    }
    if (port < 0 || port > 16) {
        return BCM_E_PARAM;
    }
    *config_count = 0;
    soc_mem_lock(unit, PORT_TABm);
    rv = READ_MGMT_FRAME_ENABLEr(unit, port, &val);
    if (BCM_SUCCESS(rv)) {
        enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, CTRL_ETHERTYPE1f);
        if (enable) {
            rv = READ_CTRL_ETHERTYPE1r(unit, &val);
            etype = soc_reg_field_get(unit, CTRL_ETHERTYPE1r, val, 
                                      ETHERTYPEf);
            config[cnt].ethertype = etype;
            config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE;
            cnt++;
            if (cnt >= max_config) {
                soc_mem_unlock(unit, PORT_TABm);
                *config_count = cnt;
                return BCM_E_NONE;
            }
        }
        enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, CTRL_ETHERTYPE2f);
        if (enable) {
            rv = READ_CTRL_ETHERTYPE2r(unit, &val);
            etype = soc_reg_field_get(unit, CTRL_ETHERTYPE2r, val, 
                                      ETHERTYPEf);
            config[cnt].ethertype = etype;
            config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE;

            rv = soc_reg_get(unit, CTRL_DA6r, port, 0, &mac_field);
            SAL_MAC_ADDR_FROM_UINT64(dest_mac, mac_field);
            ENET_SET_MACADDR(config[cnt].dest_mac, dest_mac);
            config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_ETHERTYPE | 
                              BCM_PORT_MANAGEMENT_PACKET_DEST_MAC;
            cnt++;
            if (cnt >= max_config) {
                soc_mem_unlock(unit, PORT_TABm);
                *config_count = cnt;
                return BCM_E_NONE;
            }
        }
        enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, NEIGHBOR_DISCf);
        if (enable) {
            ENET_SET_MACADDR(config[cnt].dest_mac, neigh_disc_da);
            config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_DEST_MAC;
            cnt++;
            if (cnt >= max_config) {
                soc_mem_unlock(unit, PORT_TABm);
                *config_count = cnt;
                return BCM_E_NONE;
            }
        }
        enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, C_MC_DMAC_Af);
        if (enable) {
            ENET_SET_MACADDR(config[cnt].dest_mac, cdp_da);
            config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_DEST_MAC;
            cnt++;
            if (cnt >= max_config) {
                soc_mem_unlock(unit, PORT_TABm);
                *config_count = cnt;
                return BCM_E_NONE;
            }
        }
        enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, C_MC_DMAC_Bf);
        if (enable) {
            ENET_SET_MACADDR(config[cnt].dest_mac, sstp_da);
            config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_DEST_MAC;
            cnt++;
            if (cnt >= max_config) {
                soc_mem_unlock(unit, PORT_TABm);
                *config_count = cnt;
                return BCM_E_NONE;
            }
        }

        for (i = 0; i < 5; i++) {
            enable = soc_reg_field_get(unit, MGMT_FRAME_ENABLEr, val, field[i]);
            if (enable) {
                rv = soc_reg_get(unit, reg[i], port, 0, &mac_field);
                SAL_MAC_ADDR_FROM_UINT64(dest_mac, mac_field);
                ENET_SET_MACADDR(config[cnt].dest_mac, dest_mac);
                config[cnt].flags = BCM_PORT_MANAGEMENT_PACKET_DEST_MAC;
                cnt++;
                if (cnt >= max_config) {
                    *config_count = cnt;
                    break;
                }
            }
        }
    }
    soc_mem_unlock(unit, PORT_TABm);
    return rv;
}
