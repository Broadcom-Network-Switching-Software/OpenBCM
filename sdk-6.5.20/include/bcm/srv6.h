/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_SRV6_H__
#define __BCM_SRV6_H__

#include <bcm/types.h>

/* SRv6 SID initiator flags */
#define BCM_SRV6_SID_INITIATOR_WITH_ID      (1 << 0)   /* if set, the tunnel_id
                                                          will be given by user
                                                          as input, otherwise
                                                          output from
                                                          alloc_manager */
#define BCM_SRV6_SID_INITIATOR_REPLACE      (1 << 1)   /* Replace existing
                                                          entry, update existing
                                                          Tunnel */
#define BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED (1 << 2)   /* Indicate SRv6 SID
                                                          encapsulation is
                                                          pointed from EEDB only
                                                          and not from FWD or
                                                          ACL DBs */

/* SRv6 SRH base initiator flags */
#define BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID (1 << 0)   /* if set, the tunnel_id
                                                          will be given by user
                                                          as input, otherwise
                                                          output from
                                                          alloc_manager */
#define BCM_SRV6_SRH_BASE_INITIATOR_REPLACE (1 << 1)   /* Replace existing
                                                          entry, update existing
                                                          Tunnel */
#define BCM_SRV6_SRH_BASE_INITIATOR_STAT_ENABLE (1 << 2)   /* if set, SRv6 SRH Base
                                                          supports statistics */
#define BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP (1 << 3)   /* SRH Base object is
                                                          used at first pass of
                                                          two pass encapsulation
                                                          flow, without
                                                          encapsulating the SRv6
                                                          base header */
#define BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED (1 << 4)   /* if set, SRH Base
                                                          header is built to
                                                          represent an
                                                          Unified-SID SRv6
                                                          tunnel */
#define BCM_SRV6_SRH_BASE_INITIATOR_VLAN_TRANSLATION (1 << 5)   /* if set, SRv6
                                                          encapsulation provides
                                                          L2 VLAN information
                                                          for header above */
#define BCM_SRV6_SRH_BASE_INITIATOR_T_INSERT (1 << 6)   /* if set, encapsulation
                                                          of the SRv6 function
                                                          T.Insert is performed */

/* SRv6 extension terminator mapping flags */
#define BCM_SRV6_EXTENSION_TERMINATOR_REPLACE (1 << 0)   /* Replace existing entry */

/* 
 * Structure that is used in SRv6 SID initiator APIs, for managing device
 * behavior at SRv6 Tunnel Encapsulation
 */
typedef struct bcm_srv6_sid_initiator_info_s {
    uint32 flags;                       /* BCM_SRV6_SID_INITIATOR_XXX */
    bcm_gport_t tunnel_id;              /* Tunnel SRv6 SID initiator object ID */
    bcm_ip6_t sid;                      /* SID to encapsulate */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    bcm_if_t next_encap_id;             /* Next pointer interface ID, can be
                                           next SID initiator or
                                           next-encapsulation interface */
} bcm_srv6_sid_initiator_info_t;

/* 
 * Structure that is used in SRv6 SRH Base initiator APIs, for managing
 * device behavior at SRv6 Tunnel Encapsulation
 */
typedef struct bcm_srv6_srh_base_initiator_info_s {
    uint32 flags;                       /* BCM_SRV6_SRH_BASE_INITIATOR_XXX */
    bcm_gport_t tunnel_id;              /* Tunnel SRv6 SRH base object ID */
    int nof_sids;                       /* Number of SIDs in the SID list */
    int qos_map_id;                     /* QOS map identifier */
    int ttl;                            /* Tunnel header TTL */
    int dscp;                           /* Tunnel header DSCP value */
    bcm_qos_egress_model_t egress_qos_model; /* Egress qos and ttl model */
    bcm_if_t next_encap_id;             /* Next encapsulation ID */
    int nof_additional_sids_extended_encap; /* Number of SIDs encapsulated in 1st
                                           pass of extended encapsulation, to be
                                           used to update total extension length */
} bcm_srv6_srh_base_initiator_info_t;

/* Structure for additional info for SRv6 SID initiator traverse */
typedef struct bcm_srv6_sid_initiator_traverse_info_s {
    uint32 flags;           /* BCM_SRV6_SID_INITIATOR_XXX */
    uint32 traverse_flags;  /* BCM_SRV6_SID_INITIATOR_TRAVERSE_XXX */
} bcm_srv6_sid_initiator_traverse_info_t;

/* Structure for additional info for SRv6 SID initiator traverse */
typedef struct bcm_srv6_srh_base_initiator_traverse_info_s {
    uint32 flags;           /* BCM_SRV6_SRH_BASE_INITIATOR_XXX */
    uint32 traverse_flags;  /* BCM_SRV6_SRH_BASE_INITIATOR_TRAVERSE_XXX */
} bcm_srv6_srh_base_initiator_traverse_info_t;

/* 
 * Structure that is used in SRv6 extension terminator APIs. Relevant in
 * 2-pass ESR USP case only.
 */
typedef struct bcm_srv6_extension_terminator_mapping_s {
    uint32 flags;           /* BCM_SRV6_EXTENSION_TERMINATOR_XXX */
    int nof_sids;           /* Number of SIDs in SRv6 packet */
    bcm_gport_t encap_id;   /* Encapsulation id */
    bcm_gport_t port;       /* Gport destination */
} bcm_srv6_extension_terminator_mapping_t;

/* 
 * Structure for additional info for SRv6 extension terminator mapping
 * traverse
 */
typedef struct bcm_srv6_extension_terminator_traverse_info_s {
    uint32 flags;           /* BCM_SRV6_EXTENSION_TERMINATOR_XXX */
    uint32 traverse_flags;  /* BCM_SRV6_EXTENSION_TERMINATOR_TRAVERSE_XXX */
} bcm_srv6_extension_terminator_traverse_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/Get/Delete/Traverse SID list objects */
extern int bcm_srv6_sid_initiator_create(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info);

/* Create/Get/Delete/Traverse SID list objects */
extern int bcm_srv6_sid_initiator_get(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info);

/* Create/Get/Delete/Traverse SID list objects */
extern int bcm_srv6_sid_initiator_delete(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Srv6 callback function prototype */
typedef int (*bcm_srv6_sid_initiator_traverse_cb)(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/Get/Delete/Traverse SID list objects */
extern int bcm_srv6_sid_initiator_traverse(
    int unit, 
    bcm_srv6_sid_initiator_traverse_info_t additional_info, 
    bcm_srv6_sid_initiator_traverse_cb cb, 
    void *user_data);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_srv6_srh_base_initiator_create(
    int unit, 
    bcm_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_srv6_srh_base_initiator_get(
    int unit, 
    bcm_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_srv6_srh_base_initiator_delete(
    int unit, 
    bcm_srv6_srh_base_initiator_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Srv6 callback function prototype */
typedef int (*bcm_srv6_srh_base_initiator_traverse_cb)(
    int unit, 
    bcm_srv6_srh_base_initiator_info_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_srv6_srh_base_initiator_traverse(
    int unit, 
    bcm_srv6_srh_base_initiator_traverse_info_t additional_info, 
    bcm_srv6_srh_base_initiator_traverse_cb cb, 
    void *user_data);

/* 
 * Create/Get/Delete/Traverse port and encap_id, that were mapped by the
 * nof_sids
 */
extern int bcm_srv6_extension_terminator_add(
    int unit, 
    bcm_srv6_extension_terminator_mapping_t *info);

/* 
 * Create/Get/Delete/Traverse port and encap_id, that were mapped by the
 * nof_sids
 */
extern int bcm_srv6_extension_terminator_get(
    int unit, 
    bcm_srv6_extension_terminator_mapping_t *info);

/* 
 * Create/Get/Delete/Traverse port and encap_id, that were mapped by the
 * nof_sids
 */
extern int bcm_srv6_extension_terminator_delete(
    int unit, 
    bcm_srv6_extension_terminator_mapping_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Srv6 user call back function */
typedef int (*bcm_srv6_extension_terminator_traverse_cb)(
    int unit, 
    bcm_srv6_extension_terminator_mapping_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create/Get/Delete/Traverse port and encap_id, that were mapped by the
 * nof_sids
 */
extern int bcm_srv6_extension_terminator_traverse(
    int unit, 
    bcm_srv6_extension_terminator_traverse_info_t additional_info, 
    bcm_srv6_extension_terminator_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_srv6_srh_base_initiator_info_t structure. */
extern void bcm_srv6_srh_base_initiator_info_t_init(
    bcm_srv6_srh_base_initiator_info_t *info);

/* Initialize a bcm_srv6_sid_initiator_info_t structure. */
extern void bcm_srv6_sid_initiator_info_t_init(
    bcm_srv6_sid_initiator_info_t *info);

/* Initialize a bcm_srv6_extension_terminator_mapping_t structure. */
extern void bcm_srv6_extension_terminator_mapping_t_init(
    bcm_srv6_extension_terminator_mapping_t *info);

#endif /* __BCM_SRV6_H__ */
