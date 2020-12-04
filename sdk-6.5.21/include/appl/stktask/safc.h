/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        safc.h
 * Purpose:     SAFC board programming interfaces
 */

#ifndef   _SAFC_H_
#define   _SAFC_H_

#include <appl/cpudb/cpudb.h>
#include <appl/stktask/topology.h>

extern int bcm_stack_spanning_tree_create(cpudb_ref_t db_ref,
                                          cpudb_entry_t *root, 
                                          topo_info_t **connection);

extern int bcm_stack_connection_destroy(topo_info_t *connection);

extern int bcm_board_safc_configure(topo_cpu_t *tp_cpu,
                                    cpudb_ref_t db_ref,
                                    uint32 safc_cos_map,
                                    int group_priority,
                                    topo_info_t *connection);

#endif /* _SAFC_H_ */
