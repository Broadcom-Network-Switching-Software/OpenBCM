/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/stat.h>
#include <bcm/vlan.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/flowcnt.h>
#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      _bcm_hr3_vlan_translate_stat_table_info_get
 * Description:
 *      Provides relevant flex table information(table-name,index)  
 *            for given ingress vlan translation
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) Generic port
 *      key_type         - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan       - (IN) Packet outer VLAN ID
 *      inner_vlan       - (IN) Packet inner VLAN ID
 *      table            - (OUT) accounting object table
 *      index            - (OUT) accounting object table index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_hr3_vlan_translate_stat_table_info_get(
                   int                        unit, 
                   bcm_gport_t                port,
                   bcm_vlan_translate_key_t   key_type,
                   bcm_vlan_t                 outer_vlan,
                   bcm_vlan_t                 inner_vlan,
                   soc_mem_t                  *table,
                   uint32                     *index)
{
    int rv=BCM_E_NONE;
    vlan_xlate_entry_t vent={{0}};
    vlan_xlate_entry_t vlan_xlate_entry={{0}};
    int idx=0;
    soc_mem_t mem;

    mem = VLAN_XLATEm;
    /* get table and its index */
    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    BCM_IF_ERROR_RETURN (_bcm_trx_vlan_translate_entry_assemble(
                         unit, &vent, port, key_type, inner_vlan, outer_vlan));
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, &vent,
                        &vlan_xlate_entry, 0);
    soc_mem_unlock(unit, mem);
    
    if (BCM_SUCCESS(rv)) {
        *table = mem;
        *index = idx;
    }    
    return rv;
}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_stat_detach
 * Description:
 *      Detach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_hr3_vlan_translate_stat_detach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan)
{
    soc_mem_t table=0;
    uint32 index;

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_stat_table_info_get(
                        unit, port, key_type, outer_vlan, inner_vlan, 
                        &table, &index));  

    return _bcm_esw_flowcnt_object_table_detach(unit, table, index);

}


/*
 * Function:
 *      _bcm_hr3_vlan_translate_stat_attach
 * Description:
 *      attach counters entries for the given ingress vlan translation table.
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *      Stat_counter_id  - (IN) Stat Counter ID.                   
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_hr3_vlan_translate_stat_attach(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            uint32                   stat_counter_id)
{
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32  profile_group, pool_index, base_index;
    soc_mem_t table=0;
    uint32 index;

    _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode, &object, 
        &profile_group, &pool_index, &base_index);

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_stat_table_info_get(
                        unit, port, key_type, outer_vlan, inner_vlan, 
                        &table, &index));  

    return _bcm_esw_flowcnt_object_table_attach(
            unit, object, table, index, profile_group, base_index, 
            pool_index, NULL);
}            

/* Function:
 *      _bcm_hr3_vlan_translate_stat_counter_set
 * Description:
 *      Set counter values into a ingress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
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
int _bcm_hr3_vlan_translate_stat_counter_set(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{
    uint32 byte_flag=0;
    uint32 index_count=0;
    soc_mem_t table=0;
    uint32 index;    
    
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_stat_table_info_get(
                        unit, port, key_type, outer_vlan, inner_vlan, 
                        &table, &index));  

    for (index_count=0; index_count < num_entries ; index_count++) {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_set(
                          unit, index, table, byte_flag,
                          counter_indexes[index_count],
                          &counter_values[index_count]));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_stat_counter_get
 * Description:
 *      Get counter values from ingress vlan translation table
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
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
int _bcm_hr3_vlan_translate_stat_counter_get(
            int                      unit,
            int                      sync_mode,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   num_entries,
            uint32                   *counter_indexes,
            bcm_stat_value_t         *counter_values)
{

    uint32 byte_flag=0;
    uint32 index_count=0;
    soc_mem_t table=0;
    uint32 index;    
    
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_stat_table_info_get(
                        unit, port, key_type, outer_vlan, inner_vlan, 
                        &table, &index));  
    for (index_count=0; index_count < num_entries ; index_count++) {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_get(
                          unit, sync_mode, index, table, byte_flag,
                          counter_indexes[index_count],
                          &counter_values[index_count]));
    }
    return BCM_E_NONE;
}            


/*
 * Function:
 *      _bcm_hr3_vlan_translate_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated
 *      ingress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Generic port
 *      key_type - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_hr3_vlan_translate_stat_enable_set(int unit, bcm_gport_t port,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       int enable)
{
    soc_mem_t table=0;
    uint32 index;
    bcm_stat_object_t object=bcmStatObjectIngVlanXlate;
    uint32 stat_counter_id;
    uint32 num_entries = 0;
    
    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_stat_table_info_get(
                        unit, port, key_type, outer_vlan, inner_vlan, 
                        &table, &index)); 

    if (enable) {
        BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                    unit,object,bcmStatGroupModeSingle,
                                    &stat_counter_id,
                                    &num_entries));
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_attach(
                        unit,port,key_type,outer_vlan,inner_vlan,
                        stat_counter_id));

    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_stat_id_get(
                    unit, table, index, &stat_counter_id));
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_stat_detach(
                    unit,port,key_type,outer_vlan,inner_vlan));
        BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                    unit, stat_counter_id));

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_stat_id_get
 * Description:
 *      Get stat counter id associated with given ingress vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      Port             - (IN) GPORT (global port) identifier  
 *      key_type         - (IN) VLAN translation key type  
 *      outer_vlan       - (IN) Outer VLAN ID or tag  
 *      inner_vlan       - (IN) Inner VLAN ID or tag  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_hr3_vlan_translate_stat_id_get(
            int                      unit,
            bcm_gport_t              port,
            bcm_vlan_translate_key_t key_type,
            bcm_vlan_t               outer_vlan,
            bcm_vlan_t               inner_vlan,
            bcm_vlan_stat_t          stat,
            uint32                   *stat_counter_id)
{
    soc_mem_t table=0;
    uint32 index;
    _bcm_flowcnt_stat_type_t stat_type;

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_stat_table_info_get(
                        unit, port, key_type, outer_vlan, inner_vlan, 
                        &table, &index)); 

    if (stat == bcmVlanStatIngressPackets) {
        stat_type = bcmFlowcntStatTypePacket;
    } else if (stat == bcmVlanStatIngressBytes){
        stat_type = bcmFlowcntStatTypeByte;
    } else {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_stat_type_validate(
                        unit, table, index, stat_type));

    return _bcm_esw_flowcnt_stat_id_get(
                unit, table, index, stat_counter_id);
}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_table_info_get
 * Description:
 *      Provides relevant flex table information(table-name,index)  
 *            for given ingress vlan translation
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class
 *      key_type         - (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan       - (IN) Packet outer VLAN ID
 *      inner_vlan       - (IN) Packet inner VLAN ID
 *      table            - (OUT) accounting object table
 *      index            - (OUT) accounting object table index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                   int                        unit, 
                   int                        port_class,
                   bcm_vlan_t                 outer_vlan,
                   bcm_vlan_t                 inner_vlan,
                   soc_mem_t                  *table,
                   uint32                     *index)
{
    int rv=BCM_E_NONE;
    egr_vlan_xlate_entry_t vent={{0}};
    egr_vlan_xlate_entry_t egr_vlan_xlate_entry={{0}};
    int idx=0;
    soc_mem_t mem;    
    bcm_module_t           modid=0;
    bcm_trunk_t            tgid = -1;
    int                    id = -1;

    if (BCM_GPORT_IS_SET(port_class)) {
        if (!BCM_GPORT_IS_TUNNEL(port_class)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(
                                unit, port_class, &modid, 
                                &port_class,&tgid, &id));
        }
        if ((-1 != tgid || -1 != id) && !BCM_GPORT_IS_TUNNEL(port_class)) {
             return BCM_E_PORT;
        }
    } else  {
        if (!SOC_PORT_CLASS_VALID(unit, port_class)) {
            return BCM_E_PORT;
        }
    }

    mem = EGR_VLAN_XLATEm;
    /* get table and its index */
    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));
    BCM_IF_ERROR_RETURN (_bcm_trx_vlan_translate_egress_entry_assemble(
                         unit, &vent, port_class, outer_vlan, inner_vlan, 0));
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, &vent,
                        &egr_vlan_xlate_entry, 0);
    soc_mem_unlock(unit, mem);
    
    if (BCM_SUCCESS(rv)) {
        *table = mem;
        *index = idx;
    }    
    return rv;
}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_attach
 * Description:
 *      Attach counters for the given egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class 
 *      outer_vlan       - (IN) Outer VLAN ID 
 *      inner_vlan       - (IN) Inner VLAN ID  
 *      Stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_hr3_vlan_translate_egress_stat_attach(
            int        unit,
            int        port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan,
            uint32     stat_counter_id)
{
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32  profile_group, pool_index, base_index;
    soc_mem_t table=0;
    uint32 index;

    _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode, &object, 
        &profile_group, &pool_index, &base_index);

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                        unit, port_class, outer_vlan, inner_vlan, 
                        &table, &index));  

    return _bcm_esw_flowcnt_object_table_attach(
            unit, object, table, index, profile_group, base_index, 
            pool_index, NULL);

}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_counter_get
 * Description:
 *      Get counter values from egress vlan translation table
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
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
_bcm_hr3_vlan_translate_egress_stat_counter_get(
            int              unit,
            int              sync_mode,
            int              port_class,
            bcm_vlan_t       outer_vlan,
            bcm_vlan_t       inner_vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{
 uint32 byte_flag=0;
    uint32 index_count=0;
    soc_mem_t table=0;
    uint32 index;    
    
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                        unit, port_class, outer_vlan, inner_vlan, 
                        &table, &index));  

    for (index_count=0; index_count < num_entries ; index_count++) {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_get(
                          unit, sync_mode, index, table, byte_flag,
                          counter_indexes[index_count],
                          &counter_values[index_count]));
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_counter_set
 * Description:
 *      Set counter values from egress vlan translation table . 
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
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
int
_bcm_hr3_vlan_translate_egress_stat_counter_set(
            int              unit,
            int              port_class,
            bcm_vlan_t       outer_vlan,
            bcm_vlan_t       inner_vlan,
            bcm_vlan_stat_t  stat,
            uint32           num_entries,
            uint32           *counter_indexes,
            bcm_stat_value_t *counter_values)
{

    uint32 byte_flag=0;
    uint32 index_count=0;
    soc_mem_t table=0;
    uint32 index;    
    
    if ((stat == bcmVlanStatIngressPackets) ||
        (stat == bcmVlanStatEgressPackets)) {
        byte_flag=0;
    } else {
        byte_flag=1;
    }

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                        unit, port_class, outer_vlan, inner_vlan, 
                        &table, &index)); 

    for (index_count=0; index_count < num_entries ; index_count++) {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_set(
                          unit, index, table, byte_flag,
                          counter_indexes[index_count],
                          &counter_values[index_count]));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_id_get
 * Description:
 *      Get stat counter id associated with given egress vlan
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN) Port class 
 *      outer_vlan       - (IN) Outer VLAN ID 
 *      inner_vlan       - (IN) Inner VLAN ID  
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_hr3_vlan_translate_egress_stat_id_get(
            int             unit,
            int             port_class,
            bcm_vlan_t      outer_vlan,
            bcm_vlan_t      inner_vlan,
            bcm_vlan_stat_t stat, 
            uint32          *stat_counter_id)
{
    soc_mem_t table=0;
    uint32 index;
    _bcm_flowcnt_stat_type_t stat_type;


    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                        unit, port_class, outer_vlan, inner_vlan, 
                        &table, &index)); 

    if (stat == bcmVlanStatIngressPackets) {
        stat_type = bcmFlowcntStatTypePacket;
    } else if (stat == bcmVlanStatIngressBytes){
        stat_type = bcmFlowcntStatTypeByte;
    } else {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_stat_type_validate(
                        unit, table, index, stat_type));

    return _bcm_esw_flowcnt_stat_id_get(
                unit, table, index, stat_counter_id);

}            

/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_detach
 * Description:
 *      Detach counters for the given egress vlan translation table
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_class       - (IN)    Port class 
 *      outer_vlan       - (IN)    Outer VLAN ID 
 *      inner_vlan       - (IN)    Inner VLAN ID  
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_hr3_vlan_translate_egress_stat_detach(
            int unit,
            int port_class,
            bcm_vlan_t outer_vlan,
            bcm_vlan_t inner_vlan)
{
    soc_mem_t table=0;
    uint32 index;

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                        unit, port_class, outer_vlan, inner_vlan, 
                        &table, &index)); 

    return _bcm_esw_flowcnt_object_table_detach(unit, table, index);

}


/*
 * Function:
 *      _bcm_hr3_vlan_translate_egress_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated
 *      egress VLAN translation.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Group ID of ingress port
 *      outer_vlan - (IN) Packet outer VLAN ID
 *      inner_vlan - (IN) Packet inner VLAN ID
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_hr3_vlan_translate_egress_stat_enable_set(int unit, int port_class, 
                                              bcm_vlan_t outer_vlan,
                                              bcm_vlan_t inner_vlan,
                                              int enable)
{
    soc_mem_t table=0;
    uint32 index;
    bcm_stat_object_t object=bcmStatObjectEgrVlanXlate;
    uint32 stat_counter_id;
    uint32 num_entries = 0;
    

    BCM_IF_ERROR_RETURN(_bcm_hr3_vlan_translate_egress_stat_table_info_get(
                        unit, port_class, outer_vlan, inner_vlan, 
                        &table, &index)); 

    if (enable) {
        BCM_IF_ERROR_RETURN(bcm_esw_stat_group_create(
                                    unit,object,bcmStatGroupModeSingle,
                                    &stat_counter_id,
                                    &num_entries));
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_attach(
                        unit, port_class, outer_vlan, inner_vlan,
                        stat_counter_id));

    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_stat_id_get(
                    unit, table, index, &stat_counter_id));
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_translate_egress_stat_detach(
                    unit, port_class, outer_vlan, inner_vlan));
        BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(
                    unit, stat_counter_id));

    }
    return BCM_E_NONE;
}
