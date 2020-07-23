/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_L3_H__
#define __BCM_L3_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm/qos.h>
#include <bcm/mpls.h>

/* L3 module flags. */
#define BCM_L3_L2ONLY                   (1U << 0)  /* L2 switch only on
                                                      interface. */
#define BCM_L3_UNTAG                    (1U << 1)  /* Packet goes out untagged. */
#define BCM_L3_S_HIT                    (1U << 2)  /* Source IP address match. */
#define BCM_L3_D_HIT                    (1U << 3)  /* Destination IP address
                                                      match. */
#define BCM_L3_HIT                      (BCM_L3_S_HIT | BCM_L3_D_HIT) 
#define BCM_L3_HIT_CLEAR                (1U << 4)  /* Clear HIT bit. */
#define BCM_L3_ADD_TO_ARL               (1U << 5)  /* Add interface address MAC
                                                      to ARL. */
#define BCM_L3_WITH_ID                  (1U << 6)  /* ID is provided. */
#define BCM_L3_NEGATE                   (1U << 7)  /* Negate a set. */
#define BCM_L3_INGRESS_ONLY             BCM_L3_NEGATE /* configure ingress objects
                                                      only. */
#define BCM_L3_REPLACE                  (1U << 8)  /* Replace existing entry. */
#define BCM_L3_TGID                     (1U << 9)  /* Port belongs to trunk. */
#define BCM_L3_OVERLAY_ENCAP            BCM_L3_TGID /* LL encapsulation, used for
                                                      overlay LL for intra DC
                                                      routed packets */
#define BCM_L3_RPE                      (1U << 10) /* Pick up new priority
                                                      (COS). */
#define BCM_L3_ENCAP_SPACE_OPTIMIZED    BCM_L3_RPE /* Optimize encapsulation
                                                      database resources. */
#define BCM_L3_IPMC                     (1U << 11) /* Set IPMC for real IPMC
                                                      entry. */
#define BCM_L3_L2TOCPU                  (1U << 12) /* Packet to CPU unrouted,
                                                      XGS12: Set IPMC for UC
                                                      address. */
#define BCM_L3_NATIVE_ENCAP             BCM_L3_L2TOCPU /* Eth encapsulation, used
                                                      for nativ Ethernet for
                                                      intra DC routed packets */
#define BCM_L3_DEFIP_CPU                (1U << 13) /* Strata: DEFIP CPU bit set. */
#define BCM_L3_DEFIP_LOCAL              BCM_L3_DEFIP_CPU /* XGS: Local DEFIP route. */
#define BCM_L3_VIRTUAL_OBJECT           (1U << 13) /* FEC/egress-encap (virtual
                                                      FEC). */
#define BCM_L3_EGRESS_ONLY              BCM_L3_VIRTUAL_OBJECT /*  Configure Egress objects
                                                      only. */
#define BCM_L3_MULTIPATH                (1U << 14) /* Specify ECMP treatment. */
#define BCM_L3_HOST_LOCAL               (1U << 15) /* Packet to local IP stack. */
#define BCM_L3_HOST_AS_ROUTE            (1U << 16) /* Use LPM if host table
                                                      full. */
#define BCM_L3_IP6                      (1U << 17) /* IPv6. */
#define BCM_L3_RPF                      (1U << 18) /* RPF check. */
#define BCM_L3_MC_RPF_EXPLICIT          (BCM_L3_RPF | BCM_L3_IPMC) /* Indicates explicit
                                                      multicast RPF mode */
#define BCM_L3_MC_RPF_SIP_BASE          BCM_L3_RPF /* Indicates SIP base
                                                      multicast RPF mode */
#define BCM_L3_MC_RPF_EXPLICIT_ECMP     (BCM_L3_RPF | BCM_L3_MULTIPATH) /* Indicate Explicit
                                                      multicast ECMP RPF mode */
#define BCM_L3_MC_NO_RPF                (BCM_L3_RPF | BCM_L3_L2ONLY) /* Indicates no multicast RPF
                                                      mode */
#define BCM_L3_SRC_DISCARD              (1U << 19) /* Source Match Discard. */
#define BCM_L3_DST_DISCARD              (1U << 20) /* Destination match discard. */
#define BCM_L3_SECONDARY                (1U << 21) /* Secondary L3 interface
                                                      (Must NOT be used with
                                                      L3_CASCADED) */
#define BCM_L3_CASCADED                 (1U << 21) /* Indicates an L3 egress
                                                      interface pointed by
                                                      another L3 egress
                                                      interface (Must NOT be
                                                      used with L3_SECONDARY) */
#define BCM_L3_2ND_HIERARCHY            BCM_L3_CASCADED /* Indicates that an L3
                                                      egress/ECMP interface is
                                                      placed on a 2nd hierarchy
                                                      level of L3 egresses/ECMPs
                                                      (Can be replaced by
                                                      BCM_L3_CASCADED). */
#define BCM_L3_ROUTE_LABEL              (1U << 22) /* Indicates that MPLS label
                                                      in route entry is valid. */
#define BCM_L3_COPY_TO_CPU              (1U << 23) /* Send a copy to CPU. */
#define BCM_L3_KEEP_SRCMAC              (1U << 24) /* Disable SA replacement for
                                                      L3UC packets. */
#define BCM_L3_KEEP_DSTMAC              (1U << 25) /* Disable DA replacement for
                                                      L3UC packets. */
#define BCM_L3_KEEP_VLAN                (1U << 26) /* Disable VLAN replacement
                                                      for L3UC packets. */
#define BCM_L3_KEEP_TTL                 (1U << 27) /* Disable TTL replacement
                                                      for L3UC packets.When used
                                                      in conjunction with
                                                      BCM_L3_ROUTE_LABEL,
                                                      disable TTL replacement
                                                      for MPLS flows */
#define BCM_L3_TRILL_ONLY               (1U << 28) /* Specific for TRILL Nexthop */
#define BCM_L3_SRC_DST_NAT_REALM_ID     (1U << 28) /* To set the nat realm id to
                                                      both source nat id and
                                                      destionation nat id. */
#define BCM_L3_L2GRE_ONLY               (1U << 29) /* Specific for L2GRE Nexthop */
#define BCM_L3_INTERNAL_ROUTE           BCM_L3_L2GRE_ONLY /* Add IP route into the
                                                      internal LPM table. */
#define BCM_L3_QUEUE_MAP                (1U << 30) /* Use queue map to derive
                                                      queue number */
#define BCM_L3_3RD_HIERARCHY            BCM_L3_QUEUE_MAP /* Indicates that an L3
                                                      egress/ECMP interface is
                                                      placed on a 3rd hierarchy
                                                      level of L3
                                                      egresses/ECMPs. */
#define BCM_L3_DEREFERENCED_NEXTHOP     (1U << 30) /* Indicates this is a wider
                                                      L3 entry. */
#define BCM_L3_ECMP_RH_REPLACE          BCM_L3_DEREFERENCED_NEXTHOP /* Replace ECMP member
                                                      without RH flowset table
                                                      shuffle. */
#define BCM_L3_VXLAN_ONLY               (1U << 31) /* Specific for VXLAN Nexthop */

/* L3 module flags2. */
#define BCM_L3_FLAGS2_NIV_ENCAP_LOCAL       (1 << 0)   /* Apply VNTAG_ACTION on
                                                          outgoing packet to a
                                                          HiGig Port. */
#define BCM_L3_FLAGS2_FIELD_ONLY            (1 << 1)   /* Specific for Field
                                                          Nexthop. */
#define BCM_L3_FLAGS2_UNDERLAY              (1 << 2)   /* Create underlay next
                                                          hop on a virtual port. */
#define BCM_L3_FLAGS2_SRC_DST_MAC_SWAP      (1 << 3)   /* Swap outgoing packets
                                                          L2 header SRC/DST MAC
                                                          fields, for IFP L3
                                                          egress objects only. */
#define BCM_L3_FLAGS2_EGRESS_WIDE           (1 << 4)   /* Configure egress wide
                                                          entry. */
#define BCM_L3_FLAGS2_NO_ECMP_OVERLAP       (1 << 5)   /* Create next hop on the
                                                          index without
                                                          overlapping ECMP
                                                          indexes */
#define BCM_L3_FLAGS2_SCALE_ROUTE           (1 << 6)   /* Scale the number of
                                                          routes. */
#define BCM_L3_FLAGS2_RAW_ENTRY             (1 << 7)   /* Insert raw data
                                                          payload */
#define BCM_L3_FLAGS2_SKIP_HIT_CLEAR        (1 << 8)   /* Skip hit clear when
                                                          creating a FEC to
                                                          enhance time
                                                          performance. */
#define BCM_L3_FLAGS2_ROUTE_NO_MOVE         (1 << 9)   /* A flag only to be used
                                                          with BCM_L3_REPLACE
                                                          flag for avoiding host
                                                          entries move from the
                                                          route table to the
                                                          host table. */
#define BCM_L3_FLAGS2_MPLS_PHP              (1 << 10)  /* Specific nexthop for
                                                          MPLS PHP. */
#define BCM_L3_FLOW_ONLY                    (1 << 11)  /* Specific for FLOW
                                                          Nexthop */
#define BCM_L3_MPLS_GRE_LABEL               (1 << 12)  /* Specific for MPLS over
                                                          GRE label */
#define BCM_L3_FLAGS2_XFLOW_MACSEC_ENCRYPT  (1 << 13)  /* Indicates the next hop
                                                          entry is for MACsec
                                                          encrypt */
#define BCM_L3_FLAGS2_XFLOW_MACSEC_DECRYPT  (1 << 14)  /* Indicates the next hop
                                                          entry is for MACsec
                                                          decrypt */
#define BCM_L3_FLAGS2_INGRESS_URPF_MODE     (1 << 15)  /* Enable fec rpf mode
                                                          configuration. */
#define BCM_L3_FLAGS2_DECAP_ROE_HEADER      (1 << 16)  /* Indicates the
                                                          decapsulation of
                                                          ROE/ROE Custom Header */
#define BCM_L3_FLAGS2_FCOE_ONLY             (1 << 17)  /* Specific for FCOE
                                                          Nexthop. */
#define BCM_L3_FLAGS2_VLAN_TRANSLATION      (1 << 18)  /* Indicates that
                                                          bcm_l3_egress_create
                                                          should allocate ARP+AC
                                                          entry in EEDB. */
#define BCM_L3_FLAGS2_SRC_MAC               (1 << 19)  /* Indicates that
                                                          bcm_l3_egress_create
                                                          should allocate ARP
                                                          with the ability to
                                                          set SA. */
#define BCM_L3_FLAGS2_EGRESS_STAT_ENABLE    (1 << 20)  /* Indicates statistics
                                                          enabled. */
#define BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP    (1 << 21)  /* Indicate that the used
                                                          FEC ID is overlapping
                                                          the ECMP IDs range. */
#define BCM_L3_FLAGS2_NON_PROXY_MODE        (1 << 22)  /* Indicate the
                                                          encapsulation of HG2
                                                          PPD-2 Header. */
#define BCM_L3_FLAGS2_MC_GROUP              (1 << 23)  /* Indicate that the
                                                          destination is a
                                                          multicast group. */
#define BCM_L3_FLAGS2_IFA_DST_PORT          (1 << 24)  /* Indicate that VN_TAG
                                                          fields need be
                                                          configured for IFA. */
#define BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED (1 << 25)  /* Indicate egress object
                                                          is pointed from EEDB
                                                          only and not from
                                                          FWD/ACL Dbs. */
#define BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS (1 << 26)  /* L3 object supports
                                                          VLAN Translation with
                                                          up to two VLAN tags. */
#define BCM_L3_FLAGS2_FWD_ONLY              (1 << 27)  /* Add route to FWD. */
#define BCM_L3_FLAGS2_RPF_ONLY              (1 << 28)  /* Add route to RPF. */
#define BCM_L3_FLAGS2_NO_PAYLOAD            (1 << 29)  /* Add route with no
                                                          payload. */
#define BCM_L3_FLAGS2_SRC_ROUTE             (1 << 30)  /* Add source route. */
#define BCM_L3_FLAGS2_FAILOVER_UNDERLAY     (1U << 31) /* Failover on the
                                                          underlay next hop. */

/* L3 module flags3. */
#define BCM_L3_FLAGS3_INT_ACTION            (1 << 0)   /* Enable the INT action. */
#define BCM_L3_FLAGS3_DO_NOT_CUT_THROUGH    (1 << 1)   /* Enable to indicate the
                                                          packet is not eligible
                                                          for cut-through. */
#define BCM_L3_FLAGS3_EGRESS_CASCADED       (1 << 2)   /* Enable to indicate the
                                                          egress object in
                                                          cascaded mode. */
#define BCM_L3_FLAGS3_DATA_MODE_FULL        (1 << 3)   /* Specify data mode of
                                                          an ALPM route entry as
                                                          full. */
#define BCM_L3_FLAGS3_DATA_MODE_REDUCED     (1 << 4)   /* Specify data mode of
                                                          an ALPM route entry as
                                                          reduced. */

/* L3 module Multicast flags. */
#define BCM_L3_MULTICAST_L2_DEST_PRESERVE   (1 << 0)   /* L2 multicast dest
                                                          address replacement
                                                          disable */
#define BCM_L3_MULTICAST_L2_SRC_PRESERVE    (1 << 1)   /* L2 multicast source
                                                          address replacement
                                                          disable */
#define BCM_L3_MULTICAST_L2_VLAN_PRESERVE   (1 << 2)   /* L2 multicast outer
                                                          vlan replacement
                                                          disable */
#define BCM_L3_MULTICAST_TTL_PRESERVE       (1 << 3)   /* L3 multicast TTL
                                                          decrement disable */
#define BCM_L3_MULTICAST_DEST_PRESERVE      (1 << 4)   /* L3 multicast dest
                                                          address replacement
                                                          disable */
#define BCM_L3_MULTICAST_SRC_PRESERVE       (1 << 5)   /* L3 multicast source
                                                          address replacement
                                                          disable */
#define BCM_L3_MULTICAST_VLAN_PRESERVE      (1 << 6)   /* L3 multicast outer
                                                          vlan replacement
                                                          disable */
#define BCM_L3_MULTICAST_L3_DROP            (1 << 7)   /* L3 multicast L3 drop */
#define BCM_L3_MULTICAST_L2_DROP            (1 << 8)   /* L3 multicast L2 drop */

/* L3 Ingress Interface flags. */
#define BCM_L3_INGRESS_WITH_ID              (1 << 0)   /* L3 Ingress ID is
                                                          provided. */
#define BCM_L3_INGRESS_REPLACE              (1 << 1)   /* Replace existing L3
                                                          Ingress entry. */
#define BCM_L3_INGRESS_GLOBAL_ROUTE         (1 << 2)   /* Allow Global Route on
                                                          L3 Ingress Interface. */
#define BCM_L3_INGRESS_DSCP_TRUST           (1 << 3)   /* Trust incoming DSCP on
                                                          L3 Ingress */
#define BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK (1 << 4)   /* Allow URPF Check on
                                                          Default Routes. */
#define BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST (1 << 5)   /* Disable L3 routing on
                                                          IPv4 UC packets */
#define BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST (1 << 6)   /* Disable L3 routing on
                                                          IPv4 MC packets */
#define BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST (1 << 7)   /* Disable L3 routing on
                                                          IPv6 UC packets */
#define BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST (1 << 8)   /* Disable L3 routing on
                                                          IPv6 MC packets */
#define BCM_L3_INGRESS_ROUTE_DISABLE_MPLS   (1 << 9)   /* Disable L3 routing on
                                                          MPLS packets */
#define BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE (1 << 10)  /* Disable using L3
                                                          interface as part of
                                                          IPMC forwarding lookup
                                                          key */
#define BCM_L3_INGRESS_IPV6_ROUTING_HEADER_TYPE_0_DROP (1 << 11)  /* Drop Ipv6 Header Type
                                                          0 packets */
#define BCM_L3_INGRESS_UNKNOWN_SRC_TOCPU    (1 << 12)  /* Unresolved source for
                                                          L3 send to CPU */
#define BCM_L3_INGRESS_UNKNOWN_IP4_MCAST_TOCPU (1 << 13)  /* Unknown IPMC (IPMC
                                                          miss) for IPv4 packets
                                                          sent to CPU */
#define BCM_L3_INGRESS_UNKNOWN_IP6_MCAST_TOCPU (1 << 14)  /* Unknown IPMC (IPMC
                                                          miss) for IPv6 packets
                                                          sent to CPU */
#define BCM_L3_INGRESS_ICMP_REDIRECT_TOCPU  (1 << 15)  /* Enables copy to cpu
                                                          when ICMP redirect
                                                          packet is needed */
#define BCM_L3_INGRESS_UNKNOWN_IPMC_AS_L2MC (1 << 16)  /* Enable L2MC processing
                                                          of Unknown IPMC (IPMC
                                                          miss) packets, instead
                                                          of flooding to vlan */
#define BCM_L3_INGRESS_SRC_DST_NAT_REALM_ID (1 << 17)  /* To set the nat id to
                                                          both source and
                                                          destination nat id. */
#define BCM_L3_INGRESS_L3_MCAST_L2          (1 << 18)  /* Set bridge V4MC
                                                          forwarding instead of
                                                          bridge forwarding in
                                                          case of IPV4 MC with
                                                          IPMC disable. */
#define BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY (1 << 19)  /* Enable L3 routing on
                                                          IPv4 double capacity
                                                          packets. */
#define BCM_L3_INGRESS_TUNNEL_TERM_ECN_MAP  (1 << 20)  /* To set the tunnel
                                                          termination ecn map
                                                          id. */
#define BCM_L3_INGRESS_MPLS_INTF_NAMESPACE  (1 << 21)  /* Indicates that if the
                                                          next layer is MPLS,
                                                          lookup is in per-I/F
                                                          context. */
#define BCM_L3_INGRESS_INT                  (1 << 22)  /* INT enable for L3
                                                          interface. */
#define BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK (1 << 23)  /* Set bridge fallback in
                                                          case of Unknown IPMC
                                                          (IPMC miss) packets. */
#define BCM_L3_INGRESS_PIM_BIDIR            (1 << 24)  /* Classify incoming
                                                          multicast packets as
                                                          PIM-BIDIR multicast.
                                                          Otherwise, incomming
                                                          multicast packet are
                                                          classified as PIM-SIM. */
#define BCM_L3_INGRESS_ROUTE_ENABLE_UNKNOWN (1 << 25)  /* Enable the routing
                                                          over an unknown layer,
                                                          used for tunnel
                                                          termination where the
                                                          next layer is yet to
                                                          be parsed. */
#define BCM_L3_INGRESS_NSH_OVER_L2          (1 << 26)  /* Enable NSH over L2 for
                                                          L3 interface. */
#define BCM_L3_INGRESS_FIELD_COMPRESSION_ENABLE (1 << 27)  /* Enable the algorithmic
                                                          ACL lookup in LPM for
                                                          Field Compression. */
#define BCM_L3_INGRESS_VIP_ENABLED          (1 << 28)  /* Indicates that virtual
                                                          IP routing is enabled
                                                          on the interface. */
#define BCM_L3_INGRESS_IFA                  (1 << 29)  /* Enable IFA. */
#define BCM_L3_INGRESS_IOAM                 (1 << 30)  /* Enable IOAM. */
#define BCM_L3_INGRESS_IP6_L3_MCAST_L2      (1U << 31) /* Set bridge V6MC
                                                          forwarding type
                                                          instead of bridge in
                                                          case of IPV6 MC with
                                                          IPMC disable. */

/* L3 Interface QOS flags. */
#define BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY (1 << 0)   /* Copy Outer Vlan .1p
                                                          from incoming packet
                                                          .1p. */
#define BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET  (1 << 1)   /* Set Outer Vlan .1p
                                                          from table entry. */
#define BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK (1 << 2)   /* Remark Outer Vlan .1p
                                                          from PHB. */
#define BCM_L3_INTF_QOS_INNER_VLAN_PRI_COPY (1 << 3)   /* Copy Inner Vlan .1p
                                                          from incoming packet
                                                          .1p. */
#define BCM_L3_INTF_QOS_INNER_VLAN_PRI_SET  (1 << 4)   /* Set Inner Vlan .1p
                                                          from table entry. */
#define BCM_L3_INTF_QOS_INNER_VLAN_PRI_REMARK (1 << 5)   /* Remark Outer Vlan .1p
                                                          from PHB. */
#define BCM_L3_INTF_QOS_DSCP_COPY           (1 << 6)   /* Use existing DSCP */
#define BCM_L3_INTF_QOS_DSCP_SET            (1 << 7)   /* Set DSCP from table
                                                          entry. */
#define BCM_L3_INTF_QOS_DSCP_REMARK         (1 << 8)   /* Remark DSCP from PHB. */

/* L3 Egress Interface flags. */
#define BCM_L3_INTF_INNER_VLAN_DO_NOT_MODIFY (1 << 0)   /* Do not modify inner
                                                          VLAN Tag */
#define BCM_L3_INTF_INNER_VLAN_ADD          (1 << 1)   /* Add Inner VLAN Tag */
#define BCM_L3_INTF_INNER_VLAN_REPLACE      (1 << 2)   /* Replace Inner VLAN Tag */
#define BCM_L3_INTF_INNER_VLAN_DELETE       (1 << 3)   /* Delete Inner VLAN Tag */
#define BCM_L3_INTF_UNDERLAY                (1 << 4)   /* Underlay egress
                                                          interface */
#define BCM_L3_INTF_IPMC_SAME_INTF_DROP     (1 << 5)   /* Drop IPMC packets
                                                          destined to the
                                                          interface same as the
                                                          incoming interface.
                                                          Only applicable with
                                                          BCM_L3_INTF_IPMC_SAME_INTF_CHECK. */
#define BCM_L3_INTF_TTL_CHECK               (1 << 6)   /* Enable TTL check */
#define BCM_L3_INTF_PACKET_DO_NOT_MODIFY    (1 << 7)   /* Do not modify packet. */
#define BCM_L3_INTF_OVERLAY_ENCAP           (1 << 9)   /*  L3 interface used for
                                                          overlay routing.
                                                          Relevant for intra DC
                                                          routed packets */
#define BCM_L3_INTF_NATIVE_ENCAP            (1 << 10)  /* L3 interface used for
                                                          native outing.
                                                          Relevant for intra DC
                                                          routed packets */
#define BCM_L3_INTF_IPMC_SAME_INTF_CHECK    (1 << 11)  /* Enable L3 same
                                                          interface check. The
                                                          traffic that has the
                                                          same ingress and
                                                          egress interface will
                                                          be either downgraded
                                                          to L2 switch or
                                                          dropped. */
#define BCM_L3_INTF_OVERLAY                 (1 << 12)  /* Overlay egress
                                                          interface */

/* Reserved VRF values . */
#define BCM_L3_VRF_INVALID      _SHR_L3_VRF_INVALID /* Invalid VRF ID. */
#define BCM_L3_VRF_OVERRIDE     _SHR_L3_VRF_OVERRIDE /* Matches before VRF specific
                                              entries. */
#define BCM_L3_VRF_GLOBAL       _SHR_L3_VRF_GLOBAL /* Matches after VRF specific
                                              entries. */
#define BCM_L3_VRF_DEFAULT      _SHR_L3_VRF_DEFAULT /* Default VRF ID. */
#define BCM_L3_VRF_ALL          _SHR_L3_VRF_ALL /* Represent all VRFs. */

/* Backward compatibility. */
#define BCM_L3_TUNNEL_TYPE_IP6_IP4  BCM_L3_TUNNEL_TYPE_6TO4 

/* Invalid L3 Ingress Interface */
#define BCM_L3_INGRESS_INTERFACE_INVALID    4095       /* Invalid L3 Ingress
                                                          Interface */

#define BCM_IP6_BYTE(a, n)      (((uint8 *) (a))[n]) 

#define BCM_IP6_HALF(a, n)      (((uint16 *) (a))[n]) 

#define BCM_IP6_WORD(a, n)      (((uint8 *) (a))[n]) 

#define BCM_IP4_MULTICAST(a)    \
    (((a) & 0xf0000000) == 0xe0000000) 

#define BCM_IP6_MULTICAST(a)    \
    ((BCM_IP6_BYTE(a, 0)) == 0xff) 

#define BCM_IP6_ADDR_EQ(a1, a2)  \
    (sal_memcmp((a1), (a2), BCM_IP6_ADDRLEN) == 0) 

#define BCM_IP6_LOOPBACK(a)     \
    ((BCM_IP6_WORD((a), 0) | BCM_IP6_WORD((a), 1) | \
      BCM_IP6_WORD((a), 2) | BCM_IP6_HALF((a), 6) | \
      BCM_IP6_BYTE((a), 14)) == 0 && BCM_IP6_BYTE((a), 15) == 1) 

#define BCM_IP6_V4_MAPPED(a)    \
    ((BCM_IP6_WORD((a), 0) | BCM_IP6_WORD((a), 1) | \
      BCM_IP6_HALF((a), 4)) == 0 && BCM_IP6_HALF((a), 5) == 0xffff) 

#define BCM_IP6_V4_COMPATIBLE(a)  \
    (BCM_IP6_WORD((a), 0) | BCM_IP6_WORD((a), 1) | BCM_IP6_WORD((a), 2) == 0) 

/* L3 ECMP options. */
#define BCM_L3_ECMP_O_CREATE_WITH_ID    (1 << 0)   /* Create an Egress ECMP
                                                      forwarding object with
                                                      assigned Identifier */
#define BCM_L3_ECMP_O_REPLACE           (1 << 1)   /* Replace/Update existing
                                                      Egress ECMP forwarding
                                                      object. */

/* L3 Egress Interface flags. */
#define BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY (1 << 0)   /* Match on the hash key
                                                          value */
#define BCM_L3_ECMP_RESILIENT_MATCH_INTF    (1 << 1)   /* Match on the interface
                                                          value */
#define BCM_L3_ECMP_RESILIENT_MATCH_ECMP    (1 << 2)   /* Match on the ECMP-Id
                                                          value */
#define BCM_L3_ECMP_RESILIENT_DELETE        (1 << 3)   /* Delete matched entries */
#define BCM_L3_ECMP_RESILIENT_REPLACE       (1 << 4)   /* Replace matched
                                                          entries with
                                                          replace_entry */
#define BCM_L3_ECMP_RESILIENT_PCC           (1 << 5)   /* Add/Replace matched
                                                          entries with
                                                          replace_entry in LEM
                                                          PCC State table */
#define BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL  (1 << 6)   /* Add/Delete matched
                                                          entries with
                                                          replace_entry in KBP
                                                          PCC table */
#define BCM_L3_ECMP_RESILIENT_COUNT         (1 << 7)   /* Count matched entries
                                                          in num_entries */

/* L3 Interface QOS setting. */
typedef struct bcm_l3_intf_qos_s {
    uint32 flags;   /* See BCM_L3_INTF_QOS_XXX flag definitions. */
    int qos_map_id; /* QOS Map ID. */
    uint8 pri;      /* Packet .1p. */
    uint8 cfi;      /* Packet CFI. */
    int dscp;       /* Packet DSCP. */
} bcm_l3_intf_qos_t;

/* 
 * L3 Interface Structure.
 * 
 * Contains information required for manipulating L3 interfaces.
 */
typedef struct bcm_l3_intf_s {
    uint32 l3a_flags;                   /* See BCM_L3_FLAGS_XXX flags
                                           definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_XXX flags
                                           definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_if_t l3a_intf_id;               /* Interface ID. */
    bcm_mac_t l3a_mac_addr;             /* MAC address. */
    bcm_vlan_t l3a_vid;                 /* VLAN ID. */
    bcm_vlan_t l3a_inner_vlan;          /* Inner vlan for double tagged packets. */
    int l3a_tunnel_idx;                 /* Tunnel (initiator) index. */
    int l3a_ttl;                        /* TTL threshold. */
    int l3a_mtu;                        /* MTU. */
    int l3a_mtu_forwarding;             /* Forwarding Layer MTU. */
    bcm_if_group_t l3a_group;           /* Interface group number. */
    bcm_l3_intf_qos_t vlan_qos;         /* Outer-Vlan QoS Setting. */
    bcm_l3_intf_qos_t inner_vlan_qos;   /* Inner-Vlan QoS Setting. */
    bcm_l3_intf_qos_t dscp_qos;         /* DSCP QoS Setting. */
    int l3a_intf_class;                 /* L3 interface class ID */
    int l3a_ip4_options_profile_id;     /* IP4 Options handling Profile ID */
    int l3a_nat_realm_id;               /* Realm id of the interface for NAT */
    uint16 outer_tpid;                  /* TPID value */
    uint32 l3a_intf_flags;              /* See BCM_L3_INTF_XXX flag definitions. */
    uint8 native_routing_vlan_tags;     /* Set number of VLAN tags expected when
                                           interface is used for native routing */
    bcm_gport_t l3a_source_vp;          /* Source virtual port in overlay
                                           domain. SVP is used for deriving port
                                           properties in the egress device when
                                           packets are sent over HG flow. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    uint8 l3a_ttl_dec;                  /* TTL decrement value. */
    int opaque_ctrl_id;                 /* Opqaue control ID for EFP key
                                           selection. */
    uint32 l3a_mpls_flags;              /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t l3a_mpls_label;    /* MPLS VC label. */
    uint8 l3a_mpls_ttl;                 /* MPLS TTL threshold. */
    uint8 l3a_mpls_exp;                 /* MPLS Exp. */
} bcm_l3_intf_t;

/* Invalid values for L3 ECMP dynamic load balancing parameters. */
#define BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID -1         /* Invalid value for
                                                          dynamic_scaling_factor. */
#define BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID -1         /* Invalid value for
                                                          dynamic_load_weight. */
#define BCM_L3_ECMP_DYNAMIC_QUEUE_SIZE_WEIGHT_INVALID -1         /* Invalid value for
                                                          dynamic_queue_size_weight. */

/* L3 Ingress Interface URPF Mode setting. */
typedef enum bcm_l3_ingress_urpf_mode_e {
    bcmL3IngressUrpfDisable = 0,    /* Disable unicast RPF. */
    bcmL3IngressUrpfLoose = 1,      /* Loose mode Unicast RPF. */
    bcmL3IngressUrpfStrict = 2      /* Strict mode Unicast RPF. */
} bcm_l3_ingress_urpf_mode_t;

/* 
 * L3 Egress Structure.
 * 
 * Description of an L3 forwarding destination.
 */
typedef struct bcm_l3_egress_s {
    uint32 flags;                       /* Interface flags (BCM_L3_TGID,
                                           BCM_L3_L2TOCPU). */
    uint32 flags2;                      /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    uint32 flags3;                      /* See BCM_L3_FLAGS3_xxx flag
                                           definitions. */
    bcm_if_t intf;                      /* L3 interface (source MAC, tunnel). */
    bcm_mac_t mac_addr;                 /* Next hop forwarding destination mac. */
    bcm_vlan_t vlan;                    /* Next hop vlan id. */
    bcm_module_t module; 
    bcm_port_t port;                    /* Port packet switched to (if
                                           !BCM_L3_TGID). */
    bcm_trunk_t trunk;                  /* Trunk packet switched to (if
                                           BCM_L3_TGID). */
    uint32 mpls_flags;                  /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action; /* MPLS action. */
    int mpls_qos_map_id;                /* MPLS EXP map ID. */
    int mpls_ttl;                       /* MPLS TTL threshold. */
    uint8 mpls_pkt_pri;                 /* MPLS Packet Priority Value. */
    uint8 mpls_pkt_cfi;                 /* MPLS Packet CFI Value. */
    uint8 mpls_exp;                     /* MPLS Exp. */
    int qos_map_id;                     /* General QOS map id */
    bcm_if_t encap_id;                  /* Encapsulation index. */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_if_t failover_if_id;            /* Failover Egress Object index. */
    bcm_multicast_t failover_mc_group;  /* Failover Multicast Group. */
    int dynamic_scaling_factor;         /* Scaling factor for dynamic load
                                           balancing thresholds. */
    int dynamic_load_weight;            /* Weight of traffic load in determining
                                           a dynamic load balancing member's
                                           quality. */
    int dynamic_queue_size_weight;      /* Weight of queue size in determining a
                                           dynamic load balancing member's
                                           quality. */
    int intf_class;                     /* L3 interface class ID */
    uint32 multicast_flags;             /* BCM_L3_MULTICAST flag definitions. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    bcm_vntag_t vntag;                  /* VNTAG. */
    bcm_vntag_action_t vntag_action;    /* VNTAG action. */
    bcm_etag_t etag;                    /* ETAG. */
    bcm_etag_action_t etag_action;      /* ETAG action. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    uint32 flow_label_option_handle;    /* FLOW option handle for egress label
                                           flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    int counting_profile;               /* counting profile. If not required,
                                           set it to
                                           BCM_STAT_LIF_COUNTING_PROFILE_NONE */
    int mpls_ecn_map_id;                /* IP ECN/INT CN to MPLS EXP map ID. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* fec rpf mode. */
    bcm_multicast_t mc_group;           /* Multicast Group. */
    bcm_mac_t src_mac_addr;             /* Source MAC Address. */
    bcm_gport_t hierarchical_gport;     /* Hierarchical TM-Flow. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_gport_t vlan_port_id;           /* Pointer to egress vlan translation
                                           lif entry in EEDB. */
    int replication_id;                 /* Replication copy ID of multicast
                                           Egress object. */
    int mtu;                            /* MTU. */
    int nat_realm_id;                   /* NAT destination realm ID. */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
    int egress_opaque_ctrl_id;          /* Egress opaque control ID. */
} bcm_l3_egress_t;

#define BCM_L3_INT_MAX_COSQ_STAT    8          /* Max number of INT COSQ stat. */

/* INT COSQ stat types. */
typedef enum bcm_l3_int_cosq_stat_e {
    bcmL3IntCosqStatNone = 0,           /* No stat. */
    bcmL3IntCosqStatCurrentBytes = 1,   /* Current used buffer in bytes. */
    bcmL3IntCosqStatMaxBytes = 2,       /* Maximum used buffer in bytes. */
    bcmL3IntCosqStatCurrentAvailableBytes = 3, /* Current available buffer in bytes. */
    bcmL3IntCosqStatMinAvailableBytes = 4, /* Minimum available buffer in bytes. */
    bcmL3IntCosqStatCount = 5           /* Always last. Not a usable value. */
} bcm_l3_int_cosq_stat_t;

/* 
 * L3 Ingress Structure.
 * 
 * Description of an L3 Ingress interface.
 */
typedef struct bcm_l3_ingress_s {
    uint32 flags;                       /* Interface flags. */
    bcm_vrf_t vrf;                      /* Virtual router id. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* URPF mode setting for L3 Ingress
                                           Interface. */
    int intf_class;                     /* Classification class ID. */
    bcm_vlan_t ipmc_intf_id;            /* IPMC L2 distribution Vlan. */
    int qos_map_id;                     /* QoS DSCP priority map. */
    int ip4_options_profile_id;         /* IP4 Options handling Profile ID */
    int nat_realm_id;                   /* Realm id of the interface for NAT */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map id */
    uint32 intf_class_route_disable;    /* routing enablers bit map in rif
                                           profile */
    int oam_default_profile;            /* OAM profile for RIF */
    uint32 hash_layers_disable;         /* load balancing disable hash layers
                                           bit map using the
                                           BCM_HASH_LAYER_XXX_DISABLE flags */
    int opaque_ctrl_id;                 /* Opaque control ID. */
    bcm_l3_int_cosq_stat_t int_cosq_stat[BCM_L3_INT_MAX_COSQ_STAT]; /* Select INT COSQ stat. */
} bcm_l3_ingress_t;

/* 
 * L3 Host Structure.
 * 
 * Contains information required for manipulating L3 host table entries.
 * 
 * The BCM_L3_IP6 flag in l3a_flags must be set to specify whether the
 * IPv4 or IPv6 addresses are valid.
 */
typedef struct bcm_l3_host_s {
    uint32 l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_ip_t l3a_ip_addr;               /* Destination host IP address (IPv4). */
    bcm_ip6_t l3a_ip6_addr;             /* Destination host IP address (IPv6). */
    bcm_cos_t l3a_pri;                  /* New priority in packet. */
    bcm_if_t l3a_intf;                  /* L3 intf associated with this address. */
    bcm_if_t l3a_ul_intf;               /* Underlay L3 egress object associated
                                           with this address. */
    bcm_mac_t l3a_nexthop_mac;          /* Next hop MAC addr. */
    bcm_module_t l3a_modid;             /* Module ID packet is switched to. */
    bcm_port_t l3a_port_tgid;           /* Port/TGID packet is switched to. */
    bcm_port_t l3a_stack_port;          /* Used if modid not local (Strata
                                           Only). */
    int l3a_ipmc_ptr;                   /* Pointer to IPMC table. */
    int l3a_lookup_class;               /* Classification lookup class ID. */
    bcm_if_t encap_id;                  /* Encapsulation index. */
    bcm_if_t native_intf;               /*  L3 native interface (source MAC). */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    int l3a_ipmc_ptr_l2;                /* Pointer to IPMC table for L2
                                           recipients if TTL/RPF check fails. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    int l3a_mtu;                        /* Multicast packet MTU. */
    int l3a_opaque_ctrl_id;             /* Opaque control ID. */
} bcm_l3_host_t;

/* Backward compatibility. */
typedef bcm_l3_host_t bcm_l3_ip_t;

/* 
 * L3 Key Structure.
 * 
 * Defines L3 table hash key structure.
 * 
 *   - For unicast addresses, key is DIP address or SIP to update HIT bit
 *   - For multicast addresses, the key is either IPMC group address
 *     (IPMC group address + src IP address), or
 *     (IPMC group address + src IP address + VID)
 *     depending on the chip being used and the (G, S, V) mode enable.
 * 
 * The BCM_L3_IP6 flag in l3k_flags is set to specify whether the IPv4 or
 * IPv6 addresses are valid.
 */
typedef struct bcm_l3_key_s {
    uint32 l3k_flags;           /* Set BCM_L3_IP6 for IPv6, default is IPv4, no
                                   other flags are relevant. */
    bcm_ip_t l3k_ip_addr;       /* Destination IP address (IPv4). */
    bcm_ip6_t l3k_ip6_addr;     /* Destination IP address (IPv6). */
    bcm_ip_t l3k_sip_addr;      /* Source IP address (IPv4). */
    bcm_ip6_t l3k_sip6_addr;    /* Source IP address (IPv6). */
    bcm_vlan_t l3k_vid;         /* VLAN ID. */
    bcm_vrf_t l3k_vrf;          /* BCM5660x: Virtual router instance. */
} bcm_l3_key_t;

/* 
 * L3 Route Structure
 * 
 * Contains information required for manipulating L3 route table entries.
 * 
 * The BCM_L3_IP6 flag in l3a_flags must be set to specify whether the
 * IPv4 or IPv6 addresses are valid.
 */
typedef struct bcm_l3_route_s {
    uint32 l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    uint32 l3a_flags3;                  /* See BCM_L3_FLAGS3_xxx flag
                                           definitions. */
    uint32 l3a_ipmc_flags;              /* Used for multicast route entry. See
                                           BCM_IPMC_xxx flag definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_ip_t l3a_subnet;                /* IP subnet address (IPv4). */
    bcm_ip6_t l3a_ip6_net;              /* IP subnet address (IPv6). */
    bcm_ip_t l3a_ip_mask;               /* IP subnet mask (IPv4). */
    bcm_ip6_t l3a_ip6_mask;             /* IP subnet mask (IPv6). */
    bcm_if_t l3a_intf;                  /* L3 interface associated with route. */
    bcm_if_t l3a_ul_intf;               /* Underlay L3 egress object associated
                                           with this address. */
    bcm_ip_t l3a_nexthop_ip;            /* Next hop IP address (XGS1/2, IPv4). */
    bcm_mac_t l3a_nexthop_mac;          /* Next hop MAC address. */
    bcm_module_t l3a_modid;             /* Module ID. */
    bcm_port_t l3a_port_tgid;           /* Port or trunk group ID. */
    bcm_port_t l3a_stack_port;          /* Used if modid is not local (Strata
                                           Only). */
    bcm_vlan_t l3a_vid;                 /* BCM5695 only - for per-VLAN def
                                           route. */
    bcm_cos_t l3a_pri;                  /* Priority (COS). */
    uint32 l3a_tunnel_option;           /* Tunnel option value. */
    bcm_mpls_label_t l3a_mpls_label;    /* MPLS label. */
    int l3a_lookup_class;               /* Classification class ID. */
    bcm_if_t l3a_expected_intf;         /* Expected L3 Interface used for
                                           multicast RPF check */
    int l3a_rp;                         /* Rendezvous point ID */
    bcm_multicast_t l3a_mc_group;       /* L3 Multicast group index */
    bcm_gport_t l3a_expected_src_gport; /* L3 Multicast group expected source
                                           port/trunk */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    int l3a_mtu;                        /* Multicast packet MTU. */
    int l3a_int_action_profile_id;      /* Profile ID for metadata generation
                                           and packet updating after metadata
                                           insertion. */
    int l3a_int_opaque_data_profile_id; /* Profile ID to the opaque data for
                                           metadata construction. */
    int l3a_opaque_ctrl_id;             /* Opaque control ID. */
} bcm_l3_route_t;

/* 
 * L3 Information Structure.
 * 
 * Contains information about L3 hardware capacity and utilization.
 * 
 * Note that in chips that implement the longest prefix match route table
 * as a trie, one IP route may consume up to 128 table entries in the
 * worst case.
 */
typedef struct bcm_l3_info_s {
    int l3info_max_vrf;             /* Maximum number of virtual routers
                                       allowed. */
    int l3info_used_vrf;            /* Number of virtual routers created so far. */
    int l3info_max_intf;            /* Maximum L3 interfaces the chip supports. */
    int l3info_max_intf_group;      /* Maximum L3 interface groups the chip
                                       supports. */
    int l3info_max_host;            /* L3 host table size(unit is IPv4 unicast). */
    int l3info_max_route;           /* L3 route table size (unit is IPv4 route). */
    int l3info_max_ecmp_groups;     /* Maximum ECMP groups allowed. */
    int l3info_max_ecmp;            /* Maximum ECMP paths allowed. */
    int l3info_used_intf;           /* L3 interfaces used. */
    int l3info_used_host;           /* L3 host entries used. */
    int l3info_used_route;          /* L3 route entries used. */
    int l3info_max_lpm_block;       /* Maximum LPM blocks. */
    int l3info_used_lpm_block;      /* LPM blocks used. */
    int l3info_max_l3;              /* Superseded by l3info_max_host. */
    int l3info_max_defip;           /* Superseded by l3info_max_route. */
    int l3info_used_l3;             /* Superseded by l3info_occupied_host. */
    int l3info_used_defip;          /* Superseded by l3info_occupied_route. */
    int l3info_max_nexthop;         /* Maximum NextHops. */
    int l3info_used_nexthop;        /* NextHops used. */
    int l3info_max_tunnel_init;     /* Maximum IPV4 tunnels that can be
                                       initiated. */
    int l3info_used_tunnel_init;    /* Number of active IPV4 tunnels initiated. */
    int l3info_max_tunnel_term;     /* Maximum IPV4 tunnels that can be
                                       terminated. */
    int l3info_used_tunnel_term;    /* Number of active IPV4 tunnels terminated. */
    int l3info_used_host_ip4;       /* L3 host entries used by IPv4. */
    int l3info_used_host_ip6;       /* L3 host entries used by IPv6. */
} bcm_l3_info_t;

/* Renamed bcm_l3_info_t fields. */
#define l3info_occupied_intf    l3info_used_intf 
#define l3info_occupied_host    l3info_used_host 
#define l3info_occupied_route   l3info_used_route 
#define l3info_occupied_l3      l3info_used_l3 
#define l3info_occupied_defip   l3info_used_defip 
#define bcm_l3_detach           bcm_l3_cleanup 

/* L3 DGM structure */
typedef struct bcm_l3_dgm_s {
    uint32 threshold;   /* Indicates the primary path is always selected when
                           primary optimal quality band is above this threshold. */
    uint32 cost;        /* Indicates the quality band cost of switching over to
                           alternate path. */
    uint32 bias;        /* Indicates the quality band bias in favor of alternate
                           path. */
} bcm_l3_dgm_t;

/* L3 ECMP URPF mode setting */
typedef enum bcm_l3_ecmp_urpf_mode_e {
    bcmL3EcmpUrpfInterfaceDefault = 0,  /* Use the L3 interface URPF mode. */
    bcmL3EcmpUrpfLoose = 1,             /* Use loose ECMP URPF mode. */
    bcmL3EcmpUrpfStrictEm = 2           /* Use strict EM ECMP RPF mode. */
} bcm_l3_ecmp_urpf_mode_t;

/* L3 ECMP tunnel priority - number of priorities. */
typedef enum bcm_l3_ecmp_tunnel_priority_mode_e {
    bcmL3EcmpTunnelPriorityModeDisable = 0, /*  No priorities options. */
    bcmL3EcmpTunnelPriorityModeTwoPriorities = 1, /* Two priorities. */
    bcmL3EcmpTunnelPriorityModeFourPriorities = 2, /* Four priorities. */
    bcmL3EcmpTunnelPriorityModeEightPriorities = 3 /* Eight priorities. */
} bcm_l3_ecmp_tunnel_priority_mode_t;

/* L3 ECMP tunnel priority. */
typedef struct bcm_l3_tunnel_priority_info_s {
    bcm_l3_ecmp_tunnel_priority_mode_t mode; /* Tunnel Priority mode. */
    int map_profile;                    /* Tunnel Priority Map profile. */
    int index;                          /* Index to the Tunnel Priority table to
                                           update. */
} bcm_l3_tunnel_priority_info_t;

/* L3 ECMP structure */
typedef struct bcm_l3_egress_ecmp_s {
    uint32 flags;                       /* See BCM_L3_xxx flag definitions. */
    bcm_if_t ecmp_intf;                 /* L3 interface ID pointing to egress
                                           ecmp object. */
    int max_paths;                      /* Max number of paths in ECMP group. If
                                           max_paths <= 0, the default max path
                                           which can be set by the API
                                           bcm_l3_route_max_ecmp_set will be
                                           picked. */
    uint32 ecmp_group_flags;            /* BCM_L3_ECMP_xxx flags. */
    uint32 dynamic_mode;                /* Dynamic load balancing mode. See
                                           BCM_L3_ECMP_DYNAMIC_MODE_xxx
                                           definitions. */
    uint32 dynamic_size;                /* Number of flows for dynamic load
                                           balancing. Valid values are 512, 1k,
                                           doubling up to 32k */
    uint32 dynamic_age;                 /* Inactivity duration, in microseconds. */
    uint32 dynamic_load_exponent;       /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical member
                                           load. */
    uint32 dynamic_expected_load_exponent; /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical expected
                                           member load. */
    bcm_l3_dgm_t dgm;                   /* DGM properties */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_l3_ecmp_urpf_mode_t rpf_mode;   /* Define RPF mode for the ECMP group. */
    bcm_l3_tunnel_priority_info_t tunnel_priority; /* ECMP tunnel priority. */
} bcm_l3_egress_ecmp_t;

/* L3 ECMP member structure */
typedef struct bcm_l3_ecmp_member_s {
    uint32 flags;                   /* Member flag. */
    bcm_if_t egress_if;             /* L3 interface ID pointing to Egress
                                       Forwarding Object. */
    bcm_if_t egress_if_2;           /* L3 interface ID pointing to Egress
                                       Forwarding Object. */
    bcm_failover_t failover_id;     /* Failover Object Identifier. */
    bcm_if_t failover_egress_if;    /* Failover Egress L3 Interface ID. */
    int status;                     /* Member status. */
    int weight;                     /* Member weight value. */
} bcm_l3_ecmp_member_t;

/* Resilient ecmp entry */
typedef struct bcm_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;            /* Hash key. */
    bcm_l3_egress_ecmp_t ecmp;  /* ECMP. */
    bcm_if_t intf;              /* L3 interface. */
} bcm_l3_egress_ecmp_resilient_entry_t;

/* Actions to take for DLB flow monitored packets. */
#define BCM_L3_ECMP_DLB_MON_COPY_TO_CPU     1          /* Copy DLB monitored
                                                          packets to CPU */
#define BCM_L3_ECMP_DLB_MON_MIRROR          2          /* Mirror DLB monitored
                                                          packets */
#define BCM_L3_ECMP_DLB_MON_COPY_TO_CPU_AND_MIRROR 3          /* Perform Copy to CPU
                                                          and mirroring of DLB
                                                          monitored packets */
#define BCM_L3_ECMP_DLB_MON_ACTION_NONE     4          /* Do not perform Copy to
                                                          CPU or mirroring(take
                                                          no action) */
#define BCM_L3_ECMP_DLB_MON_TRACE           5          /* Set trace event for
                                                          DLB monitored packets */

/* Per DLB configuration of monitoring parameters. */
typedef struct bcm_l3_ecmp_dlb_mon_cfg_s {
    uint32 sample_rate;     /* Rate of sampling DLB monitored packets */
    int action;             /* Either BCM_L3_ECMP_DLB_MON_COPY_TO_CPU or
                               BCM_L3_ECMP_DLB_MON_MIRROR or
                               BCM_L3_ECMP_DLB_MON_COPY_TO_CPU_AND_MIRROR or
                               BCM_L3_ECMP_DLB_MON_ACTION_NONE or
                               BCM_L3_ECMP_DLB_MON_TRACE */
    int enable;             /* Enable monitoring of DLB associated with the ECMP
                               group. 1 = Enable monitoring for DLB associated
                               with ecmp, 0 = Do not monitor DLB */
    uint32 sample_rate_64;  /* Rate of sampling DLB monitored packets with 64
                               bits */
} bcm_l3_ecmp_dlb_mon_cfg_t;

/* 
 * L3 ECMP flags. MSB is reserved and used internally for Resilient
 * Hashing in 56960
 */
#define BCM_L3_ECMP_DYNAMIC_LOAD_DECREASE_RESET 0x01       /* If set, historical
                                                          member load is reset
                                                          to the instantaneous
                                                          member load if the
                                                          latter is smaller. */
#define BCM_L3_ECMP_DYNAMIC_EXPECTED_LOAD_DECREASE_RESET 0x02       /* If set, historical
                                                          expected member load
                                                          is reset to the
                                                          instantaneous expected
                                                          member load if the
                                                          latter is smaller. */
#define BCM_L3_ECMP_PATH_NO_SORTING         0x04       /* If set, the members of
                                                          the ECMP group won't
                                                          be resorted. */
#define BCM_L3_ECMP_OVERLAY                 0x08       /* If set, the members of
                                                          the ECMP group are
                                                          configured at ECMP
                                                          Level 1. */
#define BCM_L3_ECMP_UNDERLAY                0x10       /* If set, the members of
                                                          the ECMP group are
                                                          configured at ECMP
                                                          Level 2. */
#define BCM_L3_ECMP_LARGE_TABLE             0x20       /* If selected a large
                                                          members group table is
                                                          used for the resilient
                                                          hashing for better
                                                          member distribution. */
#define BCM_L3_ECMP_WEIGHTED                0x40       /* If set, indicates it's
                                                          a weighted ECMP group
                                                          and members count
                                                          (intf_count) value
                                                          must be in powers-of-2
                                                          and supported values
                                                          are 256, 512, 1024,
                                                          2048 and 4096. */
#define BCM_L3_ECMP_EXTENDED                0x80       /* Indicate ECMP extended
                                                          range (32k to 40k
                                                          values). */
#define BCM_L3_ECMP_MEMBER_WEIGHTED         0x100      /* If set, indicates it's
                                                          a weighted ECMP group
                                                          and each ECMP member
                                                          should be with a
                                                          weight. */
#define BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED BCM_L3_ECMP_WEIGHTED /* Same as
                                                          BCM_L3_WEIGHTED and
                                                          weighted ECMP is
                                                          achieved by member
                                                          replication. */

/* L3 ECMP dynamic load balancing modes. */
#define BCM_L3_ECMP_DYNAMIC_MODE_DISABLED   0          /* ECMP dynamic load
                                                          balancing disabled. */
#define BCM_L3_ECMP_DYNAMIC_MODE_NORMAL     1          /* ECMP dynamic load
                                                          balancing normal mode:
                                                          if inactivity duration
                                                          lapsed, use optimal
                                                          member, else use
                                                          assigned member. */
#define BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED   2          /* ECMP dynamic load
                                                          balancing assigned
                                                          mode: always use
                                                          assigned member. */
#define BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL    3          /* ECMP dynamic load
                                                          balancing optimal
                                                          mode: always use
                                                          optimal member */
#define BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT  4          /* ECMP resilient load
                                                          balancing mode:
                                                          minimize reassignment
                                                          of flows to members */
#define BCM_L3_ECMP_DYNAMIC_MODE_RANDOM     5          /* ECMP Randomized load
                                                          balancing mode */
#define BCM_L3_ECMP_DYNAMIC_MODE_ROUND_ROBIN 6          /* ECMP Round Robin load
                                                          balancing mode */
#define BCM_L3_ECMP_DYNAMIC_MODE_DGM        7          /* ECMP dynamic group
                                                          multipath mode: if
                                                          primary path is good
                                                          enough, follow primary
                                                          path, else follow
                                                          alternate path. */
#define BCM_L3_ECMP_DYNAMIC_MODE_VIP        8          /* ECMP dynamic group VIP
                                                          mode: Virtual IP L4
                                                          Load Balancer */

/* L3 ECMP dynamic load balancing EtherType eligibility flags. */
#define BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE 0x01       /* If set, the specified
                                                          EtherTypes are
                                                          eligible, else
                                                          ineligible. */
#define BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER 0x02       /* Use packet's inner
                                                          EtherType. */
#define BCM_L3_ECMP_DYNAMIC_ETHERTYPE_RESILIENT 0x04       /* If set, configure
                                                          EtherType eligibility
                                                          for resilient load
                                                          balancing, else for
                                                          dynamic load
                                                          balancing. */

/* L3 ECMP dynamic load balancing link status. */
#define BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_DOWN 0          /* Force down the link
                                                          status of an ECMP
                                                          dynamic load balancing
                                                          member. */
#define BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_UP 1          /* Force up the link
                                                          status of an ECMP
                                                          dynamic load balancing
                                                          member. */
#define BCM_L3_ECMP_DYNAMIC_MEMBER_HW       2          /* Let hardware determine
                                                          the link status of an
                                                          ECMP dynamic load
                                                          balancing member. */
#define BCM_L3_ECMP_DYNAMIC_MEMBER_HW_DOWN  3          /* Hardware indicates
                                                          down link status of an
                                                          ECMP dynamic load
                                                          balancing member. */
#define BCM_L3_ECMP_DYNAMIC_MEMBER_HW_UP    4          /* Hardware indicates up
                                                          link status of an ECMP
                                                          dynamic load balancing
                                                          member. */

/* L3 ECMP Member Flags. */
#define BCM_L3_ECMP_MEMBER_DGM_ALTERNATE    0x00000001 /* If set indicating ECMP
                                                          member is within
                                                          alternate path, else
                                                          within primary path */
#define BCM_L3_ECMP_MEMBER_FAILOVER_UNDERLAY 0x00000002 /* Failover on the
                                                          underlay next hop. */

/* bcm_l3_host_traverse_cb */
typedef int (*bcm_l3_host_traverse_cb)(
    int unit, 
    int index, 
    bcm_l3_host_t *info, 
    void *user_data);

/* bcm_l3_route_traverse_cb */
typedef int (*bcm_l3_route_traverse_cb)(
    int unit, 
    int index, 
    bcm_l3_route_t *info, 
    void *user_data);

/* bcm_l3_egress_traverse_cb */
typedef int (*bcm_l3_egress_traverse_cb)(
    int unit, 
    bcm_if_t intf, 
    bcm_l3_egress_t *info, 
    void *user_data);

/* bcm_l3_ingress_traverse_cb */
typedef int (*bcm_l3_ingress_traverse_cb)(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_ingress_t *ing_intf, 
    void *user_data);

/* bcm_l3_egress_multipath_traverse_cb */
typedef int (*bcm_l3_egress_multipath_traverse_cb)(
    int unit, 
    bcm_if_t mpintf, 
    int intf_count, 
    bcm_if_t *intf_array, 
    void *user_data);

/* bcm_l3_egress_ecmp_traverse_cb */
typedef int (*bcm_l3_egress_ecmp_traverse_cb)(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    int intf_count, 
    bcm_if_t *intf_array, 
    void *user_data);

/* bcm_l3_ecmp_traverse_cb */
typedef int (*bcm_l3_ecmp_traverse_cb)(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp_info, 
    int ecmp_member_count, 
    bcm_l3_ecmp_member_t *ecmp_member_array, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM L3 subsystem. */
extern int bcm_l3_init(
    int unit);

/* De-initialize the BCM L3 subsystem. */
extern int bcm_l3_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#define bcm_l3_detach           bcm_l3_cleanup 

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable/disable L3 function without clearing any L3 tables. */
extern int bcm_l3_enable_set(
    int unit, 
    int enable);

/* Get the status of hardware tables. */
extern int bcm_l3_info(
    int unit, 
    bcm_l3_info_t *l3info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_l3_intf_t structure. */
extern void bcm_l3_intf_t_init(
    bcm_l3_intf_t *intf);

/* Initialize a bcm_l3_intf_qos_t structure. */
extern void bcm_l3_intf_qos_t_init(
    bcm_l3_intf_qos_t *intf_qos);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a new L3 interface. */
extern int bcm_l3_intf_create(
    int unit, 
    bcm_l3_intf_t *intf);

/* Delete an L3 interface */
extern int bcm_l3_intf_delete(
    int unit, 
    bcm_l3_intf_t *intf);

/* Delete all L3 interfaces. */
extern int bcm_l3_intf_delete_all(
    int unit);

/* Search for L3 interface by MAC address and VLAN. */
extern int bcm_l3_intf_find(
    int unit, 
    bcm_l3_intf_t *intf);

/* Search for L3 interface by VLAN only. */
extern int bcm_l3_intf_find_vlan(
    int unit, 
    bcm_l3_intf_t *intf);

/* Given the L3 interface number, return the interface information. */
extern int bcm_l3_intf_get(
    int unit, 
    bcm_l3_intf_t *intf);

/* Attach counters entries to the given L3 egress interface. */
extern int bcm_l3_intf_stat_attach(
    int unit, 
    bcm_if_t intf_id, 
    uint32 stat_counter_id);

/* Detach counters entries from the given L3 egress interface. */
extern int bcm_l3_intf_stat_detach(
    int unit, 
    bcm_if_t intf_id);

/* Get stat counter id associated with given L3 egress interface */
extern int bcm_l3_intf_stat_id_get(
    int unit, 
    bcm_if_t intf_id, 
    uint32 *stat_counter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_l3_egress_t structure. */
extern void bcm_l3_egress_t_init(
    bcm_l3_egress_t *egr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an Egress forwarding object. */
extern int bcm_l3_egress_create(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_t *egr, 
    bcm_if_t *if_id);

/* Destroy an Egress forwarding object. */
extern int bcm_l3_egress_destroy(
    int unit, 
    bcm_if_t intf);

/* Get an Egress forwarding object. */
extern int bcm_l3_egress_get(
    int unit, 
    bcm_if_t intf, 
    bcm_l3_egress_t *egr);

/* Find an interface pointing to an Egress forwarding object. */
extern int bcm_l3_egress_find(
    int unit, 
    bcm_l3_egress_t *egr, 
    bcm_if_t *intf);

/* 
 * Traverse through the egress object table and run callback at each
 * valid entry.
 */
extern int bcm_l3_egress_traverse(
    int unit, 
    bcm_l3_egress_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Traverse through the egress ARP object table and run callback at each
 * valid entry.
 */
extern int bcm_l3_egress_arp_traverse(
    int unit, 
    bcm_l3_egress_traverse_cb trav_fn, 
    void *user_data);

/* Retrieve valid allocation for a given number of FECs. */
extern int bcm_l3_egress_allocation_get(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_t *egr, 
    uint32 nof_members, 
    bcm_if_t *if_id);

/* Create a Multipath Egress forwarding object. */
extern int bcm_l3_egress_multipath_create(
    int unit, 
    uint32 flags, 
    int intf_count, 
    bcm_if_t *intf_array, 
    bcm_if_t *mpintf);

/* Create a Multipath Egress forwarding object with specified path width. */
extern int bcm_l3_egress_multipath_max_create(
    int unit, 
    uint32 flags, 
    int max_paths, 
    int intf_count, 
    bcm_if_t *intf_array, 
    bcm_if_t *mpintf);

/* Destroy an Egress Multipath forwarding object. */
extern int bcm_l3_egress_multipath_destroy(
    int unit, 
    bcm_if_t mpintf);

/* Get an Egress Multipath forwarding object. */
extern int bcm_l3_egress_multipath_get(
    int unit, 
    bcm_if_t mpintf, 
    int intf_size, 
    bcm_if_t *intf_array, 
    int *intf_count);

/* 
 * Add an Egress forwarding object to an Egress Multipath forwarding
 * object.
 */
extern int bcm_l3_egress_multipath_add(
    int unit, 
    bcm_if_t mpintf, 
    bcm_if_t intf);

/* 
 * Delete an Egress forwarding object from an Egress Multipath forwarding
 * object.
 */
extern int bcm_l3_egress_multipath_delete(
    int unit, 
    bcm_if_t mpintf, 
    bcm_if_t intf);

/* Find an interface pointing to an Egress Multipath forwarding object. */
extern int bcm_l3_egress_multipath_find(
    int unit, 
    int intf_count, 
    bcm_if_t *intf_array, 
    bcm_if_t *mpintf);

/* 
 * Traverse through the multipath egress object table and run callback at
 * each valid entry.
 */
extern int bcm_l3_egress_multipath_traverse(
    int unit, 
    bcm_l3_egress_multipath_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize L3 Egress ECMP object structure. */
extern void bcm_l3_egress_ecmp_t_init(
    bcm_l3_egress_ecmp_t *ecmp);

/* Initialize L3 Egress ECMP member structure. */
extern void bcm_l3_ecmp_member_t_init(
    bcm_l3_ecmp_member_t *ecmp_member);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_create(
    int unit, 
    uint32 options, 
    bcm_l3_egress_ecmp_t *ecmp_info, 
    int ecmp_member_count, 
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_destroy(
    int unit, 
    bcm_if_t ecmp_group_id);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_get(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp_info, 
    int ecmp_member_size, 
    bcm_l3_ecmp_member_t *ecmp_member_array, 
    int *ecmp_member_count);

/* Add a member to an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_member_add(
    int unit, 
    bcm_if_t ecmp_group_id, 
    bcm_l3_ecmp_member_t *ecmp_member);

/* Delete a member from an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_member_delete(
    int unit, 
    bcm_if_t ecmp_group_id, 
    bcm_l3_ecmp_member_t *ecmp_member);

/* Delete all members from an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_member_delete_all(
    int unit, 
    bcm_if_t ecmp_group_id);

/* Find an Egress ECMP forwarding object. */
extern int bcm_l3_ecmp_find(
    int unit, 
    int ecmp_member_count, 
    bcm_l3_ecmp_member_t *ecmp_member_array, 
    bcm_l3_egress_ecmp_t *ecmp_info);

/* 
 * Traverse through the valid Egress ECMP forwarding objects and run
 * callback.
 */
extern int bcm_l3_ecmp_traverse(
    int unit, 
    bcm_l3_ecmp_traverse_cb trav_fn, 
    void *user_data);

/* Attach already created monitor entry to a L3 ECMP. */
extern int bcm_l3_ecmp_agm_attach(
    int unit, 
    bcm_if_t l3_ecmp_id, 
    bcm_switch_agm_id_t agm_id);

/* Detach a monitor entry from a L3 ECMP group */
extern int bcm_l3_ecmp_agm_detach(
    int unit, 
    bcm_if_t l3_ecmp_id, 
    bcm_switch_agm_id_t agm_id);

/* Retrieve a monitor id attached to a L3 ECMP group. */
extern int bcm_l3_ecmp_agm_attach_get(
    int unit, 
    bcm_if_t l3_ecmp_id, 
    bcm_switch_agm_id_t *agm_id);

/* Attach counters entries to the given L3 ECMP interface. */
extern int bcm_l3_ecmp_stat_attach(
    int unit, 
    bcm_if_t intf_id, 
    uint32 stat_counter_id);

/* Detach counters entries from the given L3 ECMP interface. */
extern int bcm_l3_ecmp_stat_detach(
    int unit, 
    bcm_if_t intf_id);

/* Get stat counter id associated with given L3 ECMP interface. */
extern int bcm_l3_ecmp_stat_id_get(
    int unit, 
    bcm_if_t intf_id, 
    uint32 *stat_counter_id);

/* Set flex counter object value for the given ECMP interface. */
extern int bcm_l3_ecmp_flexctr_object_set(
    int unit, 
    bcm_if_t intf_id, 
    uint32 value);

/* Get flex counter object value for the given ECMP interface. */
extern int bcm_l3_ecmp_flexctr_object_get(
    int unit, 
    bcm_if_t intf_id, 
    uint32 *value);

/* Create an Egress ECMP forwarding object. */
extern int bcm_l3_egress_ecmp_create(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    int intf_count, 
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_l3_egress_ecmp_destroy(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_l3_egress_ecmp_get(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    int intf_size, 
    bcm_if_t *intf_array, 
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_l3_egress_ecmp_add(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    bcm_if_t intf);

/* 
 * Delete an Egress forwarding object from an Egress ECMP forwarding
 * object.
 */
extern int bcm_l3_egress_ecmp_delete(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_l3_egress_ecmp_find(
    int unit, 
    int intf_count, 
    bcm_if_t *intf_array, 
    bcm_l3_egress_ecmp_t *ecmp);

/* 
 * Traverse through the valid Egress ECMP forwarding objects and run
 * callback.
 */
extern int bcm_l3_egress_ecmp_traverse(
    int unit, 
    bcm_l3_egress_ecmp_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Update an ECMP table that was already allocated, used in cases where
 * an ECMP have more than one table.
 */
extern int bcm_l3_egress_ecmp_tunnel_priority_set(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    int intf_count, 
    bcm_if_t *intf_array);

/* 
 * Set EtherType eligibility for ECMP dynamic load balancing or resilient
 * hashing.
 */
extern int bcm_l3_egress_ecmp_ethertype_set(
    int unit, 
    uint32 flags, 
    int ethertype_count, 
    int *ethertype_array);

/* 
 * Get EtherType eligibility for ECMP dynamic load balancing or resilient
 * hashing.
 */
extern int bcm_l3_egress_ecmp_ethertype_get(
    int unit, 
    uint32 *flags, 
    int ethertype_max, 
    int *ethertype_array, 
    int *ethertype_count);

/* 
 * Set dynamic load balancing (DLB) link status of an Egress forwarding
 * object
 * belonging to an ECMP group.
 */
extern int bcm_l3_egress_ecmp_member_status_set(
    int unit, 
    bcm_if_t intf, 
    int status);

/* 
 * Get dynamic load balancing (DLB) link status of an Egress forwarding
 * object
 * belonging to an ECMP group.
 */
extern int bcm_l3_egress_ecmp_member_status_get(
    int unit, 
    bcm_if_t intf, 
    int *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_l3_egress_ecmp_resilient_traverse_cb */
typedef int (*bcm_l3_egress_ecmp_resilient_traverse_cb)(
    int unit, 
    bcm_l3_egress_ecmp_resilient_entry_t *entry, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse through the resilient ecmp table and run callback at each
 * valid entry. Possible replacement of the matched entries.
 */
extern int bcm_l3_egress_ecmp_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry, 
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn, 
    void *user_data);

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_l3_egress_ecmp_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_l3_egress_ecmp_resilient_add(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_l3_egress_ecmp_resilient_delete(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *entry);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize L3 Ingress Interface object structure. */
extern void bcm_l3_ingress_t_init(
    bcm_l3_ingress_t *ing_intf);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create L3 Ingress Interface object. */
extern int bcm_l3_ingress_create(
    int unit, 
    bcm_l3_ingress_t *ing_intf, 
    bcm_if_t *intf_id);

/* Destroy L3 Ingress Interface object. */
extern int bcm_l3_ingress_destroy(
    int unit, 
    bcm_if_t intf_id);

/* Get an Ingress Interface object. */
extern int bcm_l3_ingress_get(
    int unit, 
    bcm_if_t intf, 
    bcm_l3_ingress_t *ing_intf);

/* Find an interface pointing to an Ingress Interface object. */
extern int bcm_l3_ingress_find(
    int unit, 
    bcm_l3_ingress_t *ing_intf, 
    bcm_if_t *intf_id);

/* 
 * Goes through ingress interface objects table and runs the user
 * callback
 * function at each valid ingress object entry passing back the
 * information for that object.
 */
extern int bcm_l3_ingress_traverse(
    int unit, 
    bcm_l3_ingress_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_l3_host_t structure */
extern void bcm_l3_host_t_init(
    bcm_l3_host_t *ip);

#ifndef BCM_HIDE_DISPATCHABLE

/* Look up an L3 host table entry based on IP address. */
extern int bcm_l3_host_find(
    int unit, 
    bcm_l3_host_t *info);

/* Add an entry into the L3 switching table. */
extern int bcm_l3_host_add(
    int unit, 
    bcm_l3_host_t *info);

/* Delete an entry from the L3 host table. */
extern int bcm_l3_host_delete(
    int unit, 
    bcm_l3_host_t *ip_addr);

/* Delete L3 entries based on IP prefix (network). */
extern int bcm_l3_host_delete_by_network(
    int unit, 
    bcm_l3_route_t *ip_addr);

/* 
 * Deletes L3 entries that match or do not match a specified L3 interface
 * number
 */
extern int bcm_l3_host_delete_by_interface(
    int unit, 
    bcm_l3_host_t *info);

/* Deletes all L3 host table entries. */
extern int bcm_l3_host_delete_all(
    int unit, 
    bcm_l3_host_t *info);

/* Return list of conflicts in the L3 table. */
extern int bcm_l3_host_conflict_get(
    int unit, 
    bcm_l3_key_t *ipkey, 
    bcm_l3_key_t *cf_array, 
    int cf_max, 
    int *cf_count);

/* Run L3 table aging */
extern int bcm_l3_host_age(
    int unit, 
    uint32 flags, 
    bcm_l3_host_traverse_cb age_cb, 
    void *user_data);

/* Traverse through the L3 table and run callback at each valid L3 entry. */
extern int bcm_l3_host_traverse(
    int unit, 
    uint32 flags, 
    uint32 start, 
    uint32 end, 
    bcm_l3_host_traverse_cb cb, 
    void *user_data);

/* Invalidate L3 entry without clearing so it can be re-validated later. */
extern int bcm_l3_host_invalidate_entry(
    int unit, 
    bcm_ip_t info);

/* Invalidate L3 entry without clearing so it can be re-validated later. */
extern int bcm_l3_host_validate_entry(
    int unit, 
    bcm_ip_t info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_l3_route_t structure. */
extern void bcm_l3_route_t_init(
    bcm_l3_route_t *route);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an IP route to the L3 route table. */
extern int bcm_l3_route_add(
    int unit, 
    bcm_l3_route_t *info);

/* Delete an IP route from the DEFIP table. */
extern int bcm_l3_route_delete(
    int unit, 
    bcm_l3_route_t *info);

/* Delete routes based on matching or non-matching L3 interface number. */
extern int bcm_l3_route_delete_by_interface(
    int unit, 
    bcm_l3_route_t *info);

/* Delete all routes. */
extern int bcm_l3_route_delete_all(
    int unit, 
    bcm_l3_route_t *info);

/* Look up a route given the network and netmask. */
extern int bcm_l3_route_get(
    int unit, 
    bcm_l3_route_t *info);

/* Given a network, return all the paths for this route. */
extern int bcm_l3_route_multipath_get(
    int unit, 
    bcm_l3_route_t *the_route, 
    bcm_l3_route_t *path_array, 
    int max_path, 
    int *path_count);

/* Age the route table. */
extern int bcm_l3_route_age(
    int unit, 
    uint32 flags, 
    bcm_l3_route_traverse_cb age_out, 
    void *user_data);

/* Traverse through the routing table and run callback at each route. */
extern int bcm_l3_route_traverse(
    int unit, 
    uint32 flags, 
    uint32 start, 
    uint32 end, 
    bcm_l3_route_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Set the maximum ECMP paths allowed for a route (StrataXGS only).
 * For optimized Resilient Hashing, maximum ECMP paths is a dummy
 * variable.
 */
extern int bcm_l3_route_max_ecmp_set(
    int unit, 
    int max);

/* Get the maximum ECMP paths allowed for a route (StrataXGS only). */
extern int bcm_l3_route_max_ecmp_get(
    int unit, 
    int *max);

/* Set flex counter object value for the given L3 unicast route. */
extern int bcm_l3_route_flexctr_object_set(
    int unit, 
    bcm_l3_route_t *info, 
    uint32 value);

/* Get flex counter object value for the given L3 unicast route. */
extern int bcm_l3_route_flexctr_object_get(
    int unit, 
    bcm_l3_route_t *info, 
    uint32 *value);

/* 
 * Extract list of IPv6 prefixes which are forwarded based on lower 32
 * bit of IPv6 address, treated as IPv4 address.
 */
extern int bcm_l3_ip6_prefix_map_get(
    int unit, 
    int map_size, 
    bcm_ip6_t *ip6_array, 
    int *ip6_count);

/* 
 * Add IPv6 prefix to the list of prefixes which are forwarded based on
 * lower 32-bit of IPv6 address, treated as IPv4 address.
 */
extern int bcm_l3_ip6_prefix_map_add(
    int unit, 
    bcm_ip6_t ip6_addr);

/* 
 * Remove IPv6 prefix from the list of prefixes which are forwarded based
 * on lower 32-bit of IPv6 address, treated as IPv4 address.
 */
extern int bcm_l3_ip6_prefix_map_delete(
    int unit, 
    bcm_ip6_t ip6_addr);

/* 
 * Remove all the IPv6 prefixes from the list of prefixes which are
 * forwarded based on lower 32-bit of IPv6 address, treated as IPv4
 * address.
 */
extern int bcm_l3_ip6_prefix_map_delete_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_l3_age_cb */
typedef void (*bcm_l3_age_cb)(
    int unit, 
    bcm_ip_t ip);

/* Initialize a bcm_l3_key_t_init structure. */
extern void bcm_l3_key_t_init(
    bcm_l3_key_t *key);

/* Initialize a bcm_l3_info_t_init structure. */
extern void bcm_l3_info_t_init(
    bcm_l3_info_t *info);

/* Types of counters per L3 object. */
typedef enum bcm_l3_stat_e {
    bcmL3StatOutPackets = 0, 
    bcmL3StatOutBytes = 1, 
    bcmL3StatDropPackets = 2, 
    bcmL3StatDropBytes = 3, 
    bcmL3StatInPackets = 4, 
    bcmL3StatInBytes = 5 
} bcm_l3_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get L3 route statistics. */
extern int bcm_l3_route_stat_get(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint64 *val);

/* Get L3 route statistics. */
extern int bcm_l3_route_stat_get32(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint32 *val);

/* Set L3 route statistics. */
extern int bcm_l3_route_stat_set(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint64 val);

/* Set L3 route statistics. */
extern int bcm_l3_route_stat_set32(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint32 val);

/* Enable/disable collection of statistics for the indicated L3 route. */
extern int bcm_l3_route_stat_enable_set(
    int unit, 
    bcm_l3_route_t *route, 
    int enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Maximum L3 ALPM levels */
#define BCM_L3_ALPM_LEVELS      3          /* Maximum L3 ALPM levels */

/* This enum defines L3 ALPM IP types. */
typedef enum bcm_l3_alpm_ipt_e {
    bcmL3AlpmIptV4 = 0,     /* L3 ALPM IPv4 type */
    bcmL3AlpmIptV6 = 1,     /* L3 ALPM IPv6 type */
    bcmL3AlpmIptCount = 2,  /* Total count of L3 ALPM IP types */
    bcmL3AlpmIptAll = 3     /* Represent all L3 ALPM IP types */
} bcm_l3_alpm_ipt_t;

/* This enum defines L3 ALPM route groups. */
typedef enum bcm_l3_route_group_e {
    bcmL3RouteGroupPrivateV4 = 0,   /* L3 IPv4 UC private VRF route group */
    bcmL3RouteGroupGlobalV4 = 1,    /* L3 IPv4 UC global low route group */
    bcmL3RouteGroupOverrideV4 = 2,  /* L3 IPv4 UC override (global high) route
                                       group */
    bcmL3RouteGroupPrivateV6 = 3,   /* L3 IPv6 UC private VRF route group */
    bcmL3RouteGroupGlobalV6 = 4,    /* L3 IPv6 UC global low route group */
    bcmL3RouteGroupOverrideV6 = 5,  /* L3 IPv6 UC override (global high) route
                                       group */
    bcmL3RouteGroupCounter          /* L3 ALPM route group counter. This is not
                                       a group and should always be in the last */
} bcm_l3_route_group_t;

/* L3 ALPM per-level resource usage information structure. */
typedef struct bcm_l3_alpm_lvl_usage_s {
    int cnt_used;   /* used TCAM entry or bucket bank counter. */
    int cnt_total;  /* total TCAM entry or bucket bank counter. */
} bcm_l3_alpm_lvl_usage_t;

/* L3 ALPM route counter and resource usages structure. */
typedef struct bcm_l3_alpm_resource_s {
    int route_cnt;                      /* installed route number. */
    bcm_l3_alpm_lvl_usage_t lvl_usage[BCM_L3_ALPM_LEVELS]; /* ALPM level usage array. */
} bcm_l3_alpm_resource_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Get current ALPM route number and resource usage counters for a
 * specific route group.
 */
extern int bcm_l3_alpm_resource_get(
    int unit, 
    bcm_l3_route_group_t grp, 
    bcm_l3_alpm_resource_t *resource);

/* Check ALPM routes sanity and detect hw/sw mismatch. */
extern int bcm_l3_alpm_sanity_check(
    int unit, 
    int chk_type);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Return values from ALPM route trace message write callout routine. */
typedef enum bcm_l3_alpm_trace_e {
    BCM_L3_ALPM_TRACE_WRITE_OK = 0,     /* ALPM route trace message write
                                           successful. */
    BCM_L3_ALPM_TRACE_WRITE_FAIL = -1   /* ALPM route trace message write
                                           failed. */
} bcm_l3_alpm_trace_t;

/* Callback function type for applications using ALPM trace facility. */
typedef bcm_l3_alpm_trace_t (*bcm_l3_alpm_trace_cb_f)(
    int unit, 
    char *trace_msg, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Register ALPM trace callback for route insertion, update or delete.
 * Only one callback (the most recent) is allowed per unit.
 */
extern int bcm_l3_alpm_trace_cb_register(
    int unit, 
    bcm_l3_alpm_trace_cb_f write_cb, 
    void *user_data);

/* Unregister ALPM trace callback for route insertion, update or delete. */
extern int bcm_l3_alpm_trace_cb_unregister(
    int unit, 
    bcm_l3_alpm_trace_cb_f write_cb);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * This enum defines ALPM route associated data modes that are maintained
 * per L3 VRF.
 */
typedef enum bcm_l3_vrf_route_data_mode_e {
    bcmL3VrfRouteDataModeReduced = 0,   /* Reduced ALPM route associated data
                                           mode on the L3 VRF */
    bcmL3VrfRouteDataModeFull = 1       /* Full ALPM route associated data mode
                                           on the L3 VRF */
} bcm_l3_vrf_route_data_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get current ALPM route data mode for the selected VRF and IP type. */
extern int bcm_l3_vrf_route_data_mode_get(
    int unit, 
    bcm_vrf_t vrf, 
    uint32 flags, 
    bcm_l3_vrf_route_data_mode_t *mode);

/* Set ALPM route data mode for the selected VRF and IP type. */
extern int bcm_l3_vrf_route_data_mode_set(
    int unit, 
    bcm_vrf_t vrf, 
    uint32 flags, 
    bcm_l3_vrf_route_data_mode_t mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of statistics that are maintained per L3 VRF. */
typedef enum bcm_l3_vrf_stat_e {
    bcmL3VrfStatIngressPackets = 0, /* Packets that ingress on the L3 VRF */
    bcmL3VrfStatIngressBytes = 1    /* Bytes that ingress on the L3 VRF */
} bcm_l3_vrf_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable/disable packet and byte counters for the selected VRF. */
extern int bcm_l3_vrf_stat_enable_set(
    int unit, 
    bcm_vrf_t vrf, 
    int enable);

/* Get L3 VRF counter value for specified VRF statistic type. */
extern int bcm_l3_vrf_stat_get(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and retrieve L3 VRF counter value
 * for
 * specified VRF statistic type.
 */
extern int bcm_l3_vrf_stat_sync_get(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint64 *val);

/* Get L3 VRF counter value for specified VRF statistic type. */
extern int bcm_l3_vrf_stat_get32(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and retrieve L3 VRF counter value
 * for
 * specified VRF statistic type.
 */
extern int bcm_l3_vrf_stat_sync_get32(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 *val);

/* Get stat counter ID associated with given vrf. */
extern int bcm_l3_vrf_stat_id_get(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 *stat_counter_id);

/* Set L3 VRF counter value for specified VRF statistic type. */
extern int bcm_l3_vrf_stat_set(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint64 val);

/* Set L3 VRF counter value for specified VRF statistic type. */
extern int bcm_l3_vrf_stat_set32(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 val);

/* Get L3 VRF counter value for multiple VRF statistic types. */
extern int bcm_l3_vrf_stat_multi_get(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint64 *value_arr);

/* Get L3 VRF counter value for multiple VRF statistic types. */
extern int bcm_l3_vrf_stat_multi_get32(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint32 *value_arr);

/* Set L3 VRF counter value for multiple VRF statistic types. */
extern int bcm_l3_vrf_stat_multi_set(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint64 *value_arr);

/* Set L3 VRF counter value for multiple VRF statistic types. */
extern int bcm_l3_vrf_stat_multi_set32(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint32 *value_arr);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_L3_SOURCE_BIND_IP6      0x00000001 
#define BCM_L3_SOURCE_BIND_REPLACE  0x00000002 
#define BCM_L3_SOURCE_BIND_USE_MASK 0x00000004 
#define BCM_L3_SOURCE_BIND_PPPoE    0x00000008 

/* L3 source binding information structure. */
typedef struct bcm_l3_source_bind_s {
    uint32 flags;                   /* BCM_L3_SOURCE_BIND_xxx flags. */
    bcm_gport_t port;               /* Source module and port, BCM_GPORT_INVALID
                                       to wildcard. */
    bcm_ip_t ip;                    /* Source IPv4 address. */
    bcm_ip6_t ip6;                  /* Source IPv6 address. */
    bcm_mac_t mac;                  /* Source MAC address. */
    bcm_ipfix_rate_id_t rate_id;    /* IPFIX rate ID */
    bcm_ip_t ip_mask;               /* IP subnet mask (IPv4). */
    bcm_ip6_t ip6_mask;             /* IP subnet mask (IPv6). */
    uint32 session_id;              /* PPPoE session ID. */
} bcm_l3_source_bind_t;

/* Initialize a bcm_l3_source_bind_t structure. */
extern void bcm_l3_source_bind_t_init(
    bcm_l3_source_bind_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable or disable l3 source binding checks on an ingress port. */
extern int bcm_l3_source_bind_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* 
 * Retrieve whether l3 source binding checks are performed on an
 * ingress port.
 */
extern int bcm_l3_source_bind_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

/* Add or replace an L3 source binding. */
extern int bcm_l3_source_bind_add(
    int unit, 
    bcm_l3_source_bind_t *info);

/* Remove an existing L3 source binding. */
extern int bcm_l3_source_bind_delete(
    int unit, 
    bcm_l3_source_bind_t *info);

/* Remove all existing L3 source bindings. */
extern int bcm_l3_source_bind_delete_all(
    int unit);

/* Retrieve the details of an existing L3 source binding. */
extern int bcm_l3_source_bind_get(
    int unit, 
    bcm_l3_source_bind_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef int (*bcm_l3_source_bind_traverse_cb)(
    int unit, 
    bcm_l3_source_bind_t *info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse through the L3 source bindings and run callback at each
 * defined binding.
 */
extern int bcm_l3_source_bind_traverse(
    int unit, 
    bcm_l3_source_bind_traverse_cb cb, 
    void *user_data);

/* 
 * Add VRID for the given VSI. Adding a VRID using this API means the
 * physical node has become the master for the virtual router
 */
extern int bcm_l3_vrrp_add(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 vrid);

/* Delete VRID for a particular VLAN/VSI */
extern int bcm_l3_vrrp_delete(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 vrid);

/* Delete all the VRIDs for a particular VLAN/VSI */
extern int bcm_l3_vrrp_delete_all(
    int unit, 
    bcm_vlan_t vlan);

/* 
 * Get all the VRIDs for which the physical node is master for the
 * virtual routers on the given VLAN/VSI
 */
extern int bcm_l3_vrrp_get(
    int unit, 
    bcm_vlan_t vlan, 
    int alloc_size, 
    int *vrid_array, 
    int *count);

/* Attach   counters entries to the given VRF. */
extern int bcm_l3_vrf_stat_attach(
    int unit, 
    bcm_vrf_t vrf, 
    uint32 stat_counter_id);

/* Detach   counters entries to the given VRF. */
extern int bcm_l3_vrf_stat_detach(
    int unit, 
    bcm_vrf_t vrf);

/* Get L3 VRF counter value for specified VRF statistic type */
extern int bcm_l3_vrf_stat_counter_get(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve L3 VRF counter value
 * for
 * specified VRF statistic type
 */
extern int bcm_l3_vrf_stat_counter_sync_get(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set L3 VRF counter value for specified VRF statistic type */
extern int bcm_l3_vrf_stat_counter_set(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Attach counters entries to the given L3 Egress interface. */
extern int bcm_l3_egress_stat_attach(
    int unit, 
    bcm_if_t intf_id, 
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 Egress interface. */
extern int bcm_l3_egress_stat_detach(
    int unit, 
    bcm_if_t intf_id);

/* Get the specified counter statistic for a L3 egress interface. */
extern int bcm_l3_egress_stat_counter_get(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve the specified counter
 * statistic for a L3 egress interface.
 */
extern int bcm_l3_egress_stat_counter_sync_get(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get stat counter ID associated with given L3 Egress interface. */
extern int bcm_l3_egress_stat_id_get(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 *stat_counter_id);

/* Set the specified counter statistic for a L3 egress interface. */
extern int bcm_l3_egress_stat_counter_set(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set flex counter object value for the given egress object. */
extern int bcm_l3_egress_flexctr_object_set(
    int unit, 
    bcm_if_t intf_id, 
    uint32 value);

/* Get flex counter object value for the given egress object. */
extern int bcm_l3_egress_flexctr_object_get(
    int unit, 
    bcm_if_t intf_id, 
    uint32 *value);

/* Attach counters entries to the given L3 ingress interface. */
extern int bcm_l3_ingress_stat_attach(
    int unit, 
    bcm_if_t intf_id, 
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 ingress interface. */
extern int bcm_l3_ingress_stat_detach(
    int unit, 
    bcm_if_t intf_id);

/* Get counter statistic values for a l3 interface object. */
extern int bcm_l3_ingress_stat_counter_get(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get counter statistic values for a l3 interface object. */
extern int bcm_l3_ingress_stat_counter_sync_get(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get stat counter ID associated with given L3 ingress interface. */
extern int bcm_l3_ingress_stat_id_get(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 *stat_counter_id);

/* Set counter statistic values for a l3 interface object. */
extern int bcm_l3_ingress_stat_counter_set(
    int unit, 
    bcm_if_t intf_id, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Attach counters entries to the given L3 host entry */
extern int bcm_l3_host_stat_attach(
    int unit, 
    bcm_l3_host_t *info, 
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 host entry */
extern int bcm_l3_host_stat_detach(
    int unit, 
    bcm_l3_host_t *info);

/* Get the specified counter statistic for a L3 host entry. */
extern int bcm_l3_host_stat_counter_get(
    int unit, 
    bcm_l3_host_t *info, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve the specified counter
 * statistic for a L3 host entry.
 */
extern int bcm_l3_host_stat_counter_sync_get(
    int unit, 
    bcm_l3_host_t *info, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set the specified counter statistic for a L3 host entry */
extern int bcm_l3_host_stat_counter_set(
    int unit, 
    bcm_l3_host_t *info, 
    bcm_l3_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Provide stat counter ids associated with given L3 host entry */
extern int bcm_l3_host_stat_id_get(
    int unit, 
    bcm_l3_host_t *info, 
    bcm_l3_stat_t stat, 
    uint32 *stat_counter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L3 VRRP Flags. */
#define BCM_L3_VRRP_IPV4        0x00000001 /* VRID for IPV4 */
#define BCM_L3_VRRP_IPV6        0x00000002 /* VRID for IPV6 */
#define BCM_L3_VRRP_EXTENDED    0x00000004 /* Indicate extended memory usage for
                                              VRID */

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Add VRID for the given VSI. VRID can be for IPV4 or for IPV6 (if VRRP
 * mode supports IPV6).
 * Adding a VRID using this API means the physical node has become the
 * master for the virtual router
 */
extern int bcm_l3_vrrp_config_add(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan, 
    uint32 vrid);

/* Delete VRID for a particular VLAN/VSI, either for IPV4 or for IPV6 */
extern int bcm_l3_vrrp_config_delete(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan, 
    uint32 vrid);

/* Delete all the VRIDs for a particular VLAN/VSI */
extern int bcm_l3_vrrp_config_delete_all(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan);

/* 
 * Get all the VRIDs for which the physical node is master for the
 * virtual routers on the given VLAN/VSI
 */
extern int bcm_l3_vrrp_config_get(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan, 
    uint32 alloc_size, 
    uint32 *vrid_array, 
    uint32 *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L3 Route statistics maintained per route. */
typedef enum bcm_l3_route_stat_e {
    bcmL3RouteInPackets = 0,    /* Packets that ingress on the l3 route */
    bcmL3RouteInBytes = 1       /* Bytes that ingress on the l3 route */
} bcm_l3_route_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Attach counters entries to the given L3 route index */
extern int bcm_l3_route_stat_attach(
    int unit, 
    bcm_l3_route_t *info, 
    uint32 stat_counter_id);

/* Detach  counters entries to the given L3 route index. */
extern int bcm_l3_route_stat_detach(
    int unit, 
    bcm_l3_route_t *info);

/* Get counter statistic values for a l3 route index. */
extern int bcm_l3_route_stat_counter_get(
    int unit, 
    bcm_l3_route_t *info, 
    bcm_l3_route_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* 
 * Force an immediate counter update and retrieve counter statistic
 * values for a l3 route index.
 */
extern int bcm_l3_route_stat_counter_sync_get(
    int unit, 
    bcm_l3_route_t *info, 
    bcm_l3_route_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Set counter statistic values for a l3 route index */
extern int bcm_l3_route_stat_counter_set(
    int unit, 
    bcm_l3_route_t *info, 
    bcm_l3_route_stat_t stat, 
    uint32 num_entries, 
    uint32 *counter_indexes, 
    bcm_stat_value_t *counter_values);

/* Get multiple counter statistic values for multiple l3 route index. */
extern int bcm_l3_route_stat_multi_get(
    int unit, 
    bcm_l3_route_t *info, 
    int nstat, 
    bcm_l3_route_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Get 32bit multiple counter statistic values for multiple l3 route
 * index.
 */
extern int bcm_l3_route_stat_multi_get32(
    int unit, 
    bcm_l3_route_t *info, 
    int nstat, 
    bcm_l3_route_stat_t *stat_arr, 
    uint32 *value_arr);

/* Set multiple counter statistic values for multiple l3 route index. */
extern int bcm_l3_route_stat_multi_set(
    int unit, 
    bcm_l3_route_t *info, 
    int nstat, 
    bcm_l3_route_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Set 32bit multiple counter statistic values for multiple l3 route
 * index.
 */
extern int bcm_l3_route_stat_multi_set32(
    int unit, 
    bcm_l3_route_t *info, 
    int nstat, 
    bcm_l3_route_stat_t *stat_arr, 
    uint32 *value_arr);

/* Get stat counter ID associated with given L3 route index. */
extern int bcm_l3_route_stat_id_get(
    int unit, 
    bcm_l3_route_t *info, 
    bcm_l3_route_stat_t stat, 
    uint32 *stat_counter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L3 IP options Flags. */
#define BCM_L3_IP4_OPTIONS_WITH_ID  0x00000001 /* With ID option creation */
#define BCM_L3_IP4_OPTIONS_REPLACE  0x00000002 /* Replace existing entry */

/* L3 IP options handing actions. */
typedef enum bcm_l3_ip4_options_action_e {
    bcmIntfIPOptionActionNone = 0,      /* No action. */
    bcmIntfIPOptionActionCopyToCPU = 1, /* Copy to CPU action. */
    bcmIntfIPOptionActionDrop = 2,      /* Drop action. */
    bcmIntfIPOptionActionCopyCPUAndDrop = 3 /* Copy to CPU and Drop action. */
} bcm_l3_ip4_options_action_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a new L3 IP options handling profile. */
extern int bcm_l3_ip4_options_profile_create(
    int unit, 
    uint32 flags, 
    bcm_l3_ip4_options_action_t default_action, 
    int *ip4_options_profile_id);

/* Delete an L3 IP options Profile */
extern int bcm_l3_ip4_options_profile_destroy(
    int unit, 
    int ip4_options_profile_id);

/* Set Individual IP options action for a given IP option profile */
extern int bcm_l3_ip4_options_action_set(
    int unit, 
    int ip4_options_profile_id, 
    int ip4_option, 
    bcm_l3_ip4_options_action_t action);

/* Get Individual IP options action for a given IP option profile */
extern int bcm_l3_ip4_options_action_get(
    int unit, 
    int ip4_options_profile_id, 
    int ip4_option, 
    bcm_l3_ip4_options_action_t *action);

/* Find a longest prefix matched route given an IP address. */
extern int bcm_l3_route_find(
    int unit, 
    bcm_l3_host_t *host, 
    bcm_l3_route_t *route);

/* Find a longest prefix matched route given the ip and netmask. */
extern int bcm_l3_subnet_route_find(
    int unit, 
    bcm_l3_route_t *input, 
    bcm_l3_route_t *route);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L3 Multiple Egress Object related flags. */
#define BCM_L3_EGRESS_MULTI_WITH_ID (1 << 0)   /* Create multiple egress objects
                                                  with id. */

/* Information to create multiple egress objects. */
typedef struct bcm_l3_egress_multi_info_s {
    uint32 flags;           /* See BCM_L3_EGRESS_MULTI_XXX flag definitions. */
    int number_of_elements; /* number of egress objects to allocate. */
} bcm_l3_egress_multi_info_t;

/* Initialize bcm_l3_egress_multi_info_t structure. */
extern void bcm_l3_egress_multi_info_t_init(
    bcm_l3_egress_multi_info_t *egress_multi_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Alloc/Create multiple L3 Egress Objects. */
extern int bcm_l3_egress_multi_alloc(
    int unit, 
    bcm_l3_egress_multi_info_t egress_multi_info, 
    bcm_if_t *base_egress_id);

/* Deallocates/frees multiple egress objects. */
extern int bcm_l3_egress_multi_free(
    int unit, 
    bcm_if_t base_egress_id);

/* Set the VPN on a given L3 Egress interface. */
extern int bcm_l3_intf_vpn_set(
    int unit, 
    bcm_if_t l3_intf_id, 
    uint32 flags, 
    bcm_vpn_t vpn);

/* Get the VLAN ID/VPN on a given L3 Egress overlay interface. */
extern int bcm_l3_intf_vpn_get(
    int unit, 
    bcm_if_t l3_intf_id, 
    uint32 *flags, 
    bcm_vpn_t *vpn);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * L3 vPBR Structure. Contains information required for manipulating L3
 * vPBR table entries
 */
typedef struct bcm_l3_vpbr_entry_s {
    uint32 flags;               /* See BCM_L3_VPBR_xxx flag definitions. */
    int priority;               /* Entry priority. */
    bcm_vrf_t vrf;              /* Virtual Router instance. */
    bcm_vrf_t vrf_mask; 
    bcm_ip_t sip_addr;          /* Source IP address (IPv4). */
    bcm_ip_t sip_addr_mask; 
    bcm_ip_t dip_addr;          /* Destination IP address (IPv4). */
    bcm_ip_t dip_addr_mask; 
    bcm_ip6_t sip6_addr;        /* Destination IP address (IPv6). */
    bcm_ip6_t sip6_addr_mask; 
    bcm_ip6_t dip6_addr;        /* Destination IP address (IPv6). */
    bcm_ip6_t dip6_addr_mask; 
    int dscp;                   /* DSCP value */
    int dscp_mask; 
    uint32 src_port;            /* TCP/UDP src port. */
    uint32 src_port_mask; 
    uint32 dst_port;            /* TCP/UDP dst port. */
    uint32 dst_port_mask; 
    bcm_vrf_t new_vrf;          /* The new result Virtual Router instance. */
    bcm_if_t l3_intf_id;        /* The relevant incoming interface . */
    bcm_if_t l3_intf_id_mask; 
    uint8 ip_protocol;          /* IP protocol field. */
    uint8 ip_protocol_mask;     /* IP protocol field mask. */
} bcm_l3_vpbr_entry_t;

/* Initialize a bcm_l3_vpbr_entry_t structure. */
extern void bcm_l3_vpbr_entry_t_init(
    bcm_l3_vpbr_entry_t *vpbr_info);

/* L3 Flags. */
#define BCM_L3_VPBR_IP6         0x00000001 /* IPv6 entries */

/* L3 vPBR Traverse cb type */
typedef int (*bcm_l3_vpbr_traverse_cb)(
    int unit, 
    bcm_l3_vpbr_entry_t *entry, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all L3 vPBR entries in DNX devices and call a function with
 * the data for each entry found.
 */
extern int bcm_l3_vpbr_traverse(
    int unit, 
    uint32 flags, 
    bcm_l3_vpbr_traverse_cb cb, 
    void *user_data);

/* Delete an entry from the L3 vPBR table. */
extern int bcm_l3_vpbr_entry_delete(
    int unit, 
    bcm_l3_vpbr_entry_t *entry);

/* Delete all L3 interfaces. */
extern int bcm_l3_vpbr_entry_delete_all(
    int unit, 
    bcm_l3_vpbr_entry_t *entry);

/* Look up a L3 vPBR table entry. */
extern int bcm_l3_vpbr_entry_get(
    int unit, 
    bcm_l3_vpbr_entry_t *entry);

/* Add an entry to the L3 vPBR table. */
extern int bcm_l3_vpbr_entry_add(
    int unit, 
    bcm_l3_vpbr_entry_t *entry);

/* 
 * Configure monitoring parameters for a DLB corresponding to a ECMP
 * interface
 */
extern int bcm_l3_ecmp_dlb_mon_config_set(
    int unit, 
    bcm_if_t ecmp_intf, 
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/* 
 * Retrieve monitoring parameters for a DLB corresponding to a ECMP
 * interface
 */
extern int bcm_l3_ecmp_dlb_mon_config_get(
    int unit, 
    bcm_if_t intf, 
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize bcm_l3_ecmp_dlb_mon_cfg_t structure */
extern void bcm_l3_ecmp_dlb_mon_cfg_t_init(
    bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

/* L3 ECMP tunnel priority. */
typedef struct bcm_l3_ecmp_tunnel_priority_map_info_s {
    uint32 l3_flags;        /* See BCM_L3_xxx flag definitions. */
    int map_profile;        /* Tunnel Priority Map profile. */
    int tunnel_priority;    /* Tunnel Priority. */
    int index;              /* Index to the Tunnel Priority table to update. */
} bcm_l3_ecmp_tunnel_priority_map_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Creates a new ECMP tunnel priority profile. */
extern int bcm_l3_ecmp_tunnel_priority_map_create(
    int unit, 
    bcm_l3_ecmp_tunnel_priority_map_info_t *map_info);

/* 
 * Set a priority index for a set of tunnel priority and a priority
 * profile.
 */
extern int bcm_l3_ecmp_tunnel_priority_map_set(
    int unit, 
    bcm_l3_ecmp_tunnel_priority_map_info_t *map_info);

/* 
 * Get a priority index for a set of tunnel priority and a priority
 * profile.
 */
extern int bcm_l3_ecmp_tunnel_priority_map_get(
    int unit, 
    bcm_l3_ecmp_tunnel_priority_map_info_t *map_info);

/* Destroys an ECMP tunnel priority profile. */
extern int bcm_l3_ecmp_tunnel_priority_map_destroy(
    int unit, 
    bcm_l3_ecmp_tunnel_priority_map_info_t *map_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* The attributes that determine DLB port's quality. */
typedef struct bcm_l3_ecmp_dlb_port_quality_attr_s {
    int scaling_factor;     /* Scaling factor for dynamic load balancing
                               thresholds. */
    int load_weight;        /* Weight of traffic load in determining port's
                               quality. */
    int queue_size_weight;  /* Weight of total queue size in determining port's
                               quality. */
} bcm_l3_ecmp_dlb_port_quality_attr_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set ECMP DLB port quality attributes. */
extern int bcm_l3_ecmp_dlb_port_quality_attr_set(
    int unit, 
    bcm_port_t port, 
    bcm_l3_ecmp_dlb_port_quality_attr_t *quality_attr);

/* Get ECMP DLB port quality attributes. */
extern int bcm_l3_ecmp_dlb_port_quality_attr_get(
    int unit, 
    bcm_port_t port, 
    bcm_l3_ecmp_dlb_port_quality_attr_t *quality_attr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_l3_ecmp_dlb_port_quality_attr_t structure. */
extern void bcm_l3_ecmp_dlb_port_quality_attr_t_init(
    bcm_l3_ecmp_dlb_port_quality_attr_t *quality_attr);

/* Define the counting source (type) of L3 ECMP DLB counter engine. */
typedef enum bcm_l3_ecmp_dlb_stat_e {
    bcmL3ECMPDlbStatFailPackets = 0,    /* Number of packets that cannot be
                                           resolved through the DLB mechanism. */
    bcmL3ECMPDlbStatPortReassignmentCount = 1, /* Total ECMP port member reassignments
                                           counter. */
    bcmL3ECMPDlbStatMemberReassignmentCount = 2, /* Total ECMP member reassignments
                                           counter. */
    bcmL3ECMPDlbStatCount = 3           /* Always last. */
} bcm_l3_ecmp_dlb_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the DLB statistics based on the ECMP interface. */
extern int bcm_l3_ecmp_dlb_stat_set(
    int unit, 
    bcm_if_t ecmp_intf, 
    bcm_l3_ecmp_dlb_stat_t type, 
    uint64 value);

/* Get the DLB statistics based on the ECMP interface. */
extern int bcm_l3_ecmp_dlb_stat_get(
    int unit, 
    bcm_if_t ecmp_intf, 
    bcm_l3_ecmp_dlb_stat_t type, 
    uint64 *value);

/* Get the DLB statistics based on the ECMP interface in sync mode. */
extern int bcm_l3_ecmp_dlb_stat_sync_get(
    int unit, 
    bcm_if_t ecmp_intf, 
    bcm_l3_ecmp_dlb_stat_t type, 
    uint64 *value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* L3 AACL options. */
#define BCM_L3_AACL_OPTIONS_REPLACE (1U << 0)  /* Replace existing L3 AACL. */

/* L3 AACL flags. */
#define BCM_L3_AACL_FLAGS_IP6       (1U << 0)  /* Indicate L3 AACL is for IPv6
                                                  packet. */
#define BCM_L3_AACL_FLAGS_IP_SRC    (1U << 1)  /* Source IP subnet address
                                                  match. */

/* 
 * L3 AACL Structure.
 * Contains information required for manipulating L3 AACLs.
 */
typedef struct bcm_l3_aacl_s {
    uint32 flags;               /* See BCM_L3_AACL_FLAGS_xxx flag definitions. */
    bcm_ip_t ip;                /* IP subnet address (IPv4). */
    bcm_ip_t ip_mask;           /* IP subnet mask (IPv4). */
    bcm_ip6_t ip6;              /* IP subnet address (IPv6). */
    bcm_ip6_t ip6_mask;         /* IP subnet mask (IPv6). */
    bcm_l4_port_t l4_port;      /* L4 port. */
    bcm_l4_port_t l4_port_mask; /* L4 port mask. */
    uint32 class_id;            /* Compression class ID. */
} bcm_l3_aacl_t;

/* Initialize a bcm_l3_aacl_t structure. */
extern void bcm_l3_aacl_t_init(
    bcm_l3_aacl_t *aacl);

#ifndef BCM_HIDE_DISPATCHABLE

/* Adds a L3 AACL to the compression table. */
extern int bcm_l3_aacl_add(
    int unit, 
    uint32 options, 
    bcm_l3_aacl_t *aacl);

/* Deletes a L3 AACL from the compression table. */
extern int bcm_l3_aacl_delete(
    int unit, 
    bcm_l3_aacl_t *aacl);

/* Deletes all AACLs. */
extern int bcm_l3_aacl_delete_all(
    int unit, 
    bcm_l3_aacl_t *aacl);

/* Finds information from the AACL table. */
extern int bcm_l3_aacl_find(
    int unit, 
    bcm_l3_aacl_t *aacl);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_l3_aacl_traverse_cb */
typedef int (*bcm_l3_aacl_traverse_cb)(
    int unit, 
    bcm_l3_aacl_t *aacl, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse through the AACL table and run callback at each entry. */
extern int bcm_l3_aacl_traverse(
    int unit, 
    bcm_l3_aacl_traverse_cb trav_fn, 
    void *user_data);

/* 
 * Traverse through the AACL table and run callback with matched
 * condition.
 */
extern int bcm_l3_aacl_matched_traverse(
    int unit, 
    uint32 flags, 
    bcm_l3_aacl_traverse_cb trav_fn, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * L3 ECMP group info structure.
 * Contains information about L3 ECMP Group that is used mainly for 2
 * level ECMP.
 */
typedef struct bcm_l3_ecmp_group_info_s {
    int ecmp_levels;                /* Number of ECMP levels in use */
    int level1_ecmp_start_index;    /* First valid index for Level 1 ECMP group */
    int level1_ecmp_size;           /* Max number of Level 1 ECMP groups
                                       possible */
    int level2_ecmp_start_index;    /* First valid index for Level 2 ECMP group */
    int level2_ecmp_size;           /* Max number of Level 2 ECMP groups
                                       possible */
} bcm_l3_ecmp_group_info_t;

/* Initialize a bcm_l3_ecmp_group_info_t structure. */
extern void bcm_l3_ecmp_group_info_t_init(
    bcm_l3_ecmp_group_info_t *l3_ecmp_group_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Obtain ECMP group general information. */
extern int bcm_l3_ecmp_group_info_get(
    int unit, 
    bcm_l3_ecmp_group_info_t *l3_ecmp_group_info);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_L3_H__ */
