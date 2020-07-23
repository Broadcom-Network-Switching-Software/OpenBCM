/*! \file stack.h
 *
 * BCM STACK internal APIs and Structures for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XGS_STACK_H
#define BCMI_LTSW_XGS_STACK_H

#include <bcm/types.h>


extern int
xfs_ltsw_stack_set(int unit,
                   int my_modid);

extern int
xfs_ltsw_stack_modport_set(int unit,
                           int modid,
                           bcm_port_t dest_port,
                           bcm_port_t gport);

extern int
xfs_ltsw_stack_modport_get(int unit,
                           int modid,
                           bcm_port_t dest_port,
                           bcm_port_t *gport);

extern int
xfs_ltsw_stack_modport_clear(int unit,
                             int modid,
                             bcm_port_t dest_port);

extern int
xfs_ltsw_stack_l2_if_map(int unit,
                         int modid,
                         bcm_port_t dest_port);

extern int
xfs_ltsw_stack_l2_if_clear(int unit,
                           int modid,
                           bcm_port_t dest_port);

extern int
xfs_ltsw_stack_init(int unit);

extern int
xfs_ltsw_stack_sys_port_get(int unit,
                            int modid,
                            bcm_port_t port,
                            int *sys_port);

extern int
xfs_ltsw_stack_sys_port_to_modport_get(int unit,
                                       int sys_port,
                                       int *modid,
                                       bcm_port_t *port);

extern int
xfs_ltsw_stack_ifa_system_source_alloc(int unit,
                                       int *sys_source_ifa_probe_copy);

extern int
xfs_ltsw_stack_ifa_system_source_destroy(int unit);
#endif /* BCMI_LTSW_XGS_STACK_H */
