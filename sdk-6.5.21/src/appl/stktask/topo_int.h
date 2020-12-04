/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topo_int.h
 * Purpose:     
 */

#ifndef   _TOPO_INT_H_
#define   _TOPO_INT_H_

/* Accessors for topo information stored in DB structure as cookie */

#define TP_INFO(db_ref) ((topo_info_t *)((db_ref)->topo_cookie))

/*
 * The TX CXN matrix is ordered (src, dest) -> src-tx-stk-idx
 * The RX CXN matrix is ordered (dest, src) -> dest-rx-stk-idx
 *
 * src and dest are topology CPU indices
 */

#define TP_TX_CXN_MATRIX(db_ref)  (TP_INFO(db_ref)->tp_tx_cxns)
#define TP_TX_CXN_INIT(db_ref, bytes) \
    sal_memset(TP_TX_CXN_MATRIX(db_ref), TOPO_CXN_UNKNOWN, bytes)
#define TP_TX_CXN(db_ref, src_idx, dest_idx) \
    (TP_TX_CXN_MATRIX(db_ref)[db_ref->num_cpus * src_idx + dest_idx])

#define TP_RX_CXN_MATRIX(db_ref)  (TP_INFO(db_ref)->tp_rx_cxns)
#define TP_RX_CXN_INIT(db_ref, bytes) \
    sal_memset(TP_RX_CXN_MATRIX(db_ref), TOPO_CXN_UNKNOWN, bytes)
#define TP_RX_CXN(db_ref, src_idx, dest_idx) \
    (TP_RX_CXN_MATRIX(db_ref)[db_ref->num_cpus * src_idx + dest_idx])

#define TP_REACHABLE(db_ref, src_idx, dest_idx) \
         (TP_TX_CXN(db_ref, src_idx, dest_idx) != TOPO_CXN_UNKNOWN)

#define PACK_LONG _SHR_PACK_LONG
#define UNPACK_LONG _SHR_UNPACK_LONG

#endif /* _TOPO_INT_H_ */
