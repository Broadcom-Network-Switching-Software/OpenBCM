/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PPP_H__
#define __BCM_PPP_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/qos.h>
#include <bcm/l3.h>

/* PPP Tunnel flags. */
#define BCM_PPP_TERM_WITH_ID                0x00000001 /* Create object with ID. */
#define BCM_PPP_TERM_REPLACE                0x00000002 /* Update existing
                                                          object. */
#define BCM_PPP_TERM_STAT_ENABLE            0x00000004 /* Enable statistics. */
#define BCM_PPP_TERM_WIDE                   0x00000008 /* Enable additional
                                                          user-data information. */
#define BCM_PPP_TERM_SESSION_ANTI_SPOOFING  0x00000010 /* Enable Session
                                                          spoofing check */
#define BCM_PPP_TERM_CROSS_CONNECT          0x00000020 /* PPP P2P mode */
#define BCM_PPP_INIT_WITH_ID                0x00000001 /* Create object with ID. */
#define BCM_PPP_INIT_REPLACE                0x00000002 /* Update existing
                                                          object. */
#define BCM_PPP_INIT_STAT_ENABLE            0x00000004 /* Enable statistics. */

/* PPP tunnel types. */
typedef enum bcm_ppp_type_e {
    bcmPPPTypePPPoE = 0,    /* PPPoE tunnel. */
    bcmPPPTypeL2TPv2 = 1,   /* L2TPv2 tunnel. */
    bcmPPPTypeCount = 2     /* Must be last */
} bcm_ppp_type_t;

/* PPP tunnel termination spoofing check type. */
typedef enum bcm_ppp_term_spoofing_check_type_e {
    bcmPPPTermSpoofingCheckTypeVlanPort = 0, /* Spoofing check arrived from the
                                           expected Incoming VLAN-Port. */
    bcmPPPTermSpoofingCheckTypeCount = 1 /* Must be last. */
} bcm_ppp_term_spoofing_check_type_t;

/* PPP tunnel terminator. */
typedef struct bcm_ppp_terminator_s {
    uint32 flags;                       /* See BCM_PPP_TERM_xxx flag
                                           definitions. */
    bcm_ppp_type_t type;                /* PPP Terminator type */
    uint32 session_id;                  /* Session id in PPPoE/L2TP header. */
    uint32 l2tpv2_tunnel_id;            /* Tunnel id in L2TP header. */
    bcm_mac_t src_mac;                  /* source MAC address. */
    uint32 network_domain;              /* Network identifier. Used to
                                           distinguish multiple networks. */
    bcm_qos_ingress_model_t ingress_qos_model; /* Ingress qos and ttl model. */
    bcm_gport_t ppp_terminator_id;      /* Gport Tunnel id */
} bcm_ppp_terminator_t;

/* PPP tunnel initiator. */
typedef struct bcm_ppp_initiator_s {
    uint32 flags;                       /* See BCM_PPP_INIT_xxx flag
                                           definitions. */
    bcm_ppp_type_t type;                /* PPP Initiator type. */
    uint32 session_id;                  /* Session ID. */
    uint32 l2tpv2_tunnel_id;            /* Tunnel id in L2TP header. */
    bcm_if_t l3_intf_id;                /* Next Intf pointing following tunnel. */
    bcm_encap_access_t encap_access;    /* Encapsulation phase to be used */
    bcm_qos_egress_model_t egress_qos_model; /* Egress qos and ttl model */
    bcm_gport_t ppp_initiator_id;       /* Gport Tunnel id */
} bcm_ppp_initiator_t;

/* PPP tunnel termination spoofing check information. */
typedef struct bcm_ppp_term_spoofing_check_s {
    uint32 flags;                       /* See BCM_PPP_TERM_SPOOFING_CHECK_xxx
                                           flag definitions. */
    bcm_ppp_term_spoofing_check_type_t type; /* Spoofing check type */
    bcm_gport_t ppp_terminator_id;      /* PPP Terminator ID. */
    bcm_gport_t vlan_port_id;           /* VLAN Gport ID. */
} bcm_ppp_term_spoofing_check_t;

/* PPP Terminator Additional Info Structure. */
typedef struct bcm_ppp_terminator_additional_info_s {
    bcm_ppp_type_t type;    /* PPP Terminator type */
} bcm_ppp_terminator_additional_info_t;

/* PPP Initiator Additional Info Structure. */
typedef struct bcm_ppp_initiator_additional_info_s {
    bcm_ppp_type_t type;    /* PPP Initiator type */
} bcm_ppp_initiator_additional_info_t;

/* PPP Term Spoofing Check Additional Info Structure. */
typedef struct bcm_ppp_term_spoofing_check_additional_info_s {
    bcm_ppp_term_spoofing_check_type_t type; /* Spoofing check type */
} bcm_ppp_term_spoofing_check_additional_info_t;

/* bcm_ppp_terminator_traverse_cb */
typedef int (*bcm_ppp_terminator_traverse_cb)(
    int unit, 
    bcm_ppp_terminator_t *info, 
    void *user_data);

/* bcm_ppp_initiator_traverse_cb */
typedef int (*bcm_ppp_initiator_traverse_cb)(
    int unit, 
    bcm_ppp_initiator_t *info, 
    void *user_data);

/* bcm_ppp_term_spoofing_check_traverse_cb */
typedef int (*bcm_ppp_term_spoofing_check_traverse_cb)(
    int unit, 
    bcm_ppp_term_spoofing_check_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a new PPP initiator. */
extern int bcm_ppp_initiator_create(
    int unit, 
    bcm_ppp_initiator_t *info);

/* Get a PPP initiator. */
extern int bcm_ppp_initiator_get(
    int unit, 
    bcm_ppp_initiator_t *info);

/* Delete a PPP initiator. */
extern int bcm_ppp_initiator_delete(
    int unit, 
    bcm_ppp_initiator_t *info);

/* Traverse all PPP tunnel initiators */
extern int bcm_ppp_initiator_traverse(
    int unit, 
    bcm_ppp_initiator_additional_info_t *additional_info, 
    bcm_ppp_initiator_traverse_cb cb, 
    void *user_data);

/* Create a new PPP terminator. */
extern int bcm_ppp_terminator_create(
    int unit, 
    bcm_ppp_terminator_t *info);

/* Delete a PPP terminator. */
extern int bcm_ppp_terminator_delete(
    int unit, 
    bcm_ppp_terminator_t *info);

/* Get a PPP terminator. */
extern int bcm_ppp_terminator_get(
    int unit, 
    bcm_ppp_terminator_t *info);

/* Traverse PPP Tunnel Terminator Info */
extern int bcm_ppp_terminator_traverse(
    int unit, 
    bcm_ppp_terminator_additional_info_t *additional_info, 
    bcm_ppp_terminator_traverse_cb cb, 
    void *user_data);

/* Add a PPP Term spoofing check. */
extern int bcm_ppp_term_spoofing_check_add(
    int unit, 
    bcm_ppp_term_spoofing_check_t *info);

/* Delete a PPP Term spoofing check. */
extern int bcm_ppp_term_spoofing_check_delete(
    int unit, 
    bcm_ppp_term_spoofing_check_t *info);

/* Traverse PPP Term Spoofing Check Info */
extern int bcm_ppp_term_spoofing_check_traverse(
    int unit, 
    bcm_ppp_term_spoofing_check_additional_info_t *additional_info, 
    bcm_ppp_term_spoofing_check_traverse_cb cb, 
    void *user_data);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_PPP_H__ */
