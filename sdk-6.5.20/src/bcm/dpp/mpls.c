/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Mpls
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MPLS
#include <shared/bsl.h>
#include "bcm_int/common/debug.h"
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/dpp/pon.h>
#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/common/field.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/vlan.h>

#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_port.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>

#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/mbcm.h>

#include <soc/dcmn/dcmn_wb.h>

#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>

#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap.h>

#ifdef BCM_88660_A0
/* Reserved labels 0-15. */
/* For these lables (in Arad+) the check BOS handling may be set per label. */
#define _BCM_PETRA_MPLS_MAX_RESERVED_MPLS_LABEL 15

#endif /* BCM_88660_A0 */

/*
 *global SW state 
 */

_bcm_dpp_mpls_bookkeeping_t _bcm_dpp_mpls_bk_info[BCM_MAX_NUM_UNITS] = { {0} };

/*
 * when traversing the ILM, to perfrom action on each entry how many 
 * entries to return in each iteration 
 */
#define _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE (130)

#define _BCM_PETRA_MPLS_LABEL_FOR_PORT_TERMINATION (0x3)

#define _BCM_PETRA_MPLS_TUNNEL_LABEL_NOF 4

/* static varibales for traverse */
SOC_PPC_FRWRD_ILM_KEY *_bcm_mpls_traverse_ilm_keys[BCM_MAX_NUM_UNITS] = {NULL};
SOC_PPC_FRWRD_DECISION_INFO *_bcm_mpls_traverse_ilm_vals[BCM_MAX_NUM_UNITS] = {NULL};

#define _BCM_DPP_MPLS_LIF_TERM_DEFAULT_PROFILE (0)
#define _BCM_DPP_MPLS_IS_FORBIDDEN_PUSH_PROFILE(push_profile) \
        ((push_profile) == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val || \
         (push_profile) == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val || (push_profile) == 0) ? 1 : 0

/*number of push profile, should be same with  _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_COUNT(unit) */
#define NOF_MPLS_PUSH_PROFILE(unit) (SOC_IS_JERICHO_B0_AND_ABOVE(unit)? 15 : 7)

#define _BCM_DPP_MPLS_RESERVE_PROFILE_OP_LABEL_PER_PROFILE          0
#define _BCM_DPP_MPLS_RESERVE_PROFILE_OP_EL                         1    
#define _BCM_DPP_MPLS_RESERVE_PROFILE_OP_LABEL_PER_FWD_CODE         2
#define _BCM_DPP_MPLS_RESERVE_PROFILE_OP_PER_DATA_ENTRY             3

#define _BCM_DPP_MPLS_LABEL_BITS_NUM         (20)
#define _BCM_DPP_MPLS_LABEL_RESERVE_LABEL1_LSB         (20)
#define _BCM_DPP_MPLS_LABEL_RESERVE_LABEL2_LSB         (8)
#define _BCM_DPP_MPLS_UINT32_BITS_NUM                 (32)
#define _BCM_DPP_MPLS_BITS_MASK(n)                ((1<<(n))-1)

typedef enum _bcm_dpp_mpls_egress_action_type_s {
    _bcm_dpp_mpls_egress_action_type_push=0,
    _bcm_dpp_mpls_egress_action_type_swap,
    _bcm_dpp_mpls_egress_action_type_pop
} _bcm_dpp_mpls_egress_action_type_t;

/* Declaration */
int _bcm_petra_mpls_tunnel_switch_ilm_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info);

/*********** SW DBs translation function *******************/
int
_bcm_dpp_in_lif_mpls_term_match_add(int unit, bcm_mpls_tunnel_switch_t *mpls_term, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;
                       
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
                           
    gport_sw_resources.in_lif_sw_resources.criteria = mpls_term->action;
    gport_sw_resources.in_lif_sw_resources.flags = mpls_term->flags;
    gport_sw_resources.in_lif_sw_resources.port = mpls_term->port;
    gport_sw_resources.in_lif_sw_resources.match1 = mpls_term->label;
    gport_sw_resources.in_lif_sw_resources.match2 = mpls_term->ingress_if;
    gport_sw_resources.in_lif_sw_resources.match_tunnel = mpls_term->second_label;
    gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeMplsTunnel;
    gport_sw_resources.in_lif_sw_resources.gport_id = mpls_term->tunnel_id;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_in_lif_mpls_term_match_get(int unit, bcm_mpls_tunnel_switch_t *mpls_term, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

    mpls_term->action = gport_sw_resources.in_lif_sw_resources.criteria;
    mpls_term->flags = gport_sw_resources.in_lif_sw_resources.flags;
    mpls_term->port = gport_sw_resources.in_lif_sw_resources.port;
    mpls_term->label = gport_sw_resources.in_lif_sw_resources.match1;
    mpls_term->ingress_if = gport_sw_resources.in_lif_sw_resources.match2;
    mpls_term->second_label = gport_sw_resources.in_lif_sw_resources.match_tunnel;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_out_lif_mpls_tunnel_match_add(int unit, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeMplsTunnel;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_in_lif_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int lif, int allocate_outlif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

    gport_sw_resources.in_lif_sw_resources.criteria = mpls_port->criteria;
    gport_sw_resources.in_lif_sw_resources.flags = mpls_port->flags;
    if (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) {
        /* using BCM_MPLS_PORT_SERVICE_PRI_REPLACE to indicate ingress_only */
        gport_sw_resources.in_lif_sw_resources.flags |= BCM_MPLS_PORT_SERVICE_PRI_REPLACE;
    }

    gport_sw_resources.in_lif_sw_resources.port = mpls_port->port;
    if(mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL && mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL_PORT && mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL_VLAN ){
        gport_sw_resources.in_lif_sw_resources.match1 = mpls_port->match_vlan;
        gport_sw_resources.in_lif_sw_resources.match2 = mpls_port->match_inner_vlan;
    } else if(mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
         SOC_PPC_PORT   soc_ppc_port;
         SOC_PPC_PORT_INFO  port_info;
          int       core;
         BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, BCM_GPORT_LOCAL_GET(mpls_port->port), &soc_ppc_port, &core)));       
         soc_ppd_port_info_get(unit, core, soc_ppc_port, &port_info);
        gport_sw_resources.in_lif_sw_resources.match1 = mpls_port->match_label;
        gport_sw_resources.in_lif_sw_resources.match2 = port_info.vlan_domain;
    }
    else{
        gport_sw_resources.in_lif_sw_resources.match1 = mpls_port->match_label;
    }

    if (SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN ||
            mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
            gport_sw_resources.in_lif_sw_resources.match1 = mpls_port->match_label;
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) {
            gport_sw_resources.in_lif_sw_resources.match2 = mpls_port->match_vlan;
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
            gport_sw_resources.in_lif_sw_resources.match2 = mpls_port->match_inner_vlan;
        }
    }

    gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeMplsPort;
    gport_sw_resources.in_lif_sw_resources.gport_id =  mpls_port->mpls_port_id;
    gport_sw_resources.in_lif_sw_resources.additional_flags |= (allocate_outlif ? 0 : _BCM_DPP_INFLIF_MATCH_INFO_MPLS_PORT_NO_OUTLIF);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_out_lif_mpls_match_add(int unit, bcm_mpls_port_t *mpls_port, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeMplsPort;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_lif_match_get
 * Purpose:
 *      Obtain match information of a VLAN port
 * Parameters:
 *      unit    - (IN) Device Number
 *      mpls_port - (OUT) mpls port information
 *      vp  - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_mpls_match_get(int unit, bcm_mpls_port_t *mpls_port, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

     mpls_port->flags = gport_sw_resources.in_lif_sw_resources.flags;
     if (gport_sw_resources.in_lif_sw_resources.flags & BCM_MPLS_PORT_SERVICE_PRI_REPLACE) {
         /* using BCM_MPLS_PORT_SERVICE_PRI_REPLACE to indicate ingress_only */
         mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
         mpls_port->flags &= ~BCM_MPLS_PORT_SERVICE_PRI_REPLACE;
     }
     mpls_port->criteria = gport_sw_resources.in_lif_sw_resources.criteria;
     mpls_port->port = gport_sw_resources.in_lif_sw_resources.port;
     if(mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL && mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL_PORT && mpls_port->criteria != BCM_MPLS_PORT_MATCH_LABEL_VLAN ){
         mpls_port->match_vlan = gport_sw_resources.in_lif_sw_resources.match1;
         mpls_port->match_inner_vlan = gport_sw_resources.in_lif_sw_resources.match2;
     }
     else{
         mpls_port->match_label = gport_sw_resources.in_lif_sw_resources.match1;
     }


    if (SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN ||
            mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
            mpls_port->match_label = gport_sw_resources.in_lif_sw_resources.match1;
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) {
            mpls_port->match_inner_vlan = 0;
            mpls_port->match_vlan = gport_sw_resources.in_lif_sw_resources.match2;
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
            mpls_port->match_inner_vlan = gport_sw_resources.in_lif_sw_resources.match2;
            mpls_port->match_vlan = 0;
        }
    }

     mpls_port->mpls_port_id = gport_sw_resources.in_lif_sw_resources.gport_id;

     BCM_EXIT;
exit:
     BCMDNX_FUNC_RETURN;
}


/*
* returns if this port belongs to the given VPN 
* if port is not local and MP vpn, 
* then is_member cannot be resolved. 
*/
int _bcm_dpp_mpls_vpn_is_gport_member(
    int                  unit,
    bcm_vpn_t            vpn,
    bcm_gport_t          gport_id,
    int                  *is_member,
    int                  *is_local

 )
{
    SOC_PPC_LIF_ENTRY_INFO
        lif_entry_info;
    int mp;
    int vsi;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int is_local_port;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    mp = _BCM_DDP_IS_MP_VPN(vpn);

    BCMDNX_NULL_CHECK(is_member);

    *is_member = FALSE;
    
    rv = _bcm_dpp_gport_to_hw_resources(unit, gport_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, 
                                               &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_is_local(unit, gport_id, &is_local_port); 
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_local) {
        *is_local = is_local_port;
    }

    vsi  = _BCM_DPP_VPN_ID_GET(vpn);

    /* If the gport can be on any VPN and it's local to the unit, he is
       certainly a member */
    if (vsi == 0) {
        *is_member = is_local_port;
        BCM_EXIT;
    }

    if(mp) {
    
        /* from LIF get vsi */
        if (is_local_port || (!(BCM_GPORT_IS_VLAN_PORT(gport_id)))) {
            soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,gport_hw_resources.local_in_lif,&lif_entry_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
            /* is it pwe */
            if(lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE){
                if( lif_entry_info.value.pwe.vsid == vsi){
                    *is_member = TRUE;
                    BCM_EXIT;
                }
            }
            /* is it AC */
            else if(lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC){
                if( lif_entry_info.value.ac.vsid == vsi){
                    *is_member = TRUE;
                    BCM_EXIT;
                }
            }
        
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_mpls_check_init
 * Purpose:
 *      Check if MPLS is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int _bcm_dpp_mpls_check_init(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    if (!_bcm_dpp_mpls_bk_info[unit].initialized) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("not initialized")));
    } else {
        BCM_EXIT;
    }
exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_mpls_lock
 * Purpose:
 *      Take the MPLS software module Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int bcm_dpp_mpls_lock(
    int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_mpls_check_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    sal_mutex_take(MPLS_INFO((unit))->mpls_mutex, sal_mutex_FOREVER);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dpp_mpls_unlock
 * Purpose:
 *      Release  the MPLS software module Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

void bcm_dpp_mpls_unlock(
    int unit)
{
    int rv;

    rv = _bcm_dpp_mpls_check_init(unit);

    if (rv == BCM_E_NONE) {
        sal_mutex_give(MPLS_INFO((unit))->mpls_mutex);
    }
}


/*
 * Function:
 *      _bcm_petra_mpls_free_resource
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void _bcm_petra_mpls_free_resource(
    int unit,
    _bcm_dpp_mpls_bookkeeping_t * mpls_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (!mpls_info) {
        return;
    }

    if (_bcm_mpls_traverse_ilm_keys[unit] != NULL) {
       BCM_FREE(_bcm_mpls_traverse_ilm_keys[unit]);
    }

    if (_bcm_mpls_traverse_ilm_vals[unit] != NULL) {
       BCM_FREE(_bcm_mpls_traverse_ilm_vals[unit]);
    }

    BCMDNX_FUNC_RETURN_VOID;

}

/* 
 *   utilities function for port/vpn manipulation
 */



/* check if the given vpn is valid as l2 VPN */
STATIC int _bcm_dpp_l2_vpn_check(
    int unit,
    bcm_vpn_t vpn)
{
    int exist = 0;
    int vpn_id;

    BCMDNX_INIT_FUNC_DEFS;

    vpn_id = _BCM_DPP_VPN_ID_GET(vpn);

    /* Cross Connect VPN isn't allocated, but is valid for configuration */
    if (_BCM_DDP_IS_CROSS_CONNECT_P2P_VPN(vpn_id)) {
        return BCM_E_NONE;
    }

    if (vpn_id >= ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN ID is out of range"))); 
    }

    /* check if vsi/vpn */
    exist = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vpn_id);
    if (exist != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN does not exist")));
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_segment_routing_init(int unit)
{    
    int rv = BCM_E_NONE;
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    /* This value corresponds to the phase from which the outlif profile was received.
        In Jericho, the phases are 1-4. In Jericho plus they are 3,4,5,6.
        Note that in jericho plus, the outlif profile's indexing is phase_number + 1.*/
    if(SOC_IS_JERICHO_PLUS(unit)) {
        /* Outlif profiles 3,4 are used when MPLS label is placed in mpls tunnel.
            Outlif profiles 5,6 are used when FRR label is place in link layer.
            Setting any of the outlifs is enough to place the corresponding label (no need to set both outlifs) so we need to check all possible permutations.
            */
        /* ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_MPLS_FRR */
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 1/*entry*/,
                                                                   1 /*value*/,3/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 1/*entry*/,
                                                                   1 /*value*/,6/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 2/*entry*/,
                                                                   1 /*value*/,4/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 2/*entry*/,
                                                                   1 /*value*/,6/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 3/*entry*/,
                                                                   1 /*value*/,3/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 3/*entry*/,
                                                                   1 /*value*/,5/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 4/*entry*/,
                                                                   1 /*value*/,4/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 4/*entry*/,
                                                                   1 /*value*/,5/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "frr_label_in_mpls_tunnel", 0)) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 5/*entry*/,
                                                                       1 /*value*/,3/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS_FRR, 5/*entry*/,
                                                                       1 /*value*/,4/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_FRR */
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_FRR, 1/*entry*/,
                                                                   0 /*value*/,3/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_FRR, 1/*entry*/,
                                                                   0 /*value*/,4/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_FRR, 1/*entry*/,
                                                                   1 /*value*/,6/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_FRR, 2/*entry*/,
                                                                   0 /*value*/,3/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_FRR, 2/*entry*/,
                                                                   0 /*value*/,4/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_FRR, 2/*entry*/,
                                                                   1 /*value*/,5/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        /* ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_MPLS */
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS, 1/*entry*/,
                                                                   1 /*value*/,3/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS, 1/*entry*/,
                                                                   0 /*value*/,5/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS, 1/*entry*/,
                                                                   0 /*value*/,6/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS, 2/*entry*/,
                                                                   1 /*value*/,4/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS, 2/*entry*/,
                                                                   0 /*value*/,5/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_PLUS_MPLS, 2/*entry*/,
                                                                   0 /*value*/,6/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dscp_preserve_with_extend_label", 0)) {
            i = 4;
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER_OAMTS,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_IPV6_JER,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    }else {
        for (i = 1; i <= 4; i++) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER, i/*entry*/,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_SR, 1/*entry*/,
                                                                       1 /*value*/,1/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_SR, 1/*entry*/,
                                                                       1 /*value*/,2/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_SR, 2/*entry*/,
                                                                       1 /*value*/,2/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_MPLS_EXTENDED_JER_SR, 2/*entry*/,
                                                                       1 /*value*/,3/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);

        
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dscp_preserve_with_extend_label", 0)) {
            i = 2;
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER_OAMTS,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_IPV6_JER,
                                                                           1 /*value*/,i/*outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_cleanup(
    int unit)
{
    _bcm_dpp_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    rv = bcm_dpp_mpls_lock(unit);
    BCMDNX_IF_ERR_CONT(rv);

    /* Free software resources */
    (void) _bcm_petra_mpls_free_resource(unit, mpls_info);

    bcm_dpp_mpls_unlock(unit);

    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.explicit_null_support
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
        ) {

          /* Free IP-LIF-Dummy resource */
          if (bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id) == BCM_E_EXISTS) {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv);
          }
    }

    /* Destroy MPLS protection mutex. */
    if (MPLS_INFO((unit))->mpls_mutex) {
        sal_mutex_destroy(MPLS_INFO((unit))->mpls_mutex);
        MPLS_INFO((unit))->mpls_mutex = NULL;
    }
    
    /* Mark the state as uninitialized */
    mpls_info->initialized = FALSE;
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_egress_label_t_init not dispatchable */

int bcm_petra_mpls_exp_map_create(
    int unit,
    uint32 flags,
    int *exp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_exp_map_destroy(
    int unit,
    int exp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_exp_map_get(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_exp_map_set(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unavailable feature")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_exp_map_t_init not dispatchable */


int bcm_petra_mpls_init(
    int unit)
{
    _bcm_dpp_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int rv = BCM_E_NONE;
    SOC_PPC_MYMAC_TRILL_INFO mymac;    
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    uint32 i;
    SOC_PPC_MPLS_TERM_PROFILE_INFO term_profile;
    int prog_used;
    SOC_OCC_MGMT_OUTLIF_APP outlif_app;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    /* Forbid usage of roo_extension_label_encapsulation in Jericho Plus and above devices. */
    if (soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0) && SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                            (_BSL_BCM_MSG("soc property ROO_EXTENSION_LABEL_ENCAPSULATION can be configured for Jericho device only.")));
    }

    /* Forbid usage of mpls_egress_label_entropy_indicator_enable in Jericho Plus and above devices. */
    if (soc_property_get(unit, spn_MPLS_EGRESS_LABEL_ENTROPY_INDICATOR_ENABLE, 0) 
        && SOC_IS_JERICHO_PLUS(unit)
        && !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "el_eli_jr_mode", 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                            (_BSL_BCM_MSG("mpls_egress_label_entropy_indicator_enable is disabled for Jericho Plus and above devices.")));
    }

    /*
     * allocate resources
     */
    if (mpls_info->initialized) {
        rv = bcm_petra_mpls_cleanup(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*init warmbootable variables*/
    BCMDNX_ALLOC(_bcm_mpls_traverse_ilm_keys[unit], sizeof(SOC_PPC_FRWRD_ILM_KEY) *
                                        _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE,
                                        "mpls traverse info");
    if (_bcm_mpls_traverse_ilm_keys[unit] == NULL) {
        _bcm_petra_mpls_free_resource(unit, mpls_info);
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory error: couldnt allocate MPLS SW DB traverse ilm keys information")));
    }


    BCMDNX_ALLOC(_bcm_mpls_traverse_ilm_vals[unit], sizeof(SOC_PPC_FRWRD_DECISION_INFO) *
                  _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE, "mpls traverse info");
    if (_bcm_mpls_traverse_ilm_vals[unit] == NULL) {
        _bcm_petra_mpls_free_resource(unit, mpls_info);
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory error: couldnt allocate MPLS SW DB traverse ilm vals information")));
    }

    MPLS_INFO((unit))->mpls_mutex = sal_mutex_create("mpls_mutex");
    if (!MPLS_INFO((unit))->mpls_mutex) {
        _bcm_petra_mpls_free_resource(unit, mpls_info);
        BCM_EXIT;
    }

    SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&term_profile);
    
#ifdef BCM_88660_A0
    if (SOC_IS_JERICHO(unit)) {
        term_profile.check_bos = 0;
        term_profile.expect_bos = 0;

        if (!soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Soc property bcm886xx_mpls_termination_key_mode is not supported in Jericho devices.")));
        }
    }
    else {
        term_profile.check_bos = 1;

        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
          term_profile.check_bos = 0;
        }
    }
#endif /* BCM_88660_A0 */

    rv = _bcm_dpp_am_template_lif_term_init(unit, _BCM_DPP_MPLS_LIF_TERM_DEFAULT_PROFILE, &term_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_WARM_BOOT(unit)) {
        /* MPLS TP reserved mac address */
        if (soc_property_get(unit, spn_MPLS_TP_MYMAC_RESERVED_ADDRESS, 0)) {
            if (SOC_DPP_CONFIG(unit)->trill.mode) {
                /* Trill do not co exist with MPLS TP reserved addresss */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Trill and soc property MPLS_TP_MYMAC_RESERVED_ADDRESS do not coexist")));
            }
            
            soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &mymac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           
            /* MPLS TP mymac address 01-00-5E-90-00-00*/
            mymac.all_rbridges_mac.address[5] = 1;
            mymac.all_rbridges_mac.address[4] = 0;
            mymac.all_rbridges_mac.address[3] = 0x5e;
            mymac.all_rbridges_mac.address[2] = 0x90;
            mymac.all_rbridges_mac.address[1] = 0;
            mymac.all_rbridges_mac.address[0] = 0;
            soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &mymac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
   
    /* Create MPLS-Explicit-Dummy for reserved label explicit NULL */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->pp.explicit_null_support
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
        ) {
        bcm_mpls_tunnel_switch_t explicit_null_tunnel_switch;
        bcm_mpls_tunnel_switch_t_init(&explicit_null_tunnel_switch);
        BCM_GPORT_TUNNEL_ID_SET(explicit_null_tunnel_switch.tunnel_id, SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id);
        explicit_null_tunnel_switch.flags |= (BCM_MPLS_SWITCH_WITH_ID);
        explicit_null_tunnel_switch.flags |= BCM_MPLS_SWITCH_LOOKUP_NONE; /* only allocate LIF */
        explicit_null_tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
        rv = bcm_petra_mpls_tunnel_switch_create(unit, &explicit_null_tunnel_switch);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create Explicit-NULL-LIF")));
        }
    }

    /* Create MPLS-Dummy for mLDP support */
    if (SOC_DPP_CONFIG(unit)->pp.mldp_support
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
        ) {
        bcm_mpls_tunnel_switch_t mldp_tunnel_switch;
        for(i=0; i < 2; i++){
            bcm_mpls_tunnel_switch_t_init(&mldp_tunnel_switch);
            BCM_GPORT_TUNNEL_ID_SET(mldp_tunnel_switch.tunnel_id, SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[i]);
            mldp_tunnel_switch.flags |= (BCM_MPLS_SWITCH_WITH_ID);
            mldp_tunnel_switch.flags |= (BCM_MPLS_SWITCH_LOOKUP_NONE); /* only allocate LIF */
            mldp_tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
            rv = bcm_petra_mpls_tunnel_switch_create(unit, &mldp_tunnel_switch);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create Dummy-mLDP-LIF")));
            }
        }
    }

    /* Init MPLS special label termination */
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mpls_termination_spacial_labels_init,(unit)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv); 

    if ((SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp) {
        
        if ( (SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_INLIF_CTRL_EN
            && !SOC_WARM_BOOT(unit)
            ) {
            rv = arad_egr_prog_editor_prog_sel_inlif_profile_set(unit, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_JER, 1 /*value*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_inlif_profile_set(unit, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_JER_OAMTS, 1 /*value*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_inlif_profile_set(unit, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_IPV6_JER, 1 /*value*/);
            BCMDNX_IF_ERR_EXIT(rv);

            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dscp_preserve_with_extend_label", 0)) {
                rv = arad_egr_prog_editor_prog_sel_inlif_profile_set(unit, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER, 1 /*value*/);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = arad_egr_prog_editor_prog_sel_inlif_profile_set(unit, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER_OAMTS, 1 /*value*/);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = arad_egr_prog_editor_prog_sel_inlif_profile_set(unit, SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_IPV6_JER, 1 /*value*/);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        
        if ( (SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_OUTLIF_CTRL_EN
            && !SOC_WARM_BOOT(unit)
            ) {
            uint32 outlif_profile_index;

            /* the outlif profile to be selected can be from SOC property or fixed */
            outlif_profile_index = ((SOC_DPP_CONFIG(unit)->pp.routing_preserve_dscp>>SOC_DPP_DSCP_PRESERVE_OUTLIF_SEL_SHIFT)&SOC_DPP_DSCP_PRESERVE_OUTLIF_SEL_MASK)
                                       ? ARAD_EGR_PROG_OUTLIF_PROFILE_INDEX_ALL
                                       : (SOC_IS_JERICHO_PLUS(unit)?4:1);

            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_JER, 1 /*value*/,
                                                                  outlif_profile_index);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_JER_OAMTS, 1 /*value*/,
                                                                  outlif_profile_index);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_IPV6_JER, 1 /*value*/,
                                                                  outlif_profile_index);
            BCMDNX_IF_ERR_EXIT(rv);

            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dscp_preserve_with_extend_label", 0)) {
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER, 1 /*value*/,
                                                                      outlif_profile_index);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_JER_OAMTS, 1 /*value*/,
                                                                      outlif_profile_index);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_PRESERVING_DSCP_EXT_LABEL_IPV6_JER, 1 /*value*/,
                                                                      outlif_profile_index);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mixed_qos_model_enable", 0)) {
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,
                                                                      ARAD_EGR_PROG_EDITOR_PROG_COPY_DSCP_MIXED_QOS_MODEL, 1 /*value*/,
                                                                      outlif_profile_index);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

	/* Fix EVPN PRGE outlif profile data */
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
        ) {
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_EVPN,
                                                              ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI, 1 /*value*/,
                                                              _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*outlif-profile 1 or 3*/);
        BCMDNX_IF_ERR_EXIT(rv);

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.programs.prog_used.get(
            unit, ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI, &prog_used));
        if (prog_used != -1) {
            outlif_app = SOC_IS_JERICHO_PLUS(unit) ? SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL : SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL;
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, outlif_app,
                                                                       ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI,
                                                                       0 /*value*/,_BCM_DPP_MPLS_FRR_OUTLIF_PROFILE_INDEX(unit)/*outlif-profile 1 or 5*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.programs.prog_used.get(
            unit, ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI_FRR, &prog_used));
        if (prog_used != -1) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_EVPN,
                                                              ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI_FRR, 1 /*value*/,
                                                              _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*outlif-profile 1 or 3*/);
            BCMDNX_IF_ERR_EXIT(rv);

            outlif_app = SOC_IS_JERICHO_PLUS(unit) ? SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL : SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL;
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, outlif_app,
                                                                   ARAD_EGR_PROG_EDITOR_PROG_EVPN_BRIDGE_ESI_FRR,
                                                                   1 /*value*/,_BCM_DPP_MPLS_FRR_OUTLIF_PROFILE_INDEX(unit)/*outlif-profile 1 or 5*/);
            BCMDNX_IF_ERR_EXIT(rv);

        }

        /* Fix EVPN ROO PRGE outlif profile data in jericho plus:
           In this device, the roo evpn program needs evpn profile to be set, in order that this program not
           clash with segment routing */
        if (SOC_IS_JERICHO_PLUS(unit) && soc_property_get(unit, spn_MPLS_EGRESS_LABEL_EXTENDED_ENCAPSULATION_MODE, 0)) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_EVPN,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_EVPN_ROO_FRR, 1 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*outlif-profile 3*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.programs.prog_used.get(
        unit, ARAD_EGR_PROG_EDITOR_PROG_MPLS_FRR, &prog_used));
    if (prog_used != -1) 
    {
        outlif_app = SOC_IS_JERICHO_PLUS(unit) ? SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL : SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL;
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, outlif_app,
                                                                ARAD_EGR_PROG_EDITOR_PROG_MPLS_FRR,
                                                                1 /*value*/, 1/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.programs.prog_used.get(
        unit, ARAD_EGR_PROG_EDITOR_PROG_EVPN_ROO_FRR, &prog_used));
    if (prog_used != -1) {
        outlif_app = SOC_IS_JERICHO_PLUS(unit) ? SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL : SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL;
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, outlif_app,
                                                                ARAD_EGR_PROG_EDITOR_PROG_EVPN_ROO_FRR,
                                                                1 /*value*/,_BCM_DPP_MPLS_FRR_OUTLIF_PROFILE_INDEX(unit)/*outlif-profile 1 or 5*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "prge_ptagged_load", 0) ||
        soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,  "lsr_manipulation", 0)) {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "prge_ptagged_load", 0)) {

            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_PTAGGED, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_1 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_SERVICE_MODE_PTAGGED, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_2 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_NOT_PTAGGED, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_3 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_PTAGGED_MPLS_RAW, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_1 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_SERVICE_MODE_PTAGGED_MPLS_RAW, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_2 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_NOT_PTAGGED_MPLS_RAW, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_3 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);

        }
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,  "lsr_manipulation", 0)) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_LSR_MANIPULATION, ARAD_EGR_PROG_EDITOR_OUTLIF_TYPE_4 /*value*/,
                                                                  _BCM_DPP_MPLS_EGRESS_EVPN_OUTLIF_PROFILE_INDEX(unit)/*PWE-outlif-profile*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* ELI using PRGE */
    if ((SOC_IS_JERICHO_AND_BELOW(unit) || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "el_eli_jr_mode", 0)) 
         && soc_property_get(unit, spn_MPLS_EGRESS_LABEL_ENTROPY_INDICATOR_ENABLE, 0)) {

        rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_INDICATION,
                                                              ARAD_EGR_PROG_EDITOR_PROG_MPLS_ELI_JER, 1 /*value*/,
                                                              SOC_IS_JERICHO_PLUS(unit)?3:1/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);

        if (soc_property_get(unit, spn_MPLS_EGRESS_LABEL_EXTENDED_ENCAPSULATION_MODE, 0)) {
           rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                             ARAD_EGR_PROG_EDITOR_PROG_MPLS_ELI_JER, 2/*entry*/,
                                                                             1 /*value*/,SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile2*/);

            BCMDNX_IF_ERR_EXIT(rv);

           rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                             ARAD_EGR_PROG_EDITOR_PROG_MPLS_ELI_JER, 4/*entry*/,
                                                                             1 /*value*/,SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile2*/);
            
            BCMDNX_IF_ERR_EXIT(rv);

        }
    
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_mpls_entropy_label_on_non_bos_enable", 1)) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_NON_BOS_INDICATION,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_MPLS_NON_BOS_ELI_AT_ENC1_JER, 1 /*value*/,
                                                                  SOC_IS_JERICHO_PLUS(unit)?3:1/*outlif-profile1*/);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_INDICATION,
                                                                  ARAD_EGR_PROG_EDITOR_PROG_MPLS_NON_BOS_ELI_AT_ENC2_JER, 1 /*value*/,
                                                                  SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile2*/);
            BCMDNX_IF_ERR_EXIT(rv);

            if (soc_property_get(unit, spn_MPLS_EGRESS_LABEL_EXTENDED_ENCAPSULATION_MODE, 0)) {
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_NON_BOS_ELI_AT_ENC1_JER, 2/*entry*/,
                                                                                  1 /*value*/,SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile2*/);
                BCMDNX_IF_ERR_EXIT(rv);
                
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_NON_BOS_ELI_AT_ENC1_JER, 4/*entry*/,
                                                                                  1 /*value*/,SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile2*/);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_NON_BOS_ELI_AT_ENC2_JER, 2/*entry*/,
                                                                                  1 /*value*/,SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile1*/);
                BCMDNX_IF_ERR_EXIT(rv);
                
                rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                                   ARAD_EGR_PROG_EDITOR_PROG_MPLS_NON_BOS_ELI_AT_ENC2_JER, 4/*entry*/,
                                                                                  1 /*value*/,SOC_IS_JERICHO_PLUS(unit)?4:2/*outlif-profile1*/);
                BCMDNX_IF_ERR_EXIT(rv);
             }
        } 

    }

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_MPLS_EGRESS_LABEL_EXTENDED_ENCAPSULATION_MODE, 0)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_segment_routing_init(unit));
    }

    if (SOC_IS_JERICHO_PLUS(unit) 
        && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0) == 1) {
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_FOUR_ENCAP_BASIC, 1/*entry*/,
                                                                           1 /*value*/,3/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_FOUR_ENCAP_DATA_TUNNEL_BOS, 1/*entry*/,
                                                                           1 /*value*/,3/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_FOUR_ENCAP_DATA_TUNNEL_BOS, 2/*entry*/,
                                                                           1 /*value*/,3/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_FOUR_ENCAP_DATA_TUNNEL_ACT_SWAP, 1/*entry*/,
                                                                           1 /*value*/,3/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_FOUR_ENCAP_DATA_TUNNEL_NONBOS, 1/*entry*/,
                                                                           1 /*value*/,3/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_single_entry_set(unit, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,
                                                                           ARAD_EGR_PROG_EDITOR_PROG_MPLS_FOUR_ENCAP_DATA_TUNNEL_ACT_NOP, 1/*entry*/,
                                                                           1 /*value*/,3/*outlif-profile1*/);
        BCMDNX_IF_ERR_EXIT(rv);

    }


    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.test1) {
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_MIM_ENCAP,
                                                              ARAD_EGR_PROG_EDITOR_PROG_TEST1_ENCAPSULATION, 1 /*value*/,
                                                              (SOC_IS_JERICHO_PLUS(unit) ? (5) : (1))/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.test2) {
        rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_MIM_ENCAP,
                                                              ARAD_EGR_PROG_EDITOR_PROG_TEST2, 1 /*value*/,
                                                              (SOC_IS_JERICHO_PLUS(unit) ? (5) : (1))/*outlif-profile*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }    

    if (SOC_IS_JERICHO(unit)) {
        /* IPv6 Tunnel */
        if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL,
                                                               ARAD_EGR_PROG_EDITOR_PROG_IPV6_TUNNEL, 1 /*value*/,
                                                               SOC_IS_JERICHO_PLUS(unit)? 3 : 1 /*outlif-profile3*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (SOC_IS_JERICHO_PLUS(unit)) {
        /* IPv4 Tunnel TOR */
        if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_encap_mode == SOC_DPP_IP4_TUNNEL_ENCAP_MODE_LARGE) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_IPV4_TUNNEL_TOR,
                                                               ARAD_EGR_PROG_EDITOR_PROG_IPV4_TUNNEL_TOR, 1 /*value*/,
                                                               4 /*outlif-profile4*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* IPv6 Tunnel TOR */
        if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE) {
            rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL_TOR,
                                                               ARAD_EGR_PROG_EDITOR_PROG_IPV6_TUNNEL, 1 /*value*/,
                                                               3 /*outlif-profile3*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Mark the state as initialized */
    mpls_info->initialized = TRUE;

    /* Configure default actions for QAX & above:
        1) Allocate dummy push profiles 8,9 (these won't be used as they are pointed by swap and pop commands).
           In QAX, the push profile range grew to 1-15. (Also for Jericho B0)
        2) Configure the null value register via soc property.
        3) Set pipe mode register to be "exp set".
        4) Allocate initial additional push profiles (EL and ELI).
    */
    if (!SOC_WARM_BOOT(unit) && SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
        uint32 dummy_profiles_array[2];
        uint32 null_value = 0;

        /* 1 */
        dummy_profiles_array[0] = ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val;
        dummy_profiles_array[1] = ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val;

        rv = _bcm_mpls_tunnel_push_profile_alloc_dummy_profiles(unit,dummy_profiles_array,2);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 2 */
        null_value = soc_property_get(unit, spn_MPLS_ENCAP_INVALID_VALUE, 0);
        soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_dpp_pp_eg_encap_null_value_set,(unit, &null_value)));
        BCMDNX_IF_ERR_EXIT(soc_sand_rv); 
        
        if (SOC_IS_JERICHO_PLUS(unit)) {

            /* 3 */
            soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set(unit,0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* 4 */
            rv = _bcm_mpls_tunnel_initial_additional_push_profiles_alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);

        }



    }

    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.explicit_null_support
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
        ) {

        bcm_tunnel_terminator_t tunnel_term_ip_lif_dummy;
        int i = 0;
        bcm_gport_t ttl0_ttl_tunnel_id[2];

        /* Create IP-LIF-Dummy */
        bcm_tunnel_terminator_t_init(&tunnel_term_ip_lif_dummy);
        tunnel_term_ip_lif_dummy.type = bcmTunnelTypeIpAnyIn4; /* this is IPv4 termination */
        tunnel_term_ip_lif_dummy.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID);

        ttl0_ttl_tunnel_id[0] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "logical_interface_for_explicit_null_ttl0", 0);
        ttl0_ttl_tunnel_id[1] = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "logical_interface_for_explicit_null_ttl1", 0);
 
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_ip_lif_dummy.tunnel_id, SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id);
	for (i = 2;i < 256; i++)
	{
	    BCM_L3_ITF_SET(tunnel_term_ip_lif_dummy.if_class,BCM_L3_ITF_TYPE_LIF,i);
	    rv = bcm_petra_tunnel_terminator_create(unit, &tunnel_term_ip_lif_dummy);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create NULL_LABEL-LIF-Dummy")));
            }
	}

	for (i = 0;i < 2; i++)
	{
	    BCM_L3_ITF_SET(tunnel_term_ip_lif_dummy.if_class,BCM_L3_ITF_TYPE_LIF, i);
            BCM_GPORT_TUNNEL_ID_SET(tunnel_term_ip_lif_dummy.tunnel_id, (ttl0_ttl_tunnel_id[i] ? ttl0_ttl_tunnel_id[i] : SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id));
	    rv = bcm_petra_tunnel_terminator_create(unit, &tunnel_term_ip_lif_dummy);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create NULL_LABEL-LIF-Dummy")));
            }
	}
    }
    /* For MPLS port termination - adding indexed entries(label=0x3) to SEM A + B using MPLS tunnel create  */
    if (SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id
#ifdef BCM_WARM_BOOT_SUPPORT
        && !SOC_WARM_BOOT(unit)
#endif
       ) {
        bcm_mpls_tunnel_switch_t port_termination_tunnel_switch;

        bcm_mpls_tunnel_switch_t_init(&port_termination_tunnel_switch);
        BCM_GPORT_TUNNEL_ID_SET(port_termination_tunnel_switch.tunnel_id, SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id);
        port_termination_tunnel_switch.flags |= (BCM_MPLS_SWITCH_WITH_ID);
        port_termination_tunnel_switch.label = _BCM_PETRA_MPLS_LABEL_FOR_PORT_TERMINATION;
        port_termination_tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
        if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
            BCM_MPLS_INDEXED_LABEL_SET(port_termination_tunnel_switch.label, _BCM_PETRA_MPLS_LABEL_FOR_PORT_TERMINATION, 1);
        }
        rv = bcm_petra_mpls_tunnel_switch_create(unit, &port_termination_tunnel_switch);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create Port termination LIF")));
        }

    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Check if given Push Profile was already allocated. 
 * If it was, return it's attributes. 
 */
int _bcm_dpp_mpls_port_push_profile_get_internal(
    int unit,
    bcm_mpls_port_t *   mpls_port,
    int * is_allocated)
{
    int rv = BCM_E_NONE;
    uint8 has_cw;
    uint32 push_profile;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mpls_egress_label_t_init(&mpls_port->egress_label);

    /* Check if the push profile is encoded by the user, if so remove encoding */
    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port->mpls_port_id)) {
        push_profile = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(mpls_port->mpls_port_id);
    }
    else {
        push_profile = mpls_port->mpls_port_id;
    }

    rv = _bcm_dpp_am_template_mpls_push_profile_is_profile_allocated(unit, push_profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (*is_allocated) {
        /* Get information from HW: egress label and has_cw */
        rv =  _bcm_mpls_tunnel_push_profile_info_get(unit,
                                                     push_profile,
                                                     &has_cw,
                                                     &mpls_port->egress_label);
        BCMDNX_IF_ERR_EXIT(rv);

        mpls_port->flags |= has_cw ? BCM_MPLS_PORT_CONTROL_WORD : 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Push Profile allocation - verify
 */
int _bcm_dpp_mpls_port_push_profile_alloc_verify(
    int unit,
    bcm_mpls_port_t *   mpls_port)
{
    uint32 legal_flags;

    BCMDNX_INIT_FUNC_DEFS;

    if (!(mpls_port->flags & BCM_MPLS_PORT_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("push profile can be allocated only WITH_ID")));
    }



    /*checking input parameters*/
    legal_flags = BCM_MPLS_PORT_WITH_ID|BCM_MPLS_PORT_CONTROL_WORD|BCM_MPLS_PORT_REPLACE;
    if (mpls_port->flags & ~legal_flags)  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported flag set. Supported flags: BCM_MPLS_PORT_WITH_ID, BCM_MPLS_PORT_REPLACE, BCM_MPLS_PORT_CONTROL_WORD\n")));
    }


exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * allocate Push Profile
 */
int _bcm_dpp_mpls_port_push_profile_alloc(
    int unit,
    bcm_mpls_port_t *   mpls_port)
{
    int rv = BCM_E_NONE;
    uint8 has_cw, update=0;
    uint32 push_profile;
    bcm_mpls_port_t mpls_port_dummy;
    int is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    /* Check if the push profile is encoded by the user, if so remove encoding */
    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port->mpls_port_id)) {
        push_profile = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(mpls_port->mpls_port_id);
    }
    else {
        push_profile = mpls_port->mpls_port_id;
    }

    /* In arad and above devices, push profile values 8,9 are reserved for swap and pop commands.
       Valid values are 1-7 for all arad and above devices, plus 10-15 for QAX device. */
    if (_BCM_DPP_MPLS_IS_FORBIDDEN_PUSH_PROFILE(push_profile)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Push command values 8,9 are reserved for swap and pop actions. Value 0 is not in usage.")));
    }

    rv = _bcm_dpp_mpls_port_push_profile_alloc_verify(unit, mpls_port);
    BCMDNX_IF_ERR_EXIT(rv);

    if (mpls_port->flags & BCM_MPLS_PORT_REPLACE){
       update = 1;
    }

    bcm_mpls_port_t_init(&mpls_port_dummy);
    mpls_port_dummy.mpls_port_id = mpls_port->mpls_port_id;

    /* Check that this profile does not exist yet */
    rv = _bcm_dpp_mpls_port_push_profile_get_internal(unit, &mpls_port_dummy, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (update && !is_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given push profile was not allocated yet.")));
    }

    if (!update && is_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Given push profile was already allocated.")));
    }

    has_cw = (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) ? 1:0;

    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                                           &push_profile,
                                                           has_cw,
                                                           &mpls_port->egress_label,
                                                           FALSE,
                                                           TRUE,
                                                           update,
                                                           TRUE,
                                                           NULL,
                                                           0));

    /* Encode the push profile according to subtype */
    BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_SET(mpls_port->mpls_port_id, push_profile);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, mpls_port->mpls_port_id);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get Push Profile
 */
int _bcm_dpp_mpls_port_push_profile_get(
    int unit,
    bcm_mpls_port_t *   mpls_port)
{
    int rv = BCM_E_NONE;
    int is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    rv =_bcm_dpp_mpls_port_push_profile_get_internal(unit, mpls_port, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given push profile was not allocated.")));
    }

    mpls_port->flags2 |= BCM_MPLS_PORT2_TUNNEL_PUSH_INFO;

    exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* mpls_port_add api can be used for push profile allocation.
       In this case WITH_ID flag should be set as well as push profile sub type bit in mpls_port_id */
    if (mpls_port->flags2 & BCM_MPLS_PORT2_TUNNEL_PUSH_INFO) {
        rv = _bcm_dpp_mpls_port_push_profile_alloc(unit, mpls_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_mpls_port_add(unit, vpn, mpls_port));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * delete gport including
 * - allocated resources 
 * - SW state 
 * - HW state 
 */
int _bcm_mpls_port_delete(
   int                  unit,
   bcm_vpn_t            vpn,
   bcm_gport_t          port_id
   ) {
    int is_local, global_lif;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    uint32 removed_lif;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    uint8 remote_lif = 0;
    int is_ingress = TRUE, is_egress = TRUE;
    SOC_PPC_L2_LIF_PWE_INFO pwe_info;
    uint8 found;
    _bcm_dpp_gport_parse_info_t gport_parse_info;
    _bcm_lif_type_e lif_usage;
    int is_last_dummy;
    int push_profile = 0;
    bcm_mpls_port_t      mpls_port;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint8 remove_fec;
    SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO  pwe_additional_info;
    int status = BCM_E_NOT_FOUND;
    uint64 additinal_data;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.mpls_port_id = port_id;
    rv = bcm_petra_mpls_port_get(unit, vpn, &mpls_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /*
     * free HW resources
     *  - LIF 
     *  - EEP if allocated
     *  - FEC if protected gport
     * free SW state 
     *  - gport to info if not protected gport 
     */

    /* Get the gport type */
    rv = _bcm_dpp_gport_parse(unit, port_id, &gport_parse_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Remote LIF existance should be checked prior to the FEC removal */
    rv = _bcm_dpp_gport_l2_intf_gport_has_remote_destination(unit, port_id, &remote_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    if (remote_lif) {
        /* get FEC used for this gport */
        rv = _bcm_dpp_gport_to_hw_resources(unit, port_id, _BCM_DPP_GPORT_HW_RESOURCES_FEC,
                                            &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        /* get LIF + FEC used for this gport */
        rv = _bcm_dpp_gport_to_hw_resources(unit, port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_FEC,
                                            &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (gport_hw_resources.fec_id == -1 && 
        gport_hw_resources.global_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID && gport_hw_resources.global_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {

        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MPLS PORT does not exist")));
    }

    /* Check if fec is allocated */
    if ((BCM_GPORT_SUB_TYPE_IS_PROTECTION(port_id) || BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(port_id)) && (gport_hw_resources.fec_id != -1)) {
        status = bcm_dpp_am_fec_is_alloced(unit, 0 /*flags*/, 1 /*size*/, gport_hw_resources.fec_id);
        if (status == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MPLS PORT does not exist")));
        }
        else if (status != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("MPLS PORT internal error in bcm_dpp_am_fec_is_alloced")));
        }
    }

    global_lif = gport_hw_resources.global_in_lif;

    rv = _bcm_dpp_gport_is_local(unit, port_id, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);


    is_ingress = (gport_hw_resources.local_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
    is_egress = (gport_hw_resources.local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;


    /* Get LIF type and update global LIF in case needed */
    if (is_egress) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, gport_hw_resources.local_out_lif, NULL, &lif_usage));
        global_lif = gport_hw_resources.global_out_lif;
    } else if (is_ingress) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, gport_hw_resources.local_in_lif, -1, &lif_usage, NULL));
    }

    /* Verify the LIF matches the gport type */
    if (is_ingress || is_egress) {
        if (lif_usage == _bcmDppLifTypeAny) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MPLS PORT does not exist")));
        }
        if (BCM_GPORT_IS_MPLS_PORT(port_id) && (lif_usage != _bcmDppLifTypeMplsPort)) { /* PWE */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("LIF is not a PWE")));
        }
    }

    /* if FEC used for this gport - remove it */
    if (gport_hw_resources.fec_id != -1) {

        /* Free the allocation for the FEC */
        /* In case of a Protection FEC, the removal should be done only if it's the working FEC.
           In this scenario, the user should remove the protection FEC before removing the working FEC */
        remove_fec = TRUE;
        if (!is_ingress && !is_egress) { /* Forwarding group */
            rv = bcm_dpp_am_fec_dealloc(unit, 0 /*flags*/, 1 /*size*/, gport_hw_resources.fec_id);
        } else if (is_ingress && is_egress && _BCM_PPD_GPORT_IS_WORKING_PORT_ID(gport_hw_resources.fec_id)) {
            rv = bcm_dpp_am_fec_dealloc(unit, 0 /*flags*/, 2 /*size*/, gport_hw_resources.fec_id);
        } else {
            /* In case of protection port or ingress/egress only, we don't want to remove the fec. It will be removed with the working port. */
            remove_fec = FALSE;
        }
        BCMDNX_IF_ERR_EXIT(rv);

        if (remove_fec) {
            /* Reset the entry in the HW */
            soc_sand_rv = soc_ppd_frwrd_fec_remove(unit, gport_hw_resources.fec_id);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


    /* if there is no protection remove gport hash info*/
    /* No protection can be in case fec_id is invalid or fec_id given is the destination tunnel and not the protected, */
    /* or if gport is a vlan port of type forward */
    /* In case of MPLS PORT we need to remove vlan information if not egress. */
    if ((!remote_lif && is_ingress && (gport_hw_resources.fec_id == -1  || (gport_parse_info.type != _bcmDppGportParseTypeProtectedFec))) ||
        (is_ingress && !is_egress)) {
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           (sw_state_htb_key_t) &(port_id),
                                           (sw_state_htb_data_t) &phy_port,
                                           TRUE); /* remove */

        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {

            /* in case it is not found, then only the following case is exception nothing to worry */
            if (!(rv == BCM_E_NOT_FOUND &&
                  is_ingress && is_egress && gport_parse_info.type == _bcmDppGportParseTypeEncap)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "error(%s) Updating Gport Dbase (0x%x)\n"),
                           bcm_errmsg(rv), port_id));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        if (!GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
            if (phy_port.type == _bcmDppGportResolveTypeFecVC) {
                /* In case of FEC_VC push profile is allocated */
                push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(phy_port.encap_id);
            }
        }
    }

    /* read match criteria for this LIF */
    if (!remote_lif) {
        if (is_ingress) {
            if (SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
                if (mpls_port.criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
                    pwe_additional_info.inner_vid_valid = 0x1;
                    pwe_additional_info.inner_vid = mpls_port.match_inner_vlan;
                }
                if (mpls_port.criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) {
                    pwe_additional_info.outer_vid_valid = 0x1;
                    pwe_additional_info.outer_vid = mpls_port.match_vlan;
                }
            } else if(mpls_port.criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
		_bcm_dpp_gport_sw_resources gport_sw_resources;

		if (is_ingress) {
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, gport_hw_resources.local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
		}
                pwe_additional_info.vlan_domain_valid = 1;
                pwe_additional_info.vlan_domain = gport_sw_resources.in_lif_sw_resources.match2;
            }

            /* termination profile handling */
            BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                                                          gport_hw_resources.local_in_lif,
                                                                          0x0, /* Default */
                                                                          1, /* is_pwe_lif*/
                                                                          0,
                                                                          NULL));
            if (!(mpls_port.flags & BCM_MPLS_PORT_WITH_GAL)) {
                /* In GAL case, not removing the lif table entry */
                soc_sand_rv = soc_ppd_l2_lif_pwe_get(unit, mpls_port.match_label, &removed_lif, &pwe_additional_info, &pwe_info, &found);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (!found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF entry not found")));
                }
            } else {
                removed_lif = gport_hw_resources.local_in_lif;
            }

            if (removed_lif == gport_hw_resources.local_in_lif) {
                if (DPP_IS_FLAG_SET(mpls_port.flags2, BCM_MPLS_PORT2_INGRESS_WIDE))
                {
                    /* Call the SOC API for clear the additional data */
                    COMPILER_64_SET(additinal_data, 0, 0);
                    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_set,(unit, removed_lif, TRUE, additinal_data, TRUE)));
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                soc_sand_rv = soc_ppd_l2_lif_pwe_remove(unit, mpls_port.match_label, TRUE/*remove from SEM*/, (mpls_port.flags & BCM_MPLS_PORT_WITH_GAL), &removed_lif, &pwe_additional_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                removed_lif = (uint32)gport_hw_resources.local_in_lif;
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "When destroy LIF expected LIF and given LIF must be equal. Otherwise SEM entry won't be deleted.\n")));
                soc_sand_rv = soc_ppd_l2_lif_pwe_remove(unit, mpls_port.match_label, FALSE/*don't remove from SEM*/, (mpls_port.flags & BCM_MPLS_PORT_WITH_GAL), &removed_lif, &pwe_additional_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

        /* Free push profile */
        if (mpls_port.egress_label.label != BCM_MPLS_LABEL_INVALID) {
            /* Push profile is allocated - need to free */
            if (is_ingress && !is_egress) {
                /* In case of ingress only take push profile from SW DB */
                if (push_profile != 0) {
                    rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, push_profile, &is_last_dummy);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
            else {
                rv = _bcm_petra_mpls_tunnel_free_push_profile(unit, gport_hw_resources.global_out_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        if (is_egress) {
            /* Clear the egress lif. */
            rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP, gport_hw_resources.local_out_lif);
            BCM_SAND_IF_ERR_EXIT(rv);
        }

        /* Lif table deallocation */
        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif, gport_hw_resources.local_in_lif, gport_hw_resources.local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);

        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if (is_egress && is_ingress) {
                rv = bcm_dpp_am_l2_vpn_pwe_eep_dealloc(unit, gport_hw_resources.local_out_lif);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = bcm_dpp_am_l2_vpn_pwe_lif_dealloc(unit, gport_hw_resources.local_in_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (is_egress && !is_ingress) {
                rv = bcm_dpp_am_mpls_eep_dealloc(unit, gport_hw_resources.local_out_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (!is_egress && is_ingress) {
                /* Deallocating only InLif */
                rv = bcm_dpp_am_l3_lif_dealloc(unit, gport_hw_resources.local_in_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }


        if (is_ingress) {
            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, gport_hw_resources.local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        if (is_egress) {
            /* Remove entry from SW DB */
            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, gport_hw_resources.local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
            BCMDNX_IF_ERR_EXIT(rv); 
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_port_delete(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t mpls_port_id)
{
    int is_member, is_local;
    uint32 push_profile;
    int is_last_dummy;
    bcm_mpls_port_t mpls_port_dummy;
    uint8 remote_lif;
    int is_allocated;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* mpls_port_delete api can be used to free push profile allocated earlier by mpls_port_add.
       In this case push profile sub type bit in mpls_port_id should be set */
    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port_id)) {

        mpls_port_dummy.mpls_port_id = mpls_port_id;

        rv =_bcm_dpp_mpls_port_push_profile_get_internal(unit, &mpls_port_dummy, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_allocated) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given push profile was not allocated.")));
        }

        push_profile = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
        push_profile = BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE_GET(push_profile);

        rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                         push_profile,
                                                         &is_last_dummy);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }
       
    if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(mpls_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY) 
		|| BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port_id)) {
        if (_BCM_DPP_VPN_IS_SET(vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For egress and fec objects, vpn parameter must be unset")));
        }
    }

    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);


    if (BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id) == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS port Id")));
    }
    
    /* Remote LIF existance should be checked prior to the FEC removal */
    rv = _bcm_dpp_gport_l2_intf_gport_has_remote_destination(unit, mpls_port_id, &remote_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!remote_lif) {
        rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                            &gport_hw_resources);    
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (remote_lif || gport_hw_resources.local_in_lif == BCM_GPORT_INVALID) {
        is_member = 1;
        is_local = 1;
    }
    else {
        if (gport_hw_resources.local_in_lif != BCM_GPORT_INVALID){
            _bcm_lif_type_e lif_usage;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, gport_hw_resources.local_in_lif, -1, &lif_usage, NULL));
            if (lif_usage == _bcmDppLifTypeAny) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MPLS PORT does not exist")));
            }
        }
        /* check if the port is member in the vpn */
        rv = _bcm_dpp_mpls_vpn_is_gport_member(unit, vpn, mpls_port_id, &is_member,
                                          &is_local);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!is_member && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "check_mpls_port_vpn", 0)) {
        BCMDNX_ERR_EXIT_MSG(_SHR_E_PARAM, (_BSL_BCM_MSG("MPLS PORT does not belong to VPN")));
    }

    /* if member then remove it */
    /* if not local also remove it, will remove only SW state */
    if (is_member || !is_local) {
        rv = _bcm_mpls_port_delete(unit, vpn, mpls_port_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_delete_all(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    int iter, in_lif_val;
    int port_val;
    int vsi;
    SOC_PPC_LIF_ID local_lif_index_drop; 
    SOC_PPC_LIF_ID local_lif_index_simple;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    VLAN_ACCESS.vlan_info.local_lif_index_drop.get(unit, &local_lif_index_drop); 
    VLAN_ACCESS.vlan_info.local_lif_index_simple.get(unit, &local_lif_index_simple); 

    for (iter = 0; iter != -1;) {
        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit, vpn, &iter, &port_val,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if (port_val == BCM_GPORT_INVALID) {
            break;
        }

        /* Delete port only if it's an mpls port. */
        if (!BCM_GPORT_IS_MPLS_PORT(port_val)) {
            continue;
        }

        /* Don't delete default In-LIF */
        in_lif_val = iter - 1;
        if ((in_lif_val == local_lif_index_drop) ||
            (in_lif_val == local_lif_index_simple)) {
            continue;
        }

        /* fill next port */
        rv = _bcm_mpls_port_delete(unit, vpn, port_val);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    int port_val;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    _bcm_dpp_gport_resolve_type_e gport_type;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int encap_id;
    SOC_PPC_L2_LIF_PWE_INFO  pwe_info;
    uint8 found;
    uint32 lif_idx;
    int global_lif=_BCM_GPORT_ENCAP_ID_LIF_INVALID;
    int global_lif_next_eep = 0;
    uint8 is_ingress, is_egress;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    SOC_PPC_MPLS_TERM_PROFILE_INFO term_profile_info;
    _bcm_lif_type_e lif_usage;
    uint32 profile_orientation;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int push_profile = -1;
    uint8 has_cw, is_egress_wide_entry, ext_type;
    SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO  pwe_additional_info;
    uint8 remote_lif;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

    soc_sand_dev_id = (unit);

    /* is mpls port */
    if (!BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid parameter: mpls_port_id is not an MPLS port type")));
    }

    /* mpls_port_get api can be used to get push profile allocated earlier by mpls_port_add.
       In this case push profile sub type bit in mpls_port_id should be set */
    if (BCM_GPORT_SUB_TYPE_IS_MPLS_PUSH_PROFILE(mpls_port->mpls_port_id)) {

        rv = _bcm_dpp_mpls_port_push_profile_get(unit, mpls_port);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* is mpls port exists */
    port_val = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
    if (port_val == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid parameter: mpls_port_id is not an MPLS port")));
    }

    /* Initialize the structure */
    bcm_mpls_port_t_init(mpls_port);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->mpls_port_id, port_val);

    /* resolve content of the mpls-port: encap-id, dest:FEC/port Type: protected/VC/AC */
    rv = _bcm_dpp_gport_resolve(unit, mpls_port->mpls_port_id, 0, &dest_id,
                                &encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_l2_intf_gport_has_remote_destination(unit, mpls_port->mpls_port_id, &remote_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    if (remote_lif) {
        is_ingress = is_egress = 0;
    }
    else {
        rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port->mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        is_ingress = (gport_hw_resources.global_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
        is_egress = (gport_hw_resources.global_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;
    }

    if ((is_egress && !is_ingress) || BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port->mpls_port_id)) {
        if (_BCM_DPP_VPN_IS_SET(vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For egress and fec objects, vpn parameter must be unset")));
        }
    }

    if (is_ingress && is_egress) {
        if (gport_hw_resources.global_in_lif != gport_hw_resources.global_out_lif) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Global In-LIF and Global Out-LIF must be equal in case ingress (0x%x) and egress (0x%x) set"), 
                                                 gport_hw_resources.global_in_lif,gport_hw_resources.global_out_lif));
        }        
        global_lif = gport_hw_resources.global_out_lif;
    } else if (is_ingress) {
        global_lif = gport_hw_resources.global_in_lif;
    } else if (is_egress) {
        global_lif = gport_hw_resources.global_out_lif;
    }

    if (is_ingress || is_egress) { /* Otherwise get it from inside FEC */
        BCM_ENCAP_ID_SET(mpls_port->encap_id, global_lif);
    }

    if (is_ingress) {
        /* Ensure InLIF is PWE */
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(soc_sand_dev_id, gport_hw_resources.local_in_lif, -1, &lif_usage, NULL) );
        if (lif_usage != _bcmDppLifTypeMplsPort) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("InLIF is not PWE")));
        }

        /* Get the match parameters */
        rv = _bcm_dpp_in_lif_mpls_match_get(unit, mpls_port, gport_hw_resources.local_in_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
            if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
                pwe_additional_info.inner_vid_valid = 0x1;
                pwe_additional_info.inner_vid = mpls_port->match_inner_vlan;
            }
            if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) {
                pwe_additional_info.outer_vid_valid = 0x1;
                pwe_additional_info.outer_vid = mpls_port->match_vlan;
            }
        }

        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
	    _bcm_dpp_gport_sw_resources gport_sw_resources;

    	    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, gport_hw_resources.local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
            pwe_additional_info.vlan_domain_valid = 1;
            pwe_additional_info.vlan_domain = gport_sw_resources.in_lif_sw_resources.match2;
        }

        rv = _bcm_dpp_am_template_lif_term_data_get(unit, gport_hw_resources.local_in_lif, &term_profile_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((term_profile_info.flags & SOC_PPC_MPLS_TERM_DISCARD_TTL_0) || (term_profile_info.flags & SOC_PPC_MPLS_TERM_DISCARD_TTL_1)) {
            mpls_port->vccv_type = bcmMplsPortControlChannelTtl;
        }

        /* Get wide lif flag */
        if (SOC_IS_JERICHO(unit)) {
            uint8 is_wide_entry;

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, gport_hw_resources.local_in_lif, TRUE, &is_wide_entry, NULL));
            BCMDNX_IF_ERR_EXIT(rv);

            mpls_port->flags2 |= (is_wide_entry == BCM_DPP_AM_IN_LIF_FLAG_WIDE) ? BCM_MPLS_PORT2_INGRESS_WIDE : 0;
        }

    }


    /* get protection info, irrelevant in case of ingress only and egress only */
    if (_BCM_DPP_GPORT_RESOLVE_TYPE_IS_FEC(gport_type)) {
        _bcm_dpp_am_fec_alloc_usage usage;
        uint32 fec_id;

        /* In case FEC is protected, information is taken from the working */
        if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port->mpls_port_id) || (gport_type==_bcmDppGportResolveTypeForwardFec)) {
            fec_id = dest_id.dest_val;
        }
        else {
            fec_id = _BCM_PPD_GPORT_FEC_TO_WORK_FEC(dest_id.dest_val);
        }

        if (dest_id.dest_val < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved) {
            /* if value is ECMP no need to check if cascaded or not */
            usage = _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD;
        } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,  "conn_to_np_enable", 0)&&
                        (!is_egress)){
            /*In PP standalone scenario, failover_port_id for ingress_only PWE is a user defined index*/
             BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->failover_port_id, fec_id);
            soc_sand_rv = soc_ppd_l2_lif_pwe_get(soc_sand_dev_id, mpls_port->match_label,&lif_idx, &pwe_additional_info, &pwe_info,&found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF PWE not found")));
            }

            DPP_FAILOVER_TYPE_SET(mpls_port->ingress_failover_id, pwe_info.protection_pointer, pwe_info.protection_pointer?DPP_FAILOVER_TYPE_INGRESS:DPP_FAILOVER_TYPE_NONE);
            usage = _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD;
        }
        else {
            rv = bcm_dpp_am_fec_get_usage(unit, fec_id, &usage);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_l2_fec_fill_gport(unit, dest_id.dest_val, mpls_port, BCM_GPORT_MPLS_PORT);
            BCMDNX_IF_ERR_EXIT(rv);
        }


        /* In JERICHO possibly set the CASCADED flag. */
        if (SOC_IS_JERICHO(unit)) {
            if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
                mpls_port->flags2 |= BCM_MPLS_PORT2_CASCADED;
            }
        }

        if (!is_ingress && !is_egress) {
            BCM_ENCAP_REMOTE_SET(mpls_port->encap_id);
        }
    }
    /* 1+1 protection */
    else if (gport_type == _bcmDppGportResolveTypeMC) {
        soc_sand_rv = soc_ppd_l2_lif_pwe_get(soc_sand_dev_id, mpls_port->match_label,&lif_idx, &pwe_additional_info, &pwe_info,&found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF PWE not found")));
        }

        DPP_FAILOVER_TYPE_SET(mpls_port->failover_id, pwe_info.protection_pointer, DPP_FAILOVER_TYPE_INGRESS);
        _BCM_MULTICAST_GROUP_SET(mpls_port->failover_mc_group, _BCM_MULTICAST_TYPE_L2, dest_id.dest_val);
    }

    mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;

    BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_mpls_port_get.encap_entry_info");
    if (encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    if (is_egress) {
        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(soc_sand_dev_id, -1, gport_hw_resources.local_out_lif, NULL, &lif_usage);
        BCMDNX_IF_ERR_EXIT(rv);
        if (lif_usage != _bcmDppLifTypeMplsPort) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not PWE")));
        }

        soc_sand_rv =
           soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                      SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP,
                                      gport_hw_resources.local_out_lif, 1,
                                      encap_entry_info, next_eep,
                                      &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        push_profile = encap_entry_info->entry_val.pwe_info.push_profile;

        /* In case of egress protection, retrieve the egress protection info */
        mpls_port->egress_failover_id = 0;
        mpls_port->egress_failover_port_id = 0;
        if (SOC_IS_JERICHO(unit)) {
            /* Check whether an extension was allocated for Egress protection */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_is_wide_entry, (unit, gport_hw_resources.local_out_lif, FALSE, &is_egress_wide_entry, &ext_type));
            BCMDNX_IF_ERR_EXIT(rv);

            if ((is_egress_wide_entry == BCM_DPP_AM_OUT_LIF_FLAG_WIDE) && (ext_type == 0) &&
                encap_entry_info[0].entry_val.pwe_info.protection_info.is_protection_valid) {
                DPP_FAILOVER_TYPE_SET(mpls_port->egress_failover_id, encap_entry_info[0].entry_val.pwe_info.protection_info.protection_pointer,
                                      DPP_FAILOVER_TYPE_ENCAP);
                mpls_port->egress_failover_port_id = (encap_entry_info[0].entry_val.pwe_info.protection_info.protection_pass_value) ? 0 : 1;
                mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_PROTECTION;
            }
        }

        if (!is_ingress) {
            global_lif_next_eep = next_eep[0];
            if (SOC_IS_JERICHO(unit)) {
                rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[0], &global_lif_next_eep);
                if ((rv != BCM_E_NONE) &&  (rv != BCM_E_NOT_FOUND)) {
                   BCMDNX_IF_ERR_EXIT(rv);
                }
            }
            /*global lif ont found, just means the mpls tunnel does not exist anymore.*/
            if (rv == BCM_E_NOT_FOUND) {
               mpls_port->egress_tunnel_if = 0;
            } else {
               BCM_L3_ITF_SET(mpls_port->egress_tunnel_if, BCM_L3_ITF_TYPE_LIF, global_lif_next_eep);
            }
            mpls_port->egress_label.label = encap_entry_info[0].entry_val.pwe_info.label;
        }

        /* Fetch (possibly) egress tunnel label information : label and push profile */
        if (soc_property_get(unit, spn_MPLS_BIND_PWE_WITH_MPLS_ONE_CALL, 0)) {
            if (encap_entry_info->entry_val.pwe_info.egress_tunnel_label_info.nof_tunnels == 1) {
                uint8 dummy_has_cw = 0;
                uint32 egress_tunnel_label_push_profile = 0;

                egress_tunnel_label_push_profile = encap_entry_info->entry_val.pwe_info.egress_tunnel_label_info.tunnels[0].push_profile;

                rv = _bcm_mpls_tunnel_push_profile_info_get(unit,egress_tunnel_label_push_profile ,&dummy_has_cw,&(mpls_port->egress_tunnel_label));
                BCMDNX_IF_ERR_EXIT(rv);

                mpls_port->egress_tunnel_label.flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
            }

            mpls_port->egress_tunnel_label.label = encap_entry_info->entry_val.pwe_info.egress_tunnel_label_info.tunnels[0].tunnel_label;
        }
    }
    if ((_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id) && _BCM_MULTICAST_IS_SET(mpls_port->failover_mc_group)) ||
        (!_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id) && (is_ingress && !is_egress)) ||
        (!_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id) && (is_ingress && !BCM_ENCAP_REMOTE_GET(mpls_port->encap_id)))) {

        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                           (sw_state_htb_key_t)  &(mpls_port->mpls_port_id), 
                                           (sw_state_htb_data_t) &phy_port, FALSE);

        if (rv != BCM_E_NOT_FOUND) { /* We don't care if it's not found. */
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            phy_port.type = _bcmDppGportResolveTypeInvalid;
        }


        if ((phy_port.type == _bcmDppGportResolveTypeFecEEP) && vpn != 0) {
            mpls_port->flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
        }

        if ((phy_port.type == _bcmDppGportResolveTypeFecEEI) && vpn != 0) {
            mpls_port->flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP_EEI;
        }

        if (phy_port.type == _bcmDppGportResolveTypeForwardFec) {
            /* Get failover port id */
            BCM_GPORT_FORWARD_PORT_SET(mpls_port->failover_port_id, phy_port.phy_gport);
        }
        else if (phy_port.type == _bcmDppGportResolveTypeProtectedFec) {
            /* Get failover port id */
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port->failover_port_id, phy_port.phy_gport);
        }
        else if (phy_port.type == _bcmDppGportResolveTypeFecVC) {
            /* Get egress tunnel */
            BCM_L3_ITF_SET(mpls_port->egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, phy_port.phy_gport);

            mpls_port->egress_label.label = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(phy_port.encap_id);
            push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(phy_port.encap_id);
        }
        else { 
            if ((is_egress) &&(next_eep[0])) {
                global_lif_next_eep = next_eep[0];
                if (SOC_IS_JERICHO(unit)) {
                    rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[0], &global_lif_next_eep);
                    if ((rv != BCM_E_NONE) &&  (rv != BCM_E_NOT_FOUND)) {
                       BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
                /*global lif ont found, just means the mpls tunnel does not exist anymore.*/
                if (rv == BCM_E_NOT_FOUND) {
                   mpls_port->egress_tunnel_if = 0;
                } else {
                   BCM_L3_ITF_SET(mpls_port->egress_tunnel_if, BCM_L3_ITF_TYPE_LIF, global_lif_next_eep);
                }
            }
        }
    }

    if (push_profile != -1) {

        rv = _bcm_mpls_tunnel_push_profile_info_get(unit,
                            push_profile, 
                            &has_cw,
                            &mpls_port->egress_label);
        BCMDNX_IF_ERR_EXIT(rv);

        mpls_port->egress_label.action &= ~BCM_MPLS_EGRESS_ACTION_PUSH;

        mpls_port->flags |= has_cw ? BCM_MPLS_PORT_CONTROL_WORD : 0;
    }

    if (is_egress || (gport_type == _bcmDppGportResolveTypeFecVC)) {   
        /* get it from encap_id (SW-state) if PWE is not protected otherwise get it from EEP */
        if (gport_type == _bcmDppGportResolveTypeFecVC) {
            mpls_port->egress_label.label = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(encap_id);
        } 
        else if (gport_type == _bcmDppGportResolveTypeMC) {
            /* egress label */
            mpls_port->egress_label.label = encap_entry_info[0].entry_val.pwe_info.label;
        }
        /* from EEP */
        else if (is_egress) {                              
            if (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Encapsulation entry type must be PWE")));  /* has to be pwe */
            }
                
            mpls_port->egress_label.label =
                encap_entry_info[0].entry_val.pwe_info.label;
        }
    }

    if (!is_ingress && is_egress) {
        mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    }

    if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port->mpls_port_id)) {
        mpls_port->flags |= BCM_MPLS_PORT_FORWARD_GROUP;
    }

    /* network group id - if egress get from HW (already read), else read inlif HW */
    if (is_ingress || is_egress) {
        if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
            if (is_egress) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                         (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
                                          &(encap_entry_info[0].entry_val.pwe_info.outlif_profile), &profile_orientation));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                mpls_port->network_group_id = profile_orientation;
            }
            else {
                soc_sand_rv = soc_ppd_l2_lif_pwe_get(unit, mpls_port->match_label, &lif_idx, &pwe_additional_info, &pwe_info, &found);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                mpls_port->network_group_id = pwe_info.network_group;
            }

            if (mpls_port->network_group_id > 0) {
               mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
            }
        } else {
            if (!is_ingress && is_egress) {
                if (encap_entry_info[0].entry_val.pwe_info.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB) {
                    mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
                }
            }
        }
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_mpls_port_t * port_array,
    int *port_count)
{
    int nof_ports = 0;
    int rv = BCM_E_NONE;
    int iter, in_lif_val;
    int port_val;
    bcm_gport_t cur_gport;
    int vsi;
    SOC_PPC_LIF_ID local_lif_index_drop; 
    SOC_PPC_LIF_ID local_lif_index_simple;

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    VLAN_ACCESS.vlan_info.local_lif_index_drop.get(unit, &local_lif_index_drop); 
    VLAN_ACCESS.vlan_info.local_lif_index_simple.get(unit, &local_lif_index_simple); 

    for (iter = 0; iter != -1;) {

        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit, vpn, &iter, &port_val, &vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if (port_val == BCM_GPORT_INVALID) {
            break;
        }

        /* Skip default In-LIF*/
        in_lif_val = iter - 1;
        if ((in_lif_val == local_lif_index_drop) ||
            (in_lif_val == local_lif_index_simple)) {
            continue;
        }
        
        /* fill next port */
        cur_gport = port_val;

        if (BCM_GPORT_IS_MPLS_PORT(cur_gport)) {
            port_array[nof_ports].mpls_port_id = cur_gport;
            rv = bcm_petra_mpls_port_get(unit, vpn, &port_array[nof_ports]);
            BCMDNX_IF_ERR_EXIT(rv);
            ++nof_ports;

        }


        if (nof_ports >= port_max) {
            break;
        }
    }

    *port_count = nof_ports;

exit:
    BCMDNX_FUNC_RETURN;
}


/* bcm_mpls_port_t_init not dispatchable */

int _bcm_mpls_tunnel_push_profile_info_get(
    int unit,
    int push_profile,
    uint8 * has_cw,
    bcm_mpls_egress_label_t * label_entry)
{
    SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_entry);

    SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(&profile_info);

    soc_sand_dev_id = (unit);

    label_entry->action = BCM_MPLS_EGRESS_ACTION_PUSH;

    soc_sand_rv =
        soc_ppd_eg_encap_push_profile_info_get(soc_sand_dev_id, push_profile,
                                           &profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    label_entry->exp = profile_info.exp;
    label_entry->ttl = profile_info.ttl;
    *has_cw = profile_info.has_cw;
    label_entry->qos_map_id = 0;
    if (profile_info.remark_profile != 0) {
        BCM_INT_QOS_MAP_REMARK_SET(label_entry->qos_map_id,profile_info.remark_profile);
    }
      
    /* In QAX and above, ttl and exp inheritance models are configured seperately */
    if (SOC_IS_JERICHO_PLUS(unit)) {
        if (profile_info.exp_model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET) {
            label_entry->flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        } else {
            label_entry->flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }

        if (profile_info.ttl_model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET) {
            label_entry->flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        } else {
            label_entry->flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY;
        }

        label_entry->flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    } else {
      /* pipe: set TTL and EXP, and always dec TTL */
        if (profile_info.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
        label_entry->flags =
            (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET
             | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        
        #ifdef BCM_88660
        if (SOC_IS_ARADPLUS(unit)) {
            uint8 exp_copy_mode = 0;
        
            /* In arad+ pipe mode can be represented by */
            /* 1. TTL_SET | EXP_COPY in exp_copy_mode mode */
            /* 2. TTL_SET | EXP_SET in !exp_copy_mode mode */
            /* So remove add EXP_COPY/SET according to the mode */
            soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(soc_sand_dev_id, &exp_copy_mode);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
            if (exp_copy_mode) {
                label_entry->flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY; 
                label_entry->flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_SET;
            }
            else {
                label_entry->flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET; 
                label_entry->flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_COPY; 
            }
        }
         
        #endif /* BCM_88660 */
        } else {                    /* uniform: copy TTL and EXP, and always dec TTL */
    

        label_entry->flags =
            (BCM_MPLS_EGRESS_LABEL_TTL_COPY |
             BCM_MPLS_EGRESS_LABEL_EXP_COPY |
             BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
      }
    }

      if (profile_info.add_entropy_label) {
          label_entry->flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
      }

      if (SOC_IS_JERICHO_PLUS(unit) && profile_info.add_entropy_label_indicator) {
          label_entry->flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
      }

    if (SOC_IS_JERICHO_PLUS(unit)) {
        label_entry->spl_label_push_type = profile_info.rsv_mpls_label_bmp;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_mpls_tunnel_push_profile_alloc
 * Purpose:
 *      Allocate push profile according to given attributes and configure HW
 * Parameters:
 *      (IN)     unit           - Device Number
 *      (IN/OUT) push_profile   - Allocated push profile, IN if with_id is set
 *      (IN) cw                 - Indication if CW is set in the push profile
 *      (IN) label_entry        - Push profile parameters are given inside egress label struct
 *      (IN) is_test            - If set, the process is done without allocating the profile
 *      (IN) with_id            - If set, push_profile is allocated with id indicated by push_profile parameter
 *      (IN) update_existing_profile  - If set, the existing profile data specified by push_profile field will be updated by given parameters
 *      (OUT) is_first          - If allocated profile does not exist in the HW  yet, this parameter is set
 *      (IN)  is_second_mpls_label_in_entry - If set, this push profile is allocated for the second label in a MPLS EEDB entry (mpls2 field).
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_mpls_tunnel_push_profile_alloc(
    int unit,
    uint32 * push_profile,
    uint8 cw,
    bcm_mpls_egress_label_t * label_entry,
    uint8 is_test,
    uint8 with_id,
    uint8 update_existing_profile,
    uint8 set_hw,
    uint8 * is_first,
    uint8 is_second_mpls_label_in_entry)
{
    SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int first_appear;
    int tmp_push_profile = 0;
    int is_last;
    int flags = 0;
    /* Indicates whether the allocated push profile is for pop/swap actions, thus won't be used
       by the user for push purposes. The dummy push profile will be determined by illegal exp values (above 7)*/
    int is_dummy_push_profile = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(push_profile);

    soc_sand_dev_id = (unit);

    if (label_entry->action != BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH) {
        label_entry->action = BCM_MPLS_EGRESS_ACTION_PUSH;
    }

    if (with_id) {
        is_dummy_push_profile = _BCM_DPP_MPLS_IS_FORBIDDEN_PUSH_PROFILE(*push_profile);
    }

    if (update_existing_profile && is_test) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG(" Error in push profile allocation. Can not test profile updating"))); 
    }

    if (update_existing_profile && !with_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG(" Error in push profile allocation. When updating existing profile with_id flag must be set")));
    }

    SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(&profile_info);

    profile_info.exp_mark_mode = SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE;
    profile_info.exp = label_entry->exp;
#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
		/* In QAX and above, ttl and exp inheritance models are configured seperately */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
                profile_info.ttl_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET;
            } else {
                profile_info.ttl_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
            }

            if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) {
                profile_info.exp_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET;
            } else {
                profile_info.exp_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
            }
        } else {
            uint8 exp_copy_mode = 0;

            if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
                /* Validity check of the pipe model flags */
                soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(soc_sand_dev_id, &exp_copy_mode);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if ((exp_copy_mode) && (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_SET)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Configuration enables only EXP_COPY in PIPE mode. Can be changed using switch control bcmSwitchMplsPipeTunnelLabelExpSet"))); 
                }
                if ((!exp_copy_mode) && (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" Configuration enables only EXP_SET in PIPE mode. Can be changed using switch control bcmSwitchMplsPipeTunnelLabelExpSet"))); 
                }

                profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
            }
            else {
                if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" In Uniform model only EXP_COPY is suppored"))); 
                }

                profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
            }

            /* If the push profile is of swap/pop value, we deem it a dummy push profile and it's allocated
               in order to prevent the user from allocating it for push purposes. Thus, for dummy push profiles,
               the exp value shouldn't be affected by the model (uniform / pipe)*/
            if (((profile_info.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM) || (exp_copy_mode)) && !is_dummy_push_profile) {
                profile_info.exp = 0;
            }
        }
    }
    else {
#endif /* BCM_88660 */

        /* pipe: set TTL and EXP, and always dec TTL */
        if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" In Pipe model only EXP_SET is suppored"))); 
            }
            profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
        } else {                   /* uniform: copy TTL and EXP, and always dec TTL */
            if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" In Uniform model only EXP_COPY is suppored"))); 
            }
            profile_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
        }
#ifdef BCM_88660
    }
#endif /* BCM_88660 */

    profile_info.ttl = label_entry->ttl;
    profile_info.has_cw = cw;
    profile_info.remark_profile = BCM_QOS_MAP_PROFILE_GET(label_entry->qos_map_id);

    /* Entropy label */
    if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE) {
        /*
         * In JR, EL alone is inserted by pipeline,EL&ELI  is inserted by uCode
         * QAX could work with same method only when el_eli_jr_mode=1
         */
        if (SOC_IS_JERICHO_AND_BELOW(unit)) 
        {
            profile_info.add_entropy_label = (label_entry->flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE)?0:1;
        }
        else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "el_eli_jr_mode", 0))
        {
            profile_info.add_entropy_label = (label_entry->flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE)?0:1;
        }
        else 
        {
            profile_info.add_entropy_label = TRUE;
        }
    } 
   
    if (SOC_IS_JERICHO_PLUS(unit) && (label_entry->spl_label_push_type > bcmMplsSpecialLabelPushSpecialPlusEntropy)) {
        profile_info.rsv_mpls_label_bmp= label_entry->spl_label_push_type ;
    }      
 
    /* Entropy label */
    if (SOC_IS_JERICHO_PLUS(unit) 
        && !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "el_eli_jr_mode", 0)
        && (label_entry->flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE)) {
        profile_info.add_entropy_label_indicator = TRUE;
    }      
      
    if (with_id) {
        tmp_push_profile = (int)(*push_profile);
        flags = BCM_DPP_AM_TEMPLATE_FLAG_ALLOC_WITH_ID;
    }

    if (update_existing_profile) { 
        rv = _bcm_dpp_am_template_mpls_push_profile_exchange(unit, tmp_push_profile, &profile_info,  &is_last, &tmp_push_profile, &first_appear, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        rv = _bcm_dpp_am_template_mpls_push_profile_alloc(unit, flags, 0,
                                                          &profile_info,
                                                          &tmp_push_profile,
                                                          &first_appear);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* If this is the first time we try to allocate this profile and it is not done with an id,
       then profiles above 7 (that is, profiles that can be pointed only by mpls1 in EEDB, e.g. 10-15) cannot be allocated.
       Profiles 10-15 can be allocated only with with_id==1, that is from bcm_mpls_port_add() with a given push profile.
       In short, profiles 1-7 can be pointed by mpls1 and mpls2, whereas profiles 10-15 can be pointed only by mpls1      .
       and are created via bcm_mpls_port_add() with the given profile.*/
    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && tmp_push_profile > ARAD_PP_EG_ENCAP_MPLS2_MAX_PUSH_PROFILE_VALUE) {
        /* In case is_test == 1, we allow profiles 10-15 to be allocated, since they will be freed further on. */
        if (first_appear && !with_id && !is_test) {
            rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, tmp_push_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, \
            (_BSL_BCM_MSG("Profiles bigger than %d (excluding 8,9) can be created only via bcm_mpls_port_add with a given profile."),ARAD_PP_EG_ENCAP_MPLS2_MAX_PUSH_PROFILE_VALUE));
        }
        if (is_second_mpls_label_in_entry) {
            rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, tmp_push_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, \
            (_BSL_BCM_MSG("Profiles bigger than %d (excluding 8,9) cannot be given to the second label in an EEDB entry."),ARAD_PP_EG_ENCAP_MPLS2_MAX_PUSH_PROFILE_VALUE));
        }
    }

    if (!is_test && first_appear && set_hw && !is_dummy_push_profile) {
        soc_sand_rv =
            soc_ppd_eg_encap_push_profile_info_set(soc_sand_dev_id, tmp_push_profile,
                                               &profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    *push_profile = (uint32) tmp_push_profile;

    if (is_test) {
        rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, tmp_push_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (is_first) {
        *is_first = first_appear;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_mpls_tunnel_push_profile_alloc_dummy_profiles
 * Purpose:
 *      Allocate dummy push profiles (So these profiles won't be allocated by the template manager).
 *      These profiles will be differentiated from regular ones with illegal exp values (so the user won't
 *      receive or allocate those profiles).
 * Parameters:
 *      (IN)     unit              - Device Number
 *      (IN) dummy_profiles_array  - An array that holds the dummy push profiles to be allocated
 *      (IN) nof_dummy_profiles    - The number of dummy push profiles to be allocated
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_mpls_tunnel_push_profile_alloc_dummy_profiles(
    int unit,
    uint32 *dummy_profiles_array,
    int nof_dummy_profiles)
{
    int rv = BCM_E_NONE;
    int indx;
    bcm_mpls_egress_label_t label_entry;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    bcm_mpls_egress_label_t_init(&label_entry);
    /* Create dummy push profile with illegal exp values in order to prevent the user from using these profiles */
    label_entry.exp = SOC_SAND_PP_MPLS_EXP_MAX + 1;
    for (indx = 0; indx < nof_dummy_profiles; indx++) {
        rv = _bcm_mpls_tunnel_push_profile_alloc(unit,&dummy_profiles_array[indx],0,&label_entry,FALSE,TRUE,FALSE,TRUE,NULL,0);
        BCMDNX_IF_ERR_EXIT(rv);
        label_entry.exp++;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_mpls_tunnel_initial_additional_push_profiles_alloc
 * Purpose:
 *      Allocate initial additional push profiles for EL and ELI labels.
 * Parameters:
 *      (IN)     unit              - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_mpls_tunnel_initial_additional_push_profiles_alloc(
    int unit)
{
    int rv = BCM_E_NONE;
    SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO additional_label_profile_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("Reserved label profile is configured for QAX/Jericho+ and above devices.")));
    }

    /* Allocate EL */
    SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_clear(&additional_label_profile_info);
    additional_label_profile_info.exp_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
    additional_label_profile_info.ttl_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET;
    additional_label_profile_info.exp = 0;
    additional_label_profile_info.ttl = 0;
    additional_label_profile_info.label_type = _BCM_DPP_MPLS_RESERVE_PROFILE_OP_EL;
    additional_label_profile_info.label = 0;
    rv  = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_additional_label_profile_set, (unit,&additional_label_profile_info,1)));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate ELI */
    SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_clear(&additional_label_profile_info);
    additional_label_profile_info.exp_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
    additional_label_profile_info.ttl_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
    additional_label_profile_info.exp = 0;
    additional_label_profile_info.ttl = 0;
    additional_label_profile_info.label_type = _BCM_DPP_MPLS_RESERVE_PROFILE_OP_LABEL_PER_PROFILE;
    additional_label_profile_info.label = 7;
    rv  = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_additional_label_profile_set, (unit,&additional_label_profile_info,2)));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate for common mpls label, ttl/exp copy from main header */
    SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_clear(&additional_label_profile_info);
    additional_label_profile_info.exp_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
    additional_label_profile_info.ttl_model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY;
    additional_label_profile_info.exp = 0;
    additional_label_profile_info.ttl = 0;
    additional_label_profile_info.label_type = _BCM_DPP_MPLS_RESERVE_PROFILE_OP_PER_DATA_ENTRY;
    additional_label_profile_info.label = 0;
    rv  = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_additional_label_profile_set, (unit,&additional_label_profile_info,3)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_mpls_tunnel_lif_term_mpls_port_to_flags(
      int unit,
      bcm_mpls_port_t *   mpls_port,
      uint32 *inner_flags
    )
{
    BCMDNX_INIT_FUNC_DEFS;

    *inner_flags = 0;

    if (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CONTROL_WORD;
    }

    /* In VPN, only add Entropy label */
    if (mpls_port->flags & BCM_MPLS_PORT_ENTROPY_ENABLE) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_ENTROPY;
    }

    if (mpls_port->flags & BCM_MPLS_PORT_WITH_GAL) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_GAL;
    }

    if (mpls_port->vccv_type == bcmMplsPortControlChannelTtl) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0;
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_mpls_tunnel_lif_term_tunnel_switch_to_flags(
      int unit,
      bcm_mpls_tunnel_switch_t * tunnel_switch,
      uint32 *inner_flags
    )
{

    BCMDNX_INIT_FUNC_DEFS;

    *inner_flags = 0;

    if ((tunnel_switch->flags & BCM_MPLS_SWITCH_FRR) &&
        (!(SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23))) {
        *inner_flags = _BCM_DPP_MPLS_LIF_TERM_FRR;
    } else if (tunnel_switch->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL must be configured only with BCM_MPLS_SWITCH_FRR.")));
    }

    /* Entropy: in case of MPLS Tunnel, add entropy and ELI labels */ 
    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_ENTROPY_ENABLE)) {
        *inner_flags |= (_BCM_DPP_MPLS_LIF_TERM_ENTROPY | _BCM_DPP_MPLS_LIF_TERM_ELI);
    }

    /* Reject TTL 0/1 */
    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_TRAP_TTL_0)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0;
    }

    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_TRAP_TTL_1)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1;
    }

    if (tunnel_switch->flags & (BCM_MPLS_SWITCH_SKIP_ETHERNET)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_SKIP_ETHERNET;
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
        if (BCM_MPLS_INDEXED_LABEL_VALUE_GET(tunnel_switch->label) <= _BCM_PETRA_MPLS_MAX_RESERVED_MPLS_LABEL) {
            /* Reserved labels always have check bos on. */
            *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS;
        } else {/* non reserved labels */
            if (tunnel_switch->flags & BCM_MPLS_SWITCH_EXPECT_BOS) {
                /* a feature enabling checking bos=1 for non reserved labels*/
                *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS;
            }
        }

    }
#endif /* BCM_88660_A0 */

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        if (tunnel_switch->flags & BCM_MPLS_SWITCH_EVPN_IML) {
            *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_EVPN_IML;
            if (tunnel_switch->flags & BCM_MPLS_SWITCH_EXPECT_BOS) {
                *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS;
            }
        }

        if (tunnel_switch->flags2 & BCM_MPLS_SWITCH2_CONTROL_WORD) {
            *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_CONTROL_WORD;
        }
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_termination_enable", 0) && 
        (BCM_MPLS_INDEXED_LABEL_INDEX_GET(tunnel_switch->label) == SOC_PPC_MPLS_LABEL_INDEX_ZERO)) {
        *inner_flags |= _BCM_DPP_MPLS_LIF_TERM_FRR;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_mpls_tunnel_lif_term_profile_exchange(
      int unit,
      SOC_PPC_LIF_ID lif_id,
      uint32 inner_flags,
      uint8 is_pwe_lif,
      uint8 expect_bos,
      uint8* term_profile)
{
    int rv;
    int old_template, new_template, is_allocated, is_last;
    SOC_PPC_MPLS_TERM_PROFILE_INFO term_profile_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&term_profile_info);

    /* Has CW */
    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_CONTROL_WORD) {
        term_profile_info.flags |= SOC_PPC_MPLS_TERM_HAS_CW;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_ENTROPY) {
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_GAL) {
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_ELI) {
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & _BCM_DPP_MPLS_LIF_TERM_FRR) {
        term_profile_info.ttl_exp_index = 1; 
        term_profile_info.nof_headers += 1;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_0)) {
        term_profile_info.flags |= SOC_PPC_MPLS_TERM_DISCARD_TTL_0;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_REJECT_TTL_1)) {
        term_profile_info.flags |= SOC_PPC_MPLS_TERM_DISCARD_TTL_1;
    }

    if (inner_flags & (_BCM_DPP_MPLS_LIF_TERM_SKIP_ETHERNET)) {
        term_profile_info.flags |= SOC_PPC_MPLS_TERM_SKIP_ETH;
    }

#ifdef BCM_88660_A0
    term_profile_info.check_bos = 1; /*for pwe this flag should not be set*/
    if (SOC_IS_JERICHO(unit)) {
        term_profile_info.expect_bos = expect_bos;
    }
    if ((SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) || \
       ((SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0) && (inner_flags & _BCM_DPP_MPLS_LIF_TERM_EVPN_IML)))) {

        term_profile_info.check_bos = (inner_flags & _BCM_DPP_MPLS_LIF_TERM_CHECK_BOS) ? 1 : 0;
        if ((inner_flags & _BCM_DPP_MPLS_LIF_TERM_EVPN_IML)) {
            term_profile_info.nof_headers = (term_profile_info.check_bos) ? term_profile_info.nof_headers : 2;
        }
    }
#endif /* BCM_88660_A0 */

    /*When don't check bos, expect-bos should be always 0*/
    if (!term_profile_info.check_bos) {
        term_profile_info.expect_bos = 0;
    }

    rv = _bcm_dpp_am_template_lif_term_profile_exchange(unit,lif_id,is_pwe_lif,&term_profile_info, &old_template, &is_last, &new_template, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (term_profile != NULL) {
        *term_profile = new_template;
    }

    if (is_allocated) {
        /* Update HW */
        soc_sand_rv = soc_ppd_mpls_term_profile_info_set(soc_sand_dev_id, new_template, &term_profile_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
      



int _bcm_mpls_tunnel_bcm_from_ppd(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO * tunnel_info)
{
    uint8 has_cw;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(tunnel_info);

    label_entry->label = tunnel_info->tunnel_label;
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_info_get(unit,
                                                  tunnel_info->push_profile, 
                                                  &has_cw,
                                                  label_entry));
    label_entry->action = BCM_MPLS_EGRESS_ACTION_PUSH;
    label_entry->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    if (has_cw) {
        label_entry->flags |= BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
    }

exit:
    BCMDNX_FUNC_RETURN;
}




int _bcm_mpls_tunnel_bcm_to_ppd(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    uint8 has_cw,
    SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO * tunnel_info,
    uint8 is_second_mpls_label_in_entry)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(tunnel_info);
    /* fill label */
    tunnel_info->tunnel_label = label_entry->label;
    /* allocate push profile to hold TTL/EXP and model */
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                               &tunnel_info->push_profile,
                                               has_cw,
                                               label_entry,
                                               FALSE,
                                               FALSE,
                                               FALSE,
                                               TRUE,
                                               NULL,
                                               is_second_mpls_label_in_entry));
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_switch_egress_label_to_mpls_command(
    int unit,
    bcm_mpls_egress_label_t * label_entry,
    SOC_PPC_MPLS_COMMAND * command_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(label_entry);
    BCMDNX_NULL_CHECK(command_info);
    /* fill label */
    command_info->label = label_entry->label;
    if (label_entry->flags & BCM_MPLS_EGRESS_LABEL_CONTROL_WORD) {
        command_info->has_cw = TRUE; 
    } else {
        command_info->has_cw = FALSE; 
    }
    /* allocate push profile to hold TTL/EXP and model */
    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                               &command_info->push_profile,
                                               FALSE,
                                               label_entry,
                                               FALSE,
                                               FALSE,
                                               FALSE,
                                               TRUE,
                                               NULL,
                                               0));
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_mpls_egress_tunnel_is_bind_entries(
    int unit,
    bcm_mpls_egress_label_t * label_array,
    int start_indx,
    int *is_bind_entries)
{
    BCMDNX_INIT_FUNC_DEFS;

    *is_bind_entries = 0;

    /* Case of PUSH */
    if ((start_indx != 1) && /* In case start indx is 1, no case of two consecutive labels (it is the last label in stack) */
        (label_array[start_indx-1].tunnel_id == label_array[start_indx-2].tunnel_id) && /* tunnel id is the same */
        ((label_array[start_indx-1].flags & 
          (BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET))) && /* not push 1 */
        (((label_array[start_indx-2].flags & 
          (BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET)))) ) /* not push 2 */ {
    
       /* special case is true */
       /* Note: For Arad+ the EXP_SET flag is illegal, but since this is a superset of the legal flags, there is no need for change. */
       *is_bind_entries = 1;      
    } 

    /* Case of SWAP_COUPLING */
    if ((start_indx != 1) && /* In case start indx is 1, no case of two consecutive labels (it is the last label in stack) */
        (label_array[start_indx-2].tunnel_id == 0) && /* Tunnel ID of NOP action must be 0 */
        (label_array[start_indx-2].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-2].action == BCM_MPLS_EGRESS_ACTION_NOP) && /* NOP action on the first label */
        (label_array[start_indx-1].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-1].action == BCM_MPLS_EGRESS_ACTION_SWAP)) { /* SWAP action on the second label */
      *is_bind_entries = 1;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_mpls_egress_tunnel_to_egress_action_type(
    int unit,
    bcm_mpls_egress_label_t * label_array,
    int index,
    _bcm_dpp_mpls_egress_action_type_t *action_type)
{

    BCMDNX_INIT_FUNC_DEFS;

    
    if (label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_ACTION_VALID)) {
        /* Explicit */
        if (label_array[index].action == BCM_MPLS_EGRESS_ACTION_SWAP) {
            *action_type = _bcm_dpp_mpls_egress_action_type_swap;
        } else if (label_array[index].action == BCM_MPLS_EGRESS_ACTION_PHP) {
            *action_type = _bcm_dpp_mpls_egress_action_type_pop;
        } else if ((label_array[index].action == BCM_MPLS_EGRESS_ACTION_PUSH) || (label_array[index].action == BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH)) {
            *action_type = _bcm_dpp_mpls_egress_action_type_push;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label index %d without explicit operation flags 0x%x actions 0x%x"), index,label_array[index].flags,label_array[index].action)); 
        }
        
    } else {
        /* Implicit */
        /* Action POP MPLS is invalid for implicit */
        if ((label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_PHP_IPV4|BCM_MPLS_EGRESS_LABEL_PHP_IPV6|BCM_MPLS_EGRESS_LABEL_PHP_L2))) {
            *action_type = _bcm_dpp_mpls_egress_action_type_pop;
        } else if ((label_array[index].flags & (BCM_MPLS_EGRESS_LABEL_EXP_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET))) {
            /* Note: For Arad+ the EXP_SET flag is illegal, but since the above is a superset of the legal flags, there is no need for change. */
            *action_type = _bcm_dpp_mpls_egress_action_type_push;
        } else {
            /* No pop and no push then Swap */
            *action_type = _bcm_dpp_mpls_egress_action_type_swap;
        }
    }  

exit:
    BCMDNX_FUNC_RETURN;
}

/* Parameters:
   eep: the given out lif
*/
int _bcm_petra_mpls_tunnel_free_push_profile(
    int unit,
    int eep)
{
    int cur_count, cur_indx;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    int is_last;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int global_lif_id, local_lif_id;
    int rv = BCM_E_NONE;
    uint32 push_profile;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);


    if (SOC_IS_JERICHO(unit)) 
    {
        global_lif_id = eep;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        local_lif_id = global_lif_id = eep;
    }

    /* eep type is mpls. For fetching only push profile information, it is simpler to fetch the entry
       as a MPLS (not PWE) type. */
    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_lif_id, 0,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    cur_count = encap_entry_info[0].entry_val.mpls_encap_info.nof_tunnels;
    for (cur_indx = 0; cur_indx < cur_count; ++cur_indx) {
        push_profile = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[cur_indx].push_profile;
        if (push_profile != 0) {
            rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                             push_profile,
                                                             &is_last);
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_port_free_push_profile(
    int unit,
    int eep)
{
    int cur_count;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    int is_last;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(unit,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP, eep, 1,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error, EEDB entry is not PWE TYPE"))); 
    }

    cur_count = encap_entry_info[0].entry_val.mpls_encap_info.nof_tunnels;
    if (cur_count > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error, deleting PWE entry with MPLS entry"))); 
    }
    
    rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                     encap_entry_info[0].
                                                     entry_val.
                                                     pwe_info.
                                                     push_profile,
                                                     &is_last);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC 
int _bcm_petra_mpls_tunnel_switch_egress_label_from_pop_info(
    int unit,
    bcm_mpls_egress_label_t * egress_label,    
    SOC_PPC_EG_ENCAP_POP_INFO *mpls_pop_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (mpls_pop_info->pop_next_header == SOC_TMC_PKT_FRWRD_TYPE_BRIDGE) {
        egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_L2;
        if (mpls_pop_info->ethernet_info.has_cw) {
            egress_label->flags |= BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
        }
    }
    else if(mpls_pop_info->pop_next_header == SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC) {
        egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV4;             
    }
    else if(mpls_pop_info->pop_next_header == SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC) {
        egress_label->flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV6;
    }
          
    if (mpls_pop_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
        egress_label->flags |= (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET
         | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
    } else {
        egress_label->flags |= (BCM_MPLS_EGRESS_LABEL_TTL_COPY |
         BCM_MPLS_EGRESS_LABEL_EXP_COPY |
         BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
    }        

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
         if (mpls_pop_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE) {
                egress_label->flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_SET;
                egress_label->flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            } 
    }
 
#endif /* BCM_88660 */

    egress_label->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    egress_label->action = BCM_MPLS_EGRESS_ACTION_PHP;
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC 
int _bcm_petra_mpls_tunnel_switch_egress_label_to_pop_info(
    int unit,
    bcm_mpls_egress_label_t * egress_label,
    int vsi,
    SOC_PPC_EG_ENCAP_POP_INFO *mpls_pop_info)
{
    BCMDNX_INIT_FUNC_DEFS;

        mpls_pop_info->pop_type = SOC_TMC_MPLS_COMMAND_TYPE_POP;
        if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_L2) {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
            mpls_pop_info->ethernet_info.tpid_profile = 0;
            if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_CONTROL_WORD) {
                mpls_pop_info->ethernet_info.has_cw = TRUE; 
            } else {
                mpls_pop_info->ethernet_info.has_cw = FALSE; 
            }
        }
        else if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_IPV4) {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC; 
        }
        else if(egress_label->flags & BCM_MPLS_EGRESS_LABEL_PHP_IPV6) {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC; 
        } else {
            mpls_pop_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_MPLS; 
        }

        /* Enough to check only one of them (TTL/EXP since on verify it checks both) */
        if (egress_label->flags & (BCM_MPLS_EGRESS_LABEL_TTL_SET)) {
            mpls_pop_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
        }
        else if (egress_label->flags & (BCM_MPLS_EGRESS_LABEL_TTL_COPY)) {
            /* uniform: copy TTL and EXP, and always dec TTL */
            mpls_pop_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
        }
        mpls_pop_info->out_vsi = vsi;

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Add empty data EEDB entry as MPLS NOP entry
 */
int _bcm_petra_mpls_tunnel_initiator_data_nop_entry_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;
    int local_lif_id , global_lif_id;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    int flags = 0;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_sw_resources gport_sw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (num_labels > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NOP acation is supported with only one label")));
    }

    if (label_array[0].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REPLACE is not supported with NOP action")));
    }
    
    /* EEP allocation { */
    if (label_array[0].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) {
        global_lif_id = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
        flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    } else {
        global_lif_id = 0;
    }

    if (SOC_IS_JERICHO(unit)) {
        /* Set the local lif info */
        sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));
        local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_mpls_tunnel;
        local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_pwe;        
        local_out_lif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

        /* Allocate the global and local lif */
        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, flags, &global_lif_id, NULL, &local_out_lif_info);
        BCM_IF_ERROR_RETURN(rv);

        /* Retrieve the allocated local lif */
        local_lif_id = local_out_lif_info.base_lif_id;
    } else {
        /* Allocate simple mpls tunnel */
        rv = bcm_dpp_am_mpls_eep_alloc(unit, _BCM_DPP_AM_MPLS_EEP_TYPE_PWE_EGRESS_ONLY, flags, 1, &global_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        local_lif_id = global_lif_id;
    }
    /* EEP allocation } */

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeNopMplsTunnel;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, -1,local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));


    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);

    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info); 
    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, FALSE, 0);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_initiator_data_nop_entry_remove(
    int unit,
    bcm_if_t intf)
{   
    int local_lif_id , global_lif_id,eep;
    int rv = BCM_E_NONE;

    
    BCMDNX_INIT_FUNC_DEFS;
    
    eep = BCM_L3_ITF_VAL_GET(intf);
    
    if (SOC_IS_JERICHO(unit))
    {   
        global_lif_id = eep;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
        /*dealloc local+global lif*/
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit,global_lif_id,_BCM_GPORT_ENCAP_ID_LIF_INVALID,local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
   }
    else
    {   
        local_lif_id = global_lif_id = eep;
        rv = bcm_dpp_am_mpls_eep_dealloc(unit, eep);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Add empty data EEDB entry as MPLS  entry
 */
int _bcm_petra_mpls_tunnel_initiator_data_mpls_entry_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;
    int local_lif_id , global_lif_id;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    int flags = 0;
    int rv = BCM_E_NONE;
    int update = 0;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int next_eep_local_lif; 
    uint32         next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX] = {0};

    BCMDNX_INIT_FUNC_DEFS;

 
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("4 MPLS label push/swap operation is not enabled currently")));
    }

    if (num_labels > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Addtional MPLS label  cation is supported with only one label")));
    }

    if (!SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("4 MPLS label push/swap  is supported with only one J+/QAX above chip")));
    }

    update = label_array[0].flags & BCM_MPLS_EGRESS_LABEL_REPLACE?1:0;

    /* EEP allocation { */
    if (label_array[0].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) {
        global_lif_id = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
        flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    } else {
        global_lif_id = 0;
    }

    if (!update) {

        /* Set the local lif info */
        /*dpp_am_res_eep_mpls_tunnel*/
        sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));
        local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_mpls_tunnel;        
        local_out_lif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

        /* Allocate the global and local lif */
        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, flags, &global_lif_id, NULL, &local_out_lif_info);
        BCM_IF_ERROR_RETURN(rv);

        /* Retrieve the allocated local lif */
        local_lif_id = local_out_lif_info.base_lif_id;

        /* EEP allocation } */
        BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);
    }else {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(BCM_L3_ITF_TYPE_IS_LIF(label_array[0].l3_intf_id)){
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, (BCM_L3_ITF_VAL_GET(label_array[0].l3_intf_id)), &next_eep_local_lif);
        BCMDNX_IF_ERR_EXIT(rv);
        next_eep[0] = (uint32)next_eep_local_lif;

    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeDataMplsTunnel;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, -1,local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));


    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
    /*
       ---------------------------------------------------------------------------------------------------
      |             word 1                        |                     word0                           |
      ---------------------------------------------------------------------------------------------------
      |                    label[16:19]           | label[0:15] ,exp,bos,ttl, action(0:push,1:swap,2:nop)|
      ----------------------------------------------------------------------------------------------------
     */
    if (label_array[0].action == BCM_MPLS_EGRESS_ACTION_PUSH) {
        data_info.data_entry[0] = label_array[0].label <<16 | label_array[0].exp <<13| label_array[0].ttl <<4 | 0x1000 ;
        data_info.data_entry[1] = label_array[0].label >> 16;

    }else  if (label_array[0].action == BCM_MPLS_EGRESS_ACTION_SWAP) {
        data_info.data_entry[0] = label_array[0].label <<16 | label_array[0].exp <<13| label_array[0].ttl <<4 | 0x1000 | 1;
        data_info.data_entry[1] = label_array[0].label >> 16;
    }else  if (label_array[0].action == BCM_MPLS_EGRESS_ACTION_NOP) {
        data_info.data_entry[0] = label_array[0].label <<16 | label_array[0].exp <<13| label_array[0].ttl <<4 | 0x1000 | 2;
        data_info.data_entry[1] = label_array[0].label >> 16;
    }else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Addtional MPLS action only support push ")));
    }

    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, \
                  (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL, 1,
                      &(data_info.outlif_profile)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, next_eep[0]?TRUE:FALSE, next_eep[0]);

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


   
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_initiator_reserved_mpls_entry_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;
    int local_lif_id , global_lif_id;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    int flags = 0;
    int rv = BCM_E_NONE;
    int update = 0;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int next_eep_local_lif; 
    uint32         next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX] = {0};

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("reserved mpls label function  is supported with only one Jericho+ above chip")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("reserved mpls label function is not supported under lagacy ipv6 tunnel mode")));
    } 

    update = label_array[0].flags & BCM_MPLS_EGRESS_LABEL_REPLACE?1:0;

    /* EEP allocation { */
    if (label_array[0].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) {
        global_lif_id = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
        flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    } else {
        global_lif_id = 0;
    }

    if (!update) {

        /* Set the local lif info */
        /*dpp_am_res_eep_mpls_tunnel*/
        sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));
        local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_mpls_tunnel;        
        local_out_lif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        local_out_lif_info.app_alloc_info.application_type = (label_array[0].encap_access == bcmEncapAccessTunnel1)?bcm_dpp_am_egress_encap_app_third_mpls_tunnel:
                                                                                   (label_array[0].encap_access == bcmEncapAccessTunnel2)?bcm_dpp_am_egress_encap_app_second_mpls_tunnel:
                                                                                   bcm_dpp_am_egress_encap_app_mpls_tunnel;
        /* Allocate the global and local lif */
        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, flags, &global_lif_id, NULL, &local_out_lif_info);
        BCM_IF_ERROR_RETURN(rv);

        /* Retrieve the allocated local lif */
        local_lif_id = local_out_lif_info.base_lif_id;

        /* EEP allocation } */
        BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);
    }else {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(BCM_L3_ITF_TYPE_IS_LIF(label_array[0].l3_intf_id)){
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, (BCM_L3_ITF_VAL_GET(label_array[0].l3_intf_id)), &next_eep_local_lif);
        BCMDNX_IF_ERR_EXIT(rv);
        next_eep[0] = (uint32)next_eep_local_lif;

    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeDataMplsTunnel;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, -1,local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));


    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
    /*
       -------------------------------------------------------------------------------------------------------
      |             Label 1                        |                     Label 2                      |                      Label 3                                          |
      -------------------------------------------------------------------------------------------------------
      |                    data[19:0]            |                   data[39:20]                      |          Rsv_bit[2:1],outlif_profile[5],Rsv_bit[0],data[55:40]   |
      -------------------------------------------------------------------------------------------------------
     */

    if (num_labels >= 1) {
        data_info.data_entry[0] = label_array[0].label;
    }
    if (num_labels >= 2) {
        data_info.data_entry[0] |= (label_array[1].label & _BCM_DPP_MPLS_BITS_MASK(_BCM_DPP_MPLS_UINT32_BITS_NUM-_BCM_DPP_MPLS_LABEL_BITS_NUM)) << _BCM_DPP_MPLS_LABEL_RESERVE_LABEL1_LSB ;
        data_info.data_entry[1] = label_array[1].label>>(_BCM_DPP_MPLS_UINT32_BITS_NUM-_BCM_DPP_MPLS_LABEL_BITS_NUM) ;
    }
    if (num_labels >= 3) {
        data_info.data_entry[1] |= (label_array[2].label & _BCM_DPP_MPLS_BITS_MASK(16)) << _BCM_DPP_MPLS_LABEL_RESERVE_LABEL2_LSB ;
        /*outlifprofile[5] = label2[17]*/
        data_info.outlif_profile = ((label_array[2].label>>17)&1) <<5;
        /*rsv_bit[0] = label2[16],rsv_bits2[1:0] = label2[19:18]*/
        data_info.rsv_bits= ((label_array[2].label>>17)&6) | ((label_array[2].label>>16)&1);
    }

    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, next_eep[0]?TRUE:FALSE, next_eep[0]);

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


   
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_data_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array,
    int vsi_param, /* needed only in case intf is of type EEP*/
    int ll_eep_param, /* needed only in case intf is of type EEP*/
    int is_swap, /* needed only in case intf is of type EEP*/
    int *tunnel_eep
    ) /* needed only in case intf is of type EEP*/
{
    int eep = 0, flags = 0, phase_stage, start_indx, skip_entries, is_bind_entries, label_count;
    SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO mpls_encap_info;
    SOC_PPC_EG_ENCAP_POP_INFO mpls_pop_info;
    SOC_PPC_EG_ENCAP_SWAP_INFO mpls_swap_info;
    SOC_PPC_EG_ENCAP_DATA_INFO mpls_data_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int eep_alloced=0;
    _bcm_dpp_mpls_egress_action_type_t label_action_type;
    int indx;
    int local_lif_id , global_lif_id;
    int ll_eep = 0, tmp_indx = 0;
    int vsi = 0;
    SOC_PPC_EG_ENCAP_ENTRY_INFO   *encap_entry_info = NULL;
    uint32         next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32         nof_entries;
    uint8 nof_replace_flags = 0;
    /* If 1 then mode flag validation should be done*/
    int mode_flag_validation = 1;
    bcm_mpls_egress_label_t existing_label_entry;
    uint8 has_cw = 0, is_egress_protection;
    int first_encap_push_profile = 0;
    int first_pwe_outlif_profile = 0;
    int validate_pwe_replace = 0, pwe_label_read = 0;
    _bcm_lif_type_e usage = _bcmDppLifTypeAny;
    int next_eep_local_lif; 
    uint8 counting_profile_is_allocated;
    uint8 next_eep_is_mpls_with_data = FALSE;
    int local_lif_ids[_BCM_PETRA_MPLS_TUNNEL_LABEL_NOF];

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_array);

    soc_sand_dev_id = (unit);
    SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(&mpls_encap_info);
    SOC_PPC_EG_ENCAP_POP_INFO_clear(&mpls_pop_info);
    SOC_PPC_EG_ENCAP_SWAP_INFO_clear(&mpls_swap_info);
    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&mpls_data_info);
    sal_memset(local_lif_ids, 0, sizeof(local_lif_ids));
    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_mpls_tunnel_initiator_data_set.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_entry_info);  

    /* verify paramters */ 
    if ((num_labels < 0) || (num_labels > _BCM_PETRA_MPLS_TUNNEL_LABEL_NOF)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("support up to four labels")));
    }

    for (indx = 0; indx < num_labels; ++indx) {
        if (label_array[indx].exp > 7) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("EXP value %d in label array index %d is out of range"), label_array[indx].exp, indx));
        }
        if (label_array[indx].label > 0xfffff) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Label value is out of range in label array index %d"),indx));
        }


        /* l3_intf_id can be either tunnel_id of the next label in the stack or an outrif. In case we have an outrif, assert its range*/
        if (BCM_L3_ITF_TYPE_IS_RIF(label_array[indx].l3_intf_id) && label_array[indx].l3_intf_id >= (int)SOC_DPP_DEFS_GET(unit, nof_out_vsi)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Rif value %d is out of range in label array index %d"),label_array[indx].l3_intf_id,indx));
        }

                
        if (label_array[indx].pkt_pri != 0
            || label_array[indx].pkt_cfi != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("pkt_pri, pkt_cfi must be 0 label array index %d"),indx));
        }

#ifdef BCM_88660
        if (SOC_IS_ARADPLUS(unit)) {
            mode_flag_validation = 0;
        }
#endif /* BCM_88660 */

        if (mode_flag_validation != 0) {
            /* if both ttl and exp present then have to be same */
            /* ttl decrement ignored */
            if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Misconfiguration cant be combination of TTL SET - Pipe and EXP Copy - Uniform label array index %d"),indx));
            }

            if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_TTL_COPY) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_EXP_SET)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Misconfiguration cant be combination of TTL Copy - Uniform and EXP Set - Pipe label array index %d"),indx));
            }        
        }

        if(label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_DROP|BCM_MPLS_EGRESS_LABEL_PRI_REMARK
                                      |BCM_MPLS_EGRESS_LABEL_PRI_SET)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flags 0x%x not supported label array index %d"),label_array[indx].flags,indx));
        }

        if (label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE)) {
            if(!(label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Entropy Label Indication flag is only allowed where Entropy flag is used,"
                                                           " label array flags 0x%x label array index %d"),label_array[indx].flags,indx));
            }
            if(!SOC_IS_JERICHO_PLUS(unit) && indx 
                && (label_array[0].flags & (BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Entropy Label Indication flag is not allowed both on BoS and non-BoS labels"
                                                           " label array flags 0x%x label array index %d"),label_array[indx].flags,indx));                
            }
        }

        if (label_array[indx].qos_map_id != 0 && !BCM_INT_QOS_MAP_IS_REMARK(label_array[indx].qos_map_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("QOS MAP ID provided %d is not supported label array index %d"),label_array[indx].qos_map_id,indx));
        }

        if (BCM_QOS_MAP_PROFILE_GET(label_array[indx].qos_map_id) >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("QOS MAP ID provided %d is higher than device support, label array index %d"),label_array[indx].qos_map_id,indx));
        }

        if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) && !(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_MPLS_EGRESS_LABEL_REPLACE flag is set, then BCM_MPLS_EGRESS_LABEL_WITH_ID flag must also be set. label_array index: %d"), indx));
        }

        /* Perform Egress Protection validations */
        if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) {
            /* Don't allow two types of Out-LIF extension at the same time */
            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WIDE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_EGRESS_LABEL_WIDE and BCM_MPLS_EGRESS_LABEL_PROTECTION can't co-exist as they use the same Out-LIF extension. label_array index: %d"), indx));
            }
        }

        if ((label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_ACTION_VALID)) && (label_array[indx].action & BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH)) {
            if (!(SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_QAX_A0(unit))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH action suppoted only on Jericho_B0, QMX_B0, and above, but not QAX_A0 devices")));
            }
            if (!soc_property_get(unit, spn_MPLS_ENCAPSULATION_ACTION_SWAP_OR_PUSH_ENABLE, 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH action suppoted only when soc property mpls_encapsulation_action_swap_or_push_enable is set")));
            }
        }

        if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE){
            /* Check that OutLIF is MPLS tunnel or PWE in innermost label (in SW DB) */
            eep = BCM_L3_ITF_VAL_GET(label_array[indx].tunnel_id);
            if (SOC_IS_JERICHO(unit)) 
            {
                global_lif_id = eep;
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
                local_lif_ids[indx] = local_lif_id;
            }
            else
            {
                local_lif_id = eep;
            }

            BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif_id, NULL, &usage) );
            if (usage != _bcmDppLifTypeMplsTunnel) {
                if ((indx == 0) && (usage == _bcmDppLifTypeMplsPort)) {
                    /* Replace request for PWE outermost label */
                    validate_pwe_replace = 1; 
                } else if ((indx == 1) && (usage == _bcmDppLifTypeMplsPort)
                           && (eep == BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id))) {
                    /* Replace request for PWE second outermost label when using the same EEP */
                    validate_pwe_replace = 1;
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Wrong outLIF type to REPLACE in label %d (%d)"),indx, usage)); 
                }
            }
            nof_replace_flags++;
        }

        if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) && (BCM_L3_ITF_VAL_GET(label_array[indx].tunnel_id) == 0)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_MPLS_EGRESS_LABEL_WITH_ID is set, tunnel_id must be different than 0. label_array index: %d"), indx));
        }

        if (!validate_pwe_replace) {
            /* In tunnel, control word is allowed on IML and EVPN encapsulation only.*/
            if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0) &&
                  ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_IML) || (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_EVPN)))) {
                if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_CONTROL_WORD) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_EGRESS_LABEL_CONTROL_WORD can be used for IML/EVPN only in tunnel encapsulation."))); 
                }
            }
        }
    }

    if (nof_replace_flags > 0 && nof_replace_flags != num_labels ){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_MPLS_EGRESS_LABEL_REPLACE flag is set in one label, it must be set in all labels.")));
    }

    if (validate_pwe_replace) {
        /* PWE replace takes place in this api only if this soc property is unset
           If it is set, a pwe binded with mpls entry must be created only via bcm_mpls_port_add */
        if (soc_property_get(unit, spn_MPLS_BIND_PWE_WITH_MPLS_ONE_CALL, 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                              (_BSL_BCM_MSG("In case mpls_bind_pwe_with_mpls_one_call is set, a pwe binded with mpls entry can be created only via bcm_mpls_port_add")));
        }
        /* A PWE label is being replaced. Make sure only permitted fields are changed */

        /* Read label 0 (The only one that can be PWE */
        bcm_mpls_egress_label_t_init(&existing_label_entry);
        rv = bcm_petra_mpls_tunnel_initiator_get(unit, label_array[0].tunnel_id, 1, &existing_label_entry, &pwe_label_read);
        BCMDNX_IF_ERR_EXIT(rv);

        if (pwe_label_read < 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Didn't get the PWE label to REPLACE.")));
        }

        /* Validate all fields except l3_intf_id */
        if (
            (existing_label_entry.label != label_array[0].label)
            || (existing_label_entry.qos_map_id  != label_array[0].qos_map_id)
            || (existing_label_entry.exp         != label_array[0].exp)
            || (existing_label_entry.ttl         != label_array[0].ttl)
            || (existing_label_entry.pkt_pri     != label_array[0].pkt_pri)
            || (existing_label_entry.pkt_cfi     != label_array[0].pkt_cfi)
            || (existing_label_entry.action      != label_array[0].action)
            || (existing_label_entry.tunnel_id   != label_array[0].tunnel_id)
            ) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can only change l3_intf_id in PWE label REPLACE.")));
        }
    }

    phase_stage = -1; 
    /* Run over all label stack. from end to start */
    for (start_indx = num_labels; start_indx > 0; start_indx = start_indx - 1 - skip_entries) {
        int local_ll_outlif = 0;
        phase_stage++;

        /* Special cases to handle two entries as one entry in EEDB:
           1. In case of two consecutive labels are push and have same tunnel ID than bind them to one entry in HW 
           2. In case of SWAP coupling: one is NOP the other is SWAP Coupling */
        rv = _bcm_mpls_egress_tunnel_is_bind_entries(unit, label_array, start_indx, &is_bind_entries);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_bind_entries) {
           skip_entries = 1;
           indx = start_indx - 2;

           /* In case of SWAP_COUPLING, use the tunnel_id of second entry. */
           if ((start_indx != 1) && /* In case start indx is 1, no case of two consecutive labels (it is the last label in stack) */
               (label_array[start_indx-2].tunnel_id == 0) && /* Tunnel ID of NOP action must be 0 */
               (label_array[start_indx-2].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-2].action == BCM_MPLS_EGRESS_ACTION_NOP) && /* NOP action on the first label */
               (label_array[start_indx-1].flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID && label_array[start_indx-1].action == BCM_MPLS_EGRESS_ACTION_SWAP)) {
               indx = start_indx - 1;
           }
        } else {
           skip_entries = 0;
           indx = start_indx - 1;
        }  

        rv = _bcm_mpls_egress_tunnel_to_egress_action_type(unit, label_array, indx, &label_action_type);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Determine protection state */
        is_egress_protection = (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) ? TRUE : FALSE;

        if (SOC_IS_JERICHO(unit)) {
            /* verify protection state doesn't change */
            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                bcm_mpls_egress_label_t_init(&existing_label_entry);
                rv = bcm_petra_mpls_tunnel_initiator_get(unit, label_array[0].tunnel_id, 1, &existing_label_entry, &label_count);
                if (is_egress_protection) {
                    if (!(existing_label_entry.flags & BCM_MPLS_EGRESS_LABEL_PROTECTION)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel ID given is already allocated as non protection and can't replace to protection")));
                    }
                }
                else {
                    if (existing_label_entry.flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel ID given is already allocated as protection and can't replace to non protection")));
                    }
                }
            }
        }
        /* EEP allocation { */
        if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) {
            eep = BCM_L3_ITF_VAL_GET(label_array[indx].tunnel_id);
        }
        else {
            eep = 0;
        }        

        if ((label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
            /* free usage of push profiles */
            rv = _bcm_petra_mpls_tunnel_free_push_profile(unit, eep);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (eep != 0) { /* Allocate or Already exist */
            flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            rv = (SOC_IS_JERICHO(unit)) ?  bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, eep) : bcm_dpp_am_mpls_eep_is_alloced(unit,eep);
            if (rv == BCM_E_PARAM) {
                BCMDNX_IF_ERR_EXIT(rv);
            }

            eep_alloced = (rv == BCM_E_EXISTS)?1:0;    
            
            if (SOC_IS_JERICHO(unit))
            {
                global_lif_id = eep;
                if (eep_alloced) {
                    if (local_lif_ids[indx])
                    {
                        local_lif_id = local_lif_ids[indx];
                    } 
                    else
                    {
                        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
                        BCMDNX_IF_ERR_EXIT(rv);
                        local_lif_ids[indx] = local_lif_id;
                    }
                }
            }
            else
            {
                local_lif_id = global_lif_id = eep;
            }

            /* Can't replace non - existing tunnel */
            if (!eep_alloced && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Tunnel id to be replaced %d does not exist, label array index %d"),label_array[indx].tunnel_id,indx));
            }

            /* Can't replace an existing tunnel with no replace flag*/
            if (eep_alloced && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WITH_ID) && !(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Tunnel id %d already exists but replace flag is absent, label array index %d. "),label_array[indx].tunnel_id,indx));
            }
		} else {                    /* if not tunneled alloc new eep */
            flags = 0;
            eep_alloced = 0;
        }

        if(!eep_alloced) {
            /* 
             * allocate entry, from MPLS encap DB, to be used for swap/pop/push 
             * phase_stage indicates if it is the first or second entry. 
             * last entry points to LL encap.
             * Note, for 2 EEDB entries case, the 2nd DB points to Link layer.
             */
            if ((indx+is_bind_entries+1 == num_labels) 
                && BCM_L3_ITF_TYPE_IS_LIF(label_array[indx+is_bind_entries].l3_intf_id)
                && num_labels < 3) { /* tunnel points to tunnel */

                    if (SOC_IS_JERICHO_PLUS(unit)) 
                    {
                         /*
                            * when pushing four seperate mpls label, 
                            * legacy 1st mpls tunnel use stage 2;
                            * legacy 2nd mpls tunnel use stage 4;
                            * data entry mpls tunnel use stage 3
                            */

                        int next_eep_global_lif, next_eep_local_lif;
                       _bcm_lif_type_e next_eep_usage = _bcmDppLifTypeAny;

                        next_eep_global_lif = BCM_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id);
                        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep_global_lif, &next_eep_local_lif);
                        BCMDNX_IF_ERR_EXIT(rv);
                        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, next_eep_local_lif, NULL, &next_eep_usage) );
                        if (next_eep_usage ==_bcmDppLifTypeDataMplsTunnel) {
                            next_eep_is_mpls_with_data = TRUE;
                        }

                    } 
                 phase_stage++;
            }

            if (phase_stage > 1) { /* no more than two rounds i.e. only two entries are possible */
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Too many mpls labels are in stack. Supported up to two mpls entries")));
            }

            if (SOC_IS_JERICHO(unit))
            {
                bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
                SOC_PPC_EG_ENCAP_ENTRY_TYPE ll_entry_type; 

                /* get local LL outlif from global LL outlif.
                the interface object l3_intf_id of type encap contains global LL outlif */
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id), &local_ll_outlif);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Verify whether ll entry is of type linker layer */
                rv = soc_ppd_eg_encap_entry_type_get(unit, local_ll_outlif, &ll_entry_type);
                BCM_IF_ERROR_RETURN(rv);

                /* Set the local lif info */
                global_lif_id = eep;
                sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));
                local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_mpls_tunnel;

                if (ll_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP \
                    && SOC_IS_ROO_ENABLE(unit) && SOC_IS_JERICHO_AND_BELOW(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
                    local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_evpn;
                }else if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0) == 1)
                &&SOC_IS_JERICHO_PLUS(unit) && next_eep_is_mpls_with_data){
                    local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_third_mpls_tunnel;
                }else if ( SOC_IS_JERICHO(unit) && label_array[indx].encap_access > bcmEncapAccessInvalid) {
                    /*
                     * User could indicate the bank pahse explicitly now
                     * by setting encap_access other than bcmEncapAccessInvalid
                     * bcmEncapAccessNativeArp : native link layer(1),only relevant in J+/Qax
                     * bcmEncapAccessTunnel1 : MPLS tunnel 1(3)
                     * bcmEncapAccessTunnel2 : MPLS tunnel 2(4)
                     */
                    if (SOC_IS_JERICHO_AND_BELOW(unit) && label_array[indx].encap_access == bcmEncapAccessNativeArp) {
                       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NativeARP is only supported in J+")));
                    }

                    local_out_lif_info.app_alloc_info.application_type = (label_array[indx].encap_access == bcmEncapAccessNativeArp)?bcm_dpp_am_egress_encap_app_third_mpls_tunnel:
                                                                                               (label_array[indx].encap_access == bcmEncapAccessTunnel1)?bcm_dpp_am_egress_encap_app_second_mpls_tunnel:bcm_dpp_am_egress_encap_app_mpls_tunnel;
                }else {
                    local_out_lif_info.app_alloc_info.application_type = (phase_stage == 0)? bcm_dpp_am_egress_encap_app_mpls_tunnel : bcm_dpp_am_egress_encap_app_second_mpls_tunnel;
                }
                local_out_lif_info.local_lif_flags = (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_WIDE) ? BCM_DPP_AM_OUT_LIF_FLAG_WIDE : 0;

                /* Egress protection MPLS Tunnel LIFs should be assigned from an egress protection EEDB bank */
                if (is_egress_protection) {

                    /* Format the Local Out-LIF allocation attributes in order
                       to get the required Egress Protection Out-LIF */
                    local_out_lif_info.local_lif_flags |= BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
                    DPP_FAILOVER_ID_GET(local_out_lif_info.failover_id, label_array[indx].egress_failover_id);
                }
                /* 
                 * Use-LIF-counting-profile?
                 */
                rv = bcm_dpp_counter_lif_range_is_allocated(unit, &counting_profile_is_allocated);
                BCMDNX_IF_ERR_EXIT(rv);

                if (counting_profile_is_allocated) {
                    if (label_array[indx].outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE && label_array[indx].outlif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OutLIF-counting-profile %d is out of range."), label_array[indx].outlif_counting_profile));
                    }
                    BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, label_array[indx].outlif_counting_profile, &local_out_lif_info.counting_profile_id));                    
                } else {
                    local_out_lif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
                }

                /* Allocate the global and local lif */
                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, flags, &global_lif_id, NULL, &local_out_lif_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Retrieve the allocated local lif */
                local_lif_id = local_out_lif_info.base_lif_id;


            }
            else
            {
                /* Allocate simple or second mpls tunnel */
                rv = bcm_dpp_am_mpls_eep_alloc(unit, ((phase_stage == 0)? _BCM_DPP_AM_MPLS_EEP_TYPE_SIMPLE_MPLS_TUNNEL:_BCM_DPP_AM_MPLS_EEP_TYPE_SECOND_MPLS_TUNNEL), flags, 1, &eep);
                BCMDNX_IF_ERR_EXIT(rv);

                local_lif_id = global_lif_id = eep;
            }
        }
        BCM_L3_ITF_SET(label_array[indx].tunnel_id,BCM_L3_ITF_TYPE_LIF,global_lif_id);
        if (is_bind_entries) {
            BCM_L3_ITF_SET(label_array[indx+1].tunnel_id,BCM_L3_ITF_TYPE_LIF,global_lif_id);
        }
        /* EEP allocation } */

        if (!(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
            /* Update sw db */
            rv = _bcm_dpp_out_lif_mpls_tunnel_match_add(soc_sand_dev_id, local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* fill EEP { */

        /* fill EEP with next pointer information { */
        nof_entries = 0;
        if (eep_alloced) {
            soc_sand_rv =
              soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                     SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                     local_lif_id, 1,
                                     encap_entry_info, next_eep,
                                     &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            first_encap_push_profile = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[0].push_profile;
			/* in case of PWE, need to make sure w epreserve the outlif profile */
			if ((usage == _bcmDppLifTypeMplsPort) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
                first_pwe_outlif_profile = encap_entry_info[0].entry_val.mpls_encap_info.outlif_profile; 
            }
		}


        if (nof_entries > 0 && 
            (!_BCM_DPP_MPLS_EEDB_TYPES(encap_entry_info[0].entry_type)) && /* Not MPLS types */
            encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE && /* Not PWE types */
            encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_NULL) { /* Not defined */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel ID given is already allocated with type that is different than MPLS")));
        }

        /* check if there is tunnel associated with intf at all */
        if (((indx+1+is_bind_entries) == num_labels) & BCM_L3_ITF_TYPE_IS_LIF(label_array[indx+is_bind_entries].l3_intf_id)) { /* Special case: interface in last label is Tunnel */
            if (SOC_IS_JERICHO(unit))
            {
                if (local_ll_outlif) 
                {
                    next_eep[0] = (uint32)local_ll_outlif;
                } 
                else
                {
                    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, (BCM_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id)), &next_eep_local_lif);
                    BCMDNX_IF_ERR_EXIT(rv);
                    next_eep[0] = (uint32)next_eep_local_lif;
                }
            }
            else
            {
                next_eep[0] = (BCM_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id));
            }
            vsi = 0;
        } else if (((indx+1+is_bind_entries) == num_labels)) { /* last entry it should point to ll */
            if (!(label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)){
                next_eep[0] = ll_eep;
            }
            vsi = BCM_L3_ITF_VAL_GET(label_array[indx+is_bind_entries].l3_intf_id);
        } else { /* take from stack in case of not last */
            if (SOC_IS_JERICHO(unit))
            {
                if (local_lif_ids[indx+1+is_bind_entries])
                {
                    next_eep[0] = (uint32)local_lif_ids[indx+1+is_bind_entries];
                }
                else
                {
                    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, (BCM_L3_ITF_VAL_GET(label_array[indx+1+is_bind_entries].tunnel_id)), &next_eep_local_lif);
                    BCMDNX_IF_ERR_EXIT(rv);
                    next_eep[0] = (uint32)next_eep_local_lif;
                    local_lif_ids[indx+1+is_bind_entries] = next_eep_local_lif;
                }
            }
            else
            {
                next_eep[0] = (BCM_L3_ITF_VAL_GET(label_array[indx+1+is_bind_entries].tunnel_id)); 
            }
            vsi = 0;
        }
        /* fill EEP with next pointer information } */
        

        /* fill EEP with encap information, include oam_lif_set and drop for repleaced entries. { */
        if (label_action_type == _bcm_dpp_mpls_egress_action_type_swap) { /* if swap */
            mpls_swap_info.out_vsi = vsi;
            mpls_swap_info.swap_label = label_array[indx].label;


            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_swap_info.oam_lif_set = encap_entry_info[0].entry_val.swap_info.oam_lif_set;
                mpls_swap_info.drop = encap_entry_info[0].entry_val.swap_info.drop;
            }

            mpls_swap_info.remark_profile = BCM_QOS_MAP_PROFILE_GET(label_array[indx].qos_map_id);
            soc_sand_rv =
                soc_ppd_eg_encap_swap_command_entry_add(soc_sand_dev_id, local_lif_id, &mpls_swap_info, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else if(label_action_type == _bcm_dpp_mpls_egress_action_type_pop) { /* if pop */
            rv = _bcm_petra_mpls_tunnel_switch_egress_label_to_pop_info(unit, &(label_array[indx]), vsi, 
                                                           &(mpls_pop_info));
            BCM_IF_ERROR_RETURN(rv);

            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_pop_info.oam_lif_set = encap_entry_info[0].entry_val.pop_info.oam_lif_set;
                mpls_pop_info.drop = encap_entry_info[0].entry_val.pop_info.drop;
            }

            soc_sand_rv =
                soc_ppd_eg_encap_pop_command_entry_add(soc_sand_dev_id, local_lif_id, &mpls_pop_info, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else { /* if encap push */

            mpls_encap_info.nof_tunnels = 1+is_bind_entries;

            /* Set information for the first tunnel. Check if has_cw is set in push profile.
             * If it does this label is VC label and the new push profile will have CW as well. */
            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                rv = _bcm_mpls_tunnel_push_profile_info_get(unit,
                                                  first_encap_push_profile, 
                                                  &has_cw,
                                                  &existing_label_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_CONTROL_WORD) {
                has_cw = 1;
            }
            else {
                has_cw = 0;
            }

            rv = _bcm_mpls_tunnel_bcm_to_ppd(unit, &label_array[indx], has_cw,
                                              &mpls_encap_info.tunnels[0],0);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Set information of the other tunnels if exist */
            for (tmp_indx = 1; tmp_indx < mpls_encap_info.nof_tunnels; ++tmp_indx) {
                rv = _bcm_mpls_tunnel_bcm_to_ppd(unit, &label_array[indx+tmp_indx], 0 /*has_cw*/,
                                                  &mpls_encap_info.tunnels[tmp_indx], tmp_indx%2);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE) {
                mpls_encap_info.oam_lif_set = encap_entry_info[0].entry_val.mpls_encap_info.oam_lif_set;
                mpls_encap_info.drop = encap_entry_info[0].entry_val.mpls_encap_info.drop;
            }

            /* set VSI for last EEP */
            mpls_encap_info.out_vsi =  vsi;

            if (SOC_IS_JERICHO(unit)) {
                /* 
                 *  Outlif Profile configuration
                 */
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                    /* In this mode the orientation should be saved to the outlif profile */
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, 1,
                                          &(mpls_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
                if (soc_property_get(unit, spn_EVPN_ENABLE, 0) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_IML)) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_EVPN, 1,
                                          &(mpls_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }

                if ((!SOC_IS_JERICHO_PLUS(unit) || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "el_eli_jr_mode", 0)) 
                       &&  soc_property_get(unit, spn_MPLS_EGRESS_LABEL_ENTROPY_INDICATOR_ENABLE, 0)) {                        
                        /* Set outlif profile ELI bit to select the PRGE program that generates ELI */
                        if (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE) {
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                                    (unit, SOC_OCC_MGMT_TYPE_OUTLIF, 
                                                      SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_INDICATION, 1,
                                                        &(mpls_encap_info.outlif_profile)));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                        } else if (label_array[indx+skip_entries].flags & BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE){
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                                (unit, SOC_OCC_MGMT_TYPE_OUTLIF, 
                                                 SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_NON_BOS_INDICATION, 1,
                                                   &(mpls_encap_info.outlif_profile)));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                    }
                }

                if ((SOC_IS_JERICHO_B0_AND_ABOVE(unit) && !SOC_IS_QAX_A0(unit)) &&
                    (label_array[indx].flags & (BCM_MPLS_EGRESS_LABEL_ACTION_VALID)) &&
                    (label_array[indx].action == BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH) &&
                    soc_property_get(unit, spn_MPLS_ENCAPSULATION_ACTION_SWAP_OR_PUSH_ENABLE, 0)) {
					/* Set outlif profile to enable push to swap change */
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_MPLS_PUSH_OR_SWAP, 1,
                                          &(mpls_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }


                /* update is_l2_lif in outlif profile.
                   Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
                if (SOC_IS_JERICHO_PLUS(unit) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_EVPN)) {
                    /* set entry to be L2LIF */
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                                          &(mpls_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }

                /* Set Egress Protection information */
                if (is_egress_protection) {
                    DPP_FAILOVER_ID_GET(mpls_encap_info.protection_info.protection_pointer, label_array[indx].egress_failover_id);
                    mpls_encap_info.protection_info.protection_pass_value = (label_array[indx].egress_failover_if_id) ? 0 : 1;
                    mpls_encap_info.protection_info.is_protection_valid = TRUE;
                }
            }
            /* In ARAD: orietntation is being set in different API, per outlif. */
            else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                if (SOC_DPP_CONFIG(unit)->arad->pp_op_mode.split_horizon_filter_enable) {
                    soc_sand_rv =
                      soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(soc_sand_dev_id, local_lif_id, SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }

            /* if this is an adjacent entry of a PWE entry, we are changing just the MPLS but not the PWE,
               we want to make sure the outlif profile of the PWE doesnt change */
            if (indx == 0 && eep_alloced && (usage == _bcmDppLifTypeMplsPort) && (label_array[indx].flags & BCM_MPLS_EGRESS_LABEL_REPLACE)) {
                mpls_encap_info.outlif_profile =  first_pwe_outlif_profile;
            }

            soc_sand_rv =
            /* add encap entry */
            soc_ppd_eg_encap_mpls_entry_add(soc_sand_dev_id, local_lif_id, &mpls_encap_info, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        /* fill EEP with encap information { */
    }

    *tunnel_eep = eep;

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
     BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_mpls_tunnel_initiator_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int tunnel_eep;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (intf != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf parameter must be set to 0")));
    }

    /*
      * BCM_MPLS_EGRESS_LABEL_PRESERVE used for mpls encapsulation
      * with Data entry
      * There are 2 scenario:
      * 1. Push 4 separate labels, Data entry carry one mpls label and encapsulated by PRGE
      * 2. Push 7 mpls labels, 3 of them inserted by reserved mpls profile mechanism.
      */

    if (label_array[0].flags & BCM_MPLS_EGRESS_LABEL_PRESERVE ) {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0)) {
            rv = _bcm_petra_mpls_tunnel_initiator_data_mpls_entry_set(unit,
                                                                     intf,
                                                                     num_labels,
                                                                     label_array);
        } else {
            rv = _bcm_petra_mpls_tunnel_initiator_reserved_mpls_entry_set(unit,
                                                                     intf,
                                                                     num_labels,
                                                                     label_array);
      }
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }


    if (label_array[0].action == BCM_MPLS_EGRESS_ACTION_NOP) {
        rv = _bcm_petra_mpls_tunnel_initiator_data_nop_entry_set(unit,
                                                                 intf,
                                                                 num_labels,
                                                                 label_array);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        rv = bcm_petra_mpls_tunnel_initiator_data_set(
            unit,
            intf,
            num_labels,
            label_array,
            0,  /* vsi */
            0,  /* ll-eep */
            0, /* is-swap */
            &tunnel_eep);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    rv = _bcm_petra_mpls_tunnel_initiator_set(unit, intf, num_labels, label_array);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
  exit:
    BCMDNX_FUNC_RETURN;
}


/* note after this user has to call egress-object create with id/update */
int bcm_petra_mpls_tunnel_initiator_clear(
    int unit,
    bcm_if_t intf)
{
    int eep, is_tunneled;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    int vsi, ll_eep;
    SOC_PPC_FEC_ID fec_id;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE
      protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
      working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
      protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO
      protect_info;
    int encap_itf;
    uint8 success;
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;    
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;    
    _bcm_lif_type_e usage = 0;
    int global_lif_id, local_lif_id;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* check if allocated eep */
    if (BCM_L3_ITF_TYPE_IS_LIF(intf)) {
        eep = BCM_L3_ITF_VAL_GET(intf);
        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, eep);
        } else {
            rv = bcm_dpp_am_mpls_eep_is_alloced(unit,eep);
        }
        if (rv != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given L3_ITF not exist: 0x%08X!"), intf));
        }
    }

    /* check if there is tunnel associated with intf at all */
    rv = _bcm_tunnel_intf_is_tunnel(unit, intf, &is_tunneled, &eep, &vsi, &ll_eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * Dont remove PWE entries
     */ 

    if (SOC_IS_JERICHO(unit)) 
    {
        global_lif_id = eep;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        local_lif_id = global_lif_id = eep;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif_id, NULL, &usage));
    if ((usage == _bcmDppLifTypeNopMplsTunnel)
        || (usage == _bcmDppLifTypeDataMplsTunnel)) { /* OutLIF is nop mpls tunnel */
        rv = _bcm_petra_mpls_tunnel_initiator_data_nop_entry_remove(unit,intf);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }
    if (usage != _bcmDppLifTypeMplsTunnel) { /* OutLIF is not MPLS tunnel - nothing to do */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not MPLS tunnel")));
    } 

    /* if not then no labels */
    if (is_tunneled == 0) {
        BCM_EXIT;
    }

    /* free push profile used by these tunnel */
    rv = _bcm_petra_mpls_tunnel_free_push_profile(unit, eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* remove tunnel */
    if(BCM_L3_ITF_TYPE_IS_RIF(intf)) {
       /* mark this interface as not tunneled */
        rv = _bcm_tunnel_intf_eep_set(unit, intf, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if(BCM_L3_ITF_TYPE_IS_FEC(intf)) {
        _bcm_l3_intf_to_fec(unit, intf, &fec_id);

        soc_sand_rv =
            soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id, fec_id,
                                    &protect_type, &working_fec,
                                    &protect_fec, &protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_L3_ITF_SET(encap_itf, BCM_L3_ITF_TYPE_LIF,working_fec.eep);
        rv = _bcm_tunnel_intf_is_tunnel(unit,encap_itf,  &is_tunneled, &eep, &vsi, &ll_eep);
        BCMDNX_IF_ERR_EXIT(rv);

        working_fec.eep = ll_eep;

        working_fec.app_info.out_rif = vsi;
        soc_sand_rv =
            soc_ppd_frwrd_fec_entry_add(soc_sand_dev_id, fec_id,
                                    protect_type, &working_fec,
                                    &protect_fec, &protect_info, &success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    } else {
        BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
            "bcm_petra_mpls_tunnel_initiator_clear.encap_entry_info");
        if(encap_entry_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }
        soc_sand_rv =
            soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                               local_lif_id, 1,
                               encap_entry_info, next_eep,
                               &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (nof_entries > 0 && (_BCM_DPP_MPLS_EEDB_TYPES(encap_entry_info[0].entry_type))) {

            soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP,
                               local_lif_id);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            
        } else {
            /* Nothing to do */
            BCM_EXIT;
        }
    }

    /* free memory */
    if (SOC_IS_JERICHO(unit)) 
    {
        /*dealloc local+global lif*/
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit,global_lif_id,_BCM_GPORT_ENCAP_ID_LIF_INVALID,local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else    
    {
        rv = bcm_dpp_am_mpls_eep_dealloc(unit, eep);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_clear_all(
    int unit)
{
    int indx;
    int rv = BCM_E_NONE;
    bcm_if_t eep;
    _bcm_lif_type_e lif_usage;
    int global_lif;
    
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    /* Traverse over all outlifs */
    for (indx = _BCM_DPP_GPORT_LIF_SW_RESOURCES_EGRESS_ITER_START(unit); 
          indx < _BCM_DPP_GPORT_LIF_SW_RESOURCES_EGRESS_ITER_END(unit); 
          indx+=2 /* MPLS always even */) {
        /* Get LIF usage */
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, indx, NULL, &lif_usage));
        if (lif_usage == _bcmDppLifTypeMplsTunnel) {
            if (SOC_IS_JERICHO(unit)) 
            {
                rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, indx, &global_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else
            {
                global_lif = indx;
            }
            BCM_L3_ITF_SET(eep, BCM_L3_ITF_TYPE_LIF, global_lif);
            rv = bcm_petra_mpls_tunnel_initiator_clear(unit, eep); 
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_mpls_egress_label_t * label_array,
    int *label_count)
{
    int eep, is_tunneled;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO mpls_tunnel_info;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    int indx, cur_indx;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int cur_count = 0;
    int rv = BCM_E_NONE;
    int ll_eep, vsi;
    int tunnel_id;
    _bcm_lif_type_e lif_usage;
    int global_lif_id = 0, local_lif_id = 0;
    uint8 is_egress_protection;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(label_array);
    BCMDNX_NULL_CHECK(label_count);

    soc_sand_dev_id = (unit);
    SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_clear(&mpls_tunnel_info);

    if (!BCM_L3_ITF_TYPE_IS_LIF(intf)) { /* Must be OutLIF */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf parameter must be ENCAP type (OutLIF)")));        
    }

    /* check if there is tunnel associated with intf at all */
    rv = _bcm_tunnel_intf_is_tunnel(unit, intf, &is_tunneled, &eep, &vsi, &ll_eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /*convert eep to local lif*/
    if (SOC_IS_JERICHO(unit)) 
    {
        global_lif_id = eep;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        local_lif_id = global_lif_id = eep;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(soc_sand_dev_id, -1, local_lif_id, NULL, &lif_usage));
    if ((is_tunneled == 0)
           &&(lif_usage != _bcmDppLifTypeDataMplsTunnel)){
        *label_count = 0;
        rv = BCM_E_NOT_FOUND;
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* 
      * if not legacy mpls tunnel and not _bcmDppLifTypeDataMplsTunnel,then no labels 
      */
    if ((lif_usage != _bcmDppLifTypeMplsTunnel) 
        && (lif_usage != _bcmDppLifTypeMplsPort)
        && (lif_usage != _bcmDppLifTypeDataMplsTunnel)) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not MPLS")));
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_mpls_tunnel_initiator_get.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_lif_id, 2,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA) {
        /*MPLS tunnel using data entry*/
        if (lif_usage == _bcmDppLifTypeDataMplsTunnel) {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PRESERVE;
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0)) {
                label_array[0].action = (encap_entry_info[0].entry_val.data_info.data_entry[0] & 0x1)? BCM_MPLS_EGRESS_ACTION_SWAP:BCM_MPLS_EGRESS_ACTION_PUSH;
                label_array[0].label = ((encap_entry_info[0].entry_val.data_info.data_entry[0] >> 16) & 0xFFFF) | (encap_entry_info[0].entry_val.data_info.data_entry[1] << 16) ;
                label_array[0].ttl= (encap_entry_info[0].entry_val.data_info.data_entry[0] >> 4) & 0xFF ;
                label_array[0].exp= (encap_entry_info[0].entry_val.data_info.data_entry[0] >> 13) & 0x7 ;
                *label_count = 1;
            } else {
                label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
                /*label1 = data[19:0]*/
                label_array[0].label = encap_entry_info[0].entry_val.data_info.data_entry[0] & _BCM_DPP_MPLS_BITS_MASK(_BCM_DPP_MPLS_LABEL_BITS_NUM);
                if (label_max > 1) {
                    /*label2 = data[39:20]*/
                    label_array[1].label = encap_entry_info[0].entry_val.data_info.data_entry[0]>>_BCM_DPP_MPLS_LABEL_BITS_NUM | ((encap_entry_info[0].entry_val.data_info.data_entry[1] & _BCM_DPP_MPLS_BITS_MASK(_BCM_DPP_MPLS_LABEL_RESERVE_LABEL2_LSB)) << (_BCM_DPP_MPLS_UINT32_BITS_NUM-_BCM_DPP_MPLS_LABEL_BITS_NUM)) ;
                    *label_count = 2;
                }
                if (label_max > 2) {
                    /*label3 = {rsv_bit2[1:0],outlifprofile[5],rsv_bit[0],data[55:40]}*/
                    label_array[2].label = ((encap_entry_info[0].entry_val.data_info.data_entry[1]>>_BCM_DPP_MPLS_LABEL_RESERVE_LABEL2_LSB) & _BCM_DPP_MPLS_BITS_MASK(16)) |  
                                                      ((encap_entry_info[0].entry_val.data_info.outlif_profile & 0x20) << 11) |
                                                     ((encap_entry_info[0].entry_val.data_info.rsv_bits & 1)  << 16) |
                                                     ((encap_entry_info[0].entry_val.data_info.rsv_bits & 6)  << 17);
                    /*set label[17]*/
                    label_array[2].label = label_array[2].label | ((encap_entry_info[0].entry_val.data_info.outlif_profile >> 5) & 1) << 17;
                    *label_count = 3;
                }
            }
        } else {
            /* NULL data entry */
            label_array[0].action = BCM_MPLS_EGRESS_ACTION_NOP;
            *label_count = 1;
        }
    }
    else {

        /* initial value */
        *label_count = 0;

        for (indx = 0; indx < nof_entries; ++indx) {

            if (indx == 0) {
                BCM_L3_ITF_SET(tunnel_id,BCM_L3_ITF_TYPE_LIF,global_lif_id);
            } else {
                if (SOC_IS_JERICHO(unit)) {
                    rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[indx-1], &global_lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else{
                    global_lif_id = next_eep[indx-1];
                }
                BCM_L3_ITF_SET(tunnel_id,BCM_L3_ITF_TYPE_LIF,global_lif_id);
            }

            /* Reset Egress Protection info */
            label_array[*label_count].egress_failover_id = 0;
            label_array[*label_count].egress_failover_if_id = 0;

            if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {  /* mpls encap */
                cur_count =
                    encap_entry_info[indx].entry_val.mpls_encap_info.nof_tunnels;
                for (cur_indx = 0; cur_indx < cur_count; ++cur_indx) {
                    mpls_tunnel_info.tunnel_label =
                        encap_entry_info[indx].entry_val.mpls_encap_info.
                        tunnels[cur_indx].tunnel_label;
                    mpls_tunnel_info.push_profile =
                        encap_entry_info[indx].entry_val.mpls_encap_info.
                        tunnels[cur_indx].push_profile;
                    
                    rv = _bcm_mpls_tunnel_bcm_from_ppd(unit,
                                                       &label_array
                                                       [*label_count],
                                                       &mpls_tunnel_info);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (SOC_IS_JERICHO(unit)) {
                        uint8 is_wide_entry = 0, ext_type;
                        if (soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
                            uint32 val;
                            /* Checking whether outlif profile EVPN bit is set */
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_EVPN, \
                                 &(encap_entry_info[indx].entry_val.mpls_encap_info.outlif_profile),&val));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            if (val) { /* if evpn bit is on, return Inclusive multicast label indication*/
                                label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_IML;
                            }
                        }

                        if ((!SOC_IS_JERICHO_PLUS(unit)) &&
                            soc_property_get(unit, spn_MPLS_EGRESS_LABEL_ENTROPY_INDICATOR_ENABLE, 0)) {
                            uint32 val1,val2;
                            /* Checking whether outlif profile ELI bit is set */
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,(unit, SOC_OCC_MGMT_TYPE_OUTLIF, \
                                                                                              SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_INDICATION, \
                                                                                              &(encap_entry_info[indx].entry_val.mpls_encap_info.outlif_profile),&val1));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,(unit, SOC_OCC_MGMT_TYPE_OUTLIF, \
                                                                                              SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_NON_BOS_INDICATION, \
                                                                                              &(encap_entry_info[indx].entry_val.mpls_encap_info.outlif_profile),&val2));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

                            if (val1 && (SOC_IS_JERICHO_PLUS(unit) || (!SOC_IS_JERICHO_PLUS(unit) )) && !cur_indx) { /* if ELI bit is on, add the relevant flag */
                                label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
                            }

                            if (val2 && (SOC_IS_JERICHO_PLUS(unit) || (!SOC_IS_JERICHO_PLUS(unit) )) && cur_indx) { /* Imposit ELI at non BoS, add the relevant flag */
                                label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE|BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
                            }

                        }

                        if (((SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit) || SOC_IS_JERICHO_PLUS(unit)) && !SOC_IS_QAX(unit)) &&
                            soc_property_get(unit, spn_MPLS_ENCAPSULATION_ACTION_SWAP_OR_PUSH_ENABLE, 0)) {

                            uint32 val;
                            /* Checking whether outlif profile Push or Swap bit is set */
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,(unit, SOC_OCC_MGMT_TYPE_OUTLIF, \
                                                                                              SOC_OCC_MGMT_OUTLIF_APP_MPLS_PUSH_OR_SWAP, \
                                                                                              &(encap_entry_info[indx].entry_val.mpls_encap_info.outlif_profile),&val));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                            if (val) { /* if Push or Swap bit is on, add the relevant action */
                                label_array[*label_count].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
                                label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
                            }
                        }

                        /* for ROO application, 1b in outlif profile indicate if the mpls tunnel is a L2_LIF. (EVPN tunnel in mpls tunnel case) */
                        if (SOC_IS_JERICHO_PLUS(unit)) {
                            uint32 val;
                            /* Checking whether outlif profile ELI bit is set */
                            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,(unit, SOC_OCC_MGMT_TYPE_OUTLIF, \
                                                                                              SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, \
                                                                                              &(encap_entry_info[indx].entry_val.mpls_encap_info.outlif_profile),&val));
                            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                            if (val) {
                                label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_EVPN; 
                            }
                        }

                        /* Fetch local outlif from global lif */
                        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
                        BCMDNX_IF_ERR_EXIT(rv);

                        /* Verifying whether this tunnel is a wide entry. Verification is done with local out lif id */
                        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, local_lif_id, FALSE, &is_wide_entry, &ext_type));
                        BCMDNX_IF_ERR_EXIT(rv);

                        /* Identify if it's a Protection or other extension */
                        is_egress_protection = (is_wide_entry && (ext_type == 0)) ? TRUE :FALSE;

                        /* Determine if it's a non-protection wide entry */
                        label_array[*label_count].flags |= (is_wide_entry && !is_egress_protection) ? BCM_MPLS_EGRESS_LABEL_WIDE : 0;

                        /* In case of an Egress Protection */
                        if (is_egress_protection && encap_entry_info[indx].entry_val.mpls_encap_info.protection_info.is_protection_valid) {

                            /* Set the protection information */
                            DPP_FAILOVER_TYPE_SET(label_array[*label_count].egress_failover_id,
                                encap_entry_info[indx].entry_val.mpls_encap_info.protection_info.protection_pointer, DPP_FAILOVER_TYPE_ENCAP);
                            label_array[*label_count].egress_failover_if_id =
                                (encap_entry_info[indx].entry_val.mpls_encap_info.protection_info.protection_pass_value) ? 0 : 1;
                            label_array[*label_count].flags |= BCM_MPLS_EGRESS_LABEL_PROTECTION;
                        }
                    }
                    
                    /* Update label id */
                    label_array[*label_count].tunnel_id = tunnel_id;
                    ++*label_count;
                    if (*label_count >= label_max) {        /* done */
                        break;
                    }
                }

                if (*label_count >= label_max) {        /* done */
                    break;
                }
            } else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {  /* also PWE entry can be used for TUNNEL encapsulation */
                mpls_tunnel_info.push_profile =
                    encap_entry_info[indx].entry_val.pwe_info.push_profile;
                mpls_tunnel_info.tunnel_label =
                    encap_entry_info[indx].entry_val.pwe_info.label;

                rv = _bcm_mpls_tunnel_bcm_from_ppd(unit,
                                                   &label_array[*label_count],
                                                   &mpls_tunnel_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Update label id */
                label_array[*label_count].tunnel_id = tunnel_id;

                ++*label_count;
                if (*label_count >= label_max) {    /* done */
                    break;
                }
            } else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND) {  /* POP entry */
                rv = _bcm_petra_mpls_tunnel_switch_egress_label_from_pop_info(unit,
                      &label_array[*label_count],&(encap_entry_info[indx].entry_val.pop_info));
                BCMDNX_IF_ERR_EXIT(rv);       

                /* Update label id */
                label_array[*label_count].tunnel_id = tunnel_id;

                ++*label_count;
                if (*label_count >= label_max) {    /* done */
                    break;
                }
            } else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {  /* SWAP entry */
                label_array[*label_count].label = encap_entry_info[indx].entry_val.swap_info.swap_label;
                label_array[*label_count].flags |= (BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
                label_array[*label_count].action = BCM_MPLS_EGRESS_ACTION_SWAP;
                if (encap_entry_info[indx].entry_val.swap_info.remark_profile !=0) {
                    BCM_INT_QOS_MAP_REMARK_SET(label_array[*label_count].qos_map_id, encap_entry_info[indx].entry_val.swap_info.remark_profile);
                }
                            
                /* Update label id */
                label_array[*label_count].tunnel_id = tunnel_id;

                ++*label_count;
                if (*label_count >= label_max) {    /* done */
                    break;
                }
            } else {
                break;
            }

        }

        /* Update VSI (last entry) */
        BCM_L3_ITF_SET(label_array[*label_count-1].l3_intf_id,BCM_L3_ITF_TYPE_RIF,vsi);
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

/*
 * when mpls action is pop_direct or php, 
 * then the pop-type <to eth, ip, mpls) is set according to vpn type
 */
int _bcm_petra_mpls_tunnel_switch_vpn_to_pop_type(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPC_MPLS_COMMAND * mpls_info)
{
    
    BCMDNX_INIT_FUNC_DEFS;

      /* Retreive tpid_profile, model, has_cw, pop_next_header */
      mpls_info->command = SOC_TMC_MPLS_COMMAND_TYPE_POP;

      mpls_info->has_cw = FALSE;
      mpls_info->tpid_profile = 0;

      /* get next header indicated in flags */
      if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2){
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
          if (info->egress_label.flags & BCM_MPLS_EGRESS_LABEL_CONTROL_WORD) {
              mpls_info->has_cw = TRUE;
          }
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4){
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC; 
      }
      else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6){
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC; 
      }
      else/* if not present take from mpls */
      {
          mpls_info->pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
      }

      /* if not ethernet check if it unifrom or pipe */
      if (mpls_info->pop_next_header != SOC_TMC_PKT_FRWRD_TYPE_BRIDGE) {
          /* if uniform */
          if (info->
              flags & (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL)) {
              /* fix pop type */
              mpls_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
          } else {
              mpls_info->model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
          }
      }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * when mpls action is termination, 
 * then the next protocol, forwarding code <to eth, ip, mpls) is set according to vpn type or flags
 */
STATIC int _bcm_petra_mpls_tunnel_switch_vpn_to_next_prtcl_type(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPC_MPLS_TERM_INFO * term_info)
{

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_88660_A0
          if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1) \
              && !soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
              /* For Arad+ the default is IPv4. */
              term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
          } else 
#endif /* BCM_88660_A0 */
          {
              /* get next header indicated in flags */
              if (soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
                  if (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2) {
                      term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE; 
                  } else {
                      term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
                  }
              }
              else if (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2) {
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
              }
              else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4){
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC; 
              }
              else if(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6){
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC;
              }
              else /* If not present assume MPLS */
              {
                  term_info->forwarding_code = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
              }
          }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * when mpls action is termination, 
 * Get VPN and flags according to the next protocol, forwarding code <to eth, ip, mpls) 
 */
STATIC int _bcm_petra_mpls_tunnel_switch_next_prtcl_type_to_vpn(
    int unit,
    SOC_TMC_PKT_FRWRD_TYPE forwarding_code,
    SOC_PPC_RIF_INFO * rif_info,
    uint8 is_lookup_none,
    bcm_mpls_tunnel_switch_t * info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (rif_info != NULL)
    {
        _BCM_DPP_VPN_ID_SET(info->vpn, rif_info->vrf_id);
    }

    if (forwarding_code == SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC) 
    {
        info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
    }
    else if (forwarding_code == SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC)
    {
        info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
    }
    else if (!is_lookup_none && (forwarding_code == SOC_TMC_PKT_FRWRD_TYPE_BRIDGE))
    {
        info->flags |= BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPC_FRWRD_ILM_KEY * ilm_key
    )
{
    SOC_PPC_FRWRD_ILM_GLBL_INFO glbl_info;
    int rv, idx;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_ILM_KEY_clear(ilm_key);
    soc_sand_dev_id = (unit);

    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(soc_sand_dev_id, &glbl_info));

    /* In case key support l3-interface, flag must be set */    
    if (glbl_info.key_info.mask_inrif != (!(info->flags & BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag must be set only in case global key info includes In-RIF ")));
    }

    /* set key */
    /* label */
    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
        ilm_key->in_label = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->label);
    }
    else {
        ilm_key->in_label = info->label;
    }

    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) {
        /* Coupling */
        ilm_key->in_label_second = info->second_label;
    }

    /* port can be part of the key */
    if (!glbl_info.key_info.mask_port) {
       _bcm_dpp_gport_info_t gport_info;

        /* Retrive local PP ports */
        rv = _bcm_dpp_gport_to_phy_port(unit, info->port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);
   
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &ilm_key->in_local_port, (int*)&ilm_key->in_core)));
    }

    /* inrif can be part of the key */
    if (!glbl_info.key_info.mask_inrif) {
        ilm_key->inrif = info->ingress_if;
    }

    /* in exp can be part of the key */
    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_INT_PRI) {
        if ((info->label >= glbl_info.elsp_info.labels_range.start) && 
            (info->label <= glbl_info.elsp_info.labels_range.end)) {
            for(idx=0;idx<8; idx++) {
                if(glbl_info.elsp_info.exp_map_tbl[idx] == info->exp_map) {
                    break;
                }
            }
            if (idx < 8) {
                ilm_key->mapped_exp = info->exp_map;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case of ILM ELSP after EXP does not exist")));
            }
        }
    }


    if(SOC_DPP_CONFIG(unit)->arad->pp_op_mode.mpls_info.lookup_include_vrf){
       if(info->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL){
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("coupled mpls could not support when in vrf scoped ilm lookup mode ")));
       }
       if(info->flags & BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF){
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" In-Rif and VRF could not be used as lookup context at the same time ")));
       }
       ilm_key->vrf = info->vpn;
    }


exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_mpls_tunnel_switch_ilm_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_FRWRD_ILM_KEY ilm_key;
    SOC_PPC_FRWRD_DECISION_INFO ilm_val;
    SOC_PPC_FEC_ID fec_id=0;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    SOC_TMC_MULT_ID tmp_mc;
    int soc_sand_rv;

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    /* In tunnel termination, Control word is allowed only in EVPN/IML label termination.*/
    if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0))) {
        if (info->flags2 & BCM_MPLS_SWITCH2_CONTROL_WORD) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In tunnel termination, Control word is allowed only in EVPN/IML label termination ")));
        }
    }

    /* ilm key */
    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(unit, info, &ilm_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* ilm value */
    /* FEC+ Outlif or FEC + EEI of type OutLIF */
    if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if)
            ||(BCM_FORWARD_ENCAP_ID_IS_EEI(info->egress_if) && (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->egress_if) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER))) {
        /* egress interface is OutLIF */
        if (info->action != BCM_MPLS_SWITCH_ACTION_NOP){
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case egress_if is LIF, action must be BCM_MPLS_SWITCH_ACTION_NOP")));
        }
        if (BCM_GPORT_IS_FORWARD_PORT(info->port)) {
            fec_id = BCM_GPORT_FORWARD_PORT_GET(info->port);
        }
    } else {
        /* FEC + EEI of type MPLS */
        rv = _bcm_l3_intf_to_fec(unit, info->egress_if, &fec_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    SOC_PPC_FRWRD_DECISION_INFO_clear(&ilm_val);
    if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP) {

      /* make swap */
        SOC_PPD_FRWRD_DECISION_ILM_SWAP_SET(unit, &ilm_val,
                                        info->egress_label.label, fec_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_PHP
               || info->action == BCM_MPLS_SWITCH_ACTION_POP_DIRECT) {
        
        ilm_val.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;        
        ilm_val.additional_info.eei.val.mpls_command.command =
            SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE;

        /* assign pop type, according to flags and VPN type */
        rv = _bcm_petra_mpls_tunnel_switch_vpn_to_pop_type(unit, info,
                                                           &(ilm_val.
                                                             additional_info.
                                                             eei.val.
                                                             mpls_command));
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (info->action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) { /* PUSH ?! */
        ilm_val.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        ilm_val.dest_id = fec_id;
        ilm_val.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        ilm_val.additional_info.eei.val.mpls_command.command =
            SOC_PPC_MPLS_COMMAND_TYPE_PUSH;
        rv = _bcm_petra_mpls_tunnel_switch_egress_label_to_mpls_command(unit, &(info->egress_label),
                                                                        &(ilm_val.
                                                                       additional_info.
                                                                       eei.val.
                                                                       mpls_command));
        BCMDNX_IF_ERR_EXIT(rv);
        
        
    } else if (info->action == BCM_MPLS_SWITCH_ACTION_NOP) {
        /* EEI 0 */
        ilm_val.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        ilm_val.dest_id = fec_id;
        if (BCM_FORWARD_ENCAP_ID_IS_EEI(info->egress_if) && (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->egress_if) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER)) {
            ilm_val.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
            ilm_val.additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;
            ilm_val.additional_info.eei.val.outlif = BCM_FORWARD_ENCAP_ID_VAL_GET(info->egress_if);
        } else if (BCM_L3_ITF_TYPE_IS_LIF(info->egress_if)) {
                ilm_val.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
                /* Outlif */
                ilm_val.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
                ilm_val.additional_info.outlif.val = BCM_L3_ITF_VAL_GET(info->egress_if);
        }
    }
    
    /* set destination */
    /* of p2mp then destination is MC, otherwise destination is egress-object */
    if(info->flags & BCM_MPLS_SWITCH_P2MP) {
        ilm_val.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
        rv = _bcm_petra_multicast_group_to_id(info->mc_group,&tmp_mc);
        BCMDNX_IF_ERR_EXIT(rv);
        ilm_val.dest_id = tmp_mc;
    }
    else{
        ilm_val.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        ilm_val.dest_id = fec_id;       
    }

    soc_sand_rv =
        soc_ppd_frwrd_ilm_add(unit, &ilm_key, &ilm_val, &failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(failure_indication);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_label_stat_detach(
    int unit, 
    bcm_mpls_label_t label, 
    bcm_gport_t port)
{
    uint32   rv = SOC_SAND_OK;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE success = SOC_SAND_SUCCESS;
    char *propval;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    DBAL_QUAL_VALS_CLEAR(qual_vals);
    
    qual_vals[0].type= SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
    qual_vals[0].val.arr[0]= label;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lsr_stat_with_context", 0)) {
        qual_vals[1].val.arr[0]= port;
        propval = soc_property_get_str(unit, spn_MPLS_CONTEXT);
        if (propval && sal_strcmp(propval, "port") == 0) {
            /* in_port defined */
            uint32 in_local_port,in_core;
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &in_local_port, (int*)&in_core)));
            qual_vals[1].val.arr[0]= in_local_port | (in_core << 8);
            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;
        } else if(propval && sal_strcmp(propval, "interface") == 0){
            /* inRIF defined */
            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        } else if(propval && sal_strcmp(propval, "vrf") == 0){
            /* VRF defined */
            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_VRF;
        }
    }    
    rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_CNT_LEM,
    qual_vals, &success);    
    BCM_SAND_IF_ERR_EXIT(rv);
    
    SOC_SAND_IF_FAIL_RETURN(success);
    
 exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_mpls_label_stat_attach(
    int unit, 
    bcm_mpls_label_t label, 
    bcm_gport_t port, 
    uint32 stat_counter_id)
{
    uint32
      rv = SOC_SAND_OK;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    ARAD_PP_LEM_ACCESS_PAYLOAD lem_payload;
    SOC_SAND_SUCCESS_FAILURE success = SOC_SAND_SUCCESS;
    char *propval;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    
    DBAL_QUAL_VALS_CLEAR(qual_vals);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload); 

    qual_vals[0].type= SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD;
    qual_vals[0].val.arr[0]= label;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lsr_stat_with_context", 0)) {
        qual_vals[1].val.arr[0]= port;
        propval = soc_property_get_str(unit, spn_MPLS_CONTEXT);
        if (propval && sal_strcmp(propval, "port") == 0) {
            /* in_port defined */
            uint32 in_local_port,in_core;
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &in_local_port, (int*)&in_core)));
            qual_vals[1].val.arr[0]= in_local_port | (in_core << 8);
            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;
        } else if(propval && sal_strcmp(propval, "interface") == 0){
            /* inRIF defined */
            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        } else if(propval && sal_strcmp(propval, "vrf") == 0){
            /* VRF defined */
            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_VRF;
        }
    }    
    /*use dest to save counter id*/
    lem_payload.dest = (stat_counter_id&0xffff);/*low 16bit is ingress count ID, high 16bits is egress count ID*/
    lem_payload.asd = (stat_counter_id&0xffff0000)>>16;
    lem_payload.age = 0;
    lem_payload.is_dynamic = FALSE;
    lem_payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
    
    /*arad_pp_frwrd_ilm_lem_payload_build,soc_jer_pp_mpls_lable_stat*/
    rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_LSR_CNT_LEM,
    qual_vals, 0/*priority*/, (void *)(&lem_payload), &success);    
    
   BCM_SAND_IF_ERR_EXIT(rv);
    
   SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;

}
STATIC int _bcm_petra_mpls_tunnel_switch_ilm_entry_to_bcm(
    int unit,
    SOC_PPC_FRWRD_ILM_KEY * ilm_key,
    SOC_PPC_FRWRD_DECISION_INFO * ilm_val,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_FRWRD_ILM_GLBL_INFO glbl_info;
    unsigned int soc_sand_dev_id;
    int rv;
    uint8 has_cw;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(soc_sand_dev_id, &glbl_info));

    /* set key */
    /* label */
    info->label = ilm_key->in_label;

    if (!glbl_info.key_info.mask_port) {
        bcm_port_t local_port;

        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_pp_to_local_port_get, (unit, ilm_key->in_core, ilm_key->in_local_port, &local_port));
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_GPORT_LOCAL_SET(info->port,local_port);
    }
    if (!glbl_info.key_info.mask_inrif) {
        info->ingress_if = ilm_key->inrif;        
    }

    /* has to be EEI of type MPLS */
    if (ilm_val->additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS) {
          /* swapped or pushed label */
          info->egress_label.label =
              ilm_val->additional_info.eei.val.mpls_command.label;
          if (ilm_val->additional_info.eei.val.mpls_command.command ==
              SOC_PPC_MPLS_COMMAND_TYPE_SWAP) {
              /* swap action */
              info->action = BCM_MPLS_SWITCH_ACTION_SWAP;
          } else if (ilm_val->additional_info.eei.val.mpls_command.command ==
              SOC_PPC_MPLS_COMMAND_TYPE_PUSH) {
              info->action = BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH;
          } else {
              info->action = BCM_MPLS_SWITCH_ACTION_PHP;
              info->egress_label.label = BCM_MPLS_LABEL_INVALID;
              rv = _bcm_petra_mpls_tunnel_switch_next_prtcl_type_to_vpn(unit,
                                                                        ilm_val->
                                                                        additional_info.eei.val.
                                                                        mpls_command.
                                                                        pop_next_header,
                                                                        NULL, 0, info);
              BCMDNX_IF_ERR_EXIT(rv);
          }

          if (info->action == BCM_MPLS_SWITCH_ACTION_PHP) {
              if (ilm_val->additional_info.eei.val.mpls_command.pop_next_header != SOC_TMC_PKT_FRWRD_TYPE_BRIDGE){
                  /* Uniform: copy TTL and EXP */
                  if (ilm_val->additional_info.eei.val.mpls_command.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM) {
                      info->flags |= (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
                  } 
              }
               /* Always dec TTL */
              info->flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
              has_cw = ilm_val->additional_info.eei.val.mpls_command.has_cw;
         } else if (info->action == BCM_MPLS_SWITCH_ACTION_SWAP){
              /* 
               * In mpls ingress swap, the QoS model can be uniform only
               */
              info->flags |= (BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
              has_cw = ilm_val->additional_info.eei.val.mpls_command.has_cw;
         } else { /* Action is Egress PUSH */
               rv = _bcm_mpls_tunnel_push_profile_info_get(unit, 
                                                           ilm_val->
                                                           additional_info.eei.val.
                                                           mpls_command.
                                                           push_profile,
                                                           &has_cw,
                                                           &(info->egress_label));
               BCMDNX_IF_ERR_EXIT(rv);
         }

         if (has_cw) {
             info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
         }
    } else if (ilm_val->additional_info.eei.type == SOC_PPC_EEI_TYPE_EMPTY
               || ilm_val->additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
          info->action = BCM_MPLS_SWITCH_ACTION_NOP; 
          info->flags |= BCM_MPLS_SWITCH_TTL_DECREMENT; /* always done */
          info->egress_label.label = BCM_MPLS_LABEL_INVALID;
    } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error, in case of FEC EEI type must be MPLS or Empty")));
    }

    if (ilm_val->type == SOC_PPC_FRWRD_DECISION_TYPE_FEC) { 
        if (ilm_val->additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_RAW) {
            BCM_L3_ITF_SET(info->egress_if, BCM_L3_ITF_TYPE_LIF, ilm_val->additional_info.outlif.val);
            BCM_GPORT_FORWARD_PORT_SET(info->port, ilm_val->dest_id);
        }
        else if (ilm_val->additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->egress_if, BCM_FORWARD_ENCAP_ID_TYPE_EEI, BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER, ilm_val->additional_info.eei.val.outlif);
            BCM_GPORT_FORWARD_PORT_SET(info->port, ilm_val->dest_id);
        }
        else {                        
            _bcm_l3_fec_to_intf(unit, ilm_val->dest_id, &info->egress_if);        
        }
    } else if (ilm_val->type == SOC_PPC_FRWRD_DECISION_TYPE_MC) {
        info->flags |= BCM_MPLS_SWITCH_P2MP;
        rv = _bcm_petra_multicast_group_from_id(&info->mc_group,_BCM_MULTICAST_TYPE_EGRESS_OBJECT,ilm_val->dest_id);
        BCMDNX_IF_ERR_EXIT(rv);               
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not supported type: Expect always for ILM to be of type FEC or Multicast")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_tunnel_switch_ilm_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_FRWRD_ILM_KEY ilm_key;
    SOC_PPC_FRWRD_DECISION_INFO ilm_val;
    uint8 found;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(unit, info, &ilm_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* ilm value */
    soc_sand_rv = soc_ppd_frwrd_ilm_get(soc_sand_dev_id, &ilm_key, &ilm_val, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    rv = _bcm_petra_mpls_tunnel_switch_ilm_entry_to_bcm(unit, &ilm_key,
                                                        &ilm_val, info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_mpls_tunnel_switch_ilm_remove(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_FRWRD_ILM_KEY ilm_key;
    SOC_PPC_FRWRD_DECISION_INFO ilm_val;
    uint32 push_profile;
    int soc_sand_rv, rv;
    unsigned int soc_sand_dev_id;

    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_ilm_key(unit, info, &ilm_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* ilm value*/
    soc_sand_rv = soc_ppd_frwrd_ilm_get(soc_sand_dev_id, &ilm_key, &ilm_val, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    push_profile = ilm_val.additional_info.eei.val.mpls_command.push_profile;
    if(push_profile) {
        int is_allocated = 0;
        int is_last;

        rv = _bcm_dpp_am_template_mpls_push_profile_is_profile_allocated(unit, push_profile, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_allocated) {
            rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, push_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (!found) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    soc_sand_rv = soc_ppd_frwrd_ilm_remove(soc_sand_dev_id, &ilm_key);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_mpls_term_to_pwe_lif_types(
    int unit,
    uint32 lif_types,
    uint32 *pwe_lif_types)
{

    BCMDNX_INIT_FUNC_DEFS;    

    *pwe_lif_types = 0;

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_DEFAULT;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_2) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_2;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_3) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_3;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_2) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_2;
    }

    if (lif_types == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_3) {
        *pwe_lif_types = _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_3;
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;      
}

int _bcm_petra_mpls_term_resolve_lif_type_by_index(
    int unit,
    int port,
    bcm_mpls_label_t label,
    uint32 *lif_types)
{
    int is_frr_port;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;    

    rv = bcm_petra_port_control_get(unit, port, bcmPortControlMplsFRREnable, &is_frr_port);
    BCMDNX_IF_ERR_EXIT(rv);

    switch (BCM_MPLS_INDEXED_LABEL_INDEX_GET(label)) {
    case 1:     
      if (SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) {
          *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_3;
      } else if (SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2) {
          *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_2;
      } else {
          *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1;
      }  
      break;
    case 2:
      *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_2;
      break;
    case 3:
      *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_3;
      break;
    case 4:
    *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_FRR;
      break;

    default:
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported index value: %d"), BCM_MPLS_INDEXED_LABEL_INDEX_GET(label)));
    }
    
exit:
    BCMDNX_FUNC_RETURN;      
}

int _bcm_petra_mpls_term_key_to_lif_alloc_type(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    uint32 *lif_types)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;    

    if (info->flags & BCM_MPLS_SWITCH_FRR) { /* FRR */
        *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM_FRR;
    } else if ((!(info->flags & BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL)) 
                && SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable
                && !(info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE)) { /* Indexed not coupling, not frr entries */
        rv = _bcm_petra_mpls_term_resolve_lif_type_by_index(unit, info->port, info->label, lif_types);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        *lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM; /* Default */
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC
int _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    SOC_PPC_MPLS_LABEL_RIF_KEY *rif_key)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_MPLS_TERM_LKUP_INFO glbl_info;
    uint8 is_key_label_only;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPC_MPLS_LABEL_RIF_KEY_clear(rif_key);

    soc_sand_rv = soc_ppd_mpls_term_lkup_info_get(soc_sand_dev_id, &glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* In case key support l3-interface, flag must be set  */    
    if ((glbl_info.key_type == SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF) == (!(info->flags & BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag must be set only in case global key info includes In-RIF ")));
    }

    is_key_label_only = SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1);
    if (!is_key_label_only) {
      if (((info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) && !(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))
          || (!(info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) && (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flags 0x%x in case one of next procotol IP is set both of them must be set"), info->flags));
      }
    }
    /* set key */
    /* label */
    if (!(info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE))
    {
        if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
            rif_key->label_id = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->label);
            
            rif_key->label_index = BCM_MPLS_INDEXED_LABEL_INDEX_GET(info->label);
        } else {
            if (!_BCM_DPP_MPLS_LABEL_IN_RANGE(info->label)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS label is out of range. The range is 0 - (1<<20)-1")));
            }
            rif_key->label_id = info->label;
        }
    }

    /* RIF can be part of the key - NOT SUPPORTED YET */
    if (glbl_info.key_type == SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF) {
        rif_key->vsid = info->ingress_if;
    }

    /*Vlan Domain can be part of Key*/
     if (BCM_GPORT_IS_LOCAL(info->port)) {
        SOC_PPC_PORT   soc_ppc_port;
        SOC_PPC_PORT_INFO  port_info;
        int       core;

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, BCM_GPORT_LOCAL_GET(info->port), &soc_ppc_port, &core)));       
        soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppc_port, &port_info);

        rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_TERM_BASED_PORT;
        rif_key->vlan_domain = port_info.vlan_domain;
    }

    if (info->flags & (BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL | BCM_MPLS_SWITCH_FRR)) {
        if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
            rif_key->label_id_second = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->second_label);
        } else {
            if (!_BCM_DPP_MPLS_LABEL_IN_RANGE(info->second_label)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS second label is out of range. The range is 0 - (1<<20)-1")));
            }
            rif_key->label_id_second = info->second_label;
        }
        rif_key->flags |= (info->flags & BCM_MPLS_SWITCH_FRR) ? SOC_PPC_RIF_MPLS_LABEL_FRR : 0;
    }
    
    if (info->flags & BCM_MPLS_SWITCH_ENTROPY_ENABLE) { /* ELI */
        rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_ELI; /* Add ELI in case of Entropy for MPLS tunnels */
    }     

    if (!is_key_label_only) {
        if ((info->flags & BCM_MPLS_SWITCH_ENTROPY_ENABLE && 
            !((info->flags & (BCM_MPLS_SWITCH_NEXT_HEADER_L2|BCM_MPLS_SWITCH_NEXT_HEADER_IPV4|BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))))) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ELI Entorpy is not supported when entropy is not BOS"))); 
        }
    }

    /* In Arad the BOS expectation is determined by the next type. */
    if (info->flags & (BCM_MPLS_SWITCH_NEXT_HEADER_L2|BCM_MPLS_SWITCH_NEXT_HEADER_IPV4|BCM_MPLS_SWITCH_NEXT_HEADER_IPV6)) {
        rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS;
    }

    /* 
     *  Not allowed:
     *  1. SOC_PPC_MPLS_LABEL_INDEX_SECOND in case of L1_L2 namespace (use L1 only)
     *  2. SOC_PPC_MPLS_LABEL_INDEX_THIRD in case of L1_L3 namespace (use L1 only)
     */ 
    if (rif_key->label_index == SOC_PPC_MPLS_LABEL_INDEX_SECOND &&
        SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Provided label index second where there is no L2 namespace. Use L1 for both L1,L2"))); 
    }
    if (rif_key->label_index == SOC_PPC_MPLS_LABEL_INDEX_THIRD &&
        SOC_DPP_CONFIG(unit)->pp.mpls_namespaces[0] == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Provided label index second where there is no L3 namespace. Use L1 for both L1,L3"))); 
    }
       
#ifdef BCM_88660_A0
    if (is_key_label_only) {
        /* In Arad+ the BOS expectation is determined by the flags for the reserved labels. */
        /* For unreserved labels there is no expect BOS or no BOS (since they don't check the */
        /* BOS to begin with). */
        rif_key->flags &= ~SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS;
        if (BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->label) <= _BCM_PETRA_MPLS_MAX_RESERVED_MPLS_LABEL) {
            rif_key->flags |= (info->flags & BCM_MPLS_SWITCH_EXPECT_BOS) ? SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS : 0;
        } else { /* non reserved labels */
            if (info->flags & BCM_MPLS_SWITCH_EXPECT_BOS) {
                /* under soc property , and flag expect bos to be 1 */
                rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS ;
            }
        }
    }
#endif /* BCM_88660_A0 */

    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE) {
        rif_key->flags |= SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE;
    }

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        int is_iml_bos_indication = 0;
        is_iml_bos_indication = info->tunnel_id & _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION;
        rif_key->flags |= (is_iml_bos_indication) ? SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS : 0;
        rif_key->flags |= ((info->flags & BCM_MPLS_SWITCH_EVPN_IML) | is_iml_bos_indication) ? SOC_PPC_RIF_MPLS_LABEL_EVPN_IML : 0;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_MPLS_LABEL_RIF_KEY rif_key;
    SOC_PPC_MPLS_TERM_INFO term_info;
    SOC_PPC_RIF_INFO rif_info;
    SOC_PPC_MPLS_TERM_PROFILE_INFO term_profile_info;
    uint8 found, failover_status = 0;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ID lif_indx;
    int global_lif_id, local_in_lif_id = 0;
    int rv = BCM_E_NONE;
    int reset_evpn_bos_indication = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&rif_key);
    SOC_PPC_MPLS_TERM_INFO_clear(&term_info);
    SOC_PPC_RIF_INFO_clear(&rif_info);

    /* If there is an EVPN bos indication on the tunnel id, remember to reset it when re-assigning tunnel id with global lif */
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        reset_evpn_bos_indication = (info->tunnel_id & _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION) ? 1 : 0;
    }

    /* Get MPLS tunnel when lookup is none means skip SEM configuration. 
       LIF-ID must be INPUT and not implicit found from SEM lookup */
    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE &&
        info->tunnel_id == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When lookup key is BCM_MPLS_SWITCH_LOOKUP_NONE tunnel_id must be valid.")));
    }

    /* set key */
    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(unit, info, &rif_key);
    BCMDNX_IF_ERR_EXIT(rv);

    if (rif_key.flags & SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE) {
        /* No lookup to retreive LIF-ID, lif_indx is provided by the API in this case */
        if (SOC_DPP_CONFIG(unit)->pp.explicit_null_support && (BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id) == SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id)) {
            /* NULL label - no SW DB */
            local_in_lif_id = global_lif_id = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
            found = TRUE;
        } else {
            rv = _bcm_dpp_gport_to_hw_resources(unit, info->tunnel_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS,
                                                &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(rv);

            local_in_lif_id = gport_hw_resources.local_in_lif;
            global_lif_id = gport_hw_resources.global_in_lif;

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, local_in_lif_id, -1, &lif_usage, NULL));
            found = (lif_usage == _bcmDppLifTypeMplsTunnel) ? TRUE : FALSE;
        }
    } else {

        found = FALSE;
        /* In case mpls_label_index_enable is set and we have a non-indexed forwarding entry, no look-up is needed for termination */
        if (!(SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable && (rif_key.label_index == 0))) {
            soc_sand_rv =
               soc_ppd_rif_mpls_label_map_get(soc_sand_dev_id, &rif_key, &lif_indx,
                                              &term_info, &rif_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        

        if (!found) {
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
        }

        if (SOC_IS_JERICHO(unit)) {
            uint8 is_wide_entry;

            local_in_lif_id = lif_indx;

            /*map local to global lif*/
            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_in_lif_id, &global_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Add the wide entry flag. */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_is_wide_entry, (unit, local_in_lif_id, TRUE,&is_wide_entry, NULL));
            BCMDNX_IF_ERR_EXIT(rv);

            info->flags |= is_wide_entry ? BCM_MPLS_SWITCH_WIDE : 0;

        } else {
            global_lif_id = local_in_lif_id = lif_indx;
        }
    }
    info->action = BCM_MPLS_SWITCH_ACTION_POP;
    /* update from LIF sw state */
    rv = _bcm_dpp_in_lif_mpls_term_match_get(unit, info, local_in_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_lif_id);
    if (reset_evpn_bos_indication) {
        info->tunnel_id |= _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION;
    }

    /* Retrieve some flags from the the termination profile info. */
    soc_sand_rv = soc_ppd_mpls_term_profile_info_get(soc_sand_dev_id, term_info.term_profile, &term_profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (term_profile_info.flags & SOC_PPC_MPLS_TERM_SKIP_ETH) {
        info->flags |= BCM_MPLS_SWITCH_SKIP_ETHERNET;
    }

    if (term_profile_info.flags & SOC_PPC_MPLS_TERM_HAS_CW) {
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
    }
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) {
        info->flags |= (rif_key.flags & SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS) ? BCM_MPLS_SWITCH_EXPECT_BOS : 0;
    }
#endif /* BCM_88660_A0 */

    /* map term info and rif info to bcm-switch info */

    /* termination value */
    if (term_info.cos_profile != 0) {        
        BCM_INT_QOS_MAP_LIF_COS_SET(info->qos_map_id, term_info.cos_profile);        
    }
    info->tunnel_if = (term_info.rif == SOC_PPC_RIF_NULL) ? BCM_IF_INVALID:term_info.rif;

    if (term_info.processing_type == SOC_PPC_MPLS_TERM_MODEL_UNIFORM) {
        info->flags |= (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL);
    }

    if (term_info.rif != SOC_PPC_RIF_NULL) {
        rv = _bcm_petra_mpls_tunnel_switch_next_prtcl_type_to_vpn(unit, term_info.forwarding_code, &rif_info, (rif_key.flags & SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE), info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_EVPN_ENABLE, 0) && term_info.vsi != 0)) {
        info->vpn = term_info.vsi;
    }

    if (term_info.protection_pointer > 0) {
        /* Fetch ingress protection information*/
        DPP_FAILOVER_TYPE_SET(info->failover_id, term_info.protection_pointer, DPP_FAILOVER_TYPE_INGRESS);
    }

    if (_BCM_PPD_IS_VALID_FAILOVER_ID(info->failover_id)) {
        /* Check the failover status */
        rv = _bcm_dpp_failover_state_hw_get(unit,term_info.protection_pointer,DPP_FAILOVER_TYPE_INGRESS,&failover_status);
        BCMDNX_IF_ERR_EXIT(rv);
        info->failover_tunnel_id = (failover_status != term_info.protection_pass_value) ? 1 : 0;
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/* when action is pop, then another forwarding lookup will be performed
   so do the pop in the termination DB
   */
int _bcm_petra_mpls_tunnel_switch_term_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_MPLS_LABEL_RIF_KEY rif_key;
    SOC_PPC_MPLS_TERM_INFO term_info;
    SOC_PPC_RIF_INFO rif_info;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int lif_indx = 0;
    int global_lif_id, local_in_lif_id = 0;
    int rv = BCM_E_NONE;
    uint32 inner_flags = 0;
    uint32 term_lif_type;
    uint32 alloc_flags = 0;
    int global_lif_flags = 0;
    int update = 0;
    int err_code = 0;
    uint8 counting_profile_is_allocated;
    bcm_mpls_tunnel_switch_t info_get;
    uint8 lif_is_alloc = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&rif_key);
    SOC_PPC_MPLS_TERM_INFO_clear(&term_info);
    SOC_PPC_RIF_INFO_clear(&rif_info);
    SOC_PPC_LIF_ENTRY_INFO_clear(&lif_entry_info);

    /* If REPLACE flag is set, WITH_ID must also be set */
    if (info->flags & BCM_MPLS_SWITCH_REPLACE && !(info->flags & BCM_MPLS_SWITCH_WITH_ID)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If REPLACE flag is set, WITH_ID flag must also be set.")));
    }

    if (BCM_GPORT_IS_LOCAL(info->port) && !soc_property_get(unit, spn_PWE_TERMINATION_PORT_MODE_ENABLE, 0) ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Port based mpls termination should be enabled!!")));
    }


    /* If REPLACE flag is set, check that entry exists */
    if (info->flags & BCM_MPLS_SWITCH_REPLACE){
        bcm_mpls_tunnel_switch_t_init(&info_get);
        info_get.label = info->label;

        if (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE) {    
            info_get.flags = BCM_MPLS_SWITCH_LOOKUP_NONE;
            info_get.tunnel_id = info->tunnel_id;
        }

        rv = bcm_petra_mpls_tunnel_switch_get(unit, &info_get);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* In case label ranges are defined, we assert the equality og the in label to the  lower limit of the range*/
    if (SOC_IS_JERICHO(unit)) {
        uint32 entry_index;
        bcm_mpls_label_t indexed_label_value;
        SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;
        SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
        SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
        SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
        
        /*Extract the 20 LSBs (in order not to verify bits above 19'b */
        indexed_label_value = BCM_MPLS_INDEXED_LABEL_VALUE_GET(info->label); 

        /* verify whether the in label belongs to a certain label range*/
        rv = _bcm_dpp_mpls_range_action_verify(unit,indexed_label_value, indexed_label_value, &entry_index, &err_code);
        BCMDNX_IF_ERR_EXIT(rv);
        /* In this case, there is a label range containing the in label*/
        if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT || err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_get,(unit, entry_index, &range_action_info)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_get,(unit, entry_index, &range_profile_info)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            /* In UHP application (indicated by range_profile_info.mpls_label_range_use_base==1)
               we assert the equality of the label to the lower limit of the range */
            if (indexed_label_value != range_action_info.label_low && range_profile_info.mpls_label_range_use_base) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case a label range is defined, the in label should be equal to the lower limit of the range")));
            }
        } else {
            /* IML labels in EVPN application can be processed only if they fit in a pre-configured label range */
            if ((soc_property_get(unit, spn_EVPN_ENABLE, 0)) && (info->flags & BCM_MPLS_SWITCH_EVPN_IML)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IML labels in EVPN must be included in a range created with bcm_mpls_range_action_add(), using BCM_MPLS_SWITCH_EVPN_IML.")));
            }
        }
    }

    if (info->flags & BCM_MPLS_SWITCH_ENTROPY_ENABLE) {
        if (SOC_IS_JERICHO(unit)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For Jericho, ENTROPY term flag is no need! ")));
        } else if (!SOC_DPP_CONFIG(unit)->pp.mpls_eli_enable) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case ENTROPY flag is set, ELI soc property mpls_entropy_label_indicator_enable must be enabled ")));
        }
    }

    if (SOC_IS_JERICHO(unit) && (info->flags & BCM_MPLS_SWITCH_EVPN_IML) && !soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case EVPN_IML flag is set, soc property EVPN_ENABLE  must be enabled ")));
    }

    /* In tunnel termination, Control word is allowed only in EVPN/IML label termination.*/
    if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0))) {
        if (info->flags2 & BCM_MPLS_SWITCH2_CONTROL_WORD) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In tunnel termination, Control word is allowed only in EVPN/IML label termination ")));
        }
    }

    if (_BCM_PPD_IS_VALID_FAILOVER_ID(info->failover_id)) {
        rv = _bcm_dpp_failover_is_valid_id(unit,(int32)info->failover_id,DPP_FAILOVER_TYPE_INGRESS);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only ingress protection is valid for a terminated mpls label")));
        }

        /* Configure ingress protection for this tunnel */
        _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(term_info.protection_pointer, info->failover_id);
        term_info.protection_pass_value = _BCM_PPD_GPORT_IS_INGRESS_WORKING_TUNNEL(info) ? 0 : 1;
    } 

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(unit,info,&rif_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* termination value */
    term_info.cos_profile = BCM_QOS_MAP_PROFILE_GET(info->qos_map_id);

    /* set next protocol according to info */
    rv = _bcm_petra_mpls_tunnel_switch_vpn_to_next_prtcl_type(unit,info,&term_info);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (info->flags & (BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL)) {
        term_info.processing_type = SOC_PPC_MPLS_TERM_MODEL_UNIFORM;
    } else {
        term_info.processing_type = SOC_PPC_MPLS_TERM_MODEL_PIPE;
    }

    term_info.rif = (info->tunnel_if == BCM_IF_INVALID) ? SOC_PPC_RIF_NULL:info->tunnel_if;


    /* set RIF info */
    if (term_info.rif != SOC_PPC_RIF_NULL) {
        soc_sand_rv = soc_ppd_rif_info_get(soc_sand_dev_id, term_info.rif, &rif_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rif_info.vrf_id = _BCM_DPP_VPN_ID_GET(info->vpn);
        rif_info.cos_profile = term_info.cos_profile;
    } else if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_EVPN_ENABLE, 0) && info->vpn != 0)) {
        term_info.vsi = info->vpn;
    }



    /* following attributes is not updated */
    /*   rif_info.uc_rpf_enable = 0;
       rif_info.ttl_scope_index = 0; */    
    /* try to add in range */
    rv = _bcm_petra_mpls_term_key_to_lif_alloc_type(unit, info, &term_lif_type);
    BCMDNX_IF_ERR_EXIT(rv);

    update = (info->flags & BCM_MPLS_SWITCH_REPLACE) ? 1 : 0;

    /* check update/with-id allocations*/
    if (info->flags & BCM_MPLS_SWITCH_WITH_ID) {
        lif_indx = _SHR_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        alloc_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;

        /* Special case for Explicit NULL: Explicit NULL 
            termination type is concluded according to LIF-ID. LIF-ID is determined by SOC property */
        if (SOC_DPP_CONFIG(unit)->pp.explicit_null_support && (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE) &&
            lif_indx == SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id) {
            term_lif_type = _BCM_DPP_AM_L3_LIF_MPLS_TERM_EXPLICIT_NULL;
        }

        /* Special case for mLDP: termination type is concluded according to LIF-ID. LIF-ID is determined by SOC property */
        if (SOC_DPP_CONFIG(unit)->pp.mldp_support && (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE) &&
            (lif_indx == SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[0] || lif_indx == SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[1])) {
            term_lif_type = _BCM_DPP_AM_L3_LIF_MPLS_TERM_DUMMY;
        }
    }

    /* cannot update without given id */
    if (update && !(alloc_flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated flag is set without providing proper ID")));
    }

    /* if update then don't overwrite this consts */
    if (!update) {
        term_info.learn_enable = 0;
    }

    /* allocate LIf for mpls tunnel termination */
    if (update) { /* update check LIF already allocated */
        if (SOC_IS_JERICHO(unit)) {
            global_lif_id = lif_indx;
            global_lif_flags = BCM_DPP_AM_FLAG_ALLOC_INGRESS;

            rv = bcm_dpp_am_global_lif_is_alloced(unit, global_lif_flags, global_lif_id);

            if (rv != BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated mpls tunnel does not exist")));
            }

            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_lif_id, &local_in_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);

            if (DPP_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WIDE)) {
                uint64 additional_data;
                rv = arad_pp_lif_additional_data_get(unit, local_in_lif_id, TRUE, &additional_data);
                SOCDNX_SAND_IF_ERR_EXIT(rv);
                term_info.additional_data = additional_data;
            }
        } else {
            rv = bcm_dpp_am_l3_lif_is_allocated(unit, term_lif_type, 0, lif_indx);
            if (rv != BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated mpls tunnel does not exist")));
            }

            local_in_lif_id =  global_lif_id = lif_indx;
        }
    } else { /* allocated LIF [with-id]]*/
        if (SOC_IS_JERICHO(unit)) {

            bcm_dpp_am_local_inlif_info_t local_inlif_info;

            /* Save the id, in case it's WITH_ID */
            global_lif_id = lif_indx;

            sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));

            /* Set the local lif info */
            rv = bcm_dpp_am_l3_lif_type_to_app_type(term_lif_type, &local_inlif_info.app_type);
            BCMDNX_IF_ERR_EXIT(rv);
            local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WIDE) ?
               BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;

            /*Check if counter profile was allocated or if this is null label it should be reserved */
            if ((SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[0] == global_lif_id) ||
                (SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[1] == global_lif_id)) {
                local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_RESERVED;
            } else {
                rv = bcm_dpp_counter_lif_range_is_allocated(unit, &counting_profile_is_allocated);
                BCMDNX_IF_ERR_EXIT(rv);
                if (counting_profile_is_allocated) {
                    if (info->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE && info->inlif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("InLIF-counting-profile %d is out of range."), info->inlif_counting_profile));
                    }
                    BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, info->inlif_counting_profile, &local_inlif_info.counting_profile_id));                                    
                } else {
                    local_inlif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
                }
            }

            /* Allocate the global and local lif */
            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, &global_lif_id, &local_inlif_info, NULL);
            BCM_IF_ERROR_RETURN(rv);
            lif_is_alloc = 1;

            /* Retrieve the allocated local lif */
            local_in_lif_id = local_inlif_info.base_lif_id;

            if (SOC_DPP_CONFIG(unit)->pp.mldp_support) {
                if (lif_indx == SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[0]) {
                    JER_MPLS_TERM.local_mldp_dummy_lif_id.set(unit, 0, local_in_lif_id);
                } else if (lif_indx == SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[1]) {
                    JER_MPLS_TERM.local_mldp_dummy_lif_id.set(unit, 1, local_in_lif_id);
                }

            }
        } else {
            rv = bcm_dpp_am_l3_lif_alloc(unit, term_lif_type, alloc_flags, &lif_indx);
            BCMDNX_IF_ERR_EXIT(rv);
            lif_is_alloc = 1;
            local_in_lif_id =  global_lif_id = lif_indx;
        }
    }

    /* LIF termination handling */

    rv = _bcm_mpls_tunnel_lif_term_tunnel_switch_to_flags(unit, info, &inner_flags);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Special case for mLDP: termination type is concluded according to LIF-ID. LIF-ID is determined by SOC property
       MPLS Port Termination: Lif will have 2 label termination*/
    if ((SOC_DPP_CONFIG(unit)->pp.mldp_support && (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE) &&
        (lif_indx == SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[1])) || (lif_indx == SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id && SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id)) {
        inner_flags |= _BCM_DPP_MPLS_LIF_TERM_GAL;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                                                  local_in_lif_id,
                                                                  inner_flags,
                                                                  FALSE,
                                                                  ((rif_key.flags & SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS) > 0),
                                                                  &(term_info.term_profile)));
    term_info.global_lif = global_lif_id;
    term_info.is_extended = DPP_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WIDE);

    /* Get OAM Lif setting */
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, local_in_lif_id, &lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
        term_info.oam_valid = lif_entry_info.value.mpls_term_info.oam_valid;
    }

    soc_sand_rv =
       soc_ppd_rif_mpls_label_map_add(soc_sand_dev_id, &rif_key,  local_in_lif_id,
                                      &term_info, &rif_info,
                                      &failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_EXIT(failure_indication);

    /* tunnel id */
    /* tunnel ID always is LIF-id,
     * also for 1+1: protection, MC-destination is seperate handled object. 
     */
    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_lif_id);

    /* indicate bos is expected in gport in case these flags are present */
    if (SOC_IS_JERICHO(unit) && (info->flags & BCM_MPLS_SWITCH_EVPN_IML) &&\
           (info->flags & BCM_MPLS_SWITCH_EXPECT_BOS)) {
        info->tunnel_id |= _BCM_DPP_MPLS_LIF_TERM_EVPN_IML_BOS_INDICATION;
    }

    rv = _bcm_dpp_in_lif_mpls_term_match_add(unit, info, local_in_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if((_rv != BCM_E_NONE) && lif_is_alloc) {
        if (SOC_IS_JERICHO(unit)) {
            _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, local_in_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
        } else {
            bcm_dpp_am_l3_lif_dealloc(unit, local_in_lif_id);
        }
        _bcm_mpls_tunnel_lif_term_profile_exchange(unit, local_in_lif_id, 0x0, FALSE, /* Default */ 0, NULL);
        _bcm_dpp_local_lif_sw_resources_delete(unit, local_in_lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
    }
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_term_remove(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SOC_PPC_MPLS_LABEL_RIF_KEY rif_key;
    SOC_PPC_RIF_INFO rif_info;
    SOC_PPC_MPLS_TERM_INFO term_info;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ID lif_indx = 0;
    int global_lif_id,  local_in_lif_id = 0 ;
    int rv = BCM_E_NONE;
    uint8 found;
    _bcm_lif_type_e lif_usage;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint64 additinal_data;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&rif_key);

    /* Remove MPLS tunnel when lookup is none means skip SEM configuration. 
       Once SEM configuration is removed, 
       LIF-ID must be INPUT and not implicit found from SEM lookup */
    if (info->flags & BCM_MPLS_SWITCH_LOOKUP_NONE &&
        info->tunnel_id == 0) {     
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When lookup key is BCM_MPLS_SWITCH_LOOKUP_NONE tunnel_id must be valid.")));
    }

    rv = _bcm_petra_mpls_tunnel_switch_bcm_entry_to_term_key(unit, info, &rif_key);
    BCMDNX_IF_ERR_EXIT(rv);      
    
    if (rif_key.flags & SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE) {
        /* No lookup to retreive LIF-ID, lif_indx is provided by the API in this case */
        rv = _bcm_dpp_gport_to_hw_resources(unit, info->tunnel_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS,
                                            &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        local_in_lif_id = gport_hw_resources.local_in_lif;
        global_lif_id = gport_hw_resources.global_in_lif;

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, local_in_lif_id, -1, &lif_usage, NULL));
        found = (lif_usage == _bcmDppLifTypeMplsTunnel) ? TRUE : FALSE;
    } else {

        found = FALSE;
        /* In case mpls_label_index_enable is set and we have a non-indexed forwarding entry, no look-up is needed for termination */
        if (!(SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable && (rif_key.label_index == 0))) {
            soc_sand_rv =
               soc_ppd_rif_mpls_label_map_get(soc_sand_dev_id, &rif_key, &lif_indx,
                                              &term_info, &rif_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        if (found) {
            if (SOC_IS_JERICHO(unit)) {
                local_in_lif_id = lif_indx;
                /*map global to local*/
                rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_in_lif_id, &global_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                global_lif_id  = local_in_lif_id = lif_indx;
            }
        }

    }

    if (!found) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                                                  local_in_lif_id,
                                                                  0x0, /* Default */
                                                                  0,   /* is_pwe_lif */
                                                                  0,
                                                                  NULL));

    if (DPP_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WIDE))
    {
        /* Call the SOC API for clear the additional data */
        COMPILER_64_SET(additinal_data, 0, 0);
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_set,(unit, local_in_lif_id, TRUE, additinal_data, TRUE)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_rv =
       soc_ppd_rif_mpls_label_map_remove(soc_sand_dev_id, &rif_key, (SOC_PPC_LIF_ID *)&local_in_lif_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (local_in_lif_id == BCM_GPORT_INVALID) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
    if (SOC_IS_JERICHO(unit)) {
        /*dealloc local+global lif*/
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, local_in_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = bcm_dpp_am_l3_lif_dealloc(unit, local_in_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, local_in_lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_mpls_tunnel_switch_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    /* verify parameters */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(info);

    /* all these flags should be reset */
    if (info->
        flags & (BCM_MPLS_SWITCH_COUNTED | BCM_MPLS_SWITCH_INT_PRI_SET |
                 BCM_MPLS_SWITCH_INT_PRI_MAP | BCM_MPLS_SWITCH_COLOR_MAP |
                 BCM_MPLS_SWITCH_INNER_EXP | BCM_MPLS_SWITCH_INNER_TTL |
                 BCM_MPLS_SWITCH_DROP)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags")));
    }

    /* Next protocol has to be specified in Arad or Arad+ with disabled soc property BCM886XX_MPLS_TERMINATION_KEY_MODE
       In this case the next protocol is used to find whether lif is BOS or not */
    if ((!soc_property_get(unit, spn_EVPN_ENABLE, 0)) && (info->action == BCM_MPLS_SWITCH_ACTION_POP) &&
        (!SOC_IS_ARAD_B1_AND_BELOW(unit) || !soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_KEY_MODE, 1)) &&
            ((info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_L2) || 
             (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV4) ||
             (info->flags & BCM_MPLS_SWITCH_NEXT_HEADER_IPV6))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags. Next protocol is taken from the first nibble of the next header after MPLS. See BCM88660 SW Compatibility between versions 6.3 6.4, Section MPLS L3 Router")));
    }

    /* not supported cobmination of flags */

    /* both should be set, or reset, (i.e. has same bit value) */
    if ((!(info->flags & BCM_MPLS_SWITCH_OUTER_EXP)
         && (info->flags & BCM_MPLS_SWITCH_OUTER_TTL))
        || ((info->flags & BCM_MPLS_SWITCH_OUTER_EXP)
            && !(info->flags & BCM_MPLS_SWITCH_OUTER_TTL))
        ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid configuration, expect BCM_MPLS_SWITCH_OUTER_TTL and BCM_MPLS_SWITCH_OUTER_EXP to have the same settings")));
    }
    /* unsupported action - out of range */
    if (info->action > BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Action type")));
    }

    /* if action is not pop, then his is ILM action */
    if (info->action != BCM_MPLS_SWITCH_ACTION_POP) {
        /* these flags should be set */
        if ((info->action != BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH) && 
            (info->action != BCM_MPLS_SWITCH_ACTION_POP_DIRECT) && 
            (info->action != BCM_MPLS_SWITCH_ACTION_PHP) && 
            (!(info->flags & BCM_MPLS_SWITCH_TTL_DECREMENT))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_SWITCH_TTL_DECREMENT must be set in case of ILM SWAP")));     /* TTL always decemented (even if inherented/copied */
        }

        rv = _bcm_petra_mpls_tunnel_switch_ilm_add(unit, info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {                    /* else this is termination operation */
        rv = _bcm_petra_mpls_tunnel_switch_term_add(unit, info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_tunnel_switch_create(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    rv = _bcm_petra_mpls_tunnel_switch_add(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
 exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_switch_delete(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv1 = BCM_E_NONE;
    int rv2 = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* Delete the ILM */
    if (info->action != BCM_MPLS_SWITCH_ACTION_POP || !(info->flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID)) {
        /* pop actions should not be removed with this API */
        /* for backward compatibility we delete also if the action is not valid */
        rv1 = _bcm_petra_mpls_tunnel_switch_ilm_remove(unit, info);
        if (rv1 != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv1);
        }
    }

    /* Delete Tunnel termination */
    if (info->action == BCM_MPLS_SWITCH_ACTION_POP || !(info->flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID)) {
        /* only pop actions should be removed with this API */
        /* for backward compatibility we delete also if the action is not valid */
            rv2 = _bcm_petra_mpls_tunnel_switch_term_remove(unit, info);
    } else if (rv1 == BCM_E_NOT_FOUND){
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given label is not found in ILM and irrelevant for Tunnel termination")));
    }
    
    if (rv1 == BCM_E_NOT_FOUND && rv2 == BCM_E_NOT_FOUND) {
       /* In case of both NOT FOUND return error */
       BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given label is not found in both ILM and Tunnel termination"))); /* the given label cannot set in any range */
    } else if (rv2 != BCM_E_NOT_FOUND) {
       /* in case tunnel termination returned error that is not found */
       BCMDNX_IF_ERR_EXIT(rv2);
    } else {
       /* in case everything is ok - first found, second not found */
       BCM_EXIT;
    }
    
    

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_tunnel_switch_delete_all(
    int unit)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 push_profile;
    int rv = BCM_E_NONE;
    _bcm_lif_type_e usage;
    bcm_mpls_tunnel_switch_t mpls_term;
    int cur_lif;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* clear ILM */
    soc_sand_rv = soc_ppd_frwrd_ilm_table_clear(soc_sand_dev_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /*clear push profile*/
    for (push_profile = 1; push_profile < NOF_MPLS_PUSH_PROFILE(unit); push_profile++) {
        int is_allocated = 0;
        int is_last = FALSE;

        rv = _bcm_dpp_am_template_mpls_push_profile_is_profile_allocated(unit, push_profile, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_allocated) {
            while (!is_last) {
                rv = _bcm_dpp_am_template_mpls_push_profile_free(unit, push_profile, &is_last);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    
    /* clear termination */
    bcm_mpls_tunnel_switch_t_init(&mpls_term);

    /* clear SEM termination */
    for (cur_lif = 0; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, cur_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &usage, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        /* if lif used for MPLS termiantion remove it */
        if (usage == _bcmDppLifTypeMplsTunnel) {
            rv = _bcm_dpp_in_lif_mpls_term_match_get(unit, &mpls_term, cur_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            /* When remove LIF that is without SEM lookup, LIF-ID must be input */
            if (mpls_term.flags & BCM_MPLS_SWITCH_LOOKUP_NONE) {
                int lif = 0;
				
				lif = cur_lif;
                if (SOC_IS_JERICHO(unit)) {
                    rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, cur_lif, &lif);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                BCM_GPORT_TUNNEL_ID_SET(mpls_term.tunnel_id, lif);
            }
            rv = _bcm_petra_mpls_tunnel_switch_term_remove(unit, &mpls_term);
            BCMDNX_IF_ERR_EXIT(rv);

            /* remove status */
            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, cur_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv); 
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_mpls_tunnel_switch_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    if (info->label == BCM_MPLS_LABEL_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS label must be filled, BCM_MPLS_LABEL_INVALID is illegal")));
    }

    /* get from termination db */
    rv = _bcm_petra_mpls_tunnel_switch_term_get(unit, info);
    if (rv == BCM_E_NOT_FOUND) {        /* if not found get from ILM db */
        rv = _bcm_petra_mpls_tunnel_switch_ilm_get(unit, info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_tunnel_switch_t_init not dispatchable */

int bcm_petra_mpls_tunnel_switch_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    int indx;
    uint32 nof_entries;
    bcm_mpls_tunnel_switch_t cur_entry;
    int cur_lif;
    _bcm_lif_type_e usage;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    bcm_mpls_tunnel_switch_t_init(&cur_entry);

    /* traverse SEM termination */
    for (cur_lif = 0; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, cur_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &usage, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        /* if lif used for MPLS termiantion remove it */
        if (usage == _bcmDppLifTypeMplsTunnel) {
            rv = _bcm_dpp_in_lif_mpls_term_match_get(unit, &cur_entry, cur_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_petra_mpls_tunnel_switch_term_get(unit, &cur_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = cb(unit, &cur_entry, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* traverse ILM */
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    /* excluding IPMC compatible entries */
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = _BCM_PETRA_MPLS_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    while (!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        soc_sand_rv =
            soc_ppd_frwrd_ilm_get_block(soc_sand_dev_id, &block_range,
                                    _bcm_mpls_traverse_ilm_keys[unit],
                                    _bcm_mpls_traverse_ilm_vals[unit], &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (nof_entries == 0) {
            break;
        }
        for (indx = 0; indx < nof_entries; ++indx) {
            /* get entry */
            rv = _bcm_petra_mpls_tunnel_switch_ilm_entry_to_bcm(unit,
                                                                &_bcm_mpls_traverse_ilm_keys[unit][indx],
                                                                &_bcm_mpls_traverse_ilm_vals[unit][indx],
                                                                &cur_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            /* act on entry */
            rv = cb(unit, &cur_entry, user_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_create(
    int unit,
    bcm_mpls_vpn_config_t * info)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vpn_id;
    int flags = 0;
    bcm_vlan_control_vlan_t vlan_control;
    _bcm_vsi_type_e vsi_usage_type;
    int update, with_id;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    vpn_id = _BCM_DPP_VPN_ID_GET(info->vpn);

    /* not supported flag */
    if ((info->flags & (BCM_MPLS_VPN_L3 | BCM_MPLS_VPN_VPWS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flags BCM_MPLS_VPN_L3 & BCM_MPLS_VPN_VPWS are not supported")));
    }

    /* for vpls user has to call with id */
    if (info->flags & BCM_MPLS_VPN_VPLS) {
        if (!(info->flags & BCM_MPLS_VPN_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN MPLS port has to be called with id")));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN MPLS port has to be called with flag BCM_MPLS_VPN_VPLS")));
    }

    /* Read the state of the VPN ID control flags and the VPN ID */
    update = (info->flags & BCM_MPLS_VPN_REPLACE) ? TRUE : FALSE;
    with_id = (info->flags & BCM_MPLS_VPN_WITH_ID) ? TRUE : FALSE;
    vpn_id = _BCM_DPP_VPN_ID_GET(info->vpn);
    rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vpn_id, &vsi_usage_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Validate the VPN ID control flags with the current allocation state */
    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_VPN_REPLACE can't be used without BCM_MPLS_VPN_WITH_ID as well")));
        } else if (vsi_usage_type == _bcmDppVsiTypeAny) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN not allocated. In case of Replace assumed VPN is already allocated")));
        } else if (!(vsi_usage_type & _bcmDppVsiTypeMpls)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The VPN is already allocated but not as an MPLS type")));
        }
    } else if (with_id && (vsi_usage_type & _bcmDppVsiTypeMpls)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case the VSI is already allocated, BCM_MPLS_VPN_REPLACE should be used")));
    }

    /* New VPN - Allocate the VPLS ID in the SW DBs */
    if (!update) {
        /* check if with ID */
        if (with_id) {
            flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        }

        /* Add MPLS to the VSI usage and perform alloc manager allocation, if it's an unused VSI */
        rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit, flags, _bcmDppVsiTypeMpls, &vpn_id ,NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    _BCM_DPP_VPN_ID_SET(info->vpn, vpn_id);

    /* Set flooding fields */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    vlan_control.broadcast_group = info->broadcast_group;
    vlan_control.unknown_multicast_group = info->unknown_multicast_group;
    vlan_control.unknown_unicast_group = info->unknown_unicast_group;
    /* forwarding vlan has to be equal to vsi */
    vlan_control.forwarding_vlan = info->vpn;

    rv = bcm_petra_vlan_control_vlan_set(unit,info->vpn,vlan_control);
    /* De-alloc vpn if setting failed */
    if (rv != BCM_E_NONE) {
        int rv_tmp = BCM_E_NONE;
        rv_tmp = _bcm_dpp_vswitch_vsi_usage_dealloc(unit, _bcmDppVsiTypeMpls, vpn_id);
        BCMDNX_IF_ERR_EXIT(rv_tmp);
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_destroy(
    int unit,
    bcm_vpn_t vpn)
{
    int vpn_id;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* check parameters */
    rv = _bcm_dpp_l2_vpn_check(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Delete all the mpls ports on this VPN */
    rv = bcm_petra_mpls_port_delete_all(unit, vpn);
    BCMDNX_IF_ERR_EXIT(rv);

    vpn_id = _BCM_DPP_VPN_ID_GET(vpn);

    /* free resource used for VPN (VSI) / VPN ID */
    rv = _bcm_dpp_vswitch_vsi_usage_dealloc(unit, _bcmDppVsiTypeMpls, vpn_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_destroy_all(
    int unit)
{
    int vsi;
    int cur_lif;
    _bcm_vsi_type_e mpls_usage;
    int rv = BCM_E_NONE;
    int vpn;
    bcm_vpn_t cur_vpn, vpn_id = 0;
    bcm_gport_t gport;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* Delete any MPLS ports on the cross connect VPN(0) */
    rv = bcm_petra_mpls_port_delete_all(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);

    /* destroy MP usage */
    /* go over, LIF and check get all valid LIF x VPN */
    for (cur_lif = 0; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        rv = _bcm_dpp_local_lif_to_vsi(unit, cur_lif, &vsi, &gport);
        BCMDNX_IF_ERR_EXIT(rv);

        /* not relevant lif */
        if (vsi == -1) {
            continue;
        }

        rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vsi, &mpls_usage);
        BCMDNX_IF_ERR_EXIT(rv);
        /* not relevant vsi */
        if ((mpls_usage & _bcmDppVsiTypeMpls) == 0) {
            continue;
        }
        /* get vpn */
        _BCM_DPP_VPN_ID_SET(cur_vpn, vsi);
        rv = bcm_petra_mpls_port_delete(unit, cur_vpn, gport);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Destroy MP usage */
    for (vpn = 0; vpn < _BCM_GPORT_NOF_VSIS; ++vpn) {
        rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vpn, &mpls_usage);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Destroy VPNs */
        if (mpls_usage & _bcmDppVsiTypeMpls) {
            _BCM_DPP_VPN_ID_SET(vpn_id, vpn);
            rv = bcm_petra_mpls_vpn_id_destroy(unit, vpn_id);
            BCMDNX_IF_ERR_EXIT(rv);   
        }       
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_vpn_id_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_vpn_config_t * info)
{
    int rv = BCM_E_NONE;
    int vpn_id;
    bcm_vlan_control_vlan_t vlan_control;
    _bcm_vsi_type_e vsi_usage_type;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(info);

    /* clear rest of information */
    bcm_mpls_vpn_config_t_init(info);
    info->flags = BCM_MPLS_VPN_VPLS;
    info->vpn = vpn;

    /* get vsi value */
    vpn_id = _BCM_DPP_VPN_ID_GET(info->vpn);

    /* Verify that the VSI is used for MPLS */
    rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vpn_id, &vsi_usage_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!(vsi_usage_type & _bcmDppVsiTypeMpls)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The VPN isn't allocated for MPLS")));
    }

    /* if mp get flooding information */
    bcm_vlan_control_vlan_t_init(&vlan_control);

    rv = bcm_petra_vlan_control_vlan_get(unit,info->vpn,&vlan_control);
    BCMDNX_IF_ERR_EXIT(rv);  

    _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group, _BCM_MULTICAST_TYPE_VPLS, _BCM_MULTICAST_ID_GET(vlan_control.unknown_unicast_group));
    _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group, _BCM_MULTICAST_TYPE_VPLS, _BCM_MULTICAST_ID_GET(vlan_control.unknown_multicast_group));
    _BCM_MULTICAST_GROUP_SET(info->broadcast_group, _BCM_MULTICAST_TYPE_VPLS, _BCM_MULTICAST_ID_GET(vlan_control.broadcast_group));                      

exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_mpls_vpn_t_init not dispatchable */

int bcm_petra_mpls_vpn_traverse(
    int unit,
    bcm_mpls_vpn_traverse_cb cb,
    void *user_data)
{
    int vpn;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_vpn_t cur_vpn;
    int rv;
    uint8 used;

    BCMDNX_INIT_FUNC_DEFS;

    /* traverse MP usage */
    for (vpn = BCM_VLAN_DEFAULT; vpn < _BCM_GPORT_NOF_VSIS; ++vpn) {
        rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vpn);
        if (rv == BCM_E_EXISTS ) {
            rv = _bcm_dpp_vswitch_vsi_usage_type_check(unit, vpn, _bcmDppVsiTypeMpls, &used);
            BCMDNX_IF_ERR_EXIT(rv);
            if (used) {
                _BCM_DPP_VPN_ID_SET(cur_vpn, vpn);

                rv = bcm_petra_mpls_vpn_id_get(unit, cur_vpn, &vpn_info);
                BCMDNX_IF_ERR_EXIT(rv);

                cb(unit, &vpn_info, user_data);
            }
        }
    }

    BCM_EXIT;

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_mpls_range_action_add(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t * action)
{
    int rv = BCM_E_NONE, soc_sand_rv, err_code = 0;
    uint32 vacant_entry_index;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;

    int vtt_pwe_vid_search = SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search;
    int inner_flag = 0;
    int outer_flag = 0;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);
    
    if(vtt_pwe_vid_search){/* in case soc-property is on, make sure we have exactly 1 flag on*/
        switch (action->flags){
        case BCM_MPLS_RANGE_ACTION_COMPRESSED:
        case BCM_MPLS_RANGE_ACTION_EVPN_IML:
        case BCM_MPLS_RANGE_ACTION_VID :
        case BCM_MPLS_RANGE_ACTION_INNER_VID:
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("use exactly one flag for this API!")));
        }
    } else { /* in case soc-property is off, this two flags cant be used */
        if (action->flags & BCM_MPLS_RANGE_ACTION_VID || action->flags & BCM_MPLS_RANGE_ACTION_INNER_VID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag cant be used in default mode, need soc-property vtt_pwe_vid_search first!")));
        }
    }

    if (action->flags & BCM_MPLS_RANGE_ACTION_COMPRESSED) {
        if (action->compressed_label != label_low) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("compressed_label must equal label_low in case BCM_MPLS_RANGE_ACTION_COMPRESSED is on")));
        }
        if (action->flags & BCM_MPLS_RANGE_ACTION_EVPN_IML) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_RANGE_ACTION_COMPRESSED and BCM_MPLS_RANGE_ACTION_EVPN_IML cannot be on togehter")));
        }
        range_profile_info.mpls_label_range_use_base = 1;
    }

    if (!(label_high >= label_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("label_high can't be smaller than label_low")));
    }

    /* bos_value_mask==1 means don't care */
    range_action_info.bos_value_mask = 1;
    range_action_info.label_low = label_low;
    range_action_info.label_high = label_high;

    if (action->flags & BCM_MPLS_RANGE_ACTION_EVPN_IML) {
        if (!soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property EVPN_ENABLE must be on in case BCM_MPLS_RANGE_ACTION_EVPN_IML is on")));
        }
        range_profile_info.mpls_label_range_tag_mode = 1;
        range_profile_info.mpls_label_range_has_cw = 0;
        range_profile_info.mpls_label_range_set_outer_vid = 1;
        range_profile_info.mpls_label_range_set_inner_vid = 0;
        range_profile_info.mpls_label_range_use_base = 0;
        range_action_info.bos_value = 1;
        /* bos_value_mask==0 means check bos */
        range_action_info.bos_value_mask = 0;
    }

    inner_flag = action->flags & BCM_MPLS_RANGE_ACTION_INNER_VID;
    outer_flag = action->flags & BCM_MPLS_RANGE_ACTION_VID;
    if (vtt_pwe_vid_search && (outer_flag || inner_flag) ) {
        range_profile_info.mpls_label_range_tag_mode = 1;
        range_profile_info.mpls_label_range_has_cw = 0;
        range_profile_info.mpls_label_range_use_base = 0;
        range_profile_info.mpls_label_range_set_outer_vid = 0;
        range_profile_info.mpls_label_range_set_inner_vid = 0;

        if (outer_flag) { 
            range_profile_info.mpls_label_range_set_outer_vid = 1;
        }

        if (inner_flag) { 
            range_profile_info.mpls_label_range_set_inner_vid = 1;
        }
    }


    rv = _bcm_dpp_mpls_range_action_verify(unit,  label_low, label_high,&vacant_entry_index,&err_code);
    BCMDNX_IF_ERR_EXIT(rv);
    if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Given label range already exists")));
    } else if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label range intersects with an already allocated one")));
    } else if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Tabel is full")));
    } else if (err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST_IML) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("IML label range already exists using mode 20-23")));
    }

    range_action_info.label_low = label_low;
    range_action_info.label_high = label_high;

    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_set,(unit, vacant_entry_index, &range_action_info,action->flags)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_set,(unit, vacant_entry_index, &range_profile_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_range_action_remove(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high)
{
    int rv = BCM_E_NONE, soc_sand_rv, err_code = 0;
    uint32 entry_index;
    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;
    bcm_mpls_range_action_t action;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    bcm_mpls_range_action_t_init(&action);
    
    BCM_DPP_UNIT_CHECK(unit);

    if (!(label_high >= label_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("label_high can't be smaller than label_low")));
    }
    
    rv = _bcm_dpp_mpls_range_action_verify(unit, label_low, label_high,&entry_index,&err_code);
    BCMDNX_IF_ERR_EXIT(rv);
    if (err_code  != _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given label range doesn't exist in the table")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23) {
       rv = bcm_petra_mpls_range_action_get(unit,label_low,label_high,&action);
       BCMDNX_IF_ERR_EXIT(rv);
    }
    /* Placing zeros in the relevant entry */
    range_action_info.bos_value_mask = 0;
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_set,(unit, entry_index, &range_action_info,action.flags)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_set,(unit, entry_index, &range_profile_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_mpls_range_action_get(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t *action
    )
{
    int rv = BCM_E_NONE, soc_sand_rv, err_code = 0;
    uint32 entry_index;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO range_profile_info;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(&range_profile_info);
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(action);

    if (!(label_high >= label_low)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("label_high can't be smaller than label_low")));
    }
    
    rv = _bcm_dpp_mpls_range_action_verify(unit, label_low, label_high,&entry_index,&err_code);
    BCMDNX_IF_ERR_EXIT(rv);
    if (err_code  != _BCM_MPLS_TERM_LABEL_RANGE_EXIST) {
        action->compressed_label = 0;
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given label range doesn't exist in the table")));
    }

    soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_profile_get,(unit, entry_index, &range_profile_info)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    if (range_profile_info.mpls_label_range_use_base == 1) {
        action->compressed_label = label_low;
        action->flags |= BCM_MPLS_RANGE_ACTION_COMPRESSED; 
    } else { /* range_profile_info.mpls_label_range_use_base == 0 */
        if (soc_property_get(unit, spn_EVPN_ENABLE, 0) && range_profile_info.mpls_label_range_tag_mode == 1 \
            && range_profile_info.mpls_label_range_has_cw == 0  \
            && range_profile_info.mpls_label_range_set_outer_vid == 1 \
            && range_profile_info.mpls_label_range_set_inner_vid == 0 ) \
        {
            action->flags |= BCM_MPLS_RANGE_ACTION_EVPN_IML; 
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}
 
/* 
 * This is an auxilliary function that receives a range of mpls labels and returns a value err_code
 * that signifies the presence of this range in the table. 
 * Optional return values: 
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL : Entries are exhausted
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_EXIST: given label range exists in a certain entry.
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT: given label range intersects with a certain (label in) entry
 *  err_code == _BCM_MPLS_TERM_LABEL_RANGE_VACANT_ENTRY: There is a vacant entry in the table, to which this label can be assigned.
 *  The vacant entry will be returned in vacant_entry_index.
 */
int _bcm_dpp_mpls_range_action_verify(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    uint32 * vacant_entry_index,
    int *err_code)
{
    uint32 entry_index;
    int soc_sand_rv, found = 0;
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO range_action_info;
    bcm_mpls_range_action_t action_profile;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(&range_action_info);
    bcm_mpls_range_action_t_init(&action_profile);

    BCM_DPP_UNIT_CHECK(unit);

    *err_code = _BCM_MPLS_TERM_LABEL_RANGE_TABLE_FULL;

    for (entry_index = 0; entry_index < _BCM_MPLS_TERM_LABEL_RANGE_NOF_ENTRIES; entry_index++) {
        soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_range_action_get,(unit, entry_index, &range_action_info)));
        BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        /* Checks whether the given range is identical to the range in the current entry*/
        if (range_action_info.label_low == label_low && range_action_info.label_high == label_high) {
            *vacant_entry_index = entry_index;
            *err_code = _BCM_MPLS_TERM_LABEL_RANGE_EXIST;
            break; 
            
        }
        /* Checks whether the given range intersects with the range in the current entry*/
        if ((range_action_info.label_low <= label_low &&  range_action_info.label_high >= label_low) || \
            (range_action_info.label_low <= label_high &&  range_action_info.label_high >= label_high) ) {
            *vacant_entry_index = entry_index;
            *err_code = _BCM_MPLS_TERM_LABEL_RANGE_INTERSECT;
            break; 
        }
        /* check that IML range is already used per NEW VTT mode 20-23 */
        if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23) {
        	soc_sand_rv = bcm_petra_mpls_range_action_get(unit,range_action_info.label_low,range_action_info.label_high,&action_profile);
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            if (action_profile.flags & BCM_MPLS_RANGE_ACTION_EVPN_IML) {
            	*vacant_entry_index = entry_index;
            	*err_code = _BCM_MPLS_TERM_LABEL_RANGE_EXIST_IML;
            	break;
            }
        }

        /* Saving an empty entry */
        if (!range_action_info.label_low && !range_action_info.label_high && !found) {
            found = 1;
            *vacant_entry_index = entry_index;
            *err_code = _BCM_MPLS_TERM_LABEL_RANGE_VACANT_ENTRY;
        }
    }
 
exit:
    BCMDNX_FUNC_RETURN;
} 

int _bcm_gport_mpls_is_valid_replace(int unit, bcm_mpls_port_t *new_mpls_port, bcm_mpls_port_t *old_mpls_port, bcm_mpls_label_t old_egress_vc, uint32 old_push_profile){

    int o_is_protected;
    int n_is_protected;
	int n_is_egress_protection;
    uint32 new_push_profile;
    uint32 new_pwe_has_cw;
    int rv;
        
    BCMDNX_INIT_FUNC_DEFS;

    o_is_protected = _BCM_PPD_IS_VALID_FAILOVER_ID(old_mpls_port->failover_id);
    n_is_protected = _BCM_PPD_IS_VALID_FAILOVER_ID(new_mpls_port->failover_id);

    n_is_egress_protection = (new_mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_PROTECTION)? TRUE : FALSE;

    if (n_is_egress_protection) {
        if (!(old_mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_PROTECTION)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF is already allocated as non protection and can't replace to protection")));
        }
    }
    else {
        if (old_mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_PROTECTION) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF is already allocated as protection and can't replace to non protection")));
        }
    }

    if(o_is_protected != n_is_protected) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change protection on replace, cause affect gport id")));
    }

    if((new_mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) && (new_mpls_port->encap_id != old_mpls_port->encap_id)) {
        /* Only in protected remote PWE the encapi_id can be updated */
        if (!n_is_protected) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change encap if not protected as it part of ID")));
        }
        if ((!BCM_ENCAP_REMOTE_GET(new_mpls_port->encap_id)) || (!BCM_ENCAP_REMOTE_GET(old_mpls_port->encap_id))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change encap if not remote")));
        }
    }


    /* If with-encap then has to be same encap as exists */
    if((!n_is_protected) && (new_mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID)) {
        if(new_mpls_port->encap_id != old_mpls_port->encap_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change encap if not protected as it part of ID")));
        }
    }
    else if (BCM_ENCAP_REMOTE_GET(new_mpls_port->encap_id) && (new_mpls_port->encap_id != old_mpls_port->encap_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change encap if not protected as it part of ID")));
    }

    /* if not with encap then reuse exist encap */
    if(!(new_mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) && 
         !(new_mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY)) {
        new_mpls_port->encap_id = old_mpls_port->encap_id;
        new_mpls_port->flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
    }

    /* egress label can be modified only in case PWE is protected or egress only or counted */
    if ((!n_is_protected) && (!(new_mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY)) && 
        (!(new_mpls_port->flags & BCM_MPLS_PORT_COUNTED)) && (!(new_mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP)) && 
        (!(new_mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP_EEI)) && (new_mpls_port->egress_label.label != old_egress_vc) && 
        (new_mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change egress label in case it is part of learning information")));
    }

    /* push profile can be modified only in case PWE is protected */
    new_pwe_has_cw = (new_mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD)?1:0;

    rv = _bcm_mpls_tunnel_push_profile_alloc(unit,
                                             &new_push_profile,
                                             new_pwe_has_cw,
                                             &new_mpls_port->egress_label,
                                             TRUE,
                                             FALSE,
                                             FALSE,
                                             FALSE,
                                             NULL,
                                             0);

    if ((!n_is_protected) && (!(new_mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY)) && 
        (!(new_mpls_port->flags & BCM_MPLS_PORT_COUNTED)) && (!(new_mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP)) && 
        (!(new_mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP_EEI)) &&(((new_push_profile != old_push_profile) && 
        (new_mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID)) || (rv != BCM_E_NONE))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("cannot change push profile in case it is part of learning information")));
    }

    /* If setting an Ingress Protection */
    if (_BCM_MULTICAST_IS_SET(new_mpls_port->failover_mc_group)) {
        /* Verify that Ingress drop isn't already defined for the In-LIF */
        if (old_mpls_port->failover_id == _BCM_INGRESS_PROTECTION_POINTER_INVALID) {
            return BCM_E_UNAVAIL;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * set EEP information for PWE for MPLS port, given EEP = LIF
 */
int _bcm_dpp_mpls_port_pwe_eep_set(
    int unit,
    bcm_mpls_port_t*    mpls_port,
    int  local_out_lif,
    uint32  push_profile,
    SOC_PPC_EG_ENCAP_PWE_INFO *old_pwe_info,
    uint8 is_egress_protection,
    uint8 is_one_plus_one_protection,
    int is_pwe_binded_with_mpls)
{
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    uint32
        soc_sand_rv,
        profile_split_horizon_val = 0;
    SOC_PPC_FEC_ID fec_index;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE
      protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
      working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
      protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO
      protect_info;
    SOC_PPC_EG_ENCAP_PWE_INFO
            eep_pwe_info;
    int intf_global_lif = 0, intf_local_lif = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    SOC_PPC_EG_ENCAP_PWE_INFO_clear(&eep_pwe_info);

    /* set EEP/PWE information */

    if (is_pwe_binded_with_mpls) {
         _rv = _bcm_mpls_tunnel_bcm_to_ppd(unit,&(mpls_port->egress_tunnel_label),0,&(eep_pwe_info.egress_tunnel_label_info.tunnels[0]),1);
         eep_pwe_info.egress_tunnel_label_info.nof_tunnels = 1;
         BCMDNX_IF_ERR_EXIT(rv);
    }

    eep_pwe_info.push_profile = push_profile;            
    eep_pwe_info.label = mpls_port->egress_label.label;  

    if (SOC_IS_JERICHO(unit) && (mpls_port->flags & BCM_MPLS_PORT_REPLACE)) {
        eep_pwe_info.outlif_profile = old_pwe_info->outlif_profile;
    }

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
        if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
            eep_pwe_info.orientation = mpls_port->network_group_id & 1;
            profile_split_horizon_val = mpls_port->network_group_id;
        } else {
            eep_pwe_info.orientation = 0;
        }
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
            (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
             profile_split_horizon_val, &(eep_pwe_info.outlif_profile)));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        eep_pwe_info.orientation  = (mpls_port->flags & BCM_MPLS_PORT_NETWORK)? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    }

    /* update is_l2_lif in outlif profile.
       Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
    if (SOC_IS_JERICHO_PLUS(unit)) {
        /* set entry to be L2LIF */
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                          (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                              &(eep_pwe_info.outlif_profile)));
        BCMDNX_IF_ERR_EXIT(soc_sand_rv);
    }

    /* get FEC and resolve next EEP */
    if (BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if)) {
        rv = _bcm_l3_encap_id_to_eep(unit, mpls_port->egress_tunnel_if, &intf_global_lif);
        BCMDNX_IF_ERR_EXIT(rv);

    } else if (BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if)) {
        rv = _bcm_l3_intf_to_fec(unit,mpls_port->egress_tunnel_if,&fec_index);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_index,&protect_type,&working_fec,&protect_fec,&protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        intf_global_lif = working_fec.eep;
    }

    if (intf_global_lif != 0) {
        /* In Jericho Plus (but not QAX) EEI-OutLif race confition HW improvement was applied, so no need to set next outlif to zero */
        if (SOC_IS_JERICHO(unit) && !SOC_IS_JERICHO_PLUS_A0(unit) && !is_one_plus_one_protection && ((BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && _BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id)) \
            || ((mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP) || (mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP_EEI) || mpls_port->flags & BCM_MPLS_PORT_COUNTED)) ) { /* add the COUNTED flag*/
            /* H-Protection or two pointers case in Jericho - PWE is not pointing to MPLS */
            intf_local_lif = 0;
        }
        else {
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, intf_global_lif, &intf_local_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }


    if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
        eep_pwe_info.drop = old_pwe_info->drop;
        eep_pwe_info.oam_lif_set = old_pwe_info->oam_lif_set;
    }

    /* Set Egress Protection failover ID */
    if (is_egress_protection) {
        DPP_FAILOVER_ID_GET(eep_pwe_info.protection_info.protection_pointer, mpls_port->egress_failover_id);
        eep_pwe_info.protection_info.protection_pass_value = (mpls_port->egress_failover_port_id) ? 0 : 1;
        eep_pwe_info.protection_info.is_protection_valid = TRUE;
    }

    soc_sand_rv = soc_ppd_eg_encap_pwe_entry_add(soc_sand_dev_id,local_out_lif,&eep_pwe_info,intf_local_lif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_out_lif_mpls_match_add(unit,mpls_port,local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((SOC_DPP_CONFIG(unit)->arad->pp_op_mode.split_horizon_filter_enable)) {
        soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(soc_sand_dev_id, local_out_lif, eep_pwe_info.orientation);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * allocate PWE for MPLS port
 */
int _bcm_dpp_mpls_port_pwe_set(
   int unit,
   bcm_vpn_t   vpn,
   bcm_mpls_port_t *mpls_port,
   SOC_PPC_EG_ENCAP_PWE_INFO *old_pwe_info) {
    uint32 push_profile = 0;
    SOC_SAND_SUCCESS_FAILURE
       failure_indication;
    SOC_PPC_L2_LIF_PWE_INFO  pwe_info;
    SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO  pwe_additional_info;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    int global_lif_flags = 0, lif_flags = 0, fec_flags = 0;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    _BCM_GPORT_PHY_PORT_INFO temp_phy_port;
    uint32 lif_types, pwe_lif_types = 0;
    uint32 sys_port = 0;
    SOC_PPC_LIF_ID local_in_lif = 0;
    int local_out_lif = 0, global_lif_id = 0, old_mpls_port_local_out_lif = 0;
    int is_lag = 0;
    int is_1_to_1_protected = 0;
    int mp = 0, eep_required = 0, is_failover_port_valid;
    uint8 is_1_plus_1, is_mc_ingress = 0, is_egress_protection = 0;
    SOC_PPC_FEC_ID fec_id = -1, protect_fec_id;
    int update = 0;
    bcm_mpls_port_t mpls_port_o;
    uint32 local_in_lif_o;
    uint32 inner_flags = 0;
    SOC_SAND_PP_MPLS_LABEL in_vc_label;
    uint8 remote_lif = 0;
    int is_last;
    uint8 is_ingress, is_egress;
    uint8 with_id, encap_with_id, is_forward_group;
    _bcm_dpp_gport_info_t gport_info;
    uint8 is_gal;
    _bcm_dpp_am_fec_alloc_usage fec_alloc_usage;
    uint32 profile_split_horizon_val = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint8 counting_profile_is_allocated;
    int is_pwe_binded_with_mpls = 0;
    uint32 match_vlan_domain_n = 0,match_vlan_domain_o = 0, update_vlan_domain = 0;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_L2_LIF_PWE_INFO_clear(&pwe_info);
    SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

    if (SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) {
            pwe_additional_info.inner_vid_valid = 0x1;
            pwe_additional_info.inner_vid = mpls_port->match_inner_vlan;
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) {
            pwe_additional_info.outer_vid_valid = 0x1;
            pwe_additional_info.outer_vid = mpls_port->match_vlan;
        }
    }

    if (mpls_port->flags & BCM_MPLS_PORT_REPLACE) {
        update = 1;
        bcm_mpls_port_t_init(&mpls_port_o);
        mpls_port_o.mpls_port_id = mpls_port->mpls_port_id;
        bcm_petra_mpls_port_get(unit, vpn, &mpls_port_o);
    }

     /*for port*label match, get vd value from hw table*/
    if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT) {
         SOC_PPC_PORT   soc_ppc_port;
         SOC_PPC_PORT_INFO  port_info;
          int       core;
         if (!BCM_GPORT_IS_LOCAL(mpls_port->port)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local port must be specified when working at port match mode")));
          }
         BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, BCM_GPORT_LOCAL_GET(mpls_port->port), &soc_ppc_port, &core)));       
         soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppc_port, &port_info);
         match_vlan_domain_n = port_info.vlan_domain;
         pwe_additional_info.vlan_domain_valid = 1;
         pwe_additional_info.vlan_domain = match_vlan_domain_n;

        if(update) {
	         _bcm_dpp_gport_hw_resources gport_hw_resources_tmp;
		 _bcm_dpp_gport_sw_resources gport_sw_resources_tmp;
	         int        is_ingress_tmp;
	          BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, mpls_port->mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF, &gport_hw_resources_tmp));

	          is_ingress_tmp = (gport_hw_resources_tmp.global_in_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) ? FALSE : TRUE;

	          if (is_ingress_tmp) {
		       BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, gport_hw_resources_tmp.local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources_tmp));
	          }

                 match_vlan_domain_o = gport_sw_resources_tmp.in_lif_sw_resources.match2;
                 update_vlan_domain = (match_vlan_domain_o !=  match_vlan_domain_n)?1:0;
                 /*Use the current vlan domain for get operation*/
                 pwe_additional_info.vlan_domain = match_vlan_domain_o;

        }
    }


    /* flags */
    pwe_info.has_cw = (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) ? 1 : 0;
    /* In Jericho, the CASCADED flag indicates that the FEC is group B (pointed FEC). */
    fec_alloc_usage = (mpls_port->flags2 & BCM_MPLS_PORT2_CASCADED) ? _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED : _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD;
    

    if (mpls_port->egress_tunnel_label.flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID) {
        is_pwe_binded_with_mpls = 1;
    }

    is_forward_group = (mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) ? 1 : 0;
    is_gal = (mpls_port->flags & BCM_MPLS_PORT_WITH_GAL) ? 1 : 0;
    if (is_gal) {
        /* Retreive InLIF local for GAL */
        rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port->tunnel_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                            &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Find the protection type on port (if any) and verify it's legal. */
    rv = _bcm_dpp_gport_l2_intf_protection_validation(unit, mpls_port, BCM_GPORT_MPLS_PORT, TRUE, &is_1_to_1_protected,
                                                      NULL, &is_mc_ingress, &is_1_plus_1, &is_egress_protection);
    BCMDNX_IF_ERR_EXIT(rv);

    mp = _BCM_DDP_IS_MP_VPN(vpn);

    /* 
    * Set flags for ingress/egress operations and perform related validations 
    */
    rv = _bcm_dpp_gport_l2_intf_ingress_egress_only_validation(mpls_port, TRUE, &is_ingress, &is_egress, BCM_GPORT_MPLS_PORT);
    BCMDNX_IF_ERR_EXIT(rv);


    /* In case of forward group, ingress and egress are not configured */
    if (is_forward_group) {
        is_ingress = is_egress = 0;
    }

    if (mpls_port->port != BCM_GPORT_INVALID) {
        /* is this configuration on local device or remote device */
        rv = _bcm_dpp_gport_to_phy_port(unit, mpls_port->port, 0, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        is_lag = _BCM_DPP_GPORT_INFO_IS_LAG(gport_info);
        if (is_lag) {
           sys_port = BCM_GPORT_TRUNK_GET(mpls_port->port);
        } else {
           sys_port = gport_info.sys_port;
        }
    }

    /* Read the state of the flags for the following input validations */
    with_id = (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) ? TRUE : FALSE;
    encap_with_id = (mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) ? TRUE : FALSE;

    /*
     * encap_id = LIF = EEP (when alloced) 
     * mpls_port_id = protected? FEC : LIF.
     */
    if (encap_with_id) {
        global_lif_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        global_lif_id = BCM_ENCAP_ID_GET(mpls_port->encap_id);
        if (global_lif_id > SOC_DPP_CONFIG(unit)->pp.nof_global_out_lifs-1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mpls_port->encap_id is invalid")));
        }
        remote_lif = BCM_ENCAP_REMOTE_GET(mpls_port->encap_id);
        if (remote_lif) {
            is_ingress = is_egress = 0;
        }
        if (!is_1_to_1_protected && !is_forward_group && remote_lif) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("remote encap can be called only when mpls-port is protected or in case of FORWRDING_GROUP")));
        }
    } else if (with_id) { /* if not with encap, but with id, then take encap from ID if relevant */
        rv = _bcm_dpp_gport_l2_intf_get_global_lif_from_port_id(mpls_port, is_1_to_1_protected, is_mc_ingress, (SOC_PPC_LIF_ID *)&global_lif_id, BCM_GPORT_MPLS_PORT);
        BCMDNX_IF_ERR_EXIT(rv);

        if (global_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) { /* If the lif id is valid, then then WITH_ID flag is for the lif allocation. */
            global_lif_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        }
    }

    if (with_id) {
        if (is_1_to_1_protected) {
            rv = _bcm_dpp_gport_l2_intf_calculate_act_fec(mpls_port, (int *)(&fec_id), BCM_GPORT_MPLS_PORT);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (is_forward_group) { /* Override previous value */
            if (BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id)) {
                if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id))) {
                    fec_id = BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id));
                }
                else {
                    fec_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
                }
            }
            else {
                fec_id = mpls_port->mpls_port_id;
            }
        }

		/* In Jericho, Jericho_Plus and REPLACE, check that the CASCADED/not-CASCADED matches.
		   No such limitaion in QAX*/
        if ((is_forward_group || is_1_to_1_protected) && SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit) && update == 1) {
            _bcm_dpp_am_fec_alloc_usage old_fec_usage;
            rv = bcm_dpp_am_fec_get_usage(unit, fec_id, &old_fec_usage);
            BCMDNX_IF_ERR_EXIT(rv);

            if (fec_alloc_usage != old_fec_usage) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("a CASCADED FEC may not be replaced with a regular FEC (and vice versa)")));
            }
        }

        /* Usually, the given mpls_port_id doesn't take into consideration the type of mpls port used (fec, mc or lif). This is
           here so we find the relevant type */
        rv = _bcm_dpp_gport_l2_intf_calculate_port_id(unit, fec_id, global_lif_id, mpls_port, BCM_GPORT_MPLS_PORT);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* allocate needed resources */
    /* 1. allocate LIF to terminate PWE */
    /* 2. allocate EEP for flooding encapsulation and for protected PWE */
    /* 3. allocate FEC for protected PWE */

    if (is_ingress || is_egress) {
        /* 1+2: allocate EEP + LIF
           1. if MP for flooding encapsulation & local or
           2. protected PWE & local
           3. or in order to sync EEP and LIF database. currently always
        */
        if (is_ingress) {
            /* Retreive lif type by port and label */
            if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
                rv = _bcm_petra_mpls_term_resolve_lif_type_by_index(unit, mpls_port->port, mpls_port->match_label, &lif_types);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                lif_types = _BCM_DPP_AM_L3_LIF_MPLS_TERM; /* Default */
            }

            /* Convert lif types MPLS to lif types PWE */
            rv = _bcm_petra_mpls_term_to_pwe_lif_types(unit, lif_types, &pwe_lif_types);
            BCMDNX_IF_ERR_EXIT(rv);
        }


        if (!remote_lif) {			
            if (!update) {
                uint8 allocate_egress = FALSE;

                /* We will allocate outlif if one of the following is true:
                   1. PWE is multipoint and not protected - than we need the Outlif for multicast traffic only.
                   2. 1:1 PWE protection - FEC that points to 2 PWE-OutLIFs each OutLIF signal different VC label and MPLS tunnel.
                   3. MPLS tunnel is not FEC - then PWE must be Outlif that points to the MPLS EEI.
                   If PWE is P2P and not protected - then we don't need the Outlif. */
                if (((mpls_port->flags & BCM_MPLS_PORT_COUNTED) || (mp) || (is_1_to_1_protected) || (is_1_plus_1) || (is_egress_protection) || (!BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))) && is_egress) {
                    allocate_egress = TRUE;
                    eep_required = 1;
                }

                if (allocate_egress && 
                    (mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID) &&
                    !_BCM_DPP_MPLS_LABEL_IN_RANGE(mpls_port->egress_label.label)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MPLS label is out of range. The range is 0 - (1<<20)-1")));
                 }

                /* Do the lif allocation according to chip */
                if (SOC_IS_JERICHO(unit)) {
                    bcm_dpp_am_local_inlif_info_t local_inlif_info, *local_inlif_info_p = NULL;
                    bcm_dpp_am_local_out_lif_info_t local_out_lif_info, *local_out_lif_info_p = NULL;

                    sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
                    sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

                    /* 
                     *  Set the local lifs info
                     */
                    /* 
                     * Use-LIF-counting-profile?
                     */
                    rv = bcm_dpp_counter_lif_range_is_allocated(unit, &counting_profile_is_allocated);
                    BCMDNX_IF_ERR_EXIT(rv);
                    /* Inlif info */
                    if (is_ingress) {
                        /* Get the application type by the lif type */
                        rv = bcm_dpp_am_l3_lif_type_to_app_type(lif_types, &local_inlif_info.app_type);
                        BCMDNX_IF_ERR_EXIT(rv);
                        local_inlif_info.local_lif_flags = (DPP_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_WIDE)) ?
                           BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;

                        if (counting_profile_is_allocated) {
                            if (mpls_port->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE && mpls_port->inlif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OutLIF-counting-profile %d is out of range."), mpls_port->inlif_counting_profile));
                            }
                            BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, mpls_port->inlif_counting_profile, &local_inlif_info.counting_profile_id));                            
                        } else {
                            local_inlif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
                        }

                        /* Set the inlif info pointer to valid */
                        local_inlif_info_p = &local_inlif_info;
                    }

                    /* Outlif info */
                    if (allocate_egress) {
                        local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_mpls_tunnel;
                         if ((mpls_port->flags & BCM_MPLS_PORT_EXTENDED ) 
                                &&soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0) == 1
                                &&SOC_IS_JERICHO_PLUS(unit)){
                            /*This pwe is the first entry in EE list when performing four seperate mpls pushing, it uses stage 2 bank*/    
                            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_third_mpls_tunnel;
                        } else if ( SOC_IS_JERICHO(unit) && mpls_port->egress_label.encap_access > bcmEncapAccessInvalid) { 
                         /*
                          * User could indicate the bank pahse explicitly now
                          * by setting encap_access other than bcmEncapAccessInvalid
                          * bcmEncapAccessNativeArp : native link layer(1),only relevant for J+/QAX
                          * bcmEncapAccessTunnel1 : MPLS tunnel 1(3)
                          * bcmEncapAccessTunnel2 : MPLS tunnel 2(4)
                          */
                          if (SOC_IS_JERICHO_AND_BELOW(unit) && mpls_port->egress_label.encap_access == bcmEncapAccessNativeArp) {
                             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NativeARP is only supported by Jer_Plus")));
                          }
                          local_out_lif_info.app_alloc_info.application_type = (mpls_port->egress_label.encap_access == bcmEncapAccessNativeArp)?bcm_dpp_am_egress_encap_app_third_mpls_tunnel:
                                                                                               (mpls_port->egress_label.encap_access == bcmEncapAccessTunnel1)?bcm_dpp_am_egress_encap_app_second_mpls_tunnel:bcm_dpp_am_egress_encap_app_mpls_tunnel;                      
                        } else {
                            local_out_lif_info.app_alloc_info.application_type = (is_ingress) ? bcm_dpp_am_egress_encap_app_pwe
                               : bcm_dpp_am_egress_encap_app_second_mpls_tunnel;
                        }
                        if (counting_profile_is_allocated) {
                            if (mpls_port->egress_label.outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE && 
                                mpls_port->egress_label.outlif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OutLIF-counting-profile %d is out of range."), mpls_port->egress_label.outlif_counting_profile));
                            }
                            BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, mpls_port->egress_label.outlif_counting_profile, &local_out_lif_info.counting_profile_id));
                        } else {
                            local_out_lif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
                        }

                        /* Set the outlif info pointer to valid */
                        local_out_lif_info_p = &local_out_lif_info;


                        /* Egress protection PWE LIFs should be assigned from an egress protection EEDB bank */
                        if (is_egress_protection) {

                            /* Format the Local Out-LIF allocation attributes in order
                               to get the required Egress Protection Out-LIF */
                            local_out_lif_info.local_lif_flags |= BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
                            DPP_FAILOVER_ID_GET(local_out_lif_info.failover_id, mpls_port->egress_failover_id);
                        }
                    }

                    /* 
                     *  Allocate the global and local lifs.
                     */
                    if (local_inlif_info_p || local_out_lif_info_p) {
                        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, global_lif_flags, &global_lif_id, local_inlif_info_p, local_out_lif_info_p);
                        BCM_IF_ERROR_RETURN(rv);
                    }

                    /* 
                     * Retrieve the allocated local lifs
                     */
                    local_in_lif = local_inlif_info.base_lif_id;
                    local_out_lif = local_out_lif_info.base_lif_id;

                } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                    lif_flags |= global_lif_flags;
                    if (allocate_egress) {
                        if (is_ingress) {
                            rv = bcm_dpp_am_l2_vpn_pwe_alloc(unit,pwe_lif_types,lif_flags,1,(SOC_PPC_LIF_ID*)&global_lif_id);
                            BCMDNX_IF_ERR_EXIT(rv);

                            /* allocate LIF in same place where EEP allocated */
                            lif_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                            
                        }
                        rv = bcm_dpp_am_mpls_eep_alloc(unit, (is_ingress && is_egress) ? _BCM_DPP_AM_MPLS_EEP_TYPE_PWE : _BCM_DPP_AM_MPLS_EEP_TYPE_PWE_EGRESS_ONLY, lif_flags, 1, &global_lif_id);
                        BCMDNX_IF_ERR_EXIT(rv);

                    } else if (is_ingress) {
                        /* Allocating only InLif */
                        rv = bcm_dpp_am_l3_lif_alloc(unit, lif_types, lif_flags, (int *)&global_lif_id);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }

                    /* In aradplus and below, all lif ids are equal. */
                    local_in_lif = local_out_lif = global_lif_id;
                }
            } else { /* UPDATE */

                rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port->mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF,
                                                    &gport_hw_resources);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Don't support update regular LIF to wide or inverse*/
                if ((SOC_IS_JERICHO(unit)) && is_ingress) {
                    uint8 is_wide_entry;

                    rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, gport_hw_resources.local_in_lif, TRUE, &is_wide_entry, NULL));
                    BCMDNX_IF_ERR_EXIT(rv);

                    if(((mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_WIDE) && (is_wide_entry != BCM_DPP_AM_IN_LIF_FLAG_WIDE)) 
                        || ((mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_WIDE)==0 && (is_wide_entry == BCM_DPP_AM_IN_LIF_FLAG_WIDE))) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Don't support update regular LIF to wide or inverse")));
                    }
                }

                old_mpls_port_local_out_lif = gport_hw_resources.local_out_lif;

                /* Get the local lifs from the global lif */
                if (SOC_IS_JERICHO(unit)) {
                    local_in_lif = gport_hw_resources.local_in_lif;
                    local_out_lif = gport_hw_resources.local_out_lif;
                    old_mpls_port_local_out_lif = local_out_lif = gport_hw_resources.local_out_lif;
                    global_lif_id = (is_ingress) ? gport_hw_resources.global_in_lif : gport_hw_resources.global_out_lif;
                } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                    local_in_lif = local_out_lif = global_lif_id;
                }

                if (((mpls_port->flags & BCM_MPLS_PORT_COUNTED) || (mp) || (is_1_to_1_protected) || (is_1_plus_1) || (is_egress_protection) || (!BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))) && is_egress) {
                    eep_required = 1;

                    if (SOC_IS_JERICHO(unit)) {
                        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif_id);
                        if (rv != BCM_E_EXISTS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress lif to be replaced %d does not exist"), global_lif_id));
                        }
                    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                        rv = bcm_dpp_am_mpls_eep_is_alloced(unit, local_out_lif);
                        if (rv != BCM_E_EXISTS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress lif to be replaced %d does not exist"), local_out_lif));
                        }
                    }
                }

            }
        }



        /* learning gport */
        /* In case egress - allocate for eedb, in case of ingress - allocate for learning */
        if (mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID) {
            /* PUSH PROFILE template management. allocate in case local (for EEP) or FEC+VC (learn data) */
            if (!remote_lif) /* Tunnel protected */ {
                BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_push_profile_alloc(unit,
                                                                       &push_profile,
                                                                       pwe_info.has_cw,
                                                                       &mpls_port->egress_label,
                                                                       FALSE,
                                                                       FALSE,
                                                                       FALSE,
                                                                       TRUE,
                                                                       NULL,
                                                                       0));                                                                       
                if (update) {
                    /* Check whether outlif is allocated for this mpls port */
                    /* o_encap_id = BCM_ENCAP_ID_GET(mpls_port->encap_id);*/
                    rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port->mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                        &gport_hw_resources);
                    BCM_IF_ERROR_RETURN(rv);

                    /* If outlif is not allocated for this mpls port, fetch push profile from SW DB */
                    if (gport_hw_resources.local_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
                        rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                           (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                                           (sw_state_htb_data_t) & temp_phy_port, FALSE);
                        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port, rv)) {
                            return BCM_E_INTERNAL;
                        }
                        if (temp_phy_port.type == _bcmDppGportResolveTypeFecVC) {
                            /* In case of FEC_VC push profile is allocated */
                            old_pwe_info->push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(temp_phy_port.encap_id);
                        }

                    }
                    /* free old push profile */
                    rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                                     old_pwe_info->push_profile,
                                                                     &is_last);
                    BCMDNX_IF_ERR_EXIT(rv); 
                    /* Free old push profile for the egress tunnel label, only in case it has been allocated.
                       This condition wasn't applied to the pwe label because given we do replace, the pwe must
                       have been created. Whereas, the egress tunnel label may not have been created yet.
                       The creation of the egress tunnel (if happened) was accompanied by a push profile
                       creation in _bcm_mpls_tunnel_bcm_to_ppd().*/
                    if (is_pwe_binded_with_mpls && !_BCM_DPP_MPLS_IS_FORBIDDEN_PUSH_PROFILE(old_pwe_info->egress_tunnel_label_info.tunnels[0].push_profile)) {
                        rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                                 old_pwe_info->egress_tunnel_label_info.tunnels[0].push_profile,
                                                                 &is_last);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
                if (!mp) {
                    /* Reset action in case of P2P and label is for learning */
                    mpls_port->egress_label.action = 0;
                }
            }
        }
    }
    
    /* 3. allocate FEC for protected PWE */
    if (is_1_to_1_protected) {
        /* if with id, then allocate in same place */
        if (with_id) {
            fec_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        }
        rv = _bcm_dpp_gport_l2_intf_calculate_act_fec(mpls_port, (int *)(&fec_id), BCM_GPORT_MPLS_PORT);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If port is the first port on fec, and it's not update, then create it. */
        if (!_BCM_PPD_GPORT_IS_WORKING_PORT(mpls_port)
            && !update) {
            fec_id = (fec_id >> 1) * 2; /* round down*/
            /* allocate 2 fec entries */
            rv = bcm_dpp_am_fec_alloc(unit, fec_flags, fec_alloc_usage, 2, &fec_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        rv = bcm_dpp_gport_mgmt_fec_remote_destination_set(unit, fec_id, remote_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (is_forward_group) {
        /* Allocating FEC for Destination + Out-LIF */
        if (!update) {
            if (with_id) {
                rv = _bcm_dpp_gport_l2_intf_get_global_lif_from_port_id(mpls_port, is_1_to_1_protected, is_mc_ingress, &fec_id, BCM_GPORT_MPLS_PORT);
                BCMDNX_IF_ERR_EXIT(rv);

                if (fec_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
                    fec_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                }
            }
            rv = bcm_dpp_am_fec_alloc(unit, fec_flags, fec_alloc_usage, 1, &fec_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    }

    /* resource allocation } */

    /* fill handles */

    /* 
     * encap_id = LIF = EEP (when alloced) 
     * mpls_port_id = protected? FEC : LIF. 
     */


    /* set encap if not given as input*/
    if (!(encap_with_id)) {
        mpls_port->encap_id = global_lif_id;
    }

    /* if update or match only don't change/calculate mpls_port_id*/
    if (!update) {
        /* set mpls_port_id */
        rv = _bcm_dpp_gport_l2_intf_calculate_port_id(unit, fec_id, global_lif_id, mpls_port, BCM_GPORT_MPLS_PORT);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* write to SW state */

    /* store needed information for local and remote*/
    if (_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id)) {
        if (is_mc_ingress) {
            if (update) {
                /* if update remove from hash, reusue same memory*/
                rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                   (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                                   (sw_state_htb_data_t) & phy_port, TRUE);
                if (GPORT_HASH_VLAN_NOT_FOUND(phy_port, rv)) {
                    return BCM_E_INTERNAL;
                }
            }
            sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
            phy_port.type = _bcmDppGportResolveTypeMC;
            phy_port.phy_gport = mpls_port->port;  /* physical port*/
            phy_port.encap_id = BCM_ENCAP_ID_GET(mpls_port->encap_id);
            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                                 (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                                 (sw_state_htb_data_t) & phy_port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "error(%s) Updating Gport Dbase (0x%x)\n"),
                           bcm_errmsg(rv), mpls_port->mpls_port_id));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            /* nothing to store, as information is stored in FEC */
        }
    } else if (mpls_port->failover_port_id != BCM_GPORT_TYPE_NONE) {
        /* If ingress only - learn FEC.
         * Else - nothing to store, as information is stored in FEC
         * Protected case, where need to configure only LIF,
         * in this case failover-id is invalid, but failover_port_id includes the FEC id to learn 
         */
        if (is_ingress && !is_egress) {
            if (update) {
                /* if update remove from hash, reusue same memory*/
                rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                                   (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                                   (sw_state_htb_data_t) & phy_port, TRUE);
                if (GPORT_HASH_VLAN_NOT_FOUND(phy_port, rv)) {
                    return BCM_E_INTERNAL;
                }
            }
            sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));

            if (BCM_GPORT_IS_FORWARD_PORT(mpls_port->failover_port_id)) {
                /* This fec id is used for the learning information */
                phy_port.type = _bcmDppGportResolveTypeForwardFec;
                phy_port.phy_gport = BCM_GPORT_FORWARD_PORT_GET(mpls_port->failover_port_id);
            } else {
                phy_port.type = _bcmDppGportResolveTypeProtectedFec;
                protect_fec_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
                /*save relevant FEC entry for each mpls port*/
                phy_port.phy_gport = protect_fec_id;
            }

            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                                (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                                (sw_state_htb_data_t) & phy_port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "error(%s) Updating Gport Dbase (0x%x)\n"),
                           bcm_errmsg(rv), mpls_port->mpls_port_id));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

    } else if (!remote_lif && is_ingress) {
        /* if not protected, then gport is VC and FEC for tunnel */
        if (update) {
            /* if update remove from hash, reusue same memory*/
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                                               (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                               (sw_state_htb_data_t) & phy_port, TRUE);
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port, rv)) {
                return BCM_E_INTERNAL;
            }
        }
        sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));

        if (BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if)) {
            phy_port.type = _bcmDppGportResolveTypePhyEEP;
            phy_port.phy_gport = mpls_port->port; /* physical port */
            phy_port.encap_id = BCM_ENCAP_ID_GET(mpls_port->encap_id); /* encap_id = out-PWE */
        } else {
            if ((mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP) || (mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP_EEI) || (mpls_port->flags & BCM_MPLS_PORT_COUNTED)) {
                phy_port.type = (mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP_EEI) ? _bcmDppGportResolveTypeFecEEI : _bcmDppGportResolveTypeFecEEP;
                phy_port.phy_gport = BCM_L3_ITF_VAL_GET(mpls_port->egress_tunnel_if);  /* MPLS FEC */
                phy_port.encap_id = BCM_ENCAP_ID_GET(mpls_port->encap_id); /* PWE Out-Lif */
            } else {
                phy_port.type = _bcmDppGportResolveTypeFecVC;
                phy_port.phy_gport = BCM_L3_ITF_VAL_GET(mpls_port->egress_tunnel_if);  /* FEC */
                BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(phy_port.encap_id, mpls_port->egress_label.label, push_profile); /* encap_id = VC */
            }
        }
        rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                             (sw_state_htb_key_t) & (mpls_port->mpls_port_id),
                                             (sw_state_htb_data_t) & phy_port);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "error(%s) Updating Gport Dbase (0x%x)\n"),
                       bcm_errmsg(rv), mpls_port->mpls_port_id));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Update FEC->GPORT SW DB */
    if (fec_id != -1) {
        rv = bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(unit, fec_id, mpls_port->mpls_port_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* write to HW */

    /* 
     * 1. encapsulation DB: to include PWE information for flooding protected PWE
     * 2. FEC for protection information
     * 3. LIF to include PWE termination in case of p2p default destination 
     */

    /* 1. fill EEP if required*/
    if (eep_required == 1 && !remote_lif && is_egress) {
        rv = _bcm_dpp_mpls_port_pwe_eep_set(unit, mpls_port, local_out_lif,
                                            push_profile, old_pwe_info, is_egress_protection, is_1_plus_1,is_pwe_binded_with_mpls);
        BCMDNX_IF_ERR_EXIT(rv);

    }

    /* 2. fill FEC */
    if (is_1_to_1_protected || (is_forward_group)) {
        rv = _bcm_dpp_l2_gport_fill_fec(unit, mpls_port, BCM_GPORT_MPLS_PORT, fec_id, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* 1+1 protection info */
    if (is_1_plus_1) {
        _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(pwe_info.protection_pointer, mpls_port->failover_id);
        pwe_info.protection_pass_value = _BCM_PPD_GPORT_IS_WORKING_PORT(mpls_port) ? 0 : 1;
    }

    /* ingress 1+1 & Egress 1:1 */
    if (_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->ingress_failover_id)) {
        _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(pwe_info.protection_pointer, mpls_port->ingress_failover_id);
        pwe_info.protection_pass_value = _BCM_PPD_GPORT_IS_INGRESS_WORKING_PORT(mpls_port) ? 0 : 1;
    }


    if (!remote_lif && is_ingress) {
        SOC_PPC_L2_LIF_PWE_INFO  pwe_info_old;
        uint8 found = 0;

        if (update) {
            soc_sand_rv = soc_ppd_l2_lif_pwe_get(unit, mpls_port_o.match_label, &local_in_lif, &pwe_additional_info, &pwe_info_old, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF entry not found")));
            }
            pwe_additional_info.vlan_domain = match_vlan_domain_n;

            /* Copy OAM LIF SET value */
            pwe_info.oam_valid = pwe_info_old.oam_valid;
         }
        /* Split horizon configuration */
        if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
            if (mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
                pwe_info.orientation = mpls_port->network_group_id & 1;
                profile_split_horizon_val = (mpls_port->network_group_id & 2) >> 1;
            } else {
                pwe_info.orientation = 0;
            }
            if (update) {
                pwe_info.lif_profile = pwe_info_old.lif_profile;
            }

            if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                            (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                             profile_split_horizon_val,&(pwe_info.lif_profile)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            pwe_info.orientation = (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        }

        /* 3. LIF for MP */
        /* In case of GAL P2P entry should be added */
        if (_BCM_DDP_IS_MP_VPN(vpn) && !is_gal) {
            pwe_info.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP;
            pwe_info.vsid = _BCM_DDP_VPN_TO_VSI(vpn);
            pwe_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
            /* learn-info*/

            /* learning is synchronized for current hw state when updating  */
            if (update) {
                pwe_info.learn_record.enable_learning = pwe_info_old.learn_record.enable_learning;
                pwe_info.tpid_profile_index = pwe_info_old.tpid_profile_index;
            } else {
                pwe_info.learn_record.enable_learning = TRUE;
            }


            if (SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile) {
                pwe_info.learn_record.enable_learning = FALSE;
            }

            /* termination profile handling */
            rv = _bcm_mpls_tunnel_lif_term_mpls_port_to_flags(unit, mpls_port, &inner_flags);
            BCMDNX_IF_ERR_EXIT(rv);

            BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                                                          local_in_lif,
                                                                          inner_flags,
                                                                          TRUE, /* is_pwe_lif */
                                                                          TRUE, /* Expect BOS (in Jericho) */
                                                                          &pwe_info.term_profile));

            if (_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id)) {
                /* 1+1 protected-PWE: learn MC group */
                if (is_1_plus_1) {
                    SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, &(pwe_info.learn_record.learn_info), _BCM_MULTICAST_ID_GET(mpls_port->failover_mc_group), soc_sand_rv);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                /* protected-PWE: learn FEC */
                else {
                    SOC_PPD_FRWRD_DECISION_PROTECTED_PWE_SET(unit, &(pwe_info.learn_record.learn_info), fec_id, soc_sand_rv);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                rv = _bcm_dpp_local_lif_sw_resources_learn_gport_set(unit, local_in_lif, mpls_port->mpls_port_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /* protected case, where need to configure only LIF,
               in this case failover-id is invalid, but failover_port_id includes the FEC id to learn */
            else if (mpls_port->failover_port_id != BCM_GPORT_TYPE_NONE) {
                rv = _bcm_dpp_gport_is_protected(unit, mpls_port->failover_port_id, &is_failover_port_valid);
                BCMDNX_IF_ERR_EXIT(rv);
                if (!is_failover_port_valid) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("failover port has to be FEC")));
                }

                if (BCM_GPORT_IS_FORWARD_PORT(mpls_port->failover_port_id)) {
                    /* This fec id is used for the learning information */
                    fec_id = BCM_GPORT_FORWARD_PORT_GET(mpls_port->failover_port_id);
                } else {
                    protect_fec_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
                    fec_id = _BCM_PPD_GPORT_FEC_TO_WORK_FEC(protect_fec_id);
                }
                
                /*Learn SysInlif+FEC*/
                if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,  "conn_to_np_enable", 0) &&
                   (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY)){
                    SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET(unit, &(pwe_info.learn_record.learn_info), fec_id, global_lif_id, soc_sand_rv);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else {
                    SOC_PPD_FRWRD_DECISION_PROTECTED_PWE_SET(unit, &(pwe_info.learn_record.learn_info), fec_id, soc_sand_rv);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                rv = _bcm_dpp_local_lif_sw_resources_learn_gport_set(unit, local_in_lif, mpls_port->failover_port_id);
                BCMDNX_IF_ERR_EXIT(rv);
          }
          /* protected-tunnel: learn FEC + VC */
          else {
              if (BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if)) {
                /* learn EEP + Dest */
                SOC_PPD_FRWRD_DECISION_PWE_SET(unit, &(pwe_info.learn_record.learn_info), global_lif_id, is_lag, sys_port, soc_sand_rv);
              } else {
                if (mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP) {
                    /* Learn encap */
                    SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_OUTLIF_SET(unit, &(pwe_info.learn_record.learn_info), BCM_L3_ITF_VAL_GET(mpls_port->egress_tunnel_if), mpls_port->encap_id, soc_sand_rv);
                } else if (mpls_port->flags2 & BCM_MPLS_PORT2_LEARN_ENCAP_EEI) {
                    SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_WITH_EEI_SET(unit, &(pwe_info.learn_record.learn_info), BCM_L3_ITF_VAL_GET(mpls_port->egress_tunnel_if), mpls_port->encap_id, soc_sand_rv);
                }
                else {
                    /* allocate push-profile according to tunnel information */  
                    SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, &(pwe_info.learn_record.learn_info), mpls_port->egress_label.label, push_profile, BCM_L3_ITF_VAL_GET(mpls_port->egress_tunnel_if), soc_sand_rv);
                }
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
              rv = _bcm_dpp_local_lif_sw_resources_learn_gport_set(unit, local_in_lif, mpls_port->mpls_port_id);
              BCMDNX_IF_ERR_EXIT(rv);
          }                

          if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
              SOC_PPC_VC_LABEL_INDEXED_SET(in_vc_label,SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(mpls_port->match_label),SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(mpls_port->match_label));
          } else {
              in_vc_label = mpls_port->match_label;
          }

          pwe_info.global_lif_id = global_lif_id;
          
          /*create extension bank only when creating the mpls port*/
          pwe_info.is_extended = update?0:DPP_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_WIDE);

          soc_sand_rv = soc_ppd_l2_lif_pwe_add(unit, in_vc_label, local_in_lif, &pwe_additional_info, &pwe_info, &failure_indication);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          SOC_SAND_IF_FAIL_RETURN(failure_indication);

          /* store match cretiria used for this LIF */
          /* store gport this LIF is mapped to */
          rv = _bcm_dpp_in_lif_mpls_match_add(unit, mpls_port, local_in_lif, update ? (old_mpls_port_local_out_lif != -1) : eep_required);         
          BCMDNX_IF_ERR_EXIT(rv);

          /* store vsi */
          rv = _bcm_dpp_local_lif_sw_resources_vsi_set(unit, local_in_lif, vpn);
          BCMDNX_IF_ERR_EXIT(rv);
      }
      else /* p2p */ 
      {
          /*1. P2P attributes */
          pwe_info.vsid = _BCM_PPD_VSI_P2P_SERVICE;
          pwe_info.learn_record.enable_learning = FALSE;
          pwe_info.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;
          pwe_info.is_extended = update?0:DPP_IS_FLAG_SET(mpls_port->flags2, BCM_MPLS_PORT2_INGRESS_WIDE);
          /* Enough to check only one of them (TTL/EXP since in push profile alloc we checks both) */
          if (mpls_port->egress_label.flags & (BCM_MPLS_EGRESS_LABEL_TTL_COPY)) {
              /* uniform: copy TTL and EXP, and always dec TTL */
              pwe_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
          } 
          else {
              pwe_info.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
          }

          /* termination profile handling */
          rv = _bcm_mpls_tunnel_lif_term_mpls_port_to_flags(unit, mpls_port, &inner_flags);
          BCMDNX_IF_ERR_EXIT(rv);

          BCMDNX_IF_ERR_EXIT(_bcm_mpls_tunnel_lif_term_profile_exchange(unit,
                                          local_in_lif,
                                          inner_flags,
                                          TRUE, /* is_pwe_lif*/
                                          TRUE, /* Expect BOS (in Jericho) */
                                          &pwe_info.term_profile));
          
          if (is_gal) {
              /* In case of GAL we add to the lif table new entry with the same lif id as the original PWE entry */
              pwe_info.has_gal = 1;
              pwe_info.lif_table_entry_in_lif_field = gport_hw_resources.local_in_lif;
          }


          /*2. set default destination */
          /* if peer not ready, then set destiantion to drop, and save my VC*/
          /* considere my peer not exist */
          if (!update 
              || (pwe_info_old.default_frwrd.default_forwarding.type == SOC_PPC_FRWRD_DECISION_TYPE_DROP)) {
              pwe_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF; 
              SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &pwe_info.default_frwrd.default_forwarding, soc_sand_rv);
              BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          } else {
              sal_memcpy(&pwe_info.default_frwrd,&pwe_info_old.default_frwrd,sizeof(SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO));
          }
      
          /* set flags */
  
          /* call PPD */
  
  
          /* add pwe  */
          if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
              SOC_PPC_VC_LABEL_INDEXED_SET(in_vc_label,SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(mpls_port->match_label),SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(mpls_port->match_label));
          } else {
              in_vc_label = mpls_port->match_label;
          }
          pwe_info.global_lif_id = global_lif_id;
          soc_sand_rv = soc_ppd_l2_lif_pwe_add(unit, in_vc_label, local_in_lif, &pwe_additional_info, &pwe_info, &failure_indication);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          SOC_SAND_IF_FAIL_RETURN(failure_indication);

          /* store match cretiria used for this LIF */
          rv = _bcm_dpp_in_lif_mpls_match_add(unit,mpls_port,local_in_lif, update ? (old_mpls_port_local_out_lif != -1) : eep_required);
          BCMDNX_IF_ERR_EXIT(rv);

          /* store vsi */
          rv = _bcm_dpp_local_lif_sw_resources_vsi_set(unit, local_in_lif, vpn);
          BCMDNX_IF_ERR_EXIT(rv);              
      }
    }

    /* for update remove old hw resources */
    if(update && !remote_lif && is_ingress) {
        /* if key is different remove old key*/
        if((mpls_port->match_label != mpls_port_o.match_label) ||
             (update_vlan_domain)){

            if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
                SOC_PPC_VC_LABEL_INDEXED_SET(in_vc_label,SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(mpls_port_o.match_label),SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(mpls_port_o.match_label));
            } else {
                in_vc_label = mpls_port_o.match_label;
            }
            if (update_vlan_domain) {
                 pwe_additional_info.vlan_domain = match_vlan_domain_o;
            }
            soc_sand_rv = soc_ppd_l2_lif_pwe_remove(unit, in_vc_label, TRUE/*remove from SEM*/, is_gal, &local_in_lif_o, &pwe_additional_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* yet if use same LIF, re-add new key, to remove side affects*/
            if(local_in_lif_o == local_in_lif) {
                if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
                    SOC_PPC_VC_LABEL_INDEXED_SET(in_vc_label,SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(mpls_port->match_label),SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(mpls_port->match_label));
                } else {
                    in_vc_label = mpls_port->match_label;
                }
	        if (update_vlan_domain) {
	            pwe_additional_info.vlan_domain = match_vlan_domain_n;
	        }
                soc_sand_rv = soc_ppd_l2_lif_pwe_add(unit, in_vc_label, local_in_lif, &pwe_additional_info, &pwe_info, &failure_indication);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                SOC_SAND_IF_FAIL_EXIT(failure_indication);
            }
        

            if(local_in_lif_o != local_in_lif) {
                rv = _bcm_dpp_local_lif_sw_resources_delete(unit, local_in_lif_o, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
                BCMDNX_IF_ERR_EXIT(rv); 
            }
        }
    }
    BCM_EXIT;
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_mpls_port_add(
    int unit,
    bcm_vpn_t   vpn,
    bcm_mpls_port_t *   mpls_port)
{
    int rv = BCM_E_NONE;
    uint32 local_port;
    uint32 soc_sand_rv;
    SOC_PPC_PORT_INFO port_info;
    int update = 0, with_id = 0;
    bcm_mpls_port_t mpls_port_o;
   _bcm_lif_type_e used_type;    
    SOC_PPC_EG_ENCAP_ENTRY_INFO
            eep_encap_info_o[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    _bcm_dpp_gport_info_t gport_info;    
    uint32  pp_port;
    int     core;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int status;

    BCMDNX_INIT_FUNC_DEFS;

    /* Read the state of the flags */
    update = (mpls_port->flags & BCM_MPLS_PORT_REPLACE) ? TRUE : FALSE;
    with_id = (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) ? TRUE : FALSE;

    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(&eep_encap_info_o[0]);

    /* unsupported criteria */
    if ((mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_VLAN) ||
        (mpls_port->criteria == BCM_MPLS_PORT_MATCH_VLAN_PRI) ||
        (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT) ||
        (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN_STACKED)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported Match Criteria")));
    }

    if (mpls_port->match_label != 0 && mpls_port->criteria == BCM_MPLS_PORT_MATCH_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Valid match label cannot be configured if match criteria is invalid.")));
    }

    if ((mpls_port->criteria == BCM_MPLS_PORT_MATCH_LABEL_PORT)&&
          !soc_property_get(unit, spn_PWE_TERMINATION_PORT_MODE_ENABLE, 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported Match Criteria:BCM_MPLS_PORT_MATCH_LABEL_PORT")));        
    }

    if (!SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
        if ( (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_INNER_VLAN) || 
             (mpls_port->criteria == BCM_MPLS_PORT_MATCH_PORT_VLAN) ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported Match Criteria")));
        }
    }

    if (mpls_port->egress_tunnel_label.flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID) {
        if (!soc_property_get(unit, spn_MPLS_BIND_PWE_WITH_MPLS_ONE_CALL, 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
            (_BSL_BCM_MSG("Soc property mpls_bind_pwe_with_mpls_one_call must be set in order to create a pwe binded with mpls entry from bcm_mpls_port_add.")));
        }

        if (mpls_port->egress_tunnel_label.exp > 7) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("EXP value %d in mpls tunnel of the pwe entry is out of range"), mpls_port->egress_tunnel_label.exp));
        }
        if (!_BCM_DPP_MPLS_LABEL_IN_RANGE(mpls_port->egress_tunnel_label.label)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Label value is out of range in mpls tunnel of the pwe entry")));
        }
    }

    /* verify paramters for core-side */
    if(mpls_port->exp_map != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("for CORE-side ex-map has to be zero"))); 
    }
    if(!(mpls_port->flags & BCM_MPLS_PORT_EGRESS_TUNNEL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Flag BCM_MPLS_PORT_EGRESS_TUNNEL unset is not supported"))); /* currently not implemented/supported */
    }

    if (mpls_port->flags2 & (BCM_MPLS_PORT2_LEARN_ENCAP|BCM_MPLS_PORT2_LEARN_ENCAP_EEI)) {
        if (mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_PORT2_LEARN_ENCAP flag can not be set when configuring egress properties using BCM_MPLS_PORT2_EGRESS_ONLY"))); 
        }
        if (!BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_PORT2_LEARN_ENCAP flag is supported only when mpls_port->egress_tunnel_if is FEC"))); 
        }

        if (_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id) || (mpls_port->failover_port_id != BCM_GPORT_TYPE_NONE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_PORT2_LEARN_ENCAP flag is supported only when mpls port is not protected"))); 
        }

        /* BCM_MPLS_PORT2_LEARN_ENCAP means PWE outlif creation, so in ingress only mode we learn the outlif. No need for egress_label info */
        if ((mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) && (mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When using BCM_MPLS_PORT2_LEARN_ENCAP learning info in ingress only, we learn the encap id, not the VC label. So mpls_port->egress_label should be empty."))); 
        }
    }

    if ((SOC_IS_JERICHO(unit) && !soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) && (mpls_port->network_group_id > 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mpls network group id must be 0 when soc property split_horizon_forwarding_groups_mode is unset"))); 
    }

    if (!(mpls_port->flags & BCM_MPLS_PORT_NETWORK) && (mpls_port->network_group_id > 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mpls network group id must be 0 when BCM_MPLS_PORT_NETWORK flag is unset"))); 
    }

    if ((SOC_IS_JERICHO(unit) &&
         soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) &&
         soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) <= 2) &&
        (mpls_port->flags & BCM_MPLS_PORT_NETWORK) && (mpls_port->network_group_id == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
           "mpls network group id can't be 0 when BCM_MPLS_PORT_NETWORK flag and soc property split_horizon_forwarding_groups_mode are set"))); 
    }

    if ((SOC_IS_JERICHO(unit) &&
        soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 3) &&
        (mpls_port->network_group_id > 1)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
           "mpls network group id can't be  > 1 when soc property split_horizon_forwarding_groups_mode is 3"))); 
    }

    if (update || with_id) {
        /* Get the gport usage state */
        rv = _bcm_dpp_lif_sw_resources_gport_usage_get(unit, mpls_port->mpls_port_id, &used_type);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_PORT_REPLACE can't be used without BCM_MPLS_PORT_WITH_ID as well")));
        }
        else if (used_type == _bcmDppLifTypeAny) {
            /* In case of FORWARD GROUP and PROTECTION we might not have lif */
            if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port->mpls_port_id) || BCM_GPORT_SUB_TYPE_IS_PROTECTION(mpls_port->mpls_port_id)) {

                /* get LIF + FEC used for this gport */
                rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port->mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_FEC, &gport_hw_resources);
                BCMDNX_IF_ERR_EXIT(rv);

                if (gport_hw_resources.fec_id == -1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_MPLS_PORT_REPLACE, the mpls_port_id must exist")));
                }

                /* Check if fec is allocated */
                if (gport_hw_resources.fec_id != -1) {
                    status = bcm_dpp_am_fec_is_alloced(unit, 0 /*flags*/, 1 /*size*/, gport_hw_resources.fec_id);
                    if (status == BCM_E_NOT_FOUND) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_MPLS_PORT_REPLACE, the mpls_port_id must exist")));
                    }
                    else if (status != BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("MPLS PORT internal error in bcm_dpp_am_fec_is_alloced")));
                    }
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_MPLS_PORT_REPLACE, the mpls_port_id must exist")));
            }
        }
        else if (used_type != _bcmDppLifTypeMplsPort) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The mpls_port_id already exists as non MPLS_PORT type")));
        }
        else {
            bcm_mpls_label_t old_egress_vc = 0;
            uint32 old_push_profile = 0;
            /* Get the current mpls_port entry */
            bcm_mpls_port_t_init(&mpls_port_o);
            mpls_port_o.mpls_port_id = mpls_port->mpls_port_id;

            bcm_petra_mpls_port_get(unit,vpn, &mpls_port_o);

            /* Check whether outlif is allocated for this mpls port*/            
            rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port_o.mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, 
                                               &gport_hw_resources);
            BCM_IF_ERROR_RETURN(rv);            

            /* Fetch outlif only if it is allocated for this mpls port */
            if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
                soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,
                                     SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP, gport_hw_resources.local_out_lif, 0,
                                     eep_encap_info_o, next_eep, &nof_entries);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                

                if (eep_encap_info_o[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Encapsulation entry type must be PWE")));  /* has to be pwe */
                }

                old_push_profile = eep_encap_info_o[0].entry_val.pwe_info.push_profile;
                old_egress_vc = eep_encap_info_o[0].entry_val.pwe_info.label;
                
            } else{
                _BCM_GPORT_PHY_PORT_INFO phy_port;
                rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &(mpls_port->mpls_port_id), \
                                    (sw_state_htb_data_t) &phy_port,FALSE);    
                if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                    return BCM_E_INTERNAL;
                }
                if (phy_port.type == _bcmDppGportResolveTypeFecVC) {
                        /* In case of FEC_VC push profile is allocated */
                        old_push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(phy_port.encap_id); 
                        old_egress_vc = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(phy_port.encap_id) ;    
                }
            }
            /* check replace is possible, i.e. update attribute is possible */
            rv = _bcm_gport_mpls_is_valid_replace(unit, mpls_port, &mpls_port_o, old_egress_vc, old_push_profile);
            BCM_IF_ERROR_RETURN(rv);
        }	
        
    }
    /* If mpls port ingress is configured need to check that it wasn't configured previously */
    else if ((mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) || (!(mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY))) {
        if (with_id && (used_type != _bcmDppLifTypeAny) && !(mpls_port->flags & BCM_MPLS_PORT_WITH_GAL)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case mpls_port_id already exists, BCM_MPLS_PORT_REPLACE should be used")));
        }
    }

   /*
    * is local or remote configuration
    */
    rv = _bcm_dpp_gport_to_phy_port(unit, mpls_port->port ,0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);             
    
    /* configure the used port to be metro port */
    SOC_PPC_PORT_INFO_clear(&port_info);    
        
    /* get port info */
    BCM_PBMP_ITER(gport_info.pbmp_local_ports,local_port) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));

        /* set port attributes to act as metro port */
        soc_sand_rv = soc_ppd_port_info_get(unit,core, pp_port,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* if this MP enable learning in port, if it's p2p ignore will be set per lif */
        if(_BCM_DDP_IS_MP_VPN(vpn)) {
            port_info.enable_learning = TRUE;
            if (SOC_DPP_CONFIG(unit)->pp.ivl_inlif_profile) {
                port_info.enable_learning = FALSE;
            }
            /* if mpls side has no affect*/
            /* for non-CEP port update port to learn AC */
            if (port_info.port_type != SOC_SAND_PP_PORT_L2_TYPE_CEP) {
                port_info.is_learn_ac = TRUE;
            }
        }
        port_info.orientation = (mpls_port->flags & BCM_MPLS_PORT_NETWORK)? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        /* if access side then vlan-domain = local-port */
        /* don't change vlan-domain user need to call port_class set */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));
        port_info.flags = (SOC_PPC_PORT_IHB_PINFO_FLP_TBL | 
                           SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL | 
                           SOC_PPC_PORT_EGQ_PP_PPCT_TBL);
        soc_sand_rv = soc_ppd_port_info_set(unit,core,pp_port,&port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    }
    
    if(!_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id) && (mpls_port->failover_port_id == BCM_GPORT_TYPE_NONE) && 
       !(mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) && !BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (vpn != 0)) {
        /* protected-tunnel: learn FEC + VC, port must be set for learning */
        if (mpls_port->port == BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case of protected tunnel port property must be set")));
        }
    }

    /* GAL Validity check */
    if (mpls_port->flags & BCM_MPLS_PORT_WITH_GAL) {
        if (!SOC_DPP_CONFIG(unit)->pp.gal_support) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("GAL disabled on the device. To enable set soc property mpls_termination_pwe_vccv_type4_mode.")));
        }
        if (!SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("GAL supported only in case of label index mode. To enable set soc property mpls_termination_label_index_enable.")));
        }
        if (SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(mpls_port->match_label) != 2) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("GAL supported only on label index 2.")));
        }
        if (!(mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Setting flag BCM_MPLS_PORT_WITH_GAL configures only ingress side. BCM_MPLS_PORT2_INGRESS_ONLY must be set as well.")));
        }
        if (!(mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Setting flag BCM_MPLS_PORT_WITH_GAL configures GAL entry with control word. BCM_MPLS_PORT_CONTROL_WORD must be set as well.")));
        }
        if (!BCM_GPORT_IS_MPLS_PORT(mpls_port->tunnel_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Setting flag BCM_MPLS_PORT_WITH_GAL configures GAL entry with control word. tunnel_id field should contain existing PWE lif.")));
        }       
    }

    /* Separate calls */
    if ((mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) || (mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY)) {
        if ((mpls_port->flags & BCM_MPLS_PORT_WITH_ID) == 0 && vpn !=0 ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For multi point application Setting flags of BCM_MPLS_PORT2_EGRESS_ONLY/INGRESS_ONLY, is supported only with BCM_MPLS_PORT_WITH_ID flag set")));
        }
        if (_BCM_PPD_IS_VALID_FAILOVER_ID(mpls_port->failover_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MPLS_PORT2_EGRESS_ONLY/INGRESS_ONLY is not valid for protection configuration")));
        }
    }

    
    if (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) {
        if (!BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (mpls_port->egress_tunnel_if != 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring ingress using BCM_MPLS_PORT2_INRESS_ONLY, egress_tunnel_if must be fec")));
        }
        if (mpls_port->criteria == BCM_MPLS_PORT_MATCH_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported Match Creteria")));
        }
    }
    if (mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) {
        if (_BCM_DPP_VPN_IS_SET(vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring egress using BCM_MPLS_PORT2_EGRESS_ONLY, vpn parameter must be unset")));
        }
        if (mpls_port->failover_id != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring egress using BCM_MPLS_PORT2_EGRESS_ONLY, failover_id must be invalid")));
        }
        if (mpls_port->match_label != BCM_MPLS_PORT_MATCH_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring egress using BCM_MPLS_PORT2_EGRESS_ONLY, match_label must be invalid")));
        }
        if (_BCM_MULTICAST_IS_SET(mpls_port->failover_mc_group)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring egress using BCM_MPLS_PORT2_EGRESS_ONLY, failover_mc_group can not be set")));
        }
        if (!((mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) == BCM_MPLS_PORT_ENCAP_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring egress using BCM_MPLS_PORT2_EGRESS_ONLY, can not allocate without encap_id")));
        }
        if ((!BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if)) && 
            (!SOC_IS_JERICHO(unit) || (mpls_port->egress_tunnel_if!=0) )) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring egress using BCM_MPLS_PORT2_EGRESS_ONLY, egress_tunnel_if must be encap id")));
        }
        if (mpls_port->criteria != BCM_MPLS_PORT_MATCH_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported Match Creteria")));
        }

    }
    if (mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) {
        if (_BCM_DPP_VPN_IS_SET(vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, vpn parameter must be unset")));
        }
        if (!BCM_ENCAP_REMOTE_GET(mpls_port->encap_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, encap_id must be remote")));
        }
        if (mpls_port->match_label != BCM_MPLS_PORT_MATCH_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, match_label must be invalid")));
        }
        if (_BCM_MULTICAST_IS_SET(mpls_port->failover_mc_group)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, failover_mc_group can not be set")));
        }
        if (mpls_port->egress_label.label != BCM_MPLS_LABEL_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, egress_label can not be used")));
        }
        if (mpls_port->criteria != BCM_MPLS_PORT_MATCH_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported Match Creteria")));
        }
        if (mpls_port->port == BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, port must be valid")));
        }
        if (!(BCM_ENCAP_REMOTE_GET(mpls_port->encap_id))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When configuring fec using BCM_MPLS_PORT_FORWARD_GROUP, encap_id must be remote")));
        }
    }
    if (((mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) && ((mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) || (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY))) ||
        ((mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) && (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only one of BCM_MPLS_PORT2_EGRESS_ONLY/BCM_MPLS_PORT2_INGRESS_ONLY/BCM_MPLS_PORT_FORWARD_GROUP flags can be set")));
    }

    /* PWE configurations */
    rv = _bcm_dpp_mpls_port_pwe_set(unit,vpn,mpls_port, &eep_encap_info_o[0].entry_val.pwe_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

