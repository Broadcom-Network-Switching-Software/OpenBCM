/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Remote Procedure Call BCM Dispatch Utilities
 */

#ifndef _BCM_INT_RPC_H
#define _BCM_INT_RPC_H

#ifdef  BCM_RPC_SUPPORT

#include <appl/cpudb/cpudb.h>
#include <bcm_int/rpc/entry.h>

typedef struct bcm_rpc_sreq_s {
    struct bcm_rpc_sreq_s  *next;
    cpudb_key_t         cpu;
    uint8               *buf;
    void                *cookie;
    uint32              rpckey[BCM_RPC_LOOKUP_KEYLEN];
} bcm_rpc_sreq_t;

extern void bcm_rpc_run(bcm_rpc_sreq_t *sreq);
extern uint8 *bcm_rpc_setup(uint8 dir, uint32 *key,
                            uint32 len, uint32 seq, uint32 arg);
extern void bcm_rpc_free(uint8 *buf, void *cookie);
extern int  bcm_rpc_request(int unit, uint8 *buf, int len, uint8 **rbuf,
                            void **cookie);
extern int  bcm_rpc_reply(cpudb_key_t cpu, uint8 *buf, int len);
extern int  bcm_rpc_start(void);
extern int  bcm_rpc_stop(void);
extern int  bcm_rpc_detach(int unit);
extern int  bcm_rpc_cleanup(cpudb_key_t cpu);

#endif  /* BCM_RPC_SUPPORT */
#endif  /* _BCM_INT_RPC_H */
