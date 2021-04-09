/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/port_map.h>

#define _SOC_DPP_PORT_DEF_PRIORITY (2)


int _dflt_tm_pp_port_map[SOC_MAX_NUM_DEVICES];
soc_dpp_port_map_t _port_map[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];


static soc_dpp_port_map_t _internal_port_map[SOC_MAX_NUM_DEVICES][SOC_DPP_PORT_RANGE_NUM_ENTRIES];


soc_dpp_port_t *soc_dpp_pp_ports[SOC_MAX_NUM_DEVICES] = {NULL};
soc_dpp_port_t *soc_dpp_tm_ports[SOC_MAX_NUM_DEVICES] = {NULL};



int
_soc_dpp_wb_pp_port_restore(int unit)
{
    int rv;
    int port_i;
    int nof_local_ports;
    int pp_port;
    soc_dpp_port_t pp_port_use;    
    soc_dpp_port_map_t *pmap;
    
    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
        pmap = &_port_map[unit][port_i];

        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port.get(unit, port_i, &pp_port);
        SOCDNX_IF_ERR_RETURN(rv);
        pmap->pp_port = pp_port;
    }

    nof_local_ports = SOC_DPP_DEFS_GET(unit, nof_local_ports);
    for (port_i=0; port_i< nof_local_ports; port_i++) {
        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port_use.get(unit,
            port_i,
            &pp_port_use);
        SOCDNX_IF_ERR_RETURN(rv);            
        soc_dpp_pp_ports[unit][port_i].in_use = pp_port_use.in_use;
    }
    
    return rv;
}

STATIC int
_soc_dpp_is_this_port_internal(soc_port_t port, int *port_adjust)
{
    int port_is_internal;

    if ((port >= SOC_DPP_PORT_INTERNAL_START) && (port <= SOC_DPP_PORT_INTERNAL_END)) {
         *port_adjust = port - SOC_DPP_PORT_INTERNAL_START;
         port_is_internal = TRUE;
    } else {
         *port_adjust = port;
         port_is_internal = FALSE;
    }
    return port_is_internal;
}

int
_soc_dpp_port_map_init(int unit) {
    int port_i;
    int nof_local_ports;
    int nof_tm_ports;
    soc_dpp_port_map_t *pmap, *ipmap;
    soc_error_t rv = BCM_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    nof_local_ports = SOC_DPP_DEFS_GET(unit, nof_local_ports);
    nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports);

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
        pmap = &_port_map[unit][port_i];
        pmap->nif_id = SOC_TMC_NIF_ID_NONE;
        pmap->tm_port = _SOC_DPP_PORT_INVALID;
        pmap->pp_port = _SOC_DPP_PORT_INVALID;
        pmap->channel = 0;
        pmap->pp_port = _SOC_DPP_PORT_INVALID;
        pmap->if_rate_mbps = 0;
    }

    for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES; ++port_i) {
        ipmap = &_internal_port_map[unit][port_i];
        ipmap->nif_id = SOC_TMC_NIF_ID_NONE;
        ipmap->tm_port = _SOC_DPP_PORT_INVALID;
        ipmap->pp_port = _SOC_DPP_PORT_INVALID;
        ipmap->channel = 0;
    }

    
    if (soc_dpp_pp_ports[unit] == NULL) {
        soc_dpp_pp_ports[unit] = sal_alloc(sizeof(soc_dpp_port_t) * nof_local_ports, "pp_ports");
        if (soc_dpp_pp_ports[unit] == NULL) {
             return SOC_E_MEMORY;
        }
    }

    if (soc_dpp_tm_ports[unit] == NULL) {
        soc_dpp_tm_ports[unit] = sal_alloc(sizeof(soc_dpp_port_t) * nof_tm_ports, "tm_ports");
        if (soc_dpp_tm_ports[unit] == NULL) {
             return SOC_E_MEMORY;
        }
    }

    if (!SOC_WARM_BOOT(unit)) {
        for (port_i=0; port_i< nof_local_ports; port_i++) {
            SOC_DPP_PP_PORT_FREE(unit, port_i);  
        }

        for (port_i=0; port_i< nof_tm_ports; port_i++) {
            SOC_DPP_TM_PORT_FREE(unit, port_i);  
        }
    }

    _dflt_tm_pp_port_map[unit] = TRUE;

exit:
    SOCDNX_FUNC_RETURN;
}

int
_soc_dpp_port_map_deinit(int unit) 
{
    int port_i; 
    int nof_local_ports;
    int nof_tm_ports;
    soc_error_t rv = BCM_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    nof_local_ports = SOC_DPP_DEFS_GET(unit, nof_local_ports);
    nof_tm_ports = SOC_DPP_DEFS_GET(unit, nof_logical_ports);

   
    for (port_i=0; port_i< nof_local_ports; port_i++) {
        SOC_DPP_PP_PORT_FREE(unit, port_i);  
    }

    for (port_i=0; port_i< nof_tm_ports; port_i++) {
        SOC_DPP_TM_PORT_FREE(unit, port_i);  
    }

    _dflt_tm_pp_port_map[unit] = FALSE;


    
    if (soc_dpp_pp_ports[unit] != NULL) {
        sal_free(soc_dpp_pp_ports[unit]);

    }

    if (soc_dpp_tm_ports[unit] != NULL) {
        sal_free(soc_dpp_tm_ports[unit]); 
    }
    soc_dpp_pp_ports[unit]  = NULL;
    soc_dpp_tm_ports[unit] = NULL;

exit:
    SOCDNX_FUNC_RETURN;
}

int
petra_soc_dpp_local_to_tm_port_get(int unit, soc_port_t port, uint32* tm_port, int* core) {
    int port_adjust;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
       ipmap = &_internal_port_map[unit][port_adjust];    
        *tm_port = ipmap->tm_port;

    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }

    pmap = &_port_map[unit][port];
    *tm_port = pmap->tm_port; 
    *core = 0;

    return SOC_E_NONE;
}

int
petra_soc_dpp_tm_to_local_port_get(int unit, int core, uint32 tm_port, soc_port_t *port) {
    int port_i;
    soc_dpp_port_map_t *pmap, *ipmap;

    *port = _SOC_DPP_PORT_INVALID;

    for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
        pmap = &_port_map[unit][port_i];
        if (pmap->tm_port == tm_port) {
            *port = port_i;
            return SOC_E_NONE;
        }
    }

    for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES ; ++port_i) {
        ipmap = &_internal_port_map[unit][port_i];    
        if (ipmap->tm_port == tm_port) {
            *port = port_i + SOC_DPP_PORT_INTERNAL_START;
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

int
petra_soc_dpp_pp_to_local_port_get(int unit, int core, uint32 pp_port, soc_port_t *port) {
    int port_i;
    soc_dpp_port_map_t *pmap, *ipmap;

    if (!SOC_DPP_PP_PORT_IN_USE(unit, pp_port)) {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "pp_port(%d) not in use\n"), pp_port));
        *port = _SOC_DPP_PORT_INVALID;
    } else {

        for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
            pmap = &_port_map[unit][port_i];
            if (pmap->pp_port == pp_port) {
                *port = port_i;
                return SOC_E_NONE;
            }
        }
        for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES ; ++port_i) {
           ipmap = &_internal_port_map[unit][port_i];
            if (ipmap->pp_port == pp_port) {
                *port = port_i + SOC_DPP_PORT_INTERNAL_START;
                return SOC_E_NONE;
            }
        }    
    }
    return SOC_E_NOT_FOUND;
}

int
petra_soc_dpp_local_to_tm_port_set_internal(int unit, soc_port_t port, int tm_port) {

    int port_adjust;
    soc_dpp_port_map_t *ipmap;

    if (!SOC_DPP_TM_PORT_IN_USE(unit, tm_port)) {

        if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "soc_dpp_local_to_tm_port_set_internal() internal port(%d) map to tm_port(%d)\n"), 
                                 port, tm_port));
#endif
            ipmap = &_internal_port_map[unit][port_adjust];
            ipmap->tm_port = tm_port;
            SOC_DPP_TM_PORT_RESERVE(unit, tm_port);

        } else {
           LOG_ERROR(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "only internal ports valid for this function\n"))); 
            return SOC_E_PARAM;
        }
    } else {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "tm_port(%d) already in use\n"), tm_port));
        return SOC_E_RESOURCE;
    }
    
    return SOC_E_NONE;
}

int
petra_soc_dpp_local_to_tm_port_set(int unit, soc_port_t port, int core, uint32 tm_port) {
    int port_adjust=0;
    int port_i;
    soc_dpp_port_map_t *pmap, *ipmap;
    soc_error_t rv = BCM_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {

    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }
    
    if (_dflt_tm_pp_port_map[unit] == TRUE) {
        
        for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
            pmap = &_port_map[unit][port_i];
            pmap->tm_port = _SOC_DPP_PORT_INVALID;
            pmap->pp_port = _SOC_DPP_PORT_INVALID;
        }      
        for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES; ++port_i) {
            ipmap = &_internal_port_map[unit][port_i];
            ipmap->tm_port = _SOC_DPP_PORT_INVALID;
            ipmap->pp_port = _SOC_DPP_PORT_INVALID;
        }    
    for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++port_i) {
            SOC_DPP_TM_PORT_FREE(unit, port_i);
        }
    for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_local_ports); ++port_i) {
            SOC_DPP_PP_PORT_FREE(unit, port_i);
        }
        _dflt_tm_pp_port_map[unit] = FALSE;

    }

    if (tm_port != _SOC_DPP_PORT_INVALID && SOC_DPP_TM_PORT_IN_USE(unit, tm_port)) {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "tm_port(%d) already in use\n"), tm_port));
        return SOC_E_RESOURCE;
    }

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "soc_dpp_local_to_tm_port_set() internal port(%d) map to tm_port(%d)\n"), 
                             port, tm_port));
#endif
        ipmap = &_internal_port_map[unit][port_adjust];
        _internal_port_map[unit][port_adjust].tm_port = tm_port;
        if (tm_port != _SOC_DPP_PORT_INVALID) {
            SOC_DPP_TM_PORT_RESERVE(unit, tm_port);
        }
    } else {
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "soc_dpp_local_to_tm_port_set() pbmp port(%d) map to tm_port(%d)\n"), 
                             port, tm_port));
#endif
        pmap = &_port_map[unit][port];
        pmap->tm_port = tm_port;
        if (tm_port != _SOC_DPP_PORT_INVALID) {
            SOC_DPP_TM_PORT_RESERVE(unit, tm_port);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
petra_soc_dpp_local_to_pp_port_get(int unit, soc_port_t port, uint32* pp_port, int* core) {
    int port_adjust;
    soc_dpp_port_map_t *pmap, *ipmap;

    *core = 0;

   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
        ipmap = &_internal_port_map[unit][port_adjust];
        *pp_port = ipmap->pp_port;
    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit) || port < 0) {
        return SOC_E_PARAM;
    }

    pmap = &_port_map[unit][port];
    *pp_port = pmap->pp_port; 

    return SOC_E_NONE;
}


int
petra_soc_dpp_local_to_pp_port_set_internal(int unit, soc_port_t port, int pp_port) {
    int port_adjust;
    soc_dpp_port_map_t *ipmap;
    soc_error_t rv = BCM_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_PP_PORT_IN_USE(unit, pp_port)) {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "pp_port(%d) already in use\n"), pp_port));
        return SOC_E_RESOURCE;
    }

   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "soc_dpp_local_to_tm_port_set_internal() internal port(%d) map to pp_port(%d)\n"), 
                             port, pp_port));
#endif
        ipmap = &_internal_port_map[unit][port_adjust];
        ipmap->pp_port = pp_port;
        SOC_DPP_PP_PORT_RESERVE(unit, pp_port);
    } else {
       LOG_ERROR(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "only internal ports valid for this function\n"))); 
        return SOC_E_PARAM;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
petra_soc_dpp_local_to_pp_port_set(int unit, soc_port_t port, uint32 pp_port) {
    soc_error_t rv;
    int port_i;
    int port_adjust=0;
    soc_dpp_port_map_t *pmap, *ipmap;

    SOCDNX_INIT_FUNC_DEFS;

    if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
    } else if (port >= SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit)) {
        return SOC_E_PARAM;
    }    
    if (_dflt_tm_pp_port_map[unit]) {
        
        for (port_i = 0; port_i < SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit); ++port_i) {
            pmap = &_port_map[unit][port_i];
            pmap->tm_port = _SOC_DPP_PORT_INVALID;
            pmap->pp_port = _SOC_DPP_PORT_INVALID;
        }      
        for (port_i = 0; port_i < SOC_DPP_PORT_RANGE_NUM_ENTRIES; ++port_i) {
            ipmap = &_internal_port_map[unit][port_i];
            ipmap->tm_port = _SOC_DPP_PORT_INVALID;
            ipmap->pp_port = _SOC_DPP_PORT_INVALID;
        }        
        for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_logical_ports); ++port_i) {
            SOC_DPP_TM_PORT_FREE(unit, port_i);
        }
        for (port_i = 0; port_i < SOC_DPP_DEFS_GET(unit, nof_local_ports); ++port_i) {
            SOC_DPP_PP_PORT_FREE(unit, port_i);
        }
        _dflt_tm_pp_port_map[unit] = FALSE;
   }
   if (_soc_dpp_is_this_port_internal(port, &port_adjust)) {
#ifdef DBG_LOCAL_PORT    
       LOG_INFO(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "soc_dpp_local_to_pp_port_set() internal port(%d) map to pp_port(%d)\n"), 
                            port, pp_port));
#endif   
       ipmap = &_internal_port_map[unit][port_adjust];
       ipmap->pp_port = pp_port;
       if(pp_port != _SOC_DPP_PORT_INVALID) {
           SOC_DPP_PP_PORT_RESERVE(unit, pp_port);
       }

   } else {

#ifdef DBG_LOCAL_PORT    
       LOG_INFO(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "soc_dpp_local_to_pp_port_set() pbmp port(%d) map to pp_port(%d)\n"), 
                            port, pp_port));
#endif
        pmap = &_port_map[unit][port];
        pmap->pp_port = pp_port;
        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port.set(unit, port, pp_port);
        if(SOC_FAILURE(rv)) {
            return rv;
        }

        if(pp_port != _SOC_DPP_PORT_INVALID) {
            SOC_DPP_PP_PORT_RESERVE(unit, pp_port);
        }

   }
exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME



