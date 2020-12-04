/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE MULTICAST
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MCAST
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/multicast.h>

#include <bcm_int/dfe/dfe_multicast_id_map.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_multicast.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

/*
 * Function:
 *      bcm_dfe_multicast_init
 * Purpose:
 *      Initialize the multicast module
 * Parameters:
 *      unit               - (IN)  Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_multicast_init(
    int unit)
{
    int rc;
	soc_dfe_multicast_table_mode_t mc_mode;
    BCMDNX_INIT_FUNC_DEFS;

	rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_mode_get, (unit, &mc_mode));
	BCMDNX_IF_ERR_EXIT(rc);
     
    DFE_UNIT_LOCK_TAKE(unit);
    
    if (!SOC_WARM_BOOT(unit))
    {
      rc = bcm_dfe_multicst_id_map_init(unit, mc_mode);
      BCMDNX_IF_ERR_EXIT(rc);
    }    
    
    DFE_UNIT_LOCK_RELEASE(unit);
    
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_multicast_detach
 * Purpose:
 *      Shut down (uninitialize) the multicast module
 * Parameters:
 *      unit               - (IN)  Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_multicast_detach(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    

    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_multicast_create
 * Purpose:
 *      Create a multicast group for packet replication
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      flags - (IN)     Only BCM_MULTICAST_WITH_ID is relevant 
 *      group - (IN/OUT) Multicast group ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_create(
    int unit, 
    uint32 flags, 
    bcm_multicast_t *group)
{
    int rc, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(group);
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    if(flags & BCM_MULTICAST_WITH_ID)
    {
        rc = bcm_dfe_multicst_id_map_is_legal_id(unit, *group, &is_legal);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_legal)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), *group));
        }

        rc = bcm_dfe_multicst_id_map_is_free_id(unit, *group, &is_free);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_free)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Multicast group %d is already allocated"), *group));
        } 

        rc = bcm_dfe_multicst_id_map_mark_id(unit, *group);
        BCMDNX_IF_ERR_EXIT(rc);
        
    } 
    else 
    {
        rc = bcm_dfe_multicst_id_map_get_free_id(unit, group);
        BCMDNX_IF_ERR_EXIT(rc);
    
        rc = bcm_dfe_multicst_id_map_mark_id(unit, *group);
        BCMDNX_IF_ERR_EXIT(rc);
    }
   
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;    
}

/*
 * Function:
 *      bcm_dfe_multicast_destroy
 * Purpose:
 *      Destroy a multicast group
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      group   - (IN)  Multicast group ID 
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_destroy(
    int unit, 
    bcm_multicast_t group)
{
    int rc = BCM_E_NONE, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*Valiodation*/
    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), group));
    } 

    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"), group));
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_delete_all,(unit, group));
    BCMDNX_IF_ERR_EXIT(rc);
    
    rc = bcm_dfe_multicst_id_map_clear_id(unit, group);
    BCMDNX_IF_ERR_EXIT(rc);
    
 
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}


/*
 * Function:
 *      bcm_dfe_multicast_egress_add
 * Purpose:
 *      Add an encapsulated interface to a multicast group's replication list on a specific port
 * Parameters:
 *      unit     - (IN) Unit number.
 *      group    - (IN) Multicast group ID
 *      port     - (IN) Physical GPORT ID 
 *      encap_id - (IN) Encapsulation ID 
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_egress_add(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    int rc = BCM_E_NONE, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    DFE_LINK_INPUT_CHECK(unit, port);
      
    DFE_UNIT_LOCK_TAKE(unit);
    
    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), group));
    } 

    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"), group));
    }

     rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_add,(unit, group, port));
     BCMDNX_IF_ERR_EXIT(rc);
    
    
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_multicast_egress_delete
 * Purpose:
 *      Delete a port from a multicast group's replication list
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      group    - (IN)  Multicast group ID 
 *      port     - (IN) Physical GPORT ID
 *      encap_id - (IN) Encapsulation ID  
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_egress_delete(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    int rc = BCM_E_NONE, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    DFE_LINK_INPUT_CHECK(unit, port);
  
    DFE_UNIT_LOCK_TAKE(unit);
        
    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), group));
    } 

    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"), group));
    }
  
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_delete,(unit, group, port));
    BCMDNX_IF_ERR_EXIT(rc);
    
   
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_multicast_egress_delete_all
 * Purpose:
 *      Delete all replications for the specified multicast index
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      group - (IN)  Multicast group ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_egress_delete_all(
    int unit, 
    bcm_multicast_t group)
{
    int rc = BCM_E_NONE, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*Valiodation*/
    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), group));
    } 

    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"), group));
    }

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_delete_all,(unit, group));
    BCMDNX_IF_ERR_EXIT(rc);

    
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_multicast_egress_get
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
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_egress_get(
    int unit, 
    bcm_multicast_t group, 
    int port_max, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array, 
    int *port_count)
{
    int rc = BCM_E_NONE, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    BCMDNX_NULL_CHECK(port_array);
    BCMDNX_NULL_CHECK(port_count);
    
    DFE_UNIT_LOCK_TAKE(unit);
      
    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), group));
    } 

    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"), group));
    }

    /*Get reg*/  
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_get,(unit, group, (soc_gport_t)port_max, (soc_gport_t*)port_array, port_count));
    BCMDNX_IF_ERR_EXIT(rc);

    
exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_multicast_egress_set
 * Purpose:
 *      Assign a set of encapsulated interfaces on specific ports as a multicast group's replication list
 * Parameters:
 *      unit           - (IN) Unit number.
 *      group          - (IN) Multicast group ID 
 *      port_count     - (IN) Number of ports in the array
 *      port_array     - (IN) Array of physical ports 
 *      encap_id_array - (IN) Array of encapsulation IDs 
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_dfe_multicast_egress_set(
    int unit, 
    bcm_multicast_t group, 
    int port_count, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array)
{
    int rc = BCM_E_NONE;
    int i, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    BCMDNX_NULL_CHECK(port_array);
    
    for(i = 0 ; i<port_count ; i++)
    {
        DFE_LINK_INPUT_CHECK(unit, port_array[i]);
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"), group));
    } 

    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"), group));
    }
    
    /*Set reg*/  
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_set,(unit, group, port_count, (soc_gport_t*)port_array));
    BCMDNX_IF_ERR_EXIT(rc);
    
    
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME

