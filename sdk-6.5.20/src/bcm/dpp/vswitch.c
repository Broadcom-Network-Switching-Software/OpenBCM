/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_SWITCH
#include <shared/bsl.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/mpls.h>

#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm/types.h>
#include <bcm/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/switch.h>

#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_port.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dcmn/dcmn_wb.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/ARAD/arad_sw_db.h>

#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
/*
 * MACROs
 */
#define VSWITCH_ACCESS   sw_state_access[unit].dpp.bcm.vswitch

#define SHR_BITCOPY_RANGE_VSI_USAGE_GET(_dst, _dst_start, _vsi_start, _nof_bits)\
        BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.vsi_usage.bit_range_read(unit, _vsi_start, _dst_start, _nof_bits, _dst))

#define SHR_BITCOPY_RANGE_VSI_USAGE_SET(_src, _src_start, _vsi_start, _nof_bits)\
        BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.vsi_usage.bit_range_write(unit, _vsi_start, _src_start, _nof_bits, _src))



/*********** SW DBs translation functions *******************/

/*
 * Function:
 *      _bcm_dpp_in_lif_p2p_match_set
 * Purpose:
 *      Set any required p2p cross-connect information in the In-LIF SW DB:
 *      1. Indication whether the In-LIF is p2p cross-connected.
 *      2. If System Resource mode isn't Advanced, the peer p2p cross-connect
 *         gport is stored.
 * Parameters:
 *      unit - (IN) device Number
 *      in_lif - (IN) lif_index to retrieve match information
 *      is_p2p - (IN) Indication whether the In-LIF is p2p cross-connected
 *      peer_gport - (IN) peer_gport_id (usually lif/fec)
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_p2p_match_set(int unit, int in_lif, int is_p2p, int peer_gport)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Store an indication whether the In-LIF is p2p cross-connected */
    if (is_p2p) {
        gport_sw_resources.in_lif_sw_resources.additional_flags |= _BCM_DPP_INFLIF_MATCH_INFO_P2P;
    } else {
        gport_sw_resources.in_lif_sw_resources.additional_flags &= (~_BCM_DPP_INFLIF_MATCH_INFO_P2P);
    }

    /* Store the peer gport when the System Resource isn't in Advanced mode */
    if (!SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(unit)) {
        gport_sw_resources.in_lif_sw_resources.peer_gport = (is_p2p) ? peer_gport : 0;
    }

    /* Sync the entry updated values */
    rv = _bcm_dpp_local_lif_sw_resources_set(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_in_lif_p2p_match_get
 * Purpose:
 *      Get any required p2p cross-connect information from the In-LIF SW DB:
 *      1. Indication whether the In-LIF is p2p cross-connected.
 *      2. If System Resource mode isn't Advanced, the peer p2p cross-connect
 *         gport is retrieved here.
 * Parameters:
 *      unit - (IN) device Number
 *      in_lif - (IN) lif_index to retrieve match information
 *      is_p2p - (OUT) Indication whether the In-LIF is p2p cross-connected
 *      peer_gport - (OUT) peer_gport_id (usually lif/fec)
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_p2p_match_get(int unit, int in_lif, int *is_p2p, int *peer_gport)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    *is_p2p = (gport_sw_resources.in_lif_sw_resources.additional_flags & _BCM_DPP_INFLIF_MATCH_INFO_P2P) ? 1 : 0;
    *peer_gport = gport_sw_resources.in_lif_sw_resources.peer_gport;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/* given LIF info return of this LIF is cross connected */
/* return 0, 1*/
/*
 * Function:
 *      _bcm_petra_is_cross_connect_lif
 * Purpose:
 *      Return whether an In-LIF is configured for cross-connect
 * Parameters:
 *      unit    -            (IN) Device Number
 *      local_in_lif_index - (IN) Local In-LIF index
 * Returns:
 *      1: The In-LIF is cross-connected
 *      0: The In-LIF isn't cross-connected
 */
STATIC int 
_bcm_petra_is_cross_connect_lif(int unit,
                                SOC_PPC_LIF_ID local_in_lif_index)
{
    int is_p2p, peer_gport;

    /* Get the p2p indication from the In-LIF SW DB */
    _bcm_dpp_in_lif_p2p_match_get(unit, local_in_lif_index, &is_p2p, &peer_gport);

    return is_p2p;
}



/* given LIF info return forwarding decision as pointer into this lif-info */

STATIC void 
_bcm_petra_lif_get_frwrd_decision(SOC_PPC_LIF_ENTRY_INFO *lif_info,SOC_PPC_FRWRD_DECISION_INFO **frwrd_decision){

    /* in PB, LIF destination cannot be drop so use trap-code */
    if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_PWE) {
        if (lif_info->value.pwe.service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P && 
            lif_info->value.pwe.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_TRAP && 
            lif_info->value.pwe.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
            *frwrd_decision = &(lif_info->value.pwe.default_frwrd.default_forwarding);
            return;
        }
    }
    else if(lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_ISID) {
        if (lif_info->value.isid.service_type == SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P && 
            lif_info->value.isid.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_TRAP && 
            lif_info->value.isid.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
            *frwrd_decision = &(lif_info->value.isid.default_frwrd.default_forwarding);
            return;
        }
    }
    else if(lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_AC ||
            lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP ||
            lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP) {
        if (lif_info->value.ac.service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP &&
            lif_info->value.ac.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_TRAP && 
            lif_info->value.ac.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
            *frwrd_decision = &(lif_info->value.ac.default_frwrd.default_forwarding);
            return;
        }
    }
    else if(lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
        if (lif_info->value.mpls_term_info.service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P &&
            lif_info->value.mpls_term_info.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_TRAP && 
            lif_info->value.mpls_term_info.default_frwrd.default_forwarding.type != SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
            *frwrd_decision = &(lif_info->value.mpls_term_info.default_frwrd.default_forwarding);
            return;
        }
    }

    return;
}

/* check if the given vpn is allocated as l2 VSWITCH VPN */

int _bcm_dpp_vswitch_vsi_usage_get(
    int unit,
    int vsi,
    _bcm_vsi_type_e * vswitch_usage)
{
    uint32 vswitch_usage_lcl[1] = {0};

    BCMDNX_INIT_FUNC_DEFS;

    /* not exist VSI */
    if (vsi >= _BCM_GPORT_NOF_VSIS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI value is higher than chip resources")));
    }

    SHR_BITCOPY_RANGE_VSI_USAGE_GET(vswitch_usage_lcl, 0, vsi * _BCM_DPP_VSI_TYPE_NOF_BITS, _BCM_DPP_VSI_TYPE_NOF_BITS);

    *vswitch_usage = (_bcm_vsi_type_e)vswitch_usage_lcl[0];

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}



/* check if the given vpn is allocated as l2 VSWITCH VPN */

int _bcm_dpp_vswitch_vsi_usage_set(
    int unit,
    int vsi,
    _bcm_vsi_type_e vswitch_usage)
{
    /* int tmp = 0;*/

    BCMDNX_INIT_FUNC_DEFS;

    /* not exist VSI */
    if (vsi >= _BCM_GPORT_NOF_VSIS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI out of range")));
    }

    SHR_BITCOPY_RANGE_VSI_USAGE_SET(&vswitch_usage, 0, vsi*_BCM_DPP_VSI_TYPE_NOF_BITS, _BCM_DPP_VSI_TYPE_NOF_BITS);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_vswitch_vsi_usage_type_set(int unit, int	vsi, _bcm_vsi_type_e type, uint8 used)
{
    uint32 tmp = 0, bit = 0;

    BCMDNX_INIT_FUNC_DEFS;

    tmp = used;
    /* not exist VSI */
    if (vsi >= _BCM_GPORT_NOF_VSIS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI out of range")));
    }

    bit = soc_sand_log2_round_down(type);

    SHR_BITCOPY_RANGE_VSI_USAGE_SET(&tmp, 0, vsi*_BCM_DPP_VSI_TYPE_NOF_BITS+bit, 1);

exit:
    BCMDNX_FUNC_RETURN;
}




int _bcm_dpp_vswitch_vsi_usage_type_check(int unit, int	vsi, _bcm_vsi_type_e type, uint8 *used)
{
    uint32 tmp = 0, bit = 0;

    BCMDNX_INIT_FUNC_DEFS;

    tmp = 0;
    /* not exist VSI */
    if (vsi >= _BCM_GPORT_NOF_VSIS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI out of range")));
    }

    bit = soc_sand_log2_round_down(type);
    SHR_BITCOPY_RANGE_VSI_USAGE_GET(&tmp, 0, vsi*_BCM_DPP_VSI_TYPE_NOF_BITS+bit, 1);

    *used = (uint8) tmp;

exit:
    BCMDNX_FUNC_RETURN;
}

/* allocate VSI for specific usage
   - allocate VSI id if needed 
   - set the usage type. 
   - calling this API with different types over the same VSI-id should return ok. 
 */
int _bcm_dpp_vswitch_vsi_usage_alloc(int unit, int flags, _bcm_vsi_type_e type, bcm_vlan_t *vsi, uint8 *replaced)
{
    uint8 exist=0,used=0;
    uint8 replaced_val=0;
    int rv;
    int mc_support;

    BCMDNX_INIT_FUNC_DEFS;


    /* 
     * ERSPAN multicast: uses the same resources as MIM, L2GRE, VXLAN VPN types (ISID table).
     */
    mc_support = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "erspan_mc_support", 0) ;

    if (mc_support) {
      if ((type == _bcmDppVsiTypeMim ||
         type == _bcmDppVsiTypeL2gre ||
         type == _bcmDppVsiTypeVxlan)) {

          if (!(flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI must be WITH-ID when creating VPN of types MIM L2GRE VXLAN in case ERSPAN multicast is enabled")));
          }

          if (((*vsi & 0xFFF) < 16)) /* only 12 lsb matter */{
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI out of range, VSI 12 lsbs must be >= 16 in case ERSPAN multicast is enabled")));
          }
      }                 
    }
      
      
      /* not with ID, just alloc and set type*/
    if (!(flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID)) {
        /* allocate VSI */
        rv = bcm_dpp_am_l2_vpn_vsi_alloc(unit,flags,vsi);
        BCMDNX_IF_ERR_EXIT(rv);
        /* set type */
        rv = _bcm_dpp_vswitch_vsi_usage_type_set(unit,*vsi,type,1);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* not exist VSI */
    if (*vsi >= _BCM_GPORT_NOF_VSIS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI out of range")));
    }

    /* if already allocated */
    rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, *vsi);
    if (rv == BCM_E_EXISTS) {
      exist = 1;
    }
    else if (rv == BCM_E_NOT_FOUND) {
      exist = 0;
    }
    else{
        BCMDNX_IF_ERR_EXIT(rv); /* check other errors */
    }
    rv = BCM_E_NONE; /* reset error */

    if (!exist) { /* not exist alloc and set type */
        /* allocate VSI */
        rv = bcm_dpp_am_l2_vpn_vsi_alloc(unit,flags,vsi);
        BCMDNX_IF_ERR_EXIT(rv);
        /* set type */
        rv = _bcm_dpp_vswitch_vsi_usage_type_set(unit,*vsi,type,1);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* vsi already allocated: */
    /* check type usage */
    rv = _bcm_dpp_vswitch_vsi_usage_type_check(unit, *vsi, type, &used);
    BCMDNX_IF_ERR_EXIT(rv);

    /* already exist for this usage */
    if (used) {
        if (!(flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("allocate VSI with-ID and without replace, where VSI already allocated for this usage")));
        }
        else{
            /* replace do nothing */
            replaced_val = 1;
            BCM_EXIT;
        }
    }

    /* already allocated for another usage --> just set type */
    rv = _bcm_dpp_vswitch_vsi_usage_type_set(unit,*vsi,type,1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    /* set value only if pointer provided*/
    if (replaced) {
        *replaced = replaced_val;
    }

    BCMDNX_FUNC_RETURN;
}


/* dellocate VSI from specific usage
   - reset the usage type. 
   - dealloc if this was last usage of this VSI
 */
int _bcm_dpp_vswitch_vsi_usage_dealloc(int unit, _bcm_vsi_type_e type, bcm_vlan_t	vsi)
{
    uint32 usage;
    int exist;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* check if this type is really allocated for this type */
    exist = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit,vsi);
    if(exist != BCM_E_EXISTS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI %d is not allocated"),vsi));
    }

    rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vsi, &usage);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((usage & type) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI %d not allocated for this usage %08x"),vsi,type));
    }

    /* reset bit usage */
    usage = usage & (~type);

    rv = _bcm_dpp_vswitch_vsi_usage_set(unit,vsi,usage);
    BCMDNX_IF_ERR_EXIT(rv);

    /* now if there is no any usage for this VSI deallocate it */
    if (usage == _bcmDppVsiTypeAny) {
        rv = bcm_dpp_am_l2_vpn_vsi_dealloc(unit, vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * set VSI  configuration
 * currently: support same MC for UC/BC/MC 
 */
int
bcm_petra_vswitch_vsi_set(
    int    unit,
    bcm_vlan_t    vsi,
    _bcm_petra_vswitch_unknown_action_t  action,
    int is_clear)
{
    SOC_PPC_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    bcm_stg_t stg_default;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPC_VSI_INFO_clear(&vsi_info);
    soc_sand_rv = soc_ppd_vsi_info_get(unit, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    vsi_info.fid_profile_id                              = SOC_PPC_VSI_FID_IS_VSID;    
    vsi_info.default_forwarding.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
    vsi_info.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
    vsi_info.default_forwarding.dest_id = vsi;

    /* Get default stg, STG module must be initialized before */
    BCMDNX_IF_ERR_EXIT(bcm_petra_stg_default_get(unit,&stg_default));
    if ((stg_default >= 0) && (vsi < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers))) {
        if (is_clear) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_vlan_destroy(unit, stg_default, vsi));
        } else {
            BCMDNX_IF_ERR_EXIT(bcm_petra_stg_vlan_add(unit, stg_default, vsi));
        }
    }

    soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_vswitch_create(
    int    unit,
    bcm_vlan_t *    vsi)
{
    int rv = BCM_E_NONE;
    int flags=0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* created VSI as MP */

    /* resource allocation: VSI*/
    rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit,flags,_bcmDppVsiTypeVswitch,vsi,NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    /* config VSI: currently default action is drop */
    rv = bcm_petra_vswitch_vsi_set(unit,*vsi,_bcm_petra_vswitch_unknown_action_flood,FALSE);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vswitch_create_with_id(
    int    unit,
    bcm_vlan_t    vsi)
{
    int rv = BCM_E_NONE;
    int flags=BCM_DPP_AM_FLAG_ALLOC_WITH_ID;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* allocate VSI*/

    rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit,flags,_bcmDppVsiTypeVswitch,&vsi,NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    /* config VSI: currently default action is flood */
    rv = bcm_petra_vswitch_vsi_set(unit,vsi,_bcm_petra_vswitch_unknown_action_flood,FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vswitch_destroy(
    int    unit,
    bcm_vlan_t    vsi)
{
    int rv = BCM_E_NONE;
    _bcm_vsi_type_e usage;
    int is_detach_ports;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vsi, &usage);
    BCMDNX_IF_ERR_EXIT(rv);

    if (usage & _bcmDppVsiTypeVswitch) {

        /* Get the VSI detach mode to be applied in the destroy */
        rv =  _bcm_dpp_switch_control_get(unit, bcmSwitchVpnDetachPorts, &is_detach_ports);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_vswitch_destroy_internal(unit, vsi, _bcmDppVsiTypeVswitch, (is_detach_ports) ? TRUE : FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    if (usage & _bcmDppVsiTypeL2gre || usage & _bcmDppVsiTypeVxlan) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vsi 0x%08x is of l2gre/vxlan type, has to be destoryed with l2gre/vxlan API, unit %d"), vsi));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vsi 0x%08x wasn't is of vswitch type"), vsi));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* destroy vswitch and delete all ports
   difference from bcm_petra_vswitch_destroy,
   that bcm_petra_vswitch_destroy is only for vswitch created using bcm_petra_vswitch_create
 */
int
bcm_petra_vswitch_destroy_internal(
    int    unit,
    bcm_vlan_t    vsi,
    _bcm_vsi_type_e vsi_type,
    int is_port_delete)
{
    int rv = BCM_E_NONE;
    int new_learn_profile, old_learn_profile, learn_profile_first_appear, is_last, first_appear;
    uint32 soc_sand_rv = 0;
    SOC_PPC_VSI_INFO vsi_info;

    BCMDNX_INIT_FUNC_DEFS;

    /* check vsi exist */
    rv = _bcm_dpp_vlan_check(unit, vsi);

    if(BCM_E_NOT_FOUND == rv) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("vsi 0x%08x"), vsi));
    }

    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_l2_event_handle_profile_alloc(
                     unit, 0, vsi, BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER, BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER,
                     BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER, BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER, &new_learn_profile, &first_appear);

    /* Update the FID to the default learn profile */
    rv = _bcm_dpp_am_template_l2_learn_profile_exchange(unit, 0, vsi, -1, 0, 0, &old_learn_profile, &is_last, &new_learn_profile,&learn_profile_first_appear);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Clean the old profile now that it is not in use. Use default values */
    if (is_last && (old_learn_profile != new_learn_profile))
    {
        SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO      limit_info;

        /* Update fid-learn-profile to fid-aging profile */
        soc_sand_rv = soc_ppd_frwrd_mact_fid_aging_profile_set(unit,old_learn_profile, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Update fid-learn-profile to event-handle profile */
        soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_set(unit,old_learn_profile,0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set default limit */
        SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(&limit_info);
        limit_info.action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE;
        limit_info.nof_entries = 0;
        limit_info.is_limited = TRUE;
        soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_set(unit,old_learn_profile,&limit_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* Reset to fid_profile per vsi */
    soc_sand_rv = soc_ppd_vsi_info_get(unit, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    vsi_info.mac_learn_profile_id = new_learn_profile;
    soc_sand_rv = soc_ppd_vsi_info_set(unit, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* remove */
    rv = bcm_petra_vswitch_vsi_set(unit,vsi,_bcm_petra_vswitch_unknown_action_drop,TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Delete port association to the VSI according to a flag parameter.
       This flag isn't set by APIs that can remove more than one VSI as the
       ports traverse removal is done only once, prior to the VSI removal. */
    if (is_port_delete) {
        rv = bcm_petra_vswitch_port_delete_all(unit,vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_vswitch_vsi_usage_dealloc(unit, vsi_type, vsi);
    if (BCM_E_PARAM == rv) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vsi 0x%08x"), vsi));
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vswitch_detach(
    int    unit)
{
    uint8 is_allocated;
    int rv = BCM_E_NONE;

    int initialized;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.initialized.get(unit, &initialized));
    if (FALSE == initialized) {
        BCM_EXIT;
    }

    /* Don't change SW state during detach. It will either be reloaded during warmboot,
       or deleted as a whole. */
    if (!SOC_IS_DETACHING(unit)) { 
        /* Free reserved VSIs - P2P */
        if (bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, _BCM_PPD_VSI_P2P_SERVICE) == BCM_E_EXISTS)
        {
            rv = bcm_dpp_am_l2_vpn_vsi_dealloc(unit, _BCM_PPD_VSI_P2P_SERVICE);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Mark the state as uninitialized */
        BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.initialized.set(unit, FALSE));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vswitch_init(
    int    unit)
{
    uint8 is_allocated;
    bcm_error_t rv = BCM_E_NONE;
    int vsi_alloc_flags;
    bcm_vlan_t vsi_p2p_service;
     
     
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    /*
     * allocate resources
     */
    BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.alloc(unit));
        /* Allocate a few bits (_BCM_DPP_VSI_TYPE_NOF_BITS) per vsi in 32bit resolution */
        BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.vsi_usage.alloc_bitmap(unit, _BCM_GPORT_NOF_VSIS * _BCM_DPP_VSI_TYPE_NOF_BITS));
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Allocate reserved VSIs - P2P */
        vsi_alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        vsi_p2p_service = _BCM_PPD_VSI_P2P_SERVICE;
        if (bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vsi_p2p_service) == BCM_E_NOT_FOUND)
        {
            rv = bcm_dpp_am_l2_vpn_vsi_alloc(unit, vsi_alloc_flags, &vsi_p2p_service);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    
        rv = _bcm_dpp_vswitch_vsi_usage_type_set(unit, vsi_p2p_service, _bcmDppVsiTypeMpls,1);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Mark the state as initialized */
    BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.initialized.set(unit, TRUE));
	/* Mark the state as initialized */
    BCMDNX_IF_ERR_EXIT(VSWITCH_ACCESS.vpn_detach_ports.set(unit, 1));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vswitch_port_add(
    int    unit,
    bcm_vlan_t    vsi,
    bcm_gport_t    port)
{
    SOC_SAND_PP_DESTINATION_ID dest_id;
    _bcm_dpp_gport_resolve_type_e gport_type;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
	unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    SOC_PPC_FRWRD_DECISION_INFO learn_info;
	int learn_gport;
    uint8 split_horizon_filter_enable = FALSE;
    int is_ingress_only = FALSE;
    int dummy_global_lif;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    _bcm_petra_trill_vpn_info_t         trill_vpn;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* check vsi exist */
    rv = _bcm_dpp_vlan_check(unit, vsi);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get vsi info from SW: vsi -> FGL vlans */
    rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
    if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)){
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (rv != BCM_E_NOT_FOUND) {
        /* configure vsi <-> FGL vlans (HW):
         * Configure vsi -> FGL vlans (IVE)
         * Configure FGL vlans -> vsi (VTT) */
        rv = _bcm_dpp_trill_config_vpn(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }    

    /* resolve gort to destination and encap-id*/
    rv = _bcm_dpp_gport_resolve(unit,port,0, &dest_id,&dummy_global_lif,&gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Don't allow addition of Egress only LIFs */
    if ((gport_type == _bcmDppGportResolveTypeAC) && (BCM_GPORT_IS_VLAN_PORT(port))) {
        if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(port) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Doesn't support Egress only LIFs")));
        }
        if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(port) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)) {
            is_ingress_only = TRUE;
        }
    }

    /* Get the local lifs to be used */    
    rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF, 
                                                 &gport_hw_resources);                
    BCMDNX_IF_ERR_EXIT(rv);

    /* update the encap-id */
    BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO),"bcm_petra_vswitch_port_add.lif_info");    
    if (lif_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("error allocating lif info")));
    }
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,gport_hw_resources.local_in_lif,lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(lif_info->type & (SOC_PPC_LIF_ENTRY_TYPE_AC|SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)){
        bcm_vlan_port_t vlan_port;
        rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port, gport_hw_resources.local_in_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (vlan_port.flags & BCM_VLAN_PORT_VSI_BASE_VID) {
            /* No support for this API when VSI base VID */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN port with BCM_VLAN_PORT_VSI_BASE_VID attribute cant be associated to VSI ")));
        }
        lif_info->value.ac.vsid = vsi;
        lif_info->value.ac.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI; 
        lif_info->value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP;
        lif_info->value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;

		if (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) {
            uint32 isid;
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.vsi.vsi_to_isid.get(soc_sand_dev_id, vsi, &isid));
            if (isid) {
                lif_info->value.ac.ing_edit_info.vid = isid & 0xfff;
                lif_info->value.ac.ing_edit_info.vid2 = (isid & 0xfff000) >> 12;
                lif_info->value.ac.lif_profile = 15;
            }
		}
        /* Split horizon is an Egress functionality */
        if ((!is_ingress_only) && (!SOC_IS_JERICHO(unit) || (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) < 2))) {
            if(vlan_port.flags & BCM_VLAN_PORT_NETWORK) {
                lif_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
                /* if egress hairpin is enabled set egress state as well*/
                split_horizon_filter_enable = SOC_DPP_CONFIG(unit)->arad->pp_op_mode.split_horizon_filter_enable;
                if (split_horizon_filter_enable) {
                    soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(soc_sand_dev_id,gport_hw_resources.local_out_lif,SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
             }
            else{
                lif_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
                split_horizon_filter_enable = SOC_DPP_CONFIG(unit)->arad->pp_op_mode.split_horizon_filter_enable;
                if(split_horizon_filter_enable){
                    /* if egress hairpin is enabled set egress state as well*/
                    soc_sand_rv = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(soc_sand_dev_id,gport_hw_resources.local_out_lif,SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
             }
        }

        /* 
         * in Soc_petra-B: there is no capbability to set learning 
         * information for AC-MP it's taken from port-info 
         */

        SOC_PPC_FRWRD_DECISION_INFO_clear(&learn_info);
        
        rv = _bcm_dpp_local_lif_sw_resources_learn_gport_get(unit, gport_hw_resources.local_in_lif, &learn_gport);
        BCMDNX_IF_ERR_EXIT(rv);
        
        rv = _bcm_dpp_gport_to_fwd_decision(unit,learn_gport,&learn_info);
        BCMDNX_IF_ERR_EXIT(rv);
        
        SOC_PPC_FRWRD_DECISION_INFO_clear(&lif_info->value.ac.learn_record.learn_info);
        
        lif_info->value.ac.learn_record.learn_info.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
        lif_info->value.ac.learn_record.learn_info.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
        
        lif_info->value.ac.learn_record.learn_info.dest_id = learn_info.dest_id;
        /* if it's protected port, then update learn.dest_id +1 if port is the protecing port 
        if(lif_info->value.ac.learn_record.learn_info.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC && _BCM_PPD_GPORT_IS_WORKING_PORT_ID(port)) {
            lif_info->value.ac.learn_record.learn_info.dest_id += 1;
        }*/
        /* learn from learning info, if user update system port mapping need to update vlan-port */
        lif_info->value.ac.learn_record.learn_info.type = learn_info.type;
        if(lif_info->value.ac.learn_record.learn_info.type != SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) {
            lif_info->value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
        }
    }
    else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_PWE){
        if(lif_info->value.pwe.vsid != vsi) {
            BCM_EXIT; /*if not nothing to do */
        }
        lif_info->value.pwe.vsid = vsi;
        lif_info->value.pwe.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI; 
        lif_info->value.pwe.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;

        /* vswitch is MP with learning */
        lif_info->value.pwe.learn_record.enable_learning = TRUE;
		rv = _bcm_dpp_local_lif_sw_resources_learn_gport_get(unit, gport_hw_resources.local_in_lif, &learn_gport);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_to_fwd_decision(unit,learn_gport,&lif_info->value.pwe.learn_record.learn_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) {
        lif_info->value.extender.vsid = vsi;
        lif_info->value.extender.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI; 
        lif_info->value.extender.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP;

        /* vswitch is MP with learning */
        lif_info->value.extender.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
		rv = _bcm_dpp_local_lif_sw_resources_learn_gport_get(unit, gport_hw_resources.local_in_lif, &learn_gport);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_to_fwd_decision(unit, learn_gport, &lif_info->value.extender.learn_record.learn_info);
        BCMDNX_IF_ERR_EXIT(rv);
        if (lif_info->value.extender.learn_record.learn_info.type != SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) {
            lif_info->value.extender.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
        }
    }

    soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id,gport_hw_resources.local_in_lif,lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* set map to vsi */
    rv = _bcm_dpp_local_lif_sw_resources_vsi_set(unit, gport_hw_resources.local_in_lif, vsi);
    BCMDNX_IF_ERR_EXIT(rv);


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

/*
 * only detach port from VSI. 
 * port is not destroyed 
 */
int
bcm_petra_vswitch_port_delete(
    int    unit,
    bcm_vlan_t    vsi,
    bcm_gport_t    port)
{
    /* SOC_SAND_PP_DESTINATION_ID dest_id;*/    
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    _bcm_petra_trill_vpn_info_t         trill_vpn;
    int is_last = 0;
    int is_allocated = 0;
    int old_template = 0;
    int new_template = 0;
    bcm_dpp_vlan_flooding_profile_info_t flooding_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Polyphormism delete: call the most most specific delete */
    if(BCM_GPORT_IS_L2GRE_PORT(port)) {
        rv = bcm_petra_l2gre_port_delete(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    soc_sand_dev_id = (unit);

    /* check vsi exist */
    rv = _bcm_dpp_vlan_check(unit, vsi);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_trill_get_vsi_info(unit, vsi, &trill_vpn);
    if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE)){
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (rv != BCM_E_NOT_FOUND) {
        rv = _bcm_dpp_trill_unconfig_vpn(unit, vsi, port);
        BCMDNX_IF_ERR_EXIT(rv);
    }    

    /* resolve gort to destination and encap-id*/    
    rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, 
                                                 &gport_hw_resources);                    
    BCMDNX_IF_ERR_EXIT(rv);

    /*flooding_info init*/
    flooding_info.unknown_uc_add = 0;
    flooding_info.unknown_mc_add = 0;
    flooding_info.bc_add = 0;
    
    rv = _bcm_dpp_am_template_l2_flooding_exchange(unit,gport_hw_resources.local_in_lif,&flooding_info,&old_template,&is_last,&new_template,&is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO),"lif info");    
    if (lif_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("error allocating lif info")));
    }

    /* update the encap-id */
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id,gport_hw_resources.local_in_lif,lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(lif_info->type & (SOC_PPC_LIF_ENTRY_TYPE_AC|SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)){
        /* make it p2p pointing to drop */
        lif_info->value.ac.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF; 
        SOC_PPD_FRWRD_DECISION_DROP_SET(soc_sand_dev_id, &(lif_info->value.ac.default_frwrd.default_forwarding), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        lif_info->value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC;
        lif_info->value.ac.vsid = _BCM_PPD_VSI_P2P_SERVICE;
        lif_info->value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
        /* p2p has to be spoke, yet flag is stored, so when this AC set as MP, actual orientation will be set then */
        lif_info->value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;

    }
    else if(lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_PWE){
        if(lif_info->value.pwe.vsid != vsi) {
            BCM_EXIT; /*if not nothing to do */
        }
        /*1. P2P attributes */
        lif_info->value.pwe.vsid = _BCM_PPD_VSI_P2P_SERVICE;
        lif_info->value.pwe.learn_record.enable_learning = FALSE;
        lif_info->value.pwe.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;
        lif_info->value.pwe.model = SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE;
        /* no protection & use egress-tunnel*/
        lif_info->value.pwe.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
        rv = _bcm_dpp_gport_to_fwd_decision(unit,port,&lif_info->value.pwe.learn_record.learn_info);
        BCMDNX_IF_ERR_EXIT(rv);
        /* consideriny my peer not exist */
        SOC_PPD_FRWRD_DECISION_TRAP_SET(soc_sand_dev_id, &lif_info->value.pwe.default_frwrd.default_forwarding, _BCM_PETRA_UD_DROP_TRAP, 5, 0, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) {
        if(lif_info->value.extender.vsid != vsi) {
            BCM_EXIT;
        }

        /* P2P attributes */
        lif_info->value.extender.vsid = _BCM_PPD_VSI_P2P_SERVICE;
        lif_info->value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
        lif_info->value.extender.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC;

        /* no protection & use egress-tunnel*/
        lif_info->value.extender.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

        /* Set the forwarding to Drop */
        SOC_PPD_FRWRD_DECISION_DROP_SET(soc_sand_dev_id, &(lif_info->value.extender.default_frwrd.default_forwarding), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id,gport_hw_resources.local_in_lif,lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* set map to NULL VSI */
    rv = _bcm_dpp_local_lif_sw_resources_vsi_set(unit, gport_hw_resources.local_in_lif, 0);
    BCMDNX_IF_ERR_EXIT(rv);


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}



int
bcm_petra_vswitch_port_delete_all(
    int    unit,
    bcm_vlan_t    vsi)
{
    int rv = BCM_E_NONE;
    int iter, in_lif_val;
    int port_val;
    bcm_vpn_t vpn;
    int cur_vsi;
    SOC_PPC_LIF_ID local_lif_index_drop; 
    SOC_PPC_LIF_ID local_lif_index_simple;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* check vsi exist */
    if(vsi == 0) {
        /* then OK as vsi 0 means all VSI in this case*/
    }
    else{
        rv = _bcm_dpp_vlan_check(unit, vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    _BCM_DPP_VPN_ID_SET(vpn,  vsi);

    VLAN_ACCESS.vlan_info.local_lif_index_drop.get(unit, &local_lif_index_drop); 
    VLAN_ACCESS.vlan_info.local_lif_index_simple.get(unit, &local_lif_index_simple); 

    for(iter = 0; iter != -1;) {

        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit,vpn,&iter,&port_val,&cur_vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if(port_val == BCM_GPORT_INVALID) {
            break;
        }
        /* gport is not connected to VSI skip it*/
        /*if(cur_vsi == 0 || cur_vsi == -1) {*/
        /*cur_vsi is integer type, it is 0xFFFF but not -1 
              when it equal to BCM_VXLAN_VPN_INVALID */
        if(cur_vsi <= 0 || cur_vsi >= _BCM_GPORT_NOF_VSIS) {
            continue;
        }

        /* Don't delete default In-LIF */
        in_lif_val = iter - 1;
        if ((in_lif_val == local_lif_index_drop) ||
		    (in_lif_val == local_lif_index_simple)) {
            continue;
        }

        /* delete next port */
        rv = bcm_petra_vswitch_port_delete(unit,cur_vsi,port_val);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_vswitch_port_get(
    int    unit,
    bcm_gport_t    port,
    bcm_vlan_t *    vsi)
{    
    int rv = BCM_E_NONE;
    int vsi_val;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(vsi);

    *vsi = 0;
    /* check vsi exist */
    
    /* resolve gort to LIF */
    rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, 
                                                 &gport_hw_resources);                    
    BCMDNX_IF_ERR_EXIT(rv);

    /* from LIF to VSI */
    rv= _bcm_dpp_local_lif_to_vsi(unit,gport_hw_resources.local_in_lif,&vsi_val, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    *vsi = (bcm_vlan_t)vsi_val;

exit:
    BCMDNX_FUNC_RETURN;
}

/* internal functions common for vswitching */
int _bcm_dpp_vlan_check(int unit, int vsi)
{
    int exist=0;
    BCMDNX_INIT_FUNC_DEFS;

    /* if regular vlan (<=4095) no need to check allocation */
    if(BCM_VLAN_VALID(vsi))
    {
        BCM_EXIT;
    }
    /* check if vsi/vpn allocated */
    exist = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit,vsi);
    if(exist != BCM_E_EXISTS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VSI %d is not allocated"),vsi));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get the LIF entry and (if needed) verify that the port is not cross-connected already */ 
int 
_bcm_dpp_vswitch_cross_connect_get_entry(
    int unit,
    bcm_gport_t gport, 
    SOC_PPC_LIF_ID *local_lif_index,
    SOC_PPC_LIF_ENTRY_INFO *lif_info, 
    int verify)
{
    int rv, soc_sand_dev_id;
    uint32 soc_sand_rv;    
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    /* Translate parameters to SOC_SAND parameters */
    soc_sand_dev_id = (unit);

    /* resolve lif from gport */
    rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, 
                                                 &gport_hw_resources);                    
    BCMDNX_IF_ERR_EXIT(rv);
    *local_lif_index = gport_hw_resources.local_in_lif;

    /* get the lif entry */
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, *local_lif_index, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (lif_info->type == SOC_PPC_LIF_ENTRY_TYPE_EMPTY) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF is not found , type if empty")));
    }

    if (verify) {
        /* verify that the port is not cross-connected already */
        if (_bcm_petra_is_cross_connect_lif(unit, *local_lif_index)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("LIF is already cross-connected")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_vswitch_cross_connect_apply(
    int unit,
    /* data of the primary port */
    SOC_PPC_LIF_ID local_lif_index,
    SOC_PPC_LIF_ENTRY_INFO *lif_info,
    bcm_gport_t this_gport,
    /* data of the second port */
    bcm_gport_t gport,
    int         other_encap) 
{
    int rv;
    uint32 soc_sand_rv;
    int core;
    bcm_port_t port_ndx;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_PORT_INFO port_info;
    uint8 other_encap_type = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* if other encap is valid, then extract reomte gport type from the encap-id */
    if (other_encap != BCM_FORWARD_ENCAP_ID_INVALID) {
        if (BCM_FORWARD_ENCAP_ID_IS_EEI(other_encap)) {
            /* In this case although type is EEI we use the OUTLIF_USAGE to mark that
               this is MPLS Port information */
            other_encap_type = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT;
        }
        else {
            other_encap_type = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GET(other_encap);
        }
    }
    
    if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_PWE) { /* mpls_port */

        /* update PWE info (dest) */
        lif_info->value.pwe.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;
        lif_info->value.pwe.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

        /* resolve frwrd decision from port 2 */
        rv = _bcm_dpp_port_encap_to_fwd_decision(unit, gport, other_encap,&(lif_info->value.pwe.default_frwrd.default_forwarding));
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_to_phy_port(unit, this_gport, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

            soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (BCM_GPORT_IS_MIM_PORT(gport) || other_encap_type == BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID) {

                /* set LABEL_PWEP2P_VSI_PROFILE to vsi_profile=1
                   which points at VSI-base 0xffff for Mac-in-Mac-P2P-VSI */
                port_info.label_pwe_p2p_vsi_profile = 1;
            }
            else {
                /* set LABEL_PWEP2P_VSI_PROFILE to vsi_profile=0 (default) */
                port_info.label_pwe_p2p_vsi_profile = 0;
            }
            port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_VSI_PROFILE_TBL;
            soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) { /* mcast port*/
        /* update MPLS tunnel term info (dest) */
        lif_info->value.mpls_term_info.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;
        lif_info->value.mpls_term_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

        /* resolve frwrd decision from port 2 */
        rv = _bcm_dpp_port_encap_to_fwd_decision(unit, gport, other_encap, &(lif_info->value.mpls_term_info.default_frwrd.default_forwarding));
        BCMDNX_IF_ERR_EXIT(rv);

    }
    else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_ISID) { /* mim_port */

        /* update ISID info (dest) */
        lif_info->value.isid.service_type = SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P;
        lif_info->value.isid.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

        /* resolve frwrd decision from port 2 */
        rv = _bcm_dpp_port_encap_to_fwd_decision(unit, gport, other_encap,&(lif_info->value.isid.default_frwrd.default_forwarding));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* vlan_port */

        /* update AC info (dest) */
        if (BCM_GPORT_IS_MIM_PORT(gport) || other_encap_type == BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID) {
            lif_info->value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB;
            /* Outer VLAN translation will be deleted */
            if (lif_info->value.ac.ing_edit_info.vid2 != 0) {
                LOG_WARN(BSL_LS_BCM_SWITCH,
                         (BSL_META_U(unit,
                                     " Inner VLAN translation will be deleted.")));
            }

            /* in this case also vsid should point at mim_vsi */
            lif_info->value.ac.vsid = SOC_DPP_CONFIG(unit)->arad->pp_op_mode.p2p_info.mim_vsi;

            rv = _bcm_dpp_gport_to_phy_port(unit, this_gport, 0, &gport_info); 
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
                  BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_ndx, &soc_ppd_port, &core)));

                  /* set AC_P2P_TO_PBB_VSI_PROFILE to vsi_profile=1
                     which points at VSI-base 0xffff for Mac-in-Mac-P2P-VSI */
                  soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                  /* vsi_profile might already be 1 - no need to change the profile back to 0 when removing the cross-connect
                     because it is a profile specific for P2P_to_PBB */
                  if (port_info.ac_p2p_to_pbb_vsi_profile != 1) {
                      port_info.ac_p2p_to_pbb_vsi_profile = 1;
                          port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_VSI_PROFILE_TBL;
                      soc_sand_rv = soc_ppd_port_info_set(unit, core, soc_ppd_port, &port_info);
                      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                  }
            }
        }
        else if (BCM_GPORT_IS_MPLS_PORT(gport) || other_encap_type == BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT) {
            lif_info->value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE;
            /* Outer VLAN translation will be deleted */
            if (lif_info->value.ac.ing_edit_info.vid2 != 0) {
                LOG_WARN(BSL_LS_BCM_SWITCH,
                         (BSL_META_U(unit,
                                     " Inner VLAN translation will be deleted.")));
            }

        }
        else { 
            lif_info->value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC;
        }
        lif_info->value.ac.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

        /* resolve frwrd decision from port 2 */
        rv = _bcm_dpp_port_encap_to_fwd_decision(unit, gport, other_encap,&(lif_info->value.ac.default_frwrd.default_forwarding));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* update the lif entry with the new frwrd decision */
    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, local_lif_index, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* update the peer_gport_id of the cross-connected port */
    rv = _bcm_dpp_in_lif_p2p_match_set(unit, local_lif_index, TRUE, gport);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_dpp_vswitch_cross_connect_reverse(
    int unit,
    SOC_PPC_LIF_ID lif_index,
    SOC_PPC_LIF_ENTRY_INFO *lif_info) 
{
    int soc_sand_dev_id, rv;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    /* Translate parameters to SOC_SAND parameters */
    soc_sand_dev_id = (unit);

    if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_PWE) { /* mpls_port */

        /* update PWE info (dest) */
        lif_info->value.pwe.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
        lif_info->value.pwe.default_frwrd.default_forwarding.dest_id = 0;
        lif_info->value.pwe.default_frwrd.default_forwarding.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
        lif_info->value.pwe.default_frwrd.default_forwarding.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
        lif_info->value.pwe.default_frwrd.default_forwarding.additional_info.outlif.val = 0;
    }
    else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) { /* mcast port*/
        /* update MPLS tunnel term info (dest) */
        lif_info->value.mpls_term_info.service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP;
        lif_info->value.mpls_term_info.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
        lif_info->value.mpls_term_info.default_frwrd.default_forwarding.dest_id = 0;
        lif_info->value.mpls_term_info.default_frwrd.default_forwarding.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
        lif_info->value.mpls_term_info.default_frwrd.default_forwarding.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
        lif_info->value.mpls_term_info.default_frwrd.default_forwarding.additional_info.outlif.val = 0;
    }
    else if (lif_info->type & SOC_PPC_LIF_ENTRY_TYPE_ISID) { /* mim_port */

        /* update ISID info (dest) */
        lif_info->value.isid.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
        lif_info->value.isid.default_frwrd.default_forwarding.dest_id = 0;
        lif_info->value.isid.default_frwrd.default_forwarding.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
        lif_info->value.isid.default_frwrd.default_forwarding.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
        lif_info->value.isid.default_frwrd.default_forwarding.additional_info.outlif.val = 0;
    }
    else { /* vlan_port */

        /* update AC info (dest) */
        lif_info->value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC;
        lif_info->value.ac.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
        lif_info->value.ac.default_frwrd.default_forwarding.dest_id = 0;
        lif_info->value.ac.default_frwrd.default_forwarding.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
        lif_info->value.ac.default_frwrd.default_forwarding.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
        lif_info->value.ac.default_frwrd.default_forwarding.additional_info.outlif.val = 0;
    }

    /* update the lif entry with the new frwrd decision */
    soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id, lif_index, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* update the peer_gport_id of the cross-connected port */
    rv = _bcm_dpp_in_lif_p2p_match_set(unit, lif_index, FALSE, 0);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Attach given 2 ports in P2P service. */
int bcm_petra_vswitch_cross_connect_add(
    int unit, 
    bcm_vswitch_cross_connect_t *gports)
{
    int rv, is_local;
    SOC_PPC_LIF_ID local_lif_index1, local_lif_index2;
    SOC_PPC_LIF_ENTRY_INFO *lif_info1 = NULL, *lif_info2 = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if ((gports->encap1 == 0) || (gports->encap2 == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id 0 is not supported, use BCM_FORWARD_ENCAP_ID_INVALID")));
    }

    /* check port type has to be l2vpn port (which lead to inLIF)*/
    if (!(BCM_GPORT_IS_VLAN_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_MPLS_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_MIM_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_TUNNEL(gports->port1))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port1's type cannot be cross-connected")));
    }
    /* check port2 type if bidirectional has to be 
       - l2vpn port 
       if uni-directional:
       - l2vpn-port
       - phy-port 
       - phy-port + encap_id 
    */
    if (!(gports->flags & BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL)) {
        if (!(BCM_GPORT_IS_VLAN_PORT(gports->port2)) &&
            !(BCM_GPORT_IS_MPLS_PORT(gports->port2)) &&
            !(BCM_GPORT_IS_MIM_PORT(gports->port2)) &&
            !(BCM_GPORT_IS_MCAST(gports->port2)) &&
            !(BCM_GPORT_IS_FORWARD_PORT(gports->port2))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port2's type cannot be cross-connected")));
        }
    }

    /* gport1 --> gport 2*/
    BCMDNX_ALLOC(lif_info1, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_vswitch_cross_connect_add.lif_info1");
    if (!lif_info1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    /* Verify that the first gport is local. Otherwise, there's no need to access the HW */
    rv = _bcm_dpp_gport_is_local(unit, gports->port1, &is_local);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_local) {
        /* Get the gports lif_info and verify they are not cross-connected already */
        rv = _bcm_dpp_vswitch_cross_connect_get_entry(unit, gports->port1, &local_lif_index1, lif_info1, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);

        /*Check the connection between port1 and port2*/
        if (BCM_GPORT_IS_TUNNEL(gports->port1)) {
            if ((!BCM_GPORT_IS_MCAST(gports->port2) && !BCM_GPORT_IS_FORWARD_PORT(gports->port2)) ||
                !(gports->flags & BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL) ||
                !(lif_info1->type & SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG(
                    "Port1 and Port2 cann't be cross_connected")));
            }
        }

        rv = _bcm_dpp_vswitch_cross_connect_apply(unit, local_lif_index1, lif_info1, gports->port1, gports->port2, gports->encap2);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* gport2 --> gport 1 */
    if (!(gports->flags & BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL)) {
        BCMDNX_ALLOC(lif_info2, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_vswitch_cross_connect_add.lif_info2");
        if (!lif_info2) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }

        /* Verify that the second gport is local. Otherwise, there's no need to access the HW */
        rv = _bcm_dpp_gport_is_local(unit, gports->port2, &is_local);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((is_local) || (!(BCM_GPORT_IS_VLAN_PORT(gports->port2)))) {
            /* Get the gports lif_info and verify they are not cross-connected already */
            rv = _bcm_dpp_vswitch_cross_connect_get_entry(unit, gports->port2, &local_lif_index2, lif_info2, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);


            rv = _bcm_dpp_vswitch_cross_connect_apply(unit, local_lif_index2, lif_info2, gports->port2, gports->port1, gports->encap1);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCM_FREE(lif_info1);
    BCM_FREE(lif_info2);
    BCMDNX_FUNC_RETURN;
}

/* Remove attachment between the ports. */
int bcm_petra_vswitch_cross_connect_delete(
    int unit, 
    bcm_vswitch_cross_connect_t *gports)
{
    int rv;
    SOC_PPC_LIF_ID local_lif_index1, local_lif_index2;
    SOC_PPC_LIF_ENTRY_INFO *lif_info1 = NULL, *lif_info2 = NULL;
    int port1_connected, port2_connected;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* check port type */
    if (!(BCM_GPORT_IS_VLAN_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_MPLS_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_MIM_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_TUNNEL(gports->port1))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port1's type cannot be cross-connected")));
    }

    BCMDNX_ALLOC(lif_info1, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_vswitch_cross_connect_add.lif_info1");
    if (!lif_info1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    /* Get the gports lif_info */
    rv = _bcm_dpp_vswitch_cross_connect_get_entry(unit, gports->port1, &local_lif_index1, lif_info1, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_ALLOC(lif_info2, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_vswitch_cross_connect_add.lif_info2");
    if (!lif_info2) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    if (!(gports->flags & BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL)) {
        rv = _bcm_dpp_vswitch_cross_connect_get_entry(unit, gports->port2, &local_lif_index2, lif_info2, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);
        port2_connected = _bcm_petra_is_cross_connect_lif(unit, local_lif_index2);
    }
    else {
        port2_connected = 0;
    }

    port1_connected = _bcm_petra_is_cross_connect_lif(unit, local_lif_index1);

    /* When using 1+1 protection, a port might be cross connected only on one side. In this case, the cross connection can be deleted.
       If both are not connected, return error */
    if (!(port1_connected) && !(port2_connected)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Ports are not cross connected")));
    }



    if (port1_connected){
        rv = _bcm_dpp_vswitch_cross_connect_reverse(unit, local_lif_index1, lif_info1);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (port2_connected){
        rv = _bcm_dpp_vswitch_cross_connect_reverse(unit, local_lif_index2, lif_info2);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCM_FREE(lif_info1);
    BCM_FREE(lif_info2);
    BCMDNX_FUNC_RETURN;
}

/* call back used internally by traverse API to delet all cross connect ports*/
STATIC
int bcm_petra_vswitch_cross_connect_delete_cb(
    int unit, 
    bcm_vswitch_cross_connect_t *gports,
    void *dummy)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_vswitch_cross_connect_delete(unit,gports));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Return peer, if protected return primary port, invalid gport is
 * populated.
 */
int bcm_petra_vswitch_cross_connect_get(
    int unit, 
    bcm_vswitch_cross_connect_t *gports)
{
    int rv;
    int is_p2p;
    SOC_PPC_LIF_ID lif_index;
    SOC_PPC_LIF_ENTRY_INFO lif_info;
    SOC_PPC_FRWRD_DECISION_INFO *frwrd_decision=NULL;
    uint8 encap_usage=BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL;
    _bcm_lif_type_e in_lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);    

    /* check port type */
    if (!(BCM_GPORT_IS_VLAN_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_MPLS_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_MIM_PORT(gports->port1)) &&
        !(BCM_GPORT_IS_TUNNEL(gports->port1))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port1's type cannot be cross-connected")));
    }

    /* Get the gport lif_info */
    rv = _bcm_dpp_vswitch_cross_connect_get_entry(unit, gports->port1, &lif_index, &lif_info, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check if this lif is valid */
    rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, (int)lif_index, -1, &in_lif_usage, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    if (in_lif_usage == _bcmDppLifTypeAny) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port is invalid")));
    }

    /* Get p2p information including whether this is actually a p2p In-LIF */ 
    rv = _bcm_dpp_in_lif_p2p_match_get(unit, lif_index, &is_p2p, &(gports->port2));
    BCMDNX_IF_ERR_EXIT(rv);

    /* In case the given In-LIF isn't p2p */
    if (!is_p2p) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Port is not cross-connected")));
    }

    /* get peer_gport_id of the cross-connected port */
    if (SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(unit)) {

        /* if new mode, return encap + port */
        _bcm_petra_lif_get_frwrd_decision(&lif_info,&frwrd_decision);
        if (frwrd_decision == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Port2 is not retrieved successfully")));
        }

        /* If the inlif's type is AC, we can deduce the outlif's type from it.
         *  Otherwise, we can't deduce the outlif's type.
         */
        if (lif_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            switch (lif_info.value.ac.service_type) {
            case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC:
                encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT;
                break;
            case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB:
                encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_ISID;
                break;
            case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE:
                encap_usage = BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_MPLS_PORT;
                break;
            default:
                /* We shouldn't get here */
                break;
            }
        }

        rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&gports->port2,&gports->encap2,frwrd_decision,encap_usage,0/*force_destination*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Traverse existing P2P services. */
int bcm_petra_vswitch_cross_connect_traverse(
    int unit, 
    bcm_vswitch_cross_connect_traverse_cb cb, 
    void *user_data)
{
    int cur_lif = 0;
    int vsi = 0;
    int gport_id = 0;
    int rv = BCM_E_NONE;
    bcm_vswitch_cross_connect_t cross_ports;

    BCMDNX_INIT_FUNC_DEFS;

    for(; cur_lif < _BCM_GPORT_NOF_LIFS; ++cur_lif) {

        /* is it cross-connected? */
        if(_bcm_petra_is_cross_connect_lif(unit, cur_lif)){
            rv = _bcm_dpp_local_lif_to_vsi(unit,cur_lif,&vsi,&gport_id);
            BCMDNX_IF_ERR_EXIT(rv);

            if (vsi != -1) {
                /* get the port it is cross-connected to */
                /* init struct */
                bcm_vswitch_cross_connect_t_init(&cross_ports);
                /* set flag to directional */
                cross_ports.flags |= BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
                cross_ports.port1 = gport_id;
                rv = bcm_petra_vswitch_cross_connect_get(unit,&cross_ports);
                BCMDNX_IF_ERR_EXIT(rv);

                if (cross_ports.port2 != 0) {
                    rv = cb(unit, &cross_ports, user_data);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}



/* Delete all P2P services. */
int bcm_petra_vswitch_cross_connect_delete_all(
    int unit)
{
    int dummy;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_vswitch_cross_connect_traverse(unit,bcm_petra_vswitch_cross_connect_delete_cb, (void*)&dummy));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Traverse existing ports on vswitch */
int bcm_petra_vswitch_port_traverse(
    int unit, 
    bcm_vlan_t vsi, 
    bcm_vswitch_port_traverse_cb cb, 
    void *user_data)
{
    int rv = BCM_E_NONE;
    int iter, in_lif_val;
    int port_val;
    bcm_vpn_t vpn;
    int cur_vsi;
    SOC_PPC_LIF_ID local_lif_index_drop; 
    SOC_PPC_LIF_ID local_lif_index_simple;

    BCMDNX_INIT_FUNC_DEFS;

    /* check vsi exist */
    rv = _bcm_dpp_vlan_check(unit, vsi);
    BCMDNX_IF_ERR_EXIT(rv);

    _BCM_DPP_VPN_ID_SET(vpn, vsi);

    VLAN_ACCESS.vlan_info.local_lif_index_drop.get(unit, &local_lif_index_drop); 
    VLAN_ACCESS.vlan_info.local_lif_index_simple.get(unit, &local_lif_index_simple); 

    for(iter = 0; iter != -1;) {
        /* get next port */
        rv = _bcm_dpp_vpn_get_next_gport(unit,vpn,&iter,&port_val,&cur_vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* no more ports in vpn */
        if(port_val == BCM_GPORT_INVALID) {
            break;
        }

        /* Skip default In-LIF */
        in_lif_val = iter - 1;
        if ((in_lif_val == local_lif_index_drop) ||
		    (in_lif_val == local_lif_index_simple)) {
            continue;
        }

        /* call for callback */
        rv = cb(unit,vsi,port_val,user_data);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_vswitch_destroy_all(
    int unit)
{
    int rv = BCM_E_NONE;
    int vsi;
    _bcm_vsi_type_e vsi_usage_type;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Delete all the ports from all vswitch in order to avoid LIFs traverse
       for each VSI with in bcm_petra_vswitch_destroy_internal */
    rv = bcm_petra_vswitch_port_delete_all(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Destroy vswitch VSIs */
    for (vsi = 0; vsi < _BCM_GPORT_NOF_VSIS; vsi++) {
        /* Get the vsi usage type */
        rv = _bcm_dpp_vswitch_vsi_usage_get(unit, vsi, &vsi_usage_type);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Destroy the VSI if it's a vswitch */
        if (vsi_usage_type & _bcmDppVsiTypeVswitch) {
            rv = bcm_petra_vswitch_destroy_internal(unit, vsi, _bcmDppVsiTypeVswitch, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Verify port_match, port_fwd structs of flexible cross connect api */
int
_bcm_vswitch_flexible_connect_verify( 
    int unit, 
    uint32 flags, 
    bcm_vswitch_flexible_connect_match_t *port_match,
    bcm_vswitch_flexible_connect_fwd_t *port_fwd)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!soc_property_get(unit, spn_VPWS_TAGGED_MODE, 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vswitch_flexible_connect is supported only when vpws_tagged_mode soc property is set")));
    }

    if ((port_match->match != BCM_PORT_MATCH_PORT_VLAN_STACKED) &&
        (port_match->match != BCM_PORT_MATCH_PORT_VLAN)) {

        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently api supports only BCM_PORT_MATCH_PORT_VLAN, BCM_PORT_MATCH_PORT_VLAN_STACKED match")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Translate bcm_vswitch_flexible_connect_match_t info to SOC_PPC_FRWRD_MATCH_INFO */
int
_bcm_vswitch_flexible_connect_fill_soc_match_info(
   int unit, 
   bcm_vswitch_flexible_connect_match_t *port_match, 
   SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info)
{
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* resolve match info from port_match */
    rv = _bcm_dpp_gport_to_hw_resources(unit, port_match->match_port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    fwd_match_info->in_lif = gport_hw_resources.global_in_lif;
    fwd_match_info->nof_tags = (port_match->match == BCM_PORT_MATCH_PORT_VLAN) ? 1 : 2;
    fwd_match_info->outer_tag = port_match->match_vlan;
    fwd_match_info->inner_tag = port_match->match_inner_vlan;
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Add flexible cross connect connection
 */
int
bcm_petra_vswitch_flexible_connect_add( 
    int unit, 
    uint32 flags, 
    bcm_vswitch_flexible_connect_match_t *port_match,
    bcm_vswitch_flexible_connect_fwd_t *port_fwd)
{
    int rv = BCM_E_NONE;
    SOC_PPC_FRWRD_DECISION_INFO frwrd_info;
    SOC_PPC_FRWRD_MATCH_INFO fwd_match_info;

    bcm_vswitch_flexible_connect_fwd_t port_fwd_dummy;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    SOC_PPC_FRWRD_MATCH_INFO_clear(&fwd_match_info);

    rv = _bcm_vswitch_flexible_connect_verify(unit, flags, port_match, port_fwd);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check update flag */
    rv = bcm_petra_vswitch_flexible_connect_get(unit, 0, port_match, &port_fwd_dummy);
    if (flags & BCM_VSWITCH_FLEXIBLE_CONNECT_UPDATE) {
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Existing port_match not found")));
        }
        else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else {
        if (rv == BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("port_match already exists")));
        }
        else if (rv != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* resolve frwrd decision from port_fwd */
    rv = _bcm_dpp_port_encap_to_fwd_decision(unit, port_fwd->forward_port, port_fwd->forward_encap, &frwrd_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* fill fwd_match_info */
    rv = _bcm_vswitch_flexible_connect_fill_soc_match_info(unit, port_match, &fwd_match_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_extend_p2p_lem_entry_add,(unit, &fwd_match_info, &frwrd_info)));

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Get flexible cross connect connection
 */
int
bcm_petra_vswitch_flexible_connect_get( 
    int unit, 
    uint32 flags, 
    bcm_vswitch_flexible_connect_match_t *port_match,
    bcm_vswitch_flexible_connect_fwd_t *port_fwd)
{
    int rv = BCM_E_NONE;
    SOC_PPC_FRWRD_DECISION_INFO frwrd_info;
    SOC_PPC_FRWRD_MATCH_INFO fwd_match_info;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_vswitch_flexible_connect_verify(unit, flags, port_match, port_fwd);
    BCMDNX_IF_ERR_EXIT(rv);

    /* fill fwd_match_info */
    rv = _bcm_vswitch_flexible_connect_fill_soc_match_info(unit, port_match, &fwd_match_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_extend_p2p_lem_entry_get,(unit, &fwd_match_info, &frwrd_info, &found)));

    if (!found) {
        BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
    }

    /* get port_fwd from frwrd decision */
    rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit, &port_fwd->forward_port, &port_fwd->forward_encap, &frwrd_info, 
                                                    BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL, 0 /*force_destination*/);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Delete flexible cross connect connection
 */
int
bcm_petra_vswitch_flexible_connect_delete( 
    int unit, 
    bcm_vswitch_flexible_connect_match_t *port_match)
{
    int rv = BCM_E_NONE;
    SOC_PPC_FRWRD_MATCH_INFO fwd_match_info;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* fill fwd_match_info */
    rv = _bcm_vswitch_flexible_connect_fill_soc_match_info(unit, port_match, &fwd_match_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_extend_p2p_lem_entry_remove,(unit, &fwd_match_info)));

exit:
    BCMDNX_FUNC_RETURN;
}


