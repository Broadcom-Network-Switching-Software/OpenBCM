/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains generic VPN definitions internal to the BCM library.
 */

#ifndef _BCM_INT_VPN_H
#define _BCM_INT_VPN_H

#define _BCM_VPN_TYPE_L3        BCM_VPN_TYPE_MPLS_L3
#define _BCM_VPN_TYPE_VPWS      BCM_VPN_TYPE_MPLS_VPWS
#define _BCM_VPN_TYPE_VFI       BCM_VPN_TYPE_MPLS_VPLS

#define _BCM_VPN_SET(_vpn_, _type_, _id_) \
        do { \
            if (BCM_VPN_TYPE_MPLS_L3 == (_type_)) { \
                BCM_VPN_MPLS_L3_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPWS == (_type_)) { \
                BCM_VPN_MPLS_VPWS_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPLS == (_type_)) { \
                BCM_VPN_MPLS_VPLS_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_VXLAN == (_type_)) { \
                BCM_VPN_VXLAN_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_L2GRE == (_type_)) { \
                BCM_VPN_L2GRE_VPN_ID_SET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MIM == (_type_)) { \
                BCM_VPN_MIM_VPN_ID_SET(_vpn_, _id_); \
            } \
        } while (0)

#define _BCM_VPN_GET(_id_, _type_,  _vpn_) \
        do { \
            if (BCM_VPN_TYPE_MPLS_L3 == (_type_)) { \
                BCM_VPN_MPLS_L3_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPWS == (_type_)) { \
                BCM_VPN_MPLS_VPWS_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MPLS_VPLS == (_type_)) { \
                BCM_VPN_MPLS_VPLS_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_VXLAN == (_type_)) { \
                BCM_VPN_VXLAN_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_L2GRE == (_type_)) { \
                BCM_VPN_L2GRE_VPN_ID_GET(_vpn_, _id_); \
            } else if (BCM_VPN_TYPE_MIM == (_type_)) { \
                BCM_VPN_MIM_VPN_ID_GET(_vpn_, _id_); \
            } \
        } while (0)

#define _BCM_VPN_IS_L3(_vpn_) \
        (((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MPLS_L3) \
        && ((((_vpn_) >> 12) & 0xf) < BCM_VPN_TYPE_MPLS_VPWS))

#define _BCM_VPN_IS_VPLS(_vpn_) \
        ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MPLS_VPLS)

#define _BCM_VPN_IS_VPWS(_vpn_) \
        (((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MPLS_VPWS) \
        && ((((_vpn_) >> 12) & 0xf) < BCM_VPN_TYPE_MPLS_VPLS))

#define _BCM_VPN_IS_MIM(_vpn_) \
        ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_MIM)

#define _BCM_VPN_IS_L2GRE_ELINE(_vpn_) \
        ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_L2GRE)

#define _BCM_VPN_IS_L2GRE_ELAN(_vpn_) \
        ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_L2GRE)

#define _BCM_VPN_IS_VXLAN_ELINE(_vpn_) \
        ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_VXLAN)

#define _BCM_VPN_IS_VXLAN_ELAN(_vpn_) \
        ((((_vpn_) >> 12) & 0xf) >= BCM_VPN_TYPE_VXLAN)

#define _BCM_VPN_VFI_IS_SET(_vpn_) \
        (_BCM_VPN_IS_VPLS(_vpn_) \
        || _BCM_VPN_IS_MIM(_vpn_) \
        || _BCM_VPN_IS_L2GRE_ELINE(_vpn_) \
        || _BCM_VPN_IS_L2GRE_ELAN(_vpn_) \
        || _BCM_VPN_IS_VXLAN_ELINE(_vpn_) \
        || _BCM_VPN_IS_VXLAN_ELAN(_vpn_))

#define _BCM_VPN_IS_SET(_vpn_) \
        (_BCM_VPN_IS_L3(_vpn_) \
        || _BCM_VPN_IS_VPWS(_vpn_) \
        || _BCM_VPN_VFI_IS_SET(_vpn_))

#endif	/* !_BCM_INT_VPN_H */
