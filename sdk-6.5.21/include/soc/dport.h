/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DPORT_H
#define _SOC_DPORT_H

#include <soc/types.h>

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#define SOC_DPORT_MAX                   SOC_MAX_NUM_PORTS
#else
#define SOC_DPORT_MAX                   328
#endif

/* Flags used for caching config settings */
#define SOC_DPORT_MAP_F_ENABLE          0x1
#define SOC_DPORT_MAP_F_INDEXED         0x2

/* 
 * Iterate over port bitmap in dport order.
 *
 * Note that the (_p) = -1 assignment is required to avoid
 * (incorrect) warnings from certain compilers when (_p)
 * is used after the loop.
 */
#define SOC_DPORT_PBMP_ITER(_u,_pbm,_dport,_p) \
        for ((_dport) = 0, (_p) = -1; (_dport) < SOC_DPORT_MAX; (_dport)++) \
                if (((_p) = soc_dport_to_port(_u,_dport)) >= 0 && \
                    SOC_PBMP_MEMBER((_pbm),(_p)))

extern int soc_esw_dport_init(int unit);

extern int soc_dport_to_port(int unit, int dport);

extern int soc_dport_from_port(int unit, soc_port_t port);

extern int soc_dport_from_dport_idx(int unit, int dport, int idx);

extern int soc_dport_map_port(int unit, int dport, soc_port_t port);

extern int soc_dport_map_update(int unit);

#endif  /* _SOC_DPORT_H */
