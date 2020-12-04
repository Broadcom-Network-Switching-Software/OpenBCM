/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains QOS module definitions internal to the BCM library.
 */

#ifndef _BCM_INT_QOS_H
#define _BCM_INT_QOS_H

#include <bcm/qos.h>

#define _BCM_QOS_MAP_TYPE_ING_VFT_PRI_MAP    6
#define _BCM_QOS_MAP_TYPE_EGR_VFT_PRI_MAP    7
#define _BCM_QOS_MAP_TYPE_EGR_VSAN_PRI_MAP   8
#define _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP   9
#define _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP   10
#define _BCM_QOS_VFT_MAP_SHIFT               11
#ifdef BCM_HURRICANE3_SUPPORT
#define _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP    12
#define _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS      13
#endif

#define QOS_MAP_IS_FCOE(map_id)                                                \
  (  ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_VFT_PRI_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_VFT_PRI_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_VSAN_PRI_MAP)\
  )

#define QOS_MAP_IS_L2_VLAN_ETAG(map_id)                                        \
  (  ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) \
  || ((map_id >> _BCM_QOS_VFT_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) \
  )

#define QOS_FLAGS_ARE_FCOE(flags) \
  ( (flags & BCM_QOS_MAP_VFT) || (flags & BCM_QOS_MAP_VSAN) )

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_qos_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_qos_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_TRIDENT2_SUPPORT
extern int bcm_td2_qos_get_egr_vsan_hw_idx(int unit, int egr_map_id, 
                                            int *hw_index);
extern int bcm_td2_vsan_profile_to_qos_id(int unit, int profile_num, 
                                          int *egr_map);
extern int bcm_td2_qos_egr_etag_id2profile(int unit, int map_id, int *profile_num);
extern int bcm_td2_qos_egr_etag_profile2id(int unit, int profile_num, int *map_id);
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
extern int bcm_th_qos_egr_etag_id2profile(int unit, int map_id, int *profile_num);
extern int bcm_th_qos_egr_etag_profile2id(int unit, int profile_num, int *map_id);
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
extern void
_bcm_tr2_qos_id_update(int unit, uint32 dscp_map_id, uint32 new_dscp_map_id);
#endif

#if defined(BCM_HGPROXY_COE_SUPPORT)
extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_add(int unit,
                                                      bcm_qos_map_t *map);
extern int bcmi_xgs5_subport_egr_subtag_dot1p_map_delete(int unit,
                                                         bcm_qos_map_t *map);
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
extern int
_bcm_tr2_qos_egr_mpls_external_ref_add(int unit, int qos_map_id, int *hw_idx);
int
_bcm_tr2_qos_egr_mpls_external_ref_del(int unit, int hw_idx);
#endif
#endif	/* !_BCM_INT_QOS_H */
