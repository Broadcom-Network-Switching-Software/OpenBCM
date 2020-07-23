/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_L2_H__
#define __BCM_L2_H__

#include <bcm/types.h>

/* 
 * Flags for device-independent L2 address.
 * 
 * Special note on insert/delete/lookup-specific flags:
 * 
 *   BCM_L2_NATIVE
 *   BCM_L2_MOVE
 *   BCM_L2_FROM_NATIVE
 *   BCM_L2_TO_NATIVE
 *   BCM_L2_MOVE_PORT
 *   BCM_L2_LOCAL_CPU
 * 
 * On a move, two calls occur: delete and insert. The from native/to
 * native calls are set the same for both of these operations. On an age,
 * the move bit is not set and only one delete operation occurs.
 * 
 * Suggested application operation:
 * 
 *      Insert:  If native is set, send insert op to all units.
 *      Age:     If native is set, send delete operation to all units.
 *      Move:    Ignore delete indication and wait for insert operation.
 *               Treat insert like above.
 * 
 * That is, ignore move-delete operations; only forward other
 * operations if native.
 * 
 * The BCM_L2_LOCAL_CPU flag indicates the entry is for the local CPU on
 * the device. This is valid for _add operations only. Note that
 * BCM_L2_LOCAL_CPU is related to the L2_NATIVE value. L2_NATIVE is valid
 * on reads, L2_LOCAL_CPU is valid on move or write.
 */
#define BCM_L2_COS_SRC_PRI              0x00000001 /* Source COS has priority
                                                      over destination COS. */
#define BCM_L2_DISCARD_SRC              0x00000002 
#define BCM_L2_DISCARD_DST              0x00000004 
#define BCM_L2_COPY_TO_CPU              0x00000008 
#define BCM_L2_L3LOOKUP                 0x00000010 
#define BCM_L2_STATIC                   0x00000020 
#define BCM_L2_HIT                      0x00000040 
#define BCM_L2_TRUNK_MEMBER             0x00000080 
#define BCM_L2_MCAST                    0x00000100 
#define BCM_L2_REPLACE_DYNAMIC          0x00000200 
#define BCM_L2_SRC_HIT                  0x00000400 
#define BCM_L2_DES_HIT                  0x00000800 
#define BCM_L2_REMOTE_TRUNK             0x00001000 
#define BCM_L2_MIRROR                   0x00002000 
#define BCM_L2_SETPRI                   0x00004000 
#define BCM_L2_REMOTE_LOOKUP            0x00008000 
#define BCM_L2_NATIVE                   0x00010000 
#define BCM_L2_MOVE                     0x00020000 
#define BCM_L2_FROM_NATIVE              0x00040000 
#define BCM_L2_TO_NATIVE                0x00080000 
#define BCM_L2_MOVE_PORT                0x00100000 
#define BCM_L2_LOCAL_CPU                0x00200000 /* Entry is for the local CPU
                                                      on the device. */
#define BCM_L2_USE_FABRIC_DISTRIBUTION  0x00400000 /* Use specified fabric
                                                      distribution class. */
#define BCM_L2_PENDING                  0x00800000 
#define BCM_L2_LEARN_LIMIT_EXEMPT       0x01000000 
#define BCM_L2_LEARN_LIMIT              0x02000000 
#define BCM_L2_ENTRY_OVERFLOW           0x04000000 
#define BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL 0x08000000 /* Only system wide MAC limit
                                                      counter will be
                                                      incremented. */
#define BCM_L2_SET_ENCAP_VALID          0x10000000 /* DNX only: indication that
                                                      encap_id_valid needs to
                                                      set even when encap_id is
                                                      not valid. */
#define BCM_L2_SET_ENCAP_INVALID        0x20000000 /* DNX only: encap_id is
                                                      added to MAC table as is,
                                                      however valid bit is
                                                      cleared. */
#define BCM_L2_SR_SAN_DEST              0x40000000 /* Seamless Redundancy: the
                                                      destination is a SAN */
#define BCM_L2_ADD_OVERRIDE_PENDING     0x80000000 /* Override pending entry
                                                      while the same hash bucket
                                                      is full. */

/* These are in bit order. */
#define BCM_L2_FLAGS_STR        \
{ \
    "SrcCOS", \
    "SrcDisc", \
    "DestDisc", \
    "CopyCPU", \
    "L3Lkup", \
    "Static", \
    "Hit", \
    "Trunk", \
    "Mcast", \
    "RplDyn", \
    "SrcHit", \
    "DestHit", \
    "RTrunk", \
    "Mirror", \
    "SetPri", \
    "RemoteLookup", \
    "Native", \
    "Move", \
    "FrNative", \
    "ToNative", \
    "MovePort", \
    "LclCPU", \
    "UseFabricDistribution", \
    "Pending", \
    "LearnLimitExempt", \
    "LearnLimit" \
} 

/* bcm_l2_flags_str */
extern char *bcm_l2_flags_str[];

/* bcm_l2_flags_count */
extern int bcm_l2_flags_count;

/* Flags2 for device-independent L2 address. */
#define BCM_L2_FLAGS2_ROE_NHI           0x00000001 /* The destination of the l2
                                                      address is ROE/ROE Custom
                                                      Next Hop Index. */
#define BCM_L2_FLAGS2_L3_MULTICAST      0x00000002 /* The destination of the l2
                                                      address is ROE/ROE Custom
                                                      IPMC Index. */
#define BCM_L2_FLAGS2_BMACT_LEARN       0x00000004 /* Mac-in-Mac: BVID entries
                                                      will be added to dedicated
                                                      MiM learn table (instead
                                                      of BMACT FWD). */
#define BCM_L2_FLAGS2_ADD_NO_CALLBACKS  0x00000008 /* The l2 FIFO events will
                                                      not be generated if this
                                                      flag is set. */
#define BCM_L2_FLAGS2_ADD_VPN_TYPE      0x00000010 /* Add VPN type l2 entry. */
#define BCM_L2_FLAGS2_HIT_GET           0x00000020 /* Signal bcm_l2_addr_get to
                                                      return the hit status of
                                                      the MAC entry. */
#define BCM_L2_FLAGS2_AGE_GET           0x00000040 /* Signal bcm_l2_addr_get to
                                                      return the age of the MAC
                                                      entry. */
#define BCM_L2_FLAGS2_MESH              0x00000080 /* The L2 address is mesh bit
                                                      set. */

#define BCM_L2_AUTH_NONE            0          
#define BCM_L2_AUTH_STATIC_ACCEPT   4          
#define BCM_L2_AUTH_STATIC_REJECT   5          
#define BCM_L2_AUTH_SA_NUM          6          
#define BCM_L2_AUTH_SA_MATCH        7          

#define BCM_L2_VID_MASK_ALL     0x0fff     
#define BCM_L2_SRCPORT_MASK_ALL 0x3f       

#define BCM_L2_CALLBACK_DELETE          0          
#define BCM_L2_CALLBACK_ADD             1          
#define BCM_L2_CALLBACK_REPORT          2          
#define BCM_L2_CALLBACK_LEARN_EVENT     3          
#define BCM_L2_CALLBACK_AGE_EVENT       4          
#define BCM_L2_CALLBACK_MOVE_EVENT      5          
#define BCM_L2_CALLBACK_REFRESH_EVENT   6          

#define BCM_L2_ADDR_DIST_LEARN_EVENT        0x00000001 /* learning events */
#define BCM_L2_ADDR_DIST_STATION_MOVE_EVENT 0x00000002 /* station movement event */
#define BCM_L2_ADDR_DIST_AGED_OUT_EVENT     0x00000004 /* aged out events */
#define BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER 0x00000008 /* Send events to the
                                                          learning distributer */
#define BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER 0x00000010 /* Send events to the
                                                          shadow distributer */
#define BCM_L2_ADDR_DIST_REFRESH_EVENT      0x00000020 /* Age refresh event */
#define BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER 0x00000040 /* Do not send events to
                                                          any distributer */
#define BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER 0x00000080 /* Send events to the DMA
                                                          that copies them to
                                                          the CPU's memory */
#define BCM_L2_ADDR_DIST_ACK_EVENT          0x00000100 /* acknowledge events */
#define BCM_L2_ADDR_DIST_UPDATE_WITHOUT_ADD_EVENT 0x00000200 /* Update payload or age
                                                          state or fail in case
                                                          the entry doesn't
                                                          exist */
#define BCM_L2_ADDR_DIST_GPORT              0x00000400 /* Set distibution events
                                                          per LIF */

#define BCM_L2_LEARN_MSG_DEST_MULTICAST 0x00000001 /* The destination of the
                                                      learning message is
                                                      Multicast group
                                                      'dest_group' in use, if
                                                      not present then the
                                                      destination is uc and
                                                      'dest_port' in use
                                                      dst_mac_addr ,
                                                      src_mac_addr, ether_type,
                                                      vlan_prio, vlan. */
#define BCM_L2_LEARN_MSG_ETH_ENCAP      0x00000002 /* Learn message are
                                                      encapsulated with
                                                      Etherenet header. Use the
                                                      specified Ethernet
                                                      parameters */
#define BCM_L2_LEARN_MSG_LEARNING       0x00000004 /* The specified
                                                      configuration are for
                                                      message generated to
                                                      learning */
#define BCM_L2_LEARN_MSG_SHADOW         0x00000008 /* The specified
                                                      configuration are for
                                                      message generated to
                                                      manage a shadow of the ARL */

/* Device-independent L2 address structure. */
typedef struct bcm_l2_addr_s {
    uint32 flags;                       /* BCM_L2_xxx flags. */
    uint32 flags2;                      /* BCM_L2_FLAGS2_xxx flags. */
    uint32 station_flags;               /* BCM_L2_STATION_xxx flags. */
    bcm_mac_t mac;                      /* 802.3 MAC address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    int port;                           /* Zero-based port number. */
    int modid;                          /* XGS: modid. */
    bcm_trunk_t tgid;                   /* Trunk group ID. */
    bcm_cos_t cos_dst;                  /* COS based on dst addr. */
    bcm_cos_t cos_src;                  /* COS based on src addr. */
    bcm_multicast_t l2mc_group;         /* XGS: index in L2MC table */
    bcm_if_t egress_if;                 /* XGS: index in Next Hop Tables. Used
                                           it with BCM_L2_FLAGS2_ROE_NHI flag */
    bcm_multicast_t l3mc_group;         /* XGS: index in L3_IPMC table. Use it
                                           with BCM_L2_FLAGS2_L3_MULTICAST. */
    bcm_pbmp_t block_bitmap;            /* XGS: blocked egress bitmap. */
    int auth;                           /* Used if auth enabled on port. */
    int group;                          /* Group number for FP. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    int encap_id;                       /* out logical interface */
    int age_state;                      /* Age state of the entry */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    bcm_pbmp_t sa_source_filter_pbmp;   /* Source port filter bitmap for this SA */
    bcm_tsn_flowset_t tsn_flowset;      /* Time-Sensitive Networking: associated
                                           flow set */
    bcm_tsn_sr_flowset_t sr_flowset;    /* Seamless Redundancy: associated flow
                                           set */
    bcm_policer_t policer_id;           /* Base policer to be used */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
    uint32 stat_id;                     /* Object statistics ID */
    int stat_pp_profile;                /* Statistics profile */
    uint16 gbp_src_id;                  /* GBP Source ID */
    int opaque_ctrl_id;                 /* Opaque control ID. */
} bcm_l2_addr_t;

/* Flags for device-independent L2 egress. */
#define BCM_L2_EGRESS_WITH_ID               0x00000001 /* Use the specified
                                                          Encap ID while
                                                          creating. */
#define BCM_L2_EGRESS_REPLACE               0x00000002 /* Replace config of
                                                          existing egress
                                                          object. */
#define BCM_L2_EGRESS_DEST_MAC_REPLACE      0x00000004 /* Replace dest MAC. */
#define BCM_L2_EGRESS_DEST_MAC_PREFIX5_REPLACE 0x00000008 /* Replace only 5 prefix
                                                          bytes of dest MAC. */
#define BCM_L2_EGRESS_SRC_MAC_REPLACE       0x00000010 /* Replace source MAC. */
#define BCM_L2_EGRESS_OUTER_VLAN_REPLACE    0x00000020 /* Replace outer vlan. */
#define BCM_L2_EGRESS_OUTER_PRIO_REPLACE    0x00000040 /* Replace outer tag
                                                          priority. */
#define BCM_L2_EGRESS_INNER_VLAN_REPLACE    0x00000080 /* Replace inner vlan. */
#define BCM_L2_EGRESS_INNER_PRIO_REPLACE    0x00000100 /* Replace inner tag
                                                          priority. */
#define BCM_L2_EGRESS_ETHERTYPE_REPLACE     0x00000200 /* Replace EtherType */
#define BCM_L2_EGRESS_DEST_PORT             0x00000400 /* Associate l2 egress
                                                          entry with a
                                                          particular dest port */
#define BCM_L2_EGRESS_OUTER_VLAN_ADD        0x00000800 /* add outer vlan, if set
                                                          then new vlan-tag is
                                                          added according to
                                                          outer_tpid,outer_prio,outer_vlan,
                                                          attributes */
#define BCM_L2_EGRESS_VLAN_QOS_MAP_REPLACE  0x00001000 /* Replace VLAN PCP-DEI
                                                          QOS settings */
#define BCM_L2_EGRESS_WIDE                  0x00002000 /* Set wide entry in
                                                          egress */
#define BCM_L2_EGRESS_RECYCLE_HEADER        0x00004000 /* Recycle header */
#define BCM_L2_EGRESS_DEST_ENCAP_ID         0x00008000 /* Set destination
                                                          encap_id */

/* L2 Egress recycle Apps. */
typedef enum bcm_l2_egress_recycle_app_e {
    bcmL2EgressRecycleAppDropAndContinue = 0, 
    bcmL2EgressRecycleAppExtendedTerm = 1, 
    bcmL2EgressRecycleAppExtendedEncap = 2, 
    bcmL2EgressRecycleAppReflector = 3, 
    bcmL2EgressRecycleAppRedirectVrf = 4, 
    bcmL2EgressRecycleAppCount = 5 
} bcm_l2_egress_recycle_app_t;

/* Device-independent L2 egress structure. */
typedef struct bcm_l2_egress_s {
    uint32 flags;                       /* BCM_L2_EGRESS_XXX flags. */
    bcm_mac_t dest_mac;                 /* Destination MAC address to replace
                                           with. */
    bcm_mac_t src_mac;                  /* Source MAC address to replace with. */
    bcm_vlan_t outer_vlan;              /* outer vlan tag to replace with. */
    bcm_vlan_t inner_vlan;              /* inner vlan tag to replace with. */
    bcm_ethertype_t ethertype;          /* EtherType to replace with. */
    bcm_gport_t dest_port;              /* Destination gport to associate this
                                           l2 egress entry with */
    bcm_if_t encap_id;                  /* Encapsulation index */
    uint16 outer_tpid;                  /* outer vlan tag TPID */
    uint8 outer_prio;                   /* outer vlan tag User Priority */
    uint16 inner_tpid;                  /* inner vlan tag TPID */
    bcm_if_t l3_intf;                   /* L3 interface (tunnel). Used only in
                                           Native Routing overlay protocols. */
    int vlan_qos_map_id;                /* VLAN PCP-DEI QoS settings. Used only
                                           in Native Routing overlay protocols. */
    int mpls_extended_label_value;      /* An extended label placed on top of a
                                           an egress encapsulation. */
    bcm_gport_t vlan_port_id;           /* vlan gport identifier */
    int recycle_header_extension_length; /* Specify in bytes the size of
                                           extension header after recycle header */
    int additional_egress_termination_size; /* Specify in bytes the size of egress
                                           additional termination */
    bcm_l2_egress_recycle_app_t recycle_app; /* Indicates the recycle app */
    bcm_if_t dest_encap_id;             /* Destination encapsulation index */
    bcm_vrf_t vrf;                      /* Virtual router instance */
} bcm_l2_egress_t;

/* Flags for device-independent L2 cache address. */
#define BCM_L2_CACHE_CPU            0x00000001 /* Packet is copied to CPU. */
#define BCM_L2_CACHE_DISCARD        0x00000002 /* Packet is not switched. */
#define BCM_L2_CACHE_MIRROR         0x00000004 /* Packet is mirrored. */
#define BCM_L2_CACHE_L3             0x00000008 /* Packet is to be L3 routed. */
#define BCM_L2_CACHE_BPDU           0x00000010 /* Packet is BPDU. */
#define BCM_L2_CACHE_SETPRI         0x00000020 /* Internal prio from prio field. */
#define BCM_L2_CACHE_TRUNK          0x00000040 /* Destination is a trunk. */
#define BCM_L2_CACHE_REMOTE_LOOKUP  0x00000080 /* Remote L2 lookup requested. */
#define BCM_L2_CACHE_LEARN_DISABLE  0x00000100 /* Packet source address is not
                                                  learned for this destination
                                                  address. */
#define BCM_L2_CACHE_TUNNEL         0x00000200 /* Tunnel termination address. */
#define BCM_L2_CACHE_DESTPORTS      0x00000400 /* Packet is forwarded by
                                                  multiport L2 address. */
#define BCM_L2_CACHE_SUBTYPE        0x00000800 /* Slow protocol subtype to
                                                  match. */
#define BCM_L2_CACHE_LOOKUP         0x00001000 /* L2 lookup requested. */
#define BCM_L2_CACHE_MULTICAST      0x00002000 /* Destination is (flood)
                                                  multicast group. */
#define BCM_L2_CACHE_PROTO_PKT      0x00004000 /* Packet is protocol packets. */

/* Device-independent L2 cache address structure. */
typedef struct bcm_l2_cache_addr_s {
    uint32 flags;                   /* BCM_L2_CACHE_xxx flags. */
    uint32 station_flags;           /* BCM_L2_STATION_xxx flags. */
    bcm_mac_t mac;                  /* Destination MAC address to match. */
    bcm_mac_t mac_mask;             /* MAC address mask. */
    bcm_vlan_t vlan;                /* VLAN to match. */
    bcm_vlan_t vlan_mask;           /* VLAN mask. */
    bcm_port_t src_port;            /* Ingress port to match (BCM5660x). */
    bcm_port_t src_port_mask;       /* Ingress port mask (must be 0 if not
                                       BCM5660x). */
    bcm_module_t dest_modid;        /* Switch destination module ID. */
    bcm_port_t dest_port;           /* Switch destination port. */
    bcm_trunk_t dest_trunk;         /* Switch destination trunk ID. */
    int prio;                       /* Internal priority, use -1 to not set. */
    bcm_pbmp_t dest_ports;          /* Destination ports for Multiport L2
                                       address forwarding. */
    int lookup_class;               /* Classification class ID. */
    uint8 subtype;                  /* Slow protocol subtype to match. */
    bcm_if_t encap_id;              /* Encapsulation index. */
    bcm_multicast_t group;          /* Flood domain for L2CP. */
    bcm_ethertype_t ethertype;      /* EtherType to match. */
    bcm_ethertype_t ethertype_mask; /* Mask. */
} bcm_l2_cache_addr_t;

/* Flags for L2 learn limit. */
#define BCM_L2_LEARN_LIMIT_SYSTEM           0x00000001 /* Limit is system wide. */
#define BCM_L2_LEARN_LIMIT_VLAN             0x00000002 /* Limit is on per VLAN
                                                          basis. */
#define BCM_L2_LEARN_LIMIT_PORT             0x00000004 /* Limit is on per port
                                                          basis. */
#define BCM_L2_LEARN_LIMIT_TRUNK            0x00000008 /* Limit is on per trunk
                                                          basis. */
#define BCM_L2_LEARN_LIMIT_ACTION_DROP      0x00000010 /* Drop if over limit. */
#define BCM_L2_LEARN_LIMIT_ACTION_CPU       0x00000020 /* Send to CPU if over
                                                          limit. */
#define BCM_L2_LEARN_LIMIT_ACTION_PREFER    0x00000040 /* Use system drop/CPU if
                                                          over both system limit
                                                          and non-system limit,
                                                          used by system wide
                                                          setting only. */
#define BCM_L2_LEARN_LIMIT_TUNNEL           0x00000080 /* Limit is on per tunnel
                                                          basis. */

/* L2 learn limit structure. */
typedef struct bcm_l2_learn_limit_s {
    uint32 flags;               /* BCM_L2_LEARN_LIMIT_xxx actions and
                                   qualifiers. */
    bcm_vlan_t vlan;            /* VLAN identifier. */
    bcm_port_t port;            /* Port number. */
    bcm_trunk_t trunk;          /* Trunk identifier. */
    bcm_tunnel_id_t tunnel_id;  /* Tunnel identifier. */
    int limit;                  /* Maximum number of learned entries, -1 for
                                   unlimited. */
} bcm_l2_learn_limit_t;

/* L2 learn message distribution info. */
typedef struct bcm_l2_learn_msgs_config_s {
    uint32 flags;               /* BCM_L2_LEARN_MSG_XXX flags. */
    bcm_gport_t dest_port;      /* Destination port. */
    bcm_multicast_t dest_group; /* Destination group. */
    bcm_vlan_t vlan;            /* L2: vlan identifier. set to invalid for
                                   untagged Ethernet */
    uint16 tpid;                /* L2: vlan TPID. */
    uint16 vlan_prio;           /* L2: vlan priority. */
    uint16 ether_type;          /* L2: type in Ethernet II frame or 802.1Q tag. */
    bcm_mac_t src_mac_addr;     /* L2: source MAC address. */
    bcm_mac_t dst_mac_addr;     /* L2: destination MAC address. */
    uint8 priority;             /* L2: traffic class, Range: 0..7 */
    uint8 color;                /* L2: drop precedence, Range: 0..3 */
} bcm_l2_learn_msgs_config_t;

/* Learning events distibution. */
typedef struct bcm_l2_addr_distribute_s {
    uint32 flags;       /* BCM_L2_ADDR_DIST_XXX flags. */
    bcm_vlan_t vid;     /* VLAN or VPN identifier. */
    bcm_gport_t gport;  /* GPORT identifier. */
} bcm_l2_addr_distribute_t;

/* L2 control information. */
typedef struct bcm_l2_gport_control_info_s {
    uint32 flags;       /* BCM_L2_GPORT_CONTROL_XXX flags. */
    bcm_gport_t gport;  /* GPORT identifier. */
    int aging_cycles;   /* number of aging meta-cycles. */
    int entropy_id;     /* aging profile ID. */
} bcm_l2_gport_control_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM L2 subsystem. */
extern int bcm_l2_init(
    int unit);

/* Clear the BCM L2 subsystem. */
extern int bcm_l2_clear(
    int unit);

/* Finalize the BCM L2 subsystem. */
extern int bcm_l2_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Initialize an L2 address structure to a specified MAC address and VLAN
 * ID.
 */
extern void bcm_l2_addr_t_init(
    bcm_l2_addr_t *l2addr, 
    const bcm_mac_t mac_addr, 
    bcm_vlan_t vid);

#define bcm_l2_addr_init        bcm_l2_addr_t_init 

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an L2 address entry to the specified device. */
extern int bcm_l2_addr_add(
    int unit, 
    bcm_l2_addr_t *l2addr);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_L2_DELETE_STATIC        0x00000001 
#define BCM_L2_DELETE_PENDING       0x00000002 
#define BCM_L2_DELETE_NO_CALLBACKS  0x00000004 
#define BCM_L2_DELETE_REMOTE_TRUNK  0x00000008 

#ifndef BCM_HIDE_DISPATCHABLE

/* Delete an L2 address entry from the specified device. */
extern int bcm_l2_addr_delete(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vid);

/* Delete L2 entries associated with a destination module/port. */
extern int bcm_l2_addr_delete_by_port(
    int unit, 
    bcm_module_t mod, 
    bcm_port_t port, 
    uint32 flags);

/* Delete L2 entries associated with a MAC address. */
extern int bcm_l2_addr_delete_by_mac(
    int unit, 
    bcm_mac_t mac, 
    uint32 flags);

/* Delete L2 entries associated with a VLAN. */
extern int bcm_l2_addr_delete_by_vlan(
    int unit, 
    bcm_vlan_t vid, 
    uint32 flags);

/* Delete L2 entries associated with a trunk. */
extern int bcm_l2_addr_delete_by_trunk(
    int unit, 
    bcm_trunk_t tid, 
    uint32 flags);

/* 
 * Delete L2 entries associated with a MAC address and a destination
 * module/port.
 */
extern int bcm_l2_addr_delete_by_mac_port(
    int unit, 
    bcm_mac_t mac, 
    bcm_module_t mod, 
    bcm_port_t port, 
    uint32 flags);

/* 
 * Delete L2 entries associated with a VLAN and a destination
 * module/port.
 */
extern int bcm_l2_addr_delete_by_vlan_port(
    int unit, 
    bcm_vlan_t vid, 
    bcm_module_t mod, 
    bcm_port_t port, 
    uint32 flags);

/* 
 * Delete L2 entries associated with a VLAN and a destination
 * module/port.
 */
extern int bcm_l2_addr_delete_by_vlan_trunk(
    int unit, 
    bcm_vlan_t vid, 
    bcm_trunk_t tid, 
    uint32 flags);

/* Check if an L2 entry is present in the L2 table. */
extern int bcm_l2_addr_get(
    int unit, 
    bcm_mac_t mac_addr, 
    bcm_vlan_t vid, 
    bcm_l2_addr_t *l2addr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Dump a hardware-independent L2 address. */
extern void bcm_l2_addr_dump(
    bcm_l2_addr_t *l2e);

#ifndef BCM_HIDE_DISPATCHABLE

/* Dump the key portion of a hardware-independent L2 address. */
extern int bcm_l2_key_dump(
    int unit, 
    char *pfx, 
    bcm_l2_addr_t *entry, 
    char *sfx);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Callback function used for receiving notification about insertions
 * into and deletions from the L2 table dynamically as they occur. Valid
 * operations are: delete, add, and report. A report with l2addr=NULL
 * indicates a scan completion of the L2 table.
 */
typedef void (*bcm_l2_addr_callback_t)(
    int unit, 
    bcm_l2_addr_t *l2addr, 
    int operation, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register/Unregister a callback routine for BCM L2 subsystem. */
extern int bcm_l2_addr_register(
    int unit, 
    bcm_l2_addr_callback_t callback, 
    void *userdata);

/* Register/Unregister a callback routine for BCM L2 subsystem. */
extern int bcm_l2_addr_unregister(
    int unit, 
    bcm_l2_addr_callback_t callback, 
    void *userdata);

/* Set/Get the age timer. */
extern int bcm_l2_age_timer_set(
    int unit, 
    int age_seconds);

/* Set/Get the age timer. */
extern int bcm_l2_age_timer_get(
    int unit, 
    int *age_seconds);

/* Set/Get the age timer. */
extern int bcm_l2_age_timer_meta_cycle_set(
    int unit, 
    int meta_cycle_time);

/* Set/Get the age timer. */
extern int bcm_l2_age_timer_meta_cycle_get(
    int unit, 
    int *meta_cycle_time);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an L2 learn message structure. */
extern void bcm_l2_learn_msgs_config_t_init(
    bcm_l2_learn_msgs_config_t *learn_msg_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set device to distribute learning message to other devices in system
 * and/or
 * monitoring CPU.
 */
extern int bcm_l2_learn_msgs_config_set(
    int unit, 
    bcm_l2_learn_msgs_config_t *learn_msg_config);

/* 
 * Get device configuration for distribution of learning messages to
 * other
 * devices in system and/or monitoring CPU.
 */
extern int bcm_l2_learn_msgs_config_get(
    int unit, 
    bcm_l2_learn_msgs_config_t *learn_msg_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an L2 learn distribution structure. */
extern void bcm_l2_addr_distribute_t_init(
    bcm_l2_addr_distribute_t *distribution);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Per VSI set which events (learn, station move (transplant), aged-out)
 * to inform CPU about.
 */
extern int bcm_l2_addr_msg_distribute_set(
    int unit, 
    bcm_l2_addr_distribute_t *distribution);

/* 
 * Per VSI, get which events (learn, station move (transplant), aged-out)
 * to inform CPU about.
 */
extern int bcm_l2_addr_msg_distribute_get(
    int unit, 
    bcm_l2_addr_distribute_t *distribution);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an L2 control information structure. */
extern void bcm_l2_gport_control_info_t_init(
    bcm_l2_gport_control_info_t *control_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Per LIF set l2 control infomration(aging profile ID, number of aging
 * meta-cycles).
 */
extern int bcm_l2_gport_control_info_set(
    int unit, 
    bcm_l2_gport_control_info_t *control_info);

/* 
 * Per LIF get l2 control infomration(aging profile ID, number of aging
 * meta-cycles).
 */
extern int bcm_l2_gport_control_info_get(
    int unit, 
    bcm_l2_gport_control_info_t *control_info);

/* Temporarily stop/restore L2 table from changing. */
extern int bcm_l2_addr_freeze(
    int unit);

/* Temporarily stop/restore L2 table from changing. */
extern int bcm_l2_addr_thaw(
    int unit);

/* 
 * Given an L2 address entry, return existing addresses which could
 * conflict.
 */
extern int bcm_l2_conflict_get(
    int unit, 
    bcm_l2_addr_t *addr, 
    bcm_l2_addr_t *cf_array, 
    int cf_max, 
    int *cf_count);

/* Determine if the given port is "native" from the point of view of L2. */
extern int bcm_l2_port_native(
    int unit, 
    int modid, 
    int port);

/* Initialize the L2 cache. */
extern int bcm_l2_cache_init(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an L2 cache address structure. */
extern void bcm_l2_cache_addr_t_init(
    bcm_l2_cache_addr_t *addr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get number of L2 cache entries. */
extern int bcm_l2_cache_size_get(
    int unit, 
    int *size);

/* Set an L2 cache entry. */
extern int bcm_l2_cache_set(
    int unit, 
    int index, 
    bcm_l2_cache_addr_t *addr, 
    int *index_used);

/* Get an L2 cache entry. */
extern int bcm_l2_cache_get(
    int unit, 
    int index, 
    bcm_l2_cache_addr_t *addr);

/* Clear an L2 cache entry. */
extern int bcm_l2_cache_delete(
    int unit, 
    int index);

/* Clear all L2 cache entries. */
extern int bcm_l2_cache_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the l2 egress structure */
extern void bcm_l2_egress_t_init(
    bcm_l2_egress_t *egr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an l2 egress entry. */
extern int bcm_l2_egress_create(
    int unit, 
    bcm_l2_egress_t *egr);

/* Destroy an l2 egress entry. */
extern int bcm_l2_egress_destroy(
    int unit, 
    bcm_if_t encap_id);

/* Get the config of an l2 egress entry with specified encap_id. */
extern int bcm_l2_egress_get(
    int unit, 
    bcm_if_t encap_id, 
    bcm_l2_egress_t *egr);

/* Find the encap_id with a given egress config */
extern int bcm_l2_egress_find(
    int unit, 
    bcm_l2_egress_t *egr, 
    bcm_if_t *encap_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* l2 egress traverse callback */
typedef int (*bcm_l2_egress_traverse_cb)(
    int unit, 
    bcm_l2_egress_t *egr, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterates over all l2 egress entries and executes user callback
 * function for each valid entry.
 */
extern int bcm_l2_egress_traverse(
    int unit, 
    bcm_l2_egress_traverse_cb trav_fn, 
    void *user_data);

/* Add a destination L2 address to trigger tunnel processing. */
extern int bcm_l2_tunnel_add(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vlan);

/* Clear a destination L2 address used to trigger tunnel processing. */
extern int bcm_l2_tunnel_delete(
    int unit, 
    bcm_mac_t mac, 
    bcm_vlan_t vlan);

/* Clear all destination L2 addresses used to trigger tunnel processing. */
extern int bcm_l2_tunnel_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an L2 learn limit structure. */
extern void bcm_l2_learn_limit_t_init(
    bcm_l2_learn_limit_t *limit);

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable/Disable BCM L2 learn limit subsystem. */
extern int bcm_l2_learn_limit_enable(
    int unit);

/* Enable/Disable BCM L2 learn limit subsystem. */
extern int bcm_l2_learn_limit_disable(
    int unit);

/* Set/Get L2 addresses learn limit. */
extern int bcm_l2_learn_limit_set(
    int unit, 
    bcm_l2_learn_limit_t *limit);

/* Set/Get L2 addresses learn limit. */
extern int bcm_l2_learn_limit_get(
    int unit, 
    bcm_l2_learn_limit_t *limit);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_L2_LEARN_CLASS_MOVE 0x01       

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get attributes for the specified L2 learning class. */
extern int bcm_l2_learn_class_set(
    int unit, 
    int lclass, 
    int lclass_prio, 
    uint32 flags);

/* Set/Get attributes for the specified L2 learning class. */
extern int bcm_l2_learn_class_get(
    int unit, 
    int lclass, 
    int *lclass_prio, 
    uint32 *flags);

/* Set/Get L2 learning class for the specified port. */
extern int bcm_l2_learn_port_class_set(
    int unit, 
    bcm_gport_t port, 
    int lclass);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L2 Statistics. */
typedef enum bcm_l2_stat_e {
    bcmL2StatSourcePackets = 0, 
    bcmL2StatSourceBytes = 1, 
    bcmL2StatDestPackets = 2, 
    bcmL2StatDestBytes = 3, 
    bcmL2StatDropPackets = 4, 
    bcmL2StatDropBytes = 5 
} bcm_l2_stat_t;

#define BCM_STAT_CHECK_L2_STAT(type)  do { if ( ((type) < bcmL2StatSourcePackets) || ((type) > bcmL2StatDropBytes) ) { return (BCM_E_BADID); } } while (0) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the specified L2 statistic from the chip. */
extern int bcm_l2_stat_get(
    int unit, 
    bcm_l2_addr_t *l2_addr, 
    bcm_l2_stat_t stat, 
    uint64 *val);

/* Get the specified L2 statistic from the chip. */
extern int bcm_l2_stat_get32(
    int unit, 
    bcm_l2_addr_t *l2_addr, 
    bcm_l2_stat_t stat, 
    uint32 *val);

/* Set the specified L2 statistic to the indicated value. */
extern int bcm_l2_stat_set(
    int unit, 
    bcm_l2_addr_t *l2_addr, 
    bcm_l2_stat_t stat, 
    uint64 val);

/* Set the specified L2 statistic to the indicated value. */
extern int bcm_l2_stat_set32(
    int unit, 
    bcm_l2_addr_t *l2_addr, 
    bcm_l2_stat_t stat, 
    uint32 val);

/* Enable/disable collection of statistics on the indicated L2 entry. */
extern int bcm_l2_stat_enable_set(
    int unit, 
    bcm_l2_addr_t *l2_addr, 
    int enable);

/* Set/Get L2 learning class for the specified port. */
extern int bcm_l2_learn_port_class_get(
    int unit, 
    bcm_gport_t port, 
    int *lclass);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L2 Learned Statistic Type. */
typedef enum bcm_l2_learn_stat_type_e {
    bcmL2LearnStatL2TableLearned = 0,   /* The counter of L2_ENTRY learned. */
    bcmL2LearnStatOverflowPoolLearned = 1, /* The counter of 128_OVERFLOW_ENTRY
                                           learned. */
    bcmL2LearnStatHashCollisions = 2,   /* The counter of L2_ENTRY hash
                                           collision. */
    bcmL2LearnStatCount = 3             /* Must be last. */
} bcm_l2_learn_stat_type_t;

/* L2 Learned Statistic info. */
typedef struct bcm_l2_learn_stat_s {
    bcm_l2_learn_stat_type_t type;  /* L2 learned statistic type. */
    uint32 value;                   /* 32-bit data value for L2 learned
                                       statistic. */
} bcm_l2_learn_stat_t;

/* Initialize an L2 learned statistic structure. */
extern void bcm_l2_learn_stat_t_init(
    bcm_l2_learn_stat_t *learn_stat);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or clear the specified L2 hash counter to the indicated value. */
extern int bcm_l2_learn_stat_set(
    int unit, 
    bcm_l2_learn_stat_t *learn_stat);

/* Get the statistic of the specified L2 hash counter from chip. */
extern int bcm_l2_learn_stat_get(
    int unit, 
    bcm_l2_learn_stat_t *learn_stat);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_l2_traverse_cb */
typedef int (*bcm_l2_traverse_cb)(
    int unit, 
    bcm_l2_addr_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterates over all entries in the L2 table and executes user callback
 * function for each entry.
 */
extern int bcm_l2_traverse(
    int unit, 
    bcm_l2_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Iterates over entries in the L2 table and executes user callback
 * function for each entry matching both given addr and mask.
 */
extern int bcm_l2_match_masked_traverse(
    int unit, 
    uint32 flags, 
    bcm_l2_addr_t *match_addr, 
    bcm_l2_addr_t *mask_addr, 
    bcm_l2_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for l2_replace API */
#define BCM_L2_REPLACE_MATCH_STATIC         0x00000001 /* Replace L2 static
                                                          entries. */
#define BCM_L2_REPLACE_MATCH_MAC            0x00000002 /* Replace all L2 entries
                                                          matching given MAC
                                                          address */
#define BCM_L2_REPLACE_MATCH_VLAN           0x00000004 /* Replace all L2 entries
                                                          matching given VLAN id */
#define BCM_L2_REPLACE_MATCH_DEST           0x00000008 /* Replace all L2 entries
                                                          matching given modid
                                                          port/tgid */
#define BCM_L2_REPLACE_VPN_TYPE             0x00000010 /* Replace all VPN type
                                                          L2 entry */
#define BCM_L2_REPLACE_VLAN_AND_VPN_TYPE    0x00000020 /* Replace all VLAN and
                                                          VPN type L2 entry */
#define BCM_L2_REPLACE_DELETE               0x00000100 /* L2 replace will
                                                          perform delete
                                                          operation instead of
                                                          replace on matching
                                                          entries */
#define BCM_L2_REPLACE_NEW_TRUNK            0x00000200 /* New destination is a
                                                          trunk group */
#define BCM_L2_REPLACE_PENDING              0x00000400 /* Replace L2 pending
                                                          entries. */
#define BCM_L2_REPLACE_NO_CALLBACKS         0x00000800 /* Replace L2 entries
                                                          without callbacks. */
#define BCM_L2_REPLACE_LEARN_LIMIT          0x00001000 /* Force ALL entries
                                                          including static to be
                                                          counted for learn
                                                          limit */
#define BCM_L2_REPLACE_IGNORE_PENDING       0x00002000 /* Force the L2 replace
                                                          process to ignore the
                                                          pending status check. */
#define BCM_L2_REPLACE_PROTECTION_RING      0x00004000 /* Replace L2 protection
                                                          ring. */
#define BCM_L2_REPLACE_MIGRATE_PORT         0x00008000 /* Replace L2 port. */
#define BCM_L2_REPLACE_IGNORE_DISCARD_SRC   0x00010000 /* Match L2 entries with
                                                          given Discard on
                                                          source. */
#define BCM_L2_REPLACE_IGNORE_DES_HIT       0x00020000 /* Match L2 entries
                                                          ignoring hit-bit
                                                          indication. */
#define BCM_L2_REPLACE_DES_HIT_CLEAR        0x00040000 /* When set then reset
                                                          DES Hit-bit state. */
#define BCM_L2_REPLACE_DISCARD_SRC_SET      0x00080000 /* When set then set
                                                          Discard on source */
#define BCM_L2_REPLACE_DISCARD_SRC_CLEAR    0x00100000 /* When set then reset
                                                          Discard on source */
#define BCM_L2_REPLACE_SRC_HIT_CLEAR        0x00200000 /* When set then reset
                                                          SRC Hit-bit state. */
#define BCM_L2_REPLACE_REMOTE_TRUNK         0x00400000 /* L2 replace will be
                                                          performed on remote
                                                          trunk entries. */
#define BCM_L2_REPLACE_AGE                  0x01000000 /* L2 replace will
                                                          perform age operation
                                                          instead of replace on
                                                          matching entries */
#define BCM_L2_REPLACE_MATCH_UC             0x02000000 /* L2 replace will be
                                                          performed on Unicast
                                                          entries */
#define BCM_L2_REPLACE_MATCH_MC             0x04000000 /* L2 replace will be
                                                          performed on Multicast
                                                          entries */
#define BCM_L2_REPLACE_DYNAMIC_SET          0x08000000 /* L2 replace changes the
                                                          MACT entries to be
                                                          dynamic */
#define BCM_L2_REPLACE_DYNAMIC_CLEAR        0x10000000 /* L2 replace changes the
                                                          MACT entries to be
                                                          static */
#define BCM_L2_REPLACE_MATCH_AGE            0x20000000 /* L2 replace will be
                                                          performed on L2
                                                          entries of a given age
                                                          state */
#define BCM_L2_REPLACE_MATCH_CLASS_ID       0x40000000 /* L2 replace will be
                                                          performed on L2
                                                          entries of a given
                                                          class id */
#define BCM_L2_REPLACE_MATCH_GROUP          0x40000000 /* L2 replace L2 entries
                                                          with a given group */
#define BCM_L2_REPLACE_MATCH_INT_PRI        0x80000000 /* L2 replace will be
                                                          performed on L2
                                                          entries of a given
                                                          internal priority */

#ifndef BCM_HIDE_DISPATCHABLE

/* Replace destination (or delete) multiple L2 entries. */
extern int bcm_l2_replace(
    int unit, 
    uint32 flags, 
    bcm_l2_addr_t *match_addr, 
    bcm_module_t new_module, 
    bcm_port_t new_port, 
    bcm_trunk_t new_trunk);

/* 
 * Replace destination (or delete) multiple L2 entries matching the given
 * address and mask. Only bits that are 1 (turned on) in the mask will be
 * checked when checking the match.
 */
extern int bcm_l2_replace_match(
    int unit, 
    uint32 flags, 
    bcm_l2_addr_t *match_addr, 
    bcm_l2_addr_t *mask_addr, 
    bcm_l2_addr_t *replace_addr, 
    bcm_l2_addr_t *replace_mask_addr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L2 Forwarding Domain Type. */
typedef enum bcm_l2_forward_domain_type_e {
    bcmL2ForwardDomainVlan = 0,         /* VLAN. */
    bcmL2ForwardDomainVpn = 1,          /* VFI. */
    bcmL2ForwardDomainVlanClassId = 2,  /* Use VLAN class ID from VLAN table as
                                           forward field. */
    bcmL2ForwardDomainCount = 3         /* Must be last. */
} bcm_l2_forward_domain_type_t;

/* L2 Station address info. */
typedef struct bcm_l2_station_s {
    uint32 flags;                       /* BCM_L2_STATION_xxx flags. */
    int priority;                       /* Entry priority. */
    bcm_mac_t dst_mac;                  /* Destination MAC address to match. */
    bcm_mac_t dst_mac_mask;             /* Destination MAC address mask value. */
    bcm_vlan_t vlan;                    /* VLAN to match. */
    bcm_vlan_t vlan_mask;               /* VLAN mask value. */
    bcm_port_t src_port;                /* Ingress port to match. */
    bcm_port_t src_port_mask;           /* Ingress port mask value. */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
    bcm_l2_forward_domain_type_t forward_domain_type; /* L2 forwarding domain type. */
    uint8 forward_domain_type_mask;     /* L2 forwarding domain type mask. */
    bcm_vlan_t inner_vlan;              /* Inner VLAN to match. */
    bcm_vlan_t inner_vlan_mask;         /* Inner VLAN mask value. */
    bcm_vlan_t vfi;                     /* Port default virtual forwarding
                                           instance to match. */
    bcm_vlan_t vfi_mask;                /* Port default virtual forwarding
                                           instance mask. */
    bcm_vlan_t forwarding_domain;       /* Forwarding domain for the packet with
                                           matched VLAN. */
} bcm_l2_station_t;

/* bcm_l2_station_traverse_cb */
typedef int (*bcm_l2_station_traverse_cb)(
    int unit, 
    bcm_l2_station_t *l2_station_addr, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Iterates over all entries in the L2 station table and executes user
 * callback function for each entry.
 */
extern int bcm_l2_station_traverse(
    int unit, 
    bcm_l2_station_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_l2_station_add/get APIs. */
#define BCM_L2_STATION_WITH_ID      (1 << 0)   /* Use the specified Station ID. */
#define BCM_L2_STATION_REPLACE      (1 << 1)   /* Replace an existing L2 Station
                                                  table entry. */
#define BCM_L2_STATION_IPV4         (1 << 2)   /* Subject packets matching this
                                                  entry to L3 processing. */
#define BCM_L2_STATION_IPV6         (1 << 3)   /* Subject packets matching this
                                                  entry to L3 processing. */
#define BCM_L2_STATION_ARP_RARP     (1 << 4)   /* Subject packets matching this
                                                  entry to ARP/RARP processing. */
#define BCM_L2_STATION_MPLS         (1 << 5)   /* Subject packets matching this
                                                  entry to MPLS processing. */
#define BCM_L2_STATION_MIM          (1 << 6)   /* Subject packets matching this
                                                  entry to MiM processing. */
#define BCM_L2_STATION_TRILL        (1 << 7)   /* Subject packets matching this
                                                  entry to Trill processing. */
#define BCM_L2_STATION_FCOE         (1 << 8)   /* Subject packets matching this
                                                  entry to FCoE processing. */
#define BCM_L2_STATION_OAM          (1 << 9)   /* Subject packets matching this
                                                  entry to OAM processing. */
#define BCM_L2_STATION_COPY_TO_CPU  (1 << 10)  /* Copy packet to CPU. */
#define BCM_L2_STATION_OLP          (1 << 11)  /* Subject packets matching this
                                                  entry to OAM-OLP header
                                                  processing. */
#define BCM_L2_STATION_XGS_MAC      (1 << 12)  /* Add this MAC address as source
                                                  MAC in the OLP header for
                                                  packets sent to OLP. */
#define BCM_L2_STATION_IPV4_MCAST   (1 << 13)  /* Subject packets matching this
                                                  entry to IPV4 mcast
                                                  processing. */
#define BCM_L2_STATION_IPV6_MCAST   (1 << 14)  /* Subject packets matching this
                                                  entry to IPV6 mcast
                                                  processing. */
#define BCM_L2_STATION_UNDERLAY     (1 << 15)  /* When RIOT is enabled, this
                                                  flag must be set for creating
                                                  tunnel mac addresses to
                                                  distinguish them from routing
                                                  mac addresses */
#define BCM_L2_STATION_DISCARD      (1 << 16)  /* Discard packets matching this
                                                  entry */
#define BCM_L2_STATION_EXTENDED     (1 << 17)  /* Indicate extended memory usage */
#define BCM_L2_STATION_NSH_OVER_L2  (1 << 18)  /* Allow NSH over L2 termination */

/* Initialize L2 Station structure */
extern void bcm_l2_station_t_init(
    bcm_l2_station_t *addr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an entry to L2 Station table */
extern int bcm_l2_station_add(
    int unit, 
    int *station_id, 
    bcm_l2_station_t *station);

/* Delete an entry from L2 Station Table */
extern int bcm_l2_station_delete(
    int unit, 
    int station_id);

/* Clear L2 station table */
extern int bcm_l2_station_delete_all(
    int unit);

/* Get L2 station entry details from Station Table. */
extern int bcm_l2_station_get(
    int unit, 
    int station_id, 
    bcm_l2_station_t *station);

/* Get size of L2 Station Table */
extern int bcm_l2_station_size_get(
    int unit, 
    int *size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L2 ring structure. */
typedef struct bcm_l2_ring_s {
    uint32 flags;                   /* flags */
    bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
    bcm_gport_t port0;              /* VLAN gport */
    bcm_gport_t port1;              /* VLAN gport */
    bcm_vpn_vector_t vpn_vector;    /* List of VPN IDs. */
    bcm_vlan_t vlan;                /* Base Vlan id. */
    bcm_vlan_t vlan_mask;           /* Vlan Mask. */
    bcm_vpn_t vpn;                  /* Base VPN id. */
    bcm_vpn_t vpn_mask;             /* VPN Mask. */
} bcm_l2_ring_t;

/* Initialize an empty L2 ring failover structure. */
extern void bcm_l2_ring_t_init(
    bcm_l2_ring_t *l2_ring);

#ifndef BCM_HIDE_DISPATCHABLE

/* Invoke accelerated L2 ring failover mechanism */
extern int bcm_l2_ring_replace(
    int unit, 
    bcm_l2_ring_t *l2_ring);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Device-independent L2 SA authentication structure. */
typedef struct bcm_l2_auth_addr_s {
    uint32 flags;           /* BCM_L2_AUTH_XXX flags. */
    bcm_mac_t sa_mac;       /* SA MAC address to match. */
    bcm_vlan_t vlan;        /* VLAN to match. */
    bcm_port_t src_port;    /* ingress port to match. */
} bcm_l2_auth_addr_t;

/* The callback function called for each valid entry in l2-auth. */
typedef int (*bcm_l2_auth_traverse_cb)(
    int unit, 
    bcm_l2_auth_addr_t *info, 
    void *userdata);

#define BCM_L2_AUTH_CHECK_IN_PORT       0x00000001 /* check packet with sa_mac
                                                      received on the expected
                                                      src_port. */
#define BCM_L2_AUTH_CHECK_VLAN          0x00000002 /* check packet with sa_mac
                                                      received on the expected
                                                      vlan */
#define BCM_L2_AUTH_PERMIT_TAGGED_ONLY  0x00000004 /* permit only tagged packets */

/* 
 * Initialize an L2 SA authentication structure to a specified MAC
 * address and VLAN ID.
 */
extern void bcm_l2_auth_addr_t_init(
    bcm_l2_auth_addr_t *addr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an L2 authentication entry. */
extern int bcm_l2_auth_add(
    int unit, 
    bcm_l2_auth_addr_t *addr);

/* Get an L2 authentication entry. */
extern int bcm_l2_auth_get(
    int unit, 
    bcm_l2_auth_addr_t *addr);

/* Delete an L2 authentication entry. */
extern int bcm_l2_auth_delete(
    int unit, 
    bcm_l2_auth_addr_t *addr);

/* Clear all L2 authentication entries. */
extern int bcm_l2_auth_delete_all(
    int unit);

/* 
 * Iterates over all l2 authentication entriesand executes user call back
 * function for each valid entry.
 */
extern int bcm_l2_auth_traverse(
    int unit, 
    bcm_l2_auth_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Iterates over matching entries in the L2 table and executes user
 * callback function for each matching entry
 */
extern int bcm_l2_matched_traverse(
    int unit, 
    uint32 flags, 
    bcm_l2_addr_t *match_addr, 
    bcm_l2_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_l2_traverse_match API. */
#define BCM_L2_TRAVERSE_MATCH_STATIC        (1 << 0)   /* Match L2 static
                                                          entries only, if it
                                                          and
                                                          L2_TRAVERSE_MATCH_DYNAMIC
                                                          not present match
                                                          static entries
                                                          together with all
                                                          other entries */
#define BCM_L2_TRAVERSE_MATCH_MAC           (1 << 1)   /* Match L2 entries with
                                                          given MAC address */
#define BCM_L2_TRAVERSE_MATCH_VLAN          (1 << 2)   /* Match L2 entries with
                                                          given VLAN ID */
#define BCM_L2_TRAVERSE_MATCH_DEST          (1 << 3)   /* Match L2 entries with
                                                          given modid port/tgid */
#define BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC  (1 << 4)   /* match entries ignoring
                                                          destination SA-discard
                                                          indication */
#define BCM_L2_TRAVERSE_IGNORE_DES_HIT      (1 << 5)   /* match entries ignoring
                                                          destination hit-bit
                                                          indication */
#define BCM_L2_TRAVERSE_IGNORE_SRC_HIT      (1 << 6)   /* match entries ignoring
                                                          source hit-bit
                                                          indication */
#define BCM_L2_TRAVERSE_MATCH_NATIVE        (1 << 7)   /* Match entries with
                                                          given local address
                                                          indication */
#define BCM_L2_TRAVERSE_MATCH_DYNAMIC       (1 << 8)   /* Match L2 non-static
                                                          entries only, if it
                                                          and
                                                          L2_TRAVERSE_MATCH_STATIC
                                                          not present match
                                                          dynamic entries
                                                          together with all
                                                          other entries */
#define BCM_L2_TRAVERSE_MATCH_IVL           (1 << 9)   /* Traverse l2 entries
                                                          that have an IVL
                                                          format. */
#define BCM_L2_TRAVERSE_MATCH_GROUP         (1 << 10)  /* Match L2 entries with
                                                          a given group. */
#define BCM_L2_TRAVERSE_MATCH_MESH          (1 << 11)  /* Match L2 entries with
                                                          mesh bit set. */
#define BCM_L2_TRAVERSE_IGNORE_PENDING      (1 << 12)  /* non-blocking traverse. */

/* BCM_L2_MAC_PORT_* flags. */
#define BCM_L2_MAC_PORT_WITH_ID 0x00000001 /* create mac port with specified ID */
#define BCM_L2_MAC_PORT_REPLACE 0x00000002 /* Replace existing mac port */

/* MAC port type. */
typedef struct bcm_l2_mac_port_s {
    uint32 flags;               /* BCM_L2_MAC_PORT_xxx. */
    bcm_mac_t mac;              /* MAC address to match. */
    bcm_gport_t port;           /* Physical port / trunk */
    bcm_gport_t mac_port_id;    /* MAC GPORT ID. */
} bcm_l2_mac_port_t;

/* Initialize the MAC port structure. */
extern void bcm_l2_mac_port_t_init(
    bcm_l2_mac_port_t *mac_port);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create MAC port */
extern int bcm_l2_mac_port_create(
    int unit, 
    bcm_l2_mac_port_t *mac_port);

/* Destroy MAC port. */
extern int bcm_l2_mac_port_destroy(
    int unit, 
    bcm_gport_t mac_port_id);

/* Destroy all MAC ports. */
extern int bcm_l2_mac_port_destroy_all(
    int unit);

/* Get MAC Port information */
extern int bcm_l2_mac_port_get(
    int unit, 
    bcm_l2_mac_port_t *mac_port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MAC port traverse function prototype. */
typedef int (*bcm_l2_mac_port_traverse_cb)(
    int unit, 
    bcm_l2_mac_port_t *mac_port, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse MAC ports */
extern int bcm_l2_mac_port_traverse(
    int unit, 
    bcm_l2_mac_port_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Struct to get gport forward info */
typedef struct bcm_l2_gport_forward_info_s {
    int phy_gport; 
    int encap_id; 
} bcm_l2_gport_forward_info_t;

/* Initialize bcm_l2_gport_forward_info_t struct */
extern void bcm_l2_gport_forward_info_t_init(
    bcm_l2_gport_forward_info_t *forward_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the forwarding information for the given gport. */
extern int bcm_l2_gport_forward_info_get(
    int unit, 
    int gport_id, 
    bcm_l2_gport_forward_info_t *forward_info);

/* Purge L2 Entries for the specific set of (port,vlan) pairs. */
extern int bcm_l2_addr_delete_by_vlan_gport_multi(
    int unit, 
    uint32 flags, 
    int num_pairs, 
    bcm_vlan_t *vlan, 
    bcm_gport_t *gport);

/* Add mutli L2 address entry to the specified device. */
extern int bcm_l2_addr_multi_add(
    int unit, 
    bcm_l2_addr_t *l2addr, 
    int count);

/* Delete multi L2 address entry from the specified device. */
extern int bcm_l2_addr_multi_delete(
    int unit, 
    bcm_l2_addr_t *l2addr, 
    int count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Max number of MACs in the structure below. */
#define BCM_RX_TRAP_L2CP_NOF_ENTRIES    64         

/* Set of MACS for L2CP traps profile build. */
typedef struct bcm_l2cp_profile_info_s {
    int nof_entries; 
    bcm_mac_t mac_add[BCM_RX_TRAP_L2CP_NOF_ENTRIES]; 
} bcm_l2cp_profile_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* creates and writes down to HW the bitmap profile for L2CP traps. */
extern int bcm_l2_cache_profile_set(
    int unit, 
    int trap_type, 
    uint32 profile_idx, 
    uint32 flags, 
    bcm_l2cp_profile_info_t *l2cp_profile_info);

/* reads from HW the bitmap profile for L2CP traps. */
extern int bcm_l2_cache_profile_get(
    int unit, 
    int trap_type, 
    uint32 profile_idx, 
    uint32 flags, 
    bcm_l2cp_profile_info_t *l2cp_profile_info);

/* 
 * links the given VPN to required profile index (from 0 to 3) for L2CP
 * traps.
 */
extern int bcm_l2_cache_vpn_to_profile_map_set(
    int unit, 
    uint32 vsi, 
    uint32 profile_idx);

/* for the given VPN returns profile index (from 0 to 3) for L2CP traps. */
extern int bcm_l2_cache_vpn_to_profile_map_get(
    int unit, 
    uint32 vsi, 
    uint32 *profile_idx);

/* 
 * Retrieve a MAC entry according to mac, vid (used as VSI) and modid.
 * When modid != 0 it should hold a VLAN ID for IVL entries.
 */
extern int bcm_l2_addr_by_struct_get(
    int unit, 
    bcm_l2_addr_t *l2addr);

/* 
 * Delete a MAC entry according to mac, vid (used as VSI) and modid. When
 * modid != 0 it should hold a VLAN ID for IVL entries.
 */
extern int bcm_l2_addr_by_struct_delete(
    int unit, 
    bcm_l2_addr_t *l2addr);

#endif /* BCM_HIDE_DISPATCHABLE */

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
#define BCM_L2_CHANGE_FIELDS_DVP            (1 << 0)   /* DVP key paramter is
                                                          valid. */
#define BCM_L2_CHANGE_FIELDS_REPLACE        (1 << 1)   /* Replace L2 Change
                                                          Fields entry for given
                                                          key fields. */
#define BCM_L2_CHANGE_FIELDS_VLAN_ADD       (1 << 2)   /* Add payload VLAN tag. */
#define BCM_L2_CHANGE_FIELDS_VLAN_DELETE    (1 << 3)   /* Delete payload vlan. */
#define BCM_L2_CHANGE_FIELDS_VLAN_REPLACE   (1 << 4)   /* Replace payload vlan
                                                          tag. */
#define BCM_L2_CHANGE_FIELDS_VLAN_PRI_TPID_REPLACE (1 << 5)   /* Replace payload vlan,
                                                          priority and tpid. */
#define BCM_L2_CHANGE_FIELDS_VLAN_TPID_REPLACE (1 << 6)   /* Replace payload vlan
                                                          and tpid. */
#define BCM_L2_CHANGE_FIELDS_VLAN_PRI_REPLACE (1 << 7)   /* Replace payload vlan
                                                          and priority. */
#define BCM_L2_CHANGE_FIELDS_PRI_REPLACE    (1 << 8)   /* Replace payload
                                                          priority only. */
#define BCM_L2_CHANGE_FIELDS_TPID_REPLACE   (1 << 9)   /* Replace payload tpid
                                                          only. */
#endif

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
typedef struct bcm_l2_change_fields_s {
    uint32 flags; 
    bcm_mac_t macda; 
    bcm_mac_t macsa; 
    uint8 pri; 
    uint8 cfi; 
    uint16 tpid; 
    uint16 vlan; 
    uint32 change_l2_fields_class_id; 
    bcm_gport_t dvp; 
} bcm_l2_change_fields_t;
#endif

/* Requires BROADCOM_PREMIUM license */
#if defined(INCLUDE_L3)
typedef int (*bcm_l2_change_fields_traverse_cb)(
    int unit, 
    bcm_l2_change_fields_t *l2_fields_info, 
    void *user_data);
#endif

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
extern void bcm_l2_change_fields_t_init(
    bcm_l2_change_fields_t *l2_fields_info);
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
extern int bcm_l2_change_fields_add(
    int unit, 
    bcm_l2_change_fields_t *l2_fields_info);
#endif

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
extern int bcm_l2_change_fields_delete(
    int unit, 
    bcm_l2_change_fields_t *l2_fields_info);
#endif

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
extern int bcm_l2_change_fields_get(
    int unit, 
    bcm_l2_change_fields_t *l2_fields_info);
#endif

#if defined(INCLUDE_L3)
/* Requires BROADCOM_PREMIUM license */
extern int bcm_l2_change_fields_traverse(
    int unit, 
    bcm_l2_change_fields_traverse_cb cb, 
    void *user_data);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_L2_H__ */
