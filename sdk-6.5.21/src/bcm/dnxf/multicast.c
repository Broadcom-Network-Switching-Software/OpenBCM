/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF MULTICAST
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_MCAST
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/multicast.h>

#include <bcm_int/dnxf/dnxf_multicast_id_map.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_multicast.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

int
bcm_dnxf_multicast_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_multicast_detach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_multicast_init
 * Purpose:
 *      Initialize the multicast module
 * Parameters:
 *      unit               - (IN)  Unit number.
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int 
dnxf_multicast_init(
    int unit)
{
    int rc;
    soc_dnxf_multicast_table_mode_t mc_mode;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_mode_get, (unit, &mc_mode));
    SHR_IF_ERR_EXIT(rc);
     
    DNXF_UNIT_LOCK_TAKE(unit);
    
    if (!SOC_WARM_BOOT(unit))
    {
      rc = bcm_dnxf_multicst_id_map_init(unit, mc_mode);
      SHR_IF_ERR_EXIT(rc);
    }    
    
    if (dnxf_data_fabric.general.local_routing_enable_mc_get(unit))
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_local_route_default_init, (unit));
        SHR_IF_ERR_EXIT(rc);
    }

    DNXF_UNIT_LOCK_RELEASE(unit);
    
exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      dnxf_multicast_deinit
 * Purpose:
 *      Shut down (uninitialize) the multicast module
 * Parameters:
 *      unit               - (IN)  Unit number.
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int 
dnxf_multicast_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    
    

    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_multicast_create
 * Purpose:
 *      Create a multicast group for packet replication
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      flags - (IN)     Only BCM_MULTICAST_WITH_ID is relevant 
 *      group - (IN/OUT) Multicast group ID
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_create(
    int unit, 
    uint32 flags, 
    bcm_multicast_t *group)
{
    int rc, is_legal, is_free;
    uint32 nof_mc_groups_created;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(group, _SHR_E_PARAM, "group");
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    if(flags & BCM_MULTICAST_WITH_ID)
    {
        rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, *group, &is_legal);
        SHR_IF_ERR_EXIT(rc);
        if(!is_legal)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", *group);
        }

        rc = bcm_dnxf_multicst_id_map_is_free_id(unit, *group, &is_free);
        SHR_IF_ERR_EXIT(rc);
        if(!is_free)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Multicast group %d is already allocated", *group);
        } 

        rc = bcm_dnxf_multicst_id_map_mark_id(unit, *group);
        SHR_IF_ERR_EXIT(rc);
        
    } 
    else 
    {
        rc = bcm_dnxf_multicst_id_map_get_free_id(unit, group);
        SHR_IF_ERR_EXIT(rc);
    
        rc = bcm_dnxf_multicst_id_map_mark_id(unit, *group);
        SHR_IF_ERR_EXIT(rc);
    }

    rc = dnxf_state.mc.nof_mc_groups_created.get(unit, &nof_mc_groups_created);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxf_state.mc.nof_mc_groups_created.set(unit, (nof_mc_groups_created+1));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;    
}

/*
 * Function:
 *      bcm_dnxf_multicast_destroy
 * Purpose:
 *      Destroy a multicast group
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      group   - (IN)  Multicast group ID 
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_destroy(
    int unit, 
    bcm_multicast_t group)
{
    int rc = _SHR_E_NONE, is_legal, is_free;
    bcm_fabric_module_vector_t dest_array = {0};
    int local_route = 0;
    uint32 nof_mc_groups_created;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);
    
    /*Validation*/
    rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, group, &is_legal);
    SHR_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", group);
    } 

    rc = bcm_dnxf_multicst_id_map_is_free_id(unit, group, &is_free);
    SHR_IF_ERR_EXIT(rc);
    if(is_free)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group %d wasn't created", group);
    }
    
    /* 
     * Clear MC group destinations and disable MC local route for this
     * group (return to default).
     * Note that 'dest_array' and 'local_route' are zeroed.
     */
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_set, (unit, 0, 1, &group, &dest_array, &local_route));
    SHR_IF_ERR_EXIT(rc);
    
    rc = bcm_dnxf_multicst_id_map_clear_id(unit, group);
    SHR_IF_ERR_EXIT(rc);
    
    rc = dnxf_state.mc.nof_mc_groups_created.get(unit, &nof_mc_groups_created);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxf_state.mc.nof_mc_groups_created.set(unit, (nof_mc_groups_created-1));
    SHR_IF_ERR_EXIT(rc);
 
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;  
}


/*
 * Function:
 *      bcm_dnxf_multicast_egress_add
 * Purpose:
 *      Add an encapsulated interface to a multicast group's replication list on a specific port
 * Parameters:
 *      unit     - (IN) Unit number.
 *      group    - (IN) Multicast group ID
 *      port     - (IN) Physical GPORT ID 
 *      encap_id - (IN) Encapsulation ID 
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_egress_add(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    int rc = _SHR_E_NONE, is_legal, is_free;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*Validation*/
    DNXF_LINK_INPUT_CHECK(unit, port);
      
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, group, &is_legal);
    SHR_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", group);
    } 

    rc = bcm_dnxf_multicst_id_map_is_free_id(unit, group, &is_free);
    SHR_IF_ERR_EXIT(rc);
    if(is_free)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group %d wasn't created", group);
    }

     rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_egress_add,(unit, group, port));
     SHR_IF_ERR_EXIT(rc);
    
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;  
}

/*
 * Function:
 *      bcm_dnxf_multicast_egress_delete
 * Purpose:
 *      Delete a port from a multicast group's replication list
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      group    - (IN)  Multicast group ID 
 *      port     - (IN) Physical GPORT ID
 *      encap_id - (IN) Encapsulation ID  
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_egress_delete(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    int rc = _SHR_E_NONE, is_legal, is_free;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*Validation*/
    DNXF_LINK_INPUT_CHECK(unit, port);
  
    DNXF_UNIT_LOCK_TAKE(unit);
        
    rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, group, &is_legal);
    SHR_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", group);
    } 

    rc = bcm_dnxf_multicst_id_map_is_free_id(unit, group, &is_free);
    SHR_IF_ERR_EXIT(rc);
    if(is_free)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group %d wasn't created", group);
    }
  
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_egress_delete,(unit, group, port));
    SHR_IF_ERR_EXIT(rc);
    
   
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;  
}

/*
 * Function:
 *      bcm_dnxf_multicast_egress_delete_all
 * Purpose:
 *      Delete all replications for the specified multicast index
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      group - (IN)  Multicast group ID
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_egress_delete_all(
    int unit, 
    bcm_multicast_t group)
{
    int rc = _SHR_E_NONE, is_legal, is_free;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    /*Valiodation*/
    rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, group, &is_legal);
    SHR_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", group);
    } 

    rc = bcm_dnxf_multicst_id_map_is_free_id(unit, group, &is_free);
    SHR_IF_ERR_EXIT(rc);
    if(is_free)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group %d wasn't created", group);
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_egress_delete_all,(unit, group));
    SHR_IF_ERR_EXIT(rc);

    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_multicast_egress_get
 * Purpose:
 *      Get the set of encapsulated interfaces on specific ports which comprises a multicast group's replication list
 * Parameters:
 *      unit           - (IN)  Unit number.
 *      group          - (IN)  Multicast group ID 
 *      port_max       - (IN)  Number of items allocated in the array 
 *      port_array     - (OUT) Array of physical ports 
 *      encap_id_array - (OUT) Array of encapsulation IDs 
 *      port_count     - (OUT) Actual number of ports returned in the array 
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_egress_get(
    int unit, 
    bcm_multicast_t group, 
    int port_max, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array, 
    int *port_count)
{
    int rc = _SHR_E_NONE, is_legal, is_free;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*Validation*/
    SHR_NULL_CHECK(port_array, _SHR_E_PARAM, "port_array");
    SHR_NULL_CHECK(port_count, _SHR_E_PARAM, "port_count");
    
    DNXF_UNIT_LOCK_TAKE(unit);
      
    rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, group, &is_legal);
    SHR_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", group);
    } 

    rc = bcm_dnxf_multicst_id_map_is_free_id(unit, group, &is_free);
    SHR_IF_ERR_EXIT(rc);
    if(is_free)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group %d wasn't created", group);
    }

    /*Get reg*/  
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_egress_get,(unit, group, (soc_gport_t)port_max, (soc_gport_t*)port_array, port_count));
    SHR_IF_ERR_EXIT(rc);

    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_multicast_egress_set
 * Purpose:
 *      Assign a set of encapsulated interfaces on specific ports as a multicast group's replication list
 * Parameters:
 *      unit           - (IN) Unit number.
 *      group          - (IN) Multicast group ID 
 *      port_count     - (IN) Number of ports in the array
 *      port_array     - (IN) Array of physical ports 
 *      encap_id_array - (IN) Array of encapsulation IDs 
 * Returns:
 *      _SHR_E_xxx
 */
int 
bcm_dnxf_multicast_egress_set(
    int unit, 
    bcm_multicast_t group, 
    int port_count, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array)
{
    int rc = _SHR_E_NONE;
    int i, is_legal, is_free;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*Validation*/
    SHR_NULL_CHECK(port_array, _SHR_E_PARAM, "port_array");
    
    for(i = 0 ; i<port_count ; i++)
    {
        DNXF_LINK_INPUT_CHECK(unit, port_array[i]);
    }
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rc = bcm_dnxf_multicst_id_map_is_legal_id(unit, group, &is_legal);
    SHR_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "group %d is illegal", group);
    } 

    rc = bcm_dnxf_multicst_id_map_is_free_id(unit, group, &is_free);
    SHR_IF_ERR_EXIT(rc);
    if(is_free)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "group %d wasn't created", group);
    }
    
    /*Set reg*/  
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_multicast_egress_set,(unit, group, port_count, (soc_gport_t*)port_array));
    SHR_IF_ERR_EXIT(rc);
    
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE

