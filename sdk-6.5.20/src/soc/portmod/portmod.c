/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/group_member_list.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <shared/periodic_event.h>

        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

/* Terminology:
   ------------
    * Phy - represent a physical lane (serdes)
    * Logical Port (a.k.a just "port") - a phy or group of phys creating a physical interface (e.g. Network Interface) 
        providing Data Link and Physical connectivity between devices
    * PM - Port Macro
    * PM id - each HW PM is represented by SW PM entity, this id is used to identify specific PM
        (This number is internal to the PortMod.
    * PMM - Port Macros Manager
*/

/* Buffer ID for the high level portmod databases*/
#define PMM_WB_BUFFER_ID (0)

/* number to represent invalid PM id*/
#define INVALID_PM_ID (-1)

/* number to represent invalid logical port*/
#define INVALID_PORT (-1)

/* SUB_PHYS_NUM is used for QSGMII where each phy can contain up to 4 logical lanes.
    The way it's handled is by expending each phy to have slots for 4 logical ports 
    (for simplicity of the mapping phys which are candidate to be QSGMII aren't handled separately,
    therefore all the phy are extended by factor 4, regardless in whether QSGMII is valid for this phy*/
#define SUB_PHYS_NUM (4)

/* Max number of phys (lanes) in single logical port (currently derived from ILKN-24)*/
#define MAX_PHYS_PER_PORT (24)

#define MAX_ILKN_BLOCKS_NUM (8)


/* 
PortMod High Level DataBases:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The PortMod high level is called PMM (Port Macros Manager).
This entity holds the requried information for PortMod managment for specific unit.

The PMM contain 2 types of databases:
* Dynamic information (pmm_wb_var_ids)- WB protected 
    - this information is expected to be automatically restored in case of WB
* Static infortmation (pmm_info_t)- not WB protected 
    - this information is expected to be provided by the user in WB initalization 
      (this contain information like: HW description (which port macros exist), function pointers, etc).


The Flow:
~~~~~~~~~

1. When portmod API is called it's first translated to "real" port.
   (when channalized interfaces aren't used it'll be 1-to-1 mapping).
2. From the port PM id is fetched
3. Using the PM id the PM info is extracted (from pms array) and sent as input to internal PM.

Other mappings in the draw:
---------------------------
1. Logical port to interface type
2. Phy to list of PMs (details in the draw)
3. phy to logical port bi-directional mapping.


                                                 +-------------------------------+
                                                 |PMM_WB_PORT_INTERFACE_TYPE_MAP |
                                                 |                               |
                                                 | Map                           |
                                        +--------> Logical Port                  |
                                        |        | -->                           |
                                        |        | Interface Type                |
                                        |        +-------------------------------+
                                        |
   +----------------------+    +----------------------+      +--------------------------------------------+
   |PMM_WB_PORT_ALIAS_MAP |    |PMM_WB_PORT_PM_ID_MAP |      |                                            |
   |                      |    |                      |      |                                            |
   | Map                  |    |  Map                 |      |                                            |
   | Aliased Port         |    |  Logical Port        |      |      +---------------+                     | When accessing with specific port
   | -->                  +--> |  -->                 +------>      |   PM 1        |                     +--------------->
   | Real Port            |    |  PM id               |      |      |               |                     | The result is specific PM id
   |                      |    |                      |      |      |               |                     | (the specific PM id depend on phy + interface type)
   | (See aliasd ports    |    |                      |      |      |      +--------------+               | 
   | section below)       |    |                      |      |      |      |        |     |               |
   +------+-------^-------+    +---------^------------+      |      |      |        |     |               |
          |       |                                          |      |      |        |     |               |
          |       |                                          |      +---------------+     |               |
          |       |                                          |             |    PM 2      |               |
   +------v-------+-------+                                  |             |      +----------------+      |
   |PMM_WB_PORT_DB_PHYS   |                                  |             |      |       |        |      |
   |PMM_WB_PORT_DB_PORTS  |                                  |             +--------------+        |      |
   |                      |                                  |                    |                |      |
   | Map                  |                                  |                    |                |      |
   | Aliased Port         |                                  |                    |     PM n       |      |
   | -->                  |                                  |                    |                |      |
   | Real Port            |                                  |                    |                |      |
   |                      |    +------------------+          |                    +----------------+      |
   +------+-------^-------+    |PMM_WB_PHY_PM_MAP |          |                                            |
          |       |            |                  |          |                                            |
          |       |            | MAP              |          |                                            | When accessing with phy
          |       -------------+ Phy              +---------->       Device Port Macros (pms)             +---------------->
          |                    | -->              |          |                           ---              | the result is list of PM ids
          ---------------------> PM ids           |          |                                            | (there might be several PMs over same phy,
                               |                  |          |                                            | and the selction depened on specific interface type)
                               +------------------+          +--------------------------------------------+



Aliased Ports:
~~~~~~~~~~~~~
Channalized interfaces requires the ability of having several logical ports which are mapped to same physical interface.

How does it work:
1. When port is added on exisiting physical interface with same properties (interface type and phys) it'll be stored as alias port,
   and will point to the "real" port. In this case the internal PM won't be notified at all. 
2. Each port which is passed to portmod API will be  translated first to "real" port. 
   For non channalied interfaces the mapping is always 1-to-1.
3. When "real" port is deleted one of his aliased will be selected as the new "real" port.
   The databases will be updated accordinly and PM will be notied about port number change.
   (HW configuration should bot be affected).
4. When all the ports related to an interface are removed, the port will be detached.

Group Member List:
~~~~~~~~~~~~~~~~~
Manage many to one bi-directional mapping.
In PortMod case the group is the port and the members are the phys.
(See details in group_member_list.c)


WarmBoot:
~~~~~~~~~
PortMod is using WB engine to save and restore dynamic variables.

WB engine use the folowing entities:
 - "Variable" is a single WB protected data unit
    Variables of same buffer must be kept in order to be restored correctly.
    Set \ Get variables is done using WB engine macros. 

 - "Buffer" is a container for WB protected variables
    Buffers must stay in order so they can be restored correctly
    In Portmod buffer id is derived from PM id.

 - "Engine" - is a container for buffers.
    PortMod is using SOC_WB_ENGINE_PORTMOD static engine id.
*/

static int portmod_periodic_event(int unit, void* user_data);

static int portmod_pm_aggregated_update_internal(int unit, pm_info_t pm_info, const portmod_port_add_info_t *port_add_info);


typedef enum pmm_wb_var_ids_e{
    PMM_WB_PORT_PM_ID_MAP, /* See description above */
    PMM_WB_PORT_ALIAS_MAP, /* See description above */
    PMM_WB_PHY_PM_MAP_OLD,     /* See description above */
    PMM_WB_PORT_INTERFACE_TYPE_MAP, /* See description above */
    PMM_WB_PORT_DB_PHYS_OLD,   /* See description above */
    PMM_WB_PORT_DB_PORTS,  /* See description above */
    PMM_WB_XPHY_DB_ADDR,
    PMM_WB_XPHY_DB_VALID_PHYS,
    PMM_WB_PHY_PM_MAP,
    PMM_WB_PORT_DB_PHYS,
    PMM_WB_ILKN_PM_MAP,
    PMM_WB_VARS_COUNT
}pmm_wb_var_ids;

#define PMM_WB_BUFFER_VERSION            (6)

#define PMM_XPHY_VALID_PHYS_SET(unit,  valid_phys)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_VALID_PHYS, &valid_phys);
#define PMM_XPHY_VALID_PHYS_GET(unit,  valid_phys) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_VALID_PHYS, valid_phys);

typedef struct pmm_periodic_per_port_info_s {
    portmod_periodic_event_f ports_callback;
    int interval;
} pmm_periodic_per_port_info_t;

/* This structure contain information required for periodic event managment */
typedef struct pmm_periodic_data_s {
    /* callback for ports */
    pmm_periodic_per_port_info_t *ports_periodic;
    /* Handler for periodic events - see periodic_event.h for details */
    periodic_event_handler_t periodic_handler;
    /* Mutex for interval calculation */
    sal_mutex_t periodic_lock;
    /* current working port */
    int current_working_port;
    /*Disable the port monitor*/
    int portmon_disable;
} pmm_periodic_data_t;

typedef struct pmm_info_s{
    /* the unit number of this PMM. */
    int unit;
    /* this is a feature which allow to map ports to dummy PM which doesn't represent HW entity (PM NULL).
       If the feature is enabled then ports with interface portmodDispatchTypePmNull will be allocated 
       in the dummy PM. (when feature is disabled adding such ports will retrun an error).
       pm_null_support is a boolean indication whether this feature is activated.
       Note that activating \ deactivating this feature between versions isn't Warmboot safe.*/
    int pm_null_support;
    /* number of PMs added so far to the PMM. */
    uint32 pms_in_use;
    /* Array of PMs, accessed by PM id (see drawing above)*/
    pm_info_t pms;
    /* var IDs are uniqe numbers used for WB 
       This number is used to allocate next free WB variable ID
       (See comments about WB above)*/
    uint32 wb_vars_in_use;
    /* the number of phys controlled by this PMM */
    uint32 max_phys;
    /* max number of logical ports allowed in this PMM */
    uint32 max_ports;
    /* phy to logical port bi-directional mapping 
    (see sepcific comment about group member list data structure)*/
    group_member_list_t ports_phys_mapping; 
    /* number of port macros added to this PMM*/
    uint32 max_pms;
    /* priodic events */
    pmm_periodic_data_t periodic;
}pmm_info_t;
  

pmm_info_t *_pmm_info[SOC_MAX_NUM_DEVICES] = {NULL};

STATIC
int portmod_pmm_free(int unit, pmm_info_t *pmm){
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    SOC_NULL_CHECK(pmm);
    if(pmm->pms != NULL){
        for(i = 0; i< pmm->pms_in_use; i++){
            if(pmm->pms[i].pm_data.pm4x25_db != NULL){ /*all members of the union are pointers*/
                LOG_WARN(BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "potential memory leak: pm %d wasn't NULL at pmm free\n"),
                          i));
            }
        }
        sal_free(pmm->pms);
    }
    sal_free(pmm);
exit:
    SOC_FUNC_RETURN; 
}


STATIC
int port_db_port_set(void *user_data, group_entry_id_t group_id, group_entry_t* group){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_SET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, group, group_id);
}

STATIC
int port_db_port_get(void *user_data, group_entry_id_t group_id, group_entry_t* group){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_GET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, group, group_id);
}

STATIC
int port_db_phy_set(void *user_data, member_entry_id_t member_id, member_entry_t* member){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_SET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, member, member_id);
}

STATIC
int port_db_phy_get(void *user_data, member_entry_id_t member_id, member_entry_t* member){
    pmm_info_t *pmm;

    if(user_data == NULL){
        return SOC_E_PARAM;
    }
    pmm = (pmm_info_t *)user_data;
    return SOC_WB_ENGINE_GET_ARR(pmm->unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, member, member_id);
}


int portmod_xphy_db_addr_set(int unit, int idx, int xphy_addr)
{
    int rv;

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, &xphy_addr, idx);
    return rv;
}

int portmod_xphy_db_addr_get(int unit, int idx, int* xphy_addr)
{
    int rv;

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, xphy_addr, idx);
    return rv;
}

/*
 * Function:
 *     portmod_xphy_all_valid_phys_get 
 * Purpose:
 *     To get the list of valid external phys in db. 
 * Parameters:
 *      unit        - (IN) Unit number.
 *      active_phys - (OUT)Bit map list of valid phy
 * Returns:
 *      SOC_E_XXX
 */

int portmod_xphy_all_valid_phys_get(int unit, xphy_pbmp_t *active_phys)
{
    int rv;

    rv = PMM_XPHY_VALID_PHYS_GET(unit, active_phys);
    return rv;
} 

/*
 * Function:
 *    portmod_xphy_valid_phy_set 
 * Purpose:
 *    Record the validity of  a particular external phy. 
 * Parameters:
 *      unit        - (IN) Unit number.
 *      xphy_idx    - (IN) index to db.
 *      valid       - (IN) validity information. TRUE/FALSE
 * Returns:
 *      SOC_E_XXX
 */
int portmod_xphy_valid_phy_set (int unit, int xphy_idx, int valid)
{
    xphy_pbmp_t active_phys;
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PMM_XPHY_VALID_PHYS_GET(unit, &active_phys);
    _SOC_IF_ERR_EXIT(rv);

    if( valid ) {
        XPHY_PBMP_IDX_ADD(active_phys, xphy_idx);
    } else {
        XPHY_PBMP_IDX_REMOVE(active_phys, xphy_idx);
    }
    _SOC_IF_ERR_EXIT(PMM_XPHY_VALID_PHYS_SET(unit, active_phys));
exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *    portmod_xphy_valid_phy_get
 * Purpose:
 *    Retrieve the validity of  a particular external phy.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      xphy_idx    - (IN) index to db.
 *      valid       - (OUT) validity information. TRUE/FALSE
 * Returns:
 *      SOC_E_XXX
 */
int portmod_xphy_valid_phy_get (int unit, int xphy_idx, int *is_valid)
{
    xphy_pbmp_t active_phys;
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PMM_XPHY_VALID_PHYS_GET(unit, &active_phys);
    _SOC_IF_ERR_EXIT(rv);

    *is_valid = 0;

    if( XPHY_PBMP_MEMBER(active_phys, xphy_idx)){
        *is_valid =1;
    }
exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      portmod_xphy_add 
 * Purpose:
 *      Add exteranl phy to the db.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      xphy_addr   - (IN) External phy address
 *      core_access - (IN) Access information for exteranl phy being added. 
 *      xphy_idx    - (OUT) Indicate location of new entry. 
 * Returns:
 *      SOC_E_XXX
 */
int portmod_xphy_add(int unit, int xphy_addr, const phymod_core_access_t* core_access, int* xphy_idx)
{

    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT(portmod_chain_xphy_add(unit, xphy_addr, core_access, xphy_idx));
    if (*xphy_idx != PORTMOD_XPHY_EXISTING_IDX) {
        _SOC_IF_ERR_EXIT(portmod_xphy_valid_phy_set (unit, *xphy_idx, TRUE));
    }
exit:
    SOC_FUNC_RETURN;

}

/*
 * Function:
 *      portmod_xphy_delete
 * Purpose:
 *      Delete exteranl phy from the db.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      xphy_addr   - (IN) External phy address
 * Returns:
 *      SOC_E_XXX
 */

int portmod_xphy_delete(int unit, int xphy_addr)
{

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(portmod_chain_xphy_delete(unit, xphy_addr));   
    /* portmod_xphy_valid_phy_set() was called as part of
        portmod_chain_xphy_delete. */

exit:
    SOC_FUNC_RETURN;

}

/*
 * Function:
 *     portmod_xphy_wb_db_restore 
 * Purpose:
 *      Add exteranl phy to the db.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      xphy_idx    - (IN) External Db index
 *      xphy_addr   - (IN) External phy address
 *      core_access - (IN) Access information for exteranl phy to be restored.
 * Returns:
 *      SOC_E_XXX
 */
int portmod_xphy_wb_db_restore(int unit, int xphy_idx, int xphy_addr, const phymod_core_access_t *core_access)
{
    phymod_core_access_t xphy_core_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_xphy_db_addr_get(unit, xphy_idx, &xphy_addr));
    sal_memcpy(&xphy_core_access, core_access, sizeof(phymod_core_access_t));
    _SOC_IF_ERR_EXIT(portmod_chain_xphy_add_by_index(unit, xphy_idx, xphy_addr, &xphy_core_access));

exit:
    SOC_FUNC_RETURN;
}


int portmod_pm_create_info_internal_t_init(int unit, portmod_pm_create_info_internal_t *create_info_internal)
{
    sal_memset(create_info_internal, 0 , sizeof(*create_info_internal));
    return SOC_E_NONE;
}

/*calculate how many PMs objects are required for each PM type*/
STATIC
int _portmod_pm_type_to_nof_pms(int unit, portmod_dispatch_type_t pm_type, int *nof_pms)
{
    SOC_INIT_FUNC_DEFS;

    switch(pm_type){
#ifdef PORTMOD_PM4X25_SUPPORT
    case portmodDispatchTypePm4x25:
#ifdef PORTMOD_PM4X25TD_SUPPORT
    case portmodDispatchTypePm4x25td:
#endif /* PORTMOD_PM4X25TD_SUPPORT */
#ifdef PORTMOD_CPM4X25_SUPPORT
    case portmodDispatchTypeCpm4x25:
#endif /* PORTMOD_CPM4X25TD_SUPPORT */
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM4X10_SUPPORT
    case portmodDispatchTypePm4x10:
#ifdef PORTMOD_PM4X10TD_SUPPORT
    case portmodDispatchTypePm4x10td:
#endif /*PORTMOD_PM4X10TD_SUPPORT  */
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM12X10_SUPPORT
    case portmodDispatchTypePm12x10:
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
    case portmodDispatchTypePm12x10_xgs:
#endif /*PORTMOD_PM12X10_XGS_SUPPORT  */
        *nof_pms = 5; /*Top, 3 times 4x10, 4X25 */
        break;
#endif /*PORTMOD_PM12X10_SUPPORT  */
#ifdef PORTMOD_PM4x10Q_SUPPORT
    case portmodDispatchTypePm4x10Q:
        *nof_pms = 2; /*Top, 4X10Q */
        break;
#endif /*PORTMOD_PM4x10Q_SUPPORT  */
#ifdef PORTMOD_PM_QTC_SUPPORT
    case portmodDispatchTypePm_qtc:
        *nof_pms = 1; /*Top, QTCs */
        break;
#endif /*PORTMOD_PM_QTC_SUPPORT  */
#ifdef PORTMOD_PM_OS_ILKN_SUPPORT
    case portmodDispatchTypePmOsILKN:
#ifdef PORTMOD_PM_OS_ILKN_50G_SUPPORT
    case portmodDispatchTypePmOsILKN_50G:
#endif /*PORTMOD_PM_OS_ILKN_50G_SUPPORT  */
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM_OS_ILKN_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_SUPPORT
    case portmodDispatchTypeDnx_fabric:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_DNX_FABRIC_SUPPORT*/
#ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
    case portmodDispatchTypeDnx_fabric_o_nif:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT  */
#ifdef PORTMOD_PM8X50_FABRIC_SUPPORT
    case portmodDispatchTypePm8x50_fabric:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM8X50_FABRIC_SUPPORT*/
#ifdef PORTMOD_PM4X2P5_SUPPORT
    case portmodDispatchTypePm4x2p5:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM4X2P5_SUPPORT  */
#ifdef PORTMOD_PM8X50_SUPPORT
    case portmodDispatchTypePm8x50:
        *nof_pms = 1;
        break;
#endif /*PORTMOD_PM8X50_SUPPORT*/
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
        case portmodDispatchTypePm4x10_qtc:
            *nof_pms = 2;
            break;
#endif /*PORTMOD_PM4X10_QTC_SUPPORT*/
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Can't retrieve number of warmboot buffers for the specified PM type %d"), pm_type));
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_wb_debug_log (int unit)
{
    int rv;
    int xphy_idx, xphy_addr;
    int port = 0, phy, i, pm_id, master_port;
    int core;
    soc_port_if_t interface;
    member_entry_id_t member_id;
    member_entry_t member;
    xphy_pbmp_t active_phys;
    uint32 group;
    pm_info_t pm_info;

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{port,ports_to_pm_id_mapping}\n")));

    for (port = 0 ; port < _pmm_info[unit]->max_ports ; port++) {

        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), port, pm_id));
        }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{port,ports_to_pm_id_mapping}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{port,ports_alias}\n")));
    for (port = 0 ; port < _pmm_info[unit]->max_ports ; port++) {

        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port, port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), port, master_port));
        }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{port,ports_alias}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{port,port_interface_type}\n")));

    for (port = 0 ; port < _pmm_info[unit]->max_ports ; port++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), port, interface));
        }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{port,port_interface_type}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{port,port_db_ports}\n")));

    for (port = 0 ; port < _pmm_info[unit]->max_ports ; port++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, &group, port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), port, group));
        }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{port,port_db_ports}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{phy,pm_idx,phys_to_pm_ids_old}\n")));

    for (phy = 0; phy < (_pmm_info[unit]->max_phys - 1); phy++) {
        for (i = 0; i < MAX_PMS_PER_PHY; i++) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit, "\n")));
            rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP_OLD, phy, i, (uint8*)&pm_id);
            if (SOC_E_NONE == rv) {
                LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                                "{%d,%d,%d}\n"), phy, i, pm_id));
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{phy,pm_idx,phys_to_pm_ids_old}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{phy,pm_idx,phys_to_pm_ids_new}\n")));
    for (phy = 0; phy < (_pmm_info[unit]->max_phys - 1); phy++) {
        for (i = 0; i < MAX_PMS_PER_PHY; i++) {
            rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8*)&pm_id);
            if (SOC_E_NONE == rv) {
                LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                                "{%d,%d,%d}\n"), phy, i, pm_id));
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{phy,pm_idx,phys_to_pm_ids_new}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{member_id,port_db_phys_old.next,port_db_phys_old.prev,port_db_phys_old.group}\n")));

    for (member_id = 0;
         member_id < (_pmm_info[unit]->ports_phys_mapping.members_count - SUB_PHYS_NUM);
         member_id++) {
         rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS_OLD, &member, member_id);

         if (SOC_E_NONE == rv) {
             LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                             "{%d,%d,%d,%d}\n"), member_id, member.next, member.prev, member.group));
         }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{member_id,port_db_phys_old.next,port_db_phys_old.prev,port_db_phys_old.group}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{member_id,port_db_phys_new.next,port_db_phys_new.prev,port_db_phys_new.group}\n")));

    for (member_id = 0;
         member_id < _pmm_info[unit]->ports_phys_mapping.members_count;
         member_id++) {
         rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, &member, member_id);

         if (SOC_E_NONE == rv) {
             LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                             "{%d,%d,%d,%d}\n"), member_id, member.next, member.prev, member.group));
         }
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{member_id,port_db_phys_new.next,port_db_phys_new.prev,port_db_phys_new.group}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{xphy_idx,xphy_db_addr}\n")));

    for (xphy_idx=0; xphy_idx < PORTMOD_MAX_NUM_XPHY_SUPPORTED; xphy_idx++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, &xphy_addr, xphy_idx);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit, "\n")));
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), xphy_idx, xphy_addr));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{xphy_idx,xphy_db_addr}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{xphy_db_valid_phys}\n")));

    rv = PMM_XPHY_VALID_PHYS_GET(unit, &active_phys);
    if (SOC_E_NONE == rv) {
        for (i = 0; i < XPHY_PBMP_WORD_MAX; i++) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{0x%x}\n"), active_phys.pbits[i]));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{xphy_db_valid_phys}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pmm:{ilkn_core,ilkn_cores_to_pm_ids}\n")));

    for(core = 0 ; core < MAX_ILKN_BLOCKS_NUM ; core++){
        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_ILKN_PM_MAP, &pm_id, core);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), core, pm_id));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm:{ilkn_core,ilkn_cores_to_pm_ids}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pmm\n")));

    /* Walk through individual port macros and print warmboot scache info */
    for (i = 0; i < _pmm_info[unit]->pms_in_use; i++) {

        rv = portmod_pm_info_from_pm_id_get(unit, i, &pm_info);

        if (SOC_E_NONE != rv) {
            continue;
        }

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
        if (pm_info->type == portmodDispatchTypePm4x10_qtc) {

            /* pm4x10_qtc driver includes pm4x10_qtc and pm4x10.
             * The pm_info for subsequent PM (pm4x10) is under
             * pm_info->pm_data.pm4x10_qtc_db->pm4x10 and is not
             * accessible here. Hence, skip pm4x10 until the support
             * for pm4x10_qtc is added.
             *
             * When the support is being added for pm4x10_qtc, the
             * logging function for pm4x10 should be internally invoked from
             * pm4x10_qtc driver.
             */
            i++;
        }
#endif

        /* 
         * The below 'if' check can be removed when debug logging support is
         * added for all PMs
         */
        if (
#ifdef PORTMOD_PM4X25_SUPPORT
                (pm_info->type == portmodDispatchTypePm4x25) ||
#endif
#ifdef PORTMOD_PM4X10_SUPPORT
                (pm_info->type == portmodDispatchTypePm4x10) ||
#endif
#ifdef PORTMOD_PM8x50_SUPPORT
                (pm_info->type == portmodDispatchTypePm8x50) ||
#endif
           0)
        {
            portmod_pm_wb_debug_log(unit, pm_info);
        }
    }

    return SOC_E_NONE;
}

STATIC
int _portmod_max_pms_get(int unit, int nof_pm_instances, const portmod_pm_instances_t* pm_instances, int *max_pms)
{
    int i = 0;
    int nof_pms = 0;
    SOC_INIT_FUNC_DEFS;

    *max_pms = 0;

    for( i = 0 ; i < nof_pm_instances; i++){
        _SOC_IF_ERR_EXIT(_portmod_pm_type_to_nof_pms(unit, pm_instances[i].type, &nof_pms));
        *max_pms  += nof_pms* pm_instances[i].instances;
    }
exit:
    SOC_FUNC_RETURN;
}

int portmod_wb_upgrade_func(int unit, void* arg, int recovered_version, int new_version) 
{
    int port, phy, pm_id, rv, i;
    xphy_pbmp_t active_phys;
    int xphy_idx;
    int xphy_addr;
    uint32 invalid_pm_id = INVALID_PM_ID;
    member_entry_id_t member_id;
    member_entry_t member, member_end;
    SOC_INIT_FUNC_DEFS;

    /*
     * Change from version 1 to version 2:
     * pmNull was added as pm_id 0 (optionaly)
     * once it's done it pushes all other pm ids +1.
     */
    if (recovered_version == 1 &&
        new_version >= 2 &&
        _pmm_info[unit]->pm_null_support) {

        for (port=0 ; port<_pmm_info[unit]->max_ports ; port++) {
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
            _SOC_IF_ERR_EXIT(rv);
            if(pm_id != INVALID_PM_ID){
                pm_id++;
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
        for(phy=0;phy<(_pmm_info[unit]->max_phys - 1);phy++){
            for(i=0;i<MAX_PMS_PER_PHY;i++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit,SOC_WB_ENGINE_PORTMOD,PMM_WB_PHY_PM_MAP_OLD,phy,i, (uint8*)&pm_id));
                if(pm_id!=INVALID_PM_ID){
                    pm_id++;
                    _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit,SOC_WB_ENGINE_PORTMOD,PMM_WB_PHY_PM_MAP_OLD,phy,i, (uint8*)&pm_id));
                }
            }
        }
    }

    if (recovered_version <= 3 &&
        new_version >= 4) {

        /*
         * if upgrading from older version which do not have valid_phys,
         * it need to get cleared.
         */
        sal_memset(&active_phys,0, sizeof(xphy_pbmp_t));
        rv = PMM_XPHY_VALID_PHYS_SET(unit, active_phys);
        _SOC_IF_ERR_EXIT(rv);

        /*
         * if xphy_addr has valid address, that mean that phy is valid.
         */
        for (xphy_idx=0; xphy_idx < PORTMOD_MAX_NUM_XPHY_SUPPORTED; xphy_idx++) {
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, &xphy_addr, xphy_idx);
            _SOC_IF_ERR_EXIT(rv);

            if(xphy_addr != INVALID_PM_ID){
                rv = portmod_xphy_valid_phy_set (unit, xphy_idx, TRUE);
            }
        }
    }

    if (recovered_version <= 4 &&
        new_version >= 5) {
        for(phy = 0; phy < (_pmm_info[unit]->max_phys - 1); phy++){
            for(i = 0; i < MAX_PMS_PER_PHY; i++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP_OLD, phy, i, (uint8*)&pm_id));
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8*)&pm_id));
            }
        }
        /* init as INVALID_PM_ID for the last phy id */
        for(i = 0; i < MAX_PMS_PER_PHY; i++){
            _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, _pmm_info[unit]->max_phys - 1, i, (uint8*)&invalid_pm_id));
        }

        for(member_id = 0; member_id < (_pmm_info[unit]->ports_phys_mapping.members_count - SUB_PHYS_NUM); member_id++){
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS_OLD, &member, member_id));
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, &member, member_id));
        }
        /* init the last phy member */
        member_end.group = GROUP_MEM_LIST_END;
        member_end.next = GROUP_MEM_LIST_END;
        member_end.prev = GROUP_MEM_LIST_END;
        for(member_id = (_pmm_info[unit]->ports_phys_mapping.members_count - SUB_PHYS_NUM);
            member_id < _pmm_info[unit]->ports_phys_mapping.members_count; member_id++){
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, &member_end, member_id));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int portmod_create(int unit, int flags, int max_ports, int max_phys, int nof_pm_instances, const portmod_pm_instances_t* pm_instances)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int buffer_id;
    int rv;
    int max_pms = 0;
    int pm_inst = 0;
    pmm_info_t *pmm = NULL;
    portmod_pm_create_info_t pm_null_create_info;
    int max_wb_buffer_ids = 0;
    int max_wb_var_ids = 0 ;
    int max_wb_buffer_ids_pm = 0;
    int max_wb_buffer_ids_xphy = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    _SOC_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_null_create_info));

    _SOC_IF_ERR_EXIT(portmod_pm_instances_t_validate(unit, pm_instances));

    if(nof_pm_instances <= 0){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("nof_pm_instances must be > 0")));
    }

    for(pm_inst = 0; pm_inst < nof_pm_instances; pm_inst++) {
        _SOC_IF_ERR_EXIT(portmod_pm_instances_t_validate(unit, &pm_instances[pm_inst]));
    }

    if(_pmm_info[unit] != NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod already created for the unit")));
    }

    _SOC_IF_ERR_EXIT(_portmod_max_pms_get(unit, nof_pm_instances, pm_instances, &max_pms));

    if (PORTMOD_CREATE_F_PM_NULL_GET(flags)) {
        max_pms++;
    }

    pmm = sal_alloc(sizeof(pmm_info_t), "unit pmm");
    SOC_NULL_CHECK(pmm);
    _pmm_info[unit] = pmm;
    pmm->pm_null_support = 0;
#ifdef PORTMOD_PMNULL_SUPPORT
    if (PORTMOD_CREATE_F_PM_NULL_GET(flags)) {
        pmm->pm_null_support = 1;
    }
#endif /* PORTMOD_PMNULL_SUPPORT */
    pmm->pms = NULL;

    /*the pms are not saved in WB*/
    pmm->pms = sal_alloc(sizeof(struct pm_info_s)*max_pms, "port_macros");
    SOC_NULL_CHECK(pmm->pms);
    sal_memset(pmm->pms, 0, sizeof(struct pm_info_s)*max_pms);

    /*callback allocation*/    
    pmm->periodic.ports_periodic = sal_alloc(sizeof(pmm_periodic_per_port_info_t)*max_ports, "ports_periodic");
    SOC_NULL_CHECK(pmm->periodic.ports_periodic);
    sal_memset(pmm->periodic.ports_periodic, 0, sizeof(pmm_periodic_per_port_info_t)*max_ports);

    /* periodic mutex allocation */
    _pmm_info[unit]->periodic.periodic_lock = sal_mutex_create("ports_periodic lock");
    SOC_NULL_CHECK(_pmm_info[unit]->periodic.periodic_lock);

    /* mark not currently working port */
    _pmm_info[unit]->periodic.current_working_port = -1;

    /*portmon disable*/
    if (PORTMOD_CREATE_F_PORTMOD_THREAD_DISABLE_GET(flags)) {
        _pmm_info[unit]->periodic.portmon_disable = 1;
    } else {
        _pmm_info[unit]->periodic.portmon_disable = 0;
    }

    pmm->pms_in_use = 0;
    pmm->ports_phys_mapping.groups_count = max_ports;
    pmm->ports_phys_mapping.members_count = (max_phys + 1)* SUB_PHYS_NUM;
    pmm->ports_phys_mapping.user_data = pmm;
    pmm->ports_phys_mapping.group_set = port_db_port_set;
    pmm->ports_phys_mapping.group_get = port_db_port_get;
    pmm->ports_phys_mapping.member_set = port_db_phy_set;
    pmm->ports_phys_mapping.member_get = port_db_phy_get;
    pmm->max_phys = max_phys + 1;
    pmm->max_ports = max_ports;
    pmm->max_pms = max_pms;
    pmm->unit = unit;
    pmm->wb_vars_in_use = PMM_WB_VARS_COUNT;

    buffer_id = PMM_WB_BUFFER_ID;

    max_wb_buffer_ids_pm = max_pms + 1;
    max_wb_buffer_ids_xphy = PORTMOD_MAX_NUM_XPHY_SUPPORTED;
    max_wb_var_ids = (max_wb_buffer_ids_pm * MAX_VARS_PER_BUFFER) +
                     (max_wb_buffer_ids_xphy * MAX_VARS_PER_XPHY_BUFFER);
    max_wb_buffer_ids = max_wb_buffer_ids_pm + max_wb_buffer_ids_xphy;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_tables(unit, SOC_WB_ENGINE_PORTMOD, max_wb_buffer_ids, max_wb_var_ids ));

    /*
     * Buffer version number need to be bumped up ever time there is a change in buffer content.  This version will keep track of
     * what content are in the buffer. Since we are adding xphy_db_valid_phys, need to bump the number.
     */
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, PMM_WB_BUFFER_ID, "pmm_buffer", portmod_wb_upgrade_func, NULL, PMM_WB_BUFFER_VERSION, 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, "ports_to_pm_id_mapping", PMM_WB_BUFFER_ID, sizeof(int), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, "ports_interface_type", PMM_WB_BUFFER_ID, sizeof(soc_port_if_t), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, "ports_alias", PMM_WB_BUFFER_ID, sizeof(int), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP_OLD, "phys_to_pm_ids", PMM_WB_BUFFER_ID, sizeof(int), NULL, max_phys, MAX_PMS_PER_PHY, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS_OLD, "port_db_phys", PMM_WB_BUFFER_ID, group_member_list_member_entry_size_get(), NULL, max_phys*SUB_PHYS_NUM, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PORTS, "port_db_ports", PMM_WB_BUFFER_ID, group_member_list_group_entry_size_get(), NULL, max_ports, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, "xphy_db_addr", PMM_WB_BUFFER_ID, sizeof(int), NULL, PORTMOD_MAX_NUM_XPHY_SUPPORTED, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, "phys_to_pm_ids_new", PMM_WB_BUFFER_ID, sizeof(int), NULL, pmm->max_phys, MAX_PMS_PER_PHY, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_DB_PHYS, "port_db_phys_new", PMM_WB_BUFFER_ID, group_member_list_member_entry_size_get(), NULL, pmm->ports_phys_mapping.members_count, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);

    /*
     * When new warmboot content is added, it need to identify which version of the buffer is added.  So that warmboot engine can keep track of
     * when to look for this content based on Buffer version.
     */
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_VALID_PHYS, "xphy_db_valid_phys", PMM_WB_BUFFER_ID, sizeof(xphy_pbmp_t), NULL,VERSION(4));
    _SOC_IF_ERR_EXIT(rv);

    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, PMM_WB_ILKN_PM_MAP, "ilkn_cores_to_pm_ids", PMM_WB_BUFFER_ID, sizeof(int), NULL, MAX_ILKN_BLOCKS_NUM, VERSION(6));
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_BUFFER_ID, FALSE));

    /* print scache information for debugging during warmboot */
    if (SOC_WARM_BOOT(unit)) {
        portmod_wb_debug_log(unit);
    }

    if(!SOC_WARM_BOOT(unit)){ /*Cold boot*/
        int phy_id, pm, core;
        uint32 invalid_pm_id = INVALID_PM_ID;
        xphy_pbmp_t xphy_valid_pbmp ;
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, INVALID_PM_ID);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, SOC_PORT_IF_NULL);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, INVALID_PORT);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, INVALID_PM_ID);
        _SOC_IF_ERR_EXIT(rv);
        sal_memset(&xphy_valid_pbmp,0, sizeof(xphy_pbmp_t)); 
        rv = PMM_XPHY_VALID_PHYS_SET(unit,xphy_valid_pbmp);

        /* Set all PM ids in PMM_WB_PHY_PM_MAP to invalid */
        for(phy_id = 0 ; phy_id < pmm->max_phys ; phy_id++){
            for(pm = 0 ; pm < MAX_PMS_PER_PHY ; pm++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy_id, pm, (uint8 *)&invalid_pm_id));
            }
        }
        /* Set all PM ids in PMM_WB_ILKN_PM_MAP to invalid */
        for(core = 0 ; core < MAX_ILKN_BLOCKS_NUM ; core++){
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_ILKN_PM_MAP, &invalid_pm_id, core));
        }
        _SOC_IF_ERR_EXIT(group_member_list_init(&pmm->ports_phys_mapping));
    }

    if (SOC_WARM_BOOT(unit)) {
        int xphy_id, xphy_addr;
        for (xphy_id=0; xphy_id<PORTMOD_MAX_NUM_XPHY_SUPPORTED; xphy_id++) {
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_XPHY_DB_ADDR, &xphy_addr, xphy_id);
            _SOC_IF_ERR_EXIT(rv);
            portmod_xphy_addr_set(unit, xphy_id, xphy_addr);
        }
    }

    _pmm_info[unit] = pmm;

#ifdef PORTMOD_PMNULL_SUPPORT
    if (PORTMOD_CREATE_F_PM_NULL_GET(flags)) {
        pm_null_create_info.type = portmodDispatchTypePmNull;
        rv = portmod_port_macro_add(unit, &pm_null_create_info);
        _SOC_IF_ERR_EXIT(rv);
    }
#endif /* PORTMOD_PMNULL_SUPPORT */


    /* register periodic event */
    {
#define _PORTMOD_PE_NAME_MAX 30

        char pe_name[_PORTMOD_PE_NAME_MAX];
        periodic_event_config_t pe_config;
        int name_size;
        periodic_event_config_t_init(&pe_config);

        name_size = sal_snprintf(pe_name, _PORTMOD_PE_NAME_MAX, "Portmod Periodic.%d", unit);
        if(name_size >= _PORTMOD_PE_NAME_MAX){
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Failed to create periodic name")));
        }

        pe_config.name = pe_name;
        pe_config.bsl_module = _ERR_MSG_MODULE_NAME;
        pe_config.interval = 1000000; /* 1 sec - doesn't really matter */
        pe_config.callback = &portmod_periodic_event;
        pe_config.start_operation = 0;
        pe_config.error_threshold = 10; /* exit if too much failures */

        _SOC_IF_ERR_EXIT(periodic_event_create(unit, &pe_config, &(pmm->periodic.periodic_handler)));
    }

    

exit:
    /*free memories in case of error*/
    if (SOC_FUNC_ERROR){
        if(pmm != NULL){
            portmod_pmm_free(unit, pmm);
        }
    }
    SOC_FUNC_RETURN; 
}



int portmod_destroy(int unit)
{
    int i;
    pm_info_t pm_info = NULL;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    _SOC_IF_ERR_EXIT(periodic_event_destroy(&(_pmm_info[unit]->periodic.periodic_handler)));

    _pmm_info[unit]->periodic.current_working_port = -1;

    if (_pmm_info[unit]->periodic.periodic_lock != NULL) {
        sal_mutex_destroy(_pmm_info[unit]->periodic.periodic_lock);
    }

    if (_pmm_info[unit]->periodic.ports_periodic != NULL) {
        sal_free(_pmm_info[unit]->periodic.ports_periodic);
        _pmm_info[unit]->periodic.ports_periodic = NULL;
    }

    if(_pmm_info[unit]->pms != NULL){
        for(i = 0; i< _pmm_info[unit]->pms_in_use; i++){
            _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, i, &pm_info));
            _SOC_IF_ERR_EXIT(portmod_pm_destroy(unit, pm_info));
        }
    }

#ifdef CPRIMOD_SUPPORT 
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_cpm_interrupt_data_init(unit));
#endif

    portmod_chain_xphy_user_access_release(unit);
    _SOC_IF_ERR_EXIT(portmod_chain_xphy_delete_all(unit));

    _SOC_IF_ERR_EXIT(soc_wb_engine_deinit_tables(unit, SOC_WB_ENGINE_PORTMOD));
    portmod_pmm_free(unit, _pmm_info[unit]);
    _pmm_info[unit] = NULL;

exit:
    SOC_FUNC_RETURN;   
}

int portmod_thread_stop(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    _SOC_IF_ERR_EXIT(periodic_event_stop(_pmm_info[unit]->periodic.periodic_handler, FALSE));

exit:
    SOC_FUNC_RETURN;   
}


#if defined(PORTMOD_PM4X25_SUPPORT) || defined(PORTMOD_PM4X10_SUPPORT) || defined(PORTMOD_DNX_FABRIC_SUPPORT) || defined(PORTMOD_PM12X10_SUPPORT) || defined(PORTMOD_PM4x10Q_SUPPORT) || defined(PORTMOD_PM_OS_ILKN_SUPPORT) || defined(PORTMOD_PM8X50_FABRIC_SUPPORT) || defined(PORTMOD_PM8X50_SUPPORT) \
    || defined(PORTMOD_PM4X10_QTC_SUPPORT)
STATIC
int _portmod_port_macro_internal_add(int unit , const portmod_pm_create_info_internal_t* pm_add_info, int *pm_created_id)
{
    int pm_id, free_slot_found, current_val, i;
    int phy = 0;
    uint32 invalid_pm_id = INVALID_PM_ID;
    int pm_initialized = FALSE; 
    int should_add_to_map = TRUE;
    int wb_buffer_id;
    SOC_INIT_FUNC_DEFS;
    
    pm_id = _pmm_info[unit]->pms_in_use;

/*in case of ilkn interface the PM map sw-state is updated per ilkn block id  */
#if defined(PORTMOD_PM_OS_ILKN_50G_SUPPORT)
    if  (pm_add_info->type == portmodDispatchTypePmOsILKN_50G )
    {
        /* Store the newly created Pm-ID in the Ilkn-core to PM-ID Sw-state */
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_ILKN_PM_MAP, &pm_id, pm_add_info->pm_specific_info.os_ilkn.ilkn_block_index));
    }
#endif /* defined(PORTMOD_PM_OS_ILKN_50G_SUPPORT) */
#if defined(PORTMOD_PM_OS_ILKN_SUPPORT)
    if  (pm_add_info->type == portmodDispatchTypePmOsILKN )
    {
        /* Store the newly created Pm-ID in the Ilkn-core to PM-ID Sw-state */
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_ILKN_PM_MAP, &pm_id, pm_add_info->pm_specific_info.os_ilkn.ilkn_block_index));
    }
#endif /* defined(PORTMOD_PM_OS_ILKN_SUPPORT) */


    /* WB buffer ID is used to identify the specific PM database
    WB buffer order must be kept to insure WB and ISSU works.
    Buffers order:
        0 - Global Portmod DB
        1..n-1 - PMs buffers.
            when PmNull present it takes pm_id 0, so wb_buffer_id==pm_id
            when it's not present wb_buffer_id==pm_id+1
            The PmNull always tales last buffer
    */
    if (_pmm_info[unit]->pm_null_support) {
#ifdef PORTMOD_PMNULL_SUPPORT
        if (pm_add_info->type == portmodDispatchTypePmNull) {
            wb_buffer_id = _pmm_info[unit]->max_pms;
        } else 
#endif
        {
            wb_buffer_id = pm_id;
        }
    } else {
        wb_buffer_id = pm_id + 1;
    }

    _SOC_IF_ERR_EXIT(portmod_pm_init(unit, pm_add_info, wb_buffer_id, &_pmm_info[unit]->pms[pm_id]));
    pm_initialized = TRUE;
    if(!SOC_WARM_BOOT(unit)){
        /*add to map just in case of cold boot*/
        PORTMOD_PBMP_ITER(pm_add_info->phys, phy){
            free_slot_found = FALSE;
            for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
                _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&current_val));
                if(current_val == INVALID_PM_ID && pm_add_info->type == _pmm_info[unit]->pms[pm_id].type){
                    _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm_id));
                    free_slot_found = TRUE;
                    break;
                }
            }
            if(!free_slot_found){
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("phy (%d) already used by the maximum number of pms %d"), phy, MAX_PMS_PER_PHY));
            }
        }
    }

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    /*pm4x10_qtc driver includes pm4x10_qtc and pm4x10 init.*/
    if (pm_add_info->type == portmodDispatchTypePm4x10_qtc) {
        _pmm_info[unit]->pms_in_use += 2;
    } else
#endif
    {
        _pmm_info[unit]->pms_in_use++;
    }
    *pm_created_id = pm_id;
exit:
    if (SOC_FUNC_ERROR){
        if(pm_initialized){

            portmod_pm_destroy(unit, &_pmm_info[unit]->pms[pm_id]);
        }
        /*clean the map*/
        if(should_add_to_map){
            PORTMOD_PBMP_ITER(pm_add_info->phys, phy){
                /* coverity[dead_error_line] */
                for(i = 0 ; !_rv && (i < MAX_PMS_PER_PHY); i++){
                    _rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&current_val);
                    if(!_rv && (current_val == pm_id)){
                        _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&invalid_pm_id));
                        break;
                    }
                }
            }
        }
    }
    SOC_FUNC_RETURN;   
}
#endif /* defined(PORTMOD_PM4X25_SUPPORT) || defined(PORTMOD_PM4X10_SUPPORT) || defined(PORTMOD_DNX_FABRIC_SUPPORT) || defined(PORTMOD_PM12X10_SUPPORT) || defined(PORTMOD_PM4x10Q_SUPPORT) || defined(PORTMOD_PM_OS_ILKN_SUPPORT) || defined(PORTMOD_PM8X50_FABRIC_SUPPORT) */


#if defined(PORTMOD_PM4X25_SUPPORT) || \
    defined(PORTMOD_PM4X10_SUPPORT) || \
    defined(PORTMOD_DNX_FABRIC_SUPPORT) || \
    defined(PORTMOD_PM8X50_FABRIC_SUPPORT) || \
    defined(PORTMOD_PM8X50_SUPPORT) || \
    defined(PORTMOD_PM4X10_QTC_SUPPORT)
STATIC
int _portmod_simple_pm_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t create_info;
    int specific_info_size = 0;
    int pm_id = 0;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_ASSIGN(create_info.phys, pm_add_info->phys);
    create_info.type = pm_add_info->type;

    switch(pm_add_info->type){
#ifdef PORTMOD_PM4X25_SUPPORT
    case portmodDispatchTypePm4x25:
#ifdef PORTMOD_PM4X25TD_SUPPORT
    case portmodDispatchTypePm4x25td:
#endif /* PORTMOD_PM4X25TD_SUPPORT */
#ifdef PORTMOD_CPM4X25_SUPPORT
    case portmodDispatchTypeCpm4x25:
#endif /* PORTMOD_CPM4X25TD_SUPPORT */
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm4x25);
        break;
#endif /*PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM4X10_SUPPORT
    case portmodDispatchTypePm4x10:
#ifdef PORTMOD_PM4X10TD_SUPPORT
    case portmodDispatchTypePm4x10td:
#endif /*PORTMOD_PM4X10TD_SUPPORT  */
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm4x10);
        break;
#endif /*PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_SUPPORT
    case portmodDispatchTypeDnx_fabric:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.dnx_fabric);
        break;
#endif /*PORTMOD_DNX_FABRIC_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
    case portmodDispatchTypeDnx_fabric_o_nif:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.dnx_fabric);
        break;
#endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT  */
#ifdef PORTMOD_PM8X50_FABRIC_SUPPORT
    case portmodDispatchTypePm8x50_fabric:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm8x50_fabric);
        break;
#endif /*PORTMOD_PM8X50_FABRIC_SUPPORT  */
#ifdef PORTMOD_PMNULL_SUPPORT
    case portmodDispatchTypePmNull:
        specific_info_size = 0;
        break;
#endif /*PORTMOD_PMNULL_SUPPORT  */
#ifdef PORTMOD_PM_QTC_SUPPORT
    case portmodDispatchTypePm_qtc:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm_qtc);
        break;
#endif /*PORTMOD_PM_QTC_SUPPORT  */
#ifdef PORTMOD_PM4X2P5_SUPPORT
    case portmodDispatchTypePm4x2p5:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm4x2p5);
        break;
#endif /*PORTMOD_PM4X2P5_SUPPORT  */
#ifdef PORTMOD_PM8X50_SUPPORT
    case portmodDispatchTypePm8x50:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm8x50);
        break;
#endif /*PORTMOD_PM8X50_SUPPORT  */
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    case portmodDispatchTypePm4x10_qtc:
        specific_info_size = sizeof(pm_add_info->pm_specific_info.pm4x10_qtc);
        break;
#endif /*PORTMOD_PM4X10_QTC_SUPPORT  */
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unknown PM type %d"), pm_add_info->type));
    }
    sal_memcpy(&create_info.pm_specific_info, &pm_add_info->pm_specific_info, specific_info_size);
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &create_info, &pm_id));
exit:
    SOC_FUNC_RETURN;  
}
#endif



#ifdef PORTMOD_PM12X10_SUPPORT
STATIC
int _portmod_pm12x10_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    int i = 0;
    int phy;
    int nof_phys = 0;
    int pm4x10_index = 0;
    portmod_pm_create_info_internal_t pm4x10_create_info;
    portmod_pm_create_info_internal_t pm4x25_create_info;
    portmod_pm_create_info_internal_t pm12x10_create_info;
    int pm_ids[] = {INVALID_PM_ID, INVALID_PM_ID, INVALID_PM_ID, INVALID_PM_ID, INVALID_PM_ID};
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_COUNT(pm_add_info->phys, nof_phys);
    if(nof_phys != 12){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("number of phys should be 12")));
    }

    /*PM4x10*/
    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm4x10_create_info));

    PORTMOD_PBMP_ITER(pm_add_info->phys, phy){
        if( i % 4 == 0){
            if(i != 0){
                _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10_create_info, &pm_ids[pm4x10_index]));
                pm4x10_index ++;
            }
            _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm4x10_create_info));
            sal_memcpy(&pm4x10_create_info.pm_specific_info.pm4x10,&pm_add_info->pm_specific_info.pm12x10.pm4x10_infos[pm4x10_index], sizeof(pm4x10_create_info.pm_specific_info.pm4x10));
#ifdef PORTMOD_PM4X10TD_SUPPORT
            if (PORTMOD_PM12x10_F_USE_PM_TD_GET(pm_add_info->pm_specific_info.pm12x10.flags)) { 
                pm4x10_create_info.type = portmodDispatchTypePm4x10td;
            } else
#endif /* PORTMOD_PM4X10TD_SUPPORT */
            {
                pm4x10_create_info.type = portmodDispatchTypePm4x10;
            }
            pm4x10_create_info.pm_specific_info.pm4x10.in_pm_12x10 = TRUE;
        }
        PORTMOD_PBMP_PORT_ADD(pm4x10_create_info.phys, phy);
        i++;
    }
    /*add the last one*/
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10_create_info, &pm_ids[pm4x10_index]));

    /*PM4x25*/
    i = 0;       
    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm4x25_create_info));
#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (PORTMOD_PM12x10_F_USE_PM_TD_GET(pm_add_info->pm_specific_info.pm12x10.flags)) {
        pm4x25_create_info.type = portmodDispatchTypePm4x25td;
    } else 
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        pm4x25_create_info.type = portmodDispatchTypePm4x25;
    }
    pm4x25_create_info.pm_specific_info.pm4x25.in_pm_12x10 = TRUE;
    PORTMOD_PBMP_ITER(pm_add_info->phys, phy){
        if( i == 4){
            break;
        }
        PORTMOD_PBMP_PORT_ADD(pm4x25_create_info.phys, phy);
        i++;
    }
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x25_create_info, &pm_ids[3]));
    /*PM12x10*/
    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm12x10_create_info));
    PORTMOD_PBMP_ASSIGN(pm12x10_create_info.phys, pm_add_info->phys);
    pm12x10_create_info.pm_specific_info.pm12x10.flags = pm_add_info->pm_specific_info.pm12x10.flags;
    pm12x10_create_info.type = pm_add_info->type;
    pm12x10_create_info.pm_specific_info.pm12x10.blk_id = pm_add_info->pm_specific_info.pm12x10.blk_id;
    pm12x10_create_info.pm_specific_info.pm12x10.pm4x25 = &_pmm_info[unit]->pms[pm_ids[3]];
    for(i = 0 ; i < 3 ; i++)
    {
        pm12x10_create_info.pm_specific_info.pm12x10.pm4x10[i] = &_pmm_info[unit]->pms[pm_ids[i]];
    }
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm12x10_create_info, &pm_ids[4]));

exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_PM12X10_SUPPORT*/




#ifdef PORTMOD_PM4x10Q_SUPPORT
STATIC
int _portmod_pm4x10q_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t pm4x10_create_info;
    portmod_pm_create_info_internal_t pm4x10q_create_info;
    int pm4x10_id = INVALID_PM_ID;
    int pm4x10q_id = INVALID_PM_ID;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm4x10_create_info));
    sal_memcpy(&pm4x10_create_info.pm_specific_info.pm4x10, &pm_add_info->pm_specific_info.pm4x10q.pm4x10_info, sizeof(pm4x10_create_info.pm_specific_info.pm4x10));
    pm4x10_create_info.type = portmodDispatchTypePm4x10;
    PORTMOD_PBMP_ASSIGN(pm4x10_create_info.phys, pm_add_info->phys);
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10_create_info, &pm4x10_id));
  
    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm4x10q_create_info));
    pm4x10q_create_info.type = portmodDispatchTypePm4x10Q;
    PORTMOD_PBMP_ASSIGN(pm4x10q_create_info.phys, pm_add_info->phys);
    pm4x10q_create_info.pm_specific_info.pm4x10q.pm4x10 = &_pmm_info[unit]->pms[pm4x10_id];
    pm4x10q_create_info.pm_specific_info.pm4x10q.blk_id = pm_add_info->pm_specific_info.pm4x10q.blk_id;
    pm4x10q_create_info.pm_specific_info.pm4x10q.core_clock_khz = pm_add_info->pm_specific_info.pm4x10q.core_clock_khz;
    pm4x10q_create_info.pm_specific_info.pm4x10q.qsgmii_user_acc = pm_add_info->pm_specific_info.pm4x10q.qsgmii_user_acc;
    pm4x10q_create_info.pm_specific_info.pm4x10q.pm4x10_user_acc = pm_add_info->pm_specific_info.pm4x10q.pm4x10_info.access.user_acc;
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &pm4x10q_create_info, &pm4x10q_id));

exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_PM4x10Q_SUPPORT*/


#ifdef PORTMOD_PM_OS_ILKN_SUPPORT
STATIC
int _portmod_os_ilkn_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t os_ilkn_create_info;
    int i, j, phy, tmp_pm_id, rv = 0;
    pm_info_t tmp_pm_info, ilkn_pms[PORTMOD_MAX_ILKN_AGGREGATED_PMS] = {NULL};
    int pm_ids[PORTMOD_MAX_ILKN_AGGREGATED_PMS] = {0};
    portmod_dispatch_type_t pm_type;
    int pm_id= INVALID_PM_ID;
    SOC_INIT_FUNC_DEFS;

    if(pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms > PORTMOD_MAX_ILKN_AGGREGATED_PMS)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Number of PMs(%d) should be less the %d"), pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms, PORTMOD_MAX_ILKN_AGGREGATED_PMS));
    }
    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &os_ilkn_create_info));
    os_ilkn_create_info.type = pm_add_info->type;
    PORTMOD_PBMP_ASSIGN(os_ilkn_create_info.phys, pm_add_info->phys);

    for(i = 0 ; i < PORTMOD_MAX_ILKN_PORTS_PER_ILKN_PM; i++) {
        os_ilkn_create_info.pm_specific_info.os_ilkn.wm_high[i] = pm_add_info->pm_specific_info.os_ilkn.wm_high[i];
        os_ilkn_create_info.pm_specific_info.os_ilkn.wm_low[i] = pm_add_info->pm_specific_info.os_ilkn.wm_low[i];
    }
    os_ilkn_create_info.pm_specific_info.os_ilkn.nof_aggregated_pms = pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms;
    os_ilkn_create_info.pm_specific_info.os_ilkn.pms = ilkn_pms;
    os_ilkn_create_info.pm_specific_info.os_ilkn.pm_ids = pm_ids;
    os_ilkn_create_info.pm_specific_info.os_ilkn.core_clock_khz = pm_add_info->pm_specific_info.os_ilkn.core_clock_khz;
    os_ilkn_create_info.pm_specific_info.os_ilkn.fec_disable_by_bypass = pm_add_info->pm_specific_info.os_ilkn.fec_disable_by_bypass;
    os_ilkn_create_info.pm_specific_info.os_ilkn.ilkn_port_lane_map_get = pm_add_info->pm_specific_info.os_ilkn.ilkn_port_lane_map_get;
    os_ilkn_create_info.pm_specific_info.os_ilkn.ilkn_pm_lane_map_get = pm_add_info->pm_specific_info.os_ilkn.ilkn_pm_lane_map_get;
    os_ilkn_create_info.pm_specific_info.os_ilkn.ilkn_port_fec_units_set = pm_add_info->pm_specific_info.os_ilkn.ilkn_port_fec_units_set;
    os_ilkn_create_info.pm_specific_info.os_ilkn.ilkn_block_index = pm_add_info->pm_specific_info.os_ilkn.ilkn_block_index;
    for(i = 0 ; i < pm_add_info->pm_specific_info.os_ilkn.nof_aggregated_pms; i++)
    {
         for( j = 0 ; j < MAX_PMS_PER_PHY ; j++){
             phy = pm_add_info->pm_specific_info.os_ilkn.controlled_pms[i].phy;
             pm_type = pm_add_info->pm_specific_info.os_ilkn.controlled_pms[i].type;
             rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &tmp_pm_id, phy, j);
             _SOC_IF_ERR_EXIT(rv);
             if(tmp_pm_id == INVALID_PM_ID){
                 _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("PM of type %d not found in PHY %d"), pm_type, phy));
             }
             
             rv = portmod_pm_info_from_pm_id_get(unit, tmp_pm_id, &tmp_pm_info);
             _SOC_IF_ERR_EXIT(rv);
             if(tmp_pm_info->type == pm_type){
                 ilkn_pms[i] = tmp_pm_info;
                 pm_ids[i] = tmp_pm_id;
                 break;
             }

         }
    }
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &os_ilkn_create_info, &pm_id));
exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_PM_OS_ILKN_SUPPORT*/

#ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
STATIC
int _portmod_dnx_fabric_add(int unit, const portmod_pm_create_info_t *pm_add_info)
{
    portmod_pm_create_info_internal_t dnx_fabric_create_info;
    int j, phy, tmp_pm_id, rv = 0;
    pm_info_t tmp_pm_info, fabric_o_nif_pm[1] = {NULL};
    portmod_dispatch_type_t pm_type;
    int pm_id= 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &dnx_fabric_create_info));
    dnx_fabric_create_info.type = pm_add_info->type;
    SOC_PBMP_ASSIGN(dnx_fabric_create_info.phys, pm_add_info->phys);

    dnx_fabric_create_info.pm_specific_info.dnx_fabric.pms = fabric_o_nif_pm;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.ref_clk = pm_add_info->pm_specific_info.dnx_fabric.ref_clk;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.com_clk = pm_add_info->pm_specific_info.dnx_fabric.com_clk;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.access = pm_add_info->pm_specific_info.dnx_fabric.access;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.lane_map = pm_add_info->pm_specific_info.dnx_fabric.lane_map;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.fw_load_method = pm_add_info->pm_specific_info.dnx_fabric.fw_load_method;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.external_fw_loader = pm_add_info->pm_specific_info.dnx_fabric.external_fw_loader;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.polarity = pm_add_info->pm_specific_info.dnx_fabric.polarity;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.fmac_schan_id = pm_add_info->pm_specific_info.dnx_fabric.fmac_schan_id;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.fsrd_schan_id = pm_add_info->pm_specific_info.dnx_fabric.fsrd_schan_id;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.fsrd_internal_quad = pm_add_info->pm_specific_info.dnx_fabric.fsrd_internal_quad;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.first_phy_offset = pm_add_info->pm_specific_info.dnx_fabric.first_phy_offset;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.core_index = pm_add_info->pm_specific_info.dnx_fabric.core_index;
    dnx_fabric_create_info.pm_specific_info.dnx_fabric.is_over_nif =  pm_add_info->pm_specific_info.dnx_fabric.is_over_nif;

    for( j = 0 ; j < MAX_PMS_PER_PHY ; j++){
        phy = pm_add_info->pm_specific_info.dnx_fabric.fabric_o_nif_pm.phy;
        pm_type = pm_add_info->pm_specific_info.dnx_fabric.fabric_o_nif_pm.type;
        rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &tmp_pm_id, phy, j); 
        _SOC_IF_ERR_EXIT(rv);
        if(tmp_pm_id == INVALID_PM_ID){
            _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("PM of type %d not found in PHY %d"), pm_type, phy));
        }
        
        rv = portmod_pm_info_from_pm_id_get(unit, tmp_pm_id, &tmp_pm_info);
        _SOC_IF_ERR_EXIT(rv);
        if(tmp_pm_info->type == pm_type){
            fabric_o_nif_pm[0] = tmp_pm_info;   
            break;
        }

    }

    dnx_fabric_create_info.pm_specific_info.dnx_fabric.pms[0] = fabric_o_nif_pm[0];

    
    _SOC_IF_ERR_EXIT(_portmod_port_macro_internal_add(unit, &dnx_fabric_create_info, &pm_id));
exit:
    if (SOC_FUNC_ERROR){

    }
    SOC_FUNC_RETURN;   
}
#endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT*/


int portmod_port_macro_add(int unit, const portmod_pm_create_info_t* pm_add_info)
{
    SOC_INIT_FUNC_DEFS;
    
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    if(pm_add_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("pm_add_info NULL parameter")));
    }
    switch(pm_add_info->type){
#ifdef PORTMOD_PMNULL_SUPPORT
    case portmodDispatchTypePmNull:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PMNULL_SUPPORT  */
#ifdef PORTMOD_PM12X10_SUPPORT
    case portmodDispatchTypePm12x10:
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
    case portmodDispatchTypePm12x10_xgs:
#endif /*PORTMOD_PM12X10_XGS_SUPPORT  */
        _SOC_IF_ERR_EXIT(_portmod_pm12x10_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM12X10_SUPPORT  */
#ifdef PORTMOD_PM4X25_SUPPORT
    case portmodDispatchTypePm4x25:
#ifdef PORTMOD_PM4X25TD_SUPPORT
    case portmodDispatchTypePm4x25td:
#endif /* PORTMOD_PM4X25TD_SUPPORT */
#ifdef PORTMOD_CPM4X25_SUPPORT
    case portmodDispatchTypeCpm4x25:
#endif /* PORTMOD_CPM4X25TD_SUPPORT */
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM4X10_SUPPORT
    case portmodDispatchTypePm4x10:
#ifdef PORTMOD_PM4X10TD_SUPPORT
    case portmodDispatchTypePm4x10td:
#endif /*PORTMOD_PM4X10TD_SUPPORT  */
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_DNX_FABRIC_SUPPORT
    case portmodDispatchTypeDnx_fabric:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_DNX_FABRIC_SUPPORT*/
#ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
    case portmodDispatchTypeDnx_fabric_o_nif:
        _SOC_IF_ERR_EXIT(_portmod_dnx_fabric_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT  */
#ifdef PORTMOD_PM8X50_FABRIC_SUPPORT
    case portmodDispatchTypePm8x50_fabric:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM8X50_FABRIC_SUPPORT*/
#ifdef PORTMOD_PM4x10Q_SUPPORT
    case portmodDispatchTypePm4x10Q:
        _SOC_IF_ERR_EXIT(_portmod_pm4x10q_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4x10Q_SUPPORT  */
#ifdef PORTMOD_PM_QTC_SUPPORT
    case portmodDispatchTypePm_qtc:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM_QTC_SUPPORT  */
#ifdef PORTMOD_PM_OS_ILKN_SUPPORT
    case portmodDispatchTypePmOsILKN:
#ifdef PORTMOD_PM_OS_ILKN_50G_SUPPORT
    case portmodDispatchTypePmOsILKN_50G:
#endif /*PORTMOD_PM_OS_ILKN_50G_SUPPORT  */ 
        _SOC_IF_ERR_EXIT(_portmod_os_ilkn_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM_OS_ILKN_SUPPORT  */        
#ifdef PORTMOD_PM4X2P5_SUPPORT
    case portmodDispatchTypePm4x2p5:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4X2P5_SUPPORT  */
#ifdef PORTMOD_PM8X50_SUPPORT
    case portmodDispatchTypePm8x50:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM8X50_SUPPORT*/
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    case portmodDispatchTypePm4x10_qtc:
        _SOC_IF_ERR_EXIT(_portmod_simple_pm_add(unit, pm_add_info));
        break;
#endif /*PORTMOD_PM4X10_QTC_SUPPORT  */
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid PM type %d"), pm_add_info->type));
    }
exit:
    SOC_FUNC_RETURN;   
}

STATIC
int portmod_validate_all_phys_in_pm(int unit, int pm_id, portmod_pbmp_t *phys, int *all_phys_in_pm){
    int i, phy, other_pm_id, belongs_to_pm, rv;
    SOC_INIT_FUNC_DEFS;
 
    *all_phys_in_pm = TRUE;
    PORTMOD_PBMP_ITER(*phys, phy){
        belongs_to_pm = FALSE;
        for( i = 0 ; i < MAX_PMS_PER_PHY ; i++){
            rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &other_pm_id, phy, i);
            _SOC_IF_ERR_EXIT(rv);
            if(pm_id == other_pm_id){
                belongs_to_pm = TRUE;
                break;
            }else if(other_pm_id == INVALID_PM_ID){
                /*end of list*/
                break;
            }
        }
        if(!belongs_to_pm){
              *all_phys_in_pm = FALSE;
              break;
        }
    }
exit:
    SOC_FUNC_RETURN;
}


int portmod_port_add(int unit, int port, const portmod_port_add_info_t *port_add_info)
{
    group_entry_id_t port_num;
    pm_info_t pm_info = NULL;
    int pm_id, rv, phy, phys_count = 0, sub_phy = 0, i, is_interface_supported, all_phys_in_pm = FALSE;
    int is_alias = 0, init_all = 0;
#ifdef PORTMOD_PM4X10_SUPPORT
    int is_qsgmii = 0;
#endif
#ifdef PORTMOD_PM8X50_SUPPORT
    int is_init_pass1 = FALSE;
#endif
    soc_port_if_t interface;
    int master_port;
    int add_port = FALSE;
    portmod_pbmp_t pm_phys;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    SOC_NULL_CHECK(port_add_info);

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(port_add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(port_add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(port_add_info)) ? 1 : 0;

#ifdef PORTMOD_PM8X50_SUPPORT
    PORTMOD_PBMP_ITER(port_add_info->phys, phy){
        portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;

        /* coverity[check_return] */
        portmod_phy_pm_type_get(unit, phy, &pm_type);
        /* For PM8x50 port add is done as part of init pass1 */
        if (pm_type == portmodDispatchTypePm8x50) {
            is_init_pass1 = PORTMOD_PORT_ADD_F_INIT_PASS1_GET(port_add_info);
            if (is_init_pass1) {
                add_port = TRUE;
            }
        }
    }
#endif /* PORTMOD_PM8X50_SUPPORT */

    /* do this only once (first time) per port */
    if((PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(port_add_info)) || (init_all) ||
        (add_port)) {
        /*check that port is not already in use*/
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
        _SOC_IF_ERR_EXIT(rv);
        if(pm_id != INVALID_PM_ID){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Port %d already in use"), port));
        }
        
        /*check for overlaps*/
        if (port_add_info->interface_config.interface != SOC_PORT_IF_NULL) {
            if(port_add_info->interface_config.interface == SOC_PORT_IF_QSGMII){
                PORTMOD_PBMP_COUNT(port_add_info->phys, phys_count);
                if(phys_count != 1){
                    _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("QSGMII must has one lane")));
                }
#ifdef PORTMOD_PM4X10_SUPPORT
                is_qsgmii = 1;
#endif
                sub_phy = port_add_info->sub_phy;
            } else {
                sub_phy = 0; /* sub phy is zero for non QSGMII */
                PORTMOD_PBMP_ITER(port_add_info->phys, phy){
                    rv = group_member_list_group_get(&_pmm_info[unit]->ports_phys_mapping, phy*SUB_PHYS_NUM + sub_phy, &port_num);
                    _SOC_IF_ERR_EXIT(rv);
                    if(port_num != GROUP_MEM_LIST_END){
                        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, port_num);
                        _SOC_IF_ERR_EXIT(rv);
                        if(port_add_info->interface_config.interface == interface) {
                            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &port_num, port);
                            _SOC_IF_ERR_EXIT(rv);
                            is_alias = 1;
                            break;
                        } else {
                            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d overlap with port %d"), port, port_num));
                        }
                    }
                }
            }
        }

        if (!is_alias) {
            /*find the PM to add the port to*/
            PORTMOD_PBMP_ITER(port_add_info->phys, phy){
                break;
            }

            if (port_add_info->interface_config.interface == SOC_PORT_IF_NULL) {
                pm_id = 0; /* pmNull is always added first, so it's pm_id will be always 0 */
            } else {
                is_interface_supported = FALSE;
                for( i = 0 ; i < MAX_PMS_PER_PHY ; i++){
                    rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &pm_id, phy, i);
                    _SOC_IF_ERR_EXIT(rv);
                    if(pm_id == INVALID_PM_ID){
                        break;
                    } else {
                        rv = portmod_pm_info_from_pm_id_get(unit, pm_id, &pm_info);
                        _SOC_IF_ERR_EXIT(rv);
#ifdef PORTMOD_PM4X10_SUPPORT
                        if (is_qsgmii) {
                            if (pm_info->type == portmodDispatchTypePm4x10) continue;
#ifdef PORTMOD_PM4X10TD_SUPPORT
                            if (pm_info->type == portmodDispatchTypePm4x10td) continue;
#endif
                        }
#endif
                        rv = portmod_pm_interface_type_is_supported(unit, pm_info, port_add_info->interface_config.interface, &is_interface_supported); 
#if defined(PORTMOD_DNX_FABRIC_SUPPORT) && defined(PORTMOD_PM4X25_SUPPORT)
                        if (pm_info->type == portmodDispatchTypePm4x25 && port_add_info->is_fabric_o_nif)
                        {
                            is_interface_supported = FALSE;
                        }
#endif
                        _SOC_IF_ERR_EXIT(rv);
                        if(is_interface_supported){
                            sal_memcpy(&pm_phys, &(port_add_info->phys), sizeof(pm_phys));
                            rv = portmod_validate_all_phys_in_pm(unit, pm_id, &pm_phys, &all_phys_in_pm);
                            _SOC_IF_ERR_EXIT(rv);
                            if(all_phys_in_pm){
                                break;
                            }
                        }
                    }
                }
                if((!is_interface_supported) || (!all_phys_in_pm)){
                    _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("No PM found for the specified phy %d and interface_type %d"), phy, port_add_info->interface_config.interface));
                }
            
                /*adding the new port to the PMM*/
                PORTMOD_PBMP_ITER(port_add_info->phys, phy){
                    rv = group_member_list_member_add(&_pmm_info[unit]->ports_phys_mapping, port, phy*SUB_PHYS_NUM + sub_phy);
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &(port_add_info->interface_config.interface), port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &port, port);
            _SOC_IF_ERR_EXIT(rv);
        } else {

            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port, port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, master_port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, master_port);
            _SOC_IF_ERR_EXIT(rv);


            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &interface, port);
            _SOC_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
            _SOC_IF_ERR_EXIT(rv);
        }
    } else {
        /* all other calls to this function besides the first - check if this port is an alias */
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port, port);
        _SOC_IF_ERR_EXIT(rv);
        is_alias = (master_port == port) ? 0 : 1;
    }

    if (!is_alias) {

        if (port_add_info->interface_config.interface == SOC_PORT_IF_ILKN)
        {
            _SOC_IF_ERR_EXIT(portmod_pm_aggregated_update_internal(unit, pm_info, port_add_info));
        }

        /*add port in PM level */
        rv = portmod_port_attach(unit, port, port_add_info);
        if(SOC_FAILURE(rv)){
            int invalid_pm = INVALID_PM_ID, rv_warn;
            soc_port_if_t invalid_interface = SOC_PORT_IF_NULL;
            soc_port_if_t invalid_port = -1;
            /*remove from PMM*/
            group_member_list_group_remove(&_pmm_info[unit]->ports_phys_mapping, port);
            rv_warn = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &invalid_pm, port);
            if(SOC_FAILURE(rv_warn)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail remove port %d from PMM_WB_PORT_PM_ID_MAP\n"), port));
            }
            rv_warn = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &invalid_interface, port);
            if(SOC_FAILURE(rv_warn)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail remove port %d from PMM_WB_PORT_INTERFACE_TYPE_MAP\n"), port));
            }
            rv_warn = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &invalid_port, port);
            if(SOC_FAILURE(rv_warn)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "fail remove port %d from PMM_WB_PORT_ALIAS_MAP\n"), port));
            }
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int portmod_port_remove(int unit, int port){
    int rv;
    int invalid_pm = INVALID_PM_ID;
    int is_master = 0, is_channelized = 0, master_port = 0, new_master_port = 0, port_i = 0, master_port_i = 0, i;
    soc_port_if_t invalid_interface = SOC_PORT_IF_NULL;
    soc_port_if_t invalid_port = -1, port_interface;
    uint32 phys[MAX_PHYS_PER_PORT];
    uint32 phys_count;

    SOC_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, 
                                &port_interface, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail port %d interface get"), port));
    }

    /* check if master port */
    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port, port));
    if (port == master_port) {
        is_master = 1;
    }

    if (is_master) {

        /* check if channelized (if other ports exist on the same interface) and find new master port*/
        for (port_i = 0; port_i < _pmm_info[unit]->max_ports; port_i++){
            if (port == port_i) {
                continue;
            }
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port_i, port_i));
            if (master_port_i == port) {
                is_channelized = 1;
                new_master_port = port_i;
                break;
            }
        }
        if (!is_channelized) {
            _SOC_IF_ERR_EXIT(portmod_port_detach(unit, port));

            if (port_interface != SOC_PORT_IF_NULL) {
                /*remove from PMM*/
                rv = group_member_list_group_remove(&_pmm_info[unit]->ports_phys_mapping, port);
                if(rv != SOC_E_NONE){
                    LOG_ERROR(BSL_LS_SOC_PORT,
                              (BSL_META_U(unit,
                                      "fail remove port %d from port to phys map\n"), port));
                }
            }
        } else { /* channelized interface */

            /* replace port mapping */
            _SOC_IF_ERR_EXIT(portmod_port_replace(unit, port, new_master_port));

            /* update ALIAS map*/
            for (port_i = 0; port_i < _pmm_info[unit]->max_ports; port_i++){
                _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &master_port_i, port_i));
                if (master_port_i == port) {
                    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &new_master_port, port_i));
                }
            }


            /* start - update phys mapping */
            rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
            _SOC_IF_ERR_EXIT(rv);
            if(phys_count < 1) {
                _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("No Phys attached to port")));
            }

            /* remove old master from PMM */
            if (port_interface != SOC_PORT_IF_NULL) {
                rv = group_member_list_group_remove(&_pmm_info[unit]->ports_phys_mapping, port);
                if(rv != SOC_E_NONE){
                    LOG_ERROR(BSL_LS_SOC_PORT,
                              (BSL_META_U(unit,
                                          "fail remove port %d from port to phys map\n"), port));
                }
            }

            /*adding the new master port to the PMM*/
            for(i = 0; i < phys_count; i++){
                 rv = group_member_list_member_add(&_pmm_info[unit]->ports_phys_mapping, new_master_port, phys[i]);
                _SOC_IF_ERR_EXIT(rv);
            }

            /* end - update phys mapping */
        }        
    }

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &invalid_port, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail remove port %d from port alias map"), port));
    }

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &invalid_pm, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail remove port %d from port to pm map\n"), port));
    }
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &invalid_interface, port);
    if(rv != SOC_E_NONE){
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "fail remove port %d from port to interface type map"), port));
    }

    
exit:
    SOC_FUNC_RETURN;
}


int portmod_next_wb_var_id_get(int unit, int *var_id){
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    *var_id = _pmm_info[unit]->wb_vars_in_use;
    _pmm_info[unit]->wb_vars_in_use++;

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_port_is_valid
 *
 * @brief Get the specific port has valid pm.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out] valid           - TRUE if port has valid pm_id
 */
int portmod_port_is_valid(int unit, int port, int* valid) {
    int rv, pm_id;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm_id, port);
    _SOC_IF_ERR_EXIT(rv);

    *valid = ((pm_id >= _pmm_info[unit]->pms_in_use) || (pm_id == INVALID_PM_ID)) ?
                   FALSE : TRUE;
exit:
    SOC_FUNC_RETURN;
}

/*PM info retreive*/
int portmod_port_pm_id_get(int unit, int port, int *pm_id){
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, pm_id, port);
    _SOC_IF_ERR_EXIT(rv);
    if((*pm_id >= _pmm_info[unit]->pms_in_use) || (*pm_id == INVALID_PM_ID)){
        _SOC_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_SOC_MSG("Valid PM Not found.")));
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_pm_id_pm_type_get(int unit, int pm_id, portmod_dispatch_type_t *type){
    pm_info_t pm_info;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, pm_id, &pm_info));
    *type = pm_info->type;
exit:
    SOC_FUNC_RETURN;  
}


int portmod_pm_info_get(int unit, int port, pm_info_t* pm_info){
    int pm_id;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    _SOC_IF_ERR_EXIT(portmod_port_pm_id_get(unit, port, &pm_id));
    *pm_info = &_pmm_info[unit]->pms[pm_id];
    if(*pm_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("pm_info null not as expected")));
    }
exit:
    SOC_FUNC_RETURN;  
}

int portmod_pm_info_type_get(int unit, int port, portmod_dispatch_type_t type, pm_info_t* pm_info)
{
    int pm_id, i, rv, actual_phy;
    uint32 phys[MAX_PHYS_PER_PORT];
    uint32 phys_count;
    SOC_INIT_FUNC_DEFS;

    *pm_info = NULL;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
    _SOC_IF_ERR_EXIT(rv);

    if(phys_count < 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("No Phys attached to port")));
    }

    actual_phy = phys[0]/SUB_PHYS_NUM;

    for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
        _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, actual_phy, i, (uint8 *)&pm_id));
        if(pm_id == INVALID_PM_ID){
            break; 
        }

        if(type == _pmm_info[unit]->pms[pm_id].type) {
            *pm_info = &_pmm_info[unit]->pms[pm_id];
            break;
        }
    }

    if(*pm_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("pm_info null not found")));
    }

exit:
    SOC_FUNC_RETURN; 
}

int portmod_pm_info_from_pm_id_get(int unit, int pm_id, pm_info_t* pm_info){
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    if((pm_id >= _pmm_info[unit]->pms_in_use)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid pm")));
    }
    *pm_info = &_pmm_info[unit]->pms[pm_id];
    if(*pm_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("pm_info null not as expected")));
    }
exit:
    SOC_FUNC_RETURN;  
}




int portmod_port_pm_type_get(int unit, int port, int *real_port, portmod_dispatch_type_t* type){
    pm_info_t pm_info;
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, real_port, port);
    if (SOC_FAILURE(rv)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, ("port %d doesn't exist", port));
    }

    /* if real_port is invalid, it should not call portmod_pm_info_get with invalid port.*/
    if(*real_port == INVALID_PORT) {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, ("port %d is not valid.", port));        
    }

    _SOC_IF_ERR_EXIT(portmod_pm_info_get(unit, *real_port, &pm_info));
    *type = pm_info->type;
exit:
    SOC_FUNC_RETURN;     
}



int portmod_pms_num_get(int unit, int *pms_num){
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pms_num);
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    *pms_num = _pmm_info[unit]->pms_in_use;
exit:
    SOC_FUNC_RETURN;
}

int portmod_port_interface_type_get(int unit, int port, soc_port_if_t *interface){
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, interface, port);
    _SOC_IF_ERR_EXIT(rv);
   
exit:
    SOC_FUNC_RETURN;   
}

int portmod_pm_diag_info_get(int unit, int pm_id, portmod_pm_diag_info_t *diag_info){
    int i, phy, port, pm;
#ifdef PORTMOD_PM4X25_SUPPORT
    int in_pm12x10;
#endif /* PORTMOD_PM4X25_SUPPORT */
    int skip = 0;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    SOC_NULL_CHECK(diag_info);

    if(pm_id >= _pmm_info[unit]->pms_in_use){
       diag_info->type = portmodDispatchTypeCount;
       SOC_EXIT;
    }

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
#ifdef PORTMOD_PMNULL_SUPPORT
    if (_pmm_info[unit]->pms[pm_id].type == portmodDispatchTypePmNull) {
    } else
#endif /* PORTMOD_PMNULL_SUPPORT */
    {
        if(_pmm_info[unit]->pms[pm_id].pm_data.pm4x10_qtc_db == NULL){
           diag_info->type = portmodDispatchTypeCount;
           SOC_EXIT;
        }
    }
#endif /* PORTMOD_PM4X10_QTC_SUPPORT */

    PORTMOD_PBMP_CLEAR(diag_info->phys);
    diag_info->type = _pmm_info[unit]->pms[pm_id].type;
    for( phy = 0 ; phy < _pmm_info[unit]->max_phys; phy++){
         for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
            _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm));
            if(pm == INVALID_PM_ID){
                break; 
            }else if (pm == pm_id){
                PORTMOD_PBMP_PORT_ADD(diag_info->phys, phy);
                break;
            }
        }

    }
    SOC_PBMP_CLEAR(diag_info->ports);
    for( port = 0 ; port < _pmm_info[unit]->max_ports; port++){
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &pm, port));
        /*bsl_printf("portmod_pm_diag_info_get: unit=%d,port=%d,pm=%d,pmid=%d\r\n", unit, port, pm, pm_id);*/
        if(pm == INVALID_PM_ID){
            continue; 
        }else if (pm == pm_id){
            SOC_PBMP_PORT_ADD(diag_info->ports, port);
            continue;
        }
    }

    /* Core info for pm4x25 when in pm12x10 not present, skip the call */
#ifdef PORTMOD_PM4X25_SUPPORT
    if (
#ifdef PORTMOD_PM4X25TD_SUPPORT
        (diag_info->type == portmodDispatchTypePm4x25td) ||
#endif /* PORTMOD_PM4X25TD_SUPPORT */
        (diag_info->type == portmodDispatchTypePm4x25)) {
        _SOC_IF_ERR_EXIT(portmod_pm_is_in_pm12x10(unit,
                                           &(_pmm_info[unit]->pms[pm_id]),
                                           &in_pm12x10));
        if (in_pm12x10) {
            skip = 1;
        }
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    if (!skip) {
        _SOC_IF_ERR_EXIT(portmod_pm_core_info_get(unit,
                                                  &_pmm_info[unit]->pms[pm_id],
                                                  -1, &diag_info->core_info));
    }
    
exit:
    SOC_FUNC_RETURN;
}

/* Combo port control/status values. */
typedef enum _portmod_port_medium_e {
    PORTMOD_PORT_MEDIUM_NONE,   /* _SHR_PORT_MEDIUM_NONE   */
    PORTMOD_PORT_MEDIUM_COPPER, /* _SHR_PORT_MEDIUM_COPPER */
    PORTMOD_PORT_MEDIUM_FIBER,  /* _SHR_PORT_MEDIUM_FIBER  */
    PORTMOD_PORT_MEDIUM_COUNT   /* _SHR_PORT_MEDIUM_COUNT  */
} _portmod_port_medium_t;

int portmod_port_diag_info_get(int unit, int port, portmod_port_diag_info_t *diag_info){
    member_entry_id_t phys[MAX_PHYS_PER_PORT];
    uint32 phys_count = 0;
    int rv;
    int i, nof_phys;
#ifdef PORTMOD_CPM4X25_SUPPORT
    int real_port;
    int flags;
    portmod_encap_t encap;
    portmod_dispatch_type_t type;
#endif
    portmod_access_get_params_t access_param;
    phymod_phy_access_t phy_access[6];
#ifdef PORTMOD_PM8X50_SUPPORT
    phymod_firmware_lane_config_t fw_lane_config;
    portmod_speed_config_t speed_config;
#endif
#if defined(PORTMOD_PM8X50_SUPPORT) || defined(PORTMOD_PM4X10_SUPPORT)
    uint32_t fec_enable = 0;
#endif
#if defined(PORTMOD_CPM4X25_SUPPORT) || defined(PORTMOD_PM8X50_SUPPORT) || defined(PORTMOD_PM4X10_SUPPORT)
    pm_info_t pm_info;
#endif
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    SOC_NULL_CHECK(diag_info);
    rv = portmod_port_diag_info_t_init(unit, diag_info);
    _SOC_IF_ERR_EXIT(rv);
    if(port >= _pmm_info[unit]->max_ports){
        diag_info->pm_id = INVALID_PM_ID;
        SOC_EXIT;
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_ALIAS_MAP, &diag_info->original_port, port);
    _SOC_IF_ERR_EXIT(rv);
    if(diag_info->original_port == INVALID_PORT){
        diag_info->pm_id = INVALID_PM_ID;
        SOC_EXIT;
    }
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_INTERFACE_TYPE_MAP, &diag_info->interface, port);
    _SOC_IF_ERR_EXIT(rv);
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PORT_PM_ID_MAP, &diag_info->pm_id, port);
    _SOC_IF_ERR_EXIT(rv);
    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
    _SOC_IF_ERR_EXIT(rv);
    PORTMOD_PBMP_CLEAR(diag_info->phys);
    if(phys_count != 0){
        diag_info->sub_phy = phys[phys_count-1] % SUB_PHYS_NUM;
        for(i = 0; i < phys_count ;i++){
            PORTMOD_PBMP_PORT_ADD(diag_info->phys, phys[i] / SUB_PHYS_NUM);
        }
    }

    if (diag_info->interface == SOC_PORT_IF_NULL)
        return (SOC_E_NONE);

    /* get phy polority */
    rv = portmod_access_get_params_t_init(unit, &access_param);
    _SOC_IF_ERR_EXIT(rv);
    if (IS_C_PORT(unit, port)) {
        
        access_param.phyn = 0;
        rv = portmod_port_phy_lane_access_get(unit, port, &access_param,
                                              3, &phy_access[0], &nof_phys, NULL);
    } else {
        
        access_param.phyn = 0;
        rv = portmod_port_phy_lane_access_get(unit, port, &access_param, 
                                              PORTMOD_MAX_ILKN_AGGREGATED_PMS, &phy_access[0], &nof_phys, NULL);
    }
    _SOC_IF_ERR_EXIT(rv);
    if (!SAL_BOOT_SIMULATION)
#ifdef PORTMOD_CPM4X25_SUPPORT
    {
        rv = portmod_port_pm_type_get(unit, port, &real_port, &type);
        if(type == portmodDispatchTypeCpm4x25) {
            _SOC_IF_ERR_EXIT(portmod_pm_info_get(unit, real_port, &pm_info));
            _SOC_IF_ERR_EXIT(portmod_port_encap_get(unit, port, &flags, &encap));
            if ((encap == _SHR_PORT_ENCAP_CPRI)||
                (encap == _SHR_PORT_ENCAP_RSVD4)) {
                rv = 0;
                diag_info->polarity.rx_polarity = 0;
                diag_info->polarity.tx_polarity = 0;
            } else {
                rv = phymod_phy_polarity_get(&phy_access[0], &diag_info->polarity);
                _SOC_IF_ERR_EXIT(rv);
            }
        } else
#else
    {
#endif /*PORTMOD_CPM4X25_SUPPORT  */
    {
            rv = phymod_phy_polarity_get(&phy_access[0], &diag_info->polarity);
        _SOC_IF_ERR_EXIT(rv);
    }
    }

    _SOC_IF_ERROR_NOT_UNAVAIL_RETURN(portmod_port_mode_get(unit, port, &diag_info->core_mode));

#ifdef PORTMOD_PM8X50_SUPPORT
    _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, diag_info->pm_id, &pm_info));
    if (pm_info->type == portmodDispatchTypePm8x50) {
        rv = phymod_phy_firmware_lane_config_get(&phy_access[0], &fw_lane_config);
        _SOC_IF_ERR_EXIT(rv);
        diag_info->medium = fw_lane_config.MediaType;
        rv = portmod_port_speed_config_get(unit, port, &speed_config);
        _SOC_IF_ERR_EXIT(rv);
        diag_info->fec = speed_config.fec;
    } else
#endif
    {
    /* CHECK CHECK WITH DORON */
    diag_info->medium = PORTMOD_PORT_MEDIUM_NONE;
    }
#ifdef PORTMOD_PM4X10_SUPPORT
    _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, diag_info->pm_id, &pm_info));
    if (pm_info->type == portmodDispatchTypePm4x10) {
        rv = portmod_port_fec_enable_get(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &fec_enable);
        diag_info->fec = fec_enable? PORTMOD_PORT_PHY_FEC_BASE_R : PORTMOD_PORT_PHY_FEC_NONE;
    }
#endif
exit:
    SOC_FUNC_RETURN;   
}

int portmod_port_first_phy_get(int unit, int port, int *first_phy, int *sub_phy){
    member_entry_id_t phys[MAX_PHYS_PER_PORT];
    uint32 phys_count = 0;
    int i;
    int min_phy;
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    
    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys, &phys_count);
    _SOC_IF_ERR_EXIT(rv);

    min_phy = _pmm_info[unit]->max_phys * SUB_PHYS_NUM;
    /*git the minimal one*/
    for(i = 0; i < phys_count ;i++){
        if(phys[i] < min_phy){
            min_phy = phys[i];
        }
    }
    *first_phy = min_phy / SUB_PHYS_NUM;
    *sub_phy = min_phy % SUB_PHYS_NUM;
exit:
    SOC_FUNC_RETURN; 
}

/*get all the PMS ids of specific phy*/
int portmod_phy_pms_ids_get(int unit, int phy, int max_pms, int *nof_pms, portmod_dispatch_type_t *pms_type, int *pms_ids){
    pm_info_t pms_info[MAX_PMS_PER_PHY];
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, phy, max_pms, pms_info, nof_pms, pms_ids));
    for( i = 0 ; i < *nof_pms ; i++){
            pms_type[i] = pms_info[i]->type ;
    }

exit:
    SOC_FUNC_RETURN;
}

/*get all the PMS of specific phy*/
int portmod_phy_pms_info_get(int unit, int phy, int max_pms, pm_info_t *pms_info, int *nof_pms, int *pms_ids){
    int pm_id = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    *nof_pms = 0;
    for( i = 0 ; i < MAX_PMS_PER_PHY ; i++){
        _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm_id));
        if(pm_id == INVALID_PM_ID){
            break;
        }
        if (max_pms == *nof_pms){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Array supplied has less entries than needed")));
        }
        _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, pm_id, &pms_info[*nof_pms]));
        pms_ids[*nof_pms] = pm_id;
        *nof_pms += 1;
    }
exit:
    SOC_FUNC_RETURN; 
}

int portmod_ext_phy_lane_attach(int unit, int iphy, int phyn, const portmod_lane_connection_t* lane_connection)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    int pms_ids[MAX_PMS_PER_PHY] = {0};
    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms,pms_ids));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }

    /* Attach the external phy to the phy chain of each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_ext_phy_lane_attach_to_pm(unit, pms_info[i], iphy, phyn, lane_connection));
    }
    SOC_FUNC_RETURN;
exit:
    /* Rollingback the attach operations because of an error */
    --i;
    for( ; i >= 0 ; --i) {
        portmod_ext_phy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, NULL);
    }
    SOC_FUNC_RETURN;
}

int portmod_ext_phy_lane_detach(int unit, int iphy, int phyn)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    int pms_ids[MAX_PMS_PER_PHY] = {0};
    portmod_lane_connection_t lane_connection[MAX_PMS_PER_PHY];

    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms,pms_ids));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }
    /* Detach the last external phy in the phy chain from each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_ext_phy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, &lane_connection[i]));
    }
    SOC_FUNC_RETURN;
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_lane_attach(int unit, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_conn)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    int pms_ids[MAX_PMS_PER_PHY] = {0};
    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms,pms_ids));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }

    /* Attach the external phy to the phy chain of each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_xphy_lane_attach_to_pm(unit, pms_info[i], iphy, phyn, lane_conn));
    }
    SOC_FUNC_RETURN;
exit:
    /* Rollingback the attach operations because of an error */
    --i;
    for( ; i >= 0 ; --i) {
        portmod_xphy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, NULL);
    }
    SOC_FUNC_RETURN;
}

int portmod_xphy_lane_detach(int unit, int iphy, int phyn)
{
    int i = 0, nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY] = {NULL,};
    int pms_ids[MAX_PMS_PER_PHY] = {0};
    portmod_xphy_lane_connection_t lane_conn[MAX_PMS_PER_PHY];

    SOC_INIT_FUNC_DEFS;

    /* Validate input parameters */
    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, iphy, MAX_PMS_PER_PHY, pms_info, &nof_pms,pms_ids));
    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("The input phy is not attached to any port macros")));
    }
    /* Detach the last external phy in the phy chain from each port macro which uses the input phy */
    for( i = 0 ; i < nof_pms ; i++){
        _SOC_IF_ERR_EXIT(portmod_xphy_lane_detach_from_pm(unit, pms_info[i], iphy, phyn, &lane_conn[i]));
    }
    SOC_FUNC_RETURN;
exit:
    SOC_FUNC_RETURN;
}

int portmod_max_pms_get(int unit, int* max_pms)
{
    SOC_INIT_FUNC_DEFS;
    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }
    *max_pms = _pmm_info[unit]->max_pms;
exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_pm_type_get(int unit, int phy, portmod_dispatch_type_t *type) 
{ 
    int nof_pms = 0;
    pm_info_t pms_info[MAX_PMS_PER_PHY];
    int pms_ids[MAX_PMS_PER_PHY] = {0};

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_phy_pms_info_get(unit, phy, MAX_PMS_PER_PHY, pms_info, &nof_pms,pms_ids));

    if (0 == nof_pms){
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("The input phy is not attached to any port macros")));
    }

    *type = pms_info[nof_pms-1]->type;

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      portmod_pm_port_pll_div_get
 * Purpose:
 *      This function will be called during speed is changed or flexport.
 *      This function returns pll_div value by looking up the static
 *      table in pm4x10 and pm4x25. But this funciton might be called
 *      before port is attached (in flexport scenario). In that case,
 *      port number may be invalid(-1), then the pll_div only can be
 *      gotten from static table, and this function can't access registers
 *      or Warmboot DB. In speed change scenario, there is no this limitation.
 *
 *      portmod_port_pll_div_get is added just for the purpose to add
 *      function pointer [pm_type]_port_pll_div_get to each portmod_dispatch_t
 *      driver, such as portmod_pm4x10_driver, portmod_pm4x25_driver.
 *
 * Parameters:
 *      unit     - (IN)  Unit number
 *      port     - (IN)  logical port
 *      phy_port - (IN)  physical port
 *      pll_div  - (OUT) pll div
 *
 * Returns:
 * Note:
 */
int portmod_pm_port_pll_div_get(int unit, int port, int phy_port,
                                const portmod_port_resources_t* port_resource,
                                uint32_t* pll_div)
{
    portmod_dispatch_type_t __type__ = portmodDispatchTypeCount;
    int __rv__;
    int i, pm_id, rv;
    pm_info_t pm_info;

    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < MAX_PMS_PER_PHY; i++) {
        rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &pm_id, phy_port, i);
        _SOC_IF_ERR_EXIT(rv);
        if (pm_id != INVALID_PM_ID) {
            rv = portmod_pm_info_from_pm_id_get(unit, pm_id, &pm_info);
            _SOC_IF_ERR_EXIT(rv);
            break;
        }
    }
    if (pm_id == INVALID_PM_ID) {
        __rv__ = SOC_E_PARAM;
        _SOC_IF_ERR_EXIT(__rv__);
    }

    __type__ = (pm_info)->type;
    if(NULL != __portmod__dispatch__[__type__]->f_portmod_port_pll_div_get) {
        __rv__ = __portmod__dispatch__[__type__]->f_portmod_port_pll_div_get(unit, port, pm_info, port_resource, pll_div);
        _SOC_IF_ERR_EXIT(__rv__);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, ("portmod_port_pll_div_get isn't implemented for driver type"));
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_phy_pm_id_get
 *
 * @brief Geven the physical port ID, return the associated PM ID
 *
 * @param [in]  unit            - unit id
 * @param [in]  pport           - physical port
 * @param [out] pm_id           - PM ID associated with the physical port
 */
int portmod_phy_pm_id_get(int unit, int pport, int *pm_id)
{
    int i;
    SOC_INIT_FUNC_DEFS;

        /* get the PM id from PHY->PM map */
    for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
        _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD,
                                               PMM_WB_PHY_PM_MAP, pport, i,
                                               (uint8 *)pm_id));
        if ((*pm_id) != INVALID_PM_ID)
            break;
        }
exit:
    SOC_FUNC_RETURN;
}

int portmod_pm_supported_vcos_get(int unit, int port, portmod_dispatch_type_t pm_type,
                                  const portmod_vcos_speed_config_t* speed_config_list, 
                                  int size, portmod_dual_vcos_t* dual_vco)
{
    portmod_dispatch_type_t __type__ = portmodDispatchTypeCount;
    pm_info_t pm_info;
    int __rv__;

    SOC_INIT_FUNC_DEFS;
    __type__ = pm_type;
    if(__type__ >= portmodDispatchTypeCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Driver is out of range"));
    }
    pm_info = NULL;

    if(NULL != __portmod__dispatch__[__type__]->f_portmod_port_vcos_get) {
        __rv__ = __portmod__dispatch__[__type__]->f_portmod_port_vcos_get(unit, port, pm_info,  speed_config_list, size, dual_vco);
        _SOC_IF_ERR_EXIT(__rv__);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, ("portmod_port_vcos_get isn't implemented for driver type"));
    }

exit:
    SOC_FUNC_RETURN;
}

static int portmod_periodic_event(int unit, void* user_data)
{
    int port_i, rv, rv_failure, is_quit_signaled;
    portmod_periodic_event_f port_callback;
    SOC_INIT_FUNC_DEFS;

    /* Lock in case ports_callback DB is update while executing port operation */
    rv_failure = SOC_E_NONE;

    for (port_i=0 ; port_i<_pmm_info[unit]->max_ports ; port_i++) {

        _pmm_info[unit]->periodic.current_working_port = port_i;

        _SOC_IF_ERR_EXIT(periodic_event_is_quit_signaled_get(_pmm_info[unit]->periodic.periodic_handler, &is_quit_signaled));

        if (is_quit_signaled) {
            break;
        }

        port_callback = _pmm_info[unit]->periodic.ports_periodic[port_i].ports_callback;

        if(port_callback != NULL) {
                rv = port_callback(unit, port_i);

                /* iterate over all port - but rememebr if there was a failrue */
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "portmod_periodic_event failed for port %d"), port_i));
                    rv_failure = rv;
                }
        }

        
    }

    _SOC_IF_ERR_EXIT(rv_failure);

exit:
    _pmm_info[unit]->periodic.current_working_port = -1;
    SOC_FUNC_RETURN;
}

/*!
 * portmod_port_callback_update_internal 
 *  
 *  @brief add or remove per port periodic callback
 * 
 * @param unit - unit #
 * @param port - port to register or remove callback
 * @param callback - periodi callback, use NULL for remove
 * @param interval - expected period (actual period will be 
 *                 equal or smaller from given value). Use 0 for
 *                 remove.
 * 
 * @return int 
 */
#define _PORTMOD_MIN_INTERVAL_NOT_FOUND -2

static int portmod_port_callback_update_internal(int unit, int port, portmod_periodic_event_f callback, int interval)
{
    int port_i;
    int min_interval=_PORTMOD_MIN_INTERVAL_NOT_FOUND; /*-2 indicates first value*/
    SOC_INIT_FUNC_DEFS;

    /* Update DB*/
    _pmm_info[unit]->periodic.ports_periodic[port].ports_callback = callback;
    _pmm_info[unit]->periodic.ports_periodic[port].interval = interval;
    

    /* calc min interval and num of callbacks */
    for (port_i=0 ; port_i<_pmm_info[unit]->max_ports ; port_i++) {

        if (_pmm_info[unit]->periodic.ports_periodic[port_i].ports_callback != NULL) {

            /* get minimum interval */
            if (_pmm_info[unit]->periodic.ports_periodic[port_i].interval != sal_sem_FOREVER) { /*ignore forever as it's -1 and will always be the minimum...*/
                if (min_interval == _PORTMOD_MIN_INTERVAL_NOT_FOUND) {
                    min_interval = _pmm_info[unit]->periodic.ports_periodic[port_i].interval;
                }

                if (_pmm_info[unit]->periodic.ports_periodic[port_i].interval < min_interval) {
                    min_interval = _pmm_info[unit]->periodic.ports_periodic[port_i].interval;
                }
            }
        }
    }

    /* if didn't found ant minimum but there is FOREVER interval - use the FOREVER */
    if (min_interval == _PORTMOD_MIN_INTERVAL_NOT_FOUND) {
        min_interval = sal_sem_FOREVER;
    }

    /* Update the interval anyway - if no callback the period should be set to forever*/
    _SOC_IF_ERR_EXIT(periodic_event_interval_set(_pmm_info[unit]->periodic.periodic_handler, min_interval));
       
exit:
    SOC_FUNC_RETURN;
    
}

/* see .h file */ 
int portmod_port_periodic_callback_unregister(int unit, int port)
{
    soc_timeout_t to;
    int is_callback_conetxt;
    SOC_INIT_FUNC_DEFS;

    sal_mutex_take(_pmm_info[unit]->periodic.periodic_lock, sal_mutex_FOREVER);

    _SOC_IF_ERR_EXIT(portmod_port_callback_update_internal(unit, port, NULL, 0));

    /* make sure current port isn't in work currently
       - for the case callback for current port is already running before unregister was called*/
    _SOC_IF_ERR_EXIT(periodic_event_is_periodic_context_get(_pmm_info[unit]->periodic.periodic_handler, &is_callback_conetxt));
    if (!is_callback_conetxt) {

        soc_timeout_init(&to, SECOND_USEC /*1 sec*/, 100);

        while (_pmm_info[unit]->periodic.current_working_port == port) {
            if (soc_timeout_check(&to)) {
                _SOC_EXIT_WITH_ERR(SOC_E_TIMEOUT,(_SOC_MSG("Failed to unregister port %d"), port));
                break;
            }
        }
    }

exit:
    sal_mutex_give(_pmm_info[unit]->periodic.periodic_lock);
    SOC_FUNC_RETURN;
    
}

/* see .h file */ 
int portmod_port_periodic_callback_register(int unit, int port, portmod_periodic_event_f callback, int interval)
{
    int is_active;
    int portmon_disable;
    SOC_INIT_FUNC_DEFS;

    sal_mutex_take(_pmm_info[unit]->periodic.periodic_lock, sal_mutex_FOREVER);

    _SOC_IF_ERR_EXIT
        (portmod_portmon_disable_get(unit, &portmon_disable));
    if (portmon_disable) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "periodic callback register is not allowed with portmod thread disable %d"), port));
        _SOC_IF_ERR_EXIT(SOC_E_CONFIG);
    }

    /* first unregister previous callback to allow safe exit before moving to bew callback */
    _SOC_IF_ERR_EXIT(portmod_port_periodic_callback_unregister(unit, port));

    /* Update new callback */
    _SOC_IF_ERR_EXIT(portmod_port_callback_update_internal(unit, port, callback, interval));

    /* If thread never started - start it now */
    _SOC_IF_ERR_EXIT(periodic_event_is_active_get(_pmm_info[unit]->periodic.periodic_handler, &is_active));

    if (!is_active) {
        _SOC_IF_ERR_EXIT(periodic_event_start(_pmm_info[unit]->periodic.periodic_handler));
    }

exit:
    sal_mutex_give(_pmm_info[unit]->periodic.periodic_lock);
    SOC_FUNC_RETURN;
}

/* see .h file */ 
int portmod_port_periodic_interval_get(int unit, int* interval)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(periodic_event_interval_get(_pmm_info[unit]->periodic.periodic_handler, interval));

exit:
    SOC_FUNC_RETURN;
}

int portmod_portmon_disable_get(int unit, int *disable)
{
    *disable = _pmm_info[unit]->periodic.portmon_disable;
    return SOC_E_NONE;
}


#ifdef CPRIMOD_SUPPORT
int portmod_cpri_port_interrupt_callback_register(int unit, 
                                                  portmod_cpri_port_intr_type_t cpri_intr_type, 
                                                  portmod_cpri_port_interrupt_callback_t callback, 
                                                  void* cb_data)
{
    SOC_INIT_FUNC_DEFS;
   
    _SOC_IF_ERR_EXIT(cprimod_cpri_port_interrupt_callback_register(unit, 
                                                    cpri_intr_type, callback, cb_data));


exit:
    SOC_FUNC_RETURN;
   
}
#endif /* CPRIMOD_SUPPORT */

#ifdef CPRIMOD_SUPPORT
int portmod_cpri_port_interrupt_callback_unregister(int unit,
        portmod_cpri_port_intr_type_t cpri_intr_type,
        cprimod_cpri_port_interrupt_callback_t callback)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_interrupt_callback_unregister(unit, 
                                                         cpri_intr_type, callback));
exit:
    SOC_FUNC_RETURN;

}
#endif /* CPRIMOD_SUPPORT */


/*
 * This functions takes the actual hardware PM Id core_num as input and
 * returns the sw PM id mapped in the PortMod library. The function does
 * not return error and the function caller should initialize pm_id to -1.
 */
int portmod_core_pm_id_get(int unit, int core_num, int *pm_id)
{
    int i = 0;
    pm_info_t pm_info;
    int pm_core_num = 0;

    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT,
                (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    for (i = 0; i < _pmm_info[unit]->pms_in_use; i++) {

        _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, i, &pm_info));

        if (NULL == __portmod__dispatch__[pm_info->type]->
                        f_portmod_pm_core_num_get) {
            continue;
        }

        _SOC_IF_ERR_EXIT(portmod_pm_core_num_get(unit, pm_info, &pm_core_num));

        if (core_num == pm_core_num) {
            *pm_id = i;
            break;
        }
    }

    if (i == _pmm_info[unit]->pms_in_use) {
        _SOC_EXIT_WITH_ERR(SOC_E_INIT,
                (_SOC_MSG("SW PM Id not for HW PM Id[%d] found!!"), core_num));
    }

exit:
    SOC_FUNC_RETURN;
}

/* see .h file */
int
portmod_ilkn_block_phys_set(int unit, int ilkn_block_id, portmod_pbmp_t phys, int enable)
{
    int i = 0;
    int phy, pm_id, current_val;
    uint8 free_slot_found;
    uint32 invalid_pm_id = INVALID_PM_ID;
    SOC_INIT_FUNC_DEFS;


    /*The PM-ID is taken from the WB-SW state */
    _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_ILKN_PM_MAP, &pm_id, ilkn_block_id));

    if(!SOC_WARM_BOOT(unit)){
        /*add to map just in case of cold boot*/
        if (enable)
        {
            PORTMOD_PBMP_ITER(phys, phy){
                free_slot_found = FALSE;
                for(i = 0 ; i < MAX_PMS_PER_PHY; i++){
                    _SOC_IF_ERR_EXIT(soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&current_val));
                    if(current_val == INVALID_PM_ID ){
                        _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&pm_id));
                        free_slot_found = TRUE;
                        break;
                    }
                }
                if(!free_slot_found){
                    _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("phy (%d) already used by the maximum number of pms %d"), phy, MAX_PMS_PER_PHY));
                }
            }
        }
        else
        {
            /*clean the map*/
            PORTMOD_PBMP_ITER(phys, phy){
                /* coverity[dead_error_line] */
                for(i = 0 ; !_rv && (i < MAX_PMS_PER_PHY); i++){
                    _rv = soc_wb_engine_var_get(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&current_val);
                    if(!_rv && (current_val == pm_id)){
                        _SOC_IF_ERR_EXIT(soc_wb_engine_var_set(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, phy, i, (uint8 *)&invalid_pm_id));
                        break;
                    }
                }
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_pm_aggregated_update_internal
 *
 *  @brief update the aggregated PM info for ilkn port
 *  Holds an array of portmacros assigned to this port (used by ilkn)
 *  Each element describes the type of the PM, and the first phy.
 *
 * @param unit - unit #
 * @param pm_info - port macro info needed to be update
 * @param port_add_info - pointer to port add info,  contains information about port macros (type and phys)
 * @return int
 */
static int portmod_pm_aggregated_update_internal(int unit, pm_info_t pm_info, const portmod_port_add_info_t *port_add_info)
{
    pm_info_t tmp_pm_info = NULL;
    pm_info_t ilkn_pms[PORTMOD_MAX_ILKN_AGGREGATED_PMS] = {NULL};
    int pm_ids[PORTMOD_MAX_ILKN_AGGREGATED_PMS] = {0};
    int  tmp_pm_id, phy,  i, j;
    portmod_dispatch_type_t pm_type;

    SOC_INIT_FUNC_DEFS;

    if (port_add_info == NULL)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("port_add_info is NULL")));
    }

    for(i = 0 ; i < port_add_info->nof_aggregated_pms; i++)
    {
        for( j = 0 ; j < MAX_PMS_PER_PHY ; j++){
            phy = port_add_info->controlled_pms[i].phy;
            pm_type = port_add_info->controlled_pms[i].type;
            _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, PMM_WB_PHY_PM_MAP, &tmp_pm_id, phy, j));
            if(tmp_pm_id == INVALID_PM_ID)
            {
                _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("PM of type %d not found in PHY %d"), pm_type, phy));
            }
            _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, tmp_pm_id, &tmp_pm_info));
            if(tmp_pm_info->type == pm_type){
                ilkn_pms[i] = tmp_pm_info;
                pm_ids[i] = tmp_pm_id;
                break;
            }

        }
    }
    _SOC_IF_ERR_EXIT(portmod_pm_aggregated_update(unit, pm_info, port_add_info->ilkn_core_id, port_add_info->nof_aggregated_pms, ilkn_pms, pm_ids));

exit:
    SOC_FUNC_RETURN;

}

int portmod_port_phys_get(int unit, int port,portmod_pbmp_t *phys){

    member_entry_id_t phys_internal[MAX_PHYS_PER_PORT];
    uint32 phys_count = 0;
    int i;
    int phy;
    int rv;
    SOC_INIT_FUNC_DEFS;

    if(_pmm_info[unit] == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_INIT, (_SOC_MSG("Portmod was not initialized for the unit")));
    }

    rv = group_member_list_group_members_get(&_pmm_info[unit]->ports_phys_mapping, port, MAX_PHYS_PER_PORT, phys_internal, &phys_count);
    _SOC_IF_ERR_EXIT(rv);

    PORTMOD_PBMP_CLEAR(*phys);

    for(i = 0; i < phys_count ;i++){
        phy = phys_internal[i] / SUB_PHYS_NUM;
        PORTMOD_PBMP_PORT_ADD(*phys, phy);
    }
exit:
    SOC_FUNC_RETURN;

}


#undef _ERR_MSG_MODULE_NAME
