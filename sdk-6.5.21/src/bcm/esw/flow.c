/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flow.c
 * Purpose:     Flex flow match APIs.
 */

#if defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache.h>

#include <shared/bsl.h>
#include <assert.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/flow.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      bcm_esw_flow_vpn_create
 * Purpose:
 *      Create an L2/L3 VPN.  This is a service plane and is
 *      independent of the connectivity protocol.
 *   Parameters:
 *      unit        - (IN)  Unit ID.
 *      info    - (IN/OUT)  VPN structure
 */
int bcm_esw_flow_vpn_create(int unit,
                            bcm_vpn_t *vpn,
                            bcm_flow_vpn_config_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
         rv = bcmi_esw_flow_lock(unit);
         if (rv == BCM_E_NONE ) {
              rv = bcmi_esw_flow_vpn_create(unit, vpn, info);
              bcmi_esw_flow_unlock (unit);
         }    
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *      bcm_esw_flow_vpn_destroy
 * Purpose:
 *      Delete L2/L3 VPN
 *   Parameters:
 *      unit   - (IN)  Unit ID.
 *      vpn    - (IN)  VPN Id
 */
int bcm_esw_flow_vpn_destroy(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        rv = bcmi_esw_flow_lock(unit);
        if (rv == BCM_E_NONE ) {
            rv = bcmi_esw_flow_vpn_destroy(unit, vpn);
            bcmi_esw_flow_unlock (unit);
        }    
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *      bcm_esw_flow_vpn_destroy_all
 * Purpose:
 *      Delete all VPN's
 *   Parameters:
 *      unit   - (IN)  Unit ID.
 */
int bcm_esw_flow_vpn_destroy_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        rv = bcmi_esw_flow_lock(unit);
        if (rv == BCM_E_NONE ) {
            rv = bcmi_esw_flow_vpn_destroy_all(unit);
            bcmi_esw_flow_unlock (unit);
        }    
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *      bcm_esw_flow_vpn_get
 * Purpose:
 *      Get VPN properties
 *   Parameters:
 *      unit   - (IN)  Unit ID.
 *      vpn    - (IN)  VPN Id
 *      info    - (IN/OUT)  VPN structure
 */
int bcm_esw_flow_vpn_get(int unit, bcm_vpn_t vpn,
                         bcm_flow_vpn_config_t *info)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        rv = bcmi_esw_flow_lock(unit);
        if (rv == BCM_E_NONE ) {
            rv = bcmi_esw_flow_vpn_get(unit, vpn, info);
            bcmi_esw_flow_unlock (unit);
        }    
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *      bcm_esw_flow_vpn_traverse
 * Purpose:
 *      Traverse VPN's
 *   Parameters:
 *      unit      - (IN)  Unit ID.
 *      cb        - (IN)  callback function
 *      user_data -  (IN) User context data
 */
int bcm_esw_flow_vpn_traverse(int unit,
                              bcm_flow_vpn_traverse_cb cb,
                              void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        rv = bcmi_esw_flow_lock(unit);
        if (rv == BCM_E_NONE ) {
            rv = bcmi_esw_flow_vpn_traverse(unit, cb, user_data);
            bcmi_esw_flow_unlock (unit);
        }    
    }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

    return rv;
}

/*
 * Function:
 *      bcm_esw_flow_match_add
 * Description:
 *      Add a match rule to identify traffic flow with certain objects. 
 * These objects generally will be used in the switchs forwarding process. 
 * The matching rules comprise of packet fields and/or the port number the 
 * packet comes in.  The identified objects can be VFI,virtual port, interface 
 * id,  flexible objects specified by logical fields, or a combination of them.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) Match info structure 
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array 
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_match_add(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_match_add(unit, info, num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_match_delete
 * Description:
 *      Delete a match rule for the given match_criteria. For the non-flex
 * match criteria, the logical field array is not used.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) Match info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_match_delete(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_match_delete(unit, info, num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_match_get
 * Description:
 *      Get the match objects and attributes for the given match_criteria.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN/OUT) Match info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN/OUT) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_match_get(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_match_get(unit, info, num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_match_traverse
 * Description:
 *      Traverse match rules.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) Match info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 *    cb             (IN) user callback function
 *    user_data      (IN) User context data
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_match_traverse(
    int unit,
    bcm_flow_match_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_match_traverse(unit,cb,user_data);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}
/*
 * Function:
 *      bcm_esw_flow_tunnel_initiator_create
 * Description:
 *      creates a tunnel header for packet encapsulation
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) tunnel config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_tunnel_initiator_create(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_initiator_create(unit, info, 
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_tunnel_initiator_destroy
 * Description:
 *      delete a tunnel header entry
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    flow_tunnel_id (IN) tunnel config info structure
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */ 

int bcm_esw_flow_tunnel_initiator_destroy(
    int unit,
    bcm_gport_t flow_tunnel_id)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_initiator_destroy(unit,flow_tunnel_id); 
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_tunnel_initiator_get
 * Description:
 *      Get a tunnel header entry
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) tunnel config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_tunnel_initiator_get(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_initiator_get(unit, info,
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_tunnel_initiator_traverse
 * Description:
 *      Get a tunnel header entry
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) tunnel config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_tunnel_initiator_traverse(
    int unit,
    bcm_flow_tunnel_initiator_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_initiator_traverse(unit, 
                  cb, user_data);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_tunnel_terminator_create
 * Description:
 *      creates a flow tunnel terminator match
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) tunnel config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_flow_tunnel_terminator_create(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_terminator_create(unit, info,
                    num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_tunnel_terminator_destroy
 * Description:
 *      delete a tunnel termination entry
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) tunnel config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_flow_tunnel_terminator_destroy(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_terminator_destroy(unit,
               info, num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_tunnel_terminator_get
 * Description:
 *      Get a tunnel termination entry
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) tunnel config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_flow_tunnel_terminator_get(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_terminator_get(unit, 
               info, num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}
/*
 * Function:
 *      bcm_esw_flow_tunnel_terminator_traverse
 * Description:
 *      Get a tunnel termination entry
 *
 * Parameters:
 *    Unit           (IN)     Unit number
 *    cb             (IN)     Traverse function
 *    user_data      (IN/OUT) User Data
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_flow_tunnel_terminator_traverse(
    int unit,
    bcm_flow_tunnel_terminator_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_tunnel_terminator_traverse(unit, 
               cb, user_data);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_port_encap_set
 * Description:
 *      For given DVP, bind the egress object and tunnel initiator with 
 * the DVP and program DVP related encap data for L2 tunnel. 
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) egress encap info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_port_encap_set(
    int unit,
    bcm_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_port_encap_set(unit, info,
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_port_encap_get
 * Description:
 *      Get the encap configuration info for the given DVP  
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN/OUT) egress encap info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN/OUT) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_port_encap_get(
    int unit,
    bcm_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_port_encap_get(unit, info,
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_encap_add
 * Description:
 *      add the object(key) based encapsulation data to the packet. The objects
 * can be VP,VFI,VRF,interface ID, specified by logical fields, or combination 
 * of them. The encapsulation data comprises of fields in the 
 * bcm_flow_encap_config_t and/or logical fields 
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) encap config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_encap_add(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_encap_add(unit, info,
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_encap_delete
 * Description:
 *      Delete the object(key) based encapsulation data. 
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) encap config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_encap_delete(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_encap_delete(unit, info,
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_encap_get
 * Description:
 *      Get the object(key) based encapsulation data.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN/OUT) encap config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN/OUT) logical field array
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_encap_get(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_encap_get(unit, info,
                  num_of_fields, field);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_encap_traverse
 * Description:
 *      Traverse the object(key) based encapsulation data entries.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    info           (IN) encap config info structure
 *    num_of_fields  (IN) Number of logical fields
 *    field          (IN) logical field array
 *    cb             (IN) user callback function
 *    user_data      (IN) user context data
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_encap_traverse(
    int unit,
    bcm_flow_encap_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return bcmi_esw_flow_encap_traverse(unit,cb,user_data);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *    bcm_esw_flow_init
 * Purpose:
 *    Init  FLOW module
 * Parameters:
 *    IN :  unit
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_esw_flow_init(int unit)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
        return bcmi_esw_flow_init(unit);
    }    
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}

 /* Function:
 *      bcm_esw_flow_cleanup
 * Purpose:
 *      Detach the FLOW module, clear all HW states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_flow_cleanup(int unit)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
        return bcmi_esw_flow_cleanup(unit);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_flow_port_create
 * Purpose:
 *      Create and add a Access/Network Flow port to L2 VPN
 *   Parameters:
 *      unit        - (IN)  Unit ID.
 *      info    - (IN/OUT)  VP properties 
 */
int
bcm_esw_flow_port_create(int unit,
                        bcm_vpn_t l2vpn,
                        bcm_flow_port_t *flow_port)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
         if (flow_port == NULL) {
            return BCM_E_PARAM;
         }    
         rv = bcmi_esw_flow_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcmi_esw_flow_port_create(unit, l2vpn, flow_port);
              bcmi_esw_flow_unlock (unit);
         }    
    }    
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_flow_port_destroy
 * Purpose:
 *      Destroy Access/Network FLOW port
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - FLOW VPN
 *      flow_port_id     - FLOW Gport Id
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_flow_port_destroy( int unit, bcm_vpn_t l2vpn, bcm_gport_t flow_port_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
         rv = bcmi_esw_flow_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcmi_esw_flow_port_destroy(unit, l2vpn, flow_port_id);
              bcmi_esw_flow_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_flow_port_get
 * Purpose:
 *      Get Access/Network FLOW port info
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - FLOW VPN
 *      flow_port     - FLOW Gport
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_flow_port_get(int unit, bcm_vpn_t l2vpn, bcm_flow_port_t *flow_port)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
         if (flow_port == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcmi_esw_flow_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcmi_esw_flow_port_get(unit, l2vpn, flow_port);
              bcmi_esw_flow_unlock (unit);
         }
    }
#endif
    return rv;
}

 /* Function:
 *      bcm_esw_flow_port_get_all
 * Purpose:
 *      Get all Access/Network FLOW port info
 * Parameters:
 *      unit     - (IN) Device Number
 *      l2vpn   - FLOW VPN
 *      port_max   - (IN) Maximum number of FLOW ports in array
 *      port_array - (OUT) Array of FLOW ports
 *      port_count - (OUT) Number of FLOW ports returned in array
 * Returns:
 *      BCM_E_XXXX
 */

int 
bcm_esw_flow_port_get_all(
    int unit, 
    bcm_vpn_t l2vpn, 
    int port_max, 
    bcm_flow_port_t *port_array, 
    int *port_count)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow)) {
         if (port_count == NULL) {
            return BCM_E_PARAM;
         }
         rv = bcmi_esw_flow_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv = bcmi_esw_flow_port_get_all(unit, l2vpn, port_max, port_array, port_count);
              bcmi_esw_flow_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_flow_handle_get
 * Description:
 *      Get the handle for the flow name.
 * Parameters:
 *    Unit           (IN)     Unit number.
 *    flow_name      (IN)     Flow Name.
 *    handle         (IN/OUT) Flow Handle.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_handle_get(
    int unit,
    const char *flow_name,
    bcm_flow_handle_t *handle)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return soc_flow_db_flow_handle_get(unit, flow_name, handle);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_option_id_get
 * Description:
 *      Get the option id for flow option name.
 * Parameters:
 *    Unit              (IN)     Unit number.
 *    handle            (IN)     Flow Handle.
 *    flow_option_name  (IN)     Flow Option Name.
 *    option_id         (IN/OUT) Flow Option ID.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_option_id_get(
    int unit,
    bcm_flow_handle_t flow_handle,
    const char *flow_option_name,
    bcm_flow_option_id_t *option_id)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return soc_flow_db_flow_option_id_get(unit, flow_handle,
                                    flow_option_name,
                                    option_id);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_field_id_get
 * Description:
 *      Get the field id for logical field name in a flow.
 * Parameters:
 *    Unit                (IN)     Unit number.
 *    flow_handle         (IN)     Flow Handle.
 *    flow_option_name    (IN)     Flow Option Name.
 *    option_id           (IN/OUT) Flow Option ID.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_flow_logical_field_id_get(
    int unit,
    bcm_flow_handle_t flow_handle,
    const char *field_name,
    bcm_flow_field_id_t *field_id)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        return soc_flow_db_mem_view_logical_field_id_get(unit, flow_handle,
                                                         field_name,
                                                         field_id);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
         return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_stat_object_get
 * Purpose:
 *      Get the accounting stat object associated
 *          with the flex view of the  table.
 * Parameters:
 *      unit                  - (IN)     unit number.
 *      flow_handle           - (IN)     flow handle.
 *      flow_option_id        - (IN)     flow option id.
 *      flow_function_type    - (IN)     flow function type.
 *      stat object           - (IN/OUT) Stat accounting object.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int bcm_esw_flow_stat_object_get(int unit,
                          bcm_flow_handle_t flow_handle,
                          bcm_flow_option_id_t flow_option_id,
                          bcm_flow_function_type_t function_type,
                          bcm_stat_object_t *stat_object)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {

        if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return BCM_E_UNAVAIL;
        }
        return bcmi_esw_flow_stat_object_get(unit, flow_handle,
                                 flow_option_id, function_type,
                                  stat_object);

    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_stat_attach
 * Purpose:
 *      Attach counter entries to the given flex view table.
 * Parameters:
 *      unit                - (IN)     unit number.
 *      flow_stat_info      - (IN)     flow stat config structure.
 *      num_of_fields       - (IN)     number of logical fields.
 *      field               - (IN)     logical fields.
 *      stat_counter_id     - (IN)     stat counter id.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int bcm_esw_flow_stat_attach(int unit,
                          bcm_flow_stat_info_t *flow_stat_info,
                          uint32 num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32 stat_counter_id)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return BCM_E_UNAVAIL;
        }
        if (NULL == flow_stat_info) {
            return BCM_E_PARAM;
        }
        return bcmi_esw_flow_stat_attach(unit, flow_stat_info,
                                         num_of_fields, field,
                                         stat_counter_id);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_stat_detach
 * Purpose:
 *      Detach counter entries to the given flex view table.
 * Parameters:
 *      unit                - (IN)     unit number.
 *      flow_stat_info      - (IN)     flow stat config structure.
 *      num_of_fields       - (IN)     number of logical fields.
 *      field               - (IN)     logical fields.
 *      stat_counter_id     - (IN)     stat counter id.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int bcm_esw_flow_stat_detach(int unit,
                          bcm_flow_stat_info_t *flow_stat_info,
                          uint32 num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32 stat_counter_id)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return BCM_E_UNAVAIL;
        }
        if (NULL == flow_stat_info) {
            return BCM_E_PARAM;
        }
        return  bcmi_esw_flow_stat_detach(unit, flow_stat_info,
                                         num_of_fields, field, stat_counter_id);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_flow_stat_id_get
 * Purpose:
 *      Get Stat Counter if associated with flex view.
 * Parameters:
 *      unit                - (IN)     unit number.
 *      flow_stat_info      - (IN)     flow stat config structure.
 *      num_of_fields       - (IN)     number of logical fields.
 *      field               - (IN)     logical fields.
 *      stat_object         - (IN)     stat object.
 *      stat_counter_id     - (IN/OUT) stat counter id.
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 */
int bcm_esw_flow_stat_id_get(int unit,
                         bcm_flow_stat_info_t *flow_stat_info,
                         uint32 num_of_fields,
                         bcm_flow_logical_field_t *field,
                         bcm_stat_object_t stat_object,
                         uint32 *stat_counter_id)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_flow)) {
        if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
            return BCM_E_UNAVAIL;
        }
        if (NULL == flow_stat_info) {
            return BCM_E_PARAM;
        }
        if (NULL == stat_counter_id) {
            return BCM_E_PARAM;
        }
        return  bcmi_esw_flow_stat_id_get(unit, flow_stat_info,
                                         num_of_fields, field,
                                         stat_object,
                                         stat_counter_id);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      _bcm_esw_flow_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a flow virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) flow virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flow_port_source_vp_lag_set(
    int unit, bcm_gport_t gport, int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        if (!_SHR_GPORT_IS_FLOW_PORT(gport)) {
            return BCM_E_PARAM;
        }
        rv = bcmi_esw_flow_match_svp_replace(unit, gport, vp_lag_vp);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_port_source_vp_lag_clear
 * Purpose:
 *      Set source VP LAG for a flow virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) flow virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flow_port_source_vp_lag_clear(
    int unit, bcm_gport_t gport, int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    int vp;

    if (soc_feature(unit, soc_feature_flex_flow) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        if (!_SHR_GPORT_IS_FLOW_PORT(gport)) {
            return BCM_E_PARAM;
        }
        vp = _SHR_GPORT_FLOW_PORT_ID_GET(gport);

        rv = bcmi_esw_flow_match_svp_replace(unit, gport, vp);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_port_source_vp_lag_get
 * Purpose:
 *      Set source VP LAG for a flow virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) flow virtual port GPORT ID.
 *      vp_lag_vp - (IN) PTR of VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flow_port_source_vp_lag_get(
    int unit, bcm_gport_t gport, int* vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flow) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        if (!_SHR_GPORT_IS_FLOW_PORT(gport)) {
            return BCM_E_PARAM;
        }
        rv = bcmi_esw_flow_match_svp_get(unit, gport, vp_lag_vp);
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }

    return rv;
}

#else   /* INCLUDE_L3 */
typedef int bcm_esw_flow_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */


