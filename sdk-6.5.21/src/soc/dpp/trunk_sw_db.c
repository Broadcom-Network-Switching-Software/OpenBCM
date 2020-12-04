/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trunk_sw_db.c
 * Purpose: software database action to manage Trunk
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TRUNK

#include <shared/bsl.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/debug.h>
#include <soc/dpp/trunk_sw_db.h>


int soc_dpp_trunk_sw_db_get_nof_replications(int unit, int tid, const SOC_PPC_LAG_MEMBER *lag_member, int* nof_replications, int* last_replicated_member_index)
{
    int temp_last_replicated_member_index = -1;
    int nof_members = 0;
    int current_member_index = 0;
    int current_member_position = -1;
    int max_nof_port_in_lag = -1;
    int current_member_system_port = -1;
    int nof_replications_counter = 0;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(nof_replications);
    SOCDNX_NULL_CHECK(last_replicated_member_index);

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = max_nof_port_in_lag * tid + current_member_index;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, current_member_position, &current_member_system_port));
        if(current_member_system_port != lag_member->sys_port)
        {
            continue;
        }

        ++nof_replications_counter;
        temp_last_replicated_member_index = current_member_index;
    }

    
    *nof_replications = nof_replications_counter;
    *last_replicated_member_index = temp_last_replicated_member_index;
exit:
    SOCDNX_FUNC_RETURN;
}





int soc_dpp_trunk_sw_db_get_nof_enabled_members(int unit, int tid, int* nof_enabled_members)
{
    int nof_members = 0;
    int current_member_index = 0;
    int current_member_position = -1;
    int max_nof_port_in_lag = -1;
    int enabled_members_counter = 0;
    uint32 current_member_flags = -1;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(nof_enabled_members);

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = max_nof_port_in_lag * tid + current_member_index;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, current_member_position, &current_member_flags));
        if(current_member_flags != 0x0)
        {
            continue;
        }

        ++enabled_members_counter;
    }

    
    *nof_enabled_members = enabled_members_counter;

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_trunk_sw_db_get_first_replication_index(int unit, int tid, const SOC_PPC_LAG_MEMBER *lag_member, int* first_replication_index)
{
    int nof_members = 0;
    int current_member_index = 0;
    int current_member_position = -1;
    int max_nof_port_in_lag = -1;
    int current_member_system_port = -1;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(first_replication_index);

    *first_replication_index = -1;

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = max_nof_port_in_lag * tid + current_member_index;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, current_member_position, &current_member_system_port));
        if(current_member_system_port == lag_member->sys_port)
        {
            *first_replication_index = current_member_index;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_trunk_sw_db_add(int unit, int tid, const SOC_PPC_LAG_MEMBER *lag_member)
{
    int         max_nof_tid = 0;
    int         nof_replications = 0;
    int         last_replicated_member_index = 0;
    int         last_replicated_member_position = 0;
    int         max_nof_port_in_lag = 0;
    int         nof_members;
    int         new_member_id;
    int         new_mamber_position;
    uint32      last_replicated_member_flags = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(lag_member);
    if(lag_member->flags & SOC_PPC_LAG_MEMBER_EGRESS_DISABLE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("%s: Failed.  INVALID flag for add\n"), FUNCTION_NAME(), tid));
    }
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOC_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }

    SOCDNX_IF_ERR_EXIT(soc_dpp_trunk_sw_db_get_nof_replications(unit, tid, lag_member, &nof_replications, &last_replicated_member_index));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));

    if(nof_replications > 0)
    {
        if(lag_member->flags & SOC_PPC_LAG_MEMBER_INGRESS_DISABLE)
        {
            
            SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOC_MSG("%s: Failed.  INGRESS_DISABLEd members can have only one replication in lag\n"), FUNCTION_NAME(), tid));
        }
    
        last_replicated_member_position = max_nof_port_in_lag * tid + last_replicated_member_index;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, last_replicated_member_position, &last_replicated_member_flags));
        if(last_replicated_member_flags & (SOC_PPC_LAG_MEMBER_INGRESS_DISABLE | SOC_PPC_LAG_MEMBER_EGRESS_DISABLE))
        {
            
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_replicated_member_position, 0x0));
            SOC_EXIT;
        }
    }

    if(nof_members + 1 > max_nof_port_in_lag) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOC_MSG("%s: Failed. excided allowed number of port per lag (%d)\n"), FUNCTION_NAME(), max_nof_port_in_lag ));
    }

    
    if (nof_members == 0) {
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, TRUE)); 
    }
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, nof_members + 1));
    new_member_id = nof_members;
    new_mamber_position = tid * max_nof_port_in_lag + new_member_id;
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, new_mamber_position, lag_member->sys_port));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, new_mamber_position, lag_member->flags));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_trunk_sw_db_remove_all(int unit, int tid)
{
    int max_nof_tid = 0;
    int nof_members = 0;
    int max_nof_port_in_lag = 0;
    int current_member_index = 0;
    int current_member_position = 0;


    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOC_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = max_nof_port_in_lag * tid + current_member_index;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, current_member_position, BCM_GPORT_INVALID));
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, current_member_position, 0));
    }
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, 0));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, 0));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, 0));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.set(unit, tid, 0));

exit:
    SOCDNX_FUNC_RETURN;    
}



int soc_dpp_trunk_sw_db_set(int unit, int tid, const SOC_PPC_LAG_INFO *lag_info)
{
    int max_nof_tid = 0;
    int nof_members = 0;
    int current_member_index = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(lag_info);
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOC_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_trunk_sw_db_remove_all(unit, tid));
    
    
    nof_members = lag_info->nof_entries;
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        SOCDNX_IF_ERR_EXIT(soc_dpp_trunk_sw_db_add(unit, tid, &lag_info->members[current_member_index]));            
    }

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, lag_info->lb_type));
    if (SOC_IS_ARADPLUS(unit)) {
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.set(unit, tid, lag_info->is_stateful)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_trunk_sw_db_remove(int unit, int tid, SOC_PPC_LAG_MEMBER *lag_member)
{

    int         last_replicated_member_index = -1;
    int         last_replicated_member_position = -1;
    int         max_nof_port_in_lag = 0;
    int         nof_replications = 0;
    int         max_nof_tid = 0;
    int         nof_members = 0;
    int         last_member_index = 0;
    int         last_member_position = 0;
    int         last_member_system_port = BCM_GPORT_INVALID;
    uint32      last_replicated_member_flags = 0;
    uint32      last_member_flags = 0x0;


    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_NULL_CHECK(lag_member);
    if(lag_member->flags & SOC_PPC_LAG_MEMBER_INGRESS_DISABLE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("%s: Failed.  INVALID flag for remove\n"), FUNCTION_NAME(), tid));
    }
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.ngroups.get(unit, &max_nof_tid));
    if(tid < 0 || tid >= max_nof_tid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOC_MSG("%s: Failed.  lag (id:%d) not valid\n"), FUNCTION_NAME(), tid));
    }
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_trunk_sw_db_get_nof_replications(unit, tid, lag_member, &nof_replications, &last_replicated_member_index));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));

    if(nof_replications > 0)
    {
        
        last_replicated_member_position = max_nof_port_in_lag * tid + last_replicated_member_index;
        if(lag_member->flags & SOC_PPC_LAG_MEMBER_EGRESS_DISABLE)
        {
            if(nof_replications > 1)
            {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOC_MSG("%s: Failed.  EGRESS_DISABLEd remove of members is allowed only when one replication in lag(id=%d)\n"), FUNCTION_NAME(), tid));
            }

            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, last_replicated_member_position, &last_replicated_member_flags));
            
            
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_replicated_member_position, SOC_PPC_LAG_MEMBER_EGRESS_DISABLE));
            SOC_EXIT;
        }

        last_member_index = nof_members - 1;
        last_member_position = max_nof_port_in_lag * tid + last_member_index;
        if(nof_members > 1)
        {
            
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, last_member_position, &last_member_system_port));
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, last_member_position, &last_member_flags));
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, last_replicated_member_position, last_member_system_port));
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_replicated_member_position, last_member_flags));         
        } else 
        {
            
            SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.in_use.set(unit, tid, FALSE));
        }

        
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.set(unit, last_member_position, BCM_GPORT_INVALID));
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.set(unit, last_member_position, 0x0));
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.set(unit, tid, nof_members - 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}   



int soc_dpp_trunk_sw_db_set_trunk_attributes(int unit, int tid, int psc, int is_stateful)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.set(unit, tid, psc));
    
    if(SOC_IS_ARADPLUS(unit)){
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.set(unit, tid, is_stateful));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_trunk_sw_db_get(int unit, int tid, SOC_PPC_LAG_INFO *lag_info)
{
    int             nof_members = 0;
    int             max_nof_port_in_lag = 0;
    int             current_member_index = 0;
    int             current_member_position = 0;    
    int             is_stateful = 0;
    int             psc = 0;
    bcm_gport_t     system_port = BCM_GPORT_INVALID;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_NULL_CHECK(lag_info);

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.nof_members.get(unit, tid, &nof_members));
    lag_info->nof_entries = nof_members;
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.psc.get(unit, tid, &psc));
    lag_info->lb_type = psc;
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.t_info.is_stateful.get(unit, tid, &is_stateful));
    lag_info->is_stateful = is_stateful;

    
    SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.nports.get(unit, &max_nof_port_in_lag));
    for(current_member_index = 0; current_member_index < nof_members; ++current_member_index)
    {
        current_member_position = tid * max_nof_port_in_lag + current_member_index;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.system_port.get(unit, current_member_position, &system_port));
        lag_info->members[current_member_index].sys_port = system_port;
        SOCDNX_IF_ERR_EXIT(TRUNK_ACCESS.trunk_members.flags.get(unit, current_member_position, &lag_info->members[current_member_index].flags));
    }

exit:
    SOCDNX_FUNC_RETURN;    
}


int soc_dpp_trunk_sw_db_max_nof_trunks_get(int unit, int* max_nof_trunks)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_NULL_CHECK(max_nof_trunks);

    
    TRUNK_ACCESS.ngroups.get(unit, max_nof_trunks);
    
exit:
    SOCDNX_FUNC_RETURN;    
}

