/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trunk.c
 * Purpose: BCM level APIs for Link Aggregation (a.k.a Trunking)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRUNK

#include <shared/bsl.h>

#include <soc/types.h>
#include <sal/appl/io.h>
#include <sal/core/alloc.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>
#include <soc/dpp/PPD/ppd_api_slb.h>
#include <soc/dpp/trunk_sw_db.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/trunk.h>
#include <bcm/vlan.h>
#include <bcm/stack.h>
#include <bcm/pkt.h>
#include <bcm/cosq.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/trunk.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/trunk.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/switch.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#include <soc/dpp/ARAD/arad_ports.h>


int _bcm_petra_gport_to_sys_port(int unit, bcm_gport_t gport, uint32 *sys_port);
int soc_dpp_arad_trunk_groups_ports_num_get(int unit, int *ngroups, int *nports);

/*
 * Check if port is in LAG (this function return correct value only if port is local).
 */
int
_bcm_petra_trunk_member_in_other_trunk_check(int unit,  bcm_gport_t gport, bcm_trunk_t tid);

/*
 * One trunk control entry for each SOC device containing trunk book keeping
 * info for that device.
 */

#define TRUNK_MIN_MEMBERS   0   /* Minimum number of ports in a trunk */

#define _BCM_TRUNK_RESILIENT_MATCH_FLAGS   (BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID | BCM_TRUNK_RESILIENT_MATCH_HASH_KEY | BCM_TRUNK_RESILIENT_MATCH_MEMBER)
#define _BCM_TRUNK_RESILIENT_ACTION_FLAGS  (BCM_TRUNK_RESILIENT_REPLACE | BCM_TRUNK_RESILIENT_COUNT | BCM_TRUNK_RESILIENT_DELETE)

#define SYSTEM_PORT_AGGREGATE_MASK 0xffff

#define TRUNK_DB_LOCK(unit)                                                                         \
        do {                                                                                        \
            if (NULL != sw_state_sync_db[(unit)].dpp.trunk_lock)                             \
                sal_mutex_take(sw_state_sync_db[(unit)].dpp.trunk_lock, sal_mutex_FOREVER);  \
        } while (0);

#define TRUNK_DB_UNLOCK(unit)                                                       \
        do {                                                                        \
            if (NULL != sw_state_sync_db[(unit)].dpp.trunk_lock)             \
                sal_mutex_give(sw_state_sync_db[(unit)].dpp.trunk_lock);    \
        } while (0);

/*
 * Cause a routine to return BCM_E_INIT if trunking subsystem is not
 * initialized to an acceptable initialization level (il_).
 */
#define TRUNK_CHECK_INIT(u_, il_)                                               \
    do {                                                                        \
        trunk_init_state_t _init_state;                                         \
        uint8 _is_allocated;                                                    \
        BCM_DPP_UNIT_CHECK(u_);                                                 \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.is_allocated(unit, &_is_allocated));    \
        if(!_is_allocated) {                                                    \
            BCM_RETURN_VAL_EXIT(BCM_E_INIT);                                    \
        }                                                                       \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.init_state.get(unit, &_init_state));    \
        if (_init_state < (il_)) {                                              \
            BCM_RETURN_VAL_EXIT(BCM_E_INIT);                                    \
        }                                                                       \
    } while (0);

/*
 * Make sure TID is within valid range.
 */
#define TRUNK_CHECK_TID(unit, tid) \
    {                                                                   \
        int _ngroups;                                                   \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &_ngroups));  \
        if (((tid) < 0) || ((tid) >= _ngroups)) {                       \
            return BCM_E_BADID;                                         \
        }                                                               \
    }

#define TRUNK_CHECK_STK_TID(unit, tid) \
    {                                                                           \
        int _ngroups;                                                           \
        int _stk_ngroups;                                                       \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &_ngroups));          \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &_stk_ngroups));  \
        if (((tid) < 0) || ((tid) >= _ngroups + _stk_ngroups)) {                \
            return BCM_E_BADID;                                                 \
        }                                                                       \
    }

/*
 * TID is in range, check to make sure it is actually in use.
 */
#define TRUNK_TID_VALID(trunk_id)                          \
    (trunk_id != BCM_TRUNK_INVALID)
    

#define TRUNK_PORTCNT_VALID(port_cnt, nports)                  \
    (((/* not really needed. just to aviod coverity defect */int)port_cnt >= TRUNK_MIN_MEMBERS) && (port_cnt <= nports))

/* 
 *  Trunk Staking TID=TM-domain
 */
#define TRUNK_TID_STACKING_TID_SET(unit, tid)\
    {                                                                           \
        int _ngroups;                                                           \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &_ngroups));          \
        ((tid) +=  _ngroups + 1);                                               \
    }
#define TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid)\
    {                                                                           \
        int _ngroups;                                                           \
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &_ngroups));          \
        (stk_tid = tid - _ngroups - 1);                                         \
    }
#define BCM_TRUNK_STACKING_TID_VALID(stk_tid)    ((stk_tid < 0) || (stk_tid >= BCM_DPP_MAX_STK_TRUNKS))
#define BCM_TRUNK_IS_STACKING_TID(tid)  (tid & (1 << BCM_TRUNK_STACKING_TID_BIT) ? 1 : 0)

static int        _stk_ngroups      = BCM_DPP_MAX_STK_TRUNKS;



int _bcm_petra_trunk_psc_to_lb_type(int                 unit, 
                                    int                 psc, 
                                    SOC_PPC_LAG_LB_TYPE *lb_type)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (lb_type == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_petra_trunk_psc_to_lb_type errm lb_type == NULL")));
    }

    switch (psc) {
    case BCM_TRUNK_PSC_ROUND_ROBIN:
        *lb_type = SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN;
        break;
    case BCM_TRUNK_PSC_PORTFLOW:
        *lb_type = SOC_PPC_LAG_LB_TYPE_HASH;
        break;
    case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        if (SOC_IS_JERICHO(unit)) {
            *lb_type = SOC_PPC_LAG_LB_TYPE_SMOOTH_DIVISION;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
    case BCM_TRUNK_PSC_DYNAMIC_RESILIENT:
        if (SOC_IS_ARADPLUS(unit)) {
            *lb_type = SOC_PPC_LAG_LB_TYPE_HASH;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_trunk_lb_type_to_psc(int                 unit, 
                                    SOC_PPC_LAG_LB_TYPE lb_type, 
                                    int                 *psc)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (psc == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_petra_trunk_lb_type_to_psc err,  psc == NULL")));
    }

    switch (lb_type) {
    case SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN:
        *psc = BCM_TRUNK_PSC_ROUND_ROBIN;
        break;
    case SOC_PPC_LAG_LB_TYPE_HASH:
        *psc = BCM_TRUNK_PSC_PORTFLOW;
        break;
    case SOC_PPC_LAG_LB_TYPE_SMOOTH_DIVISION:
        if (SOC_IS_JERICHO(unit)) {
            *psc = BCM_TRUNK_PSC_SMOOTH_DIVISION;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_trunk_stacking_tid_to_local_tid(int          unit, 
                                               bcm_trunk_t  *tid)
{
    int rv = BCM_E_NONE;
    bcm_trunk_t stk_tid;

    BCMDNX_INIT_FUNC_DEFS;

    if (tid == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_petra_trunk_stacking_tid_to_local_tid err, tid == NULL")));
    }

    if (BCM_TRUNK_IS_STACKING_TID(*tid)) {

        stk_tid = BCM_TRUNK_STACKING_TID_GET(*tid);
        if (BCM_TRUNK_STACKING_TID_VALID(stk_tid)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Stacking tid not in range.")));
        }
        TRUNK_TID_STACKING_TID_SET(unit, stk_tid);
        *tid = stk_tid;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int _bcm_petra_stacking_trunk_set(int                unit,
                                         bcm_trunk_t        tid,
                                         bcm_trunk_info_t   *trunk_info,
                                         int                member_count,
                                         bcm_trunk_member_t *member_array)
{
    int                         rv = BCM_E_NONE;
    int                         soc_sand_rc = SOC_SAND_OK;
    int                         entry;
    int                         index;
    int                         system_port;
    int                         core = SOC_CORE_ALL;
    bcm_module_t                modid, my_modid; 
    uint32                      tm_port;
    uint32                      dest_base_queue; 
    uint8                       is_dest_valid = FALSE;
    uint8                       is_sw_only = FALSE;
    bcm_gport_t                 gport;
    bcm_gport_t                 gport_modport = 0x0;   
    bcm_trunk_t                 stk_tid = 0x0;
    
    BCMDNX_INIT_FUNC_DEFS;

    TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
 
    LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(),%d: adding Stacking Trunk  tid=%d(%d(."), FUNCTION_NAME(), unit, tid, stk_tid));

    if(member_count > SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: member_count=0x%x - maximum ports allowed in stacking trunk is 64"), member_count));
    }

    index = 0x0;
    for (entry = 0; entry < SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX; entry++) {

        rv = bcm_petra_stk_gport_sysport_get(unit, member_array[index].gport, &gport);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_system_port from gport=0x%x"), member_array[index].gport));
        }   

        system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        rv = bcm_petra_stk_sysport_gport_get(unit, gport, &gport_modport);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_modport from gport=0x%x"), gport));
        }

        tm_port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
        modid   = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
        BCMDNX_IF_ERR_EXIT( bcm_petra_stk_my_modid_get(unit, &my_modid));
        if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, my_modid)) {
            /* local stacking ports */
            core = SOC_DPP_MODID_TO_CORE(unit, my_modid, modid);
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_stacking_info_set,(unit, core, tm_port, 0x1, stk_tid)));
            if (SOC_SAND_FAILURE(soc_sand_rc)) {
                BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting Stacking info. tm_port=%d, core=%d, stk_tid=%d,"), tm_port, core, stk_tid));
            }
        } 

        /* get stk port voq */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_destination_id_packets_base_queue_id_get,(unit, SOC_CORE_ALL, system_port, &is_dest_valid, &is_sw_only, &dest_base_queue)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("retrieving base queue for dest port(%d) failed, soc_sand error (0x%x),"), system_port, soc_sand_rc));
        }
        if (is_dest_valid == 0x0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Failed to get queue for local_stk_sysport=%d"), system_port));
        }

        /* Write entries to Stack trunk resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set,(unit, stk_tid, entry, dest_base_queue)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting stack_lag_packets_base_queue_id. stk_tid=%d, entry=%d, dest_base_queue=%d"), stk_tid, entry, dest_base_queue));
        }

        index = (index + 1) % member_count;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    _bcm_petra_trunk_set
 * Purpose:
 *      Set ports in a trunk group.
 * Parameters:
 *      unit            - Device unit number.
 *      tid             - The trunk ID to be affected.
 *      trunk_info      - Information on the trunk group.
 *      member_count    - amount of member in member_array
 *      member_array    - array of members to set in trunk
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_PARAM     - Invalid ports specified.
 *      BCM_E_XXXXX     - As set by lower layers of software
 * Notes:
 *      the only the fields of the bcm_trunk_info_t structure which is relevant for DPP is:
 *          psc
 */
STATIC int _bcm_petra_trunk_set(int                 unit,
                                bcm_trunk_t         tid,
                                bcm_trunk_info_t    *trunk_info,
                                int                 member_count,
                                bcm_trunk_member_t  *member_array )
{
    int                     index;
    int                     rv = BCM_E_NONE;
    int                     ngroups;
    int                     stk_ngroups;
    int                     is_stacking_trunk;
    uint8                   is_stateful = 0;
    bcm_gport_t             gport;
    SOC_PPC_LAG_INFO        lag_info;
    SOC_PPC_LAG_LB_TYPE     lb_type = SOC_PPC_LAG_LB_TYPE_HASH;



    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *, [n=%d - "), FUNCTION_NAME(), unit, tid, member_count));
    for (index = 0; index < member_count; index++) 
    {
        LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, " %08X flags:%08X"), member_array[index].gport, member_array[index].flags));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "]) - Enter\n")));

    /* Validate PSC */
    if (trunk_info->psc <= 0) 
    {
        /* default to Round Robin */
        lb_type = SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN;

    } else 
    {
        rv = _bcm_petra_trunk_psc_to_lb_type(unit, trunk_info->psc, &lb_type);
        if (BCM_FAILURE(rv)) 
        {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: Invalid trunk psc (%d) specified"), FUNCTION_NAME(), trunk_info->psc));
        }

        if (SOC_IS_ARADPLUS(unit)) 
        {
            is_stateful = (trunk_info->psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) ? 1 : 0;
        }
    }

    /* check validation of the trunk members */

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
    is_stacking_trunk = (tid > ngroups) && (tid < ngroups + stk_ngroups);
    if (is_stacking_trunk) 
    {
        rv = _bcm_petra_stacking_trunk_set(unit, tid, trunk_info, member_count, member_array);
        if (BCM_FAILURE(rv)) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Stacking trunk Set Failed."), FUNCTION_NAME()));
        }
        TRUNK_CHECK_STK_TID(unit, tid);
    } 

    /* Make sure the ports/nodes supplied are valid. */
    SOC_PPC_LAG_INFO_clear(&lag_info);
    
    for (index = 0; (BCM_E_NONE == rv) && (index < member_count); index++) 
    {
        gport = member_array[index].gport;

        rv = _bcm_petra_gport_to_sys_port(unit, gport, &lag_info.members[index].sys_port);
        if (BCM_FAILURE(rv)) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("%s: Invalid trunk member (%d) specified"), FUNCTION_NAME(), index));
        }        
        lag_info.members[index].member_id = index;
        
        /* Set ingress disable flags */
        if((member_array[index].flags & BCM_TRUNK_MEMBER_INGRESS_DISABLE) != 0) 
        {
            lag_info.members[index].flags = SOC_PPC_LAG_MEMBER_INGRESS_DISABLE;
        } 
        
        /* Check that ports of the lag are not memners of other LAG */
        rv = _bcm_petra_trunk_member_in_other_trunk_check(unit, gport, tid);
        if (BCM_FAILURE(rv)) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: port ox%x is other lag member"), FUNCTION_NAME(), lag_info.members[index].sys_port));
        }

    }

    /* for (all members as long as no error) */

    lag_info.nof_entries = member_count;
    lag_info.lb_type = lb_type;

    if (SOC_IS_ARADPLUS(unit)) 
    {
        lag_info.is_stateful = is_stateful;
    }

    if (is_stacking_trunk) 
    {
        SOC_SAND_IF_ERR_RETURN(soc_ppd_lag_lb_key_range_set(unit, &lag_info));
    } else 
    {
        SOC_SAND_IF_ERR_RETURN(soc_ppd_lag_set(unit, tid, &lag_info));
    }
    
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, member_count, bcm_errmsg(rv)));
exit:
    BCMDNX_FUNC_RETURN;
}





int _bcm_petra_gport_to_sys_port(int            unit, 
                                 bcm_gport_t    gport, 
                                 uint32         *sys_port)
{
    bcm_gport_t     sys_gport=BCM_GPORT_INVALID;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(sys_port);

    /* Get System Gport from the gport */
    if (BCM_GPORT_IS_MODPORT(gport) || BCM_GPORT_IS_LOCAL(gport)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_gport_sysport_get(unit, gport, &sys_gport));
    } else if (BCM_GPORT_IS_SYSTEM_PORT(gport)) {
        sys_gport = gport;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Invalid Gport type specified, must be System port or Modport."), FUNCTION_NAME()));
    }

    /* Get the System Port ID from the System Gport and verify it is valid */
    *sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sys_gport);
    if(*sys_port >= (ARAD_NOF_SYS_PHYS_PORTS_GET(unit) - 1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Port not mapped to System Port ID!"), FUNCTION_NAME())); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_trunk_member_set(int                  unit, 
                               bcm_trunk_t          tid, 
                               int                  member_count, 
                               bcm_trunk_member_t   *member_array)
{
    int                 rv = BCM_E_NONE, index;
    uint32              sys_port;
    bcm_trunk_t         trunk_id;
    SOC_PPC_LAG_INFO    lag_info;
    int                 trunk_db_lock_was_taken ;


    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (member_array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_set err,  member_array == NULL")));
    }
    if ((member_count <= 0) || (member_count > BCM_TRUNK_MAX_PORTCNT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_set err,(member_count <= 0) || (member_count > BCM_TRUNK_MAX_PORTCNT) ")));
    }

    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
       BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
       TRUNK_CHECK_STK_TID(unit, tid);
    }
    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (TRUNK_TID_VALID(trunk_id)) {
        SOC_PPC_LAG_INFO_clear(&lag_info);
        for (index = 0; index < member_count; index++) {
            rv = _bcm_petra_gport_to_sys_port(unit, member_array[index].gport, &sys_port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Invalid gport(%x) in member_array at index:%d \n"), FUNCTION_NAME(), member_array[index].gport, index));
                break;
            } else {
                lag_info.members[index].sys_port = sys_port;
                lag_info.members[index].member_id = index;
            }
        }
        if (BCM_SUCCESS(rv)) {
            lag_info.nof_entries = member_count;
            lag_info.lb_type = SOC_PPC_LAG_LB_TYPE_HASH; /* default to hash */

            rv = soc_ppd_lag_set(unit, tid, &lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed(%s) to set members on trunk:%d \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
            }
            rv = soc_dpp_trunk_sw_db_set(unit, tid, &lag_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed(%s) to update software database for trunk:%d \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trunk_member_get(int                  unit, 
                               bcm_trunk_t          tid, 
                               int                  member_max,
                               bcm_trunk_member_t   *member_array, 
                               int                  *member_count)
{
    int                 index;
    int                 ngroups;
    int                 stk_ngroups;
    int                 rv = BCM_E_NONE;
    bcm_gport_t         gport;
    bcm_trunk_t         trunk_id;
    bcm_trunk_t         stk_tid;
    bcm_trunk_info_t    trunk_info;        
    SOC_PPC_LAG_INFO    *lag_info = NULL;
    int                 trunk_db_lock_was_taken ;


    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    TRUNK_CHECK_INIT(unit, ts_init);

    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    BCMDNX_NULL_CHECK(member_count);

    if ((member_max > 0) && (member_array == NULL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_get err, (member_max > 0) && (member_array == NULL)")));
    }

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
    if (tid > ngroups && tid < ngroups + stk_ngroups) {
        TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
        BCM_TRUNK_STACKING_TID_SET(tid, stk_tid);
           
        sal_memset(&trunk_info, 0x0, sizeof(bcm_trunk_info_t));
        rv = bcm_petra_trunk_get(unit, tid, &trunk_info, BCM_DPP_MAX_STK_TRUNKS, member_array, member_count);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Get trunk. tid=0x%x."), FUNCTION_NAME(), tid));
        }
    } else {

        TRUNK_DB_LOCK(unit);
        trunk_db_lock_was_taken = 1 ;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
        if (TRUNK_TID_VALID(trunk_id)) {
            BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "bcm_petra_trunk_member_get.lag_info");
            if(lag_info == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
            }
            SOC_PPC_LAG_INFO_clear(lag_info);
            rv = soc_ppd_lag_get(unit, tid, lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed (%s) to read trunk (id:%d) info \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
            } else {
                if (member_max > 0) {
                    for (index = 0; ((index < lag_info->nof_entries) && (index < member_max)); index++) 
                    {


                        BCM_GPORT_SYSTEM_PORT_ID_SET(gport,
                                                     lag_info->members[index].sys_port);
                         member_array[index].gport = gport;
                         member_array[index].flags = 0;

                    }
                    *member_count = index;
                } else {
                    *member_count = lag_info->nof_entries;
                }
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        TRUNK_DB_UNLOCK(unit);
        trunk_db_lock_was_taken = 0 ;
    }

    BCM_EXIT;
exit:
    BCM_FREE(lag_info);
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trunk_member_add(int                  unit, 
                               bcm_trunk_t          tid,
                               bcm_trunk_member_t   *member)
{
    int                         rv = BCM_E_NONE;
    int                         ngroups;
    int                         stk_ngroups;
    int                         nports;
    int                         member_count = 0;
    int                         i;
    int                         is_stacking_trunk = FALSE;
    int                         nof_replications = 0; 
    int                         last_replicated_member_index = -1;
    uint32                      sys_port;
    SOC_PPC_LAG_INFO            *lag_info = NULL;
    SOC_PPC_LAG_MEMBER          lag_member;
    SOC_SAND_SUCCESS_FAILURE    soc_sand_rv;
    bcm_trunk_t                 trunk_id;
    bcm_trunk_t                 stk_tid;
    bcm_trunk_info_t            trunk_info;
    bcm_trunk_member_t          member_array[BCM_DPP_MAX_STK_TRUNKS];
    int                         trunk_db_lock_was_taken ;
    uint32                      disable_member_exist = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) 
    {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
        is_stacking_trunk = TRUE;
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    if (member == NULL) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_add err,  member == NULL")));
    }

    rv = _bcm_petra_gport_to_sys_port(unit, member->gport, &sys_port); 
    if (BCM_FAILURE(rv)) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Invalid member port type (not modport | sysport) specified."), FUNCTION_NAME()));
    }
        
    /* check if the port is already member of a trunk */
    rv = _bcm_petra_trunk_member_in_other_trunk_check(unit, member->gport, tid);
    if (BCM_FAILURE(rv)) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: error port 0x%x lag member"), FUNCTION_NAME(), sys_port));
    }

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
    if (is_stacking_trunk) 
    {
            TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
            BCM_TRUNK_STACKING_TID_SET(tid, stk_tid);
               
            sal_memset(&trunk_info, 0x0, sizeof(bcm_trunk_info_t));
            sal_memset(member_array, 0x0, BCM_DPP_MAX_STK_TRUNKS * sizeof(bcm_trunk_member_t));
            rv = bcm_petra_trunk_get(unit, tid, &trunk_info, BCM_DPP_MAX_STK_TRUNKS, member_array, &member_count);
            if (BCM_FAILURE(rv)) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Get trunk. tid=0x%x."), FUNCTION_NAME(), tid));
            }

            if (member_count >= BCM_DPP_MAX_STK_TRUNKS) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_STR("%s: Stacking Trunk already contains max number of supported members (%d)\n"), FUNCTION_NAME(),BCM_DPP_MAX_STK_TRUNKS));
            }

            for (i=0 ; i < member_count; i++) 
            {
                rv = bcm_petra_stk_gport_sysport_get(unit, member_array[i].gport, &(member_array[i].gport));
                if (BCM_FAILURE(rv)) 
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed convert trunk member from gport modport to gport sysport. member_array[%d].gport=0x%x."), FUNCTION_NAME(), i, member_array[i].gport));
                }
                if (member_array[i].gport == member->gport) 
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: New member already exist in stacking trunk. member_array[%d].gport=0x%x."), FUNCTION_NAME(), i, member_array[i].gport));
                }
            }
            sal_memcpy(&member_array[member_count], member, sizeof(bcm_trunk_member_t));

            rv = bcm_petra_trunk_set(unit, tid, &trunk_info, member_count + 1, member_array);
            if (BCM_FAILURE(rv)) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Set trunk. tid=0x%x."), FUNCTION_NAME(), tid));
            }

    } else 
    {
        TRUNK_DB_LOCK(unit);
        trunk_db_lock_was_taken = 1 ;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));

        if (!TRUNK_TID_VALID(trunk_id)) 
        {
            rv = BCM_E_NOT_FOUND;
            BCMDNX_IF_ERR_EXIT(rv);
        }

        BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "bcm_petra_trunk_member_add.lag_info");
        if(lag_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }
        SOC_PPC_LAG_INFO_clear(lag_info);
        rv = soc_dpp_trunk_sw_db_get(unit, tid, lag_info);
        if (BCM_FAILURE(rv)) 
        {
            TRUNK_DB_UNLOCK(unit);
            trunk_db_lock_was_taken = 0 ;
            BCMDNX_ERR_EXIT_MSG(BSL_LS_BCM_TRUNK, (_BSL_BCM_MSG("%s: Failed (%s) to read trunk (id:%d) info \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
        } 
        
        SOC_PPC_LAG_MEMBER_clear(&lag_member);
        lag_member.sys_port = sys_port;
        lag_member.member_id = lag_info->nof_entries;

        if((member->flags & BCM_TRUNK_MEMBER_INGRESS_DISABLE) != 0) 
        {
            /* Set ingress disable flags */
            lag_member.flags = SOC_PPC_LAG_MEMBER_INGRESS_DISABLE;
        } else {
            /* check and resolve case of existing member with INGRESS_DISABLE or EGRESS_DISABLE flags */
            /* find nof replications and last replication index */
            BCMDNX_IF_ERR_EXIT(soc_dpp_trunk_sw_db_get_nof_replications(unit, tid, &lag_member, &nof_replications, &last_replicated_member_index));
            /* if has replications and has INGRESS_DISABLE or EGRESS_DISABLE flags */
            if ((nof_replications != 0) && (((lag_info->members[last_replicated_member_index].flags & BCM_TRUNK_MEMBER_INGRESS_DISABLE) != 0) ||
                                            ((lag_info->members[last_replicated_member_index].flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0)))
            {
                lag_member.member_id = last_replicated_member_index;
                disable_member_exist = TRUE;
            }
        }

        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &nports));
        if (disable_member_exist == FALSE && !TRUNK_PORTCNT_VALID(lag_info->nof_entries + 1, nports))
        {
            TRUNK_DB_UNLOCK(unit);
            trunk_db_lock_was_taken = 0 ;
            LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Trunk already contains max number of supported members (%d)\n"), FUNCTION_NAME(), nports));
            rv = BCM_E_RESOURCE;
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = soc_ppd_lag_member_add(unit, tid, &lag_member, &soc_sand_rv);
        rv = handle_sand_result(rv);
        if (BCM_FAILURE(rv)) 
        {
            LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Error(%s) adding member (gport:%x) to trunk (id:%d) \n"), FUNCTION_NAME(), bcm_errmsg(rv), member->gport, tid));
        } else 
        {
            LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Added member (gport:%x) to trunk (id:%d)\n"), FUNCTION_NAME(), member->gport, tid));
            rv = TRUNK_ACCESS.t_info.in_use.set(unit, tid, TRUE); /* set trunk in use for a case it dosn't set yet */
            if (rv != BCM_E_NONE) 
            {
                BCMDNX_IF_ERR_EXIT(rv);
            }

        }

        TRUNK_DB_UNLOCK(unit);
        trunk_db_lock_was_taken = 0 ;
    }

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCM_FREE(lag_info);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_stacking_trunk_member_delete(int                  unit, 
                                           bcm_trunk_t          tid, 
                                           bcm_trunk_member_t   *member)
{
    int                     rv = BCM_E_NONE;
    int                     soc_sand_rc = SOC_SAND_OK;
    int                     member_count = 0;
    int                     i; 
    int                     member_del_idx = 0xffffffff;
    int                     core = SOC_CORE_ALL;
    bcm_module_t            modid, my_modid;
    uint32                  tm_port;
    bcm_gport_t             gport_modport = 0x0;
    bcm_trunk_t             stk_tid;
    bcm_trunk_info_t        trunk_info;
    bcm_trunk_member_t      member_array[BCM_DPP_MAX_STK_TRUNKS];
    SOC_PPC_LAG_INFO        *lag_info = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
    BCM_TRUNK_STACKING_TID_SET(tid, stk_tid);
       
    sal_memset(&trunk_info, 0x0, sizeof(bcm_trunk_info_t));
    sal_memset(member_array, 0x0, BCM_DPP_MAX_STK_TRUNKS * sizeof(bcm_trunk_member_t));
    rv = bcm_petra_trunk_get(unit, tid, &trunk_info, BCM_DPP_MAX_STK_TRUNKS, member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Get trunk. tid=0x%x."), FUNCTION_NAME(), tid));
    }

    if (member_count == 0x0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_STR("%s: Stacking Trunk already empty (member_count=%d)\n"), FUNCTION_NAME(),member_count));
    }

    rv = bcm_petra_stk_sysport_gport_get(unit, member->gport, &gport_modport);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_modport from gport=0x%x"), member->gport));
    }

    for (i=0 ; i < member_count; i++) {
        if (member_array[i].gport == gport_modport) {
            member_del_idx = i;
        }
    }

    if (member_del_idx == 0xffffffff) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG_STR("%s: Input gport member (0x%x), are not in Stacking trunk (%d)\n"), FUNCTION_NAME(),member->gport, tid));
    }

    /* Update port stacking info (is_stacking, peer) */

    tm_port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
    modid   = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
    BCMDNX_IF_ERR_EXIT( bcm_petra_stk_my_modid_get(unit, &my_modid));
    if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, my_modid)) {
        /* local stacking ports */
        core = SOC_DPP_MODID_TO_CORE(unit, my_modid, modid);
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_stacking_info_set,(unit, core, tm_port, 0x0, 0x0)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting Stacking info. tm_port=%d, core=%d, stk_tid=%d,"), tm_port, core, stk_tid));
        }
    }

    /* Clear port lb-key range */
    BCMDNX_ALLOC(lag_info, sizeof(SOC_PPC_LAG_INFO), "bcm_petra_stacking_trunk_member_delete.lag_info");
    if (!lag_info) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    SOC_PPC_LAG_INFO_clear(lag_info);
    lag_info->members[0x0].sys_port = _SHR_GPORT_IS_MODPORT(member->gport) ? BCM_GPORT_MODPORT_PORT_GET(member->gport) : BCM_GPORT_SYSTEM_PORT_ID_GET(member->gport);
    lag_info->members[0x0].member_id = 0x0;
    lag_info->nof_entries = 0x1;
    lag_info->lb_type = SOC_PPC_LAG_LB_TYPE_HASH;
    rv = soc_ppd_lag_lb_key_range_set(unit, lag_info);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to Clear lag lb-key range.")));
    }

    if (member_count == 1) {

        /* Write entries to Stack trunk resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set,(unit, stk_tid, SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_ALL, 0x0)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting stack_lag_packets_base_queue_id. stk_tid=%d,"), stk_tid));
        }

    } else {
        if (member_del_idx != member_count) {
            sal_memcpy(&member_array[member_del_idx], &member_array[member_count - 1], sizeof(bcm_trunk_member_t));
        }

        rv = bcm_petra_trunk_set(unit, tid, &trunk_info, member_count -1, member_array);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed to Set trunk. tid=0x%x."), FUNCTION_NAME(), tid));
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trunk_member_delete(int                   unit, 
                                  bcm_trunk_t           tid, 
                                  bcm_trunk_member_t    *member)
{
    int                     rv = BCM_E_NONE;
    int                     ngroups;
    int                     stk_ngroups;
    int                     remove_idx = -1;
    unsigned int            soc_sand_unit = (unit);
    unsigned int            lag_member_idx;
    uint32                  sys_port;
    uint32                  sys_port_to_remove;
    uint32                  soc_sand_rv;
    bcm_trunk_t             trunk_id;
    SOC_PPC_LAG_MEMBER      lag_member;
    SOC_PPC_LAG_INFO        *lag_info = NULL;
    int                     trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);
     if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }
    if (member == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_trunk_member_delete err,  member == NULL")));
    }

    rv = _bcm_petra_gport_to_sys_port(unit, member->gport, &sys_port);
    if (BCM_FAILURE(rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Invalid member port type (not modport | sysport) specified."), FUNCTION_NAME()));
    }

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
    if (tid > ngroups && tid < ngroups + stk_ngroups) {

        rv = bcm_petra_stacking_trunk_member_delete(unit, tid, member);
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Stacking trunk delete Failed."), FUNCTION_NAME()));
        }

    } else {
        BCMDNX_ALLOC(lag_info, sizeof(SOC_PPC_LAG_INFO), "bcm_petra_trunk_member_delete.lag_info");
        if (!lag_info) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }
        /* Before removing the member, get some data for later. */
        SOC_PPC_LAG_INFO_clear(lag_info);

        TRUNK_DB_LOCK(unit);
        trunk_db_lock_was_taken = 1 ;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));

        if (TRUNK_TID_VALID(trunk_id)) 
        {
            if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) 
            {
                /* Get the LAG info. */
                BCMDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, trunk_id, lag_info));

                if (lag_info->is_stateful) 
                {
                    /* Get the sysport to remove. */
                    rv = _bcm_petra_gport_to_sys_port(unit, member->gport, &sys_port_to_remove);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Get the first index matching the sysport. */
                    for (lag_member_idx = 0; lag_member_idx < lag_info->nof_entries; lag_member_idx++) {
                        if (lag_info->members[lag_member_idx].sys_port == sys_port_to_remove) {
                            break;
                        }
                    }   

                    if (lag_member_idx >= lag_info->nof_entries) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LAG group %d does not contain sysport %d.\n"), trunk_id, sys_port_to_remove));
                    }

                    remove_idx = lag_member_idx; 
                }
            }

            /* Set ingress disable flags */
            SOC_PPC_LAG_MEMBER_clear(&lag_member);
            lag_member.sys_port = sys_port;	
            if ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0) {
                lag_member.flags = SOC_PPC_LAG_MEMBER_EGRESS_DISABLE;
            } 

            rv = soc_ppd_lag_member_remove(unit, tid, &lag_member);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Error(%s) deleting member(gport:%x) from trunk (id:%d) \n"), FUNCTION_NAME(), bcm_errmsg(rv), member->gport, tid));
            } else {
                LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Deleted member(gport:%x) from trunk (id:%d)\n"), FUNCTION_NAME(), member->gport, tid));
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        TRUNK_DB_UNLOCK(unit);
        trunk_db_lock_was_taken = 0 ;

/* For SLB delete all flows pointing to the deleted member and transplant all flows pointing to the last member to its new index. */
/* WARNING - If this fails there will be no rollback (the lag member is still deleted). */
/* This is why we do not expect anything to fail here. */
        if (lag_info->is_stateful && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) 
        {
            /* Note: The lag_info is still of the LAG group before the removal. */
            /* Remove all flows pointing to the removed idx. */
            {
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG match_lag;
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG match_lag_member;
                const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
                SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE remove_action;

                SOC_PPC_SLB_CLEAR(&match_lag, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
                SOC_PPC_SLB_CLEAR(&match_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);
                SOC_PPC_SLB_CLEAR(&remove_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE);

                match_lag.lag_ndx = trunk_id;
                match_lag_member.lag_member_ndx = remove_idx;

                match_rules[0] = SOC_PPC_SLB_DOWNCAST(&match_lag, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
                match_rules[1] = SOC_PPC_SLB_DOWNCAST(&match_lag_member, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);

                soc_sand_rv = soc_ppd_slb_traverse(soc_sand_unit, match_rules, 2, SOC_PPC_SLB_DOWNCAST(&remove_action, SOC_PPC_SLB_TRAVERSE_ACTION), NULL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            /* If the member being removed is not the last member, then transplant all flows pointing to the last idx to point to the removed member idx. */
            if (remove_idx != (lag_info->nof_entries - 1)) 
            {
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG match_lag;
                SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG match_lag_member;
                const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
                SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE update_action;
                SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER update_lag_member;

                SOC_PPC_SLB_CLEAR(&match_lag, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
                SOC_PPC_SLB_CLEAR(&match_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);
                SOC_PPC_SLB_CLEAR(&update_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE);
                SOC_PPC_SLB_CLEAR(&update_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_LAG_MEMBER);

                match_lag.lag_ndx = trunk_id;
                match_lag_member.lag_member_ndx = lag_info->nof_entries - 1;

                match_rules[0] = SOC_PPC_SLB_DOWNCAST(&match_lag, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
                match_rules[1] = SOC_PPC_SLB_DOWNCAST(&match_lag_member, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
                update_action.traverse_update_value = SOC_PPC_SLB_DOWNCAST(&update_lag_member, SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE);
                update_lag_member.new_lag_member_ndx = remove_idx;

                soc_sand_rv = soc_ppd_slb_traverse(soc_sand_unit, match_rules, 2, SOC_PPC_SLB_DOWNCAST(&update_action, SOC_PPC_SLB_TRAVERSE_ACTION), NULL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCM_FREE(lag_info);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_trunk_member_delete_all(int           unit, 
                                      bcm_trunk_t   tid)
{
    int                 rv; 
    int                 index; 
    int                 count;
    bcm_trunk_member_t  *members = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_ALLOC(members, (sizeof(bcm_trunk_member_t) * BCM_TRUNK_MAX_PORTCNT), "trunk mem");
    if (members == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("bcm_petra_trunk_member_delete_all err, member == NULL")));
    } 

    rv =  bcm_petra_trunk_member_get(unit, tid, BCM_TRUNK_MAX_PORTCNT, members, 
                                     &count);
    if (BCM_SUCCESS(rv)) {
        for (index = 0; index < count; index++) {

            if (BCM_TRUNK_IS_STACKING_TID(tid))  {
                rv = bcm_petra_stk_gport_sysport_get(unit, members[index].gport, &(members[index].gport));
                if (BCM_FAILURE(rv)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Failed convert trunk member from gport modport to gport sysport. members[%d].gport=0x%x."), FUNCTION_NAME(), index, members[index].gport));
                }    
            }

             rv = bcm_petra_trunk_member_delete(unit, tid, &members[index]);
             if (BCM_FAILURE(rv))  {
                 break;
             }
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(members);
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_trunk_member_in_other_trunk_check(int            unit,  
                                             bcm_gport_t    gport, 
                                             bcm_trunk_t    tid)
{
    int             rv = BCM_E_NONE;
    int             core;
    int             soc_sand_rc = SOC_SAND_OK;
    int             lag_check = 0;
    uint32          tm_port;
    uint32          lag_id;
    uint8           is_in_lag = FALSE;
    bcm_module_t    modid,port_modid; 

    BCMDNX_INIT_FUNC_DEFS;

    lag_check = soc_property_get(unit, spn_DISABLE_LAG_OTM_CHECK, 0);
    /* if SOC property is set, there is no OTM port check in LAG set/add
       allows OTM port to be a member of multiple LAG's
       property is disabled by default and should only be enabled in case 
       you need to manage policies per LAG sub interface. Enabling this will
       also require to control LAG key paramerts per OTM port and DSP mapping
       to avoid configuration collusions. If you're not sure what this means,
       you don't need this.
    */ 
    if (lag_check == 0) {
        /* Get local port */
        rv = bcm_petra_stk_sysport_gport_get(unit, gport, &gport);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Failed to get gport_modport from gport=0x%x"), gport));
        }

        tm_port = BCM_GPORT_MODPORT_PORT_GET(gport);
        BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &modid));
        port_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        /* check if port is local */ 
        if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, port_modid, modid)) {   
            core = SOC_DPP_MODID_TO_CORE(unit, modid, port_modid);
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_is_port_lag_member,(unit, core, tm_port, &is_in_lag, &lag_id)));
            if (SOC_SAND_FAILURE(soc_sand_rc)) {   
                    BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed getting port lag information. gport=%d"), gport)); 
            } 
            LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: core %d member (gport:%x) from trunk (id:%d) TM-Port %d MODID %d in lag %d lag-id %d \n"), FUNCTION_NAME(), core, gport, tid, tm_port, port_modid, is_in_lag, lag_id));
            if(is_in_lag && lag_id != tid) {
                rv = BCM_E_PARAM;
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_trunk_create_id
 * Purpose:
 *      Create the software data structure for this trunk ID and program the
 *      hardware for this TID. User must call bcm_petra_trunk_set() to finish setting
 *      up this trunk.
 * Parameters:
 *      unit - Device unit number.
 *      tid - The trunk ID.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_EXISTS    - TID already used
 *      BCM_E_BADID     - TID out of range
 */

STATIC int _bcm_petra_trunk_create_id(int           unit, 
                                      bcm_trunk_t   tid, 
                                      int           is_stacking)
{
    int             rv = BCM_E_EXISTS;
    bcm_trunk_t     trunk_id;
    int             trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, tid));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (is_stacking) {
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (trunk_id == BCM_TRUNK_INVALID) {
        rv = BCM_E_NONE;
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.set(unit, tid, tid));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, FALSE));
    }

    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid,  bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_create
 * Purpose:
 *      Allocate an available Trunk ID from the pool
 *      bcm_trunk_create_id.
 * Parameters:
 *      unit - Device unit number.
 *      flags - Flags.
 *      tid - (IN/Out) The trunk ID, IN if BCM_TRUNK_FLAG_WITH_ID flag is set.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_FULL      - Trunk table full, no more trunks available.
 *      BCM_E_XXXXX     - As set by lower layers of software
 */
int bcm_petra_trunk_create(int         unit, 
                           uint32      flags, 
                           bcm_trunk_t *tid)
{
    int             rv = BCM_E_FULL;
    int             index;
    int             is_stacking = FALSE;
    int             ngroups;
    bcm_trunk_t     trunk_id;
    bcm_trunk_t     stk_tid;
    int             trunk_db_lock_was_taken ;
    
    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    BCM_DPP_UNIT_CHECK(unit);

    if (tid == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param Error: tid == NULL.")));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    if (flags & BCM_TRUNK_FLAG_WITH_ID) {
        if (BCM_TRUNK_IS_STACKING_TID(*tid)) {
            is_stacking = TRUE;
            stk_tid = *tid;
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &stk_tid));
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_create_id(unit, stk_tid, is_stacking));
        } else {
            is_stacking = FALSE;
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_create_id(unit, *tid, is_stacking));
        }
        BCM_EXIT;
    }

    TRUNK_CHECK_INIT(unit, ts_init);
    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    for (index = 0; index < ngroups; index++) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, index, &trunk_id));
        if (trunk_id == BCM_TRUNK_INVALID) {
            rv = _bcm_petra_trunk_create_id(unit, index,is_stacking);
            if (BCM_SUCCESS(rv)) {
                *tid = index;
            }
            break;
        }
    }

    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, *tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_psc_set
 * Purpose:
 *      Set the trunk selection criteria.
 * Parameters:
 *      unit - Device unit number.
 *      tid  - The trunk ID to be affected.
 *      psc  - Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_BADID     - TID out of range
 *      BCM_E_RESOURCE -  psc set to a different value than is currently set
 *      BCM_E_PARAM     - psc value specified is not supported
 * Notes:
 *      On this platform, port selection criteria is global and cannot be
 *      configured per trunk group. The rule is, last psc_set wins and affects
 *      EVERY trunk group!
 */

int bcm_petra_trunk_psc_set(int         unit, 
                            bcm_trunk_t tid, 
                            int         psc)
{
    int                     rv;
    uint8                   is_stateful = 0;
    bcm_trunk_t             trunk_id;
    SOC_PPC_LAG_INFO        lag_info;
    SOC_PPC_LAG_LB_TYPE     lb_type = SOC_PPC_LAG_LB_TYPE_HASH;
    int                     trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;

    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, tid, psc));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    if (SOC_IS_ARADPLUS(unit)) {
        is_stateful = (psc == BCM_TRUNK_PSC_DYNAMIC_RESILIENT) ? 1 : 0;
    }

    rv = _bcm_petra_trunk_psc_to_lb_type(unit, psc, &lb_type);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, psc, bcm_errmsg(rv)));
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: Failed (%s) to read trunk (id:%d) info"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
    }

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (TRUNK_TID_VALID(trunk_id)) {
        SOC_PPC_LAG_INFO_clear(&lag_info);
        rv = soc_ppd_lag_get(unit, tid, &lag_info);
        rv = handle_sand_result(rv);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed (%s) to read trunk (id:%d) info \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
        } else if (lag_info.lb_type == lb_type &&((!SOC_IS_ARADPLUS(unit)) || (SOC_IS_ARADPLUS(unit) && lag_info.is_stateful == is_stateful))) 
        {
            /* nothing TBD */
            LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Trunk (%d) psc already set to same(%d)\n"), FUNCTION_NAME(), tid, psc));
        } else {

            if (SOC_IS_ARADPLUS(unit)) {
                lag_info.is_stateful = is_stateful;
            }
            lag_info.lb_type = lb_type;
            rv = soc_ppd_lag_set(unit, tid, &lag_info);
            rv = handle_sand_result(rv);
            if (BCM_FAILURE(rv)) {
                TRUNK_DB_UNLOCK(unit);
                trunk_db_lock_was_taken = 0 ;
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed(%s) to set trunk (id:%d) PSC to %d \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid, psc));
                BCMDNX_IF_ERR_EXIT(rv);
            } 
            rv = soc_dpp_trunk_sw_db_set_trunk_attributes(unit, tid, lb_type, is_stateful);
            if (BCM_FAILURE(rv)) {
                TRUNK_DB_UNLOCK(unit);
                trunk_db_lock_was_taken = 0 ;
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed(%s) to update software data-base with trunk attributes \n"), FUNCTION_NAME(), bcm_errmsg(rv)));
                BCMDNX_IF_ERR_EXIT(rv);
            } 
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, psc, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_psc_get
 * Purpose:
 *      Get the trunk selection criteria.
 * Parameters:
 *      unit - Device unit number.
 *      tid  - The trunk ID to be used.
 *      psc  - (OUT) Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_BADID     - TID out of range
 */

int bcm_petra_trunk_psc_get(int         unit, 
                            bcm_trunk_t tid, 
                            int         *psc)
{
    int                 rv;
    bcm_trunk_t         trunk_id;
    SOC_PPC_LAG_INFO    *lag_info = NULL;
    int                 trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, tid));

    BCMDNX_NULL_CHECK(psc);

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (TRUNK_TID_VALID(trunk_id)) {
        BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "bcm_petra_trunk_psc_get.lag_info");
        if(lag_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }
        SOC_PPC_LAG_INFO_clear(lag_info);
        rv = soc_ppd_lag_get(unit, tid, lag_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed (%s) to read trunk (id:%d) info \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
        } else {
            rv = _bcm_petra_trunk_lb_type_to_psc(unit, lag_info->lb_type, psc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Invalid PSC found on trunk (id:%d) \n"), FUNCTION_NAME(), tid));
            }
            if (SOC_IS_ARADPLUS(unit)) {
                if (lag_info->is_stateful) {
                    /* Verify that the PSC is not round robin. */
                    BCMDNX_VERIFY(*psc == BCM_TRUNK_PSC_PORTFLOW);
                    *psc = BCM_TRUNK_PSC_DYNAMIC_RESILIENT;
                }
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, *psc, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_chip_info_get
 * Purpose:
 *      Get device specific trunking information.
 * Parameters:
 *      unit    - Device unit number.
 *      ta_info - (OUT) Chip specific Trunk information.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 */
int bcm_petra_trunk_chip_info_get(int                   unit, 
                                  bcm_trunk_chip_info_t *ta_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    TRUNK_CHECK_INIT(unit, ts_init);

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &(ta_info->trunk_group_count)));
    ta_info->trunk_id_min = 0;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &(ta_info->trunk_id_max)));
    --ta_info->trunk_id_max;
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &(ta_info->trunk_ports_max)));
    ta_info->trunk_fabric_id_min = -1;
    ta_info->trunk_fabric_id_max = -1;
    ta_info->trunk_fabric_ports_max = -1;

    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit,
                          bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trunk_set
 * Purpose:
 *      Adds ports to a trunk group.
 * Parameters:
 *      unit - Device unit number.
 *      tid - The trunk ID to be affected.
 *      trunk_info - Information on the trunk group.
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes:
 */
int bcm_petra_trunk_set(int                     unit,
                        bcm_trunk_t             tid,
                        bcm_trunk_info_t        *trunk_info,
                        int                     member_count,
                        bcm_trunk_member_t      *member_array )
{
    int                 index;
    int                 nports;
    bcm_error_t         rv = BCM_E_NONE;
    bcm_trunk_t         trunk_id;
    int                 trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *, [n=%d - "), FUNCTION_NAME(), unit, tid, member_count));
    for (index = 0; index < member_count; index++) 
    {
        LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, " %08X flags:%08X"), member_array[index].gport, member_array[index].flags));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "]) - Enter\n")));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) 
    {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    /* make sure trunk is in use */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (TRUNK_TID_VALID(trunk_id)) 
    {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &nports));
        /* Check number of ports in trunk group */
        if (TRUNK_PORTCNT_VALID(member_count, nports)) 
        {
            rv = _bcm_petra_trunk_set(unit, tid, trunk_info, member_count, member_array);
            if (BCM_SUCCESS(rv)) 
            {
                BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, TRUE));
            }
        } else {
            rv = BCM_E_PARAM;
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stacking_trunk_get(int unit,
                    bcm_trunk_t tid,
                    bcm_trunk_info_t *trunk_info,
                    int member_max,
                    bcm_trunk_member_t *member_array,
                    int *member_count)
{
    int             index;
    int             rv = BCM_E_NOT_FOUND;
    int             soc_sand_rc = SOC_SAND_OK;
    int             num_cos_levels = 0x0;
    uint32          dest_base_queue = 0x0; 
    uint32          first_dest_base_queue = 0x0;
    uint32          flags = 0x0;
    bcm_trunk_t     stk_tid = 0x0;
    bcm_gport_t     gport_qid, physical_port;

    BCMDNX_INIT_FUNC_DEFS;

    TRUNK_TID_STACKING_TID_GET(unit, tid, stk_tid);
            
    for (index = 0; index < SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX; index++) {

        if (index >= member_max) {
            break;
        }

        /* Read entries from Stack trunk resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_lag_packets_base_queue_id_get,(unit, stk_tid, index, &dest_base_queue)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed getting stack_lag_packets_base_queue_id. stk_tid=%d, index=%d, dest_base_queue=%d"), stk_tid, index, dest_base_queue));
        }

        if (dest_base_queue == 0x0) {
            LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(): Stacking:  Empty stack_lag. dest_base_queue=0x%x\n"), FUNCTION_NAME(), dest_base_queue));
            rv = BCM_E_NONE;
            break;
        }

        if (index == 0) {
            first_dest_base_queue = dest_base_queue;
        } else {
            if (dest_base_queue == first_dest_base_queue) {
                LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(): Stacking:  wrap around DB. index=%d, dest_base_queue=0x%x\n"), FUNCTION_NAME(), index, dest_base_queue));
                break;
            }
        }

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport_qid, dest_base_queue);

        LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(): Stacking:  tid=%d, stk_tid=%d, index=%d, dest_base_queue=0x%x,  gport_qid=0x%x\n"), FUNCTION_NAME(), tid, stk_tid, index, dest_base_queue, gport_qid));

         rv = bcm_petra_cosq_gport_get(unit, gport_qid, &physical_port, &num_cos_levels, &flags);
         if (rv != BCM_E_NONE) {
             LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(): failure in getting gport(0x%x) info, error 0x%x\n"),FUNCTION_NAME(), gport_qid, rv));
             rv = BCM_E_INTERNAL;
         }

         member_array[index].gport = physical_port;

         LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(): Stacking:  member_array[index].gport=0x%x, physical_port=0x%x\n"), FUNCTION_NAME(), member_array[index].gport, physical_port));
    }

    *member_count = index;

    sal_memset(trunk_info, 0, sizeof(*trunk_info));
    trunk_info->dlf_index = trunk_info->mc_index = -1;
    trunk_info->ipmc_index = -1;
    trunk_info->psc = SOC_PPC_LAG_LB_TYPE_HASH;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_trunk_get
 * Purpose:
 *      Return a port information of given trunk ID.
 * Parameters:
 *      unit - Device unit number.
 *      tid - Trunk ID.
 *      trunk_info   - (OUT) Place to store returned trunk info.
 *      member_max   - (IN) Size of member_array.
 *      member_array - (OUT) Place to store returned trunk members.
 *      member_count - (OUT) Place to store returned number of trunk members.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
int bcm_petra_trunk_get(int                 unit,
                        bcm_trunk_t         tid,
                        bcm_trunk_info_t    *trunk_info,
                        int                 member_max,
                        bcm_trunk_member_t  *member_array,
                        int                 *member_count)
{
    int                 index;
    int                 rv = BCM_E_NOT_FOUND;
    int                 psc;
    int                 ngroups;
    int                 stk_ngroups;
    bcm_gport_t         modport; 
    bcm_gport_t         sys_gport;
    bcm_trunk_t         trunk_id;
    SOC_PPC_LAG_INFO    *lag_info = NULL;
    int                 trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,(BSL_META_U(unit, "%s(%d, %d, *, %d, *, *) - Enter\n"), FUNCTION_NAME(), unit, tid, member_max));

    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    BCMDNX_NULL_CHECK(trunk_info);
    BCMDNX_NULL_CHECK(member_count);

    if (member_max != 0) {
        BCMDNX_NULL_CHECK(member_array);
    }

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (!TRUNK_TID_VALID(trunk_id)) {
        LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Trunk(%d) not initialized\n"), FUNCTION_NAME(), tid));
        rv = BCM_E_NOT_FOUND;
    } else {
        BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "bcm_petra_trunk_get.lag_info");
        if(lag_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }
        SOC_PPC_LAG_INFO_clear(lag_info);

        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
        if (tid > ngroups && tid < ngroups + stk_ngroups) { 
            rv = bcm_petra_stacking_trunk_get(unit, tid, trunk_info, member_max, member_array, member_count);
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: Stacking trunk Get Failed."), FUNCTION_NAME()));
            }
        } else {

            rv = soc_dpp_trunk_sw_db_get(unit, tid, lag_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Failed (%s) to read trunk (id:%d) info \n"), FUNCTION_NAME(), bcm_errmsg(rv), tid));
            } else {
                rv = _bcm_petra_trunk_lb_type_to_psc(unit, lag_info->lb_type, &psc);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: Invalid PSC on trunk (id:%d) info \n"), FUNCTION_NAME(), tid));
                    rv = BCM_E_INTERNAL;
                }

                if (SOC_IS_ARADPLUS(unit)) {
                    if (lag_info->is_stateful) {
                        /* Verify that the PSC is not round robin. */
                        BCMDNX_VERIFY(psc == BCM_TRUNK_PSC_PORTFLOW);
                        psc = BCM_TRUNK_PSC_DYNAMIC_RESILIENT;
                    }
                }

            }
            if (!BCM_FAILURE(rv)) {
                sal_memset(trunk_info, 0, sizeof(*trunk_info));
                trunk_info->dlf_index = trunk_info->mc_index = -1;
                trunk_info->ipmc_index = -1;
                trunk_info->psc = psc;
                if (!member_max) {
                    /* no member space means querying size of aggregate */
                    *member_count = lag_info->nof_entries;
                } else {
                    /* otherwise fill available space or to end of aggregate */
                    for (index = 0; (index < member_max) && (index < lag_info->nof_entries) && (BCM_SUCCESS(rv)); index++) {
                        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, lag_info->members[index].sys_port);
                        rv = bcm_petra_stk_sysport_gport_get(unit, sys_gport, &modport);
                        sal_memset(&(member_array[index]), 0x0, sizeof(member_array[index]));
                        member_array[index].gport = modport;
                        member_array[index].flags = lag_info->members[index].flags;
                    }
                *member_count = index;
                }
            }
        }
    }

    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, *, %d, [n=%d - "), FUNCTION_NAME(), unit, tid, member_max, *member_count));
    for (index = 0; index < member_max; index++) {
        LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, " %08X flags:%08X"), member_array[index].gport, member_array[index].flags));
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "]) - Exit(%s)\n"), bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lag_info);
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_destroy
 * Purpose:
 *      Removes a trunk group. Performs hardware steps neccessary to tear
 *      down a created trunk.
 * Parameters:
 *      unit - Device unit number.
 *      tid  - Trunk Id.
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_BADID     - TID out of range
 *      BCM_E_NOT_FOUND - Trunk does not exist
 *      BCM_E_XXXXX     - As set by lower layers of software
 * Notes:
 *      The return code of the trunk_set call is purposely ignored.
 */

int bcm_petra_trunk_destroy(int         unit, 
                            bcm_trunk_t tid)
{
    int                     rv = BCM_E_NOT_FOUND;
    int                     ngroups;
    int                     stk_ngroups;
    bcm_trunk_t             trunk_id;
    bcm_trunk_info_t        trunkInfo;
    int                     trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, tid));
    TRUNK_CHECK_INIT(unit, ts_init);
    if (BCM_TRUNK_IS_STACKING_TID(tid)) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        TRUNK_CHECK_STK_TID(unit, tid);
    } else {
        TRUNK_CHECK_TID(unit, tid);
    }

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, tid, &trunk_id));
    if (trunk_id != BCM_TRUNK_INVALID) {

        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
        if (tid > ngroups && tid < ngroups + stk_ngroups) { 
            TRUNK_TID_STACKING_TID_GET(unit, tid, tid);
            BCM_TRUNK_STACKING_TID_SET(tid, tid);
            rv = bcm_petra_trunk_member_delete_all(unit, tid);
            BCMDNX_IF_ERR_EXIT(_bcm_petra_trunk_stacking_tid_to_local_tid(unit, &tid));
        } else {
            sal_memset((void *)&trunkInfo, 0, sizeof(trunkInfo));
            rv = bcm_petra_trunk_psc_get(unit, tid, &trunkInfo.psc);
            if (BCM_SUCCESS(rv)) {
                    
             BCMDNX_IF_ERR_EXIT(bcm_petra_trunk_member_delete_all(unit, tid));
             rv = _bcm_petra_trunk_set(unit, tid, &trunkInfo, 0, NULL);

            }
        }
        if (BCM_SUCCESS(rv)) {
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.set(unit, tid, BCM_TRUNK_INVALID));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, FALSE));
        }
    }

    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, tid, bcm_errmsg(rv)));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    bcm_petra_trunk_detach
 * Purpose:     Shuts down the trunk module.
 * Parameters:  unit - Device unit number.
 * Returns:     BCM_E_NONE              Success.
 *              BCM_E_XXX
 */
int
bcm_petra_trunk_detach(int unit)
{
    bcm_error_t         rv = BCM_E_NONE;
    trunk_init_state_t  init_state;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) 
    {
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.init_state.get(unit, &init_state));
    if (init_state == ts_none) 
    {
        BCM_EXIT;
    }

    if (!SOC_IS_DETACHING(unit))
    {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.init_state.set(unit, ts_none));
    }
    
    
    /* Destroy LOCK (no more data to protect */
    if (sw_state_sync_db[unit].dpp.trunk_lock != NULL) 
    {
        sal_mutex_destroy(sw_state_sync_db[unit].dpp.trunk_lock);
        sw_state_sync_db[unit].dpp.trunk_lock = NULL;
    }

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d) - Exit(%s)\n"), FUNCTION_NAME(), unit, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_bitmap_expand
 * Purpose:
 *      Given a port bitmap, if any of the ports are in a trunk,
 *      add all the trunk member ports to the bitmap.
 * Parameters:
 *      unit     - Device unit number.
 *      pbmp_ptr - Input/output port bitmap
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 * Notes:
 */
int
bcm_petra_trunk_bitmap_expand(int unit, bcm_pbmp_t *pbmp_ptr)
{
    bcm_trunk_t             tid;
    int                     index;
    bcm_pbmp_t              pbmp, t_pbmp, o_pbmp;
    int                     rv = BCM_E_NONE;
    bcm_module_t            modid;
    bcm_gport_t             modport, sys_gport;
    SOC_PPC_LAG_INFO        lag_info;
    int                     in_use;
    int                     ngroups;
    int                     trunk_db_lock_was_taken = 0;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, SOC_PBMP_WORD_GET(*pbmp_ptr,0)));

    BCMDNX_IF_ERR_EXIT( bcm_petra_stk_my_modid_get(unit, &modid));

    TRUNK_CHECK_INIT(unit, ts_init);
    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    BCM_PBMP_CLEAR(t_pbmp);
    BCM_PBMP_CLEAR(o_pbmp);
    BCM_PBMP_CLEAR(pbmp);

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    /* search over trunk IDs */
    for (tid = 0; tid < ngroups; tid++) 
    {
        /* continue to next trunk if not used */
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.get(unit, tid, &in_use));
        if (in_use != TRUE) 
        {
            continue;
        }
        
        /* create a bitmap with ports on this trunk */
        BCMDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, tid, &lag_info));
        for (index = 0; index < lag_info.nof_entries; index++) 
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, lag_info.members[index].sys_port);
            BCMDNX_IF_ERR_EXIT( bcm_petra_stk_sysport_gport_get(unit, sys_gport, &modport));
            if ( BCM_GPORT_IS_MODPORT(modport) && SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, BCM_GPORT_MODPORT_MODID_GET(modport), modid) )
            {
                BCM_PBMP_PORT_ADD(t_pbmp, BCM_GPORT_MODPORT_PORT_GET(modport));
            }
        }

        /* save a copy */
        BCM_PBMP_ASSIGN(pbmp, t_pbmp);
        /* find common ports */
        BCM_PBMP_AND(t_pbmp, *pbmp_ptr);

        /* if lists have common member */
        if (BCM_PBMP_NOT_NULL(t_pbmp) == TRUE) 
        {
            /* add saved member set */
            BCM_PBMP_OR(o_pbmp, pbmp);
        }
    }
    
    /* add all found members to the caller's pbmp */
    BCM_PBMP_OR(*pbmp_ptr, o_pbmp);
    
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, SOC_PBMP_WORD_GET(*pbmp_ptr,0), bcm_errmsg(rv)));
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_mcast_join
 * Purpose:
 *    Add the trunk group to existing MAC multicast entry.
 * Parameters:
 *      unit - Device unit number.
 *      tid - Trunk Id.
 *      vid - Vlan ID.
 *      mac - MAC address.
 * Returns:
 *    BCM_E_XXX
 * Notes:
 *      Applications have to remove the MAC multicast entry and re-add in with
 *      new port bitmap to remove the trunk group from MAC multicast entry.
 */

int
bcm_petra_trunk_mcast_join(int unit, bcm_trunk_t tid, bcm_vlan_t vid, bcm_mac_t mac)
{
    BCMDNX_INIT_FUNC_DEFS;
    LOG_VERBOSE(BSL_LS_BCM_TRUNK,
                (BSL_META_U(unit,
                            "%s(unit %d, tid %d) - This API is not available.\n"),
                            FUNCTION_NAME(), unit, tid));

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_mcast_join err , not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_trunk_find
 * Description:
 *      Get trunk id that contains the given system port
 * Parameters:
 *      unit    - Device unit number
 *      modid   - Module ID
 *      port    - TM Port number
 *      tid     - (OUT) Trunk id
 * Returns:
 *      BCM_E_NONE      - Success.
 *      BCM_E_INIT      - Trunking software not initialized
 *      BCM_E_NOT_FOUND - The module:port combo was not found in a trunk.
 */
int
bcm_petra_trunk_find(int unit, bcm_module_t modid, bcm_port_t port, bcm_trunk_t *tid)
{
    bcm_trunk_t         trunk_index;
    int                 index, found;
    int                 rv = BCM_E_NONE;
    SOC_PPC_LAG_INFO    lag_info;
    bcm_gport_t         modport, sys_gport;
    int                 in_use;
    bcm_trunk_t         trunk_id;
    int                 ngroups;
    int                 trunk_db_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    trunk_db_lock_was_taken = 0 ;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, modid, port));

    TRUNK_CHECK_INIT(unit, ts_init);

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1 ;

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    *tid = BCM_TRUNK_INVALID;

    found = 0;
    for (trunk_index = 0; ( (trunk_index < ngroups) && (found == 0) ); trunk_index++) 
    {
        /* if trunk not in use, continue */
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.get(unit, trunk_index, &in_use));
        if (TRUE != in_use) 
        {
            continue;
        }

        /* get trunk info */
        SOC_PPC_LAG_INFO_clear(&lag_info);
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, trunk_index, &trunk_id));
        
        BCMDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, trunk_id, &lag_info));

        /* check if given port is found in current trunk */
        for (index = 0; index < lag_info.nof_entries; index++) 
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, lag_info.members[index].sys_port);
            BCMDNX_IF_ERR_EXIT( bcm_petra_stk_sysport_gport_get(unit, sys_gport, &modport));
            if ((modid == BCM_GPORT_MODPORT_MODID_GET(modport)) && (port == BCM_GPORT_MODPORT_PORT_GET(modport))) 
            {
                found = 1;
                *tid = trunk_id;
                break;
            }
        }
    }

    /* check if port was found */
    if (found == 0) 
    {
        LOG_VERBOSE(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s: port:%d, in modid:%d - was not found in any trunk\n"), FUNCTION_NAME(), port, modid));
        BCMDNX_IF_ERR_EXIT( BCM_E_NOT_FOUND);
    }
    
exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, %d, %d, %d) - Exit(%s)\n"), FUNCTION_NAME(), unit, modid, port, *tid, bcm_errmsg(rv)));
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:    bcm_petra_trunk_init
 * Purpose:     Initializes the trunk module. The hardware and the software 
 *              data structures are both set to their initial states with no 
 *              trunks configured.
 * Parameters:  unit - Device unit number.
 * Returns:     BCM_E_NONE      - Success.
 *              BCM_E_MEMORY    - Out of memory
 *              BCM_E_XXXXX     - As set by lower layers of software
 */
int
bcm_petra_trunk_init(int unit)
{
    int             rv = BCM_E_NONE;
    int             ngroups;
    int             nports;        
    int             stk_ngroups;
    int             member_id = 0;
    int             current_position = 0;
    bcm_trunk_t     tid;
    uint8           is_allocated;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));


    if (sw_state_sync_db[(unit)].dpp.trunk_lock != NULL) {
        rv = bcm_petra_trunk_detach(unit);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to detach trunk module"), FUNCTION_NAME()));
        }
        sw_state_sync_db[(unit)].dpp.trunk_lock = NULL;        
    }
 
    if (sw_state_sync_db[(unit)].dpp.trunk_lock == NULL) {
        if (NULL == (sw_state_sync_db[(unit)].dpp.trunk_lock = sal_mutex_create("soc_petra_trunk_lock"))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("bcm_petra_trunk_init err,  cannot create mutex tc->lock")));
        }
    }

    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.alloc(unit));
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.set(unit, _stk_ngroups));
    rv = soc_dpp_arad_trunk_groups_ports_num_get(unit, &ngroups, &nports);
    if (rv != BCM_E_NONE) { BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("%s: failed to get correct groups & ports number of trunk"), FUNCTION_NAME()));
    }
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.set(unit, ngroups));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.set(unit, nports));

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &nports));
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));

    /* alloc memory and clear */
    if ((ngroups > 0) || (stk_ngroups > 0)) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_allocated(unit, &is_allocated));
        if(!is_allocated) {
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.alloc(unit, ngroups + stk_ngroups));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_petra_trunk_init err, ((tc->ngroups <= 0) && (tc->stk_ngroups <= 0)) ")));
    }

    if (ngroups > 0) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.is_allocated(unit, &is_allocated));
        if(!is_allocated) {
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.alloc(unit, ngroups * nports));
        }
    }

    /* Init internal structures */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.stk_ngroups.get(unit, &stk_ngroups));
    for (tid = 0; tid < (ngroups + stk_ngroups); tid++) {
        /* disable all trunk group */
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.set(unit, tid, BCM_TRUNK_INVALID));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, FALSE));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, 0));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.set(unit, tid, 0));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, 0));
    }

    for (tid = 0; tid < ngroups; ++tid) 
    {
        for (member_id = 0; member_id < nports; ++member_id) 
        {
            current_position = tid * nports + member_id;
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, current_position, BCM_GPORT_INVALID));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, current_position, 0x0));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.member_id.set(unit, current_position, member_id));
            BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.trunk_id.set(unit, current_position, tid));
        }
    }

    /* check if any HW init is required for each group */

    if (rv == BCM_E_NONE) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.init_state.set(unit, ts_init));
    }

    /* Setting all the hash polynomial indexes to a unique value in case the PP initialization was skipped. */
    if (!SOC_DPP_CONFIG(unit)->arad->init.pp_enable) {
        BCMDNX_IF_ERR_EXIT(arad_pp_lag_init_polynomial_for_tm_mode(unit));
    }

    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "bcm_petra_trunk_init: unit=%d rv=%d(%s)\n"), unit, rv, bcm_errmsg(rv)));

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    if(rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
    }
    BCMDNX_FUNC_RETURN;
}


int soc_dpp_arad_trunk_groups_ports_num_get(int unit, int *ngroups, int *nports)
{
  ARAD_PORT_LAG_MODE     lag_mode;
  uint32                 sys_lag_port_id_nof_bits;
    
    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));
    arad_ports_lag_mode_get_unsafe(unit, &lag_mode, &sys_lag_port_id_nof_bits);
    if (SOC_IS_QUX(unit)) {
        if (lag_mode == SOC_TMC_PORT_LAG_MODE_32_64) {
            *ngroups = 32;
            *nports = 64;
        } else if (lag_mode == SOC_TMC_PORT_LAG_MODE_16_128) {
            *ngroups = 16;
            *nports = 128;
        } else if (lag_mode == SOC_TMC_PORT_LAG_MODE_8_256) {
            *ngroups = 8;
            *nports = 256;
        } else {
            BCMDNX_ERR_EXIT_MSG(SOC_E_FAIL, (_BSL_BCM_MSG("%s: failed to get correct lag mode"), FUNCTION_NAME()));
        }
    } else {
        switch (lag_mode) {
        case SOC_TMC_PORT_LAG_MODE_1K_16:
            *ngroups = 1024;
            *nports = 16;
            break;
        case SOC_TMC_PORT_LAG_MODE_512_32:
            *ngroups = 512;
            *nports = 32;
            break;
        case SOC_TMC_PORT_LAG_MODE_256_64:
            *ngroups = 256;
            *nports = 64;
            break;
        case SOC_TMC_PORT_LAG_MODE_128_128:
            *ngroups = 128;
            *nports = 128;
            break;
        case SOC_TMC_PORT_LAG_MODE_64_256:
            *ngroups = 64;
            if (SOC_IS_ARADPLUS(unit)) {
                *nports = 256; /* Fixed */
            } else {
                *nports = 255;
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(SOC_E_FAIL, (_BSL_BCM_MSG("%s: failed to get correct lag mode"), FUNCTION_NAME()));
            break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_trunk_failover_set
 * Purpose:
 *      Assign the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to specify failover port list.
 *      psc - (IN) Port selection criteria for failover port list.
 *      flags - (IN) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      count - (IN) Number of ports in failover port list.
 *      fail_to_array - (IN) Failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_petra_trunk_failover_set(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                             int psc, uint32 flags, int count, 
                             bcm_gport_t *fail_to_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_failover_set unsupported"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_trunk_trunk_failover_get
 * Purpose:
 *      Retrieve the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to retrieve failover port list.
 *      psc - (OUT) Port selection criteria for failover port list.
 *      flags - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      array_size - (IN) Maximum number of ports in provided failover port list.
 *      fail_to_array - (OUT) Failover port list.
 *      array_count - (OUT) Number of ports in returned failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_petra_trunk_failover_get(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                             int *psc, uint32 *flags, int array_size, 
                             bcm_gport_t *fail_to_array, int *array_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_failover_get unsupported"))); 
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_trunk_spa_to_system_phys_port_map_get(
    int                  unit, 
    uint32               flags,
    uint32               system_port_aggregate,
    bcm_gport_t*         gport)
{
    int                         trunk_db_lock_was_taken = 0 ;
    int                         trunk_in_use = 0;
    uint8                       is_lag = 0;
    uint32                      lag_id = 0;
    uint32                      lag_member_id = 0;
    uint32                      sys_phys_port_id_dummy = 0;
    uint32                      sysport_id = 0;
    SOC_PPC_LAG_INFO            lag_info;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_TRUNK, (BSL_META_U(unit, "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    /* check that system_port_aggregate size is ok */
    if(system_port_aggregate & (~SYSTEM_PORT_AGGREGATE_MASK))
    {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("system_port_aggregate 0x%x is invalid"), system_port_aggregate));
    }

    /* check that system_port_aggregate represants a lag */
    BCM_SAND_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_logical_sys_id_parse, (unit, system_port_aggregate, &is_lag, &lag_id, &lag_member_id, &sys_phys_port_id_dummy)));
    if(!is_lag)
    {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("system_port 0x%x is not a system port aggregate invalid"), system_port_aggregate));
    }

    /* make sure valid trunk id */
    /* coverity[unsigned_compare:FALSE] */
    TRUNK_CHECK_TID(unit, lag_id);

    TRUNK_DB_LOCK(unit);
    trunk_db_lock_was_taken = 1;

    /* make sure trunk in use */
    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.get(unit, lag_id, &trunk_in_use));
    if(!trunk_in_use)
    {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("trunk ID %d is not in use"), lag_id));
    }

    /* get lag info */
    SOC_PPC_LAG_INFO_clear(&lag_info);
    BCMDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, lag_id, &lag_info));
    
    /* make sure lag_member_id is smaller than number of members in trunk */
    if(lag_member_id >= lag_info.nof_entries)
    {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("member ID %d greater or equal to number of members in trunk %d (contains %d members)"), lag_member_id, lag_id, lag_info.nof_entries));        
    }

    TRUNK_DB_UNLOCK(unit);
    trunk_db_lock_was_taken = 0;

    /* get member's sysport_id and get gport from it */
    sysport_id = lag_info.members[lag_member_id].sys_port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(*gport, sysport_id);

exit:
    if (trunk_db_lock_was_taken)
    {
        TRUNK_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP      
int      
_bcm_dpp_trunk_sw_dump(int unit)      
{
    int ngroups;
    uint32 i;
    int in_use;
    bcm_trunk_t     trunk_id;

    BCMDNX_INIT_FUNC_DEFS;

    /* Make sure the trunk module is initialized */
    TRUNK_CHECK_INIT(unit, ts_init);


    LOG_CLI((BSL_META_U(unit,
                        "\nTRUNK:")));
    LOG_CLI((BSL_META_U(unit,
                        "\n------")));

    LOG_CLI((BSL_META_U(unit,
                        "\n\ntrunks:\n")));

    BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &ngroups));
    for (i = 0; i < ngroups; i++) {
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.get(unit, i, &in_use));
        BCMDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.trunk_id.get(unit, i, &trunk_id));
        if(BCM_TRUNK_INVALID != trunk_id) {
            LOG_CLI((BSL_META_U(unit,
                                "  (%3d) trunk_id %3d in_use %c\n"),
                     i, 
                     trunk_id,
                     in_use ? 'T' : 'F'));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


#ifdef BCM_88660_A0

/** 
 * PURPOSE 
 *   Traverse all SLB entries and execute the callback (trav_fn) 
 *   on each entry that matchs the rule specified in match_entry.
 *  
 * PARAMETERS 
 *   unit [IN]
 *   flags [IN]                                   - Specify options. Flag combinations are allowed.
 *     BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID [FLAG]    - Match the trunk (LAG) group (specified in match_entry).
 *     BCM_TRUNK_RESILIENT_MATCH_MEMBER [FLAG]      - Match the trunk (LAG) member (specified in match_entry).
 *   match_entry [IN]                             - Specify the rule to use when matching entries.
 *     tid [IN]                                     - The trunk (LAG) to match (in case MATCH_TRUNK_ID is specified).
 *     member [IN]                                  - The trunk (LAG) member to match (in case MATCH_MEMBER is specified).
 *                                                    [ In by_index, the gport should be the INDEX and NOT the gport ]
 *   trav_fn [IN]                                 - A callback to be called for each matching entry.
 *                                                  If the callback returns a non-zero result, then
 *                                                  traverse stops, and BCM_E_FAIL is returned.
 *   user_data [IN]                               - Opaque user data to be passed back to the callback.
 *  
 * RETURNS 
 *   BCM_E_PARAM     - Error in parameters.
 *   BCM_E_INTERNAL  - Internal error or error in parameters.
 *   BCM_E_FAIL      - The traverse callback returned a non-zero result. 
 *  
 * NOTES 
 *   This function is only available for Arad+. 
 *   If no match flag is specified then all entries are matched. 
 */
int 
  _bcm_petra_trunk_resilient_traverse_by_index(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    bcm_trunk_resilient_traverse_cb trav_fn, 
    void *user_data
  )
{
  int rv = BCM_E_NONE;
  uint32 soc_sand_rv;

  uint32 nof_match_rules = 0;
  const uint32 possible_flags = _BCM_TRUNK_RESILIENT_MATCH_FLAGS;
  uint32 nof_scanned_entries;

  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_LAG all_lag_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG group_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG member_match_rule;
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE * match_rules[2];

  SOC_SAND_TABLE_BLOCK_RANGE block_range;

  SOC_PPC_SLB_ENTRY_KEY   *slb_keys = NULL;
  SOC_PPC_SLB_ENTRY_VALUE *slb_values = NULL;

  BCMDNX_INIT_FUNC_DEFS;

  TRUNK_CHECK_INIT(unit, ts_init);

  /* Only Arad+ and above is supported. */
  if (!SOC_IS_ARADPLUS(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_egress_ecmp_resilient_replace is unsupported for this device.\n")));
  }

  /* Check flags */
  if ((flags & ~possible_flags) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags specified (0x%x).\n"), flags & ~possible_flags));
  }

  /* We currently do not support matching the key. */
  if ((flags & BCM_TRUNK_RESILIENT_MATCH_HASH_KEY) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY (0x%x) is unsupported.\n"), BCM_TRUNK_RESILIENT_MATCH_HASH_KEY));
  }

  /* If we have some match flag, then check match_entry. */
  if (flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) {
    BCMDNX_NULL_CHECK(match_entry);
  } 

  /* Check the callback is not NULL. */
  BCMDNX_NULL_CHECK(trav_fn);

  unit = (unit);

  BCMDNX_ALLOC(slb_keys, sizeof(slb_keys[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "slb_keys");
  BCMDNX_ALLOC(slb_values, sizeof(slb_values[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "slb_values");

  if ((slb_keys == NULL) || (slb_values == NULL)) {
    BCM_FREE(slb_keys);
    BCM_FREE(slb_values);
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Could not allocate enough memory for this operation.\n")));
  }

  SOC_PPC_SLB_CLEAR(&all_lag_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG);
  SOC_PPC_SLB_CLEAR(&group_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
  SOC_PPC_SLB_CLEAR(&member_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);

  /* If there are no match flags then match all. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) == 0) {
    match_rules[0] = SOC_PPC_SLB_DOWNCAST(&all_lag_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules = 1;
  }

  if (flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) {
    group_match_rule.lag_ndx = match_entry->tid;

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&group_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    member_match_rule.lag_member_ndx = match_entry->member->gport;
    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&member_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* Prepare the block range. */
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

  SOC_SAND_TBL_ITER_SET_BEGIN(&block_range.iter);
  
  block_range.entries_to_act = SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK;
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  /* Loop over all entries. */
  while (!SOC_SAND_TBL_ITER_IS_END(&block_range.iter)) {
    unsigned int entry_idx;

    TRUNK_DB_LOCK(unit); /* { */

      soc_sand_rv = soc_ppd_slb_get_block(
        unit,
        match_rules,
        nof_match_rules,
        &block_range,
        slb_keys,
        slb_values,
        &nof_scanned_entries
      );

    /* } */ TRUNK_DB_UNLOCK(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    for (entry_idx = 0; entry_idx < nof_scanned_entries; entry_idx++) {
      bcm_trunk_resilient_entry_t entry;
      bcm_trunk_member_t lag_member;
      uint32 lag_member_ndx;
      SOC_PPC_LAG_INFO lag_info;

      BCMDNX_VERIFY(!(slb_keys[entry_idx].is_fec));
      BCMDNX_VERIFY(slb_values[entry_idx].lag_valid);
      BCMDNX_VERIFY(slb_values[entry_idx].lag_ndx == slb_keys[entry_idx].lb_group.lag_ndx);

      lag_member_ndx = slb_values[entry_idx].lag_member_ndx;
      
      SOC_PPC_LAG_INFO_clear(&lag_info);

      entry.tid = slb_values[entry_idx].lag_ndx;

      rv = soc_ppd_lag_get(unit, entry.tid, &lag_info);
      BCM_SAND_IF_ERR_EXIT(rv);

      BCMDNX_VERIFY(lag_info.is_stateful);
      BCMDNX_VERIFY(lag_info.nof_entries > lag_member_ndx);

      bcm_trunk_member_t_init(&lag_member);

      /* See above for an exaplanation about this. */
      if (lag_member_ndx >= lag_info.nof_entries) {
        lag_member.gport = lag_member_ndx;
      } else{
        rv = bcm_petra_stk_sysport_gport_get(unit, lag_info.members[lag_member_ndx].sys_port, &lag_member.gport);
        BCMDNX_IF_ERR_EXIT(rv);
      }
      lag_member.flags = 0;
      
      entry.hash_key = slb_keys[entry_idx].flow_label_id;
      entry.member = &lag_member;

      rv = trav_fn(unit, &entry, user_data);
      if (rv != 0) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_FAIL);
      }
    }
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/**
 * Wrapper around _bcm_petra_trunk_resilient_traverse_by_index to convert from gport to index. 
 * gport as match rule -> match all indices containing this gport. 
 */
int 
  bcm_petra_trunk_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    bcm_trunk_resilient_traverse_cb trav_fn, 
    void *user_data
  )
{
  int rv;
  uint32 lag_member_ndx;
  SOC_PPC_LAG_INFO *lag_info = NULL;
  bcm_trunk_resilient_entry_t match_entry_internal;
  bcm_trunk_member_t match_member;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  /* If a match flag or replace is present then check match_entry. */
  /* For matching, we have to have he match_entry. */
  /* For replace we need the match_entry to specify the group we replace the member to. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) != 0) {
    BCMDNX_NULL_CHECK(match_entry);

    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "bcm_petra_trunk_resilient_traverse.lag_info");
    if(lag_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    SOC_PPC_LAG_INFO_clear(lag_info);

    /* Remark: replace_entry->tid is ignored! We assume the gport in replace_entry is relevant to the LAG match_entry->tid. */
    /*         Otherwise we might end up trying to replace the member of LAG A with the member of LAG B. */
    rv = soc_ppd_lag_get(unit, match_entry->tid, lag_info);
    BCM_SAND_IF_ERR_EXIT(rv);
  }

  /* We currently do not support MEMBER without TRUNK_ID. */
  if (((flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) != 0) && ((flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) == 0)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                     (_BSL_BCM_MSG("BCM_TRUNK_RESILIENT_MATCH_MEMBER is only allowed in combination with BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID.\n")));
  }

  TRUNK_CHECK_INIT(unit, ts_init);

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    uint32 match_sys_port;

    rv = _bcm_petra_gport_to_sys_port(unit, match_entry->member->gport, &match_sys_port);
    BCMDNX_IF_ERR_EXIT(rv);

    match_entry_internal = *match_entry;
    match_entry_internal.member = &match_member;

    /* We need to traverse all member indices with a matching gport. */
    for (lag_member_ndx = 0; lag_member_ndx < lag_info->nof_entries; lag_member_ndx++) {
      if (lag_info->members[lag_member_ndx].sys_port == match_sys_port) {
        match_entry_internal.member->gport = lag_member_ndx;

        rv = _bcm_petra_trunk_resilient_traverse_by_index(unit, flags, &match_entry_internal, trav_fn, user_data);
        BCMDNX_IF_ERR_EXIT(rv);
      }
    }
  } else {
    rv = _bcm_petra_trunk_resilient_traverse_by_index(unit, flags, match_entry, trav_fn, user_data);
    BCMDNX_IF_ERR_EXIT(rv);
  }

exit:
  BCM_FREE(lag_info);
  BCMDNX_FUNC_RETURN;
}

/** 
 * PURPOSE 
 *   Perform an action on matching SLB ECMP entries.
 *  
 * PARAMETERS 
 *   flags [IN]                                   - Specify options. Flag combinations are allowed.
 *                                                  Any combination of match flags (MATCH_XXX) is allowed.
 *                                                  However exactly one action (REPLACE/DELETE/COUNT) flag
 *                                                  must be specified.
 *     BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID [FLAG]    - Match the trunk (LAG) group (specified in match_entry).
 *     BCM_TRUNK_RESILIENT_MATCH_MEMBER [FLAG]      - Match the trunk (LAG) member (specified in match_entry).
 *     BCM_TRUNK_RESILIENT_REPLACE [FLAG]           - Replace matching entries with the data specified
 *                                                    in replace_entry.
 *     BCM_TRUNK_RESILIENT_DELETE [FLAG]            - Delete matching entries.
 *     BCM_TRUNK_RESILIENT_COUNT [FLAG]             - Count matching entries (no action).
 *   match_entry [IN]                             - Specify the rule to use when matching entries.
 *     tid [IN]                                     - The trunk (LAG) to match (in case MATCH_TRUNK_ID is specified).
 *     member [IN]                                  - The trunk (LAG) member to match (in case MATCH_MEMBER is specified).
 *                                                    [ In by_index, the gport should be the INDEX and NOT the gport ]
 *   num_entries [OUT]                            - If non-NULL then the no. of entries that were matched is
 *                                                  returned.
 *   replace_entry [IN]                           - In case the action is REPLACE, then this will determine
 *                                                  the values that matching entries will be replaced with.
 *     member [IN]                                  - The LAG member will be replaced to this.
 *                                                    [ In by_index, the gport should be the INDEX and NOT the gport ]
 *    
 * RETURNS 
 *   BCM_E_PARAM     - Error in parameters.
 *   BCM_E_INTERNAL  - Internal error or error in parameters.
 *  
 * NOTES 
 *   This function is only available for Arad+. 
 *   If no match flag is specified then all entries are matched. 
 */
int 
  _bcm_petra_trunk_resilient_replace_by_index(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_trunk_resilient_entry_t *replace_entry
  )
{
  uint32 soc_sand_rv;

  uint32 nof_match_rules = 0;
  uint32 num_entries_internal;
  const uint32 possible_flags = _BCM_TRUNK_RESILIENT_MATCH_FLAGS | _BCM_TRUNK_RESILIENT_ACTION_FLAGS;
  
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_LAG all_lag_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG group_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG member_match_rule;
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
  SOC_PPC_SLB_TRAVERSE_ACTION *action;
  SOC_PPC_SLB_TRAVERSE_ACTION_COUNT count_action;
  SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE remove_action;
  SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE update_action;
  SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER update_value_lag_member;

  BCMDNX_INIT_FUNC_DEFS;

  TRUNK_CHECK_INIT(unit, ts_init);

  /* This is only supported for Arad+ and above. */
  if (!SOC_IS_ARADPLUS(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_resilient_replace is unsupported for this device.\n")));
  }

  /* Check possible flags. */
  if ((flags & ~possible_flags) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags specified (0x%x).\n"), flags & ~possible_flags));
  }

  /* If a match flag is present then check match_entry. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) != 0) {
    BCMDNX_NULL_CHECK(match_entry);
  }

  /* An action must be specified. */
  if ((flags & _BCM_TRUNK_RESILIENT_ACTION_FLAGS) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No action specified.\n")));
  }

  /* We currently do not support matching the key. */
  if ((flags & BCM_TRUNK_RESILIENT_MATCH_HASH_KEY) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TRUNK_MATCH_HASH_KEY (0x%x) is unsupported.\n"), BCM_TRUNK_RESILIENT_MATCH_HASH_KEY));
  }

  /* If the action is REPLACE then check replace_entry. */
  if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    BCMDNX_NULL_CHECK(replace_entry);
  }

  unit = (unit);

  SOC_PPC_SLB_CLEAR(&all_lag_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG);
  SOC_PPC_SLB_CLEAR(&group_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG);
  SOC_PPC_SLB_CLEAR(&member_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG);
  SOC_PPC_SLB_CLEAR(&count_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_COUNT);
  SOC_PPC_SLB_CLEAR(&remove_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE);
  SOC_PPC_SLB_CLEAR(&update_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE);
  SOC_PPC_SLB_CLEAR(&update_value_lag_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_LAG_MEMBER);

  /* If there are no match flags then match all. */
  if ((flags & _BCM_TRUNK_RESILIENT_MATCH_FLAGS) == 0) {
    match_rules[0] = SOC_PPC_SLB_DOWNCAST(&all_lag_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules = 1;
  }

  if (flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) {
    group_match_rule.lag_ndx = match_entry->tid;

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&group_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    member_match_rule.lag_member_ndx = (uint32)match_entry->member->gport;
    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&member_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* COUNT action. */
  if (flags & BCM_TRUNK_RESILIENT_COUNT) {
    action = SOC_PPC_SLB_DOWNCAST(&count_action, SOC_PPC_SLB_TRAVERSE_ACTION);

  /* DELETE action. */
  } else if (flags & BCM_TRUNK_RESILIENT_DELETE) {
    action = SOC_PPC_SLB_DOWNCAST(&remove_action, SOC_PPC_SLB_TRAVERSE_ACTION);

  /* REPLACE action. */
  } else if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    action = SOC_PPC_SLB_DOWNCAST(&update_action, SOC_PPC_SLB_TRAVERSE_ACTION);
    update_action.traverse_update_value = SOC_PPC_SLB_DOWNCAST(&update_value_lag_member, SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE);

    update_value_lag_member.new_lag_member_ndx = (uint32)replace_entry->member->gport;
  } else {
    BCMDNX_VERIFY(FALSE);
    action = NULL;
  }
  
  TRUNK_DB_LOCK(unit); /* { */

  soc_sand_rv = soc_ppd_slb_traverse(
      unit,
      match_rules,
      nof_match_rules,
      action,
      &num_entries_internal
    );

  /* } */ TRUNK_DB_UNLOCK(unit);

  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (num_entries) {
    *num_entries = num_entries_internal;
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/**
 * Wrapper around _bcm_petra_trunk_resilient_replace_by_index to convert from gport to index. 
 * gport as match rule -> match all indices containing this gport. 
 * gport as replace value -> replace all matches to the first member containing this gport. 
 */
int 
  bcm_petra_trunk_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_trunk_resilient_entry_t *replace_entry
  )
{
  int rv;
  uint32 lag_member_ndx;
  SOC_PPC_LAG_INFO *lag_info = NULL;
  bcm_trunk_resilient_entry_t match_entry_internal;
  bcm_trunk_resilient_entry_t replace_entry_internal;
  bcm_trunk_resilient_entry_t *replace_entry_ptr = NULL;
  bcm_trunk_member_t replace_member;
  bcm_trunk_member_t match_member;


  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  /* If a match flag or replace is present then check match_entry. */
  /* For matching, we have to have he match_entry. */
  /* For replace we need the match_entry to specify the group we replace the member to. */
  if ((flags & (_BCM_TRUNK_RESILIENT_MATCH_FLAGS | BCM_TRUNK_RESILIENT_REPLACE)) != 0) {
    BCMDNX_NULL_CHECK(match_entry);

    BCMDNX_ALLOC(lag_info, sizeof(*lag_info), "bcm_petra_trunk_resilient_replace.lag_info");
    if(lag_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    SOC_PPC_LAG_INFO_clear(lag_info);

    /* Remark: replace_entry->tid is ignored! We assume the gport in replace_entry is relevant to the LAG match_entry->tid. */
    /*         Otherwise we might end up trying to replace the member of LAG A with the member of LAG B. */
    rv = soc_ppd_lag_get(unit, match_entry->tid, lag_info);
    BCM_SAND_IF_ERR_EXIT(rv);

  }

  if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    BCMDNX_NULL_CHECK(replace_entry);
    replace_entry_internal = *replace_entry;
    replace_entry_ptr = &replace_entry_internal;
  }

  /* We currently do not support MEMBER without TRUNK_ID. */
  if (((flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) != 0) && ((flags & BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID) == 0)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                     (_BSL_BCM_MSG("BCM_TRUNK_RESILIENT_MATCH_MEMBER is only allowed in combination with BCM_TRUNK_RESILIENT_MATCH_TRUNK_ID.\n")));
  }

  TRUNK_CHECK_INIT(unit, ts_init);

  /* First find the index of the replace_entry gport and prepare replace_entry_internal if needed. */
  if (flags & BCM_TRUNK_RESILIENT_REPLACE) {
    uint32 replace_sys_port;

    rv = _bcm_petra_gport_to_sys_port(unit, replace_entry->member->gport, &replace_sys_port);
    BCMDNX_IF_ERR_EXIT(rv);

    for (lag_member_ndx = 0; lag_member_ndx < lag_info->nof_entries; lag_member_ndx++) {
      if (lag_info->members[lag_member_ndx].sys_port == replace_sys_port) {
        break;
      }
    }
    if (lag_member_ndx == lag_info->nof_entries) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No member of the LAG group %d contains gport %d.\n"), match_entry->tid, replace_entry->member->gport));
    }

    replace_entry_internal.member = &replace_member;
    replace_entry_internal.member->gport = lag_member_ndx;
  }

  if (flags & BCM_TRUNK_RESILIENT_MATCH_MEMBER) {
    uint32 match_sys_port;

    rv = _bcm_petra_gport_to_sys_port(unit, match_entry->member->gport, &match_sys_port);
    BCMDNX_IF_ERR_EXIT(rv);

    match_entry_internal = *match_entry;
    match_entry_internal.member = &match_member;
    if (num_entries) {
      *num_entries = 0;
    }

    /* We need to traverse all member indices with a matching gport. */
    for (lag_member_ndx = 0; lag_member_ndx < lag_info->nof_entries; lag_member_ndx++) {
      if (lag_info->members[lag_member_ndx].sys_port == match_sys_port) {
        int num_entries_internal;
        
        match_entry_internal.member->gport = lag_member_ndx;

        rv = _bcm_petra_trunk_resilient_replace_by_index(unit, flags, &match_entry_internal, &num_entries_internal, replace_entry_ptr);
        BCMDNX_IF_ERR_EXIT(rv);

        if (num_entries) {
          *num_entries += num_entries_internal;
        }
      }
    }
  } else {
    rv = _bcm_petra_trunk_resilient_replace_by_index(unit, flags, match_entry, num_entries, replace_entry_ptr);
    BCMDNX_IF_ERR_EXIT(rv);
  }

exit:
  BCM_FREE(lag_info);
  BCMDNX_FUNC_RETURN;
}
#else
int 
  bcm_petra_trunk_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    bcm_trunk_resilient_traverse_cb trav_fn, 
    void *user_data
  )
{
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_resilient_traverse is unsupported for this device.\n")));

exit:
  BCMDNX_FUNC_RETURN;
}

int 
  bcm_petra_trunk_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_trunk_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_trunk_resilient_entry_t *replace_entry
  )
{
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_trunk_resilient_replace is unsupported for this device.\n")));

exit:
  BCMDNX_FUNC_RETURN;
}
#endif /* BCM_88660_A0 */


