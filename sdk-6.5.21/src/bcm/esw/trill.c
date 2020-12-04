/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW Trill API
 */

#ifdef INCLUDE_L3

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident.h>
#include <bcm/trill.h>
#include <bcm_int/esw/trill.h>

/*
 * Function:
 *    bcm_esw_trill_init
 * Purpose:
 *    Init  TRILL module
 * Parameters:
 *    IN :  unit
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_esw_trill_init(int unit)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
        return bcm_td_trill_init(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}


 /* Function:
 *      bcm_trill_cleanup
 * Purpose:
 *      Detach the TRILL module, clear all HW states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_trill_cleanup(int unit)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
        return bcm_td_trill_cleanup(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

 /*
  * Purpose:
  *     Add Trill port to TRILL Cloud
  * Parameters:
  *  unit  - (IN) Device Number
  *  trill_port - (IN/OUT) trill port information (OUT : trill_port_id)
 */
int
bcm_esw_trill_port_add(int unit, bcm_trill_port_t *trill_port)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_port_add(unit, trill_port);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}
  
 /*
  * Purpose:
  *   Delete TRILL port from TRILL Cloud
  * Parameters:
  *    unit  - (IN) Device Number
  *    trill_port_id - (IN) Trill port ID
  */
  
int 
bcm_esw_trill_port_delete(int unit, bcm_gport_t trill_port_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_port_delete(unit, trill_port_id);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
  * Purpose:
  *	  Delete all TRILL ports from TRILL Cloud
  * Parameters:
  *	  unit - Device Number
  */

int 
bcm_esw_trill_port_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_port_delete_all(unit);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
  * Purpose:
  *      Get a TRILL port 
  * Parameters:
  *      unit    - (IN) Device Number
  *      trill_port - (IN/OUT) Trill port information (IN : trill_port_id)
  */

int 
bcm_esw_trill_port_get(int unit, bcm_trill_port_t *trill_port)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_port_get(unit, trill_port);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
 * Purpose:
 *      Get all TRILL ports from TRILL Cloud
 * Parameters:
 *      unit     - (IN) Device Number
 *      port_max   - (IN) Maximum number of TRILL ports in array
 *      port_array - (OUT) Array of TRILL ports
 *      port_count - (OUT) Number of TRILL ports returned in array
 */

int 
bcm_esw_trill_port_get_all(int unit, 
                           int port_max, 
                           bcm_trill_port_t *trill_port, 
                           int *port_count )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_port_get_all(unit, port_max, trill_port, port_count);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
 * Purpose:
 *     Add TRILL multicast entry
 * Parameters:
 *     unit   - Device Number
 *     trill_mc_entry - (INOUT) Trill Multicast Entry
 */

int
bcm_esw_trill_multicast_entry_add (int unit,
    bcm_trill_multicast_entry_t *trill_mc_entry)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_multicast_add(unit, 
                                        trill_mc_entry->flags, 
                                        trill_mc_entry->root_name, 
                                        trill_mc_entry->c_vlan, 
                                        trill_mc_entry->c_dmac, 
                                        trill_mc_entry->group);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
 * Purpose:
 *        Delete TRILL multicast entry
 * Parameters:
 *        unit   - Device Number
 *     trill_mc_entry - (INOUT) Trill Multicast Entry
 */

int
bcm_esw_trill_multicast_entry_delete (int unit,
    bcm_trill_multicast_entry_t *trill_mc_entry)

{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_multicast_delete(unit,
                                        trill_mc_entry->flags, 
                                        trill_mc_entry->root_name, 
                                        trill_mc_entry->c_vlan, 
                                        trill_mc_entry->c_dmac);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
 * Purpose:
  *    Get TRILL multicast entry
  * Parameters:
  *    unit   - Device Number
 *     trill_mc_entry - (INOUT) Trill Multicast Entry
 */

int
bcm_esw_trill_multicast_entry_get( int unit, 
    bcm_trill_multicast_entry_t *trill_mc_entry)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_multicast_entry_get(unit, trill_mc_entry);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

 /*
 * Purpose:
 *    Delete all TRILL multicast entries
 * Parameters:
 *    unit   - Device Number
 *    root_name  - (IN) Trill Root_RBridge
 */

int
bcm_esw_trill_multicast_delete_all (int  unit,  bcm_trill_name_t  root_name)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_multicast_delete_all(unit, root_name);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}
 
 /*
 * Purpose:
  *    Traverse all valid TRILL Multicast entries and call the
  *     supplied callback routine.
  * Parameters:
  *    unit   - Device Number
  *    cb    - User callback function, called once per Trill Multicast entry.
  *    user_data - cookie
 */

int
bcm_esw_trill_multicast_entry_traverse( int unit, 
                                  bcm_trill_multicast_entry_traverse_cb  cb,
                                  void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =  bcm_td_trill_multicast_traverse(unit, cb, user_data);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
* Purpose:
*     Add TRILL multicast RPF entry
* Parameters:
*     unit  - Device Number
*     root_name  - (IN) Trill Root_RBridge
*     source_name - (IN) Source Rbridge
*     port   - (IN) Ingress port
*/

int bcm_esw_trill_multicast_source_add(int unit, 
                                                                            bcm_trill_name_t root_name, 
                                                                            bcm_trill_name_t source_name, 
                                                                            bcm_gport_t port,
                                                                            bcm_if_t  encap_intf)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =	bcm_td_trill_multicast_source_add(unit, root_name, 
                                                           source_name, port, encap_intf);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *    _bcm_esw_trill_sync
 * Purpose:
 *    Store Trill state to persistent memory.
 * Parameters:
 *    IN :  unit
 * Returns:
 *    BCM_E_XXX
 */

int
_bcm_esw_trill_sync(int unit)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
        return bcm_td_trill_sync(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
* Purpose:
*      Delete TRILL multicast RPF entry
* Parameters:
*        unit  - Device Number
*        root_name  - (IN) Trill Root_RBridge
*        source_name - (IN) Source Rbridge
*        port   - (IN) Ingress port
*/

int bcm_esw_trill_multicast_source_delete(int unit, 
                                                                                 bcm_trill_name_t root_name, 
                                                                                 bcm_trill_name_t source_name, 
                                                                                 bcm_gport_t port,
                                                                                 bcm_if_t  encap_intf)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =	bcm_td_trill_multicast_source_delete(unit, root_name, 
                                                                source_name, port, encap_intf);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
* Purpose:
*    Get TRILL multicast RPF entry
* Parameters:
*   unit  - Device Number
*   root_name (IN) Trill Root_RBridge
*   source_name - (IN) Source Rbridge
*   port    - (OUT) Ingress port
*/

int bcm_esw_trill_multicast_source_get(int unit, 
                                                                           bcm_trill_name_t root_name, 
                                                                           bcm_trill_name_t source_name, 
                                                                           bcm_gport_t *port)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =	bcm_td_trill_multicast_source_get(unit, root_name, 
                                                                     source_name, port);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
* Purpose:
 *  Traverse all valid TRILL Multicast RPF entries and call the
 *  supplied callback routine.
 * Parameters:
 *  unit  - Device Number
 *     cb  - User callback function, called once per Trill Multicast RPF entry.
 *     user_data - cookie
*/

int bcm_esw_trill_multicast_source_traverse(int unit, 
                                                                                     bcm_trill_multicast_source_traverse_cb cb, 
                                                                                     void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
         if ( rv == BCM_E_NONE ) {
              rv =	bcm_td_trill_multicast_source_traverse(unit, cb, user_data);
              bcm_td_trill_unlock (unit);
         }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_trill_stat_get
 * Purpose:
 *      Get TRILL Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port   - (IN) port
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_esw_trill_stat_get(int unit, bcm_port_t port,
                            bcm_trill_stat_t stat, uint64 *val)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
       if ( rv == BCM_E_NONE ) {
           rv = bcm_td_trill_stat_get(unit, port, stat, val);
           bcm_td_trill_unlock (unit);
       }
       return rv;
    }
#endif
    return rv;
}

int
bcm_esw_trill_stat_get32(int unit, bcm_port_t port,
                              bcm_trill_stat_t stat, uint32 *val)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
       if ( rv == BCM_E_NONE ) {
           rv = bcm_td_trill_stat_get32(unit, port, stat, val);
           bcm_td_trill_unlock (unit);
       }
       return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_trill_stat_clear
 * Purpose:
 *      Clear TRILL Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port   - (IN) port
 *      stat   - (IN)  specify the Stat type
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_esw_trill_stat_clear(int unit, bcm_port_t port,
                            bcm_trill_stat_t stat)
{
int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
         rv = bcm_td_trill_lock(unit);
       if ( rv == BCM_E_NONE ) {
             rv = bcm_td_trill_stat_clear(unit, port, stat);
             bcm_td_trill_unlock (unit);
         }
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_trill_trunk_member_add
 * Purpose:
 *      Set Trill state for trunk members leaving a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be added
 *     trunk_member_array - (IN) Trunk member ports to be added
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_trill_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) 
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
        rv = bcm_td_trill_trunk_member_add(unit, trunk_id,
                trunk_member_count, trunk_member_array);
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_trill_trunk_member_delete
 * Purpose:
 *      Clear Trill state for trunk members leaving a trunk group.
 * Parameters:
 *     unit  - (IN) Device Number
 *     trunk_id - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
 * Returns:
 *      BCM_E_XXX
 */     
int
_bcm_esw_trill_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) 
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(unit, soc_feature_trill)) {
        rv = bcm_td_trill_trunk_member_delete(unit, trunk_id,
                trunk_member_count, trunk_member_array);
        return rv;
    }
#endif
    return rv;
}

/*
 * Purpose:
 *      Add TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */

int
_bcm_esw_trill_multicast_entry_add(int unit, uint32 flags, int key, 
                                    bcm_vlan_t vid, bcm_mac_t c_dmac, 
                                    uint8 trill_tree_id, bcm_multicast_t group)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv =   bcm_td2_trill_l2_multicast_entry_add(unit, flags,
                       key, vid, c_dmac, trill_tree_id, group);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
         rv =   bcm_td2_trill_l2_multicast_entry_add(unit, flags,
                       key, vid, c_dmac, trill_tree_id, group);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_trill_l2_multicast_entry_add(unit, flags, 
                          key, vid, c_dmac, trill_tree_id, group);
    }
#endif

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
         rv =   bcm_td_l2_trill_multicast_entry_add(unit, flags,
                       key, vid, c_dmac, trill_tree_id, group);
    }
#endif

    return rv;
}

/*
 * Purpose:
 *      Delete TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */

int 
_bcm_esw_trill_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t c_dmac, uint8 trill_tree_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
             rv = bcm_td2_trill_l2_multicast_entry_delete(unit, key,
                                            vid, c_dmac, trill_tree_id);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
             rv = bcm_td2_trill_l2_multicast_entry_delete(unit, key,
                                            vid, c_dmac, trill_tree_id);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
             rv = bcm_tr3_trill_l2_multicast_entry_delete(unit, key,
                                            vid, c_dmac, trill_tree_id);
    }
#endif

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
             rv = bcm_td_l2_trill_multicast_entry_delete(unit, key,
                                            vid, c_dmac, trill_tree_id);
    }
#endif

    return rv;

}

/*
 * Purpose:
 *      Update TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - Trill Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */

int 
_bcm_esw_trill_multicast_entry_update(int unit, int key, 
                                    bcm_vlan_t vid, bcm_mac_t c_dmac,
                                    uint8 trill_tree_id, bcm_multicast_t group)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv =  bcm_td2_trill_l2_multicast_entry_update(unit, key, 
                         vid, c_dmac, trill_tree_id, group);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
         rv =  bcm_td2_trill_l2_multicast_entry_update(unit, key, 
                         vid, c_dmac, trill_tree_id, group);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_trill_l2_multicast_entry_update(unit, key, 
                         vid, c_dmac, trill_tree_id, group);
    }
#endif

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
         rv =  bcm_td_l2_trill_multicast_entry_update(unit, key, 
                         vid, c_dmac, trill_tree_id, group);
    }
#endif

    return rv;

}

/*
 * Purpose:
 *      Modify TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      vid             - Customer Vlan ID
 *      set_bit        - Enable / Disable
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */

int 
_bcm_esw_trill_vlan_multicast_entry_modify(int unit,  bcm_vlan_t vid, int set_bit)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv =  bcm_td2_trill_vlan_multicast_entry_modify(unit, vid, set_bit);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
         rv =  bcm_td2_trill_vlan_multicast_entry_modify(unit, vid, set_bit);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_trill_vlan_multicast_entry_modify(unit, vid, set_bit);
    }
#endif

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
         rv =  bcm_td_l2_trill_vlan_multicast_entry_modify(unit, vid, set_bit);
    }
#endif

    return rv;

}

/*
 * Purpose:
 *       Reset TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      mc_index   - (IN) Multicast index
 */

int
_bcm_esw_trill_multicast_transit_entry_set(int unit, int flags,
                   bcm_trill_name_t root_name, int l3mc_index, uint8  trill_tree_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_set(unit, flags,
                            root_name, l3mc_index, trill_tree_id);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_set(unit, flags,
                            root_name, l3mc_index, trill_tree_id);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
              rv = bcm_tr3_trill_multicast_transit_entry_set(unit, flags,
                            root_name, l3mc_index, trill_tree_id);
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_set(unit, flags,
                            root_name, l3mc_index, trill_tree_id);
    }
#endif

    return rv;
}


/*
 * Purpose:
 *       Reset TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      mc_index   - (IN) Multicast index
 */

int
_bcm_esw_trill_multicast_transit_entry_reset(int unit, bcm_trill_name_t root_name)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_reset(unit, root_name);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_reset(unit, root_name);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
              rv = bcm_tr3_trill_multicast_transit_entry_reset(unit, root_name);
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_reset(unit, root_name);
    }
#endif

    return rv;
}

/*
 * Purpose:
 *       Reset all TRILL Multicast entries for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      tree_id - (IN) trill Tree ID
 *      mc_index   - (IN) Multicast index
 */

int
_bcm_esw_trill_multicast_transit_entry_reset_all (int unit, uint8 trill_tree_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_reset_all (unit, trill_tree_id);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_reset_all (unit, trill_tree_id);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
          rv = bcm_tr3_trill_multicast_transit_entry_reset_all(unit, trill_tree_id);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_reset_all (unit, trill_tree_id);
    }
#endif

    return rv;
}

/*
 * Purpose:
 *       Get TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      group   - (OUT) Multicast Group
 */

int
_bcm_esw_trill_multicast_transit_entry_get(int unit, bcm_trill_name_t root_name, bcm_multicast_t *group)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_get(unit, 
                      root_name, group);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_get(unit, 
                      root_name, group);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
          rv = bcm_tr3_trill_multicast_transit_entry_get(unit,
                      root_name, group);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
          rv = bcm_td_trill_multicast_transit_entry_get(unit, 
                      root_name, group);
    }
#endif

    return rv;
}

/*
 * Description:
 *      Delete all TRILL tree-sepecific Multicast entries
 * Parameters:
 *     unit             - device number
 *     tree_id          - Trill Tree ID
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_esw_trill_multicast_entry_delete_all (int unit, uint8 trill_tree_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td2_trill_l2_multicast_entry_delete_all (unit, trill_tree_id);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
         rv = bcm_td2_trill_l2_multicast_entry_delete_all (unit, trill_tree_id);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
          rv = bcm_tr3_trill_l2_multicast_entry_delete_all(unit, trill_tree_id);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
         rv = bcm_td_l2_trill_multicast_entry_delete_all (unit, trill_tree_id);
    }
#endif

    return rv;
}

/*
 * Purpose:
 *       Get TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      group   - (OUT) Multicast Group
 */

int
_bcm_esw_trill_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td2_trill_l2_multicast_entry_get(unit, key, 
                             vid, mac, trill_tree_id, group);    
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
         rv = bcm_td2_trill_l2_multicast_entry_get(unit, key, 
                             vid, mac, trill_tree_id, group);    
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
         rv = bcm_tr3_trill_l2_multicast_entry_get(unit, key, 
                             vid, mac, trill_tree_id, group);    
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
         rv = bcm_td_l2_trill_multicast_entry_get(unit, key, 
                             vid, mac, trill_tree_id, group);    
    }
#endif

    return rv;
}

/*
 * Purpose:
 *      Enable TRILL Multicast traffic   
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 */

int
_bcm_esw_trill_multicast_enable(int unit, int flags, bcm_trill_name_t root_name)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
          rv = bcm_td_trill_multicast_enable(unit, flags, root_name);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
          rv = bcm_td_trill_multicast_enable(unit, flags, root_name);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
             rv = bcm_tr3_trill_multicast_enable(unit, flags, root_name);
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
           rv = bcm_td_trill_multicast_enable(unit, flags, root_name);
    }
#endif

    return rv;
}


/*
 * Purpose:
 *      Disable TRILL Multicast traffic   
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 */

int
_bcm_esw_trill_multicast_disable(int unit, int flags, bcm_trill_name_t root_name)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
          rv = bcm_td_trill_multicast_disable(unit, flags, root_name);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
          rv = bcm_td_trill_multicast_disable(unit, flags, root_name);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
             rv = bcm_tr3_trill_multicast_disable(unit, flags, root_name);
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
           rv = bcm_td_trill_multicast_disable(unit, flags, root_name);
    }
#endif

    return rv;
}

/*
 * Description:
 *      Traverse  TRILL Access to Network Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */

int
_bcm_esw_trill_access_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
           rv = bcm_td2_trill_l2_access_multicast_entry_traverse(unit, trav_st);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
           rv = bcm_td2_trill_l2_access_multicast_entry_traverse(unit, trav_st);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
           rv = bcm_tr3_trill_l2_access_multicast_entry_traverse(unit, trav_st);
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
           rv = bcm_td_l2_trill_access_multicast_entry_traverse(unit, trav_st);
    }
#endif

    return rv;
}

/*
 * Description:
 *      Traverse  TRILL Long and Short Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */

int
_bcm_esw_trill_network_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td2_trill_l2_network_multicast_entry_traverse(unit, trav_st);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
       rv = bcm_td2_trill_l2_network_multicast_entry_traverse(unit, trav_st);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = bcm_tr3_trill_l2_network_multicast_entry_traverse(unit, trav_st);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
       rv = bcm_td_l2_trill_network_multicast_entry_traverse(unit, trav_st);
    }
#endif

    return rv;
}

/*
 * Description:
 *      Traverse  TRILL Unpruned Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_esw_trill_unpruned_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
         rv = bcm_td_trill_unpruned_entry_traverse(unit, trav_st);
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit)) {
        rv = bcm_td_trill_unpruned_entry_traverse(unit, trav_st);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = bcm_tr3_trill_unpruned_entry_traverse(unit, trav_st);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
        rv = bcm_td_trill_unpruned_entry_traverse(unit, trav_st);
    }
#endif

    return rv;
}

#else   /* INCLUDE_L3 */
typedef int bcm_esw_trill_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */
