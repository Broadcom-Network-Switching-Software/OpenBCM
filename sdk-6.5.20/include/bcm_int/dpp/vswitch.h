/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vswitch.h
 * Purpose:     vswitch internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_VSWITCH_H_
#define   _BCM_INT_DPP_VSWITCH_H_

#include <bcm/types.h>
#include <shared/swstate/sw_state.h>
#include <bcm_int/dpp/mim.h>

typedef struct _bcm_tr_vswitch_bookkeeping_s {
    int         initialized;        /* Set to TRUE when VSWITCH module initialized */
    PARSER_HINT_ARR SHR_BITDCL      *vsi_usage;
    int         vpn_detach_ports;  /* Set to 1 when VSWITCH module initialized */
} _bcm_dpp_vswitch_bookkeeping_t;


/*
 * vswitch definition
 */
#define _BCM_DPP_VPN_ID_MASK           0xffff
#define _BCM_DPP_VPN_BVID_MASK(_unit_) ((SOC_IS_JERICHO_A0(_unit_) | SOC_IS_JERICHO_B0(_unit_) | SOC_IS_QMX(_unit_)) ? 0x7000 : 0xf000)
#define _BCM_DPP_VPN_VID_MASK          0xfff

#define _BCM_DPP_VPN_IS_SET(_vpn_)    \
        ((_vpn_) != 0)

#define _BCM_DPP_VPN_ID_SET(_vpn_, _id_) \
    ((_vpn_) = ((_id_) & _BCM_DPP_VPN_ID_MASK))

#define _BCM_DPP_VPN_ID_GET(_vpn_) \
    ((_vpn_) & _BCM_DPP_VPN_ID_MASK)

#define _BCM_DPP_VPN_IS_CROSS_CONNECT_VPWS(_vpn) (_vpn == 0)


#define _BCM_DPP_VLAN_TO_BVID(_unit_, _vpn_) \
    ((_vpn_) | _BCM_DPP_VPN_BVID_MASK(_unit_))

#define _BCM_DPP_VLAN_IS_BVID(_unit_, _vpn_) \
  ((((_vpn_) & _BCM_DPP_VPN_BVID_MASK(_unit_)) == _BCM_DPP_VPN_BVID_MASK(_unit_)) & (MIM_IS_INIT(_unit_) | (!SOC_IS_JERICHO_A0(_unit_) & !SOC_IS_JERICHO_B0(_unit_) & !SOC_IS_QMX(_unit_))))

#define _BCM_DPP_BVID_TO_VLAN(_vpn_) \
    ((_vpn_) & _BCM_DPP_VPN_VID_MASK)

/*
 * given VPN check if it's MP
 */
#define _BCM_DDP_IS_MP_VPN(_vpn)  (_BCM_DPP_VPN_IS_SET(_vpn))
#define _BCM_DDP_IS_CROSS_CONNECT_P2P_VPN(_vpn) (_BCM_DPP_VPN_IS_CROSS_CONNECT_VPWS(_vpn))

/* map from MP VPN to VSI*/
#define _BCM_DDP_VPN_TO_VSI(vpn) ((_BCM_DDP_IS_MP_VPN(vpn))?vpn:SOC_PPC_VSI_P2P_SERVICE)

typedef enum _bcm_petra_vswitch_unknown_action_e {
   _bcm_petra_vswitch_unknown_action_drop=0,/* drop unknown da*/         
   _bcm_petra_vswitch_unknown_action_flood,
   _bcm_petra_vswitch_unknown_action_flood_per_action
} _bcm_petra_vswitch_unknown_action_t;


/* check if the given vsi/vpn exist */
int _bcm_dpp_vlan_check(int unit, int vsi);

#define _BCM_DPP_VSI_TYPE_NOF_BITS (8)

typedef enum _bcm_vsi_type_s {
    _bcmDppVsiTypeAny=0,       /* Any VPN */
    _bcmDppVsiTypeMpls=1,      /* MPLS VPN */
    _bcmDppVsiTypeVswitch=2,   /* VSWITCH VSI */
    _bcmDppVsiTypeMim=4,   /* MIM VSI */
    _bcmDppVsiTypeL2gre=8,   /* L2GRE VSI */
    _bcmDppVsiTypeVxlan=0x10,  /* VXLAN */
    _bcmDppVsiTypeAll=0x1F,    /* All */
    _bcmDppVsiTypeCount
} _bcm_vsi_type_e;


extern _bcm_dpp_vswitch_bookkeeping_t  _bcm_dpp_vswitch_bk_info[BCM_MAX_NUM_UNITS];

/* set/return bitmap of vsi usage */
int _bcm_dpp_vswitch_vsi_usage_get(int unit, int	vsi, _bcm_vsi_type_e *usage);
int _bcm_dpp_vswitch_vsi_usage_set(int unit, int	vsi, _bcm_vsi_type_e vswitch_usage);

/* check specific usage of the vsi */
int _bcm_dpp_vswitch_vsi_usage_type_set(int unit, int	vsi, _bcm_vsi_type_e type, uint8 used);
int _bcm_dpp_vswitch_vsi_usage_type_check(int unit, int	vsi, _bcm_vsi_type_e type, uint8 *used);

/* allocate VSI for specific usage
   - allocate VSI id if needed 
   - set the usage type. 
   - calling this API with different types over the same VSI-id should return ok. 
   - Remarks: 
        - replaced is optional, store if this call replace exist vsi of same type 
 */
int _bcm_dpp_vswitch_vsi_usage_alloc(int unit, int flags, _bcm_vsi_type_e type, bcm_vlan_t *vsi, uint8 *replaced);

/* dellocate VSI from specific usage
   - reset the usage type. 
   - dealloc if this was last usage of this VSI
 */
int _bcm_dpp_vswitch_vsi_usage_dealloc(int unit, _bcm_vsi_type_e type, bcm_vlan_t	vsi);



/* destroy vswitch and delete all ports */
int bcm_petra_vswitch_destroy_internal(int unit, bcm_vlan_t vsi, _bcm_vsi_type_e vsi_type, int is_port_delete);



/*
 * set VSI  flooding configuration
 */
int
bcm_petra_vswitch_vsi_set(
    int    unit,
    bcm_vlan_t    vsi,
	_bcm_petra_vswitch_unknown_action_t  action,
    int is_clear);

int
bcm_petra_vswitch_vsi_get(
    int    unit,
    bcm_vlan_t    vsi,
	_bcm_petra_vswitch_unknown_action_t  *action);


#endif /* _BCM_INT_DPP_VSWITCH_H_ */
