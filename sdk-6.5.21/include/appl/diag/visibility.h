/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DIAG_VISIBILITY_H
#define _DIAG_VISIBILITY_H

extern int appl_visibility_trace(int unit, int vis_options, int vis_sport, bcm_pkt_t *pkt_info);

extern void appl_pkt_trace_info_print(int unit, bcm_switch_pkt_trace_info_t *pkt_trace_info);

#endif /* _DIAG_VISIBILITY_H */
