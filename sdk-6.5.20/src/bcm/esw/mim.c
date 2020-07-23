/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * Mac-in-Mac initializers
 */
#ifdef INCLUDE_L3

#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/rate.h>
#include <bcm/ipmc.h>
#include <bcm/mim.h>
#include <bcm/stack.h>
#include <bcm/topo.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/flex_ctr.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT*/
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs3.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */

/* Initialize the MIM module. */
int 
bcm_esw_mim_init(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_init(unit);
    }
#endif
    return rv;
}

/* Detach the MIM module. */
int 
bcm_esw_mim_detach(int unit)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_detach(unit);
    }
#endif
    return rv;
}

/* Create a VPN instance. */
int 
bcm_esw_mim_vpn_create(int unit, bcm_mim_vpn_config_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_vpn_create(unit, info);
    }
#endif
    return rv;
}

/* Delete a VPN instance. */
int 
bcm_esw_mim_vpn_destroy(int unit, bcm_mim_vpn_t vpn)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_vpn_destroy(unit, vpn);
    }
#endif
    return rv;
}


/* Delete all VPN instances. */
int 
bcm_esw_mim_vpn_destroy_all(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_vpn_destroy_all(unit);
    }
#endif
    return rv;
}

/* Get a VPN instance by ID. */
int 
bcm_esw_mim_vpn_get(int unit, bcm_mim_vpn_t vpn, 
                    bcm_mim_vpn_config_t *info)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_vpn_get(unit, vpn, info);
    }
#endif
    return rv;
}

/* bcm_mim_vpn_traverse */
int 
bcm_esw_mim_vpn_traverse(int unit, bcm_mim_vpn_traverse_cb cb, 
                         void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_vpn_traverse(unit, cb, user_data);
    }
#endif
    return rv;
}

/* bcm_mim_port_add */
int 
bcm_esw_mim_port_add(int unit, bcm_mim_vpn_t vpn, 
                     bcm_mim_port_t *mim_port)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_port_add(unit, vpn, mim_port);
    }
#endif
    return rv;
}

/* bcm_mim_port_delete */
int 
bcm_esw_mim_port_delete(int unit, bcm_mim_vpn_t vpn, 
                    bcm_gport_t mim_port_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_port_delete(unit, vpn, mim_port_id);
    }
#endif
    return rv;
}

/* bcm_mim_port_delete_all */
int 
bcm_esw_mim_port_delete_all(int unit, bcm_mim_vpn_t vpn)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_port_delete_all(unit, vpn);
    }
#endif
    return rv;
}

/* bcm_mim_port_get */
int 
bcm_esw_mim_port_get(int unit, bcm_mim_vpn_t vpn, 
                     bcm_mim_port_t *mim_port)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_port_get(unit, vpn, mim_port);
    }
#endif
    return rv;
}

/* bcm_mim_port_get_all */
int 
bcm_esw_mim_port_get_all(int unit, bcm_mim_vpn_t vpn, int port_max, 
                     bcm_mim_port_t *port_array, int *port_count)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim)) {
        rv = bcm_tr2_mim_port_get_all(unit, vpn, port_max, port_array, 
                                      port_count);
    }
#endif
    return rv;
}
#if defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_esw_mim_lookup_id_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given MiM I-SID entry.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t _bcm_esw_mim_lookup_id_stat_get_table_info(
                   int                        unit,
                   int                        lookup_id,
                   uint32                     *num_of_tables,
                   bcm_stat_flex_table_info_t *table_info)
{
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT2X(unit)
        || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return  _bcm_tr3_mim_lookup_id_stat_get_table_info( unit, 
                                lookup_id, num_of_tables, table_info);
    } else {
        return BCM_E_UNAVAIL; 
    }
}

/*
 * Function:
 *      _bcm_esw_mim_vpn_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given MiM VPN entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
bcm_error_t _bcm_esw_mim_vpn_stat_get_table_info(
                   int                        unit,
                   bcm_mim_vpn_t              vpn,
                   uint32                     *num_of_tables,
                   bcm_stat_flex_table_info_t *table_info)
{

    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT2X(unit)) {
        return  _bcm_tr3_mim_vpn_stat_get_table_info( unit, 
                                vpn, num_of_tables, table_info);
    } else {
        return BCM_E_UNAVAIL; 
    }

   return BCM_E_UNAVAIL;
}
#endif

/*
 * Function:
 *      bcm_esw_mim_lookup_id_stat_attach
 * Description:
 *      Attach counters entries to the given MiM I-SID entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value 
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_lookup_id_stat_attach(
            int unit,
            int lookup_id,
            uint32 stat_counter_id)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    soc_mem_t                 table=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_mim_lookup_id_stat_get_table_info(
                   unit, lookup_id,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table)) {
             if (direction == bcmStatFlexDirectionIngress) {
                 return _bcm_esw_stat_flex_attach_ingress_table_counters(
                        unit,
                        table_info[count].table,
                        table_info[count].index,
                        offset_mode,
                        base_index,
                        pool_number);
             } else {
                 return _bcm_esw_stat_flex_attach_egress_table_counters(
                        unit,
                        table_info[count].table,
                        table_info[count].index,
                        0,
                        offset_mode,
                        base_index,
                        pool_number);
             }
         }
    }
    return BCM_E_NOT_FOUND;
#else
    return BCM_E_UNAVAIL;
#endif
}
/*
 * Function:
 *      bcm_esw_mim_lookup_id_stat_detach
 * Description:
 *      Detach counters entries to the given MiM I-SID entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value 
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_lookup_id_stat_detach(
            int unit,
            int lookup_id)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_lookup_id_stat_get_table_info(
                   unit, lookup_id,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
        if (table_info[count].direction == bcmStatFlexDirectionIngress) {
            rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index);
            if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
            }
        } else {
            rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                          unit, 0,
                          table_info[count].table,
                          table_info[count].index);
            if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
            }
        }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      _bcm_esw_mim_lookup_id_stat_counter_get
 * Description:
 *  	Get counter statistic values for specific MiM I-SID entry
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      lookup_id        - (IN) I-SID value 
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
 _bcm_esw_mim_lookup_id_stat_counter_get(int unit,
                                        int sync_mode,
                                        int lookup_id,
                                        bcm_mim_stat_t stat,
                                        uint32 num_entries,
                                        uint32 *counter_indexes,
                                        bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress; 
    }
    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_lookup_id_stat_get_table_info(
                   unit, lookup_id,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
               /*ctr_offset_info.offset_index = counter_indexes[index_count];*/
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_mim_sync
 *
 * Purpose:
 *      Record MIM module persistent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *  WB_VERSION_1_0
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_mim_sync(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mim)) {
        return bcm_tr2_mim_sync(unit);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_esw_mim_lookup_id_stat_counter_get
 * Description:
 *  	Get counter statistic values for specific MiM I-SID entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value 
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_mim_lookup_id_stat_counter_get(int unit,
                                       int lookup_id,
                                       bcm_mim_stat_t stat,
                                       uint32 num_entries,
                                       uint32 *counter_indexes,
                                       bcm_stat_value_t *counter_values)
{
    return _bcm_esw_mim_lookup_id_stat_counter_get(unit, 0, lookup_id, stat,
                                                   num_entries, 
                                                   counter_indexes, 
                                                   counter_values);
}

/*
 * Function:
 *      bcm_esw_mim_lookup_id_stat_counter_sync_get
 * Description:
 *      Get counter statistic values for specific MiM VPN entry
 *      sw accumulated counters synced with hw count.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value 
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_mim_lookup_id_stat_counter_sync_get(int unit,
                                           int lookup_id,
                                           bcm_mim_stat_t stat,
                                           uint32 num_entries,
                                           uint32 *counter_indexes,
                                           bcm_stat_value_t *counter_values)
{
    return _bcm_esw_mim_lookup_id_stat_counter_get(unit, 1, lookup_id, stat,
                                                   num_entries, 
                                                   counter_indexes, 
                                                   counter_values);
}
/*
 * Function:
 *      bcm_esw_mim_lookup_id_stat_counter_set
 * Description:
 *  	Set counter statistic values for specific MiM I-SID entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value 
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_lookup_id_stat_counter_set(
            int unit,
            int lookup_id,
            bcm_mim_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if (stat == bcmMimStatIngressPackets) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_lookup_id_stat_get_table_info(
                   unit, lookup_id,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                unit,
                                table_info[table_count].index,
                                table_info[table_count].table,
                                0,
                                byte_flag,
                                counter_indexes[index_count],
                                &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}
/*
 * Function:
 *      bcm_esw_mim_lookup_id_stat_id_get
 * Description:
 *      Get stat counter id associated with given MiM I-SID entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      lookup_id        - (IN) I-SID value 
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_lookup_id_stat_id_get(
            int unit,
            int lookup_id,
            bcm_mim_stat_t stat,
            uint32 *stat_counter_id)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress; 
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_lookup_id_stat_get_table_info(
                        unit,lookup_id,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
#else
    return BCM_E_UNAVAIL;
#endif
}
/*
 * Function:
 *      bcm_esw_mim_vpn_stat_attach
 * Description:
 *      Attach counters entries to the given MiM VPN entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_vpn_stat_attach(
            int unit,
            bcm_mim_vpn_t vpn,
            uint32 stat_counter_id)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    soc_mem_t                 table=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);
    
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,1,&actual_num_tables,&table,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_mim_vpn_stat_get_table_info(
                   unit, vpn,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table)) {
             if (direction == bcmStatFlexDirectionIngress) {
                 return _bcm_esw_stat_flex_attach_ingress_table_counters(
                        unit,
                        table_info[count].table,
                        table_info[count].index,
                        offset_mode,
                        base_index,
                        pool_number);
             } else {
                 return _bcm_esw_stat_flex_attach_egress_table_counters(
                        unit,
                        table_info[count].table,
                        table_info[count].index,
                        0,
                        offset_mode,
                        base_index,
                        pool_number);
             }
         }
    }
    return BCM_E_NOT_FOUND;
#else
    return BCM_E_UNAVAIL;
#endif
}
/*
 * Function:
 *      bcm_esw_mim_vpn_stat_detach
 * Description:
 *      Detach counters entries to the given MiM VPN entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_vpn_stat_detach(
            int unit,
            bcm_mim_vpn_t vpn)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_vpn_stat_get_table_info(
                   unit, vpn,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv = _bcm_esw_stat_flex_detach_egress_table_counters(
                          unit, 0,
                          table_info[count].table,
                          table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      _bcm_esw_mim_vpn_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MiM VPN entry
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) if 1 sync sw accumulated counter
 *                         ihw count else retrieve sw count
 *      vpn              - (IN) MiM VPN
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int  
_bcm_esw_mim_vpn_stat_counter_get(int unit,
                                  int sync_mode,
                                  bcm_mim_vpn_t vpn,
                                  bcm_mim_stat_t stat,
                                  uint32 num_entries,
                                  uint32 *counter_indexes,
                                  bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_vpn_stat_get_table_info(
                   unit, vpn,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
               /*ctr_offset_info.offset_index = counter_indexes[index_count];*/
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_mim_vpn_stat_counter_get
 * Description:
 *      Get counter statistic values for specific MiM VPN entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_mim_vpn_stat_counter_get(int unit,
                                            bcm_mim_vpn_t vpn,
                                            bcm_mim_stat_t stat,
                                            uint32 num_entries,
                                            uint32 *counter_indexes,
                                            bcm_stat_value_t *counter_values)
{
    return _bcm_esw_mim_vpn_stat_counter_get(unit, 0, vpn, stat, num_entries,
                                             counter_indexes, counter_values);
}

/*
 * Function:
 *      bcm_esw_mim_vpn_stat_counter_sync_get
 * Description:
 *      Get counter statistic values for specific MiM VPN entry
 *      Sw accumulated count is synced with hw counters.
 *
 * ParameterS:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_mim_vpn_stat_counter_sync_get(int unit,
            bcm_mim_vpn_t vpn,
            bcm_mim_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    return _bcm_esw_mim_vpn_stat_counter_get(unit, 1, vpn, stat, num_entries,
                                             counter_indexes, counter_values);
}

/*
 * Function:
 *      bcm_esw_mim_vpn_stat_counter_set
 * Description:
 *      Set counter statistic values for specific MiM VPN entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_vpn_stat_counter_set(
            int unit,
            bcm_mim_vpn_t vpn,
            bcm_mim_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }
    if (stat == bcmMimStatIngressPackets) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_vpn_stat_get_table_info(
                   unit, vpn,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                unit,
                                table_info[table_count].index,
                                table_info[table_count].table,
                                0,
                                byte_flag,
                                counter_indexes[index_count],
                                &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}
/*
 * Function:
 *      bcm_esw_mim_vpn_stat_id_get
 * Description:
 *      Get stat counter id associated with given MiM VPN entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      vpn              - (IN) MiM VPN
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_mim_vpn_stat_id_get(
            int unit,
            bcm_mim_vpn_t vpn,
            bcm_mim_stat_t stat,
            uint32 *stat_counter_id)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmMimStatIngressPackets) ||
        (stat == bcmMimStatIngressBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         direction = bcmStatFlexDirectionEgress;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_mim_vpn_stat_get_table_info(
                        unit,vpn,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      _bcm_esw_mim_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mim_port_source_vp_lag_set(
    int unit, bcm_gport_t gport, int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_mim_port_source_vp_lag_set(unit, gport, vp_lag_vp);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_mim_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (IN) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mim_port_source_vp_lag_clear(
    int unit, bcm_gport_t gport, int vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_mim_port_source_vp_lag_clear(unit, gport, vp_lag_vp);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_esw_mim_port_source_vp_lag_get
 * Purpose:
 *      Get source VP LAG for a MIM virtual port.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      gport     - (IN) MIM virtual port GPORT ID.
 *      vp_lag_vp - (OUT) VP representing the VP LAG.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mim_port_source_vp_lag_get(
    int unit, bcm_gport_t gport, int *vp_lag_vp)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mim) &&
        soc_feature(unit, soc_feature_vp_lag)) {
        rv = bcm_td2_mim_port_source_vp_lag_get(unit, gport, vp_lag_vp);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

#else /* INCLUDE_L3 */
typedef int _bcm_esw_mim_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */
