/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/dpp/PORT/arad_ps_db.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/port_sw_db.h>
#include <shared/bitop.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dcmn/error.h>
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#define ARAD_PS_DB_IS_PS_RCPU_TYPE(ps_id) ARAD_PS_RCPU_VALID_RANGE(ps_id)
#define ARAD_PS_DB_IS_PS_CPU_TYPE(ps_id) ARAD_PS_CPU_VALID_RANGE(ps_id)
#define ARAD_PS_DB_IS_PS_ERP_TYPE(ps_id) (ARAD_FAP_EGRESS_REPLICATION_BASE_Q_PAIR == ARAD_PS2BASE_PORT_TC(ps_id))
#define ARAD_PS_DB_IS_PS_OAMP_TYPE(ps_id) (ARAD_OAMP_PORT_ID == ARAD_PS2BASE_PORT_TC(ps_id))
#define ARAD_PS_DB_IS_PS_OLP_TYPE(ps_id) (ARAD_OLP_PORT_ID == ARAD_PS2BASE_PORT_TC(ps_id))

#define ARAD_PS_DB_NOF_PS_RESOURCES 32

#define ARAD_PS_MAX_NIF_INTERFACE 29 
#define ARAD_PS_RCY_INTERFACE     ARAD_PS_MAX_NIF_INTERFACE + 1
#define ARAD_PS_ERP_INTERFACE     ARAD_PS_MAX_NIF_INTERFACE + 2
#define ARAD_PS_OAMP_INTERFACE    ARAD_PS_MAX_NIF_INTERFACE + 3
#define ARAD_PS_OLP_INTERFACE     ARAD_PS_MAX_NIF_INTERFACE + 4
#define ARAD_PS_NON_CH_INTERFACE  ARAD_PS_MAX_NIF_INTERFACE + 5 
#define ARAD_PS_RCPU_INTERFACE    ARAD_PS_MAX_NIF_INTERFACE + 6
#define ARAD_PS_DB_NOF_INTERFACES ARAD_PS_MAX_NIF_INTERFACE + 7

typedef enum arad_ps_db_resource_type_e {
    psResourceNif = 0,
    psResourceRcpu,
    psResourceCpu,
    psResourceOlp,
    psResourceErp,
    psResourceOamp,
    psResourceCount
} arad_ps_db_resource_type_t;

typedef struct arad_ps_db_data_s {
    uint32 allocated_binding_qs_bmap;
    uint32 allocated_non_binding_qs_bmap;
    uint32 prio_mode;
} arad_ps_db_data_t;

typedef struct arad_ps_db_resource_s {
    uint32 bitmap[1];
} arad_ps_db_resource_t;

typedef struct arad_ps_db_core_resources_s {
    arad_ps_db_data_t       ps[ARAD_PS_DB_NOF_PS_RESOURCES];
    arad_ps_db_resource_t   free_ps[psResourceCount];
    arad_ps_db_resource_t   allocated_ps[ARAD_PS_DB_NOF_INTERFACES];
} arad_ps_db_core_resources_t;

static arad_ps_db_core_resources_t core_resources[SOC_MAX_NUM_DEVICES][SOC_DPP_DEFS_MAX(NOF_CORES)];

STATIC void 
arad_ps_db_ps_to_type(int unit, int ps_idx, int core, arad_ps_db_resource_type_t* type) {

    if(ARAD_PS_DB_IS_PS_ERP_TYPE(ps_idx)) {
        *type = (SOC_INFO(unit).erp_port[core] == -1) ? psResourceCpu : psResourceErp;
    } else if(ARAD_PS_DB_IS_PS_OAMP_TYPE(ps_idx)) {
        *type = (SOC_INFO(unit).oamp_port[core] == -1) ? psResourceCpu : psResourceOamp;
    } else if(ARAD_PS_DB_IS_PS_OLP_TYPE(ps_idx)) {
        *type = (SOC_INFO(unit).olp_port[core] == -1) ? psResourceCpu : psResourceOlp;
    } else if(ARAD_PS_DB_IS_PS_RCPU_TYPE(ps_idx)) {
        *type = psResourceRcpu;
    } else if(ARAD_PS_DB_IS_PS_CPU_TYPE(ps_idx)) {
        *type = psResourceCpu;
    } else {
        *type = psResourceNif;
    }
}

int
arad_ps_db_init(int unit) {

    int i, core;
    arad_ps_db_resource_type_t type;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(core_resources[unit], 0, sizeof(arad_ps_db_core_resources_t)*SOC_DPP_DEFS_MAX(NOF_CORES));

    for(i=0 ; i<ARAD_PS_DB_NOF_PS_RESOURCES ; i++) {
        for(core = 0 ; core < SOC_DPP_DEFS_GET(unit, nof_cores) ; core++) {
            arad_ps_db_ps_to_type(unit, i, core, &type);
            SHR_BITSET(core_resources[unit][core].free_ps[type].bitmap, i);
        }
    }

    SOCDNX_FUNC_RETURN;
}

STATIC int
arad_ps_db_try_allocate(int unit, arad_ps_db_data_t* ps, int core, int ps_id, int out_port_priority, int is_binding, int is_init, int* allocated, int* internal_id) {

    int i , already_allocated, priority_i, q_pair;
    uint32 queues[1];
    uint8 is_hr_free;

    SOCDNX_INIT_FUNC_DEFS;

    *allocated = 0;
    *internal_id = -1;
    is_hr_free = 0;

    if(out_port_priority != ps->prio_mode && ps->prio_mode != 0 && is_binding) {
        SOC_EXIT;
    }

    *queues = ps->allocated_binding_qs_bmap | ps->allocated_non_binding_qs_bmap;

    for(i=0 ; i<ARAD_EGR_NOF_Q_PAIRS_IN_PS ; i += out_port_priority) {
        SHR_BITTEST_RANGE(queues, i, out_port_priority, already_allocated);
        if(!already_allocated) {
            
            if(is_init) {
                is_hr_free = 1;
            } else { 
                for(priority_i = 0; priority_i < out_port_priority; priority_i++) {
                    q_pair = ps_id * ARAD_EGR_NOF_Q_PAIRS_IN_PS + i + priority_i;
                    SOCDNX_IF_ERR_EXIT(soc_arad_validate_hr_is_free(unit, core, q_pair, &is_hr_free));
                    if(!is_hr_free) {
                        break;
                    }
                }
            }

            if (is_hr_free) {
                *allocated = 1;
                *internal_id = i;
                if(is_binding) {
                    SHR_BITSET_RANGE(&ps->allocated_binding_qs_bmap, i, out_port_priority);
                    ps->prio_mode = out_port_priority;
                } else {
                    SHR_BITSET_RANGE(&ps->allocated_non_binding_qs_bmap, i, out_port_priority);
                }
                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC void
arad_ps_db_try_allocate_with_id(arad_ps_db_data_t* ps, int out_port_priority, int is_binding, int use_port_priority ,int internal_id, int* allocated) {

    int already_allocated;
    uint32 queues[1];

    *allocated = 0;

    if(out_port_priority != ps->prio_mode && ps->prio_mode != 0 && use_port_priority) {
        return;
    }

    *queues = ps->allocated_binding_qs_bmap | ps->allocated_non_binding_qs_bmap;


    SHR_BITTEST_RANGE(queues, internal_id, out_port_priority, already_allocated);
    if(!already_allocated) {
        *allocated = 1;
        if(is_binding) {
            SHR_BITSET_RANGE(&ps->allocated_binding_qs_bmap, internal_id, out_port_priority);        
        } else {
            SHR_BITSET_RANGE(&ps->allocated_non_binding_qs_bmap, internal_id, out_port_priority);
        }
        if (use_port_priority) {
            ps->prio_mode = out_port_priority;
        }
    }
    
}

STATIC int
arad_ps_db_interface_get(int unit, soc_port_t port, uint32 *interface) {

    soc_port_if_t interface_type;
    int is_channelized, core, rv;
    uint32 tm_port;
    soc_dpp_config_arad_t *dpp_arad;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
    ilkn_tdm_dedicated_queuing;
    
    SOCDNX_INIT_FUNC_DEFS;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    switch(interface_type) {
        case SOC_PORT_IF_OLP:
            (*interface) = ARAD_PS_OLP_INTERFACE;
            break;
        case SOC_PORT_IF_OAMP:
            (*interface) = ARAD_PS_OAMP_INTERFACE;
            break;
        case SOC_PORT_IF_ERP:
            (*interface) = ARAD_PS_ERP_INTERFACE;
            break;
        case SOC_PORT_IF_RCY:
            (*interface) = ARAD_PS_RCY_INTERFACE;
            break;
        case SOC_PORT_IF_CPU:
            if (SOC_PBMP_MEMBER(dpp_arad->init.rcpu.slave_port_pbmp, port)) {
                (*interface) = ARAD_PS_RCPU_INTERFACE;
                break;
            }
            
        default:
            
            if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, interface));
                if(interface_type == SOC_PORT_IF_ILKN) {
                    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
                    if((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && (!IS_TDM_PORT(unit,port))) {
                        (*interface)= (*interface) + 1;
                    }
                }
            } else {
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, interface ));
                SOCDNX_IF_ERR_EXIT(rv);
                rv = soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized);
                SOCDNX_IF_ERR_EXIT(rv);

                if((*interface) >= SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces) || !is_channelized) {
                    (*interface) = ARAD_PS_NON_CH_INTERFACE;
                }
            }
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_ps_db_alloc_binding_ps_with_id(int unit, soc_port_t port, int out_port_priority, int base_q_pair) {

    uint32 interface;
    int core, allocated, internal_id, i;
    int ps_is_free = 0, ps_type = 0, ps_idx;
    SOCDNX_INIT_FUNC_DEFS;

    ps_idx = base_q_pair / ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    internal_id = base_q_pair % ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    SOCDNX_IF_ERR_EXIT(arad_ps_db_interface_get(unit, port, &interface));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));

    
    if(SHR_BITGET(core_resources[unit][core].allocated_ps[interface].bitmap, ps_idx)) {
        arad_ps_db_try_allocate_with_id(&core_resources[unit][core].ps[ps_idx], out_port_priority, 1, 1, internal_id, &allocated);
        if(!allocated) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to allocate with-id qpairs for port %d"), port)); 

        }
    } else {
        
        ps_is_free = 0;
        for(i=0 ; i<psResourceCount ; i++) {
            if(SHR_BITGET(core_resources[unit][core].free_ps[i].bitmap, ps_idx)){
                ps_is_free = 1;
                ps_type = i;
                break;
            }
        }

        if(!ps_is_free) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to allocate with-id qpairs for port %d, PS is used by other interface"), port)); 
        }

        
        arad_ps_db_try_allocate_with_id(&core_resources[unit][core].ps[ps_idx], out_port_priority, 1, 1, internal_id, &allocated);
        if(allocated) {
            SHR_BITCLR(core_resources[unit][core].free_ps[ps_type].bitmap, ps_idx);
            SHR_BITSET(core_resources[unit][core].allocated_ps[interface].bitmap, ps_idx);
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to allocate with-id qpairs for port %d"), port)); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_ps_db_find_free_binding_ps(int unit, soc_port_t port, int out_port_priority, int is_init, int* base_q_pair) {

    int i, j, core;
    int allocated = FALSE;
    int internal_id;
    uint32 interface, tm_port;
    arad_ps_db_resource_type_t types[psResourceCount];
    soc_port_if_t interface_type;
    soc_dpp_config_arad_t *dpp_arad;
    
    SOCDNX_INIT_FUNC_DEFS;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    *base_q_pair = -1;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));  
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    
    SOCDNX_IF_ERR_EXIT(arad_ps_db_interface_get(unit, port, &interface));

    for(i=0 ; i<ARAD_PS_DB_NOF_PS_RESOURCES ; i++) {
        if(SHR_BITGET(core_resources[unit][core].allocated_ps[interface].bitmap, i)) {
            SOCDNX_IF_ERR_EXIT(arad_ps_db_try_allocate(unit, &core_resources[unit][core].ps[i], core, i, out_port_priority, 1, is_init, &allocated, &internal_id));
            if(allocated) {
                *base_q_pair = i*ARAD_EGR_NOF_Q_PAIRS_IN_PS + internal_id;
                if (*base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
                {
                    
                    allocated = 0;
                    *base_q_pair = -1;
                }
                else
                {
                    break;
                }
            }
        }
    }

    
    if(!allocated) {
        for(i=0 ; i<psResourceCount ; i++) {
            types[i] = psResourceCount;
        }

        switch(interface_type) {
            case SOC_PORT_IF_CPU:
                if (SOC_PBMP_MEMBER(dpp_arad->init.rcpu.slave_port_pbmp, port)) {
                    types[0] = psResourceRcpu;
                } else {
                    types[0] = psResourceCpu;
                }
                break;
            case SOC_PORT_IF_OLP:
                types[0] = psResourceOlp;
                break;
            case SOC_PORT_IF_OAMP:
                types[0] = psResourceOamp;
                break;
            case SOC_PORT_IF_ERP:
                types[0] = psResourceErp;
                break;
            default:
                
                types[0] = psResourceNif;
                types[1] = psResourceRcpu;
                types[2] = psResourceCpu;
                types[3] = psResourceErp;
                break;
        }

        j=0;
        while(types[j] != psResourceCount) {
            for(i=0 ; i<ARAD_PS_DB_NOF_PS_RESOURCES ; i++) {
                if(SHR_BITGET(core_resources[unit][core].free_ps[types[j]].bitmap, i)) {
                    SOCDNX_IF_ERR_EXIT(arad_ps_db_try_allocate(unit, &core_resources[unit][core].ps[i], core, i, out_port_priority, 1, is_init, &allocated, &internal_id));
                    if(allocated) {
                        SHR_BITCLR(core_resources[unit][core].free_ps[types[j]].bitmap, i);
                        SHR_BITSET(core_resources[unit][core].allocated_ps[interface].bitmap, i);
                        *base_q_pair = i*ARAD_EGR_NOF_Q_PAIRS_IN_PS + internal_id;
                        break;
                    }
                }
            }

            if(allocated) {
                break;
            }

            j++;
        }
    }
    if (!allocated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("no free PS port %d"), port)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_ps_db_find_free_non_binding_ps(int unit, int core, int is_init, int* base_q_pair) {

    int i,
        allocated = FALSE,
        internal_id,
        any_allocated;

    SOCDNX_INIT_FUNC_DEFS;

    *base_q_pair = -1;

    
    for(i=0 ; i<ARAD_PS_DB_NOF_PS_RESOURCES ; i++) {
        if(SHR_BITGET(core_resources[unit][core].free_ps[psResourceOlp].bitmap, i) ||
           SHR_BITGET(core_resources[unit][core].free_ps[psResourceOamp].bitmap, i) ||
           SHR_BITGET(core_resources[unit][core].free_ps[psResourceErp].bitmap, i)) {
           SOCDNX_IF_ERR_EXIT(arad_ps_db_try_allocate(unit, &core_resources[unit][core].ps[i], core, i, 1, 0, is_init, &allocated, &internal_id));
            if(allocated) {
                *base_q_pair = i*ARAD_EGR_NOF_Q_PAIRS_IN_PS + internal_id;
                break;
            }
        }
    }

    
    if(!allocated) {
        for(i=0 ; i < ARAD_PS_DB_NOF_PS_RESOURCES ; i++) {
            SHR_BITTEST_RANGE(&core_resources[unit][core].ps[i].allocated_binding_qs_bmap, 0, ARAD_EGR_NOF_Q_PAIRS_IN_PS, any_allocated);
            if(any_allocated) {
                SOCDNX_IF_ERR_EXIT(arad_ps_db_try_allocate(unit, &core_resources[unit][core].ps[i], core, i, 1, 0, is_init, &allocated, &internal_id));
                if(allocated) {
                    *base_q_pair = i*ARAD_EGR_NOF_Q_PAIRS_IN_PS + internal_id;
                    break;
                }
            }
        }
    }

    
    if(!allocated) {
        for(i=0 ; i < ARAD_PS_DB_NOF_PS_RESOURCES ; i++) {
            SHR_BITTEST_RANGE(&core_resources[unit][core].ps[i].allocated_binding_qs_bmap, 0, ARAD_EGR_NOF_Q_PAIRS_IN_PS, any_allocated);
            if(!any_allocated) {
                SOCDNX_IF_ERR_EXIT(arad_ps_db_try_allocate(unit, &core_resources[unit][core].ps[i], core, i, 1, 0, is_init, &allocated, &internal_id));
                if(allocated) {
                    *base_q_pair = i*ARAD_EGR_NOF_Q_PAIRS_IN_PS + internal_id;
                    break;
                }
            }
        }
    }

    if (!allocated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("no free PS"))); 
    }
exit:   
    SOCDNX_FUNC_RETURN;
}

int 
arad_ps_db_find_free_non_binding_ps_with_id(int unit, soc_port_t port, int base_q_pair) {
    uint32 out_port_priority;
    int core, allocated, internal_id, ps_idx;

    SOCDNX_INIT_FUNC_DEFS;

    ps_idx = base_q_pair / ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    internal_id = base_q_pair % ARAD_EGR_NOF_Q_PAIRS_IN_PS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &out_port_priority));

    
    arad_ps_db_try_allocate_with_id(&core_resources[unit][core].ps[ps_idx], out_port_priority, 0, 0, internal_id, &allocated);
    if(!allocated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to allocate with-id qpairs for port %d"), port)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_ps_db_release_binding_ps(int unit, soc_port_t port, int base_q_pair) {

    int idx, core;
    int internal_id;
    int any_allocated;
    uint32 interface, tm_port;
    arad_ps_db_resource_type_t type;
    soc_port_if_t interface_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    idx = (base_q_pair) / ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    internal_id = (base_q_pair) % ARAD_EGR_NOF_Q_PAIRS_IN_PS;

    SHR_BITCLR_RANGE(&core_resources[unit][core].ps[idx].allocated_binding_qs_bmap, internal_id, core_resources[unit][core].ps[idx].prio_mode);

    
    SHR_BITTEST_RANGE(&core_resources[unit][core].ps[idx].allocated_binding_qs_bmap, 0, ARAD_EGR_NOF_Q_PAIRS_IN_PS, any_allocated);
    if(!any_allocated) {

        
        if (!((SOC_INFO(unit).erp_port[core] != -1) && ARAD_PS_DB_IS_PS_ERP_TYPE(idx))) {
            core_resources[unit][core].ps[idx].prio_mode = 0;
        }

        arad_ps_db_ps_to_type(unit, idx, core, &type);
        SHR_BITSET(core_resources[unit][core].free_ps[type].bitmap, idx);

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));  

        
        SOCDNX_IF_ERR_EXIT(arad_ps_db_interface_get(unit, port, &interface));
        SHR_BITCLR(core_resources[unit][core].allocated_ps[interface].bitmap, idx);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_ps_db_release_non_binding_ps(int unit, int core, int out_port_priority, int base_q_pair) {

    int idx;
    int internal_id;

    SOCDNX_INIT_FUNC_DEFS;

    idx = (base_q_pair) / ARAD_EGR_NOF_Q_PAIRS_IN_PS;
    internal_id = (base_q_pair) % ARAD_EGR_NOF_Q_PAIRS_IN_PS;

    SHR_BITCLR_RANGE(&core_resources[unit][core].ps[idx].allocated_non_binding_qs_bmap, internal_id, out_port_priority);

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


