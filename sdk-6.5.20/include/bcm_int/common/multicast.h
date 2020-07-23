/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains multicast definitions internal to the BCM library.
 */

#ifndef _BCM_INT_MULTICAST_H
#define _BCM_INT_MULTICAST_H
#include <shared/multicast.h>

#define _BCM_MULTICAST_TYPE_L2            _SHR_MULTICAST_TYPE_L2
#define _BCM_MULTICAST_TYPE_L3            _SHR_MULTICAST_TYPE_L3
#define _BCM_MULTICAST_TYPE_VPLS          _SHR_MULTICAST_TYPE_VPLS
#define _BCM_MULTICAST_TYPE_SUBPORT       _SHR_MULTICAST_TYPE_SUBPORT
#define _BCM_MULTICAST_TYPE_MIM           _SHR_MULTICAST_TYPE_MIM
#define _BCM_MULTICAST_TYPE_WLAN          _SHR_MULTICAST_TYPE_WLAN
#define _BCM_MULTICAST_TYPE_VLAN          _SHR_MULTICAST_TYPE_VLAN
#define _BCM_MULTICAST_TYPE_TRILL         _SHR_MULTICAST_TYPE_TRILL
#define _BCM_MULTICAST_TYPE_NIV           _SHR_MULTICAST_TYPE_NIV
#define _BCM_MULTICAST_TYPE_EGRESS_OBJECT _SHR_MULTICAST_TYPE_EGRESS_OBJECT
#define _BCM_MULTICAST_TYPE_L2GRE         _SHR_MULTICAST_TYPE_L2GRE
#define _BCM_MULTICAST_TYPE_VXLAN         _SHR_MULTICAST_TYPE_VXLAN
#define _BCM_MULTICAST_TYPE_FLOW          _SHR_MULTICAST_TYPE_FLOW
#define _BCM_MULTICAST_TYPE_PORTS_GROUP   _SHR_MULTICAST_TYPE_PORTS_GROUP
#define _BCM_MULTICAST_TYPE_EXTENDER      _SHR_MULTICAST_TYPE_EXTENDER
#define _BCM_MULTICAST_TYPE_ING_BMP       _SHR_MULTICAST_TYPE_ING_BMP
#define _BCM_MULTICAST_TYPE_EGR_BMP       _SHR_MULTICAST_TYPE_EGR_BMP


#define _BCM_MULTICAST_TYPE_SHIFT         _SHR_MULTICAST_TYPE_SHIFT
#define _BCM_MULTICAST_TYPE_MASK          _SHR_MULTICAST_TYPE_MASK

#define _BCM_MULTICAST_ID_SHIFT           _SHR_MULTICAST_ID_SHIFT
#define _BCM_MULTICAST_ID_MASK            _SHR_MULTICAST_ID_MASK

#define _BCM_MULTICAST_IS_SET(_group_)                  _SHR_MULTICAST_IS_SET(_group_)
#define _BCM_MULTICAST_GROUP_SET(_group_, _type_, _id_) _SHR_MULTICAST_GROUP_SET(_group_, _type_, _id_)
#define _BCM_MULTICAST_ID_GET(_group_)                  _SHR_MULTICAST_ID_GET(_group_)
#define _BCM_MULTICAST_TYPE_GET(_group_)                _SHR_MULTICAST_TYPE_GET(_group_)
#define _BCM_MULTICAST_IS_L2(_group_)                   _SHR_MULTICAST_IS_L2(_group_)
#define _BCM_MULTICAST_IS_L3(_group_)                   _SHR_MULTICAST_IS_L3(_group_)
#define _BCM_MULTICAST_IS_VPLS(_group_)                 _SHR_MULTICAST_IS_VPLS(_group_)
#define _BCM_MULTICAST_IS_SUBPORT(_group_)              _SHR_MULTICAST_IS_SUBPORT(_group_)
#define _BCM_MULTICAST_IS_MIM(_group_)                  _SHR_MULTICAST_IS_MIM(_group_)
#define _BCM_MULTICAST_IS_WLAN(_group_)                 _SHR_MULTICAST_IS_WLAN(_group_)
#define _BCM_MULTICAST_IS_VLAN(_group_)                 _SHR_MULTICAST_IS_VLAN(_group_)
#define _BCM_MULTICAST_IS_TRILL(_group_)                _SHR_MULTICAST_IS_TRILL(_group_)
#define _BCM_MULTICAST_IS_NIV(_group_)                  _SHR_MULTICAST_IS_NIV(_group_)
#define _BCM_MULTICAST_IS_EGRESS_OBJECT(_group_)        _SHR_MULTICAST_IS_EGRESS_OBJECT(_group_)
#define _BCM_MULTICAST_IS_L2GRE(_group_)                _SHR_MULTICAST_IS_L2GRE(_group_)
#define _BCM_MULTICAST_IS_VXLAN(_group_)                _SHR_MULTICAST_IS_VXLAN(_group_)
#define _BCM_MULTICAST_IS_FLOW(_group_)                 _SHR_MULTICAST_IS_FLOW(_group_)
#define _BCM_MULTICAST_IS_PORTS_GROUP(_group_)          _SHR_MULTICAST_IS_PORTS_GROUP(_group_)
#define _BCM_MULTICAST_IS_EXTENDER(_group_)             _SHR_MULTICAST_IS_EXTENDER(_group_)

#endif	/* !_BCM_INT_MULTICAST_H */
