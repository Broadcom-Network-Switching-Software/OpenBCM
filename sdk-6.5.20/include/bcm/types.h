/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TYPES_H__
#define __BCM_TYPES_H__

#include <sal/types.h>
#include <shared/bitop.h>
#include <shared/pbmp.h>
#include <shared/types.h>
#include <shared/util.h>
#include <shared/gport.h>
#include <shared/cosq.h>
#include <shared/mpls.h>
#include <shared/l3.h>
#include <shared/stat.h>
#include <shared/policer.h>

/* IP constants. */
#define BCM_IP4_ADDRLEN         (_SHR_L3_IP_ADDRLEN) 
#define BCM_IP6_ADDRLEN         (_SHR_L3_IP6_ADDRLEN) 

/* bcm_multicast_t */
typedef int bcm_multicast_t;

#if !defined(BCM_LOCAL_UNITS_MAX)
#define BCM_LOCAL_UNITS_MAX     18         
#endif

#define BCM_MAX_NUM_UNITS       BCM_LOCAL_UNITS_MAX 

#if !defined(BCM_UNITS_MAX)
#define BCM_UNITS_MAX           128        
#endif

#define BCM_PP_PIPES_MAX        _SHR_SWITCH_MAX_PP_PIPES 

#define BCM_PBMP_PORT_MAX       _SHR_PBMP_PORT_MAX 

#define BCM_PBMP_CLEAR(pbm)     _SHR_PBMP_CLEAR(pbm) 
#define BCM_PBMP_MEMBER(bmp, port)  _SHR_PBMP_MEMBER((bmp), (port)) 
#define BCM_PBMP_ITER(bmp, port)  _SHR_PBMP_ITER((bmp), (port)) 
#define BCM_PBMP_COUNT(pbm, count)  _SHR_PBMP_COUNT(pbm, count) 

#define BCM_PBMP_IS_NULL(pbm)   _SHR_PBMP_IS_NULL(pbm) 
#define BCM_PBMP_NOT_NULL(pbm)  _SHR_PBMP_NOT_NULL(pbm) 
#define BCM_PBMP_EQ(pbm_a, pbm_b)  _SHR_PBMP_EQ(pbm_a, pbm_b) 
#define BCM_PBMP_NEQ(pbm_a, pbm_b)  _SHR_PBMP_NEQ(pbm_a, pbm_b) 

/* Assignment operators */
#define BCM_PBMP_ASSIGN(dst, src)  _SHR_PBMP_ASSIGN(dst, src) 
#define BCM_PBMP_AND(pbm_a, pbm_b)  _SHR_PBMP_AND(pbm_a, pbm_b) 
#define BCM_PBMP_OR(pbm_a, pbm_b)  _SHR_PBMP_OR(pbm_a, pbm_b) 
#define BCM_PBMP_XOR(pbm_a, pbm_b)  _SHR_PBMP_XOR(pbm_a, pbm_b) 
#define BCM_PBMP_REMOVE(pbm_a, pbm_b)  _SHR_PBMP_REMOVE(pbm_a, pbm_b) 
#define BCM_PBMP_NEGATE(pbm_a, pbm_b)  _SHR_PBMP_NEGATE(pbm_a, pbm_b) 

/* Port PBMP operators */
#define BCM_PBMP_PORT_SET(pbm, port)  _SHR_PBMP_PORT_SET(pbm, port) 
#define BCM_PBMP_PORT_ADD(pbm, port)  _SHR_PBMP_PORT_ADD(pbm, port) 
#define BCM_PBMP_PORT_REMOVE(pbm, port)  _SHR_PBMP_PORT_REMOVE(pbm, port) 
#define BCM_PBMP_PORT_FLIP(pbm, port)  _SHR_PBMP_PORT_FLIP(pbm, port) 

/* Set the default tag protocol ID (TPID) for the specified port. */
typedef int bcm_port_t;

/* bcm_pbmp_t */
typedef _shr_pbmp_t bcm_pbmp_t;

/* bcm_mac_t */
typedef uint8 bcm_mac_t[6];

#define BCM_MAC_IS_MCAST(_mac_)  \
    (_mac_[0] & 0x1) 

#define BCM_MAC_IS_ZERO(_mac_)  \
    (((_mac_)[0] | (_mac_)[1] | (_mac_)[2] | \
      (_mac_)[3] | (_mac_)[4] | (_mac_)[5]) == 0) 

/* bcm_ip_t */
typedef uint32 bcm_ip_t;

/* bcm_ip6_t */
typedef uint8 bcm_ip6_t[16];

/* bcm_if_t */
typedef int bcm_if_t;

/* bcm_trill_name_t */
typedef int bcm_trill_name_t;

#define BCM_IF_INVALID          -1         

/* bcm_l4_port_t */
typedef int bcm_l4_port_t;

/* bcm_if_group_t */
typedef int bcm_if_group_t;

/* bcm_vrf_t */
typedef int bcm_vrf_t;

/* bcm_mpls_label_t */
typedef uint32 bcm_mpls_label_t;

/* bcm_l3_nat_id_t */
typedef uint32 bcm_l3_nat_id_t;

/* bcm_class_t */
typedef uint64 bcm_class_t;

#define BCM_MPLS_LABEL_INVALID  0xffffffff 

#define BCM_MPLS_EXP_INVALID    -1         

#define BCM_VLAN_NONE           ((bcm_vlan_t)0x0000) 
#define BCM_VLAN_ALL            ((bcm_vlan_t)0xffff) 
#define BCM_VLAN_DEFAULT        ((bcm_vlan_t)0x0001) 
#define BCM_VLAN_INVALID        ((bcm_vlan_t)0x1000) 

#define BCM_VLAN_VALID(id)      \
    ((id) >= BCM_VLAN_DEFAULT && \
     (id) < BCM_VLAN_INVALID) 

#define BCM_VLAN_CTRL(prio, cfi, id)  \
    (((prio) & 0x007) << 13 | \
     ((cfi ) & 0x001) << 12 | \
     ((id  ) & 0xfff) << 0) 

#define BCM_VLAN_CTRL_PRIO(c)   ((c) >> 13 & 0x007) 
#define BCM_VLAN_CTRL_CFI(c)    ((c) >> 12 & 0x001) 
#define BCM_VLAN_CTRL_ID(c)     ((c) >>  0 & 0xfff) 

/* bcm_vlan_t */
typedef uint16 bcm_vlan_t;

/* bcm_ethertype_t */
typedef uint16 bcm_ethertype_t;

/* bcm_vpn_t */
typedef bcm_vlan_t bcm_vpn_t;

/* bcm_mim_vpn_t */
typedef bcm_vlan_t bcm_mim_vpn_t;

/* bcm_policer_t */
typedef int bcm_policer_t;

/* bcm_vlan_tag_t */
typedef uint32 bcm_vlan_tag_t;

/* bcm_tunnel_id_t */
typedef uint32 bcm_tunnel_id_t;

/* bcm_stat_group_mode_attr_class_id_t */
typedef uint32 bcm_stat_group_mode_attr_class_id_t;

#define BCM_VLAN_MIN            0          
#define BCM_VLAN_MAX            4095       
#define BCM_VLAN_COUNT          (BCM_VLAN_MAX - BCM_VLAN_MIN + 1) 
#define BCM_VLAN_UNTAG          0xfff      

#define BCM_VPN_MIN             0          
#define BCM_VPN_MAX             4095       
#define BCM_VPN_COUNT           (BCM_VPN_MAX - BCM_VPN_MIN + 1) 
#define BCM_VPN_ID_MASK         0xfff      

#define _BCM_MAX_VLANS          BCM_VLAN_COUNT /* For backward compatibility */

/* bcm_vlan_vector_t */
typedef uint32 bcm_vlan_vector_t[_SHR_BITDCLSIZE(BCM_VLAN_COUNT)];

/* bcm_vpn_vector_t */
typedef uint32 bcm_vpn_vector_t[_SHR_BITDCLSIZE(BCM_VPN_COUNT)];

#define BCM_VLAN_VEC_GET(vec, n)  SHR_BITGET(vec, n) 
#define BCM_VLAN_VEC_SET(vec, n)  SHR_BITSET(vec, n) 
#define BCM_VLAN_VEC_CLR(vec, n)  SHR_BITCLR(vec, n) 
#define BCM_VLAN_VEC_ZERO(vec)  \
    sal_memset(vec, 0, \
                    SHR_BITALLOCSIZE(BCM_VLAN_COUNT)) 

#define BCM_VPN_VEC_GET(vec, n)  SHR_BITGET(vec, (n & BCM_VPN_ID_MASK)) 
#define BCM_VPN_VEC_SET(vec, n)  SHR_BITSET(vec, (n & BCM_VPN_ID_MASK)) 
#define BCM_VPN_VEC_CLR(vec, n)  SHR_BITCLR(vec, (n & BCM_VPN_ID_MASK)) 
#define BCM_VPN_VEC_ZERO(vec)   \
    sal_memset(vec, 0, \
                    SHR_BITALLOCSIZE(BCM_VPN_COUNT)) 

/* bcm_cos_t */
typedef int bcm_cos_t;

/* bcm_cos_queue_t */
typedef int bcm_cos_queue_t;

/* bcm_cosq_buffer_id_t */
typedef int bcm_cosq_buffer_id_t;

/* bcm_core_t */
typedef int32 bcm_core_t;

#define BCM_COS_MIN             0          
#define BCM_COS_MAX             7          
#define BCM_COS_COUNT           8          
#define BCM_COS_DEFAULT         4          
#define BCM_COS_INVALID         -1         

#define BCM_MAX_MEM_REG_NAME    100        /* Max mem/register name bytes */
#define BCM_MAX_MEM_WORDS       20         /* Max memory entry words */

/* Special buffer id */
#define BCM_COSQ_BUFFER_ID_INVALID  -1         /* ignored buffer id */

#define BCM_PRIO_MIN            0          
#define BCM_PRIO_MAX            7          
#define BCM_PRIO_RED            0x100      
#define BCM_PRIO_YELLOW         0x200      
#define BCM_PRIO_DROP_FIRST     BCM_PRIO_RED 
#define BCM_PRIO_MASK           0xff       
#define BCM_PRIO_GREEN          0x400      
#define BCM_PRIO_DROP_LAST      0x800      
#define BCM_PRIO_PRESERVE       BCM_PRIO_DROP_LAST 
#define BCM_PRIO_STAG           0x100      
#define BCM_PRIO_CTAG           0x200      
#define BCM_DSCP_ECN            0x100      
#define BCM_PRIO_BLACK          0x1000     
#define BCM_PRIO_SECONDARY      0x2000     

/* bcm_module_t */
typedef int bcm_module_t;

#define BCM_MODID_INVALID       ((bcm_module_t) -1) 
#define BCM_MODID_ALL           ((bcm_module_t) -2) 

#define BCM_TRUNK_INVALID       ((bcm_trunk_t) -1) 

/* bcm_trunk_t */
typedef int bcm_trunk_t;

/* bcm_switch_agm_id_t */
typedef int bcm_switch_agm_id_t;

/* Split Horizon Network Group */
typedef int bcm_switch_network_group_t;

/* 
 * GPORT (generic port) definitions.
 * See shared/gport.h for more details.
 */
typedef int bcm_gport_t;

/* 
 * L3_ITF (generic l3 interface) definitions.
 * See shared/gport.h for more details.
 */
typedef int bcm_l3_itf_t;

/* System-wide port types */
#define BCM_GPORT_TYPE_NONE                 _SHR_GPORT_NONE 
#define BCM_GPORT_INVALID                   _SHR_GPORT_INVALID 
#define BCM_GPORT_TYPE_LOCAL                _SHR_GPORT_TYPE_LOCAL 
#define BCM_GPORT_TYPE_MODPORT              _SHR_GPORT_TYPE_MODPORT 
#define BCM_GPORT_TYPE_TRUNK                _SHR_GPORT_TYPE_TRUNK 
#define BCM_GPORT_TYPE_BLACK_HOLE           _SHR_GPORT_TYPE_BLACK_HOLE 
#define BCM_GPORT_TYPE_LOCAL_CPU            _SHR_GPORT_TYPE_LOCAL_CPU 
#define BCM_GPORT_MPLS_PORT                 _SHR_GPORT_TYPE_MPLS_PORT 
#define BCM_GPORT_SUBPORT_GROUP             _SHR_GPORT_TYPE_SUBPORT_GROUP 
#define BCM_GPORT_SUBPORT_PORT              _SHR_GPORT_TYPE_SUBPORT_PORT 
#define BCM_GPORT_TYPE_UCAST_QUEUE_GROUP    _SHR_GPORT_TYPE_UCAST_QUEUE_GROUP 
#define BCM_GPORT_TYPE_DESTMOD_QUEUE_GROUP  _SHR_GPORT_TYPE_DESTMOD_QUEUE_GROUP 
#define BCM_GPORT_TYPE_MCAST                _SHR_GPORT_TYPE_MCAST 
#define BCM_GPORT_TYPE_MCAST_QUEUE_GROUP    _SHR_GPORT_TYPE_MCAST_QUEUE_GROUP 
#define BCM_GPORT_TYPE_SCHEDULER            _SHR_GPORT_TYPE_SCHEDULER 
#define BCM_GPORT_TYPE_SPECIAL              _SHR_GPORT_TYPE_SPECIAL 
#define BCM_GPORT_TYPE_MIRROR               _SHR_GPORT_TYPE_MIRROR 
#define BCM_GPORT_MIM_PORT                  _SHR_GPORT_TYPE_MIM_PORT 
#define BCM_GPORT_VLAN_PORT                 _SHR_GPORT_TYPE_VLAN_PORT 
#define BCM_GPORT_WLAN_PORT                 _SHR_GPORT_TYPE_WLAN_PORT 
#define BCM_GPORT_TRILL_PORT                _SHR_GPORT_TYPE_TRILL_PORT 
#define BCM_GPORT_NIV_PORT                  _SHR_GPORT_TYPE_NIV_PORT 
#define BCM_GPORT_L2GRE_PORT                _SHR_GPORT_TYPE_L2GRE_PORT 
#define BCM_GPORT_VXLAN_PORT                _SHR_GPORT_TYPE_VXLAN_PORT 
#define BCM_GPORT_FLOW_PORT                 _SHR_GPORT_TYPE_FLOW_PORT 
#define BCM_GPORT_EXTENDER_PORT             _SHR_GPORT_TYPE_EXTENDER_PORT 
#define BCM_EXTENDER_GPORT_TYPE_PORT        _SHR_EXTENDER_GPORT_TYPE_PORT 
#define BCM_EXTENDER_GPORT_TYPE_FORWARD     _SHR_EXTENDER_GPORT_TYPE_FORWARD 
#define BCM_EXTENDER_GPORT_TYPE_ENCAP       _SHR_EXTENDER_GPORT_TYPE_ENCAP 
#define BCM_GPORT_MAC_PORT                  _SHR_GPORT_TYPE_MAC_PORT 
#define BCM_GPORT_TYPE_TUNNEL               _SHR_GPORT_TYPE_TUNNEL 
#define BCM_GPORT_TYPE_CHILD                _SHR_GPORT_TYPE_CHILD 
#define BCM_GPORT_TYPE_EGRESS_GROUP         _SHR_GPORT_TYPE_EGRESS_GROUP 
#define BCM_GPORT_TYPE_EGRESS_CHILD         _SHR_GPORT_TYPE_EGRESS_CHILD 
#define BCM_GPORT_TYPE_EGRESS_MODPORT       _SHR_GPORT_TYPE_EGRESS_MODPORT 
#define BCM_GPORT_TYPE_UCAST_SUBSCRIBER_QUEUE_GROUP _SHR_GPORT_TYPE_UCAST_SUBSCRIBER_QUEUE_GROUP 
#define BCM_GPORT_TYPE_MCAST_SUBSCRIBER_QUEUE_GROUP _SHR_GPORT_TYPE_MCAST_SUBSCRIBER_QUEUE_GROUP 
#define BCM_GPORT_TYPE_CONGESTION           _SHR_GPORT_TYPE_CONGESTION 
#define BCM_GPORT_TYPE_SYSTEM_PORT          _SHR_GPORT_TYPE_SYSTEM_PORT 
#define BCM_GPORT_TYPE_COSQ                 _SHR_GPORT_TYPE_COSQ 
#define BCM_COSQ_GPORT_TYPE_VOQ_CONNECTOR   _SHR_COSQ_EXT_GPORT_TYPE_VOQ_CONNECTOR 
#define BCM_COSQ_GPORT_TYPE_COMPOSITE_SF2_SCHED _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_SCHED 
#define BCM_COSQ_GPORT_TYPE_COMPOSITE_SF2_CONN _SHR_COSQ_EXT_GPORT_TYPE_COMPOSITE_SF2_CONN 
#define BCM_COSQ_GPORT_TYPE_UCAST_EGRESS_QUEUE _SHR_COSQ_GPORT_TYPE_UCAST_EGRESS_QUEUE 
#define BCM_COSQ_GPORT_TYPE_MCAST_EGRESS_QUEUE _SHR_COSQ_GPORT_TYPE_MCAST_EGRESS_QUEUE 
#define BCM_COSQ_GPORT_TYPE_VSQ             _SHR_COSQ_GPORT_TYPE_VSQ 
#define BCM_COSQ_GPORT_TYPE_E2E_PORT        _SHR_COSQ_GPORT_TYPE_E2E_PORT 
#define BCM_COSQ_GPORT_TYPE_ISQ             _SHR_COSQ_GPORT_TYPE_ISQ 
#define BCM_COSQ_GPORT_TYPE_MULTIPATH       _SHR_COSQ_GPORT_TYPE_MULTIPATH 
#define BCM_COSQ_GPORT_TYPE_SCHED_CIR       _SHR_COSQ_EXT_GPORT_TYPE_SCHED_CIR 
#define BCM_COSQ_GPORT_TYPE_SCHED_PIR       _SHR_COSQ_EXT_GPORT_TYPE_SCHED_PIR 
#define BCM_COSQ_GPORT_TYPE_SHARED_SHAPER_ELEM _SHR_COSQ_EXT_GPORT_TYPE_SHARED_SHAPER_ELEM 
#define BCM_COSQ_GPORT_TYPE_PORT_TC         _SHR_COSQ_GPORT_TYPE_PORT_TC 
#define BCM_COSQ_GPORT_TYPE_PORT_TCG        _SHR_COSQ_GPORT_TYPE_PORT_TCG 
#define BCM_COSQ_GPORT_TYPE_E2E_PORT_TC     _SHR_COSQ_GPORT_TYPE_E2E_PORT_TC 
#define BCM_COSQ_GPORT_TYPE_E2E_PORT_TCG    _SHR_COSQ_GPORT_TYPE_E2E_PORT_TCG 
#define BCM_COSQ_GPORT_TYPE_SYSTEM_RED      _SHR_COSQ_GPORT_TYPE_SYSTEM_RED 
#define BCM_COSQ_GPORT_TYPE_FABRIC          _SHR_COSQ_GPORT_TYPE_FABRIC 
#define BCM_GPORT_TYPE_EPON_LINK            _SHR_GPORT_TYPE_EPON_LINK 
#define BCM_GPORT_TYPE_PHY                  _SHR_GPORT_TYPE_PHY 
#define BCM_PHY_GPORT_TYPE_LANE_PORT        _SHR_PHY_GPORT_TYPE_LANE_PORT 
#define BCM_PHY_GPORT_TYPE_PHYN_PORT        _SHR_PHY_GPORT_TYPE_PHYN_PORT 
#define BCM_PHY_GPORT_TYPE_PHYN_LANE_PORT   _SHR_PHY_GPORT_TYPE_PHYN_LANE_PORT 
#define BCM_PHY_GPORT_TYPE_PHYN_SYS_SIDE_PORT _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_PORT 
#define BCM_PHY_GPORT_TYPE_PHYN_SYS_SIDE_LANE_PORT _SHR_PHY_GPORT_TYPE_PHYN_SYS_SIDE_LANE_PORT 
#define BCM_GPORT_TYPE_PROXY                _SHR_GPORT_TYPE_PROXY 
#define BCM_GPORT_TYPE_FORWARD_PORT         _SHR_GPORT_TYPE_FORWARD_PORT 
#define BCM_GPORT_TYPE_VP_GROUP             _SHR_GPORT_TYPE_VP_GROUP 
#define BCM_COSQ_GPORT_TYPE_SRC_QUEUE       _SHR_COSQ_GPORT_TYPE_SRC_QUEUE 
#define BCM_COSQ_GPORT_TYPE_DST_QUEUE       _SHR_COSQ_GPORT_TYPE_DST_QUEUE 
#define BCM_COSQ_GPORT_TYPE_CORE            _SHR_COSQ_GPORT_TYPE_CORE 
#define BCM_GPORT_TYPE_PROFILE              _SHR_GPORT_TYPE_PROFILE 
#define BCM_GPORT_TYPE_VPLAG_PORT           _SHR_GPORT_TYPE_VPLAG_PORT 
#define BCM_GPORT_TYPE_FLEXE_GROUP          _SHR_GPORT_TYPE_FLEXE_GROUP 
#define BCM_GPORT_TYPE_TDM_STREAM           _SHR_GPORT_TYPE_TDM_STREAM 
#define BCM_GPORT_TYPE_NIF_RX_PRIORITY      _SHR_GPORT_TYPE_NIF_RX_PRIORITY 

#define BCM_GPORT_IS_SET(_gport)            _SHR_GPORT_IS_SET(_gport) 
#define BCM_GPORT_IS_LOCAL(_gport)          _SHR_GPORT_IS_LOCAL(_gport) 
#define BCM_GPORT_IS_MODPORT(_gport)        _SHR_GPORT_IS_MODPORT(_gport) 
#define BCM_GPORT_IS_TRUNK(_gport)          _SHR_GPORT_IS_TRUNK(_gport) 
#define BCM_GPORT_IS_UCAST_QUEUE_GROUP(_gport)  _SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport) 
#define BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(_gport)  _SHR_GPORT_IS_DESTMOD_QUEUE_GROUP(_gport) 
#define BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(_gport)  _SHR_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(_gport) 
#define BCM_GPORT_IS_MCAST(_gport)          _SHR_GPORT_IS_MCAST(_gport) 
#define BCM_GPORT_IS_MCAST_QUEUE_GROUP(_gport)  _SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) 
#define BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(_gport)  _SHR_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(_gport) 
#define BCM_GPORT_IS_BLACK_HOLE(_gport)     _SHR_GPORT_IS_BLACK_HOLE(_gport) 
#define BCM_GPORT_IS_LOCAL_CPU(_gport)      _SHR_GPORT_IS_LOCAL_CPU(_gport) 
#define BCM_GPORT_IS_LOCAL_CPU_IEEE(_gport)  _SHR_GPORT_IS_LOCAL_CPU_IEEE(_gport) 
#define BCM_GPORT_IS_LOCAL_CPU_HIGIG(_gport)  _SHR_GPORT_IS_LOCAL_CPU_HIGIG(_gport) 
#define BCM_GPORT_IS_MPLS_PORT(_gport)      _SHR_GPORT_IS_MPLS_PORT(_gport) 
#define BCM_GPORT_IS_SUBPORT_GROUP(_gport)  _SHR_GPORT_IS_SUBPORT_GROUP(_gport) 
#define BCM_GPORT_IS_SUBPORT_PORT(_gport)   _SHR_GPORT_IS_SUBPORT_PORT(_gport) 
#define BCM_GPORT_IS_SCHEDULER(_gport)      _SHR_GPORT_IS_SCHEDULER(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS(_gport)    _SHR_GPORT_IS_FABRIC_CLOS(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FABRIC(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FABRIC(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV1(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV1(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV2(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV2(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV3(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV3(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV4(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV4(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV5(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV5(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV6(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV6(_gport) 
#define BCM_GPORT_IS_EPON_LINK(_gport)      _SHR_GPORT_IS_EPON_LINK(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV7(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV7(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH(_gport)    _SHR_GPORT_IS_FABRIC_MESH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_COMMON_LOCAL0(_gport)  _SHR_GPORT_IS_FABRIC_MESH_COMMON_LOCAL0(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_COMMON_LOCAL1(_gport)  _SHR_GPORT_IS_FABRIC_MESH_COMMON_LOCAL1(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_COMMON_DEV1(_gport)  _SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV1(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_COMMON_DEV2(_gport)  _SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV2(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_COMMON_DEV3(_gport)  _SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV3(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_COMMON_MC(_gport)  _SHR_GPORT_IS_FABRIC_MESH_COMMON_MC(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL0(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL0(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL1(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL1(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_COMMON_FABRIC(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_COMMON_FABRIC(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_COMMON_UNICAST_FABRIC(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_COMMON_UNICAST_FABRIC(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_COMMON_MULTICAST_FABRIC(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_COMMON_MULTICAST_FABRIC(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV1_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV1_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV1_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV1_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV1_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV1_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV1_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV1_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV2_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV2_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV2_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV2_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV2_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV2_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV2_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV2_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV3_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV3_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV3_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV3_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV3_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV3_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_DEV3_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_DEV3_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_MC_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_MC_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_MC_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_MC_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_MC_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_MESH_MC_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_MC_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_MESH_MC_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL0(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL0(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_LOCAL1(_gport)  _SHR_GPORT_IS_FABRIC_MESH_LOCAL1(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_MC(_gport)  _SHR_GPORT_IS_FABRIC_MESH_MC(_gport) 
#define BCM_GPORT_IS_FABRIC_MESH_SCOPE(_gport)  _SHR_GPORT_IS_FABRIC_MESH_SCOPE(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_OCB(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_OCB(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_MIX(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_MIX(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL0_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_LOCAL1_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_OCB_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_OCB_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_MIX_HIGH(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_MIX_HIGH(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_OCB_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_OCB_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_MIX_LOW(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_MIX_LOW(_gport) 
#define BCM_GPORT_IS_FABRIC_CLOS_SCOPE(_gport)  _SHR_GPORT_IS_FABRIC_CLOS_SCOPE(_gport) 
#define BCM_GPORT_IS_DEVPORT(_gport)        _SHR_GPORT_IS_DEVPORT(_gport) 
#define BCM_GPORT_IS_SPECIAL(_gport)        _SHR_GPORT_IS_SPECIAL(_gport) 
#define BCM_GPORT_IS_MIRROR(_gport)         _SHR_GPORT_IS_MIRROR(_gport) 
#define BCM_GPORT_IS_MIM_PORT(_gport)       _SHR_GPORT_IS_MIM_PORT(_gport) 
#define BCM_GPORT_IS_VLAN_PORT(_gport)      _SHR_GPORT_IS_VLAN_PORT(_gport) 
#define BCM_GPORT_IS_WLAN_PORT(_gport)      _SHR_GPORT_IS_WLAN_PORT(_gport) 
#define BCM_GPORT_IS_TRILL_PORT(_gport)     _SHR_GPORT_IS_TRILL_PORT(_gport) 
#define BCM_GPORT_IS_NIV_PORT(_gport)       _SHR_GPORT_IS_NIV_PORT(_gport) 
#define BCM_GPORT_IS_L2GRE_PORT(_gport)     _SHR_GPORT_IS_L2GRE_PORT(_gport) 
#define BCM_GPORT_IS_VXLAN_PORT(_gport)     _SHR_GPORT_IS_VXLAN_PORT(_gport) 
#define BCM_GPORT_IS_FLOW_PORT(_gport)      _SHR_GPORT_IS_FLOW_PORT(_gport) 
#define BCM_GPORT_IS_EXTENDER_PORT(_gport)  _SHR_GPORT_IS_EXTENDER_PORT(_gport) 
#define BCM_EXTENDER_GPORT_IS_PORT(_gport)  _SHR_EXTENDER_GPORT_IS_PORT(_gport) 
#define BCM_EXTENDER_GPORT_IS_FORWARD(_gport)  _SHR_EXTENDER_GPORT_IS_FORWARD(_gport) 
#define BCM_EXTENDER_GPORT_IS_ENCAP(_gport)  _SHR_EXTENDER_GPORT_IS_ENCAP(_gport) 
#define BCM_GPORT_IS_MAC_PORT(_gport)       _SHR_GPORT_IS_MAC_PORT(_gport) 
#define BCM_GPORT_IS_TUNNEL(_gport)         _SHR_GPORT_IS_TUNNEL(_gport) 
#define BCM_GPORT_IS_CHILD(_gport)          _SHR_GPORT_IS_CHILD(_gport) 
#define BCM_GPORT_IS_EGRESS_GROUP(_gport)   _SHR_GPORT_IS_EGRESS_GROUP(_gport) 
#define BCM_GPORT_IS_EGRESS_CHILD(_gport)   _SHR_GPORT_IS_EGRESS_CHILD(_gport) 
#define BCM_GPORT_IS_EGRESS_MODPORT(_gport)  _SHR_GPORT_IS_EGRESS_MODPORT(_gport) 
#define BCM_GPORT_IS_CONGESTION(_gport)     _SHR_GPORT_IS_CONGESTION(_gport) 
#define BCM_GPORT_IS_CONGESTION_COE(_gport)  _SHR_GPORT_IS_CONGESTION_COE(_gport) 
#define BCM_GPORT_IS_CONGESTION_OOB(_gport)  _SHR_GPORT_IS_CONGESTION_OOB(_gport) 
#define BCM_GPORT_IS_NIF_RX_PRIORITY(_gport)  _SHR_GPORT_IS_NIF_RX_PRIORITY(_gport) 
#define BCM_GPORT_IS_NIF_RX_PRIORITY_LOW(_gport)  _SHR_GPORT_IS_NIF_RX_PRIORITY_LOW(_gport) 
#define BCM_GPORT_IS_NIF_RX_PRIORITY_HIGH(_gport)  _SHR_GPORT_IS_NIF_RX_PRIORITY_HIGH(_gport) 
#define BCM_GPORT_IS_NIF_RX_PRIORITY_TDM(_gport)  _SHR_GPORT_IS_NIF_RX_PRIORITY_TDM(_gport) 
#define BCM_GPORT_IS_SYSTEM_PORT(_gport)    _SHR_GPORT_IS_SYSTEM_PORT(_gport) 
#define BCM_GPORT_IS_COSQ(_gport)           _SHR_GPORT_IS_COSQ(_gport) 
#define BCM_COSQ_GPORT_IS_ISQ(_gport)       _SHR_COSQ_GPORT_IS_ISQ(_gport) 
#define BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(_gport)  _SHR_COSQ_GPORT_IS_VOQ_CONNECTOR(_gport) 
#define BCM_COSQ_GPORT_IS_SRC_QUEUE(_gport)  _SHR_COSQ_GPORT_IS_SRC_QUEUE(_gport) 
#define BCM_COSQ_GPORT_IS_DST_QUEUE(_gport)  _SHR_COSQ_GPORT_IS_DST_QUEUE(_gport) 
#define BCM_COSQ_GPORT_IS_COMPOSITE_SF2(_gport)  _SHR_COSQ_GPORT_IS_COMPOSITE_SF2(_gport) 
#define BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(_gport)  _SHR_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(_gport) 
#define BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(_gport)  _SHR_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(_gport) 
#define BCM_COSQ_GPORT_IS_VSQ(_gport)       _SHR_COSQ_GPORT_IS_VSQ(_gport) 
#define BCM_COSQ_GPORT_IS_E2E_PORT(_gport)  _SHR_COSQ_GPORT_IS_E2E_PORT(_gport) 
#define BCM_COSQ_GPORT_IS_MULTIPATH(_gport)  _SHR_COSQ_GPORT_IS_MULTIPATH(_gport) 
#define BCM_COSQ_GPORT_IS_SCHED_CIR(_gport)  _SHR_COSQ_GPORT_IS_SCHED_CIR(_gport) 
#define BCM_COSQ_GPORT_IS_SCHED_PIR(_gport)  _SHR_COSQ_GPORT_IS_SCHED_PIR(_gport) 
#define BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(_gport)  _SHR_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(_gport) 
#define BCM_COSQ_GPORT_IS_PORT_TC(_gport)   _SHR_COSQ_GPORT_IS_PORT_TC(_gport) 
#define BCM_COSQ_GPORT_IS_PORT_TCG(_gport)  _SHR_COSQ_GPORT_IS_PORT_TCG(_gport) 
#define BCM_COSQ_GPORT_IS_E2E_PORT_TC(_gport)  _SHR_COSQ_GPORT_IS_E2E_PORT_TC(_gport) 
#define BCM_COSQ_GPORT_IS_E2E_PORT_TCG(_gport)  _SHR_COSQ_GPORT_IS_E2E_PORT_TCG(_gport) 
#define BCM_COSQ_GPORT_IS_ISQ_ROOT(_gport)  _SHR_COSQ_GPORT_IS_ISQ_ROOT(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_ROOT(_gport)  _SHR_COSQ_GPORT_IS_FMQ_ROOT(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_GUARANTEED(_gport)  _SHR_COSQ_GPORT_IS_FMQ_GUARANTEED(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(_gport)  _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT_AGR(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT0(_gport)  _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT0(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT1(_gport)  _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT1(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_BESTEFFORT2(_gport)  _SHR_COSQ_GPORT_IS_FMQ_BESTEFFORT2(_gport) 
#define BCM_COSQ_GPORT_IS_FMQ_CLASS(_gport)  _SHR_COSQ_GPORT_IS_FMQ_CLASS(_gport) 
#define BCM_COSQ_GPORT_IS_SYSTEM_RED(_gport)  _SHR_COSQ_GPORT_IS_SYSTEM_RED(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_PIPE(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_PIPE(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(_gport) 
#define BCM_GPORT_IS_PHY(_gport)            _SHR_GPORT_IS_PHY(_gport) 
#define BCM_PHY_GPORT_IS_LANE(_gport)       _SHR_PHY_GPORT_IS_LANE(_gport) 
#define BCM_PHY_GPORT_IS_PHYN(_gport)       _SHR_PHY_GPORT_IS_PHYN(_gport) 
#define BCM_PHY_GPORT_IS_PHYN_LANE(_gport)  _SHR_PHY_GPORT_IS_PHYN_LANE(_gport) 
#define BCM_PHY_GPORT_IS_PHYN_SYS_SIDE(_gport)  _SHR_PHY_GPORT_IS_PHYN_SYS_SIDE(_gport) 
#define BCM_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(_gport)  _SHR_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(_gport) 
#define BCM_GPORT_IS_PROXY(_gport)          _SHR_GPORT_IS_PROXY(_gport) 
#define BCM_GPORT_IS_FORWARD_PORT(_gport)   _SHR_GPORT_IS_FORWARD_PORT(_gport) 
#define BCM_GPORT_IS_VP_GROUP(_gport)       _SHR_GPORT_IS_VP_GROUP(_gport) 
#define BCM_GPORT_IS_PROFILE(_gport)        _SHR_GPORT_IS_PROFILE(_gport) 
#define BCM_ENCAP_REMOTE_SET(_encap_id)     _SHR_ENCAP_REMOTE_SET(_encap_id) 
#define BCM_ENCAP_REMOTE_GET(_encap_id)     _SHR_ENCAP_REMOTE_GET(_encap_id) 
#define BCM_ENCAP_ID_SET(_encap_id, _id)    _SHR_ENCAP_ID_SET(_encap_id, _id) 
#define BCM_ENCAP_ID_GET(_encap_id)         _SHR_ENCAP_ID_GET(_encap_id) 
#define BCM_GPORT_IS_VPLAG_PORT(_gport)     _SHR_GPORT_IS_VPLAG_PORT(_gport) 
#define BCM_COSQ_GPORT_IS_INBAND_COE(_gport)  _SHR_COSQ_GPORT_IS_INBAND_COE(_gport) 
#define BCM_GPORT_IS_FLEXE_GROUP(_gport)    _SHR_GPORT_IS_FLEXE_GROUP(_gport) 
#define BCM_GPORT_IS_TDM_STREAM(_gport)     _SHR_GPORT_IS_TDM_STREAM(_gport) 

#define BCM_GPORT_LOCAL_SET(_gport, _port)  \
    _SHR_GPORT_LOCAL_SET(_gport, _port) 

#define BCM_GPORT_LOCAL_GET(_gport)  \
    (!_SHR_GPORT_IS_LOCAL(_gport) ? -1 : \
    _SHR_GPORT_LOCAL_GET(_gport)) 

#define BCM_GPORT_MODPORT_SET(_gport, _module, _port)  \
    _SHR_GPORT_MODPORT_SET(_gport, _module, _port) 

#define BCM_GPORT_MODPORT_MODID_GET(_gport)  \
    (!_SHR_GPORT_IS_MODPORT(_gport) ? -1 : \
    _SHR_GPORT_MODPORT_MODID_GET(_gport)) 

#define BCM_GPORT_MODPORT_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_MODPORT(_gport) ? -1 : \
    _SHR_GPORT_MODPORT_PORT_GET(_gport)) 

#define BCM_GPORT_TRUNK_SET(_gport, _trunk_id)  \
    _SHR_GPORT_TRUNK_SET(_gport, _trunk_id) 

#define BCM_GPORT_VP_GROUP_SET(_gport, _vp_group)  \
    _SHR_GPORT_VP_GROUP_SET(_gport, _vp_group) 

#define BCM_GPORT_PROFILE_SET(_gport, _profile)  \
    _SHR_GPORT_PROFILE_SET(_gport, _profile) 

#define BCM_GPORT_VP_GROUP_GET(_gport)  \
    (!_SHR_GPORT_IS_VP_GROUP(_gport) ? -1 : \
    _SHR_GPORT_VP_GROUP_GET(_gport)) 

#define BCM_GPORT_PROFILE_GET(_gport)  \
    (!_SHR_GPORT_IS_PROFILE(_gport) ? -1 : \
    _SHR_GPORT_PROFILE_GET(_gport)) 

#define BCM_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength)  \
   _SHR_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength) 

#define BCM_GPORT_TRAP_GET_ID(_gport)  \
     (!_SHR_GPORT_IS_TRAP(_gport) ? -1 : \
   _SHR_GPORT_TRAP_GET_ID(_gport)) 

#define BCM_GPORT_TRAP_GET_STRENGTH(_gport)  \
   _SHR_GPORT_TRAP_GET_STRENGTH(_gport) 

#define BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(_gport)  \
   _SHR_GPORT_TRAP_GET_SNOOP_STRENGTH(_gport) 

#define BCM_GPORT_IS_TRAP(_gport)  \
   _SHR_GPORT_IS_TRAP(_gport)         

#define BCM_GPORT_TRUNK_GET(_gport)  \
    (!_SHR_GPORT_IS_TRUNK(_gport) ? BCM_TRUNK_INVALID : \
    _SHR_GPORT_TRUNK_GET(_gport)) 

#define BCM_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _qid)  \
         _SHR_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _qid) 

#define BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)  \
         _SHR_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid) 

#define BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)  \
            (!_SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport))
 

#define BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport)  \
            (!_SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport))
 

#define BCM_GPORT_DESTMOD_QUEUE_GROUP_SET(_gport, _qid)  \
         _SHR_GPORT_DESTMOD_QUEUE_GROUP_SET(_gport, _qid) 

#define BCM_GPORT_DESTMOD_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)  \
         _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid) 

#define BCM_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_GET(_gport)  \
            (!_SHR_GPORT_IS_DESTMOD_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_GET(_gport))
 

#define BCM_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(_gport)  \
            (!_SHR_GPORT_IS_DESTMOD_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(_gport))
 

#define BCM_GPORT_UNICAST_QUEUE_GROUP_SET(_gport, _qid)  \
         _SHR_GPORT_UNICAST_QUEUE_GROUP_SET(_gport, _qid) 

#define BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(_gport, _core, _qid)  \
         _SHR_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(_gport, _core, _qid) 

#define BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(_gport)  \
         (!_SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport) ? -1 : \
         _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_GET(_gport)) 

#define BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(_gport)  \
         (!_SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport) ? -1 : \
         _SHR_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(_gport))
 

#define BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(_gport, _qid)  \
         _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(_gport, _qid) 

#define BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(_gport)  \
            (!_SHR_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(_gport))
 

#define BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)  \
         _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid) 

#define BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(_gport)  \
            (!_SHR_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(_gport))
 

#define BCM_GPORT_MCAST_SET(_gport, _mcast_id)  \
        (_SHR_GPORT_MCAST_SET(_gport, _mcast_id))
 

#define BCM_GPORT_MCAST_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_GET(_gport))
 

#define BCM_GPORT_MCAST_DS_ID_SET(_gport, _ds_id)  \
     _SHR_GPORT_MCAST_DS_ID_SET(_gport,_ds_id)
 

#define BCM_GPORT_MCAST_DS_ID_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_DS_ID_GET(_gport))
 

#define BCM_GPORT_MCAST_QUEUE_GROUP_SET(_gport, _qid)  \
         _SHR_GPORT_MCAST_QUEUE_GROUP_SET(_gport, _qid)
 

#define BCM_GPORT_MCAST_QUEUE_GROUP_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_QUEUE_GROUP_GET(_gport))
 

#define BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)  \
     _SHR_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)
 

#define BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport))
 

#define BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)) 

/* for multicast queue gports representing queue ID + core ID */
#define BCM_GPORT_MCAST_QUEUE_GROUP_QUEUE_SET(_gport, _qid)  \
         _SHR_GPORT_MCAST_QUEUE_GROUP_QUEUE_SET(_gport, _qid) 
#define BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(_gport, _core, _qid)  \
         _SHR_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(_gport, _core, _qid) 
#define BCM_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(_gport)  \
         (!_SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) ? -1 : \
         _SHR_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(_gport)) 
#define BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(_gport)  \
         (!_SHR_GPORT_IS_MCAST_QUEUE_GROUP(_gport) ? -1 : \
         _SHR_GPORT_MCAST_QUEUE_GROUP_CORE_GET(_gport))
 

#define BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(_gport, _qid)  \
         _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(_gport, _qid) 

#define BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(_gport))
 

#define BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid)  \
         _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(_gport, _sysport_id, _qid) 

#define BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(_gport)  \
            (!_SHR_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(_gport) ? -1 :  \
        _SHR_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(_gport))
 

#define BCM_GPORT_SCHEDULER_SET(_gport, _scheduler_id)  \
    _SHR_GPORT_SCHEDULER_SET(_gport, _scheduler_id) 

#define BCM_GPORT_SCHEDULER_NODE_SET(_gport, _scheduler_level, _scheduler_id)  \
    _SHR_GPORT_SCHEDULER_NODE_SET(_gport, _scheduler_level,_scheduler_id) 

#define BCM_GPORT_SCHEDULER_GET(_gport)  \
    _SHR_GPORT_SCHEDULER_GET(_gport) 

#define BCM_GPORT_SCHEDULER_CORE_GET(_gport)  \
    _SHR_GPORT_SCHEDULER_CORE_GET(_gport) 

#define BCM_GPORT_SCHEDULER_CORE_SET(_gport, _scheduler_id, _core_id)  \
    _SHR_GPORT_SCHEDULER_CORE_SET(_gport, _scheduler_id, _core_id) 

#define BCM_GPORT_MPLS_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_MPLS_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_MPLS_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_MPLS_PORT(_gport) ? -1 :  \
    _SHR_GPORT_MPLS_PORT_ID_GET(_gport)) 

#define BCM_GPORT_VLAN_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_VLAN_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_VLAN_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_VLAN_PORT(_gport) ? -1 :  \
    _SHR_GPORT_VLAN_PORT_ID_GET(_gport)) 

#define BCM_GPORT_SUBPORT_GROUP_SET(_gport, _id)  \
    _SHR_GPORT_SUBPORT_GROUP_SET(_gport, _id) 

#define BCM_GPORT_SUBPORT_GROUP_GET(_gport)  \
    (!_SHR_GPORT_IS_SUBPORT_GROUP(_gport) ? -1 :  \
    _SHR_GPORT_SUBPORT_GROUP_GET(_gport)) 

#define BCM_GPORT_SUBPORT_PORT_SET(_gport, _id)  \
    _SHR_GPORT_SUBPORT_PORT_SET(_gport, _id) 

#define BCM_GPORT_SUBPORT_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_SUBPORT_PORT(_gport) ? -1 :  \
    _SHR_GPORT_SUBPORT_PORT_GET(_gport)) 

#define BCM_GPORT_CHILD_SET(_gport, _modid, _port)  \
    _SHR_GPORT_CHILD_SET(_gport, _modid, _port) 

#define BCM_GPORT_CHILD_MODID_GET(_gport)  \
    (!_SHR_GPORT_IS_CHILD(_gport) ? -1 :  \
    _SHR_GPORT_CHILD_MODID_GET(_gport)) 

#define BCM_GPORT_CHILD_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_CHILD(_gport) ? -1 :  \
    _SHR_GPORT_CHILD_PORT_GET(_gport)) 

#define BCM_GPORT_EGRESS_GROUP_SET(_egress_group_gport, _child_modid, _egress_child)  \
    _SHR_GPORT_EGRESS_GROUP_SET(_egress_group_gport, _child_modid, _egress_child) 

#define BCM_GPORT_EGRESS_GROUP_MODID_GET(_egress_group_gport)  \
    (!_SHR_GPORT_IS_EGRESS_GROUP(_egress_group_gport) ? -1 :  \
    _SHR_GPORT_EGRESS_GROUP_MODID_GET(_egress_group_gport)) 

#define BCM_GPORT_EGRESS_GROUP_GET(_egress_group_gport)  \
    (!_SHR_GPORT_IS_EGRESS_GROUP(_egress_group_gport) ? -1 :  \
    _SHR_GPORT_EGRESS_GROUP_GET(_egress_group_gport)) 

#define BCM_GPORT_EGRESS_CHILD_SET(_egress_child_gport, _child_modid, _child_port)  \
    _SHR_GPORT_EGRESS_CHILD_SET(_egress_child_gport, _child_modid, _child_port) 

#define BCM_GPORT_EGRESS_CHILD_MODID_GET(_egress_child_gport)  \
    (!_SHR_GPORT_IS_EGRESS_CHILD(_egress_child_gport) ? -1 :  \
    _SHR_GPORT_EGRESS_CHILD_MODID_GET(_egress_child_gport)) 

#define BCM_GPORT_EGRESS_CHILD_PORT_GET(_egress_child_gport)  \
    (!_SHR_GPORT_IS_EGRESS_CHILD(_egress_child_gport) ? -1 :  \
    _SHR_GPORT_EGRESS_CHILD_PORT_GET(_egress_child_gport)) 

#define BCM_GPORT_EGRESS_MODPORT_SET(_gport, _module, _port)  \
    _SHR_GPORT_EGRESS_MODPORT_SET(_gport, _module, _port) 

#define BCM_GPORT_EGRESS_MODPORT_MODID_GET(_gport)  \
    (!_SHR_GPORT_IS_EGRESS_MODPORT(_gport) ? -1 :  \
    _SHR_GPORT_EGRESS_MODPORT_MODID_GET(_gport)) 

#define BCM_GPORT_EGRESS_MODPORT_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_EGRESS_MODPORT(_gport) ? -1 :  \
    _SHR_GPORT_EGRESS_MODPORT_PORT_GET(_gport)) 

#define BCM_GPORT_CONGESTION_SET(_gport, _port)  \
    _SHR_GPORT_CONGESTION_SET(_gport, _port) 

#define BCM_GPORT_CONGESTION_OOB_SET(_gport, _port)  \
    _SHR_GPORT_CONGESTION_OOB_SET(_gport, _port) 

#define BCM_GPORT_CONGESTION_COE_SET(_gport, _port)  \
    _SHR_GPORT_CONGESTION_COE_SET(_gport, _port) 

#define BCM_GPORT_CONGESTION_GET(_gport)  \
    (!_SHR_GPORT_IS_CONGESTION(_gport) ? -1 : \
    _SHR_GPORT_CONGESTION_GET(_gport)) 

#define BCM_GPORT_NIF_RX_PRIORITY_LOW_SET(_gport, _port)  \
    _SHR_GPORT_NIF_RX_PRIORITY_LOW_SET(_gport, _port) 

#define BCM_GPORT_NIF_RX_PRIORITY_HIGH_SET(_gport, _port)  \
    _SHR_GPORT_NIF_RX_PRIORITY_HIGH_SET(_gport, _port) 

#define BCM_GPORT_NIF_RX_PRIORITY_TDM_SET(_gport, _port)  \
    _SHR_GPORT_NIF_RX_PRIORITY_TDM_SET(_gport, _port) 

#define BCM_GPORT_NIF_RX_PRIORITY_GET(_gport)  \
    (!_SHR_GPORT_IS_NIF_RX_PRIORITY(_gport) ? -1 : \
    _SHR_GPORT_NIF_RX_PRIORITY_GET(_gport)) 

#define BCM_GPORT_SYSTEM_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_SYSTEM_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_SYSTEM_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_SYSTEM_PORT(_gport) ? -1 :  \
    _SHR_GPORT_SYSTEM_PORT_ID_GET(_gport)) 

#define BCM_COSQ_GPORT_ISQ_ROOT_SET(_gport)  \
    _SHR_COSQ_GPORT_ISQ_ROOT_SET(_gport) 

#define BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_ISQ_ROOT_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_ROOT_SET(_gport)  \
    _SHR_COSQ_GPORT_FMQ_ROOT_SET(_gport) 

#define BCM_COSQ_GPORT_FMQ_ROOT_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_FMQ_ROOT_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_GUARANTEED_SET(_gport)  \
    _SHR_COSQ_GPORT_FMQ_GUARANTEED_SET(_gport) 

#define BCM_COSQ_GPORT_FMQ_GUARANTEED_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_FMQ_GUARANTEED_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT_AGR_SET(_gport)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT_AGR_SET(_gport) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT_AGR_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT_AGR_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT0_SET(_gport)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT0_SET(_gport) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT0_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT0_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT1_SET(_gport)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT1_SET(_gport) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT1_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT1_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT2_SET(_gport)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT2_SET(_gport) 

#define BCM_COSQ_GPORT_FMQ_BESTEFFORT2_CORE_SET(_gport, _core_id)  \
    _SHR_COSQ_GPORT_FMQ_BESTEFFORT2_CORE_SET(_gport, _core_id) 

#define BCM_COSQ_GPORT_FMQ_CLASS_SET(_gport, _class)  \
    _SHR_COSQ_GPORT_FMQ_CLASS_SET(_gport, _class) 

#define BCM_COSQ_GPORT_FMQ_CLASS_CORE_SET(_gport, _core_id, _class)  \
    _SHR_COSQ_GPORT_FMQ_CLASS_CORE_SET(_gport, _core_id, _class) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV_SET(_gport, _dev_id)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV_SET(_gport, _dev_id) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_LOCAL_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FABRIC_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FABRIC_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FABRIC_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FABRIC_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FABRIC_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FABRIC_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_BESTEFFORT_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_COMMON_LOCAL0_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_LOCAL0_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_COMMON_LOCAL1_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_LOCAL1_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_COMMON_DEV1_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_DEV1_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_COMMON_DEV2_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_DEV2_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_COMMON_DEV3_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_DEV3_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_COMMON_MC_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_COMMON_MC_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_COMMON_LOCAL0_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_LOCAL0_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_COMMON_LOCAL1_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_LOCAL1_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_COMMON_FABRIC_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_FABRIC_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_UNICAST_FABRIC_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_COMMON_MULTICAST_FABRIC_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV1_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV1_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV1_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV1_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV1_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV2_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV2_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV2_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV2_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV2_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV3_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV3_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV3_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_DEV3_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_DEV3_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_MC_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_MC_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_MC_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_MC_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_MC_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_MC_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_MC_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_MC_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL0_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL0_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_LOCAL1_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_LOCAL1_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_MC_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_MC_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_MESH_SCOPE_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_MESH_SCOPE_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_OCB_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_GUARANTEED_MIX_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL0_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL0_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_LOCAL1_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_LOCAL1_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_OCB_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_OCB_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_MIX_HIGH_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_MIX_HIGH_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_OCB_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_OCB_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_MIX_LOW_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_MIX_LOW_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_CLOS_SCOPE_SET(_gport)  \
    _SHR_COSQ_GPORT_FABRIC_CLOS_SCOPE_SET(_gport) 

#define BCM_COSQ_GPORT_COMPOSITE_SF2_SET(_gport_sf2, _gport)  \
    _SHR_COSQ_GPORT_COMPOSITE_SF2_SET(_gport_sf2, _gport) 

#define BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(_gport_sf2, _gport, _core_id)  \
    _SHR_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(_gport_sf2, _gport, _core_id) 

#define BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_COMPOSITE_SF2(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_COMPOSITE_SF2_CORE_GET(_gport)) 

#define BCM_COSQ_GPORT_COMPOSITE_SF2_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_COMPOSITE_SF2(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_COMPOSITE_SF2_GET(_gport)) 

#define BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(_gport, _port) 

#define BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_UCAST_EGRESS_QUEUE_GET(_gport)) 

#define BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(_gport, _port) 

#define BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_SET(_gport, _type, _type_info)  \
    _SHR_COSQ_GPORT_VSQ_SET(_gport, _type, _type_info) 

#define BCM_COSQ_GPORT_VSQ_TYPE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_VSQ(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_VSQ_FLAGS_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_VSQ(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_VSQ_TYPE_INFO_GET(_gport)) 

#define BCM_COSQ_VSQ_TYPE_INFO_GROUPA_SET(_type_info, _category)  \
    _SHR_COSQ_VSQ_TYPE_INFO_GROUPA_SET(_type_info, _category) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPA_CATEGORY_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPA_CATEGORY_GET(_type_info)) 

#define BCM_COSQ_VSQ_TYPE_INFO_GROUPB_SET(_type_info, _category, _tc)  \
    _SHR_COSQ_VSQ_TYPE_INFO_GROUPB_SET(_type_info, _category, _tc) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPB_CATEGORY_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPB_CATEGORY_GET(_type_info)) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPB_TC_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPB_TC_GET(_type_info)) 

#define BCM_COSQ_VSQ_TYPE_INFO_GROUPC_SET(_type_info, _category, _connection)  \
    _SHR_COSQ_VSQ_TYPE_INFO_GROUPC_SET(_type_info, _category, _connection) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPC_CATEGORY_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPC_CATEGORY_GET(_type_info)) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPC_CONNECTION_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPC_CONNECTION_GET(_type_info)) 

#define BCM_COSQ_VSQ_TYPE_INFO_GROUPD_SET(_type_info, _statistics_tag)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPD_SET(_type_info, _statistics_tag)) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPD_STATISTICS_TAG_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPD_STATISTICS_TAG_GET(_type_info)) 

#define BCM_COSQ_VSQ_TYPE_INFO_GROUPE_SET(_type_info, _src_port, _tc)  \
    _SHR_COSQ_VSQ_TYPE_INFO_GROUPE_SET(_type_info, _src_port, _tc) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPE_SRC_PORT_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPE_SRC_PORT_GET(_type_info)) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPE_TC_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPE_TC_GET(_type_info)) 

#define BCM_COSQ_VSQ_TYPE_INFO_GROUPF_SET(_type_info, _src_port)  \
    _SHR_COSQ_VSQ_TYPE_INFO_GROUPF_SET(_type_info, _src_port) 

#define BCM_COSQ_GPORT_VSQ_TYPE_INFO_GROUPF_SRC_PORT_GET(_type_info)  \
    (_SHR_COSQ_VSQ_TYPE_INFO_GROUPF_SRC_PORT_GET(_type_info)) 

#define BCM_COSQ_GPORT_VSQ_GL_SET(_gport, _core_id)  \
    (_SHR_COSQ_GPORT_VSQ_GL_SET(_gport, _core_id)) 

#define BCM_COSQ_GPORT_IS_VSQ_GL(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_GL(_gport)) 

#define BCM_COSQ_GPORT_VSQ_CT_SET(_gport, _core_id, _is_ocb_only, _category)  \
    (_SHR_COSQ_GPORT_VSQ_CT_SET(_gport, _core_id, _is_ocb_only, _category)) 

#define BCM_COSQ_GPORT_IS_VSQ_CT(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_CT(_gport)) 

#define BCM_COSQ_GPORT_VSQ_CTTC_SET(_gport, _core_id, _is_ocb_only, _category, _traffic_class)  \
    (_SHR_COSQ_GPORT_VSQ_CTTC_SET(_gport, _core_id, _is_ocb_only, _category, _traffic_class)) 

#define BCM_COSQ_GPORT_IS_VSQ_CTTC(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_CTTC(_gport)) 

#define BCM_COSQ_GPORT_VSQ_CTCC_SET(_gport, _core_id, _is_ocb_only, _category, _connection_class)  \
    (_SHR_COSQ_GPORT_VSQ_CTCC_SET(_gport, _core_id, _is_ocb_only, _category, _connection_class)) 

#define BCM_COSQ_GPORT_IS_VSQ_CTCC(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_CTCC(_gport)) 

#define BCM_COSQ_GPORT_VSQ_PP_SET(_gport, _core_id, _is_ocb_only, _statistics_tag)  \
    (_SHR_COSQ_GPORT_VSQ_PP_SET(_gport, _core_id, _is_ocb_only, _statistics_tag)) 

#define BCM_COSQ_GPORT_IS_VSQ_PP(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_PP(_gport)) 

#define BCM_COSQ_GPORT_VSQ_SRC_PORT_SET(_gport, _core_id, _is_ocb_only, _src_port_vsq_id)  \
    (_SHR_COSQ_GPORT_VSQ_SRC_PORT_SET(_gport, _core_id, _is_ocb_only, _src_port_vsq_id)) 

#define BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_SRC_PORT(_gport)) 

#define BCM_COSQ_GPORT_VSQ_PG_SET(_gport, _core_id, _pg_vsq_id)  \
    (_SHR_COSQ_GPORT_VSQ_PG_SET(_gport, _core_id, _pg_vsq_id)) 

#define BCM_COSQ_GPORT_IS_VSQ_PG(_gport)  \
    (_SHR_COSQ_GPORT_IS_VSQ_PG(_gport)) 

#define BCM_COSQ_GPORT_VSQ_CT_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_CT_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_TC_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_TC_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_CC_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_CC_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_PP_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_PP_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_CORE_ID_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_CORE_ID_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_IS_OCB_ONLY(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_IS_OCB_ONLY(_gport)) 

#define BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_SRC_PORT_GET(_gport)) 

#define BCM_COSQ_GPORT_VSQ_PG_GET(_gport)  \
    (_SHR_COSQ_GPORT_VSQ_PG_GET(_gport)) 

#define BCM_COSQ_GPORT_E2E_PORT_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_E2E_PORT_SET(_gport, _port) 

#define BCM_COSQ_GPORT_E2E_PORT_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_E2E_PORT(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_E2E_PORT_GET(_gport)) 

#define BCM_COSQ_GPORT_ISQ_SET(_gport, _qid)  \
    _SHR_COSQ_GPORT_ISQ_SET(_gport, _qid) 

#define BCM_COSQ_GPORT_ISQ_CORE_QUEUE_SET(_gport, _core, _qid)  \
         _SHR_COSQ_GPORT_ISQ_CORE_QUEUE_SET(_gport, _core, _qid) 

#define BCM_COSQ_GPORT_ISQ_QID_GET(_gport)  \
         (!_SHR_COSQ_GPORT_IS_ISQ(_gport) ? -1 :  \
         _SHR_COSQ_GPORT_ISQ_QID_GET(_gport)) 

#define BCM_COSQ_GPORT_ISQ_CORE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_ISQ(_gport) ? -1 : \
    _SHR_COSQ_GPORT_ISQ_CORE_GET(_gport)) 

#define BCM_COSQ_GPORT_VOQ_CONNECTOR_SET(_gport, _cid)  \
    _SHR_COSQ_GPORT_VOQ_CONNECTOR_SET(_gport, _cid) 

#define BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(_gport, _cid, _core_id)  \
    _SHR_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(_gport, _cid, _core_id) 

#define BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(_gport) 

#define BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_VOQ_CONNECTOR(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(_gport)) 

#define BCM_COSQ_GPORT_SRC_QUEUE_SET(_gport, _qid)  \
    _SHR_COSQ_GPORT_SRC_QUEUE_SET(_gport, _qid) 

#define BCM_COSQ_GPORT_SRC_QUEUE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_SRC_QUEUE(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_SRC_QUEUE_GET(_gport)) 

#define BCM_COSQ_GPORT_DST_QUEUE_SET(_gport, _qid)  \
    _SHR_COSQ_GPORT_DST_QUEUE_SET(_gport, _qid) 

#define BCM_COSQ_GPORT_DST_QUEUE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_DST_QUEUE(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_DST_QUEUE_GET(_gport)) 

#define BCM_COSQ_GPORT_MULTIPATH_SET(_gport, _id)  \
    _SHR_COSQ_GPORT_MULTIPATH_SET(_gport, _id) 

#define BCM_COSQ_GPORT_MULTIPATH_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_MULTIPATH(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_MULTIPATH_GET(_gport)) 

#define BCM_COSQ_GPORT_SCHED_CIR_SET(_gport_cir, _gport)  \
    _SHR_COSQ_GPORT_SCHED_CIR_SET(_gport_cir, _gport) 

#define BCM_COSQ_GPORT_SCHED_CIR_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_SCHED_CIR(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_SCHED_CIR_GET(_gport)) 

#define BCM_COSQ_GPORT_SCHED_CIR_CORE_SET(_gport, _cid, _core_id)  \
    _SHR_COSQ_GPORT_SCHED_CIR_CORE_SET(_gport, _cid, _core_id) 

#define BCM_COSQ_GPORT_SCHED_CIR_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_SCHED_CIR_CORE_GET(_gport) 

#define BCM_COSQ_GPORT_SCHED_PIR_SET(_gport_pir, _gport)  \
    _SHR_COSQ_GPORT_SCHED_PIR_SET(_gport_pir, _gport) 

#define BCM_COSQ_GPORT_SCHED_PIR_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_SCHED_PIR(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_SCHED_PIR_GET(_gport)) 

#define BCM_COSQ_GPORT_SCHED_PIR_CORE_SET(_gport, _cid, _core_id)  \
    _SHR_COSQ_GPORT_SCHED_PIR_CORE_SET(_gport, _cid, _core_id) 

#define BCM_COSQ_GPORT_SCHED_PIR_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_SCHED_PIR_CORE_GET(_gport) 

#define BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_SET(_gport, _cid, _core_id)  \
    _SHR_COSQ_GPORT_SHARED_SHAPER_ELEM_SET(_gport, _cid, _core_id) 

#define BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_FLOW_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_SHARED_SHAPER_ELEM_FLOW_GET(_gport)) 

#define BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_CORE_GET(_gport)  \
    _SHR_COSQ_GPORT_SHARED_SHAPER_ELEM_CORE_GET(_gport) 

#define BCM_COSQ_GPORT_PORT_TC_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_PORT_TC_SET(_gport, _port) 

#define BCM_COSQ_GPORT_PORT_TC_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_PORT_TC(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_PORT_TC_GET(_gport)) 

#define BCM_COSQ_GPORT_PORT_TCG_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_PORT_TCG_SET(_gport, _port) 

#define BCM_COSQ_GPORT_PORT_TCG_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_PORT_TCG(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_PORT_TCG_GET(_gport)) 

#define BCM_COSQ_GPORT_E2E_PORT_TC_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_E2E_PORT_TC_SET(_gport, _port) 

#define BCM_COSQ_GPORT_E2E_PORT_TC_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_E2E_PORT_TC(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_E2E_PORT_TC_GET(_gport)) 

#define BCM_COSQ_GPORT_E2E_PORT_TCG_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_E2E_PORT_TCG_SET(_gport, _port) 

#define BCM_COSQ_GPORT_E2E_PORT_TCG_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_E2E_PORT_TCG(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_E2E_PORT_TCG_GET(_gport)) 

#define BCM_COSQ_GPORT_SYSTEM_RED_SET(_gport)  \
    _SHR_COSQ_GPORT_SYSTEM_RED_SET(_gport) 

#define BCM_COSQ_GPORT_FABRIC_PIPE_SET(_gport, _fabric_port, _fabric_pipe)  \
    _SHR_COSQ_GPORT_FABRIC_PIPE_SET(_gport, _fabric_port, _fabric_pipe) 

#define BCM_COSQ_GPORT_FABRIC_PIPE_INGRESS_SET(_gport, _fabric_port, _fabric_pipe)  \
    _SHR_COSQ_GPORT_FABRIC_PIPE_INGRESS_SET(_gport, _fabric_port, _fabric_pipe) 

#define BCM_COSQ_GPORT_FABRIC_PIPE_EGRESS_SET(_gport, _fabric_port, _fabric_pipe)  \
    _SHR_COSQ_GPORT_FABRIC_PIPE_EGRESS_SET(_gport, _fabric_port, _fabric_pipe) 

#define BCM_COSQ_GPORT_FABRIC_PIPE_MIDDLE_SET(_gport, _fabric_port, _fabric_pipe)  \
    _SHR_COSQ_GPORT_FABRIC_PIPE_MIDDLE_SET(_gport, _fabric_port, _fabric_pipe) 

#define BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(_gport)  \
    ((!_SHR_COSQ_GPORT_IS_FABRIC_PIPE(_gport) &&  !_SHR_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(_gport) && \
      !_SHR_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(_gport) && !_SHR_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(_gport)) ? -1 : _SHR_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(_gport)) 

#define BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(_gport)  \
    ((!_SHR_COSQ_GPORT_IS_FABRIC_PIPE(_gport) &&  !_SHR_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(_gport) && \
      !_SHR_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(_gport) && !_SHR_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(_gport)) ? -1 : _SHR_COSQ_GPORT_FABRIC_PIPE_PORT_GET(_gport)) 

/* Multicast distribution set */
typedef int bcm_fabric_distribution_t;

/* Failover Object */
typedef int bcm_failover_t;

#define BCM_GPORT_DEVPORT(_device, _port)  \
    _SHR_GPORT_DEVPORT(_device, _port) 

#define BCM_GPORT_DEVPORT_SET(_gport, _device, _port)  \
    _SHR_GPORT_DEVPORT_SET(_gport, _device, _port) 

#define BCM_GPORT_DEVPORT_DEVID_GET(_gport)  \
    (!_SHR_GPORT_IS_DEVPORT(_gport) ? -1 : \
    _SHR_GPORT_DEVPORT_DEVID_GET(_gport)) 

#define BCM_GPORT_DEVPORT_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_DEVPORT(_gport) ? -1 : \
    _SHR_GPORT_DEVPORT_PORT_GET(_gport)) 

#define BCM_GPORT_LOCAL_CPU     _SHR_GPORT_LOCAL_CPU 

#define BCM_GPORT_LOCAL_CPU_IEEE    _SHR_GPORT_LOCAL_CPU_IEEE 

#define BCM_GPORT_LOCAL_CPU_HIGIG   _SHR_GPORT_LOCAL_CPU_HIGIG 

#define BCM_GPORT_BLACK_HOLE    _SHR_GPORT_BLACK_HOLE 

#define BCM_GPORT_SPECIAL(_value)  \
    _SHR_GPORT_SPECIAL(_value) 

#define BCM_GPORT_SPECIAL_SET(_gport, _value)  \
    _SHR_GPORT_SPECIAL_SET(_gport, _value) 

#define BCM_GPORT_SPECIAL_GET(_gport)  \
    (!_SHR_GPORT_IS_SPECIAL(_gport) ? -1 : \
    _SHR_GPORT_SPECIAL_GET(_gport)) 

#define BCM_GPORT_MIRROR_MIRROR_SET(_gport, _value)  \
     BCM_GPORT_MIRROR_SET(_gport, _value)  

#define BCM_GPORT_MIRROR_SNOOP_SET(_gport, _value)  \
     _SHR_GPORT_MIRROR_SNOOP_SET(_gport, _value)  

#define BCM_GPORT_IS_MIRROR_MIRROR(_gport)  \
     _SHR_GPORT_IS_MIRROR_MIRROR(_gport)  

#define BCM_GPORT_IS_MIRROR_SNOOP(_gport)  \
     _SHR_GPORT_IS_MIRROR_SNOOP(_gport)  

#define BCM_GPORT_IS_MIRROR_STAT_SAMPLE(_gport)  \
     _SHR_GPORT_IS_MIRROR_STAT_SAMPLE(_gport)  

#define BCM_GPORT_MIRROR_SUBTYPE_VAL_MIRROR _SHR_GPORT_MIRROR_SUBTYPE_VAL_MIRROR 
#define BCM_GPORT_MIRROR_SUBTYPE_VAL_SNOOP  _SHR_GPORT_MIRROR_SUBTYPE_VAL_SNOOP 

#define BCM_GPORT_MIRROR_FULL_SET(_gport, _value, _mir_subtype)  \
      _SHR_GPORT_MIRROR_FULL_SET(_gport, _value, _mir_subtype)  

#define BCM_GPORT_MIRROR_SUBTYPE_GET(_gport)  \
       _SHR_GPORT_MIRROR_SUBTYPE_GET(_gport)  

#define BCM_GPORT_MIRROR_SET(_gport, _value)  \
    _SHR_GPORT_MIRROR_SET(_gport, _value) 

#define BCM_GPORT_MIRROR_GET(_gport)  \
    (!_SHR_GPORT_IS_MIRROR(_gport) ? -1 : \
    _SHR_GPORT_MIRROR_GET(_gport)) 

#define BCM_GPORT_MIRROR_STAT_SAMPLE_SET(_gport, _value)  \
    _SHR_GPORT_MIRROR_STAT_SAMPLE_SET(_gport, _value) 

#define BCM_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_SET(whole_utilization, fractional_utilization)  \
    _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_SET(whole_utilization, fractional_utilization) 

#define BCM_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_WHOLE_GET(utilization)  \
    _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_WHOLE_GET(utilization) 

#define BCM_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_FRACTIONAL_GET(utilization)  \
    _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_FRACTIONAL_GET(utilization) 

#define BCM_GPORT_MIM_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_MIM_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_MIM_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_MIM_PORT(_gport) ? -1 :  \
    _SHR_GPORT_MIM_PORT_ID_GET(_gport)) 

#define BCM_GPORT_WLAN_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_WLAN_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_WLAN_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_WLAN_PORT(_gport) ? -1 :  \
    _SHR_GPORT_WLAN_PORT_ID_GET(_gport)) 

#define BCM_GPORT_TRILL_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_TRILL_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_EPON_LINK_SET(_gport, _num)  \
    _SHR_GPORT_EPON_LINK_SET(_gport, _num) 

#define BCM_GPORT_EPON_LINK_GET(_gport)  \
    (!_SHR_GPORT_IS_EPON_LINK(_gport) ? -1 :  \
    _SHR_GPORT_EPON_LINK_GET(_gport)) 

#define BCM_GPORT_TRILL_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_TRILL_PORT(_gport) ? -1 :  \
    _SHR_GPORT_TRILL_PORT_ID_GET(_gport)) 

#define BCM_GPORT_NIV_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_NIV_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_NIV_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_NIV_PORT(_gport) ? -1 :  \
    _SHR_GPORT_NIV_PORT_ID_GET(_gport)) 

#define BCM_GPORT_L2GRE_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_L2GRE_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_L2GRE_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_L2GRE_PORT(_gport) ? -1 :  \
    _SHR_GPORT_L2GRE_PORT_ID_GET(_gport)) 

#define BCM_GPORT_VXLAN_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_VXLAN_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_VXLAN_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_VXLAN_PORT(_gport) ? -1 :  \
    _SHR_GPORT_VXLAN_PORT_ID_GET(_gport)) 

#define BCM_GPORT_FLOW_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_FLOW_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_FLOW_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_FLOW_PORT(_gport) ? -1 :  \
    _SHR_GPORT_FLOW_PORT_ID_GET(_gport)) 

#define BCM_EXTENDER_GPORT_PORT_SET(_gport, _id)  \
    _SHR_EXTENDER_GPORT_PORT_SET(_gport, _id) 

#define BCM_EXTENDER_GPORT_FORWARD_SET(_gport, _id)  \
    _SHR_EXTENDER_GPORT_FORWARD_SET(_gport, _id) 

#define BCM_EXTENDER_GPORT_ENCAP_SET(_gport, _id)  \
    _SHR_EXTENDER_GPORT_ENCAP_SET(_gport, _id) 

#define BCM_GPORT_EXTENDER_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_EXTENDER_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_EXTENDER_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_EXTENDER_PORT(_gport) ? -1 :  \
    _SHR_GPORT_EXTENDER_PORT_ID_GET(_gport)) 

#define BCM_GPORT_MAC_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_MAC_PORT_ID_SET(_gport, _id) 

#define BCM_GPORT_MAC_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_MAC_PORT(_gport) ? -1 :  \
    _SHR_GPORT_MAC_PORT_ID_GET(_gport)) 

#define BCM_GPORT_TUNNEL_ID_SET(_gport, _tunnel_id)  \
        (_SHR_GPORT_TUNNEL_ID_SET(_gport, _tunnel_id))
 

#define BCM_GPORT_TUNNEL_ID_GET(_gport)  \
            (!_SHR_GPORT_IS_TUNNEL(_gport) ? -1 :  \
        _SHR_GPORT_TUNNEL_ID_GET(_gport))
 

#define BCM_PHY_GPORT_LANE_PORT_SET(_gport, _phy_lane, _port)  \
        (_SHR_PHY_GPORT_LANE_PORT_SET(_gport, _phy_lane, _port)) 

#define BCM_PHY_GPORT_LANE_PORT_LANE_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_LANE_PORT_LANE_GET(_gport))
 

#define BCM_PHY_GPORT_LANE_PORT_PORT_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_LANE_PORT_PORT_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_PORT_SET(_gport, _phyn, _port)  \
        (_SHR_PHY_GPORT_PHYN_PORT_SET(_gport, _phyn, _port)) 

#define BCM_PHY_GPORT_PHYN_PORT_PHYN_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_PORT_PHYN_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_PORT_PORT_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_PORT_PORT_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_LANE_PORT_SET(_gport, _phyn, _lane, _port)  \
        (_SHR_PHY_GPORT_PHYN_LANE_PORT_SET(_gport, _phyn, _lane, _port)) 

#define BCM_PHY_GPORT_PHYN_LANE_PORT_PHYN_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_LANE_PORT_PHYN_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_LANE_PORT_LANE_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_LANE_PORT_LANE_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_LANE_PORT_PORT_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_LANE_PORT_PORT_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_SET(_gport, _phyn, _port)  \
        (_SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_SET(_gport, _phyn, _port)) 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_SYS_SIDE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PHYN_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_SYS_SIDE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_SET(_gport, _phyn, _lane, _port)  \
        (_SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_SET(_gport, _phyn, _lane, _port)) 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PHYN_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_LANE_GET(_gport))
 

#define BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_GET(_gport)  \
            (!_SHR_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(_gport) ? -1 :  \
        _SHR_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_GET(_gport))
 

#define BCM_GPORT_PROXY_SET(_gport, _module, _port)  \
    _SHR_GPORT_PROXY_SET(_gport, _module, _port) 

#define BCM_GPORT_PROXY_MODID_GET(_gport)  \
    (!_SHR_GPORT_IS_PROXY(_gport) ? -1 : \
    _SHR_GPORT_PROXY_MODID_GET(_gport)) 

#define BCM_GPORT_PROXY_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_PROXY(_gport) ? -1 : \
    _SHR_GPORT_PROXY_PORT_GET(_gport)) 

#define BCM_GPORT_FORWARD_PORT_GET(_gport)  \
    (!_SHR_GPORT_IS_FORWARD_PORT(_gport) ? -1 : \
    _SHR_GPORT_FORWARD_PORT_GET(_gport)) 

#define BCM_GPORT_FORWARD_PORT_SET(_gport, _id)  \
    _SHR_GPORT_FORWARD_PORT_SET(_gport, _id) 

#define BCM_GPORT_VPLAG_PORT_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_VPLAG_PORT(_gport) ? -1 : \
    _SHR_GPORT_VPLAG_PORT_ID_GET(_gport)) 

#define BCM_GPORT_VPLAG_PORT_ID_SET(_gport, _id)  \
    _SHR_GPORT_VPLAG_PORT_ID_SET(_gport, _id) 

#define BCM_STG_DEFAULT         ((bcm_stg_t) 1) 
#define BCM_STG_INVALID         ((bcm_stg_t) -1)
 
#define BCM_CORE_ALL            _SHR_CORE_ALL 

/* bcm_stg_t */
typedef int bcm_stg_t;

/* bcm_dma_chan_t */
typedef _shr_dma_chan_t bcm_dma_chan_t;

/* IPFIX flow rate meter identifier */
typedef int bcm_ipfix_rate_id_t;

/* bcm_mod_port_t */
typedef struct bcm_mod_port_s {
    bcm_module_t mod; 
    bcm_port_t port; 
} bcm_mod_port_t;

/* bcm_color_t */
typedef enum bcm_color_e {
    bcmColorGreen = _SHR_COLOR_GREEN, 
    bcmColorYellow = _SHR_COLOR_YELLOW, 
    bcmColorRed = _SHR_COLOR_RED, 
    bcmColorDropFirst = bcmColorRed, 
    bcmColorBlack = _SHR_COLOR_BLACK, 
    bcmColorPreserve = _SHR_COLOR_PRESERVE, 
    bcmColorCount = _SHR_COLOR_COUNT 
} bcm_color_t;

typedef struct bcm_priority_mapping_s {
    int internal_pri;           /* Internal priority */
    bcm_color_t color;          /* (internal) color or drop precedence */
    int remark_internal_pri;    /* (internal) remarking priority */
    bcm_color_t remark_color;   /* (internal) remarking color or drop precedence */
    int policer_offset;         /* Offset based on pri/cos to fetch a policer */
} bcm_priority_mapping_t;

/* Flow Logical Field. */
typedef struct bcm_flow_logical_field_s {
    uint32 id;      /* logical field id. */
    uint32 value;   /* logical field value. */
} bcm_flow_logical_field_t;

#define BCM_FLOW_MAX_NOF_LOGICAL_FIELDS 20         

#if defined(LE_HOST)
#define bcm_htonl(_l)           _shr_swap32(_l) 
#define bcm_htons(_s)           _shr_swap16(_s) 
#define bcm_ntohl(_l)           _shr_swap32(_l) 
#define bcm_ntohs(_s)           _shr_swap16(_s) 
#else
#define bcm_htonl(_l)           (_l)       
#define bcm_htons(_s)           (_s)       
#define bcm_ntohl(_l)           (_l)       
#define bcm_ntohs(_s)           (_s)       
#endif

/* Base time structure. */
typedef struct bcm_time_spec_s {
    uint8 isnegative;   /* Sign identifier. */
    uint64 seconds;     /* Seconds absolute value. */
    uint32 nanoseconds; /* Nanoseconds absolute value. */
} bcm_time_spec_t;

/* bcm_unit_bmp_t */
typedef uint32 bcm_unit_bmp_t[_SHR_BITDCLSIZE(BCM_UNITS_MAX)];

#define BCM_UNIT_BMP_CLEAR(_bmp)  \
    sal_memset(_bmp, 0, \
    SHR_BITALLOCSIZE(BCM_UNITS_MAX)) 

#define BCM_UNIT_BMP_ADD(_bmp, _u)  SHR_BITSET(_bmp, _u) 
#define BCM_UNIT_BMP_REMOVE(_bmp, _u)  SHR_BITCLR(_bmp, _u) 
#define BCM_UNIT_BMP_TEST(_bmp, _u)  SHR_BITGET(_bmp, _u) 

/* bcm_unit_valid */
extern int bcm_unit_valid(
    int unit);

/* bcm_unit_local */
extern int bcm_unit_local(
    int unit);

/* bcm_unit_max */
extern int bcm_unit_max(
    void );

/* bcm_unit_remote */
extern int bcm_unit_remote(
    int unit);

/* bcm_unit_remote_unit_get */
extern int bcm_unit_remote_unit_get(
    int unit, 
    int *remunit);

/* bcm_unit_subtype_get */
extern int bcm_unit_subtype_get(
    int unit, 
    char *subtype, 
    int maxlen);

/* Creates IPv6 mask. */
extern int bcm_ip6_mask_create(
    bcm_ip6_t ip6, 
    int len);

/* Calculates bit length of the IPv6 network address mask. */
extern int bcm_ip6_mask_length(
    bcm_ip6_t mask);

/* Creates IPv4 mask for a specific prefix length. */
extern bcm_ip_t bcm_ip_mask_create(
    int len);

/* Calculates bit length of the IPv4 network address mask. */
extern int bcm_ip_mask_length(
    bcm_ip_t ip);

#define BCM_IP4_MASKLEN_TO_ADDR(_ml_)  \
        ((_ml_) ? (~((0x1 << (32 - (_ml_))) - 1)) : 0) 

#define BCM_TTL_VALID(_ttl_)    \
        (((_ttl_) >= 0) && ((_ttl_) <= 0xff)) 

#define BCM_PRIORITY_VALID(_pri_)  \
        (((_pri_) >= 0) && ((_pri_) <= 0x7)) 

#define BCM_RPE_PRIORITY_VALID(_pri_)  \
        (((_pri_) >= 0) && ((_pri_) <= 0xf)) 

#define BCM_FOREACH_LOCAL_UNIT(_unit)  \
    for (_unit = 0; _unit < bcm_unit_max(); _unit++) \
        if (bcm_unit_local(_unit)) 

#define BCM_FOREACH_UNIT(_unit)  \
    for (_unit = 0; _unit < bcm_unit_max(); _unit++) \
        if (bcm_unit_valid(_unit)) 

/* VLAN TPID Action definitions. */
typedef enum bcm_vlan_tpid_action_e {
    bcmVlanTpidActionNone = 0,      /* Do not modify. */
    bcmVlanTpidActionModify = 1,    /* Set to given value. */
    bcmVlanTpidActionInner = 2,     /* Use packet's inner tpid. */
    bcmVlanTpidActionOuter = 3      /* Use packet's outer tpid. */
} bcm_vlan_tpid_action_t;

/* VLAN Pcp Action definitions. */
typedef enum bcm_vlan_pcp_action_e {
    bcmVlanPcpActionNone = 0    /* Do not modify. */
} bcm_vlan_pcp_action_t;

/* VLAN Action definitions. */
typedef enum bcm_vlan_action_e {
    bcmVlanActionNone = 0,              /* Do not modify. */
    bcmVlanActionAdd = 1,               /* Add VLAN tag. */
    bcmVlanActionReplace = 2,           /* Replace VLAN tag. */
    bcmVlanActionDelete = 3,            /* Delete VLAN tag. */
    bcmVlanActionCopy = 4,              /* Copy VLAN tag. */
    bcmVlanActionCompressed = 5,        /* Set VLAN compress tag. */
    bcmVlanActionMappedAdd = 6,         /* Add a new VLAN tag according to
                                           Mapped VLAN. */
    bcmVlanActionMappedReplace = 7,     /* Replace existing VLAN tag according
                                           to Mapped VLAN. */
    bcmVlanActionOuterAdd = 8,          /* Add a new VLAN tag with the Outer
                                           VLAN tag value. */
    bcmVlanActionInnerAdd = 9,          /* Add a new VLAN tag with the Inner
                                           VLAN tag value. */
    bcmVlanActionCopyVsan = 10,         /* Copy VSAN and Pri. */
    bcmVlanActionReplaceVsan = 11,      /* Replace VSAN and Pri. */
    bcmVlanActionArpVlanTranslateAdd = 12 /* ARP+AC Add VLAN tag. */
} bcm_vlan_action_t;

/* TSN priority map id */
typedef int bcm_tsn_pri_map_t;

/* Initialize a VLAN tag action set structure. */
typedef struct bcm_vlan_action_set_s {
    bcm_vlan_t new_outer_vlan;          /* New outer VLAN for Add/Replace
                                           actions. */
    bcm_vlan_t new_inner_vlan;          /* New inner VLAN for Add/Replace
                                           actions. */
    uint8 new_inner_pkt_prio;           /* New inner packet priority for
                                           Add/Replace actions. */
    uint8 new_outer_cfi;                /* New outer packet CFI for Add/Replace
                                           actions. */
    uint8 new_inner_cfi;                /* New inner packet CFI for Add/Replace
                                           actions. */
    bcm_if_t ingress_if;                /* L3 Ingress Interface. */
    int priority;                       /* Internal or packet priority. */
    bcm_vlan_action_t dt_outer;         /* Outer-tag action for double-tagged
                                           packets. */
    bcm_vlan_action_t dt_outer_prio;    /* Outer-priority-tag action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_outer_pkt_prio; /* Outer packet priority action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_outer_cfi;     /* Outer packet CFI action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_inner;         /* Inner-tag action for double-tagged
                                           packets. */
    bcm_vlan_action_t dt_inner_prio;    /* Inner-priority-tag action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_inner_pkt_prio; /* Inner packet priority action for
                                           double-tagged packets. */
    bcm_vlan_action_t dt_inner_cfi;     /* Inner packet CFI action for
                                           double-tagged packets. */
    bcm_vlan_action_t ot_outer;         /* Outer-tag action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_prio;    /* Outer-priority-tag action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_pkt_prio; /* Outer packet priority action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_cfi;     /* Outer packet CFI action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner;         /* Inner-tag action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner_pkt_prio; /* Inner packet priority action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner_cfi;     /* Inner packet CFI action for
                                           single-outer-tagged packets. */
    bcm_vlan_action_t it_outer;         /* Outer-tag action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_outer_pkt_prio; /* Outer packet priority action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_outer_cfi;     /* Outer packet CFI action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner;         /* Inner-tag action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_prio;    /* Inner-priority-tag action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_pkt_prio; /* Inner packet priority action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_cfi;     /* Inner packet CFI action for
                                           single-inner-tagged packets. */
    bcm_vlan_action_t ut_outer;         /* Outer-tag action for untagged
                                           packets. */
    bcm_vlan_action_t ut_outer_pkt_prio; /* Outer packet priority action for
                                           untagged packets. */
    bcm_vlan_action_t ut_outer_cfi;     /* Outer packet CFI action for untagged
                                           packets. */
    bcm_vlan_action_t ut_inner;         /* Inner-tag action for untagged
                                           packets. */
    bcm_vlan_action_t ut_inner_pkt_prio; /* Inner packet priority action for
                                           untagged packets. */
    bcm_vlan_action_t ut_inner_cfi;     /* Inner packet CFI action for untagged
                                           packets. */
    bcm_vlan_pcp_action_t outer_pcp;    /* Outer tag's pcp field action of
                                           outgoing packets. */
    bcm_vlan_pcp_action_t inner_pcp;    /* Inner tag's pcp field action of
                                           outgoing packets. */
    bcm_policer_t policer_id;           /* Base policer to be used */
    uint16 outer_tpid;                  /* New outer-tag's tpid field for modify
                                           action */
    uint16 inner_tpid;                  /* New inner-tag's tpid field for modify
                                           action */
    bcm_vlan_tpid_action_t outer_tpid_action; /* Action of outer-tag's tpid field */
    bcm_vlan_tpid_action_t inner_tpid_action; /* Action of inner-tag's tpid field */
    int action_id;                      /* Action Set index */
    uint32 class_id;                    /* Class ID */
    bcm_tsn_pri_map_t taf_gate_primap;  /* TAF (Time Aware Filtering) gate
                                           priority mapping */
    uint32 flags;                       /* BCM_VLAN_ACTION_SET_xxx. */
    bcm_vlan_action_t outer_tag;        /* Outer-tag action for all tagged
                                           packets. */
    bcm_vlan_action_t inner_tag;        /* Inner_tag action for all tagged
                                           packets. */
    bcm_vlan_t forwarding_domain;       /* Forwarding domain (VLAN or VPN). */
    int inner_qos_map_id;               /* Map ID of inner QOS profile */
    int outer_qos_map_id;               /* Map ID of outer QOS profile. */
    uint32 opaque_ctrl_id;              /* Opaque control ID. */
} bcm_vlan_action_set_t;

/* bcm_fcoe_vsan_id_t */
typedef uint32 bcm_fcoe_vsan_id_t;

/* bcm_service_pool_id_t */
typedef int bcm_service_pool_id_t;

/* bcm_oob_fc_rx_intf_id_t */
typedef int bcm_oob_fc_rx_intf_id_t;

/* bcm_pstats_session_id_t */
typedef int bcm_pstats_session_id_t;

/* UDF Id */
typedef int bcm_udf_id_t;

/* Packet format ID */
typedef int bcm_udf_pkt_format_id_t;

/* TSN flow set */
typedef int bcm_tsn_flowset_t;

/* SR flow set */
typedef int bcm_tsn_sr_flowset_t;

/* Packet Format: L2 */
#define BCM_PKT_FORMAT_L2_ANY       (0x0)      /* All packet types */
#define BCM_PKT_FORMAT_L2_ETH_II    (1 << 0)   /* L2 Eth-II packet */
#define BCM_PKT_FORMAT_L2_SNAP      (1 << 1)   /* L2 SNAP packet */
#define BCM_PKT_FORMAT_L2_LLC       (1 << 2)   /* L2 LLC packet */

/* Packet Format: Vlan Tag */
#define BCM_PKT_FORMAT_VLAN_TAG_ANY     (0x0)      /* Any packet */
#define BCM_PKT_FORMAT_VLAN_TAG_NONE    (1 << 0)   /* Vlan untagged packet */
#define BCM_PKT_FORMAT_VLAN_TAG_SINGLE  (1 << 1)   /* Vlan single tagged packet */
#define BCM_PKT_FORMAT_VLAN_TAG_DOUBLE  (1 << 2)   /* Vlan double tagged packet */

/* Packet Format: IP */
#define BCM_PKT_FORMAT_IP_ANY           (0x0)      /* Any packet */
#define BCM_PKT_FORMAT_IP_NONE          (1 << 0)   /* NON IP packets */
#define BCM_PKT_FORMAT_IP4              (1 << 1)   /* IPv4 packets */
#define BCM_PKT_FORMAT_IP6              (1 << 2)   /* IPv6 packets */
#define BCM_PKT_FORMAT_IP4_WITH_OPTIONS (1 << 3)   /* IPv4 packets with IP
                                                      options */
#define BCM_PKT_FORMAT_IP6_WITH_OPTIONS (1 << 4)   /* IPv6 packets with IP
                                                      options */

/* Packet Format: MPLS */
#define BCM_PKT_FORMAT_MPLS_ANY             (0x0)      /* Any packet */
#define BCM_PKT_FORMAT_MPLS_ONE_LABEL       (1 << 0)   /* Packet with 1 MPLS
                                                          Label */
#define BCM_PKT_FORMAT_MPLS_TWO_LABELS      (1 << 1)   /* Packet with 2 MPLS
                                                          Label */
#define BCM_PKT_FORMAT_MPLS_THREE_LABELS    (1 << 2)   /* Packet with 3 MPLS
                                                          Label */
#define BCM_PKT_FORMAT_MPLS_FOUR_LABELS     (1 << 3)   /* Packet with 4 MPLS
                                                          Label */
#define BCM_PKT_FORMAT_MPLS_FIVE_LABELS     (1 << 4)   /* Packet with 5 MPLS
                                                          Label */

/* Packet Format: FCOE */
#define BCM_PKT_FORMAT_FIBRE_CHAN_ANY       (0x0)      /* Any packet */
#define BCM_PKT_FORMAT_FIBRE_CHAN           (1 << 0)   /* Standard fibre-channel
                                                          header */
#define BCM_PKT_FORMAT_FIBRE_CHAN_ENCAP     (1 << 1)   /* FC Encapsulation
                                                          header */
#define BCM_PKT_FORMAT_FIBRE_CHAN_VIRTUAL   (1 << 2)   /* FC Virtual fabric tag
                                                          (VFT) header */
#define BCM_PKT_FORMAT_FIBRE_CHAN_ROUTED    (1 << 3)   /* FC Interfabric routing
                                                          (IFR) header */

/* Packet Format: Tunnels */
#define BCM_PKT_FORMAT_TUNNEL_ANY       (0x0)      /* Any packet */
#define BCM_PKT_FORMAT_TUNNEL_NONE      (1 << 0)   /* Non tunneled packet */
#define BCM_PKT_FORMAT_TUNNEL_IP_IN_IP  (1 << 1)   /* IP in IP tunneled packet */
#define BCM_PKT_FORMAT_TUNNEL_GRE       (1 << 2)   /* GRE tunneled packet */
#define BCM_PKT_FORMAT_TUNNEL_MPLS      (1 << 3)   /* MPLS tunneled packet */
#define BCM_PKT_FORMAT_TUNNEL_FCOE      (1 << 4)   /* FCOE tunneled packet */
#define BCM_PKT_FORMAT_TUNNEL_FCOE_INIT (1 << 5)   /* FCOE initialization
                                                      proptocol (FIP) packet */

/* Packet Format: HIGIG */
#define BCM_PKT_FORMAT_HIGIG_ANY        (0x0)      /* Any HIGIG packet */
#define BCM_PKT_FORMAT_HIGIG_PRESENT    (1 << 0)   /* HIGIG packets only */
#define BCM_PKT_FORMAT_HIGIG_NONE       (1 << 1)   /* Non-HIGIG packets only */

/* Packet Format: NIV */
#define BCM_PKT_FORMAT_VNTAG_ANY        (0x0)      /* Any NIV packet */
#define BCM_PKT_FORMAT_VNTAG_PRESENT    (1 << 0)   /* NIV packets only */
#define BCM_PKT_FORMAT_VNTAG_NONE       (1 << 1)   /* Non-NIV packets only */

/* Packet Format: Extender tag */
#define BCM_PKT_FORMAT_ETAG_ANY     (0x0)      /* Any Extender tagged packets */
#define BCM_PKT_FORMAT_ETAG_PRESENT (1 << 0)   /* Extender tagged packets only */
#define BCM_PKT_FORMAT_ETAG_NONE    (1 << 1)   /* Non-Extender tagged packets
                                                  only */

/* Packet Format: CNTAG */
#define BCM_PKT_FORMAT_CNTAG_ANY        (0x0)      /* Any CNTAG packet */
#define BCM_PKT_FORMAT_CNTAG_PRESENT    (1 << 0)   /* CNTAG packets only */
#define BCM_PKT_FORMAT_CNTAG_NONE       (1 << 1)   /* Non-CNTAG packets only */

/* Packet Format: ICNM */
#define BCM_PKT_FORMAT_ICNM_ANY     (0x0)      /* Any ICNM packets */
#define BCM_PKT_FORMAT_ICNM_PRESENT (1 << 0)   /* ICNM packets only */
#define BCM_PKT_FORMAT_ICNM_NONE    (1 << 1)   /* Non-ICNM packets only */

/* Packet Format: Subport Tag */
#define BCM_PKT_FORMAT_SUBPORT_TAG_ANY      (0x0)      /* Any subport tagged
                                                          packet */
#define BCM_PKT_FORMAT_SUBPORT_TAG_PRESENT  (1 << 0)   /* Subport tagged packets
                                                          only */
#define BCM_PKT_FORMAT_SUBPORT_TAG_NONE     (1 << 1)   /* Non-Subport tagged
                                                          packets only */

/* Packet Format: LB_PKT_TYPE */
#define BCM_PKT_FORMAT_LB_TYPE_ANY      (0x0)      /* Any Loopback or normal
                                                      packet */
#define BCM_PKT_FORMAT_LB_TYPE_MIM      (1 << 0)   /* MIM Loopback */
#define BCM_PKT_FORMAT_LB_TYPE_GENERIC  (1 << 1)   /* Generic Loopback */

/* Packet Format: IFA */
#define BCM_PKT_FORMAT_IFA_ANY      (0x0)      /* Do not care for IFA header */
#define BCM_PKT_FORMAT_IFA_PRESENT  (1 << 0)   /* IFA header present in the
                                                  packet */
#define BCM_PKT_FORMAT_IFA_NONE     (1 << 1)   /* IFA header not present in the
                                                  packet */

/* fabric gport is implemented as a new type of local gport */
#define BCM_GPORT_LOCAL_FABRIC_SET(gport, link_id)  _SHR_GPORT_LOCAL_FABRIC_SET(gport, link_id) 
#define BCM_GPORT_LOCAL_FABRIC_GET(gport)  _SHR_GPORT_LOCAL_FABRIC_GET(gport) 
#define BCM_GPORT_IS_LOCAL_FABRIC(gport)  _SHR_GPORT_IS_LOCAL_FABRIC(gport) 

/* local interface gport is implemented as a new type of local gport */
#define BCM_GPORT_LOCAL_INTERFACE_SET(gport, port)  _SHR_GPORT_LOCAL_INTERFACE_SET(gport, port) 
#define BCM_GPORT_LOCAL_INTERFACE_GET(gport)  _SHR_GPORT_LOCAL_INTERFACE_GET(gport) 
#define BCM_GPORT_IS_LOCAL_INTERFACE(gport)  _SHR_GPORT_IS_LOCAL_INTERFACE(gport) 

/* Fabric gport set functions */
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_UCAST_SET(_gport, _core)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_UCAST_SET(_gport, _core) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_MCAST_SET(_gport, _core)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_MCAST_SET(_gport, _core) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_TDM_SET(_gport, _core)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TDM_SET(_gport, _core) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_SET(_gport, _core)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_SET(_gport, _core) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_SET(_gport, _core)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_SET(_gport, _core) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_TDM_SET(_gport, _core)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_TDM_SET(_gport, _core) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_SET(_gport, _core, _pipe)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_SET(_gport, _core, _pipe) 

/* Fabric gport get functions */
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_UCAST_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_UCAST_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_MCAST_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_MCAST_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_TDM_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_TDM_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_UCAST_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_MCAST_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_TDM_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_LOCAL_TDM_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_CORE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_PIPE_GET(_gport)  _SHR_COSQ_GPORT_FABRIC_RX_QUEUE_GENERIC_PIPE_PIPE_GET(_gport) 

/* Fabric gport bool functions */
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_UCAST(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_MCAST(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TDM(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_TDM(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_UCAST(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_MCAST(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_MCAST(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_TDM(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_LOCAL_TDM(_gport) 
#define BCM_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(_gport)  _SHR_COSQ_GPORT_IS_FABRIC_RX_QUEUE_GENERIC_PIPE(_gport) 

/* Core gport functions */
#define BCM_COSQ_GPORT_CORE_SET(_gport, _core)  _SHR_COSQ_GPORT_CORE_SET(_gport, _core) 
#define BCM_COSQ_GPORT_CORE_GET(_gport)  _SHR_COSQ_GPORT_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_IS_CORE(_gport)  _SHR_COSQ_GPORT_IS_CORE(_gport) 

/* bcm_policer_pool_t */
typedef int bcm_policer_pool_t;

/* Qualifier Forwarding Type (for bcm_forwarding_type). */
typedef enum bcm_forwarding_type_e {
    bcmForwardingTypeL2 = _SHR_FORWARDING_TYPE_L2, /* L2 switching forwarding. */
    bcmForwardingTypeIp4Ucast = _SHR_FORWARDING_TYPE_IP4UCAST, /* IPv4 Unicast Routing forwarding. */
    bcmForwardingTypeIp4Mcast = _SHR_FORWARDING_TYPE_IP4MCAST, /* IPv4 Multicast Routing forwarding. */
    bcmForwardingTypeIp6Ucast = _SHR_FORWARDING_TYPE_IP6UCAST, /* IPv6 Unicast Routing forwarding. */
    bcmForwardingTypeIp6Mcast = _SHR_FORWARDING_TYPE_IP6MCAST, /* IPv6 Multicast Routing forwarding. */
    bcmForwardingTypeMpls = _SHR_FORWARDING_TYPE_MPLS, /* MPLS Switching forwarding. */
    bcmForwardingTypeTrill = _SHR_FORWARDING_TYPE_TRILL, /* Trill forwarding. */
    bcmForwardingTypeRxReason = _SHR_FORWARDING_TYPE_RXREASON, /* Forwarding according to a RxReason. */
    bcmForwardingTypeTrafficManagement = _SHR_FORWARDING_TYPE_TRAFFIC_MANAGMENT, /* Traffic Management forwarding, when
                                           an external Packet Processor sets the
                                           forwarding decision. */
    bcmForwardingTypeSnoop = _SHR_FORWARDING_TYPE_SNOOP, /* Snooped packet. */
    bcmForwardingTypeFCoE = _SHR_FORWARDING_TYPE_FCoE, /* Fiber Channel over Ethernet
                                           forwarding. */
    bcmForwardingTypeCount = _SHR_FORWARDING_TYPE_COUNT /* Always Last. Not a usable value. */
} bcm_forwarding_type_t;

#define BCM_FORWARDINGTYPE_STRINGS \
{ \
    "L2", \
    "Ip4Ucast", \
    "Ip4Mcast", \
    "Ip6Ucast", \
    "Ip6Mcast", \
    "Mpls", \
    "Trill", \
    "RxReason", \
    "TrafficManagement", \
    "Snoop", \
    "FCoE", \
    "Count"  \
}

/* VPN types */
#define BCM_VPN_TYPE_MPLS_L3    1          
#define BCM_VPN_TYPE_MPLS_VPWS  3          
#define BCM_VPN_TYPE_MPLS_VPLS  7          
#define BCM_VPN_TYPE_VXLAN      7          
#define BCM_VPN_TYPE_L2GRE      7          
#define BCM_VPN_TYPE_MIM        7          
#define BCM_VPN_TYPE_FLOW       7          

/* Set VPN ID */
#define BCM_VPN_MPLS_L3_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = (((_id) & 0x1fff) + (BCM_VPN_TYPE_MPLS_L3 << 12))) 
#define BCM_VPN_MPLS_VPWS_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = (((_id) & 0x3fff) + (BCM_VPN_TYPE_MPLS_VPWS << 12))) 
#define BCM_VPN_MPLS_VPLS_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = ((_id) & 0x8000) ? \
        ((((_id) & 0xfff) | 0x8000) + (BCM_VPN_TYPE_MPLS_VPLS << 12)) : \
        (((_id) & 0x7fff) + (BCM_VPN_TYPE_MPLS_VPLS << 12))) 
#define BCM_VPN_VXLAN_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = ((_id) & 0x8000) ? \
        ((((_id) & 0xfff) | 0x8000) + (BCM_VPN_TYPE_VXLAN << 12)) : \
        (((_id) & 0x7fff) + (BCM_VPN_TYPE_VXLAN << 12))) 
#define BCM_VPN_L2GRE_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = ((_id) & 0x8000) ? \
        ((((_id) & 0xfff) | 0x8000) + (BCM_VPN_TYPE_L2GRE << 12)) : \
        (((_id) & 0x7fff) + (BCM_VPN_TYPE_L2GRE << 12))) 
#define BCM_VPN_MIM_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = ((_id) & 0x8000) ? \
        ((((_id) & 0xfff) | 0x8000) + (BCM_VPN_TYPE_MIM << 12)) : \
        (((_id) & 0x7fff) + (BCM_VPN_TYPE_MIM << 12))) 
#define BCM_VPN_FLOW_VPN_ID_SET(_vpn, _id)  \
        ((_vpn) = ((_id) & 0x8000) ? \
        ((((_id) & 0xfff) | 0x8000) + (BCM_VPN_TYPE_FLOW << 12)) : \
        (((_id) & 0x7fff) + (BCM_VPN_TYPE_FLOW << 12))) 

/* Get Real ID */
#define BCM_VPN_MPLS_L3_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_MPLS_L3 << 12))) 
#define BCM_VPN_MPLS_VPWS_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_MPLS_VPWS << 12))) 
#define BCM_VPN_MPLS_VPLS_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_MPLS_VPLS << 12))) 
#define BCM_VPN_VXLAN_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_VXLAN << 12))) 
#define BCM_VPN_L2GRE_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_L2GRE << 12))) 
#define BCM_VPN_MIM_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_MIM << 12))) 
#define BCM_VPN_FLOW_VPN_ID_GET(_vpn, _id)  \
        ((_id) = ((_vpn) - (BCM_VPN_TYPE_FLOW << 12))) 

/* Set MPLS Header */
#define BCM_MPLS_HEADER_LABEL_SET(_hdr, _label)  \
        ((_hdr) = ((_hdr) | ((_label & 0xfffff) << 12))) 
#define BCM_MPLS_HEADER_EXP_SET(_hdr, _exp)  \
        ((_hdr) = ((_hdr) | ((_exp & 0x7) << 9))) 
#define BCM_MPLS_HEADER_S_BIT_SET(_hdr, _s)  \
        ((_hdr) = ((_hdr) | ((_s & 0x1) << 8))) 
#define BCM_MPLS_HEADER_TTL_SET(_hdr, _ttl)  \
        ((_hdr) = ((_hdr) | _ttl)) 

/* Get MPLS Header */
#define BCM_MPLS_HEADER_LABEL_GET(_hdr, _label)  \
        ((_label) = (((_hdr) & 0xfffff000) >> 12)) 
#define BCM_MPLS_HEADER_EXP_GET(_hdr, _exp)  \
        ((_exp) = (((_hdr) & 0xe00) >> 9)) 
#define BCM_MPLS_HEADER_S_BIT_GET(_hdr, _s)  \
        ((_s) = (((_hdr) & 0x100) >> 8)) 
#define BCM_MPLS_HEADER_TTL_GET(_hdr, _ttl)  \
        ((_ttl) = ((_hdr) & 0xff)) 

/* GPORT subtypes */
#define BCM_GPORT_SUB_TYPE_PROTECTION       _SHR_GPORT_SUB_TYPE_PROTECTION 
#define BCM_GPORT_SUB_TYPE_FORWARD_GROUP    _SHR_GPORT_SUB_TYPE_FORWARD_GROUP 
#define BCM_GPORT_SUB_TYPE_LIF              _SHR_GPORT_SUB_TYPE_LIF 
#define BCM_GPORT_SUB_TYPE_MULTICAST        _SHR_GPORT_SUB_TYPE_MULTICAST 
#define BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE 
#define BCM_GPORT_SUB_TYPE_TRILL_OUTLIF     _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH 
#define BCM_GPORT_SUB_TYPE_VLAN_TRANSLATION _SHR_GPORT_SUB_TYPE_VLAN_TRANSLATION 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED 

/* SUB GPORT MULTICAST Protection Switch */
#define BCM_GPORT_SUB_TYPE_MULTICAST_PS_SECONDARY _SHR_GPORT_SUB_TYPE_MULTICAST_PS_SECONDARY 
#define BCM_GPORT_SUB_TYPE_MULTICAST_PS_PRIMARY _SHR_GPORT_SUB_TYPE_MULTICAST_PS_PRIMARY 

/* SUB GPORT LIF Exclude Bits */
#define BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY _SHR_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY 
#define BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY _SHR_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY 

/* GPORT subtypes MACROS */
#define BCM_GPORT_SUB_TYPE_IS_PROTECTION(_gport)  _SHR_GPORT_SUB_TYPE_IS_PROTECTION(_gport) 
#define BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(_gport)  _SHR_GPORT_SUB_TYPE_IS_FORWARD_GROUP(_gport) 
#define BCM_GPORT_SUB_TYPE_IS_LIF(_gport)   _SHR_GPORT_SUB_TYPE_IS_LIF(_gport) 
#define BCM_GPORT_SUB_TYPE_IS_MULTICAST(_gport)  _SHR_GPORT_SUB_TYPE_IS_MULTICAST(_gport) 
#define BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(_gport)  _SHR_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(_gport) 
#define BCM_GPORT_SUB_TYPE_IS_TRILL_OUTLIF(_gport)  _SHR_GPORT_SUB_TYPE_IS_TRILL_OUTLIF(_gport) 
#define BCM_GPORT_SUB_TYPE_GET(_gport)      _SHR_GPORT_SUB_TYPE_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_PROTECTION_GET(_gport)  _SHR_GPORT_SUB_TYPE_PROTECTION_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(_gport)  _SHR_GPORT_SUB_TYPE_FORWARD_GROUP_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_LIF_VAL_GET(_gport)  _SHR_GPORT_SUB_TYPE_LIF_VAL_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_LIF_EXC_GET(_gport)  _SHR_GPORT_SUB_TYPE_LIF_EXC_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_MULTICAST_VAL_GET(_gport)  _SHR_GPORT_SUB_TYPE_MULTICAST_VAL_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_MULTICAST_PS_GET(_gport)  _SHR_GPORT_SUB_TYPE_MULTICAST_PS_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(_gport)  _SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(_gport)  _SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(_gport)  _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(_gport)  _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SET(_gport, _virtual_id)  _SHR_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SET(_gport, _virtual_id) 
#define BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(_gport)  _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(_gport)  _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(_gport, _virtual_id)  _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(_gport, _virtual_id) 
#define BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(_gport)  _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(_gport)  _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(_gport, _virtual_id)  _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(_gport, _virtual_id) 
#define BCM_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(_gport)  _SHR_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(_gport) 
#define BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_GET(_gport)  _SHR_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(_gport, _vlan_translation_val)  _SHR_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(_gport, _vlan_translation_val) 
#define BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_gport)  _SHR_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_gport)  _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_gport) 
#define BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(_gport, _virtual_egress_pointed_val)  _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(_gport, _virtual_egress_pointed_val) 

#define BCM_GPORT_SUB_TYPE_PROTECTION_SET(_gport, _protection_val)  \
        (_SHR_GPORT_SUB_TYPE_PROTECTION_SET(_gport, _protection_val)) 

#define BCM_GPORT_SUB_TYPE_FORWARD_GROUP_SET(_gport, _fwd_group_val)  \
        (_SHR_GPORT_SUB_TYPE_FORWARD_GROUP_SET(_gport, _fwd_group_val)) 

#define BCM_GPORT_SUB_TYPE_LIF_SET(_gport, _exc_val, _lif_val)  \
        (_SHR_GPORT_SUB_TYPE_LIF_SET(_gport, _exc_val, _lif_val)) 

#define BCM_GPORT_SUB_TYPE_MULTICAST_SET(_gport, _ps_val, _multicast_val)  \
        (_SHR_GPORT_SUB_TYPE_MULTICAST_SET(_gport, _ps_val, _multicast_val)) 

#define BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SET(_gport, _mpls_push_profile_val)  \
        (_SHR_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SET(_gport, _mpls_push_profile_val)) 

#define BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_SET(_gport, _trill_outlif_val)  \
        (_SHR_GPORT_SUB_TYPE_TRILL_OUTLIF_SET(_gport, _trill_outlif_val)) 

/* L3_ITF subtypes */
#define BCM_L3_ITF_SUB_TYPE_DEFAULT         _SHR_L3_ITF_SUB_TYPE_DEFAULT 
#define BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED 

/* L3_ITF subtypes MACROS */
#define BCM_L3_ITF_SUB_TYPE_IS_SET(_l3_itf)  _SHR_L3_ITF_SUB_TYPE_IS_SET(_l3_itf) 
#define BCM_L3_ITF_SUB_TYPE_GET(_l3_itf)    _SHR_L3_ITF_SUB_TYPE_GET(_l3_itf) 
#define BCM_L3_ITF_SUB_TYPE_VALUE_GET(_l3_itf)  _SHR_L3_ITF_SUB_TYPE_VALUE_GET(_l3_itf) 
#define BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_l3_itf)  _SHR_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(_l3_itf) 
#define BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_l3_itf)  _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(_l3_itf) 
#define BCM_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(_l3_itf, _virtual_egress_pointed_val)  _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(_l3_itf, _virtual_egress_pointed_val) 

/* L3 interface */
#define BCM_L3_ITF_TYPE_RIF     _SHR_L3_ITF_TYPE_RIF 
#define BCM_L3_ITF_TYPE_FEC     _SHR_L3_ITF_TYPE_FEC 
#define BCM_L3_ITF_TYPE_LIF     _SHR_L3_ITF_TYPE_LIF 

/* L3 interface MACROS */
#define BCM_L3_ITF_TYPE_IS_FEC(_l3_itf)  _SHR_L3_ITF_TYPE_IS_FEC(_l3_itf) 
#define BCM_L3_ITF_TYPE_IS_RIF(_l3_itf)  _SHR_L3_ITF_TYPE_IS_RIF(_l3_itf) 
#define BCM_L3_ITF_TYPE_IS_LIF(_l3_itf)  _SHR_L3_ITF_TYPE_IS_LIF(_l3_itf) 
#define BCM_L3_ITF_VAL_GET(_l3_itf)  _SHR_L3_ITF_VAL_GET(_l3_itf) 
#define BCM_L3_ITF_SET(_l3_itf, _type, _id)  _SHR_L3_ITF_SET(_l3_itf, _type, _id) 

/* GPORT to IF */
#define BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(_gport_forward_group, _l3_itf_fec)  \
        _SHR_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(_gport_forward_group, _l3_itf_fec) 

/* GPORT to IF */
#define BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(_l3_itf_fec, _gport_forward_group)  \
        _SHR_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(_l3_itf_fec, _gport_forward_group) 

/* GPORT to IF */
#define BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(_gport_tunnel, _l3_itf_lif)  \
        _SHR_L3_ITF_LIF_TO_GPORT_TUNNEL(_gport_tunnel, _l3_itf_lif) 

/* GPORT to IF */
#define BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(_l3_itf_lif, _gport_tunnel)  \
        _SHR_GPORT_TUNNEL_TO_L3_ITF_LIF(_l3_itf_lif , _gport_tunnel) 

/* FORWARD ENCAP ID */
#define BCM_FORWARD_ENCAP_ID_INVALID        _SHR_FORWARD_ENCAP_ID_INVALID 
#define BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF    _SHR_FORWARD_ENCAP_ID_TYPE_OUTLIF 
#define BCM_FORWARD_ENCAP_ID_TYPE_EEI       _SHR_FORWARD_ENCAP_ID_TYPE_EEI 
#define BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL 
#define BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT 
#define BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID 
#define BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT 
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT 
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER _SHR_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER 
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_DEST_VTEP_PTR _SHR_FORWARD_ENCAP_ID_EEI_USAGE_DEST_VTEP_PTR 

/* FORWARD ENCAP ID MACROS */
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(_forward_encap_id, _label, _push_profile)  _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(_forward_encap_id, _label, _push_profile) 
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_IS_OUTLIF(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_IS_OUTLIF(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_IS_EEI(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_IS_EEI(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_VAL_GET(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_VAL_GET(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GET(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_OUTLIF_USAGE_GET(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(_forward_encap_id)  _SHR_FORWARD_ENCAP_ID_EEI_USAGE_GET(_forward_encap_id) 
#define BCM_FORWARD_ENCAP_ID_VAL_SET(_forward_encap_id, _type, _usage, _val)  _SHR_FORWARD_ENCAP_ID_VAL_SET(_forward_encap_id, _type, _usage, _val)  

/* Creates a stat_id encoding of counter engine ID and counter ID. */
#define BCM_FIELD_STAT_ID_SET(_stat_id, _proc, _ctr)  _SHR_FIELD_STAT_ID_SET(_stat_id, _proc, _ctr) 
#define BCM_FIELD_STAT_ID_COUNTER_GET(_stat_id)  _SHR_FIELD_STAT_ID_COUNTER_GET(_stat_id) 
#define BCM_FIELD_STAT_ID_PROCESSOR_GET(_stat_id)  _SHR_FIELD_STAT_ID_PROCESSOR_GET(_stat_id) 

/* VNTAG structure. */
typedef struct bcm_vntag_s {
    uint8 direction;    /* Direction (0-IV to VIS, 1-VIS to IV). */
    uint8 pointer;      /* Pointer (0-Unicast, 1-Multicast). */
    uint16 dst_vif;     /* Destination Virtual Interface. */
    uint8 loop;         /* Looped (0-No loop, 1-Loop). */
    uint16 src_vif;     /* Source Virtual Interface. */
} bcm_vntag_t;

/* VNTAG action definitions. */
typedef enum bcm_vntag_action_e {
    bcmVnTagActionNone = 0,     /* No action. */
    bcmVnTagActionChange = 1,   /* Replace VnTag if already present or add VnTag
                                   if not already present. */
    bcmVnTagActionDelete = 2,   /* Delete VnTag. */
    bcmVnTagActionCount = 3     /* Always Last, not a usable value. */
} bcm_vntag_action_t;

/* ETAG structure. */
typedef struct bcm_etag_s {
    uint8 pcp;      /* Priority code point. */
    uint8 de;       /* Discard eligibility. */
    uint16 src_vid; /* Source extended port vlan id. */
    uint16 vid;     /* Extended port vlan id. */
} bcm_etag_t;

typedef struct bcm_ipv4_ipv6_addr_s {
    bcm_ip_t ipv4_addr;     /* Ip address (IPv4). */
    uint8 ipv6_addr[16];    /* Ip address (IPv6). */
} bcm_ipv4_ipv6_addr_t;

typedef struct bcm_ip_addr_s {
    int protocol; 
    bcm_ipv4_ipv6_addr_t addr; 
} bcm_ip_addr_t;

/* ETAG action definitions. */
typedef enum bcm_etag_action_e {
    bcmETagActionNone = 0,      /* No action. */
    bcmETagActionChange = 1,    /* Replace ETag if already present or add ETag
                                   if not already present. */
    bcmETagActionDelete = 2,    /* Delete ETag. */
    bcmETagActionCount = 3      /* Always Last, not a usable value. */
} bcm_etag_action_t;

/* bcm_field_stat_e */
typedef enum bcm_field_stat_e {
    bcmFieldStatBytes = 0,              /* Byte count of traffic of all colors. */
    bcmFieldStatPackets = 1,            /* Packet count of traffic of all
                                           colors. */
    bcmFieldStatDefault = bcmFieldStatPackets, /* Default stat mode. */
    bcmFieldStatGreenBytes = 2,         /* Byte count of green traffic. */
    bcmFieldStatGreenPackets = 3,       /* Packet count of green traffic. */
    bcmFieldStatYellowBytes = 4,        /* Byte count of yellow traffic. */
    bcmFieldStatYellowPackets = 5,      /* Packet count of yellow traffic. */
    bcmFieldStatRedBytes = 6,           /* Byte count of red traffic. */
    bcmFieldStatRedPackets = 7,         /* Packet count of red traffic. */
    bcmFieldStatNotGreenBytes = 8,      /* Byte count of not green traffic. */
    bcmFieldStatNotGreenPackets = 9,    /* Packet count of not green traffic. */
    bcmFieldStatNotYellowBytes = 10,    /* Byte count of not yellow traffic. */
    bcmFieldStatNotYellowPackets = 11,  /* Packet count of not yellow traffic. */
    bcmFieldStatNotRedBytes = 12,       /* Byte count of not red traffic. */
    bcmFieldStatNotRedPackets = 13,     /* Packet count of not red traffic. */
    bcmFieldStatAcceptedBytes = 14,     /* Byte count of accepted traffic of all
                                           colors. */
    bcmFieldStatAcceptedPackets = 15,   /* Packet count of accepted traffic of
                                           all colors. */
    bcmFieldStatAcceptedGreenBytes = 16, /* Byte count of accepted green traffic. */
    bcmFieldStatAcceptedGreenPackets = 17, /* Packet count of accepted green
                                           traffic. */
    bcmFieldStatAcceptedYellowBytes = 18, /* Byte count of accepted yellow
                                           traffic. */
    bcmFieldStatAcceptedYellowPackets = 19, /* Packet count of accepted yellow
                                           traffic. */
    bcmFieldStatAcceptedRedBytes = 20,  /* Byte count of accepted red traffic. */
    bcmFieldStatAcceptedRedPackets = 21, /* Packet count of accepted red traffic. */
    bcmFieldStatAcceptedNotGreenBytes = 22, /* Byte count of accepted not green
                                           traffic. */
    bcmFieldStatAcceptedNotGreenPackets = 23, /* Packet count of accepted not green
                                           traffic. */
    bcmFieldStatAcceptedNotYellowBytes = 24, /* Byte count of accepted not yellow
                                           traffic. */
    bcmFieldStatAcceptedNotYellowPackets = 25, /* Packet count of accepted not yellow
                                           traffic. */
    bcmFieldStatAcceptedNotRedBytes = 26, /* Byte count of accepted not red
                                           traffic. */
    bcmFieldStatAcceptedNotRedPackets = 27, /* Packet count of accepted not red
                                           traffic. */
    bcmFieldStatDroppedBytes = 28,      /* Byte count of dropped traffic of all
                                           colors. */
    bcmFieldStatDroppedPackets = 29,    /* Packet count of dropped traffic of
                                           all colors. */
    bcmFieldStatDroppedGreenBytes = 30, /* Byte count of dropped green traffic. */
    bcmFieldStatDroppedGreenPackets = 31, /* Packet count of dropped green
                                           traffic. */
    bcmFieldStatDroppedYellowBytes = 32, /* Byte count of dropped yellow traffic. */
    bcmFieldStatDroppedYellowPackets = 33, /* Packet count of dropped yellow
                                           traffic. */
    bcmFieldStatDroppedRedBytes = 34,   /* Byte count of dropped red traffic. */
    bcmFieldStatDroppedRedPackets = 35, /* Packet count of dropped red traffic. */
    bcmFieldStatDroppedNotGreenBytes = 36, /* Byte count of dropped not green
                                           traffic. */
    bcmFieldStatDroppedNotGreenPackets = 37, /* Packet count of dropped not green
                                           traffic. */
    bcmFieldStatDroppedNotYellowBytes = 38, /* Byte count of dropped not yellow
                                           traffic. */
    bcmFieldStatDroppedNotYellowPackets = 39, /* Packet count of dropped not yellow
                                           traffic. */
    bcmFieldStatDroppedNotRedBytes = 40, /* Byte count of dropped not red
                                           traffic. */
    bcmFieldStatDroppedNotRedPackets = 41, /* Packet count of dropped not red
                                           traffic. */
    bcmFieldStatOffset0Bytes = 42,      /* Byte count at configurable offset 0. */
    bcmFieldStatOffset0Packets = 43,    /* Packet count at configurable offset
                                           0. */
    bcmFieldStatOffset1Bytes = 44,      /* Byte count at configurable offset 1. */
    bcmFieldStatOffset1Packets = 45,    /* Packet count at configurable offset
                                           1. */
    bcmFieldStatOffset2Bytes = 46,      /* Byte count at configurable offset 2. */
    bcmFieldStatOffset2Packets = 47,    /* Packet count at configurable offset
                                           2. */
    bcmFieldStatOffset3Bytes = 48,      /* Byte count at configurable offset 3. */
    bcmFieldStatOffset3Packets = 49,    /* Packet count at configurable offset
                                           3. */
    bcmFieldStatOffset4Bytes = 50,      /* Byte count at configurable offset 4. */
    bcmFieldStatOffset4Packets = 51,    /* Packet count at configurable offset
                                           4. */
    bcmFieldStatOffset5Bytes = 52,      /* Byte count at configurable offset 5. */
    bcmFieldStatOffset5Packets = 53,    /* Packet count at configurable offset
                                           5. */
    bcmFieldStatOffset6Bytes = 54,      /* Byte count at configurable offset 6. */
    bcmFieldStatOffset6Packets = 55,    /* Packet count at configurable offset
                                           6. */
    bcmFieldStatOffset7Bytes = 56,      /* Byte count at configurable offset 7. */
    bcmFieldStatOffset7Packets = 57,    /* Packet count at configurable offset
                                           7. */
    bcmFieldStatCount = 58              /* Always Last. Not a usable value. */
} bcm_field_stat_t;

#define BCM_FIELD_STAT \
{ \
    "Bytes", \
    "Packets", \
    "GreenBytes", \
    "GreenPackets", \
    "YellowBytes", \
    "YellowPackets", \
    "RedBytes", \
    "RedPackets", \
    "NotGreenBytes", \
    "NotGreenPackets", \
    "NotYellowBytes", \
    "NotYellowPackets", \
    "NotRedBytes", \
    "NotRedPackets", \
    "AcceptedBytes", \
    "AcceptedPackets", \
    "AcceptedGreenBytes", \
    "AcceptedGreenPackets", \
    "AcceptedYellowBytes", \
    "AcceptedYellowPackets", \
    "AcceptedRedBytes", \
    "AcceptedRedPackets", \
    "AcceptedNotGreenBytes", \
    "AcceptedNotGreenPackets", \
    "AcceptedNotYellowBytes", \
    "AcceptedNotYellowPackets", \
    "AcceptedNotRedBytes", \
    "AcceptedNotRedPackets", \
    "DroppedBytes", \
    "DroppedPackets", \
    "DroppedGreenBytes", \
    "DroppedGreenPackets", \
    "DroppedYellowBytes", \
    "DroppedYellowPackets", \
    "DroppedRedBytes", \
    "DroppedRedPackets", \
    "DroppedNotGreenBytes", \
    "DroppedNotGreenPackets", \
    "DroppedNotYellowBytes", \
    "DroppedNotYellowPackets", \
    "DroppedNotRedBytes", \
    "DroppedNotRedPackets", \
    "Offset0Bytes", \
    "Offset0Packets", \
    "Offset1Bytes", \
    "Offset1Packets", \
    "Offset2Bytes", \
    "Offset2Packets", \
    "Offset3Bytes", \
    "Offset3Packets", \
    "Offset4Bytes", \
    "Offset4Packets", \
    "Offset5Bytes", \
    "Offset5Packets", \
    "Offset6Bytes", \
    "Offset6Packets", \
    "Offset7Bytes", \
    "Offset7Packets"  \
}

/* PTP Timestamp type */
typedef struct bcm_ptp_timestamp_s {
    uint64 seconds;     /* Seconds */
    uint32 nanoseconds; /* Nanoseconds */
} bcm_ptp_timestamp_t;

#define BCM_STAT_COUNTER_STAT_SET(is_bytes, offset)  _SHR_STAT_COUNTER_STAT_SET(is_bytes, offset) 
#define BCM_STAT_COUNTER_STAT_OFFSET_GET(stat)  _SHR_STAT_COUNTER_STAT_OFFSET_GET(stat) 
#define BCM_STAT_COUNTER_STAT_IS_BYTES_GET(stat)  _SHR_STAT_COUNTER_STAT_IS_BYTES_GET(stat) 

/* Types of casts. */
typedef enum bcm_cast_e {
    bcmCastUC = 0,  /* Unicast */
    bcmCastMC = 1,  /* Multicast */
    bcmCastAll = -1 /* both Unicast and Multicast */
} bcm_cast_t;

/* Packet Format: INT_PKT */
#define BCM_PKT_FORMAT_INT_PKT_ANY      (0x0)      /* Any packet */
#define BCM_PKT_FORMAT_INT_PKT_PRESENT  (1 << 0)   /*  Match on Inband Telemetry
                                                      Packets Only */
#define BCM_PKT_FORMAT_INT_PKT_NONE     (1 << 1)   /*  Match on packets other
                                                      than Inband Telemetry
                                                      Packet */

/* 
 * Set and get macros which decode the database key parameters and stopre
 * it in database handle.
 */
#define BCM_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, is_global, core_id, database_id)  _SHR_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, is_global, core_id, database_id) 
#define BCM_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle)  _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle) 
#define BCM_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle)  _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle) 
#define BCM_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle)  _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle) 
#define BCM_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle)  _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle) 

/* 
 * Set and get macros which decode the database handle and the meter id
 * (called offset) in policer_id.
 */
#define BCM_POLICER_ID_SET(policer_id, database_handle, offset)  _SHR_POLICER_ID_SET(policer_id, database_handle, offset) 
#define BCM_POLICER_ID_OFFSET_GET(policer_id)  _SHR_POLICER_ID_METER_INDEX_GET(policer_id) 
#define BCM_POLICER_ID_DATABASE_HANDLE_GET(policer_id)  _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id) 

/* Encapsulation Access stage */
typedef enum bcm_encap_access_e {
    bcmEncapAccessInvalid = 0,      /* INVALID (automatic decided) */
    bcmEncapAccessTunnel1 = 1,      /* FIRST_ACCESS */
    bcmEncapAccessTunnel2 = 2,      /* SECOND_ACCESS */
    bcmEncapAccessTunnel3 = 3,      /* THIRD_ACCESS */
    bcmEncapAccessTunnel4 = 4,      /* FOURTH_ACCESS */
    bcmEncapAccessNativeArp = 5,    /* NATIVE ARP ACCESS */
    bcmEncapAccessNativeAc = 6,     /* NATIVE ACACCESS */
    bcmEncapAccessArp = 7,          /* OUTER ARP ACCESS */
    bcmEncapAccessRif = 8           /* RIF ACCESS */
} bcm_encap_access_t;

#define BCM_COSQ_GPORT_IS_RCY(gport)  _SHR_COSQ_GPORT_IS_RCY(gport) 
#define BCM_COSQ_GPORT_RCY_SET(gport, core)  _SHR_COSQ_GPORT_RCY_SET(gport,core) 
#define BCM_COSQ_GPORT_IS_RCY_MIRR(gport)  _SHR_COSQ_GPORT_IS_RCY_MIRR(gport) 
#define BCM_COSQ_GPORT_RCY_MIRR_SET(gport, core)  _SHR_COSQ_GPORT_RCY_MIRR_SET(gport,core) 

#define BCM_PORT_INVALID        -1         /* Invalid port. */

#define BCM_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(_gport)  _SHR_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(_gport) 
#define BCM_COSQ_GPORT_INGRESS_RECEIVE_CPU_GUARANTEED_SET(_gport, _core)  _SHR_COSQ_GPORT_INGRESS_RECEIVE_CPU_GUARANTEED_SET(_gport, _core) 
#define BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED(_gport)  _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED(_gport) 
#define BCM_COSQ_GPORT_INGRESS_RECEIVE_RCY_GUARANTEED_SET(_gport, _core)  _SHR_COSQ_GPORT_INGRESS_RECEIVE_RCY_GUARANTEED_SET(_gport, _core) 
#define BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED(_gport)  _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED(_gport) 
#define BCM_COSQ_GPORT_INGRESS_RECEIVE_NIF_SET(_gport, _core)  _SHR_COSQ_GPORT_INGRESS_RECEIVE_NIF_SET(_gport, _core) 
#define BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_NIF(_gport)  _SHR_COSQ_GPORT_IS_INGRESS_RECEIVE_NIF(_gport) 

#define BCM_COSQ_GPORT_E2E_INTERFACE_GET(_gport)  _SHR_COSQ_GPORT_E2E_INTERFACE_GET(_gport) 
#define BCM_COSQ_GPORT_E2E_INTERFACE_SET(_gport, _port)  _SHR_COSQ_GPORT_E2E_INTERFACE_SET(_gport, _port) 
#define BCM_COSQ_GPORT_IS_E2E_INTERFACE(_gport)  _SHR_COSQ_GPORT_IS_E2E_INTERFACE(_gport) 

/* Maximum bucket count for a latency histogram. */
#define BCM_MAX_HISTOGRAM_BUCKET_COUNT  8          

/* 
 * Drop Event definitions which are available for stat, switch and
 * mirorring usage.
 */
typedef enum bcm_pkt_drop_event_e {
    bcmPktDropEventIngressNoDrop = 0,   /* Ingress: No Drop */
    bcmPktDropEventIngressAllDrops = 1, /* Ingress: All Drops */
    bcmPktDropEventIngressCmlFlagsDrop = 2, /* Ingress: Cml Flags Drop */
    bcmPktDropEventIngressL2SrcStaticMove = 3, /* Ingress: L2 Src Static Move */
    bcmPktDropEventIngressL2SrcDiscard = 4, /* Ingress: L2 Src Discard */
    bcmPktDropEventIngressMacsaMulticast = 5, /* Ingress: Macsa Multicast */
    bcmPktDropEventIngressOuterTpidCheckFailed = 6, /* Ingress: Outer Tpid Check Failed */
    bcmPktDropEventIngressIncomingPvlanCheckFailed = 7, /* Ingress: Incoming Pvlan Check Failed */
    bcmPktDropEventIngressPktIntegrityCheckFailed = 8, /* Ingress: Pkt Integrity Check Failed */
    bcmPktDropEventIngressProtocolPktDrop = 9, /* Ingress: Protocol Pkt Drop */
    bcmPktDropEventIngressMembershipCheckFailed = 10, /* Ingress: Membership Check Failed */
    bcmPktDropEventIngressSpanningTreeCheckFailed = 11, /* Ingress: Spanning Tree Check Failed */
    bcmPktDropEventIngressL2DstLookupMiss = 12, /* Ingress: L2 Dst Lookup Miss */
    bcmPktDropEventIngressL2DstDiscard = 13, /* Ingress: L2 Dst Discard */
    bcmPktDropEventIngressL3DstLookupMiss = 14, /* Ingress: L3 Dst Lookup Miss */
    bcmPktDropEventIngressL3DstDiscard = 15, /* Ingress: L3 Dst Discard */
    bcmPktDropEventIngressL3HdrError = 16, /* Ingress: L3 Hdr Error */
    bcmPktDropEventIngressL3TtlError = 17, /* Ingress: L3 Ttl Error */
    bcmPktDropEventIngressIpmcL3IifOrRpaIdCheckFailed = 18, /* Ingress: Ipmc L3 Iif Or Rpa Id Check
                                           Failed */
    bcmPktDropEventIngressTunnelTtlCheckFailed = 19, /* Ingress: Tunnel Ttl Check Failed */
    bcmPktDropEventIngressTunnelShimHdrError = 20, /* Ingress: Tunnel Shim Hdr Error */
    bcmPktDropEventIngressTunnelObjectValidationFailed = 21, /* Ingress: Tunnel Object Validation
                                           Failed */
    bcmPktDropEventIngressTunnelAdaptDrop = 22, /* Ingress: Tunnel Adapt Drop */
    bcmPktDropEventIngressPvlanDrop = 23, /* Ingress: Pvlan Drop */
    bcmPktDropEventIngressVfp = 24,     /* Ingress: Vfp */
    bcmPktDropEventIngressIfp = 25,     /* Ingress: Ifp */
    bcmPktDropEventIngressIfpMeter = 26, /* Ingress: Ifp Meter */
    bcmPktDropEventIngressDstFp = 27,   /* Ingress: Dst Fp */
    bcmPktDropEventIngressMplsProtectionDrop = 28, /* Ingress: Mpls Protection Drop */
    bcmPktDropEventIngressMplsLabelActionInvalid = 29, /* Ingress: Mpls Label Action Invalid */
    bcmPktDropEventIngressMplsTermPolicySelectTableDrop = 30, /* Ingress: Mpls Term Policy Select
                                           Table Drop */
    bcmPktDropEventIngressMplsReservedLabelExposed = 31, /* Ingress: Mpls Reserved Label Exposed */
    bcmPktDropEventIngressMplsTtlError = 32, /* Ingress: Mpls Ttl Error */
    bcmPktDropEventEgressNoDrop = 33,   /* Egress: No Drop */
    bcmPktDropEventEgressAllDrops = 34, /* Egress: All Drops */
    bcmPktDropEventEgressL2OifDrop = 35, /* Egress: L2 Oif Drop */
    bcmPktDropEventEgressMembershipDrop = 36, /* Egress: Membership Drop */
    bcmPktDropEventEgressDvpMembershipDrop = 37, /* Egress: Dvp Membership Drop */
    bcmPktDropEventEgressTtlDrop = 38,  /* Egress: Ttl Drop */
    bcmPktDropEventEgressL3SameIntfDrop = 39, /* Egress: L3 Same Intf Drop */
    bcmPktDropEventEgressL2SamePortDrop = 40, /* Egress: L2 Same Port Drop */
    bcmPktDropEventEgressSplitHorizonDrop = 41, /* Egress: Split Horizon Drop */
    bcmPktDropEventEgressStgDisableDrop = 42, /* Egress: Stg Disable Drop */
    bcmPktDropEventEgressStgBlockDrop = 43, /* Egress: Stg Block Drop */
    bcmPktDropEventEgressEfpDrop = 44,  /* Egress: Efp Drop */
    bcmPktDropEventEgressEgrMeterDrop = 45, /* Egress: Egr Meter Drop */
    bcmPktDropEventIngressEmFt = 46,    /* Ingress: Em Ft */
    bcmPktDropEventIngressIvxlt = 47,   /* Ingress: Ivxlt */
    bcmPktDropEventEgressEgrMtuDrop = 48, /* Egress: Egr Mtu Drop */
    bcmPktDropEventEgressEgrVxltDrop = 49, /* Egress: Egr Vxlt Drop */
    bcmPktDropEventIngressUrpfCheckFailed = 50, /* Ingress: Urpf Check Failed */
    bcmPktDropEventIngressSrcPortKnockoutDrop = 51, /* Ingress: Src Port Knockout Drop */
    bcmPktDropEventIngressLagFailoverPortDown = 52, /* Ingress: Lag Failover Port Down */
    bcmPktDropEventIngressSplitHorizonCheckFailed = 53, /* Ingress: Split Horizon Check Failed */
    bcmPktDropEventIngressDstLinkDown = 54, /* Ingress: Dst Link Down */
    bcmPktDropEventIngressBlockMaskDrop = 55, /* Ingress: Block Mask Drop */
    bcmPktDropEventIngressL3MtuCheckFailed = 56, /* Ingress: L3 Mtu Check Failed */
    bcmPktDropEventIngressSeqNumCheckFailed = 57, /* Ingress: Seq Num Check Failed */
    bcmPktDropEventIngressL3IifEqL3Oif = 58, /* Ingress: L3 Iif Eq L3 Oif */
    bcmPktDropEventIngressStormControlDrop = 59, /* Ingress: Storm Control Drop */
    bcmPktDropEventIngressEgrMembershipCheckFailed = 60, /* Ingress: Egr Membership Check Failed */
    bcmPktDropEventIngressEgrSpanningTreeCheckFailed = 61, /* Ingress: Egr Spanning Tree Check
                                           Failed */
    bcmPktDropEventIngressDstPbmZero = 62, /* Ingress: Dst Pbm Zero */
    bcmPktDropEventEgressEgrCellTooLarge = 63, /* Egress: Egr Cell Too Large */
    bcmPktDropEventEgressEgrCellTooSmall = 64, /* Egress: Egr Cell Too Small */
    bcmPktDropEventEgressQosRemarkingDrop = 65, /* Egress: Qos Remarking Drop */
    bcmPktDropEventEgressSvpEqualDvpDrop = 66, /* Egress: Svp Equal Dvp Drop */
    bcmPktDropEventEgressInvalid1588Pkt = 67, /* Egress: Invalid 1588 Pkt */
    bcmPktDropEventIngressMplsEcnError = 68, /* Ingress: Mpls Ecn Error */
    bcmPktDropEventIngressMplsCtrlPktDrop = 69, /* Ingress: Mpls Ctrl Pkt Drop */
    bcmPktDropEventEgressFlexEditorDrop = 70, /* Egress: Flex Editor Drop */
    bcmPktDropEventIngressEpRecircDrop = 71, /* Ingress: Switched Packet Dropped Due
                                           To EP Recirculate */
    bcmPktDropEventIngressExtIpv4GatewayTable = 72, /* Ingress: IPv4 Packet Dropped As A
                                           Result Of External IPv4 Gateway Table
                                           Lookup */
    bcmPktDropEventEgressEpRecircDrop = 73, /* Egress: Switched Packet Dropped Due
                                           To EP Recirculate */
    bcmPktDropEventEgressIfaMdDeleteDrop = 74, /* Egress: IFA (Inband-Flow Analyzer)
                                           Metadata Delete Error */
    bcmPktDropEventCount = 75           /* Must Be Last. Not A Valid Event */
} bcm_pkt_drop_event_t;

/* 
 * Trace Event definitions which are available for stat, switch and
 * mirorring usage.
 */
typedef enum bcm_pkt_trace_event_e {
    bcmPktTraceEventIngressCmlFlags = 0, /* Ingress: Cml Flags */
    bcmPktTraceEventIngressL2SrcStaticMove = 1, /* Ingress: L2 Src Static Move */
    bcmPktTraceEventIngressPktIntegrityCheckFailed = 2, /* Ingress: Pkt Integrity Check Failed */
    bcmPktTraceEventIngressProtocolPkt = 3, /* Ingress: Protocol Pkt */
    bcmPktTraceEventIngressL2DstLookupMiss = 4, /* Ingress: L2 Dst Lookup Miss */
    bcmPktTraceEventIngressL2DstLookup = 5, /* Ingress: L2 Dst Lookup */
    bcmPktTraceEventIngressL3DstLookupMiss = 6, /* Ingress: L3 Dst Lookup Miss */
    bcmPktTraceEventIngressL3DstLookup = 7, /* Ingress: L3 Dst Lookup */
    bcmPktTraceEventIngressL3HdrError = 8, /* Ingress: L3 Hdr Error */
    bcmPktTraceEventIngressL3TtlError = 9, /* Ingress: L3 Ttl Error */
    bcmPktTraceEventIngressIpmcL3IifOrRpaIdCheckFailed = 10, /* Ingress: Ipmc L3 Iif Or Rpa Id Check
                                           Failed */
    bcmPktTraceEventIngressLearnCacheFull = 11, /* Ingress: Learn Cache Full */
    bcmPktTraceEventIngressVfp = 12,    /* Ingress: Vfp */
    bcmPktTraceEventIngressIfp = 13,    /* Ingress: Ifp */
    bcmPktTraceEventIngressIfpMeter = 14, /* Ingress: Ifp Meter */
    bcmPktTraceEventIngressDstFp = 15,  /* Ingress: Dst Fp */
    bcmPktTraceEventEgressEfp = 16,     /* Egress: Efp */
    bcmPktTraceEventIngressL2SrcDiscard = 17, /* Ingress: L2 Src Discard */
    bcmPktTraceEventIngressMacsaMulticast = 18, /* Ingress: Macsa Multicast */
    bcmPktTraceEventIngressMembershipCheckFailed = 19, /* Ingress: Membership Check Failed */
    bcmPktTraceEventIngressSpanningTreeCheckFailed = 20, /* Ingress: Spanning Tree Check Failed */
    bcmPktTraceEventIngressSvp = 21,    /* Ingress: Svp */
    bcmPktTraceEventIngressEmFt = 22,   /* Ingress: Em Ft */
    bcmPktTraceEventIngressIvxlt = 23,  /* Ingress: Ivxlt */
    bcmPktTraceEventIngressTraceDoNotMirror = 24, /* Ingress: Trace Do Not Mirror */
    bcmPktTraceEventIngressTraceDoNotCopyToCpu = 25, /* Ingress: Trace Do Not Copy To CPU */
    bcmPktTraceEventIngressTraceDop = 26, /* Ingress: Trace Dop */
    bcmPktTraceEventIngressUrpfCheckFailed = 27, /* Ingress: Urpf Check Failed */
    bcmPktTraceEventIngressL3IifEqL3Oif = 28, /* Ingress: L3 Iif Eq L3 Oif */
    bcmPktTraceEventIngressDlbEcmpMonitorEnOrMemberReassined = 29, /* Ingress: Dlb Ecmp Monitor En Or
                                           Member Reassined */
    bcmPktTraceEventIngressDlbLagMonitorEnOrMemberReassined = 30, /* Ingress: Dlb Lag Monitor En Or Member
                                           Reassined */
    bcmPktTraceEventIngressMatchedRuleBit0 = 31, /* Ingress: Matched Rule Bit 0 */
    bcmPktTraceEventIngressMatchedRuleBit1 = 32, /* Ingress: Matched Rule Bit 1 */
    bcmPktTraceEventIngressMatchedRuleBit2 = 33, /* Ingress: Matched Rule Bit 2 */
    bcmPktTraceEventIngressMatchedRuleBit3 = 34, /* Ingress: Matched Rule Bit 3 */
    bcmPktTraceEventIngressMatchedRuleBit4 = 35, /* Ingress: Matched Rule Bit 4 */
    bcmPktTraceEventIngressMatchedRuleBit5 = 36, /* Ingress: Matched Rule Bit 5 */
    bcmPktTraceEventIngressMatchedRuleBit6 = 37, /* Ingress: Matched Rule Bit 6 */
    bcmPktTraceEventIngressMatchedRuleBit7 = 38, /* Ingress: Matched Rule Bit 7 */
    bcmPktTraceEventIngressMirrorSamplerSampled = 39, /* Ingress: Mirror Sampler Sampled */
    bcmPktTraceEventIngressMirrorSamplerEgrSampled = 40, /* Ingress: Mirror Sampler Egr Sampled */
    bcmPktTraceEventIngressSerDrop = 41, /* Ingress: Ser Drop */
    bcmPktTraceEventIngressMplsCtrlPktToCpu = 42, /* Ingress: Mpls Ctrl Pkt To Cpu */
    bcmPktTraceEventIngressExtIpv4GatewayTable = 43, /* Ingress: IPv4 Packet Hit As A Result
                                           Of External IPv4 Gateway Table Lookup */
    bcmPktTraceEventIngressDlbEcmpPktSampled = 44, /* Ingress: DLB ECMP Packet Sampled */
    bcmPktTraceEventIngressDlbLagPktSampled = 45, /* Ingress: DLB LAG Packet Sampled */
    bcmPktTraceEventCount = 46          /* Must Be Last. Not A Valid Event */
} bcm_pkt_trace_event_t;

#define BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(_gport)  \
     _SHR_GPORT_IS_INGRESS_LATENCY_PROFILE(_gport) 
#define BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(_gport)  \
    (!_SHR_GPORT_IS_INGRESS_LATENCY_PROFILE(_gport) ? -1 : \
    _SHR_GPORT_INGRESS_LATENCY_PROFILE_GET(_gport)) 
#define BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(_gport, _profile)  \
        (_SHR_GPORT_INGRESS_LATENCY_PROFILE_SET(_gport, _profile))
 
#define BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(_gport)  \
     _SHR_GPORT_IS_END_TO_END_LATENCY_PROFILE(_gport) 
#define BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(_gport)  \
    (!_SHR_GPORT_IS_END_TO_END_LATENCY_PROFILE(_gport) ? -1 : \
    _SHR_GPORT_END_TO_END_LATENCY_PROFILE_GET(_gport)) 
#define BCM_GPORT_END_TO_END_LATENCY_PROFILE_SET(_gport, _profile)  \
        (_SHR_GPORT_END_TO_END_LATENCY_PROFILE_SET(_gport, _profile))
 
#define BCM_GPORT_IS_LATENCY_AQM_FLOW_ID(_gport)  \
     _SHR_GPORT_IS_LATENCY_AQM_FLOW_ID(_gport) 
#define BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_LATENCY_AQM_FLOW_ID(_gport) ? -1 : \
    _SHR_GPORT_LATENCY_AQM_FLOW_ID_GET(_gport)) 
#define BCM_GPORT_LATENCY_AQM_FLOW_ID_SET(_gport, _profile)  \
        (_SHR_GPORT_LATENCY_AQM_FLOW_ID_SET(_gport, _profile))
 
#define BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(_gport)  \
     _SHR_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(_gport) 
#define BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_GET(_gport)  \
    (!_SHR_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(_gport) ? -1 : \
    _SHR_GPORT_END_TO_END_AQM_LATENCY_PROFILE_GET(_gport)) 
#define BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_SET(_gport, _profile)  \
        (_SHR_GPORT_END_TO_END_AQM_LATENCY_PROFILE_SET(_gport, _profile))
 

#define BCM_COSQ_GPORT_INBAND_COE_SET(_gport, _port)  \
    _SHR_COSQ_GPORT_INBAND_COE_SET(_gport, _port) 

#define BCM_COSQ_GPORT_INBAND_COE_GET(_gport)  \
    (!_SHR_COSQ_GPORT_IS_INBAND_COE(_gport) ? -1 :  \
    _SHR_COSQ_GPORT_INBAND_COE_GET(_gport)) 

#define BCM_FLEXE_GROUP_GPORT_SET(_gport, _port)  \
    _SHR_FLEXE_GROUP_GPORT_SET(_gport, _port) 

#define BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(_gport)  \
    (!_SHR_GPORT_IS_FLEXE_GROUP(_gport) ? -1 :  \
    _SHR_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(_gport)) 

#define BCM_GPORT_TDM_STREAM_SET(_gport, _port)  \
    _SHR_GPORT_TDM_STREAM_SET(_gport, _port) 

#define BCM_GPORT_TDM_STREAM_ID_GET(_gport)  \
    (!_SHR_GPORT_IS_TDM_STREAM(_gport) ? -1 :  \
    _SHR_GPORT_TDM_STREAM_ID_GET(_gport)) 

/* Opaque handle to a field entry. */
typedef int bcm_field_entry_t;

/* Opaque handle to a field group. */
typedef int bcm_field_group_t;

/* Drop Reasons */
typedef enum bcm_cosq_drop_reason_e {
    bcmCosqDropReasonGlobalDramBdbsReject = 0, 
    bcmCosqDropReasonGlobalSramBuffersReject = 1, 
    bcmCosqDropReasonGlobalSramPdbsReject = 2, 
    bcmCosqDropReasonDropPrecedenceLevelReject = 3, 
    bcmCosqDropReasonQueueNumResolutionErrorReject = 4, 
    bcmCosqDropReasonQueueNumNotValidReject = 5, 
    bcmCosqDropReasonMulticastReplicationErrorReject = 6, 
    bcmCosqDropReasonMulticastFifoFullReject = 7, 
    bcmCosqDropReasonTarFifoFullReject = 8, 
    bcmCosqDropReasonPacketSizeErrorReject = 9, 
    bcmCosqDropReasonSramResourceErrorReject = 10, 
    bcmCosqDropReasonExternalErrorReject = 11, 
    bcmCosqDropReasonVoqSramPdsTotalFreeSharedReject = 12, 
    bcmCosqDropReasonVoqSramPdsSharedMaxSizeReject = 13, 
    bcmCosqDropReasonVoqSramBuffersTotalFreeSharedReject = 14, 
    bcmCosqDropReasonVoqSramBuffersSharedMaxSizeReject = 15, 
    bcmCosqDropReasonVoqWordsTotalFreeSharedReject = 16, 
    bcmCosqDropReasonVoqWordsSharedMaxSizeReject = 17, 
    bcmCosqDropReasonVoqSystemRedReject = 18, 
    bcmCosqDropReasonVoqWredReject = 19, 
    bcmCosqDropReasonVoqDramBlockReject = 20, 
    bcmCosqDropReasonPbVsqSramPdsTotalFreeSharedReject = 21, 
    bcmCosqDropReasonPbVsqSramPdsSharedMaxSizeReject = 22, 
    bcmCosqDropReasonVsqDSramPdsSharedMaxSizeReject = 23, 
    bcmCosqDropReasonVsqCSramPdsSharedMaxSizeReject = 24, 
    bcmCosqDropReasonVsqBSramPdsSharedMaxSizeReject = 25, 
    bcmCosqDropReasonVsqASramPdsSharedMaxSizeReject = 26, 
    bcmCosqDropReasonPbVsqSramBuffersTotalFreeSharedReject = 27, 
    bcmCosqDropReasonPbVsqSramBuffersSharedMaxSizeReject = 28, 
    bcmCosqDropReasonVsqDSramBuffersSharedMaxSizeReject = 29, 
    bcmCosqDropReasonVsqCSramBuffersSharedMaxSizeReject = 30, 
    bcmCosqDropReasonVsqBSramBuffersSharedMaxSizeReject = 31, 
    bcmCosqDropReasonVsqASramBuffersSharedMaxSizeReject = 32, 
    bcmCosqDropReasonPbVsqWordsTotalFreeSharedReject = 33, 
    bcmCosqDropReasonPbVsqWordsSharedMaxSizeReject = 34, 
    bcmCosqDropReasonVsqDWordsSharedMaxSizeReject = 35, 
    bcmCosqDropReasonVsqCWordsSharedMaxSizeReject = 36, 
    bcmCosqDropReasonVsqBWordsSharedMaxSizeReject = 37, 
    bcmCosqDropReasonVsqAWordsSharedMaxSizeReject = 38, 
    bcmCosqDropReasonVsqFWredReject = 39, 
    bcmCosqDropReasonVsqEWredReject = 40, 
    bcmCosqDropReasonVsqDWredReject = 41, 
    bcmCosqDropReasonVsqCWredReject = 42, 
    bcmCosqDropReasonVsqBWredReject = 43, 
    bcmCosqDropReasonVsqAWredReject = 44, 
    bcmCosqDropReasonLatencyReject = 45, 
    bcmCosqDropReasonRejectCount = 46 
} bcm_cosq_drop_reason_t;

/* SOBMH header Timestamp offset for different packet flows */
#define BCM_PKT_HDR_TS_OFFSET_TWAMP_OWAMP   54         /*  SOBMH header
                                                          Timestamp offset for
                                                          TWAMP/OWAMP */

#define BCM_PORT_FLEXE_MAX_NOF_SLOTS    80         

/* FlexE time slot type, each bit stands for one slot */
typedef uint32 bcm_port_flexe_time_slot_t[_SHR_BITDCLSIZE(BCM_PORT_FLEXE_MAX_NOF_SLOTS)];

#define BCM_PORT_FLEXE_TIME_SLOT_GET(time_slot, slot_id)  SHR_BITGET(time_slot, slot_id) 
#define BCM_PORT_FLEXE_TIME_SLOT_SET(time_slot, slot_id)  SHR_BITSET(time_slot, slot_id) 
#define BCM_PORT_FLEXE_TIME_SLOT_CLR(time_slot, slot_id)  SHR_BITCLR(time_slot, slot_id) 

/* Type of SVTag packet. */
typedef enum bcm_svtag_pkt_type_e {
    bcmSvtagPktTypeNonMacsec = 0,   /* Non-MACSEC packet. */
    bcmSvtagPktTypeMacsec = 1,      /* MACsec Packet. */
    bcmSvtagPktTypeNonKayMgmt = 2,  /* Non Key Management Packet. */
    bcmSvtagPktTypeKayMgmt = 3      /* Key Management Packet. */
} bcm_svtag_pkt_type_t;

/* Load balancing disable hash layers flags. */
#define BCM_HASH_LAYER_ETHERNET_DISABLE 0x1        /* Disable the Ethernet layer
                                                      from the load balancing
                                                      hash. */
#define BCM_HASH_LAYER_IPV4_DISABLE     0x2        /* Disable the IPv4 layer
                                                      from the load balancing
                                                      hash. */
#define BCM_HASH_LAYER_IPV6_DISABLE     0x4        /* Disable the IPv6 layer
                                                      from the load balancing
                                                      hash. */
#define BCM_HASH_LAYER_MPLS_DISABLE     0x8        /* Disable the MPLS layer
                                                      from the load balancing
                                                      hash. */
#define BCM_HASH_LAYER_TCP_DISABLE      0x10       /* Disable the TCP layer from
                                                      the load balancing hash. */
#define BCM_HASH_LAYER_UDP_DISABLE      0x20       /* Disable the UDP layer from
                                                      the load balancing hash. */

#endif /* __BCM_TYPES_H__ */
