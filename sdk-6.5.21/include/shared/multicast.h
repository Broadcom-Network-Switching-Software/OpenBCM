/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains multicast definitions.
 */

#ifndef _SHR_MULTICAST_H
#define _SHR_MULTICAST_H
#include <shared/types.h>
#include <shared/gport.h>

#define _SHR_MULTICAST_TYPE_L2           1
#define _SHR_MULTICAST_TYPE_L3           2
#define _SHR_MULTICAST_TYPE_VPLS         3
#define _SHR_MULTICAST_TYPE_SUBPORT      4
#define _SHR_MULTICAST_TYPE_MIM          5
#define _SHR_MULTICAST_TYPE_WLAN         6
#define _SHR_MULTICAST_TYPE_VLAN         7
#define _SHR_MULTICAST_TYPE_TRILL        8
#define _SHR_MULTICAST_TYPE_NIV          9
#define _SHR_MULTICAST_TYPE_EGRESS_OBJECT 10
#define _SHR_MULTICAST_TYPE_L2GRE  11
#define _SHR_MULTICAST_TYPE_VXLAN  12
#define _SHR_MULTICAST_TYPE_PORTS_GROUP 13 
#define _SHR_MULTICAST_TYPE_EXTENDER    14 
#define _SHR_MULTICAST_TYPE_FLOW        15
#define _SHR_MULTICAST_TYPE_ING_BMP     16
#define _SHR_MULTICAST_TYPE_EGR_BMP     17

#define _SHR_MULTICAST_TYPE_SHIFT        24
#define _SHR_MULTICAST_TYPE_MASK         0xff

#define _SHR_MULTICAST_ID_SHIFT          0
#define _SHR_MULTICAST_ID_MASK           0xffffff

/* BCM_MULTICAST_IS_SET */
#define _SHR_MULTICAST_IS_SET(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) != 0)

#define _SHR_MULTICAST_GROUP_SET(_group_, _type_, _id_) \
    ((_group_) = ((_type_) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_id_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_ID_GET(_group_) \
    (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK)

#define _SHR_MULTICAST_TYPE_GET(_group_) \
    (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) & _SHR_MULTICAST_TYPE_MASK)

/* BCM_MULTICAST_IS_XXXX */
#define _SHR_MULTICAST_IS_L2(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_L2)

#define _SHR_MULTICAST_IS_L3(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_L3)

#define _SHR_MULTICAST_IS_VPLS(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_VPLS)

#define _SHR_MULTICAST_IS_SUBPORT(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_SUBPORT)

#define _SHR_MULTICAST_IS_MIM(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_MIM)

#define _SHR_MULTICAST_IS_WLAN(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_WLAN)

#define _SHR_MULTICAST_IS_VLAN(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_VLAN)

#define _SHR_MULTICAST_IS_TRILL(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_TRILL)

#define _SHR_MULTICAST_IS_NIV(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_NIV)

#define _SHR_MULTICAST_IS_EGRESS_OBJECT(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_EGRESS_OBJECT)

#define _SHR_MULTICAST_IS_L2GRE(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_L2GRE)

#define _SHR_MULTICAST_IS_VXLAN(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_VXLAN)

#define _SHR_MULTICAST_IS_FLOW(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_FLOW)

#define _SHR_MULTICAST_IS_PORTS_GROUP(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_PORTS_GROUP)

#define _SHR_MULTICAST_IS_EXTENDER(_group_)    \
        (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) == _SHR_MULTICAST_TYPE_EXTENDER)

/* BCM_MULTICAST_XXXX_SET */
#define _SHR_MULTICAST_L2_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_L2) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_L3_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_L3) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_VPLS_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_VPLS) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_SUBPORT_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_SUBPORT) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_MIM_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_MIM) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_WLAN_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_WLAN) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_VLAN_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_VLAN) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_TRILL_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_TRILL) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_NIV_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_NIV) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_EGRESS_OBJECT_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_EGRESS_OBJECT) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_L2GRE_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_L2GRE) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_VXLAN_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_VXLAN) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_FLOW_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_FLOW) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_PORTS_GROUP_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_PORTS_GROUP) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_EXTENDER_SET(_group_, _mc_index_)   \
    ((_group_) = ((_SHR_MULTICAST_TYPE_EXTENDER) << _SHR_MULTICAST_TYPE_SHIFT)  | \
     (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

/* BCM_MULTICAST_XXXX_GET */
#define _SHR_MULTICAST_L2_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_L2) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_L3_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_L3) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_VPLS_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_VPLS) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_SUBPORT_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_SUBPORT) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_MIM_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_MIM) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_WLAN_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_WLAN) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_VLAN_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_VLAN) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_TRILL_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_TRILL) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_NIV_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_NIV) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_EGRESS_OBJECT_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_EGRESS_OBJECT) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_L2GRE_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_L2GRE) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_VXLAN_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_VXLAN) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_FLOW_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_FLOW) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_PORTS_GROUP_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_PORTS_GROUP) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

#define _SHR_MULTICAST_EXTENDER_GET(_group_) \
    ((_SHR_MULTICAST_TYPE(_group_) == _SHR_MULTICAST_TYPE_EXTENDER) ? \
            (((_group_) >> _SHR_MULTICAST_ID_SHIFT) & _SHR_MULTICAST_ID_MASK) : -1)

/* BCM_MULTICAST_XXXX_GET */
#define _SHR_MULTICAST_L2(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_L2) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_L3(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_L3) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_VPLS(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_VPLS) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_SUBPORT(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_SUBPORT) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_MIM(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_MIM) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_WLAN(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_WLAN) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_VLAN(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_VLAN) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_TRILL(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_TRILL) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_NIV(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_NIV) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_EGRESS_OBJECT(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_EGRESS_OBJECT) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_L2GRE(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_L2GRE) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_VXLAN(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_VXLAN) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_FLOW(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_FLOW) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_PORTS_GROUP(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_PORTS_GROUP) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

#define _SHR_MULTICAST_EXTENDER(_mc_index_)    \
         (((_SHR_MULTICAST_TYPE_EXTENDER) << _SHR_MULTICAST_TYPE_SHIFT)  | \
             (((_mc_index_) & _SHR_MULTICAST_ID_MASK) << _SHR_MULTICAST_ID_SHIFT))

/* BCM_MULTICAST_TYPE */
#define _SHR_MULTICAST_TYPE(_group_) \
    (((_group_) >> _SHR_MULTICAST_TYPE_SHIFT) & _SHR_MULTICAST_TYPE_MASK)

/*
 * Multicast replication structure and flags
 */
typedef struct { /* the structure represents a multicast replication */
    uint32 flags;       /* information on the replication */
    _shr_gport_t port;   /* destination */
    _shr_if_t encap1;    /* first encapsulation. */
    _shr_if_t encap2;    /* second encapsulation. */
} _shr_multicast_replication_t;

/* values for the flags field of _shr_multicast_replication_t */
#define _SHR_MUTICAST_REPLICATION_ENCAP2_VALID 1   /* determines if encap2 is used. */
#define _SHR_MUTICAST_REPLICATION_ENCAP1_L3_INTF 2 /* when encap2 is used, determines if encap1 is a routing interface or not. */

#endif /* _SHR_MULTICAST_H */
