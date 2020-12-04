/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        stack.h
 * Purpose:     public stack interfaces
 */

#ifndef   _BOARD_STACK_H_
#define   _BOARD_STACK_H_

#include <bcm/types.h>
#if defined(INCLUDE_LIB_CPUDB)
#include <appl/cpudb/cpudb.h>
#endif
#if defined(INCLUDE_LIB_STKTASK)
#include <appl/stktask/topology.h>
#endif

#if defined(INCLUDE_LIB_CPUDB) && defined(INCLUDE_LIB_STKTASK)

extern int board_stack_program(cpudb_ref_t db_ref, uint32 flags);

extern int board_cpudb_get(board_driver_t *driver, cpudb_ref_t db_ref);

#endif /* defined(INCLUDE_LIB_CPUDB) defined(INCLUDE_LIB_STKTASK) */


#endif /* _STACK_H_ */
