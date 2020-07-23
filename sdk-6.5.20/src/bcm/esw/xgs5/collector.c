/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Collector - Common collector infrastructure interface
 * Purpose: APIs to configure collector parameters
 *          which will be used by instrumentation applications.
 */

#include <soc/defs.h>

#if defined(BCM_COLLECTOR_SUPPORT)

#include <shared/bsl.h>
#include <soc/drv.h>
#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm_int/esw/collector.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/switch.h>
#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_export.h>
#endif /* BCM_FLOWTRACKER_SUPPORT */


/* Collector global information */
_bcm_collector_global_info_t *collector_global_info[BCM_MAX_NUM_UNITS] = {0};

/* List of collector export profiles info */
_bcm_collector_export_profile_info_t
                      *export_profiles_info_list[BCM_MAX_NUM_UNITS] = {0};
/* List of collectors info*/
_bcm_collector_info_t *collectors_info_list[BCM_MAX_NUM_UNITS] = {0};

#define BCM_COLLECTOR_INVALID_INDEX -1

/******************************************************
*                                                     *
*                 UTILITY FUNCTIONS                   *
 */

/*
 * Functions:
 *      _bcm_xgs5_collector_<header>_get
 * Purpose:
 *      The following set of _get() functions builds a given header for
 *      a given collector.
 * Parameters:
 *      collector_info -  (IN) Pointer to Collector information structure.
 *      <... other IN args ...>
 *      <header>  - (OUT) Returns header.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_xgs5_collector_udp_header_get(int unit,
                bcm_collector_info_t *collector_info,
                shr_udp_header_t *udp)
{
    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = collector_info->udp.src_port;
    udp->dst = collector_info->udp.dst_port;
    /* Do not set UDP length since this will be filled by
     * EAPP.
     */
    udp->length = 0;
    udp->sum = 0;    /* Calculated later */

    return BCM_E_NONE;
}

int
_bcm_xgs5_collector_ipv4_header_get(int unit, uint8 protocol,
                              uint8 tos, uint8 ttl,
                              bcm_ip_t src_ip_addr, bcm_ip_t dst_ip_addr,
                              shr_ipv4_header_t *ip)
{
    uint8 buffer[SHR_IPV4_HEADER_LENGTH];

    sal_memset(ip, 0, sizeof(*ip));

    ip->version  = SHR_IPV4_VERSION;
    ip->h_length = SHR_IPV4_HEADER_LENGTH_WORDS;
    ip->dscp     = tos;
    ip->ecn      = 0;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ip->length   = 0;
    ip->id       = 0;
    ip->flags    = 0;
    ip->f_offset = 0;
    ip->ttl      = ttl;
    ip->protocol = protocol;
    ip->sum      = 0;
    ip->src      = src_ip_addr;
    ip->dst      = dst_ip_addr;

    /* Calculate IP header checksum */
    shr_ipv4_header_pack(buffer, ip);
    ip->sum = 0;

    return BCM_E_NONE;
}

int
_bcm_xgs5_collector_ip_headers_get(int unit,
                             bcm_collector_info_t *collector_info,
                             uint16 *etype,
                             shr_ipv4_header_t *ipv4)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;


    *etype = SHR_L2_ETYPE_IPV4;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_collector_ipv4_header_get(unit, ip_protocol,
                                       collector_info->ipv4.dscp,
                                       collector_info->ipv4.ttl,
                                       collector_info->ipv4.src_ip,
                                       collector_info->ipv4.dst_ip,
                                       ipv4));

    return BCM_E_NONE;
}

int
_bcm_xgs5_collector_l2_header_get(int unit,
                           bcm_collector_info_t *collector_info,
                           uint16 etype, shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, collector_info->eth.dst_mac, SHR_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, collector_info->eth.src_mac, SHR_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag(s)
     */
    if (collector_info->eth.vlan_tag_structure == 
        BCM_COLLECTOR_ETH_HDR_UNTAGGED) { 
        /* Set to 0 to indicate untagged */
        l2->outer_vlan_tag.tpid = 0;
        l2->inner_vlan_tag.tpid = 0;
    } else {
        /* Fill outer vlan */
        if ((collector_info->eth.vlan_tag_structure ==
             BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED) ||
            (collector_info->eth.vlan_tag_structure ==
             BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED))
        {
            l2->outer_vlan_tag.tpid     = collector_info->eth.outer_tpid;
            l2->outer_vlan_tag.tci.prio =
                     BCM_VLAN_CTRL_PRIO(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.cfi  =
                     BCM_VLAN_CTRL_CFI(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.vid  =
                     BCM_VLAN_CTRL_ID(collector_info->eth.outer_vlan_tag);
        } else {
            return BCM_E_PARAM;
        }
        /* Fill inner vlan if double tagged */
        if (collector_info->eth.vlan_tag_structure ==
            BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
            l2->inner_vlan_tag.tpid     = collector_info->eth.inner_tpid;
            l2->inner_vlan_tag.tci.prio =
                     BCM_VLAN_CTRL_PRIO(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.cfi  =
                     BCM_VLAN_CTRL_CFI(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.vid  =
                     BCM_VLAN_CTRL_ID(collector_info->eth.inner_vlan_tag);
        } else {
            /* Set to 0 to indicate not inner tagged */
            l2->inner_vlan_tag.tpid = 0;
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

/***********************************************************
*                                                          *
*     COLLECTOR EXPORT PROFILE LIST MANAGEMENT FUNCTIONS   *
 */

/*
 * Function:
 *     _bcm_xgs5_export_profile_list_export_profile_add
 * Purpose:
 *     Adds an export profile to the export profiles list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     export_profile_id   - (IN) Export profile ID
 *     export_profile_info - (IN) Export profile informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_export_profile_list_export_profile_add(int unit,
                           int export_profile_id,
                           bcm_collector_export_profile_t *export_profile_info)
{
    _bcm_collector_export_profile_info_t *export_profiles_info_list_node =
        &(export_profiles_info_list[unit][export_profile_id]);

    if (export_profiles_info_list_node->export_profile_id !=
        BCM_COLLECTOR_INVALID_INDEX) {
        return BCM_E_EXISTS;
    }
    export_profiles_info_list_node->export_profile_id = export_profile_id;

    sal_memcpy(&(export_profiles_info_list_node->export_profile_info),
               export_profile_info,
               sizeof(bcm_collector_export_profile_t));

    LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
            (BSL_META_U(unit,
                        "COLLECTOR(unit %d) Info: %s" 
                        " (export_profile_id=0x%x).\n"), unit, FUNCTION_NAME(),
                         export_profile_id));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_export_profile_list_export_profile_delete
 * Purpose:
 *     Deletes a collector export profile from the export profiles list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     export_profile_id   - (IN) Export profile ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_export_profile_list_export_profile_delete (int unit,
                              int export_profile_id)
{
    _bcm_collector_export_profile_info_t *export_profiles_info_list_node =
        &(export_profiles_info_list[unit][export_profile_id]);

    if (export_profiles_info_list_node->export_profile_id !=
                              export_profile_id) {
        LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: export profile ID "
                  "does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(export_profiles_info_list_node, 0,
                                 sizeof(_bcm_collector_export_profile_info_t));

    export_profiles_info_list_node->export_profile_id =
            BCM_COLLECTOR_INVALID_INDEX;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_export_profile_list_export_profile_modify
 * Purpose:
 *     Modfiy a collector in the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     export_profile_id   - (IN) Export profile ID
 *     export_profile_info - (IN) Export profile informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_export_profile_list_export_profile_modify(int unit,
                           int export_profile_id,
                           bcm_collector_export_profile_t *export_profile_info)
{
    _bcm_collector_export_profile_info_t *export_profiles_info_list_node =
        &(export_profiles_info_list[unit][export_profile_id]);

    if (export_profiles_info_list_node->export_profile_id !=
                                        export_profile_id) {
        LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: export profile ID "
                  "does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(&(export_profiles_info_list_node->export_profile_info),
               export_profile_info,
               sizeof(bcm_collector_export_profile_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_export_profile_list_export_profile_get
 * Purpose:
 *     Get a collector from the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     export_profile_id   - (IN) The export profile ID
 *     export_profile_info - (OUT) collector export profile information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_export_profile_list_export_profile_get (int unit,
                           int export_profile_id,
                           bcm_collector_export_profile_t *export_profile_info)
{
    _bcm_collector_export_profile_info_t *export_profiles_info_list_node =
        &(export_profiles_info_list[unit][export_profile_id]);

    if (export_profiles_info_list_node->export_profile_id !=
                                        export_profile_id) {
        LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: export profile ID "
                  "does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(export_profile_info,
               &(export_profiles_info_list_node->export_profile_info), 
                sizeof(bcm_collector_export_profile_t));


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_export_profile_create
 * Purpose:
 *      Creates a collector export profile with given export profile info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector export profile create options
 *      id              - (INOUT)  Collector Id
 *      export_profile_info  - (IN)  Collector export profile info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_export_profile_create(int unit,
                          uint32 options,
                          int *id,
                          bcm_collector_export_profile_t *export_profile_info)
{
    int rv = BCM_E_NONE, export_profile_id = 0;
    bcm_collector_export_profile_t old_export_profile_info;
    _bcm_collector_global_info_t *collector_per_unit_global_info =
                                    COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: collector"
                            " module is not initialized"), unit));
        return BCM_E_INIT;
    }

    if (export_profile_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: NULL export profile"
                            " passed"), unit));
        return BCM_E_PARAM;
    }

    /* 1. IF replace, WITH_ID is mandatory. If not provided, return
     * BCM_E_PARAM.
     *
     * 2. Also verify that a collector export profile with such an ID
     * exists, else return BCM_E_NOT_FOUND
     */
    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE) &&
       !(options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID)) {
        return BCM_E_PARAM;
    }

    /* Check collector wire format is valid or not */
    if ((export_profile_info->wire_format < bcmCollectorWireFormatIpfix) ||
        (export_profile_info->wire_format >= bcmCollectorWireFormatCount)) {
        return BCM_E_PARAM;
    }

    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID) &&
        (options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
        sal_memset(&old_export_profile_info, 0,
                   sizeof(old_export_profile_info));
        rv = _bcm_xgs5_export_profile_list_export_profile_get(unit, *id,
                                                     &old_export_profile_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* WITH_ID */
    if (options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID) {
        if (!(options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
            /* If WITH_ID and not replace, reserve the ID */
            rv = shr_idxres_list_reserve(
                            collector_per_unit_global_info->export_profile_pool,
                                         *id, *id);
            if (BCM_FAILURE(rv)) {
                return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
            }
        }
    } else {
        /* Else allocate the ID */
        rv = shr_idxres_list_alloc(
                            collector_per_unit_global_info->export_profile_pool,
                            (shr_idxres_element_t *)(&export_profile_id));
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_RESOURCE) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: Cannot create more than"
                  " %d collector export profiles"), unit,
                  BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES));
            }
            return rv;
        }
        *id = export_profile_id;
    }

    /* WITH_ID and replace. */
    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID)  &&
        (options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
        /* Modify the collector info in the list with ID. */
        rv = _bcm_xgs5_export_profile_list_export_profile_modify(unit, *id,
                                                          export_profile_info);
    }
    else {
        /* Add it to the list of collectors */
        rv = _bcm_xgs5_export_profile_list_export_profile_add(unit, *id,
                                                    export_profile_info);
    }
    if (BCM_FAILURE(rv)) {
        /* Free up the collector id allocated */
        if (!(options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
            BCM_IF_ERROR_RETURN(
               shr_idxres_list_free(
                            collector_per_unit_global_info->export_profile_pool,
                            *id));
        }
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_export_profile_get
 * Purpose:
 *      Gets a collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      export_profile_info  - (OUT) Collector export profile info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_export_profile_get(int unit,
                           int id,
                           bcm_collector_export_profile_t *export_profile_info)
{
    int rv = BCM_E_NONE;

    if (export_profiles_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure export_profile_info is not NULL */
    if (export_profile_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the collector export profile info from the list
     * using ID.
     */
    rv = _bcm_xgs5_export_profile_list_export_profile_get(unit, id,
                                                   export_profile_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_export_profile_ids_get_all
 * Purpose:
 *      Get the ID list of all collectors configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) Collector id list
 *      list_szie            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_export_profile_ids_get_all(
                           int unit,
                           int max_size,
                           int *collector_list,
                           int *list_size)
{
    int i;
    int num_export_profiles;

    if (export_profiles_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && collector_list == NULL) {
        return BCM_E_PARAM;
    }

    num_export_profiles = 0;
    *list_size = 0;
    for (i = BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i <= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID; i++) {
        if (export_profiles_info_list[unit][i].export_profile_id == i) {
            /* Collector export profile exists */
            num_export_profiles++;
            if (*list_size < max_size) {
                collector_list[*list_size] = i;
                (*list_size)++;
            }
        }
    }

    /* If the max_size is 0 then return the number of collector export profiles
     *  configured in list_size
     */
    if (max_size == 0) {
        *list_size = num_export_profiles;
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_export_profile_destroy
 * Purpose:
 *      Destroys a collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      id             - (IN) Collector expor profile Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_export_profile_destroy(int unit,
                                 int id)
{
    int rv = BCM_E_NONE;
     _bcm_collector_global_info_t *collector_per_unit_global_info =
                            COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: collector"
                            " module is not initialized"), unit));
        return BCM_E_INIT;
    }

    if (export_profiles_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (id > BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID) {
        return BCM_E_PARAM;
    }

    if (export_profiles_info_list[unit][id].ref_count != 0) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                              "COLLECTOR(unit %d) Error: Export profile in use\n"),
                   unit));
        return BCM_E_EXISTS;
    }

    /* Delete the collector expor profile from the expor profile list with
     * given ID.
     */
    rv = _bcm_xgs5_export_profile_list_export_profile_delete(unit, id);
    /* If not found in the list, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free up the ID from the pool */
    BCM_IF_ERROR_RETURN(
        shr_idxres_list_free(
                   collector_per_unit_global_info->export_profile_pool, id));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_export_profile_list_destroy
 * Purpose:
 *     Destroy the collector export profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_export_profile_list_destroy (int unit)
{
    if (export_profiles_info_list[unit] != NULL) {
        sal_free(export_profiles_info_list[unit]);
    }

    export_profiles_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_export_profile_list_create
 * Purpose:
 *     Create the collector export profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_export_profile_list_create (int unit)
{
    int i = 0;
    _bcm_collector_export_profile_info_t *export_profiles_node = NULL;

    if (export_profiles_info_list[unit] == NULL) {
        _BCM_COLLECTOR_ALLOC(export_profiles_info_list[unit],
                             _bcm_collector_export_profile_info_t,
                      sizeof(_bcm_collector_export_profile_info_t) *
                (BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES +
                 BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID),
                "collector export profile list");

    }

    if (export_profiles_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    for (i= BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i<= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID; i++) {
        export_profiles_node = &(export_profiles_info_list[unit][i]);
        export_profiles_node->export_profile_id =
                      BCM_COLLECTOR_INVALID_INDEX;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_export_profile_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for an export profile
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Export profile Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_export_profile_ref_count_update(int unit, int id, int update)
{
    if (id > BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID) {
        return BCM_E_PARAM;
    }

    if ((export_profiles_info_list[unit][id].ref_count + update) < 0) {
        return BCM_E_PARAM;
    }
    export_profiles_info_list[unit][id].ref_count += update;

    return BCM_E_NONE;
}

/******************************************************
*                                                     *
*        COLLECTOR LIST MANAGEMENT FUNCTIONS          *
 */

/*
 * Function:
 *     _bcm_xgs5_collector_list_collector_add
 * Purpose:
 *     Add a collector to the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     collector_info - (IN) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_list_collector_add(int unit,
                           bcm_collector_t collector_id,
                           bcm_collector_info_t *collector_info)
{
    _bcm_collector_info_t *collectors_info_list_node =
        &(collectors_info_list[unit][collector_id]);

    if (collectors_info_list_node->collector_id !=
        BCM_COLLECTOR_INVALID_INDEX) {
        return BCM_E_EXISTS;
    }
    collectors_info_list_node->collector_id = collector_id;

    sal_memcpy(&(collectors_info_list_node->collector_info), collector_info,
               sizeof(bcm_collector_info_t));

    LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
            (BSL_META_U(unit,
                        "COLLECTOR(unit %d) Info: %s" 
                        " (Collector=0x%x).\n"), unit, FUNCTION_NAME(),
                         collector_id));
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_xgs5_collector_list_collector_delete
 * Purpose:
 *     Delete a collector from the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_list_collector_delete (int unit,
                              bcm_collector_t collector_id)
{
    _bcm_collector_info_t *collectors_info_list_node =
        &(collectors_info_list[unit][collector_id]);

    if (collectors_info_list_node->collector_id != collector_id) {
        LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: collector ID"
                  " does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(collectors_info_list_node, 0,
                                 sizeof(_bcm_collector_info_t));
    collectors_info_list_node->collector_id = BCM_COLLECTOR_INVALID_INDEX;
    collectors_info_list_node->user         = _BCM_COLLECTOR_EXPORT_APP_NONE;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_xgs5_collector_list_collector_modify
 * Purpose:
 *     Modfiy a collector in the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     collector_info - (IN) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_list_collector_modify(int unit,
                           bcm_collector_t collector_id,
                           bcm_collector_info_t *collector_info)
{
    _bcm_collector_info_t *list_node =
        &(collectors_info_list[unit][collector_id]);

    if (list_node->collector_id != collector_id) {
        LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: collector ID"
                  " does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(&(list_node->collector_info), collector_info,
               sizeof(bcm_collector_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_list_collector_get
 * Purpose:
 *     Get a collector from the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_id   - (IN) The collector ID
 *     collector_info - (OUT) collector information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_list_collector_get (int unit,
                              bcm_collector_t collector_id,
                              bcm_collector_info_t *collector_info)
{
    _bcm_collector_info_t *list_node =
        &(collectors_info_list[unit][collector_id]);

    if (list_node->collector_id != collector_id) {
        LOG_DEBUG(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: collector ID"
                  " does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(collector_info, &(list_node->collector_info), 
                sizeof(bcm_collector_info_t));


    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_list_destroy
 * Purpose:
 *     Destroy the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_list_destroy (int unit)
{
    if (collectors_info_list[unit] != NULL) {
        sal_free(collectors_info_list[unit]);
    }

    collectors_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_list_create
 * Purpose:
 *     Create the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_list_create (int unit)
{
    int i = 0;
    _bcm_collector_info_t *collector_node = NULL;

    if (collectors_info_list[unit] == NULL) {
        _BCM_COLLECTOR_ALLOC(collectors_info_list[unit], _bcm_collector_info_t,
                      sizeof(_bcm_collector_info_t) *
                (BCM_INT_COLLECTOR_MAX_COLLECTORS +
                 BCM_INT_COLLECTOR_COLLECTOR_START_ID),
                "collector list");
    }

    if (collectors_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    for (i= BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i<= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {
        collector_node = &(collectors_info_list[unit][i]);
        collector_node->collector_id =
                      BCM_COLLECTOR_INVALID_INDEX;
        collector_node->user = _BCM_COLLECTOR_EXPORT_APP_NONE;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_create
 * Purpose:
 *      Createa a collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      id              - (INOUT)  Collector Id
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_create(int unit,
                                uint32 options,
                                bcm_collector_t *id,
                                bcm_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE, collector_id = 0;
    bcm_collector_info_t old_collector_info;
    _bcm_collector_global_info_t *collector_per_unit_global_info =
                                    COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: collector"
                            " module is not initialized"), unit));
        return BCM_E_INIT;
    }

    if (collector_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: NULL collector info"
                            " passed"), unit));
        return BCM_E_PARAM;
    }

    /* 1. IF replace, WITH_ID is mandatory. If not provided, return
     * BCM_E_PARAM.
     *
     * 2. Also verify that a collector with such an ID
     * exists, else return BCM_E_NOT_FOUND
     */
    if ((options & BCM_COLLECTOR_REPLACE) &&
       !(options & BCM_COLLECTOR_WITH_ID)) {
        return BCM_E_PARAM;
    }

    /* Validate ETH header */
    switch (collector_info->eth.vlan_tag_structure) {
        case BCM_COLLECTOR_ETH_HDR_UNTAGGED:
            /* Nothing to validate */
            break;

        case BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED:
            if (collector_info->eth.outer_vlan_tag > BCM_VLAN_MAX) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                          (BSL_META_U(unit,
                                      "COLLECTOR(unit %d) Error: Invalid outer_vlan_tag=%d\n"),
                           unit, collector_info->eth.outer_vlan_tag));
                return BCM_E_PARAM;
            }
            break;

        case BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED:
            if ((collector_info->eth.outer_vlan_tag > BCM_VLAN_MAX) ||
                (collector_info->eth.inner_vlan_tag > BCM_VLAN_MAX)) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                          (BSL_META_U(unit,
                                      "COLLECTOR(unit %d) Error: Invalid outer_vlan_tag(%d)"
                                      " or inner_vlan_tag(%d) \n"),
                           unit, collector_info->eth.outer_vlan_tag,
                           collector_info->eth.inner_vlan_tag));
                return BCM_E_PARAM;
            }
            break;

        default:
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                      (BSL_META_U(unit,
                                  "COLLECTOR(unit %d) Error: Invalid vlan_tag_structure=%d\n"),
                       unit, collector_info->eth.vlan_tag_structure));
            return BCM_E_PARAM;
    }

    if ((collector_info->collector_type == bcmCollectorLocal) &&
        (collector_info->transport_type != bcmCollectorTransportTypeRaw)) {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Error: Transport type configuration"), unit));
        return BCM_E_CONFIG;
    }

    /* Check collector transport type is valid or not */
    if ((collector_info->transport_type < bcmCollectorTransportTypeIpv4Udp) ||
        (collector_info->transport_type >= bcmCollectorTransportTypeCount)) {
        return BCM_E_PARAM;
    }

    if ((options & BCM_COLLECTOR_WITH_ID) &&
        (options & BCM_COLLECTOR_REPLACE)) {
        sal_memset(&old_collector_info, 0, sizeof(old_collector_info));
        rv = _bcm_xgs5_collector_list_collector_get(unit, *id,
                                                       &old_collector_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        if((collector_info->ipv4.src_ip == 0) ||
           (collector_info->ipv4.dst_ip == 0))
        {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Error: Invalid IP configuration"), unit));
            return BCM_E_PARAM;
        }
    }

    if ((collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) ||
        (collector_info->transport_type == bcmCollectorTransportTypeIpv6Udp)) {
        if((collector_info->udp.src_port == 0) ||
           (collector_info->udp.dst_port == 0)) {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error:"
                            " Invalid UDP configuration"), unit));
            return BCM_E_PARAM;
        }
    }

    /* WITH_ID */
    if (options & BCM_COLLECTOR_WITH_ID) {
        if (!(options & BCM_COLLECTOR_REPLACE)) {
            /* If WITH_ID and not replace, reserve the ID */
            rv = shr_idxres_list_reserve(
                                collector_per_unit_global_info->collector_pool,
                                *id, *id);
            if (BCM_FAILURE(rv)) {
                return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
            }
        }
    } else {
        /* Else allocate the ID */
        rv = shr_idxres_list_alloc(
                         collector_per_unit_global_info->collector_pool,
                                   (shr_idxres_element_t *)(&collector_id));
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_RESOURCE) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Error: Cannot create more than"
                  " %d collectors"), unit, BCM_INT_COLLECTOR_MAX_COLLECTORS));
            }
            return rv;
        }
        *id = collector_id;    
    }

    /* WITH_ID and replace. */
    if ((options & BCM_COLLECTOR_WITH_ID)  &&
        (options & BCM_COLLECTOR_REPLACE)) {
        /* Modify the collector info in the list with ID. */
        rv = _bcm_xgs5_collector_list_collector_modify(unit, *id,
                                                          collector_info);
    }
    else {
        /* Add it to the list of collectors */
        rv = _bcm_xgs5_collector_list_collector_add(unit, *id, collector_info);
    }
    if (BCM_FAILURE(rv)) {
        /* Free up the collector id allocated */
        if (!(options & BCM_COLLECTOR_REPLACE)) {
            BCM_IF_ERROR_RETURN(
               shr_idxres_list_free(
                           collector_per_unit_global_info->collector_pool,*id));
        }
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_get
 * Purpose:
 *      Gets a collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_get(int unit,
                             bcm_collector_t id,
                             bcm_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;

    if (collectors_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure collector_info is not NULL */
    if (collector_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the collector info from the list
     * using ID.
     */
    rv = _bcm_xgs5_collector_list_collector_get(unit, id,
                                                   collector_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_get_ids_all
 * Purpose:
 *      Get the ID list of all collectors configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) Collector id list
 *      list_szie            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_ids_get_all(
                           int unit,
                           int max_size,
                           bcm_collector_t *collector_list,
                           int *list_size)
{
    int i;
    int num_collectors;

    if (collectors_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && collector_list == NULL) {
        return BCM_E_PARAM;
    }

    num_collectors = 0;
    *list_size = 0;
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {
        if (collectors_info_list[unit][i].collector_id == i) {
            /* Collector exists */
            num_collectors++;
            if (*list_size < max_size) {
                collector_list[*list_size] = i;
                (*list_size)++;
            }
        }
    }

    /* If the max_size is 0 then return the number of collectors configured in
     * list_size
     */
    if (max_size == 0) {
        *list_size = num_collectors;
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_collector_destroy
 * Purpose:
 *      Destroys a collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      id             - (IN) Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_destroy(int unit,
                                 bcm_collector_t id)
{
    int rv = BCM_E_NONE;
     _bcm_collector_global_info_t *collector_per_unit_global_info =
                            COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: collector"
                            " module is not initialized"), unit));
        return BCM_E_INIT;
    }

    if (collectors_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if (collectors_info_list[unit][id].ref_count != 0) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                              "COLLECTOR(unit %d) Error: Collector in use\n"),
                   unit));
        return BCM_E_EXISTS;
    }

    /* Delete the collector from the collector list with
     * given ID.
     */
    rv = _bcm_xgs5_collector_list_collector_delete(unit, id);
    /* If not found in the list, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free up the ID from the pool */
    BCM_IF_ERROR_RETURN(
        shr_idxres_list_free(
                   collector_per_unit_global_info->collector_pool, id));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_ft_export_record_register
 * Description:
 *     To register the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     collector_id  Collector Id
 *     callback_options Callback options
 *     callback_fn   user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 */
STATIC int
_bcm_xgs5_collector_ft_export_record_register(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn,
    void *userdata)
{
    int rv = BCM_E_UNAVAIL;

#if defined (BCM_FLOWTRACKER_SUPPORT)
    bcm_flowtracker_collector_t ft_collector_id;
    bcm_flowtracker_collector_callback_options_t ft_callback_options;
    bcm_flowtracker_export_record_cb_f ft_callback_fn;
#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined (BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        ft_collector_id = collector_id;
        ft_callback_fn = (bcm_flowtracker_export_record_cb_f)callback_fn;
        sal_memcpy(&ft_callback_options, &callback_options,
                   sizeof(bcm_collector_callback_options_t));

        rv = bcmi_ft_export_record_register(unit,
                                            ft_collector_id,
                                            ft_callback_options,
                                            ft_callback_fn,
                                            userdata);
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */

    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_collector_export_record_register
 * Description:
 *     To register the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     callback      user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 */
int
_bcm_xgs5_collector_export_record_register(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn,
    void *userdata)
{
    int rv = BCM_E_NONE;
    _bcm_collector_global_info_t *collector_per_unit_global_info =
        COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: collector"
                            " module is not initialized"), unit));
        return BCM_E_INIT;
    }

    if (collectors_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if (BCM_COLLECTOR_INVALID_INDEX ==
            collectors_info_list[unit][collector_id].collector_id) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                        "COLLECTOR(unit %d) Error: Collector"
                        " not created\n"), unit));
        return BCM_E_NOT_FOUND;
    }

    switch(collectors_info_list[unit][collector_id].user) {
        case _BCM_COLLECTOR_EXPORT_APP_NONE:
            LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: Collector not"
                            " associated\n"), collector_id));
            rv = BCM_E_CONFIG;
            break;
        case _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX:
            rv = _bcm_xgs5_collector_ft_export_record_register(unit,
                    collector_id, callback_options, callback_fn, userdata);
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: Collector associated"
                            " app do not support callback\n"), collector_id));
            rv = BCM_E_UNAVAIL;
            break;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_collector_ft_export_record_unregister
 * Description:
 *     To unregister the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     collector_id  Collector Id
 *     callback_options Callback options
 *     callback_fn   user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
STATIC int
_bcm_xgs5_collector_ft_export_record_unregister(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn)
{
    int rv = BCM_E_UNAVAIL;

#if defined (BCM_FLOWTRACKER_SUPPORT)
    bcm_flowtracker_collector_t ft_collector_id;
    bcm_flowtracker_collector_callback_options_t ft_callback_options;
    bcm_flowtracker_export_record_cb_f ft_callback_fn;
#endif /* BCM_FLOWTRACKER_SUPPORT */

#if defined (BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_bscan_ft_fifodma_support)) {
        ft_collector_id = collector_id;
        ft_callback_fn = (bcm_flowtracker_export_record_cb_f)callback_fn;
        sal_memcpy(&ft_callback_options, &callback_options,
                   sizeof(bcm_collector_callback_options_t));

        rv = bcmi_ft_export_record_unregister(unit,
                                              ft_collector_id,
                                              ft_callback_options,
                                              ft_callback_fn);
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */

    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_collector_export_record_unregister
 * Description:
 *     To unregister the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     callback      user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
_bcm_xgs5_collector_export_record_unregister(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn)
{
    int rv = BCM_E_NONE;
    _bcm_collector_global_info_t *collector_per_unit_global_info =
        COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: collector"
                            " module is not initialized"), unit));
        return BCM_E_INIT;
    }

    if (collectors_info_list[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if (BCM_COLLECTOR_INVALID_INDEX ==
            collectors_info_list[unit][collector_id].collector_id) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                        "COLLECTOR(unit %d) Error: Collector"
                        " not created\n"), unit));
        return BCM_E_NOT_FOUND;
    }

    switch(collectors_info_list[unit][collector_id].user) {
        case _BCM_COLLECTOR_EXPORT_APP_NONE:
            LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: Collector not"
                            " associated\n"), collector_id));
            rv = BCM_E_CONFIG;
            break;
        case _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX:
            rv = _bcm_xgs5_collector_ft_export_record_unregister(unit,
                    collector_id, callback_options, callback_fn);
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_COLLECTOR, (BSL_META_U(unit,
                            "COLLECTOR(unit %d) Error: Collector associated"
                            " app do not support callback\n"), collector_id));
            rv = BCM_E_UNAVAIL;
            break;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_collector_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for a collector
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_ref_count_update(int unit, bcm_collector_t id, int update)
{
    if (id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    if ((collectors_info_list[unit][id].ref_count + update) < 0) {
        return BCM_E_PARAM;
    }
    collectors_info_list[unit][id].ref_count += update;

    if (collectors_info_list[unit][id].ref_count == 0) {
        collectors_info_list[unit][id].user = _BCM_COLLECTOR_EXPORT_APP_NONE ;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_ref_count_get
 * Purpose:
 *     Get reference count for a collector
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 * Returns:
 *     Reference count
 */
int
_bcm_xgs5_collector_ref_count_get(int unit, bcm_collector_t id)
{
    if (id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return 0;
    }
    return (collectors_info_list[unit][id].ref_count);
}

/*
 * Function:
 *     _bcm_xgs5_collector_user_update
 * Purpose:
 *     Update the user for the collector.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     user          - (IN) The user type of the calling app.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_user_update(int unit, bcm_collector_t id, _bcm_collector_export_app_t user)
{
    if (id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return BCM_E_PARAM;
    }

    collectors_info_list[unit][id].user = user;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_collector_check_user_is_other
 * Purpose:
 *      Check if collector ID is used by other user.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     my_user       - (IN) The user type of the calling app.
 * Returns:
 *     BCM_E_XXX
 */
uint8
_bcm_xgs5_collector_check_user_is_other(int unit, bcm_collector_t id, _bcm_collector_export_app_t my_user)
{
    if (id > BCM_INT_COLLECTOR_COLLECTOR_END_ID) {
        return 0;
    }

    if ((collectors_info_list[unit][id].user != _BCM_COLLECTOR_EXPORT_APP_NONE) &&
        (collectors_info_list[unit][id].user != my_user)) {
        return 1;
    }

    return 0;
}
#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_COLLECTOR_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_COLLECTOR_VERSION_1_1      SOC_SCACHE_VERSION(1,1)
/* Added sync/recover for ipfix related fields in collector info. */
#define BCM_WB_COLLECTOR_VERSION_1_2      SOC_SCACHE_VERSION(1,2)
/* Added sync/recover for ProtoBuf related fields in collector info. */
#define BCM_WB_COLLECTOR_VERSION_1_3      SOC_SCACHE_VERSION(1,3)
/* Added sync/recover for Collector Type in collector info */
#define BCM_WB_COLLECTOR_VERSION_1_4      SOC_SCACHE_VERSION(1,4)
/* Added sync/recover for timestamp source in collector info */
#define BCM_WB_COLLECTOR_VERSION_1_5      SOC_SCACHE_VERSION(1,5)
/* Added sync/recover for max_records_reserve, Observation domain ID
 * in collector info
 */
#define BCM_WB_COLLECTOR_VERSION_1_6      SOC_SCACHE_VERSION(1,6)
/* Added sync/recover for IPFIX enterprise number */
#define BCM_WB_COLLECTOR_VERSION_1_7      SOC_SCACHE_VERSION(1,7)
#define BCM_WB_COLLECTOR_DEFAULT_VERSION  BCM_WB_COLLECTOR_VERSION_1_7

#define COLLECTOR_SCACHE_WRITE(_scache, _val, _type)                     \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define COLLECTOR_SCACHE_READ(_scache, _var, _type)                      \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)

/* Collector flags to be written in scache */
#define _BCM_COLLECTOR_WB_FLAGS_L4_CHECKSUM_ENABLE (1 << 0)

/*
 * Function:
 *     _bcm_collector_info_sync
 * Purpose:
 *     Syncs all collectors information to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_collector_info_sync(int unit, uint8 **scache)
{

    int i, j;
    _bcm_collector_info_t *collector_list = collectors_info_list[unit];
    _bcm_collector_info_t *collector = NULL;
    _bcm_collector_export_profile_info_t *export_profiles_list =
                                     export_profiles_info_list[unit];
    _bcm_collector_export_profile_info_t *export_profile = NULL;
    uint32 collector_alloc_count = 0, export_profile_alloc_count = 0;
    _bcm_collector_global_info_t  *collector_per_unit_global_info =
                                    COLLECTOR_GLOBAL_INFO_GET(unit);
    uint32 first, last, validLow, validHigh, freeCount;
    uint32 flags;

    shr_idxres_list_state(collector_per_unit_global_info->collector_pool,
            &first, &last,
            &validLow, &validHigh,
            &freeCount, &collector_alloc_count);
    shr_idxres_list_state(collector_per_unit_global_info->export_profile_pool,
            &first, &last,
            &validLow, &validHigh,
            &freeCount, &export_profile_alloc_count);

    /* Number of collectors configured */
    COLLECTOR_SCACHE_WRITE(*scache, collector_alloc_count, uint32);

    /* Number of collector export profiles configured */
    COLLECTOR_SCACHE_WRITE(*scache, export_profile_alloc_count, uint32);

    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Collector Id */
        COLLECTOR_SCACHE_WRITE(*scache, collector->collector_id, uint16);

        /* Transport type */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.transport_type,
                               uint16);

        /* Dst mac addr */
        for (j = 0; j < 6; j++) {
            COLLECTOR_SCACHE_WRITE(*scache,
                                   collector->collector_info.eth.dst_mac[j],
                                   uint8);
        }

        /* Src mac addr */
        for (j = 0; j < 6; j++) {
            COLLECTOR_SCACHE_WRITE(*scache,
                                   collector->collector_info.eth.src_mac[j],
                                   uint8);
        }

        /* Vlan tag structure */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.eth.vlan_tag_structure,
                               uint8);

        /* Outer TPID */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.eth.outer_tpid,
                               uint16);

        /* Inner TPID */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.eth.inner_tpid,
                               uint16);

        /* Outer vlan tag */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.eth.outer_vlan_tag,
                               uint32);

        /* Inner vlan tag */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.eth.inner_vlan_tag,
                               uint32);

        if(collector->collector_info.transport_type ==
                       bcmCollectorTransportTypeIpv4Udp) {
            /* Src IP */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv4.src_ip, uint32);

            /* Dst IP */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv4.dst_ip, uint32);

            /* DSCP */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv4.dscp, uint8);

            /* TTL */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv4.ttl, uint8);
        }
        else
        {
            /* Src IP */
            for (j = 0; j < 16; j++) {
                COLLECTOR_SCACHE_WRITE(*scache,
                        collector->collector_info.ipv6.src_ip[j],
                        uint8);
            }

            /* Dst IP */
            for (j = 0; j < 16; j++) {
                COLLECTOR_SCACHE_WRITE(*scache,
                        collector->collector_info.ipv6.dst_ip[j],
                        uint8);
            }

            /* Traffic class */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv6.traffic_class, uint8);
            /* Flow label */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv6.flow_label, uint32);
            /* Hop limit */
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.ipv6.hop_limit, uint8);
        }

        /* Src port */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.udp.src_port, uint16);

        /* Dst port */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.udp.dst_port, uint16);

    }

    for (i = BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i <= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID; i++) {

        export_profile = &(export_profiles_list[i]);

        if (export_profile->export_profile_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector export profile does not exist, continue */
            continue;
        }

       /* Collector export profile Id */
        COLLECTOR_SCACHE_WRITE(*scache,
                               export_profile->export_profile_id, uint16);

        /* Collector export profile wire format */
        COLLECTOR_SCACHE_WRITE(*scache,
                   export_profile->export_profile_info.wire_format, uint32);
 
        /* Collector export profile export interval */
        COLLECTOR_SCACHE_WRITE(*scache,
                   export_profile->export_profile_info.export_interval, uint32);

        /* Collector export profile max packet length or num records */
        if (export_profile->export_profile_info.flags &
                            BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
            COLLECTOR_SCACHE_WRITE(*scache,
                                   export_profile->export_profile_info.num_records,
                                   uint32);
        } else {
            COLLECTOR_SCACHE_WRITE(*scache,
                                   export_profile->export_profile_info.max_packet_length,
                                   uint32);
        }
     }

    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Collector Id */
        COLLECTOR_SCACHE_WRITE(*scache, collector->collector_id, uint16);

        flags = 0;
        if((collector->collector_info.transport_type ==
                                           bcmCollectorTransportTypeIpv4Udp) ||
           (collector->collector_info.transport_type ==
                                           bcmCollectorTransportTypeIpv6Udp)) {

            if (collector->collector_info.udp.flags &
                                       BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE) {
                flags |= _BCM_COLLECTOR_WB_FLAGS_L4_CHECKSUM_ENABLE;
            }
        }
        /* Collector flags */
        COLLECTOR_SCACHE_WRITE(*scache, flags, uint32);
    }

    for (i = BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i <= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID; i++) {

        export_profile = &(export_profiles_list[i]);

        if (export_profile->export_profile_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector export profile does not exist, continue */
            continue;
        }

        /* Collector export profile Id */
        COLLECTOR_SCACHE_WRITE(*scache,
                               export_profile->export_profile_id, uint16);

        /* Export profile flags */
        COLLECTOR_SCACHE_WRITE(*scache,
                               export_profile->export_profile_info.flags, uint32);

    }

    /* Scache v2 sync */
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Collector Id */
        COLLECTOR_SCACHE_WRITE(*scache, collector->collector_id, uint16);

        /* Ipfix Protocol version */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.ipfix.version, int);

        /* Ipfix Initial sequence number */
        COLLECTOR_SCACHE_WRITE(*scache,
                collector->collector_info.ipfix.initial_sequence_num, int);
    }

    /* Scache v3 sync */
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* System Id length */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.protobuf.system_id_length, int);

        /* System Id */
        for (j = 0; j < collector->collector_info.protobuf.system_id_length; j++) {
            COLLECTOR_SCACHE_WRITE(*scache,
                    collector->collector_info.protobuf.system_id[j],
                    uint8);
        }

        /* Component Id */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.protobuf.component_id, uint32);

    }

    /* Scache v4 sync */
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Collector Type */
        COLLECTOR_SCACHE_WRITE(*scache,
                               collector->collector_info.collector_type, int);
    }

    /* Scache v5 sync */
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
            i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Collector Type */
        COLLECTOR_SCACHE_WRITE(*scache,
                collector->collector_info.src_ts, bcm_collector_timestamp_source_t);
    }

   /* Scache v6 sync */
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
            i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Collector max number of FIFO records to reserve */
        COLLECTOR_SCACHE_WRITE(*scache,
                collector->collector_info.max_records_reserve, int);

        /* Ipfix observation domain ID */
        COLLECTOR_SCACHE_WRITE(*scache,
                collector->collector_info.ipfix.observation_domain_id, uint32);
    }

   /* Scache v7 sync */
    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
            i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
            /* Collector does not exist, continue */
            continue;
        }

        /* Ipfix enterprise number */
        COLLECTOR_SCACHE_WRITE(*scache,
                collector->collector_info.ipfix.enterprise_number, uint32);
    }

}

/*
 * Function:
 *     _bcm_xgs5_collector_sync
 * Purpose:
 *     Syncs Collector data to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_collector_sync(int unit)
{

    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;
    _bcm_collector_global_info_t  *collector_per_unit_global_info =
                                    COLLECTOR_GLOBAL_INFO_GET(unit);

    if (collector_per_unit_global_info == NULL) {
        return BCM_E_NONE;
    }

    /* Get Collector module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COLLECTOR, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache,
                                                BCM_WB_COLLECTOR_DEFAULT_VERSION,
                                                NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    _bcm_collector_info_sync(unit, &scache);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_collector_recover
 * Purpose:
 *     Recover Collector data from scache
 * Parameters:
 *     unit          - (IN) Device unit number
 *     recovered_ver - (IN) Recovered version
 *     scache        - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_collector_recover(int unit, uint16 recovered_ver, uint8 **scache)
{
    int i = 0, j = 0;
    int rv;
    int recovered_count = 0;
    uint16 collector_id;
    uint16 collector_export_profile_id;
    shr_idxres_element_t tmp_id = 0;
    _bcm_collector_global_info_t *collector_per_unit_global_info =
                                    COLLECTOR_GLOBAL_INFO_GET(unit);
    _bcm_collector_info_t *collector = NULL;
    _bcm_collector_export_profile_info_t *export_profile = NULL;
    uint32 collector_alloc_count = 0, export_profile_alloc_count = 0;
    uint32 flags;

    /* Number of collectors configured */
    COLLECTOR_SCACHE_READ(*scache, collector_alloc_count, uint32);

    /* Number of collector export profiles configured */
    COLLECTOR_SCACHE_READ(*scache, export_profile_alloc_count, uint32);

    /* Note that from v1.3 onwards, we are not syncing collector id
     * per warmboot version and relying on collector Id from this loop v1.0
     * This shall save scache memory for syncing/recovering relevant info
     * per warmboot version.
     */
    for (i = 0; i< collector_alloc_count; i++) {

        /* Collector Id */
        COLLECTOR_SCACHE_READ(*scache, collector_id, uint16);
        if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID)
        {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Info: Recoved collector Id"
                  " %d out of range"), unit,
                  collector_id));
            return BCM_E_INTERNAL;
        }
        collector = &(collectors_info_list[unit][collector_id]);

        collector->collector_id = collector_id;
        tmp_id = collector_id;
        rv = shr_idxres_list_reserve(
                            collector_per_unit_global_info->collector_pool,
                            tmp_id, tmp_id);
        BCM_IF_ERROR_RETURN(rv);

        /* Transport type */
        COLLECTOR_SCACHE_READ(*scache, collector->collector_info.transport_type,
                              uint16);
        /* Dst mac addr */
        for (j = 0; j < 6; j++) {
            COLLECTOR_SCACHE_READ(*scache,
                                  collector->collector_info.eth.dst_mac[j],
                                  uint8);
        }

        /* Src mac addr */
        for (j = 0; j < 6; j++) {
            COLLECTOR_SCACHE_READ(*scache,
                                  collector->collector_info.eth.src_mac[j],
                                  uint8);
        }

        /* Vlan tag structure */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.eth.vlan_tag_structure,
                              uint8);

        /* Outer TPID */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.eth.outer_tpid, uint16);

        /* Inner TPID */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.eth.inner_tpid, uint16);

        /* Outer vlan tag */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.eth.outer_vlan_tag,
                              uint32);

        /* Inner vlan tag */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.eth.inner_vlan_tag,
                              uint32);

        if(collector->collector_info.transport_type ==
                       bcmCollectorTransportTypeIpv4Udp) {
            /* Src IP */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv4.src_ip, uint32);

            /* Dst IP */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv4.dst_ip, uint32);

            /* DSCP */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv4.dscp, uint8);

            /* TTL */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv4.ttl, uint8);
        }
        else
        {
            /* Src IP */
            for (j = 0; j < 16; j++) {
                COLLECTOR_SCACHE_READ(*scache,
                        collector->collector_info.ipv6.src_ip[j],
                        uint8);
            }

            /* Dst IP */
            for (j = 0; j < 16; j++) {
                COLLECTOR_SCACHE_READ(*scache,
                        collector->collector_info.ipv6.dst_ip[j],
                        uint8);
            }

            /* Traffic class */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv6.traffic_class, uint8);
            /* Flow label */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv6.flow_label, uint32);
            /* Hop limit */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipv6.hop_limit, uint8);
        }

        /* Src port */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.udp.src_port, uint16);

        /* Dst port */
        COLLECTOR_SCACHE_READ(*scache,
                              collector->collector_info.udp.dst_port, uint16);

    }

    for (i = 0;
         i < export_profile_alloc_count; i++) {


        /* Collector export profile Id */
        COLLECTOR_SCACHE_READ(*scache,
                              collector_export_profile_id, uint16);
        if (collector_export_profile_id >
                   BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID)
        {
            return BCM_E_INTERNAL;
        }
        export_profile = 
             &(export_profiles_info_list[unit][collector_export_profile_id]);

        export_profile->export_profile_id = collector_export_profile_id;

        tmp_id = collector_export_profile_id;
        rv = shr_idxres_list_reserve(
                            collector_per_unit_global_info->export_profile_pool,
                            tmp_id, tmp_id);
        BCM_IF_ERROR_RETURN(rv);

        /* Collector export profile wire format */
        COLLECTOR_SCACHE_READ(*scache,
                   export_profile->export_profile_info.wire_format, uint32);
 
        /* Collector export profile export interval */
        COLLECTOR_SCACHE_READ(*scache,
                   export_profile->export_profile_info.export_interval, uint32);
        
        /* Collector export profile max packet length */
        COLLECTOR_SCACHE_READ(*scache,
                 export_profile->export_profile_info.max_packet_length, uint32);
     }   

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_1) {
        /* Scache v1 recovery */
        for (i = 0; i< collector_alloc_count; i++) {

            /* Collector Id */
            COLLECTOR_SCACHE_READ(*scache, collector_id, uint16);
            if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID)
                {
                    LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                              (BSL_META_U(unit,
                                          "COLLECTOR(unit %d) Info: Recoved collector Id"
                                          " %d out of range"), unit,
                               collector_id));
                    return BCM_E_INTERNAL;
                }
            collector = &(collectors_info_list[unit][collector_id]);

            /* Flags */
            COLLECTOR_SCACHE_READ(*scache, flags, uint32);

            if (flags & _BCM_COLLECTOR_WB_FLAGS_L4_CHECKSUM_ENABLE) {
                collector->collector_info.udp.flags |= BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE;
            }
        }

        for (i = 0; i < export_profile_alloc_count; i++) {
            /* Collector export profile Id */
            COLLECTOR_SCACHE_READ(*scache,
                                  collector_export_profile_id, uint16);
            if (collector_export_profile_id > BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID) {
                    return BCM_E_INTERNAL;
            }
            export_profile =
                &(export_profiles_info_list[unit][collector_export_profile_id]);
            COLLECTOR_SCACHE_READ(*scache,
                                  export_profile->export_profile_info.flags,
                                  uint32);
            if (export_profile->export_profile_info.flags &
                           BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
                /* If the num_records flag is set, set copy num_records from
                 * max_packet_length as we would written num_records into it while
                 * syncing
                 */
                export_profile->export_profile_info.num_records =
                           export_profile->export_profile_info.max_packet_length;
                export_profile->export_profile_info.max_packet_length = 0;
            }
        }

    }

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_2) {
        /* Scache v2 recovery */
        for (i = 0; i< collector_alloc_count; i++) {

            /* Collector Id */
            COLLECTOR_SCACHE_READ(*scache, collector_id, uint16);
            if (collector_id > BCM_INT_COLLECTOR_COLLECTOR_END_ID)
            {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                        (BSL_META_U(unit,
                                    "COLLECTOR(unit %d) Info: Recoved collector Id"
                                    " %d out of range"), unit,
                         collector_id));
                return BCM_E_INTERNAL;
            }
            collector = &(collectors_info_list[unit][collector_id]);

            /* Ipfix version */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipfix.version, int);

            /* Ipfix Initial sequence number */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.ipfix.initial_sequence_num, int);
        }
    }

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_3) {
        /* Scache v3 recovery */
        for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID, recovered_count = 0;
                ((i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID) &&
                 (recovered_count < collector_alloc_count)); i++) {

            /* Collector Id */
            collector = &(collectors_info_list[unit][i]);

            if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
                /* Collector does not exist, continue */
                continue;
            }

            /* System Id length */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.protobuf.system_id_length, int);

            /* System Id */
            for (j = 0; j < collector->collector_info.protobuf.system_id_length; j++) {
                COLLECTOR_SCACHE_READ(*scache,
                        collector->collector_info.protobuf.system_id[j],
                        uint8);
            }

            /* Component Id */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.protobuf.component_id, uint32);

            recovered_count++;
        }
    }

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_4) {
        /* Scache v4 recovery */
        for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID, recovered_count = 0;
                ((i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID) &&
                 (recovered_count < collector_alloc_count)); i++) {

            /* Collector Id */
            collector = &(collectors_info_list[unit][i]);

            if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
                /* Collector does not exist, continue */
                continue;
            }

            /* Collector Type */
            COLLECTOR_SCACHE_READ(*scache,
                    collector->collector_info.collector_type, int);

            recovered_count++;
        }
    }

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_5) {
        /* Scache v5 recovery */
        for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID, recovered_count = 0;
                ((i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID) &&
                 (recovered_count < collector_alloc_count)); i++) {

            /* Collector Id */
            collector = &(collectors_info_list[unit][i]);

            if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
                /* Collector does not exist, continue */
                continue;
            }

            /* Collector Type */
            COLLECTOR_SCACHE_READ(*scache,
                                  collector->collector_info.src_ts,
                                  bcm_collector_timestamp_source_t);

            recovered_count++;
        }
    }

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_6) {
        /* Scache v6 recovery */
        for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID, recovered_count = 0;
                ((i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID) &&
                 (recovered_count < collector_alloc_count)); i++) {

            /* Collector Id */
            collector = &(collectors_info_list[unit][i]);

            if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
                /* Collector does not exist, continue */
                continue;
            }

            /* Collector max number of FIFO records to reserve */
            COLLECTOR_SCACHE_READ(*scache,
                                  collector->collector_info.max_records_reserve,
                                  int);
            /* Collector IPFIX observation domain ID */
            COLLECTOR_SCACHE_READ(*scache,
                          collector->collector_info.ipfix.observation_domain_id,
                          uint32);

            recovered_count++;
        }
    }

    if (recovered_ver >= BCM_WB_COLLECTOR_VERSION_1_7) {
        /* Scache v7 recovery */
        for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID, recovered_count = 0;
                ((i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID) &&
                 (recovered_count < collector_alloc_count)); i++) {

            /* Collector Id */
            collector = &(collectors_info_list[unit][i]);

            if (collector->collector_id == BCM_COLLECTOR_INVALID_INDEX) {
                /* Collector does not exist, continue */
                continue;
            }

            /* Collector IPFIX enterprise number */
            COLLECTOR_SCACHE_READ(*scache,
                          collector->collector_info.ipfix.enterprise_number,
                          uint32);

            recovered_count++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_collector_scache_size_get
 * Purpose:
 *     Get the size required to sync collector data to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_collector_scache_size_get(int unit)
{
    uint32 collecter_total_size = 0;
    uint32 collecter_info_size = 0;
    uint32 collecter_export_profile_info_size = 0;


    /*******************/
    /* Scache v0 size  */
    /*******************/

    /* Max collectors configured */
    collecter_total_size += sizeof(uint32);

    /* Max collector export profiles configured */
    collecter_total_size += sizeof(uint32);

    /* Collector Id */
    collecter_info_size += sizeof(uint16);

    /* Transport type */
    collecter_info_size += sizeof(uint16);

    /* Dst mac addr */
    collecter_info_size += (sizeof(uint8) * 6);

    /* Src mac addr */
    collecter_info_size += (sizeof(uint8) * 6);

    /* Vlan tag structure */
    collecter_info_size += sizeof(uint8);

    /* Outer TPID */
    collecter_info_size += sizeof(uint16);

    /* Inner TPID */
    collecter_info_size += sizeof(uint16);

    /* Outer vlan tag */
    collecter_info_size += sizeof(uint32);

    /* Inner vlan tag */
    collecter_info_size += sizeof(uint32);

    /*Consider IPv6 encap which can cover IPv4 as well*/
    /* Src IP */
    collecter_info_size += (sizeof(uint8) * 16);

    /* Dst IP */
    collecter_info_size += (sizeof(uint8) * 16);

    /* Traffic class */
    collecter_info_size += sizeof(uint8);

    /* Flow label */
    collecter_info_size += sizeof(uint32);

    /* Hop limit */
    collecter_info_size += sizeof(uint16);

    /* Dst port */
    collecter_info_size += sizeof(uint16);

    /* Multiply by number of collectors */
    collecter_info_size *= BCM_INT_COLLECTOR_MAX_COLLECTORS;

    /* Collector export profile Id */
    collecter_export_profile_info_size += sizeof(uint16);

    /* Collector export profile wire format */
    collecter_export_profile_info_size += sizeof(uint32);

    /* Collector export profile interval */
    collecter_export_profile_info_size += sizeof(uint32);

    /* Collector export profile max packet size */
    collecter_export_profile_info_size += sizeof(uint32);

    /* Multiply by number of collector export profiles */
    collecter_export_profile_info_size *= BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES;

    collecter_total_size += (collecter_info_size +
                             collecter_export_profile_info_size); 


    /*******************/
    /* Scache v1 size  */
    /*******************/
    /* Collector Id */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(uint16));

    /* Collector flags */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(uint32));

    /* Export profile Id */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES * sizeof(uint16));

    /* Export profile flags */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES * sizeof(uint32));

    /*******************/
    /* Scache v2 size  */
    /*******************/
    /* Collector Id */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(uint16));

    /* ipfix version */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(int));

    /* ipfix initial sequence number */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(int));

    /*******************/
    /* Scache v3 size  */
    /*******************/
    /* System Id length */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(int));

    /* System Id */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS *
                             BCM_COLLECTOR_PROTOBUF_MAX_SYSTEM_ID_LENGTH *
                             sizeof(uint8));

    /* Component Id */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(uint32));

    /*******************/
    /* Scache v4 size  */
    /*******************/
    /* Collector Type */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(int));

    /*******************/
    /* Scache v5 size  */
    /*******************/
    /* Collector Timestamp source Type */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS *
                                    sizeof(bcm_collector_timestamp_source_t));

    /*******************/
    /* Scache v6 size  */
    /*******************/
    /* Collector max number of FIFO records to reserve */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(int));

    /* Collector IPFIX observation domain ID */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(uint32));

    /*******************/
    /* Scache v7 size  */
    /*******************/
    /* Collector IPFIX enterprise number */
    collecter_total_size += (BCM_INT_COLLECTOR_MAX_COLLECTORS * sizeof(uint32));

    return collecter_total_size;
}

/*
 * Function:
 *      _bcm_collector_scache_alloc
 * Purpose:
 *      Collector WB scache alloc
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_collector_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_collector_scache_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COLLECTOR, 0);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                               &scache, BCM_WB_COLLECTOR_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_collector_wb_recover
 * Purpose:
 *      Collector WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_collector_wb_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COLLECTOR, 0);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_COLLECTOR_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_collector_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_collector_recover(unit, recovered_ver, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_collector_scache_alloc(unit, 0));

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_xgs5_collector_detach
 * Purpose:
 *      Detaches the Collector module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_detach(int unit)
{
    _bcm_collector_global_info_t *collector_info_per_unit =
                           COLLECTOR_GLOBAL_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (collector_info_per_unit->collector_pool != NULL) {
        rv = shr_idxres_list_destroy(collector_info_per_unit->collector_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        collector_info_per_unit->collector_pool = NULL;
    }
    
    /* Free up collector list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_list_destroy(unit));

    if (collector_info_per_unit->export_profile_pool != NULL) {
        rv = shr_idxres_list_destroy(
                         collector_info_per_unit->export_profile_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        collector_info_per_unit->export_profile_pool = NULL;
    }
    /* Free up collector export profile list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_export_profile_list_destroy(unit));

    sal_free(collector_info_per_unit);
    collector_global_info[unit] = NULL;

    return BCM_E_NONE; 
}

/*
 * Function:
 *      _bcm_xgs5_collector_init
 * Purpose:
 *      Initializes the Collector module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_collector_init(int unit)
{
     _bcm_collector_global_info_t *collector_info_per_unit =
                             COLLECTOR_GLOBAL_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (collector_info_per_unit != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_detach(unit));
        collector_info_per_unit = NULL;
    }

    _BCM_COLLECTOR_ALLOC(collector_info_per_unit, _bcm_collector_global_info_t,
                 sizeof(_bcm_collector_global_info_t), "COLLECTOR_INFO_PER_UNIT");
    if (collector_info_per_unit == NULL) {
        return BCM_E_MEMORY;
    }
    collector_global_info[unit] = collector_info_per_unit;

    /* Create collector list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_list_create(unit));
    rv = shr_idxres_list_create(&(collector_info_per_unit->collector_pool),
                                BCM_INT_COLLECTOR_COLLECTOR_START_ID,
                                BCM_INT_COLLECTOR_COLLECTOR_END_ID,
                                BCM_INT_COLLECTOR_COLLECTOR_START_ID,
                                BCM_INT_COLLECTOR_COLLECTOR_END_ID,
                                "collector IDs pool");

    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_detach(unit));
        return rv;
    }
    /* Create collector export profile list */
    rv = _bcm_xgs5_collector_export_profile_list_create(unit);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_detach(unit));
        return rv;
    }
    rv = shr_idxres_list_create(&(collector_info_per_unit->export_profile_pool),
                                BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID,
                                BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID,
                                BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID,
                                BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID,
                                "collector export profile IDs pool");
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_collector_detach(unit));
        return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_collector_wb_recover(unit);
    } else {
        rv = _bcm_collector_scache_alloc(unit, 1);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (rv == BCM_E_NOT_FOUND) {
        /* Continue with initialization if scache not found */
        rv = BCM_E_NONE;
    }
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    return BCM_E_NONE;
}

void
_bcm_xgs5_collector_encap_l2_dump(
                    bcm_collector_eth_header_t *eth)
{
   LOG_CLI((BSL_META("\t\tDST_MAC = %x:%x:%x:%x:%x:%x\r\n"),
                    eth->dst_mac[0],
                    eth->dst_mac[1],
                    eth->dst_mac[2],
                    eth->dst_mac[3],
                    eth->dst_mac[4],
                    eth->dst_mac[5])); 
   LOG_CLI((BSL_META("\t\tSRC_MAC = %x:%x:%x:%x:%x:%x\r\n"),
                    eth->src_mac[0],
                    eth->src_mac[1],
                    eth->src_mac[2],
                    eth->src_mac[3],
                    eth->src_mac[4],
                    eth->src_mac[5])); 
     LOG_CLI((BSL_META("\t\tVlan tag structure = %d\r\n"),
                    eth->vlan_tag_structure));
     LOG_CLI((BSL_META("\t\touter_tpid = %u\r\n"),
                    eth->outer_tpid));
     LOG_CLI((BSL_META("\t\tinner_tpid = %u\r\n"),
                    eth->inner_tpid));
     LOG_CLI((BSL_META("\t\touter_vlan_tag = %u\r\n"),
                    eth->outer_vlan_tag));
     LOG_CLI((BSL_META("\t\tinner_vlan_tag = %u\r\n"),
                    eth->inner_vlan_tag));
}

void
_bcm_xgs5_collector_encap_ipv4_dump(
        bcm_collector_ipv4_header_t *ipv4)
{
     LOG_CLI((BSL_META("\t\tsrc_ip = %x\r\n"),
                    ipv4->src_ip));
     LOG_CLI((BSL_META("\t\tdst_ip = %x\r\n"),
                    ipv4->dst_ip));
     LOG_CLI((BSL_META("\t\tdscp = %u\r\n"),
                    ipv4->dscp));
     LOG_CLI((BSL_META("\t\tttl = %u\r\n"),
                    ipv4->ttl));
}

void
_bcm_xgs5_collector_encap_ipv6_dump(
        bcm_collector_ipv6_header_t *ipv6)
{
    LOG_CLI((BSL_META("\t\tsrc_ip = %x:%x:%x:%x:%x:%x%x:%x:%x:%x:%x:%x%x:%x:%x:%x\r\n"),
                ipv6->src_ip[0],
                ipv6->src_ip[1],
                ipv6->src_ip[2],
                ipv6->src_ip[3],
                ipv6->src_ip[4],
                ipv6->src_ip[5],
                ipv6->src_ip[6],
                ipv6->src_ip[7],
                ipv6->src_ip[8],
                ipv6->src_ip[9],
                ipv6->src_ip[10],
                ipv6->src_ip[11],
                ipv6->src_ip[12],
                ipv6->src_ip[13],
                ipv6->src_ip[14],
                ipv6->src_ip[15]));
     LOG_CLI((BSL_META("\t\tdst_ip = %x:%x:%x:%x:%x:%x%x:%x:%x:%x:%x:%x%x:%x:%x:%x\r\n"),
                ipv6->dst_ip[0],
                ipv6->dst_ip[1],
                ipv6->dst_ip[2],
                ipv6->dst_ip[3],
                ipv6->dst_ip[4],
                ipv6->dst_ip[5],
                ipv6->dst_ip[6],
                ipv6->dst_ip[7],
                ipv6->dst_ip[8],
                ipv6->dst_ip[9],
                ipv6->dst_ip[10],
                ipv6->dst_ip[11],
                ipv6->dst_ip[12],
                ipv6->dst_ip[13],
                ipv6->dst_ip[14],
                ipv6->dst_ip[15]));

    LOG_CLI((BSL_META("\t\ttraffic_class = %u\r\n"),
                ipv6->traffic_class));
    LOG_CLI((BSL_META("\t\tflow_label = %u\r\n"),
                ipv6->flow_label));
    LOG_CLI((BSL_META("\t\thop_limit = %u\r\n"),
                ipv6->hop_limit));
}

void
_bcm_xgs5_collector_encap_udp_dump(
        bcm_collector_udp_header_t *udp)
{
     LOG_CLI((BSL_META("\t\tDst L4 port = %d\r\n"),
                    udp->dst_port));
     LOG_CLI((BSL_META("\t\tSrc L4 port = %d\r\n"),
                    udp->src_port));
}

void
_bcm_xgs5_collector_encap_ipfix_dump(
        bcm_collector_ipfix_header_t *ipfix)
{
     LOG_CLI((BSL_META("\t\tIPFIX version = 0x%x\r\n"),
                    ipfix->version));
     LOG_CLI((BSL_META("\t\tinitial sequence number = 0x%x\r\n"),
                    ipfix->initial_sequence_num));
     LOG_CLI((BSL_META("\t\tObservation domain ID = 0x%x\r\n"),
                    ipfix->observation_domain_id));
     LOG_CLI((BSL_META("\t\tEnterprise number = 0x%x\r\n"),
                    ipfix->enterprise_number));
}

char *_bcm_xgs5_collector_transport_type_str[bcmCollectorTransportTypeCount] =
{
    "IPv4 UDP",
    "IPv6 UDP",
    "Raw"
};

/*
 * Function:
 *     _bcm_xgs5_collector_dump
 * Purpose:
 *     Dump the collector info.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_node - (IN) Collector information.
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_collector_dump ( 
    _bcm_collector_info_t *collector_node)
{
     LOG_CLI((BSL_META("Collector ID = %d\r\n"),collector_node->collector_id));
     LOG_CLI((BSL_META("\tTransport type = %s\r\n"),
  _bcm_xgs5_collector_transport_type_str[collector_node->collector_info.transport_type]));
     LOG_CLI((BSL_META("\tEncap info:\r\n")));
     _bcm_xgs5_collector_encap_l2_dump(&(collector_node->collector_info.eth));
    if (collector_node->collector_info.transport_type ==
        bcmCollectorTransportTypeIpv4Udp) {
     _bcm_xgs5_collector_encap_ipv4_dump(
             &(collector_node->collector_info.ipv4));
    } else {
     _bcm_xgs5_collector_encap_ipv6_dump(
             &(collector_node->collector_info.ipv6));
    }
     _bcm_xgs5_collector_encap_udp_dump(&(collector_node->collector_info.udp));
     _bcm_xgs5_collector_encap_ipfix_dump(&(collector_node->collector_info.ipfix));
}

/*
 * Function:
 *     _bcm_xgs5_collector_list_dump
 * Purpose:
 *     Dump the collector list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_collector_list_dump (int unit)
{
    _bcm_collector_info_t *collector_node = NULL;
    int i;

    if (collectors_info_list[unit] == NULL) {
        return;
    }

    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID; i++) {
        collector_node = &(collectors_info_list[unit][i]);
        if (collector_node->collector_id !=
                BCM_COLLECTOR_INVALID_INDEX) {
            _bcm_xgs5_collector_dump(collector_node);
        }
    } 
}

char *_bcm_xgs5_wire_format_str[bcmCollectorWireFormatCount] =
{
    "Ipfix",
    "ProtoBuf",
};

/*
 * Function:
 *     _bcm_xgs5_export_profile_dump
 * Purpose:
 *     Dump the collector export profile info.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     export_profile_node - (IN) Collector export profile information.
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_export_profile_dump ( 
    _bcm_collector_export_profile_info_t *export_profile_node)
{
     LOG_CLI((BSL_META("Collector export profile ID = %d\r\n"),
                       export_profile_node->export_profile_id));
     LOG_CLI((BSL_META("\tWire format = %s\r\n"),
                       _bcm_xgs5_wire_format_str
                       [export_profile_node->export_profile_info.wire_format]));
     LOG_CLI((BSL_META("\texport interval = %u\r\n"), 
                   export_profile_node->export_profile_info.export_interval));
     LOG_CLI((BSL_META("\tMax packet length = %u\r\n"), 
                   export_profile_node->export_profile_info.max_packet_length));
}

/*
 * Function:
 *     _bcm_xgs5_collector_export_profile_list_dump
 * Purpose:
 *     Dump the collector export profile list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     Nothing
 */
void
_bcm_xgs5_collector_export_profile_list_dump (int unit)
{
    _bcm_collector_export_profile_info_t *export_profile_node = NULL;
    int i;

    if (export_profiles_info_list[unit] == NULL) {
        return;
    }

    for (i = BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i <= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID; i++) {
        export_profile_node = &(export_profiles_info_list[unit][i]);
        if (export_profile_node->export_profile_id !=
                BCM_COLLECTOR_INVALID_INDEX) {
            _bcm_xgs5_export_profile_dump(export_profile_node);
        }
    } 
}

/* Dump routine for dumping
 * collector list and export profile list.
 */
void _bcm_xgs5_collector_sw_dump(int unit)
{
    _bcm_xgs5_collector_list_dump(unit);
    _bcm_xgs5_collector_export_profile_list_dump(unit);
}

#else /* BCM_COLLECTOR_SUPPORT */
typedef int _bcm_xgs5_collector_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_COLLECTOR_SUPPORT */
