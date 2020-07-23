/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utilities for LTSW diag.
 */

#if defined(BCM_LTSW_SUPPORT)

#ifndef DIAG_LTSW_UTIL_H
#define DIAG_LTSW_UTIL_H

#include <bcm/types.h>


/*
 * Iterate over port bitmap in dport order.
 *
 * Note that the (_p) = -1 assignment is required to avoid
 * (incorrect) warnings from certain compilers when (_p)
 * is used after the loop.
 */
#define DPORT_PBMP_ITER(_u,_pbm,_dport,_p) \
        for ((_dport) = 0, (_p) = -1; (_dport) < SOC_DPORT_MAX; (_dport)++) \
                if (((_p) = bcmi_ltsw_dport_to_port(_u,_dport)) >= 0 && \
                    BCM_PBMP_MEMBER((_pbm),(_p)))

extern int
bcmi_ltsw_dport_to_port(int unit, int dport);

extern int
ltsw_parse_pbmp(int unit, char *s, bcm_pbmp_t *pbmp);

extern char *
ltsw_format_pbmp(int unit, char *buf, int bufsize, bcm_pbmp_t pbmp);

extern char *
ltsw_port_name(int unit, bcm_port_t port);

extern int
ltsw_parse_port(int unit, char *s, bcm_port_t *port);

#endif /* DIAG_LTSW_UTIL_H */

#endif /* BCM_LTSW_SUPPORT */
