/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_FCOE_H__
#define __BCM_FCOE_H__

#include <bcm/types.h>
#include <bcm/stat.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM Fiber Channel Over Ethernet module. */
extern int bcm_fcoe_init(
    int unit);

/* Detach the Fiber Channel Over Ethernet module. */
extern int bcm_fcoe_cleanup(
    int unit);

/* enable/disable FCOE without clearing any FCOE table */
extern int bcm_fcoe_enable_set(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef uint32 bcm_fcoe_nport_id_t;

typedef uint32 bcm_fcoe_fcmap_t;

#define BCM_FCOE_HOST_ROUTE             0x00000001 /* Host FCOE route */
#define BCM_FCOE_SOURCE_ROUTE           0x00000002 /* Source FCOE route */
#define BCM_FCOE_DOMAIN_ROUTE           0x00000004 /* Domain FCOE route */
#define BCM_FCOE_DST_DISCARD            0x00000008 /* Destination match discard */
#define BCM_FCOE_SRC_DISCARD            0x00000010 /* Destination match discard */
#define BCM_FCOE_MULTIPATH              0x00000020 /* Specify ECMP treatment */
#define BCM_FCOE_RPE                    0x00000040 /* Pick up new priority from
                                                      Entry */
#define BCM_FCOE_LOCAL_ADDRESS          0x00000080 /* Address belong to switch -
                                                      for FCOE_HOST_ROUTE */
#define BCM_FCOE_NO_HOP_COUNT_CHANGE    0x00000100 /* Don't decrement VFP Hop
                                                      Count */
#define BCM_FCOE_COPY_TO_CPU            0x00000200 /* Send a Copy to CPU */
#define BCM_FCOE_DEFIP_ENTRY_TYPE_FCOE  0x00000400 /* FCOE Entry in DEFIP table */

/* FCOE route info */
typedef struct bcm_fcoe_route_s {
    uint32 flags;                       /* BCM_FCOE_* flags */
    bcm_vrf_t vrf;                      /* Virtual router instance */
    bcm_fcoe_nport_id_t nport_id;       /* Destination/Source FC Nport ID */
    bcm_fcoe_nport_id_t nport_id_mask;  /* Mask used for Domain/prefix routes */
    bcm_cos_t pri;                      /* New priority in packet */
    bcm_if_t intf;                      /* L3 intf associated with this address */
    bcm_mac_t nexthop_mac;              /* Next-hop mac address */
    bcm_gport_t port;                   /* Port */
    int lookup_class;                   /* Classification lookup class ID */
    bcm_if_t encap_id;                  /* encapsulation index */
} bcm_fcoe_route_t;

/* Initialize bcm_fcoe_route_t structure. */
extern void bcm_fcoe_route_t_init(
    bcm_fcoe_route_t *route);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an entry into the FCOE routing table. */
extern int bcm_fcoe_route_add(
    int unit, 
    bcm_fcoe_route_t *route);

/* Delete an nport_id entry from the FCOE routing table. */
extern int bcm_fcoe_route_delete(
    int unit, 
    bcm_fcoe_route_t *route);

/* Delete FCOE entries based on Masked Domain prefix (network). */
extern int bcm_fcoe_route_delete_by_prefix(
    int unit, 
    bcm_fcoe_route_t *route);

/* Delete FCOE entries that do/don't match a specified L3 interface. */
extern int bcm_fcoe_route_delete_by_interface(
    int unit, 
    bcm_fcoe_route_t *route);

/* Deletes all FCOE routing table entries. */
extern int bcm_fcoe_route_delete_all(
    int unit);

/* Look up an FCOE routing table entry based on nport id. */
extern int bcm_fcoe_route_find(
    int unit, 
    bcm_fcoe_route_t *route);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FCOE_VSAN_OPTION_REPLACE        0x00000001 /* Replace the VSAN ID */
#define BCM_FCOE_VSAN_CONFIG_NORMALIZED_ZONE_CHECK 0x00000001 /* VSAN normalized zone
                                                          check */
#define BCM_FCOE_VSAN_CONFIG_FCOE_ROUTE_ENABLE 0x00000002 /* Enable routing on VSAN */

/* FCOE VSAN object */
typedef struct bcm_fcoe_vsan_s {
    uint32 config_flags;    /* BCM_FCOE_VSAN_CONFIG_* flags */
    bcm_if_t l3_intf_id;    /* L3 interface ID */
    bcm_fcoe_fcmap_t fcmap; /* Associated FC Map */
} bcm_fcoe_vsan_t;

/* VSAN controls */
typedef enum bcm_fcoe_vsan_control_e {
    bcmFcoeVsanNormalizedZoneCheck = 0, /* FCoE Zone checking is normalized */
    bcmFcoeVsanRouteEnable = 1          /* FCoE Routing enable */
} bcm_fcoe_vsan_control_t;

/* Initialize bcm_fcoe_vsan_t. */
extern void bcm_fcoe_vsan_t_init(
    bcm_fcoe_vsan_t *vsan);

#ifndef BCM_HIDE_DISPATCHABLE

/* Allocate and configure a VSAN. */
extern int bcm_fcoe_vsan_create(
    int unit, 
    uint32 options, 
    bcm_fcoe_vsan_t *vsan, 
    bcm_fcoe_vsan_id_t *vsan_id);

/* Destroy a VSAN. */
extern int bcm_fcoe_vsan_destroy(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id);

/* Destroy all VSANs. */
extern int bcm_fcoe_vsan_destroy_all(
    int unit);

/* 
 * Get VSAN controls
 * Set VSAN controls
 */
extern int bcm_fcoe_control_vsan_get(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id, 
    bcm_fcoe_vsan_control_t type, 
    int *arg);

/* 
 * Get VSAN controls
 * Set VSAN controls
 */
extern int bcm_fcoe_control_vsan_set(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id, 
    bcm_fcoe_vsan_control_t type, 
    int arg);

/* 
 * Get VSAN properties associated with a VSAN ID
 * Update VSAN properties associated with a VSAN ID
 */
extern int bcm_fcoe_vsan_get(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id, 
    bcm_fcoe_vsan_t *vsan);

/* 
 * Get VSAN properties associated with a VSAN ID
 * Update VSAN properties associated with a VSAN ID
 */
extern int bcm_fcoe_vsan_set(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id, 
    bcm_fcoe_vsan_t *vsan);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Actions on packets matching zone check */
typedef enum bcm_fcoe_zone_action_e {
    bcmFcoeZoneActionAllow = 0,     /* Allow packets matching the zone check */
    bcmFcoeZoneActionDeny = 1,      /* Deny packets matching the zone check */
    bcmFcoeZoneActionCopyToCpu = 2, /* Copy-to-CPU packets matching the zone
                                       check */
    bcmFcoeZoneActionRedirect = 3   /* redirect port to given port */
} bcm_fcoe_zone_action_t;

/* Device-independent Zone address structure */
typedef struct bcm_fcoe_zone_entry_s {
    uint32 flags;                   /* BCM_FCOE_ZONE_* flags - for Future */
    bcm_fcoe_vsan_id_t vsan_id;     /* VSAN identifier */
    bcm_fcoe_nport_id_t d_id;       /* Destination FC Nport ID */
    bcm_fcoe_nport_id_t s_id;       /* Source FC Nport ID */
    int class_id;                   /* Class id to Field Processor */
    bcm_fcoe_zone_action_t action;  /* Action to take for Zone match packets */
    bcm_gport_t dest_port;          /* destination port relevant for redirect
                                       action */
} bcm_fcoe_zone_entry_t;

/* Initialize FCOE zone entry. */
extern void bcm_fcoe_zone_entry_t_init(
    bcm_fcoe_zone_entry_t *zone);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add FCOE zoning entry. */
extern int bcm_fcoe_zone_add(
    int unit, 
    bcm_fcoe_zone_entry_t *zone);

/* Delete FCOE zoning entry by VSAN ID. */
extern int bcm_fcoe_zone_delete_by_vsan(
    int unit, 
    bcm_fcoe_zone_entry_t *zone);

/* Delete FCOE zoning entry. */
extern int bcm_fcoe_zone_delete(
    int unit, 
    bcm_fcoe_zone_entry_t *zone);

/* Delete FCOE zoning entry by Source ID. */
extern int bcm_fcoe_zone_delete_by_sid(
    int unit, 
    bcm_fcoe_zone_entry_t *zone);

/* Delete FCOE zoning entry by Destination ID. */
extern int bcm_fcoe_zone_delete_by_did(
    int unit, 
    bcm_fcoe_zone_entry_t *zone);

/* Delete all FCOE zoning entry. */
extern int bcm_fcoe_zone_delete_all(
    int unit);

/* Get FCOE zoning entry. */
extern int bcm_fcoe_zone_get(
    int unit, 
    bcm_fcoe_zone_entry_t *zone);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_FCOE_FC_HEADER_TYPE_INGRESS 0x00000001 /* Ingress header type */
#define BCM_FCOE_FC_HEADER_TYPE_EGRESS  0x00000002 /* Egress header type */

/* Header types for bcm_fcoe_fc_header_type_set/get */
typedef enum bcm_fcoe_fc_header_type_e {
    bcmFcoeFcHeaderTypeNone = 0,    /* No header */
    bcmFcoeFcHeaderTypeStd = 1,     /* Standard header */
    bcmFcoeFcHeaderTypeEncap = 2,   /* Encapsulated header */
    bcmFcoeFcHeaderTypeVft = 3,     /* VFT header */
    bcmFcoeFcHeaderTypeIfr = 4,     /* IFR Header */
    bcmFcoeFcHeaderTypeUnknown = 5, /* Unknown Header */
    bcmFcoeFcHeaderTypeCount = 6 
} bcm_fcoe_fc_header_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the header type that a given r_ctl should be parsed as. */
extern int bcm_fcoe_fc_header_type_set(
    int unit, 
    uint32 flags, 
    uint8 r_ctl, 
    bcm_fcoe_fc_header_type_t hdr_type);

/* Get the header type that a given r_ctl is set to be parsed as. */
extern int bcm_fcoe_fc_header_type_get(
    int unit, 
    uint32 flags, 
    uint8 r_ctl, 
    bcm_fcoe_fc_header_type_t *hdr_type);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FCOE statistics maintained per VSAN. */
typedef enum bcm_fcoe_vsan_stat_e {
    bcmFcoeVsanInPackets = 0,   /* Packets that ingress on fcoe vsan */
    bcmFcoeVsanInBytes = 1      /* Bytes that ingress on fcoe vsan */
} bcm_fcoe_vsan_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Attach a flex counter ID to FCOE VSAN ID. */
extern int bcm_fcoe_vsan_stat_attach(
    int unit, 
    bcm_fcoe_vsan_id_t vsan, 
    uint32 stat_counter_id);

/* Detach a flex counter ID from FCOE VSAN ID. */
extern int bcm_fcoe_vsan_stat_detach(
    int unit, 
    bcm_fcoe_vsan_id_t vsan);

/* Get flex counter value for FCOE VSAN ID. */
extern int bcm_fcoe_vsan_stat_counter_get(
    int unit, 
    bcm_fcoe_vsan_id_t vsan, 
    bcm_fcoe_vsan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve flex counter
 * value for FCOE VSAN ID.
 */
extern int bcm_fcoe_vsan_stat_counter_sync_get(
    int unit, 
    bcm_fcoe_vsan_id_t vsan, 
    bcm_fcoe_vsan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set flex counter value for FCOE VSAN ID. */
extern int bcm_fcoe_vsan_stat_counter_set(
    int unit, 
    bcm_fcoe_vsan_id_t vsan, 
    bcm_fcoe_vsan_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get stat counter ID for FCOE VSAN ID. */
extern int bcm_fcoe_vsan_stat_id_get(
    int unit, 
    bcm_fcoe_vsan_id_t vsan, 
    bcm_fcoe_vsan_stat_t stat, 
    uint32 *stat_counter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FCOE statistics maintained per route. */
typedef enum bcm_fcoe_route_stat_e {
    bcmFcoeRouteInPackets = 0,  /* Packets that ingress on fcoe route */
    bcmFcoeRouteInBytes = 1     /* Bytes that ingress on fcoe route */
} bcm_fcoe_route_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Attach a flex counter ID to FCOE route. */
extern int bcm_fcoe_route_stat_attach(
    int unit, 
    bcm_fcoe_route_t *route, 
    uint32 stat_counter_id);

/* Detach flex counter ID from FCOE route. */
extern int bcm_fcoe_route_stat_detach(
    int unit, 
    bcm_fcoe_route_t *route);

/* Get flex counter value for FCOE route. */
extern int bcm_fcoe_route_stat_counter_get(
    int unit, 
    bcm_fcoe_route_t *route, 
    bcm_fcoe_route_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve flex counter
 * value for FCOE route.
 */
extern int bcm_fcoe_route_stat_counter_sync_get(
    int unit, 
    bcm_fcoe_route_t *route, 
    bcm_fcoe_route_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set flex counter value for FCOE route. */
extern int bcm_fcoe_route_stat_counter_set(
    int unit, 
    bcm_fcoe_route_t *route, 
    bcm_fcoe_route_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get stat counter ID for FCOE route. */
extern int bcm_fcoe_route_stat_id_get(
    int unit, 
    bcm_fcoe_route_t *route, 
    bcm_fcoe_route_stat_t stat, 
    uint32 *stat_counter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FCOE VSAN VFT object */
typedef struct bcm_fcoe_vsan_vft_s {
    uint8 r_ctl; 
    uint8 version; 
    uint8 type; 
    uint8 hop_count; 
} bcm_fcoe_vsan_vft_t;

/* FCOE Interface VSAN ID source */
typedef enum bcm_fcoe_intf_vsan_id_source_e {
    bcmFcoeIntfVsanSet = 0,         /* Get VSAN ID from table entry */
    bcmFcoeIntfVsanInternal = 1,    /* Get VSAN ID from internal VSAN ID */
    bcmFcoeIntfVsanInnerVlan = 2,   /* Get VSAN ID from internal ITAG */
    bcmFcoeIntfVsanOuterVlan = 3    /* Get VSAN ID from internal OVID */
} bcm_fcoe_intf_vsan_id_source_t;

/* FCOE Interface VSAN Priority source */
typedef enum bcm_fcoe_intf_vsan_pri_source_e {
    bcmFcoeIntfVsanPriSet = 0,      /* Use VSAN PRI from table entry */
    bcmFcoeIntfVsanPriInternal = 1, /* Get VSAN PRI from internal VSAN PRI */
    bcmFcoeIntfVsanPriRemark = 2    /* Get VSAN PRI from internal PRI */
} bcm_fcoe_intf_vsan_pri_source_t;

/* FCOE interface configuration parameters */
typedef struct bcm_fcoe_intf_config_s {
    uint32 flags; 
    int qos_map_id; 
    bcm_fcoe_vsan_id_t vsan; 
    uint8 vsan_pri; 
    bcm_fcoe_vsan_vft_t vft; 
    bcm_fcoe_intf_vsan_id_source_t vsan_source; 
    bcm_fcoe_intf_vsan_pri_source_t vsan_pri_source; 
} bcm_fcoe_intf_config_t;

#define BCM_FCOE_INTF_VFT_DO_NOT_MODIFY 0x00000001 /* Do not modify any existing
                                                      VFT header */
#define BCM_FCOE_INTF_VFT_ADD_REPLACE   0x00000002 /* Add or replace VFT header */
#define BCM_FCOE_INTF_VFT_DELETE        0x00000004 /* Delete VFT header if
                                                      present */

#ifndef BCM_HIDE_DISPATCHABLE

/* Change settings for FCOE specific properties of a L3 interface. */
extern int bcm_fcoe_intf_config_set(
    int unit, 
    uint32 flags, 
    bcm_if_t intf, 
    bcm_fcoe_intf_config_t *cfg);

/* Retrieve settings for FCOE specific properties of a L3 interface. */
extern int bcm_fcoe_intf_config_get(
    int unit, 
    uint32 flags, 
    bcm_if_t intf, 
    bcm_fcoe_intf_config_t *cfg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* FCOE VSAN Translation actions */
typedef enum bcm_fcoe_vsan_action_e {
    bcmFcoeVsanActionNone = 0,      /* Do not modify */
    bcmFcoeVsanActionInnerVlan = 1, /* Use Inner Vlan's value */
    bcmFcoeVsanActionOuterVlan = 2, /* Use Outer Vlan's value */
    bcmFcoeVsanActionReplace = 3,   /* Replace with specified value */
    bcmFcoeVsanActionDelete = 4,    /* Delete */
    bcmFcoeVsanActionInternal = 5,  /* Replace with Internal value */
    bcmFcoeVsanActionCount = 6 
} bcm_fcoe_vsan_action_t;

/* FCOE VSAN Translate Key types */
typedef enum bcm_fcoe_vsan_translate_key_e {
    bcmFcoeVsanTranslateKeyInvalid = -1, /* Invalid Key Type */
    bcmFcoeVsanTranslateKeyDoubleVsan = 0, /* Use I-VID[11:0],O-VID[11:0] and
                                           VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyInnerVsan = 1, /* Use I-VID[11:0] and VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyOuterVsan = 2, /* Use O-VID[11:0] and VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyDouble = 3,  /* Use O-VID[11:0] and I-VID[11:0] */
    bcmFcoeVsanTranslateKeyOuter = 4,   /* Use O-VID[11:0] */
    bcmFcoeVsanTranslateKeyInner = 5,   /* Use I-VID[11:0] */
    bcmFcoeVsanTranslateKeyOuterTag = 6, /* Use O-TAG[15:0] */
    bcmFcoeVsanTranslateKeyInnerTag = 7, /* Use I-TAG[15:0] */
    bcmFcoeVsanTranslateKeyOuterPri = 8, /* Use (VLAN-PRI, VLAN-CFI of O-TAG) */
    bcmFcoeVsanTranslateKeyPortDouble = 9, /* Use Port, O-VID[11:0] and I-VID[11:0] */
    bcmFcoeVsanTranslateKeyPortOuter = 10, /* Use Port, O-VID[11:0] */
    bcmFcoeVsanTranslateKeyPortInner = 11, /* Use Port, I-VID[11:0] */
    bcmFcoeVsanTranslateKeyPortOuterTag = 12, /* Use Port, O-TAG[15:0] */
    bcmFcoeVsanTranslateKeyPortInnerTag = 13, /* Use Port, I-TAG[15:0] */
    bcmFcoeVsanTranslateKeyPortOuterPri = 14, /* Use Port, (VLAN-PRI, VLAN-CFI of
                                           O-TAG) */
    bcmFcoeVsanTranslateKeyPortDoubleVsan = 15, /* Use Port, I-VID[11:0],O-VID[11:0] and
                                           VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyPortInnerVsan = 16, /* Use Port, I-VID[11:0] and
                                           VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyPortOuterVsan = 17 /* Use Port, O-VID[11:0] and
                                           VSAN-ID[11:0] */
} bcm_fcoe_vsan_translate_key_t;

/* FCOE VSAN Translate Key types */
typedef enum bcm_fcoe_vsan_translate_key_egress_e {
    bcmFcoeVsanTranslateKeyEgressInvalid = -1, /* Invalid Key Type */
    bcmFcoeVsanTranslateKeyEgressDoubleVsan = 0, /* Use I-VID[11:0],O-VID[11:0] and
                                           VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyEgressVsan = 1, /* Use VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyEgressInnerVsan = 2, /* Use I-VID[11:0] and VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyEgressOuterVsan = 3, /* Use O-VID[11:0] and VSAN-ID[11:0] */
    bcmFcoeVsanTranslateKeyEgressDouble = 4, /* Use O-VID[11:0] and I-VID[11:0] */
    bcmFcoeVsanTranslateKeyEgressInner = 5, /* Use I-VID[11:0] */
    bcmFcoeVsanTranslateKeyEgressOuter = 6 /* Use O-VID[11:0] */
} bcm_fcoe_vsan_translate_key_egress_t;

/* Translate Key Configuration. */
typedef struct bcm_fcoe_vsan_translate_key_config_s {
    uint32 flags; 
    bcm_fcoe_vsan_translate_key_t key_type; 
    bcm_vlan_t inner_vlan;              /* Inner Vlan ID to match on */
    bcm_vlan_t outer_vlan;              /* Outer Vlan ID to match on */
    bcm_fcoe_vsan_id_t vsan;            /* VFT VSAN ID to match on */
    bcm_gport_t port;                   /* Port to match on */
} bcm_fcoe_vsan_translate_key_config_t;

#define BCM_FCOE_VSAN_ACTION_INGRESS    0x00000001 /* Translate at ingress */
#define BCM_FCOE_VSAN_ACTION_EGRESS     0x00000002 /* Translate at egress */
#define BCM_FCOE_VSAN_ACTION_VFT_NONE   0x00000004 /* Indicates no VFT tag is
                                                      present */

/* VLAN/VSAN Translation Action Set. */
typedef struct bcm_fcoe_vsan_action_set_s {
    uint32 flags;                       /* BCM_FCOE_VSAN_ACTION_* */
    bcm_fcoe_vsan_id_t new_vsan;        /* new VSAN ID to replace with */
    uint8 new_vsan_pri;                 /* new VSAN priority to replace with */
    bcm_fcoe_vsan_vft_t vft;            /* remaining fields of VFT header */
    bcm_if_t ingress_if;                /* TBD */
    bcm_fcoe_vsan_action_t dt_vsan;     /* Vsan action when double tagged */
    bcm_fcoe_vsan_action_t dt_vsan_pri; /* VSAN Priority action when double
                                           tagged */
    bcm_fcoe_vsan_action_t it_vsan;     /* Vsan action when single inner tagged */
    bcm_fcoe_vsan_action_t it_vsan_pri; /* Vsan Priority action when single
                                           inner tagged */
    bcm_fcoe_vsan_action_t ot_vsan;     /* Vsan action when single outer tagged */
    bcm_fcoe_vsan_action_t ot_vsan_pri; /* Vsan Priority action when single
                                           outer tagged */
    bcm_fcoe_vsan_action_t ut_vsan;     /* Vsan action when un-tagged */
    bcm_fcoe_vsan_action_t ut_vsan_pri; /* Vsan Priority action when un-tagged */
    bcm_vlan_action_set_t vlan_action;  /* VLAN actions */
} bcm_fcoe_vsan_action_set_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Add FCOE VSAN and VLAN translation action. */
extern int bcm_fcoe_vsan_translate_action_add(
    int unit, 
    bcm_fcoe_vsan_translate_key_config_t *key, 
    bcm_fcoe_vsan_action_set_t *action);

/* Delete FCOE VSAN and VLAN translation action. */
extern int bcm_fcoe_vsan_translate_action_delete(
    int unit, 
    bcm_fcoe_vsan_translate_key_config_t *key);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize bcm_fcoe_intf_config_t structure. */
extern void bcm_fcoe_intf_config_t_init(
    bcm_fcoe_intf_config_t *intf);

/* Initialize bcm_fcoe_vsan_vft_t structure. */
extern void bcm_fcoe_vsan_vft_t_init(
    bcm_fcoe_vsan_vft_t *vft);

/* Initialize bcm_fcoe_vsan_translate_key_config_t structure. */
extern void bcm_fcoe_vsan_translate_key_config_t_init(
    bcm_fcoe_vsan_translate_key_config_t *key_config);

/* Initialize bcm_fcoe_vsan_action_set_t structure. */
extern void bcm_fcoe_vsan_action_set_t_init(
    bcm_fcoe_vsan_action_set_t *action_set);

/* bcm_fcoe_zone_traverse_cb */
typedef int (*bcm_fcoe_zone_traverse_cb)(
    int unit, 
    bcm_fcoe_zone_entry_t *zone, 
    void *user_data);

/* bcm_fcoe_vsan_traverse_cb */
typedef int (*bcm_fcoe_vsan_traverse_cb)(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id, 
    bcm_fcoe_vsan_t *vsan, 
    void *user_data);

/* bcm_fcoe_route_traverse_cb */
typedef int (*bcm_fcoe_route_traverse_cb)(
    int unit, 
    bcm_fcoe_route_t *route, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse through the FCOE zone table and run callback at each zone. */
extern int bcm_fcoe_zone_traverse(
    int unit, 
    uint32 flags, 
    bcm_fcoe_zone_traverse_cb trav_fn, 
    void *user_data);

/* Traverse through the FCOE vsan table and run callback at each vsan. */
extern int bcm_fcoe_vsan_traverse(
    int unit, 
    uint32 flags, 
    bcm_fcoe_vsan_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Traverse through the FCOE routing table and run callback at each
 * route.
 */
extern int bcm_fcoe_route_traverse(
    int unit, 
    uint32 flags, 
    bcm_fcoe_route_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_FCOE_H__ */
