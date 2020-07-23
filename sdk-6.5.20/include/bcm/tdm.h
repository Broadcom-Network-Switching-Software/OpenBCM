/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TDM_H__
#define __BCM_TDM_H__

#include <bcm/types.h>
#include <bcm/error.h>

/* Egress editing type */
typedef enum bcm_tdm_egress_editing_type_e {
    bcmTdmEgressEditingNoHeader = 0,    /* No Header is prepended for outgoing
                                           TDM packet */
    bcmTdmEgressEditingNoStamp = 1,     /* No stamping performed by egress, TDM
                                           packet header left as it arrived from
                                           ingress */
    bcmTdmEgressEditingStampMulticastEncapId = 2, /* Stamp MC ID inside the TDM
                                           header(either OTMH or FTMH) */
    bcmTdmEgressEditingStampMulticastUserDefined = 3 /* Stamp Port Attribute in Outgoing TDM
                                           Header(either OTMH or FTMH) */
} bcm_tdm_egress_editing_type_t;

/* Ingress editing type */
typedef enum bcm_tdm_ingress_editing_type_e {
    bcmTdmIngressEditingNoChange = 0,   /* Do not prepend FTMH header, assuming
                                           is was added by upstream device */
    bcmTdmIngressEditingPrepend = 1     /* Prepend internal forwarding header
                                           (FTMH) to all received TDM packets */
} bcm_tdm_ingress_editing_type_t;

/* Control of various global TDM features */
typedef enum bcm_tdm_control_e {
    bcmTdmBypassMinPacketSize = 0,  /* Minimum packet size for Bypass TDM packet */
    bcmTdmBypassMaxPacketSize = 1,  /* Maximum packet size for Bypass TDM packet */
    bcmTdmOutHeaderType = 2,        /* One of BCM_TDM_CONTROL_OUT_HEADER_XXX. */
    bcmTdmStreamMulticastPrefix = 3 /* Set 5 Bit prefix from Stream ID to
                                       Multicast ID. */
} bcm_tdm_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_tdm_control_set(
    int unit, 
    bcm_tdm_control_t type, 
    int arg);

extern int bcm_tdm_control_get(
    int unit, 
    bcm_tdm_control_t type, 
    int *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TDM controls defines for outgoing header type */
#define BCM_TDM_CONTROL_OUT_HEADER_OTMH     0          /* TDM outgoing header is
                                                          OTMH */
#define BCM_TDM_CONTROL_OUT_HEADER_FTMH     1          /* TDM outgoing header is
                                                          FTMH */
#define BCM_TDM_CONTROL_OUT_HEADER_OTMH_4B_WITH_CUD 2          /* TDM outgoing header is
                                                          4 bytes OTMH with CUD */

/* tdm egress editing configuration */
typedef struct bcm_tdm_egress_editing_s {
    bcm_tdm_egress_editing_type_t type; /* Editing type */
    uint32 user_data;                   /* User data to be stamped in outgoing
                                           TDM header, valid only for
                                           TdmEgressEditingStampMulticastUserDefined
                                           editing type */
} bcm_tdm_egress_editing_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get visibility mode */
extern int bcm_tdm_egress_editing_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_tdm_egress_editing_t *editing);

/* Set/Get visibility mode */
extern int bcm_tdm_egress_editing_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_tdm_egress_editing_t *editing);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TDM ingress context flags */
#define BCM_TDM_INGRESS_CONTEXT_WITH_ID     0x0001     /* Use the specified
                                                          ingress context ID */
#define BCM_TDM_INGRESS_CONTEXT_REPLACE     0x0002     /* Replace the content of
                                                          already existing
                                                          ingress context */
#define BCM_TDM_INGRESS_CONTEXT_SPECIFIC_FABRIC_LINKS 0x0004     /* Select setup for
                                                          fabric links assigned
                                                          for TDM */

/* Misc. defines for TDM module */
#define BCM_TDM_USER_DATA_MAX_SIZE  6          /* Maximum size of user field in
                                                  bytes */
#define BCM_TDM_MAX_FABRIC_PORTS    112        /* maximum fabric links */

/* TDM ingress editing context */
typedef struct bcm_tdm_ingress_context_s {
    bcm_tdm_ingress_editing_type_t type; /* Ingress editing type */
    int is_mc;                          /* TRUE => multicast_id is valid, FALSE
                                           => destination_port is valid */
    bcm_gport_t destination_port;       /* destination port. Valid for unicast
                                           configuration. */
    bcm_multicast_t multicast_id;       /* Multicast Id. Valid for multicast
                                           configuration. */
    uint8 user_data[BCM_TDM_USER_DATA_MAX_SIZE]; /* User data to be set in FTMH header.
                                           Not relevant for Optimized FTMH
                                           header */
    int user_data_count;                /* Actual number of valid bits in
                                           user_data */
    bcm_port_t fabric_ports[BCM_TDM_MAX_FABRIC_PORTS]; /* Array of fabric ports used for direct
                                           routing */
    int fabric_ports_count;             /* Actual number of valid fabric ports
                                           in array. Setting count to zero
                                           implies no direct routing */
    int is_not_tdm;                     /* TRUE => CPU Injected Traffic will not
                                           be marked TDM, FALSE => Regular
                                           behaviour */
    int stamp_mcid_with_sid;            /* TRUE value means replace Multicast ID
                                           by Stream ID in FTMH header */
} bcm_tdm_ingress_context_t;

/* Initialize a tdm ingress editing context structure. */
extern void bcm_tdm_ingress_editing_context_t_init(
    bcm_tdm_ingress_context_t *editing);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set TDM editing context configuration */
extern int bcm_tdm_ingress_context_create(
    int unit, 
    uint32 flags, 
    int *ingress_context_id, 
    bcm_tdm_ingress_context_t *editing);

/* Delete TDM Ingress Context */
extern int bcm_tdm_ingress_context_destroy(
    int unit, 
    uint32 flags, 
    int ingress_context_id);

/* Get TDM editing context configuration */
extern int bcm_tdm_ingress_context_get(
    int unit, 
    uint32 flags, 
    int ingress_context_id, 
    bcm_tdm_ingress_context_t *editing);

/* Enable/Disable Drop per editing context */
extern int bcm_tdm_ingress_context_failover_set(
    int unit, 
    uint32 flags, 
    int nof_contexts, 
    int *context_ids, 
    int *context_en);

/* Get Drop mode per editing context configuration */
extern int bcm_tdm_ingress_context_failover_get(
    int unit, 
    uint32 flags, 
    int nof_contexts, 
    int *context_ids, 
    int *context_en);

/* Set context ID per stream */
extern int bcm_tdm_stream_ingress_context_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    int ingress_context_id);

/* Get context ID per stream */
extern int bcm_tdm_stream_ingress_context_get(
    int unit, 
    uint32 flags, 
    int stream_id, 
    int *ingress_context_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TDM stream configuration */
typedef struct bcm_tdm_interface_config_s {
    int stream_id_base;         /* start address for interface stream entries in
                                   Flow to Context map */
    int stream_id_enable;       /* When TRUE, Stream is defined by offset &
                                   size, When FALSE, ILKN - Stream is assigned
                                   per channel, Ethernet - only one stream per
                                   interface */
    int stream_id_key_offset;   /* Relevant for stream_id_enable TRUE only. Key
                                   offset from the beginning of packet header in
                                   bits 8b 0-255 */
    int stream_id_key_size;     /* Relevant for stream_id_enable TRUE only. Key
                                   size in bits 1-14 */
} bcm_tdm_interface_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set TDM interface stream configuration */
extern int bcm_tdm_interface_config_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_tdm_interface_config_t *interface_config);

/* Get TDM interface stream configuration */
extern int bcm_tdm_interface_config_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_tdm_interface_config_t *interface_config);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TDM_H__ */
