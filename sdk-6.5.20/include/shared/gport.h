/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines gport (generic port) parameters. The gport is useful
 * for specifying the following types of identifiers:
 *   LOCAL      :  port on the local unit
 *   MODPORT    :  {module ID, port} pair
 *   TRUNK      :  trunk ID
 *   PROXY      :  {module ID, port} pair employed in proxy operations
 *   BLACK_HOLE :  identifier indicating drop destination
 *   LOCAL_CPU  :  identifier indicating local CPU destination
 *   MPLS_PORT  :  L2 MPLS virtual-port (VPLS/VPWS)
 *   SUBPORT_GROUP : Subport group
 *   SUBPORT_PORT  : Subport virtual-port
 *   UCAST_QUEUE_GROUP : Group of unicast COSQs
 *   MCAST_QUEUE_GROUP : Group of multicast COSQs
 *   UCAST_SUBSCRIBER_QUEUE_GROUP : Group of 64K subscriber unicast COSQs
 *   MCAST_SUBSCRIBER_QUEUE_GROUP : Group of 64K subscriber multicast COSQs
 *   MCAST :  distribution set identifier
 *   SCHEDULER  : COSQ scheduler
 *   DEVPORT    : {device ID, port} pair (ports on devices without a module ID)
 *   SPECIAL    :  application special value (invalid in BCM APIs)
 *   MIRROR     :  Mirror (modport & encapsulation) for mirrored traffic.
 *   MIM_PORT   :  MIM virtual-port
 *   VLAN_PORT  :  VLAN virtual-port
 *   WLAN_PORT  :  WLAN virtual-port
 *   TRILL_PORT :  TRILL virtual-port
 *   NIV_PORT   :  NIV virtual-port
 *   EXTENDER_PORT :  Extender virtual-port
 *   MAC_PORT   :  MAC virtual-port
 *   TUNNEL     :  Tunnel ID
 *   MULTIPATH  :  Multipath shaper
 *
 * This header requires that the uint32 type be defined before inclusion.
 * Using <sal/types.h> is the simplest (and recommended) way of doing this.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define gport parameters.
 *
 * The following macros are made available.  All have names starting
 * with _SHR_, which have been elided from this list:
 *
 * Constants or Expressions suitable for assignment:
 * GPORT_NONE                   gport initializer
 * GPORT_INVALID                invalid gport identifier
 * GPORT_BLACK_HOLE             black-hole gport idenitifier
 * GPORT_LOCAL_CPU              local CPU gport identifier
 *
 * Predicates: (return 0 or 1, suitable for using in if statements)
 * GPORT_IS_SET                 is the gport set?
 * GPORT_IS_LOCAL               is the gport a local port type?
 * GPORT_IS_MODPORT             is the gport a modid/port type?
 * GPORT_IS_TRUNK               is the gport a trunk type?
 * GPORT_IS_BLACK_HOLE          is the gport a black-hole type?
 * GPORT_IS_LOCAL_CPU           is the gport a local CPU type?
 * GPORT_IS_MPLS_PORT           is the gport a MPLS port type?
 * GPORT_IS_SUBPORT_GROUP       is the gport a subport group type?
 * GPORT_IS_SUBPORT_PORT        is the gport a subport port type?
 * GPORT_IS_UCAST_QUEUE_GROUP   is the gport a unicast group type?
 * GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP   is the gport a subscriber unicast group type?
 * GPORT_IS_MCAST               is the gport a multicast set type?
 * GPORT_IS_MCAST_QUEUE_GROUP   is the gport a multicast queue group type?
 * GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP   is the gport a multicast subscriber queue group type?
 * GPORT_IS_SCHEDULER           is the gport a scheduler type?
 * GPORT_IS_DEVPORT             is the gport a devid/port type?
 * GPORT_IS_SPECIAL             is the gport a special type?
 * GPORT_IS_MIRROR              is the gport a mirror destination type?
 * GPORT_IS_MIM_PORT            is the gport a MIM port type?
 * GPORT_IS_VLAN_PORT           is the gport a VLAN port type?
 * GPORT_IS_WLAN_PORT           is the gport a WLAN port type?
 * GPORT_IS_TRILL_PORT          is the gport a TRILL port type?
 * GPORT_IS_NIV_PORT            is the gport a NIV port type?
 * GPORT_IS_EXTENDER_PORT       is the gport an Extender port type?
 * EXTENDER_GPORT_IS_PORT       is the extender gport a port subtype?
 * EXTENDER_GPORT_IS_FORWARD    is the extender gport a port forward?
 * EXTENDER_GPORT_IS_ENCAP      is the extender gport a port encap?
 * GPORT_IS_MAC_PORT            is the gport a MAC port type?
 * GPORT_IS_TUNNEL              is the gport a tunnel type?
 * GPORT_IS_CHILD               is the gport a child port type?
 * GPORT_IS_EGRESS_GROUP        is the gport a egress group type?
 * GPORT_IS_EGRESS_CHILD        is the gport a egress child type?
 * GPORT_IS_EGRESS_MODPORT      is the gport a egress modport type?
 * COSQ_GPORT_IS_MULTIPATH      is the gport a multipath type?
 *
 * Statements: (cannot be used as a predicate)
 * GPORT_LOCAL_SET              set a gport local port type and value
 * GPORT_LOCAL_GET              get a port value from a local gport
 * GPORT_LOCAL_CPU_SET          set a gport local CPU port type and value
 * GPORT_LOCAL_CPU_GET          get a CPU port value from a local gport
 * GPORT_MODPORT_SET            set a gport modid/port type and values
 * GPORT_MODPORT_MODID_GET      get a modid value from a modport gport
 * GPORT_MODPORT_PORT_GET       get a port value from a modport gport
 * GPORT_TRUNK_SET              set a gport trunk type and value
 * GPORT_TRUNK_GET              get a trunk_id value from a trunk gport
 * GPORT_MPLS_PORT_ID_SET       set a MPLS ID type and value
 * GPORT_MPLS_PORT_ID_GET       get a MPLS ID from a MPLS gport
 * GPORT_SUBPORT_GROUP_SET      set a subport group type and value
 * GPORT_SUBPORT_GROUP_GET      get a subport group ID from a gport
 * GPORT_SUBPORT_PORT_SET       set a subport port type and value
 * GPORT_SUBPORT_PORT_GET       get a subport port ID from a gport
 * GPORT_SCHEDULER_SET          set a scheduler type and value
 * GPORT_SCHEDULER_GET          get a scheduler ID from a gport
 * GPORT_DEVPORT_SET            set a gport devid/port type and values
 * GPORT_DEVPORT_DEVID_GET      get a devid value from a devport gport
 * GPORT_DEVPORT_PORT_GET       get a port value from a devport gport
 * GPORT_UCAST_QUEUE_GROUP_SET  set a queue group type and value
 * GPORT_UCAST_QUEUE_GROUP_GET  get a queue group ID from a gport
 * GPORT_MCAST_SET              set a mcast dist set and value
 * GPORT_MCAST_GET              get an mcast dist set from a gport
 * GPORT_MCAST_DS_ID_SET        set an mcast DS ID and value
 * GPORT_MCAST_DS_ID_GET        get an mcast DS ID from a gport
 * GPORT_MCAST_QUEUE_GROUP_SET  set an mcast queue group type and value
 * GPORT_MCAST_QUEUE_GROUP_GET  get an mcast queue group ID from a gport
 * GPORT_MCAST_QUEUE_GROUP_SYSQID_SET set an mcast queue group system queue ID
 * GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET get an mcast queue group system port ID
 * GPORT_MCAST_QUEUE_GROUP_QID_GET get an mcast queue group queue ID
 * GPORT_UCAST_QUEUE_GROUP_SYSQID_SET get a unicast queue group system queue ID
 * GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET get a unicast queue group system port ID
 * GPORT_UCAST_QUEUE_GROUP_QID_GET get a unicast queue group queue ID
 * GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET set a ucast subscriber queue group queue id
 * GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET get a ucast subscriber queue group queue id
 * GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET set an mcast subscriber queue group queue id
 * GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET get an mcast subscriber queue group queue id
 * GPORT_SPECIAL_SET            set a gport special type and value
 * GPORT_SPECIAL_GET            get a value from a special gport
 * GPORT_MIRROR_SET             set a gport mirror type and value
 * GPORT_MIRROR_GET             get a value from a mirror gport
 * GPORT_MIM_PORT_ID_SET        set a MIM ID type and value
 * GPORT_MIM_PORT_ID_GET        get a MIM ID from a Mim gport
 * GPORT_VLAN_PORT_ID_SET       set a VLAN port ID type and value
 * GPORT_VLAN_PORT_ID_GET       get a VLAN port ID from a VLAN gport
 * GPORT_WLAN_PORT_ID_SET       set a WLAN ID type and value
 * GPORT_WLAN_PORT_ID_GET       get a WLAN ID from a WLAN gport
 * GPORT_TRILL_PORT_ID_SET      set a TRILL ID type and value
 * GPORT_TRILL_PORT_ID_GET      get a TRILL ID from a Trill gport
 * GPORT_NIV_PORT_ID_SET        set a NIV ID type and value 
 * GPORT_NIV_PORT_ID_GET        get a NIV ID from a NIV gport
 * EXTENDER_GPORT_PORT_SET      set an Extender type, Port subtype and ID value
 * EXTENDER_GPORT_FORWARD_SET   set an Extender type, Forward subtype and ID value
 * EXTENDER_GPORT_ENCAP_SET     set an Extender type, Encap subtype and ID value
 * GPORT_EXTENDER_PORT_ID_SET   same as EXTENDER_GPORT_PORT_SET for backward compatability
 * GPORT_EXTENDER_PORT_ID_GET   get an Extender ID from an Extender gport
 * GPORT_MAC_PORT_ID_SET        set a MAC ID type and value 
 * GPORT_MAC_PORT_ID_GET        get a MAC ID from an MAC gport
 * GPORT_TUNNEL_ID_SET          set a gport tunnel type and value
 * GPORT_TUNNEL_ID_GET          get a tunnel_id value from a tunnel gport
 * GPORT_CHILD_SET              set a child gport
 * GPORT_CHILD_MODID_GET        get a child gport modid
 * GPORT_CHILD_PORT_GET         get a child gport port
 * GPORT_EGRESS_GROUP_SET       set an egress group
 * GPORT_EGRESS_GROUP_GET       get an egress group
 * GPORT_EGRESS_GROUP_MODID_GET get an egress group modid
 * GPORT_EGRESS_CHILD_SET       set an egress child gport
 * GPORT_EGRESS_CHILD_MODID_GET get an egress child gport modid
 * GPORT_EGRESS_CHILD_PORT_GET  get an egress child gport port
 * GPORT_EGRESS_MODPORT_SET     set an egress modport
 * GPORT_EGRESS_MODPORT_MODID_GET get an egress modport modid
 * GPORT_EGRESS_MODPORT_PORT_GET  get an egress modport port
 * GPORT_MULTIPATH_SET          set an multipath port id
 * GPORT_MULTIPATH_GET          get an multipath port id
 */

#ifndef _SHR_GPORT_H
#define _SHR_GPORT_H
/*************
 * INCLUDES  *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPEDEFs  *
 */
/*
 * {
 */
/*
 * Typedef:
 *    _shr_gport_t
 * Purpose:
 *    GPORT (generic port) type for shared definitions
 */
typedef uint32 _shr_gport_t;
/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */


/*
 * Defines:
 *     _SHR_GPORT_*
 * Purpose:
 *     GPORT (Generic port) definitions. GPORT can be used to specify
 *     a {module, port} pair, trunk, and other port types.
 */

#define _SHR_GPORT_NONE             (0)
#define _SHR_GPORT_INVALID         (-1)

#define _SHR_GPORT_TYPE_LOCAL               1U   /* Port on local unit */
#define _SHR_GPORT_TYPE_MODPORT             2U   /* Module ID and port */
#define _SHR_GPORT_TYPE_TRUNK               3U   /* Trunk ID */
#define _SHR_GPORT_TYPE_BLACK_HOLE          4U   /* Black hole destination */
#define _SHR_GPORT_TYPE_LOCAL_CPU           5U   /* CPU destination */
#define _SHR_GPORT_TYPE_MPLS_PORT           6U   /* MPLS port ID */
#define _SHR_GPORT_TYPE_SUBPORT_GROUP       7U   /* Subport group ID */
#define _SHR_GPORT_TYPE_SUBPORT_PORT        8U   /* Subport port ID */
#define _SHR_GPORT_TYPE_UCAST_QUEUE_GROUP   9U   /* Queue Group */
#define _SHR_GPORT_TYPE_DEVPORT            10U   /* Device ID and port */
#define _SHR_GPORT_TYPE_MCAST              11U   /* Multicast Set */
#define _SHR_GPORT_TYPE_MCAST_QUEUE_GROUP  12U   /* Multicast Queue Group */
#define _SHR_GPORT_TYPE_SCHEDULER          13U   /* Scheduler */
#define _SHR_GPORT_TYPE_SPECIAL            14U   /* Special value */
#define _SHR_GPORT_TYPE_MIRROR             15U   /* Mirror destination.  */
#define _SHR_GPORT_TYPE_MIM_PORT           16U   /* MIM port ID */
#define _SHR_GPORT_TYPE_VLAN_PORT          17U   /* VLAN port */
#define _SHR_GPORT_TYPE_WLAN_PORT          18U   /* WLAN port */
#define _SHR_GPORT_TYPE_TUNNEL             19U   /* Tunnel ID */
#define _SHR_GPORT_TYPE_CHILD              20U   /* Child port */
#define _SHR_GPORT_TYPE_EGRESS_GROUP       21U   /* Egress group port */
#define _SHR_GPORT_TYPE_EGRESS_CHILD       22U   /* Egress child port */
#define _SHR_GPORT_TYPE_EGRESS_MODPORT     23U   /* Egress Mod and port */
#define _SHR_GPORT_TYPE_UCAST_SUBSCRIBER_QUEUE_GROUP  24U        /* Local Queue Group */
#define _SHR_GPORT_TYPE_MCAST_SUBSCRIBER_QUEUE_GROUP  25U        /* Local Multicast Queue Group */
#define _SHR_GPORT_TYPE_TRILL_PORT         26U   /* TRILL port */
#define _SHR_GPORT_TYPE_SYSTEM_PORT        27U   /* DPP System-Port */
#define _SHR_GPORT_TYPE_NIV_PORT           28U   /* NIV port */
#define _SHR_GPORT_TYPE_CONGESTION         29U   /* congestion port, e.g. out of band HCFC port */
#define _SHR_GPORT_TYPE_COSQ               30U   /* cosq gport */
#define _SHR_GPORT_TYPE_L2GRE_PORT         31U   /* L2GRE gport */
#define _SHR_GPORT_TYPE_VXLAN_PORT         32U   /* VXLAN gport */
#define _SHR_GPORT_TYPE_EPON_LINK          33U   /* EPON link gport */
#define _SHR_GPORT_TYPE_PHY                34U   /* Phy gport multiple types */
#define _SHR_GPORT_TYPE_EXTENDER_PORT      35U   /* Extender port */
#define _SHR_GPORT_TYPE_MAC_PORT           36U   /* MAC port */
#define _SHR_GPORT_TYPE_PROXY              37U   /* Proxy (module ID, port) source or destination */
#define _SHR_GPORT_TYPE_FORWARD_PORT       38U   /* Gport for fec destinations */
#define _SHR_GPORT_TYPE_VP_GROUP           39U   /* VP group gport */
#define _SHR_GPORT_TYPE_PROFILE            40U
#define _SHR_GPORT_TYPE_DESTMOD_QUEUE_GROUP      41U     /* DMVOQ gport */
#define _SHR_GPORT_TYPE_LOCAL_CPU_IEEE     42U   /* CPU ethernet gport */
#define _SHR_GPORT_TYPE_LOCAL_CPU_HIGIG    43U   /* CPU higig gport */
#define _SHR_GPORT_TYPE_FLOW_PORT          44U   /* FLOW gport */
#define _SHR_GPORT_TYPE_VPLAG_PORT         45U   /* VPLAG gport */
#define _SHR_GPORT_TYPE_FLEXE_GROUP        46U   /* FlexE group gport */
#define _SHR_GPORT_TYPE_TDM_STREAM         47U   /* TDM Stream gport */
#define _SHR_GPORT_TYPE_NIF_RX_PRIORITY    48U   /* NIF Rx priority gport */
#define _SHR_GPORT_TYPE_COSQ_EXT           49U   /* cosq extended gport */
#define _SHR_GPORT_TYPE_MAX _SHR_GPORT_TYPE_COSQ_EXT   /* Used for sanity */
                                                /*
                                                 * checks only.  
                                                 */
 /*
  * MCAST GPORT subtypes 
  */
#define _SHR_MCAST_GPORT_TYPE_GROUP_ID          0
#define _SHR_MCAST_GPORT_TYPE_BITMAP_ID         1
#define _SHR_MCAST_GPORT_TYPE_STREAM_ID         2

/*
 * Congestion Gport subtypes, shift & mask definitions 
 */
#define _SHR_GPORT_CONGESTION_TYPE_SHIFT   23
#define _SHR_GPORT_CONGESTION_TYPE_MASK    0x7
#define _SHR_GPORT_CONGESTION_TYPE_OOB     0   /* congestion port subtupe for usage with OOB calendar */
#define _SHR_GPORT_CONGESTION_TYPE_COE     1   /* congestion port subtupe for usage with COE calendar */

/* COSQ GPORT subtypes*/
#define _SHR_COSQ_GPORT_TYPE_UCAST_EGRESS_QUEUE  2
#define _SHR_COSQ_GPORT_TYPE_MCAST_EGRESS_QUEUE  3
#define _SHR_COSQ_GPORT_TYPE_VSQ                 4
#define _SHR_COSQ_GPORT_TYPE_E2E_PORT            5
#define _SHR_COSQ_GPORT_TYPE_ISQ                 6
#define _SHR_COSQ_GPORT_TYPE_MULTIPATH           7
#define _SHR_COSQ_GPORT_TYPE_PORT_TC             11
#define _SHR_COSQ_GPORT_TYPE_PORT_TCG            12
#define _SHR_COSQ_GPORT_TYPE_E2E_PORT_TC         13
#define _SHR_COSQ_GPORT_TYPE_E2E_PORT_TCG        14
#define _SHR_COSQ_GPORT_TYPE_SYSTEM_RED          15
#define _SHR_COSQ_GPORT_TYPE_SRC_QUEUE           16
#define _SHR_COSQ_GPORT_TYPE_DST_QUEUE           17
#define _SHR_COSQ_GPORT_TYPE_FABRIC              18
#define _SHR_COSQ_GPORT_TYPE_FABRIC_RX_QUEUE     19
#define _SHR_COSQ_GPORT_TYPE_CORE                20
#define _SHR_COSQ_GPORT_TYPE_RCY                 21
#define _SHR_COSQ_GPORT_TYPE_RCY_MIRR            22
#define _SHR_COSQ_GPORT_TYPE_INGRESS_RECEIVE     23
#define _SHR_COSQ_GPORT_TYPE_E2E_INTERFACE       24
#define _SHR_COSQ_GPORT_TYPE_INBAND_COE          28


/* COSQ_EXT GPORT subtypes*/
#define _SHR_COSQ_EXT_GPORT_TYPE_VOQ_CONNECTOR        0
#define _SHR_COSQ_EXT_GPORT_TYPE_SCHED_CIR            1
#define _SHR_COSQ_EXT_GPORT_TYPE_SCHED_PIR            2
#define _SHR_COSQ_EXT_GPORT_TYPE_SHARED_SHAPER_ELEM   3
#define _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_SCHED  4
#define _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_CONN   5

#define _SHR_COSQ_EXT_GPORT_RESERVED_VOQ_CONNECTOR 1
#define _SHR_COSQ_EXT_GPORT_RESERVED_SCHEDULER     0

/* Extender GPORT subtypes*/
#define _SHR_EXTENDER_GPORT_TYPE_PORT               (0)
#define _SHR_EXTENDER_GPORT_TYPE_FORWARD            (1)
#define _SHR_EXTENDER_GPORT_TYPE_ENCAP              (2)

/* Profile GPORT subtypes*/
#define _SHR_GPORT_PROFILE_TYPE_UNKNOWN 0
#define _SHR_GPORT_PROFILE_TYPE_CREDIT_REQUEST 1
#define _SHR_GPORT_PROFILE_TYPE_INGRESS_LATENCY 2
#define _SHR_GPORT_PROFILE_TYPE_END_TO_END_LATENCY 3
#define _SHR_GPORT_PROFILE_TYPE_LATENCY_AQM_FLOW_ID 4
#define _SHR_GPORT_PROFILE_TYPE_END_TO_END_AQM_LATENCY 5



/* definitions for cosq core handling */
#define _SHR_COSQ_GPORT_COMMON_QUEUE_BITS 18
#define _SHR_COSQ_GPORT_COMMON_QUEUE_MASK ((1 << _SHR_COSQ_GPORT_COMMON_QUEUE_BITS) - 1)
#define _SHR_COSQ_GPORT_COMMON_CORE_BITS 3
#define _SHR_COSQ_GPORT_COMMON_CORE_SHIFT _SHR_COSQ_GPORT_COMMON_QUEUE_BITS
#define _SHR_COSQ_GPORT_COMMON_CORE_MASK ((1 << _SHR_COSQ_GPORT_COMMON_CORE_BITS) - 1)
#define _SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE _SHR_COSQ_GPORT_COMMON_CORE_MASK
/*
 * Note that only the bits under _SHR_COSQ_GPORT_CORE_MASK are considered
 * See, e.g. _SHR_COSQ_GPORT_CORE_GET
 */
#define _SHR_CORE_ALL -17
#define _SHR_CORE_FIELD2ID(field) ((field) != _SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE ? field : _SHR_CORE_ALL)
#define _SHR_CORE_ID2FIELD(id) ((id) != _SHR_CORE_ALL ? id : _SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE)

#define _SHR_GPORT_TYPE_SHIFT                           26
#define _SHR_GPORT_TYPE_MASK                            0x3f
#define _SHR_GPORT_MODID_SHIFT                          11
#define _SHR_GPORT_MODID_MASK                           0x7fff
#define _SHR_GPORT_PORT_SHIFT                           0
#define _SHR_GPORT_PORT_MASK                            0x7ff
#define _SHR_GPORT_TRUNK_SHIFT                          0
#define _SHR_GPORT_TRUNK_MASK                           0x3ffffff
#define _SHR_GPORT_MPLS_PORT_SHIFT                      0
#define _SHR_GPORT_MPLS_PORT_MASK                       0x3ffffff
#define _SHR_GPORT_SUBPORT_GROUP_SHIFT                  0
#define _SHR_GPORT_SUBPORT_GROUP_MASK                   0xffffff
#define _SHR_GPORT_SUBPORT_PORT_SHIFT                   0
#define _SHR_GPORT_SUBPORT_PORT_MASK                    0xffffff
#define _SHR_GPORT_UCAST_QUEUE_GROUP_SHIFT              0
#define _SHR_GPORT_UCAST_QUEUE_GROUP_MASK               0x3ffffff
#define _SHR_GPORT_UCAST_QUEUE_GROUP_QID_SHIFT          0
#define _SHR_GPORT_UCAST_QUEUE_GROUP_QID_MASK           0x3fff
#define _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_SHIFT    14
#define _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_MASK     0xfff
#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_SHIFT              0
#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_MASK               0x3ffffff
#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_SHIFT          0
#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_MASK           0x3fff
#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_SHIFT    14
#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_MASK     0xfff
#define _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_SHIFT        0
#define _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_MASK _SHR_COSQ_GPORT_COMMON_QUEUE_MASK
#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_SHIFT    16
#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_MASK     0xff
#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT 0
#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK  0xffff
#define _SHR_GPORT_MCAST_SHIFT                          0
#define _SHR_GPORT_MCAST_MASK                           0x3ffffff
#define _SHR_GPORT_MCAST_DS_ID_SHIFT                    0
#define _SHR_GPORT_MCAST_DS_ID_MASK                     0x3ffffff
#define _SHR_GPORT_MCAST_QUEUE_GROUP_SHIFT              0
#define _SHR_GPORT_MCAST_QUEUE_GROUP_MASK               0x3ffffff
#define _SHR_GPORT_MCAST_QUEUE_GROUP_QID_SHIFT          0
#define _SHR_GPORT_MCAST_QUEUE_GROUP_QID_MASK           0x3fff
#define _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_SHIFT    14
#define _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_MASK     0xfff
#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_SHIFT    16
#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_MASK     0xff
#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT 0
#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK  0x3fff
#define _SHR_GPORT_SCHEDULER_SHIFT                      0
#define _SHR_GPORT_SCHEDULER_MASK                       0x7fffff
#define _SHR_GPORT_SCHEDULER_CORE_SHIFT                 23
#define _SHR_GPORT_SCHEDULER_CORE_MASK                  _SHR_COSQ_GPORT_COMMON_CORE_MASK
#define _SHR_GPORT_SCHEDULER_LEVEL_SHIFT                20
#define _SHR_GPORT_SCHEDULER_LEVEL_MASK                 0x1f
#define _SHR_GPORT_SCHEDULER_NODE_SHIFT                 0
#define _SHR_GPORT_SCHEDULER_NODE_MASK                  0xfffff
#define _SHR_GPORT_DEVID_SHIFT                          _SHR_GPORT_MODID_SHIFT
#define _SHR_GPORT_DEVID_MASK                           _SHR_GPORT_MODID_MASK
#define _SHR_GPORT_SPECIAL_SHIFT                        0
#define _SHR_GPORT_SPECIAL_MASK                         0xffff
#define _SHR_GPORT_MIRROR_SHIFT                         0
#define _SHR_GPORT_MIRROR_MASK                          0xffff
#define _SHR_GPORT_MIRROR_SUBTYPE_SHIFT 22
#define _SHR_GPORT_MIRROR_SUBTYPE_MASK 0xf
#define _SHR_GPORT_MIRROR_SUBTYPE_VAL_MIRROR            0
#define _SHR_GPORT_MIRROR_SUBTYPE_VAL_SNOOP             1
#define _SHR_GPORT_MIRROR_SUBTYPE_VAL_STAT_SAMPLE       2
#define _SHR_GPORT_MIM_PORT_SHIFT                       0
#define _SHR_GPORT_MIM_PORT_MASK                        0xffffff
#define _SHR_GPORT_VLAN_PORT_SHIFT                      0
#define _SHR_GPORT_VLAN_PORT_MASK                       0x3ffffff
#define _SHR_GPORT_WLAN_PORT_SHIFT                      0
#define _SHR_GPORT_WLAN_PORT_MASK                       0xffffff
#define _SHR_GPORT_TRILL_PORT_SHIFT                     0
#define _SHR_GPORT_TRILL_PORT_MASK                      0xffffff
#define _SHR_GPORT_NIV_PORT_SHIFT                       0
#define _SHR_GPORT_NIV_PORT_MASK                        0xffffff
#define _SHR_GPORT_L2GRE_PORT_SHIFT                   0
#define _SHR_GPORT_L2GRE_PORT_MASK                    0xffffff
#define _SHR_GPORT_VXLAN_PORT_SHIFT                   0
#define _SHR_GPORT_VXLAN_PORT_MASK                    0xffffff
#define _SHR_GPORT_FLOW_PORT_SHIFT                    0
#define _SHR_GPORT_FLOW_PORT_MASK                     0xffffff
#define _SHR_GPORT_VPLAG_PORT_SHIFT                     0
#define _SHR_GPORT_VPLAG_PORT_MASK                      0xffffff
#define _SHR_GPORT_MAC_PORT_SHIFT                       0
#define _SHR_GPORT_MAC_PORT_MASK                        0xffffff
#define _SHR_GPORT_TUNNEL_SHIFT                         0
#define _SHR_GPORT_TUNNEL_MASK                          0x3ffffff
#define _SHR_GPORT_CHILD_SHIFT                          0
#define _SHR_GPORT_CHILD_MASK                           0x3ffffff
#define _SHR_GPORT_CHILD_MODID_SHIFT                    10
#define _SHR_GPORT_CHILD_MODID_MASK                     0xffff
#define _SHR_GPORT_CHILD_PORT_SHIFT                     0
#define _SHR_GPORT_CHILD_PORT_MASK                      0x3ff
#define _SHR_GPORT_EGRESS_GROUP_MODID_SHIFT             10
#define _SHR_GPORT_EGRESS_GROUP_MODID_MASK              0xffff
#define _SHR_GPORT_EGRESS_GROUP_SHIFT                   0
#define _SHR_GPORT_EGRESS_GROUP_MASK                    0x3ff
#define _SHR_GPORT_EGRESS_CHILD_SHIFT                   0
#define _SHR_GPORT_EGRESS_CHILD_MASK                    0x3ffffff
#define _SHR_GPORT_EGRESS_CHILD_PORT_SHIFT              0
#define _SHR_GPORT_EGRESS_CHILD_PORT_MASK               0x3ff
#define _SHR_GPORT_EGRESS_CHILD_MODID_SHIFT             10
#define _SHR_GPORT_EGRESS_CHILD_MODID_MASK              0xffff
#define _SHR_GPORT_EGRESS_MODPORT_SHIFT                 0
#define _SHR_GPORT_EGRESS_MODPORT_MASK                  0x3ffffff
#define _SHR_GPORT_EGRESS_MODPORT_MODID_SHIFT           10
#define _SHR_GPORT_EGRESS_MODPORT_MODID_MASK            0xffff
#define _SHR_GPORT_EGRESS_MODPORT_PORT_SHIFT            0
#define _SHR_GPORT_EGRESS_MODPORT_PORT_MASK             0x3ff
#define _SHR_GPORT_CONGESTION_SHIFT                     0
#define _SHR_GPORT_CONGESTION_MASK                      0xff
#define _SHR_GPORT_SYSTEM_PORT_SHIFT                    0
#define _SHR_GPORT_SYSTEM_PORT_MASK                     0xffffff
#define _SHR_GPORT_EPON_LINK_SHIFT                      0
#define _SHR_GPORT_EPON_LINK_MASK                       0xffff
#define _SHR_GPORT_EPON_LINK_NUM_SHIFT                  0
#define _SHR_GPORT_EPON_LINK_NUM_MASK                   0xffff
#define _SHR_GPORT_FORWARD_PORT_SHIFT                   0
#define _SHR_GPORT_FORWARD_PORT_MASK                    0xffffff
#define _SHR_GPORT_VP_GROUP_SHIFT                       0
#define _SHR_GPORT_VP_GROUP_MASK                        0x3ffffff
#define _SHR_ENCAP_ID_SHIFT                             0
#define _SHR_ENCAP_ID_MASK                              0xfffffff
#define _SHR_ENCAP_REMOTE_SHIFT                         28
#define _SHR_ENCAP_REMOTE_MASK                          0x1
#define _SHR_GPORT_FLEXE_GROUP_SHIFT                    0
#define _SHR_GPORT_FLEXE_GROUP_MASK                     0x7
#define _SHR_GPORT_TDM_STREAM_SHIFT                     0
#define _SHR_GPORT_TDM_STREAM_MASK                      0xFFFF

#define _SHR_GPORT_ISQ_ROOT                             (_SHR_GPORT_SCHEDULER_MASK - 1)
#define _SHR_GPORT_FMQ_ROOT                             (_SHR_GPORT_SCHEDULER_MASK - 2)
#define _SHR_GPORT_FMQ_GUARANTEED                       (_SHR_GPORT_SCHEDULER_MASK - 3)
#define _SHR_GPORT_FMQ_BESTEFFORT_AGR                   (_SHR_GPORT_SCHEDULER_MASK - 4)
#define _SHR_GPORT_FMQ_BESTEFFORT0                      (_SHR_GPORT_SCHEDULER_MASK - 5)
#define _SHR_GPORT_FMQ_BESTEFFORT1                      (_SHR_GPORT_SCHEDULER_MASK - 6)
#define _SHR_GPORT_FMQ_BESTEFFORT2                      (_SHR_GPORT_SCHEDULER_MASK - 7)
#define _SHR_GPORT_FABRIC_MESH_LOCAL                    (_SHR_GPORT_SCHEDULER_MASK - 8)
#define _SHR_GPORT_FABRIC_MESH_DEV1                     (_SHR_GPORT_SCHEDULER_MASK - 9)
#define _SHR_GPORT_FABRIC_MESH_DEV2                     (_SHR_GPORT_SCHEDULER_MASK - 10)
#define _SHR_GPORT_FABRIC_MESH_DEV3                     (_SHR_GPORT_SCHEDULER_MASK - 11)
#define _SHR_GPORT_FABRIC_MESH_DEV4                     (_SHR_GPORT_SCHEDULER_MASK - 12)
#define _SHR_GPORT_FABRIC_MESH_DEV5                     (_SHR_GPORT_SCHEDULER_MASK - 13)
#define _SHR_GPORT_FABRIC_MESH_DEV6                     (_SHR_GPORT_SCHEDULER_MASK - 14)
#define _SHR_GPORT_FABRIC_MESH_DEV7                     (_SHR_GPORT_SCHEDULER_MASK - 15)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL            (_SHR_GPORT_SCHEDULER_MASK - 16)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW        (_SHR_GPORT_SCHEDULER_MASK - 17)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH       (_SHR_GPORT_SCHEDULER_MASK - 18)
#define _SHR_GPORT_FABRIC_CLOS_FABRIC                   (_SHR_GPORT_SCHEDULER_MASK - 19)
#define _SHR_GPORT_FABRIC_CLOS_FABRIC_HIGH              (_SHR_GPORT_SCHEDULER_MASK - 20)
#define _SHR_GPORT_FABRIC_CLOS_FABRIC_LOW               (_SHR_GPORT_SCHEDULER_MASK - 21)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH      (_SHR_GPORT_SCHEDULER_MASK - 22)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW       (_SHR_GPORT_SCHEDULER_MASK - 23)
#define _SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED           (_SHR_GPORT_SCHEDULER_MASK - 24)
#define _SHR_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT           (_SHR_GPORT_SCHEDULER_MASK - 25)
#define _SHR_GPORT_FMQ_CLASS1                           (_SHR_GPORT_SCHEDULER_MASK - 26)
#define _SHR_GPORT_FMQ_CLASS2                           (_SHR_GPORT_SCHEDULER_MASK - 27)
#define _SHR_GPORT_FMQ_CLASS3                           (_SHR_GPORT_SCHEDULER_MASK - 28)
#define _SHR_GPORT_FMQ_CLASS4                           (_SHR_GPORT_SCHEDULER_MASK - 29)
#define _SHR_GPORT_FABRIC_MESH_COMMON_LOCAL0            (_SHR_GPORT_SCHEDULER_MASK - 30)
#define _SHR_GPORT_FABRIC_MESH_COMMON_LOCAL1            (_SHR_GPORT_SCHEDULER_MASK - 31)
#define _SHR_GPORT_FABRIC_MESH_COMMON_DEV1              (_SHR_GPORT_SCHEDULER_MASK - 32)
#define _SHR_GPORT_FABRIC_MESH_COMMON_DEV2              (_SHR_GPORT_SCHEDULER_MASK - 33)
#define _SHR_GPORT_FABRIC_MESH_COMMON_DEV3              (_SHR_GPORT_SCHEDULER_MASK - 34)
#define _SHR_GPORT_FABRIC_MESH_COMMON_MC                (_SHR_GPORT_SCHEDULER_MASK - 35)
#define _SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL0            (_SHR_GPORT_SCHEDULER_MASK - 36)
#define _SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL1            (_SHR_GPORT_SCHEDULER_MASK - 37)
#define _SHR_GPORT_FABRIC_CLOS_COMMON_FABRIC            (_SHR_GPORT_SCHEDULER_MASK - 38)
#define _SHR_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC    (_SHR_GPORT_SCHEDULER_MASK - 39)
#define _SHR_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC  (_SHR_GPORT_SCHEDULER_MASK - 40)
#define _SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 41)
#define _SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW           (_SHR_GPORT_SCHEDULER_MASK - 42)
#define _SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 43)
#define _SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW           (_SHR_GPORT_SCHEDULER_MASK - 44)
#define _SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 45)
#define _SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW           (_SHR_GPORT_SCHEDULER_MASK - 46)
#define _SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 47)
#define _SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW           (_SHR_GPORT_SCHEDULER_MASK - 48)
#define _SHR_GPORT_FABRIC_MESH_DEV1_OCB_HIGH            (_SHR_GPORT_SCHEDULER_MASK - 49)
#define _SHR_GPORT_FABRIC_MESH_DEV1_OCB_LOW             (_SHR_GPORT_SCHEDULER_MASK - 50)
#define _SHR_GPORT_FABRIC_MESH_DEV1_MIX_HIGH            (_SHR_GPORT_SCHEDULER_MASK - 51)
#define _SHR_GPORT_FABRIC_MESH_DEV1_MIX_LOW             (_SHR_GPORT_SCHEDULER_MASK - 52)
#define _SHR_GPORT_FABRIC_MESH_DEV2_OCB_HIGH            (_SHR_GPORT_SCHEDULER_MASK - 53)
#define _SHR_GPORT_FABRIC_MESH_DEV2_OCB_LOW             (_SHR_GPORT_SCHEDULER_MASK - 54)
#define _SHR_GPORT_FABRIC_MESH_DEV2_MIX_HIGH            (_SHR_GPORT_SCHEDULER_MASK - 55)
#define _SHR_GPORT_FABRIC_MESH_DEV2_MIX_LOW             (_SHR_GPORT_SCHEDULER_MASK - 56)
#define _SHR_GPORT_FABRIC_MESH_DEV3_OCB_HIGH            (_SHR_GPORT_SCHEDULER_MASK - 57)
#define _SHR_GPORT_FABRIC_MESH_DEV3_OCB_LOW             (_SHR_GPORT_SCHEDULER_MASK - 58)
#define _SHR_GPORT_FABRIC_MESH_DEV3_MIX_HIGH            (_SHR_GPORT_SCHEDULER_MASK - 59)
#define _SHR_GPORT_FABRIC_MESH_DEV3_MIX_LOW             (_SHR_GPORT_SCHEDULER_MASK - 60)
#define _SHR_GPORT_FABRIC_MESH_MC_OCB_HIGH              (_SHR_GPORT_SCHEDULER_MASK - 61)
#define _SHR_GPORT_FABRIC_MESH_MC_OCB_LOW               (_SHR_GPORT_SCHEDULER_MASK - 62)
#define _SHR_GPORT_FABRIC_MESH_MC_MIX_HIGH              (_SHR_GPORT_SCHEDULER_MASK - 63)
#define _SHR_GPORT_FABRIC_MESH_MC_MIX_LOW               (_SHR_GPORT_SCHEDULER_MASK - 64)
#define _SHR_GPORT_FABRIC_MESH_LOCAL0                   (_SHR_GPORT_SCHEDULER_MASK - 65)
#define _SHR_GPORT_FABRIC_MESH_LOCAL1                   (_SHR_GPORT_SCHEDULER_MASK - 66)
#define _SHR_GPORT_FABRIC_MESH_MC                       (_SHR_GPORT_SCHEDULER_MASK - 67)
#define _SHR_GPORT_FABRIC_MESH_SCOPE                    (_SHR_GPORT_SCHEDULER_MASK - 68)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 69)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW           (_SHR_GPORT_SCHEDULER_MASK - 70)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 71)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW           (_SHR_GPORT_SCHEDULER_MASK - 72)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 73)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW           (_SHR_GPORT_SCHEDULER_MASK - 74)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH          (_SHR_GPORT_SCHEDULER_MASK - 75)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW           (_SHR_GPORT_SCHEDULER_MASK - 76)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH  (_SHR_GPORT_SCHEDULER_MASK - 77)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW   (_SHR_GPORT_SCHEDULER_MASK - 78)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH  (_SHR_GPORT_SCHEDULER_MASK - 79)
#define _SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW   (_SHR_GPORT_SCHEDULER_MASK - 80)
#define _SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB       (_SHR_GPORT_SCHEDULER_MASK - 81)
#define _SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB      (_SHR_GPORT_SCHEDULER_MASK - 82)
#define _SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX       (_SHR_GPORT_SCHEDULER_MASK - 83)
#define _SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX      (_SHR_GPORT_SCHEDULER_MASK - 84)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0                   (_SHR_GPORT_SCHEDULER_MASK - 85)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1                   (_SHR_GPORT_SCHEDULER_MASK - 86)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0_HIGH              (_SHR_GPORT_SCHEDULER_MASK - 87)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL0_LOW               (_SHR_GPORT_SCHEDULER_MASK - 88)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1_HIGH              (_SHR_GPORT_SCHEDULER_MASK - 89)
#define _SHR_GPORT_FABRIC_CLOS_LOCAL1_LOW               (_SHR_GPORT_SCHEDULER_MASK - 90)
#define _SHR_GPORT_FABRIC_CLOS_OCB_HIGH                 (_SHR_GPORT_SCHEDULER_MASK - 91)
#define _SHR_GPORT_FABRIC_CLOS_MIX_HIGH                 (_SHR_GPORT_SCHEDULER_MASK - 92)
#define _SHR_GPORT_FABRIC_CLOS_OCB_LOW                  (_SHR_GPORT_SCHEDULER_MASK - 93)
#define _SHR_GPORT_FABRIC_CLOS_MIX_LOW                  (_SHR_GPORT_SCHEDULER_MASK - 94)
#define _SHR_GPORT_FABRIC_CLOS_SCOPE                    (_SHR_GPORT_SCHEDULER_MASK - 95)

 /*
  * MCAST GPORT subtype shift & mask definitions 
  */
#define _SHR_MCAST_GPORT_TYPE_SHIFT                      23
#define _SHR_MCAST_GPORT_TYPE_MASK                       0x7
#define _SHR_MCAST_GPORT_GROUP_ID_MASK                   ((1<<_SHR_MCAST_GPORT_TYPE_SHIFT)-1)

/* COSQ GPORT subtype shift & mask definitions */
#define _SHR_COSQ_GPORT_TYPE_SHIFT                      21
#define _SHR_COSQ_GPORT_TYPE_MASK                       0x1F
#define _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_PORT_MASK    0x7FF
#define _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_PORT_SHIFT   0
#define _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_PORT_MASK    0x7FF
#define _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_PORT_SHIFT   0
#define _SHR_COSQ_GPORT_VSQ_TYPE_MASK                   0xF
#define _SHR_COSQ_GPORT_VSQ_TYPE_SHIFT                  17
#define _SHR_COSQ_GPORT_VSQ_TYPE_INFO_MASK              0x1FFFF
#define _SHR_COSQ_GPORT_VSQ_TYPE_INFO_SHIFT             0
#define _SHR_VSQ_TYPE_INFO_CATEGORY_MASK                0x3
#define _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT               5
#define _SHR_VSQ_TYPE_INFO_TC_MASK                      0x7
#define _SHR_VSQ_TYPE_INFO_TC_SHIFT                     0
#define _SHR_VSQ_TYPE_INFO_CORE_MASK                    _SHR_COSQ_GPORT_COMMON_CORE_MASK
#define _SHR_VSQ_TYPE_INFO_CORE_SHIFT                   13
#define _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK                1
#define _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT               16
#define _SHR_VSQ_TYPE_INFO_CONNECTION_MASK              0xF
#define _SHR_VSQ_TYPE_INFO_CONNECTION_SHIFT             0
#define _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK                0x1FF
#define _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT               4
#define _SHR_VSQ_TYPE_INFO_PG_MASK                      0xFFF
#define _SHR_VSQ_TYPE_INFO_PG_SHIFT                     0
#define _SHR_VSQ_TYPE_INFO_STATISTICS_TAG_MASK          0x1FF
#define _SHR_VSQ_TYPE_INFO_STATISTICS_TAG_SHIFT         0
#define _SHR_COSQ_GPORT_E2E_PORT_MASK                   0x7FF
#define _SHR_COSQ_GPORT_E2E_PORT_SHIFT                  0
#define _SHR_COSQ_GPORT_ISQ_QID_SHIFT                   0
#define _SHR_COSQ_GPORT_ISQ_QID_MASK _SHR_COSQ_GPORT_COMMON_QUEUE_MASK
#define _SHR_COSQ_GPORT_MULTIPATH_SHIFT                 0
#define _SHR_COSQ_GPORT_MULTIPATH_MASK                  0x7fffff
#define _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_SHIFT         0
#define _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_MASK          0xfff
#define _SHR_COSQ_GPORT_ATTACH_ID_FCD_SHIFT             12
#define _SHR_COSQ_GPORT_ATTACH_ID_FCD_MASK              0x3ff
#define _SHR_COSQ_GPORT_INBAND_COE_MASK         0x5 /* Calandar 0~5 */
#define _SHR_COSQ_GPORT_INBAND_COE_SHIFT        0


#define _SHR_COSQ_GPORT_PORT_TC_SHIFT                   0
#define _SHR_COSQ_GPORT_PORT_TC_MASK                    0x7FF
#define _SHR_COSQ_GPORT_PORT_TCG_SHIFT                  0
#define _SHR_COSQ_GPORT_PORT_TCG_MASK                   0x7FF
#define _SHR_COSQ_GPORT_E2E_PORT_TC_SHIFT               0
#define _SHR_COSQ_GPORT_E2E_PORT_TC_MASK                0x7FF
#define _SHR_COSQ_GPORT_E2E_PORT_TCG_SHIFT              0
#define _SHR_COSQ_GPORT_E2E_PORT_TCG_MASK               0x7FF
#define _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT          16
#define _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_MASK           0x1f
#define _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT          9
#define _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK           0xf
#define _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT          0
#define _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK           0x1ff
#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_MASK       0xf
#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_SHIFT      7
#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TYPE_MASK       0x7f
#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TYPE_SHIFT      0
#define _SHR_COSQ_GPORT_CORE_SHIFT                      0
#define _SHR_COSQ_GPORT_CORE_MASK                       0xFF

#define _SHR_COSQ_GPORT_SRC_QUEUE_SHIFT                 0
#define _SHR_COSQ_GPORT_SRC_QUEUE_MASK                  0xFFFF
#define _SHR_COSQ_GPORT_DST_QUEUE_SHIFT                 0
#define _SHR_COSQ_GPORT_DST_QUEUE_MASK                  0xFFFF

#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_MASK 0x1f
#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT 0
#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_CPU_GUARANTEED 1
#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_RCY_GUARANTEED 2
#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_NIF 3

#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_SHIFT    18
#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_MASK     _SHR_COSQ_GPORT_COMMON_CORE_MASK

/* definitions for cosq gport type fabric handling */
#define _SHR_COSQ_FABRIC_TYPE_PIPE_GLOBAL           0
#define _SHR_COSQ_FABRIC_TYPE_PIPE_INGRESS          1
#define _SHR_COSQ_FABRIC_TYPE_PIPE_EGRESS           2
#define _SHR_COSQ_FABRIC_TYPE_PIPE_MIDDLE           3
#define _SHR_COSQ_FABRIC_PIPE_ALL_PIPES             _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK
#define _SHR_COSQ_FABRIC_PIPE_ALL_PORTS             _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK

/* COSQ_EXT GPORT subtype shift & mask definitions*/
#define _SHR_COSQ_EXT_GPORT_TYPE_SHIFT                      23
#define _SHR_COSQ_EXT_GPORT_TYPE_MASK                       0x7
#define _SHR_COSQ_EXT_GPORT_COMMON_FLOW_MASK                0x7FFFF
#define _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_ID_SHIFT          0
#define _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_ID_MASK           _SHR_COSQ_EXT_GPORT_COMMON_FLOW_MASK
#define _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_CORE_SHIFT        20
#define _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_CORE_MASK         _SHR_COSQ_GPORT_COMMON_CORE_MASK
#define _SHR_COSQ_EXT_GPORT_SCHED_CIR_SHIFT                 0
#define _SHR_COSQ_EXT_GPORT_SCHED_CIR_MASK                  _SHR_COSQ_EXT_GPORT_COMMON_FLOW_MASK
#define _SHR_COSQ_EXT_GPORT_SCHED_CIR_CORE_SHIFT            20
#define _SHR_COSQ_EXT_GPORT_SCHED_CIR_CORE_MASK             _SHR_COSQ_GPORT_COMMON_CORE_MASK
#define _SHR_COSQ_EXT_GPORT_SCHED_PIR_SHIFT                 0
#define _SHR_COSQ_EXT_GPORT_SCHED_PIR_MASK                  _SHR_COSQ_EXT_GPORT_COMMON_FLOW_MASK
#define _SHR_COSQ_EXT_GPORT_SCHED_PIR_CORE_SHIFT            20
#define _SHR_COSQ_EXT_GPORT_SCHED_PIR_CORE_MASK             _SHR_COSQ_GPORT_COMMON_CORE_MASK
#define _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_FLOW_SHIFT   0
#define _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_FLOW_MASK    _SHR_COSQ_EXT_GPORT_COMMON_FLOW_MASK
#define _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_CORE_SHIFT   20
#define _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_CORE_MASK    _SHR_COSQ_GPORT_COMMON_CORE_MASK
#define _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_FLOW_SHIFT        0
#define _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_FLOW_MASK         _SHR_COSQ_EXT_GPORT_COMMON_FLOW_MASK
#define _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_SHIFT        20
#define _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_MASK         _SHR_COSQ_GPORT_COMMON_CORE_MASK

/* Extender GPORT subtype shift & mask definitions */
#define _SHR_GPORT_EXTENDER_PORT_SHIFT                  (0)
#define _SHR_GPORT_EXTENDER_PORT_MASK                   (0x3FFFFFF)
#define _SHR_EXTENDER_GPORT_TYPE_SHIFT                  (23)
#define _SHR_EXTENDER_GPORT_TYPE_MASK                   (0x7)

/* Profile GPORT subtype shift & mask definitions */
#define _SHR_GPORT_PROFILE_SHIFT                        0
#define _SHR_GPORT_PROFILE_MASK                         0xffff
#define _SHR_GPORT_PROFILE_TYPE_MASK                    0x1f
#define _SHR_GPORT_PROFILE_TYPE_SHIFT                   21

#define _SHR_GPORT_TYPE_TRAP                            ((_SHR_GPORT_TYPE_LOCAL_CPU << 1) | 1)  /* This will mark the
                                                                                                 * CPU type, and the
                                                                                                 * 1st bit in the value 
                                                                                                 * * * to indicate trap 
                                                                                                 */
#define _SHR_GPORT_TYPE_TRAP_SHIFT                      (_SHR_GPORT_TYPE_SHIFT-1)       /* 25 */
#define _SHR_GPORT_TYPE_TRAP_MASK                       (_SHR_GPORT_TYPE_MASK<<1|1)     /* 0x4f */
#define _SHR_GPORT_TRAP_ID_SHIFT                         0
#define _SHR_GPORT_TRAP_ID_MASK                          0xfff
#define _SHR_GPORT_TRAP_STRENGTH_SHIFT          	     12
#define _SHR_GPORT_TRAP_STRENGTH_MASK          		     0xf
#define _SHR_GPORT_TRAP_SNOOP_STRENGTH_SHIFT             16
#define _SHR_GPORT_TRAP_SNOOP_STRENGTH_MASK              0xf

#define _SHR_PHY_GPORT_TYPE_SHIFT 20
#define _SHR_PHY_GPORT_TYPE_MASK 0x3F

#define _SHR_PHY_GPORT_TYPE_LANE_PORT 1
#define _SHR_PHY_GPORT_LANE_PORT_PORT_MASK 0x3ff
#define _SHR_PHY_GPORT_LANE_PORT_LANE_MASK 0x3ff
#define _SHR_PHY_GPORT_LANE_PORT_PORT_SHIFT 0
#define _SHR_PHY_GPORT_LANE_PORT_LANE_SHIFT 10

#define _SHR_PHY_GPORT_TYPE_PHYN_PORT 2
#define _SHR_PHY_GPORT_PHYN_PORT_PHYN_MASK 0x7
#define _SHR_PHY_GPORT_PHYN_PORT_PHYN_SHIFT 17
#define _SHR_PHY_GPORT_PHYN_PORT_PORT_MASK 0x3FF
#define _SHR_PHY_GPORT_PHYN_PORT_PORT_SHIFT 0

#define _SHR_PHY_GPORT_TYPE_PHYN_LANE_PORT 3
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_MASK 0x7
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_SHIFT 17
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_MASK 0x7F
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_SHIFT 10
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_MASK 0x3FF
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_SHIFT 0

#define _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_PORT 4
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_MASK 0x7
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_SHIFT 17
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_MASK 0x3FF
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_SHIFT 0

#define _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_LANE_PORT 5
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_MASK 0x7
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_SHIFT 17
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_MASK 0x7F
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_SHIFT 10
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_MASK 0x3FF
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_SHIFT 0

#define _SHR_GPORT_LOCAL_TYPE_COMMON    (0)
#define _SHR_GPORT_LOCAL_TYPE_FABRIC    (1)
#define _SHR_GPORT_LOCAL_TYPE_INTERFACE (2)
#define _SHR_GPORT_LOCAL_TYPE_SHIFT (21)
#define _SHR_GPORT_LOCAL_TYPE_MASK (0x1f)

#define _SHR_COSQ_VSQ_GL         0x0
#define _SHR_COSQ_VSQ_CT         0x1
#define _SHR_COSQ_VSQ_CTTC       0x2
#define _SHR_COSQ_VSQ_CTCC       0x3
#define _SHR_COSQ_VSQ_PP         0x4
#define _SHR_COSQ_VSQ_SRC_PORT   0x5
#define _SHR_COSQ_VSQ_PG         0x6

/*
 * NIF Rx priority Gport subtypes, shift & mask definitions 
 */
#define _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT   23
#define _SHR_GPORT_NIF_RX_PRIORITY_TYPE_MASK    0x3
#define _SHR_GPORT_NIF_RX_PRIORITY_TYPE_LOW     0   /* NIF Rx priority LOW */
#define _SHR_GPORT_NIF_RX_PRIORITY_TYPE_HIGH    1   /* NIF Rx priority HIGH */
#define _SHR_GPORT_NIF_RX_PRIORITY_TYPE_TDM     2   /* NIF Rx priority HIGH */

#define _SHR_GPORT_IS_SET(_gport)    \
        (((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) > 0) && \
         ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) <= _SHR_GPORT_TYPE_MAX))

#define _SHR_GPORT_IS_LOCAL(_gport) \
        (((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
          _SHR_GPORT_TYPE_LOCAL) && \
        ((((_gport) >> _SHR_GPORT_LOCAL_TYPE_SHIFT) & _SHR_GPORT_LOCAL_TYPE_MASK) == \
          _SHR_GPORT_LOCAL_TYPE_COMMON))

#define _SHR_GPORT_LOCAL_SET(_gport, _port)\
        ((_gport) = (_SHR_GPORT_TYPE_LOCAL << _SHR_GPORT_TYPE_SHIFT) |\
        (_SHR_GPORT_LOCAL_TYPE_COMMON << _SHR_GPORT_LOCAL_TYPE_SHIFT)|\
        (((_port) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_LOCAL_GET(_gport) \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_IS_MODPORT(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MODPORT)

#define _SHR_GPORT_MODPORT_SET(_gport, _module, _port)                       \
        ((_gport) = (_SHR_GPORT_TYPE_MODPORT   << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_module) & _SHR_GPORT_MODID_MASK)  << _SHR_GPORT_MODID_SHIFT)  | \
         (((_port) & _SHR_GPORT_PORT_MASK)     << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_MODPORT_MODID_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_MODID_SHIFT) & _SHR_GPORT_MODID_MASK)

#define _SHR_GPORT_MODPORT_PORT_GET(_gport)     \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_IS_TRUNK(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_TRUNK)

#define _SHR_GPORT_TRUNK_SET(_gport, _trunk_id)                              \
        ((_gport) = (_SHR_GPORT_TYPE_TRUNK      << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_trunk_id) & _SHR_GPORT_TRUNK_MASK) << _SHR_GPORT_TRUNK_SHIFT))

#define _SHR_GPORT_TRUNK_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_TRUNK_SHIFT) & _SHR_GPORT_TRUNK_MASK)

#define _SHR_GPORT_IS_MPLS_PORT(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MPLS_PORT)

#define _SHR_GPORT_MPLS_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_MPLS_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_MPLS_PORT_MASK) << _SHR_GPORT_MPLS_PORT_SHIFT))

#define _SHR_GPORT_MPLS_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MPLS_PORT_SHIFT) & _SHR_GPORT_MPLS_PORT_MASK)

#define _SHR_GPORT_IS_SUBPORT_GROUP(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_SUBPORT_GROUP)

#define _SHR_GPORT_SUBPORT_GROUP_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_SUBPORT_GROUP << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_SUBPORT_GROUP_MASK) << _SHR_GPORT_SUBPORT_GROUP_SHIFT))

#define _SHR_GPORT_SUBPORT_GROUP_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_SUBPORT_GROUP_SHIFT) & _SHR_GPORT_SUBPORT_GROUP_MASK)

#define _SHR_GPORT_IS_SUBPORT_PORT(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_SUBPORT_PORT)

#define _SHR_GPORT_SUBPORT_PORT_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_SUBPORT_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_SUBPORT_PORT_MASK) << _SHR_GPORT_SUBPORT_PORT_SHIFT))

#define _SHR_GPORT_SUBPORT_PORT_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_SUBPORT_PORT_SHIFT) & _SHR_GPORT_SUBPORT_PORT_MASK)

#define _SHR_GPORT_IS_SCHEDULER(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_SCHEDULER)

#define _SHR_GPORT_SCHEDULER_SET(_gport, _id)                            \
        _SHR_GPORT_SCHEDULER_CORE_SET(_gport, _id, _SHR_CORE_ALL)

#define _SHR_GPORT_SCHEDULER_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK)

#define _SHR_GPORT_SCHEDULER_ONLY_CORE_SET(_gport, _core_id)   \
        ((_gport) = (_gport &  (~(_SHR_GPORT_SCHEDULER_CORE_MASK << _SHR_GPORT_SCHEDULER_CORE_SHIFT))) | \
        (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define  _SHR_GPORT_SCHEDULER_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID((((_gport) >> _SHR_GPORT_SCHEDULER_CORE_SHIFT) & _SHR_GPORT_SCHEDULER_CORE_MASK))

#define _SHR_GPORT_SCHEDULER_CORE_SET(_gport, _scheduler_id, _core_id)       \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_scheduler_id) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_GPORT_SCHEDULER_NODE_SET(_gport, _level, _node)		\
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_level) & _SHR_GPORT_SCHEDULER_LEVEL_MASK) << _SHR_GPORT_SCHEDULER_LEVEL_SHIFT) | \
         (((_node) & _SHR_GPORT_SCHEDULER_NODE_MASK) << _SHR_GPORT_SCHEDULER_NODE_SHIFT))

#define _SHR_GPORT_SCHEDULER_LEVEL_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_SCHEDULER_LEVEL_SHIFT) & _SHR_GPORT_SCHEDULER_LEVEL_MASK)

#define _SHR_GPORT_SCHEDULER_NODE_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_SCHEDULER_NODE_SHIFT) & _SHR_GPORT_SCHEDULER_NODE_MASK)

#define _SHR_GPORT_IS_DEVPORT(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_DEVPORT)

#define _SHR_GPORT_DEVPORT(_device, _port)                       \
        ((_SHR_GPORT_TYPE_DEVPORT   << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_device) & _SHR_GPORT_DEVID_MASK)  << _SHR_GPORT_DEVID_SHIFT)  | \
         (((_port) & _SHR_GPORT_PORT_MASK)     << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_DEVPORT_SET(_gport, _device, _port)                       \
        ((_gport) = _SHR_GPORT_DEVPORT(_device, _port))

#define _SHR_GPORT_DEVPORT_DEVID_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_DEVID_SHIFT) & _SHR_GPORT_DEVID_MASK)

#define _SHR_GPORT_DEVPORT_PORT_GET(_gport)     \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_BLACK_HOLE    \
        (_SHR_GPORT_TYPE_BLACK_HOLE << _SHR_GPORT_TYPE_SHIFT)

#define _SHR_GPORT_IS_BLACK_HOLE(_gport)  ((_gport) == _SHR_GPORT_BLACK_HOLE)

#define _SHR_GPORT_LOCAL_CPU	    \
        (_SHR_GPORT_TYPE_LOCAL_CPU << _SHR_GPORT_TYPE_SHIFT)

#define _SHR_GPORT_LOCAL_CPU_IEEE	    \
        (_SHR_GPORT_TYPE_LOCAL_CPU_IEEE << _SHR_GPORT_TYPE_SHIFT)

#define _SHR_GPORT_LOCAL_CPU_HIGIG	    \
        (_SHR_GPORT_TYPE_LOCAL_CPU_HIGIG << _SHR_GPORT_TYPE_SHIFT)

#define _SHR_GPORT_IS_LOCAL_CPU(_gport)   ((_gport) == _SHR_GPORT_LOCAL_CPU)

#define _SHR_GPORT_IS_LOCAL_CPU_IEEE(_gport)   ((_gport) == _SHR_GPORT_LOCAL_CPU_IEEE)

#define _SHR_GPORT_IS_LOCAL_CPU_HIGIG(_gport)   ((_gport) == _SHR_GPORT_LOCAL_CPU_HIGIG)

#define _SHR_GPORT_IS_DESTMOD_QUEUE_GROUP(_gport)    \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) \
         == _SHR_GPORT_TYPE_DESTMOD_QUEUE_GROUP)

#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_SET(_gport, _qid)       \
            _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSQID_SET(_gport,  \
            _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_MASK, \
             _qid)

#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_DESTMOD_QUEUE_GROUP \
            << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_sysport_id) & _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_MASK) \
          << _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_SHIFT)  | \
         (((_qid) & _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_MASK) \
          << _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_SHIFT) \
         & _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_MASK)

#define _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_SHIFT) \
         & _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_MASK)

#define _SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_UCAST_QUEUE_GROUP)

#define _SHR_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _qid)                           \
            _SHR_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(_gport,                      \
            _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_MASK, \
             _qid)

#define _SHR_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)                       \
        ((_gport) = (_SHR_GPORT_TYPE_UCAST_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_sysport_id) & _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_MASK)  << _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_SHIFT)  | \
         (((_qid) & _SHR_GPORT_UCAST_QUEUE_GROUP_QID_MASK)     << _SHR_GPORT_UCAST_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_SHIFT) & _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_MASK)

#define _SHR_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_UCAST_QUEUE_GROUP_QID_SHIFT) & _SHR_GPORT_UCAST_QUEUE_GROUP_QID_MASK)

#define _SHR_GPORT_UNICAST_QUEUE_GROUP_SET(_gport, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_UCAST_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
        (((_qid) & _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_MASK) << _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(_gport, _core, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_UCAST_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_CORE_ID2FIELD(_core) << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
        (((_qid) & _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_MASK) << _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_GET(_gport)                               \
        (((_gport) >> _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_SHIFT) & _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_MASK)

#define _SHR_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID(((_gport) >> _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) & _SHR_COSQ_GPORT_COMMON_CORE_MASK)

#define _SHR_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_UCAST_SUBSCRIBER_QUEUE_GROUP)

#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(_gport, _qid)          \
        ((_gport) = (_SHR_GPORT_TYPE_UCAST_SUBSCRIBER_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT)   | \
        (((_qid) & _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK) << _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT) & \
                              _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK)

#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_SHIFT) \
         & _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_MASK)

#define _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)                       \
        ((_gport) = (_SHR_GPORT_TYPE_UCAST_SUBSCRIBER_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT) | \
         (((_sysport_id) & _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_MASK)  << _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_SHIFT) | \
         (((_qid) & _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK)     << _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_IS_MCAST(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MCAST)

#define _SHR_MCAST_GPORT_IS_BITMAP_ID(_gport)                                       \
        (_SHR_GPORT_IS_MCAST(_gport) &&                                                 \
        ((((_gport) >> _SHR_MCAST_GPORT_TYPE_SHIFT) & _SHR_MCAST_GPORT_TYPE_MASK) ==     \
                                                _SHR_MCAST_GPORT_TYPE_BITMAP_ID))

#define _SHR_MCAST_GPORT_IS_GROUP_ID(_gport)                                       \
        (_SHR_GPORT_IS_MCAST(_gport) &&                                                 \
        ((((_gport) >> _SHR_MCAST_GPORT_TYPE_SHIFT) & _SHR_MCAST_GPORT_TYPE_MASK) ==     \
                                                _SHR_MCAST_GPORT_TYPE_GROUP_ID))

#define _SHR_MCAST_GPORT_IS_STREAM_ID(_gport)                                       \
        (_SHR_GPORT_IS_MCAST(_gport) &&                                                 \
        ((((_gport) >> _SHR_MCAST_GPORT_TYPE_SHIFT) & _SHR_MCAST_GPORT_TYPE_MASK) ==     \
                                                _SHR_MCAST_GPORT_TYPE_STREAM_ID))

#define _SHR_GPORT_MCAST_SET(_gport, _mcast_id)                              \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST      << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_mcast_id) & _SHR_GPORT_MCAST_MASK) << _SHR_GPORT_MCAST_SHIFT))

#define _SHR_MCAST_GPORT_GROUP_ID_SET(_gport, _mcast_id)                              \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST << _SHR_GPORT_TYPE_SHIFT)  | \
         (_SHR_MCAST_GPORT_TYPE_GROUP_ID << _SHR_MCAST_GPORT_TYPE_SHIFT)  | \
         (((_mcast_id) & _SHR_MCAST_GPORT_GROUP_ID_MASK) << _SHR_GPORT_MCAST_SHIFT))

#define _SHR_MCAST_GPORT_BITMAP_ID_SET(_gport, _bmp_id)                              \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST      << _SHR_GPORT_TYPE_SHIFT)  | \
         (_SHR_MCAST_GPORT_TYPE_BITMAP_ID      << _SHR_MCAST_GPORT_TYPE_SHIFT)  | \
         (((_bmp_id) & _SHR_MCAST_GPORT_GROUP_ID_MASK) << _SHR_GPORT_MCAST_SHIFT))

#define _SHR_MCAST_GPORT_STREAM_ID_SET(_gport, _stream_id)                              \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST      << _SHR_GPORT_TYPE_SHIFT)  | \
         (_SHR_MCAST_GPORT_TYPE_STREAM_ID      << _SHR_MCAST_GPORT_TYPE_SHIFT)  | \
         (((_stream_id) & _SHR_MCAST_GPORT_GROUP_ID_MASK) << _SHR_GPORT_MCAST_SHIFT))

#define _SHR_GPORT_MCAST_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_SHIFT) & _SHR_GPORT_MCAST_MASK)

#define _SHR_MCAST_GPORT_GROUP_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_SHIFT) & _SHR_MCAST_GPORT_GROUP_ID_MASK)

#define _SHR_MCAST_GPORT_BITMAP_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_SHIFT) & _SHR_MCAST_GPORT_GROUP_ID_MASK)

#define _SHR_MCAST_GPORT_STREAM_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_SHIFT) & _SHR_MCAST_GPORT_GROUP_ID_MASK)

#define _SHR_GPORT_MCAST_DS_ID_SET(_gport, _ds_id) \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST      << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_ds_id) & _SHR_GPORT_MCAST_DS_ID_MASK) << _SHR_GPORT_MCAST_DS_ID_SHIFT))

#define _SHR_GPORT_MCAST_DS_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_DS_ID_SHIFT) & _SHR_GPORT_MCAST_DS_ID_MASK)

#define _SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MCAST_QUEUE_GROUP)

#define _SHR_GPORT_MCAST_QUEUE_GROUP_SET(_gport, _qid)            \
        _SHR_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(_gport,           \
        _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_MASK,              \
         _qid)

#define _SHR_GPORT_MCAST_QUEUE_GROUP_GET(_gport)   \
        _SHR_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)

#define _SHR_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)                       \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_sysport_id) & _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_MASK)  << _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_SHIFT)  | \
         (((_qid) & _SHR_GPORT_MCAST_QUEUE_GROUP_QID_MASK)     << _SHR_GPORT_MCAST_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_SHIFT) & _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_MASK)

#define _SHR_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)                       \
         (((_gport) >> _SHR_GPORT_MCAST_QUEUE_GROUP_QID_SHIFT) & _SHR_GPORT_MCAST_QUEUE_GROUP_QID_MASK)

/* for multicast queue gports representing queue ID + core ID */
#define _SHR_GPORT_MCAST_QUEUE_GROUP_QUEUE_SET(_gport, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
         (((_qid) & _SHR_COSQ_GPORT_COMMON_QUEUE_MASK) << _SHR_GPORT_MCAST_QUEUE_GROUP_QID_SHIFT))
#define _SHR_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(_gport, _core, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_CORE_ID2FIELD(_core) << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
         (((_qid) & _SHR_COSQ_GPORT_COMMON_QUEUE_MASK) << _SHR_GPORT_MCAST_QUEUE_GROUP_QID_SHIFT))
#define _SHR_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(_gport) \
        (_SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport) == _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_MASK ? \
        _SHR_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport) : \
        (((_gport) >> _SHR_GPORT_MCAST_QUEUE_GROUP_QID_SHIFT) & _SHR_COSQ_GPORT_COMMON_QUEUE_MASK))
#define _SHR_GPORT_MCAST_QUEUE_GROUP_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID(((_gport) >> _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) & _SHR_COSQ_GPORT_COMMON_CORE_MASK)

#define _SHR_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MCAST_SUBSCRIBER_QUEUE_GROUP)

#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(_gport, _qid)           	        \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST_SUBSCRIBER_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT)   | \
        (((_qid) & _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK) << _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_SHIFT) \
         & _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_MASK)

#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)                       \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST_SUBSCRIBER_QUEUE_GROUP << _SHR_GPORT_TYPE_SHIFT) | \
         (((_sysport_id) & _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_MASK)  << _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_SHIFT) | \
         (((_qid) & _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK)     << _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT))

#define _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(_gport)                       \
        (((_gport) >> _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SHIFT) & \
                              _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_MASK)

#define _SHR_GPORT_IS_SPECIAL(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_SPECIAL)

#define _SHR_GPORT_SPECIAL(_value)                              \
        ((_SHR_GPORT_TYPE_SPECIAL << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_value) & _SHR_GPORT_SPECIAL_MASK) << _SHR_GPORT_SPECIAL_SHIFT))

#define _SHR_GPORT_SPECIAL_SET(_gport, _value)  \
        ((_gport) = _SHR_GPORT_SPECIAL(_value))

#define _SHR_GPORT_SPECIAL_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_SPECIAL_SHIFT) & _SHR_GPORT_SPECIAL_MASK)

#define _SHR_GPORT_IS_MIRROR(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MIRROR)

#define _SHR_GPORT_MIRROR_SET(_gport, _value)                               \
        ((_gport) = (_SHR_GPORT_TYPE_MIRROR << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_value) & _SHR_GPORT_MIRROR_MASK) << _SHR_GPORT_MIRROR_SHIFT))

#define _SHR_GPORT_MIRROR_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_MIRROR_SHIFT) & _SHR_GPORT_MIRROR_MASK)

#define _SHR_GPORT_MIRROR_FULL_SET(_gport, _value, _mir_subtype) \
        ((_gport) = (_SHR_GPORT_TYPE_MIRROR << _SHR_GPORT_TYPE_SHIFT) | \
         (((_value) & _SHR_GPORT_MIRROR_MASK) << _SHR_GPORT_MIRROR_SHIFT) | \
         (((_mir_subtype) & _SHR_GPORT_MIRROR_SUBTYPE_MASK) << _SHR_GPORT_MIRROR_SUBTYPE_SHIFT))
#define _SHR_GPORT_MIRROR_SNOOP_SET(_gport, _value) _SHR_GPORT_MIRROR_FULL_SET(_gport, _value, _SHR_GPORT_MIRROR_SUBTYPE_VAL_SNOOP)
#define _SHR_GPORT_MIRROR_STAT_SAMPLE_SET(_gport, _value) _SHR_GPORT_MIRROR_FULL_SET(_gport, _value, _SHR_GPORT_MIRROR_SUBTYPE_VAL_STAT_SAMPLE)
#define _SHR_GPORT_MIRROR_SUBTYPE_GET(_gport) \
        (((_gport) >> _SHR_GPORT_MIRROR_SUBTYPE_SHIFT) & _SHR_GPORT_MIRROR_SUBTYPE_MASK)
#define _SHR_GPORT_IS_MIRROR_MIRROR(_gport) \
        (_SHR_GPORT_IS_MIRROR(_gport) && (_SHR_GPORT_MIRROR_SUBTYPE_GET(_gport) == _SHR_GPORT_MIRROR_SUBTYPE_VAL_MIRROR))
#define _SHR_GPORT_IS_MIRROR_SNOOP(_gport) \
        (_SHR_GPORT_IS_MIRROR(_gport) && (_SHR_GPORT_MIRROR_SUBTYPE_GET(_gport) == _SHR_GPORT_MIRROR_SUBTYPE_VAL_SNOOP))
#define _SHR_GPORT_IS_MIRROR_STAT_SAMPLE(_gport) \
        (_SHR_GPORT_IS_MIRROR(_gport) && (_SHR_GPORT_MIRROR_SUBTYPE_GET(_gport) == _SHR_GPORT_MIRROR_SUBTYPE_VAL_STAT_SAMPLE))

#define _SHR_GPORT_IS_MIM_PORT(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MIM_PORT)

#define _SHR_GPORT_MIM_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_MIM_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_MIM_PORT_MASK) << _SHR_GPORT_MIM_PORT_SHIFT))

#define _SHR_GPORT_MIM_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MIM_PORT_SHIFT) & _SHR_GPORT_MIM_PORT_MASK)

#define _SHR_GPORT_IS_VLAN_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_VLAN_PORT)

#define _SHR_GPORT_VLAN_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_VLAN_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_VLAN_PORT_MASK) << _SHR_GPORT_VLAN_PORT_SHIFT))

#define _SHR_GPORT_VLAN_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_VLAN_PORT_SHIFT) & _SHR_GPORT_VLAN_PORT_MASK)

#define _SHR_GPORT_IS_WLAN_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_WLAN_PORT)

#define _SHR_GPORT_WLAN_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_WLAN_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_WLAN_PORT_MASK) << _SHR_GPORT_WLAN_PORT_SHIFT))

#define _SHR_GPORT_WLAN_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_WLAN_PORT_SHIFT) & _SHR_GPORT_WLAN_PORT_MASK)

#define _SHR_GPORT_IS_TRILL_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_TRILL_PORT)

#define _SHR_GPORT_TRILL_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_TRILL_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_TRILL_PORT_MASK) << _SHR_GPORT_TRILL_PORT_SHIFT))

#define _SHR_GPORT_TRILL_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_TRILL_PORT_SHIFT) & _SHR_GPORT_TRILL_PORT_MASK)

#define _SHR_GPORT_IS_NIV_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_NIV_PORT)

#define _SHR_GPORT_NIV_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_NIV_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_NIV_PORT_MASK) << _SHR_GPORT_NIV_PORT_SHIFT))

#define _SHR_GPORT_NIV_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_NIV_PORT_SHIFT) & _SHR_GPORT_NIV_PORT_MASK)

#define _SHR_GPORT_IS_L2GRE_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_L2GRE_PORT)

#define _SHR_GPORT_L2GRE_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_L2GRE_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_L2GRE_PORT_MASK) << _SHR_GPORT_L2GRE_PORT_SHIFT))

#define _SHR_GPORT_L2GRE_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_L2GRE_PORT_SHIFT) & _SHR_GPORT_L2GRE_PORT_MASK)

#define _SHR_GPORT_IS_VXLAN_PORT(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_VXLAN_PORT)

#define _SHR_GPORT_VXLAN_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_VXLAN_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_VXLAN_PORT_MASK) << _SHR_GPORT_VXLAN_PORT_SHIFT))

#define _SHR_GPORT_VXLAN_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_VXLAN_PORT_SHIFT) & _SHR_GPORT_VXLAN_PORT_MASK)

#define _SHR_GPORT_IS_FLOW_PORT(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_FLOW_PORT)

#define _SHR_GPORT_FLOW_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_FLOW_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_FLOW_PORT_MASK) << _SHR_GPORT_FLOW_PORT_SHIFT))

#define _SHR_GPORT_FLOW_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_FLOW_PORT_SHIFT) & _SHR_GPORT_FLOW_PORT_MASK)

#define _SHR_GPORT_IS_VPLAG_PORT(_gport)    \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_VPLAG_PORT)

#define _SHR_GPORT_VPLAG_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_VPLAG_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_VPLAG_PORT_MASK) << _SHR_GPORT_VPLAG_PORT_SHIFT))

#define _SHR_GPORT_VPLAG_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_VPLAG_PORT_SHIFT) & _SHR_GPORT_VPLAG_PORT_MASK)

/* FlexE group gport utility macros */
#define _SHR_GPORT_IS_FLEXE_GROUP(_gport)    \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_FLEXE_GROUP)

#define _SHR_FLEXE_GROUP_GPORT_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_FLEXE_GROUP << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_FLEXE_GROUP_MASK) << _SHR_GPORT_FLEXE_GROUP_SHIFT))

#define _SHR_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_FLEXE_GROUP_SHIFT) & _SHR_GPORT_FLEXE_GROUP_MASK)

/* NIF Rx priority utility macros */
#define _SHR_GPORT_IS_NIF_RX_PRIORITY(_gport)    \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == _SHR_GPORT_TYPE_NIF_RX_PRIORITY)

#define _SHR_GPORT_NIF_RX_PRIORITY_GET(_gport)  \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_NIF_RX_PRIORITY_LOW_SET(_gport, _port)  \
        ((_gport) = (_SHR_GPORT_TYPE_NIF_RX_PRIORITY << _SHR_GPORT_TYPE_SHIFT) |  \
         (_SHR_GPORT_NIF_RX_PRIORITY_TYPE_LOW << _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT) |  \
         (((_port) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_NIF_RX_PRIORITY_HIGH_SET(_gport, _port)  \
        ((_gport) = (_SHR_GPORT_TYPE_NIF_RX_PRIORITY << _SHR_GPORT_TYPE_SHIFT) |  \
         (_SHR_GPORT_NIF_RX_PRIORITY_TYPE_HIGH << _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT) |  \
         (((_port) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_NIF_RX_PRIORITY_TDM_SET(_gport, _port)  \
        ((_gport) = (_SHR_GPORT_TYPE_NIF_RX_PRIORITY << _SHR_GPORT_TYPE_SHIFT) |  \
         (_SHR_GPORT_NIF_RX_PRIORITY_TYPE_TDM << _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT) |  \
         (((_port) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_IS_NIF_RX_PRIORITY_LOW(_gport)  \
        (_SHR_GPORT_IS_NIF_RX_PRIORITY(_gport) &&  \
        ((((_gport) >> _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT) & _SHR_GPORT_NIF_RX_PRIORITY_TYPE_MASK) ==  \
          _SHR_GPORT_NIF_RX_PRIORITY_TYPE_LOW))

#define _SHR_GPORT_IS_NIF_RX_PRIORITY_HIGH(_gport)  \
        (_SHR_GPORT_IS_NIF_RX_PRIORITY(_gport) &&  \
        ((((_gport) >> _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT) & _SHR_GPORT_NIF_RX_PRIORITY_TYPE_MASK) ==  \
          _SHR_GPORT_NIF_RX_PRIORITY_TYPE_HIGH))

#define _SHR_GPORT_IS_NIF_RX_PRIORITY_TDM(_gport)  \
            (_SHR_GPORT_IS_NIF_RX_PRIORITY(_gport) &&  \
            ((((_gport) >> _SHR_GPORT_NIF_RX_PRIORITY_TYPE_SHIFT) & _SHR_GPORT_NIF_RX_PRIORITY_TYPE_MASK) ==  \
              _SHR_GPORT_NIF_RX_PRIORITY_TYPE_TDM))

 /* TDM Stream gport utility macros */
 #define _SHR_GPORT_IS_TDM_STREAM(_gport)    \
         ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                 _SHR_GPORT_TYPE_TDM_STREAM)

 #define _SHR_GPORT_TDM_STREAM_SET(_gport, _id)                            \
         ((_gport) = (_SHR_GPORT_TYPE_TDM_STREAM << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_TDM_STREAM_MASK) << _SHR_GPORT_TDM_STREAM_SHIFT))

 #define _SHR_GPORT_TDM_STREAM_ID_GET(_gport)                                   \
         (((_gport) >> _SHR_GPORT_TDM_STREAM_SHIFT) & _SHR_GPORT_TDM_STREAM_MASK)

/* Extender GPORT Type & Subtype utility macros */
#define _SHR_GPORT_IS_EXTENDER_PORT(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_EXTENDER_PORT)
#define _SHR_EXTENDER_GPORT_IS_PORT(_gport)                                                 \
        (_SHR_GPORT_IS_EXTENDER_PORT(_gport) &&                                             \
         ((((_gport) >> _SHR_EXTENDER_GPORT_TYPE_SHIFT) & _SHR_EXTENDER_GPORT_TYPE_MASK) == \
          _SHR_EXTENDER_GPORT_TYPE_PORT))

#define _SHR_EXTENDER_GPORT_IS_FORWARD(_gport)                                              \
        (_SHR_GPORT_IS_EXTENDER_PORT(_gport) &&                                             \
         ((((_gport) >> _SHR_EXTENDER_GPORT_TYPE_SHIFT) & _SHR_EXTENDER_GPORT_TYPE_MASK) == \
          _SHR_EXTENDER_GPORT_TYPE_FORWARD))

#define _SHR_EXTENDER_GPORT_IS_ENCAP(_gport)                                                \
        (_SHR_GPORT_IS_EXTENDER_PORT(_gport) &&                                             \
         ((((_gport) >> _SHR_EXTENDER_GPORT_TYPE_SHIFT) & _SHR_EXTENDER_GPORT_TYPE_MASK) == \
          _SHR_EXTENDER_GPORT_TYPE_ENCAP))

#define _SHR_EXTENDER_GPORT_PORT_SET(_gport, _id)                                           \
        ((_gport) = (_SHR_GPORT_TYPE_EXTENDER_PORT << _SHR_GPORT_TYPE_SHIFT)            |   \
         (_SHR_EXTENDER_GPORT_TYPE_PORT << _SHR_EXTENDER_GPORT_TYPE_SHIFT)              |   \
         (((_id) & _SHR_GPORT_EXTENDER_PORT_MASK) << _SHR_GPORT_EXTENDER_PORT_SHIFT))

#define _SHR_EXTENDER_GPORT_FORWARD_SET(_gport, _id)                                        \
        ((_gport) = (_SHR_GPORT_TYPE_EXTENDER_PORT << _SHR_GPORT_TYPE_SHIFT)            |   \
         (_SHR_EXTENDER_GPORT_TYPE_FORWARD << _SHR_EXTENDER_GPORT_TYPE_SHIFT)           |   \
         (((_id) & _SHR_GPORT_EXTENDER_PORT_MASK) << _SHR_GPORT_EXTENDER_PORT_SHIFT))

#define _SHR_EXTENDER_GPORT_ENCAP_SET(_gport, _id)                                          \
        ((_gport) = (_SHR_GPORT_TYPE_EXTENDER_PORT << _SHR_GPORT_TYPE_SHIFT)            |   \
         (_SHR_EXTENDER_GPORT_TYPE_ENCAP << _SHR_EXTENDER_GPORT_TYPE_SHIFT)             |   \
         (((_id) & _SHR_GPORT_EXTENDER_PORT_MASK) << _SHR_GPORT_EXTENDER_PORT_SHIFT))

    /*
     * Note: Same as _SHR_EXTENDER_GPORT_PORT_SET, exists only for backward competability 
     */
#define _SHR_GPORT_EXTENDER_PORT_ID_SET(_gport, _id)                                        \
        ((_gport) = (_SHR_GPORT_TYPE_EXTENDER_PORT << _SHR_GPORT_TYPE_SHIFT)            |   \
        (((_id) & _SHR_GPORT_EXTENDER_PORT_MASK) << _SHR_GPORT_EXTENDER_PORT_SHIFT))

#define _SHR_GPORT_EXTENDER_PORT_ID_GET(_gport)                                             \
        (((_gport) >> _SHR_GPORT_EXTENDER_PORT_SHIFT) & _SHR_GPORT_EXTENDER_PORT_MASK)

#define _SHR_GPORT_IS_MAC_PORT(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_MAC_PORT)

#define _SHR_GPORT_MAC_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_MAC_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
        (((_id) & _SHR_GPORT_MAC_PORT_MASK) << _SHR_GPORT_MAC_PORT_SHIFT))

#define _SHR_GPORT_MAC_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MAC_PORT_SHIFT) & _SHR_GPORT_MAC_PORT_MASK)

#define _SHR_GPORT_IS_TUNNEL(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_TUNNEL)

#define _SHR_GPORT_TUNNEL_ID_SET(_gport, _tunnel_id)                       \
        ((_gport) = (_SHR_GPORT_TYPE_TUNNEL   << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_tunnel_id) & _SHR_GPORT_TUNNEL_MASK) << _SHR_GPORT_TUNNEL_SHIFT))

#define _SHR_GPORT_TUNNEL_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_TUNNEL_SHIFT) & _SHR_GPORT_TUNNEL_MASK)

#define _SHR_GPORT_IS_CHILD(_gport)   \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_CHILD)

#define _SHR_GPORT_CHILD_SET(_child_gport,_child_modid, _child_port) \
        ((_child_gport) = (_SHR_GPORT_TYPE_CHILD << _SHR_GPORT_TYPE_SHIFT)  |	\
        (((_child_modid) & _SHR_GPORT_CHILD_MODID_MASK) << _SHR_GPORT_CHILD_MODID_SHIFT) | \
        (((_child_port) & _SHR_GPORT_CHILD_PORT_MASK) << _SHR_GPORT_CHILD_PORT_SHIFT))

#define _SHR_GPORT_CHILD_MODID_GET(_gport)  \
        (((_gport) >> _SHR_GPORT_CHILD_MODID_SHIFT) & _SHR_GPORT_CHILD_MODID_MASK)

#define _SHR_GPORT_CHILD_PORT_GET(_gport)  \
        (((_gport) >> _SHR_GPORT_CHILD_PORT_SHIFT) & _SHR_GPORT_CHILD_PORT_MASK)

#define _SHR_GPORT_IS_EGRESS_GROUP(_gport)  \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_EGRESS_GROUP)

#define _SHR_GPORT_EGRESS_GROUP_SET(_egress_group_gport, _child_modid, _egress_child) \
        ((_egress_group_gport) = (_SHR_GPORT_TYPE_EGRESS_GROUP << _SHR_GPORT_TYPE_SHIFT)  |	\
        (((_child_modid) & _SHR_GPORT_EGRESS_CHILD_MODID_MASK) << _SHR_GPORT_EGRESS_CHILD_MODID_SHIFT) | \
        (((_egress_child) & _SHR_GPORT_EGRESS_CHILD_PORT_MASK) << (_SHR_GPORT_EGRESS_GROUP_SHIFT)))

#define _SHR_GPORT_EGRESS_GROUP_MODID_GET(_egress_group_gport)  \
        (((_egress_group_gport) >> _SHR_GPORT_EGRESS_GROUP_MODID_SHIFT) & _SHR_GPORT_EGRESS_GROUP_MODID_MASK)

#define _SHR_GPORT_EGRESS_GROUP_GET(_egress_group_gport)  \
        (((_egress_group_gport) >> _SHR_GPORT_EGRESS_GROUP_SHIFT) & _SHR_GPORT_EGRESS_GROUP_MASK)

#define _SHR_GPORT_IS_EGRESS_CHILD(_gport)  \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_EGRESS_CHILD)

#define _SHR_GPORT_EGRESS_CHILD_SET(_egress_child_gport,_child_modid, _child_port) \
        ((_egress_child_gport) = (_SHR_GPORT_TYPE_EGRESS_CHILD << _SHR_GPORT_TYPE_SHIFT)  |	\
        (((_child_modid) & _SHR_GPORT_EGRESS_CHILD_MODID_MASK) << _SHR_GPORT_EGRESS_CHILD_MODID_SHIFT) | \
        (((_child_port) & _SHR_GPORT_EGRESS_CHILD_PORT_MASK) << _SHR_GPORT_EGRESS_CHILD_PORT_SHIFT))

#define _SHR_GPORT_EGRESS_CHILD_MODID_GET(_egress_child_gport)  \
        (((_egress_child_gport) >> _SHR_GPORT_EGRESS_CHILD_MODID_SHIFT) & _SHR_GPORT_EGRESS_CHILD_MODID_MASK)

#define _SHR_GPORT_EGRESS_CHILD_PORT_GET(_egress_child_gport)  \
        (((_egress_child_gport) >> _SHR_GPORT_EGRESS_CHILD_PORT_SHIFT) & _SHR_GPORT_EGRESS_CHILD_PORT_MASK)

#define _SHR_GPORT_IS_EGRESS_MODPORT(_gport)  \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_EGRESS_MODPORT)

#define _SHR_GPORT_EGRESS_MODPORT_SET(_gport, _module, _port)  \
        ((_gport) = (_SHR_GPORT_TYPE_EGRESS_MODPORT   << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_module) & _SHR_GPORT_EGRESS_MODPORT_MODID_MASK)  << _SHR_GPORT_EGRESS_MODPORT_MODID_SHIFT)  | \
         (((_port) & _SHR_GPORT_EGRESS_MODPORT_PORT_MASK)     << _SHR_GPORT_EGRESS_MODPORT_PORT_SHIFT))

#define _SHR_GPORT_EGRESS_MODPORT_MODID_GET(_gport)  \
        (((_gport) >> _SHR_GPORT_EGRESS_MODPORT_MODID_SHIFT) & _SHR_GPORT_EGRESS_MODPORT_MODID_MASK)

#define _SHR_GPORT_EGRESS_MODPORT_PORT_GET(_gport)  \
        (((_gport) >> _SHR_GPORT_EGRESS_MODPORT_PORT_SHIFT) & _SHR_GPORT_EGRESS_MODPORT_PORT_MASK)

#define _SHR_GPORT_IS_CONGESTION(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_CONGESTION)

#define _SHR_GPORT_CONGESTION_SET(_gport, _port)                                       \
        ((_gport) = (_SHR_GPORT_TYPE_CONGESTION << _SHR_GPORT_TYPE_SHIFT)   |          \
         (((_port) & _SHR_GPORT_CONGESTION_MASK) << _SHR_GPORT_CONGESTION_SHIFT))

#define _SHR_GPORT_CONGESTION_GET(_gport)                                              \
        (((_gport) >> _SHR_GPORT_CONGESTION_SHIFT) & _SHR_GPORT_CONGESTION_MASK)

#define _SHR_GPORT_CONGESTION_OOB_SET(_gport, _port)                                       \
        ((_gport) = (_SHR_GPORT_TYPE_CONGESTION << _SHR_GPORT_TYPE_SHIFT)    |    \
         (_SHR_GPORT_CONGESTION_TYPE_OOB << _SHR_GPORT_CONGESTION_TYPE_SHIFT) |    \
         (((_port) & _SHR_GPORT_CONGESTION_MASK) << _SHR_GPORT_CONGESTION_SHIFT))

#define _SHR_GPORT_CONGESTION_COE_SET(_gport, _port)                                       \
        ((_gport) = (_SHR_GPORT_TYPE_CONGESTION << _SHR_GPORT_TYPE_SHIFT)    |    \
         (_SHR_GPORT_CONGESTION_TYPE_COE << _SHR_GPORT_CONGESTION_TYPE_SHIFT) |    \
         (((_port) & _SHR_GPORT_CONGESTION_MASK) << _SHR_GPORT_CONGESTION_SHIFT))

#define _SHR_GPORT_IS_CONGESTION_COE(_gport)    \
        (_SHR_GPORT_IS_CONGESTION(_gport) &&    \
        ((((_gport) >> _SHR_GPORT_CONGESTION_TYPE_SHIFT) & _SHR_GPORT_CONGESTION_TYPE_MASK) == \
                                                                _SHR_GPORT_CONGESTION_TYPE_COE))

#define _SHR_GPORT_IS_CONGESTION_OOB(_gport)    \
        (_SHR_GPORT_IS_CONGESTION(_gport) &&    \
        ((((_gport) >> _SHR_GPORT_CONGESTION_TYPE_SHIFT) & _SHR_GPORT_CONGESTION_TYPE_MASK) == \
                                                                _SHR_GPORT_CONGESTION_TYPE_OOB))

#define _SHR_GPORT_IS_SYSTEM_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_SYSTEM_PORT)
#define _SHR_GPORT_SYSTEM_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_SYSTEM_PORT_SHIFT) & _SHR_GPORT_SYSTEM_PORT_MASK)
#define _SHR_GPORT_SYSTEM_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_SYSTEM_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_SYSTEM_PORT_MASK) << _SHR_GPORT_SYSTEM_PORT_SHIFT))

#define _SHR_COSQ_GPORT_ISQ_ROOT_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_ISQ_ROOT) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_ISQ_ROOT_SET(_gport) \
        _SHR_COSQ_GPORT_ISQ_ROOT_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_IS_ISQ_ROOT(_gport)                                            \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_ISQ_ROOT))
#define _SHR_COSQ_GPORT_IS_FMQ_ROOT(_gport)                                            \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_FMQ_ROOT))

#define _SHR_COSQ_GPORT_FMQ_ROOT_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_FMQ_ROOT) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_FMQ_ROOT_SET(_gport) \
        _SHR_COSQ_GPORT_FMQ_ROOT_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_IS_FMQ_GUARANTEED(_gport)                                      \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_FMQ_GUARANTEED))

#define _SHR_COSQ_GPORT_FMQ_GUARANTEED_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_FMQ_GUARANTEED) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_FMQ_GUARANTEED_SET(_gport) \
        _SHR_COSQ_GPORT_FMQ_GUARANTEED_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(_gport)                                  \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_FMQ_BESTEFFORT_AGR))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT_AGR_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_FMQ_BESTEFFORT_AGR) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT_AGR_SET(_gport) \
        _SHR_COSQ_GPORT_FMQ_BESTEFFORT_AGR_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT0(_gport)                                     \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_FMQ_BESTEFFORT0))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT0_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_FMQ_BESTEFFORT0) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT0_SET(_gport) \
        _SHR_COSQ_GPORT_FMQ_BESTEFFORT0_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT1(_gport)                                     \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_FMQ_BESTEFFORT1))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT1_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_FMQ_BESTEFFORT1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT1_SET(_gport) \
        _SHR_COSQ_GPORT_FMQ_BESTEFFORT1_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT2(_gport)                                     \
        (_SHR_GPORT_IS_SCHEDULER(_gport) &&                                            \
        ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==     \
                                                _SHR_GPORT_FMQ_BESTEFFORT2))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT2_CORE_SET(_gport, _core_id) \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_SHR_GPORT_FMQ_BESTEFFORT2) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT))

#define _SHR_COSQ_GPORT_FMQ_BESTEFFORT2_SET(_gport) \
        _SHR_COSQ_GPORT_FMQ_BESTEFFORT2_CORE_SET(_gport, _SHR_CORE_ALL)

#define _SHR_GPORT_IS_FABRIC_MESH(_gport)                                                         \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
            ( ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL & _SHR_GPORT_SCHEDULER_MASK))  ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV4 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV5 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV6 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV7 & _SHR_GPORT_SCHEDULER_MASK))   ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_DEV1 & _SHR_GPORT_SCHEDULER_MASK))           ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_DEV2 & _SHR_GPORT_SCHEDULER_MASK))           ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_DEV3 & _SHR_GPORT_SCHEDULER_MASK))           ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_MC & _SHR_GPORT_SCHEDULER_MASK))             ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))       ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))        ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))       ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))        ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))       ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))        ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))       ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))        ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK))              ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))         ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))          ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK))              ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC & _SHR_GPORT_SCHEDULER_MASK))                  ||   \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_SCOPE & _SHR_GPORT_SCHEDULER_MASK)) ))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL(_gport)                                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV1(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV2(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV3(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV4(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV4 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV5(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV5 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV6(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV6 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV7(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV7 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL_SET(_gport)                             \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV_SET(_gport, _dev_id)                      \
    switch (_dev_id) {                                                            \
        case 1:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FABRIC_MESH_DEV1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        case 2:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
             (((_SHR_GPORT_FABRIC_MESH_DEV2) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        case 3:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
             (((_SHR_GPORT_FABRIC_MESH_DEV3) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        case 4:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FABRIC_MESH_DEV4) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        case 5:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FABRIC_MESH_DEV5) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        case 6:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FABRIC_MESH_DEV6) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        case 7:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FABRIC_MESH_DEV7) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT));                                                                            \
            break;                                                                \
        default:                                                                  \
            ((_gport) = _SHR_GPORT_INVALID);                                      \
            break;                                                                \
    }

#define _SHR_GPORT_IS_FABRIC_MESH_COMMON_LOCAL0(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_COMMON_LOCAL1(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV1(_gport)                                                     \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_DEV1 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV2(_gport)                                                     \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_DEV2 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV3(_gport)                                                     \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_DEV3 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_COMMON_MC(_gport)                                                     \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_COMMON_MC & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL0(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL1(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_COMMON_FABRIC(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_FABRIC & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_COMMON_UNICAST_FABRIC(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_COMMON_MULTICAST_FABRIC(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV1_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV1_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV1_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV1_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV1_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV2_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV2_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV2_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV2_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV2_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV3_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV3_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV3_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_DEV3_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_DEV3_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_MC_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_MC_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_MC_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_MC_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL0(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_LOCAL1(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_MC(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_MC & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_MESH_SCOPE(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_MESH_SCOPE & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_OCB(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_MIX(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_OCB_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_MIX_HIGH(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_OCB_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_MIX_LOW(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_SCOPE(_gport)                                   \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==          \
                                (_SHR_GPORT_FABRIC_CLOS_SCOPE & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_COSQ_GPORT_FMQ_CLASS_CORE_SET(_gport, _core_id, _class)              \
    switch (_class) {                                                             \
        case 1:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FMQ_CLASS1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) |\
             (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT));                                                                                 \
            break;                                                                \
        case 2:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
             (((_SHR_GPORT_FMQ_CLASS2) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) |\
             (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT));                                                                                   \
            break;                                                                \
        case 3:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  | \
             (((_SHR_GPORT_FMQ_CLASS3) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) |\
             (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT));                                                                                   \
            break;                                                                \
        case 4:                                                                   \
            ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |   \
             (((_SHR_GPORT_FMQ_CLASS4) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT) |\
             (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_GPORT_SCHEDULER_CORE_MASK) << _SHR_GPORT_SCHEDULER_CORE_SHIFT));                                                                                \
            break;                                                                \
        default:                                                                  \
            ((_gport) = _SHR_GPORT_INVALID);                                      \
            break;                                                                \
    }

#define _SHR_COSQ_GPORT_FMQ_CLASS_SET(_gport, _class)                             \
        _SHR_COSQ_GPORT_FMQ_CLASS_CORE_SET(_gport, _SHR_CORE_ALL, _class)

#define _SHR_COSQ_GPORT_IS_FMQ_CLASS(_gport)                                              \
        ( _SHR_GPORT_IS_SCHEDULER(_gport) &&                                              \
          ( ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==    \
                                                _SHR_GPORT_FMQ_CLASS1) ||                 \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==    \
                                                _SHR_GPORT_FMQ_CLASS2) ||                 \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==    \
                                                _SHR_GPORT_FMQ_CLASS3) ||                 \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==    \
                                                _SHR_GPORT_FMQ_CLASS4) ) )

#define _SHR_GPORT_IS_FABRIC_CLOS(_gport)                                                                        \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
            ( ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL & _SHR_GPORT_SCHEDULER_MASK))        ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW & _SHR_GPORT_SCHEDULER_MASK))    ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH & _SHR_GPORT_SCHEDULER_MASK))   ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FABRIC & _SHR_GPORT_SCHEDULER_MASK))               ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FABRIC_HIGH & _SHR_GPORT_SCHEDULER_MASK))          ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FABRIC_LOW & _SHR_GPORT_SCHEDULER_MASK))           ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH & _SHR_GPORT_SCHEDULER_MASK))  ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW & _SHR_GPORT_SCHEDULER_MASK))   ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK))        ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK))        ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_FABRIC & _SHR_GPORT_SCHEDULER_MASK))        ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC & _SHR_GPORT_SCHEDULER_MASK))       ||     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC & _SHR_GPORT_SCHEDULER_MASK))     ||     \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB & _SHR_GPORT_SCHEDULER_MASK))        ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX & _SHR_GPORT_SCHEDULER_MASK))        ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0 & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1 & _SHR_GPORT_SCHEDULER_MASK))       ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_HIGH & _SHR_GPORT_SCHEDULER_MASK))  ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL0_LOW & _SHR_GPORT_SCHEDULER_MASK))   ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_HIGH & _SHR_GPORT_SCHEDULER_MASK))  ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_LOCAL1_LOW & _SHR_GPORT_SCHEDULER_MASK))   ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_OCB_HIGH & _SHR_GPORT_SCHEDULER_MASK))     ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_MIX_HIGH & _SHR_GPORT_SCHEDULER_MASK))     ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_OCB_LOW & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_MIX_LOW & _SHR_GPORT_SCHEDULER_MASK))      ||    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_SCOPE & _SHR_GPORT_SCHEDULER_MASK)) ))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(_gport)                                                          \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                           \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(_gport)                                                      \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                           \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(_gport)                                                     \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                           \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FABRIC(_gport)                                                                 \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
            ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                           \
                                (_SHR_GPORT_FABRIC_CLOS_FABRIC & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(_gport)                                                            \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FABRIC_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(_gport)                                                             \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FABRIC_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(_gport)                                                    \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(_gport)                                                     \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(_gport)                                                         \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(_gport)                                                         \
        ( _SHR_GPORT_IS_SCHEDULER(_gport)  &&                                                                    \
              ((((_gport) >> _SHR_GPORT_SCHEDULER_SHIFT) & _SHR_GPORT_SCHEDULER_MASK) ==                         \
                                (_SHR_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT & _SHR_GPORT_SCHEDULER_MASK)))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_SET(_gport)                     \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW_SET(_gport)                 \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH_SET(_gport)                \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FABRIC_SET(_gport)                            \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FABRIC) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FABRIC_HIGH_SET(_gport)                       \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FABRIC_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FABRIC_LOW_SET(_gport)                        \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FABRIC_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH_SET(_gport)               \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW_SET(_gport)                \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_LOCAL0_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_COMMON_LOCAL0) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_LOCAL1_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_COMMON_LOCAL1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_DEV1_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_COMMON_DEV1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_DEV2_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_COMMON_DEV2) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_DEV3_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_COMMON_DEV3) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_MC_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_COMMON_MC) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_LOCAL0_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL0) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_LOCAL1_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_COMMON_LOCAL1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_FABRIC_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_COMMON_FABRIC) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV1_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV1_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV1_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV1_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV2_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV2_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV2_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV2_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV3_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV3_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV3_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_DEV3_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_MC_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_MC_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_MC_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_MC_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_MC_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_MC_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_MC_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_MC_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL0) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_LOCAL1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_MC_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_MC) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_MESH_SCOPE_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_MESH_SCOPE) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL0_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_LOCAL1_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_OCB_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_OCB_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_OCB_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_OCB_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_MIX_HIGH_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_MIX_HIGH) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_MIX_LOW_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_MIX_LOW) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_CLOS_SCOPE_SET(_gport)                    \
        ((_gport) = (_SHR_GPORT_TYPE_SCHEDULER << _SHR_GPORT_TYPE_SHIFT)  |       \
         (((_SHR_GPORT_FABRIC_CLOS_SCOPE) & _SHR_GPORT_SCHEDULER_MASK) << _SHR_GPORT_SCHEDULER_SHIFT))

#define _SHR_GPORT_IS_COSQ_EXT(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)== _SHR_GPORT_TYPE_COSQ_EXT)

#define _SHR_GPORT_IS_COSQ_BASE(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)== _SHR_GPORT_TYPE_COSQ)

#define _SHR_GPORT_IS_COSQ(_gport) \
        (_SHR_GPORT_IS_COSQ_EXT(_gport) || _SHR_GPORT_IS_COSQ_BASE(_gport))

#define _SHR_COSQ_GPORT_IS_VOQ_CONNECTOR(_gport)                                       \
        (_SHR_GPORT_IS_COSQ_EXT(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_EXT_GPORT_TYPE_VOQ_CONNECTOR))

#define _SHR_COSQ_GPORT_IS_SRC_QUEUE(_gport)                                       \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_SRC_QUEUE))

#define _SHR_COSQ_GPORT_IS_DST_QUEUE(_gport)                                       \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_DST_QUEUE))

#define _SHR_COSQ_GPORT_COMPOSITE_SF2_GET(_gport)  \
        (((_gport) >> _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_FLOW_SHIFT) & _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_FLOW_MASK)

#define _SHR_COSQ_GPORT_COMPOSITE_SF2_CORE_GET(_gport)  \
        (_SHR_CORE_FIELD2ID((((_gport) >> _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_SHIFT) & _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_MASK)))

#define _SHR_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(_gport)                                  \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_UCAST_EGRESS_QUEUE))
#define _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(_gport, _port)                          \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_UCAST_EGRESS_QUEUE << _SHR_COSQ_GPORT_TYPE_SHIFT)   |   \
         (((_port) & _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_PORT_MASK)  << _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_PORT_SHIFT))

#define _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_GET(_gport)  \
        (((_gport) >> _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_PORT_SHIFT) & _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_PORT_MASK)

#define _SHR_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(_gport)                                  \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_MCAST_EGRESS_QUEUE))
#define _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(_gport, _port)                          \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_MCAST_EGRESS_QUEUE << _SHR_COSQ_GPORT_TYPE_SHIFT)   |   \
         (((_port) & _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_PORT_MASK)  << _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_PORT_SHIFT))

#define _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(_gport)  \
        (((_gport) >> _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_PORT_SHIFT) & _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_PORT_MASK)

#define _SHR_COSQ_GPORT_IS_E2E_PORT(_gport)                                            \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_E2E_PORT))

#define _SHR_COSQ_GPORT_E2E_PORT_SET(_gport, _port)                                       \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                   \
         (_SHR_COSQ_GPORT_TYPE_E2E_PORT << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                \
         (((_port) & _SHR_COSQ_GPORT_E2E_PORT_MASK)  << _SHR_COSQ_GPORT_E2E_PORT_SHIFT))

#define _SHR_COSQ_GPORT_E2E_PORT_GET(_gport)  \
        (((_gport) >> _SHR_COSQ_GPORT_E2E_PORT_SHIFT) & _SHR_COSQ_GPORT_E2E_PORT_MASK)

#define _SHR_COSQ_GPORT_IS_ISQ(_gport)                                                 \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_ISQ))
#define _SHR_COSQ_GPORT_ISQ_SET(_gport, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_COSQ_GPORT_TYPE_ISQ << _SHR_COSQ_GPORT_TYPE_SHIFT) | \
         (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
         (((_qid) & _SHR_COSQ_GPORT_ISQ_QID_MASK) << _SHR_COSQ_GPORT_ISQ_QID_SHIFT))
#define _SHR_COSQ_GPORT_ISQ_CORE_QUEUE_SET(_gport, _core, _qid) \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_COSQ_GPORT_TYPE_ISQ << _SHR_COSQ_GPORT_TYPE_SHIFT) | \
         (_SHR_CORE_ID2FIELD(_core) << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
         (((_qid) & _SHR_COSQ_GPORT_ISQ_QID_MASK) << _SHR_COSQ_GPORT_ISQ_QID_SHIFT))
#define _SHR_COSQ_GPORT_ISQ_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID(((_gport) >> _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) & _SHR_COSQ_GPORT_COMMON_CORE_MASK)

#define _SHR_COSQ_GPORT_ISQ_QID_GET(_gport)                                            \
        (((_gport) >> _SHR_COSQ_GPORT_ISQ_QID_SHIFT) & _SHR_COSQ_GPORT_ISQ_QID_MASK)

#define _SHR_COSQ_GPORT_IS_MULTIPATH(_gport)  \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&    \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_MULTIPATH))

#define _SHR_COSQ_GPORT_MULTIPATH_GET(_gport)  \
        (((_gport) >> _SHR_COSQ_GPORT_MULTIPATH_SHIFT) & _SHR_COSQ_GPORT_MULTIPATH_MASK)

#define _SHR_COSQ_GPORT_MULTIPATH_SET(_gport, _id)            \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_MULTIPATH << _SHR_COSQ_GPORT_TYPE_SHIFT)   |            \
         (((_id) & _SHR_COSQ_GPORT_MULTIPATH_MASK) << _SHR_COSQ_GPORT_MULTIPATH_SHIFT))

#define _SHR_COSQ_GPORT_ATTACH_ID_SET(_attach_id,_fcd,_sysport)		               \
  ((_attach_id) =                                                                      \
   ((((_fcd) & _SHR_COSQ_GPORT_ATTACH_ID_FCD_MASK) << _SHR_COSQ_GPORT_ATTACH_ID_FCD_SHIFT) | \
    (((_sysport) & _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_MASK) << _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_SHIFT)))

#define _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_SET(_attach_id,_sysport)                      \
        _SHR_COSQ_GPORT_ATTACH_ID_SET(_attach_id,_SHR_COSQ_GPORT_ATTACH_ID_FCD_MASK,_sysport)

#define _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_GET(_attach_id)                               \
        (((_attach_id) >> _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_SHIFT) &                    \
        _SHR_COSQ_GPORT_ATTACH_ID_SYSPORT_MASK)

#define _SHR_COSQ_GPORT_ATTACH_ID_FCD_GET(_attach_id)                                   \
        (((_attach_id) >> _SHR_COSQ_GPORT_ATTACH_ID_FCD_SHIFT)     &                    \
        _SHR_COSQ_GPORT_ATTACH_ID_FCD_MASK)

#define _SHR_COSQ_GPORT_IS_COMPOSITE_SF2(_gport) \
        (_SHR_GPORT_IS_COSQ_EXT(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_SCHED || \
        ((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) ==     \
         _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_CONN )))

#define _SHR_COSQ_GPORT_VSQ_TYPE(_type)                                                                  \
        ((_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                                             \
         (_SHR_COSQ_GPORT_TYPE_VSQ << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                                    \
         (((_type) & _SHR_COSQ_GPORT_VSQ_TYPE_MASK)  << _SHR_COSQ_GPORT_VSQ_TYPE_SHIFT))

#define _SHR_COSQ_GPORT_VSQ_FLAGS2TYPE(_flags) ((_flags == -1) ? -1 : _flags - 1)
#define _SHR_COSQ_GPORT_VSQ_TYPE2FLAGS(_type) ((_type == -1) ? -1 : _type + 1)

#define _SHR_COSQ_GPORT_VSQ_SET(_gport, _flags, _type_info)                                                  \
             ((_gport) = _SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_GPORT_VSQ_FLAGS2TYPE(_flags)) |                  \
              (((_type_info) & _SHR_COSQ_GPORT_VSQ_TYPE_INFO_MASK)  << _SHR_COSQ_GPORT_VSQ_TYPE_INFO_SHIFT));

#define _SHR_COSQ_GPORT_IS_VSQ(_gport)                                                 \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_VSQ))
#define _SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport)    \
        (!_SHR_COSQ_GPORT_IS_VSQ(_gport) ? -1 : \
        (((_gport) >> _SHR_COSQ_GPORT_VSQ_TYPE_SHIFT) & _SHR_COSQ_GPORT_VSQ_TYPE_MASK))

#define _SHR_COSQ_GPORT_VSQ_FLAGS_GET(_gport)    \
        (_SHR_COSQ_GPORT_VSQ_TYPE2FLAGS(_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport)))

#define _SHR_COSQ_GPORT_VSQ_TYPE_INFO_GET(_gport)                                      \
        (((_gport) >> _SHR_COSQ_GPORT_VSQ_TYPE_INFO_SHIFT) & _SHR_COSQ_GPORT_VSQ_TYPE_INFO_MASK)

#define _SHR_COSQ_GPORT_VSQ_GL_SET(_gport, _core_id)  \
             ((_gport) = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_GL)) |                                         \
                        ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_GL(_gport)  \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_GL)

#define _SHR_COSQ_GPORT_VSQ_CT_SET(_gport, _core_id, _is_ocb_only, _category)                                     \
             (_gport = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_CT)) |                                             \
                       ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT) |             \
                       ((_category & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK) << _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT)|     \
                       ((_is_ocb_only & _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK) << _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_CT(_gport) \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_CT)

#define _SHR_COSQ_GPORT_VSQ_CTTC_SET(_gport, _core_id, _is_ocb_only, _category, _traffic_class)                   \
             (_gport = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_CTTC)) |                                           \
                       ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT) |             \
                       ((_category & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK) << _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) |    \
                       ((_traffic_class & _SHR_VSQ_TYPE_INFO_TC_MASK)  << _SHR_VSQ_TYPE_INFO_TC_SHIFT)       |    \
                       ((_is_ocb_only & _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK) << _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_CTTC(_gport) \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_CTTC)

#define _SHR_COSQ_GPORT_VSQ_CTCC_SET(_gport, _core_id, _is_ocb_only, _category, _connection_class) \
             (_gport = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_CTCC)) | \
                       ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT) | \
                       ((_category & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK) << _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) | \
                       ((_connection_class & _SHR_VSQ_TYPE_INFO_CONNECTION_MASK)  << _SHR_VSQ_TYPE_INFO_CONNECTION_SHIFT) | \
                       ((_is_ocb_only & _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK) << _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_CTCC(_gport) \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_CTCC)

#define _SHR_COSQ_GPORT_VSQ_PP_SET(_gport, _core_id, _is_ocb_only, _statistics_tag) \
             (_gport = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_PP)) |                                           \
                       ((_is_ocb_only & _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK) << _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT) | \
                       ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT) |\
                       ((_statistics_tag & _SHR_VSQ_TYPE_INFO_STATISTICS_TAG_MASK)  << _SHR_VSQ_TYPE_INFO_STATISTICS_TAG_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_PP(_gport) \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_PP)

#define _SHR_COSQ_GPORT_VSQ_SRC_PORT_SET(_gport, _core_id, _is_ocb_only, _src_port_vsq_id) \
             (_gport = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_SRC_PORT)) |  \
                       ((_src_port_vsq_id & _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK) << _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT) | \
                       ((_is_ocb_only & _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK) << _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT) | \
                       ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_SRC_PORT(_gport) \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_SRC_PORT)

#define _SHR_COSQ_GPORT_VSQ_PG_SET(_gport, _core_id, _pg_vsq_id) \
             (_gport = (_SHR_COSQ_GPORT_VSQ_TYPE(_SHR_COSQ_VSQ_PG)) | \
                       ((_SHR_CORE_ID2FIELD(_core_id) & _SHR_VSQ_TYPE_INFO_CORE_MASK) << _SHR_VSQ_TYPE_INFO_CORE_SHIFT) | \
                       ((_pg_vsq_id & _SHR_VSQ_TYPE_INFO_PG_MASK) << _SHR_VSQ_TYPE_INFO_PG_SHIFT))

#define _SHR_COSQ_GPORT_IS_VSQ_PG(_gport) \
             (_SHR_COSQ_GPORT_VSQ_TYPE_GET(_gport) == _SHR_COSQ_VSQ_PG)

#define _SHR_COSQ_GPORT_VSQ_CT_GET(_gport)  \
             (!(_SHR_COSQ_GPORT_IS_VSQ_CT(_gport) ||_SHR_COSQ_GPORT_IS_VSQ_CTTC(_gport) || _SHR_COSQ_GPORT_IS_VSQ_CTCC(_gport)) ? - 1 : \
             (((_gport) >> _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK))

#define _SHR_COSQ_GPORT_VSQ_TC_GET(_gport) \
             (!(_SHR_COSQ_GPORT_IS_VSQ_CTTC(_gport) || _SHR_COSQ_GPORT_IS_VSQ_SRC_PORT(_gport)) ? - 1 : \
             (((_gport) >> _SHR_VSQ_TYPE_INFO_TC_SHIFT) & _SHR_VSQ_TYPE_INFO_TC_MASK))

#define _SHR_COSQ_GPORT_VSQ_CC_GET(_gport) \
             (!_SHR_COSQ_GPORT_IS_VSQ_CTCC(_gport) ? - 1 : \
             (((_gport) >> _SHR_VSQ_TYPE_INFO_CONNECTION_SHIFT) & _SHR_VSQ_TYPE_INFO_CONNECTION_MASK))

#define _SHR_COSQ_GPORT_VSQ_PP_GET(_gport) \
             (!_SHR_COSQ_GPORT_IS_VSQ_PP(_gport) ? - 1 : \
             (((_gport) >> _SHR_VSQ_TYPE_INFO_STATISTICS_TAG_SHIFT) & _SHR_VSQ_TYPE_INFO_STATISTICS_TAG_MASK))

#define _SHR_COSQ_GPORT_VSQ_CORE_ID_GET(_gport) \
             (!(_SHR_COSQ_GPORT_IS_VSQ(_gport)) ? - 1 : (_SHR_CORE_FIELD2ID((((_gport) >> _SHR_VSQ_TYPE_INFO_CORE_SHIFT)) & _SHR_VSQ_TYPE_INFO_CORE_MASK)))

#define _SHR_COSQ_GPORT_VSQ_IS_OCB_ONLY(_gport)            \
             (!(_SHR_COSQ_GPORT_IS_VSQ_CT(_gport) ||       \
                _SHR_COSQ_GPORT_IS_VSQ_CTTC(_gport) ||     \
                _SHR_COSQ_GPORT_IS_VSQ_CTCC(_gport) ||     \
                _SHR_COSQ_GPORT_IS_VSQ_PP(_gport) ||       \
                _SHR_COSQ_GPORT_IS_VSQ_SRC_PORT(_gport)) ? \
                - 1 : (((_gport) >> _SHR_VSQ_TYPE_INFO_OCB_ONLY_SHIFT) & _SHR_VSQ_TYPE_INFO_OCB_ONLY_MASK))

#define _SHR_COSQ_GPORT_VSQ_SRC_PORT_GET(_gport) \
             (!_SHR_COSQ_GPORT_IS_VSQ_SRC_PORT(_gport) ? - 1 : \
             (((_gport) >> _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT) & _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK))

#define _SHR_COSQ_GPORT_VSQ_PG_GET(_gport) \
             (!_SHR_COSQ_GPORT_IS_VSQ_PG(_gport) ? - 1 : \
             (((_gport) >> _SHR_VSQ_TYPE_INFO_PG_SHIFT) & _SHR_VSQ_TYPE_INFO_PG_MASK))


#define _SHR_COSQ_GPORT_IS_INBAND_COE(_gport)                                  \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_INBAND_COE))
#define _SHR_COSQ_GPORT_INBAND_COE_SET(_gport, _port)                          \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |          \
         (_SHR_COSQ_GPORT_TYPE_INBAND_COE << _SHR_COSQ_GPORT_TYPE_SHIFT)   |   \
         (((_port) & _SHR_COSQ_GPORT_INBAND_COE_MASK)  << _SHR_COSQ_GPORT_INBAND_COE_SHIFT))

#define _SHR_COSQ_GPORT_INBAND_COE_GET(_gport)  \
        (((_gport) >> _SHR_COSQ_GPORT_INBAND_COE_SHIFT) & _SHR_COSQ_GPORT_INBAND_COE_MASK)

/*Lecacy VSQ Macros - compatible to the new onces*/
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPA_SET(_type_info, _category)                      \
        ((_type_info) = _category)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPA_CATEGORY_GET(_type_info)                        \
        ((_type_info))
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPB_SET(_type_info, _category, _tc)                 \
        ((_type_info) = (((_category & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK) << _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) | ((_tc & _SHR_VSQ_TYPE_INFO_TC_MASK) << _SHR_VSQ_TYPE_INFO_TC_SHIFT)))
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPB_CATEGORY_GET(_type_info)                        \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPB_TC_GET(_type_info)                              \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_TC_SHIFT) & _SHR_VSQ_TYPE_INFO_TC_MASK)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPC_SET(_type_info, _category, _connection)         \
        ((_type_info) = (((_category & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK) << _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) | ((_connection & _SHR_VSQ_TYPE_INFO_CONNECTION_MASK) << _SHR_VSQ_TYPE_INFO_CONNECTION_SHIFT)))
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPC_CATEGORY_GET(_type_info)                        \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_CATEGORY_SHIFT) & _SHR_VSQ_TYPE_INFO_CATEGORY_MASK)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPC_CONNECTION_GET(_type_info)                      \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_CONNECTION_SHIFT) & _SHR_VSQ_TYPE_INFO_CONNECTION_MASK)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPD_SET(_type_info, _statistics_tag)                \
        ((_type_info) = _statistics_tag)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPD_STATISTICS_TAG_GET(_type_info)                  \
        ((_type_info))
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPE_SET(_type_info, _src_port, _tc)         \
        ((_type_info) = (((_src_port & _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK) << _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT) | ((_tc & _SHR_VSQ_TYPE_INFO_TC_MASK) << _SHR_VSQ_TYPE_INFO_TC_SHIFT)))
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPE_SRC_PORT_GET(_type_info)                        \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT) & _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPE_TC_GET(_type_info)                      \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_TC_SHIFT) & _SHR_VSQ_TYPE_INFO_TC_MASK)
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPF_SET(_type_info, _src_port)         \
        ((_type_info) = (((_src_port & _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK) << _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT)))
#define _SHR_COSQ_VSQ_TYPE_INFO_GROUPF_SRC_PORT_GET(_type_info)                        \
        (((_type_info) >> _SHR_VSQ_TYPE_INFO_SRC_PORT_SHIFT) & _SHR_VSQ_TYPE_INFO_SRC_PORT_MASK)

#define _SHR_GPORT_IS_EPON_LINK(_gport)    \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_EPON_LINK)
#define _SHR_GPORT_EPON_LINK_SET(_gport, _num)                                      \
        ((_gport) = (_SHR_GPORT_TYPE_EPON_LINK << _SHR_GPORT_TYPE_SHIFT)  |         \
         (((_num) & _SHR_GPORT_EPON_LINK_NUM_MASK) << _SHR_GPORT_EPON_LINK_NUM_SHIFT))
#define _SHR_GPORT_EPON_LINK_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_EPON_LINK_SHIFT) & _SHR_GPORT_EPON_LINK_MASK)

#define _SHR_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(_gport, _cid, _core_id)             \
    ((_gport) = (_SHR_GPORT_TYPE_COSQ_EXT << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_EXT_GPORT_TYPE_VOQ_CONNECTOR << _SHR_COSQ_EXT_GPORT_TYPE_SHIFT)   |        \
         (((_cid) & _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_ID_MASK)  << _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_ID_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_CORE_MASK) << _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_CORE_SHIFT) )

#define _SHR_COSQ_GPORT_VOQ_CONNECTOR_SET(_gport, _cid)                            \
        _SHR_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(_gport, _cid, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(_gport)                                   \
        (((_gport) >> _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_ID_SHIFT) & _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_ID_MASK)

#define  _SHR_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID((((_gport) >> _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_CORE_SHIFT) & _SHR_COSQ_EXT_GPORT_VOQ_CONNECTOR_CORE_MASK))

#define _SHR_COSQ_GPORT_SRC_QUEUE_SET(_gport, _qid)                                \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |            \
         (_SHR_COSQ_GPORT_TYPE_SRC_QUEUE << _SHR_COSQ_GPORT_TYPE_SHIFT)   |        \
         (((_qid) & _SHR_COSQ_GPORT_SRC_QUEUE_MASK)  << _SHR_COSQ_GPORT_SRC_QUEUE_SHIFT))

#define _SHR_COSQ_GPORT_SRC_QUEUE_GET(_gport)                                   \
        (((_gport) >> _SHR_COSQ_GPORT_SRC_QUEUE_SHIFT) & _SHR_COSQ_GPORT_SRC_QUEUE_MASK)

#define _SHR_COSQ_GPORT_DST_QUEUE_SET(_gport, _qid)                                \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |            \
         (_SHR_COSQ_GPORT_TYPE_DST_QUEUE << _SHR_COSQ_GPORT_TYPE_SHIFT)   |        \
         (((_qid) & _SHR_COSQ_GPORT_DST_QUEUE_MASK)  << _SHR_COSQ_GPORT_DST_QUEUE_SHIFT))

#define _SHR_COSQ_GPORT_DST_QUEUE_GET(_gport)                                   \
        (((_gport) >> _SHR_COSQ_GPORT_DST_QUEUE_SHIFT) & _SHR_COSQ_GPORT_DST_QUEUE_MASK)

#define _SHR_COSQ_GPORT_RESERVED_TYPE_GET(_gport) \
    (((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) == _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_CONN) ? \
     _SHR_COSQ_EXT_GPORT_RESERVED_VOQ_CONNECTOR : _SHR_COSQ_EXT_GPORT_RESERVED_SCHEDULER)

#define _SHR_COSQ_GPORT_COMPOSITE_SF2_SET(_gport_sf2, gport) \
  _SHR_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(_gport_sf2, gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(_gport_sf2, gport, _core_id) \
    if (_SHR_GPORT_IS_SCHEDULER(gport))  { \
        (_gport_sf2) = ((_SHR_GPORT_TYPE_COSQ_EXT << _SHR_GPORT_TYPE_SHIFT) |                                       \
                        (_SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_SCHED << _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) |                    \
                        ((_SHR_GPORT_SCHEDULER_GET(gport)) & _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_FLOW_MASK) | \
                        (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_MASK) << _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_SHIFT) ) ;        \
    } else if (_SHR_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) {      \
             (_gport_sf2) = ((_SHR_GPORT_TYPE_COSQ_EXT << _SHR_GPORT_TYPE_SHIFT) |                                       \
                          (_SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_CONN << _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) |                       \
                          (_SHR_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport) & _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_FLOW_MASK) | \
                          (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_MASK) << _SHR_COSQ_EXT_GPORT_COMPOSITE_SF2_CORE_SHIFT) ) ;        \
    }

#define _SHR_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(_gport)                                  \
        (_SHR_GPORT_IS_COSQ_EXT(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_EXT_GPORT_TYPE_SHARED_SHAPER_ELEM))

#define _SHR_COSQ_GPORT_SHARED_SHAPER_ELEM_SET(_gport, _sched_gport, _core_id)              \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ_EXT << _SHR_GPORT_TYPE_SHIFT)   |           \
         (_SHR_COSQ_EXT_GPORT_TYPE_SHARED_SHAPER_ELEM << _SHR_COSQ_EXT_GPORT_TYPE_SHIFT)   |           \
         ( ((_SHR_GPORT_SCHEDULER_GET(_sched_gport)) & _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_FLOW_MASK)  << _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_FLOW_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_CORE_MASK) << _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_CORE_SHIFT) )

#define _SHR_COSQ_GPORT_SHARED_SHAPER_ELEM_FLOW_GET(_gport)                                         \
        (((_gport) >> _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_FLOW_SHIFT) & _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_FLOW_MASK)

#define  _SHR_COSQ_GPORT_SHARED_SHAPER_ELEM_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID((((_gport) >> _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_CORE_SHIFT) & _SHR_COSQ_EXT_GPORT_SHARED_SHAPER_ELEM_CORE_MASK))


#define _SHR_COSQ_GPORT_IS_SCHED_CIR(_gport)                                           \
        (_SHR_GPORT_IS_COSQ_EXT(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_EXT_GPORT_TYPE_SCHED_CIR))

#define _SHR_COSQ_GPORT_IS_SCHED_PIR(_gport)                                           \
        (_SHR_GPORT_IS_COSQ_EXT(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_EXT_GPORT_TYPE_SHIFT) & _SHR_COSQ_EXT_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_EXT_GPORT_TYPE_SCHED_PIR))


#define _SHR_COSQ_GPORT_SCHED_CIR_CORE_SET(_gport_cir, _gport, _core_id)              \
        ((_gport_cir) = (_SHR_GPORT_TYPE_COSQ_EXT << _SHR_GPORT_TYPE_SHIFT)   |           \
         (_SHR_COSQ_EXT_GPORT_TYPE_SCHED_CIR << _SHR_COSQ_EXT_GPORT_TYPE_SHIFT)   |           \
         ( ((_SHR_GPORT_SCHEDULER_GET(_gport)) & _SHR_COSQ_EXT_GPORT_SCHED_CIR_MASK)  << _SHR_COSQ_EXT_GPORT_SCHED_CIR_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_COSQ_EXT_GPORT_SCHED_CIR_CORE_MASK) << _SHR_COSQ_EXT_GPORT_SCHED_CIR_CORE_SHIFT) )

#define _SHR_COSQ_GPORT_SCHED_CIR_SET(_gport_cir, _gport)              \
        _SHR_COSQ_GPORT_SCHED_CIR_CORE_SET(_gport_cir, _gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_SCHED_CIR_GET(_gport)                                         \
        (((_gport) >> _SHR_COSQ_EXT_GPORT_SCHED_CIR_SHIFT) & _SHR_COSQ_EXT_GPORT_SCHED_CIR_MASK)

#define  _SHR_COSQ_GPORT_SCHED_CIR_CORE_GET(_gport) \
        _SHR_CORE_FIELD2ID((((_gport) >> _SHR_COSQ_EXT_GPORT_SCHED_CIR_CORE_SHIFT) & _SHR_COSQ_EXT_GPORT_SCHED_CIR_CORE_MASK))


#define _SHR_COSQ_GPORT_SCHED_PIR_CORE_SET(_gport_pir, _gport, _core_id)              \
        ((_gport_pir) = (_SHR_GPORT_TYPE_COSQ_EXT << _SHR_GPORT_TYPE_SHIFT)   |           \
         (_SHR_COSQ_EXT_GPORT_TYPE_SCHED_PIR << _SHR_COSQ_EXT_GPORT_TYPE_SHIFT)   |           \
         ( ((_SHR_GPORT_SCHEDULER_GET(_gport)) & _SHR_COSQ_EXT_GPORT_SCHED_PIR_MASK)  << _SHR_COSQ_EXT_GPORT_SCHED_PIR_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core_id)) & _SHR_COSQ_EXT_GPORT_SCHED_PIR_CORE_MASK) << _SHR_COSQ_EXT_GPORT_SCHED_PIR_CORE_SHIFT) )

#define _SHR_COSQ_GPORT_SCHED_PIR_SET(_gport_cir, _gport)              \
        _SHR_COSQ_GPORT_SCHED_PIR_CORE_SET(_gport_cir, _gport, _SHR_CORE_ALL)

#define _SHR_COSQ_GPORT_SCHED_PIR_GET(_gport)                                         \
        (((_gport) >> _SHR_COSQ_EXT_GPORT_SCHED_PIR_SHIFT) & _SHR_COSQ_EXT_GPORT_SCHED_PIR_MASK)

#define  _SHR_COSQ_GPORT_SCHED_PIR_CORE_GET(_gport) \
    _SHR_CORE_FIELD2ID((((_gport) >> _SHR_COSQ_EXT_GPORT_SCHED_PIR_CORE_SHIFT) & _SHR_COSQ_EXT_GPORT_SCHED_PIR_CORE_MASK))

#define _SHR_COSQ_GPORT_IS_PORT_TC(_gport)                                             \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_PORT_TC))

#define _SHR_COSQ_GPORT_PORT_TC_SET(_gport, _port)                                     \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_PORT_TC << _SHR_COSQ_GPORT_TYPE_SHIFT)   |              \
         (((_port) & _SHR_COSQ_GPORT_PORT_TC_MASK)  << _SHR_COSQ_GPORT_PORT_TC_SHIFT))

#define _SHR_COSQ_GPORT_PORT_TC_GET(_gport)                                            \
        (((_gport) >> _SHR_COSQ_GPORT_PORT_TC_SHIFT) & _SHR_COSQ_GPORT_PORT_TC_MASK)

#define _SHR_COSQ_GPORT_IS_PORT_TCG(_gport)                                            \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_PORT_TCG))

#define _SHR_COSQ_GPORT_PORT_TCG_SET(_gport, _port)                                    \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_PORT_TCG << _SHR_COSQ_GPORT_TYPE_SHIFT)   |             \
         (((_port) & _SHR_COSQ_GPORT_PORT_TCG_MASK)  << _SHR_COSQ_GPORT_PORT_TCG_SHIFT))

#define _SHR_COSQ_GPORT_PORT_TCG_GET(_gport)                                           \
        (((_gport) >> _SHR_COSQ_GPORT_PORT_TCG_SHIFT) & _SHR_COSQ_GPORT_PORT_TCG_MASK)

#define _SHR_COSQ_GPORT_IS_E2E_PORT_TC(_gport)                                         \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_E2E_PORT_TC))

#define _SHR_COSQ_GPORT_E2E_PORT_TC_SET(_gport, _port)                                 \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_E2E_PORT_TC << _SHR_COSQ_GPORT_TYPE_SHIFT)   |          \
         (((_port) & _SHR_COSQ_GPORT_E2E_PORT_TC_MASK)  << _SHR_COSQ_GPORT_E2E_PORT_TC_SHIFT))

#define _SHR_COSQ_GPORT_E2E_PORT_TC_GET(_gport)                                        \
        (((_gport) >> _SHR_COSQ_GPORT_E2E_PORT_TC_SHIFT) & _SHR_COSQ_GPORT_E2E_PORT_TC_MASK)

#define _SHR_COSQ_GPORT_IS_E2E_PORT_TCG(_gport)                                        \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_E2E_PORT_TCG))

#define _SHR_COSQ_GPORT_E2E_PORT_TCG_SET(_gport, _port)                                \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_E2E_PORT_TCG << _SHR_COSQ_GPORT_TYPE_SHIFT)   |         \
         (((_port) & _SHR_COSQ_GPORT_E2E_PORT_TCG_MASK)  << _SHR_COSQ_GPORT_E2E_PORT_TCG_SHIFT))

#define _SHR_COSQ_GPORT_E2E_PORT_TCG_GET(_gport)                                       \
        (((_gport) >> _SHR_COSQ_GPORT_E2E_PORT_TCG_SHIFT) & _SHR_COSQ_GPORT_E2E_PORT_TCG_MASK)

#define _SHR_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength)     \
        ((_gport) = (_SHR_GPORT_TYPE_TRAP << _SHR_GPORT_TYPE_TRAP_SHIFT)   |      \
         (((_trap_id) & _SHR_GPORT_TRAP_ID_MASK) << _SHR_GPORT_TRAP_ID_SHIFT)  |    \
         (((_trap_strength) & _SHR_GPORT_TRAP_STRENGTH_MASK) << _SHR_GPORT_TRAP_STRENGTH_SHIFT)  |  \
         (((_snoop_strength) & _SHR_GPORT_TRAP_SNOOP_STRENGTH_MASK) << _SHR_GPORT_TRAP_SNOOP_STRENGTH_SHIFT) )

#define _SHR_GPORT_TRAP_GET_ID(_gport)    \
        (((_gport) >> _SHR_GPORT_TRAP_ID_SHIFT) & _SHR_GPORT_TRAP_ID_MASK)

#define _SHR_GPORT_TRAP_GET_STRENGTH(_gport)    \
        (((_gport) >> _SHR_GPORT_TRAP_STRENGTH_SHIFT & _SHR_GPORT_TRAP_STRENGTH_MASK))

#define _SHR_GPORT_TRAP_GET_SNOOP_STRENGTH(_gport)    \
        (((_gport) >> _SHR_GPORT_TRAP_SNOOP_STRENGTH_SHIFT & _SHR_GPORT_TRAP_SNOOP_STRENGTH_MASK))

#define _SHR_GPORT_IS_TRAP(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_TRAP_SHIFT) == _SHR_GPORT_TYPE_TRAP)

#define _SHR_COSQ_GPORT_IS_SYSTEM_RED(_gport)                                        \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_SYSTEM_RED))

#define _SHR_COSQ_GPORT_SYSTEM_RED_SET(_gport)                                \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                \
         (_SHR_COSQ_GPORT_TYPE_SYSTEM_RED << _SHR_COSQ_GPORT_TYPE_SHIFT))

#define _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE(_gport)                                     \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
         _SHR_COSQ_GPORT_TYPE_INGRESS_RECEIVE))

#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(_gport)                               \
        _SHR_CORE_FIELD2ID((((_gport) >> _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_SHIFT) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_MASK))

#define _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED(_gport)                                       \
        (_SHR_COSQ_GPORT_IS_INGRESS_RECEIVE (_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_MASK) ==     \
         _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_CPU_GUARANTEED))

#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_CPU_GUARANTEED_SET(_gport, _core)                                     \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                                       \
         (_SHR_COSQ_GPORT_TYPE_INGRESS_RECEIVE  << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                            \
         (_SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_CPU_GUARANTEED << _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core)) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_MASK) << _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_SHIFT))

#define _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED(_gport)                                       \
        (_SHR_COSQ_GPORT_IS_INGRESS_RECEIVE(_gport) &&                                                  \
        ((((_gport) >> _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_MASK) ==     \
         _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_RCY_GUARANTEED))

#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_RCY_GUARANTEED_SET(_gport, _core)                                     \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                                       \
         (_SHR_COSQ_GPORT_TYPE_INGRESS_RECEIVE  << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                            \
         (_SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_RCY_GUARANTEED << _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core)) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_MASK) << _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_SHIFT))

#define _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE_NIF(_gport)                                                 \
        (_SHR_COSQ_GPORT_IS_INGRESS_RECEIVE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_MASK) ==     \
         _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_NIF))

#define _SHR_COSQ_GPORT_INGRESS_RECEIVE_NIF_SET(_gport, _core)                                      \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                             \
         (_SHR_COSQ_GPORT_TYPE_INGRESS_RECEIVE  << _SHR_COSQ_GPORT_TYPE_SHIFT)   | \
         (_SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_NIF << _SHR_COSQ_GPORT_INGRESS_RECEIVE_TYPE_SHIFT) | \
         (((_SHR_CORE_ID2FIELD(_core)) & _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_MASK) << _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_ID_SHIFT))

#define _SHR_COSQ_GPORT_IS_FABRIC_PIPE(_gport)                                         \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                    _SHR_COSQ_GPORT_TYPE_FABRIC) &&    \
         ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) & _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_MASK) ==     \
                                                                  _SHR_COSQ_FABRIC_TYPE_PIPE_GLOBAL))

#define _SHR_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(_gport)                                 \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                    _SHR_COSQ_GPORT_TYPE_FABRIC) &&    \
         ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) & _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_MASK) ==     \
                                                                  _SHR_COSQ_FABRIC_TYPE_PIPE_INGRESS))

#define _SHR_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(_gport)                                  \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                    _SHR_COSQ_GPORT_TYPE_FABRIC) &&    \
         ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) & _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_MASK) ==     \
                                                                  _SHR_COSQ_FABRIC_TYPE_PIPE_EGRESS))

#define _SHR_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(_gport)                                  \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                    _SHR_COSQ_GPORT_TYPE_FABRIC) &&    \
         ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) & _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_MASK) ==     \
                                                                  _SHR_COSQ_FABRIC_TYPE_PIPE_MIDDLE))

#define _SHR_COSQ_GPORT_FABRIC_PIPE_SET(_gport, _fabric_port, _fabric_pipe)               \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                   \
         (_SHR_COSQ_GPORT_TYPE_FABRIC << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                  \
         (_SHR_COSQ_FABRIC_TYPE_PIPE_GLOBAL << _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) | \
         (((_fabric_pipe == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PIPES : _fabric_pipe ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT) | \
         (((_fabric_port == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PORTS : _fabric_port ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_PIPE_INGRESS_SET(_gport, _fabric_port, _fabric_pipe)       \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                   \
         (_SHR_COSQ_GPORT_TYPE_FABRIC << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                  \
         (_SHR_COSQ_FABRIC_TYPE_PIPE_INGRESS << _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) | \
         (((_fabric_pipe == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PIPES : _fabric_pipe ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT) | \
         (((_fabric_port == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PORTS : _fabric_port ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_PIPE_EGRESS_SET(_gport, _fabric_port, _fabric_pipe)               \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                   \
         (_SHR_COSQ_GPORT_TYPE_FABRIC << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                  \
         (_SHR_COSQ_FABRIC_TYPE_PIPE_EGRESS << _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) | \
         (((_fabric_pipe == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PIPES : _fabric_pipe ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT) | \
         (((_fabric_port == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PORTS : _fabric_port ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_PIPE_MIDDLE_SET(_gport, _fabric_port, _fabric_pipe)        \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |                   \
         (_SHR_COSQ_GPORT_TYPE_FABRIC << _SHR_COSQ_GPORT_TYPE_SHIFT)   |                  \
         (_SHR_COSQ_FABRIC_TYPE_PIPE_MIDDLE << _SHR_COSQ_GPORT_FABRIC_PIPE_TYPE_SHIFT) | \
         (((_fabric_pipe == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PIPES : _fabric_pipe ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT) | \
         (((_fabric_port == -1 ? _SHR_COSQ_FABRIC_PIPE_ALL_PORTS : _fabric_port ) & _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK) << _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT))

#define _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(_gport) \
        ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT) & \
                        _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK) == _SHR_COSQ_FABRIC_PIPE_ALL_PIPES ? -1 : \
         (((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_SHIFT) & \
                        _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_MASK))

#define _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_GET(_gport) \
        ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT) & \
                        _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK) == _SHR_COSQ_FABRIC_PIPE_ALL_PORTS ? -1 : \
         (((_gport) >> _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_SHIFT) & \
                        _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_MASK))

#define _SHR_GPORT_IS_PHY(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                        _SHR_GPORT_TYPE_PHY)

/* _SHR_PHY_GPORT_TYPE_LANE_PORT */
#define _SHR_PHY_GPORT_IS_LANE(_gport) \
        (_SHR_GPORT_IS_PHY(_gport) && \
        ((((_gport) >> _SHR_PHY_GPORT_TYPE_SHIFT) & \
                        _SHR_PHY_GPORT_TYPE_MASK) == \
                                        _SHR_PHY_GPORT_TYPE_LANE_PORT))

#define _SHR_PHY_GPORT_LANE_PORT_SET(_gport, _phy_lane, _port) \
        ((_gport) = (_SHR_GPORT_TYPE_PHY << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_PHY_GPORT_TYPE_LANE_PORT << _SHR_PHY_GPORT_TYPE_SHIFT) | \
         (((_phy_lane) & _SHR_PHY_GPORT_LANE_PORT_LANE_MASK) << \
                                _SHR_PHY_GPORT_LANE_PORT_LANE_SHIFT) | \
         (((_port) & _SHR_PHY_GPORT_LANE_PORT_PORT_MASK) << \
                                _SHR_PHY_GPORT_LANE_PORT_PORT_SHIFT))

#define _SHR_PHY_GPORT_LANE_PORT_LANE_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_LANE_PORT_LANE_SHIFT) & \
                        _SHR_PHY_GPORT_LANE_PORT_LANE_MASK)
#define _SHR_PHY_GPORT_LANE_PORT_PORT_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_LANE_PORT_PORT_SHIFT) & \
                        _SHR_PHY_GPORT_LANE_PORT_PORT_MASK)

/* _SHR_PHY_GPORT_TYPE_PHYN_PORT */
#define _SHR_PHY_GPORT_IS_PHYN(_gport) \
        (_SHR_GPORT_IS_PHY(_gport) && \
        ((((_gport) >> _SHR_PHY_GPORT_TYPE_SHIFT) & \
                        _SHR_PHY_GPORT_TYPE_MASK) == \
                                        _SHR_PHY_GPORT_TYPE_PHYN_PORT))
#define _SHR_PHY_GPORT_PHYN_PORT_SET(_gport, _phyn, _port) \
        ((_gport) = (_SHR_GPORT_TYPE_PHY << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_PHY_GPORT_TYPE_PHYN_PORT << _SHR_PHY_GPORT_TYPE_SHIFT) | \
         (((_phyn) & _SHR_PHY_GPORT_PHYN_PORT_PHYN_MASK) << \
                                _SHR_PHY_GPORT_PHYN_PORT_PHYN_SHIFT) | \
         (((_port) & _SHR_PHY_GPORT_PHYN_PORT_PORT_MASK) << \
                                _SHR_PHY_GPORT_PHYN_PORT_PORT_SHIFT))

#define _SHR_PHY_GPORT_PHYN_PORT_PHYN_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_PORT_PHYN_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_PORT_PHYN_MASK)
#define _SHR_PHY_GPORT_PHYN_PORT_PORT_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_PORT_PORT_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_PORT_PORT_MASK)

/* _SHR_PHY_GPORT_TYPE_PHYN_LANE_PORT */
#define _SHR_PHY_GPORT_IS_PHYN_LANE(_gport) \
        (_SHR_GPORT_IS_PHY(_gport) && \
        ((((_gport) >> _SHR_PHY_GPORT_TYPE_SHIFT) & \
                        _SHR_PHY_GPORT_TYPE_MASK) == \
                                        _SHR_PHY_GPORT_TYPE_PHYN_LANE_PORT))
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_SET(_gport, _phyn, _phy_lane, _port) \
        ((_gport) = (_SHR_GPORT_TYPE_PHY << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_PHY_GPORT_TYPE_PHYN_LANE_PORT << _SHR_PHY_GPORT_TYPE_SHIFT) | \
         (((_phyn) & _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_MASK) << \
                                _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_SHIFT) | \
         (((_phy_lane) & _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_MASK) << \
                                _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_SHIFT) | \
         (((_port) & _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_MASK) << \
                                _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_SHIFT))

#define _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_MASK)
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_MASK)
#define _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_MASK)

/* _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_PORT */
#define _SHR_PHY_GPORT_IS_PHYN_SYS_SIDE(_gport) \
        (_SHR_GPORT_IS_PHY(_gport) && \
        ((((_gport) >> _SHR_PHY_GPORT_TYPE_SHIFT) & \
                        _SHR_PHY_GPORT_TYPE_MASK) == \
                                        _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_PORT))
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_SET(_gport, _phyn, _port) \
        ((_gport) = (_SHR_GPORT_TYPE_PHY << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_PORT << _SHR_PHY_GPORT_TYPE_SHIFT) | \
         (((_phyn) & _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_MASK) << \
                                _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_SHIFT) | \
         (((_port) & _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_MASK) << \
                                _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_SHIFT))

#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_MASK)
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_MASK)

/* _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_LANE_PORT */
#define _SHR_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(_gport) \
        (_SHR_GPORT_IS_PHY(_gport) && \
        ((((_gport) >> _SHR_PHY_GPORT_TYPE_SHIFT) & \
                        _SHR_PHY_GPORT_TYPE_MASK) == \
                                        _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_LANE_PORT))
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_SET(_gport, _phyn, _phy_lane, _port) \
        ((_gport) = (_SHR_GPORT_TYPE_PHY << _SHR_GPORT_TYPE_SHIFT) | \
         (_SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_LANE_PORT << _SHR_PHY_GPORT_TYPE_SHIFT) | \
         (((_phyn) & _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_MASK) << \
                                _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_SHIFT) | \
         (((_phy_lane) & _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_MASK) << \
                                _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_SHIFT) | \
         (((_port) & _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_MASK) << \
                                _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_SHIFT))

#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_MASK)
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_MASK)
#define _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_GET(_gport) \
        (((_gport) >> _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_SHIFT) & \
                        _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_MASK)

#define _SHR_GPORT_IS_PROXY(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_PROXY)

#define _SHR_GPORT_PROXY_SET(_gport, _module, _port)                       \
        ((_gport) = (_SHR_GPORT_TYPE_PROXY   << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_module) & _SHR_GPORT_MODID_MASK)  << _SHR_GPORT_MODID_SHIFT)  | \
         (((_port) & _SHR_GPORT_PORT_MASK)     << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_PROXY_MODID_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_MODID_SHIFT) & _SHR_GPORT_MODID_MASK)

#define _SHR_GPORT_PROXY_PORT_GET(_gport)     \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_IS_FORWARD_PORT(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
                                                _SHR_GPORT_TYPE_FORWARD_PORT)

#define _SHR_GPORT_FORWARD_PORT_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_FORWARD_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_FORWARD_PORT_MASK) << _SHR_GPORT_FORWARD_PORT_SHIFT))

#define _SHR_GPORT_FORWARD_PORT_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_FORWARD_PORT_SHIFT) & _SHR_GPORT_FORWARD_PORT_MASK)

#define _SHR_ENCAP_REMOTE_SET(_encap_id)    ((_encap_id) |= (1 << _SHR_ENCAP_REMOTE_SHIFT))
#define _SHR_ENCAP_REMOTE_GET(_encap_id)    (((_encap_id) >> _SHR_ENCAP_REMOTE_SHIFT) & _SHR_ENCAP_REMOTE_MASK)
#define _SHR_ENCAP_ID_SET(_encap_id, _id)                               \
        ((_encap_id) = ((_SHR_ENCAP_REMOTE_MASK << _SHR_ENCAP_REMOTE_SHIFT) & (_encap_id))  | \
        (((_id) & _SHR_ENCAP_ID_MASK) << _SHR_ENCAP_ID_SHIFT))
#define _SHR_ENCAP_ID_GET(_encap_id)        (((_encap_id) >> _SHR_ENCAP_ID_SHIFT) & _SHR_ENCAP_ID_MASK)

#define _SHR_GPORT_IS_VP_GROUP(_gport)    \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
		_SHR_GPORT_TYPE_VP_GROUP)

#define _SHR_GPORT_VP_GROUP_SET(_gport, _vp_group)                              \
        ((_gport) = (_SHR_GPORT_TYPE_VP_GROUP      << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_vp_group) & _SHR_GPORT_VP_GROUP_MASK) << _SHR_GPORT_VP_GROUP_SHIFT))

#define _SHR_GPORT_VP_GROUP_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_VP_GROUP_SHIFT) & _SHR_GPORT_VP_GROUP_MASK)

#define _SHR_GPORT_IS_ANY_PROFILE(_gport) \
        ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == _SHR_GPORT_TYPE_PROFILE)

#define _SHR_GPORT_IS_PROFILE_UNKNOWN(_gport)   \
        (_SHR_GPORT_IS_ANY_PROFILE(_gport) &&   \
        ((((_gport) >> _SHR_GPORT_PROFILE_TYPE_SHIFT) & _SHR_GPORT_PROFILE_TYPE_MASK) ==  _SHR_GPORT_PROFILE_TYPE_UNKNOWN))

#define _SHR_GPORT_PROFILE_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_PROFILE_SHIFT) & _SHR_GPORT_PROFILE_MASK)

#define _SHR_GPORT_PROFILE_UNKNOWN_SET(_gport, _profile)                              \
        ((_gport) = (_SHR_GPORT_TYPE_PROFILE << _SHR_GPORT_TYPE_SHIFT)  |             \
        (_SHR_GPORT_PROFILE_TYPE_UNKNOWN << _SHR_GPORT_PROFILE_TYPE_MASK) |           \
         (((_profile) & _SHR_GPORT_PROFILE_MASK) << _SHR_GPORT_PROFILE_SHIFT))

#define _SHR_GPORT_IS_PROFILE(_gport) _SHR_GPORT_IS_PROFILE_UNKNOWN(_gport)
#define _SHR_GPORT_PROFILE_SET(_gport, _profile) _SHR_GPORT_PROFILE_UNKNOWN_SET(_gport, _profile)

#define _SHR_GPORT_IS_CREDIT_REQUEST_PROFILE(_gport) \
        (_SHR_GPORT_IS_ANY_PROFILE(_gport) &&        \
        ((((_gport) >> _SHR_GPORT_PROFILE_TYPE_SHIFT) & _SHR_GPORT_PROFILE_TYPE_MASK) ==  _SHR_GPORT_PROFILE_TYPE_CREDIT_REQUEST))

#define _SHR_GPORT_IS_INGRESS_LATENCY_PROFILE(_gport) \
        (_SHR_GPORT_IS_ANY_PROFILE(_gport) &&        \
        ((((_gport) >> _SHR_GPORT_PROFILE_TYPE_SHIFT) & _SHR_GPORT_PROFILE_TYPE_MASK) ==  _SHR_GPORT_PROFILE_TYPE_INGRESS_LATENCY))

#define _SHR_GPORT_IS_END_TO_END_LATENCY_PROFILE(_gport) \
        (_SHR_GPORT_IS_ANY_PROFILE(_gport) &&        \
        ((((_gport) >> _SHR_GPORT_PROFILE_TYPE_SHIFT) & _SHR_GPORT_PROFILE_TYPE_MASK) ==  _SHR_GPORT_PROFILE_TYPE_END_TO_END_LATENCY))

#define _SHR_GPORT_IS_LATENCY_AQM_FLOW_ID(_gport) \
        (_SHR_GPORT_IS_ANY_PROFILE(_gport) &&        \
        ((((_gport) >> _SHR_GPORT_PROFILE_TYPE_SHIFT) & _SHR_GPORT_PROFILE_TYPE_MASK) == _SHR_GPORT_PROFILE_TYPE_LATENCY_AQM_FLOW_ID))

#define _SHR_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(_gport) \
        (_SHR_GPORT_IS_ANY_PROFILE(_gport) &&        \
        ((((_gport) >> _SHR_GPORT_PROFILE_TYPE_SHIFT) & _SHR_GPORT_PROFILE_TYPE_MASK) == _SHR_GPORT_PROFILE_TYPE_END_TO_END_AQM_LATENCY))

#define _SHR_GPORT_CREDIT_REQUEST_PROFILE_GET(_gport)   \
        _SHR_GPORT_PROFILE_GET(_gport)

#define _SHR_GPORT_INGRESS_LATENCY_PROFILE_GET(_gport)   \
        _SHR_GPORT_PROFILE_GET(_gport)

#define _SHR_GPORT_END_TO_END_LATENCY_PROFILE_GET(_gport)   \
        _SHR_GPORT_PROFILE_GET(_gport)    
        
#define _SHR_GPORT_LATENCY_AQM_FLOW_ID_GET(_gport)   \
        _SHR_GPORT_PROFILE_GET(_gport)

#define _SHR_GPORT_END_TO_END_AQM_LATENCY_PROFILE_GET(_gport)   \
        _SHR_GPORT_PROFILE_GET(_gport)    

#define _SHR_GPORT_CREDIT_REQUEST_PROFILE_SET(_gport, _profile)                              \
        ((_gport) = (_SHR_GPORT_TYPE_PROFILE << _SHR_GPORT_TYPE_SHIFT)  |                    \
        (_SHR_GPORT_PROFILE_TYPE_CREDIT_REQUEST << _SHR_GPORT_PROFILE_TYPE_SHIFT) |           \
        (((_profile) & _SHR_GPORT_PROFILE_MASK) << _SHR_GPORT_PROFILE_SHIFT))

#define _SHR_GPORT_INGRESS_LATENCY_PROFILE_SET(_gport, _profile)                              \
        ((_gport) = (_SHR_GPORT_TYPE_PROFILE << _SHR_GPORT_TYPE_SHIFT)  |                    \
        (_SHR_GPORT_PROFILE_TYPE_INGRESS_LATENCY << _SHR_GPORT_PROFILE_TYPE_SHIFT) |           \
        (((_profile) & _SHR_GPORT_PROFILE_MASK) << _SHR_GPORT_PROFILE_SHIFT))

#define _SHR_GPORT_END_TO_END_LATENCY_PROFILE_SET(_gport, _profile)                              \
        ((_gport) = (_SHR_GPORT_TYPE_PROFILE << _SHR_GPORT_TYPE_SHIFT)  |                    \
        (_SHR_GPORT_PROFILE_TYPE_END_TO_END_LATENCY << _SHR_GPORT_PROFILE_TYPE_SHIFT) |           \
        (((_profile) & _SHR_GPORT_PROFILE_MASK) << _SHR_GPORT_PROFILE_SHIFT))

#define _SHR_GPORT_LATENCY_AQM_FLOW_ID_SET(_gport, _profile)                              \
        ((_gport) = (_SHR_GPORT_TYPE_PROFILE << _SHR_GPORT_TYPE_SHIFT)  |                    \
        (_SHR_GPORT_PROFILE_TYPE_LATENCY_AQM_FLOW_ID << _SHR_GPORT_PROFILE_TYPE_SHIFT) |           \
        (((_profile) & _SHR_GPORT_PROFILE_MASK) << _SHR_GPORT_PROFILE_SHIFT))

#define _SHR_GPORT_END_TO_END_AQM_LATENCY_PROFILE_SET(_gport, _profile)                              \
        ((_gport) = (_SHR_GPORT_TYPE_PROFILE << _SHR_GPORT_TYPE_SHIFT)  |                    \
        (_SHR_GPORT_PROFILE_TYPE_END_TO_END_AQM_LATENCY << _SHR_GPORT_PROFILE_TYPE_SHIFT) |           \
        (((_profile) & _SHR_GPORT_PROFILE_MASK) << _SHR_GPORT_PROFILE_SHIFT))


#define _SHR_GPORT_LOCAL_FABRIC_SET(_gport, _link_id)\
        ((_gport) = (_SHR_GPORT_TYPE_LOCAL << _SHR_GPORT_TYPE_SHIFT) |\
        (_SHR_GPORT_LOCAL_TYPE_FABRIC << _SHR_GPORT_LOCAL_TYPE_SHIFT) |\
        (((_link_id) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_LOCAL_FABRIC_GET(_gport) \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_IS_LOCAL_FABRIC(_gport) \
        (((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
          _SHR_GPORT_TYPE_LOCAL) && \
         ((((_gport) >> _SHR_GPORT_LOCAL_TYPE_SHIFT) & _SHR_GPORT_LOCAL_TYPE_MASK) == \
          _SHR_GPORT_LOCAL_TYPE_FABRIC))

#define _SHR_GPORT_IS_LOCAL_INTERFACE(_gport) \
        (((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
          _SHR_GPORT_TYPE_LOCAL) && \
         ((((_gport) >> _SHR_GPORT_LOCAL_TYPE_SHIFT) & _SHR_GPORT_LOCAL_TYPE_MASK) == \
          _SHR_GPORT_LOCAL_TYPE_INTERFACE))

#define _SHR_GPORT_LOCAL_INTERFACE_SET(_gport, _port)\
        ((_gport) = (_SHR_GPORT_TYPE_LOCAL << _SHR_GPORT_TYPE_SHIFT) |\
        (_SHR_GPORT_LOCAL_TYPE_INTERFACE << _SHR_GPORT_LOCAL_TYPE_SHIFT) |\
        (((_port) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))
 
#define _SHR_GPORT_LOCAL_INTERFACE_GET(_gport) \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_COSQ_GPORT_IS_E2E_INTERFACE(_gport) \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) && \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) == \
        _SHR_COSQ_GPORT_TYPE_E2E_INTERFACE))

#define _SHR_COSQ_GPORT_E2E_INTERFACE_SET(_gport, _port) \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT) | \
        (_SHR_COSQ_GPORT_TYPE_E2E_INTERFACE << _SHR_COSQ_GPORT_TYPE_SHIFT) | \
        (((_port) & _SHR_COSQ_GPORT_E2E_PORT_MASK) << _SHR_COSQ_GPORT_E2E_PORT_SHIFT))

#define _SHR_COSQ_GPORT_E2E_INTERFACE_GET(_gport) \
        (((_gport) >> _SHR_COSQ_GPORT_E2E_PORT_SHIFT) & _SHR_COSQ_GPORT_E2E_PORT_MASK)

#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_UCAST          0
#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_MCAST          1
#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_TDM            2
#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_UCAST    3
#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_MCAST    4
#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_TDM      5
#define _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_GENERIC_PIPE   6

/*Set macros*/
#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_UCAST_SET(_gport, _core)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, 0, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_UCAST)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_MCAST_SET(_gport, _core)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, 0, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_MCAST)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TDM_SET(_gport, _core)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, 0, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_TDM)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_SET(_gport, _core)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, 0, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_UCAST)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_SET(_gport, _core)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, 0, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_MCAST)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_TDM_SET(_gport, _core)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, 0, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_TDM)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_SET(_gport, _core, _pipe)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, _pipe, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_GENERIC_PIPE)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_SET(_gport, _core, _pipe, _fabric_rx_queue_type)\
            ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   | \
             (_SHR_COSQ_GPORT_TYPE_FABRIC_RX_QUEUE << _SHR_COSQ_GPORT_TYPE_SHIFT)   | \
             (_SHR_CORE_ID2FIELD(_core) << _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) | \
             (((_pipe) & _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_MASK) << _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_SHIFT) | \
             (((_fabric_rx_queue_type) & _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TYPE_MASK)  << _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TYPE_SHIFT))

/*get macros*/
#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_UCAST_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_MCAST_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TDM_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_TDM_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_PIPE_GET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_GET(_gport)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_CORE_GET(_gport)\
    _SHR_CORE_FIELD2ID(((_gport) >> _SHR_COSQ_GPORT_COMMON_CORE_SHIFT) & _SHR_COSQ_GPORT_COMMON_CORE_MASK)

#define _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_GET(_gport)\
    (((_gport) >> _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_SHIFT) & _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_PIPE_MASK)

/*is macros*/
#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_UCAST)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_MCAST)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TDM(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_TDM)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_UCAST)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_MCAST(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_MCAST)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_TDM(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_LOCAL_TDM)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(_gport)  \
    _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _SHR_GPORT_FABRIC_RX_QUEUE_TYPE_GENERIC_PIPE)

#define _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TYPE(_gport, _fabric_rx_queue_type)\
    (_SHR_GPORT_IS_COSQ_BASE(_gport) &&   \
    ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) == _SHR_COSQ_GPORT_TYPE_FABRIC_RX_QUEUE) && \
    ((((_gport) >> _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TYPE_SHIFT) & _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TYPE_MASK) ==  _fabric_rx_queue_type))

/* Core Gport */
#define _SHR_COSQ_GPORT_CORE_SET(_gport, _core)                                 \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |         \
         (_SHR_COSQ_GPORT_TYPE_CORE << _SHR_COSQ_GPORT_TYPE_SHIFT)   |          \
         ((_core == _SHR_CORE_ALL) ? (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE) : \
         (((_core) & _SHR_COSQ_GPORT_CORE_MASK)  << _SHR_COSQ_GPORT_CORE_SHIFT)))

#define _SHR_COSQ_GPORT_CORE_GET(_gport)                                                                           \
        (((((_gport) >> _SHR_COSQ_GPORT_CORE_SHIFT) & _SHR_COSQ_GPORT_CORE_MASK) == (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE)) ? (_SHR_CORE_ALL) : \
          (((_gport) >> _SHR_COSQ_GPORT_CORE_SHIFT) & _SHR_COSQ_GPORT_CORE_MASK))

#define _SHR_COSQ_GPORT_IS_CORE(_gport)                                       \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_CORE))
#define _SHR_COSQ_GPORT_RCY_SET(_gport, _core)                                 \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |         \
         (_SHR_COSQ_GPORT_TYPE_RCY << _SHR_COSQ_GPORT_TYPE_SHIFT)    |         \
         ((_core == _SHR_CORE_ALL) ? (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE) : \
         (((_core) & _SHR_COSQ_GPORT_CORE_MASK)  << _SHR_COSQ_GPORT_CORE_SHIFT)))

#define _SHR_COSQ_GPORT_IS_RCY(_gport)                                       \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_RCY))

#define _SHR_COSQ_GPORT_RCY_MIRR_SET(_gport, _core)                                 \
        ((_gport) = (_SHR_GPORT_TYPE_COSQ << _SHR_GPORT_TYPE_SHIFT)   |         \
         (_SHR_COSQ_GPORT_TYPE_RCY_MIRR << _SHR_COSQ_GPORT_TYPE_SHIFT)|        \
         ((_core == _SHR_CORE_ALL) ? (_SHR_COSQ_GPORT_COMMON_ALL_CORES_VALUE) : \
         (((_core) & _SHR_COSQ_GPORT_CORE_MASK)  << _SHR_COSQ_GPORT_CORE_SHIFT)))

#define _SHR_COSQ_GPORT_IS_RCY_MIRR(_gport)                                       \
        (_SHR_GPORT_IS_COSQ_BASE(_gport) &&                                                 \
        ((((_gport) >> _SHR_COSQ_GPORT_TYPE_SHIFT) & _SHR_COSQ_GPORT_TYPE_MASK) ==     \
                                                _SHR_COSQ_GPORT_TYPE_RCY_MIRR))

/* GPORT subtypes */
#define _SHR_GPORT_SUB_TYPE_PROTECTION                  0
#define _SHR_GPORT_SUB_TYPE_FORWARD_GROUP               1
#define _SHR_GPORT_SUB_TYPE_LIF                         2
#define _SHR_GPORT_SUB_TYPE_MULTICAST                   3
#define _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE           4
#define _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION            7
/**
 * \brief
 *   Used to hold handles to 'virtual egress pointed' objects.
 */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED      8

#define _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF                2

/**
 * \brief
 *  Used to hold handles to ingress glifless native logical interfaces.
 */

#define _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE      (4)
/**
 * \brief
 *  Used to hold handles to egress default match info.
 */

#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT      (5)
/**
 * \brief
 *  Used to hold egress vlan translation (esem) native / non-native handlers.
 */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH                (6)

#define _SHR_GPORT_SUB_TYPE_SHIFT                      22
#define _SHR_GPORT_SUB_TYPE_MASK                      0xF

/* SUB GPORT MULTICAST Protection Switch */
#define _SHR_GPORT_SUB_TYPE_MULTICAST_PS_SECONDARY        0
#define _SHR_GPORT_SUB_TYPE_MULTICAST_PS_PRIMARY          1

#define _SHR_GPORT_SUB_TYPE_MULTICAST_PS_SHIFT           21
#define _SHR_GPORT_SUB_TYPE_MULTICAST_PS_MASK           0x1

#define _SHR_GPORT_SUB_TYPE_MULTICAST_SHIFT               0
#define _SHR_GPORT_SUB_TYPE_MULTICAST_MASK          0x1FFFF

/* SUB GPORT LIF Exclude Bits */
#define _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY          1
#define _SHR_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY           2

#define _SHR_GPORT_SUB_TYPE_LIF_EXC_SHIFT                20
#define _SHR_GPORT_SUB_TYPE_LIF_EXC_MASK                0x3

#define _SHR_GPORT_SUB_TYPE_LIF_SHIFT                      0
#define _SHR_GPORT_SUB_TYPE_LIF_MASK                 0xFFFFF

/* SUB GPORT FORWARD GROUP */
#define _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_SHIFT            0
#define _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_MASK       0x1FFFF

/* SUB GPORT PROTECTION */
#define _SHR_GPORT_SUB_TYPE_PROTECTION_SHIFT               0
#define _SHR_GPORT_SUB_TYPE_PROTECTION_MASK          0x1FFFF

/* SUB GPORT VLAN_TRANSLATION */
#define _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION_SHIFT                0
#define _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION_MASK         (0x3FFFFF)

/* SUB GPORT VIRTUAL_EGRESS_POINTED */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SHIFT          0
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK   (0x3FFFFF)

/* SUB GPORT PUSH PROFILE */
#define _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SHIFT               0
#define _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_MASK              0xF

#define _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_SHIFT             0
#define _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_MASK        0x3FFFF

/* SUB GPORT VIRTUAL INGRESS NATIVE */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SHIFT             (0)
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_MASK       (0x3FFFFF)

/* SUB GPORT VIRTUAL EGRESS_DEFAULT */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SHIFT      (0)
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_MASK    (0xFF)

/* SUB GPORT VIRTUAL EGRESS MATCH */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SHIFT             (0)
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_MASK       (0x3FFFFF)

/* GPORT subtypes MACROS */

/* given gport return if this is PROTECTION subtype */
#define _SHR_GPORT_SUB_TYPE_IS_PROTECTION(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_PROTECTION)

/* given gport return if this is FORWARD GROUP subtype */
#define _SHR_GPORT_SUB_TYPE_IS_FORWARD_GROUP(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_FORWARD_GROUP)

/* given gport return if this is LIF subtype */
#define _SHR_GPORT_SUB_TYPE_IS_LIF(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_LIF)

/* given gport return if this is MULTICAST subtype */
#define _SHR_GPORT_SUB_TYPE_IS_MULTICAST(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_MULTICAST)

/* given gport return if this is VLAN_TRANSLATION subtype */
#define _SHR_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(_gport)             \
     ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION)

/* given gport return if this is VIRTUAL_EGRESS_POINTED subtype */
#define _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_gport)          \
     ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED)

/* given gport return if this is MPLS PUSH PROFILE subtype */
#define _SHR_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE)

/* given gport return if this is TRILL_OUTLIF subtype */
#define _SHR_GPORT_SUB_TYPE_IS_TRILL_OUTLIF(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF)

 /*
  * given gport return if the sub-type is Ingress native 
  */
#define _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(_gport)             \
     ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE)

/* given gport return if this is Virtual Egress Default subtype */
#define _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT)

/* given gport return if this is Virtual EGRESS MATCH subtype */
#define _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(_gport)             \
    ((((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK) == _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH)

/* given gport return its subtype */
#define _SHR_GPORT_SUB_TYPE_GET(_gport)             \
    (((_gport) >> _SHR_GPORT_SUB_TYPE_SHIFT) & _SHR_GPORT_SUB_TYPE_MASK)

/* Get GPORT subvalue of subtype PROTECTION */
#define _SHR_GPORT_SUB_TYPE_PROTECTION_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_PROTECTION) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_PROTECTION_SHIFT) & _SHR_GPORT_SUB_TYPE_PROTECTION_MASK):-1)

/* Get GPORT subvalue of subtype FORWARD_GROUP */
#define _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_FORWARD_GROUP) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_SHIFT) & _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_MASK) :-1)

/* Get GPORT subvalues of subtype LIF */
#define _SHR_GPORT_SUB_TYPE_LIF_VAL_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_LIF) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_LIF_SHIFT) & _SHR_GPORT_SUB_TYPE_LIF_MASK) :-1)
#define _SHR_GPORT_SUB_TYPE_LIF_EXC_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_LIF) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_LIF_EXC_SHIFT) & _SHR_GPORT_SUB_TYPE_LIF_EXC_MASK) :-1)

/* Get GPORT subvalues of subtype MULTICAST */
#define _SHR_GPORT_SUB_TYPE_MULTICAST_VAL_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_MULTICAST) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_MULTICAST_SHIFT) & _SHR_GPORT_SUB_TYPE_MULTICAST_MASK) :-1)
#define _SHR_GPORT_SUB_TYPE_MULTICAST_PS_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_MULTICAST) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_MULTICAST_PS_SHIFT) & _SHR_GPORT_SUB_TYPE_MULTICAST_PS_MASK) :-1)

/* Get GPORT subvalue of subtype VLAN_TRANSLATION */
#define _SHR_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_GET(_gport) \
     ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION) ? \
      (((_gport) >> _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION_SHIFT) & _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION_MASK):-1)

/* Get GPORT subvalue of subtype VIRTUAL_EGRESS_POINTED */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_gport) \
     ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED) ? \
      (((_gport) >> _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SHIFT) & _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK):-1)

/* Get GPORT subvalue of subtype PUSH PROFILE */
#define _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SHIFT) & _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_MASK):-1)

/* Get GPORT subvalue of subtype TRILL_OUTLIF */
#define _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_SHIFT) & _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_MASK):-1)

 /*
  * Get GPORT subvalue of subtype Virtual INGRESS NATIVE 
  */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(_gport) \
     ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE) ? \
      (((_gport) >> _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SHIFT) & _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_MASK):-1)

 /*
  * Get GPORT subvalue of subtype Virtual Egress Port Default 
  */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(_gport) \
     ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT) ? \
      (((_gport) >> _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SHIFT) & _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_MASK):-1)

/* Get GPORT subvalue of subtype Virtual EGRESS MATCH */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(_gport) \
    ((_SHR_GPORT_SUB_TYPE_GET(_gport) == _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH) ? \
     (((_gport) >> _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SHIFT) & _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_MASK):-1)

/* Set GPORT subvalue and subtype PROTECTION */
#define _SHR_GPORT_SUB_TYPE_PROTECTION_SET(_gport, _protection_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_PROTECTION << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_protection_val) & _SHR_GPORT_SUB_TYPE_PROTECTION_MASK) << _SHR_GPORT_SUB_TYPE_PROTECTION_SHIFT))

/* Set GPORT subvalue and subtype FORWARD_GROUP */
#define _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_SET(_gport, _fwd_group_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_FORWARD_GROUP << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_fwd_group_val) & _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_MASK) << _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_SHIFT))

/* Set GPORT subvalue and subtype LIF */
#define _SHR_GPORT_SUB_TYPE_LIF_SET(_gport, _exc_val, _lif_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_LIF << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_exc_val) & _SHR_GPORT_SUB_TYPE_LIF_EXC_MASK) << _SHR_GPORT_SUB_TYPE_LIF_EXC_SHIFT)    |\
      (((_lif_val) & _SHR_GPORT_SUB_TYPE_LIF_MASK) << _SHR_GPORT_SUB_TYPE_LIF_SHIFT))

/* Set GPORT subvalue and subtype MULTICAST */
#define _SHR_GPORT_SUB_TYPE_MULTICAST_SET(_gport, _ps_val, _multicast_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_MULTICAST << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_ps_val) & _SHR_GPORT_SUB_TYPE_MULTICAST_PS_MASK) << _SHR_GPORT_SUB_TYPE_MULTICAST_PS_SHIFT)    |\
      (((_multicast_val) & _SHR_GPORT_SUB_TYPE_MULTICAST_MASK) << _SHR_GPORT_SUB_TYPE_MULTICAST_SHIFT))

/* Set GPORT subvalue and subtype VLAN_TRANSLATION */
#define _SHR_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(_gport, _vlan_translation_val) \
     ((_gport) = (_SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
       (((_vlan_translation_val) & _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION_MASK) << _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION_SHIFT))

/* Set GPORT subvalue and subtype VIRTUAL_EGRESS_POINTED */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(_gport, _virtual_egress_pointed_val) \
     ((_gport) = (_SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
       (((_virtual_egress_pointed_val) & _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK) << _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SHIFT))

/* Set GPORT subvalue and subtype PUSH PROFILE */
#define _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SET(_gport, _mpls_push_profile_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_mpls_push_profile_val) & _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_MASK) << _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SHIFT))

/* Set GPORT subvalue and subtype TRILL_OUTLIF */
#define _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_SET(_gport, _trill_outlif_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_TRILL_OUTLIF << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_trill_outlif_val) & _SHR_GPORT_SUB_TYPE_LIF_MASK) << _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_SHIFT))

/* Set GPORT subvalue and subtype Virtual Ingress Native */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SET(_gport, _virtual_val) \
     ((_gport) = (_SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
       (((_virtual_val) & _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_MASK) << _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SHIFT))

/* Set GPORT subvalue and subtype Virtual Egress Default */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(_gport, _virtual_EGRESS_DEFAULT_val) \
     ((_gport) = (_SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
       (((_virtual_EGRESS_DEFAULT_val) & _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_MASK) << _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SHIFT))

/* Set GPORT subvalue and subtype Virtual EGRESS MATCH */
#define _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(_gport, _virtual_egress_match_val) \
    ((_gport) = (_SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH << _SHR_GPORT_SUB_TYPE_SHIFT)   | \
      (((_virtual_egress_match_val) & _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_MASK) << _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SHIFT))

/* L3 Interface */

#define _SHR_L3_ITF_TYPE_RIF     (0)
#define _SHR_L3_ITF_TYPE_FEC     (1)
#define _SHR_L3_ITF_TYPE_LIF     (2)
/*
 * All subtypes for 'L3_ITF'
 * The value '0' is reserved to indicate 'not encoded by subtype'
 * Range starts at '1'. Currently available:  up to to '15'
 * Note that _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED must be kept, hardcoded,
 * as '0' since there are many locations in the code where _SHR_L3_ITF_SET
 * is used ant it, effectively, encodes sub-type of '0'.
 * {
 */
/*
 * Used for sanity
 */
#define _SHR_L3_ITF_SUB_TYPE_MIN                         0
/*
 * By default, there is no subtype encoding for 'L3_ITF' so the 'not_encoded'
 * subtype is the default subtype.
 */
#define _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED                 0
#define _SHR_L3_ITF_SUB_TYPE_DEFAULT                     _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED
#define _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED      1
/*
 * Used for sanity
 */
#define _SHR_L3_ITF_SUB_TYPE_MAX                         _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED
#define _SHR_L3_ITF_SUB_TYPE_INVALID                     (-1)
/*
 * }
 */
#define _SHR_L3_ITF_INVALID                 (-1)

#define _SHR_L3_ITF_TYPE_SIZE               2 
#define _SHR_L3_ITF_TYPE_SHIFT              29 
#define _SHR_L3_ITF_TYPE_MASK               SAL_UPTO_BIT(_SHR_L3_ITF_TYPE_SIZE)

#define _SHR_L3_ITF_VAL_SHIFT               0
#define _SHR_L3_ITF_VAL_MASK                SAL_UPTO_BIT(_SHR_L3_ITF_TYPE_SHIFT)
/*
 * All macros related to 'L3_ITF' subtypes
 * {
 */
#define _SHR_L3_ITF_SUB_TYPE_SIZE           4
#define _SHR_L3_ITF_SUB_TYPE_SHIFT          (_SHR_L3_ITF_TYPE_SHIFT - _SHR_L3_ITF_SUB_TYPE_SIZE)
#define _SHR_L3_ITF_SUB_TYPE_MASK           SAL_UPTO_BIT(_SHR_L3_ITF_SUB_TYPE_SIZE)
/*
 * Subtype of L3_ITF: NOT_ENCODED
 */
#define _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SIZE           _SHR_L3_ITF_SUB_TYPE_SHIFT
#define _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SHIFT          0
#define _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_MASK           SAL_UPTO_BIT(_SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SIZE)
/*
 * Subtype of L3_ITF: VIRTUAL_EGRESS_POINTED
 */
#define _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SIZE           19
#define _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SHIFT          0
#define _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK           SAL_UPTO_BIT(_SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SIZE)
/*
 * Given L3_ITF indicate whether it is legally encoded by 'sub_type'
 */
#define _SHR_L3_ITF_SUB_TYPE_IS_SET(_l3_itf_)    \
        (((unsigned int)(((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_SHIFT) & _SHR_L3_ITF_SUB_TYPE_MASK) > _SHR_L3_ITF_SUB_TYPE_MIN) && \
         ((unsigned int)(((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_SHIFT) & _SHR_L3_ITF_SUB_TYPE_MASK) <= _SHR_L3_ITF_SUB_TYPE_MAX))
/*
 * Given L3_ITF return its subtype
 */
#define _SHR_L3_ITF_SUB_TYPE_GET(_l3_itf_)             \
    (((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_SHIFT) & _SHR_L3_ITF_SUB_TYPE_MASK)
/*
 * Given L3_ITF return the VALUE in the 'subtype' field
 */
#define _SHR_L3_ITF_SUB_TYPE_VALUE_GET(_l3_itf_)             \
    (((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SHIFT) & _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_MASK)
/*
 * Given full L3_ITF, indicate whether this is not encoded by any subtype
 */
#define _SHR_L3_ITF_SUB_TYPE_IS_NOT_ENCODED(_l3_itf_)          \
     ((((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_SHIFT) & _SHR_L3_ITF_SUB_TYPE_MASK) == _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED)
/*
 * Get L3_ITF subtype value of an ITF which is NOT_ENCODED
 */
#define _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_GET(_l3_itf_) \
     ((_SHR_L3_ITF_SUB_TYPE_GET(_l3_itf_) == _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED) ? \
      (((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SHIFT) & _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_MASK):-1)
/*
 * Set L3_ITF subvalue and subtype NOT_ENCODED
 */
#define _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SET(_l3_itf_, _not_encoded_val) \
     ((_l3_itf_) = (_SHR_L3_ITF_SUB_TYPE_NOT_ENCODED << _SHR_L3_ITF_SUB_TYPE_SHIFT)   | \
       (((_not_encoded_val) & _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_MASK) << _SHR_L3_ITF_SUB_TYPE_NOT_ENCODED_SHIFT))
/*
 * Given full L3_ITF, indicate whether this is VIRTUAL_EGRESS_POINTED subtype
 */
#define _SHR_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_l3_itf_)          \
     ((((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_SHIFT) & _SHR_L3_ITF_SUB_TYPE_MASK) == _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED)
/*
 * Get L3_ITF subvalue of subtype VIRTUAL_EGRESS_POINTED
 */
#define _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_l3_itf_) \
     ((_SHR_L3_ITF_SUB_TYPE_GET(_l3_itf_) == _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED) ? \
      (((_l3_itf_) >> _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SHIFT) & _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK):-1)
/*
 * Set L3_ITF subvalue and subtype VIRTUAL_EGRESS_POINTED
 */
#define _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(_l3_itf_, _virtual_egress_pointed_val) \
     ((_l3_itf_) = (_SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED << _SHR_L3_ITF_SUB_TYPE_SHIFT)   | \
       (((_virtual_egress_pointed_val) & _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_MASK) << _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SHIFT))

/*
 * }
 */
#define _SHR_L3_ITF_TYPE_IS_FEC(_l3_itf_)    \
        (((_l3_itf_) >> _SHR_L3_ITF_TYPE_SHIFT) == _SHR_L3_ITF_TYPE_FEC)

#define _SHR_L3_ITF_TYPE_IS_RIF(_l3_itf_)    \
        (((_l3_itf_) >> _SHR_L3_ITF_TYPE_SHIFT) == _SHR_L3_ITF_TYPE_RIF)

#define _SHR_L3_ITF_TYPE_IS_LIF(_l3_itf_)    \
        (((_l3_itf_) >> _SHR_L3_ITF_TYPE_SHIFT) == _SHR_L3_ITF_TYPE_LIF)

#define _SHR_L3_ITF_VAL_GET(_l3_itf_) \
    (((_l3_itf_) >> _SHR_L3_ITF_VAL_SHIFT) & _SHR_L3_ITF_VAL_MASK)

#define _SHR_L3_ITF_SET(_l3_itf_, _type_, _id_) \
    ((_l3_itf_) = ((((_type_) << _SHR_L3_ITF_TYPE_SHIFT)  | \
     (((_id_) & _SHR_L3_ITF_VAL_MASK) << _SHR_L3_ITF_VAL_SHIFT))))

/* GPORT to IF */
#define _SHR_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(_gport_forward_group, _l3_itf_fec)    \
   do { \
        if (_SHR_L3_ITF_TYPE_IS_FEC(_l3_itf_fec)) { \
              _SHR_GPORT_FORWARD_PORT_SET(_gport_forward_group, _SHR_L3_ITF_VAL_GET(_l3_itf_fec));  \
           } \
        else { \
            _gport_forward_group=_SHR_GPORT_INVALID; \
        } \
  } while (0)

#define _SHR_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(_l3_itf_fec, _gport_forward_group)    \
   do { \
    int __fwd_group_val;    \
    if (_SHR_GPORT_IS_FORWARD_PORT(_gport_forward_group)) { \
        __fwd_group_val = _SHR_GPORT_FORWARD_PORT_GET(_gport_forward_group); \
        _SHR_L3_ITF_SET(_l3_itf_fec, _SHR_L3_ITF_TYPE_FEC, __fwd_group_val); \
    }\
    else {\
        _l3_itf_fec = -1;\
    }\
  } while (0)

#define _SHR_L3_ITF_LIF_TO_GPORT_TUNNEL(_gport_tunnel, _l3_itf_lif)         \
    do \
    {                                       \
        if (_SHR_L3_ITF_TYPE_IS_LIF(_l3_itf_lif)) \
        {                                         \
            if (_SHR_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_l3_itf_lif)) \
            {                       \
                int gport_id_dummy; \
                _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(gport_id_dummy, _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_l3_itf_lif)); \
                _SHR_GPORT_TUNNEL_ID_SET(_gport_tunnel, gport_id_dummy);     \
            }    \
            else if (_SHR_L3_ITF_SUB_TYPE_IS_NOT_ENCODED(_l3_itf_lif))       \
            {    \
                _SHR_GPORT_TUNNEL_ID_SET(_gport_tunnel, _SHR_L3_ITF_VAL_GET(_l3_itf_lif));\
            }    \
            else     \
            {        \
                _gport_tunnel=_SHR_GPORT_INVALID;\
            }        \
        }        \
        else     \
        {        \
            _gport_tunnel=_SHR_GPORT_INVALID;\
        }        \
   } while (0)
 
#define _SHR_GPORT_TUNNEL_TO_L3_ITF_LIF(_l3_itf_lif , _gport_tunnel)    \
    do \
    { \
        int _gport_tunnel_val;                   \
        if (_SHR_GPORT_IS_TUNNEL(_gport_tunnel)) \
        {                                        \
            if (_SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_gport_tunnel))         \
            {                        \
                int l3_itf_id_dummy; \
                _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(l3_itf_id_dummy, _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_gport_tunnel)); \
                _SHR_L3_ITF_SET(_l3_itf_lif, _SHR_L3_ITF_TYPE_LIF, l3_itf_id_dummy);  \
            }                        \
            else                     \
            {                        \
                _gport_tunnel_val = _SHR_GPORT_TUNNEL_ID_GET(_gport_tunnel);          \
                _SHR_L3_ITF_SET(_l3_itf_lif, _SHR_L3_ITF_TYPE_LIF, _gport_tunnel_val);\
            } \
        }     \
        else  \
        {     \
            _l3_itf_lif = _SHR_L3_ITF_INVALID;\
        }     \
    } while (0)

/* FORWARD ENCAP INTERFACE */

#define _SHR_FORWARD_ENCAP_ID_INVALID               0XFFFFFFFF

/* FORWARD ENCAP OUTLIF USAGE */
#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL   0
#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT 1
#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID      2
#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT 3

/* FORWARD ENCAP EEI USAGE */
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT     0
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER 1
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_DEST_VTEP_PTR 2

#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_SHIFT      28
#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_MASK       0x3

#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_SHIFT         28
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MASK          0x3

/* FORWARD ENCAP TYPE */
#define _SHR_FORWARD_ENCAP_ID_TYPE_OUTLIF    0
#define _SHR_FORWARD_ENCAP_ID_TYPE_EEI       1

#define _SHR_FORWARD_ENCAP_ID_TYPE_SHIFT      30
#define _SHR_FORWARD_ENCAP_ID_TYPE_MASK      0x3

#define _SHR_FORWARD_ENCAP_ID_VAL_SHIFT        0
#define _SHR_FORWARD_ENCAP_ID_VAL_MASK         0xFFFFFFF

/* FORWARD ENCAP ID parse for PWE USAGE */
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_SHIFT                      0
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_MASK                       0xfffff

#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_SHIFT            20
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_MASK          0xF

#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(_forward_encap_id, _label, _push_profile)                            \
       ((_forward_encap_id) = (((_label) & _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_MASK)  << _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_SHIFT)  | \
        (((_push_profile) & _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_MASK)     << _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_SHIFT))

#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(_forward_encap_id)  \
        (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_SHIFT) & _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_MASK)

#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(_forward_encap_id)  \
        (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_SHIFT) & _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_MASK)

/* given FORWARD ENCAP id return if this is outlif FORWARD_ENCAP according to type */
#define _SHR_FORWARD_ENCAP_ID_IS_OUTLIF(_forward_encap_id)             \
    (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_TYPE_SHIFT) == _SHR_FORWARD_ENCAP_ID_TYPE_OUTLIF)

/* given FORWARD ENCAP_id return if this EEI FORWARD_ENCAP according to type */
#define _SHR_FORWARD_ENCAP_ID_IS_EEI(_forward_encap_id)             \
    (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_TYPE_SHIFT) == _SHR_FORWARD_ENCAP_ID_TYPE_EEI)

/* given FORWARD ENCAP_id return FORWARD_ENCAP value */
#define _SHR_FORWARD_ENCAP_ID_VAL_GET(_forward_encap_id)     \
    (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_VAL_SHIFT) & _SHR_FORWARD_ENCAP_ID_VAL_MASK)

/* given FORWARD ENCAP_id return FORWARD_ENCAP OUTLIF usage */
#define _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_GET(_forward_encap_id)     \
    (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_SHIFT) & _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_MASK)

/* given FORWARD ENCAP_id return FORWARD_ENCAP EEI usage */
#define _SHR_FORWARD_ENCAP_ID_EEI_USAGE_GET(_forward_encap_id)     \
    (((_forward_encap_id) >> _SHR_FORWARD_ENCAP_ID_EEI_USAGE_SHIFT) & _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MASK)

/* set FORWARD ENCAP_id accoring to type, usage and value */
#define _SHR_FORWARD_ENCAP_ID_VAL_SET(_forward_encap_id, _type, _usage, _val)     \
  do { \
      if (_type == _SHR_FORWARD_ENCAP_ID_TYPE_OUTLIF) {   \
        _forward_encap_id = (_type << _SHR_FORWARD_ENCAP_ID_TYPE_SHIFT)   | \
         (((_usage) &_SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_MASK) << _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_SHIFT)   | \
         (((_val) & _SHR_FORWARD_ENCAP_ID_VAL_MASK) << _SHR_FORWARD_ENCAP_ID_VAL_SHIFT); \
      }\
      else {\
        _forward_encap_id = (_type << _SHR_FORWARD_ENCAP_ID_TYPE_SHIFT)   | \
         (((_usage) &_SHR_FORWARD_ENCAP_ID_EEI_USAGE_MASK) << _SHR_FORWARD_ENCAP_ID_EEI_USAGE_SHIFT)   | \
         (((_val) & _SHR_FORWARD_ENCAP_ID_VAL_MASK) << _SHR_FORWARD_ENCAP_ID_VAL_SHIFT); \
      }\
  } while (0)

#define _SHR_FIELD_CTR_PROC_MASK_LEGACY ((1 << 2) - 1)  /* 2 bits for counter engines in Arad */
#define _SHR_FIELD_CTR_PROC_SHIFT_LEGACY 29     /* minimum 20 for the Statistic-Report Counter in Arad */
#define _SHR_FIELD_CTR_SET_MASK_LEGACY ((1 << _SHR_FIELD_CTR_PROC_SHIFT_LEGACY) - 1)
#define _SHR_FIELD_CTR_SET_SHIFT_LEGACY 0

#define _SHR_FIELD_STAT_ID_PROC_LEGACY_GET(_stat_id) \
    (((_stat_id) >> _SHR_FIELD_CTR_PROC_SHIFT_LEGACY) & _SHR_FIELD_CTR_PROC_MASK_LEGACY)

#define _SHR_FIELD_STAT_ID_CNTR_LEGACY_GET(_stat_id) \
    (((_stat_id) >> _SHR_FIELD_CTR_SET_SHIFT_LEGACY) & _SHR_FIELD_CTR_SET_MASK_LEGACY)

#define _SHR_FIELD_STAT_ID_LEGACY_SET(_proc, _ctr) \
    ((((_proc) & _SHR_FIELD_CTR_PROC_MASK_LEGACY) << _SHR_FIELD_CTR_PROC_SHIFT_LEGACY)\
    | (((_ctr) & _SHR_FIELD_CTR_SET_MASK_LEGACY) << _SHR_FIELD_CTR_SET_SHIFT_LEGACY))

#define _SHR_FIELD_STAT_ID_IS_LEGACY(_stat_id) (!((_stat_id >> 31) & 0x1))
#define _SHR_FIELD_STAT_ID_LEGACY              (0x1 << 31)

/* minimum 20 for the Statistic-Report Counter in Arad */
#define _SHR_FIELD_CTR_PROC_MASK ((1 << 4) - 1) /* 4 bits for counter engines in Arad */

#define _SHR_FIELD_CTR_PROC_SHIFT 27    /* minimum 20 for the Statistic-Report Counter in Jericho */
#define _SHR_FIELD_CTR_SET_MASK ((1 << _SHR_FIELD_CTR_PROC_SHIFT) - 1)
#define _SHR_FIELD_CTR_SET_SHIFT 0

#define _SHR_FIELD_CTR_PROC_SHIFT_GET(_stat_id) (_SHR_FIELD_STAT_ID_IS_LEGACY(_stat_id) ? _SHR_FIELD_CTR_PROC_SHIFT_LEGACY : _SHR_FIELD_CTR_PROC_SHIFT)
#define _SHR_FIELD_STAT_ID_PROC_NEW_GET(_stat_id) \
    (((_stat_id) >> _SHR_FIELD_CTR_PROC_SHIFT) & _SHR_FIELD_CTR_PROC_MASK)

#define _SHR_FIELD_STAT_ID_CNTR_NEW_GET(_stat_id) \
    (((_stat_id) >> _SHR_FIELD_CTR_SET_SHIFT) & _SHR_FIELD_CTR_SET_MASK)

#define _SHR_FIELD_STAT_ID_NEW_SET(_proc, _ctr) \
    (_SHR_FIELD_STAT_ID_LEGACY | \
    (((_proc) & _SHR_FIELD_CTR_PROC_MASK) << _SHR_FIELD_CTR_PROC_SHIFT)|\
     (((_ctr) & _SHR_FIELD_CTR_SET_MASK) << _SHR_FIELD_CTR_SET_SHIFT))

#define _SHR_FIELD_STAT_ID_PROCESSOR_GET(_stat_id) \
             (_SHR_FIELD_STAT_ID_IS_LEGACY(_stat_id) ? \
             _SHR_FIELD_STAT_ID_PROC_LEGACY_GET(_stat_id) : \
             _SHR_FIELD_STAT_ID_PROC_NEW_GET(_stat_id))

#define _SHR_FIELD_STAT_ID_COUNTER_GET(_stat_id) \
             (_SHR_FIELD_STAT_ID_IS_LEGACY(_stat_id) ? \
             _SHR_FIELD_STAT_ID_CNTR_LEGACY_GET(_stat_id) : \
             _SHR_FIELD_STAT_ID_CNTR_NEW_GET(_stat_id))

#define _SHR_FIELD_STAT_ID_SET(_stat_id, _proc, _ctr) \
    (_stat_id = _SHR_FIELD_STAT_ID_NEW_SET(_proc, _ctr))

/**
 * }
 */
#endif /* !_SHR_GPORT_H */
