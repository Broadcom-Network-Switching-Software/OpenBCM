/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        idents.h
 * Purpose:     
 */

#ifndef   _SHARED_IDENTS_H_
#define   _SHARED_IDENTS_H_

#include <shared/types.h>

/* This is the Broadcom network switching SNAP MAC and type */
#define SHARED_BRCM_NTSW_SNAP_MAC        {0xaa, 0xaa, 0x03, 0x00, 0x10, 0x18}
#define SHARED_BRCM_NTSW_SNAP_TYPE       ((uint16) 1)

/*
 * These are local IDs used to differentiate pkts.  These are really
 * used in at least two different fields, but we share the name space
 * to keep them all unique.
 */

typedef enum shared_pkt_types_e {
    SHARED_PKT_TYPE_INVALID,
    SHARED_PKT_TYPE_PROBE,          /* Discovery, probe */
    SHARED_PKT_TYPE_ROUTING,        /* Discovery, routing */
    SHARED_PKT_TYPE_CONFIG,         /* Discovery, configuration */
    SHARED_PKT_TYPE_DISC_NH,        /* Discovery, next hop pkt */
    SHARED_PKT_TYPE_NEXT_HOP,       /* Next hop protocol */
    SHARED_PKT_TYPE_ATP,            /* Acknowledge/best effort transport */
    SHARED_PKT_TYPE_NH_TX,          /* Next hop packet */
    SHARED_PKT_TYPE_C2C,            /* CPU directed packets */
    SHARED_PKT_TYPE_COUNT           /* Last, please */
} shared_pkt_types_t;

#define SHARED_PKT_TYPE_NAMES \
    {                                               \
        "invalid",                                  \
        "probe",                                    \
        "routing",                                  \
        "config",                                   \
        "disc nexthop",                             \
        "next hop",                                 \
        "atp",                                      \
        "next hop tx",                              \
        "cpu2cpu",                                  \
        "unknown"                                   \
    }


/* Client IDs for various resources */

typedef enum shared_client_id_e {
    SHARED_CLIENT_ID_INVALID,
    SHARED_CLIENT_ID_DISC_CONFIG,          /* Discovery configuration */
    SHARED_CLIENT_ID_TOPOLOGY,             /* Topology packet */
    SHARED_CLIENT_ID_TUNNEL_RX_CONNECT,    /* Set up packet tunnel */
    SHARED_CLIENT_ID_TUNNEL_RX_PKT,        /* Tunnel a received packet */
    SHARED_CLIENT_ID_TUNNEL_RX_PKT_NO_ACK, /* Tunnel best effort */
    SHARED_CLIENT_ID_TUNNEL_TX_PKT,        /* Tunnel a packet for transmit */
    SHARED_CLIENT_ID_TUNNEL_TX_PKT_NO_ACK, /* Tunnel a packet for transmit */
    SHARED_CLIENT_ID_RPC,	           /* Remote Procedure call */
    SHARED_CLIENT_ID_RLINK,                /* Async event notification */
    SHARED_CLIENT_ID_COUNT                 /* Last, please */
} shared_client_id_t;

#define SHARED_CLIENT_ID_NAMES \
    {                                               \
        "invalid",                                  \
        "disc config",                              \
        "topology",                                 \
        "tunnel connect",                           \
        "tunnel rx pkt",                            \
        "tunnel rx pkt no ack",                     \
        "tunnel tx pkt",                            \
        "tunnel tx pkt no ack",                     \
        "rpc",                                      \
        "rlink async event",                        \
        "unknown"                                   \
    }

#endif /* _SHARED_IDENTS_H_ */
