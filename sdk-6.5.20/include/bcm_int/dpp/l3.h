/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L3 Internal header
 */

#ifndef _BCM_INT_DPP_L3_H_
#define _BCM_INT_DPP_L3_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/l3.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <shared/swstate/sw_state.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>

#define _BCM_PETRA_L3_NOF_ITFS SOC_DPP_CONFIG(unit)->l3.nof_rifs
#define _BCM_PETRA_L3_ITF_ID_BASE (0)

/* Number of entries in the VridMyMacCam table */
#define _BCM_PETRA_L3_VRRP_CAM_MAX_NOF_ENTIRES(_unit)    (SOC_PPC_VRRP_CAM_MAX_NOF_ENTRIES(_unit))

/* Maximum number of VSIs that can be assigned to a vrrp entry. */
#define _BCM_PETRA_L3_VRRP_MAX_VSI(_unit)      SOC_PPC_VRRP_MAX_VSI(_unit)

#define DPP_DEFAULT_VRF 0
#define DPP_VRF_VALID(unit, _vrf) (((_vrf) >= 0) && (_vrf <  SOC_DPP_DEFS_GET(unit, nof_vrfs)))



/* Multiple mymac enabled modes. 
 *  Multiple mymac and VRRP can coexist on the same device, but sometimes the user might want to disable VRRP.
 *  For that reason, the multiple mymac enabled soc_dpp_config can be configured to 2 at device bringup.
 */

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_ENABLED_EXCLUSIVE                  2

#define _BCM_L3_MULTIPLE_MYMAC_ENABLED(_unit)  \
        (SOC_DPP_CONFIG(_unit)->l3.multiple_mymac_enabled)

#define _BCM_L3_MULTIPLE_MYMAC_EXCLUSIVE(_unit)  \
        (SOC_DPP_CONFIG(_unit)->l3.multiple_mymac_enabled == _BCM_PETRA_L3_MULTIPLE_MYMAC_ENABLED_EXCLUSIVE)


/* Multiple mymac modes. 
 *  Mode None                   : No protocol distinction and no use of mac masks.
 *  Mode IPv4 distinct          : No use of mac masks, distinction only between IPv4 and other protocols.
 *  Mode Protocol Group Active  : Full use of protocol groups for protocol distinction, full use of mac masks.
 */
#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_NONE                          0
#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IPV4_DISTINCT                 1
#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_ACTIVE         2

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(unit) (SOC_DPP_CONFIG(unit)->l3.multiple_mymac_mode)

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_IS_ACTIVE(_unit)  \
        (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(_unit) >= _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_ACTIVE)

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_IPV4_DISTINCT(_unit)   \
        (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(_unit) == _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IPV4_DISTINCT)

#define _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_NONE(_unit)   \
        (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(_unit) == _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_NONE)


/* Given a vsi, returns true if it's the vrrp's "configure all VSIs" vsi. */
#define _BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(_vsi) (_vsi == 0)

/* Given the api's l3 protocol group flags, returns the soc ppc flags. */
#define BCM_PETRA_L3_VRRP_TRANSLATE_API_FLAGS_TO_INTERNAL_PROTOCOL_GROUP_FLAGS(_src, _target)         \
    (_target) = 0;                                                                              \
    DPP_TRANSLATE_FLAG(_src, BCM_L3_VRRP_IPV4, _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4);   \
    DPP_TRANSLATE_FLAG(_src, BCM_L3_VRRP_IPV6, _target, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6);

/* Given the soc ppc l3 protocol group flags, returns the api flags. */
#define BCM_PETRA_L3_VRRP_TRANSLATE_PROTOCOL_GROUP_INTERNAL_FLAGS_TO_API(_src, _target)             \
    (_target) = 0;                                                                              \
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4, _target, BCM_L3_VRRP_IPV4);   \
    DPP_TRANSLATE_FLAG(_src, SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6, _target, BCM_L3_VRRP_IPV6);   




void Lpm_Cache_Test(int32 masklen, int nof_routes, int flags, int num);

/* given interface check if it's valid value
   Note: also for this should work also for uncoded FEC, (i.e. intf
 */

#define _BCM_PETRA_L3_ITF_IS_VALID_FEC(_unit, _l3_itf_)    \
        ((_l3_itf_) != 0)

#define L3_ACCESS sw_state_access[unit].dpp.bcm.l3

/* ECMP Object */
typedef int bcm_ecmp_t;

/*
 *  This structure contains information about a given unit's l3 state.
 */
typedef struct bcm_dpp_l3_state_s {
    int         init;
    int         urpf_mode;
    PARSER_HINT_ARR int *bcm_tunnel_intf_to_eep;         /* The associated EEP per interface */
} bcm_dpp_l3_state_t;

typedef struct bcm_dpp_l3_info_s {
  int used_intf;
  int used_vrf;
  int used_host;
  int used_route;
  PARSER_HINT_PTR bcm_dpp_l3_state_t *dpp_l3_state;
  uint32 vrrp_protocol_set_types[SOC_DPP_DEFS_MAX(VRRP_NOF_PROTOCOL_GROUPS)];
  uint32 vrrp_protocol_set_counts[SOC_DPP_DEFS_MAX(VRRP_NOF_PROTOCOL_GROUPS)];
} bcm_dpp_l3_info_t;

/* Internal structure to handle VRFs routing */
typedef struct _dpp_l3_vrf_route_s {
    int is_ipv6;                        /* TRUE indicates ipv6 address */
    bcm_vrf_t vrf_id ;                  /* Virtual router instance. */
    bcm_ip_t vrf_subnet;                /* IP subnet address (IPv4). */
    bcm_ip6_t vrf_ip6_net;              /* IP subnet address (IPv6). */
    bcm_ip_t vrf_ip_mask;               /* IP subnet mask (IPv4). */
    bcm_ip6_t vrf_ip6_mask;             /* IP subnet mask (IPv6). */
    bcm_if_t vrf_intf;                  /* L3 interface associated with route, (FEC-id) */
    uint32 l3a_flags;                   /* L3 flags */
    uint32 l3a_flags2;                  /* L3 flags2 */
    bcm_if_t encap_id;                  /* Encapsulation index. */
} _dpp_l3_vrf_route_t;


/* L3 module flags for soc layer */
#define _BCM_L3_FLAGS2_RAW_ENTRY                   (1 << 0)   /* insert entry with raw payload */ 

/* Init vrf structure */


extern bcm_dpp_l3_info_t *_dpp_l3_info[BCM_MAX_NUM_UNITS];


void
  __dpp_l3_vrf_route_t_init(_dpp_l3_vrf_route_t *data);

/* Common functions to handle ipv4 vrf route (UC and MC) */
int
  _bcm_ppd_frwrd_ipv4_vrf_route_add(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_vrf_route_get(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_vrf_route_remove(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

/* Common functions to handle ipv6 vrf route (UC and MC) */
int
  _bcm_ppd_frwrd_ipv6_vrf_route_add(
    int unit,
    _dpp_l3_vrf_route_t *info,
    SOC_PPC_FRWRD_DECISION_INFO *route_info
  );

int
  _bcm_ppd_frwrd_ipv6_vrf_route_get(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv6_vrf_route_remove(
    int unit,
    _dpp_l3_vrf_route_t *info
  );

/* Conversions */
int
  _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(
    int unit,
    SOC_SAND_PP_IPV6_ADDRESS *soc_sand_ipv6_addr,
    bcm_ip6_t *ipv6_addr
  );

int
  _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(
    int unit,
    bcm_ip6_t ipv6_addr,
    SOC_SAND_PP_IPV6_ADDRESS *soc_sand_ipv6_addr
  );

int
  _bcm_l3_intf_to_fec(
    int unit,
    bcm_if_t intf,
    SOC_PPC_FEC_ID *fec_index
  );

void
  _bcm_l3_fec_to_intf(
    int unit,
    SOC_PPC_FEC_ID fec_index,
    bcm_if_t *intf
  );

int
  _bcm_l3_encap_id_to_eep(
    int unit,
    int encap_id,
    int *eep
  );

int
  _bcm_ppd_frwrd_fec_entry_get(
    int unit,
    int fec_index,
    bcm_l3_egress_t *egr
  );

int
  _bcm_ppd_frwrd_fec_urpf_mode_traverse_set(
    int unit, 
    uint32 uc_rpf_mode
  );

int
  _bcm_ppd_rif_global_urpf_mode_traverse_set(
    int unit, 
    uint32 uc_rpf_mode
  );

int
  _bcm_ppd_frwrd_route_print_info(
    int unit,
    bcm_l3_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv6_route_print_info(
    int unit,
    bcm_l3_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_uc_route_print_info(
    int unit,
    bcm_l3_route_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_vrf_uc_route_print_info(
    int unit,
    bcm_l3_route_t *info
  );

int
  _bcm_ppd_frwrd_host_print_info(
    int unit,
    bcm_l3_host_t *info
  );

int
  _bcm_ppd_frwrd_ipv4_host_print_info(
    int unit,
    bcm_l3_host_t *info
  );

/* 
 * _bcm_ppd_frwrd_fec_entry_from_hw
 *  Get fec info from HW according to working/protected fec index
 */
int
_bcm_ppd_frwrd_fec_entry_from_hw(
   int unit,
   SOC_PPC_FEC_ID fec_ndx,
   SOC_PPC_FRWRD_FEC_PROTECT_TYPE
     *protect_type,
   SOC_PPC_FRWRD_FEC_ENTRY_INFO
     *working_fec,
   bcm_failover_t *failover_id
   );

int _bcm_tunnel_intf_ll_eep_set(int unit, bcm_if_t intf, int tunnel_eep, int ll_eep);

int _bcm_tunnel_intf_eep_set(int unit, bcm_if_t intf, int eep);

int _bcm_tunnel_intf_eep_get(int unit, bcm_if_t intf, int *eep);

int _bcm_tunnel_intf_is_tunnel(int unit, bcm_if_t intf, int *tunneled, int *eep, int *vsi, int *ll_eep);

/* get eedb index from router/tunnel intf */
int _bcm_l3_intf_eep_get(int unit, bcm_if_t intf, uint8 *tunneled, int *eep);

#ifdef BCM_88660_A0

int _bcm_l3_vrrp_mac_to_global_mac(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, uint32 flags, int vrid);

int _bcm_l3_vrrp_setting_get(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info);

int _bcm_l3_vrrp_setting_exists(int unit, int profile_index, int vlan);

int _bcm_l3_vrrp_setting_delete(int unit, int cam_index, SOC_PPC_VRRP_CAM_INFO *cam_info, int vlan);

int _bcm_l3_vrrp_setting_add(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int vlan);

int
_bcm_l3_sand_mac_is_vrrp_mac(SOC_SAND_PP_MAC_ADDRESS *address, uint8 *is_vrrp);


#endif /*BCM_88660_A0*/

#endif /* _BCM_INT_DPP_L3_H_ */

