/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        acl.h
 *
 * Purpose:     
 *     Access Control Lists (ACL) Macros, type definitions,
 *     and API prototypes.
 */

#ifndef   _ACL_H_
#define   _ACL_H_

#if defined(INCLUDE_ACL)

#include <bcm/types.h>
#include <bcmx/lport.h>
#include <bcmx/lplist.h>

/*
 * Macros: BCMA_ACL_ACTION_xxx
 *
 * Purpose:
 *     Action flags to be performed in response to matches. These flags are
 *     part of bcma_acl_action_t.
 *
 * Actions:
 *     PERMIT - Allow packet to be switched
 *     DENY   - Drop matching packet
 *     REDIR  - Redirect packet to an alternative port
 *     MIRROR - Copy packet to mirror port
 *     LOG    - Send copy of packet to CPU
 */
#define BCMA_ACL_ACTION_PERMIT     (1 << 0)
#define BCMA_ACL_ACTION_DENY       (1 << 1)
#define BCMA_ACL_ACTION_REDIR      (1 << 2)
#define BCMA_ACL_ACTION_MIRROR     (1 << 3)
#define BCMA_ACL_ACTION_LOG        (1 << 4)

typedef int bcma_acl_list_id_t;
typedef int bcma_acl_rule_id_t;

/*
 * Typedef: bcma_acl_action_t
 *
 * Purpose:
 *     Actions to be performed on when packet meets matching criteria.
 */

typedef struct bcma_acl_action_s {
    uint16                  flags; /* BCMA_ACL_ACTION_XXX */

    /* Module/port parameters for redirection and mirroring */
    bcmx_lport_t            redir_port;
    bcmx_lport_t            mirror_port;

} bcma_acl_action_t;

typedef uint8 bcma_acl_ip_protocol_t;

/* Values for bcma_acl_ip_protocol_t from RFC 1700. */
#define BCMA_ACL_IPPROTO_HOPBYHOP     0 /* IPv6 Hop-by-Hop option  */
#define BCMA_ACL_IPPROTO_ICMP         1 /* Internet Control Message*/
#define BCMA_ACL_IPPROTO_IGMP         2 /* Internet Group Management*/
#define BCMA_ACL_IPPROTO_GGP          3 /* Gateway-to-Gateway */
#define BCMA_ACL_IPPROTO_IP           4 /* IP in IP (encasulation) */
#define BCMA_ACL_IPPROTO_ST           5 /* Stream */
#define BCMA_ACL_IPPROTO_TCP          6 /* Transmission Control */
#define BCMA_ACL_IPPROTO_UCL          7 /* UCL */
#define BCMA_ACL_IPPROTO_EGP          8 /* Exterior Gateway Protocol */
#define BCMA_ACL_IPPROTO_IGP          9 /* any private interior gateway */
#define BCMA_ACL_IPPROTO_BBN_RCC_MON 10 /* BBN RCC Monitoring */
#define BCMA_ACL_IPPROTO_NVP_II      11 /* Network Voice Protocol */
#define BCMA_ACL_IPPROTO_PUP         12 /* PUP */
#define BCMA_ACL_IPPROTO_ARGUS       13 /* ARGUS */
#define BCMA_ACL_IPPROTO_EMCON       14 /* EMCON */
#define BCMA_ACL_IPPROTO_XNET        15 /* Cross Net Debugger */
#define BCMA_ACL_IPPROTO_CHAOS       16 /* Chaos */
#define BCMA_ACL_IPPROTO_UDP         17 /* User Datagram */
#define BCMA_ACL_IPPROTO_MUX         18 /* Multiplexing */
#define BCMA_ACL_IPPROTO_DCN_MEAS    19 /* DCN Measurement Subsystems */
#define BCMA_ACL_IPPROTO_HMP         20 /* Host Monitoring */
#define BCMA_ACL_IPPROTO_PRM         21 /* Packet Radio Measurement */
#define BCMA_ACL_IPPROTO_XNS_IDP     22 /* XEROX NS IDP */
#define BCMA_ACL_IPPROTO_TRUNK_1     23 /* Trunk-1 */
#define BCMA_ACL_IPPROTO_TRUNK_2     24 /* Trunk-2 */
#define BCMA_ACL_IPPROTO_LEAF_1      25 /* Leaf-1 */
#define BCMA_ACL_IPPROTO_LEAF_2      26 /* Leaf-2 */
#define BCMA_ACL_IPPROTO_RDP         27 /* Reliable Data Protocol */
#define BCMA_ACL_IPPROTO_IRTP        28 /* Internet Reliable Transaction */
#define BCMA_ACL_IPPROTO_ISO_TP4     29 /* ISO Transport Protocol Class 4 */
#define BCMA_ACL_IPPROTO_NETBLT      30 /* Bulk Data Transfer Protocol */
#define BCMA_ACL_IPPROTO_MFE_NSP     31 /* MFE Network Services Protocol */
#define BCMA_ACL_IPPROTO_MERIT_INP   32 /* MERIT Internodal Protocol */
#define BCMA_ACL_IPPROTO_SEP         33 /* Sequential Exchange Protocol */
#define BCMA_ACL_IPPROTO_3PC         34 /* Third Party Connect Protocol */
#define BCMA_ACL_IPPROTO_IDPR        35 /* Inter-Domain Policy Routing Protocol */
#define BCMA_ACL_IPPROTO_XTP         36 /* XTP */
#define BCMA_ACL_IPPROTO_DDP         37 /* Datagram Delivery Protocol */
#define BCMA_ACL_IPPROTO_IDPR_CMTP   38 /* IDPR Control Message Transport Proto */
#define BCMA_ACL_IPPROTO_TP_P_P      39 /* TP++ Transport Protocol */
#define BCMA_ACL_IPPROTO_IL          40 /* IL Transport Protocol */
#define BCMA_ACL_IPPROTO_SIP         41 /* Simple Internet Protocol */
#define BCMA_ACL_IPPROTO_SDRP        42 /* Source Demand Routing Protocol */
#define BCMA_ACL_IPPROTO_SIP_SR      43 /* SIP Source Route */
#define BCMA_ACL_IPPROTO_SIP_FRAG    44 /* SIP Fragment */
#define BCMA_ACL_IPPROTO_IDRP        45 /* Inter-Domain Routing Protocol */
#define BCMA_ACL_IPPROTO_RSVP        46 /* Reservation Protocol */
#define BCMA_ACL_IPPROTO_GRE         47 /* General Routing Encapsulation */
#define BCMA_ACL_IPPROTO_MHRP        48 /* Mobile Host Routing Protocol */
#define BCMA_ACL_IPPROTO_BNA         49 /* BNA */
#define BCMA_ACL_IPPROTO_SIPP_ESP    50 /* SIPP Encap Security Payload */
#define BCMA_ACL_IPPROTO_SIPP_AH     51 /* SIPP Authentication Header */
#define BCMA_ACL_IPPROTO_I_NLSP      52 /* Integrated Net Layer Security  TUBA */
#define BCMA_ACL_IPPROTO_SWIPE       53 /* IP with Encryption */
#define BCMA_ACL_IPPROTO_NHRP        54 /* NBMA Next Hop Resolution Protocol */
/* 55-60                 Unassigned*/
#define BCMA_ACL_IPPROTO_AHIP        61 /* any host internal protocol */
#define BCMA_ACL_IPPROTO_CFTP        62 /* CFTP */
#define BCMA_ACL_IPPROTO_HI          63 /* Host internal protocol */
#define BCMA_ACL_IPPROTO_SAT_EXPAK   64 /* SATNET and Backroom EXPAK */
#define BCMA_ACL_IPPROTO_KRYPTOLAN   65 /* Kryptolan */
#define BCMA_ACL_IPPROTO_RVD         66 /* MIT Remote Virtual Disk Protocol */
#define BCMA_ACL_IPPROTO_IPPC        67 /* Internet Pluribus Packet Core */
#define BCMA_ACL_IPPROTO_ADFS        68 /* any distributed file system */
#define BCMA_ACL_IPPROTO_SAT_MON     69 /* SATNET Monitoring */
#define BCMA_ACL_IPPROTO_VISA        70 /* VISA Protocol */
#define BCMA_ACL_IPPROTO_IPCV        71 /* Internet Packet Core Utility */
#define BCMA_ACL_IPPROTO_CPNX        72 /* Computer Protocol Network Executive */
#define BCMA_ACL_IPPROTO_CPHB        73 /* Computer Protocol Heart Beat */
#define BCMA_ACL_IPPROTO_WSN         74 /* Wang Span Network */
#define BCMA_ACL_IPPROTO_PVP         75 /* Packet Video Protocol */
#define BCMA_ACL_IPPROTO_BR_SAT_MON  76 /* Backroom SATNET Monitoring */
#define BCMA_ACL_IPPROTO_SUN_ND      77 /* SUN ND PROTOCOL-Temporary */
#define BCMA_ACL_IPPROTO_WB_MON      78 /* WIDEBAND Monitoring */
#define BCMA_ACL_IPPROTO_WB_EXPAK    79 /* WIDEBAND EXPAK */
#define BCMA_ACL_IPPROTO_ISO_IP      80 /* ISO Internet Protocol */
#define BCMA_ACL_IPPROTO_VMTP        81 /* VMTP */
#define BCMA_ACL_IPPROTO_SECURE_VMTP 82 /* SECURE-VMTP */
#define BCMA_ACL_IPPROTO_VINES       83 /* VINES */
#define BCMA_ACL_IPPROTO_TTP         84 /* TTP */
#define BCMA_ACL_IPPROTO_NSFNET_IGP  85 /* NSFNET-IGP */
#define BCMA_ACL_IPPROTO_DGP         86 /* Dissimilar Gateway Protocol */
#define BCMA_ACL_IPPROTO_TCF         87 /* TCF */
#define BCMA_ACL_IPPROTO_IGRP        88 /* IGRP */
#define BCMA_ACL_IPPROTO_OSPFIGP     89 /* OSPFIGP */
#define BCMA_ACL_IPPROTO_SPRITE_RPC  90 /* Sprite RPC Protocol */
#define BCMA_ACL_IPPROTO_LARP        91 /* Locus Address Resolution Protocol */
#define BCMA_ACL_IPPROTO_MTP         92 /* Multicast Transport Protocol */
#define BCMA_ACL_IPPROTO_AX_25       93 /* AX.25 Frames */
#define BCMA_ACL_IPPROTO_IPIP        94 /* IP-in-IP Encapsulation Protocol */
#define BCMA_ACL_IPPROTO_MICP        95 /* Mobile Internetworking Control Pro. */
#define BCMA_ACL_IPPROTO_SCC_SP      96 /* Semaphore Communications Sec. Pro. */
#define BCMA_ACL_IPPROTO_ETHERIP     97 /* Ethernet-within-IP Encapsulation */
#define BCMA_ACL_IPPROTO_ENCAP       98 /* Encapsulation Header */
#define BCMA_ACL_IPPROTO_APES        99 /* any private encryption scheme */
#define BCMA_ACL_IPPROTO_GMTP       100 /* GMTP */
/* 101-254                Unassigned */
#define BCMA_ACL_IPPROTO_ANY        255 /* Reserved */



#define BCMA_ACL_RULE_L4_SRC_PORT     (1 << 0)
#define BCMA_ACL_RULE_L4_DST_PORT     (1 << 1)
#define BCMA_ACL_RULE_SRC_MAC         (1 << 2)
#define BCMA_ACL_RULE_DST_MAC         (1 << 3)
#define BCMA_ACL_RULE_SRC_IP4         (1 << 4)
#define BCMA_ACL_RULE_DST_IP4         (1 << 5)
#define BCMA_ACL_RULE_SRC_IP6         (1 << 6)
#define BCMA_ACL_RULE_DST_IP6         (1 << 7)
#define BCMA_ACL_RULE_VLAN            (1 << 8)
#define BCMA_ACL_RULE_IPPROTOCOL      (1 << 9)
#define BCMA_ACL_RULE_ETHERTYPE       (1 <<10)

/*
 * Typedef: bcma_acl_rule_t
 *
 * Purpose:
 *     Set of packet matching criteria and the actions to be performed in the
 *     event of a match.
 *
 * Fields:
 *     bcma_acl_rule_id_t rule_id - Handle to this rule
 *     uint16 flags             - see BCMA_ACL_RULE_XXX
 *     bcm_l4_port src_port_min - Minimum Layer 4 (i.e. TCP,UDP) source port 
 *     bcm_l4_port src_port_min - Maximum Layer 4 (i.e. TCP,UDP) source port 
 *     bcm_l4_port dst_port_min - Minimum Layer 4 (i.e. TCP,UDP) destination
 *                                port 
 *     bcm_l4_port dst_port_min - Maximum Layer 4 (i.e. TCP,UDP) destination
 *                                port 
 *     bcm_mac_t src_mac        - Source MAC address
 *     bcm_mac_t dst_mac        - Destination MAC address
 *     bcm_ip_t src_ip          - IPv4 source IP address
 *     bcm_ip_t src_ip_mask     - IPv4 source IP address mask
 *     bcm_ip_t dst_ip          - IPv4 destination IP address
 *     bcm_ip_t dst_ip_mask     - IPv4 destination IP address mask
 *     bcm_ip6_t src_ip         - IPv6 source IP address
 *     bcm_ip6_t src_ip_mask    - IPv6 source IP address mask
 *     bcm_ip6_t dst_ip         - IPv6 destination IP address
 *     bcm_ip6_t dst_ip_mask    - IPv6 destination IP address mask
 *     bcm_vlan_t vlan_min      - Minimum VLAN to match
 *     bcm_vlan_t vlan_max      - Maximum VLAN to match
 *     bcma_acl_ip_protocol ip_protocol - IP protocol (TCP, UDP, etc.)
 *     bcma_acl_action_t actions - Actions to take in response to matches
 */
typedef struct bcma_acl_rule_s {
    bcma_acl_rule_id_t      rule_id;
    uint16                  flags;

    /* Pattern Matching Fields */
    bcm_mac_t               src_mac;
    bcm_mac_t               dst_mac;
    bcm_ip_t                src_ip,  src_ip_mask;
    bcm_ip_t                dst_ip,  dst_ip_mask;
    bcm_ip6_t               src_ip6, src_ip6_mask;
    bcm_ip6_t               dst_ip6, dst_ip6_mask;

    /* Range of VLANs */
    bcm_vlan_t              vlan_min;
    bcm_vlan_t              vlan_max;

    /* Layer 4: i.e. TCP, UDP ranges */
    bcm_l4_port_t           src_port_min;
    bcm_l4_port_t           src_port_max;
    bcm_l4_port_t           dst_port_min;
    bcm_l4_port_t           dst_port_max;

    uint16                  ether_type;
    bcma_acl_ip_protocol_t  ip_protocol;
    bcma_acl_action_t       actions;
} bcma_acl_rule_t; 

/*
 * Typedef: bcma_acl_t
 *
 * Purpose:
 *     Data about an access control list. The List ID is used when adding
 *     rules to the list.
 *
 * Fields:
 *     bcma_acl_list_id_t lid - Handle to this ACL
 *     bcmx_lplist_t lplist   - Logical port list where ACL applies
 */
typedef struct bcma_acl_s {
    bcma_acl_list_id_t      list_id; /* Unique handle to this list   */
    bcmx_lplist_t           lplist;  /* Ports where list applies     */
    int                     prio;    /* Relative priority of list.   */
} bcma_acl_t; 

/* Constructor and destructor for ACL module */
extern int bcma_acl_init(void);
extern int bcma_acl_detach(void);

/* List Management functions */
extern int bcma_acl_add(bcma_acl_t *list_id);
extern int bcma_acl_remove(bcma_acl_list_id_t list_id);
extern int bcma_acl_get(bcma_acl_list_id_t list_id, bcma_acl_t *list);

extern int bcma_acl_rule_add(bcma_acl_list_id_t list_id,
                             bcma_acl_rule_t *rule);
extern int bcma_acl_rule_remove(bcma_acl_list_id_t list_id,
                                bcma_acl_rule_id_t rule_id);
extern int bcma_acl_rule_get(bcma_acl_rule_id_t rule_id,
                             bcma_acl_rule_t **rule);

/* Validation and Installation functions */
extern int bcma_acl_install(void);
extern int bcma_acl_uninstall(void);

#ifdef BROADCOM_DEBUG
extern int bcma_acl_show(void);
extern int bcma_acl_list_show(bcma_acl_list_id_t list_id);
extern int bcma_acl_rule_show_id(bcma_acl_rule_id_t rule_id);
extern int bcma_acl_rule_show(bcma_acl_rule_t *rule);
extern int bcma_acl_action_show(bcma_acl_action_t *action);
#endif /* BROADCOM_DEBUG */

#endif /* INCLUDE_ACL */
#endif /* _ACL_H_ */
