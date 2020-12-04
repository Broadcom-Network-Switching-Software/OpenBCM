/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The diag shell port mapping functions are located in the SOC layer
 * for historical reasons, but the mappings are not used internally
 * in either the SOC layer or the BCM layer.
 */

#ifndef __DIAG_DPORT_H__
#define __DIAG_DPORT_H__

#include <sal/types.h>

#include <soc/dport.h>

#include <bcm_int/api_xlate_port.h>

#ifdef INCLUDE_BCM_API_XLATE_PORT
#define XLATE_P2A(_u,_p) BCM_API_XLATE_PORT_P2A(_u,&_p) == 0
#else
#define XLATE_P2A(_u,_p) 1
#endif

/* Iterate over BCM port bitmap in user port order */
#define DPORT_BCM_PBMP_ITER(_u,_pbm,_dport,_p) \
        for ((_dport) = 0, (_p) = -1; (_dport) < SOC_DPORT_MAX; (_dport)++) \
                if (((_p) = soc_dport_to_port(_u,_dport)) >= 0 && \
                    XLATE_P2A(_u,_p) && SOC_PBMP_MEMBER((_pbm),(_p)))

/* Iterate over (SOC) port bitmap in user port order */
#define DPORT_SOC_PBMP_ITER(_unit,_pbm,_dport,_port) \
    SOC_DPORT_PBMP_ITER(_unit,_pbm,_dport,_port)

/* Tranalate user port number to internal port number */
#define DPORT_TO_PORT(_unit,_dport) \
    soc_dport_to_port(_unit,_dport)

/* Tranalate internal port number to user port number */
#define DPORT_FROM_PORT(_unit,_port) \
    soc_dport_from_port(_unit,_port)

/* Get user port number or port index based on currrent configuration */
#define DPORT_FROM_DPORT_INDEX(_unit,_dport,_i) \
    soc_dport_from_dport_idx(_unit,_dport,_i)

/* Map a user port number to an internal port number */
#define DPORT_MAP_PORT(_unit,_dport,_port) \
    soc_dport_map_port(_unit,_dport,_port)

/* Update dport mappings and port names */
#define DPORT_MAP_UPDATE(_unit) \
    soc_dport_map_update(_unit)

#endif /* __DIAG_DPORT_H__ */
