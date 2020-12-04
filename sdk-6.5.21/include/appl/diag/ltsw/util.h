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
