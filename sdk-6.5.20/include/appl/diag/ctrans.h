/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ctrans.h
 * Purpose:     
 */

#ifndef   _DIAG_CTRANS_H_
#define   _DIAG_CTRANS_H_

#include <bcm/types.h>
#include <appl/cpudb/cpudb.h>

#if defined(INCLUDE_LIB_CPUDB) && defined(INCLUDE_LIB_CPUTRANS) && \
    defined(INCLUDE_LIB_DISCOVER) && defined(INCLUDE_LIB_STKTASK)

extern const bcm_mac_t _bcast_mac;

#define MAX_DBS 10
extern cpudb_ref_t db_refs[MAX_DBS];
extern int cur_db;
extern int num_db;

#define PQ_KEY PQ_MAC

/* See stklink.h */
#define EVENT_LIST_NAMES \
    "INVALID",                    \
    "UNBlock",                    \
    "BLock",                      \
    "LinkUp",                     \
    "LinkDown",                   \
    "PKT",                        \
    "DRestart",                   \
    "DSuccess",                   \
    "DFailure",                   \
    "TSuccess",                   \
    "TFailure",                   \
    "ASuccess",                   \
    "AFailure",                   \
    "TimeOut",                    \
    "COMMFAIL",                   \
    NULL


#define PARSE_OBJ_CPUDB_STK_PORT                0
#define PARSE_OBJ_CPUDB_ENTRY                   1
#define PARSE_OBJ_MASTER_KEY                    2
#define PARSE_OBJ_LOCAL_KEY                     3
#define PARSE_OBJ_TOPO_STK_PORT                 4
#define PARSE_OBJ_TOPO_TX_MOD                   5
#define PARSE_OBJ_TOPO_RX_MOD                   6
#define PARSE_OBJ_TOPO_CPU                      7
#define PARSE_OBJ_TOPO_CLEAR                    8
#define PARSE_OBJ_TOPO_INSTALL                  9
#define PARSE_OBJ_STACK_INSTALL                 10
#define PARSE_OBJ_CPUDB_UNIT_MOD_IDS            11
#define PARSE_OBJ_LOCAL_ENTRY                   12
#define PARSE_OBJ_CPUDB_STK_PORT_STR            13

#define PARSE_OBJECT_NAMES \
    "cpudb_stk_port", \
    "cpudb_entry", \
    "master_key", \
    "local_key", \
    "topo_stk_port", \
    "topo_tx_mod",   \
    "topo_rx_mod",   \
    "topo_cpu",   \
    "topo_clear",      /* Command to clear data from parse_tp_cpu */ \
    "topo_install",    /* Command to install topology info  */ \
    "stack_install",   /* Call with current CPUDB */ \
    "cpudb_unit_mod_ids", \
    "local_entry", \
    "cpudb_stk_port_str", \
    NULL

#endif /* TKS libs defined */

#endif /* _DIAG_CTRANS_H_ */
