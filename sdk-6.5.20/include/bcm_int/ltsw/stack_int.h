/*! \file stack.h
 *
 * BCM STACK internal APIs and Structures used internal only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_STACK_INT_H
#define BCMI_LTSW_STACK_INT_H

#include <bcm/types.h>

#define BCMI_L2_IF_INVALID      0xFFFF

#define BCMINT_STACK_SUB_COMP_ID_STACK_PORTS_CURRENT 1
#define BCMINT_STACK_SUB_COMP_ID_STACK_PORTS_INACTIVE 2
#define BCMINT_STACK_SUB_COMP_ID_STACK_PORTS_PREVIOUS 3

typedef struct modport_control_s {
    int                *l2_if;        /* L2_EIF allocated for stacking */
    bcm_port_t         port;           /* Port used in stacking */
    int                port_count;      /* Number of system ports on remote modid */
}modport_control_t;

typedef struct ltsw_stack_info_s {
    modport_control_t *modport;                  /*Modport used in stacking. */
    int                modport_max;        /* Max port id in each module */
    int                modport_max_first; /* Max port id in first module  for asymmetric dual modid */
    /*
     * Stacking related:
     *     stack_ports_current:   Ports currently engaged in stacking
     *     stack_ports_inactive:  Ports stacked, but explicitly blocked
     *     stack_ports_previous:  Last stack port; to detect changes
     */
    bcm_pbmp_t *stack_ports_current;
    bcm_pbmp_t *stack_ports_inactive;
    bcm_pbmp_t *stack_ports_previous;
} ltsw_stack_info_t;

extern ltsw_stack_info_t *ltsw_stack_info[BCM_MAX_NUM_UNITS];
extern int stk_my_modid[BCM_MAX_NUM_UNITS];

#define MODID_ADDRESSABLE(_u,_m) ((_m) >= 0 && (_m) <= MODID_MAX(_u))

#endif /* BCMI_LTSW_STACK_INT_H */

